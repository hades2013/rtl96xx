/*
=============================================================================
     file Name: dal_storm.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                		Date              	Description of Changes
----------------   ------------  ----------------------------------------------
 zran 				2009/05/20		Initial Version	
----------------   ------------  ----------------------------------------------

*/

#include "dal_lib.h"
#include "tm.h"
//#define MODULE MOD_STORM

/**
@brief This defines storm type
*/
 #define UNICAST_STORM_TYPE 0	
 #define MULTICAST_STORM_TYPE 1
 #define BROADCAST_STORM_TYPE 2	

 /**
@brief This defines queue type
*/
#define MULTICAST_EN_QUEUE_NUM 5
#define BRAODCAST_EN_QUEUE_NUM 6

 /**
@brief This defines downstream pord id.
*/
#define DOWNSTREAM_PORD_ID 6

 /**
@brief helper function to get the queue id for different storm type.
*/
#define RETRIEVE_QUEUE_ID_BY_LIMITED_TYPE(queueMapId, stormLimitType) \
do { \
    if (BROADCAST_STORM_TYPE == stormLimitType){ \
        queueMapId = BRAODCAST_EN_QUEUE_NUM; \
    } \
    else if (MULTICAST_STORM_TYPE == stormLimitType){ \
        queueMapId = MULTICAST_EN_QUEUE_NUM; \
    } \
    else { \
        OPL_LOG_TRACE(); \
        return OPL_ERROR; \
     } \
    } \
 while(0);

/**
@ 0x01: multicast rule, 0x02: broadcast rule, 0x03: broadcast & multicast.
*/
//static UINT8 g_stromClsRuleAdded;
static UINT16 g_stormClsRuleId[2][2]; /*0:multicast, 1:broadcast.*/
/**
@ 0x01: upstream is set, 0x02: dowstream is set, 0x03: upstream & dowstream.
*/
//static UP_DOWN_STREAM_e g_streamType = 0x0; 


/*******************************************************************************
* dalStormClsRuleAdd
*
* DESCRIPTION:
*		helper function to add classify rule.
* INPUT:
*		stormLimitType: 1/2.
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS dalStormClsRuleAdd(UINT32 stormLimitType, UP_DOWN_STREAM_e streamType)
{
    UINT16* pRule, *pOtherRule;
    OPL_STATUS ret = OPL_OK;
    CLS_CONFIG_INFO_t stClassCfg;
	UINT32 setFlag = 0;
	
    OPL_MEMSET(&stClassCfg, 0, sizeof(stClassCfg));
    
    stClassCfg.valid = 1;
    stClassCfg.t_act = DONOT_DROP;
    stClassCfg.dstMacFlag = 1;
    stClassCfg.rmkicoschange = 1;

    stClassCfg.dstMac.highRange[0] = 0xff;
    stClassCfg.dstMac.highRange[1] = 0xff;
    stClassCfg.dstMac.highRange[2] = 0xff;
    
    if (BROADCAST_STORM_TYPE == stormLimitType){
        pRule = &(g_stormClsRuleId[1]);
        stClassCfg.queueMapId = BRAODCAST_EN_QUEUE_NUM;
        stClassCfg.dstMac.highRange[3] = 0xff;
        stClassCfg.dstMac.highRange[4] = 0xff;
        stClassCfg.dstMac.highRange[5] = 0xff; 
        OPL_MEMSET(stClassCfg.dstMac.lowRange, 0xff, sizeof(stClassCfg.dstMac.lowRange));
    }
    else if (MULTICAST_STORM_TYPE == stormLimitType){
        pRule = &(g_stormClsRuleId[0]);
        stClassCfg.queueMapId = MULTICAST_EN_QUEUE_NUM;
        stClassCfg.dstMac.lowRange[0] = 0x01;
        stClassCfg.dstMac.lowRange[1] = 0x00;
        stClassCfg.dstMac.lowRange[2] = 0x5e;
        stClassCfg.dstMac.lowRange[3] = 0x00;
        stClassCfg.dstMac.lowRange[4] = 0x00;
        stClassCfg.dstMac.lowRange[5] = 0x00;
    }
    else {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

	pOtherRule = pRule;
	if (UP_STREAM == streamType){
		pOtherRule++;
		if (*pOtherRule == 0){
			stClassCfg.bingress = 0;
        	stClassCfg.ingressmask = 1;
		}
	}
	else if(DOWN_STREAM == streamType){
		if (*pOtherRule == 0){
			stClassCfg.bingress = 1;
        	stClassCfg.ingressmask = 1;
		}
		pRule++;
	}
	else {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }
	
	if (*pRule > 0){
		OPL_DAL_PRINTF(("[ %s ]: storm type %d stream type %d classify rule %d has been created.\r\n", __FUNCTION__, stormLimitType, streamType, *pRule));
		return OPL_OK;
	}

	if (*pOtherRule > 0){
		ret = dalVoipClsDelEntry(*pOtherRule);
		if (OPL_OK != ret){
			OPL_LOG_TRACE();
		}
		else {
			setFlag = 1;
			*pOtherRule = 0;
		}
	}
	
    ret = dalVoipClsRuleCtcAdd(&stClassCfg, pRule);
    if (ret == OPL_OK && setFlag == 1){
        *pOtherRule = *pRule;
    }

    OPL_DAL_PRINTF(( "[ %s ]: ruleId:%d, otherRuleId:%d, return code:%d.\r\n", __FUNCTION__, *pRule, *pOtherRule, ret));

    return ret;
}

/*******************************************************************************
* dalStormClsRuleDel
*
* DESCRIPTION:
*		helper function to delete the added cls rule from dalStormClsRuleAdd.
*	INPUT:
*		stormLimitType:1/2
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS dalStormClsRuleDel(UINT32 stormLimitType, UP_DOWN_STREAM_e streamType)
{
     UINT32  ret = OPL_OK;
     UINT16* pRule, *pOtherRule;

    if (BROADCAST_STORM_TYPE == stormLimitType){
        pRule = &(g_stormClsRuleId[1]);
    }
    else if (MULTICAST_STORM_TYPE == stormLimitType){
        pRule = &(g_stormClsRuleId[0]);
    }
    else {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

	pOtherRule = pRule;
	if(DOWN_STREAM == streamType){
		pRule++;
	}
	else {
		pOtherRule++;
	}


	if (*pRule > 0){
		 ret = dalVoipClsDelEntry(*pRule);
        if (ret == OPL_OK){
            *pRule = 0;
			if (*pOtherRule > 0){
				*pOtherRule = 0;
				dalStormClsRuleAdd(stormLimitType, (DOWN_STREAM==streamType)?UP_STREAM:DOWN_STREAM);
			}
        }
	     else{
	        OPL_LOG_TRACE();
	     }
	}
	
    OPL_DAL_PRINTF(( "[ %s ]: storm type %d stream type %d return code:%d.\r\n", __FUNCTION__, stormLimitType, streamType, ret));

    return ret;
}

/*******************************************************************************
* dalStormCtrlFrameSet
*
* DESCRIPTION:
*		set the controlled storm frame type.
*	INPUT:
*		portId:1(upstream),6(downstream)
*       stormLimitType: 1(multicast), 2(broadcast)
*       limitEnable: 0(disable), 1(enable)
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS dalStormCtrlFrameSet(UINT32 portId,UINT32 stormLimitType,UINT32 limitEnable)
{
    UINT32  ret = OPL_OK;
    UINT32  queueMapId = TM_UP_STREAM_NUM; 
    UP_DOWN_STREAM_e streamType = (DOWNSTREAM_PORD_ID==portId)?DOWN_STREAM:UP_STREAM;

    OPL_DAL_PRINTF(( "[ %s ]: portId:%d,stormLimitType:%d,limitEnable:%d.\r\n", __FUNCTION__, portId,stormLimitType,limitEnable));

	RETRIEVE_QUEUE_ID_BY_LIMITED_TYPE(queueMapId, stormLimitType);

    if (0 == limitEnable){
        ret = dalStormClsRuleDel(stormLimitType, streamType);
    }
    else {
        ret = dalStormClsRuleAdd(stormLimitType, streamType);
    }

    if ( OPL_OK != ret){
		OPL_DAL_PRINTF(( "[ %s ]: portId:%d,stormLimitType:%d,limitEnable:%d.\r\n", __FUNCTION__, portId,stormLimitType,limitEnable));
        return OPL_ERROR;
    }
    if (!IS_ONU_RESPIN)
    {
	    ret = tmQueueShaperEnableHwWrite(streamType, queueMapId, limitEnable);
	    if ( OPL_OK != ret){
	    	if (0 != limitEnable){
	    		dalStormClsRuleDel(stormLimitType, streamType);
	    	}
	        OPL_LOG_TRACE();
	        return OPL_ERROR;
	    }
    }
	else
	{
	    ret = tmQueueShaperPirEnableHwWrite(streamType, queueMapId, limitEnable);
	    if ( OPL_OK != ret){
	    	if (0 != limitEnable){
	    		dalStormClsRuleDel(stormLimitType, streamType);
	    	}
	        OPL_LOG_TRACE();
	        return OPL_ERROR;
	    }
	}

	
	OPL_DAL_PRINTF(( "[ %s ]: return %d.\r\n", __FUNCTION__, ret));

    return ret;

}

/*******************************************************************************
* dalStormCtrlFrameGet
*
* DESCRIPTION:
*		get the enable value according pord id and storm type.
*	INPUT:
*		portId:1(upstream),6(downstream)
*       stormLimitType: 1(multicast), 2(broadcast)
*
* OUTPUTS:
*       *limitEnable: 0(disable), 1(enable)
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS dalStormCtrlFrameGet(UINT32 portId,UINT32 stormLimitType,UINT32 *limitEnable)
{
    UINT32  ret = OPL_OK;
    UINT32  queueMapId = TM_UP_STREAM_NUM; 
    UP_DOWN_STREAM_e streamType = (DOWNSTREAM_PORD_ID==portId)?DOWN_STREAM:UP_STREAM;
 
    if (limitEnable == OPL_NULL){
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

    RETRIEVE_QUEUE_ID_BY_LIMITED_TYPE(queueMapId, stormLimitType);
    
    ret = tmQueueShaperEnableHwRead(streamType, queueMapId, &limitEnable);
    if ( OPL_OK != ret){
        OPL_LOG_TRACE();
        return OPL_ERROR;
    }

    OPL_DAL_PRINTF(( "[ %s ]: portId:%d,stormLimitType:%d,limitEnable:%d,\r\n", __FUNCTION__, portId,stormLimitType,*limitEnable));
         
    return ret;

}

/*******************************************************************************
* dalStormCtrlRateSet
*
* DESCRIPTION:
*		set the storm limted rate.
*	INPUT:
*		portId:1(upstream),6(downstream)
*       rateLimit: rate size.
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS dalStormCtrlRateSet(UINT32 portId,UINT32 rateLimit)
{
    UINT32 cir, cbs;
    UINT32 pir, pbs;
    OPL_STATUS ret = OPL_OK;
    UP_DOWN_STREAM_e streamType = (DOWNSTREAM_PORD_ID==portId)?DOWN_STREAM:UP_STREAM;

    OPL_DAL_PRINTF(( "[ %s ]: portId:%d,rateLimit:%d.\r\n", __FUNCTION__, portId,rateLimit));

    if (rateLimit%32 != 0 /*|| g_stromClsRuleAdded==0*/){
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }
    
    cir = rateLimit/32 -1;
    cbs =  0x3ff;

    pir = rateLimit/32 -1;
    pbs =  0x3ff;
    
    if (1/*g_stromClsRuleAdded & BROADCAST_STORM_TYPE*/){
        ret = tmQueueCirHwWrite(streamType, BRAODCAST_EN_QUEUE_NUM, cir);
        OPL_DAL_PRINTF(( "[ %s ]: cir:%d, ret:%d.", __FUNCTION__, cir, ret));
        if ( OPL_OK != ret){
            OPL_LOG_TRACE();
            return OPL_ERROR;
        }
        ret = tmQueueCbsHwWrite(streamType, BRAODCAST_EN_QUEUE_NUM, cbs);
        OPL_DAL_PRINTF(( "[ %s ]: cbs:%d, ret:%d.", __FUNCTION__, cbs, ret));
        if ( OPL_OK != ret){
            OPL_LOG_TRACE();
            return OPL_ERROR;
        }
		if (IS_ONU_RESPIN)
		{
			ret=tmUsShaperTabPirDirectHwWrite(BRAODCAST_EN_QUEUE_NUM,pir,pbs);
			OPL_DAL_PRINTF(( "[ %s ]: pir:%d,pbs:%d, ret:%d.", __FUNCTION__, pir,pbs, ret));
			if ( OPL_OK != ret){
				OPL_LOG_TRACE();
				return OPL_ERROR;
			}
		}
           
    }
    /* if (1 ) {
        ret = tmQueueCirHwWrite(streamType, MULTICAST_EN_QUEUE_NUM, cir);
        ret += tmQueueCbsHwWrite(streamType, BRAODCAST_EN_QUEUE_NUM, cbs);
        if ( OPL_OK != ret){
            OPL_LOG_TRACE();
            return OPL_ERROR;
        }
    } */
    
    return ret;	
}

/*******************************************************************************
* dalStormCtrlRateGet
*
* DESCRIPTION:
*		get the storm limted rate.
*	INPUT:
*		portId:1(upstream),6(downstream)
*
* OUTPUTS:
*       rateLimit: rate size.
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS dalStormCtrlRateGet(UINT32 portId,UINT32 *rateLimit)
{
    UINT32 cir = 0;
    OPL_STATUS ret = OPL_OK;
    UP_DOWN_STREAM_e streamType = (DOWNSTREAM_PORD_ID==portId)?DOWN_STREAM:UP_STREAM;   
    
    if (rateLimit == OPL_NULL){
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

    ret = tmQueueCirHwRead(streamType, BRAODCAST_EN_QUEUE_NUM, &cir);
    if ( OPL_OK != ret){
         OPL_LOG_TRACE();
         return OPL_ERROR;
     }

    *rateLimit = (cir+1)*32;

    OPL_DAL_PRINTF(( "[ %s ]: portId:%d,rateLimit:%d,\r\n", __FUNCTION__, portId,*rateLimit));

    return ret;
}

