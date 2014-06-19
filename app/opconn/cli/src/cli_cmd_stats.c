/******************************************************************************
*    COPYRIGHT (C) 2003-2007 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*  This software is made available only to customers and prospective
*  customers of Opulan Technologies Corporation under license and may be
*  used only with Opulan semi-conductor products.
*
* FILENAME:
*	cli_cmd_stats.c
*
* DESCRIPTION:
*	This file implements cli interface for stats module.
*
* Date Created:
*	2008-09-02
*
* Authors(optional):
*	Zhihua Lu
*
* Reviewed by (optional):
*
******************************************************************************/
#include <stdlib.h>
#include "cli.h"
#include "cli_cmd_list.h"

#include "ctc_2_1.h"
#include "hal_stats.h"
#include "cli_page.h"
#include "odm_pon.h"
#include "opl_debug.h"

STATUS CliShowCntGe(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS			iStatus=OK;
	HAL_STATS_PAR_t	stStatsPar;
	OPL_API_DATA_t	stApiData;

	if (NULL == pstEnv)
	{
		return ERROR;
	}

	stStatsPar.iWrFd = pstEnv->nWriteFd;
	stStatsPar.uiMode = pstEnv->pmode;

	stApiData.apiId = HAL_API_STATS_GE;
	stApiData.param = (void *)&stStatsPar;
    stApiData.length = sizeof(HAL_STATS_PAR_t);

    cliShowStart(pstEnv);
	/* iStatus = ShowCntGe(pstEnv); */
	halAppApiCallSync(&stApiData);
    cliShowEnd(pstEnv);
	if (OK != iStatus)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nshow counter ge error! 0x%08x\r\n", iStatus);
		return OK;
	}

	return iStatus;
}

STATUS CliShowCntPon(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS			iStatus=OK;
	HAL_STATS_PAR_t	stStatsPar;
	OPL_API_DATA_t	stApiData;

	if (NULL == pstEnv)
	{
		return ERROR;
	}

	stStatsPar.iWrFd = pstEnv->nWriteFd;
	stStatsPar.uiMode = pstEnv->pmode;

	stApiData.apiId = HAL_API_STATS_PON;
	stApiData.param = (void *)&stStatsPar;
    stApiData.length = sizeof(HAL_STATS_PAR_t);

    cliShowStart(pstEnv);
	/* iStatus = ShowCntPon(pstEnv); */
	halAppApiCallSync(&stApiData);
    cliShowEnd(pstEnv);
	if (OK != iStatus)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nshow counter pon error! 0x%08x\r\n", iStatus);
		return OK;
	}

	return iStatus;
}

STATUS CliShowCntFe(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS			iStatus=OK;
	HAL_STATS_PAR_t	stStatsPar;
	OPL_API_DATA_t	stApiData;

	if (NULL == pstEnv)
	{
		return ERROR;
	}

	stStatsPar.iWrFd = pstEnv->nWriteFd;
	stStatsPar.uiMode = pstEnv->pmode;

	stApiData.apiId = HAL_API_STATS_FE;
	stApiData.param = (void *)&stStatsPar;
    stApiData.length = sizeof(HAL_STATS_PAR_t);

    cliShowStart(pstEnv);
	/* iStatus = ShowCntFe(pstEnv); */
	halAppApiCallSync(&stApiData);
    cliShowEnd(pstEnv);
	if (OK != iStatus)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nshow counter fe error! 0x%08x\r\n", iStatus);
		return OK;
	}

	return iStatus;
}

STATUS CliShowCntTm(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS			iStatus=OK;
	HAL_STATS_PAR_t	stStatsPar;
	OPL_API_DATA_t	stApiData;

	if (NULL == pstEnv)
	{
		return ERROR;
	}

	stStatsPar.iWrFd = pstEnv->nWriteFd;
	stStatsPar.uiMode = pstEnv->pmode;

	stApiData.apiId = HAL_API_STATS_TM;
	stApiData.param = (void *)&stStatsPar;
    stApiData.length = sizeof(HAL_STATS_PAR_t);

    cliShowStart(pstEnv);
	/* iStatus = ShowCntTm(pstEnv); */
	halAppApiCallSync(&stApiData);
    cliShowEnd(pstEnv);
	if (OK != iStatus)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nshow counter tm error! 0x%08x\r\n", iStatus);
		return OK;
	}

	return iStatus;
}

STATUS CliShowCntCle(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS			iStatus=OK;
	HAL_STATS_PAR_t	stStatsPar;
	OPL_API_DATA_t	stApiData;

	if (NULL == pstEnv)
	{
		return ERROR;
	}

	stStatsPar.iWrFd = pstEnv->nWriteFd;
	stStatsPar.uiMode = pstEnv->pmode;

	stApiData.apiId = HAL_API_STATS_CLE;
	stApiData.param = (void *)&stStatsPar;
    stApiData.length = sizeof(HAL_STATS_PAR_t);

    cliShowStart(pstEnv);
	/* iStatus = ShowCntCle(pstEnv); */
	halAppApiCallSync(&stApiData);
    cliShowEnd(pstEnv);
	if (OK != iStatus)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nshow counter cle error! 0x%08x\r\n", iStatus);
		return OK;
	}

	return iStatus;
}

STATUS CliShowCntBrg(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS			iStatus=OK;
	HAL_STATS_PAR_t	stStatsPar;
	OPL_API_DATA_t	stApiData;

	if (NULL == pstEnv)
	{
		return ERROR;
	}

	stStatsPar.iWrFd = pstEnv->nWriteFd;
	stStatsPar.uiMode = pstEnv->pmode;

	stApiData.apiId = HAL_API_STATS_BRG;
	stApiData.param = (void *)&stStatsPar;
    stApiData.length = sizeof(HAL_STATS_PAR_t);

    cliShowStart(pstEnv);
	/* iStatus = ShowCntBrg(pstEnv); */
	halAppApiCallSync(&stApiData);
    cliShowEnd(pstEnv);
	if (OK != iStatus)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nshow counter brg error! 0x%08x\r\n", iStatus);
		return OK;
	}

	return iStatus;
}

STATUS CliShowCntMpcp(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS			iStatus=OK;
	HAL_STATS_PAR_t	stStatsPar;
	OPL_API_DATA_t	stApiData;

	if (NULL == pstEnv)
	{
		return ERROR;
	}

	stStatsPar.iWrFd = pstEnv->nWriteFd;
	stStatsPar.uiMode = pstEnv->pmode;

	stApiData.apiId = HAL_API_STATS_MPCP;
	stApiData.param = (void *)&stStatsPar;
    stApiData.length = sizeof(HAL_STATS_PAR_t);

    cliShowStart(pstEnv);
	/* iStatus = ShowCntMpcp(pstEnv); */
	halAppApiCallSync(&stApiData);
    cliShowEnd(pstEnv);
	if (OK != iStatus)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nshow counter mpcp error! 0x%08x\r\n", iStatus);
		return OK;
	}

	return iStatus;
}

STATUS CliShowCntMarb(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS			iStatus=OK;
	HAL_STATS_PAR_t	stStatsPar;
	OPL_API_DATA_t	stApiData;

	if (NULL == pstEnv)
	{
		return ERROR;
	}

	stStatsPar.iWrFd = pstEnv->nWriteFd;
	stStatsPar.uiMode = pstEnv->pmode;

	stApiData.apiId = HAL_API_STATS_MARB;
	stApiData.param = (void *)&stStatsPar;
    stApiData.length = sizeof(HAL_STATS_PAR_t);

    cliShowStart(pstEnv);
	/* iStatus = ShowCntMarb(pstEnv); */
	halAppApiCallSync(&stApiData);
    cliShowEnd(pstEnv);
	if (OK != iStatus)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nshow counter marb error! 0x%08x\r\n", iStatus);
		return OK;
	}

	return iStatus;
}

STATUS CliShowCntCpdma(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS			iStatus=OK;
	HAL_STATS_PAR_t	stStatsPar;
	OPL_API_DATA_t	stApiData;

	if (NULL == pstEnv)
	{
		return ERROR;
	}

	stStatsPar.iWrFd = pstEnv->nWriteFd;
	stStatsPar.uiMode = pstEnv->pmode;

	stApiData.apiId = HAL_API_STATS_CPDMA;
	stApiData.param = (void *)&stStatsPar;
    stApiData.length = sizeof(HAL_STATS_PAR_t);

    cliShowStart(pstEnv);
	/* iStatus = ShowCntCpdma(pstEnv); */
	halAppApiCallSync(&stApiData);
	printf("software cnt : \n");
	showCntHost();
    cliShowEnd(pstEnv);
	if (OK != iStatus)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nshow counter cpdma error! 0x%08x\r\n", iStatus);
		return OK;
	}

	return iStatus;
}

STATUS CliShowCntFedma(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS			iStatus=OK;
	HAL_STATS_PAR_t	stStatsPar;
	OPL_API_DATA_t	stApiData;

	if (NULL == pstEnv)
	{
		return ERROR;
	}

	stStatsPar.iWrFd = pstEnv->nWriteFd;
	stStatsPar.uiMode = pstEnv->pmode;

	stApiData.apiId = HAL_API_STATS_FEDMA;
	stApiData.param = (void *)&stStatsPar;
    stApiData.length = sizeof(HAL_STATS_PAR_t);

    cliShowStart(pstEnv);
	/* iStatus = ShowCntFedma(pstEnv); */
	halAppApiCallSync(&stApiData);
    cliShowEnd(pstEnv);
	if (OK != iStatus)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nshow counter fedma error! 0x%08x\r\n", iStatus);
		return OK;
	}

	return iStatus;
}

STATUS CliShowCntGeParser(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS			iStatus=OK;
	HAL_STATS_PAR_t	stStatsPar;
	OPL_API_DATA_t	stApiData;

	if (NULL == pstEnv)
	{
		return ERROR;
	}

	stStatsPar.iWrFd = pstEnv->nWriteFd;
	stStatsPar.uiMode = pstEnv->pmode;

	stApiData.apiId = HAL_API_STATS_GEPARSER;
	stApiData.param = (void *)&stStatsPar;
    stApiData.length = sizeof(HAL_STATS_PAR_t);

    cliShowStart(pstEnv);
	/* iStatus = ShowCntGeParser(pstEnv); */
	halAppApiCallSync(&stApiData);
    cliShowEnd(pstEnv);
	if (OK != iStatus)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nshow counter geparser error! 0x%08x\r\n", iStatus);
		return OK;
	}

	return iStatus;
}

STATUS CliShowCntCleHit(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS				iStatus=OK;
	HAL_STATS_CLEHIT_t	stStatsClehit;
	OPL_API_DATA_t		stApiData;

	if ((NULL == pstEnv)||(NULL == pstPara))
	{
		return ERROR;
	}

	stStatsClehit.iWrFd = pstEnv->nWriteFd;
	stStatsClehit.usStart = pstPara[0].u;
	stStatsClehit.usNum = pstPara[1].u;

	stApiData.apiId = HAL_API_STATS_CLEHIT;
	stApiData.param = (void *)&stStatsClehit;
    stApiData.length = sizeof(HAL_STATS_CLEHIT_t);

    cliShowStart(pstEnv);
	/* iStatus = ShowCntCleHit(pstEnv, pstPara[0].u, pstPara[1].u); */
	halAppApiCallSync(&stApiData);
    cliShowEnd(pstEnv);
	if (OK != iStatus)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nshow counter cle hit error! 0x%08x\r\n", iStatus);
		return OK;
	}

	return iStatus;
}

STATUS CliShowCntSwhPort(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS				iStatus=OK;
	HAL_STATS_SWHPORT_t	stStatsSwhport;
	OPL_API_DATA_t		stApiData;

	if ((NULL == pstEnv)||(NULL == pstPara))
	{
		return ERROR;
	}

	stStatsSwhport.iWrFd = pstEnv->nWriteFd;
	stStatsSwhport.usStart = pstPara[0].u;
	stStatsSwhport.usNum = pstPara[1].u;
//	stStatsSwhport.iType = pstPara[0].i;

	stApiData.apiId = HAL_API_STATS_SWHPORT;
	stApiData.param = (void *)&stStatsSwhport;
    stApiData.length = sizeof(HAL_STATS_SWHPORT_t);

    cliShowStart(pstEnv);
	/* iStatus = ShowCntSwhPort(pstEnv, pstPara[1].u, pstPara[2].u, pstPara[0].i); */
	halAppApiCallSync(&stApiData);
    cliShowEnd(pstEnv);
	if (OK != iStatus)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nshow counter cle hit error! 0x%08x\r\n", iStatus);
		return OK;
	}

	return iStatus;
}

STATUS CliCntPollEndis(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	if ((NULL == pstEnv)||(NULL == pstPara))
	{
		return ERROR;
	}

	if (2 == pstPara[0].i)
	{
        dalCounterEnable(0xFFFFFFFF);
	}
    else {
        dalCounterEnable(0);
    }

	return OK;
}

STATUS CliCntClr(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS				iStatus=OK;
	HAL_STATS_POLL_t	stStatsPoll;
	OPL_API_DATA_t		stApiData;
	int 				i;

	if ((NULL == pstEnv)||(NULL == pstPara))
	{
		return ERROR;
	}

	if (pstPara[0].i < 14)
	{
		stStatsPoll.iModule = 1 << (pstPara[0].i-1); /* translate to bitmap */
		stStatsPoll.usStart = pstPara[2].u;
		stStatsPoll.usNum = pstPara[3].u;
		stApiData.apiId = HAL_API_STATS_CLR;
		stApiData.param = (void *)&stStatsPoll;
        stApiData.length = sizeof(HAL_STATS_POLL_t);

		halAppApiCallSync(&stApiData);
		if (OK != iStatus)
		{
			vosPrintf(pstEnv->nWriteFd, "\r\ncounter poll endis error! 0x%08x\r\n", iStatus);
			return OK;
		}
	}
	else {
		for (i = 1; i < 14; i++)
		{
			stStatsPoll.iModule = 1 << i; /* translate to bitmap */
			stStatsPoll.usStart = 1;
			stStatsPoll.usNum = 4;
			stApiData.apiId = HAL_API_STATS_CLR;
			stApiData.param = (void *)&stStatsPoll;
            stApiData.length = sizeof(HAL_STATS_POLL_t);

			halAppApiCallSync(&stApiData);
			ClearCntHost();
		}
	}

	return iStatus;
}

STATUS CliOamDbgEndis(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS iStatus=OK;

	if ((NULL == pstEnv)||(NULL == pstPara))
	{
		return ERROR;
	}

	iStatus = OamDbgEnable(pstPara[0].i-1);

	if (OK != iStatus)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\noam endis error! 0x%08x\r\n", iStatus);
		return OK;
	}

	return iStatus;
}

extern UINT32 oam_ver_type;
STATUS CliOamReportAlarm(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS iStatus = OK;
	CTC_OAM_ALARM_ID_e alarm_id;
	CTC_ALARM_SRC_t alarm_src;

	if((NULL == pstEnv))
	{
		return ERROR;
	}

	alarm_id = ONU_POWERING_VOLT_LOW_ALARM;
	alarm_src.obj_type = OBJ_ONU;
#if 0
    if (oam_ver_type == 2) /* 2.1 */
    {
	    iStatus = eopl_ctc_alarm_report(alarm_id, alarm_src, NULL, 0);
    }
#endif

	return iStatus;
}

STATUS CliOamClearAlarm(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS iStatus = OK;
	CTC_OAM_ALARM_ID_e alarm_id;
	CTC_ALARM_SRC_t alarm_src;
	
	if(NULL == pstEnv)
	{
		return ERROR;
	}
	
	alarm_id = ONU_POWERING_VOLT_LOW_ALARM;
	alarm_src.obj_type = OBJ_ONU;

#if 0
    if (oam_ver_type == 2) /* 2.1 */
    {
	    iStatus = eopl_ctc_alarm_clear(alarm_id, alarm_src, NULL, 0);
    }
#endif

	return iStatus;
}

STATUS CliOamDygaspnumConfig(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	UINT32 uiOamNum;
	STATUS iRet;
	UINT32 TempVal=0;
	UINT32 olddyinggaspOamNum;

	if (IS_ONU_RESPIN)
	{
		/* parameter check */
		if ((NULL == pstEnv) || (NULL == pstPara))
		{
			OPL_LOG_TRACE();
			return ERR_NULL_POINTER;
		}

		uiOamNum = pstPara[0].u;
		if((uiOamNum<0)||(uiOamNum>127))
		{
			vosPrintf(pstEnv->nWriteFd, "Invalid input.\r\n");
			return QOS_INIT_ERROR;
		}

		iRet = oplRegRead(0x4030*4,&TempVal);
	    if(OK != iRet)
	    {
			vosPrintf(pstEnv->nWriteFd, "Read 0x4030 failed.\r\n");
	        return QOS_INIT_ERROR;
	    }
	    else
	    {
	    	if(!((TempVal&0x80000000)>>31))
	    	{
	    		iRet += vosConfigKeyIsExisted(CFG_MPCP_DYINGGASP_CFG,CFG_MPCP_DYINGGASP_SECTION,CFG_MPCP_DYINGGASP_OAMNUM);
			olddyinggaspOamNum = vosConfigUInt32Get(CFG_MPCP_DYINGGASP_CFG,CFG_MPCP_DYINGGASP_SECTION,CFG_MPCP_DYINGGASP_OAMNUM,0);
			if((OK == iRet && olddyinggaspOamNum != uiOamNum)||(OK != iRet))
			{
				TempVal = TempVal | 0x80000000;
				iRet = oplRegWrite(0x4030*4, TempVal);
				iRet += oplRegFieldWrite(0x4030*4, 24, 7, uiOamNum);
			}
			vosConfigUInt32Set(CFG_MPCP_DYINGGASP_CFG,CFG_MPCP_DYINGGASP_SECTION,CFG_MPCP_DYINGGASP_OAMNUM,uiOamNum);
	    	}
	    	else
	    	{
			iRet += vosConfigKeyIsExisted(CFG_MPCP_DYINGGASP_CFG,CFG_MPCP_DYINGGASP_SECTION,CFG_MPCP_DYINGGASP_OAMNUM);
			olddyinggaspOamNum = vosConfigUInt32Get(CFG_MPCP_DYINGGASP_CFG,CFG_MPCP_DYINGGASP_SECTION,CFG_MPCP_DYINGGASP_OAMNUM,0);
			if((OK == iRet && olddyinggaspOamNum != uiOamNum)||(OK != iRet))
			{
				iRet += oplRegFieldWrite(0x4030*4, 24, 7, uiOamNum);
			}
			vosConfigUInt32Set(CFG_MPCP_DYINGGASP_CFG,CFG_MPCP_DYINGGASP_SECTION,CFG_MPCP_DYINGGASP_OAMNUM,uiOamNum);
	    	}
	    }

		if (NO_ERROR != iRet)
		{
			OPL_LOG_TRACE();
			return iRet;
		}
	}
	else
	{
		vosPrintf(pstEnv->nWriteFd, "%%Not support.\r\n");
	}

	return NO_ERROR;
}

extern UINT8   oam_cfg_ver_type;
STATUS CliOamVersionSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS iStatus=OK;
    UINT8   value;

	if ((NULL == pstEnv)||(NULL == pstPara))
	{
		return ERROR;
	}

    switch (pstPara[0].i)
    {
        case 1:
            oam_cfg_ver_type = OAM_VER_CTC_2_0_SUPPORT;
            vosPrintf(pstEnv->nWriteFd, "\r\nSet OAM to Support CTC 2.0 Only\r\n");
            break;
        case 2 :
            oam_cfg_ver_type = OAM_VER_CTC_2_1_SUPPORT;
            vosPrintf(pstEnv->nWriteFd, "\r\nSet OAM to Support CTC 2.1 Only\r\n");
            break;
        case 3:
            oam_cfg_ver_type = OAM_VER_CTC_20_21_MIX_SUPPORT;
            vosPrintf(pstEnv->nWriteFd, "\r\nSet OAM to Support Both  CTC 2.1 and CTC 2.0 \r\n");
            break;
        default :
            vosPrintf(pstEnv->nWriteFd, "\r\nInput error OAM value\r\n");

    }

    value = pstPara[0].i ;

    odmPonOAMVerSet(&value);

	return iStatus;
}

