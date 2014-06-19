/*******************************************************************************
 Copyright (c) 2013, Hangzhou LEPEI Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                             mp_cfg.c
  Project Code: lware
   Module Name: 
  Date Created: 2013-7-15
        Author: wanghuanyu 
   Description: 

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD  

*******************************************************************************/

#include "master.h"
#include "sys_vlan.h"
#include "timer.h"

#include <str_utils.h>
#include <vlan_utils.h>
#include <pdt_config.h>
#include <lw_type.h>
#include <lw_drv_pub.h>
#include <lw_config_oid.h>
#include <lw_config_api.h>

#include <lw_drv_pub.h>
#include <lw_drv_req.h>

#include <ipc_protocol.h>


#define vlan_exist(vh, vid) (vh[vid].id.value > 0 )

#ifdef DEBUG
#define DBG_PRINTF(fmt, ...) do { fprintf(stderr, "[%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\n"); } while(0)
#define DBG_ASSERT(bool, fmt, ...) do { if(!(bool)) { fprintf(stderr, "\nFAIL [%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\r\n"); } } while(0)

#else
#define DBG_PRINTF(fmt, ...)
#define DBG_ASSERT(fmt, ...)
#endif
extern int master_cfg_setval(int ifindex,unsigned int oid,void *val);


struct vlan_list{
	switch_vlan_t vlan;
	struct vlan_list *next;
};

typedef struct vlan_list switch_vlan_list_t;


typedef struct {
	uint8_t vlanmode;
	uint16_t count;
	switch_vlan_interface_t *vlanifs[MAX_PORT_NUM+1];
}
clt_vlanif_entry_t;

typedef struct {
	// for most cases, vlan mode works together with ports
//	uint8_t vlan_mode; // Transparent or 802.1Q
	uint16_t count;
	switch_vlan_group_t *groups[MAX_VLAN_GROUPS];
	switch_vlan_list_t *lists[MAX_VLAN_GROUPS];	//  for temporary data use
}
clt_vlan_entry_t;

static clt_vlanif_entry_t clt_vlanif_entry;
static clt_vlan_entry_t clt_vlan_entry;
static int vlan_debug = 0;
static int vcfg_debug = 0;
switch_interface_t clt_interfaces[MAX_PORT_NUM+1];
static void clt_vlan_group_dump(switch_vlan_group_t *vlan_groups, uint16_t count);
static int clt_vlan_entry_set(switch_vlan_group_t *vlan_groups, UINT16 count, UINT8 flag);
static int clt_vlan_entry_refresh(switch_vlan_t *vlans, int count);
static void clt_vlan_entry_update(void);
static void vcfg_vlan_config_dump(vlan_apply_t *vp);
static void clt_vlan_entry_dump(void);
extern char *switchport_itoa(int mode);

typedef uint32_t vlan_member_t;

/* 
  static vlan list functions 
*/
static void vlan_list_free(struct vlan_list *head)
{
	struct vlan_list *next;	
	while(head != NULL){
		next = head->next;
		free(head);
		head = next; 
	}
}


static struct vlan_list * vlan_list_new(uint16_t id, logic_pmask_t member)
{
	struct vlan_list *vl = NULL;
	vl = (struct vlan_list *)malloc(sizeof(struct vlan_list));
	if (vl == NULL){
		DBG_ASSERT((vl != NULL), "Failed to alloc memory for vlan list");
		return NULL;
	}
	vl->vlan.id.value = id;
	vl->vlan.id.flag = 0;
//	vl->vlan.member = member;
	LgcMaskCopy(&(vl->vlan.member),&(member));
	vl->next = NULL;
	return vl;
}


static struct vlan_list * vlan_list_add(struct vlan_list **head, switch_vlan_t *vlan)
{
	struct vlan_list *vl = NULL;
	if (*head == NULL){
		vl = vlan_list_new(vlan->id.value, vlan->member);		
		*head = vl;	
		return vl;
	}else {
		vl = *head;
		while(vl->next != NULL){
			vl = vl->next;
		}
		vl->next = vlan_list_new(vlan->id.value, vlan->member);
		return vl->next;
	}
	return vl;
}

static switch_vlan_t * clt_vlan_entry_vlans(int *vlan_count)
{
	int i, count = 0;
	static switch_vlan_t vlans[MAX_VLAN_NUMS * 2 + 1];
	clt_vlan_entry_t *entry = &clt_vlan_entry;
	
	for (i = 0; (i < entry->count) && (count < MAX_VLAN_NUMS * 2); i ++)
	{
		if (entry->groups[i]){
			entry->groups[i]->flag = VLAN_FLAG_NONE;
			count += get_group_vlans(entry->groups[i], &vlans[count], (MAX_VLAN_NUMS * 2) - count);
		}
	}	
	// the last one is 0 to mark the end of vlans
	vlans[count].id.value = 0;
	vlans[count].id.flag = 0;
	if (vlan_count) *vlan_count = count;
	
	return vlans;
}

static void clt_vlan_entry_free_groups(void)
{
	int i;
	clt_vlan_entry_t *entry = &clt_vlan_entry;
	for (i = 0; i < MAX_VLAN_GROUPS; i ++)
	{
		if (entry->groups[i]){
			free(entry->groups[i]);
			entry->groups[i] = NULL;
		}
	}
}
static void clt_vlan_entry_free_lists(void)
{
	int i;
	clt_vlan_entry_t *entry = &clt_vlan_entry;
	for (i = 0; i < MAX_VLAN_GROUPS; i ++)
	{
		if (entry->lists[i]){
			vlan_list_free(entry->lists[i]);
			entry->lists[i] = NULL;
		}
	}
}



static int clt_vlan_entry_add(switch_vlan_t *vlan)
{
	int i;
	clt_vlan_entry_t *entry = &clt_vlan_entry;
	
	// find the list with same member
	//DBG_PRINTF("vlan entry add %d[%X]", vlan->id.value, vlan->member);
	for (i = 0; i < MAX_VLAN_GROUPS; i ++){
		if (entry->lists[i] == NULL) break;
		if ((entry->lists[i] != NULL) 
			&& ((memcmp(&(entry->lists[i]->vlan.member),&(vlan->member),sizeof(vlan->member)))==0)){
			vlan_list_add(&entry->lists[i], vlan);
			return (vlan->id.flag > 0) ? 1 : 0;
		}
	}
	if (i < MAX_VLAN_GROUPS){
		vlan_list_add(&entry->lists[i], vlan);
		return (vlan->id.flag > 0) ? 1 : 0;
	}
	return 0;
}


static int vlan_list_count(struct vlan_list *head)
{
	int count = 0;
	uint16_t last_id = 0;
	switch_vlan_list_t *v, *continue_start, *last;
	// get lists count
	v = head;
	last = NULL;
	
	if (v != NULL){
		last_id = v->vlan.id.value;
		last = v;
		v = v->next;
	}	
	count = 0;
	continue_start = NULL;	
	while(v != NULL){
		if (v->vlan.id.value == last_id + 1){
			if (continue_start == NULL){
				continue_start = last;
			}
		}else {
		// save the last one
			if (continue_start != NULL){
				// from continue_star to last one
				count ++;
				continue_start = NULL;
			}else {
				// the last one;
				count ++;
			}
		}		
		last_id = v->vlan.id.value;
		last = v;
		v = v->next;
	}
	// don't forget the last one or range
	if (last != NULL){
		count ++;
	}
	return count;
}

//#define MAX_PORT_NUM 5
static int clt_vlan_interface_set_default(switch_vlan_mode_t *vm);
static int clt_vlan_compare(const void *v1, const void *v2)
{
	int id1, id2;
	id1 = ((switch_vlan_t *)v1)->id.value;
	id2 = ((switch_vlan_t *)v2)->id.value;
	return (id1 > id2);
}

int switch_vlan_mode_set(switch_vlan_mode_t *vm)
{
	clt_vlanif_entry_t * entry = &clt_vlanif_entry;
	int ret=0;

	if (vm->mode != entry->vlanmode){
		
		ret= clt_vlan_interface_set_default(vm);		
		return ret;
	}
	return 0;
}
static void clt_vlans_dump(switch_vlan_t *vlans)
{
	int i = 0;
	if (!vlan_debug) return; 
	
	fprintf(stderr, "VLANS\n\t");
	while(vlans[i].id.value > 0){
		fprintf(stderr, "%d[%X]%02X ", vlans[i].id.value, vlans[i].member.pbits[0], vlans[i].id.flag/* ? "*" : ""*/);
		if (i && !(i & 0x07)) fprintf(stderr, "\n\t");
		i ++;
	}
	fprintf(stderr, "\nDump %d VLANs\n", i);
}


void clt_vlanif_dump(switch_vlan_interface_t *vlanif, int index)
{
	if (!vlan_debug) return;
	
	if (vlanif){
		fprintf(stderr, "VlanIF<%d>\n", index);
		fprintf(stderr, "\tName:%s\n", vlanif->ifname);
		fprintf(stderr, "\tPHY :%d\n", vlanif->phy);
		fprintf(stderr, "\tPVID:%d\n", vlanif->pvid);
		fprintf(stderr, "\tMode:%s\n", switchport_itoa(vlanif->mode));
		clt_vlan_group_dump(vlanif->trunk_hybrid, vlanif->count);			
	}else {
		fprintf(stderr, "entry<%d> empty\n", index);
	}
}
static switch_vlan_t *clt_vlan_interface_vlans(switch_vlan_interface_t *vi, int *vlan_count)
{
	int i, count = 0;
	static switch_vlan_t vlans[MAX_VLAN_NUMS * 2 + 1];
	switch_vlan_group_t *vg;

	vg = vi->trunk_hybrid;
	for (i = 0; (i < vi->count) && (count < MAX_VLAN_NUMS * 2); i ++)
	{
		if (vlan_tagged(vg->flag)) vg->flag = VLAN_FLAG_TAG;
		count += get_group_vlans(vg, &vlans[count], (MAX_VLAN_NUMS * 2) - count);
		vg = vlan_group_to_next(vg);//added by w03828
	}	
	// the last one is 0 to mark the end of vlans
	vlans[count].id.value = 0;
	vlans[count].id.flag = 0;
	if (vlan_count) *vlan_count = count;
	
	return vlans;
}
static void clt_vlan_interface_refresh(switch_vlan_t *vlans, int count,
	switch_vlan_list_t **taglist, switch_vlan_list_t **untaglist, int mode)
{

	int v, same_pending, update = 0;
	uint16_t last_id;
	switch_vlan_t *newvlan = NULL;
	switch_vlan_t *lastvlan = NULL;

	switch_vlan_list_t *ltag = NULL, *luntag = NULL;


#define add_to_tagged(mode, flag) 	((mode == LINK_TYPE_TRUNK) ? 1 : flag)

	// sort ids from small to large
	qsort((void *)vlans, count, sizeof(switch_vlan_t), clt_vlan_compare);

	clt_vlans_dump(vlans);

	same_pending = 0;
	last_id = 0;

	for (v = 0;  (v == 0) || ((v > 0) && (vlans[v - 1].id.value > 0)); v ++){
		if (last_id != vlans[v].id.value){
			// current one is different from before, 
			if (same_pending){//  has some the same ids
				// don't 
				if (vlan_flag(lastvlan->id.flag) != VLAN_FLAG_NONE){// should select this one
					newvlan = lastvlan;
				}			
				// don't add vlan to be deleted
				if (newvlan){
					update += (vlan_flag(newvlan->id.flag) > 0) ? 1 : 0;
					if (vlan_flag(newvlan->id.flag) != VLAN_FLAG_DEL){
						if (add_to_tagged(mode, vlan_tagged(newvlan->id.flag))){
							vlan_list_add(&ltag, newvlan);
						}else {
							vlan_list_add(&luntag, newvlan);
						}
					}				
				}else {// no newvlan, just add last one, should not be here
					if (vlan_flag(lastvlan->id.flag) != VLAN_FLAG_DEL){
						if (add_to_tagged(mode, vlan_tagged(lastvlan->id.flag))){
							vlan_list_add(&ltag, lastvlan);
						}else {
							vlan_list_add(&luntag, lastvlan);
						}
					}
				}
				newvlan = NULL;
			}else if (lastvlan != NULL){
				// is single , just add
				update += (vlan_flag(lastvlan->id.flag) > 0) ? 1 : 0;
				// don't add vlan to be deleted
				if (vlan_flag(lastvlan->id.flag) != VLAN_FLAG_DEL){
					if (add_to_tagged(mode, vlan_tagged(lastvlan->id.flag))){
						vlan_list_add(&ltag, lastvlan);
					}else {
						vlan_list_add(&luntag, lastvlan);
					}
				}
			}
			same_pending = 0;
			last_id = vlans[v].id.value;
		}else {// the same as before
			same_pending = 1;
			// current one is the same with before, and handle the last one.
			if (lastvlan && (vlan_flag(lastvlan->id.flag) != VLAN_FLAG_NONE)){// should select this one
				newvlan = lastvlan;
			}
		}
		lastvlan = &vlans[v];
	}

	if (ltag){
		if (taglist){
			*taglist = ltag;
		}else {
			vlan_list_free(ltag);
		}
	}

	if (luntag){
		if (untaglist){
			*untaglist = luntag;
		}else {
			vlan_list_free(luntag);
		}
	}
	
}
switch_vlan_group_t *vlan_group_new_from_list(switch_vlan_list_t *head)
{
	int vlan_num, count;
	uint16_t last_id = 0;
	switch_vlan_list_t *v, *continue_start, *last;
	switch_vlan_group_t *group = NULL;
	
	vlan_num = vlan_list_count(head);
	group = (switch_vlan_group_t *)malloc(sizeof_vlan_group(vlan_num));
	if (group == NULL){
		DBG_ASSERT((group != NULL), "Alloc memory for vlan group failed");
		return NULL;
	}
	memset(group, 0, sizeof_vlan_group(vlan_num));
	
	// get lists count
	v = head;
	last = NULL;
	
	if (v != NULL){
		last_id = v->vlan.id.value;
		last = v;
		v = v->next;
	}	
	count = 0;
	continue_start = NULL;	
	while(v != NULL){
		if (v->vlan.id.value == last_id + 1){
			if (continue_start == NULL){
				continue_start = last;
			}
		}else {
		// save the last one
			if (continue_start != NULL){
				// from continue_star to last one
				if (count > vlan_num) break;
				group->vlans[count].id_range.start = continue_start->vlan.id.value;
				group->vlans[count].id_range.end = last->vlan.id.value;		
				LgcMaskCopy(&(group->vlans[count].member),&(last->vlan.member));			
				LgcMaskCopy(&(group->member),&(last->vlan.member));
				count ++;
				continue_start = NULL;
			}else {
				// the last one;
				group->vlans[count].id.value = last->vlan.id.value;
				group->vlans[count].id.flag = 0;						
				LgcMaskCopy(&(group->vlans[count].member),&(last->vlan.member));					
				LgcMaskCopy(&(group->member),&(last->vlan.member));
				count ++;
			}
		}		
		last_id = v->vlan.id.value;
		last = v;
		v = v->next;
	}
	// don't forget the last one or range
	if ((last != NULL) && (count < vlan_num)){
		if (continue_start != NULL){
			// from continue_star to last one
			group->vlans[count].id_range.start = continue_start->vlan.id.value;
			group->vlans[count].id_range.end = last->vlan.id.value;
			
			LgcMaskCopy(&(group->vlans[count].member),&(last->vlan.member));		
			LgcMaskCopy(&(group->member),&(last->vlan.member));
			count ++;
			continue_start = NULL;
		}else {
			// the last one;
			group->vlans[count].id.value = last->vlan.id.value;
			group->vlans[count].id.flag = 0;		
			LgcMaskCopy(&(group->vlans[count].member),&(last->vlan.member));	
			LgcMaskCopy(&(group->member),&(last->vlan.member));
			count ++;
		}
	}
	group->count = count;
	return group;
}

static int clt_vlan_interface_update(switch_vlan_interface_t **vi_old, switch_vlan_interface_t *vi_new, uint8_t flag)
{
	switch_vlan_interface_t *vi = *vi_old;
	switch_vlan_t *vlans;
	switch_vlan_group_t *vg, *v_tag, *v_untag;
	switch_vlan_list_t *taglist, *untaglist;
	uint16_t newpvid, mvlan;
	uint8_t newmode;
	
	int i, vlan_count;

	clt_vlanif_dump(*vi_old, 0);

	mvlan = sys_management_vlan();

	if ((flag == VLAN_FLAG_NEW) || (flag == VLAN_FLAG_DEL)){
		// for new and del, only change the vlans 
		newpvid = vi->pvid;
		newmode = vi->mode;
		
		if (vi->mode == LINK_TYPE_ACCESS){
			return 0;
		}
	}else {
		newpvid = vi_new->pvid;
		newmode = vi_new->mode;
	}

	// Step 1, move all vlans into vlans
	vlans = clt_vlan_interface_vlans(vi, &vlan_count);

	clt_vlans_dump(vlans);


	if (flag == VLAN_FLAG_SET){
		vlan_count = 0;// remove old vlans, when set
	}

	vg = vi_new->trunk_hybrid;
	for (i = 0; i < vi_new->count; i ++){
		vg->flag |= flag;// update = true
		vlan_count += get_group_vlans(vg, &vlans[vlan_count], MAX_VLAN_NUMS * 2 - vlan_count - 1);
		vg = vlan_group_to_next(vg);
	}

// we do something to make management ok

	// for access port pvid always is mvlan
	#if 0
	if (newmode == LINK_TYPE_ACCESS){
		newpvid = mvlan;
	}
	#endif

//end 
	
	vlans[vlan_count].id.value = 0;
	vlans[vlan_count].id.flag = 0;

	clt_vlans_dump(vlans);

	v_tag = NULL;
	v_untag = NULL;

	if ((vlan_count > 0) && (newmode != LINK_TYPE_ACCESS)){
		taglist = NULL;
		untaglist = NULL;
		
		clt_vlan_interface_refresh(vlans, vlan_count, &taglist, &untaglist, newmode);
		if (taglist){
			v_tag = vlan_group_new_from_list(taglist);
			if (v_tag) v_tag->flag = VLAN_FLAG_TAG;			
			vlan_list_free(taglist);
		}
		if (untaglist){
			v_untag = vlan_group_new_from_list(untaglist);
			vlan_list_free(untaglist);
		}		
	}
	
	vi = vlan_interface_new(vi_new->ifname, newmode, v_tag, v_untag);

	DBG_ASSERT(vi, "vlan_interface_new return NULL");

	if (v_tag) free(v_tag);
	if (v_untag) free(v_untag);
	
	if (vi){
		vi->pvid = newpvid;
		vi->phy = (*vi_old)->phy;
		vlan_interface_free(*vi_old);
		*vi_old = vi;
		clt_vlanif_dump(*vi_old, 1);		
		return 1;
	}
	
	return 0;
}

static int clt_vlan_interface_set(const char *name, switch_vlan_interface_t *vlanif, uint8_t flag)
{
	int i, set_index;
	clt_vlanif_entry_t * entry = &clt_vlanif_entry;

	// step 1, find index
	set_index = entry->count+1;
	
	for (i = 0; i < entry->count+1; i ++){
		if (entry->vlanifs[i] && !strcmp(name, entry->vlanifs[i]->ifname)) {
			set_index = i;
			break;		
		}
	}
	// if could not find?
	if (set_index > entry->count){
		return 0;
	}

	// step 2, re-create vlan groups

	return clt_vlan_interface_update(&entry->vlanifs[set_index], vlanif, flag);
}

static int clt_vlan_interface_group_set(switch_vlan_interface_t *vlanif, int count, int flag)
{
	int i, update = 0;
	
	for (i = 0; i < count; i ++){
		// get interface size by req->ifname
		clt_vlanif_dump(vlanif, i);
		update += clt_vlan_interface_set(vlanif->ifname, vlanif, flag);
		vlanif = vlan_interface_to_next(vlanif);
	}	
	return update;
}

static int clt_vlan_interface_set_default(switch_vlan_mode_t *vm)
{
	int i, count, update = 0;
	char buffer[sizeof(switch_vlan_interface_t) * (MAX_PORT_NUM+1)];
	switch_vlan_interface_t *vlanifs, *vif;
	clt_vlanif_entry_t *entry = &clt_vlanif_entry;
	// Step 1, set mode, 
	// for 802.1Q: 		eth1 access, others trunk
	// fro Transparent: 	eth1 eth2 by setting, others trunk	
	//int eth0_mode = LINK_TYPE_ACCESS;
	//int eth1_mode = LINK_TYPE_TRUNK;

	
	memset(buffer, 0, sizeof(buffer));
	vlanifs = (switch_vlan_interface_t *)buffer;
	count = 0;
	vif = vlanifs;
	
	for (i = 0; i < entry->count+1 ; i ++ ){	
		if (entry->vlanifs[i])
		{
			safe_strncpy(vif->ifname, entry->vlanifs[i]->ifname, sizeof(vif->ifname));
			// Step 2, set pvid,
			// for access port, pvid = mvlan
			// for other ports, pvid = 1			
			vif->mode = LINK_TYPE_ACCESS;
			vif->pvid = (vif->mode == LINK_TYPE_ACCESS) ? sys_management_vlan() : 1;
			vif->count = 0;
			count ++;
			printf(" %s %d port%d pvid %d\n",__FUNCTION__,__LINE__,i,vif->pvid);
			vif = vlan_interface_to_next(vif);
		}
	}
	
	
	if (count > 0){		
		entry->vlanmode = vm->mode;
		update = clt_vlan_interface_group_set(vlanifs, count, VLAN_FLAG_SET);
		return update;
	}

	return 0;
}
static switch_vlan_t * clt_vlan_entry_hash(int *vlan_count)
{
	int i, v, count = 0, cnt;
	static switch_vlan_t vlans[MAX_VLAN_NUMS + 1];
	switch_vlan_t vtmp[MAX_VLAN_NUMS + 1];
	clt_vlan_entry_t *entry = &clt_vlan_entry;

	memset(vlans, 0, sizeof(vlans));
		
	for (i = 0; (i < entry->count) && (count < MAX_VLAN_NUMS); i ++)
	{
		if (entry->groups[i]){
			cnt = get_group_vlans(entry->groups[i], vtmp, MAX_VLAN_NUMS);
			vtmp[cnt].id.value = 0;
			count += cnt;
			for (v = 0; vtmp[v].id.value > 0; v ++){
				vlans[vtmp[v].id.value].id.value = vtmp[v].id.value;			
				LgcMaskCopy(&(vlans[vtmp[v].id.value].member),&(vtmp[v].member));
			}
		}
	}	

	if (vlan_count) *vlan_count = count;
	return vlans;
}

static int clt_check_update_vlan_interface(switch_vlan_t *vlans_hash, int pvid_only)
{
	int i,k, update = 0, rmcnt = 0;
	clt_vlanif_entry_t * entry = &clt_vlanif_entry;
	switch_vlan_t *vlanth;
	uint16_t vlanrm[4096];
	switch_vlan_group_t *vgrm;
	switch_vlan_interface_t *vif, *vlanifrm;

	

	LgcPortFor(i)
	{
		vif = entry->vlanifs[i];
		// check pvid;
		if ((vif->mode == LINK_TYPE_ACCESS)
			/*|| cable_interface(vif->ifname)*/){
			if (vif->pvid !=  sys_management_vlan()){
				vif->pvid = sys_management_vlan();
				update ++;
			}
		}else {
			if (!vlan_exist(vlans_hash, vif->pvid)){
				vif->pvid = 1;
				update ++;
			}
		}
		
		if (pvid_only){
			continue;
		}
		
		// check vlan groups
		rmcnt = 0;

		vlanth = clt_vlan_interface_vlans(vif, NULL);
		for (k = 0; vlanth[k].id.value > 0; k ++){
			if (!vlan_exist(vlans_hash, vlanth[k].id.value)){
				update ++;
				//DBG_PRINTF("change %d @ %s", vlanth[k].id.value, vif->ifname);
				vlanrm[rmcnt ++] = vlanth[k].id.value;
			}
		}

		if (rmcnt > 0){
			vgrm = (switch_vlan_group_t *)malloc(sizeof_vlan_group(rmcnt));
			DBG_ASSERT(vgrm, " Malloc failed");
			if (vgrm){
				memset(vgrm, 0, sizeof_vlan_group(rmcnt));				

				for (k = 0; k < rmcnt; k ++){
					vgrm->vlans[k].id.value = vlanrm[k];
				}
				vgrm->count = rmcnt;
				
				vlanifrm = vlan_interface_new(vif->ifname, vif->mode, vgrm, NULL);
				if(vlanifrm) clt_vlan_interface_set(vif->ifname, vlanifrm, VLAN_FLAG_DEL);
				vlan_interface_free(vlanifrm);
				free(vgrm);
				rmcnt = 0;

				// vlans chaneg 
				//vlans = clt_vlan_entry_vlans(NULL);
				
			}			
		}
	}

	if (update){
		switch_vlan_interface_commit(0);
	}
	
	return update;
}
static void clt_interface_pvid_update(void)
{
	int i;
	
	LgcPortFor(i)
	{
		if (clt_vlanif_entry.vlanifs[i]){
			clt_interfaces[i].pvid = clt_vlanif_entry.vlanifs[i]->pvid;
		}		
	}
}
static int clt_check_update_vlan(switch_vlan_t * vlans_hash)
{
	int i,v, update = 0, count;
	clt_vlanif_entry_t * entry = &clt_vlanif_entry;
	switch_vlan_interface_t *vif;	
	switch_vlan_group_t *vg;
	switch_vlan_t *vs, vtmp[MAX_VLAN_NUMS + 1];
	


	LgcPortFor(i)
	{
		vif = entry->vlanifs[i];


		switch(vif->mode){
		case LINK_TYPE_ACCESS:
			for (v = 0; v < MAX_VLAN_NUMS; v++){
				if (vlan_exist(vlans_hash, v)){				
					ClrLgcMaskBit(vif->phy,&(vlans_hash[v].member));
				}
			}
			if (vlan_exist(vlans_hash, vif->pvid)){			
				SetLgcMaskBit(vif->phy,&(vlans_hash[vif->pvid].member));
			}		
			break;
			
		case LINK_TYPE_TRUNK:
			if (!vif->count){// all vlans
				for (v = 0; v < MAX_VLAN_NUMS; v++){
					if (vlan_exist(vlans_hash, v)){						
						SetLgcMaskBit(vif->phy,&(vlans_hash[v].member));
					}
				}					
			}else {
				for (v = 0; v < MAX_VLAN_NUMS; v++){
					if (vlan_exist(vlans_hash, v)){					
						ClrLgcMaskBit(vif->phy,&(vlans_hash[v].member));
					}
				}
				vs = clt_vlan_interface_vlans(vif, NULL);

				for (v = 0; vs[v].id.value > 0; v ++){
					DBG_ASSERT(vlan_exist(vlans_hash, vs[v].id.value), "Invalid Trunk VLAN %d", vs[v].id.value);
					if (vlan_exist(vlans_hash, vs[v].id.value)){						
						SetLgcMaskBit(vif->phy,&(vlans_hash[vs[v].id.value].member));
					}
				}				
			}
			break;
			
		case LINK_TYPE_HYBRID:	
			
			DBG_ASSERT(vif->count, "Hybrid VLAN empty");			
			// in any cases, we should clear this port in all vlans
			for (v = 0; v < MAX_VLAN_NUMS; v++){
				if (vlans_hash[v].id.value > 0){					
					ClrLgcMaskBit(vif->phy,&(vlans_hash[v].member));
				}
			}	
			vs = clt_vlan_interface_vlans(vif, NULL);

			for (v = 0; vs[v].id.value > 0; v ++){
				DBG_ASSERT(vlan_exist(vlans_hash, vs[v].id.value), "Invalid Hybrid VLAN %d", vs[v].id.value);
				if (vlan_exist(vlans_hash, vs[v].id.value)){			
					SetLgcMaskBit(vif->phy,&(vlans_hash[vs[v].id.value].member));
				}
			}			
			
			break;
		default:
			DBG_ASSERT(0, "Invalid VLAN Mode %d @ Port %s", vif->mode, vif->ifname);
		}
	}

	count = 0;
	memset(vtmp, 0, sizeof(vtmp));	
	for (v = 0; v < MAX_VLAN_NUMS; v++){
		if (vlan_exist(vlans_hash, v)){
			vtmp[count].id.value = vlans_hash[v].id.value;	
			LgcMaskCopy(&(vtmp[count].member),&(vlans_hash[v].member));
			count ++;
		}
	}	

	DBG_ASSERT(count, "Invalid VLAN NUM %d", count);

	if (!count){
		return 0;
	}

	vg = (switch_vlan_group_t *)malloc(sizeof_vlan_group(count));
	DBG_ASSERT(vg, "Malloc failed");
	if (vg){	
		vg->count = count;
		vg->flag = VLAN_FLAG_NONE;		
		ClrLgcMaskAll(&(vg->member));
		memcpy(vg->vlans, vtmp, count * sizeof(switch_vlan_t));

		clt_vlan_group_dump(vg, 1);
		
		update = clt_vlan_entry_set(vg, 1, VLAN_FLAG_SET);
		if (update){// save to nvram
			switch_vlan_commit(0);		
		}
		free(vg);
	}

	return update;
}
static char *vlan_member_string(logic_pmask_t member)
{
	static char buf[80];
	char tmpbuf[256];
	buf[0] = 0;
	port_num_t lport;
	int i=0;
	LgcPortFor(lport)
	{
	
		if(TstLgcMaskBit(lport,&member)==TRUE)
		{
			if(i==0)
			sprintf(tmpbuf,"Ethernet%d",lport);
			else
			sprintf(tmpbuf,",Ethernet%d",lport);			
			strcat(buf, tmpbuf);
			i++;
		}
	}

	
	return buf;
}

static void clt_vlan_group_dump(switch_vlan_group_t *vlan_groups, uint16_t count)
{
	int i, v;
	switch_vlan_group_t *group;

	if (!vlan_debug) return;
	group = vlan_groups;
	for (i = 0; i < count; i ++){
		fprintf(stderr, "Group<%d>\n", i);
		fprintf(stderr, "\tMember:%s\n", vlan_member_string(group->member));
		fprintf(stderr, "\tFlag  :%04X\n", group->flag);		
		fprintf(stderr, "\tVlans :");
		for (v = 0; v < group->count; v ++){
			if (is_vlan_range(&group->vlans[v])){
				fprintf(stderr, "%d-%d[%X],", 
					group->vlans[v].id_range.start,
					group->vlans[v].id_range.end,
					group->vlans[v].member.pbits[0]);
			}else{
				fprintf(stderr, "%d[%X]%s,", 
					group->vlans[v].id.value, 
					group->vlans[v].member.pbits[0],
					group->vlans[v].id.flag ? "*" : "");
			}
			if (v && !(v & 0x0f)) fprintf(stderr, "\n\t       ");
		}
		fprintf(stderr, "\n");			
		group = vlan_group_to_next(group);
	}
}

static int clt_vlan_entry_set(switch_vlan_group_t *vlan_groups, UINT16 count, UINT8 flag)
{
	int i;
	switch_vlan_group_t *vg;
	switch_vlan_t *vlans;
	int vlan_count;
	vg = vlan_groups;	
	clt_vlan_group_dump(vlan_groups, count);	
	vlans = clt_vlan_entry_vlans(&vlan_count);
	clt_vlans_dump(vlans);
	for (i = 0; i < count; i ++){
		vg->flag = flag;// update = true
		vlan_count += get_group_vlans(vg, &vlans[vlan_count], MAX_VLAN_NUMS * 2 - vlan_count);
		vg = vlan_group_to_next(vg);
	}
	
	vlans[vlan_count].id.value = 0;
	vlans[vlan_count].id.flag = 0;
	clt_vlans_dump(vlans);	
	return 	clt_vlan_entry_refresh(vlans, vlan_count);
}
static int clt_check_update_management_vlan(switch_vlan_t *vlans_hash)
{
	int mvid, update = 0;

	mvid = sys_management_vlan();

	if ((mvid < MAX_VLAN_NUMS) && !vlan_exist(vlans_hash, mvid)){
		update = sys_management_vlan_update(1);
	}
	return update;
}


int switch_vlan_config_apply(int reason)
{
	int update = 0, count = 0;
	switch_vlan_t *vlan_hash;
	
	vlan_hash = clt_vlan_entry_hash(&count);
	DBG_ASSERT(count, "Total %d VLANs", count);	
	if (reason == VLAN_CHANGED){
		update += clt_check_update_management_vlan(vlan_hash);
		update += clt_check_update_vlan_interface(vlan_hash, 0);// pvid_only = 0, update all
	}else if (reason == MVLAN_CHANGED){
		update += clt_check_update_vlan_interface(vlan_hash, 1);// pvid_only = 1, update pvid only	
	}else if (reason == VLANIF_CHANGED){
		// for now, do nothing
	}
	clt_interface_pvid_update();
	update += clt_check_update_vlan(vlan_hash);	
	return update;
}
static int clt_vlan_entry_refresh(switch_vlan_t *vlans, int count)
{
	int v, same_pending, update = 0;
	uint16_t last_id;
	switch_vlan_t *newvlan = NULL;
	switch_vlan_t *lastvlan = NULL;
		
	// sort ids from small to large
	qsort((void *)vlans, count, sizeof(switch_vlan_t), clt_vlan_compare);
	clt_vlans_dump(vlans);
	
	same_pending = 0;
	last_id = 0;
	
	for (v = 0; (v == 0) || ((v > 0) && (vlans[v - 1].id.value > 0)); v ++){
		if (last_id != vlans[v].id.value){
			// current one is different from before, 
			if (same_pending){//  has some the same ids
				// don't 
				if (vlan_flag(lastvlan->id.flag) != VLAN_FLAG_NONE){// should select this one
					newvlan = lastvlan;
				}
				// don't add vlan to be deleted
				if (newvlan){
					update += (vlan_flag(newvlan->id.flag) > 0) ? 1 : 0;
					if (default_vlan(newvlan->id.value) || (vlan_flag(newvlan->id.flag) != VLAN_FLAG_DEL)){
						clt_vlan_entry_add(newvlan);
					}				
				}else {// no newvlan, just add last one, should not be here
					if (default_vlan(lastvlan->id.value) || (vlan_flag(lastvlan->id.flag) != VLAN_FLAG_DEL)){
						clt_vlan_entry_add(lastvlan);
					}
				}
				newvlan = NULL;
			}else if (lastvlan != NULL){
				// is single , just add
				update += (vlan_flag(lastvlan->id.flag) > 0) ? 1 : 0;
				// don't add vlan to be deleted
				if (default_vlan(lastvlan->id.value) || (vlan_flag(lastvlan->id.flag) != VLAN_FLAG_DEL)){
					clt_vlan_entry_add(lastvlan);
				}
			}
			same_pending = 0;
			last_id = vlans[v].id.value;
		}else {// the same as before
			same_pending = 1;
			// current one is the same with before, and handle the last one.
			if (lastvlan && (vlan_flag(lastvlan->id.flag) != VLAN_FLAG_NONE)){// should select this one
				newvlan = lastvlan;
			}
		}
		lastvlan = &vlans[v];
	}
	// debug 
	clt_vlan_entry_dump();
	clt_vlan_entry_update();
	
	return update;
}
static void clt_vlan_entry_update(void)
{
	int i;
	clt_vlan_entry_t *entry = &clt_vlan_entry;
	switch_vlan_t vlan1;

	memset(&vlan1, 0, sizeof(vlan1));
	vlan1.id.value = 1;
	SetLgcMaskAll(&(vlan1.member));
	// now free all groups
	clt_vlan_entry_free_groups();

	if (entry->lists[0] == NULL){
		clt_vlan_entry_add(&vlan1);
	}

	entry->count = 0;
	for (i = 0; i < MAX_VLAN_GROUPS; i ++){
		if (entry->lists[i] == NULL){
			break;
		}
		entry->groups[i] = vlan_group_new_from_list(entry->lists[i]);
		if(entry->groups[i]){
			entry->count++;
		}			
	}
	// debug 
	clt_vlan_entry_dump();
	
	// now free all lists
	clt_vlan_entry_free_lists();
}
static vcfg_vlan_t *vcfg_vlan_apply_search(vlan_apply_t *vp, vlan_id_t vid)
{
	
	if ((vid < MAX_VLAN_NUMS) && (vp->vlans[vid].valid)){
		return &vp->vlans[vid];
	}
	
	return NULL;	
	
}
static int vcfg_vlan_port_config_access(vlan_apply_t *vp, vcfg_port_t *port, switch_vlan_interface_t *vlanif)
{
	vcfg_vlan_t *v;
	
	port->phy = vlanif->phy;
	port->ingress_filter = TRUE;
	port->accept_type = PORT_IN_FRAM_BOTH;//w03828
	port->tag_mode = PORT_EG_TAG_MODE_ORI;//w03828
	
	port->pvid = vlanif->pvid;

	v = vcfg_vlan_apply_search(vp, port->pvid);
	if (v){
	
		SetLgcMaskBit(port->phy,&(v->mbrmask));	
		SetLgcMaskBit(port->phy,&(v->untagmask));
	}
	
	return 1;
}

static int vcfg_vlan_port_config_trunk(vlan_apply_t *vp, vcfg_port_t *port, switch_vlan_interface_t *vlanif)
{
	int i;
	vcfg_vlan_t *v;
	switch_vlan_t *vlans;
	
	port->phy = vlanif->phy;
	port->ingress_filter = TRUE;
	port->accept_type = PORT_IN_FRAM_BOTH;
	port->tag_mode = PORT_EG_TAG_MODE_ORI;
	
	port->pvid = vlanif->pvid;

	v = vcfg_vlan_apply_search(vp, port->pvid);
	DBG_ASSERT(v, "Invalid Port %s PVID %d", vlanif->ifname, port->pvid);
	if (v)
	{
		if(TRUE == TstLgcMaskBit(port->phy,&(v->mbrmask)))
		{		
			SetLgcMaskBit(port->phy,&(v->untagmask));
		}
	}


// for vlan logic , we have already done in 'clt_check_update_vlan'

// If has some vlans, checks it 
	if (vlanif->count > 0){	
		// enable the specified vlans
		vlans = clt_vlan_interface_vlans(vlanif, NULL);
		
		for (i = 0; vlans[i].id.value > 0; i ++){
			v = vcfg_vlan_apply_search(vp, vlans[i].id.value);
			DBG_ASSERT(v, "Invalid Trunk VLAN %d at Port %s ", vlans[i].id.value, vlanif->ifname);	
			if (v){
				//DBG_ASSERT(MBRMASK_VALUE(v->mbrmask) & PORT_MBRMASK(port->phy), "Invalid VLAN %d member %08X", v->vid, MBRMASK_VALUE(v->mbrmask));
			}
		}
	}else {// allow all vlans
		for (i = 0; i < MAX_VLAN_NUMS; i ++){
			if (vp->vlans[i].valid ) {
				//debug;
			}
			else break;
		}
	}
	
	return 1;
}
static int vcfg_vlan_port_config_hybrid(vlan_apply_t *vp, vcfg_port_t *port, switch_vlan_interface_t *vlanif)
{
	int i;
	vcfg_vlan_t *v;
	switch_vlan_t *vlans;
	
	port->phy = vlanif->phy;
	port->ingress_filter = TRUE;
	port->accept_type = PORT_IN_FRAM_BOTH;
	port->tag_mode = PORT_EG_TAG_MODE_ORI;
	
	port->pvid = vlanif->pvid;

	
	v = vcfg_vlan_apply_search(vp, port->pvid);
	DBG_ASSERT(v, "Invalid Port %s PVID %d", vlanif->ifname, port->pvid);

	DBG_ASSERT(vlanif->count, "Hybrid VLAN empty at Port %s", vlanif->ifname);

	vlans = clt_vlan_interface_vlans(vlanif, NULL);

	for (i = 0;  vlans[i].id.value > 0; i ++){
		v = vcfg_vlan_apply_search(vp, vlans[i].id.value);
		DBG_ASSERT(v, "Invalid Hybrid VLAN %d at Port %s ", vlans[i].id.value, vlanif->ifname);					
		if (v){
			
			SetLgcMaskBit(port->phy,&(v->mbrmask));
			if (!vlan_tagged(vlans[i].id.flag)){			
				SetLgcMaskBit(port->phy,&(v->untagmask));
			}			
		}
	}	
	
	return 1;
}

static int vcfg_vlan_interface_config_get(vlan_apply_t *vp, switch_vlan_interface_t *vlanifs[])
{
	int i;

	LgcPortFor(i)
	{
		switch(vlanifs[i]->mode){
			case LINK_TYPE_ACCESS:
				vcfg_vlan_port_config_access(vp, &vp->ports[i], vlanifs[i]);
				break;
			case LINK_TYPE_TRUNK:
				vcfg_vlan_port_config_trunk(vp, &vp->ports[i], vlanifs[i]);
				break;
			case LINK_TYPE_HYBRID:
				vcfg_vlan_port_config_hybrid(vp, &vp->ports[i], vlanifs[i]);
				break;
		}
	}
	return 0;
}

static int vcfg_vlan_config_get(vlan_apply_t *vp)
{
	int i, vi;
	logic_pmask_t rmtag;
	switch_vlan_t *vlans;
	clt_vlanif_entry_t *ifentry = &clt_vlanif_entry;
	
	vp->vlanmode = ifentry->vlanmode;
	vp->management_vlan = sys_management_vlan();

	
	if (vp->vlanmode == VLAN_MODE_TRANSPARENT){		
	
		LgcPortFor(i)
		{
			if (ifentry->vlanifs[i]&&ifentry->vlanifs[i]->mode == LINK_TYPE_ACCESS){
				
				SetLgcMaskBit(ifentry->vlanifs[i]->phy,&rmtag);
			}
		}
		
		LgcPortFor(i)
		{
			vp->ports[i].phy = ifentry->vlanifs[i]->phy;
			vp->ports[i].pvid = (ifentry->vlanifs[i]->mode == LINK_TYPE_ACCESS) ? vp->management_vlan : 1;
			vp->ports[i].ingress_filter = FALSE;
			vp->ports[i].accept_type = PORT_IN_FRAM_BOTH;
			vp->ports[i].tag_mode = PORT_EG_TAG_MODE_KEEP;
			vp->vlans[i].vid = i;
			vp->vlans[i].fid = 0;
			ClrLgcMaskAll(&vp->vlans[i].mbrmask);
			SetLgcMaskBit(ifentry->vlanifs[i]->phy,&vp->vlans[i].mbrmask);	
			LgcMaskCopy(&(vp->vlans[i].untagmask),&rmtag);			
			vp->vlans[i].valid = 1;
		}
	}else { // 8021Q VLAN
		// Get all vlans
		vlans = clt_vlan_entry_vlans(NULL);
		clt_vlans_dump(vlans);
		for (i = 0; vlans[i].id.value > 0; i ++){
			vi = vlans[i].id.value;
			vp->vlans[vi].vid = vi;
			vp->vlans[vi].fid = 0;
//			vp->vlans[vi].fid = i + 1;		
			LgcMaskCopy(&(vp->vlans[vi].mbrmask),&(vlans[i].member));		
			ClrLgcMaskAll(&(vp->vlans[vi].untagmask));
			vp->vlans[vi].valid = 1;		
		}
		
		vcfg_vlan_interface_config_get(vp, ifentry->vlanifs);
	}
	
	return 1;
}
static vcfg_vlan_t * vcfg_vlan_config_static(int init)
{
	static vcfg_vlan_t vlans[MAX_VLAN_NUMS];

	if (init){
		memset(vlans, 0, sizeof(vlans));
	}
	return vlans;
}

//#define VLAN_APPLY_USE_FORK
#define VLAN_APPLY_USE_TIMER

#ifdef VLAN_APPLY_USE_TIMER
static int vcfg_timer_handler(timer_element_t *timer, void *data)
{
	vcfg_vlan_t *vlan;
	int i, ret, set_cnt = 0;
	logic_pmask_t emptymask;
	logic_pmask_t allmask;
	logic_pmask_t untagmask;


	
	ClrLgcMaskAll(&emptymask);
	ClrLgcMaskAll(&untagmask);
	ClrLgcMaskAll(&allmask);
# define VCFG_SET_NUMS  200

	vlan = vcfg_vlan_config_static(0);
	for (i = timer->privates; i < MAX_VLAN_NUMS; i ++){
		if (vlan[i].valid){		
			if(memcmp(&emptymask,&(vlan[i].mbrmask),sizeof(emptymask))!=0)
			{
				Ioctl_SetVlanEntryCreate(i);
			}
			Ioctl_GetVlanMember(i,&allmask,&untagmask);
			LgcMaskRemove(&untagmask,&(vlan[i].untagmask));			
			Ioctl_SetVlanMemberRemove(i,untagmask);
			/*begin add by shipeng 2013-09-05*/
			Ioctl_GetVlanMember(i,&allmask,&untagmask);				
			LgcMaskXor(&allmask,&(vlan[i].mbrmask));			
			Ioctl_SetVlanMemberRemove(i,allmask);
			/*end add by shipeng 2013-09-05*/
			
			/*Begin add by huangmingjian 2013-09-12*/
		#ifdef CONFIG_PRODUCT_5500
			logic_pmask_t ext_mbrmask;
			ClrLgcMaskAll(&ext_mbrmask);
			SetLgcMaskBit(LOGIC_PON_PORT, &ext_mbrmask);
			LgcMaskOr(&ext_mbrmask, &(vlan[i].mbrmask));
			Ioctl_SetVlanMemberAdd(i, ext_mbrmask, vlan[i].untagmask);
		#endif
			/*End add by huangmingjian 2013-09-12*/
			Ioctl_SetVlanMemberAdd(i,vlan[i].mbrmask, vlan[i].untagmask);
			if(memcmp(&emptymask,&(vlan[i].mbrmask),sizeof(emptymask))==0)
			{
				Ioctl_SetVlanEntryDelete(i);
			}
			vlan[i].valid = 0;			
			set_cnt ++;
		}
		if (set_cnt >= VCFG_SET_NUMS){
			break;
		}
	}
	timer->privates = i;
	fprintf(stderr, "set %d vlans\n", set_cnt);	
	return (i >= MAX_VLAN_NUMS) ? TIMER_RUN_ONCE : TIMER_RUN_FOREVER;
}
static void vcfg_timer_on_exit(timer_element_t *timer)
{
	int *p;
	if(timer->data){
		p = timer->data;
		*p = -1;
	}
}
#endif
static int vcfg_vlan_config_apply(vlan_apply_t *vp)
{
	int i, ret, update = 0;
	vcfg_vlan_t *last;
	vcfg_port_t * port;
	unsigned int vlanpri = 0;
	logic_pmask_t emptymask;
	logic_pmask_t allmask;
	logic_pmask_t untagmask;
	//printf("%s %d\n",__FUNCTION__,__LINE__);
	if(vp->vlanmode==VLAN_MODE_TRANSPARENT)
	{
		Ioctl_SetVlanMode(TRSPVLAN);
	}
	else
	{
		Ioctl_SetVlanMode(QVLAN);
	}
	ClrLgcMaskAll(&emptymask);
	ClrLgcMaskAll(&untagmask);
	ClrLgcMaskAll(&allmask);
#ifdef VLAN_APPLY_USE_FORK
	static pid_t vcfg_pid = -1;

	if (vcfg_pid > 0){
		kill(vcfg_pid, SIGTERM);
		vcfg_pid = -1;
	}
#endif /*#ifdef VLAN_APPLY_USE_FORK*/
	
#ifdef VLAN_APPLY_USE_TIMER
	static int timer_id = -1;
	if (timer_id > 0){
		timer_cancel(timer_id);
	}
#endif /*#ifdef VLAN_APPLY_USE_TIMER*/

	last = vcfg_vlan_config_static(0);

	for (i = 0; i < MAX_VLAN_NUMS; i ++){
		if ((last[i].vid != vp->vlans[i].vid) 
			|| (memcmp(&(last[i].mbrmask),&(vp->vlans[i].mbrmask),sizeof(vp->vlans[i].mbrmask))!=0)
			|| (memcmp(&(last[i].untagmask),&(vp->vlans[i].untagmask),sizeof(vp->vlans[i].untagmask))!=0)
			|| (last[i].fid != vp->vlans[i].fid)){
			//last[i].vid = vp->vlans[i].vid;
			//last[i].fid = vp->vlans[i].fid;			
			//printf("%s %d vlan %d\n",__FUNCTION__,__LINE__,vp->vlans[i].vid);
			LgcMaskCopy(&(last[i].mbrmask),&(vp->vlans[i].mbrmask));			
			LgcMaskCopy(&(last[i].untagmask),&( vp->vlans[i].untagmask));
			last[i].valid = 1; // update flags			
			update ++;
		}else {
			last[i].valid = 0;
		}
	}	

	
	if (update < 4095){
		for (i = 0; i < MAX_VLAN_NUMS; i++){
			if (last[i].valid){			
				//printf("%s %d vlan %d %x %x\n",__FUNCTION__,__LINE__,last[i].vid,vp->vlans[i].mbrmask,vp->vlans[i].untagmask);
				if(memcmp(&emptymask,&(last[i].mbrmask),sizeof(emptymask))!=0)
				{
					Ioctl_SetVlanEntryCreate(i);				
				}				
				Ioctl_GetVlanMember(i,&allmask,&untagmask);				
				LgcMaskRemove(&untagmask,&(vp->vlans[i].untagmask));			
				Ioctl_SetVlanMemberRemove(i,untagmask);
				/*begin add by shipeng 2013-09-05*/
				Ioctl_GetVlanMember(i,&allmask,&untagmask);				
				LgcMaskXor(&allmask,&(vp->vlans[i].mbrmask));			
				Ioctl_SetVlanMemberRemove(i,allmask);
				/*end add by shipeng 2013-09-05*/
				
				/*Begin add by huangmingjian 2013-09-12*/
			#ifdef CONFIG_PRODUCT_5500
				logic_pmask_t ext_mbrmask;
				ClrLgcMaskAll(&ext_mbrmask);
				SetLgcMaskBit(LOGIC_PON_PORT, &ext_mbrmask);
				LgcMaskOr(&ext_mbrmask, &(vp->vlans[i].mbrmask));
				Ioctl_SetVlanMemberAdd(i, ext_mbrmask, vp->vlans[i].untagmask);
			#endif
				/*End add by huangmingjian 2013-09-12*/
				Ioctl_SetVlanMemberAdd(i,vp->vlans[i].mbrmask, vp->vlans[i].untagmask);
				if(memcmp(&emptymask,&(last[i].mbrmask),sizeof(emptymask))==0)
				{
					Ioctl_SetVlanEntryDelete(i);
				}
				last[i].valid = 0;
			}
		}
		DBG_PRINTF("update %d vlans\n", update);	
	}else {
		#ifdef VLAN_APPLY_USE_FORK	
		vcfg_pid = fork();
		if (vcfg_pid == -1){
			DBG_ASSERT(0, "Failed to fork");
			return 0;
		}else if (vcfg_pid == 0){// child
			//free_all();
			//setsid();
			for (i = 0; i < MAX_VLAN_NUMS; i ++){
				if (last[i].valid){	
					if(memcmp(&emptymask,&(last[i].mbrmask),sizeof(emptymask))!=0)
					{
						Ioctl_SetVlanEntryCreate(i);
					}
					Ioctl_GetVlanMember(i,&allmask,&untagmask);
					LgcMaskRemove(&untagmask,&(vp->vlans[i].untagmask));			
					Ioctl_SetVlanMemberRemove(i,untagmask);
					/*begin add by shipeng 2013-09-05*/
					Ioctl_GetVlanMember(i,&allmask,&untagmask);				
					LgcMaskXor(&allmask,&(vp->vlans[i].mbrmask));			
					Ioctl_SetVlanMemberRemove(i,allmask);
					/*end add by shipeng 2013-09-05*/
					Ioctl_SetVlanMemberAdd(i,last[i].mbrmask, last[i].untagmask);
					if(memcmp(&emptymask,&(last[i].mbrmask),sizeof(emptymask))==0)
					{
						Ioctl_SetVlanEntryDelete(i);
					}
					last[i].valid = 0;
				}
			}
			DBG_PRINTF("update %d vlans\n", update);
			exit(0);
		}
		#endif /*#ifdef VLAN_APPLY_USE_FORK*/

		#ifdef VLAN_APPLY_USE_TIMER
		timer_register(500, 0, (timer_func_t)vcfg_timer_handler, &timer_id, (timer_exit_t)vcfg_timer_on_exit, "vlan_apply");
		#endif /*#ifdef VLAN_APPLY_USE_TIMER*/
	}
	
	// Apply all ports
	
	LgcPortFor(i)
	{
		port = &vp->ports[i];	
		//printf("%s %d port %d pvid %d\n",__FUNCTION__,__LINE__,i,port->pvid);
		Ioctl_SetVlanEntryCreate(i);
		Ioctl_GetPortPriority(i,&vlanpri);
		port->pvid = ((vlanpri & 0x0000000F) << 12)|(port->pvid & 0x0FFF);
		Ioctl_SetVlanPvid(i,port->pvid);
		Ioctl_port_ingress_vlan_filter(i, port->ingress_filter);
		Ioctl_port_ingress_vlan_rcv_mod(i,port->accept_type);		
		Ioctl_port_egress_vlan_mode(i, port->tag_mode);
			
	}
	
	return 1;
}

int switch_vlan_apply_all(void)
{
	vlan_apply_t vconf;
	
	memset(&vconf, 0, sizeof(vconf));
	//printf("%s %d\n",__FUNCTION__,__LINE__);
	vcfg_vlan_config_get(&vconf);
	//printf("%s %d\n",__FUNCTION__,__LINE__);
	vcfg_vlan_config_dump(&vconf);
	//printf("%s %d\n",__FUNCTION__,__LINE__);
	vcfg_vlan_config_apply(&vconf);
	//printf("%s %d\n",__FUNCTION__,__LINE__);
	return 1;
}

int switch_vlan_mode_apply( int apply_option)
{
		//printf("%s %d\n",__FUNCTION__,__LINE__);
        switch_vlan_config_apply(VLANIF_CHANGED);
		//printf("%s %d\n",__FUNCTION__,__LINE__);
        return switch_vlan_apply_all();
}
int switch_vlan_apply(int apply_option)
{
		//printf("%s %d\n",__FUNCTION__,__LINE__);
        switch_vlan_config_apply(VLAN_CHANGED);
		//printf("%s %d\n",__FUNCTION__,__LINE__);
        return switch_vlan_apply_all();
}
switch_vlan_group_t *switch_vlan_data_malloc(void)
{
        clt_vlan_entry_t *entry = &clt_vlan_entry;
        switch_vlan_group_t *group;
        uint32_t size;
        char *p, *pi;
        int i;
        size = switch_vlan_data_size();
        if (size == 0) return NULL;

        p = (char *)malloc(size);
        if (p == NULL){
                DBG_ASSERT((p != NULL), "Alloc memory for vlan group data failed");
                return NULL;
        }
        pi = p;
        for (i = 0; i < entry->count ; i ++){
                group = entry->groups[i];
                if (group){
                        size = offsetof(switch_vlan_group_t, vlans) + group->count * sizeof(switch_vlan_t);
                        memcpy(pi, group, size);
                        pi += size;
                }
        }
        return (switch_vlan_group_t*)p;
}


/*
 for group get
*/

switch_vlan_group_t *switch_vlan_group_get(switch_vlan_group_t *vlan_groups, uint16_t count, int *ret_count)
{
	int i, v, vcnt;
	switch_vlan_group_t *vg;
	switch_vlan_t vlans_req[MAX_VLAN_NUMS];
	switch_vlan_t *vlans, *lastvlan;
	switch_vlan_list_t *lists[MAX_VLAN_GROUPS];
	switch_vlan_group_t *groups[MAX_VLAN_GROUPS];
	uint32_t vgsize, size;
	char *p;
	uint16_t last_id;


	//Step 1, convert to vlans
	vg = vlan_groups;
	vcnt = 0;
	for (i = 0; i < count; i ++){
		vcnt = get_group_vlans(vg, &vlans_req[vcnt], MAX_VLAN_NUMS - vcnt - 1);
		vg = vlan_group_to_next(vg);
	}
	// set to the end
	vlans_req[vcnt].id.value = 0;
	vlans_req[vcnt].id.flag = 0;
	
	vlans = clt_vlan_entry_vlans(NULL);

	clt_vlans_dump(vlans_req);
	//clt_vlans_dump(vlans);
	
	// Step 2, convert fill member value
	for (i = 0; vlans_req[i].id.value > 0; i ++){
		for (v = 0; vlans[v].id.value > 0; v ++){
			if (vlans_req[i].id.value == vlans[v].id.value){
			//	vlans_req[i].member = vlans[v].member;
				LgcMaskCopy(&(vlans_req[i].member),&(vlans[v].member));
				break;
			}
		}
		if (vlans[v].id.value == 0){// Could not find the vlan id, not exist
			//vlans_req[i].member = 0;
			ClrLgcMaskAll(&(vlans_req[i].member));
		}
	}	
	clt_vlans_dump(vlans_req);
	
	// Step 3, convert to lists
	qsort((void *)vlans_req, vcnt, sizeof(switch_vlan_t), clt_vlan_compare);

	memset(lists, 0, sizeof(lists));

	last_id = 0;
	lastvlan = NULL;

	for (v = 0;  (v == 0) || ((v > 0) && (vlans_req[v - 1].id.value > 0)); v ++){
		if ((last_id != vlans_req[v].id.value) && lastvlan){
			for (i = 0; i < MAX_VLAN_GROUPS; i ++){
				if (lists[i] == NULL) {
					vlan_list_add(&lists[i], lastvlan);
					break;
				}
				if ((lists[i] != NULL) 
					&& (memcmp(&(lists[i]->vlan.member),&(lastvlan->member),sizeof(lastvlan->member))==0)){
					vlan_list_add(&lists[i], lastvlan);
					break;
				}
			}
		}else {// the same as before
			//same_pending = 1;
		}
		last_id = vlans_req[v].id.value;		
		lastvlan = &vlans_req[v];
	}
	
	// Step 4, new from list

	count = 0;
	vgsize = 0;
	for (i = 0; i < MAX_VLAN_GROUPS; i ++){
		if (lists[i] == NULL){
			break;
		}
		groups[count] = vlan_group_new_from_list(lists[i]);
		if(groups[count]){
			vgsize += sizeof_vlan_group(groups[count]->count);
			count ++;
		}
		// free the list
		vlan_list_free(lists[i]);
		lists[i] = NULL;
	}

	vg = NULL;
	if (count > 0){
		vg = (switch_vlan_group_t *)malloc(vgsize);
		DBG_ASSERT(vg, "malloc failed");
		if (vg != NULL){
			size = 0;
			p = (char *)vg;
			for (i = 0; i < count; i ++){
				memcpy(p + size, groups[i], sizeof_vlan_group(groups[i]->count));
				size += sizeof_vlan_group(groups[i]->count);
			}
		}
	}

	// Step 5, free all memory

	for (i = 0; i < count; i ++){
		if (groups[i]) {
			free(groups[i]);
		}	
	}

	// Step 6, return values

	if (ret_count){
		*ret_count = count;
	}
	if (vg){
		clt_vlan_group_dump(vg, count);
	}
	
	return vg;
}
int switch_vlan_group_update(switch_vlan_group_t *vlan_groups, uint16_t count)
{
        return clt_vlan_entry_set(vlan_groups, count, VLAN_FLAG_SET);
}
int switch_vlan_group_delete(switch_vlan_group_t *vlan_groups, uint16_t count)
{
        return clt_vlan_entry_set(vlan_groups, count, VLAN_FLAG_DEL);;
}
int switch_vlan_group_new(switch_vlan_group_t *vlan_groups, uint16_t count)
{
        return clt_vlan_entry_set(vlan_groups, count, VLAN_FLAG_NEW);
}
void switch_vlan_mode_get(switch_vlan_mode_t *vm)
{
	int lport;
	clt_vlanif_entry_t * entry = &clt_vlanif_entry;
		
	vm->mode = entry->vlanmode;
	vm->mvlan = sys_management_vlan();

	LgcPortFor(lport)
	{
		if(entry->vlanifs[lport])
		{
			safe_strncpy(vm->ports[lport].ifname, entry->vlanifs[lport]->ifname, sizeof(vm->ports[lport].ifname));
			vm->ports[lport].type = entry->vlanifs[lport]->mode;
		}
	}
}

uint32_t switch_vlan_data_size(void)
{
	clt_vlan_entry_t *entry = &clt_vlan_entry;
	switch_vlan_group_t *group;
	int i;
	uint32_t size = 0;
	for (i = 0; i < entry->count ; i ++){
		group = entry->groups[i];
		if (group)
			size += offsetof(switch_vlan_group_t, vlans) + group->count * sizeof(switch_vlan_t);
	}
	return size;
}

static void vcfg_vlan_config_dump(vlan_apply_t *vp)
{
	int i;
	vcfg_vlan_t * vlan;
	vcfg_port_t * port;
	char s1[80], s2[80];
	clt_vlanif_entry_t *ifentry = &clt_vlanif_entry;	
	logic_pmask_t tag;

	if (!vcfg_debug) return;

	fprintf(stderr, "------VLAN----------------------\n");

	
	for (i = 0; i < MAX_VLAN_NUMS; i ++){
		vlan = &vp->vlans[i];
		if (vlan->valid){		
			LgcMaskCopy(&tag,&(vlan->mbrmask));
			LgcMaskRemove(&tag,&(vlan->untagmask));			
			strcpy(s1, vlan_member_string(tag));
			strcpy(s2, vlan_member_string((vlan->untagmask)));
			fprintf(stderr, " VLAN:%d \tFID:%d \tTag:%s \tUntag:%s\n", vlan->vid, vlan->fid, s1, s2);
		}
	}
	fprintf(stderr, "------PORT----------------------\n");	
	// Apply all ports
	LgcPortFor(i)
	{
		port = &vp->ports[i];
		fprintf(stderr, " PORT:%s PHY:%d PVID:%d PRI:%d ATTR(%d,%d,%d)\n", ifentry->vlanifs[i]->ifname,
			port->phy, port->pvid, port->priority, port->accept_type, port->ingress_filter, port->tag_mode);	
	}
	
	fprintf(stderr, "------END-----------------------\n");	
}


static void clt_vlan_entry_dump(void)
{
	int i, v;
	clt_vlan_entry_t *entry = &clt_vlan_entry;	
	switch_vlan_group_t *group;
	switch_vlan_list_t *list;

	if (!vlan_debug) return;
	
	// dump groups
	for (i = 0; i < MAX_VLAN_GROUPS; i ++){
		group = entry->groups[i];
		if (group){
			fprintf(stderr, "Group<%d>\n", i);
			fprintf(stderr, "\tMember:%s\n", vlan_member_string(group->member));
			fprintf(stderr, "\tVlans :");
			for (v = 0; v < group->count; v ++){
				if (is_vlan_range(&group->vlans[v])){
					fprintf(stderr, "%d-%d,", 
						group->vlans[v].id_range.start,
						group->vlans[v].id_range.end);
				}else{
					fprintf(stderr, "%d%s,", 
						group->vlans[v].id.value, 
						group->vlans[v].id.flag ? "(*)" : "");
				}
				if (v && !(v & 0x0f)) fprintf(stderr, "\n\t       ");
			}
			fprintf(stderr, "\n");
		}else {
			fprintf(stderr, "entry<%d> empty\n", i);
		}
	}

	for (i = 0; i < MAX_VLAN_GROUPS; i ++){
		list = entry->lists[i];
		if (list){
			fprintf(stderr, "List<%d>\n", i);
			fprintf(stderr, "\tMember:%s\n", vlan_member_string(list->vlan.member));
			fprintf(stderr, "\tVlans :");
			v = 0;
			while(list != NULL){
				fprintf(stderr, "%d%s,", 
					list->vlan.id.value, 
					list->vlan.id.flag ? "(*)" : "");		
				list = list->next;
				if (v && !(v & 0x0f)) fprintf(stderr, "\n\t		 ");
				v ++;
			}
			fprintf(stderr, "\n");
		}else {
			fprintf(stderr, "List<%d> empty\n", i);
		}
	}
	return ;
		
}
uint16_t switch_vlan_entry_count(void)
{
	clt_vlan_entry_t *entry = &clt_vlan_entry;
	return entry->count;
}
void switch_vlan_data_free(void *data)
{
	if (data) free(data);
}
static int clt_vlan_entry_init(void)
{
	int i;
	char nv_name[BUF_SIZE1024*16], nv_val[BUF_SIZE1024*16];
	clt_vlan_entry_t *entry = &clt_vlan_entry;
	memset(&clt_vlan_entry, 0, sizeof(clt_vlan_entry));
	//printf(" %s %d\n",__FUNCTION__,__LINE__);
	for (i = 0; i < MAX_VLAN_GROUPS; i ++){
		memset(nv_val,0,sizeof(nv_val));
		cfg_getval(IF_ROOT_IFINDEX,CONFIG_VLANGROUP+i+1,nv_val,NULL,sizeof(nv_val));
		//printf(" %s %d %s\n",__FUNCTION__,__LINE__,nv_val);
		if(!nv_val[0]) break;
		entry->groups[entry->count] = vlan_group_new_from_string(nv_val);
		if(entry->groups[entry->count]){
			entry->count ++;
		}	
	}
	return entry->count;
}
static void clt_vlan_interface_init(void)
{
	int i, pvid, mode,lport;
	switch_vlan_group_t *vlan0, *vlan1; // for trunk use vlan0 only , for hybrid vlan0 is tag and vlan1 is untag 
	clt_vlanif_entry_t * entry = &clt_vlanif_entry;
	char vp[BUF_SIZE1024*4];
	char ifname[BUF_SIZE_64];
//	vp = nvram_safe_get(nvram_handle, "vlan_mode");
	memset(vp,0,sizeof(vp));
	cfg_getval(IF_ROOT_IFINDEX,CONFIG_VLAN_MODE,vp,NULL,sizeof(vp));
	if (vp[0]!=0){
		entry->vlanmode = strtoul(vp, NULL, 0);
	}else {
		entry->vlanmode = DEFAULT_SWITCH_VLAN_MODE;
	}

	
	LgcPortFor(lport)
	{		
		
		sprintf(ifname,"%s%d",IF_ETHPORT_NAMEPREFIX,lport);
		memset(vp,0,sizeof(vp));
		cfg_getval(IF_ROOT_IFINDEX,CONFIG_PORT_PVID_ETH+lport,vp,NULL,sizeof(vp));
		pvid = vp[0]!=0? strtoul(vp, NULL, 0) : 1;			
		memset(vp,0,sizeof(vp));
		cfg_getval(IF_ROOT_IFINDEX,CONFIG_PORT_LINKTYPE_ETH+lport,vp,NULL,sizeof(vp));
		mode = vp[0]!=0 ? switchport_atoi(vp) : LINK_TYPE_ACCESS;
		
		vlan0 = NULL;
		vlan1 = NULL;

		if (mode == LINK_TYPE_TRUNK){
			// get premit vlans	
			memset(vp,0,sizeof(vp));
			cfg_getval(IF_ROOT_IFINDEX,CONFIG_PORT_TRUNK_PERMIT_ETH+lport,vp,NULL,sizeof(vp));		
			vlan0 = vp[0]!=0  ? vlan_group_new_from_string_simple(vp) : NULL;
			if (vlan0) vlan0->flag = VLAN_FLAG_TAG;

		}else if(mode == LINK_TYPE_HYBRID){
			// get tag vlans 
			memset(vp,0,sizeof(vp));		
			cfg_getval(IF_ROOT_IFINDEX,CONFIG_PORT_LINK_HYBRID_TAG_ETH+lport,vp,NULL,sizeof(vp));		
			vlan0 = vp[0]!=0 ? vlan_group_new_from_string_simple(vp) : NULL;
			if (vlan0) vlan0->flag = VLAN_FLAG_TAG;
			// get untag vlans		
			memset(vp,0,sizeof(vp));
			cfg_getval(IF_ROOT_IFINDEX,CONFIG_PORT_LINK_HYBRID_UNTAG_ETH+lport,vp,NULL,sizeof(vp));
			vlan1 = vp[0]!=0 ? vlan_group_new_from_string_simple(vp) : NULL;
			//if (vlan1) vlan1->flag = VLAN_FLAG_UNTAG; wanghuanyu
		}
		
		entry->vlanifs[lport] = vlan_interface_new(ifname, mode, vlan0, vlan1);
		if (entry->vlanifs[lport] != NULL){
			entry->vlanifs[lport]->pvid = pvid;
			entry->vlanifs[lport]->phy = lport;
		}
		if (vlan0) free(vlan0);
		if (vlan1) free(vlan1);
	}
	entry->count = MAX_PORT_NUM;
}

static void clt_vlanif_entry_dump(void)
{
	int i;
	clt_vlanif_entry_t *entry = &clt_vlanif_entry;	

	if (!vlan_debug) return;

	// dump groups
	for (i = 0; i < entry->count+1; i ++){
		clt_vlanif_dump(entry->vlanifs[i], i);
	}
}

int switch_config_init(void)
{
	#ifdef SWITCH_STYLE
	clt_vlan_entry_init();	
	clt_vlan_entry_dump();	
	clt_vlan_interface_init();	
	clt_vlanif_entry_dump();	
	vcfg_vlan_config_static(1);
	#endif
	Ioctl_SetRateLimitInit();
	Ioctl_SetQosInit();
	return 0;
}
uint32_t switch_vlan_interface_data_size(void)
{
	int i;
	uint32_t size = 0;
	clt_vlanif_entry_t * entry = &clt_vlanif_entry;
	for (i = 0; i < entry->count+1; i ++){
		if (entry->vlanifs[i]){
			size += sizeof_vlan_interface(entry->vlanifs[i]->trunk_hybrid, entry->vlanifs[i]->count);
		}
	}
	return size;	
}
switch_vlan_interface_t *switch_vlan_interface_data_malloc(void)
{
	int i;
	uint32_t size = 0;
	clt_vlanif_entry_t * entry = &clt_vlanif_entry;
	switch_vlan_interface_t *vlanif, *vi;

	size = switch_vlan_interface_data_size();
	vlanif = (switch_vlan_interface_t *)malloc(size);
	if (vlanif == NULL){
		DBG_ASSERT((vlanif != NULL), "Alloc memory for vlan group data failed");
		return NULL;		
	}
	vi = vlanif;
	for (i = 0; i < entry->count+1; i ++){
		if (entry->vlanifs[i]){
			memcpy(vi, entry->vlanifs[i], sizeof_vlan_interface(entry->vlanifs[i]->trunk_hybrid, entry->vlanifs[i]->count));
			vi = vlan_interface_to_next(vi);
		}
	}
	
	return vlanif;	
}

uint16_t switch_vlan_interface_count(void)
{
	clt_vlanif_entry_t * entry = &clt_vlanif_entry;
	return entry->count;
}
void switch_vlan_interface_data_free(void *vlanif)
{
	if (vlanif) free(vlanif);
}
static uint32_t clt_vlan_interface_mem_size(const char *ifname)
{
	int i;
	clt_vlanif_entry_t * entry = &clt_vlanif_entry;
	
	for (i = 0; i < entry->count+1; i ++) {
		if (entry->vlanifs[i] && !strcmp(ifname, entry->vlanifs[i]->ifname)){
			return sizeof_vlan_interface(entry->vlanifs[i]->trunk_hybrid, entry->vlanifs[i]->count);
		}
	}
	return 0;
}

static switch_vlan_interface_t * clt_vlan_interface_mem(const char *ifname)
{
	int i;
	clt_vlanif_entry_t * entry = &clt_vlanif_entry;
	
	for (i = 1; i < MAX_PORT_NUM+1; i++) {
		if (entry->vlanifs[i] && !strcmp(ifname, entry->vlanifs[i]->ifname)){
			return entry->vlanifs[i];
		}
	}
	return 0;
}
void switch_vlan_interface_group_free(void *vlanif)
{
	if (vlanif) free(vlanif);
}

switch_vlan_interface_t *switch_vlan_interface_group_malloc(switch_vlan_interface_t *req_vlanif, uint16_t req_count, uint16_t *ret_count)
{
	int i, count = 0;
	switch_vlan_interface_t *vi_req,*vi_ack, *vi;
	uint32_t size = 0;
	
	// step 1, get data size;
	vi_req = req_vlanif;
	for (i = 0; i < req_count; i ++){
		// get interface size by req->ifname
		size += clt_vlan_interface_mem_size(vi_req->ifname);
		vi_req = vlan_interface_to_next(vi_req);
	}
	// step 2, malloc 

	vi_ack = (switch_vlan_interface_t *)malloc(size);
	if (vi_ack == NULL){
		DBG_ASSERT((vi_ack != NULL), "Alloc memory for vlan group data failed");
		return NULL;			
	}

	// step 3, fill data
	vi_req = req_vlanif;
	vi = vi_ack;
	count = 0;
	for (i = 0; i < req_count; i ++){
		// get interface size by req->ifname
		size = clt_vlan_interface_mem_size(vi_req->ifname);
		if (size > 0){
			memcpy(vi, clt_vlan_interface_mem(vi_req->ifname), size);
			vi =  vlan_interface_to_next(vi);
			count ++;
		}
		vi_req = vlan_interface_to_next(vi_req);
	}	
	if (ret_count){
		*ret_count = count;
	}
	return vi_ack;
}
int switch_vlan_interface_group_update(switch_vlan_interface_t *vlanif, uint16_t count)
{
	return clt_vlan_interface_group_set(vlanif, count, VLAN_FLAG_SET);
}

int switch_vlan_interface_group_delete(switch_vlan_interface_t *vlanif, uint16_t count)
{
	return clt_vlan_interface_group_set(vlanif, count, VLAN_FLAG_DEL);
}
int switch_vlan_interface_group_new(switch_vlan_interface_t *vlanif, uint16_t count)
{
	return clt_vlan_interface_group_set(vlanif, count, VLAN_FLAG_NEW);
}
int switch_vlan_interface_apply(int apply_option)
{
	switch_vlan_config_apply(VLANIF_CHANGED);
	return switch_vlan_apply_all();
}
int switch_vlan_commit(int apply_option)
{
	int i, vlen;
	clt_vlan_entry_t *entry = &clt_vlan_entry;	
	switch_vlan_group_t *group;
	char buffer[4096*4+1];
	
	for (i = 0; i < MAX_VLAN_GROUPS; i ++){
		group = entry->groups[i];
		master_cfg_del(IF_ROOT_IFINDEX,CONFIG_VLANGROUP+i+1);
		if (!group) break;
		
		vlen = sprintf(buffer, "%d;%s;", i, vlan_member_string(group->member));
		vlen += vlan_group_sprintf(buffer + vlen, group);
		master_cfg_setval(IF_ROOT_IFINDEX,CONFIG_VLANGROUP+i+1,buffer);
	}
	
	return 1;
}
int switch_vlan_interface_commit(int apply_option)
{
	int i=0, g=0;
	switch_vlan_interface_t *ifp;
	clt_vlanif_entry_t * entry = &clt_vlanif_entry;
	switch_vlan_group_t *group;
	char tag[4096*4], untag[4096*4];
	char buf[1024];
	
	master_cfg_setval(IF_ROOT_IFINDEX,CONFIG_VLAN_MODE,entry->vlanmode ? "1" : "0");
	
	LgcPortFor(i)
	{
		ifp = entry->vlanifs[i];		
		memset(buf,0,sizeof(buf));
		sprintf(buf,"%d", ifp->pvid);
		master_cfg_setval(IF_ROOT_IFINDEX,CONFIG_PORT_PVID_ETH+i,buf);
		master_cfg_setval(IF_ROOT_IFINDEX,CONFIG_PORT_LINKTYPE_ETH+i,switchport_itoa(ifp->mode));
		if ((ifp->mode != LINK_TYPE_TRUNK)
			&& (ifp->mode != LINK_TYPE_HYBRID)){
			continue;	
		}

		tag[0] = 0;
		untag[0] = 0;
		group = ifp->trunk_hybrid;
		for (g = 0; g < ifp->count; g ++){
			vlan_group_sprintf((g == 0) ? tag : untag, group);
			group = vlan_group_to_next(group);
		}	
		
		// for hybrid, at least one untag group is needed. set to 1 
		if (ifp->mode == LINK_TYPE_HYBRID){
			if (!tag[0] && !untag[0]){
				strcpy(untag, "1");
			}		
			master_cfg_setval(IF_ROOT_IFINDEX,CONFIG_PORT_LINK_HYBRID_TAG_ETH+i,tag);
			master_cfg_setval(IF_ROOT_IFINDEX,CONFIG_PORT_LINK_HYBRID_UNTAG_ETH+i,untag);
		}else {// trunk				
			master_cfg_setval(IF_ROOT_IFINDEX,CONFIG_PORT_TRUNK_PERMIT_ETH+i,tag);
		}
	}
	return 1;
}

int switch_vlan_mode_commit( int apply_option)
{
	return switch_vlan_interface_commit(apply_option);
}
int switch_apply(void)
{
        
	#ifdef SWITCH_STYLE
    switch_vlan_apply_all();
	#endif      
	return 0;
}

