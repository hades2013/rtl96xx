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
* FILENAME:  vos_msq.h
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_msq.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_MSQ_H_
#define __VOS_MSQ_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
#include <vos_types.h>

int vosMsqInit(char * pcPath);
void vosMsqDestroy(void);
int vosMsqCreate(int maxMsgLength);
int vosMsqSend(int msqid, void *msgBuf, uint32 msgsz);
uint32 vosMsqReceive(int msqid, void *msgBuf, uint32 msgsz);
int vosMsqDelete(int msqid);
 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_MSQ_H_ */
 
