/*
=============================================================================
     File Name: brg_storm.c

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

#include "odm_storm.h"
#include "hal.h"
#include "odm_port.h"
int port_parse(int port)
{
	switch(port)
	{
		case 1:
			return 1;
		case 2:
#ifdef ONU_1PORT
			return 6;
#else
	        return 2;
#endif
		case 3:
			return 3;
		case 4:
			return 4;
#ifdef ONU_4PORT_AR8327
		case 5:
			return 0;
#else
        case 5:
			return 6;
#endif
		default:
			return 1;
	}
}

/*******************************************************************************
* odmStormCtrlFrameSet
*
* DESCRIPTION:
* this function is used to set the strom config info 
*	 
* INPUTS:
*		stormLimitTYpe: 0 for broadcast,1 for multicast while 2 for dlf 
* 		limitEnable : 1 for enable 0 for disable
*
* OUTPUTS: 
*		no 
* RETURNS:
* OK if the access is success.
* ERROR if the access is failed.
* SEE ALSO: 
*/
UINT32 odmStormCtrlFrameSet(UINT32 portId,UINT32 stormLimitType, UINT32 limitEnable)
{
	UINT32 retVal = OK;
	UINT8 	portStormSectionBuff[255] = {0};
	UINT32  oldLimitEnable;
	UINT32  oldLimitType;
	UINT32   dalStatus = OK;
    OPL_API_DATA_t       stData;
	HAL_PORT_STORM_CFG_t portStormCfg;
	UINT8 	stormBuff[32] = {0};
	const char *Storm[]={"UC","MC","BC"};

	vosSprintf(portStormSectionBuff,CFG_PORT_STORM_SECTION,portId);
	vosSprintf(stormBuff,"%s",Storm[stormLimitType]);
	oldLimitEnable = vosConfigUInt32Get(STORM_CFG,portStormSectionBuff,stormBuff,0);
 	if((OK == retVal && (oldLimitEnable != limitEnable ))||(OK != retVal))
	{
		stData.apiId = HAL_API_PORT_STORM_CTRL_FRAME_SET;
		stData.length = sizeof(HAL_PORT_STORM_CFG_t);
		portStormCfg.portId = portId;
		portStormCfg.stormType = stormLimitType;
		portStormCfg.limitEds = limitEnable;
		stData.param = (void *)&portStormCfg;
		dalStatus = halAppApiCallSync(&stData);
		if(OK != dalStatus)
		{
			ODM_PORT_LOG_TRACE();
			return dalStatus;
		}
	
		vosConfigUInt32Set(STORM_CFG,portStormSectionBuff,stormBuff,limitEnable);
	}
	
	return OK;		
}
/*******************************************************************************
* odmStormCtrlFrameSet
*
* DESCRIPTION:
* this function is used get the bridge storm config info
*	 
* INPUTS:
*		stormLimitTYpe: 0 for unicast,1 for multicast while 2 for broadcast
* 		limitEnable : 1 for enable 0 for disable
*
* OUTPUTS: 
*		no 
* RETURNS:
* OK if the access is success.
* ERROR if the access is failed.
* SEE ALSO: 
*/
UINT32 odmStormCtrlFrameGet(UINT32 portId,UINT32 stormLimitType, UINT32 *limitEnable)
{
	UINT32 retVal = OK;
	UINT8 	portStormSectionBuff[255] = {0};
	UINT32  tmpLimitType;
	UINT8 	stormBuff[32] = {0};
	const char *Storm[]={"UC","MC","BC"};

	if(NULL == limitEnable)
	{
		return ERROR;	
	}
	vosSprintf(portStormSectionBuff,CFG_PORT_STORM_SECTION,portId);
	vosSprintf(stormBuff,"%s",Storm[stormLimitType]);
	*limitEnable = vosConfigUInt32Get(STORM_CFG,portStormSectionBuff,stormBuff,FALSE);

	return OK;	
}
/*******************************************************************************
* odmStormCtrlRateSet
*
* DESCRIPTION:
* this function is used set the rate of port for storm ctrl
*	 
* INPUTS:
*		
*
* OUTPUTS: 
*		no 
* RETURNS:
* OK if the access is success.
* ERROR if the access is failed.
* SEE ALSO: 
*/
UINT32 odmStormCtrlRateSet(UINT32 portId,UINT32 rateLimit)
{
	UINT32  retVal = OK;
	UINT32  oldRateLimit;
	UINT8 	portStormSectionBuff[255] = {0};
	INT32   dalStatus = OK;
    OPL_API_DATA_t      stData;
    HAL_PORT_STORM_CFG_t halPortStormCfg;

#if defined(ONU_1PORT) || defined(ONU_4PORT_AR8327)
	if (rateLimit%32 != 0){
	    /*ODM_PORT_LOG_TRACE(); */
        return ERR_INVALID_PARAMETERS;
	}
    #define  STORM_RATE_LIMIT_VALUE_MAX 1000000
	if (rateLimit > STORM_RATE_LIMIT_VALUE_MAX){
	    /*ODM_PORT_LOG_TRACE(); */
        return ERR_INVALID_PARAMETERS;
	}
#else
	if( rateLimit<0 || rateLimit >1024)
	{
	    ODM_PORT_LOG_TRACE();
		return ERR_INVALID_PARAMETERS;
	}
#endif

	vosSprintf(portStormSectionBuff,CFG_PORT_STORM_SECTION,portId);
	retVal = vosConfigKeyIsExisted(STORM_CFG,portStormSectionBuff,CFG_STORM_RATE_LIMIT);
	oldRateLimit = vosConfigUInt32Get(STORM_CFG,portStormSectionBuff,CFG_STORM_RATE_LIMIT,1);

	if((OK == retVal && oldRateLimit != rateLimit)||(OK != retVal))
	{
	    stData.apiId = HAL_API_PORT_STORM_CTRL_RATE_SET;
        stData.length = sizeof(HAL_PORT_CFG_INFO_t);
        halPortStormCfg.portId = portId;
        halPortStormCfg.rateLimit= rateLimit;
        stData.param = (void *)&halPortStormCfg;
        dalStatus = halAppApiCallSync(&stData);
		if(OK != dalStatus)
		{
		    ODM_PORT_LOG_TRACE();
			return dalStatus;
		}

		vosConfigUInt32Set(STORM_CFG,portStormSectionBuff,CFG_STORM_RATE_LIMIT,rateLimit);
	}
	return OK;

}

UINT32 odmStormCtrlRateGet(UINT32 portId,UINT32 *rateLimit)
{
	UINT8 	portStormSectionBuff[255] = {0};
    if(NULL == rateLimit)
    {
        return ERR_NULL_POINTER;
    }
	vosSprintf(portStormSectionBuff,CFG_PORT_STORM_SECTION,portId);
	*rateLimit = vosConfigUInt32Get(STORM_CFG,portStormSectionBuff,CFG_STORM_RATE_LIMIT,1);
	return OK;
}

UINT32 odmPortStormCfgInit(void)
{
    UINT32  portId;
	UINT32  stormLimitType;
	UINT32  limitEnable;
	UINT32  stormRate;
	UINT32  retVal = OK;
    UINT32  i;
    OPL_API_DATA_t              stData;
	HAL_PORT_STORM_CFG_t        portStormCfg;


    for(i = ODM_START_PORT_NUN; i <= ODM_NUM_OF_PORTS+1; i++)
    {
        portId = port_parse(i);

        odmStormCtrlRateGet(portId, &stormRate);

    	stData.apiId = HAL_API_PORT_STORM_CTRL_RATE_SET;
        stData.length = sizeof(HAL_PORT_STORM_CFG_t);
        portStormCfg.portId = portId;
        portStormCfg.rateLimit= stormRate;
        stData.param = (void *)&portStormCfg;
        retVal = halAppApiCallSync(&stData);

		if(OK != retVal)
		{
			printf("fail to set port storm rate!\n");
		}

		for(stormLimitType = 0; stormLimitType < 3; stormLimitType++)
		{
            odmStormCtrlFrameGet(portId,stormLimitType,&limitEnable);
    
    		stData.apiId = HAL_API_PORT_STORM_CTRL_FRAME_SET;
    		stData.length = sizeof(HAL_PORT_STORM_CFG_t);
    		portStormCfg.portId = portId;
    		portStormCfg.stormType = stormLimitType;
    		portStormCfg.limitEds = limitEnable;
    		stData.param = (void *)&portStormCfg;
    		retVal = halAppApiCallSync(&stData);
    
    		if(OK != retVal)
    		{
    			printf("fail to set port storm type and limitEnable!\n");
    		}
		}
    }
    return OK;
}


