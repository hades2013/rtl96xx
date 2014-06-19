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
* FILENAME:  vos_thread.h
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_thread.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_THREAD_H_
#define __VOS_THREAD_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
//#include <vos_types.h>

/*--------------------------Macro definition------------------------- */

#define	OP_VOS_THREAD_STKSZ	65536	/* Default Stack Size */


/*--------------------------type definition------------------------- */
#define	VOS_THREAD_t	pthread_t

int vosThreadInit(void);
VOS_THREAD_t vosThreadCreate(char *name, int stackSize, int pri, void *(*func)(void *), void *arg);
int vosThreadDestroy(VOS_THREAD_t);
VOS_THREAD_t vosThreadSelf(void);
void vosThreadExit(int rc);
void vosThreadMainSet(VOS_THREAD_t thread);
VOS_THREAD_t vosThreadMainGet(void);
int vosThreadVerify(VOS_THREAD_t thread);
void vosThreadShow(int fd);

 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_THREAD_H_ */
 
