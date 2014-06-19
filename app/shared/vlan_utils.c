/*
 * vlan_utils.c
 *
 *  Created on: Mar 18, 2011
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

//#include <cable.h>
#include <debug.h>
#include "basetype.h"
#include "str_utils.h"
#include <lw_type.h>
#include <lw_drv_pub.h>

#include "lw_if_pub.h"

#include <pdt_config.h>
#include <vlan_utils.h>

//#define DBG_ASSERT(...)


static int get_vlan_num(const char *vstr)
{
	int items;
	char *vars[4096];	
	char *str= strdup(vstr);
	items = split_string(str, ',', vars);	
	free(str);
	return items;
}

#if 1

/*
  nvram string functions 
*/
static int decode_vlan_member(logic_pmask_t *member, char *str)
{
	
	
	char *input, *vars[10];
	int items, i;
	int port=0;
	input = strdup(str);
	//*member = 0;
	ClrLgcMaskAll(member);
	items = split_string(input, ',', vars);
	for(i = 0; i < items; i ++) {
		#if 0
		if(!strcmp(vars[i], "cab0")) *member |= (1 << CLT0_PORT_NUMBER);
		else if(!strcmp(vars[i], "cab1")) *member |= (1 << CLT1_PORT_NUMBER);
		else if(!strcmp(vars[i], "eth0")) *member |= (1 << ETH0_PORT_NUMBER);
		else if(!strcmp(vars[i], "eth1")) *member |= (1 << ETH1_PORT_NUMBER);
		#endif
		if(strstr(vars[i],"Ethernet")!=NULL)
		sscanf(vars[i],"Ethernet%d",&port);	
		else if(strstr(vars[i],CLI_GIGAPORTPREFIX)!=NULL)
		sscanf(vars[i],CLI_GIGAPORTPREFIX"%d",&port);
		else if(strstr(vars[i],CLI_FASTPORTPREFIX)!=NULL)
		sscanf(vars[i],CLI_FASTPORTPREFIX"%d",&port);
		else if(strstr(vars[i],WEB_GIGAPORTPREFIX)!=NULL)
		sscanf(vars[i],WEB_GIGAPORTPREFIX"%d",&port);
		else if(strstr(vars[i],WEB_FASTPORTPREFIX)!=NULL)
		sscanf(vars[i],WEB_FASTPORTPREFIX"%d",&port);
		if(VALID_PORT(port))
		SetLgcMaskBit((port_num_t)port,member);
	}
	free(input);
	
	return 0;
}

#endif


static int decode_vlan_ids(switch_vlan_t *vlans, char *vstr, int num)
{
	char *input, *vars[4096], *cp;
	int items, i;
	input = strdup(vstr);
	items = split_string(input, ',', vars);
	items = (items < num) ? items : num;
	for(i = 0; i < items; i ++) {
		cp  = strchr(vars[i], '-');
		if (cp){
			*cp = 0;
			cp ++;
			if (*cp) {
				vlans[i].id_range.start = strtoul(vars[i], NULL, 0);
				vlans[i].id_range.end = strtoul(cp, NULL, 0);
				continue;
			}
		}
		vlans[i].id.value = strtoul(vars[i], NULL, 0);
		vlans[i].id.flag = 0;
	}
	free(input);
	return items;
}


switch_vlan_group_t *vlan_group_new_from_string_simple(const char* vstr)
{
	int vlan_num; 
//	char *vars[10];
	switch_vlan_group_t *group = NULL;

	//DBG_PRINTF("vstr:%s", vstr);
	char *str= strdup(vstr);
	
	vlan_num = get_vlan_num(str);
	if (vlan_num > 0){
		//DBG_PRINTF("vlan_num:%d", vlan_num);
		group = (switch_vlan_group_t *)malloc(sizeof_vlan_group(vlan_num));
		if (group == NULL){
			DBG_ASSERT((group != NULL), "Alloc memory for vlan group failed");
			free(str);
			return NULL;
		}
		memset(group, 0, sizeof_vlan_group(vlan_num));
		group->count = decode_vlan_ids(&group->vlans[0], str, vlan_num);
	}
	
	free(str);
	return group;
}



switch_vlan_group_t *vlan_group_new_from_string(const char* vstr)
{
	int items, vlan_num, i; 
	char *vars[10];
	switch_vlan_group_t *group = NULL;

	//DBG_PRINTF("vstr:%s", vstr);
	char *str= strdup(vstr);
	
	items = split_string(str, ';', vars);
	if (items >= 3){
		vlan_num = get_vlan_num(vars[2]);
		//DBG_PRINTF("vlan_num:%d", vlan_num);
		group = (switch_vlan_group_t *)malloc(sizeof_vlan_group(vlan_num));
		if (group == NULL){
			//DBG_ASSERT((group != NULL), "Alloc memory for vlan group failed");
			free(str);
			return NULL;
		}
		memset(group, 0, sizeof_vlan_group(vlan_num));
		decode_vlan_member(&group->member, vars[1]);
		group->count = decode_vlan_ids(&group->vlans[0], vars[2], vlan_num);
		// update member to each vlan
		for (i = 0; i < group->count; i ++){
		//	group->vlans[i].member = group->member;
			LgcMaskCopy(&(group->vlans[i].member),&(group->member));
		}
	}
	free(str);
	return group;
}



switch_vlan_group_t *vlan_group_new_from_hash(switch_vlan_t *vlans, int count)
{
	int i, vg_num = 0, phase = 0, s_vid = 0, v;
	switch_vlan_group_t * vg;	
		
	for (i = 0; i < count; i ++)
	{
		if ((vlans[i].id.value == 0) && (phase > 0)){
			vg_num ++;
			phase = 0;
			if (s_vid + 1 == i){
				vlans[s_vid].id.value = s_vid;
				vlans[s_vid].id.flag = 0;
			}else {
				vlans[s_vid].id_range.start = s_vid;
				vlans[s_vid].id_range.end = i - 1;
			}
		} else if ((vlans[i].id.value > 0) && (phase == 0)){
			phase = 1;
			s_vid = i;
			vlans[i].id.value = 0;
			//memset(&vlans[i], 0, sizeof(switch_vlan_t));
		} else {
			vlans[i].id.value = 0;
			//memset(&vlans[i], 0, sizeof(switch_vlan_t));
		}
	}

	if (vg_num == 0){
		return NULL;
	}
	
	vg = (switch_vlan_group_t *)malloc(sizeof_vlan_group(vg_num));

	if (vg){
		for (i = 0, v = 0; (v < vg_num) && (i < count); i ++){
			if (vlans[i].id.value > 0){
				memcpy(&vg->vlans[v], &vlans[i], sizeof(switch_vlan_t));
				v ++;
			}
		}
		vg->flag = 0;
		SetLgcMaskAll(&(vg->member));
	//	vg->member = (1 << ETH0_PORT_NUMBER) | (1 << ETH1_PORT_NUMBER) | (1 << CLT0_PORT_NUMBER) | (1 << CLT1_PORT_NUMBER);
		vg->count = v;		
	}	
	return vg;
}


/*
 test a vlan is a range or not
*/
int is_vlan_range(switch_vlan_t *vlan)
{
	if ((vlan->id_range.start > 0) && (vlan->id_range.end > 0)){
		if (vlan->id_range.start < vlan->id_range.end){
			return 1;// is a range
		}
	}
	return 0;
}


switch_vlan_group_t *vlan_group_to_next(switch_vlan_group_t *vg)
{
	char *p = (char *)vg;
	p += sizeof_vlan_group(vg->count);
	return (switch_vlan_group_t *)p;
}

uint32_t vlan_group_mem_size(switch_vlan_group_t *vg, uint16_t count)
{
	char *p = (char *)vg;
	char *pe = p;
	while(count > 0){
		pe = (char *)vlan_group_to_next((switch_vlan_group_t *)pe);
		count --;
	}
	return pe - p;
}



uint32_t sizeof_vlan_interface(switch_vlan_group_t *vlan_groups, int count)
{
	return ((int)((switch_vlan_interface_t*)0)->trunk_hybrid) + vlan_group_mem_size(vlan_groups, count);
}


switch_vlan_interface_t *vlan_interface_to_next(switch_vlan_interface_t *vi)
{
	char *p = (char *)vi;
	p += (int)(((switch_vlan_interface_t*)0)->trunk_hybrid) + vlan_group_mem_size(vi->trunk_hybrid, vi->count);
	return (switch_vlan_interface_t *)p;
}



uint32_t vlan_interface_mem_size(switch_vlan_interface_t *vi, uint16_t count)
{
	char *p = (char *)vi;
	char *pe = p;
	while(count > 0){
		pe = (char *)vlan_interface_to_next((switch_vlan_interface_t *)pe);
		count --;
	}
	return pe - p;
}

int vlan_group_sprintf(char *buffer, switch_vlan_group_t *vlan_group)
{
	int v, vlen = 0;
	
	for (v = 0; v < vlan_group->count; v ++){
		if (is_vlan_range(&vlan_group->vlans[v])){
			vlen += sprintf(buffer + vlen, "%d-%d",vlan_group->vlans[v].id_range.start,
				vlan_group->vlans[v].id_range.end);
		}else {
			vlen += sprintf(buffer + vlen, "%d", vlan_group->vlans[v].id.value);
		}
		if (v != vlan_group->count - 1){
			vlen += sprintf(buffer + vlen, "%c", ',');
		}
	}
	buffer[vlen] = 0;
	return vlen;
}


switch_vlan_interface_t *vlan_interface_new(char *name, int mode, switch_vlan_group_t *vlan0, switch_vlan_group_t * vlan1)
{		
	int count, size;
	switch_vlan_interface_t *vlanif;
	switch_vlan_group_t *hybrid_default = NULL;
	void *p;
	unsigned char buffer[sizeof_vlan_group(1)];
	
	memset(buffer, 0, sizeof(buffer));	
	hybrid_default = (switch_vlan_group_t *)buffer;
	hybrid_default->count = 1;
	hybrid_default->vlans[0].id.value = 1;			
	
	count = 0;
	size = (int)(((switch_vlan_interface_t*)0)->trunk_hybrid);
	if ((mode == LINK_TYPE_TRUNK) && (vlan0)){
		count ++;
		size += sizeof_vlan_group(vlan0->count);
	}else if (mode == LINK_TYPE_HYBRID){
	
		if (!vlan0 && !vlan1){
			vlan1 = hybrid_default;
		}
		
		if (vlan0){
			count ++;
			size += sizeof_vlan_group(vlan0->count);
		}
		if (vlan1){
			count ++;
			size += sizeof_vlan_group(vlan1->count);
		}			
	}
	vlanif = (switch_vlan_interface_t*)malloc(size);
	if (vlanif == NULL){
		DBG_ASSERT(vlanif, "Failed to malloc");
		return NULL;
	}
	memset(vlanif, 0, size);
	
	safe_strncpy(vlanif->ifname , name, sizeof(vlanif->ifname));
	vlanif->mode = mode;
//	vlanif->pvid = pvid;
	vlanif->count = count;
	if ((mode == LINK_TYPE_TRUNK) && vlan0){
		memcpy(vlanif->trunk_hybrid, vlan0, sizeof_vlan_group(vlan0->count));
	}

	if (mode == LINK_TYPE_HYBRID){
	   p = vlanif->trunk_hybrid;
	   if (vlan0){
		   memcpy(p, vlan0, sizeof_vlan_group(vlan0->count));
		   p += sizeof_vlan_group(vlan0->count);
	   }
	   if (vlan1){
		   memcpy(p, vlan1, sizeof_vlan_group(vlan1->count));
	   }
	 }
	return vlanif;
}



void vlan_interface_free(switch_vlan_interface_t *vlanif)
{
	if (vlanif != NULL){
		free(vlanif);
	}
}


int get_group_vlans_hash(switch_vlan_group_t *group, switch_vlan_t *vlans, int size)
{
	int i, v;
	int cnt = 0;
	switch_vlan_t *vlan;

	if (group == NULL) return 0;
	for (i = 0; i < group->count; i ++){
		vlan = &group->vlans[i];
		if (is_vlan_range(vlan)){
			for (v = vlan->id_range.start; (v <= vlan->id_range.end); v ++){
				if (v < size){
					vlans[v].id.value = v;
					vlans[v].id.flag = group->flag;
				//	vlans[v].member = vlan->member;
					LgcMaskCopy(&(vlans[v].member),&(vlan->member));
					cnt ++;
				}
			}
		}else {
		//	printf("i=%d value %d\n",i,vlan->id.value);
			if (vlan->id.value < size){
				vlans[vlan->id.value].id.value = vlan->id.value;
				vlans[vlan->id.value].id.flag = group->flag;
				//vlans[i].member = vlan->member;
				LgcMaskCopy(&(vlans[vlan->id.value].member),&(vlan->member));
				cnt ++;
			}		
		}
	}
	return cnt;
}


/*
 convert vlan ranges to specified vlans, 
 eg, 1-10 will fill 10 vlans as 1,2,3,4,5,6,7,8,9,10 with member = groups member
*/
int get_group_vlans(switch_vlan_group_t *group, switch_vlan_t *vlans, int size)
{
	int i, v;
	int cnt = 0;
	switch_vlan_t *vlan;

	if (group == NULL) return 0;
	for (i = 0; (i < group->count) && (cnt < size); i ++){
		vlan = &group->vlans[i];
		if (is_vlan_range(vlan)){
			for (v = vlan->id_range.start; (v <= vlan->id_range.end) && (cnt < size); v ++){
				vlans[cnt].id.value = v;
				vlans[cnt].id.flag = group->flag;
			//	vlans[cnt].member = vlan->member;
				LgcMaskCopy(&(vlans[cnt].member),&(vlan->member));
				cnt ++;
			}
		}else {
			vlans[cnt].id.value = vlan->id.value;
			vlans[cnt].id.flag = group->flag;
	//		vlans[cnt].member = vlan->member;
			LgcMaskCopy(&(vlans[cnt].member),&(vlan->member));
			cnt ++;
		}
	}
	return cnt;
}



#define TYPE_INCLUDE 0
#define TYPE_EXCLUDE 1

static int vlan_groups_compare(int type, switch_vlan_group_t *vg, switch_vlan_group_t *vgs, int vgs_count)
{
	int i, vid, cnt;
	switch_vlan_t vlans_hash[MAX_VLAN_NUMS + 1];
	switch_vlan_t vlans[MAX_VLAN_NUMS + 1];



	memset(vlans_hash, 0, sizeof(vlans_hash));		
	memset(vlans, 0, sizeof(vlans));

	if(vgs!=NULL)
	{
		for (i = 0; i < vgs_count; i ++)
		{
			get_group_vlans_hash(vgs, vlans_hash, MAX_VLAN_NUMS);
			vgs = vlan_group_to_next(vgs);			
		}	
	}
	cnt = get_group_vlans(vg, vlans, MAX_VLAN_NUMS);
	vlans[cnt].id.value = 0;

	switch(type)
	{
		case TYPE_INCLUDE:
			// every one in vg should in vgs, return 1;
			for (i = 0; vlans[i].id.value > 0; i ++){
				vid = vlans[i].id.value;
				
				if (vlans_hash[vid].id.value <= 0){					
					return 0;
				}
			}
			return 1;
		case TYPE_EXCLUDE:
			// every one in vg should not in vgs, return 1
			for (i = 0; vlans[i].id.value > 0; i ++){
				vid = vlans[i].id.value;
				if (vlans_hash[vid].id.value > 0){					
					return 0;
				}
			}
			return 1;
			
	}
	
	return 0;
}



int all_vlan_in_groups(switch_vlan_group_t *vg, switch_vlan_group_t *vgs, int vgs_count)
{
	return vlan_groups_compare(TYPE_INCLUDE, vg, vgs, vgs_count);
}

int any_vlan_in_groups(switch_vlan_group_t *vg, switch_vlan_group_t *vgs, int vgs_count)
{
	return !vlan_groups_compare(TYPE_EXCLUDE, vg, vgs, vgs_count);
}


int vlan_id_in_groups( int vlanid, switch_vlan_group_t *vgs, int vgs_count)
{
	char buf[sizeof_vlan_group(1)];
	switch_vlan_group_t *vg = (switch_vlan_group_t *)buf;
	vg->count = 1;
	vg->flag = 0;
	//vg->member = 0;
	ClrLgcMaskAll(&(vg->member));
	vg->vlans[0].id.value = vlanid;
	vg->vlans[0].id.flag = 0;
	return vlan_groups_compare(TYPE_INCLUDE, vg, vgs, vgs_count);
}




/*
int vlan_ifmode_int2str(int mode)
{
	switch(mode)
	{
		case LINK_TYPE_ACCESS:
			return "access";
		case LINK_TYPE_TRUNK:
			return "trunk";
		case LINK_TYPE_HYBRID:
			return "hybrid";
	}
	return "";
}


int vlan_ifmode_str2int(char *str)
{
	if (!strcmp(str, "access")){
		return LINK_TYPE_ACCESS;
	}else if (!strcmp(str, "trunk")){
		return LINK_TYPE_TRUNK;
	}else if (!strcmp(str, "hybrid")){
		return LINK_TYPE_HYBRID;
	}
	return LINK_TYPE_ACCESS;
}
*/


int switchport_atoi(const char *mode)
{
	if(!strcasecmp(mode, "access")) return LINK_TYPE_ACCESS;
	if(!strcasecmp(mode, "trunk")) return LINK_TYPE_TRUNK;
	if(!strcasecmp(mode, "hybrid")) return LINK_TYPE_HYBRID;
	return LINK_TYPE_ACCESS;
}

char *switchport_itoa(int mode)
{
	static char *switch_mode_string[] = {"access", "trunk", "hybrid"};
	return mode < LINK_TYPE_END ? switch_mode_string[mode] : switch_mode_string[DEFAULT_SWITCH_PORT_MODE];
}
