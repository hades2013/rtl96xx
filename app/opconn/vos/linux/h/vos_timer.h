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
* FILENAME:  vos_timer.h
*
* DESCRIPTION:
*   op_timer is designed to provide a protocol timing util, and it has a granularity of 10ms,
*   that is, it ticks every 500ms.
*
*   Timer is divided into 2 classes:
*       1.  One Shot timer.  when timer times out it will not re-occur again.
*       2.  Sticky Timer ( or Continuous Timer).  When it times out it will automatically restart again.
*
*   To start a timer, user need to get a timer object by calling
*       vosTimerGetFree();
*   next, user can call  vosTimerStart(...), opTimerCalcel(), or vosTimerReSchedule().  When timer times out,
*   the callback function is called under timer task's context.
*
* Date Created: Aug 06, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_timer.h#1 $
* $Log:$
*
*
**************************************************************************/

#ifndef __VOS_TIMER_H_
#define __VOS_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <vos_types.h>

typedef enum TIMER_TYPE_s {
    TMRT_UNKNOWN = 0,
    ONE_SHOT_TIMER = 1,
    REOCURRING_TIMER  = 2
}TIMER_TYPE_t;

typedef enum TIMER_STATE_s
{
    TMR_ST_FREE = 0,
    TMR_ST_TAKEN = 1,
    TMR_ST_RUNNING = 2
}TIMER_STATE_t;

enum { TIMER_GRANULARITY = 50 }; //granularity in ms

typedef struct TIMER_OBJ_s
{
    TIMER_STATE_t  stState;
    uint32         ulIndex;
    uint32         ulCalIndex;
    TIMER_TYPE_t   stType;
    uint32         ulTimeOut;
    struct TIMER_OBJ_s   *pstPrev;
    struct TIMER_OBJ_s   *pstNext; //to nested the timer in the linkedlist
    FUNCPTR        pstCallBackFunc;
    uint32         ulCallBackArg1;
    uint32         ulCallBackArg2;
    uint32         ulCallBackArg3;
    uint32         ulCallBackArg4;
    uint32         ulCallBackArg5;
    uint32         ulCallBackArg6;
    uint32         ulCallBackArg7;
    uint32         ulCallBackArg8;
    UINT64         ulWrapCounter; //in which wraparound the callback function will be called
}TIMER_OBJ_t;

int vosTimerInit();

int vosTimerShutdown();

TIMER_OBJ_t * vosTimerGetFree(void);

int vosTimerRelease(TIMER_OBJ_t *pstTimer);

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
    uint32 ulCallBackArg8);

int vosTimerCancel(TIMER_OBJ_t *pstTimer);

int vosTimerReSchedule(TIMER_OBJ_t *pstTimer, uint32 ulTimeout);

void vosTimerShow(int fd);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __VOS_TIMER_H_ */

