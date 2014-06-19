/*************************************************************************
*
*  COPYRIGHT (C) 2003-2009 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  vos_ipc.h
*
* DESCRIPTION: 
*	
*
* Date Created: Sep 21, 2009
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_ipc.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_IPC_H_
#define __VOS_IPC_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 

/***** DEFINES and ENUMS *****/
#define VOS_IPC_TYPE_UNUSED 0       // invalid entry
#define VOS_IPC_TYPE_SHARE  1       // share memory, semaphore
#define VOS_IPC_TYPE_SOCKET 2       // socket
#define VOS_IPC_TYPE_MSQ    3       // message queue

enum {
    VOS_IPC_ID_MAIN,
    VOS_IPC_ID_CLI_1, 
    VOS_IPC_ID_CLI_2,
    VOS_IPC_ID_CLI_3,
    VOS_IPC_ID_CLI_4,
    VOS_IPC_ID_CLI_5,
    VOS_IPC_ID_CLI_6,
    VOS_IPC_ID_OAM, 
    VOS_IPC_ID_SNMP, 
    VOS_IPC_ID_WEB, 
    VOS_IPC_ID_EXTERNAL_1, 
    VOS_IPC_ID_EXTERNAL_2, 
    VOS_IPC_ID_EXTERNAL_3, 
    VOS_IPC_ID_EXTERNAL_4, 
    VOS_IPC_ID_EXTERNAL_5, 
    VOS_IPC_ID_EXTERNAL_6,
    VOS_IPC_ID_EXTERNAL_7, 
    VOS_IPC_ID_EXTERNAL_8, 
    VOS_IPC_ID_EXTERNAL_9, 
    VOS_IPC_ID_EXTERNAL_10, 
    VOS_IPC_ID_MAX_DO_NOT_USE /* must be last one in the list, do not use this task name */
};

#define VOS_IPC_ID_TOTAL (VOS_IPC_ID_MAX_DO_NOT_USE)

/*
 * VOS_IPC_SEM1 to make sure only one CLI/SNMP/OAM/WEB user can access resource
 * (share memory).
 * VOS_IPC_SEM2 to wakeup the process in sync_wait
 * VOS_IPC_SEM3 to protect only one process can write to share memory
 * VOS_IPC_ASEM1 to save async queue size
 * VOS_IPC_ASEM2 to protect only one process can write to share memory
 */
enum { VOS_IPC_SEM1, VOS_IPC_SEM2, VOS_IPC_SEM3, VOS_IPC_SEM4, VOS_IPC_ASEM1, VOS_IPC_ASEM2 };
#define VOS_IPC_TOTAL_SEMA (VOS_IPC_ASEM2+1)

/*
 * VOS_IPC_ID_SEMA to protect only one process can write to share memory
 */
enum { VOS_IPC_ID_SEMA };
#define VOS_IPC_ID_TOTAL_SEMA (VOS_IPC_ID_SEMA+1)

#define VOS_IPC_DATA_SIZE 2048
#define VOS_IPC_ASYNC_SLOT 10           // Async queue size

#define VOS_IPC_SUCCESS 0
#define VOS_IPC_DATA_AVAILABLE 1

/* Error Codes */
#define VOS_IPC_GENERAL_ERR -1
#define VOS_IPC_QUEUE_EMPTY_ERR -2
#define VOS_IPC_QUEUE_FULL_ERR -3
#define VOS_IPC_TIMEOUT_ERR -4
#define VOS_IPC_RESOURCE_ERR -5
#define VOS_IPC_UNKNOWN_TYPE -6     // Not share memory, socket
#define VOS_IPC_SEMOP_ERR -7        // Semaphore operation error
#define VOS_IPC_RANGE_ERR -8        // variable out of valid range
#define VOS_IPC_UNKNOWN_CMD_ERR -9  // undefined command
#define VOS_IPC_BAD_CMD -10         // command error
#define VOS_IPC_CMD_ERR -11         // ENO command error
#define VOS_IPC_LICENSE_ERR -12     // Invalid license port
#define VOS_IPC_TIMEOUT_SET_SMALL -13   // Timeout values is set to smaller than stored one
#define VOS_IPC_BUSY -14            // Command data still in use, can't process
#define VOS_IPC_DUPLICATE -15       // Command data already defined


#define VOS_IPC_SEMA_LOCK 0
#define VOS_IPC_SEMA_AVAIL 1

#define VOS_IPC_SEM1_SET 1          // From user (e.g. CLI)
#define VOS_IPC_SEM1_CLEAR 0        // From daemon process (e.g. CM)

/*
 * These SEED definition were used to create unique key for share memory and
 * semaphore, so they should put in here.
 */
#define SEED_SHARE 'S'	// sync/async data area share memory
#define SEED_SEMA 'M'	// sync/async data area semaphore
#define SEED_ID_SHARE 'I'	// IPC ID table share memory
#define SEED_ID_SEMA 'i'	// IPC ID table semaphore
#define SEED_TC      'T'   // Task Control area shared memory


/***** STRUCTURES and UNIONS *****/
/***** TYPEDEFS *****/

typedef struct VOS_IPC_CTRL_s {       // IPC control structure
    int nIpcId;
    int nType;                   // 1: share memory/semaphore 2: socket
    int nSemaBase;              // Semaphore base number
} VOS_IPC_CTRL_t;

typedef struct VOS_IPC_CTRL_MEM_s {
    struct VOS_IPC_CTRL_s astIpcCtrl[VOS_IPC_ID_TOTAL];
    int nCount;           // Register count
} VOS_IPC_CTRL_MEM_t;


/***** EXTERNS *****/
/***** PROTOTYPES *****/
int vosIpcRegister(int /* VOS_IPC_ID */ );
void vosIpcUnregister(int /* VOS_IPC_ID */ );
void vosIpcDestroy(void);
int vosIpcSyncEventNotify(int /* dest_id */ , char * /* buf */ ,
                          int /* size */ ,
                          int /* millisecond */ ,
                          int flag /* use SEM1 */ );
int vosIpcSyncEventWait(int /* dest_id */  );
int vosIpcSyncEventReply(int /* dest_id */ , char * /* buf */ );
int vosIpcAsyncEventNotify(int /* dest_id */ , char * /* buf */ );
int vosIpcAsyncEventWait(int /* dest_id */ , char * /* buf */ );
int vosIpcAsyncEventPoll(int /* dest_id */ , char * /* buf */ );
void vosIpcDebugIdTable();
void vosIpcDebugTrace(char * /* data_buf */);
void vosIpcDebugTLVDataTrace(char * /* address */);
void vosIpcSleep(int /* millisecond */ );
 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_IPC_H_ */

