/*
=============================================================================
     Header Name: odm_storm.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/14		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifndef  __BRG_STORM_H__
#define __BRG_STORM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <vos.h>


#define STORM_CFG		"/cfg/storm.conf"

#define	CFG_PORT_STORM_SECTION     "Port%0.4d"


#define STORM_BCAST_LIMIT_SECTION "BroadcastLimit"

#define STORM_BCAST_LIMIT_ACTIVE "Enable"
#define STORM_BCAST_LIMIT_PERCENT "%0.4d"

#define STORM_MCAST_LIMIT_SECTION "MulticastLimit"

#define STORM_MCAST_LIMIT_ACTIVE "Enable"
#define STORM_MCAST_LIMIT_PERCENT "%0.4d"

#define STORM_DLF_LIMIT_SECTION    "DlfLimit"

#define STORM_DLF_LIMIT_ACTIVE "Enable"
#define STORM_DLF_LIMIT_PERCENT "%0.4d"

#define CFG_PORT_STORM_TYPE  "StormType"
#define CFG_PORT_STORM_EDIS  "Enable"
#define CFG_STORM_RATE_LIMIT  "RateLimit"

#define STROM_PERCENT_IS_OUTOF_RANGE(p)  ((p)>100)?1:0

typedef enum BRG_STROM_LIMIT_s{
	STROM_BCAST_LIMIT = 0,
	STROM_MCAST_LIMIT = 1,
	STROM_DLF_LIMIT = 2,
	STROM_END_LIMIT 
}BRG_STROM_LIMIT_e;

/*******************************************************************************
* odmStormCtrlFrameSet
*
* DESCRIPTION:
* this function is used to set the strom config info 
*	 
* INPUTS:
*           portId:1--4
*		stormLimitTYpe: 0 for broadcast,1 for multicast while 2 for dlf 
* 		limitEnable : 1 for enable 0 for disable
*
* OUTPUTS: 
*		no 
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
UINT32 odmStormCtrlFrameSet(UINT32 portId,UINT32 stormLimitType, UINT32 limitEnable);

/*******************************************************************************
* odmStormCtrlFrameGet
*
* DESCRIPTION:
* this function is used get the bridge storm config info
*	 
* INPUTS:
*           portId:1---4
*		stormLimitTYpe: 0 for broadcast,1 for multicast while 2 for dlf 
* 		limitEnable : 1 for enable 0 for disable
*
* OUTPUTS: 
*		no 
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
UINT32 odmStormCtrlFrameGet(UINT32 portId,UINT32 stormLimitType, UINT32 *limitEnable);

/*******************************************************************************
* odmStormCtrlRateSet
*
* DESCRIPTION:
* this function is used set the default config for bridge storm limit 
*	 
* INPUTS:
*		
*
* OUTPUTS: 
*		no 
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
UINT32 odmStormCtrlRateSet(UINT32 portId,UINT32 rateLimit);

UINT32 odmStormCtrlRateGet(UINT32 portId,UINT32 *rateLimit);

UINT32 odmPortStormCfgInit(void);


#ifdef __cplusplus
}
#endif

#endif

