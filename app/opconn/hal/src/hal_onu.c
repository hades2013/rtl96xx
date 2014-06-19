
/*
=============================================================================
     File Name: hal_onu.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu	   2009/03/17		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#include "hal.h"
#include "opl_driver.h"
#include <vos.h>
#include "errors.h"
#include "hal_onu.h"

/*******************************************************************************
* halOnuInit
*
* DESCRIPTION:
*  		this function is used to init the default pon config ,such as serdes delay, laser on and of time count
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuInit(void *pInput)
{
    HAL_ONU_DEBUG_TRACE();
    dalPonInit();
    return ;
}
/*******************************************************************************
* halOnuMacIdSet
*
* DESCRIPTION:
*  		this function is used to set the onu macld, used for mpcp register and oam pkt.
*
*	INPUTS:
*		mac : 
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuMacIdSet(void *pInput)
{
    HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonMacIdSet(p->ponMac);

    return ;
}

/*******************************************************************************
* halOnuMacIdGet
*
* DESCRIPTION:
*  		this function is used get the llid mac address used for mpcp register.
*
*	INPUTS:
*		mac:
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuMacIdGet(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;
    UINT8 macAddr[6] = {0x0};
    OPL_RESULT halStatus = 0;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }
    
	halStatus = dalPonMacIdGet(macAddr);

    if(OPL_OK == halStatus)
    {
        vosMemCpy(p->ponMac,macAddr,6);
    }

    return ;
}

/*******************************************************************************
* halOnuPhyFecModeSet
*
* DESCRIPTION:
*  		this function is used to set the fec mode. 0x01 for unknown,0x02 for eanble,0x03 for disable.
*
*	INPUTS:
*		portId: 		portId
*		mode:		0 disable, 1 enable
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuPhyFecModeSet(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonPhyFecModeSet(p->fecStatus);

    return ;
}
/*******************************************************************************
* halOnuPhyFecModeGet
*
* DESCRIPTION:
*  		this function is used to get the fec mode info.0x01 for unknown,0x02 for eanble,0x03 for disable.
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		mode:		0x01:unknown,0x02:enable,0x03:disable
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuPhyFecModeGet(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;
    UINT32 fecStatus = 0;
    OPL_RESULT halStatus = 0;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPonPhyFecModeGet(&fecStatus);

    if(OPL_OK == halStatus)
    {
        p->fecStatus = fecStatus;
    }

    return;
}

/*******************************************************************************
* halOnuLaserOnCntSet
*
* DESCRIPTION:
*  		this function is used to set the laser on count
*
*	INPUTS:
*		count: num of tq laser on used.
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuLaserOnCntSet(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonLaserOnCntSet(p->laserOnTime);

    return;
}

/*******************************************************************************
* halOnuLaserOnCntSet
*
* DESCRIPTION:
*  		this function is used to get the laser on count
*
*	INPUTS:
*		count: buff used to store the value of tq used for laser on
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuLaserOnCntGet(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;
    UINT32 laserOnCnt = 0;
    OPL_RESULT halStatus = 0;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPonLaserOnCntGet(&laserOnCnt);

    if(OPL_OK == halStatus)
    {
        p->laserOnTime = laserOnCnt;
    }

    return;
}
/*******************************************************************************
* halOnuLaserOffCntSet
*
* DESCRIPTION:
*  		this function is used to set the laser off count
*       
*	INPUTS:
*		this function is used to set the laser off count
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuLaserOffCntSet(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonLaserOffCntSet(p->laserOffTime);

    return;
}

/*******************************************************************************
* halOnuLaserOnCntSet
*
* DESCRIPTION:
*  		this function is used to set the laser off count
*
*	INPUTS:
*		count: buff used to store the value of tq used for laser off
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuLaserOffCntGet(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;
    UINT32 laserOffTime = 0;
    OPL_RESULT halStatus = 0;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPonLaserOffCntGet(&laserOffTime);

    if(OPL_OK == halStatus)
    {
        p->laserOffTime = laserOffTime;
    }

    return;
}
/*******************************************************************************
* halOnuSyncTimeGet
*
* DESCRIPTION:
*  		this function is used get the synctime value learned from olt.
*
*	INPUTS:
*		count: buff used to store the value of tq used for synctime
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuSyncTimeGet(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;
    UINT32 syncTime = 0;
    OPL_STATUS halStatus = 0;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPonSyncTimeGet(&syncTime);

    if(OPL_OK == halStatus)
    {
        p->syncTime = syncTime;
    }

    return;
}
/*******************************************************************************
* halOnuLlidGet
*
* DESCRIPTION:
*  		this function is used get the llid assigned by olt.
*
*	INPUTS:
*		count: buff used to store the value of tq used for synctime
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuLlidGet(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;
    UINT32 llid = 0;
    OPL_RESULT halStatus = 0;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPonLlidGet(&llid);

    if(OPL_OK == halStatus)
    {
        p->llid = llid;
    }

    return;
}
/*******************************************************************************
* halOnuSerdesDelayCntSet
*
* DESCRIPTION:
*  		this function is used set the serdes dealy count
*
*	INPUTS:
*		count:
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuSerdesDelayCntSet(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonSerdesDelayCntSet(p->serdesDelay);

    return ;
}
/*******************************************************************************
* halOnuSerdesDelayCntSet
*
* DESCRIPTION:
*  		this function is used get the serdes delay count
*	INPUTS:
*		count:
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuSerdesDelayCntGet(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;
    OPL_RESULT halStatus = 0;
    UINT32 serdesDelay = 0;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    halStatus = dalPonSerdesDelayCntGet(&serdesDelay);

    if(OPL_OK == halStatus)
    {
        p->serdesDelay = serdesDelay;
    }

    return;
}

/*******************************************************************************
* halOnuLinkStatusGet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuLinkStatusGet(void *pInput)
{
    HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;
    DAL_PON_STATUS_t  ponStatus;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonMpcpStatusGet(&ponStatus);

    p->mpcpStatus = *(UINT32 *)(&ponStatus);
    
    return ;
}

/*******************************************************************************
* halOnuOamFinishedSet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuOamFinishedSet(void *pInput)
{
    HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonOamFinishedSet(p->oamStatus);

    return ;
}
/*******************************************************************************
* halOnuPonMacBerGet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuPonMacBerGet(void *pInput)
{
    HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;
    float realTimeBer = 0;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    //ponBerGet(&realTimeBer);

    p->realTimeBer = realTimeBer;

    return ;
}

/*******************************************************************************
* halOnuSecurityStatusSet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuSecurityStatusSet(void *pInput)
{
    HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }
    
    return ;
}
/*******************************************************************************
* halOnuSecurityStatusGet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuSecurityStatusGet(void *pInput)
{
    HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    p->secStatus = OPL_ENABLE;
    
    return ;
}
/*******************************************************************************
* halOnuSecurityModeSet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuSecurityModeSet(void *pInput)
{
    HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonSecurityModeSet(p->channelOfSec,p->secMode);
    
    return ;
}
/*******************************************************************************
* halOnuSecurityModeGet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuSecurityModeGet(void *pInput)
{
    HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;
    UINT32 secMode = 0;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonSecurityModeGet(p->channelOfSec,&secMode);

    p->secMode = secMode;
    
    return ;
}
/*******************************************************************************
* halOnuSecurityAesModeSet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuSecurityAesModeSet(void *pInput)
{
    HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonSecurityAesModeSet(p->channelOfSec,p->aesMode);

    return ;
}
/*******************************************************************************
* halOnuSecurityAesModeGet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuSecurityAesModeGet(void *pInput)
{
    HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;
    UINT32 aesMode = 0; 

    HAL_ONU_DEBUG_TRACE();       
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonSecurityAesModeGet(p->channelOfSec,&aesMode);

    p->aesMode = aesMode;

    return ;
}
/*******************************************************************************
* halOnuUniSecurityKeyGet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuUniSecurityKeyGet(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    if(NULL == p->secKey)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    switch(p->secMode)
    {
        case HAL_PON_SECU_MODE_TRIPLE_CHURNING:
            dalPonUnicastTripleChurningKeyGet(p->secKeyIndex, p->secKey);
            break;
        case HAL_PON_SECU_MODE_AES:
            dalPonUnicastAesKeyGet(p->secKeyIndex, p->secKey,p->aesMode);
            break;
        default:
            return;
    }

    return;
}
/*******************************************************************************
* halOnuScbSecurityKeySet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuScbSecurityKeySet(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    if(NULL == p->secKey)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    switch(p->secMode)
    {
        case HAL_PON_SECU_MODE_TRIPLE_CHURNING:
            dalPonScbTripleChurningKeySet(p->secKeyIndex, p->secKey);
            break;
        case HAL_PON_SECU_MODE_AES:
            dalPonScbAesKeySet(p->secKeyIndex, p->secKey,p->aesMode);
            break;
        default:
            return ;
    }

    return;
}
/*******************************************************************************
* halOnuDbaQsetThesholdSet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuDbaQsetThesholdSet(void *pInput)
{
	HAL_ONU_DBA_CFG_t *p = (HAL_ONU_DBA_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalDbaQueueThresholdSet(p->qSetNum,p->qNum,p->threshold);
    
    return;
}

/*******************************************************************************
* halOnuDbaQsetThesholdGet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuDbaQsetThesholdGet(void *pInput)
{
	HAL_ONU_DBA_CFG_t *p = (HAL_ONU_DBA_CFG_t *)pInput;
    UINT16 threshold;

    HAL_ONU_DEBUG_TRACE();    
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalDbaQueueThresholdGet(p->qSetNum,p->qSetNum,&threshold);

    p->threshold = threshold;
    
    return;
}
/*******************************************************************************
* halOnuDbaRptModeSet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuDbaRptModeSet(void *pInput)
{
	HAL_ONU_DBA_CFG_t *p = (HAL_ONU_DBA_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalDbaReportModeSet(p->reportMode);
    
    return;
}

/*******************************************************************************
* halOnuDbaRptModeGet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuDbaRptModeGet(void *pInput)
{
	HAL_ONU_DBA_CFG_t *p = (HAL_ONU_DBA_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalDbaReportModeGet(&(p->reportMode));
    
    return;
}

/*******************************************************************************
* halOnuDbaAgentSet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuDbaAgentSet(void *pInput)
{
	HAL_ONU_DBA_AGENT_CFG_t *p = (HAL_ONU_DBA_AGENT_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonMpcpDbaAgentConfigAllSet(
        p->cpRptStaMode,
        p->cpMergeRptSendMode,
        p->cpRptSendMode,
        p->cpPktInitTimerCfg, 
        p->cpDeltaTxNormalNoFecTime,
        p->cpTxMpcpNoFecTime);

    return;
}

/*******************************************************************************
* halOnuDbaAgentGet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuDbaAgentGet(void *pInput)
{
	HAL_ONU_DBA_AGENT_CFG_t *p = (HAL_ONU_DBA_AGENT_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonMpcpDbaAgentConfigAllGet(
        &(p->cpRptStaMode),
        &(p->cpMergeRptSendMode),
        &(p->cpRptSendMode),
        &(p->cpPktInitTimerCfg), 
        &(p->cpDeltaTxNormalNoFecTime),
        &(p->cpTxMpcpNoFecTime));
    
    return;
}

/*******************************************************************************
* halOnuDbaDlySet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuDbaDlySet(void *pInput)
{
	HAL_ONU_DBA_DLY_CFG_t *p = (HAL_ONU_DBA_DLY_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonDelayConfigAllSet(p->cpTxDlyEn,p->cpSyncEn,p->cpRptsentDelt,p->cpTxDly);

    return;
}

/*******************************************************************************
* halOnuDbaDlyGet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuDbaDlyGet(void *pInput)
{
	HAL_ONU_DBA_DLY_CFG_t *p = (HAL_ONU_DBA_DLY_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonDelayConfigAllGet(&(p->cpTxDlyEn),&(p->cpSyncEn),&(p->cpRptsentDelt),&(p->cpTxDly));
    
    return;
}

/*******************************************************************************
* halOnuMpcpHoldoverContrlSet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuMpcpHoldoverContrlSet(void *pInput)
{
	HAL_ONU_MPCP_FSM_HOLD_CTRL_CFG_t *p = (HAL_ONU_MPCP_FSM_HOLD_CTRL_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonMpcpFsmHoldOverCtrlSet(p->enable);
		
    return;
}

/*******************************************************************************
* halOnuMpcpHoldoverContrlGet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuMpcpHoldoverContrlGet(void *pInput)
{
	HAL_ONU_MPCP_FSM_HOLD_CTRL_CFG_t *p = (HAL_ONU_MPCP_FSM_HOLD_CTRL_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonMpcpFsmHoldOverCtrlGet(&(p->enable));
    
    return;
}

/*******************************************************************************
* halOnuMpcpHoldoverTimeSet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuMpcpHoldoverTimeSet(void *pInput)
{
	HAL_ONU_MPCP_FSM_HOLD_TIME_CFG_t *p = (HAL_ONU_MPCP_FSM_HOLD_TIME_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

   	dalPonMpcpFsmHoldOverTimeSet(p->holdtime);

    return;
}

/*******************************************************************************
* halOnuMpcpHoldoverTimeGet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuMpcpHoldoverTimeGet(void *pInput)
{
	HAL_ONU_MPCP_FSM_HOLD_TIME_CFG_t *p = (HAL_ONU_MPCP_FSM_HOLD_TIME_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalPonMpcpFsmHoldOverTimeGet(&(p->holdtime));
    
    return;
}



/*******************************************************************************
* halOnuMpcpHoldoverTimeGet
*
* DESCRIPTION:
*  		this function is used to set onu mtu info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuMtuSet(void *pInput)
{
	HAL_ONU_MTU_CFG_t *p = (HAL_ONU_MTU_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalOnuMtuSet(p->mtuSize);

    return;
}

/*******************************************************************************
* halOnuMpcpHoldoverTimeGet
*
* DESCRIPTION:
*  		this function is used to get onu mtu infor 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuMtuGet(void *pInput)
{
	HAL_ONU_MTU_CFG_t *p = (HAL_ONU_MTU_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalOnuMtuGet(&(p->mtuSize));
    
    return;
}

/*******************************************************************************
* halOnuPonConfig
*
* DESCRIPTION:
*  		this function is used to config pon loopback
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuPonLoopbackConfig(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalOnuPonLoopbackConfig(p->lpChannel,p->lpEnable);
    
    return;
}


void halOnuPonTxPolarityConfig(void *pInput)
{
	HAL_ONU_PON_CFG_t *p = (HAL_ONU_PON_CFG_t *)pInput;

    HAL_ONU_DEBUG_TRACE();
    if(NULL == p)
    {
        HAL_ONU_TRACE();
        HAL_ONU_PRINTF(("input null pointer.\n"));
        return;
    }

    dalOnuPonTxPolarityConfig(p->PontxPolarity);
    
    return;
}

/*******************************************************************************
* halOnuRegister
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void halOnuRegister(void)
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet = 0;

    stApiUnit.apiId = HAL_API_ONU_INIT;
    stApiUnit.apiFunc = halOnuInit;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_MACID_SET;
    stApiUnit.apiFunc = halOnuMacIdSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_MACID_GET;
    stApiUnit.apiFunc = halOnuMacIdGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_PHY_FEC_MODE_SET;
    stApiUnit.apiFunc = halOnuPhyFecModeSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_PHY_FEC_MODE_GET;
    stApiUnit.apiFunc = halOnuPhyFecModeGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_LASER_ONCNT_SET;
    stApiUnit.apiFunc = halOnuLaserOnCntSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_LASER_ONCNT_GET;
    stApiUnit.apiFunc = halOnuLaserOnCntGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_LASER_OFFCNT_SET;
    stApiUnit.apiFunc = halOnuLaserOffCntSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_LASER_OFFCNT_GET;
    stApiUnit.apiFunc = halOnuLaserOffCntGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_SYNCTIME_GET;
    stApiUnit.apiFunc = halOnuSyncTimeGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_LLID_GET;
    stApiUnit.apiFunc = halOnuLlidGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_SERDES_DELAYCNT_SET;
    stApiUnit.apiFunc = halOnuSerdesDelayCntSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_SERDES_DELAYCNT_GET;
    stApiUnit.apiFunc = halOnuSerdesDelayCntGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_LINK_STATUS_GET;
    stApiUnit.apiFunc = halOnuLinkStatusGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_OAM_FINISHED_SET;
    stApiUnit.apiFunc = halOnuOamFinishedSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_PON_MAC_BER_GET;
    stApiUnit.apiFunc = halOnuPonMacBerGet;
    ulRet += halDrvApiRegister(&stApiUnit);
    
    stApiUnit.apiId = HAL_API_ONU_SECURITY_STATUS_SET;
    stApiUnit.apiFunc = halOnuSecurityStatusSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_SECURITY_STATUS_GET;
    stApiUnit.apiFunc = halOnuSecurityStatusGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_SECURITY_MODE_SET;
    stApiUnit.apiFunc = halOnuSecurityModeSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_SECURITY_MODE_GET;
    stApiUnit.apiFunc = halOnuSecurityModeGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_SECURITY_AES_MODE_SET;
    stApiUnit.apiFunc = halOnuSecurityAesModeSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_SECURITY_AES_MODE_GET;
    stApiUnit.apiFunc = halOnuSecurityAesModeGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_UNI_SECURITY_KEY_GET;
    stApiUnit.apiFunc = halOnuUniSecurityKeyGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_SCB_SECURITY_KEY_SET;
    stApiUnit.apiFunc = halOnuScbSecurityKeySet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_DBA_QSET_Q_THRESHOLD_SET;
    stApiUnit.apiFunc = halOnuDbaQsetThesholdSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_DBA_QSET_Q_THRESHOLD_GET;
    stApiUnit.apiFunc = halOnuDbaQsetThesholdGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_DBA_RPT_MODE_SET;
    stApiUnit.apiFunc = halOnuDbaRptModeSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_DBA_RPT_MODE_GET;
    stApiUnit.apiFunc = halOnuDbaRptModeGet;
    ulRet += halDrvApiRegister(&stApiUnit);
    
    stApiUnit.apiId = HAL_API_ONU_DBA_AGENT_SET;
    stApiUnit.apiFunc = halOnuDbaAgentSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_DBA_AGENT_GET;
    stApiUnit.apiFunc = halOnuDbaAgentGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_DBA_DLY_SET;
    stApiUnit.apiFunc = halOnuDbaDlySet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_ONU_DBA_DLY_GET;
    stApiUnit.apiFunc = halOnuDbaDlyGet;
    ulRet += halDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = HAL_API_ONU_MPCP_HOLDOVER_CTRL_SET;
	stApiUnit.apiFunc = halOnuMpcpHoldoverContrlSet;
	ulRet += halDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = HAL_API_ONU_MPCP_HOLDOVER_CTRL_GET;
	stApiUnit.apiFunc = halOnuMpcpHoldoverContrlGet;
	ulRet += halDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = HAL_API_ONU_MPCP_HOLDOVER_TIME_SET;
	stApiUnit.apiFunc = halOnuMpcpHoldoverTimeSet;
	ulRet += halDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = HAL_API_ONU_MPCP_HOLDOVER_TIME_GET;
	stApiUnit.apiFunc = halOnuMpcpHoldoverTimeGet;
	ulRet += halDrvApiRegister(&stApiUnit);	
	
	stApiUnit.apiId = HAL_API_ONU_MTU_SET;
	stApiUnit.apiFunc = halOnuMtuSet;
	ulRet += halDrvApiRegister(&stApiUnit); 
	
	stApiUnit.apiId = HAL_API_ONU_MTU_GET;
	stApiUnit.apiFunc = halOnuMtuGet;
	ulRet += halDrvApiRegister(&stApiUnit); 

	stApiUnit.apiId = HAL_API_ONU_PON_LOOPBACK_CONFIG;
	stApiUnit.apiFunc = halOnuPonLoopbackConfig;
	ulRet += halDrvApiRegister(&stApiUnit); 

	stApiUnit.apiId = HAL_API_ONU_PONTX_POLARITY_CONFIG;
	stApiUnit.apiFunc = halOnuPonTxPolarityConfig;
	ulRet += halDrvApiRegister(&stApiUnit); 

	return ;
}


