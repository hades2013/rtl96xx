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
#include "opl_driver.h"

#define	SUBVERSTR_LEN	16

/* 0xC7 0x0046 */
#define OAM_CTC_IGMP_SNOOPING_NOT_FAST_LEAVE  0x00000000
#define OAM_CTC_IGMP_SNOOPING_FAST_LEAVE      0x00000001
#define OAM_CTC_IGMP_CTC_NOT_FAST_LEAVE       0x00000010
#define OAM_CTC_IGMP_CTC_FAST_LEAVE           0x00000011

/* 0xC7 0x0047 */
#define OAM_CTC_IGMP_FAST_LEAVE_DISABLE		  0x1
#define OAM_CTC_IGMP_FAST_LEAVE_ENABLE        0x2

extern void OamDbgSwitch (int iEndis);
extern void eopl_ctc_header(u8_t ext_code);
extern void eopl_get_onusn_rsp(void);
extern void eopl_get_onu_firmwarever(void);
extern void eopl_get_onu_chipid(void);
extern void eopl_get_onu_capabilities(void);
extern void eopl_get_onu_eth_linkstate(void);
extern void eopl_get_onu_eth_portpause(void);
extern void eopl_get_onu_eth_portpolicing(void);
extern void eopl_get_onu_eth_portdspolicing(void);
extern void eopl_get_onu_eth_phyadmin(void);
extern void eopl_get_onu_eth_autonegadmin(void);
extern void eopl_get_onu_eth_autoneglocal(void);
extern void eopl_get_onu_eth_autonegadv(void);
extern void eopl_get_onu_eth_fecability(void);
extern void eopl_get_onu_eth_fecmode(void);
extern void eopl_get_onu_mcast_tagstrip(void);
extern void eopl_get_onu_mcast_switch(void);
extern void eopl_get_onu_grp_nummax(void);
extern void eopl_get_onu_fastleave_abl(void);
extern void eopl_get_onu_fastleave_admin(void);
extern void eopl_get_onu_mcast_vlan(void);
extern void eopl_get_onu_mcast_control(void);
extern void eopl_set_dba(u8_t ucQSetNum, oam_dba_queue_t *p_dba);
extern void eopl_get_dba(void);
extern bool_t eopl_get_tag_id(u8_t portid,u32_t *value);
extern bool_t eopl_get_vtt_list(u8_t portid,u8_t *p_byte,u8_t *num, u32_t uiDefVid);
extern int eopl_get_onu_vlan(void);
extern void eopl_get_onu_classify(void);
extern void eopl_ctc_get_req_2_0(u8_t* p_byte);
extern void eopl_set_onu_eth_portpause(u8_t action);
extern void eopl_set_onu_eth_policing(u8_t operation,u8_t* p_cir,u8_t* p_cbs,u8_t* p_ebs);
extern void eopl_set_onu_eth_dspolicing(u8_t operation,u8_t* p_cir,u8_t* p_pir);
extern void eopl_set_onu_eth_admincontrol(u32_t action);
extern void eopl_set_onu_eth_atuorestart(void);
extern void eopl_set_onu_eth_autonegadmin(u8_t action);
extern void eopl_set_onu_eth_fecmode(u32_t mode);
extern void eopl_set_onu_mcast_group_max(u8_t max);
extern void eopl_set_onu_mcast_adm_control(u8_t value);
extern void eopl_set_onu_mcast_control(u8_t *p_in);
extern void eopl_set_onu_mcast_switch(u8_t action);
extern void eopl_set_onu_mcast_tagstrip(u32_t tagstriped);
extern void eopl_set_onu_mcast_vlan(u8_t *p_in);
extern int eopl_set_tag_id(u8_t portid,u32_t *value);
extern int eopl_set_onu_vlan(u8_t*  p_in);
extern void eopl_set_onu_classfy(u8_t*  p_in);
extern int eopl_set_onu_classify_set_2_0(u8_t*  p_in, u8_t ucPort);
extern void eopl_ctc_set_req_2_0(u8_t* p_byte);
extern void eopl_req_new_key(u8_t index);
extern void eopl_req_tri_scb_key(u8_t index, u8_t *pucKey);
extern void eopl_ctc_churning(u8_t* p_byte);
extern void eopl_ctc_dba(u8_t* p_byte);
extern void eopl_req_aes_key(u8_t index, u8_t ucMode);
extern void eopl_req_aesscb_key(u8_t index, u8_t ucMode, u8_t *pucKey);
extern void eopl_aeskey_req(u8_t* p_byte);
extern void eopl_aesscbkey_req(u8_t* p_byte);
extern void eopl_TriScbkey_req(u8_t* p_byte);
extern void eopl_ctc_pdu_process_2_0(u8_t *p_byte,u16_t length);
extern int SwVersionGet(u8_t *pucVerStr);
extern OPL_STATUS eopl_ctc_authen(u8_t *pucData);

#endif

