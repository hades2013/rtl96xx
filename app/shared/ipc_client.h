/*
 * ipc_client.h
 *
 *  Created on: Jan 6, 2011
 *      Author: root
 */

#ifndef IPC_CLIENT_H_
#define IPC_CLIENT_H_

#include <ipc_protocol.h>
//#include "ipc_cnu_service.h"


typedef struct {
	char *fancy_state_string;
	char *short_state_string;
}
cnu_state_string_t;

extern cnu_state_string_t cnu_state_string[];

#define	IPC_ACK_NULL_ASSERT(x) do { DBG_ASSERT(x, "NO ACK"); if(x) { DBG_ASSERT(x->hdr.status == IPC_STATUS_OK, "IPC error: %d:%s", x->hdr.status, ipc_strerror(x->hdr.status)); } } while(0)
#define	IPC_ACK_OK(x) ((x && (x->hdr.status == IPC_STATUS_OK)))

int ipc_client_connect(char *process_name);

void ipc_send_request(int ipc_fd, void *msg);

ipc_acknowledge_hdr_t *ipc_recv_acknowledge(int ipc_fd);

int sysmon_cmd(int ipc_fd, uint32_t cmd);

char *ipc_strerror(int err);
ipc_nvram_access_ack_t *ipc_nvram_access(int ipc_fd, uint8_t access, char *name, char *value);
ipc_mme_stat_ack_t *ipc_mme_stat(int ipc_fd, uint8_t clean);
ipc_network_info_ack_t *ipc_network_info(int ipc_fd, uint8_t clt, uint8_t update);
ipc_cnu_link_stats_ack_t *ipc_cnu_link_stats(int ipc_fd, int filter_type, uint8_t clt, uint8_t cnu, ethernet_addr_t *cnu_mac);
ipc_cnu_info_ack_t *ipc_cnu_info(int ipc_fd, int filter_type, uint8_t clt, uint8_t cnu, ethernet_addr_t *cnu_mac);
ipc_cnu_mib_ack_t *ipc_cnu_mib(int ipc_fd, int clean, int filter_type, uint8_t clt, uint8_t cnu, ethernet_addr_t *cnu_mac);
ipc_clt_cable_param_ack_t * ipc_clt_cable_param (int ipc_fd, uint8_t access, uint8_t apply_option, ipc_clt_cable_param_t* cab_param);
//ipc_clt_vlan_config_ack_t *ipc_clt_vlan_config(int ipc_fd, uint8_t access, uint8_t apply_option, switch_vlan_group_t *vlan_group, uint16_t count);
//ipc_clt_vlan_mode_ack_t *ipc_clt_vlan_mode_config(int ipc_fd, uint8_t access, uint8_t apply_option, switch_vlan_mode_t *vlan_mode);
//ipc_clt_vlan_interface_config_ack_t *ipc_clt_vlan_interface_config(int ipc_fd, uint8_t access, uint8_t apply_option, switch_vlan_interface_t *vlanif, uint16_t count);
//ipc_cnu_user_config_ack_t *ipc_cnu_user_config(int ipc_fd, uint8_t filter, uint8_t access, uint8_t apply_option, cnu_user_config_t *cfg, uint16_t count);
//ipc_clt_interface_status_ack_t *ipc_clt_interface_status(int ipc_fd, char *ifname);
//ipc_clt_interface_mibs_ack_t *ipc_clt_interface_mibs(int ipc_fd, int access, ipc_ifmibs_t *mibsreq, int count);
//ipc_clt_interface_config_ack_t *ipc_clt_interface_config(int ipc_fd, uint8_t access, uint8_t apply_option, switch_interface_t *cfg, uint16_t count);
//ipc_service_template_ack_t *ipc_service_template(int ipc_fd, uint8_t access, uint8_t apply_option, ipc_service_template_t *cfg, uint16_t count);
ipc_supported_device_ack_t *ipc_supported_device(int ipc_fd);

ipc_sys_security_ack_t *ipc_sys_security(int ipc_fd, uint8_t access, uint8_t apply_option, sys_security_t *cfg, uint8_t mask);
ipc_sys_info_ack_t *ipc_sys_info(int ipc_fd);
ipc_sys_network_ack_t *ipc_sys_networking(int ipc_fd, uint8_t access, uint8_t apply_option, ipc_network_t *network);
ipc_sys_snmp_ack_t *ipc_sys_snmp(int ipc_fd, uint8_t access, uint8_t apply_option, sys_snmp_t *cfg);
ipc_sys_admin_ack_t *ipc_sys_admin(int ipc_fd, uint8_t access, uint8_t apply_option, sys_admin_t *cfg);
ipc_sys_time_ack_t *ipc_sys_time(int ipc_fd, uint8_t access, uint8_t apply_option, sys_time_t *cfg);
ipc_system_ack_t *ipc_system_req(int ipc_fd, uint8_t access, uint8_t process);
ipc_get_user_info_ack_t *ipc_nvram_get_user_info(int ipc_fd, uint8_t access, ipc_all_vty_user_info_t *vty_user_info);
ipc_sys_loop_ack_t *ipc_sys_loop(int ipc_fd, uint8_t access, uint8_t apply_option, sys_loop_t *cfg);
ipc_sys_epon_ack_t *ipc_sys_epon(int ipc_fd, uint8_t access, uint8_t apply_option, sys_epon_t *cfg);

//add by zhouguanhua 2013/6/6
ipc_sys_mvlan_ack_t *ipc_sys_mvlan(int ipc_fd, uint8_t access, uint8_t apply_option, sys_mvlan_t *cfg);
ipc_syslog_ack_t *ipc_sys_syslog(int ipc_fd, uint8_t access, uint8_t apply_option, sys_syslog_t *cfg);
int ipc_cfg_set(ifindex_t ifindex,unsigned uioid, char *value);
ipc_sys_qos_ack_t *ipc_qos_trustmode(int ipc_fd, uint8_t access, uint8_t apply_option, uint32_t QosTrustmode);
ipc_sys_qosqueuemod_ack_t *ipc_qos_queuemode(int ipc_fd, uint8_t access, uint8_t apply_option, QueueMode_S *cfg);
//end by zhouguanhua 2013/6/6
#if defined(CONFIG_ZBL_SNMP)
ipc_analogPropertyTable_ack_t *ipc_analogPropertyTable(int ipc_fd, uint8_t access, uint8_t apply_option, AnalogProperty_t *cfg);
ipc_commonAdminGroup_ack_t *ipc_commonAdminGroup(int ipc_fd, uint8_t access, uint8_t apply_option, commonAdminGroup_t *cfg);
#endif

#endif /* IPC_CLIENT_H_ */
