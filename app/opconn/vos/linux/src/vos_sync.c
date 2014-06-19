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
* FILENAME:  vos_sync.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos_sync.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>

#include <vos_alloc.h>
#include <vos_thread.h>
#include <vos_sync.h>
#include <vos_libc.h>


int vosMutexCreate(pthread_mutex_t *mutex)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    attr.__mutexkind = PTHREAD_MUTEX_RECURSIVE_NP;
    return pthread_mutex_init(mutex, &attr);
}

int vosSemCreate(sem_t *sem, int shared, int initial_count)
{
    return sem_init(sem, shared, initial_count);
}


