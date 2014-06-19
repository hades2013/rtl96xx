/*=============================================================================
FILE - ctc.c

DESCRIPTION
	Implements the 802.3 and CTC oam stack

MODIFICATION DETAILS
=============================================================================*/


#include "lw_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "oam.h"
#include "ctc_2_1.h"
#include "opl_driver.h"
#include "multicast_control.h"
#if 0/*wfxu 0401*/
#include "dal_port.h"
#include "dal_vtt.h"
#include "dal_dba.h"
#include "dal_pon.h"
#endif
#include <vos.h>
#include "log.h"
#include "odm_port.h"
#include "odm_pon.h"
#include "odm_vlan.h"
#include "system.h"
#include "version.h"
#include "qos.h"
#include "time.h"
#include "mc_control.h"
#if 0 /*wfxu 0401*/
#include "dal_multicast.h"
#include "dal_vtt.h"
#include "dal_lib.h"
#endif
#include "zte.h"
#include "oam_alarm.h"
#include "opconn_usr_ioctrl.h"
#include "oam_lw.h"

#define MODULE MOD_OAM
extern u8_t alarm_out_data[OAM_MAXIMUM_PDU_SIZE+4];
extern u8_t	out_data[OAM_MAXIMUM_PDU_SIZE+4];
extern u8_t	in_data[OAM_MAXIMUM_PDU_SIZE+4];
extern u16_t in_data_len;

extern u8_t	oam_oui[3];
extern u8_t	oam_onu_vendor[4];
extern u8_t	oam_chip_vendor[2];
extern u8_t	oam_onu_model[4];
extern u8_t	oam_chip_model[2];
extern u8_t	oam_chip_revision;
extern u8_t	oam_broadcast_mac[6];
extern u8_t	oam_src_mac[6];
extern u8_t	oam_firm_ver[2];
extern u8_t	oam_onu_hwarever[8];
extern u8_t	oam_onu_swarever[16];
extern u8_t	oam_chip_revision;
extern u8_t	oam_chip_version[3];
extern u8_t oamtransmitevent;
extern UINT8 OamClsCount;


extern int g_alarm_seq_no;

static u8_t	upgrade_out_data[OAM_MAXIMUM_PDU_SIZE+4];


//extern oam_instance_tlv_t_2_0 port_instance_2_0;
oam_instance_tlv_t	port_instance;
TFTP_FILE_t g_stImageFile;
uint8 g_upgradeOngoing;
uint8 g_waitForActive;
extern oam_sate_t oams;

extern  u8_t g_aucCtcOui[3];
extern u16_t g_usAlarmOutDataIdx;
extern u16_t g_usOutDataIdx;
extern int g_iOamSetDbg;

extern int g_iOamAuthCode;


extern int CTC_DRV_SetTransparentVlan(u32_t uiLPortId);
extern int CTC_DRV_SetTagVlan(u32_t uiLPortId, u32_t uiPvid, u32_t uiPriority);
extern int  CTC_DRV_GetVlanIdAndPri(u32_t uiVlanTag, u32_t *puiVlanId, u32_t *puiPri);


#if 1
/* 0x0006 service SLA  */
static u16_t stub_queue_weight[8] = {16, 12,12,12, 12, 12, 12,12};

/* 0x0008 holdover config */
static u32_t stub_holdover_state;
static u32_t stub_holdover_time;

/* 0x0009 snmp global para */
static u32_t stub_mng_ip_addr;
static u32_t stub_mng_ip_mask;
static u32_t stub_mng_gw;
static u16_t stub_mng_data_cvlan;
static u16_t stub_mng_data_svlan;
static u8_t  stub_mng_data_priority;

/* 0x000a snmp para */
static u8_t	stub_snmp_ver;
static u32_t stub_trap_host_ip;
static u16_t stub_trap_port;
static u16_t stub_snmp_port;
static u8_t	stub_security_name[32];
static u8_t	stub_community_for_read[32];
static u8_t	stub_community_for_write[32];

/* 0x000B active pon if admin */
static u8_t stub_active_port;

/* 0x0013 eth port us policing */

/* 0x0014 voip port */
static u8_t stub_voip_lock[4];

/* 0x0015 e1 port */
static u8_t stub_e1_lock[4];

/* 0x0016 eth port ds policing */

/* 0x0017 port loop detect */
static u32_t stub_port_loop_state[4];

/* 0x0021 vlan */
static u8_t stub_vlan_mode;

/* 0x0061 */

/* 0x0062 */
static u8_t stub_voice_ip_mode;
static u32_t stub_iad_ip_addr;
static u32_t stub_iad_net_mask;
static u32_t stub_iad_default_gw;
static u8_t stub_pppoe_mode;
static u8_t stub_pppoe_username[32];
static u8_t stub_pppoe_password[32];
static u8_t stub_tagged_flag;
static u16_t stub_voice_cvlan;
static u16_t stub_voice_svlan;
static u8_t stub_voice_priority;

/* 0x0063 */
static u16_t stub_mg_port_num;
static u32_t stub_mgc_ip;
static u16_t stub_mgc_com_port_num;
static u32_t stub_backup_mgc_ip;
static u16_t stub_backup_mgc_com_port_num;
static u8_t stub_active_mgc;
static u8_t stub_reg_mode;
static u8_t stub_mid[64];
static u8_t stub_heart_beat_mode;
static u16_t stub_heart_beat_cycle;
static u8_t stub_heart_beat_count;

/* 0x0064 */
static u8_t stub_user_tid_name[4+1][32];

/* 0x0065 * 0x0066 */
static u8_t	stub_num_of_rtp_tid;
static u8_t stub_rtp_tid_prefix[16];
static u8_t stub_rtp_tid_digit_begin[8];
static u8_t stub_rtp_tid_mode;
static u8_t stub_rtp_tid_digit_len;

/* 0x0067 */
static u16_t stub_sip_mg_port_num;
static u32_t stub_sip_proxy_serv_ip;
static u16_t stub_sip_proxy_serv_com_port_num;
static u32_t stub_backup_sip_proxy_serv_ip;
static u16_t stub_backup_sip_proxy_serv_com_port_num;
static u32_t stub_active_sip_proxy_serv;
static u32_t stub_sip_reg_serv_ip;
static u16_t stub_sip_reg_serv_com_port_num;
static u32_t stub_backup_sip_reg_serv_ip;
static u16_t stub_backup_sip_reg_serv_com_port_num;
static u32_t stub_outbound_serv_ip;
static u16_t stub_outbound_serv_port_num;
static u32_t stub_sip_reg_interval;
static u8_t  stub_sip_heart_beat_switch;
static u16_t stub_sip_heart_beat_cycle;
static u16_t stub_sip_heart_beat_count;

/* 0x0068 */
static u8_t stub_user_account[16];
static u8_t stub_user_name[32];
static u8_t stub_user_password[16];

/* 0x0069 */
static u8_t stub_voice_t38_enable;
static u8_t stub_voice_fax_control;

/* 0x006C */
static u32_t stub_iad_operation;

/* 0x006D */
static u8_t stub_sip_digital_map[1024];
typedef struct{
	u16_t num_of_src_vlan;
	u32_t dst_vlan;
	u32_t src_vlan[64];
}__attribute__((packed))ctc_aggr_vlan_tbl_t;

typedef struct{
	u32_t default_vlan;
	u16_t num_of_aggr_table;
	ctc_aggr_vlan_tbl_t aggr_table[64];
}__attribute__((packed))ctc_aggr_vlan_t;

/* N:1 mode vlan database for 4 ports */
static ctc_aggr_vlan_t stub_aggr_vlan_db[4+1];

typedef struct{
	u32_t default_vlan;
	u32_t allowed_vlan[16];
	u8_t num_of_allowed_vlan;
}__attribute__((packed))ctc_trunk_vlan_t;

/* trunk mode vlan database for 4 ports */
static ctc_trunk_vlan_t stub_trunk_vlan_db[4];

/* EricYang add to store VOIP-relative parameter */
static u16_t stub_mg_port_num;
static u32_t stub_mgc_ip;
static u16_t stub_mgc_port;
static u32_t stub_backup_mgc_ip;
static u16_t stub_backup_mgc_port;
static u8_t  stub_heartbeat_mode;
static u16_t stub_heartbeat_cycle;
static u8_t  stub_port_id;
static u8_t  stub_user_tid[32];
static u8_t  stub_rtp_tid_prefix[16];
static u8_t  stub_rtp_tid_digit[8];
/* !EricYang */
#endif

#if 0
#define ONU_ALARM_NUM 10
static alarm_cfg_t stub_onu_alarm_config[ONU_ALARM_NUM] = {
	{0x0001, CTC_ALARM_ENABLE, 0, 100},
	{0x0002, CTC_ALARM_ENABLE, 0, 100},
	{0x0003, CTC_ALARM_DISABLE, 0, 100},
	{0x0004, CTC_ALARM_ENABLE, 0, 100},
	{0x0005, CTC_ALARM_ENABLE, 0, 100},
	{0x0006, CTC_ALARM_ENABLE, 0, 100},
	{0x0007, CTC_ALARM_ENABLE, 0, 100},
	{0x0008, CTC_ALARM_ENABLE, 0, 100},
	{0x0009, CTC_ALARM_ENABLE, 0, 100},
	{0x000a, CTC_ALARM_ENABLE, 0, 100},
};

#define PON_ALARM_NUM 20
static alarm_cfg_t stub_pon_alarm_config[PON_ALARM_NUM] = {
	{0x0101, CTC_ALARM_DISABLE, 0, 100},
	{0x0102, CTC_ALARM_DISABLE, 0, 100},
	{0x0103, CTC_ALARM_DISABLE, 0, 100},
	{0x0104, CTC_ALARM_DISABLE, 0, 100},
	{0x0105, CTC_ALARM_DISABLE, 0, 100},
	{0x0106, CTC_ALARM_DISABLE, 0, 100},
	{0x0107, CTC_ALARM_DISABLE, 0, 100},
	{0x0108, CTC_ALARM_DISABLE, 0, 100},
	{0x0109, CTC_ALARM_DISABLE, 0, 100},
	{0x010a, CTC_ALARM_DISABLE, 0, 100},
	{0x010b, CTC_ALARM_DISABLE, 0, 100},
	{0x010c, CTC_ALARM_DISABLE, 0, 100},
	{0x010d, CTC_ALARM_DISABLE, 0, 100},
	{0x010e, CTC_ALARM_DISABLE, 0, 100},
	{0x010f, CTC_ALARM_DISABLE, 0, 100},
	{0x0110, CTC_ALARM_DISABLE, 0, 100},
	{0x0111, CTC_ALARM_DISABLE, 0, 100},
	{0x0112, CTC_ALARM_DISABLE, 0, 100},
	{0x0113, CTC_ALARM_DISABLE, 0, 100},
	{0x0114, CTC_ALARM_DISABLE, 0, 100},
};

#define CARD_ALARM_NUM 2
static alarm_cfg_t stub_card_alarm_config[CARD_ALARM_NUM] = {
	{0x0201, CTC_ALARM_DISABLE, 0, 100},
	{0x0202, CTC_ALARM_DISABLE, 0, 100},
};

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	#define UNI_PORT_NUM 4
#else
	#define UNI_PORT_NUM 1
#endif

#define UNI_ALARM_NUM 5
static alarm_cfg_t stub_eth_port_alarm_config[UNI_PORT_NUM][UNI_ALARM_NUM] = {
	{
		{0x0301, CTC_ALARM_DISABLE, 0, 100},
		{0x0302, CTC_ALARM_DISABLE, 0, 100},
		{0x0303, CTC_ALARM_DISABLE, 0, 100},
		{0x0304, CTC_ALARM_DISABLE, 0, 100},
		{0x0305, CTC_ALARM_DISABLE, 0, 100},
	},
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	{
		{0x0301, CTC_ALARM_DISABLE, 0, 100},
		{0x0302, CTC_ALARM_DISABLE, 0, 100},
		{0x0303, CTC_ALARM_DISABLE, 0, 100},
		{0x0304, CTC_ALARM_DISABLE, 0, 100},
		{0x0305, CTC_ALARM_DISABLE, 0, 100},
	},
	{
		{0x0301, CTC_ALARM_DISABLE, 0, 100},
		{0x0302, CTC_ALARM_DISABLE, 0, 100},
		{0x0303, CTC_ALARM_DISABLE, 0, 100},
		{0x0304, CTC_ALARM_DISABLE, 0, 100},
		{0x0305, CTC_ALARM_DISABLE, 0, 100},
	},
	{
		{0x0301, CTC_ALARM_DISABLE, 0, 100},
		{0x0302, CTC_ALARM_DISABLE, 0, 100},
		{0x0303, CTC_ALARM_DISABLE, 0, 100},
		{0x0304, CTC_ALARM_DISABLE, 0, 100},
		{0x0305, CTC_ALARM_DISABLE, 0, 100},
	},
#endif
};
#endif

static u32_t stub_voip_port_get(u8_t port_id, u8_t *voip_port)
{
	/*
	Lock/Unlock
	0x00求求Port disactivated or to disactivate the port
	0x01求求port activated or to activate the port
	The default value of this field is 0x00
	*/
    *voip_port = 0;
    return OK;

}


u32_t getFirstValidByte(u32_t sizeOfStr, u8_t *porgStr)
{
    u32_t i = 0;
    while ( i < sizeOfStr ){
		if ( *(porgStr + i) != 0 )
			break;
		i++;
    }
    return i;
}

extern void OamDbgSwitch (int iEndis);

void eopl_ctc_header(u8_t ext_code)
{
	u8_t	*p_byte;
	ctc_header_t	*p_ctc;

	p_byte = &out_data[0];
	eopl_oam_pdu_hdr(p_byte, oams.flags, OAM_CODE_ORG_SPEC);
	p_byte += sizeof(oampdu_hdr_t);

	p_ctc = (ctc_header_t*)p_byte;
	vosMemCpy(p_ctc->oui, g_aucCtcOui, 3);
	p_ctc->ext_code = ext_code;
}
/* EricYang add for the Ev_Reg_CalllBack */
static u8_t * eopl_get_output_buf(u8_t ext_code)
{
	u8_t			*p_byte;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(ext_code);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
    return p_byte;
}
/* !EricYang */

int oamLSCAlignmentStrGet(UINT8 *pucSrc, UINT16 usSrcSize, char *pcDst, UINT16 usDstSize)
{
    UINT8 *pucResult;
    char *pcStr;

    if (NULL == pucSrc || NULL == pcDst)
        return ERROR;

    pucResult = (UINT8 *)memrchr(pucSrc, 0, usSrcSize);
    pcStr = (char *)(pucResult + 1);
    vosSafeStrNCpy(pcDst, pcStr, usDstSize);

    return OK;
}

int SwVersionGet(u8_t *pucVerStr)
{
	int		iStatus=0;
	u8_t	ucCnt, ucCnt1=0;
	u8_t	ucVer;
	u8_t	aucVer[SUBVERSTR_LEN];

	if(0 == pucVerStr)
	{
		return -1;
	}

	if((9<MAJOR_VERSION) || (9<MINOR_VERSION) || (99<BUILD_NUMBER))
	{
		return -1;
	}

	ucVer = MAJOR_VERSION;
	memset(aucVer, 0, SUBVERSTR_LEN);
	for(ucCnt=SUBVERSTR_LEN; ((0<ucVer)&&(0<ucCnt)); ucCnt--)
	{
		aucVer[ucCnt-1] = 0x30 + ucVer%10;
		ucVer /= 10;
	}
	if(16 < ucCnt1+SUBVERSTR_LEN-ucCnt)
	{
		return -1;
	}
	memcpy(pucVerStr+ucCnt1, aucVer+ucCnt, SUBVERSTR_LEN-ucCnt);
	ucCnt1 += SUBVERSTR_LEN-ucCnt;
	pucVerStr[ucCnt1++] = '.';

	ucVer = MINOR_VERSION;
	memset(aucVer, 0, SUBVERSTR_LEN);
 	if(0 == ucVer)
	{
		ucCnt = SUBVERSTR_LEN;
		aucVer[ucCnt-1] = 0x30;
		ucCnt--;
	}else{
        for(ucCnt=SUBVERSTR_LEN; ((0<ucVer)&&(0<ucCnt)); ucCnt--)
        {
            aucVer[ucCnt-1] = 0x30 + ucVer%10;
            ucVer /= 10;
        }
        if(16 < ucCnt1+SUBVERSTR_LEN-ucCnt)
        {
            return -1;
        }
    }
	memcpy(pucVerStr+ucCnt1, aucVer+ucCnt, SUBVERSTR_LEN-ucCnt);
	ucCnt1 += SUBVERSTR_LEN-ucCnt;
	pucVerStr[ucCnt1++] = '.';

	ucVer = BUILD_NUMBER;
	memset(aucVer, 0, SUBVERSTR_LEN);
	if(0 == ucVer)
	{
		ucCnt = SUBVERSTR_LEN;
		aucVer[ucCnt-1] = 0x30;
		ucCnt--;
	}
	else
	{
		for(ucCnt=SUBVERSTR_LEN; ((0<ucVer)&&(0<ucCnt)); ucCnt--)
		{
			aucVer[ucCnt-1] = 0x30 + ucVer%10;
			ucVer /= 10;
		}
		if(16 < ucCnt1+SUBVERSTR_LEN-ucCnt)
		{
			return -1;
		}
	}
	memcpy(pucVerStr+ucCnt1, aucVer+ucCnt, SUBVERSTR_LEN-ucCnt);
	ucCnt1 += SUBVERSTR_LEN-ucCnt;
	pucVerStr[ucCnt1] = 0;
/*
	for(ucCnt=15; ucCnt1>0; ucCnt--, ucCnt1--)
	{
		pucVerStr[ucCnt] = pucVerStr[ucCnt1-1];
	}
	for(ucCnt++; ucCnt>0; ucCnt--)
	{
		pucVerStr[ucCnt-1] = 0;
	}
*/
	return iStatus;
}

/*** checked ok ***/
void eopl_get_onu_sn_rsp(void)
{
	int				iStatus=0;
	u8_t			*p_byte;
	oam_onu_sn_t	*p_sn;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_sn = (oam_onu_sn_t*)p_byte;
	p_sn->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_sn->leaf = OAM_ATTR_ONU_SN;
	p_sn->width = 38;
	vosMemCpy(p_sn->vendor_id, oam_onu_vendor, 4);
	vosMemCpy(p_sn->model, oam_onu_model, 4);
	vosMemCpy(p_sn->onu_id, oam_src_mac, 6);
	vosMemCpy(p_sn->hware_ver,oam_onu_hwarever, 8);
	#if 0
	iStatus = SwVersionGet(oam_onu_swarever);
	if(0 != iStatus)
	{
		memset(oam_onu_swarever, 0, 16);
		OAMDBG(("software version get error.\n"));
	}
	#else
	vosSprintf((CHAR *)oam_onu_swarever, "%s", CONFIG_RELEASE_VER);
	#endif
	vosMemCpy(p_sn->sware_ver,oam_onu_swarever, 16);
	
	g_usOutDataIdx += sizeof(oam_onu_sn_t);
}

/*** checked ok ***/
void eopl_get_onu_firmwarever(void)
{
	u8_t	*p_byte;
	oam_fware_ver_t	*p_fver;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_fver = (oam_fware_ver_t*)p_byte;
	p_fver->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_fver->leaf = OAM_ATTR_ONU_FIMWAREVER;
	p_fver->width = FIRM_LENGTH;
	vosMemCpy(p_fver->fware_ver, oam_firm_ver, FIRM_LENGTH);
	g_usOutDataIdx += sizeof(oam_fware_ver_t);
}

/*** checked ok ***/
void eopl_get_onu_chipid(void)
{
	u8_t	*p_byte;
	oam_chipset_id_t	*p_chip;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_chip = (oam_chipset_id_t*)p_byte;
	p_chip->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_chip->leaf = OAM_ATTR_CHIPSET_ID;
	p_chip->width = 8;
	vosMemCpy(p_chip->vendor_id, oam_chip_vendor, 2);
	vosMemCpy(p_chip->model, oam_chip_model, 2);
	p_chip->revision = oam_chip_revision;
	vosMemCpy(p_chip->ic_version, oam_chip_version, 3);

	g_usOutDataIdx += sizeof(oam_chipset_id_t);
}

/*** checked ok ***/
void eopl_get_onu_capabilities1(void)
{
	u8_t	*p_byte;
	//oam_rsp_indication_t	*p_rsp;
	oam_onu_capabilites_t	*p_cap;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	/* be compatible for the cortina olt.*/
	#if 0
	p_rsp = (oam_rsp_indication_t *)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_ONU_CAPABILITIES1;
	p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
	g_usOutDataIdx += sizeof(oam_rsp_indication_t);
	#endif

	p_cap = (oam_onu_capabilites_t *)p_byte;
	p_cap->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_cap->leaf = OAM_ATTR_ONU_CAPABILITIES1;
	p_cap->width = 0x1A;
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)
	p_cap->serv_supported = 0x02;
	p_cap->number_GE = 0;
	vosMemSet(p_cap->bitmap_GE, 0, 8);
	p_cap->number_FE = 4;
	vosMemSet(p_cap->bitmap_FE, 0, 8);
	p_cap->bitmap_FE[7] = 0x0F;
#elif defined(ONU_4PORT_AR8327)
       p_cap->serv_supported = 0x01;
	p_cap->number_GE = 4;
	vosMemSet(p_cap->bitmap_GE, 0, 8);
	p_cap->bitmap_GE[7] = 0x0F;
	p_cap->number_FE = 0;
	vosMemSet(p_cap->bitmap_FE, 0, 8);
#else
	
    switch ( g_enOnuBoardType)
	{
	    case ONU_BORAD_TYPE_EPN104:	    
            p_cap->serv_supported = 0x02;
            /*begin modified by liaohongjun 2013/1/5 of EPN104QID0090*/
            #ifdef CONFIG_SWITCH_CHIP_RTL8365MB
        	p_cap->number_GE = 4;
        	vosMemSet(p_cap->bitmap_GE, 0, 8);
            p_cap->bitmap_GE[7] = 0x0F;
        	p_cap->number_FE = 0;
        	vosMemSet(p_cap->bitmap_FE, 0, 8);            
            #else
			p_cap->number_GE = 0;
	        vosMemSet(p_cap->bitmap_GE, 0, 8);
        	p_cap->number_FE = 4;
        	vosMemSet(p_cap->bitmap_FE, 0, 8);
        	p_cap->bitmap_FE[7] = 0x0F;
            #endif
            /*end modified by liaohongjun 2013/1/5 of EPN104QID0090*/
	        break;	   
	    default:
	        break;
	}		
#endif
	p_cap->number_POTS = 0;
	p_cap->number_E1 = 0;

#if defined(ONU_4_PORT_88E6045) || defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097) 
	p_cap->number_usque = 8;
	p_cap->quemax_us = 8;
	p_cap->number_dsque = 8;
	p_cap->quemax_ds = 8;
#endif
#if defined (ONU_1PORT)
    /*We report 8 queues here.
      But only use 5 queues:0~3 used for normal data. 7 used for oam.
     */
    p_cap->number_usque = 8;
	p_cap->quemax_us = 8;
	p_cap->number_dsque = 8;
	p_cap->quemax_ds = 8;
#endif
#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	p_cap->number_usque = 4;
	p_cap->quemax_us = 4;
	p_cap->number_dsque = 16;
	p_cap->quemax_ds = 4;
#endif

	p_cap->batteryBackup = 0;

	g_usOutDataIdx += sizeof(oam_onu_capabilites_t);
}

//add by wuxiao 2010.1.8
int stub_get_onu_optical_transiver_parameter(OPTICAL_MODULE_PARAMETER_GET get_type,u16_t *pGetVal)
{
  STATUS s_Ret = ERROR;
  
  enum 
  {
   MAX_LOOP = 5,
   BUF_SIZE = 0x02,
   A2_ADDR  = 0xA2>>1,
   ORIGIN_SUB_ADDR = 0x60
  };
  
  UINT8 aucBuf[BUF_SIZE+1] = {0};

  int iCnt = 0;

  /*i2cInit(1, 1, 0, 0);*/

for(iCnt = 0; iCnt<MAX_LOOP; iCnt++)
{

  switch(get_type)
  {
     case GET_TEMPERATURE:
	  	 s_Ret = i2cReadRandom(A2_ADDR, ORIGIN_SUB_ADDR, BUF_SIZE, aucBuf);
	  	break;
		
	  case GET_VOLTAGE:
        s_Ret = i2cReadRandom(A2_ADDR, (ORIGIN_SUB_ADDR + 2), BUF_SIZE, aucBuf);
		break;
		
	  case GET_BASIC:
       s_Ret = i2cReadRandom(A2_ADDR, (ORIGIN_SUB_ADDR + 4), BUF_SIZE, aucBuf);;
		break;
		
	  case GET_TX_PWR:
        s_Ret = i2cReadRandom(A2_ADDR, (ORIGIN_SUB_ADDR + 6), BUF_SIZE, aucBuf);;
		break;
		
	   case GET_RX_PWR:
        s_Ret = i2cReadRandom(A2_ADDR, (ORIGIN_SUB_ADDR + 8), BUF_SIZE, aucBuf);;
		break;
		
		default:
	      vosPrintf(1, "\r\n unknow argument \r\n");
		   return ERROR;
		break;

  }

	 if(s_Ret == BUF_SIZE)
	 {
	   *pGetVal = aucBuf[1] | aucBuf[0]<<8;
	  // vosPrintf(1, "\r\n aucBuf[0]: %x, aucBuf[1]: %x\r\n",aucBuf[0],aucBuf[1]);
       return s_Ret;
	 }    

}	

if(MAX_LOOP == iCnt)
{
  *pGetVal = ERROR;
  return ERROR;
}

}



int stub_get_onu_optical_transiver_temperature(u16_t *pTemperature)
{
	return stub_get_onu_optical_transiver_parameter(GET_TEMPERATURE,pTemperature);
}

int stub_get_onu_optical_transiver_supply_vcc(u16_t *pVcc)
{
	return stub_get_onu_optical_transiver_parameter(GET_VOLTAGE,pVcc);
}

int stub_get_onu_optical_transiver_tx_bias_current(u16_t *pCurrent)
{
	return stub_get_onu_optical_transiver_parameter(GET_BASIC,pCurrent);
}

int stub_get_onu_optical_transiver_tx_power(u16_t *pTxPower)
{
	return stub_get_onu_optical_transiver_parameter(GET_TX_PWR,pTxPower);
}

int stub_get_onu_optical_transiver_rx_power(u16_t *pRxPower)
{
	return stub_get_onu_optical_transiver_parameter(GET_RX_PWR,pRxPower);
}



void eopl_get_onu_optical_transceiver_diag(void)
{
	u8_t *p_byte;
	oam_optical_transceiver_diag_t	*p_diag;
	u16_t temperature;
	u16_t vcc;
	u16_t current;
	u16_t txPower;
	u16_t rxPower;
	short sValue;
#if 0
	stub_get_onu_optical_transiver_temperature(&temperature);
	stub_get_onu_optical_transiver_supply_vcc(&vcc);
	stub_get_onu_optical_transiver_tx_bias_current(&current);
	stub_get_onu_optical_transiver_tx_power(&txPower);
	stub_get_onu_optical_transiver_rx_power(&rxPower);
#else
    (void)Drv_GetOpticsTransmitPower(&txPower);
    (void)Drv_GetOpticsReceivePower(&rxPower);
	(void)Drv_GetOpticsTemperature(&sValue, &vcc, &current);
	if (sValue > 0)
    {
        temperature = sValue;
    }
    else
    {
        temperature = 0;
    }
#endif
	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_diag = (oam_optical_transceiver_diag_t*)p_byte;
	p_diag->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_diag->leaf = OAM_ATTR_OPTICS_DIAG;
	p_diag->width = 0xa;
	p_diag->temperature = temperature;
	p_diag->supply_vcc = vcc;
	p_diag->tx_bias_current = current;
	p_diag->tx_power = txPower;
	p_diag->rx_power = rxPower;
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[%s,%d]:txPower = %d,rxPower = %d,temperature = %d,vcc = %d,current = %d\n", __FUNCTION__, __LINE__,\
        txPower,rxPower,temperature,vcc,current);   
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[%s,%d]:txPower = 0x%04x,rxPower = 0x%04x,temperature = 0x%04x,vcc = 0x%04x,current = 0x%04x\n", __FUNCTION__, __LINE__,\
        txPower,rxPower,temperature,vcc,current);     
	g_usOutDataIdx += sizeof(oam_optical_transceiver_diag_t);

}
void eopl_get_onu_service_sla(void)
{
	u8_t *p_byte;
	oam_service_sla_t *p_service_sla;
	oam_service_sla_body_hdr_t *p_sla_body_hdr;
	oam_service_sla_body_entry_t *p_sla_body_entry;
	u8_t queue_num;
	u8_t operation;
	u8_t i;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_service_sla = (oam_service_sla_t*)p_byte;
	p_service_sla->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_service_sla->leaf = OAM_ATTR_SERVICE_SLA;

	operation = OAM_CTC_SERVICE_DBA_ACTIVE;

	if(operation == OAM_CTC_SERVICE_DBA_DEACTIVE)
	{
		p_service_sla->operation = operation;
		p_service_sla->width = 1;

		p_byte += sizeof(oam_service_sla_t);
		g_usOutDataIdx += sizeof(oam_service_sla_t);
		return;
	}
	else if(operation == OAM_CTC_SERVICE_DBA_ACTIVE)
	{
		queue_num = 8;
		p_service_sla->operation = operation;
		p_service_sla->width = 8 + 10 * queue_num;
		p_byte += sizeof(oam_service_sla_t);
		g_usOutDataIdx += sizeof(oam_service_sla_t);

		p_sla_body_hdr = (oam_service_sla_body_hdr_t *)p_byte;
		p_sla_body_hdr->schedule_scheme = 0;
		p_sla_body_hdr->high_pri_boundry = 7;
		p_sla_body_hdr->cycle_len = 0;
		p_sla_body_hdr->service_num = 8;

		p_byte += sizeof(oam_service_sla_body_hdr_t);
		g_usOutDataIdx += sizeof(oam_service_sla_body_hdr_t);

		for(i = 0; i < queue_num; i++)
		{
			p_sla_body_entry = (oam_service_sla_body_entry_t *)p_byte;
			p_sla_body_entry->queue_id = i;
			p_sla_body_entry->fix_size = 0;
			p_sla_body_entry->fix_bw = 0;
			p_sla_body_entry->guaranteed_bw = 0;
			p_sla_body_entry->best_effor_bw = 0;
			p_sla_body_entry->wrr_weight = 0;
			p_byte += sizeof(oam_service_sla_body_entry_t);
			g_usOutDataIdx += sizeof(oam_service_sla_body_entry_t);
		}

		return;
	}
	return;
}

void eopl_get_onu_capabilities2(void)
{
	u8_t	*p_byte;
	oam_onu_capabilites2_t	*p_cap2;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_cap2 = (oam_onu_capabilites2_t *)p_byte;
	p_cap2->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_cap2->leaf = OAM_ATTR_ONU_CAPABILITIES2;
	p_cap2->width = sizeof(oam_onu_capabilites2_t)-4;
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)
	p_cap2->onu_type = OAM_CTC_ONU_TYPE_SFU;
	p_cap2->multiLlid = OAM_CTC_SINGLE_LLID;
	p_cap2->protection_type = OAM_CTC_PROTECTION_NONE;
	p_cap2->number_of_PONIf = 1;
	p_cap2->number_of_slot = 0;
	p_cap2->number_of_interface_type = 1;
	p_cap2->interface_type = OAM_CTC_MODULE_FE;
	p_cap2->number_of_port = 4;
	p_cap2->battery_backup = 0;
#elif defined(ONU_4PORT_AR8327)
       p_cap2->onu_type = OAM_CTC_ONU_TYPE_SFU;
	p_cap2->multiLlid = OAM_CTC_SINGLE_LLID;
	p_cap2->protection_type = OAM_CTC_PROTECTION_NONE;
	p_cap2->number_of_PONIf = 1;
	p_cap2->number_of_slot = 0;
	p_cap2->number_of_interface_type = 1;
	p_cap2->interface_type = OAM_CTC_MODULE_GE;
	p_cap2->number_of_port = 4;
	p_cap2->battery_backup = 0;
#else
	p_cap2->onu_type = OAM_CTC_ONU_TYPE_SFU;
	p_cap2->multiLlid = OAM_CTC_SINGLE_LLID;
	p_cap2->protection_type = OAM_CTC_PROTECTION_NONE;
	p_cap2->number_of_PONIf = 1;
	p_cap2->number_of_slot = 0;
	p_cap2->number_of_interface_type = 1;
	switch ( g_enOnuBoardType)
	{
	    case ONU_BORAD_TYPE_EPN104:
            /*begin modified by liaohongjun 2013/1/5 of EPN104QID0090*/
            #ifdef CONFIG_SWITCH_CHIP_RTL8365MB
            p_cap2->interface_type = OAM_CTC_MODULE_GE;
            #else
	        p_cap2->interface_type = OAM_CTC_MODULE_FE;
            #endif
            /*end modified by liaohongjun 2013/1/5 of EPN104QID0090*/
	        break;	   
	    default:
	        p_cap2->interface_type = OAM_CTC_MODULE_FE;
	}	
	p_cap2->number_of_port = CTC_MAX_SUBPORT_NUM;
	p_cap2->battery_backup = 0;
#endif
	g_usOutDataIdx += sizeof(oam_onu_capabilites2_t);
	return;
}


void eopl_get_onu_holdover_config(void)
{
	u8_t *p_out;
	oam_onu_holdover_config_t *p_holdover_cfg;
	u32_t holdover_state;
	u32_t holdover_time;

	OAMDBG(("[ enter %s ]\n", __FUNCTION__));

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct ctc_header);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	odmPonMpcpFsmHoldoverControlGet(&holdover_state);
	odmPonMpcpFsmHoldoverTimeGet(&holdover_time);

	p_holdover_cfg = (oam_onu_holdover_config_t *)p_out;
	p_holdover_cfg->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_holdover_cfg->leaf = OAM_ATTR_HOLDOVER_CONFIG;
	p_holdover_cfg->width = 0x8;
	p_holdover_cfg->holdover_state = holdover_state+1;
	p_holdover_cfg->holdover_time = holdover_time;

	g_usOutDataIdx += sizeof(oam_onu_holdover_config_t);

	return;
}

int stub_get_onu_active_pon_if_admin(u8_t *p_active_ports)
{
	*p_active_ports = stub_active_port;
	OAMDBG(("[ %s ]: %d\n", __FUNCTION__, stub_active_port));
	OAMDBG(("[ %s ]: %d\n", __FUNCTION__, *p_active_ports));

	return 0;
}

void eopl_get_onu_active_pon_if_admin(void)
{
	u8_t *p_out;
	oam_active_pon_if_admin_t *p_pon_if_admin;
	u8_t active_port;

	OAMDBG(("[ enter %s ]\n", __FUNCTION__));

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct ctc_header);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	stub_get_onu_active_pon_if_admin(&active_port);
	OAMDBG(("[ %s ]: %d\n", __FUNCTION__, active_port));

	p_pon_if_admin = (oam_active_pon_if_admin_t *)p_out;
	p_pon_if_admin->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_pon_if_admin->leaf = OAM_ATTR_ACTIVE_PON_IF_ADMIN;
	p_pon_if_admin->width = 1;
	p_pon_if_admin->active_port_id = active_port;

	g_usOutDataIdx += sizeof(oam_active_pon_if_admin_t);

	return;
}

/*** checked ok ***/
void eopl_get_onu_eth_linkstate(void)
{
	u8_t rlt=0;
	u16_t portS, portE;
	u8_t *p_byte;
	oam_eth_linkstate_t	*p_link;
	oam_instance_tlv_t	*p_inst;

	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
    u32_t state = 0;

	BOOL bPortStatus = TRUE;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = CTC_MAX_SUBPORT_NUM;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		p_rsp = (oam_rsp_indication_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_ETH_LINKSTATE;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_instance_tlv_t) + sizeof(oam_rsp_indication_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	for(; portS<=portE; portS++)
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}


		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;

		value = 0;
		value = OAM_SET_PORT_NUM(value, portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;
		p_byte += sizeof(oam_instance_tlv_t);

		p_link = (oam_eth_linkstate_t*)p_byte;
		p_link->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_link->leaf = OAM_ATTR_ETH_LINKSTATE;
		p_link->width = 0x1;
		#if 0
		rlt = odmPortLinkStateGet(portS, &state);
		if(rlt != OPL_OK)
		{
			continue;
		}
		#else
		if ( NO_ERROR == DRV_GetPortLinkStatus((UINT32) portS, &bPortStatus))
        {
            if ( TRUE == bPortStatus )
            {
                state = 1;
            }
            else
            {
                state = 0;
            }
        }
        else
        {
            continue;
        }
		#endif
		p_link->linkstate = state;
		g_usOutDataIdx += (sizeof(oam_instance_tlv_t) + sizeof(oam_eth_linkstate_t));
	}
	return;
}

/*** checked ok ***/
void eopl_get_onu_eth_port_pause(void)
{
	u8_t	rlt=0;
	u16_t portS, portE;
	u8_t *p_byte;
	u32_t	enable=1;
	oam_eth_portpause_t	*p_pause;
	oam_instance_tlv_t	*p_inst;

	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = CTC_MAX_SUBPORT_NUM;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		p_rsp = (oam_rsp_indication_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_ETH_PORTPAUSE;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += (sizeof(oam_instance_tlv_t) + sizeof(oam_rsp_indication_t));
		return;
	}


	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	for(; portS<=portE; portS++)
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;

		value = 0;
		value = OAM_SET_PORT_NUM(value, portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;
		p_byte += sizeof(oam_instance_tlv_t);

		p_pause = (oam_eth_portpause_t*)p_byte;
		p_pause->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_pause->leaf = OAM_ATTR_ETH_PORTPAUSE;
		p_pause->width = 1;
		rlt = odmPortFlowCtrolEnableGet(portS, &enable);
		if(rlt != OPL_OK)
		{
			continue;
		}
		p_pause->back_oper = enable;
		g_usOutDataIdx += (sizeof(oam_instance_tlv_t) + sizeof(oam_eth_portpause_t));
		OAMDBG(("odmPortFlowCtrolEnableGet success, g_usOutDataIdx = %d !\n", g_usOutDataIdx));
	}
	return;
}

void eopl_get_onu_eth_port_us_policing(void)
{
	u8_t enable=1, rlt=0;
	u16_t portS, portE;
	u8_t *p_byte, *p_temp;
	u32_t	cir=0xFF,cbs=0xFF,ebs=0xFF;
	oam_eth_portpolicing_t	*p_policing;
	oam_instance_tlv_t		*p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portId;
	u16_t portMax;

	OAMDBG(("[ enter %s ]\n", __FUNCTION__));

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portId = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = 1;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portId > portMax)&&(portId != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_rsp_indication_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_ETH_PORT_US_POLICING;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);

		return;
	}

	if(0xFFFF == portId)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portId;
		portE = portId;
	}

	for(; portS<=portE; portS++)
	{
		OAMDBG(("[ %s ]: port = %d\n", __FUNCTION__, portS));
		port_instance.value = portS;
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;

		value = 0;
		value = OAM_SET_PORT_NUM(value, portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;

		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_policing = (oam_eth_portpolicing_t*)p_byte;
		p_policing->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_policing->leaf = OAM_ATTR_ETH_PORT_US_POLICING;
		rlt = odmPortUsPolicingGet(portS, &enable,&cir,&cbs,&ebs);
		if(rlt != OPL_OK)
		{
			continue;
		}
		p_policing->operation = enable;
		if(enable == 0)
		{
			p_policing->width = 1;
		}
		else
		{
			p_policing->width = 10;
			p_temp = (u8_t*)&cir;
			vosMemCpy(p_policing->cir, &p_temp[1], 3);
			p_temp = (u8_t*)&cbs;
			vosMemCpy(p_policing->cbs, &p_temp[1], 3);
			p_temp = (u8_t*)&ebs;
			vosMemCpy(p_policing->ebs, &p_temp[1], 3);
		}
		g_usOutDataIdx += (4+p_policing->width);
	}
	return;
}

void eopl_get_onu_voip_port(void)
{
	u8_t *p_byte;
	oam_instance_tlv_t *p_inst;
	oam_voip_port_t *p_voip;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	u16_t portS;
	u16_t portE;
       u8_t tmp = 0;
    u16_t voip_port = 0;

	portMax = OAM_MAX_VOIP_PORT_ID;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

    #if 0
    if (portNum >= 0x50)
    #else
    if (portNum >= 0x50 && portNum != 0xFFFF)
    #endif
        voip_port = portNum - 0x50;
    else
        voip_port = portNum - odmSysVoIPPortBaseGet();
	OAMDBG(("[%s]: portType = %d, portNum = %d\n", __FUNCTION__, portType, portNum));

	if((portType != OAM_CTC_PORT_TYPE_VOIP) || ((voip_port > portMax) && (portNum != 0xFFFF)))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_rsp_indication_t *)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_VOIP_PORT;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		p_byte += sizeof(oam_rsp_indication_t);
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 0;
		portE = portMax;
	}
	else
	{
		portS = voip_port;
		portE = voip_port;
	}

	for(; portS<=portE; portS++)
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;

		value = 0;
		#if 0
		value = OAM_SET_PORT_NUM(value, portNum);
		#else
		if (0xFFFF == portNum)
		value = OAM_SET_PORT_NUM(value, portS);
    	else
    	    value = OAM_SET_PORT_NUM(value, portNum);
		#endif
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_VOIP);
		p_inst->value = value;
		p_byte += sizeof(oam_instance_tlv_t);

		p_voip = (oam_voip_port_t *)p_byte;
		p_voip->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_voip->leaf = OAM_ATTR_VOIP_PORT;
		p_voip->width = 1;
		stub_voip_port_get((UINT8)portS, &tmp);
		p_voip->lock = tmp;
		g_usOutDataIdx += (sizeof(oam_instance_tlv_t) + sizeof(oam_voip_port_t));
	}
	return;
}

void eopl_get_onu_e1_port(void)
{
	u8_t *p_byte;
	oam_instance_tlv_t *p_inst;
	oam_e1_port_t *p_e1;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	u8_t portS;
	u8_t portE;
	u16_t e1_port = 0;

	portMax = OAM_MAX_E1_PORT_ID;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

	if ( portNum >= 0x90 && portNum != 0xFFFF){
		e1_port = portNum - 0x90;
	}

	OAMDBG(("[%s]: portType = %d, portNum = %d\n", __FUNCTION__, portType, portNum));

	if((portType != OAM_CTC_PORT_TYPE_E1) || ((e1_port > portMax) && (portNum != 0xFFFF)))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_rsp_indication_t *)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_E1_PORT;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		p_byte += sizeof(oam_rsp_indication_t);
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 0;
		portE = portMax;
	}
	else
	{
		portS = e1_port;
		portE = e1_port;
	}

	for(; portS<=portE; portS++)
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;

		value = 0;
		value = OAM_SET_PORT_NUM(value, portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_E1);
		p_inst->value = value;
		p_byte += sizeof(oam_instance_tlv_t);

		p_e1 = (oam_e1_port_t *)p_byte;
		p_e1->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_e1->leaf = OAM_ATTR_E1_PORT;
		p_e1->width = 1;
		p_e1->lock = stub_e1_lock[portS];

		g_usOutDataIdx += (sizeof(oam_instance_tlv_t) + sizeof(oam_e1_port_t));
	}
	return;
}


void eopl_get_onu_eth_port_ds_policing(void)
{
	u8_t enable=1,rlt=0;
	u16_t portS, portE;
	u8_t *p_byte,*p_temp;
	u32_t	cir=0xFF,pir=0xFF;
	oam_eth_portdspolicing_t	*p_policing;
	oam_instance_tlv_t			*p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = 1;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);
		p_rsp = (oam_rsp_indication_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_ETH_PORT_DS_POLICING;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	for(; portS<=portE; portS++)
	{
		port_instance.value = portS;
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		value = 0;
		value = OAM_SET_PORT_NUM(value, portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);
		p_policing = (oam_eth_portdspolicing_t*)p_byte;
		p_policing->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_policing->leaf = OAM_ATTR_ETH_PORT_DS_POLICING;

		rlt = odmPortDsPolicingGet(portS, &enable,&cir,&pir);
		if(rlt != OPL_OK)
		{
			continue;
		}
		p_policing->operation = enable;
		if(enable == 0)
		{
			p_policing->width = 1;
		}
		else
		{
			p_policing->width = 7;
			p_temp = (u8_t*)&cir;
			vosMemCpy(p_policing->cir, &p_temp[1], 3);
			p_temp = (u8_t*)&pir;
			vosMemCpy(p_policing->pir, &p_temp[1], 3);
		}
		g_usOutDataIdx += (4+p_policing->width);
	}
	return;
}

/*** checked ok ***/
void eopl_get_onu_eth_phyadmin(void)
{
	u8_t rlt=0;
	u16_t portS, portE;
	u8_t *p_byte;
	u32_t state=0x00000002;
	oam_phy_adminstate_t	*p_admin;
	oam_instance_tlv_t		*p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = 1;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_rsp_indication_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_STANDARD_ATTRIB;
		p_rsp->leaf = OAM_ATTR_PHY_ADMIN;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}
	for(; portS<=portE; portS++)
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		value = 0;
		value = OAM_SET_PORT_NUM(value, portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_admin = (oam_phy_adminstate_t*)p_byte;
		p_admin->branch = OAM_BRANCH_STANDARD_ATTRIB;
		p_admin->leaf = OAM_ATTR_PHY_ADMIN;
		p_admin->width = 4;

		rlt = odmPortAdminGet(portS, &state);
		if(rlt != OPL_OK)
		{
			continue;
		}
		if(0 == state)
		{
			p_admin->value = OAM_STD_PHY_ADMIN_DISABLE;
		}
		else if(1 == state)
		{
			p_admin->value = OAM_STD_PHY_ADMIN_ENABLE;
		}
		else
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\r\nphyadmin state value get error\r\n");
		}

		g_usOutDataIdx += sizeof(oam_phy_adminstate_t);
	}
	return;
}

/*** checked ok ***/
void eopl_get_onu_eth_autonegadmin(void)
{
	u8_t rlt=0;
	u16_t portS, portE;
	u8_t *p_byte;
	u32_t state=0x00000002;
	oam_autoneg_adminstate_t	*p_admin;
	oam_instance_tlv_t			*p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = CTC_MAX_SUBPORT_NUM;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);
		p_rsp = (oam_rsp_indication_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_STANDARD_ATTRIB;
		p_rsp->leaf = OAM_ATTR_AUTONEG_ADMIN_STATE;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	for(; portS<=portE; portS++)
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst =   port_instance;
		value = 0;
		value = OAM_SET_PORT_NUM(value, portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);
		p_admin = (oam_autoneg_adminstate_t*)p_byte;
		p_admin->branch = OAM_BRANCH_STANDARD_ATTRIB;
		p_admin->leaf = OAM_ATTR_AUTONEG_ADMIN_STATE;
		p_admin->width = 4;
		#if 0
		rlt = odmPortAutoEnableGet(portS, &state);		
		if(rlt != OPL_OK)
		{
			continue;
		}
		#else
		state = (u32_t)m_abAutoNegEnable[portS];
		#endif
		if(0 == state)
		{
			p_admin->value = OAM_STD_AUTONEG_ADMIN_DISABLE;
		}
		else if(1 == state)
		{
			p_admin->value = OAM_STD_AUTONEG_ADMIN_ENABLE;
		}
		else
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\r\nautoneg state value get error\r\n");
		}

		g_usOutDataIdx += sizeof(oam_autoneg_adminstate_t);
	}
	return;
}

/*** checked ok ***/
void eopl_get_onu_eth_autoneglocal(void)
{
	u16_t portS, portE;
	u8_t *p_byte;
	oam_autoneg_localability_t	*p_local;
	oam_instance_tlv_t *p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = CTC_MAX_SUBPORT_NUM;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_rsp_indication_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_STANDARD_ATTRIB;
		p_rsp->leaf = OAM_ATTR_AUTONEG_LOCAL_ABILITY;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	for(; portS<=portE; portS++)
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		value = 0;
		value = OAM_SET_PORT_NUM(value, portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_local = (oam_autoneg_localability_t*)p_byte;
		p_local->branch = OAM_BRANCH_STANDARD_ATTRIB;
		p_local->leaf = OAM_ATTR_AUTONEG_LOCAL_ABILITY;
#if defined(ONU_1PORT)
        p_local->number = 0;
        switch ( g_enOnuBoardType)
    	{
    	    case ONU_BORAD_TYPE_EPN104:    	          
    		{
        		/* set the ablities of Marvell switch port */
                p_local->first = CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_TX;
                p_local->second= CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_TXFD;
                p_local->third = CTC_STACK_TECHNOLOGY_ABILITY_10_BASE_TFD;
                p_local->fourth = CTC_STACK_TECHNOLOGY_ABILITY_10_BASE_T;
                p_local->fifth = CTC_STACK_TECHNOLOGY_ABILITY_FDX_S_PAUSE;
                p_local->sixth  = CTC_STACK_TECHNOLOGY_ABILITY_FDX_A_PAUSE;
                /*begin modified by liaohongjun 2013/1/5 of EPN104QID0090*/
                #ifdef CONFIG_SWITCH_CHIP_RTL8365MB
                p_local->seventh = CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_T;
                p_local->eighth = CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_TFD;
                p_local->number = 8;
                #else
				p_local->number = 6;
                #endif
                /*end modified by liaohongjun 2013/1/5 of EPN104QID0090*/
                p_local->width = 4 *(1 + p_local->number) ;	
        		break;
    	    }
    		
    		default:
    			break;
        }		
#elif defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	/*Support 100BASE_T2FD*/
		p_local->number = 5;
		p_local->width = 4+p_local->number*4;
	/*should use the real value, later on*/
		p_local->first = 0x0000000E;//10BASE-T
		p_local->second = 0x0000008E;//10BASE-TFD
		p_local->third = 0x00000019;//100BASE-TX
		p_local->fourth = 0x000000FC;//100BASE-TXFD
		p_local->fifth = 0x00000138;//FDX-PAUSE
#endif
		g_usOutDataIdx += sizeof(oam_autoneg_localability_t);
	}
	return;
}

/*** checked ok ***/
void eopl_get_onu_eth_autonegadv(void)
{
	u16_t portS, portE;
	u8_t *p_byte;
	oam_autoneg_advability_t	*p_adv;
	oam_instance_tlv_t			*p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = CTC_MAX_SUBPORT_NUM;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_rsp_indication_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_STANDARD_ATTRIB;
		p_rsp->leaf = OAM_ATTR_AUTONEG_ADV_ABILITY;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	for(; portS<=portE; portS++)
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		value = 0;
		value = OAM_SET_PORT_NUM(value, portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_adv = (oam_autoneg_advability_t*)p_byte;
		p_adv->branch = OAM_BRANCH_STANDARD_ATTRIB;
		p_adv->leaf = OAM_ATTR_AUTONEG_ADV_ABILITY;
#if defined(ONU_1PORT)
	    p_adv->number = 0;
        switch ( g_enOnuBoardType)
    	{
    	    case ONU_BORAD_TYPE_EPN104:    	           
    		{
        		/* set the ablities of Marvell switch port */
                p_adv->first = CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_TX;
                p_adv->second= CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_TXFD;
                p_adv->third = CTC_STACK_TECHNOLOGY_ABILITY_10_BASE_TFD;
                p_adv->fourth = CTC_STACK_TECHNOLOGY_ABILITY_10_BASE_T;
                p_adv->fifth = CTC_STACK_TECHNOLOGY_ABILITY_FDX_S_PAUSE;
                p_adv->sixth  = CTC_STACK_TECHNOLOGY_ABILITY_FDX_A_PAUSE;
                /*begin modified by liaohongjun 2013/1/5 of EPN104QID0090*/
                #ifdef CONFIG_SWITCH_CHIP_RTL8365MB
                p_adv->seventh = CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_T;
                p_adv->eighth = CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_TFD;
                p_adv->number = 8;
                #else
				p_adv->number = 6;
                #endif
                /*end modified by liaohongjun 2013/1/5 of EPN104QID0090*/
                p_adv->width = 4 *(1 + p_adv->number) ;	
        		break;
    	    }
    		
    		default:
    			break;
        }				
#elif defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	/*Support 100BASE_T2FD*/
		p_adv->number = 5;
		p_adv->width = 4+p_adv->number*4;
	/*should use the real value, later on*/
		p_adv->first = 0x0000000E;//10BASE-T
		p_adv->second = 0x0000008E;//10BASE-TFD
		p_adv->third = 0x00000019;//100BASE-TX
		p_adv->fourth = 0x000000FC;//100BASE-TXFD
		p_adv->fifth = 0x00000138;//FDX-PAUSE
		//p_adv->sixth = 0x00000028;//1000BASE-T
		//p_adv->seventh = 0x00000192;//1000BASE-TFD
#endif

		g_usOutDataIdx += sizeof(oam_autoneg_advability_t);
	}
	return;
}

void eopl_get_onu_eth_fecability(void)
{
	u8_t	*p_byte;
	oam_fec_capability_t	*p_fec;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_fec = (oam_fec_capability_t*)p_byte;
	p_fec->branch = OAM_BRANCH_STANDARD_ATTRIB;
	p_fec->leaf = OAM_ATTR_FEC_ABILITY;
	p_fec->width = 4;
	/* use a fixed value currently, change later on */
	p_fec->indication = 0x00000002;

	g_usOutDataIdx += sizeof(oam_fec_capability_t);
	return;
}

void eopl_get_onu_eth_fecmode(void)
{
	u32_t mode=0;
	u8_t rlt=0;
	u8_t *p_byte;
	oam_fec_mode_t *p_fec;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_fec = (oam_fec_mode_t*)p_byte;
	p_fec->branch = OAM_BRANCH_STANDARD_ATTRIB;
	p_fec->leaf = OAM_ATTR_FEC_MODE;
	p_fec->width = 4;
	/*shawn modified for fixing bug 3275*/
	/*rlt = odmPonFecEnableGet(&mode);*/
	rlt = oplRegRead(0x17*4, &mode);
	if(rlt != OPL_OK)
	{
		return;
	}

	if(3 == mode)/*fec mode:enable*/
	{
		p_fec->value = 2;
	}
	else if(0 == mode)/*fec mode:disable*/
	{
		p_fec->value = 3;
	}
	else/*fec mode:unknown*/
	{
		p_fec->value = 1;
	}
/*
	if(0 == mode)
	{
		p_fec->value = 3;
	}
	else if(1 == mode)
	{
		p_fec->value = 2;
	}
	else
	{
		p_fec->value = 1;
	}
*/
	g_usOutDataIdx += sizeof(oam_fec_mode_t);
	return;
}

void eopl_get_onu_port_loop_detect(void)
{
	u32_t enable=1;
	u8_t rlt=0;
	u16_t portS, portE;
	u8_t *p_byte,*p_temp;
	oam_port_loop_detect_t	*p_loop;
	oam_instance_tlv_t			*p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = 1;
#endif
	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}
		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);
		p_rsp = (oam_rsp_indication_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_PORT_LOOP_DETECT;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		return;
	}
	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}
	for(; portS<=portE; portS++)
	{
		port_instance.value = portS;
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}
		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		value = 0;
		value = OAM_SET_PORT_NUM(value, portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);
		p_loop = (oam_port_loop_detect_t *)p_byte;
		p_loop->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_loop->leaf = OAM_ATTR_PORT_LOOP_DETECT;
		rlt = odmLoopDetectionGet(portS, &enable);
		if(rlt != OPL_OK)
		{
			continue;
		}
		if (0 == enable)
		{
			p_loop->state = 1;	/* loop disactivated */
		}
		else
		{
			p_loop->state = 2;  /* loop activated */
		}

		p_loop->width = 4;

		g_usOutDataIdx += sizeof(oam_port_loop_detect_t);
	}
	return;
}


void stub_vlan_aggr_entry_add(u16_t port_id, u32_t new_dst_vlan, u32_t new_src_vlan)
{
	u16_t num_of_aggr_table;
	u16_t num_of_src_vlan;
	u16_t i;
	u16_t j;
	u32_t dst_vlan;
	u32_t src_vlan;

	num_of_aggr_table = stub_aggr_vlan_db[port_id].num_of_aggr_table;

	for(i = 0; i < num_of_aggr_table; i++)
	{
		dst_vlan = stub_aggr_vlan_db[port_id].aggr_table[i].dst_vlan;

		if(new_dst_vlan == dst_vlan)
		{
			/* dst_vlan already in database */
			num_of_src_vlan = stub_aggr_vlan_db[port_id].aggr_table[i].num_of_src_vlan;

			for(j = 0; j < num_of_src_vlan; j++)
			{
				src_vlan = stub_aggr_vlan_db[port_id].aggr_table[i].src_vlan[j];

				if(new_src_vlan == src_vlan)
				{
					/* already in vlan aggr db, exit */
					return;
				}

			}

			if(j == num_of_src_vlan)/* new_src_vlan not in database, create it */
			{
				stub_aggr_vlan_db[port_id].aggr_table[i].src_vlan[j] = new_src_vlan;
				stub_aggr_vlan_db[port_id].aggr_table[i].num_of_src_vlan += 1;

				return;
			}
		}
	}

	if(i == num_of_aggr_table)
	{

			stub_aggr_vlan_db[port_id].aggr_table[num_of_aggr_table].dst_vlan = new_dst_vlan;
			stub_aggr_vlan_db[port_id].aggr_table[num_of_aggr_table].src_vlan[0] = new_src_vlan;
			stub_aggr_vlan_db[port_id].aggr_table[num_of_aggr_table].num_of_src_vlan = 1;

			num_of_aggr_table += 1;
			stub_aggr_vlan_db[port_id].num_of_aggr_table = num_of_aggr_table;
	}

	return;
}

int eopl_get_onu_vlan(void)
{
	u8_t	number, rlt=0;
	u16_t   portS, portE;
	u8_t    *p_out;
	u32_t	tag, mode=1;
	oam_vlan_config_t	*p_vlan_config;
	oam_instance_tlv_t	*p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	u16_t i;
	u16_t j;
	u16_t num_of_src_vlan;
	VLAN_AGG_DS_t *p_vlan_aggr_entry;
	u32_t defaultTag;
	u32_t vlanTrunkNum;
	u32_t vlanId;
	u32_t retVal;
	u32_t length;
	u8_t  vlanmode;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = CTC_MAX_SUBPORT_NUM;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_out;
		*p_inst = port_instance;
		p_out += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_rsp_indication_t*)p_out;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_VLAN;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		return 0;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	for(; portS<=portE; portS++)
	{
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		value = OAM_SET_PORT_NUM(value, portS);

		port_instance.value = value;
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_out;
		*p_inst =  port_instance;
		value = 0;
		value = OAM_SET_PORT_NUM(value,portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;

		p_out += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_vlan_config = (oam_vlan_config_t*)p_out;
		p_out += sizeof(oam_vlan_config_t);

		p_vlan_config->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_vlan_config->leaf = OAM_ATTR_VLAN;

		#if defined(CONFIG_PRODUCT_EPN200)
		rlt = odmPortVlanModeGet(portS, &mode);		
		if(rlt != OPL_OK)
		{
			OAMDBG(("[ %s ]: mode = %d\n", __FUNCTION__, mode));
			return -1;
		}

		/* p_vlan_config->mode = stub_vlan_mode;*/
		p_vlan_config->mode = mode;
		p_vlan_config->width = 1;

		if(OAM_CTC_VLAN_TRANSPARENT == p_vlan_config->mode)
		{
			OAMDBG(("[ %s ]: OAM_CTC_VLAN_TRANSPARENT\n", __FUNCTION__));
			p_vlan_config->width = 1;
			g_usOutDataIdx += sizeof(oam_vlan_config_t);
		}
		else if(OAM_CTC_VLAN_TAG == p_vlan_config->mode)
		{

			OAMDBG(("[ %s ]: OAM_CTC_VLAN_TAG\n", __FUNCTION__));
			if(OPL_OK != eopl_get_tag_id(portS, &tag))
			{
				return -1;
			}

			vosMemCpy(p_out,(u8_t*)&tag, 4);
			p_out += 4;

			p_vlan_config->width += 4;
			g_usOutDataIdx += sizeof(oam_vlan_config_t) + 4;
		}
		else if(OAM_CTC_VLAN_TRANSLATE == p_vlan_config->mode)
		{
			OAMDBG(("[ %s ]: OAM_CTC_VLAN_TRANSLATE\n", __FUNCTION__));
			if(OPL_OK != eopl_get_tag_id(portS, &tag))
			{
				return -1;
			}

			vosMemCpy(p_out,(u8_t*)&tag, 4);
			p_out += 4;

			if(OPL_OK != eopl_get_vtt_list(portS, p_out, &number, tag))
			{
				return -1;
			}

			p_vlan_config->width += (4 + 8 * number);
			g_usOutDataIdx += (sizeof(oam_vlan_config_t) + 4 + 8 * number);
		}
		else if(OAM_CTC_VLAN_AGGREGATION == p_vlan_config->mode)
		{
			OAMDBG(("[ %s ]: OAM_CTC_VLAN_AGGREGATION\n", __FUNCTION__));

			g_usOutDataIdx += sizeof(oam_vlan_config_t);

			/* get default vlan tag */
			eopl_get_tag_id(portS, &tag);

			vosMemCpy(p_out, (u8_t *)&tag, 4);
			p_out += sizeof(u32_t);
			g_usOutDataIdx += sizeof(u32_t);
			p_vlan_config->width += sizeof(u32_t);

			/* call api to get vlan aggr entries */
			p_vlan_aggr_entry = (VLAN_AGG_DS_t *)odmVlanAggGet();

			stub_aggr_vlan_db[portS].num_of_aggr_table = 0;

			if(p_vlan_aggr_entry){
				/* traverse through all vlan aggr entries */
				for(i=0; i < ODM_MAX_NUM_OF_AGGRE_ENTRY; i++)
				{
					if(p_vlan_aggr_entry->egressPort == portS)
					{
						stub_vlan_aggr_entry_add(portS, p_vlan_aggr_entry->targetVlan, p_vlan_aggr_entry->aggedVlan);
					}
					p_vlan_aggr_entry++;
				}
			}
			#if 0
			printf("num_of_aggr_table = %d\n", stub_aggr_vlan_db[portS].num_of_aggr_table);
			for(i = 0; i < stub_aggr_vlan_db[portS].num_of_aggr_table; i++)
			{
				printf("------------------------------------\n");
				printf("dst_vlan = %d\n", stub_aggr_vlan_db[portS].aggr_table[i].dst_vlan);
				num_of_src_vlan = stub_aggr_vlan_db[portS].aggr_table[i].num_of_src_vlan;
				printf("num_of_src_vlan = %d\n", num_of_src_vlan);
				for(j = 0; j < num_of_src_vlan; j++)
				{
					printf("src_vlan_%d = %d\n", j, stub_aggr_vlan_db[portS].aggr_table[i].src_vlan[j]);
				}
			}
			printf("------------------------------------\n");
			#endif

			/* generate response packet */
			vosMemCpy(p_out, (u8_t *)(&stub_aggr_vlan_db[portS].num_of_aggr_table), sizeof(u16_t));
			p_out += sizeof(u16_t);
			g_usOutDataIdx += sizeof(u16_t);
			p_vlan_config->width += sizeof(u16_t);

			for(i = 0; i < stub_aggr_vlan_db[portS].num_of_aggr_table; i++)
			{
				num_of_src_vlan = stub_aggr_vlan_db[portS].aggr_table[i].num_of_src_vlan;
				vosMemCpy(p_out, (u8_t *)(&num_of_src_vlan), sizeof(u16_t));
				p_out += sizeof(u16_t);
				g_usOutDataIdx += sizeof(u16_t);
				p_vlan_config->width += sizeof(u16_t);

				vosMemCpy(p_out, (u8_t *)(&stub_aggr_vlan_db[portS].aggr_table[i].dst_vlan), sizeof(u32_t));
				p_out += sizeof(u32_t);
				g_usOutDataIdx += sizeof(u32_t);
				p_vlan_config->width += sizeof(u32_t);

				for(j = 0; j <= num_of_src_vlan; j++)
				{
					vosMemCpy(p_out, (u8_t *)(&stub_aggr_vlan_db[portS].aggr_table[i].src_vlan[j]), sizeof(u32_t));
					p_out += sizeof(u32_t);
					g_usOutDataIdx += sizeof(u32_t);
					p_vlan_config->width += sizeof(u32_t);
				}
			}
		}
		else if(OAM_CTC_VLAN_TRUNK == p_vlan_config->mode)
		{
			OAMDBG(("[ %s ]: OAM_CTC_VLAN_TRUNK\n", __FUNCTION__));

            g_usOutDataIdx += sizeof(oam_vlan_config_t);
			/* get default vlan tag */
			eopl_get_tag_id(portS, &defaultTag);
			vosMemCpy(p_out, (u8_t *)&defaultTag, 4);

			p_out += sizeof(u32_t);
			g_usOutDataIdx += sizeof(u32_t);
			p_vlan_config->width += sizeof(u32_t);

			/* get vlan trunk entry */
			odmPortVlanTrunkEntryNumGet(portS, &vlanTrunkNum);

			for(i = 0; i < ODM_MAX_NUM_OF_TRUNK_VLAN; i++)
			{
				retVal = odmPortVlanTrunkEntryValueGet(portS, i, &vlanId);

				if(retVal != OK)
				{
					continue;
				}
				
				vosMemCpy(p_out, (u8_t *)&vlanId, sizeof(u32_t));

				p_out += sizeof(u32_t);
				g_usOutDataIdx += sizeof(u32_t);
				p_vlan_config->width += sizeof(u32_t);
			}
		}
		#else
        (VOID)DRV_GetPortVlanCfg(portS, &vlanmode, p_out, &length);
		p_vlan_config->mode = vlanmode;
		p_vlan_config->width += length + 1; 
	    g_usOutDataIdx += sizeof(oam_vlan_config_t) + p_vlan_config->width;		
		#endif
	}
	return 0;
}

u8_t stub_ctc_port_rule_get(u8_t portId,
  	                      u8_t precedenceOfRule,
	                      oam_clasmark_rulebody_t *pClsRule,
	                      oam_clasmark_fieldbody_t *pClsField)
{
	u32_t ret;
	CLS_CONFIG_INFO_t odmClsRule;
	u8_t numOfField = 0;

	vosMemSet(&odmClsRule, 0, sizeof(CLS_CONFIG_INFO_t));
	OP_DEBUG(DEBUG_LEVEL_INFO, "call odmQosClsRuleGet(%d, %d, odmClsRule)\n", portId, precedenceOfRule);
	ret = odmoamClassRuleGet(portId, precedenceOfRule, &odmClsRule);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_INFO, "call odmQosClsRuleGet(%d, %d, odmClsRule) fail, rule not exist!\n", portId, precedenceOfRule);

		return OAM_GET_NO_RESOURCE;
	}

	OP_DEBUG(DEBUG_LEVEL_INFO,"oam for precedence %d queue %d pri %d \n",odmClsRule.rulePri,
		odmClsRule.queueMapId,odmClsRule.cosMapVal);

	pClsRule->precedenceOfRule = odmClsRule.rulePri;
	pClsRule->queueMapped = odmClsRule.queueMapId;
	pClsRule->ethPriMark = odmClsRule.cosMapVal;

	if(odmClsRule.dstMacFlag)
	{
		pClsField->fieldSelect = OAM_CTC_CLS_FIELD_TYPE_DA_MAC;
		pClsField->operator = odmClsRule.dstMacOp;
		vosMemCpy(&(pClsField->matchValue[0]), &(odmClsRule.dstMac.lowRange[0]), 6);

		OP_DEBUG(DEBUG_LEVEL_INFO,"[ %s ]: dstMac=%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", __FUNCTION__, pClsField->matchValue[0], pClsField->matchValue[1],
				pClsField->matchValue[2], pClsField->matchValue[3], pClsField->matchValue[4], pClsField->matchValue[5]);

		pClsField++;
		numOfField++;
	}

	if(odmClsRule.srcMacFlag)
	{
		pClsField->fieldSelect = OAM_CTC_CLS_FIELD_TYPE_SA_MAC;
		pClsField->operator = odmClsRule.srcMacOp;
		vosMemCpy(&(pClsField->matchValue[0]), &(odmClsRule.srcMac.lowRange[0]), 6);
		OP_DEBUG(DEBUG_LEVEL_INFO,"[ %s ]: srcMac=%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", __FUNCTION__, pClsField->matchValue[0], pClsField->matchValue[1],
				pClsField->matchValue[2], pClsField->matchValue[3], pClsField->matchValue[4], pClsField->matchValue[5]);

		pClsField++;
		numOfField++;
	}

	if(odmClsRule.etherPriFlag)
	{
		pClsField->fieldSelect = OAM_CTC_CLS_FIELD_TYPE_ETH_PRI;
		pClsField->operator = odmClsRule.etherPriOp;
		pClsField->matchValue[5] = odmClsRule.etherPri.lowRange;
		OP_DEBUG(DEBUG_LEVEL_INFO,"[ %s ]: ethPri = %d\n", __FUNCTION__, pClsField->matchValue[5]);

		pClsField++;
		numOfField++;
	}

	if(odmClsRule.vlanIdFlag)
	{
		pClsField->fieldSelect = OAM_CTC_CLS_FIELD_TYPE_VLAN_ID;
		pClsField->operator = odmClsRule.vlanIdOp;

		pClsField->matchValue[5] = odmClsRule.vlanId.lowRange & 0x00FF;
		pClsField->matchValue[4] = (odmClsRule.vlanId.lowRange & 0x0F00) >> 8;

		OP_DEBUG(DEBUG_LEVEL_INFO,"[ %s ]: vlanId = 0X%.2x %.2x\n",__FUNCTION__, pClsField->matchValue[4], pClsField->matchValue[5]);

		pClsField++;
		numOfField++;
	}

	if(odmClsRule.lenOrTypeFlag)
	{

		pClsField->fieldSelect = OAM_CTC_CLS_FIELD_TYPE_ETHER_TYPE;
		pClsField->operator = odmClsRule.lenOrTypeOp;
        pClsField->matchValue[4] = (odmClsRule.lenOrType.lowRange & 0xFF00 )>> 8;
        pClsField->matchValue[5] = odmClsRule.lenOrType.lowRange & 0x00FF;

		OP_DEBUG(DEBUG_LEVEL_INFO,"[ %s ]: EtherType = 0X%x,%x\n", __FUNCTION__, pClsField->matchValue[4],pClsField->matchValue[5]);
		pClsField++;
		numOfField++;
	}

	if(odmClsRule.dstIpFlag)
	{
		pClsField->fieldSelect = OAM_CTC_CLS_FIELD_TYPE_DST_IP;
		pClsField->operator = odmClsRule.dstIpOp;

		pClsField->matchValue[2] = (odmClsRule.dstIp.lowRange & 0xFF000000) >> 24;
		pClsField->matchValue[3] = (odmClsRule.dstIp.lowRange & 0x00FF0000 )>> 16;
		pClsField->matchValue[4] = (odmClsRule.dstIp.lowRange & 0x0000FF00 )>> 8;
		pClsField->matchValue[5] = odmClsRule.dstIp.lowRange & 0x000000FF;

		OP_DEBUG(DEBUG_LEVEL_INFO,"[ %s ]: dstIp = %x.%x.%x.%x\n", __FUNCTION__,pClsField->matchValue[2], pClsField->matchValue[3],
			pClsField->matchValue[4], pClsField->matchValue[5]);

		pClsField++;
		numOfField++;
	}

	if(odmClsRule.srcIpFlag)
	{
		pClsField->fieldSelect = OAM_CTC_CLS_FIELD_TYPE_SRC_IP;
		pClsField->operator = odmClsRule.srcIpOp;

		pClsField->matchValue[2] = (odmClsRule.srcIp.lowRange & 0xFF000000) >> 24;
		pClsField->matchValue[3] = (odmClsRule.srcIp.lowRange & 0x00FF0000)>> 16;
		pClsField->matchValue[4] = (odmClsRule.srcIp.lowRange & 0x0000FF00) >> 8;
		pClsField->matchValue[5] = odmClsRule.srcIp.lowRange & 0x000000FF;
		OP_DEBUG(DEBUG_LEVEL_INFO,"[ %s ]: srcIp = %x.%x.%x.%x\n", __FUNCTION__,pClsField->matchValue[2], pClsField->matchValue[3],
			pClsField->matchValue[4], pClsField->matchValue[5]);
		pClsField++;
		numOfField++;
	}

	if(odmClsRule.ipTypeFlag)
	{
		pClsField->fieldSelect = OAM_CTC_CLS_FIELD_TYPE_IP_TYPE;
		pClsField->operator = odmClsRule.ipTypeOp;

		pClsField->matchValue[5] = odmClsRule.ipType.lowRange;
		OP_DEBUG(DEBUG_LEVEL_INFO,"[ %s ]: IPType = %d\n", __FUNCTION__, pClsField->matchValue[5]);

		pClsField++;
		numOfField++;
	}

	if(odmClsRule.ipV4DscpFlag)
	{
		pClsField->fieldSelect = OAM_CTC_CLS_FIELD_TYPE_IP_DSCP;
		pClsField->operator = odmClsRule.ipV4DscpOp;

		pClsField->matchValue[5] = (odmClsRule.ipV4Dscp.lowRange)>>2;
		OP_DEBUG(DEBUG_LEVEL_INFO,"[ %s ]: DSCP = %d\n", __FUNCTION__, pClsField->matchValue[5]);
		pClsField++;
		numOfField++;
	}

	if(odmClsRule.ipV6PreFlag)
	{

		pClsField->fieldSelect = OAM_CTC_CLS_FIELD_TYPE_IP_PRECEDENCE;
		pClsField->operator = odmClsRule.ipV6PreOp;
		pClsField->matchValue[5] = odmClsRule.ipV6Pre.lowRange;

		pClsField++;
		numOfField++;
	}

	if(odmClsRule.srcL4PortFlag)
	{
		pClsField->fieldSelect = OAM_CTC_CLS_FIELD_TYPE_L4_SRC_PORT;
		pClsField->operator = odmClsRule.srcL4PortOp;
		pClsField->matchValue[4] = (odmClsRule.srcL4Port.lowRange & 0xFF00) >> 8;
		pClsField->matchValue[5] = odmClsRule.srcL4Port.lowRange & 0x00FF;

		OP_DEBUG(DEBUG_LEVEL_INFO,"[ %s ]: srcL4Port = 0X%.x ,%.x\n", __FUNCTION__, pClsField->matchValue[4],pClsField->matchValue[5]);

		pClsField++;
		numOfField++;
	}

	if(odmClsRule.dstL4PortFlag)
	{

		pClsField->fieldSelect = OAM_CTC_CLS_FIELD_TYPE_L4_DST_PORT;
		pClsField->operator = odmClsRule.dstL4PortOp;

		pClsField->matchValue[4] = (odmClsRule.dstL4Port.lowRange & 0xFF00) >> 8;
		pClsField->matchValue[5] = odmClsRule.dstL4Port.lowRange & 0x00FF;

		OP_DEBUG(DEBUG_LEVEL_INFO,"[ %s ]: dstL4Port = 0X%.x, %.x\n", __FUNCTION__, pClsField->matchValue[4],pClsField->matchValue[5]);
		pClsField++;
		numOfField++;
	}

	pClsRule->numOfField = numOfField;
	pClsRule->lenOfRule = 3 + numOfField * sizeof(oam_clasmark_fieldbody_t);

	return OAM_GET_OK;
}
void eopl_get_onu_classify(void)
{
	u8_t size, rlt=0, ucRuleNum=0, ucFieldNum=0;
	u16_t portS, portE;
	u8_t ucRuleIdx, ucFieldIdx, *p_byte;
	u16_t usLen=0;
	oam_clasmark_t				*pClsHeader;;
	oam_clasmark_rulebody_t		*pClsRule;
	oam_clasmark_fieldbody_t	*pClsField;
	oam_instance_tlv_t			*p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	oam_clasmark_rulebody_t  clsRule;
	oam_clasmark_fieldbody_t clsFields[12];
	u8_t precedenceOfRule;
	u8_t numOfRule = 0;
	u8_t numOfField;
	u8_t fieldId;
	u8_t curPortId;
	u32_t ret;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);


	portMax = CTC_MAX_SUBPORT_NUM;


	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		p_rsp = (oam_rsp_indication_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_CLAMARK;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += (sizeof(oam_instance_tlv_t) + sizeof(oam_rsp_indication_t));
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	for(curPortId = portS; curPortId <= portE; curPortId++)
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst =   port_instance;
		value = 0;
		value = OAM_SET_PORT_NUM(value,curPortId);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		pClsHeader = (oam_clasmark_t *)p_byte;
		pClsHeader->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		pClsHeader->leaf = OAM_ATTR_CLAMARK;
		pClsHeader->width = 2; /* action(1Byte) +  numOfRule(1Byte) */
		p_byte += sizeof(oam_clasmark_t);

		for(precedenceOfRule = 1; precedenceOfRule <= 8; precedenceOfRule++)
		{

			memset(&clsRule, 0, sizeof(oam_clasmark_rulebody_t));
			memset(clsFields, 0, sizeof(oam_clasmark_fieldbody_t)*12);
			ret = stub_ctc_port_rule_get((u8_t)curPortId, precedenceOfRule, &clsRule, &clsFields[0]);
			if(ret != OAM_GET_OK)
			{
				/* rule with this precedence not exist */
				continue;
			}

			numOfRule++;

			/* add cls rule entry */
			pClsRule = (oam_clasmark_rulebody_t *)p_byte;
			vosMemCpy(pClsRule, &clsRule, sizeof(oam_clasmark_rulebody_t));
			p_byte += sizeof(oam_clasmark_rulebody_t);

			/* add fileds entry */
			pClsField = (oam_clasmark_fieldbody_t *)p_byte;
			numOfField = pClsRule->numOfField;
			vosMemCpy(pClsField, & clsFields, numOfField * sizeof(oam_clasmark_fieldbody_t));
			p_byte += numOfField * sizeof(oam_clasmark_fieldbody_t);

			pClsHeader->width += pClsRule->lenOfRule + 2;
		}

		pClsHeader->action = OAM_CLASSIFY_ACTION_SHOW;
		pClsHeader->numOfRule = numOfRule;

		g_usOutDataIdx += pClsHeader->width + 4;
	}

	return;
}

void eopl_get_onu_mcast_vlan(void)
{
	u8_t rlt=0;
	u16_t portS, portE;
	u8_t number;
	u8_t *p_out;
	u16_t *p_vid;
	oam_mcast_vlan_t *p_mcast_vlan;
	oam_instance_tlv_t *p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
  
    #ifdef CTC_MULTICAST_SURPORT
    portMax = 4;
    #else
	portMax = 1;
    #endif
   
#endif

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	if((portType != OAM_CTC_PORT_TYPE_ETH) || (portE > portMax))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_out;
		*p_inst = port_instance;
		p_out += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_rsp_indication_t *)p_out;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_MCAST_VLAN;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		return;
	}
	else
	{

		for(; portS<=portE; portS++)
		{
			if(0 == g_usOutDataIdx)
			{
				memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
				eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
				p_out = &out_data[0];
				p_out += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
				g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			}
			else
			{
				p_out = &out_data[g_usOutDataIdx];
			}

			p_inst = (oam_instance_tlv_t *)p_out;
			*p_inst =   port_instance;
			value = 0;
			value = OAM_SET_PORT_NUM(value,portS);
			value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
			p_inst->value = value;
			p_out += sizeof(oam_instance_tlv_t);
			g_usOutDataIdx += sizeof(oam_instance_tlv_t);

			p_mcast_vlan = (oam_mcast_vlan_t*)p_out;
			p_mcast_vlan->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_mcast_vlan->leaf = OAM_ATTR_MCAST_VLAN;
			p_mcast_vlan->operation = OAM_CTC_MCAST_VLAN_LIST;
			p_out += sizeof(oam_mcast_vlan_t);
			g_usOutDataIdx += sizeof(oam_mcast_vlan_t);

			p_vid = (u16_t*)p_out;
			rlt = odmMulticastVlanGet(portS, p_vid, &number);
			if(0 != rlt)
			{
				OAMDBG(("[ %s ]: odmMulticastVlanGet return error!\n", __FUNCTION__));
				number = 0;
			}
			OAMDBG(("[ %s ]: mcalst vlan num = %d\n", __FUNCTION__, number));

			p_mcast_vlan->width = 1+2*number;

			g_usOutDataIdx += (2 * number);
		}
	}
	return;
}

void eopl_get_onu_mcast_tagstrip(void)
{
	u8_t *p_byte;
	u16_t portS, portE;
	oam_mcast_tagstripe_t *p_tagstripe;
	oam_instance_tlv_t *p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	u8_t  *p_num_of_mc_vlan;
	u8_t  num_of_mc_vlan;
	oam_mcast_vlan_translation_entry_t *p_tranlation_entry;
	u32_t ret;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = 1;
#endif
	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax) && (portNum != 0xFFFF)))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_rsp_indication_t *)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_MCAST_TAG_STRIP;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	for(; portS<=portE; portS++)
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst =   port_instance;
		value = 0;
		value = OAM_SET_PORT_NUM(value,portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_tagstripe = (oam_mcast_tagstripe_t*)p_byte;
		p_tagstripe->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_tagstripe->leaf = OAM_ATTR_MCAST_TAG_STRIP;
		p_byte += sizeof(oam_mcast_tagstripe_t);

		OP_DEBUG(DEBUG_LEVEL_INFO, "call odmMulticastTagstripGet(), portId = %d", portS);
		ret = odmMulticastTagstripGet(portS, &(p_tagstripe->tagstriped));
		if(ret != OPL_OK)
		{
			OP_DEBUG(DEBUG_LEVEL_INFO, "call odmMulticastTagstripGet() fail!\n");
		}
		OP_DEBUG(DEBUG_LEVEL_INFO, "call odmMulticastTagstripGet() successfully, tagstriped = %d\n", p_tagstripe->tagstriped);
		if(OAM_CTC_MCAST_VLAN_TAG_TRANLATE == p_tagstripe->tagstriped)
		{
			/* not support now */
			p_num_of_mc_vlan = (u8_t *)p_byte;
			p_byte += sizeof(u8_t);
			p_tranlation_entry = (oam_mcast_vlan_translation_entry_t *)p_byte;
			OP_DEBUG(DEBUG_LEVEL_INFO, "call odmMulticastTagTranslationGet(), portId = %d", portS);
			ret = odmMulticastTagTranslationGet(portS, p_tranlation_entry, &num_of_mc_vlan);
			if(ret != OPL_OK)
			{
				OP_DEBUG(DEBUG_LEVEL_INFO, "call odmMulticastTagTranslationGet() fail!\n");
			}
			OP_DEBUG(DEBUG_LEVEL_INFO, "call odmMulticastTagTranslationGet() successfully, num_of_mc_vlan = %d\n", num_of_mc_vlan);

			p_tagstripe->width = 2 + 4 * num_of_mc_vlan;

			*p_num_of_mc_vlan = num_of_mc_vlan;

			g_usOutDataIdx += p_tagstripe->width + 4;
		}
		else
		{
			p_tagstripe->width = 0x1;
			g_usOutDataIdx += p_tagstripe->width + 4;
		}
	}
	return;
}

void eopl_get_onu_mcast_switch(void)
{
	u8_t *p_byte;
	oam_mcast_switch_t *p_switch;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_switch = (oam_mcast_switch_t*)p_byte;
	p_switch->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_switch->leaf = OAM_ATTR_MCAST_SWITCH;
	p_switch->width = 1;
	p_switch->action = odmMulticastSwitchGet();
	g_usOutDataIdx += sizeof(oam_mcast_switch_t);
}

void eopl_get_onu_mcast_control(void)
{
	u16_t number;
	u8_t *p_byte;
	oam_mcast_control_t	*p_mcast_control;
	oam_mcast_control_entry_t *p_mcast_entry_src=0;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_mcast_control = (oam_mcast_control_t*)p_byte;
	p_mcast_control->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_mcast_control->leaf = OAM_ATTR_MCAST_CONTROL;
	p_mcast_control->control_type = (u8_t)odmMulticastControlTypeGet();
	p_mcast_control->action = OAM_CTC_MCAST_ENTRY_LIST;
	p_byte += sizeof(oam_mcast_control_t);

	p_mcast_entry_src = (oam_mcast_control_entry_t*)p_byte;
	odmMulticastControlEntryGet((multicast_control_entry_t *)p_mcast_entry_src, &number);
	p_mcast_control->width = 3+10*number;
	p_mcast_control->number = number;

	g_usOutDataIdx += (sizeof(oam_mcast_control_t) + sizeof(oam_mcast_control_entry_t) * number);
}

void eopl_get_onu_mcast_grp_nummax(void)
{
	u16_t portS, portE;
	u8_t *p_byte;
	oam_group_num_max_t *p_group;
	oam_instance_tlv_t *p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = 1;
#endif


	if(0xFFFF == portS)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	if((portType != OAM_CTC_PORT_TYPE_ETH) || (portE > portMax))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_rsp_indication_t *)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_MCAST_GRP_NUMMAX;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);
	}
	else
	{
		for(; portS<=portE; portS++)
		{
			if(0 == g_usOutDataIdx)
			{
				memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
				eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
				p_byte = &out_data[0];
				p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
				g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			}
			else
			{
				p_byte = &out_data[g_usOutDataIdx];
			}

			p_inst = (oam_instance_tlv_t *)p_byte;
			*p_inst =	port_instance;
			value = 0;
			value = OAM_SET_PORT_NUM(value,portS);
			value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
			p_inst->value = value;
			p_byte += sizeof(oam_instance_tlv_t);
			p_group = (oam_group_num_max_t*)p_byte;
			p_group->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_group->leaf = OAM_ATTR_MCAST_GRP_NUMMAX;
			p_group->width = 1;
			p_group->num_max = odmMulticastGroupMaxNumGet(portS);
			g_usOutDataIdx += (sizeof(oam_instance_tlv_t) + sizeof(oam_group_num_max_t));
		}
	}

	return;
}

void eopl_get_onu_mcast_fast_leave_abl(void)
{
	u8_t ucIdx;
	u8_t *p_byte;
	oam_fastleave_abl_t	*p_fast;
	u32_t *p_val;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_fast = (oam_fastleave_abl_t*)p_byte;
	p_fast->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_fast->leaf = OAM_ATTR_MCAST_GFASTLEAVE_ABL;
	p_fast->width = 4 + 4*4;
	p_fast->num_of_mode = 4;
	p_fast->mode_1 = OAM_CTC_IGMP_SNOOPING_NOT_FAST_LEAVE;
	p_fast->mode_2 = OAM_CTC_IGMP_SNOOPING_FAST_LEAVE;
	p_fast->mode_3 = OAM_CTC_IGMP_CONTROL_NOT_FAST_LEAVE;
	p_fast->mode_4 = OAM_CTC_IGMP_CONTROL_FAST_LEAVE;

	p_byte += sizeof(oam_fastleave_abl_t);
	g_usOutDataIdx += sizeof(oam_fastleave_abl_t);

	return;
}

void eopl_get_onu_mcast_fast_leave_admin(void)
{
	u8_t rlt=0;
	u8_t *p_byte;
	oam_fastleave_adm_t	*p_fast;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_fast = (oam_fastleave_adm_t*)p_byte;
	p_fast->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_fast->leaf = OAM_ATTR_MCAST_GFASTLEAVE_ADMIN;
	p_fast->width = 4;
	rlt = odmMulticastFastleaveAdminStateGet();
	if(!rlt)
	{
		p_fast->value = OAM_CTC_IGMP_FAST_LEAVE_ENABLE;
	}
	else
	{
		p_fast->value = OAM_CTC_IGMP_FAST_LEAVE_DISABLE;
	}

	p_byte += sizeof(oam_fastleave_adm_t);

	g_usOutDataIdx += sizeof(oam_fastleave_adm_t);

	return;
}

/*modify by lizhe 2010.1.7*/
void eopl_get_onu_alarm_admin_state(u8_t *in_p_byte)
{
#if 0
	u32_t value;
	u8_t uni_type;
	u16_t uni_id;
	u16_t obj_type;
	u32_t pon_id;
	u32_t card_id;
	u32_t llid;
	u8_t *p_byte;
	oam_alarm_admin_hdr_t *p_alarm_admin_hdr;
	oam_alarm_admin_entry_t *p_alarm_admin_entry;
	oam_instance_tlv_t *p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t ret;
	u16_t i,j;
	u16_t portS;
	u16_t portE;
	u16_t portMax;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	value = port_instance.value;

	switch(port_instance.leaf)
	{
	case OAM_CTC_OBJ_UNI:
        #if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)
			portMax = 4;
		#else
			portMax = 1;
		#endif
		uni_type = OAM_GET_PORT_TYPE(value);
		uni_id = OAM_GET_PORT_NUM(value);

		if(uni_id == 0xFFFF)
		{
			portS = 0;
			portE = OAM_MAX_INSTANCE_ID;
		}
		else
		{
			portS = uni_id;
			portE = uni_id;
		}

		if((uni_type == OAM_CTC_PORT_TYPE_ETH) && (portE <= OAM_MAX_INSTANCE_ID))
		{
			for(i = portS; i < portE; i++)
			{
				p_inst = (oam_instance_tlv_t *)p_byte;
				*p_inst = port_instance;

				value = 0;
				value = OAM_SET_PORT_NUM(value, portS);
				value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
				p_inst->value = value;
				p_byte += sizeof(oam_instance_tlv_t);
				g_usOutDataIdx += sizeof(oam_instance_tlv_t);

				for(j = 0; j < UNI_ALARM_NUM; j++)
				{
					p_alarm_admin_hdr = (oam_alarm_admin_hdr_t *)p_byte;
					p_alarm_admin_hdr->branch = OAM_BRANCH_EXTENDED_ATTRIB;
					p_alarm_admin_hdr->leaf = OAM_ATTR_ALARM_ADMIN_STATE;
					p_alarm_admin_hdr->width = sizeof(oam_alarm_admin_entry_t);
					p_byte += sizeof(oam_alarm_admin_hdr_t);
					g_usOutDataIdx += sizeof(oam_alarm_admin_hdr_t);

					p_alarm_admin_entry = (oam_alarm_admin_entry_t *)p_byte;
					p_alarm_admin_entry->alarm_id = stub_eth_port_alarm_config[i][j].alarm_id;
					p_alarm_admin_entry->alarm_config = stub_eth_port_alarm_config[i][j].enable_status;
					p_byte += sizeof(oam_alarm_admin_entry_t);
					g_usOutDataIdx += sizeof(oam_alarm_admin_entry_t);
				}
			}
		}
		else
		{
			p_inst = (oam_instance_tlv_t *)p_byte;
			*p_inst = port_instance;
			p_byte += sizeof(oam_instance_tlv_t);
			g_usOutDataIdx += sizeof(oam_instance_tlv_t);

			p_rsp = (oam_rsp_indication_t *)p_byte;
			p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_rsp->leaf = OAM_ATTR_ALARM_ADMIN_STATE;
			p_rsp->var_indication = OAM_SET_BAD_PARAMETERS;
			g_usOutDataIdx += sizeof(oam_rsp_indication_t);

		}
		break;
	case OAM_CTC_OBJ_CARD:
		card_id = value;
		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_rsp_indication_t *)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_ALARM_ADMIN_STATE;
		p_rsp->var_indication = OAM_SET_BAD_PARAMETERS;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);

		break;

	case OAM_CTC_OBJ_LLID:
		llid = value;
		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_rsp_indication_t *)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_ALARM_ADMIN_STATE;
		p_rsp->var_indication = OAM_SET_BAD_PARAMETERS;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);

		break;

	case OAM_CTC_OBJ_PON:
		pon_id = value;

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		for(i = 0; i < PON_ALARM_NUM; i++)
		{
			p_alarm_admin_hdr = (oam_alarm_admin_hdr_t *)p_byte;
			p_alarm_admin_hdr->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_alarm_admin_hdr->leaf = OAM_ATTR_ALARM_ADMIN_STATE;
			p_alarm_admin_hdr->width = sizeof(oam_alarm_admin_entry_t);
			p_byte += sizeof(oam_alarm_admin_hdr_t);
			g_usOutDataIdx += sizeof(oam_alarm_admin_hdr_t);

			p_alarm_admin_entry = (oam_alarm_admin_entry_t *)p_byte;
			p_alarm_admin_entry->alarm_id = stub_pon_alarm_config[i].alarm_id;
			p_alarm_admin_entry->alarm_config = stub_pon_alarm_config[i].enable_status;
			p_byte += sizeof(oam_alarm_admin_entry_t);
			g_usOutDataIdx += sizeof(oam_alarm_admin_entry_t);
		}

		break;
	default:
		/* OAM_CTC_OBJ_ONU */
		p_alarm_admin_entry = (oam_alarm_admin_entry_t *)p_byte;
		for(i = 0; i < ONU_ALARM_NUM; i++)
		{
			p_alarm_admin_hdr = (oam_alarm_admin_hdr_t *)p_byte;
			p_alarm_admin_hdr->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_alarm_admin_hdr->leaf = OAM_ATTR_ALARM_ADMIN_STATE;
			p_alarm_admin_hdr->width = sizeof(oam_alarm_admin_entry_t);
			p_byte += sizeof(oam_alarm_admin_hdr_t);
			g_usOutDataIdx += sizeof(oam_alarm_admin_hdr_t);

			p_alarm_admin_entry = (oam_alarm_admin_entry_t *)p_byte;
			p_alarm_admin_entry->alarm_id = stub_onu_alarm_config[i].alarm_id;
			p_alarm_admin_entry->alarm_config = stub_onu_alarm_config[i].enable_status;
			p_byte += sizeof(oam_alarm_admin_entry_t);
			g_usOutDataIdx += sizeof(oam_alarm_admin_entry_t);
		}
		break;
	}

	return;


#else
	u8_t *p_byte;
	u8_t *p_in_byte = sizeof(oam_variable_descriptor_t ) + in_p_byte; /*add for alarm 2010.1.7*/
	oam_alarm_admin_entry_t *p_alarm_adm;
	oam_alarm_admin_hdr_t *p_alarm_adm_hdr;
	oam_alarm_admin_entry_t *p_in_alarm_adm;
	u32_t value = port_instance.value;
	oam_instance_tlv_t *p_oam_instance;
	oam_rsp_indication_t *p_rsp;
	u8_t alarmState;
	u8_t uni_type;
	u16_t uni_id;
	u32_t i;
	u32_t portS;
	u32_t portE;
	int ret;

	uni_type = OAM_GET_PORT_TYPE(value);
	uni_id = OAM_GET_PORT_NUM(value);
	
	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	if(uni_id == 0xffff && uni_type == OAM_CTC_OBJ_UNI)
	{
		portS = 1;
		portE = ODM_NUM_OF_PORTS;
	}
	else
	{
		portS = uni_id;
		portE = uni_id;
	}

	p_in_byte += 1;/*width*/
	
	p_in_alarm_adm = (oam_alarm_admin_entry_t *)p_in_byte;
	p_in_byte += sizeof(oam_alarm_admin_entry_t);
	

	for(i = portS; i <= portE; i++)
	{
		p_oam_instance = (oam_instance_tlv_t *)p_byte;
		value = 0;
		value = OAM_SET_PORT_NUM(value, i);
		value = OAM_SET_PORT_TYPE(value, uni_type);
		*p_oam_instance = port_instance;
		p_oam_instance->value = value;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		/* call api to get alarm state */
		//ret = stub_get_onu_alarm_admin_state(alarmId, &alarmState);
		//ret = CTC_STACK_alarm_get_state(p_in_alarm_adm->alarm_id,  i, &alarmState);
		if(ret == 0)
		{
			p_alarm_adm_hdr = (oam_alarm_admin_hdr_t *)p_byte;
			p_byte += sizeof(oam_alarm_admin_hdr_t);
			
			p_alarm_adm_hdr->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_alarm_adm_hdr->leaf = OAM_ATTR_ALARM_ADMIN_STATE;
			p_alarm_adm_hdr->width = 6;
			g_usOutDataIdx += sizeof(oam_alarm_admin_hdr_t);

			p_alarm_adm = (oam_alarm_admin_entry_t *)p_byte;
			p_byte += sizeof(oam_alarm_admin_entry_t);
			p_alarm_adm->alarm_id = p_in_alarm_adm->alarm_id ;
			p_alarm_adm->alarm_config = alarmState;
			g_usOutDataIdx += sizeof(oam_alarm_admin_entry_t);
		}
		else
		{
			p_rsp = (oam_rsp_indication_t *)p_byte;
			p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_rsp->leaf = OAM_ATTR_ALARM_THRESHOLD;
			p_rsp->var_indication = 0x87;

			p_byte += sizeof(oam_rsp_indication_t);

			g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		}
	}
	
	return;
#endif
}

/*modify by lizhe 2010.1.7*/
void eopl_get_onu_alarm_threshold(u8_t *in_p_byte)
{
#if 0
	u32_t value;
	u8_t uni_type;
	u16_t uni_id;
	u16_t obj_type;
	u32_t pon_id;
	u32_t card_id;
	u32_t llid;
	u8_t *p_byte;
	oam_alarm_threshold_hdr_t *p_alarm_threshold_hdr;
	oam_alarm_threshold_entry_t *p_alarm_threshold_entry;
	oam_instance_tlv_t *p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t ret;
	u16_t i,j;
	u16_t portS;
	u16_t portE;
	u16_t portMax;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	value = port_instance.value;

	switch(port_instance.leaf)
	{
	case OAM_CTC_OBJ_UNI:
        #if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)
			portMax = 4;
		#else
			portMax = 1;
		#endif
		uni_type = OAM_GET_PORT_TYPE(value);
		uni_id = OAM_GET_PORT_NUM(value);

		if(uni_id == 0xFFFF)
		{
			portS = 0;
			portE = OAM_MAX_INSTANCE_ID;
		}
		else
		{
			portS = uni_id;
			portE = uni_id;
		}

		if((uni_type == OAM_CTC_PORT_TYPE_ETH) && (portE <= OAM_MAX_INSTANCE_ID))
		{
			for(i = portS; i < portE; i++)
			{
				p_inst = (oam_instance_tlv_t *)p_byte;
				*p_inst = port_instance;

				value = 0;
				value = OAM_SET_PORT_NUM(value, portS);
				value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
				p_inst->value = value;
				p_byte += sizeof(oam_instance_tlv_t);
				g_usOutDataIdx += sizeof(oam_instance_tlv_t);

				for(j = 0; j < UNI_ALARM_NUM; j++)
				{
					p_alarm_threshold_hdr = (oam_alarm_threshold_hdr_t *)p_byte;
					p_alarm_threshold_hdr->branch = OAM_BRANCH_EXTENDED_ATTRIB;
					p_alarm_threshold_hdr->leaf = OAM_ATTR_ALARM_THRESHOLD;
					p_alarm_threshold_hdr->width = sizeof(oam_alarm_threshold_entry_t);
					p_byte += sizeof(oam_alarm_threshold_hdr_t);
					g_usOutDataIdx += sizeof(oam_alarm_threshold_hdr_t);

					p_alarm_threshold_entry = (oam_alarm_threshold_entry_t *)p_byte;
					p_alarm_threshold_entry->alarm_id = stub_eth_port_alarm_config[i][j].alarm_id;
					p_alarm_threshold_entry->raising_threshold = stub_eth_port_alarm_config[i][j].raising_threshold;
					p_alarm_threshold_entry->falling_threshold = stub_eth_port_alarm_config[i][j].falling_threshold;
					p_byte += sizeof(oam_alarm_threshold_entry_t);
					g_usOutDataIdx += sizeof(oam_alarm_threshold_entry_t);
				}
			}
		}
		else
		{
			p_inst = (oam_instance_tlv_t *)p_byte;
			*p_inst = port_instance;
			p_byte += sizeof(oam_instance_tlv_t);
			g_usOutDataIdx += sizeof(oam_instance_tlv_t);

			p_rsp = (oam_rsp_indication_t *)p_byte;
			p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_rsp->leaf = OAM_ATTR_ALARM_THRESHOLD;
			p_rsp->var_indication = OAM_SET_BAD_PARAMETERS;
			g_usOutDataIdx += sizeof(oam_rsp_indication_t);

		}
		break;
	case OAM_CTC_OBJ_CARD:
		card_id = value;
		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_rsp_indication_t *)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_ALARM_THRESHOLD;
		p_rsp->var_indication = OAM_SET_BAD_PARAMETERS;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);

		break;

	case OAM_CTC_OBJ_LLID:
		llid = value;
		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_rsp_indication_t *)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_ALARM_THRESHOLD;
		p_rsp->var_indication = OAM_SET_BAD_PARAMETERS;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		break;

	case OAM_CTC_OBJ_PON:
		pon_id = value;

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		for(i = 0; i < PON_ALARM_NUM; i++)
		{
			p_alarm_threshold_hdr = (oam_alarm_threshold_hdr_t *)p_byte;
			p_alarm_threshold_hdr->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_alarm_threshold_hdr->leaf = OAM_ATTR_ALARM_THRESHOLD;
			p_alarm_threshold_hdr->width = PON_ALARM_NUM * sizeof(oam_alarm_threshold_entry_t);
			p_byte += sizeof(oam_alarm_threshold_hdr_t);
			g_usOutDataIdx += sizeof(oam_alarm_threshold_hdr_t);

			p_alarm_threshold_entry = (oam_alarm_threshold_entry_t *)p_byte;
			p_alarm_threshold_entry->alarm_id = stub_pon_alarm_config[i].alarm_id;
			p_alarm_threshold_entry->raising_threshold = stub_pon_alarm_config[i].raising_threshold;
			p_alarm_threshold_entry->falling_threshold = stub_pon_alarm_config[i].falling_threshold;
			p_byte += sizeof(oam_alarm_threshold_entry_t);
			g_usOutDataIdx += sizeof(oam_alarm_threshold_entry_t);
		}

		break;
	default:
		/* OAM_CTC_OBJ_ONU */
		for(i = 0; i < ONU_ALARM_NUM; i++)
		{
			p_alarm_threshold_hdr = (oam_alarm_threshold_hdr_t *)p_byte;
			p_alarm_threshold_hdr->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_alarm_threshold_hdr->leaf = OAM_ATTR_ALARM_THRESHOLD;
			p_alarm_threshold_hdr->width = sizeof(oam_alarm_threshold_entry_t);
			p_byte += sizeof(oam_alarm_threshold_hdr_t);
			g_usOutDataIdx += sizeof(oam_alarm_threshold_hdr_t);

			p_alarm_threshold_entry = (oam_alarm_threshold_entry_t *)p_byte;
			p_alarm_threshold_entry->alarm_id = stub_onu_alarm_config[i].alarm_id;
			p_alarm_threshold_entry->raising_threshold = stub_onu_alarm_config[i].raising_threshold;
			p_alarm_threshold_entry->falling_threshold = stub_onu_alarm_config[i].falling_threshold;
			p_byte += sizeof(oam_alarm_threshold_entry_t);
			g_usOutDataIdx += sizeof(oam_alarm_threshold_entry_t);
		}
		break;
	}

	return;
#else
	u8_t *p_byte;
	u8_t *p_in_byte = in_p_byte + sizeof(oam_variable_descriptor_t ); 
	oam_alarm_threshold_entry_t *p_alarm_threshold;
	oam_alarm_threshold_hdr_t *p_alarm_threshold_hdr;
	oam_alarm_threshold_entry_t *p_alarm_in_threshold;
	u32_t value = port_instance.value;
	oam_instance_tlv_t *p_oam_instance;
	oam_rsp_indication_t *p_rsp;
	i32_t raising_threshold;
	i32_t falling_threshold;
	int ret;
	u8_t uni_type;
	u16_t uni_id;
	u32_t portS;
	u32_t portE;
	u32_t i;
	
	uni_type = OAM_GET_PORT_TYPE(value);
	uni_id = OAM_GET_PORT_NUM(value);
		
	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	if(uni_id == 0xffff && uni_type == OAM_CTC_OBJ_UNI)
	{
		portS = 1;
		portE = ODM_NUM_OF_PORTS;
	}
	else
	{
		portS = uni_id;
		portE = uni_id;
	}
	
	p_in_byte += 1;/*width*/

	p_alarm_in_threshold = (oam_alarm_threshold_entry_t *)p_in_byte;
	p_in_byte += sizeof(oam_alarm_threshold_entry_t);
	

	for(i = portS; i <= portE; i++)
	{
		p_oam_instance = (oam_instance_tlv_t *)p_byte;
		value = 0;
		value = OAM_SET_PORT_NUM(value, i);
		value = OAM_SET_PORT_TYPE(value, uni_type);
		*p_oam_instance = port_instance;
		p_oam_instance->value = value;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		//ret = stub_get_alarm_raising_threshold(alarmId, &raising_threshold, &falling_threshold);				
		//ret = CTC_STACK_alarm_get_thr(p_alarm_in_threshold->alarm_id, i, &raising_threshold, &falling_threshold);
		if(ret == 0)
		{
			//get_p_param(p_alarm_threshold_hdr,p_byte, oam_alarm_threshold_hdr_t );
			p_alarm_threshold_hdr = (oam_alarm_threshold_hdr_t  *)p_byte;
			p_byte += sizeof(oam_alarm_threshold_hdr_t);
			
			p_alarm_threshold_hdr->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_alarm_threshold_hdr->leaf = OAM_ATTR_ALARM_THRESHOLD;
			p_alarm_threshold_hdr->width = 10;

			g_usOutDataIdx += sizeof(oam_alarm_threshold_hdr_t);

			//get_p_param(p_alarm_threshold, p_byte, oam_alarm_threshold_entry_t );			
			p_alarm_threshold = (oam_alarm_threshold_entry_t  *)p_byte;
			p_byte += sizeof(oam_alarm_threshold_entry_t);
			
			p_alarm_threshold->alarm_id = p_alarm_in_threshold->alarm_id;
			p_alarm_threshold->raising_threshold = raising_threshold;
			p_alarm_threshold->falling_threshold = falling_threshold;

			g_usOutDataIdx += sizeof(oam_alarm_threshold_entry_t);
		}
		else
		{
			p_rsp = (oam_rsp_indication_t *)p_byte;
			p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_rsp->leaf = OAM_ATTR_ALARM_THRESHOLD;
			p_rsp->var_indication = 0x87;

			p_byte += sizeof(oam_rsp_indication_t);

			g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		}
	}
	return;
#endif
}
u32_t stub_voip_protocol_supported_status_get(u8_t *status)
{
	*status = VOIP_PROTOCOL_H248;  /* 0x00:H248, 0x01: SIP */

	return NO_ERROR;
}

u32_t stub_iad_sw_version_get(u8_t *p_sw_version)
{
    /* for now, SwVersionGet take care 16 bytes */
    vosMemSet(&p_sw_version[0], 0, 16);
    SwVersionGet(&p_sw_version[16]);

	return NO_ERROR;
}

u32_t stub_iad_sw_time_get(u8_t *p_sw_time)
{
    /* Software build time format must be YYYYMMDDHHMMSS */
    vosMemSet(&p_sw_time[0], 0, 18);
    odmSysSoftwareBuildTimeGet(&p_sw_time[18]);

	return NO_ERROR;
}

u32_t stub_voip_user_count(u8_t *p_user_count)
{
	*p_user_count = VOIP_POTS_NUM;
	return NO_ERROR;
}
void eopl_get_onu_iad_info(void)
{
	u8_t *p_byte;
	oam_iad_info_t *p_iad_info;
	u32_t ret;
	oam_rsp_indication_t *p_rsp;
	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_iad_info = (oam_iad_info_t *)p_byte;
	p_iad_info->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_iad_info->leaf = OAM_ATTR_IAD_INFO;
	p_iad_info->width = sizeof(oam_iad_info_t) - 4;
	/* get IAD mac address */
    vosHWAddrGet("eth0", &p_iad_info->mac[0]);

    /* save the supported VOIP protocal type */
	ret = stub_voip_protocol_supported_status_get(&p_iad_info->protocol_support);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get voip protocl supported status!\n");
	}
	ret = stub_iad_sw_version_get(&p_iad_info->iad_sw_version[0]);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get iad software version!\n");
		goto send_rsp;
	}
	ret = stub_iad_sw_time_get(&p_iad_info->iad_sw_time[0]);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get iad software time!\n");
		goto send_rsp;
	}
	ret = stub_voip_user_count(&p_iad_info->voip_user_count);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get voip user count!\n");
		goto send_rsp;
	}
send_rsp:
	{
		p_byte += sizeof(oam_iad_info_t);
		g_usOutDataIdx += sizeof(oam_iad_info_t);
	}

	return;
}

u32_t stub_voice_ip_mode_get(u8_t *p_ip_mode)
{
	odmSysCfgIfIpModeGet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, p_ip_mode);
	return NO_ERROR;
}
u32_t stub_voice_ip_mode_set(u8_t ip_mode)
{
	odmSysCfgIfIpModeSet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, ip_mode,NULL);
	return NO_ERROR;
}
u32_t stub_iad_ip_addr_para_get(u32_t *p_ip_addr, u32_t *p_net_mask, u32_t *p_default_gw)
{
	odmSysCfgIfIpAddrGet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, p_ip_addr);
	odmSysCfgIfIpMaskGet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, p_net_mask);
	odmSysCfgIfIpGatewayGet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, p_default_gw);
	return NO_ERROR;
}
u32_t stub_iad_ip_addr_para_set(u32_t ip_addr, u32_t net_mask, u32_t default_gw)
{
	odmSysCfgIfIpAddrSet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, ip_addr);
	odmSysCfgIfIpMaskSet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, net_mask);
	odmSysCfgIfIpGatewaySet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, default_gw);
	return NO_ERROR;
}
u32_t stub_pppoe_mode_get(u8_t *p_pppoe_mode)
{
	odmSysCfgIfPPPoEModeGet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, p_pppoe_mode);
	return NO_ERROR;
}
u32_t stub_pppoe_mode_set(u8_t pppoe_mode)
{
	odmSysCfgIfPPPoEModeSet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, pppoe_mode);
	return NO_ERROR;
}
u32_t stub_pppoe_para_get(u8_t *p_username, u8_t *p_password)
{
	odmSysCfgIfPPPoEUserGet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, p_username);
	odmSysCfgIfPPPoEPassGet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, p_password);
	return NO_ERROR;
}
u32_t stub_pppoe_para_set(u8_t *p_username, u8_t *p_password)
{
	odmSysCfgIfPPPoEUserSet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, p_username);
	odmSysCfgIfPPPoEPassSet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, p_password);
	return NO_ERROR;
}
u32_t stub_voice_data_para_get(u8_t *p_tag_flag, u16_t *p_cvlan, u16_t *p_svlan, u8_t *p_priority)
{
	odmSysCfgIfTagModeGet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, p_tag_flag);
	odmSysCfgIfCVLANGet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, p_cvlan);
	odmSysCfgIfPriGet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, p_priority);
	odmSysCfgIfSVLANGet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, p_svlan);
	return NO_ERROR;
}
u32_t stub_voice_data_para_set(u8_t tag_flag, u16_t cvlan, u16_t svlan, u8_t priority)
{
	odmSysCfgIfTagModeSet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, tag_flag);
	odmSysCfgIfCVLANSet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, cvlan);
	odmSysCfgIfPriSet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, priority);
	odmSysCfgIfSVLANSet(INTERFACE_WAN, INTERFACE_MODE_ADMIN, svlan);
	return NO_ERROR;
}
u32_t stub_mg_port_num_get(u16_t *p_mg_port)
{
	*p_mg_port = stub_mg_port_num ;

	return NO_ERROR;
}

u32_t stub_mg_port_num_set(u16_t mg_port)
{
    stub_mg_port_num = mg_port;
	return NO_ERROR;
}

u32_t stub_mgc_para_get(u32_t *p_mgc_ip, u16_t *p_com_port)
{

    *p_mgc_ip = stub_mgc_ip;
    *p_com_port = stub_mgc_port;
	return NO_ERROR;
}

u32_t stub_mgc_para_set(u32_t mgc_ip, u16_t com_port)
{

    stub_mgc_ip = mgc_ip;
    stub_mgc_port = com_port;
	return NO_ERROR;
}

u32_t stub_backup_mgc_para_get(u32_t *p_mgc_ip, u16_t *p_com_port)
{
    *p_mgc_ip = stub_backup_mgc_ip;
    *p_com_port = stub_backup_mgc_port;

	return NO_ERROR;
}

u32_t stub_backup_mgc_para_set(u32_t mgc_ip, u16_t com_port)
{
    stub_backup_mgc_ip = mgc_ip;
    stub_backup_mgc_port = com_port;

	return NO_ERROR;
}

u32_t stub_active_mgc_get(u8_t *p_active_mgc)
{
	*p_active_mgc = stub_active_mgc;

	return NO_ERROR;
}

u32_t stub_active_mgc_set(u8_t active_mgc)
{
	stub_active_mgc = active_mgc;

	return NO_ERROR;
}

u32_t stub_reg_mode_get(u8_t *p_mode)
{
	*p_mode = VOIP_H248_REG_MODE_IP;

	return NO_ERROR;
}

u32_t stub_reg_mode_set(u8_t mode)
{
	stub_reg_mode = mode;

	return NO_ERROR;
}

u32_t stub_mid_get(u8_t *p_mid)
{

    vosStrCpy(p_mid,stub_mid);
    *p_mid = stub_mid;
	return NO_ERROR;
}

u32_t stub_mid_set(u8_t *p_mid)
{

    vosStrCpy(stub_mid,p_mid);
	return NO_ERROR;
}

u32_t stub_heart_beat_mode_get(u8_t *p_mode)
{

    *p_mode = stub_heartbeat_mode;
	return NO_ERROR;
}

u32_t stub_heart_beat_mode_set(u8_t mode)
{

    stub_heartbeat_mode = mode;
	return NO_ERROR;
}

u32_t stub_heart_beat_cycle_get(u16_t *p_cycle)
{

    *p_cycle = stub_heartbeat_cycle;
	return NO_ERROR;
}

u32_t stub_heart_beat_cycle_set(u16_t cycle)
{

    stub_heartbeat_cycle = cycle;
	return NO_ERROR;
}

u32_t stub_heart_beat_count_get(u8_t *p_count)
{
	*p_count = VOIP_H248_DEF_HEARTBEAT_COUNT;

	return NO_ERROR;
}

u32_t stub_heart_beat_count_set(u8_t count)
{
	stub_heart_beat_count = count;

	return NO_ERROR;
}

u32_t stub_user_tid_get(u8_t port_id, u8_t *p_tid)
{
    char acTempStr[32], acTempStr2[32], acTidName[32] = {0};
    int nLen;

    OAM_DBG("[%s], stub_user_tid is [%s]",__FUNCTION__,stub_user_tid);
    nLen = vosStrLen(stub_user_tid);
    if (nLen < 32)
    {
        vosMemSet(p_tid, 0, 32 - nLen);
        vosMemCpy(&p_tid[32 - nLen], stub_user_tid, nLen);
    }
    else {
        vosMemCpy(p_tid, acTidName, 32);
    }

	return NO_ERROR;
}

u32_t stub_user_tid_set(u8_t port_id, u8_t *p_tid)
{
    char *pcStr, *pcResult;
    int nLen;

    if (NULL == p_tid)
        return PARAMETER_INPUT_POINTER_NULL;

    oamLSCAlignmentStrGet(&p_tid[0], 32, stub_user_tid, 32);
    /*vosMemCpy(stub_user_tid,p_tid,vosStrLen(p_tid));*/

    OAM_DBG("[%s], stub_user_tid is [%s]\n",__FUNCTION__,stub_user_tid);

    stub_port_id = port_id;

	return NO_ERROR;
}

u32_t stub_num_of_rtp_tid_get(u8_t *p_rtp_tid_num)
{
    OAM_DBG("[%s] stub_num_of_rtp_tid is [%d]\n",stub_num_of_rtp_tid);
	*p_rtp_tid_num = stub_num_of_rtp_tid;
	return NO_ERROR;
}

u32_t stub_num_of_rtp_tid_set(u8_t rtp_tid_num)
{
	stub_num_of_rtp_tid = rtp_tid_num;
	return NO_ERROR;
}

u32_t stub_rtp_tid_prefix_get(u8_t *p_rtp_tid_prefix)
{
    vosStrCpy(p_rtp_tid_prefix,stub_rtp_tid_prefix);
	return NO_ERROR;
}

u32_t stub_rtp_tid_prefix_set(u8_t *p_rtp_tid_prefix)
{
    vosStrCpy(stub_rtp_tid_prefix,p_rtp_tid_prefix);
	return NO_ERROR;
}

u32_t stub_rtp_tid_digit_begin_get(u8_t *p_digit_begin)
{

    vosStrCpy(p_digit_begin,stub_rtp_tid_digit);

	return NO_ERROR;
}

u32_t stub_rtp_tid_digit_begin_set(u8_t *p_digit_begin)
{
    vosStrCpy(stub_rtp_tid_digit,p_digit_begin);
	return NO_ERROR;
}

u32_t stub_rtp_tid_mode_get(u8_t *p_mode)
{
	*p_mode = stub_rtp_tid_mode;
	return NO_ERROR;
}

u32_t stub_rtp_tid_mode_set(u8_t mode)
{
	stub_rtp_tid_mode = mode;
	return NO_ERROR;
}


u32_t stub_rtp_tid_digit_len_get(u8_t *p_digit_len)
{
	*p_digit_len = stub_rtp_tid_digit_len;

	return NO_ERROR;
}

u32_t stub_rtp_tid_digit_len_set(u8_t digit_len)
{
	stub_rtp_tid_digit_len = digit_len;

	return NO_ERROR;
}

u32_t stub_first_rtp_tid_name_get(u8_t *p_rtp_tid_name)
{
#if 0
    char acTempStr[32], acTempStr2[32], acTidName[32] = {0};
    int nLen;

    CfmAgent_GetCfg(VOIP_H248RTPTIDConfig_Prefix,acTempStr,32);
    CfmAgent_GetCfg(VOIP_H248RTPTIDConfig_DigitBegin,acTempStr2,32);

    vosStrCat(acTidName, acTempStr);
    vosStrCat(acTidName, VOIP_H248UserTIDInfo_UserTIDNameSlash);
    vosStrCat(acTidName, acTempStr2);

    nLen = vosStrLen(acTidName);
    if (nLen < 32)
    {
        vosMemSet(&p_rtp_tid_name[0], 0, 32 - nLen);
        vosMemCpy(&p_rtp_tid_name[32 - nLen], acTidName, nLen);
    }
    else {
        vosMemCpy(&p_rtp_tid_name[0], acTidName, 32);
    }
#endif
	return NO_ERROR;
}

u32_t stub_sip_mg_port_no_get(u16_t *p_sip_mg_port_num)
{
	*p_sip_mg_port_num = stub_sip_mg_port_num;

	return NO_ERROR;
}

u32_t stub_sip_mg_port_no_set(u16_t sip_mg_port_num)
{
	stub_sip_mg_port_num = sip_mg_port_num;

	return NO_ERROR;
}

u32_t stub_sip_proxy_server_ip_get(u32_t *p_sip_proxy_server_ip)
{
	*p_sip_proxy_server_ip = stub_sip_proxy_serv_ip;

	return NO_ERROR;
}

u32_t stub_sip_proxy_server_ip_set(u32_t sip_proxy_server_ip)
{
	stub_sip_proxy_serv_ip = sip_proxy_server_ip;

	return NO_ERROR;
}


u32_t stub_sip_proxy_serv_com_port_num_get(u16_t *p_sip_proxy_serv_com_port_num)
{
	*p_sip_proxy_serv_com_port_num = stub_sip_proxy_serv_com_port_num;

	return NO_ERROR;
}

u32_t stub_sip_proxy_serv_com_port_num_set(u16_t sip_proxy_serv_com_port_num)
{
	stub_sip_proxy_serv_com_port_num = sip_proxy_serv_com_port_num;

	return NO_ERROR;
}

u32_t stub_backup_sip_proxy_serv_ip_get(u32_t *p_backup_sip_proxy_serv_ip)
{
	*p_backup_sip_proxy_serv_ip = stub_backup_sip_proxy_serv_ip;

	return NO_ERROR;
}
u32_t stub_backup_sip_proxy_serv_ip_set(u32_t backup_sip_proxy_serv_ip)
{
	stub_backup_sip_proxy_serv_ip = backup_sip_proxy_serv_ip;

	return NO_ERROR;
}

u32_t stub_backup_sip_proxy_serv_com_port_num_get(u16_t *p_backup_sip_proxy_com_port_num)
{
	*p_backup_sip_proxy_com_port_num = stub_backup_sip_proxy_serv_com_port_num;

	return NO_ERROR;
}
u32_t stub_backup_sip_proxy_serv_com_port_num_set(u16_t backup_sip_proxy_com_port_num)
{
	stub_backup_sip_proxy_serv_com_port_num = backup_sip_proxy_com_port_num;

	return NO_ERROR;
}

u32_t stub_active_sip_proxy_server_get(u32_t *p_active_sip_proxy_server)
{
	*p_active_sip_proxy_server = stub_active_sip_proxy_serv;;

	return NO_ERROR;
}
u32_t stub_active_sip_proxy_server_set(u32_t active_sip_proxy_server)
{
	stub_active_sip_proxy_serv = active_sip_proxy_server;

	return NO_ERROR;
}

u32_t stub_sip_reg_server_com_port_num_get(u16_t *p_sip_reg_server_com_port_num)
{
	*p_sip_reg_server_com_port_num = stub_sip_reg_serv_com_port_num;

	return NO_ERROR;
}
u32_t stub_sip_reg_server_com_port_num_set(u16_t sip_reg_server_com_port_num)
{
	stub_sip_reg_serv_com_port_num = sip_reg_server_com_port_num;

	return NO_ERROR;
}

u32_t stub_backup_sip_reg_server_ip_get(u32_t *p_backup_sip_reg_server_ip)
{
	*p_backup_sip_reg_server_ip = stub_backup_sip_reg_serv_ip;

	return NO_ERROR;
}

u32_t stub_backup_sip_reg_server_ip_set(u32_t backup_sip_reg_server_ip)
{
	stub_backup_sip_reg_serv_ip = backup_sip_reg_server_ip;

	return NO_ERROR;
}

u32_t stub_backup_sip_reg_server_com_port_num_get(u16_t *p_backup_sip_reg_server_com_port_num)
{
	*p_backup_sip_reg_server_com_port_num = stub_backup_sip_reg_serv_com_port_num;

	return NO_ERROR;
}

u32_t stub_backup_sip_reg_server_com_port_num_set(u16_t backup_sip_reg_server_com_port_num)
{
	stub_backup_sip_reg_serv_com_port_num = backup_sip_reg_server_com_port_num;

	return NO_ERROR;
}

u32_t stub_outbound_server_ip_get(u32_t *p_outbound_server_ip)
{
	*p_outbound_server_ip = stub_outbound_serv_ip;

	return NO_ERROR;
}
u32_t stub_outbound_server_ip_set(u32_t outbound_server_ip)
{
	stub_outbound_serv_ip = outbound_server_ip;

	return NO_ERROR;
}

u32_t stub_outbound_server_port_num_get(u16_t *p_outbound_server_port_num)
{
	*p_outbound_server_port_num = stub_outbound_serv_port_num;

	return NO_ERROR;
}
u32_t stub_outbound_server_port_num_set(u16_t outbound_server_port_num)
{
	stub_outbound_serv_port_num = outbound_server_port_num;

	return NO_ERROR;
}

u32_t stub_sip_reg_interval_get(u32_t *p_sip_reg_interval)
{
	*p_sip_reg_interval = stub_sip_reg_interval;

	return NO_ERROR;
}
u32_t stub_sip_reg_interval_set(u32_t sip_reg_interval)
{
	stub_sip_reg_interval = sip_reg_interval;

	return NO_ERROR;
}

u32_t stub_sip_heartbeat_switch_get(u8_t *p_sip_heartbeat_switch)
{
	*p_sip_heartbeat_switch = stub_sip_heart_beat_switch;

	return NO_ERROR;
}
u32_t stub_sip_heartbeat_switch_set(u8_t sip_heartbeat_switch)
{
	stub_sip_heart_beat_switch = sip_heartbeat_switch;

	return NO_ERROR;
}

u32_t stub_sip_heartbeat_cycle_get(u16_t *p_sip_heartbeat_cycle)
{
	*p_sip_heartbeat_cycle = stub_sip_heart_beat_cycle;

	return NO_ERROR;
}
u32_t stub_sip_heartbeat_cycle_set(u16_t sip_heartbeat_cycle)
{
	stub_sip_heart_beat_cycle = sip_heartbeat_cycle;

	return NO_ERROR;
}

u32_t stub_sip_heartbeat_count_get(u16_t *p_sip_heartbeat_count)
{
	*p_sip_heartbeat_count = stub_sip_heart_beat_count;

	return NO_ERROR;
}
u32_t stub_sip_heartbeat_count_set(u16_t sip_heartbeat_count)
{
	stub_sip_heart_beat_count = sip_heartbeat_count;

	return NO_ERROR;
}

u32_t stub_sip_user_para_get(u8_t port_id, u8_t *p_user_account, u8_t *p_user_name, u8_t * p_password)
{

	return NO_ERROR;
}

u32_t stub_sip_user_para_set(u8_t port_id, u8_t *p_user_account, u8_t *p_user_name, u8_t *p_password)
{
	return NO_ERROR;
}

u32_t stub_voice_t38_enable_get(u8_t *p_enable)
{
	*p_enable = stub_voice_t38_enable;
	return NO_ERROR;
}

u32_t stub_voice_t38_enable_set(u8_t enable)
{
	stub_voice_t38_enable = enable;
	return NO_ERROR;
}

u32_t stub_voice_fax_modem_control_get(u8_t *p_control_method)
{
	*p_control_method = stub_voice_fax_control;

	return NO_ERROR;
}

u32_t stub_voice_fax_modem_control_set(u8_t control_method)
{
	stub_voice_fax_control = control_method;

	return NO_ERROR;
}

u32_t stub_h248_iad_operation_status_get(u32_t *p_status)
{
#if 0
    char acTempStr[128];

    CfmAgent_GetStatus(VOIP_H248IADOperationStatus_IadOperStatus, acTempStr, 128, FALSE);
    if (0 == vosStrCmp(VOIP_H248IADOperationStatus_Registered, acTempStr))
    {
        *p_status = VOIP_IAD_H248_OPER_STATUS_REGISTERED;
    }
    else
    {
        *p_status = VOIP_IAD_H248_OPER_STATUS_UNREGISTERED;
    }
#endif
	return NO_ERROR;
}

u32_t stub_iad_port_status_get(u8_t port_id, u32_t *p_status)
{
#if 0
    char acTempStr[128];

    CfmAgent_SetPort(port_id);
    CfmAgent_GetStatus(VOIP_POTSStatus_IADPortStauts, acTempStr, 128, FALSE);
    if (0 == vosStrCmp(VOIP_POTSStatus_IADPortStauts_Onhook, acTempStr))
    {
        p_status = VOIP_IAD_PORT_STATUS_IDLE;
    }
    else
    {
        p_status = VOIP_IAD_PORT_STATUS_HOOKOFF;
    }
#endif
	return NO_ERROR;
}

u32_t stub_iad_port_service_state_get(u8_t port_id, u32_t *p_state)
{
	return NO_ERROR;
}

u32_t stub_iad_port_codec_mode_get(u8_t port_id, u32_t *p_mode)
{
	return NO_ERROR;
}

u32_t stub_iad_operation_set(u32_t operation)
{
	stub_iad_operation = operation;

	return NO_ERROR;
}

u32_t stub_sip_digital_map_set(u8_t *p_digital_map)
{
	vosMemCpy(&stub_sip_digital_map[0], p_digital_map, 1024);

	return NO_ERROR;
}
void eopl_get_onu_global_para_config(void)
{
	u8_t *p_byte;
	oam_global_para_cfg_t *p_global_cfg;
	u32_t ret;
	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_global_cfg = (oam_global_para_cfg_t *)p_byte;
	p_global_cfg->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_global_cfg->leaf = OAM_ATTR_GLOBAL_PARA_CONFIG;
	p_global_cfg->width = sizeof(oam_global_para_cfg_t) - 4;
	ret = stub_voice_ip_mode_get(&p_global_cfg->voice_ip_mode);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get voice ip mode!\n");
		goto send_rsp;
	}
	ret = stub_iad_ip_addr_para_get(&p_global_cfg->iad_ipaddr,
									&p_global_cfg->iad_netmask,
									&p_global_cfg->iad_defaultgw);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get iad ip addr parameters!\n");
		goto send_rsp;
	}
	ret = stub_pppoe_mode_get(&p_global_cfg->pppoe_mode);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get iad pppoe mode!\n");
		goto send_rsp;
	}
	ret = stub_pppoe_para_get(&p_global_cfg->pppoe_username[0],
								&p_global_cfg->pppoe_passwd[0]);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get pppoe parameters!\n");
		goto send_rsp;
	}
	ret = stub_voice_data_para_get(&p_global_cfg->tagged_flag,
									&p_global_cfg->voice_cvlan,
									&p_global_cfg->voice_svlan,
									(u8_t *)&p_global_cfg->voice_priority);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get voice data parameters!\n");
		goto send_rsp;
	}
send_rsp:
	#if 0
	if(ret != NO_ERROR);
	{
		/* send var no resource response */
	}
	else
	#endif
	{
		p_byte += sizeof(oam_global_para_cfg_t);

		g_usOutDataIdx += sizeof(oam_global_para_cfg_t);
	}

	return;
}

void eopl_get_onu_h248_para_config(void)
{
	u8_t *p_byte;
	oam_h248_para_cfg_t *p_h248_cfg;
	u32_t ret;
	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_h248_cfg = (oam_h248_para_cfg_t *)p_byte;
	p_h248_cfg->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_h248_cfg->leaf = OAM_ATTR_H248_PARA_CONFIG;
	p_h248_cfg->width = sizeof(oam_h248_para_cfg_t) - 4;
	ret = stub_mg_port_num_get(&p_h248_cfg->mg_port_no);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get mg port num!\n");
		goto send_rsp;
	}
	ret = stub_mgc_para_get(&p_h248_cfg->mgc_ip,
							&p_h248_cfg->mgc_com_port_no);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get mgc parameters!\n");
		goto send_rsp;
	}
	ret = stub_backup_mgc_para_get(&p_h248_cfg->backup_mgc_ip,
									&p_h248_cfg->backup_mgc_com_port_no);

	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get backup mgc parameters!\n");
		goto send_rsp;
	}

	ret = stub_active_mgc_get(&p_h248_cfg->active_mgc);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get active mgc!\n");
		goto send_rsp;
	}

	ret = stub_reg_mode_get(&p_h248_cfg->reg_mode);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get reg mode!\n");
		goto send_rsp;
	}

	ret = stub_mid_get(&p_h248_cfg->mid[0]);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get mid!\n");
		goto send_rsp;
	}

	ret = stub_heart_beat_mode_get(&p_h248_cfg->heart_beat_mode);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get heart beat mode!\n");
		goto send_rsp;
	}

	ret = stub_heart_beat_cycle_get(&p_h248_cfg->heart_beat_cycle);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get heart beat cycle!\n");
		goto send_rsp;
	}

	ret = stub_heart_beat_count_get(&p_h248_cfg->heart_beat_count);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get heart beat count!\n");
		goto send_rsp;
	}

send_rsp:
#if 0
	if(ret != NO_ERROR);
	{
		/* send var no resource response */
	}
	else
#endif
	{

		p_byte += sizeof(oam_h248_para_cfg_t);

		g_usOutDataIdx += sizeof(oam_h248_para_cfg_t);
	}
	return;
}

void eopl_get_onu_h248_user_tid_config(void)//POTS
{
	u16_t portS, portE;
	u8_t *p_out;
	oam_h248_user_tid_cfg_t *p_h248_user_tid_cfg;
	oam_instance_tlv_t	*p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	u32_t ret;


    OAM_TRACE();
	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

	portMax = OAM_MAX_VOIP_PORT_ID;

	if((portType != OAM_CTC_PORT_TYPE_VOIP) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_out;
		*p_inst = port_instance;
		p_out += sizeof(oam_instance_tlv_t);
		p_rsp = (oam_rsp_indication_t*)p_out;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_H248_USER_TID_CONFIG;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += (sizeof(oam_instance_tlv_t) + sizeof(oam_rsp_indication_t));
		return;
	}


	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = portMax;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	for(; portS<=portE; portS++)
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_out;
		*p_inst = port_instance;

		value = 0;
		value = OAM_SET_PORT_NUM(value, portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;
		p_out += sizeof(oam_instance_tlv_t);

		p_h248_user_tid_cfg = (oam_h248_user_tid_cfg_t*)p_out;
		p_h248_user_tid_cfg->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_h248_user_tid_cfg->leaf = OAM_ATTR_H248_USER_TID_CONFIG;
		p_h248_user_tid_cfg->width = sizeof(oam_h248_user_tid_cfg_t) - 4;

		/* get h248 user tid */
		ret = stub_user_tid_get(portS, &p_h248_user_tid_cfg->user_tid_name[0]);
		if(ret != NO_ERROR)
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get mg port num!\n");
		}

		g_usOutDataIdx += (sizeof(oam_instance_tlv_t) + sizeof(oam_h248_user_tid_cfg_t));
		OAMDBG(("odmPortFlowCtrolEnableGet success, g_usOutDataIdx = %d !\n", g_usOutDataIdx));
	}
	return;
}

void eopl_get_onu_h248_rtp_tid_info(void)
{
	u8_t *p_out;
	oam_h248_rtp_tid_info_t *p_h248_rtp_info;
	u32_t ret;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_h248_rtp_info = (oam_h248_rtp_tid_info_t *)p_out;
	p_h248_rtp_info->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_h248_rtp_info->leaf = OAM_ATTR_H248_RTP_TID_INFO;
	p_h248_rtp_info->width = sizeof(oam_h248_rtp_tid_info_t) - 4;

	ret = stub_num_of_rtp_tid_get(&p_h248_rtp_info->num_of_rtp_tid);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get num of rtp tid!\n");
		goto send_rsp;
	}

	ret = stub_first_rtp_tid_name_get(&p_h248_rtp_info->first_rtp_tid_name[0]);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get first rtp tid name!\n");
		goto send_rsp;
	}

	send_rsp:
#if 0
	if(ret != NO_ERROR);
	{
		/* send var no resource response */
	}
	else
#endif
	{

		p_out += sizeof(oam_h248_rtp_tid_info_t);

		g_usOutDataIdx += sizeof(oam_h248_rtp_tid_info_t);
	}

	return;
}

void eopl_get_onu_sip_para_config(void)
{
	u8_t *p_out;
	oam_sip_para_cfg_t *p_sip_para_cfg;
	u32_t ret;

    OAM_TRACE();
	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_sip_para_cfg = (oam_sip_para_cfg_t *)p_out;
	p_sip_para_cfg->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_sip_para_cfg->leaf = OAM_ATTR_SIP_PARA_CONFIG;
	p_sip_para_cfg->width = sizeof(oam_sip_para_cfg_t) - 4;

	ret = stub_sip_mg_port_no_get(&p_sip_para_cfg->mg_port_no);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get sip mg port num!\n");
		goto send_rsp;
	}

	ret = stub_sip_proxy_server_ip_get(&p_sip_para_cfg->sip_proxy_server_ip);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get sip proxy server ip!\n");
		goto send_rsp;
	}

	ret = stub_sip_proxy_serv_com_port_num_get(&p_sip_para_cfg->sip_proxy_server_com_port_no);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get sip proxy server com port num!\n");
		goto send_rsp;
	}
	ret = stub_backup_sip_proxy_serv_ip_get(&p_sip_para_cfg->backup_sip_proxy_serv_ip);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get backup sip proxy server ip!\n");
		goto send_rsp;
	}
	ret = stub_backup_sip_proxy_serv_com_port_num_get(&p_sip_para_cfg->backup_sip_proxy_serv_com_port_no);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get backup sip proxy com port num!\n");
		goto send_rsp;
	}
	ret = stub_sip_reg_server_com_port_num_get(&p_sip_para_cfg->sip_reg_server_com_port_no);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get reg server com port num!\n");
		goto send_rsp;
	}

	ret = stub_backup_sip_reg_server_ip_get(&p_sip_para_cfg->backup_sip_reg_server_ip);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get sip reg server ip!\n");
		goto send_rsp;
	}
	ret = stub_backup_sip_reg_server_com_port_num_get(&p_sip_para_cfg->backup_sip_reg_server_com_port_no);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get sip reg server com port num!\n");
		goto send_rsp;
	}
	ret = stub_outbound_server_ip_get(&p_sip_para_cfg->outbound_server_ip);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get outbound server ip!\n");
		goto send_rsp;
	}

	ret = stub_outbound_server_port_num_get(&p_sip_para_cfg->outbound_server_port_no);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get outbound server port num!\n");
		goto send_rsp;
	}

	ret = stub_sip_reg_interval_get(&p_sip_para_cfg->sip_reg_interval);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get sip reg interval!\n");
		goto send_rsp;
	}

	ret = stub_sip_heartbeat_switch_get(&p_sip_para_cfg->heartbeat_switch);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get sip heart beat switche!\n");
		goto send_rsp;
	}
	ret = stub_sip_heartbeat_cycle_get(&p_sip_para_cfg->heartbeat_cycle);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get sip heart beat cycle!\n");
		goto send_rsp;
	}
	ret = stub_sip_heartbeat_count_get(&p_sip_para_cfg->heartbeat_count);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get sip heart beat count!\n");
		goto send_rsp;
	}

send_rsp:
#if 0
	if(ret != NO_ERROR);
	{
		/* send var no resource response */
	}
	else
#endif
	{

		p_out += sizeof(oam_sip_para_cfg_t);

		g_usOutDataIdx += sizeof(oam_sip_para_cfg_t);
	}

	return;
}

void eopl_get_onu_sip_user_para(void)//POTS
{
	u16_t portS, portE;
	u8_t *p_out;
	oam_sip_user_para_cfg_t *p_sip_cfg;
	oam_instance_tlv_t	*p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	u32_t ret;


	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

	portMax = OAM_MAX_VOIP_PORT_ID;

	if((portType != OAM_CTC_PORT_TYPE_VOIP) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_out;
		*p_inst = port_instance;
		p_out += sizeof(oam_instance_tlv_t);
		p_rsp = (oam_rsp_indication_t*)p_out;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_SIP_USER_PARA_CONFIG;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += (sizeof(oam_instance_tlv_t) + sizeof(oam_rsp_indication_t));
		return;
	}


	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = portMax;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	for(; portS<=portE; portS++)
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_out;
		*p_inst = port_instance;

		value = 0;
		value = OAM_SET_PORT_NUM(value, portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;
		p_out += sizeof(oam_instance_tlv_t);

		p_sip_cfg = (oam_sip_user_para_cfg_t*)p_out;
		p_sip_cfg->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_sip_cfg->leaf = OAM_ATTR_SIP_USER_PARA_CONFIG;
		p_sip_cfg->width = sizeof(oam_sip_user_para_cfg_t) - 4;

		ret = stub_sip_user_para_get(portS,
									&p_sip_cfg->user_account[0],
									&p_sip_cfg->user_name[0],
									&p_sip_cfg->user_passwd[0]);

		if(ret != NO_ERROR)
			break;

		g_usOutDataIdx += (sizeof(oam_instance_tlv_t) + sizeof(oam_sip_user_para_cfg_t));
		OAMDBG(("odmPortFlowCtrolEnableGet success, g_usOutDataIdx = %d !\n", g_usOutDataIdx));
	}
	return;
}

void eopl_get_onu_fax_modem_config(void)
{
	u8_t *p_out;
	oam_fax_modem_cfg_t *p_fax_modem_cfg;
	u32_t ret;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_fax_modem_cfg = (oam_fax_modem_cfg_t *)p_out;
	p_fax_modem_cfg->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_fax_modem_cfg->leaf = OAM_ATTR_FAX_MODEM_CONFIG;
	p_fax_modem_cfg->width = sizeof(oam_fax_modem_cfg_t) - 4;

	ret = stub_voice_t38_enable_get(&p_fax_modem_cfg->voice_t38_enable);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get voice t38 enable status!\n");
		goto send_rsp;
	}
	ret = stub_voice_fax_modem_control_get(&p_fax_modem_cfg->voice_fax_modem_control);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get voice fax modem control!\n");
		goto send_rsp;
	}

send_rsp:
#if 0
		if(ret != NO_ERROR);
		{
			/* send var no resource response */
		}
		else
#endif
	{

		p_out += sizeof(oam_fax_modem_cfg_t);

		g_usOutDataIdx += sizeof(oam_fax_modem_cfg_t);
	}

	return;
}

void eopl_get_onu_iad_operation_status(void)
{
	u8_t *p_out;
	oam_h248_iad_oper_status_t *p_h248_iad_oper_status;
	u32_t ret;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_h248_iad_oper_status = (oam_h248_iad_oper_status_t *)p_out;
	p_h248_iad_oper_status->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_h248_iad_oper_status->leaf = OAM_ATTR_IAD_OPERATION_STATUS;
	p_h248_iad_oper_status->width = sizeof(oam_h248_iad_oper_status_t) - 4;

	ret = stub_h248_iad_operation_status_get(&p_h248_iad_oper_status->iad_oper_status);
	if(ret != NO_ERROR)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get voice fax modem control!\n");
		goto send_rsp;
	}

send_rsp:
#if 0
	if(ret != NO_ERROR);
	{
		/* send var no resource response */
	}
	else
#endif
	{

		p_out += sizeof(oam_h248_iad_oper_status_t);

		g_usOutDataIdx += sizeof(oam_h248_iad_oper_status_t);
	}

	return;
}

void eopl_get_onu_pots_status()//POTS
{
	u16_t portS, portE;
	u8_t *p_out;
	oam_pots_status_t *p_pots_status;
	oam_instance_tlv_t	*p_inst;
	oam_rsp_indication_t *p_rsp;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	u32_t ret;


	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

	portMax = OAM_MAX_VOIP_PORT_ID;

	if((portType != OAM_CTC_PORT_TYPE_VOIP) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_out;
		*p_inst = port_instance;
		p_out += sizeof(oam_instance_tlv_t);
		p_rsp = (oam_rsp_indication_t*)p_out;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_POTS_STATUS;
		p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += (sizeof(oam_instance_tlv_t) + sizeof(oam_rsp_indication_t));
		return;
	}


	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = portMax;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	for(; portS<=portE; portS++)
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_out;
		*p_inst = port_instance;

		value = 0;
		value = OAM_SET_PORT_NUM(value, portS);
		value = OAM_SET_PORT_TYPE(value, OAM_CTC_PORT_TYPE_ETH);
		p_inst->value = value;
		p_out += sizeof(oam_instance_tlv_t);

		p_pots_status = (oam_pots_status_t*)p_out;
		p_pots_status->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_pots_status->leaf = OAM_ATTR_POTS_STATUS;
		p_pots_status->width = sizeof(oam_pots_status_t) - 4;

		ret = stub_iad_port_status_get(portS, &p_pots_status->iad_port_status);
		if(ret != NO_ERROR)
			break;

		ret = stub_iad_port_service_state_get(portS, &p_pots_status->iad_port_service_state);
		if(ret != NO_ERROR)
			break;

		ret = stub_iad_port_codec_mode_get(portS, &p_pots_status->iad_port_codec_mode);
		if(ret != NO_ERROR)
			break;

		g_usOutDataIdx += (sizeof(oam_instance_tlv_t) + sizeof(oam_pots_status_t));
		OAMDBG(("odmPortFlowCtrolEnableGet success, g_usOutDataIdx = %d !\n", g_usOutDataIdx));
	}
	return;
}

void eopl_get_onu_iad_operation(void)
{
	u8_t *p_out;
	oam_iad_operation_t *p_iad_operation;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_iad_operation = (oam_iad_operation_t *)p_out;
	p_iad_operation->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_iad_operation->leaf = OAM_ATTR_IAD_OPERATION;
	p_iad_operation->width = sizeof(oam_iad_operation_t) - 4;
	p_iad_operation->iad_operation = 0;


	g_usOutDataIdx += sizeof(oam_iad_operation_t);

	return;
}
void eopl_get_onu_sip_digit_map(void)
{
	u8_t *p_out;
	oam_sip_digit_map_t *p_sip_digit_map;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_sip_digit_map = (oam_sip_digit_map_t *)p_out;
	p_sip_digit_map->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_sip_digit_map->leaf = OAM_ATTR_SIP_DIGIT_MAP;
	p_sip_digit_map->width = sizeof(oam_sip_digit_map_t) - 4;
	p_sip_digit_map->sip_digit_map[0] = 0;


	g_usOutDataIdx += sizeof(oam_sip_digit_map_t);

	return;
}


void eopl_get_onu_llid_queue_config(void)
{
	u8_t *p_byte;
	oam_rsp_indication_t *p_rsp;
	oam_instance_tlv_t   *p_inst;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst = port_instance;
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);


	p_rsp = (oam_rsp_indication_t *)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_LLID_QUEUE_CONFIG;
	p_rsp->var_indication = OAM_CTC_RSP_BAD_PARA;

	g_usOutDataIdx += sizeof(oam_rsp_indication_t);

	return;
}

void eopl_get_onu_mxu_mng_global_para(void)
{
	u8_t *p_out;
	oam_mxu_mng_global_para_t *p_global_para;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_global_para = (oam_mxu_mng_global_para_t *)p_out;
	p_global_para->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_global_para->leaf = OAM_ATTR_MXU_MNG_GLOBAL_PARA;
	p_global_para->width = sizeof(oam_mxu_mng_global_para_t) - 4;
	p_global_para->mng_ip_addr = stub_mng_ip_addr;
	p_global_para->mng_ip_mask = stub_mng_ip_mask;
	p_global_para->mng_gw = stub_mng_gw;
	p_global_para->mng_data_cvlan = stub_mng_data_cvlan;
	p_global_para->mng_data_svlan = stub_mng_data_svlan;
	p_global_para->mng_data_priority = stub_mng_data_priority;

	p_out += sizeof(oam_mxu_mng_global_para_t);

	g_usOutDataIdx += sizeof(oam_mxu_mng_global_para_t);

	return;
}

void eopl_get_onu_mxu_mng_snmp_para(void)
{
	u8_t *p_byte;
	oam_mxu_mng_snmp_para_t *p_snmp_para;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_snmp_para = (oam_mxu_mng_snmp_para_t *)p_byte;
	p_snmp_para->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_snmp_para->leaf = OAM_ATTR_MXU_MNG_SNMP_PARA;
	p_snmp_para->width = sizeof(oam_mxu_mng_snmp_para_t) - 4;
	p_snmp_para->snmp_ver = stub_snmp_ver;
	p_snmp_para->trap_host_ip = stub_trap_host_ip;
	p_snmp_para->trap_port = stub_trap_port;
	p_snmp_para->snmp_port = stub_snmp_port;
	strncpy(&p_snmp_para->security_name[0], stub_security_name, 32);
	strncpy(&p_snmp_para->community_for_read[0], stub_community_for_read, 32);
	strncpy(&p_snmp_para->community_for_write[0], stub_community_for_write,32);

	p_byte += sizeof(oam_mxu_mng_snmp_para_t);

	g_usOutDataIdx += sizeof(oam_mxu_mng_snmp_para_t);

	return;

}

void eopl_set_dba(u8_t ucQSetNum, oam_dba_queue_t *p_dba)
{
	u8_t	ucIdx, size, ucQId, *p_byte;
	int		rlt=0;
	oam_dba_queue_t	*p_tmp;

	eopl_ctc_header(OAM_EXT_CODE_DBA);
	p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
#if 1
	*p_byte = OAM_SET_DBA_RESPONSE;
	p_byte++;

	p_tmp = p_dba;

	for(ucIdx=0;ucIdx<ucQSetNum;ucIdx++){ /*fix bug3299.*/
		size = 0;
		//printf("p_dba->bitmap 0x%x ucQSetNum %d.\n", p_dba->bitmap, ucQSetNum);
		for(ucQId=0; ucQId<8; ucQId++){
			if ((p_dba->bitmap)&(1<<ucQId)){	
				//printf("set queueset %d queue %d threshold %d.\n", ucIdx,ucQId,p_dba->threshold[size]);
				rlt = odmPonQsetThresholdSet(ucIdx,ucQId,p_dba->threshold[size]);
				if(rlt != OPL_OK){
					break;
				}
				size++;
			}
		}
		if(rlt != OPL_OK){
			break;
		}
		if (size > 0){
			p_dba = (oam_dba_queue_t*)((u8_t*)p_dba+1+size*2);
		}
	}
	
	
	if(ucIdx<ucQSetNum)
	{
		*p_byte++ = 0x00;
	}
	else
	{
		*p_byte++ = 0x01; /* ack success*/
	}
	*p_byte++ = ucQSetNum;
	size = (u8_t*)p_dba-(u8_t*)p_tmp + sizeof(oam_dba_queue_t);
		vosMemCpy(p_byte,(u8_t*)p_tmp,size);

		p_byte += size;

		size = sizeof(struct oampdu_hdr) + sizeof(struct ctc_header) + 3 + size;

		if(size < OAM_MINIMIZE_PDU_SIZE)
		{
			oam_pdu_padding(p_byte,  OAM_MINIMIZE_PDU_SIZE - size);
			size = OAM_MINIMIZE_PDU_SIZE;
		}
#else
		if(ucQSetNum != 0x02)
		{
			/* DBA code */
			*p_byte = OAM_SET_DBA_RESPONSE;
			p_byte += 1;

			/* NACK */
			*p_byte = 0x00;
			p_byte += 1;

			/* QueueSet Num */
			*p_byte = ucQSetNum;
			p_byte += 1;

			size = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t) + 3;
		}
		else
		{
			/* DBA code */
			*p_byte = OAM_SET_DBA_RESPONSE;
			p_byte += 1;

			/* NACK */
			*p_byte = 0x01;
			p_byte += 1;

			/* QueueSet Num */
			*p_byte = ucQSetNum;
			p_byte += 1;

			/* queueset threshold */
			for(ucQId = 0; ucQId < 8; ucQId++)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "call odmPonQsetThresholdSet(0, %d, %d)\n", ucQId, p_dba->threshold[ucQId]);
				odmPonQsetThresholdSet(0, ucQId, p_dba->threshold[ucQId]);
			}

			for(ucQId = 0; ucQId < 8; ucQId++)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "call odmPonQsetThresholdSet(0, %d, 0xFFFF)\n", ucQId);
				odmPonQsetThresholdSet(1, ucQId, 0xFFFF);
			}

			vosMemCpy(p_byte,(u8_t*)p_dba, sizeof(oam_dba_queue_t));
			p_byte += sizeof(oam_dba_queue_t);

			size = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t) + 3 + sizeof(oam_dba_queue_t);
		}
 	if(size < OAM_MINIMIZE_PDU_SIZE)
 	{
		oam_pdu_padding(p_byte,  OAM_MINIMIZE_PDU_SIZE - size);
		size = OAM_MINIMIZE_PDU_SIZE;
 	}
#endif
	OamFrameSend(out_data, size);
}

void eopl_get_dba(void)
{
	u8_t	size,rlt=0, *p_byte;
	u16_t	value=0xFF;
	u32_t	uiIdx=0;
	oam_dba_queue_t	*p_dba;

	eopl_ctc_header(OAM_EXT_CODE_DBA);
	p_byte = &out_data[0];
	p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	*p_byte = OAM_GET_DBA_RESPONSE;
	p_byte++;
	*p_byte = 3;
	p_byte++;

	p_dba = (oam_dba_queue_t *)p_byte;

	for(size=0;size<=1;size++)
	{
		p_dba->bitmap = 0xFF;
		for(uiIdx=0; uiIdx<8; uiIdx++)
		{
			rlt = odmPonQsetThresholdGet(size,uiIdx,&value);
			if(rlt != OPL_OK)
			{
				return;
			}

			p_dba->threshold[uiIdx] = value;
		}
		p_dba++;
	}

	p_byte += 3*sizeof(oam_dba_queue_t);

	size = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t)
	       			      + 2 + 3*sizeof(oam_dba_queue_t);

 	if(size < OAM_MINIMIZE_PDU_SIZE)
 	{
		oam_pdu_padding(p_byte,  OAM_MINIMIZE_PDU_SIZE - size);
		size = OAM_MINIMIZE_PDU_SIZE;
 	}

	OamFrameSend(out_data, size);
}

bool_t eopl_get_tag_id(u16_t portid,u32_t *value)
{
	u16_t	*p_16;
	u8_t	rlt=OPL_OK,pri=0;
	u32_t	vid=0;

	p_16 = (u16_t*)value;
	rlt = dalVttTpidGet(p_16, portid);
	if(rlt != OPL_OK)
	{
		return -1;
	}

	p_16++;
	rlt = odmPortDefaultVlanGet(portid, &vid);
	if(rlt != OPL_OK)
	{
		return -1;
	}
	rlt = odmPortDefaultEtherPriGet(portid, &pri);
	if(rlt != OPL_OK)
	{
		return -1;
	}

	*p_16 = (vid|(pri<<13));

	return OPL_OK;
}

bool_t eopl_get_vtt_list(u8_t portid,u8_t *p_byte, u8_t *pNum, u32_t uiDefVid)
{
	u32_t	ovlan,nvlan,i, entryValid;
	u8_t numOfVttEntry;
	u32_t validEntryNum;

	if(OPL_OK != odmPortNumOfVttGet(portid, &numOfVttEntry))
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: call odmPortNumOfVttGet fail!\n", __FUNCTION__);
		return -1;
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: call odmPortNumOfVttGet successfully, vtt entry num = %d!\n", __FUNCTION__, numOfVttEntry);
	}

	validEntryNum = 0;
	for(i=0;i < ODM_NUM_OF_VTT;i++)
	{
		if(validEntryNum == numOfVttEntry)
		{
			break;
		}

		if(OPL_OK != odmPortVttEntryGet(portid, i, &entryValid, &ovlan, &ovlan, &nvlan, &nvlan))
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: call odmPortVttEntryGet fail!\n", __FUNCTION__);
			return -1;
		}

		if(!entryValid)
		{
			continue;
		}

		validEntryNum++;

		if((uiDefVid == ovlan) && (ovlan == nvlan))
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: old vlan == new vlan!\n", __FUNCTION__);
			continue;
		}

		vosMemCpy(p_byte,(u8_t*)&ovlan, 4);
		p_byte += 4;
		vosMemCpy(p_byte,(u8_t*)&nvlan, 4);
		p_byte += 4;
	}

	*pNum = numOfVttEntry;

	return OPL_OK;
}


int stub_get_ctc_auth_loid(char *p_loid)
{
	char loid[25];
	char tmpLoid[25];
	int len;
	int i, j;
	UINT32 ret;

	memset(&loid[0], 0, 25);

	ret = odmPonLoidGet(&tmpLoid[0]);

	if(ret == ERR_NULL_LOID)
	{
		vosMemCpy(p_loid, loid, 24);
	}
	else
	{
		len = vosStrLen(tmpLoid);

		for(i = 24-len, j=0; i < 24; i++, j++)
			loid[i] = tmpLoid[j];

		vosMemCpy(p_loid, loid, 24);
	}

	return 0;
}

int stub_get_ctc_auth_password(char *p_passwd)
{
	char password[13];
	char tmpPassword[13];
	int len;
	int i, j;
	UINT32 ret;

	memset(&password[0], 0, 13);

	ret = odmPonPasswordGet(&tmpPassword[0]);

	if(ret == ERR_NULL_PASSWORD)
	{
		vosMemCpy(p_passwd, password, 12);
	}
	else
	{
		/* decrypt password */
		odmUserEncryptString(&tmpPassword[0]);

		len = vosStrLen(tmpPassword);

		for(i = 12-len, j=0; i < 12; i++, j++)
			password[i] = tmpPassword[j];

		vosMemCpy(p_passwd, password, 12);
	}

	return 0;
}


void eopl_ctc_auth_req(u8_t *p_in)
{
	u8_t *p_byte;
	oam_ctc_auth_req_t *p_auth_req;
	oam_ctc_auth_rsp_t *p_auth_rsp;
	u8_t loid[25];
	u8_t password[13];
	u8_t size;
	u8_t i;

	p_byte = p_in;

	p_auth_req = (oam_ctc_auth_req_t *)p_in;

	if(p_auth_req->auth_type == CTC_OAM_AUTH_LOID_PASSWORD)
	{
		/* get loid and password from system */
		stub_get_ctc_auth_loid(&loid[0]);
		stub_get_ctc_auth_password(&password[0]);

		/* response with loid + password */
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		p_byte = &out_data[0];
		size = 0;

		eopl_ctc_header(OAM_EXT_CODE_AUTHENTICATION);

		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		size += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));

		p_auth_rsp = (oam_ctc_auth_rsp_t *)p_byte;

		p_auth_rsp->auth_code = 0x2;
		p_auth_rsp->data_len = 0x25;
		p_auth_rsp->auth_type = 0x1;
		vosMemCpy(p_auth_rsp->loid, loid, 24);
		vosMemCpy(p_auth_rsp->password, password, 12);

		p_byte += sizeof(oam_ctc_auth_rsp_t);
		size += sizeof(oam_ctc_auth_rsp_t);

		if(size < OAM_MINIMIZE_PDU_SIZE)
 		{
			oam_pdu_padding(p_byte,  OAM_MINIMIZE_PDU_SIZE - size);
			size = OAM_MINIMIZE_PDU_SIZE;
 		}

		OamFrameSend(out_data, size);
	}

	return;
}

void eopl_ctc_auth(u8_t *p_byte)
{
	switch(*p_byte)
	{
		case CTC_OAM_AUTH_REQUEST:
            OP_DEBUG(DEBUG_LEVEL_INFO, "CTC_OAM_AUTH_REQUEST");
            g_iOamAuthCode = CTC_OAM_AUTH_REQUEST;
			eopl_ctc_auth_req(p_byte);
			break;
		case CTC_OAM_AUTH_SUCCESS:
            OP_DEBUG(DEBUG_LEVEL_INFO, "CTC_OAM_AUTH_SUCCESS");
            g_iOamAuthCode = CTC_OAM_AUTH_SUCCESS;
			break;
		case CTC_OAM_AUTH_FAILURE:
            OP_DEBUG(DEBUG_LEVEL_WARNING, "CTC_OAM_AUTH_FAILURE");
            g_iOamAuthCode = CTC_OAM_AUTH_FAILURE;
			break;
		default:
			break;
	}

	return;
}

void eopl_not_support_response(u8_t branch ,u16_t leafid,u8_t code)//onu based
{
	u8_t	           *p_out;
	u8_t               oamRspCode;
	oam_rsp_indication_t *p_rsp;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(code);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_rsp_indication_t *)p_out;
	p_out += sizeof(oam_rsp_indication_t);
	g_usOutDataIdx += sizeof(oam_rsp_indication_t);

    p_rsp->branch = branch;
    p_rsp->leaf = leafid;

	p_rsp->var_indication = OAM_GET_NO_RESOURCE;
	g_usOutDataIdx += 4;
}

void eopl_ctc_get_req(u8_t* p_byte)
{
	oam_variable_descriptor_t *p_des;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portNumMax;
	u16_t *pAlarmId;
	u16_t alarmId;
    u8_t *p_src = p_byte;

#ifdef OAM_SUPPORT_REG_CALLBACK
    u8_t *   p_in_buf = NULL;
    u8_t *   p_out_buf = NULL;
    OAM_EV_CB_ID_t key_val;
    vosMemSet(&key_val,0,sizeof(OAM_EV_CB_ID_t));
#endif

	while(1)
	{
        if ((p_byte - p_src) >= OAM_MAXIMUM_PDU_SIZE)
        {
            OP_DEBUG(DEBUG_LEVEL_CRITICAL, "invalid packet size");
            break;
        }

		switch(*p_byte)
		{
			case OAM_BRANCH_INSTANCE_V_2_1:
				port_instance = *((oam_instance_tlv_t*)p_byte);
				p_byte += sizeof(oam_instance_tlv_t);

				break;
			case OAM_BRANCH_INSTANCE_V_2_0:
			/*	port_instance_2_0 = *((oam_instance_tlv_t_2_0*)p_byte);
				p_byte += sizeof(oam_instance_tlv_t_2_0);
				break;*/
                eopl_ctc_get_req_2_0(p_src);
				return;
			case OAM_BRANCH_EXTENDED_ATTRIB:
			case OAM_BRANCH_STANDARD_ATTRIB:
				p_des = (oam_variable_descriptor_t *)p_byte;
				switch(p_des->leaf)
				{
					case OAM_ATTR_ONU_SN:
						eopl_get_onu_sn_rsp();
						break;
					case OAM_ATTR_ONU_FIMWAREVER:
						eopl_get_onu_firmwarever();
						break;
					case OAM_ATTR_CHIPSET_ID:
						eopl_get_onu_chipid();
						break;
					case OAM_ATTR_ONU_CAPABILITIES1:
						eopl_get_onu_capabilities1();
						break;
					case OAM_ATTR_OPTICS_DIAG:
						eopl_get_onu_optical_transceiver_diag();
						break;

					case OAM_ATTR_SERVICE_SLA:
						eopl_get_onu_service_sla();
						break;

					case OAM_ATTR_ONU_CAPABILITIES2:
						eopl_get_onu_capabilities2();
						break;

					case OAM_ATTR_HOLDOVER_CONFIG:
						eopl_get_onu_holdover_config();
						break;

					case OAM_ATTR_MXU_MNG_GLOBAL_PARA:
						eopl_get_onu_mxu_mng_global_para();
						break;

					case OAM_ATTR_MXU_MNG_SNMP_PARA:
						eopl_get_onu_mxu_mng_snmp_para();
						break;

					case OAM_ATTR_ACTIVE_PON_IF_ADMIN:
						eopl_get_onu_active_pon_if_admin();
						break;

					case OAM_ATTR_ETH_LINKSTATE:
						eopl_get_onu_eth_linkstate();
						break;

					case OAM_ATTR_ETH_PORTPAUSE:
						eopl_get_onu_eth_port_pause();
						break;

					case OAM_ATTR_ETH_PORT_US_POLICING:
						eopl_get_onu_eth_port_us_policing();
						break;

#if defined(CTC_VOIP_DISPATCH)  /* modified by Gan Zhiheng - 2010/06/12 */
					case OAM_ATTR_VOIP_PORT:
						oamMsqSend(&in_data[4],in_data_len);
						return;
#else
					case OAM_ATTR_VOIP_PORT:
						eopl_get_onu_voip_port();
						break;
#endif
					case OAM_ATTR_E1_PORT:
						eopl_get_onu_e1_port();
						break;

					case OAM_ATTR_ETH_PORT_DS_POLICING:
						eopl_get_onu_eth_port_ds_policing();
						break;

					case OAM_ATTR_VLAN:
						eopl_get_onu_vlan();
						break;

					case OAM_ATTR_CLAMARK:
						eopl_get_onu_classify();
						p_byte += 2;
						break;

					case OAM_ATTR_MCAST_VLAN:
						eopl_get_onu_mcast_vlan();
						p_byte += 2;
						break;
					case OAM_ATTR_MCAST_TAG_STRIP:
						eopl_get_onu_mcast_tagstrip();
						break;
					case OAM_ATTR_MCAST_SWITCH:
						eopl_get_onu_mcast_switch();
						break;
					case OAM_ATTR_MCAST_CONTROL:
						eopl_get_onu_mcast_control();
						p_byte += 2;
						break;
					case OAM_ATTR_MCAST_GRP_NUMMAX:
						eopl_get_onu_mcast_grp_nummax();
						break;
					case OAM_ATTR_MCAST_GFASTLEAVE_ABL:
						eopl_get_onu_mcast_fast_leave_abl();
						break;
					case OAM_ATTR_MCAST_GFASTLEAVE_ADMIN:
						eopl_get_onu_mcast_fast_leave_admin();
						break;

					case OAM_ATTR_LLID_QUEUE_CONFIG:
						eopl_get_onu_llid_queue_config();
						break;

					case OAM_ATTR_ALARM_ADMIN_STATE:
						eopl_get_onu_alarm_admin_state(p_byte);
						break;

					case OAM_ATTR_ALARM_THRESHOLD:
						eopl_get_onu_alarm_threshold(p_byte);
						break;

#if defined(CTC_VOIP_DISPATCH)  /* modified by Gan Zhiheng - 2010/06/08 */
					case OAM_ATTR_IAD_INFO:
					case OAM_ATTR_GLOBAL_PARA_CONFIG:
					case OAM_ATTR_H248_PARA_CONFIG:
					case OAM_ATTR_H248_USER_TID_CONFIG:
					case OAM_ATTR_H248_RTP_TID_CONFIG:
					case OAM_ATTR_H248_RTP_TID_INFO:
					case OAM_ATTR_SIP_PARA_CONFIG:
					case OAM_ATTR_SIP_USER_PARA_CONFIG:
					case OAM_ATTR_FAX_MODEM_CONFIG:
					case OAM_ATTR_IAD_OPERATION_STATUS:
					case OAM_ATTR_POTS_STATUS:
					case OAM_ATTR_IAD_OPERATION:
					case OAM_ATTR_SIP_DIGIT_MAP:
						oamMsqSend(&in_data[4],in_data_len);
						return;
#else
					case OAM_ATTR_IAD_INFO:
						eopl_get_onu_iad_info();
						break;

					case OAM_ATTR_GLOBAL_PARA_CONFIG:
						eopl_get_onu_global_para_config();
						break;

					case OAM_ATTR_H248_PARA_CONFIG:
						eopl_get_onu_h248_para_config();
						break;

					case OAM_ATTR_H248_USER_TID_CONFIG: //POTS
						eopl_get_onu_h248_user_tid_config();
						break;

					case OAM_ATTR_H248_RTP_TID_INFO:
						eopl_get_onu_h248_rtp_tid_info();
						break;

					case OAM_ATTR_SIP_PARA_CONFIG:
						eopl_get_onu_sip_para_config();
						break;

					case OAM_ATTR_SIP_USER_PARA_CONFIG://POTS
						eopl_get_onu_sip_user_para();
						break;

					case OAM_ATTR_FAX_MODEM_CONFIG:
						eopl_get_onu_fax_modem_config();
						break;

					case OAM_ATTR_IAD_OPERATION_STATUS:
						eopl_get_onu_iad_operation_status();
						break;

					case OAM_ATTR_POTS_STATUS://POTS
						eopl_get_onu_pots_status();
						break;

					case OAM_ATTR_IAD_OPERATION:
						eopl_get_onu_iad_operation();
						break;

					case OAM_ATTR_SIP_DIGIT_MAP:
						eopl_get_onu_sip_digit_map();
						break;
#endif

					/* standard attribution */
					case OAM_ATTR_PHY_ADMIN:
						eopl_get_onu_eth_phyadmin();
						break;
					case OAM_ATTR_AUTONEG_ADMIN_STATE:
						eopl_get_onu_eth_autonegadmin();
						break;
					case OAM_ATTR_AUTONEG_LOCAL_ABILITY:
						eopl_get_onu_eth_autoneglocal();
						break;

					case OAM_ATTR_AUTONEG_ADV_ABILITY:
						eopl_get_onu_eth_autonegadv();
						break;
					case OAM_ATTR_FEC_ABILITY:
						eopl_get_onu_eth_fecability();
						break;
					case OAM_ATTR_FEC_MODE:
						eopl_get_onu_eth_fecmode();
						break;
					case OAM_ATTR_PORT_LOOP_DETECT:
						eopl_get_onu_port_loop_detect();
						break;
					default:
#if defined(OAM_SUPPORT_REG_CALLBACK)
                        //OAM_DBG("[%s]Unknown leaf value,so search the OAM CALLBACK table\n ",__FUNCTION__ );
                        key_val.oam_ver = OAM_VER_CTC_2_1_SUPPORT;
                        key_val.oam_ext_code = OAM_EXT_CODE_GET_REQUEST ;
                        key_val.oam_branch_id = *p_byte;
                        key_val.oam_leaf_id = p_des->leaf;

                        p_in_buf = p_byte;
                        p_out_buf = eopl_get_output_buf(OAM_EXT_CODE_GET_REQUEST );
                        OamEventCallBack(key_val,
                                         p_in_buf,
                                         p_out_buf);
#elif defined(OAM_SUPPORT_ZTE_EXT) /* ZTE CTC extension */
						eopl_zte_ctc_get_req(p_des->leaf, &p_byte);
#else
                        eopl_not_support_response((u8_t)*p_byte,p_des->leaf,OAM_EXT_CODE_GET_RESPONSE);
                        OAM_DBG("[%s]:%d Unknown leaf value!!\n ",__FUNCTION__, __LINE__);
#endif
						break;
				}
				p_byte += sizeof(oam_variable_descriptor_t);
				break;
			default:
				if(g_usOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
			 	{
					oam_pdu_padding(&out_data[g_usOutDataIdx],  OAM_MINIMIZE_PDU_SIZE - g_usOutDataIdx);
					g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
			 	}
				OamFrameSend(out_data, g_usOutDataIdx);
				memset(&port_instance, 0, sizeof(oam_instance_tlv_t));
				//memset(&port_instance_2_0, 0, sizeof(oam_instance_tlv_t_2_0));
				g_usOutDataIdx = 0;
				return;
		}
	 }

}

void eopl_set_onu_service_sla(u8_t *p_in)
{
	u8_t *p_byte;
	u8_t *p_in_byte;
	oam_set_response_t *p_rsp;
	oam_service_sla_t *p_sla;
	oam_service_sla_body_hdr_t   *p_body_hdr;
	oam_service_sla_body_entry_t *p_body_entry;
	u8_t schedule_scheme;
	u8_t high_pri_boundry;
	u32_t cycle_len;
	u8_t queue_num;
	u8_t i;
	u32_t ret;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_in_byte = p_in;
	p_sla = (oam_service_sla_t *)p_in_byte;

	if(p_sla->operation == OAM_CTC_SERVICE_DBA_DEACTIVE)
	{
		/*
		 * set service sla parameters
		 */

		ret = 0;
	}
	else if(p_sla->operation == OAM_CTC_SERVICE_DBA_ACTIVE)
	{
		p_in_byte += sizeof(oam_service_sla_t);
		p_body_hdr = (oam_service_sla_body_hdr_t *)p_in_byte;

		schedule_scheme = p_body_hdr->schedule_scheme;
		high_pri_boundry = p_body_hdr->high_pri_boundry;
		cycle_len = p_body_hdr->cycle_len;
		queue_num = p_body_hdr->service_num;
		/*
		 * set service sla parameters
		 */


		p_in_byte += sizeof(oam_service_sla_body_hdr_t);
		p_body_entry = (oam_service_sla_body_entry_t *)p_in_byte;

		for(i = 0; i < queue_num; i++)
		{
			/*
			 * set queue parameters
			 */

			ret = 0;
			p_body_entry++;
		}
	}

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_SERVICE_SLA;

	p_rsp->indication = OAM_SET_OK;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}


void eopl_set_onu_holdover_config(u8_t *p_in)
{
	u8_t *p_out;
	oam_onu_holdover_config_t *p_holdover_cfg;
	oam_set_response_t *p_rsp;
	u32_t holdover_state;
	u32_t holdover_time;
	u32_t ret;

	ret = 0;

	p_holdover_cfg = (oam_onu_holdover_config_t *)p_in;
	holdover_state = p_holdover_cfg->holdover_state;
	holdover_time = p_holdover_cfg->holdover_time;


	ret += odmPonMpcpFsmHoldoverControlSet(holdover_state-1);
	ret += odmPonMpcpFsmHoldoverTimeSet(holdover_time);

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t *)p_out;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_HOLDOVER_CONFIG;

	if(ret == 0)
		p_rsp->indication = OAM_SET_OK;
	else
		p_rsp->indication = OAM_SET_BAD_PARAMETERS;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

u32_t stub_set_onu_active_pon_if_admin(u8_t active_port_id)
{
	stub_active_port = active_port_id;

	OAMDBG(("[ %s ]: %d\n", __FUNCTION__, stub_active_port));
	return 0;
}

void eopl_set_onu_active_pon_if_admin(u8_t *p_in)
{
	u8_t *p_out;
	oam_active_pon_if_admin_t *p_active_pon_admin;
	oam_set_response_t *p_rsp;
	u8_t active_port_id;
	u32_t ret;

	p_active_pon_admin = (oam_active_pon_if_admin_t *)p_in;

	active_port_id = p_active_pon_admin->active_port_id;

	ret = stub_set_onu_active_pon_if_admin(active_port_id);

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t *)p_out;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_ACTIVE_PON_IF_ADMIN;

	if(ret == 0)
		p_rsp->indication = OAM_SET_OK;
	else
		p_rsp->indication = OAM_SET_BAD_PARAMETERS;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_eth_port_pause(u8_t action)
{
	int rlt=OPL_OK;
	u16_t portS, portE;
	u8_t *p_byte;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t	*p_inst;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	u32_t pause_state;

	pause_state = (u32_t)action;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = CTC_MAX_SUBPORT_NUM;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_set_response_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_ETH_PORTPAUSE;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_instance_tlv_t) + sizeof(oam_rsp_indication_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst =   port_instance;
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_ETH_PORTPAUSE;

	for(; portS<=portE; portS++)
	{
		/* OAMDBG(("[ %s ]: port %d flow ctrl set: %d\n", __FUNCTION__, portS, action)); */
		#if defined(CONFIG_PRODUCT_EPN200)
		rlt = odmPortFlowCtrolEnableSet(portS, pause_state);
		#else
		rlt = DRV_SetPortFlowCtrl(portS, (TRUE == pause_state)? TRUE : FALSE);
		#endif
		/* OAMDBG(("[ %s ]: ret = %d\n", __FUNCTION__, rlt)); */
	}
	if(rlt == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_eth_port_us_policing(u8_t operation,u8_t* p_cir,u8_t* p_cbs,u8_t* p_ebs)
{
	u8_t	rlt=0;
	u16_t   portS, portE;
	u8_t    *p_byte;
	u32_t	cir=0, cbs=0, ebs=0;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t	*p_inst;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = CTC_MAX_SUBPORT_NUM;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_set_response_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_ETH_PORT_US_POLICING;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_rsp_indication_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst = port_instance;
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);
	p_rsp = (oam_set_response_t*)p_byte;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_ETH_PORT_US_POLICING;

	if(operation == 1)
	{
 		vosMemCpy((u8_t*)&cir+1, p_cir, 3);
		vosMemCpy((u8_t*)&cbs+1, p_cbs, 3);
		vosMemCpy((u8_t*)&ebs+1, p_ebs, 3);
	}
	for(; portS<=portE; portS++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "us policing, cir: %d, cbs: %d, ebs: %d\n", cir, cbs, ebs);
		#if defined(CONFIG_PRODUCT_EPN200)
		rlt = odmPortUsPolicingSet(portS, operation, cir,cbs,ebs);
		#else
        rlt = DRV_SetPortLineRate(portS, DIRECT_IN, cir);
		#endif
	}

	if(rlt == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_voip_port(u8_t lock)
{
	u8_t *p_byte;
	oam_instance_tlv_t *p_inst;
	oam_set_response_t *p_rsp;
	u32_t ret;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	u16_t portS;
	u16_t portE;
	u16_t voip_port = 0;

	portMax = OAM_MAX_VOIP_PORT_ID;

    OAM_TRACE();
	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

	#if 0
	if ( portNum >= 0x50 ){
	#else
	if ( portNum >= 0x50 && portNum != 0xFFFF){
	#endif
		voip_port = portNum - 0x50;
	}else{
		voip_port = portNum - odmSysVoIPPortBaseGet();
	}
	OAMDBG(("[%s]: portType = %d, portNum = %d\n", __FUNCTION__, portType, portNum));
	if((portType != OAM_CTC_PORT_TYPE_VOIP) || ((voip_port > portMax) && (portNum != 0xFFFF)))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_set_response_t *)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_VOIP_PORT;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_set_response_t);

		return;
	}

	if(0xFFFF == portNum)
	{
		#if 0
		portS = 1;
		#else
		portS = 0;
		#endif
		portE = portMax;
	}
	else
	{
		portS = voip_port;
		portE = voip_port;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst =   port_instance;
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_VOIP_PORT;

#if 0
    CfmAgent_Init("127.0.0.1", 0);
	CfmAgent_PostGroupBegin();
	for(; portS<=portE; portS++)
	{
		ret = stub_voip_port_set(portS, lock);
		/* do voip lock or unlock */
		stub_voip_lock[portS] = lock;
		//ret = OPL_OK;
	}
    CfmAgent_PostGroupEnd(0);
	CfAgent_Exit();
#endif
	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_e1_port(u8_t lock)
{
	u8_t *p_byte;
	oam_instance_tlv_t *p_inst;
	oam_set_response_t *p_rsp;
	u32_t ret;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	u16_t portS;
	u16_t portE;
	u16_t e1_port = 0;

	portMax = OAM_MAX_E1_PORT_ID;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

	if ( portNum >= 0x90 && portNum != 0xFFFF){
		e1_port = portNum - 0x90;
	}

	OAMDBG(("[%s]: portType = %d, portNum = %d\n", __FUNCTION__, portType, portNum));

	if((portType != OAM_CTC_PORT_TYPE_E1) || ((e1_port > portMax) && (portNum != 0xFFFF)))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_set_response_t *)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_E1_PORT;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_set_response_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 0;
		portE = portMax;
	}
	else
	{
		portS = e1_port;
		portE = e1_port;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst =   port_instance;
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_E1_PORT;

	for(; portS<=portE; portS++)
	{
		/* do E1 port lock or unlock */
		stub_e1_lock[portS] = lock;
		ret = OPL_OK;
	}

	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_eth_port_ds_policing(u8_t operation,u8_t* p_cir,u8_t* p_pir)
{
	u8_t	rlt=0;
	u16_t   portS, portE;
	u8_t    *p_byte;
	u32_t	cir=0, pir=0;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t	*p_inst;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = CTC_MAX_SUBPORT_NUM;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_set_response_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_ETH_PORT_DS_POLICING;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_instance_tlv_t) + sizeof(oam_rsp_indication_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst =   port_instance;
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_ETH_PORT_DS_POLICING;

	if(operation == 1)
	{
		vosMemCpy((u8_t*)&cir+1, p_cir, 3);
		vosMemCpy((u8_t*)&pir+1, p_pir, 3);
	}
	for(; portS<=portE; portS++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "ds policing, cir: %d, pir: %d\n", cir, pir);
		#if defined(CONFIG_PRODUCT_EPN200)
		rlt = odmPortDsPolicingSet(portS, operation, cir,pir);
		#else
		rlt = DRV_SetPortLineRate(portS, DIRECT_OUT, cir);
		#endif
	}
	if(rlt == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_multi_llid_admin(u32_t llid_num)
{
	u8_t *p_byte;
	oam_set_response_t *p_rsp;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	/*
	 * call onu api to set llid num
	 */
	p_rsp = (oam_set_response_t *)p_byte;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ACTION;
	p_rsp->leaf = OAM_ACTION_MULTI_LLID_ADMIN;
	p_rsp->indication = OAM_SET_BAD_PARAMETERS;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_reset_card(void)
{
	u32_t card_id;
	u8_t *p_byte;
	oam_set_response_t *p_rsp;
	oam_instance_tlv_t	*p_inst;

	card_id = (u32_t)port_instance.value;

	/*
	 * call onu card reset api
	 */
	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst =   port_instance;
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	p_rsp = (oam_set_response_t *)p_byte;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ACTION;
	p_rsp->leaf = OAM_ACTION_RESET_CARD;
	p_rsp->indication = OAM_SET_OK;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_eth_admincontrol(u32_t action)
{
	u8_t	rlt=0;
	u16_t   portS, portE;
	u8_t    *p_byte;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t	*p_inst;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = CTC_MAX_SUBPORT_NUM;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_set_response_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ACTION_PHY_ADMIN_CONTROL;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_instance_tlv_t) + sizeof(oam_set_response_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst =   port_instance;
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_STANDARD_ACTION;
	p_rsp->leaf = OAM_ACTION_PHY_ADMIN_CONTROL;

	if(1 == action)
	{
		action = 0;
	}
	else if(2 == action)
	{
		action = 1;
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\r\nunknown admin action\r\n");
	}

	for(; portS<=portE; portS++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "mc admin ctrl: %d\n", action);
        #if 0
		rlt = odmPortAdminSet(portS, action);
		#else
        rlt = DRV_SetPortLinkState (portS, action);
		#endif
		if(rlt == OPL_OK)
		{
			p_rsp->indication = OAM_SET_OK;
		}
		else
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmPortAdminSet fail\n");
			p_rsp->indication = OAM_SET_NO_RESOURCE;
		}
	}
	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_eth_atuorestart(void)
{
	u8_t	rlt=0;
	u16_t   portS, portE;
	u8_t    *p_byte;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t	*p_inst;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = CTC_MAX_SUBPORT_NUM;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		p_rsp = (oam_set_response_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ACTION_AUTONEG_RST_AUTOCONFIG;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_instance_tlv_t) + sizeof(oam_set_response_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst =   port_instance;
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_STANDARD_ACTION;
	p_rsp->leaf = OAM_ACTION_AUTONEG_RST_AUTOCONFIG;

	for(; portS<=portE; portS++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "autoneg restart\n");
		#if 0
		rlt = dalPhyAutonegRestart(portS);
		#else
        rlt = DRV_PortRestartNeg(portS);
		#endif
		if(rlt == OPL_OK)
		{
			p_rsp->indication = OAM_SET_OK;
		}
		else
		{
		    p_rsp->indication = OAM_SET_NO_RESOURCE;
		}
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_eth_autonegadmin(u8_t action)
{
	u8_t	rlt=0, ucPortS, ucPortE, *p_byte;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t	*p_inst;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else
	portMax = CTC_MAX_SUBPORT_NUM;
#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		p_rsp = (oam_set_response_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ACTION_AUTONEG_ADMIN_CONTROL;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_instance_tlv_t) + sizeof(oam_set_response_t);
		return;
	}


	if(0xFFFF == portNum)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = portNum;
		ucPortE = portNum;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst =   port_instance;
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_STANDARD_ACTION;
	p_rsp->leaf = OAM_ACTION_AUTONEG_ADMIN_CONTROL;

	if(1 == action)
	{
		action = 0;
	}
	else if(2 == action)
	{
		action = 1;
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\r\nunknown admin action\r\n");
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "autoneg ctrl: %d\n", action);
        #if defined(CONFIG_PRODUCT_EPN200)
        #if 0
		rlt = odmPortAutoEnableSet(ucPortS, action);
        #else
        rlt = OPL_NOT_SUPPORTED;
        #endif
		#else
        rlt = DRV_SetPortAutoNeg((u32_t)ucPortS, action);
		#endif
		if(rlt == OPL_OK)
		{
			p_rsp->indication = OAM_SET_OK;
		}
		else
		{

			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmPortAutoEnableSet fail\n");
			p_rsp->indication = OAM_SET_NO_RESOURCE;
		}
	}
	g_usOutDataIdx += sizeof(oam_set_response_t);
	return;
}

void eopl_set_onu_global_para_cfg(u8_t *p_in)
{
	u8_t *p_out;
	oam_global_para_cfg_t *p_global_para;
	oam_set_response_t *p_rsp;
	u32_t ret;

	p_global_para = (oam_global_para_cfg_t *)p_in;

    OAM_TRACE();
	/* set voice ip mode */
	ret = stub_voice_ip_mode_set(p_global_para->voice_ip_mode);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set voice ip mode!\n");
		goto send_rsp;
	}

	/* set iad ip addr parameters */
	ret = stub_iad_ip_addr_para_set(p_global_para->iad_ipaddr,
									p_global_para->iad_netmask,
									p_global_para->iad_defaultgw);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set ip addr parameters!\n");
		goto send_rsp;
	}

	/* set pppoe mode */
	ret = stub_pppoe_mode_set(p_global_para->pppoe_mode);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set pppoe mode!\n");
		goto send_rsp;
	}

	/* set pppoe parameters */
	ret = stub_pppoe_para_set(&p_global_para->pppoe_username[0],
								&p_global_para->pppoe_passwd[0]);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set pppoe parameters!\n");
		goto send_rsp;
	}

	/* set voice data parameters */
	ret = stub_voice_data_para_set(p_global_para->tagged_flag,
								p_global_para->voice_cvlan,
								p_global_para->voice_svlan,
								p_global_para->voice_priority);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set voice data parameters!\n");
		goto send_rsp;
	}

send_rsp:

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t *)p_out;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_GLOBAL_PARA_CONFIG;

	if(ret == 0)
		p_rsp->indication = OAM_SET_OK;
	else
		p_rsp->indication = OAM_SET_NO_RESOURCE;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}
void eopl_set_onu_h248_para_cfg(u8_t *p_in)
{
	u8_t *p_out;
	oam_h248_para_cfg_t *p_h248_para;
	oam_set_response_t *p_rsp;
	u32_t ret;

	p_h248_para = (oam_h248_para_cfg_t *)p_in;

    OAM_TRACE();
	/* set mg port num */
	ret = stub_mg_port_num_set(p_h248_para->mg_port_no);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set mg port num!\n");
		goto send_rsp;
	}

	/* set mgc parameters */
	ret = stub_mgc_para_set(p_h248_para->mgc_ip,
							p_h248_para->mgc_com_port_no);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set mgc parameter!\n");
		goto send_rsp;
	}

	/* set backup mgc parameters */
	ret = stub_backup_mgc_para_set(p_h248_para->mgc_ip,
									p_h248_para->mgc_com_port_no);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set backup mgc parameter!\n");
		goto send_rsp;
	}

	/* set active mgc */
	ret = stub_active_mgc_set(p_h248_para->active_mgc);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set active mgc!\n");
		goto send_rsp;
	}

	/* set reg mode */
	ret = stub_reg_mode_set(p_h248_para->reg_mode);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set reg mode!\n");
		goto send_rsp;
	}

	/* set mid */
	ret = stub_mid_set(&p_h248_para->mid[0]);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set mid!\n");
		goto send_rsp;
	}

	/* set heartbeat mode */
	ret = stub_heart_beat_mode_set(p_h248_para->heart_beat_mode);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set heart beat mode!\n");
		goto send_rsp;
	}

	/* set heartbeat cycle */
	ret = stub_heart_beat_cycle_set(p_h248_para->heart_beat_cycle);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set heart beat cycle!\n");
		goto send_rsp;
	}

	/* set heartbeat count */
	ret = stub_heart_beat_count_set(p_h248_para->heart_beat_count);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set heart beat count!\n");
		goto send_rsp;
	}

send_rsp:

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t *)p_out;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_H248_PARA_CONFIG;

	if(ret == 0)
		p_rsp->indication = OAM_SET_OK;
	else
		p_rsp->indication = OAM_SET_NO_RESOURCE;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}
void eopl_set_onu_h248_user_tid_cfg(u8_t *p_in)
{
	u8_t *p_out;
	oam_instance_tlv_t *p_inst;
	oam_set_response_t *p_rsp;
	oam_h248_user_tid_cfg_t *p_user_tid_cfg;
	u32_t ret;
	u32_t value;
	u8_t  portType;
	u16_t portNum;
	u16_t portMax;
	u16_t portS;
	u16_t portE;

	portMax = OAM_MAX_VOIP_PORT_ID;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[%s]: portType = %d, portNum = %d\n", __FUNCTION__, portType, portNum);

	if((portType != OAM_CTC_PORT_TYPE_VOIP) || ((portNum > portMax) && (portNum != 0xFFFF)))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_out;
		*p_inst = port_instance;
		p_out += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_set_response_t *)p_out;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_H248_USER_TID_CONFIG;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_set_response_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = portMax;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_out;
	*p_inst =   port_instance;
	p_out += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_H248_USER_TID_CONFIG;

	p_user_tid_cfg = (oam_h248_user_tid_cfg_t *)p_in;

	for(; portS<=portE; portS++)
	{
		/* set h248 user tid name */
		ret = stub_user_tid_set(portS, &p_user_tid_cfg->user_tid_name[0]);
		if(ret != NO_ERROR)
		{
			break;
		}
	}

	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}
void eopl_set_onu_h248_rtp_tid_cfg(u8_t *p_in)
{
	u8_t *p_out;
	oam_h248_rtp_tid_cfg_t *p_h248_rtp_tid_cfg;
	oam_set_response_t *p_rsp;
	u32_t ret;

	p_h248_rtp_tid_cfg = (oam_h248_rtp_tid_cfg_t *)p_in;

    OAM_TRACE();
	/* set num of rtp tid */
	ret = stub_num_of_rtp_tid_set(&p_h248_rtp_tid_cfg->num_of_rtp_tid);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set num of rtp tid!\n");
		goto send_rsp;
	}

	/* set rtp tid prefix */
	ret = stub_rtp_tid_prefix_set(&p_h248_rtp_tid_cfg->rtp_tid_prefix[0]);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set rtp tid prefix!\n");
		goto send_rsp;
	}

	/* set rtp tid digit begin */
	ret = stub_rtp_tid_digit_len_set(&p_h248_rtp_tid_cfg->rtp_tid_digit_begin[0]);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set rtp tid digit begin!\n");
		goto send_rsp;
	}

	/* set rtp tid mode */
	ret = stub_rtp_tid_mode_set(p_h248_rtp_tid_cfg->rtp_tid_mode);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set rtp tid mode!\n");
		goto send_rsp;
	}

	/* set rtp tid digit len */
	ret = stub_rtp_tid_digit_len_set(p_h248_rtp_tid_cfg->rtp_tid_digit_len);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set rtp tid digit len!\n");
		goto send_rsp;
	}

send_rsp:

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t *)p_out;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_H248_RTP_TID_CONFIG;

	if(ret == 0)
		p_rsp->indication = OAM_SET_OK;
	else
		p_rsp->indication = OAM_SET_NO_RESOURCE;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}
void eopl_set_onu_sip_para_cfg(u8_t *p_in)
{
	u8_t *p_out;
	oam_sip_para_cfg_t *p_sip_para_cfg;
	oam_set_response_t *p_rsp;
	u32_t ret;

	p_sip_para_cfg = (oam_sip_para_cfg_t *)p_in;

    OAM_TRACE();
	/* set mg port num */
	ret = stub_sip_mg_port_no_set(&p_sip_para_cfg->mg_port_no);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set mg port num!\n");
		goto send_rsp;
	}

	/* set sip proxy server ip */
	ret = stub_sip_proxy_server_ip_set(&p_sip_para_cfg->sip_proxy_server_ip);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set sip proxy server ip!\n");
		goto send_rsp;
	}

	/* set sip proxy server com port num */
	ret = stub_sip_proxy_serv_com_port_num_get(&p_sip_para_cfg->sip_proxy_server_com_port_no);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set sip proxy server com port num!\n");
		goto send_rsp;
	}

	/* set backup sip proxy server ip */
	ret = stub_backup_sip_proxy_serv_ip_set(p_sip_para_cfg->backup_sip_proxy_serv_ip);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set backup sip proxy server ip!\n");
		goto send_rsp;
	}

	/* set backup sip proxy server com port num */
	ret = stub_backup_sip_proxy_serv_com_port_num_set(p_sip_para_cfg->backup_sip_proxy_serv_com_port_no);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set backup sip proxy server com port num!\n");
		goto send_rsp;
	}

	/* set active sip proxy server */
	ret = stub_active_sip_proxy_server_set(p_sip_para_cfg->active_sip_proxy_server);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set active sip proxy server!\n");
		goto send_rsp;
	}
	/* set sip reg server ip */
	ret = stub_sip_reg_server_com_port_num_set(p_sip_para_cfg->sip_reg_server_ip);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set  sip reg server ip!\n");
		goto send_rsp;
	}
	/* set sip reg server com port num */
	ret = stub_sip_reg_server_com_port_num_set(p_sip_para_cfg->sip_reg_server_com_port_no);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set sip reg server com port num!\n");
		goto send_rsp;
	}
	/* set backup sip reg server ip */
	ret = stub_backup_sip_reg_server_ip_set(p_sip_para_cfg->backup_sip_reg_server_ip);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set backup sip reg server ip!\n");
		goto send_rsp;
	}
	/* set backup sip reg server com port num */
	ret = stub_backup_sip_reg_server_com_port_num_set(p_sip_para_cfg->backup_sip_reg_server_com_port_no);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set backup sip reg server com port num!\n");
		goto send_rsp;
	}
	/* set outbound server ip */
	ret = stub_outbound_server_ip_set(p_sip_para_cfg->outbound_server_ip);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set outbound server ip\n");
		goto send_rsp;
	}

	/* set outbound server port num */
	ret = stub_outbound_server_port_num_set(p_sip_para_cfg->outbound_server_port_no);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set outbound server port num!\n");
		goto send_rsp;
	}
	/* set sip reg interval */
	ret = stub_sip_reg_interval_set(p_sip_para_cfg->sip_reg_interval);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set sip reg interval!\n");
		goto send_rsp;
	}
	/* set sip heart beat switch */
	ret = stub_sip_heartbeat_switch_set(p_sip_para_cfg->heartbeat_switch);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set sip heart beat switch!\n");
		goto send_rsp;
	}
	/* set sip heart beat scycle */
	ret = stub_sip_heartbeat_cycle_set(p_sip_para_cfg->heartbeat_cycle);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set sip heart beat scycl!\n");
		goto send_rsp;
	}
	/* set sip heart beat count */
	ret = stub_sip_heartbeat_count_set(p_sip_para_cfg->heartbeat_count);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set sip heart beat count!\n");
		goto send_rsp;
	}

send_rsp:

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t *)p_out;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_SIP_PARA_CONFIG;

	if(ret == 0)
		p_rsp->indication = OAM_SET_OK;
	else
		p_rsp->indication = OAM_SET_NO_RESOURCE;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;

}
void eopl_set_onu_sip_user_para_cfg(u8_t *p_in)
{
	u8_t *p_out;
	oam_instance_tlv_t *p_inst;
	oam_set_response_t *p_rsp;
	oam_sip_user_para_cfg_t *p_sip_user_para_cfg;
	u32_t ret;
	u32_t value;
	u8_t  portType;
	u16_t portNum;
	u16_t portMax;
	u16_t portS;
	u16_t portE;

	portMax = OAM_MAX_VOIP_PORT_ID;

    OAM_TRACE();
	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[%s]: portType = %d, portNum = %d\n", __FUNCTION__, portType, portNum);

	if((portType != OAM_CTC_PORT_TYPE_VOIP) || ((portNum > portMax) && (portNum != 0xFFFF)))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_out;
		*p_inst = port_instance;
		p_out += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_set_response_t *)p_out;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_H248_USER_TID_CONFIG;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_set_response_t);
		return;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = portMax;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_out;
	*p_inst =   port_instance;
	p_out += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_SIP_USER_PARA_CONFIG;

	p_sip_user_para_cfg = (oam_sip_user_para_cfg_t *)p_in;

	for(; portS<=portE; portS++)
	{
		/* set sip user parameter  */
		ret = stub_sip_user_para_set(portS,
									&p_sip_user_para_cfg->user_account[0],
									&p_sip_user_para_cfg->user_name[0],
									&p_sip_user_para_cfg->user_passwd[0]);
		if(ret != NO_ERROR)
		{
			break;
		}
	}

	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}
void eopl_set_onu_fax_modem_cfg(u8_t *p_in)
{
	u8_t *p_out;
	oam_fax_modem_cfg_t *p_onu_fax_modem_cfg;
	oam_set_response_t *p_rsp;
	u32_t ret;

	p_onu_fax_modem_cfg = (oam_fax_modem_cfg_t *)p_in;

    OAM_TRACE();
	/* set vocie t38 enable status  */
	ret = stub_voice_t38_enable_set(p_onu_fax_modem_cfg->voice_t38_enable);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set vocie t38 enable status!\n");
		goto send_rsp;
	}
	/* set voice fax control */
	ret = stub_voice_fax_modem_control_set(p_onu_fax_modem_cfg->voice_fax_modem_control);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set voice fax control!\n");
		goto send_rsp;
	}

send_rsp:

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t *)p_out;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_FAX_MODEM_CONFIG;

	if(ret == 0)
		p_rsp->indication = OAM_SET_OK;
	else
		p_rsp->indication = OAM_SET_NO_RESOURCE;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;

}
void eopl_set_onu_iad_operation(u8_t *p_in)
{
	u8_t *p_out;
	oam_iad_operation_t *p_iad_operation;
	oam_set_response_t *p_rsp;
	u32_t ret;

#if 0
	p_iad_operation = (oam_iad_operation_t *)p_in;
	CfmAgent_Init("127.0.0.1", 0);
	/* del by pangjr on 20091120 for need not here */
	#if 0
	CfmAgent_PostGroupBegin();
	#endif
	/* set onu iad operation  */
	ret = stub_iad_operation_set(p_iad_operation->iad_operation);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set iad operation!\n");
		goto send_rsp;
	}

send_rsp:
	/* del by pangjr on 20091120 for need not here */
	#if 0
	CfmAgent_PostGroupEnd(0);
	#endif
	/* del end */
	CfmAgent_Exit();
	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t *)p_out;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ACTION;
	p_rsp->leaf = OAM_ATTR_IAD_OPERATION;

	if(ret == 0)
		p_rsp->indication = OAM_SET_OK;
	else
		p_rsp->indication = OAM_SET_NO_RESOURCE;

	g_usOutDataIdx += sizeof(oam_set_response_t);
#endif
	return;
}

void eopl_set_sip_digit_map(u8_t *p_in)
{
	u8_t *p_out;
	oam_sip_digit_map_t *p_sip_digit_map;
	oam_set_response_t *p_rsp;
	u32_t ret;

	p_sip_digit_map = (oam_sip_digit_map_t *)p_in;

    OAM_TRACE();
	/* set sip digit map  */
#if 0
    //Eric Yang hide for source code merging.
	ret = stub_sip_digital_map_set(&p_sip_digit_map->sip_digit_map[0]);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to set sip digit map!\n");
		goto send_rsp;
	}
#endif

send_rsp:

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t *)p_out;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_SIP_DIGIT_MAP;

	if(ret == 0)
		p_rsp->indication = OAM_SET_OK;
	else
		p_rsp->indication = OAM_SET_NO_RESOURCE;

	g_usOutDataIdx += sizeof(oam_set_response_t);
	return;
}

void eopl_set_onu_llid_queue_config(u8_t *p_in)
{
	u8_t *p_out;
	oam_set_response_t *p_rsp;
	oam_llid_queue_cfg_t *p_llid_queue_cfg;
	oam_llid_queue_cfg_entry_t *p_llid_queue_cfg_entry;
	u8_t num_of_queue;
	u16_t queue_id;
	u16_t queue_weight;
	u8_t i;
	u32_t ret = OAM_SET_BAD_PARAMETERS;
	oam_instance_tlv_t *p_inst;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	/* config llid queue weight */
send_rsp:

	p_inst = (oam_instance_tlv_t *)p_out;
	*p_inst = port_instance;
	p_out += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	/* generate response */
	p_rsp = (oam_set_response_t *)p_out;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_LLID_QUEUE_CONFIG;

	if(ret == 0)
		p_rsp->indication = OAM_SET_OK;
	else
		p_rsp->indication = OAM_SET_BAD_PARAMETERS;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_mxu_mng_global_para(u8_t *p_in)
{
	u8_t *p_out;
	oam_mxu_mng_global_para_t *p_mng_global_para;
	oam_set_response_t *p_rsp;
	ODM_SYS_IF_CONFIG_t stIpConfig;

	p_mng_global_para  = (oam_mxu_mng_global_para_t *)p_in;
	vosMemSet(&stIpConfig, 0, sizeof(ODM_SYS_IF_CONFIG_t));

 /*	if(0 == p_mng_global_para->mng_ip_addr)
 	{
	    stIpConfig.ucIf = INTERFACE_WAN;
		stIpConfig.ucIfMode = INTERFACE_MODE_ADMIN;
	    stIpConfig.ucIfShareMode = INTERFACE_SHARE_MODE_SHARE;
	    stIpConfig.ucIpMode = IF_IP_MODE_DHCP;
	    stIpConfig.ulIpAddr = p_mng_global_para->mng_ip_addr;
	    stIpConfig.ulIpMask = p_mng_global_para->mng_ip_mask;
	    stIpConfig.ulGwAddr = p_mng_global_para->mng_gw;

	    odmSysCfgIfQoSModeGet(stIpConfig.ucIf, stIpConfig.ucIfMode, &stIpConfig.ucQoSMode);
		odmSysCfgIfQoSValGet(stIpConfig.ucIf, stIpConfig.ucIfMode, &stIpConfig.ucQoSVal);

	    if (p_mng_global_para->mng_data_cvlan > 0){
	        stIpConfig.ucTagged = 1;
	        stIpConfig.usCVlan = p_mng_global_para->mng_data_cvlan;
	        stIpConfig.ucPriority = p_mng_global_para->mng_data_priority;
	    }
		else{
		    odmSysCfgIfTagModeGet(stIpConfig.ucIf, stIpConfig.ucIfMode, &stIpConfig.ucTagged);
			odmSysCfgIfCVLANGet(stIpConfig.ucIf, stIpConfig.ucIfMode, &stIpConfig.usCVlan);
			odmSysCfgIfPriGet(stIpConfig.ucIf, stIpConfig.ucIfMode, &stIpConfig.ucPriority);
	    }
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Set interface wan to dhcp mode !\n");
 	}
	else */
	{
	    stIpConfig.ucIf = INTERFACE_WAN;
		stIpConfig.ucIfMode = INTERFACE_MODE_ADMIN;
	    stIpConfig.ucIfShareMode = INTERFACE_SHARE_MODE_SHARE;
	    stIpConfig.ucIpMode = IF_IP_MODE_STATIC;
	    stIpConfig.ulIpAddr = p_mng_global_para->mng_ip_addr;
	    stIpConfig.ulIpMask = p_mng_global_para->mng_ip_mask;
	    stIpConfig.ulGwAddr = p_mng_global_para->mng_gw;

	    odmSysCfgIfQoSModeGet(stIpConfig.ucIf, stIpConfig.ucIfMode, &stIpConfig.ucQoSMode);
		odmSysCfgIfQoSValGet(stIpConfig.ucIf, stIpConfig.ucIfMode, &stIpConfig.ucQoSVal);

	    if (p_mng_global_para->mng_data_cvlan > 0){
	        stIpConfig.ucTagged = 1;
	        stIpConfig.usCVlan = p_mng_global_para->mng_data_cvlan;
	        stIpConfig.ucPriority = p_mng_global_para->mng_data_priority;
	    }
		else{
		    odmSysCfgIfTagModeGet(stIpConfig.ucIf, stIpConfig.ucIfMode, &stIpConfig.ucTagged);
			odmSysCfgIfCVLANGet(stIpConfig.ucIf, stIpConfig.ucIfMode, &stIpConfig.usCVlan);
			odmSysCfgIfPriGet(stIpConfig.ucIf, stIpConfig.ucIfMode, &stIpConfig.ucPriority);
	    }
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Set interface wan to static ip!\n");

	}
	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t *)p_out;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_MXU_MNG_GLOBAL_PARA;
	if (NO_ERROR != odmSysIfConfigSet(&stIpConfig)){
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Failed to set interface wan ip !\n");
        p_rsp->indication = OAM_SET_NO_RESOURCE;
	}	
	else {
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Successfully to set interface wan ip !\n");		
		p_rsp->indication = OAM_SET_OK;
		stub_mng_ip_addr = p_mng_global_para->mng_ip_addr;
		stub_mng_ip_mask = p_mng_global_para->mng_ip_mask;
		stub_mng_gw = p_mng_global_para->mng_gw;
		stub_mng_data_cvlan = p_mng_global_para->mng_data_cvlan;
		stub_mng_data_svlan = p_mng_global_para->mng_data_svlan;
		stub_mng_data_priority = p_mng_global_para->mng_data_priority;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_mxu_mng_snmp_para(u8_t *p_in)
{
	u8_t *p_out;
	oam_mxu_mng_snmp_para_t *p_mng_snmp_para;
	oam_set_response_t *p_rsp;

	p_mng_snmp_para  = (oam_mxu_mng_snmp_para_t *)p_in;

	stub_snmp_ver = p_mng_snmp_para->snmp_ver;
	stub_trap_host_ip = p_mng_snmp_para->trap_host_ip;
	stub_trap_port = p_mng_snmp_para->trap_port;
	stub_snmp_port = p_mng_snmp_para->snmp_port;
	strncpy(stub_security_name, &p_mng_snmp_para->security_name[0], 32);
	strncpy(stub_community_for_read, &p_mng_snmp_para->community_for_read[0], 32);
	strncpy(stub_community_for_write, &p_mng_snmp_para->community_for_write[0], 32);

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t *)p_out;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_MXU_MNG_SNMP_PARA;
	p_rsp->indication = OAM_SET_OK;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_eth_fecmode(u32_t mode)
{
	u8_t	rlt=0, *p_byte;
	oam_set_response_t	*p_rsp;
	u32_t enable;
	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_STANDARD_ATTRIB;
	p_rsp->leaf = OAM_ATTR_FEC_MODE;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fec ctrl: %d\n", mode);
	if(1 == mode)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\r\nunknown fec mode\r\n");
		//mode = 0xf;
	}
	else if(2 == mode)
	{
		enable = 1;
	}
	else if(3 == mode)
	{
		enable = 0;
	}

	rlt = (u8_t)odmPonFecEnableSet(enable);
	if(rlt == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += 4;
}

void eopl_set_onu_mcast_group_max(u8_t max)
{
	u8_t	rlt=0;
	u16_t   portS, portE;
	u8_t    *p_byte;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t	*p_inst;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else

    #ifdef CTC_MULTICAST_SURPORT
    portMax = 4;
    #else
    portMax = 1;
    #endif

#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		/* invalid parameter */
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		p_rsp = (oam_set_response_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_ETH_PORTPAUSE;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_instance_tlv_t) + sizeof(oam_set_response_t);
		return;
	}


	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst =   port_instance;
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_MCAST_GRP_NUMMAX;
#if defined(CONFIG_PRODUCT_EPN200)
    rlt = OPL_NOT_SUPPORTED;
#else
	for(; portS<=portE; portS++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "mc vlan grp max: %d\n", max);
		rlt = odmMulticastGroupMaxNumSet(portS, max);
	}
#endif
	if(!rlt) /*for multicast, true means success */
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}


int eopl_private_oam_tx(u8_t *p_in, u16_t p_len)
{
	int ret;

	ret  = OamFrameSend(p_in, p_len);

	return ret;
}

void eopl_set_onu_alarm_admin_state(u16_t alarm_id, u32_t alarm_state)
{
	u32_t value;
	u8_t uni_type;
	u16_t uni_id;
	u32_t pon_id;
	u32_t card_id;
	u32_t llid;
	u8_t *p_byte;
	oam_instance_tlv_t *p_inst;
	oam_set_response_t *p_rsp;
	u8_t ret;
	u16_t i;
	u8_t port_start;
	u8_t port_end;
	u8_t port_id;

	value = port_instance.value;

#if 0
	switch(port_instance.leaf)
	{
	case OAM_CTC_OBJ_UNI:
		uni_id = OAM_GET_PORT_NUM(value);
		uni_type = OAM_GET_PORT_TYPE(value);

		if(uni_id == 0xFFFF)
		{
			port_start = 1;
			port_end = OAM_MAX_INSTANCE_ID;
		}
		else
		{
			port_start = uni_id;
			port_end = uni_id;
		}

		if((uni_type == OAM_CTC_PORT_TYPE_ETH) && (port_end <= OAM_MAX_INSTANCE_ID))
		{
			for(port_id = port_start; port_id <= port_end; port_id++)
			{
				for(i = 0; i < UNI_ALARM_NUM; i++)
				{
					if(alarm_id == stub_eth_port_alarm_config[port_id-1][i].alarm_id)
					{
						stub_eth_port_alarm_config[port_id-1][i].enable_status = alarm_state;
					}
				}
			}
			ret = OAM_SET_OK;
		}
		else
		{
			ret = OAM_SET_BAD_PARAMETERS;
		}

		break;
	case OAM_CTC_OBJ_CARD:
		card_id = value;
		for(i = 0; i < CARD_ALARM_NUM; i++)
		{
			if(alarm_id == stub_card_alarm_config[i].alarm_id)
			{
				stub_card_alarm_config[i].enable_status = alarm_state;
			}
		}

		ret = OAM_SET_OK;
		break;

	case OAM_CTC_OBJ_PON:
		pon_id = value;
		for(i = 0; i < PON_ALARM_NUM; i++)
		{
			if(alarm_id == stub_pon_alarm_config[i].alarm_id)
			{
				stub_pon_alarm_config[i].enable_status = alarm_state;
			}
		}
		ret = OAM_SET_OK;
		break;
	default:
		/* ONU object */
		for(i = 0; i < ONU_ALARM_NUM; i++)
		{
			if(alarm_id == stub_onu_alarm_config[i].alarm_id)
			{
				stub_onu_alarm_config[i].enable_status = alarm_state;
			}
		}
		ret = OAM_SET_OK;
		break;
	}
#else
	uni_id = OAM_GET_PORT_NUM(value);
	if(OAM_CTC_OBJ_UNI == port_instance.leaf && uni_id == 0xFFFF)
	{
		for(i=1; i<=ODM_NUM_OF_PORTS; i++)
		{
			//CTC_STACK_alarm_set_state(alarm_id, i, alarm_state);
		}
	}
	else
	{
		 //CTC_STACK_alarm_set_state(alarm_id, uni_id, alarm_state);
	}	
#endif
	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	if(port_instance.leaf != 0)
	{
		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);
	}

	p_rsp = (oam_set_response_t *)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_ALARM_ADMIN_STATE;
	p_rsp->indication = OAM_SET_OK;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_alarm_threshold(u16_t alarm_id, i32_t raising_threshold, i32_t falling_threshold)
{
	u32_t value;
	u8_t uni_type;
	u16_t uni_id;
	u32_t pon_id;
	u32_t card_id;
	u32_t llid;
	u8_t *p_byte;
	oam_instance_tlv_t *p_inst;
	oam_set_response_t *p_rsp;
	u8_t ret;
	u16_t i;
	u8_t port_start;
	u8_t port_end;
	u8_t port_id;

	value = port_instance.value;

#if 0
	switch(port_instance.leaf)
	{
	case OAM_CTC_OBJ_UNI:
		uni_id = OAM_GET_PORT_NUM(value);
		uni_type = OAM_GET_PORT_TYPE(value);

		if(uni_id = 0xFFFF)
		{
			port_start = 1;
			port_end = OAM_MAX_INSTANCE_ID;
		}
		else
		{
			port_start = uni_id;
			port_end = uni_id;
		}

		if((uni_type == OAM_CTC_PORT_TYPE_ETH) && (port_end <= OAM_MAX_INSTANCE_ID))
		{
			for(port_id = port_start; port_id <= port_end; port_id++)
			{
				for(i = 0; i < UNI_ALARM_NUM; i++)
				{
					if(alarm_id == stub_eth_port_alarm_config[port_id-1][i].alarm_id)
					{
						stub_eth_port_alarm_config[port_id-1][i].raising_threshold = raising_threshold;
						stub_eth_port_alarm_config[port_id-1][i].falling_threshold = falling_threshold;
					}
				}
			}
			ret = OAM_SET_OK;
		}
		else
		{
			ret = OAM_SET_BAD_PARAMETERS;
		}

		break;
	case OAM_CTC_OBJ_CARD:
		card_id = value;
		for(i = 0; i < CARD_ALARM_NUM; i++)
		{
			if(alarm_id == stub_card_alarm_config[i].alarm_id)
			{
				stub_card_alarm_config[i].raising_threshold = raising_threshold;
				stub_card_alarm_config[i].falling_threshold = falling_threshold;
			}
		}

		ret = OAM_SET_OK;
		break;

	case OAM_CTC_OBJ_PON:
		pon_id = value;
		for(i = 0; i < PON_ALARM_NUM; i++)
		{
			if(alarm_id == stub_pon_alarm_config[i].alarm_id)
			{
				stub_pon_alarm_config[i].raising_threshold = raising_threshold;
				stub_pon_alarm_config[i].falling_threshold = falling_threshold;
			}
		}
		ret = OAM_SET_OK;
		break;
	default:

		for(i = 0; i < ONU_ALARM_NUM; i++)
		{
			if(alarm_id == stub_onu_alarm_config[i].alarm_id)
			{
				stub_onu_alarm_config[i].raising_threshold = raising_threshold;
				stub_onu_alarm_config[i].falling_threshold = falling_threshold;
			}
		}

		ret = OAM_SET_OK;
		break;
	}
#else
	uni_id = OAM_GET_PORT_NUM(value);
	if(OAM_CTC_OBJ_UNI == port_instance.leaf && uni_id == 0xFFFF)
	{
		for(i=1; i<=ODM_NUM_OF_PORTS; i++)
		{
			//CTC_STACK_alarm_set_thr(alarm_id, i, raising_threshold, falling_threshold);
		}
	}
	else
	{
		//CTC_STACK_alarm_set_thr(alarm_id, uni_id, raising_threshold, falling_threshold);
	}	
#endif
	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	if(port_instance.leaf != 0)
	{
		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);
	}

	p_rsp = (oam_set_response_t *)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_ALARM_THRESHOLD;
	p_rsp->indication = OAM_SET_OK;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}


void eopl_set_onu_mcast_fast_leave_control(u8_t value)
{
	u8_t	rlt=0, *p_byte;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t	*p_inst;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ACTION;
	p_rsp->leaf = OAM_ACTION_FAST_LEAVE_ADMIN;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "mc fast leave set: %d\n", value);
#if defined(CONFIG_PRODUCT_EPN200)
    rlt = OPL_NOT_SUPPORTED;
#else
	if (1 == value)
	{
		rlt = odmMulticastFastleaveModeSet(0);
	}
	else if (2 == value)
	{
		rlt = odmMulticastFastleaveModeSet(1);
	}
#endif
	if(!rlt) /*for multicast, true means success */
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_mcast_control(u8_t *p_in)
{
	u8_t	i, *p_byte;
	oam_set_response_t			*p_rsp;
	oam_mcast_control_t			*p_mcast_control;
	oam_mcast_control_entry_t	*p_mcast_entry;
    /*  */
    int retVal = NO_ERROR;
    /* 014530 */

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_MCAST_CONTROL;

	p_mcast_control = (oam_mcast_control_t*)p_in;

	if(p_mcast_control->width == 1)
	{
		odmMulticastControlEntryClear();
	}
	else
	{
		if(p_mcast_control->action == OAM_CTC_MCAST_ENTRY_ADD)
		{
    
          #if 0
          multicast_control_entry_t ctl_entry_list[64*ODM_NUM_OF_PORTS];
          #else
          multicast_control_entry_t ctl_entry_list[MC_MAX_GROUP_NUM];
          #endif
         
		  
		  UINT16 ctl_entry_num = 0;
		  UINT16 ctl_entry_index;
		  oam_mcast_control_entry_t	*p_mcast_entry_check;
		  UINT8 ctl_entry_num_per_port[ODM_NUM_OF_PORTS+1] = {0};
		  
		  odmMulticastControlEntryGet(&ctl_entry_list, &ctl_entry_num);

		  for(ctl_entry_index=0; ctl_entry_index<ctl_entry_num; ctl_entry_index++)
		  {
    		    ctl_entry_num_per_port[ctl_entry_list[ctl_entry_index].port]++;
		  }

		  p_mcast_entry_check = (oam_mcast_control_entry_t*)(p_in + sizeof(oam_mcast_control_t));
		  for(i=0; i<p_mcast_control->number; i++)
		  {
    		    ctl_entry_num_per_port[p_mcast_entry_check->port_id]++;
    		    p_mcast_entry_check++;
		  }

		  for(i=ODM_START_PORT_NUN; i<=ODM_NUM_OF_PORTS; i++)
		  {
            #ifdef CTC_MULTICAST_SURPORT
    		    if(ctl_entry_num_per_port[i]>ODM_MAX_GRUP_NUM_PER_PORT)
            #else
                if(ctl_entry_num_per_port[i]>64)
            #endif
    		    {
        		    retVal = OPL_ERROR;
        		    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control: port%d entry>64\n", i);
        		    goto send_rsp;
    		    }
		  }
		}

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
		if ((p_mcast_control->action == OAM_CTC_MCAST_ENTRY_ADD) 
		  && (odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID
		      || odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID))
		{
		  extern UINT8 dal_vtt_num;
		  oam_mcast_control_entry_t	*p_mcast_entry_check;
		  oam_mcast_control_entry_t vtt_resource[256];
		  UINT8 addNumber = 0;
		  UINT8 numOfRecords = 0;
		  UINT8 vttUcNum = 0;
		  UINT8 j;
          
		  p_mcast_entry_check = (oam_mcast_control_entry_t*)(p_in + sizeof(oam_mcast_control_t));

		  if (odmMulticastControlTypeGet() == p_mcast_control->control_type)
		  {        		                
        		  for(i=0; i < p_mcast_control->number; i++)
        		  {
                        if(mcVlanMapExist(p_mcast_entry_check->port_id, p_mcast_entry_check->vlan_id) == OPL_FALSE)
                        {
                            for (j = 0; j <=addNumber; j++)
                            {
                                if (p_mcast_entry_check->port_id == vtt_resource[j].port_id && p_mcast_entry_check->vlan_id == vtt_resource[j].vlan_id)
                                    break;
                            }
                            
                            if (j > addNumber)
                            {
                                vtt_resource[addNumber].port_id = p_mcast_entry_check->port_id;
                                vtt_resource[addNumber].vlan_id = p_mcast_entry_check->vlan_id;
                                addNumber++;
                            }
                        }
                        
                        p_mcast_entry_check++;
        		  }
                  
        		  if(dal_vtt_num+addNumber>16) {
        		    retVal = OPL_ERROR;
        		    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control: %d+%d>SHIVA_MAX_VLAN_TRANS\n", dal_vtt_num, addNumber);
        		    goto send_rsp;
        		  }
		  }
		  else
		  {     
        		  for(i=0; i < ODM_NUM_OF_PORTS; i++) {
                        dalVttNumOfRecordGet(&numOfRecords, i);
                        vttUcNum += numOfRecords;
        		  }
                       
        		  for(i=0; i < p_mcast_control->number; i++)
        		  {
                        for (j = 0; j <=addNumber; j++)
                        {
                            if (p_mcast_entry_check->port_id == vtt_resource[j].port_id && p_mcast_entry_check->vlan_id == vtt_resource[j].vlan_id)
                                break;
                        }
                        
                        if (j > addNumber)
                        {
                            vtt_resource[addNumber].port_id = p_mcast_entry_check->port_id;
                            vtt_resource[addNumber].vlan_id = p_mcast_entry_check->vlan_id;
                            addNumber++;
                        }
                        
                        p_mcast_entry_check++;
        		  }
                  
        		  if(vttUcNum+addNumber>16) {
        		    retVal = OPL_ERROR;
        		    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control: %d+%d>SHIVA_MAX_VLAN_TRANS\n", vttUcNum, addNumber);
        		    goto send_rsp;
        		  }
		  }
		}
#elif defined(ONU_1PORT)
#if defined(CONFIG_PRODUCT_EPN200)
        retVal = OPL_NOT_SUPPORTED;
        goto send_rsp;
#endif

#ifndef CTC_MULTICAST_SURPORT
		if (p_mcast_control->action == OAM_CTC_MCAST_ENTRY_ADD)
		{
    		  UINT8 numOfRule = 0;
    		  UINT8 numOfMvlan = 0;
    		  UINT16 vlanIdArray[4096];
    		  oam_mcast_control_entry_t	*p_mcast_entry_check;
    		  oam_mcast_control_entry_t vtt_resource[256];
    		  UINT8 addNumber = 0;
    		  UINT8 numOfRecords = 0;
    		  UINT8 j;
                
                if (odmMulticastControlTypeGet() == p_mcast_control->control_type)
    		    dalMulticastNumofRuleGet(&numOfRule);
                
    		  if(numOfRule+p_mcast_control->number>MC_MAX_GROUP_NUM) {
    		    retVal = OPL_ERROR;
    		    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control: %d+%d>%d\n", numOfRule, p_mcast_control->number, MC_MAX_GROUP_NUM);
    		    goto send_rsp;
    		  }
              
                if(p_mcast_control->control_type==MC_CTL_GDA_MAC)
                {   
                    odmMulticastVlanGet(1, vlanIdArray, &numOfMvlan);
                    if(numOfRule+p_mcast_control->number*numOfMvlan>MC_MAX_GROUP_NUM) {
        		  retVal = OPL_ERROR;
        		  OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control: %d+%d*%d>%d\n", numOfRule, p_mcast_control->number, numOfMvlan, MC_MAX_GROUP_NUM);
        		  goto send_rsp;
                    }
                }
                else
                {
                    p_mcast_entry_check = (oam_mcast_control_entry_t*)(p_in + sizeof(oam_mcast_control_t));

                    if (odmMulticastControlTypeGet() == p_mcast_control->control_type)
                    {        		                
                        for(i=0; i < p_mcast_control->number; i++)
                        {
                            if(mcVlanMapExist(p_mcast_entry_check->port_id, p_mcast_entry_check->vlan_id) == OPL_FALSE)
                            {
                                for (j = 0; j <=addNumber; j++)
                                {
                                    if (p_mcast_entry_check->port_id == vtt_resource[j].port_id && p_mcast_entry_check->vlan_id == vtt_resource[j].vlan_id)
                                        break;
                                }
                                
                                if (j > addNumber)
                                {
                                    vtt_resource[addNumber].port_id = p_mcast_entry_check->port_id;
                                    vtt_resource[addNumber].vlan_id = p_mcast_entry_check->vlan_id;
                                    addNumber++;
                                }
                            }
                            
                            p_mcast_entry_check++;
                        }
                        
                        dalVttMulticastNumOfRecordGet(&numOfRecords);
                        if(numOfRecords+addNumber>DAL_PORT_MCAST_ENTRY_NUM) {
                		    retVal = OPL_ERROR;
                		    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control: %d+%d>%d\n", numOfRecords, addNumber, DAL_PORT_MCAST_ENTRY_NUM);
                		    goto send_rsp;
                        }
                    }
                    else
                    { 
                        for(i=0; i < p_mcast_control->number; i++)
                        {
                            for (j = 0; j <=addNumber; j++)
                            {
                                if (p_mcast_entry_check->port_id == vtt_resource[j].port_id && p_mcast_entry_check->vlan_id == vtt_resource[j].vlan_id)
                                    break;
                            }
                            
                            if (j > addNumber)
                            {
                                vtt_resource[addNumber].port_id = p_mcast_entry_check->port_id;
                                vtt_resource[addNumber].vlan_id = p_mcast_entry_check->vlan_id;
                                addNumber++;
                            }
                            
                            p_mcast_entry_check++;
                        }
                  
        		  if(addNumber>DAL_PORT_MCAST_ENTRY_NUM) {
        		    retVal = OPL_ERROR;
        		    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control: %d>%d\n", addNumber, DAL_PORT_MCAST_ENTRY_NUM);
        		    goto send_rsp;
        		  }
                    }
                }
		}
#endif

#endif
		odmMulticastControlTypeSet(p_mcast_control->control_type);
		p_in += sizeof(oam_mcast_control_t);
		p_mcast_entry = (oam_mcast_control_entry_t*)p_in;

		for(i=0; i < p_mcast_control->number; i++)
		{
			if(p_mcast_control->action == OAM_CTC_MCAST_ENTRY_DEL)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "mc vlan ctrl del, port: %d, vlan: %d, gda: %02x:%02x:%02x:%02x:%02x:%02x\n",
					p_mcast_entry->port_id, p_mcast_entry->vlan_id, p_mcast_entry->gda[0],
					p_mcast_entry->gda[1], p_mcast_entry->gda[2], p_mcast_entry->gda[3],
					p_mcast_entry->gda[4], p_mcast_entry->gda[5]);
				retVal = odmMulticastControlEntryDelete(p_mcast_entry->port_id,
	 	         				       p_mcast_entry->vlan_id,
	 	         				       p_mcast_entry->gda);
			}
			else if(p_mcast_control->action == OAM_CTC_MCAST_ENTRY_ADD)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "mc vlan ctrl add, port: %d, vlan: %d, gda: %02x:%02x:%02x:%02x:%02x:%02x\n",
					p_mcast_entry->port_id, p_mcast_entry->vlan_id, p_mcast_entry->gda[0],
					p_mcast_entry->gda[1], p_mcast_entry->gda[2], p_mcast_entry->gda[3],
					p_mcast_entry->gda[4], p_mcast_entry->gda[5]);
				retVal = odmMulticastControlEntryAdd(p_mcast_entry->port_id,
	 	         				       p_mcast_entry->vlan_id,
	 	         				       p_mcast_entry->gda);
			}

			p_mcast_entry++;
		}
	}

send_rsp:
	if(retVal != NO_ERROR)
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}
	else
	{
		p_rsp->indication = OAM_SET_OK;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_mcast_switch(u8_t action)
{
	u8_t	rlt=0, *p_byte;
	oam_set_response_t	*p_rsp;

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_MCAST_SWITCH;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "mc vlan switch: %d\n", action);
#if defined(CONFIG_PRODUCT_EPN200)
    /*ctc mc*/
    if (0 != action)
    {
        rlt = OPL_NOT_SUPPORTED;
    }
    /*igsp*/
    else
    {
        rlt = OPL_OK;
    }
#else
	rlt = odmMulticastSwitchSet(action);
#endif
	if(!rlt) /*for multicast, true means success */
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_set_onu_mcast_tag_strip(u8_t *p_in)
{
	u8_t	rlt=0;
	u16_t   portS, portE;
	u8_t    *p_byte;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t	*p_inst;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	u8_t  num_of_mcast_vlan;
	oam_mcast_tagstripe_t *p_mcast_tag_strip_hdr;
	oam_mcast_vlan_translation_entry_t *p_translation_entry;
	u8_t  i;
	u16_t portId;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else

    #ifdef CTC_MULTICAST_SURPORT
    portMax = 4;
    #else
    portMax = 1;
    #endif

#endif

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	if((portType != OAM_CTC_PORT_TYPE_ETH) || (portE > portMax))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_set_response_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_MCAST_TAG_STRIP;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_set_response_t);
		return;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst =   port_instance;
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_MCAST_TAG_STRIP;

	p_mcast_tag_strip_hdr = (oam_mcast_tagstripe_t *)p_in;
	p_in += sizeof(oam_mcast_tagstripe_t);

      ////20100327_XFAN_ADD_BEGIN
#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	if(p_mcast_tag_strip_hdr->tagstriped == 0x02)
	{
        extern UINT8 dal_vtt_num;
        UINT32 vlanAddNum;
        vlanAddNum = 0;
        UINT8 tagOper;
        UINT32 mvlan;
        UINT32 reusableVttNum = 0;

        num_of_mcast_vlan = *((u8_t *)p_in);

        if(portS != portE) {
          vlanAddNum = ODM_NUM_OF_PORTS*num_of_mcast_vlan;
        }
        else {
          vlanAddNum = 1*num_of_mcast_vlan;
        }

        for(portId = portS; portId <= portE; portId++) {
            odmMulticastTagstripGet(portId, &tagOper);
            if (tagOper != MC_TAG_OPER_MODE_TRANSLATION) {
              for(mvlan=1; mvlan<4096; mvlan++) {
                  if(mcVlanMapExist(portId, mvlan))
                      reusableVttNum++;
              }
            }
        }
        
        if(dal_vtt_num-reusableVttNum+vlanAddNum>16) {
          rlt = OPL_ERROR;
          OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_tag_strip: %d+%d>SHIVA_MAX_VLAN_TRANS\n", dal_vtt_num, vlanAddNum);
          goto send_rsp;
        }
	}
#elif defined(ONU_1PORT)

#if defined(CONFIG_PRODUCT_EPN200)
    rlt = OPL_NOT_SUPPORTED;
    goto send_rsp;
#endif
  
    #ifndef CTC_MULTICAST_SURPORT
	if(p_mcast_tag_strip_hdr->tagstriped == 0x02)
	{
        UINT32 vlanAddNum = 0;
        UINT8 numOfRecords = 0;
        UINT8 tagOper;

        num_of_mcast_vlan = *((u8_t *)p_in);

        if(portS != portE) {
          vlanAddNum = ODM_NUM_OF_PORTS*num_of_mcast_vlan;
        }
        else {
          vlanAddNum = 1*num_of_mcast_vlan;
        }

      odmMulticastTagstripGet(portS, &tagOper);
      if (tagOper == MC_TAG_OPER_MODE_TRANSLATION)
      {
        dalVttMulticastNumOfRecordGet(&numOfRecords);
      }
      
      if(numOfRecords+vlanAddNum>DAL_PORT_MCAST_ENTRY_NUM) {
          rlt = OPL_ERROR;
          OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_tag_strip: %d+%d>%d\n", numOfRecords, vlanAddNum, DAL_PORT_MCAST_ENTRY_NUM);
          goto send_rsp;
        }
	}
    #else
    if(p_mcast_tag_strip_hdr->tagstriped == 0x02)
    {
      rlt = OPL_ERROR;
      mc_printf("\nfunc %s does not support mcvlan translation.\n",__FUNCTION__);
      goto send_rsp;
    }
    #endif
   
#endif
      ////20100327_XFAN_ADD_END

	for(portId = portS; portId <= portE; portId++)
	{
		OP_DEBUG(DEBUG_LEVEL_INFO, "call odmMulticastTagstripSet(%d, %d)\n", portId, p_mcast_tag_strip_hdr->tagstriped);
		rlt = odmMulticastTagstripSet(portId,p_mcast_tag_strip_hdr->tagstriped);

		if(rlt != OPL_OK)
		{
			OP_DEBUG(DEBUG_LEVEL_INFO, "call odmMulticastTagstripSet() fail!\n");
			goto send_rsp;
		}
	}

	/* if tagstriped == 0x02, add mcast vlan translation entry */
	if(p_mcast_tag_strip_hdr->tagstriped == 0x02)
	{
        #ifndef CTC_MULTICAST_SURPORT
		num_of_mcast_vlan = *((u8_t *)p_in);
		p_in += sizeof(u8_t);

		for(i = 0; i < num_of_mcast_vlan; i++)
		{
			for(portId = portS; portId <= portE; portId++)
			{

				p_translation_entry = (oam_mcast_vlan_translation_entry_t *)p_in;
				p_in += sizeof(oam_mcast_vlan_translation_entry_t);

				OP_DEBUG(DEBUG_LEVEL_INFO, "call odmMulticastTagTranslationAdd(%ld, %ld, %ld)\n",
												portId,
												p_translation_entry->mcast_vlan,
												p_translation_entry->iptv_vlan);

				rlt = odmMulticastTagTranslationAdd(portId,
					                                p_translation_entry->mcast_vlan,
					                                p_translation_entry->iptv_vlan);

				if(rlt != OPL_OK)
				{
					OP_DEBUG(DEBUG_LEVEL_INFO, "call odmMulticastTagTranslationAdd() fail!\n");
					goto send_rsp;
				}
			}
		}
        #else
        rlt = OPL_ERROR;
        // mc_printf("\nfunc %s does not support mcvlan translation.\n",__FUNCTION__);
        goto send_rsp;
        #endif
      
	}

send_rsp:
	
	if(!rlt)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);
	
	return;
}

void eopl_set_onu_mcast_vlan(u8_t *p_in)
{
	u8_t	i, number;
	u16_t   portS, portE, curPortId;
	u8_t    *p_byte;
	u16_t	*p_vid;
	oam_set_response_t	*p_rsp;
	oam_mcast_vlan_t	*p_mcast_vlan;
	oam_instance_tlv_t	*p_inst;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	u32_t   rlt = 0;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else

    #ifndef CTC_MULTICAST_SURPORT
	portMax = 1;
    #else
    portMax = 4;
    #endif

#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_set_response_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_MCAST_VLAN;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_instance_tlv_t) + sizeof(oam_set_response_t);
		return;
	}


	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst =  port_instance;
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_MCAST_VLAN;

	p_mcast_vlan = (oam_mcast_vlan_t*)p_in;

	if(p_mcast_vlan->width == 1)
	{
		for(; portS<=portE; portS++)
		{
			odmMulticastVlanClear(portS);
		}
	}
	else
	{
        /*  2012-9-3 */
        #ifdef CTC_MULTICAST_SURPORT
        extern u32_t mcVlanNumGet(void);
        extern OPL_STATUS mcVlanExist(u32_t vlanId);
        u16_t *p_vid_tmp = NULL;
        u32_t ui_mc_vlan_num = 0;
        #endif
        
		p_in += sizeof(oam_mcast_vlan_t);
		p_vid = (u16_t*)p_in;
		number = p_mcast_vlan->width-1;
        
        #ifndef CTC_MULTICAST_SURPORT
		if(number%2 !=0)
		{
			return;
		}
        #else
        /*every uni supports max 4 mc vlans.*/
        if ((number%2 !=0) || (15 < number/2))
        {
            rlt = OPL_ERROR;
	    	goto send_rsp;
        }

        p_vid_tmp = p_vid;
        for (i = 0; i < (number/2); i++)
        {
            if (OPL_FALSE == mcVlanExist(*(p_vid_tmp + i)))
            {
                ui_mc_vlan_num++;
            }

            /* 2012-9-6 */
            /*Mc vlan can not be same with ctc vlan.*/
            {
                extern BOOL CTC_CheckValidVidForMcvlan(u32_t uiMcVid);
                if (FALSE == CTC_CheckValidVidForMcvlan(*(p_vid_tmp + i)))
                {
                    rlt = OPL_ERROR;
	    	        goto send_rsp;
                }
            }
            /* 014040 */
        }

        ui_mc_vlan_num = ui_mc_vlan_num + mcVlanNumGet();
        if (15 < ui_mc_vlan_num)
        {
            rlt = OPL_ERROR;
	    	goto send_rsp;
        }
        #endif
        /* 013987 */

    ////20100327_XFAN_ADD_BEGIN
#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    UINT32 vlanAddNum;
    vlanAddNum = 0;
    extern UINT8	dal_vtt_num;
    
    if(p_mcast_vlan->operation == OAM_CTC_MCAST_VLAN_ADD) {
      if(portS != portE) {
        vlanAddNum = ODM_NUM_OF_PORTS*(number/2);
      }
      else {
        vlanAddNum = 1*(number/2);
      }
      if(dal_vtt_num+vlanAddNum>16) {
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_vlan: %d+%d>SHIVA_MAX_VLAN_TRANS\n", dal_vtt_num, vlanAddNum);
        rlt = OPL_ERROR;
	    	goto send_rsp;
      }
    }
#elif defined(ONU_1PORT)

#if defined(CONFIG_PRODUCT_EPN200)
    rlt = OPL_NOT_SUPPORTED;
    goto send_rsp;
#endif

#ifndef CTC_MULTICAST_SURPORT
    UINT32 vlanAddNum = 0;
/* 014547 */
    UINT16 numOfRecords = 0;
    multicast_control_entry_t ctl_entry_list[MC_MAX_GROUP_NUM];
    UINT16 numOfRule = 0;    
/*  */
    UINT16 numOfEntries = 0;
  
    if(p_mcast_vlan->operation == OAM_CTC_MCAST_VLAN_ADD) {
      if(portS != portE) {
        vlanAddNum = ODM_NUM_OF_PORTS*(number/2);
      }
      else {
        vlanAddNum = 1*(number/2);
      }

      dalVttMulticastNumOfRecordGet(&numOfRecords);
      if(numOfRecords+vlanAddNum>DAL_PORT_MCAST_ENTRY_NUM) {
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_vlan: %d+%d>%d\n", numOfRecords, vlanAddNum, DAL_PORT_MCAST_ENTRY_NUM);
        rlt = OPL_ERROR;
	    	goto send_rsp;
      }

      if(mcControlBlockModeGet()==MC_MODE_SNOOPING ||
        (mcControlBlockModeGet()==MC_MODE_CTC && odmMulticastControlTypeGet()==MC_CTL_GDA_MAC))
      {
          dalMulticastNumofRuleGet(&numOfRule);
              
          odmMulticastControlEntryGet(ctl_entry_list, &numOfEntries);
          
          if(numOfRule+vlanAddNum*numOfEntries>MC_MAX_GROUP_NUM) {
            rlt = OPL_ERROR;
            OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control: %d+%d*%d>%d\n", numOfRule, vlanAddNum, numOfEntries, MC_MAX_GROUP_NUM);
            goto send_rsp;
          }
      }
    }
#endif

#endif
    ////20100327_XFAN_ADD_END
		for(i=0;i<number/2;i++,p_vid++)
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "mc vlan: %d\n", *p_vid);
	 		if(p_mcast_vlan->operation == OAM_CTC_MCAST_VLAN_DEL)
	 		{
				for(curPortId=portS; curPortId<=portE; curPortId++)
				{
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"call odmMulticastVlanDelete(%d, %d)", curPortId, *p_vid);
					odmMulticastVlanDelete(curPortId, *p_vid);
				}
	 		}

			if(p_mcast_vlan->operation == OAM_CTC_MCAST_VLAN_ADD)
			{
				for(curPortId=portS; curPortId<=portE; curPortId++)
				{
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"call odmMulticastVlanAdd(%d, %d)", curPortId, *p_vid);
					odmMulticastVlanAdd(curPortId, *p_vid);
				}
			}
		}
	}

send_rsp:
	if(rlt == OPL_OK)
	{
	p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}
	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

int eopl_set_tag_id(u16_t portid,u32_t *value)
{
	u8_t	ret=OPL_OK;
	u16_t	*p_16;

	p_16 = (u16_t*)value;

	if (0x0000 != *p_16)
	{
		ret = dalVttTpidSet(*p_16, portid);
	}
	
	if(ret != OPL_OK)
	{
		return ret;
	}

	p_16++;
	ret = odmPortDefaultVlanSet(portid, (*p_16)&0xFFF);
	if(ret != OPL_OK)
	{
		return ret;
	}
	ret = odmPortDefaultEtherPriSet(portid, (*p_16)>>13);

	return ret;
}


void eopl_set_port_loop_detect(u8_t *p_in)
{
	u8_t *p_out;
	oam_port_loop_detect_t *p_port_loop_detect;
	oam_instance_tlv_t *p_inst;
	oam_set_response_t *p_rsp;
	u32_t state;
	u32_t ret;
	u32_t value;
	u8_t port_type;
	u16_t port_num;
	u16_t port_max;
	u16_t port_start;
	u16_t port_end;
	u16_t i;

	value = (u32_t)port_instance.value;
	port_type = OAM_GET_PORT_TYPE(value);
	port_num = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	port_max = 4;
#else
	port_max = 1;
#endif

	if((port_type != OAM_CTC_PORT_TYPE_ETH) || ((port_num > port_max)&&(port_num != 0xFFFF)))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_out;
		*p_inst = port_instance;
		p_out += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_set_response_t*)p_out;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_PORT_LOOP_DETECT;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_instance_tlv_t) + sizeof(oam_set_response_t);
		return;
	}

	if(0xFFFF == port_num)
	{
		port_start = 1;
		port_end = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		port_start = port_num;
		port_end = port_num;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_out;
	*p_inst =   port_instance;
	p_out += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);
	p_rsp = (oam_set_response_t *)p_out;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_PORT_LOOP_DETECT;

	p_port_loop_detect = (oam_port_loop_detect_t *)p_in;
	state = p_port_loop_detect->state;

#if 1  /* modified by Gan Zhiheng - 2009/12/18 */
	for(i = port_start; i <= port_end; i++)
	{
		if ((1 == state ) || ( 2 == state))
		{
			ret = odmLoopDetectionSet(i, state - 1);
		}
	}
#else
    ret = odmLoopDetectionSet(state - 1);
#endif /* #if 0 */
	if(ret == 0)
		p_rsp->indication = OAM_SET_OK;
	else
		p_rsp->indication = OAM_SET_BAD_PARAMETERS;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

int eopl_IsExistList(u32_t * pList, u32_t vlan, u32_t num)
{
	u32_t index;

	for(index = 0; index < num; index++)
	{
		if (pList[index] == vlan)
		{
			return TRUE;
		}
	}

	return FALSE;
}

int eopl_set_onu_vlan(u8_t*  p_in)
{
	u32_t ret=0;
	u8_t i, j;
	u16_t portS, portE;
	u8_t  *p_byte;
	u32_t oldTag;
	u32_t newTag;
	u32_t baktag,oldMode;
	u32_t defaultTag;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t	*p_inst;
	oam_vlan_config_t	*p_vlan;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	ctc_aggr_vlan_tbl_t *p_aggr_vlan_tbl;
	ctc_trunk_vlan_t *p_trunk_vlan;
	u32_t default_vlan;
	u16_t num_of_aggr_table;
	u16_t num_of_src_vlan;
	u32_t src_vlan;
	u32_t dst_vlan;
	u32_t allowed_vlan;
	u32_t vlanTrunkNum;
	u32_t vlanId;
	u32_t vlanlist[32];
	u32_t vlanIndex;
	u8_t * p_vlan_check;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portMax = 4;
#else

    #if defined(CONFIG_PRODUCT_EPN104)
    portMax = 4;
    #else
    portMax = 1;
    #endif

#endif

	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_set_response_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_VLAN;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_instance_tlv_t) + sizeof(oam_set_response_t);
		return 0;
	}

	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
    
	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst =   port_instance;
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_VLAN;

	p_vlan = (oam_vlan_config_t*)p_in;
	p_in += sizeof(oam_vlan_config_t);

	stub_vlan_mode = p_vlan->mode;

	if(OAM_CTC_VLAN_TRANSPARENT == p_vlan->mode)
	{
		
        #if defined(CONFIG_PRODUCT_EPN200)
  		extern INT32 DRV_ResetPortQinq(UINT32 uiLPortId);
        extern INT32 DRV_ResetQinqPvid(UINT32 uiLPortId);
        i32_t iRet = NO_ERROR;
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"[ %s ]: OAM_CTC_VLAN_TRANSPARENT\n", __FUNCTION__);
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "vlan mode transparent.\n");
		for(; portS<=portE; portS++)
		{
			ret = odmPortVlanModeSet(portS, OAM_CTC_VLAN_TRANSPARENT);
			if(ret != OPL_OK)
				goto response;
            
            iRet = DRV_ResetPortQinq(portS);
            iRet += DRV_ResetQinqPvid(portS);
            if (NO_ERROR != iRet)
            {
                printf("\nFailed to reset qinq vlan for port %d.\n",portNum);
                ret = OPL_ERROR;
                goto response;
            }
		}
        #else
        i32_t iRet = NO_ERROR;

        if (0xFFFF == portNum)
        {
            /*printf("\nDo not set transparent vlan for all ports.\n");*/
            ret = OPL_OK;
            goto response;
        }
        
        iRet = CTC_DRV_SetTransparentVlan(portNum);
        if (NO_ERROR != iRet)
        {
            printf("\nFailed to set transparent vlan for port %d.\n",portNum);
            ret = OPL_ERROR;
            goto response;
        }
        #endif
	
	}
	else if(OAM_CTC_VLAN_TAG == p_vlan->mode)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: OAM_CTC_VLAN_TAG\n", __FUNCTION__);
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "vlan mode tag.\n");

		vosMemCpy(&defaultTag, p_in, 4);

#if defined(CONFIG_PRODUCT_EPN200)
        
		/*Not support*/
        ret = OPL_NOT_SUPPORTED;
        goto response;
		
		////20100327_XFAN_ADD_BEGIN
#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
              UINT32 vlanAddNum;
              vlanAddNum = 0;
              extern UINT8	dal_vtt_num;
	       extern UINT8 dalToAtherosPortMap[11];
		extern VLAN_CONFI_INFO_t dal_vtt_info[ODM_NUM_OF_PORTS +1];
	       UINT32 ar8228Port;
			        
              if(portS != portE) {
                vlanAddNum = ODM_NUM_OF_PORTS;
              }
              else {
                vlanAddNum = 1;
              }
	       for(j=portS; j <= portE; j++)
	       {
	           ar8228Port = dalToAtherosPortMap[j];
		    if((dal_vtt_info[ar8228Port].vlanMode == VLAN_TAG) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION))
			vlanAddNum = vlanAddNum - 1;
	       }
			        
              if(dal_vtt_num+vlanAddNum>16) {
              	ret += 1;
                OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_vlan OAM_CTC_VLAN_TAG: %d+%d>SHIVA_MAX_VLAN_TRANS\n", dal_vtt_num, vlanAddNum);
                goto response;
              }

#endif
    ////20100327_XFAN_ADD_END		

		//set_vlan_to_eth2(defaultTag,OAM_CTC_VLAN_TAG);
		for(j=portS; j <= portE; j++)
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: eopl_set_tag_id(%d, %lx)\n", __FUNCTION__, j, defaultTag);
			ret = eopl_set_tag_id(j, &defaultTag);
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "tag: port %d, defaultTag 0x%08x\n", j, defaultTag);
			if (OPL_OK != ret)
			{
				goto response;
			}			

			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: odmPortVlanModeSet(%d, OAM_CTC_VLAN_TAG)\n", __FUNCTION__, j);
			ret = odmPortVlanModeSet(j, OAM_CTC_VLAN_TAG);
			if (OPL_OK != ret)
			{
				goto response;
			}
		}
#else
        i32_t iRet = NO_ERROR;
        u32_t priority;

        ret = OPL_OK;

        iRet = CTC_DRV_GetVlanIdAndPri(defaultTag, &default_vlan, &priority);
        if (NO_ERROR != iRet)
        {
            printf("\nFailed to get tag vlan for port %d.\n",portNum);
            ret = OPL_ERROR;
		    goto response;
        }

        iRet = CTC_DRV_SetTagVlan(portNum, default_vlan, priority);
		if (NO_ERROR != iRet)
        {
            printf("\nFailed to set tag vlan for port %d.\n",portNum);
            ret = OPL_ERROR;
		    goto response;
        }
#endif

	}
	else if(OAM_CTC_VLAN_TRANSLATE == p_vlan->mode)
	{
        #if defined(CONFIG_PRODUCT_EPN200)
        
		/*Not support*/
        ret = OPL_NOT_SUPPORTED;
        goto response;		
        
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: OAM_CTC_VLAN_TRANSLATION\n", __FUNCTION__);
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "vlan mode translation.\n");

		vosMemCpy(&defaultTag, p_in, 4);
		p_in += 4;

		/* check if default vlan exist vlan list or exist duplicate vlan */
		vlanId = defaultTag & 0xFFF;
		vosMemSet(vlanlist, 0, sizeof(vlanlist));
		vlanIndex = 0;
		p_vlan_check = p_in;
		for(i = 0; i < (p_vlan->width - 5)/8; i++)
		{
			vosMemCpy(&oldTag, (u8_t*)p_vlan_check, 4);
			p_vlan_check += 4;
			vosMemCpy(&newTag, (u8_t*)p_vlan_check, 4);
			p_vlan_check += 4;

			oldTag = oldTag&0xFFF;
			newTag = newTag&0xFFF;

			if (oldTag == newTag)
			{
		    	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "vlan id to be translate is equal vlan id that translated.\n");
				ret = OAM_SET_BAD_PARAMETERS;
				goto response;
			}
			
			if (eopl_IsExistList(vlanlist, oldTag, vlanIndex) || eopl_IsExistList(vlanlist, newTag, vlanIndex))
			{
		    	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "vlan %d.%d duplicate.\n", oldTag, newTag);
				ret = OAM_SET_BAD_PARAMETERS;
				goto response;
			}
			vlanlist[vlanIndex++] = oldTag;
			vlanlist[vlanIndex++] = newTag;

			/* max support 16 translate record */
			if (vlanIndex > 16)
			{
                /*begin modified by liaohongjun 2012/7/30 of QID0001*/
                printf("\n Onu only supports 8 vlan translation pairs for every port. \n");
                /*end modified by liaohongjun 2012/7/30 of QID0001*/
				ret = OAM_SET_BAD_PARAMETERS;
				goto response;
			}

			if (oldTag == vlanId)
			{
                /*begin modified by liaohongjun 2012/7/30 of QID0001*/
		    	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "default tag %d exist in vlan list that be translate.\n", vlanId);
                /*end modified by liaohongjun 2012/7/30 of QID0001*/
				ret = OAM_SET_BAD_PARAMETERS;
				goto response;
			}
		}		
		
		//set_vlan_to_eth2(defaultTag,OAM_CTC_VLAN_TRANSLATE);

		////20100327_XFAN_ADD_BEGIN
#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
              UINT32 vlanAddNum;
              vlanAddNum = 0;
              extern UINT8	dal_vtt_num;
			        
              if(portS != portE) {
                vlanAddNum = ODM_NUM_OF_PORTS*((p_vlan->width - 5)/8);
              }
              else {
                vlanAddNum = 1*((p_vlan->width - 5)/8);
              }
			        
              if(vlanAddNum>16) {
              	ret += 1;
                OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_vlan OAM_CTC_VLAN_TRANSLATE: %d+%d>SHIVA_MAX_VLAN_TRANS\n", dal_vtt_num, vlanAddNum);
                goto response;
              }
#elif defined(ONU_1PORT)
              UINT32 vlanAddNum = 0;
              
              if(portS != portE) {
                vlanAddNum = ODM_NUM_OF_PORTS*((p_vlan->width - 5)/8);
              }
              else {
                vlanAddNum = 1*((p_vlan->width - 5)/8);
              }

              if(vlanAddNum>11) {
                ret += 1;
                OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_vlan OAM_CTC_VLAN_TRANSLATE: %d>11\n",  vlanAddNum);
                goto response;
              }
#endif
    ////20100327_XFAN_ADD_END		
		for(j = portS; j <= portE; j++)
		{
			//odmPortVttFlush(j);

			ret = eopl_set_tag_id(j, &defaultTag);
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[port %d] defaultTag 0x%08x\n", j, defaultTag);
			if (OPL_OK != ret)
			{
				goto response;
			}				

			ret = odmPortVlanModeSet(j, OAM_CTC_VLAN_TRANSLATE);
			if (OPL_OK != ret)
			{
				goto response;
			}
		}

		for(i=0;i<(p_vlan->width-5)/8;i++)
		{
			vosMemCpy(&oldTag, (u8_t*)p_in, 4);
			p_in += 4;
			vosMemCpy(&newTag, (u8_t*)p_in, 4);
			p_in += 4;

			for(j = portS; j <= portE; j++)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[port %d] old tag: 0x%08x, new tag: 0x%08x\n", j, oldTag, newTag);
				ret = odmPortVttEntryAdd(j, 0, oldTag, 0, newTag);
				if (OPL_OK != ret)
				{
					goto response;
				}
			}
		}
        #else
        i32_t iRet = NO_ERROR;
        u32_t uiVlanPair;
        u32_t priority;
        
        ret = OPL_OK;

        vosMemCpy(&defaultTag, p_in, 4);
        p_in += 4;
		vlanIndex = 0;
		p_vlan_check = p_in;

        uiVlanPair = (p_vlan->width - 5)/8;
        
		for(i = 0; i < uiVlanPair; i++)
        {
			vosMemCpy(&oldTag, (u8_t*)p_vlan_check, 4);
			p_vlan_check += 4;
			vosMemCpy(&newTag, (u8_t*)p_vlan_check, 4);
			p_vlan_check += 4;

			oldTag = oldTag&0xFFF;
			newTag = newTag&0xFFF;
            //printf(" %d to %d \n",oldTag,newTag); 
         
			vlanlist[vlanIndex++] = oldTag;
			vlanlist[vlanIndex++] = newTag;

			/* max support 16 translate record */
			if (vlanIndex > 16)
			{
                printf("\n Onu only supports 8 vlan translation pairs for every port. \n");
				ret = OAM_SET_BAD_PARAMETERS;
				goto response;
			}
		}
        iRet = CTC_DRV_SetTranslationVlan(portNum, defaultTag, vlanlist, (vlanIndex/2));
		if (NO_ERROR != iRet)
        {
            printf("\nFailed to set translation vlan for port %d.\n",portNum);
            ret = OPL_ERROR;
		    goto response;
        }
        #endif
	}
	else if(OAM_CTC_VLAN_AGGREGATION == p_vlan->mode)
	{

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: OAM_CTC_VLAN_AGGREGATION\n", __FUNCTION__);
		#if defined(ONU_1PORT) || defined(ONU_4PORT_AR8228)|| defined(ONU_4PORT_AR8327)

		vosMemCpy(&default_vlan, p_in, 4);
		p_in += sizeof(u32_t);

		vosMemCpy(&num_of_aggr_table, p_in, 2);
		p_in += sizeof(u16_t);
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ] num_of_aggr_table: %d: OAM_CTC_VLAN_AGGREGATION\n", __FUNCTION__,num_of_aggr_table);

		/* check if default vlan exist vlan list */
		vlanId = default_vlan & 0xFFF;
		p_vlan_check = p_in;
		for(i = 0; i < num_of_aggr_table; i++)
		{
			vosMemCpy(&num_of_src_vlan, p_vlan_check, 2);
			p_vlan_check += sizeof(u16_t);

			vosMemCpy(&dst_vlan, p_vlan_check, 4);
			dst_vlan = dst_vlan & 0xFFF;
			p_vlan_check += sizeof(u32_t);

			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "default vlan is %d, dest vlan is %d.\n", vlanId, dst_vlan);

			if ( dst_vlan == 0)
			{
		    	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Target vlan is 0.\n");
				ret = ERR_VARNORESOURCE;
				goto response;	
			}
			
			if ( dst_vlan == vlanId)
			{
                /*begin modified by liaohongjun 2012/7/30 of QID0001*/
		    	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "default tag %d exist in vlan list that be aggregation.\n", dst_vlan);
                /*end modified by liaohongjun 2012/7/30 of QID0001*/
				ret = ERR_VARNORESOURCE;
				goto response;	
			}

			vosMemSet(vlanlist, 0, sizeof(vlanlist));
			vlanIndex = 0;		
			for(j = 0; j < num_of_src_vlan; j++)
			{
				vosMemCpy(&src_vlan, p_vlan_check, 4);
				src_vlan = src_vlan & 0xFFF;
				p_vlan_check += sizeof(u32_t);	

				if (eopl_IsExistList(vlanlist, src_vlan, vlanIndex))
				{
			    	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "vlan %d duplicate.\n", src_vlan);
					ret = OAM_SET_BAD_PARAMETERS;
					goto response;			
				}
				vlanlist[vlanIndex++] = src_vlan;							

				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "src vlan is %d.\n", src_vlan);

				if (src_vlan == vlanId)
				{
                    /*begin modified by liaohongjun 2012/7/30 of QID0001*/
			    	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "default tag %d exist in vlan list that be aggregation.\n", src_vlan);
                    /*end modified by liaohongjun 2012/7/30 of QID0001*/
					ret = ERR_VARNORESOURCE;
					goto response;	
				}				
			}
		}
		
		for(; portS<=portE; portS++)
		{
			/* set default vlan */
			ret = eopl_set_tag_id(portS, &default_vlan);
			if (OPL_OK != ret)
			{
				goto response;
			}			

			/* set vlan mode */
			ret = odmPortVlanModeSet(portS, OAM_CTC_VLAN_AGGREGATION);
			if (OPL_OK != ret)
			{
				goto response;
			}			

			/* clear old vlan aggr entries */
			odmVlanAggDelByPort(portS);
			for(i = 0; i < num_of_aggr_table; i++)
			{
				vosMemCpy(&num_of_src_vlan, p_in, 2);
				p_in += sizeof(u16_t);

				vosMemCpy(&dst_vlan, p_in, 4);
				p_in += sizeof(u32_t);

				/* clear old vlan aggr entries */

				for(j = 0; j < num_of_src_vlan; j++)
				{
					vosMemCpy(&src_vlan, p_in, 4);
					p_in += sizeof(u32_t);
					/* call odm api */
					ret = odmVlanAggAdd(portS, dst_vlan & 0xFFF, src_vlan& 0xFFF);
					if(ret != OPL_OK)
					{
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: Add vlan aggregation record failed[%d]!\n", __FUNCTION__, ret);
						goto response;
					}
				}
			}
		}
		#else
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: ONU not support VLAN aggregation mode!\n", __FUNCTION__);
			ret = ERR_VARNORESOURCE;
			goto response;
		#endif
	}
	else if(OAM_CTC_VLAN_TRUNK == p_vlan->mode)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: OAM_CTC_VLAN_TRUNK\n", __FUNCTION__);

		////20100327_XFAN_ADD_BEGIN
#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
              UINT32 vlanAddNum;
              vlanAddNum = 0;
              extern UINT8	dal_vtt_num;
			        
              if(portS != portE) {
                vlanAddNum = ODM_NUM_OF_PORTS*((p_vlan->width - 1 - 4)/4);
              }
              else {
                vlanAddNum = 1*((p_vlan->width - 1 - 4)/4);
              }
			        
              if(vlanAddNum>16) {
              	ret += 1;
                OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_vlan OAM_CTC_VLAN_TRUNK: %d+%d>SHIVA_MAX_VLAN_TRANS\n", dal_vtt_num, vlanAddNum);
                goto response;
              }
#elif defined(ONU_1PORT)
              UINT32 vlanAddNum = 0;
			        
              if(portS != portE) {
                vlanAddNum = ODM_NUM_OF_PORTS*((p_vlan->width - 1 - 4)/4);
              }
              else {
                vlanAddNum = 1*((p_vlan->width - 1 - 4)/4);
              }

              if(vlanAddNum>11) {
                ret += 1;
                OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_vlan OAM_CTC_VLAN_TRUNK: %d>11\n", vlanAddNum);
                goto response;
              }
#endif
    ////20100327_XFAN_ADD_END		
    
		for(; portS<=portE; portS++)
		{
			vosMemCpy(&default_vlan, p_in, 4);
			p_in += sizeof(u32_t);

			/* set default vlan */
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: eopl_set_tag_id(%d, %llx)\n", __FUNCTION__, portS, default_vlan);

			/* check if default vlan exist vlan list */
			vosMemSet(vlanlist, 0, sizeof(vlanlist));
			vlanIndex = 0;			
			default_vlan = default_vlan & 0x00000FFF;
			vlanTrunkNum = (p_vlan->width - 1 - 4)/4;
			p_vlan_check = p_in;
			for(i = 0; i < vlanTrunkNum; i++)
			{
				vosMemCpy(&vlanId, p_vlan_check, 4);
				p_vlan_check += sizeof(u32_t);
				vlanId = vlanId&0xFFF;

				if (eopl_IsExistList(vlanlist, vlanId, vlanIndex))
				{
			    	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "vlan %d duplicate.\n", vlanId);
					ret = OAM_SET_BAD_PARAMETERS;
					goto response;			
				}
				vlanlist[vlanIndex++] = vlanId;

				if (default_vlan == vlanId)
				{					
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: default tag exist in vlan list\n", __FUNCTION__);
					ret = ERR_INVALID_PARAMETERS;
					goto response;
				}
			}
			
            ret = odmPortDefaultVlanSet(portS, default_vlan);
			if (OPL_OK != ret)
			{
				goto response;
			}				
			
			/* set vlan mode */
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: odmPortVlanModeSet(%d, OAM_CTC_VLAN_TRUNK)\n", __FUNCTION__, portS);
			ret = odmPortVlanModeSet(portS, OAM_CTC_VLAN_TRUNK);
			if (OPL_OK != ret)
			{
				goto response;
			}

			/* clear old vlan trunk entries */
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: odmPortVlanTrunkEntryClear(%d)\n", __FUNCTION__, portS);
			ret = odmPortVlanTrunkEntryClear(portS);
			if (OPL_OK != ret)
			{
				goto response;
			}

			/* set new vlan trunk entries */
			for(i = 0; i < vlanTrunkNum; i++)
			{
				vosMemCpy(&vlanId, p_in, 4);
				p_in += sizeof(u32_t);

				/* set vlan trunk entry */
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: odmPortVlanTrunkEntryAdd(%d, %llx)\n", __FUNCTION__, portS, vlanId);
				ret = odmPortVlanTrunkEntryAdd(portS, vlanId);
				if (OPL_OK != ret)
				{
					goto response;
				}
			}
		}


		ret = OPL_OK;
	}else {
        ret = ERR_VARNORESOURCE;
    }


response:

	switch(ret)
	{
		case OPL_OK:
			p_rsp->indication = OAM_SET_OK;
			break;
		case PORT_INVALID:
		case ERR_INVALID_PARAMETERS:
			p_rsp->indication = OAM_SET_BAD_PARAMETERS;
			break;
		default:
			p_rsp->indication = OAM_SET_NO_RESOURCE;
			break;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return 0;
}

u8_t stub_ctc_port_rule_clear(u8_t port_id)
{
	u32_t ret;

	OP_DEBUG(DEBUG_LEVEL_INFO, "call odmQosClsRuleClear(%d)\n", port_id);
	ret = odmOamClassRemoveAll(port_id);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_INFO, "call odmQosClsRuleClear(%d) fail!\n", port_id);
		return OAM_SET_NO_RESOURCE;
	}

	return OAM_SET_OK;
}

u8_t stub_ctc_port_rule_delete(u8_t port_id, u8_t precedenceOfRule)
{
	u32_t ret;

	OP_DEBUG(DEBUG_LEVEL_INFO, "call odmQosClsRuleDelete(%d, %d)\n", port_id, precedenceOfRule);
	ret = odmOamClassRemove(port_id, precedenceOfRule);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_INFO, "call odmQosClsRuleDelete(%d, %d) fail!\n", port_id, precedenceOfRule);
		return OAM_SET_NO_RESOURCE;
	}

	return OAM_SET_OK;
}

u8_t stub_ctc_port_rule_add(u8_t portId,
	                         u8_t precedenceOfRule,
	                         oam_clasmark_rulebody_t *pClsRule,
	                         oam_clasmark_fieldbody_t *pClsField)
{
	u32_t ret;
	CLS_CONFIG_INFO_t odmClsRule;
	u8_t numOfField;
	u8_t fieldId;
	UINT8  fullmacMask[MAC_LENGTH] = {
	  0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF
	};

	numOfField = pClsRule->numOfField;

	vosMemSet(&odmClsRule, 0, sizeof(CLS_CONFIG_INFO_t));
	odmClsRule.rulePri =pClsRule->precedenceOfRule;
	odmClsRule.queueMapId = pClsRule->queueMapped;
	odmClsRule.cosMapVal = pClsRule->ethPriMark;

    /* only up stream take effect */
    odmClsRule.bingress=0x0;
    odmClsRule.ingressmask=0x01;

	/*modified by ltang,for bug3222 ---start*/
	/*in CTC mode we set the rule entry RMK_ACT bit215:216 default value  [1:1] */
	odmClsRule.rmkcoschange =1;
	odmClsRule.rmkicoschange =1;
	/*modified by ltang,for bug3222 ---end*/
	for(fieldId = 0; fieldId < numOfField; fieldId++)
	{
		if(pClsField->fieldSelect == OAM_CTC_CLS_FIELD_TYPE_DA_MAC)
		{
			odmClsRule.dstMacFlag = 1;
			odmClsRule.dstMacOp = pClsField->operator;
			vosMemCpy(odmClsRule.dstMac.lowRange, pClsField->matchValue, 6);
			vosMemCpy(odmClsRule.dstMac.highRange,fullmacMask,MAC_LENGTH);
			OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: dst MAC = %.2x:%.2x%.2x:%.2x:%.2x:%.2x:\n",
				odmClsRule.dstMac.lowRange[0],
				odmClsRule.dstMac.lowRange[1],
				odmClsRule.dstMac.lowRange[2],
				odmClsRule.dstMac.lowRange[3],
				odmClsRule.dstMac.lowRange[4],
				odmClsRule.dstMac.lowRange[5]);

			pClsField++;

			continue;
		}

		if(pClsField->fieldSelect == OAM_CTC_CLS_FIELD_TYPE_SA_MAC)
		{
			odmClsRule.srcMacFlag = 1;
			odmClsRule.srcMacOp = pClsField->operator;
			vosMemCpy(odmClsRule.srcMac.lowRange, pClsField->matchValue, 6);
			vosMemCpy(odmClsRule.srcMac.highRange,fullmacMask,MAC_LENGTH);
			OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: source MAC = %.2x:%.2x%.2x:%.2x:%.2x:%.2x:\n",
				odmClsRule.srcMac.lowRange[0],
				odmClsRule.srcMac.lowRange[1],
				odmClsRule.srcMac.lowRange[2],
				odmClsRule.srcMac.lowRange[3],
				odmClsRule.srcMac.lowRange[4],
				odmClsRule.srcMac.lowRange[5]);

			pClsField++;

			continue;
		}

		if(pClsField->fieldSelect == OAM_CTC_CLS_FIELD_TYPE_ETH_PRI)
		{

			odmClsRule.etherPriFlag = 1;
			odmClsRule.etherPriOp = pClsField->operator;
			odmClsRule.etherPri.lowRange = pClsField->matchValue[5];
			odmClsRule.etherPri.highRange=0xff;
			OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: ether priority = %d\n", odmClsRule.etherPri.lowRange);

			pClsField++;

			continue;
		}

		if(pClsField->fieldSelect == OAM_CTC_CLS_FIELD_TYPE_VLAN_ID)
		{

			odmClsRule.vlanIdFlag = 1;
			odmClsRule.vlanIdOp = pClsField->operator;
			odmClsRule.vlanId.lowRange = pClsField->matchValue[4] << 8;
			odmClsRule.vlanId.lowRange |= pClsField->matchValue[5];
			odmClsRule.vlanId.highRange=0xffff;

			OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: vlan Id = %d\n", odmClsRule.vlanId.lowRange);

			pClsField++;

			continue;
		}

		if(pClsField->fieldSelect == OAM_CTC_CLS_FIELD_TYPE_ETHER_TYPE)
		{

			odmClsRule.lenOrTypeFlag= 1;
			odmClsRule.lenOrTypeOp = pClsField->operator;
			odmClsRule.lenOrType.lowRange = pClsField->matchValue[4] << 8;
			odmClsRule.lenOrType.lowRange |= pClsField->matchValue[5];
			odmClsRule.lenOrType.highRange=0xffff;
			OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: ether type = 0x%lx\n", odmClsRule.lenOrType.lowRange);

			pClsField++;

			continue;
		}

		if(pClsField->fieldSelect == OAM_CTC_CLS_FIELD_TYPE_DST_IP)
		{

			odmClsRule.dstIpFlag= 1;
			odmClsRule.dstIpOp = pClsField->operator;
			odmClsRule.dstIp.lowRange = pClsField->matchValue[2] << 24;
			odmClsRule.dstIp.lowRange |= pClsField->matchValue[3] << 16;
			odmClsRule.dstIp.lowRange |= pClsField->matchValue[4] << 8;
			odmClsRule.dstIp.lowRange |= pClsField->matchValue[5];
			odmClsRule.dstIp.highRange=0xffffffff;
			OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: dst IP = 0x%lx\n", odmClsRule.dstIp.lowRange);

			pClsField++;

			continue;
		}

		if(pClsField->fieldSelect == OAM_CTC_CLS_FIELD_TYPE_SRC_IP)
		{

			odmClsRule.srcIpFlag= 1;

			odmClsRule.srcIpOp = pClsField->operator;
			odmClsRule.srcIp.lowRange = pClsField->matchValue[2] << 24;
			odmClsRule.srcIp.lowRange |= pClsField->matchValue[3] << 16;
			odmClsRule.srcIp.lowRange |= pClsField->matchValue[4] << 8;
			odmClsRule.srcIp.lowRange |= pClsField->matchValue[5];
			odmClsRule.srcIp.highRange=0xffffffff;
			OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: src IP = 0x%lx\n", odmClsRule.srcIp.lowRange);

			pClsField++;

			continue;
		}

		if(pClsField->fieldSelect == OAM_CTC_CLS_FIELD_TYPE_IP_TYPE)
		{

			odmClsRule.ipTypeFlag= 1;

			odmClsRule.ipTypeOp = pClsField->operator;
			odmClsRule.ipType.lowRange = pClsField->matchValue[5];
			odmClsRule.ipType.highRange=0xff;
			OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: IP Type = 0x%x\n", odmClsRule.ipType.lowRange);

			pClsField++;

			continue;
		}

		if(pClsField->fieldSelect == OAM_CTC_CLS_FIELD_TYPE_IP_DSCP)
		{

			odmClsRule.ipV4DscpFlag= 1;

			odmClsRule.ipV4DscpOp = pClsField->operator;
			odmClsRule.ipV4Dscp.lowRange = (pClsField->matchValue[5])<<2;
			odmClsRule.ipV4Dscp.highRange=0xff;
			OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: IPV4 DSCP = 0x%x\n", odmClsRule.ipV4Dscp.lowRange);

			pClsField++;

			continue;
		}

		if(pClsField->fieldSelect == OAM_CTC_CLS_FIELD_TYPE_IP_PRECEDENCE)
		{

			odmClsRule.ipV6PreFlag= 1;

			odmClsRule.ipV6PreOp = pClsField->operator;
			odmClsRule.ipV6Pre.lowRange = pClsField->matchValue[5];
			OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: IPV6 precedence = %d\n", odmClsRule.ipV6Pre.lowRange);
			odmClsRule.ipV6Pre.highRange=0xff;

			pClsField++;

			continue;
		}

		if(pClsField->fieldSelect == OAM_CTC_CLS_FIELD_TYPE_L4_SRC_PORT)
		{

			odmClsRule.srcL4PortFlag = 1;

			odmClsRule.srcL4PortOp = pClsField->operator;
			odmClsRule.srcL4Port.lowRange = pClsField->matchValue[4] << 8;
			odmClsRule.srcL4Port.lowRange |= pClsField->matchValue[5];
			odmClsRule.srcL4Port.highRange=0xffff;
			OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: L4 src port = %d\n", odmClsRule.srcL4Port.lowRange);

			pClsField++;

			continue;
		}

		if(pClsField->fieldSelect == OAM_CTC_CLS_FIELD_TYPE_L4_DST_PORT)
		{

			odmClsRule.dstL4PortFlag = 1;
			odmClsRule.dstL4PortOp = pClsField->operator;
			odmClsRule.dstL4Port.lowRange = pClsField->matchValue[4] << 8;
			odmClsRule.dstL4Port.lowRange |= pClsField->matchValue[5];
			odmClsRule.dstL4Port.highRange=0xffff;
			OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: L4 dst port = %d\n", odmClsRule.dstL4Port.lowRange);

			pClsField++;

			continue;
		}

		pClsField++;
	}

	OP_DEBUG(DEBUG_LEVEL_INFO, "call odmQosClsRuleAdd(%d, %d, odmClsRule)\n", portId, precedenceOfRule);
	ret = odmOamClassInstall(portId, precedenceOfRule, &odmClsRule);
	if(ret != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_INFO, "call odmQosClsRuleAdd(%d, %d, odmClsRule) fail!\n", portId, precedenceOfRule);
		return OAM_SET_NO_RESOURCE;
	}

	return OAM_SET_OK;
}

void eopl_set_onu_classfy(u8_t*  p_in)
{
	u16_t	portS, portE;
	u8_t    *p_byte;
	int		iStatus;
	oam_instance_tlv_t	*p_inst;
	oam_set_response_t	*p_rsp;
	oam_clasmark_t		*pClsHeader;
	oam_clasmark_rulebody_t	*pClsRule;
	oam_clasmark_fieldbody_t *pClsField;
	u32_t value;
	u8_t portType;
	u16_t portNum;
	u16_t portMax;
	u8_t ret;
	u8_t precedenceOfRule;
	u8_t numOfRule;
	u8_t numOfField;
	u8_t ruleId;
	u16_t curPortId;

	value = (u32_t)port_instance.value;
	portType = OAM_GET_PORT_TYPE(value);
	portNum = OAM_GET_PORT_NUM(value);


	portMax = CTC_MAX_SUBPORT_NUM;

/* 014035 */
#if 0
	if((portType != OAM_CTC_PORT_TYPE_ETH) || ((portNum > portMax)&&(portNum != 0xFFFF)))
#else
    if((portType != OAM_CTC_PORT_TYPE_ETH)
        || ((portNum > portMax) && (0xFF != portNum) && (portNum != 0xFFFF)))
#endif
/* End   014035 */
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t *)p_byte;
		*p_inst = port_instance;
		p_byte += sizeof(oam_instance_tlv_t);

		p_rsp = (oam_set_response_t*)p_byte;
		p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_rsp->leaf = OAM_ATTR_CLAMARK;
		p_rsp->indication = OAM_CTC_RSP_BAD_PARA;
		g_usOutDataIdx += sizeof(oam_instance_tlv_t) + sizeof(oam_set_response_t);
		return;
	}


	#if 0
	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		portS = portNum;
		portE = portNum;
	}
	#endif
	//begin modified by tanglin for the bug 2992
    
    #if 0
	if(0xFFFF == portNum)
	{
		portS = 1;
		portE = 1;
	}
	else
	{
		if ( portNum > 1 )
		{
			ret =OAM_SET_BAD_PARAMETERS;
			goto send_rsp;
		}
		
		portS = portNum;
		portE = portNum;
	}
    #else
    if(0xFF == portNum)
	{
		portS = 1;
		portE = portMax;
	}
	else
	{
		portS = portNum;
		portE = portNum;

	}
    #endif
   
	//end  modified by tanglin for the bug 2992
	pClsHeader = (oam_clasmark_t *)p_in;
	p_in += sizeof(oam_clasmark_t);
	if(OAM_CLASSIFY_ACTION_CLR == pClsHeader->action)
	{
		
        #if defined(CONFIG_PRODUCT_EPN200)
		for(curPortId = portS; curPortId <= portE; curPortId++)
		{
			ret = stub_ctc_port_rule_clear((u8_t)portS);
            if (ret != OAM_SET_OK)
            {
			    goto send_rsp;
            }
		}
        #else
        ret = OAM_SET_OK;
        for(curPortId = portS; curPortId <= portE; curPortId++)
        {
            //printf("\nctc2.1: Clear port%d's clf.\n",curPortId);
            if (NO_ERROR != DRV_CtcClearClfRmkByPort(curPortId))
            {
                printf("\nfunc:%s, line:%d\n",__FUNCTION__, __LINE__);
                ret = OAM_SET_NO_RESOURCE;
                goto send_rsp;
            }
        }
        #endif
		
	}
	else if(OAM_CLASSIFY_ACTION_DEL == pClsHeader->action)
	{
        BOOL bBatchCfg = FALSE;
        
		numOfRule = pClsHeader->numOfRule;
		for(ruleId = 0; ruleId < numOfRule; ruleId++)
		{
			pClsRule = (oam_clasmark_rulebody_t *)p_in;
			p_in += sizeof(oam_clasmark_rulebody_t);

			precedenceOfRule = pClsRule->precedenceOfRule;
			numOfField = pClsRule->numOfField;

			pClsField = (oam_clasmark_fieldbody_t *)p_in;
			p_in += numOfField * sizeof(oam_clasmark_fieldbody_t);
            
            #if defined(CONFIG_PRODUCT_EPN200)
            for(curPortId = portS; curPortId <= portE; curPortId++)
			{
				ret = stub_ctc_port_rule_delete((u8_t)curPortId, precedenceOfRule);
                if (ret != OAM_SET_OK)
                {
    			    goto send_rsp;
                }
			}
            #else
            /* The precedence of rule begins with 0 when OLT sets batch configuration.
            Otherwise the precedence begins with 1. */
            if (0 == precedenceOfRule)
            { 
                bBatchCfg = TRUE;
            }

            if (TRUE != bBatchCfg)
            {
                precedenceOfRule -= 1;
            }
            
            if (NO_ERROR != DRV_CtcDelClfRmkByPrec(portNum, precedenceOfRule))
            {
                printf("\nfunc:%s, line:%d\n",__FUNCTION__, __LINE__);
                ret = OAM_SET_NO_RESOURCE;
                goto send_rsp;
            }
            #endif          
		}
	}
	else if((OAM_CLASSIFY_ACTION_ADD == pClsHeader->action))
	{
        BOOL bBatchCfg = FALSE;
        
      
        #if defined(CONFIG_PRODUCT_EPN200)
		/*Not support*/
        ret = OPL_NOT_SUPPORTED;
        goto send_rsp;
        #endif
		
        
		numOfRule = pClsHeader->numOfRule;
		for(ruleId = 0; ruleId < numOfRule; ruleId++)
		{
			pClsRule = (oam_clasmark_rulebody_t *)p_in;
			p_in += sizeof(oam_clasmark_rulebody_t);
			precedenceOfRule = pClsRule->precedenceOfRule;
			numOfField = pClsRule->numOfField;
			pClsField = (oam_clasmark_fieldbody_t *)p_in;
			p_in += numOfField * sizeof(oam_clasmark_fieldbody_t);
            /* 014090 */
            /* only support equal and always-match */
            if((OAM_CLF_EQUAL != pClsField->operator) && (OAM_CLF_ALWAYS_MATCH != pClsField->operator))
            {
               /*Not support*/
                ret = OPL_NOT_SUPPORTED;
                goto send_rsp;
            }
            /* 014090 */

            
            #if defined(CONFIG_PRODUCT_EPN200)
            for(curPortId = portS; curPortId <= portE; curPortId++)
			{
				ret = stub_ctc_port_rule_add((u8_t)curPortId, precedenceOfRule, pClsRule, pClsField);
                if (ret != OAM_SET_OK)
                {
    			    goto send_rsp;
                }
			}
            #else
            /* The precedence of rule begins with 0, when OLT sets batch configuration.
            Otherwise the precedence begins with 1. */
            if (0 == precedenceOfRule)
            { 
                bBatchCfg = TRUE;
            }

            if (TRUE != bBatchCfg)
            {
                precedenceOfRule -= 1;
            }

            //printf("\nctc2.1: Add clf for port%d. prece:%d, num:%d\n",portNum,precedenceOfRule,numOfRule);
            if (NO_ERROR != DRV_CtcAddClfRmkByPrec(portNum, precedenceOfRule, pClsRule, pClsField))
            {
                printf("\nfunc:%s, line:%d\n",__FUNCTION__, __LINE__);
                ret = OAM_SET_NO_RESOURCE;
                goto send_rsp;
            }
            #endif
       
		}
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "classify unknown\n");
		p_rsp->indication = OAM_SET_BAD_PARAMETERS;
	}
send_rsp:
	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t *)p_byte;
	*p_inst =   port_instance;
    /* D014035 */
    /*PMC OLT bug, must use 0xffff for all port mask*/
    if ((0xFF == portNum) && (OLT_FLAG_H3C == OamGetOltFlag()))
    {
        p_inst->value |= 0x0000FFFF;
    }

    /* 014035 */
	p_byte += sizeof(oam_instance_tlv_t);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t);

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_CLAMARK;

	p_rsp->indication = ret;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

UINT8 PonTxPowerDownByOam;

extern UINT32 odmPonTxPowerDownAndDuration();

int eopl_set_onu_tx_power_supply_ctrl(u8_t *p_in)
{
	u8_t *p_out;
	oam_onu_tx_power_supply_ctrl_t *p_tx_power_supply_ctrl;
	oam_set_response_t *p_rsp;
	u32_t ret;

	p_tx_power_supply_ctrl = (oam_onu_tx_power_supply_ctrl_t *)p_in;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: enter", __FUNCTION__);

	if (memcmp(p_tx_power_supply_ctrl->onu_id, oam_src_mac, 
		sizeof(p_tx_power_supply_ctrl->onu_id)) != 0 
		&& memcmp(p_tx_power_supply_ctrl->onu_id, oam_broadcast_mac, 
		sizeof(p_tx_power_supply_ctrl->onu_id)) != 0)
	{
		OP_DEBUG(DEBUG_LEVEL_WARNING, "%s: invalid ONU ID", __FUNCTION__);
		return;
	}

	/* TX power up */
	if (p_tx_power_supply_ctrl->action == 0)
	{
		ret = odmPonTxPowerDown(FALSE);
		PonTxPowerDownByOam = FALSE;
	}
	/* TX power down */
	else if (p_tx_power_supply_ctrl->action == 65535)
	{
		ret = odmPonTxPowerDown(TRUE);
		PonTxPowerDownByOam = TRUE;
	}
	/* TX power down and wait a duration then turn on tx power */
	else if (p_tx_power_supply_ctrl->action > 0 &&
		p_tx_power_supply_ctrl->action < 65535)
	{
		TIMER_OBJ_t * pstTimer = NULL;

		ret = odmPonTxPowerDown(TRUE);

		/* start a timer and wait the duration then turn on tx power */
		pstTimer = vosTimerGetFree();
	    if (pstTimer)
	    {
	        vosTimerStart(pstTimer, ONE_SHOT_TIMER, p_tx_power_supply_ctrl->action * 1000, (FUNCPTR)odmPonTxPowerDownAndDuration, 1, 2, 3, 4, 5, 6, 7, 8);
	    }
	
		PonTxPowerDownByOam = TRUE;
	}
	else 
	{
		OP_DEBUG(DEBUG_LEVEL_WARNING, "%s: invalid action value", __FUNCTION__);
		ret = OAM_SET_BAD_PARAMETERS;
	}

send_rsp:

	if(0 == g_usOutDataIdx)
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t *)p_out;

	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_TX_POWER_SUPPLY_CTRL;

	if(ret == 0)
		p_rsp->indication = OAM_SET_OK;
	else
		p_rsp->indication = OAM_SET_BAD_PARAMETERS;

	g_usOutDataIdx += sizeof(oam_set_response_t);
	return;
}


void eopl_ctc_set_req(u8_t* p_byte)
{
	u8_t	number;
	oam_service_sla_t           *p_sla;
	oam_service_sla_body_hdr_t  *p_sla_body_hdr;
	oam_variable_descriptor_t	*p_des;
	oam_eth_portpause_t			*p_pause;
	oam_eth_portpolicing_t		*p_policing;
	oam_eth_portdspolicing_t	*p_dspolicing;
	oam_phy_admincontrol_t		*p_phy_admin;
	oam_autoneg_admincontrol_t	*p_auto_admin;
	oam_fec_mode_t				*p_fec;
	oam_group_num_max_t  	   	*p_group;
	oam_mcast_control_t			*p_mcast_control;
	oam_mcast_switch_t			*p_mcast_switch;
	oam_mcast_tagstripe_t		*p_mcast_stripe;
	oam_mcast_vlan_t			*p_mcast_vlan;
	oam_fastleave_adm_t			*p_fast;
	oam_vlan_config_t			*p_vlan;
	oam_clasmark_t				*pstClassify;
	oam_voip_port_t             *p_voip_port;
	oam_e1_port_t               *p_e1_port;
	oam_alarm_admin_hdr_t     	*p_alarm_admin_hdr;
	oam_alarm_admin_entry_t     *p_alarm_admin_entry;
	oam_alarm_threshold_hdr_t   *p_alarm_threshold_hdr;
	oam_alarm_threshold_entry_t *p_alarm_threshold_entry;
	oam_multi_llid_ctrl_t       *p_multi_llid;
    u8_t *p_src = p_byte;

#ifdef OAM_SUPPORT_REG_CALLBACK
    u8_t *   p_in_buf = NULL;
    u8_t *   p_out_buf = NULL;
    OAM_EV_CB_ID_t key_val;
    vosMemSet(&key_val,0,sizeof(OAM_EV_CB_ID_t));
#endif

	while(1)
	{
        if ((p_byte - p_src) >= OAM_MAXIMUM_PDU_SIZE)
        {
            OP_DEBUG(DEBUG_LEVEL_CRITICAL, "invalid packet size");
            break;
        }

		switch(*p_byte)
		{
			case OAM_BRANCH_INSTANCE_V_2_1:
				port_instance = *((oam_instance_tlv_t*)p_byte);
				p_byte += sizeof(oam_instance_tlv_t);

				break;
			case OAM_BRANCH_INSTANCE_V_2_0:
			/*	port_instance_2_0= *((oam_instance_tlv_t_2_0*)p_byte);
				p_byte += sizeof(oam_instance_tlv_t_2_0);
				break;*/
				eopl_ctc_set_req_2_0(p_src);
				return;
			case OAM_BRANCH_EXTENDED_ATTRIB:
			case OAM_BRANCH_STANDARD_ATTRIB:
				p_des = (oam_variable_descriptor_t *)p_byte;
				switch(p_des->leaf)
				{
					case OAM_ATTR_SERVICE_SLA:
						p_sla = (oam_service_sla_t *)p_byte;
						eopl_set_onu_service_sla(p_byte);

						if(p_sla->operation == OAM_CTC_SERVICE_DBA_DEACTIVE)
						{
							p_byte += sizeof(oam_service_sla_t);
						}
						else if(p_sla->operation == OAM_CTC_SERVICE_DBA_ACTIVE)
						{
							p_byte += sizeof(oam_service_sla_t);

							p_sla_body_hdr = (oam_service_sla_body_hdr_t *)p_byte;
							p_byte += sizeof(oam_service_sla_body_hdr_t);

							p_byte += p_sla_body_hdr->service_num * 10;
						}
						break;
					case OAM_ATTR_HOLDOVER_CONFIG:
						eopl_set_onu_holdover_config(p_byte);
						p_byte += sizeof(oam_onu_holdover_config_t);
						break;

					case OAM_ATTR_MXU_MNG_GLOBAL_PARA:
						eopl_set_onu_mxu_mng_global_para(p_byte);
						p_byte += sizeof(oam_mxu_mng_global_para_t);
						break;

					case OAM_ATTR_MXU_MNG_SNMP_PARA:
						eopl_set_onu_mxu_mng_snmp_para(p_byte);
						p_byte += sizeof(oam_mxu_mng_snmp_para_t);
						break;

					case OAM_ATTR_ACTIVE_PON_IF_ADMIN:
						eopl_set_onu_active_pon_if_admin(p_byte);
						p_byte += sizeof(oam_active_pon_if_admin_t);
						break;

					case OAM_ATTR_ETH_PORTPAUSE:
						p_pause = (oam_eth_portpause_t*)p_byte;
						eopl_set_onu_eth_port_pause(p_pause->back_oper);
						p_byte += sizeof(oam_eth_portpause_t);
						break;
					case OAM_ATTR_ETH_PORT_US_POLICING:
						p_policing = (oam_eth_portpolicing_t*)p_byte;
						eopl_set_onu_eth_port_us_policing(p_policing->operation, p_policing->cir,p_policing->cbs,p_policing->ebs);
						p_byte += (4+p_policing->width);
						break;

#if defined(CTC_VOIP_DISPATCH)  /* modified by Gan Zhiheng - 2010/06/12 */
					case OAM_ATTR_VOIP_PORT:
						oamMsqSend(&in_data[4],in_data_len);
						return;
#else
					case OAM_ATTR_VOIP_PORT:
						p_voip_port = (oam_voip_port_t *)p_byte;
						eopl_set_onu_voip_port(p_voip_port->lock);
						p_byte += sizeof(oam_voip_port_t);
						break;
#endif
					case OAM_ATTR_E1_PORT:
						p_e1_port = (oam_e1_port_t *)p_byte;
						eopl_set_onu_e1_port(p_e1_port->lock);
						p_byte += sizeof(oam_e1_port_t);
						break;

					case OAM_ATTR_ETH_PORT_DS_POLICING:
						p_dspolicing = (oam_eth_portdspolicing_t*)p_byte;
						eopl_set_onu_eth_port_ds_policing(p_dspolicing->operation, p_dspolicing->cir,p_dspolicing->pir);
						p_byte += (4+p_dspolicing->width);
						break;

					case OAM_ATTR_PORT_LOOP_DETECT:
						eopl_set_port_loop_detect(p_byte);
						p_byte += sizeof(oam_port_loop_detect_t);
						break;

					case OAM_ATTR_VLAN:
						p_vlan = (oam_vlan_config_t*)p_byte;
						eopl_set_onu_vlan(p_byte);
						p_byte += p_vlan->width + 4;
						break;

					case OAM_ATTR_CLAMARK:
						pstClassify = (oam_clasmark_t *)p_byte;
						eopl_set_onu_classfy(p_byte);
						p_byte += pstClassify->width + 4;
						break;

					case OAM_ATTR_MCAST_VLAN:
						p_mcast_vlan = (oam_mcast_vlan_t*)p_byte;
						eopl_set_onu_mcast_vlan(p_byte);
						#if 0
						p_byte += sizeof(oam_mcast_vlan_t);
						if(p_mcast_vlan->width != 1)
						{
							number = p_mcast_vlan->width-1;
							if(number%2 !=0)
							{
								 return;
							}
							p_byte += 2*number;
						}
						break;
						#endif
						p_byte += p_mcast_vlan->width + 4;
						break;
					case OAM_ATTR_MCAST_TAG_STRIP:
						p_mcast_stripe = (oam_mcast_tagstripe_t*)p_byte;
						eopl_set_onu_mcast_tag_strip(p_byte);
						p_byte += sizeof(oam_mcast_tagstripe_t);
						break;

					case OAM_ATTR_MCAST_SWITCH:
						p_mcast_switch = (oam_mcast_switch_t*)p_byte;
						if((p_mcast_switch->action == 0) || (p_mcast_switch->action == 1))
						{
						eopl_set_onu_mcast_switch(p_mcast_switch->action);
						}
						p_byte += sizeof(oam_mcast_switch_t);
						break;

					case OAM_ATTR_MCAST_CONTROL:
						p_mcast_control = (oam_mcast_control_t*)p_byte;
						eopl_set_onu_mcast_control(p_byte);
						if(p_mcast_control->width == 1)
						{
							p_byte += sizeof(oam_mcast_control_t) - 2;
						}
						else
						{
							p_byte += sizeof(oam_mcast_control_t) + 10*p_mcast_control->number;
						}
						break;

					case OAM_ATTR_MCAST_GRP_NUMMAX:
						p_group = (oam_group_num_max_t*)p_byte;
						eopl_set_onu_mcast_group_max(p_group->num_max);
						p_byte += sizeof(oam_group_num_max_t);
						break;

					case OAM_ATTR_ALARM_ADMIN_STATE:
						p_byte  += sizeof(oam_alarm_admin_hdr_t);

						p_alarm_admin_entry = (oam_alarm_admin_entry_t *)p_byte;
						eopl_set_onu_alarm_admin_state(p_alarm_admin_entry->alarm_id,
													   p_alarm_admin_entry->alarm_config);
						p_byte += sizeof(oam_alarm_admin_entry_t);
						/* todo */
						break;

					case OAM_ATTR_ALARM_THRESHOLD:
						p_byte += sizeof(oam_alarm_threshold_hdr_t);

						p_alarm_threshold_entry = (oam_alarm_threshold_entry_t *)p_byte;
						eopl_set_onu_alarm_threshold(p_alarm_threshold_entry->alarm_id,
							                         p_alarm_threshold_entry->raising_threshold,
							                         p_alarm_threshold_entry->falling_threshold);
						p_byte += sizeof(oam_alarm_threshold_entry_t);
						break;

#if defined(CTC_VOIP_DISPATCH)  /* modified by Gan Zhiheng - 2010/06/08 */
					case OAM_ATTR_IAD_INFO:
					case OAM_ATTR_GLOBAL_PARA_CONFIG:
					case OAM_ATTR_H248_PARA_CONFIG:
					case OAM_ATTR_H248_USER_TID_CONFIG:
					case OAM_ATTR_H248_RTP_TID_CONFIG:
					case OAM_ATTR_H248_RTP_TID_INFO:
					case OAM_ATTR_SIP_PARA_CONFIG:
					case OAM_ATTR_SIP_USER_PARA_CONFIG:
					case OAM_ATTR_FAX_MODEM_CONFIG:
					case OAM_ATTR_IAD_OPERATION_STATUS:
					case OAM_ATTR_POTS_STATUS:
					case OAM_ATTR_IAD_OPERATION:
					case OAM_ATTR_SIP_DIGIT_MAP:
						oamMsqSend(&in_data[4],in_data_len);
						return;
#else
					case OAM_ATTR_GLOBAL_PARA_CONFIG:
						eopl_set_onu_global_para_cfg(p_byte);
						p_byte += sizeof(oam_global_para_cfg_t);
						break;

					case OAM_ATTR_H248_PARA_CONFIG:
						eopl_set_onu_h248_para_cfg(p_byte);
						p_byte += sizeof(oam_h248_para_cfg_t);
						break;

					case OAM_ATTR_H248_USER_TID_CONFIG:
						eopl_set_onu_h248_user_tid_cfg(p_byte);
						p_byte += sizeof(oam_h248_user_tid_cfg_t);
						break;

					case OAM_ATTR_H248_RTP_TID_CONFIG:
						eopl_set_onu_h248_rtp_tid_cfg(p_byte);
						p_byte += sizeof(oam_h248_rtp_tid_cfg_t);
						break;

					case OAM_ATTR_SIP_PARA_CONFIG:
						eopl_set_onu_sip_para_cfg(p_byte);
						p_byte += sizeof(oam_sip_para_cfg_t);
						break;

					case OAM_ATTR_SIP_USER_PARA_CONFIG:
						eopl_set_onu_sip_user_para_cfg(p_byte);
						p_byte += sizeof(oam_sip_user_para_cfg_t);
						break;

					case OAM_ATTR_FAX_MODEM_CONFIG:
						eopl_set_onu_fax_modem_cfg(p_byte);
						p_byte += sizeof(oam_fax_modem_cfg_t);
						break;

					case OAM_ATTR_SIP_DIGIT_MAP:
						eopl_set_sip_digit_map(p_byte);
						p_byte += sizeof(oam_sip_digit_map_t);
						break;
#endif
					case OAM_ATTR_LLID_QUEUE_CONFIG:
						eopl_set_onu_llid_queue_config(p_byte);
						p_byte += sizeof(oam_llid_queue_cfg_t);
						break;

					case OAM_ATTR_FEC_MODE:
						p_fec = (oam_fec_mode_t*)p_byte;
						eopl_set_onu_eth_fecmode(p_fec->value);
						p_byte += sizeof(oam_fec_mode_t);
						break;
					/* tx power supply control */
					case OAM_ATTR_TX_POWER_SUPPLY_CTRL:
						eopl_set_onu_tx_power_supply_ctrl(p_byte);
						p_byte += sizeof(oam_onu_tx_power_supply_ctrl_t);
						break;
					default:
#if defined(OAM_SUPPORT_REG_CALLBACK)
                        //OAM_DBG("[%s]Unknown leaf value,so search the OAM CALLBACK table\n ",__FUNCTION__ );
                        key_val.oam_ver = OAM_VER_CTC_2_1_SUPPORT;
                        key_val.oam_ext_code = OAM_EXT_CODE_SET_REQUEST ;
                        key_val.oam_branch_id = *p_byte;
                        key_val.oam_leaf_id = p_des->leaf;

                        p_in_buf = p_byte;
                        p_out_buf = eopl_get_output_buf(OAM_EXT_CODE_SET_REQUEST );
                        OamEventCallBack(key_val,
                                         p_in_buf,
                                         p_out_buf);
#elif defined(OAM_SUPPORT_ZTE_EXT) /* ZTE CTC extension */
						eopl_zte_ctc_set_req(p_des->leaf, &p_byte);
#else
                        eopl_not_support_response((u8_t)*p_byte,p_des->leaf,OAM_EXT_CODE_SET_RESPONSE);
                        p_byte += sizeof(oam_rsp_indication_t);
                        OAM_DBG("[%s]:%d Unknown leaf value!!\n ",__FUNCTION__, __LINE__);
#endif
	                    break;
				}
				break;
			case OAM_BRANCH_STANDARD_ACTION:
			case OAM_BRANCH_EXTENDED_ACTION:
				p_des = (oam_variable_descriptor_t *)p_byte;
				switch(p_des->leaf)
				{
					case OAM_ACTION_RESET_ONU:
						odmSysReset();
						return;
					case OAM_ACTION_FAST_LEAVE_ADMIN:
						p_fast = (oam_fastleave_adm_t*)p_byte;
						eopl_set_onu_mcast_fast_leave_control(p_fast->value);
						p_byte += sizeof(oam_fastleave_adm_t);
						break;
#if defined(CTC_VOIP_DISPATCH)  /* modified by Gan Zhiheng - 2011/03/23 */
					case OAM_ATTR_IAD_OPERATION:
					case OAM_ATTR_SIP_DIGIT_MAP:
						oamMsqSend(&in_data[4],in_data_len);
						return;
#else
					case OAM_ATTR_IAD_OPERATION:
						eopl_set_onu_iad_operation(p_byte);
						p_byte += sizeof(oam_iad_operation_t);
						break;
					case OAM_ATTR_SIP_DIGIT_MAP:
						eopl_set_sip_digit_map(p_byte);
						p_byte += sizeof(oam_sip_digit_map_t);
						break;
#endif
					case OAM_ACTION_MULTI_LLID_ADMIN:
						p_multi_llid = (oam_multi_llid_ctrl_t *)p_byte;
						eopl_set_onu_multi_llid_admin(p_multi_llid->llid_num);
						p_byte += sizeof(oam_multi_llid_ctrl_t);
						break;

					case OAM_ACTION_RESET_CARD:
						eopl_set_onu_reset_card();
						p_byte += 3;
						break;

					case OAM_ACTION_PHY_ADMIN_CONTROL:
						p_phy_admin = (oam_phy_admincontrol_t*)p_byte;
						eopl_set_onu_eth_admincontrol(p_phy_admin->value);
						p_byte += sizeof(oam_phy_admincontrol_t);
						break;
					case OAM_ACTION_AUTONEG_RST_AUTOCONFIG:
						eopl_set_onu_eth_atuorestart();
						p_byte += sizeof(oam_autoneg_restart_t);
						p_byte += 5; /* add to satisfy tk*/
						break;
					case OAM_ACTION_AUTONEG_ADMIN_CONTROL:
						p_auto_admin = (oam_autoneg_admincontrol_t*)p_byte;
						eopl_set_onu_eth_autonegadmin(p_auto_admin->indication);
						p_byte += sizeof(oam_phy_admincontrol_t);
						break;
					default:
#if defined(OAM_SUPPORT_REG_CALLBACK)
                        //OAM_DBG("[%s]Unknown leaf value,so search the OAM CALLBACK table\n ",__FUNCTION__ );
                        key_val.oam_ver = OAM_VER_CTC_2_1_SUPPORT;
                        key_val.oam_ext_code = OAM_EXT_CODE_SET_REQUEST ;
                        key_val.oam_branch_id = *p_byte;
                        key_val.oam_leaf_id = p_des->leaf;

                        p_in_buf = p_byte;
                        p_out_buf = eopl_get_output_buf(OAM_EXT_CODE_GET_REQUEST );
                        OamEventCallBack(key_val,
                                         p_in_buf,
                                         p_out_buf);
#elif defined(OAM_SUPPORT_ZTE_EXT) /* ZTE CTC extension */
						eopl_zte_ctc_set_req(p_des->leaf, &p_byte);
#else
                        eopl_not_support_response((u8_t)*p_byte,p_des->leaf,OAM_EXT_CODE_SET_RESPONSE);
                        p_byte += sizeof(oam_rsp_indication_t);
                        OAM_DBG("[%s]:%d Unknown leaf value!!\n ",__FUNCTION__, __LINE__);
#endif
						break;
				}
				break;
			default:
				if(g_usOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
			 	{
					oam_pdu_padding(&out_data[g_usOutDataIdx],  OAM_MINIMIZE_PDU_SIZE - g_usOutDataIdx);
					g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
			 	}
				OamFrameSend(out_data, g_usOutDataIdx);
				memset(&port_instance, 0, sizeof(oam_instance_tlv_t));
				//memset(&port_instance_2_0, 0, sizeof(oam_instance_tlv_t_2_0));
				g_usOutDataIdx = 0;
				return;
		}
	}

}

u8_t indexBuf=0xFF;
u8_t aucOldKeyVal[3]={0};
void eopl_req_new_key(u8_t index)
{
	u8_t	ucKeyId, *p_byte, aucKeyVal[3];

    
    if(index == indexBuf)
    {
	memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header(OAM_EXT_CODE_CHURNING);
	p_byte = &out_data[0];
	p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);

	*p_byte = OAM_NEW_CHURNING_KEY;
	p_byte++;
	ucKeyId = (0==index)?1:0;
	*p_byte = ucKeyId;
	p_byte++;
	/* interface to get churning key from hardware , 3bytes key*/
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "old key index in use: %d, to get: %d\n", index, ucKeyId);
	//odmPonUnicastTripleChunringKeyGet(ucKeyId, aucKeyVal);
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "old key: 0x%02x, 0x%02x, 0x%02x\n", aucOldKeyVal[0], aucOldKeyVal[1], aucOldKeyVal[2]);
	vosMemCpy(p_byte, aucOldKeyVal, 3);

	p_byte += 3;

	oam_pdu_padding(p_byte, 33);

	OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
    }
    else
    {
    memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header(OAM_EXT_CODE_CHURNING);
	p_byte = &out_data[0];
	p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);

	*p_byte = OAM_NEW_CHURNING_KEY;
	p_byte++;
	ucKeyId = (0==index)?1:0;
	*p_byte = ucKeyId;
	p_byte++;
	/* interface to get churning key from hardware , 3bytes key*/
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "key index in use: %d, to get: %d\n", index, ucKeyId);
	odmPonUnicastTripleChunringKeyGet(ucKeyId, aucKeyVal);
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "key: 0x%02x, 0x%02x, 0x%02x\n", aucKeyVal[0], aucKeyVal[1], aucKeyVal[2]);
	vosMemCpy(p_byte, aucKeyVal, 3);
	p_byte += 3;

	oam_pdu_padding(p_byte, 33);

	OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
    
    vosMemCpy(aucOldKeyVal, aucKeyVal, 3);
    indexBuf = index;
    }
}

void eopl_req_tri_scb_key(u8_t index, u8_t *pucKey)
{
	u8_t *p_byte, ucCnt;

	memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header(OAM_OPL_OPCODE_TRI_SCB);
	p_byte = &out_data[0];
	p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);

	*p_byte = OAM_NEW_CHURNING_KEY;
	p_byte++;
	*p_byte = index;
	p_byte++;
	/* interface to get churning key from hardware , 3bytes key*/
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "key index to set: %d\n", index);
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "key: ");
	for(ucCnt=0; ucCnt<16; ucCnt++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "0x%02x ", pucKey[ucCnt]);
	}
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n");
	odmPonScbTripleChurningKeySet(index, pucKey);
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "key: 0x%02x, 0x%02x, 0x%02x\n", pucKey[0], pucKey[1], pucKey[2]);
	vosMemCpy(p_byte, pucKey, 3);
	p_byte += 3;

	oam_pdu_padding(p_byte, 33);

	OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
}

void eopl_ctc_churning(u8_t* p_byte)
{
	switch(*p_byte)
	{
		case OAM_NEW_KEY_REQUEST:
			eopl_req_new_key(*(p_byte+1));
			break;
		default:
			break;
	}
}

void eopl_ctc_dba(u8_t* p_byte)
{
	switch(*p_byte)
	{
		case OAM_GET_DBA_REQUEST:
			eopl_get_dba();
			break;
		case OAM_SET_DBA_REQUEST:
			/*if(*(p_byte+1) != 2)
				return;*/
			p_byte+=2;
			eopl_set_dba(*(p_byte-1), (oam_dba_queue_t *)p_byte);
			break;
		default:
			break;
	}
}

void eopl_req_aes_key(u8_t index, u8_t ucMode)
{
	u8_t	ucKeyId, aucKeyVal[16], ucCnt, *p_byte;

	memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header(OAM_OPL_OPCODE_AES);
	p_byte = &out_data[0];
	p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);

	*p_byte++ = OAM_NEW_CHURNING_KEY;
	ucKeyId = (0==index)?1:0;
	*p_byte++ = ucKeyId;
	*p_byte++ = ucMode;
	/* interface to get churning key from hardware , 3bytes key*/
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "key index in use: %d, to get: %d\n", index, ucKeyId);
	odmPonUnicastAesKeyGet(ucKeyId, aucKeyVal, ucMode);

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "key: ");
	for(ucCnt=0; ucCnt<16; ucCnt++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "0x%02x ", aucKeyVal[ucCnt]);
	}
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n");
	vosMemCpy(p_byte, aucKeyVal, 16);
	p_byte += 16;

	oam_pdu_padding(p_byte, 20);

	OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
}

void eopl_req_aesscb_key(u8_t index, u8_t ucMode, u8_t *pucKey)
{
	u8_t	*p_byte, ucCnt;

	memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header(OAM_OPL_OPCODE_AES_SCB);
	p_byte = &out_data[0];
	p_byte += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);

	*p_byte++ = OAM_NEW_CHURNING_KEY;
	*p_byte++ = index;
	*p_byte++ = ucMode;
	/* interface to get churning key from hardware , 3bytes key*/
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "key index to set: %d\n", index);
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "key: ");
	for(ucCnt=0; ucCnt<16; ucCnt++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "0x%02x ", pucKey[ucCnt]);
	}
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n");
	odmPonScbAesKeySet(index, pucKey, ucMode);
	vosMemCpy(p_byte, pucKey, 16);
	p_byte += 16;

	oam_pdu_padding(p_byte, 20);

	OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
}

void eopl_aeskey_req(u8_t* p_byte)
{
	switch(*p_byte)
	{
		case OAM_NEW_KEY_REQUEST:
			eopl_req_aes_key(*(p_byte+1), *(p_byte+2));
			break;
		default:
			break;
	}
}

void eopl_aesscbkey_req(u8_t* p_byte)
{
	switch(*p_byte)
	{
		case OAM_NEW_KEY_REQUEST:
			eopl_req_aesscb_key(*(p_byte+1), *(p_byte+2), p_byte+3);
			break;
		default:
			break;
	}
}

void eopl_TriScbkey_req(u8_t* p_byte)
{
	switch(*p_byte)
	{
		case OAM_NEW_KEY_REQUEST:
			eopl_req_tri_scb_key(*(p_byte+1), p_byte+2);
			break;
		default:
			break;
	}
}

OPL_STATUS eopl_ctc_tftp_file_write_req_handler(u8_t *pucFileName, u8_t ucNameLen)
{
	u8_t *pucPkt;
	u32_t	uiLlid;
	SOFT_DOWNLOAD_HD_MASK_t *pstDownHd;
	TFTP_DATA_t *pstDataCur;
	TFTP_DATA_t *pstDataNext;

	if(g_upgradeOngoing)
	{
		pstDataCur = g_stImageFile.pstData;
		while(pstDataCur != NULL)
		{
			pstDataNext = pstDataCur->pstNext;
			vosFree(pstDataCur);
			pstDataCur = pstDataNext;
		}
	}

	OP_DEBUG(DEBUG_LEVEL_INFO, "Start to receive file: %s\n", pucFileName);
	g_upgradeOngoing = 1;

	g_stImageFile.ucNameLen = ucNameLen;
	g_stImageFile.ucFlash = 0;
	g_stImageFile.usBlock1 = 0;
	g_stImageFile.uiFileSize = 0;
	g_stImageFile.pstData = 0;

	memcpy(g_stImageFile.aucFileName, pucFileName, ucNameLen);
	memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header(OAM_EXT_CODE_DOWNLOAD);
	pucPkt = &out_data[0];
	pucPkt += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)pucPkt;
	pstDownHd->ucType = OAM_SOFT_TYPE_TFTP;
	pstDownHd->usLen = 0x09;
	dalPonLlidGet(&uiLlid);/* zlu modify */
	pstDownHd->usTid = uiLlid;/* zlu modify */
	*(u16_t *)(pstDownHd->aucMsg) = TFTP_ACK;
	*(u16_t *)&(pstDownHd->aucMsg[2]) = 0x0000;
	oam_pdu_padding(&out_data[31], 33);

	OP_DEBUG(DEBUG_LEVEL_INFO, "Send ack message for write request... \n");
	OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
#if 0
	if(!g_upgradeOngoing)
	{
		g_upgradeOngoing = 1;

		g_stImageFile.ucNameLen = ucNameLen;
		g_stImageFile.ucFlash = 0;
		g_stImageFile.usBlock1 = 0;
		g_stImageFile.uiFileSize = 0;
		g_stImageFile.pstData = 0;

		memcpy(g_stImageFile.aucFileName, pucFileName, ucNameLen);

		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_DOWNLOAD);
		pucPkt = &out_data[0];
		pucPkt += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)pucPkt;
		pstDownHd->ucType = OAM_SOFT_TYPE_TFTP;
		pstDownHd->usLen = 0x09;
		dalPonLlidGet((u32_t *)&(pstDownHd->usTid));
		*(u16_t *)(pstDownHd->aucMsg) = TFTP_ACK;
		*(u16_t *)&(pstDownHd->aucMsg[2]) = 0x0000;
		oam_pdu_padding(&out_data[31], 33);
		OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
	}
	else
	{
		OAMDBG(("\nOnu is already in firmwire upgrade process now!\n"));
		eopl_ctc_header(OAM_EXT_CODE_DOWNLOAD);
		pucPkt = &out_data[0];
		pucPkt += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)pucPkt;
		pstDownHd->ucType = OAM_SOFT_TYPE_TFTP;
		pstDownHd->usLen = 0x09;
		dalPonLlidGet((u32_t *)&(pstDownHd->usTid));
		*(u16_t *)(pstDownHd->aucMsg) = TFTP_ERR;
		*(u16_t *)&(pstDownHd->aucMsg[2]) = 0x0000;
		oam_pdu_padding(&out_data[31], 33);
		OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);

	}
#endif
	return 0;
}
/*
OPL_STATUS eopl_ctc_tftp_wreq(u8_t *pucFileName, u8_t ucNameLen)
{
	u8_t *pucPkt;
	SOFT_DOWNLOAD_HD_MASK_t *pstDownHd;

	if ((0 == g_stImageFile.ucNameLen)||(1 == g_stImageFile.ucFlash))
	{
		g_stImageFile.ucNameLen = ucNameLen;
		g_stImageFile.ucFlash = 0;
		g_stImageFile.usBlock1 = 0;
		g_stImageFile.uiFileSize = 0;
		g_stImageFile.pstData = 0;

		memcpy(g_stImageFile.aucFileName, pucFileName, ucNameLen);

		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header(OAM_EXT_CODE_DOWNLOAD);
		pucPkt = &out_data[0];
		pucPkt += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)pucPkt;
		pstDownHd->ucType = OAM_SOFT_TYPE_TFTP;
		pstDownHd->usLen = 0x09;
		dalPonLlidGet((u32_t *)&(pstDownHd->usTid));
		*(u16_t *)(pstDownHd->aucMsg) = TFTP_ACK;
		*(u16_t *)&(pstDownHd->aucMsg[2]) = 0x0000;
		oam_pdu_padding(&out_data[31], 33);
		OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
	}
	else
	{
		OAMDBG(("\nmaybe one file is being transfered or written to flash\n"));
	}

	return 0;
}
*/
OPL_STATUS eopl_ctc_gen_file(void)
{
	FILE *pFile;
	TFTP_DATA_t *pstData;

	pFile = fopen((char *)(g_stImageFile.aucFileName), "wb");

	pstData = g_stImageFile.pstData;
	while(pstData)
	{
		fwrite(pstData->aucDataBlock, 1, pstData->usLen, pFile);
		pstData = pstData->pstNext;
	}

	fclose(pFile);
	chmod((char *)g_stImageFile.aucFileName, S_IXUSR|S_IXGRP|S_IXOTH);

	return 0;
}

OPL_STATUS eopl_ctc_write_flash(void)
{
	OPL_STATUS iStatus=0;

	g_stImageFile.ucFlash = 1;

	return iStatus;
}

OPL_STATUS eopl_ctc_tftp_data_handler(u8_t *pucData, u16_t usDatalen)
{
	u8_t *pucIn;
	TFTP_DATA_t *pstData;
	SOFT_DOWNLOAD_HD_MASK_t *pstDownHd;
	u8_t *pucPkt;
	u16_t usBlockLen;
	u16_t usBlockNum;
	u32_t	uiLlid;

	pucIn = pucData;
	usBlockNum = *((u16_t *)pucIn);
	pucIn += sizeof(u16_t);

	usBlockLen = usDatalen - sizeof(u16_t);

	if (( usBlockLen >0 && usBlockLen <= 1400))
	{
		pstData = (TFTP_DATA_t *)vosAlloc(sizeof(TFTP_DATA_t));
		memcpy(pstData->aucDataBlock, pucIn, usBlockLen);
		pstData->usLen = usBlockLen;
		pstData->pstNext = 0;
	}
	else
	{
		/* OAMDBG(("Receive invalid tftp file transfer data!\n"));*/
		OP_DEBUG(DEBUG_LEVEL_INFO, "Receive invalid tftp file transfer data, len is invalid!... \n");
		return 0;
	}

	if(g_stImageFile.usBlock1+1 == usBlockNum)
	{
		/* OAMDBG(("file data block id(%d) mactch, block length: %d.\n", usBlockNum, usBlockLen));*/

		g_stImageFile.usBlock1 = usBlockNum;

		if (0 == g_stImageFile.pstData)
		{
			g_stImageFile.pstData = pstData;
			g_stImageFile.pstDataEnd = pstData;
			g_stImageFile.uiFileSize = (u32_t)usBlockLen;
		}
		else
		{
			g_stImageFile.pstDataEnd->pstNext = pstData;
			g_stImageFile.pstDataEnd = pstData;
			g_stImageFile.uiFileSize += (u32_t)usBlockLen;
		}

		OP_DEBUG(DEBUG_LEVEL_INFO, "Receive tftp file transfer data, serial no is %d!... \n", usBlockNum);
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_INFO, "Receive invalid tftp file transfer data, no is invalid!... \n");
	}

	memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header(OAM_EXT_CODE_DOWNLOAD);
	pucPkt = &out_data[0];
	pucPkt += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)pucPkt;
	pstDownHd->ucType = OAM_SOFT_TYPE_TFTP;
	pstDownHd->usLen = 0x09;
	dalPonLlidGet(&uiLlid);/* zlu modify */
	pstDownHd->usTid = uiLlid;/* zlu modify */
	*(u16_t *)(pstDownHd->aucMsg) = TFTP_ACK;
	*(u16_t *)&(pstDownHd->aucMsg[2]) = g_stImageFile.usBlock1;

	oam_pdu_padding(&out_data[31], 33);
	
	OP_DEBUG(DEBUG_LEVEL_INFO, "Send ack message for transfer data... \n");
	OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
	/* OAMDBG(("file data block %d acked\n", g_stImageFile.usBlock1));*/

	if(usBlockLen < 1400)
	{
		/* OAMDBG(("file transfer finish, file size = %d\n", g_stImageFile.uiFileSize));*/
		OP_DEBUG(DEBUG_LEVEL_INFO, "Finish receiving file: %s (%d Bytes)\n", g_stImageFile.aucFileName, g_stImageFile.uiFileSize);
	}

	return 0;
}

void eopl_ctc_send_end_download_response(u8_t ucRpsCode)
{
	u16_t usPktLen;
	ctc_header_t	*p_ctc;
	ctc_tftp_header_t *pstTftpHdr;
	u8_t *p_out;
	u32_t uiLlid;
	OP_DEBUG(DEBUG_LEVEL_INFO, "Send End Download Response PDU, RpsCode = %d!\n", ucRpsCode);
	memset(upgrade_out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	p_out = &upgrade_out_data[0];
	usPktLen = 0;
	eopl_oam_pdu_hdr(p_out, oams.flags, OAM_CODE_ORG_SPEC);
	p_out += sizeof(oampdu_hdr_t);
	usPktLen += sizeof(oampdu_hdr_t);
	p_ctc = (ctc_header_t*)p_out;
	vosMemCpy(p_ctc->oui, g_aucCtcOui, 3);
	p_ctc->ext_code = OAM_EXT_CODE_DOWNLOAD;
	p_out += sizeof(ctc_header_t);
	usPktLen += sizeof(ctc_header_t);
	pstTftpHdr = (ctc_tftp_header_t *)p_out;

	pstTftpHdr->data_type = OAM_SOFT_TYPE_CRC;
	pstTftpHdr->len = sizeof(ctc_tftp_header_t) + sizeof(u16_t) + sizeof(u8_t);
	dalPonLlidGet(&uiLlid);/* zlu modify */
	pstTftpHdr->tid = uiLlid;/* zlu modify */

	p_out += sizeof(ctc_tftp_header_t);
	usPktLen += sizeof(ctc_tftp_header_t);

	*((u16_t *)p_out) = TFTP_END_DOWNLOAD_RSP;
	p_out += sizeof(u16_t);/* zlu add */
	usPktLen += sizeof(u16_t);

	*((u8_t *)p_out) = ucRpsCode;
	usPktLen += sizeof(u8_t);

	if(OAM_MINIMIZE_PDU_SIZE > usPktLen)
	{
		usPktLen = OAM_MINIMIZE_PDU_SIZE;
	}

	OamFrameSend(upgrade_out_data, usPktLen);
}
OPL_STATUS eopl_ctc_end_download_req_handler(u32_t ulFileSize)
{
	u8_t ucRpsCode;
	FILE *pFile;
	TFTP_DATA_t *pstData;
    u8_t bootFlag;
	struct timeval tv;
	struct tm *tm;

	/* OAMDBG(("file size: %d\n", ulFileSize));*/
	OP_DEBUG(DEBUG_LEVEL_INFO, "Receive End Download Request PDU!\n");

	/* check if wait for active request */
	if(g_waitForActive)
	{
		OP_DEBUG(DEBUG_LEVEL_INFO, "Send end download response, write success... \n");
		ucRpsCode = OAM_FIRMWARE_WRITE_SUCCESS;
		eopl_ctc_send_end_download_response(ucRpsCode);
		vosThreadExit(0);
	}

	/* check if flash writing in progress */
	if(g_stImageFile.ucFlash)
	{
		OP_DEBUG(DEBUG_LEVEL_INFO, "Send end download response, file is writing... \n");
		ucRpsCode = OAM_FIRMWARE_WRITING;
		eopl_ctc_send_end_download_response(ucRpsCode);
		vosThreadExit(0);
	}

	/* check file len */
	#if 0
	if(ulFileSize != g_stImageFile.uiFileSize)
	{
		ucRpsCode = OAM_FIRMWARE_VERIFY_ERROR;
		printf("\OAM_FIRMWARE_VERIFY_ERROR!\n");
		goto send_rsp_msg;
	}
	#endif
	/* check crc */

	/* start writing firmware to flash */
	#if 1
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    OP_DEBUG(DEBUG_LEVEL_INFO,"Start writing Flash:%04d.%02d.%02d %02d:%02d:%02d\n", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	#endif
	/* write firmware to flash */
	vosSystem("rm -f /tmp/opconn.img.bak.oam");
	pFile = fopen("/tmp/opconn.img.bak.oam", "wb");

	if(pFile != NULL)
	{
		pstData = g_stImageFile.pstData;
		while(pstData)
		{
			fwrite(pstData->aucDataBlock, 1, pstData->usLen, pFile);
			pstData = pstData->pstNext;
		}
		fclose(pFile);
	}

	/* indicate flash writing finish */

	if (check_image("/tmp/opconn.img.bak.oam") != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_INFO, "Send end download response, verify error... \n");
		ucRpsCode = OAM_FIRMWARE_VERIFY_ERROR;
		eopl_ctc_send_end_download_response(ucRpsCode);

		vosThreadExit(0);
	}

	/* indicate flash writing in progress */
	g_stImageFile.ucFlash = 1;

	/* send end download response with rpsCode = OAM_FIRMWARE_WRITING */
	OP_DEBUG(DEBUG_LEVEL_INFO, "Send end download response, start writing file... \n");
	ucRpsCode = OAM_FIRMWARE_WRITING;
	eopl_ctc_send_end_download_response(ucRpsCode);

    bootFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_NORMAL);
    if (FLASH_BOOT_OS2_FLAG == bootFlag)
    {
		OP_DEBUG(DEBUG_LEVEL_INFO, "Writing image to FLASH_DEV_NAME_OS1!\n");
        write_flash("/tmp/opconn.img.bak.oam", FLASH_DEV_NAME_OS1);
		
        vosSystem("rm -f /tmp/opconn.img.bak.oam ");
    }
    else {
		OP_DEBUG(DEBUG_LEVEL_INFO, "Writing image to FLASH_DEV_NAME_OS2!\n");
        write_flash("/tmp/opconn.img.bak.oam", FLASH_DEV_NAME_OS2);
		
        vosSystem("rm -f /tmp/opconn.img.bak.oam ");
    }

	/* indicate flash writing finish */
	g_stImageFile.ucFlash = 0;
	#if 1
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    OP_DEBUG(DEBUG_LEVEL_INFO, "Finish writing flash:%04d.%02d.%02d %02d:%02d:%02d\n", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	#endif
	ucRpsCode = OAM_FIRMWARE_WRITE_SUCCESS;

	g_waitForActive = 1;

	vosThreadExit(0);

	return 0;
}

OPL_STATUS eopl_ctc_tftp_handler(u8_t *pucData, u16_t msg_len)
{
	u8_t *pucIn;
	u16_t opcode;
	u16_t usDataLen;

	pucIn = pucData;

	opcode = *((u16_t *)pucIn);
	pucIn += sizeof(u16_t);

	switch(opcode)
	{
		case TFTP_R_REQ:
			break;
		case TFTP_W_REQ:
			eopl_ctc_tftp_file_write_req_handler(pucIn, strlen(pucIn));
			break;
		case TFTP_DATA:
			usDataLen = msg_len - sizeof(u16_t);
			eopl_ctc_tftp_data_handler(pucIn, usDataLen);
			break;
		case TFTP_ACK:
			break;
		case TFTP_ERR:
			break;
		default:
			OAMDBG(("\nno such tftp cmd\n"));
			break;
	}

	return 0;
}

OPL_STATUS eopl_ctc_crc_handler(u8_t *pucData)
{
	u8_t *pucIn;
	u16_t usOpcode;
	u32_t ulFileSize;

	pucIn = pucData;
	usOpcode = *((u16_t *)pucIn);
	pucIn += sizeof(u16_t);

	switch(usOpcode)
	{
		case TFTP_END_DOWNLOAD_REQ:
			ulFileSize = *((u32_t *)pucIn);
			/*
			eopl_ctc_end_download_req_handler(ulFileSize);
			*/
#if 1
			vosThreadCreate("tUpgraded", OP_VOS_THREAD_STKSZ, 45,
					(void *)eopl_ctc_end_download_req_handler, (void *)ulFileSize);


#endif
			break;
		case TFTP_END_DOWNLOAD_RSP:
			break;
		default:
			OAMDBG(("\nunknow opcode %d\n", usOpcode));
			break;
	}

	return 0;
}

OPL_STATUS eopl_ctc_load_new_img(void)
{
	OPL_STATUS	iStatus=OPL_OK;
	u8_t *pucPkt;
	ctc_tftp_header_t *pusTftpHdr;
	u16_t usPktLen;
	u8_t ucAck;
	FILE *pFile;
	u8_t ucBootOption;
	u32_t	uiLlid;
	u8_t bootFlag;

	g_waitForActive = 0;
	/* set boot option */
	ucBootOption = 1;
	OP_DEBUG(DEBUG_LEVEL_INFO, "Receive Active Image PDU!\n");

    bootFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_NORMAL);
    if (FLASH_BOOT_OS2_FLAG == bootFlag)
    {
		OP_DEBUG(DEBUG_LEVEL_INFO, "Set FLASH_BOOT_FLAG_NORMAL option to FLASH_BOOT_OS1_FLAG!\n");
		vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS1_FLAG);
    }
    else {
		OP_DEBUG(DEBUG_LEVEL_INFO, "Set FLASH_BOOT_FLAG_NORMAL option to FLASH_BOOT_OS2_FLAG!\n");
		vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS2_FLAG);
    }
	
    /* for FLASH_BOOT_FLAG_UPGRADE, we use FLASH_BOOT_OS2_FLAG to indicate
       that the os upgrade by OAM
    */
	OP_DEBUG(DEBUG_LEVEL_INFO, "Set FLASH_BOOT_FLAG_UPGRADE to FLASH_BOOT_OS2_FLAG!\n");
    vosConfigBootFlagSet(FLASH_BOOT_FLAG_UPGRADE, FLASH_BOOT_OS2_FLAG);

	ucAck = 0;

	memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header(OAM_EXT_CODE_DOWNLOAD);
	pucPkt = &out_data[0];
	usPktLen = 0;

	pucPkt += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	usPktLen += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);

	pusTftpHdr = (ctc_tftp_header_t *)pucPkt;

	pusTftpHdr->data_type = OAM_SOFT_TYPE_LOAD;
	pusTftpHdr->len = sizeof(ctc_tftp_header_t) + sizeof(u16_t) + sizeof(u8_t);

	dalPonLlidGet(&uiLlid);/* zlu modify */
	pusTftpHdr->tid = uiLlid;/* zlu modify */

	pucPkt += sizeof(ctc_tftp_header_t);
	usPktLen += sizeof(ctc_tftp_header_t);

	*((u16_t *)(pucPkt)) = TFTP_ACT_IMG_RSP;
	pucPkt += sizeof(u16_t);
	usPktLen += sizeof(u16_t);

	*((u8_t *)(pucPkt)) = ucAck;
	pucPkt += sizeof(u8_t);
	usPktLen += sizeof(u8_t);

 	if(usPktLen < OAM_MINIMIZE_PDU_SIZE)
 	{
		oam_pdu_padding(pucPkt,  OAM_MINIMIZE_PDU_SIZE - usPktLen);
		usPktLen = OAM_MINIMIZE_PDU_SIZE;
 	}

	OAMDBG(("[ %s ]: pktlen = %d\n", __FUNCTION__, usPktLen));

	OamFrameSend(out_data, usPktLen);
	OAMDBG(("Send Activate Image Response message!\n"));
	OP_DEBUG(DEBUG_LEVEL_INFO, "Send Activate Image Response PDU!\n");

	/* reboot device */
	OP_DEBUG(DEBUG_LEVEL_INFO, "Call odmSysReset() to reboot system!\n");
	odmSysReset();
	OAMDBG(("reboot\n"));

	return iStatus;
}

OPL_STATUS eopl_ctc_image_mark(void)
{
	u8_t					*pucPkt;
	u32_t					uiLlid;
	OPL_STATUS				iStatus=OPL_OK;
	SOFT_DOWNLOAD_HD_MASK_t	*pstDownHd;

	memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header(OAM_EXT_CODE_DOWNLOAD);
	pucPkt = &out_data[0];
	pucPkt += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)pucPkt;
	pstDownHd->ucType = OAM_SOFT_TYPE_LOAD;
	pstDownHd->usLen = 0x08;
	dalPonLlidGet(&uiLlid);/* zlu modify */
	pstDownHd->usTid = uiLlid;/* zlu modify */
	*(u16_t *)(pstDownHd->aucMsg) = TFTP_ACT_IMG_RSP;
	pstDownHd->aucMsg[2] = 0;

	oam_pdu_padding(&out_data[22+pstDownHd->usLen], 42-pstDownHd->usLen);
	OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
	OAMDBG(("Activate Image Response\n"));


	return iStatus;
}

OPL_STATUS eopl_ctc_image_reboot(void)
{
	u8_t					*pucPkt;
	u32_t					uiLlid;
	OPL_STATUS				iStatus=OPL_OK;
	SOFT_DOWNLOAD_HD_MASK_t	*pstDownHd;

	memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header(OAM_EXT_CODE_DOWNLOAD);
	pucPkt = &out_data[0];
	pucPkt += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)pucPkt;
	pstDownHd->ucType = OAM_SOFT_TYPE_LOAD;
	pstDownHd->usLen = 0x08;
	dalPonLlidGet(&uiLlid);/* zlu modify */
	pstDownHd->usTid = uiLlid;/* zlu modify */
	*(u16_t *)(pstDownHd->aucMsg) = TFTP_ACT_IMG_RSP;
	pstDownHd->aucMsg[2] = 0;

	oam_pdu_padding(&out_data[22+pstDownHd->usLen], 42-pstDownHd->usLen);
	OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
	OAMDBG(("reboot acked.\n"));
	return iStatus;
}

OPL_STATUS eopl_ctc_active_req_handler(u8_t *pucData)
{
	u8_t *pucIn;
	u8_t ucFlag;

	pucIn = pucData;

	ucFlag = *((u8_t *)pucIn);

	OPL_STATUS	iStatus=OPL_OK;
	switch(ucFlag)
	{
		case LOAD_NEW_IMG:
			eopl_ctc_load_new_img();
			break;

		default:
			OAMDBG(("unknown active imgage req flag = %d\n", ucFlag));
			break;
	}

	return iStatus;
}

OPL_STATUS eopl_ctc_commit_req_handler(u8_t *pucData)
{
	OPL_STATUS	iStatus=OPL_OK;
	u8_t *pucIn;
	u8_t ucFlag;

	pucIn = pucData;

	ucFlag = *((u8_t *)pucIn);

	switch(ucFlag)
	{
		case COMMIT_NEW_IMG:
			eopl_ctc_commit_new_img();
			break;

		default:
			OAMDBG(("unknown commit imgage req flag = %d\n", ucFlag));
			break;
	}

	return iStatus;
}


OPL_STATUS eopl_ctc_commit_new_img(void)
{
	OPL_STATUS	iStatus=OPL_OK;
	u8_t *pucPkt;
	ctc_tftp_header_t *pusTftpHdr;
	u16_t usPktLen;
	u8_t ucAck, bootFlag;
	u32_t uiLlid;

	/* commit new image as primary bootable image*/

    bootFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_NORMAL);
    if (FLASH_BOOT_OS2_FLAG == bootFlag)
    {
		OP_DEBUG(DEBUG_LEVEL_INFO, "Set FLASH_BOOT_FLAG_NORMAL option to FLASH_BOOT_OS1_FLAG!\n");
		vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS1_FLAG);
    }
    else {
		OP_DEBUG(DEBUG_LEVEL_INFO, "Set FLASH_BOOT_FLAG_NORMAL option to FLASH_BOOT_OS2_FLAG!\n");
		vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS2_FLAG);
    }

	ucAck = 0;

send_rsp_msg:
	OP_DEBUG(DEBUG_LEVEL_INFO, "Send Commit Response PDU!\n");
	memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header(OAM_EXT_CODE_DOWNLOAD);
	pucPkt = &out_data[0];
	usPktLen = 0;

	pucPkt += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	usPktLen += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);

	pusTftpHdr = (ctc_tftp_header_t *)pucPkt;

	pusTftpHdr->data_type = OAM_SOFT_TYPE_COMMIT;
	pusTftpHdr->len = sizeof(ctc_tftp_header_t) + sizeof(u16_t) + sizeof(u8_t);
	dalPonLlidGet(&uiLlid);/* zlu modify */
	pusTftpHdr->tid = uiLlid;/* zlu modify */
	pucPkt += sizeof(ctc_tftp_header_t);
	usPktLen += sizeof(ctc_tftp_header_t);

	*((u16_t *)(pucPkt)) = TFTP_COMMIT_IMG_RSP;
	pucPkt += sizeof(u16_t);
	usPktLen += sizeof(u16_t);

	*((u8_t *)(pucPkt)) = ucAck;
	pucPkt += sizeof(u8_t);
	usPktLen += sizeof(u8_t);

 	if(usPktLen < OAM_MINIMIZE_PDU_SIZE)
 	{
		oam_pdu_padding(pucPkt,  OAM_MINIMIZE_PDU_SIZE - usPktLen);
		usPktLen = OAM_MINIMIZE_PDU_SIZE;
 	}

	OamFrameSend(out_data, usPktLen);
	OAMDBG(("Send Activate Image Response message!\n"));

	return iStatus;
}

OPL_STATUS eopl_ctc_load_handler(u8_t *pucData)
{
	u8_t *pucIn;
	u16_t usOpcode;

	pucIn = pucData;

	usOpcode = *((u16_t *)pucIn);
	pucIn += sizeof(u16_t);

	switch(usOpcode)
	{
		case TFTP_ACT_IMG_REQ:
			eopl_ctc_active_req_handler(pucIn);
			break;
		case TFTP_ACT_IMG_RSP:
			break;

		case TFTP_COMMIT_IMG_REQ:
			eopl_ctc_commit_req_handler(pucIn);
			break;

		case TFTP_COMMIT_IMG_RSP:
			break;

		default:
			OAMDBG(("\nno such tftp cmd\n"));
			break;
	}

	return 0;
}

OPL_STATUS eopl_ctc_commit_handler(u8_t *pucData)
{
	u8_t *pucIn;
	u16_t usOpcode;

	pucIn = pucData;

	usOpcode = *((u16_t *)pucIn);
	pucIn += sizeof(u16_t);

	switch(usOpcode)
	{
		case TFTP_COMMIT_IMG_REQ:
			eopl_ctc_commit_req_handler(pucIn);
			break;
	}

	return 0;
}

OPL_STATUS eopl_ctc_soft_download(u8_t *pucData)
{
	u8_t *pucIn;
	ctc_tftp_header_t *p_tftp_header;
	u16_t usMsgLen;

	pucIn = pucData;

	p_tftp_header = (ctc_tftp_header_t *)pucIn;
	pucIn += sizeof(ctc_tftp_header_t);

	usMsgLen = (p_tftp_header->len) - sizeof(ctc_tftp_header_t);

	switch(p_tftp_header->data_type)
	{
		case OAM_SOFT_TYPE_TFTP:
			eopl_ctc_tftp_handler(pucIn, usMsgLen);
			break;
		case OAM_SOFT_TYPE_CRC:
			eopl_ctc_crc_handler(pucIn);
			break;
		case OAM_SOFT_TYPE_LOAD:
			eopl_ctc_load_handler(pucIn);
			break;
		case OAM_SOFT_TYPE_COMMIT:
			eopl_ctc_commit_handler(pucIn);
			break;
		default:
			return 0;
	}

	return 0;
}

void eopl_ctc_pdu_process(u8_t *p_byte,u16_t length)
{
	ctc_header_t	*p_ctc;
	oampdu_hdr_t	*p_hdr;
	UINT8 enable = FALSE;

	p_hdr = (oampdu_hdr_t*)p_byte;
	p_byte += sizeof(oampdu_hdr_t);

	p_ctc = (ctc_header_t*)p_byte;
	p_byte += sizeof(ctc_header_t);

#ifdef OPL_COM_UART1
    odmOnuUartCommEnableGet(&enable);
#endif
	switch(p_ctc->ext_code)
	{
		case OAM_EXT_CODE_GET_REQUEST:
#ifdef OPL_COM_UART1
		    if (TRUE == enable)
		    {
    		    if (TRUE == oamtransmitevent)
                    OnuOamTransmit(0,p_ctc->ext_code,p_byte,(length-sizeof(oampdu_hdr_t)-1));
            }
            else
#endif
            {
			    eopl_ctc_get_req(p_byte);
			}
	  	    break;
		case OAM_EXT_CODE_SET_REQUEST:
#ifdef OPL_COM_UART1
			if (TRUE == enable)
		    {
		    	if (TRUE == oamtransmitevent)
                	OnuOamTransmit(0,p_ctc->ext_code,p_byte,(length-sizeof(oampdu_hdr_t)-1));
			}
			else
#endif
			{
			    eopl_ctc_set_req(p_byte);
			}
			break;
		case OAM_EXT_CODE_AUTHENTICATION:
#ifdef OPL_COM_UART1
			if (TRUE == enable)
			{
				if(TRUE == oamtransmitevent)
                	OnuOamTransmit(0,p_ctc->ext_code,p_byte,(length-sizeof(oampdu_hdr_t)-1));
			}
			else
#endif
			{
				eopl_ctc_auth(p_byte);
			}
	  	    break;
		case OAM_EXT_CODE_DOWNLOAD:
			eopl_ctc_soft_download(p_byte);
			break;
		case OAM_EXT_CODE_CHURNING:
			eopl_ctc_churning(p_byte);
			break;
		case OAM_EXT_CODE_DBA:
			eopl_ctc_dba(p_byte);
			break;
		case OAM_OPL_OPCODE_AES:
			eopl_aeskey_req(p_byte);
			break;
		case OAM_OPL_OPCODE_AES_SCB:
			eopl_aesscbkey_req(p_byte);
			break;
		case OAM_OPL_OPCODE_TRI_SCB:
			eopl_TriScbkey_req(p_byte);
			break;
		default:
			break;
	}
}

#if 0
int stub_get_onu_alarm_admin_state(u16_t alarm_id, CTC_ALARM_SRC_t alarm_src, u32_t *alarm_state)
{
	u8_t port_id;
	u8_t i;
	int ret;

	ret = -1;

	switch(alarm_src.obj_type)
	{
	case OBJ_ONU:
		for(i = 0; i < ONU_ALARM_NUM; i++)
		{
			if(alarm_id == stub_onu_alarm_config[i].alarm_id)
			{
				*alarm_state = stub_onu_alarm_config[i].enable_status;
				ret = 0;
				break;
			}
		}
		break;
	case OBJ_UNI:
		port_id = alarm_src.para.uni_id.port_id;
		for(i = 0; i < UNI_ALARM_NUM; i++)
		{
			if(alarm_id == stub_eth_port_alarm_config[port_id][i].alarm_id)
			{
				*alarm_state = stub_eth_port_alarm_config[port_id][i].enable_status;
				ret = 0;
				break;
			}
		}
		break;
	case OBJ_CARD:
		for(i = 0; i < CARD_ALARM_NUM; i++)
		{
			if(alarm_id == stub_card_alarm_config[i].alarm_id)
			{
				*alarm_state = stub_card_alarm_config[i].enable_status;
				ret = 0;
				break;
			}
		}
		break;
	case OBJ_LLID:
		break;

	case OBJ_PON:
		for(i = 0; i < PON_ALARM_NUM; i++)
		{
			if(alarm_id == stub_pon_alarm_config[i].alarm_id)
			{
				*alarm_state = stub_pon_alarm_config[i].enable_status;
				ret = 0;
				break;
			}
		}
		break;
	default:
		break;
	}

	return ret;
}

#endif

/*modify by lizhe 2010.1.4*/
//int eopl_ctc_alarm_report(CTC_OAM_ALARM_ID_e alarm_id, CTC_ALARM_SRC_t alarm_src, u8_t *p_info, u8_t info_len)
int eopl_ctc_alarm_report(alarm_report_t *reportInfo)
{
	u8_t *p_byte;
	u16_t obj_type;
	u32_t instance_num;
	ctc_event_hdr_t *p_ctc_event_hdr;
	ctc_alarm_entry_hdr_t *p_ctc_alarm_entry;
	int ret;

	#if 0
	/* check if alarm is enable */
	ret = stub_get_onu_alarm_admin_state(alarm_id, alarm_src, &alarm_state);

	if(ret != 0)
	{
		return ret;
	}

	/* alarm is disabled */
	if(alarm_state == CTC_ALARM_DISABLE)
	{
		ret = -1;
		return ret;
	}
	#endif
	
	/* alarm is enabled */
	if(0 == g_usAlarmOutDataIdx)
	{
		memset(alarm_out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		p_byte = &alarm_out_data[0];

		eopl_oam_pdu_hdr(p_byte, oams.flags, OAM_CODE_EVENT);
		p_byte += sizeof(oampdu_hdr_t);
		g_usAlarmOutDataIdx += sizeof(oampdu_hdr_t);
	}
	else
	{
		p_byte = &alarm_out_data[g_usAlarmOutDataIdx];
	}

	p_ctc_event_hdr = (ctc_event_hdr_t *)p_byte;
	p_ctc_event_hdr->seq_num = g_alarm_seq_no++;
	p_ctc_event_hdr->event_type = 0xFE;
	p_ctc_event_hdr->oui[0] = g_aucCtcOui[0];
	p_ctc_event_hdr->oui[1] = g_aucCtcOui[1];
	p_ctc_event_hdr->oui[2] = g_aucCtcOui[2];

	//p_ctc_event_hdr->event_len = sizeof(ctc_alarm_entry_hdr_t) + reportInfo->length;
	p_ctc_event_hdr->event_len = sizeof(ctc_alarm_entry_hdr_t) + sizeof(ctc_event_hdr_t) -2 + reportInfo->length;
	p_byte += sizeof(ctc_event_hdr_t);
	g_usAlarmOutDataIdx += sizeof(ctc_event_hdr_t);

	p_ctc_alarm_entry = (ctc_alarm_entry_hdr_t *)p_byte;

	#if 0
	switch(alarm_src.obj_type)
	{
	case OBJ_ONU:
		p_ctc_alarm_entry->obj_type = 0xFFFF;
		p_ctc_alarm_entry->instance_num = 0xFFFFFFFF;
		break;
	case OBJ_UNI:
		p_ctc_alarm_entry->obj_type = OAM_CTC_OBJ_UNI;
		instance_num = OAM_SET_PORT_TYPE(instance_num, alarm_src.para.uni_id.port_type);
		instance_num = OAM_SET_PORT_FRAME_ID(instance_num, alarm_src.para.uni_id.frame_id);
		instance_num = OAM_SET_PORT_SLOT_ID(instance_num, alarm_src.para.uni_id.slot_id);
		instance_num = OAM_SET_PORT_NUM(instance_num, alarm_src.para.uni_id.port_id);
		p_ctc_alarm_entry->instance_num = instance_num;
		break;
	case OBJ_CARD:
		p_ctc_alarm_entry->obj_type = OAM_CTC_OBJ_CARD;
		p_ctc_alarm_entry->instance_num = alarm_src.para.card_id;
		break;
	case OBJ_LLID:
		p_ctc_alarm_entry->obj_type = OAM_CTC_OBJ_LLID;
		p_ctc_alarm_entry->instance_num = alarm_src.para.llid;
		break;
	case OBJ_PON:
		p_ctc_alarm_entry->obj_type = OAM_CTC_OBJ_PON;
		p_ctc_alarm_entry->instance_num = alarm_src.para.pon_id;
		break;
	default:
		break;
	}
	#else
		p_ctc_alarm_entry->obj_type = reportInfo->objectType;
		p_ctc_alarm_entry->instance_num = reportInfo->instanceNum;
	#endif
	
	p_ctc_alarm_entry->alarm_id = reportInfo->alarmID;

	//	p_ctc_alarm_entry->alarm_state = 0x00;
	p_ctc_alarm_entry->timeStamp = 0;
	/*梓袧載蜊綴�◎�*/
	p_ctc_alarm_entry->alarm_state = reportInfo->alarmState;
	g_usAlarmOutDataIdx += sizeof(ctc_alarm_entry_hdr_t);

	if(reportInfo->length > 0)
	{
		p_byte += sizeof(ctc_alarm_entry_hdr_t);
		memcpy(p_byte, reportInfo->alarmInfo, reportInfo->length);
		g_usAlarmOutDataIdx += reportInfo->length;
	}

	ret = 0;

	if(g_usAlarmOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
	{
		oam_pdu_padding(&alarm_out_data[g_usAlarmOutDataIdx],	OAM_MINIMIZE_PDU_SIZE - g_usAlarmOutDataIdx);
		g_usAlarmOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
	}
	OamFrameSend(alarm_out_data, g_usAlarmOutDataIdx);
	g_usAlarmOutDataIdx = 0;

	return ret;
}

#if 0
int eopl_ctc_alarm_clear(CTC_OAM_ALARM_ID_e alarm_id, CTC_ALARM_SRC_t alarm_src, u8_t *p_info, u8_t info_len)
{
	u8_t *p_byte;
	u16_t obj_type;
	u32_t instance_num;
	ctc_event_hdr_t *p_ctc_event_hdr;
	ctc_alarm_entry_hdr_t *p_ctc_alarm_entry;
	u32_t alarm_state;
	int ret;

	/* check if alarm is enable */
	ret = stub_get_onu_alarm_admin_state(alarm_id, alarm_src, &alarm_state);
	if(ret != 0)
	{
		return ret;
	}

	/* alarm is disabled */
	if(alarm_state == 0)
	{
		ret = -1;
		return ret;
	}

	/* alarm is enabled */
	if(0 == g_usAlarmOutDataIdx)
	{
		memset(alarm_out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		p_byte = &alarm_out_data[0];

		eopl_oam_pdu_hdr(p_byte, oams.flags, OAM_CODE_ORG_SPEC);
		p_byte += sizeof(oampdu_hdr_t);
		g_usAlarmOutDataIdx += sizeof(oampdu_hdr_t);
	}
	else
	{
		p_byte = &alarm_out_data[g_usAlarmOutDataIdx];
	}

	p_ctc_event_hdr = (ctc_event_hdr_t *)p_byte;
	p_ctc_event_hdr->seq_num = g_alarm_seq_no++;
	p_ctc_event_hdr->event_type = 0xFE;
	p_ctc_event_hdr->oui[0] = 0x11;
	p_ctc_event_hdr->oui[1] = 0x11;
	p_ctc_event_hdr->oui[2] = 0x11;

	p_ctc_event_hdr->event_len = sizeof(ctc_alarm_entry_hdr_t) + info_len;
	p_byte += sizeof(ctc_event_hdr_t);
	g_usAlarmOutDataIdx += sizeof(ctc_event_hdr_t);

	p_ctc_alarm_entry = (ctc_alarm_entry_hdr_t *)p_byte;

	switch(alarm_src.obj_type)
	{
	case OBJ_ONU:
		p_ctc_alarm_entry->obj_type = 0xFFFF;
		p_ctc_alarm_entry->instance_num = 0xFFFFFFFF;
		break;
	case OBJ_UNI:
		p_ctc_alarm_entry->obj_type = OAM_CTC_OBJ_UNI;
		instance_num = OAM_SET_PORT_TYPE(instance_num, alarm_src.para.uni_id.port_type);
		instance_num = OAM_SET_PORT_FRAME_ID(instance_num, alarm_src.para.uni_id.frame_id);
		instance_num = OAM_SET_PORT_SLOT_ID(instance_num, alarm_src.para.uni_id.slot_id);
		instance_num = OAM_SET_PORT_NUM(instance_num, alarm_src.para.uni_id.port_id);
		p_ctc_alarm_entry->instance_num = instance_num;
		break;
	case OBJ_CARD:
		p_ctc_alarm_entry->obj_type = OAM_CTC_OBJ_CARD;
		p_ctc_alarm_entry->instance_num = alarm_src.para.card_id;
		break;
	case OBJ_LLID:
		p_ctc_alarm_entry->obj_type = OAM_CTC_OBJ_LLID;
		p_ctc_alarm_entry->instance_num = alarm_src.para.llid;
		break;
	case OBJ_PON:
		p_ctc_alarm_entry->obj_type = OAM_CTC_OBJ_PON;
		p_ctc_alarm_entry->instance_num = alarm_src.para.pon_id;
		break;
	default:
		break;
	}

	ret = 0;

	p_ctc_alarm_entry->alarm_id = (u16_t)alarm_id;

	p_ctc_alarm_entry->alarm_state = 0x01;
	g_usAlarmOutDataIdx += sizeof(ctc_alarm_entry_hdr_t);

	if(info_len > 0)
	{
		p_byte += sizeof(ctc_alarm_entry_hdr_t);
		memcpy(p_byte, p_info, info_len);
		g_usAlarmOutDataIdx += info_len;
	}

	if(g_usAlarmOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
	{
		oam_pdu_padding(&alarm_out_data[g_usAlarmOutDataIdx],  OAM_MINIMIZE_PDU_SIZE - g_usAlarmOutDataIdx);
		g_usAlarmOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
	}
	OamFrameSend(alarm_out_data, g_usAlarmOutDataIdx);
	g_usAlarmOutDataIdx = 0;

	return 0;
}
#endif
