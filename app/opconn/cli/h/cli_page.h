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
* FILENAME:  cli_page.h
*
* DESCRIPTION: 
*	To surport showing in more screen.
*
* Date Created: Aug 12, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/h/cli_page.h#1 $
* $Log:$
*
*
**************************************************************************/

#ifndef __CLI_PAGE_H__
#define __CLI_PAGE_H__

/* define page memory size */ 
#define PAGE_MEMORY  524288 /* 512 KB */   

extern void cliPrintInMorePage(ENV_t *pstEnv, char *pMessage);
extern void cliShowStart(ENV_t *pstEnv);
extern void cliShowEnd(ENV_t *pstEnv);
extern void cliShowFree(ENV_t *pstEnv);

#endif/*__CLI_PAGE_H__*/

