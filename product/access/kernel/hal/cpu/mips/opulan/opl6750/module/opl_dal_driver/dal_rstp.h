/*
=============================================================================
     Header Name: dal_mac.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 jiangmingli	   2008/10/10		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifndef _DAL_RSTP_H_
#define _DAL_RSTP_H_

#include "opl_driver.h"
#include "opconn_api.h"

#ifndef INOUT
#define IN      /* consider as comments near 'input' parameters */
#define OUT     /* consider as comments near 'output' parameters */
#define INOUT   /* consider as comments near 'input/output' parameters */
#endif

/*******************************************************************************
* dalRstpModeSet
*
* DESCRIPTION:
*  		this function is used to enable or disable rstp globally
*
*	INPUTS:
*		bEnable: 		enable or disable rstp
*
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalRstpModeSet(IN OPL_BOOL bEnable);

/*******************************************************************************
* dalRstpPortStateGet
*
* DESCRIPTION:
*  		this function is used to get port state of ulPortId
*
*	INPUTS:
*		ulPortId: 		logical portid
*
*	OUTPUTS:
*		pucPortState:	port state
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalRstpPortStateGet(IN UINT32 ulPortId, OUT UINT8 *pucPortState);

/*******************************************************************************
* dalRstpPortStateSet
*
* DESCRIPTION:
*  		this function is used to set port state of ulPortId
*
*	INPUTS:
*		ulPortId: 		logical portid
*		ucPortState:	port state
*
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalRstpPortStateSet(IN UINT32 ulPortId, IN UINT8 ucPortState);

/*******************************************************************************
* dalRstpPortMacFlush
*
* DESCRIPTION:
*  		this function is used to flush mac of only this port or all except this port
*
*	INPUTS:
*		ulPortid: 		port id
*		bOnlyThisPort	1 --- only this port
*					0 --- all except this port
*
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalRstpPortMacFlush(UINT32 ulPortid, BOOL_T bOnlyThisPort);


#endif
