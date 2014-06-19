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
* FILENAME:  vos_alloc.h
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_alloc.h#1 $
* $Log:$
*
*
**************************************************************************/
#ifndef __VOS_ALLOC_H_
#define __VOS_ALLOC_H_

/*************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <stdlib.h>
#include<malloc.h>

/*******************************************************************************
*
* vosAlloc:	 Allocate general purpose system memory.
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	size - size of memory block to allocate.
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	Pointer to memory block
*
* SEE ALSO: 
*/
#define  vosAlloc(size)   malloc(size)

/*******************************************************************************
*
* vosFree:	 Free memory block allocate by vosAlloc 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	pMemBlock - Previously allocated memory block to be freed
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
#define  vosFree(pMemBlock)   free(pMemBlock)

/*******************************************************************************
*
* vosReAlloc:	 Reallocate general purpose system memory.
*
* DESCRIPTION:
* 	
*
* INPUTS:
*   pMemBlock - Pointer to previously allocated memory block
*   size - size of memory block to allocate.
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	Pointer to memory block
*
* SEE ALSO: 
*/
#define  vosReAlloc(pMemBlock,size)   realloc(pMemBlock,size)


#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_ALLOC_H_ */
 
 
