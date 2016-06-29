/*
 * ipc_funcs.c
 *
 *  Created on: Mar 4, 2011
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <netdb.h>
#include <time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <linux/if_packet.h>

#include "debug.h"

#include "cable.h"
#include "ipc_client.h"
//#include "switch.h"
//#include "vlan_utils.h"
#include <lw_type.h>
#include <lw_if_pub.h>
#include <fcntl.h>
#include <ipc_protocol.h> //add by linguobin 2013-12-6

#define	TEST_IPC_FD(x) do { if(x < 0) { DBG_PRINTF("Invalid ipc fd"); return NULL; } } while(0)

#define IPC_RECEIVE(pack, type, fd) \
	pack = (type *)ipc_recv_acknowledge(fd); \
	DBG_ASSERT(pack, "NO ACK"); \
	if(pack) DBG_ASSERT(pack->hdr.status == IPC_STATUS_OK, "IPC fail: %s", ipc_strerror(pack->hdr.status));


static const char *ipc_error_string[] = {
		"No error",
		"Operation fail",
		"Invalid argument",
		"Resource busy",
		"App Invalid",
		"No such CLT device",
		"No such CNU device",
		"No such interface",
		"No such VLAN",
		"No such template",
		"No such user",
		"CNU is down",
		"CNU is not supported",
		"CNU do not support the operation",
		"CNU not response",		
		"Apply configuration fail",
		"Commit configuration fail",
		"Already exists",
		"Maxim items exceeds",
		"Nvram Changed",
		"Nvram updated",
};

char *ipc_strerror(int err)
{
	if(err < IPC_STATUS_END)
		return (char *)(ipc_error_string[err]);
	else
		return "Unknown error";
}

ipc_mme_stat_ack_t *ipc_mme_stat(int ipc_fd, uint8_t clean)
{
	ipc_mme_stat_req_t req;
	ipc_mme_stat_ack_t* pack;	

	TEST_IPC_FD(ipc_fd);

	req.hdr.ipc_type = IPC_GET_MME_STAT;
	req.clean = clean;
	req.hdr.msg_len = REQ_MSGLEN(ipc_mme_stat_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_mme_stat_ack_t, ipc_fd);
	return pack;
}

ipc_cnu_info_ack_t *ipc_cnu_info(int ipc_fd, int filter_type, uint8_t clt, uint8_t cnu, ethernet_addr_t *cnu_mac)
{
	ipc_cnu_status_req_t req;
	ipc_cnu_info_ack_t *pack;

	req.hdr.ipc_type = IPC_GET_CNU_INFO;
	req.filtertype = filter_type;
	switch(filter_type) {
	case FILTER_TYPE_ID:
		req.cnu_id.clt = clt;
		req.cnu_id.cnu = cnu;
		break;
	case FILTER_TYPE_MAC:
		memcpy(&req.mac, cnu_mac, sizeof(ethernet_addr_t));
		break;
	default:
		DBG_PRINTF("Invalid CNU filter type!");
		return NULL;
	}
	req.hdr.msg_len = REQ_MSGLEN(ipc_cnu_status_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_cnu_info_ack_t, ipc_fd);
	return pack;

}

ipc_cnu_mib_ack_t *ipc_cnu_mib(int ipc_fd, int clean, int filter_type, uint8_t clt, uint8_t cnu, ethernet_addr_t *cnu_mac)
{
	ipc_cnu_status_req_t req;
	ipc_cnu_mib_ack_t *pack;


	req.hdr.ipc_type = IPC_GET_CNU_MIB;
	req.clean = clean;
	req.filtertype = filter_type;
	switch(filter_type) {
	case FILTER_TYPE_ID:
		req.cnu_id.clt = clt;
		req.cnu_id.cnu = cnu;
		break;
	case FILTER_TYPE_MAC:
		memcpy(&req.mac, cnu_mac, sizeof(ethernet_addr_t));
		break;
	default:
		DBG_PRINTF("Invalid CNU filter type!");
		return NULL;
	}
	req.hdr.msg_len = REQ_MSGLEN(ipc_cnu_status_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_cnu_mib_ack_t, ipc_fd);
	return pack;
}


ipc_cnu_link_stats_ack_t *ipc_cnu_link_stats(int ipc_fd, int filter_type, uint8_t clt, uint8_t cnu, ethernet_addr_t *cnu_mac)
{
	ipc_cnu_status_req_t req;
	ipc_cnu_link_stats_ack_t *pack;


	req.hdr.ipc_type = IPC_GET_CNU_LINK_STATS;
	req.clean = 0;
	req.filtertype = filter_type;
	switch(filter_type) {
	case FILTER_TYPE_ID:
		req.cnu_id.clt = clt;
		req.cnu_id.cnu = cnu;
		break;
	case FILTER_TYPE_MAC:
		DBG_ASSERT(cnu_mac, "Mac Empty");
		memcpy(&req.mac, cnu_mac, sizeof(ethernet_addr_t));
		break;
	default:
		DBG_PRINTF("Invalid CNU filter type!");
		return NULL;
	}
	req.hdr.msg_len = REQ_MSGLEN(ipc_cnu_status_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_cnu_link_stats_ack_t, ipc_fd);
	return pack;
}



ipc_network_info_ack_t *ipc_network_info(int ipc_fd, uint8_t clt, uint8_t update)
{
	ipc_network_info_req_t req;
	ipc_network_info_ack_t *pack;
		

	req.hdr.ipc_type = IPC_NETWORK_INFO;
	req.clt = clt;
	req.update = update;
	req.hdr.msg_len = REQ_MSGLEN(ipc_network_info_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_network_info_ack_t, ipc_fd);
	return pack;
}
#if 0
ipc_nvram_access_ack_t *ipc_nvram_access(int ipc_fd, uint8_t access, char *name, char *value)
{
	ipc_nvram_access_req_t req;
	ipc_nvram_access_ack_t *pack;

	req.hdr.ipc_type = IPC_NVRAM_ACCESS;
	req.access = access;
	if(name) strncpy(req.name, name, sizeof(req.name) - 1);
	if(value) strncpy(req.value, value, sizeof(req.value) - 1);
	req.hdr.msg_len = REQ_MSGLEN(ipc_nvram_access_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_nvram_access_ack_t, ipc_fd);
	return pack;
}
#endif
int ipc_cfg_set(ifindex_t ifindex,unsigned uioid, char *value)
{
	ipc_nvram_access_req_t req;
	ipc_nvram_access_ack_t *pack;
    int fd;
	req.hdr.ipc_type = IPC_CFG_ACCESS;
	req.access = NVRAM_ACCESS_SET;
    if(value==NULL)
    {
        return -1;
    }
    fd=ipc_client_connect("SLAVER");
    if(fd <=0)
    {
        return -1;
    }
//	if(name) strncpy(req.name, name, sizeof(req.name) - 1);
    //strncpy(req.value, value, sizeof(req.value) - 1);
    strcpy(req.value,value);
	req.ifindex = ifindex;
    req.oid=uioid;
	req.hdr.msg_len = REQ_MSGLEN(ipc_nvram_access_req_t);
	ipc_send_request(fd, &req);

	IPC_RECEIVE(pack, ipc_nvram_access_ack_t, fd);
    if(pack)
    {
        //strcpy(value,pack->value);   
        close(fd);
    	return pack->hdr.status;
    }
    else
    {
        close(fd);
        return -1;
    }
}
int ipc_cfg_del(ifindex_t ifindex,unsigned uioid)
{
	ipc_nvram_access_req_t req;
	ipc_nvram_access_ack_t *pack;
    int fd;
	req.hdr.ipc_type = IPC_CFG_ACCESS;
	req.access = NVRAM_ACCESS_UNSET;
    fd=ipc_client_connect("SLAVER");
    if(fd <=0)
    {
        return -1;
    }
//	if(name) strncpy(req.name, name, sizeof(req.name) - 1);
    //strncpy(req.value, value, sizeof(req.value) - 1);
    
	req.ifindex = ifindex;
    req.oid=uioid;
	req.hdr.msg_len = REQ_MSGLEN(ipc_nvram_access_req_t);
	ipc_send_request(fd, &req);

	IPC_RECEIVE(pack, ipc_nvram_access_ack_t, fd);
    if(pack)
    {
        //strcpy(value,pack->value);   
        close(fd);
    	return pack->hdr.status;
    }
    else
    {
        close(fd);
        return -1;
    }
}

int ipc_cfg_backup(char *value)
{
    ipc_cfg_backup_req_t req;
	ipc_cfg_backup_ack_t *pack;
    int fd;
	req.hdr.ipc_type = IPC_CFG_BACKUP;	
    if(value==NULL)
    {
        return -1;
    }
    fd=ipc_client_connect("SLAVER");
    if(fd <=0)
    {
        return -1;
    }
//	if(name) strncpy(req.name, name, sizeof(req.name) - 1);
    //strncpy(req.value, value, sizeof(req.value) - 1);
    strcpy(req.path,value);	
	req.hdr.msg_len = REQ_MSGLEN(ipc_nvram_access_req_t);
	ipc_send_request(fd, &req);

	IPC_RECEIVE(pack, ipc_cfg_backup_ack_t, fd);
    if(pack)
    {
        //strcpy(value,pack->value);   
        close(fd);
    	return pack->hdr.status;
    }
    else
    {
        close(fd);
        return -1;
    }
}
int ipc_show_startupstart(void)
{
	int fd;
	ipc_show_startup_req_t req;
	req.hdr.ipc_type = IPC_SHOW_STARTUPSTART;	
    ipc_show_startup_ack_t *pack;
    fd=ipc_client_connect("SLAVER");
    if(fd <=0)
    {
        return -1;
    }
   // strcpy(req.path,pPath);	
	req.hdr.msg_len = REQ_MSGLEN(ipc_nvram_access_req_t);
	ipc_send_request(fd, &req);

	IPC_RECEIVE(pack, ipc_cfg_backup_ack_t, fd);
    if(pack)
    {
        //strcpy(value,pack->value);   
        close(fd);
    	return pack->hdr.status;
    }
    else
    {
        close(fd);
        return -1;
    }
}
int ipc_show_startupend(void)
{
	int fd;
	ipc_show_startup_req_t req;
	req.hdr.ipc_type = IPC_SHOW_STARTUPEND;	
     ipc_show_startup_ack_t *pack;
	 
    fd=ipc_client_connect("SLAVER");
    if(fd <=0)
    {
        return -1;
    }
   // strcpy(req.path,pPath);	
	req.hdr.msg_len = REQ_MSGLEN(ipc_nvram_access_req_t);
	ipc_send_request(fd, &req);

	IPC_RECEIVE(pack, ipc_cfg_backup_ack_t, fd);
    if(pack)
    {
        //strcpy(value,pack->value);   
        close(fd);
    	return pack->hdr.status;
    }
    else
    {
        close(fd);
        return -1;
    }
}

/*****************************************************************
    Function:ipc_nvram_get_user_info
    Description: get all vty user infomation
    Author:liaohongjun
    Date:2012/8/17
    Input:int ipc_fd, uint8_t access     
    Output:ipc_all_vty_user_info_t *vty_user_info         
    Return:
=================================================
    Note:
*****************************************************************/
ipc_get_user_info_ack_t *ipc_nvram_get_user_info(int ipc_fd, uint8_t access, ipc_all_vty_user_info_t *vty_user_info)
{
	ipc_get_user_info_req_t req;
	ipc_get_user_info_ack_t *pack = NULL;
    
    if(NULL == vty_user_info)
    {
        return NULL;
    }
    memset(&req, 0, sizeof(ipc_get_user_info_req_t));

	req.hdr.ipc_type = IPC_GET_USER_INFO;
	req.access = access;

	req.hdr.msg_len = REQ_MSGLEN(ipc_get_user_info_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_get_user_info_ack_t, ipc_fd);
	return pack;
}

ipc_clt_cable_param_ack_t * ipc_clt_cable_param (int ipc_fd, uint8_t access, uint8_t apply_option, ipc_clt_cable_param_t* cab_param)
{
	ipc_clt_cable_param_req_t req;
	ipc_clt_cable_param_ack_t *pack;
	memset(&req, 0, sizeof(ipc_clt_cable_param_req_t));
	if (cab_param) memcpy(&req.cab_param, cab_param, sizeof(ipc_clt_cable_param_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_CLT_CABLE_PARAM;
	req.hdr.msg_len = REQ_MSGLEN(ipc_clt_cable_param_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_clt_cable_param_ack_t, ipc_fd);
	return pack;
}
#if 0
ipc_clt_interface_mibs_ack_t *ipc_clt_interface_mibs(int ipc_fd, int access, ipc_ifmibs_t *mibsreq, int count)
{
	ipc_clt_interface_mibs_req_t *req = NULL;
	ipc_clt_interface_mibs_ack_t *pack;

	if (mibsreq == NULL) count = 0;
	req = (ipc_clt_interface_mibs_req_t *)malloc(offsetof(ipc_clt_interface_mibs_req_t, mibs_req) + sizeof(ipc_ifmibs_t) * count);
	DBG_ASSERT(req, "fail to allocate memory!");
	if(!req) return NULL;
	if(mibsreq && count) memcpy(req->mibs_req, mibsreq, sizeof(ipc_ifmibs_t) * count);
	req->count = count;
	req->access = access;
	req->hdr.ipc_type = IPC_CLT_INTERFACE_MIB;
	req->hdr.msg_len =  REQ_MSGLEN_R(ipc_clt_interface_mibs_req_t, mibs_req, ipc_ifmibs_t, count);
	ipc_send_request(ipc_fd, req);
	free(req);
	IPC_RECEIVE(pack, ipc_clt_interface_mibs_ack_t, ipc_fd);
	return pack;
}



ipc_clt_interface_status_ack_t *ipc_clt_interface_status(int ipc_fd, char *ifname)
{
	ipc_clt_interface_status_req_t req;
	ipc_clt_interface_status_ack_t *pack;

	memset(&req, 0, sizeof(ipc_clt_interface_status_req_t));
	if(ifname)
		strncpy(req.ifname, ifname, sizeof(req.ifname) - 1);
	req.hdr.ipc_type = IPC_CLT_INTERFACE_STATUS;
	req.hdr.msg_len = REQ_MSGLEN(ipc_clt_interface_status_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_clt_interface_status_ack_t, ipc_fd);
	return pack;
}

ipc_clt_interface_config_ack_t *ipc_clt_interface_config(int ipc_fd, uint8_t access, uint8_t apply_option, switch_interface_t *cfg, uint16_t count)
{
	ipc_clt_interface_config_req_t *req = NULL;
	ipc_clt_interface_config_ack_t *pack;

	if (cfg == NULL) count = 0;
	req = (ipc_clt_interface_config_req_t *)malloc(offsetof(ipc_clt_interface_config_req_t, ifs) + sizeof(switch_interface_t) * count);
	DBG_ASSERT(req, "fail to allocate memory!");
	if(!req) return NULL;
	if(cfg && count) memcpy(req->ifs, cfg, sizeof(switch_interface_t) * count);
	req->count = count;
	req->access = access;
	req->apply_option = apply_option;
	req->hdr.ipc_type = IPC_CLT_INTERFACE;
	req->hdr.msg_len =  REQ_MSGLEN_R(ipc_clt_interface_config_req_t, ifs, switch_interface_t, count);
	ipc_send_request(ipc_fd, req);
	free(req);
	IPC_RECEIVE(pack, ipc_clt_interface_config_ack_t, ipc_fd);
	return pack;
}

#endif

ipc_clt_vlan_interface_config_ack_t *ipc_clt_vlan_interface_config(int ipc_fd, uint8_t access, uint8_t apply_option, switch_vlan_interface_t *vlanif, uint16_t count)
{
	ipc_clt_vlan_interface_config_req_t *req;
	ipc_clt_vlan_interface_config_ack_t *pack;
	uint32_t data_size = vlanif ? vlan_interface_mem_size(vlanif, count) : 0;

	req = (ipc_clt_vlan_interface_config_req_t *)malloc(offsetof(ipc_clt_vlan_interface_config_req_t, vlanifs) + data_size);
	DBG_ASSERT(req, "fail to allocate memory!");
	if(!req) return NULL;
	if(vlanif) memcpy(req->vlanifs, vlanif, data_size);
	req->count = count;
	req->access = access;
	req->apply_option = apply_option;
	req->hdr.ipc_type = IPC_CLT_VLAN_INTERFACE;
	req->hdr.msg_len = offsetof(ipc_clt_vlan_interface_config_req_t, vlanifs) + data_size - sizeof(ipc_request_hdr_t);
	ipc_send_request(ipc_fd, req);
	free(req);
	IPC_RECEIVE(pack, ipc_clt_vlan_interface_config_ack_t, ipc_fd);
	return pack;
}


ipc_clt_vlan_mode_ack_t *ipc_clt_vlan_mode_config(int ipc_fd, uint8_t access, uint8_t apply_option, switch_vlan_mode_t *vlan_mode)
{
	ipc_clt_vlan_mode_req_t req;
	ipc_clt_vlan_mode_ack_t *pack;

	memset(&req, 0, sizeof(req));
	if(vlan_mode) memcpy(&req.vlanmode, vlan_mode, sizeof(switch_vlan_mode_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_CLT_VLAN_MODE;
	req.hdr.msg_len = sizeof(ipc_clt_vlan_mode_req_t) - sizeof(ipc_request_hdr_t);
	ipc_send_request(ipc_fd, &req);
	IPC_RECEIVE(pack, ipc_clt_vlan_mode_ack_t, ipc_fd);
	return pack;
}


ipc_clt_vlan_config_ack_t *ipc_clt_vlan_config(int ipc_fd, uint8_t access, uint8_t apply_option, switch_vlan_group_t *vlan_group, uint16_t count)
{
	ipc_clt_vlan_config_req_t *req;
	ipc_clt_vlan_config_ack_t *pack;
	uint32_t data_size = vlan_group ? vlan_group_mem_size(vlan_group, count) : 0;

	req = (ipc_clt_vlan_config_req_t *)malloc(offsetof(ipc_clt_vlan_config_req_t, vlan_groups) + data_size);
	DBG_ASSERT(req, "fail to allocate memory!");
	if(!req) return NULL;
	if(vlan_group) memcpy(req->vlan_groups, vlan_group, data_size);
	req->count = count;
	req->access = access;
	req->apply_option = apply_option;
	req->hdr.ipc_type = IPC_CLT_VLAN;
	req->hdr.msg_len = offsetof(ipc_clt_vlan_config_req_t, vlan_groups) + data_size - sizeof(ipc_request_hdr_t);
	ipc_send_request(ipc_fd, req);
	free(req);
	IPC_RECEIVE(pack, ipc_clt_vlan_config_ack_t, ipc_fd);
	//printf("pack->count =%d...\n",pack->count);
	return pack;
}


ipc_supported_device_ack_t *ipc_supported_device(int ipc_fd)
{
	ipc_supported_device_req_t req;
	ipc_supported_device_ack_t *pack;	

	req.hdr.ipc_type = IPC_SUPPORTED_DEVICE;
	req.hdr.msg_len = 0;
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_supported_device_ack_t, ipc_fd);
	return pack;
}
int ipc_set_vlan(int ipc_fd,int access, switch_vlan_group_t *vg)
{
        ipc_clt_vlan_config_ack_t *pack;
        int ret;

        pack = ipc_clt_vlan_config(ipc_fd, access, IPC_APPLY_NOW, vg, 1);
        if (pack){
                ret = pack->hdr.status;
        }else {
                ret = IPC_STATUS_FAIL;
        }

        if(pack) free(pack);
        return ret;
}
int ipc_set_vlan_mode(int ipc_fd,switch_vlan_mode_t *mode)
{
	ipc_clt_vlan_mode_ack_t *pack;
	int ret;

	pack = ipc_clt_vlan_mode_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, mode);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_get_vlan_mode(int ipc_fd,switch_vlan_mode_t *mode)
{
	ipc_clt_vlan_mode_ack_t *pack;
	int ret;
	
	pack = ipc_clt_vlan_mode_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW, NULL);
	
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		if (mode){
			memcpy(mode, &pack->vlanmode, sizeof(*mode));
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
/*begin added by wanghuanyu for 129*/
int ipc_shutdown_intf(int ipc_fd,ifindex_t ifindex,int op)
{	
	int ret=0;
	
	ipc_shutdown_interface_req_t *req;
	ipc_shutdown_interface_ack_t *pack;

	req = (ipc_shutdown_interface_req_t *)malloc(sizeof(ipc_shutdown_interface_req_t));
	DBG_ASSERT(req, "fail to allocate memory!");
	if(!req) return NULL;	
	req->apply_option = IPC_APPLY_NOW;
	req->uiifindex=ifindex;
	req->op=op;
	req->hdr.ipc_type = IPC_SHUTDOWN_INTF;
	req->hdr.msg_len =sizeof(ipc_shutdown_interface_req_t) - sizeof(ipc_request_hdr_t);;
	ipc_send_request(ipc_fd, req);
	free(req);

	IPC_RECEIVE(pack, ipc_shutdown_interface_ack_t, ipc_fd);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		
		ret = 0;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
	//return pack;
}
/*end added by wanghuanyu for 129*/
#if 0
ipc_cnu_user_config_ack_t *ipc_cnu_user_config(int ipc_fd, uint8_t filter, uint8_t access, uint8_t apply_option, cnu_user_config_t *cfg, uint16_t count)
{
	ipc_cnu_user_config_req_t *req;
	ipc_cnu_user_config_ack_t *pack;

	req = (ipc_cnu_user_config_req_t *)malloc(offsetof(ipc_cnu_user_config_req_t, user) + sizeof(cnu_user_config_t) * count);
	DBG_ASSERT(req, "fail to allocate memory!");
	if(!req) return NULL;
	if(cfg) memcpy(req->user, cfg, sizeof(cnu_user_config_t) * count);
	req->filter = filter;
	req->count = count;
	req->access = access;
	req->apply_option = apply_option;
	req->hdr.ipc_type = IPC_CNU_USER;
	req->hdr.msg_len = REQ_MSGLEN_R(ipc_cnu_user_config_req_t, user, cnu_user_config_t, count);
	ipc_send_request(ipc_fd, req);
	free(req);

	IPC_RECEIVE(pack, ipc_cnu_user_config_ack_t, ipc_fd);
	return pack;
}

ipc_service_template_ack_t *ipc_service_template(int ipc_fd, uint8_t access, uint8_t apply_option, ipc_service_template_t *cfg, uint16_t count)
{
	ipc_service_template_req_t *req;
	ipc_service_template_ack_t *pack;

	req = (ipc_service_template_req_t *)malloc(offsetof(ipc_service_template_req_t, templates) + sizeof(ipc_service_template_t) * count);
	DBG_ASSERT(req, "fail to allocate memory!");
	if(!req) return NULL;
	if(cfg) memcpy(req->templates, cfg, sizeof(ipc_service_template_t) * count);
	req->count = count;
	req->access = access;
	req->apply_option = apply_option;
	req->hdr.ipc_type = IPC_SERVICE_TEMPLATE;
	req->hdr.msg_len = REQ_MSGLEN_R(ipc_service_template_req_t, templates, ipc_service_template_t, count);
	ipc_send_request(ipc_fd, req);
	free(req);

	IPC_RECEIVE(pack, ipc_service_template_ack_t, ipc_fd);
	return pack;
}

#endif
ipc_sys_security_ack_t *ipc_sys_security(int ipc_fd, uint8_t access, uint8_t apply_option, sys_security_t *cfg, uint8_t mask)
{
	ipc_sys_security_req_t req;
	ipc_sys_security_ack_t *pack;

	if (cfg) memcpy(&req.security, cfg, sizeof(sys_security_t));
	req.mask = mask;
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_SYS_SECURITY;
	req.hdr.msg_len = REQ_MSGLEN(ipc_sys_security_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_sys_security_ack_t, ipc_fd);
	return pack;
}


ipc_sys_info_ack_t *ipc_sys_info(int ipc_fd)
{
	ipc_sys_info_req_t req;
	ipc_sys_info_ack_t *pack;

	req.hdr.ipc_type = IPC_SYS_INFO;
	req.hdr.msg_len = 0;
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_sys_info_ack_t, ipc_fd);
	return pack;
}


ipc_sys_network_ack_t *ipc_sys_networking(int ipc_fd, uint8_t access, uint8_t apply_option, ipc_network_t *network)
{
	ipc_sys_network_req_t req;
	ipc_sys_network_ack_t *pack;

	if (network) memcpy(&req.network, network, sizeof(ipc_network_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_SYS_NETWORKING;
	req.hdr.msg_len = REQ_MSGLEN(ipc_sys_network_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_sys_network_ack_t, ipc_fd);
	return pack;
}


ipc_sys_snmp_ack_t *ipc_sys_snmp(int ipc_fd, uint8_t access, uint8_t apply_option, sys_snmp_t *cfg)
{
	ipc_sys_snmp_req_t req;
	ipc_sys_snmp_ack_t *pack;

	if (cfg) memcpy(&req.snmp, cfg, sizeof(sys_snmp_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_SYS_SNMP;
	req.hdr.msg_len = REQ_MSGLEN(ipc_sys_snmp_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_sys_snmp_ack_t, ipc_fd);
	return pack;
}
/*****************************************************************
Function:ipc_sys_syslog
Description: 
Author:zhouguanhua
Date:2013/4/5
INPUT:    
OUTPUT:         
Return:
Others:	       
*****************************************************************/
ipc_syslog_ack_t *ipc_sys_syslog(int ipc_fd, uint8_t access, uint8_t apply_option, sys_syslog_t *cfg)
{
	ipc_syslog_req_t req;
	ipc_syslog_ack_t *pack;

	if (cfg) memcpy(&req.syslog, cfg, sizeof(sys_syslog_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_SYSLOG;
	req.hdr.msg_len = REQ_MSGLEN(ipc_syslog_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_syslog_ack_t, ipc_fd);
	return pack;
}


/*****************************************************************
Function:ipc_mac_setting_config
Description: 
Author:huangmingjian
Date:2013/09/08
INPUT:    
OUTPUT:         
Return:
Others:	       
*****************************************************************/

ipc_mac_setting_ack_t *ipc_mac_setting_config(int ipc_fd, uint8_t access, uint8_t apply_option, mac_setting_t *cfg)
{
	ipc_mac_setting_req_t req;
	ipc_mac_setting_ack_t *pack;

	if (cfg) memcpy(&req.mac_setting, cfg, sizeof(mac_setting_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_MAC_SETTING;
	req.hdr.msg_len = REQ_MSGLEN(ipc_mac_setting_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_mac_setting_ack_t, ipc_fd);
	return pack;
}


ipc_sys_admin_ack_t *ipc_sys_admin(int ipc_fd, uint8_t access, uint8_t apply_option, sys_admin_t *cfg)
{
	ipc_sys_admin_req_t req;
	ipc_sys_admin_ack_t *pack;

	if (cfg) memcpy(&req.admin, cfg, sizeof(sys_admin_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_SYS_ADMIN;
	req.hdr.msg_len = REQ_MSGLEN(ipc_sys_admin_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_sys_admin_ack_t, ipc_fd);
	return pack;
}


ipc_sys_time_ack_t *ipc_sys_time(int ipc_fd, uint8_t access, uint8_t apply_option, sys_time_t *cfg)
{
	ipc_sys_time_req_t req;
	ipc_sys_time_ack_t *pack;

	if (cfg) memcpy(&req.time, cfg, sizeof(sys_time_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_SYS_TIME;
	req.hdr.msg_len = REQ_MSGLEN(ipc_sys_time_req_t);
	ipc_send_request(ipc_fd, &req);
	
	IPC_RECEIVE(pack, ipc_sys_time_ack_t, ipc_fd);
	return pack;
}

ipc_sys_loop_ack_t *ipc_sys_loop(int ipc_fd, uint8_t access, uint8_t apply_option, sys_loop_t *cfg)
{
	ipc_sys_loop_req_t req;
	ipc_sys_loop_ack_t *pack;
	if (cfg) memcpy(&req.loop, cfg, sizeof(sys_loop_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_LOOP;
	req.hdr.msg_len = REQ_MSGLEN(ipc_sys_loop_req_t);
	ipc_send_request(ipc_fd, &req);
	IPC_RECEIVE(pack, ipc_sys_loop_ack_t, ipc_fd);
	return pack;
}

ipc_sys_mvlan_ack_t *ipc_sys_mvlan(int ipc_fd, uint8_t access, uint8_t apply_option, sys_mvlan_t *cfg)
{
	ipc_sys_mvlan_req_t req;
	ipc_sys_mvlan_ack_t *pack;
	if (cfg) memcpy(&req.mvlan, cfg, sizeof(sys_mvlan_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_MVLAN;
	req.hdr.msg_len = REQ_MSGLEN(ipc_sys_mvlan_req_t);
	ipc_send_request(ipc_fd, &req);
	IPC_RECEIVE(pack, ipc_sys_mvlan_ack_t, ipc_fd);
	return pack;
}

/*
ipc_acknowledge_hdr_t *ipc_sys_dhcpc_event(int ipc_fd, sys_dhcpc_t *dhcpc)
{
	ipc_sys_dhcpc_req_t req;

	req.hdr.ipc_type = IPC_SYS_DHCPC_EVENT;
	if (dhcpc) memcpy(&req.dhcpc, dhcpc, sizeof(sys_dhcpc_t));
	req.hdr.msg_len = REQ_MSGLEN(ipc_sys_dhcpc_req_t);
	ipc_send_request(ipc_fd, &req);
	
	return (ipc_acknowledge_hdr_t *)ipc_recv_acknowledge(ipc_fd);
}
*/
/*Begin added by feihuaxin for qos 2013-7-10*/
ipc_sys_qos_ack_t *ipc_qos_trustmode(int ipc_fd, uint8_t access, uint8_t apply_option, uint32_t QosTrustmode)
{
    ipc_sys_qos_req_t req;
    ipc_sys_qos_ack_t *pack;
    
    req.uiQosTrustmode = QosTrustmode;
    req.access = access;
    req.apply_option = apply_option;
    req.hdr.ipc_type = IPC_QOS_TRUSTMODE;
    req.hdr.msg_len = REQ_MSGLEN(ipc_sys_qos_req_t);
    ipc_send_request(ipc_fd, &req);
    
    IPC_RECEIVE(pack, ipc_sys_qos_ack_t, ipc_fd);
    return pack;
}

ipc_sys_qosqueuemod_ack_t *ipc_qos_queuemode(int ipc_fd, uint8_t access, uint8_t apply_option, QueueMode_S *cfg)
{
    ipc_sys_qosqueuemod_req_t req;
    ipc_sys_qosqueuemod_ack_t *pack;
    
    if (NULL != cfg) 
    {
        memcpy(&req.Qosqueuemode, cfg, sizeof(QueueMode_S));
    }
    req.access = access;
    req.apply_option = apply_option;
    req.hdr.ipc_type = IPC_QOS_QUEUESCH;
    req.hdr.msg_len = REQ_MSGLEN(ipc_sys_qosqueuemod_req_t);
    ipc_send_request(ipc_fd, &req);
    
    IPC_RECEIVE(pack, ipc_sys_qosqueuemod_ack_t, ipc_fd);
    return pack;
}
/*End added by feihuaxin for qos 2013-7-10*/


ipc_sys_qosqueuemod_ack_t *ipc_port_stormctl(int ipc_fd, uint8_t access, uint8_t apply_option, STROM_CTL_S *cfg)
{
    ipc_stormctl_req_t req;
    ipc_stormctl_ack_t *pack;
    
    if (NULL != cfg) 
    {
        memcpy(req.stormctl, cfg, sizeof(STROM_CTL_S)*LOGIC_PORT_NO);
    }
    req.access = access;
    req.apply_option = apply_option;
    req.hdr.ipc_type = IPC_STORM_CTL;
    req.hdr.msg_len = REQ_MSGLEN(ipc_stormctl_req_t);
    ipc_send_request(ipc_fd, &req);
    
    IPC_RECEIVE(pack, ipc_stormctl_ack_t, ipc_fd);
    return pack;
}

ipc_port_ack_t *ipc_port_req(int ipc_fd, uint8_t access, uint8_t apply_option,  PORT_CFG_VALUE_S *port_cfg_val)
{
    ipc_port_req_t req;
    ipc_port_ack_t *pack;
    
    if (NULL != port_cfg_val) 
    {
        memcpy(req.port_cfg_val, port_cfg_val, sizeof(PORT_CFG_VALUE_S)*LOGIC_PORT_NO);
    }
    
    req.access = access;
    req.apply_option = apply_option;
    req.hdr.ipc_type = IPC_PORT;
    req.hdr.msg_len = REQ_MSGLEN(ipc_port_req_t);
    ipc_send_request(ipc_fd, &req);
    
    IPC_RECEIVE(pack, ipc_port_ack_t, ipc_fd);
    return pack;
}

ipc_linerate_ack_t *ipc_linerate_req(int ipc_fd, uint8_t access, uint8_t apply_option,  PORT_LINERATE_VALUES *port_linerate)
{
    ipc_linerate_req_t req;
    ipc_linerate_ack_t *pack;
    
    if (NULL != port_linerate) 
    {
        memcpy(req.linerate_val, port_linerate, sizeof(PORT_LINERATE_VALUES)*LOGIC_PORT_NO);
    }
    
    req.access = access;
    req.apply_option = apply_option;
    req.hdr.ipc_type = IPC_LINERATE;
    req.hdr.msg_len = REQ_MSGLEN(ipc_linerate_req_t);
    ipc_send_request(ipc_fd, &req);
    
    IPC_RECEIVE(pack, ipc_linerate_ack_t, ipc_fd);
    return pack;
}

ipc_mirror_ack_t *ipc_mirror_req(int ipc_fd, uint8_t access, uint8_t apply_option,  MIRROR_GROUP_S *port_mirror)
{
    ipc_mirror_req_t req;
    ipc_mirror_ack_t *pack;
    
    if (NULL != port_mirror) 
    {
        memcpy(&req.portmirror, port_mirror, sizeof(MIRROR_GROUP_S));
    }
    
    req.access = access;
    req.apply_option = apply_option;
    req.hdr.ipc_type = IPC_MIRROR;
    req.hdr.msg_len = REQ_MSGLEN(ipc_mirror_req_t);
    ipc_send_request(ipc_fd, &req);
    
    IPC_RECEIVE(pack, ipc_mirror_ack_t, ipc_fd);
    return pack;
}

ipc_system_ack_t *ipc_system_req(int ipc_fd, uint8_t access, uint8_t process)
{
	ipc_system_req_t req;
	ipc_system_ack_t *pack;

    if (ipc_fd < 0)return NULL;

	req.hdr.ipc_type = IPC_SYSTEM;
	req.access = access;
	req.process = process;
	req.hdr.msg_len = REQ_MSGLEN(ipc_system_req_t);
	ipc_send_request(ipc_fd, &req);
	
	IPC_RECEIVE(pack, ipc_system_ack_t, ipc_fd);
	return pack;
}

ipc_sys_epon_ack_t *ipc_sys_epon(int ipc_fd, uint8_t access, uint8_t apply_option, sys_epon_t *cfg)
{
	ipc_sys_epon_req_t req;
	ipc_sys_epon_ack_t *pack;
	if (cfg) memcpy(&req.epon_info, cfg, sizeof(sys_epon_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_EPON;
	req.hdr.msg_len = REQ_MSGLEN(ipc_sys_epon_req_t);
	ipc_send_request(ipc_fd, &req);
	IPC_RECEIVE(pack, ipc_sys_epon_ack_t, ipc_fd);
	return pack;
}
/*Begin modified for bug 199 by sunmingliang*/
int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
	struct flock    lock;
	memset(&lock, 0, sizeof(struct flock ));
	lock.l_type = type;     /* F_RDLCK, F_WRLCK, F_UNLCK */
	lock.l_start = offset;  /* byte offset, relative to l_whence */
	lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END */
	lock.l_len = len;       /* #bytes (0 means to EOF) */

	return(fcntl(fd, cmd, &lock) );
}
/*End modified for bug 199 by sunmingliang*/

/*Begin add by linguobin 2013-12-6*/
#if defined(CONFIG_ONU_COMPATIBLE)
ipc_compatible_setting_ack_t *ipc_compatible_setting_config(int ipc_fd, uint8_t access, uint8_t apply_option, compatible_setting_t *cfg)
{
	ipc_compatible_setting_req_t req;
	ipc_compatible_setting_ack_t *pack;

    if (ipc_fd < 0) return NULL;

	if (cfg) memcpy(&req.compatible_setting, cfg, sizeof(compatible_setting_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_COMPATIBLE_SETTING;
	req.hdr.msg_len = REQ_MSGLEN(ipc_compatible_setting_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_compatible_setting_ack_t, ipc_fd);
	return pack;
}

int oam_ipc_get_sys_compatible(int ipc_fd,compatible_setting_t *info)
{
	ipc_compatible_setting_ack_t *pack;
	int ret;
    if (ipc_fd < 0) return IPC_STATUS_OK;
	pack = ipc_compatible_setting_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		if (info){
			memcpy(info, &pack->compatible_setting, sizeof(*info));
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

#endif
/*End add by linguobin 2013-12-6*/

#ifdef  CONFIG_CATVCOM

/*****************************************************************
Function:		ipc_catv_config
Description:    ipc_catv_config
Author:		huangmingjian
Date:		2013/10/15
INPUT:    
OUTPUT:         
Return:
Others:	       
*****************************************************************/

ipc_catvinfo_ack_t *ipc_catv_config(int ipc_fd, uint8_t access, uint8_t apply_option, catvinfo_t *cfg)
{
	ipc_catvinfo_req_t req;
	ipc_catvinfo_ack_t *pack;

	if (cfg) memcpy(&req.catvinfo, cfg, sizeof(catvinfo_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_CATV;
	req.hdr.msg_len = REQ_MSGLEN(ipc_catvinfo_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_catvinfo_ack_t, ipc_fd);
	return pack;
}

#endif


#ifdef  CONFIG_WLAN_AP

char *enable_str[] = {
	"disable",
	"enable",
	NULL
};

char *model_str[] = {
	"",
	"b_only",
	"g_only",
	"b_g",
	"n_only",
	"b_g_n",
	NULL
};

char *bandwidth_str[] = {
	"",
	"20MHZ",
	"40MHZ",
	NULL
};


char *channel_str[] = {
	"0_AUTO",
	"1_2_412GHZ",
	"2_2_417GHZ",
	"3_2_422GHZ",
	"4_2_427GHZ",
	"5_2_432GHZ",
	"6_2_437GHZ",
	"7_2_442GHZ",
	"8_2_447GHZ",
	"9_2_452GHZ",
	"10_2_457GHZ",
	"11_2_462GHZ",
	"12_2_467GHZ",
	"13_2_472GHZ",
	NULL
};

char *tx_rate_str[] = {
	"0_AUTO",
	"1_1MBPS",
	"2_2MBPS",
	"3_5_5MBPS",
	"4_6MBPS",
	"5_6_5MBPS",
	"6_9MBPS",
	"7_11MBPS",
	"8_12MBPS",
	"9_13MBPS",
	"10_15MBPS",
	"11_18MBPS",
	"12_19_5MBPS",
	"13_24MBPS",
	"14_26MBPS",
	"15_30MBPS",
	"16_36MBPS",
	"17_39MBPS",
	"18_45MBPS",
	"19_48MBPS",
	"20_52MBPS",
	"21_54MBPS",
	"22_58_5MBPS",
	"23_60MBPS",
	"24_65MBPS",
	"25_90MBPS",
	"26_120MBPS",
	"27_135MBPS",
	"28_150MBPS",
	NULL
};

char *filter_model_str[] = {"only_forbiden", "only_allow", NULL};
char *enc_wep_key_len_str[] = {"", "64b_10H_5A", "128b_26H_13A", NULL};
char *enc_wep_key_type_str[] = {"", "HEX", "ASCII", NULL};
char *enc_wpa_enc_pro_str[] = {"", "TKIP", "AES", "TKIP_AES", NULL};
char *enc_str[] = {
	"NONE",
	"WEP",
	"WPA_PSK",
	"WPA2_PSK",
	"WPA_PSK_WPA2_PSK",	
	NULL
};

char *authtype_str[] = {
	"open_system",
	"shared_key",
	"auto",	
	NULL
};



/*****************************************************************
Function:ipc_wlan_ap_config
Description: 
Author:huangmingjian
Date:2013/10/05
INPUT:    
OUTPUT:         
Return:
Others:	       
*****************************************************************/

ipc_wlan_ap_ack_t *ipc_wlan_ap_config(int ipc_fd, uint8_t access, uint8_t apply_option, wlan_ap_t *cfg)
{
	ipc_wlan_ap_req_t req;
	ipc_wlan_ap_ack_t *pack;

	if (cfg) memcpy(&req.wlan_ap, cfg, sizeof(wlan_ap_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_WLAN_AP;
	req.hdr.msg_len = REQ_MSGLEN(ipc_wlan_ap_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_wlan_ap_ack_t, ipc_fd);
	return pack;
}

#endif
/*Begin add by linguobin 2014-04-22 for mib*/
#if defined(CONFIG_ZBL_SNMP)
ipc_analogPropertyTable_ack_t *ipc_analogPropertyTable(int ipc_fd, uint8_t access, uint8_t apply_option, AnalogProperty_t *cfg)
{
	ipc_analogPropertyTable_req_t req;
	ipc_analogPropertyTable_ack_t *pack;

	if (cfg) memcpy(&req.AnalogProperty, cfg, sizeof(AnalogProperty_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_ANALOGPROPERTYTABLE;
	req.hdr.msg_len = REQ_MSGLEN(ipc_analogPropertyTable_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_analogPropertyTable_ack_t, ipc_fd);
	return pack;
}

ipc_commonAdminGroup_ack_t *ipc_commonAdminGroup(int ipc_fd, uint8_t access, uint8_t apply_option, commonAdminGroup_t *cfg)
{
	ipc_commonAdminGroup_req_t req;
	ipc_commonAdminGroup_ack_t *pack;

	if (cfg) memcpy(&req.commonAdminGroup, cfg, sizeof(commonAdminGroup_t));
	req.access = access;
	req.apply_option = apply_option;
	req.hdr.ipc_type = IPC_COMMONADMINGROUP;
	req.hdr.msg_len = REQ_MSGLEN(ipc_commonAdminGroup_req_t);
	ipc_send_request(ipc_fd, &req);

	IPC_RECEIVE(pack, ipc_commonAdminGroup_ack_t, ipc_fd);
	return pack;
}
#endif
/*End add by linguobin 2014-04-22 for mib*/

