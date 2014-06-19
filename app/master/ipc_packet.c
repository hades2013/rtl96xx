/*
 * ipc_packet.c
 *
 *  Created on: Dec 29, 2010
 *      Author: Ryan
 */

#include "master.h"
//#include "cltmon.h"
#include "sysmon.h"
//#include "mme_generic.h"
#include "ipc.h"
#include <ipc_protocol.h>
#include <hex_utils.h>
#include <str_utils.h>
//#include <vlan_utils.h>
//#include <cnu_service.h>
//#include <switch.h>
#include "sys_upgrade.h"
#include <time_zone.h>
#include <lw_config.h>
#include <memshare.h>
#include <lw_config_oid.h>
#include <lw_config_api.h>
#include <sys_vlan.h>
#include "lw_drv_req.h"
//#include <vos_config.h>
/*Begin modified by sunmingliagn for bug 199*/
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <ipc_protocol.h>
/*End modified by sunmingliagn for bug 199*/
#include <systemlog.h>			//add by leijinbao 2013/9/17 for USER_LOG
#include <ipc_protocol.h>

extern int cfg_restore_for_file(char *file);
extern int vosConfigParserFile(char * pPath,CFG_DOMAIN_E enDomain);
extern int vosConfigDomainDelete(CFG_DOMAIN_E enDomain);
extern void master_cfg_do_commit(void);
extern int master_cfg_backup(const char *file);
extern int master_cfg_setval(int ifindex,unsigned int oid,void *val);
extern int master_cfg_del(int ifindex,unsigned int oid);
extern uint32_t vlan_interface_mem_size(switch_vlan_interface_t *vi, uint16_t count);



extern uint32_t vlan_group_mem_size(switch_vlan_group_t *vg, uint16_t count);



//#define	IPC_HEADER_CHECK(req, t)	(req->hdr.msg_len < sizeof(t) - sizeof(ipc_request_hdr_t))

#define	IPC_DECLARE_HEADER(type) \
	type *ack = NULL; \
	ipc_acknowledge_hdr_t hdr;

// Send acknowledge and return
#if 1
#define	IPC_ACK(len) \
	ack->hdr.status = IPC_STATUS_OK; \
	ack->hdr.msg_len = len; \
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *)ack); \
	free(ack); \
	return;
#endif
// Send simple header and return
#define	IPC_OUT(sts) \
	do { \
	if(ack) free(ack); \
	hdr.status = sts; \
	hdr.msg_len = 0; \
	ipc_send_ack(ipc_fd, &hdr); \
	return; \
	} while(0)

// Check IPC header and message length
#define	IPC_HEADER_CHECK(type, req) \
	if(req->hdr.msg_len < sizeof(type) - sizeof(ipc_request_hdr_t)) { \
		hdr.status = IPC_STATUS_ARGV; \
		hdr.msg_len = 0; \
		DBG_ASSERT(0, "Invalid IPC request, message length: %d", req->hdr.msg_len); \
		ipc_send_ack(ipc_fd, &hdr); \
		return; \
	}

// Allocate memory for array object
#define	IPC_ACK_MALLOC_ARRAY(type, member, m_type, m_number) \
	do { \
	ack = (type *)malloc(offsetof(type, member) + sizeof(m_type) * (m_number)); \
	DBG_ASSERT(ack, "fail to alloc memory"); \
	if(!ack) { \
		hdr.status = IPC_STATUS_FAIL; \
		hdr.msg_len = 0; \
		ipc_send_ack(ipc_fd, &hdr); \
		return; \
	} \
	} while(0) \

// Allocate memory for struct object
#define	IPC_ACK_MALLOC_STRUCT(type, member, size) \
	do { \
	ack = (type *)malloc(offsetof(type, member) + (size)); \
	DBG_ASSERT(ack, "fail to alloc memory"); \
	if(!ack) { \
		hdr.status = IPC_STATUS_FAIL; \
		hdr.msg_len = 0; \
		ipc_send_ack(ipc_fd, &hdr); \
		return; \
	} \
	} while(0) \

#if 0
extern clt_record_t	clt_records[];
extern cnu_service_cfg_t cnu_service_template[];
extern int cnu_service_template_count;
extern cnu_devinfo_t supported_device_info[];
#endif
extern int supported_device_count;
#ifdef CONFIG_CATVCOM
extern CATV_WORK_INFO_S *gst_CATVWorkInfo;
#endif
//extern switch_interface_t *clt_interfaces[];
//extern switch_interface_status_t clt_interface_status[];
//extern switch_interface_mib_t clt_interface_mibs[];
extern PORT_LOOPD_STATE_S *g_stLoopdState;//add by zhouguanhua 2013/6/10
//extern switch_vlan_entry_t clt_vlan_entry[];
//extern int clt_vlan_count;

int ipc_debug = 0;
void get_uni_current_state_from_shm(void);//add by zhouguanhua 2013/6/10
/*****************************************************************
Function:get_uni_current_state_from_shm
Description:get uni current state from shm
Author:huangmingjian
Date:2012/12/01
INPUT: 
OUTPUT:         
Return:

Others:	       
*****************************************************************/


void get_uni_current_state_from_shm(void)
{
    unsigned char lgcPort = 0;
	
	sys_loop_t *loop_info = &sys_loop;

   	for(lgcPort = 1; lgcPort < (MAX_PORT_NUM + 1); lgcPort++)
   	{	
    	loop_info->loopback_unis_status[lgcPort-1] = g_stLoopdState->port[lgcPort].curstate;
    }  
    return;
}

#if 0
static cnu_info_t *find_cnu_by_id(int clt, int cnu, int *err_code)
{
	cnu_info_t *pcnu;

	if(!CLT_ID_RANGE_CHECK(clt) || !CNU_ID_RANGE_CHECK(cnu)) {
		*err_code = IPC_STATUS_ARGV;
		return NULL;
	}
	if(clt_records[clt - 1].state != CLT_STATE_STANDBY) {
		*err_code = IPC_STATUS_NOCLT;
		return NULL;
	}
	pcnu = &clt_records[clt - 1].cnus[cnu - 1];
	if(pcnu->state <= CNU_STATE_INSIGHT) {
		*err_code = IPC_STATUS_NOCNU;
		return NULL;
	}
	*err_code = IPC_STATUS_OK;
	return pcnu;
}
#endif
static void ipc_send_ack(int ipc_fd, void *msg);
static void ipc_sysmon_cmd(int ipc_fd, ipc_sysmon_req_t *req)
{
	ipc_sysmon_ack_t ack;

	if(SYSMON_TESTCMD(req->cmd, SYSCMD_CONFIG)) {
		if(SYSMON_TESTFLAG(req->cmd, SYSCMD_FLAG_DEFCONF)) {	
            /*begin modified by liaohongjun 2012/6/29*/
			//nvram_restore_default();
			//nvram_restore_for_file(DEFAULT_CFG_FILE);
			cfg_restore_for_file(CONFIG_DEFAULTFILE);
            /*end modified by liaohongjun 2012/6/25*/
		}
	}else if (SYSMON_TESTFLAG(req->cmd, SYSCMD_FLAG_REBOOT)){
		sys_reboot();
	}

	if(ipc_debug) DBG_PRINTF("sysmon command: %08x", req->cmd);
	ack.hdr.status = IPC_STATUS_OK;
	ack.hdr.msg_len = sizeof(ack.result);
	ack.result = 0;
	ipc_send_ack(ipc_fd, &ack);
}
#if 0
void parse_cnu_info(ipc_cnu_info_t *pinfo, cnu_info_t *pcnu)
{
	cnu_user_config_t * user;
	clt_record_t *pclt = (clt_record_t *)pcnu->clt;
	pinfo->clt = pclt->index;
	pinfo->index = pcnu->index;
	pinfo->state = pcnu->state;
	memcpy(&pinfo->mac, &pcnu->network_info.mac, sizeof(ethernet_addr_t));

//	DBG_PRINTF("get cnu:%s\n", 	mac2str(&pinfo->mac));

	pinfo->tei = pcnu->network_info.tei;
	pinfo->avgtx = pcnu->network_info.avgtx;
	pinfo->avgrx = pcnu->network_info.avgrx;
//	if ((pcnu->state > CNU_STATE_INSIGHT)/* && pcnu->hfid[0]*/){
		if (pcnu->devinfo){
			safe_strncpy(pinfo->alias, pcnu->devinfo->alias, sizeof(pinfo->alias));			
		}else {
			safe_strncpy(pinfo->alias, pcnu->hfid, sizeof(pinfo->alias));
		}
//	}else {
//		pinfo->alias[0] = 0;
//	}
	pinfo->link = cnu_link_status(pcnu);
	pinfo->auth = pcnu->auth;
	// system time change may cause online time invalid
	// only consider the online_tm smaller than system time
	if (time(NULL) < pcnu->online_tm){
		pcnu->online_tm = time(NULL);
	}
	pinfo->online_tm = pcnu->online_tm;

	user = find_user_by_mac(&pinfo->mac);
	if (user && user->name[0]){
		safe_strncpy(pinfo->username, user->name, sizeof(pinfo->username));
	}else {
		pinfo->username[0] = 0; 
	}
	if (pcnu->version[0]){
		safe_strncpy(pinfo->version, pcnu->version, sizeof(pcnu->version));
	}else {
		pinfo->version[0] = 0;
	}	

	if(pcnu->devinfo) {
		pinfo->ports = pcnu->devinfo->ports;
		pinfo->device_id = pcnu->devinfo->device_id;
	}else {
		pinfo->ports = 2;		// Default ports for unsupported device
		pinfo->device_id = -1;	// Default device_id for unsupported device
	}
	if (pcnu->cnu_cfg){
		pinfo->template_id = pcnu->cnu_cfg->tmpl_id;
	}else {
		pinfo->template_id = -1;
	}
}

static void ipc_cnu_info(int ipc_fd, ipc_cnu_status_req_t *req)
{
	cnu_info_t *pcnu = NULL;
	ipc_cnu_info_ack_t ack;
	int err_code;

	if(req->hdr.msg_len < sizeof(ipc_cnu_status_req_t) - sizeof(ipc_request_hdr_t)) {
		DBG_ASSERT(0, "Invalid IPC request, message length: %d", req->hdr.msg_len);
		ack.hdr.status = IPC_STATUS_ARGV;
		goto out;
	}

	switch(req->filtertype) {
	case FILTER_TYPE_MAC:
		pcnu = find_cnu_by_mac(&req->mac);
		ack.hdr.status = pcnu == NULL ? IPC_STATUS_NOCNU : IPC_STATUS_OK;
		break;

	case FILTER_TYPE_ID:
		pcnu = find_cnu_by_id(req->cnu_id.clt, req->cnu_id.cnu, &err_code);
		ack.hdr.status = pcnu == NULL ? err_code : IPC_STATUS_OK;
		break;

	default:
		ack.hdr.status = IPC_STATUS_ARGV;
	}

	if(pcnu) parse_cnu_info(&ack.cnu_info, pcnu);

out:
	ack.hdr.msg_len = ACK_MSGLEN(ipc_cnu_info_ack_t);
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}

static void ipc_cnu_mib_callback(void *in)
{
	cnu_stats_t *stats = (cnu_stats_t*)in; 
	ipc_cnu_mib_ack_t ack;
	cnu_info_t *pcnu;
	int ipc_fd, port;

	DBG_ASSERT(stats, "Stats empty");
	DBG_ASSERT(stats->data, "Stats->data empty");

	pcnu = stats->pcnu;
	ipc_fd = (int)stats->data;

	if (stats->status != STATS_OP_OK){
		ack.hdr.status = IPC_STATUS_CNUNORESP;
		ack.hdr.msg_len = 0;
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *)&ack);		
	}

	memset(&ack.cnu_mib, 0, sizeof(ack.cnu_mib)); // For unsupported device, all zero
	ack.ports = 2;		// Default ports for unsupported device
	if (pcnu->devinfo) {
		ack.ports = pcnu->devinfo->ports;
		for (port = 0; port < pcnu->devinfo->ports; port++) {
//			DBG_PRINTF("BMSR: %d:%04X", port, pcnu->cnu_cfg->lsw_cfg.eth_mibs[port].mii_bmsr);
			if(port == 0) {
				ack.cnu_mib[port].link = 1;
				ack.cnu_mib[port].spd = 1;
				ack.cnu_mib[port].duplex = 1;
			}
			else {
				ack.cnu_mib[port].link = (stats->eth_mibs[port].mii_bmsr & 0x0004) != 0 ? 1 : 0;
				ack.cnu_mib[port].spd = (stats->eth_mibs[port].mii_bmsr & 0x6000) != 0 ? 1 : 0;	// FIXME: only 100M/10M detected
				ack.cnu_mib[port].duplex = 1; // FIXME: do not remember duplex bit
			}
			ack.cnu_mib[port].txpacket = stats->eth_mibs[port].txpacket;
			ack.cnu_mib[port].rxpacket = stats->eth_mibs[port].rxpacket;
			ack.cnu_mib[port].rxcrc = stats->eth_mibs[port].rxcrc;
			ack.cnu_mib[port].rxdrop = stats->eth_mibs[port].rxdrop;
			ack.cnu_mib[port].rxfrag = stats->eth_mibs[port].rxfrag;
		}
	}
	ack.hdr.status = IPC_STATUS_OK;
	ack.hdr.msg_len = ACK_MSGLEN(ipc_cnu_mib_ack_t);
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *)&ack);
}

static void ipc_cnu_mib(int ipc_fd, ipc_cnu_status_req_t *req)
{
	IPC_DECLARE_HEADER(ipc_cnu_mib_ack_t);
	ipc_cnu_mib_ack_t sack;
	cnu_info_t *pcnu = NULL;
	int err_code;

	IPC_HEADER_CHECK(ipc_cnu_status_req_t, req);

	switch(req->filtertype) {
	case FILTER_TYPE_MAC:
		pcnu = find_cnu_by_mac(&req->mac);
		if (pcnu == NULL) err_code = IPC_STATUS_NOCNU;
		break;

	case FILTER_TYPE_ID:
		pcnu = find_cnu_by_id(req->cnu_id.clt, req->cnu_id.cnu, &err_code);
		break;

	default:
		IPC_OUT(IPC_STATUS_ARGV);
	}
	if(pcnu) {
		if (pcnu->linkup && pcnu->devinfo 
			&& (pcnu->devinfo->switch_chip_id != SWITCH_CHIPID_PHYONLY)){
			if (cnu_statistics_mibs(pcnu, req->clean, ipc_cnu_mib_callback, (void*)ipc_fd) < 0){
				IPC_OUT(IPC_STATUS_CNUNOSUPP);
			}	
			return;
		}
#if 1
		if (!pcnu->linkup){
			sack.hdr.status = IPC_STATUS_CNUDOWN;
		}else /*if (!pcnu->devinfo || (pcnu->devinfo->switch_chip_id == SWITCH_CHIPID_PHYONLY))*/{
			sack.hdr.status = IPC_STATUS_CNUNOSUPP;
		}
		sack.hdr.msg_len = ACK_MSGLEN(ipc_cnu_mib_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *)&sack);
		return ;		
#else		
		memset(&sack, 0, sizeof(sack));
		if (pcnu->devinfo){
			sack.ports = pcnu->devinfo->ports;
		}else {
			sack.ports = 2;
		}
		if (pcnu->linkup){
			sack.cnu_mib[0].link = 1;
			if (sack.ports == 2) sack.cnu_mib[1].link = 1;
		}
		for (i = 0; i < sack.ports; i ++){
			sack.cnu_mib[i].spd = (sack.cnu_mib[i].link) ? ETH_SPEED_100 : ETH_SPEED_10;
		}
		sack.hdr.status = IPC_STATUS_OK;
		sack.hdr.msg_len = ACK_MSGLEN(ipc_cnu_mib_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *)&sack);
		return ;
#endif		
	}
	IPC_OUT(err_code);
}

static void ipc_cnu_link_stats_callback(void *in)
{
	cnu_stats_t *stats = (cnu_stats_t*)in; 
	ipc_cnu_link_stats_ack_t ack;
	cnu_info_t *pcnu;
	int ipc_fd;

	DBG_ASSERT(stats, "Stats empty");
	DBG_ASSERT(stats->data, "Stats->data empty");

	pcnu = stats->pcnu;
	ipc_fd = (int)stats->data;

	if ((stats->status == STATS_OP_MSTATUS) && (stats->mme_parm.mstatus == 0x02)){
		// Unknown tone slot
		stats->status = STATS_OP_OK;
	}

	if (stats->status != STATS_OP_OK){
		ack.hdr.status = IPC_STATUS_CNUNORESP;
		ack.hdr.msg_len = 0;
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *)&ack);		
	}
	memcpy(&ack.stats, &stats->cab_stats, sizeof(cable_link_stats_t)); 
	ack.hdr.status = IPC_STATUS_OK;
	ack.hdr.msg_len = ACK_MSGLEN(ipc_cnu_link_stats_ack_t);
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *)&ack);
}

static void ipc_cnu_link_stats(int ipc_fd, ipc_cnu_status_req_t *req)
{
	IPC_DECLARE_HEADER(ipc_cnu_link_stats_ack_t);
	cnu_info_t *pcnu = NULL;
	int err_code;

	IPC_HEADER_CHECK(ipc_cnu_status_req_t, req);

	switch(req->filtertype) {
	case FILTER_TYPE_MAC:
		pcnu = find_cnu_by_mac(&req->mac);
		break;

	case FILTER_TYPE_ID:
		pcnu = find_cnu_by_id(req->cnu_id.clt, req->cnu_id.cnu, &err_code);
		break;

	default:
		IPC_OUT(IPC_STATUS_ARGV);
	}
	if(pcnu) {
		if (!pcnu->linkup){
			IPC_OUT(IPC_STATUS_CNUDOWN);			
		}
		if (cnu_statistics_link(pcnu, ipc_cnu_link_stats_callback, (void*)ipc_fd) < 0){
			IPC_OUT(IPC_STATUS_CNUNOFUNC);
		}	
		return;
	}
	IPC_OUT(IPC_STATUS_NOCNU);
}


static void ipc_network_info(int ipc_fd, ipc_network_info_req_t *req)
{
	IPC_DECLARE_HEADER(ipc_network_info_ack_t);
	cnu_info_t *pcnu;
	int i, c, cnu_cnt;

	IPC_HEADER_CHECK(ipc_network_info_req_t, req);

	if(req->update != 0) networkinfo_schedule(1, REASON_IPC_CLIENT);

	if(req->clt == 0xff) {
		cnu_cnt = 0;
		IPC_ACK_MALLOC_ARRAY(ipc_network_info_ack_t, cnus, ipc_cnu_info_t, MAX_CLT_CHANNEL * MAX_CNU_PER_CLT);
		for(i = 0; i < MAX_CLT_CHANNEL; i ++) {
			if(clt_records[i].state != CLT_STATE_STANDBY) continue;
			for(c = 0; c < MAX_CNU_PER_CLT; c++) {
				pcnu = &clt_records[i].cnus[c];
				if(pcnu->state <= CNU_STATE_INSIGHT) continue;
				parse_cnu_info(&ack->cnus[cnu_cnt], pcnu);
				cnu_cnt ++;
			}
		}
		ack->clt = 0xff;
		ack->count = cnu_cnt;
		IPC_ACK(ACK_MSGLEN_R(ipc_network_info_ack_t, cnus, ipc_cnu_info_t, ack->count));
	}
	else {
		if(CLT_ID_RANGE_CHECK(req->clt)) {
			IPC_ACK_MALLOC_ARRAY(ipc_network_info_ack_t, cnus, ipc_cnu_info_t, MAX_CNU_PER_CLT);
			ack->clt = req->clt;
			i = req->clt - 1;
			cnu_cnt = 0;
			for(c = 0; c < MAX_CNU_PER_CLT; c++) {
				pcnu = &clt_records[i].cnus[c];
				if(pcnu->state <= CNU_STATE_INSIGHT) continue;
				parse_cnu_info(&ack->cnus[cnu_cnt], pcnu);
				cnu_cnt ++;
			}
			ack->count = cnu_cnt;
			IPC_ACK(ACK_MSGLEN_R(ipc_network_info_ack_t, cnus, ipc_cnu_info_t, ack->count));
		}
	}

	IPC_OUT(IPC_STATUS_ARGV);
}
#endif
static void ipc_nvram_access(int ipc_fd, ipc_nvram_access_req_t *req)
{
	ipc_nvram_access_ack_t ack;	
	//sys_network_t *sys = &sys_network;
	
	//unsigned int oid;
	if(req->hdr.msg_len < sizeof(ipc_nvram_access_req_t) - sizeof(ipc_request_hdr_t)) {
		ack.hdr.status = IPC_STATUS_ARGV;
		DBG_ASSERT(1, "Invalid IPC request, message length: %d", req->hdr.msg_len);
		goto out;
	}

	ack.hdr.status = IPC_STATUS_OK;
	switch(req->access) {
	case NVRAM_ACCESS_GET:
        if(0 != cfg_getval(req->ifindex,req->oid,ack.value,"default",(sizeof(ack.value)- 1)))
		{
		    ack.hdr.status = IPC_STATUS_FAIL;
			break;
		}
        ack.hdr.status = IPC_STATUS_OK;
		break;
	case NVRAM_ACCESS_SET: 
       
		if(0 !=master_cfg_setval(req->ifindex, req->oid, req->value))
		{
		    ack.hdr.status = IPC_STATUS_FAIL;            
			break;	
		} 
		if(IF_ROOT_IFINDEX == req->ifindex && CONFIG_TELNET_SERVICE_ENABLE == req->oid)
		{
			sys_security.telnet_en= (0==strcmp(req->value, ENABLE_STR)?1:0);
		}
        ack.hdr.status = IPC_STATUS_OK;
		//nvram_set(nvram_handle, req->name, req->value);
		break;
	case NVRAM_ACCESS_UNSET:
		//nvram_unset(nvram_handle, req->name);
		#if 1
		if(0 !=master_cfg_del(req->ifindex, req->oid))
		{
		    ack.hdr.status = IPC_STATUS_FAIL; 
			break;	
		}
        #endif
        ack.hdr.status = IPC_STATUS_OK;
		break;
	case NVRAM_ACCESS_COMMIT:
		//nvram_commit(nvram_handle);
		//nvram_update();
		break;		
	default:
		ack.hdr.status = IPC_STATUS_ARGV;
	}

out:
	ack.hdr.msg_len = ACK_MSGLEN(ipc_nvram_access_ack_t);
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}
static void ipc_cfg_backup(int ipc_fd, ipc_cfg_backup_req_t *req)
{
    ipc_cfg_backup_ack_t ack;
    if(req->hdr.msg_len < sizeof(ipc_cfg_backup_req_t) - sizeof(ipc_request_hdr_t)) {
		ack.hdr.status = IPC_STATUS_ARGV;
		DBG_ASSERT(1, "Invalid IPC request, message length: %d", req->hdr.msg_len);
		goto out;
	}
    ack.hdr.status=master_cfg_backup(req->path);
    out:
        ack.hdr.msg_len = ACK_MSGLEN(ipc_cfg_backup_ack_t);
        ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}
/*****************************************************************
    Function:ipc_nvram_get_user_info
    Description:get vty user infomation
    Author:liaohongjun
    Date:2012/8/17
    Input:int ipc_fd, ipc_get_user_info_req_t *req     
    Output:         
    Return:   
=================================================
    Note: add of QID0007
*****************************************************************/
static void ipc_nvram_get_user_info(int ipc_fd, ipc_get_user_info_req_t *req)
{
    int i = 0;
    ipc_get_user_info_ack_t ack;
   // char name[64] = {0};
    
	if(req->hdr.msg_len < sizeof(ipc_get_user_info_req_t) - sizeof(ipc_request_hdr_t)) {
		ack.hdr.status = IPC_STATUS_ARGV;
		DBG_ASSERT(0, "Invalid IPC request, message length: %d", req->hdr.msg_len);
        goto out;
	}

    memset(&ack, 0x0, sizeof(ipc_get_user_info_ack_t));
    switch(req->access)
    {
        case USER_VTY_INFO_ALL_GET:
            for(i = MIN_VTY_USER_NO; i < MAX_VTY_COUNT; i++)
            {
                /*begin modified by liaohongjun 2012/9/3 of QID0015*/
				if(0 != cfg_getval(0,CONFIG_USER_VTY_NAME+i,ack.vty_user_info.vty_user[i].username,"default",sizeof(ack.vty_user_info.vty_user[i].username)))
				{
					break;
				}
				if(0 != cfg_getval(0,CONFIG_USER_VTY_PASSWORD+i,ack.vty_user_info.vty_user[i].passwd,"default",sizeof(ack.vty_user_info.vty_user[i].passwd)))
				{
					break;
				}
				if(0 != cfg_getval(0,CONFIG_USER_VTY_AUTHMODE+i,ack.vty_user_info.vty_user[i].authmode,"default",sizeof(ack.vty_user_info.vty_user[i].authmode)))
				{
					break;
				}
				if(0 != cfg_getval(0,CONFIG_USER_VTY_PASSWORD_TYPE+i,ack.vty_user_info.vty_user[i].passwd_type,"default",sizeof(ack.vty_user_info.vty_user[i].passwd_type)))
				{
					break;
				}
				if(0 != cfg_getval(0,CONFIG_USER_VTY_TIMEOUT+i,&ack.vty_user_info.vty_user[i].timeout,"default",sizeof(ack.vty_user_info.vty_user[i].timeout)))
				{
					break;
				}
			
                /*end modified by liaohongjun 2012/9/3 of QID0015*/
            }         
            ack.hdr.status = IPC_STATUS_OK;
            break;
        default:
            ack.hdr.status = IPC_STATUS_ARGV;
            break;
    }
    
out:    
	ack.hdr.msg_len = ACK_MSGLEN(ipc_get_user_info_ack_t);
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);    
}

#if 0
static void ipc_service_template(int ipc_fd, ipc_service_template_req_t *req)
{
	IPC_DECLARE_HEADER(ipc_service_template_ack_t);
	int i, update;
	cnu_service_cfg_t *cfg;

	IPC_HEADER_CHECK(ipc_service_template_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:
		if(req->count == 0) {
			// All templates
			IPC_ACK_MALLOC_ARRAY(ipc_service_template_ack_t, templates, ipc_service_template_t, cnu_service_template_count);
			ack->count = cnu_service_template_count;
			for(i = 0; i < cnu_service_template_count; i ++) cnu_template_config_parse(&ack->templates[i], &cnu_service_template[i]);
			IPC_ACK(ACK_MSGLEN_R(ipc_service_template_ack_t, templates, ipc_service_template_t, ack->count));
		}
		else {
			// Templates by tmpl_id
			IPC_ACK_MALLOC_ARRAY(ipc_service_template_ack_t, templates, ipc_service_template_t, req->count);
			ack->count = 0;
			for(i = 0; i < req->count; i ++) {
				cfg = find_template_by_id(req->templates[i].tmpl_id);
				if(!cfg) IPC_OUT(IPC_STATUS_NOTMPL);
				cnu_template_config_parse(&ack->templates[i], cfg);
			}
			ack->count = req->count;
			IPC_ACK(ACK_MSGLEN_R(ipc_service_template_ack_t, templates, ipc_service_template_t, ack->count));
		}
		IPC_OUT(IPC_STATUS_NOTMPL);

	case IPC_CONFIG_SET:
	case IPC_CONFIG_DEL:
	case IPC_CONFIG_NEW:
		update = 0;
		for(i = 0; i < req->count; i ++) {
			//DBG_PRINTF("TMPL ID:%d\n", req->templates[i].tmpl_id);

			if ((req->access == IPC_CONFIG_SET) || ((req->access == IPC_CONFIG_NEW))){
				if (!cnu_template_config_check(&req->templates[i])){
					 IPC_OUT(IPC_STATUS_ARGV);
				}
			}
			
			cfg = find_template_by_id(req->templates[i].tmpl_id);
			switch(req->access) {
			case IPC_CONFIG_SET:
				if(!cfg) IPC_OUT(IPC_STATUS_NOTMPL);
				update += cnu_template_config_update(cfg, &req->templates[i]);
				break;
			case IPC_CONFIG_DEL:
				if(!cfg) IPC_OUT(IPC_STATUS_NOTMPL);
				update += cnu_template_config_delete(cfg);
				break;
			case IPC_CONFIG_NEW:
				update = cnu_template_config_new(&req->templates[i]);
				if(update < 0) IPC_OUT(IPC_STATUS_EXCEED);
				break;
			}
		}
		if(update > 0) {	
			if(req->apply_option & IPC_APPLY_NOW){
				if(cnu_template_config_apply(req->apply_option) < 0) IPC_OUT(IPC_STATUS_APPLY_FAIL);
				if(cnu_template_config_commit(req->apply_option) < 0) IPC_OUT(IPC_STATUS_COMMIT_FAIL);			
			}	
			if(req->apply_option & IPC_APPLY_COMMIT){
				nvram_do_commit();
			}
		}
		IPC_OUT(IPC_STATUS_OK);
	}
	IPC_OUT(IPC_STATUS_ARGV);
}
static void ipc_supported_device(int ipc_fd, ipc_supported_device_req_t *req)
{
	ipc_supported_device_ack_t ack;
	int i, p;

	ack.count = supported_device_count;
	for(i = 0; i < supported_device_count; i ++) {
		ack.devinfo[i].device_id = supported_device_info[i].device_id;
		ack.devinfo[i].ports = supported_device_info[i].ports;
		ack.devinfo[i].switch_chip_id = supported_device_info[i].switch_chip_id;
		safe_strncpy(ack.devinfo[i].alias, supported_device_info[i].alias, sizeof(ack.devinfo[i].alias));
		for(p = 0; p < CNU_MAX_ETH_PORTS + 1; p ++) ack.devinfo[i].port_map[p] = supported_device_info[i].port_map[p];
	}
	ack.hdr.status = IPC_STATUS_OK;
	ack.hdr.msg_len = ACK_MSGLEN_R(ipc_supported_device_ack_t, devinfo, ipc_cnu_devinfo_t, ack.count);
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}

static void ipc_clt_cable_param(int ipc_fd, ipc_clt_cable_param_req_t *req)
{
	ipc_clt_cable_param_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	
	int i, update;

	IPC_HEADER_CHECK(ipc_clt_cable_param_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:
		memset(&ack, 0, sizeof(ack));
		for (i = 0; i < MAX_CLT_CHANNEL; i ++){
			clt_cable_param_get(i, &ack.cab_param.clts[i]);
		}
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_clt_cable_param_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	case IPC_CONFIG_SET:
		update = 0;
	
		for (i = 0; i < MAX_CLT_CHANNEL; i ++){
			update += clt_cable_param_update(i, &req->cab_param.clts[i]);
		}
		
		if(update > 0) {			
			if(req->apply_option & IPC_APPLY_NOW){
				if(clt_cable_param_apply(req->apply_option) < 0) {
					hdr.status = IPC_STATUS_APPLY_FAIL;
					hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, &hdr);
					return;					
				}
				if(clt_cable_param_commit(req->apply_option) < 0) {
					hdr.status = IPC_STATUS_COMMIT_FAIL;
					hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, &hdr);
					return;					
				}					
			}
			if(req->apply_option & IPC_APPLY_COMMIT){
				nvram_do_commit();
			
			}
		}
		hdr.status = IPC_STATUS_OK;
		hdr.msg_len = 0;
		ipc_send_ack(ipc_fd, &hdr);
		return;

	}
	hdr.status = IPC_STATUS_ARGV;
	hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, &hdr);
}


static void ipc_clt_interface_mibs(int ipc_fd, ipc_clt_interface_mibs_req_t *req)
{
	IPC_DECLARE_HEADER(ipc_clt_interface_mibs_ack_t);
	int update, i;
	switch_interface_mib_t *ifmib;

	IPC_HEADER_CHECK(ipc_clt_interface_mibs_req_t, req);

	switch(req->access) {
	case IPC_MIBS_GET:	
		if (req->count > MAX_CLT_ETH_PORTS){
			req->count = 0;
		}
		if (req->count == 0){ // get all
			switch_interface_mibs_update_all(0);
			
			IPC_ACK_MALLOC_ARRAY(ipc_clt_interface_mibs_ack_t, ifmibs, switch_interface_mib_t, MAX_CLT_ETH_PORTS);
			ack->count = MAX_CLT_ETH_PORTS;
			memcpy(ack->ifmibs, clt_interface_mibs, sizeof(switch_interface_mib_t) * MAX_CLT_ETH_PORTS);	
			IPC_ACK(ACK_MSGLEN_R(ipc_clt_interface_mibs_ack_t, ifmibs, switch_interface_mib_t, MAX_CLT_ETH_PORTS));			
		}else {
			IPC_ACK_MALLOC_ARRAY(ipc_clt_interface_mibs_ack_t, ifmibs, switch_interface_mib_t, req->count);
			for (i = 0; i < req->count; i ++){
				ifmib = switch_interface_mibs_update(req->mibs_req[i].ifname, 0);
				if (ifmib){					
					memcpy(&ack->ifmibs[i], ifmib, sizeof(switch_interface_mib_t));	
				}else {
					IPC_OUT(IPC_STATUS_ARGV);
				}
			}
			ack->count = req->count;
			IPC_ACK(ACK_MSGLEN_R(ipc_clt_interface_mibs_ack_t, ifmibs, switch_interface_mib_t, ack->count));			
		}
	case IPC_MIBS_RESET:
		update = 0;
		if (!req->count){
			update = switch_interface_mibs_update_all(1);// reset all
		}else {
			for (i = 0; i < req->count; i ++){
				ifmib = switch_interface_mibs_update(req->mibs_req[i].ifname, 1);
				if (!ifmib){					
					IPC_OUT(IPC_STATUS_ARGV);
				}
			}			
		}
		IPC_OUT(IPC_STATUS_OK);
	}

	IPC_OUT(IPC_STATUS_ARGV);
}


static void ipc_clt_interface(int ipc_fd, ipc_clt_interface_config_req_t *req)
{
	IPC_DECLARE_HEADER(ipc_clt_interface_config_ack_t);
	int update;

	IPC_HEADER_CHECK(ipc_clt_interface_config_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:		
		if (req->count == 0){
			IPC_ACK_MALLOC_ARRAY(ipc_clt_interface_config_ack_t, ifs, switch_interface_t, MAX_CLT_ETH_PORTS);
			ack->count = MAX_CLT_ETH_PORTS;
			memcpy(ack->ifs, clt_interfaces, sizeof(switch_interface_t) * MAX_CLT_ETH_PORTS);	
			IPC_ACK(ACK_MSGLEN_R(ipc_clt_interface_config_ack_t, ifs, switch_interface_t, MAX_CLT_ETH_PORTS));			
		}else {
			if (!switch_interface_group_get(req->ifs, req->count)){
				IPC_OUT(IPC_STATUS_ARGV);
			}
			IPC_ACK_MALLOC_ARRAY(ipc_clt_interface_config_ack_t, ifs, switch_interface_t, req->count);
			memcpy(ack->ifs, req->ifs, sizeof(switch_interface_t) * req->count);	
			ack->count = req->count;
			IPC_ACK(ACK_MSGLEN_R(ipc_clt_interface_config_ack_t, ifs, switch_interface_t, ack->count));			
		}
	case IPC_CONFIG_SET:
		update = 0;
		if(req->count > 0) {
			update = switch_interface_group_set(req->ifs, req->count);		
			if(update > 0) {
				if(req->apply_option & IPC_APPLY_NOW){
					if(switch_interface_apply(req->apply_option) < 0) IPC_OUT(IPC_STATUS_APPLY_FAIL);
					if(switch_interface_commit(req->apply_option) < 0) IPC_OUT(IPC_STATUS_COMMIT_FAIL);									
				}
				
				if(req->apply_option & IPC_APPLY_COMMIT){
					nvram_do_commit();
				}	
			}
			IPC_OUT(IPC_STATUS_OK);
		}
	}

	IPC_OUT(IPC_STATUS_ARGV);
}

static void ipc_clt_interface_status(int ipc_fd, ipc_clt_interface_status_req_t *req)
{
	IPC_DECLARE_HEADER(ipc_clt_interface_status_ack_t);
	int i;

	IPC_HEADER_CHECK(ipc_clt_interface_status_req_t, req);

	switch_interface_status_update();

	if(req->ifname[0]) {
		for(i = 0; i < MAX_CLT_ETH_PORTS; i ++) {
			if(!strcmp(clt_interface_status[i].ifname, req->ifname)) {
				IPC_ACK_MALLOC_ARRAY(ipc_clt_interface_status_ack_t, status, switch_interface_status_t, 1);
				memcpy(&ack->status, &clt_interface_status[i], sizeof(switch_interface_status_t));
				ack->count = 1;
				IPC_ACK(ACK_MSGLEN_R(ipc_clt_interface_status_ack_t, status, switch_interface_status_t, ack->count));
			}
		}
		IPC_OUT(IPC_STATUS_NOIF);
	}
	else {
		IPC_ACK_MALLOC_ARRAY(ipc_clt_interface_status_ack_t, status, switch_interface_status_t, MAX_CLT_ETH_PORTS);
		memcpy(&ack->status, &clt_interface_status[0], sizeof(switch_interface_status_t) * MAX_CLT_ETH_PORTS);
		ack->count = MAX_CLT_ETH_PORTS;
		IPC_ACK(ACK_MSGLEN_R(ipc_clt_interface_status_ack_t, status, switch_interface_status_t, ack->count));
	}
}



#endif

static void ipc_clt_vlan_interface(int ipc_fd, ipc_clt_vlan_interface_config_req_t *req)
{
	IPC_DECLARE_HEADER(ipc_clt_vlan_interface_config_ack_t);
	int update = 0;
	uint16_t count;
	uint32_t data_size;
	switch_vlan_interface_t *vlanif;

	IPC_HEADER_CHECK(ipc_clt_vlan_interface_config_req_t, req);
	
	switch(req->access) {
	case IPC_CONFIG_GET:
		if(req->count == 0) {
			// Get all VLAN Interfaces
			data_size = switch_vlan_interface_data_size();
			IPC_ACK_MALLOC_STRUCT(ipc_clt_vlan_interface_config_ack_t, vlanifs, data_size);
			ack->count = 0;
			vlanif = switch_vlan_interface_data_malloc();
			if (!vlanif){			
				IPC_OUT(IPC_STATUS_OK);
			}	
			memcpy(ack->vlanifs, vlanif, data_size);
			ack->count = switch_vlan_interface_count();	
			switch_vlan_interface_data_free(vlanif);
			IPC_ACK(offsetof(ipc_clt_vlan_interface_config_ack_t, vlanifs) + data_size - sizeof(ipc_acknowledge_hdr_t));
		}
		else {
			// Get specified Interfaces,
			count = 0;
			vlanif = switch_vlan_interface_group_malloc(req->vlanifs, req->count, &count);
			data_size = vlanif && (count > 0) ? vlan_interface_mem_size(vlanif, count) : 0;
			if (!data_size){
				switch_vlan_interface_group_free(vlanif);
				IPC_OUT(IPC_STATUS_ARGV);
			}
			IPC_ACK_MALLOC_STRUCT(ipc_clt_vlan_interface_config_ack_t, vlanifs, data_size);
			memcpy(ack->vlanifs, vlanif, data_size);
			ack->count = count;
			switch_vlan_interface_group_free(vlanif);
			IPC_ACK(offsetof(ipc_clt_vlan_interface_config_ack_t, vlanifs) + data_size - sizeof(ipc_acknowledge_hdr_t));			
		}
		// Will not be here

	case IPC_CONFIG_SET:		
	case IPC_CONFIG_DEL:
	case IPC_CONFIG_NEW:
		update = 0;
		if (req->count > 0){
			switch(req->access) {
			case IPC_CONFIG_SET:
				update += switch_vlan_interface_group_update(req->vlanifs, req->count);
				break;
			case IPC_CONFIG_DEL: // only delete vlans , not set pvid and mode
				update += switch_vlan_interface_group_delete(req->vlanifs, req->count);
				break;
			case IPC_CONFIG_NEW: // only add vlans , not set pvid and mode
				update += switch_vlan_interface_group_new(req->vlanifs, req->count);
				break;
			}
		}
		if(update > 0) {

			if(req->apply_option & IPC_APPLY_NOW){
				if(switch_vlan_interface_apply(req->apply_option) < 0) IPC_OUT(IPC_STATUS_APPLY_FAIL);
				if(switch_vlan_interface_commit(req->apply_option) < 0) IPC_OUT(IPC_STATUS_COMMIT_FAIL);	
			}		

			if(req->apply_option & IPC_APPLY_COMMIT){
			//	nvram_do_commit();
			}
		}
		IPC_OUT(IPC_STATUS_OK);
		// Will not be here
	}
	IPC_OUT(IPC_STATUS_ARGV);
}


static void ipc_clt_vlan_mode(int ipc_fd, ipc_clt_vlan_mode_req_t *req)
{
	ipc_clt_vlan_mode_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	

	IPC_HEADER_CHECK(ipc_clt_vlan_mode_req_t, req);
//	printf("%s %d req->access %d \n",__FUNCTION__,__LINE__,req->access);
	switch(req->access) {
	case IPC_CONFIG_GET:
		memset(&ack, 0, sizeof(ack));
	//	printf("%s %d\n",__FUNCTION__,__LINE__);
		switch_vlan_mode_get(&ack.vlanmode);
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = sizeof(ipc_clt_vlan_mode_ack_t) - sizeof(ipc_acknowledge_hdr_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		//printf("%s %d\n",__FUNCTION__,__LINE__);
		return;
		
	case IPC_CONFIG_SET:	
		//printf("...%s %d\n",__FUNCTION__,__LINE__);
		
		if(switch_vlan_mode_set(&req->vlanmode) > 0) {	
			
			if(req->apply_option & IPC_APPLY_NOW){		
				
				if(switch_vlan_mode_apply(req->apply_option) < 0){
					hdr.status = IPC_STATUS_APPLY_FAIL;
					hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, &hdr);					
				} 
				
				
				if(switch_vlan_mode_commit(req->apply_option) < 0) {
					hdr.status = IPC_STATUS_COMMIT_FAIL;
					hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, &hdr);					
				} 	
				
			}	
			if(req->apply_option & IPC_APPLY_COMMIT){
			//	nvram_do_commit();
			}
			

		}
		
		//printf("%s %d\n",__FUNCTION__,__LINE__);
		//printf("%s %d\n",__FUNCTION__,__LINE__);
		hdr.status = IPC_STATUS_OK;
		hdr.msg_len = 0;
		ipc_send_ack(ipc_fd, &hdr);
		return;
		// Will not be here
	}
	//printf("%s %d req->access %d \n",__FUNCTION__,__LINE__,req->access);
	hdr.status = IPC_STATUS_ARGV;
	hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, &hdr);
}


static void ipc_clt_vlan(int ipc_fd, ipc_clt_vlan_config_req_t *req)
{
	IPC_DECLARE_HEADER(ipc_clt_vlan_config_ack_t);
	int update = 0, count;
	uint32_t data_size;
	switch_vlan_group_t *vlan_group;
	
	IPC_HEADER_CHECK(ipc_clt_vlan_config_req_t, req);
	
	switch(req->access) {
	case IPC_CONFIG_GET:
		//printf("ipc_clt_vlan req->count %d\n",req->count);
		if(req->count == 0) {
			// Get all VLAN
			data_size = switch_vlan_data_size();
			IPC_ACK_MALLOC_STRUCT(ipc_clt_vlan_config_ack_t, vlan_groups, data_size);
			vlan_group = switch_vlan_data_malloc();
		//	ack->count=0;
			//printf("ipc_clt_vlan vlan count %d...\n",ack->count);
			
			//	IPC_OUT(IPC_STATUS_OK);
			
			if (vlan_group){
			memcpy(ack->vlan_groups, vlan_group, data_size);
			}	
			ack->count = switch_vlan_entry_count();	
			if (vlan_group){
			switch_vlan_data_free(vlan_group);
			}
		//	printf("ipc_clt_vlan vlan count ... %d\n",ack->count);
			IPC_ACK(offsetof(ipc_clt_vlan_config_ack_t, vlan_groups) + data_size - sizeof(ipc_acknowledge_hdr_t));
		}
		else {
			vlan_group = switch_vlan_group_get(req->vlan_groups, req->count, &count);
			if (!vlan_group || !count){
				IPC_OUT(IPC_STATUS_ARGV);
			}
			data_size = vlan_group_mem_size(vlan_group, count);
			IPC_ACK_MALLOC_STRUCT(ipc_clt_vlan_config_ack_t, vlan_groups, data_size);
			memcpy(ack->vlan_groups, vlan_group, data_size);
			free(vlan_group);
			ack->count = count;
			IPC_ACK(offsetof(ipc_clt_vlan_config_ack_t, vlan_groups) + data_size - sizeof(ipc_acknowledge_hdr_t));			
		}
		// Will not be here

	case IPC_CONFIG_SET:		
	case IPC_CONFIG_DEL:
	case IPC_CONFIG_NEW:
		update = 0;
		//	printf("%s %d\n",__FUNCTION__,__LINE__);
		if (req->count > 0){
			switch(req->access) {
			case IPC_CONFIG_SET:
				//printf("%s %d\n",__FUNCTION__,__LINE__);
				update += switch_vlan_group_update(req->vlan_groups, req->count);
				break;
			case IPC_CONFIG_DEL:
				update += switch_vlan_group_delete(req->vlan_groups, req->count);
				break;
			case IPC_CONFIG_NEW:
			//	printf("%s %d\n",__FUNCTION__,__LINE__);
				update += switch_vlan_group_new(req->vlan_groups, req->count);
				break;
			}
		}
		if(update > 0) {
			if(req->apply_option & IPC_APPLY_NOW){
				if(switch_vlan_apply(req->apply_option) < 0) IPC_OUT(IPC_STATUS_APPLY_FAIL);
				if(switch_vlan_commit(req->apply_option) < 0) IPC_OUT(IPC_STATUS_COMMIT_FAIL);
			}
			if(req->apply_option & IPC_APPLY_COMMIT){
			//	nvram_do_commit();
			}
			
		}
		//printf("%s %d\n",__FUNCTION__,__LINE__);
		IPC_OUT(IPC_STATUS_OK);
		// Will not be here
	}
	IPC_OUT(IPC_STATUS_ARGV);
}
#if 0

static void ipc_cnu_user(int ipc_fd, ipc_cnu_user_config_req_t *req)
{
	IPC_DECLARE_HEADER(ipc_cnu_user_config_ack_t);
	int i, update, ret;
	cnu_user_config_t *cfg;

	IPC_HEADER_CHECK(ipc_cnu_user_config_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:
		if(req->count == 0) {
			// Get all users
			//DBG_PRINTF("get config_count = %d\n", cnu_user_config_count);
			IPC_ACK_MALLOC_ARRAY(ipc_cnu_user_config_ack_t, user, cnu_user_config_t, cnu_user_config_count);
			memcpy(ack->user, cnu_user_config, sizeof(cnu_user_config_t) * cnu_user_config_count);
			ack->count = cnu_user_config_count;
			IPC_ACK(ACK_MSGLEN_R(ipc_cnu_user_config_ack_t, user, cnu_user_config_t, cnu_user_config_count));
		}
		else {
			IPC_ACK_MALLOC_ARRAY(ipc_cnu_user_config_ack_t, user, cnu_user_config_t, req->count);
			for(i = 0; i < req->count; i ++) {
				cfg = NULL;
				if(req->filter == FILTER_TYPE_ID) cfg = find_user_by_id(req->user[i].user_id);
				if(req->filter == FILTER_TYPE_MAC) cfg = find_user_by_mac(&req->user[i].mac);
				if(!cfg) IPC_OUT(IPC_STATUS_NOUSER);
				memcpy(&ack->user[i], cfg, sizeof(cnu_user_config_t));
			}
			ack->count = req->count;
			IPC_ACK(ACK_MSGLEN_R(ipc_cnu_user_config_ack_t, user, cnu_user_config_t, ack->count));
		}
		// Will not be here
		DBG_ASSERT(1, "!!! SHOULD NOT BE HERE !!!");
	case IPC_CONFIG_SET:
	case IPC_CONFIG_DEL:
	case IPC_CONFIG_NEW:
		update = 0;
		for(i = 0; i < req->count; i ++) {
			/*char tmp[24];
			DBG_PRINTF("DEC%d.mac=%s,%d, %d,%d,%s,%s\n", i, 
				ether_addr_str(tmp, &req->user[i].mac),
				req->user[i].device_id,
				req->user[i].tmpl_id,
				req->user[i].auth,
				req->user[i].name,
				req->user[i].desc);
			*/
			cfg = NULL;
			if(req->filter == FILTER_TYPE_ID) cfg = find_user_by_id(req->user[i].user_id);
			if(req->filter == FILTER_TYPE_MAC) cfg = find_user_by_mac(&req->user[i].mac);
			switch(req->access) {
			case IPC_CONFIG_SET:
				if(!cfg) IPC_OUT(IPC_STATUS_NOUSER);
				req->user[i].user_id = cfg->user_id;
				update += cnu_user_config_update(cfg, &req->user[i]);
				break;
			case IPC_CONFIG_DEL:
				if(!cfg) IPC_OUT(IPC_STATUS_NOUSER);
				update += cnu_user_config_delete(cfg);
				break;
			case IPC_CONFIG_NEW:
				// Add a new one, 
				//if(!cfg) IPC_OUT(IPC_STATUS_NOUSER);
				if (!cfg){
					ret = cnu_user_config_new(&req->user[i]);
					if (ret < 0){
						IPC_OUT(IPC_STATUS_EXCEED);
					}else {
						update ++;
					} 					
				}else {
					req->user[i].user_id = cfg->user_id;				
					update += cnu_user_config_update(cfg, &req->user[i]);
				}
				
				break;
			}
		}
		if(update > 0) {
			if(req->apply_option & IPC_APPLY_NOW){
				if(cnu_user_config_apply(req->apply_option) < 0) IPC_OUT(IPC_STATUS_APPLY_FAIL);
				if(cnu_user_config_commit(req->apply_option) < 0) IPC_OUT(IPC_STATUS_COMMIT_FAIL);					
			}
			
			if(req->apply_option & IPC_APPLY_COMMIT){
				nvram_do_commit();
			}
			
		}
		IPC_OUT(IPC_STATUS_OK);
		// Will not be here
		DBG_ASSERT(1, "!!! SHOULD NOT BE HERE !!!");
	}

	IPC_OUT(IPC_STATUS_ARGV);

}

#endif

static void ipc_sys_security(int ipc_fd, ipc_sys_security_req_t *req)
{
	ipc_sys_security_ack_t ack;
	ipc_acknowledge_hdr_t hdr;
	
	int update;
	uint8_t mask = req->mask;

	IPC_HEADER_CHECK(ipc_sys_security_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:
		memcpy(&ack.security, &sys_security, sizeof(sys_security_t));
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_security_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	case IPC_CONFIG_SET:
		update = 0;
		update += sys_security_update(&req->security, mask);
		
		if(update > 0) {			
			if(req->apply_option & IPC_APPLY_NOW){
				if(sys_security_apply(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_APPLY_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}
				if(sys_security_commit(req->apply_option, update) < 0) {
					ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}
			}
			if(req->apply_option & IPC_APPLY_COMMIT){
				//nvram_do_commit();
				master_cfg_do_commit();
			}
		}
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_security_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	}
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
	return;	
}



static void ipc_sys_info(int ipc_fd, ipc_sys_info_req_t *req)
{
	ipc_sys_info_ack_t ack;

	memcpy(&ack.info, &sys_info, sizeof(ack.info));
	// get uptime
	ack.info.uptime = sys_get_uptime();
//	ack.info.online_cnu_num = 2;
	
	ack.hdr.status = IPC_STATUS_OK;
	ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_info_ack_t);
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}


static void ipc_sys_networking(int ipc_fd, ipc_sys_network_req_t *req)
{
	ipc_sys_network_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	
	int update;

	IPC_HEADER_CHECK(ipc_sys_network_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:
		memcpy(&ack.network, &sys_network, sizeof(sys_network_t));
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_network_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	case IPC_CONFIG_SET:
		update = 0;
		update += sys_networking_update(&req->network);
		
		if(update > 0) {			
			if(req->apply_option & IPC_APPLY_NOW){
				if(sys_networking_apply(req->apply_option) < 0) {
					hdr.status = IPC_STATUS_APPLY_FAIL;
					hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, &hdr);
					return;					
				}
				if(sys_networking_commit(req->apply_option) < 0) {
					hdr.status = IPC_STATUS_COMMIT_FAIL;
					hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, &hdr);
					return;					
				}					
			}
			if(req->apply_option & IPC_APPLY_COMMIT){
				//nvram_do_commit();
			
				master_cfg_do_commit();
			}
		}
		hdr.status = IPC_STATUS_OK;
		hdr.msg_len = 0;
		ipc_send_ack(ipc_fd, &hdr);
		return;

	}
	hdr.status = IPC_STATUS_ARGV;
	hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, &hdr);
}

static void ipc_sys_mvlan(int ipc_fd, ipc_sys_mvlan_req_t *req)
{
	ipc_sys_mvlan_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	
	int update;
	IPC_HEADER_CHECK(ipc_sys_mvlan_req_t, req);
	switch(req->access) {
	case IPC_CONFIG_GET:
		memcpy(&ack.mvlan, &sys_mvlan, sizeof(sys_mvlan_t));
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_mvlan_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;
	case IPC_CONFIG_SET:
		update = 0;
		update += sys_mvlan_update(&req->mvlan);
		if(update > 0) {			
			if(req->apply_option & IPC_APPLY_NOW){
				if(sys_mvlan_apply(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_APPLY_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}
				if(sys_mvlan_commit(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}	
			}			
			if(req->apply_option & IPC_APPLY_COMMIT){
				//nvram_do_commit();			
				master_cfg_do_commit();
			}
		}
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_admin_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	}
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}
static void ipc_sys_loop(int ipc_fd, ipc_sys_loop_req_t *req)
{
	ipc_sys_loop_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	
	int update;

	IPC_HEADER_CHECK(ipc_sys_loop_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:
        get_uni_current_state_from_shm();/*Add by huangmingjian 2012/12/01 for EPN104QID0083*/ 
		memcpy(&ack.loop, &sys_loop, sizeof(sys_loop_t));
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_loop_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	case IPC_CONFIG_SET:
		update = 0;
		update += sys_loop_update(&req->loop);
		
		if(update > 0) {			
			if(req->apply_option & IPC_APPLY_NOW){
				if(sys_loop_apply(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_APPLY_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}
				if(sys_loop_commit(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}	
			}			
			if(req->apply_option & IPC_APPLY_COMMIT){
				//nvram_do_commit();			
				master_cfg_do_commit();
			}
		}
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_admin_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	}
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}

#if 1
static void ipc_sys_epon(int ipc_fd, ipc_sys_epon_req_t *req)
{
	ipc_sys_epon_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	
	int update;

	IPC_HEADER_CHECK(ipc_sys_loop_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:
		memcpy(&ack.epon_info, &sys_epon, sizeof(sys_epon_t));
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_epon_req_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	case IPC_CONFIG_SET:
		update = 0;
		update += sys_epon_update(&req->epon_info);
		
		if(update > 0) {			
			if(req->apply_option & IPC_APPLY_NOW){
				if(sys_epon_apply(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_APPLY_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}
				if(sys_epon_commit(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}	
			}			
			if(req->apply_option & IPC_APPLY_COMMIT){
				//nvram_do_commit();			
				master_cfg_do_commit();
			}
		}
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_admin_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;
	}
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}
#endif

static void ipc_sys_snmp(int ipc_fd, ipc_sys_snmp_req_t *req)
{
	ipc_sys_snmp_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	
	int update = 0;
	int access_flag = DONT_REREAD_CONF;

	IPC_HEADER_CHECK(ipc_sys_snmp_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:
		memcpy(&ack.snmp, &sys_snmp, sizeof(sys_snmp_t));
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_snmp_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	case IPC_CONFIG_SET:
		update += sys_snmp_update(&req->snmp, &access_flag);
		if(update > 0) {
			if(req->apply_option & IPC_APPLY_NOW){
				if(sys_snmp_apply(req->apply_option, access_flag) < 0) {
					ack.hdr.status = IPC_STATUS_APPLY_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}
				if(sys_snmp_commit(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return; 				
				}	
			}			
			if(req->apply_option & IPC_APPLY_COMMIT){
				//nvram_do_commit();
				master_cfg_do_commit();
			}
		}
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_snmp_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	}
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);

}

/*****************************************************************
  Function:        ipc_sys_log
  Description:     ipc_sys_log
  Author: 	         zhouguanhua
  Date:   		   2013/4/7
  INPUT:           (ipc_sys_log_req_t *)nreq
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/
static void ipc_syslog(int ipc_fd, ipc_syslog_req_t  *req)
{
	ipc_syslog_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	
	int update;

	IPC_HEADER_CHECK(ipc_syslog_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:
		memcpy(&ack.syslog, &sys_syslog, sizeof(sys_syslog_t));
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_syslog_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	case IPC_CONFIG_SET:
		update = 0;
		update += sys_syslog_update(&req->syslog);
		
		if(update > 0) {
			if(req->apply_option & IPC_APPLY_NOW){
				if(sys_syslog_apply(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_APPLY_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}
				if(sys_syslog_commit(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return; 				
				}	
			}			
			if(req->apply_option & IPC_APPLY_COMMIT){
				//nvram_do_commit();
				master_cfg_do_commit();
			}
		}
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_syslog_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	}
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}


/*****************************************************************
  Function:        ipc_sys_mac_setting
  Description:    ipc_sys_mac_setting
  Author: 	         huangmingjian
  Date:   		   2013/09/08
  INPUT:           (ipc_mac_setting_req_t *)nreq
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/
static void ipc_mac_setting(int ipc_fd, ipc_mac_setting_req_t *req)
{
	ipc_mac_setting_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	 
	int update = 0;
	UINT32 aging_time = 0;
	
	IPC_HEADER_CHECK(ipc_mac_setting_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:
		if(0 != Ioctl_GetMacAgeTime(&aging_time))
		{	
			DBG_PRINTF("Get aging_time by Ioctl fail");
		}
		sys_mac_setting.aging_time = aging_time/10;
		memcpy(&ack.mac_setting, &sys_mac_setting, sizeof(mac_setting_t));
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_mac_setting_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	case IPC_CONFIG_SET:
		update = 0;
		update += sys_mac_setting_update(&req->mac_setting);
		
		if(update > 0) {
			if(req->apply_option & IPC_APPLY_NOW){
				if(sys_mac_setting_apply(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_APPLY_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}
				if(sys_mac_setting_commit(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return; 				
				}	
			}			
			if(req->apply_option & IPC_APPLY_COMMIT){
				//nvram_do_commit();
				master_cfg_do_commit();
			}
		}
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_mac_setting_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	}
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}


static void ipc_sys_admin(int ipc_fd, ipc_sys_admin_req_t *req)
{
	ipc_sys_admin_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	
	int update;

	IPC_HEADER_CHECK(ipc_sys_admin_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:
		memcpy(&ack.admin, &sys_admin, sizeof(sys_admin_t));
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_admin_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	case IPC_CONFIG_SET:
		update = 0;
		update += sys_admin_update(&req->admin);
		
		if(update > 0) {			
			if(req->apply_option & IPC_APPLY_NOW){
				if(sys_admin_apply(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_APPLY_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}
				if(sys_admin_commit(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}	
			}			
			if(req->apply_option & IPC_APPLY_COMMIT){
				//nvram_do_commit();			
				master_cfg_do_commit();
			}
		}
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_admin_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	}
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}



static void ipc_sys_time(int ipc_fd, ipc_sys_time_req_t *req)
{
	ipc_sys_time_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	
	int update;
	

	IPC_HEADER_CHECK(ipc_sys_time_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:
		memcpy(&ack.time, &sys_time, sizeof(sys_time_t));
		// update systime
		ack.time.time = local_time(time(NULL), ack.time.zone);
        /*Begin modify by zhouguanhua 2012-9-1 chat ntpclient synchronization*/
        sys_time.ntp_ok= get_ntp_syn_state();  
        /*end modify by zhouguanhua 2012-9-1 chat ntpclient synchronization*/

		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_time_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	case IPC_CONFIG_SET:
		update = 0;
		update += sys_time_update(&req->time);
		
		if(update > 0) {	
			if(req->apply_option & IPC_APPLY_NOW){
				if(sys_time_apply(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_APPLY_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}
				if(sys_time_commit(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}						
			}
			if(req->apply_option & IPC_APPLY_COMMIT){
				//nvram_do_commit();				
				master_cfg_do_commit();
			}
		}
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_time_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	}
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);

}


static void ipc_sys_dhcpc_event(int ipc_fd, ipc_sys_dhcpc_req_t *req)
{
	ipc_acknowledge_hdr_t hdr;	
	
	IPC_HEADER_CHECK(ipc_sys_dhcpc_req_t, req);

	if (!sys_ip_dhcpc_event_apply(&req->dhcpc)){
		hdr.status = IPC_STATUS_ARGV;
	}else {
		hdr.status = IPC_STATUS_OK;
	}
	hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, &hdr);
}

static void ipc_system_req(int ipc_fd, ipc_system_req_t *req)
{
	ipc_system_ack_t ack;	
	ipc_acknowledge_hdr_t hdr;
	
	int reboot_fd = -1;
    char appPath[64] = {0};
	char cfgPath[64] = {0};
	
	IPC_HEADER_CHECK(ipc_system_req_t, req);

	ack.status = 0;
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = ACK_MSGLEN(ipc_system_ack_t);

	switch(req->access) {
	case IPC_SYS_REBOOT:
		USER_LOG(LangId,SYSTEM,LOG_NOTICE,M_REBOOT,"",""); //add by leijinbao 2013/09/17 for reboot log 
		sys_dealy_do(SYS_DO_REBOOT);		
		ack.hdr.status = IPC_STATUS_OK;
		break;		
	case IPC_SYS_RESTORE:
		sys_dealy_do(SYS_DO_RESTORE);		
		ack.hdr.status = IPC_STATUS_OK;
		break;		
	case IPC_SYS_RESTORE_AND_REBOOT:
		USER_LOG(LangId,SYSTEM,LOG_NOTICE,M_RECONF,"","");  //add by leijinbao 2013/09/17 for restored configure log
		sys_dealy_do(SYS_DO_RESTORE_AND_REBOOT);
		ack.hdr.status = IPC_STATUS_OK;
		break;

	case IPC_SYS_NVRAM_COMMIT:
		//sys_dealy_do(SYS_DO_NVRAM_COMMIT);	
		//nvram_do_commit();
		master_cfg_do_commit();
		USER_LOG(LangId,SYSTEM,LOG_INFO,M_SAVECONF,"","");  //add by leijinbao 2013/09/17 for save log
		ack.hdr.status = IPC_STATUS_OK;
		break;
		
	case IPC_SYS_CONFIG_BACKUP:
        sprintf(cfgPath, "%s%s.config", UFILE_TMP_PATH, PRODUCT_NAME);	
		if (master_cfg_backup(cfgPath) < 0){
			ack.hdr.status = IPC_STATUS_FAIL;
		}else {
			ack.hdr.status = IPC_STATUS_OK;
		}
		break;
	case IPC_SYS_LOG_CLEAR:
		system("echo  \"\" > "SYSLOG_FILE_PATH);
		ack.hdr.status = IPC_STATUS_OK;	
		break;
	case IPC_SYS_CONFIG_READY:
        /*begin modify by liaohongjun 2012/8/1 of QID0003*/
        #if 0
		if (sys_perform_upgrade(req->process)){
			ack.process = sys_upgrade_state(0);
			ack.percentage = sys_upgrade_percentage();
			
			if (ack.process == UFILE_STATE_FAILED){
				ack.hdr.status = IPC_STATUS_FAIL;
			}else {
				ack.hdr.status = IPC_STATUS_OK;
			}			

		}else {
			ack.hdr.status = IPC_STATUS_ARGV;
		}
		break;
        #endif
		#define UFILE_TMP_CONFIG "/tmp/config.bin"
        #if 1
        if(!ufile_check(UFILE_TMP_CONFIG))
        {
            ack.hdr.status = IPC_STATUS_FAIL;
        }
        else
        {
            if(SYS_OK == sys_import_config())
            {
    			ack.process = sys_upgrade_state(0);
    			ack.percentage = sys_upgrade_percentage();                
                if (ack.process == UFILE_STATE_FAILED)
                {
                    ack.hdr.status = IPC_STATUS_FAIL;
                }
                else 
                {
                    ack.hdr.status = IPC_STATUS_OK;
                }
            }
            else
            {
                ack.hdr.status = IPC_STATUS_ARGV;
            }
        }
        #endif
        break;
        /*end modified by liaohongjun 2012/8/1 of QID0003*/
    /*begin modified by liaohongjun 2012/7/10*/    
	#if 0
	case IPC_SYS_UPGRADE_READY: 
		if (sys_perform_upgrade(req->process)){
			ack.hdr.status = IPC_STATUS_OK;
		}else {
			ack.hdr.status = IPC_STATUS_ARGV;
		}
		break;
    #endif
    
    case IPC_SYS_UPGRADE_APP: 
		/*Begin modify by zhouguanhua 2013-4-16 for BugID0005*/
		 sprintf(appPath, "/tmp/%s", APP_UPGRADE_FILE);
		/*End modify by zhouguanhua 2013-4-16 for   BugID0005*/

        /*Begin modify by zhouguanhua 2013/7/2*/
        if (APP_CHECK_OK != app_ufile_check(appPath))
        {
        	unlink(appPath);/*Add by huangmingjian 2014-04-24 for Bug 551*/
            ack.hdr.status = IPC_STATUS_APP_INVALID;
        }
        else 
        {
        	/*Begin modified by sunmingliang for  bug 199*/
        	if((reboot_fd = open(REBOOT_LOCK, O_RDWR|O_CREAT|O_TRUNC)) < 0)
        	{
				printf("Open file error!\n");
				ack.hdr.status = IPC_STATUS_ARGV;
				break;
			}
			/*we are not release lock, because system is going reboot after upgrade*/
        	if(WRITE_LOCK(reboot_fd, 0, SEEK_SET, 0) < 0)
			{
				ack.hdr.status = IPC_STATUS_BUSY;
			}
			else
			{
				if(SYS_OK == sys_upgrade_app((int)req->process))
	            {
	                ack.hdr.status = IPC_STATUS_OK;
	            }
	            else
	            {
	                ack.hdr.status = IPC_STATUS_ARGV;
	            }
//				UNLOCK(reboot_fd, 0, SEEK_SET, 0);
			}
           /*End modified by sunmingliang for bug 199*/
        }
        break;
    /*end modified by liaohongjun 2012/7/10*/    
    case IPC_SYS_UPGRADE_STATUS: 
    //if (sys_is_performing_upgrade())
    //{
        ack.status |= IPC_SYS_STATUS_UPGRADING;
        ack.process = sys_upgrade_state(0);
        ack.percentage = sys_upgrade_percentage();

        if (ack.process == UFILE_STATE_FAILED)
        {
            ack.error = sys_upgrade_error();
        }
        else
        {
            ack.error = 0;
        }
        //DBG_PRINTF("up get %d %d %d", ack.process, ack.percentage, ack.error);
    //}
        ack.hdr.status = IPC_STATUS_OK;	
        break;
	case IPC_SYS_NVRAM_STATUS:
		/*need modify in future*/
		/*
		if (nvram_handle->has_change){
			ack.status |= IPC_SYS_STATUS_NVRAM_CHANGE;
		}
		*/
		ack.hdr.status = IPC_STATUS_OK;

		break;	
	case IPC_SYS_STATUS:

		if (sys_is_performing_upgrade()){
			ack.status |= IPC_SYS_STATUS_UPGRADING;
			ack.process = sys_upgrade_state(0);
			if (ack.process == UFILE_STATE_FAILED){
				ack.error = sys_upgrade_error();
			}
		}
		if (master_cfg_getcommitflag(IF_ROOT_IFINDEX))
		{
			ack.status |= IPC_SYS_STATUS_NVRAM_CHANGE;
		}
		ack.hdr.status = IPC_STATUS_OK;
		break;	
	}

	ipc_send_ack(ipc_fd, &ack);
}


static void ipc_send_ack(int ipc_fd, void *msg)
{
	ipc_acknowledge_hdr_t *ipc_msg = (ipc_acknowledge_hdr_t *)msg;
	if(ipc_debug) DBG_PRINTF("IPC send %d bytes", ipc_msg->msg_len);
	send(ipc_fd, ipc_msg, ipc_msg->msg_len + sizeof(ipc_acknowledge_hdr_t), 0);
}
extern int  show_startupstart(VOID);
extern int  show_startupend(VOID);

void ipc_cfg_showstartupstart(int ipc_fd, ipc_show_startup_req_t *req)
{	
	ipc_show_startup_ack_t ack;

	if(req->hdr.msg_len < sizeof(ipc_cfg_backup_req_t) - sizeof(ipc_request_hdr_t)) {
		ack.hdr.status = IPC_STATUS_ARGV;
		DBG_ASSERT(1, "Invalid IPC request, message length: %d", req->hdr.msg_len);
		goto out;
	}
	ack.hdr.status=show_startupstart();
  out:
    ack.hdr.msg_len = ACK_MSGLEN(ipc_show_startup_ack_t);
    ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}
void ipc_cfg_showstartupend(int ipc_fd, ipc_show_startup_req_t *req)
{	
	ipc_show_startup_ack_t ack;

	if(req->hdr.msg_len < sizeof(ipc_cfg_backup_req_t) - sizeof(ipc_request_hdr_t)) {
		ack.hdr.status = IPC_STATUS_ARGV;
		DBG_ASSERT(1, "Invalid IPC request, message length: %d", req->hdr.msg_len);
		goto out;
	}
	ack.hdr.status=show_startupend();
  out:
    ack.hdr.msg_len = ACK_MSGLEN(ipc_show_startup_ack_t);
    ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}
/*Begin added by feihuaxin for qos 2013-7-10*/
static void ipc_qos_trustmode(int ipc_fd, ipc_sys_qos_req_t *req)
{
#if 0
    ipc_sys_qos_ack_t ack;

    if((QOS_COS != req->uiQosTrustmode) && (QOS_DSCP != req->uiQosTrustmode))
    {
        ack.hdr.status = IPC_STATUS_APP_INVALID;
        ack.hdr.msg_len = 0;
        ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
        return;                 
    }
    
    if (DRV_OK != Ioctl_SetQosTrustMode(req->uiQosTrustmode))
    {
        ack.hdr.status = IPC_STATUS_APPLY_FAIL;
        ack.hdr.msg_len = 0;
        ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
        return;  
    }
    
    #if 0
    if(sys_qos_trustmode_commit(req->uiQosTrustmode) < 0) {
        ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
        ack.hdr.msg_len = 0;
        ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
        return;                 
    }   
    #endif
    
    ack.hdr.status = IPC_STATUS_OK;
    ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_qos_ack_t);
    ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
    return;
#endif
    ipc_sys_qos_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	
    int update = 0;

    memset(&ack, 0, sizeof(ipc_sys_qos_ack_t));
    IPC_HEADER_CHECK(ipc_sys_qos_req_t, req);

    switch(req->access)
    {
        case IPC_CONFIG_GET:
        {
            ack.uiQosTrustmode = sys_qos.uiQosTrustmode;
            break;
        }
        case IPC_CONFIG_SET:
        {
            update += sys_qosmode_update(req->uiQosTrustmode);
            if(update > 0) 
            {            
                if(req->apply_option & IPC_APPLY_NOW)
                {
                    if(DRV_OK != Ioctl_SetQosTrustMode(req->uiQosTrustmode)) 
                    {
                        ack.hdr.status = IPC_STATUS_APPLY_FAIL;
                        ack.hdr.msg_len = 0;
                        ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
                        return;                 
                    }
                    #if 1
                    if(sys_qos_trustmode_commit(req->apply_option) < 0)
                    {
                        ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
                        ack.hdr.msg_len = 0;
                        ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
                        return;                 
                    } 
                    #endif
                }           
                if(req->apply_option & IPC_APPLY_COMMIT)
                {
                    //nvram_do_commit();            
                    master_cfg_do_commit();
                }
            }
            break;
        }
        default:
        {
            ack.hdr.status = IPC_STATUS_ARGV;
            ack.hdr.msg_len = 0;
            ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
            return;
        }
            
    }
    ack.hdr.status = IPC_STATUS_OK;
    ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_qos_ack_t);
    ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);

    return;

}

void ipc_qos_queueschedul(int ipc_fd, ipc_sys_qosqueuemod_req_t *req)
{
    ipc_sys_qosqueuemod_ack_t ack;    
	ipc_acknowledge_hdr_t hdr;	
    int update = 0;

    memset(&ack, 0, sizeof(ipc_sys_qosqueuemod_ack_t));
    IPC_HEADER_CHECK(ipc_sys_qosqueuemod_req_t, req);

    switch(req->access)
    {
        case IPC_CONFIG_GET:
        {
            memcpy(&(ack.Qosqueuemode), &(sys_qos.Qosqueuemode), sizeof(QueueMode_S));
            break;
        }
        case IPC_CONFIG_SET:
        {
            update += sys_queuesch_update(&(req->Qosqueuemode));
            if(update > 0) 
            {            
                if(req->apply_option & IPC_APPLY_NOW)
                {
                    if(DRV_OK != Ioctl_SetQosQueueSchedul(req->Qosqueuemode)) 
                    {
                        ack.hdr.status = IPC_STATUS_APPLY_FAIL;
                        ack.hdr.msg_len = 0;
                        ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
                        return;                 
                    }
                    #if 1 
                    if(sys_qos_queueschedul_commit(req->apply_option) < 0)
                    {  
                        ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
                        ack.hdr.msg_len = 0;
                        ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
                        return;                 
                    } 
                    #endif
                }           
                if(req->apply_option & IPC_APPLY_COMMIT)
                {
                    //nvram_do_commit();            
                    master_cfg_do_commit();
                }
            }
            break;
        }
        default:
        {
            ack.hdr.status = IPC_STATUS_ARGV;
            ack.hdr.msg_len = 0;
            ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
            return;
        }
            
    }
    ack.hdr.status = IPC_STATUS_OK;
    ack.hdr.msg_len = ACK_MSGLEN(ipc_sys_qosqueuemod_ack_t);
    ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
    
    return;
}
/*End added by feihuaxin for qos 2013-7-10*/

void ipc_port(int ipc_fd, ipc_port_req_t *req)
{
    int update = 0;
    char arryidx = 0;
    ipc_port_ack_t ack;    
	ipc_acknowledge_hdr_t hdr;	
    port_num_t lport = 0;
    logic_pmask_t lPortMask;
    long long updatPortMsk = 0;
    PORT_CFG_VALUE_S port_cfg_val[LOGIC_PORT_NO] = {0};
    memset(&ack, 0, sizeof(ipc_port_ack_t));
    IPC_HEADER_CHECK(ipc_port_req_t, req);

    switch(req->access)
    {
        case IPC_CONFIG_GET:
        {
            memcpy(ack.port_cfg_val, sys_port.port_cfg_val, sizeof(PORT_CFG_VALUE_S)*LOGIC_PORT_NO);
            break;
        }
        case IPC_CONFIG_SET:
        {
            update += sys_port_update(req->port_cfg_val, &updatPortMsk);
            if(update > 0) 
            {            
                if(req->apply_option & IPC_APPLY_NOW)
                {
                    LgcPortFor(lport)
                    {  
                        arryidx = lport - 1;
                        if(updatPortMsk & (1<<arryidx))
                        {
                            Ioctl_SetPortEnable(lport, req->port_cfg_val[arryidx].ulIfEnable);
                            #if 0
                            Ioctl_SetPortDuplex(lport, req->port_cfg_val[arryidx].stDuplex, PORT_TYPE_IS_COPPER);
                            Ioctl_SetPortSpeed(lport, req->port_cfg_val[arryidx].stSpeed, PORT_TYPE_IS_COPPER);
                            #endif
                            SetPortDuplex(lport, req->port_cfg_val[arryidx].stDuplex);
                            SetPortSpeed(lport, req->port_cfg_val[arryidx].stSpeed);
                            Ioctl_SetPortPause(lport, req->port_cfg_val[arryidx].ulFlowCtl, req->port_cfg_val[arryidx].ulFlowCtl);
                        }
                        Ioctl_SetPortMdix(lport, req->port_cfg_val[arryidx].ulMdix);
                        Ioctl_SetPortPriority(lport,req->port_cfg_val[arryidx].ulPriority);

                        if(req->port_cfg_val[arryidx].ulIsolateEn)
                        {
                            SetLgcMaskBit(lport,&lPortMask);
                        }
                    }
                    Ioctl_SetPortIsolate(lPortMask);
                    
                    if(sys_port_commit(req->apply_option) < 0)
                    {  
                        ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
                        ack.hdr.msg_len = 0;
                        ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
                        return;                 
                    }

                }           
                if(req->apply_option & IPC_APPLY_COMMIT)
                {
                    //nvram_do_commit();            
                    master_cfg_do_commit();
                }
            }
            break;
        }
        default:
        {
            ack.hdr.status = IPC_STATUS_ARGV;
            ack.hdr.msg_len = 0;
            ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
            return;
        }
            
    }
    ack.hdr.status = IPC_STATUS_OK;
    ack.hdr.msg_len = ACK_MSGLEN(ipc_port_ack_t);
    ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
    
    return;


}

void ipc_strom_ctl(int ipc_fd, ipc_stormctl_req_t *req)
{
    ipc_stormctl_ack_t ack;    
	ipc_acknowledge_hdr_t hdr;	
    int update = 0;
    logic_pmask_t lPortMask;
	long long updatPortMsk = 0;
    STORM_CTLRATE_S pstStorm = {0};
    int ret = 0;
	int i = 0;
	port_num_t lport = 0;

    memset(&ack, 0, sizeof(ipc_stormctl_ack_t));
    IPC_HEADER_CHECK(ipc_stormctl_req_t, req);

    switch(req->access)
    {
        case IPC_CONFIG_GET:
        {
            memcpy(ack.stormctl, sys_storm.stormclt, sizeof(STROM_CTL_S)*LOGIC_PORT_NO);
            break;
        }
        case IPC_CONFIG_SET:
        {
            update += sys_stormctl_update(req->stormctl ,&updatPortMsk);
            if(update > 0) 
            {        
            
                if(req->apply_option & IPC_APPLY_NOW)
                {
                    //SetLgcMaskAll(&lPortMask);
                   
                    for(i = 0; i < LOGIC_PORT_NO; i++)
                	{
                	  
                	    if(updatPortMsk & (1 << i))
            	    	{
            	    		lport = i + 1;
						    ClrLgcMaskAll(&lPortMask);
		                    SetLgcMaskBit(lport, &lPortMask);
							
							pstStorm.rateType = STORM_RATE_PERCENT;
							
							if(0 == req->stormctl[i].uucasten)
							{
								/*if value is 100% close it*/
								pstStorm.value = 100;
								ret = Ioctl_SetStormCtrlPort(STORM_DLF_CTRL, &lPortMask, &pstStorm);
							}
							else
							{
								pstStorm.value = req->stormctl[i].ratepercent;
								ret = Ioctl_SetStormCtrlPort(STORM_DLF_CTRL, &lPortMask, &pstStorm);
							}
							
							if(0 != ret)
							{
								ack.hdr.status = IPC_STATUS_APPLY_FAIL;
								ack.hdr.msg_len = 0;
								ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
								return;    
							}
							
							if(0 == req->stormctl[i].umcasten)
							{
								/*if value is 100% close it*/
								pstStorm.value = 100;
								ret = Ioctl_SetStormCtrlPort(STORM_MCAST_CTRL, &lPortMask, &pstStorm);
							}
							else
							{
								pstStorm.value = req->stormctl[i].ratepercent;
								ret = Ioctl_SetStormCtrlPort(STORM_MCAST_CTRL, &lPortMask, &pstStorm);
							}
							
							if(0 != ret)
							{
								ack.hdr.status = IPC_STATUS_APPLY_FAIL;
								ack.hdr.msg_len = 0;
								ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
								return;    
							}
							
							
							if(0 == req->stormctl[i].bcasten)
							{
								/*if value is 100% close it*/
								pstStorm.value = 100;
								ret = Ioctl_SetStormCtrlPort(STORM_BCAST_CTRL, &lPortMask, &pstStorm);
							}
							else
							{
								pstStorm.value = req->stormctl[i].ratepercent;
								ret = Ioctl_SetStormCtrlPort(STORM_BCAST_CTRL, &lPortMask, &pstStorm);
							}
							
							if(0 != ret)
							{
								ack.hdr.status = IPC_STATUS_APPLY_FAIL;
								ack.hdr.msg_len = 0;
								ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
								return;    
							}

            	    	}
                	}
					
                    if(sys_stormctl_commit(req->apply_option) < 0)
                    {  
                        ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
                        ack.hdr.msg_len = 0;
                        ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
                        return;                 
                    }

                }           
                if(req->apply_option & IPC_APPLY_COMMIT)
                {
                    //nvram_do_commit();            
                    master_cfg_do_commit();
                }
            }
            break;
        }
        default:
        {
            ack.hdr.status = IPC_STATUS_ARGV;
            ack.hdr.msg_len = 0;
            ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
            return;
        }
            
    }
    
    ack.hdr.status = IPC_STATUS_OK;
    ack.hdr.msg_len = ACK_MSGLEN(ipc_stormctl_ack_t);
    ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
    
    return;
}


void ipc_portlinerate(int ipc_fd, ipc_linerate_req_t *req)
{
	
    int update = 0;
    char arryidx = 0;
    ipc_linerate_ack_t ack;    
	ipc_acknowledge_hdr_t hdr;	
    port_num_t lport = 0;
    long long updatPortMsk = 0;
    PORT_LINERATE_VALUES portlineVal[LOGIC_PORT_NO] = {0};
    memset(&ack, 0, sizeof(ipc_linerate_ack_t));
    IPC_HEADER_CHECK(ipc_linerate_req_t, req);

    switch(req->access)
    {
        case IPC_CONFIG_GET:
        {
            memcpy(ack.linerate_val, sys_linerate.port_linerate_val, sizeof(PORT_LINERATE_VALUES)*LOGIC_PORT_NO);
            break;
        }
        case IPC_CONFIG_SET:
        {
            update += sys_linerate_update(req->linerate_val, &updatPortMsk);
            if(update > 0) 
            {            
                if(req->apply_option & IPC_APPLY_NOW)
                {
                    LgcPortFor(lport)
                    {  
                        arryidx = lport - 1;
                        if(updatPortMsk & (1<<arryidx))
                        {
							Setportlinerate(lport, DRV_RATELIMIT_RX, req->linerate_val[arryidx].ulRateIn);
							Setportlinerate(lport, DRV_RATELIMIT_TX, req->linerate_val[arryidx].ulRateOut);
                        }
                    }
                    
                    if(sys_linerate_commit(req->apply_option) < 0)
                    {  
                        ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
                        ack.hdr.msg_len = 0;
                        ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
                        return;                 
                    }
                }           
                if(req->apply_option & IPC_APPLY_COMMIT)
                {
                    //nvram_do_commit();            
                    master_cfg_do_commit();
                }
            }
            break;
        }
        default:
        {
            ack.hdr.status = IPC_STATUS_ARGV;
            ack.hdr.msg_len = 0;
            ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
            return;
        }
            
    }
    ack.hdr.status = IPC_STATUS_OK;
    ack.hdr.msg_len = ACK_MSGLEN(ipc_linerate_ack_t);
    ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
    
    return;

}

void ipc_portmirror(int ipc_fd, ipc_mirror_req_t *req)
{
    int update = 0;
	int ret = 0;
    ipc_mirror_ack_t ack;    
	ipc_acknowledge_hdr_t hdr;	
    memset(&ack, 0, sizeof(ipc_mirror_ack_t));
    IPC_HEADER_CHECK(ipc_mirror_ack_t, req);

    switch(req->access)
    {
        case IPC_CONFIG_GET:
        {
			memcpy(&ack.portmirror, &sys_mirror.port_mirror_val, sizeof(MIRROR_GROUP_S));
            break;
        }
        case IPC_CONFIG_SET:
        {
            update += sys_mirror_update(&(req->portmirror));
            if(update > 0) 
            {            
                if(req->apply_option & IPC_APPLY_NOW)
                {
					ret  = Ioctl_SetMirrorGroup(req->portmirror.ulPortMonitor, 
					req->portmirror.stMirrPortInList, req->portmirror.stMirrPortOutList
					);

					if(DRV_OK != ret)
					{
                        ack.hdr.status = IPC_STATUS_APPLY_FAIL;
                        ack.hdr.msg_len = 0;
                        ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
                        return;      
					}
                    
                    if(sys_mirror_commit(req->apply_option) < 0)
                    {  
                        ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
                        ack.hdr.msg_len = 0;
                        ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
                        return;                 
                    }
                }           
                if(req->apply_option & IPC_APPLY_COMMIT)
                {
                    //nvram_do_commit();            
                    master_cfg_do_commit();
                }
            }
            break;
        }
        default:
        {
            ack.hdr.status = IPC_STATUS_ARGV;
            ack.hdr.msg_len = 0;
            ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
            return;
        }
            
    }
    ack.hdr.status = IPC_STATUS_OK;
    ack.hdr.msg_len = ACK_MSGLEN(ipc_mirror_ack_t);
    ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
    
    return;

}


#ifdef CONFIG_CATVCOM
#if defined(CONFIG_ZBL_SNMP)

/*****************************************************************
Function:get_catvinfo
Description:get catvinfo
Author:huangmingjian
Date:2012/12/01
INPUT: 
OUTPUT:         
Return:

Others:	       
*****************************************************************/

void get_catvinfo(void)
{	
	catvinfo_t *info = &sys_catv;
	
	memset(info->input_laser_power, '\0', sizeof(info->input_laser_power));
	if(VAL_INT_ERROR == gst_CATVWorkInfo->input_laser_power)
	{
		sprintf(info->input_laser_power, "%s", ERROR_DISPLAY);
		info->input_laser_power_mw = VAL_INT_ERROR;
	}
	else
	{
		if(0x7FFF == gst_CATVWorkInfo->input_laser_power)
		{
			sprintf(info->input_laser_power, "%s", "< -25.0"); 								
		}
		else
		{	
			sprintf(info->input_laser_power, "%0.1f", (float)gst_CATVWorkInfo->input_laser_power/10); 	
			info->input_laser_power_mw = gst_CATVWorkInfo->input_laser_power_mw;
		}		
	}
	DBG_PRINTF("input_laser_power:%04x, val_int=%04x, val_str=%s", VAL_INT_ERROR, gst_CATVWorkInfo->input_laser_power, info->input_laser_power);

	//memset(info->output_level, '\0', sizeof(info->output_level));
	if(VAL_INT_ERROR == gst_CATVWorkInfo->output_level)
	{
		info->output_level = VAL_INT_ERROR;	
	}
	else
	{	
		info->output_level = gst_CATVWorkInfo->output_level;			
	}
	DBG_PRINTF("output_level:%04x, val_int=%04x", VAL_INT_ERROR, gst_CATVWorkInfo->output_level);

	memset(info->temper, '\0', sizeof(info->temper));

	if(VAL_UINT32_ERROR == gst_CATVWorkInfo->temper.val_int)
	{
		sprintf(info->temper, "%s", ERROR_DISPLAY);	
	}
	else
	{	
		sprintf(info->temper, "%d.%d", gst_CATVWorkInfo->temper.val_int, gst_CATVWorkInfo->temper.val_dec);
	}
	DBG_PRINTF("temper:%04x, val_int=%04x, val_str=%s", VAL_INT_ERROR, gst_CATVWorkInfo->temper.val_int, info->temper);

	//memset(info->power_12v, '\0', sizeof(info->power_12v));
	if(VAL_INT_ERROR == gst_CATVWorkInfo->power_12v)
	{
		info->power_12v = VAL_INT_ERROR;	
	}
	else
	{	
		info->power_12v = gst_CATVWorkInfo->power_12v;			
	}
	DBG_PRINTF("power_12v:%04x, val_int=%04x", VAL_INT_ERROR, gst_CATVWorkInfo->power_12v);

    return;
}

#else
void get_catvinfo(void)
{	
	catvinfo_t *info = &sys_catv;

	memset(info->soft_version, '\0', sizeof(info->soft_version));
	if(VAL_UINT8_ERROR == gst_CATVWorkInfo->soft_ver)
	{
		sprintf(info->soft_version, "%s", "N/A");
	}
	else
	{
		sprintf(info->soft_version, "%d", gst_CATVWorkInfo->soft_ver);		
	}
	
	memset(info->input_laser_power, '\0', sizeof(info->input_laser_power));
	if(VAL_INT_ERROR == gst_CATVWorkInfo->input_laser_power.val_int)
	{
		sprintf(info->input_laser_power, "%s", ERROR_DISPLAY);
	}
	else
	{	if(gst_CATVWorkInfo->input_laser_power.val_int & FLAG_MASK)	
		{	
			sprintf(info->input_laser_power, "-%d.%d", (gst_CATVWorkInfo->input_laser_power.val_int)&VALUE_MASK, gst_CATVWorkInfo->input_laser_power.val_dec);				
		}
		else 
		{	
			if((0x7FFF == gst_CATVWorkInfo->input_laser_power.val_int) && (0xFF == gst_CATVWorkInfo->input_laser_power.val_dec))
			{
				sprintf(info->input_laser_power, "%s", "< -12.0"); 								
			}
			else
			{	
				sprintf(info->input_laser_power, "%d.%d", gst_CATVWorkInfo->input_laser_power.val_int, gst_CATVWorkInfo->input_laser_power.val_dec); 			
			}
		}
	}
	DBG_PRINTF("input_laser_power:%04x, val_int=%04x, val_str=%s", VAL_INT_ERROR, gst_CATVWorkInfo->input_laser_power.val_int, info->input_laser_power);

	memset(info->output_level, '\0', sizeof(info->output_level));
	if(VAL_INT_ERROR == gst_CATVWorkInfo->output_level.val_int)
	{
		sprintf(info->output_level, "%s", ERROR_DISPLAY);	
	}
	else
	{	if(gst_CATVWorkInfo->output_level.val_int & FLAG_MASK) 
		{
			sprintf(info->output_level, "-%d.%d", (gst_CATVWorkInfo->output_level.val_int)&VALUE_MASK, gst_CATVWorkInfo->output_level.val_dec);				
		}
		else
		{
			sprintf(info->output_level, "%d.%d", gst_CATVWorkInfo->output_level.val_int, gst_CATVWorkInfo->output_level.val_dec);			
		}
	}
	DBG_PRINTF("output_level:%04x, val_int=%04x, val_str=%s", VAL_INT_ERROR, gst_CATVWorkInfo->output_level.val_int, info->output_level);

	memset(info->temper, '\0', sizeof(info->temper));
	if(VAL_INT_ERROR == gst_CATVWorkInfo->temper.val_int)
	{
		sprintf(info->temper, "%s", ERROR_DISPLAY);	
	}
	else
	{	if(gst_CATVWorkInfo->temper.val_int& FLAG_MASK)
		{
			sprintf(info->temper, "-%d.%d", (gst_CATVWorkInfo->temper.val_int)&VALUE_MASK, gst_CATVWorkInfo->temper.val_dec);				
		}
		else
		{
			sprintf(info->temper, "%d.%d", gst_CATVWorkInfo->temper.val_int, gst_CATVWorkInfo->temper.val_dec);
		}
	}
	DBG_PRINTF("temper:%04x, val_int=%04x, val_str=%s", VAL_INT_ERROR, gst_CATVWorkInfo->temper.val_int, info->temper);

	memset(info->power_12v, '\0', sizeof(info->power_12v));
	if(VAL_INT_ERROR == gst_CATVWorkInfo->power_12v.val_int)
	{
		sprintf(info->power_12v, "%s", ERROR_DISPLAY);	
	}
	else
	{	if(gst_CATVWorkInfo->power_12v.val_int & FLAG_MASK)
		{
			sprintf(info->power_12v, "-%d.%d", (gst_CATVWorkInfo->power_12v.val_int)&VALUE_MASK, gst_CATVWorkInfo->power_12v.val_dec);				
		}
		else
		{
			sprintf(info->power_12v, "%d.%d", gst_CATVWorkInfo->power_12v.val_int, gst_CATVWorkInfo->power_12v.val_dec);
		}
	}
	DBG_PRINTF("power_12v:%04x, val_int=%04x, val_str=%s", VAL_INT_ERROR, gst_CATVWorkInfo->power_12v.val_int, info->power_12v);

	info->output_level_atten = (gst_CATVWorkInfo->output_atten_set & VALUE_MASK);
	DBG_PRINTF("output_atten_set:%04x, val_int=%04x, val=%d", VAL_INT_ERROR, gst_CATVWorkInfo->output_atten_set, info->output_level_atten);
    return;
}

#endif

void ipc_catv(int ipc_fd, ipc_catvinfo_req_t *req)
{
	ipc_catvinfo_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	  
	int update = 0;
	
	IPC_HEADER_CHECK(ipc_catvinfo_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:
		get_catvinfo();
		memcpy(&ack.catvinfo, &sys_catv, sizeof(catvinfo_t));
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_catvinfo_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	case IPC_CONFIG_SET:
		update = 0;
		update += sys_catv_update(&req->catvinfo);
		
		if(update > 0) {
			if(req->apply_option & IPC_APPLY_NOW){
				if(sys_catv_apply(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_APPLY_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}
				if(sys_catv_commit(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return; 				
				}
			}			
			if(req->apply_option & IPC_APPLY_COMMIT){
				master_cfg_do_commit();
			}
		}
		ack.hdr.status = (update<0?IPC_STATUS_ARGV:IPC_STATUS_OK);
		ack.hdr.msg_len = ACK_MSGLEN(ipc_catvinfo_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	}
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}
#endif


#ifdef CONFIG_WLAN_AP	
void ipc_wlan_ap(int ipc_fd, ipc_wlan_ap_req_t *req)
{
	ipc_wlan_ap_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	  
	int update = 0;
	
	IPC_HEADER_CHECK(ipc_wlan_ap_req_t, req);

	switch(req->access) {
	case IPC_CONFIG_GET:
		SYS_ERR_CHK_NORET(get_wlan_mac_filter(&sys_wlan_ap));
		memcpy(&ack.wlan_ap, &sys_wlan_ap, sizeof(wlan_ap_t));
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_wlan_ap_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	case IPC_CONFIG_SET:
		update = 0;
		update += sys_wlan_ap_update(&req->wlan_ap);
		
		if(update > 0) {
			if(req->apply_option & IPC_APPLY_NOW){
				if(sys_wlan_ap_apply(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_APPLY_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}
				if(sys_wlan_ap_commit(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return; 				
				}
			}			
			if(req->apply_option & IPC_APPLY_COMMIT){
				master_cfg_do_commit();
			}
		}
		ack.hdr.status = (update<0?IPC_STATUS_ARGV:IPC_STATUS_OK);
		ack.hdr.msg_len = ACK_MSGLEN(ipc_wlan_ap_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	}
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}
#endif


/*begin added by wanghuanyu for 129*/
int shutdown_interface(int ipc_fd,ipc_shutdown_interface_req_t *preq)
{

	ipc_shutdown_interface_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	
	int update;

	IPC_HEADER_CHECK(ipc_shutdown_interface_req_t, preq);
	if(preq->op==SHUTDOWN)
	{
		IF_Disable(preq->uiifindex);
		master_cfg_setval(preq->uiifindex,CONFIG_IF_ENABLE,ENUMTOSTR(IF_MOD_DISABLE));
	}
	else
	{
		IF_Enable(preq->uiifindex);
		master_cfg_setval(preq->uiifindex,CONFIG_IF_ENABLE,ENUMTOSTR(IF_MOD_ENABLE));
	}
	ack.hdr.status = IPC_STATUS_OK;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
	return 0;	
}
/*end added by wanghuanyu for 129*/

/*begin added by linguobin 2013-12-6*/
#if defined(CONFIG_ONU_COMPATIBLE)
static void ipc_compatible_setting(int ipc_fd, ipc_compatible_setting_req_t *req)
{
	ipc_compatible_setting_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	
	int update;
	IPC_HEADER_CHECK(ipc_compatible_setting_req_t, req);
	switch(req->access) {
	case IPC_CONFIG_GET:
		memcpy(&ack.compatible_setting, &sys_compatible, sizeof(compatible_setting_t));
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_compatible_setting_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;
	case IPC_CONFIG_SET:
		update = 0;
		update += sys_compatible_update(&req->compatible_setting);
		if(update > 0) {			
			if(req->apply_option & IPC_APPLY_NOW){				
				if(sys_compatible_commit(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}	
			}			
			if(req->apply_option & IPC_APPLY_COMMIT){
				//nvram_do_commit();			
				master_cfg_do_commit();
			}
		}
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_compatible_setting_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	}
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}
#endif
/*end added by linguobin 2013-12-6*/

/*begin added by linguobin 2014-04-22*/
#if defined(CONFIG_ZBL_SNMP)
static void ipc_analogPropertyTable_set(int ipc_fd, ipc_analogPropertyTable_req_t *req)
{
	ipc_analogPropertyTable_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	
	int update;
	IPC_HEADER_CHECK(ipc_analogPropertyTable_req_t, req);
	switch(req->access) {
	case IPC_CONFIG_GET:
		memcpy(&ack.AnalogProperty, &sys_AnalogProperty, sizeof(AnalogProperty_t));
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_analogPropertyTable_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;
	case IPC_CONFIG_SET:
		update = 0;
		update += sys_analogPropertyTable_update(&req->AnalogProperty);
		if(update > 0) {			
			if(req->apply_option & IPC_APPLY_NOW){				
				if(sys_analogPropertyTable_commit(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}	
			}			
			if(req->apply_option & IPC_APPLY_COMMIT){
				//nvram_do_commit();			
				master_cfg_do_commit();
			}
		}
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_analogPropertyTable_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	}
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}

static void ipc_commonAdminGroup_set(int ipc_fd, ipc_commonAdminGroup_req_t *req)
{
	ipc_commonAdminGroup_ack_t ack;
	ipc_acknowledge_hdr_t hdr;	
	int update;
	IPC_HEADER_CHECK(ipc_commonAdminGroup_req_t, req);
	switch(req->access) {
	case IPC_CONFIG_GET:
		memcpy(&ack.commonAdminGroup, &sys_commonAdminGroup, sizeof(commonAdminGroup_t));
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_commonAdminGroup_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;
	case IPC_CONFIG_SET:
		update = 0;
		update += sys_commonAdminGroup_update(&req->commonAdminGroup);
		if(update > 0) {			
			if(req->apply_option & IPC_APPLY_NOW){				
				if(sys_commonAdminGroup_commit(req->apply_option) < 0) {
					ack.hdr.status = IPC_STATUS_COMMIT_FAIL;
					ack.hdr.msg_len = 0;
					ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
					return;					
				}	
			}			
			if(req->apply_option & IPC_APPLY_COMMIT){
				//nvram_do_commit();			
				master_cfg_do_commit();
			}
		}
		ack.hdr.status = IPC_STATUS_OK;
		ack.hdr.msg_len = ACK_MSGLEN(ipc_commonAdminGroup_ack_t);
		ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
		return;

	}
	ack.hdr.status = IPC_STATUS_ARGV;
	ack.hdr.msg_len = 0;
	ipc_send_ack(ipc_fd, (ipc_acknowledge_hdr_t *) &ack);
}
#endif
/*end added by linguobin 2014-04-22*/


void ipc_packet_process(ipc_entry_t *ipc_entry)
{
	ipc_request_hdr_t req, *nreq;
	int ret, msize;

#ifdef IPC_PACKET_DEBUG
	if(ipc_debug) DBG_PRINTF("IPC client packet from %s.%d", ipc_entry->process_name, ipc_entry->pid);
#endif

	ret = recv(ipc_entry->fd, &req, sizeof(ipc_request_hdr_t), MSG_PEEK);
//	DBG_PRINTF("Receive from IPC: %d bytes", ret);
	if(ret < sizeof(ipc_request_hdr_t)) {
#ifdef IPC_PACKET_DEBUG
		DBG_ASSERT(0, "IPCs receive fail: %d.", ret);
		if(ret <= 0) {
			DBG_ASSERT(0, "IPC warning, process %s.%d terminated.", ipc_entry->process_name, ipc_entry->pid);
		}
#endif
		close(ipc_entry->fd);
		ipc_entry->fd = -1;
		return;
	}
	
	msize = sizeof(ipc_request_hdr_t) + req.msg_len;
	nreq = (ipc_request_hdr_t *)malloc(msize);
	if(nreq == NULL) {
		DBG_ASSERT(0, "Fail to allocate memory for IPCs content, size: %d", msize);
		return;
	}
	ret = recv(ipc_entry->fd, nreq, msize, 0);
#ifdef IPC_PACKET_DEBUG
	if(ipc_debug) DBG_PRINTF("Receive from IPC: %d bytes", ret);
#endif
	if(ret < msize) {
		free(nreq);
		DBG_ASSERT(0, "Fail to read IPCs message content: %d", ret);
		return;
	}
#ifdef IPC_PACKET_DEBUG
	if(ipc_debug) {
		DBG_PRINTF("IPC_TYPE: %d", nreq->ipc_type);
		DBG_PRINTF("IPC_MSGLEN: %d", nreq->msg_len);
		//	hexdump(nreq, 0, nreq->msg_len + sizeof(ipc_request_hdr_t));
	}
#endif
	//DBG_ASSERT(0, "%s %d\n",__FUNCTION__,__LINE__);
//	printf("%s %d nreq->ipc_type %d\n",__FUNCTION__,__LINE__,nreq->ipc_type);
	switch(nreq->ipc_type) {
	case IPC_SYSMON_CMD:
		ipc_sysmon_cmd(ipc_entry->fd, (ipc_sysmon_req_t *)nreq);
		break;
    #if 0
	case IPC_NETWORK_INFO:
		ipc_network_info(ipc_entry->fd, (ipc_network_info_req_t *)nreq);
		break;
	
	case IPC_GET_MME_STAT:
		ipc_mme_statistics(ipc_entry->fd, (ipc_mme_stat_req_t *)nreq);
		break;

	case IPC_GET_CNU_INFO:
		ipc_cnu_info(ipc_entry->fd, (ipc_cnu_status_req_t *)nreq);
		break;
	case IPC_GET_CNU_LINK_STATS:
		ipc_cnu_link_stats(ipc_entry->fd, (ipc_cnu_status_req_t *)nreq);
		break;
	case IPC_GET_CNU_MIB:
		ipc_cnu_mib(ipc_entry->fd, (ipc_cnu_status_req_t *)nreq);
	 	break;
	#endif
	case IPC_CFG_ACCESS:
		ipc_nvram_access(ipc_entry->fd, (ipc_nvram_access_req_t *)nreq);
		break;    
	case IPC_GET_USER_INFO:
		ipc_nvram_get_user_info(ipc_entry->fd, (ipc_get_user_info_req_t *)nreq);
		break;  
    case IPC_CFG_BACKUP:
        ipc_cfg_backup(ipc_entry->fd,(ipc_cfg_backup_req_t *)nreq);
        break;
	case IPC_SHOW_STARTUPSTART:
		ipc_cfg_showstartupstart(ipc_entry->fd,(ipc_show_startup_req_t *)nreq);
		break;
	case IPC_SHOW_STARTUPEND:
		ipc_cfg_showstartupend(ipc_entry->fd,(ipc_show_startup_req_t *)nreq);
		break;
 #if 0
	case IPC_SERVICE_TEMPLATE:
		ipc_service_template(ipc_entry->fd, (ipc_service_template_req_t *)nreq);
		break;

	case IPC_SUPPORTED_DEVICE:
		ipc_supported_device(ipc_entry->fd, (ipc_supported_device_req_t *)nreq);
		break;
	case IPC_CLT_INTERFACE:
		ipc_clt_interface(ipc_entry->fd, (ipc_clt_interface_config_req_t *)nreq);
		break;
	case IPC_CLT_INTERFACE_STATUS:
		ipc_clt_interface_status(ipc_entry->fd, (ipc_clt_interface_status_req_t *)nreq);
		break;
	case IPC_CLT_INTERFACE_MIB:
		ipc_clt_interface_mibs(ipc_entry->fd, (ipc_clt_interface_mibs_req_t*)nreq);
		break;	
#endif 

	case IPC_CLT_VLAN_INTERFACE:
		ipc_clt_vlan_interface(ipc_entry->fd, (ipc_clt_vlan_interface_config_req_t *)nreq);
		break;			

	case IPC_CLT_VLAN_MODE:
		//printf("1 %s %d\n",__FUNCTION__,__LINE__);
		//printf("%s %d\n",__FUNCTION__,__LINE__);
		ipc_clt_vlan_mode(ipc_entry->fd, (ipc_clt_vlan_mode_req_t *)nreq);
		break;

	case IPC_CLT_VLAN:
		//printf("2 %s %d\n",__FUNCTION__,__LINE__);
		ipc_clt_vlan(ipc_entry->fd, (ipc_clt_vlan_config_req_t *)nreq);
		break;
#if 0

	case IPC_CNU_USER:
		ipc_cnu_user(ipc_entry->fd, (ipc_cnu_user_config_req_t *)nreq);
		break;


#endif
	case IPC_SYS_SECURITY:
		ipc_sys_security(ipc_entry->fd, (ipc_sys_security_req_t *)nreq);
		break;		
	case IPC_SYS_INFO:
		ipc_sys_info(ipc_entry->fd, (ipc_sys_info_req_t *)nreq);
		break;		
	case IPC_SYS_SNMP:
		ipc_sys_snmp(ipc_entry->fd, (ipc_sys_snmp_req_t *)nreq);
		break;		
	case IPC_SYS_ADMIN:
		ipc_sys_admin(ipc_entry->fd, (ipc_sys_admin_req_t *)nreq);
		break;		
	case IPC_SYS_NETWORKING:
		ipc_sys_networking(ipc_entry->fd, (ipc_sys_network_req_t *)nreq);
		break;		
	case IPC_SYS_TIME:
		ipc_sys_time(ipc_entry->fd, (ipc_sys_time_req_t *)nreq);
		break;	
	case IPC_SYS_DHCPC_EVENT:
		ipc_sys_dhcpc_event(ipc_entry->fd, (ipc_sys_dhcpc_req_t *)nreq);
		break;
	case IPC_SYSTEM:
		ipc_system_req(ipc_entry->fd, (ipc_system_req_t *)nreq);
		break;	
    case IPC_MVLAN:
        ipc_sys_mvlan(ipc_entry->fd, (ipc_sys_mvlan_req_t *)nreq);
        break;	
    case IPC_LOOP:
        ipc_sys_loop(ipc_entry->fd, (ipc_sys_loop_req_t *)nreq);
        break;  
    case IPC_SYSLOG:
       ipc_syslog(ipc_entry->fd, (ipc_syslog_req_t *)nreq);
        break;  
/*begin added by wanghuanyu for 129*/
	case IPC_SHUTDOWN_INTF:
		{			
			shutdown_interface(ipc_entry->fd,((ipc_shutdown_interface_req_t *)nreq));
		}
		break;
/*end added by wanghuanyu for 129*/
case IPC_MAC_SETTING:
   		ipc_mac_setting(ipc_entry->fd, (ipc_mac_setting_req_t *)nreq);
	break; 

#if 0
	case IPC_CLT_CABLE_PARAM:
		ipc_clt_cable_param(ipc_entry->fd, (ipc_clt_cable_param_req_t*)nreq);
		break;
#endif
    case IPC_QOS_TRUSTMODE:
        ipc_qos_trustmode(ipc_entry->fd, (ipc_sys_qos_req_t *)nreq);
        break;
    case IPC_QOS_QUEUESCH:
        ipc_qos_queueschedul(ipc_entry->fd, (ipc_sys_qosqueuemod_req_t *)nreq);
        break;
    case IPC_PORT:
        ipc_port(ipc_entry->fd, (ipc_port_req_t *)nreq);
        break;
    case IPC_STORM_CTL:
        ipc_strom_ctl(ipc_entry->fd, (ipc_stormctl_req_t *)nreq);
        break;
	case IPC_LINERATE:
		ipc_portlinerate(ipc_entry->fd, (ipc_linerate_req_t *)nreq);
        break;
	case IPC_EPON:
		ipc_sys_epon(ipc_entry->fd, (ipc_sys_loop_req_t *)nreq);
		break;
    case IPC_MIRROR:
        ipc_portmirror(ipc_entry->fd, (ipc_mirror_req_t *)nreq);
        break;
#ifdef CONFIG_CATVCOM	
	case IPC_CATV:
		ipc_catv(ipc_entry->fd, (ipc_catvinfo_req_t *)nreq);
		break;
#endif
#ifdef CONFIG_WLAN_AP	
	case IPC_WLAN_AP:
		ipc_wlan_ap(ipc_entry->fd, (ipc_wlan_ap_req_t *)nreq);
		break;
#endif	
#if defined(CONFIG_ONU_COMPATIBLE)
    case IPC_COMPATIBLE_SETTING:
		ipc_compatible_setting(ipc_entry->fd, (ipc_compatible_setting_req_t *)nreq);
		break;
#endif
#if defined(CONFIG_ZBL_SNMP)
	case IPC_ANALOGPROPERTYTABLE:
		ipc_analogPropertyTable_set(ipc_entry->fd, (ipc_analogPropertyTable_req_t *)nreq);
		break;
	case IPC_COMMONADMINGROUP:
		ipc_commonAdminGroup_set(ipc_entry->fd, (ipc_commonAdminGroup_req_t *)nreq);
		break;
#endif
	default:
		DBG_PRINTF("Unknown IPC request: %d", nreq->ipc_type);
	}

	free(nreq);

}
