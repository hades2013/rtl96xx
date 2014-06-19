#ifndef __H3C_H__
#define __H3C_H__
#if 0
#ifndef OAM_SUPPORT_ZTE_EXT
#define OAM_SUPPORT_ZTE_EXT 1
#endif
#endif

#define FIBERHOME_OAM_OUI                         { 0x00, 0x0a, 0xc2 }
#define FIBERHOME_ORGSPEC_HDR_LEN                 11 /* OUI(3) + Ext. Opcode(1)  + branch(1) + leaf(2) + sequence(4)*/


//#include "error"
/* definition for h3c OAM extension */
#define OAM_FIBERHOME_EXT_OPCODE_GET_REQUEST    0x01
#define OAM_FIBERHOME_EXT_OPCODE_GET_RESPONSE   0x02
#define OAM_FIBERHOME_EXT_OPCODE_SET_REQUEST    0x03
#define OAM_FIBERHOME_EXT_OPCODE_SET_RESPONSE   0x04
#define OAM_FIBERHOME_BRANCH_STANDARD_ATTRIB		0x07
#define OAM_FIBERHOME_BRANCH_STANDARD_ACTION		0x09
#define OAM_FIBERHOME_BRANCH_EXTENDED_ATTRIB		0xC7
#define OAM_FIBERHOME_BRANCH_EXTENDED_ACTION		0xC9
#define OAM_FIBERHOME_BRANCH_INSTANCE_INDEX_2_1		0x37
#define OAM_FIBERHOME_BRANCH_INSTANCE_INDEX_2_0		0x36

#define OAM_FIBERHOME_EXT_LEAF_CONFIG_PARAM     0x0106   /*set*/
#define OAM_FIBERHOME_EXT_LEAF_MGMT_IP_CFG        0x4B00 /* set */
#define OAM_FIBERHOME_EXT_LEAF_DHCP		  	0X5C00
#define OAM_FIBERHOME_EXT_LEAF_MGMT_VLAN		  0X5900
#define OAM_FIBERHOME_EXT_LEAF_SPEED_SET		0X2100
#define OAM_FIBERHOME_EXT_LEAF_SPEED_SET_1000M		0X6D00
#define OAM_FIBERHOME_EXT_LEAF_DUPLEX_SET		0X2200
#define OAM_FIBERHOME_EXT_LEAF_MIRROR_SET		0X5600
#define OAM_FIBERHOME_EXT_LEAF_MONITOR_SET	0X5700
#define OAM_FIBERHOME_EXT_LEAF_ISOLATE_SET	0X4800
#define OAM_FIBERHOME_EXT_LEAF_MDI_SET		0X3200
#define OAM_FIBERHOME_EXT_LEAF_FILTER_SET		0X3e00
#define OAM_FIBERHOME_EXT_LEAF_UNI_STATS_RESET		0X3100
#define OAM_FIBERHOME_EXT_LEAF_UNI_STATS		0X2300
#define OAM_FIBERHOME_EXT_LEAF_BOARD_STATS	0X4F00
#define OAM_FIBERHOME_EXT_LEAF_MGMT_VLANIF		0X5a00
#define OAM_FIBERHOME_EXT_LEAF_UNI_CONFIG			0XFE00
#define OAM_FIBERHOME_EXT_LEAF_ONU_CONFIG			0XFF00
#define OAM_FIBERHOME_EXT_LEAF_MCAST_FAST_LEAVE	0X5500
#define OAM_FIBERHOME_EXT_LEAF_MAC_AGETIME	0X2003
#define OAM_FIBERHOME_EXT_LEAF_PORT_SEPERATE	0X2009
#define OAM_FIBERHOME_EXT_LEAF_MAC_LIMIT	0X2102
#define OAM_FIBERHOME_EXT_LEAF_ETH_MAC_TABLE 0X2106
#define OAM_FIBERHOME_EXT_LEAF_RSTP 0X210a
#define OAM_FIBERHOME_EXT_LEAF_MNT_IP	0X201a
#define OAM_FIBERHOME_EXT_LEAF_STATS 0X2013
#define OAM_FIBERHOME_EXT_LEAF_INSTANCE_MAC_LIMIT 0x0001

#define FIBERHOME_PORT_MODE_SPEED_AUTO 0X0000
#define FIBERHOME_PORT_MODE_SPEED_10M  0X0A00
#define FIBERHOME_PORT_MODE_SPEED_100M  0X6400
#define FIBERHOME_PORT_MODE_SPEED_1000M  0X03e8

#define FIBERHOME_PORT_MODE_DUPLEX_HALF 0X00
#define FIBERHOME_PORT_MODE_DUPLEX_FULL 0X01
#define FIBERHOME_PORT_MODE_DUPLEX_AUTO 0X02

#define FIBERHOME_PORT_MODE_MIRROR_INBOUND 0X0101
#define FIBERHOME_PORT_MODE_MIRROR_OUTBOUND 0X0201
#define FIBERHOME_PORT_MODE_MIRROR_BOTH 0X0301
#define FIBERHOME_PORT_MODE_MIRROR_UNDO 0X0302

#define FIBERHOME_PORT_MODE_MONITOR_SET 0X01
#define FIBERHOME_PORT_MODE_MONITOR_UNDO 0X02

#define FIBERHOME_ENABLE 0X01
#define FIBERHOME_DISABLE 0X02
#define OAM_FIBERHOME_EXT_LEAF_MAC_LEARN_ENABLE 0X01
#define OAM_FIBERHOME_EXT_LEAF_MAC_LEARN_DISABLE 0X00

#define FIBERHOME_PORT_MODE_MDI_NORMAL 0X00
#define FIBERHOME_PORT_MODE_MDI_ACROSS 0X01
#define FIBERHOME_PORT_MODE_MDI_AUTO 0X02

#define FIBERHOME_MGMT_IF_SHUTDOWN 0X02
#define FIBERHOME_MGMT_IF_SHUTDOWN_NO 0X01

#define FIBERHOME_PACKET_FILTER_ENABLE_SET 0X0001
#define FIBERHOME_PACKET_FILTER_DISABLE_SET 0X0002

#define FIBERHOME_PACKET_FILTER_TRUST_SMAC 0X0000
#define FIBERHOME_PACKET_FILTER_TRUST_DMAC 0X0001
#define FIBERHOME_PACKET_FILTER_TRUST_ETHTYPE 0X0002
#define FIBERHOME_PACKET_FILTER_TRUST_CVLAN 0x0003
#define FIBERHOME_PACKET_FILTER_TRUST_SIP 0X0004
#define FIBERHOME_PACKET_FILTER_TRUST_DIP 0X0005


#define FIBERHOME_PACKET_FILTER_TRUST_TCP_SPORT 8
#define FIBERHOME_PACKET_FILTER_TRUST_TCP_DPORT 9
#define FIBERHOME_PACKET_FILTER_TRUST_UDP_SPORT 10
#define FIBERHOME_PACKET_FILTER_TRUST_UDP_DPORT 11


//#define FIBERHOME_PORT_MODE_AUTO


#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

struct fiberhome_header {
	unsigned char oui[3];
	unsigned char ext_opcode;
}__attribute__((packed));
typedef struct fiberhome_header fiberhome_header_t;

struct fiberhome_packet_filter {
	unsigned short action;
	unsigned int type;
	unsigned int dirction;
	unsigned char  value[6];
}__attribute__((packed));
typedef struct fiberhome_packet_filter fiberhome_packet_filter_t;

struct fiberhome_variable{
	unsigned char	branch;
	unsigned short	leaf;
	unsigned char	width;
}__attribute__((packed));
typedef struct fiberhome_variable fiberhome_variable_t;

struct fiberhome_fast_leave{
	unsigned char	port;
	unsigned char	act;
}__attribute__((packed));
typedef struct fiberhome_fast_leave fiberhome_fast_leave_t;
struct filter_acl {
	unsigned char direction;
	unsigned char type;
	unsigned char value[6];
	unsigned char aclruleID;
	int validflag;
};
typedef struct filter_acl filter_acl_t;

struct fiberhome_stats 
{
	unsigned int	 speedType;
	unsigned int	 speed;
	unsigned int	 duplex;
	unsigned int	 duplexType;

	unsigned int    ifOutUcastPkts;
	unsigned int    ifOutMcastPkts;
	unsigned int    ifOutBcastPkts;
	unsigned int    ifInUcastPkts;
	unsigned int    ifInMcastPkts;
	unsigned int    ifInBcastPkts;

	unsigned int    ifOutPausePkts;
	unsigned int    ifInPausePkts;

	unsigned int    ifInRxErrors;
	unsigned int    ifInGiantsPkts;
	unsigned int    ifInRuntsPkts;
	unsigned int    ifInCRCErrors; 

	unsigned int    ifOutTxErr;
	unsigned int    ifOutcollisions;
	unsigned int    ifOutdeferred;
	
	unsigned long long    ifInOctets; 
	unsigned int	 ifOutOctetsL;
	unsigned int	 ifOutOctetsH;
	
	unsigned int    ifInBufferErrors;

};
typedef struct fiberhome_stats fiberhome_stats_t;

struct fiberhome_port_mode{
	unsigned int speedType;
	unsigned int duplexType;
};
typedef struct fiberhome_port_mode fiberhome_port_mode_t;

struct fiberhome_mac_aging
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u16_t	agingTime;
}__attribute__((packed));
typedef struct fiberhome_mac_aging fiberhome_mac_aging_t;
struct fiberhome_instance_mac_limit
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	value;
}__attribute__((packed));
typedef struct fiberhome_instance_mac_limit fiberhome_instance_mac_limit_t;
struct fiberhome_port_separate
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	separate;
}__attribute__((packed));
typedef struct fiberhome_port_separate fiberhome_port_separate_t;

struct fiberhome_mac_limit
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	enable;
	u16_t	numLimit;
}__attribute__((packed));
typedef struct fiberhome_mac_limit fiberhome_mac_limit_t;
struct fiberhome_adminip
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	struct in_addr ipaddr;
	struct in_addr mask;
	struct in_addr gateway;
}__attribute__((packed));
typedef struct fiberhome_adminip fiberhome_adminip_t;
struct fiberhome_adminvlan
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u16_t 	reserved;
	u16_t	vlan;
}__attribute__((packed));
typedef struct fiberhome_adminvlan fiberhome_adminvlan_t;
struct oam_fiberhome_mgmt_dhcp{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u16_t	vlanid;
	u8_t 	pri;
	u8_t	action;
}__attribute__((packed));
typedef struct oam_fiberhome_mgmt_dhcp oam_fiberhome_mgmt_dhcp_t;
#if 0
struct fiberhome_port_speed_mode
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	port;
	u8_t	mode;
}__attribute__((packed));
typedef struct fiberhome_port_speed_mode fiberhome_port_mode_t;
#endif
struct fiberhome_shutdowm
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u16_t	shutdowm;
}__attribute__((packed));
typedef struct fiberhome_shutdowm fiberhome_shutdowm_t;
struct fiberhome_mdi
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	port_num;
	u32_t	subtype;
}__attribute__((packed));
typedef struct fiberhome_mdi fiberhome_mdi_t;
struct fiberhome_stats_reset
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	port;	
}__attribute__((packed));
typedef struct fiberhome_stats_reset fiberhome_stats_reset_t;
struct fiberhome_port_state
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u16_t	mode;
}__attribute__((packed));
typedef struct fiberhome_port_state fiberhome_port_state_t;
struct fiberhome_rstp
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	value;	
}__attribute__((packed));
typedef struct fiberhome_rstp fiberhome_rstp_t;
struct fiberhome_port
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	port;	
}__attribute__((packed));
typedef struct fiberhome_port fiberhome_port_t;

struct fiberhome_get_mac_var
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	macNum;
}__attribute__((packed));
typedef struct fiberhome_get_mac_var fiberhome_get_mac_var_t;
struct fiberhome_mac_field
{
	u8_t	mac[6];
	u16_t	vlan;
	u32_t	portMask;
}__attribute__((packed));
typedef struct fiberhome_mac_field fiberhome_mac_field_t;
struct fiberhome_mac_bind_static
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u16_t 	action;
	u8_t 	reseved;
	u8_t 	mac[6];
	u16_t 	vlan;
	u32_t	portMask;
}__attribute__((packed));
typedef struct fiberhome_mac_bind_static fiberhome_mac_bind_static_t;

struct fiberhome_port_stats
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;

	u64_t   ifInOctets; 
	u64_t	ifInTotalPkts;
	u32_t   ifInUcastPkts;
	u32_t   ifInMcastPkts;
	u32_t   ifInBcastPkts;
	u32_t   ifInRxDiscardPkts;
	u32_t	ifInErrorPkts;
	u32_t   ifInRuntsPkts;
	u32_t   ifInGiantsPkts;
	u32_t	ifInUnknowPkts;
	u32_t	ifInCRCPkts;
	
	u64_t	ifOutOctets;
	u64_t	ifOutTotalPkts;
	u32_t   ifOutUcastPkts;
	u32_t   ifOutMcastPkts;
	u32_t   ifOutBcastPkts;
	
	u32_t   ifRxRate;
	u32_t   ifTxRate;
}__attribute__((packed));
typedef struct fiberhome_port_stats fiberhome_port_stats_t;


#if 0
//OAM_ZTE_EXT_LEAF_MGMT_IP_CFG        0x201a /* set */
struct oam_fiberhome_mgmt_ip_cfg {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t   ip_addr;          /* ONU IP address */
	u32_t   ip_mask;          /* ONU IP mask */
	u8_t    mgmt_pri;         /* Mgmt Priority */
	u16_t   mgmt_vlan;        /* Mgmt vlan */
	u8_t    mgmt_status;      /* 0x01: Enable 0x02:Disable */
	u32_t   mgmt_host_subnet; /* 远程主机路由网段 */
	u32_t   mgmt_host_mask;   /* 远程主机掩码 */
	u32_t   mgmt_gateway;      /* 网段所指向的网关 */
}__attribute__((packed));
typedef struct oam_fiberhome_mgmt_ip_cfg oam_fiberhome_mgmt_ip_cfg_t;
#endif
extern int eopl_ip_processing(struct in_addr ipaddr, struct in_addr mask, struct in_addr gateway);
extern int eopl_mgmtvlan_processing(unsigned short vlanid);
extern int eopl_dhcp_handler(void);
extern int eopl_fiberhome_set_default_ip(unsigned char* p_in);
extern int fiberhome_oam_init(void);

#endif

