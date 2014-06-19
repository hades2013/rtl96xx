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
* FILENAME:  opl_api_pack.c
*
* DESCRIPTION: 
*	pack get/set/oampdu request to frame
*
* Date Created: Apr 30, 2008
*
* Authors(optional): zzhu
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/exchange/src/opl_api_pack.c#1 $
* $Log:$
*
*
**************************************************************************/
#include <opl_type.h>
#include <opl_pal.h>
#include <opl_msg_header.h>
#include <opl_api_pack.h>
#include <opl_api.h>
#include "version.h"
#include "stats.h"
#include "hal.h"
#include "hal_fdb.h"
#include "mc_control.h"

extern opuint8 link_event_enbale     ;
extern opuint8 nack_event_enbale     ;
extern opuint8 register_event_enbale ;
extern opuint8 oamtransmitevent;
extern OPL_CNT_t g_astCntPon[];
extern OPL_CNT_t g_astCntGe[];
ADM_FUNC_TAB_t AdmFuncTabArry[ADM_MAX_FUNC_INDEX] =
{
   {ADM_MAC_ID_SET,                 1,       (PFUNCTION)AdmPonMacIdSet},
   {ADM_MAC_ID_GET,                 0,       (PFUNCTION)AdmPonMacIdGet},
   {ADM_LASER_ENABLE_SET,           1,       (PFUNCTION)AdmPonLaserEnableSet},
   {ADM_LASER_ENABLE_GET,           0,       (PFUNCTION)AdmPonLaserEnableGet}, 
   {ADM_DEVICE_RESET,               1,       (PFUNCTION)AdmPonDeviceReset},
   {ADM_VERSION_GET,                0,       (PFUNCTION)AdmPonVersionGet},
   {ADM_PON_LINK_GET,               0,       (PFUNCTION)AdmPonLinkGet},
   {ADM_PON_UPTIME_GET,             0,       (PFUNCTION)AdmPonUptimeGet},
   {ADM_EXTEND_MTU_ENABLE,          1,       (PFUNCTION)AdmPonExtendMtuEnableSet},
   {ADM_EXTEND_MTU_ENABLE_GET,      0,       (PFUNCTION)AdmPonExtendMtuEnableGet},
   {ADM_UNI_CONFIG_SET,             1,       (PFUNCTION)AdmPonUNIConfigSet},
   {ADM_UNI_CONFIG_GET,             0,       (PFUNCTION)AdmPonUNIConfigGet},
   {ADM_PON_CONNECTION_SET,         1,       (PFUNCTION)AdmPonConnectionSet},
   {ADM_PON_CONNECTION_GET,         0,       (PFUNCTION)AdmPonConnectionGet},
   {ADM_PON_STATUS_GET,             0,       (PFUNCTION)AdmPonStatusGet},
   {ADM_PON_ENCRYPTION_SET,         1,       (PFUNCTION)AdmPonEncryptionSet},
   {ADM_PON_ENCRYPTION_GET,         0,       (PFUNCTION)AdmPonEncryptionGet},
   {ADM_PON_ENCRYPTION_ACT_KEY_SET, 1,       (PFUNCTION)AdmPonEncryptionActKeySet},
   {ADM_PON_ENCRYPTION_ACT_KEY_GET, 0,       (PFUNCTION)AdmPonEncryptionActKeyGet},
   {ADM_EVENT_ENABLE,               1,       (PFUNCTION)AdmPonEventEnable},
   {ADM_PON_PORT_STATISTIC_GET,     0,       (PFUNCTION)AdmPonPortStatsGet},
   {ADM_UNI_PORT_STATISTIC_GET,     1,       (PFUNCTION)AdmUNIPortStatsGet},
   {ADM_PON_REGISTER_READ,          1,       (PFUNCTION)AdmPonRegRead},
   {ADM_PON_REGISTER_WRITE,         1,       (PFUNCTION)AdmPonRegWrite},
   {ADM_PON_LED_SET,                1,       (PFUNCTION)AdmPonLedSet},
   {ADM_PON_IP_MULTICASET_SET,      1,       (PFUNCTION)AdmPonIPMcSet},
   {ADM_PON_IP_MULTICASET_GET,      1,       (PFUNCTION)AdmPonIPMcGet},
   {ADM_PON_QUEUE_LIMIT_GET,        1,       (PFUNCTION)AdmPonQueueLimitGet},
   {ADM_PON_CLASSIFY_MULTICASET_SET,1,       (PFUNCTION)AdmPonClsMcSet},
   {ADM_PON_CLASSIFY_MULTICASET_GET,1,       (PFUNCTION)AdmPonClsMcGet},
   {ADM_PON_IGMP_ENABLE_SET,        1,       (PFUNCTION)AdmPonIgmpEnableSet},
   {ADM_PON_IGMP_ENABLE_GET,        0,       (PFUNCTION)AdmPonIgmpEnableGet},
   {ADM_PON_LEARNING_ENABLE_SET,    1,       (PFUNCTION)AdmPonLearnEnableSet},
   {ADM_PON_LEARNING_ENABLE_GET,    0,       (PFUNCTION)AdmPonLearnEnableGet},
   {ADM_TEST_3,         3,         NULL},
   {ADM_TEST_4,         4,         NULL},
   {0xffff    ,         0,         NULL}
};

ADM_ACK_PAR_PACKET_t AdmAckParaTabArry[ADM_MAX_ACK_PAR_INDEX] = 
{
   {ADM_MAC_ID_SET_ACK,     opl_ok,4,         0},
   {ADM_MAC_ID_GET_ACK,     opl_ok,4,         0},
   {0XFFFF,                 opl_ok,4,         0}
};

opuint32 AdmGetPtype(opuint16 type,opuint8 *value,opuint32 len)
{
    switch(type)
     {
        case ADM_UINT8:
            if(len == 1)
            {
                return *(opuint8*)value;
            }else
            {
                return (opuint32)(opuint8 *)value;
            }
            break;
        case ADM_UINT16:
            if(len == 2)
            {
                return *(opuint16*)value;
            }else
            {
                return (opuint32)(opuint16*)value;
            }
            break;
        case ADM_UINT32:
            if(len == 4)
            {
                return *(opuint32*)value;
            }else
            {
                return (opuint32)(opuint32*)value;
            }
            break;
        case ADM_INT8:
            if(len == 1)
            {
                return *(opint8*)value;
            }else
            {
                return (opuint32)(opint8 *)value;
            }
            break;
        case ADM_INT16:
            if(len == 2)
            {
                return *(opint16*)value;
            }else
            {
                return (opuint32)(opint16*)value;
            }
            break;
        case ADM_INT32:
            if(len == 4)
            {
                return *(opint32*)value;
            }else
            {
                return (opuint32)(opint32*)value;
            }
            break;
        case ADM_STRING:
            return (opuint32)(opuint8 *)value;
            break;
        default:
            return (opuint32)(opuint8 *)value;
            break;
     }
}

opint32 AdmDepacket(opuint8 onuId,opuint8 *pBuff)
{
    opint32 retVal = opl_ok;
    ADM_PAR_PACKET_t admParPacket;
    ADM_PAR_INFO_t   admParInfoArry[ADM_MAX_NUM_OF_PAR];   
    opuint8 pNum = 0;
    opuint16 funindex = 0;

    if(NULL != *pBuff)
    {
        opl_sdk_printf(("input null pointer.\n"));
        return opl_error;
    }

    OPL_UNPACK_UINT16(pBuff, admParPacket.funIndex);
    OPL_UNPACK_UINT8(pBuff,admParPacket.numOfP);

    if(admParPacket.numOfP > ADM_MAX_NUM_OF_PAR)
    {
        opl_sdk_printf(("too many parameter.\n"));
        return opl_error;
    }
    
    for(pNum = 0; pNum < admParPacket.numOfP; pNum++)
    {
        OPL_UNPACK_UINT8(pBuff,admParInfoArry[pNum].typeOfPar);
        OPL_UNPACK_UINT8(pBuff,admParInfoArry[pNum].lengthOfPar);
        OPL_UNPACK_BUFF(pBuff,admParInfoArry[pNum].pVal,admParInfoArry[pNum].lengthOfPar);
    }
    for (funindex = 0;funindex < ADM_MAX_FUNC_INDEX;funindex++)
    {
        if (AdmFuncTabArry[funindex].funIndex == admParPacket.funIndex)
            break;
    }
    if (funindex == ADM_TEST_4) return opl_error;
    if(admParPacket.funIndex < ADM_MAX_FUNC_INDEX)
    {
        switch(AdmFuncTabArry[funindex].numOfPara)
        {
            case 0: 
                retVal = AdmFuncTabArry[funindex].func(onuId);
                break;
            case 1:
                retVal = AdmFuncTabArry[funindex].func(onuId,
                    admParInfoArry[0].pVal
                    );
                break;              
            case 2:
                retVal = AdmFuncTabArry[funindex].func(onuId,
                    admParInfoArry[0].pVal,
                    admParInfoArry[1].pVal
                    );
                break;
            case 3:
                retVal = AdmFuncTabArry[funindex].func(onuId,
                    admParInfoArry[0].pVal,
                    admParInfoArry[1].pVal,
                    admParInfoArry[2].pVal
                    );
                break;
            case 4:
                retVal = AdmFuncTabArry[funindex].func(onuId,
                    admParInfoArry[0].pVal,
                    admParInfoArry[1].pVal,
                    admParInfoArry[2].pVal,
                    admParInfoArry[3].pVal
                    );
                break;
            case 5:
                retVal = AdmFuncTabArry[funindex].func(onuId,
                    admParInfoArry[0].pVal,
                    admParInfoArry[1].pVal,
                    admParInfoArry[2].pVal,
                    admParInfoArry[3].pVal,
                    admParInfoArry[4].pVal
                    );
                break;
            case 6:
                retVal = AdmFuncTabArry[funindex].func(onuId,
                    admParInfoArry[0].pVal,
                    admParInfoArry[1].pVal,
                    admParInfoArry[2].pVal,
                    admParInfoArry[3].pVal,
                    admParInfoArry[4].pVal,
                    admParInfoArry[5].pVal
                    );

            default:
                break;

        }
    }else
    {
        return opl_error;
    }
}

opint32 AdmPacket(opuint8 onuId,ADM_PAR_PACKET_t *p)
{
    return opl_ok;
}

opint32 AdmPonMacIdSet(opuint8 onuId,opuint8 *pMac)
{
    opuint8 macid[6];
    OPL_UNPACK_BUFF(pMac,macid,6);
    
    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    
    opl_sdk_printf(("set mac:%02x %02x %02x %02x %02x %02x \n",
        macid[0],
        macid[1],
        macid[2],
        macid[3],
        macid[4],
        macid[5]));
    odmPonMacIdSet(macid);
    return opl_ok;
}

opint32 AdmPonMacIdGet(opuint8 onuId)
{
    opuint8 macId[6] = {0x0};
    ADM_ACK_PAR_PACKET_t admAckParPacket;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    
    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    odmPonMacIdGet(macId);
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_MAC_ID_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = 6;

    opl_sdk_memcpy(admAckParPacket.pData,macId,admAckParPacket.dataLen);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);

    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    OPL_PACK_BUFF(pBuff,macId,admAckParPacket.dataLen);

    opl_dump_data(buff,len,(16));

    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}
opint32 AdmPonLaserEnableSet(opuint8 onuId,opuint8 *pVal)
{
    opuint8 enable;

    OPL_UNPACK_UINT8(pVal,enable);

    opl_sdk_printf(("%s,%d value = %d\n",__FUNCTION__,__LINE__,enable));

}
opint32 AdmPonLaserEnableGet(opuint8 onuId)
{
    opuint8 enable;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;

    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_LASER_ENABLE_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(enable);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    OPL_PACK_UINT8(pBuff,enable);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmPonDeviceReset(opuint8 onuId,opuint8 *pVal)
{
    opuint8 resettype;

    OPL_UNPACK_UINT8(pVal,resettype);

    opl_sdk_printf(("%s,%d value = %d\n",__FUNCTION__,__LINE__,resettype));
    odmSysReset();
}


opint32 AdmPonVersionGet(opuint8 onuId)
{
    opuint8 enable;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;
    
    OPL_ONU_VERSION_t version;
    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    opl_sdk_memset(&version,0x0,sizeof(OPL_ONU_VERSION_t));

    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));

    version.fw_major_ver = MAJOR_VERSION;
    version.fw_minor_ver = MINOR_VERSION;
    version.fw_build_num = BUILD_NUMBER;
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_VERSION_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(OPL_ONU_VERSION_t);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    /*add version data*/
    OPL_PACK_UINT16(pBuff,version.hw_major_ver);
    OPL_PACK_UINT16(pBuff,version.hw_minor_ver);
    OPL_PACK_UINT16(pBuff,version.fw_major_ver);
    OPL_PACK_UINT16(pBuff,version.fw_minor_ver);
    OPL_PACK_UINT16(pBuff,version.fw_build_num);
    OPL_PACK_UINT16(pBuff,version.fw_maintenance_ver);
    OPL_PACK_UINT32(pBuff,version.oamVersion);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmPonLinkGet(opuint8 onuId)
{
    opuint8 link;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;
    
    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    
    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_PON_LINK_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(link);;

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    /*add link status data*/
    OPL_PACK_UINT8(pBuff,link);


    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmPonUptimeGet(opuint8 onuId)
{

    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;

    opuint64 uptime;
    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);

    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    uptime = vosTimeGet();
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_PON_UPTIME_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(uptime);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    /*add uptime data*/
    OPL_PACK_UINT64(pBuff,uptime);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmPonExtendMtuEnableSet(opuint8 onuId,opuint8 *pVal)
{
    opuint8 enable;

    OPL_UNPACK_UINT8(pVal,enable);

    opl_sdk_printf(("%s,%d value = %d\n",__FUNCTION__,__LINE__,enable));
}

opint32 AdmPonExtendMtuEnableGet(opuint8 onuId)
{
    opuint8 enable;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;

    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);

    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_EXTEND_MTU_ENABLE_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(enable);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    
    OPL_PACK_UINT8(pBuff,enable);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmPonUNIConfigSet(opuint8 onuId,opuint8 *pVal)
{
    ONU_UNI_CONFIG_t cfg;

    opl_sdk_memset(&cfg,0x0,sizeof(cfg));
    OPL_UNPACK_BUFF(pVal,&cfg,sizeof(cfg));

    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
}

opint32 AdmPonUNIConfigGet(opuint8 onuId)
{
    opuint8 enable;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;
    
    ONU_UNI_CONFIG_t cfg;
    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    opl_sdk_memset(&cfg,0x0,sizeof(cfg));

    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_UNI_CONFIG_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(cfg);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    /*add uni cofnig data*/
    OPL_PACK_UINT8(pBuff,cfg.MAC_type);
    OPL_PACK_UINT8(pBuff,cfg.MII_rate);
    OPL_PACK_UINT8(pBuff,cfg.duplex);
    OPL_PACK_UINT8(pBuff,cfg.gmii_master_mode);
    OPL_PACK_UINT8(pBuff,cfg.auto_negotiate);
    OPL_PACK_UINT8(pBuff,cfg.mode);
    OPL_PACK_UINT8(pBuff,cfg.advertise);
    OPL_PACK_UINT8(pBuff,cfg.adv_10_half);
    OPL_PACK_UINT8(pBuff,cfg.adv_10_full);
    OPL_PACK_UINT8(pBuff,cfg.adv_100_half);
    OPL_PACK_UINT8(pBuff,cfg.adv_100_full);
    OPL_PACK_UINT8(pBuff,cfg.adv_100_t4);
    OPL_PACK_UINT8(pBuff,cfg.adv_pause);
    OPL_PACK_UINT8(pBuff,cfg.adv_asym_pause);
    OPL_PACK_UINT8(pBuff,cfg.adv_1000_half);
    OPL_PACK_UINT8(pBuff,cfg.adv_1000_full);
    OPL_PACK_UINT8(pBuff,cfg.adv_port_type);
    OPL_PACK_UINT8(pBuff,cfg.phy_address);
    
    opl_dump_data(buff,len,(16));    
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmPonEncryptionSet(opuint8 onuId,opuint8 *pVal)
{
    opuint8 enctryption;

    OPL_UNPACK_UINT8(pVal,enctryption);

    opl_sdk_printf(("%s,%d value = %d\n",__FUNCTION__,__LINE__,enctryption));
    odmPonSecurityModeSet(enctryption);
}

opint32 AdmPonEncryptionGet(opuint8 onuId)
{
    opuint8 enctryption;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;

    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);

    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));

    odmPonSecurityModeGet(&enctryption);
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_PON_ENCRYPTION_GET;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(enctryption);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    OPL_PACK_UINT8(pBuff,enctryption);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmPonStatusGet(opuint8 onuId)
{
   
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;

    OPL_ONU_STATUS_t     status;
    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    opl_sdk_memset(&status,0x0,sizeof(status));
   
    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_PON_STATUS_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(status);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    
    OPL_PACK_UINT8(pBuff,status.connection);
    OPL_PACK_UINT8(pBuff,status.oam_link_established);
    OPL_PACK_UINT8(pBuff,status.authorization_state);
    OPL_PACK_UINT8(pBuff,status.PON_Loopback);
    OPL_PACK_BUFF(pBuff,status.mac_addr,6);
    OPL_PACK_UINT8(pBuff,status.ONU_llid);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmPonConnectionSet(opuint8 onuId,opuint8 *pVal)
{
    opuint8 connection;

    OPL_UNPACK_UINT8(pVal,connection);

    opl_sdk_printf(("%s,%d value = %d\n",__FUNCTION__,__LINE__,connection));
}

opint32 AdmPonConnectionGet(opuint8 onuId)
{
    opuint8 connection;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;

    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);

    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_PON_CONNECTION_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(connection);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    OPL_PACK_UINT8(pBuff,connection);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmPonEncryptionActKeySet(opuint8 onuId,opuint8 *pVal)
{
    opuint8 key_seq;
    OPL_ENCRYPTION_KEY_t key;

    OPL_UNPACK_UINT8(pVal,key_seq);
    OPL_UNPACK_UINT32(pVal,key.key[0]);
    OPL_UNPACK_UINT32(pVal,key.key[1]);
    OPL_UNPACK_UINT32(pVal,key.key[2]);
    OPL_UNPACK_UINT32(pVal,key.key[3]);

    opl_sdk_printf(("%s,%d value = %d\n",__FUNCTION__,__LINE__,key));

    odmPonScbAesKeySet(key_seq,(opuint8 *)&key,0x7);
}

opint32 AdmPonEncryptionActKeyGet(opuint8 onuId)
{
    opuint8 key_seq;
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;

    ADM_ACK_PAR_PACKET_t admAckParPacket;
    OPL_ENCRYPTION_KEY_t key;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);

    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_PON_ENCRYPTION_ACT_KEY_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(key_seq);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    OPL_PACK_UINT8(pBuff,key_seq);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmPonEventEnable(opuint8 onuId,opuint8 *pVal)
{
    opuint8                enable;
    OPL_ALARM_EVENT_TYPE_e type;

    OPL_UNPACK_UINT8(pVal,type);
    OPL_UNPACK_UINT8(pVal,enable);

    opl_sdk_printf(("%s,%d  type = %d enable = %d\n",__FUNCTION__,__LINE__,type,enable));
    switch (type)
    {
        case OPL_LINK_FAULT_ALARM:
            link_event_enbale = enable;
            vosConfigUInt32Set(UART1_CFG,UART1_CFG_GENERAL_SEC,UART1_CFG_LINK_EVENT_EN_KEY,enable);
            break;
        case OPL_MPCP_NACK_EVENT:
            nack_event_enbale = enable;
            vosConfigUInt32Set(UART1_CFG,UART1_CFG_GENERAL_SEC,UART1_CFG_NACK_EVENT_EN_KEY,enable);
            break;
        case OPL_AUTHORIZATION_EVENT:
            register_event_enbale = enable;
            vosConfigUInt32Set(UART1_CFG,UART1_CFG_GENERAL_SEC,UART1_CFG_REG_EVENT_EN_KEY,enable);
            break;
        case OPL_OAM_RECEIVE_EVENT:
            oamtransmitevent = enable;
            vosConfigUInt32Set(UART1_CFG,UART1_CFG_GENERAL_SEC,UART1_CFG_OAM_EVENT_EN_KEY,enable);
            break;
        default :
            break;
    }


    return opl_ok;
}

opint32 AdmPonPortStatsGet(opuint8 onuId)
{

    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;
    opuint8 ucCnt;

    opuint32 *uiRegVal;
    OPL_ONU_STATISTIC_t  stats;
    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    opl_sdk_memset(&stats,0x0,sizeof(stats));
    
    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_PON_PORT_STATISTIC_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(stats);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);

    uiRegVal = (opuint32 *)&(stats);
    for (ucCnt=0; ucCnt<DAL_CNT_GE_NUM; ucCnt++)
    {
        if ((ucCnt == 13)||(36 == ucCnt)||(37 == ucCnt)
        ||(38 == ucCnt)||(40 == ucCnt)) continue;
        oplRegRead(g_astCntPon[ucCnt].usId*4, uiRegVal);
        uiRegVal++;
    }
    OPL_PACK_UINT32(pBuff,stats.TxTotalPkt);
    OPL_PACK_UINT32(pBuff,stats.TxBroad);
    OPL_PACK_UINT32(pBuff,stats.TxMulti);
    OPL_PACK_UINT32(pBuff,stats.TxPause);
    OPL_PACK_UINT32(pBuff,stats.TxMacCtrl);
    OPL_PACK_UINT32(pBuff,stats.Tx64byte);
    OPL_PACK_UINT32(pBuff,stats.Tx65to127);
    OPL_PACK_UINT32(pBuff,stats.Tx128to255);
    OPL_PACK_UINT32(pBuff,stats.Tx256to511);
    OPL_PACK_UINT32(pBuff,stats.Tx512to1023);
    OPL_PACK_UINT32(pBuff,stats.Tx1024to1518);
    OPL_PACK_UINT32(pBuff,stats.TxByte);
    OPL_PACK_UINT32(pBuff,stats.Tx1519toMTU);
    
    OPL_PACK_UINT32(pBuff,stats.RxGood);
    OPL_PACK_UINT32(pBuff,stats.RxDrop);
    OPL_PACK_UINT32(pBuff,stats.RxCRCAlignErr);
    OPL_PACK_UINT32(pBuff,stats.RxLenErr);
    OPL_PACK_UINT32(pBuff,stats.RxFragments);
    OPL_PACK_UINT32(pBuff,stats.RxJabbers);
    OPL_PACK_UINT32(pBuff,stats.RxTotalPks);
    OPL_PACK_UINT32(pBuff,stats.RxBroad);
    OPL_PACK_UINT32(pBuff,stats.RxMulti);
    OPL_PACK_UINT32(pBuff,stats.RxPause);
    OPL_PACK_UINT32(pBuff,stats.RxErrMacCtrl);

    OPL_PACK_UINT32(pBuff,stats.RxMACCtrl);
    OPL_PACK_UINT32(pBuff,stats.Rxshort);
    OPL_PACK_UINT32(pBuff,stats.Rxlong);
    OPL_PACK_UINT32(pBuff,stats.Rx64byte);
    OPL_PACK_UINT32(pBuff,stats.Rx65to127);
    OPL_PACK_UINT32(pBuff,stats.Rx128to255);
    OPL_PACK_UINT32(pBuff,stats.Rx256to511);
    OPL_PACK_UINT32(pBuff,stats.Rx512to1023);
    OPL_PACK_UINT32(pBuff,stats.Rx1024to1518);
    OPL_PACK_UINT32(pBuff,stats.Rx1519toMTU);
    OPL_PACK_UINT32(pBuff,stats.RxByte);
    OPL_PACK_UINT32(pBuff,stats.RxL3Multi);
    OPL_PACK_UINT32(pBuff,stats.RxRxERPkts);
    OPL_PACK_UINT32(pBuff,stats.RxIPGErr);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmUNIPortStatsGet(opuint8 onuId,opuint8 *pVal)
{

    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;
    opuint8 port;
    opuint8 ucCnt;


    opuint32 *uiRegVal;
    OPL_ONU_STATISTIC_t  stats;
    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    opl_sdk_memset(&stats,0x0,sizeof(stats));
    
    OPL_UNPACK_UINT8(pVal,port);

    opl_sdk_printf(("%s,%d  port = %d\n",__FUNCTION__,__LINE__,port));
    
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_UNI_PORT_STATISTIC_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(stats);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    uiRegVal = (opuint32 *)&(stats);
    for (ucCnt=0; ucCnt<DAL_CNT_GE_NUM; ucCnt++)
    {
        oplRegRead(g_astCntGe[ucCnt].usId*4, uiRegVal);
        uiRegVal++;
    }
    OPL_PACK_UINT32(pBuff,stats.TxTotalPkt);
    OPL_PACK_UINT32(pBuff,stats.TxBroad);
    OPL_PACK_UINT32(pBuff,stats.TxMulti);
    OPL_PACK_UINT32(pBuff,stats.TxPause);
    OPL_PACK_UINT32(pBuff,stats.TxMacCtrl);
    OPL_PACK_UINT32(pBuff,stats.Tx64byte);
    OPL_PACK_UINT32(pBuff,stats.Tx65to127);
    OPL_PACK_UINT32(pBuff,stats.Tx128to255);
    OPL_PACK_UINT32(pBuff,stats.Tx256to511);
    OPL_PACK_UINT32(pBuff,stats.Tx512to1023);
    OPL_PACK_UINT32(pBuff,stats.Tx1024to1518);
    OPL_PACK_UINT32(pBuff,stats.TxByte);
    OPL_PACK_UINT32(pBuff,stats.Tx1519toMTU);
    
    OPL_PACK_UINT32(pBuff,stats.RxGood);
    OPL_PACK_UINT32(pBuff,stats.RxDrop);
    OPL_PACK_UINT32(pBuff,stats.RxCRCAlignErr);
    OPL_PACK_UINT32(pBuff,stats.RxLenErr);
    OPL_PACK_UINT32(pBuff,stats.RxFragments);
    OPL_PACK_UINT32(pBuff,stats.RxJabbers);
    OPL_PACK_UINT32(pBuff,stats.RxTotalPks);
    OPL_PACK_UINT32(pBuff,stats.RxBroad);
    OPL_PACK_UINT32(pBuff,stats.RxMulti);
    OPL_PACK_UINT32(pBuff,stats.RxPause);
    OPL_PACK_UINT32(pBuff,stats.RxErrMacCtrl);

    OPL_PACK_UINT32(pBuff,stats.RxMACCtrl);
    OPL_PACK_UINT32(pBuff,stats.Rxshort);
    OPL_PACK_UINT32(pBuff,stats.Rxlong);
    OPL_PACK_UINT32(pBuff,stats.Rx64byte);
    OPL_PACK_UINT32(pBuff,stats.Rx65to127);
    OPL_PACK_UINT32(pBuff,stats.Rx128to255);
    OPL_PACK_UINT32(pBuff,stats.Rx256to511);
    OPL_PACK_UINT32(pBuff,stats.Rx512to1023);
    OPL_PACK_UINT32(pBuff,stats.Rx1024to1518);
    OPL_PACK_UINT32(pBuff,stats.Rx1519toMTU);
    OPL_PACK_UINT32(pBuff,stats.RxByte);
    OPL_PACK_UINT32(pBuff,stats.RxL3Multi);
    OPL_PACK_UINT32(pBuff,stats.RxRxERPkts);
    OPL_PACK_UINT32(pBuff,stats.RxIPGErr);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}


opint32 AdmPonRegRead(opuint8 onuId,opuint8 *pVal)
{ 
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;
    
    opuint32 reg,value;
    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);

    OPL_UNPACK_UINT32(pVal, reg);
    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    opl_sdk_printf(("regaddr=0x%0x \n",reg));
    oplRegRead(reg*4,&value);
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_PON_REGISTER_READ_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(value);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    OPL_PACK_UINT32(pBuff,value);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmPonRegWrite(opuint8 onuId,opuint8 *pVal)
{
    opuint32  reg,value;

    OPL_UNPACK_UINT32(pVal,reg);
    OPL_UNPACK_UINT32(pVal,value);

    opl_sdk_printf(("%s,%d  reg = %d value = %d\n",__FUNCTION__,__LINE__,reg,value));
    owr(reg,value);
    
    return opl_ok;
}

opint32 AdmPonLedSet(opuint8 onuId,opuint8 *pVal)
{
    OPL_LED_e led;
    OPL_LED_ACT_e act;

    OPL_UNPACK_UINT8(pVal,led);
    OPL_UNPACK_UINT8(pVal,act);

    opl_sdk_printf(("%s,%d  led = %d act = %d\n",__FUNCTION__,__LINE__,led,act));
}

opint32 AdmPonIPMcSet(opuint8 onuId,opuint8 *pVal)
{
    OPL_ONU_DIRECTION_e direction;
    OPBOOL enable;

    OPL_UNPACK_UINT8(pVal,direction);
    OPL_UNPACK_UINT8(pVal,enable);

    opl_sdk_printf(("%s,%d  direction = %d enable = %d\n",__FUNCTION__,__LINE__,direction,enable));
}
opint32 AdmPonIPMcGet(opuint8 onuId,opuint8 *pVal)
{ 
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;
    
    OPBOOL enable;
    OPL_ONU_DIRECTION_e direction;
    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);

    OPL_UNPACK_UINT8(pVal,direction);
    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    opl_sdk_printf(("direction=%d \n",direction));
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_PON_IP_MULTICASET_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(enable);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    OPL_PACK_UINT8(pBuff,enable);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmPonQueueLimitGet(opuint8 onuId,opuint8 *pVal)
{ 
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;
    
    OPL_ONU_PQUEUE_e queue;
    opuint8 pri;
    opuint16 limit;
    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);

    OPL_UNPACK_UINT8(pVal,queue);
    OPL_UNPACK_UINT8(pVal,pri);
    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    opl_sdk_printf(("queue=%d pri = %d\n",queue,pri));
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_PON_QUEUE_LIMIT_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = sizeof(limit);

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,limit);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmPonClsMcSet(opuint8 onuId,opuint8 *pVal)
{
    OPL_ONU_MULTICAST_TYPE_e mc_type;
    OPL_ONU_DIRECTION_e direction;
    OPL_ONU_FOREARDING_ACTION_e act;

    OPL_UNPACK_UINT8(pVal,mc_type);
    OPL_UNPACK_UINT8(pVal,direction);
    OPL_UNPACK_UINT8(pVal,act);

    opl_sdk_printf(("%s,%d  type = %d direction = %d act = %d \n",__FUNCTION__,__LINE__,mc_type,direction,act));
}

opint32 AdmPonClsMcGet(opuint8 onuId,opuint8 *pVal)
{ 
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;
    
    OPL_ONU_MULTICAST_TYPE_e mc_type;
    OPL_ONU_DIRECTION_e direction;
    OPL_ONU_FOREARDING_ACTION_e act;
    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);

    OPL_UNPACK_UINT8(pVal,mc_type);
    OPL_UNPACK_UINT8(pVal,direction);
    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    opl_sdk_printf(("mc_type=%d direction = %d\n",mc_type,direction));
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_PON_CLASSIFY_MULTICASET_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = 1;

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    OPL_PACK_UINT8(pBuff,act);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}
extern struct multicast_igmp_cfg igmp_cfg;
opint32 AdmPonIgmpEnableSet(opuint8 onuId,opuint8 *pVal)
{
    opuint8 enable;

    OPL_UNPACK_UINT8(pVal,enable);

    opl_sdk_printf(("%s,%d  enable:%d \n",__FUNCTION__,__LINE__,enable));
    switch (enable)
    {
        case opl_true:
            dalMulticastEnable();
            break;
        case opl_false:
            dalMulticastDisable();
            break;
        default:
            return opl_error;
    }
    
    //igmp_cfg.igmp_mode = enable;
    vosConfigUInt32Set(CFGFILE_MULTICAST, CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
        CFGKEY_MULTICAST_IGMP_MANAGEMENT_IGMP_MODE, enable);
    
    return opl_ok;
}
opint32 AdmPonIgmpEnableGet(opuint8 onuId)
{ 
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;
    
    opuint8 enable = opl_false;
    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);


    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));
    enable = vosConfigUInt32Set(CFGFILE_MULTICAST, CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
        CFGKEY_MULTICAST_IGMP_MANAGEMENT_IGMP_MODE, opl_false);
   
    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_PON_IGMP_ENABLE_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = 1;

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    OPL_PACK_UINT8(pBuff,enable);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmPonLearnEnableSet(opuint8 onuId,opuint8 *pVal)
{
    opuint8 enable;

    OPL_UNPACK_UINT8(pVal,enable);
    opl_sdk_printf(("%s,%d  enable:%d \n",__FUNCTION__,__LINE__,enable));
    switch (enable)
    {
        case opl_true:
            odmPortLearningEnableSet(1,opl_true);
            break;
        case opl_false:
            odmPortLearningEnableSet(1,opl_false);
            break;
        default:
            return opl_error;
    }
    return opl_ok;
}

opint32 AdmPonLearnEnableGet(opuint8 onuId)
{ 
    opuint8 buff[OPL_API_PACK_BUFF_LENGTH] = {0x0};
    opuint8 *pBuff = buff;
    opuint8 len= 0;
    
    opuint32 enable;
    ADM_ACK_PAR_PACKET_t admAckParPacket;

    opl_sdk_memset(&admAckParPacket,0x0,sizeof(ADM_ACK_PAR_PACKET_t));
    opl_sdk_memset(buff,0x00,OPL_API_PACK_BUFF_LENGTH);
    
    odmPortLearningEnableGet(1,&enable);
    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__));

    /*get local info,and send to master cpu*/
    admAckParPacket.funcIndex = ADM_PON_LEARNING_ENABLE_GET_ACK;
    admAckParPacket.iRet      = opl_ok;
    admAckParPacket.dataLen   = 1;

    len = (sizeof(opuint16)+sizeof(opuint32)+sizeof(opuint16)+admAckParPacket.dataLen);
    OPL_PACK_UINT16(pBuff,admAckParPacket.funcIndex);
    OPL_PACK_UINT32(pBuff,admAckParPacket.iRet);
    OPL_PACK_UINT16(pBuff,admAckParPacket.dataLen);
    OPL_PACK_UINT8(pBuff,enable);

    opl_dump_data(buff,len,(16));
    msgGetAckPackAndSend(onuId,buff,len);
    
    return opl_ok;
}

opint32 AdmTest1(opuint8 a)
{
    printf("%s %d\n",__FUNCTION__,__LINE__);

    printf("a = %d\n",a);

    return opl_ok;
}

opint32 AdmTest2(opuint8 a,opuint8 b)
{
    printf("%s %d\n",__FUNCTION__,__LINE__);

    printf("a = %d b = %d\n",a,b);

    return opl_ok;
}

opint32 AdmTest3(opuint8 a,opuint8 b,opuint8 c)
{
    printf("%s %d\n",__FUNCTION__,__LINE__);

    printf("a = %d b = %d c = %d\n",a,b,c);

    return opl_ok;
}

opint32 AdmTest4(opuint8 a,opuint8 b,opuint8 c,opuint8 d)
{
    printf("%s %d\n",__FUNCTION__,__LINE__);

    printf("a = %d b = %d c = %d d = %d\n",a,b,c,d);

    return opl_ok;
}

opint32 AdmHalTest(opuint8 a)
{
    opint32 retVal = opl_ok;
    
    switch(a)
    {

        case 1:
            retVal = AdmFuncTabArry[0].func(1);
            break;              
        case 2:
            retVal = AdmFuncTabArry[1].func(1,2);
            break;
        case 3:
            retVal = AdmFuncTabArry[2].func(1,2,3);
            break;
        case 4:
            retVal = AdmFuncTabArry[3].func(1,2,3,4);
            break;
        case 5:
            break;
        default:
            break;
    }
}

opint32 AdmHalTestP(opuint8 a,opuint8 *pa,opuint16 b,opuint16 *pb,opuint32 c,opuint32 *pc)
{
    printf("a = 0x%x\n",a);
    printf("pa = 0x%x 0x%x\n",*pa,*(pa+1));

    printf("b = 0x%x\n",b);
    printf("pb = 0x%x 0x%x\n",*pb,*(pb+1));


    printf("c = 0x%x\n",a);
    printf("pc = 0x%x 0x%x\n",*pc,*(pc+1));
    
}
opuint32 AdmPonUart1CfgInit()
{
    opuint8 enable;
    HAL_API_FDB_MAC_LEARN_S stPara;
    OPL_API_DATA_t stData;
    
    link_event_enbale = vosConfigUInt32Get(UART1_CFG,UART1_CFG_GENERAL_SEC,UART1_CFG_LINK_EVENT_EN_KEY,opl_true);

    nack_event_enbale = vosConfigUInt32Set(UART1_CFG,UART1_CFG_GENERAL_SEC,UART1_CFG_NACK_EVENT_EN_KEY,opl_true);

    register_event_enbale = vosConfigUInt32Set(UART1_CFG,UART1_CFG_GENERAL_SEC,UART1_CFG_REG_EVENT_EN_KEY,opl_true);

    oamtransmitevent = vosConfigUInt32Set(UART1_CFG,UART1_CFG_GENERAL_SEC,UART1_CFG_OAM_EVENT_EN_KEY,opl_true);

    enable = vosConfigUInt32Set(CFGFILE_MULTICAST, CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
        CFGKEY_MULTICAST_IGMP_MANAGEMENT_IGMP_MODE, opl_false);

    if (opl_true == enable)
        dalMulticastEnable();
    else
        dalMulticastDisable();

    return opl_ok;
}
