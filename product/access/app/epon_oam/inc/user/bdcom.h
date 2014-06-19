#ifndef __BDCOM_H__
#define __BDCOM_H__

/* definition for BDCOM OAM extension */

#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

//#include "lw_drv_pub.h"

#define BDCOM_OAM_OUI							{0x00, 0xe0, 0x0f}
#define BDCOM_ORGSPEC_HDR_LEN					7 /* OUI(3) + Ext. Opcode(1)  + branch(1) + leaf(2)*/


/* definition for BDCOM OAM extension */
#define OAM_BDCOM_EXT_OPCODE_GET_REQUEST    0x01
#define OAM_BDCOM_EXT_OPCODE_GET_RESPONSE   0x02
#define OAM_BDCOM_EXT_OPCODE_SET_REQUEST    0x03
#define OAM_BDCOM_EXT_OPCODE_SET_RESPONSE   0x04
#define OAM_BDCOM_BRANCH_INSTANCE_V_2_1		0x37

#define OAM_BDCOM_BRANCH_STANDARD_ATTRIB		0x07
#define OAM_BDCOM_BRANCH_STANDARD_ACTION		0x09
#define OAM_BDCOM_BRANCH_EXTENDED_ATTRIB		0xC7
#define OAM_BDCOM_RANCH_EXTENDED_ACTION			0xC9


/* bdcom Private extension */
#define OAM_BDCOM_EXT_LEAF_ISOLATE			0X0006
#define OAM_BDCOM_EXT_LEAF_MIRROR			0X000B
#define OAM_BDCOM_EXT_LEAF_LOOKBACK			0X0014
#define OAM_BDCOM_EXT_LEAF_SPEED			0X0015
#define OAM_BDCOM_EXT_LEAF_DUPLEX			0X0016
#define OAM_BDCOM_EXT_LEAF_MGMTIP			0X0007
#define OAM_BDCOM_EXT_LEAF_MAC				0X001F
#define OAM_BDCOM_EXT_LEAF_MAC_NUM			0X0012
#define OAM_BDCOM_EXT_LEAF_PORT_STATS		0X0017
#define OAM_BDCOM_EXT_LEAF_MAC_AGING		0X0003
#define OAM_BDCOM_EXT_LEAF_MAC_LEARNING		0X0002
#define OAM_BDCOM_EXT_LEAF_MAC_BIND_STATIC	0X0001
#define OAM_BDCOM_EXT_LEAF_SPANNING_TREE	0x000a


#define OAM_BDCOM_EXT_LEAF_ISLATE_SET	0X01
#define OAM_BDCOM_EXT_LEAF_ISLATE_UNSET	0X00

#define OAM_BDCOM_EXT_LEAF_SPEED_10M 0X01
#define OAM_BDCOM_EXT_LEAF_SPEED_100M 0X02
#define OAM_BDCOM_EXT_LEAF_SPEED_AUTO 0X04

#define OAM_BDCOM_EXT_LEAF_DUPLEX_HALF 0X02
#define OAM_BDCOM_EXT_LEAF_DUPLEX_FULL 0X01
#define OAM_BDCOM_EXT_LEAF_DUPLEX_AUTO 0X03

#define OAM_BDCOM_EXT_LEAF_IP_DHCP 0X01
#define OAM_BDCOM_EXT_LEAF_IP_STATIC 0X02

#define OAM_BDCOM_EXT_LEAF_PORT_LOOKBACK 0X01
#define OAM_BDCOM_EXT_LEAF_PORT_LOOKBACK_UNSET 0X00

#define OAM_BDCOM_EXT_LEAF_PORT_MIRROR 0X01
#define OAM_BDCOM_EXT_LEAF_PORT_MIRROR_UNSET 0X00


#define OAM_BDCOM_EXT_LEAF_PORT_MAC_ENABLE 0X01
#define OAM_BDCOM_EXT_LEAF_PORT_MAC_DISABLE 0X00

//#define OAM_BDCOM_EXT_LEAF_MAC_LEARN_SVL		0X01
#define OAM_BDCOM_EXT_LEAF_MAC_LEARN_ENABLE		0X02
#define OAM_BDCOM_EXT_LEAF_MAC_LEARN_DISABLE	0X00

#define OAM_BDCOM_EXT_LEAF_ENABLE 0X01
#define OAM_BDCOM_EXT_LEAF_DISABLE 0X00

#define OAM_BDCOM_EXT_LEAF_MAC_STATIC_ADD		0X0100
#define OAM_BDCOM_EXT_LEAF_MAC_STATIC_DEL		0X0204
#define OAM_BDCOM_EXT_LEAF_MAC_DYNAMIC_DEL		0X0203
#define OAM_BDCOM_EXT_LEAF_MAC_DYNAMIC_DEL_ALL	0X0201

#define BDCOM_MAC_STATIC		1
#define BDCOM_MAC_DYNAMIC		0

struct bdcom_header {
	u8_t	oui[3];
	u8_t	ext_opcode;
}__attribute__((packed));
typedef struct bdcom_header bdcom_header_t;


struct bdcom_rsp_indication {
	u8_t	branch;
	u16_t	leaf;
	u8_t	var_indication;
}__attribute__((packed));
typedef struct bdcom_rsp_indication bdcom_rsp_indication_t;

struct bdcom_port_isolate
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	action;	
}__attribute__((packed));
typedef struct bdcom_port_isolate bdcom_port_isolate_t;

struct bdcom_port_mirror
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	action[6];	
}__attribute__((packed));
typedef struct bdcom_port_mirror bdcom_port_mirror_t;

struct bdcom_port_speed
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	action;	
}__attribute__((packed));
typedef struct bdcom_port_speed bdcom_port_speed_t;

struct bdcom_port_duplex
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	action;	
}__attribute__((packed));
typedef struct bdcom_port_duplex bdcom_port_duplex_t;

struct bdcom_port_lookback
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	action;	
}__attribute__((packed));
typedef struct bdcom_port_lookback bdcom_port_lookback_t;


struct bdcom_mgmtip
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	action;
	struct in_addr ipaddr;
	struct in_addr mask;
	struct in_addr gateway;
	u16_t mvlan;	
}__attribute__((packed));
typedef struct bdcom_mgmtip bdcom_mgmtip_t;

struct bdcom_monitor
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	session;
	u8_t	action;
	u8_t 	monitor_port;
	u8_t 	mirror_num;	
}__attribute__((packed));
typedef struct bdcom_monitor bdcom_monitor_t;

struct bdcom_mirror
{
	u8_t 	monitor_port;
	u8_t 	monitor_dir;	
}__attribute__((packed));
typedef struct bdcom_mirror bdcom_mirror_t;

typedef struct
{
	u8_t port;
	u8_t dir;
}port_mirror_t;


typedef struct
{
	u8_t empty_flag;
	u8_t monitor_port;	
	port_mirror_t mirror_port[3];
}port_monitor_t;

struct bdcom_mac
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;	
	u8_t 	action;
}__attribute__((packed));
typedef struct bdcom_mac bdcom_mac_t;


struct bdcom_port_mac_num
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;	
	u16_t 	mac_num;
}__attribute__((packed));
typedef struct bdcom_port_mac_num bdcom_port_mac_num_t;

/*debug struct to delet*/

struct bdcom_get_var
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t 	value;
}__attribute__((packed));
typedef struct bdcom_get_var bdcom_get_var_t;

struct oam_bdcom_port_stats{
	u8_t     branch;
	u16_t    leaf;
	u8_t     width;
	u64_t    ifInGoodOctets;       /* 收到的上行包字节数 */
	u32_t	 ifInBadOctets;
	u32_t    ifInBcastPkts;   /* 收到的上行广播包数目 */
	u32_t 	 ifInMcastPkts;	
	u32_t    ifInUcastPkts;    /* 收到的上行单播包数目 */
	u32_t	 ifInPausePkts;	
	u32_t    ifInMacErrors;
	u32_t 	 ifInFCSErrors;
	u32_t 	 ifInUndersizePkts;
	u32_t 	 ifInFragmentPkts;
	u32_t 	 ifInOversizePkts;
	u32_t    ifJabberPkts;
	
	u64_t    ifOutOctets;      /* 收到的下行包字节数  */
	u32_t    ifOutBcastPkts;  
	u32_t    ifOutMcastPkts; 
	u32_t    ifOutUcastPkts;   /* 收到的下行单播包数目  */
	u32_t    ifOutPausePkts;
	u32_t 	 ifOutFCSErrors;
	u32_t 	 ifOutDeferredPkts;
	u32_t 	 ifOutExcessivePkts;
	u32_t 	 ifOutScollisionPkts;
	u32_t 	 ifOutMCollisionPkts;
	u32_t    ifOutLatePkts;
	u32_t 	 ifOutCollisionPkts;
	
	u32_t    if_64_Octets;   /*inPkts+outPkts*/
	u32_t    if_64_127_Octets;      
	u32_t    if_128_255_Octets;
	u32_t    if_256_511_Octets;
	u32_t    if_512_1023_Octets;
	u32_t    if_1023_max_Octets;
}__attribute__((packed));
typedef struct oam_bdcom_port_stats oam_bdcom_port_stats_t;

struct bdcom_mac_aging
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t 	agingTime;
}__attribute__((packed));
typedef struct bdcom_mac_aging bdcom_mac_aging_t;

struct bdcom_mac_learning
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t 	action;
}__attribute__((packed));
typedef struct bdcom_mac_learning bdcom_mac_learning_t;

struct bdcom_mac_bind_static
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
typedef struct bdcom_mac_bind_static bdcom_mac_bind_static_t;

struct bdcom_spanning_tree
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t 	action;
}__attribute__((packed));
typedef struct bdcom_spanning_tree bdcom_spanning_tree_t;

struct bdcom_mac_field
{
	u8_t	mac[6];
	u16_t	vlan;
	u32_t	portMask;
}__attribute__((packed));
typedef struct bdcom_mac_field bdcom_mac_field_t;


struct bdcom_get_mac_var
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u16_t	reserve;
	u8_t	macNum;
}__attribute__((packed));
typedef struct bdcom_get_mac_var bdcom_get_mac_var_t;


//extern void eopl_bdcom_pdu_process(u8_t *p_byte, u16_t length);
extern int bdcom_oam_init(void);

#endif


