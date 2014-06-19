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
* FILENAME:  vos_time.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/src/vos_time.c#1 $
* $Log:$
*
*
**************************************************************************/

//#include <vos_compiler.h>
#include <vos_time.h>
#include <time.h> 
#include <sys/time.h>
#include <sys/types.h>
#include <lw_type.h>


/*******************************************************************************
*
* vosTimeUsecsGet:	Get time in microseconds modulo 2^32 
*
* DESCRIPTION:
*   The precision is limited to the Unix clock period (typically 10000 usec.)
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	Returns the relative time in microseconds modulo 2^32. 
*
* SEE ALSO: 
*/
VOS_TIME_USECS_t vosTimeUsecsGet(void)
{
    struct timespec times;
    clock_gettime(CLOCK_REALTIME, &times);
    return (((UINT64)times.tv_sec) * SECOND_USEC + times.tv_nsec/1000);

}
    	
/*******************************************************************************
*
* vosTimeGet:	 Get time in seconds
*
* DESCRIPTION:
* 	
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	Return the current time in seconds since 00:00, Jan 1, 1970.
*
* SEE ALSO: 
*/
VOS_TIME_t vosTimeGet(void)
{
    return (VOS_TIME_t) time(0);
}

/*******************************************************************************
*
* vosUDelay:	 Spin wait for an approximate number of microseconds
*
* DESCRIPTION:
*      The current implementation assumes that clock_gettime has
*      system tick resolution (or better).
*
* INPUTS:
*	usec - number of microseconds
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void vosUDelay(UINT32 usec)
{
	unsigned long long end=0;
	struct timespec tp;

	clock_gettime(CLOCK_REALTIME,&tp);
	unsigned long long start=end=tp.tv_sec*1000000 + tp.tv_nsec/1000;

	while(end  - start < usec){
	        clock_gettime(CLOCK_REALTIME,&tp);
	        end=tp.tv_sec*1000000 + tp.tv_nsec/1000;
	}
}

