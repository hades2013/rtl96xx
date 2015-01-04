/*=============================================================================
FILE - oam.c

DESCRIPTION
	Implements the 802.3 and CTC oam stack

MODIFICATION DETAILS
=============================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include "oam.h"
#include "opl_driver.h"
#include "opl_host.h"
//wfxu #include "bridgeport.h"
#include <vos.h>
#include "log.h"
#include "stats.h"
//wfxu #include "hw_interface.h"
#include "hal_oam.h"
#include "hal.h"
#include "version.h"
#include "zte.h"
#include "opconn_usr_ioctrl.h"
#include "oam_lw.h"
#include "lw_drv_pub.h"
#include "lw_drv_req.h"


#define MODULE MOD_OAM
oam_sate_t oams;

/*EricYang add for the OAM version 2.0&2.1 merging */
UINT32 oam_ver_type;
UINT8  oam_cfg_ver_type = OAM_VER_CTC_20_21_MIX_SUPPORT;
/*!EricYang */

/* EricYang add for EVENT callback */
#ifdef OAM_SUPPORT_REG_CALLBACK
HASH_TABLE_t * oam_cb_event_table = NULL;
oam_process_entry oam_private_entry = NULL;
u8_t    oam_private_oui[3] = {0x00,0x00,0x00};
/* to save the send handler */
oam_send_handler oam_snd_cb = NULL;
/*!EricYang */
#endif
u8_t 	oam_broadcast_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
u8_t	oam_src_mac[6] = {0x00, 0x07, 0x49, 0x08, 0x30, 0xc8};
u8_t	oam_dest_mac[6] = {0x01,0x80,0xc2,0x00,0x00,0x02};
u8_t	oam_length_type[2] = {0x88,0x09};
u8_t	oam_oui[3] = {0x11,0x11,0x11};
#if 1
u8_t	oam_onu_vendor[4] = {0x4F,0x50,0x4C,0x00};
u8_t	oam_onu_model[4] = {0x0,0x0,0x07,0x04};
#endif
u8_t	oam_chip_vendor[2] = {0x45,0x30};
u8_t	oam_chip_model[2] = {0x67,0x50};
u8_t	oam_chip_revision = 0xA0;
u8_t	oam_chip_version[3] = {0x08, 0x03, 0x1B};
u8_t	oam_firm_ver[FIRM_LENGTH]={0x01, 0x00};
u8_t	oam_onu_hwarever[8] = {0x41,0x30};
u8_t	oam_onu_swarever[16] = {0x30,0x2E,0x30,0x2E,0x31,0x39};
u8_t	ctc_oui[3] = {0x11, 0x11, 0x11};
u8_t	fiberhome_oui[3] = {0x00, 0x0a, 0xc2};
u8_t    zte_oui[3] = {0x00, 0x0F, 0x3E};

u8_t    alarm_out_data[OAM_MAXIMUM_PDU_SIZE+1];
u8_t	out_data[OAM_MAXIMUM_PDU_SIZE+4];
u8_t	aucTimeOut[OAM_MAXIMUM_PDU_SIZE+4];
u8_t	in_data[OAM_MAXIMUM_PDU_SIZE+4];
u16_t   in_data_len = 0;
static oam_info_tlv_t eopl_remote_info;

extern u16_t g_usOutDataIdx;
OLT_FLAG_E m_enOltFlag = OLT_FLAG_CTC;

volatile u8_t oamtransmitevent = TRUE;
/*static bool_t local_link_timer_running;*/

int g_alarm_seq_no;
u8_t g_ucFrmDbgDis=0;
sem_t g_iTimeSem;
TIMER_UNIT_t g_stLinkLost;
TIMER_UNIT_t g_stPduSend;
u8_t g_ucPduMax;
u8_t g_ucOamActiveSend;
u8_t g_ucDbgOamSendNum=1;
extern int g_iOamSetDbg;
/*extern TIMER_UNIT_t g_stStatsTimer;*/

int g_iOamAuthCode = 0;

/* TX power supply control PDU */
u8_t g_tx_power_set_pdu[7] = {0x11, 0x11, 0x11, 0x03, 0xc7, 0x00, 0xa1};

static u8_t flag48Found = 0;
#define MAX_FLAG_8_FOUND_CNT 5
static u8_t flag8FoundCnt = 0;


static u32_t    m_ulLast_frame_err, m_ulLast_frame_period_err;
static u32_t    m_ulLast_frame_ok;
static u32_t    m_ulCurr_frame_err;
static u32_t    m_ulFrame_ok_curr, m_ulsec_err_curr, m_ulLast_sec_err;
static u64_t    m_ullSym_ok_curr, m_ullLast_sym_ok;
static u64_t    m_ullSym_err_curr, m_ullLast_sym_err;
static u8_t     m_ucCurOamState, m_ucLastOamState;
static u16_t    m_usframe_win_curr, m_usframe_sec_win_curr;
static unsigned short   time_stamp, sequence_number;

static u32_t   m_aulOam_err_evt[OAM_THRSD_TYPE_END];
alarm_oam_window_t g_stOamAlarmWindow;
alarm_oam_thrsh_t  g_stOamAlarmThrsh;


static attr_statistics_t m_stOamStatData;
OLT_FLAG_E OamGetOltFlag(void)
{
    return m_enOltFlag;
}

static void OamSetOltFlag(OLT_FLAG_E enOltFlag)
{
    if (enOltFlag >= OLT_FLAG_BUTT)
    {
        return;
    }

    m_enOltFlag = enOltFlag;

    return;
}

/*****************************************************************************
 
*****************************************************************************/
void OamPeriodHandle(void)
{
    u64_t ullTmpU64Window = 0;
    u64_t ullTmpU64Thrsh = 0;
    u32_t ulTmpU32Window  = 0;
    u32_t ulTmpU32Thrsh  = 0;
    u8_t  *p_byte   = NULL;   
    oam_envet_hdr_t *pstEventHdr = NULL;
    oam_err_sym_period_hdr_t *pstSymbolEvt = NULL;
    oam_err_frm_period_hdr_t *pstFrameErrEvt = NULL;
    oam_err_frm_hdr_t        *pstFrameEvt = NULL;
    oam_err_frm_sec_hdr_t    *pstFrameSecEvt = NULL;
    u16_t usPktLen = 0;
	OPL_CNT_t astPonCounter[DAL_CNT_PON_NUM];
    
    time_stamp++;

    m_ucLastOamState = m_ucCurOamState;
    m_ucCurOamState = oams.state; 
    
    memset(alarm_out_data, 0, OAM_MAXIMUM_PDU_SIZE);
    p_byte = &alarm_out_data[0];
    eopl_oam_pdu_hdr(p_byte, oams.flags, OAM_CODE_EVENT);
	p_byte += sizeof(alarm_oampdu_hdr_t);
    usPktLen += sizeof(alarm_oampdu_hdr_t);

    if (OPL_OK != dalCounterGet(DAL_CNT_POLL_PON_MASK, astPonCounter, 0, DAL_CNT_PON_NUM))
	{
	    return;
	}
	
    vosMemCpy(&m_ullSym_ok_curr, &astPonCounter[STAT_PON_RCV_BITS_OK].stAdded, sizeof(m_ullSym_ok_curr));
    ullTmpU64Window = m_ullSym_ok_curr - m_ullLast_sym_ok;
    
    vosMemCpy(&m_ullSym_err_curr, &astPonCounter[STAT_PON_RCV_BITS_ERR].stAdded, sizeof(m_ullSym_err_curr));
    ullTmpU64Thrsh = m_ullSym_err_curr - m_ullLast_sym_err;    

    ///////////////////////////////////////////////
    // window of symbol error
    ///////////////////////////////////////////////
    if (ullTmpU64Window >= g_stOamAlarmWindow.ullSym_err_prd_window)
    {
        m_ullLast_sym_ok = m_ullSym_ok_curr;
        m_ullLast_sym_err = m_ullSym_err_curr;
        if (ullTmpU64Thrsh >= g_stOamAlarmThrsh.ullSym_err_prd_thrsh)
        {
            m_aulOam_err_evt[OAM_THRSD_ERRORED_SYM_PRD]++;                      
            pstEventHdr = (oam_envet_hdr_t *)p_byte;
            pstEventHdr->event_type = SYM_ERROR_TYPE;
            pstEventHdr->event_len = SYM_ERROR_LEN;
            pstEventHdr->event_stamp = time_stamp * 10;
            p_byte += sizeof(oam_envet_hdr_t);
                
            pstSymbolEvt = (oam_err_sym_period_hdr_t *)p_byte;
            pstSymbolEvt->sym_window = g_stOamAlarmWindow.ullSym_err_prd_window;
            pstSymbolEvt->sym_thrsh = g_stOamAlarmThrsh.ullSym_err_prd_thrsh;
            pstSymbolEvt->sym_err_value = ullTmpU64Thrsh;
            pstSymbolEvt->err_total = m_ullSym_err_curr;
            pstSymbolEvt->event_total = m_aulOam_err_evt[OAM_THRSD_ERRORED_SYM_PRD];
            p_byte += sizeof(oam_err_sym_period_hdr_t);

            usPktLen += SYM_ERROR_LEN;
        }          
    }
    
    ///////////////////////////////////////////////
    // window of frame period error
    ///////////////////////////////////////////////
    m_ulFrame_ok_curr = astPonCounter[STAT_PON_RCV_PKT_OK].stAdded.uiLow;
    ulTmpU32Window = m_ulFrame_ok_curr - m_ulLast_frame_ok;
    
    m_ulCurr_frame_err = astPonCounter[STAT_PON_RCV_FRM_CRC].stAdded.uiLow;
    ulTmpU32Thrsh = m_ulCurr_frame_err - m_ulLast_frame_period_err;
    if (ulTmpU32Thrsh > 0)
    {
        m_ulsec_err_curr++;
    }
    
    if (ulTmpU32Window >= g_stOamAlarmWindow.ulFrm_err_prd_window)
    {
        m_ulLast_frame_ok = m_ulFrame_ok_curr;
        m_ulLast_frame_period_err = m_ulCurr_frame_err;
        if (ulTmpU32Thrsh >= g_stOamAlarmThrsh.ulFrm_err_prd_thrsh)
        {
            m_aulOam_err_evt[OAM_THRSD_ERRORED_FRAME_PRD]++;
                       
            pstEventHdr = (oam_envet_hdr_t *)p_byte;
            pstEventHdr->event_type = FRM_PRD_TYPE;
            pstEventHdr->event_len = FRM_PRD_LEN;
            pstEventHdr->event_stamp = time_stamp * 10;
            p_byte += sizeof(oam_envet_hdr_t);
                
            pstFrameErrEvt = (oam_err_frm_period_hdr_t *)p_byte;
            pstFrameErrEvt->frm_window = g_stOamAlarmWindow.ulFrm_err_prd_window;
            pstFrameErrEvt->frm_thrsh = g_stOamAlarmThrsh.ulFrm_err_prd_thrsh;
            pstFrameErrEvt->frm_err_value = ulTmpU32Thrsh;
            pstFrameErrEvt->err_total = m_ulCurr_frame_err;
            pstFrameErrEvt->event_total = m_aulOam_err_evt[OAM_THRSD_ERRORED_FRAME_PRD];
            p_byte += sizeof(oam_err_frm_period_hdr_t);

            usPktLen += FRM_PRD_LEN;
        }
    }

    ///////////////////////////////////////////////
    // window of frame error 
    ///////////////////////////////////////////////
    if (m_usframe_win_curr > 0)
    {
        m_usframe_win_curr--;
    }
    else
    {  
        /*单位是100ms*/
        m_usframe_win_curr = g_stOamAlarmWindow.usErr_frm_window / 10; 
        ulTmpU32Thrsh = m_ulCurr_frame_err - m_ulLast_frame_err;
        m_ulLast_frame_err = m_ulCurr_frame_err;
        if (ulTmpU32Thrsh >= g_stOamAlarmThrsh.ulErr_frm_thrsh)
        {   
            m_aulOam_err_evt[OAM_THRSD_ERRORED_FRAME]++;
            
            pstEventHdr = (oam_envet_hdr_t *)p_byte;
            pstEventHdr->event_type = FRM_ERROR_TYPE;
            pstEventHdr->event_len = FRM_ERROR_LEN;
            pstEventHdr->event_stamp = time_stamp * 10;
            p_byte += sizeof(oam_envet_hdr_t);

            pstFrameEvt = (oam_err_frm_hdr_t *)p_byte;
            pstFrameEvt->frm_interval_window = g_stOamAlarmWindow.usErr_frm_window;
            pstFrameEvt->frm_thrsh = g_stOamAlarmThrsh.ulErr_frm_thrsh;
            pstFrameEvt->frm_err_value = ulTmpU32Thrsh;
            pstFrameEvt->err_total = m_ulCurr_frame_err;
            pstFrameEvt->event_total = m_aulOam_err_evt[OAM_THRSD_ERRORED_FRAME];
            p_byte += sizeof(oam_err_frm_hdr_t);
            usPktLen += FRM_ERROR_LEN;
        }        
    }
    
    ///////////////////////////////////////////////
    // window of frame sec summury error 
    ///////////////////////////////////////////////
    if (m_usframe_sec_win_curr > 0)
    {
        m_usframe_sec_win_curr--;
    }
    else
    {   
        /*单位是100ms*/
        m_usframe_sec_win_curr = g_stOamAlarmWindow.usErr_frm_sec_window / 10;
        ulTmpU32Thrsh = m_ulsec_err_curr - m_ulLast_sec_err;
        m_ulLast_sec_err = m_ulsec_err_curr;
		/* 上电后错包较多，因此第一次不报 */
        if ((ulTmpU32Thrsh >= g_stOamAlarmThrsh.usErr_frm_sec_thrsh) && (time_stamp <= m_usframe_sec_win_curr))
        {
            m_aulOam_err_evt[OAM_THRSD_ERRORED_FRM_SEC]++;           
            
            pstEventHdr = (oam_envet_hdr_t *)p_byte;
            pstEventHdr->event_type = SEC_ERROR_TYPE;
            pstEventHdr->event_len = SEC_ERROR_LEN;
            pstEventHdr->event_stamp = time_stamp * 10;
            p_byte += sizeof(oam_envet_hdr_t);

            pstFrameSecEvt = (oam_err_frm_sec_hdr_t *)p_byte;
            pstFrameSecEvt->frm_sec_window = g_stOamAlarmWindow.usErr_frm_sec_window;
            pstFrameSecEvt->frm_sec_thrsh = g_stOamAlarmThrsh.usErr_frm_sec_thrsh;
            pstFrameSecEvt->frm_sec_err_value = ulTmpU32Thrsh;
            pstFrameSecEvt->sec_err_total = m_ulsec_err_curr;
            pstFrameSecEvt->event_total = m_aulOam_err_evt[OAM_THRSD_ERRORED_FRAME];
            p_byte += sizeof(oam_err_frm_hdr_t);
            usPktLen += FRM_ERROR_LEN;
        }        
    }
    
    /* send alarm only if oam is established */
    if ((usPktLen > sizeof(oampdu_hdr_t) + sizeof(u16_t)) 
        && (EX_DISCOVERY_SEND_ANY == m_ucCurOamState)
        && (EX_DISCOVERY_SEND_ANY == m_ucLastOamState)
        && (time_stamp > 10))
    {
        if(usPktLen < OAM_MINIMIZE_PDU_SIZE)
    	{
    		oam_pdu_padding(&alarm_out_data[usPktLen],	OAM_MINIMIZE_PDU_SIZE - usPktLen);
    		usPktLen = OAM_MINIMIZE_PDU_SIZE;
    	}
        p_byte = &alarm_out_data[0];
        ((alarm_oampdu_hdr_t*)p_byte)->seqno = g_alarm_seq_no++;

		OamFrameSend(alarm_out_data, usPktLen);	
    }  

    return;
}

/*****************************************************************************
 
*****************************************************************************/
void OamClrPeirodStats(void)
{
    m_ulLast_frame_err = 0; 
    m_ulLast_frame_period_err = 0;
    m_ulLast_frame_ok = 0; 
    m_ullLast_sym_err = 0;
    m_ulCurr_frame_err = 0; 
    m_ulFrame_ok_curr = 0; 
    m_ullSym_ok_curr = 0;
    m_ullLast_sym_ok = 0;
    m_ullSym_err_curr = 0;
    m_ulsec_err_curr = 0;
    m_ulLast_sec_err = 0;

    /*单位是100ms*/
    m_usframe_win_curr = g_stOamAlarmWindow.usErr_frm_window/10;
    m_usframe_sec_win_curr = g_stOamAlarmWindow.usErr_frm_sec_window/10;
    
    return;
}


/*****************************************************************************
                                        
*****************************************************************************/
void InitOamAlarmParam(void)
{
    g_stOamAlarmWindow.ullSym_err_prd_window = OAM_DEFAULT_ERR_SYMBOL_WINDOW;
    g_stOamAlarmThrsh.ullSym_err_prd_thrsh = OAM_DEFAULT_ERR_SYMBOL_THRESH;
    g_stOamAlarmWindow.ulFrm_err_prd_window = OAM_DEFAULT_ERR_FR_PERIOD_WINDOW;
    g_stOamAlarmThrsh.ulFrm_err_prd_thrsh = OAM_DEFAULT_ERR_FR_PERIOD_THRESH;
    g_stOamAlarmWindow.usErr_frm_window = OAM_DEFAULT_ERR_FRAME_WINDOW;
    g_stOamAlarmThrsh.ulErr_frm_thrsh = OAM_DEFAULT_ERR_FRAME_THRESH;
    g_stOamAlarmWindow.usErr_frm_sec_window = OAM_DEFAULT_ERR_SEC_SUMMRY_WINDOW;
    g_stOamAlarmThrsh.usErr_frm_sec_thrsh = OAM_DEFAULT_ERR_SEC_SUMMRY_THRESH;

    /*单位是100ms*/
    m_usframe_win_curr = g_stOamAlarmWindow.usErr_frm_window/10;
    m_usframe_sec_win_curr = g_stOamAlarmWindow.usErr_frm_sec_window/10;
    return;             
}


int OamDbgSendNumSet (u8_t ucNum)
{
	int	iStatus = EOPL_OK;

	g_ucDbgOamSendNum = ucNum;

	return iStatus;
}

int OamDbgActiveEnable (u8_t ucEnable)
{
	int	iStatus = EOPL_OK;

	g_ucOamActiveSend = ucEnable;

	return iStatus;
}

int OamDbgEnable (u8_t ucEnable)
{
	int iStatus = EOPL_OK;

	g_ucFrmDbgDis = ucEnable;

	return iStatus;
}

int eopl_oam_chk_satisfied(u8_t *pucPkt)
{
	oam_info_tlv_t *pstRInfo=(oam_info_tlv_t *)pucPkt;;

	if(pstRInfo->oam_version != oams.ucOamVersion)
	{
		return 0;
	}
	else if(pstRInfo->oampdu_config != oams.oampdu_config)
	{
		return 0;
	}
	/*else if(pstRInfo->oam_config != oams.oam_config)
	{
		return 0;
	}*/

	return 1;
}

int eopl_oam_link_request(bool_t *pucLinkStatus)
{
	int iStatus=EOPL_OK;

	*pucLinkStatus = LINK_OK;

	return iStatus;
}

/******************************************************************************
*
* eopl_oam_pdu_receive - receive oam pdu from low level layer
*
* DESCRIPTION:
*     This function is called by low level layer to receive oam pdu, and to
* store the pdu in global buffer in_data[].
*
* INPUT:
*     pucFrameIn - pointer to frame received
*     usFrameLen - length of the frame received
*
* OUTPUT:
*     NULL
*
* RETURN:
*     PARAMETER_INPUT_POINTER_NULL - The input parameter is NULL.
*     PARAMETER_RANGE_ERROR        - The range of the parameter is wrong.
*     EOPL_OK                      - The operation is successful.
*/
int eopl_oam_pdu_receive (u8_t *pucFrameIn, u16_t usFrameLen)
{
	int		iStatus=EOPL_OK;
	u32_t	uiIdx;
	u8_t	*pucFrmTmp=pucFrameIn+4;
	oampdu_hdr_t	*p_hdr;
	oam_info_tlv_t	*p_info;

	if(NULL == pucFrameIn)
	{
		return PARAMETER_INPUT_POINTER_NULL;
	}

	if((OAM_MINIMIZE_PDU_SIZE + 4 > usFrameLen)
		|| (OAM_MAXIMUM_PDU_SIZE+4 < usFrameLen))
	{
		return PARAMETER_RANGE_ERROR;
	}

	p_hdr = (oampdu_hdr_t*)pucFrmTmp;
	p_info = (oam_info_tlv_t*)(pucFrmTmp+sizeof(struct oampdu_hdr));

	if((1 == g_ucFrmDbgDis)&&((oams.state<EX_DISCOVERY_SEND_ANY)
		||((EX_DISCOVERY_SEND_ANY == oams.state)&&(OAM_CODE_INFO != p_hdr->pcode))))
	/*if((1 == g_ucFrmDbgDis) && (((OAM_CODE_INFO != p_hdr->pcode)
		||(0 == oams.usRInfoTlvVer)	|| (oams.usRInfoTlvVer<p_info->revision))))*/
	{
		printf("==========receive payload length: %d===========", usFrameLen);
		for(uiIdx=0; uiIdx<usFrameLen; uiIdx++)
		{
			if(0 == uiIdx%16)
			{
				printf("\n0x%08x\t", uiIdx);
			}
			printf(" 0x%02x", *pucFrmTmp++);
		}
		printf("\n--------------------------------------\n");
	}

	g_stLinkLost.uiCnt = 0;

	vosMemCpy (in_data, pucFrameIn, usFrameLen);
	eopl_oam_pdu_process(usFrameLen);

	return iStatus;
}

int OamFrameSend(u8_t *pucFrame, u16_t usFrmLen)
{
	int		iStatus=EOPL_OK;
	u16_t	usIdx;
	u8_t	*pucTmp=pucFrame;
	u8_t	aucPkt[1518]={0};
	host_outbound_hdr_t	*pstHdr;
	oampdu_hdr_t		*p_hdr;

	p_hdr = (oampdu_hdr_t*)pucFrame;
/* 014025 */
    #if 0
	if((1 == g_ucFrmDbgDis)&&((oams.state<EX_DISCOVERY_SEND_ANY)
		||((EX_DISCOVERY_SEND_ANY == oams.state)&&(OAM_CODE_INFO != p_hdr->pcode))))
	{
		printf("********** payload length: %d ***********", usFrmLen);
		for(usIdx=0; usIdx<usFrmLen; usIdx++)
		{
			if(0 == usIdx%16)
			{
				printf("\n0x%08x\t", usIdx);
			}
			printf(" 0x%02x", *pucTmp++);
		}
		printf("\n#####################################\n");
	}
    #endif

	pstHdr = (host_outbound_hdr_t *)(&aucPkt[0]);
	pstHdr->eport = PORT_PON_LOGIC;
	pstHdr->icos = 7;
	vosMemCpy(&aucPkt[1], pucFrame, usFrmLen);
#ifdef OAM_SUPPORT_REG_CALLBACK
    if (oam_snd_cb == NULL) {
        iStatus = eopl_host_send(aucPkt, usFrmLen+1);
        if(0 != iStatus)
        {
            /*OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "send error!\n");*/
	    hostCnt.oamTxFail++;
            return iStatus;
        }
    }else{
        iStatus = oam_snd_cb(aucPkt, usFrmLen+1);

        if(0 != iStatus)
        {
            /*OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "send error!\n");*/
	    hostCnt.oamTxFail++;
            return iStatus;
        }

    }
#else
        iStatus = eopl_host_send(aucPkt, usFrmLen+1);
        if(0 != iStatus)
        {
            /*OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "send error!\n");*/
	    hostCnt.oamTxFail++;
            return iStatus;
        }

#endif

	hostCnt.oamTx++;
	/*g_ucPduMax++;*/

	return iStatus;
}

int OamPduEncode(u8_t *pucSrcAdd, u16_t usFlag, u8_t ucCode, u8_t *pucData)
{
	int	iStatus=EOPL_OK;
	oampdu_hdr_t *pstOamHdr=(oampdu_hdr_t *)pucData;

	vosMemCpy(pstOamHdr->dest, oam_dest_mac, 6);
	vosMemCpy(pstOamHdr->src, pucSrcAdd, 6);
	pstOamHdr->length[0] = 0x88;
	pstOamHdr->length[1] = 0x09;
	pstOamHdr->subtype = 3;
	pstOamHdr->flags = usFlag;
	pstOamHdr->pcode = ucCode;

	return iStatus;
}

/******************************************************************************
*
* eopl_oam_pdu_request - send oam pdu to oam sublayer
*
* DESCRIPTION:
*     This function is called by oam client to send oam pdu to oam sublayer,
* and the pdu is in global buffer out_data[].
*
* INPUT:
*     pucOamSrcMac - pointer to source mac address
*     usOamFlag    - flag used to encode into OAM frame header
*     ucOamCode    - code used to encode into OAM frame header
*     pucData      - pointer to data which used to encode into OAM frame header
*
* OUTPUT:
*     NULL
*
* RETURN:
*     PARAMETER_INPUT_POINTER_NULL - The input parameter is NULL.
*     EOPL_OK                      - The operation is successful.
*/
int eopl_oam_pdu_request(u8_t *pucOamSrcMac, u16_t usOamFlag, u8_t ucOamCode, u8_t *pucData, u16_t usLen)
{
	int	iStatus=EOPL_OK;

	if((NULL == pucOamSrcMac) || (NULL == pucData))
	{
		return PARAMETER_INPUT_POINTER_NULL;
	}

	iStatus = OamPduEncode(pucOamSrcMac, usOamFlag, ucOamCode, pucData);
	if(0 != iStatus)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "pdu encode error\n");
		return 0;
	}

	iStatus = OamFrameSend(pucData, usLen);
	if(0 != iStatus)
	{
		/*OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "frame send error\n");*/
		return 0;
	}

	return iStatus;
}

/*
  * Function:
  * eopl_oam_info_tlv
  * Purpose:
  * routine to set the oam local info tlv
  * Parameters:
  * Input - ptr,type
  *			ptr - pointer to the data buffer of the local info
  *			type - type to be set for this lcoal info
  * Output  - none
  * Returns:
  *     None
  */
void eopl_oam_info_tlv(u8_t *ptr, u8_t type)
{
	oam_info_tlv_t	*p_info;

	p_info = (oam_info_tlv_t*)ptr;
	p_info->info_type = type;
	p_info->info_length = 0x10;
	p_info->oam_version = 0x01;
	p_info->revision = oams.usLInfoTlvVer++;
	p_info->state = oams.action_state;
	p_info->oam_config = oams.oam_config;
	p_info->oampdu_config = oams.oampdu_config;
	vosMemCpy(p_info->oui, oam_oui, 3);
	vosMemCpy(p_info->vendor_info, oam_onu_vendor, 4);
}

void eopl_oam_remote_info_tlv(u8_t *ptr, u8_t type)
{
	oam_info_tlv_t	*p_info;

	p_info = (oam_info_tlv_t*)ptr;
	p_info->info_type = type;
	p_info->info_length = 0x10;
	p_info->oam_version = oams.ucRmtOamVer;
	p_info->revision = oams.usRInfoTlvVer;
	p_info->state = oams.remote_action_state;
	p_info->oam_config = oams.remote_oam_config;
	p_info->oampdu_config = oams.remote_oampdu_config;
	vosMemCpy(p_info->oui, oams.aucRmtOui, 3);
	vosMemCpy(p_info->vendor_info, oams.aucRmtVendor, 4);
}

/*
  * Function:
  * eopl_oam_pdu_hdr
  * Purpose:
  * routine to set the oam pdu header
  * Parameters:
  * Input - ptr,flags,pcode
  *			ptr - pointer to the data buffer of pdu header
  *			flags - flags to be set for this pdu header
  *			pcode - code to be set for this pdu header
  * Output  - none
  * Returns:
  *     None
  */
void eopl_oam_pdu_hdr(u8_t *ptr, u16_t flags, u8_t pcode)
{
	oampdu_hdr_t	*p_hdr;

	p_hdr = (oampdu_hdr_t*)ptr;
	vosMemCpy(p_hdr->dest, oam_dest_mac,6);
	vosMemCpy(p_hdr->src, oam_src_mac, 6);
	vosMemCpy(p_hdr->length, oam_length_type, 2);
	p_hdr->subtype = 0x03;
	p_hdr->pcode = pcode;
	p_hdr->flags = flags;
}

/*
  * Function:
  * oam_pdu_padding
  * Purpose:
  * fault state routine of oam state machine
  * Parameters:
  * Input - ptr,len
  *			ptr - pointer to the part to be padded  with zero of the oam pdu
  *			len - len of the part to be padded  with zero of the oam pdu
  * Output  - none
  * Returns:
  *     None
  */
void oam_pdu_padding(u8_t *ptr, u16_t len)
{
	u16_t i;

	for(i=0;i<len;i++)
		*ptr++ = 0;
}

/*
  * Function:
  * eopl_oam_fault_state
  * Purpose:
  * fault state routine of oam state machine
  * Parameters:
  * Input - event
  *			input event for the fault state
  * Output  - none
  * Returns:
  *     None
  */
void eopl_oam_fault_state(enum eopl_oam_event_t event)
{
	u8_t flags;

	flags = oams.flags;
	oams.local_link_timer_running = FALSE;

	if((event == EOPL_LOCAL_LINK_STATUS_CHANGE) && !READ_BITS(flags,0))
	{
		oams.local_pdu = PDU_RX_INFO;
		if(oams.local_oam_mode == OAM_ACTIVE)
		{
			oams.state = DISCOVERY_ACTIVE_SEND_LOCAL;
			oams.local_pdu = PDU_INFO;
		}
		else
		{
			oams.state = DISCOVERY_PASSIVE_WAIT;
			oams.local_pdu = PDU_RX_INFO;
		}
	}
}

/*
  * Function:
  * eopl_oam_timer_process
  * Purpose:
  * timer routine for oam state machine
  * Parameters:
  * Input - none
  * Output  - none
  * Returns:
  *     None
  */
void eopl_oam_timer_process(void)
{
	u8_t	*p_byte;
	u16_t	size;
	oam_info_tlv_t	*p_info;

	p_byte = &out_data[0];
	eopl_oam_pdu_hdr(p_byte, oams.flags, OAM_CODE_INFO);
	p_byte += sizeof(struct oampdu_hdr);

	switch(oams.state)
	{
		case DISCOVERY_ACTIVE_SEND_LOCAL:
			eopl_oam_info_tlv(p_byte, INFO_LOCAL_TYPE);
			size = OAM_MINIMIZE_PDU_SIZE- sizeof(struct oampdu_hdr)-sizeof(struct oam_info_tlv);
			oam_pdu_padding(p_byte,  size);
			break;
		case DISCOVERY_PASSIVE_WAIT:
		case DISCOVERY_SEND_LOCAL_REMOTE:
		case DISCOVERY_SEND_LOCAL_REMOTE_OK:
		case DISCOVERY_SEND_ANY:
			eopl_oam_info_tlv(p_byte, INFO_LOCAL_TYPE);
			p_byte += sizeof(struct oam_info_tlv);
			p_info = (oam_info_tlv_t*)p_byte;
			*p_info = eopl_remote_info;
			p_byte += sizeof(struct oam_info_tlv);
			size = OAM_MINIMIZE_PDU_SIZE- sizeof(struct oampdu_hdr)-2*sizeof(struct oam_info_tlv);
			oam_pdu_padding(p_byte, size);
			break;
		case EX_DISCOVERY_PASSIVE_WAIT:
			break;
		case EX_DISCOVERY_SEND_EXT_INFO:
			break;
		case EX_DISCOVERY_SEND_OAM_EXT_INFO_ACK:
			break;
		case EX_DISCOVERY_SEND_OAM_EXT_INFO_NACK:
			break;
		case EX_DISCOVERY_SEND_ANY_NO_EXT:
			break;
		case EX_DISCOVERY_SEND_ANY:
			break;
		default:
			size = OAM_MINIMIZE_PDU_SIZE- sizeof(struct oampdu_hdr);
			oam_pdu_padding(p_byte, size);
			break;
	}
	/* call hardware interface to tx oam pdu */
}

/*
  * Function:
  * eopl_oam_init
  * Purpose:
  * init routine for oam state machine
  * Parameters:
  * Input - none
  * Output  - none
  * Returns:
  *     None
  */
int OamInit(void)
{
	int		iStatus=EOPL_OK;
	bool_t	ucLinkStatus;
    UINT8   value;

/*
	pthread_t iTimerThread;

	iStatus = pthread_create(&iTimerThread, NULL, (void *)&OamTimerThread, NULL);
	if(EOPL_OK != iStatus)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "oam timer thread created error!\n");
		return 0;
	}
*/
#if defined(CTC_VOIP_DISPATCH)
    oamMsqInit();
#endif

	iStatus = eopl_oam_link_request(&ucLinkStatus);
	if(EOPL_OK != iStatus)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "local link status check error. 0x%08x\r\n", iStatus);
		return iStatus;
	}

    g_iOamAuthCode = 0;

	oams.flags = 0x08;
	oams.ucOamVersion = 0x01;
	oams.usLInfoTlvVer = 0x0004;
	oams.usRInfoTlvVer = 0;
	oams.action_state = 0x06;
	SET_BITS(oams.oam_config,0x14);
	oams.oampdu_config = OAM_MAXIMUM_PDU_SIZE;
	vosMemCpy (oams.aucOui, oam_oui, 3);
	vosMemCpy (oams.aucVendor, oam_onu_vendor, 4);

#if 0
	if(vosStrLen(MAJOR_VERSION) < 16)
		vosStrCpy(oam_onu_swarever, MAJOR_VERSION);
	else
		vosMemCpy(oam_onu_swarever, MAJOR_VERSION, 16);
#endif
	oams.ucExVer = 0x01;
	if(LINK_FAIL == ucLinkStatus)
	{
		oams.local_pdu = PDU_LF_INFO;
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "warning: link fault.\r\n");
	}
	else
	{
		oams.local_pdu = PDU_RX_INFO;
	}
	oams.local_stable = FALSE;
	oams.state = DISCOVERY_FAULT;
	oams.local_link_timer_running = TRUE;

    /*EricYang add for the OAM version 2.0&2.1 merging */
    oam_ver_type = OAM_VER_CTC_2_0_SUPPORT;
    odmPonOAMVerGet(&oam_cfg_ver_type);//OAM_VER_CTC_2_0_SUPPORT;
    /*!EricYang*/
	eopl_oam_state_machine(EOPL_LOCAL_LINK_STATUS_CHANGE);
    /*EricYang to init LIB and event-callback interface */
    //oam_snd_cb = NULL;
    /*!EricYang */
	if (IS_ONU_RESPIN)
	{
		iStatus = oplRegWrite(REG_CP_OAM_REGI_CTRL, 0x00);/*turn off the OAM led*/
		if(OPL_OK != iStatus)
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "register set error. reg:0x0026, value:0x0\n");
		}
	}

	flag8FoundCnt = 0;

	oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 0, 1, 0);
	oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 0, 1, 0);

	return iStatus;
}

/*
  * Function:
  * eopl_oam_pdu_process
  * Purpose:
  * process routine for incoming oam pdu
  * Parameters:
  * Input - none
  * Output  - none
  * Returns:
  *     None
  */
void eopl_oam_pdu_process(u16_t framelen)
{
	int							iStatus;
	u8_t						*p_byte;
	u16_t						flags;
	oampdu_hdr_t				*p_hdr;
	oam_info_tlv_t				*p_info;
	oam_info_tlv_t				*pstRmtInfo;
	EX_OAM_INFO_TLV_t			*pstExInfoTlv;
	HAL_OAM_SEND_FRAME_PAR_t	stOamFramePar;
	OPL_API_DATA_t				stApiData;
    int                         i;
    EX_OAM_INFO_TLV_t           *pstSendTlv;
    u8_t                        *p_out;                
    
	p_byte = &in_data[4];
    in_data_len = framelen - 4;
	p_hdr = (oampdu_hdr_t*)p_byte;
	p_info = (oam_info_tlv_t *)(p_byte+sizeof(struct oampdu_hdr));
	pstRmtInfo = (oam_info_tlv_t *)(p_byte+sizeof(struct oampdu_hdr)+sizeof(oam_info_tlv_t));

	/*set the flags of remote evaluating and stable */
	flags = READ_BITS(p_hdr->flags, 0x18);
	flags <<= 2;
	WRITE_BITS(oams.flags,0x60, flags);

	if(p_hdr->pcode == OAM_CODE_INFO &&
			p_info->info_length == 0x16)
	{
		eopl_remote_info = *p_info;
		eopl_remote_info.info_type = INFO_REMOTE_TYPE;
	}

	if((OAM_CODE_INFO == p_hdr->pcode)
			  && (INFO_LOCAL_TYPE == p_info->info_type)
			  && (0x10 == p_info->info_length)
			  && (INFO_LOCAL_END == pstRmtInfo->info_type)
			  && (0x00 == pstRmtInfo->info_length))
	{
		OamInit();
	}

	/* TX power supply control packet */
	if(OAM_CODE_ORG_SPEC == p_hdr->pcode)
    {
        if(memcmp(g_tx_power_set_pdu, (u8_t *)p_info, sizeof(g_tx_power_set_pdu)) == 0)
        {
            printf("Received TX power supply control packet!\n");
            eopl_ctc_pdu_process(p_byte, framelen-4);
			return;
        }
    }

#if 1
	if((OAM_CODE_INFO == p_hdr->pcode) && (p_hdr->flags == 0x48) && (!flag48Found))
	{
		flag48Found = 1;
		OamInit();
	}
#endif
	if((OAM_CODE_INFO == p_hdr->pcode) && (p_hdr->flags == 0x8))
	{
		if (oams.flags == 0x10)
		{
			flag8FoundCnt++;
		}
		else 
		{
			flag8FoundCnt = 0;
		}
		if (flag8FoundCnt >= MAX_FLAG_8_FOUND_CNT)
		{
			//printf("Continuous flag 8 packet more than %d times!\n",MAX_FLAG_8_FOUND_CNT);
			//system("echo \"Continuous flag 8 packet more than 5 times!\" >> /tmp/trace.log");
			oplRegWrite(REG_MPCP_GATE_TIMEOUT, 1);
			OamInit();
			oplRegWrite(REG_MPCP_GATE_TIMEOUT, 0x03B9ACA0);
		}
	}

	if((OAM_CODE_INFO != p_hdr->pcode)||(0 == oams.usRInfoTlvVer)
		|| (oams.usRInfoTlvVer<p_info->revision)||(DISCOVERY_SEND_ANY<oams.state))
	{
		if(OAM_CODE_INFO == p_hdr->pcode)
		{
			oams.usRInfoTlvVer = p_info->revision;
		}

		switch(oams.state)
		{
			case DISCOVERY_ACTIVE_SEND_LOCAL:
			case DISCOVERY_PASSIVE_WAIT:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nstate: PASSIVE_WAIT\n");
				#if 0
				if((OAM_CODE_INFO == p_hdr->pcode)
				  && (INFO_LOCAL_TYPE == p_info->info_type)
				  && (0x10 == p_info->info_length)
				  && (INFO_LOCAL_END == pstRmtInfo->info_type)
				  && (0x00 == pstRmtInfo->info_length))
				#else
		  /****Fix bug 3571 To IOP with some OLT,after the MPCP register disable the GE port till the OAM is registed */
			    //oplRegWrite(REG_GMAC_CFG_TRANSFER_ENA, 0x02);
				oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 0, 1, 0);
				oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 0, 1, 0);

		  /*****************************Fix bug 3571 end***********************************/
				if((OAM_CODE_INFO == p_hdr->pcode)
				  && (INFO_LOCAL_TYPE == p_info->info_type)
				  && (0x10 == p_info->info_length))
				#endif
				{
					oams.ucRmtOamVer = p_info->oam_version;
					oams.remote_action_state = p_info->state;
					oams.remote_oam_config = p_info->oam_config;
					oams.remote_oampdu_config = p_info->oampdu_config;
					vosMemCpy(oams.aucRmtOui, p_info->oui, 3);
					vosMemCpy(oams.aucRmtVendor, p_info->vendor_info, 4);

					oams.remote_state_valid = TRUE;
					oams.state = DISCOVERY_SEND_LOCAL_REMOTE;
					OamDygaspCfg();
					if(eopl_oam_chk_satisfied((u8_t *)p_info))
					{
						oams.state = DISCOVERY_SEND_LOCAL_REMOTE_OK;
					}
					eopl_oam_state_machine(EOPL_OAM_PDU_RX);
				}
				break;
			case DISCOVERY_SEND_LOCAL_REMOTE:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nstate: SEND_LOCAL_REMOTE\n");
				if((OAM_CODE_INFO == p_hdr->pcode)
				 && (INFO_LOCAL_TYPE == p_info->info_type)
				 && (0x10 == p_info->info_length))
				{
					oams.remote_state_valid = TRUE;
					oams.local_satisfied = TRUE;
					oams.state = DISCOVERY_SEND_LOCAL_REMOTE_OK;
		/*** Update the remote infomation according to the received info */
					oams.ucRmtOamVer = p_info->oam_version;
					oams.remote_action_state = p_info->state;
					oams.remote_oam_config = p_info->oam_config;
					oams.remote_oampdu_config = p_info->oampdu_config;
					vosMemCpy(oams.aucRmtOui, p_info->oui, 3);
					vosMemCpy(oams.aucRmtVendor, p_info->vendor_info, 4);					
					eopl_oam_state_machine(EOPL_OAM_PDU_RX);
				}
				else
				{
					eopl_oam_state_machine(EOPL_OAM_PDU_RX);
				}
				break;
			case DISCOVERY_SEND_LOCAL_REMOTE_OK:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nstate: SEND_LOCAL_REMOTE_OK\n");
				if(p_hdr->pcode == OAM_CODE_INFO)
				{
					flags = p_hdr->flags;
					if(READ_BITS(flags, 0x10))
					{
			/*** update remote info according to the received info */	
						oams.ucRmtOamVer = p_info->oam_version;
						oams.remote_action_state = p_info->state;
						oams.remote_oam_config = p_info->oam_config;
						oams.remote_oampdu_config = p_info->oampdu_config;
						vosMemCpy(oams.aucRmtOui, p_info->oui, 3);
						vosMemCpy(oams.aucRmtVendor, p_info->vendor_info, 4);
						oams.state = DISCOVERY_SEND_ANY;
						eopl_oam_state_machine(EOPL_OAM_PDU_RX);
					}
					else
					{
						oams.state = DISCOVERY_SEND_LOCAL_REMOTE;
						eopl_oam_state_machine(EOPL_OAM_PDU_RX);
					}
				}
				break;
			case DISCOVERY_SEND_ANY:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nstate: SEND_ANY\n");
				if(p_hdr->pcode == OAM_CODE_INFO)
				{
				/*** update remote info according to the received info */	
					oams.ucRmtOamVer = p_info->oam_version;
					oams.remote_action_state = p_info->state;
					oams.remote_oam_config = p_info->oam_config;
					oams.remote_oampdu_config = p_info->oampdu_config;
					vosMemCpy(oams.aucRmtOui, p_info->oui, 3);
					vosMemCpy(oams.aucRmtVendor, p_info->vendor_info, 4);
					oams.state = EX_DISCOVERY_PASSIVE_WAIT;
					eopl_oam_state_machine(EOPL_OAM_PDU_RX);
				}
				break;
			case EX_DISCOVERY_PASSIVE_WAIT:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nstate: EX_PASSIVE_WAIT\n");
				if(OAM_CODE_INFO == p_hdr->pcode)
				{
					oams.ucRmtOamVer = p_info->oam_version;
					oams.remote_action_state = p_info->state;
					oams.remote_oam_config = p_info->oam_config;
					oams.remote_oampdu_config = p_info->oampdu_config;
					vosMemCpy(oams.aucRmtOui, p_info->oui, 3);
					vosMemCpy(oams.aucRmtVendor, p_info->vendor_info, 4);

					pstExInfoTlv = (EX_OAM_INFO_TLV_t *)(p_byte+sizeof(struct oampdu_hdr)
									+ 2*sizeof(oam_info_tlv_t));
					if((INFO_ORG_SPEC_TYPE == pstExInfoTlv->ucType)
						 && (0x0B <= pstExInfoTlv->ucLength))
					{
						vosMemCpy (oams.aucOui, pstExInfoTlv->aucOui, 3);
						oams.state = EX_DISCOVERY_SEND_EXT_INFO;
						/*** now any remote OUI is satisfied ***/
						if(0)
						{
							oams.state = EX_DISCOVERY_SEND_OAM_EXT_INFO_ACK;
						}
						eopl_oam_state_machine(EOPL_OAM_PDU_RX);
					}
					else
					{
						oams.state = DISCOVERY_SEND_ANY;
						eopl_oam_state_machine(EOPL_OAM_PDU_RX);
					}
				}
				break;
			case EX_DISCOVERY_SEND_EXT_INFO:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nstate: SEND_EXT_INFO\n");
				if(OAM_CODE_INFO == p_hdr->pcode)
				{
					oams.ucRmtOamVer = p_info->oam_version;
					oams.remote_action_state = p_info->state;
					oams.remote_oam_config = p_info->oam_config;
					oams.remote_oampdu_config = p_info->oampdu_config;
					vosMemCpy(oams.aucRmtOui, p_info->oui, 3);
					vosMemCpy(oams.aucRmtVendor, p_info->vendor_info, 4);

					pstExInfoTlv = (EX_OAM_INFO_TLV_t *)(p_byte+sizeof(struct oampdu_hdr)
									+ 2*sizeof(oam_info_tlv_t));
					if((INFO_ORG_SPEC_TYPE == pstExInfoTlv->ucType)
						 && !(memcmp(oams.aucOui, pstExInfoTlv->aucOui, 3)))
					{
                        if (pstExInfoTlv->ucLength < (7+4)) {
                            OP_DEBUG(DEBUG_LEVEL_DEBUGGING,
                                "the Info Ack from OLT didnt contain OAM version info \n");
                            switch (pstExInfoTlv->ucVersion) {
                            case 0x01:
                                /*printf("$$$$ the OLT support CTC V_0_1 $$$\n");*/
                                oam_ver_type = OAM_VER_CTC_0_1_SUPPORT;
                                break;
                            case 0x20:
                                /*printf("$$$$$ the OLT support CTC V_2_0 $$$\n");*/
                                oam_ver_type = OAM_VER_CTC_2_0_SUPPORT;
                                break;
                            case 0x21:
                                /*printf("$$$$$ the OLT support CTC V_2_1 $$$\n");*/
                                oam_ver_type = OAM_VER_CTC_2_1_SUPPORT;
                                break;
                            default:
                                oam_ver_type = OAM_VER_MAX;
                                OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"Unknown support OAM version!\n");
                            }


                            if((((OAM_VER_CTC_0_1_SUPPORT == oam_ver_type)||
                                (OAM_VER_CTC_2_0_SUPPORT == oam_ver_type))  &&
                                (OAM_VER_CTC_2_0_SUPPORT !=  oam_cfg_ver_type)) ||
                               ((OAM_VER_CTC_2_1_SUPPORT == oam_ver_type) &&
                               (OAM_VER_CTC_2_1_SUPPORT != oam_cfg_ver_type)))
                            {
                            }else
                            {
                            }

                            //printf("$$$ support is %02x ^^^^^ \n",pstExInfoTlv->ucSupport);
                            //oam_ver_type = OAM_VER_CTC_2_0_SUPPORT;
                        }
						oams.state = EX_DISCOVERY_SEND_OAM_EXT_INFO_ACK;
						eopl_oam_state_machine(EOPL_OAM_PDU_RX);
						oams.state = EX_DISCOVERY_SEND_ANY;
					}
				}
				break;
			case EX_DISCOVERY_SEND_OAM_EXT_INFO_ACK:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nstate: SEND_OAM_EXT_INFO_ACK\n");
                    // $$$ just for test
					pstExInfoTlv = (EX_OAM_INFO_TLV_t *)(p_byte+sizeof(struct oampdu_hdr)
									+ 2*sizeof(oam_info_tlv_t));
					if((INFO_ORG_SPEC_TYPE == pstExInfoTlv->ucType)
						 && !(memcmp(oams.aucOui, pstExInfoTlv->aucOui, 3)))
                    {

                        /* printf("$$$ EX_DISCOVERY_SEND_EXT_INFO length is %d $$$$$\n",pstExInfoTlv->ucLength);*/
                        if (pstExInfoTlv->ucLength < (7+4)) {
                            OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"the Info Ack from OLT didnt contain OAM version info \n");
                            /*printf("$$$$$ the Info Ack from OLT didnt contain OAM version $$$\n");*/
                            oam_ver_type = OAM_VER_CTC_2_0_SUPPORT;
                        }else
                        {
                            switch (pstExInfoTlv->aucVersionList[3]) {
                            case 0x20:
                                oam_ver_type = OAM_VER_CTC_2_0_SUPPORT;
                                break;
                            case 0x21:
                                oam_ver_type = OAM_VER_CTC_2_1_SUPPORT;
                                break;
                            default:
                                OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"Unknown support OAM version!\n");
                            }

                        }
                    }

                    //$$$$

				if((0 == oams.usRInfoTlvVer) || (oams.usRInfoTlvVer<p_info->revision))
				{
					oams.state = EX_DISCOVERY_SEND_ANY;
					eopl_oam_state_machine(EOPL_OAM_PDU_RX);
				}
				break;
			case EX_DISCOVERY_SEND_OAM_EXT_INFO_NACK:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nstate: SEND_OAM_EXT_INFO_NACK\n");
				break;
			case EX_DISCOVERY_SEND_ANY_NO_EXT:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nstate: SEND_ANY_NO_EXT\n");
				break;
			case EX_DISCOVERY_SEND_ANY:
				if(OAM_CODE_INFO == p_hdr->pcode)
				{
					/*OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nstate: EX_SEND_ANY\n");*/
					oams.ucRmtOamVer = p_info->oam_version;
					oams.remote_action_state = p_info->state;
					oams.remote_oam_config = p_info->oam_config;
					oams.remote_oampdu_config = p_info->oampdu_config;
					vosMemCpy(oams.aucRmtOui, p_info->oui, 3);
					vosMemCpy(oams.aucRmtVendor, p_info->vendor_info, 4);

					pstExInfoTlv = (EX_OAM_INFO_TLV_t *)(p_byte+sizeof(struct oampdu_hdr)
									+ 2*sizeof(oam_info_tlv_t));
														
					if((INFO_ORG_SPEC_TYPE == pstExInfoTlv->ucType)
						 && (0x07 <= pstExInfoTlv->ucLength)
						 && (!memcmp(ctc_oui, pstExInfoTlv->aucOui, 3)))
					{
						vosMemCpy (oams.aucOui, pstExInfoTlv->aucOui, 3);
						oams.state = EX_DISCOVERY_SEND_EXT_INFO;;
						eopl_oam_state_machine(EOPL_OAM_PDU_RX);
					}					
					else
					{
					    eopl_oam_state_machine (EX_DISCOVERY_SEND_ANY);
					}
					
				}
				else if(OAM_CODE_ORG_SPEC ==  p_hdr->pcode)
				{
					if(!memcmp(ctc_oui, (u8_t *)p_info, 3)){
						/*stOamFramePar.pucFrame = p_byte;
						stOamFramePar.usFrmLen = framelen-4;
						stApiData.apiId = HAL_API_OAM_CTC_RECEIVE;
						stApiData.param = &stOamFramePar;
		                stApiData.length = sizeof(HAL_OAM_SEND_FRAME_PAR_t);
						halAppApiCallSync(&stApiData);*/
						switch(oam_ver_type)
						{
							case OAM_VER_CTC_0_1_SUPPORT:
							case OAM_VER_CTC_2_0_SUPPORT:
								eopl_ctc_pdu_process_2_0(p_byte, (framelen-4));
								break;
							case OAM_VER_CTC_2_1_SUPPORT:
								eopl_ctc_pdu_process(p_byte, (framelen-4));
								break;
							default:
								printf("Unknown OAM version!!\n");
						}
					/* eopl_ctc_pdu_process(p_byte,(framelen-4)); */
					}	
						
              
            
          
        
      
    
  

                    




                 
					else if(!memcmp(fiberhome_oui, (u8_t *)p_info, 3)){
#ifdef CTC_VOIP_DISPATCH
						oamMsqSend(&in_data[4],in_data_len);
#endif
					}
					else {
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nOAM_CODE_ORG_SPEC\n");
#if defined(OAM_SUPPORT_REG_CALLBACK)
	                    if (oam_private_entry != NULL &&
	                        (!memcmp(oam_private_oui,(u8_t*)p_info,3)))                       
						{
	                        stOamFramePar.pucFrame = p_byte;
	                        stOamFramePar.usFrmLen = framelen-4;
	                        stApiData.apiId = HAL_API_OAM_OUI_RECEIVE;
	                        stApiData.param = &stOamFramePar;
	                        stApiData.length = sizeof(HAL_OAM_SEND_FRAME_PAR_t);
	                        halAppApiCallSync(&stApiData);
	                    }
						else {
							OAM_INFO("receive an unkown OUI !\n");
						}
#elif defined(OAM_SUPPORT_ZTE_EXT)
						if(!memcmp(zte_oui, (u8_t *)p_info, 3)){
							eopl_zte_pdu_process(p_byte, framelen-4);
						}
						else {
							OAM_INFO("receive an unkown OUI !\n");
						}
#endif
#if 0
	                    /* EricYang add to fix the bug 2387 */
	                    if (oam_ver_type == OAM_VER_CTC_2_0_SUPPORT ||
	                        oam_ver_type == OAM_VER_CTC_0_1_SUPPORT)
	                    {
	                        eopl_not_support_response_2_0();
	                    }else if (oam_ver_type == OAM_VER_CTC_2_1_SUPPORT) {
	                        eopl_not_support_response();
	                    }

	                    if(g_usOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
	                    {
	                        oam_pdu_padding(&out_data[g_usOutDataIdx],  OAM_MINIMIZE_PDU_SIZE - g_usOutDataIdx);
	                        g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
	                    }
	                    OamFrameSend(out_data, g_usOutDataIdx);
						 /* !EricYang */
#endif
		                g_usOutDataIdx = 0;	               						
					}
				}
				else if(OAM_CODE_VARIABLE_REQ == p_hdr->pcode)
				{
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nOamVarReq\n");
					OamVarReq(	p_byte + sizeof(struct oampdu_hdr));
				}
				else if(OAM_CODE_LOOPBACK == p_hdr->pcode)
				{
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nOamLoopback\n");
					OamLoopback(p_byte + sizeof(struct oampdu_hdr));
				}
				else if(OAM_CODE_ORG_SPEC ==  p_hdr->pcode)                    
				{
                                      
				}                    
				else {
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nUnknown code %d.\n", p_hdr->pcode);
				}
				break;
			default:
				break;
		}
	}
}

/*
  * Function:
  * eopl_oam_state_machine
  * Purpose:
  * 802.3 oam state machine
  * Parameters:
  * Input - event
  *		event - input event to the state machine;
  * Output  - none
  * Returns:
  *     None
  */
void eopl_oam_state_machine(enum eopl_oam_event_t event)
{
	int		iStatus=EOPL_OK;
	u16_t	flags;
	u8_t	ucCnt;
	EX_OAM_INFO_TLV_t	*pstSendTlv;

	flags = oams.flags;

	if((event == EOPL_LOCAL_LINK_STATUS_CHANGE &&!READ_BITS(flags,0))||
		(event == EOPL_LOST_LINK_TIMER))
	{
		eopl_oam_fault_state(event);
		return;
	}
	if(event == EOPL_PDU_TIMER)
	{
		eopl_oam_timer_process();
	}
	/*if(event == EOPL_OAM_PDU_RX)
			eopl_oam_pdu_process();*/

	switch(oams.state)
	{
		case DISCOVERY_FAULT:
			eopl_oam_fault_state(event);
			break;
		case DISCOVERY_ACTIVE_SEND_LOCAL:
			oams.local_pdu = PDU_INFO;
			/*set local evaluating bit */
			SET_BITS(oams.flags,0x08);
			break;
		case DISCOVERY_PASSIVE_WAIT:
			oams.local_pdu = PDU_RX_INFO;
			/*set local evaluating bit */
			SET_BITS(oams.flags,0x08);
			break;
		case DISCOVERY_SEND_LOCAL_REMOTE:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\naction state: SEND_LOCAL_REMOTE\n");
			WRITE_BITS(oams.flags,0x18, 0x10);
			oams.local_pdu = PDU_INFO;
			vosMemSet(out_data, 0, OAM_MAXIMUM_PDU_SIZE+4);
			eopl_oam_info_tlv(&out_data[18], INFO_LOCAL_TYPE);
			eopl_oam_remote_info_tlv(&out_data[34], INFO_REMOTE_TYPE);
			eopl_oam_pdu_request(oam_src_mac, oams.flags, OAM_CODE_INFO,
										out_data, OAM_MINIMIZE_PDU_SIZE);
			oams.state = DISCOVERY_SEND_LOCAL_REMOTE_OK;
			break;
		case DISCOVERY_SEND_LOCAL_REMOTE_OK:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\naction state: SEND_LOCAL_REMOTE_OK\n");
			WRITE_BITS(oams.flags,0x18,0x10);
			oams.local_pdu = PDU_INFO;
			vosMemSet(out_data, 0, OAM_MAXIMUM_PDU_SIZE+4);
			eopl_oam_info_tlv(&out_data[18], INFO_LOCAL_TYPE);
			eopl_oam_remote_info_tlv(&out_data[34], INFO_REMOTE_TYPE);
			iStatus = eopl_oam_pdu_request(oam_src_mac, oams.flags, OAM_CODE_INFO,
										out_data, OAM_MINIMIZE_PDU_SIZE);
			if(EOPL_OK != iStatus)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "pdu request error. 0x%08x\r\n", iStatus);
			}
			break;
		case DISCOVERY_SEND_ANY:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\naction state: SEND_ANY\n");
			oams.local_pdu = PDU_ANY;
			/*set local stable bit,clear local evaluating bit */
			WRITE_BITS(oams.flags,0x18,0x10);
			WRITE_BITS(oams.flags, 0x60, 0x40);
			oams.action_state = 0;
			vosMemSet(out_data, 0, OAM_MAXIMUM_PDU_SIZE+4);
			eopl_oam_info_tlv(&out_data[18], INFO_LOCAL_TYPE);
			eopl_oam_remote_info_tlv(&out_data[34], INFO_REMOTE_TYPE);
			iStatus = eopl_oam_pdu_request(oam_src_mac, oams.flags, OAM_CODE_INFO,
										out_data, OAM_MINIMIZE_PDU_SIZE);
			if(EOPL_OK != iStatus)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "pdu request error. 0x%08x\r\n", iStatus);
			}
			oams.state = EX_DISCOVERY_PASSIVE_WAIT;
			break;
		case EX_DISCOVERY_PASSIVE_WAIT:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\naction state: EX_PASSIVE_WAIT\n");
			oams.local_pdu = PDU_INFO;
			oams.OAM_Ext_Status = FALSE;
			oams.state = EX_DISCOVERY_SEND_EXT_INFO;
			/*
			eopl_oam_info_tlv(&out_data[18], INFO_LOCAL_TYPE);
			iStatus = eopl_oam_pdu_request(oam_src_mac, oams.flags, OAM_CODE_INFO,
										out_data, OAM_MINIMIZE_PDU_SIZE);
			if(EOPL_OK != iStatus)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "pdu request error. 0x%08x\r\n", iStatus);
			}
			*/
			break;
		case EX_DISCOVERY_SEND_EXT_INFO:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "action state: SEND_EXT_INFO\n");
			oams.local_pdu = PDU_INFO;
			vosMemSet(out_data, 0, OAM_MAXIMUM_PDU_SIZE+4);
			eopl_oam_info_tlv(&out_data[18], INFO_LOCAL_TYPE);
			eopl_oam_remote_info_tlv(&out_data[34], INFO_REMOTE_TYPE);
			pstSendTlv = (EX_OAM_INFO_TLV_t *)&out_data[50];
			pstSendTlv->ucType = INFO_ORG_SPEC_TYPE;
                     if(OAM_VER_CTC_2_0_SUPPORT == oam_cfg_ver_type)
                     {
            			pstSendTlv->ucLength = 7+4*1;//there are 1 version list in TLV
			vosMemCpy (pstSendTlv->aucOui, ctc_oui, 3);
			pstSendTlv->ucSupport = 1;
			/*pstSendTlv->ucVersion = oams.ucExVer;*/
			pstSendTlv->ucVersion = 0;
			vosMemCpy(pstSendTlv->aucVersionList, ctc_oui, 3);
			pstSendTlv->aucVersionList[3] = 0x01;
                     }else if (OAM_VER_CTC_2_1_SUPPORT == oam_cfg_ver_type)
                     {

            			pstSendTlv->ucLength = 7+4*1;//there are 1 version list in TLV
            			vosMemCpy (pstSendTlv->aucOui, ctc_oui, 3);
            			pstSendTlv->ucSupport = 1;
            			/*pstSendTlv->ucVersion = oams.ucExVer;*/
            			pstSendTlv->ucVersion = 0;
            			vosMemCpy(pstSendTlv->aucVersionList, ctc_oui, 3);

                            /*EricYang add for the OAM-VER-MERGING */
                            pstSendTlv->aucVersionList[3] = 0x21;
                            /* !EricYang */

                     }else if (OAM_VER_CTC_20_21_MIX_SUPPORT == oam_cfg_ver_type)
                     {
            			pstSendTlv->ucLength = 7+4*2;//there are 2 version list in TLV
            			vosMemCpy (pstSendTlv->aucOui, ctc_oui, 3);
            			pstSendTlv->ucSupport = 1;
            			/*pstSendTlv->ucVersion = oams.ucExVer;*/
            			pstSendTlv->ucVersion = 0;
            			vosMemCpy(pstSendTlv->aucVersionList, ctc_oui, 3);

                            /*EricYang add for the OAM-VER-MERGING */
                            pstSendTlv->aucVersionList[3] = 0x01;
                            vosMemCpy((UINT*)&pstSendTlv->aucVersionList[3+1], ctc_oui, 3);
                            pstSendTlv->aucVersionList[3+1+3] = 0x21;
                            /* !EricYang */

                     }else
                     {
                            printf("unknow OAM configuration version ![%d]",oam_cfg_ver_type);
        			pstSendTlv->ucLength = 7;//there are 2 version list in TLV
        			vosMemCpy (pstSendTlv->aucOui, ctc_oui, 3);
        			pstSendTlv->ucSupport = 0;
        			/*pstSendTlv->ucVersion = oams.ucExVer;*/
        			pstSendTlv->ucVersion = 0;

                     }

			iStatus = eopl_oam_pdu_request(oam_src_mac, oams.flags, OAM_CODE_INFO,
										out_data, 80);
			if(EOPL_OK != iStatus)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "pdu request error. 0x%08x\r\n", iStatus);
			}
			/*oams.state = EX_DISCOVERY_SEND_OAM_EXT_INFO_ACK;*/
			break;
		case EX_DISCOVERY_SEND_OAM_EXT_INFO_ACK:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\naction state: SEND_OAM_EXT_INFO_ACK\n");
			oams.local_pdu = PDU_INFO;
			oams.OAM_Ext_Status = TRUE;
			vosMemSet(out_data, 0, OAM_MAXIMUM_PDU_SIZE+4);
			eopl_oam_info_tlv(&out_data[18], INFO_LOCAL_TYPE);
			eopl_oam_remote_info_tlv(&out_data[34], INFO_REMOTE_TYPE);
			/*** add another OS information TLV to meet salira's OLT ***/
			pstSendTlv = (EX_OAM_INFO_TLV_t *)&out_data[50];
			pstSendTlv->ucType = INFO_ORG_SPEC_TYPE;
			pstSendTlv->ucLength = 0x07;
			vosMemCpy (pstSendTlv->aucOui, ctc_oui, 3);
			if(oam_ver_type == oam_cfg_ver_type ||
               (OAM_VER_CTC_0_1_SUPPORT == oam_ver_type && OAM_VER_CTC_2_0_SUPPORT == oam_cfg_ver_type)||
               (OAM_VER_CTC_20_21_MIX_SUPPORT == oam_cfg_ver_type))
			{
			    pstSendTlv->ucSupport = 1;
            }else
            {
                pstSendTlv->ucSupport = 0;
            }
                    switch (oam_ver_type) {
                    case OAM_VER_CTC_0_1_SUPPORT:
        			  pstSendTlv->ucVersion = 0x01;
                      break;
                    case OAM_VER_CTC_2_0_SUPPORT:
        			  pstSendTlv->ucVersion = 0x20;
                      break ;
                    case OAM_VER_CTC_2_1_SUPPORT:
        			  pstSendTlv->ucVersion = 0x21;
                      break;
                    default :
                      printf("unknow CTC version!!\n");
                    }
			/*vosMemCpy(pstSendTlv->aucVersionList, ctc_oui, 3);
			pstSendTlv->aucVersionList[3] = 0x21;
            pstSendTlv->ucLength += 4;
            */
			iStatus = eopl_oam_pdu_request(oam_src_mac, oams.flags, OAM_CODE_INFO,
										out_data, 80);
			if(EOPL_OK != iStatus)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "pdu request error. 0x%08x\r\n", iStatus);
			}
			oams.state = EX_DISCOVERY_SEND_ANY;
	    /****Fix bug 3571 To IOP with some OLT,after the MPCP register disable the GE port till the OAM is registed */
			//oplRegWrite(REG_GMAC_CFG_TRANSFER_ENA,0x3);
			oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 0, 1, 1);
			oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 0, 1, 1);

		/************************Fix bug 3571 end*********************************************/
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\nOAM discovery process completed!\n");
			iStatus = oplRegWrite(REG_CP_OAM_REGI_CTRL, 0x01);
			if(OPL_OK != iStatus)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "register set error. reg:0x0026, value:0x1\n");
			}
			if (!IS_ONU_RESPIN)
			{
				iStatus = oplRegWrite(REG_CP_OAM_REGI_CTRL, 0x00);
				if(OPL_OK != iStatus)
				{
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "register set error. reg:0x0026, value:0x0\n");
				}
			}
			break;
		case EX_DISCOVERY_SEND_OAM_EXT_INFO_NACK:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\naction state: SEND_OAM_EXT_INFO_NACK\n");
			oams.local_pdu = PDU_INFO;
			oams.OAM_Ext_Status = FALSE;
			pstSendTlv = (EX_OAM_INFO_TLV_t *)&out_data[18];
			pstSendTlv->ucType = INFO_ORG_SPEC_TYPE;
			pstSendTlv->ucLength = 0x07;
			vosMemCpy (pstSendTlv->aucOui, ctc_oui, 3);
			pstSendTlv->ucSupport = 1;
			pstSendTlv->ucVersion = oams.ucExVer;
			iStatus = eopl_oam_pdu_request(oam_src_mac, oams.flags, OAM_CODE_INFO,
										(u8_t *)pstSendTlv, OAM_MINIMIZE_PDU_SIZE);
			if(EOPL_OK != iStatus)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "pdu request error. 0x%08x\r\n", iStatus);
			}
			break;
		case EX_DISCOVERY_SEND_ANY_NO_EXT:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\naction state: SEND_ANY_NO_EXT\n");
			oams.local_pdu = PDU_ANY_NO_Ext;
			break;
		case EX_DISCOVERY_SEND_ANY:
			oams.local_pdu = PDU_ANY;
			vosMemSet(out_data, 0, OAM_MAXIMUM_PDU_SIZE+4);
			oams.usLInfoTlvVer--;
			eopl_oam_info_tlv(&out_data[18], INFO_LOCAL_TYPE);
			eopl_oam_remote_info_tlv(&out_data[34], INFO_REMOTE_TYPE);
			/*** add another OS information TLV to meet salira's OLT ***/
			/*pstSendTlv = (EX_OAM_INFO_TLV_t *)&out_data[50];
			pstSendTlv->ucType = INFO_ORG_SPEC_TYPE;
			pstSendTlv->ucLength = 0x07;
			vosMemCpy (pstSendTlv->aucOui, ctc_oui, 3);
			pstSendTlv->ucSupport = 1;
			pstSendTlv->ucVersion = 0x01;*/
			for(ucCnt=0; ucCnt<g_ucDbgOamSendNum; ucCnt++)
			{
				iStatus = eopl_oam_pdu_request(oam_src_mac, oams.flags, OAM_CODE_INFO,
											out_data, OAM_MINIMIZE_PDU_SIZE);
				if(EOPL_OK != iStatus)
				{
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "pdu request error. 0x%08x\r\n", iStatus);
				}
			}
			break;
		default:
			break;
	}

}

int StartLinkLostTimer (void)
{
	int iStatus=EOPL_OK;
	struct itimerval stTime;
	struct sigaction stSigAction;

	stTime.it_value.tv_sec = OAM_LINK_LOST_TIME;
	stTime.it_value.tv_usec = 0;
	stTime.it_interval = stTime.it_value;

	vosMemSet (&stSigAction, 0, sizeof(stSigAction));
	stSigAction.sa_handler = LinkLostHandler;
	iStatus = sigaction (SIGPROF, &stSigAction, 0);
	if(EOPL_OK != iStatus)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "signal action set for link lost timer error!\n");
		return iStatus;
	}

	iStatus = setitimer (ITIMER_PROF, &stTime, 0);
	if(EOPL_OK != iStatus)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "link lost timer start error!\n");
		return iStatus;
	}

	return iStatus;
}

int StopLinkLostTimer (void)
{
	int iStatus=EOPL_OK;
	struct itimerval stTime;

	vosMemSet (&stTime, 0, sizeof(stTime));
	iStatus = setitimer (ITIMER_PROF, &stTime, 0);
	if(EOPL_OK != iStatus)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "link lost timer stop error!\n");
		return iStatus;
	}

	return iStatus;
}

void LinkLostHandler (int signal_number)
{
	int iStatus=EOPL_OK;
	struct itimerval stTime;

	vosMemSet (&stTime, 0, sizeof(stTime));
	iStatus = setitimer (ITIMER_PROF, &stTime, 0);
	if(EOPL_OK != iStatus)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "link lost timer stop error!\n");
		return;
	}

	iStatus = OamInit();
	if(EOPL_OK != iStatus)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "oam init error!\n");
		return;
	}
}

void OamTimerThread(void)
{
	int iStatus=EOPL_OK;
    int oam_timeout_count = 0;
	struct itimerval stTime;
	struct sigaction stSigAction;
    VOS_TIME_USECS_t start = 0;
    VOS_TIME_USECS_t current = 0;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "timer thread\n");

	sem_init (&g_iTimeSem, 0, 0);

	vosMemSet (&stSigAction, 0, sizeof(stSigAction));
	stSigAction.sa_handler = TimerUnit;
	iStatus = sigaction (SIGALRM, &stSigAction, 0);
	if(EOPL_OK != iStatus)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "signal action set for timer unit error!\n");
		return;
	}

	vosMemSet (&stTime, 0, sizeof(stTime));
	stTime.it_value.tv_sec = 0;
	stTime.it_value.tv_usec = 1000;
	stTime.it_interval = stTime.it_value;
	iStatus = setitimer (ITIMER_REAL, &stTime, 0);
	if(EOPL_OK != iStatus)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "timer unit start error!\n");
		return;
	}

	while (1)
	{
		sem_wait (&g_iTimeSem);
        current = vosTimeUsecsGet();

        if ((current - start) >= SECOND_USEC)
        {
    		if((EX_DISCOVERY_SEND_ANY == oams.state) && (1 == g_ucOamActiveSend))
    		{
    			if(1 == g_ucFrmDbgDis)
    			{
    				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "time send\n");
    			}
    			vosMemSet(aucTimeOut, 0, OAM_MAXIMUM_PDU_SIZE+4);
    			oams.local_pdu = PDU_ANY;
    			oams.usLInfoTlvVer--;
    			eopl_oam_info_tlv(&aucTimeOut[18], INFO_LOCAL_TYPE);
    			eopl_oam_remote_info_tlv(&aucTimeOut[34], INFO_REMOTE_TYPE);
    			iStatus = eopl_oam_pdu_request(oam_src_mac, oams.flags, OAM_CODE_INFO,
    										aucTimeOut, OAM_MINIMIZE_PDU_SIZE);
    			if(EOPL_OK != iStatus)
    			{
    				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "pdu request error. 0x%08x\r\n", iStatus);
    			}

    		}

           start = vosTimeUsecsGet(); 
        }
        
		if(1 == g_stLinkLost.iFlag)
		{
            /*begin modified by liaohongjun 2012/7/18*/
			g_stLinkLost.iFlag = 0;
            oam_timeout_count++;
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "oam timeout.\n");
            /*reset the optical module if without oam pkts in 5mins*/
            if(MAX_PON_WAIT_TIMES == oam_timeout_count)
            {
                /*begin modified by liaohongjun 2012/11/19 of EPN204QID0008*/
                #ifndef CONFIG_BOSA                
                OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "ReSet pon port.\n");
                if(DRV_OK != Ioctl_SetGpioVal(DRV_GPIO_11_VCCT, 0))
                {
                    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Set GPIO_11 low failed.\n");
                }
                sleep(5);
                if(DRV_OK != Ioctl_SetGpioVal(DRV_GPIO_11_VCCT, 1))
                {
                    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Set GPIO_11 hight failed.\n");
                }     
                #endif
                /*end modified by liaohongjun 2012/11/19 of EPN204QID0008*/
                oam_timeout_count = 0;
            }
            /*end modified by liaohongjun 2012/7/18*/
			OamInit();
		}
		/*if(1 == g_stStatsTimer.iFlag)
		{
			g_stStatsTimer.iFlag = 0;
			PollingHandler();
		}*/
		/*
		if(1 == g_stPduSend.iFlag)
		{
			g_stPduSend.iFlag = 0;
			if(0 == g_ucPduMax)
			{
				if(EX_DISCOVERY_SEND_ANY == oams.state)
				{
					if(1 == g_ucFrmDbgDis)
					{
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "time send\n");
					}
					vosMemSet(aucTimeOut, 0, OAM_MAXIMUM_PDU_SIZE+4);
					oams.usLInfoTlvVer--;
					eopl_oam_info_tlv(&aucTimeOut[18], INFO_LOCAL_TYPE);
					eopl_oam_remote_info_tlv(&aucTimeOut[34], INFO_REMOTE_TYPE);
					iStatus = eopl_oam_pdu_request(oam_src_mac, oams.flags, OAM_CODE_INFO,
												aucTimeOut, OAM_MINIMIZE_PDU_SIZE);
					if(EOPL_OK != iStatus)
					{
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "pdu request error. 0x%08x\r\n", iStatus);
					}

				}
			}
			g_ucPduMax = 0;
		}*/
	}
}

void OamInfoSend(void)
{
	int iStatus=EOPL_OK;

	if(1 == g_ucFrmDbgDis)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "info send\n");
	}

	vosMemSet(aucTimeOut, 0, OAM_MAXIMUM_PDU_SIZE+4);
	oams.local_pdu = PDU_ANY;
	oams.usLInfoTlvVer--;
	eopl_oam_info_tlv(&aucTimeOut[18], INFO_LOCAL_TYPE);
	eopl_oam_remote_info_tlv(&aucTimeOut[34], INFO_REMOTE_TYPE);
	iStatus = eopl_oam_pdu_request(oam_src_mac, oams.flags, OAM_CODE_INFO,
								aucTimeOut, OAM_MINIMIZE_PDU_SIZE);
	if(EOPL_OK != iStatus)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "pdu request error. 0x%08x\r\n", iStatus);
	}
}


void TimerUnit(int signal)
{
	g_stLinkLost.uiCnt++;
	/*g_stPduSend.uiCnt++;*/
	if(1000 == g_stLinkLost.uiCnt)
	{
		/*OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\ntimeout.\n");*/
		g_stLinkLost.uiCnt = 0;
		g_stLinkLost.iFlag = 1;
	}

	/*if(g_stStatsTimer.iStart)
	{
		g_stStatsTimer.uiCnt++;
		if(20 == g_stStatsTimer.uiCnt)
		{
			g_stStatsTimer.uiCnt = 0;
			g_stStatsTimer.iFlag = 1;
		}
	}*/
	/*
	if(20 == g_stPduSend.uiCnt)
	{
		g_stPduSend.uiCnt = 0;
		g_stPduSend.iFlag = 1;
	}*/
	sem_post (&g_iTimeSem);
}

int OamVarReq (u8_t *pucData)
{
	int	iStatus=EOPL_OK;
	oam_variable_descriptor_t	*pstVarDes;

	pstVarDes = (oam_variable_descriptor_t *)pucData;

	for(; 0 != pstVarDes->branch; pstVarDes++)
	{
		switch (pstVarDes->branch)
		{
			case OAM_BRANCH_STANDARD_ATTRIB:
				           
        #if defined(ONU_1PORT)
            case OAM_BRANCH_PMC_ATTRIB:
        #endif
				iStatus = OamVarAttr(pstVarDes);
        
				if(EOPL_OK != iStatus)
				{
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "variable attribute get error!\n");
				}
				break;
			default:
				break;
		}
	}

	if(g_usOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
 	{
		oam_pdu_padding(&out_data[g_usOutDataIdx],  OAM_MINIMIZE_PDU_SIZE - g_usOutDataIdx);
		g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
 	}
	OamFrameSend(out_data, g_usOutDataIdx);
	g_usOutDataIdx = 0;

	return iStatus;
}


int OamGetStatistics(u32_t ulRequestId, attr_statistics_t* pstStat)
{
    OPL_CNT_t astPonCounter[DAL_CNT_PON_NUM];
	OPL_CNT_t astGeCounter[DAL_CNT_GE_NUM];
	
    if (NULL == pstStat)
    {
        return EOPL_PARAM_ERROR;
    }

	vosMemSet (astPonCounter, 0, sizeof(OPL_CNT_t) * DAL_CNT_PON_NUM);
	vosMemSet (astGeCounter, 0, sizeof(OPL_CNT_t) * DAL_CNT_GE_NUM);
	
    dalCounterGet(DAL_CNT_POLL_PON_MASK, astPonCounter, 0, DAL_CNT_PON_NUM);
	dalCounterGet(DAL_CNT_POLL_GE_MASK, astGeCounter, 0, DAL_CNT_GE_NUM);
	
    switch ( ulRequestId )
    {
        case STAT_ATTR_BROD_TX_RRM :
            pstStat->ucDataLength = 4;
            pstStat->ulData = astPonCounter[STAT_PON_TRX_BROD_PKT].stAdded.uiLow;
            break;
        case STAT_ATTR_BROD_RX_RRM :
            pstStat->ucDataLength = 4;
            pstStat->ulData = astPonCounter[STAT_PON_RCV_BROD_PKT].stAdded.uiLow;
            break;
        case STAT_ATTR_PON_PAUSE_RX:
            pstStat->ucDataLength = 4;
            pstStat->ulData = astPonCounter[STAT_PON_RCV_PAUSE].stAdded.uiLow;
            break;
        case STAT_ATTR_PON_PAUSE_TX:
            pstStat->ucDataLength = 4;
            pstStat->ulData = astPonCounter[STAT_PON_TRX_PAUSE].stAdded.uiLow; 
            break;
        case STAT_ATTR_SYS_PAUSE_RX: 
            pstStat->ucDataLength = 4;
            pstStat->ulData = astGeCounter[STAT_GE_RX_PAUSE].stAdded.uiLow;
            break;
        case STAT_ATTR_SYS_PAUSE_TX: 
            pstStat->ucDataLength = 4;
            pstStat->ulData = astGeCounter[STAT_GE_TX_PAUSE].stAdded.uiLow;
            break;
        default:
            return EOPL_NOT_SUPPORT;
    }

    return EOPL_OK;
}


int OamVarAttr (u8_t *pucData)
{
	int	iStatus=EOPL_OK;
	oam_variable_descriptor_t	*pstVarDes;
    u8_t *p_byte = NULL; 
    oam_variable_container_t *pstVarCon = NULL;

    
    if (NULL == pucData)
    {
        return EOPL_PARAM_ERROR;
    }
    
	pstVarDes = (oam_variable_descriptor_t *)pucData;

    if(0 == g_usOutDataIdx)
	{
		p_byte = &out_data[0];
		eopl_oam_pdu_hdr(p_byte, oams.flags, OAM_CODE_VARIABLE_RSP);
		p_byte += sizeof(struct oampdu_hdr);
		g_usOutDataIdx = sizeof(struct oampdu_hdr);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

    pstVarCon = (oam_variable_container_t *)p_byte;
    pstVarCon->variable_branch = pstVarDes->branch;
	pstVarCon->vairable_leaf = pstVarDes->leaf;

    vosMemSet(&m_stOamStatData, 0, sizeof(m_stOamStatData));
    /* 不支持的话，也要返回不支持 */
    iStatus = OamGetStatistics( (pstVarDes->branch << 16) | pstVarDes->leaf, &m_stOamStatData ); 
    if (EOPL_OK != iStatus)
    {
        pstVarCon->width = iStatus;
        vosMemSet(pstVarCon->value, 0, 4);
        g_usOutDataIdx += 4;
    }
    else
    {
        pstVarCon->width = m_stOamStatData.ucDataLength;
        HLTOBUF(pstVarCon->value, m_stOamStatData.ulData);
        g_usOutDataIdx += pstVarCon->width + 4;
    }  
    
#if 0        
	switch (pstVarDes->leaf)
	{
		case OAM_ATTR_FRM_TX_OK:
			OamGetFrmTxOk();
			break;
		case OAM_ATTR_FRM_RX_OK:
			OamGetFrmTxOk();
			break;
		default:
			break;
	}
#endif
    return EOPL_OK;  
   
}

void OamGetFrmTxOk(void)
{
	u8_t *p_byte;
	oam_variable_container_t *pstVarCon;

	if(0 == g_usOutDataIdx)
	{
		p_byte = &out_data[0];
		eopl_oam_pdu_hdr(p_byte, oams.flags, OAM_CODE_VARIABLE_RSP);
		p_byte += sizeof(struct oampdu_hdr);
		g_usOutDataIdx = sizeof(struct oampdu_hdr);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	pstVarCon = (oam_variable_container_t *)p_byte;
	pstVarCon->variable_branch = OAM_BRANCH_STANDARD_ATTRIB;
	pstVarCon->vairable_leaf = OAM_ATTR_FRM_TX_OK;
	pstVarCon->width = 4;
	pstVarCon->value[0] = 0xFF;

	g_usOutDataIdx += pstVarCon->width + 4;
}

void OamGetFrmRxOk(void)
{
	u8_t *p_byte;
	oam_variable_container_t *pstVarCon;

	if(0 == g_usOutDataIdx)
	{
		p_byte = &out_data[0];
		eopl_oam_pdu_hdr(p_byte, oams.flags, OAM_CODE_VARIABLE_RSP);
		p_byte += sizeof(struct oampdu_hdr);
		g_usOutDataIdx = sizeof(struct oampdu_hdr);
	}
	else
	{
		p_byte = &out_data[g_usOutDataIdx];
	}

	pstVarCon = (oam_variable_container_t *)p_byte;
	pstVarCon->variable_branch = OAM_BRANCH_STANDARD_ATTRIB;
	pstVarCon->vairable_leaf = OAM_ATTR_FRM_TX_OK;
	pstVarCon->width = 4;
	pstVarCon->value[0] = 0xFF;

	g_usOutDataIdx += pstVarCon->width + 4;
}

int OamLoopback(u8_t *pucData)
{
	int	iStatus=EOPL_OK;

	if(0x01 == *pucData)
	{
		OamEnableLp();
	}
	else if(0x02 == *pucData)
	{
		OamDisableLp();
	}
	else
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "unknown loopback code\n");
	}

	return iStatus;
}

int OamEnableLp(void)
{
	int	iStatus=EOPL_OK;
	BRG_PORT_TAB_PON_t stPonPortEntry;
	u32_t *pulValue = (UINT32 *)&stPonPortEntry;

	oplRegRead(REG_BRG_PON_PORT_TABLE,(u32_t *)&stPonPortEntry);
	stPonPortEntry.lp = OPL_ENABLE;
	stPonPortEntry.trsmt_en = OPL_DISABLE;
	oplRegWrite(REG_BRG_PON_PORT_TABLE, *pulValue);

	vosMemSet(out_data, 0, OAM_MAXIMUM_PDU_SIZE+4);
	oams.action_state = 5;
	eopl_oam_info_tlv(&out_data[18], INFO_LOCAL_TYPE);
	eopl_oam_remote_info_tlv(&out_data[34], INFO_REMOTE_TYPE);
	eopl_oam_pdu_request(oam_src_mac, oams.flags, OAM_CODE_LOOPBACK,
										out_data, OAM_MINIMIZE_PDU_SIZE);

	return iStatus;
}

int OamDisableLp(void)
{
	int	iStatus=EOPL_OK;
	BRG_PORT_TAB_PON_t stPonPortEntry;
    u32_t *pulValue = (UINT32 *)&stPonPortEntry;

	oplRegRead(REG_BRG_PON_PORT_TABLE, (u32_t *)&stPonPortEntry);
	stPonPortEntry.lp = OPL_DISABLE;
	stPonPortEntry.trsmt_en = OPL_ENABLE;
	oplRegWrite(REG_BRG_PON_PORT_TABLE, *pulValue);

	vosMemSet(out_data, 0, OAM_MAXIMUM_PDU_SIZE+4);
	oams.action_state = 0;
	eopl_oam_info_tlv(&out_data[18], INFO_LOCAL_TYPE);
	eopl_oam_remote_info_tlv(&out_data[34], INFO_REMOTE_TYPE);
	eopl_oam_pdu_request(oam_src_mac, oams.flags, OAM_CODE_LOOPBACK,
										out_data, OAM_MINIMIZE_PDU_SIZE);

	return iStatus;
}

int OamDygaspCfg(void)
{
	int		iStatus=EOPL_OK;
	u32_t	auiCfgReg[8];
	u16_t	usCnt, usIdx;
	oam_info_tlv_t *pstLInfo, *pstRInfo;

	oplRegWrite(0x4030*4, 0x80000000|(0x0052<<8));

	pstLInfo = (oam_info_tlv_t *)&(auiCfgReg[0]);
	pstRInfo = (oam_info_tlv_t *)&(auiCfgReg[4]);
	eopl_oam_info_tlv((u8_t *)pstLInfo, INFO_LOCAL_TYPE);
	eopl_oam_remote_info_tlv((u8_t *)pstRInfo, INFO_REMOTE_TYPE);

	for(usCnt=0x4031, usIdx=0; usCnt<=0x4038; usCnt++, usIdx++)
	{
		oplRegWrite(usCnt*4, auiCfgReg[usIdx]);
	}

	return iStatus;
}

int OamOplRstCfg(u8_t ucEndis, u16_t usFlag, u8_t ucCode, u32_t uiPayLoad)
{
	int		iStatus=EOPL_OK;
	u32_t	uiRstCfg=0;

	uiRstCfg |= (ucEndis&0x01) << 24;
	uiRstCfg |= usFlag << 8;
	uiRstCfg |= ucCode;
	iStatus = oplRegWrite(OAM_OPL_RST_CFG, uiRstCfg);
	if(EOPL_OK != iStatus)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "oam opl reset cfg error\n");
		return iStatus;
	}

	iStatus = oplRegWrite(OAM_OPL_RST_PAY, uiPayLoad);
	if(EOPL_OK != iStatus)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "oam opl reset payload error\n");
		return iStatus;
	}

	return iStatus;
}

int OamStatusGet(u8_t *pState)
{
	*pState = oams.state;

	return OPL_OK;
}

/* Eric Yang add for the OAM-CALLBACK-Function Registered */
#ifdef OAM_SUPPORT_REG_CALLBACK
UINT32 OamEventCalllBackRegister( IN const OAM_EV_CB_ID_t value,
                                  IN const event_callback pcb)
{
    UINT32 ret = OPL_ERROR;
    OAM_EV_CB_ID_t * key_val = NULL;

    OAM_TRACE();
    if (NULL == pcb) {
        OAM_WRN("Input NULL callback Function !\n");
        return ret;
    }

    key_val = vosAlloc(sizeof(OAM_EV_CB_ID_t));
    if (NULL == key_val) {
        OAM_WRN("Alloc key value Failed !\n");
        return ret;
    }
    vosMemSet((void *)key_val, 0 , sizeof(OAM_EV_CB_ID_t));
    key_val->oam_ver = value.oam_ver;
    key_val->oam_ext_code = value.oam_ext_code;
    key_val->oam_branch_id = value.oam_branch_id;
    key_val->oam_leaf_id = value.oam_leaf_id;

    OAM_DBG("version is [%02x],ext_code is [%02x],branch_id is [%02x],leaf id [%04x] ",
            key_val->oam_ver,key_val->oam_ext_code,key_val->oam_branch_id,key_val->oam_leaf_id);
    vosHashInsert((void *)key_val,(void *)pcb,oam_cb_event_table);

    return OPL_OK;
}

UINT32 OamEventCallBackDeregister(IN const OAM_EV_CB_ID_t value)
{
    UINT32 ret = OPL_OK;

    OAM_TRACE();

    vosHashRemove((void *)&value,oam_cb_event_table);
    return ret;
}


static int  OamEvHashValueComp(void * val1,void * val2)
{
    P_OAM_EV_CB_ID_t pkey1 = (P_OAM_EV_CB_ID_t) val1;
    P_OAM_EV_CB_ID_t pkey2 = (P_OAM_EV_CB_ID_t) val2;

    if (pkey1->oam_ver == pkey2->oam_ver &&
        pkey1->oam_ext_code == pkey2->oam_ext_code &&
        pkey1->oam_branch_id == pkey2->oam_branch_id &&
        pkey1->oam_leaf_id == pkey2->oam_leaf_id) {

        return 1;
    }
    return 0;
}

static UINT32 OamEvHashValue(void * value)
{
    P_OAM_EV_CB_ID_t p_val = (P_OAM_EV_CB_ID_t)value;
    UINT32 key = 0;

    key = (UINT32)(((UINT32)p_val->oam_ext_code) << 24)+ (UINT32)(((UINT32)p_val->oam_branch_id) << 16)+ (UINT32)p_val->oam_leaf_id;
    if (p_val->oam_ver == OAM_VER_CTC_2_0_SUPPORT ||
        p_val->oam_ver == OAM_VER_CTC_0_1_SUPPORT) {

       /* using the bit 14-15:32 to prescence the version info */
       /*  00 ---- version 2.0*/
       /*  01 ---- version 2.1 */
        key |= 0x00003000;
    }else if (p_val->oam_ver == OAM_VER_CTC_2_1_SUPPORT) {
        key |= 0x00007000;
    }

    return key;
}
static void OamEvHashDestroy(void * key,void * funptr)
{

    OAM_TRACE();
    if (key == NULL ) {
        OAM_WRN("Hash Key is NULL !\n");
        return ;
    }
    vosFree(key);
    funptr = NULL;

}

UINT32 OamEventQueryCount()
{
    OAM_TRACE();
    /* todo */
    return vosHashCount(oam_cb_event_table);
}

UINT32 OamEventCallBackInit(UINT16 hash_size)
{
    UINT32 ret = OPL_OK;
    OAM_TRACE();

    if (oam_cb_event_table != NULL) {
        OAM_INFO("The OAM Event CallBack Table already exist!\n");
        return ret;
    }else {

        oam_cb_event_table = vosHashCreate(OamEvHashValue,OamEvHashValueComp,
                                           OamEvHashDestroy, hash_size);
        if (NULL == oam_cb_event_table) {
            OAM_WRN("\r\n Create OAM Event Hash Table Failed!! \r\n");
            return OPL_ERROR;
        }

    }

    return ret;
}
UINT32 OamEventCallBackCleanUp()
{
    UINT32 ret = OPL_OK;
    OAM_TRACE();

    vosHashClear(oam_cb_event_table);
    return ret;
}

UINT32 OamEventCallBack(IN const OAM_EV_CB_ID_t value,
                               IN const  UINT8 * in_byte,
                               IN OUT UINT8 * out_byte)
{

    UINT32 ret = OPL_ERROR;
    UINT8 * p_out_byte = NULL;
    event_callback p_cb = NULL;


    OAM_EV_CB_ID_t  oam_ev_id ;

    OAM_TRACE();
    OAM_DBG("vesion is: [%02d],extern code is:[%02x],branch id: [%02x],leaf :[%04x] ",\
            value.oam_ver,value.oam_ext_code,value.oam_branch_id,value.oam_leaf_id);


#if 0
    if (ver == OAM_VER_CTC_2_0_SUPPORT ) {
        p_out_byte = eopl_get_output_buf_2_0(ext_code);
    }if (ver == oam_VER_CTC_2_1_SUPPORT) {
        p_out_byte = eopl_get_output_buf(ext_code);
    }
    oam_ev_id.oam_ver_type = ver;
    oam_ev_id.oam_ext_code = ext_code ;
    oam_ev_id.oam_branch_id = branch_id;
    oam_ev_id.oam_leaf_id = leaf_id;
#endif

    p_cb = (event_callback )vosHashValue((void *)&value,oam_cb_event_table);
    if (NULL == p_cb) {
        OAM_INFO("Cannt find the CallBack Function \n ");
        return ret;
    }

    ret = p_cb(in_byte,p_out_byte);

    return ret;




}

u32_t OamRegisterSendHandler(oam_send_handler snd_hdl)
{
    if (NULL == snd_hdl) {
        oam_snd_cb = NULL;
        OAM_WRN("Input NULL point of callback point!\n");
        return OPL_ERROR;
    }

    oam_snd_cb = snd_hdl;
    return OPL_OK;


}
u32_t OamRegisterProcessEntry(u8_t * oui,oam_process_entry entry )
{

    OAM_TRACE();
    if (oui == NULL || entry == NULL ) {
        OAM_INFO("Input NULL Point !!\n");
        return OPL_ERROR;
    }

    vosMemCpy(&oam_private_oui,oui,3);

    oam_private_entry = entry;

}

u32_t OamDeRegisterProcessEntry(u8_t * oui)
{
    OAM_TRACE();

    if (oui == NULL ) {
        OAM_INFO("Input NULL Point !!\n");
        return OPL_ERROR;
    }
    vosMemSet(&oam_private_oui,0,3);
    oam_private_entry = NULL;
}

#endif

/* ! EricYang*/

