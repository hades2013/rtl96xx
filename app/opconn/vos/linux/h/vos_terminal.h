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
* FILENAME:  vos_terminal.h
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_terminal.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_TERMINAL_H_
#define __VOS_TERMINAL_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
int vosTermSettingsGet(int fd, void *pSettings);
int vosTermSettingsSet(int fd, const void *pSettings);
int vosTermBackup(int fd);
int vosTermConfig(int fd);
int vosTermRestore(int fd);
 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_TERMINAL_H_ */
 
