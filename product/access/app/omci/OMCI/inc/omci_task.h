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
 * Purpose : Definition of OMCI Task API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) OMCI Task APIs
 *
 */
 #ifndef _OMCI_TASK_H_
 #define _OMCI_TASK_H_

#include "app_basic.h"
#include "omci_defs.h"




#define OMCI_MSG_ID_INVALID             ((OMCI_MSG_Q_ID)-1)
#define OMCI_MAX_BYTE_PER_MSG           (1024*6)
#define OMCI_MSGQKEY(applId, taskIndex) ((applId) + (taskIndex)) 


typedef UINT32 OMCI_APPL_PID;
#define OMCI_APPL_ID_INVALID           ((OMCI_APPL_ID)OMCI_MAX_NUM_OF_APPL)

#define OMCI_TASK_ID_INVALID           ((OMCI_TASK_ID)ERROR)


#define OMCI_MAX_NUM_OF_APPL           (1)
#define OMCI_MAX_TASKS_PER_APPL        (8)
#define OMCI_TASK_NAME_MAX_LEN          (32)


typedef struct  
{
    OMCI_MSG_PRI     priority;  
    UINT32           srcApplId; 
    OMCI_MSG_TYPE    type;
    UINT32           srcMsgQKey;
    UINT32           len;       
} OMCI_MSG_HDR_T;



typedef struct 
{
	OMCI_TASK_ID    taskId;                      
	CHAR            name[OMCI_TASK_NAME_MAX_LEN]; 
	OMCI_APPL_ID    applId;                      
	OMCI_TASK_ENTRY pEntryFn;                    
	UINT32          taskNumber;                  
	OMCI_MSG_Q_ID   msgQId;
	UINT32          msgQKey;
	UINT32 			priority;
} OMCI_TASK_INFO_T;

typedef struct
{
    CHAR                      name[OMCI_TASK_NAME_MAX_LEN];
    OMCI_APPL_ID              applId;
    OMCI_TASK_ID              mainTaskId;                     
    OMCI_APPL_PID             applPid;                        
    OMCI_TASK_INFO_T          tasks[OMCI_MAX_TASKS_PER_APPL];
    OMCI_MSG_Q_ID             msgQId;
    OMCI_APPL_INIT_PTR        init;
    OMCI_APPL_DEINIT_PTR      deinit;
    OMCI_APPL_MSG_HANDLER_PTR msgHandler;
} OMCI_APPL_INFO_T;

#endif

