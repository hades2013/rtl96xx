/*
=============================================================================
     File Name: odm_port.c

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

#include "hal.h"
#include "hal_fdb.h"
#include "odm_port.h"
#include "log.h"

#define MODULE MOD_PORT
#define TRANSPARENTMODE 0
#define TAGMODE 1
#define ETH0 0
#define ADMIN 0 
VOS_MUTEX_t g_pstPortApiLock = PTHREAD_MUTEX_INITIALIZER;
#define PORT_API_LOCK  vosMutexTake(&g_pstPortApiLock) 
#define PORT_API_UNLOCK  vosMutexGive(&g_pstPortApiLock)

typedef struct VoiceVlan_s
{
	UINT16 VoiceVlanId;
	UINT8  VoiceVlanCos;
	UINT8 VoipVlanDSCP;
	UINT8 VoipTagValid;
	UINT8 VoipPortNum;
	UINT16 usVlanTpid;	  /* CVLAN Tpid */
	UINT8 VoipQinQEn;	 /* 0 - disable QinQ, 1 - enable QinQ */
	UINT8  VoiceSVlanCos; /* SVLAN COS */
	UINT16 VoiceSVlanId; /* SVLAN */
	UINT16 usSVlanTpid;   /* SVLAN Tpid */
}VoiceVlan_t;
#define NR_START 4000 //_MIPS_SIM_ABI32:4000,316; _MIPS_SIM_ABI64:5000,275; _MIPS_SIM_NABI32:6000,279
#define NR_CFG_VOICEVLAN (NR_START + 320)

#if defined(ONU_1PORT)
#define NUMBER_OF_PORTS  1
#define PORT_RATE_LIMIT_MAX 1000000
#elif defined(ONU_4PORT_AR8327)
#define NUMBER_OF_PORTS	 4
#define PORT_RATE_LIMIT_MAX 1000000
#else
#define NUMBER_OF_PORTS	 4
#define PORT_RATE_LIMIT_MAX 100000
#endif
extern unsigned int bOnePortLoopDetectEn[NUMBER_OF_PORTS];
extern unsigned int bOnePortLoopWhile[NUMBER_OF_PORTS];
extern unsigned int bOnePortLooptimer[NUMBER_OF_PORTS];


UINT32 odmPortRangeCheck(UINT32 portId)
{
	if(ODM_START_PORT_NUN > portId || ODM_NUM_OF_PORTS < portId)
	{
		return PORT_INVALID;
	}else
	{
		return OK;
	}
}

UINT32 odmVlanRangeCheck(UINT32 vlanId)
{
#if defined(ONU_1PORT)
	if(ODM_DEF_VLAN_MAX < vlanId || ODM_DEF_VLAN_NUM_START > vlanId)
#else
	if(ODM_MAX_VLAN_NUM < vlanId || ODM_CONFIG_VLAN_NUM_START > vlanId)
#endif
	{
		return ERR_INVALID_PARAMETERS;
	}else
	{
		return OK;
	}	
}


UINT32 odmPortLinkStateGet(UINT32 portId,UINT32 *linkStatus)
{
	UINT32 retVal = OK;
	INT32   dalStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_PORT_CFG_INFO_t halPortCfgInfo;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

    if(NULL == linkStatus)
	{
		return ERR_NULL_POINTER;
	}

    PORT_API_LOCK;

	stData.apiId = HAL_API_PORT_STATE_GET;
    stData.length = sizeof(HAL_PORT_CFG_INFO_t);
    halPortCfgInfo.portId = portId;
    stData.param = (void *)&halPortCfgInfo;
    dalStatus = halAppApiCallSync(&stData);

	if(OK != dalStatus)
	{
        PORT_API_UNLOCK;
		return dalStatus;
	}

	*linkStatus = halPortCfgInfo.linkStatus;

    PORT_API_UNLOCK;
	return OK;

}


UINT32 odmPhyAdminStateGet(UINT32 portId,UINT32 *phyStatus)
{
	UINT32 retVal = OK;
	INT32   dalStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_PORT_CFG_INFO_t halPortCfgInfo;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

    if(NULL == phyStatus)
	{
		return ERR_NULL_POINTER;
	}

    PORT_API_LOCK;

	stData.apiId = HAL_API_PORT_ADMIN_GET;
    stData.length = sizeof(HAL_PORT_CFG_INFO_t);
    halPortCfgInfo.portId = portId;
    stData.param = (void *)&halPortCfgInfo;
    dalStatus = halAppApiCallSync(&stData);

	if(OK != dalStatus)
	{
        PORT_API_UNLOCK;
		return dalStatus;
	}

	*phyStatus = halPortCfgInfo.admin;

    PORT_API_UNLOCK;
	return OK;

}


UINT32 odmPhyAdminStateSet(UINT32 portId,UINT32 phyStatus)
{
	UINT32 retVal = OK;
	INT32   dalStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_PORT_CFG_INFO_t halPortCfgInfo;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

    PORT_API_LOCK;

	stData.apiId = HAL_API_PORT_ADMIN_SET;
    stData.length = sizeof(HAL_PORT_CFG_INFO_t);
    halPortCfgInfo.portId = portId;
    halPortCfgInfo.admin = phyStatus;
    stData.param = (void *)&halPortCfgInfo;
    dalStatus = halAppApiCallSync(&stData);

	if(OK != dalStatus)
	{
        PORT_API_UNLOCK;
		return dalStatus;
	}

    PORT_API_UNLOCK;
	return OK;

}


/*******************************************************************************
* odmPortAdminSet
*
* DESCRIPTION:
* this function is used to set admin status of the specified port.
*	 	portId: 			1 .....
*		adminstatus:    enable or disable
* INPUTS:
*
* OUTPUTS:
*		no
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
UINT32 odmPortAdminSet(UINT32 portId,UINT32 adminStatus)
{
	UINT32 retVal = OK;
	UINT32 oldAdminStatus, linkStatus;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_PORT_CFG_INFO_t halPortCfgInfo;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\r\[ %s ]: portId = %d, adminStatus = %d\r\n", __FUNCTION__, portId, adminStatus);

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	if(TRUE != adminStatus && FALSE != adminStatus)
	{
		return ERR_INVALID_PARAMETERS;
	}

    PORT_API_LOCK;

    odmPortAdminGet(portId, &oldAdminStatus);
	if (oldAdminStatus == adminStatus)
	{
    	stData.apiId = HAL_API_PORT_STATE_GET;
        stData.length = sizeof(HAL_PORT_CFG_INFO_t);
        halPortCfgInfo.portId = portId;
        stData.param = (void *)&halPortCfgInfo;
        dalStatus = halAppApiCallSync(&stData);
		if (halPortCfgInfo.linkStatus == adminStatus)
		{
            PORT_API_UNLOCK;
			return OK;
		}
	}

    vosSprintf(portSectionBuff, CFG_PORT_SECTION, portId);
		
    stData.apiId = HAL_API_PORT_ADMIN_SET;
    stData.length = sizeof(HAL_PORT_CFG_INFO_t);
    halPortCfgInfo.portId = portId;
    halPortCfgInfo.admin = adminStatus;
    stData.param = (void *)&halPortCfgInfo;
    dalStatus = halAppApiCallSync(&stData);

	if(OK != dalStatus)
	{
        PORT_API_UNLOCK;
		return dalStatus;
	}
	vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_ADMIN_STATUS,adminStatus);

    PORT_API_UNLOCK;
	return OK;
}

UINT32 odmPortAdminGet(UINT32 portId,UINT32 *adminStatus)
{
	UINT8 	portSectionBuff[255] = {0};
	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
	if(NULL == adminStatus)
	{
		return ERR_NULL_POINTER;
	}
    PORT_API_LOCK;
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*adminStatus = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_ADMIN_STATUS,TRUE);
    PORT_API_UNLOCK;
	return OK;
}
/*******************************************************************************
* odmPortAutoEnableSet
*
* DESCRIPTION:
* this function is used to set auto negocitation status
*	 	portId: 			1 .....
*		adminstatus:    enable or disable
* INPUTS:
*
* OUTPUTS:
*		no
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
UINT32 odmPortAutoEnableSet(UINT32 portId,UINT32 adminStatus)
{
	UINT32 retVal = OK;
	UINT32 oldAutoNegAdminStatus;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_PORT_CFG_INFO_t halPortCfgInfo;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: portId = %d, adminStatus = %d\n", __FUNCTION__, portId, adminStatus);

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	if(TRUE != adminStatus && FALSE != adminStatus)
	{
		return ERR_INVALID_PARAMETERS;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

#if 0  /* deleted by Gan Zhiheng - 2010/11/13 */
	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_AUTO_NEG_ENABLE);
	oldAutoNegAdminStatus = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_AUTO_NEG_ENABLE,TRUE);

	if((OK == retVal && oldAutoNegAdminStatus != adminStatus)||(OK != retVal))
#endif /* #if 0 */
	{
		stData.apiId = HAL_API_PORT_AUTO_ADMIN_SET;
        stData.length = sizeof(HAL_PORT_CFG_INFO_t);
        halPortCfgInfo.portId = portId;
        halPortCfgInfo.autoAdmin = adminStatus;
        stData.param = (void *)&halPortCfgInfo;
        dalStatus = halAppApiCallSync(&stData);
		if(OK != dalStatus)
		{
			return dalStatus;
		}

		vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_AUTO_NEG_ENABLE,adminStatus);
	}

	return OK;
}

UINT32 odmPortAutoEnableGet(UINT32 portId,UINT32 *adminStatus)
{
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
	if(NULL == adminStatus)
	{
		return ERR_NULL_POINTER;
	}
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*adminStatus = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_AUTO_NEG_ENABLE,TRUE);
	return OK;
}

UINT32 odmPortSpeedSet(UINT32 portId,UINT32 portSpeed)
{
	UINT32 retVal = OK;
	UINT32 oldPortSpeed;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_PORT_CFG_INFO_t halPortCfgInfo;
    UINT32 ulPortBitmap;
    UINT32 ulPcost;

	if(odmPortRangeCheck(portId))
	{
	    ODM_PORT_LOG_TRACE();
		return PORT_INVALID;
	}

	if(ODM_PORT_SPEED_10M != portSpeed && ODM_PORT_SPEED_100M != portSpeed && ODM_PORT_SPEED_1000M != portSpeed)
	{
	    ODM_PORT_LOG_TRACE();
		return ERR_INVALID_PARAMETERS;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_SPEED);
	oldPortSpeed = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_SPEED,ODM_PORT_SPEED_100M);

#if 0 /* set the speed mode when autoneg is disabled.*/
	if((OK == retVal && oldPortSpeed != portSpeed)||(OK != retVal))
#endif
	{
	    stData.apiId = HAL_API_PORT_SPEED_SET;
        stData.length = sizeof(HAL_PORT_CFG_INFO_t);
        halPortCfgInfo.portId = portId;
        halPortCfgInfo.speed = portSpeed;
        stData.param = (void *)&halPortCfgInfo;
        dalStatus = halAppApiCallSync(&stData);
		if(OK != dalStatus)
		{
		    ODM_PORT_LOG_TRACE();
			return dalStatus;
		}

		vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_SPEED,portSpeed);
	}

    /* begin added by jiangmingli for bug 2698 */
    ulPortBitmap = (1 << (portId - 1));
    switch (portSpeed)
    {
        case ODM_PORT_SPEED_10M:
        {
            ulPcost = 2000000;
            break;
        }
        case ODM_PORT_SPEED_100M:
        {
            ulPcost = 200000;
            break;
        }
        case ODM_PORT_SPEED_1000M:
        {
            ulPcost = 20000;
            break;
        }
        default:
        {
            ODM_PORT_LOG_TRACE();
    		return ERR_INVALID_PARAMETERS;
        }
    }
#ifndef ONU_1PORT
    cli_multiport_pcost(ulPortBitmap, ulPcost);
#endif
    /* end added by jiangmingli for bug 2698 */
    
	return OK;
}

UINT32 odmPortSpeedGet(UINT32 portId,UINT32 *portSpeed)
{
	UINT8 	portSectionBuff[255] = {0};
	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
	if(NULL == portSpeed)
	{
		return ERR_NULL_POINTER;
	}
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	#if defined(ONU_1PORT) | defined(ONU_4PORT_AR8327)
	*portSpeed = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_SPEED,ODM_PORT_SPEED_1000M);
	#else
	*portSpeed = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_SPEED,ODM_PORT_SPEED_100M);
	#endif
	return OK;
}

UINT32 odmPortDuplexSet(UINT32 portId,UINT32 duplex)
{
	UINT32 retVal = OK;
	UINT32 oldDuplex;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_PORT_CFG_INFO_t halPortCfgInfo;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	if(ODM_PORT_DUPLEX_HALF != duplex && ODM_PORT_DUPLEX_FULL!= duplex)
	{
		return ERR_INVALID_PARAMETERS;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DUPLEX);
	oldDuplex = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DUPLEX,ODM_PORT_DUPLEX_FULL);

	if((OK == retVal && oldDuplex != duplex)||(OK != retVal))
	{

		stData.apiId = HAL_API_PORT_DUPLEX_SET;
        stData.length = sizeof(HAL_PORT_CFG_INFO_t);
        halPortCfgInfo.portId = portId;
        halPortCfgInfo.duplex = duplex;
        stData.param = (void *)&halPortCfgInfo;
        dalStatus = halAppApiCallSync(&stData);
		if(OK != dalStatus)
		{
			return dalStatus;
		}

		vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DUPLEX,duplex);
	}

	return OK;
}

UINT32 odmPortDuplexGet(UINT32 portId,UINT32 *duplex)
{
	UINT8 	portSectionBuff[255] = {0};
	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
	if(NULL == duplex)
	{
		return ERR_INVALID_PARAMETERS;
	}
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*duplex = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DUPLEX,ODM_PORT_DUPLEX_FULL);
	return OK;
}

UINT32 odmPortFlowCtrolEnableSet(UINT32 portId,UINT32 flowCtrlEnable)
{
	UINT32 retVal = OK;
	UINT32 oldPauseEnable;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_PORT_CFG_INFO_t halPortCfgInfo;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	if(TRUE != flowCtrlEnable && FALSE!= flowCtrlEnable)
	{
		return ERR_INVALID_PARAMETERS;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);


	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_FLOWCTRL);
	oldPauseEnable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_FLOWCTRL,FALSE);

	if((OK == retVal && oldPauseEnable != flowCtrlEnable)||(OK != retVal))
	{
	    stData.apiId = HAL_API_PORT_FLOWCONTROL_SET;
        stData.length = sizeof(HAL_PORT_CFG_INFO_t);
        halPortCfgInfo.portId = portId;
        halPortCfgInfo.flowCtrol = flowCtrlEnable;
        stData.param = (void *)&halPortCfgInfo;
        dalStatus = halAppApiCallSync(&stData);
        if(OK != dalStatus)
        {
            return dalStatus;
        }
		vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_FLOWCTRL,flowCtrlEnable);
	}

	return OK;
}

UINT32 odmPortFlowCtrolEnableGet(UINT32 portId,UINT32 *flowCtrlEnable)
{
	UINT8 	portSectionBuff[255] = {0};
	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
	if(NULL == flowCtrlEnable)
	{
		return ERR_INVALID_PARAMETERS;
	}
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*flowCtrlEnable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_FLOWCTRL,FALSE);
	return OK;
}

UINT32 odmPortLoopbackEnableSet(UINT32 portId,UINT32 lbEnable)
{
	UINT32 retVal = OK;
	UINT32 oldLbEnable;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_PORT_CFG_INFO_t halPortCfgInfo;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	if(TRUE != lbEnable && FALSE!= lbEnable)
	{
		return ERR_INVALID_PARAMETERS;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);


	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_LB);
	oldLbEnable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_LB,FALSE);

	if((OK == retVal && oldLbEnable != lbEnable)||(OK != retVal))
	{
	    stData.apiId = HAL_API_PORT_LOOPBACK_SET;
        stData.length = sizeof(HAL_PORT_CFG_INFO_t);
        halPortCfgInfo.portId = portId;
        halPortCfgInfo.loopback= lbEnable;
        stData.param = (void *)&halPortCfgInfo;
        dalStatus = halAppApiCallSync(&stData);
        if(OK != dalStatus)
        {
            return dalStatus;
        }
		vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_LB,lbEnable);
	}

	return OK;
}

UINT32 odmPortLoopbackEnableGet(UINT32 portId,UINT32 *lbEnable)
{
	UINT8 	portSectionBuff[255] = {0};
	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
	if(NULL == lbEnable)
	{
		return ERR_INVALID_PARAMETERS;
	}
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*lbEnable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_LB,FALSE);
	return OK;
}
UINT32 odmPortLearningEnableSet(UINT32 portId,UINT32 learningEnable)
{
	UINT32 retVal = OK;
	UINT32 oldEnable;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    HAL_API_FDB_MAC_LEARN_S stPara;
    OPL_API_DATA_t stData;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	if(TRUE != learningEnable && FALSE!= learningEnable)
	{
		return ERR_INVALID_PARAMETERS;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_LEARNING);
	oldEnable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_LEARNING,FALSE);

	if((OK == retVal && oldEnable != learningEnable)||(OK != retVal))
	{
        stPara.ulPortId = portId;
        stPara.ulLearnEnable = learningEnable;
        stData.apiId = HAL_API_FDB_LEARN_EN_SET;
        stData.param = &stPara;
        stData.length = sizeof(stPara);
        retVal = halAppApiCallSync(&stData);
        if(OK != retVal)
        {
            return retVal;
        }
		vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_LEARNING,learningEnable);
	}

	return OK;
}

UINT32 odmPortLearningEnableGet(UINT32 portId,UINT32 *enable)
{
	UINT8 	portSectionBuff[255] = {0};
	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
	if(NULL == enable)
	{
		return ERR_INVALID_PARAMETERS;
	}
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	/* modified by zgan - 2009/05/20, default learning change to enabled */
 	*enable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_LEARNING,TRUE);
	return OK;
}

UINT32 odmPortMacLimitNumEnableSet(UINT32 portId,UINT32 enable,UINT32 numOfMac)
{
	UINT32 retVal = OK;
	UINT32 oldMacLimitEnable;
	UINT32 oldMacLimitNum;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    HAL_API_FDB_MAC_LIMIT_S stMacLimit;
    OPL_API_DATA_t stData;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	if(TRUE != enable && FALSE!= enable)
	{
		return ERR_INVALID_PARAMETERS;
	}

#if defined(ONU_1PORT)
	if (TRUE == enable && numOfMac > PON_BRG_MAC_ENTRY_NUM){
		return ERR_INVALID_PARAMETERS;
	}
#elif defined(ONU_4PORT_AR8327)
       if (TRUE == enable && ((numOfMac > DAL_MAX_PORT_LEARN_LIMIT_NUM) ||(numOfMac <= 0))){
		return ERR_INVALID_PARAMETERS;
	}
#endif 	

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MAC_LIMIT);
	oldMacLimitEnable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MAC_LIMIT,FALSE);

	if((OK == retVal && oldMacLimitEnable != enable)||(OK != retVal))
	{
		vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MAC_LIMIT,enable);
	}

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MAC_LIMIT_NUM);
	oldMacLimitNum = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MAC_LIMIT_NUM,1024);

	if(TRUE == enable)
	{
        stMacLimit.ulMacNum = numOfMac;
	}
	else
	{
		#if 0
	 	stMacLimit.ulMacNum = 0;
		#else
		stMacLimit.ulMacNum = PON_BRG_MAC_ENTRY_NUM;  /* bug 3285 */
		#endif
	}

    stMacLimit.ulEnable = enable;
	stMacLimit.ulPortId =	portId;
	stData.apiId = HAL_API_FDB_MAC_NUM_LIMIT_SET;
	stData.param = &stMacLimit;
	stData.length = sizeof(HAL_API_FDB_MAC_LIMIT_S);
	retVal = halAppApiCallSync(&stData);

	vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MAC_LIMIT_NUM,numOfMac);

	return OK;
}

UINT32 odmPortMacLimitNumEnableGet(UINT32 portId,UINT32 *enable,UINT32 *numOfMac)
{
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
	if(NULL == enable || NULL == numOfMac)
	{
		return ERR_NULL_POINTER;
	}
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*enable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MAC_LIMIT,FALSE);
    if(TRUE == *enable)
    {
        *numOfMac = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MAC_LIMIT_NUM,1024);
    }else
    {
        /**numOfMac = 0;*/
		*numOfMac = PON_BRG_MAC_ENTRY_NUM; /* bug 3285 */
    }
    return OK;
}

UINT32 odmPortUsPolicingSet(UINT32 portId,UINT8 enable,UINT32 cir,UINT32 cbs,UINT32 ebs)
{
    UINT32 retVal = OK;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: portId = %d, enable = %d, cir = %d, cbs = %d, ebs = %d\n", 
			__FUNCTION__, portId, enable, cir, cbs, ebs);

	if (TRUE == enable) /*fix bug3592.*/
	{
		retVal = odmPortUsCirSet(portId,cir);
	    if(OK != retVal)
	    {
	        return retVal;
	    }

	    retVal = odmPortUsCbsSet(portId,cbs);
	    if(OK != retVal)
	    {
	        return retVal;
	    }

	    retVal = odmPortUsEbsSet(portId,ebs);
	    if(OK != retVal)
	    {
	        return retVal;
	    }

	}

	retVal = odmPortUsPolicingEnableSet(portId,enable);
    if(OK != retVal)
    {
        return retVal;
    }

    return OK;
}

UINT32 odmPortUsPolicingGet(UINT32 portId,UINT8 *enable,UINT32 *cir,UINT32 *cbs,UINT32 *ebs)
{
    UINT32 retVal = OK;
    UINT32 tmpEnable;

    retVal = odmPortUsPolicingEnableGet(portId,&tmpEnable);
    if(OK != retVal)
    {
        return retVal;
    }
    *enable = tmpEnable;

    retVal = odmPortUsCirGet(portId,cir);
    if(OK != retVal)
    {
        return retVal;
    }

    retVal = odmPortUsCbsGet(portId,cbs);
    if(OK != retVal)
    {
        return retVal;
    }

    retVal = odmPortUsEbsGet(portId,ebs);
    if(OK != retVal)
    {
        return retVal;
    }

    return OK;
}


UINT32 odmPortDsPolicingSet(UINT32 portId,UINT8 enable,UINT32 cir,UINT32 pir)
{
    UINT32 retVal = OK;
	
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "[ %s ]: portId = %d, enable = %d, cir = %d, pir = %d\n", 
			__FUNCTION__, portId, enable, cir, pir);

	if (TRUE == enable) /*fix bug3592.*/
	{
	    retVal = odmPortDsCirSet(portId,cir);
	    if(OK != retVal)
	    {
	        return retVal;
	    }

	    retVal = odmPortDsCbsSet(portId,pir);
	    if(OK != retVal)
	    {
	        return retVal;
	    }
	}

	retVal = odmPortDsRateLimitEnableSet(portId,enable);
    if(OK != retVal)
    {
        return retVal;
    }

    return OK;
}

UINT32 odmPortDsPolicingGet(UINT32 portId,UINT8 *enable,UINT32 *cir,UINT32 *cbs)
{
    UINT32 retVal = OK;
    UINT32 tmpEnable;

    retVal = odmPortDsRateLimitEnableGet(portId,&tmpEnable);
    if(OK != retVal)
    {
        return retVal;
    }
    *enable = tmpEnable;

    retVal = odmPortDsCirGet(portId,cir);
    if(OK != retVal)
    {
        return retVal;
    }

    retVal = odmPortDsCbsGet(portId,cbs);
    if(OK != retVal)
    {
        return retVal;
    }

    return OK;
}


UINT32 odmPortUsPolicingEnableSet(UINT32 portId,UINT32 enable)
{
	UINT32 retVal = OK;
	UINT32 oldEnable;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    UINT32  cir,cbs,ebs;
    OPL_API_DATA_t              stData;
    HAL_PORT_RATE_LIMIT_CFG_t   halPortRateLimitCfgInfo;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	if(TRUE != enable && FALSE!= enable)
	{
		return ERR_INVALID_PARAMETERS;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_USPOLICING);
	oldEnable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_USPOLICING,FALSE);
    cir       = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_CIR,ODM_US_CIR);
    cbs       = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_CBS,ODM_US_CBS);
    ebs       = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_EBS,ODM_US_EBS);

	if((OK == retVal && oldEnable != enable)||(OK != retVal))
	{
        stData.apiId = HAL_API_PORT_USPOLICING_SET;
        stData.length = sizeof(HAL_PORT_RATE_LIMIT_CFG_t);
        halPortRateLimitCfgInfo.portId = portId;
        halPortRateLimitCfgInfo.ingressRateLimitEnable = enable;
        halPortRateLimitCfgInfo.ingressCir = cir;
        halPortRateLimitCfgInfo.ingressCbs = cbs;
        halPortRateLimitCfgInfo.ingressEbs = ebs;
        stData.param = (void *)&halPortRateLimitCfgInfo;
        retVal = halAppApiCallSync(&stData);
        if(OK != retVal)
        {
            ODM_PORT_LOG_TRACE();
            return retVal;
        }
	}

	vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_USPOLICING,enable);

	return OK;
}

UINT32 odmPortUsPolicingEnableGet(UINT32 portId,UINT32 *enable)
{
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
	if(NULL == enable)
	{
		return ERR_NULL_POINTER;
	}
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*enable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_USPOLICING,FALSE);
	return OK;
}

UINT32 odmPortUsCirSet(UINT32 portId,UINT32 cir)
{
	UINT32  retVal = OK;
	UINT32  oldCir;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    UINT32  usPolicingEnable;
    UINT32  cbs,ebs;
    OPL_API_DATA_t              stData;
    HAL_PORT_RATE_LIMIT_CFG_t   halPortRateLimitCfgInfo;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	if(PORT_RATE_LIMIT_MAX < cir || 32 > cir)
	{
		return ERR_INVALID_PARAMETERS;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_CIR);
	oldCir = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_CIR,ODM_US_CIR);
    usPolicingEnable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_USPOLICING,FALSE);
    cbs       = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_CBS,ODM_US_CBS);
    ebs       = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_EBS,ODM_US_EBS);

	if((OK == retVal && oldCir != cir)||(OK != retVal))
	{
		if(usPolicingEnable)
		{
			stData.apiId = HAL_API_PORT_USPOLICING_SET;
            stData.length = sizeof(HAL_PORT_RATE_LIMIT_CFG_t);
            halPortRateLimitCfgInfo.portId = portId;
            halPortRateLimitCfgInfo.ingressRateLimitEnable = usPolicingEnable;
            halPortRateLimitCfgInfo.ingressCir = cir;
            halPortRateLimitCfgInfo.ingressCbs = cbs;
            halPortRateLimitCfgInfo.ingressEbs = ebs;
            stData.param = (void *)&halPortRateLimitCfgInfo;
            retVal = halAppApiCallSync(&stData);

			if(OK != retVal)
	        {
	            ODM_PORT_LOG_TRACE();
	            return retVal;
	        }
		}
	}

	vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_CIR,cir);

	return OK;
}

UINT32 odmPortUsCirGet(UINT32 portId,UINT32 *cir)
{
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
    if(NULL == cir)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*cir = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_CIR,ODM_US_CIR);
	return OK;
}

UINT32 odmPortUsCbsSet(UINT32 portId,UINT32 cbs)
{
	UINT32  retVal = OK;
	UINT32  oldCbs;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    UINT32  usPolicingEnable;
    UINT32  cir,ebs;
    OPL_API_DATA_t              stData;
    HAL_PORT_RATE_LIMIT_CFG_t   halPortRateLimitCfgInfo;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_CBS);
	oldCbs = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_CBS,ODM_US_CBS);
	usPolicingEnable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_USPOLICING,FALSE);
    cir = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_CIR,ODM_US_CIR);
    ebs = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_EBS,ODM_US_EBS);

	if((OK == retVal && oldCbs!= cbs)||(OK != retVal))
	{
		if(usPolicingEnable)
		{
		    stData.apiId = HAL_API_PORT_USPOLICING_SET;
            stData.length = sizeof(HAL_PORT_RATE_LIMIT_CFG_t);
            halPortRateLimitCfgInfo.portId = portId;
            halPortRateLimitCfgInfo.ingressRateLimitEnable = usPolicingEnable;
            halPortRateLimitCfgInfo.ingressCir = cir;
            halPortRateLimitCfgInfo.ingressCbs = cbs;
            halPortRateLimitCfgInfo.ingressEbs = ebs;
            stData.param = (void *)&halPortRateLimitCfgInfo;
            retVal = halAppApiCallSync(&stData);

	        if(OK != retVal)
	        {
	            ODM_PORT_LOG_TRACE();
	            return retVal;
	        }
		}
	}

	vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_CBS,cbs);

	return OK;
}

UINT32 odmPortUsCbsGet(UINT32 portId,UINT32 *cbs)
{
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
    if(NULL == cbs)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*cbs = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_CBS,ODM_US_CBS);
	return OK;
}

UINT32 odmPortUsEbsSet(UINT32 portId,UINT32 ebs)
{
	UINT32  retVal = OK;
	UINT32  oldEbs;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    UINT32  usPolicingEnable;
    UINT32  cir,cbs;
    OPL_API_DATA_t              stData;
    HAL_PORT_RATE_LIMIT_CFG_t   halPortRateLimitCfgInfo;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_EBS);
	oldEbs = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_EBS,ODM_US_EBS);
	usPolicingEnable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_USPOLICING,FALSE);
	cir    = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_CIR,ODM_US_CIR);
    cbs    = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_CBS,ODM_US_CBS);

	if((OK == retVal && oldEbs != ebs)||(OK != retVal))
	{
		if(usPolicingEnable)
		{
			stData.apiId = HAL_API_PORT_USPOLICING_SET;
            stData.length = sizeof(HAL_PORT_RATE_LIMIT_CFG_t);
            halPortRateLimitCfgInfo.portId = portId;
            halPortRateLimitCfgInfo.ingressRateLimitEnable = usPolicingEnable;
            halPortRateLimitCfgInfo.ingressCir = cir;
            halPortRateLimitCfgInfo.ingressCbs = cbs;
            halPortRateLimitCfgInfo.ingressEbs = ebs;
            stData.param = (void *)&halPortRateLimitCfgInfo;
            retVal = halAppApiCallSync(&stData);

	        if(OK != retVal)
	        {
	            ODM_PORT_LOG_TRACE();
	            return retVal;
	        }
		}
	}

	vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_EBS,ebs);

	return OK;
}

UINT32 odmPortUsEbsGet(UINT32 portId,UINT32 *ebs)
{
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
    if(NULL == ebs)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*ebs = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_US_POLICING_EBS,ODM_US_EBS);
	return OK;
}

UINT32 odmPortDsRateLimitEnableSet(UINT32 portId,UINT32 enable)
{
	UINT32  retVal = OK;
	UINT32  oldEnable;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    UINT32  cir,cbs;
    OPL_API_DATA_t              stData;
    HAL_PORT_RATE_LIMIT_CFG_t   halPortRateLimitCfgInfo;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

    if(TRUE != enable && FALSE != enable)
    {
        return ERR_INVALID_PARAMETERS;
    }

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_RATE_LIMIT);
	oldEnable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_RATE_LIMIT,FALSE);
    cir       = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_CIR,ODM_DS_CIR);
    cbs       = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_CBS,ODM_DS_CBS);

	if((OK == retVal && oldEnable != enable)||(OK != retVal))
	{
   		stData.apiId = HAL_API_PORT_DSPOLICING_SET;
        stData.length = sizeof(HAL_PORT_RATE_LIMIT_CFG_t);
        halPortRateLimitCfgInfo.portId = portId;
        halPortRateLimitCfgInfo.egressRateLimitEnable = enable;
        halPortRateLimitCfgInfo.egressCir = cir;
        halPortRateLimitCfgInfo.egressCbs = cbs;
        halPortRateLimitCfgInfo.egressEbs = 0;
        stData.param = (void *)&halPortRateLimitCfgInfo;
        retVal = halAppApiCallSync(&stData);

        if(OK != retVal)
        {
            ODM_PORT_LOG_TRACE();
            return retVal;
        }
	}
	vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_RATE_LIMIT,enable);

	return OK;
}

UINT32 odmPortDsRateLimitEnableGet(UINT32 portId,UINT32 *enable)
{
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
    if(NULL == enable)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*enable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_RATE_LIMIT,FALSE);
	return OK;
}

UINT32 odmPortDsCirSet(UINT32 portId,UINT32 cir)
{
	UINT32  retVal = OK;
	UINT32  oldCir;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    UINT32  enable,cbs;
    OPL_API_DATA_t              stData;
    HAL_PORT_RATE_LIMIT_CFG_t   halPortRateLimitCfgInfo;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	if(PORT_RATE_LIMIT_MAX < cir || 32 > cir)
	{
		return ERR_INVALID_PARAMETERS;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_CIR);
	oldCir = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_CIR,ODM_DS_CIR);
    enable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_RATE_LIMIT,FALSE);
    cbs    = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_CBS,ODM_DS_CBS);
	if((OK == retVal && oldCir != cir)||(OK != retVal))
	{
		if(enable)
		{
			stData.apiId = HAL_API_PORT_DSPOLICING_SET;
            stData.length = sizeof(HAL_PORT_RATE_LIMIT_CFG_t);
            halPortRateLimitCfgInfo.portId = portId;
            halPortRateLimitCfgInfo.egressRateLimitEnable = enable;
            halPortRateLimitCfgInfo.egressCir = cir;
            halPortRateLimitCfgInfo.egressCbs = cbs;
            halPortRateLimitCfgInfo.egressEbs = 0;
            stData.param = (void *)&halPortRateLimitCfgInfo;
            retVal = halAppApiCallSync(&stData);

			if(OK != retVal)
	        {
	            ODM_PORT_LOG_TRACE();
	            return retVal;
	        }
		}
	}
	vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_CIR,cir);

	return OK;
}

UINT32 odmPortDsCirGet(UINT32 portId,UINT32 *cir)
{
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
    if(NULL == cir)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*cir = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_CIR,ODM_DS_CIR);
	return OK;
}

UINT32 odmPortDsCbsSet(UINT32 portId,UINT32 cbs)
{
	UINT32  retVal = OK;
	UINT32  oldCbs;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    UINT32  enable,cir;
    OPL_API_DATA_t              stData;
    HAL_PORT_RATE_LIMIT_CFG_t   halPortRateLimitCfgInfo;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_CBS);
	oldCbs = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_CBS,ODM_DS_CBS);
    cir = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_CIR,ODM_DS_CIR);
    enable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_RATE_LIMIT,FALSE);

	if((OK == retVal && oldCbs != cbs)||(OK != retVal))
	{
		if(enable)
		{
			stData.apiId = HAL_API_PORT_DSPOLICING_SET;
            stData.length = sizeof(HAL_PORT_RATE_LIMIT_CFG_t);
            halPortRateLimitCfgInfo.portId = portId;
            halPortRateLimitCfgInfo.egressRateLimitEnable = enable;
            halPortRateLimitCfgInfo.egressCir = cir;
            halPortRateLimitCfgInfo.egressCbs = cbs;
            halPortRateLimitCfgInfo.egressEbs = 0;
            stData.param = (void *)&halPortRateLimitCfgInfo;
            retVal = halAppApiCallSync(&stData);

			if(OK != retVal)
	        {
	            ODM_PORT_LOG_TRACE();
	            return retVal;
	        }
		}
	}
	vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_CBS,cbs);

	return OK;
}

UINT32 odmPortDsCbsGet(UINT32 portId,UINT32 *cbs)
{
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
    if(NULL == cbs)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
    *cbs = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DS_CBS,ODM_DS_CBS);
	return OK;
}

UINT32 odmPortMultcastTagStripEnableSet(UINT32 portId,UINT32 enable)
{
	UINT32  retVal = OK;
	UINT32  oldEnable;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

    if(TRUE != enable && FALSE != enable)
    {
        return ERR_INVALID_PARAMETERS;
    }

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	//retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MULTICAST_TAG_STRIP);
	//oldEnable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MULTICAST_TAG_STRIP,0);

	//if((OK == retVal && oldEnable != enable)||(OK != retVal))
	{
    #if defined(ONU_1PORT)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
        retVal = dalMulticastVlanTagStripe(portId,enable);
    #elif defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)	    
        retVal = OK;
    #elif defined(ONU_4PORT_88E6045)
        retVal = OK;
    #endif
		vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MULTICAST_TAG_STRIP,enable);
	}

	return OK;
}

UINT32 odmPortMultcastTagStripEnableGet(UINT32 portId,UINT32 *enable)
{
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
    if(NULL == enable)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*enable = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MULTICAST_TAG_STRIP,0);
	return OK;
}

UINT32 odmPortMaxMulticastGroupNumSet(UINT32 portId,UINT32 num)
{
	UINT32  retVal = OK;
	UINT32  oldNum;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

    if(ODM_MAX_GRUP_NUM_PER_PORT < num || 0 == num)
    {
        return ERR_INVALID_PARAMETERS;
    }

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MAX_MULTICAST_GROUP);
	oldNum = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MAX_MULTICAST_GROUP,0);

	if((OK == retVal && oldNum != num)||(OK != retVal))
	{
	/*
		set dal layer api for chip
	*/

		vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MAX_MULTICAST_GROUP,num);
	}

	return OK;
}

UINT32 odmPortMaxMulticastGroupNumGet(UINT32 portId,UINT32 *num)
{
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
    if(NULL == num)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*num = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_MAX_MULTICAST_GROUP,0);
	return OK;
}

UINT32 odmPortVlanModeSet(UINT32 portId,UINT32 vlanMode)
{
	UINT32  retVal = OK;
	UINT32  oldVttMode;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    UINT32  vttRecIndex;
    UINT32  oldVttOldVlan,oldVttNewVlan;
    UINT32  valid = FALSE;
    UINT32  oldTpid,newTpid;
    OPL_API_DATA_t      stData;
    HAL_PORT_VLAN_CFG_t halPortVlanCfg;
    HAL_PORT_VTT_TAB_t  halPortVttTab;
	VoiceVlan_t SysVoiceVlan;
    UINT32  defaultVlan;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

    if(ODM_VLAN_END <= vlanMode)
    {
        return ERR_INVALID_PARAMETERS;
    }

    odmPortDefaultVlanGet(portId, &defaultVlan);

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_MODE);
	oldVttMode = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_MODE,ODM_VLAN_TRANSPARENT);
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "oldVttMode = %d\n", oldVttMode);

#if defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6097)
    if(ODM_VLAN_TRANSLATION == vlanMode)
    {
        printf("no support this mode.\n");
        return ERR_INVALID_PARAMETERS;
    }
#endif
//if(oldVttMode!=vlanMode)/*fixed bug 3513*/
if(ODM_VLAN_TRANSPARENT==vlanMode)
{
   if((OK==retVal)&&(ODM_VLAN_TRANSPARENT==oldVttMode))
             return OK;
}
{
#if defined(ONU_4PORT_AR8228) || defined(ONU_1PORT)||defined(ONU_4PORT_AR8327)
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "check if is vlanmode(%d)\n", ODM_VLAN_TRUNK);
	if(oldVttMode == ODM_VLAN_TRUNK)
	{
		/* remove all vlan trunk entries */
		odmPortVlanTrunkEntryClear(portId);
	}
#endif

	if(oldVttMode == ODM_VLAN_TRANSLATION)
	{
		odmPortVttFlush(portId);
	}

#if defined(ONU_1PORT) || defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	if(oldVttMode == ODM_VLAN_AGGREGATION)
	{
		odmVlanAggUninit(portId);
	}

	if(oldVttMode == ODM_VLAN_TRANSPARENT)
	{
		odmPortVttFlush(portId);
	}

	if (vlanMode == ODM_VLAN_AGGREGATION)
	{
		odmVlanAggInit(portId);
	}

	/* update CPU If */
	vosMemSet(&SysVoiceVlan,0,sizeof(VoiceVlan_t));
	SysVoiceVlan.VoiceVlanCos=5;
	SysVoiceVlan.VoipPortNum=0;
	SysVoiceVlan.VoiceVlanId = (UINT16)defaultVlan;

#endif

    if(vlanMode == ODM_VLAN_TRANSPARENT)
    {
		odmPortDefaultVlanSetToAny(portId, 1);
#if defined(ONU_1PORT)
		/* update CPU If */
		SysVoiceVlan.VoipTagValid=0;
		odmSysCfgIfTagModeSet(ETH0, ADMIN, vlanMode); 
		syscall(NR_CFG_VOICEVLAN, (char*)&SysVoiceVlan, sizeof(VoiceVlan_t));
#endif
    }
	else 
	{
#if defined(ONU_1PORT)
		/* update CPU If */
		SysVoiceVlan.VoipTagValid=1;
		syscall(NR_CFG_VOICEVLAN, (char*)&SysVoiceVlan, sizeof(VoiceVlan_t));
		odmSysCfgIfTagModeSet(ETH0, ADMIN, TAGMODE);  //TAG MODE
		odmSysCfgIfCVLANSet(ETH0, ADMIN, (UINT16)defaultVlan);
		odmSysCfgIfPriSet(ETH0, ADMIN, 5);
#endif
	}

	/*
    if(oldVttMode == ODM_VLAN_TRANSLATION && vlanMode != ODM_VLAN_TRANSLATION)
    {
        odmPortVttFlush(portId);
    }
    if(oldVttMode == ODM_VLAN_AGGREGATION && vlanMode != ODM_VLAN_AGGREGATION)
    {
		odmVlanAggDelByPort(portId);
    }
    */
    if((OK == retVal)||(OK != retVal))
	{
        stData.apiId = HAL_API_PORT_VLAN_MODE_SET;
        stData.length = sizeof(HAL_PORT_VLAN_CFG_t);
        halPortVlanCfg.portId = portId;
        halPortVlanCfg.vlanMode = vlanMode;
        stData.param = (void *)&halPortVlanCfg;
        retVal = halAppApiCallSync(&stData);
        if(OK != retVal)
        {
            ODM_PORT_LOG_TRACE();		
            return retVal;
        }
		vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_MODE,vlanMode);
	}
}

	return OK;
}

UINT32 odmPortVlanModeGet(UINT32 portId,UINT32 *vlanMode)
{
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
    if(NULL == vlanMode)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*vlanMode = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_MODE,ODM_VLAN_TRANSPARENT);
	return OK;
}

UINT32 odmPortDefaultVlanSetToAny(UINT32 portId,UINT32 defaultVlan)
{
	UINT32  retVal = OK;
	UINT32  oldDefaultVlan;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_PORT_CFG_INFO_t halPortCfg;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\r\[ %s ]: portId = %d, defaultVid = %d\r\n", __FUNCTION__, portId, defaultVlan);

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	/*bug 3042: vlan id 1 is default value and not supposed as invalid id for F23, besides so does F13.*/
    if(1 != defaultVlan && odmVlanRangeCheck(defaultVlan))
    {
        return ERR_INVALID_PARAMETERS;
    }

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DEFAULT_VID);
	oldDefaultVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DEFAULT_VID,1);

	/* force to set the pvid */
	if((OK == retVal)||(OK != retVal))
	{
        stData.apiId = HAL_API_PORT_PVID_SET;
        stData.length = sizeof(HAL_PORT_CFG_INFO_t);
        halPortCfg.portId = portId;
        halPortCfg.pvid = defaultVlan;
        stData.param = (void *)&halPortCfg;
        retVal = halAppApiCallSync(&stData);
        if(OK != retVal)
        {
            ODM_PORT_LOG_TRACE();
            return retVal;
        }
		vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DEFAULT_VID,defaultVlan);
	}

	return OK;
}


UINT32 odmPortDefaultVlanSet(UINT32 portId,UINT32 defaultVlan)
{
	UINT32  retVal = OK;
	UINT32  oldDefaultVlan;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_PORT_CFG_INFO_t halPortCfg;
	VoiceVlan_t SysVoiceVlan;
    UINT8   ucCPUIfTagMode;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\r\[ %s ]: portId = %d, defaultVid = %d\r\n", __FUNCTION__, portId, defaultVlan);

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
    if(odmVlanRangeCheck(defaultVlan))
    {
        return ERR_INVALID_PARAMETERS;
    }

#if defined(ONU_1PORT)
    odmSysCfgIfTagModeGet(ETH0, ADMIN, &ucCPUIfTagMode);
    if (TRANSPARENTMODE != ucCPUIfTagMode)
    {
        vosMemSet(&SysVoiceVlan,0,sizeof(VoiceVlan_t));
    	SysVoiceVlan.VoiceVlanCos=5;
    	SysVoiceVlan.VoipTagValid=1;
    	SysVoiceVlan.VoipPortNum=0;
    	SysVoiceVlan.VoiceVlanId = (UINT16)defaultVlan;
    	syscall(NR_CFG_VOICEVLAN, (char*)&SysVoiceVlan, sizeof(VoiceVlan_t));
    }
#endif

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DEFAULT_VID);
	oldDefaultVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DEFAULT_VID,1);

	/* force to set the pvid */
	if((OK == retVal)||(OK != retVal))
	{
        stData.apiId = HAL_API_PORT_PVID_SET;
        stData.length = sizeof(HAL_PORT_CFG_INFO_t);
        halPortCfg.portId = portId;
        halPortCfg.pvid = defaultVlan;
        stData.param = (void *)&halPortCfg;
        retVal = halAppApiCallSync(&stData);
        if(OK != retVal)
        {
            ODM_PORT_LOG_TRACE();
            return retVal;
        }
		vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DEFAULT_VID,defaultVlan);
	}

	return OK;
}

UINT32 odmPortDefaultVlanGet(UINT32 portId,UINT32 *defaultVlan)
{
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
    if(NULL == defaultVlan)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*defaultVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DEFAULT_VID,1);
	return OK;
}

UINT32 odmPortDefaultEtherPriSet(UINT32 portId,UINT8 defaultEtherPri)
{
	UINT32  retVal = OK;
	UINT32  oldDefaultPri;
	UINT8 	portSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_PORT_VLAN_CFG_t halPortVlanCfg;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\r\[ %s ]: portId = %d, defaultPri = %d\r\n", __FUNCTION__, portId, defaultEtherPri);

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

    if(ODM_MAX_ETH_PRI < defaultEtherPri)
    {
        return ERR_INVALID_PARAMETERS;
    }

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DEFAULT_ETHER_PRI);
	oldDefaultPri = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DEFAULT_ETHER_PRI,0);

	if((OK == retVal && oldDefaultPri != defaultEtherPri)||(OK != retVal))
	{
#if defined(ONU_1PORT)
	    retVal = dalPortDefaultPriSet(portId, defaultEtherPri);
#else
		retVal = OK;
#endif
        if(OK != retVal)
        {
            ODM_PORT_LOG_TRACE();
            return retVal;
        }
		vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DEFAULT_ETHER_PRI,defaultEtherPri);
	}

	return OK;
}

UINT32 odmPortDefaultEtherPriGet(UINT32 portId,UINT8 *defaultEtherPri)
{
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
    if(NULL == defaultEtherPri)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
	*defaultEtherPri = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DEFAULT_ETHER_PRI,0);
	return OK;
}

UINT32 odmPortVttEntryAdd(UINT32 portId,UINT32 oldTpid,UINT32 oldVlan,UINT32 newTpid,UINT32 newVlan)
{
	UINT32 retVal = OK;
	UINT32 vttEntryIndex;
    UINT32 oldVttOldVlan,oldVttNewVlan,newVttOldVlan,newVttNewVlan,newVttOtherVlan;
	UINT8  portSectionBuff[255] = {0};
    UINT8  vttEntryOldBuff[255] = {0};
    UINT8  vttEntryNewBuff[255] = {0};
    UINT8  vttEntryportBuff[255] = {0};
    UINT32 portVlanMode;
	INT32  dalStatus = OK;
    UINT32 vttEntryNum = 0;
    UINT32 firstEmptyEntryId = 0xffff;
    OPL_API_DATA_t      stData;
    HAL_PORT_VTT_TAB_t  halPortVttTab;
    UINT32 realChipIndex = 0;
	UINT32 portNum;
	UINT32 vttRecId = 0;
	UINT32 vlanMode ;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

    newVttOldVlan = oldVlan&0xfff;
    newVttNewVlan = newVlan&0xfff;

    if (odmVlanRangeCheck(newVttOldVlan) || odmVlanRangeCheck(newVttNewVlan))
    {
        return ERR_INVALID_PARAMETERS;
    }
	/*
	for(portNum = 1; portNum <= ODM_NUM_OF_PORTS; portNum++)
	{
		if(portNum!=portId)
		{		
			 if(odmPortVlanModeGet(portNum,&vlanMode)|| vlanMode!=ODM_VLAN_TRANSLATION) 
				{
					continue;
				}
				vosSprintf(portSectionBuff,CFG_PORT_SECTION,portNum);
				
				for(vttRecId = 0; vttRecId < ODM_NUM_OF_VTT; vttRecId++)	
				{
					vosSprintf(vttEntryportBuff,CFG_PORT_VTT_NEW,vttRecId);
					newVttOtherVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vttEntryportBuff,0);
	
					if(newVttOtherVlan==newVlan) 
						return VTT_ENTRY_CONFLICT;
				}
		}
	}
	*/
    /*NOW ALL CHIP DO NOT SUPPORT TPID TRANSLATION, SO NO CHECK TPID*/

		vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	    portVlanMode = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_MODE,ODM_VLAN_TRANSPARENT);

	    if(ODM_VLAN_TRANSPARENT == portVlanMode)
	    {
	        return VTT_VLAN_NOT_AT_TRANSLATION_MODE;
	    }

	    for(vttEntryIndex = 0; vttEntryIndex < ODM_NUM_OF_VTT; vttEntryIndex++)
	    {
	        vosSprintf(vttEntryOldBuff,CFG_PORT_VTT_OLD,vttEntryIndex);
	        vosSprintf(vttEntryNewBuff,CFG_PORT_VTT_NEW,vttEntryIndex);

	        retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff);
	        if(OK != retVal)
	        {
	            if(0xffff == firstEmptyEntryId)
	            {
	                firstEmptyEntryId = vttEntryIndex;
	            }
	            continue;
	        }
	        vttEntryNum++;
	        oldVttOldVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff,0);
	        oldVttNewVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vttEntryNewBuff,0);
		
	        if(oldVttOldVlan == newVttOldVlan && oldVttNewVlan == newVttNewVlan)
	        {
	            return VTT_ENTRY_ISEXISTED;
	        }else if(oldVttOldVlan == newVttOldVlan || oldVttNewVlan == newVttNewVlan)
	        {
	            return VTT_ENTRY_CONFLICT;
	        }

	        if(ODM_NUM_OF_VTT == vttEntryNum)
	        {
	            return VTT_ENTRY_FULL;
	        }
    }
    if(0xffff == firstEmptyEntryId)
    {
        return VTT_ENTRY_FULL;
    }

    stData.apiId = HAL_API_PORT_VTT_ENTRY_ADD;
    stData.length = sizeof(HAL_PORT_VTT_TAB_t);
    halPortVttTab.portId = portId;
    halPortVttTab.sTpid = 0;
    halPortVttTab.sVlan = newVttNewVlan;
    halPortVttTab.cTpid = 0;
    halPortVttTab.cVlan = newVttOldVlan;
    stData.param = (void *)&halPortVttTab;
    retVal = halAppApiCallSync(&stData);

    if(OK != retVal)
    {
        return retVal;
    }

	if(halPortVttTab.retVal != NO_ERROR)
	{
		return halPortVttTab.retVal;
	}

    vosSprintf(vttEntryOldBuff,CFG_PORT_VTT_OLD,firstEmptyEntryId);
    vosSprintf(vttEntryNewBuff,CFG_PORT_VTT_NEW,firstEmptyEntryId);
    vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff,newVttOldVlan);
    vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,vttEntryNewBuff,newVttNewVlan);
	return OK;
}

UINT32 odmPortVttEntryGet(UINT32 portId,UINT32 entryIndex,UINT32 *valid,UINT32 *oldTpid,UINT32 *oldVlan,UINT32 *newTpid,UINT32 *newVlan)
{
	UINT32 retVal = OK;
	UINT8  portSectionBuff[255] = {0};
    UINT8  vttEntryOldBuff[255] = {0};
    UINT8  vttEntryNewBuff[255] = {0};
    UINT32 portVlanMode;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
    if(ODM_NUM_OF_VTT <= entryIndex)
    {
        return VTT_INDEX_OUT_OF_RANGE;
    }
    if(NULL == valid || NULL == oldTpid || NULL == newTpid || NULL == oldVlan || NULL == newVlan)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);
    portVlanMode = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_MODE,ODM_VLAN_TRANSPARENT);
    if(ODM_VLAN_TRANSPARENT == portVlanMode)
    {
        return VTT_VLAN_NOT_AT_TRANSLATION_MODE;
    }
    vosSprintf(vttEntryOldBuff,CFG_PORT_VTT_OLD,entryIndex);
    vosSprintf(vttEntryNewBuff,CFG_PORT_VTT_NEW,entryIndex);
    retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff);
    if(OK != retVal)
    {
        *valid = FALSE;
        return OK;
    }
	*valid = TRUE;
    *oldVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff,0);
    *newVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vttEntryNewBuff,0);
	return OK;
}

UINT32 odmPortVttEntryDel(UINT32 portId,UINT32 oldTpid,UINT32 oldVlan,UINT32 newTpid,UINT32 newVlan)
{
	UINT32 retVal = OK;
	UINT32 vttEntryIndex;
    UINT32 oldVttOldVlan,oldVttNewVlan,newVttOldVlan,newVttNewVlan;
	UINT8  portSectionBuff[255] = {0};
    UINT8  vttEntryOldBuff[255] = {0};
    UINT8  vttEntryNewBuff[255] = {0};
    UINT32 portVlanMode;
	INT32  dalStatus = OK;
    UINT32 vttEntryNum = 0;
    OPL_API_DATA_t      stData;
    HAL_PORT_VTT_TAB_t  halPortVttTab;
    UINT32 realChipIndex = 0;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	if (odmVlanRangeCheck(oldVlan) || odmVlanRangeCheck(newVlan))
    {
        return ERR_INVALID_PARAMETERS;
    }

    /*NOW ALL CHIP DO NOT SUPPORT TPID TRANSLATION, SO NO CHECK TPID*/

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

    portVlanMode = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_MODE,ODM_VLAN_TRANSPARENT);

    if(ODM_VLAN_TRANSLATION != portVlanMode)
    {
        return VTT_VLAN_NOT_AT_TRANSLATION_MODE;
    }

    for(vttEntryIndex = 0; vttEntryIndex < ODM_NUM_OF_VTT; vttEntryIndex++)
    {
        vosSprintf(vttEntryOldBuff,CFG_PORT_VTT_OLD,vttEntryIndex);
        vosSprintf(vttEntryNewBuff,CFG_PORT_VTT_NEW,vttEntryIndex);

        retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff);
        if(OK != retVal)
        {
            continue;
        }

        oldVttOldVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff,0);
        oldVttNewVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vttEntryNewBuff,0);

        if(oldVttOldVlan == oldVlan && oldVttNewVlan == newVlan)
        {
            stData.apiId = HAL_API_PORT_VTT_ENTRY_DEL;
            stData.length = sizeof(HAL_PORT_VTT_TAB_t);
            halPortVttTab.portId = portId;
            halPortVttTab.sTpid = 0;
            halPortVttTab.sVlan = oldVttNewVlan;
            halPortVttTab.cTpid = 0;
            halPortVttTab.cVlan = oldVttOldVlan;
            stData.param = (void *)&halPortVttTab;
            retVal = halAppApiCallSync(&stData);
            if(OK != retVal)
            {
                ODM_PORT_LOG_TRACE();
                return retVal;
            }

			if(halPortVttTab.retVal != NO_ERROR)
			{
				return halPortVttTab.retVal;
			}

            vosConfigKeyDelete(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff);
            vosConfigKeyDelete(CFG_PORT_CFG,portSectionBuff,vttEntryNewBuff);

            return OK;
        }
    }

    return VTT_NO_SUCH_ENTRY;
}

UINT32 odmPortVttSpecifyEntryDel(UINT32 portId,UINT32 entryId)
{
	UINT32 retVal = OK;
    UINT32 oldVttOldVlan,oldVttNewVlan;
	UINT8  portSectionBuff[255] = {0};
    UINT8  vttEntryOldBuff[255] = {0};
    UINT8  vttEntryNewBuff[255] = {0};
    UINT32 portVlanMode;
	INT32  dalStatus = OK;
    UINT32 vttEntryNum = 0;
    OPL_API_DATA_t      stData;
    HAL_PORT_VTT_TAB_t  halPortVttTab;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

    if(ODM_NUM_OF_VTT <= entryId)
    {
        return VTT_INDEX_OUT_OF_RANGE;
    }
    /*NOW ALL CHIP DO NOT SUPPORT TPID TRANSLATION, SO NO CHECK TPID*/

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

    portVlanMode = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_MODE,ODM_VLAN_TRANSPARENT);

    if(ODM_VLAN_TRANSLATION != portVlanMode)
    {
        return VTT_VLAN_NOT_AT_TRANSLATION_MODE;
    }

    vosSprintf(vttEntryOldBuff,CFG_PORT_VTT_OLD,entryId);
    vosSprintf(vttEntryNewBuff,CFG_PORT_VTT_NEW,entryId);
    retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff);
    if(OK != retVal)
    {
        return OK;
    }

    oldVttOldVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff,0);
    oldVttNewVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vttEntryNewBuff,0);

    stData.apiId = HAL_API_PORT_VTT_ENTRY_DEL;
    stData.length = sizeof(HAL_PORT_VTT_TAB_t);
    halPortVttTab.portId = portId;
    halPortVttTab.sTpid = 0;
    halPortVttTab.sVlan = oldVttNewVlan;
    halPortVttTab.cTpid = 0;
    halPortVttTab.cVlan = oldVttOldVlan;
    stData.param = (void *)&halPortVttTab;
    retVal = halAppApiCallSync(&stData);

    if(OK != retVal)
    {
        ODM_PORT_LOG_TRACE();
        return retVal;
    }

	if(halPortVttTab.retVal != NO_ERROR)
	{
		return halPortVttTab.retVal;
	}

    vosConfigKeyDelete(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff);
    vosConfigKeyDelete(CFG_PORT_CFG,portSectionBuff,vttEntryNewBuff);

    return OK;
}

UINT32 odmPortVttFlush(UINT32 portId)
{
	UINT32 retVal = OK;
    UINT32 oldVttOldVlan,oldVttNewVlan;
	UINT8  portSectionBuff[255] = {0};
    UINT8  vttEntryOldBuff[255] = {0};
    UINT8  vttEntryNewBuff[255] = {0};
    UINT32 portVlanMode;
	INT32  dalStatus = OK;
    UINT32 vttEntryId = 0;
    OPL_API_DATA_t      stData;
    HAL_PORT_VTT_TAB_t  halPortVttTab;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

    /*NOW ALL CHIP DO NOT SUPPORT TPID TRANSLATION, SO NO CHECK TPID*/

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

    portVlanMode = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_MODE,ODM_VLAN_TRANSPARENT);

    if(ODM_VLAN_TRANSLATION != portVlanMode)
    {
        return VTT_VLAN_NOT_AT_TRANSLATION_MODE;
    }

    for(vttEntryId = 0; vttEntryId < ODM_NUM_OF_VTT; vttEntryId++)
    {
        vosSprintf(vttEntryOldBuff,CFG_PORT_VTT_OLD,vttEntryId);
        vosSprintf(vttEntryNewBuff,CFG_PORT_VTT_NEW,vttEntryId);
        retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff);
        if(OK != retVal)
        {
            continue;
        }

        oldVttOldVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff,0);
        oldVttNewVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vttEntryNewBuff,0);

        stData.apiId = HAL_API_PORT_VTT_ENTRY_DEL;
        stData.length = sizeof(HAL_PORT_VTT_TAB_t);
        halPortVttTab.portId = portId;
        halPortVttTab.sTpid = 0;
        halPortVttTab.sVlan = oldVttNewVlan;
        halPortVttTab.cTpid = 0;
        halPortVttTab.cVlan = oldVttOldVlan;
        stData.param = (void *)&halPortVttTab;
        retVal = halAppApiCallSync(&stData);
        if(OK != retVal)
        {
            ODM_PORT_LOG_TRACE();
            return retVal;
        }

		if(halPortVttTab.retVal != NO_ERROR)
		{
			return halPortVttTab.retVal;
		}

        vosConfigKeyDelete(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff);
        vosConfigKeyDelete(CFG_PORT_CFG,portSectionBuff,vttEntryNewBuff);

    }
    return OK;
}
UINT32 odmPortNumOfVttGet(UINT32 portId,UINT8 *numOfVttRecs)
{
	UINT32 retVal = OK;
    HAL_PORT_VLAN_CFG_t halPortVlanCfg;
    OPL_API_DATA_t      stData;

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}
    if(NULL == numOfVttRecs)
    {
        return ERR_NULL_POINTER;
    }

    stData.apiId = HAL_API_PORT_VTT_ENTRY_COUNT_GET;
    stData.length = sizeof(HAL_PORT_VLAN_CFG_t);
    halPortVlanCfg.portId = portId;
    stData.param = (void *)&halPortVlanCfg;
    retVal = halAppApiCallSync(&stData);
    if(OK != retVal)
    {
       return retVal;
    }

	if(halPortVlanCfg.retVal != NO_ERROR)
	{
		return halPortVlanCfg.retVal;
	}

	*numOfVttRecs = halPortVlanCfg.numOfVttEntry;
    return retVal;
}

UINT32 odmPortCfgInit(void)
{
    UINT32  vlanMode;
    UINT32  enableFlag;
    UINT32  macLimitNum;
    UINT32  learnEnable;
    UINT32  portId;
	UINT8   dsRateLimitEn;
	UINT32  dsCir;
	UINT32  dsPir;
	UINT8   usPolicingEn;
	UINT32  usCir;
	UINT32  usCbs;
	UINT32  usEbs;
    UINT8   portSectionBuff[255] = {0};

	UINT32  vttEntryIndex;
    UINT32  vttOldVlan,vttNewVlan;
    UINT8   vttEntryOldBuff[255] = {0};
    UINT8   vttEntryNewBuff[255] = {0};
    UINT32  portVlanMode;
    UINT32  vttEntryNum = 0;

	UINT32  vlanTrunkEntryId;
	UINT32  vlanTrunkEntryValue;
	UINT32  vlanTrunkEntryBuff[255] = {0};
	UINT32  defaultVlan;
	UINT32  retVal = OK;
	
	UINT32 portSpeed;
	UINT32 duplex;
	UINT32 adminStatus, autoStatus;

	retVal =  vosMutexCreate(&g_pstPortApiLock);
	if (VOS_MUTEX_OK != retVal)
    { 
        printf(("odmPortCfgInit: create port access mutex failed.\r\n"));
        return OPL_ERROR;
    } 

    OPL_API_DATA_t              stData;
    HAL_PORT_VTT_TAB_t          halPortVttTab;
    HAL_PORT_CFG_INFO_t         halPortCfg;
    HAL_PORT_VLAN_CFG_t         halPortVlanCfg;
    HAL_PORT_RATE_LIMIT_CFG_t   halPortRateLimitCfg;
    HAL_API_FDB_MAC_LEARN_S     stPara;
	HAL_PORT_VLAN_TRUNK_ENTRY_t halVlanTrunkEntry;
    HAL_API_FDB_MAC_LIMIT_S     stMacLimit;
	VoiceVlan_t SysVoiceVlan;

    for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
    {
    	vosMemSet(&portSectionBuff[0], 0, 255);
		vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

		odmPortDefaultVlanGet(portId, &defaultVlan);

		stData.apiId = HAL_API_PORT_PVID_SET;
		stData.length = sizeof(HAL_PORT_CFG_INFO_t);
		halPortCfg.portId = portId;
		halPortCfg.pvid = (UINT16)defaultVlan;
		stData.param = (void *)&halPortCfg;
		retVal = halAppApiCallSync(&stData);
		if(OK != retVal)
		{
			printf("\r\n Fail to set port default tag!\n");
		}

		odmPortAdminGet(portId,&adminStatus);

		odmPortAutoEnableGet(portId, &autoStatus);
		#ifndef ONU_1PORT
		if (TRUE == adminStatus)
		{
			stData.apiId = HAL_API_PORT_ADMIN_SET;
	        stData.length = sizeof(HAL_PORT_CFG_INFO_t);
	        halPortCfg.portId = portId;
	        halPortCfg.admin = adminStatus;
	        stData.param = (void *)&halPortCfg;
	        retVal = halAppApiCallSync(&stData);
			if(OK != retVal)
			{
				printf("\r\n Fail to set port admin state!\n");
			}
		}
        #endif
		
		if (TRUE != autoStatus)
		{
			stData.apiId = HAL_API_PORT_AUTO_ADMIN_SET;
			stData.length = sizeof(HAL_PORT_CFG_INFO_t);
			halPortCfg.portId = portId;
			halPortCfg.autoAdmin = autoStatus;
			stData.param = (void *)&halPortCfg;
			retVal = halAppApiCallSync(&stData);
			if(OK != retVal)
			{
				printf("\r\n Fail to set port auto!\n");
			} 
		}

		odmPortSpeedGet(portId, &portSpeed);
		stData.apiId = HAL_API_PORT_SPEED_SET;
		stData.length = sizeof(HAL_PORT_CFG_INFO_t);
		halPortCfg.portId = portId;
		halPortCfg.speed = portSpeed;
		stData.param = (void *)&halPortCfg;
		retVal = halAppApiCallSync(&stData);
		if(OK != retVal)
		{
			printf("\r\n Fail to set port speed!\n");
		}
		
		odmPortDuplexGet(portId, &duplex);
		stData.apiId = HAL_API_PORT_DUPLEX_SET;
		stData.length = sizeof(HAL_PORT_CFG_INFO_t);
		halPortCfg.portId = portId;
		halPortCfg.duplex = duplex;
		stData.param = (void *)&halPortCfg;
		retVal = halAppApiCallSync(&stData);
		if(OK != retVal)
		{
			printf("\r\n Fail to set port duplex!\n");
		}

		if (TRUE == autoStatus)
		{
			stData.apiId = HAL_API_PORT_AUTO_ADMIN_SET;
			stData.length = sizeof(HAL_PORT_CFG_INFO_t);
			halPortCfg.portId = portId;
			halPortCfg.autoAdmin = autoStatus;
			stData.param = (void *)&halPortCfg;
			retVal = halAppApiCallSync(&stData);
			if(OK != retVal)
			{
				printf("\r\n Fail to set port auto!\n");
			} 
		}

		if (TRUE != adminStatus)
		{
			stData.apiId = HAL_API_PORT_ADMIN_SET;
	        stData.length = sizeof(HAL_PORT_CFG_INFO_t);
	        halPortCfg.portId = portId;
	        halPortCfg.admin = adminStatus;
	        stData.param = (void *)&halPortCfg;
	        retVal = halAppApiCallSync(&stData);
			if(OK != retVal)
			{
				printf("\r\n Fail to set port admin state!\n");
			}
		}

		if(OK == odmPortLearningEnableGet(portId, &learnEnable))
        {
            stPara.ulPortId = portId;
            stPara.ulLearnEnable = learnEnable;
            stData.apiId = HAL_API_FDB_LEARN_EN_SET;
            stData.param = &stPara;
            stData.length = sizeof(stPara);
            halAppApiCallSync(&stData);
        }
		
        odmPortVlanModeGet(portId,&vlanMode);

        stData.apiId = HAL_API_PORT_VLAN_MODE_SET;
        stData.length = sizeof(HAL_PORT_VLAN_CFG_t);
        halPortVlanCfg.portId = portId;
        halPortVlanCfg.vlanMode = vlanMode;
        stData.param = (void *)&halPortVlanCfg;
        retVal = halAppApiCallSync(&stData);
		if(OK != retVal)
		{
			printf("\r\n Fail to set port vlan mode[%d]!\n", vlanMode);
		}

#if defined(ONU_1PORT)
        if (ODM_VLAN_TRANSPARENT != vlanMode)
        {
            vosMemSet(&SysVoiceVlan,0,sizeof(VoiceVlan_t));
        	SysVoiceVlan.VoiceVlanCos=0;
        	SysVoiceVlan.VoipTagValid=1;
        	SysVoiceVlan.VoipPortNum=0;
        	SysVoiceVlan.VoiceVlanId = (UINT16)defaultVlan;
        	syscall(NR_CFG_VOICEVLAN, (char*)&SysVoiceVlan, sizeof(VoiceVlan_t));
        }
#endif

        if (ODM_VLAN_TAG == vlanMode &&
            OK == vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DEFAULT_VID))
        {
        	#if 0
        	defaultVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_DEFAULT_VID,1);
            stData.apiId = HAL_API_PORT_PVID_SET;
            stData.length = sizeof(HAL_PORT_CFG_INFO_t);
            halPortCfg.portId = portId;
            halPortCfg.pvid = defaultVlan;
            stData.param = (void *)&halPortCfg;
            halAppApiCallSync(&stData);
			#endif
        }

        if (ODM_VLAN_TRANSLATION == vlanMode)
        {
            for(vttEntryIndex = 0; vttEntryIndex < ODM_NUM_OF_VTT; vttEntryIndex++)
            {
                vosSprintf(vttEntryOldBuff,CFG_PORT_VTT_OLD,vttEntryIndex);
                vosSprintf(vttEntryNewBuff,CFG_PORT_VTT_NEW,vttEntryIndex);

                if(OK != vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff))
                {
                    continue;
                }

                vttOldVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vttEntryOldBuff,0);
                vttNewVlan = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vttEntryNewBuff,0);

                stData.apiId = HAL_API_PORT_VTT_ENTRY_ADD;
                stData.length = sizeof(HAL_PORT_VTT_TAB_t);
                halPortVttTab.portId = portId;
                halPortVttTab.sTpid = 0;
                halPortVttTab.sVlan = vttNewVlan;
                halPortVttTab.cTpid = 0;
                halPortVttTab.cVlan = vttOldVlan;
                stData.param = (void *)&halPortVttTab;
                halAppApiCallSync(&stData);
            }
        }

		if (ODM_VLAN_TRUNK == vlanMode)
		{
			for(vlanTrunkEntryId = 0; vlanTrunkEntryId < ODM_MAX_NUM_OF_TRUNK_VLAN; vlanTrunkEntryId++)
			{
				vosSprintf((char*)vlanTrunkEntryBuff, CFG_PORT_VLAN_TRUNK_ENTRY, vlanTrunkEntryId);

				if(OK != vosConfigKeyIsExisted(CFG_PORT_CFG, portSectionBuff, (char *)vlanTrunkEntryBuff))
				{
					continue;
				};
		
				vlanTrunkEntryValue = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,(char *)vlanTrunkEntryBuff,0);

				stData.apiId = HAL_API_PORT_VLAN_TRUNK_ENTRY_ADD;
				stData.length = sizeof(HAL_PORT_VLAN_TRUNK_ENTRY_t);
				halVlanTrunkEntry.portId = portId;
				halVlanTrunkEntry.vlanId = vlanTrunkEntryValue;
				stData.param = (void *)&halVlanTrunkEntry;
				
				halAppApiCallSync(&stData);

				if(halVlanTrunkEntry.retVal != NO_ERROR)
				{
					printf("fail to init vlan trunk entry %d\n", vlanTrunkEntryId);
				}
			}
		}

        enableFlag = FALSE;
        if(OK == odmPortFlowCtrolEnableGet(portId, &enableFlag))
        {
            halPortCfg.portId = portId;
            halPortCfg.flowCtrol = enableFlag;
            stData.apiId = HAL_API_PORT_FLOWCONTROL_SET;
            stData.param = &halPortCfg;
            stData.length = sizeof(halPortCfg);
            halAppApiCallSync(&stData);
        }

        enableFlag = FALSE;
        if(OK == odmPortMacLimitNumEnableGet(portId, &enableFlag, &macLimitNum))
        {
        	if(TRUE == enableFlag) /*fix bug3211*/
        	{
                stMacLimit.ulMacNum = macLimitNum;
				stMacLimit.ulEnable = enableFlag;
		        stMacLimit.ulPortId = portId;
		    	stData.apiId = HAL_API_FDB_MAC_NUM_LIMIT_SET;
		    	stData.param = &stMacLimit;
		    	stData.length = sizeof(HAL_API_FDB_MAC_LIMIT_S);
		    	retVal = halAppApiCallSync(&stData);
        	}
        	/*else
        	{
        	 	stMacLimit.ulMacNum = 0;
        	}
            stMacLimit.ulEnable = enableFlag;
            stMacLimit.ulPortId = portId;
        	stData.apiId = HAL_API_FDB_MAC_NUM_LIMIT_SET;
        	stData.param = &stMacLimit;
        	stData.length = sizeof(HAL_API_FDB_MAC_LIMIT_S);
        	retVal = halAppApiCallSync(&stData);*/
        }


		if(OK == odmPortUsPolicingGet(portId,&usPolicingEn,&usCir,&usCbs,&usEbs))
		{
#if 0
			dalPortUsPolicingSet(portId,usPolicingEn,usCir,usCbs,usEbs);
#else
            stData.apiId = HAL_API_PORT_USPOLICING_SET;
            stData.length = sizeof(HAL_PORT_RATE_LIMIT_CFG_t);
            halPortRateLimitCfg.portId = portId;
            halPortRateLimitCfg.ingressRateLimitEnable = usPolicingEn;
            halPortRateLimitCfg.ingressCir = usCir;
            halPortRateLimitCfg.ingressCbs = usCbs;
            halPortRateLimitCfg.ingressEbs = usEbs;
            stData.param = (void *)&halPortRateLimitCfg;
            halAppApiCallSync(&stData);
#endif /* #if 0 */
		}

		if(OK == odmPortDsPolicingGet(portId,&dsRateLimitEn,&dsCir,&dsPir))
		{
#if 0
			dalPortDsPolicingSet(portId,dsRateLimitEn,dsCir,dsPir);
#else
            stData.apiId = HAL_API_PORT_DSPOLICING_SET;
            stData.length = sizeof(HAL_PORT_RATE_LIMIT_CFG_t);
            halPortRateLimitCfg.portId = portId;
            halPortRateLimitCfg.egressRateLimitEnable = dsRateLimitEn;
            halPortRateLimitCfg.egressCir = dsCir;
            halPortRateLimitCfg.egressCbs = dsPir;
            stData.param = (void *)&halPortRateLimitCfg;
            halAppApiCallSync(&stData);
#endif /* #if 0 */
		}
    }
    return OK;
}

UINT32 odmPortVlanTrunkEntryAdd(UINT32 portId, UINT32 vlanId)
{
	UINT32 retVal = OK;
	UINT8  portSectionBuff[255] = {0};
	UINT8  vlanTrunkEntryBuff[255] = {0};
	UINT32 portVlanMode;
	UINT32 vlanTrunkNum;
    OPL_API_DATA_t stData;
	HAL_PORT_VLAN_TRUNK_ENTRY_t halVlanTrunkEntry;
	UINT32 portNum;
	UINT32 vlanMode;
	UINT32 vlanTrunkEntryId;
	UINT32 vlanTrunkEntryValue;
    UINT32 firstEmptyEntryId = 0xffff;
	UINT32 vlanTrunkEntryNum;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: portId = %d, vlanId = %llx\n", __FUNCTION__, portId, vlanId);

	if(odmPortRangeCheck(portId))
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call odmPortRangeCheck fail!\n", __FUNCTION__);
		return PORT_INVALID;
	}

	vlanId = vlanId & 0x0000FFFF;
    if(odmVlanRangeCheck(vlanId))
    {
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call odmVlanRangeCheck fail!\n", __FUNCTION__);
        return ERR_INVALID_PARAMETERS;
    }
	
	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

    portVlanMode = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_MODE,ODM_VLAN_TRANSPARENT);

    if(ODM_VLAN_TRUNK != portVlanMode)
    {
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: ODM_VLAN_TRUNK != portVlanMode\n", __FUNCTION__);
        return PORT_NOT_AT_VLAN_TRUNK_MODE;
    }

	retVal = odmPortVlanTrunkEntryNumGet(portId, &vlanTrunkNum);
	if(retVal != OK)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: odmPortVlanTrunkEntryNumGet fail!\n", __FUNCTION__);
		return retVal;
	}

	if(vlanTrunkNum == ODM_MAX_NUM_OF_TRUNK_VLAN)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: No enough vlan trunk resource!\n", __FUNCTION__);
		return PORT_NO_VLAN_TRUNK_RESOURCE;
	}

	
	/* call hal */
    stData.apiId = HAL_API_PORT_VLAN_TRUNK_ENTRY_ADD;
    stData.length = sizeof(HAL_PORT_VLAN_TRUNK_ENTRY_t);
    halVlanTrunkEntry.portId = portId;
	halVlanTrunkEntry.vlanId = vlanId;
    stData.param = (void *)&halVlanTrunkEntry;

	retVal = halAppApiCallSync(&stData);

	if(retVal != NO_ERROR)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: call halAppApiCallSync fail!\n", __FUNCTION__);
		return retVal;
	}

    if(NO_ERROR != halVlanTrunkEntry.retVal)
    {
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: NO_ERROR != halVlanTrunkEntry.retVal!\n", __FUNCTION__);
        return halVlanTrunkEntry.retVal;
    }

	/* add vlan trunk entry to config file */
	for(vlanTrunkEntryId = 0; vlanTrunkEntryId < ODM_MAX_NUM_OF_TRUNK_VLAN; vlanTrunkEntryId++)
	{
		vosSprintf(vlanTrunkEntryBuff,CFG_PORT_VLAN_TRUNK_ENTRY,vlanTrunkEntryId);

	    retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,vlanTrunkEntryBuff);
		if(OK != retVal)
		{
    		if(0xffff == firstEmptyEntryId)
    		{
        		firstEmptyEntryId = vlanTrunkEntryId;
    		}
    		continue;
		}

		vlanTrunkEntryValue = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vlanTrunkEntryBuff,0);

		if(vlanTrunkEntryValue == vlanId)
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: VTT_ENTRY_ISEXISTED!\n", __FUNCTION__);
		    return VTT_ENTRY_ISEXISTED;
		}
    }
	
    if(0xffff == firstEmptyEntryId)
    {
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: VTT_ENTRY_FULL!\n", __FUNCTION__);
        return VTT_ENTRY_FULL;
    }

	vosSprintf(vlanTrunkEntryBuff, CFG_PORT_VLAN_TRUNK_ENTRY,firstEmptyEntryId);

    vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,vlanTrunkEntryBuff,vlanId);

	vlanTrunkEntryNum = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_TRUNK_NUM,0);

	vlanTrunkEntryNum += 1;

	vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_TRUNK_NUM,vlanTrunkEntryNum);	
	
	return OK;
};

UINT32 odmPortVlanTrunkEntryDel(UINT32 portId, UINT32 vlanId)
{
	UINT32 retVal = OK;
	UINT8  portSectionBuff[255] = {0};
	UINT8  vlanTrunkEntryBuff[255] = {0};
	UINT32 portVlanMode;
    OPL_API_DATA_t stData;
	HAL_PORT_VLAN_TRUNK_ENTRY_t halVlanTrunkEntry;
	UINT32 vlanTrunkEntryId;
	UINT32 vlanTrunkEntryValue;
	UINT32 vlanTrunkEntryNum;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: portId = %d, vlanId = %d\n", __FUNCTION__, portId, vlanId);

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

    if(odmVlanRangeCheck(vlanId))
    {
        return ERR_INVALID_PARAMETERS;
    }

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

    portVlanMode = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_MODE,ODM_VLAN_TRANSPARENT);

    if(ODM_VLAN_TRUNK != portVlanMode)
    {
        return PORT_NOT_AT_VLAN_TRUNK_MODE;
    }	

	/* call hal */
    stData.apiId = HAL_API_PORT_VLAN_TRUNK_ENTRY_DEL;
    stData.length = sizeof(HAL_PORT_VLAN_TRUNK_ENTRY_t);
	vosMemSet((UINT8 *)&halVlanTrunkEntry, 0, sizeof(halVlanTrunkEntry));
    halVlanTrunkEntry.portId = portId;
	halVlanTrunkEntry.vlanId = vlanId;
    stData.param = (void *)&halVlanTrunkEntry;
    retVal = halAppApiCallSync(&stData);

	if(retVal != NO_ERROR)
	{
		return retVal;
	}

    if(NO_ERROR != halVlanTrunkEntry.retVal)
    {
    	OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"[ %s ]: retVal = %d\n", __FUNCTION__, halVlanTrunkEntry.retVal);
        return halVlanTrunkEntry.retVal;
    }
	
	/* delete vlan trunk entry from config file */
	for(vlanTrunkEntryId = 0; vlanTrunkEntryId < ODM_MAX_NUM_OF_TRUNK_VLAN; vlanTrunkEntryId++)
	{
		vosSprintf(vlanTrunkEntryBuff,CFG_PORT_VLAN_TRUNK_ENTRY,vlanTrunkEntryId);

		retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,vlanTrunkEntryBuff);
		if(OK != retVal)
		{
			continue;
		}

		vlanTrunkEntryValue = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vlanTrunkEntryBuff,0);

		if(vlanTrunkEntryValue == vlanId)
		{
			vosConfigKeyDelete(CFG_PORT_CFG,portSectionBuff,vlanTrunkEntryBuff);

			vlanTrunkEntryNum = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_TRUNK_NUM,0);

			vlanTrunkEntryNum -= 1;

			vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_TRUNK_NUM,vlanTrunkEntryNum);

			return OK;
		}
	}

	return PORT_VLAN_TRUNK_ENTRY_NOT_EXIST;
};

UINT32 odmPortVlanTrunkEntryValueGet(UINT32 portId, UINT32 entryIndex, UINT32 *pVlanId)
{
	UINT32 retVal = OK;
	UINT8  portSectionBuff[255] = {0};
	UINT8  vlanTrunkEntryBuff[255] = {0};	
	UINT32 portVlanMode;

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	portVlanMode = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_MODE,ODM_VLAN_TRANSPARENT);
	if(ODM_VLAN_TRUNK != portVlanMode)
	{
		return PORT_NOT_AT_VLAN_TRUNK_MODE;
	}

	vosSprintf(vlanTrunkEntryBuff,CFG_PORT_VLAN_TRUNK_ENTRY,entryIndex);

	retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,vlanTrunkEntryBuff);
	if(OK != retVal)
	{
		return PORT_VLAN_TRUNK_ENTRY_NOT_EXIST;
	}

	*pVlanId = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,vlanTrunkEntryBuff,0);

	return OK;
};

UINT32 odmPortVlanTrunkEntryNumGet(UINT32 portId, UINT32 *pEntryNum)
{
	UINT32 retVal = OK;
	UINT8  portSectionBuff[255] = {0};
	UINT8  vlanTrunkEntryBuff[255] = {0};	
	UINT32 portVlanMode;
	UINT32 vlanTrunkNum;

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

	portVlanMode = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_MODE,ODM_VLAN_TRANSPARENT);
	if(ODM_VLAN_TRUNK != portVlanMode)
	{
		return PORT_NOT_AT_VLAN_TRUNK_MODE;
	}

	*pEntryNum = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_TRUNK_NUM,0);

	return OK;
};

UINT32 odmPortVlanTrunkEntryClear(UINT32 portId)
{
	UINT32 retVal = OK;
	UINT32 portVlanMode;
	UINT8  portSectionBuff[255] = {0};
	UINT8  vlanTrunkEntryBuff[255] = {0};
    OPL_API_DATA_t stData;
	HAL_PORT_VLAN_CFG_t halVlanCfg;
	UINT32 vlanTrunkEntryNum;
	UINT32 vlanTrunkEntryId;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "enter odmPortVlanTrunkEntryClear(1)\n");

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

    portVlanMode = vosConfigUInt32Get(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_MODE,ODM_VLAN_TRANSPARENT);

    if(ODM_VLAN_TRUNK != portVlanMode)
    {
        return PORT_NOT_AT_VLAN_TRUNK_MODE;
    }	

	/* call hal */
    stData.apiId = HAL_API_PORT_VLAN_TRUNK_ENTRY_CLEAR;
    stData.length = sizeof(HAL_PORT_VLAN_CFG_t);
	vosMemSet((UINT8 *)&halVlanCfg, 0, sizeof(halVlanCfg));
	halVlanCfg.portId = portId;
    stData.param = (void *)&halVlanCfg;
    retVal = halAppApiCallSync(&stData);

	if(retVal != NO_ERROR)
	{
		return retVal;
	}

    if(OK != halVlanCfg.retVal)
    {
        return halVlanCfg.retVal;
    }

	/* clear vlan trunk entries in config file */
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"[ %s ]: clear vlan trunk entriesin config file", __FUNCTION__);
	for(vlanTrunkEntryId = 0; vlanTrunkEntryId < ODM_MAX_NUM_OF_TRUNK_VLAN; vlanTrunkEntryId++)
	{
		vosSprintf(vlanTrunkEntryBuff, CFG_PORT_VLAN_TRUNK_ENTRY,vlanTrunkEntryId);

		retVal = vosConfigKeyIsExisted(CFG_PORT_CFG,portSectionBuff,vlanTrunkEntryBuff);

		if(retVal == OK)
		{
			vosConfigKeyDelete(CFG_PORT_CFG,portSectionBuff,vlanTrunkEntryBuff);
		}	
	}

	vosConfigUInt32Set(CFG_PORT_CFG,portSectionBuff,CFG_PORT_VLAN_TRUNK_NUM,0);

	return OK;
};

UINT32 odmPortCtcStackConfigGet(CTC_STACK_config_t * ctcStackConfig)
{
	UINT32 portId;
	UINT32 portIndex;
    UINT32 linkState;
	UINT32 powerDown;
	UINT32 pause;
	UINT32 speedDuplex;
	UINT32 autoNeg;
	UINT8  priority;
	UINT32 pvid;
	UINT32 usEnable;	
	UINT32 ingressRate;
	UINT32 dsEnable;	
	UINT32 engressRate;	
	UINT8 isolate;
	UINT32 vlanMode;
	UINT8 mulcastTrip;
	UINT8 mulcastMaxNum;
	
	UINT32 adminStatus;
	UINT32 portSpeed;
	UINT32 duplex;

	int ret = 0;
	
    for( portId = 1; portId <= NUMBER_OF_PORTS; portId++)
  	{
       ret += odmPortLinkStateGet(portId, &linkState);	   
       ret += odmPortAdminGet(portId, &powerDown);
       ret += odmPortFlowCtrolEnableGet(portId, &pause);
       ret += odmPortAutoEnableGet(portId, &adminStatus);
       ret += odmPortSpeedGet(portId, &portSpeed);
       ret += odmPortDuplexGet(portId, &duplex);
       if (1 == adminStatus)
       {
         speedDuplex = 0;
       }
       else if ((portSpeed==ODM_PORT_SPEED_10M) && (duplex==ODM_PORT_DUPLEX_HALF))
       {
         speedDuplex = 1;
       }
       else if ((portSpeed==ODM_PORT_SPEED_10M) && (duplex==ODM_PORT_DUPLEX_FULL))
       {
         speedDuplex = 2;
       }
       else if ((portSpeed==ODM_PORT_SPEED_100M) && (duplex==ODM_PORT_DUPLEX_HALF))
       {
         speedDuplex = 3;
       }
       else if ((portSpeed==ODM_PORT_SPEED_100M) && (duplex==ODM_PORT_DUPLEX_FULL))
       {
         speedDuplex = 4;
       }

	   ret += odmPortVlanModeGet(portId, &vlanMode);
       ret += odmPortDefaultVlanGet(portId, &pvid);
	   ret += odmPortUsPolicingEnableGet(portId,&usEnable);
       ret += odmPortUsCirGet(portId,&ingressRate);
	   ret += odmMulticastTagstripGet(portId, &mulcastTrip);
	   mulcastMaxNum = odmMulticastGroupMaxNumGet(portId);
	   ret += odmPortDsRateLimitEnableGet(portId,&dsEnable);
       ret += odmPortDsCirGet(portId,&engressRate);	   

	   if (OK != ret)
       {
			return ret;
	   }

	   if(usEnable == 0)
	   {
			ingressRate = 0;
	   }

	   if(dsEnable == 0)
	   {
			engressRate = 0;
	   }	   

	   portIndex = portId - 1;
	   ctcStackConfig->linkstate[portIndex] = linkState;   
	   ctcStackConfig->PhyPrtMode[portIndex] = speedDuplex;	   	   	   
	   ctcStackConfig->PhyAdmin[portIndex] = powerDown;	   
	   ctcStackConfig->PhyPause[portIndex] = pause;	   	   
	   ctcStackConfig->eth_policing[portIndex].operation = usEnable;	   
	   ctcStackConfig->eth_policing[portIndex].cir = ingressRate;	   	   
	   ctcStackConfig->eth_Ratelimit[portIndex].operation = dsEnable;	   
	   ctcStackConfig->eth_Ratelimit[portIndex].cir = engressRate;	
	   ctcStackConfig->VlanMode[portIndex] = vlanMode;	   	   
	   ctcStackConfig->uc_vlan[portIndex] = pvid;	   	   
	   ctcStackConfig->mc_strip[portIndex] = mulcastTrip;
	   ctcStackConfig->mc_max_count[portIndex] = mulcastMaxNum;
	}

   ctcStackConfig->mc_mode = odmMulticastSwitchGet();
   ctcStackConfig->mc_fast_leave = odmMulticastFastleaveAdminStateGet() ? 0 : 1;
   return OK;
}

UINT32 odmLoopDetectionSet(UINT32 portId, UINT32 uiEnable)
{
    UINT32  retVal = OK;
	UINT8 	portSectionBuff[255] = {0};

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

    if(TRUE != uiEnable && FALSE != uiEnable)
	{
		return ERR_INVALID_PARAMETERS;
	}
#ifndef ONU_1PORT	
    bOnePortLoopDetectEn[portId-1] = uiEnable;
    bOnePortLoopWhile[portId-1] = 60;
    bOnePortLooptimer[portId-1] = 60;
#endif	 
	retVal = vosConfigUInt32Set(CFG_PORT_CFG, portSectionBuff,
        CFG_LOOP_DECTION_EN, uiEnable);

    return retVal;
}

UINT32 odmLoopDetectionGet(UINT32 portId, UINT32 *puiEnable)
{
    UINT8 	portSectionBuff[255] = {0};

    if (NULL == puiEnable)
    {
        return ERR_NULL_POINTER;
    }

    vosSprintf(portSectionBuff, CFG_PORT_SECTION, portId);
    *puiEnable = vosConfigUInt32Get(CFG_PORT_CFG,
        portSectionBuff,
        CFG_LOOP_DECTION_EN,
        TRUE);

    return OK;
}

UINT32 odmLoopDetectionHoldDownTimeSet(UINT32 portId, UINT32 uiTime)
{
	UINT32  retVal = OK;
    UINT8 	portSectionBuff[255] = {0};

    if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

#ifndef ONU_1PORT	
    bOnePortLoopWhile[portId-1] = uiTime;
    bOnePortLooptimer[portId-1] = uiTime;
#endif	 

	vosConfigUInt32Set(CFG_PORT_CFG, portSectionBuff,
        CFG_LOOP_DECTION_HOLD_DOWN_TIME, uiTime);

	return OK;
}

UINT32 odmLoopDetectionHoldDownTimeGet(UINT32 portId, UINT32 *puiTime)
{
	UINT32  retVal = OK;
    UINT8 	portSectionBuff[255] = {0};

    if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

    if (NULL == puiTime)
    {
        return ERR_NULL_POINTER;
    }

    vosSprintf(portSectionBuff,CFG_PORT_SECTION,portId);

    *puiTime = vosConfigUInt32Get(CFG_PORT_CFG,
        portSectionBuff,
        CFG_LOOP_DECTION_HOLD_DOWN_TIME,
        60);

	return OK;

}

UINT32 odmPortMirrorCfgInit()
{
    UINT8  count;
    UINT8  roop;
    UINT8  portId;
    UINT8  enable;
    UINT8  type;
    char   *session = NULL;
	
    count = vosConfigSectionCount(CFG_MIRROR_CFG);
    for (roop = 0;roop < count;roop++)
    {
        if (vosConfigSectionGetByIndex(CFG_MIRROR_CFG,roop,&session) == 0)
        {
            for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
            {
                enable = 0;
                if (odmMirrorPortGet(session, portId, &enable) == OK)
                {
	             if (enable == 1)
	             {
	                  type = 0xff;
	                  if (odmMirrorTypeGet(session, &type) == OK)
	                      odmMirrorPortSet(session, portId, type, enable);
	             }
                }
		  enable = 0;
		  if (odmMonitorPortGet(session, portId, &enable) == OK)
		  {
		       if (enable == 1)
		       {
		           odmMonitorPortSet(session, portId, enable);
		       }
		  }
            }
        }
    }
	
}

UINT32 odmMirrorGroupNameSet(char *name)
{
    UINT32 mode;	
    UINT8   count;
	
    if (name == NULL)
    {
        return PORT_MIRROR_INVALID_POINTER;
    }
    PORT_API_LOCK;
    if (odmCheckMirrorGroupExist(name) != NO_ERROR)
    {
        count = vosConfigSectionCount(CFG_MIRROR_CFG);
        if (count >= 1)
        {
            PORT_API_UNLOCK;
	        return PORT_MIRROR_EXIST_CHECK_ERROR;    
        }
	 else
	 {
	     vosConfigSectionCreate(CFG_MIRROR_CFG,name);
	 }
    }
    PORT_API_UNLOCK;
    return NO_ERROR;
}

UINT32 odmMirrorPortSet(char *name, UINT8 portid, UINT8 type, UINT8 enable)
{
    UINT8 retVal = OK;
    OPL_API_DATA_t stData;
    HAL_PORT_MIRROR_INGRESS_CFG_t halMirrorIngCfg;
    HAL_PORT_MIRROR_ENGRESS_CFG_t halMirrorEngCfg;
    UINT8  portKeyBuff[255] = {0};
	
    if(odmPortRangeCheck(portid))
    {
        return PORT_INVALID;
    } 

    /* cfg */
    PORT_API_LOCK;
    if (name == NULL)
    {
        PORT_API_UNLOCK;
        return PORT_MIRROR_INVALID_POINTER;
    }
    if (odmCheckMirrorGroupExist(name) != NO_ERROR)
    {
        PORT_API_UNLOCK;
        return PORT_MIRROR_EXIST_CHECK_ERROR;
    }
    vosMemSet(&portKeyBuff[0], 0, 255);
    vosSprintf(portKeyBuff,CFG_MIRROR_MIRROR_PORT,portid);
    vosConfigUInt32Set(CFG_MIRROR_CFG,name,portKeyBuff,enable);
    if ((type == ODM_MIRROR_BOTH) || (type == ODM_MIRROR_INGRESS))  
        vosConfigUInt32Set(CFG_MIRROR_CFG,name,CFG_MIRROR_INGRESS,enable);
    if ((type == ODM_MIRROR_BOTH) || (type == ODM_MIRROR_EGRESS))  
        vosConfigUInt32Set(CFG_MIRROR_CFG,name,CFG_MIRROR_EGRESS,enable);
    PORT_API_UNLOCK;   

    /* call hal */
    if ((type == ODM_MIRROR_BOTH) || (type == ODM_MIRROR_INGRESS))
    {
        stData.apiId = HAL_API_PORT_MIRROR_INGRESS_STATE_SET;
        stData.length = sizeof(HAL_PORT_MIRROR_INGRESS_CFG_t);
        vosMemSet((UINT8 *)&halMirrorIngCfg, 0, sizeof(halMirrorIngCfg));
        halMirrorIngCfg.portId = portid;
        halMirrorIngCfg.state = enable;
        stData.param = (void *)&halMirrorIngCfg;
        retVal = halAppApiCallSync(&stData);
    }

    if ((type == ODM_MIRROR_BOTH) || (type == ODM_MIRROR_EGRESS))
    {
        stData.apiId = HAL_API_PORT_MIRROR_ENGRESS_STATE_SET;
        stData.length = sizeof(HAL_PORT_MIRROR_ENGRESS_CFG_t);
        vosMemSet((UINT8 *)&halMirrorEngCfg, 0, sizeof(halMirrorEngCfg));
        halMirrorEngCfg.portId = portid;
        halMirrorEngCfg.state = enable;
        stData.param = (void *)&halMirrorEngCfg;
        retVal = halAppApiCallSync(&stData);
    }

    return retVal;
}

UINT32 odmMirrorPortGet(char *name, UINT8 portid, UINT8 *enable)
{
    UINT8 retVal = OK;
    UINT8   portKeyBuff[255] = {0};
	
    if(odmPortRangeCheck(portid))
    {
        return PORT_INVALID;
    } 
	
    PORT_API_LOCK;
    if (name == NULL)
    {   
        PORT_API_UNLOCK;
        return PORT_MIRROR_INVALID_POINTER;
    }
    if (odmCheckMirrorGroupExist(name) != NO_ERROR)
    {   
        PORT_API_UNLOCK;
        return PORT_MIRROR_EXIST_CHECK_ERROR;
    }
    vosMemSet(&portKeyBuff[0], 0, 255);
    vosSprintf(portKeyBuff,CFG_MIRROR_MIRROR_PORT,portid); 
    *enable = vosConfigUInt32Get(CFG_MIRROR_CFG,name,portKeyBuff,0);
    PORT_API_UNLOCK;
    
    return retVal;
}

UINT32 odmMonitorPortSet(char *name, UINT8 portid, UINT8 enable)
{
    UINT8 retVal = OK;
    OPL_API_DATA_t stData;
    HAL_PORT_MIRROR_PORT_CFG_t halMonitorPortCfg;
    UINT8  portKeyBuff[255] = {0};

    if(odmPortRangeCheck(portid))
    {
        return PORT_INVALID;
    } 

    /* cfg */
    PORT_API_LOCK;
    if (name == NULL)
    {
        PORT_API_UNLOCK;
        return PORT_MIRROR_INVALID_POINTER;
    }
    if (odmCheckMirrorGroupExist(name) != NO_ERROR)
    {
        PORT_API_UNLOCK;
        return PORT_MIRROR_EXIST_CHECK_ERROR;
    }
    vosMemSet(&portKeyBuff[0], 0, 255);
    vosSprintf(portKeyBuff,CFG_MIRROR_MONITOR_PORT,portid);
    vosConfigUInt32Set(CFG_MIRROR_CFG,name,portKeyBuff,enable);
    PORT_API_UNLOCK;   

    /* call hal */
    stData.apiId = HAL_API_PORT_MIRROR_PORT_SET;
    stData.length = sizeof(HAL_PORT_MIRROR_INGRESS_CFG_t);
    vosMemSet((UINT8 *)&halMonitorPortCfg, 0, sizeof(halMonitorPortCfg));
    if (enable == 0)
    {  
        halMonitorPortCfg.portId = 0xF;
    }
    else
    {  
        halMonitorPortCfg.portId = portid;
    }
    stData.param = (void *)&halMonitorPortCfg;
    retVal = halAppApiCallSync(&stData);

    return retVal;
}

UINT32 odmMonitorPortGet(char *name, UINT8 portid, UINT8 *enable)
{
    UINT8   retVal = OK;
    UINT8   portKeyBuff[255] = {0};

    if(odmPortRangeCheck(portid))
    {
        return PORT_INVALID;
    } 

    PORT_API_LOCK;
    if (name == NULL)
    {
        PORT_API_UNLOCK;
        return PORT_MIRROR_INVALID_POINTER;
    }
    if (odmCheckMirrorGroupExist(name) != NO_ERROR)
    {
        PORT_API_UNLOCK;
        return PORT_MIRROR_EXIST_CHECK_ERROR;
    }
    vosMemSet(&portKeyBuff[0], 0, 255);
    vosSprintf(portKeyBuff,CFG_MIRROR_MONITOR_PORT,portid); 
    *enable = vosConfigUInt32Get(CFG_MIRROR_CFG,name,portKeyBuff,0);
    PORT_API_UNLOCK;
    
    return retVal;
}

UINT32 odmMirrorTypeGet(char *name, UINT8 *type)
{
    UINT8   retVal = OK;
    UINT8   portKeyBuff[255] = {0};
    UINT8   ingress, egress;
    

    PORT_API_LOCK;
    if (name == NULL)
    {
        PORT_API_UNLOCK;
        return PORT_MIRROR_INVALID_POINTER;
    }
    if (odmCheckMirrorGroupExist(name) != NO_ERROR)
    {
        PORT_API_UNLOCK;
        return PORT_MIRROR_EXIST_CHECK_ERROR;
    }

    *type = ODM_INVALID_MIRROR_TYPE;
    ingress = vosConfigUInt32Get(CFG_MIRROR_CFG,name,CFG_MIRROR_INGRESS,0);
    egress = vosConfigUInt32Get(CFG_MIRROR_CFG,name,CFG_MIRROR_EGRESS,0);
    if (ingress == 1)       *type = ODM_MIRROR_INGRESS;
    if (egress == 1)        *type = ODM_MIRROR_EGRESS;
    if ((ingress == 1) && (egress == 1))       *type = ODM_MIRROR_BOTH;
    PORT_API_UNLOCK;
    
    return retVal;
}

UINT32 odmMirrorGroupClear(char *name)
{
    UINT8   retVal = OK;
    UINT8   portnum;
    UINT8   portKeyBuff[255] = {0};
    UINT8   type;
    UINT32 mirrorEnable, monitorEnable,ingressEnable,egressEnable;

    for(portnum = ODM_START_PORT_NUN; portnum <= ODM_NUM_OF_PORTS; portnum++)
    {
        ingressEnable = 0;
	 egressEnable = 0;
        mirrorEnable = 0;
	 monitorEnable = 0;
        vosMemSet(&portKeyBuff[0], 0, 255);
        vosSprintf(portKeyBuff,CFG_MIRROR_MIRROR_PORT,portnum); 
	 mirrorEnable = vosConfigUInt32Get(CFG_MIRROR_CFG,name,portKeyBuff,0);
	 if (mirrorEnable == 1)
	 {
	     ingressEnable = vosConfigUInt32Get(CFG_MIRROR_CFG,name,CFG_MIRROR_INGRESS,0);
	     egressEnable = vosConfigUInt32Get(CFG_MIRROR_CFG,name,CFG_MIRROR_EGRESS,0);
	     if (ingressEnable == 1)       type = ODM_MIRROR_INGRESS;
	     if (egressEnable == 1)        type = ODM_MIRROR_EGRESS;
	     if ((ingressEnable == 1) && (egressEnable == 1))       type = ODM_MIRROR_BOTH;
	     odmMirrorPortSet(name, portnum, type, 0);
	 }
	 vosMemSet(&portKeyBuff[0], 0, 255);
        vosSprintf(portKeyBuff,CFG_MIRROR_MONITOR_PORT,portnum); 
	 monitorEnable = vosConfigUInt32Get(CFG_MIRROR_CFG,name,portKeyBuff,0);
	 if (monitorEnable == 1)
	 {  
	     odmMonitorPortSet(name, portnum, 0);
	 }
    }
	
    /* delete the port mirror  from mirror configure file */
    PORT_API_LOCK;	
    retVal = vosConfigSectionDelete(CFG_MIRROR_CFG, name);
    PORT_API_UNLOCK;

    return  retVal;
}

UINT32 odmMirrorGroupCfgDel(char *name)
{
    UINT8   retVal = OK;
    UINT8   portnum;
    UINT8   portKeyBuff[255] = {0};
    UINT8   type;
    UINT32 mirrorEnable, monitorEnable,ingressEnable,egressEnable;

    for(portnum = ODM_START_PORT_NUN; portnum <= ODM_NUM_OF_PORTS; portnum++)
    {
        ingressEnable = 0;
	 egressEnable = 0;
        mirrorEnable = 0;
	 monitorEnable = 0;
        vosMemSet(&portKeyBuff[0], 0, 255);
        vosSprintf(portKeyBuff,CFG_MIRROR_MIRROR_PORT,portnum); 
	 mirrorEnable = vosConfigUInt32Get(CFG_MIRROR_CFG,name,portKeyBuff,0);
	 if (mirrorEnable == 1)
	 {
	     ingressEnable = vosConfigUInt32Get(CFG_MIRROR_CFG,name,CFG_MIRROR_INGRESS,0);
	     egressEnable = vosConfigUInt32Get(CFG_MIRROR_CFG,name,CFG_MIRROR_EGRESS,0);
	     if (ingressEnable == 1)       type = ODM_MIRROR_INGRESS;
	     if (egressEnable == 1)        type = ODM_MIRROR_EGRESS;
	     if ((ingressEnable == 1) && (egressEnable == 1))       type = ODM_MIRROR_BOTH;
	     odmMirrorPortSet(name, portnum, type, 0);
	 }
	 vosMemSet(&portKeyBuff[0], 0, 255);
        vosSprintf(portKeyBuff,CFG_MIRROR_MONITOR_PORT,portnum); 
	 monitorEnable = vosConfigUInt32Get(CFG_MIRROR_CFG,name,portKeyBuff,0);
	 if (monitorEnable == 1)
	 {  
	     odmMonitorPortSet(name, portnum, 0);
	 }
    }

    return retVal;
}

int odmCheckMirrorGroupExist
(
    char *name
)
{
    int status;
    
    if (name == NULL)
    {
        return PORT_MIRROR_INVALID_POINTER;
    }
    return (vosConfigSectionIsExisted(CFG_MIRROR_CFG,name));    
}

void odmShowMirrorList
(
    int fd
)
{
    UINT8  count;
    UINT8  roop;
    UINT16 ruleId ;
   UINT16 Rulecnt=0;
    UINT8  portnum;
    UINT8  val;
    char   *session = NULL;
    
    count = vosConfigSectionCount(CFG_MIRROR_CFG);
    for (roop = 0;roop < count;roop++)
    {
        if (vosConfigSectionGetByIndex(CFG_MIRROR_CFG,roop,&session) == 0)
        {
             vosPrintf(fd,"Mirror-Group-Name: %s\r\n", session);
	      vosPrintf(fd,"Mirror-port: ");
	      val = 0;
	      for(portnum = ODM_START_PORT_NUN; portnum <= ODM_NUM_OF_PORTS; portnum++)
	      {
	          odmMirrorPortGet(session, portnum, &val);
	          if (val == 1)
		       vosPrintf(fd,"    %d", portnum);
	      }
	      vosPrintf(fd,"\nMonitor-port: ");
	      val = 0;
	      for(portnum = ODM_START_PORT_NUN; portnum <= ODM_NUM_OF_PORTS; portnum++)
	      {
	          odmMonitorPortGet(session, portnum, &val);
	          if (val == 1)
		       vosPrintf(fd,"    %d", portnum);
	      }
	      
             val = ODM_INVALID_MIRROR_TYPE;
	      odmMirrorTypeGet(session, &val);
	      if (val == ODM_MIRROR_BOTH)
	          vosPrintf(fd,"\nType: Both\r\n");
	      if (val == ODM_MIRROR_INGRESS)
	          vosPrintf(fd,"\nType: Ingress\r\n");
	      if (val == ODM_MIRROR_EGRESS)
	          vosPrintf(fd,"\nType: Egress\r\n");
             vosPrintf(fd,"\r\n");

        }
    }
}

