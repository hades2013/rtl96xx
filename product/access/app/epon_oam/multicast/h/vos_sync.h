
 
#ifndef __VOS_SYNC_H_
#define __VOS_SYNC_H_
 
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
//#include <vos_sync.h>
//#include <vos_libc.h>

#ifdef __cplusplus
extern "C" {
#endif /*--------------------------Macro definition------------------------- */

#define VOS_MUTEX_FOREVER	(-1)
#define VOS_MUTEX_NOWAIT	0

#define VOS_SEM_FOREVER		(-1)
#define VOS_SEM_BINARY		1
#define VOS_SEM_COUNTING	0

#define  VOS_MUTEX_OK    0
#define  VOS_SEM_OK        0

#define VOS_MUTEX_t  pthread_mutex_t
#define VOS_SEM_t  sem_t

int vosMutexCreate(pthread_mutex_t *mutex);
#define  vosMutexDestroy(mutex)    pthread_mutex_destroy(mutex)
#define  vosMutexTake(mutex)     pthread_mutex_lock(mutex)
#define  vosMutexGive(mutex)     pthread_mutex_unlock(mutex)

int vosSemCreate(sem_t *sem, int shared, int initial_count);
#define  vosSemDestroy(sem)   sem_destroy(sem)
#define  vosSemTake(sem)    sem_wait(sem)
#define  vosSemGive(sem)    sem_post(sem)
 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_SYNC_H_ */
 
