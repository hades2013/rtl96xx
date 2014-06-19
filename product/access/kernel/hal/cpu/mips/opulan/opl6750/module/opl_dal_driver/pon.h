/*
=============================================================================
     Header Name: pon.h

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
#ifndef PON_H
#define PON_H

#define OPL_MAX_QUEUE_SET1    0
#define OPL_MAX_QUEUE_SET2    1
#define OPL_MAX_QUEUE_SET3    2
#define OPL_MAX_QUEUE_SET4    3

#define OPL_MIN_QUEUE_NUM     1
#define OPL_MAX_QUEUE_NUM 	 4	

#define OPL_ONU_DISCOVER_PROCESS_WAIT    0
#define OPL_ONU_DISCOVER_AGENT_PERMIT    1
#define OPL_ONU_DISCOVER_AGENT_DENIED    2

#define OPL_ONU_GRANT_ENTRY_LEN     8

typedef struct PON_MPCP_DISC_CTL_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 bfRsved:27;
	UINT32 bfDiscAgentAck:2;
	UINT32 bfDiscAgentAckAuto:1;
	UINT32 bfDiscAgentRegister:1;
#else
	UINT32 bfDiscAgentRegister:1;
	UINT32 bfDiscAgentAckAuto:1;
	UINT32 bfDiscAgentAck:2;
	UINT32 bfRsved:27;
#endif
}PON_MPCP_DISC_CTL_t;

typedef struct PON_MPCP_DISC_STATUS_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 bfRsved1:3;
	UINT32 bfGatePendingNum:5;
	UINT32 bfRsved0:8;
	UINT32 bfDiscLEvent:4;
	UINT32 bfDiscCEvent:4;
	UINT32 bfDiscLState:4;
	UINT32 bfDiscCState:4;
#else
	UINT32 bfDiscCState:4;
	UINT32 bfDiscLState:4;
	UINT32 bfDiscCEvent:4;
	UINT32 bfDiscLEvent:4;
	UINT32 bfResved0:8;
	UINT32 bfGatePendingNum:5;
	UINT32 bfRsved1:3; 
#endif
}PON_MPCP_DISC_STATUS_t;

typedef struct PON_DLY_CONF_s
{
    UINT32 reserved0:10;
	UINT32 cpRptsentDelt:6;
    UINT32 reserved1:2;
	UINT32 cpTxDlyEn:1;
	UINT32 cpSyncEn:1;
    UINT32 reserved2:2;
	UINT32 cpTxDly:10;
}PON_DLY_CONF_t;

typedef struct PON_DBA_AGETN_CONF_s
{
	UINT32 cpRptStaMode:1;
    UINT32 cpMergeRptSendMode:1;
    UINT32 reserved0:1;
    UINT32 cpRptSendMode:1;
    UINT32 cpPktInitTimerCfg:4;
    UINT32 reserved1:3;
	UINT32 cpDeltaTxNormalNoFecTime:7;
	UINT32 reserved:7;
	UINT32 cpTxMpcpNoFecTime:7;
}PON_DBA_AGETN_CONF_t;

typedef enum PON_MPCP_EVENT_s
{
	EVENT_BEGIN,
	EVENT_REGISTER_EVENT_FROM_DISCAGENT,
	EVENT_INSIDE_DISCWINDOW,
	EVENT_DEREGISTER_FROM_DISCAGENT_AND_NOT_IN_DISCWINDOW,
	EVENT_OLT_PERMIT_REGISTER_AND_NOT_IN_DISCWINDOW,
	EVENT_OLT_DENY_REGISTER_AND_NOT_IN_DISCWINDOW,
	EVENT_NEW_DISCWINDOW,
	EVENT_PERMIT_ACK_EVENT_FROM_DISCAGENT,
	EVENT_DENY_ACK_EVENT_FROM_DISCAGENT,
	EVENT_REGISTER_FROM_OLT,
	EVENT_REGISTERED_AND_TIMSTAMP_DRIFT_EVENT,
	EVENT_DEREGISTER_EVENT_FROM_OLT,
	EVENT_DEREGISTER_EVENT_FROM_DISCAGENT_WHEN_ONU_IS_REGISTERED,
	EVENT_WATCHDOG_TIMEOUT,
	EVENT_END
}PON_MPCP_EVENT_e;

typedef enum PON_MPCP_STATE_s
{
	STATE_WAIT,
	STATE_REGISTERING,
	STATE_REGISTER_REQUEST,
	STATE_REGISTER_PENDING,
	STATE_DENIED,
	STATE_RETRY,
	STATE_REGISTER_ACK,
	STATE_NACK,
	STATE_REGISTERD,
	STATE_REMOTE_DEREGISTER,
	STATE_LOCAL_DEREGISTER,
	STATE_WATCHDOG_TIMEOUT,
	STATE_END
}PON_MPCP_STATE_e;

typedef enum PON_AES_MODE_s
{
	PON_AES_MODE_OFB 			= 2,
	PON_AES_MODE_CTR 			= 3,
	PON_AES_MODE_CFB_1 		= 4,
	PON_AES_MODE_CFB_8 		= 5,
	PON_AES_MODE_CFB_64 	= 6,
	PON_AES_MODE_CFB_128 	= 7,
	PON_AES_MODE_END
}PON_AES_MODE_e;

typedef enum PON_SECU_TYPE_s
{
	PON_SECU_SCB_EN,
	PON_SECU_UNICAST_EN,
	PON_SECU_SCB_MODE,
	PON_SECU_UNICAST_MODE,
	PON_SECU_AES_MODE,
}PON_SECU_TYPE_e;

typedef enum PON_SECU_MODE_TYPE_s
{
	PON_SECU_MODE_AES,
	PON_SECU_MODE_TRIPLE_CHURNING,
}PON_SECU_MODE_TYPE_e;

typedef enum PON_SECU_CHANNEL_TYPE_s
{
	PON_SECU_CHANNEL_SCB,
	PON_SECU_CHANNEL_UNICAST,	
}PON_SECU_CHANNEL_TYPE_e;

typedef enum PON_SECU_KEY_INDEX_s
{
	PON_SECU_KEY_0,
	PON_SECU_KEY_1,	
}PON_SECU_KEY_INDEX_e;

typedef struct PON_SECU_CFG_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 bfReserved0:22;
	UINT32 bfSecScbEn:1;
	UINT32 bfSecUnicastEn:1;
	UINT32 bfReserved1:2;
	UINT32 bfSecScbMode:1;
	UINT32 bfSecUnicastMode:1;
	UINT32 bfReserved2:1;
	UINT32 bfAesMode:3;	
#else
	UINT32 bfAesMode:3;
	UINT32 bfReserved2:1;
	UINT32 bfSecUnicastMode:1;
	UINT32 bfSecScbMode:1;
	UINT32 bfReserved1:2;
	UINT32 bfSecUnicastEn:1;
	UINT32 bfSecScbEn:1;
	UINT32 bfReserved0:22;
#endif
}PON_SECU_CFG_t;

typedef struct PON_MPCP_FSM_HOLD_CTRL_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 reserved1:23;
	UINT32 softHoldEn:1;
	UINT32 reserved0:2;
	UINT32 linkHoldEn:1;
	UINT32 losHoldEn:1;
	UINT32 losPosEn:1;
	UINT32 grantFlushEn:1;
	UINT32 timerHoldEn:1;
	UINT32 driftHoldEn:1;
#else
	UINT32 driftHoldEn:1;
	UINT32 timerHoldEn:1;
	UINT32 grantFlushEn:1;
	UINT32 losPosEn:1;
	UINT32 losHoldEn:1;
	UINT32 linkHoldEn:1;
	UINT32 reserved0:2;
	UINT32 softHoldEn:1;
	UINT32 reserved1:23;
#endif
}PON_MPCP_FSM_HOLD_CTRL_t;

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
OPL_STATUS ponForceReportTxEnHwWrite(UINT8 enable);
 
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
OPL_STATUS ponForceReportTxEnHwRead(UINT8 *enable);
 
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
OPL_STATUS ponBypassModeEnHwWrite(UINT8 enable);
 
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
OPL_STATUS ponBypassModeEnHwRead(UINT8 *enable);
 
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
OPL_STATUS ponQsetNumHwWrite(UINT8 qSetNum);
 
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
OPL_STATUS ponQsetNumHwRead(UINT8 *qSetNum);
 
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
OPL_STATUS ponNullReportIntervalHwWrite(UINT32 nullReportInterval);
 
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
OPL_STATUS ponNullReportIntervalHwRead(UINT32 *nullReportInterval);
 
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
OPL_STATUS ponBypassModeSyncTimeHwWrite(UINT16 syncTime);
 
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
OPL_STATUS ponBypassModeSyncTimeHwRead(UINT16 *syncTime);
 
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
OPL_STATUS ponBypassModeLlidHwWrite(UINT16 llid);
 
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
OPL_STATUS ponBypassModeLlidHwRead(UINT16 *llid);
 
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
OPL_STATUS ponSyncTimeHwRead(UINT32 *syncTime);
 
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
OPL_STATUS ponLlidHwRead(UINT32 *LLID);
 
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
OPL_STATUS ponAesEnHwWrite(UINT8 enable);
 
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
OPL_STATUS ponAesEnHwRead(UINT8 *enable);
 
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
OPL_STATUS ponTripleChuningEnHwWrite(UINT8 enable);
 
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
OPL_STATUS ponTripleChuningEnHwRead(UINT8 *enable);
 
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
OPL_STATUS ponFecRxEnHwWrite(UINT8 enable);
 
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
OPL_STATUS ponFecRxEnHwRead(UINT8 *enable);
 
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
OPL_STATUS ponFecTxEnHwWrite(UINT8 enable);
 
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
OPL_STATUS ponFecTxEnHwRead(UINT8 *enable);
 

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
OPL_STATUS ponSerdesDelayHwWrite(UINT32 delayCount);
 
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
OPL_STATUS ponSerdesDelayHwRead(UINT32 *delayCount);
 
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
OPL_STATUS ponReportDisableHwWrite(UINT8 enable);
 
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
OPL_STATUS ponReportDisableHwRead(UINT8 *enable);


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
OPL_STATUS ponLastQsetModHwWrite(UINT8 mode);
 
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
OPL_STATUS ponLastQsetModHwRead(UINT8 *mode);
 
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
OPL_STATUS ponQsetThreshHoldHwWrite(UINT8 qNum,UINT8 queue,UINT32 thresholdVal);
 
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
OPL_STATUS ponQsetThreshHoldHwRead(UINT8 qNum,UINT8 queue,UINT32 *thresholdVal);
 
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
OPL_STATUS ponMacIdHwWrite(UINT8 *mac);
 
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
OPL_STATUS ponMacIdHwRead(UINT8 *mac);
 
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
OPL_STATUS ponLaserOnCntHwWrite(UINT32 count);
 
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
OPL_STATUS ponLaserOnCntHwRead(UINT32 *count);
 
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
OPL_STATUS ponLaserOffCntHwWrite(UINT32 count);
 
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
OPL_STATUS ponLaserOffCntHwRead(UINT32 *count);

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
OPL_STATUS ponOamFinishedHwWrite(UINT32 oamFinished);


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
OPL_STATUS ponActiveLedEnHwWrite(UINT32 enable);


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
OPL_STATUS ponActiveLedOnOffHwWrite(UINT32 onOff);

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
OPL_STATUS ponLinkLedEnHwWrite(UINT32 enable);


/*******************************************************************************
* ponLinkLedOnOffHwWrite
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
OPL_STATUS ponLinkLedOnOffHwWrite(UINT32 onOff);


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
OPL_STATUS ponLinkLedBlinkModeHwWrite(UINT32 mode);


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
OPL_STATUS ponLinkLedBlinkStepHwWrite(UINT32 step);


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
OPL_STATUS ponActiveLedBlinkModeHwWrite(UINT32 mode);


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
OPL_STATUS ponActiveLedBlinkStepHwWrite(UINT32 step);

 
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
void ponMpcpProcess(void);
 
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
OPL_STATUS ponMacIdSetTest(UINT8 *macString);
 
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
OPL_STATUS ponMacIdShow(void);
 

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
OPL_STATUS ponMpcpStatusShow(void);
 
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
OPL_STATUS ponDelayConfigAllShow(void);
 

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
OPL_STATUS ponDelayConfigAllSet(UINT8 cpTxDlyEn,UINT8 cpSyncEn,UINT16 cpRptsentDelt,UINT16 cpTxDly);

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
OPL_STATUS ponDelayConfigAllGet(UINT32 *cpTxDlyEn,UINT32 *cpSyncEn,UINT32 *cpRptsentDelt,UINT32 *cpTxDly);


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
OPL_STATUS ponMpcpDbaAgetConfigAllShow(void);
 

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
    );  

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
    );

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

OPL_STATUS ponSecuConfigHwWrite(UINT8 type,UINT32 value);
 
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

OPL_STATUS ponSecuConfigHwRead(UINT8 type,UINT32 *value);
 
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
OPL_STATUS ponSecuKeyGenerate(UINT8 keyIndex);

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
OPL_STATUS ponSecuNoCtcKeyGenerate(UINT8 keyIndex);
 
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
OPL_STATUS ponSecuKeyHwWrite(UINT8 channel,UINT8 secuMode,UINT8 keyIndex,UINT8 *pBuff);
 
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
OPL_STATUS ponSecuKeyHwRead(UINT8 channel,UINT8 secuMode,UINT8 keyIndex,UINT8 *pBuff);
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
OPL_STATUS ponTripleChurningModeHwWrite(UINT32 mode);

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
OPL_STATUS ponTripleChurningModeHwRead(UINT32 *mode);
 
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
OPL_STATUS ponSecuKeyShow(UINT8 channel,UINT8 secuMode,UINT8 keyIndex);
 
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
OPL_STATUS ponDygaspCfgSet(UINT8 enable, UINT16 dygaspFlag,UINT8 dygaspCode);
 
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
OPL_STATUS ponDygaspCfgGet(UINT8 *enable, UINT16 *dygaspFlag,UINT8 *dygaspCode);
 
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
OPL_STATUS ponDygaspPayLoadSet(UINT8 *pBuff);

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
OPL_STATUS ponBerGet(float *ber);
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
OPL_STATUS ponDygaspPayLoadShow(void);

/*******************************************************************************
* ponSerdesReset
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
*   n/a.
* SEE ALSO: 
*/
void ponSerdesReset(void);

/*******************************************************************************
* ponBufferReset
*
* DESCRIPTION: 
*
* INPUTS: 
*	
* OUTPUTS: 
*		
* RETURNS:
*   n/a.
* SEE ALSO: 
*/
void ponBufferReset(void);

#endif
