/******************************************************************************
*    COPYRIGHT (C) 2003-2007 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*  This software is made available only to customers and prospective
*  customers of Opulan Technologies Corporation under license and may be
*  used only with Opulan semi-conductor products.
*
* FILENAME:
*	zte.h
*
* DESCRIPTION:
*	This file implements the structures and macros for zte module.
*
******************************************************************************/

#ifndef __ZTE_H__
#define __ZTE_H__

/* definition for ZTE OAM extension */
#if 0
#ifndef OAM_SUPPORT_ZTE_EXT
#define OAM_SUPPORT_ZTE_EXT 1
#endif
#endif

#include "ctc.h"
#include "oam.h"
#include "opl_driver.h"

/* definition for ZTE OAM extension */
#define OAM_ZTE_EXT_OPCODE_GET_REQUEST    0x01
#define OAM_ZTE_EXT_OPCODE_GET_RESPONSE   0x02
#define OAM_ZTE_EXT_OPCODE_SET_REQUEST    0x03
#define OAM_ZTE_EXT_OPCODE_SET_RESPONSE   0x04

#define OAM_ZTE_EXT_TYPE_GENERAL          0x00
#define OAM_ZTE_EXT_TYPE_FE               0x01
#define OAM_ZTE_EXT_TYPE_VOIP             0x02
#define OAM_ZTE_EXT_TYPE_CATV             0x03

#define OAM_ZTE_EXT_BRANCH_ATTR           0x0F
#define OAM_ZTE_ZTE_OUI_EXT_BRANCH_ATTR   0x10


/* ZTE Private extension */
#define OAM_ZTE_EXT_LEAF_ONU_SUPPORT             0x0101 /* get */
#define OAM_ZTE_EXT_LEAF_ONU_SN             0x0104 /* get */

#define OAM_ZTE_EXT_LEAF_CONFIG_PARAM     0x0106   /*set*/
#define OAM_ZTE_EXT_LEAF_MGMT_IP_CFG        0x0107 /* set */
#define OAM_ZTE_EXT_LEAF_TYPE_C_SET         0x0109 /* set */
#define OAM_ZTE_EXT_LEAF_TYPE_C_GET         0x010a /* get */
#define OAM_ZTE_EXT_LEAF_SET_PORT_MODE      0x0204 /* set */
#define OAM_ZTE_EXT_LEAF_GET_PORT_MODE      0x0205 /* get */

/* ZTE CTC extension */
#define OAM_ZTE_EXT_LEAF_ALARM_INFO         0x8000 /* auto report */

#define OAM_ZTE_EXT_LEAF_ALARM_CFG          0x8001 /* get & set */
#define OAM_ZTE_EXT_LEAF_UPGRADE_PARA       0x8002 /* get & set */
#define OAM_ZTE_EXT_LEAF_MAC_ADDR_LIMIT     0x8003 /* get & set */
#define OAM_ZTE_EXT_LEAF_MAC_ADDR_AGING     0x8004 /* get & set */
#define OAM_ZTE_EXT_LEAF_PORT_MAC_FILTER    0x8005 /* get & set */
#define OAM_ZTE_EXT_LEAF_PORT_MAC_BIND      0x8006 /* get & set */
#define OAM_ZTE_EXT_LEAF_STATIC_MAC_CFG     0x8007 /* get & set */
#define OAM_ZTE_EXT_LEAF_UNI_ISOLATION      0x8008 /* get & set */
#define OAM_ZTE_EXT_LEAF_ONU_ETHER_STATS    0x8010 /* get */
#define OAM_ZTE_EXT_LEAF_PORT_MODE          0x8011 /* get & set */
#define OAM_ZTE_EXT_LEAF_UNI_MAC_QUERY      0x8012 /* get */
#define OAM_ZTE_EXT_LEAF_PMAC_DS_SHAPING    0x8013 /* get & set */
#define OAM_ZTE_EXT_LEAF_PMAC_BUFF_CFG      0x8014 /* get & set */
#define OAM_ZTE_EXT_LEAF_PMAC_BUFF_QUERY    0x8015 /* get */
#define OAM_ZTE_EXT_LEAF_UNI_STATS_CFG      0x8016 /* get & set */
#define OAM_ZTE_EXT_LEAF_UNI_STATS_RESET    0x8017 /* set */
#define OAM_ZTE_EXT_LEAF_UNI_STATS_QUERY    0x8018 /* get */

#define OAM_ZTE_EXT_LEAF_TX_LASER_ALWAYS_ON 0x8019 
#define OAM_ZTE_EXT_LEAF_TX_LASER_CONTROL   0x801A  
#define OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_1  0x801B
#define OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_2  0x801C

#define OAM_ZTE_EXT_LEAF_HG_REPORT          0x6000 /* auto */
#define OAM_ZTE_EXT_LEAF_HG_MAC_INFO        0x6001 /* get & set */
#define OAM_ZTE_EXT_LEAF_HG_MGMT_VLAN       0x6002 /* get & set */
#define OAM_ZTE_EXT_LEAF_HG_ACCESS_INFO     0x6003 /* get & set */

#define OAM_ZTE_EXT_LEAF_IAD_INFO           0x0061 /* get */
#define OAM_ZTE_EXT_LEAF_VOIP_INFO          0x0062 /* get */
#define OAM_ZTE_EXT_LEAF_GBL_PARA           0x0063 /* get & set */
#define OAM_ZTE_EXT_LEAF_H248_PARA          0x0064 /* get & set */
#define OAM_ZTE_EXT_LEAF_H248_USER_TID_SET  0x0065 /* set */
#define OAM_ZTE_EXT_LEAF_H248_USER_TID_GET  0x0066 /* get */
#define OAM_ZTE_EXT_LEAF_H248_RTP_TID_CFG   0x0067 /* set */
#define OAM_ZTE_EXT_LEAF_H248_RTP_TID_INFO  0x0068 /* get */
#define OAM_ZTE_EXT_LEAF_SIP_PARA           0x0069 /* get & set */
#define OAM_ZTE_EXT_LEAF_SIP_USER_PARA      0x006a /* get & set */
#define OAM_ZTE_EXT_LEAF_FAX_CFG            0x006b /* get & set */
#define OAM_ZTE_EXT_LEAF_IAD_OPER_STAT      0x006c /* get */
#define OAM_ZTE_EXT_LEAF_USER_POTS_IF_STAT  0x006d /* get */
#define OAM_ZTE_EXT_LEAF_IAD_RESET          0x006e /* set */
#define OAM_ZTE_EXT_LEAF_SIP_PORT_RESET     0x006f /* set */
#define OAM_ZTE_EXT_LEAF_ONU_CARD_INFO      0x0070 /* get */
#define OAM_ZTE_EXT_LEAF_VOIP_OTHER_CFG       0x8080 /* get & set */
#define OAM_ZTE_EXT_LEAF_VOIP_L3_TOS       0x8081 /* get & set */
#define OAM_ZTE_EXT_LEAF_VOIP_CALLER_ID       0x8082 /* get & set */
#define OAM_ZTE_EXT_LEAF_VOIP_PRIVATE_SIP_SDP       0x8083 /* get & set */
#define OAM_ZTE_EXT_LEAF_VOIP_SIPDIGITALMAP       0x8084 /* set */
#define OAM_ZTE_EXT_LEAF_VOIP_TIMEER_CONFIG      0x8085 /* get & set */
#define OAM_ZTE_EXT_LEAF_VOIP_SIP_USERPARAM_CONFIG_1      0x8086 /* get & set */

#define OAM_ZTE_POTS_PORT_ID_MIN 0x50
#define OAM_ZTE_POTS_PORT_ID_MAX 0x51

struct zte_header {
	u8_t oui[3];
	u8_t ext_opcode;
	u8_t type;
	u8_t port;
}__attribute__((packed));
typedef struct zte_header zte_header_t;

/* ZTE Private extension */

//OAM_ZTE_EXT_LEAF_ONU_SOUPORT            0x0101 /* get */
struct oam_zte_onu_souport {
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    is_souport;/* 1 souprot; 2 not souport*/
}__attribute__((packed));
typedef struct oam_zte_onu_souport oam_zte_onu_souport_t;


//OAM_ZTE_EXT_LEAF_ONU_SN             0x0104 /* get */
struct oam_zte_onu_sn {
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    sn[128];
}__attribute__((packed));
typedef struct oam_zte_onu_sn oam_zte_onu_sn_t;

//OAM_ZTE_EXT_LEAF_CONFIG_PAMRAM             0x0106 /* set */
struct oam_zte_onu_config_param {
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    action;
}__attribute__((packed));
typedef struct oam_zte_onu_config_param oam_zte_onu_config_param_t;

//OAM_ZTE_EXT_LEAF_MGMT_IP_CFG        0x0107 /* set */
struct oam_zte_mgmt_ip_cfg {
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
typedef struct oam_zte_mgmt_ip_cfg oam_zte_mgmt_ip_cfg_t;

//OAM_ZTE_EXT_LEAF_TYPE_C_SET         0x0109 /* set */
struct oam_zte_type_c {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t    k1;
	u8_t    k2;
}__attribute__((packed));
typedef struct oam_zte_type_c oam_zte_type_c_t;

/* ZTE CTC extension */
//OAM_ZTE_EXT_LEAF_ALARM_INFO         0x8000 /* auto report */
struct oam_zte_alarm_info{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u16_t   alarm_id;
	u8_t    reserved[3];
}__attribute__((packed));
typedef struct oam_zte_alarm_info oam_zte_alarm_info_t;

//OAM_ZTE_EXT_LEAF_ALARM_CFG          0x8001 /* get & set */
struct oam_zte_alarm_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    alarm_config;    /* 控制ONU的告警上报功能的开关，0x00-关闭， 0x01-打开 */
}__attribute__((packed));
typedef struct oam_zte_alarm_cfg oam_zte_alarm_cfg_t;

//OAM_ZTE_EXT_LEAF_UPGRADE_PARA       0x8002 /* get & set */
struct oam_zte_upgrade_para{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   ip_addr;        /* 服务器IP地址 */
	u32_t   ip_mask;        /* 服务器掩码 */
	u32_t   gateway;        /* 服务器网关地址 */
	u16_t   vlanid;         /* VLAN ID */
	u8_t    username[8];    /* 服务器 user name */
	u8_t    password[8];    /* 服务器 password */
	u8_t    action;         /* 0x00 update to flash, 0x01 update to linecard */
	u8_t    ver_num;        /* 待更新的版本文件数目 */
	u8_t    ver_name1[16];  /* 第一个待更新的版本文件名 */
	u8_t    ver_name2[16];  /* 第二个待更新的版本文件名 */
	u8_t    ver_name3[16];  /* 第三个待更新的版本文件名 */
}__attribute__((packed));
typedef struct oam_zte_upgrade_para oam_zte_upgrade_para_t;

//OAM_ZTE_EXT_LEAF_MAC_ADDR_LIMIT     0x8003 /* get & set */
struct oam_zte_mac_limit{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u16_t   limit_num;
}__attribute__((packed));
typedef struct oam_zte_mac_limit oam_zte_mac_limit_t;

//OAM_ZTE_EXT_LEAF_MAC_ADDR_AGING     0x8004 /* get & set */
struct oam_zte_mac_aging{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   aging_time;
}__attribute__((packed));
typedef struct oam_zte_mac_aging oam_zte_mac_aging_t;

//OAM_ZTE_EXT_LEAF_PORT_MAC_FILTER    0x8005 /* get & set */
struct oam_zte_port_mac_filter{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    action;
	u8_t    num_of_entries;
}__attribute__((packed));
typedef struct oam_zte_port_mac_filter oam_zte_port_mac_filter_t;

struct oam_zte_port_mac_filter_entry{
	u16_t  vlan_id;
	u8_t   mac_addr[6];
}__attribute__((packed));
typedef struct oam_zte_port_mac_filter_entry oam_zte_port_mac_filter_entry_t;

//OAM_ZTE_EXT_LEAF_PORT_MAC_BIND      0x8006 /* get & set */
struct oam_zte_port_mac_binding{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    action;
	u8_t    num_of_entries;
}__attribute__((packed));
typedef struct oam_zte_port_mac_binding oam_zte_port_mac_binding_t;

struct oam_zte_port_mac_binding_entry{
	u16_t  vlan_id;
	u8_t   mac_addr[6];
}__attribute__((packed));
typedef struct oam_zte_port_mac_binding_entry oam_zte_port_mac_binding_entry_t;

//OAM_ZTE_EXT_LEAF_STATIC_MAC_CFG     0x8007 /* get & set */
struct oam_zte_static_mac_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    action;
	u8_t    num_of_entries;
}__attribute__((packed));
typedef struct oam_zte_static_mac_cfg oam_zte_static_mac_cfg_t;

struct oam_zte_static_mac_entry{
	u16_t  vlan_id;
	u8_t   mac_addr[6];
}__attribute__((packed));
typedef struct oam_zte_static_mac_entry oam_zte_static_mac_entry_t;

//OAM_ZTE_EXT_LEAF_UNI_ISOLATION      0x8008 /* get & set */
struct oam_zte_uni_isolation{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    isolation; /* 0x00: not isolation, 0x01: isolation(default) */
}__attribute__((packed));
typedef struct oam_zte_uni_isolation oam_zte_uni_isolation_t;

//OAM_ZTE_EXT_LEAF_ONU_ETHER_STATS    0x8010 /* get */
struct oam_zte_onu_ether_stats{
	u8_t     branch;
	u16_t    leaf;
	u8_t     width;
	u64_t    ifInOctets;       /* 收到的上行包字节数 */
	u64_t    ifInUcastPkts;    /* 收到的上行单播包数目 */
	u64_t    ifInNUcastPkts;   /* 收到的上行组播包数目 */
	u64_t    ifInDiscards;     /* 丢弃的上行包数目 */
	u64_t    ifInErrors;       /* 收到的上行错误包数目  */
	u64_t    ifOutOctets;      /* 收到的下行包字节数  */
	u64_t    ifOutUcastPkts;   /* 收到的下行单播包数目  */
	u64_t    ifOutNUcastPkts;  /* 收到的下行组播包数目  */
	u64_t    ifOutDiscards;    /* 丢弃的下行包数目  */
	u64_t    ifOutErrors;      /* 收到的下行包数目  */
	u64_t    reserved1;
	u64_t    reserved2;
	u64_t    reserved3;
	u64_t    reserved4;
}__attribute__((packed));
typedef struct oam_zte_onu_ether_stats oam_zte_onu_ether_stats_t;

//OAM_ZTE_EXT_LEAF_PORT_MODE          0x8011 /* get & set */
struct oam_zte_port_mode{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    port_mode;  /* 0x00: Auto
	                       0x01: 10M half
	                       0x02: 10M full
	                       0x03: 100M half
	                       0x04: 100M full
	                       0x05: 1000M
	                       0x06: 10G */
}__attribute__((packed));
typedef struct oam_zte_port_mode oam_zte_port_mode_t;

//OAM_ZTE_EXT_LEAF_UNI_MAC_QUERY      0x8012 /* get */
struct oam_zte_uni_mac_query{
	u8_t    branch;
	u16_t   leaf;
	u8_t     op_code;
	u16_t    width;
	u16_t   random_value;
	u16_t   seq_no;
	u16_t   total_num;
}__attribute__((packed));
typedef struct oam_zte_uni_mac_query oam_zte_uni_mac_query_t;

struct oam_zte_uni_mac_query_entry{
	u8_t    mac_addr[6];
}__attribute__((packed));
typedef struct oam_zte_uni_mac_query_entry oam_zte_uni_mac_query_entry_t;

struct oam_zte_uni_mac_query_block{
	u32_t   addr_num;
	u32_t   addr_type;
	oam_zte_uni_mac_query_entry_t mac_value[20];
}__attribute__((packed));
typedef struct oam_zte_uni_mac_query_block oam_zte_uni_mac_query_block_t;



/* OAM_ZTE_EXT_LEAF_UNI_MAC_QUERY (not support) */


//OAM_ZTE_EXT_LEAF_PMAC_DS_SHAPING    0x8013 /* get & set */
struct oam_zte_pmac_ds_shaping{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    ds_rate_oper;  /* 0x00-disabled(default), 0x01-enabled */
	u8_t    ds_rate_cir[3];
	u8_t    ds_rate_cbs[3];
}__attribute__((packed));
typedef struct oam_zte_pmac_ds_shaping oam_zte_pmac_ds_shaping_t;

//OAM_ZTE_EXT_LEAF_PMAC_BUFF_CFG      0x8014 /* get & set */
struct oam_zte_pmac_buff_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    mgmt_indication; /* 0x00-disabled(default), 0x01-enabled */
	u8_t    direction; /* 0x00-upstream, 0x01 downstream, 0x02-both */
	u8_t    buff_size[3];
}__attribute__((packed));
typedef struct oam_zte_pmac_buff_cfg oam_zte_pmac_buff_cfg_t;

//OAM_ZTE_EXT_LEAF_PMAC_BUFF_QUERY    0x8015 /* get */
struct oam_zte_pmac_buff_query{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    buff_mgmt_capability; /* 0x00 not support, 0x01 support */
	u8_t    ds_buff_min[3];
	u8_t    ds_buff_max[3];
	u8_t    us_buff_min[3];
	u8_t    us_buff_max[3];
}__attribute__((packed));
typedef struct oam_zte_pmac_buff_query oam_zte_pmac_buff_query_t;

//OAM_ZTE_EXT_LEAF_UNI_STATS_CFG      0x8016 /* get & set */
struct oam_zte_uni_stats_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    action;            /* 0x00-stop collection, 0x01-start collection */
	u8_t    stats_interval[3]; /* 性能数据采集时间间隔，单位ms, default 1000ms */
	u32_t   stats_duration;    /* 采集周期剩余时间，单位s, 默认900s */
}__attribute__((packed));
typedef struct oam_zte_uni_stats_cfg oam_zte_uni_stats_cfg_t;

//OAM_ZTE_EXT_LEAF_UNI_STATS_RESET    0x8017 /* set */
struct oam_zte_uni_stats_reset{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    reset; /* 0x00-reset */
}__attribute__((packed));
typedef struct oam_zte_uni_stats_reset oam_zte_uni_stats_reset_t;

//OAM_ZTE_EXT_LEAF_UNI_STATS_QUERY    0x8018 /* get */
struct oam_zte_uni_stats_query{
	u8_t     branch;
	u16_t    leaf;
	u8_t     width;
	u8_t    collect_indication; /* 0x00-stop, 0x01-start(duration不等于0) */
	u64_t    ifInOctets;       /* 收到的上行包字节数 */
	u64_t    ifInUcastPkts;    /* 收到的上行单播包数目 */
	u64_t    ifInNUcastPkts;   /* 收到的上行组播包数目 */
	u64_t    ifInDiscards;     /* 丢弃的上行包数目 */
	u64_t    ifInErrors;       /* 收到的上行错误包数目  */
	u64_t    ifOutOctets;      /* 收到的下行包字节数  */
	u64_t    ifOutUcastPkts;   /* 收到的下行单播包数目  */
	u64_t    ifOutNUcastPkts;  /* 收到的下行组播包数目  */
	u64_t    ifOutDiscards;    /* 丢弃的下行包数目  */
	u64_t    ifOutErrors;      /* 收到的下行包数目  */
}__attribute__((packed));
typedef struct oam_zte_uni_stats_query oam_zte_uni_stats_query_t;

//OAM_ZTE_EXT_LEAF_TX_LASER_ALWAYS_ON 0x8019 get/set
struct oam_zte_tx_laser_always_on{
	u8_t     branch;
	u16_t    leaf;
	u8_t     width;
	u8_t     pon_port_num;
	u8_t     main_pon_tx_always_en;
	u8_t   second_pon_tx_always_en;
}__attribute__((packed));
typedef struct oam_zte_tx_laser_always_on oam_zte_tx_laser_always_on_t;

//OAM_ZTE_EXT_LEAF_TX_LASER_CONTROL 0x801a get/set
struct oam_zte_tx_laser_control{
	u8_t     branch;
	u16_t    leaf;
	u8_t     width;
	u8_t     matchmac_en;
        u8_t     pon_mac_value[6];
	u8_t     action;
	u16_t   durtime;
}__attribute__((packed));
typedef struct oam_zte_tx_laser_control oam_zte_tx_laser_control_t;

//OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_1    0x801b /* get */
struct oam_zte_pon_stats_statics_1{
	u8_t     branch;
	u16_t    leaf;
	u8_t     width;
	u64_t    FramesTransmittedOK;       /* LLID端口发送帧计数*/
	u64_t    OctetsTransmittedOK;    /* LLID端口发送字节计数*/
	u64_t    MulticastFramesXmittedOK;   /* LLID端口发送组播帧计数*/
	u64_t    BroadcastFramesXmittedOK;     /* LLID端口发送广播包计数 */
	u64_t    PONPauseFramesTransmittedOk;       /* PON端口发送PAUSE帧计数*/
	u64_t    FramesReceivedOK;      /*LLID端口接收帧计数*/
	u64_t    OctetsReceivedOK;   /* LLID端口接收字节计数 */
	u64_t    MulticastFramesReceivedOK;  /* LLID端口接收组播帧计数*/
	u64_t    BroadcastFramesReceivedOK;    /* LLID端口接收广播包计数 */
	u64_t    PONPauseFramesReceivedOk;      /* PON端口接收PAUSE帧计数*/
	u64_t    SingleCollisionFrames;       /*单次帧碰撞计数*/
	u64_t    MultipleCollisionFrames;   /*多次帧碰撞计数*/ 	
	u64_t    AlignmentErrors;  /*对齐错误计数*/
	u64_t    FrameTooLongErrors;  /*帧超长计数*/
}__attribute__((packed));
typedef struct oam_zte_pon_stats_statics_1 oam_zte_pon_stats_statics_1_t;

//OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_2    0x801c /* get */
struct oam_zte_pon_stats_statics_2{
	u8_t     branch;
	u16_t    leaf;
	u8_t     width;
	u64_t    CRC8Errors;       /* CRC错误计数 */
	u64_t    FECCorrectedBlocks;    /* FEC正确块计数 */
	u64_t    FECUncorrectableBlocks;   /* FEC错误块计数 */
	u64_t    MPCPMACCtrlFramesTransmitted;     /* MPCP控制帧发送计数*/
	u64_t    MPCPMACCtrlFramesReceived;       /* MPCP控制帧接收计数*/
	u64_t    MPCPTxGate;      /* MPCP Tx Gate计数*/
	u64_t    MPCPTxRegAck;   /* MPCP Tx Reg Ack计数*/
	u64_t    MPCPTxRegister;  /* MPCP Tx Register计数*/
	u64_t    MPCPTxRegRequest;    /* MPCP Tx Registe Requestr计数  */
	u64_t    MPCPTxReport;      /* MPCP Tx Report计数 */
	u64_t    MPCPRxGate;  /*MPCP Rx Gate计数*/
	u64_t    MPCPRxRegAck;    /*MPCP Rx Reg Ack计数*/
	u64_t    MPCPRxRegister;  /*MPCP Rx Register计数*/
	u64_t    MPCPRxRegRequest;    /*MPCP Rx RegRequest计数*/
	u64_t    MPCPRxReport;    /*MPCP Rx Report计数*/	
}__attribute__((packed));
typedef struct oam_zte_pon_stats_statics_2 oam_zte_pon_stats_statics_2_t;

//OAM_ZTE_EXT_LEAF_HG_REPORT          0x6000 /* auto report */
struct oam_zte_hg_report{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u16_t   msg_id;          /* 消息编码 */
	u8_t    msg_info[120];   /* 消息携带的内容 MAC@Data_Vlan@IPTV_VLAN@VOIP_VLAN  */
}__attribute__((packed));
typedef struct oam_zte_hg_report oam_zte_hg_report_t;


//OAM_ZTE_EXT_LEAF_HG_MAC_INFO        0x6001 /* get & set */
struct oam_zte_hg_mac_info{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    action;
	u8_t    num_of_entries;
}__attribute__((packed));
typedef struct oam_zte_hg_mac_info oam_zte_hg_mac_info_t;

struct oam_zte_hg_mac_entry{
	u8_t    mac_addr[6];    /* 家庭网关MAC特征码 */
}__attribute__((packed));
typedef struct oam_zte_hg_mac_entry oam_zte_hg_mac_entry_t;

//OAM_ZTE_EXT_LEAF_HG_MGMT_VLAN       0x6002 /* get & set */
struct oam_zte_hg_mgmt_vlan{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u16_t   hg_mgmt_vlan;   /* 家庭网关网管VLAN */
}__attribute__((packed));
typedef struct oam_zte_hg_mgmt_vlan oam_zte_hg_mgmt_vlan_t;

//OAM_ZTE_EXT_LEAF_HG_ACCESS_INFO     0x6003 /* get & set */
struct oam_zte_hg_access_info{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    hg_mac[6];
	u8_t    hg_report_state;  /* 家庭网关入网标志 */
}__attribute__((packed));
typedef struct oam_zte_hg_access_info oam_zte_hg_access_info_t;

//OAM_ZTE_EXT_LEAF_IAD_INFO           0x0061 /* get */
struct oam_zte_iad_info{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    mac_addr[6];        /* IAD的MAC地址  */
	u8_t    protocol_supported; /* 支持的语音协议, bit1(MGCP) bit2(H248) bit0(SIP) */
	u8_t    software_ver[32];   /* 软件版本 */
	u8_t    software_time[32];  /* 软件版本时间  */
	u8_t    voip_user_count;    /* 支持的用户数  */
}__attribute__((packed));
typedef struct oam_zte_iad_info oam_zte_iad_info_t;

//OAM_ZTE_EXT_LEAF_VOIP_INFO          0x0062 /* get */
struct oam_zte_voip_info{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   protocol_cfg;       /* 语音协议类型
	                               0x00000001 SIP
	                               0x00000002 H248
	                               0x00000003 MGCP */
}__attribute__((packed));
typedef struct oam_zte_voip_info oam_zte_voip_info_t;

//OAM_ZTE_EXT_LEAF_GBL_PARA           0x0063 /* get & set */
struct oam_zte_gbl_para{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    voice_ip_releation;  /* 语音IP与管理IP的关系，0-独立模式 1-共享模式  */
	u8_t    voice_ip_mode;       /* 地址配置模式，0-静态 1-DHCP 2-PPPOE/PPPOE+  */
	u32_t   iad_ip_address;      /* IP地址  */
    u32_t   iad_net_mask;        /* 地址掩码  */
	u32_t   iad_default_gw;      /* 默认网关  */
	u32_t   primary_dns;         /* 主用DNS  */
	u8_t    pppoe_mode;          /* PPPOE方式 0-Auto 1-CHAP 2-PAP  */
	u8_t    pppoe_username[32];  /* PPPOE用户民  */
	u8_t    pppoe_password[32];  /* PPPOE密码  */
	u8_t    tagged_flag;         /* 语音数据标记方式 0-透传，1-tag, 2-vlan stacking  */
	u16_t   voice_data_cvlan;    /* 语音数据cvlan  */
	u8_t    voice_data_pri;      /* 语音数据priority  */
	u16_t   voice_data_svlan;    /* 语音数据svlan  */
}__attribute__((packed));
typedef struct oam_zte_gbl_para oam_zte_gbl_para_t;

//OAM_ZTE_EXT_LEAF_H248_PARA          0x0064 /* get & set */
struct oam_zte_h248_para{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   reg_serv_ip;          /* 注册服务器的IP地址 */
	u16_t   reg_serv_port_no;     /* 注册服务器的端口号 */
	u32_t   bak_reg_serv_ip;      /* 备份注册服务器的IP地址  */
	u16_t   bak_reg_serv_port_no; /* 备份注册服务器的端口号   */
	u8_t    rtp_link_kept_flag;   /* RTP链路检测开关 0-关闭 1-启用 */
	u8_t    test_link_kept_mode;  /* 链路检测模式 0-关闭 1-H248 service change 2-H248 IT/ITO Notify 3-MGCP */
	u16_t   heartbeat_cycle;      /* 心跳周期   */
	u8_t    heartbeat_count;      /* 心跳检测次数   */
	u8_t    h248_reg_mode;        /* 注册方式   */
	u8_t    mid[64];              /* MG域名   */
	u16_t   mgp_port_no;          /* MG端口号 */
}__attribute__((packed));
typedef struct oam_zte_h248_para oam_zte_h248_para_t;

//OAM_ZTE_EXT_LEAF_H248_USER_TID_SET  0x0065 /* set */
struct oam_zte_h248_user_tid_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    num_of_user_tid_group;
}__attribute__((packed));
typedef struct oam_zte_h248_user_tid_cfg oam_zte_h248_user_tid_cfg_t;

struct oam_zte_h248_user_tid_grp{
	u8_t    first_tid_index;     /* 起始序号 */
	u8_t    num_of_tid;          /* User Tid个数 */
	u8_t    tid_prefix[16];      /* User Tid前缀 */
	u8_t    tid_digit_begin[8];  /* User Tid数字部分起始数值 */
	u8_t    tid_mode;            /* User Tid数字部分对齐方式 */
	u8_t    tid_digit_len;       /* User Tid数字部分的位数 */
}__attribute__((packed));
typedef struct oam_zte_h248_user_tid_grp oam_zte_h248_user_tid_grp_t;

//OAM_ZTE_EXT_LEAF_H248_USER_TID_GET  0x0066 /* get */
struct oam_zte_h248_user_tid_name{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    user_tid_name[32];
}__attribute__((packed));
typedef struct oam_zte_h248_user_tid_name oam_zte_h248_user_tid_name_t;

//OAM_ZTE_EXT_LEAF_H248_RTP_TID_CFG   0x0067 /* set */
struct oam_zte_h248_rtp_tid_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    rtp_tid_prefix[16];      /* RTP Tid前缀 */
	u8_t    rtp_tid_digit_begin[8];  /* RTP Tid数字部分起始数值 */
	u8_t    rtp_tid_mode;            /* RTP Tid数字部分对齐方式 */
	u8_t    rtp_tid_digit_len;       /* RTP Tid数字部分的位数 */
}__attribute__((packed));
typedef struct oam_zte_h248_rtp_tid_cfg oam_zte_h248_rtp_tid_cfg_t;

//OAM_ZTE_EXT_LEAF_H248_RTP_TID_INFO   0x0068 /* get */
struct oam_zte_h248_rtp_tid_info{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    num_of_rtp_tid;
	u8_t    first_rtp_tid_name[32];
}__attribute__((packed));
typedef struct oam_zte_h248_rtp_tid_info oam_zte_h248_rtp_tid_info_t;

//OAM_ZTE_EXT_LEAF_SIP_PARA           0x0069 /* get & set */
struct oam_zte_sip_para{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u16_t   mg_port_no;             /* MG端口号 */
	u32_t   reg_serv_ip;            /* 注册服务器的IP地址 */
	u16_t   reg_serv_port_no;       /* 注册服务器的端口号 */
	u32_t   back_reg_serv_ip;       /* 备份注册服务器的IP地址 */
	u16_t   back_reg_serv_port_no;  /* 备份注册服务器的端口号 */
	u32_t   proxy_serv_ip;          /* 代理服务器的IP地址 */
	u16_t   proxy_serv_port_no;     /* 代理服务器的端口号 */
	u32_t   bak_proxy_serv_ip;      /* 备份代理服务器的IP地址 */
	u16_t   bak_proxy_serv_port_no; /* 备份代理服务器的端口号 */
	u32_t   out_bound_serv_ip;      /* 外发服务器的IP地址 */
	u16_t   out_bound_serv_port_no; /* 外发服务器的端口号 */
	u32_t   reg_interval;           /* 注册刷新周期，单位秒 */
	u8_t    heart_beat_switch;      /* 心跳开关，0-关闭 1-打开 */
	u16_t   heart_beat_cycle;       /* 心跳周期，默认20s */
	u16_t   heart_beat_count;       /* 心跳超时次数，默认3次 */
}__attribute__((packed));
typedef struct oam_zte_sip_para oam_zte_sip_para_t;

//OAM_ZTE_EXT_LEAF_SIP_USER_PARA      0x006a /* get & set */
struct oam_zte_sip_user_para{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    user_account[10];     /* 用户电话号码  */
	u8_t    user_name[32];        /* 用户名  */
	u8_t    user_passwd[16];      /* 密码  */
}__attribute__((packed));
typedef struct oam_zte_sip_user_para oam_zte_sip_user_para_t;


//OAM_ZTE_EXT_LEAF_FAX_CFG            0x006b /* get & set */
struct oam_zte_fax_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   voice_t38_enable;    /* 0: T30, 1: T38 */
	u32_t   voice_fax_control;   /* 0: 无控方式
	                                1: RTCP控制
	                                2: 软交换全控方式
	                                3: Auto VBD */
}__attribute__((packed));
typedef struct oam_zte_fax_cfg oam_zte_fax_cfg_t;

//OAM_ZTE_EXT_LEAF_IAD_OPER_STAT      0x006c /* get */
struct oam_zte_iad_oper_stat{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   iad_oper_status;    /* 1: 正在注册
	                               2: 注册成功
	                               3: IAD故障
	                               4: 注销
	                               5: rebooting
	                               255: 未知状态 */
}__attribute__((packed));
typedef struct oam_zte_iad_oper_stat oam_zte_iad_oper_stat_t;


//OAM_ZTE_EXT_LEAF_USER_POTS_IF_STAT  0x006d /* get */
struct oam_zte_user_pots_if_stat{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   iad_port_status;       /* 用户端口状态:
	                                  1: 端口正在注册
	                                  2: 端口空闲
	                                  3: 摘机
	                                  4: 正在拨号
	                                  5: 正在振铃
	                                  6: 正在回铃
	                                  7: 正在连接
	                                  8: 已连接
	                                  9: 正在释放连接
	                                  10:端口忙
	                                  11:端口注册失败
	                                  12:端口未激活 */
	u32_t   iad_port_service_type; /* 用户端口业务类型
	                                  1: 正常业务:
	                                  2: 内部半永久:
	                                  3: IP半永久
	                                  4: 没有配置业务*/
	u32_t   iad_port_service_stat; /* 用户端口业务状态
									  1: 本地终止业务
									  2: 对端终止业务
									  3: 自动终止业务
									  4: 业务正常*/
	u32_t   iad_port_codec_mode;   /* 本端口目前正在使用的编解码方式
	                                  0: G.711U
	                                  1: G.711A
	                                  2: G.723
	                                  3: G.729
	                                  4: G.726
	                                  5: T.38
	                                  6: No Connection
	                                  7: Others*/
}__attribute__((packed));
typedef struct oam_zte_user_pots_if_stat oam_zte_user_pots_if_stat_t;

//OAM_ZTE_EXT_LEAF_IAD_RESET          0x006e /* set */
struct oam_zte_iad_reset{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   iad_reg_or_unreg;      /* 语音模块操作类型
	                                  1: 注册
	                                  2: 注销
	                                  3: 复位
	                                  4: 恢复出厂配置*/
}__attribute__((packed));
typedef struct oam_zte_iad_reset oam_zte_iad_reset_t;

//OAM_ZTE_EXT_LEAF_SIP_PORT_RESET     0x006f /* set */
struct oam_zte_sip_port_reset{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   sip_port_reg_or_unreg;  /* SIP协议端口操作类型
                                       1: 注册
                                       2: 注销
	                                   */
}__attribute__((packed));
typedef struct oam_zte_sip_port_reset oam_zte_sip_port_reset_t;

//OAM_ZTE_EXT_LEAF_ONU_CARD_INFO      0x0070 /* get */
struct oam_zte_onu_card_info{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    total_voip_module_installed; /* 安装的VOIP模块总数 */
}__attribute__((packed));
typedef struct oam_zte_onu_card_info oam_zte_onu_card_info_t;

struct oam_zte_onu_card_entry{
	u8_t    slot_num;        /* VOIP模块的安装槽位号 */
	u8_t    user_count;      /* 支持的用户数 */
}__attribute__((packed));
typedef struct oam_zte_onu_card_entry oam_zte_onu_card_entry_t;

//OAM_ZTE_EXT_LEAF_OTHER_CONFIG           0x8080 /* get & set */
struct oam_zte_onu_other_config{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    noice_generation_ncg; /* 舒适噪音功能(暂不支持)*/
	u8_t    voice_detection_vad; /* 静音检测功能*/
	u8_t    echo_cancellation; /*回声抑制功能*/
	u16_t    sending_dtmf; /*DTMF传送方式*/
}__attribute__((packed));
typedef struct oam_zte_onu_other_config oam_zte_onu_other_config_t;

//OAM_ZTE_EXT_LEAF_VOIP_L3_TOS        0x8081 /* get & set */
struct oam_zte_onu_l3_tos{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    l3_tos_mode; /* 信令和语音L3 ToS标记是否分开*/
	u8_t    sigal_tos_dscp_labelling; /* 信令L3 ToS标记*/
	u8_t    voice_tos_dscp_labelling; /*语音L3 ToS标记*/
}__attribute__((packed));
typedef struct oam_zte_onu_l3_tos oam_zte_onu_l3_tos_t;

//OAM_ZTE_EXT_LEAF_CALLER_ID_PRESENT  0x8082 /* get & set */
struct oam_zte_onu_caller_id{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    presenting_caller_enable; /* 来电显示*/
	u8_t    cid_type; /*来电显示方式*/
}__attribute__((packed));
typedef struct oam_zte_onu_caller_id oam_zte_onu_caller_id_t;

//OAM_ZTE_EXT_LEAF_VOIP_SIP_SDP       0x8083 /* get & set */
struct oam_zte_onu_sip_sdp{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    sdp_codecs_precedence[20]; /* SDP Codec编解码匹配优先顺序*/
}__attribute__((packed));
typedef struct oam_zte_onu_sip_sdp oam_zte_onu_sip_sdp_t;

//OAM_ZTE_EXT_LEAF_VOIP_sipdigitalmap       0x8084 /* get & set */

struct oam_zte_onu_sipdigitalmap{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    num_sipdigitalmap; /* SIP协议数图的分块个数*/
}__attribute__((packed));
typedef struct oam_zte_onu_sipdigitalmap oam_zte_onu_sipdigitalmap_t;

struct oam_zte_onu_sipdigitalmap_block{
	u8_t    block_no;
	u8_t    sipdigitalmap_block[125]; /* SIP协议数图的分块*/
}__attribute__((packed));
typedef struct oam_zte_onu_sipdigitalmap_block oam_zte_onu_sipdigitalmap_block_t;

//OAM_ZTE_EXT_LEAF_VOIP_timer_config       0x8085 /* get & set */

struct oam_zte_onu_timer_config{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    dml; /* 收号长定时器*/
	u8_t    dms; /*收号短定时器*/
}__attribute__((packed));
typedef struct oam_zte_onu_timer_config oam_zte_onu_timer_config_t;

//OAM_ZTE_EXT_LEAF_SIP_USER_PARA_1      0x8086 /* get & set */
struct oam_zte_sip_user_para_1{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    user_account[16];     /* 用户电话号码  */
	u8_t    user_name[32];        /* 用户名  */
	u8_t    user_passwd[16];      /* 密码  */
}__attribute__((packed));
typedef struct oam_zte_sip_user_para_1 oam_zte_sip_user_para_1_t;

struct oam_zte_onu_capabilites2{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	onu_type;
	u8_t	multiLlid;
	u8_t    protection_type;
	u8_t    number_of_PONIf;
	u8_t    number_of_slot;
	u8_t 	number_of_interface_type;
	u32_t   fe_interface_type;
	u16_t   number_of_fe_port;
	u32_t   voip_interface_type;
	u16_t   number_of_voip_port;
	u8_t    battery_backup;
}__attribute__((packed));
typedef struct oam_zte_onu_capabilites2 oam_zte_onu_capabilites2_t;

void eopl_zte_pdu_process(u8_t *p_byte,u16_t length);

void eopl_zte_ctc_get_req(u16_t leaf, u8_t** pp_byte);

void eopl_zte_ctc_set_req(u16_t leaf, u8_t** pp_byte);

#endif

