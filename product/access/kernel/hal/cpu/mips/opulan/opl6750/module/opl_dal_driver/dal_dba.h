/*
=============================================================================
     Header File Name: dal_dba.h

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
#ifndef DAL_DBA_H
#define DAL_DBA_H
#include "opl_driver.h"
#include "opconn_api.h"

typedef enum DAL_DBA_RPT_MODE_s
{
    DAL_DBA_RPT_65535 = 0,
    DAL_DBA_RPT_BASE_THRESHOLD = 1,    
}DAL_DBA_RPT_MODE_e;

/*******************************************************************************
* dalDbaReportModeSet
*
* DESCRIPTION:
*		this function is used to set the report mode
*	INPUT:
*		mode: 0,last queue set report 
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalDbaReportModeSet(UINT32 mode);

/*******************************************************************************
* dalDbaReportModeGet
*
* DESCRIPTION:
*		this function is used to set the report mode
*	INPUT:
*		mode: 0,last queue set report 
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalDbaReportModeGet(UINT32 *mode);

/*******************************************************************************
* dalDbaQueueNumSet
*
* DESCRIPTION:
*		opconn support 2 queue sets,this function is uset to set the qSetNum of the system.
*	INPUT:
*		qSetNum:0:for 1,1 for 2
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalDbaQueueNumSet(UINT8 qSetNum);

/*******************************************************************************
* dalDbaQueueNumGet
*
* DESCRIPTION:
*		opconn support 2 queue sets,this function is uset to get the qSetNum of the system.
*	INPUT:
*		
*
* OUTPUTS:
*   	qSetNum:0:for 1,1 for 2
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalDbaQueueNumGet(UINT8 *qSetNum);

/*******************************************************************************
* dalDbaQueueThresholdSet
*
* DESCRIPTION:
*		opconn support 2 queue sets,this function is uset to set the threshold for the specified queue in the
*		qset.
*
*	INPUT:
*		qSetNum: the qSetNum (0-1)	
*		qNum:		 the queue(0-7)
*		thresholdVal: the threshhold.
* OUTPUTS:
*   	qSetNum:0:for 1,1 for 2
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalDbaQueueThresholdSet(UINT8 qSetNum,UINT8 qNum,UINT16 thresholdVal);

/*******************************************************************************
* dalDbaQueueThresholdGet
*
* DESCRIPTION:
*		opconn support 2 queue sets,this function is uset to get the qSetNum of the system.
*	INPUT:
*		qSetNum
*		qNum
* OUTPUTS:
*   	thresholdVal
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalDbaQueueThresholdGet(UINT8 qSetNum,UINT8 qNum,UINT16 *thresholdVal);

#endif
