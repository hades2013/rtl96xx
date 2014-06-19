/*
=============================================================================
     Header Name: pon_mac.h

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

#ifndef  __PON_MAC_H__
#define __PON_MAC_H__

#include <vos.h>
#include "errors.h"

#define ODM_PON_PRINTF(x) printf x
#define ODM_PON_TRACE() ODM_PON_PRINTF(("%s,%s,%d.\n",__FILE__,__FUNCTION__,__LINE__))

#define ODM_PON_MAX_LASERON                     256
#define ODM_PON_MAX_LASEROFF                    256
#define ODM_PON_MAX_SERDESDELAY                 256
#define ODM_PON_DEFAULT_LASERON                 0x20 
#define ODM_PON_DEFAULT_LASEROFF                0X20
#define ODM_PON_DEFAULT_SERDESDELAY             0xc
#define ODM_PON_DEFAULT_SILENCE_TIME			60
#define ODM_PON_DEFAULT_SILENCE_CTRL			1

#define ODM_Q_SET_NUM                       2
#define ODM_Q_NUM                           8  

#define ODM_QSET0_DEFAULT_THRESHOLD         (0X0400)
#define ODM_QSET1_DEFAULT_THRESHOLD         (0X0800)

#define CFG_PON_MAC_CFG            "/cfg/pmac.conf"

#define CFG_PON_MAC_SECTION 	    "Pmac"

#define CFG_PON_LOID_SN				"Onusn"/*pippo add for sn get/set*/
#define CFG_PON_LASER_ON            "Laseron"
#define CFG_PON_LASER_OFF           "Laeroff"
#define CFG_PON_SERDES_DELAY        "Serdesdelay"
#define CFG_PON_MACID               "Ponmacid"
#define CFG_PON_FEC                 "Ponfec"

#define  CFG_MPCP_DYINGGASP_CFG            "/cfg/mpcpdygasp.conf"
#define  CFG_MPCP_DYINGGASP_SECTION            "Dygaspoamnum"
#define  UNI_LLID_LP_ENABLE 1
#define  UNI_LLID_LP_DISABLE 0
#define  SCB_LLID_LP_ENABLE 1
#define  SCB_LLID_LP_DISABLE 0
#define  PON_TX_LOW_ACTIVE 0
#define  PON_TX_HIGH_ACTIVE 1
#define CFG_MPCP_DYINGGASP_OAMNUM                 "Dyinggaspoamnum"
#define CFG_PON_TX_POLARITY         "Pontxpolarity"
#define CFG_PON_LP_UNI                 "Ponuniloopback"
#define CFG_PON_LP_SCB                 "Ponscbloopback"

#define CFG_MPCP_HOLDOVER_CTRL		"holdoverCtrl"
#define CFG_MPCP_HOLDOVER_TIME		"holdoverTime"
#define CFG_ONU_SILENCE_CTRL		"SilenceCtrl"
#define CFG_ONU_SILENCE_TIME		"SilenceTime"

#define CFG_PON_US_SHAPING_EN		"UsShaping"
#define CFG_PON_US_SHAPING_CIR		"Uscir"
#define CFG_PON_US_SHAPING_CBS		"Uscbs"
#define CFG_PON_DS_SHAPING_EN		"DsShaping"
#define CFG_PON_DS_SHAPING_CIR		"Dscir"
#define CFG_PON_DS_SHAPING_CBS		"Dscbs"

#define CFG_PON_DBA_SECTION	        "Pdba"
#define CFG_RPT_SNED_MODE               "Rptsendmode"
#define CFG_DELTA_TX_NORMAL_FEC_TIME    "Txnormalfectime"
#define CFG_DELTA_TX_NORMAL_NO_FEC_TIME "Txnormalnofectime"
#define CFG_DELTA_TX_MPCP_FEC_TIME      "Txmpcpfectime"
#define CFG_DELTA_TX_MPCP_NO_FEC_TIME   "Txmpcpnofectime"

#define CFG_PON_DELAY_SECTION       "Pdelay"

#define CFG_CP_RPTSENT_DELT         "Rptsenddelta"
#define CFG_CP_RTTCOMPEN_EN         "Rttcomen"
#define CFG_CP_RTTCOMPEN_SYNC_EN    "Rttcomsyncen"
#define CFG_CP_RTTCORRECT_EN        "Rttcorrecten"    
#define CFG_CP_RTTCORRECT_SYNC_EN   "Rttcorrectsyncen" 
#define CFG_CP_RTT_OFFSET           "Rttoffset"
#define CFG_CP_TXDLY_EN             "Txdlyen"    
#define CFG_CP_SYNC_EN              "Syncen"
#define CFG_CP_TX_DLY               "Txdlycnt"

#define CFG_REPORT_SECTION          "Preport"
#define CFG_REPORT_MODE             "Reportmode"

#define CFG_QSET_SECTION            "Qset%d"
#define CFG_QSET_THRESHOLD          "QsetQ%d.threshold"

#define CFG_PON_REG_SECTION         "Reg"
#define CFG_PON_REG_VALUE	        "Reg.0x%04x"

#define CFG_PON_TAB_SECTION         "Tab%04d"
#define CFG_PON_TAB_REC_VAL         "Rec%04d"


#define CFG_PON_LOID_CFG            "/cfg/loid.conf"

#define CFG_PON_LOID_SECTION 	    "General"
#define CFG_PON_LOID_KEY			"Loid"
#define CFG_PON_LOID_PASSWORD		"Password"

/* Eric Yang add section to  save OAM version */
#define CFG_PON_OAM_VER_CFG         "OAMVer"
/* !Eric Yang */

#define CFG_PON_MTU_SIZE              "MtuSize"

#define CFG_PON_BER_MAX             0.000000000001

typedef struct ODM_PON_STATUS_s
{
    UINT32 bfGatePendingNum:5;
	UINT32 bfRsved0:8;
	UINT32 bfDiscLEvent:4;
	UINT32 bfDiscCEvent:4;
	UINT32 bfDiscLState:4;
	UINT32 bfDiscCState:4;
}ODM_PON_STATUS_t;

typedef enum ODM_PON_REPORT_MODE_s{
    ODM_PON_REPORT_65535 = 0,    
    ODM_PON_REPORT_BASE_THRESHOLD = 1
}ODM_PON_REPORT_MODE_e;

typedef enum ODM_SECURITY_KEY_s{
    ODM_SECURITY_KEY_0 = 0,
    ODM_SECURITY_KEY_1 = 1    
}ODM_SECURITY_KEY_e;

typedef enum ODM_SECURITY_MODE_s{
    ODM_SECURITY_CTC_3 = 0,
    ODM_SECURITY_AES = 1    
}ODM_SECURITY_MODE_e;

typedef enum ODM_LIGHT_MODE_s{
	 ODM_LIGHT_OFF = 0,
	 ODM_LIGHT_ON,
	 ODM_LIGHT_NORMAL,
	 ODM_LIGHT_ON_A_WHILE,
	 ODM_LIGHT_BUTT 
}ODM_LIGHT_MODE_E;

#ifdef __cplusplus
extern "C" {
#endif

UINT32 odmPonLaserOnSet(UINT32 laserOnTq);

UINT32 odmPonLaserOnGet(UINT32 *laserOnTq);

UINT32 odmPonLaserOffSet(UINT32 laserOffTq);

UINT32 odmPonLaserOffGet(UINT32 *laserOffTq);

UINT32 odmPonSerdesDelaySet(UINT32 serdesDelayByte);

UINT32 odmPonSerdesDelayGet(UINT32 *serdesDelayByte);

UINT32 odmPonMacIdSet(UINT8 macid[]);

UINT32 odmPonMacIdGet(UINT8 macid[]);

UINT32 odmPonDbaAgentAllParaSet(UINT32 cpRptSendMode,UINT32 cpDeltaTxNormalFecTime,UINT32 cpDeltaTxNormalNoFecTime,
    UINT32 cpTxMpcpFecTime,UINT32 cpTxMpcpNoFecTime);

UINT32 odmPonDbaAgentAllParaGet(UINT32 *cpRptSendMode,UINT32 *cpDeltaTxNormalFecTime,UINT32 *cpDeltaTxNormalNoFecTime,
    UINT32 *cpTxMpcpFecTime,UINT32 *cpTxMpcpNoFecTime);

UINT32 odmPonTxDelaySet(UINT32  cpRttCompenEn,UINT32 cpRttCompenSyncEn,UINT32 cpRttCorrectEn,UINT32 cpRttCorrectSyncEn,
	UINT32 cpTxDlyEn,UINT32 cpSyncEn,UINT32 cpRptsentDelt,UINT32 cpRttOffset,UINT32 cpTxDly);

UINT32 odmPonTxDelayGet(UINT32  *cpRttCompenEn,UINT32 *cpRttCompenSyncEn,UINT32 *cpRttCorrectEn,UINT32 *cpRttCorrectSyncEn,
	UINT32 *cpTxDlyEn,UINT32 *cpSyncEn,UINT32 *cpRptsentDelt,UINT32 *cpRttOffset,UINT32 *cpTxDly);

UINT32 odmPonRegSet(UINT32 regId,UINT32 regVal);

UINT32 odmPonRegGet(UINT32 regId,UINT32 *pVal);

UINT32 odmPonMultiTabGet(INT32 nWriteFd,UINT32 tabId,UINT32 startRecId,UINT32 numOfRec);

UINT32 odmPonTabInfoGet(INT32 nWriteFd);

UINT32 odmPonUnicastTripleChunringKeyGet(UINT8 keyIndex,UINT8 *keyBuff);

UINT32 odmPonScbTripleChurningKeySet(UINT8 keyIndex,UINT8 *keyBuff);

UINT32 odmPonUnicastAesKeyGet(UINT8 keyIndex,UINT8 *keyBuff,UINT8 mode);

UINT32 odmPonScbAesKeySet(UINT8 keyIndex,UINT8 *keyBuff,UINT8 mode);

UINT32 odmPonSecurityModeGet(UINT8 *mode);

UINT32 odmPonSecurityModeSet(UINT8 mode);

UINT32 odmPonLoopbackConfig(UINT8 uniorscb,UINT32 enable);

UINT32 odmPonFecEnableSet(UINT32 enable);

UINT32 odmPonFecEnableGet(UINT32 *enable);

UINT32 odmPonStatusGet(ODM_PON_STATUS_t *odmPonStatus);

UINT32 odmPonOamFinishedSet(UINT32 oamFinished);

UINT32 odmPonLoidSet(UINT8 *pLoid);

UINT32 odmPonLoidGet(UINT8 *pLoid);

UINT32 odmPonPasswordSet(UINT8 *pPassword);

UINT32 odmPonPasswordGet(UINT8 *pPassword);

void odmMpcpCtrlThread(void);

UINT32 odmPonSilenceEnableSet(UINT32 enable);

UINT32 odmPonSilenceTimeSet(UINT32 silenceTime);

UINT32 odmPonMacIdInit();

/*******************************************************************************
* odmPonCfgInit
*
*   call ponMacIdSetTest("00:07:49:08:30:ff")
*    call owr(0xa80,0x10337)
*   call owr(0x4002,0x92d1d2a)
*   ##call owr(0x4002,0x892d1d2a)
*   call owr(0x400a,0xfffffff)
*   call owr(0x4008,0x20)       
*   call owr(0x4009,0x1a)       
*   call owr(0x4003,0x08000c20)
*   ##call owr(0x4003,0x10000C20)
*   call owr(0x4004,0x103)      
*   call owr(0x400e, 0x000f000f)
*   call owr(0x400f,0x807c007e)
*   ##call owr(0x20,3)            
*   call owr(0x0a19,5)        
*   call owr(0x20,0xc)
*   call owr(0x4006,0x42)
*   DESCRIPTION:
*  		set pon config to default value
*
*	INPUTS:
*		
*	OUTPUTS:
*		     
* 	RETURNS:
* 		NO_ERROR									success
*		FDB_MAC_SET_AGINGTIME_ERROR				set aging time error
* 		FDB_MAC_SET_CONFIG_FIEL_AGINGTIME_ERROR	set config file error
* 	SEE ALSO: 
*/
UINT32 odmPonCfgInit(void);

UINT32 odmPonTxPowerDown(UINT8 state);

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
UINT32 odmPonUsShapingSet(UINT8 enable,UINT32 cir,UINT32 cbs);
UINT32 odmPonUsShapingGet(UINT8 *enable,UINT32 *cir,UINT32 *cbs);
UINT32 odmPonDsShapingSet(UINT8 enable,UINT32 cir,UINT32 cbs);
UINT32 odmPonDsShapingGet(UINT8 *enable,UINT32 *cir,UINT32 *cbs);
#endif
UINT32 odmOnuMtuSet(UINT32 mtuSize);
UINT32 odmOnuMtuGet(UINT32 *mtuSize);
UINT32 odmOnuLuminateSet(ODM_LIGHT_MODE_E enMode);
UINT32 odmOnuLuminateGet(UINT32 *pulEnable);

#ifdef __cplusplus
}
#endif

#endif

