/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*   used only with Opulan semi-conductor products. 
*
* FILENAME:  vos_thread.c
*
* DESCRIPTION: 
*   
*
* Date Created: Oct 31, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos_thread.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sched.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <vos_types.h>

#include <vos_thread.h>
#include <vos_time.h>
#include <vos_sync.h>
#include <vos_alloc.h>

/* thread priority 0~255 -> 99~1 */
#define THREAD_PRI(pri) ((int)(99-(pri)/2.603))

typedef struct THREAD_INFO_s {
    void        *(*vFunc)(void *);
    pthread_t   tId;
    int         nPid;
    int         nStackSize; 
    int         nPriority;
    char        *pcName;
    void        *pvArg;
    struct THREAD_INFO_s *pstNext;
} THREAD_INFO_t;

static THREAD_INFO_t    *pInfoHead;
static VOS_THREAD_t pThreadMain = 0;
static VOS_MUTEX_t g_pstThreadLock;

/* List of signals to mask in the subthreads */
static int g_naSigList[] = {
        SIGINT, SIGPIPE, 0
};

/*******************************************************************************
*
* vosThreadInit:	 init thread task
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	n/a.
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	0 on success
*   -1 on failure
*
* SEE ALSO: 
*/

int vosThreadInit(void)
{
    if (VOS_MUTEX_OK != vosMutexCreate(&g_pstThreadLock))
    { 
        printf("vosThreadInit: create mutex failed.\r\n");
        return -1;
    } 
    return 0;
}

static void *vosThreadSetup(void *pstThreadInfo)
{
    int i;
    sigset_t mask;
    THREAD_INFO_t *pstLocalThreadInfo = pstThreadInfo;
    void      *(*vFunc)(void *);
    void      *pvArg;

    /* Mask asynchronous signals for this thread */
    sigemptyset(&mask);
    for ( i=0; g_naSigList[i]; ++i ) {
        sigaddset(&mask, g_naSigList[i]);
    }
    sigprocmask(SIG_BLOCK, &mask, 0);

    /* Allow ourselves to be asynchronously cancelled */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    vFunc = pstLocalThreadInfo->vFunc;
    pvArg = pstLocalThreadInfo->pvArg;

    pstLocalThreadInfo->tId = pthread_self();
    pstLocalThreadInfo->nPid = getpid();

    return (*vFunc)(pvArg);
}

/*******************************************************************************
*
* vosThreadCreate:
*
* DESCRIPTION:
* 	
*
* INPUTS:
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	Thread ID
*
* SEE ALSO: 
*/
VOS_THREAD_t vosThreadCreate(char *name, int stackSize, int pri, void *(*func)(void *), void *arg)
{
	pthread_attr_t    attribs;
	THREAD_INFO_t *pThreadInfo;
	pthread_t     id = 0;
    struct sched_param param;

	if ((pThreadInfo = (THREAD_INFO_t *)malloc(sizeof(struct THREAD_INFO_s))) == NULL) return id;

	if (pthread_attr_init(&attribs)) return id;

	vosMutexTake(&g_pstThreadLock);

#if 0  /* deleted by Gan Zhiheng - 2010/10/30 */
    param.sched_priority = THREAD_PRI(pri);
    pthread_attr_setschedparam(&attribs, &param);
    pthread_attr_setschedpolicy(&attribs, SCHED_RR);
#endif /* #if 0 */
	pthread_attr_setstacksize(&attribs, stackSize);

	pThreadInfo->tId = 0;
	pThreadInfo->nPid = 0;
    pThreadInfo->nStackSize = stackSize;
    pThreadInfo->nPriority = pri;
	pThreadInfo->pvArg = arg;
	pThreadInfo->vFunc = func;
	pThreadInfo->pcName = (char *)vosStrDup(name);
	pThreadInfo->pstNext = pInfoHead;
    pInfoHead = pThreadInfo;

	if (pthread_create(&id, &attribs, vosThreadSetup, (void *)pThreadInfo)){
        pInfoHead = pInfoHead->pstNext;
        free(pThreadInfo->pcName);
		free(pThreadInfo);
        vosMutexGive(&g_pstThreadLock);
		return id;
	}

    while (pThreadInfo->tId == 0) {
        usleep(1000);
    }

    id = pThreadInfo->tId;

    pthread_detach(pThreadInfo->tId);

    pthread_attr_destroy(&attribs);

	vosMutexGive(&g_pstThreadLock);
	return id;
}


/*******************************************************************************
*
* vosThreadDestroy:
*
* DESCRIPTION:
*
* INPUTS:
*	thread - thread ID
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	0 on success
*   -1 on failure
*
* SEE ALSO: 
*/
int vosThreadDestroy(VOS_THREAD_t thread)
{
	THREAD_INFO_t *pThreadInfo, **temp;
	int           ret = 0;
	
	if (0 != pthread_cancel(thread)){
		ret = -1;
		return ret;
	}
	vosMutexTake(&g_pstThreadLock);
	for (temp = &pInfoHead; (*temp) != NULL; temp = &(*temp)->pstNext) {
        	if ((*temp)->tId == thread) {
	            pThreadInfo = (*temp);
	            (*temp) = (*temp)->pstNext;
                free(pThreadInfo->pcName);
	            free(pThreadInfo);
	            break;
		}
	}
	vosMutexGive(&g_pstThreadLock);
	return ret;
}

/*******************************************************************************
*
* vosThreadSelf:
*
* DESCRIPTION:
* 	Get the thread self ID 
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	Thread ID
*
* SEE ALSO: 
*/
VOS_THREAD_t vosThreadSelf(void)
{
	return (VOS_THREAD_t) pthread_self();
}


/*******************************************************************************
*
* vosThreadExit:
*
* DESCRIPTION:
*
* INPUTS:
*	rc - return code from thread.
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void vosThreadExit(int rc)
{
	THREAD_INFO_t *pThreadInfo, **temp;
	pthread_t     id = pthread_self();

	vosMutexTake(&g_pstThreadLock);
	for (temp = &pInfoHead; (*temp) != NULL; temp = &(*temp)->pstNext) {
	    if ((*temp)->tId == id) {
	        pThreadInfo = (*temp);
	        (*temp) = (*temp)->pstNext;
            free(pThreadInfo->pcName);
	        free(pThreadInfo);
	        break;
	    }
	}
	vosMutexGive(&g_pstThreadLock);
	pthread_exit(0);
}

/*******************************************************************************
*
* vosThreadMainSet:	 Set which thread is the main thread 
*
* DESCRIPTION:
*   The main thread is the one that runs in the foreground on the
*   console.  It prints normally, takes keyboard signals, etc.
*
* INPUTS:
*	thread - thread ID
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void vosThreadMainSet(VOS_THREAD_t thread)
{
   pThreadMain = thread;
}

/*******************************************************************************
*
* vosThreadMainGet:	 Return which thread is the main thread
*
* DESCRIPTION:
* 	Get which thread is the main thread
*
* INPUTS:
*	void
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	Thread ID
*
* SEE ALSO: 
*/
VOS_THREAD_t vosThreadMainGet(void)
{
    return pThreadMain;
}


/*******************************************************************************
*
* vosThreadVerify:	 Verify the existence of a thread
*
* DESCRIPTION:
* 	verify the thread
*
* INPUTS:
*	thread - thread ID
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	0 on success
*   -1 on failure
*
* SEE ALSO: 
*/
int vosThreadVerify(VOS_THREAD_t thread)
{
	THREAD_INFO_t  **tp;
	int           ret = -1;

	vosMutexTake(&g_pstThreadLock);
	for (tp = &pInfoHead; (*tp) != NULL; tp = &(*tp)->pstNext) {
		if ((*tp)->tId == thread) {
			ret = 0;
			break;
        }
	}
	vosMutexGive(&g_pstThreadLock);
	return ret;
}


/*******************************************************************************
*
* vosThreadShow:	 Show thread list
*
* DESCRIPTION:
* 	Show the thread info 
*
* INPUTS:
*	fd - the file descriptor which to write to
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void vosThreadShow(int fd)
{
    THREAD_INFO_t *pThreadInfo;
    int i = 0;
	vosPrintf(fd, " #     Id       Pid        Name         Stack    Priority \r\n");
    vosPrintf(fd, "----- --------- --------- ------------ -------- ---------- \r\n");
	vosMutexTake(&g_pstThreadLock);
	for (pThreadInfo = pInfoHead; pThreadInfo != NULL;pThreadInfo = pThreadInfo->pstNext) 
    {
		vosPrintf(fd, "%5d %9x %9d %-12s %8d %10d\r\n",
            i+1,
            pThreadInfo->tId,
            pThreadInfo->nPid,
            (NULL == pThreadInfo->pcName) ? "" : pThreadInfo->pcName,
            pThreadInfo->nStackSize,
            pThreadInfo->nPriority);
		i++;
	}
	vosPrintf(fd, "\r\n");
	vosMutexGive(&g_pstThreadLock);
}


