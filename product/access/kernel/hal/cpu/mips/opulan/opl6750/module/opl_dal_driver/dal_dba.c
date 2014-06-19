/*
=============================================================================
     File Name: dal_dba.c

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

#include "dal_lib.h"

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
OPL_STATUS dalDbaReportModeSet(UINT32 mode)
{
    if(DAL_DBA_RPT_BASE_THRESHOLD != mode &&
        DAL_DBA_RPT_65535 != mode)
    {
        return OPL_ERR_INVALID_PARAMETERS;
    }
    
	return ponLastQsetModHwWrite(mode);
}

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
OPL_STATUS dalDbaReportModeGet(UINT32 *mode)
{
    UINT8       reportMode;
    OPL_STATUS  retVal = OPL_OK;
    
	retVal = ponLastQsetModHwRead(&reportMode);

    *mode = reportMode;

    return retVal;
}
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
OPL_STATUS dalDbaQueueNumSet(UINT8 qSetNum)
{
	UINT8 numOfQset;

	numOfQset = qSetNum - 1;
	return ponQsetNumHwWrite(numOfQset);
}
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
OPL_STATUS dalDbaQueueNumGet(UINT8 *qSetNum)
{
	UINT8 numOfQset;
	OPL_STATUS retVal = OPL_OK;

	retVal = ponQsetNumHwRead(&numOfQset);
	*qSetNum = numOfQset + 1;
	return retVal;
}
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
OPL_STATUS dalDbaQueueThresholdSet(UINT8 qSetNum,UINT8 qNum,UINT16 thresholdVal)
{
	return ponQsetThreshHoldHwWrite( qSetNum, qNum, thresholdVal);
}
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
OPL_STATUS dalDbaQueueThresholdGet(UINT8 qSetNum,UINT8 qNum,UINT16 *thresholdVal)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(thresholdVal == OPL_NULL)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	retVal = ponQsetThreshHoldHwRead( qSetNum, qNum, &regVal);

	*thresholdVal = regVal;

	return retVal;
}

