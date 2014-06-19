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

#include <omci_task.h>

static OMCI_APPL_INFO_T* spOmciApplInfo = NULL;




/*
*   Define local function
*/


static OMCI_MSG_Q_ID omci_CreateMsgQ(UINT32 msgKey)
{
    OMCI_MSG_Q_ID msgId;
    

    if ((msgId = msgget(msgKey, S_IRUSR|S_IWUSR|IPC_CREAT)) == -1) 
    { 
        return OMCI_MSG_ID_INVALID;
    } 
	
    return msgId;
}


static GOS_ERROR_CODE omci_DeleteMsgQ(OMCI_MSG_Q_ID msgId)
{
    if (OMCI_MSG_ID_INVALID == msgId)
    {
        return GOS_ERR_PARAM;
    }
    
    if( 0 != msgctl(msgId, IPC_RMID, NULL))
    {
        return GOS_ERR_SYS_CALL_FAIL;
    }

    return GOS_OK;
}


static void omci_SigSegvHandle(INT32 signum, siginfo_t* info, void*ptr) 
{
    fprintf(stderr, "! Segmentation Fault ! tid(%d)\n", getpid());
    exit (-1);
}


static GOS_ERROR_CODE omci_ApplEntry(void)
{
    OMCI_APPL_INFO_T* pAppInfo = spOmciApplInfo;
    CHAR*             pUsrData = NULL;
    OMCI_MSG_HDR_T*   pMsg; 
    CHAR              msgBuff[OMCI_MAX_BYTE_PER_MSG];

    pMsg = (OMCI_MSG_HDR_T*)msgBuff;

    if (pAppInfo->msgHandler)
    {
        while (1)
        {
            if (GOS_OK == OMCI_RecvFromMsgQ(pAppInfo->msgQId, pMsg, OMCI_MAX_BYTE_PER_MSG, -1 * OMCI_MSG_PRI_NORMAL))
            {
                if (pMsg->len >= sizeof(OMCI_MSG_HDR_T))
                {
                    pUsrData = (CHAR*)pMsg + sizeof(OMCI_MSG_HDR_T);
                }

                (*pAppInfo->msgHandler)(pUsrData, pMsg->type, pMsg->priority, pMsg->srcApplId);
            }
        }
    }

    return GOS_OK;
}


static void omci_DeinitApp(INT32 arg)
{
    OMCI_APPL_INFO_T* pAppInfo = spOmciApplInfo;

    if (pAppInfo->deinit)
    {
        (*pAppInfo->deinit)();
    }

    omci_DeleteMsgQ(pAppInfo->msgQId);
    pAppInfo->msgQId = OMCI_MSG_ID_INVALID;

    exit(0);
}

static GOS_ERROR_CODE omci_TaskEnty(UINT32 taskIndex)
{
    OMCI_APPL_INFO_T* pAppInfo = spOmciApplInfo;


    GOS_ASSERT(pAppInfo);

    while (OMCI_TASK_ID_INVALID == pAppInfo->tasks[taskIndex].taskId)
    {
        OMCI_TaskDelay(100);
    }

    if (pAppInfo->tasks[taskIndex].pEntryFn)
    {
        setpriority(0, 0, 0);
        (*((OMCI_TASK_ENTRY)pAppInfo->tasks[taskIndex].pEntryFn))();
    }

    omci_DeleteMsgQ(pAppInfo->tasks[taskIndex].msgQId);
    pAppInfo->tasks[taskIndex].msgQId = OMCI_MSG_ID_INVALID;

    pAppInfo->tasks[taskIndex].taskId = OMCI_TASK_ID_INVALID;

    
    return GOS_OK;
}



static OMCI_MSG_HDR_T* omci_CreateMsg(OMCI_MSG_HDR_T* sendBuf, OMCI_MSG_TYPE type, OMCI_MSG_PRI pri, UINT32 len)
{
    OMCI_TASK_INFO_T* taskInfo = NULL;
    OMCI_APPL_INFO_T* pAppInfo = spOmciApplInfo;
    
    GOS_ASSERT(sendBuf != NULL);


    sendBuf->len       = len;
    sendBuf->priority  = pri;
    sendBuf->type      = type;
    sendBuf->srcApplId = 0; //pAppInfo->applId;

    if (taskInfo)
    {
        sendBuf->srcMsgQKey = taskInfo->msgQKey;
    }

    return sendBuf;
}

/*
*  Define Global Function for Task 
*/

GOS_ERROR_CODE OMCI_AppInit(OMCI_APPL_ID appId, const CHAR* appName)
{
    OMCI_APPL_INFO_T* pAppInfo;
    OMCI_TASK_INFO_T  taskInfo;
    UINT32           taskIndex;

    struct sigaction action;


    memset(&action, 0, sizeof(action));
    action.sa_sigaction = omci_SigSegvHandle;
    action.sa_flags     = SA_SIGINFO;
    
    if (sigaction(SIGSEGV, &action, NULL) < 0)
    {
        return GOS_FAIL;
    }
    
    spOmciApplInfo = (OMCI_APPL_INFO_T*)malloc(sizeof(OMCI_APPL_INFO_T));
    GOS_ASSERT(spOmciApplInfo != NULL);
    memset(spOmciApplInfo, 0x00, sizeof(OMCI_APPL_INFO_T));
    
    pAppInfo = spOmciApplInfo;

    for (taskIndex = 0; taskIndex < OMCI_MAX_TASKS_PER_APPL; taskIndex++)
    {
        memset(&pAppInfo->tasks[taskIndex], 0x00, sizeof(OMCI_TASK_INFO_T));
        pAppInfo->tasks[taskIndex].taskId = OMCI_TASK_ID_INVALID;
    }

    pAppInfo->applId = appId;
    strcpy(pAppInfo->name, appName);
    pAppInfo->applPid    = getpid();
    pAppInfo->mainTaskId = pthread_self();
    
    pAppInfo->msgQId = omci_CreateMsgQ(OMCI_MSGQKEY(appId, 0));
    GOS_ASSERT(pAppInfo->msgQId != OMCI_MSG_ID_INVALID);

    memset(&taskInfo, 0x00, sizeof(OMCI_TASK_INFO_T));
    taskInfo.applId     = pAppInfo->applId;
    strcpy(taskInfo.name, appName);
    taskInfo.pEntryFn   = NULL;
    taskInfo.taskNumber = 0;
    taskInfo.msgQId     = pAppInfo->msgQId;
    taskInfo.msgQKey    = OMCI_MSGQKEY(pAppInfo->applId, 0);
    taskInfo.taskId     = pAppInfo->mainTaskId;
    memcpy(&(pAppInfo->tasks[0]), &taskInfo, sizeof(OMCI_TASK_INFO_T));

    return GOS_OK;
}

GOS_ERROR_CODE OMCI_AppStart(OMCI_APPL_INIT_PTR pInitFn, OMCI_APPL_MSG_HANDLER_PTR pMsgHandlerFn, OMCI_APPL_DEINIT_PTR pDeinitFn)
{
    OMCI_APPL_INFO_T* pAppInfo =  spOmciApplInfo;
    
    pAppInfo->init       = pInitFn;
    pAppInfo->msgHandler = pMsgHandlerFn;
    pAppInfo->deinit     = pDeinitFn;

    if (pInitFn)
    {
        (*pInitFn)();
    }

    signal(SIGTERM, omci_DeinitApp);

    omci_ApplEntry();

    omci_DeinitApp(0);

    return GOS_OK;
}





OMCI_TASK_ID OMCI_SpawnTask(const CHAR* pTaskName, OMCI_TASK_ENTRY pEntryFn,UINT32 priority)
{
    OMCI_APPL_INFO_T* pAppInfo = spOmciApplInfo;
    OMCI_TASK_INFO_T  taskInfo;
    UINT32            taskIndex;
    pthread_t         thread_a;
    struct sched_param sched;

    GOS_ASSERT(pAppInfo);
    memset(&taskInfo, 0x00, sizeof(OMCI_TASK_INFO_T));
    
    taskInfo.applId    = pAppInfo->applId;
    strcpy(taskInfo.name, pTaskName);
    taskInfo.taskId    = OMCI_TASK_ID_INVALID;
    taskInfo.pEntryFn  = pEntryFn;
    taskInfo.priority  = priority;
    
    for (taskIndex = 0; taskIndex < OMCI_MAX_TASKS_PER_APPL; taskIndex++)
    {
        if (pAppInfo->tasks[taskIndex].taskId == OMCI_TASK_ID_INVALID)
        {
            taskInfo.taskNumber = taskIndex;
            memcpy(&(pAppInfo->tasks[taskIndex]), &taskInfo, sizeof(OMCI_TASK_INFO_T));
            break;
        }
    }
    if (OMCI_MAX_TASKS_PER_APPL == taskIndex)
    {
        return OMCI_TASK_ID_INVALID;
    }
    pAppInfo->tasks[taskIndex].msgQId = omci_CreateMsgQ(OMCI_MSGQKEY(pAppInfo->applId, taskIndex));
		
    if (OMCI_MSG_ID_INVALID == pAppInfo->tasks[taskIndex].msgQId)
    {
        pAppInfo->tasks[taskIndex].taskId = OMCI_TASK_ID_INVALID;
        return OMCI_TASK_ID_INVALID;
    }
    pAppInfo->tasks[taskIndex].msgQKey = OMCI_MSGQKEY(pAppInfo->applId, taskIndex);

    if (0 != pthread_create(&thread_a, NULL, (void*)omci_TaskEnty, (void*)taskIndex))
    {
        GOS_ASSERT(0);
    }
    
    /*set priority*/
    sched.sched_priority = priority;
    if(0 != pthread_setschedparam(thread_a,SCHED_FIFO,&sched))
    {
	GOS_ASSERT(0);
    }

    pAppInfo->tasks[taskIndex].taskId = (OMCI_TASK_ID)thread_a;

    if (0 != pthread_detach(thread_a))
    {
        GOS_ASSERT(0);
    }
    
    return pAppInfo->tasks[taskIndex].taskId;
}




void OMCI_TaskDelay(UINT32 num)
{
    UINT32 i = 0 ;
    
    for (i = 0; i < num / 10; i++)
    {
        usleep(10000);
    }
}


GOS_ERROR_CODE OMCI_RecvFromMsgQ(OMCI_MSG_Q_ID msgId, void* pMsgHdr, UINT32 uiMaxBytes, INT32 msgType)
{
    int size;

    
    if (OMCI_MSG_ID_INVALID == msgId)
    {
        return GOS_ERR_PARAM;
    }

    if (NULL == pMsgHdr)
    {
        return GOS_ERR_PARAM;
    }
    
    if (uiMaxBytes > OMCI_MAX_BYTE_PER_MSG)
    {
        return GOS_ERR_PARAM;
    }

    if (ERROR == (size = msgrcv(msgId, pMsgHdr, uiMaxBytes, msgType, 0)))  
    {
        return GOS_ERR_SYS_CALL_FAIL;
    }

    if (0 == size)
    {
        return GOS_FAIL;
    }
    
    return GOS_OK;
}


GOS_ERROR_CODE OMCI_SendToMsgQ(UINT32 msgKey, OMCI_MSG_HDR_T* pMsg, UINT32 len)
{
    OMCI_MSG_Q_ID msgQId;
    UINT32        tryCounter = 0;
    
    if (NULL == pMsg)
    {
        return GOS_ERR_PARAM;
    }
    if(len > OMCI_MAX_BYTE_PER_MSG)
    {
        return GOS_ERR_PARAM;
    }

    msgQId = msgget(msgKey, S_IRUSR|S_IWUSR);

    if (OMCI_MSG_ID_INVALID == msgQId)
    {
        return GOS_ERR_NOT_FOUND;
    }

    while (1)
    {
        if (0 != msgsnd(msgQId, pMsg, len - 4, IPC_NOWAIT))  
        {
            if ((EAGAIN == errno) && (tryCounter < 3))
            {
                tryCounter++;
                OMCI_TaskDelay(100);
                continue;
            }
            else
            {
                 return GOS_ERR_SYS_CALL_FAIL;
            }
        }
        else
        {
            return GOS_OK;
        }
    }

    return GOS_OK;
}


GOS_ERROR_CODE OMCI_SendMsg(OMCI_APPL_ID destAppl, OMCI_MSG_TYPE type, OMCI_MSG_PRI pri, void* pData, UINT32 dataLen)
{
    GOS_ERROR_CODE    ret;
    CHAR              sendBuf[OMCI_MAX_BYTE_PER_MSG];
    OMCI_MSG_HDR_T*   pHdr = NULL;

    GOS_ASSERT((dataLen + sizeof(OMCI_MSG_HDR_T)) <= sizeof(sendBuf));
    memset(sendBuf, 0, sizeof(sendBuf));
    
    pHdr = omci_CreateMsg((OMCI_MSG_HDR_T*)sendBuf, type, pri, dataLen + sizeof(OMCI_MSG_HDR_T));

    GOS_ASSERT(pHdr != NULL);
    
    if ((NULL !=  pData) && (0 != dataLen))
    {
        memcpy(((CHAR*)pHdr) + sizeof(OMCI_MSG_HDR_T), pData, dataLen);
    }

    ret = OMCI_SendToMsgQ(OMCI_MSGQKEY(destAppl, 0), pHdr, pHdr->len);

    return ret;
}


OMCI_TASK_INFO_T* OMCI_GetTaskInfo(OMCI_TASK_ID taskId)
{
    UINT32            taskIndex;
    OMCI_APPL_INFO_T* pAppInfo = spOmciApplInfo;


    if (OMCI_TASK_ID_INVALID == taskId)
    {
        return NULL;
    }
    
    GOS_ASSERT(pAppInfo);

    for (taskIndex = 0; taskIndex < OMCI_MAX_TASKS_PER_APPL; taskIndex++)
    {
        if (pAppInfo->tasks[taskIndex].taskId == taskId)
        {
            return &pAppInfo->tasks[taskIndex];
        }
    }

    return NULL;
}

