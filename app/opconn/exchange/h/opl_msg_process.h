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
* FILENAME:  opl_msg_process.h
*
* DESCRIPTION: 
*	msg process,rx packet,tx packet and hooks registered
*
* Date Created: Apr 30, 2008
*
* Authors(optional): zzhu
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/exchange/h/opl_msg_process.h#1 $
* $Log:$
*
*
**************************************************************************/
#ifndef __OPL_MSG_PROCESS_H__
#define __OPL_MSG_PROCESS_H__
#include <opl_type.h>
#include <opl_msg_header.h>

opint32 msgGetAckProcessHandler(opuint8 onu_id,opuint8 *pData,opuint32 len);


opint32 msgBuffDataRxClear(MSG_DATA_BUFF_RX_CHAIN_t *p);


opint32 msgBuffDataTxClear(MSG_DATA_BUFF_TX_CHAIN_t *p);


opint32 msgBuffChainRxInit(void);


opint32 msgBuffChainTxInit(void);


opint32 msgDataPorcessHander(opuint8 *pData,opuint32 len);


opint32 msgDataProcessComplete(void);


opint32 msgTxHandler(opuint8 *payLoad,opuint32 len);


void msgProcessThread(void);


void msgRxPktThread(void);


void msgTxPktThread(void);

opint32 msgInit(void);

opint32 msgExit(void);

#ifdef using_sem_for_send
void TimerThread(void);
#else
#endif

opint32 msgSendToTxQueue(opuint8 onuId,opuint16 eventId,opuint8 *payLoad,opuint32 len);


void msgDebugThread(void);


opint32 msgHookRegister(void);


opint32 msgInit(void);

#endif
