/*
=============================================================================
     Header Name: pon.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/10/10		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/random.h>
#include <linux/uaccess.h>


#include "opl_driver.h"
#include "opconn_lib.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"
#include "hw_interface.h"
#include "pon.h"
#include "tm.h"

UINT8	oam_src_mac[6] = {0x00, 0x07, 0x49, 0x08, 0x30, 0xc8};

UINT8 discAgentAckAutoEnable = 1;

UINT8 discAgentRegAutoEnable = 1;

UINT8 discAgentPermitRegister =  1;

UINT8 *mpcpEventDisc[EVENT_END + 1] = 
{
	"Begin (initialization)",
	"Register event from discovery agent.",
	"Inside Discovery Window.",
	"Deregister event from discovery agent and not in discovery window.",
	"OLT permit register and not in discovery window.",
	"OLT deny register and not in discovery window.",
	"New discovery window.",
	"Permit ACK event from discovery agent.",
	"Deny ACK event from discovery agent.",
	"Reregister event from OLT.",
	"Registered and timestamp drift event.",
	"Deregister event from OLT.",
	"Deregister event from discovery agent when ONU is registered.",
	"WATCHDOG_TIMEOUT"	,
	OPL_NULL
};

UINT8 *mpcpStateDisc[STATE_END + 1] = 
{
	"WAIT.",
	"REGISTERING.",
	"REGISTER_REQUEST.",
	"REGISTER_PENDING.",
	"DENIED.",
	"RETRY.",
	"REGISTER_ACK.",
	"NACK.",
	"REGISTERD.",
	"REMOTE_DEREGISTER.",
	"LOCAL_DEREGISTER.",
	"WATCHDOG_TIMEOUT.",
	OPL_NULL
};
#if 1
#define PON_DEBUG   1
#endif

UINT8 ponResetBufferEnable = 1;

/*******************************************************************************
* ponForceReportTxEnHwWrite
*
* DESCRIPTION:
*	this function is used to enabl tx force report
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponForceReportTxEnHwWrite(UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	retVal = oplRegFieldWrite(REG_GB_PONTX_MODE, 1, 1, regVal);
	return retVal;
}
/*******************************************************************************
* ponForceReportTxEnHwRead
*
* DESCRIPTION:
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponForceReportTxEnHwRead(UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	retVal = oplRegFieldRead(REG_GB_PONTX_MODE, 1, 1, &regVal);
	*enable = regVal?OPL_ENABLE:OPL_DISABLE;
	return retVal;
}
/*******************************************************************************
* ponBypassModeEnHwWrite
*
* DESCRIPTION:
*	this function is used to enabl by pass mode
*
* INPUTS: 
*	enable: 0 disab while 1 enable
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponBypassModeEnHwWrite(UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = enable==0?OPL_DISABLE:OPL_ENABLE;
	retVal = oplRegFieldWrite(REG_GB_PONTX_MODE, 0, 1, regVal);
	return retVal;
}
/*******************************************************************************
* ponBypassModeEnHwRead
*
* DESCRIPTION:
*	this function is used to get the bypass mode status (enable or disable)
*
* INPUTS: 
*		
* OUTPUTS: 
*		enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponBypassModeEnHwRead(UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	retVal = oplRegFieldRead(REG_GB_PONTX_MODE, 0, 1, &regVal);
	*enable = regVal?OPL_DISABLE:OPL_ENABLE;
	return retVal;
}
/*******************************************************************************
* ponQsetNumHwWrite
*
* DESCRIPTION:
*	this function is used to the queue set num for report
*
* INPUTS: 
*		qSetNum: 0, 1, 2,3
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponQsetNumHwWrite(UINT8 qSetNum)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = qSetNum;
	
	retVal = oplRegFieldWrite(REG_GB_QSET_NUM, 0, 2, regVal);
	return retVal;
}
/*******************************************************************************
* ponQsetNumHwRead
*
* DESCRIPTION:
*	this function is used to get the queue set num report support
*
* INPUTS: 
*	
* OUTPUTS: 
*		qSetNum:0,1,2,3
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponQsetNumHwRead(UINT8 *qSetNum)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == qSetNum)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	retVal = oplRegFieldRead(REG_GB_QSET_NUM, 0, 2, &regVal);
	*qSetNum = regVal;
	return retVal;
}
/*******************************************************************************
* ponNullReportIntervalHwWrite
*
* DESCRIPTION:
*	this function is used to set the null report interval
*
* INPUTS: 
*	nullReportInterval:
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponNullReportIntervalHwWrite(UINT32 nullReportInterval)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = nullReportInterval;
	
	retVal = oplRegFieldWrite(REG_GB_REPORT_TIMERVALUE, 0, 32, regVal);
	return retVal;
}
/*******************************************************************************
* ponNullReportIntervalHwRead
*
* DESCRIPTION:
*	this function is used to get the nullReportInterval
*
* INPUTS: 
*	
* OUTPUTS: 
*		nullReportInterval
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponNullReportIntervalHwRead(UINT32 *nullReportInterval)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == nullReportInterval)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	retVal = oplRegFieldRead(REG_GB_REPORT_TIMERVALUE, 0, 32, &regVal);
	*nullReportInterval = regVal;
	return retVal;
}
/*******************************************************************************
* ponBypassModeSyncTimeHwWrite
*
* DESCRIPTION:
*	this function is used set the synctime of the bypass mode
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponBypassModeSyncTimeHwWrite(UINT16 syncTime)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = syncTime;
	
	retVal = oplRegFieldWrite(REG_GB_SYNCTIME, 0, 16, regVal);
	return retVal;
}
/*******************************************************************************
* ponBypassModeSyncTimeHwRead
*
* DESCRIPTION:
*	this function is used to get the sync time of bypass mode
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponBypassModeSyncTimeHwRead(UINT16 *syncTime)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == syncTime)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	retVal = oplRegFieldRead(REG_GB_SYNCTIME, 0, 16, &regVal);
	*syncTime = regVal;
	return retVal;
}
/*******************************************************************************
* ponBypassModeLlidHwWrite
*
* DESCRIPTION:
*	this function is used to set the llid for bypassmode
*
* INPUTS: 
*		llid:
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponBypassModeLlidHwWrite(UINT16 llid)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = llid;
	
	retVal = oplRegFieldWrite(REG_GB_LLID, 0, 16, regVal);
	return retVal;
}
/*******************************************************************************
* ponBypassModeLlidHwRead
*
* DESCRIPTION:
*	this function is used to get the llid for bypass mode
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponBypassModeLlidHwRead(UINT16 *llid)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == llid)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	retVal = oplRegFieldRead(REG_GB_LLID, 0, 16, &regVal);
	*llid = regVal;
	return retVal;
}
/*******************************************************************************
* ponSyncTimeHwRead
*
* DESCRIPTION:
*	this function is used to get sync time learning form olt
*
* INPUTS: 
*	
* OUTPUTS: 
*		syncTime
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponSyncTimeHwRead(UINT32 *syncTime)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == syncTime)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	retVal = oplRegFieldRead(REG_MPCP_SYNCTIME, 0, 16, &regVal);
	*syncTime = regVal;
	return retVal;
}
/*******************************************************************************
* ponLlidHwRead
*
* DESCRIPTION:
*	this function is used to onu's llid assigned by olt
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponLlidHwRead(UINT32 *LLID)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == LLID)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	retVal = oplRegFieldRead(REG_MPCP_LLID, 0, 16, &regVal);
	*LLID = regVal;
	return retVal;
}
/*******************************************************************************
* ponAesEnHwWrite
*
* DESCRIPTION:
*	this function is used to enable or disable the aes security
*
* INPUTS: 
*		enable:0 for disable while 1 for disable
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponAesEnHwWrite(UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	
	retVal = oplRegFieldWrite(REG_TFA_EN, 3, 1, regVal);
	return retVal;
}
/*******************************************************************************
* ponAesEnHwRead
*
* DESCRIPTION:
*	this function is used to get the aes config info,enable or disable
*
* INPUTS: 
*	
* OUTPUTS: 
*		enable: 0 for disable 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponAesEnHwRead(UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = oplRegFieldRead(REG_TFA_EN, 3, 1, &regVal);
	*enable = regVal;
	return retVal;
}
/*******************************************************************************
* ponTripleChuningEnHwWrite
*
* DESCRIPTION:
 *	this function is used to enabl or disable triplechuning.
 *
* INPUTS: 
*		enable: 0 for disable, 1 for enable
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponTripleChuningEnHwWrite(UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	
	retVal = oplRegFieldWrite(REG_TFA_EN, 2, 1, regVal);
	return retVal;
}
/*******************************************************************************
* ponTripleChuningEnHwRead
*
* DESCRIPTION:
*	this function is used to triplechuning config info,enable or disable
*
* INPUTS: 
*	
* OUTPUTS: 
*		enable:0 for disable 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponTripleChuningEnHwRead(UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = oplRegFieldRead(REG_TFA_EN, 2, 1, &regVal);
	*enable = regVal;
	return retVal;
}
/*******************************************************************************
* ponFecRxEnHwWrite
*
* DESCRIPTION:
*	this function is used to enabl or disable fec in rx
*
* INPUTS: 
*		enable: 0 for disable while 1 for enable
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponFecRxEnHwWrite(UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	
	retVal = oplRegFieldWrite(REG_TFA_EN, 1, 1, regVal);
	return retVal;
}
/*******************************************************************************
* ponFecRxEnHwRead
*
* DESCRIPTION:
*	this function is used to get fec config info in rx.
*
* INPUTS: 
*	
* OUTPUTS: 
*		enable: 1 for enable, 0 for disable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponFecRxEnHwRead(UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = oplRegFieldRead(REG_TFA_EN, 0, 1, &regVal);
	*enable = regVal;
	return retVal;
}
/*******************************************************************************
* ponFecTxEnHwWrite
*
* DESCRIPTION:
*	this function is used to enable or disable fec in tx.
*
* INPUTS: 
*		enable : 0 for disable,1 for enable
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponFecTxEnHwWrite(UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	
	retVal = oplRegFieldWrite(REG_TFA_EN, 0, 1, regVal);
	return retVal;
}
/*******************************************************************************
* ponFecTxEnHwRead
*
* DESCRIPTION:
*	this function is used to get fec config info .
*
* INPUTS: 
*	
* OUTPUTS: 
*		enable:0 for disalbe 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponFecTxEnHwRead(UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = oplRegFieldRead(REG_TFA_EN, 1, 1, &regVal);
	*enable = regVal;
	return retVal;
}

/*******************************************************************************
* ponSerdesDelayHwWrite
*
* DESCRIPTION:
*	this function is used to set the serdes delay count
*
* INPUTS: 
*		delayCount:
* OUTPUTS: 
*				
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponSerdesDelayHwWrite(UINT32 delayCount)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = delayCount;
	
	retVal = oplRegFieldWrite(REG_SERDES_DELAY, 0, 8, regVal);
	return retVal;
}
/*******************************************************************************
* ponSerdesDelayHwRead
*
* DESCRIPTION:
*	this function is used to get the serdes delayCount
*
* INPUTS: 
*	
* OUTPUTS: 
*		delayCount
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponSerdesDelayHwRead(UINT32 *delayCount)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == delayCount)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = oplRegFieldRead(REG_SERDES_DELAY, 0, 8, &regVal);
	*delayCount = regVal;
	return retVal;
}
/*******************************************************************************
* ponReportDisableHwWrite
*
* DESCRIPTION:
*	this function is used to enable or disable the report 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponReportDisableHwWrite(UINT8 enable)
{
	UINT32 regVal;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;

	return oplRegFieldWrite(REG_RPT_MODE, 1, 1, regVal);
}
/*******************************************************************************
* ponReportDisableHwRead
*
* DESCRIPTION:
*	this function is used to enabl or disable report
*
* INPUTS: 
*	
* OUTPUTS: 
*		enalbe: 0 for disable, 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponReportDisableHwRead(UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	regVal = oplRegFieldRead(REG_RPT_MODE, 1, 1, &regVal);

	*enable = regVal;

	return retVal;
}

/*******************************************************************************
* ponLastQsetModHwWrite
*
* DESCRIPTION:
*  	this function is used to set the last queue report mode.
*	0 : the last queue set report the total packet length in the queue or 65536.
*	1 : the last queue set is the packet length calculated according to threshold register.
*
* INPUTS: 
*		mode:
*			0 : the last queue set report the total packet length in the queue or 65536.
*			1 : the last queue set is the packet length calculated according to threshold register.
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponLastQsetModHwWrite(UINT8 mode)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal ;
	
	regVal =  mode;
	retVal = oplRegFieldWrite(REG_RPT_MODE, 0, 1, regVal);
	return retVal;
}
/*******************************************************************************
* ponLastQsetModHwRead
*
* DESCRIPTION:
*  	this function is used to set the last queue report mode.
*	0 : the last queue set report the total packet length in the queue or 65536.
*	1 : the last queue set is the packet length calculated according to threshold register.
*
* INPUTS: 
*		mode:
*			0 : the last queue set report the total packet length in the queue or 65536.
*			1 : the last queue set is the packet length calculated according to threshold register.
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponLastQsetModHwRead(UINT8 *mode)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == mode)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	regVal = oplRegFieldRead(REG_RPT_MODE, 0, 1, &regVal);

	*mode = regVal;

	return retVal;
}
/*******************************************************************************
* ponQsetThreshHoldHwWrite
*
* DESCRIPTION:
*	this function is used to set the threshold of queue
*
* INPUTS: 
*		qNum: 			0 for queueu set 1, 1 for queue set 2.
*		queue: 		the queue in queue set (qNum) will be reported. (0-7)
*		threshold:	0 - 0xffff
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponQsetThreshHoldHwWrite(UINT8 qNum,UINT8 queue,UINT32 thresholdVal)
{
	UINT32 retVal = OPL_OK;
	UINT32 regAddr;

	if(qNum >= OPL_MAX_QUEUE_SET3)
	{
		OPL_DRV_PRINTF(("this chip only support 2 queuesets.\n"));
		return OPL_OK;
	}

	if(queue >= 8)
	{
		OPL_DRV_PRINTF(("this chip only support 8 queue.\n"));
		return OPL_OK;
	}
	
	regAddr = REG_RPT_Q0_THR0;
	regAddr +=((qNum*8 +queue)*4);

	retVal = oplRegFieldWrite(regAddr,0,16,thresholdVal);

	return retVal;
}
/*******************************************************************************
* ponQsetThreshHoldHwRead
*
* DESCRIPTION:
*	this function is used to get the threshold of queue
*
* INPUTS: 
*		qNum: 			0 for queueu set 1, 1 for queue set 2.
*		queue: 		the queue in queue set (qNum) will be reported. (0-7)
*		
* OUTPUTS: 
*		threshold:	0 - 0xffff
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponQsetThreshHoldHwRead(UINT8 qNum,UINT8 queue,UINT32 *thresholdVal)
{
	UINT32 retVal = OPL_OK;
	UINT32 regAddr;

	if(qNum >= OPL_MAX_QUEUE_SET3)
	{
		OPL_DRV_PRINTF(("this chip only support 2 queuesets.\n"));
		return OPL_OK;
	}

	if(queue >= 8)
	{
		OPL_DRV_PRINTF(("this chip only support 8 queue.\n"));
		return OPL_OK;
	}
	
	if(OPL_NULL == thresholdVal)
	{
		return OPL_ERR_NULL_POINTER;
	}

	regAddr = REG_RPT_Q0_THR0;
	regAddr +=((qNum*8 +queue)*4);

	retVal = oplRegFieldRead(regAddr,0,16,thresholdVal);

	return retVal;
}
/*******************************************************************************
* ponMacIdHwWrite
*
* DESCRIPTION:
*	this function is used to set the pon macid:
*
* INPUTS: 
*		mac:
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponMacIdHwWrite(UINT8 *mac)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regValH;
	UINT32 regValL;

	if(OPL_NULL == mac)
	{
		return OPL_ERR_NULL_POINTER;
	}
	regValH = mac[0]<<8|mac[1];
	regValL = mac[2]<<24|mac[3]<<16|mac[4]<<8|mac[5];
#ifdef PON_DEBUG
	OPL_DRV_PRINTF(("regValL = %08x\n",regValL));
	OPL_DRV_PRINTF(("regValH = %08x\n",regValH));
#endif
	retVal = oplRegFieldWrite(REG_MPCP_ONU_MACID0, 0, 32, regValL);
	retVal = oplRegFieldWrite(REG_MPCP_ONU_MACID1, 0, 16, regValH);
	return retVal;
}
/*******************************************************************************
* ponMacIdHwRead
*
* DESCRIPTION:
*	this function is used to enabl tx force report
*
* INPUTS: 
*	
* OUTPUTS: 
*		mac:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponMacIdHwRead(UINT8 *mac)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regValH;
	UINT32 regValL;

	if(OPL_NULL == mac)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = oplRegFieldRead(REG_MPCP_ONU_MACID0, 0, 32, &regValL);
	retVal = oplRegFieldRead(REG_MPCP_ONU_MACID1, 0, 16, &regValH);
#ifdef PON_DEBUG
	OPL_DRV_PRINTF(("regValL = %08x\n",regValL));
	OPL_DRV_PRINTF(("regValH = %08x\n",regValH));
#endif

	mac[2] = regValL>>24;
	mac[3] = regValL>>16;
	mac[4] = regValL>>8;
	mac[5] = regValL;
	
	mac[0] = regValH>>8;
	mac[1] = regValH;
	
	return retVal;
}
/*******************************************************************************
* ponLaserOnCntHwWrite
*
* DESCRIPTION:
*	this function is used set laser on count(tq)
*	
* INPUTS: 
*		count:
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponLaserOnCntHwWrite(UINT32 count)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = oplRegFieldWrite(REG_MPCP_GATE_LASER_ON, 0, 16, count);

	return retVal;
}
/*******************************************************************************
* ponLaserOnCntHwRead
*
* DESCRIPTION:
*	this function is used to get laserOn count
*
* INPUTS: 
*		
* OUTPUTS: 
*		count:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponLaserOnCntHwRead(UINT32 *count)
{
	OPL_STATUS retVal = OPL_OK;

	if(OPL_NULL == count)
	{
		OPL_DRV_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}
	
	retVal = oplRegFieldRead(REG_MPCP_GATE_LASER_ON, 0, 16, count);

	return retVal;
}
/*******************************************************************************
* ponLaserOffCntHwWrite
*
* DESCRIPTION:
*	this function is used to set laseroff count (tq)
*
* INPUTS: 
*		count:
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponLaserOffCntHwWrite(UINT32 count)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = oplRegFieldWrite(REG_MPCP_GATE_LASER_OFF, 0, 16, count);

	return retVal;
}

/*******************************************************************************
* ponMpcpFsmHoldControlHwWrite
*
* DESCRIPTION: 
*	this function is used to enable/disable mpcp fsm holdover feature
*
* INPUTS: 
*	driftHoldEn: permit hardware control MPCP ignore time stamp drift event
*	timerHoldEn: permit hardware control MPCP gate receive time counter hold to zero
*	grantFlushEn: permit hardware remove all grant in grant list hwne hold is processing
*	losPosEn: LOS polarity
*	losHoldEn: permit hold process when LOS event occur, when softHoldEn is 0
*	linkHoldEn: permit hold process when LINK sync is failed and softHoldEn is 0
* 	softHoldEn: 1-software control hold time 0-hardware control hold time
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponMpcpFsmHoldControlHwWrite(UINT8 driftHoldEn,
										UINT8 timerHoldEn,
										UINT8 grantFlushEn,
										UINT8 losPosEn,
										UINT8 losHoldEn,
										UINT8 linkHoldEn,
									    UINT8 softHoldEn)
{
	OPL_STATUS retVal = OPL_OK;
	PON_MPCP_FSM_HOLD_CTRL_t ponFsmCtrl;

    ponFsmCtrl.driftHoldEn = driftHoldEn;
	ponFsmCtrl.timerHoldEn = timerHoldEn;
	ponFsmCtrl.grantFlushEn = grantFlushEn;
	ponFsmCtrl.losPosEn = losPosEn;
	ponFsmCtrl.losHoldEn = losHoldEn;
	ponFsmCtrl.linkHoldEn = linkHoldEn;
	ponFsmCtrl.softHoldEn = softHoldEn;

	retVal = oplRegWrite(REG_MPCP_FSM_HOLD_CTRL, *((UINT32 *)&ponFsmCtrl));
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	return retVal;

}

/*******************************************************************************
* ponMpcpFsmHoldControlHwRead
*
* DESCRIPTION: 
*	this function is used to get enable status of mpcp fsm holdover feature
*
* INPUTS: 
*	
* OUTPUTS: 
*	driftHoldEn: permit hardware control MPCP ignore time stamp drift event
*	timerHoldEn: permit hardware control MPCP gate receive time counter hold to zero
*	grantFlushEn: permit hardware remove all grant in grant list hwne hold is processing
*	losPosEn: LOS polarity
*	losHoldEn: permit hold process when LOS event occur, when softHoldEn is 0
*	linkHoldEn: permit hold process when LINK sync is failed and softHoldEn is 0
* 	softHoldEn: 1-software control hold time 0-hardware control hold time
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponMpcpFsmHoldControlHwRead(UINT8 *driftHoldEn,
										UINT8 *timerHoldEn,
										UINT8 *grantFlushEn,
										UINT8 *losPosEn,
										UINT8 *losHoldEn,
										UINT8 *linkHoldEn,
									    UINT8 *softHoldEn)
{
	OPL_STATUS retVal = OPL_OK;
	PON_MPCP_FSM_HOLD_CTRL_t ponFsmCtrl;

    if(NULL == driftHoldEn
        ||NULL == timerHoldEn
        ||NULL == grantFlushEn
        ||NULL == losPosEn
        ||NULL == losHoldEn
        ||NULL == linkHoldEn
        ||NULL == softHoldEn)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
	retVal = oplRegRead(REG_MPCP_FSM_HOLD_CTRL, (UINT32 *)&ponFsmCtrl);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

    *driftHoldEn = ponFsmCtrl.driftHoldEn;
	*timerHoldEn = ponFsmCtrl.timerHoldEn;
	*grantFlushEn = ponFsmCtrl.grantFlushEn;
	*losPosEn = ponFsmCtrl.losPosEn;
	*losHoldEn = ponFsmCtrl.losHoldEn;
	*linkHoldEn = ponFsmCtrl.linkHoldEn;
	*softHoldEn = ponFsmCtrl.softHoldEn;

	return retVal;

}

/*******************************************************************************
* ponMpcpHoldTimeHwRead
*
* DESCRIPTION:
*	this function is used to set mpcp fsm hold timeout value
*
* INPUTS: 
*	holdTime:hardware control hold time
* 
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponMpcpFsmHoldTimeHwWrite(UINT32 holdTime)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = oplRegFieldWrite(REG_MPCP_FSM_HOLD_TIME, 0, 32, holdTime);
	
	return retVal;
}

/*******************************************************************************
* ponMpcpHoldTimeHwRead
*
* DESCRIPTION:
*	this function is used to get mpcp fsm hold timeout value
*
* INPUTS: 
*	
* OUTPUTS: 
*	holdTime: hardware control hold  time
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponMpcpFsmHoldTimeHwRead(UINT32 *holdTime)
{
	OPL_STATUS retVal = OPL_OK;
	
	if(OPL_NULL == holdTime)
	{
		OPL_DRV_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}
	
	retVal = oplRegFieldRead(REG_MPCP_FSM_HOLD_TIME, 0, 32, holdTime);

	
	return retVal;
}


/*******************************************************************************
* ponForceReportTxEnHwWrite
*
* DESCRIPTION:
*	this function is used to get laseroff count
*
* INPUTS: 
*	
* OUTPUTS: 
*		count:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponLaserOffCntHwRead(UINT32 *count)
{
	OPL_STATUS retVal = OPL_OK;
	
	if(OPL_NULL == count)
	{
		OPL_DRV_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}
	retVal = oplRegFieldRead(REG_MPCP_GATE_LASER_OFF, 0, 16, count);

	return retVal;
}

/*******************************************************************************
* ponOamFinishedHwWrite
*
* DESCRIPTION:
*	this function is used to set the oam finished register
*
* INPUTS: 
*	oamFinished:0 for not finished while 1 for finished
* OUTPUTS: 
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponOamFinishedHwWrite(UINT32 oamFinished)
{
	return oplRegFieldWrite(REG_CP_OAM_REGI_CTRL,0,1,oamFinished);
}

/*******************************************************************************
* ponActiveLedEnHwWrite
*
* DESCRIPTION:
*	this function is used to set the led wether or not cotrolled by sw.
*   enable:0 controle by hw,1 control by sw
*
* INPUTS: 
*	enable:
* OUTPUTS: 
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponActiveLedEnHwWrite(UINT32 enable)
{
    return oplRegFieldWrite(REG_CP_LED_CTRL,9,1,enable);	
}

/*******************************************************************************
* ponActiveLedOnOffHwWrite
*
* DESCRIPTION:
*	when active led controlled by sw,set this value will turn on/off the active 
*   led
*
* INPUTS: 
*	onOff:0 for off,while 1 for on
* OUTPUTS: 
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponActiveLedOnOffHwWrite(UINT32 onOff)
{
    return oplRegFieldWrite(REG_CP_LED_CTRL,8,1,onOff);		
}
/*******************************************************************************
* ponLinkLedEnHwWrite
*
* DESCRIPTION:
*	this function is used to set the led wether or not cotrolled by sw.
*   enable:0 controle by hw,1 control by sw
*
* INPUTS: 
*	enable:
* OUTPUTS: 
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponLinkLedEnHwWrite(UINT32 enable)
{
    return oplRegFieldWrite(REG_CP_LED_CTRL,1,1,enable);		
}

/*******************************************************************************
* ponActiveLedOnOffHwWrite
*
* DESCRIPTION:
*	when active led controlled by sw,set this value will turn on/off the active 
*   led
*
* INPUTS: 
*	onOff:0 for off,while 1 for on
* OUTPUTS: 
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponLinkLedOnOffHwWrite(UINT32 onOff)
{
    return oplRegFieldWrite(REG_CP_LED_CTRL,0,1,onOff);	
}

/*******************************************************************************
* ponLinkLedBlinkModeHwWrite
*
* DESCRIPTION:
*	this function is used to set link led mode.
*
* INPUTS: 
*	mode:
* OUTPUTS: 
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponLinkLedBlinkModeHwWrite(UINT32 mode)
{
    return oplRegFieldWrite(REG_LED_REGI_BLINK_CFG,16,3,mode);		
}

/*******************************************************************************
* ponLinkLedBlinkStepHwWrite
*
* DESCRIPTION:
*	this function is used to set link led's blink mode
*
* INPUTS: 
*	step:
* OUTPUTS: 
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponLinkLedBlinkStepHwWrite(UINT32 step)
{
    return oplRegFieldWrite(REG_LED_REGI_BLINK_CFG,0,16,step);			
}

/*******************************************************************************
* ponActiveLedBlinkModeHwWrite
*
* DESCRIPTION:
*	this function is used to set link led mode.
*
* INPUTS: 
*	mode:
* OUTPUTS: 
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponActiveLedBlinkModeHwWrite(UINT32 mode)
{
    return oplRegFieldWrite(REG_LED_TRAF_BLINK_CFG,16,3,mode);	
}

/*******************************************************************************
* ponActiveLedBlinkStepHwWrite
*
* DESCRIPTION:
*	this function is used to set link led's blink mode
*
* INPUTS: 
*	step:
* OUTPUTS: 
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponActiveLedBlinkStepHwWrite(UINT32 step)
{
    return oplRegFieldWrite(REG_LED_TRAF_BLINK_CFG,0,16,step);		
}
/*******************************************************************************
* ponMpcpProcess
*
* DESCRIPTION:
*	
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
void ponMpcpProcess(void)
{
	PON_MPCP_DISC_STATUS_t ponMpcpState;
	
	while(1)
	{
		switch(ponMpcpState.bfDiscCState)
		{
			case STATE_WAIT:
				if(discAgentRegAutoEnable != 1)
				{
					oplRegFieldWrite(REG_MPCP_DISC_CTRL,0, 2, 2);
				}
				break;
			case STATE_REGISTER_PENDING:
				if(discAgentAckAutoEnable != 1)
				{
					oplRegFieldWrite(REG_MPCP_DISC_CTRL,4, 2, 1);
					/* set to ack */
				}
				break;
			case STATE_DENIED:
			case STATE_RETRY:
			case STATE_REGISTER_ACK:
			case STATE_NACK:
			case STATE_REGISTERD:
			case STATE_REMOTE_DEREGISTER:
			case STATE_LOCAL_DEREGISTER:
			case STATE_WATCHDOG_TIMEOUT:
			default:
				break;
		}
	}
}
/*******************************************************************************
* ponMacIdSetTest
*
* DESCRIPTION:
*	this function is used set the mac id
*
* INPUTS: 
*		macString: xx:xx:xx:xx:xx:xx
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponMacIdSetTest(UINT8 *macString)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 mac[MAC_LENGTH];

	if(OPL_NULL == macString)
	{
		return OPL_ERR_NULL_POINTER;
	}

	string2mac(macString, mac);

	OPL_MEMCPY(&oam_src_mac[0],&mac[0],6);
	
	retVal = ponMacIdHwWrite(mac);
	return retVal;
}

/*******************************************************************************
* ponDelayConfigAllSet
*
* DESCRIPTION:
*	this function is used to enabl tx force report
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponDelayConfigAllSet(UINT8 cpTxDlyEn,UINT8 cpSyncEn,UINT16 cpRptsentDelt,UINT16 cpTxDly)
{
	OPL_STATUS retVal = OPL_OK;
	PON_DLY_CONF_t ponDlyConfVal;

    ponDlyConfVal.cpTxDlyEn = cpTxDlyEn;
	ponDlyConfVal.cpSyncEn = cpSyncEn;
	ponDlyConfVal.cpRptsentDelt = cpRptsentDelt;
	ponDlyConfVal.cpTxDly = cpTxDly;

	retVal = oplRegWrite(REG_MPCP_TX_DLY, *((UINT32 *)&ponDlyConfVal));
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	return retVal;
}

/*******************************************************************************
* ponDelayConfigAllGet
*
* DESCRIPTION:
*	this function is used to enabl tx force report
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponDelayConfigAllGet(UINT32 *cpTxDlyEn,UINT32 *cpSyncEn,UINT32 *cpRptsentDelt,UINT32 *cpTxDly)
{
	OPL_STATUS retVal = OPL_OK;
	PON_DLY_CONF_t ponDlyConfVal;

    if(NULL == cpTxDly
        ||NULL == cpSyncEn
        ||NULL == cpRptsentDelt
        ||NULL == cpTxDlyEn)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
	retVal = oplRegRead(REG_MPCP_TX_DLY, (UINT32 *)&ponDlyConfVal);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

    *cpTxDlyEn = ponDlyConfVal.cpTxDlyEn;
	*cpSyncEn = ponDlyConfVal.cpSyncEn;
	*cpRptsentDelt = ponDlyConfVal.cpRptsentDelt;
	*cpTxDly = ponDlyConfVal.cpTxDly;

	return retVal;
}

/*******************************************************************************
* ponMpcpDbaAgetConfigAllSet
*
* DESCRIPTION:
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponMpcpDbaAgetConfigAllSet(
	UINT32 cpRptStaMode,
    UINT32 cpMergeRptSendMode,
    UINT32 cpRptSendMode,
    UINT32 cpPktInitTimerCfg,
	UINT32 cpDeltaTxNormalNoFecTime,
	UINT32 cpTxMpcpNoFecTime
    )
{
	OPL_STATUS retVal = OPL_OK;
	PON_DBA_AGETN_CONF_t ponDbaAgentConf;
    
	ponDbaAgentConf.cpRptStaMode =             cpRptStaMode ;           
    ponDbaAgentConf.cpMergeRptSendMode =       cpMergeRptSendMode ;     
    ponDbaAgentConf.cpRptSendMode =            cpRptSendMode ;           
    ponDbaAgentConf.cpPktInitTimerCfg =        cpPktInitTimerCfg ;      
    ponDbaAgentConf.cpDeltaTxNormalNoFecTime = cpDeltaTxNormalNoFecTime ;
    ponDbaAgentConf.cpTxMpcpNoFecTime        = cpTxMpcpNoFecTime;         

	retVal = oplRegWrite(REG_MPCP_DBA_AGENT_CFG, *((UINT32 *)&ponDbaAgentConf));
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	return retVal;
}

/*******************************************************************************
* ponMpcpDbaAgetConfigAllGet
*
* DESCRIPTION:
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponMpcpDbaAgetConfigAllGet(
	UINT32 *cpRptStaMode,
    UINT32 *cpMergeRptSendMode,
    UINT32 *cpRptSendMode,
    UINT32 *cpPktInitTimerCfg,
	UINT32 *cpDeltaTxNormalNoFecTime,
	UINT32 *cpTxMpcpNoFecTime
    )
{
	OPL_STATUS retVal = OPL_OK;
	PON_DBA_AGETN_CONF_t ponDbaAgentConf;

    if(NULL == cpRptStaMode
        ||NULL == cpMergeRptSendMode
        ||NULL == cpRptSendMode
        ||NULL == cpPktInitTimerCfg
    	||NULL == cpDeltaTxNormalNoFecTime
    	||NULL == cpTxMpcpNoFecTime)
    {
        return OPL_ERR_NULL_POINTER;
    }

    retVal = oplRegRead(REG_MPCP_DBA_AGENT_CFG, (UINT32 *)&ponDbaAgentConf);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
    
	*cpRptStaMode =                ponDbaAgentConf.cpRptStaMode;                        
    *cpMergeRptSendMode =          ponDbaAgentConf.cpMergeRptSendMode;                  
    *cpRptSendMode =               ponDbaAgentConf.cpRptSendMode;                       
    *cpPktInitTimerCfg =           ponDbaAgentConf.cpPktInitTimerCfg;                   
	*cpDeltaTxNormalNoFecTime =    ponDbaAgentConf.cpDeltaTxNormalNoFecTime;            
	*cpTxMpcpNoFecTime =           ponDbaAgentConf.cpTxMpcpNoFecTime;                   

	return retVal;
}

/*******************************************************************************
* ponSecuConfigHwWrite
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/

OPL_STATUS ponSecuConfigHwWrite(UINT8 type,UINT32 value)
{
	PON_SECU_CFG_t ponSecuCfg;
	OPL_STATUS  retVal = OPL_OK;

	retVal = oplRegRead(REG_PMAC_SECU_CFG, (UINT32 *)&ponSecuCfg);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	switch(type)
	{
		case PON_SECU_SCB_EN:
			ponSecuCfg.bfSecScbEn = value;
			break;
		case PON_SECU_UNICAST_EN:
			ponSecuCfg.bfSecUnicastEn = value;
			break;
		case PON_SECU_SCB_MODE:
			ponSecuCfg.bfSecScbMode	= value;
			break;
		case PON_SECU_UNICAST_MODE:
			ponSecuCfg.bfSecUnicastMode	= value;
			break;	
		case PON_SECU_AES_MODE:
			ponSecuCfg.bfAesMode	= value;
			break;	
		default:
			OPL_DRV_PRINTF(("no such type.\n"));
			OPL_LOG_TRACE();
			return OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	retVal = oplRegWrite(REG_PMAC_SECU_CFG, *(UINT32 *)&ponSecuCfg);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	return retVal;
}
/*******************************************************************************
* ponSecuConfigHwRead
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/

OPL_STATUS ponSecuConfigHwRead(UINT8 type,UINT32 *value)
{
	PON_SECU_CFG_t ponSecuCfg;
	OPL_STATUS  retVal = OPL_OK;\

	if(OPL_NULL == value)
	{
		OPL_DRV_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	retVal = oplRegRead(REG_PMAC_SECU_CFG, (UINT32 *)&ponSecuCfg);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	switch(type)
	{
		case PON_SECU_SCB_EN:
			*value = ponSecuCfg.bfSecScbEn;
			break;
		case PON_SECU_UNICAST_EN:
			*value = ponSecuCfg.bfSecUnicastEn;
			break;
		case PON_SECU_SCB_MODE:
			*value = ponSecuCfg.bfSecScbMode;
			break;
		case PON_SECU_UNICAST_MODE:
			*value = ponSecuCfg.bfSecUnicastMode;
			break;	
		case PON_SECU_AES_MODE:
			*value = ponSecuCfg.bfAesMode;
			break;	
		default:
			OPL_DRV_PRINTF(("no such type.\n"));
			OPL_LOG_TRACE();
			return OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
/*******************************************************************************
* ponSecuKeyGenerate
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponSecuKeyGenerate(UINT8 keyIndex)
{
	UINT32 keyGenConfig = OPL_ZERO;
	UINT32 timeOutVal = 1000;
	UINT32 keyGenBusy = OPL_ZERO;
	
	if(keyIndex > PON_SECU_KEY_1)
	{
		OPL_DRV_PRINTF(("no such index.\n"));
		return OPL_ERROR;
	}

	keyGenConfig = 1<<4|keyIndex;

	oplRegWrite(REG_PMAC_SECU_GEN,keyGenConfig);

	while(timeOutVal--)
	{
		oplRegFieldRead(REG_PMAC_SECU_GEN, 4, 1, &keyGenBusy);
		if(keyGenBusy != OPL_TRUE)
		{
			break;
		}
	}

	if(timeOutVal == OPL_ZERO)
	{
		OPL_DRV_PRINTF(("gerenate key timeout.\n"));
		return OPL_ERROR;
	}

	return OPL_OK;
}

/*******************************************************************************
* ponSecuNoCtcKeyGenerate
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponSecuNoCtcKeyGenerate(UINT8 keyIndex)
{
	static UINT8 randVecTor;
	
	UINT8 keyArry[5] = {0x00,0x5a,0xa5,0x66,0x99};
	union {
		UINT8 keyVal8[4];
		UINT32 keyVal32;
	}keyValUnion;

	randVecTor++;
	
	keyValUnion.keyVal8[0] = 0x00;
	srandom32(123+randVecTor*0X123);
	keyValUnion.keyVal8[1] = (random32()+randVecTor)&0xff;
	
	srandom32(238+randVecTor*458);
	keyValUnion.keyVal8[2] = keyArry[(random32()+randVecTor)%5];

	srandom32(0xffff+randVecTor*666);
	keyValUnion.keyVal8[3] = keyArry[(random32()+randVecTor)%5];

	if(keyIndex == 0)
	{
		oplRegWrite(REG_PMAC_SECU_GEN,0x100);
		oplRegWrite(REG_PMAC_SECU_KEY0_0,keyValUnion.keyVal32);
		
	}else if(keyIndex == 1)
	{
		oplRegWrite(REG_PMAC_SECU_GEN,0x100);
		oplRegWrite(REG_PMAC_SECU_KEY1_0,keyValUnion.keyVal32);
	}else
	{
		OPL_DRV_PRINTF(("only support two key.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	return OPL_OK;
}
/*******************************************************************************
* ponSecuKeyHwWrite
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponSecuKeyHwWrite(UINT8 channel,UINT8 secuMode,UINT8 keyIndex,UINT8 *pBuff)
{
	union keyBuffUnion{
		UINT32 aesKeyBuff[4];
		UINT8   tripleChurningKeyBuff[16];
	}keyBuff;

	if(pBuff == OPL_NULL)
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

	OPL_MEMSET(&keyBuff,0x0,sizeof(union keyBuffUnion));
	
	if(secuMode == PON_SECU_MODE_AES)
	{
		OPL_MEMCPY((UINT8 *)&(keyBuff.aesKeyBuff),pBuff,16);
	}else if(secuMode == PON_SECU_MODE_TRIPLE_CHURNING) 
	{
		OPL_MEMCPY((UINT8 *)&(keyBuff.tripleChurningKeyBuff[1]),pBuff,3);	
		keyBuff.tripleChurningKeyBuff[0] = OPL_ZERO;
	}else 
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}
	if(channel == PON_SECU_CHANNEL_SCB)
	{
		switch(secuMode)
		{
			case PON_SECU_MODE_AES:
				if(keyIndex == PON_SECU_KEY_0)
				{
					oplRegWrite(REG_PMAC_SECU_SCB_KEY0_0,keyBuff.aesKeyBuff[3]);
					oplRegWrite(REG_PMAC_SECU_SCB_KEY0_1,keyBuff.aesKeyBuff[2]);
					oplRegWrite(REG_PMAC_SECU_SCB_KEY0_2,keyBuff.aesKeyBuff[1]);
					oplRegWrite(REG_PMAC_SECU_SCB_KEY0_3,keyBuff.aesKeyBuff[0]);
				}else if(keyIndex == PON_SECU_KEY_1)
				{
					oplRegWrite(REG_PMAC_SECU_SCB_KEY1_0,keyBuff.aesKeyBuff[3]);
					oplRegWrite(REG_PMAC_SECU_SCB_KEY1_1,keyBuff.aesKeyBuff[2]);
					oplRegWrite(REG_PMAC_SECU_SCB_KEY1_2,keyBuff.aesKeyBuff[1]);
					oplRegWrite(REG_PMAC_SECU_SCB_KEY1_3,keyBuff.aesKeyBuff[0]);
				}else 
				{
					OPL_LOG_TRACE();
					return OPL_ERR_INVALID_PARAMETERS;
				}
				break;
			case PON_SECU_MODE_TRIPLE_CHURNING:
				if(keyIndex == PON_SECU_KEY_0)
				{
					oplRegFieldWrite(REG_PMAC_SECU_SCB_KEY0_0, 0, 24, keyBuff.aesKeyBuff[0]);
				}else if(keyIndex == PON_SECU_KEY_1)
				{
					oplRegFieldWrite(REG_PMAC_SECU_SCB_KEY1_0, 0, 24, keyBuff.aesKeyBuff[0]);
				}else 
				{
					OPL_LOG_TRACE();
					return OPL_ERR_INVALID_PARAMETERS;
				}
				break;
			default:
				OPL_LOG_TRACE();
				return OPL_ERR_INVALID_PARAMETERS;
				break;
		}
	}else if(channel == PON_SECU_CHANNEL_UNICAST)
	{
		switch(secuMode)
		{
			case PON_SECU_MODE_AES:
				if(keyIndex == PON_SECU_KEY_0)
				{
					oplRegWrite(REG_PMAC_SECU_KEY0_0,keyBuff.aesKeyBuff[3]);
					oplRegWrite(REG_PMAC_SECU_KEY0_0,keyBuff.aesKeyBuff[2]);
					oplRegWrite(REG_PMAC_SECU_KEY0_0,keyBuff.aesKeyBuff[1]);
					oplRegWrite(REG_PMAC_SECU_KEY0_0,keyBuff.aesKeyBuff[0]);
				}else if(keyIndex == PON_SECU_KEY_1)
				{
					oplRegWrite(REG_PMAC_SECU_KEY1_0,keyBuff.aesKeyBuff[3]);
					oplRegWrite(REG_PMAC_SECU_KEY1_0,keyBuff.aesKeyBuff[2]);
					oplRegWrite(REG_PMAC_SECU_KEY1_0,keyBuff.aesKeyBuff[1]);
					oplRegWrite(REG_PMAC_SECU_KEY1_0,keyBuff.aesKeyBuff[0]);
				}else 
				{
					OPL_LOG_TRACE();
					return OPL_ERR_INVALID_PARAMETERS;
				}
				break;
			case PON_SECU_MODE_TRIPLE_CHURNING:
				if(keyIndex == PON_SECU_KEY_0)
				{
					oplRegFieldWrite(REG_PMAC_SECU_KEY0_0, 0, 24, keyBuff.aesKeyBuff[0]);
				}else if(keyIndex == PON_SECU_KEY_1)
				{
					oplRegFieldWrite(REG_PMAC_SECU_KEY1_0, 0, 24, keyBuff.aesKeyBuff[0]);
				}else 
				{
					OPL_LOG_TRACE();
					return OPL_ERR_INVALID_PARAMETERS;
				}
				break;
			default:
				OPL_LOG_TRACE();
				return OPL_ERR_INVALID_PARAMETERS;
				break;
		}
	}else 
	{
		OPL_LOG_TRACE();
	}
	return OPL_OK;
}
/*******************************************************************************
* ponSecuKeyHwRead
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponSecuKeyHwRead(UINT8 channel,UINT8 secuMode,UINT8 keyIndex,UINT8 *pBuff)
{
	union keyBuffUnion{
		UINT32 aesKeyBuff[4];
		UINT8   tripleChurningKeyBuff[16];
	}keyBuff;

	if(pBuff == OPL_NULL)
	{
		OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

	OPL_MEMSET(&keyBuff,0x0,sizeof(union keyBuffUnion));

	if(channel == PON_SECU_CHANNEL_SCB)
	{
		switch(secuMode)
		{
			case PON_SECU_MODE_AES:
				if(keyIndex == PON_SECU_KEY_0)
				{
					oplRegRead(REG_PMAC_SECU_SCB_KEY0_0,&(keyBuff.aesKeyBuff[3]));
					oplRegRead(REG_PMAC_SECU_SCB_KEY0_1,&(keyBuff.aesKeyBuff[2]));
					oplRegRead(REG_PMAC_SECU_SCB_KEY0_2,&(keyBuff.aesKeyBuff[1]));
					oplRegRead(REG_PMAC_SECU_SCB_KEY0_3,&(keyBuff.aesKeyBuff[0]));
				}else if(keyIndex == PON_SECU_KEY_1)
				{
					oplRegRead(REG_PMAC_SECU_SCB_KEY1_0,&(keyBuff.aesKeyBuff[3]));
					oplRegRead(REG_PMAC_SECU_SCB_KEY1_1,&(keyBuff.aesKeyBuff[2]));
					oplRegRead(REG_PMAC_SECU_SCB_KEY1_2,&(keyBuff.aesKeyBuff[1]));
					oplRegRead(REG_PMAC_SECU_SCB_KEY1_3,&(keyBuff.aesKeyBuff[0]));
				}else 
				{
					OPL_LOG_TRACE();
					return OPL_ERR_INVALID_PARAMETERS;
				}
				break;
			case PON_SECU_MODE_TRIPLE_CHURNING:
				if(keyIndex == PON_SECU_KEY_0)
				{
					oplRegFieldRead(REG_PMAC_SECU_SCB_KEY0_0, 0, 24, &(keyBuff.aesKeyBuff[0]));
				}else if(keyIndex == PON_SECU_KEY_1)
				{
					oplRegFieldRead(REG_PMAC_SECU_SCB_KEY1_0, 0, 24, &(keyBuff.aesKeyBuff[0]));
				}else 
				{
					OPL_LOG_TRACE();
					return OPL_ERR_INVALID_PARAMETERS;
				}
				break;
			default:
				OPL_LOG_TRACE();
				return OPL_ERR_INVALID_PARAMETERS;
				break;
		}
	}else if(channel == PON_SECU_CHANNEL_UNICAST)
	{
		switch(secuMode)
		{
			case PON_SECU_MODE_AES:
				if(keyIndex == PON_SECU_KEY_0)
				{
					oplRegRead(REG_PMAC_SECU_KEY0_0,&(keyBuff.aesKeyBuff[3]));
					oplRegRead(REG_PMAC_SECU_KEY0_1,&(keyBuff.aesKeyBuff[2]));
					oplRegRead(REG_PMAC_SECU_KEY0_2,&(keyBuff.aesKeyBuff[1]));
					oplRegRead(REG_PMAC_SECU_KEY0_3,&(keyBuff.aesKeyBuff[0]));
				}else if(keyIndex == PON_SECU_KEY_1)
				{
					oplRegRead(REG_PMAC_SECU_KEY1_0,&(keyBuff.aesKeyBuff[3]));
					oplRegRead(REG_PMAC_SECU_KEY1_1,&(keyBuff.aesKeyBuff[2]));
					oplRegRead(REG_PMAC_SECU_KEY1_2,&(keyBuff.aesKeyBuff[1]));
					oplRegRead(REG_PMAC_SECU_KEY1_3,&(keyBuff.aesKeyBuff[0]));
				}else 
				{
					OPL_LOG_TRACE();
					return OPL_ERR_INVALID_PARAMETERS;
				}
				break;
			case PON_SECU_MODE_TRIPLE_CHURNING:
				if(keyIndex == PON_SECU_KEY_0)
				{
					oplRegFieldRead(REG_PMAC_SECU_KEY0_0, 0, 24, &(keyBuff.aesKeyBuff[0]));
				}else if(keyIndex == PON_SECU_KEY_1)
				{
					oplRegFieldRead(REG_PMAC_SECU_KEY1_0, 0, 24, &(keyBuff.aesKeyBuff[0]));
				}else 
				{
					OPL_LOG_TRACE();
					return OPL_ERR_INVALID_PARAMETERS;
				}
				break;
			default:
				OPL_LOG_TRACE();
				return OPL_ERR_INVALID_PARAMETERS;
				break;
		}
	}else 
	{
		OPL_LOG_TRACE();
	}

	switch(secuMode)
	{
		case PON_SECU_MODE_AES:
			OPL_MEMCPY(pBuff,&keyBuff,16);
			break;
		case PON_SECU_MODE_TRIPLE_CHURNING:
			OPL_MEMCPY(pBuff,&(keyBuff.tripleChurningKeyBuff[1]),3);
			break;
		default:
			break;
	}

	return OPL_OK;
}

/*******************************************************************************
* ponTripleChurningModeHwWrite
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponTripleChurningModeHwWrite(UINT32 mode)
{
	return oplRegFieldWrite(REG_PMAC_SECU_CFG, 16, 1, mode);
}

/*******************************************************************************
* ponTripleChurningModeHwRead
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponTripleChurningModeHwRead(UINT32 *mode)
{
	return oplRegFieldRead(REG_PMAC_SECU_CFG, 16, 1, mode);
}
/*******************************************************************************
* ponDygaspCfgSet
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponDygaspCfgSet(UINT8 enable, UINT16 dygaspFlag,UINT8 dygaspCode)
{
	OPL_STATUS retVal = OPL_OK;

	if(OPL_ENABLE == enable)
	{
		oplRegFieldWrite(REG_MPCP_DYGSP_CFG0, 31, 1, OPL_ENABLE);
		oplRegFieldWrite(REG_MPCP_DYGSP_CFG0, 8, 16, dygaspFlag);
		oplRegFieldWrite(REG_MPCP_DYGSP_CFG0, 8, 16, dygaspCode);
	}else
	{
		oplRegFieldWrite(REG_MPCP_DYGSP_CFG0, 31, 1, OPL_DISABLE);
		oplRegFieldWrite(REG_MPCP_DYGSP_CFG0, 8, 16, dygaspFlag);
		oplRegFieldWrite(REG_MPCP_DYGSP_CFG0, 8, 16, dygaspCode);
	}
	return retVal;
}
/*******************************************************************************
* ponDygaspCfgGet
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponDygaspCfgGet(UINT8 *enable, UINT16 *dygaspFlag,UINT8 *dygaspCode)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal0;
	UINT32 regVal1;
	UINT32 regVal2;

	if(OPL_NULL == enable || OPL_NULL == dygaspFlag || OPL_NULL == dygaspCode)
	{
		OPL_DRV_PRINTF(("in put null pointer.\n"));
		return OPL_ERROR;
	}
	
	oplRegFieldRead(REG_MPCP_DYGSP_CFG0, 31, 1, &regVal0);
	oplRegFieldRead(REG_MPCP_DYGSP_CFG0, 8, 16,  &regVal1);
	oplRegFieldRead(REG_MPCP_DYGSP_CFG0, 0, 8,  &regVal2);
	
	if(regVal0)
	{
		*enable = OPL_ENABLE;
	}

	*dygaspFlag = regVal1;
	*dygaspCode = regVal2;

	return retVal;
}
/*******************************************************************************
* ponDygaspPayLoadSet
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponDygaspPayLoadSet(UINT8 *pBuff)
{
	UINT8 index;

	if(OPL_NULL == pBuff)
	{
		OPL_DRV_PRINTF(("input null pointer.\n"));
		return OPL_ERROR;
	}

	for(index = OPL_ZERO; index < 8; index++)
	{
		oplRegWrite(REG_MPCP_DYGSP_CFG8 - index*4,*((UINT32 *)pBuff + index));
	}

	return OPL_OK;
}

#if 0 /* implement in user space */
/*******************************************************************************
* ponBerGet
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponBerGet(float *ber)
{
    UINT32 EBR;
    UINT32 CBR;
    float febr;
    float fcbr;

    if(NULL != ber)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
    oplRegRead(REG_PMAC_EBR,&EBR);
    oplRegRead(REG_PMAC_CBR,&CBR);
    febr = EBR;
    fcbr = CBR;
    
    *ber = febr/(febr+fcbr);
    
    return OPL_OK;
}
#endif

/*******************************************************************************
* ponDygaspPayLoadShow
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponDygaspPayLoadShow(void)
{
	UINT8 index;
	UINT32 payLoad[10];
	
	for(index = OPL_ZERO; index < 8; index++)
	{
		oplRegRead(REG_MPCP_DYGSP_CFG8 - index*4, &payLoad[index]);
	}

	for(index = OPL_ZERO; index < 8; index++)
	{
		OPL_DRV_SHOW_PRINTF(("%08x:%08x\n",index,payLoad[index]));
	}
	return OPL_OK;
}
/*******************************************************************************
* ponMacIdShow
*
* DESCRIPTION:
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponMacIdShow(void)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 mac[MAC_LENGTH];
	
	retVal = ponMacIdHwRead(mac);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	OPL_DRV_SHOW_PRINTF(("macId:%02x:%02x:%02x:%02x:%02x:%02x\n",
		mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]));
	return retVal;
}

/*******************************************************************************
* ponMpcpStatusShow
*
* DESCRIPTION:
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponMpcpStatusShow(void)
{
	OPL_STATUS retVal;
	PON_MPCP_DISC_STATUS_t mpcpStatus;

	retVal = oplRegRead(REG_MPCP_DISC_STATUS, (UINT32 *)&mpcpStatus);

	if(OPL_OK != retVal)
	{
		return retVal;
	}

	OPL_DRV_SHOW_PRINTF(("%-12s %-5s %s\n","type", "value", "desc"));

	OPL_DRV_SHOW_PRINTF(("%-12s %d\n","gatePendNum",mpcpStatus.bfGatePendingNum));
	
	if(mpcpStatus.bfDiscLEvent <= EVENT_WATCHDOG_TIMEOUT)
	{
		OPL_DRV_SHOW_PRINTF(("%-12s %-5d %s\n","lastEvent",mpcpStatus.bfDiscLEvent,mpcpEventDisc[mpcpStatus.bfDiscLEvent]));
	}
	
	if(mpcpStatus.bfDiscCEvent <= EVENT_WATCHDOG_TIMEOUT)
	{
		OPL_DRV_SHOW_PRINTF(("%-12s %-5d %s\n","currEvent",mpcpStatus.bfDiscCEvent,mpcpEventDisc[mpcpStatus.bfDiscCEvent]));
	}

	if(mpcpStatus.bfDiscLState <= STATE_WATCHDOG_TIMEOUT)
	{
		OPL_DRV_SHOW_PRINTF(("%-12s %-5d %s\n","lastState",mpcpStatus.bfDiscLState,mpcpStateDisc[mpcpStatus.bfDiscLState]));
	}
	
	if(mpcpStatus.bfDiscCState <= STATE_WATCHDOG_TIMEOUT)
	{
		OPL_DRV_SHOW_PRINTF(("%-12s %-5d %s\n","currState",mpcpStatus.bfDiscCState,mpcpStateDisc[mpcpStatus.bfDiscCState]));
	}

	return OPL_OK;
}
/*******************************************************************************
* ponDelayConfigAllShow
*
* DESCRIPTION:
*	this function is used to show delayConfig info 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponDelayConfigAllShow(void)
{
	OPL_STATUS retVal = OPL_OK;
	PON_DLY_CONF_t ponDlyConfVal;
	UINT8 *stringStatus[] = {"Disable","Enable",OPL_NULL};

	retVal = oplRegRead(REG_MPCP_TX_DLY, (UINT32 *)&ponDlyConfVal);

	if(OPL_OK != retVal)
	{
		return retVal;
	}

	OPL_DRV_SHOW_PRINTF(("set ponDelayConfigVal:\n"));
	OPL_DRV_SHOW_PRINTF(("%-30s:%d\n","CP_RPTSENT_DELT",ponDlyConfVal.cpRptsentDelt));
	OPL_DRV_SHOW_PRINTF(("%-30s:%s\n","CP_TXDLY_EN",stringStatus[ponDlyConfVal.cpTxDlyEn]));
	OPL_DRV_SHOW_PRINTF(("%-30s:%s\n","CP_SYNC_EN",stringStatus[ponDlyConfVal.cpSyncEn]));
	OPL_DRV_SHOW_PRINTF(("%-30s:%d\n","CP_TX_DLY",ponDlyConfVal.cpTxDly));
	
	return retVal;
}

/*******************************************************************************
* ponMpcpDbaAgetConfigAllShow
*
* DESCRIPTION:
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponMpcpDbaAgetConfigAllShow(void)
{
	OPL_STATUS retVal = OPL_OK;
	PON_DBA_AGETN_CONF_t ponDbaAgentConf;
	UINT8 *rptMode[] = {"report send last","report send first"};
    UINT8 *staMode[] = {"Enable simplified report statistic mode","Enable accurate report statistic mode"}; 
    UINT8 *mgRptMode[] = {"Only one Report is sent for all the merged force report grants.",
        "One Report is sent at each of the merged force report grant."};
	
	retVal = oplRegRead(REG_MPCP_DBA_AGENT_CFG, (UINT32 *)&ponDbaAgentConf);

	if(OPL_OK != retVal)
	{
		return retVal;
	}

    OPL_DRV_SHOW_PRINTF(("%-25s:%s\n","statis mode",			    staMode[ponDbaAgentConf.cpRptStaMode]));
    OPL_DRV_SHOW_PRINTF(("%-25s:%s\n","merg rtp send mode",			mgRptMode[ponDbaAgentConf.cpMergeRptSendMode]));
	OPL_DRV_SHOW_PRINTF(("%-25s:%s\n","report send mode",			rptMode[ponDbaAgentConf.cpRptSendMode]));
    OPL_DRV_SHOW_PRINTF(("%-25s:%d\n","PKT_INIT_TIMER_CFG",			ponDbaAgentConf.cpPktInitTimerCfg));
	OPL_DRV_SHOW_PRINTF(("%-25s:%d\n","CP_DELT_TXNORM_NOFEC_TIME",	ponDbaAgentConf.cpDeltaTxNormalNoFecTime ));
	OPL_DRV_SHOW_PRINTF(("%-25s:%d\n","CP_TXMPCP_NOFEC_TIME",		ponDbaAgentConf.cpTxMpcpNoFecTime));
	
	return retVal;
}
/*******************************************************************************
* ponSecuKeyShow
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS ponSecuKeyShow(UINT8 channel,UINT8 secuMode,UINT8 keyIndex)
{
	OPL_STATUS retVal;
	UINT8 keyBuff[16];
	UINT8 index;
	
	retVal = ponSecuKeyHwRead(channel,secuMode,keyIndex,&keyBuff[0]);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return OPL_ERROR;
	}

	OPL_DRV_SHOW_PRINTF(("KEY IS :\n"));
	if(secuMode == PON_SECU_MODE_AES)
	{
		OPL_DRV_SHOW_PRINTF(("0X\n"));
		for(index = OPL_ZERO; index < 16; index++)
		{
			OPL_DRV_SHOW_PRINTF(("%02x ",keyBuff[index]));
		}
		OPL_DRV_SHOW_PRINTF(("\n"));
	}else if(secuMode == PON_SECU_MODE_TRIPLE_CHURNING)
	{
		OPL_DRV_SHOW_PRINTF(("0X\n"));
		for(index = OPL_ZERO; index < 3; index++)
		{
			OPL_DRV_SHOW_PRINTF(("%02x ",keyBuff[index]));
		}
		OPL_DRV_SHOW_PRINTF(("\n"));
	}else 
	{
		OPL_DRV_SHOW_PRINTF(("no such mode.\n"));
		return OPL_ERROR;
	}
	return OPL_OK;
}

void ponSerdesReset()
{
    oplRegAccessLock();

    /* Reset SERDES POWER MODE */
    /* oplRegWrite(REG_PMAC_SERDES_COMMON, 0x0000056c); */
    *(UINT32 *)((UINT32)ONU_REGBASE + REG_PMAC_SERDES_COMMON) = 0x0000056c;
    msleep(1);

    /* oplRegWrite(REG_PMAC_SERDES_COMMON, 0x8000056c); */
    *(UINT32 *)((UINT32)ONU_REGBASE + REG_PMAC_SERDES_COMMON) = 0x8000056c;

    /* delay for a while, around 1ms */
    msleep(1);

    /* Enable SERDES PLL */
    /* oplRegWrite(REG_PMAC_SERDES_COMMON, 0xC000056C); */
    *(UINT32 *)((UINT32)ONU_REGBASE + REG_PMAC_SERDES_COMMON) = 0xC000056C;

    /* delay for a while, around 1ms */
    msleep(1);

    /* 
        Asynchronous reset for SERDES except PLL: 
        1: Normal operation
        0: Reset operation 
    */
    /* oplRegWrite(REG_PMAC_SERDES_COMMON, 0xE000056C); */
    *(UINT32 *)((UINT32)ONU_REGBASE + REG_PMAC_SERDES_COMMON) = 0xE000056C;
    msleep(1);

    oplRegAccessUnLock();
}

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
    ponBypassModeEnHwWrite(1);
    /* Wait for upstream queue empty */
    for (i = 0; i < MAX_LOOP_CNT && ponBufferCellNumRead(); i++);
    if (i == MAX_LOOP_CNT)
    {
        OPL_DRV_PRINTF_ERR(("ponBufferReset: buffer is not empty\n"));
    }
    /* Disable bypass mode */ 
    ponBypassModeEnHwWrite(0);

    /* Enable Laser */ 
    oplRegFieldWrite(REG_PMAC_LASER_ON_CFG, 0, 4, 0x5);
	
    /* Enable CPDMA */ 
    oplRegFieldWrite(REG_DMA0_EN, 1, 1, 1); 
	oplRegWrite(REG_GMAC_CFG_TRANSFER_ENA, gmacCfg);
}

