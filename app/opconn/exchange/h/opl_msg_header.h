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
* FILENAME:  opl_msg_header.h
*
* DESCRIPTION: 
*	define the msg struct and other msg info
*
* Date Created: Apr 30, 2008
*
* Authors(optional): zzhu
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/exchange/h/opl_msg_header.h#1 $
* $Log:$
*
*
**************************************************************************/
#ifndef __OPL_MSG_HEADER_H__
#define __OPL_MSG_HEADER_H__

#include <opl_type.h>
#include <opl_pal.h>

#define MSG_DATA_BUFF_RX_LEN           1500
#define MSG_DATA_BUFF_TX_LEN           1500

#define MAX_GET_INFO_LENGTH            1500 

#define MAX_PALOAD_LENGTH              (MSG_DATA_BUFF_RX_LEN-sizeof(opuint8)-sizeof(opuint16))

#define UPLOAD_IMAGE_START              0xff
#define UPLOAD_IMAGE_END                0xffff

#define MAX_IMAGE_LOAD                  1024*1024
typedef opint32 (*PFUNCTION)();

typedef enum MSG_TYPE_s{
    MSG_DYING_GSP = 0,
    MSG_LINK_FAULT,
    MSG_AUNTHENTIED,
    MSG_NACK_EVEN,
    MSG_OAM_RECEIVED,
    MSG_SET,
    MSG_SET_ACK,
    MSG_GET,
    MSG_GET_ACK,
    MSG_IMAGE_UPLOAD,
    MSG_IMAGE_UPLOAD_ACK,
    MSG_TYPE_END
}MSG_TYPE_e;

typedef struct MSG_HEAD_s{
    opuint8   onuId;
    opuint16  eventId;
}__attribute__((packed)) MSG_HEAD_t;
typedef struct FRAME_FORMAT_s{
    MSG_HEAD_t msghead;
    opuint8    payLoad[0];
}__attribute__((packed)) FRAME_FORMAT_t;

typedef struct MSG_DATA_BUFF_RX_CHAIN_s{
    opuint8       active;
    opuint16      dataLen;
    opuint8       dataBuff[MSG_DATA_BUFF_RX_LEN];
    PFUNCTION   msgHandler;
    PFUNCTION   msgHandlerComplete;
    struct      MSG_DATA_BUFF_RX_CHAIN_s *next;
}MSG_DATA_BUFF_RX_CHAIN_t;

typedef struct MSG_DATA_BUFF_TX_CHAIN_s
{
    opuint8       active;
    opuint16      dataLen;
    opuint8       dataBuff[MSG_DATA_BUFF_TX_LEN];
    PFUNCTION   msgTxHandler;
    PFUNCTION   msgTxComplete;
    struct      MSG_DATA_BUFF_TX_CHAIN_s *next;
}MSG_DATA_BUFF_TX_CHAIN_t;

typedef struct MSG_DYING_GSP_PACK_FORMAT_s
{
    
}MSG_DYING_GSP_PACK_FORMAT_t;

typedef struct MSG_LINK_FAULT_PACK_FORMAT_s
{
    
}MSG_LINK_FAULT_PACK_FORMAT_t;

typedef struct MSG_AUTHENTITED_PACK_FORMAT_s
{

}MSG_AUTHENTITED_PACK_FORMAT_t;

typedef struct MSG_NACK_PACK_FORMAT_s
{
    
}MSG_NACK_PACK_FORMAT_t;

typedef struct MSG_OAM_RCV_FORMAT_s
{

}MSG_OAM_RCV_FORMAT_t;

typedef struct MSG_SET_PACK_FORMAT_s
{
    
}MSG_SET_PACK_FORMAT_t;

typedef struct MSG_GET_PACK_FORMAT_s
{
    
}MSG_GET_PACK_FORMAT_t;

typedef struct MSG_IMAGE_UPLOAD_FORMAT_s
{

}MSG_IMAGE_UPLOAD_FORMAT_t;

#define NUM_OF_DATA_BUFF_CHAIN_RX      16
#define NUM_OF_DATA_BUFF_CHAIN_TX      16

#define lockRxMsgBuffDataChain()
#define unlockRxMsgBuffDataChain()
#define lockTxMsgBuffDataChain()
#define unlockTxMsgBuffDataChain()

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


opint32 msgSendToTxQueue(opuint8 onuId,opuint16 eventId,opuint8 *payLoad,opuint32 len);


#endif
