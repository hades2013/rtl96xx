
#ifndef __EOC_UTILS_H__
#define __EOC_UTILS_H__

#include <ipc_client.h>
//#include <switch.h>
//#include <vlan_utils.h>
#include <str_utils.h>
#include <debug.h>
#include <valids.h>
#include <shutils.h>
#include "memshare.h"
#include <ipc_protocol.h>


extern int ipc_fd;
extern VTY_USER_INFO_S vty_user[MAX_VTY_COUNT];
#ifdef CONFIG_CATVCOM
extern CATV_WORK_INFO_S *gst_CATVInfo;
#endif


#define STR(x)  "TODO: Make String["#x"]\n"

#define VTY_IPC_ASSERT(ret) do {\
								if (ret != 0){\
									vty_out(vty, "%% IPC ERROR %s%s", ipc_strerror(ret), VTY_NEWLINE);\
									return CMD_SUCCESS;\
								}\
							}while(0) 

#define VTY_NOMEM_ASSERT(mem) do {\
								if (mem == NULL){\
									vty_out(vty, "%% Memory Error%s", VTY_NEWLINE);\
									return CMD_SUCCESS;\
								}\
							}while(0) 

#define VTY_CNU_ASSERT(ret) do {\
								if (ret != 0){\
									vty_out(vty, "%% ERROR %s%s", ipc_strerror(ret), VTY_NEWLINE);\
									return CMD_SUCCESS;\
								}\
							}while(0) 



#define ENABLE_TO_STR(en) (en ? "Enabled" : "Disabled")
#define ONOFF_STR(en) (en ? "On" : "Off")
#define YESNO_STR(en) (en ? "Yes" : "No")
#define LINK_STR(en) (en ? "Up" : "Down")
#define DUPLEX_STR(en) ((en == ETH_DUPLEX_FULL) ? "Full" : "Half")

char *BYTES_STR(uint64_t n);
char *RATE_STR(unsigned long n);
char *PRIORITY_STR(int n);
char *SPEED_STR(int n);
char *FLOAT_STR(float  v, char *tail);
char *FLOATT_STR(float  v, char *tail);



char *CNU_LINK_STR(int link);
//char *CNU_STATE_STR(int state);
char *CNU_AUTH_STR(int auth);
char *CNU_DEVID_STR(int id);
char *CNU_TMPLID_STR(int id);



char *runtime_str(time_t tm);
//int ipc_get_sys_info(sys_info_t *info);
int ipc_get_sys_security(sys_security_t *info);
int ipc_get_sys_networking(sys_network_t *info);
int ipc_set_sys_networking(ipc_network_t *info);

int ipc_get_host_info(host_init_t *info );
int ipc_set_host_info(host_init_t *info, int element);

int ipc_set_sys_epon(sys_epon_t *info);
int ipc_get_sys_epon(sys_epon_t *info);

int ipc_set_sys_security(sys_security_t *info, int mask);
int ipc_system_request(int handle);
int ipc_get_sys_loop(sys_loop_t *info);
int ipc_set_sys_mvlan(sys_mvlan_t *info);
int get_sys_mvlan_value(sys_mvlan_t *sys);
int get_sys_networking_value(sys_network_t *sys,int cfgdomain);
int get_info_val(sys_info_t *info);
#if 0 //add by zhouguanhua
int ipc_set_vlan(int access, switch_vlan_group_t *vg);
int ipc_set_vlan_mode(switch_vlan_mode_t *mode);
int ipc_get_vlan_mode(switch_vlan_mode_t *mode);
int ipc_get_interface(switch_interface_t *info);
int ipc_get_interface_status(char *ifname, switch_interface_status_t *info);	
int ipc_set_interface(switch_interface_t *info);
int ipc_get_interface_statistics(char *ifname, switch_interface_mib_t *info);
int ipc_reset_interface_statistics(char *ifname);
int ipc_set_interface_switchport(switch_vlan_interface_t *vlanif);
#endif

int ipc_get_cnu_info(int clt, int cnu,  ipc_cnu_info_t *info, int *cnu_status);
int ipc_get_cnu_link_stats(int clt, int cnu,  cable_link_stats_t *info, int *cnu_status);
int ipc_get_cnu_port_stats(int clt, int cnu, ipc_cnu_port_mib_t *info, int *port_num, int *cnu_status);


int vlan_string_check(const char *vstr);
//int all_vlan_exist(switch_vlan_group_t *vg, int *ipc_assert);
int vlan_exist(int vid, int *ipc_assert);
//void vty_out_vlans(struct vty* vty, const char *prefix, switch_vlan_group_t *vg);
int ipc_get_vty_user_info_all(ipc_all_vty_user_info_t *vty_info);
char *ipc_get_telnet_service(char *service);
/*begin added by liaohongjun 2012/9/5 of QID0015*/
//int ipc_vtysh_nvram_set(char *pName, char *pValue);
int ipc_cfg_set_user(char *pName, char *pValue);
int ipc_cfg_set_telnetService(char *pName, char *pValue);
int get_syslog_value(sys_syslog_t *sys,int cfgdomain );
int ipc_set_syslog(sys_syslog_t *info);
//int ipc_vtysh_nvram_unset(char *pName);
//int ipc_vtysh_nvram_get(char *pName, CFG_VALUE_S *pstValue);
/*begin added by liaohongjun 2012/9/5 of QID0015*/
extern int update_prompt_from_web();
int ipc_get_sys_snmp_value(sys_snmp_t *sys,int cfgdomain);//add by linguobin 2013-10-22

#endif /*#ifndef __EOC_UTILS_H__*/


