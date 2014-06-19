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
* FILENAME:  vos_time.h
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_time.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_TIME_H_
#define __VOS_TIME_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
#include <vos_compiler.h>
#include <vos_types.h>

/*--------------------------Macro definition------------------------- */

#define SECOND_USEC			(1000000)

/*--------------------------type definition------------------------- */

typedef unsigned long VOS_TIME_t;
typedef UINT64 VOS_TIME_USECS_t;


/* Relative time in microseconds modulo 2^32 */
VOS_TIME_USECS_t vosTimeUsecsGet(void);

/* Absolute time in seconds ala Unix time, interrupt-safe */
VOS_TIME_t vosTimeGet(void);

void vosUDelay(uint32 usec);

#define	vosSleep(sec)	sleep(sec)
#define	vosUSleep(usec)		usleep(usec)


#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_TIME_H_ */

