
#ifndef __HEXICOM_H__
#define __HEXICOM_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



#include <ipc_client.h>
#include <switch.h>
#include <vlan_utils.h>
#include <str_utils.h>
#include <debug.h>
#include <valids.h>
#include <shutils.h>
#include <vendor.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>


extern int ipc_fd;

#define IFNAME_CPU0  "cpu0"
#define IFNAME_ETH0  "eth0"
#define IFNAME_ETH1  "eth1"
#define IFNAME_CAB0  "cab0"
#define IFNAME_CAB1  "cab1"

#define VLANIF_CAB0  0x80
#define VLANIF_CAB1  0x40
#define VLANIF_ETH0  0x20
#define VLANIF_ETH1  0x10



#define VLANIF_ALL (VLANIF_CAB0 | VLANIF_CAB1 | VLANIF_ETH0 | VLANIF_ETH1)
#define VLAN_IFLIST  IFNAME_CAB0, IFNAME_CAB1, IFNAME_ETH0, IFNAME_ETH1



#define HC_IPC_ASSERT(ret) do {\
								if (ret != 0){\
									snmp_log(LOG_ERR, "IPC ERROR %s\n", ipc_strerror(ret)); \
									return 0;\
								}\
							}while(0) 

#define HC_NOMEM_ASSERT(mem) do {\
								if (mem == NULL){\
									snmp_log(LOG_ERR, "Memory Error\n"); \
									return 0;\
								}\
							}while(0) 

#define HC_CNU_ASSERT(ret) do {\
								if (ret != 0){\
									snmp_log(LOG_ERR, "CNU ERROR %s\n", ipc_strerror(ret)); \
									return 0;\
								}\
							}while(0) 

#define HC_IPC_ASSERT1(ret) do {\
							if (ret != 0){\
								snmp_log(LOG_ERR, "IPC ERROR %s\n", ipc_strerror(ret)); \
								return ;\
							}\
						}while(0) 

#define HC_NOMEM_ASSERT1(mem) do {\
							if (mem == NULL){\
								snmp_log(LOG_ERR, "Memory Error\n"); \
								return ;\
							}\
						}while(0) 

#define HC_CNU_ASSERT1(ret) do {\
							if (ret != 0){\
								snmp_log(LOG_ERR, "CNU ERROR %s\n", ipc_strerror(ret)); \
								return ;\
							}\
						}while(0) 

							
#define IPC_FD_ASSERT(fd) do {\
							if (fd <= 0){\
								snmp_log(LOG_ERR, "Invalid IPC FD = %d\n", fd); \
								return 0;\
							}\
						}while(0) 


#define HC_IPC_ERROR(ret)  (ret != IPC_STATUS_OK)

int ipc_get_sys_info(sys_info_t *info);
int ipc_get_sys_security(sys_security_t *info);
int ipc_get_sys_networking(sys_network_t *info);
int ipc_get_sys_admin(sys_admin_t *info);
int ipc_get_sys_time(sys_time_t *info);
int ipc_set_sys_time(sys_time_t *info);
int ipc_set_sys_admin(sys_admin_t *info);
int ipc_set_sys_networking(ipc_network_t *info);
int ipc_set_sys_security(sys_security_t *info, int mask);
int ipc_system_request(int handle);
int ipc_set_vlan(switch_vlan_group_t *vg);
int ipc_del_vlan(switch_vlan_group_t *vg);
int ipc_set_vlan_mode(switch_vlan_mode_t *mode);
int ipc_get_vlan_mode(switch_vlan_mode_t *mode);
int ipc_get_interface(switch_interface_t *info, int *ifcount);
int ipc_get_clt_cable_param(ipc_clt_cable_param_t *info);
int ipc_set_clt_cable_param(ipc_clt_cable_param_t *info);


int ipc_get_interface_status(switch_interface_status_t *info, int *count);
int ipc_set_interface(switch_interface_t *info,  int count);
int ipc_get_interface_statistics(char *ifname, switch_interface_mib_t *info);
int ipc_reset_interface_statistics(char *ifname);
int ipc_set_vlan_interface(switch_vlan_interface_t *vlanif, int count);

int ipc_get_dev_info(ipc_cnu_devinfo_t *info, int *dev_count);
int ipc_get_cnu_info(int clt, int cnu,  ipc_cnu_info_t *info, int *cnu_status);
int ipc_get_cnu_link_stats(ether_addr_t *mac, cable_link_stats_t *info, int *cnu_status);
int ipc_get_cnu_port_stats(ether_addr_t *mac, ipc_cnu_port_mib_t *info, int *port_num, int *cnu_status);

int ipc_set_template(ipc_service_template_t *tmpl, int count);
int ipc_del_template(ipc_service_template_t *tmpl, int count);
int ipc_new_template(int count);


int ipc_new_userlist(cnu_user_config_t *info, int count);
int ipc_set_userlist(cnu_user_config_t *user, int count);
int ipc_del_userlist(cnu_user_config_t *user, int count);
//int ipc_new_userlist(int count);

int valid_xname(char *n);
int valid_description(char *n);


int vlan_string_check(const char *vstr);
int all_vlan_exist(switch_vlan_group_t *vg, int *ipc_assert);
int vlan_exist(int vid, int *ipc_assert);

int vlan_update(void);
int vlan_interface_update(void);

extern switch_vlan_mode_t sys_vlan_mode;
extern switch_vlan_t dot1q_vlans[EOC_MAX_VLAN_NUMS];
extern switch_vlan_t dot1q_vlans_u[EOC_MAX_VLAN_NUMS];

extern int dot1q_vlans_num; 
extern switch_vlan_interface_t *dot1q_vlan_interfaces[MAX_CLT_ETH_PORTS - 1];

#define vlan_in_transparent_mode()  (sys_vlan_mode.mode == VLAN_MODE_TRANSPARENT) 
int transparent_vlanif_search(char *ifname);
void transparent_vlanif_set(char *ifname, int type);
switch_vlan_interface_t *dot1q_vlanif_search(char *ifname);
switch_vlan_interface_t *dot1q_vlanif_dup(char *ifname);
int dot1q_vlan_untagged_portlist_update(void);
int dot1q_vlan_commit(void);
switch_vlan_group_t *vlan_group_get(int untag, vlan_member_t member);
int dot1q_vlan_interface_commit(void);
int vlan_update(void);
int vlan_interface_update(void);



char *snmp_strdup(unsigned char *val, int val_len);
char *interface_fancy_name(char *ifname);



/*Supported devices*/
extern ipc_cnu_devinfo_t supported_devices[];
extern int supported_devices_num;

int supported_devices_update(void);
char *supported_devices_alias(int id_b0);
int supported_devices_id_b0(char *alias);
#define valid_device_alias(s) (supported_devices_id_b0(s) < supported_devices_num)

/*Templates*/
extern ipc_service_template_t device_templates[];
extern int device_templates_num;
int device_templates_update(void);
int valid_template_name(char *v);
#define default_template(id_b1) (id_b1 <= supported_devices_num)
void device_template_default(ipc_service_template_t *tmpl);


/*Userlists*/
extern cnu_user_config_t device_userlists[];
extern int device_userlists_num;
int device_userlists_update(void);
cnu_user_config_t *device_userlists_search(ether_addr_t *mac);
void userlist_new_init(cnu_user_config_t *user, ether_addr_t *mac, char *model);
void device_userlist_default(cnu_user_config_t *user);


int valid_userlist(cnu_user_config_t *user);


/*Topology */

#define MAX_TOPOLOGY_NUM (MAX_CLT_CHANNEL * MAX_CNU_PER_CLT)

struct device_stats{
	//int clt;
	ether_addr_t mac_address;
	int valid_link_stats;
	int valid_port_mibs;
	cable_link_stats_t link_stats;
	int port_num;
	ipc_cnu_port_mib_t ports[CNU_MAX_ETH_PORTS];
	struct device_stats *next;
};

extern ipc_cnu_info_t device_topology[];
extern int device_topology_num;
struct device_stats *device_stats_head;

int device_topology_update(int update);
struct device_stats *device_stats_search(ether_addr_t *mac);
void device_stats_init(struct device_stats *stats, ipc_cnu_info_t *dev);
int device_link_stats_update(ether_addr_t *mac);
int device_port_mibs_update(ether_addr_t *mac);


void init_hexicom(void);


#endif /*#ifndef __HEXICOM_H__*/

