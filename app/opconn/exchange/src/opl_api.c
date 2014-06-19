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
* FILENAME:  opl_api.c
*
* DESCRIPTION: 
*	api called by customer.
*
* Date Created: Apr 30, 2008
*
* Authors(optional): zzhu
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/exchange/src/opl_api.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <opl_api.h>
#if 0
opint32 OnuMacIdSet(opuint8 onu_id,opuint8 *macId)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff = buff;
    
    opint32 iRet = opl_error;    
    opuint32 len = 0;

    if(NULL == macId)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }

    opl_sdk_memset(pBuff,0x0,OPL_API_PACK_BUFF_LENGTH);
    opl_dump_data((macId),(6),(16));
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));

    
    /*add function index--set pon mac address*/
    admParPacket.funIndex = ADM_MAC_ID_SET;
    admParPacket.numOfP   = 1;
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 6;
    
    len = OPL_API_PACK_HEADER_LENGTH + sizeof(opuint8)+sizeof(opuint8)+admParPacket.pVal[0].lengthOfPar;

    /*pack data*/
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_BUFF(pBuff,macId,admParPacket.pVal[0].lengthOfPar);

    opl_dump_data(buff,(len),(16));
    /*pack end*/
    iRet = msgSetReqPackAndSend(onu_id,buff,len);
    
    opl_return_parse(iRet);
}
opint32 OnuMacIdGet(opuint8 onu_id,opuint8 *macId)
{
    
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;

    if(NULL == macId)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(pBuff,0x0,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--get pon mac address*/
    admParPacket.funIndex = ADM_MAC_ID_GET;
    admParPacket.numOfP   = 0;

    OPL_PACK_UINT16(pBuff, admParPacket.funIndex);
    OPL_PACK_UINT16(pBuff, admParPacket.numOfP);
    
    len = OPL_API_PACK_HEADER_LENGTH;
    opl_dump_data(buff,(len),(16));
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);

    if (opl_ok == iRet)
    {
        if ((ADM_MAC_ID_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_BUFF(pReturnBuff,macId,ackpacket.dataLen);
            iRet = opl_ok;
        }
        else
        {
             iRet = opl_error;
        }
    }
    opl_dbg(("ponMac= %02x:%02x:%02x:%02x:%02x:%02x\n",macId[0],
                                                       macId[1],
                                                       macId[2],
                                                       macId[3],
                                                       macId[4],
                                                       macId[5]));
    opl_return_parse(iRet);
}

opint32 OnuLaserEnable(opuint8 onu_id,OPBOOL enable)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff = buff;
    
    opint32 iRet = opl_error;
    opuint32 len = 0;

    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);

    
    /*add function index--laser enable*/
    admParPacket.funIndex = ADM_LASER_ENABLE_SET;
    admParPacket.numOfP = 1;

    /*add parameter*/
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 1;
    admParPacket.pVal[0].pVal[0]     = enable;  

    /*pack data*/
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].pVal[0]);
    
    len = OPL_API_PACK_HEADER_LENGTH + admParPacket.numOfP*sizeof(ADM_PAR_INFO_t);

    opl_dump_data(buff,(len),(16));
    
    iRet = msgSetReqPackAndSend(onu_id,buff,len);
    
    opl_return_parse(iRet);
}
opint32 OnuLaserEnableGet(opuint8 onu_id,OPBOOL *enable)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff     = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;

    if(NULL == enable)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--get laser enable status*/
    admParPacket.funIndex = ADM_LASER_ENABLE_GET;
    admParPacket.numOfP   = 0;     

    len = OPL_API_PACK_HEADER_LENGTH;

    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);

    if (opl_ok == iRet)
    {
        if ((ADM_LASER_ENABLE_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT8(pReturnBuff,*enable);
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}
opint32 OnuDevicerReset(opuint8 onu_id,OPL_ONU_RESET_REASON_e type)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x00};
    opuint8 *pBuff = buff;
    opint32 iRet = opl_error;    
    opint32 len = 0;

    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--reset device*/
    admParPacket.funIndex = ADM_DEVICE_RESET;
    admParPacket.numOfP = 1; 
    /*add parameter*/
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 1;
    admParPacket.pVal[0].pVal[0]     = type; 
    
    len = OPL_API_PACK_HEADER_LENGTH + sizeof(opuint8) + sizeof(opuint8) + sizeof(opuint8);

    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].pVal[0]);
    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgSetReqPackAndSend(onu_id,buff,len);
    
    opl_return_parse(iRet);
}
opint32 OnuVersionGet(opuint8 onu_id,OPL_ONU_VERSION_t *version)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;

    if(NULL == version)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--get version*/
    admParPacket.funIndex = ADM_VERSION_GET;
    admParPacket.numOfP   = 0;     

    len = OPL_API_PACK_HEADER_LENGTH ;

    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);

    if (opl_ok == iRet)
    {
        if ((ADM_VERSION_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT16(pReturnBuff,version->hw_major_ver);
            OPL_UNPACK_UINT16(pReturnBuff,version->hw_minor_ver);
            OPL_UNPACK_UINT16(pReturnBuff,version->fw_major_ver);
            OPL_UNPACK_UINT16(pReturnBuff,version->fw_minor_ver);
            OPL_UNPACK_UINT16(pReturnBuff,version->fw_build_num);
            OPL_UNPACK_UINT16(pReturnBuff,version->fw_maintenance_ver);
            OPL_UNPACK_UINT32(pReturnBuff,version->oamVersion);
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Software version: %d.%d.%d\r\n",version->fw_build_num,version->fw_major_ver,version->fw_minor_ver));
    opl_return_parse(iRet);
}
#endif
opint32 OnuOamTransmit(opuint8 onu_id,opuint32 opcode,opuint8 *p,opuint32 len)
{
    
    opint32 iRet = opl_error;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff = buff;

    if(NULL == p)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    OPL_PACK_UINT8(pBuff,opcode);
    OPL_PACK_BUFF(pBuff,p,len);
    
    iRet = msgOamPackAndSend(onu_id,buff,len+1);

    opl_return_parse(iRet);
}
#if 0
opint32 OnuLostGet(opuint8 onu_id,OPBOOL *status)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;

    if(NULL == status)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x0,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--get lost status*/
    admParPacket.funIndex = ADM_PON_LINK_GET;
    admParPacket.numOfP   = 0;     

    len = OPL_API_PACK_HEADER_LENGTH ;

    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);

    if (opl_ok == iRet)
    {
        if ((ADM_PON_LINK_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT8(pReturnBuff,*status);
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}
opint32 OnuUpTimeGet(opuint8 onu_id,opuint64 *up_time_msec)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff     =  buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;

    if(NULL == up_time_msec)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--get uptime*/
    admParPacket.funIndex = ADM_PON_UPTIME_GET;
    admParPacket.numOfP   = 0;     

    len = OPL_API_PACK_HEADER_LENGTH ;
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);

    if (opl_ok == iRet)
    {
        if ((ADM_PON_UPTIME_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT64(pReturnBuff,*up_time_msec);
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}
opint32 OnuExtendMTUSizeEnable(opuint8 onu_id,OPBOOL enable)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;

    ADM_ACK_PAR_PACKET_t *ackpacket = (ADM_ACK_PAR_PACKET_t*)pR;

    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--set Extend MTU size*/
    admParPacket.funIndex = ADM_EXTEND_MTU_ENABLE;
    admParPacket.numOfP   = 1;     

    /*add parameter*/
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 1;
    admParPacket.pVal[0].pVal[0]     = enable; 
    
    len = OPL_API_PACK_HEADER_LENGTH + sizeof(opuint8) + sizeof(opuint8) + sizeof(opuint8);

    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].pVal[0]);
    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgSetReqPackAndSend(onu_id,buff,len);
    
    opl_return_parse(iRet);
}
opint32 OnuExtendMTUSizeEnableGet(opuint8 onu_id,OPBOOL *enable)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff     = buff;
    opint32 iRet       = opl_ok;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;

    if(NULL == enable)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--get extend MTU size*/
    admParPacket.funIndex = ADM_EXTEND_MTU_ENABLE_GET;
    admParPacket.numOfP   = 0;     

    len = OPL_API_PACK_HEADER_LENGTH ;
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);

    if (opl_ok == iRet)
    {
        if ((ADM_EXTEND_MTU_ENABLE_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT8(pReturnBuff,*enable);
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}
opint32 OnuImageBurnEx(opuint8 onu_id,const opuint8 *image,const opuint32 length,OPBOOL block)
{       
    opint32 iRet       = opl_error;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint16 roop   = 0;
    
    opuint16 sequence = 0;
    opuint16 crc      = 0;
    opuint32 len      = 0;
    opuint8  name[20] = "opconn.img";
    
    if(NULL == image)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    oplCRC16BitShift(image,length,&crc);
        
    /*-------------------send upload star and image name---------------*/
    sequence = UPLOAD_IMAGE_START;
    opl_sdk_memset(pBuff,0x0,OPL_API_PACK_BUFF_LENGTH);
    OPL_PACK_UINT16(pBuff,sequence);
    OPL_PACK_BUFF(pBuff,name,sizeof(name));
    msgImageUploadPackAndSend(onu_id,buff,(sizeof(sequence)+sizeof(name)));
    /*-------------------send image data---------------*/
    /*middle data*/
    roop = (length/(MAX_PALOAD_LENGTH - sizeof(sequence))); 
    while(roop)
    {
        roop--;
        sequence++;
        pBuff = buff;
        len = (MAX_PALOAD_LENGTH - sizeof(sequence));
        opl_sdk_memset(pBuff,0x0,OPL_API_PACK_BUFF_LENGTH);
        OPL_PACK_UINT16(pBuff,sequence);
        OPL_PACK_BUFF(pBuff,image,len);  
        image += len;
        msgImageUploadPackAndSend(onu_id,buff,MAX_PALOAD_LENGTH);

    }

    /*tail data*/
    if (length%(MAX_PALOAD_LENGTH - sizeof(sequence)))
    {
        sequence++;
        pBuff = buff;
        len = (length%(MAX_PALOAD_LENGTH - sizeof(sequence)));
        opl_sdk_memset(pBuff,0x0,OPL_API_PACK_BUFF_LENGTH);
        OPL_PACK_UINT16(pBuff,sequence);
        OPL_PACK_BUFF(pBuff,image,len);
        msgImageUploadPackAndSend(onu_id,buff,(len+2));
    }

    /*-------------------send upload end and CRC check---------------*/
    sequence = UPLOAD_IMAGE_END;
    pBuff = buff;
    opl_sdk_memset(pBuff,0x0,OPL_API_PACK_BUFF_LENGTH);   
    OPL_PACK_UINT16(pBuff,sequence);
    OPL_PACK_UINT16(pBuff,crc);
    msgImageUploadPackAndSend(onu_id,buff,(sizeof(sequence)+sizeof(crc)));

    
    opl_return_parse(iRet);
}
opint32 OnuUNIConfigSet(opuint8 onu_id,ONU_UNI_CONFIG_t *config)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;

    ADM_ACK_PAR_PACKET_t *ackpacket = (ADM_ACK_PAR_PACKET_t*)pR;

    if(NULL == config)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    
    /*add function index--set UNI config*/
    admParPacket.funIndex = ADM_UNI_CONFIG_SET;
    admParPacket.numOfP   = 1;     

    /*add parameter*/
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = sizeof(ONU_UNI_CONFIG_t);
    opl_sdk_memcpy(&(admParPacket.pVal[0].pVal),config,sizeof(ONU_UNI_CONFIG_t));

    len = OPL_API_PACK_HEADER_LENGTH + sizeof(opuint8)+sizeof(opuint8)+admParPacket.pVal[0].lengthOfPar;

    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_BUFF(pBuff,admParPacket.pVal[0].pVal,admParPacket.pVal[0].lengthOfPar);  

    opl_dump_data(buff,(len),(16));
   
    iRet = msgSetReqPackAndSend(onu_id,buff,len);
    
    opl_return_parse(iRet);
}
opint32 OnuUNIConfigGet(opuint8 onu_id,ONU_UNI_CONFIG_t *config)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff     = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;

    if(NULL == config)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--get UNI config*/
    admParPacket.funIndex = ADM_UNI_CONFIG_GET;
    admParPacket.numOfP   = 0;     

    len = OPL_API_PACK_HEADER_LENGTH ;

    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    opl_dump_data(buff,(len),(16));
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);

    if (opl_ok == iRet)
    {
        if ((ADM_UNI_CONFIG_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT8(pReturnBuff,config->MAC_type);
            OPL_UNPACK_UINT8(pReturnBuff,config->MII_rate);
            OPL_UNPACK_UINT8(pReturnBuff,config->duplex);
            OPL_UNPACK_UINT8(pReturnBuff,config->gmii_master_mode);
            OPL_UNPACK_UINT8(pReturnBuff,config->auto_negotiate);
            OPL_UNPACK_UINT8(pReturnBuff,config->mode);
            OPL_UNPACK_UINT8(pReturnBuff,config->advertise);
            OPL_UNPACK_UINT8(pReturnBuff,config->adv_10_half);
            OPL_UNPACK_UINT8(pReturnBuff,config->adv_10_full);
            OPL_UNPACK_UINT8(pReturnBuff,config->adv_100_half);
            OPL_UNPACK_UINT8(pReturnBuff,config->adv_100_full);
            OPL_UNPACK_UINT8(pReturnBuff,config->adv_100_t4);
            OPL_UNPACK_UINT8(pReturnBuff,config->adv_pause);
            OPL_UNPACK_UINT8(pReturnBuff,config->adv_asym_pause);
            OPL_UNPACK_UINT8(pReturnBuff,config->adv_1000_half);
            OPL_UNPACK_UINT8(pReturnBuff,config->adv_1000_full);
            OPL_UNPACK_UINT8(pReturnBuff,config->adv_port_type);
            OPL_UNPACK_UINT8(pReturnBuff,config->phy_address);
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}
opint32 OnuConnectionSet(opuint8 onu_id,OPL_ONU_CONNECTION_e connection)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;

    ADM_ACK_PAR_PACKET_t *ackpacket = (ADM_ACK_PAR_PACKET_t*)pR;

    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--set connection*/
    admParPacket.funIndex = ADM_PON_CONNECTION_SET;
    admParPacket.numOfP   = 1;     

    /*add parameter*/
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 1;
    admParPacket.pVal[0].pVal[0] = connection;

    len = OPL_API_PACK_HEADER_LENGTH + sizeof(opuint8)+sizeof(opuint8)+sizeof(opuint32);

    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].pVal[0]);    

    opl_dump_data(buff,(len),(16));
    
    iRet = msgSetReqPackAndSend(onu_id,buff,len);
    
    opl_return_parse(iRet);
}
opint32 OnuConnectionGet(opuint8 onu_id,OPL_ONU_CONNECTION_e *connection)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff     = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;
    opuint8 cnnState;

    if(NULL == connection)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--get connection*/
    admParPacket.funIndex = ADM_PON_CONNECTION_GET;
    admParPacket.numOfP   = 0;     

    len = OPL_API_PACK_HEADER_LENGTH ;
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);

    if (opl_ok == iRet)
    {
        if ((ADM_PON_CONNECTION_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT8(pReturnBuff,cnnState);
            *connection = cnnState;
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}
opint32 OnuStatusGet(opuint8 onu_id,OPL_ONU_STATUS_t *status)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff     = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;

    if(NULL == status)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--get status*/
    admParPacket.funIndex = ADM_PON_STATUS_GET;
    admParPacket.numOfP   = 0;     

    len = OPL_API_PACK_HEADER_LENGTH ;
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);

        
    if (opl_ok == iRet)
    {
        if ((ADM_PON_STATUS_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT8(pReturnBuff,status->connection);
            OPL_UNPACK_UINT8(pReturnBuff,status->oam_link_established);
            OPL_UNPACK_UINT8(pReturnBuff,status->authorization_state);
            OPL_UNPACK_UINT8(pReturnBuff,status->PON_Loopback);
            OPL_UNPACK_BUFF(pReturnBuff,status->mac_addr,6);
            OPL_UNPACK_UINT16(pReturnBuff,status->ONU_llid);
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}
opint32 OnuEncryptionSet(opuint8 onu_id,OPL_ONU_ENCRYPTION_e mode)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;

    ADM_ACK_PAR_PACKET_t *ackpacket = (ADM_ACK_PAR_PACKET_t*)pR;

    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--set Encryption*/
    admParPacket.funIndex = ADM_PON_ENCRYPTION_SET;
    admParPacket.numOfP   = 1;     

    /*add parameter*/
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 1;
    admParPacket.pVal[0].pVal[0]     = mode;
    
    len = OPL_API_PACK_HEADER_LENGTH + sizeof(opuint8)+sizeof(opuint8)+sizeof(opuint32);
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].pVal[0]); 
    opl_dump_data(buff,(len),(16));    
    
    iRet = msgSetReqPackAndSend(onu_id,buff,len);
    
    opl_return_parse(iRet);
}
opint32 OnuEncryptionGet(opuint8 onu_id,OPL_ONU_ENCRYPTION_e *mode)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff     = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;
    opuint8 encryMode;
    
    if(NULL == mode)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--get Encryption*/
    admParPacket.funIndex = ADM_PON_ENCRYPTION_GET;
    admParPacket.numOfP   = 0;     

    len = OPL_API_PACK_HEADER_LENGTH ;
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);

    if (opl_ok == iRet)
    {
        if ((ADM_PON_ENCRYPTION_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT8(pReturnBuff,encryMode);
            *mode = encryMode;
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}
opint32 OnuEncryptionActKeySet(opuint8 onu_id,opuint8 key_seq)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;

    ADM_ACK_PAR_PACKET_t *ackpacket = (ADM_ACK_PAR_PACKET_t*)pR;

    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--set EncryptionActKey*/
    admParPacket.funIndex = ADM_PON_ENCRYPTION_ACT_KEY_SET;
    admParPacket.numOfP   = 1;     

    /*add parameter*/
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 1;
    admParPacket.pVal[0].pVal[0] = key_seq;
    
    len = OPL_API_PACK_HEADER_LENGTH + admParPacket.numOfP*sizeof(ADM_PAR_INFO_t);

    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].pVal[0]);
    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgSetReqPackAndSend(onu_id,buff,len);
    
    opl_return_parse(iRet);
}
opint32 OnuEncryptionActKeyGet(opuint8 onu_id,opuint8 *key_seq)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff     = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;

    if(NULL == key_seq)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--get EncryptionActKey*/
    admParPacket.funIndex = ADM_PON_ENCRYPTION_ACT_KEY_GET;
    admParPacket.numOfP   = 0;     

    len = OPL_API_PACK_HEADER_LENGTH ;
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);

    opl_dump_data(buff,len,16);
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);
    
    if (opl_ok == iRet)
    {
        if ((ADM_PON_ENCRYPTION_ACT_KEY_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT8(pReturnBuff,*key_seq);
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}

opint32 OnuPonPortStatsGet(opuint8 onu_id,OPL_ONU_STATISTIC_t *stats)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;

    if(NULL == stats)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--get pon port statstic*/
    admParPacket.funIndex = ADM_PON_PORT_STATISTIC_GET;
    admParPacket.numOfP   = 0;     

    len = OPL_API_PACK_HEADER_LENGTH ;
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);

    opl_dump_data(buff,len,16);
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);
    
    if (opl_ok == iRet)
    {
        if ((ADM_PON_PORT_STATISTIC_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT32(pReturnBuff,stats->TxTotalPkt);
            OPL_UNPACK_UINT32(pReturnBuff,stats->TxBroad);
            OPL_UNPACK_UINT32(pReturnBuff,stats->TxMulti);
            OPL_UNPACK_UINT32(pReturnBuff,stats->TxPause);
            OPL_UNPACK_UINT32(pReturnBuff,stats->TxMacCtrl);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Tx64byte);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Tx65to127);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Tx128to255);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Tx256to511);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Tx512to1023);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Tx1024to1518);
            OPL_UNPACK_UINT32(pReturnBuff,stats->TxByte);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Tx1519toMTU);
            
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxGood);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxDrop);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxCRCAlignErr);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxLenErr);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxFragments);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxJabbers);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxTotalPks);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxBroad);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxMulti);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxPause);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxErrMacCtrl);

            OPL_UNPACK_UINT32(pReturnBuff,stats->RxMACCtrl);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rxshort);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rxlong);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rx64byte);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rx65to127);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rx128to255);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rx256to511);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rx512to1023);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rx1024to1518);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rx1519toMTU);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxByte);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxL3Multi);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxRxERPkts);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxIPGErr);
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}

opint32 OnuUniPortStatsGet(opuint8 onu_id,opuint8 port,OPL_ONU_STATISTIC_t *stats)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff     = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;
    
    if(NULL == stats)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--get Ge port statstic*/
    admParPacket.funIndex = ADM_UNI_PORT_STATISTIC_GET;
    admParPacket.numOfP   = 1;     

    /*add parameter*/
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 1;
    admParPacket.pVal[0].pVal[0]     = port; /*led type*/ 

    /*pack data*/
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].pVal[0]);

    len = (OPL_API_PACK_HEADER_LENGTH + admParPacket.numOfP*sizeof(ADM_PAR_INFO_t)); 
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);
    
    if (opl_ok == iRet)
    {
        if ((ADM_UNI_PORT_STATISTIC_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT32(pReturnBuff,stats->TxTotalPkt);
            OPL_UNPACK_UINT32(pReturnBuff,stats->TxBroad);
            OPL_UNPACK_UINT32(pReturnBuff,stats->TxMulti);
            OPL_UNPACK_UINT32(pReturnBuff,stats->TxPause);
            OPL_UNPACK_UINT32(pReturnBuff,stats->TxMacCtrl);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Tx64byte);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Tx65to127);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Tx128to255);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Tx256to511);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Tx512to1023);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Tx1024to1518);
            OPL_UNPACK_UINT32(pReturnBuff,stats->TxByte);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Tx1519toMTU);
            
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxGood);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxDrop);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxCRCAlignErr);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxLenErr);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxFragments);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxJabbers);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxTotalPks);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxBroad);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxMulti);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxPause);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxErrMacCtrl);

            OPL_UNPACK_UINT32(pReturnBuff,stats->RxMACCtrl);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rxshort);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rxlong);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rx64byte);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rx65to127);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rx128to255);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rx256to511);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rx512to1023);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rx1024to1518);
            OPL_UNPACK_UINT32(pReturnBuff,stats->Rx1519toMTU);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxByte);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxL3Multi);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxRxERPkts);
            OPL_UNPACK_UINT32(pReturnBuff,stats->RxIPGErr);
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}

opint32 OnuEventHookRegister(opuint8 onu_id,OPL_ALARM_EVENT_TYPE_e type,PFUNCTION fun)
{
    opint32 iRet       = opl_error;
    if(NULL == fun)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    
    iRet = HookRegister(type,fun);

    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}
opint32 OnuEventEnable(opuint8 onu_id,OPL_ALARM_EVENT_TYPE_e type,OPBOOL enable)
{
    
    ADM_PAR_PACKET_t admParPacket;
    /*send buffer*/
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff = buff;
    
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    ADM_ACK_PAR_PACKET_t ackpacket;

    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(pBuff,0x0,OPL_API_PACK_BUFF_LENGTH);
    
    /*add function index*/
    admParPacket.funIndex = ADM_EVENT_ENABLE;
    admParPacket.numOfP   = 1;

    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 2;
    
    /*pack data*/
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    
    OPL_PACK_UINT8(pBuff,type);  
    OPL_PACK_UINT8(pBuff,enable); 
    len = OPL_API_PACK_HEADER_LENGTH + admParPacket.numOfP*sizeof(ADM_PAR_INFO_t);    
    opl_dump_data(buff,(len),(16));
    
    /*pack end*/
    iRet = msgSetReqPackAndSend(onu_id,buff,len);
    
    opl_return_parse(iRet);
}
opint32 OnuHwRegisterRead(opuint8 onu_id,opuint32 regaddr,opuint32 *value)
{
    
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff     = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;

    if(NULL == value)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(pBuff,0x0,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--read hardware register*/
    admParPacket.funIndex = ADM_PON_REGISTER_READ;
    admParPacket.numOfP   = 1;

    admParPacket.pVal[0].typeOfPar   = ADM_UINT32;
    admParPacket.pVal[0].lengthOfPar = sizeof(regaddr);
    
    /*pack data*/
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_UINT32(pBuff,regaddr);    
    len = OPL_API_PACK_HEADER_LENGTH + admParPacket.numOfP*sizeof(ADM_PAR_INFO_t);    
    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);

    if (opl_ok == iRet)
    {
        if ((ADM_MAC_ID_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT32(pReturnBuff, *value)
            iRet = opl_ok;
        }
        else
        {
             iRet = opl_error;
        }
    }
    opl_return_parse(iRet);
}


opint32 OnuHwRegisterWrite(opuint8 onu_id,opuint32 regaddr,opuint32 value)
{
    
    ADM_PAR_PACKET_t admParPacket;
    /*send buffer*/
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff = buff;
    
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    ADM_ACK_PAR_PACKET_t ackpacket;

    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(pBuff,0x0,OPL_API_PACK_BUFF_LENGTH);
    
    /*add function index--write hardware register*/
    admParPacket.funIndex = ADM_PON_REGISTER_WRITE;
    admParPacket.numOfP   = 1;

    admParPacket.pVal[0].typeOfPar   = ADM_UINT32;
    admParPacket.pVal[0].lengthOfPar = 8;
    
    /*pack data*/
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_UINT32(pBuff,regaddr);  
    OPL_PACK_UINT32(pBuff,value); 
    len = OPL_API_PACK_HEADER_LENGTH + admParPacket.numOfP*sizeof(ADM_PAR_INFO_t);    
    opl_dump_data(buff,(len),(16));
    
    /*pack end*/
    iRet = msgSetReqPackAndSend(onu_id,buff,len);
    
    opl_return_parse(iRet);
}
opint32 OnuLEDSetConfig(opuint8 onu_id,OPL_LED_e led,OPL_LED_ACT_e act)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff = buff;
    
    opint32 iRet = opl_error;
    opuint32 len = 0;

    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);

    
    /*add function index--LED config*/
    admParPacket.funIndex = ADM_PON_LED_SET;
    admParPacket.numOfP = 1;

    /*add parameter*/
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 2;
    admParPacket.pVal[0].pVal[0]     = led; /*led type*/ 
    admParPacket.pVal[0].pVal[1]     = act; /*led action*/ 

    /*pack data*/
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_BUFF(pBuff,admParPacket.pVal[0].pVal,admParPacket.pVal[0].lengthOfPar);
    
    len = OPL_API_PACK_HEADER_LENGTH + admParPacket.numOfP*sizeof(ADM_PAR_INFO_t);

    opl_dump_data(buff,(len),(16));
    
    iRet = msgSetReqPackAndSend(onu_id,buff,len);
    
    opl_return_parse(iRet);
}
opint32 OnuIpMulticastEnable(opuint8 onu_id,OPL_ONU_DIRECTION_e direction,OPBOOL enable)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff = buff;
    
    opint32 iRet = opl_error;
    opuint32 len = 0;

    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);

    
    /*add function index--ip multicast enable*/
    admParPacket.funIndex = ADM_PON_IP_MULTICASET_SET;
    admParPacket.numOfP = 1;

    /*add parameter*/
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 2;
    admParPacket.pVal[0].pVal[0]     = direction; /*stream direction*/
    admParPacket.pVal[0].pVal[1]     = enable; /*enable flag*/ 

    /*pack data*/
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_BUFF(pBuff,admParPacket.pVal[0].pVal,admParPacket.pVal[0].lengthOfPar);
    
    len = OPL_API_PACK_HEADER_LENGTH + admParPacket.numOfP*sizeof(ADM_PAR_INFO_t);

    opl_dump_data(buff,(len),(16));
    
    iRet = msgSetReqPackAndSend(onu_id,buff,len);
    
    opl_return_parse(iRet);
}
opint32 OnuIpMulticastEnableGet(opuint8 onu_id,OPL_ONU_DIRECTION_e direction,OPBOOL *enable)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff     = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;

    if(NULL == enable)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--ip multicast enable get*/
    admParPacket.funIndex = ADM_PON_IP_MULTICASET_GET;
    admParPacket.numOfP   = 1;     

    /*add parameter*/
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 1;
    admParPacket.pVal[0].pVal[0]     = direction; /*stream direction*/ 

    /*pack data*/
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].pVal[0]);

    len = OPL_API_PACK_HEADER_LENGTH + admParPacket.numOfP*sizeof(ADM_PAR_INFO_t);    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);

    if (opl_ok == iRet)
    {
        if ((ADM_PON_IP_MULTICASET_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT8(pReturnBuff,*enable);
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}

opint32 OnuPQUEUEIngressLimitGet(opuint8 onu_id,OPL_ONU_PQUEUE_e queue,opuint8 pri,opuint16 *limit)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff     = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;

    if(NULL == limit)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--queue limit get*/
    admParPacket.funIndex = ADM_PON_QUEUE_LIMIT_GET;
    admParPacket.numOfP   = 1;     

    /*add parameter*/
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 2;
    admParPacket.pVal[0].pVal[0]     = queue; /*queue type*/ 
    admParPacket.pVal[0].pVal[1]     = pri; /*queue priority*/ 

    /*pack data*/
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_BUFF(pBuff,admParPacket.pVal[0].pVal,admParPacket.pVal[0].lengthOfPar);

    len = OPL_API_PACK_HEADER_LENGTH + admParPacket.numOfP*sizeof(ADM_PAR_INFO_t);    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);

    if (opl_ok == iRet)
    {
        if ((ADM_PON_QUEUE_LIMIT_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT16(pReturnBuff,*limit);
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}
opint32 OnuClassifyMulticastConfigSet(opuint8 onu_id,
                     OPL_ONU_MULTICAST_TYPE_e mc_type,
                     OPL_ONU_DIRECTION_e direction,
                     OPL_ONU_FOREARDING_ACTION_e act)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff = buff;
    
    opint32 iRet = opl_error;
    opuint32 len = 0;

    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);

    
    /*add function index--classify multicast config set*/
    admParPacket.funIndex = ADM_PON_CLASSIFY_MULTICASET_SET;
    admParPacket.numOfP = 1;

    /*add parameter*/
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 3;
    admParPacket.pVal[0].pVal[0]     = mc_type;   /*multicast type*/
    admParPacket.pVal[0].pVal[1]     = direction; /*stream direction*/ 
    admParPacket.pVal[0].pVal[2]     = act; /*stream action*/

    /*pack data*/
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_BUFF(pBuff,admParPacket.pVal[0].pVal,admParPacket.pVal[0].lengthOfPar);
    
    len = OPL_API_PACK_HEADER_LENGTH + admParPacket.numOfP*sizeof(ADM_PAR_INFO_t);

    opl_dump_data(buff,(len),(16));
    
    iRet = msgSetReqPackAndSend(onu_id,buff,len);
    
    opl_return_parse(iRet);
}
opint32 OnuClassifyMulticastConfigGet(opuint8 onu_id,
                     OPL_ONU_MULTICAST_TYPE_e mc_type,
                     OPL_ONU_DIRECTION_e direction,
                     OPL_ONU_FOREARDING_ACTION_e *act)
{
    ADM_PAR_PACKET_t admParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH];
    opuint8 *pBuff     = buff;
    opint32 iRet       = opl_error;
    opuint32 len       = 0;
    opuint8 pR[OPL_API_RET_BUFF_LENGTH]    = {0X0} ;
    opuint8 *pReturnBuff = pR;
    ADM_ACK_PAR_PACKET_t ackpacket;

    if(NULL == act)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }
    opl_sdk_memset(&admParPacket,0x0,sizeof(ADM_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    /*add function index--classify multicast config get*/
    admParPacket.funIndex = ADM_PON_CLASSIFY_MULTICASET_GET;
    admParPacket.numOfP   = 1;     

    /*add parameter*/
    admParPacket.pVal[0].typeOfPar   = ADM_UINT8;
    admParPacket.pVal[0].lengthOfPar = 2;
    admParPacket.pVal[0].pVal[0]     = mc_type;   /*multicast type*/
    admParPacket.pVal[0].pVal[1]     = direction; /*stream direction*/ 

    /*pack data*/
    OPL_PACK_UINT16(pBuff,admParPacket.funIndex);
    OPL_PACK_UINT8(pBuff,admParPacket.numOfP);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].typeOfPar);
    OPL_PACK_UINT8(pBuff,admParPacket.pVal[0].lengthOfPar);
    OPL_PACK_BUFF(pBuff,admParPacket.pVal[0].pVal,admParPacket.pVal[0].lengthOfPar);

    len = OPL_API_PACK_HEADER_LENGTH + admParPacket.numOfP*sizeof(ADM_PAR_INFO_t);    
    opl_dump_data(buff,(len),(16));
    
    iRet = msgGetReqPackAndSend(onu_id,buff,len,pR);
    
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.funcIndex);
    OPL_UNPACK_UINT32(pReturnBuff,ackpacket.iRet);
    OPL_UNPACK_UINT16(pReturnBuff,ackpacket.dataLen);

    if (opl_ok == iRet)
    {
        if ((ADM_PON_CLASSIFY_MULTICASET_GET == ackpacket.funcIndex)&&(opl_ok == ackpacket.iRet))
        {
            OPL_UNPACK_UINT8(pReturnBuff,*act);
            iRet = opl_ok;
        }
        else
        {
            iRet = opl_error;
        }
    }
    opl_dbg(("Get Status:%d\r\n",iRet));
    opl_return_parse(iRet);
}

void upimagetest(char *name)
{
    opuint8  buff[1024*1024];
    opuint32 roop;
    opuint8  imageid;
    long size = 0;

    if (NULL == name)
    {
        return opl_error;
    }
    imageid = open(name,O_RDWR|O_CREAT,0); 

    if (imageid < 0)
    {
        return opl_error;
    }
    memset(buff,0x0,1024*1024);
    size = 0;
    size = read(imageid,buff,1024*1024);
    close(imageid);
    opl_sdk_printf(("imagesize = %d\n",size));
    OnuImageBurnEx(0,buff,size,TRUE);

    return ;
}
#endif
