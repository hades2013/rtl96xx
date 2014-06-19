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
* FILENAME:  opl_msg_send.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/exchange/src/opl_msg_send.c#1 $
* $Log:$
*
*
**************************************************************************/
#include <opl_type.h>
#include <opl_pal.h>
#include <opl_msg_header.h>
#include <opl_msg_send.h>


extern sem_t msgGetSem;
extern sem_t getack;
extern opint32 msgGetTimerflag;
extern opint32 msgGetAckflag;
extern int uploadack;
extern int imageflag;
#ifdef _VXWORKS_
extern SEM_ID  getAckMutex;
extern int     getAckPipefd;
#else
extern opint32  msgGetPipeFd[2];
pthread_mutex_t mgsGetMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

#ifdef using_sem_for_send

#else
opuint32 sendGetReq = 0;
#endif

opint32 msgOamPackAndSend(opuint8 onuId,opuint8 *p,opuint32 len)
{
    return msgSendToTxQueue(onuId,MSG_OAM_RECEIVED,p,len);
}
opint32 msgDyingGspAndSend(opuint8 onuId,opuint8 *p,opuint32 len)
{
    return msgSendToTxQueue(onuId,MSG_DYING_GSP,p,len);
}
opint32 msgLinkFaultAndSend(opuint8 onuId,opuint8 *p,opuint32 len)
{
    return msgSendToTxQueue(onuId,MSG_LINK_FAULT,p,len);
}
opint32 msgAunthentiedAndSend(opuint8 onuId,opuint8 *p,opuint32 len)
{
    return msgSendToTxQueue(onuId,MSG_AUNTHENTIED,p,len);
}
opint32 msgNackAndSend(opuint8 onuId,opuint8 *p,opuint32 len)
{
    return msgSendToTxQueue(onuId,MSG_NACK_EVEN,p,len);
}
opint32 msgSetReqPackAndSend(opuint8 onuId,opuint8 *p,opuint32 len)
{
    opint32 iRet = opl_ok;

    if(NULL == p)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
       
    iRet = msgSendToTxQueue(onuId,MSG_SET,p,len);

    return iRet;
}

opint32 msgSetAckPackAndSend(opuint8 onuId,opuint8 *p,opuint32 len)
{
    opint32 iRet = opl_ok;

    if(NULL == p)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    
    opl_dump_data((p),(len),(16));
    
    iRet = msgSendToTxQueue(onuId,MSG_SET_ACK,p,len);

    return iRet;  
}

opint32 msgGetReqPackAndSend(opuint8 onuId,opuint8 *p,opuint32 len,opuint8 *pR)
{
    opint32  iRet = opl_ok;
    opuint32 numOfBytes = 0; 
    opuint8  numsleep   = 200;

    if(NULL == p)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    
#ifdef _VXWORKS_
    semTake(getAckMutex,WAIT_FOREVER);

	/* resolve can't get response message issue */
    #ifdef using_sem_for_send
    msgGetTimerflag = 1;
	#endif
	
    iRet = msgSendToTxQueue(onuId,MSG_GET,p,len);   

  
    #ifdef using_sem_for_send
    sem_wait(&msgGetSem);
    #else
    while((!sendGetReq)||(numsleep--))
    {

       taskDelay(sysClkRateGet()/2);
    }
    sendGetReq--;
    #endif    
    opl_dbg(("wait end.\n"));
    if (1 == msgGetAckflag)
    {
        numOfBytes = read(getAckPipefd,pR,MAX_GET_INFO_LENGTH);
    }

    opl_dbg(("get response:\n"));
    opl_dump_data((pR),(numOfBytes),(16));
    semGive(getAckMutex);
#else
    pthread_mutex_lock(&mgsGetMutex);
    
    iRet = msgSendToTxQueue(onuId,MSG_GET,p,len);
    

#ifdef using_sem_for_send
    msgGetTimerflag = 1;
    sem_wait(&msgGetSem);
#else
    while((!sendGetReq)||(numsleep--))
    {

        usleep(MSECOND_BASE(1)*OPL_TIMER_GRANULARITY);
    }
    sendGetReq--;
#endif
    opl_dbg(("wait end.\n"));
    if (1 == msgGetAckflag)
    {
        numOfBytes = read(msgGetPipeFd[0],pR,MAX_GET_INFO_LENGTH);
    }

    opl_dbg(("get response:\n"));
    opl_dump_data((pR),(numOfBytes),(16));
    pthread_mutex_unlock(&mgsGetMutex);
#endif
    return iRet;
}

opint32 msgGetAckPackAndSend(opuint8 onuId,opuint8 *p,opuint32 len)
{
    opint32 iRet = opl_ok;
    opuint32 numOfBytes = 0; 
    
    if(NULL == p)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }

    iRet = msgSendToTxQueue(onuId,MSG_GET_ACK,p,len);

    return iRet;
}

opint32 msgImageUploadPackAndSend(opuint8 onuId,opuint8 *p,opuint32 len)
{
    opuint32 iRet = opl_ok;
    iRet = msgSendToTxQueue(onuId,MSG_IMAGE_UPLOAD,p,len);
    /*
    if (opl_error == iRet)
    {
        #ifdef _VXWORKS_
        taskDelay(sysClkRateGet());
        #else
        usleep(MSECOND_BASE(1)*OPL_TIMER_GRANULARITY);
        #endif
        
        opl_sdk_printf(("send again \n"));
        msgSendToTxQueue(onuId,MSG_IMAGE_UPLOAD,p,len);
    */
    /*
        #ifdef _VXWORKS_
        taskDelay(sysClkRateGet()*2);
        #else
        usleep(MSECOND_BASE(1)*OPL_TIMER_GRANULARITY);
        #endif
    */
    return iRet;
}
opint32 msgImageUploadAckPackAndSend(opuint8 onuId,opuint8 *p,opuint32 len)
{
    opuint32 iRet = opl_ok;
    iRet = msgSendToTxQueue(onuId,MSG_IMAGE_UPLOAD_ACK,p,len);    
    return iRet;
}

opint32 msgImageUploadWaitAckPackAndSend(opuint8 onuId,opuint8 *p,opuint32 len)
{
    opuint32 iRet = opl_ok;
    opuint8  roop = 3;
    iRet = msgSendToTxQueue(onuId,MSG_IMAGE_UPLOAD,p,len);
    uploadack = UPLOAD_WAIT;
    /*upload end free uart1 resource*/
    imageflag = FALSE;
    while(roop--)
    {
        if (UPLOAD_OK == uploadack)
        {
            return opl_ok;
        }
        else if (UPLOAD_WAIT == uploadack)
        {
            sleep(1);
        }
        else
        {
            return uploadack;
        }

    }
    return opl_error;
}
