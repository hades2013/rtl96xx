/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  vos_timer.c
*
* DESCRIPTION: 
*   has an array of pointers, and the current pos.  When caller wants
*   to schedule timer, it will look at the pos and plus additional tick to it.
*   the timer task will timeout every 10ms, and see if there is something in 
*   the array to execute, if so do it, other wise go back to sleep.
*
* Date Created: Aug 06, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos_timer.c#1 $
* $Log:$
*
*
*/


#include <sys/time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <stdio.h>
#include <vos_types.h>

#include <vos_thread.h>
#include <vos_sync.h>
#include <vos_time.h>
#include <vos_timer.h>
#include <vos_bitmap.h>
#include <vos_alloc.h>

#define VOS_TIMER_FD_PATH "/etc/vos_timer_fd"

enum { 
    CALENDAR_SIZE = 64,
    MAX_TIMER_OBJ = 256,
    MAX_TIMER_OBJ_MAP = MAX_TIMER_OBJ/32
}; 

static TIMER_OBJ_t *g_pstOpTimerTbl = NULL;
static uint32 g_ulOpTimerMap[MAX_TIMER_OBJ_MAP];
static TIMER_OBJ_t *g_pastCalendar[CALENDAR_SIZE];
static uint8 g_ucHand = 0;
static UINT64 g_ulClockWrap = 0;
static uint8 g_ucInitialized = 0;
static VOS_THREAD_t g_pstOpTimerThreadId = NULL;
static VOS_MUTEX_t g_pstOpTimerMutex;
static int g_opTimerFd = 0;

static void timerObjInit(TIMER_OBJ_t *pstTimer);
static void * vosTimerTaskEntry(void *arg);


/*******************************************************************************
*
* vosTimerInit:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosTimerInit()
{
    int i;

    if (g_ucInitialized) {
        printf("vosTimerInit: optimer has already initialized.\r\n");
        return -1;
    }

	if ( NULL != g_pstOpTimerTbl )
	{
		vosFree(g_pstOpTimerTbl);
	}

	g_pstOpTimerTbl = (TIMER_OBJ_t *)vosAlloc(sizeof(TIMER_OBJ_t) * MAX_TIMER_OBJ);

	if  (NULL == g_pstOpTimerTbl)
	{
        printf("vosTimerInit: out of memory, line %d\r\n", __LINE__);
		return -1;
	}

    for(i=0; i<CALENDAR_SIZE; i++)
    {
        g_pastCalendar[i] = NULL;
    }

    g_ucHand = 0;
    g_ulClockWrap = 0;

    for (i = 0; i < MAX_TIMER_OBJ; i++)
    {
        timerObjInit(&g_pstOpTimerTbl[i]);
    }

    for (i = 0; i < MAX_TIMER_OBJ_MAP; i++)
    {
        g_ulOpTimerMap[i] = 0;
    }

    g_opTimerFd = open(VOS_TIMER_FD_PATH, O_RDWR | O_CREAT | O_TRUNC);

    if (vosMutexCreate(&g_pstOpTimerMutex) != VOS_MUTEX_OK) {
        printf("vosTimerInit: g_pstOpTimerMutex create failed.\r\n");
        return -1;
    }

    g_pstOpTimerThreadId = vosThreadCreate("tOpTimer", OP_VOS_THREAD_STKSZ, 30,
                        (void *)vosTimerTaskEntry, (void *)NULL);
    if (g_pstOpTimerThreadId ==(VOS_THREAD_t)NULL) {
        vosMutexDestroy(&g_pstOpTimerMutex);
        printf("vosTimerInit: optimer task create failed.\r\n");
        return -1;
    }

    g_ucInitialized = 1;

    return 0;
}

/*******************************************************************************
*
* vosTimerShutdown:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosTimerShutdown()
{
    int i;

    if (!g_ucInitialized) {
        printf("vosTimerShutdown: optimer has not initialized.\r\n");
        return -1;
    }

    g_ucInitialized = 0;

    if (g_pstOpTimerThreadId != NULL) {
        vosThreadDestroy(g_pstOpTimerThreadId);
        g_pstOpTimerThreadId =(VOS_THREAD_t) NULL;
    }

        vosMutexDestroy(&g_pstOpTimerMutex);

	if ( NULL != g_pstOpTimerTbl )
	{
		vosFree(g_pstOpTimerTbl);
        g_pstOpTimerTbl = (VOS_THREAD_t)NULL;
	}
    for (i = 0; i < MAX_TIMER_OBJ_MAP; i++)
    {
        g_ulOpTimerMap[i] = 0;
    }

    if (0 != g_opTimerFd)
    {
        close(g_opTimerFd);
    }

    return 0;
}

/*******************************************************************************
*
* vosTimerTaskEntry:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
static void * vosTimerTaskEntry(void *arg)
{
    TIMER_OBJ_t * pCurr;
    TIMER_OBJ_t * pTmp = NULL;
    FUNCPTR CallBack = NULL;
    uint32 Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8;
    struct timeval t;
    VOS_TIME_USECS_t start = 0;
    VOS_TIME_USECS_t end = 0;
    VOS_TIME_USECS_t current = 0;

    t.tv_sec = 0;
    t.tv_usec = 1000 * TIMER_GRANULARITY;    //the granularity of timer: 500ms   
    Arg1 = Arg2 = Arg3 = Arg4 = Arg5 = Arg6 = Arg7 = Arg8 = 0xFFFFFFFF;
    start = vosTimeUsecsGet();

    while (1)
    {  
        current = vosTimeUsecsGet();
        if ((current - start) >= (1000 * TIMER_GRANULARITY))
        {
            start = vosTimeUsecsGet();
            vosMutexTake(&g_pstOpTimerMutex);
            g_ucHand++;
            if (g_ucHand >= CALENDAR_SIZE)
            {
                g_ucHand = 0;
                g_ulClockWrap++;
            }
         
            if (g_pastCalendar[g_ucHand] != NULL)
            {
                pCurr = g_pastCalendar[g_ucHand];
                while(pCurr != NULL)
                {  
                    Arg1 = Arg2 = INVALID_32; 
                    CallBack = NULL;
                    pTmp = NULL;
                    if(pCurr->ulWrapCounter == g_ulClockWrap)
                    {
                        if( pCurr->pstCallBackFunc )
                        {
                            /* pCurr->pstCallBackFunc(
                                pCurr->ulCallBackArg1, 
                                pCurr->ulCallBackArg2,
                                pCurr->ulCallBackArg3, 
                                pCurr->ulCallBackArg4,
                                pCurr->ulCallBackArg5, 
                                pCurr->ulCallBackArg6,
                                pCurr->ulCallBackArg7, 
                                pCurr->ulCallBackArg8
                                );*/
                            CallBack = pCurr->pstCallBackFunc;
                            Arg1 = pCurr->ulCallBackArg1;
                            Arg2 = pCurr->ulCallBackArg2;
                            Arg3 = pCurr->ulCallBackArg3;
                            Arg4 = pCurr->ulCallBackArg4;
                            Arg5 = pCurr->ulCallBackArg5;
                            Arg6 = pCurr->ulCallBackArg6;
                            Arg7 = pCurr->ulCallBackArg7;
                            Arg8 = pCurr->ulCallBackArg8;
                      
                            if (pCurr->stType == ONE_SHOT_TIMER)
                            {
                                pTmp = pCurr->pstNext;
                                vosTimerCancel(pCurr);    //will release it and pop out the first one
                                pCurr = pTmp;
                            }
                            else if (pCurr->stType == REOCURRING_TIMER)
                            {
                                pTmp = pCurr->pstNext;
                                vosTimerReSchedule(pCurr, pCurr->ulTimeOut * TIMER_GRANULARITY);
                                pCurr = pTmp;
                            }
                            else
                            {
                                pTmp = pCurr->pstNext;
                                vosTimerCancel(pCurr); 
                                pCurr = pTmp;
                                continue;
                            }
                            vosMutexGive(&g_pstOpTimerMutex);
                            CallBack(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8); 
                            vosMutexTake(&g_pstOpTimerMutex);
                        }
                        else
                        {
                            pTmp = pCurr->pstNext;
                            vosTimerCancel(pCurr);//will release it and pop out the first one
                            pCurr = pTmp;
                        }
                    }
                    else
                    {
                        pCurr = pCurr->pstNext;
                    }
               
                }
            }
            vosMutexGive(&g_pstOpTimerMutex);
            end = vosTimeUsecsGet();
            if ((end - start) < (1000 * TIMER_GRANULARITY))
            {
                t.tv_sec = 0;
                t.tv_usec = (1000 * TIMER_GRANULARITY) - (end - start);
                select(g_opTimerFd,0,0,0,&t);
            }
        }
    }
    vosThreadExit(0);
    return NULL;
}

/*******************************************************************************
*
* timerObjInit:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
static void timerObjInit(TIMER_OBJ_t *pstTimer)
{
   pstTimer->stState            = TMR_ST_FREE;
   pstTimer->ulIndex            = INVALID_32;
   pstTimer->stType             = TMRT_UNKNOWN;
   pstTimer->ulTimeOut          = INVALID_32;
   pstTimer->pstPrev            = NULL;
   pstTimer->pstNext            = NULL; //to nested the timer in the linkedlist
   pstTimer->pstCallBackFunc    = NULL;
   pstTimer->ulCallBackArg1     = INVALID_32;
   pstTimer->ulCallBackArg2     = INVALID_32;
   pstTimer->ulCallBackArg3     = INVALID_32;
   pstTimer->ulCallBackArg4     = INVALID_32;
   pstTimer->ulCallBackArg5     = INVALID_32;
   pstTimer->ulCallBackArg6     = INVALID_32;
   pstTimer->ulCallBackArg7     = INVALID_32;
   pstTimer->ulCallBackArg8     = INVALID_32;
   pstTimer->ulWrapCounter      = (UINT64)(-1); //in which wraparound the callback function will be called
}


/*******************************************************************************
*
* vosTimerGetFree:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
TIMER_OBJ_t * vosTimerGetFree(void)
{
    uint16 i, BitPos, Index;
    TIMER_OBJ_t *pstTimer;

    if (NULL == g_pstOpTimerTbl)
    {
        return NULL;
    }

    vosMutexTake(&g_pstOpTimerMutex);

    for (i = 0; i < MAX_TIMER_OBJ_MAP; i++)
    {
        BitPos = vosFirstClearBitPosGet(g_ulOpTimerMap[i], 0);
        if( BitPos != INVALID_16) break;
    }
    
    if( BitPos == INVALID_16 )
    {
        printf("optimer exceed the maximum.\r\n");
        vosMutexGive(&g_pstOpTimerMutex);
        return NULL;
    }  

    Index = i*32 + BitPos;
    /* Begin Added*/
    if (MAX_TIMER_OBJ <= Index)
    {
        vosMutexGive(&g_pstOpTimerMutex);
        return NULL;
    }
    /* End   Added of porting */
    pstTimer = &g_pstOpTimerTbl[Index];
    timerObjInit(pstTimer);
    pstTimer->ulIndex = Index;
    pstTimer->stState = TMR_ST_TAKEN;
    /* Begin Added */
    if (MAX_TIMER_OBJ_MAP <= i)
    {
        vosMutexGive(&g_pstOpTimerMutex);
        return NULL;
    }
    /* End   Added of porting */
    g_ulOpTimerMap[i] = vosBitSet(g_ulOpTimerMap[i], BitPos);
    
    vosMutexGive(&g_pstOpTimerMutex);

    return pstTimer;
}

/*******************************************************************************
*
* vosTimerRelease:	relaase timer 
*
* DESCRIPTION:
* 	This function is used to release timer
*
* INPUTS:
*	pstTimer - the timer to be released
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosTimerRelease(TIMER_OBJ_t *pstTimer)
{
    uint16 i, BitPos;

    if (NULL == pstTimer)
    {
        return -1;
    }

    vosMutexTake(&g_pstOpTimerMutex);

    BitPos = (pstTimer->ulIndex) % 32;
    i = (uint16)((pstTimer->ulIndex) / 32);
    g_ulOpTimerMap[i] = vosBitClear(g_ulOpTimerMap[i], BitPos);
    timerObjInit(pstTimer);
    
    vosMutexGive(&g_pstOpTimerMutex);

    return 0;
}

/*******************************************************************************
*
* vosTimerStart:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pstTimer    - timer object
*   stType      - timer type: ONE_SHOT_TIMER or REOCURRING_TIMER
*   ulTimeout   - timeout value, in millisecond
*   pstCallBack - timer call back function
*   ulCallBackArg1, ulCallBackArg2, ... ulCallBackArg8 - 
*                 call back function arguments
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosTimerStart(TIMER_OBJ_t *pstTimer, 
    TIMER_TYPE_t stType, 
    uint32 ulTimeout, 
    FUNCPTR pstCallBack, 
    uint32 ulCallBackArg1, 
    uint32 ulCallBackArg2,
    uint32 ulCallBackArg3, 
    uint32 ulCallBackArg4,
    uint32 ulCallBackArg5, 
    uint32 ulCallBackArg6,
    uint32 ulCallBackArg7, 
    uint32 ulCallBackArg8)
{
    if (NULL == pstTimer)
    {
        return -1;
    }
    
    if ((REOCURRING_TIMER != stType) && (ONE_SHOT_TIMER != stType))
    {
        return -1;
    }
   
    vosMutexTake(&g_pstOpTimerMutex);

    if (TMR_ST_RUNNING == pstTimer->stState)
    {
        vosMutexGive(&g_pstOpTimerMutex);
        return 0;
    }
   
    pstTimer->stState = TMR_ST_RUNNING;
    pstTimer->stType = stType;      //also need to protect these local data
    pstTimer->pstCallBackFunc = pstCallBack;   //incase the callers have 2 task
    pstTimer->ulCallBackArg1 = ulCallBackArg1; //that callit
    pstTimer->ulCallBackArg2 = ulCallBackArg2; //that callit
    pstTimer->ulCallBackArg3 = ulCallBackArg3; //that callit
    pstTimer->ulCallBackArg4 = ulCallBackArg4; //that callit
    pstTimer->ulCallBackArg5 = ulCallBackArg5; //that callit
    pstTimer->ulCallBackArg6 = ulCallBackArg6; //that callit
    pstTimer->ulCallBackArg7 = ulCallBackArg7; //that callit
    pstTimer->ulCallBackArg8 = ulCallBackArg8; //that callit
    pstTimer->ulTimeOut = ulTimeout / TIMER_GRANULARITY; //time out in tic

    pstTimer->ulWrapCounter = (pstTimer->ulTimeOut + g_ucHand) / CALENDAR_SIZE + g_ulClockWrap;
    pstTimer->ulCalIndex = (pstTimer->ulTimeOut + g_ucHand) % CALENDAR_SIZE;

    pstTimer->pstNext = g_pastCalendar[pstTimer->ulCalIndex];
    g_pastCalendar[pstTimer->ulCalIndex] = pstTimer;
    pstTimer->pstPrev = NULL;
    if(pstTimer->pstNext != NULL)
    {
        pstTimer->pstNext->pstPrev = pstTimer;
    }

    vosMutexGive(&g_pstOpTimerMutex);
    return 0;
}

/*******************************************************************************
*
* vosTimerCancel:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosTimerCancel(TIMER_OBJ_t *pstTimer)
{
    if (NULL == pstTimer)
    {
        return -1;
    }
   
    vosMutexTake(&g_pstOpTimerMutex);

    if (TMR_ST_FREE == pstTimer->stState) //incase got cancel more than one
    {
        vosMutexGive(&g_pstOpTimerMutex);
        return 0;
    }
    else if (TMR_ST_TAKEN == pstTimer->stState)
    {
        vosTimerRelease(pstTimer);
        vosMutexGive(&g_pstOpTimerMutex);
        return 0;
    }
   
    if (NULL == pstTimer->pstNext && 
        NULL == pstTimer->pstPrev)    //this is the unique node in the list
    {
        g_pastCalendar[pstTimer->ulCalIndex] = NULL;
    }

    if (pstTimer->pstPrev != NULL)
    {    //other than  head node
        pstTimer->pstPrev->pstNext = pstTimer->pstNext;
    }

    if (pstTimer->pstNext != NULL)
    {    //other than  tail node
        pstTimer->pstNext->pstPrev = pstTimer->pstPrev;
        if(pstTimer->pstPrev == NULL)
        {    // head node 
            g_pastCalendar[pstTimer->ulCalIndex] = pstTimer->pstNext;
        }
    }
    vosTimerRelease(pstTimer);
    vosMutexGive(&g_pstOpTimerMutex);
    return 0;
}

/*******************************************************************************
*
* vosTimerReSchedule:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
int vosTimerReSchedule(TIMER_OBJ_t *pstTimer, uint32 ulTimeout)
{
    uint8 tmpID;
    if (NULL == pstTimer)
    {
        return -1;
    }

    vosMutexTake(&g_pstOpTimerMutex);

    tmpID = pstTimer->ulCalIndex;

    if (TMR_ST_FREE == pstTimer->stState)
    {
        vosMutexGive(&g_pstOpTimerMutex);
        return 0;
    }
    else if(TMR_ST_RUNNING != pstTimer->stState)
    {
        vosMutexGive(&g_pstOpTimerMutex);
        return 0;
    }
   
    pstTimer->ulTimeOut = ulTimeout / TIMER_GRANULARITY;
    pstTimer->ulWrapCounter = (pstTimer->ulTimeOut + g_ucHand) / CALENDAR_SIZE + g_ulClockWrap;
    pstTimer->ulCalIndex = (pstTimer->ulTimeOut + g_ucHand) % CALENDAR_SIZE;

    if (pstTimer->pstNext == NULL && 
        pstTimer->pstPrev == NULL)    //this is the unique node in the list
    {
        g_pastCalendar[tmpID] = NULL;
    }

    if (pstTimer->pstPrev != NULL)
    {    //other than  head node
        pstTimer->pstPrev->pstNext = pstTimer->pstNext;
    }

    if (pstTimer->pstNext != NULL)
    {    //other than  tail node
        pstTimer->pstNext->pstPrev = pstTimer->pstPrev;
        if(pstTimer->pstPrev == NULL)
        {    // head node 
            g_pastCalendar[tmpID] = pstTimer->pstNext;
        }
    }

    pstTimer->pstNext = g_pastCalendar[pstTimer->ulCalIndex];
    g_pastCalendar[pstTimer->ulCalIndex] = pstTimer;
    pstTimer->pstPrev = NULL;

    if(pstTimer->pstNext != NULL)
    {
        pstTimer->pstNext->pstPrev = pstTimer;
    }

    vosMutexGive(&g_pstOpTimerMutex);

   return 0;
}

void vosTimerShow(int fd)
{
    char * pHeader = "TmrId   WrapCounter   Type   TmOut   Elapsed   Remaining\r\n"
                     "------ ------------- ------ ------- --------- ----------\r\n";

    char * pFormat = "%-6d %-13lu %-6d %-7d %-9d %-10d\r\n";

    TIMER_OBJ_t *pTmp;

    int i;

    vosPrintf(fd, pHeader);

    for (i = 0; i < CALENDAR_SIZE; i++)
    {
        if (g_pastCalendar[i] != NULL)
        {
            pTmp = g_pastCalendar[i];
            while(pTmp)
            {
                vosPrintf(fd, pFormat,
                    pTmp->ulCalIndex,
                    pTmp->ulWrapCounter,
                    pTmp->stType,
                    pTmp->ulTimeOut,
                    pTmp->ulTimeOut-(pTmp->ulWrapCounter * CALENDAR_SIZE + pTmp->ulCalIndex-g_ulClockWrap*CALENDAR_SIZE-g_ucHand),
                    (pTmp->ulWrapCounter*CALENDAR_SIZE+pTmp->ulCalIndex-g_ulClockWrap*CALENDAR_SIZE-g_ucHand));
            
                pTmp = pTmp->pstNext;

            }
        }
    }        
}


/*
 *Test driver
 * */
static int vosTimerTestCb(uint32,uint32);
static TIMER_OBJ_t * g_pstTimer1 = NULL;
static TIMER_OBJ_t * g_pstTimer2 = NULL;
static TIMER_OBJ_t * g_pstTimer3 = NULL;

/*vosTimerTestStart(): add 3 timer to it */
int vosTimerTestStart()
{
    struct timeval t;
    t.tv_sec = 2; 
    t.tv_usec = 0;

    /*if (NULL == g_pstTimer1)
    {
        g_pstTimer1 = vosTimerGetFree();
        if (g_pstTimer1)
        {
            vosTimerStart(g_pstTimer1, REOCURRING_TIMER, 3000, (FUNCPTR)vosTimerTestCb,98,98,3,4,5,6,7,8);
        }
        printf("1st event\n");
        printf("Now mHand is: %d\n", g_ucHand);
    }

    if (NULL == g_pstTimer2)
    {
        select(0,0,0,0,&t);
        g_pstTimer2 = vosTimerGetFree();  
        if (g_pstTimer2)
        {
            vosTimerStart(g_pstTimer2, ONE_SHOT_TIMER, 6000, (FUNCPTR)vosTimerTestCb,777,777,3,4,5,6,7,8);
        }
        printf("2nd event\n");
        printf("Now mHand is: %d\n", g_ucHand);
    }*/

    if (NULL == g_pstTimer3)
    {
        select(0,0,0,0,&t);
        g_pstTimer3 = vosTimerGetFree();
        if (g_pstTimer3)
        {
            vosTimerStart(g_pstTimer3, REOCURRING_TIMER, 2000, (FUNCPTR)vosTimerTestCb,3333,3333,3,4,5,6,7,8);
        }
        printf("3rd event\n");
        printf("Now mHand is: %d\n", g_ucHand);
    }

    return 0;
}

int vosTimerTestStop()
{
    vosTimerCancel(g_pstTimer1);
    g_pstTimer1 = NULL;
    
    vosTimerCancel(g_pstTimer2);
    g_pstTimer2 = NULL;
    
    vosTimerCancel(g_pstTimer3);
    g_pstTimer3 = NULL;

    return 0;
}

int vosTimerTestCb(uint32 m,uint32 n)
{
    static struct timeval t = {0, 0};
    static struct timezone tz = {0, 0};
    static int totalCounter = 0;
    static int counter = 0;
    struct timeval tmp;
    if (m==3333 && n==3333){
        if (counter == 0){
            gettimeofday(&t, &tz);
        }

        if(counter == 10){
            tmp = t;
            gettimeofday(&t, &tz);
            printf("Time segment between two calls: start %u:%u, end %u:%u, interval %ld ms\n",
                tmp.tv_sec,tmp.tv_usec,t.tv_sec,t.tv_usec, 
                ((long)((long)t.tv_usec - (long)tmp.tv_usec))/1000 + ((long)t.tv_sec - (long)tmp.tv_sec) * 1000);
            counter = 0;
        }

        counter++;
    }

    totalCounter++;

    printf("(FUNCPTR)vosTimerTestCb is called (%ld): %d, %d\n", totalCounter,m,n);

    return 0;
}

