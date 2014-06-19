/*
=============================================================================
     file Name: dal_storm.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/13		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#ifndef DAL_STORM_H
#define DAL_STORM_H
OPL_STATUS dalStormCtrlFrameSet(UINT32 portId,UINT32 stormLimitType,UINT32 limitEnable);
OPL_STATUS dalStormCtrlFrameGet(UINT32 portId,UINT32 stormLimitType,UINT32 *limitEnable);
OPL_STATUS dalStormCtrlRateSet(UINT32 portId,UINT32 rateLimit);
OPL_STATUS dalStormCtrlRateGet(UINT32 portId,UINT32 *rateLimit);

OPL_STATUS dalStormClsRuleAdd(UINT32 stormLimitType, UP_DOWN_STREAM_e streamType);
OPL_STATUS dalStormClsRuleDel(UINT32 stormLimitType, UP_DOWN_STREAM_e streamType);

#endif
