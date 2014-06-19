/******************************************************************************
*    COPYRIGHT (C) 2003-2007 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*  This software is made available only to customers and prospective
*  customers of Opulan Technologies Corporation under license and may be
*  used only with Opulan semi-conductor products.
*
* FILENAME:
*	ctc.h
*
* DESCRIPTION:
*	This file implements the structures and macros for ctc module.
*
* Date Created:
*	2008-09-11
*
* Authors(optional):
*	Zhihua Lu
*
* Reviewed by (optional):
*
******************************************************************************/

#ifndef __CTC_H__
#define __CTC_H__

#include "oam.h"
/*add for alarm 2010.1.6*/
#include "oam_alarm.h"

#define	SUBVERSTR_LEN	16

extern int CfmAgent_SetPort(int port);
extern int CfmAgent_GetCfg(char *key, char *value, int len);
extern int CfmAgent_SetCfg(char *key, char *value);

extern void OamDbgSwitch (int iEndis);
extern void eopl_ctc_header(u8_t ext_code);
extern void eopl_get_onu_sn_rsp(void);
extern void eopl_get_onu_firmwarever(void);
extern void eopl_get_onu_chipid(void);
extern void eopl_get_onu_capabilities1(void);
extern void eopl_get_onu_eth_linkstate(void);
extern void eopl_get_onu_eth_portpause(void);
extern void eopl_get_onu_eth_port_us_policing(void);
extern void eopl_get_onu_eth_port_ds_policing(void);
extern void eopl_get_onu_eth_phyadmin(void);
extern void eopl_get_onu_eth_autonegadmin(void);
extern void eopl_get_onu_eth_autoneglocal(void);
extern void eopl_get_onu_eth_autonegadv(void);
extern void eopl_get_onu_eth_fecability(void);
extern void eopl_get_onu_eth_fecmode(void);
extern void eopl_get_onu_mcast_tagstrip(void);
extern void eopl_get_onu_mcast_switch(void);
extern void eopl_get_onu_mcast_grp_nummax(void);
extern void eopl_get_onu_mcast_fast_leave_abl(void);
extern void eopl_get_onu_mcast_fast_leave_admin(void);
extern void eopl_get_onu_mcast_vlan(void);
extern void eopl_get_onu_mcast_control(void);
extern void eopl_set_dba(u8_t ucQSetNum, oam_dba_queue_t *p_dba);
extern void eopl_get_dba(void);
extern bool_t eopl_get_tag_id(u16_t portid,u32_t *value);
extern bool_t eopl_get_vtt_list(u8_t portid,u8_t *p_byte,u8_t *num, u32_t uiDefVid);
extern int eopl_get_onu_vlan(void);
extern void eopl_get_onu_classify(void);
extern void eopl_ctc_get_req(u8_t* p_byte);
extern void eopl_set_onu_eth_port_pause(u8_t action);
extern void eopl_set_onu_eth_port_us_policing(u8_t operation,u8_t* p_cir,u8_t* p_cbs,u8_t* p_ebs);
extern void eopl_set_onu_eth_port_ds_policing(u8_t operation,u8_t* p_cir,u8_t* p_pir);
extern void eopl_set_onu_eth_admincontrol(u32_t action);
extern void eopl_set_onu_eth_atuorestart(void);
extern void eopl_set_onu_eth_autonegadmin(u8_t action);
extern void eopl_set_onu_eth_fecmode(u32_t mode);
extern void eopl_set_onu_mcast_group_max(u8_t max);
extern void eopl_set_onu_mcast_fast_leave_control(u8_t value);
extern void eopl_set_onu_mcast_control(u8_t *p_in);
extern void eopl_set_onu_mcast_switch(u8_t action);
extern void eopl_set_onu_mcast_tag_strip(u8_t *p_in);
extern void eopl_set_onu_mcast_vlan(u8_t *p_in);
extern int eopl_set_tag_id(u16_t portid,u32_t *value);
extern int eopl_set_onu_vlan(u8_t*  p_in);
extern void eopl_set_onu_classfy(u8_t*  p_in);
extern void eopl_ctc_set_req(u8_t* p_byte);
extern void eopl_req_new_key(u8_t index);
extern void eopl_req_tri_scb_key(u8_t index, u8_t *pucKey);
extern void eopl_ctc_churning(u8_t* p_byte);
extern void eopl_ctc_dba(u8_t* p_byte);
extern void eopl_req_aes_key(u8_t index, u8_t ucMode);
extern void eopl_req_aesscb_key(u8_t index, u8_t ucMode, u8_t *pucKey);
extern void eopl_aeskey_req(u8_t* p_byte);
extern void eopl_aesscbkey_req(u8_t* p_byte);
extern void eopl_TriScbkey_req(u8_t* p_byte);
extern void eopl_ctc_pdu_process(u8_t *p_byte,u16_t length);
extern int SwVersionGet(u8_t *pucVerStr);
extern void eopl_ctc_auth(u8_t *pucData);
extern int eopl_ctc_alarm_report(alarm_report_t *reportInfo);
//add by wuxiao 2010.1.6

extern int stub_get_onu_optical_transiver_temperature(u16_t *pTemperature);
extern int stub_get_onu_optical_transiver_supply_vcc(u16_t *pVcc);
extern int stub_get_onu_optical_transiver_tx_bias_current(u16_t *pCurrent);
extern int stub_get_onu_optical_transiver_tx_power(u16_t *pTxPower);
extern int stub_get_onu_optical_transiver_rx_power(u16_t *pRxPower);

extern int  IpcHalTxFaultStutasGet(int *iTxFault);
extern int eopl_oam_TxFault_report(int iEnable);
extern void eopl_ctc_TxFault_header(void );

#if defined(ONU_1PORT)
#define VOIP_PORT_NUM 1
#endif

typedef struct TX_FAULT_REPORT_s
{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	pon_num;
	u8_t	major_pon;
	u8_t    sub_pon;
}TX_FAULT_REPORT_s;


typedef enum OPTICAL_MODULE_PARAMETER_GET
{  
  GET_TEMPERATURE = 0,
  GET_VOLTAGE = 1,
  GET_BASIC = 2,
  GET_TX_PWR = 3,
  GET_RX_PWR = 4
}OPTICAL_MODULE_PARAMETER_GET;

typedef enum ctc_oam_alarm_id{
	ONU_EQUIPMENT_ALARM = 0x0001,
	ONU_POWERING_ALARM,
	ONU_BATTERY_MISSING_ALARM,
	ONU_BATTERY_FAILURE_ALARM,
	ONU_BATTERY_VOLT_LOW_ALARM,
	ONU_PHYSICAL_INTRUSION_ALARM,
	ONU_SELF_TEST_FAILURE_ALARM,
	ONU_POWERING_VOLT_LOW_ALARM,
	ONU_TEMP_HIGH_ALARM,
	PON_RX_POWER_HIGH_ALARM = 0x0101,
	PON_RX_POWER_LOW_ALARM,
	PON_TX_POWER_HIGH_ALARM,
	PON_TX_POWER_LOW_ALARM,
	PON_TX_BIAS_HIGH_ALARM,
	PON_TX_BIAS_LOW_ALARM,
	PON_VCC_HIGH_ALARM,
	PON_VCC_LOW_ALARM,
	PON_TEMP_HIGH_ALARM,
	PON_TEMP_LOW_ALARM,
	PON_RX_POWER_HIGH_WARNING,
	PON_RX_POWER_LOW_WARNING,
	PON_TX_POWER_HIGH_WARNING,
	PON_TX_POWER_LOW_WARNING,
	PON_TX_BIAS_HIGH_WARNING,
	PON_TX_BIAS_LOW_WARNING,
	PON_VCC_HIGH_WARNING,
	PON_VCC_LOW_WARNING,
	PON_TEMP_HIGH_WARNING,
	PON_TEMP_LOW_WARNING,
	CARD_ALARM = 0x0201,
	CARD_SELF_TEST_FAILURE_ALARM,
	ETHPORT_AUTO_NEG_FAILURE_ALARM = 0x0301,
	ETHPORT_LOS_ALARM,
	ETHPORT_LOOPBACK_ALARM,
	ETHPORT_CONGESTION_ALARM,
	POTSPORT_FAILURE_ALARM = 0x0401,
	E1PORT_FAILURE_ALARM = 0x0501,
	E1PORT_TIMING_UNLOCK_ALARM,
	E1PORT_LOS_ALARM,
	IAD_CONNECTION_FAILURE_ALARM = 0x0601,
}CTC_OAM_ALARM_ID_e;

typedef enum ctc_oam_object{
	OBJ_ONU,
	OBJ_UNI,
	OBJ_CARD,
	OBJ_LLID,
	OBJ_PON,
}CTC_OAM_OBJ_TYPE_e;

typedef enum ctc_oam_port_type{
	PORT_ETH = 0x1,
	PORT_VOIP,
	PORT_ADSL2,
	PORT_VDSL2,
	PORT_E1,
}CTC_OAM_PORT_TYPE_e;

typedef enum ctc_oam_alarm_status{
	ALARM_DISABLE,
	ALARM_ENABLE,
}CTC_OAM_ALARM_STATUS_e;

typedef struct uni_source{
	CTC_OAM_PORT_TYPE_e port_type;
	u8_t frame_id;
	u8_t slot_id;
	u16_t port_id;
}UNI_SRC_e;

typedef struct alarm_source{
	CTC_OAM_OBJ_TYPE_e obj_type;
	union{
		UNI_SRC_e uni_id; 		
		u32_t card_id;
		u32_t llid;
		u32_t pon_id;
	}para;
}CTC_ALARM_SRC_t;

struct ctc_envet_hdr{
	u16_t seq_num;
	u8_t event_type;
	u8_t event_len;
	u8_t oui[3];
}__attribute__((packed));
typedef struct ctc_envet_hdr ctc_event_hdr_t;

struct ctc_alarm_entry_hdr{
	u16_t obj_type;
	u32_t instance_num;
	u16_t alarm_id;
	u16_t timeStamp;
	u8_t alarm_state;
}__attribute__((packed));
typedef struct ctc_alarm_entry_hdr ctc_alarm_entry_hdr_t;

#define CTC_ALARM_ENABLE  0x00000002
#define CTC_ALARM_DISABLE 0x00000001

/* CTC OAM auth code definition */
#define CTC_OAM_AUTH_REQUEST   0x01
#define CTC_OAM_AUTH_RESPONSE  0x02
#define CTC_OAM_AUTH_SUCCESS   0x03
#define CTC_OAM_AUTH_FAILURE   0x04

/* CTC OAM auth type definition */
#define CTC_OAM_AUTH_LOID_PASSWORD 0x1
#define CTC_OAM_AUTH_NAK           0x2

/* Stand Auto negotiation admin status defintion */
#define OAM_STD_AUTONEG_ADMIN_DISABLE 0x00000001
#define OAM_STD_AUTONEG_ADMIN_ENABLE  0x00000002

/* Stand PHY admin status definition */
#define OAM_STD_PHY_ADMIN_DISABLE  0x00000001
#define OAM_STD_PHY_ADMIN_ENABLE   0x00000002

/* CTC IGMP fast leave status definition*/
#define OAM_CTC_IGMP_FAST_LEAVE_DISABLE   0x00000001
#define OAM_CTC_IGMP_FAST_LEAVE_ENABLE    0x00000002


#define OAM_CTC_IGMP_SNOOPING_NOT_FAST_LEAVE 0x00000000
#define OAM_CTC_IGMP_SNOOPING_FAST_LEAVE     0x00000001
#define OAM_CTC_IGMP_CONTROL_NOT_FAST_LEAVE  0x00000010
#define OAM_CTC_IGMP_CONTROL_FAST_LEAVE      0x00000011

/* CTC SLA operation definition */
#define OAM_CTC_SERVICE_DBA_DEACTIVE 0x0
#define OAM_CTC_SERVICE_DBA_ACTIVE   0x1

/* CTC MCAST VLAN operation definition */
#define OAM_CTC_MCAST_VLAN_DEL   0x0
#define OAM_CTC_MCAST_VLAN_ADD   0x1
#define OAM_CTC_MCAST_VLAN_CLEAR 0x2
#define OAM_CTC_MCAST_VLAN_LIST  0x3

/* CTC MCAST entry operation definition */
#define OAM_CTC_MCAST_ENTRY_DEL   0x0
#define OAM_CTC_MCAST_ENTRY_ADD   0x1
#define OAM_CTC_MCAST_ENTRY_CLEAR 0x2
#define OAM_CTC_MCAST_ENTRY_LIST  0x3

/* CTC MCAST VLAN tag operation definition */
#define OAM_CTC_MCAST_VLAN_TAG_NOT_STRIP 0x0
#define OAM_CTC_MCAST_VLAN_TAG_STRIP     0x1
#define OAM_CTC_MCAST_VLAN_TAG_TRANLATE  0x2

/* CTC ONU type definition */
#define OAM_CTC_ONU_TYPE_SFU   0x00000000
#define OAM_CTC_ONU_TYPE_HGU   0x00000001
#define OAM_CTC_ONU_TYPE_SBU   0x00000002
#define OAM_CTC_ONU_TYPE_MDU_1 0x00000003
#define OAM_CTC_ONU_TYPE_MDU_2 0x00000004
#define OAM_CTC_ONU_TYPE_MDU_3 0x00000005
#define OAM_CTC_ONU_TYPE_MDU_4 0x00000006
#define OAM_CTC_ONU_TYPE_MTU   0x00000007

/* CTC ONU multi-LLID definition */
#define OAM_CTC_SINGLE_LLID  0x1
#define OAM_CTC_MULTI_LLID	 0x0

/* CTC ONU protection type definition */
#define OAM_CTC_PROTECTION_NONE   0x0
#define OAM_CTC_PROTECTION_TYPE_C 0x1
#define OAM_CTC_PROTECTION_TYPE_D 0x2

/* CTC module type definition */
#define OAM_CTC_MODULE_GE    0x0
#define OAM_CTC_MODULE_FE    0x1
#define OAM_CTC_MODULE_VOIP  0x2
#define OAM_CTC_MODULE_TDM   0x3
#define OAM_CTC_MODULE_ADSL2 0x4
#define OAM_CTC_MODULE_VDSL2 0x5
#define OAM_CTC_MODULE_WIFI  0x6
#define OAM_CTC_MODULE_USB   0x7

/* CTC OAM response indication value definition */
#define OAM_CTC_RSP_SET_OK      0x80
#define OAM_CTC_RSP_BAD_PARA    0x86
#define OAM_CTC_RSP_NO_RESOURCE 0x87

/* CTC PORT type definition */
#define OAM_CTC_PORT_TYPE_ETH   0x1
#define OAM_CTC_PORT_TYPE_VOIP  0x2
#define OAM_CTC_PORT_TYPE_ADSL2 0x3
#define OAM_CTC_PORT_TYPE_VDSL2 0x4
#define OAM_CTC_PORT_TYPE_E1    0x5

/* CTC 2.1 TLV definition */
#define OAM_CTC_PORT_TYPE_OFFSET 24
#define OAM_CTC_PORT_FRAME_OFFSET 22 
#define OAM_CTC_PORT_SLOT_OFFSET 16
#define OAM_CTC_PORT_NUM_OFFSET  0

#define OAM_CTC_PORT_TYPE_MASK   0xFF000000
#define OAM_CTC_PORT_NUM_MASK	 0x0000FFFF
#define OAM_CTC_PORT_SLOT_MASK   0x002F0000
#define OAM_CTC_PORT_FRAME_MASK  0x00C00000

/* CTC 2.1 instance tlv leaf type definition */
#define OAM_CTC_OBJ_UNI     	0x0001
#define OAM_CTC_OBJ_CARD  		0x0002
#define OAM_CTC_OBJ_LLID		0x0003
#define OAM_CTC_OBJ_PON			0x0004

#define OAM_GET_PORT_TYPE(value) (u8_t)(((u32_t)value & OAM_CTC_PORT_TYPE_MASK) >> OAM_CTC_PORT_TYPE_OFFSET)
#define OAM_GET_PORT_NUM(value)  (u16_t)(((u32_t)value & OAM_CTC_PORT_NUM_MASK) >> OAM_CTC_PORT_NUM_OFFSET)
#define OAM_GET_PORT_FRAME_ID(value) (u8_t)((u32_t)value & OAM_CTC_PORT_FRAME_MASK) >> OAM_CTC_PORT_FRAME_OFFSET)
#define OAM_GET_PORT_SLOT_ID(value) (u8_t)((u32_t)value & OAM_CTC_PORT_SLOT_MASK) >> OAM_CTC_PORT_SLOT_OFFSET)

#define OAM_SET_PORT_TYPE(value,type) (value & ~(OAM_CTC_PORT_TYPE_MASK)) | ((u32_t)type << OAM_CTC_PORT_TYPE_OFFSET)
#define OAM_SET_PORT_NUM(value, num)  (value & ~(OAM_CTC_PORT_NUM_MASK)) | ((u32_t)num << OAM_CTC_PORT_NUM_OFFSET)
#define OAM_SET_PORT_FRAME_ID(value, frame) (value & ~(OAM_CTC_PORT_FRAME_MASK)) << ((u32_t)frame << OAM_CTC_PORT_FRAME_OFFSET)
#define OAM_SET_PORT_SLOT_ID(value, frame) (value & ~(OAM_CTC_PORT_SLOT_MASK)) << ((u32_t)frame << OAM_CTC_PORT_SLOT_OFFSET)

#define ONU_HOLDOVER_ACTIVE	   0x0002
#define ONU_HOLDOVER_DISACTIVE 0x0001


#endif

