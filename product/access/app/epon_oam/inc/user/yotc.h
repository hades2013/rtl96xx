/*=============================================================================
FILE - optc.c

DESCRIPTION
	Implements the optc private oam.

MODIFICATION DETAILS
=============================================================================*/
#ifndef __YOTC_H__
#define __YOTC_H__
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>	
/* definition for OPTC OAM extension */

#define YOTC_OAM_OUI							{0x00, 0x0c, 0xd8}
#define YOTC_ORGSPEC_HDR_LEN					7 /* OUI(3) + Ext. Opcode(1)  + branch(1) + leaf(2)*/


#define OAM_YOTC_EXT_OPCODE_GET_REQUEST    0x01
#define OAM_YOTC_EXT_OPCODE_GET_RESPONSE   0x02
#define OAM_YOTC_EXT_OPCODE_SET_REQUEST    0x03
#define OAM_YOTC_EXT_OPCODE_SET_RESPONSE   0x04

#define OAM_YOTC_BRANCH_STANDARD_ATTRIB		0x07
#define OAM_YOTC_BRANCH_STANDARD_ACTION		0x09
#define OAM_YOTC_BRANCH_EXTENDED_ATTRIB		0xC7
#define OAM_YOTC_BRANCH_EXTENDED_ACTION		0xC9

#define OAM_YOTC_EXT_LEAF_STATS		0X0043
#define OAM_YOTC_EXT_LEAF_OPTICS		0X0050
#define OAM_YOTC_EXT_LEAF_RSTP		0X0016
#define OAM_YOTC_EXT_LEAF_ETH_STATE 0X0013
#define OAM_YOTC_EXT_LEAF_PORT_MODE 0X0012
#define OAM_YOTC_EXT_LEAF_ISOLATE 	0X000A
#define OAM_YOTC_EXT_LEAF_ADMIN_IP	0x0019
#define OAM_YOTC_EXT_LEAF_ADMIN_VLAN	0x0024
#define OAM_YOTC_EXT_LEAF_MAC_AGING		0x0010
#define OAM_YOTC_EXT_LEAF_MAC_LIMIT 	0X0005
#define OAM_YOTC_EXT_LEAF_MAC_BLACK		0x0033
#define OAM_YOTC_EXT_LEAF_MAC_WHITE		0X0021
#define OAM_YOTC_EXT_LEAF_VLAN_TRUNK	0X0017
#define OAM_YOTC_EXT_LEAF_UPGRADE		0X0031

typedef enum{
	YOTC_PORT_MODE_AUTO = 0,
	YOTC_PORT_MODE_10HALF,
	YOTC_PORT_MODE_10FULL,
	YOTC_PORT_MODE_100HALF,
	YOTC_PORT_MODE_100FULL,
	YOTC_PORT_MODE_1G,
	YOTC_PORT_MODE_10G
}
yotc_port_mode_e;

#define OAM_YOTC_ENABLE 	0X01
#define OAM_YOTC_DISABLE 	0X00

#define OAM_YOTC_TRUNK_DEL 	0X0000
#define OAM_YOTC_TRUNK_ADD 	0X0100
#define OAM_YOTC_TRUNK_PVID 0X0200

struct yotc_header {
	u8_t oui[3];
	u8_t ext_opcode;
}__attribute__((packed));
typedef struct yotc_header yotc_header_t;

struct yotc_stats_reset
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	port;	
}__attribute__((packed));
typedef struct yotc_stats_reset yotc_stats_reset_t;

struct yotc_optics
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u16_t	tranceiverTemp;
	u16_t	supplyVolt;
	u16_t	biasCurrent;
	u16_t	txPower;
	u16_t	rxPower;
}__attribute__((packed));
typedef struct yotc_optics yotc_optics_t;

struct yotc_port_stats
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
typedef struct yotc_port_stats yotc_port_stats_t;

struct yotc_port
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	port;	
}__attribute__((packed));
typedef struct yotc_port yotc_port_t;

struct yotc_rstp
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	value;	
}__attribute__((packed));
typedef struct yotc_rstp yotc_rstp_t;

struct yotc_port_state
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u16_t	mode;
}__attribute__((packed));
typedef struct yotc_port_state yotc_port_state_t;

struct yotc_port_mode
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	port;
	u8_t	mode;
}__attribute__((packed));
typedef struct yotc_port_mode yotc_port_mode_t;

struct yotc_port_isolate
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	action;
}__attribute__((packed));
typedef struct yotc_port_isolate yotc_port_isolate_t;

struct yotc_adminip
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	struct in_addr ipaddr;
	struct in_addr mask;
	struct in_addr gateway;
}__attribute__((packed));
typedef struct yotc_adminip yotc_adminip_t;

struct yotc_adminvlan
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u16_t 	reserved;
	u16_t	vlan;
}__attribute__((packed));
typedef struct yotc_adminvlan yotc_adminvlan_t;

struct yotc_mac_aging
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	agingTime;
}__attribute__((packed));
typedef struct yotc_mac_aging yotc_mac_aging_t;

struct yotc_mac_limit
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	port;
	u8_t	numLimit;
}__attribute__((packed));
typedef struct yotc_mac_limit yotc_mac_limit_t;

struct yotc_mac_white
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	action;
	u8_t	mac[6];
	u8_t 	port;
}__attribute__((packed));
typedef struct yotc_mac_white yotc_mac_t;

struct yotc_port_trunk
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	port;
	u16_t	type;
	u8_t 	pri;
	u16_t	vlanid;
	u16_t	action;
}__attribute__((packed));
typedef struct yotc_port_trunk yotc_port_trunk_t;

struct yotc_onu_upgrade
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
}__attribute__((packed));
typedef struct yotc_onu_upgrade yotc_onu_upgrade_t;

extern int yotc_oam_init(void);
//extern void eopl_yotc_pdu_process(u8_t *p_byte,u16_t length);
//extern void eopl_yotc_pdu_process2(u8_t *p_byte,u16_t length);

#endif

