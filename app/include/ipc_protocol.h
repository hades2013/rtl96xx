/*
 * ipc_protocol.h
 *
 *  Created on: Jan 5, 2011
 *      Author: root
 */

#ifndef IPC_PROTOCOL_H_
#define IPC_PROTOCOL_H_

#include <basetype.h>
#include <cable.h>
//#include <switch.h>
//#include <ipc_cnu_service.h>
#include <ufile_process.h>
#include <user.h>
#include <lw_type.h>
#include <lw_if_pub.h>
#include <lw_drv_pub.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <debug.h>


#ifndef __packed 
#define __packed __attribute__((__packed__))
#endif 

#define	IPC_SERVER_PATH		"/tmp/ipc_server"

#define	IPC_MAX_DATA_SIZE	1024
#define MAX_CONFIG_VAL_SIZE 256



#define SYS_MVLAN  0x01
#define SYS_HTTPS  0x02
#define SYS_SSH  	0x04
#define SYS_TELNET  0x08

#define READ_LOCK(fd, offset, whence, len) \
	lock_reg(fd, F_SETLK, F_RDLCK, offset, whence, len)
#define WRITE_LOCK(fd, offset, whence, len) \
	lock_reg(fd, F_SETLK, F_WRLCK, offset, whence, len)
#define UNLOCK(fd, offset, whence, len)\
	lock_reg(fd, F_SETLK, F_UNLCK, offset, whence, len)
extern int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len);
extern int countnum;

#define SYS_DEFAULT_IPADDR "192.168.0.1"
#define SYS_DEFAULT_NETMASK "255.255.255.0"
#define SYS_NETIF_NAME "eth0"
#define SYS_VIF_NAME  "eth0.1"
#define SYS_VLANIF_NUM  "1"
#define SYS_RESOLV_CONF  "/etc/resolv.conf"
#define SYS_RESOLV_CONF_TMP "/etc/resolv.conf.tmp"
#define SYS_UDHCPC_PID_FILE "/tmp/udhcpc.pid"
//#define SYS_NTPCLIENT_PID_FILE    "/tmp/ntpclient.pid"
#define SYS_NTPCLIENT_OK_FILE    "/tmp/ntpclient.txt"

#define	SYSLOG_FILE_PATH		"/var/log/messages"
//#define SYS_CONFIG_TMP_FILE   "/tmp/config.bin"
#define SYS_CONFIG_TMP_FILE   "/tmp/EPN104.config"

#define DEFAULT_CFG_FILE    "/etc/config/default.txt"

#define INTERFACE_STATE_UP "up"
#define INTERFACE_STATE_DOWN "down"

#define PROCESS_DISABLE 0
#define PROCESS_ENABLE  1
#define LOOP_STARTUP_CMD "/usr/sbin/loopd &"
#define LOOPD_NAME "loopd" 

#define MAX_COMMUNITY_SIZE  64
#define MAX_SNMPINFO_SIZE  80
#define MAX_VERSION_SIZE   32
#define MAX_BUILDTIME_SIZE 64
#define MAX_SYSINFO_SIZE   32
#define MAX_ADMIN_NAME_SIZE  16
#define MAX_ADMIN_PWD_SIZE  16
#define MAX_HOSTNAME_SIZE  32
#define MAX_DOMAIN_SIZE  32
#define MAX_URL_SIZE       128
#define MAX_IP_STR_SIZE   18 
#define MAX_DNS_NUM   	3 
#define MAX_ADMIN_NUM  	2
#define MAX_INTERFACE_STATE_SIZE 16
#define UNI_MAX_NUM 4

#define MAX_HOSTNAME_SIZE_T  16
#define MAX_SUPER_SIZE       16
#define SNMP_V1_VAL    1
#define SNMP_V2c_VAL   4
#define SNMP_V3_VAL    9

#define SNMP_AGENT_FLAG 		0x0001  
#define SNMP_ADD_COMM_FLAG 		0x0002
#define SNMP_DEL_COMM_FLAG 		0x0004
#define SNMP_TRAP_FLAG 			0x0008	
#define SNMP_ADD_GROUP_FLAG 	0x0010
#define SNMP_DEL_GROUP_FLAG 	0x0020	
#define SNMP_ADD_USER_FLAG 		0x0040
#define SNMP_DEL_USER_FLAG 		0x0080
#define SNMP_CATVINFO_FLAG 		0x0100
#define DONT_TRIGER_SIGNAL		0x0200
#define ENGINE_MAX_SIZE 		128
#define MAX_SNMP_COMM_NAME_SIZE 33 
#define MAX_SNMP_GROUP_NAME_SIZE 33 
#define SNMP_ACESS_MODE_RO 		0
#define SNMP_ACESS_MODE_RW 		1
#define SNMP_ACESS_MODE_NULL	2

#define MAX_SNMP_USER_NAME_SIZE 32
#define MAX_SNMP_USER_GROUP_SIZE 33
#define MAX_SNMP_USER_PASSWORD_SIZE 64
#define MAX_SNMP_USER_ENPASSWORD_SIZE 64
#define MAX_SNMP_USER_COUNT 4
#define SNMP_GROUP_SECU_NOAUTHNOENCRYPT		0
#define SNMP_GROUP_SECU_AUTHNOENCRYPT		1
#define SNMP_GROUP_SECU_AUTHENCRYPT			2
#define SNMP_GROUP_SECU_NULL			    3

#define SNMP_GROUP_MODE_RO 		0
#define SNMP_GROUP_MODE_RW 		1
#define SNMP_USER_SECU_NOAUTHNOENCRYPT		0
#define SNMP_USER_SECU_AUTHNOENCRYPT		1
#define SNMP_USER_SECU_AUTHENCRYPT			2
#define SNMP_USER_SECU_NULL  			    3

#define MAX_SNMP_COMM_COUNT 	4
#define MAX_SNMP_GROUP_COUNT 	4
#define NO_DEL_NUM 				-1
#define NO_ADD_NUM				-1	
#define SNMP_DISABLE 0 
#define SNMP_ENABLE  1
#define SNMP_DEBUG 0
#define SNMP_ENABLE_LEN 1
#define COMMONDEVICESID_MAX		40
#define ANALOGALARM_TABLE    3

#if (SNMP_DEBUG)
#define SNMP_STARTUP_CMD "/usr/sbin/snmpd -Lo -C -c /etc/config/snmpd.conf \
							-Dsnmpv3 \
							-Dusm \
							-Dnetsnmp_udp \
							-Dread_config:parser \
							-DcltConfigRemoteUpgradeTable \
							-DmiscIdent \
							-DdorDCPowerTable"  
#else
#define SNMP_STARTUP_CMD "/usr/sbin/snmpd -Lo -C -c /etc/config/snmpd.conf"  
#endif

#define SNMPD_NAME "snmpd" 
#define SNMPDCONFI_PATH  	"/etc/config/snmpd.conf"
#define DONT_REREAD_CONF 0
#define NEED_REREAD_CONF 1
#define MAX_LINE_STRING_SIZE 1024
#define CATV_STARTUP_CMD "/usr/sbin/catvcom &"

/*for ssh*/
#define DROPBEAR_NAME "dropbear"  
#define DROPBEAR_FULL_PATH "/usr/local/sbin/dropbear"  
#define SSH_DFAULT_PORT 22  


typedef struct {
	char *(*toString)(void);
	uint32_t (*toInt)(void);
	char *(*toDate)(void);
}
version_t;


typedef enum {
	IP_PROTO_STATIC = 0,
	IP_PROTO_DYNAMIC
}
ip_proto_t;

typedef struct{
	struct in_addr ip;
	struct in_addr subnet;
	struct in_addr broadcast;
	struct in_addr gateway;
	struct in_addr dns[MAX_DNS_NUM];	
} 
netif_param_t;

typedef struct {
	uint16_t mvlan;
	uint8_t https_en;
	uint8_t ssh_en;
	uint16_t ssh_port;
	uint8_t telnet_en;
	uint16_t telnet_port;
}
sys_security_t;
typedef struct {
	uint8_t epon_loid[24 + 1];
	uint8_t epon_passwd[12 + 1];
}
sys_epon_t;

typedef struct {
	char snmp_comm_name[MAX_SNMP_COMM_NAME_SIZE];
	uint8_t snmp_access_mode;
}snmp_community_t;

typedef struct {
	char snmp_group_name[MAX_SNMP_GROUP_NAME_SIZE];
	uint8_t snmp_group_security;
	uint8_t snmp_group_mode;
}snmp_group_t;
typedef struct {
	char snmp_user_name[MAX_SNMP_USER_NAME_SIZE];
	uint8_t snmp_user_security;
	char snmp_user_group[MAX_SNMP_USER_GROUP_SIZE];
	char snmp_user_password[MAX_SNMP_USER_PASSWORD_SIZE];
	char snmp_user_enpassword[MAX_SNMP_USER_ENPASSWORD_SIZE];
}snmp_user_t;
typedef struct {
	uint16_t snmp_set_flag;
	/*agnet:  SNMP_AGENT_SET_FLAG :0x0001*/
	uint8_t snmp_en;
	char 		snmp_engineID[ENGINE_MAX_SIZE];
	uint16_t 	snmp_max_packet_len;
	char 		snmp_location[MAX_SNMPINFO_SIZE+1]; 
	char 		snmp_contact[MAX_SNMPINFO_SIZE+1];
	uint8_t 	snmp_version; 
	char 		snmp_trust_host[MAX_IP_STR_SIZE];
	/*add comm*/
	uint8_t 	snmp_comm_current_count;
	snmp_community_t snmp_comm[MAX_SNMP_COMM_COUNT];


	/*add num*/
	int8_t 		snmp_add_which_num; 
										   
	/*del num*/
	int8_t		snmp_del_which_num;

    /*trap */
    uint8_t 	snmp_trap_en;
    struct in_addr snmp_trap_ip;
	uint16_t 	snmp_trap_com;
    char        snmp_trap_community[MAX_COMMUNITY_SIZE]; 
	uint8_t 	snmp_trap_version;
	uint8_t 	snmp_trap_safe;
	uint8_t     snmp_last_save_safe_num;

    /*add user */
    uint8_t 	snmp_user_current_count;
    snmp_user_t snmp_user[MAX_SNMP_USER_COUNT];

	/*add group*/
	uint8_t 	 snmp_group_current_count;
	snmp_group_t snmp_group[MAX_SNMP_GROUP_COUNT];
}
sys_snmp_t;



typedef struct{ 
	char hostname[MAX_HOSTNAME_SIZE];
	char ifname[IFNAMESIZE];	
	ethernet_addr_t mac;	
	uint8_t ip_proto;
	netif_param_t netif;
	netif_param_t netif_static;
	uint32_t lease_time; // for dhcp	
	uint8_t dns_static; 
	char domain[MAX_DOMAIN_SIZE];
	uint32_t mtu;
    char interface_state[MAX_INTERFACE_STATE_SIZE];
}
sys_network_t;
typedef struct{ 
	uint16_t m_vlan;
    uint16_t vlan_interface;
}
sys_mvlan_t;
typedef struct{ 
    uint8_t loopback_enable;  
    uint8_t loop_update;
    uint8_t loopback_unis_status[UNI_MAX_NUM];
}
sys_loop_t;

typedef struct {
    uint8_t syslog_en;
    char syslog_ip[MAX_IP_STR_SIZE]; 
    uint8_t syslog_degree;
    uint8_t syslog_save_degree;
	uint16_t syslog_port;					 
	uint8_t syslog_console_en;  			
	uint8_t syslog_console_save_degree;		
}sys_syslog_t;

typedef struct{
    char ht_name[MAX_HOSTNAME_SIZE_T+1];
	char ht_super[MAX_SUPER_SIZE+1];
}host_init_t;


typedef enum{
	ACTION_NONE = 0,
	ACTION_RELEASE = 1,
	ACTION_RENEW 
}
dhcpc_action_t;

typedef struct{ 
	char hostname[MAX_HOSTNAME_SIZE];
	uint8_t ip_proto;
//	union{
		netif_param_t netif;
		uint8_t action;
//	};
    char interface_state[MAX_INTERFACE_STATE_SIZE];
}
ipc_network_t;


typedef struct {
	uint32_t uptime;
	uint16_t mvlan;
	char sw_version[MAX_VERSION_SIZE];
	char hw_version[MAX_VERSION_SIZE];	
	char buildtime[MAX_VERSION_SIZE];		
	char sw_inter_version[MAX_VERSION_SIZE];
	char bootload_version[MAX_VERSION_SIZE];
	char kernel_version[MAX_BUILDTIME_SIZE];
	char kernel_buildtime[MAX_BUILDTIME_SIZE];
	ethernet_addr_t sysmac;	
//	ethernet_addr_t cabmac[MAX_CLT_CHANNEL];
}
sys_info_t;


typedef struct{
	time_t time;
	uint8_t zone;
	uint8_t ntp_en;
	char server[MAX_URL_SIZE];
	uint8_t ntp_ok;
	time_t time_before_change; 
}
sys_time_t;


typedef struct{
	uint8_t enable;		
	char old_name[MAX_ADMIN_NAME_SIZE];// for admin setting use
	char name[MAX_ADMIN_NAME_SIZE];
	char pwd[MAX_ADMIN_PWD_SIZE];	
}admin_t;

typedef struct{
	uint16_t idle_time;
	int count;
	admin_t users[MAX_ADMIN_NUM];
}
sys_admin_t;

typedef struct {
    uint32_t uiQosTrustmode;	
	QueueMode_S Qosqueuemode;
}
sys_qos_t;

typedef struct {
    PORT_CFG_VALUE_S port_cfg_val[LOGIC_PORT_NO];
}
sys_port_t;

typedef struct {
    STROM_CTL_S stormclt[LOGIC_PORT_NO];
}
sys_stormctl_t;


typedef struct {
    PORT_LINERATE_VALUES port_linerate_val[LOGIC_PORT_NO];
}
sys_linerate_t;


typedef struct {
    MIRROR_GROUP_S port_mirror_val;
}
sys_mirror_t;

			

/********************************************************************************/
/*						 catv info																														 */
/********************************************************************************/
						
#define MAX_CATVINFO_SIZE   10

typedef enum tagCATV_Module_Mask
{
    DEV_STATUS 			= 0x01,
    CATV_MM_END
} CATV_MODULE_MASK_E;

typedef enum tagCATV_Para_Mask
{
	/*dev status*/
    PM_UPDATE 						= 0x01,
	PM_OUTPUT_LEVEL_ATTEN			= 0x01<<1,

} CATV_PARA_MASK_E;

typedef enum tagCATV_Default_Val
{
	/*dev status*/
    DEF_UPDATE 						= 15,
    DEF_OUTPUT_LEVEL_ATTEN 			= 0,  

} CATV_DEFAULT_VAL_E;

typedef struct{
		UINT16 dev_status_update;
		char soft_version[MAX_CATVINFO_SIZE];
		char input_laser_power[MAX_CATVINFO_SIZE];
		char temper[MAX_CATVINFO_SIZE];
		#if defined(CONFIG_ZBL_SNMP)
		UINT32 input_laser_power_mw;
		UINT32 output_level;		  
		UINT32 power_12v;	
		#else
		char output_level[MAX_CATVINFO_SIZE];		  
		char power_12v[MAX_CATVINFO_SIZE];	
		#endif		
		UINT32 output_level_atten;
		UINT8 	module_mask;
		UINT32 	para_mask;		
}catvinfo_t;


/********************************************************************************/
/*                       wlan_ap                                                                                                                         */
/********************************************************************************/
#ifdef CONFIG_WLAN_AP
extern char *enable_str[];
extern char *model_str[];
extern char *bandwidth_str[];
extern char *channel_str[];
extern char *tx_rate_str[];
extern char *filter_model_str[];
extern char *enc_wep_key_len_str[];
extern char *enc_wep_key_type_str[];
extern char *enc_wpa_enc_pro_str[];
extern char *enc_str[];
extern char *authtype_str[];
extern int wlan_fd;
#endif

#define MAX_SSID 					4
#define MAX_FILTER 					20

#define ONLY_ALLOW					1
#define ONLY_FORBIDEN				0

#define _64BIT_10HEX_5ASCII			1
#define _128BIT_26HEX_13ASCII		2

#define ENC_WEP_KEY_TYPE_HEX		1
#define ENC_WEP_KEY_TYPE_ASCII		2

#define ENC_WPA_ENCODE_PRO_TKIP		1
#define ENC_WPA_ENCODE_PRO_AES		2
#define ENC_WPA_ENCODE_PRO_TKIP_AES	3

#define WEP_KEY_MAX_SIZE   			30
#define WPA_KEY_MAX_SIZE			64
#define MASK_NO_USE		 			0

#define WALAN_UP					1
#define WALAN_DOWN					0

#define WALAN_BLOCK					2
#define MAC_SIZE					18

/*must keep same with  IW_MAX_AP in master/wareless.h*/
#define MAX_AP_CLIENT				64 

#define DEF_SSID_NAME				"SSID-name"
#define DEF_SSID_ENC_WEP_KEY		"admin"
#define DEF_SSID_ENC_WPA_KEY		"admin"
#define DEF_FILTER_MAC				"00.00.00.00.00.00"
#define DEF_FILTER_MAC_DEC			"default mac"
#define BLANK_STR					""
#define ZERO_STR					"0"
#define IWPRIV_WLAN0_SET_MIB		"iwpriv wlan0 set_mib"
#define IWPRIV_WLAN0_VAPN_SET_MIB	"iwpriv wlan0-vap%d set_mib"

#define UP_STR						"up"
#define DOWN_STR					"down"

#define SSID_MASK					0x80000000

#define SYS_ERR_CHK_RET(op)                           		\
do {														\		
		UINT32 err_code;									\
		if((err_code = (op)) != NO_ERROR)				   	\
		{													\
			DBG_PRINTF("Operation failed:%d", err_code);	\
			return err_code;								\
		}													\
} while(0)


#define SYS_ERR_CHK_NORET(op)                           	\
do {														\		
		UINT32 err_code;									\
		if((err_code = (op)) != NO_ERROR)				   	\
		{													\
			DBG_PRINTF("Operation failed:%d", err_code);	\
		}													\
} while(0)



typedef enum tagWLAN_Module_Mask
{
    WLAN_MM_BASIC 			= 0x01,
    WLAN_MM_SECURITY 		= 0x02,
    WLAN_MM_ACCESS_CTR 		= 0x04,
    WLAN_MM_ADVANCE 		= 0x08,
    WLAN_MM_CLIENT 			= 0x10,
    WLAN_MM_END
} WLAN_MODULE_MASK_E;



typedef enum tagWLAN_Model
{
    WLAN_MODEL_B_ONLY = 1,
    WLAN_MODEL_G_ONLY,
    WLAN_MODEL_B_G,
    WLAN_MODEL_N_ONLY,
    WLAN_MODEL_B_G_N,
    WLAN_MODEL_END
} WLAN_MODEL_E;



typedef enum tagWLAN_Bandwidth
{
    WLAN_BANDWIDTH_20MHZ = 1,
	WLAN_BANDWIDTH_40MHZ,
    WLAN_BANDWIDTH_END
} WLAN_BANDWIDTH_E;


typedef enum tagWLAN_Channel
{
    WLAN_CHANNEL_0_AUTO = 0,
	WLAN_CHANNEL_1_2_412GHZ,
	WLAN_CHANNEL_2_2_417GHZ,
	WLAN_CHANNEL_3_2_422GHZ,
	WLAN_CHANNEL_4_2_427GHZ,
	WLAN_CHANNEL_5_2_432GHZ,
	WLAN_CHANNEL_6_2_437GHZ,
	WLAN_CHANNEL_7_2_442GHZ,
	WLAN_CHANNEL_8_2_447GHZ,
	WLAN_CHANNEL_9_2_452GHZ,
	WLAN_CHANNEL_10_2_457GHZ,
	WLAN_CHANNEL_11_2_462GHZ,
	WLAN_CHANNEL_12_2_467GHZ,
	WLAN_CHANNEL_13_2_472GHZ,
    WLAN_CHANNEL_END
} WLAN_CHANNEL_E;



typedef enum tagWLAN_TxRate
{
    WLAN_TXRAT_0_AUTO = 0,
    WLAN_TXRAT_1_1MBPS,
    WLAN_TXRAT_2_2MBPS,
    WLAN_TXRAT_3_5_5MBPS,
    WLAN_TXRAT_4_6MBPS,
    WLAN_TXRAT_5_6_5MBPS,
    WLAN_TXRAT_6_9MBPS,
    WLAN_TXRAT_7_11MBPS,
    WLAN_TXRAT_8_12MBPS,
    WLAN_TXRAT_9_13MBPS,
    WLAN_TXRAT_10_13_5MBPS,
    WLAN_TXRAT_11_18MBPS,
    WLAN_TXRAT_12_19_5MBPS,
    WLAN_TXRAT_13_24MBPS,
    WLAN_TXRAT_14_26MBPS,
    WLAN_TXRAT_15_27MBPS,
    WLAN_TXRAT_16_36MBPS,
    WLAN_TXRAT_17_39MBPS,
    WLAN_TXRAT_18_40_5MBPS,
    WLAN_TXRAT_19_48MBPS,
    WLAN_TXRAT_20_52MBPS,
    WLAN_TXRAT_21_54MBPS,
    WLAN_TXRAT_22_58_5MBPS,
    WLAN_TXRAT_23_60MBPS,
    WLAN_TXRAT_24_65MBPS,
    WLAN_TXRAT_25_81MBPS,
    WLAN_TXRAT_26_108MBPS,
    WLAN_TXRAT_27_121_5MBPS,
    WLAN_TXRAT_28_135MBPS,
    WLAN_TXRAT_END
} WLAN_TXRATE_E;


typedef enum tagWLAN_TxPower
{
    TXPOWER_100_PER = 100,
    TXPOWER_70_PER = 70,
    TXPOWER_50_PER = 50,
    TXPOWER_35_PER = 35,
    TXPOWER_10_PER = 10,
} WLAN_TxPower_E;


typedef struct __packed {
	UINT8 				enable;
	WLAN_MODEL_E 		model;
	WLAN_BANDWIDTH_E 	bandwidth; 
	WLAN_CHANNEL_E 		channel; 
	WLAN_TXRATE_E 		tx_rate; 
	UINT16				up_ser_vlan;
}wlan_basic_t;



typedef enum tagEncrypt_Type
{
	ENC_NONE 				= 0,
	ENC_WEP,
	ENC_WPA_PSK,
	ENC_WPA2_PSK,
	ENC_WPA_PSK_WPA2_PSK,
}ENCRYPT_TYPE_E;

typedef enum tagAUTH_Type
{
	OPEN_SYSTEM 			= 0,
	SHARED_KEY,
	AUTH_AUTO
}AUTH_TYPE_E;

typedef enum tagWLAN_Default_Val
{
	/*basic*/
    DEF_ENABLE 						= ENABLE,
    DEF_MODEL 						= WLAN_MODEL_B_G_N,  
    DEF_BANDWIDTH 					= WLAN_BANDWIDTH_40MHZ,
    DEF_CHANNEL 					= WLAN_CHANNEL_0_AUTO,
    DEF_TX_RATE 					= WLAN_TXRAT_0_AUTO,

	/*security*/
	DEF_SSID_EN						= ENABLE,
	DEF_SSID_ISOLATE_EN				= DISABLE,
	DEF_SSID_BCAST_EN				= ENABLE,
	DEF_AUTHTYPE					= AUTH_AUTO,
	DEF_SSID_ENC_TYPE				= ENC_NONE,
	DEF_SSID_ENC_WEP_GROUP			= 1,
	DEF_SSID_ENC_WEP_KEY_LEN		= _64BIT_10HEX_5ASCII,
	DEF_SSID_ENC_WEP_KEY_TYPE    	= ENC_WEP_KEY_TYPE_ASCII,
	DEF_SSID_ENC_WPA_ENCODE_PRO		= ENC_WPA_ENCODE_PRO_TKIP,
	DEF_SSID_ENC_WPA_KEY_PERIOD  	= 3600,
	
	/*access ctr*/
    DEF_FILTER_EN 					= DISABLE,
	DEF_FILTER_MODEL				= ONLY_FORBIDEN,
	DEF_FILTER_CURR_COUNT			= 0,

	/*advance*/
	DEF_BEACON_INTERVAL 			= 100,
	DEF_RST_THRESHOLD				= 2347,
	DEF_FRAG						= 2346,
	DEF_DTIM_INTERVAL				= 1,
	DEF_TXPOWER						= 15,
	DEF_WMM_EN						= ENABLE
	/*client*/
} WLAN_DEFAULT_VAL_E;


/*para_mask[0]*/
typedef enum tagWLAN_Para_Mask
{
	/*basic*/
    PM_ENABLE 						= 0x00000001,
    PM_MODEL 						= 0x00000002,
    PM_BANDWIDTH 					= 0x00000004,
    PM_CHANNEL 						= 0x00000008,
    PM_TX_RATE 						= 0x00000010,

	/*security*/
    PM_SSID_INDEX 					= 0x00000020,
	PM_SSID_NAME					= 0x00000040,
	PM_SSID_ENABLE					= 0x00000080,
	PM_ISOLATE_EN					= 0x00000100,
	PM_BCAST_EN						= 0x00000200,
	PM_ENC_TYPE						= 0x00000400,
	PM_AUTHTYPE 					= 0x80000000,	
	PM_ENC_WEP_GROUP				= 0x00000800,
	PM_ENC_WEP_KEY_LEN				= 0x00001000,
	PM_ENC_WEP_KEY_TYPE				= 0x00002000,
	PM_ENC_WEP_KEY					= 0x00004000,
	PM_ENC_WPA_KEY					= 0x00008000,
	PM_ENC_WPA_KEY_TYPE				= 0x00010000,
	PM_ENC_WPA_KEY_PERIOD			= 0x00020000,

	/*access ctr*/
	PM_FILTER_EN					= 0x00040000,
	PM_FILTER_MODEL					= 0x00080000,
	PM_FILTER_MAC					= 0x00100000,
	PM_FILTER_MAC_DEC				= 0x00200000,
	PM_DEL							= 0x00400000,
	PM_FILTER_UPLOAD				= 0x00800000,
	PM_FILTER_DOWNLOAD				= 0x01000000,

	/*advance*/
	PM_BEACON_INTERVAL				= 0x02000000,
	PM_RST_THRESHOLD				= 0x04000000,
	PM_FRAG							= 0x08000000,
	PM_DTIM_INTERVAL				= 0x10000000,
	PM_TX_POWER						= 0x20000000,
	PM_WMM_EN						= 0x40000000,
	/*client*/
} WLAN_PARA_MASK_E;


/*para_mask[1]*/
typedef enum tagWLAN_Para_Mask_Ext1
{
	/*basic*/
    PM_UP_SER_VLAN 					= 0x80000000
	
} WLAN_PARA_MASK_Ext1_E;  



typedef struct __packed {
	UINT8 authtype;
	UINT8 group;
	UINT8 key_len;
	UINT8 key_type;
	char key[WEP_KEY_MAX_SIZE];
}enc_wep_t;

typedef struct __packed {
	char key[WPA_KEY_MAX_SIZE];
	UINT8 key_type;
	UINT16 period;
}enc_wpa_t;



typedef struct __packed {
	char name[BUF_SIZE_32];
	UINT8 enable;
	UINT8 isolate_en;
	UINT8 bcast_en;
	ENCRYPT_TYPE_E enc_type;
	union {
		enc_wep_t wep;
		enc_wpa_t wpa;
	}enc;

}wlan_ssid_t;


typedef struct __packed {
	UINT8 curr_count;
	UINT8 curr_index;
	wlan_ssid_t ssid[MAX_SSID];
}wlan_security_t;


typedef struct __packed {
	char mac[MAC_SIZE];
	char dec[BUF_SIZE_32];
}wlan_filter_t;


typedef struct __packed {
	UINT8 filter_en;
	UINT8 filter_model;
	UINT16 curr_count;
	UINT16 curr_index;
	wlan_filter_t filter[MAX_FILTER];
	char filename[BUF_SIZE_32];
}wlan_access_ctr_t;


typedef struct __packed {
	UINT8 beacon_interval;
	UINT16 rts_threshold;
	UINT16 frag;
	UINT8 dtim_interval;
	UINT8 tx_power;
	UINT8 wmm_en;
}wlan_advance_t;

typedef struct __packed {
	UINT8 ssid; 
	char mac[MAC_SIZE];
	char tx[BUF_SIZE_16];
	char rx[BUF_SIZE_16];
}block_t;


typedef struct __packed {
	
	block_t block[MAX_AP_CLIENT];

}wlan_client_t;


typedef struct __packed {
	wlan_basic_t 		wlan_basic;
    wlan_security_t	 	wlan_security;
    wlan_access_ctr_t 	wlan_access_ctr;
    wlan_advance_t 		wlan_advance;
    wlan_client_t 		wlan_client;
	WLAN_MODULE_MASK_E 	module_mask;
	UINT32 				para_mask[WALAN_BLOCK];
	UINT32 				del_mask;
}wlan_ap_t;
#if defined(CONFIG_ZBL_SNMP)
typedef struct{ 
	char alarmEnable[SNMP_ENABLE_LEN];
	long 			analogAlarmState;
	long            analogAlarmHIHI;
	long            analogAlarmHI;
	long            analogAlarmLO;
	long            analogAlarmLOLO;
	long            analogAlarmDeadband;
}AnalogAlarmPropertyTable_t;

typedef struct{ 
	int index;
	AnalogAlarmPropertyTable_t AnalogAlarmPropertyTable[ANALOGALARM_TABLE];
}AnalogProperty_t;

typedef struct{ 
	char commonNELogicalID[COMMONDEVICESID_MAX];
	unsigned long commonCheckCode;	
	char commonTrapCommunityString[MAX_COMMUNITY_SIZE];
}
commonAdminGroup_t;
#endif

typedef struct sysUpdate{
	union{
		struct {
			uint32_t ip:1;
			uint32_t hostname:1;
			uint32_t snmp_en:1;
			uint32_t https:1;	
			uint32_t ssh:1;
			uint32_t telnet:1;
			uint32_t mvlan:1;
			uint32_t time:1;
            uint32_t interface_state:1;
            uint32_t loop_enable:1;
            uint32_t syslog:1;
            uint32_t qos:1;
            uint32_t port:1;
            uint32_t storm:1;
			uint32_t linerate:1;
			uint32_t mirror:1;
			uint32_t mac_setting:1;
            uint32_t compatible:1;
			uint32_t analogPropertyTable:1;
			uint32_t commonAdminGroup:1;
		};
		uint32_t data;
	};	
	// to do 
}sys_update_t;

typedef struct{
	sys_security_t security;
	sys_snmp_t snmp;
	sys_network_t network[IF_L3VLAN_MAX];
	sys_info_t info;
	sys_time_t rtc;
	sys_admin_t admin;
	sys_update_t update;
	sys_mvlan_t  mvlan;
	sys_loop_t   loop;
    sys_syslog_t syslog;
	mac_setting_t mac_setting;
    sys_qos_t  qos;
    sys_port_t  port;
    sys_stormctl_t storm;
	sys_linerate_t linerate;
	sys_mirror_t mirror;
	sys_epon_t  epon_info;
#ifdef CONFIG_CATVCOM	
	catvinfo_t  catvinfo;
#endif
#ifdef CONFIG_WLAN_AP	
	wlan_ap_t	wlan_ap;
#endif
#if defined(CONFIG_ONU_COMPATIBLE)
    compatible_setting_t compatible_setting;
#endif
#if defined(CONFIG_ZBL_SNMP)
	AnalogProperty_t AnalogProperty;
	commonAdminGroup_t commonAdminGroup;
#endif
}
sys_config_t;


/*
	this structure is used in dhcp client callback script, 
	the dhcpc handler will attach this data in ipc when dhcp event happened.
*/
typedef enum{
	DHCPC_DECONFIG = 0,
	DHCPC_BOUND,
	DHCPC_RENEW
}
sys_dhcpc_event_t;

typedef struct {
	uint8_t event;
	char ifname[IFNAMESIZE];	
	struct in_addr ip;
	struct in_addr subnet;
	struct in_addr broadcast;
	struct in_addr gateway;
	struct in_addr dns[MAX_DNS_NUM];
	uint32_t lease_time; 
	char domain[MAX_DOMAIN_SIZE];	
}
sys_dhcpc_t;


typedef enum{
	SYS_DO_REBOOT = 0,
	SYS_DO_RESTORE,
	SYS_DO_RESTORE_AND_REBOOT,
	SYS_DO_NVRAM_COMMIT
}
sys_action_t;



extern sys_config_t sys_config;

#define sys_security 	sys_config.security
#define sys_snmp 	  	sys_config.snmp
#define sys_network 	sys_config.network[0]
#define sys_info     	sys_config.info
#define sys_time     	sys_config.rtc
#define sys_admin     	sys_config.admin
#define sys_mvlan     	sys_config.mvlan
#define sys_loop     	sys_config.loop
#define sys_syslog 		sys_config.syslog 
#define sys_mac_setting  sys_config.mac_setting
#define sys_qos         sys_config.qos
#define sys_port        sys_config.port
#define sys_storm       sys_config.storm
#define sys_linerate    sys_config.linerate
#define sys_mirror      sys_config.mirror
#define sys_epon		sys_config.epon_info
#ifdef CONFIG_CATVCOM	
#define sys_catv		sys_config.catvinfo
#endif
#ifdef CONFIG_WLAN_AP	
#define sys_wlan_ap	sys_config.wlan_ap
#endif
#if defined(CONFIG_ONU_COMPATIBLE)
#define sys_compatible		sys_config.compatible_setting
#endif
#if defined(CONFIG_ZBL_SNMP)
#define sys_AnalogProperty		sys_config.AnalogProperty
#define sys_commonAdminGroup		sys_config.commonAdminGroup
#endif
#define set_update(x) (sys_config.update.x = 1)
#define clr_update(x) (sys_config.update.x = 0)
#define set_update_all() (sys_config.update.data = 0xffffffff)
#define clr_update_all() (sys_config.update.data = 0)
#define test_update(x) (sys_config.update.x > 0)

#define ip_set(x,v) ((x).s_addr = (v).s_addr)
#define ip_equal(x, v) ((x).s_addr == (v).s_addr)


typedef struct tagCfgValue
{
    char value[MAX_CONFIG_VAL_SIZE];
}CFG_VALUE_S;

typedef enum {
	IPC_PING = 0,
	IPC_CFG_ACCESS,
	IPC_CFG_BACKUP,
	IPC_GET_USER_INFO,
	IPC_SYSMON_CMD,
	IPC_NETWORK_INFO,
	IPC_GET_CNU_INFO,
	IPC_GET_CNU_MIB,
	IPC_GET_CNU_LINK_STATS,
	IPC_GET_MME_STAT,
	IPC_GET_VLAN_CONFIG,
	IPC_GET_IF_CONFIG,
	IPC_SERVICE_TEMPLATE,
	IPC_SUPPORTED_DEVICE,
	IPC_CLT_CABLE_PARAM,
	IPC_CLT_INTERFACE,
	IPC_CLT_INTERFACE_TRUNK_HYBRID,// no use
	IPC_CLT_INTERFACE_STATUS,
	IPC_CLT_INTERFACE_MIB,
	IPC_CLT_VLAN_INTERFACE,
	IPC_CLT_VLAN_MODE,	
	IPC_CLT_VLAN,
	IPC_CNU_USER,
	IPC_SYS_SECURITY,
	IPC_SYS_NETWORKING,
	IPC_SYS_SNMP,
	IPC_SYS_TIME,
	IPC_SYS_INFO,	
	IPC_SYS_ADMIN,
	IPC_SYS_DHCPC_EVENT,
	IPC_SYSTEM,
	IPC_MVLAN,
	IPC_LOOP,
	IPC_SYSLOG,
	IPC_SHOW_STARTUPSTART,
	IPC_SHOW_STARTUPEND,
	IPC_QOS_TRUSTMODE,
	IPC_QOS_QUEUESCH,
	IPC_PORT,
	IPC_STORM_CTL,
	IPC_LINERATE,
	IPC_MIRROR,
	IPC_SHUTDOWN_INTF,  
	IPC_MAC_SETTING,	
	IPC_EPON,
#ifdef CONFIG_CATVCOM	
	IPC_CATV,
#endif	
#ifdef CONFIG_WLAN_AP	
	IPC_WLAN_AP,
#endif
#if defined(CONFIG_ONU_COMPATIBLE)
    IPC_COMPATIBLE_SETTING,
#endif
#if defined(CONFIG_ZBL_SNMP)
	IPC_ANALOGPROPERTYTABLE,
	IPC_COMMONADMINGROUP,
#endif
	IPC_END
}
ipc_request_type_t;

typedef enum {
	IPC_STATUS_OK = 0,
	IPC_STATUS_FAIL,
	IPC_STATUS_ARGV,
	IPC_STATUS_BUSY,
	IPC_STATUS_APP_INVALID,
	IPC_STATUS_NOCLT,
	IPC_STATUS_NOCNU,
	IPC_STATUS_NOIF,
	IPC_STATUS_NOVLAN,
	IPC_STATUS_NOTMPL,
	IPC_STATUS_NOUSER,
	IPC_STATUS_CNUDOWN,
	IPC_STATUS_CNUNOSUPP,
	IPC_STATUS_CNUNOFUNC,
	IPC_STATUS_CNUNORESP,	
	IPC_STATUS_APPLY_FAIL,
	IPC_STATUS_COMMIT_FAIL,
	IPC_STATUS_EXIST,
	IPC_STATUS_EXCEED,
	IPC_STATUS_END
}
ipc_status_t;

typedef enum {
	NVRAM_ACCESS_GET = 0,
	NVRAM_ACCESS_SET,
	NVRAM_ACCESS_UNSET,
	NVRAM_ACCESS_COMMIT
}
ipc_nvram_access_t;
typedef enum {
	USER_VTY_INFO_GET = 0,
	USER_VTY_INFO_ALL_GET,
}ipc_get_user_info_t;

typedef enum {
	IPC_SYS_REBOOT = 0x01,
	IPC_SYS_RESTORE,
	IPC_SYS_RESTORE_AND_REBOOT,	
	IPC_SYS_NVRAM_COMMIT,
	IPC_SYS_CONFIG_BACKUP,	
	IPC_SYS_UPGRADE_READY,	
	IPC_SYS_UPGRADE_APP,
	IPC_SYS_CONFIG_READY,
	IPC_SYS_STATUS,	
	IPC_SYS_NVRAM_STATUS,
	IPC_SYS_UPGRADE_STATUS,
	IPC_SYS_LOG_CLEAR
}
ipc_system_access_t;

typedef enum {
	IPC_SYS_STATUS_NVRAM_CHANGE = 0x01,
	IPC_SYS_STATUS_UPGRADING = 0x02,
}
ipc_system_status_t;


typedef enum {
	IPC_MIBS_GET = 0x01,
	IPC_MIBS_RESET
}
ipc_mibs_access_t;


typedef enum {
	IPC_CONFIG_GET = 0x01,
	IPC_CONFIG_SET,
	IPC_CONFIG_DEL,
	IPC_CONFIG_NEW
}
ipc_config_access_t;

typedef enum {
	IPC_APPLY_NONE = 0,
	IPC_APPLY_NOW = 0x01,
	IPC_APPLY_COMMIT = 0x02,
	IPC_APPLY_DELAY = 0x04,
}
ipc_apply_option_t;

typedef struct {
	int ipc_type;
	uint32_t msg_len;
}
ipc_request_hdr_t;

typedef struct {
	int	status;
	int msg_len;
}
ipc_acknowledge_hdr_t;

enum{
	FILTER_TYPE_NONE = 0,
	FILTER_TYPE_MAC,
	FILTER_TYPE_ID,
};

typedef struct __packed {
	ipc_request_hdr_t hdr;
	uint8_t	clt;	// CLT id, 0xff for all
	uint8_t	update;	// Non-zero for update request
}
ipc_network_info_req_t;

typedef struct __packed {
	uint8_t clt;
	uint8_t index;
	ethernet_addr_t mac;
	uint8_t tei;
	uint16_t avgtx;
	uint16_t avgrx;
	char alias[ALIAS_NAME_SIZE];
	uint8_t link;
	uint8_t auth;
	int device_id;
	int template_id;
	time_t online_tm;
	char username[USER_NAME_SIZE];
	uint8_t ports;
	uint8_t state;
	char version[EOC_VERSION_SIZE];
}
ipc_cnu_info_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	uint8_t clt;
	uint16_t count;
	ipc_cnu_info_t cnus[];
}
ipc_network_info_ack_t;


typedef struct __packed {
	uint8_t		link;
	uint8_t		spd;
	uint8_t		duplex;
	uint32_t 	txpacket;
	uint32_t 	rxpacket;
	uint32_t 	rxcrc;
	uint32_t 	rxdrop;
	uint32_t 	rxfrag;
}
ipc_cnu_port_mib_t;

typedef struct __packed {
	ipc_request_hdr_t hdr;
	uint8_t filtertype;
	uint8_t clean;
	union {
		ethernet_addr_t mac;
		struct __packed {
			uint8_t clt;
			uint8_t cnu;
		}
		cnu_id;
	};
}
ipc_cnu_status_req_t;



typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	ipc_cnu_info_t cnu_info;
}
ipc_cnu_info_ack_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	uint8_t ports;
	ipc_cnu_port_mib_t cnu_mib[CNU_MAX_ETH_PORTS + 1];
}
ipc_cnu_mib_ack_t;


typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	cable_link_stats_t stats;
}
ipc_cnu_link_stats_ack_t;



typedef struct __packed {
	ipc_request_hdr_t hdr;
	uint8_t clean;
}
ipc_mme_stat_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	uint32_t send;
	uint32_t recv;
	uint32_t ack;
	uint32_t fail;
	uint32_t queues;
	uint32_t avglat;
	uint32_t queues_peak;
}
ipc_mme_stat_ack_t;

typedef struct __packed {
	ipc_request_hdr_t hdr;
	uint32_t cmd;
}
ipc_sysmon_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	uint32_t result;
}
ipc_sysmon_ack_t;

typedef struct __packed {
	ipc_request_hdr_t hdr;
    ifindex_t ifindex;
	uint8_t access;
	//char name[64];
	UINT oid;
	char value[2048];
}
ipc_nvram_access_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	char value[2048];
}
ipc_nvram_access_ack_t;

typedef struct __packed {
	ipc_request_hdr_t hdr;   
	char path[2048];
}
ipc_cfg_backup_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;  	
}
ipc_cfg_backup_ack_t;
typedef struct __packed {
	ipc_request_hdr_t hdr;   
	char path[2048];
}
ipc_show_startup_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;  	
}
ipc_show_startup_ack_t;


typedef struct __packed
{
    char username[MAX_VTY_USERNAME_LEN+1];
    char authmode[MAX_AUTHMODE_TXT_LEN+1];
    char passwd[MAX_VTY_PASSWD_LEN+1];
    char passwd_type[MAX_VTY_PASSWD_TYPE_LEN+1];
    unsigned long timeout;
}ipc_user_info_t;

typedef struct __packed
{
    ipc_user_info_t vty_user[MAX_VTY_COUNT];
}ipc_all_vty_user_info_t;

typedef struct __packed 
{
	ipc_request_hdr_t hdr;
	uint8_t access;
    ipc_all_vty_user_info_t vty_user_info; 
}ipc_get_user_info_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	ipc_all_vty_user_info_t vty_user_info;
}ipc_get_user_info_ack_t;
#if 0
typedef struct __packed {
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t apply_option;
	uint16_t count;
	ipc_service_template_t templates[];
}
ipc_service_template_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	uint16_t count;
	ipc_service_template_t templates[];
}
ipc_service_template_ack_t;
#endif
typedef struct __packed {
	ipc_request_hdr_t hdr;
}
ipc_supported_device_req_t;
typedef struct __packed {
        char ifname[IFNAMSIZ];
        uint8_t phy;
        uint8_t enabled;
        uint8_t autoneg;
        uint8_t speed;
        uint8_t duplex;
        uint8_t flowctrl;
        uint8_t priority;
        uint32_t ingress_limit; // 0 disabled 1k will be 1024  etc..
        uint32_t egress_limit;  // 0 disabled
        uint8_t ingress_filter;
        uint16_t pvid;// read only
}
switch_interface_t;

typedef struct __packed {
	char alias[ALIAS_NAME_SIZE];
	int device_id;
	int ports;
	int switch_chip_id;
	uint8_t port_map[CNU_MAX_ETH_PORTS + 1];
}
ipc_cnu_devinfo_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	uint16_t count;
	ipc_cnu_devinfo_t devinfo[MAX_SUPPORTED_CNU_MODELS];
}
ipc_supported_device_ack_t;
typedef struct __packed {
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t apply_option;	
	uint16_t count;
	switch_interface_t ifs[];
}
ipc_clt_interface_config_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	uint16_t count;
	switch_interface_t ifs[];
}
ipc_clt_interface_config_ack_t;

typedef struct __packed {
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t apply_option;	
	uint16_t count;
	switch_vlan_interface_t vlanifs[];
}
ipc_clt_vlan_interface_config_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	uint16_t count;
	switch_vlan_interface_t vlanifs[];
}
ipc_clt_vlan_interface_config_ack_t;


typedef struct __packed {
	ipc_request_hdr_t hdr;
	UINT8 access;
	UINT8 apply_option;
	switch_vlan_mode_t vlanmode;
}
ipc_clt_vlan_mode_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	switch_vlan_mode_t vlanmode;
}
ipc_clt_vlan_mode_ack_t;

typedef struct __packed {
	ipc_request_hdr_t hdr;
	UINT8 access;
	UINT8 apply_option;
	mac_setting_t mac_setting;
}
ipc_mac_setting_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	mac_setting_t mac_setting;
}
ipc_mac_setting_ack_t;



typedef struct __packed {
	ipc_request_hdr_t hdr;
	UINT8 access;
	UINT8 apply_option;
	catvinfo_t catvinfo;
}ipc_catvinfo_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	catvinfo_t catvinfo; 
}ipc_catvinfo_ack_t;


typedef struct __packed {
	ipc_request_hdr_t hdr;
	UINT8 access;
	UINT8 apply_option;
	wlan_ap_t wlan_ap;
}ipc_wlan_ap_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	wlan_ap_t wlan_ap; 
}ipc_wlan_ap_ack_t;

/*
  VLAN change note:
   For WEB 
   vlan edit, cmd = CONFIG_SET 
   vlan create, cmd = CONFIG_NEW
   vlan delete, cmd= CONFIG_DEL
   always as uint with a group with the same members

   For CLT or SNMP
   a group may have more vlans with different members
*/



typedef struct __packed {
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t apply_option;
	uint16_t count;
	switch_vlan_group_t vlan_groups[];
}
ipc_clt_vlan_config_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	int count;
	switch_vlan_group_t vlan_groups[];
}
ipc_clt_vlan_config_ack_t;

#define SHUTDOWN  0
#define UNDOSHUTDOWN 1
typedef struct __packed {
	ipc_request_hdr_t hdr;
	uint8_t apply_option;
	ifindex_t uiifindex;
	int op;
}
ipc_shutdown_interface_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;	
}
ipc_shutdown_interface_ack_t;

#if 0
typedef struct __packed {
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t filter;
	uint8_t apply_option;
	uint16_t count;
	cnu_user_config_t user[];
}
ipc_cnu_user_config_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	uint16_t count;
	cnu_user_config_t user[];
}
ipc_cnu_user_config_ack_t;
#endif
typedef struct __packed {
	ipc_request_hdr_t hdr;
	char ifname[IFNAMSIZ];
}
ipc_clt_interface_status_req_t;
#if 0
typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	uint16_t count;
	switch_interface_status_t status[];
}
ipc_clt_interface_status_ack_t;

#endif
typedef struct __packed{
	char ifname[IFNAMSIZ];	
	uint8_t reset; 		
}
ipc_ifmibs_t;

typedef struct __packed {
	ipc_request_hdr_t hdr;
	uint8_t access;	
	uint16_t count;
	ipc_ifmibs_t mibs_req[];
}
ipc_clt_interface_mibs_req_t;
#if 0
typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	uint16_t count;
	switch_interface_mib_t ifmibs[];
}
ipc_clt_interface_mibs_ack_t;
#endif

typedef struct __packed{
	cable_parameter_t clts[MAX_CLT_CHANNEL];
}
ipc_clt_cable_param_t;

typedef struct __packed{
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t apply_option;
	ipc_clt_cable_param_t cab_param;
}
ipc_clt_cable_param_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
	ipc_clt_cable_param_t cab_param;
}
ipc_clt_cable_param_ack_t;



typedef struct __packed{
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t apply_option;
	uint8_t mask;
	sys_security_t security;
}
ipc_sys_security_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
	sys_security_t security;
}
ipc_sys_security_ack_t;


typedef struct __packed{
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t apply_option;
	sys_snmp_t snmp;
}
ipc_sys_snmp_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
	sys_snmp_t snmp;
}
ipc_sys_snmp_ack_t;

typedef struct __packed{
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t apply_option;
	sys_syslog_t syslog;
}
ipc_syslog_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
	sys_syslog_t syslog;
}
ipc_syslog_ack_t;

typedef struct __packed{
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t apply_option;
	ipc_network_t network;
}
ipc_sys_network_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
	sys_network_t network;
}
ipc_sys_network_ack_t;


typedef struct __packed{
	ipc_request_hdr_t hdr;
}
ipc_sys_info_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
	sys_info_t info;
}
ipc_sys_info_ack_t;

typedef struct __packed{
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t apply_option;
	sys_time_t time;
}
ipc_sys_time_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
	sys_time_t time;
}
ipc_sys_time_ack_t;

typedef struct __packed{
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t apply_option;
	sys_admin_t admin;
}
ipc_sys_admin_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
	sys_admin_t admin;
}
ipc_sys_admin_ack_t;

typedef struct __packed{
	ipc_request_hdr_t hdr;
	sys_dhcpc_t dhcpc;
}
ipc_sys_dhcpc_req_t;


typedef struct __packed {
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t process;
}
ipc_system_req_t;



typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	uint8_t status;
	uint8_t process;
	uint8_t error;
	uint8_t percentage;
}
ipc_system_ack_t;

//mvlan
typedef struct __packed{
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t apply_option;
	sys_mvlan_t mvlan;
}
ipc_sys_mvlan_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
	sys_mvlan_t mvlan;
}
ipc_sys_mvlan_ack_t;

//loop
typedef struct __packed{
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t apply_option;
	sys_loop_t loop;
}
ipc_sys_loop_req_t;


typedef struct __packed{
	ipc_request_hdr_t hdr;
	uint8_t access;
	uint8_t apply_option;
	sys_epon_t epon_info;
}
ipc_sys_epon_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
	sys_epon_t epon_info;
}ipc_sys_epon_ack_t;
typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
	sys_loop_t loop;
}
ipc_sys_loop_ack_t;

typedef struct __packed{
    ipc_request_hdr_t hdr;
    uint8_t access;
    uint8_t apply_option;
    UINT32 uiQosTrustmode;
}
ipc_sys_qos_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
	UINT32 uiQosTrustmode;
}
ipc_sys_qos_ack_t;

typedef struct __packed{
    ipc_request_hdr_t hdr;
    uint8_t access;
    uint8_t apply_option;
    QueueMode_S Qosqueuemode;
}
ipc_sys_qosqueuemod_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
	QueueMode_S Qosqueuemode;
}
ipc_sys_qosqueuemod_ack_t;


typedef struct __packed{
    ipc_request_hdr_t hdr;
    uint8_t access;
    uint8_t apply_option;
    PORT_CFG_VALUE_S port_cfg_val[LOGIC_PORT_NO];
}
ipc_port_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
    PORT_CFG_VALUE_S port_cfg_val[LOGIC_PORT_NO];
}
ipc_port_ack_t;

typedef struct __packed{
    ipc_request_hdr_t hdr;
    uint8_t access;
    uint8_t apply_option;
    STROM_CTL_S stormctl[LOGIC_PORT_NO];
}
ipc_stormctl_req_t;


typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
    STROM_CTL_S stormctl[LOGIC_PORT_NO];
}
ipc_stormctl_ack_t;

typedef struct __packed{
    ipc_request_hdr_t hdr;
    uint8_t access;
    uint8_t apply_option;
    PORT_LINERATE_VALUES linerate_val[LOGIC_PORT_NO];
}
ipc_linerate_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
    PORT_CFG_VALUE_S linerate_val[LOGIC_PORT_NO];
}
ipc_linerate_ack_t;

typedef struct __packed{
    ipc_request_hdr_t hdr;
    uint8_t access;
    uint8_t apply_option;
    MIRROR_GROUP_S portmirror;
}
ipc_mirror_req_t;

typedef struct __packed{
	ipc_acknowledge_hdr_t hdr;
    MIRROR_GROUP_S portmirror;
}
ipc_mirror_ack_t;

typedef struct __packed {
	ipc_request_hdr_t hdr;
	UINT8 access;
	UINT8 apply_option;
	compatible_setting_t compatible_setting;
}
ipc_compatible_setting_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	compatible_setting_t compatible_setting;
}
ipc_compatible_setting_ack_t;
/*Begin add by linguobin 2014-04-22 for mib*/
#if defined(CONFIG_ZBL_SNMP)
typedef struct __packed {
	ipc_request_hdr_t hdr;
	UINT8 access;
	UINT8 apply_option;
	AnalogProperty_t AnalogProperty;
}
ipc_analogPropertyTable_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	AnalogProperty_t AnalogProperty;
}
ipc_analogPropertyTable_ack_t;

typedef struct __packed {
	ipc_request_hdr_t hdr;
	UINT8 access;
	UINT8 apply_option;
	commonAdminGroup_t commonAdminGroup;
}
ipc_commonAdminGroup_req_t;

typedef struct __packed {
	ipc_acknowledge_hdr_t hdr;
	commonAdminGroup_t commonAdminGroup;
}
ipc_commonAdminGroup_ack_t;
#endif
/*End add by linguobin 2014-04-22 for mib*/

#define	REQ_MSGLEN(x) (sizeof(x) - sizeof(ipc_request_hdr_t))
#define	REQ_MSGLEN_R(t, m, mt, c) (offsetof(t, m) + c * sizeof(mt) - sizeof(ipc_request_hdr_t))
#define	ACK_MSGLEN(x) (sizeof(x) - sizeof(ipc_acknowledge_hdr_t))
#define	ACK_MSGLEN_R(t, m, mt, c) (offsetof(t, m) + c * sizeof(mt) - sizeof(ipc_acknowledge_hdr_t))
int ipc_show_startupstart(void);
int ipc_show_startupend(void);
int ipc_get_vlan_mode(int ipc_fd,switch_vlan_mode_t *mode);
ipc_clt_vlan_config_ack_t *ipc_clt_vlan_config(int ipc_fd, uint8_t access, uint8_t apply_option, switch_vlan_group_t *vlan_group, uint16_t count);
int ipc_set_vlan_mode(int ipc_fd,switch_vlan_mode_t *mode);
int ipc_set_vlan(int ipc_fd,int access, switch_vlan_group_t *vg);
int ipc_shutdown_intf(int ipc_fd,ifindex_t ifindex,int op);
extern version_t sys_version;
extern void sys_config_init(void);
extern void sys_config_apply(void);
extern uint16_t sys_management_vlan(void);
extern int sys_management_vlan_update(uint16_t newvlan);

extern int sys_security_update(sys_security_t *cfg, int mask);
extern int sys_security_apply(int apply_options);
extern int sys_security_commit(int apply_options, int mask);
extern int sys_time_update(sys_time_t *cfg);
extern int sys_time_apply(int apply_options);
extern int sys_time_commit(int apply_options);
extern int sys_networking_update(ipc_network_t *cfg);
extern int sys_networking_apply(int apply_options);
extern int sys_networking_commit(int apply_options);
extern int sys_ip_dhcpc_event_apply(sys_dhcpc_t *dhcpc);
extern int sys_admin_update(sys_admin_t *cfg);
extern int sys_admin_apply(int apply_options);
extern int sys_admin_commit(int apply_options);
//extern uint32_t sys_get_uptime(void);
extern void sys_get_time(struct tm *tm);
extern void sys_info_mac_reload(void);
extern void sys_mac_update(ethernet_addr_t *newmac);
extern int sys_mac_apply(ethernet_addr_t *current_mac);
extern void sys_reboot(void);
extern void sys_reboot_later(int sec);
extern void sys_dealy_do(int action);

extern void ntpd_start(void);
extern int get_ntp_syn_state(void);
extern char *version_string(void);
extern int startup_process(char *cmd);
extern void sys_syslog_load(sys_syslog_t *sys);
extern int sys_syslog_update(sys_syslog_t *cfg);
extern void syslog_start(void);
extern int sys_syslog_apply(int apply_options);
extern int sys_syslog_commit(int apply_options);
extern void sys_mvlan_load(sys_mvlan_t *sys);
extern int sys_mvlan_apply(int apply_options);
extern int sys_mvlan_update(sys_mvlan_t *cfg);
extern int sys_mvlan_commit(int apply_options);

void add_enter(char *string);
int snmpdconf_access();

extern void sys_loop_load(sys_loop_t *sys);
extern void loopd_init(void);
extern int sys_loop_update(sys_loop_t *cfg);
extern int sys_loop_apply(int apply_options);
extern int sys_loop_commit(int apply_options);

extern void sys_info_load(sys_info_t *info);
extern void sys_networking_load(sys_network_t *sys);
extern void sys_time_load(sys_time_t *sys);
extern void sys_admin_load(sys_admin_t *sys);

extern INT sys_qosmode_update(UINT32 qosmode);
extern INT sys_queuesch_update(QueueMode_S *queuesch);
extern INT sys_qos_trustmode_commit(unsigned char apply_options);
extern INT sys_qos_queueschedul_commit(unsigned char apply_options);
void sys_snmp_load(sys_snmp_t *sys);
void snmpd_init();
extern int sys_snmp_apply(int apply_options, int access_flag);
extern int sys_snmp_update(sys_snmp_t *cfg, int *access_flag);
extern int sys_snmp_commit(int apply_options);
extern void sys_mac_setting_load(mac_setting_t *sys);
extern int sys_mac_setting_update(mac_setting_t *cfg);
extern int sys_mac_setting_apply(int apply_options);
extern int sys_mac_setting_commit(int apply_options);
#ifdef CONFIG_CATVCOM
extern void catv_init(void);
extern void sys_catv_load(catvinfo_t *sys); 
extern void catv_init_apply(catvinfo_t *sys); 
extern int sys_catv_update(catvinfo_t *cfg);
extern int sys_catv_apply(int apply_options); 
extern int sys_catv_commit(int apply_options);
#endif
#ifdef CONFIG_WLAN_AP
extern void sys_wlan_ap_load(wlan_ap_t *sys); 
extern void wlan_ap_init_apply(wlan_ap_t *sys);
extern int sys_wlan_ap_update(wlan_ap_t *cfg);
extern int sys_wlan_ap_apply(int apply_options);
extern int sys_wlan_ap_commit(int apply_options);
extern int get_wlan_mac_filter(wlan_ap_t *sys);
#endif
#if defined(CONFIG_ONU_COMPATIBLE)
extern void sys_compatible_load(compatible_setting_t *sys);
extern int sys_compatible_update(compatible_setting_t *cfg);
extern int sys_compatible_commit(int apply_options);
extern ipc_compatible_setting_ack_t *ipc_compatible_setting_config(int ipc_fd, uint8_t access, uint8_t apply_option, compatible_setting_t *cfg);
extern int oam_ipc_get_sys_compatible(int ipc_fd,compatible_setting_t *info);

#endif

#endif /* IPC_PROTOCOL_H_ */
