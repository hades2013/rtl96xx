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
* FILENAME:  opl_pal.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/exchange/src/opl_pal.c#1 $
* $Log:$
*
*
**************************************************************************/
#include <opl_type.h>
#include <opl_pal.h>
#include <opl_msg_header.h>

#ifdef _VXWORKS_


#if defined(__ONU_SDK__)
#define DEVICE_UFILETEST  "/onuUfile"
#elif defined(__FPGA_ONU_SDK__)
#define DEVICE_UFILETEST  "/fpgaOnuUfile"
#elif defined(__IPMUX_SDK__)
#define DEVICE_UFILETEST  "/ipmuxUfile"
#endif

#else


#if defined(__ONU_SDK__)
#define DEVICE_UFILETEST  "/dev/onuUfile"
#elif defined(__FPGA_ONU_SDK__)
#define DEVICE_UFILETEST  "/dev/fpgaOnuUfile"
#elif defined(__IPMUX_SDK__)
#define DEVICE_UFILETEST  "/dev/ipmuxUfile"
#endif

#endif

/* CRC calculate */
#define CRC_BYTE_BIT_COUNT           	0x80
#define CRC8_POLYNOMIAL_CCITT       	0x07
#define CRC8_MOST_SIGNIFICIANT_BIT  	0x80

#define CRC16_POLYNOMIAL_AMERICAN    	0x8005
#define CRC16_POLYNOMIAL_CCITT          0x1021
#define CRC16_RESULT_LENGTH          	0x0000FFFF
#define CRC16_MOST_SIGNIFICIANT_BIT  	0x00008000
#define CRC32_POLYNOMIAL             	0x04C11DB7
#define CRC32_RESULT_LENGTH             0xFFFFFFFF
#define CRC32_MOST_SIGNIFICIANT_BIT     0x80000000
opint32 uartFileFd = 0;

unsigned char palSfd[4] = {0x5a,0x5a,0x5a,0xd5};
unsigned char palEfd[4] = {0xa5,0xa5,0xa5,0x5d};


pthread_mutex_t WriteMutex = PTHREAD_MUTEX_INITIALIZER;
extern sem_t msgpro;

void oplDbgPrintf(char* format,...)
{
    va_list argP;
    char dbgStr[1000] = "";

    va_start(argP, format);
    vsprintf(dbgStr, format, argP);
    
#ifdef _VXWORKS_
    fdprintf(1,"%s",dbgStr);
#else
    printf("%s",dbgStr);
#endif
    return;
}
opuint32 oplCRC16BitShift(opuint8 *pucValue,opuint32 ulValueLegnth
                                            ,opuint16 *pusCrcValue)
{
	opuint8  ucCharBit  = 0;
	opuint16 usCrc     = 0xFFFF;

	if (NULL == pucValue)
	{
		return 1;
	}

	while(0 != ulValueLegnth--)
	{
		for (ucCharBit = CRC_BYTE_BIT_COUNT; 0 != ucCharBit; ucCharBit /= 2)
		{
			if (0 != (usCrc & CRC16_MOST_SIGNIFICIANT_BIT))
			{
				usCrc *= 2;
				usCrc ^= CRC16_POLYNOMIAL_CCITT;
			}
			else
			{
				usCrc *= 2;
			}

			if (((*pucValue) & ucCharBit) != 0)
			{
				usCrc ^= CRC16_POLYNOMIAL_CCITT;
			}
		}

		pucValue++;
	}

	*pusCrcValue = usCrc;

	return(0);
}
opint32 palGetPktHeader(opuint8 *pHead,opuint32 len)
{
    unsigned int dataLen = 0;
    unsigned char buff[len];
    unsigned char buffTmp[len];
    unsigned char c;
    opint32 iRet = 0;
    opint8 frameLen[2] = {0};
    opint32 cnt = 0;

    while(dataLen != 4)
    {
        iRet = read(uartFileFd,&c,1);
        if(!iRet)
        {
           continue;
        }
  
        buff[dataLen++] = c;

        if(dataLen == 4)
        {
            if((buff[0] == 0x5a) 
            && (buff[1] == 0x5a)
            && (buff[2] == 0x5a)
            && (buff[3] == 0xd5))
            {
                cnt = 0;
                while(cnt < 2)
                {
                    iRet = read(uartFileFd,&frameLen[cnt],1);
                    if(iRet != 1)
                    {
                        continue;
                    }
 
                    cnt++;
                }
            }else
            {
                dataLen = 3;
                opl_sdk_memcpy(&buffTmp[0],&buff[1],3);
                opl_sdk_memcpy(&buff[0],&buffTmp[0],3);
            }
        }
    }

    opl_sdk_memcpy(pHead,buff,4);
    opl_sdk_memcpy(&pHead[4],frameLen,2);

    /*
    opl_sdk_printf(("finished rx header.\n"));

    opl_dump_data(pHead,6,16);
    */
    return opl_ok;
}

opint32 palGetPktBody(opuint8 *pData,opuint32 len)
{
    opint32 dataLen = 0;
    opint32 iRet = 0;
#if 0
    while(dataLen < len)
    {
        iRet = read(uartFileFd,&pData[dataLen],1);
        if(iRet == 1)
        {            
            dataLen++;
        }
    }

    if(len != dataLen)
    {
        opl_sdk_printf(("rx data body error.\n"));
        return opl_error;
    }else
    {
        /*opl_dump_data(pData,len,16);*/
    }
#else
    dataLen = read(uartFileFd,pData,len);
    if(len != dataLen)
    {
        opl_sdk_printf(("rx data body error.\n"));
        return opl_error;
    }
 #endif
    return opl_ok;
}

opint32 palGetPktTail(opuint8 *pTail,opuint32 len)
{
    opint32 iRet = 0;
    opint32 count = 0;

    while(count < len)
    {
        iRet = read(uartFileFd,&pTail[count],1);
        if(iRet == 1)
        {
            /*opl_sdk_printf(("%02x ",pTail[count]));*/    
            count++;
        }
    }

    if(opl_sdk_memcmp(&pTail[2],palEfd,4))
    {
    /*
        opl_sdk_printf(("rx end of pkt failed.\n"));
        opl_dump_data(pTail,count,16);
    */
        return opl_error;
    }

    return opl_ok;
}


opint32 palRxData(opuint8 *p,opuint16 *len)
{
    opint32 iRet = 0;
    PAL_FRAME_HEADER_t palFrameHeader;
    PAL_FRAME_TAILER_t palFrameTail;
    PAL_FRAME_HEADER_t *pSfd;
    PAL_FRAME_TAILER_t *pEfd;
    opuint8   buff[MAX_PACKET_LENGTH];
    opuint16 crc;

    opl_sdk_memset(buff,0x0,MAX_PACKET_LENGTH);
    iRet = palGetPktHeader((opuint8 *)&palFrameHeader,sizeof(PAL_FRAME_HEADER_t));
    if(opl_ok != iRet)
    {
        opl_sdk_printf(("get pkt header failed.\n"));
        return opl_error;
    }
    iRet = palGetPktBody(buff,letohs(palFrameHeader.dalaLen));

    if(opl_ok != iRet)
    {
        opl_sdk_printf(("get pkt body failed.\n"));
        return opl_error;
    }

    iRet = palGetPktTail((opuint8 *)&palFrameTail,sizeof(PAL_FRAME_TAILER_t));
    if(opl_ok != iRet)
    {
        opl_sdk_printf(("get pkt tail failed.\n"));
        return opl_error;
    }
    oplCRC16BitShift(buff,letohs(palFrameHeader.dalaLen),&crc);
    if (crc != palFrameTail.crc)
    {
        opl_sdk_printf(("crc check error\n"));
        return opl_error;
    }
    
    opl_sdk_memcpy(p,buff,letohs(palFrameHeader.dalaLen));  
    
    *len = letohs(palFrameHeader.dalaLen);
    /*
    pSfd = (PAL_FRAME_HEADER_t *)buff;
    opl_sdk_memcpy(buff,(opuint8 *)&palFrameHeader,sizeof(PAL_FRAME_HEADER_t));
    opl_sdk_memcpy(pSfd->payload,p,palFrameHeader.dalaLen);
    pEfd = (PAL_FRAME_TAILER_t *)&buff[4+2+palFrameHeader.dalaLen];
    opl_sdk_memcpy((opuint8 *)pEfd,&palFrameTail,sizeof(PAL_FRAME_TAILER_t));
    
    opl_sdk_printf(("length = %d\n.\n",palFrameHeader.dalaLen + 10));
    opl_dump_data(buff,palFrameHeader.dalaLen + 10,16);
    */
    return opl_ok;
}


opint32 palTxData(opuint8 *p,opuint16 len)
{
    opint32 iRet = opl_ok;
    opuint8 buff[MAX_PACKET_LENGTH];
    opint32 count = 0;
    opuint16 crc;
    
    PAL_FRAME_HEADER_t *pSfd;
    PAL_FRAME_TAILER_t *pEfd;

    opl_sdk_memset(buff,0x0,MAX_PACKET_LENGTH);
    pSfd = (PAL_FRAME_HEADER_t*)buff;
    pEfd = (PAL_FRAME_TAILER_t *)&buff[4+2+len];
    opl_sdk_memcpy(pSfd->sfd,palSfd,4);
    pSfd->dalaLen = htoles(len);
    opl_sdk_memcpy(pSfd->payload,p,len);
    oplCRC16BitShift(p,len,&crc);
    pEfd->crc = htoles(crc);
    opl_sdk_memcpy(pEfd->efd,palEfd,4);

    opl_dump_data(buff,4+2+sizeof(PAL_FRAME_TAILER_t)+len,16);

    pthread_mutex_lock(&WriteMutex);
    count = write(uartFileFd,buff,4+2+sizeof(PAL_FRAME_TAILER_t)+len);

    if(count !=  (4+2+sizeof(PAL_FRAME_TAILER_t)+len))
    {
        opl_sdk_printf(("send pkt only %d bytes,but %d bytes.\n",count,(4+2+4+sizeof(PAL_FRAME_TAILER_t))));
        return opl_error;
    }
    usleep(10);
    pthread_mutex_unlock(&WriteMutex);    
    return iRet;
}

opint32 palInit(void)
{
    opint32 iRet = opl_ok;

    uartFileFd = open( DEVICE_UFILETEST,O_CREAT | O_RDWR,0); 
    if(uartFileFd < 0)
    {
        opl_sdk_printf(("open uart file failed.\n"));
        return -1;
    }    

    return iRet;
}
opint32 palExit(void)
{
    if (uartFileFd > 0)
        close(uartFileFd);
    return opl_ok;
}
opuint32 palClearUart1Txbuff(void)
{
    if (uartFileFd > 0)
        ioctl(uartFileFd,UART1_COMMAND_CLEAR_TXBUFFER,NULL);
    return opl_ok;
}
opuint32 palClearUart1Rxbuff(void)
{
    if (uartFileFd > 0)
        ioctl(uartFileFd,UART1_COMMAND_CLEAR_RXBUFFER,NULL);
    return opl_ok;
}
opuint32 palEnableUart1Interrupt(void)
{
    if (uartFileFd > 0)
        ioctl(uartFileFd,ENABLE_UART1_INTERRUPT,NULL);
    return opl_ok;
}
opuint32 palDisableUart1Interrupt(void)
{
    if (uartFileFd > 0)
        ioctl(uartFileFd,DISABLE_UART1_INTERRUPT,NULL);
    return opl_ok;
}
int palShowRxData(opuint32 length)
{
    opuint32 len;
    unsigned char buff[2048];

    if (uartFileFd > 0)
        ioctl(uartFileFd,DUMP_UART1_RXBUFFER,(char *)length);

    return 0;
}
int palShowTxData(opuint32 length)
{
    opuint32 len;
    unsigned char buff[2048];

    if (uartFileFd > 0)
        ioctl(uartFileFd,DUMP_UART1_TXBUFFER,(char *)length);

    return 0;
}
