#ifndef __HUAWEI_H__
#define __HUAWEI_H__

/* definition for HUAWEI OAM extension */
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include "oam.h"
#define SHARE_OAM_OUI 						{0x00, 0x0c, 0xd5}
#define SHARE_ORGSPEC_HDR_LEN                 7 /* OUI(3) + Ext. Opcode(1)  + branch(1) + leaf(2) + sequence(4)*/

#define HUAWEI_OAM_OUI						{0xbc, 0x61, 0x4e}
#define HUAWEI_ORGSPEC_HDR_LEN                 7 /* OUI(3) + Ext. Opcode(1)  + branch(1) + leaf(2) + sequence(4)*/
//#include "error"
/* definition for h3c OAM extension */
#define OAM_HUAWEI_EXT_OPCODE_GET_REQUEST    0x01
#define OAM_HUAWEI_EXT_OPCODE_GET_RESPONSE   0x02
#define OAM_HUAWEI_EXT_OPCODE_SET_REQUEST    0x03
#define OAM_HUAWEI_EXT_OPCODE_SET_RESPONSE   0x04

#define OAM_HUAWEI_EXT_LEAF_SET_RINGCHECK	0x00b1
#define OAM_HUAWEI_EXT_LEAF_UNI_CLEAR			0x009c 
#define OAM_HUAWEI_EXT_LEAF_GET_OPTICS			0x00f0
#define OAM_HUAWEI_EXT_LEAF_GET_ETH_STATS   0X009b


#define OAM_HUAWEI_EXT_LEAF_RINGCHECK_ENABLE   0X01
#define OAM_HUAWEI_EXT_LEAF_RINGCHECK_DISABLE   0X00


/*huawei ip config*/
#define OAM_HUAWEI_IP_MGMT_VLAN_CONFIG			0X0090

#define OAM_HUAWEI_PORT_MODE_GET			0X00070006
#define OAM_HUAWEI_PORT_MODE_SET			0X00060006

#define OAM_HUAWEI_MODE_HALF	0X00000000
#define OAM_HUAWEI_MODE_FULL	0X00000001

#define OAM_HUAWEI_MODE_10M		0X00000000
#define OAM_HUAWEI_MODE_100M	0X00000001
#define OAM_HUAWEI_MODE_1000M	0X00000002


/*Begin added by sunmingliang 2013-01-18*/

struct huawei_header {
	u8_t	oui[3];
	u8_t	ext_code;
}__attribute__((packed));
typedef struct huawei_header huawei_header_t;

struct oam_huawei_instance_tlv_2_0 {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	value;
}__attribute__((packed));
typedef struct oam_huawei_instance_tlv_2_0 oam_huawei_instance_tlv_2_0_t;



struct oam_huawei_ip_mgmt_vlan_set {
	/*
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;*/
	u16_t   vlanid;
	struct in_addr ipaddr;
	struct in_addr mask;
	struct in_addr gateway;
	u16_t pri;
}__attribute__((packed));
typedef struct oam_huawei_ip_mgmt_vlan_set oam_huawei_ip_mgmt_vlan_set_t;

struct oam_onu_optics_info {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	temperature;
	u32_t	supplyVolt;
	u16_t	biasCurrent;
	u16_t	txPower;
	u16_t	rxPower;
}__attribute__((packed));
typedef struct oam_onu_optics_info oam_onu_optics_info_t;


struct oam_huawei_ringcheck {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	action;
}__attribute__((packed));
typedef struct oam_huawei_ringcheck oam_huawei_ringcheck_t;


struct oam_huawei_port_stats {
	u8_t	branch;
	u16_t	leaf;
	u16_t	width;
	u64_t   ifInTotalPkts;
	u64_t   ifInUcastPkts;
	u64_t   ifInMcastPkts;
	u64_t   ifInBcastPkts;
	u64_t   ifIn_64_Pkts;
	u64_t   ifIn_65_127_Pkts;
	u64_t   ifIn_128_255_Pkts;
	u64_t   ifIn_256_511_Pkts;
	u64_t   ifIn_512_1023_Pkts;
	u64_t   ifIn_1024_1518_Pkts;
	u64_t   ifInOver1518Pkts;
	u64_t   ifInUndersizePkts;
	u64_t   ifInOversizePkts;
	
	u64_t   ifInFragmentsPkts;
	u64_t   ifInJabbersPkts;
	u64_t   ifInFCSErrors;
	u64_t   ifInDiscardPkts;
	u64_t   ifInAlignmentErrorPkts;
	u64_t   ifInSubErrorsPkts;
	u64_t   ifInPPPOEFilterPkts;
	u64_t   ifInOverflowsPkts;
	u64_t   ifInPausePkts;
	u64_t   ifInGoodOctets;
	u64_t   ifInEthernetOctets;

	u64_t   ifOutPkts;
	u64_t   ifOutUcastPkts;
	u64_t   ifOutMcastPkts;
	u64_t   ifOutBcastPkts;
	u64_t   ifOut_64_Pkts;
	u64_t   ifOut_65_127_Pkts;
	u64_t   ifOut_128_255_Pkts;
	u64_t   ifOut_256_511_Pkts;
	u64_t   ifOut_1024_1518_Pkts;
	u64_t   ifOutOver_1518_Pkts;
	u64_t   ifOutOversizePkts;
	u64_t   ifOutDelayPkts;
	u64_t   ifOutOverMTUPkts;
	u64_t   ifOutSenseErrorsPkts;
	u64_t   ifOutSQEErrorMessages;
	u64_t   ifOutScollisionPkts;
	u64_t   ifOutMcollisionPkts;
	u64_t   ifOutTotalCollisionPkts;
	u64_t   ifOutLateCollisionPkts;
	u64_t   ifOutSubErrorPkts;
	u64_t   ifOutOverflowsPkts;
	u64_t   ifOutPausePkts;
	u64_t   ifOutGoodOctets;
}__attribute__((packed));
typedef struct oam_huawei_port_stats oam_huawei_port_stats_t;

struct huawei_header2 {
	u8_t	oui[3];
	u16_t	alter;
	u32_t	reseved2;
	u32_t	branch;
}__attribute__((packed));
typedef struct huawei_header2 huawei_header2_t;

struct huawei_mode_get{
	u16_t	lport;
	u32_t	reserved1;
	u32_t	reserved2;
}__attribute__((packed));
typedef struct huawei_mode_get huawei_mode_get_t;

struct huawei_mode_out{
	u16_t	reserved1;
	u32_t	lport;
	u32_t	reserved2;
	u32_t	duplex;
	u32_t 	speed;
	u64_t	reserved3;
	u64_t	reserved4;
	u32_t	reserved5;
	u32_t	reserved6;
	u32_t	reserved7;
	u32_t	reserved8;
	u64_t	reserved9;
	u64_t	reserved10;
	u32_t	reserved11;
	u32_t 	reserved12;
	u32_t 	reserved13;
	u16_t 	reserved14;
}__attribute__((packed));
typedef struct huawei_mode_out huawei_mode_out_t;

struct huawei_mode_set{
	u16_t	port;
	u32_t	speed_1000M;
	u32_t 	speed;
	u32_t	duplex;
	u64_t	reserved3;
	u64_t	reserved4;
	u32_t	reserved5;
	u32_t	reserved6;
	u32_t	reserved7;
	u32_t	reserved8;
	u64_t	reserved9;
	u64_t	reserved10;
	u32_t	reserved11;
	u32_t 	reserved12;
	u32_t 	reserved13;
	u16_t 	reserved14;
}__attribute__((packed));
typedef struct huawei_mode_set huawei_mode_set_t;

struct share_header {
	u8_t	oui[3];
	u8_t	branch;
}__attribute__((packed));
typedef struct share_header share_header_t;
extern int huawei_oam_init(void);
extern int huawei_yotc_oam_init(void);
extern int oam_huawei_ip_mgmt_vlan_process(
    unsigned char llidIdx, /* LLID index of the incoming operation */
    unsigned char op, /* Operation to be taken */
    ctc_varInstant_t *pVarInstant, /* It might be NULL if no instant specified */
    ctc_varDescriptor_t varDesc,
    unsigned short *pVarContainer_Total_Len,
    ctc_varContainer_t *pVarContainer);

//extern void oam_huawei_ip_mgmt_vlan_process(u8_t* p_in);
//extern void eopl_huawei_pdu_process2(u8_t *p_byte,u16_t length);
//extern void eopl_huawei_pdu_process(u8_t *p_byte,u16_t length);

#endif
