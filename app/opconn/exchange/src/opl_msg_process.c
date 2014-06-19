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
* FILENAME:  opl_msg_process.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/exchange/src/opl_msg_process.c#1 $
* $Log:$
*
*
**************************************************************************/
#include <opl_type.h>
#include <opl_pal.h>
#include <opl_msg_header.h>
#include <semaphore.h>
#include <opl_api.h>
#include "oam.h"
#include <vos.h>
sem_t txpkt;
sem_t rxpkt;
sem_t msgpro;
sem_t getack;

#ifdef _VXWORKS_
SEM_ID getAckMutex;
int    getAckPipefd;
char Ackpipe[] = "/pipe/getAckPipe";
#else
opint32 msgGetPipeFd[2] = {0x0,0x0};
#endif

MSG_DATA_BUFF_RX_CHAIN_t msgDataBuffRxChain[NUM_OF_DATA_BUFF_CHAIN_RX];
MSG_DATA_BUFF_RX_CHAIN_t *pRxMsgHeader = NULL;
MSG_DATA_BUFF_RX_CHAIN_t *pRxMsgTailer = NULL;

MSG_DATA_BUFF_TX_CHAIN_t msgDataBuffTxChain[NUM_OF_DATA_BUFF_CHAIN_TX];
MSG_DATA_BUFF_TX_CHAIN_t *pTxMsgHeader = NULL;
MSG_DATA_BUFF_TX_CHAIN_t *pTxMsgTailer = NULL;

int eventdbg[20] = {0};
sem_t msgGetSem;
volatile opint32 msgGetTimerflag = 0;
volatile opint32 msgGetAckflag   = 0;

int imagefd = 0;
int seq = 0xff;
volatile  int imageflag = FALSE;
volatile  int uploadack = UPLOAD_ERROR;
volatile  int uart1_comm_enable = opl_false;

opint32 (*pMsgDyingGaspProcessHandler)(opuint8 onuId,opuint8 *pData,opuint32 len) = NULL;

opint32 (*pMsgLinkFaultEventProcessHandler)(opuint8 onuId,opuint8 *pData,opuint32 len) = NULL;

opint32 (*pMsgAuthentitedProcessHandler)(opuint8 onuId,opuint8 *pData,opuint32 len) = NULL;

opint32 (*pMsgOamRcvProcessHandler)(opuint8 onuId,opuint8 *pData,opuint32 len) = NULL;

opint32 (*pMsgSetProcessHandler)(opuint8 onuId,opuint8 *pData,opuint32 len) = NULL;

opint32 (*pMsgSetAckProcessHandler)(opuint8 onuId,opuint8 *pData,opuint32 len) = NULL;

opint32 (*pMsgGetProcessHandler)(opuint8 onuId,opuint8 *pData,opuint32 len) = NULL;

opint32 (*pMsgGetAckProcessHandler)(opuint8 onuId,opuint8 *pData,opuint32 len) = NULL;

opint32 (*pMsgImageUploadProcessHandler)(opuint8 onuId,opuint8 *pData,opuint32 len) = NULL;
opint32 (*pMsgImageUploadAckProcessHandler)(opuint8 onuId,opuint8 *pData,opuint32 len) = NULL;

opint32 (*pMsgNackProcessHandler)(opuint8 onuId,opuint8 *pData,opuint32 len) = NULL;

extern u8_t	out_data[OAM_MAXIMUM_PDU_SIZE+4];


opint32 msgDyingGasgProcessHandler(opuint8 onuId,opuint8 *pData,opuint32 len)
{
    return opl_ok;
}

opint32 msgAuthenticatedProcessHandler(opuint8 onuId,opuint8 *pData,opuint32 len)
{
    return opl_ok;
}

opint32 msgLinkFaultEventProcessHandler(opuint8 onuId,opuint8 *pData,opuint32 len)
{
    return opl_ok;
}
/*******************************************************************************
* msgOamRcvProcessHandler
*
* DESCRIPTION:
* 
*	this function is the call back handle when receive oam message.

* RETURNS:
* opl_ok if the access is success.
* opl_error if the access is failed.
* SEE ALSO: 
*/
opint32 msgOamRcvProcessHandler(opuint8 onuId,opuint8 *pData,opuint32 len)
{
    opuint8  ext_code;
    opuint16 size;
    opl_sdk_printf(("opcode:%d\n",*pData));
    OPL_UNPACK_UINT8(pData,ext_code);
    eopl_ctc_header(ext_code);
    size = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
    opl_sdk_memcpy(&out_data[size],pData,len-1);
    size += (len - 1);
    if(size < OAM_MINIMIZE_PDU_SIZE)
 	{
		oam_pdu_padding(&out_data[size],  OAM_MINIMIZE_PDU_SIZE - size);
		size = OAM_MINIMIZE_PDU_SIZE;
 	}
 	/*send to olt*/
    OamFrameSend(out_data,size);
    return opl_ok;
}
/*******************************************************************************
* msgSetProcessHandler
*
* DESCRIPTION:
* 
*	this function is the call back handle when receive set message.

* RETURNS:
* opl_ok if the access is success.
* opl_error if the access is failed.
* SEE ALSO: 
*/
opint32 msgSetProcessHandler(opuint8 onuId,opuint8 *pData,opuint32 len)
{
    opint32 iRet = opl_ok;

    if(NULL == pData)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    /*depacket set message*/
    iRet = AdmDepacket(onuId,pData);

    return iRet;
}

opint32 msgSetAckProcessHandler(opuint8 onuId,opuint8 *pData,opuint32 len)
{
    return opl_ok;
}
/*******************************************************************************
* msgGetProcessHandler
*
* DESCRIPTION:
* 
*	this function is the call back handle when receive get message.

* RETURNS:
* opl_ok if the access is success.
* opl_error if the access is failed.
* SEE ALSO: 
*/
opint32 msgGetProcessHandler(opuint8 onuId,opuint8 *pData,opuint32 len)
{
    opint32 iRet = opl_ok;

    if(NULL == pData)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    /*depacket get message*/
    iRet = AdmDepacket(onuId,pData);

    return iRet;
}
/*******************************************************************************
* msgGetAckProcessHandler
*
* DESCRIPTION:
* 
*	this function is the call back handle when receive get ack message.

* RETURNS:
* opl_ok if the access is success.
* opl_error if the access is failed.
* SEE ALSO: 
*/
opint32 msgGetAckProcessHandler(opuint8 onuId,opuint8 *pData,opuint32 len)
{
    opint32 iRet = opl_ok;
    
#ifdef using_sem_for_send
    
#else
    extern opuint32 sendGetReq;
#endif

    opl_dbg(("%s,%d.\n",__FUNCTION__,__LINE__));
    /*write get ack message to waiting process by pipe*/
#ifdef _VXWORKS_
    write(getAckPipefd,pData,len);
#else
    write(msgGetPipeFd[1],pData,len);
#endif
    msgGetAckflag   = 1;
    opl_dbg(("post.\n"));

    /*notify the waiting process*/   
#ifdef using_sem_for_send    
    msgGetTimerflag = 0;
    sem_post(&msgGetSem);
#else
    sendGetReq++;
#endif
    return opl_ok;
}
/*******************************************************************************
* msgImageUploadProcessHandler
*
* DESCRIPTION:
* 
*	this function is the call back handle when receive image upload message.

* RETURNS:
* opl_ok if the access is success.
* opl_error if the access is failed.
* SEE ALSO: 
*/
opint32 msgImageUploadProcessHandler(opuint8 onuId,opuint8 *pData,opuint32 len)
{
    opuint16 sequence;
    char     image[255];
    opuint8  imname[20] = {0};
    opuint32 count      = 0;
    opuint16 crc        = 0;
    opuint16 checksum   = 0;
    opuint8  *buffer    = NULL;
    opuint8  *name;
    opuint8  bootFlag;

    /*opl_dump_data((pData),(len),(16));*/
    OPL_UNPACK_UINT16(pData,sequence);
    opl_sdk_printf(("seq = 0x%0x sequence = 0x%0x \n",seq,sequence));
    seq++;

    /*upload start*/
    if (UPLOAD_IMAGE_START == sequence)
    {
        imageflag = TRUE;
        name = pData;
        /*memcpy(imname,name,20);*/
        /*temporary directory*/
        /*sprintf(image,"/tmp/""%s",imname);*/
        sprintf(image,"/tmp/opconn.img.bak.uart");
        opl_sdk_printf(("%s\n",image));
        /*delete backup image*/
        unlink(image);
        if (imagefd > 0)
            close(imagefd);
        imagefd = open(image,O_RDWR|O_CREAT,0); 
        if(imagefd < 0)
        {
            opl_sdk_printf(("open uart file failed.\n"));
            return -1;
        } 

        opl_sdk_printf(("star time = %d \n",time(0)));
    }
    /*upload end*/
    else if(UPLOAD_IMAGE_END == sequence)
    {
    
        OPL_UNPACK_UINT16(pData,crc);
        buffer = (opuint8 *)vosAlloc(MAX_IMAGE_LOAD);
        if (buffer)
        {
            opl_sdk_memset(buffer,0x0,MAX_IMAGE_LOAD);
            opl_sdk_printf(("crc = 0x%0x \n",crc));
            lseek(imagefd,0,SEEK_SET);
            count = read(imagefd,buffer,MAX_IMAGE_LOAD);
            oplCRC16BitShift(buffer,count,&checksum);
            opl_sdk_printf(("count = %d checksum = 0x%0x \n",count,checksum));
            if (crc == checksum)
            {
                opl_sdk_printf(("successful time = %d \n",time(0)));
                /*send upload ack*/
                imname[0] = UPLOAD_OK;
                msgImageUploadAckPackAndSend(onuId,imname,1);
                /*copy image to flash*/
                bootFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_NORMAL);
                if (FLASH_BOOT_OS2_FLAG == bootFlag)
                {
                    vosSystem("mtd erase "FLASH_DEV_NAME_OS1);
					vosSystem("mtd write /tmp/opconn.img.bak.uart "FLASH_DEV_NAME_OS1);
                    vosSystem("rm -f /tmp/opconn.img.bak.uart ");
					vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS1_FLAG);
                }
                else {
                    vosSystem("mtd erase "FLASH_DEV_NAME_OS2);
                    vosSystem("mtd write /tmp/opconn.img.bak.uart "FLASH_DEV_NAME_OS2);
                    vosSystem("rm -f /tmp/opconn.img.bak.uart ");
					vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS2_FLAG);
                }
                /*system reboot*/
                /*odmSysReset();*/
            }
            else
            {
                /*remove image*/
                /*send error*/
                imname[0] = UPLOAD_ERROR;
                msgImageUploadAckPackAndSend(onuId,imname,1);
            }
            vosFree(buffer);
        }
        if (imagefd > 0)
            close(imagefd);
        imageflag = FALSE;
       
    }
    /*upload image data*/
    else
    {
        if (imagefd > 0)
        {
            count = write(imagefd,pData,(len-2));

            if(count !=  (len-2))
            {
                opl_sdk_printf(("write only %d bytes,but %d bytes.\n",count,(len-2)));
                return opl_error;
            }
        }
        else return opl_error;
    }

    return opl_ok;
}
opuint32 msgImageUploadAckProcessHandler(opuint8 onuId,opuint8 *pData,opuint32 len)
{
    OPL_UNPACK_UINT8(pData,uploadack);
    return opl_ok;
}
opint32 msgBuffDataRxClear(MSG_DATA_BUFF_RX_CHAIN_t *p)
{
    if(NULL == p)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }

    opl_sdk_memset(p->dataBuff,0x00,MSG_DATA_BUFF_RX_LEN);
    p->active = opl_false;

    return opl_ok;
}

opint32 msgBuffDataTxClear(MSG_DATA_BUFF_TX_CHAIN_t *p)
{
    if(NULL == p)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }

    opl_sdk_memset(p->dataBuff,0x00,MSG_DATA_BUFF_TX_LEN);
    p->active = opl_false;

    return opl_ok;
}

opint32 msgBuffChainRxInit(void)
{
    opuint32 index;

    opl_sdk_memset(&msgDataBuffRxChain[0],0x00,sizeof(MSG_DATA_BUFF_TX_CHAIN_t)*NUM_OF_DATA_BUFF_CHAIN_RX);

    for(index = 0 ;index < NUM_OF_DATA_BUFF_CHAIN_RX; index++)
    {
        msgDataBuffRxChain[index].next = &msgDataBuffRxChain[(index+1)%NUM_OF_DATA_BUFF_CHAIN_RX];
        msgDataBuffRxChain[index].msgHandler = msgDataPorcessHander;
        msgDataBuffRxChain[index].msgHandlerComplete = msgDataProcessComplete;
    } 

    pRxMsgHeader = &msgDataBuffRxChain[0];
    pRxMsgTailer = pRxMsgHeader;

    return opl_ok;
}

opint32 msgBuffChainTxInit(void)
{
    opuint32 index;

    opl_sdk_memset(&msgDataBuffTxChain[0],0x00,sizeof(MSG_DATA_BUFF_TX_CHAIN_t)*NUM_OF_DATA_BUFF_CHAIN_TX);

    for(index = 0 ;index < NUM_OF_DATA_BUFF_CHAIN_TX; index++)
    {
        msgDataBuffTxChain[index].next = &msgDataBuffTxChain[(index+1)%NUM_OF_DATA_BUFF_CHAIN_TX];
        msgDataBuffTxChain[index].msgTxHandler = msgTxHandler;
        msgDataBuffTxChain[index].msgTxComplete = NULL;
    } 

    pTxMsgHeader = &msgDataBuffTxChain[0];
    pTxMsgTailer = pTxMsgHeader;

    return opl_ok;
}

opint32 msgDataPorcessHander(opuint8 *pData,opuint32 len)
{
    opint32 iRet = opl_ok;
    opuint8 onuId;
    opuint16 eventId;
    opuint8 *pBuff = pData;
        
    if(NULL == pData)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }

    /*opl_dump_data((pData),(len),(16));*/

    OPL_UNPACK_UINT8(pBuff,onuId);
    OPL_UNPACK_UINT16(pBuff,eventId);
    
    switch(eventId)
    {
        case MSG_DYING_GSP:
            opl_sdk_printf(("rcv dying gasp.\n"));
            if(NULL != pMsgDyingGaspProcessHandler)
            {
                iRet = pMsgDyingGaspProcessHandler(onuId,pBuff,len - 3);    
            }else
            {
                opl_sdk_printf(("no reigster handler.\n"));
                return opl_error;
            }
            break;
        case MSG_LINK_FAULT:
            if(NULL != pMsgLinkFaultEventProcessHandler)
            {
                iRet = pMsgLinkFaultEventProcessHandler(onuId,pBuff,len - 3);   
            }else
            {
                opl_sdk_printf(("no reigster handler.\n"));
                return opl_error;
            }
            
            opl_sdk_printf(("rcv link fault.\n"));
            break;
        case MSG_AUNTHENTIED:
            if(NULL != pMsgAuthentitedProcessHandler)
            {
                iRet = pMsgAuthentitedProcessHandler(onuId,pBuff,len - 3);   
            }else
            {
                opl_sdk_printf(("no reigster handler.\n"));
                return opl_error;
            }
            opl_sdk_printf(("rcv authentied.\n"));
            
            break;
        case MSG_OAM_RECEIVED:
            if(NULL != pMsgOamRcvProcessHandler)
            {
                iRet = pMsgOamRcvProcessHandler(onuId,pBuff,len - 3);
            }else
            {
                opl_sdk_printf(("no reigster handler.\n"));
                return opl_error;
            }
            
            opl_sdk_printf(("rcv oam received.\n"));
            break;
        case MSG_SET:
            if(NULL != pMsgSetProcessHandler)
            {
                iRet = pMsgSetProcessHandler(onuId,pBuff,len - 3);    
            }else
            {
                opl_sdk_printf(("no reigster handler.\n"));
                return opl_error;
            }
            
            opl_sdk_printf(("rcv set request.\n"));
            break;
        case MSG_SET_ACK:
            if(NULL != pMsgSetAckProcessHandler)
            {
                iRet = pMsgSetAckProcessHandler(onuId,pBuff,len - 3);  
            }else
            {
                opl_sdk_printf(("no reigster handler.\n"));
                return opl_error;
            }
            
            opl_sdk_printf(("rcv set ack.\n"));
            break;
        case MSG_GET:
            if(NULL != pMsgGetProcessHandler)
            {
                iRet = pMsgGetProcessHandler(onuId,pBuff,len - 3);  
            }else
            {
                opl_sdk_printf(("no reigster handler.\n"));
                return opl_error;
            }
            
            opl_sdk_printf(("rcv get requeset.\n"));
            break;
        case MSG_GET_ACK:
            if(NULL != pMsgGetAckProcessHandler)
            {
                iRet = pMsgGetAckProcessHandler(onuId,pBuff,len - 3);  
            }else
            {
                opl_sdk_printf(("no reigster handler.\n"));
                return opl_error;
            }
            opl_dbg(("rcv get ack.\n"));
            break;
        case MSG_IMAGE_UPLOAD:
            if(NULL != pMsgImageUploadProcessHandler )
            {
                iRet = pMsgImageUploadProcessHandler(onuId,pBuff,len - 3);    
            }else
            {
                opl_sdk_printf(("no reigster handler.\n"));
                return opl_error;
            }
            opl_sdk_printf(("rcv image upload.\n"));
            break;
         case MSG_IMAGE_UPLOAD_ACK:
            if(NULL != pMsgImageUploadAckProcessHandler )
            {
                iRet = pMsgImageUploadAckProcessHandler(onuId,pBuff,len - 3);    
            }else
            {
                opl_sdk_printf(("no reigster handler.\n"));
                return opl_error;
            }
            opl_sdk_printf(("rcv image upload ack.\n"));
            break;
        case MSG_NACK_EVEN:
            if(NULL != pMsgNackProcessHandler)
            {
                iRet = pMsgNackProcessHandler(onuId,pBuff,len - 3);    
            }else
            {
                opl_sdk_printf(("no reigster handler.\n"));
                return opl_error;
            }
            opl_sdk_printf(("rcv image upload.\n"));
            break;
        default:
            opl_sdk_printf(("unknow event.\n"));
            return opl_ok;
            break;
    }
    
    return opl_ok;
}

opint32 msgDataProcessComplete(void)
{
    return opl_ok;
}

opint32 msgTxHandler(opuint8 *payLoad,opuint32 len)
{
    opint32 iRet = opl_ok;

    iRet = palTxData(payLoad,len);

    return iRet;
}

void msgProcessThread(void)
{
    opint32 iRet = opl_ok;
    MSG_DATA_BUFF_RX_CHAIN_t *p;
    
    while(1)
    {
        sem_wait(&msgpro);
        while(pRxMsgHeader->active)
        {
            lockRxMsgBuffDataChain();
            if(pRxMsgHeader->active)
            {
                iRet = pRxMsgHeader->msgHandler(pRxMsgHeader->dataBuff,pRxMsgHeader->dataLen);
                if(opl_ok != iRet)
                {
                    msgBuffDataRxClear(pRxMsgHeader);
                    continue;
                }
                iRet = pRxMsgHeader->msgHandlerComplete(pRxMsgHeader->dataBuff,pRxMsgHeader->dataLen);
                if(opl_ok != iRet)
                {
                    msgBuffDataRxClear(pRxMsgHeader);
                    continue;
                }
                
                p = pRxMsgHeader->next;
                msgBuffDataRxClear(pRxMsgHeader);
                pRxMsgHeader = p; 
            }
            unlockRxMsgBuffDataChain();
        }
        /*opl_sdk_trace();*/
    }
}

void msgRxPktThread(void)
{
    opint32 iRet = opl_ok;
    
    while(1)
    {
        opl_sdk_trace();
          
        while(!pRxMsgTailer->active)
        {
            lockRxMsgBuffDataChain();
            iRet = palRxData(pRxMsgTailer->dataBuff,&pRxMsgTailer->dataLen);
            if(opl_ok != iRet)
            {
                opl_sdk_trace();   
                continue;
            }
            /*opl_sdk_trace();*/
            pRxMsgTailer->active = opl_true;
            pRxMsgTailer = pRxMsgTailer->next;
            
            /*wake up process thread*/
            unlockRxMsgBuffDataChain();
            sem_post(&msgpro);
        }

        /*opl_sdk_trace();*/
    }
}

void msgTxPktThread(void)
{
    opint32 iRet = opl_ok;
    opuint8 sInit = 0xf;
    MSG_DATA_BUFF_TX_CHAIN_t *p;
    
    while(1)
    {
        sem_wait(&txpkt);
        while(pTxMsgHeader->active)
        {
            lockTxMsgBuffDataChain();
            if(pTxMsgHeader->active)
            {
                iRet = pTxMsgHeader->msgTxHandler(pTxMsgHeader->dataBuff,pTxMsgHeader->dataLen);
                if(opl_ok != iRet)
                {
                    continue;
                }
                if(NULL != pTxMsgHeader->msgTxComplete)
                {
                    iRet = pTxMsgHeader->msgTxComplete();    
                }
                if(opl_ok != iRet)
                {
                    continue;
                }
                p = pTxMsgHeader->next;
                msgBuffDataTxClear(pTxMsgHeader);
                pTxMsgHeader = p;
            }
            unlockTxMsgBuffDataChain();
  
        }

        /*opl_sdk_trace();*/
   }
}
#ifdef using_sem_for_send
void TimerThread(void)
{
     
    while(1)
    {
#ifdef _VXWORKS_
        taskDelay(sysClkRateGet()*2);
#else
        usleep(MSECOND_BASE(1)*OPL_TIMER_GRANULARITY);
#endif
        if (msgGetTimerflag > 0)
        {
            msgGetTimerflag++;
            if (msgGetTimerflag == TIME_OUT)
            {
                msgGetTimerflag = 0;
                msgGetAckflag   = 0;
                sem_post(&msgGetSem);
            }
        }
    }
}
#else
#endif
opint32 msgSendToTxQueue(opuint8 onuId,opuint16 eventId,opuint8 *payLoad,opuint32 len)
{
    opuint8 *pBuff = NULL;
    opuint8 buffer[MSG_DATA_BUFF_RX_LEN];
    if(NULL == payLoad || MSG_TYPE_END <= eventId)
    {
        opl_sdk_printf(("invalid parameter.\n"));
        return opl_error;
    }

    if(len > (MSG_DATA_BUFF_TX_LEN - sizeof(MSG_HEAD_t)))
    {
        opl_sdk_printf(("too long data, must no more then %d.\n",(MSG_DATA_BUFF_TX_LEN - 2)));
        return opl_error;
    }
    #if 0
    /*
    if(!pTxMsgTailer->active)
    {
        lockRxMsgBuffDataChain();
        pBuff = pTxMsgTailer->dataBuff;
        buffer = pTxMsgTailer->dataBuff;
        OPL_PACK_UINT8(pBuff,onuId);
        OPL_PACK_UINT16(pBuff,eventId);
        OPL_PACK_BUFF(pBuff,payLoad,len);
        pTxMsgTailer->dataLen = len + sizeof(opuint8) + sizeof(opuint16);
        pTxMsgTailer->active = opl_true;
        pTxMsgTailer = pTxMsgTailer->next;
        unlockTxMsgBuffDataChain();
    }else
    {
        opl_sdk_printf(("tx msg queue is full,please wait.\n"));
        return opl_error;
    } */
    /*opl_sdk_trace();
    /*sem_post(&txpkt);*/
    #endif
    /*check event type*/
    if (MSG_IMAGE_UPLOAD == eventId)
    {
        if (FALSE == imageflag)
            imageflag = TRUE;
    }
    else if (MSG_IMAGE_UPLOAD_ACK == eventId)
    {}
    else
    {
        if (TRUE == imageflag)
            return opl_ok;
    }
    pBuff = buffer;
    opl_sdk_memset(pBuff,0x0,MSG_DATA_BUFF_RX_LEN);
    OPL_PACK_UINT8(pBuff,onuId);
    OPL_PACK_UINT16(pBuff,eventId);
    OPL_PACK_BUFF(pBuff,payLoad,len);
    if (opl_true == uart1_comm_enable)
        palTxData(buffer,len + sizeof(opuint8) + sizeof(opuint16));
    return opl_ok;
}
void msgDataDump()
{
        opuint32 iRet;

        MSG_DATA_BUFF_TX_CHAIN_t *p;
        while(pTxMsgHeader->active)
        {

            opl_dump_data(pTxMsgHeader->dataBuff,pTxMsgHeader->dataLen,16);
            p = pTxMsgHeader->next;
            pTxMsgHeader = p;

  
        }

        return ;

}

void msgDebugThread(void)
{
    opuint32 count = 0;
    while(1)
    {
        count++;
        sleep(1000);
    }
}


opint32 HookRegister(OPL_ALARM_EVENT_TYPE_e type, PFUNCTION fun)
{
    opint32 iRet = opl_ok;

    switch(type)
    {
        case OPL_DYING_GASP_ALARM:
            pMsgDyingGaspProcessHandler = fun;
            break;
        case OPL_LINK_FAULT_ALARM:
            pMsgLinkFaultEventProcessHandler = fun;
            break;
        case OPL_MPCP_NACK_EVENT:
            pMsgNackProcessHandler = fun;
            break;
        case OPL_OAM_RECEIVE_EVENT:
            pMsgOamRcvProcessHandler = fun;
            break;
        case OPL_AUTHORIZATION_EVENT:
            pMsgAuthentitedProcessHandler = fun;
            break;
        default:
           break;
    }
    return iRet;
}
opint32 msgHookRegister()
{
    opint32 iRet = opl_ok;

    pMsgDyingGaspProcessHandler = (PFUNCTION)msgDyingGasgProcessHandler;

    pMsgLinkFaultEventProcessHandler = (PFUNCTION)msgLinkFaultEventProcessHandler;

    pMsgAuthentitedProcessHandler = (PFUNCTION)msgAuthenticatedProcessHandler;

    pMsgOamRcvProcessHandler = (PFUNCTION)msgOamRcvProcessHandler;

    pMsgSetProcessHandler= (PFUNCTION)msgSetProcessHandler;

    pMsgSetAckProcessHandler = (PFUNCTION)msgSetAckProcessHandler;

    pMsgGetProcessHandler = (PFUNCTION)msgGetProcessHandler;
  
    pMsgImageUploadProcessHandler = (PFUNCTION)msgImageUploadProcessHandler;
    pMsgImageUploadAckProcessHandler = (PFUNCTION)msgImageUploadAckProcessHandler;

    pMsgGetAckProcessHandler = (PFUNCTION)msgGetAckProcessHandler;

    return iRet;
}

opint32 msgInit(void)
{
    opint32 iRet = opl_ok;

    iRet = msgBuffChainRxInit();

    if(iRet != opl_ok)
    {
        opl_sdk_printf(("init rx chain failed.\n"));
        return iRet;
    }

    iRet = msgBuffChainTxInit();

    if(iRet != opl_ok)
    {
        opl_sdk_printf(("init Tx chain failed.\n"));
        return iRet;
    }
    sem_init(&txpkt,0,0);
    sem_init(&rxpkt,0,0);
    sem_init(&msgpro,0,0);
    sem_init(&getack,0,0);
    sem_init(&msgGetSem,0,0);
#ifdef _VXWORKS_
    pipeDevCreate(Ackpipe,10,MAX_GET_INFO_LENGTH);
    getAckPipefd = open(Ackpipe,O_RDWR,0644);
    getAckMutex = semBCreate(SEM_Q_FIFO,SEM_FULL);
#else
    pipe(msgGetPipeFd);
#endif    

    msgHookRegister();
    iRet = palInit(); 
    if (iRet != opl_ok)
    {
        return iRet;
    }   
    uart1_comm_enable = opl_true;
    return opl_ok;
}

opint32 msgExit(void)
{
    sem_destroy(&txpkt);
    sem_destroy(&rxpkt);
    sem_destroy(&msgpro);
    sem_destroy(&getack);
    sem_destroy(&msgGetSem);
#ifdef _VXWORKS_
    pipeDevDelete(Ackpipe,opl_true);
    semDelete(getAckMutex);
#else
    close(msgGetPipeFd[0]);
    close(msgGetPipeFd[1]);
#endif  

    palExit();
    uart1_comm_enable = opl_false;
    return opl_ok;
}
#if 0
opint32 sdkMainStart(void)
{
    opint32 i;  

    if (opl_ok != msgInit())
    {
        return opl_error;
    }

#ifdef _VXWORKS_
    taskSpawn ("processThreadId", 220, 0,1024*10,(FUNCPTR)msgProcessThread,0,0,0,0,0,0,0,0,0,0);

    taskSpawn ("rxThreadId", 210, 0,8192,(FUNCPTR)msgRxPktThread,0,0,0,0,0,0,0,0,0,0);

    taskSpawn ("txThreadId", 215, 0,8192,(FUNCPTR)msgTxPktThread,0,0,0,0,0,0,0,0,0,0);

    #ifdef using_sem_for_send
    taskSpawn ("TimerThread",224,0,2000,(FUNCPTR)TimerThread,0,0,0,0,0,0,0,0,0,0);

    #else 
    #endif
    
#else 
    pthread_t rxThreadId;
    pthread_t txThreadId;
    pthread_t processThreadId;
    pthread_attr_t debugAttr;
  	pthread_t threadDebug[4];
    #ifdef using_sem_for_send
    pthread_t TimerThreadId;
    #endif 

    if(pthread_create(&processThreadId,NULL,(void *)&msgProcessThread,NULL)!=0)
  	{
  	    opl_sdk_printf(("create process thread faild.\n"));
  	    palExit();
        return opl_error;
  	}

  	if(pthread_create(&rxThreadId,NULL,(void *)&msgRxPktThread,NULL)!=0)
  	{
  	    opl_sdk_printf(("create rx thread faild.\n"));
  	    palExit();
        return opl_error;
  	}

    if(pthread_create(&txThreadId,NULL,(void *)&msgTxPktThread,NULL)!=0)
  	{
  	    opl_sdk_printf(("create tx thread faild.\n"));
  	    palExit();
        return opl_error;
  	}

    #ifdef using_sem_for_send
    if(pthread_create(&TimerThreadId,NULL,(void *)&TimerThread,NULL)!=0)
  	{
  	    opl_sdk_printf(("create timer thread faild.\n"));
  	    palExit();
        return opl_error;
  	}
    #else 
    #endif
    pthread_attr_init(&debugAttr);
    pthread_attr_setdetachstate(&debugAttr,PTHREAD_CREATE_DETACHED);

    for(i = 0; i < 3; i++)
    {
        if(pthread_create(&threadDebug[i],&debugAttr,(void *)&msgDebugThread,NULL) != 0)
        {
            opl_sdk_printf(("create debug %d failed.\n",i));
            palExit();
            return opl_error;
        }
    }
#endif
    return opl_ok;
}
#endif
