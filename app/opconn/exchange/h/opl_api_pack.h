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
* FILENAME:  opl_api_pack.h
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/exchange/h/opl_api_pack.h#1 $
* $Log:$
*
*
**************************************************************************/
#ifndef __OPL_API_PACK_H__
#define __OPL_API_PACK_H__
#include <opl_type.h>

#define ADM_MAX_FUNC_INDEX          256
#define ADM_MAX_ACK_PAR_INDEX       256
#define ADM_MAX_NUM_OF_PAR          6

#define ADM_MAX_NUM_OF_PARA         20
#define ADM_MAX_LEN_OF_PARA         20
#define ADM_MAC_PARA_LENGTH         1500

#define UART1_CFG                      "/cfg/uart1.conf"

#define UART1_CFG_GENERAL_SEC          "Uart1_sec"

#define UART1_CFG_LINK_EVENT_EN_KEY    "U_linkevent"
#define UART1_CFG_NACK_EVENT_EN_KEY    "U_nackevent"
#define UART1_CFG_REG_EVENT_EN_KEY     "U_regevent"
#define UART1_CFG_OAM_EVENT_EN_KEY     "U_oamevent"
#define UART1_CFG_IGMPEN_KEY           "U_igmpen"
#define UART1_CFG_MACLERN_KEY          "U_maclearn"
enum ADM_TYPE_OF_VAR_e
{
    ADM_UINT8   = 0,
    ADM_UINT16  = 1,
    ADM_UINT32  = 2,
    ADM_INT8    = 3,
    ADM_INT16   = 4,
    ADM_INT32   = 5,
    ADM_STRING  = 6,
    ADM_TYPE_END
};

enum ADM_FUNC_e
{
    ADM_MAC_ID_SET = 0,
    ADM_MAC_ID_GET,
    ADM_LASER_ENABLE_SET,
    ADM_LASER_ENABLE_GET,
    ADM_DEVICE_RESET,
    ADM_VERSION_GET,
    ADM_PON_LINK_GET,
    ADM_PON_UPTIME_GET,
    ADM_EXTEND_MTU_ENABLE,
    ADM_EXTEND_MTU_ENABLE_GET,
    ADM_UNI_CONFIG_SET,
    ADM_UNI_CONFIG_GET,
    ADM_PON_CONNECTION_SET,
    ADM_PON_CONNECTION_GET,
    ADM_PON_STATUS_GET,
    ADM_PON_ENCRYPTION_SET,
    ADM_PON_ENCRYPTION_GET,
    ADM_PON_ENCRYPTION_ACT_KEY_SET,
    ADM_PON_ENCRYPTION_ACT_KEY_GET,
    ADM_EVENT_ENABLE,
    ADM_PON_PORT_STATISTIC_GET,
    ADM_UNI_PORT_STATISTIC_GET,
    ADM_PON_REGISTER_READ,
    ADM_PON_REGISTER_WRITE,
    ADM_PON_LED_SET,
    ADM_PON_IP_MULTICASET_SET,
    ADM_PON_IP_MULTICASET_GET,
    ADM_PON_QUEUE_LIMIT_GET,
    ADM_PON_CLASSIFY_MULTICASET_SET,
    ADM_PON_CLASSIFY_MULTICASET_GET,
    ADM_PON_IGMP_ENABLE_SET,
    ADM_PON_IGMP_ENABLE_GET,
    ADM_PON_LEARNING_ENABLE_SET,
    ADM_PON_LEARNING_ENABLE_GET,
    ADM_PON_BPDU_ACTION_SET,
    ADM_PON_BPDU_ACTION_GET,
    ADM_TEST_3,
    ADM_TEST_4,
};

enum ADM_ACK_e
{
   ADM_MAC_ID_SET_ACK = 0,
   ADM_MAC_ID_GET_ACK,
   ADM_LASER_ENABLE_SET_ACK,
   ADM_LASER_ENABLE_GET_ACK,
   ADM_DEVICE_RESET_ACK,
   ADM_VERSION_GET_ACK,
   ADM_PON_LINK_GET_ACK,
   ADM_PON_UPTIME_GET_ACK,
   ADM_EXTEND_MTU_ENABLE_ACK,
   ADM_EXTEND_MTU_ENABLE_GET_ACK,
   ADM_UNI_CONFIG_SET_ACK,
   ADM_UNI_CONFIG_GET_ACK,
   ADM_PON_CONNECTION_SET_ACK,
   ADM_PON_CONNECTION_GET_ACK,
   ADM_PON_STATUS_GET_ACK,
   ADM_PON_ENCRYPTION_SET_ACK,
   ADM_PON_ENCRYPTION_GET_ACK,
   ADM_PON_ENCRYPTION_ACT_KEY_SET_ACK,
   ADM_PON_ENCRYPTION_ACT_KEY_GET_ACK,
   ADM_EVENT_ENABLE_ACK,
   ADM_PON_PORT_STATISTIC_GET_ACK,
   ADM_UNI_PORT_STATISTIC_GET_ACK,
   ADM_PON_REGISTER_READ_ACK,
   ADM_PON_REGISTER_WRITE_ACK,
   ADM_PON_LED_SET_ACK,
   ADM_PON_IP_MULTICASET_SET_ACK,
   ADM_PON_IP_MULTICASET_GET_ACK,
   ADM_PON_QUEUE_LIMIT_GET_ACK,
   ADM_PON_CLASSIFY_MULTICASET_SET_ACK,
   ADM_PON_CLASSIFY_MULTICASET_GET_ACK,
   ADM_PON_IGMP_ENABLE_SET_ACK,
   ADM_PON_IGMP_ENABLE_GET_ACK,
   ADM_PON_LEARNING_ENABLE_SET_ACK,
   ADM_PON_LEARNING_ENABLE_GET_ACK,
   ADM_PON_BPDU_ACTION_SET_ACK,
   ADM_PON_BPDU_ACTION_GET_ACK,
   ADM_END_ACK,
};

typedef struct ADM_FUNC_TAB_s{
    opuint16      funIndex;
    opuint8       numOfPara;
    PFUNCTION   func;
}__attribute__((packed)) ADM_FUNC_TAB_t;

typedef struct ADM_PAR_INFO_s{
    opuint8 typeOfPar;
    opuint8 lengthOfPar;
    opuint8 pVal[ADM_MAX_LEN_OF_PARA];
}__attribute__((packed)) ADM_PAR_INFO_t;

typedef struct ADM_PAR_PACKET_s{
   opuint16 funIndex;
   opuint8  numOfP;
   ADM_PAR_INFO_t  pVal[ADM_MAX_NUM_OF_PARA];
}__attribute__((packed)) ADM_PAR_PACKET_t;

typedef struct ADM_ACK_PAR_PACKET_s{
    opuint16 funcIndex;
    opuint32 iRet;
    opuint16 dataLen;
    opuint8  pData[20];
}__attribute__((packed)) ADM_ACK_PAR_PACKET_t;

opuint32 AdmGetPtype(opuint16 type,opuint8 *value,opuint32 len);

opint32 AdmDepacket(opuint8 onuId,opuint8 *pBuff);

opint32 AdmPacket(opuint8 onuId,ADM_PAR_PACKET_t *p);


opint32 AdmPonMacIdSet(opuint8 onuId,opuint8 macid[6]);

opint32 AdmPonMacIdGet(opuint8 onuId);

opint32 AdmPonLaserEnableSet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonLaserEnableGet(opuint8 onuId);

opint32 AdmPonDeviceReset(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonVersionGet(opuint8 onuId);

opint32 AdmPonLinkGet(opuint8 onuId);

opint32 AdmPonUptimeGet(opuint8 onuId);

opint32 AdmPonExtendMtuEnableSet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonExtendMtuEnableGet(opuint8 onuId);

opint32 AdmPonUNIConfigSet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonUNIConfigGet(opuint8 onuId);

opint32 AdmPonConnectionSet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonConnectionGet(opuint8 onuId);

opint32 AdmPonStatusGet(opuint8 onuId);

opint32 AdmPonEncryptionSet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonEncryptionGet(opuint8 onuId);

opint32 AdmPonEncryptionActKeySet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonEncryptionActKeyGet(opuint8 onuId);

opint32 AdmPonEventEnable(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonPortStatsGet(opuint8 onuId);

opint32 AdmUNIPortStatsGet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonRegRead(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonRegWrite(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonLedSet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonIPMcSet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonIPMcGet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonQueueLimitGet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonClsMcSet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonClsMcSet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonClsMcGet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonIgmpEnableSet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonIgmpEnableGet(opuint8 onuId);

opint32 AdmPonLearnEnableSet(opuint8 onuId,opuint8 *pVal);

opint32 AdmPonLearnEnableGet(opuint8 onuId);

opint32 AdmTest1(opuint8 a);


opint32 AdmTest2(opuint8 a,opuint8 b);


opint32 AdmTest3(opuint8 a,opuint8 b,opuint8 c);


opint32 AdmTest4(opuint8 a,opuint8 b,opuint8 c,opuint8 d);


opint32 AdmHalTest(opuint8 a);


opint32 AdmHalTestP(opuint8 a,opuint8 *pa,opuint16 b,opuint16 *pb,opuint32 c,opuint32 *pc);

#endif

