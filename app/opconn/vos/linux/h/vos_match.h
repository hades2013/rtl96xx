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
* FILENAME:  vos_match.h
*
* DESCRIPTION: 
*	
*
* Date Created: Jul 22, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_match.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_MATCH_H_
#define __VOS_MATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <sys/types.h>
 
int vosMatch(char *string, char *pattern);
 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_MATCH_H_ */

