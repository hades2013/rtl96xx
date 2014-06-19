/*=============================================================================
     Header Name: dal_pon.h

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
#ifndef DAL_PON_H
#define DAL_PON_H

#include "opl_driver.h"
#include "opconn_api.h"

#define MAX_LASER_ON_TIME 		(0XFFFF)
#define MAX_LASER_OFF_TIME 		(0XFFFF)
#define MAX_SERDES_DELAY_TIME	(0XFF)

typedef struct DAL_PON_STATUS_s{
    UINT32 bfGatePendingNum:5;
	UINT32 bfRsved0:8;
	UINT32 bfDiscLEvent:4;
	UINT32 bfDiscCEvent:4;
	UINT32 bfDiscLState:4;
	UINT32 bfDiscCState:4;
}DAL_PON_STATUS_t;

/* Begin Added, 2011-5-6*/
typedef struct PON_MPCP_RTT_CTRL_s
{
#ifdef OPCONN_BIG_ENDIAN
    UINT32 reserved1:2;
    UINT32 RttCompEn:1;
    UINT32 RttCompSyncEn:1;
    UINT32 reserved2:2;
    UINT32 RttCompOffset:10;
    UINT32 reserved3:2;
    UINT32 RttCorEn:1;
    UINT32 RttCorSyncEn:1;
    UINT32 reserved4:2;
    UINT32 RttCorOffset:10;
#else      
    UINT32 RttCompOffset:10;
    UINT32 reserved4:2;    
    UINT32 RttCorSyncEn:1;
    UINT32 RttCorEn:1;
    UINT32 reserved3:2;
    UINT32 RttCorOffset:10;
    UINT32 reserved2:2;
    UINT32 RttCompSyncEn:1;
    UINT32 RttCompEn:1;
    UINT32 reserved1:2;
#endif    
}PON_MPCP_RTT_CTRL_t;

/* End   Added, 2011-5-6*/
OPCONN_TAB_INFO_t *dalTabAttrInfoGet(void);
/*******************************************************************************
* dalPonInit
*
* DESCRIPTION:
*  		this function is used to init the default pon config ,such as serdes delay, laser on and of time count
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonInit(void);

/*******************************************************************************
* dalPonMacIdSet
*
* DESCRIPTION:
*  		this function is used to set the onu macld, used for mpcp register and oam pkt.
*
*	INPUTS:
*		mac : 
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMacIdSet(UINT8 *mac);

/*******************************************************************************
* dalPonMacIdGet
*
* DESCRIPTION:
*  		this function is used get the llid mac address used for mpcp register.
*
*	INPUTS:
*		mac:
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMacIdGet(UINT8 *mac);

/*******************************************************************************
* dalPonPhyFecModeSet
*
* DESCRIPTION:
*  		this function is used to set the fec mode. 0x01 for unknown,0x02 for eanble,0x03 for disable.
*
*	INPUTS:
*		portId: 		portId
*		mode:		0x01:unknown,0x02:enable,0x03:disable
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonPhyFecModeSet(UINT32 mode);

/*******************************************************************************
* dalPonPhyFecModeGet
*
* DESCRIPTION:
*  		this function is used to get the fec mode info.0x01 for unknown,0x02 for eanble,0x03 for disable.
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		mode:		0x01:unknown,0x02:enable,0x03:disable
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonPhyFecModeGet(UINT32 *mode);

/*******************************************************************************
* dalPonPhyTripleChuningModeSet
*
* DESCRIPTION:
*  		this function is used to enable or disable ctc triple chuning.
*
*	INPUTS:
*		mode: 0 for disable while 1 for enable
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonPhyTripleChuningModeSet(UINT32 mode);

/*******************************************************************************
* dalPonPhyTripleChuningModeGet
*
* DESCRIPTION:
*  		this function is used to get the ctc triple chuning mode status
*
*	INPUTS:
*		
*	OUTPUTS:
*		mode: 0 for disable while 1 for enable
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonPhyTripleChuningModeGet(UINT32 *mode);

/*******************************************************************************
* dalPonPhyAesModeSet
*
* DESCRIPTION:
*  		this function is used to enable or disable aes
*
*	INPUTS:
*		mode: 0 for disable while 1 for enable
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonPhyAesModeSet(UINT32 mode);

/*******************************************************************************
* dalPonPhyTripleChuningModeGet
*
* DESCRIPTION:
*  		this function is used to get the aes
*
*	INPUTS:
*		
*	OUTPUTS:
*		mode: 0 for disable while 1 for enable
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonPhyAesModeGet(UINT32 *mode);


/*******************************************************************************
* dalPonLaserOnCntSet
*
* DESCRIPTION:
*  		this function is used to set the laser on count
*
*	INPUTS:
*		count: num of tq laser on used.
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonLaserOnCntSet(UINT32 count);


/*******************************************************************************
* dalPonLaserOnCntSet
*
* DESCRIPTION:
*  		this function is used to get the laser on count
*
*	INPUTS:
*		count: buff used to store the value of tq used for laser on
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonLaserOnCntGet(UINT32 *count);

/*******************************************************************************
* dalPonLaserOffCntSet
*
* DESCRIPTION:
*  		this function is used to set the laser off count
*       
*	INPUTS:
*		this function is used to set the laser off count
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonLaserOffCntSet(UINT32 count);


/*******************************************************************************
* dalPonLaserOnCntSet
*
* DESCRIPTION:
*  		this function is used to set the laser off count
*
*	INPUTS:
*		count: buff used to store the value of tq used for laser off
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonLaserOffCntGet(UINT32 *count);

OPL_STATUS dalponMpcpRttCtrlRead(UINT32 *puiRttCompEn,
                              UINT32 *puiRttCompSyncEn,
                              UINT32 *puiRttCompOffset,
                              UINT32 *puiRttCorEn,
                              UINT32 *puiRttCorSyncEn,
                              UINT32 *puiRttCorOffset);

OPL_STATUS dalponMpcpRttCtrlWrite(UINT32 uiRttCompEn,
                               UINT32 uiRttCompSyncEn,
                               UINT32 uiRttCompOffset,
                               UINT32 uiRttCorEn,
                               UINT32 uiRttCorSyncEn,
                               UINT32 uiRttCorOffset);

/*******************************************************************************
* dalPonSyncTimeGet
*
* DESCRIPTION:
*  		this function is used get the synctime value learned from olt.
*
*	INPUTS:
*		count: buff used to store the value of tq used for synctime
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonSyncTimeGet(UINT32 *count);

/*******************************************************************************
* dalPonLlidGet
*
* DESCRIPTION:
*  		this function is used get the llid assigned by olt.
*
*	INPUTS:
*		count: buff used to store the value of tq used for synctime
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonLlidGet(UINT32 *llid);

/*******************************************************************************
* dalPonSerdesDelayCntSet
*
* DESCRIPTION:
*  		this function is used set the serdes dealy count
*
*	INPUTS:
*		count:
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonSerdesDelayCntSet(UINT32 count);

/*******************************************************************************
* dalPonSerdesDelayCntSet
*
* DESCRIPTION:
*  		this function is used get the serdes delay count
*	INPUTS:
*		count:
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonSerdesDelayCntGet(UINT32 *count);

/*******************************************************************************
* changbits
*
* DESCRIPTION:
*  		this function is used change bit like other venders
*	INPUTS:
*		count:
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void changbits(UINT8 *bitstring);

OPL_STATUS dalPonSecurityModeSet(UINT32 channel,UINT32 mode);


OPL_STATUS dalPonSecurityModeGet(UINT32 channel,UINT32 *mode);

OPL_STATUS dalPonSecurityAesModeSet(UINT32 channel,UINT32 mode);

OPL_STATUS dalPonSecurityAesModeGet(UINT32 channel,UINT32 *mode);

/*******************************************************************************
* dalPonUnicastTripleChurningKeyGet
*
* DESCRIPTION:
*  		this function is used get the Triple Churning Key
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonUnicastTripleChurningKeyGet(UINT8 keyIndex,UINT8 *keyBuff);


/*******************************************************************************
* dalPonScbTripleChurningKeySet
*
* DESCRIPTION:
*  		this function is used get the Triple Churning Key
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonScbTripleChurningKeySet(UINT8 keyIndex,UINT8 *keyBuff);


/*******************************************************************************
* dalPonUnicastAesKeyGet
*
* DESCRIPTION:
*  		this function is used get aes key
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonUnicastAesKeyGet(UINT8 keyIndex,UINT8 *keyBuff,UINT8 mode);


/*******************************************************************************
* dalPonScbAesKeySet
*
* DESCRIPTION:
*  		this function is used get aes key
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonScbAesKeySet(UINT8 keyIndex,UINT8 *keyBuff,UINT8 mode);

/*******************************************************************************
* dalPonTripleChurningModeGet
*
* DESCRIPTION:
*  		this function is used get aes key
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonTripleChurningModeGet(UINT8 *mode);

/*******************************************************************************
* dalPonTripleChurningModeSet
*
* DESCRIPTION:
*  		this function is used get aes key
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonTripleChurningModeSet(UINT8 mode);
/*******************************************************************************
* dalPonMpcpDbaAgentConfigAllSet
*
* DESCRIPTION:
*  		this function is used to set mpcp dba agent delay config
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMpcpDbaAgentConfigAllSet(
	UINT32 cpRptStaMode,
    UINT32 cpMergeRptSendMode,
    UINT32 cpRptSendMode,
    UINT32 cpPktInitTimerCfg,
	UINT32 cpDeltaTxNormalNoFecTime,
	UINT32 cpTxMpcpNoFecTime
    );

/*******************************************************************************
* dalPonMpcpDbaAgentConfigAllGet
*
* DESCRIPTION:
*  		this function is used to set mpcp dba agent delay config
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMpcpDbaAgentConfigAllGet(
	UINT32 *cpRptStaMode,
    UINT32 *cpMergeRptSendMode,
    UINT32 *cpRptSendMode,
    UINT32 *cpPktInitTimerCfg,
	UINT32 *cpDeltaTxNormalNoFecTime,
	UINT32 *cpTxMpcpNoFecTime
    );

/*******************************************************************************
* dalPonDelayConfigAllSet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonDelayConfigAllSet(UINT8 cpTxDlyEn,UINT8 cpSyncEn,UINT16 cpRptsentDelt,UINT16 cpTxDly);

/*******************************************************************************
* dalPonDelayConfigAllGet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonDelayConfigAllGet(UINT32 *cpTxDlyEn,UINT32 *cpSyncEn,UINT32 *cpRptsentDelt,UINT32 *cpTxDly);

OPL_STATUS dalPonMpcpStatusGet(DAL_PON_STATUS_t *ponStatus);

/*******************************************************************************
* dalPonOamFinishedSet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonOamFinishedSet(UINT32 oamFinished);

/*******************************************************************************
* dalPonMpcpFsmHoldOverCtrlSet
*
* DESCRIPTION:
*  		this function is used to enable or disable holdover feature 
*	INPUTS:
*		enable: 1-enable 0-disable
*		
*	OUTPUTS:
*		 
* 	RETURNS:
* 		OPL_OK if the access is success.
* 		OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMpcpFsmHoldOverCtrlSet(UINT32 enable);

/*******************************************************************************
* dalPonMpcpFsmHoldOverCtrlGet
*
* DESCRIPTION:
*  		this function is used to get enable status of holdover feature 
*	INPUTS:
*		
*	OUTPUTS:
*		enable: 0-disable 1-enable
*		 
* 	RETURNS:
* 		OPL_OK if the access is success.
* 		OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMpcpFsmHoldOverCtrlGet(UINT32 *enable);


/*******************************************************************************
* dalPonMpcpFsmHoldOverTimeSet
*
* DESCRIPTION:
*  		this function is used to set holdover time 
*	INPUTS:
*		holdTime: holdover time
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 		OPL_OK if the access is success.
* 		OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMpcpFsmHoldOverTimeSet(UINT32 holdTime);

/*******************************************************************************
* dalPonMpcpFsmHoldOverTimeGet
*
* DESCRIPTION:
*  		this function is used to get holdover time
*	INPUTS:
*		
*		
*	OUTPUTS:
*		holdTime: holdover time
*
* 	RETURNS:
* 		OPL_OK if the access is success.
* 		OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMpcpFsmHoldOverTimeGet(UINT32 *holdTime);

OPL_STATUS dalOnuMtuSet(UINT32 mtuSize);

/*******************************************************************************
* dalOnuMtuGet
*
* DESCRIPTION:
*  		this function is used to get onu mtu info 
*	INPUTS:
*		mtuSize: mtu size

*
* 	RETURNS:
* 		OPL_OK if the access is success.
* 		OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalOnuMtuGet(UINT32 *mtuSize);

OPL_STATUS dalOnuPonLoopbackConfig(UINT32 uniorscb, UINT32 enable);

OPL_STATUS dalOnuPonTxPolarityConfig(UINT32 loworhigh);


#endif
