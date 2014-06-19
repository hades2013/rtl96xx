/*
=============================================================================
     File Name: hal_onu.h

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

#ifndef _HAL_ONU_H_
#define _HAL_ONU_H_

enum HAL_PON_AES_MODE_e
{
	HAL_PON_AES_MODE_OFB 		= 2,
	HAL_PON_AES_MODE_CTR 		= 3,
	HAL_PON_AES_MODE_CFB_1 		= 4,
	HAL_PON_AES_MODE_CFB_8 		= 5,
	HAL_PON_AES_MODE_CFB_64 	= 6,
	HAL_PON_AES_MODE_CFB_128 	= 7,
	HAL_PON_AES_MODE_END
};

enum HAL_PON_SECU_MODE_TYPE_e
{
	HAL_PON_SECU_MODE_AES,
	HAL_PON_SECU_MODE_TRIPLE_CHURNING
};

enum HAL_PON_SECU_CHANNEL_TYPE_e
{
	HAL_PON_SECU_CHANNEL_SCB,
	HAL_PON_SECU_CHANNEL_UNICAST	
};

enum PON_MPCP_STATE_e
{
	HAL_STATE_WAIT,
	HAL_STATE_REGISTERING,
	HAL_STATE_REGISTER_REQUEST,
	HAL_STATE_REGISTER_PENDING,
	HAL_STATE_DENIED,
	HAL_STATE_RETRY,
	HAL_STATE_REGISTER_ACK,
	HAL_STATE_NACK,
	HAL_STATE_REGISTERD,
	HAL_STATE_REMOTE_DEREGISTER,
	HAL_STATE_LOCAL_DEREGISTER,
	HAL_STATE_WATCHDOG_TIMEOUT,
	HAL_STATE_END
};

#pragma pack(1)

typedef struct HAL_ONU_PON_CFG_s{
    UINT8  ponMac[6];
    UINT32 serdesDelay;
    UINT32 laserOnTime;
    UINT32 laserOffTime;
    UINT32 syncTime;
    UINT32 llid;
    UINT32 mpcpStatus;
    UINT32 oamStatus;
    UINT32 fecStatus;
    float  realTimeBer;
    
    UINT32 secStatus;           /*enable or disable*/
    UINT32 secMode;             /*triplechurning or aes mode*/
    UINT32 channelOfSec;        /*unicast channel or scb channel*/
    UINT32 secKeyIndex;
    UINT8  *secKey;
    UINT32 aesMode;

    UINT32 regAddr;
    UINT32 regValue;
    UINT32 PontxPolarity;
    UINT32 lpChannel;		/*uni_llid or scb_llid loopback*/
    UINT32 lpEnable;           /*enable or disable loopback*/
}HAL_ONU_PON_CFG_t;

typedef struct HAL_ONU_DBA_CFG_s{
    UINT32 reportMode;
    UINT32 qSetNum;
    UINT32 qNum;
    UINT32 threshold;
}HAL_ONU_DBA_CFG_t;

typedef struct HAL_ONU_DBA_AGENT_CFG_s
{
	UINT32 cpRptStaMode;/*statis mode 1 or 0*/
    UINT32 cpMergeRptSendMode;
    UINT32 cpRptSendMode;
    UINT32 cpPktInitTimerCfg;
    UINT32 cpDeltaTxNormalNoFecTime;
	UINT32 cpTxMpcpNoFecTime;
}HAL_ONU_DBA_AGENT_CFG_t;

typedef struct HAL_ONU_DBA_DLY_CFG_s{
	UINT32 cpRptsentDelt;
	UINT32 cpTxDlyEn;
	UINT32 cpSyncEn;
	UINT32 cpTxDly;    
}HAL_ONU_DBA_DLY_CFG_t;

typedef struct HAL_ONU_MPCP_FSM_HOLD_CTRL_CFG_s{
	UINT32 enable;
}HAL_ONU_MPCP_FSM_HOLD_CTRL_CFG_t;

typedef struct HAL_ONU_MPCP_FSM_HOLD_TIME_CFG_s{
	UINT32 holdtime;
}HAL_ONU_MPCP_FSM_HOLD_TIME_CFG_t;

typedef struct HAL_ONU_MTU_CFG_s{
	UINT32 mtuSize;
}HAL_ONU_MTU_CFG_t;

#pragma pack()

#define HAL_ONU_PRINTF(x)       printf x
#define HAL_ONU_TRACE()         HAL_ONU_PRINTF(("%s,%s,%d\n",__FILE__,__FUNCTION__,__LINE__))
#define HAL_ONU_DEBUG_TRACE()   
#endif
