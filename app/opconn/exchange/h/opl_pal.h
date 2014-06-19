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
* FILENAME:  opl_pal.h
*
* DESCRIPTION: 
*	this file contend the rx packet and tx packet api
*
* Date Created: Apr 30, 2008
*
* Authors(optional): zzhu
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/exchange/h/opl_pal.h#1 $
* $Log:$
*
*
**************************************************************************/
#ifndef __OPL_PAL_H__
#define __OPL_PAL_H__
#include <opl_type.h>

#define MAX_PACKET_LENGTH 1518
typedef struct PAL_FRAME_HEADER_s
{
    opuint8   sfd[4];
    opuint16  dalaLen;
    opuint8   payload[0];
}__attribute__((packed)) PAL_FRAME_HEADER_t;

typedef struct PAL_FRAME_TAILER_s{
    opuint16  crc;
    opuint8   efd[4];
}__attribute__((packed)) PAL_FRAME_TAILER_t;

opint32 palGetPktHeader(opuint8 *pHead,opuint32 len);


opint32 palGetPktBody(opuint8 *pData,opuint32 len);


opint32 palGetPktTail(opuint8 *pTail,opuint32 len);


opint32 palRxData(opuint8 *p,opuint16 *len);


opint32 palTxData(opuint8 *p,opuint16 len);


opint32 palInit(void);

#endif
