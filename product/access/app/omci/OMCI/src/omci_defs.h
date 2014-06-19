 /*
  * Copyright (C) 2012 Realtek Semiconductor Corp. 
  * All Rights Reserved.
  *
  * This program is the proprietary software of Realtek Semiconductor
  * Corporation and/or its licensors, and only be used, duplicated, 
  * modified or distributed under the authorized license from Realtek. 
  *
  * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
  * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
  *
  * $Revision: 39101 $
  * $Date: 2013-05-03 04:35:27 -0500 (Fri, 03 May 2013) $
  *
  * Purpose : Definition of SVLAN API
  *
  * Feature : The file includes the following modules and sub-modules
  * 		  (1) OAM (802.3ah) configuration
  *
  */

#ifndef __OMCI_DEFS_H__
#define __OMCI_DEFS_H__

#ifdef  __cplusplus
extern "C" {
#endif


#include <omci_driver.h>

#define OMCI_APPL 0x00000800


typedef UINT32 OMCI_APPL_ID;
typedef INT32 OMCI_TASK_ID;
typedef INT32 OMCI_MSG_Q_ID;

typedef enum
{
    OMCI_RX_OMCI_MSG,
    OMCI_AVC_MSG,
    OMCI_TIMEOUT_MSG,
    OMCI_CMD_MSG,
} OMCI_MSG_TYPE;

typedef enum
{
    OMCI_MSG_PRI_URGENT = 1,
    OMCI_MSG_PRI_NORMAL,
    OMCI_MSG_PRI_MAX
} OMCI_MSG_PRI;

typedef enum 
{
	OMCI_TASK_PRI_MIN  = 0,
	OMCI_TASK_PRI_MIB  = 25,
	OMCI_TASK_PRI_MAIN = 50, 
	OMCI_TASK_PRI_MAX  = 99
   
} OMCI_TASK_PRI_T;


typedef GOS_ERROR_CODE (*OMCI_TASK_ENTRY)(void);
typedef GOS_ERROR_CODE (*OMCI_APPL_INIT_PTR)(void);
typedef GOS_ERROR_CODE (*OMCI_APPL_DEINIT_PTR)(void);
typedef GOS_ERROR_CODE (*OMCI_APPL_MSG_HANDLER_PTR)(void* pData, OMCI_MSG_TYPE type, OMCI_MSG_PRI pri, OMCI_APPL_ID srcAppl);



extern BOOL gOmciProcOn;
extern BOOL gOmciLogEnable;
extern int gOmciLogLevel;


/* export interfaces in OMCI_task.c */
GOS_ERROR_CODE OMCI_AppInit(OMCI_APPL_ID appId, const CHAR* appName);
GOS_ERROR_CODE OMCI_AppStart(OMCI_APPL_INIT_PTR pInitFn, OMCI_APPL_MSG_HANDLER_PTR pMsgHandlerFn, OMCI_APPL_DEINIT_PTR pDeinitFn);
OMCI_TASK_ID OMCI_SpawnTask(const CHAR* pTaskName, OMCI_TASK_ENTRY pEntryFn, UINT32 priority);
void OMCI_TaskDelay(UINT32 num);
GOS_ERROR_CODE OMCI_SendMsg(OMCI_APPL_ID destAppl, OMCI_MSG_TYPE type, OMCI_MSG_PRI pri, void* pData, UINT32 dataLen);
GOS_ERROR_CODE OMCI_RecvFromMsgQ(OMCI_MSG_Q_ID msgId, void* pMsgHdr, UINT32 uiMaxBytes, INT32 msgType);


/* export interfaces in OMCI_main.c */
GOS_ERROR_CODE OMCI_Init(void);
GOS_ERROR_CODE OMCI_DeInit(void);


/* export interfaces in OMCI_message.c */
GOS_ERROR_CODE OMCI_ResetHistoryRspMsg(void);
GOS_ERROR_CODE OMCI_ResetLastRspMsgIndex(void);
GOS_ERROR_CODE OMCI_HandleMsg(void* pData, OMCI_MSG_TYPE type, OMCI_MSG_PRI pri, OMCI_APPL_ID srcAppl);
GOS_ERROR_CODE OMCI_SendIndicationToOlt(PON_OMCI_MSG_H_T* pNorOmciMsg, UINT16 tr);
GOS_ERROR_CODE OMCI_NormalizeMsg(PON_OMCI_MSG_T* pOmciMsg, PON_OMCI_MSG_H_T* pNorOmciMsg);
GOS_ERROR_CODE OMCI_ResponseMsg(PON_OMCI_MSG_H_T* pNorOmciMsg, PON_OMCI_MSG_RESULT result, BOOL clear);


/* export interfaces in OMCI_action.c */
GOS_ERROR_CODE OMCI_InitStateMachines(void);
GOS_ERROR_CODE OMCI_OnCreateMsg(PON_OMCI_MSG_H_T* pNorOmciMsg);
GOS_ERROR_CODE OMCI_OnDeleteMsg(PON_OMCI_MSG_H_T* pNorOmciMsg);
GOS_ERROR_CODE OMCI_OnSetMsg(PON_OMCI_MSG_H_T* pNorOmciMsg);
GOS_ERROR_CODE OMCI_OnGetMsg(PON_OMCI_MSG_H_T* pNorOmciMsg);
GOS_ERROR_CODE OMCI_OnGetNextMsg(PON_OMCI_MSG_H_T* pNorOmciMsg);
GOS_ERROR_CODE OMCI_OnMibReset(PON_OMCI_MSG_H_T* pNorOmciMsg);
GOS_ERROR_CODE OMCI_OnActivateSw(PON_OMCI_MSG_H_T* pNorOmciMsg);
GOS_ERROR_CODE OMCI_OnCommitSw(PON_OMCI_MSG_H_T* pNorOmciMsg);
GOS_ERROR_CODE OMCI_OnReboot(PON_OMCI_MSG_H_T* pNorOmciMsg);
GOS_ERROR_CODE OMCI_OnMibUploadMsg(PON_OMCI_MSG_H_T* pNorOmciMsg);
GOS_ERROR_CODE OMCI_OnMibUploadNextMsg(PON_OMCI_MSG_H_T* pNorOmciMsg);
GOS_ERROR_CODE OMCI_AttrsMibSetToOmciSet(MIB_ATTRS_SET* pMibAttrSet, PON_OMCI_ATTRS_SET* pOmciAttrSet);
BOOL OMCI_CheckIsActionSupported(UINT16 meClassID, UINT8 type);
GOS_ERROR_CODE OMCI_CheckIsMeValid(UINT16 MeClassID, UINT16* pMeInstance, BOOL* pMeValid, BOOL* pMeExist);

    
/* export interfaces from OMCI_mib.c */
GOS_ERROR_CODE OMCI_ResetMib(void);
GOS_ERROR_CODE OMCI_InitMib(void);


/* export interfaces from OMCI_config.c */
GOS_ERROR_CODE OMCI_MeOperCfg(MIB_TABLE_INDEX tableIndex, void* pOldRow, void* pNewRow, MIB_OPERA_TYPE operationType);


/* export interfaces from OMCI_driver.c */
GOS_ERROR_CODE OMCI_initDriv(void);
GOS_ERROR_CODE OMCI_createGmacDev(void);
GOS_ERROR_CODE OMCI_deleteGmacDev();
GOS_ERROR_CODE OMCI_waitGmacMsg(void);
GOS_ERROR_CODE OMCI_wrapper_SendOmciMsg(void* rawMsg);
GOS_ERROR_CODE OMCI_wrapper_CreateTcont(UINT16 entityId,UINT16 allocId, PON_US_TRAFF_MG_OPTION tmOpt, UINT16* tcontId);
GOS_ERROR_CODE OMCI_wrapper_DeleteTcont(UINT16 entityId,UINT16 allocId);
GOS_ERROR_CODE OMCI_wrapper_CfgGemFlow(OMCI_GEM_FLOW_ts data);
GOS_ERROR_CODE OMCI_wrapper_ActiveBdgConn(OMCI_TRAFF_RULE_ts *pData);
GOS_ERROR_CODE OMCI_wrapper_DeactiveBdgConn(OMCI_TRAFF_RULE_ts data);

enum {
	OMCI_LOG_LEVEL_HIGH,
	OMCI_LOG_LEVEL_NORMAL,
	OMCI_LOG_LEVEL_LOW
};


#define OMCI_LOG(level,fmt, arg...) \
            do { if (gOmciLogEnable==TRUE && gOmciLogLevel >= level) { printf(fmt, ##arg); printf("\n"); } } while (0);



#include "omci_mib.h"


#ifdef  __cplusplus
}
#endif

#endif //__OMCI_DEFS_H__

 
