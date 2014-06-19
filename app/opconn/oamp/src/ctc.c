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
#include "ctc.h"
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
#include "odm_fdb.h"
#include "system.h"
#include "version.h"
#include "qos.h"
#include "stats.h"
#include "mc_control.h"
#if 0/*wfxu 0401*/
#include "dal_multicast.h"
#include "dal_vtt.h"
#include "dal_lib.h"
#endif
#include "opconn_usr_ioctrl.h"

#include "zte.h"
#include "oam_lw.h"

#define MODULE MOD_OAM
extern u8_t alarm_out_data[OAM_MAXIMUM_PDU_SIZE+4];
extern u8_t	out_data[OAM_MAXIMUM_PDU_SIZE+4];
extern u8_t	in_data[OAM_MAXIMUM_PDU_SIZE+4];

extern u8_t	oam_oui[3];
extern u8_t	oam_onu_vendor[4];
extern u8_t	oam_chip_vendor[2];
extern u8_t	oam_onu_model[4];
extern u8_t	oam_chip_model[2];
extern u8_t	oam_chip_revision;
extern u8_t	oam_src_mac[6];
extern u8_t	oam_firm_ver[2];
extern u8_t	oam_onu_hwarever[8];
extern u8_t	oam_onu_swarever[16];
extern u8_t	oam_chip_revision;
extern u8_t	oam_chip_version[3];
extern UINT8 OamClsCount;


extern int CTC_DRV_SetTransparentVlan(u32_t uiLPortId);
extern int CTC_DRV_SetTagVlan(u32_t uiLPortId, u32_t uiPvid, u32_t uiPriority);
extern int  CTC_DRV_GetVlanIdAndPri(u32_t uiVlanTag, u32_t *puiVlanId, u32_t *puiPri);


oam_instance_tlv_t_2_0	port_instance_2_0;
TFTP_FILE_t g_stImageFile_2_0;
extern oam_sate_t oams;

u8_t g_aucCtcOui[3]={0x11, 0x11, 0x11};
u16_t g_usOutDataIdx;
u16_t g_usAlarmOutDataIdx;
int g_iOamSetDbg=0;

u8_t *gpucOlid="opulan";
u8_t *gpucPassWd="opulan";

/* EricYang add for Trace */
 #define  E_T /* printf("~~~~~    enter %s ~~~~~~~~\n",__FUNCTION__) */
void OamDbgSwitch (int iEndis)
{
	g_iOamSetDbg = iEndis;
}

void eopl_ctc_header_2_0(u8_t ext_code)
{
	u8_t	*p_byte;
	ctc_header_t	*p_ctc;

	p_byte = &out_data[0];
	eopl_oam_pdu_hdr(p_byte, oams.flags, OAM_CODE_ORG_SPEC);
	p_byte += sizeof(struct oampdu_hdr);
	p_ctc = (ctc_header_t*)p_byte;
	vosMemCpy(p_ctc->oui, g_aucCtcOui, 3);
	p_ctc->ext_code = ext_code;
}

/* EricYang add for the Ev_Reg_CalllBack */
static u8_t * eopl_get_output_buf_2_0(u8_t ext_code)
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

int oamLSCAlignmentStrGet_2_0(UINT8 *pucSrc, UINT16 usSrcSize, char *pcDst, UINT16 usDstSize)
{
    UINT8 *pucResult;
    char *pcStr;

    if (NULL == pucSrc || NULL == pcDst)
        return ERROR;

    pucResult = (UINT8 *)memrchr(pucSrc, 0, usSrcSize);
    if(pucResult == NULL)
    {
	    vosSafeStrNCpy(pcDst, pucSrc, usDstSize);
    }
    else
    {
	    pcStr = (char *)(pucResult + 1);
	    vosSafeStrNCpy(pcDst, pcStr, usDstSize);
    }
    return OK;
}

int SwVersionGet_2_0(u8_t *pucVerStr)
{
	int		iStatus=0;
	u8_t	ucCnt, ucCnt1=0;
	u8_t	ucVer;
	u8_t	aucVer[SUBVERSTR_LEN];

	if(0 == pucVerStr)
	{
		return -1;
	}

#if 0
	memset(pucVerStr, 0, SUBVERSTR_LEN);
	memcpy(&pucVerStr[SUBVERSTR_LEN -vosStrLen(MAJOR_VERSION)], MAJOR_VERSION, vosStrLen(MAJOR_VERSION) );
#endif
#if 0
	if (SUBVERSTR_LEN > vosStrLen(MAJOR_VERSION) )
	{
		memcpy(pucVerStr, MAJOR_VERSION, vosStrLen(MAJOR_VERSION) );
		pucVerStr[vosStrLen(MAJOR_VERSION) ] = 0;
	}
	else {
		memcpy(pucVerStr, MAJOR_VERSION, SUBVERSTR_LEN);
	}
#endif
	#if 1
	if((9<MAJOR_VERSION) || (9<MINOR_VERSION) || (100<BUILD_NUMBER))
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
	#endif
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
void eopl_get_onusn_rsp_2_0(void)
{
	int				iStatus=0;
	u8_t			*p_byte;
	oam_onu_sn_t	*p_sn;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
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
	iStatus = SwVersionGet_2_0(oam_onu_swarever);
	if(0 != iStatus)
	{
		memset(oam_onu_swarever, 0, 16);
		printf("software version get error.\n");
	}
	#else
	vosSprintf((CHAR *)oam_onu_swarever, "%s", CONFIG_RELEASE_VER);
	#endif
	vosMemCpy(p_sn->sware_ver,oam_onu_swarever, 16);
	g_usOutDataIdx += p_sn->width + 4;
}

/*** checked ok ***/
void eopl_get_onu_firmwarever_2_0(void)
{
	u8_t	*p_byte;
	oam_fware_ver_t	*p_fver;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
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
	g_usOutDataIdx += p_fver->width + 4;
}

/*** checked ok ***/
void eopl_get_onu_chipid_2_0(void)
{
	u8_t	*p_byte;
	oam_chipset_id_t	*p_chip;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
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
	g_usOutDataIdx += p_chip->width + 4;
}

/*** checked ok ***/
void eopl_get_onu_capabilities_2_0(void)
{
	u8_t	*p_byte;
	oam_onu_capabilites_t	*p_cap;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct ctc_header);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

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
    #if defined(CONFIG_PRODUCT_EPN200)
	p_cap->serv_supported = 0x01;
	p_cap->number_GE = 1;
	vosMemSet(p_cap->bitmap_GE, 0, 8);
    p_cap->bitmap_GE[7] = 0x01;
	p_cap->number_FE = 0;
	vosMemSet(p_cap->bitmap_FE, 0, 8);
    #else
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
    #endif
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
	g_usOutDataIdx += p_cap->width + 4;
}

/*** checked ok ***/
void eopl_get_onu_eth_linkstate_2_0(void)
{
	u8_t rlt=0, ucPortS, ucPortE, *p_byte;
	oam_eth_linkstate_t	*p_link;
	oam_instance_tlv_t_2_0	*p_inst;
    u32_t state=0;
    BOOL bPortStatus = TRUE;
	
	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(struct oampdu_hdr) + sizeof(struct ctc_header);
			g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct ctc_header);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst = port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
		p_link = (oam_eth_linkstate_t*)p_byte;
		p_link->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_link->leaf = OAM_ATTR_ETH_LINKSTATE;
		p_link->width = 0x1;
		#if 0
		rlt = odmPortLinkStateGet(ucPortS, &state);
		if(rlt != OPL_OK)
		{
			printf("link %d state get error\n", ucPortS);
			continue;
		}
		#else
        if ( NO_ERROR == DRV_GetPortLinkStatus((UINT32)ucPortS, &bPortStatus))
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
		p_link->linkstate = (u8_t)state;
		g_usOutDataIdx += p_link->width + 9;
	}
}

/*** checked ok ***/
void eopl_get_onu_eth_portpause_2_0(void)
{
	u8_t	rlt=0, ucPortS, ucPortE, *p_byte;
	u32_t	enable=1;
	oam_eth_portpause_t	*p_pause;
	oam_instance_tlv_t_2_0	*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(struct oampdu_hdr) + sizeof(struct ctc_header);
			g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct ctc_header);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst =   port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
		p_pause = (oam_eth_portpause_t*)p_byte;
		p_pause->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_pause->leaf = OAM_ATTR_ETH_PORTPAUSE;
		p_pause->width = 1;
		rlt = odmPortFlowCtrolEnableGet(ucPortS, &enable);
		if(rlt != OPL_OK)
			return;
		p_pause->back_oper = enable;
		g_usOutDataIdx += p_pause->width + 9;
	}
}

void eopl_get_onu_eth_portpolicing_2_0(void)
{
	u8_t	enable=1, rlt=0, ucPortS, ucPortE, *p_byte, *p_temp;
	u32_t	cir=0xFF,cbs=0xFF,ebs=0xFF;
	oam_eth_portpolicing_t	*p_policing;
	oam_instance_tlv_t_2_0		*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
		return;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		port_instance_2_0.value = ucPortS;
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(struct oampdu_hdr) + sizeof(struct ctc_header);
			g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct ctc_header);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst = port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
		p_policing = (oam_eth_portpolicing_t*)p_byte;
		p_policing->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_policing->leaf = OAM_ATTR_ETH_PORT_US_POLICING;
		rlt = odmPortUsPolicingGet(ucPortS, &enable,&cir,&cbs,&ebs);
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
		g_usOutDataIdx += p_policing->width + 9;
	}
}

void eopl_get_onu_eth_portdspolicing_2_0(void)
{
	u8_t	enable=1,rlt=0, ucPortS, ucPortE, *p_byte,*p_temp;
	u32_t	cir=0xFF,pir=0xFF;
	oam_eth_portdspolicing_t	*p_policing;
	oam_instance_tlv_t_2_0			*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		port_instance_2_0.value = ucPortS;
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
			g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst = port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
		p_policing = (oam_eth_portdspolicing_t*)p_byte;
		p_policing->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_policing->leaf = OAM_ATTR_ETH_PORT_DS_POLICING;
		rlt = odmPortDsPolicingGet(ucPortS, &enable,&cir,&pir);
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
		g_usOutDataIdx += p_policing->width + 9;
	}
}

/*** checked ok ***/
void eopl_get_onu_eth_phyadmin_2_0(void)
{
	u8_t	rlt=0, ucPortS, ucPortE, *p_byte;
	u32_t	state=0x00000002;
	oam_phy_adminstate_t	*p_admin;
	oam_instance_tlv_t_2_0		*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}
	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(struct oampdu_hdr) + sizeof(struct ctc_header);
			g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct ctc_header);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst =   port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
		p_admin = (oam_phy_adminstate_t*)p_byte;
		p_admin->branch = OAM_BRANCH_STANDARD_ATTRIB;
		p_admin->leaf = OAM_ATTR_PHY_ADMIN;
		p_admin->width = 4;

		rlt = odmPortAdminGet(ucPortS, &state);
		if(rlt != OPL_OK)
		{
			continue;
		}
		if(0 == state)
		{
			p_admin->value = 1;
		}
		else if(1 == state)
		{
			p_admin->value = 2;
		}
		else
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\r\nphyadmin state value get error\r\n");
		}

		g_usOutDataIdx += p_admin->width + 9;
	}
}

/*** checked ok ***/
void eopl_get_onu_eth_autonegadmin_2_0(void)
{
	u8_t	rlt=0, ucPortS, ucPortE, *p_byte;
	u32_t	state=0x00000002;
	oam_autoneg_adminstate_t	*p_admin;
	oam_instance_tlv_t_2_0			*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
			g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst =   port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
		p_admin = (oam_autoneg_adminstate_t*)p_byte;
		p_admin->branch = OAM_BRANCH_STANDARD_ATTRIB;
		p_admin->leaf = OAM_ATTR_AUTONEG_ADMIN_STATE;
		p_admin->width = 4;
		#if 0
		rlt = odmPortAutoEnableGet(ucPortS, &state);		
		if(rlt != OPL_OK)
		{
			continue;
		}
		#else
		state = (u32_t)m_abAutoNegEnable[ucPortS];
		#endif
		if(0 == state)
		{
			p_admin->value = 1;
		}
		else if(1 == state)
		{
			p_admin->value = 2;
		}
		else
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\r\nautoneg state value get error\r\n");
		}

		g_usOutDataIdx += p_admin->width + 9;
	}
}

/*** checked ok ***/
void eopl_get_onu_eth_autoneglocal_2_0(void)
{
	u8_t	ucPortS, ucPortE, *p_byte;
	oam_autoneg_localability_t	*p_local;
	oam_instance_tlv_t_2_0			*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
			g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst =   port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
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
		g_usOutDataIdx += p_local->width + 9;
	}
}

/*** checked ok ***/
void eopl_get_onu_eth_autonegadv_2_0(void)
{
	u8_t	ucPortS, ucPortE, *p_byte;
	oam_autoneg_advability_t	*p_adv;
	oam_instance_tlv_t_2_0			*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
			g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst =   port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
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
		g_usOutDataIdx += p_adv->width + 9;
	}
}

void eopl_get_onu_eth_fecability_2_0(void)
{
	u8_t	*p_byte;
	oam_fec_capability_t	*p_fec;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
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

	g_usOutDataIdx += p_fec->width + 4;
}

void eopl_get_onu_eth_fecmode_2_0(void)
{
	u32_t	mode=0;
	u8_t	rlt=0, *p_byte;
	oam_fec_mode_t	*p_fec;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_fec = (oam_fec_mode_t*)p_byte;
	p_fec->branch = OAM_BRANCH_STANDARD_ATTRIB;
	p_fec->leaf = OAM_ATTR_FEC_MODE;
	p_fec->width = 4;
	rlt = odmPonFecEnableGet(&mode);
	if(rlt != OPL_OK)
	{
		return;
	}

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

	g_usOutDataIdx += p_fec->width + 4;
}

void eopl_get_onu_mcast_tagstrip_2_0(void)
{
	u8_t	ucPortS, ucPortE, *p_byte;
	oam_mcast_tagstripe_t	*p_tagstripe;
	oam_instance_tlv_t_2_0		*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		port_instance_2_0.value = ucPortS;
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
			g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst =   port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
		p_tagstripe = (oam_mcast_tagstripe_t*)p_byte;
		p_tagstripe->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_tagstripe->leaf = OAM_ATTR_MCAST_TAG_STRIP;
		p_tagstripe->width = 1;
		odmMulticastTagstripGet(ucPortS, &(p_tagstripe->tagstriped));
		g_usOutDataIdx += p_tagstripe->width + 9;
	}
}

void eopl_get_onu_mcast_switch_2_0(void)
{
	u8_t	*p_byte;
	oam_mcast_switch_t	*p_switch;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
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
	g_usOutDataIdx += p_switch->width + 4;
}

void eopl_get_onu_grp_nummax_2_0(void)
{
	u8_t	ucPortS, ucPortE, *p_byte;
	oam_group_num_max_t	*p_group;
	oam_instance_tlv_t_2_0	*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		port_instance_2_0.value = ucPortS;
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
			g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst =   port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
		p_group = (oam_group_num_max_t*)p_byte;
		p_group->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_group->leaf = OAM_ATTR_MCAST_GRP_NUMMAX;
		p_group->width = 1;
		p_group->num_max = odmMulticastGroupMaxNumGet(ucPortS);
		g_usOutDataIdx += p_group->width + 9;
	}
}

void eopl_get_onu_fastleave_abl_2_0(void)
{
	u32_t	*puiMode;
	u8_t	ucIdx, *p_byte;
	oam_fastleave_abl_t	*p_fast;

	if(0 == g_usOutDataIdx)
	{
		vosMemSet(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_fast = (oam_fastleave_abl_t*)p_byte;
	p_fast->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_fast->leaf = OAM_ATTR_MCAST_GFASTLEAVE_ABL;
	p_fast->width = 20;
	p_fast->num_of_mode = 4;
	p_fast->mode_1 = OAM_CTC_IGMP_SNOOPING_NOT_FAST_LEAVE;
	p_fast->mode_2 = OAM_CTC_IGMP_SNOOPING_FAST_LEAVE;
	p_fast->mode_3 = OAM_CTC_IGMP_CTC_NOT_FAST_LEAVE;
	p_fast->mode_4 = OAM_CTC_IGMP_CTC_FAST_LEAVE;

	g_usOutDataIdx += p_fast->width + 4;
}

void eopl_get_onu_fastleave_admin_2_0(void)
{
	u8_t	rlt=0, *p_byte;
	oam_fastleave_adm_t	*p_fast;

	if(0 == g_usOutDataIdx)
	{
		vosMemSet(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
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

	p_byte += sizeof(struct oam_fastleave_adm);

	g_usOutDataIdx += p_fast->width + 4;
}

void eopl_get_onu_mcast_vlan_2_0(void)
{
	u8_t	rlt=0, ucPortS, ucPortE, number, *p_byte;
	u16_t	*p_vid;
	oam_mcast_vlan_t	*p_mcast_vlan;
	oam_instance_tlv_t_2_0	*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		port_instance_2_0.value = ucPortS;
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
			g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}

		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst =   port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
		p_mcast_vlan = (oam_mcast_vlan_t*)p_byte;
		p_mcast_vlan->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_mcast_vlan->leaf = OAM_ATTR_MCAST_VLAN;
		p_mcast_vlan->operation = 0x03;
		p_byte += sizeof(struct  oam_mcast_vlan);
		p_vid = (u16_t*)p_byte;
		rlt = odmMulticastVlanGet(ucPortS, p_vid, &number);
		if(0 != rlt)
		{
			number = 0;
		}

		p_mcast_vlan->width = 1+2*number;

		g_usOutDataIdx += p_mcast_vlan->width + 9;
	}
}

void eopl_get_onu_mcast_control_2_0(void)
{
	u8_t *p_byte;
	u16_t number;
	oam_mcast_control_t			*p_mcast_control;
	oam_mcast_control_entry_t	*p_mcast_entry_src=0,*p_mcast_entry_dst;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_mcast_control = (oam_mcast_control_t*)p_byte;
	p_mcast_control->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_mcast_control->leaf = OAM_ATTR_MCAST_CONTROL;
	p_mcast_control->control_type = (u8_t)odmMulticastControlTypeGet();
	p_mcast_control->action = 3;
	p_byte += sizeof(struct  oam_mcast_control);
	p_mcast_entry_src = (oam_mcast_control_entry_t*)p_byte;


	#if 0
	odmMulticastControlEntryGet((multicast_control_entry_t *)p_mcast_entry_src, &number);

	if(!p_mcast_entry_src)
	{
		number = 0;
	}
	else if(number)
	{
		vosMemCpy((u8_t*)p_mcast_entry_dst,(u8_t*)p_mcast_entry_src, number*sizeof(struct oam_mcast_control_entry));
	}

	p_mcast_control->width = 3+10*number;
	p_mcast_control->number = number;

	g_usOutDataIdx += p_mcast_control->width + 4 + p_mcast_control->number *sizeof(struct oam_mcast_control_entry);
	#endif

	p_mcast_entry_src = (oam_mcast_control_entry_t*)p_byte;
	odmMulticastControlEntryGet((multicast_control_entry_t *)p_mcast_entry_src, &number);
	p_mcast_control->width = 3+10*number;
	p_mcast_control->number = number;

	g_usOutDataIdx += (sizeof(oam_mcast_control_t) + sizeof(oam_mcast_control_entry_t) * number);
}

void eopl_set_dba_2_0(u8_t ucQSetNum, oam_dba_queue_t *p_dba)
{
	u8_t	ucIdx, size, ucQId, *p_byte;
	int		rlt=0;
	oam_dba_queue_t	*p_tmp;

	eopl_ctc_header_2_0(OAM_EXT_CODE_DBA);
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
		oam_pdu_padding(p_byte, OAM_MINIMIZE_PDU_SIZE - size);
		size = OAM_MINIMIZE_PDU_SIZE;
	}
#endif
	OamFrameSend(out_data, size);
}

void eopl_get_dba_2_0(void)
{
	u8_t	size,rlt=0, *p_byte;
	u16_t	value=0xFF;
	u32_t	uiIdx=0;
	oam_dba_queue_t	*p_dba;
    OAM_TRACE();

	eopl_ctc_header_2_0(OAM_EXT_CODE_DBA);
	p_byte = &out_data[0];
	p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	*p_byte++ = OAM_GET_DBA_RESPONSE;
	*p_byte++ = 3;
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

	p_byte += 3*sizeof(struct oam_dba_queue);

	size = sizeof(struct oampdu_hdr) + sizeof(struct ctc_header)
	       			      + 2 + 3*sizeof(struct oam_dba_queue);

 	if(size < OAM_MINIMIZE_PDU_SIZE)
 	{
		oam_pdu_padding(p_byte,  OAM_MINIMIZE_PDU_SIZE - size);
		size = OAM_MINIMIZE_PDU_SIZE;
 	}

	OamFrameSend(out_data, size);
}

bool_t eopl_get_tag_id_2_0(u8_t portid,u32_t *value)
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

bool_t eopl_get_vtt_list_2_0(u8_t portid,u8_t *p_byte,u8_t *pNum, u32_t uiDefVid)
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

int eopl_get_onu_vlan_2_0(void)
{
	u8_t	number, rlt=0, ucPortS, ucPortE, *p_byte;
	u32_t	tag, mode=1;
	oam_vlan_config_t	*p_vlan_config;
	oam_instance_tlv_t_2_0	*p_inst;
	u32_t length;
	u8_t vlanmode;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return -1;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		port_instance_2_0.value = ucPortS;
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
			g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}
		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst =   port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
		p_vlan_config = (oam_vlan_config_t*)p_byte;
		p_vlan_config->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_vlan_config->leaf = OAM_ATTR_VLAN;

		#if defined(CONFIG_PRODUCT_EPN200)
		rlt = odmPortVlanModeGet(port_instance_2_0.value, &mode);
		if(rlt != OPL_OK)
		{
			return -1;
		}
		p_vlan_config->mode = mode;

		p_byte += sizeof(struct  oam_vlan_config);

		if(OAM_CTC_VLAN_TRANSPARENT == p_vlan_config->mode)
		{
			p_vlan_config->width = 1;
			g_usOutDataIdx += p_vlan_config->width + 9;
			continue;
		}
		else
		{
			if(OPL_OK != eopl_get_tag_id_2_0(ucPortS, &tag))
			{
				return -1;
			}

			vosMemCpy(p_byte,(u8_t*)&tag, 4);
			p_byte += 4;

			if(OAM_CTC_VLAN_TAG == p_vlan_config->mode)
			{
				number = 0;
			}
			else if(OAM_CTC_VLAN_TRANSLATE == p_vlan_config->mode)
			{
				if(OPL_OK != eopl_get_vtt_list_2_0(ucPortS, p_byte, &number, tag))
				{
					return -1;
				}
			}
			p_vlan_config->width = 5+ number*8;

			g_usOutDataIdx += p_vlan_config->width + 9;
		}
		#else
		p_byte += sizeof(struct  oam_vlan_config);
        (VOID)DRV_GetPortVlanCfg(ucPortS, &vlanmode, p_byte, &length);
		p_vlan_config->mode = vlanmode;
		p_vlan_config->width += length + 1; 
	    g_usOutDataIdx += p_vlan_config->width + 9;
		#endif
	}
	return 0;
}

u8_t stub_ctc_port_rule_get_2_0(u8_t portId,
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
void eopl_get_onu_classify_2_0(void)
{
	u8_t	size,rlt=0, ucRuleNum=0, ucFieldNum=0, ucPortS, ucPortE;
	u8_t	ucRuleIdx, ucFieldIdx, *p_byte;
	u16_t	usLen=0;
	oam_clasmark_t				*pClsHeader;
	oam_clasmark_rulebody_t		*pClsRule;
	oam_clasmark_fieldbody_t	*pClsField;
	oam_instance_tlv_t_2_0			*p_inst;
	oam_clasmark_rulebody_t  clsRule;
	oam_clasmark_fieldbody_t clsFields[12];
	u8_t precedenceOfRule;
	u8_t numOfRule=0;
	u8_t numOfField;
	u8_t fieldId;
	u8_t curPortId;
	u32_t ret;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}
	for(curPortId = ucPortS; curPortId <= ucPortE; curPortId++)
	{
		if(0 == g_usOutDataIdx)
		{
			memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
			eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
			p_byte = &out_data[0];
			p_byte += (sizeof(oampdu_hdr_t) + sizeof(ctc_header_t));
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_byte = &out_data[g_usOutDataIdx];
		}
		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst =   port_instance_2_0;
		p_inst->value = curPortId;
		p_byte += sizeof(oam_instance_tlv_t_2_0);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);
		pClsHeader = (oam_clasmark_t *)p_byte;
		pClsHeader->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		pClsHeader->leaf = OAM_ATTR_CLAMARK;
		pClsHeader->width = 2; /* action(1Byte) +  numOfRule(1Byte) */
		p_byte += sizeof(oam_clasmark_t);
		for(precedenceOfRule = 1; precedenceOfRule <= 8; precedenceOfRule++)
		{
			memset(&clsRule, 0, sizeof(oam_clasmark_rulebody_t));
			memset(clsFields, 0, sizeof(oam_clasmark_fieldbody_t)*12);
			ret = stub_ctc_port_rule_get_2_0((u8_t)curPortId, precedenceOfRule, &clsRule, &clsFields[0]);
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

void eopl_not_support_response_2_0(u8_t branch ,u16_t leafid,u8_t code)//onu based
{
	u8_t	           *p_out;
	u8_t               oamRspCode;
	oam_rsp_indication_t *p_rsp;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(code);
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
void eopl_ctc_get_req_2_0(u8_t* p_byte)
{
	u8_t	size = sizeof(struct oam_variable_descriptor);
	oam_variable_descriptor_t	*p_des;

#ifdef OAM_SUPPORT_REG_CALLBACK
    u8_t *   p_in_buf = NULL;
    u8_t *   p_out_buf = NULL;
    OAM_EV_CB_ID_t key_val;
    vosMemSet(&key_val,0,sizeof(OAM_EV_CB_ID_t));
#endif

	port_instance_2_0.value = 200; /* magic invalid value 200 */
    u8_t *p_src = p_byte;

	 while(1)
	 {
        if ((p_byte - p_src) >= OAM_MAXIMUM_PDU_SIZE)
        {
            OP_DEBUG(DEBUG_LEVEL_CRITICAL, "invalid packet size");
            break;
        }

		switch(*p_byte)
		{
			case OAM_BRANCH_INSTANCE_V_2_0:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_BRANCH_INSTANCE_V_2_0\n");
				port_instance_2_0 = *((oam_instance_tlv_t_2_0*)p_byte);
                /*memcpy(&port_instance_2_0,p_byte,sizeof(port_instance_2_0));*/
				p_byte +=    sizeof(struct oam_instance_tlv_2_0);
				if((port_instance_2_0.value>OAM_MAX_INSTANCE_ID)
					&& (port_instance_2_0.value != 0xFF)
					&& (port_instance_2_0.value != 0x50)
					&& (port_instance_2_0.value != 0x51)
				)
				{
					return;
				}
				break;
			case OAM_BRANCH_EXTENDED_ATTRIB:
			case OAM_BRANCH_STANDARD_ATTRIB:
				p_des = (oam_variable_descriptor_t *)p_byte;
				switch(p_des->leaf)
				{
					case OAM_ATTR_ONU_SN:
						eopl_get_onusn_rsp_2_0();
						break;
					case OAM_ATTR_ONU_FIMWAREVER:
						eopl_get_onu_firmwarever_2_0();
						break;
					case OAM_ATTR_CHIPSET_ID:
						eopl_get_onu_chipid_2_0();
						break;
					case OAM_ATTR_ONU_CAPABILITIES1:
						eopl_get_onu_capabilities_2_0();
						break;
					case OAM_ATTR_ETH_LINKSTATE:
						eopl_get_onu_eth_linkstate_2_0();
						break;
					case	OAM_ATTR_ETH_PORTPAUSE:
						eopl_get_onu_eth_portpause_2_0();
						break;
					case	OAM_ATTR_ETH_PORT_US_POLICING:
						eopl_get_onu_eth_portpolicing_2_0();
						break;
					case	OAM_ATTR_ETH_PORT_DS_POLICING:
						eopl_get_onu_eth_portdspolicing_2_0();
						break;
					case	OAM_ATTR_PHY_ADMIN:
						eopl_get_onu_eth_phyadmin_2_0();
						break;
					case	OAM_ATTR_AUTONEG_ADMIN_STATE:
						eopl_get_onu_eth_autonegadmin_2_0();
						break;
					case OAM_ATTR_AUTONEG_LOCAL_ABILITY:
						eopl_get_onu_eth_autoneglocal_2_0();
						break;
					case OAM_ATTR_AUTONEG_ADV_ABILITY:
						eopl_get_onu_eth_autonegadv_2_0();
						break;
					case OAM_ATTR_FEC_ABILITY:
						eopl_get_onu_eth_fecability_2_0();
						break;
					case OAM_ATTR_FEC_MODE:
						eopl_get_onu_eth_fecmode_2_0();
						break;
					case OAM_ATTR_MCAST_TAG_STRIP:
						eopl_get_onu_mcast_tagstrip_2_0();
						break;
					case OAM_ATTR_MCAST_SWITCH:
						eopl_get_onu_mcast_switch_2_0();
						break;
					case OAM_ATTR_MCAST_GRP_NUMMAX:
						eopl_get_onu_grp_nummax_2_0();
						break;
					case OAM_ATTR_MCAST_GFASTLEAVE_ABL:
						eopl_get_onu_fastleave_abl_2_0();
						break;
					case OAM_ATTR_MCAST_GFASTLEAVE_ADMIN:
						eopl_get_onu_fastleave_admin_2_0();
						break;
					case OAM_ATTR_MCAST_VLAN:
						eopl_get_onu_mcast_vlan_2_0();
						p_byte += 2;
						break;
					case OAM_ATTR_MCAST_CONTROL:
						eopl_get_onu_mcast_control_2_0();
						p_byte += 2;
						break;
					case OAM_ATTR_VLAN:
						eopl_get_onu_vlan_2_0();
						break;
					case OAM_ATTR_CLAMARK:
						eopl_get_onu_classify_2_0();
						p_byte += 2;
						break;
					default:
#if defined(OAM_SUPPORT_REG_CALLBACK)
                        /*OAM_DBG("Unknown leaf value,so search the OAM CALLBACK table\n");*/
                        key_val.oam_ver = OAM_VER_CTC_2_0_SUPPORT;
                        key_val.oam_ext_code = OAM_EXT_CODE_GET_REQUEST ;
                        key_val.oam_branch_id = *p_byte;
                        key_val.oam_leaf_id = p_des->leaf;

                        p_in_buf = p_byte;
                        p_out_buf = eopl_get_output_buf_2_0(OAM_EXT_CODE_GET_REQUEST );
                        OamEventCallBack(key_val,
                                         p_in_buf,
                                         p_out_buf);
#elif defined(OAM_SUPPORT_ZTE_EXT) /* ZTE CTC extension */
						eopl_zte_ctc_get_req(p_des->leaf, &p_byte);
#else
                        eopl_not_support_response_2_0((u8_t)*p_byte,p_des->leaf,OAM_EXT_CODE_GET_RESPONSE);
                        p_byte += sizeof(oam_rsp_indication_t);
                        OAM_DBG("[%s]Unknown leaf value!!\n ",__FUNCTION__);
#endif
                        break;
				}
				p_byte += size;
				break;
			default:
				if(g_usOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
			 	{
					oam_pdu_padding(&out_data[g_usOutDataIdx],  OAM_MINIMIZE_PDU_SIZE - g_usOutDataIdx);
					g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
			 	}
				OamFrameSend(out_data, g_usOutDataIdx);
				g_usOutDataIdx = 0;
				memset(&port_instance_2_0, 0, sizeof(oam_instance_tlv_t_2_0));
				return;
		}
	 }

}

void eopl_set_onu_eth_portpause_2_0(u8_t action)
{
	u8_t	rlt=OPL_OK, ucPortS, ucPortE, *p_byte;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t_2_0	*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
	*p_inst =   port_instance_2_0;
	p_byte += sizeof(struct oam_instance_tlv_2_0);
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_ETH_PORTPAUSE;

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "flow ctrl set: %d\n", action);
		#if defined(CONFIG_PRODUCT_EPN200)
		rlt = odmPortFlowCtrolEnableSet(ucPortS, action);
		#else
		rlt = DRV_SetPortFlowCtrl(ucPortS, (TRUE == action)? TRUE : FALSE);
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

	g_usOutDataIdx += 9;
}

void eopl_set_onu_eth_policing_2_0(u8_t operation,u8_t* p_cir,u8_t* p_cbs,u8_t* p_ebs)
{
	u8_t	rlt=0, ucPortS, ucPortE, *p_byte;
	u32_t	cir=0, cbs=0, ebs=0;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t_2_0	*p_inst;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}
	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
	*p_inst =   port_instance_2_0;
	p_byte += sizeof(struct oam_instance_tlv_2_0);
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_ETH_PORT_US_POLICING;

	if(operation == 1)
	{
 		vosMemCpy((u8_t*)&cir+1, p_cir, 3);
		vosMemCpy((u8_t*)&cbs+1, p_cbs, 3);
		vosMemCpy((u8_t*)&ebs+1, p_ebs, 3);
	}
	for(; ucPortS<=ucPortE; ucPortS++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "us policing: port %d, cir: %d, cbs: %d, ebs: %d\n", ucPortS, cir, cbs, ebs);
		#if defined(CONFIG_PRODUCT_EPN200)
		rlt = odmPortUsPolicingSet(ucPortS, operation, cir,cbs,ebs);
		#else
        rlt = DRV_SetPortLineRate(ucPortS, DIRECT_IN, cir);
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

	g_usOutDataIdx += 9;
}

void eopl_set_onu_eth_dspolicing_2_0(u8_t operation,u8_t* p_cir,u8_t* p_pir)
{
	u8_t	rlt=0, ucPortS, ucPortE, *p_byte;
	u32_t	cir=0, pir=0;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t_2_0	*p_inst;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}
	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
	*p_inst =   port_instance_2_0;
	p_byte += sizeof(struct oam_instance_tlv_2_0);
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_ETH_PORT_DS_POLICING;

	if(operation == 1)
	{
		vosMemCpy((u8_t*)&cir+1, p_cir, 3);
		vosMemCpy((u8_t*)&pir+1, p_pir, 3);
	}
	for(; ucPortS<=ucPortE; ucPortS++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "ds policing, cir: %d, pir: %d\n", cir, pir);

		#if defined(CONFIG_PRODUCT_EPN200)
		rlt = odmPortDsPolicingSet(ucPortS, operation, cir,pir);
		#else
		rlt = DRV_SetPortLineRate(ucPortS, DIRECT_OUT, cir);
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

	g_usOutDataIdx += 9;
}

void eopl_set_onu_eth_admincontrol_2_0(u32_t action)
{
	u8_t	rlt=0, ucPortS, ucPortE, *p_byte;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t_2_0	*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst =   port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header) + 5;
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_STANDARD_ACTION;
	p_rsp->leaf = OAM_ATTR_PHY_ADMIN_CONTROL;

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
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "mc admin ctrl: %d\n", action);
		#if 0
		rlt = odmPortAdminSet(ucPortS, action);
		#else
        rlt = DRV_SetPortLinkState (ucPortS, action);
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
	g_usOutDataIdx += 4;
}

void eopl_set_onu_eth_atuorestart_2_0(void)
{
	u8_t	rlt=0, ucPortS, ucPortE, *p_byte;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t_2_0	*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst =   port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header) + 5;
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_STANDARD_ACTION;
	p_rsp->leaf = OAM_ATTR_AUTONEG_RST_AUTOCONFIG;

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "autoneg restart\n");
		#if 0
		rlt = dalPhyAutonegRestart(ucPortS);
		#else
        rlt = DRV_PortRestartNeg(ucPortS);
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

	g_usOutDataIdx += 4;
}

void eopl_set_onu_eth_autonegadmin_2_0(u8_t action)
{
	u8_t	rlt=0, ucPortS, ucPortE, *p_byte;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t_2_0	*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}
	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
		*p_inst =   port_instance_2_0;
		p_byte += sizeof(struct oam_instance_tlv_2_0);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header) + 5;
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_STANDARD_ACTION;
	p_rsp->leaf = OAM_ATTR_AUTONEG_ADMIN_CONTROL;

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
	g_usOutDataIdx += 4;
}

void eopl_set_onu_eth_fecmode_2_0(u32_t mode)
{
	u8_t	rlt=0, *p_byte;
	oam_set_response_t	*p_rsp;
	u32_t enable=0;
	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
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

void eopl_set_onu_mcast_group_max_2_0(u8_t max)
{
	u8_t	rlt=0, ucPortS, ucPortE, *p_byte;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t_2_0	*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
	    return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
	*p_inst =   port_instance_2_0;
	p_byte += sizeof(struct oam_instance_tlv_2_0);
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_MCAST_GRP_NUMMAX;
#if !defined(CONFIG_PRODUCT_EPN200)
	for(; ucPortS<=ucPortE; ucPortS++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "mc vlan grp max: %d\n", max);
		rlt = odmMulticastGroupMaxNumSet(ucPortS, max);
	}
#else
    rlt = OPL_NOT_SUPPORTED;
#endif
	if(!rlt) /*for multicast, true means success */
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += 9;
}

void eopl_set_onu_mcast_adm_control_2_0(u8_t value)
{
	u8_t	rlt=0, *p_byte;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t_2_0	*p_inst;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header) ;
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ACTION;
	p_rsp->leaf = OAM_ATTR_FASTLEAVE_CONTROL;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "mc fast leave set: %d\n", value);
#if !defined(CONFIG_PRODUCT_EPN200)
	if (1 == value)
	{
		rlt = odmMulticastFastleaveModeSet(0);
	}
	else if (2 == value)
	{
		rlt = odmMulticastFastleaveModeSet(1);
	}
#else
    rlt = OPL_NOT_SUPPORTED;
#endif
	if(!rlt) /*for multicast, true means success */
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += 4;
}

void eopl_set_onu_mcast_control_2_0(u8_t *p_in)
{
	u8_t	i, *p_byte;
	oam_set_response_t			*p_rsp;
	oam_mcast_control_t			*p_mcast_control;
	oam_mcast_control_entry_t	*p_mcast_entry;
    /*  */
    u32_t retVal = NO_ERROR;
    /* 014530 */

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
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
		retVal = odmMulticastControlEntryClear();
	}
	else
	{
		if (p_mcast_control->action == 1)
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
    		    if(ctl_entry_num_per_port[i]>64)
    		    {
        		    retVal = OPL_ERROR;
        		    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control: port%d entry>64\n", i);
        		    goto send_rsp;
    		    }
		  }
		}

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
		if ((p_mcast_control->action == 1) 
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
        		    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control_2_0: %d+%d>SHIVA_MAX_VLAN_TRANS\n", dal_vtt_num, addNumber);
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
        		    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control_2_0: %d+%d>SHIVA_MAX_VLAN_TRANS\n", vttUcNum, addNumber);
        		    goto send_rsp;
        		  }
		  }
		}
#elif defined(ONU_1PORT)

#if defined(CONFIG_PRODUCT_EPN200)
       
		/*Not support*/
        retVal = OPL_NOT_SUPPORTED;
        goto send_rsp;		
#endif        
#ifndef CTC_MULTICAST_SURPORT
		if (p_mcast_control->action == 1)
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
    		    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control_2_0: %d+%d>%d\n", numOfRule, p_mcast_control->number, MC_MAX_GROUP_NUM);
    		    goto send_rsp;
    		  }
              
                if(p_mcast_control->control_type==MC_CTL_GDA_MAC)
                {   
                    odmMulticastVlanGet(1, vlanIdArray, &numOfMvlan);
                    if(numOfRule+p_mcast_control->number*numOfMvlan>MC_MAX_GROUP_NUM) {
        		  retVal = OPL_ERROR;
        		  OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control_2_0: %d+%d*%d>%d\n", numOfRule, p_mcast_control->number, numOfMvlan, MC_MAX_GROUP_NUM);
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
                		    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control_2_0: %d+%d>%d\n", numOfRecords, addNumber, DAL_PORT_MCAST_ENTRY_NUM);
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
        		    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_control_2_0: %d>%d\n", addNumber, DAL_PORT_MCAST_ENTRY_NUM);
        		    goto send_rsp;
        		  }
                    }
                }
		}
#endif

#endif
		odmMulticastControlTypeSet(p_mcast_control->control_type);
		p_in += sizeof(struct oam_mcast_control);
		p_mcast_entry = (oam_mcast_control_entry_t*)p_in;
		for(i=0;i<p_mcast_control->number;i++,p_mcast_entry++)
		{
			if(p_mcast_control->action == 0)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "mc vlan ctrl del, port: %d, vlan: %d, gda: %02x:%02x:%02x:%02x:%02x:%02x\n",
					p_mcast_entry->port_id, p_mcast_entry->vlan_id, p_mcast_entry->gda[0],
					p_mcast_entry->gda[1], p_mcast_entry->gda[2], p_mcast_entry->gda[3],
					p_mcast_entry->gda[4], p_mcast_entry->gda[5]);

				retVal = odmMulticastControlEntryDelete(p_mcast_entry->port_id,
	 	         				       p_mcast_entry->vlan_id,
	 	         				       p_mcast_entry->gda);
			}
			if(p_mcast_control->action == 1)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "mc vlan ctrl add, port: %d, vlan: %d, gda: %02x:%02x:%02x:%02x:%02x:%02x\n",
					p_mcast_entry->port_id, p_mcast_entry->vlan_id, p_mcast_entry->gda[0],
					p_mcast_entry->gda[1], p_mcast_entry->gda[2], p_mcast_entry->gda[3],
					p_mcast_entry->gda[4], p_mcast_entry->gda[5]);
				retVal = odmMulticastControlEntryAdd(p_mcast_entry->port_id,
	 	         				       p_mcast_entry->vlan_id,
	 	         				       p_mcast_entry->gda);
			}
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

	g_usOutDataIdx += 4;
}

void eopl_set_onu_mcast_switch_2_0(u8_t action)
{
	u8_t	rlt=0, *p_byte;
	oam_set_response_t	*p_rsp;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_MCAST_SWITCH;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "mc vlan switch: %d\n", action);

#if !defined(CONFIG_PRODUCT_EPN200)
	rlt = odmMulticastSwitchSet(action);
#else
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
#endif
	if(!rlt) /*for multicast, true means success */
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += 4;
}

void eopl_set_onu_mcast_tagstrip_2_0(u32_t tagstriped)
{
	u8_t	rlt=0, ucPortS, ucPortE, *p_byte;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t_2_0	*p_inst;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
	*p_inst =   port_instance_2_0;
	p_byte += sizeof(struct oam_instance_tlv_2_0);
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_MCAST_TAG_STRIP;

#if !defined(CONFIG_PRODUCT_EPN200)
	for(; ucPortS<=ucPortE; ucPortS++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "mc vlan strip: %d\n", tagstriped);
		rlt = odmMulticastTagstripSet(ucPortS,tagstriped);
	}
#else
    rlt = OPL_NOT_SUPPORTED;
#endif
	if(!rlt)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}
	g_usOutDataIdx += 9;
}

void eopl_set_onu_mcast_vlan_2_0(u8_t *p_in)
{
	u8_t	i, number, ucPortS, ucPortE, curPortId, *p_byte;
	u16_t	*p_vid;
	oam_set_response_t	*p_rsp;
	oam_mcast_vlan_t	*p_mcast_vlan;
	oam_instance_tlv_t_2_0	*p_inst;
	u32_t   rlt = 0;

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return;
	}

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
	*p_inst =   port_instance_2_0;
	p_byte += sizeof(struct oam_instance_tlv_2_0);
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_MCAST_VLAN;

	p_mcast_vlan = (oam_mcast_vlan_t*)p_in;

	if(p_mcast_vlan->width == 1)
	{
		for(; ucPortS<=ucPortE; ucPortS++)
		{
			odmMulticastVlanClear(ucPortS);
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

            /*  2012-9-6 */
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
              
              if(p_mcast_vlan->operation == 1) {
                if(ucPortS != ucPortE) {
                  vlanAddNum = ODM_NUM_OF_PORTS*(number/2);
                }
                else {
                  vlanAddNum = 1*(number/2);
                }
                if(dal_vtt_num+vlanAddNum>16) {
                  OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_vlan_2_0: %d+%d>SHIVA_MAX_VLAN_TRANS\n", dal_vtt_num, vlanAddNum);
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
            UINT8 numOfRecords = 0;
            multicast_control_entry_t ctl_entry_list[MC_MAX_GROUP_NUM];
            UINT8 numOfRule = 0;    
            UINT16 numOfEntries = 0;
          
            if(p_mcast_vlan->operation == 1) {
              if(ucPortS != ucPortE) {
                vlanAddNum = ODM_NUM_OF_PORTS*(number/2);
              }
              else {
                vlanAddNum = 1*(number/2);
              }

              dalVttMulticastNumOfRecordGet(&numOfRecords);
              if(numOfRecords+vlanAddNum>DAL_PORT_MCAST_ENTRY_NUM) {
                OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_vlan_2_0: %d+%d>%d\n", numOfRecords, vlanAddNum, DAL_PORT_MCAST_ENTRY_NUM);
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
                    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_mcast_vlan_2_0: %d+%d*%d>%d\n", numOfRule, vlanAddNum, numOfEntries, MC_MAX_GROUP_NUM);
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
	 		if(p_mcast_vlan->operation == 0)
	 		{
				for(curPortId=ucPortS; curPortId<=ucPortE; curPortId++)
				{
					odmMulticastVlanDelete(curPortId, *p_vid);
	 			}
	 		}

			if(p_mcast_vlan->operation == 1)
			{
				for(curPortId=ucPortS; curPortId<=ucPortE; curPortId++)
				{
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
  
  g_usOutDataIdx += 9;
}

int eopl_set_tag_id_2_0(u8_t portid,u32_t *value)
{
	u8_t	rlt=0;
	u16_t	*p_16;

	p_16 = (u16_t*)value;
	if (0x0000 != *p_16)
	{
		rlt = dalVttTpidSet(*p_16, portid);
	}
	
	if(rlt != OPL_OK)
	{
		return -1;
	}

	p_16++;
	rlt = odmPortDefaultVlanSet(portid, (*p_16)&0xFFF);
	if(rlt != OPL_OK)
	{
		return -1;
	}
	rlt = odmPortDefaultEtherPriSet(portid, (*p_16)>>13);
	if(rlt != OPL_OK)
	{
		return -1;
	}

	return OPL_OK;
}

int eopl_IsExistVlanList(u32_t * pList, u32_t vlan, u32_t num)
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

int eopl_set_onu_vlan_2_0(u8_t*  p_in)
{
	u8_t	i, j, ucPortS, ucPortE, *p_byte;
	u32_t   rlt = 0;
	u32_t	defaultTag, oldTag, newTag;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t_2_0	*p_inst;
	oam_vlan_t			*p_vlan;
	UINT32 oldMode;
	UINT32 vlanId;
	u32_t vlanlist[32];
	u32_t vlanIndex;	
	u8_t * p_vlan_check;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	/* invalid magic value 200, out*/
	if(port_instance_2_0.value == 200)
	{
		return -1;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
	*p_inst =   port_instance_2_0;
	p_byte += sizeof(struct oam_instance_tlv_2_0);
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_VLAN;

	p_vlan = (oam_vlan_t*)p_in;
	
	if(OAM_CTC_VLAN_TRANSPARENT == p_vlan->mode)
	{
        #if defined(CONFIG_PRODUCT_EPN200)
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "vlan mode transparent.\n");
		for(j = ucPortS; j<=ucPortE; j++)
		{
			rlt = odmPortVlanModeSet(j, OAM_CTC_VLAN_TRANSPARENT);
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[port %d] retVal = %d\n", j, rlt);
		}
        #else
        i32_t iRet = NO_ERROR;

        if (0xFFFF == ucPortS)
        {
            printf("\nDo not set transparent vlan for all ports.\n");
            rlt = OPL_OK;
            goto send_rsp;
        }
        
        iRet = CTC_DRV_SetTransparentVlan(ucPortS);
        if (NO_ERROR != iRet)
        {
            printf("\nFailed to set transparent vlan for port %d.\n",ucPortS);
            rlt = OPL_ERROR;
            goto send_rsp;
        }
        #endif
	}
	else if(OAM_CTC_VLAN_TAG == p_vlan->mode)
	{
		p_in += 5;
		vosMemCpy(&defaultTag, p_in, 4);
#if defined(CONFIG_PRODUCT_EPN200)
		
		/*Not support*/
        rlt = OPL_NOT_SUPPORTED;
        goto send_rsp;
		
		////20100327_XFAN_ADD_BEGIN
#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
              UINT32 vlanAddNum;
              vlanAddNum = 0;
              extern UINT8	dal_vtt_num;
	       extern UINT8 dalToAtherosPortMap[11];
		extern VLAN_CONFI_INFO_t dal_vtt_info[ODM_NUM_OF_PORTS +1];
	       UINT32 ar8228Port;
			        
              if(ucPortS != ucPortE) {
                vlanAddNum = ODM_NUM_OF_PORTS;
              }
              else {
                vlanAddNum = 1;
              }

              for(j=ucPortS; j <= ucPortE; j++)
	       {
	           ar8228Port = dalToAtherosPortMap[j];
		    if((dal_vtt_info[ar8228Port].vlanMode == VLAN_TAG) |
			(dal_vtt_info[ar8228Port].vlanMode == VLAN_TRANSLATION))
			vlanAddNum = vlanAddNum - 1;
	       }
			        
              if(dal_vtt_num+vlanAddNum>16) {
              	rlt += 1;
                OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_vlan_2_0 OAM_CTC_VLAN_TAG:%d+%d>16\n", dal_vtt_num, vlanAddNum);
                goto send_rsp;
              }

#endif
    ////20100327_XFAN_ADD_END	
		
		//set_vlan_to_eth2(defaultTag,OAM_CTC_VLAN_TAG); /*pippo add*/
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[port %d] vlan mode tag.\n", ucPortS);
	    for(j=ucPortS; j<=ucPortE; j++)
		{
			rlt = eopl_set_tag_id_2_0(j, &defaultTag);
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[port %d] default tag: 0x%08x\n", j, defaultTag);
			if (OPL_OK != rlt)
			{
				goto send_rsp;
			}

			rlt = odmPortVlanModeSet(j, OAM_CTC_VLAN_TAG);
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[port %d] retVal = %d\n", j, rlt);
			if (OPL_OK != rlt)
			{
				goto send_rsp;
			}			
		}
#else
        i32_t iRet = NO_ERROR;
        u32_t priority;
        u32_t default_vlan;

        rlt = OPL_OK;

        iRet = CTC_DRV_GetVlanIdAndPri(defaultTag, &default_vlan, &priority);
        if (NO_ERROR != iRet)
        {
            printf("\nFailed to set tag vlan for port %d.\n",ucPortS);
            rlt = OPL_ERROR;
		    goto send_rsp;
        }

        iRet = CTC_DRV_SetTagVlan(ucPortS, default_vlan, priority);
		if (NO_ERROR != iRet)
        {
            printf("\nFailed to set tag vlan for port %d.\n",ucPortS);
            rlt = OPL_ERROR;
		    goto send_rsp;
        }
#endif
	}
	else if(OAM_CTC_VLAN_TRANSLATE == p_vlan->mode)
	{
#if defined(CONFIG_PRODUCT_EPN200)
        
		/*Not suport*/
        rlt = OPL_NOT_SUPPORTED;
        goto send_rsp;
		
        
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "vlan mode translation.\n");
		p_in += 5;

		vosMemCpy(&defaultTag, p_in, 4);
		p_in += 4;

		/* check if default vlan exist vlan list */
		vosMemSet(vlanlist, 0, sizeof(vlanlist));
		vlanIndex = 0;		
		vlanId = defaultTag & 0xFFF;
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
				rlt = OAM_SET_BAD_PARAMETERS;
				goto send_rsp;
			}			

			if (eopl_IsExistVlanList(vlanlist, oldTag, vlanIndex) || eopl_IsExistVlanList(vlanlist, newTag, vlanIndex))
			{
		    	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "vlan %d.%d duplicate.\n", oldTag, newTag);
				rlt = OAM_SET_BAD_PARAMETERS;
				goto send_rsp;
			}
			vlanlist[vlanIndex++] = oldTag;
			vlanlist[vlanIndex++] = newTag;

			/* max support 16 translate record */
			if (vlanIndex > 16)
			{
                /*begin modified by liaohongjun 2012/7/30 of QID0001*/
                printf("\n Onu only supports 8 vlan translation pairs for every port. \n");   
                /*end modified by liaohongjun 2012/7/30 of QID0001*/
				rlt = OAM_SET_BAD_PARAMETERS;
				goto send_rsp;
			}

			if (oldTag == vlanId)
			{
                /*begin modified by liaohongjun 2012/7/30 of QID0001*/
		    	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "default tag %d exist in vlan list that be translate.\n", vlanId);
                /*end modified by liaohongjun 2012/7/30 of QID0001*/
				rlt = OAM_SET_BAD_PARAMETERS;
				goto send_rsp;
			}
		}		
		
		//set_vlan_to_eth2(defaultTag,OAM_CTC_VLAN_TRANSLATE); /*pippo add*/
		
		////20100327_XFAN_ADD_BEGIN
#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
              UINT32 vlanAddNum;
              vlanAddNum = 0;
              extern UINT8	dal_vtt_num;
			        
              if(ucPortS != ucPortE) {
                vlanAddNum = ODM_NUM_OF_PORTS*((p_vlan->width - 5)/8);
              }
              else {
                vlanAddNum = 1*((p_vlan->width - 5)/8);
              }
			        
              if(vlanAddNum>16) {
              	rlt += 1;
                OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_vlan_2_0 OAM_CTC_VLAN_TRANSLATE:%d+%d>16\n", dal_vtt_num, vlanAddNum);
                goto send_rsp;
              }
#elif defined(ONU_1PORT)
              UINT32 vlanAddNum = 0;
              
              if(ucPortS != ucPortE) {
                vlanAddNum = ODM_NUM_OF_PORTS*((p_vlan->width - 5)/8);
              }
              else {
                vlanAddNum = 1*((p_vlan->width - 5)/8);
              }

              if(vlanAddNum > 11) {
                rlt += 1;
                OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "eopl_set_onu_vlan_2_0 OAM_CTC_VLAN_TRANSLATE: %d>11\n", vlanAddNum);
                goto send_rsp;
              }
#endif
    ////20100327_XFAN_ADD_END		
    
		for(j = ucPortS; j <= ucPortE; j++)
		{
			rlt = eopl_set_tag_id_2_0(j, &defaultTag);
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ port %d ] default tag: 0x%08x\n", j, defaultTag);
			if (OPL_OK != rlt)
			{
				goto send_rsp;
			}			

			rlt = odmPortVlanModeSet(j, OAM_CTC_VLAN_TRANSLATE);
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ port %d ] retVal = %d\n", j, rlt);
			if (OPL_OK != rlt)
			{
				goto send_rsp;
			}
		}

		for(i = 0; i < (p_vlan->width - 5)/8; i++)
		{
			vosMemCpy(&oldTag, (u8_t*)p_in, 4);
			p_in += 4;
			vosMemCpy(&newTag, (u8_t*)p_in, 4);
			p_in += 4;

			for(j = ucPortS; j <= ucPortE; j++)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ port %d ] old tag: 0x%08x, new tag: 0x%08x\n", j, oldTag, newTag);
				rlt = odmPortVttEntryAdd(j, 0, oldTag, 0, newTag);
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ port %d ] retVal = %d\n", j, rlt);
				if (OPL_OK != rlt)
				{
					goto send_rsp;
				}
			}
		}
#else
        i32_t iRet = NO_ERROR;
        u32_t uiVlanPair;
        u32_t priority;
        
        rlt = OPL_OK;
        p_in += 5;

		vosMemCpy(&defaultTag, p_in, 4);

		p_in += 4;

		vosMemSet(vlanlist, 0, sizeof(vlanlist));
		vlanIndex = 0;		
		//vlanId = defaultTag & 0xFFF;
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

			vlanlist[vlanIndex++] = oldTag;
			vlanlist[vlanIndex++] = newTag;

			/* max support 16 translate record */
			if (vlanIndex > 16)
			{
                printf("\n Onu only supports 8 vlan translation pairs for every port. \n");
				rlt = OAM_SET_BAD_PARAMETERS;
				goto send_rsp;
			}
		}

        iRet = CTC_DRV_SetTranslationVlan(ucPortS, defaultTag, vlanlist, (vlanIndex/2));

		if (NO_ERROR != iRet)
        {
            printf("\nFailed to set translation vlan for port %d.\n",ucPortS);
            rlt = OPL_ERROR;
		    goto send_rsp;
        }
#endif
	}else {
        rlt = ERR_VARNORESOURCE;
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

	g_usOutDataIdx += 9;

	return 0;
}

u8_t stub_ctc_port_rule_clear_2_0(u8_t port_id)
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

u8_t stub_ctc_port_rule_delete_2_0(u8_t port_id, u8_t precedenceOfRule)
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

u8_t stub_ctc_port_rule_add_2_0(u8_t portId,
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

	for(fieldId = 0; fieldId < numOfField; fieldId++)
	{
		if(pClsField->fieldSelect == OAM_CTC_CLS_FIELD_TYPE_DA_MAC)
			{
				odmClsRule.dstMacFlag = 1;
				odmClsRule.dstMacOp = pClsField->operator;
				vosMemCpy(odmClsRule.dstMac.lowRange, pClsField->matchValue, 6);
				OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: dst MAC = %.2x:%.2x%.2x:%.2x:%.2x:%.2x:\n",
					odmClsRule.dstMac.lowRange[0],
					odmClsRule.dstMac.lowRange[1],
					odmClsRule.dstMac.lowRange[2],
					odmClsRule.dstMac.lowRange[3],
					odmClsRule.dstMac.lowRange[4],
					odmClsRule.dstMac.lowRange[5]);
				vosMemCpy(&(odmClsRule.dstMac.highRange),fullmacMask,MAC_LENGTH);

				pClsField++;

				continue;
			}

			if(pClsField->fieldSelect == OAM_CTC_CLS_FIELD_TYPE_SA_MAC)
			{
				odmClsRule.srcMacFlag = 1;
				odmClsRule.srcMacOp = pClsField->operator;
				vosMemCpy(odmClsRule.srcMac.lowRange, pClsField->matchValue, 6);
				OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: source MAC = %.2x:%.2x%.2x:%.2x:%.2x:%.2x:\n",
					odmClsRule.srcMac.lowRange[0],
					odmClsRule.srcMac.lowRange[1],
					odmClsRule.srcMac.lowRange[2],
					odmClsRule.srcMac.lowRange[3],
					odmClsRule.srcMac.lowRange[4],
					odmClsRule.srcMac.lowRange[5]);
				vosMemCpy(&(odmClsRule.srcMac.highRange),fullmacMask,MAC_LENGTH);

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

				odmClsRule.lenOrTypeFlag = 1;
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

				odmClsRule.dstIpFlag = 1;
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
				odmClsRule.ipV6Pre.highRange=0xff;
				OP_DEBUG(DEBUG_LEVEL_INFO, "file entry: IPV6 precedence = %d\n", odmClsRule.ipV6Pre.lowRange);

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
				odmClsRule.dstL4Port.highRange=0xffff;
				odmClsRule.dstL4Port.lowRange = pClsField->matchValue[4] << 8;
				odmClsRule.dstL4Port.lowRange |= pClsField->matchValue[5];
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

void eopl_set_onu_classfy_2_0(u8_t*  p_in)
{
	u8_t	ucPortS, ucPortE, *p_byte;
	int		iStatus;
	oam_instance_tlv_t_2_0	*p_inst;
	oam_set_response_t	*p_rsp;
	oam_clasmark_t		*pClsHeader;
	oam_clasmark_rulebody_t	*pClsRule;
	oam_clasmark_fieldbody_t *pClsField;
	oam_clasmark_t		*pstClassify;
	u8_t ret;
	u8_t precedenceOfRule;
	u8_t numOfRule;
	u8_t numOfField;
	u8_t ruleId;
	u16_t curPortId;

	#if 0
	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}
	#endif
	//begin: added by tanglin for the bug 2992

#if 0
	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = 1;
	}
	else
	{
		if( port_instance_2_0.value > 1)
		{
            ret=OAM_SET_BAD_PARAMETERS;
            goto send_rsp;
		}
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}
#else
    if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = CTC_MAX_SUBPORT_NUM;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}
#endif

	//end : added by tanglin for the bug 2992


	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_byte = &out_data[0];
		p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}
	p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
	*p_inst =   port_instance_2_0;
	p_byte += sizeof(struct oam_instance_tlv_2_0);
	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_CLAMARK;

	pClsHeader = (oam_clasmark_t *)p_in;
	p_in += sizeof(oam_clasmark_t);
	if(OAM_CLASSIFY_ACTION_CLR == pClsHeader->action)
	{
		
        #if defined(CONFIG_PRODUCT_EPN200)
		for(curPortId = ucPortS; curPortId <= ucPortE; curPortId++)
	{
			ret = stub_ctc_port_rule_clear_2_0((u8_t)ucPortS);
			if (OAM_SET_OK != ret)
			{
				goto send_rsp;
			}
		}
        #else
        ret = OAM_SET_OK;

        for(curPortId = ucPortS; curPortId <= ucPortE; curPortId++)
        {
            DRV_DEBUG_PRINT("\nctc2.0: Clear port%d's clf.\n",curPortId);
            if (NO_ERROR != DRV_CtcClearClfRmkByPort(curPortId))
            {
                DRV_DEBUG_PRINT("\nfunc:%s, line:%d\n",__FUNCTION__, __LINE__);
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
			precedenceOfRule = *((u8_t *)p_in);
           
            #if defined(CONFIG_PRODUCT_EPN200)
			for(curPortId = ucPortS; curPortId <= ucPortE; curPortId++)
			{
				ret = stub_ctc_port_rule_delete_2_0((u8_t)curPortId, precedenceOfRule);
				if (OAM_SET_OK != ret)
				{
					goto send_rsp;
				}
			}
            #else
            ret = OAM_SET_OK;
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
            
            if (NO_ERROR != DRV_CtcDelClfRmkByPrec(ucPortS, precedenceOfRule))
            {
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
           
		    /*Not suport*/
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
            for(curPortId = ucPortS; curPortId <= ucPortE; curPortId++)
			{
				ret = stub_ctc_port_rule_add_2_0((u8_t)curPortId, precedenceOfRule, pClsRule, pClsField);
				if (OAM_SET_OK != ret)
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
            
            DRV_DEBUG_PRINT("\nctc2.0: Add clf for port%d. prece:%d, num:%d\n",ucPortS,precedenceOfRule,numOfRule);
            if (NO_ERROR != DRV_CtcAddClfRmkByPrec(ucPortS, precedenceOfRule, pClsRule, pClsField))
            {
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

	p_inst = (oam_instance_tlv_t_2_0 *)p_byte;
	*p_inst =	port_instance_2_0;
	p_byte += sizeof(oam_instance_tlv_t_2_0);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

	p_rsp = (oam_set_response_t*)p_byte;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ATTR_CLAMARK;
	p_rsp->indication = ret;

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

void eopl_ctc_set_req_2_0(u8_t* p_byte)
{
	u8_t	number;
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
	oam_vlan_t					*p_vlan;
	oam_clasmark_t				*pstClassify;
    u8_t *p_src = p_byte;
	port_instance_2_0.value = 200; /* magic invalid value 200 */
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
			case OAM_BRANCH_INSTANCE_V_2_0:
				/* port_instance_2_0 = *((oam_instance_tlv_t_2_0*)p_byte); */
                            memcpy(&port_instance_2_0,p_byte,sizeof(port_instance_2_0));
				p_byte +=    sizeof(struct oam_instance_tlv_2_0);
				if((port_instance_2_0.value>OAM_MAX_INSTANCE_ID)
					&& (port_instance_2_0.value != 0xFF))
				{
					return;
				}
				break;
			case OAM_BRANCH_EXTENDED_ATTRIB:
			case OAM_BRANCH_STANDARD_ATTRIB:
				p_des = (oam_variable_descriptor_t *)p_byte;
				switch(p_des->leaf)
				{
					case OAM_ATTR_ETH_PORTPAUSE:
						p_pause = (oam_eth_portpause_t*)p_byte;
						eopl_set_onu_eth_portpause_2_0(p_pause->back_oper);
						p_byte += p_pause->width + 4;
						break;
					case OAM_ATTR_ETH_PORT_US_POLICING:
						p_policing = (oam_eth_portpolicing_t*)p_byte;
						eopl_set_onu_eth_policing_2_0(p_policing->operation, p_policing->cir,p_policing->cbs,p_policing->ebs);
						p_byte += p_policing->width + 4;
						break;
					case OAM_ATTR_ETH_PORT_DS_POLICING:
						p_dspolicing = (oam_eth_portdspolicing_t*)p_byte;
						eopl_set_onu_eth_dspolicing_2_0(p_dspolicing->operation, p_dspolicing->cir,p_dspolicing->pir);
						p_byte += p_dspolicing->width + 4;
						break;
					case OAM_ATTR_VLAN:
						p_vlan = (oam_vlan_t*)p_byte;
						eopl_set_onu_vlan_2_0(p_byte);
						p_byte += p_vlan->width + 4;
						break;
					case OAM_ATTR_CLAMARK:
						pstClassify = (oam_clasmark_t *)p_byte;
						eopl_set_onu_classfy_2_0(p_byte);
						p_byte += pstClassify->width + 4;
						break;
					case OAM_ATTR_MCAST_VLAN:
						p_mcast_vlan = (oam_mcast_vlan_t*)p_byte;
						eopl_set_onu_mcast_vlan_2_0(p_byte);

						#if 0
						p_byte += sizeof(struct oam_mcast_vlan);
						if(p_mcast_vlan->width != 1)
						{
							number = p_mcast_vlan->width-1;
							if(number%2 !=0)
							{
								 return;
							}
							p_byte += 2*number;
						}
						#endif
						p_byte += p_mcast_vlan->width + 4;


						break;
					case OAM_ATTR_MCAST_TAG_STRIP:
						p_mcast_stripe = (oam_mcast_tagstripe_t*)p_byte;
						eopl_set_onu_mcast_tagstrip_2_0(p_mcast_stripe->tagstriped);
						p_byte += sizeof(struct oam_mcast_tagstripe);
						break;
					case OAM_ATTR_MCAST_SWITCH:
						p_mcast_switch = (oam_mcast_switch_t*)p_byte;
						eopl_set_onu_mcast_switch_2_0(p_mcast_switch->action);
						p_byte += sizeof(struct oam_mcast_switch);
						break;
					case OAM_ATTR_MCAST_CONTROL:
						p_mcast_control = (oam_mcast_control_t*)p_byte;
						eopl_set_onu_mcast_control_2_0(p_byte);
						if(p_mcast_control->width == 1)
						{
							p_byte += sizeof(struct oam_mcast_control) - 2;
						}
						else
						{
							p_byte += sizeof(struct oam_mcast_control)+10*p_mcast_control->number;
						}
						break;
					case OAM_ATTR_MCAST_GRP_NUMMAX:
						p_group = (oam_group_num_max_t*)p_byte;
						eopl_set_onu_mcast_group_max_2_0(p_group->num_max);
						p_byte += sizeof(struct oam_group_num_max);
						break;
					case OAM_ATTR_FEC_MODE:
						p_fec = (oam_fec_mode_t*)p_byte;
						eopl_set_onu_eth_fecmode_2_0(p_fec->value);
						p_byte += sizeof(struct oam_fec_mode);
						break;
					default:
#if defined(OAM_SUPPORT_REG_CALLBACK)
                        //OAM_DBG("[%s]Unknown leaf value,so search the OAM CALLBACK table\n ",__FUNCTION__ );
                        key_val.oam_ver = OAM_VER_CTC_2_0_SUPPORT;
                        key_val.oam_ext_code = OAM_EXT_CODE_SET_REQUEST ;
                        key_val.oam_branch_id = *p_byte;
                        key_val.oam_leaf_id = p_des->leaf;

                        p_in_buf = p_byte;
                        p_out_buf = eopl_get_output_buf_2_0(OAM_EXT_CODE_GET_REQUEST );
                        OamEventCallBack(key_val,
                                         p_in_buf,
                                         p_out_buf);
#elif defined(OAM_SUPPORT_ZTE_EXT) /* ZTE CTC extension */
						eopl_zte_ctc_set_req(p_des->leaf, &p_byte);
#else
                        eopl_not_support_response_2_0((u8_t)*p_byte,p_des->leaf,OAM_EXT_CODE_SET_RESPONSE);
                        p_byte += sizeof(oam_rsp_indication_t);
                        OAM_DBG("[%s]Unknown leaf value!!\n ",__FUNCTION__);    
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
						return ;
					case OAM_ATTR_FASTLEAVE_CONTROL:
						p_fast = (oam_fastleave_adm_t*)p_byte;
						eopl_set_onu_mcast_adm_control_2_0(p_fast->value);
						p_byte += sizeof(struct oam_fastleave_adm);
						break;
					case OAM_ATTR_PHY_ADMIN_CONTROL:
						p_phy_admin = (oam_phy_admincontrol_t*)p_byte;
						eopl_set_onu_eth_admincontrol_2_0(p_phy_admin->value);
						p_byte += sizeof(struct oam_phy_admincontrol);
						break;
					case OAM_ATTR_AUTONEG_RST_AUTOCONFIG:
						eopl_set_onu_eth_atuorestart_2_0();
						p_byte += sizeof(struct oam_autoneg_restart);
						p_byte += 5; /* add to satisfy tk*/
						break;
					case OAM_ATTR_AUTONEG_ADMIN_CONTROL:
						p_auto_admin = (oam_autoneg_admincontrol_t*)p_byte;
						eopl_set_onu_eth_autonegadmin_2_0(p_auto_admin->indication);
						p_byte += sizeof(struct oam_phy_admincontrol);
						break;
					default:
                        eopl_not_support_response_2_0((u8_t)*p_byte,p_des->leaf,OAM_EXT_CODE_SET_RESPONSE);

                        p_byte += sizeof(oam_rsp_indication_t);
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
				g_usOutDataIdx = 0;
				memset(&port_instance_2_0, 0, sizeof(oam_instance_tlv_t_2_0));
				return;
		}
	}

}

void eopl_req_new_key_2_0(u8_t index)
{
	u8_t	ucKeyId, *p_byte, aucKeyVal[3];

	vosMemSet(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header_2_0(OAM_EXT_CODE_CHURNING);
	p_byte = &out_data[0];
	p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);

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
}

void eopl_req_tri_scb_key_2_0(u8_t index, u8_t *pucKey)
{
	u8_t *p_byte, ucCnt;

	vosMemSet(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header_2_0(OAM_OPL_OPCODE_TRI_SCB);
	p_byte = &out_data[0];
	p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);

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

void eopl_ctc_churning_2_0(u8_t* p_byte)
{
	switch(*p_byte)
	{
		case OAM_NEW_KEY_REQUEST:
			eopl_req_new_key_2_0(*(p_byte+1));
			break;
		default:
			break;
	}
}

void eopl_ctc_dba_2_0(u8_t* p_byte)
{
	switch(*p_byte)
	{
		case OAM_GET_DBA_REQUEST:
			eopl_get_dba_2_0();
			break;
		case OAM_SET_DBA_REQUEST:
			/*if(*(p_byte+1) != 2)
				return;*/
			p_byte+=2;
			eopl_set_dba_2_0(*(p_byte-1), (oam_dba_queue_t *)p_byte);
			break;
		default:
			break;
	}
}

void eopl_req_aes_key_2_0(u8_t index, u8_t ucMode)
{
	u8_t	ucKeyId, aucKeyVal[16], ucCnt, *p_byte;

	vosMemSet(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header_2_0(OAM_OPL_OPCODE_AES);
	p_byte = &out_data[0];
	p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);

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

void eopl_req_aesscb_key_2_0(u8_t index, u8_t ucMode, u8_t *pucKey)
{
	u8_t	*p_byte, ucCnt;

	vosMemSet(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header_2_0(OAM_OPL_OPCODE_AES_SCB);
	p_byte = &out_data[0];
	p_byte += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);

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

void eopl_aeskey_req_2_0(u8_t* p_byte)
{
	switch(*p_byte)
	{
		case OAM_NEW_KEY_REQUEST:
			eopl_req_aes_key_2_0(*(p_byte+1), *(p_byte+2));
			break;
		default:
			break;
	}
}

void eopl_aesscbkey_req_2_0(u8_t* p_byte)
{
	switch(*p_byte)
	{
		case OAM_NEW_KEY_REQUEST:
			eopl_req_aesscb_key_2_0(*(p_byte+1), *(p_byte+2), p_byte+3);
			break;
		default:
			break;
	}
}

void eopl_TriScbkey_req_2_0(u8_t* p_byte)
{
	switch(*p_byte)
	{
		case OAM_NEW_KEY_REQUEST:
			eopl_req_tri_scb_key_2_0(*(p_byte+1), p_byte+2);
			break;
		default:
			break;
	}
}
extern u8_t oamtransmitevent;
OPL_STATUS eopl_ctc_tftp_wreq_2_0(u8_t *pucFileName, u8_t ucNameLen)
{
	u8_t *pucPkt;
	SOFT_DOWNLOAD_HD_MASK_t *pstDownHd;

	if ((0 == g_stImageFile_2_0.ucNameLen)||(1 == g_stImageFile_2_0.ucFlash))
	{
		g_stImageFile_2_0.ucNameLen = ucNameLen;
		g_stImageFile_2_0.ucDownload = 0;
		g_stImageFile_2_0.ucFlash = 0;
		g_stImageFile_2_0.usBlock1 = 0;
		g_stImageFile_2_0.usBlock2 = 0;
		g_stImageFile_2_0.uiFileSize = 0;
		g_stImageFile_2_0.pstData = 0;
		memcpy(g_stImageFile_2_0.aucFileName, "/home/zlu/yy", FILENAME_OFFSET);
		memcpy(g_stImageFile_2_0.aucFileName+FILENAME_OFFSET, pucFileName, ucNameLen);
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header_2_0(OAM_EXT_CODE_DOWNLOAD);
		pucPkt = &out_data[0];
		pucPkt += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
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

OPL_STATUS eopl_ctc_gen_file_2_0(void)
{
	FILE *pFile;
	TFTP_DATA_t *pstData;

	pFile = fopen((char *)(g_stImageFile_2_0.aucFileName), "wb");

	pstData = g_stImageFile_2_0.pstData;
	while(pstData)
	{
		fwrite(pstData->aucDataBlock, 1, pstData->usLen, pFile);
		pstData = pstData->pstNext;
	}

	fclose(pFile);
	chmod((char *)g_stImageFile_2_0.aucFileName, S_IXUSR|S_IXGRP|S_IXOTH);

	return 0;
}

OPL_STATUS eopl_ctc_write_flash_2_0(void)
{
	OPL_STATUS iStatus=0;

	g_stImageFile_2_0.ucFlash = 1;

	return iStatus;
}

OPL_STATUS eopl_ctc_tftp_data_2_0(u8_t *pucData, u16_t usLen, u8_t ucCnt)
{
	TFTP_DATA_t *pstData;
	SOFT_DOWNLOAD_HD_MASK_t *pstDownHd;
	u8_t *pucPkt;

	if ((4 <= usLen)&&(516 >= usLen))
	{
		pstData = (TFTP_DATA_t *)malloc(sizeof(TFTP_DATA_t));
		memcpy(pstData->aucDataBlock, pucData+2, usLen-4);
		pstData->usLen = usLen - 4;
		pstData->pstNext = 0;
	}
	else
	{
		return 0;
	}

	if(g_stImageFile_2_0.usBlock1+1 == *(u16_t *)pucData)
	{
		/*printf("file data block id(%d) mactch, block length: %d.\n",
			g_stImageFile.usBlock1+1, usLen);*/
		g_stImageFile_2_0.usBlock2 = g_stImageFile_2_0.usBlock1;
		g_stImageFile_2_0.usBlock1 = *(u16_t *)pucData;
		if (0 == g_stImageFile_2_0.pstData)
		{
			g_stImageFile_2_0.pstData = pstData;
			g_stImageFile_2_0.pstDataEnd = pstData;
			g_stImageFile_2_0.uiFileSize = (u32_t)(pstData->usLen);
		}
		else
		{
			g_stImageFile_2_0.pstDataEnd->pstNext = pstData;
			g_stImageFile_2_0.pstDataEnd = pstData;
			g_stImageFile_2_0.uiFileSize += (u32_t)(pstData->usLen);
		}
	}

	if ((1 == ucCnt)||(512 > usLen-4))
	{
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header_2_0(OAM_EXT_CODE_DOWNLOAD);
		pucPkt = &out_data[0];
		pucPkt += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)pucPkt;
		pstDownHd->ucType = OAM_SOFT_TYPE_TFTP;
		pstDownHd->usLen = 0x09;
		dalPonLlidGet((u32_t *)&(pstDownHd->usTid));
		*(u16_t *)(pstDownHd->aucMsg) = TFTP_ACK;
		*(u16_t *)&(pstDownHd->aucMsg[2]) = g_stImageFile_2_0.usBlock2;
		pucPkt += 9;
		pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)pucPkt;
		pstDownHd->ucType = OAM_SOFT_TYPE_TFTP;
		pstDownHd->usLen = 0x09;
		dalPonLlidGet((u32_t *)&(pstDownHd->usTid));
		*(u16_t *)(pstDownHd->aucMsg) = TFTP_ACK;
		*(u16_t *)&(pstDownHd->aucMsg[2]) = g_stImageFile_2_0.usBlock1;
		oam_pdu_padding(&out_data[40], 24);
		OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
		/*printf("file data block %d and %d acked\n", g_stImageFile.usBlock2,
						g_stImageFile.usBlock1);*/
	}

	if (512 > usLen-4)
	{
		eopl_ctc_gen_file_2_0();
		/*write flash*/
		eopl_ctc_write_flash_2_0();
	}

	return 0;
}

OPL_STATUS eopl_ctc_down_req_2_0(u8_t *pucData, u8_t ucFileNameLen)
{
	SOFT_DOWNLOAD_HD_MASK_t *pstDownHd;
	u8_t *pucPkt;

	printf("file name len: %d\n", ucFileNameLen);
	if (!memcmp(pucData, g_stImageFile_2_0.aucFileName+FILENAME_OFFSET, ucFileNameLen)
		&& (g_stImageFile_2_0.uiFileSize==*(u32_t *)(pucData+ucFileNameLen+4))
		&& (1/* crc check */)
		/*&& (1 == g_stImageFile.ucFlash)*/)
	{
		printf("crc all match\n");
		memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
		eopl_ctc_header_2_0(OAM_EXT_CODE_DOWNLOAD);
		pucPkt = &out_data[0];
		pucPkt += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)pucPkt;
		pstDownHd->ucType = OAM_SOFT_TYPE_CRC;
		pstDownHd->usLen = 0x09 + g_stImageFile_2_0.ucNameLen;
		dalPonLlidGet((u32_t *)&(pstDownHd->usTid));
		*(u16_t *)(pstDownHd->aucMsg) = TFTP_END_DOWNLOAD_RSP;
		memcpy(pucPkt+7, g_stImageFile_2_0.aucFileName, g_stImageFile_2_0.ucNameLen);
		*(pucPkt+pstDownHd->usLen-1)=DOWN_RSP_OK_BURN;
		oam_pdu_padding(&out_data[22+pstDownHd->usLen], 42-pstDownHd->usLen);
		OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
		printf("crc acked.\n");
	}

	return 0;
}

OPL_STATUS eopl_ctc_tftp_2_0(u8_t *pucData, u16_t usLen, u8_t ucCnt)
{
	switch(*(u16_t *)pucData)
	{
		case TFTP_R_REQ:
			break;
		case TFTP_W_REQ:
			eopl_ctc_tftp_wreq_2_0(pucData+2, strlen((char *)(pucData+2)));
			break;
		case TFTP_DATA:
			eopl_ctc_tftp_data_2_0(pucData+2, usLen, ucCnt);
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

OPL_STATUS eopl_ctc_crc_2_0(u8_t *pucData, u16_t usLen)
{
	switch(*(u16_t *)pucData)
	{
		case TFTP_END_DOWNLOAD_REQ:
			eopl_ctc_down_req_2_0(pucData+2, usLen-15);
			break;
		case TFTP_END_DOWNLOAD_RSP:
			break;
		default:
			OAMDBG(("\nno such tftp cmd\n"));
			break;
	}

	return 0;
}

OPL_STATUS eopl_ctc_image_mark_2_0(void)
{
	u8_t					*pucPkt;
	OPL_STATUS				iStatus=OPL_OK;
	SOFT_DOWNLOAD_HD_MASK_t	*pstDownHd;

	memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header_2_0(OAM_EXT_CODE_DOWNLOAD);
	pucPkt = &out_data[0];
	pucPkt += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)pucPkt;
	pstDownHd->ucType = OAM_SOFT_TYPE_LOAD;
	pstDownHd->usLen = 0x08;
	dalPonLlidGet((u32_t *)&(pstDownHd->usTid));
	*(u16_t *)(pstDownHd->aucMsg) = TFTP_ACT_IMG_RSP;
	pstDownHd->aucMsg[2] = 0;

	oam_pdu_padding(&out_data[22+pstDownHd->usLen], 42-pstDownHd->usLen);
	OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
	printf("mark acked.\n");
	return iStatus;
}

OPL_STATUS eopl_ctc_image_reboot_2_0(void)
{
	u8_t					*pucPkt;
	OPL_STATUS				iStatus=OPL_OK;
	SOFT_DOWNLOAD_HD_MASK_t	*pstDownHd;

	memset(out_data, 0, OAM_MAXIMUM_PDU_SIZE);
	eopl_ctc_header_2_0(OAM_EXT_CODE_DOWNLOAD);
	pucPkt = &out_data[0];
	pucPkt += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	pstDownHd = (SOFT_DOWNLOAD_HD_MASK_t *)pucPkt;
	pstDownHd->ucType = OAM_SOFT_TYPE_LOAD;
	pstDownHd->usLen = 0x08;
	dalPonLlidGet((u32_t *)&(pstDownHd->usTid));
	*(u16_t *)(pstDownHd->aucMsg) = TFTP_ACT_IMG_RSP;
	pstDownHd->aucMsg[2] = 0;

	oam_pdu_padding(&out_data[22+pstDownHd->usLen], 42-pstDownHd->usLen);
	OamFrameSend(out_data, OAM_MINIMIZE_PDU_SIZE);
	printf("reboot acked.\n");
	return iStatus;
}

OPL_STATUS eopl_ctc_act_req_2_0(u8_t ucFlag)
{
	OPL_STATUS	iStatus=OPL_OK;
	switch(ucFlag)
	{
		case ACTIVE_MARK:
			eopl_ctc_image_mark_2_0();
			break;
		case ACTIVE_REBOOT:
			eopl_ctc_image_reboot_2_0();
			break;
		default:
			OAMDBG(("\nno such flag\n"));
			break;
	}

	return iStatus;
}

OPL_STATUS eopl_ctc_load_2_0(u8_t *pucData)
{
	switch(*(u16_t *)pucData)
	{
		case TFTP_ACT_IMG_REQ:
			eopl_ctc_act_req_2_0(*(pucData+2));
			break;
		case TFTP_ACT_IMG_RSP:
			break;
		default:
			OAMDBG(("\nno such tftp cmd\n"));
			break;
	}

	return 0;
}

OPL_STATUS eopl_ctc_soft_download_2_0(u8_t *pucData)
{
	u8_t ucCnt;

	for (ucCnt=0; ucCnt<2; ucCnt++)
	{
		switch(*pucData)
		{
			case OAM_SOFT_TYPE_TFTP:
				eopl_ctc_tftp_2_0(pucData+5, *(u16_t *)(pucData+1)-5, ucCnt);
				pucData += *(u16_t *)(pucData+1);
				break;
			case OAM_SOFT_TYPE_CRC:
				eopl_ctc_crc_2_0(pucData+5, *(u16_t *)(pucData+1));
				pucData += *(u16_t *)(pucData+1);
				break;
			case OAM_SOFT_TYPE_LOAD:
				eopl_ctc_load_2_0(pucData+5);
				pucData += *(u16_t *)(pucData+1);
				break;
			default:
				return 0;
		}
	}

	return 0;
}

OPL_STATUS eopl_ctc_authen_2_0(u8_t *pucData)
{
	OPL_STATUS	iStatus=0;

	return iStatus;
}
void eopl_ctc_pdu_process_2_0(u8_t *p_byte,u16_t length)
{
	ctc_header_t	*p_ctc;
	oampdu_hdr_t	*p_hdr;
	UINT8 enable = FALSE;

	p_hdr = (oampdu_hdr_t*)p_byte;
	p_byte += sizeof(struct oampdu_hdr);
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
			    eopl_ctc_get_req_2_0(p_byte);
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
			    eopl_ctc_set_req_2_0(p_byte);
			}
			break;
		case OAM_EXT_CODE_AUTHENTICATION:
			eopl_ctc_authen_2_0(p_byte);
	  	    break;
		case OAM_EXT_CODE_DOWNLOAD:
			eopl_ctc_soft_download_2_0(p_byte);
			break;
		case OAM_EXT_CODE_CHURNING:
			eopl_ctc_churning_2_0(p_byte);
			break;
		case OAM_EXT_CODE_DBA:
			eopl_ctc_dba_2_0(p_byte);
			break;
		case OAM_OPL_OPCODE_AES:
			eopl_aeskey_req_2_0(p_byte);
			break;
		case OAM_OPL_OPCODE_AES_SCB:
			eopl_aesscbkey_req_2_0(p_byte);
			break;
		case OAM_OPL_OPCODE_TRI_SCB:
			eopl_TriScbkey_req_2_0(p_byte);
			break;
		default:
			break;
	}
}

