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
* FILENAME:  vos_ipc.c
*
* DESCRIPTION: Library functions for IPC
*	
*
* Date Created: Sep 21, 2009
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos_ipc.c#1 $
* $Log:$
*
*
**************************************************************************/


/***** INCLUDE FILES *****/
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/syslog.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <dirent.h>
#include <string.h>
#include <pthread.h>

#include "vos_ipc.h"
#include "vos_tlv.h"

//#define VOS_IPC_SHM_TEST 1
#define VOS_IPC_FOR_HAL

#ifdef VOS_IPC_FOR_HAL
#include "ipc_hal.h"
#endif

/***** LOCAL DEFINES and ENUM *****/
//#define VOS_IPC_LOCAL_DEBUG 1

#ifdef VOS_IPC_LOCAL_DEBUG
#define VOS_IPC_DEBUG printf
#else
#define VOS_IPC_DEBUG(fmt, ...) {}
#endif

#define DEFAULT_TEMP_PATH           "/tmp"
#define DEFAULT_TEMP_IPC_PATH       "/tmp/%08x"
#define VOS_IPC_PATH_LEN 64

extern void *vosIPCThreadInit(void *nipcId);
static pthread_t gPthreadArr[VOS_IPC_ID_MAX_DO_NOT_USE] = {0};


/***** LOCAL STRUCTURES and UNIONS *****/

typedef struct VOS_IPC_MEM_s {
    unsigned char aucSyncBuf[VOS_IPC_DATA_SIZE];      /* For sync communication */
    unsigned char aucAsyncBuf[VOS_IPC_ASYNC_SLOT][VOS_IPC_DATA_SIZE]; /* For async queues */
    int nAsyncHead;             /*  For Async Queue head */
    int nAsyncTail;             /* For Async Queue tail */
} VOS_IPC_MEM_t;

union SEM_u {
    int val;                    /* value for SETVAL */
    struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
    unsigned short int *array;  /* array for GETALL, SETALL */
    struct seminfo *__buf;      /* buffer for IPC_INFO */
};

/***** LOCAL TYPEDEFS ****/
/***** LOCAL (PRIVATE) PROTOTYPES *****/
static int vosIpcInitResources(key_t, key_t, int);
static void vosIpcIdNewEntry(int, int);
static struct VOS_IPC_CTRL_s *vosIpcGetEntryByIpcId(int);
static int vosIpcAsyncProcess(char *, struct VOS_IPC_CTRL_s *, char *);
static int vosIpcShareSyncEvenNotify(int, struct VOS_IPC_CTRL_s *, char *,
                                       char *, int, int, int);
static int vosIpcShareSyncEvenWait(int, struct VOS_IPC_CTRL_s *, char *);
static int vosIpcShareSyncEvenReply(int, struct VOS_IPC_CTRL_s *, char *,
                                      char *);
static int vosIpcShareAsyncEvenNotify(int, struct VOS_IPC_CTRL_s *, char *,
                                        char *);
static int vosIpcShareAsyncEvenWait(int, struct VOS_IPC_CTRL_s *, char *,
                                      char *);
static int vosIpcShareAsyncEvenPoll(int, struct VOS_IPC_CTRL_s *, char *,
                                      char *);
static int vosIpcShareMemThreadWait(int nDstIpcId, struct VOS_IPC_CTRL_s *pstIpcCtrl, 
									char *pcIpcId);
/* static void vosIpcPrintDestIpcId(int); */

/***** DATA ALLOCATION *****/
static int g_nIpcIdShaMemId;        // VOS IPC ID Share Memory ID
static char *g_pcIpcIdShaMemData;   // VOS IPC ID share memory data pointer
static int g_nIpcIdSemaId;          // VOS IPC ID Semaphore ID
static int g_nShaMemId;             // Global Share Memory ID
static int g_nSemaId;               // Global Semaphore ID
static char *g_pcShaMemData;          // Global Share memory data pointer

static struct sembuf g_stAcquire = { 0, -1, 0 };
static struct sembuf g_stRelease = { 0, 1, 0 };
static unsigned short g_ausIpcIdSemaVal[VOS_IPC_ID_TOTAL_SEMA] = { VOS_IPC_SEMA_AVAIL };
static unsigned short g_ausIpcSemaVal[VOS_IPC_TOTAL_SEMA * VOS_IPC_ID_TOTAL];
static unsigned long g_ulSeqNo = 0;
static int g_nVosIpcInitFlag = 0;

static char g_acIpcPath[VOS_IPC_PATH_LEN];


static unsigned int sysIPAddrGet(char *ifName)
{
    struct ifreq stIfreq;
    int nSock;
    struct sockaddr_in *psLocalAddr;

    if (NULL == ifName)
    {
        return 0;
    }

    if ((nSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return 0;
    }
    strcpy(stIfreq.ifr_name, (char *)ifName);

    if (ioctl(nSock, SIOCGIFADDR, &stIfreq) < 0)
    {
        close(nSock);
        return 0;
    }
    psLocalAddr = (struct sockaddr_in *)&stIfreq.ifr_addr;
    close(nSock);
    return psLocalAddr->sin_addr.s_addr;
}




/***** PUBLIC FUNCTIONS ****/

/*******************************************************************************
*
* NAME: vosIpcRegister
*
* DESCRIPTION: This function was called to allocate share memory and semaphore 
*              resources.
*
* RETURN: registration success or failure.
*
* INPUTS: 
*         int nIpcId: module identifier 
*
* OUTPUTS: None
*
* SIDE EFFECTS: First function call before module can use this library.
*
*******************************************************************************/
int vosIpcRegister(int nIpcId)
{
    DIR *pDir;
    int i, nRetVal;
    key_t key1, key2;
    char ucSemaFlag;
	char ipcIdStr[10] = {0};
	
    struct VOS_IPC_CTRL_MEM_s *pstIpcCtrlMem;
	struct VOS_IPC_CTRL_s *pstInitIpcCtrl;
	struct VOS_IPC_MEM_s *pstIpcMem;

	if ( nIpcId >= VOS_IPC_ID_TOTAL ){
		VOS_IPC_DEBUG("ipc id error: %d\n",nIpcId);
		return VOS_IPC_UNKNOWN_TYPE;
	}

	
	if ( nIpcId == VOS_IPC_ID_EXTERNAL_10 ){
		//vosIpcDestroy();
	}
	

    /* get tmp directory */
    pDir = opendir(DEFAULT_TEMP_PATH);
    if (pDir == NULL)
    {
        mkdir(DEFAULT_TEMP_PATH, 0777);
    }
    else
    {
        closedir(pDir);
    }

    /* make tmp/ip dir */
    sprintf(g_acIpcPath, DEFAULT_TEMP_IPC_PATH, 0xFFFFFFFF);
    pDir = opendir(g_acIpcPath);
    if (pDir == NULL)
    {
        mkdir(g_acIpcPath, 0777);
    }
    else
    {
        closedir(pDir);
    }

	/* generate keys for ipc_id and ipc_data */
    key1 = ftok(g_acIpcPath, SEED_ID_SHARE);
    key2 = ftok(g_acIpcPath, SEED_SHARE);
	VOS_IPC_DEBUG("-->register begin:\n");

	VOS_IPC_DEBUG("\n get shm and sema id\n");
    if (!g_nVosIpcInitFlag) {
		/* generate Shared Memory ID for ipc_id and ipc_data, and attache each of them */
        if ((g_nIpcIdShaMemId =
            shmget(key1, sizeof(VOS_IPC_CTRL_MEM_t),
                    IPC_CREAT | 0666)) < 0) {
            VOS_IPC_DEBUG("vosIpcRegister: Create task ID table share memory error\n");
            return (VOS_IPC_RESOURCE_ERR);
        }
        if ((g_pcIpcIdShaMemData = (char *) shmat(g_nIpcIdShaMemId, 0, 0)) == (char *) -1) {
            VOS_IPC_DEBUG("vosIpcRegister: share memory task ID table attach error, ipc_id:%d\n",
                    nIpcId);
            return (VOS_IPC_RESOURCE_ERR);
        }
        if ((g_nShaMemId =
             shmget(key2, sizeof(struct VOS_IPC_MEM_s) * VOS_IPC_ID_TOTAL,
                    IPC_CREAT | 0666)) < 0) {
            VOS_IPC_DEBUG("vosIpcRegister: Create data share memory error\n");
            return (VOS_IPC_RESOURCE_ERR);
        }
        if ((g_pcShaMemData = (char *) shmat(g_nShaMemId, 0, 0)) == (char *) -1) {
            VOS_IPC_DEBUG("vosIpcRegister: ID table register succeed key:%x g_nIpcIdShaMemId:%d, addr:%x, ipc_id:%d\n", 
                key1, g_nIpcIdShaMemId, (unsigned int) g_pcIpcIdShaMemData, nIpcId);
            VOS_IPC_DEBUG("vosIpcRegister: data attach error key2:%x g_nShaMemId:%d, err:%d ipc_id:%d\n",
                key2, g_nShaMemId, errno, nIpcId);
            return (VOS_IPC_RESOURCE_ERR);
        }
		
		/* generate OR get a sema id for ipc_id of shared mem */
		ucSemaFlag = SEED_ID_SEMA;
		if ((g_nIpcIdSemaId = 
			semget(ftok(g_acIpcPath, ucSemaFlag), 1, IPC_CREAT | IPC_EXCL | 0666)) < 0) {
			
				if (errno == EEXIST) {
					g_nIpcIdSemaId = semget(ftok(g_acIpcPath, ucSemaFlag), 1, 0);
					if (g_nIpcIdSemaId < 0)
						return (VOS_IPC_RESOURCE_ERR);
					VOS_IPC_DEBUG("vosIpcRegister: ID table register succeed key:%x g_nIpcIdShaMemId:%d, g_nIpcIdSemaId:%d addr:%x\n", 
						key1, g_nIpcIdShaMemId, g_nIpcIdSemaId, (unsigned int) g_pcIpcIdShaMemData);
				} else {
					return (VOS_IPC_RESOURCE_ERR);
				}
		}
		
		/* generate OR get a sema id for ipc_data of shared mem */
		ucSemaFlag = SEED_SEMA;
		if ((g_nSemaId =
			 semget(ftok(g_acIpcPath, ucSemaFlag), VOS_IPC_TOTAL_SEMA * VOS_IPC_ID_TOTAL,
						IPC_CREAT | IPC_EXCL | 0666)) < 0) {
						
				if (errno == EEXIST) {
					g_nSemaId = semget(ftok(g_acIpcPath, ucSemaFlag), 1, 0);
					
					if (g_nSemaId < 0){
						return (VOS_IPC_RESOURCE_ERR);
					}
				}
				VOS_IPC_DEBUG("vosIpcRegister: Data register succeed key:%x g_nShaMemId:%d, g_nSemaId:%d addr:%x\n", 
						key2, g_nShaMemId, g_nSemaId, (unsigned int) g_pcShaMemData);
		}else{
			VOS_IPC_DEBUG("init clear ipc id table\n");
			// init clear the ipc id table
			nRetVal = vosIpcInitResources(key1, key2, nIpcId);
			if (nRetVal != VOS_IPC_SUCCESS)
				return (nRetVal);

		}

		g_nVosIpcInitFlag = 1;
    }else{
		VOS_IPC_DEBUG("g_nVosIpcInitFlag is 1\n");	
    }



	/* acquire the above sema id */
	g_stAcquire.sem_num = VOS_IPC_ID_SEMA;
	semop(g_nIpcIdSemaId, &g_stAcquire, 1);
	pstIpcCtrlMem = (struct VOS_IPC_CTRL_MEM_s *) g_pcIpcIdShaMemData;

	//check if ipc opconn is running
	VOS_IPC_DEBUG("chek if init ipc is registered\n");
	if ( nIpcId != VOS_IPC_ID_EXTERNAL_10 ){
		for (i = 0; i < VOS_IPC_ID_MAX_DO_NOT_USE; i++) {
			if ( (pstIpcCtrlMem->astIpcCtrl[i].nType != VOS_IPC_TYPE_UNUSED &&
				pstIpcCtrlMem->astIpcCtrl[i].nIpcId == VOS_IPC_ID_EXTERNAL_10) ) {
					break;
				}
		}
		if ( i >= VOS_IPC_ID_MAX_DO_NOT_USE ){// VOS_IPC_ID_EXTERNAL_10 not register, exit
			
			VOS_IPC_DEBUG("ipc for opconn unregistered: exit\n");
			vosIpcDebugIdTable();
			
			g_stRelease.sem_num = VOS_IPC_ID_SEMA;
			semop(g_nIpcIdSemaId, &g_stRelease, 1);	
			VOS_IPC_DEBUG("<--register end:\n");
			return (VOS_IPC_SUCCESS);
		}
	}

	VOS_IPC_DEBUG("check new ipc id in ipc table\n");
	/* if this ipc_id is alreadly registered, release the sema id and exit */
	for (i = 0; i < VOS_IPC_ID_MAX_DO_NOT_USE; i++) {
		if (pstIpcCtrlMem->astIpcCtrl[i].nType != VOS_IPC_TYPE_UNUSED &&
			pstIpcCtrlMem->astIpcCtrl[i].nIpcId== nIpcId) {
			g_stRelease.sem_num = VOS_IPC_ID_SEMA;
			semop(g_nIpcIdSemaId, &g_stRelease, 1);
			
			VOS_IPC_DEBUG("alreadly registered: ipc id %d\n",nIpcId);
			VOS_IPC_DEBUG("<--register end:\n");
			
			return (VOS_IPC_SUCCESS);
		}
	}


	VOS_IPC_DEBUG("insert to ipc table\n");
	// else use an empty slot to register, fill in values and release the sema id then exit
	for (i = 0; i < VOS_IPC_ID_MAX_DO_NOT_USE; i++) {
		
		if (pstIpcCtrlMem->astIpcCtrl[i].nType == VOS_IPC_TYPE_UNUSED) {
			
			vosIpcIdNewEntry(nIpcId, i);
			
			g_stRelease.sem_num = VOS_IPC_ID_SEMA;
			semop(g_nIpcIdSemaId, &g_stRelease, 1);
			
			vosIpcDebugIdTable();

			//when register finished, generate a thread to wait for this ipc id
			if ( nIpcId != VOS_IPC_ID_EXTERNAL_10 ){
				
				pstIpcMem = ((struct VOS_IPC_MEM_s *) g_pcShaMemData + VOS_IPC_ID_EXTERNAL_10);
		    	if ((pstInitIpcCtrl = vosIpcGetEntryByIpcId(VOS_IPC_ID_EXTERNAL_10)) != NULL) {
	
					g_stAcquire.sem_num = pstInitIpcCtrl->nSemaBase + VOS_IPC_SEM3;
			    	semop(g_nSemaId, &g_stAcquire, 1);
					
					VOS_IPC_DEBUG("newly register ipc id: %d ()\n",nIpcId);
					sprintf(ipcIdStr,"%d",nIpcId);
					memcpy(pstIpcMem, ipcIdStr, sizeof(ipcIdStr));	
					
					//g_stRelease.sem_num = pstInitIpcCtrl->nSemaBase + VOS_IPC_SEM3; 
					//semop(g_nSemaId, &g_stRelease, 1);	

				    g_stRelease.sem_num = pstInitIpcCtrl->nSemaBase + VOS_IPC_SEM2;
				    semop(g_nSemaId, &g_stRelease, 1);	
		    	}
				VOS_IPC_DEBUG("<--register end\n");
			}else{
				VOS_IPC_DEBUG("<--register end\n");
			}
			
			return (VOS_IPC_SUCCESS);
		}
	}


}

/*******************************************************************************
*
* NAME: vosIpcUnregister
*
* DESCRIPTION: This function was used by module to g_stRelease resources.
*
* RETURN: None
*
* INPUTS:
*         int ipc_id: module identifier 
*
* OUTPUTS: None
*
* SIDE EFFECTS:
*
*******************************************************************************/
void vosIpcUnregister(int nIpcId)
{
    int i;
    struct VOS_IPC_CTRL_MEM_s *pstIpcCtrlMem;
	struct VOS_IPC_MEM_s *pstIpcMem;
	struct VOS_IPC_CTRL_s *pstInitIpcCtrl;
	char ipcIdStr[20] = {0};
	
    g_stAcquire.sem_num = VOS_IPC_ID_SEMA;
    semop(g_nIpcIdSemaId, &g_stAcquire, 1);
    pstIpcCtrlMem = (struct VOS_IPC_CTRL_MEM_s *) g_pcIpcIdShaMemData;
    for (i = 0; i < pstIpcCtrlMem->nCount; i++) {
        if (pstIpcCtrlMem->astIpcCtrl[i].nIpcId == nIpcId) {
            pstIpcCtrlMem->astIpcCtrl[i].nType = VOS_IPC_TYPE_UNUSED; // Set unused flag
            break;
        }
    }
    g_stRelease.sem_num = VOS_IPC_ID_SEMA;
    semop(g_nIpcIdSemaId, &g_stRelease, 1);


	if ( nIpcId != VOS_IPC_ID_EXTERNAL_10 ){
		
		pstIpcMem = ((struct VOS_IPC_MEM_s *) g_pcShaMemData + VOS_IPC_ID_EXTERNAL_10);
    	if ((pstInitIpcCtrl = vosIpcGetEntryByIpcId(VOS_IPC_ID_EXTERNAL_10)) != NULL) {

			g_stAcquire.sem_num = pstInitIpcCtrl->nSemaBase + VOS_IPC_SEM3;
	    	semop(g_nSemaId, &g_stAcquire, 1);
			
			printf("newly unregister ipc id: %d ()\n",nIpcId);
			nIpcId += 100;
			sprintf(ipcIdStr,"%d",nIpcId);
			memcpy(pstIpcMem, ipcIdStr, sizeof(ipcIdStr));	
			
		    g_stRelease.sem_num = pstInitIpcCtrl->nSemaBase + VOS_IPC_SEM2;
		    semop(g_nSemaId, &g_stRelease, 1);	
    	}
	}	
}

/*******************************************************************************
*
* NAME: vosIpcDestroy
*
* DESCRIPTION: This function was called to g_stRelease share memory and 
*              semaphore resources.
*
* RETURN: None
*
* INPUTS: None
*
* OUTPUTS: None
*
* SIDE EFFECTS: No module can use this library after resources g_stRelease.
*
*******************************************************************************/
void vosIpcDestroy()
{
    shmdt(g_pcShaMemData);
    shmctl(g_nShaMemId, IPC_RMID, (struct shmid_ds *) 0);
    semctl(g_nSemaId, 0, IPC_RMID, 0);
    shmdt(g_pcIpcIdShaMemData);
    shmctl(g_nIpcIdShaMemId, IPC_RMID, (struct shmid_ds *) 0);
    semctl(g_nIpcIdSemaId, 0, IPC_RMID, 0);
}

/*******************************************************************************
*
* NAME: vosIpcSyncEventNotify
*
* DESCRIPTION: The calling process use this function to send and receive the
*              result data. If the function returns VOS_IPC_SUCCESS, the result
*              data will be in buf(e.g. show command).
*
* RETURN: sync event success or failure.
*
* INPUTS: 
*         int nDstIpcId: dest task identifier 
*         char *pBuf: input data packet 
*         int nSize: input data packet size 
*         int nMillisecond: timeout value (unit: nMillisecond) 
*         int nFlag: VOS_IPC_SEM1_SET/VOS_IPC_SEM1_CLEAR
*
* OUTPUTS:
*         char *buf: output data packet 
*
* SIDE EFFECTS:
*
*******************************************************************************/

int vosIpcSyncEventNotify(int nDstIpcId, char *pBuf, int nSize,
                          int nMillisecond, int nFlag)
{
    struct VOS_IPC_CTRL_s *pstIpcCtrl;

	if ( pBuf == NULL || nDstIpcId >= VOS_IPC_ID_MAX_DO_NOT_USE || nSize > VOS_IPC_DATA_SIZE ){
		return VOS_IPC_GENERAL_ERR;
	}
#ifdef VOS_IPC_SHM_TEST
	int res=0;
	VOS_TLV_MSG_t testData;

	testData.major_version = 'a';
	testData.minor_version = 'b';
	testData.release_version = 'c';
	testData.update_version = 'd';
	testData.seq_no = 1;
	testData.rtn_seq_no = 2;
	testData.rtn_code = 0;
	testData.event_id = 3;
	testData.attribute_len = 1024;

	strcpy(testData.data,"tlvdata--->type:3,len:1024,value:1111");

	VOS_IPC_DEBUG("testData event id: %d\n",testData.event_id);
	VOS_IPC_DEBUG("testData in original tlv only: %s\n",testData.data);
	if ((pstIpcCtrl = vosIpcGetEntryByIpcId(nDstIpcId)) != NULL) {

		switch (pstIpcCtrl->nType) {
		case VOS_IPC_TYPE_SHARE:
			res = (vosIpcShareSyncEvenNotify(nDstIpcId, pstIpcCtrl,
				(char *)((struct VOS_IPC_MEM_s *) g_pcShaMemData + nDstIpcId), (char *)&testData, sizeof(testData),
				nMillisecond, nFlag));
			
				//VOS_IPC_DEBUG("--notify--reply data back to cgi: \n %s\n\n",testData.data);


			return res;
		case VOS_IPC_TYPE_SOCKET:
			break;
		}
		
	}
	VOS_IPC_DEBUG("vosIpcSyncEventNotify: Unknown type or unregister error\n");
	return (VOS_IPC_UNKNOWN_TYPE);
#else
	VOS_TLV_MSG_t *ptlv_msg = NULL;
	int res = 0;
    if ((pstIpcCtrl = vosIpcGetEntryByIpcId(nDstIpcId)) != NULL) {

        switch (pstIpcCtrl->nType) {
        case VOS_IPC_TYPE_SHARE:
            res = (vosIpcShareSyncEvenNotify(nDstIpcId, pstIpcCtrl,
                (char *)((struct VOS_IPC_MEM_s *) g_pcShaMemData + nDstIpcId), pBuf, nSize,
                nMillisecond, nFlag));
			ptlv_msg = (VOS_TLV_MSG_t *)pBuf;
			VOS_IPC_DEBUG("--notify--reply data back to cgi: data4-7 %d,%d,%d,%d\n\n",ptlv_msg->data[4],ptlv_msg->data[5],ptlv_msg->data[6],ptlv_msg->data[7]);
			return res;
        case VOS_IPC_TYPE_SOCKET:
            break;
        }
		
    }
    VOS_IPC_DEBUG("vosIpcSyncEventNotify: Unknown type or unregister error\n");
    return (VOS_IPC_UNKNOWN_TYPE);


#endif



}

/*******************************************************************************
*
* NAME: vosIpcSyncEventWait
*
* DESCRIPTION: The called process use this function to wait incoming event.
*
* RETURN: success
*
* INPUTS: 
*         int nDstIpcId: called process identifier 
*
* OUTPUTS:
*         char *pBuf: whole data packet 
*
* SIDE EFFECTS: No return until event arrived.
*
*******************************************************************************/
int vosIpcSyncEventWait(int nDstIpcId)
{
    struct VOS_IPC_CTRL_s *pstIpcCtrl;
	VOS_IPC_DEBUG("sync wait with ipc id: %d\n",nDstIpcId );

    if ((pstIpcCtrl = vosIpcGetEntryByIpcId(nDstIpcId)) != NULL) {
        switch (pstIpcCtrl->nType) {
        case VOS_IPC_TYPE_SHARE:
			if ( nDstIpcId == VOS_IPC_ID_EXTERNAL_10 ){//for new ipc id register
				return vosIpcShareMemThreadWait(VOS_IPC_ID_EXTERNAL_10, pstIpcCtrl, 
					(char *)((struct VOS_IPC_MEM_s *)g_pcShaMemData + nDstIpcId));
			}else{//for data notify from web
	            return (vosIpcShareSyncEvenWait(nDstIpcId, pstIpcCtrl,(char *)((struct VOS_IPC_MEM_s *)g_pcShaMemData + nDstIpcId)));
			}
        case VOS_IPC_TYPE_SOCKET:
            break;
        }
    }
    VOS_IPC_DEBUG("vosIpcSyncEventWait: Unknown type or unregister error\n");
    return (VOS_IPC_UNKNOWN_TYPE);
}

/*******************************************************************************
*
* NAME: vosIpcSyncEventReply
*
* DESCRIPTION: The called process use this function to return the result data
*              back to the calling process. 
*
* RETURN: return data to the caller process success or failure.
*
* INPUTS: 
*         int nDstIpcId: called process identifier 
*         char *pBuf: whole data packet 
*
* OUTPUTS:
*
* SIDE EFFECTS:
*
*******************************************************************************/
int vosIpcSyncEventReply(int nDstIpcId, char *pBuf)
{
    struct VOS_IPC_CTRL_s *pstIpcCtrl;

    if ((pstIpcCtrl = vosIpcGetEntryByIpcId(nDstIpcId)) != NULL) {
        switch (pstIpcCtrl->nType) {
        case VOS_IPC_TYPE_SHARE:
            return (vosIpcShareSyncEvenReply(nDstIpcId, pstIpcCtrl,
                (char *)((struct VOS_IPC_MEM_s *)g_pcShaMemData + nDstIpcId), pBuf));
        case VOS_IPC_TYPE_SOCKET:
            break;
        }
    }
    VOS_IPC_DEBUG("vosIpcSyncEventReply: Unknown type or unregister error\n");
    return (VOS_IPC_UNKNOWN_TYPE);
}


/*******************************************************************************
*
* NAME: vosIpcAsyncEventNotify
*
* DESCRIPTION: The calling process use this function to send data to the called
*              process.
*
* RETURN: Put data to the queue success or failure.
*
* INPUTS: 
*         int nDstIpcId: dest task identifier 
*         char *pBuf: whole data packet 
*
* OUTPUTS: None
*
* SIDE EFFECTS:
*
*******************************************************************************/
int vosIpcAsyncEventNotify(int nDstIpcId, char *pBuf)
{
    struct VOS_IPC_CTRL_s *pstIpcCtrl;

    if ((pstIpcCtrl = vosIpcGetEntryByIpcId(nDstIpcId)) != NULL) {
        switch (pstIpcCtrl->nType) {
        case VOS_IPC_TYPE_SHARE:
            return (vosIpcShareAsyncEvenNotify(nDstIpcId, pstIpcCtrl,
                (char *)((struct VOS_IPC_MEM_s *)g_pcShaMemData + nDstIpcId), pBuf));
        case VOS_IPC_TYPE_SOCKET:
            break;
        }
    }
    VOS_IPC_DEBUG("vosIpcAsyncEventNotify: Unknown type or unregister error\n");
    return (VOS_IPC_UNKNOWN_TYPE);
}

/*******************************************************************************
*
* NAME: vosIpcAsyncEventWait
*
* DESCRIPTION: The called process use this function to retrieve data from the
*              queue.
*
* RETURN: success for data available.
*
* INPUTS: 
*         int nDstIpcId: called process identifier 
*
* OUTPUTS:
*         char *pBuf: whole data packet 
*
* SIDE EFFECTS: No return until event arrived.
*
*******************************************************************************/
int vosIpcAsyncEventWait(int nDstIpcId, char *pBuf)
{
    struct VOS_IPC_CTRL_s *pstIpcCtrl;

    if ((pstIpcCtrl = vosIpcGetEntryByIpcId(nDstIpcId)) != NULL) {
        switch (pstIpcCtrl->nType) {
        case VOS_IPC_TYPE_SHARE:
            return (vosIpcShareAsyncEvenWait(nDstIpcId, pstIpcCtrl,
                (char *)((struct VOS_IPC_MEM_s *)g_pcShaMemData + nDstIpcId), pBuf));
        case VOS_IPC_TYPE_SOCKET:
            break;
        }
    }
    VOS_IPC_DEBUG("vosIpcAsyncEventWait: Unknown type or unregister error\n");
    return (VOS_IPC_UNKNOWN_TYPE);
}

/*******************************************************************************
*
* NAME: vosIpcAsyncEventPoll
*
* DESCRIPTION: The called process use this function to retrieve data from the
*              queue.
*
* RETURN: success for data available or failure.
*
* INPUTS: 
*         int nDstIpcId: called process identifier 
*
* OUTPUTS:
*         char *pBuf: whole data packet 
*
* SIDE EFFECTS:
*
*******************************************************************************/
int vosIpcAsyncEventPoll(int nDstIpcId, char *pBuf)
{
    struct VOS_IPC_CTRL_s *pstIpcCtrl;

    if ((pstIpcCtrl = vosIpcGetEntryByIpcId(nDstIpcId)) != NULL) {
        switch (pstIpcCtrl->nType) {
        case VOS_IPC_TYPE_SHARE:
            return (vosIpcShareAsyncEvenPoll(nDstIpcId, pstIpcCtrl,
                (char *)((struct VOS_IPC_MEM_s *)g_pcShaMemData + nDstIpcId), pBuf));
        case VOS_IPC_TYPE_SOCKET:
            break;
        }
    }
    VOS_IPC_DEBUG("vosIpcAsyncEventPoll: Unknown type or unregister error\n");
    return (VOS_IPC_UNKNOWN_TYPE);
}

/*******************************************************************************
*
* NAME: vosIpcSleep
*
* DESCRIPTION: Sleep less than 1 second.
*
* RETURN: None
*
* INPUTS: 
*         int milliseconds: range 1-999 
*
* OUTPUTS: None
*
* SIDE EFFECTS: If input variable not in the range, the result is unpredictable.
*               Linux 2.4 define HZ to 100 (10 nMillisecond), if you use less 
*               than 10 nMillisecond, you still need to wait 10 nMillisecond.
*               Becasuse only hardware interrupt is reliable wakeup mechanism.
*               This HZ change to 1000 in Linux 2.6.
*
*******************************************************************************/
void vosIpcSleep(int milliseconds)
{
    struct timespec timeout;

    timeout.tv_sec = 0;
    timeout.tv_nsec = milliseconds * 1000000;
    nanosleep(&timeout, NULL);
}

/*******************************************************************************
*
* NAME: vosIpcDebugIdTable
*
* DESCRIPTION: Dump ENO ID table
*
* RETURN: N/a
*
* INPUTS: N/a
*
* OUTPUTS: N/a
*
* SIDE EFFECTS:
*
*******************************************************************************/
void vosIpcDebugIdTable()
{
    int i;
    struct VOS_IPC_CTRL_MEM_s *pstIpcCtrlMem;

    VOS_IPC_DEBUG("\nDump VOS IPC ID table\nIndex ipcId type semaBase\n");

    pstIpcCtrlMem = (struct VOS_IPC_CTRL_MEM_s *) g_pcIpcIdShaMemData;
    for (i = 0; i < pstIpcCtrlMem->nCount; i++) {
        VOS_IPC_DEBUG("%d:\t%d\t%d\t%d\n", i,
            pstIpcCtrlMem->astIpcCtrl[i].nIpcId, pstIpcCtrlMem->astIpcCtrl[i].nType,
            pstIpcCtrlMem->astIpcCtrl[i].nSemaBase);
    }
    VOS_IPC_DEBUG("\n");
}

/*******************************************************************************
*
* NAME: vosIpcDebugTrace
*
* DESCRIPTION: Dump ENO data (Big Endian format)
*
* RETURN: None
*
* INPUTS: 
*         char *pcDataBuf: input data packet 
*
* OUTPUTS: None
*
* SIDE EFFECTS: 
*
*******************************************************************************/
void vosIpcDebugTrace(char *pcDataBuf)
{
    int i, size;
    int id, len;
    unsigned int attrSize;
    struct VOS_TLV_MSG_s tlvMsg;
	return;
    memcpy((char *) &tlvMsg, pcDataBuf, sizeof(VOS_TLV_MSG_t));
    attrSize = ntohl(tlvMsg.attribute_len);

    VOS_IPC_DEBUG("Major version:%d\n", tlvMsg.major_version);
    VOS_IPC_DEBUG("Minor version:%d\n", tlvMsg.minor_version);
    VOS_IPC_DEBUG("Release version:%d\n",
                tlvMsg.release_version);
    VOS_IPC_DEBUG("Update version:%d\n",
                tlvMsg.update_version);
    VOS_IPC_DEBUG("Seq No:%d\n", ntohl(tlvMsg.seq_no));
    VOS_IPC_DEBUG("Return seq No:%d\n", ntohl(tlvMsg.rtn_seq_no));
    VOS_IPC_DEBUG("Return code:%d\n", ntohl(tlvMsg.rtn_code));
    VOS_IPC_DEBUG("Event ID:0x%x\n", ntohl(tlvMsg.event_id));
    VOS_IPC_DEBUG("Attribute Len:%d\n", attrSize);

    size = 0;
    if (attrSize != VOS_TLV_EMPTY_ENTRY_SIZE) {
        VOS_IPC_DEBUG(" ID\tLEN\tValue\n");
    }
    while (1) {
        id = vosTlvShortGet(&tlvMsg.data[size], (int *) &size);
        if (id == 0)
            break;              // Last TLV entry
        VOS_IPC_DEBUG(" %x ", id);

        len = vosTlvShortGet(&tlvMsg.data[size], (int *) &size);
        VOS_IPC_DEBUG("    %d\t", len);

        for (i = 0; i < len; i++) {
            VOS_IPC_DEBUG("%02X ",
                (unsigned char)tlvMsg.data[size + i]);

            if ((i % 16) == 15) {
                VOS_IPC_DEBUG("\n\t\t");
            }
        }
        VOS_IPC_DEBUG("\n");
        size += len;
    }
    VOS_IPC_DEBUG("\n");
}

/*******************************************************************************
*
* NAME: vosIpcDebugTLVDataTrace
*
* DESCRIPTION: Dump TLV message attribute data (Big Endian format)
*
* RETURN: None
*
* INPUTS: 
*         char *pBuf: TLV attribute data address
*
* OUTPUTS: None
*
* SIDE EFFECTS:
*
*******************************************************************************/
void vosIpcDebugTLVDataTrace(char *pBuf)
{
    int i, size;
    int id, len;

    size = 0;

    VOS_IPC_DEBUG("Dump VOS_TLV_attribute_data:\n");
    VOS_IPC_DEBUG(" ID\tLEN\tValue\n");

    while (1) {
        id = vosTlvShortGet(&pBuf[size], (int *) &size);
        if (id == 0)
            break;              // Last TLV entry
        VOS_IPC_DEBUG(" %x ", id);

        len = vosTlvShortGet(&pBuf[size], (int *) &size);
        VOS_IPC_DEBUG("    %d\t", len);

        for (i = 0; i < len; i++) {
            VOS_IPC_DEBUG("%02X ", (unsigned char) pBuf[size + i]);
            if ((i % 16) == 15) {
                VOS_IPC_DEBUG("\n\t\t");
            }
        }
        VOS_IPC_DEBUG("\n");
        size += len;
    }
    VOS_IPC_DEBUG("\n");
}

/***** PRIVATE FUNCTIONS *****/

/*******************************************************************************
*
* NAME: vosIpcInitResources
*
* DESCRIPTION: This function was called to handle semaphore and ID table 
*              initialization.
*
* RETURN: Initialization success or failure.
*
* INPUTS:
*         int key1: Task ID table key 
*         int key2: Data area key 
*         int nIpcId: module identifier 
*
* OUTPUTS: None
*
* SIDE EFFECTS:
*
*******************************************************************************/
static int vosIpcInitResources(key_t key1, key_t key2, int nIpcId)
{
    int i;
    union SEM_u semaArg;
	
//set sema id for ipc_id
    semaArg.array = g_ausIpcIdSemaVal;
    if ((semctl(g_nIpcIdSemaId, 0, SETALL, semaArg)) < 0) {
		
        semctl(g_nIpcIdSemaId, 0, IPC_RMID, semaArg);
        return (VOS_IPC_RESOURCE_ERR);
    }

    VOS_IPC_DEBUG("ID table register succeed key:%x g_nIpcIdShaMemId:%d, g_nIpcIdSemaId:%d addr:%x\n", 
        key1, g_nIpcIdShaMemId, g_nIpcIdSemaId, (unsigned int) g_pcIpcIdShaMemData);


//set sema id for ipc_data

    semaArg.array = g_ausIpcSemaVal;
    for (i = 0; i < (VOS_IPC_TOTAL_SEMA * VOS_IPC_ID_TOTAL);) {
        g_ausIpcSemaVal[i] = VOS_IPC_SEMA_LOCK;//VOS_IPC_SEMA_AVAIL;           // VOS_IPC_SEM1
        g_ausIpcSemaVal[i + 1] = VOS_IPC_SEMA_LOCK;        // VOS_IPC_SEM2 (lock)
        g_ausIpcSemaVal[i + 2] = VOS_IPC_SEMA_AVAIL;       // VOS_IPC_SEM3
        g_ausIpcSemaVal[i + 3] = VOS_IPC_SEMA_AVAIL;		// VOS_IPC_SEM3
        g_ausIpcSemaVal[i + 4] = VOS_IPC_ASYNC_SLOT;       // VOS_IPC_ASEM1
        g_ausIpcSemaVal[i + 5] = VOS_IPC_SEMA_AVAIL;       // VOS_IPC_ASEM2
        i += 6;
    }

    if ((semctl(g_nSemaId, 0, SETALL, semaArg)) < 0) {
		
        semctl(g_nSemaId, 0, IPC_RMID, semaArg);
        return (VOS_IPC_RESOURCE_ERR);
    }

    VOS_IPC_DEBUG("Data register succeed key:%x g_nShaMemId:%d, g_nSemaId:%d addr:%x\n", 
        key2, g_nShaMemId, g_nSemaId, (unsigned int) g_pcShaMemData);

    g_stAcquire.sem_num = VOS_IPC_ID_SEMA;
    semop(g_nIpcIdSemaId, &g_stAcquire, 1);
    memset(g_pcIpcIdShaMemData, 0, sizeof(VOS_IPC_CTRL_MEM_t)); // Clear all the ID table area
    g_stRelease.sem_num = VOS_IPC_ID_SEMA;
    semop(g_nIpcIdSemaId, &g_stRelease, 1);

    return (VOS_IPC_SUCCESS);
}

/*******************************************************************************
*
* NAME: vosIpcIdNewEntry
*
* DESCRIPTION: This function was called to add a new entry to task ID table 
*
* RETURN: None
*
* INPUTS:
*         int nIpcId: module identifier 
*         int nIndex: IPC ID table entry index 
*
* OUTPUTS: None
*
* SIDE EFFECTS: No protection in this routine, caller should put protection
*               before call this function.
*
*******************************************************************************/
static void vosIpcIdNewEntry(int nIpcId, int nIndex)
{
    struct VOS_IPC_CTRL_MEM_s *pstIpcCtrlMem;

    pstIpcCtrlMem = (struct VOS_IPC_CTRL_MEM_s *) g_pcIpcIdShaMemData;
    pstIpcCtrlMem->astIpcCtrl[nIndex].nIpcId = nIpcId;
    pstIpcCtrlMem->astIpcCtrl[nIndex].nType = VOS_IPC_TYPE_SHARE;
    pstIpcCtrlMem->astIpcCtrl[nIndex].nSemaBase = nIndex * VOS_IPC_TOTAL_SEMA;

    VOS_IPC_DEBUG("vosIpcIdNewEntry: ipcId:%d index:%d\n", nIpcId, nIndex);
    pstIpcCtrlMem->nCount++;
}

/*******************************************************************************
*
* NAME: vosIpcGetEntryByIpcId
*
* DESCRIPTION: This function was called to get the correct entry from 
*              task ID table 
*
* RETURN:
*         VOS_IPC_CTRL_s *: task ID table entry address 
*
* INPUTS:
*         int nIpcId: module identifier
*
* OUTPUTS: None
*
* SIDE EFFECTS:
*
*******************************************************************************/
static struct VOS_IPC_CTRL_s *vosIpcGetEntryByIpcId(int nIpcId)
{
    int i;
    struct VOS_IPC_CTRL_MEM_s *pstIpcCtrlMem;

    pstIpcCtrlMem = (struct VOS_IPC_CTRL_MEM_s *) g_pcIpcIdShaMemData;
    for (i = 0; i < pstIpcCtrlMem->nCount; i++) {
        if (pstIpcCtrlMem->astIpcCtrl[i].nIpcId == nIpcId
            && pstIpcCtrlMem->astIpcCtrl[i].nType != VOS_IPC_TYPE_UNUSED) {
            return ((struct VOS_IPC_CTRL_s *) &pstIpcCtrlMem->astIpcCtrl[i]);
        }
    }
    return ((struct VOS_IPC_CTRL_s *) NULL);
}

/*******************************************************************************
*
* NAME: vosIpcNewCliIpcId
*
* DESCRIPTION: Because multiple CLI/cshell instances are allowed (max 5).
*              CLI call this function to get a unused CLI task ID to start schell
*
* RETURN:
*         IPC ID or -1
*
* INPUTS: None
*
* OUTPUTS: None
*
* SIDE EFFECTS:
*
*******************************************************************************/
int vosIpcNewCliIpcId(void)
{
    int i, j, k;
    struct VOS_IPC_CTRL_MEM_s *pstIpcCtrlMem;

    pstIpcCtrlMem = (struct VOS_IPC_CTRL_MEM_s *) g_pcIpcIdShaMemData;

    for (j = VOS_IPC_ID_CLI_1; j <= VOS_IPC_ID_CLI_6; j++) {
        k = 0; /* cli ipc id is not in use */
        for (i = 0; i < pstIpcCtrlMem->nCount; i++) {
            if (pstIpcCtrlMem->astIpcCtrl[i].nType != VOS_IPC_TYPE_UNUSED &&
                pstIpcCtrlMem->astIpcCtrl[i].nIpcId == j) {
                k = 1; /* cli ipc id is in use */
                break;
            }
        }
        if (k == 0) {
            VOS_IPC_DEBUG("vosIpcNewCliIpcId: found cli_ipc_id = %d\n", j); 
            return j;
        }
    }

    return (-1);
}

/*******************************************************************************
*
* NAME: vosIpcCheckTaskRunning
*
* DESCRIPTION: Because multiple CLI/cshell instances are allowed (max 5).
*              HAL calls this function to find out if a CLI task is running
*
* RETURN:
*         0 - nIpcId is active, -1 - task is inactive 
*
* INPUTS:
*         int ipc_id: task identifier 
*
* OUTPUTS: None
*
* SIDE EFFECTS:
*
*******************************************************************************/
int vosIpcCheckTaskRunning(int nIpcId)
{
    int i;
    struct VOS_IPC_CTRL_MEM_s *pstIpcCtrlMem;

    pstIpcCtrlMem = (struct VOS_IPC_CTRL_MEM_s *) g_pcIpcIdShaMemData;

    for (i = 0; i < pstIpcCtrlMem->nCount; i++) {
        if (pstIpcCtrlMem->astIpcCtrl[i].nType != VOS_IPC_TYPE_UNUSED &&
            pstIpcCtrlMem->astIpcCtrl[i].nIpcId == nIpcId) {
            VOS_IPC_DEBUG("vosIpcCheckTaskRunning: found task %d id running\n", nIpcId); 
            return 0;
        }
    }

    return (-1);
}

/*******************************************************************************
*
* NAME: vosIpcCheckCliActiveTasks
*
* DESCRIPTION: Because multiple CLI instances are allowed (max 6).
*              CLI call this function to get the number of active CLI tasks
*
* RETURN:
*         Number of active CLI task id in IPC table 
*
* INPUTS: None
*
* OUTPUTS: None
*
* SIDE EFFECTS:
*
*******************************************************************************/
int vosIpcCheckCliActiveTasks(void)
{
    int i, j, k = 0;
    struct VOS_IPC_CTRL_MEM_s *pstIpcCtrlMem;

    pstIpcCtrlMem = (struct VOS_IPC_CTRL_MEM_s *) g_pcIpcIdShaMemData;

    for (j = VOS_IPC_ID_CLI_1; j <= VOS_IPC_ID_CLI_6; j++) {
        for (i = 0; i < pstIpcCtrlMem->nCount; i++) {
            if (pstIpcCtrlMem->astIpcCtrl[i].nType != VOS_IPC_TYPE_UNUSED &&
                pstIpcCtrlMem->astIpcCtrl[i].nIpcId == j) {
                k++; /* cli task id is in use */
            }
        }
    }

    return (k);
}

/*******************************************************************************
*
* NAME: vosIpcAsyncProcess
*
* DESCRIPTION: The called process use this function to retrieve data from the
*              queue.
*
* RETURN: success for data available.
*
* INPUTS: 
*         char *pcIpcId: IPC ID share memory address
*         VOS_IPC_CTRL_s *pstIpcCtrl: task ID table entry address 
*
* OUTPUTS:
*         char *pBuf: whole data packet 
*
* SIDE EFFECTS: The caller need to make sure data available before calling this 
*               function.
*
*******************************************************************************/
static int vosIpcAsyncProcess(char *pcIpcId, struct VOS_IPC_CTRL_s *pstIpcCtrl, char *pBuf)
{
    int nRetVal;
    struct VOS_IPC_MEM_s *pstIpcMem;

    pstIpcMem = (struct VOS_IPC_MEM_s *) pcIpcId;

    g_stAcquire.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_ASEM2;
    semop(g_nSemaId, &g_stAcquire, 1);
    memcpy(pBuf, pstIpcMem->aucAsyncBuf[pstIpcMem->nAsyncTail], VOS_IPC_DATA_SIZE);  /* Get data */
    pstIpcMem->nAsyncTail++;
    if (pstIpcMem->nAsyncTail == VOS_IPC_ASYNC_SLOT)
        pstIpcMem->nAsyncTail = 0;
    g_stRelease.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_ASEM2;
    semop(g_nSemaId, &g_stRelease, 1);

#ifdef VOS_IPC_LOCAL_DEBUG
    VOS_IPC_DEBUG("vosIpcAsyncProcess:\n");
    vosIpcDebugTrace(pBuf);
#endif

    // Increase available queue number
    g_stRelease.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_ASEM1;
    semop(g_nSemaId, &g_stRelease, 1);
    nRetVal = semctl(g_nSemaId, pstIpcCtrl->nSemaBase + VOS_IPC_ASEM1, GETVAL, 0);

    VOS_IPC_DEBUG("vosIpcAsyncProcess:\n new queue size:%d\n", nRetVal);

    if (nRetVal != VOS_IPC_ASYNC_SLOT) {
        return (VOS_IPC_DATA_AVAILABLE);    // Caller should get data again
    } else {
        return (VOS_IPC_SUCCESS);
    }
}

/*******************************************************************************
*
* NAME: vosIpcShareSyncEvenNotify
*
* DESCRIPTION: Share memory/Semaphore version.
*
* RETURN: sync event success or failure.
*
* INPUTS: 
*         int nDstIpcId: dest IPC identifier 
*         VOS_IPC_CTRL_s *pstIpcCtrl: task ID table entry address 
*         char *pcIpcId: IPC id share memory address
*         char *pBuf: input data packet 
*         int size: input data packet size 
*         int nMillisecond: timeout value (unit: nMillisecond) 
*         int flag: VOS_IPC_SEM1_SET(1)/VOS_IPC_SEM1_CLEAR(0)
*
* OUTPUTS:
*         char *pBuf: output data packet 
*
* SIDE EFFECTS:
*
* VOS_IPC_SEM1 to make sure only one CLI or SNMP user can access resource (share mem)
* VOS_IPC_SEM2 to wakeup the process in sync_wait
* VOS_IPC_SEM3 to protect only one process can write to share memory
*
*******************************************************************************/

static int vosIpcShareSyncThreadNotify(int nDstIpcId,
                                       struct VOS_IPC_CTRL_s *pstIpcCtrl)

{

    g_stRelease.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM2;
    semop(g_nSemaId, &g_stRelease, 1);  /* Wakeup the thread generate process sleep in sync_wait */

    return 0;
}



static int vosIpcShareSyncEvenNotify(int nDstIpcId,
                                       struct VOS_IPC_CTRL_s *pstIpcCtrl,
                                       char *pcIpcId, char *pBuf, int nSize,
                                       int nMillisecond, int nFlag)
{
    int myType = 0, ret = VOS_IPC_SUCCESS, tryCount = 0;
    unsigned long seq, replySeq;
    struct VOS_IPC_MEM_s *pstIpcMem;
    int loop = nMillisecond / 10; /* Linux 2.4 define HZ to 100 (10 nMillisecond) */

	VOS_TLV_MSG_t *ptlvMsg = NULL;
	
	ptlvMsg = (VOS_TLV_MSG_t *)pBuf;
	
	VOS_IPC_DEBUG("--notify--in ipc notify, nsize: %d\n",nSize);
	VOS_IPC_DEBUG("--notify--in ipc notify, event id: %d\n",ptlvMsg->event_id);


#ifdef VOS_IPC_LOCAL_DEBUG
    VOS_IPC_DEBUG("VOS_IPC_SEM1 value:%d\n",
        semctl(g_nSemaId, pstIpcCtrl->nSemaBase + VOS_IPC_SEM1, GETVAL, 0));
#endif

    pstIpcMem = (struct VOS_IPC_MEM_s *) pcIpcId;


#ifdef VOS_IPC_LOCAL_DEBUG
    VOS_IPC_DEBUG("addr:%x %x id:%d\n", g_pcShaMemData, pcIpcId, nDstIpcId);
    vosIpcDebugTrace(pBuf);
#endif

	/*get global access sema */
    g_stAcquire.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM4; 
    semop(g_nSemaId, &g_stAcquire, 1);
	VOS_IPC_DEBUG("-----------global access sema get\n");
	
	/* write data to shared memory */
    g_stAcquire.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM3; /* get write sema */
    semop(g_nSemaId, &g_stAcquire, 1); /* protect low layer buf */
	VOS_IPC_DEBUG("copying data to share memory: %s\n",pBuf);

    memcpy(pstIpcMem->aucSyncBuf, pBuf, nSize);   /* send user message to dest */

	g_stRelease.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM3; 
	semop(g_nSemaId, &g_stRelease, 1);

	VOS_IPC_DEBUG("data in ipc addr:%x\n", pstIpcMem);
	VOS_IPC_DEBUG("now send data to opconn\n");

	/* notify wait thread to get data */
    g_stRelease.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM2;//go to:vosIpcShareSyncEvenWait
    semop(g_nSemaId, &g_stRelease, 1); 
	VOS_IPC_DEBUG("waiting for reply, try to get sema...\n\n");

	/* get reply from shared memory */
	g_stAcquire.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM1; // wait for reply conntect finished...
	semop(g_nSemaId, &g_stAcquire, 1);
	VOS_IPC_DEBUG("ok,sema got, here is the reply: \n");

	memcpy(pBuf, pstIpcMem->aucSyncBuf, nSize);
	VOS_IPC_DEBUG("reply back event id: %d\n",ptlvMsg->event_id);
	VOS_IPC_DEBUG("data: %d,%d,%d,%d\n",ptlvMsg->data[4],ptlvMsg->data[5],ptlvMsg->data[6],ptlvMsg->data[7]);

	g_stRelease.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM3; 
	semop(g_nSemaId, &g_stRelease, 1);

	g_stRelease.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM4; 
	semop(g_nSemaId, &g_stRelease, 1);
	VOS_IPC_DEBUG("----------global access sema released\n");

	
	


#ifdef VOS_IPC_LOCAL_DEBUG
    VOS_IPC_DEBUG("VOS_IPC_SEM2 value:%d\n",
        semctl(g_nSemaId, pstIpcCtrl->nSemaBase + VOS_IPC_SEM2, GETVAL, 0));
#endif

#ifdef VOS_IPC_LOCAL_DEBUG
    VOS_IPC_DEBUG("result:\n");
    vosIpcDebugTrace(pBuf);
#endif

	VOS_IPC_DEBUG("----------notify end\n\n\n");


    return (ret);
}

/*******************************************************************************
*
* NAME: vosIpcShareSyncEvenWait
*
* DESCRIPTION: Share memory/Semaphore version.
*
* RETURN: success
*
* INPUTS: 
*         int nDstIpcId: called process identifier 
*         VOS_IPC_CTRL_s *pstIpcCtrl: task ID table entry address 
*         char *pcIpcId: IPC id share memory address
*
* OUTPUTS:
*         char *pBuf: whole data packet 
*
* SIDE EFFECTS: No return until event arrived.
*
*******************************************************************************/

#ifdef VOS_IPC_FOR_HAL
int ipcGetEventIdTlvPara(VOS_TLV_MSG_t *ptlvMsg, OPL_IPC_DATA_t *pDataOut)
{

	pDataOut->apiId = ptlvMsg->event_id;
	pDataOut->tlvbuff = (void *)ptlvMsg;

}
#endif

static int vosIpcShareSyncEvenWait(int nDstIpcId,
                                     struct VOS_IPC_CTRL_s *pstIpcCtrl, char *pcIpcId)
{
    struct VOS_IPC_MEM_s *pstIpcMem;
	char replyBuf[100] = "data reply back to web  cgi-data reply back to web";
	VOS_TLV_MSG_t *ptlvMsg = NULL;
		
#ifdef VOS_IPC_FOR_HAL
	int halRes = 0;
	OPL_IPC_DATA_t pstData;
	memset(&pstData,0,sizeof(OPL_IPC_DATA_t));
#endif


    pstIpcMem = (struct VOS_IPC_MEM_s *)pcIpcId;
	VOS_IPC_DEBUG("--event wait--ipc id %d waiting for data ...\n",nDstIpcId);

    g_stAcquire.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM2;
    semop(g_nSemaId, &g_stAcquire, 1);   /* Wait sync_notify */

    VOS_IPC_DEBUG("==== Event Wake Up SeqNo %lu ====\n", g_ulSeqNo);

    VOS_IPC_DEBUG("SEM2:%d, id:%d\n",
        semctl(g_nSemaId, pstIpcCtrl->nSemaBase + VOS_IPC_SEM2, GETVAL, 0));

	//VOS_IPC_DEBUG("copy data from ipc addr: %x\n",pstIpcMem);
    //memcpy(pBuf, pstIpcMem->aucSyncBuf, VOS_IPC_DATA_SIZE); /* copy data */
	
	ptlvMsg = (VOS_TLV_MSG_t *)pstIpcMem->aucSyncBuf;
	VOS_IPC_DEBUG("--event wait--ok, wait thread get data from notify success: \n");
	VOS_IPC_DEBUG("#########data#########\n");
	VOS_IPC_DEBUG("event id : %d\n",ptlvMsg->event_id);
	VOS_IPC_DEBUG("before call hal data4-7: %d,%d,%d,%d\n",ptlvMsg->data[4],ptlvMsg->data[5],ptlvMsg->data[6],ptlvMsg->data[7]);
	VOS_IPC_DEBUG("#########data#########\n");

// analysis and call below.

	VOS_IPC_DEBUG("--event wait--wait for hal...\n");


#ifdef VOS_IPC_SHM_TEST
	VOS_IPC_DEBUG("--event wait--ok, get result from hal: %s\n",replyBuf);

//after get reply, write back
	g_stAcquire.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM3; /* pho-added new */
	semop(g_nSemaId, &g_stAcquire, 1); /* protect low layer buf */
	
	memcpy(pstIpcMem->aucSyncBuf, replyBuf, sizeof(replyBuf));   /* send user message to dest */
	VOS_IPC_DEBUG("--event wait--write result to shm, and reply back to cgi with data: %s,\n address: %x\n",pstIpcMem->aucSyncBuf, (int)pstIpcMem);

	VOS_IPC_DEBUG("--event wait--write ok, notify cgi to get reply\n");
	g_stRelease.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM1; // let notify know the reply is ready.
	semop(g_nSemaId, &g_stRelease, 1); /* goto :vosIpcShareSyncEvenNotify */

	#ifdef VOS_IPC_LOCAL_DEBUG
    VOS_IPC_DEBUG("addr:%x %x, id:%d\n", g_pcShaMemData, pcIpcId, nDstIpcId);
    vosIpcDebugTrace(pstIpcMem->aucSyncBuf);
	#endif

    VOS_IPC_DEBUG("==== Event Process End SeqNo %lu ====\n", g_ulSeqNo);

	VOS_IPC_DEBUG("--event wait--end\n\n");
	
    return (VOS_IPC_SUCCESS);
	
#elif defined(VOS_IPC_FOR_HAL)

	VOS_IPC_DEBUG("--event wait--get cmd id and tlv for hal...\n");
		ipcGetEventIdTlvPara((VOS_TLV_MSG_t *)pstIpcMem->aucSyncBuf, &pstData);

	g_stAcquire.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM3; /* try to let hal write back */
	semop(g_nSemaId, &g_stAcquire, 1); /* protect low layer buf */		
	
	VOS_IPC_DEBUG("--event wait--call to hal with cmd id: %d\n",pstData.apiId);	
		halRes = IpcHalAppApiCallSync(&pstData);
	VOS_IPC_DEBUG("--event wait--hal returns: %d\n",halRes);

	VOS_IPC_DEBUG("after call hal data4-7: %d,%d,%d,%d\n",ptlvMsg->data[4],ptlvMsg->data[5],ptlvMsg->data[6],ptlvMsg->data[7]);

	if (halRes == 0){
		VOS_IPC_DEBUG("--event wait--write ok, notify cgi to get reply\n");
	}else{
		VOS_IPC_DEBUG("--event wait--notify cgi with hal error. :( \n");
	}
	
	g_stRelease.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM1; // let notify know the reply is ready.
	semop(g_nSemaId, &g_stRelease, 1); /* goto :vosIpcShareSyncEvenNotify */
	
	VOS_IPC_DEBUG("==== Event Process End SeqNo %lu ====\n", g_ulSeqNo);
	
	VOS_IPC_DEBUG("--event wait--end\n\n");
	
	return (VOS_IPC_SUCCESS);

	
#else

	VOS_IPC_DEBUG("--event wait--do nothing, original data back to cgi. \n");
	g_stRelease.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM1; // let notify know the reply is ready.
	semop(g_nSemaId, &g_stRelease, 1); /* goto :vosIpcShareSyncEvenNotify */

	VOS_IPC_DEBUG("==== Event Process End SeqNo %lu ====\n", g_ulSeqNo);

	VOS_IPC_DEBUG("--event wait--end\n\n");

#endif

}

// refer to threadBoot
void *vosIPCWaitThreadInit(void *nipcId)
{
	int nIpcId = *((int *)nipcId);
	
	pthread_detach(pthread_self());
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	
	VOS_IPC_DEBUG("in new thread wait()\n");
	
	while (1){
		vosIpcSyncEventWait(nIpcId);
	}

}


static int vosIpcShareMemThreadWait(int nDstIpcId, struct VOS_IPC_CTRL_s *pstIpcCtrl, char *pcIpcId)
{
	char ipcIdStr[10] = {0};
	struct VOS_IPC_MEM_s *pstIpcMem;
	pthread_t a_thread, b_thread, c_thread, d_thread;
	int newIpcId, initIpcId;
	int res = 0;
	
	if ( nDstIpcId != VOS_IPC_ID_EXTERNAL_10 ){
		VOS_IPC_DEBUG("Thread Wait error: wrong ipc id.\n");
		return VOS_IPC_GENERAL_ERR;
	}
	
	VOS_IPC_DEBUG("Thread Wait for ipc to register \n");
	pstIpcMem = (struct VOS_IPC_MEM_s *)pcIpcId;
    g_stAcquire.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM2;
    semop(g_nSemaId, &g_stAcquire, 1);   /* Wait new ipc id to register */

	memcpy(ipcIdStr, pstIpcMem, sizeof(ipcIdStr));

    g_stRelease.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_SEM3; 
    semop(g_nSemaId, &g_stRelease, 1);	
	
	newIpcId = atoi(ipcIdStr);
	if (newIpcId >= 100){// for unregister
		newIpcId -= 100;
		if ( pthread_cancel(gPthreadArr[newIpcId]) == 0 ){
			VOS_IPC_DEBUG("newly unregister ipc id successfully: %d\n",newIpcId);
		}else{
			VOS_IPC_DEBUG("newly unregister ipc id un-successfully: %d\n",newIpcId);
		};
	}else{// for register
		VOS_IPC_DEBUG("newly register ipc id: %d\n",newIpcId);
		
		res = pthread_create(&a_thread, NULL, vosIPCWaitThreadInit, (void *)&newIpcId );
		if ( res != 0 ){
				perror("thread create failed\n");
		}

		initIpcId = VOS_IPC_ID_EXTERNAL_10;//continue to wait for register
		res = pthread_create(&b_thread, NULL, vosIPCWaitThreadInit, (void *)&initIpcId );
		if ( res != 0 ){
				perror("thread create failed\n");
		}	
		
		VOS_IPC_DEBUG("newly register ipc id %s wait started.\n",ipcIdStr);
		
		pthread_join(a_thread, NULL);
		pthread_join(b_thread, NULL);
	}
    return (VOS_IPC_SUCCESS);
}


/*******************************************************************************
*
* NAME: vosIpcShareSyncEvenReply
*
* DESCRIPTION: Share memory/Semaphore version.
*
* RETURN: return data to the caller process success or failure.
*
* INPUTS: 
*         int nDstIpcId: called process identifier 
*         VOS_IPC_CTRL_s *pstIpcCtrl: task ID table entry address 
*         char *pcIpcId: IPC id share memory address
*         char *pBuf: whole data packet 
*
* OUTPUTS:
*
* SIDE EFFECTS:
*
*******************************************************************************/
static int vosIpcShareSyncEvenReply(int nDstIpcId,
                                      struct VOS_IPC_CTRL_s *pstIpcCtrl,
                                      char *pcIpcId, char *pBuf)
{
    int nRetVal;
    unsigned long seq, replySeq;
    struct VOS_IPC_MEM_s *pstIpcMem;

    pstIpcMem = (struct VOS_IPC_MEM_s *)pcIpcId;

#ifdef VOS_IPC_LOCAL_DEBUG
    VOS_IPC_DEBUG("\n");
    vosIpcDebugTrace(pBuf);
#endif

    seq = vosTlvGetInt((char *) &pstIpcMem->aucSyncBuf[VOS_TLV_SEQ_NO_OFFSET]);
    replySeq = vosTlvGetInt(&pBuf[VOS_TLV_RTN_SEQ_NO_OFFSET]);

    VOS_IPC_DEBUG("nDstIpcId:%lu, seq:%lu, replySeq:%ly\n",
        nDstIpcId, seq, replySeq);

    if (seq == replySeq) {    /* command back from reply */
        memcpy(pstIpcMem, pBuf, VOS_IPC_DATA_SIZE);  /* Send reply data */
        nRetVal = VOS_IPC_SUCCESS;

    } else { /* seq mismatch - may be timeout command now return */
        VOS_IPC_DEBUG("nDstIpcId:%d, eno_seq:%ld, reply_seq:%ld (new command)\n",
            nDstIpcId, seq, replySeq);
        nRetVal = VOS_IPC_SUCCESS; /* mismatch, ignore it, possibly late (timeout) reply */
    }

    return (nRetVal);
}

/*******************************************************************************
*
* NAME: vosIpcShareAsyncEvenNotify
*
* DESCRIPTION: Share memory/Semaphore version.
*
* RETURN: Put data to the queue success or failure.
*
* INPUTS: 
*         int nDstIpcId: dest eno identifier
*         VOS_IPC_CTRL_s *pstIpcCtrl: task ID table entry address 
*         char *pcIpcId: IPC id share memory address
*         char *pBuf: whole data packet 
*
* OUTPUTS: None
*
* SIDE EFFECTS:
*
*******************************************************************************/
static int vosIpcShareAsyncEvenNotify(int nDstIpcId,
                                        struct VOS_IPC_CTRL_s *pstIpcCtrl,
                                        char *pcIpcId, char *pBuf)
{
    int nRetVal;
    struct VOS_IPC_MEM_s *pstIpcMem;

    pstIpcMem = (struct VOS_IPC_MEM_s *)pcIpcId;

    nRetVal = semctl(g_nSemaId, pstIpcCtrl->nSemaBase + VOS_IPC_ASEM1, GETVAL, 0);
    VOS_IPC_DEBUG("vosIpcShareAsyncEvenNotify: id:%d, queue size:%d\n", nDstIpcId, nRetVal);

    if (nRetVal == 0)
        return (VOS_IPC_QUEUE_FULL_ERR);   /* No space, consumer has problem */

#ifdef VOS_IPC_LOCAL_DEBUG
    VOS_IPC_DEBUG("\n");
    vosIpcDebugTrace(pBuf);
#endif

    g_stAcquire.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_ASEM2;
    semop(g_nSemaId, &g_stAcquire, 1);
    /* Put data to share memory queue */
    memcpy(pstIpcMem->aucAsyncBuf[pstIpcMem->nAsyncHead], pBuf, VOS_IPC_DATA_SIZE);
    pstIpcMem->nAsyncHead++;
    if (pstIpcMem->nAsyncHead == VOS_IPC_ASYNC_SLOT)
        pstIpcMem->nAsyncHead = 0;
    g_stRelease.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_ASEM2;
    semop(g_nSemaId, &g_stRelease, 1);

    g_stAcquire.sem_num = pstIpcCtrl->nSemaBase + VOS_IPC_ASEM1;
    semop(g_nSemaId, &g_stAcquire, 1);  /* Decrease available queue number */

    VOS_IPC_DEBUG("vosIpcShareAsyncEvenNotify: new queue size:%d\n",
        semctl(g_nSemaId, pstIpcCtrl->nSemaBase + VOS_IPC_ASEM1, GETVAL, 0));

    return (VOS_IPC_SUCCESS);
}

/*******************************************************************************
*
* NAME: vosIpcShareAsyncEvenWait
*
* DESCRIPTION: Share memory/Semaphore version.
*
* RETURN: success for data available.
*
* INPUTS: 
*         int nDstIpcId: called process identifier
*         VOS_IPC_CTRL_s *pstIpcCtrl: task ID table entry address 
*         char *pcIpcId: IPC id share memory address
*
* OUTPUTS:
*         char *pBuf: whole data packet 
*
* SIDE EFFECTS: No return until event arrived.
*
*******************************************************************************/
static int vosIpcShareAsyncEvenWait(int nDstIpcId,
                                      struct VOS_IPC_CTRL_s *pstIpcCtrl,
                                      char *pcIpcId, char *pBuf)
{
    int nRetVal;

    nRetVal = semctl(g_nSemaId, pstIpcCtrl->nSemaBase + VOS_IPC_ASEM1, GETVAL, 0);
    if (nRetVal != VOS_IPC_ASYNC_SLOT) {
        VOS_IPC_DEBUG("vosIpcShareAsyncEvenWait: id:%d, queue size:%d\n", 
            nDstIpcId, nRetVal);
        return (vosIpcAsyncProcess(pcIpcId, pstIpcCtrl, pBuf));
    } else {                   /*  No producer data */
        do {
            nRetVal = semctl(g_nSemaId, pstIpcCtrl->nSemaBase + VOS_IPC_ASEM1, GETVAL, 0);
            vosIpcSleep(1);
        } while (nRetVal == VOS_IPC_ASYNC_SLOT);
        return (vosIpcAsyncProcess(pcIpcId, pstIpcCtrl, pBuf));
    }
}

/*******************************************************************************
*
* NAME: vosIpcShareAsyncEvenPoll
*
* DESCRIPTION: Share memory/Semaphore version.
*
* RETURN: success for data available or failure.
*
* INPUTS: 
*         int nDstIpcId: called process identifier
*         VOS_IPC_CTRL_s *pstIpcCtrl: task ID table entry address 
*         char *pcIpcId: IPC id share memory address
*
* OUTPUTS:
*         char *pBuf: whole data packet 
*
* SIDE EFFECTS:
*
*******************************************************************************/
static int vosIpcShareAsyncEvenPoll(int nDstIpcId,
                                      struct VOS_IPC_CTRL_s *pstIpcCtrl,
                                      char *pcIpcId, char *pBuf)
{
    int nRetVal;

    nRetVal = semctl(g_nSemaId, pstIpcCtrl->nSemaBase + VOS_IPC_ASEM1, GETVAL, 0);
    if (nRetVal != VOS_IPC_ASYNC_SLOT) {
        VOS_IPC_DEBUG("vosIpcShareAsyncEvenPoll: id:%d, queue size:%d\n",
            nDstIpcId, nRetVal);
        return (vosIpcAsyncProcess(pcIpcId, pstIpcCtrl, pBuf));
    } else {
        return (VOS_IPC_QUEUE_EMPTY_ERR);
    }
}

void vosIpcPrintDestIpcId(int nDstIpcId)
{
    switch(nDstIpcId) {
    case VOS_IPC_ID_CLI_1: 
        printf("nDstIpcId = VOS_IPC_ID_CLI_1\n");
	break;
    case VOS_IPC_ID_CLI_2:
        printf("nDstIpcId = VOS_IPC_ID_CLI_2\n");
	break;
    case VOS_IPC_ID_CLI_3:
        printf("nDstIpcId = VOS_IPC_ID_CLI_3\n");
	break;
    case VOS_IPC_ID_CLI_4:
        printf("nDstIpcId = VOS_IPC_ID_CLI_4\n");
	break;
    case VOS_IPC_ID_CLI_5:
        printf("nDstIpcId = VOS_IPC_ID_CLI_5\n");
	break;
    case VOS_IPC_ID_CLI_6:
        printf("nDstIpcId = VOS_IPC_ID_CLI_6\n");
	break;
    case VOS_IPC_ID_OAM: 
        printf("nDstIpcId = VOS_IPC_ID_OAM\n");
	break;
    case VOS_IPC_ID_SNMP: 
        printf("nDstIpcId = VOS_IPC_ID_SNMP\n");
	break;
    case VOS_IPC_ID_WEB: 
        printf("nDstIpcId = VOS_IPC_ID_WEB\n");
	break;
    case VOS_IPC_ID_EXTERNAL_1: 
        printf("nDstIpcId = VOS_IPC_ID_EXTERNAL_1\n");
	break;
    case VOS_IPC_ID_EXTERNAL_2: 
        printf("nDstIpcId = VOS_IPC_ID_EXTERNAL_2\n");
	break;
    case VOS_IPC_ID_EXTERNAL_3: 
        printf("nDstIpcId = VOS_IPC_ID_EXTERNAL_3\n");
	break;
    case VOS_IPC_ID_EXTERNAL_4: 
        printf("nDstIpcId = VOS_IPC_ID_EXTERNAL_4\n");
	break;
    case VOS_IPC_ID_EXTERNAL_5: 
        printf("nDstIpcId = VOS_IPC_ID_EXTERNAL_5\n");
	break;
    case VOS_IPC_ID_EXTERNAL_6:
        printf("nDstIpcId = VOS_IPC_ID_EXTERNAL_6\n");
	break;
    case VOS_IPC_ID_EXTERNAL_7: 
        printf("nDstIpcId = VOS_IPC_ID_EXTERNAL_7\n");
	break;
    case VOS_IPC_ID_EXTERNAL_8: 
        printf("nDstIpcId = VOS_IPC_ID_EXTERNAL_8\n");
	break;
    case VOS_IPC_ID_EXTERNAL_9: 
        printf("nDstIpcId = VOS_IPC_ID_EXTERNAL_9\n");
	break;
    case VOS_IPC_ID_EXTERNAL_10: 
        printf("nDstIpcId = VOS_IPC_ID_EXTERNAL_10\n");
	break;
    default:
    	printf("nDstIpcId = ?\n");
	break;
    }
}

/* END FILE */
