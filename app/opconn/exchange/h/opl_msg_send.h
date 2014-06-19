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
* FILENAME:  opl_msg_send.h
*
* DESCRIPTION: 
*	msg send api
*
* Date Created: Apr 30, 2008
*
* Authors(optional): zzhu
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/exchange/h/opl_msg_send.h#1 $
* $Log:$
*
*
**************************************************************************/
#ifndef __OPL_MSG_SEND_H__
#define __OPL_MSG_SEND_H__
#include <opl_type.h>
#include <opl_msg_header.h>
#ifdef _VXWORKS_
#else
#include <pthread.h>
#endif
opint32 msgOamPackAndSend(opuint8 onuId,opuint8 *p,opuint32 len);


opint32 msgSetReqPackAndSend(opuint8 onuId,opuint8 *p,opuint32 len);


opint32 msgSetAckPackAndSend(opuint8 onuId,opuint8 *p,opuint32 len);


opint32 msgGetReqPackAndSend(opuint8 onuId,opuint8 *p,opuint32 len,opuint8 *pR);


opint32 msgGetAckPackAndSend(opuint8 onuId,opuint8 *p,opuint32 len);

opint32 msgImageUploadPackAndSend(opuint8 onuId,opuint8 *p,opuint32 len);

#endif

