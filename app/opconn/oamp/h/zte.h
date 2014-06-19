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
	u32_t   mgmt_host_subnet; /* Զ������·������ */
	u32_t   mgmt_host_mask;   /* Զ���������� */
	u32_t   mgmt_gateway;      /* ������ָ������� */
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
	u8_t    alarm_config;    /* ����ONU�ĸ澯�ϱ����ܵĿ��أ�0x00-�رգ� 0x01-�� */
}__attribute__((packed));
typedef struct oam_zte_alarm_cfg oam_zte_alarm_cfg_t;

//OAM_ZTE_EXT_LEAF_UPGRADE_PARA       0x8002 /* get & set */
struct oam_zte_upgrade_para{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   ip_addr;        /* ������IP��ַ */
	u32_t   ip_mask;        /* ���������� */
	u32_t   gateway;        /* ���������ص�ַ */
	u16_t   vlanid;         /* VLAN ID */
	u8_t    username[8];    /* ������ user name */
	u8_t    password[8];    /* ������ password */
	u8_t    action;         /* 0x00 update to flash, 0x01 update to linecard */
	u8_t    ver_num;        /* �����µİ汾�ļ���Ŀ */
	u8_t    ver_name1[16];  /* ��һ�������µİ汾�ļ��� */
	u8_t    ver_name2[16];  /* �ڶ��������µİ汾�ļ��� */
	u8_t    ver_name3[16];  /* �����������µİ汾�ļ��� */
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
	u64_t    ifInOctets;       /* �յ������а��ֽ��� */
	u64_t    ifInUcastPkts;    /* �յ������е�������Ŀ */
	u64_t    ifInNUcastPkts;   /* �յ��������鲥����Ŀ */
	u64_t    ifInDiscards;     /* ���������а���Ŀ */
	u64_t    ifInErrors;       /* �յ������д������Ŀ  */
	u64_t    ifOutOctets;      /* �յ������а��ֽ���  */
	u64_t    ifOutUcastPkts;   /* �յ������е�������Ŀ  */
	u64_t    ifOutNUcastPkts;  /* �յ��������鲥����Ŀ  */
	u64_t    ifOutDiscards;    /* ���������а���Ŀ  */
	u64_t    ifOutErrors;      /* �յ������а���Ŀ  */
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
	u8_t    stats_interval[3]; /* �������ݲɼ�ʱ��������λms, default 1000ms */
	u32_t   stats_duration;    /* �ɼ�����ʣ��ʱ�䣬��λs, Ĭ��900s */
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
	u8_t    collect_indication; /* 0x00-stop, 0x01-start(duration������0) */
	u64_t    ifInOctets;       /* �յ������а��ֽ��� */
	u64_t    ifInUcastPkts;    /* �յ������е�������Ŀ */
	u64_t    ifInNUcastPkts;   /* �յ��������鲥����Ŀ */
	u64_t    ifInDiscards;     /* ���������а���Ŀ */
	u64_t    ifInErrors;       /* �յ������д������Ŀ  */
	u64_t    ifOutOctets;      /* �յ������а��ֽ���  */
	u64_t    ifOutUcastPkts;   /* �յ������е�������Ŀ  */
	u64_t    ifOutNUcastPkts;  /* �յ��������鲥����Ŀ  */
	u64_t    ifOutDiscards;    /* ���������а���Ŀ  */
	u64_t    ifOutErrors;      /* �յ������а���Ŀ  */
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
	u64_t    FramesTransmittedOK;       /* LLID�˿ڷ���֡����*/
	u64_t    OctetsTransmittedOK;    /* LLID�˿ڷ����ֽڼ���*/
	u64_t    MulticastFramesXmittedOK;   /* LLID�˿ڷ����鲥֡����*/
	u64_t    BroadcastFramesXmittedOK;     /* LLID�˿ڷ��͹㲥������ */
	u64_t    PONPauseFramesTransmittedOk;       /* PON�˿ڷ���PAUSE֡����*/
	u64_t    FramesReceivedOK;      /*LLID�˿ڽ���֡����*/
	u64_t    OctetsReceivedOK;   /* LLID�˿ڽ����ֽڼ��� */
	u64_t    MulticastFramesReceivedOK;  /* LLID�˿ڽ����鲥֡����*/
	u64_t    BroadcastFramesReceivedOK;    /* LLID�˿ڽ��չ㲥������ */
	u64_t    PONPauseFramesReceivedOk;      /* PON�˿ڽ���PAUSE֡����*/
	u64_t    SingleCollisionFrames;       /*����֡��ײ����*/
	u64_t    MultipleCollisionFrames;   /*���֡��ײ����*/ 	
	u64_t    AlignmentErrors;  /*����������*/
	u64_t    FrameTooLongErrors;  /*֡��������*/
}__attribute__((packed));
typedef struct oam_zte_pon_stats_statics_1 oam_zte_pon_stats_statics_1_t;

//OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_2    0x801c /* get */
struct oam_zte_pon_stats_statics_2{
	u8_t     branch;
	u16_t    leaf;
	u8_t     width;
	u64_t    CRC8Errors;       /* CRC������� */
	u64_t    FECCorrectedBlocks;    /* FEC��ȷ����� */
	u64_t    FECUncorrectableBlocks;   /* FEC�������� */
	u64_t    MPCPMACCtrlFramesTransmitted;     /* MPCP����֡���ͼ���*/
	u64_t    MPCPMACCtrlFramesReceived;       /* MPCP����֡���ռ���*/
	u64_t    MPCPTxGate;      /* MPCP Tx Gate����*/
	u64_t    MPCPTxRegAck;   /* MPCP Tx Reg Ack����*/
	u64_t    MPCPTxRegister;  /* MPCP Tx Register����*/
	u64_t    MPCPTxRegRequest;    /* MPCP Tx Registe Requestr����  */
	u64_t    MPCPTxReport;      /* MPCP Tx Report���� */
	u64_t    MPCPRxGate;  /*MPCP Rx Gate����*/
	u64_t    MPCPRxRegAck;    /*MPCP Rx Reg Ack����*/
	u64_t    MPCPRxRegister;  /*MPCP Rx Register����*/
	u64_t    MPCPRxRegRequest;    /*MPCP Rx RegRequest����*/
	u64_t    MPCPRxReport;    /*MPCP Rx Report����*/	
}__attribute__((packed));
typedef struct oam_zte_pon_stats_statics_2 oam_zte_pon_stats_statics_2_t;

//OAM_ZTE_EXT_LEAF_HG_REPORT          0x6000 /* auto report */
struct oam_zte_hg_report{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u16_t   msg_id;          /* ��Ϣ���� */
	u8_t    msg_info[120];   /* ��ϢЯ�������� MAC@Data_Vlan@IPTV_VLAN@VOIP_VLAN  */
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
	u8_t    mac_addr[6];    /* ��ͥ����MAC������ */
}__attribute__((packed));
typedef struct oam_zte_hg_mac_entry oam_zte_hg_mac_entry_t;

//OAM_ZTE_EXT_LEAF_HG_MGMT_VLAN       0x6002 /* get & set */
struct oam_zte_hg_mgmt_vlan{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u16_t   hg_mgmt_vlan;   /* ��ͥ��������VLAN */
}__attribute__((packed));
typedef struct oam_zte_hg_mgmt_vlan oam_zte_hg_mgmt_vlan_t;

//OAM_ZTE_EXT_LEAF_HG_ACCESS_INFO     0x6003 /* get & set */
struct oam_zte_hg_access_info{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    hg_mac[6];
	u8_t    hg_report_state;  /* ��ͥ����������־ */
}__attribute__((packed));
typedef struct oam_zte_hg_access_info oam_zte_hg_access_info_t;

//OAM_ZTE_EXT_LEAF_IAD_INFO           0x0061 /* get */
struct oam_zte_iad_info{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    mac_addr[6];        /* IAD��MAC��ַ  */
	u8_t    protocol_supported; /* ֧�ֵ�����Э��, bit1(MGCP) bit2(H248) bit0(SIP) */
	u8_t    software_ver[32];   /* ����汾 */
	u8_t    software_time[32];  /* ����汾ʱ��  */
	u8_t    voip_user_count;    /* ֧�ֵ��û���  */
}__attribute__((packed));
typedef struct oam_zte_iad_info oam_zte_iad_info_t;

//OAM_ZTE_EXT_LEAF_VOIP_INFO          0x0062 /* get */
struct oam_zte_voip_info{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   protocol_cfg;       /* ����Э������
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
	u8_t    voice_ip_releation;  /* ����IP�����IP�Ĺ�ϵ��0-����ģʽ 1-����ģʽ  */
	u8_t    voice_ip_mode;       /* ��ַ����ģʽ��0-��̬ 1-DHCP 2-PPPOE/PPPOE+  */
	u32_t   iad_ip_address;      /* IP��ַ  */
    u32_t   iad_net_mask;        /* ��ַ����  */
	u32_t   iad_default_gw;      /* Ĭ������  */
	u32_t   primary_dns;         /* ����DNS  */
	u8_t    pppoe_mode;          /* PPPOE��ʽ 0-Auto 1-CHAP 2-PAP  */
	u8_t    pppoe_username[32];  /* PPPOE�û���  */
	u8_t    pppoe_password[32];  /* PPPOE����  */
	u8_t    tagged_flag;         /* �������ݱ�Ƿ�ʽ 0-͸����1-tag, 2-vlan stacking  */
	u16_t   voice_data_cvlan;    /* ��������cvlan  */
	u8_t    voice_data_pri;      /* ��������priority  */
	u16_t   voice_data_svlan;    /* ��������svlan  */
}__attribute__((packed));
typedef struct oam_zte_gbl_para oam_zte_gbl_para_t;

//OAM_ZTE_EXT_LEAF_H248_PARA          0x0064 /* get & set */
struct oam_zte_h248_para{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   reg_serv_ip;          /* ע���������IP��ַ */
	u16_t   reg_serv_port_no;     /* ע��������Ķ˿ں� */
	u32_t   bak_reg_serv_ip;      /* ����ע���������IP��ַ  */
	u16_t   bak_reg_serv_port_no; /* ����ע��������Ķ˿ں�   */
	u8_t    rtp_link_kept_flag;   /* RTP��·��⿪�� 0-�ر� 1-���� */
	u8_t    test_link_kept_mode;  /* ��·���ģʽ 0-�ر� 1-H248 service change 2-H248 IT/ITO Notify 3-MGCP */
	u16_t   heartbeat_cycle;      /* ��������   */
	u8_t    heartbeat_count;      /* ����������   */
	u8_t    h248_reg_mode;        /* ע�᷽ʽ   */
	u8_t    mid[64];              /* MG����   */
	u16_t   mgp_port_no;          /* MG�˿ں� */
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
	u8_t    first_tid_index;     /* ��ʼ��� */
	u8_t    num_of_tid;          /* User Tid���� */
	u8_t    tid_prefix[16];      /* User Tidǰ׺ */
	u8_t    tid_digit_begin[8];  /* User Tid���ֲ�����ʼ��ֵ */
	u8_t    tid_mode;            /* User Tid���ֲ��ֶ��뷽ʽ */
	u8_t    tid_digit_len;       /* User Tid���ֲ��ֵ�λ�� */
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
	u8_t    rtp_tid_prefix[16];      /* RTP Tidǰ׺ */
	u8_t    rtp_tid_digit_begin[8];  /* RTP Tid���ֲ�����ʼ��ֵ */
	u8_t    rtp_tid_mode;            /* RTP Tid���ֲ��ֶ��뷽ʽ */
	u8_t    rtp_tid_digit_len;       /* RTP Tid���ֲ��ֵ�λ�� */
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
	u16_t   mg_port_no;             /* MG�˿ں� */
	u32_t   reg_serv_ip;            /* ע���������IP��ַ */
	u16_t   reg_serv_port_no;       /* ע��������Ķ˿ں� */
	u32_t   back_reg_serv_ip;       /* ����ע���������IP��ַ */
	u16_t   back_reg_serv_port_no;  /* ����ע��������Ķ˿ں� */
	u32_t   proxy_serv_ip;          /* �����������IP��ַ */
	u16_t   proxy_serv_port_no;     /* ����������Ķ˿ں� */
	u32_t   bak_proxy_serv_ip;      /* ���ݴ����������IP��ַ */
	u16_t   bak_proxy_serv_port_no; /* ���ݴ���������Ķ˿ں� */
	u32_t   out_bound_serv_ip;      /* �ⷢ��������IP��ַ */
	u16_t   out_bound_serv_port_no; /* �ⷢ�������Ķ˿ں� */
	u32_t   reg_interval;           /* ע��ˢ�����ڣ���λ�� */
	u8_t    heart_beat_switch;      /* �������أ�0-�ر� 1-�� */
	u16_t   heart_beat_cycle;       /* �������ڣ�Ĭ��20s */
	u16_t   heart_beat_count;       /* ������ʱ������Ĭ��3�� */
}__attribute__((packed));
typedef struct oam_zte_sip_para oam_zte_sip_para_t;

//OAM_ZTE_EXT_LEAF_SIP_USER_PARA      0x006a /* get & set */
struct oam_zte_sip_user_para{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    user_account[10];     /* �û��绰����  */
	u8_t    user_name[32];        /* �û���  */
	u8_t    user_passwd[16];      /* ����  */
}__attribute__((packed));
typedef struct oam_zte_sip_user_para oam_zte_sip_user_para_t;


//OAM_ZTE_EXT_LEAF_FAX_CFG            0x006b /* get & set */
struct oam_zte_fax_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   voice_t38_enable;    /* 0: T30, 1: T38 */
	u32_t   voice_fax_control;   /* 0: �޿ط�ʽ
	                                1: RTCP����
	                                2: ����ȫ�ط�ʽ
	                                3: Auto VBD */
}__attribute__((packed));
typedef struct oam_zte_fax_cfg oam_zte_fax_cfg_t;

//OAM_ZTE_EXT_LEAF_IAD_OPER_STAT      0x006c /* get */
struct oam_zte_iad_oper_stat{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   iad_oper_status;    /* 1: ����ע��
	                               2: ע��ɹ�
	                               3: IAD����
	                               4: ע��
	                               5: rebooting
	                               255: δ֪״̬ */
}__attribute__((packed));
typedef struct oam_zte_iad_oper_stat oam_zte_iad_oper_stat_t;


//OAM_ZTE_EXT_LEAF_USER_POTS_IF_STAT  0x006d /* get */
struct oam_zte_user_pots_if_stat{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   iad_port_status;       /* �û��˿�״̬:
	                                  1: �˿�����ע��
	                                  2: �˿ڿ���
	                                  3: ժ��
	                                  4: ���ڲ���
	                                  5: ��������
	                                  6: ���ڻ���
	                                  7: ��������
	                                  8: ������
	                                  9: �����ͷ�����
	                                  10:�˿�æ
	                                  11:�˿�ע��ʧ��
	                                  12:�˿�δ���� */
	u32_t   iad_port_service_type; /* �û��˿�ҵ������
	                                  1: ����ҵ��:
	                                  2: �ڲ�������:
	                                  3: IP������
	                                  4: û������ҵ��*/
	u32_t   iad_port_service_stat; /* �û��˿�ҵ��״̬
									  1: ������ֹҵ��
									  2: �Զ���ֹҵ��
									  3: �Զ���ֹҵ��
									  4: ҵ������*/
	u32_t   iad_port_codec_mode;   /* ���˿�Ŀǰ����ʹ�õı���뷽ʽ
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
	u32_t   iad_reg_or_unreg;      /* ����ģ���������
	                                  1: ע��
	                                  2: ע��
	                                  3: ��λ
	                                  4: �ָ���������*/
}__attribute__((packed));
typedef struct oam_zte_iad_reset oam_zte_iad_reset_t;

//OAM_ZTE_EXT_LEAF_SIP_PORT_RESET     0x006f /* set */
struct oam_zte_sip_port_reset{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   sip_port_reg_or_unreg;  /* SIPЭ��˿ڲ�������
                                       1: ע��
                                       2: ע��
	                                   */
}__attribute__((packed));
typedef struct oam_zte_sip_port_reset oam_zte_sip_port_reset_t;

//OAM_ZTE_EXT_LEAF_ONU_CARD_INFO      0x0070 /* get */
struct oam_zte_onu_card_info{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    total_voip_module_installed; /* ��װ��VOIPģ������ */
}__attribute__((packed));
typedef struct oam_zte_onu_card_info oam_zte_onu_card_info_t;

struct oam_zte_onu_card_entry{
	u8_t    slot_num;        /* VOIPģ��İ�װ��λ�� */
	u8_t    user_count;      /* ֧�ֵ��û��� */
}__attribute__((packed));
typedef struct oam_zte_onu_card_entry oam_zte_onu_card_entry_t;

//OAM_ZTE_EXT_LEAF_OTHER_CONFIG           0x8080 /* get & set */
struct oam_zte_onu_other_config{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    noice_generation_ncg; /* ������������(�ݲ�֧��)*/
	u8_t    voice_detection_vad; /* ������⹦��*/
	u8_t    echo_cancellation; /*�������ƹ���*/
	u16_t    sending_dtmf; /*DTMF���ͷ�ʽ*/
}__attribute__((packed));
typedef struct oam_zte_onu_other_config oam_zte_onu_other_config_t;

//OAM_ZTE_EXT_LEAF_VOIP_L3_TOS        0x8081 /* get & set */
struct oam_zte_onu_l3_tos{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    l3_tos_mode; /* ���������L3 ToS����Ƿ�ֿ�*/
	u8_t    sigal_tos_dscp_labelling; /* ����L3 ToS���*/
	u8_t    voice_tos_dscp_labelling; /*����L3 ToS���*/
}__attribute__((packed));
typedef struct oam_zte_onu_l3_tos oam_zte_onu_l3_tos_t;

//OAM_ZTE_EXT_LEAF_CALLER_ID_PRESENT  0x8082 /* get & set */
struct oam_zte_onu_caller_id{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    presenting_caller_enable; /* ������ʾ*/
	u8_t    cid_type; /*������ʾ��ʽ*/
}__attribute__((packed));
typedef struct oam_zte_onu_caller_id oam_zte_onu_caller_id_t;

//OAM_ZTE_EXT_LEAF_VOIP_SIP_SDP       0x8083 /* get & set */
struct oam_zte_onu_sip_sdp{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    sdp_codecs_precedence[20]; /* SDP Codec�����ƥ������˳��*/
}__attribute__((packed));
typedef struct oam_zte_onu_sip_sdp oam_zte_onu_sip_sdp_t;

//OAM_ZTE_EXT_LEAF_VOIP_sipdigitalmap       0x8084 /* get & set */

struct oam_zte_onu_sipdigitalmap{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    num_sipdigitalmap; /* SIPЭ����ͼ�ķֿ����*/
}__attribute__((packed));
typedef struct oam_zte_onu_sipdigitalmap oam_zte_onu_sipdigitalmap_t;

struct oam_zte_onu_sipdigitalmap_block{
	u8_t    block_no;
	u8_t    sipdigitalmap_block[125]; /* SIPЭ����ͼ�ķֿ�*/
}__attribute__((packed));
typedef struct oam_zte_onu_sipdigitalmap_block oam_zte_onu_sipdigitalmap_block_t;

//OAM_ZTE_EXT_LEAF_VOIP_timer_config       0x8085 /* get & set */

struct oam_zte_onu_timer_config{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    dml; /* �պų���ʱ��*/
	u8_t    dms; /*�պŶ̶�ʱ��*/
}__attribute__((packed));
typedef struct oam_zte_onu_timer_config oam_zte_onu_timer_config_t;

//OAM_ZTE_EXT_LEAF_SIP_USER_PARA_1      0x8086 /* get & set */
struct oam_zte_sip_user_para_1{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    user_account[16];     /* �û��绰����  */
	u8_t    user_name[32];        /* �û���  */
	u8_t    user_passwd[16];      /* ����  */
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

