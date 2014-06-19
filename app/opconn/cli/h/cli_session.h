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
* FILENAME:  cli_session.h
*
* DESCRIPTION: 
*	
*
* Date Created: May 07, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/h/cli_session.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __CLI_SESSION_H_
#define __CLI_SESSION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "defs.h"
#include "cli_engine.h"
 
UINT32 cliSessionInit(void);

UINT32 cliSessionDestroy(void);

UINT32 cliSessionCount(void);

ENV_t * cliSessionGet(VOS_THREAD_t tThreadId);

ENV_t * cliSessionGetByIndex(UINT16 usIndex);

ENV_t * cliSessionAdd(VOS_THREAD_t tThreadId, UINT8 ucIsConsole);

UINT32 cliSessionDelete(VOS_THREAD_t tThreadId);

UINT32 cliSessionClose(ENV_t *pstEnv);

UINT32 cliSessionCloseByIndex(ENV_t *pstCurrentEnv, UINT16 usIndex);

HASH_TABLE_t * cliSessionTableGet(void);

void cliSessionShow(int fd);

UINT8 cliSessionFreeIndexGet();

STATUS cliSessionIndexRelease(UINT8 ucIndex);

STATUS cliSessionDebugLevelSet(UINT8 ucIndex, UINT8 ucModule, UINT8 ucLevel);

UINT8 cliSessionDebugLevelGet(UINT8 ucIndex, UINT8 ucModule);

STATUS cliSessionDebugLevelCheck(UINT8 ucIndex, UINT8 ucModule, UINT8 ucLevel);

STATUS cliSessionGlobalDebugLevelCheck(UINT8 ucModule, UINT8 ucLevel);

#ifdef __cplusplus
}
#endif
  
#endif /* #ifndef __CLI_SESSION_H_ */

