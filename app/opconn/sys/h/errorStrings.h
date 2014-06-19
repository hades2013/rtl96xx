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
* FILENAME:  errorStrings.h
*
* DESCRIPTION: 
*	
*
* Date Created: Apr 23, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/h/errorStrings.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __ERRORSTRINGS_H_
#define __ERRORSTRINGS_H_

#include "defs.h" 
#include "errors.h"

#ifdef __cplusplus
extern "C" {
#endif

STATUS odmErrorStringInit();
STATUS odmErrorStringDestroy();
STATUS odmErrorStringInsert(
    UINT32 ulErrCode, 
    const char *pcErrString, 
    const char *pcErrCodeString);
UINT32 odmErrorStringDelete(UINT32 ulErrCode);
const char * odmErrorStringGet(UINT32 ulErrCode);
const char * odmErrorCodeStringGet(UINT32 ulErrCode);

#define INSERT_ERROR(ERROR_CODE, ERROR_STRING)                   \
    odmErrorStringInsert((UINT32)ERROR_CODE, (char*)ERROR_STRING, (char*)#ERROR_CODE)

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __ERRORSTRINGS_H_ */

