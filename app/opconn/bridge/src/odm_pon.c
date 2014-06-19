/*
=============================================================================
     File Name: pon_mac.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/20		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#include "odm_pon.h"
#include "hal.h"
#include "hal_onu.h"
#include "odm_port.h"
#include "log.h"
#include "ctc_2_1.h"
#include "oam.h"
#include "lw_type.h"
#include  "lw_drv_pub.h"
#include  "lw_drv_req.h"

static TIMER_OBJ_t * g_pstTimePonBer = NULL;
static float berArry[4] = {0x0};
static UINT32 berIndex = 0x0;

UINT8 g_onuSilenceEn;
static UINT32 g_onuSilenceTime = 60;
#define MODULE MOD_PON

extern UINT8 oam_src_mac[6];
extern int g_iOamAuthCode;
extern UINT8 g_ucLOSInt;

#define OFFLINE_PRINT_LEVEL DEBUG_LEVEL_CRITICAL

UINT32 odmPonSilenceEnableSet(UINT32 enable)
{
	UINT32  retVal = OK;
	UINT32  oldEnable;
	UINT8 	ponSectionBuff[255] = {0};

	vosSprintf(ponSectionBuff,CFG_PON_MAC_SECTION);
	retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG,ponSectionBuff,CFG_ONU_SILENCE_CTRL);
	oldEnable = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_ONU_SILENCE_CTRL,ODM_PON_DEFAULT_SILENCE_CTRL);	
	if((OK == retVal && oldEnable != enable)||(OK != retVal))
	{
		g_onuSilenceEn = enable;

		vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_ONU_SILENCE_CTRL, enable);

        /* change to one shot register */
        if (!g_onuSilenceEn)
        {
            odmPonRegSet(0x4006,0x44);
        }

	}

	return OK;
}

UINT32 odmPonSilenceEnableGet(INT32 *pEnable)
{
	UINT8 	ponSectionBuff[255] = {0};

	if(NULL == pEnable)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(ponSectionBuff,CFG_PON_MAC_SECTION);

	*pEnable = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_ONU_SILENCE_CTRL,ODM_PON_DEFAULT_SILENCE_CTRL);	

	return OK;
}


UINT32 odmPonSilenceTimeSet(UINT32 silenceTime)
{
	UINT32  retVal = OK;
	UINT32  oldSilenceTime;
	UINT8 	ponSectionBuff[255] = {0};

	vosSprintf(ponSectionBuff,CFG_PON_MAC_SECTION);
	retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG,ponSectionBuff,CFG_ONU_SILENCE_TIME);
	oldSilenceTime = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_ONU_SILENCE_TIME,ODM_PON_DEFAULT_SILENCE_TIME);	
	if((OK == retVal && oldSilenceTime != silenceTime)||(OK != retVal))
	{
		g_onuSilenceTime = silenceTime;

		vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_ONU_SILENCE_TIME, silenceTime);
	}

	return OK;

}

UINT32 odmPonSilenceTimeGet(UINT32 *pSilenceTime)
{
	UINT8 	ponSectionBuff[255] = {0};

	if(NULL == pSilenceTime)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(ponSectionBuff,CFG_PON_MAC_SECTION);

	*pSilenceTime = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_ONU_SILENCE_TIME,ODM_PON_DEFAULT_SILENCE_TIME);	

	return OK;
}


INT32 OdmPonBerCollectTimer(void)
{
    OPL_STATUS retVal = OPL_OK;
    float berReal = 0x0;
    float averageBer = 0x0;
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;

    stData.apiId = HAL_API_ONU_PON_MAC_BER_GET;
    stData.length = sizeof(HAL_ONU_PON_CFG_t);
    stData.param = (void *)&halOnuPonCfg;
    retVal = halAppApiCallSync(&stData);  
    if(OK != retVal)
    {
        ODM_PON_TRACE();
        return ERROR;
    }
    
    berArry[berIndex%4] = halOnuPonCfg.realTimeBer;

    if(berIndex == 3)
    {
        averageBer = berArry[0]+berArry[1]+berArry[2]+berArry[3];
        if(CFG_PON_BER_MAX <= averageBer)
        {
            ODM_PON_PRINTF(("pon mac bit error rate:%15.15f\n",averageBer));
        }
    }

    berIndex = (berIndex+1)%4;
    
    return retVal;   
}

INT32 OdmPonBerCollectStart(void)
{
    g_pstTimePonBer = vosTimerGetFree();

    if (g_pstTimePonBer)
    {
        vosTimerStart(g_pstTimePonBer, REOCURRING_TIMER, 1000, (FUNCPTR)OdmPonBerCollectTimer,3333,3333,3,4,5,6,7,8);
    }else
    {
        return ERROR;
    }
    return OK;
}

INT32 OdmPonBerCollectStop(void)
{
    vosTimerCancel(g_pstTimePonBer);
    g_pstTimePonBer = NULL;

    return OPL_OK;
}

UINT32 odmPonLaserOnSet(UINT32 laserOnTq)
{
	UINT32  retVal = OK;
	UINT32  oldLaserOn;
	UINT8 	ponSectionBuff[255] = {0};
	INT32   halStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;

    if(ODM_PON_MAX_LASERON <= laserOnTq)
    {
        return ERR_INVALID_PARAMETERS;
    }
	vosSprintf(ponSectionBuff,CFG_PON_MAC_SECTION);
	retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_LASER_ON);
	oldLaserOn = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_LASER_ON,ODM_PON_DEFAULT_LASERON);	
	if((OK == retVal && oldLaserOn != laserOnTq)||(OK != retVal))
	{
	    stData.apiId = HAL_API_ONU_LASER_ONCNT_SET;
        stData.length = sizeof(HAL_ONU_PON_CFG_t);
        stData.param = (void *)&halOnuPonCfg;
        halOnuPonCfg.laserOnTime = laserOnTq;
        halStatus = halAppApiCallSync(&stData);  
		
		if(OK != halStatus)
		{
			return halStatus;
		}
		vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_LASER_ON,laserOnTq);	
	}
	return OK;
}

UINT32 odmPonLaserOnGet(UINT32 *laserOnTq)
{
	UINT8 	ponSectionBuff[255] = {0};
    if(NULL == laserOnTq)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(ponSectionBuff,CFG_PON_MAC_SECTION);
	*laserOnTq = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_LASER_ON,ODM_PON_DEFAULT_LASERON);	
	return OK;
}

UINT32 odmPonTxPolarityConfig(UINT32 loworhigh)
{
	UINT32  retVal = OK;
	INT32   halStatus = OK;
	UINT32  oldpontxpolarity;
	UINT8 	ponSectionBuff[255] = {0};
	OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;
	if((0 != loworhigh) && (1 != loworhigh))
	{
		return ERR_INVALID_PARAMETERS;
	}
	if (IS_ONU_RESPIN)
	{
		retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_TX_POLARITY);
		oldpontxpolarity = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_TX_POLARITY,PON_TX_LOW_ACTIVE);
		if((OK == retVal && oldpontxpolarity != loworhigh)||(OK != retVal))
		{
			stData.apiId = HAL_API_ONU_PONTX_POLARITY_CONFIG;
			stData.length = sizeof(HAL_ONU_PON_CFG_t);
			stData.param = (void *)&halOnuPonCfg;
			halOnuPonCfg.PontxPolarity = loworhigh;
			halStatus = halAppApiCallSync(&stData);  
			if(OK != halStatus)
			{
				return halStatus;
			}
			vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_TX_POLARITY,loworhigh);	
		}
	}
	return OK;
}

UINT32 odmPonLaserOffSet(UINT32 laserOffTq)
{
	UINT32  retVal = OK;
	UINT32  oldLaserOff;
	UINT8 	ponSectionBuff[255] = {0};
	INT32   halStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;
    
    if(ODM_PON_MAX_LASEROFF <= laserOffTq)
    {
        return ERR_INVALID_PARAMETERS;
    }
	vosSprintf(ponSectionBuff,CFG_PON_MAC_SECTION);
	
	retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_LASER_OFF);
	oldLaserOff = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_LASER_OFF,ODM_PON_DEFAULT_LASEROFF);	
	if((OK == retVal && oldLaserOff != laserOffTq)||(OK != retVal))
	{
	    stData.apiId = HAL_API_ONU_LASER_OFFCNT_SET;
        stData.length = sizeof(HAL_ONU_PON_CFG_t);
        stData.param = (void *)&halOnuPonCfg;
        halOnuPonCfg.laserOffTime = laserOffTq;
        halStatus = halAppApiCallSync(&stData);  

		if(OK != halStatus)
		{
			return halStatus;
		}
		vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_LASER_OFF,laserOffTq);	
	}
	return OK;
}

UINT32 odmPonLaserOffGet(UINT32 *laserOffTq)
{
	UINT8 	ponSectionBuff[255] = {0};
    if(NULL == laserOffTq)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(ponSectionBuff,CFG_PON_MAC_SECTION);
	*laserOffTq = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_LASER_OFF,ODM_PON_DEFAULT_LASEROFF);	
	return OK;
}

UINT32 odmPonSerdesDelaySet(UINT32 serdesDelayByte)
{
	UINT32  retVal = OK;
	UINT32  oldSerdesDelayCount;
	UINT8 	ponSectionBuff[255] = {0};
	INT32   halStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;

    if(ODM_PON_MAX_SERDESDELAY <= serdesDelayByte)
    {
        return ERR_INVALID_PARAMETERS;
    }
	vosSprintf(ponSectionBuff,CFG_PON_MAC_SECTION);
	retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_SERDES_DELAY);
	oldSerdesDelayCount = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_SERDES_DELAY,ODM_PON_DEFAULT_SERDESDELAY);	
	if((OK == retVal && oldSerdesDelayCount != serdesDelayByte)||(OK != retVal))
	{
		stData.apiId = HAL_API_ONU_SERDES_DELAYCNT_SET;
        stData.length = sizeof(HAL_ONU_PON_CFG_t);
        stData.param = (void *)&halOnuPonCfg;
        halOnuPonCfg.serdesDelay = serdesDelayByte;
        halStatus = halAppApiCallSync(&stData);  
		if(OK != halStatus)
		{
			return halStatus;
		}
 		vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_SERDES_DELAY,serdesDelayByte);	
	}
	return OK;
}

UINT32 odmPonSerdesDelayGet(UINT32 *serdesDelayByte)
{
	UINT8 	ponSectionBuff[255] = {0};
    if(NULL == serdesDelayByte)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(ponSectionBuff,CFG_PON_MAC_SECTION);
	*serdesDelayByte = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_SERDES_DELAY,ODM_PON_DEFAULT_SERDESDELAY);	
	return OK;
}

UINT32 odmPonLoidSet(UINT8 *pLoid)
{
	UINT32  retVal = OK;
    UINT8 	ponSectionBuff[255] = {0};
    
    if(NULL == pLoid)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(ponSectionBuff,CFG_PON_LOID_SECTION);

	retVal = vosConfigValueSet(CFG_PON_LOID_CFG,ponSectionBuff,CFG_PON_LOID_KEY,pLoid);	
    if (0 != retVal)
    {
        return ERR_CONFIG_SET_FAIL;
    }

    retVal = vosConfigFlushByModule(CFG_PON_LOID_CFG);
    if (0 != retVal)
    {
        return ERR_CONFIG_SET_FAIL;
    }

	return retVal;
}

UINT32 odmPonLoidGet(UINT8 *pLoid)
{
	UINT32  retVal = OK;
    UINT8 	ponSectionBuff[255] = {0};
    UINT8   *pTmp = NULL;
    UINT32  value = 1;
	UINT8 	length;

	if(NULL == pLoid)
    {
        return ERR_NULL_POINTER;
    }
	
	vosSprintf(ponSectionBuff,CFG_PON_LOID_SECTION);
    pTmp = vosConfigValueGet(CFG_PON_LOID_CFG,ponSectionBuff,CFG_PON_LOID_KEY, "opconn");

	if(pTmp == 0)
	{
		retVal = ERR_NULL_LOID;
		return retVal;
	}
	
	length = vosStrLen(pTmp);

	if(length > 24)
		length = 24;

	strncpy(pLoid, pTmp, length);
	*(pLoid+length) = '\0';

	return retVal;
}

UINT32 odmPonOAMVerSet(UINT8 *p_ver)
{
	UINT32  retVal = OK;
    UINT32  value = (UINT32) *p_ver;

    if(NULL == p_ver)
    {
        return ERR_NULL_POINTER;
    }

    retVal = vosConfigUInt32Set(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_OAM_VER_CFG,value);
    if (0 != retVal)
    {
        return ERR_CONFIG_SET_FAIL;
    }

	return retVal;
}

UINT8 odmPonOAMVerGet(UINT8 *p_ver)
{
	if(NULL == p_ver)
    {
        return ERR_NULL_POINTER;
    }

    /* by default, OAM should be supported OAM_VER_CTC_20_21_MIX_SUPPORT */
	*p_ver  = (UINT8)vosConfigUInt32Get(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_OAM_VER_CFG,3);

	return *p_ver;
}
UINT32 odmPonPasswordSet(UINT8 *pPassword)
{
	UINT32  retVal = OK;
    UINT8 	ponSectionBuff[255] = {0};
    UINT8   encryptedPasswd[13];

	vosSprintf(ponSectionBuff,CFG_PON_LOID_SECTION);
    
    if(NULL == pPassword)
    {
        retVal = vosConfigKeyDelete(CFG_PON_LOID_CFG,ponSectionBuff,CFG_PON_LOID_PASSWORD);
        if (0 != retVal)
        {
            return ERR_CONFIG_SET_FAIL;
        }
        
        retVal = vosConfigFlushByModule(CFG_PON_LOID_CFG);
        if (0 != retVal)
        {
            return ERR_CONFIG_SET_FAIL;
        }
        
        return retVal;
    }

	vosMemSet(&encryptedPasswd[0], 0, 13);
	//modified by tanglin ,for the bug 3005,add the judgement of the length of pPassword.
	//vosMemCpy(&encryptedPasswd[0], pPassword, vosStrLen(pPassword));
	 if(vosStrLen(pPassword)>12)
         {
                   vosMemCpy(&encryptedPasswd[0], pPassword,12);
         }
         else
         {
                   vosMemCpy(&encryptedPasswd[0],pPassword,vosStrLen(pPassword));
         }


	odmUserEncryptString(encryptedPasswd);
	

	retVal = vosConfigValueSet(CFG_PON_LOID_CFG,ponSectionBuff,CFG_PON_LOID_PASSWORD,&encryptedPasswd[0]);	
    if (0 != retVal)
    {
        return ERR_CONFIG_SET_FAIL;
    }

    retVal = vosConfigFlushByModule(CFG_PON_LOID_CFG);
    if (0 != retVal)
    {
        return ERR_CONFIG_SET_FAIL;
    }

	return retVal;	
}

UINT32 odmPonPasswordGet(UINT8 *pPassword)
{
	UINT32  retVal = OK;
    UINT8 	ponSectionBuff[255] = {0};
    UINT8   *pTmp = NULL;
	UINT8 	length;

	if(NULL == pPassword)
    {
        return ERR_NULL_POINTER;
    }
	
	vosSprintf(ponSectionBuff,CFG_PON_LOID_SECTION);
    pTmp = vosConfigValueGet(CFG_PON_LOID_CFG,ponSectionBuff,CFG_PON_LOID_PASSWORD, 0);

	if(pTmp == 0)
	{
		retVal = ERR_NULL_PASSWORD;
		return retVal;
	}
	
	length = vosStrLen(pTmp);

	if(length > 12)
		length = 12;

	strncpy(pPassword, pTmp, length);
	*(pPassword+length) = '\0';
	
	return retVal;
}

UINT32 odmPonSnSet(UINT8 *pSn)
{
	UINT32  retVal = OK;
    UINT8 	snSectionBuff[255] = {0};

    if(NULL == pSn)
    {
        return ERR_NULL_POINTER;
    }

	vosSprintf(snSectionBuff,CFG_PON_MAC_SECTION);
	retVal = vosConfigValueSet(CFG_PON_MAC_CFG,snSectionBuff,CFG_PON_LOID_SN,pSn);
    if (0 != retVal)
    {
        return ERR_CONFIG_SET_FAIL;
    }
    retVal = vosConfigFlushByModule(CFG_PON_MAC_CFG);
    if (0 != retVal)
    {
        return ERR_CONFIG_SET_FAIL;
    }

	return retVal;
}

UINT32 odmPonSnGet(UINT8 *pSn)
{
	UINT32  retVal = OK;
	UINT8 	snSectionBuff[255] = {0};
	UINT8   *pTmp = NULL;
	UINT8 	length;
    UINT8   aucMac[6] = {0};
    UINT8   aucMacString[129] = {0};

	if(NULL == pSn)
	{
		return ERR_NULL_POINTER;
	}

    odmPonMacIdGet(aucMac);
    vosSprintf(aucMacString,"%02x%02x%02x%02x%02x%02x",
        aucMac[0],
        aucMac[1],
        aucMac[2],
        aucMac[3],
        aucMac[4],
        aucMac[5]);
	vosSprintf(snSectionBuff,CFG_PON_MAC_SECTION);
	pTmp = vosConfigValueGet(CFG_PON_MAC_CFG,snSectionBuff,CFG_PON_LOID_SN, aucMacString);

	if(pTmp == 0)
	{
		retVal = ERR_NULL_LOID;
		return retVal;
	}

	length = vosStrLen(pTmp);

	if(length > 128)
		length = 128;

	strncpy(pSn, pTmp, length);
	*(pSn+length) = '\0';

	return retVal;
}

UINT32 odmPonMacIdSet(UINT8 macid[])
{
	UINT32  retVal = OK;
    UINT8 	ponSectionBuff[255] = {0};
	UINT8   oldMacId[6] = {0};
    UINT8   macIdString[20] = {0};
    UINT8   *p = NULL;
	INT32   halStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;
    
    if(NULL == macid)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(ponSectionBuff,CFG_PON_MAC_SECTION);

    vosSprintf(macIdString,"%02x:%02x:%02x:%02x:%02x:%02x",
        macid[0],
        macid[1],
        macid[2],
        macid[3],
        macid[4],
        macid[5]);

	stData.apiId = HAL_API_ONU_MACID_SET;
    stData.length = sizeof(HAL_ONU_PON_CFG_t);
    stData.param = (void *)&halOnuPonCfg;
    vosMemCpy(halOnuPonCfg.ponMac,macid,6);
    halStatus = halAppApiCallSync(&stData);  
	if(OK != halStatus)
	{
		return halStatus;
	}
	vosConfigValueSet(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_MACID,macIdString);	
	return OK;
}

UINT32 odmPonMacIdGet(UINT8 macid[])
{
    UINT8 	ponSectionBuff[255] = {0};
    UINT8   *p = NULL;
    if(NULL == macid)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(ponSectionBuff,CFG_PON_MAC_SECTION);
    p = vosConfigValueGet(CFG_PON_MAC_CFG,ponSectionBuff,CFG_PON_MACID,"00:00:00:00:00:00");
    string2mac(p,macid);
	return OK;
}

UINT32 odmPonDbaAgentAllParaSet(UINT32 cpRptSendMode,UINT32 cpDeltaTxNormalFecTime,UINT32 cpDeltaTxNormalNoFecTime,
    UINT32 cpTxMpcpFecTime,UINT32 cpTxMpcpNoFecTime)
{
    UINT32  retVal = OK;
    UINT8   ponSectionBuff[255] = {0};
    INT32   halStatus = OK;

    if(TRUE != cpRptSendMode && FALSE != cpRptSendMode)
    {
        return ERR_INVALID_PARAMETERS;
    }
    if(127 < cpDeltaTxNormalFecTime || 127 < cpDeltaTxNormalNoFecTime || 127 < cpTxMpcpFecTime || 127 < cpTxMpcpNoFecTime)
    {
        return ERR_INVALID_PARAMETERS;
    }
    vosSprintf(ponSectionBuff,CFG_PON_DBA_SECTION);

#if 0
    retVal = dalPonMpcpDbaAgentConfigAllSet(cpRptSendMode,cpDeltaTxNormalFecTime,cpDeltaTxNormalNoFecTime,
        cpTxMpcpFecTime,cpTxMpcpNoFecTime);
#endif

    if(OK != retVal)
    {
        return retVal;
    }
    
    vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_RPT_SNED_MODE,cpRptSendMode);	
    vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_DELTA_TX_NORMAL_FEC_TIME,cpDeltaTxNormalFecTime);	
    vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_DELTA_TX_NORMAL_NO_FEC_TIME,cpDeltaTxNormalNoFecTime);
    vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_DELTA_TX_MPCP_FEC_TIME,cpTxMpcpFecTime);
    vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_DELTA_TX_MPCP_NO_FEC_TIME,cpTxMpcpNoFecTime);
    return OK;
}

UINT32 odmPonDbaAgentAllParaGet(UINT32 *cpRptSendMode,UINT32 *cpDeltaTxNormalFecTime,UINT32 *cpDeltaTxNormalNoFecTime,
    UINT32 *cpTxMpcpFecTime,UINT32 *cpTxMpcpNoFecTime)
{
    UINT32  retVal = OK;
    UINT8   ponSectionBuff[255] = {0};
    INT32   halStatus = OK;

    if(NULL == cpRptSendMode || NULL == cpDeltaTxNormalFecTime
        || NULL == cpDeltaTxNormalNoFecTime
        || NULL == cpTxMpcpFecTime
        || NULL == cpTxMpcpNoFecTime)
    {
        return ERR_NULL_POINTER;
    }
    vosSprintf(ponSectionBuff,CFG_PON_DBA_SECTION);
    *cpRptSendMode = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_RPT_SNED_MODE,0);	
    *cpDeltaTxNormalFecTime = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_DELTA_TX_NORMAL_FEC_TIME,73);	
    *cpDeltaTxNormalNoFecTime = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_DELTA_TX_NORMAL_NO_FEC_TIME,52);
    *cpTxMpcpFecTime = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_DELTA_TX_MPCP_FEC_TIME,58);
    *cpTxMpcpNoFecTime= vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_DELTA_TX_MPCP_NO_FEC_TIME,42);
    if(TRUE != *cpRptSendMode && FALSE != *cpRptSendMode)
    {
        return ERR_INVALID_PARAMETERS;
    }
    if(127 < *cpDeltaTxNormalFecTime || 127 < *cpDeltaTxNormalNoFecTime || 127 < *cpTxMpcpFecTime || 127 < *cpTxMpcpNoFecTime)
    {
        return ERR_INVALID_PARAMETERS;
    }
    return OK;
}

UINT32 odmPonTxDelaySet(UINT32  cpRttCompenEn,UINT32 cpRttCompenSyncEn,UINT32 cpRttCorrectEn,UINT32 cpRttCorrectSyncEn,
	UINT32 cpTxDlyEn,UINT32 cpSyncEn,UINT32 cpRptsentDelt,UINT32 cpRttOffset,UINT32 cpTxDly)
{
	UINT32  retVal = OK;
    UINT8 	ponSectionBuff[255] = {0};
	INT32   halStatus = OK;
    if((TRUE != cpRttCompenEn && FALSE != cpRttCompenEn)||
        (TRUE != cpRttCompenSyncEn && FALSE != cpRttCompenSyncEn)||
        (TRUE != cpRttCorrectEn && FALSE != cpRttCorrectEn)||
        (TRUE != cpRttCorrectSyncEn && FALSE != cpRttCorrectSyncEn)||
        (TRUE != cpTxDlyEn && FALSE != cpTxDlyEn)||
        (TRUE != cpSyncEn && FALSE != cpSyncEn))
{
        return ERR_INVALID_PARAMETERS;
    }

    if(63 < cpRptsentDelt)
    {
        return ERR_INVALID_PARAMETERS;
    }

    if(1023 < cpRttOffset)
    {
        return ERR_INVALID_PARAMETERS;
    }

    if(1023 < cpTxDly)
    {
        return ERR_INVALID_PARAMETERS;
    }

#if 0
    retVal = dalPonDelayConfigAllSet(cpRttCompenEn,cpRttCompenSyncEn,cpRttCorrectEn,cpRttCorrectSyncEn,
        cpTxDlyEn,cpSyncEn,cpRptsentDelt,cpRttOffset,cpTxDly);
#endif

    if(OPL_OK != retVal)
    {
        return retVal;
    }
    
    vosSprintf(ponSectionBuff,CFG_PON_DELAY_SECTION);
    vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_RTTCOMPEN_EN,cpRttCompenEn);	
    vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_RTTCOMPEN_SYNC_EN,cpRttCompenSyncEn);
    vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_RTTCORRECT_EN,cpRttCorrectEn);
    vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_RTTCORRECT_SYNC_EN,cpRttCorrectSyncEn);
    vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_TXDLY_EN,cpTxDlyEn);
    vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_SYNC_EN,cpSyncEn);
    vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_RPTSENT_DELT,cpRptsentDelt);
    vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_RTT_OFFSET,cpRttOffset);
    vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_TX_DLY,cpTxDly);
	return OK;    
}

UINT32 odmPonTxDelayGet(UINT32  *cpRttCompenEn,UINT32 *cpRttCompenSyncEn,UINT32 *cpRttCorrectEn,UINT32 *cpRttCorrectSyncEn,
	UINT32 *cpTxDlyEn,UINT32 *cpSyncEn,UINT32 *cpRptsentDelt,UINT32 *cpRttOffset,UINT32 *cpTxDly)
{
	UINT32  retVal = OK;
    UINT8 	ponSectionBuff[255] = {0};
	INT32   halStatus = OK;
	
    if(NULL == cpRttCompenEn || NULL == cpRttCompenSyncEn
        || NULL == cpRttCorrectEn
        || NULL == cpRttCorrectSyncEn
        || NULL == cpTxDlyEn
        || NULL == cpSyncEn
        || NULL == cpRptsentDelt
        || NULL == cpRttOffset
        || NULL == cpTxDly)
    {
        return ERR_NULL_POINTER;
    }

    vosSprintf(ponSectionBuff,CFG_PON_DELAY_SECTION);
    *cpRttCompenEn = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_RTTCOMPEN_EN,FALSE);	
    *cpRttCompenSyncEn = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_RTTCOMPEN_SYNC_EN,FALSE);
    *cpRttCorrectEn = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_RTTCORRECT_EN,FALSE);
    *cpRttCorrectSyncEn = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_RTTCORRECT_SYNC_EN,FALSE);
    *cpTxDlyEn = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_TXDLY_EN,1);
    *cpSyncEn = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_SYNC_EN,1);
    *cpRptsentDelt = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_RPTSENT_DELT,2);	
    *cpRttOffset = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_RTT_OFFSET,0);
    *cpTxDly = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_CP_TX_DLY,32);
#if 0
    printf("\n%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
        *cpRttCompenEn,
        *cpRttCompenSyncEn,
        *cpRttCorrectEn,
        *cpRttCorrectSyncEn,
        *cpTxDlyEn,
        *cpSyncEn,
        *cpRptsentDelt,
        *cpRttOffset,
        *cpTxDly);
#endif
    if((TRUE != *cpRttCompenEn && FALSE != *cpRttCompenEn)||
        (TRUE != *cpRttCompenSyncEn && FALSE != *cpRttCompenSyncEn)||
        (TRUE != *cpRttCorrectEn && FALSE != *cpRttCorrectEn)||
        (TRUE != *cpRttCorrectSyncEn && FALSE != *cpRttCorrectSyncEn)||
        (TRUE != *cpTxDlyEn && FALSE != *cpTxDlyEn)||
        (TRUE != *cpSyncEn && FALSE != *cpSyncEn))
    {
        return ERR_INVALID_PARAMETERS;
    }

    if(63 < *cpRptsentDelt)
    {
        return ERR_INVALID_PARAMETERS;
    }

    if(1023 < *cpRttOffset)
    {
        return ERR_INVALID_PARAMETERS;
    }
	
    if(1023 < *cpTxDly)
    {
        return ERR_INVALID_PARAMETERS;
    }
	return OK;    
}

UINT32 odmPonReportModeSet(UINT32 mode)
{
    UINT32  retVal = OK;
    UINT32  dalOldMode;
    OPL_API_DATA_t      stData;
    HAL_ONU_DBA_CFG_t   halOnuDbaCfg;
    
    if(ODM_PON_REPORT_65535 != mode && ODM_PON_REPORT_BASE_THRESHOLD != mode)
    {
        return ERR_INVALID_PARAMETERS;
    }

    retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG,CFG_REPORT_SECTION,CFG_REPORT_MODE);
    dalOldMode = vosConfigUInt32Get(CFG_PON_MAC_CFG,CFG_REPORT_SECTION,CFG_REPORT_MODE,
        ODM_PON_REPORT_65535);

    if((OK == retVal && dalOldMode != mode)||(OK != retVal))
    {
        halOnuDbaCfg.reportMode = mode;
        stData.apiId = HAL_API_ONU_DBA_RPT_MODE_SET;
        stData.length = sizeof(HAL_ONU_DBA_CFG_t);
        stData.param = (void *)&halOnuDbaCfg;
         
        retVal = halAppApiCallSync(&stData);  
        retVal = OK;
        if(OK != retVal)
        {
            return retVal;
        }
        vosConfigUInt32Set(CFG_PON_MAC_CFG,CFG_REPORT_SECTION,CFG_REPORT_MODE,mode);
    }
    return OK;
}

UINT32 odmPonReportModeGet(UINT32 *mode)
{
    if(NULL == mode)
    {
        return ERR_NULL_POINTER;
    }

    *mode = vosConfigUInt32Get(CFG_PON_MAC_CFG,CFG_REPORT_SECTION,CFG_REPORT_MODE,
        ODM_PON_REPORT_65535);

    return OK;
}

UINT32 odmPonQsetThresholdSet(UINT8 QsetNum,UINT8 QueueNum,UINT16 thresholdVal)
{
    UINT32  retVal = OK;
    UINT16  oldThreshold;
    UINT8   ponSectionBuff[255] = {0};
    UINT8   ponKeyBuff[255] = {0};
    UINT16  qsetDefaultTh[2] = {ODM_QSET0_DEFAULT_THRESHOLD,ODM_QSET1_DEFAULT_THRESHOLD};
    OPL_API_DATA_t      stData;
    HAL_ONU_DBA_CFG_t   halOnuDbaCfg;

    if(QsetNum >= ODM_Q_SET_NUM || QueueNum >= ODM_Q_NUM)
    {
        /*for the other device,i can only return ok,or it cannot work*/
        return OK;
    }

    vosSprintf(ponSectionBuff,CFG_QSET_SECTION,QsetNum);
    vosSprintf(ponKeyBuff,CFG_QSET_THRESHOLD,QueueNum);

    retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG,ponSectionBuff,ponKeyBuff);
    oldThreshold = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,ponKeyBuff,
        qsetDefaultTh[QsetNum]);

    if((OK == retVal && oldThreshold != thresholdVal)||(OK != retVal))
    {
        halOnuDbaCfg.qSetNum = QsetNum;
        halOnuDbaCfg.qNum = QueueNum;
        halOnuDbaCfg.threshold = thresholdVal;
        stData.apiId = HAL_API_ONU_DBA_QSET_Q_THRESHOLD_SET;
        stData.length = sizeof(HAL_ONU_DBA_CFG_t);
        stData.param = (void *)&halOnuDbaCfg;
         
        retVal = halAppApiCallSync(&stData);  
        
        if(OK != retVal)
        {
            return retVal;
        }
        vosConfigUInt32Set(CFG_PON_MAC_CFG,ponSectionBuff,ponKeyBuff,thresholdVal);
    }
    return OK;
}

UINT32 odmPonQsetThresholdGet(UINT8 QsetNum,UINT8 QueueNum,UINT16 *thresholdVal)
{
    UINT32  retVal = OK;
    UINT8   ponSectionBuff[255] = {0};
    UINT8   ponKeyBuff[255] = {0};
    UINT16  qsetDefaultTh[2] = {ODM_QSET0_DEFAULT_THRESHOLD,ODM_QSET1_DEFAULT_THRESHOLD};

    if(QsetNum >= ODM_Q_SET_NUM || QueueNum >= ODM_Q_NUM)
    {
        *thresholdVal = 0XFFFF;
        return OK;
    }

    if(NULL == thresholdVal)
    {
        return ERR_NULL_POINTER;
    }

    vosSprintf(ponSectionBuff,CFG_QSET_SECTION,QsetNum);
    vosSprintf(ponKeyBuff,CFG_QSET_THRESHOLD,QueueNum);

    *thresholdVal = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,ponKeyBuff,
        qsetDefaultTh[QsetNum]);

    return OK;
}

UINT32 odmPonRegSet(UINT32 regId,UINT32 regVal)
{
    UINT32 retVal;

    HAL_API_GBL_REG_SET(regId, regVal, retVal);

    return retVal;
}

UINT32 odmPonRegGet(UINT32 regId,UINT32 *pVal)
{
    UINT32 retVal;

    if (NULL == pVal)
    {
        return ERR_NULL_POINTER;
    }

    HAL_API_GBL_REG_GET(regId, *pVal, retVal);

    return retVal;
}

UINT32 odmPonMultiTabGet(INT32 nWriteFd,UINT32 tabId,UINT32 startRecId,UINT32 numOfRec)
{
    STATUS retVal;
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_ORT_t stRegParam;

    stRegParam.ulRegion = tabId;
    stRegParam.ulRecordStart = startRecId;
    stRegParam.ulRecordNum = numOfRec;
    stRegParam.ulFd = nWriteFd;

    stApiData.apiId = HAL_API_GBL_ORT;
    stApiData.length = sizeof(HAL_API_GBL_ORT_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    retVal = halAppApiCallSync(&stApiData);

    return retVal;
}

UINT32 odmPonTabInfoGet(INT32 nWriteFd)
{
    STATUS retVal;
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_OST_t stRegParam;

    stRegParam.ulFd = nWriteFd;

    stApiData.apiId = HAL_API_GBL_OST;
    stApiData.length = sizeof(HAL_API_GBL_OST_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    retVal = halAppApiCallSync(&stApiData);

    return retVal;
}

UINT32 odmPonUnicastTripleChunringKeyGet(UINT8 keyIndex,UINT8 *keyBuff)
{  
    INT32 retVal;
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;

    halOnuPonCfg.secStatus = TRUE;
    halOnuPonCfg.secKey = keyBuff;
    halOnuPonCfg.secKeyIndex = keyIndex;
    halOnuPonCfg.secMode = HAL_PON_SECU_MODE_TRIPLE_CHURNING;
    halOnuPonCfg.channelOfSec = HAL_PON_SECU_CHANNEL_UNICAST;
    stData.apiId = HAL_API_ONU_UNI_SECURITY_KEY_GET;
    stData.param = (void *)&halOnuPonCfg;
    stData.length = sizeof(HAL_ONU_PON_CFG_t);
    retVal = halAppApiCallSync(&stData);
    return retVal;
}

UINT32 odmPonScbTripleChurningKeySet(UINT8 keyIndex,UINT8 *keyBuff)
{  
    INT32 retVal;
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;

    halOnuPonCfg.secStatus = TRUE;
    halOnuPonCfg.secKey = keyBuff;
    halOnuPonCfg.secKeyIndex = keyIndex;
    halOnuPonCfg.secMode = HAL_PON_SECU_MODE_TRIPLE_CHURNING;
    halOnuPonCfg.channelOfSec = HAL_PON_SECU_CHANNEL_SCB;
    stData.apiId = HAL_API_ONU_SCB_SECURITY_KEY_SET;
    stData.param = (void *)&halOnuPonCfg;
    stData.length = sizeof(HAL_ONU_PON_CFG_t);
    retVal = halAppApiCallSync(&stData);
    return retVal;
}

UINT32 odmPonUnicastAesKeyGet(UINT8 keyIndex,UINT8 *keyBuff,UINT8 mode)
{  
    INT32 retVal;
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;

    halOnuPonCfg.secStatus = TRUE;
    halOnuPonCfg.secKey = keyBuff;
    halOnuPonCfg.secKeyIndex = keyIndex;
    halOnuPonCfg.secMode = HAL_PON_SECU_MODE_AES;
    halOnuPonCfg.aesMode = mode;
    halOnuPonCfg.channelOfSec = HAL_PON_SECU_CHANNEL_UNICAST;
    stData.apiId = HAL_API_ONU_UNI_SECURITY_KEY_GET;
    stData.param = (void *)&halOnuPonCfg;
    stData.length = sizeof(HAL_ONU_PON_CFG_t);
    retVal = halAppApiCallSync(&stData);
    return retVal;
}

UINT32 odmPonScbAesKeySet(UINT8 keyIndex,UINT8 *keyBuff,UINT8 mode)
{  
    INT32 retVal;
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;

    halOnuPonCfg.secStatus = TRUE;
    halOnuPonCfg.secKey = keyBuff;
    halOnuPonCfg.secKeyIndex = keyIndex;
    halOnuPonCfg.secMode = HAL_PON_SECU_MODE_AES;
    halOnuPonCfg.aesMode = mode;
    halOnuPonCfg.channelOfSec = HAL_PON_SECU_CHANNEL_SCB;
    stData.apiId = HAL_API_ONU_SCB_SECURITY_KEY_SET;
    stData.param = (void *)&halOnuPonCfg;
    retVal = halAppApiCallSync(&stData);
    stData.length = sizeof(HAL_ONU_PON_CFG_t);
    return retVal;
}

UINT32 odmPonSecurityModeGet(UINT8 *mode)
{  
    INT32 retVal;
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;

    if(NULL == mode)
    {
        return ERROR;
    }

    halOnuPonCfg.secStatus = TRUE;
    stData.apiId = HAL_API_ONU_SECURITY_MODE_GET;
    stData.param = (void *)&halOnuPonCfg;
    retVal = halAppApiCallSync(&stData);
    stData.length = sizeof(HAL_ONU_PON_CFG_t);
    if(OK == retVal)
    {
        *mode = halOnuPonCfg.secMode;
    }
    
    return retVal;
}

UINT32 odmPonSecurityModeSet(UINT8 mode)
{  
    INT32 retVal;
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;

    halOnuPonCfg.secStatus = TRUE;
    stData.apiId = HAL_API_ONU_SECURITY_MODE_SET;
    halOnuPonCfg.secMode = mode;
    stData.param = (void *)&halOnuPonCfg;
    stData.length = sizeof(HAL_ONU_PON_CFG_t);
    retVal = halAppApiCallSync(&stData);
    
    return retVal;
}

UINT32 odmPonLoopbackConfig(UINT8 uniorscb,UINT32 enable)
{
	UINT32  retVal = OK;
	UINT32 olduniEnable;
	UINT32 oldscbEnable;
	OPL_API_DATA_t      stData;
	HAL_ONU_PON_CFG_t   halOnuPonCfg;
	if(TRUE != uniorscb && FALSE != uniorscb)
	{
		return ERR_INVALID_PARAMETERS;
	}
	if(TRUE != enable && FALSE != enable)
	{
		return ERR_INVALID_PARAMETERS;
	}
	if (IS_ONU_RESPIN)
	{
		if(0 == uniorscb)//uni_llid
		{
			retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_LP_UNI);
			olduniEnable = vosConfigUInt32Get(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_LP_UNI,UNI_LLID_LP_DISABLE);
			if((OK == retVal && olduniEnable != enable)||(OK != retVal))
			{
				 halOnuPonCfg.lpChannel = 0;
				 halOnuPonCfg.lpEnable = enable;
			        stData.apiId = HAL_API_ONU_PON_LOOPBACK_CONFIG;
			        stData.param = (void *)&halOnuPonCfg;
			        stData.length = sizeof(HAL_ONU_PON_CFG_t);
			        retVal = halAppApiCallSync(&stData);
			        if(OK != retVal)
			        {
			            return retVal;
			        }  
				vosConfigUInt32Set(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_LP_UNI,enable);	
			}
		}
	     	else if(1 == uniorscb)//scb_llid
	     	{
	     		retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_LP_SCB);
			oldscbEnable = vosConfigUInt32Get(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_LP_SCB,SCB_LLID_LP_DISABLE);
			if((OK == retVal && olduniEnable != enable)||(OK != retVal))
			{
				 halOnuPonCfg.lpChannel = 1;
				 halOnuPonCfg.lpEnable = enable;
			        stData.apiId = HAL_API_ONU_PON_LOOPBACK_CONFIG;
			        stData.param = (void *)&halOnuPonCfg;
			        stData.length = sizeof(HAL_ONU_PON_CFG_t);
			        retVal = halAppApiCallSync(&stData);
			        if(OK != retVal)
			        {
			            return retVal;
			        }  
				vosConfigUInt32Set(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_LP_SCB,enable);	
			}
	     	}
		else
		{
			printf("Input error.\r\n");
		}
	}
	return retVal;
}

UINT32 odmPonFecEnableSet(UINT32 enable)
{
	UINT32  retVal = OK;
	UINT32  oldFecEnable;
	UINT8 	ponSectionBuff[255] = {0};
	INT32   halStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;

	if (IS_ONU_RESPIN)
	{
		if(TRUE== enable)//enable
		{
			/* FEC enable , increase the IPG and enable FEC */
			//oplRegWrite(REG_MPCP_PKT_OVERHEAD, 0x20000F);	
			/* fec deregistered by 64bytes traffic */
			oplRegWrite(REG_MPCP_PKT_OVERHEAD, 0x1b000f);
			oplRegWrite(REG_TM_RPT_DELTA, 0x0004);
			vosUDelay(1000);
			oplRegWrite(REG_TFA_EN, 0x3);
		}
		else if(FALSE== enable)//disable
		{
			/* FEC disable , decrease the IPG and disable FEC */
			oplRegWrite(REG_TFA_EN, 0x0);
			oplRegWrite(REG_MPCP_PKT_OVERHEAD, 0xc000f);
			oplRegWrite(REG_TM_RPT_DELTA, 0x0);
		}
		else 
			return ERR_INVALID_PARAMETERS;
	}
	else
	{
	    if(TRUE != enable && FALSE != enable)
	    {
	        return ERR_INVALID_PARAMETERS;
	    }
	    
		retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_FEC);
		oldFecEnable = vosConfigUInt32Get(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_FEC,FALSE);	

		if((OK == retVal && oldFecEnable != enable)||(OK != retVal))
		{
	        halOnuPonCfg.fecStatus = enable;
	        stData.apiId = HAL_API_ONU_PHY_FEC_MODE_SET;
	        stData.param = (void *)&halOnuPonCfg;
	        stData.length = sizeof(HAL_ONU_PON_CFG_t);
	        retVal = halAppApiCallSync(&stData);

	        if(OK != retVal)
	        {
	            return retVal;
	        }  
			vosConfigUInt32Set(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_FEC,enable);	
		}
	}	
	return OK;
}

UINT32 odmPonFecEnableGet(UINT32 *enable)
{
    *enable = vosConfigUInt32Get(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_FEC,FALSE);		
	return OK;
}

UINT32 odmPonStatusGet(ODM_PON_STATUS_t *odmPonStatus)
{
    DAL_PON_STATUS_t dalPonStatus;
    OPL_STATUS retVal = OK;
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;
    
    stData.apiId = HAL_API_ONU_LINK_STATUS_GET;
    stData.param = (void *)&halOnuPonCfg;
    stData.length = sizeof(HAL_ONU_PON_CFG_t);
    retVal = halAppApiCallSync(&stData);
    
    if(OK != retVal)
    {
        return retVal;
    }

    *(UINT32 *)&dalPonStatus = halOnuPonCfg.mpcpStatus;

    odmPonStatus->bfGatePendingNum = dalPonStatus.bfGatePendingNum;
   	odmPonStatus->bfDiscLEvent = dalPonStatus.bfDiscLEvent;
	odmPonStatus->bfDiscCEvent = dalPonStatus.bfDiscCEvent;
	odmPonStatus->bfDiscLState = dalPonStatus.bfDiscLState;
	odmPonStatus->bfDiscCState = dalPonStatus.bfDiscCState;

    return retVal;
}

UINT32 odmPonOamFinishedSet(UINT32 oamFinished)
{
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;

    halOnuPonCfg.oamStatus = oamFinished;
    stData.apiId = HAL_API_ONU_OAM_FINISHED_SET;
    stData.param = (void *)&halOnuPonCfg;
    stData.length = sizeof(HAL_ONU_PON_CFG_t);
    
    return halAppApiCallSync(&stData);
}

UINT32 odmPonMpcpFsmHoldoverControlSet(UINT32 enableStatus)
{
    UINT32  retVal = OK;
    UINT32  oldStatus;
    OPL_API_DATA_t stData;
	HAL_ONU_MPCP_FSM_HOLD_CTRL_CFG_t halMpcpHoldoverCtrl;
    
    retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_MPCP_HOLDOVER_CTRL);
    oldStatus = vosConfigUInt32Get(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_MPCP_HOLDOVER_CTRL,DISABLE);

    if((OK == retVal && oldStatus != enableStatus)||(OK != retVal))
    {
		halMpcpHoldoverCtrl.enable = enableStatus;
        stData.apiId = HAL_API_ONU_MPCP_HOLDOVER_CTRL_SET;
		stData.param = (void *)&halMpcpHoldoverCtrl;
		stData.length = sizeof(HAL_ONU_MPCP_FSM_HOLD_CTRL_CFG_t);

        retVal = halAppApiCallSync(&stData);  
        retVal = OK;
		
        if(OK != retVal)
        {
            return retVal;
        }
        vosConfigUInt32Set(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_MPCP_HOLDOVER_CTRL, enableStatus);
    }

	return retVal;
}

UINT32 odmPonMpcpFsmHoldoverControlGet(UINT32 *enableStatus)
{
	UINT32 retVal;
	UINT8 	ponSectionBuff[255] = {0};

	retVal = OK;
	
    if(NULL == enableStatus)
    {
        return ERR_NULL_POINTER;
    }
	
	vosSprintf(ponSectionBuff,CFG_PON_MAC_SECTION);
	*enableStatus = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_MPCP_HOLDOVER_CTRL,DISABLE);	

	return retVal;
}

UINT32 odmPonMpcpFsmHoldoverTimeSet(UINT32 holdTime)
{
    UINT32  retVal = OK;
    UINT32  oldHoldtime;
    OPL_API_DATA_t stData;
	HAL_ONU_MPCP_FSM_HOLD_TIME_CFG_t halMpcpHoldoverTime;
    
    retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_MPCP_HOLDOVER_TIME);
    oldHoldtime = vosConfigUInt32Get(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_MPCP_HOLDOVER_TIME, 1);

    if((OK == retVal && oldHoldtime != holdTime)||(OK != retVal))
    {
		halMpcpHoldoverTime.holdtime = holdTime;
        stData.apiId = HAL_API_ONU_MPCP_HOLDOVER_TIME_SET;
		stData.param = (void *)&halMpcpHoldoverTime;
		stData.length = sizeof(HAL_ONU_MPCP_FSM_HOLD_TIME_CFG_t);

        retVal = halAppApiCallSync(&stData);  
        retVal = OK;
		
        if(OK != retVal)
        {
            return retVal;
        }
        vosConfigUInt32Set(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_MPCP_HOLDOVER_TIME, holdTime);
    }

	return retVal;
}

UINT32 odmPonMpcpFsmHoldoverTimeGet(UINT32 *holdTime)
{
	UINT32 retVal;
	UINT8 	ponSectionBuff[255] = {0};

	retVal = OK;
	
    if(NULL == holdTime)
    {
        return ERR_NULL_POINTER;
    }
	
	vosSprintf(ponSectionBuff,CFG_PON_MAC_SECTION);
	*holdTime = vosConfigUInt32Get(CFG_PON_MAC_CFG,ponSectionBuff,CFG_MPCP_HOLDOVER_TIME, 1);	

	return retVal;
}


/*******************************************************************************
* odmPonMacIdInit
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		NO_ERROR									success
* 	SEE ALSO: 
*/
UINT32 odmPonMacIdInit()
{
    UINT32  retVal = OK;
    UINT8   macId[MAC_LENGTH], newMacId[MAC_LENGTH];
    char    macIdString[20] = {0};

	vosHWAddrGet("eth0", &macId[0]);
    vosSprintf(macIdString, "%02x:%02x:%02x:%02x:%02x:%02x",
            macId[0],
            macId[1],
            macId[2],
            macId[3],
            macId[4],
            macId[5]);
	odmPonMacIdGet(&newMacId[0]);
    vosSprintf(macIdString, "%02x:%02x:%02x:%02x:%02x:%02x",
            newMacId[0],
            newMacId[1],
            newMacId[2],
            newMacId[3],
            newMacId[4],
            newMacId[5]);

	if ((0 != memcmp(macId, newMacId, MAC_LENGTH)) && 
		(0 != newMacId[0] ||
		0 != newMacId[1] ||
		0 != newMacId[2] ||
		0 != newMacId[3] ||
		0 != newMacId[4] ||
		0 != newMacId[5]))
	{
		vosMemCpy(&oam_src_mac[0],newMacId,MAC_LENGTH);
	}
	else
	{
        vosSprintf(macIdString, "%02x:%02x:%02x:%02x:%02x:%02x",
            macId[0],
            macId[1],
            macId[2],
            macId[3],
            macId[4],
            macId[5]);
        vosConfigValueSet(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_MACID,macIdString);
        vosMemCpy(&oam_src_mac[0],macId,MAC_LENGTH);

	}
    return OK;
}

/*******************************************************************************
* odmPonCfgInit
*
*   set reg 0xa80 value 0x10337
*   set reg 0x4002 value 0x892d1d2a
*   set reg 0x400a value 0xfffffff
*   set reg 0x4008 value 0x20       
*   set reg 0x4009 value 0x20       
*   set reg 0x4003 value 0x08000c20
*   set reg 0x4004 value 0x103      
*   set reg 0x400e value 0x000f000f
*   set reg 0x400f value 0x807c007e   
*   set reg 0x0a19 value 5        
*   set reg 0x20 value 0xc
*   set reg 0x4006 value 0x44
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		NO_ERROR									success
*		FDB_MAC_SET_AGINGTIME_ERROR				set aging time error
* 		FDB_MAC_SET_CONFIG_FIEL_AGINGTIME_ERROR	set config file error
* 	SEE ALSO: 
*/
UINT32 odmPonCfgInit(void)
{
    UINT32  retVal = OK;

 	UINT32  silenceEn;
	UINT32  silenceTime;
    UINT32  ponLaserOn;
    UINT32  ponLaserOff;
    UINT32  ponSerdesDelay;

    UINT32  cpRptSendMode;
    UINT32  cpDeltaTxNormalFecTime;
    UINT32  cpDeltaTxNormalNoFecTime;
    UINT32  cpTxMpcpFecTime;
    UINT32  cpTxMpcpNoFecTime;

    UINT32  cpRttCompenEn;
    UINT32  cpRttCompenSyncEn;
    UINT32  cpRttCorrectEn;
    UINT32  cpRttCorrectSyncEn;
	UINT32  cpTxDlyEn;
    UINT32  cpSyncEn;
    UINT32  cpRptsentDelt;
    UINT32  cpRttOffset;
    UINT32  cpTxDly;
    UINT32  regMode;
	INT32   halStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_ONU_PON_CFG_t   halOnuPonCfg;
	PON_MPCP_DISC_STATUS_t mpcpStatus;
    UINT32 holdOverStatus;
	HAL_ONU_MPCP_FSM_HOLD_CTRL_CFG_t halMpcpHoldoverCtrl;
    UINT32 holdOverTime;
	HAL_ONU_MPCP_FSM_HOLD_TIME_CFG_t halMpcpHoldoverTime;

    HAL_ONU_MTU_CFG_t onuMtuCfg;
 
    UINT8   ponMacId[MAC_LENGTH];

	odmPonMacIdGet(&ponMacId[0]);

    vosConfigRefreshByModule(CFG_PON_LOID_CFG);

	if (0 != ponMacId[0] ||
		0 != ponMacId[1] ||
		0 != ponMacId[2] ||
		0 != ponMacId[3] ||
		0 != ponMacId[4] ||
		0 != ponMacId[5])
	{
        //printf("odmPonCfgInit: Using configured PON MAC !!!\r\n");
		stData.apiId = HAL_API_ONU_MACID_SET;
	    stData.length = sizeof(HAL_ONU_PON_CFG_t);
	    stData.param = (void *)&halOnuPonCfg;
	    vosMemCpy(halOnuPonCfg.ponMac,ponMacId,6);
	    halStatus = halAppApiCallSync(&stData);  
		if(OK != halStatus)
		{
			retVal = halStatus;
		}
	}

    if(OK == odmPonSilenceEnableGet(&silenceEn))
    {
        g_onuSilenceEn = silenceEn;
    } 

    if(OK == odmPonSilenceTimeGet(&silenceTime))
    {
        g_onuSilenceTime = silenceTime;
    } 
    if(OK == odmPonLaserOnGet(&ponLaserOn))
    {
        stData.apiId = HAL_API_ONU_LASER_ONCNT_SET;
        stData.length = sizeof(HAL_ONU_PON_CFG_t);
        stData.param = (void *)&halOnuPonCfg;
        halOnuPonCfg.laserOnTime = ponLaserOn;
        halAppApiCallSync(&stData);  
    } 

    if(OK == odmPonLaserOffGet(&ponLaserOff))
    {
	    stData.apiId = HAL_API_ONU_LASER_OFFCNT_SET;
        stData.length = sizeof(HAL_ONU_PON_CFG_t);
        stData.param = (void *)&halOnuPonCfg;
        halOnuPonCfg.laserOffTime = ponLaserOff;
        halAppApiCallSync(&stData);  
    }
    if(OK == odmPonSerdesDelayGet(&ponSerdesDelay))
    {
        stData.apiId = HAL_API_ONU_SERDES_DELAYCNT_SET;
        stData.length = sizeof(HAL_ONU_PON_CFG_t);
        stData.param = (void *)&halOnuPonCfg;
        halOnuPonCfg.serdesDelay = ponSerdesDelay;
        halAppApiCallSync(&stData); 
    }

    if (OK == odmPonMpcpFsmHoldoverTimeGet(&holdOverTime))
    {
        halMpcpHoldoverTime.holdtime = holdOverTime;
        stData.apiId = HAL_API_ONU_MPCP_HOLDOVER_TIME_SET;
		stData.param = (void *)&halMpcpHoldoverTime;
		stData.length = sizeof(HAL_ONU_MPCP_FSM_HOLD_TIME_CFG_t);
        halAppApiCallSync(&stData);  
    }

    if (OK == odmPonMpcpFsmHoldoverControlGet(&holdOverStatus))
    {
        halMpcpHoldoverCtrl.enable = holdOverStatus;
        stData.apiId = HAL_API_ONU_MPCP_HOLDOVER_CTRL_SET;
		stData.param = (void *)&halMpcpHoldoverCtrl;
		stData.length = sizeof(HAL_ONU_MPCP_FSM_HOLD_CTRL_CFG_t);
        halAppApiCallSync(&stData);  
    }

	if (OK == odmOnuMtuGet(&onuMtuCfg.mtuSize))		
	{
		stData.apiId = HAL_API_ONU_MTU_SET;
		stData.param = (void *)&onuMtuCfg;
		stData.length = sizeof(HAL_ONU_MTU_CFG_t);
		retVal = halAppApiCallSync(&stData);
	    if(OK != retVal)
	    {
	        printf("set pon MTU failed.\n");
	        goto exit_label;
	    }
	}
    retVal = odmPonRegSet(0x20,0x7);
    if(OK != retVal)
    {
        printf("set 0x20 failed.\n");
        goto exit_label;
    }

	/* laser on time */
    retVal = odmPonRegSet(0x4008,0x1C);
    if(OK != retVal)
    {
        printf("set 0x4008 failed.\n");
        goto exit_label;
    }

	/* laser off time */
#if 0  /* modified by Gan Zhiheng - 2010/10/30 */
    retVal = odmPonRegSet(0x4009,0x20);
#else
    retVal = odmPonRegSet(0x4009,0x1C);
#endif /* #if 0 */
    if(OK != retVal)
    {
        printf("set 0x4009 failed.\n");
        goto exit_label;
    }
    
    retVal = odmPonRegSet(0xa19,0x05);
    if(OK != retVal)
    {
        printf("set 0xa19 failed.\n");
        goto exit_label;
    }

    retVal = odmPonRegSet(0xa80,0x10337);
    if(OK != retVal)
    {
        printf("set 0xa80 failed.\n");
        goto exit_label;
    }
    
    retVal = odmPonRegSet(0x904,0x8);
    if(OK != retVal)
    {
        printf("set 0x904 failed.\n");
        goto exit_label;
    }

	/* mpcp control register */
    retVal = odmPonRegSet(0x4003,0x3020);
    if(OK != retVal)
    {
        printf("set 0x4003 failed.\n");
        goto exit_label;
    }

	/* dba agent configure */
    retVal = odmPonRegSet(0x4002,0x40a802a);
    if(OK != retVal)
    {
        printf("set 0x4002 failed.\n");
        goto exit_label;
    }

    retVal = odmPonRegSet(0x4004,0x103);
    if(OK != retVal)
    {
        printf("set 0x4004 failed.\n");
        goto exit_label;
    }

#if 0  /* deleted by Gan Zhiheng - 2010/10/30 */
    /* set report delta register */
    retVal = odmPonRegSet(0x20C9,0x4);
    if(OK != retVal)
    {
        printf("set 0x20C9 failed.\n");
        goto exit_label;
    }
#endif /* #if 0 */

    retVal = odmPonRegSet(0x400e,0xc000f);
    if(OK != retVal)
    {
        printf("set 0x400e failed.\n");
        goto exit_label;
    }
    
    retVal = odmPonRegSet(0x400f,0x80050005);
     if(OK != retVal)
    {
        printf("set 0x400f failed.\n");
        goto exit_label;
    }

	/* set mpcp grant timeout */
    retVal = odmPonRegSet(0x401F,0x03B9ACA0);
    if(OK != retVal)
    {
        printf("set 0x401f failed.\n");
        goto exit_label;
    }
    retVal = odmPonRegSet(0x3023,0x847FF87);
    if(OK != retVal)
    {
        printf("set 0x3023 failed.\n");
        goto exit_label;
    }

    /* Enable PMAC */
    retVal = odmPonRegSet(0xA00, 0x3);
    if(OK != retVal)
    {
        printf("set 0xA00 failed.\n");
        goto exit_label;
    }
    /* Enable GMAC */
    retVal = odmPonRegSet(0x900,0x3);
    if(OK != retVal)
    {
        printf("set 0x900 failed.\n");
        goto exit_label;
    }

	if (!IS_ONU_RESPIN)
	{
		/* fix rtt inaccuracy*/
	    retVal = odmPonRegSet(0x4026,0x20792079);
	    if(OK != retVal)
	    {
	        printf("set 0x900 failed.\n");
	        goto exit_label;
	    }
	}
    
    /*begin added by liaohongjun 2012/7/18*/
    /*set gpio 11 as output*/
    if(DRV_OK != Ioctl_SetGpioDirOut(DRV_GPIO_11_VCCT))
    {
        printf("set GPIO_11 as output failed.\n");
    }    
    /*pull hight of gpio 11*/
    if(DRV_OK != Ioctl_SetGpioVal(DRV_GPIO_11_VCCT, 1))
    {
        printf("set GPIO_11 hight failed.\n");
    }    
    /*end added by liaohongjun 2012/7/18*/
#if 0
    if (g_onuSilenceEn)
    {
    	/* manual generate register request */
        regMode = 0x4A;
    }
    else
    {
    	/* hardware auto generate register request */
        regMode = 0x44;
    }

    retVal = odmPonRegSet(0x4006,regMode);
    if(OK != retVal)
    {
        printf("set 0x4006 failed.\n");
        goto exit_label;
    }
#endif
#if 0
    OdmPonBerCollectStart();
#endif

exit_label:
    return retVal;
}

UINT8 ponResetBufferEnable = 1;

void ponBufferResetConfigSet(UINT8 enable)
{
	ponResetBufferEnable = enable;
}

void ponBufferResetConfigGet(UINT8* enable)
{
	if (enable){
		*enable = ponResetBufferEnable;
	}
}

UINT32 ponBufferCellNumRead()
{
	int i;
	UINT32 numOfCell;
    UINT32 sum = 0;

	for (i = 0; i < 8; i++){
		numOfCell = 0;
		tmCellNumInQueueHwRead(UP_STREAM, i, &numOfCell);
		sum += numOfCell;
	}

	return sum;
}

void ponBufferReset()
{
    int i;
    int MAX_LOOP_CNT = 10000;
	UINT32 gmacCfg = 0;

	if (!ponResetBufferEnable){
		return;
	}

	if (0 == ponBufferCellNumRead()){
		return;
	}

    /* Disable GMAC */ 
	oplRegRead(REG_GMAC_CFG_TRANSFER_ENA, &gmacCfg);
    oplRegWrite(REG_GMAC_CFG_TRANSFER_ENA, 0); 
    /* Disable CPDMA */ 
    oplRegFieldWrite(REG_DMA0_EN, 1, 1, 0); 

    /* Disable Laser */ 
    oplRegFieldWrite(REG_PMAC_LASER_ON_CFG, 0, 4, 0x8); 



    /* Enable Bypass Mode for clear buffer */ 
    //ponBypassModeEnHwWrite(1);
    oplRegFieldWrite(REG_GB_PONTX_MODE, 0, 1, 1);
    /* Wait for upstream queue empty */
    for (i = 0; i < MAX_LOOP_CNT && ponBufferCellNumRead(); i++);
    if (i == MAX_LOOP_CNT)
    {
        printf("ponBufferReset: buffer is not empty\n");
    }
    /* Disable bypass mode */ 
    oplRegFieldWrite(REG_GB_PONTX_MODE, 0, 1, 0);

    /* Enable Laser */ 
    oplRegFieldWrite(REG_PMAC_LASER_ON_CFG, 0, 4, 0x5);
	
    /* Enable CPDMA */ 
    oplRegFieldWrite(REG_DMA0_EN, 1, 1, 1); 
	oplRegWrite(REG_GMAC_CFG_TRANSFER_ENA, gmacCfg);
}

void odmMpcpCtrlThread(void)
{
	UINT32  regMode;
	PON_MPCP_DISC_STATUS_t mpcpStatus;
    VOS_TIME_USECS_t startSilence = 0;
    VOS_TIME_USECS_t current = 0;
    int setFlag = 0;
    int registeredFlag = 0;
    UINT32  syncTime = 32;
    static int onlined = 0;

	extern UINT8 *mpcpEventDisc[] ;
	extern UINT8 *mpcpStateDisc[] ; 
	UINT8 oam_status;
	OP_DEBUG(DEBUG_LEVEL_INFO, "Reset PON MAC Buffer");
    ponBufferReset();
	/*odmClsFilterAdd(); *//*add for not to receive downstream packets when onu not registered*/

	if (g_onuSilenceEn){
    	/* manual generate register request */
        regMode = 0x4A;
    }
    else{
    	/* hardware auto generate register request */
        regMode = 0x44;
    }

    if(OK != odmPonRegSet(0x4006,regMode))
    {
        printf("set 0x4006 failed.\n");
    }

	
	while(1)
	{
        //if (g_onuSilenceEn)
        {

    		oplRegRead(REG_MPCP_DISC_STATUS, (UINT32 *)&mpcpStatus);
			OamStatusGet(&oam_status);

    		if (mpcpStatus.bfDiscCState == STATE_WAIT)
    		{
    			/* mpcp drop event */
    			if (1 == registeredFlag) {

					if (IS_ONU_RESPIN)
					{
	    				/* fec disable */
	    				oplRegWrite(REG_TFA_EN, 0x0);
	    				/* IPG back to normal */
	    				oplRegWrite(REG_MPCP_PKT_OVERHEAD, 0xc000f); 
					}

    				/* inform */
    				OP_DEBUG(DEBUG_LEVEL_CRITICAL, "MPCP off line");
    					
    				if (oam_status == EX_DISCOVERY_SEND_ANY)
    					OP_DEBUG(OFFLINE_PRINT_LEVEL, " - OAM Reged");
    				else
    					OP_DEBUG(OFFLINE_PRINT_LEVEL, "-OAM UnReged");
    					
    				if(mpcpStatus.bfDiscLEvent <= EVENT_WATCHDOG_TIMEOUT)
    				{
    					OP_DEBUG(OFFLINE_PRINT_LEVEL," - %s %s","le:",mpcpEventDisc[mpcpStatus.bfDiscLEvent]);
    				}
    					
    				if(mpcpStatus.bfDiscCEvent <= EVENT_WATCHDOG_TIMEOUT)
    				{
    					OP_DEBUG(OFFLINE_PRINT_LEVEL," - %s %s","ce:",mpcpEventDisc[mpcpStatus.bfDiscCEvent]);
    				}
    					
    				if(mpcpStatus.bfDiscLState <= STATE_WATCHDOG_TIMEOUT)
    				{
    					OP_DEBUG(OFFLINE_PRINT_LEVEL," - %s %s","ls:",mpcpStateDisc[mpcpStatus.bfDiscLState]);
    				}
    					
    				if(mpcpStatus.bfDiscCState <= STATE_WATCHDOG_TIMEOUT)
    				{
    					OP_DEBUG(OFFLINE_PRINT_LEVEL," - %s %s","cs:",mpcpStateDisc[mpcpStatus.bfDiscCState]);
    				}
    			}

    			if (g_onuSilenceEn && 
                    mpcpStatus.bfDiscLState == STATE_DENIED && 
                    setFlag == 0)
    			{
        			OP_DEBUG(DEBUG_LEVEL_INFO, "MPCP in STATE_DENIED to STATE_WAIT");
    				OP_DEBUG(DEBUG_LEVEL_INFO, "Silence %d seconds", g_onuSilenceTime);
    				startSilence = vosTimeUsecsGet();
                    //OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Start at %llu", start);
                    OamInit();
                    setFlag = 1;
    			}
                if (mpcpStatus.bfDiscLState == STATE_REMOTE_DEREGISTER && setFlag == 0)
                {
            		OP_DEBUG(DEBUG_LEVEL_INFO, "MPCP in STATE_REMOTE_DEREGISTER to STATE_WAIT");
                    if (g_onuSilenceEn && 
                        g_iOamAuthCode == CTC_OAM_AUTH_FAILURE)
        			{
        				OP_DEBUG(DEBUG_LEVEL_INFO, "Silence %d seconds", g_onuSilenceTime);
        				startSilence = vosTimeUsecsGet();
                        OamInit();
                        //OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Start at %llu", start);
                    }
                    /*
    				OP_DEBUG(DEBUG_LEVEL_INFO, "Reset PON MAC Buffer");
                    ponBufferReset();
                    */
                    setFlag = 1;
    			}

                current = vosTimeUsecsGet();
    			/* generate mpcp register request */
    			if ((UINT64)(current - startSilence) >= (UINT64)(g_onuSilenceTime*SECOND_USEC))
    			{
					//OP_DEBUG(DEBUG_LEVEL_INFO, "Reset PON MAC Buffer");
                    ponBufferReset();
                    OP_DEBUG(DEBUG_LEVEL_INFO, "Generate MPCP register request");
                    //OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Current at %llu", current);
    			    oplRegWrite(REG_MPCP_DISC_CTRL,0x4A);
                    setFlag = 0;
    			}
    		}
            if (0 == registeredFlag)
            {
                if (mpcpStatus.bfDiscCState == STATE_REGISTERD &&
                    mpcpStatus.bfDiscLState == STATE_REGISTER_ACK)
                {
                    if (oam_status != EX_DISCOVERY_SEND_ANY)
                    {
                        /****Fix bug 3571 To IOP with some OLT,after the MPCP register disable the GE port till the OAM is registed */
    				    //oplRegWrite(REG_GMAC_CFG_TRANSFER_ENA,0x2);
    				    OP_DEBUG(DEBUG_LEVEL_INFO, "1brg drop packets after classify module.");
						//brgGePortLinkControlSet(GE_PORT_TRSMT_EN, 0);
						oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 0, 1, 0);
						oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 0, 1, 0);
                        /*******************************Fix bug 3571 end***************************** */
                    }
                    registeredFlag = 1;
                    g_iOamAuthCode = 0;
					/*odmClsFilterDel();*/
                }
            }
            else
            {
                if (mpcpStatus.bfDiscCState != STATE_REGISTERD)
                {
                    registeredFlag = 0;
				    /* drop packets when Classify module finishes processing the packet.*/
					OP_DEBUG(DEBUG_LEVEL_INFO, "2brg drop packets after classify module.");
					oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 0, 1, 0);
					oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 0, 1, 0);
                }
            }
        }
		vosUSleep(50000);
	}
}

/*
    state - TRUE: power down, FLASE: power up
*/
UINT32 odmPonTxPowerDown(UINT8 state)
{
    UINT32 retVal;
	if(TRUE == state)
	{
#if defined(ONU_1PORT_V1E_RF100_GNA) || defined(ONU_4PORT_V1E_RF400_GNA)
	    HAL_API_GBL_REG_FIELD_SET(REG_GPIO_DIRECTION/4, GPIO_TX_POWER_CTRL, 1, 1, retVal);
		HAL_API_GBL_REG_FIELD_SET(REG_GPIO_CLEAR/4, GPIO_TX_POWER_CTRL, 1, 1, retVal);
	    HAL_API_GBL_REG_FIELD_SET(REG_GPIO_SET/4, GPIO_TX_POWER_CTRL, 1, 0, retVal);
#endif
	}
	else {
#if defined(ONU_1PORT_V1E_RF100_GNA) || defined(ONU_4PORT_V1E_RF400_GNA)
	    HAL_API_GBL_REG_FIELD_SET(REG_GPIO_DIRECTION/4, GPIO_TX_POWER_CTRL, 1, 1, retVal);
	    HAL_API_GBL_REG_FIELD_SET(REG_GPIO_CLEAR/4, GPIO_TX_POWER_CTRL, 1, 1, retVal);
		HAL_API_GBL_REG_FIELD_SET(REG_GPIO_SET/4, GPIO_TX_POWER_CTRL, 1, 1, retVal);
#endif
        if (!g_onuSilenceEn)
        {
    		odmPonRegSet(0x4006,0x44);
        }

	}
    return retVal;
}

UINT32 odmPonTxPowerDownAndDuration(UINT16 duration)
{
    UINT32 retVal;
	TIMER_OBJ_t * pstTimer = NULL;

	/* Tx power down */
#if defined(ONU_1PORT_V1E_RF100_GNA) || defined(ONU_4PORT_V1E_RF400_GNA)
    HAL_API_GBL_REG_FIELD_SET(REG_GPIO_DIRECTION/4, GPIO_TX_POWER_CTRL, 1, 1, retVal);
	HAL_API_GBL_REG_FIELD_SET(REG_GPIO_CLEAR/4, GPIO_TX_POWER_CTRL, 1, 1, retVal);
    HAL_API_GBL_REG_FIELD_SET(REG_GPIO_SET/4, GPIO_TX_POWER_CTRL, 1, 1, retVal);

	/* start a timer and wait the duration then turn on tx power */
	pstTimer = vosTimerGetFree();
    if (pstTimer)
    {
        vosTimerStart(pstTimer, ONE_SHOT_TIMER, duration*1000, (FUNCPTR)odmPonTxPowerDown,FALSE,2,3,4,5,6,7,8);
    }
#endif

    return NO_ERROR;
}


#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
UINT32 odmPonUsShapingSet(UINT8 enable,UINT32 cir,UINT32 cbs)
{
	UINT32  retVal, retVal2, retVal3;
	UINT32  oldEnable, oldCir, oldCbs;

	retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG, 
        CFG_PON_MAC_SECTION, CFG_PON_US_SHAPING_EN);
	oldEnable = (UINT8)vosConfigUInt32Get(CFG_PON_MAC_CFG,
        CFG_PON_MAC_SECTION, CFG_PON_US_SHAPING_EN, FALSE);

	retVal2 = vosConfigKeyIsExisted(CFG_PON_MAC_CFG, 
        CFG_PON_MAC_SECTION, CFG_PON_US_SHAPING_CIR);
	oldCir = vosConfigUInt32Get(CFG_PON_MAC_CFG,
        CFG_PON_MAC_SECTION, CFG_PON_US_SHAPING_CIR, ODM_US_CIR);

    retVal3 = vosConfigKeyIsExisted(CFG_PON_MAC_CFG, 
        CFG_PON_MAC_SECTION, CFG_PON_US_SHAPING_CBS);
    oldCbs = vosConfigUInt32Get(CFG_PON_MAC_CFG, 
        CFG_PON_MAC_SECTION, CFG_PON_US_SHAPING_CBS, ODM_US_CBS);

	if (((OK == retVal && oldEnable != enable)||(OK != retVal)) ||
        ((OK == retVal2 && oldCir != cir)||(OK != retVal2)) ||
        ((OK == retVal3 && oldCbs != cbs)||(OK != retVal3)))
	{
		vosConfigUInt32Set(CFG_PON_MAC_CFG, CFG_PON_MAC_SECTION, 
            CFG_PON_US_SHAPING_EN, enable);

		vosConfigUInt32Set(CFG_PON_MAC_CFG, CFG_PON_MAC_SECTION, 
            CFG_PON_US_SHAPING_CIR, cir);

        vosConfigUInt32Set(CFG_PON_MAC_CFG, CFG_PON_MAC_SECTION, 
            CFG_PON_US_SHAPING_CBS, cbs);

        tmGeCirHwWrite(UP_STREAM, cir);
        tmGeCbsHwWrite(UP_STREAM, cbs);
        if (enable)
        {
            tmGeShaperEnableHwWrite(UP_STREAM, TRUE);
        }
        else {
            tmGeShaperEnableHwWrite(UP_STREAM, FALSE);
        }
	}

    return OK;
}

UINT32 odmPonUsShapingGet(UINT8 *enable,UINT32 *cir,UINT32 *cbs)
{
    if(NULL == enable || NULL == cir || NULL == cbs)
    {
        return ERR_NULL_POINTER;
    }

	*enable = (UINT8)vosConfigUInt32Get(CFG_PON_MAC_CFG, CFG_PON_MAC_SECTION,
        CFG_PON_US_SHAPING_EN, FALSE);

	*cir = vosConfigUInt32Get(CFG_PON_MAC_CFG, CFG_PON_MAC_SECTION,
        CFG_PON_US_SHAPING_CIR, ODM_US_CIR);

	*cbs = vosConfigUInt32Get(CFG_PON_MAC_CFG, CFG_PON_MAC_SECTION,
        CFG_PON_US_SHAPING_CBS, ODM_US_CBS);

    return OK;
}


UINT32 odmPonDsShapingSet(UINT8 enable, UINT32 cir,UINT32 cbs)
{
	UINT32  retVal, retVal2, retVal3;
	UINT32  oldEnable, oldCir, oldCbs;

	retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG, 
        CFG_PON_MAC_SECTION, CFG_PON_DS_SHAPING_EN);
	oldEnable = (UINT8)vosConfigUInt32Get(CFG_PON_MAC_CFG,
        CFG_PON_MAC_SECTION, CFG_PON_DS_SHAPING_EN, FALSE);

	retVal2 = vosConfigKeyIsExisted(CFG_PON_MAC_CFG, 
        CFG_PON_MAC_SECTION, CFG_PON_DS_SHAPING_CIR);
	oldCir = vosConfigUInt32Get(CFG_PON_MAC_CFG,
        CFG_PON_MAC_SECTION, CFG_PON_DS_SHAPING_CIR, ODM_US_CIR);
    retVal3 = vosConfigKeyIsExisted(CFG_PON_MAC_CFG, 
        CFG_PON_MAC_SECTION, CFG_PON_DS_SHAPING_CBS);
    oldCbs = vosConfigUInt32Get(CFG_PON_MAC_CFG, 
        CFG_PON_MAC_SECTION, CFG_PON_DS_SHAPING_CBS, ODM_US_CBS);

	if (((OK == retVal && oldEnable != enable)||(OK != retVal)) ||
        ((OK == retVal2 && oldCir != cir)||(OK != retVal2)) ||
        ((OK == retVal3 && oldCbs != cbs)||(OK != retVal3)))
	{
		vosConfigUInt32Set(CFG_PON_MAC_CFG, CFG_PON_MAC_SECTION, 
            CFG_PON_DS_SHAPING_EN, enable);

		vosConfigUInt32Set(CFG_PON_MAC_CFG, CFG_PON_MAC_SECTION, 
            CFG_PON_DS_SHAPING_CIR, cir);

        vosConfigUInt32Set(CFG_PON_MAC_CFG, CFG_PON_MAC_SECTION, 
            CFG_PON_DS_SHAPING_CBS, cbs);

        tmGeCirHwWrite(DOWN_STREAM, cir);
        tmGeCbsHwWrite(DOWN_STREAM, cbs);
        if (enable)
        {
            tmGeShaperEnableHwWrite(DOWN_STREAM, TRUE);
        }
        else {
            tmGeShaperEnableHwWrite(DOWN_STREAM, FALSE);
        }
	}

    return OK;
}

UINT32 odmPonDsShapingGet(UINT8 *enable,UINT32 *cir,UINT32 *cbs)
{
    if(NULL == enable || NULL == cir || NULL == cbs)
    {
        return ERR_NULL_POINTER;
    }

	*enable = (UINT8)vosConfigUInt32Get(CFG_PON_MAC_CFG, CFG_PON_MAC_SECTION,
        CFG_PON_DS_SHAPING_EN, FALSE);

	*cir = vosConfigUInt32Get(CFG_PON_MAC_CFG, CFG_PON_MAC_SECTION,
        CFG_PON_DS_SHAPING_CIR, ODM_DS_CIR);

	*cbs = vosConfigUInt32Get(CFG_PON_MAC_CFG, CFG_PON_MAC_SECTION,
        CFG_PON_DS_SHAPING_CBS, ODM_DS_CBS);

    return OK;
}
#endif

/* data path */
UINT32 odmOnuMtuSet(UINT32 mtuSize)
{
	UINT32 retVal = OK;
	UINT32 oldMtuSize;
	UINT8 	ponSectionBuff[255] = {0};
    HAL_ONU_MTU_CFG_t onuMtuCfg;
    OPL_API_DATA_t stData;


	if(PON_MAC_MTU < mtuSize)
	{
		return ERR_INVALID_PARAMETERS;
	}
	retVal = vosConfigKeyIsExisted(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_MTU_SIZE);

	onuMtuCfg.mtuSize = mtuSize;

	oldMtuSize = vosConfigUInt32Get(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_MTU_SIZE,PON_MAC_MTU);
	if((OK == retVal && oldMtuSize != mtuSize)||(OK != retVal))
	{
    	stData.apiId = HAL_API_ONU_MTU_SET;
    	stData.param = &onuMtuCfg;
    	stData.length = sizeof(HAL_ONU_MTU_CFG_t);
    	retVal = halAppApiCallSync(&stData);
    	if(OK != retVal)
    	{
    		return retVal;
    	}
    	vosConfigUInt32Set(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_MTU_SIZE,mtuSize);	
	}
	return OK;

}

/* data path */
UINT32 odmOnuMtuGet(UINT32 *mtuSize)
{
	UINT8 	ponSectionBuff[255] = {0};


	if(NULL == mtuSize)
	{
		return ERR_NULL_POINTER;
	}

	*mtuSize = vosConfigUInt32Get(CFG_PON_MAC_CFG,CFG_PON_MAC_SECTION,CFG_PON_MTU_SIZE,PON_MAC_MTU);

    return OK;
}

UINT32 odmOnuLuminateSet(ODM_LIGHT_MODE_E enMode)
{
    UINT32 ulRegVal = 0;

    /* TRUE £º light always 
       FALSE:  ligth normal
      register bits:
      01xx = Laser On/Off is controlled by MPCP module, affected by grant 
      00x1= Laser On/Off affected only by upstream data
      0010= Laser On
      0000= Laser On for 3 seconds, and laser will be off  automatically after 3 seconds. Then Laser On/Off is controlled by MPCP module, affected  by upstream data
      1xxx= Laser Off*/ 
    switch ( enMode )
    {
        case ODM_LIGHT_OFF :
            ulRegVal = 0x8;
            break;
        case ODM_LIGHT_ON :
            ulRegVal = 0x2;
            break;
        case ODM_LIGHT_NORMAL :
            ulRegVal = 0x4;
            break;
        case ODM_LIGHT_ON_A_WHILE :
            ulRegVal = 0;
            break;
        default:
            return ERR_INVALID_PARAMETERS;
    }
    oplRegFieldWrite(REG_PMAC_LASER_ON_CFG, 0, 4, ulRegVal);
    
    return OK;
}

UINT32 odmOnuLuminateGet(UINT32 *pulEnable)
{
    UINT32 ulRegData = 0;

    if(NULL == pulEnable)
	{
		return ERR_INVALID_PARAMETERS;
	}

    oplRegFieldRead(REG_PMAC_LASER_ON_CFG, 0, 4, &ulRegData);

    #if 0
    if (!(ulRegData & OPCONN_BIT(3)) &&
        !(ulRegData & OPCONN_BIT(2)) &&
        (ulRegData & OPCONN_BIT(1)) &&
        !(ulRegData & OPCONN_BIT(0)))
    {
        *pulEnable = TRUE;
    }
    else
    {
        *pulEnable = FALSE;
    }
    #else
    *pulEnable = ulRegData;
    #endif
    
    return OK;
}

