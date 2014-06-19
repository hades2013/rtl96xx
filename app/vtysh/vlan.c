/*
 * vlan.c
 *
 *  Created on: Mar 10, 2011
 *      Author: root
 */

#include <zebra.h>

#include "command.h"
#include "if.h"
#include "vty.h"
#include <ipc_client.h>
#include <cable.h>
#include <debug.h>
#include <switch.h>
// #include <vlan_utils.h>
#include "demo_interface.h"

extern int ipc_fd;




static char *vlan_member_string(uint32_t member)
{
	static char buf[80];

	buf[0] = 0;
	if(member & (1 << CLT0_PORT_NUMBER)) strcat(buf, "Cable 1/1 ");
	if(member & (1 << CLT1_PORT_NUMBER)) strcat(buf, "Cable 1/2 ");
	if(member & (1 << ETH0_PORT_NUMBER)) strcat(buf, "Ethernet 1/1 ");
	if(member & (1 << ETH1_PORT_NUMBER)) strcat(buf, "Ethernet 1/2 ");
	return buf;
}

#define VTY_MAX_VLAN_STRING_SIZE_PER_LINE 50

static void vty_out_vlans(struct vty* vty, const char *prefix, switch_vlan_group_t *vg)
{
	int i;
	char buffer[4094*4];
	char *cp, *prev, *p;
	char bfix[256];

	for (i = 0; (i < sizeof(bfix) - 1) && (i < strlen(prefix)); i ++){
		bfix[i] = ' ';
	}
	bfix[i] = '\0';

	vlan_group_sprintf(buffer, vg);
	p = prev = buffer;
	while((cp = strchr(p, ',')) != NULL){
		if (cp - prev > VTY_MAX_VLAN_STRING_SIZE_PER_LINE){
			*cp = 0;
			vty_out(vty, "%s %s%s", (prev == buffer) ? prefix : bfix, prev, VTY_NEWLINE);
			prev = cp + 1;
		}
		p = cp + 1;
	}
	// don't forget the last line
	vty_out(vty, "%s %s%s", (prev == buffer) ?  prefix : bfix, prev, VTY_NEWLINE);
}


static int vlan_item_check(const char *item)
{
	int mi, ma, ret = 0;
	char *idstr = strdup(item);
	char *p = strchr(idstr, '-');
	if (p != NULL){
		*p = 0;
		mi = strtoul(idstr, NULL, 0);
		ma = strtoul(p + 1, NULL, 0);
		if ((mi <= ma) && (mi > 0) && (ma < 4095)){
			ret = 1;
		}
	}else {
		mi = strtoul(idstr, NULL, 0);
		if (mi > 0 && mi < 4095){
			ret = 1;
		}
	}
	free(idstr);
	return ret;
}


static int vlan_string_check(const char *vstr)
{
	char *vp;
	char *vars[4096];
	int items, i, ret = 1;
	char *vs = strdup(vstr);
	char *p, *pm, *pn;
/*
	var vstr= /^((\d{1,4}\-\d{1,4}|\d{1,4})\,)*(\d{1,4}\-\d{1,4}|\d{1,4})$/; 
	if (!vstr.test(s)) return false;
*/
	// Step 1, string check
	p = vs;
	do {
		pm = strchr(p, ',');
		pn = strchr(p, '-');
		if ((pn != NULL) && (pm != NULL)){
			pm = (pn < pm) ? pn : pm;
		}else if ((pn != NULL) && (pm == NULL)){
			pm = pn;
		}
		if (pm != NULL){
			if (pm == vs)ret = 0;
			if ((pm - p) > 4)ret = 0;
			for (pn = p; pn < pm; pn ++){
				if (!isdigit(*pn)) { ret = 0; break;}
			}
			if (*(pm + 1) == '\0') ret = 0;
			p = pm + 1;	
		}else {
			for (pn = p; *pn > 0; pn ++){
				if (!isdigit(*pn)) { ret = 0; break;}
			}			
		}
	}
	while((pm != NULL) && (ret > 0));
	

	// Step 2, items check
	if (ret > 0){
		items = split_string(vs, ',', vars);	
		for (i = 0; i < items; i ++){
			if (!vlan_item_check(vars[i])){
				ret = 0;
				break;
			}
		}
	}
	
	free(vs);
	return ret;
}

static void clt_vlan_group_dump(switch_vlan_group_t *vlan_groups, uint16_t count)
{
	int i, v;
	switch_vlan_group_t *group;
	group = vlan_groups;
	for (i = 0; i < count; i ++){
		fprintf(stderr, "Group<%d>\n", i);
		fprintf(stderr, "\tMember:%s\n", vlan_member_string(group->member));
		fprintf(stderr, "\tVlans :");
		for (v = 0; v < group->count; v ++){
			if (is_vlan_range(&group->vlans[v])){
				fprintf(stderr, "%d-%d[%X],", 
					group->vlans[v].id_range.start,
					group->vlans[v].id_range.end,
					group->vlans[v].member);
			}else{
				fprintf(stderr, "%d[%X]%s,", 
					group->vlans[v].id.value, 
					group->vlans[v].member,
					group->vlans[v].id.flag ? "*" : "");
			}
			if (v && !(v & 0x0f)) fprintf(stderr, "\n\t       ");
		}
		fprintf(stderr, "\n");			
		group = vlan_group_to_next(group);
	}
}

void show_vlan(struct vty* vty, switch_vlan_group_t *vlan_group)
{
	int i;
	ipc_clt_vlan_config_ack_t *pack = NULL;
	switch_vlan_group_t *vg, *vg_unexist = NULL;

	pack =  ipc_clt_vlan_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, vlan_group, (vlan_group ? 1 : 0));
	IPC_ACK_NULL_ASSERT(pack);
	if(IPC_ACK_OK(pack)) {
		vty_out(vty, "VLAN information:%s", VTY_NEWLINE);
		vty_out(vty, "----------------------------------------%s", VTY_NEWLINE);
		vg = pack->vlan_groups;
		for(i = 0; i < pack->count; i ++) {
			if (vg->member > 0) {
				vty_out_vlans(vty, "VLAN       :", vg);			
				vty_out(vty, "Ports      : %s%s", vlan_member_string(vg->member), VTY_NEWLINE);
				vty_out(vty, "VLAN alias : %s%s",  "default", VTY_NEWLINE);
				vty_out(vty, "----------------------------------------%s", VTY_NEWLINE);
			}else {
				vg_unexist = vg;
			}
			vg = vlan_group_to_next(vg);
		}
		if (vg_unexist){
			vty_out(vty, "VLAN that do not exist :%s", VTY_NEWLINE);
			vty_out_vlans(vty, "  ", vg_unexist);
			vty_out(vty, "----------------------------------------%s", VTY_NEWLINE);
		}
		vty_out(vty, "Total %d VLAN Groups%s", vg_unexist ? pack->count - 1 : pack->count , VTY_NEWLINE);
	}

	if(pack) free(pack);
	
}

DEFUN (vlan_show,
		vlan_show_cmd,
		"show vlan (all|<1-4094>|RANGE)",
		SHOW_STR
		"Show VLAN configuration information\n"
		"Show all VLAN information\n"
		"Show VLAN information of VLAN ID\n"
		"Show VLAN information of VLAN ID range\n")
{
	char *cp;
	switch_vlan_group_t *vg;
	if (!strcmp(argv[0], "all")){
		show_vlan(vty, NULL);
		return CMD_SUCCESS;
	}else if (vlan_string_check(argv[0])){
		vg = vlan_group_new_from_string_simple(argv[0]);
		//clt_vlan_group_dump(vg, 1);
		show_vlan(vty, vg);
		if (vg) free(vg);
		return CMD_SUCCESS;
	}

	vty_out(vty, "Invalid VLAN identifier.%s", VTY_NEWLINE);
	return CMD_SUCCESS;
}

DEFUN(switchport_show,
		switch_port_show_eth_cmd,
		"show switchport interface ethernet IFNAME",
		SHOW_STR
		"Display switchport information\n"
		"Interface information\n"
		"Ethernet interface\n"
		"Interface's name")
{
	switch_vlan_interface_t *vif;
	int ret, i;
	switch_vlan_group_t *vg, *vgt = NULL, *vgu = NULL;
	char buffer[4094*4];
	char *cp, *prev, *p;

	//vif = get_interface_config(argv[0], self->string);
	if(vif == NULL) {
		vty_out(vty, "Fail to get interface status%s", VTY_NEWLINE);
		goto out;
	}

	vty_out(vty, "%s%s", if_fancy_name(vif->ifname), VTY_NEWLINE);
	vty_out(vty, "MAC address limit: No limit%s", VTY_NEWLINE);
	vty_out(vty, "Mode: %s%s", if_fancy_mode(vif->mode), VTY_NEWLINE);
	vty_out(vty, "Port VID: %d%s", vif->pvid, VTY_NEWLINE);


	if (vif->count > 0){
		vg = vif->trunk_hybrid;		
		if (vif->mode == ETH_SWITCHPORT_TRUNK){
			vgt = vg;
		}else {// for hybrid
			for (i = 0; i < vif->count; i ++){
				if (vlan_tagged(vg->flag)){
					vgt = vg;
				}else {
					vgu = vg;
				}
				vg = vlan_group_to_next(vg);
			}			
		}
	}

	switch(vif->mode) {
	case ETH_SWITCHPORT_TRUNK:
		if(vgt == NULL) {
			vty_out(vty, "Trunk allowed Vlan: 1-4094%s", VTY_NEWLINE);
		}
		else {
			vty_out_vlans(vty, "Trunk allowed Vlan:", vgt);		
		}
		break;
	case ETH_SWITCHPORT_HYBRID:
		if(vgt == NULL) {
			vty_out(vty, "Hybrid Tag Vlan: %s", VTY_NEWLINE);
		}
		else {
			vty_out_vlans(vty, "Hybrid Tag Vlan:", vgt);		
		}
		if(vgu == NULL) {
			vty_out(vty, "Hybrid Untag Vlan: %s", VTY_NEWLINE);
		}
		else {
			vty_out_vlans(vty, "Hybrid Untag Vlan:", vgu);		
		}		

		break;
	}
	
out:
	return CMD_SUCCESS;
}

ALIAS(switchport_show,
		switch_port_show_cab_cmd,
		"show switchport interface cable IFNAME",
		SHOW_STR
		"Display switchport information\n"
		"Interface information\n"
		"Cable interface\n"
		"Interface's name")

void vlan_cmd_init(void)
{
/*	install_element(ENABLE_NODE, &vlan_show_cmd);
	install_element(ENABLE_NODE, &switch_port_show_eth_cmd);
	install_element(ENABLE_NODE, &switch_port_show_cab_cmd);*/
//	install_element(ENABLE_NODE, &show_cnu_info_cmd);
//	install_element(ENABLE_NODE, &show_cnu_stat_cmd);
//	install_element(ENABLE_NODE, &clean_cnu_stat_cmd);
}
