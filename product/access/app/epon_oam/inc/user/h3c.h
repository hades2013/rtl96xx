#ifndef __H3C_H__
#define __H3C_H__

/* definition for ZTE OAM extension */
#if 0
#ifndef OAM_SUPPORT_ZTE_EXT
#define OAM_SUPPORT_ZTE_EXT 1
#endif
#endif

#define H3C_OAM_OUI                         { 0x00, 0x0f, 0xe2 }
#define H3C_ORGSPEC_HDR_LEN                 11 /* OUI(3) + Ext. Opcode(1)  + branch(1) + leaf(2) + sequence(4)*/


//#include "error"
/* definition for h3c OAM extension */
#define OAM_H3C_EXT_OPCODE_GET_REQUEST    0x01
#define OAM_H3C_EXT_OPCODE_GET_RESPONSE   0x02
#define OAM_H3C_EXT_OPCODE_SET_REQUEST    0x03
#define OAM_H3C_EXT_OPCODE_SET_RESPONSE   0x04

#define OAM_H3C_EXT_LEAF_CONFIG_PARAM     0x0106   /*set*/
#define OAM_H3C_EXT_LEAF_MGMT_IP_CFG        0x4B00 /* set */
#define OAM_H3C_EXT_LEAF_DHCP		  	0X5C00
#define OAM_H3C_EXT_LEAF_MGMT_VLAN		  0X5900
#define OAM_H3C_EXT_LEAF_SPEED_SET		0X2100
#define OAM_H3C_EXT_LEAF_SPEED_SET_1000M		0X6D00
#define OAM_H3C_EXT_LEAF_DUPLEX_SET		0X2200
#define OAM_H3C_EXT_LEAF_MIRROR_SET		0X5600
#define OAM_H3C_EXT_LEAF_MONITOR_SET	0X5700
#define OAM_H3C_EXT_LEAF_ISOLATE_SET	0X4800
#define OAM_H3C_EXT_LEAF_MDI_SET		0X3200
#define OAM_H3C_EXT_LEAF_FILTER_SET		0X3e00
#define OAM_H3C_EXT_LEAF_UNI_STATS_RESET		0X3100
#define OAM_H3C_EXT_LEAF_UNI_STATS		0X2300
#define OAM_H3C_EXT_LEAF_BOARD_STATS	0X4F00
#define OAM_H3C_EXT_LEAF_MGMT_VLANIF		0X5a00
#define OAM_H3C_EXT_LEAF_UNI_CONFIG			0XFE00
#define OAM_H3C_EXT_LEAF_ONU_CONFIG			0XFF00
#define OAM_H3C_EXT_LEAF_MCAST_FAST_LEAVE	0X5500


#define H3C_PORT_MODE_SPEED_AUTO 0X0000
#define H3C_PORT_MODE_SPEED_10M  0X0A00
#define H3C_PORT_MODE_SPEED_100M  0X6400
#define H3C_PORT_MODE_SPEED_1000M  0X03e8

#define H3C_PORT_MODE_DUPLEX_HALF 0X00
#define H3C_PORT_MODE_DUPLEX_FULL 0X01
#define H3C_PORT_MODE_DUPLEX_AUTO 0X02

#define H3C_PORT_MODE_MIRROR_INBOUND 0X0101
#define H3C_PORT_MODE_MIRROR_OUTBOUND 0X0201
#define H3C_PORT_MODE_MIRROR_BOTH 0X0301
#define H3C_PORT_MODE_MIRROR_UNDO 0X0302

#define H3C_PORT_MODE_MONITOR_SET 0X01
#define H3C_PORT_MODE_MONITOR_UNDO 0X02

#define H3C_ENABLE 0X01
#define H3C_DISABLE 0X02

#define H3C_PORT_MODE_ISOLATE_SET 0X01
#define H3C_PORT_MODE_ISOLATE_UNDO 0X02
#define H3C_PORT_MODE_ISOLATE_ALL 0XFF


#define H3C_PORT_MODE_MDI_NORMAL 0X00
#define H3C_PORT_MODE_MDI_ACROSS 0X01
#define H3C_PORT_MODE_MDI_AUTO 0X02

#define H3C_MGMT_IF_SHUTDOWN 0X02
#define H3C_MGMT_IF_SHUTDOWN_NO 0X01

#define H3C_PACKET_FILTER_ENABLE_SET 0X0001
#define H3C_PACKET_FILTER_DISABLE_SET 0X0002

#define H3C_PACKET_FILTER_TRUST_SMAC 0X0000
#define H3C_PACKET_FILTER_TRUST_DMAC 0X0001
#define H3C_PACKET_FILTER_TRUST_ETHTYPE 0X0002
#define H3C_PACKET_FILTER_TRUST_CVLAN 0x0003
#define H3C_PACKET_FILTER_TRUST_SIP 0X0004
#define H3C_PACKET_FILTER_TRUST_DIP 0X0005


#define H3C_PACKET_FILTER_TRUST_TCP_SPORT 8
#define H3C_PACKET_FILTER_TRUST_TCP_DPORT 9
#define H3C_PACKET_FILTER_TRUST_UDP_SPORT 10
#define H3C_PACKET_FILTER_TRUST_UDP_DPORT 11


//#define H3C_PORT_MODE_AUTO


#define OAM_H3C_EXT_LEAF_TYPE_C_SET         0x0109 /* set */
#define OAM_H3C_EXT_LEAF_TYPE_C_GET         0x010a /* get */
#define OAM_H3C_EXT_LEAF_SET_PORT_MODE      0x0204 /* set */
#define OAM_H3C_EXT_LEAF_GET_PORT_MODE      0x0205 /* get */

#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

/*Begin added by sunmingliang 2013-01-18*/
struct h3c_header {
	unsigned char oui[3];
	unsigned char ext_opcode;
	unsigned char branch;
	unsigned short leaf;
	unsigned char sequence[4];
}__attribute__((packed));
typedef struct h3c_header h3c_header_t;

struct h3c_packet_filter {
	unsigned short action;
	unsigned int type;
	unsigned int dirction;
	unsigned char  value[6];
}__attribute__((packed));
typedef struct h3c_packet_filter h3c_packet_filter_t;

struct h3c_variable{
	unsigned char	branch;
	unsigned short	leaf;
	unsigned char	width;
}__attribute__((packed));
typedef struct h3c_variable h3c_variable_t;

struct h3c_fast_leave{
	unsigned char	port;
	unsigned char	act;
}__attribute__((packed));
typedef struct h3c_fast_leave h3c_fast_leave_t;
struct filter_acl {
	unsigned char direction;
	unsigned char type;
	unsigned char value[6];
	unsigned char aclruleID;
	int validflag;
};
typedef struct filter_acl filter_acl_t;

struct h3c_stats 
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
typedef struct h3c_stats h3c_stats_t;

struct h3c_port_mode{
	unsigned int speedType;
	unsigned int duplexType;
};
typedef struct h3c_port_mode h3c_port_mode_t;

extern int eopl_ip_processing(struct in_addr ipaddr, struct in_addr mask, struct in_addr gateway);
extern int eopl_mgmtvlan_processing(unsigned short vlanid);
extern int eopl_dhcp_handler(void);
extern int eopl_h3c_set_default_ip(unsigned char* p_in);
extern int ctc_oam_init(void);

#endif

