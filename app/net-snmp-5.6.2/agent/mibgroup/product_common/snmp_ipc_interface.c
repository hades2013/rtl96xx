
#ifndef __SNMP_IPC_INTERFACE_C__
#define  __SNMP_IPC_INTERFACE_C__

#include "snmp_ipc_interface.h"

int ipc_fd = -1;
#if 0 //add by zhouguanhua 2013/8/8
void device_template_default(ipc_service_template_t *tmpl)
{
	char name_tail[64];
	int i;
	sprintf(name_tail, "%04X%04X", (unsigned int)time(NULL), (unsigned int)tmpl);
	memset(tmpl, 0, sizeof(*tmpl));
	tmpl->device_id = 0;
	sprintf(tmpl->tmpl_name, "%s-%s", supported_devices_alias(tmpl->device_id), name_tail);
	sprintf(tmpl->tmpl_desc, "desc-%s", name_tail);
	tmpl->cable_rate_dn = 0;
	tmpl->cable_rate_up = 0;
	tmpl->mac_limit = 0;
	tmpl->lsw_cfg.broadcast_storm = 0;
	tmpl->lsw_cfg.ports = supported_devices[tmpl->device_id].ports;
	tmpl->lsw_cfg.cab_port.autoneg = 0;
	tmpl->lsw_cfg.cab_port.disabled = 0;
	tmpl->lsw_cfg.cab_port.speed = ETH_SPEED_100;
	tmpl->lsw_cfg.cab_port.duplex = ETH_DUPLEX_FULL;
	tmpl->lsw_cfg.cab_port.flowctrl = 0;
	tmpl->lsw_cfg.cab_port.priority = ETH_PRIORITY_OFF;
	tmpl->lsw_cfg.cab_port.egress_rate_limit = 0;
	tmpl->lsw_cfg.cab_port.ingress_rate_limit = 0;
	tmpl->lsw_cfg.cab_port.pvid = 1;
	tmpl->lsw_cfg.cab_port.tagged = 1;

	for (i = 0; i < tmpl->lsw_cfg.ports - 1; i ++)
	{
		tmpl->lsw_cfg.eth_ports[i].autoneg = 1;
		tmpl->lsw_cfg.eth_ports[i].disabled = 0;
		tmpl->lsw_cfg.eth_ports[i].speed = ETH_SPEED_100;
		tmpl->lsw_cfg.eth_ports[i].duplex = ETH_DUPLEX_FULL;
		tmpl->lsw_cfg.eth_ports[i].flowctrl = 0;
		tmpl->lsw_cfg.eth_ports[i].priority = ETH_PRIORITY_OFF;
		tmpl->lsw_cfg.eth_ports[i].egress_rate_limit = 0;
		tmpl->lsw_cfg.eth_ports[i].ingress_rate_limit = 0;
		tmpl->lsw_cfg.eth_ports[i].pvid = 1;
		tmpl->lsw_cfg.eth_ports[i].tagged = 0;
	}
}

void device_userlist_default(cnu_user_config_t *user)
{
	char name_tail[64];
	sprintf(name_tail, "%04X%04X", (unsigned int)time(NULL), (unsigned int)user);
//	memset(user, 0, sizeof(*user));
	memset(user->mac.octet, 0xff, sizeof(user->mac));
	user->device_id = 0;
	user->tmpl_id = 0;
	sprintf(user->name, "%s-%s", supported_devices_alias(user->device_id), name_tail);
	sprintf(user->desc, "desc-%s", name_tail);
	user->auth = CNU_AUTH_ACCEPT;
}



int ipc_get_sys_info(sys_info_t *info)
{
	ipc_sys_info_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);	
	pack = ipc_sys_info(ipc_fd);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		if (info){
			memcpy(info, &pack->info, sizeof(*info));
		}
		ret = 0;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}

int ipc_get_sys_security(sys_security_t *info)
{
	ipc_sys_security_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);
	pack = ipc_sys_security(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL, 0);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		if (info){
			memcpy(info, &pack->security, sizeof(*info));
		}
		ret = 0;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}
#endif

int ipc_get_sys_networking(sys_network_t *info)
{
	ipc_sys_network_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);
	pack = ipc_sys_networking(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		if (info){
			memcpy(info, &pack->network, sizeof(*info));
		}
		ret = 0;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}

int ipc_get_sys_time(sys_time_t *info)
{
	ipc_sys_time_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_sys_time(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		if (info){
			memcpy(info, &pack->time, sizeof(*info));
		}
		ret = 0;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_set_sys_time(sys_time_t *info)
{
	ipc_sys_time_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_sys_time(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, info);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_get_sys_admin(sys_admin_t *info)
{
	ipc_sys_admin_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_sys_admin(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, info);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		if (info){
			memcpy(info, &pack->admin, sizeof(*info));
		}
		ret = 0;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_set_sys_networking(ipc_network_t *info)
{
	ipc_sys_network_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_sys_networking(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, info);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_set_sys_security(sys_security_t *info, int mask)
{
	ipc_sys_security_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_sys_security(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, info, mask);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_set_sys_admin(sys_admin_t *info)
{
	ipc_sys_admin_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_sys_admin(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, info);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}



int ipc_system_request(int handle)
{
	ipc_system_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_system_req(ipc_fd, handle, 0);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}

#if 0 //add by zhouguanhua 2013/8/8

int ipc_set_vlan(switch_vlan_group_t *vg)
{
	ipc_clt_vlan_config_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);	
	pack = ipc_clt_vlan_config(ipc_fd, IPC_CONFIG_NEW, IPC_APPLY_NOW, vg, 1);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_del_vlan(switch_vlan_group_t *vg)
{
	ipc_clt_vlan_config_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);	
	pack = ipc_clt_vlan_config(ipc_fd, IPC_CONFIG_DEL, IPC_APPLY_NOW, vg, 1);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}



int ipc_set_vlan_mode(switch_vlan_mode_t *mode)
{
	ipc_clt_vlan_mode_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_clt_vlan_mode_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, mode);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_get_vlan_mode(switch_vlan_mode_t *mode)
{
	ipc_clt_vlan_mode_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);	
	pack = ipc_clt_vlan_mode_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW, NULL);
	
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		if (mode){
			memcpy(mode, &pack->vlan_mode, sizeof(*mode));
		}
		ret = 0;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}



int ipc_get_clt_cable_param(ipc_clt_cable_param_t *info)
{
	ipc_clt_cable_param_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);	
	pack = ipc_clt_cable_param(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, info);
	if (pack && (pack->hdr.status == IPC_STATUS_OK)) {
		if (info){
			memcpy(info, &pack->cab_param, sizeof(*info));
		}
		ret = 0;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_set_clt_cable_param(ipc_clt_cable_param_t *info)
{
	ipc_clt_cable_param_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);	
	pack = ipc_clt_cable_param(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, info);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_get_interface(switch_interface_t *info, int *ifcount)
{
	ipc_clt_interface_config_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);	
	pack = ipc_clt_interface_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL, 0);
	if (pack && (pack->hdr.status == IPC_STATUS_OK) && (pack->count > 0)) {
		if (info){
			*ifcount = pack->count;
			memcpy(info, pack->ifs, pack->count * sizeof(*info));
		}
		ret = 0;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_set_interface(switch_interface_t *info, int count)
{
	ipc_clt_interface_config_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);	
	pack = ipc_clt_interface_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, info, count);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_get_interface_statistics(char *ifname, switch_interface_mib_t *info)
{
	ipc_clt_interface_mibs_ack_t *pack;
	ipc_ifmibs_t req;
	int ret;

	safe_strncpy(req.ifname, ifname, sizeof(req.ifname));
	req.reset = 0;
	IPC_FD_ASSERT(ipc_fd);		
	pack = ipc_clt_interface_mibs(ipc_fd, IPC_MIBS_GET, &req, 1);
	if (pack && (pack->hdr.status == IPC_STATUS_OK) && (pack->count == 1)) {
		if (info){
			memcpy(info, pack->ifmibs, sizeof(*info));
		}
		ret = 0;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}

	
int ipc_get_interface_status(switch_interface_status_t *info, int *count)
{
	ipc_clt_interface_status_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_clt_interface_status(ipc_fd, NULL);
	if (pack && (pack->hdr.status == IPC_STATUS_OK) && (pack->count > 0)) {
		if (info){
			memcpy(info, pack->status, pack->count * sizeof(*info));
		}
		ret = 0;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_reset_interface_statistics(char *ifname)
{
	ipc_clt_interface_mibs_ack_t *pack;
	ipc_ifmibs_t req;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	safe_strncpy(req.ifname, ifname, sizeof(req.ifname));
	req.reset = 0;
	pack = ipc_clt_interface_mibs(ipc_fd, IPC_MIBS_RESET, &req, 1);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}




int ipc_set_vlan_interface(switch_vlan_interface_t *vlanif, int count)
{
	ipc_clt_vlan_interface_config_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_clt_vlan_interface_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, vlanif, count);

	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_get_dev_info(ipc_cnu_devinfo_t *info, int *dev_count)
{
	ipc_supported_device_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);	
	pack = ipc_supported_device(ipc_fd);
	if (pack && (pack->hdr.status == IPC_STATUS_OK)) {
		if (info && dev_count){
			memcpy(info, pack->devinfo, pack->count * sizeof(*info));
			*dev_count = pack->count;
		}
		ret = IPC_STATUS_OK;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_get_cnu_info(int clt, int cnu,  ipc_cnu_info_t *info, int *cnu_status)
{
	ipc_cnu_info_ack_t *pack;
	int ret;
	
	*cnu_status = IPC_STATUS_OK;
	IPC_FD_ASSERT(ipc_fd);		
	pack = ipc_cnu_info(ipc_fd, FILTER_TYPE_ID, clt, cnu, NULL);
	if (pack && (pack->hdr.status == IPC_STATUS_OK)) {
		if (info){
			memcpy(info, &pack->cnu_info, sizeof(*info));
		}
		ret = IPC_STATUS_OK;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		if ((pack->hdr.status == IPC_STATUS_NOCNU) 
			|| (pack->hdr.status == IPC_STATUS_NOCLT)){
			*cnu_status = pack->hdr.status;
			ret = IPC_STATUS_OK;
		}else {
			ret = pack->hdr.status;
		}
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_get_cnu_link_stats(ether_addr_t *mac,  cable_link_stats_t *info, int *cnu_status)
{
	ipc_cnu_link_stats_ack_t *pack;
	int ret;

	*cnu_status = IPC_STATUS_OK;
	IPC_FD_ASSERT(ipc_fd);		
	pack = ipc_cnu_link_stats(ipc_fd, FILTER_TYPE_MAC, 0, 0, mac);
	if (pack && (pack->hdr.status == IPC_STATUS_OK)) {
		if (info){
			memcpy(info, &pack->stats, sizeof(*info));
		}
		ret = IPC_STATUS_OK;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		if ((pack->hdr.status == IPC_STATUS_NOCNU) 
			|| (pack->hdr.status == IPC_STATUS_NOCLT)){
			*cnu_status = pack->hdr.status;
			ret = IPC_STATUS_OK;
		}else {
			ret = pack->hdr.status;
		}
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_get_cnu_port_stats(ether_addr_t *mac, ipc_cnu_port_mib_t *info, int *port_num, int *cnu_status)
{
	ipc_cnu_mib_ack_t *pack;
	int ret;

	*cnu_status = IPC_STATUS_OK;
	IPC_FD_ASSERT(ipc_fd);		
	pack = ipc_cnu_mib(ipc_fd, 0, FILTER_TYPE_MAC, 0, 0, mac);
	if (pack && (pack->hdr.status == IPC_STATUS_OK)) {
		if (info){
			*port_num = pack->ports;
			memcpy(info, pack->cnu_mib, sizeof(*info) * pack->ports);
		}
		ret = IPC_STATUS_OK;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		if ((pack->hdr.status == IPC_STATUS_NOCNU) 
			|| (pack->hdr.status == IPC_STATUS_NOCLT)
			|| (pack->hdr.status == IPC_STATUS_CNUDOWN)
			|| (pack->hdr.status == IPC_STATUS_CNUNOSUPP)
			|| (pack->hdr.status == IPC_STATUS_CNUNORESP)){
			*cnu_status = pack->hdr.status;
			ret = IPC_STATUS_OK;
		}else {
			ret = pack->hdr.status;
		}
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}



int ipc_set_template(ipc_service_template_t *tmpl, int count)
{
	ipc_service_template_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_service_template(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, tmpl, count);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_del_template(ipc_service_template_t *tmpl, int count)
{
	ipc_service_template_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_service_template(ipc_fd, IPC_CONFIG_DEL, IPC_APPLY_NOW, tmpl, count);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_new_template(int count)
{
	ipc_service_template_ack_t *pack;
	int ret, i;
	ipc_service_template_t tmpl[EOC_MAX_TMPL_NUMS];
	IPC_FD_ASSERT(ipc_fd);

	device_template_default(&tmpl[0]);
	
	for (i = 1; i < count; i ++){
		memcpy(&tmpl[i], &tmpl[0], sizeof(tmpl[0]));
	}

	pack = ipc_service_template(ipc_fd, IPC_CONFIG_NEW, IPC_APPLY_NOW, tmpl, count);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_set_userlist(cnu_user_config_t *user, int count)
{
	ipc_cnu_user_config_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_cnu_user_config(ipc_fd, FILTER_TYPE_MAC, IPC_CONFIG_SET, IPC_APPLY_NOW, user, count);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_del_userlist(cnu_user_config_t *user, int count)
{
	ipc_cnu_user_config_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_cnu_user_config(ipc_fd, FILTER_TYPE_MAC, IPC_CONFIG_DEL, IPC_APPLY_NOW, user, count);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_new_userlist(cnu_user_config_t *info, int count)
{
	ipc_cnu_user_config_ack_t *pack;
	int ret;
	IPC_FD_ASSERT(ipc_fd);	
	pack = ipc_cnu_user_config(ipc_fd, FILTER_TYPE_MAC, IPC_CONFIG_NEW, IPC_APPLY_NOW, info, count);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}

/*
int ipc_new_userlist(int count)
{
	ipc_cnu_user_config_ack_t *pack;
	int ret, i;
	cnu_user_config_t user[EOC_MAX_USER_NUMS];
	IPC_FD_ASSERT(ipc_fd);

	device_userlist_default(&user[0]);
	
	for (i = 1; i < count; i ++){
		memcpy(&user[i], &user[0], sizeof(user[0]));
	}

	pack = ipc_cnu_user_config(ipc_fd, FILTER_TYPE_MAC, IPC_CONFIG_DEL, IPC_APPLY_NOW, user, count);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}
*/


/*
void clt_vlan_group_dump(switch_vlan_group_t *vlan_groups, uint16_t count)
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

*/

#if 0
int vlan_string_check(const char *vstr)
{
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

#endif

int all_vlan_exist(switch_vlan_group_t *vg, int *ipc_assert)
{
	int ipc = IPC_STATUS_OK, ret = 0;
	ipc_clt_vlan_config_ack_t *pack;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_clt_vlan_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW, NULL, 0);
	
	if (pack && (pack->hdr.status == IPC_STATUS_OK)) {
		ret = all_vlan_in_groups(vg, pack->vlan_groups, pack->count);

	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		ipc = pack->hdr.status;
	}else {
		ipc = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);

	if (ipc_assert) *ipc_assert = ipc;

	return ret;	
}

int vlan_exist(int vid, int *ipc_assert)
{
	char buffer[sizeof_vlan_group(1)];
	switch_vlan_group_t *vg = (switch_vlan_group_t *)buffer;
	memset(buffer, 0, sizeof(buffer));
	vg->count = 1;
	vg->vlans[0].id.flag = 0;
	vg->vlans[0].id.value = vid;
	return all_vlan_exist(vg, ipc_assert);
}

switch_vlan_mode_t sys_vlan_mode;
switch_vlan_t dot1q_vlans[EOC_MAX_VLAN_NUMS];
switch_vlan_t dot1q_vlans_u[EOC_MAX_VLAN_NUMS]; // record untagged portlist

int dot1q_vlans_num = 0; 
switch_vlan_interface_t *dot1q_vlan_interfaces[MAX_CLT_ETH_PORTS - 1];



int transparent_vlanif_search(char *ifname)
{
	int i;
	for (i = 0; i < MAX_CLT_ETH_PORTS; i ++){
		if (sys_vlan_mode.ports[i].ifname[0] 
			&& (!strcmp(ifname, sys_vlan_mode.ports[i].ifname))){
			return sys_vlan_mode.ports[i].type;
		}
	}
	return 0;
}
 

void transparent_vlanif_set(char *ifname, int type)
{
	int i;
	for (i = 0; i < MAX_CLT_ETH_PORTS; i ++){
		if (sys_vlan_mode.ports[i].ifname[0] 
			&& (!strcmp(ifname, sys_vlan_mode.ports[i].ifname))){
			sys_vlan_mode.ports[i].type = type;
		}
	}
}


switch_vlan_interface_t *dot1q_vlanif_search(char *ifname)
{
	int i;
	switch_vlan_interface_t * vlanif;
	for (i = 0; i < MAX_CLT_ETH_PORTS - 1; i ++)
	{
		vlanif = dot1q_vlan_interfaces[i];
		if (vlanif && !strcmp(vlanif->ifname, ifname)){
			return vlanif;
		}
	}
	return NULL;
}

switch_vlan_interface_t *dot1q_vlanif_dup(char *ifname)
{
	switch_vlan_interface_t *vlanif, *vi;	
	int size;
	vlanif = dot1q_vlanif_search(ifname);
	if (vlanif){
		size = sizeof_vlan_interface(vlanif->trunk_hybrid, vlanif->count);
		vi =  (switch_vlan_interface_t *)malloc(size);
		memcpy(vi, vlanif, size);
		return vi;
	}
	return NULL;
}





static int dot1q_vlan_update(void)
{
	int ipc = IPC_STATUS_OK, ret = 0, i;
	ipc_clt_vlan_config_ack_t *pack;
	switch_vlan_group_t *vg;
	IPC_FD_ASSERT(ipc_fd);

	pack = ipc_clt_vlan_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW, NULL, 0);
	
	if (pack && (pack->hdr.status == IPC_STATUS_OK)) {

		memset(dot1q_vlans, 0, sizeof(dot1q_vlans));
		dot1q_vlans_num = 0;
		vg = pack->vlan_groups;
		for (i = 0; i < pack->count; i ++){
			dot1q_vlans_num += get_group_vlans_hash(vg, dot1q_vlans, EOC_MAX_VLAN_NUMS);
			vg = vlan_group_to_next(vg);
		}
		ret = 0;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		ipc = pack->hdr.status;
	}else {
		ipc = IPC_STATUS_FAIL;
	}
	if(pack) free(pack);
	return ret;	
}



static int dot1q_vlan_interface_update(void)
{
	int ipc = IPC_STATUS_OK, ret = 0, i;
	ipc_clt_vlan_interface_config_ack_t *pack;
	switch_vlan_interface_t *vlanif, *vi;
	IPC_FD_ASSERT(ipc_fd);

	
	

	pack = ipc_clt_vlan_interface_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW, NULL, 0);
	if (pack && (pack->hdr.status == IPC_STATUS_OK)) {
		for (i = 0; i < sizeof(dot1q_vlan_interfaces)/ sizeof(dot1q_vlan_interfaces[0]); i ++){
			if (dot1q_vlan_interfaces[i]){
				free(dot1q_vlan_interfaces[i]);
			}
		}
		memset(dot1q_vlan_interfaces, 0, sizeof(dot1q_vlan_interfaces));
		vlanif = pack->vlanifs;
		for (i = 0; (i < pack->count) && (i < MAX_CLT_ETH_PORTS - 1); i ++){
			vi = (switch_vlan_interface_t *)malloc(vlan_interface_mem_size(vlanif, 1));
			if (vi == NULL){
				continue;
			}
			memcpy(vi, vlanif, vlan_interface_mem_size(vlanif, 1));
			dot1q_vlan_interfaces[i] = vi;
			vlanif = vlan_interface_to_next(vlanif);
		}
		ret = 0;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		ipc = pack->hdr.status;
	}else {
		ipc = IPC_STATUS_FAIL;
	}
	if(pack) free(pack);
	return ret;	
}


int dot1q_vlan_untagged_portlist_update(void)
{
	int i, v, cnt, vid;
	switch_vlan_interface_t *vlanif;
	switch_vlan_group_t *vg;
	char *show_if[] = {VLAN_IFLIST, NULL};
	switch_vlan_t vlans[EOC_MAX_VLAN_NUMS];

	memset(dot1q_vlans_u, 0, sizeof(dot1q_vlans_u));	

	for (i = 0; i < EOC_MAX_VLAN_NUMS; i ++){
		dot1q_vlans_u[i].id.value = dot1q_vlans[i].id.value;
	}
	
	for (i  = 0; show_if[i]; i ++){
		
		vlanif = dot1q_vlanif_search(show_if[i]);
		if (!vlanif){
			continue;
		}
		switch(vlanif->mode){
			case ETH_SWITCHPORT_ACCESS:

				if (dot1q_vlans_u[vlanif->pvid].id.value > 0){
					dot1q_vlans_u[vlanif->pvid].member |= (1 << vlanif->phy);
				}
				break;
			case ETH_SWITCHPORT_TRUNK:
				if ((vlanif->count == 0) && (dot1q_vlans_u[vlanif->pvid].id.value > 0)){
					dot1q_vlans_u[vlanif->pvid].member |= (1 << vlanif->phy);
				}else if(vlan_id_in_groups(vlanif->pvid, vlanif->trunk_hybrid, vlanif->count)){
					dot1q_vlans_u[vlanif->pvid].member |= (1 << vlanif->phy);
				}
								
				break;
			case ETH_SWITCHPORT_HYBRID:
				vg = vlanif->trunk_hybrid;
				for (v = 0; v < vlanif->count; v ++){
					if (!(vg->flag & VLAN_FLAG_TAG)){
						break;
					}
					vg = vlan_group_to_next(vg);
				}
				
				if (v < vlanif->count){
					cnt = get_group_vlans(vg, vlans, EOC_MAX_VLAN_NUMS - 1);
					for (v = 0; v < cnt; v ++){
						vid = vlans[v].id.value;
						if (dot1q_vlans_u[vid].id.value > 0){
							dot1q_vlans_u[vid].member |= (1 << vlanif->phy);
						}
					}	
				}
				break;
		}	
	}
	return 0;
}




int dot1q_vlan_commit(void)
{
	switch_vlan_group_t *vg;

	vg = vlan_group_new_from_hash(dot1q_vlans, EOC_MAX_VLAN_NUMS);
	if (vg){
		return ipc_set_vlan(vg);		
	}
	return 0;
}



switch_vlan_group_t *vlan_group_get(int untag, vlan_member_t member)
{
	int i = 0;
	switch_vlan_t vlans[EOC_MAX_VLAN_NUMS];
	memset(vlans, 0, sizeof(vlans));	

	if (untag){
		for (i = 0; i < EOC_MAX_VLAN_NUMS; i ++){
			if ((dot1q_vlans[i].id.value > 0) && (dot1q_vlans[i].member & member)){
				vlans[i].id.value = dot1q_vlans[i].id.value;
			}
		}
	}else {
		for (i = 0; i < EOC_MAX_VLAN_NUMS; i ++){
			if ((dot1q_vlans[i].id.value > 0) && (dot1q_vlans_u[i].member & member)){
				vlans[i].id.value = dot1q_vlans[i].id.value;
			}
		}
	}

	return vlan_group_new_from_hash(vlans, EOC_MAX_VLAN_NUMS);
}




int dot1q_vlan_interface_commit(void)
{
	// parse vlan member for trunk port and hybrid port

	int i, ret;
	char *show_if[] = {VLAN_IFLIST, NULL};
	switch_vlan_interface_t *vlanifs[MAX_CLT_ETH_PORTS], *vlanif,  *vi, *v;
	switch_vlan_group_t *vg_tag, *vg_untag;
	int vlanif_size, vlanif_cnt;


	memset(vlanifs, 0, sizeof(vlanifs));
	vlanif_size = 0;
	vlanif_cnt = 0;
	
	for (i  = 0; show_if[i]; i ++){
		vlanif = dot1q_vlanif_search(show_if[i]);
		if (!vlanif){
			continue;
		}

		if (vlanif->mode == ETH_SWITCHPORT_TRUNK){
			vg_tag = vlan_group_get( 0, 1 << vlanif->phy);
			vlanifs[i] = vlan_interface_new(vlanif->ifname, vlanif->mode, vg_tag, NULL);
			vlanif_size += sizeof_vlan_interface(vlanifs[i]->trunk_hybrid, vlanifs[i]->count);
			vlanif_cnt ++;
			if (vg_tag){
				free(vg_tag);
				vg_tag = NULL;
			}
		}else if (vlanif->mode == ETH_SWITCHPORT_HYBRID){
			vg_tag = vlan_group_get(0, 1 << vlanif->phy);
			vg_untag = vlan_group_get(1, 1 << vlanif->phy);
			vlanifs[i] = vlan_interface_new(vlanif->ifname, vlanif->mode, vg_tag, vg_untag);
			vlanif_size += sizeof_vlan_interface(vlanifs[i]->trunk_hybrid, vlanifs[i]->count);
			vlanif_cnt ++;			
			if (vg_tag){
				free(vg_tag);
				vg_tag = NULL;
			}
			if (vg_untag){
				free(vg_untag);
				vg_untag = NULL;
			}
		}
	}	

	if ((vlanif_size > 0) && (vlanif_cnt > 0)){
		vi = (switch_vlan_interface_t *)malloc(vlanif_size);

		if (vi == NULL){
			for (i = 0; i < MAX_CLT_ETH_PORTS; i ++){
				if (vlanifs[i]){
					free(vlanifs[i]);
				}
			}
			HC_NOMEM_ASSERT(vi);
		}
		v = vi;
		for (i = 0; i < MAX_CLT_ETH_PORTS; i ++){
			if (vlanifs[i]){
				memcpy(v, vlanifs[i], sizeof_vlan_interface(vlanifs[i]->trunk_hybrid,vlanifs[i]->count));
				v = vlan_interface_to_next(v);
			}
		}
		// free all vlanif 
		for (i = 0; i < MAX_CLT_ETH_PORTS; i ++){
			if (vlanifs[i]){
				free(vlanifs[i]);
			}
		}
		// commit all
		ret = ipc_set_vlan_interface(vi, vlanif_cnt);
		free(vi);
		return ret;
	}

	return IPC_STATUS_OK;
}



int vlan_update(void)
{
	int ret;

	ret = ipc_get_vlan_mode(&sys_vlan_mode);
	if (ret != IPC_STATUS_OK){
		return ret;
	}

	if (sys_vlan_mode.mode == VLAN_MODE_8021Q){
		return dot1q_vlan_update();
	}else {
		dot1q_vlans_num = 0;
	}	
	return IPC_STATUS_OK;
}


int vlan_interface_update(void)
{
	int ret;

	ret = ipc_get_vlan_mode(&sys_vlan_mode);
	if (ret != IPC_STATUS_OK){
		return ret;
	}

	if (sys_vlan_mode.mode == VLAN_MODE_8021Q){
		return dot1q_vlan_interface_update();
	}
	
	return IPC_STATUS_OK;
}





char *snmp_strdup(unsigned char *val, int val_len)
{
	static char buffer[1024];
	if (val_len > sizeof(buffer) - 1){
		val_len = sizeof(buffer) - 1;
	}
	strncpy(buffer, (char *)val, val_len);
	buffer[val_len] = 0;
	return buffer;
}


char *interface_fancy_name(char *ifname)
{
	if(!strcmp(ifname, IFNAME_ETH0)) return "Ethernet 1";
	if(!strcmp(ifname, IFNAME_ETH1)) return "Ethernet 2";
	if(!strcmp(ifname, IFNAME_CAB0)) return "Cable 1";
	if(!strcmp(ifname, IFNAME_CAB1)) return "Cable 2";
	return "";
}


int valid_xname(char *n)
{
	
	while(*n){
		if ((*n == ' ') || (*n == ';') || (*n == '"') || (*n == '\'') || (*n == '\\')){
			return 0;
		}
		n ++;
	}
	return 1;
}


int valid_description(char *n)
{
	while(*n){
		if ((*n == ';') || (*n == '"') || (*n == '\'') || (*n == '\\')){
			return 0;
		}
		n ++;
	}
	return 1;
}


ipc_cnu_devinfo_t supported_devices[MAX_SUPPORTED_CNU_MODELS];
int supported_devices_num = 0;


int supported_devices_update(void)
{
	return ipc_get_dev_info(supported_devices, &supported_devices_num);
}


char *supported_devices_alias(int id_b0)
{
	if (id_b0 < supported_devices_num){
		return supported_devices[id_b0].alias;
	}
	return "";
}



int supported_devices_id_b0(char *alias)
{
	int i;
	for (i = 0; i < supported_devices_num; i ++){
		if (!strcmp(alias, supported_devices[i].alias)){
			break;
		}
	}
	return i;
}



/*
TODO: Default Template name disabled modification in web 
*/

ipc_service_template_t device_templates[EOC_MAX_TMPL_NUMS];
int device_templates_num = 0;


int device_templates_update(void)
{
	long			ret = -1;
  
	ipc_service_template_ack_t *pack;

	IPC_FD_ASSERT(ipc_fd);
	device_templates_num = 0;
	pack = ipc_service_template(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL, 0);
	if (pack && (pack->hdr.status == IPC_STATUS_OK) 
		&& (pack->count > 0) && (pack->count <= EOC_MAX_TMPL_NUMS)){
		memcpy(device_templates, pack->templates, pack->count * sizeof(device_templates[0]));
		device_templates_num = pack->count;
		ret = IPC_STATUS_OK;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	free(pack);	
	
	return ret; 
} 


int valid_template_name(char *v)
{
//	int i;
//	char *val;
	if (!valid_xname(v)){
		return 0;
	}
//	for (i = 0; i < device_templates_num; i ++){
//		if (!strcmp(device_templates[i].tmpl_name, v)){
//			return 0;
//		}
//	}
	return 1;
}


cnu_user_config_t device_userlists[EOC_MAX_USER_NUMS];
int device_userlists_num = 0;


int device_userlists_update(void)
{
	long			ret = -1;
  
	ipc_cnu_user_config_ack_t *pack;
	IPC_FD_ASSERT(ipc_fd);


	device_userlists_num = 0;
	pack = ipc_cnu_user_config(ipc_fd, FILTER_TYPE_NONE, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL, 0);
	if (pack && (pack->hdr.status == IPC_STATUS_OK) 
		&& (pack->count > 0) && (pack->count <= EOC_MAX_USER_NUMS)){
		memcpy(device_userlists, pack->user, pack->count * sizeof(device_userlists[0]));
		device_userlists_num = pack->count;
		ret = IPC_STATUS_OK;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	free(pack);	
	
	return ret; 
} 

cnu_user_config_t *device_userlists_search(ether_addr_t *mac)
{
	int i;
	for (i = 0; i < device_userlists_num; i ++)
	{
		if (!memcmp(device_userlists[i].mac.octet, mac->octet, sizeof(*mac))){
			return &device_userlists[i];
		}
	}
	return NULL;
}


int valid_userlist(cnu_user_config_t *user)
{
	if (device_userlists_search(&user->mac)){
		snmp_log(LOG_ERR, "user:mac address exist\n");
		return 0;
	}

	if (user->tmpl_id >= device_templates_num){
		snmp_log(LOG_ERR, "user: tmpl_id is too big\n");
		return 0;
	}

	if (device_templates[user->tmpl_id].device_id != user->device_id){
		snmp_log(LOG_ERR, "user: tmpl_id and dev_id unmatch\n");		
		return 0;
	}

	return 1;
}



void userlist_new_init(cnu_user_config_t *user, ether_addr_t *mac, char *model)
{
	memcpy(user->mac.octet, mac->octet, sizeof(ether_addr_t));
	user->auth = CNU_AUTH_ACCEPT;
	user->device_id = supported_devices_id_b0(model);
	if (user->device_id >= supported_devices_num){
		user->device_id = 0;// set to 0
	}
	user->tmpl_id = user->device_id;
	sprintf(user->name, "snmp%02X%02X%02X", mac->octet[3], mac->octet[4], mac->octet[5]);
	sprintf(user->desc, "desc%02X%02X%02X", mac->octet[3], mac->octet[4], mac->octet[5]);
}




ipc_cnu_info_t device_topology[MAX_TOPOLOGY_NUM];
int device_topology_num = 0;
struct device_stats *device_stats_head = NULL;


int device_topology_update(int update)
{
	long			ret = -1;
  
	ipc_network_info_ack_t *pack;
	IPC_FD_ASSERT(ipc_fd);

	device_topology_num = 0;
	pack = ipc_network_info(ipc_fd, 0xff, update);
	if (pack && (pack->hdr.status == IPC_STATUS_OK) 
		&& (pack->count > 0) && (pack->count <= MAX_TOPOLOGY_NUM)){
		memcpy(device_topology, pack->cnus, pack->count * sizeof(device_topology[0]));
		device_topology_num = pack->count;
		ret = IPC_STATUS_OK;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	free(pack);	
	
	return ret; 
} 


struct device_stats *device_stats_search(ether_addr_t *mac)
{
	struct device_stats *ptr = 	device_stats_head;
	while(ptr != NULL){
		if (!memcmp(ptr->mac_address.octet, mac->octet, sizeof(ether_addr_t))){
			return ptr;
		}
		ptr = ptr->next;
	}
	return NULL;
}

void device_stats_init(struct device_stats *stats, ipc_cnu_info_t *dev)
{
	memset(stats, 0, sizeof(*stats));
	memcpy(stats->mac_address.octet, dev->mac.octet, sizeof(ether_addr_t));
}


int device_link_stats_update(ether_addr_t *mac)
{
	long			ret = -1;
	cable_link_stats_t link_stats;	
	struct device_stats * n;	
	int cnu_status = 0;

	// reset valid flag if any
	n = device_stats_search(mac);	
	if (n) n->valid_link_stats = 0;

	if (n){
		//DEBUGMSGTL(("hexicom", "device_stats_search get :%s\n", mac2str(mac)));		
	}
    // get  
	ret = ipc_get_cnu_link_stats(mac, &link_stats, &cnu_status);

	//DEBUGMSGTL(("hexicom", "ipc_get_cnu_link_stats return ret=%d, cnu_status=%d\n", ret, cnu_status));		

	
	HC_CNU_ASSERT(cnu_status);
	HC_IPC_ASSERT(ret);

	if (!n) {
		n = (struct device_stats *)malloc(sizeof(struct device_stats));
		HC_NOMEM_ASSERT(n);
		memset(n, 0, sizeof(struct device_stats));
		memcpy(n->mac_address.octet, mac->octet, sizeof(ether_addr_t));
		n ->next = device_stats_head;
		device_stats_head = n;
	}
	// update
	memcpy(&n->link_stats, &link_stats, sizeof(link_stats));
	n->valid_link_stats = 1;

	//DEBUGMSGTL(("hexicom", "valid_link_stats = 1\n"));		

	
	return ret;
}


int device_port_mibs_update(ether_addr_t *mac)
{
	long			ret = -1;
	int cnu_status = 0;
	ipc_cnu_port_mib_t ports[CNU_MAX_ETH_PORTS];
	int port_num = 0;
	struct device_stats * n;	

	// reset valid flag if any
	n = device_stats_search(mac);	
	if (n) n->valid_port_mibs = 0;	
  
	ret = ipc_get_cnu_port_stats(mac, ports, &port_num, &cnu_status);
	HC_CNU_ASSERT(cnu_status);
	HC_IPC_ASSERT(ret);

	if (!n){
		n = (struct device_stats *)malloc(sizeof(struct device_stats));
		HC_NOMEM_ASSERT(n);
		memset(n, 0, sizeof(struct device_stats));
		memcpy(n->mac_address.octet, mac->octet, sizeof(ether_addr_t));
		n ->next = device_stats_head;
		device_stats_head = n;
	}
	n->port_num = port_num;
	memcpy(n->ports, &ports, sizeof(ports[0]) * port_num);	
	return ret;
}

#endif

void init_snmp_ipc_interface(void)
{		
	//DEBUGMSGTL(("snmp_ipc", "Initializing\n"));
	ipc_fd = ipc_client_connect("snmpd");
	//snmp_log(LOG_INFO, "snmp IPC connected FD=%d\n", ipc_fd);
//	memset(dot1q_vlan_interfaces, 0, sizeof(dot1q_vlan_interfaces));
}


#endif /*#ifndef __SNMP_IPC_INTERFACE_C__*/


