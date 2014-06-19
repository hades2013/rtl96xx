/*****************************************************************************
           
*****************************************************************************/
#ifndef __LW_EOAM_H__
#define __LW_EOAM_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "lw_config.h"
/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define EOAM_ENABLE                  1
#define EOAM_DISABLE                 2

#define EOAM_ETH_HDR_SIZE           18      /* ��ͨOAMPDUͷ����        */
#define EOAM_PAD_MIN_SIZE           42      /* OAM�������ݲ�����С���� */
#define EOAM_PAD_MAX_SIZE           1496    /* OAM�������ݲ�����󳤶� */
#define EOAM_PAD_HDR_SIZE           11      /* EOAMPDU�����Զ���ͷ���� */
#define EOAM_PAD_EVT_HDR_SIZE       6       /* �¼��ϱ�����ͷ������    */
#define EOAM_PAD_DATA_MIN_SIZE      (EOAM_PAD_MIN_SIZE - EOAM_PAD_HDR_SIZE) /* EOAMPDU������С���ݳ���: 31   */
#define EOAM_PAD_DATA_MAX_SIZE      (EOAM_PAD_MAX_SIZE - EOAM_PAD_HDR_SIZE) /* EOAMPDU����������ݳ���: 1485 */

#define EOAM_PKT_MAX_SIZE          (EOAM_PAD_MAX_SIZE + EOAM_ETH_HDR_SIZE)
#define EOAM_PKT_MIN_SIZE          (EOAM_PAD_MIN_SIZE + EOAM_ETH_HDR_SIZE)

/* EOAM���Ĳ������� */
#define EOAM_GET_REQUEST            0x01
#define EOAM_GET_RESPONSE           0x02
#define EOAM_SET_REQUEST            0x03
#define EOAM_SET_RESPONSE           0x04
#define EOAM_EVENT                  0x05
#define EOAM_USER_DATAGRAM          0x06

/* EOAM�����ֶ�ƫ��λ�� */
#define EOAM_ETH_OFFSET_DA          0       /* Ŀ��Mac��ַƫ�� */
#define EOAM_ETH_OFFSET_SA          6       /* ԴMac��ַƫ�� */
#define EOAM_ETH_OFFSET_TYPE        12      /* Type��ַƫ�� */
#define EOAM_ETH_OFFSET_SUBTYPE     14      /* Sub type��ַƫ�� */
#define EOAM_ETH_OFFSET_FLAGS       15      /* FLAG��ַƫ�� */
#define EOAM_ETH_OFFSET_CODE        17      /* OAM Code��ַƫ�� */

/* EOAM PAD�ֶ�ƫ��λ�� */
#define EOAM_PAD_OFFSET_OUI         0       /* OUI��ַƫ�� */
#define EOAM_PAD_OFFSET_PDUTYPE     3       /* PDU Type��ַƫ�� */
#define EOAM_PAD_OFFSET_REQID       4       /* Reqest Id��ַƫ�� */
#define EOAM_PAD_OFFSET_ERRCODE     6       /* ONU��������ֵ��ַƫ�� */
#define EOAM_PAD_OFFSET_CTRL        7       /* Request ������ַƫ�� */
#define EOAM_PAD_OFFSET_DATA        11      /* ���ݲ��ֵ�ַƫ�� */

#define EOAM_BATCH_CFG_TLV_OFF      6       /* ���������������ñ��������е�ƫ�� */
/* �Ӷ˿�link״̬ */
#define LINK_IS_UP                  1
#define LINK_IS_DOWN                0

/* �·������� */
#define EOAM_OP_ADD                 1 
#define EOAM_OP_DEL                 2

#define     IP_NORMAL        0x00
#define     IP_ALL_ZERO      0x01
#define     IP_BROADCAST     0x02
#define     IP_HOST          0x03
#define     IP_MULTICAST     0x04
#define     IP_EXPERIMENTAL  0x05
#define     IP_SELF          0x06
#define     IP_FIRST_ZERO    0x07
#define     IP_ADDR_LEN      4

/* PASONU_transmit_frame������������͵�OAM���� */
#define MAX_PERMIT_OAM_LEN          100

#define BATCH_HEAD_SIZE 6 

/*  ONU��ģ�����͵Ķ��壬����OLTҪ���� */
#define _1000M_SFF_PX10_U            4
#define _1000M_SFF_PX20_U            5

/* OUI�ĳ��� */
#define EOAM_OUI_SIZE 3  

/* ����EOAM��������break */
#define CALL_EOAM_FUNC_AND_BREAK(Type, EoamFunc)\
    case Type :\
        ulRet = EoamFunc(pucPduDataBuf, usPduDataLen);\
        break;

/* EOAM������䵽42�ֽ� */
#define FILL_MIN_PAD_DATA(usPadLen) ((usPadLen) = (usPadLen) < EOAM_PAD_MIN_SIZE ? EOAM_PAD_MIN_SIZE : (usPadLen))

#define NTOHL(x) ntohl((UINT32)(x))
#define NTOHS(x) ntohs((UINT16)(x))
#define HTONL(x) htonl((UINT32)(x))
#define HTONS(x) htons((UINT16)(x))

#define OAM_H3C_SUPPORT 1

/* ONU������������� */
#define    ONU_START_SEND_SOFTWARE  0x01     /* ֪ͨONU��һ֡Ҫ����ONU��� */
#define    ONU_SENDING_SOFTWARE     0x02     /* ����ONU��� */
#define    ONU_STOP_SEND_SOFTWARE   0x03     /* ֹͣ����ONU������Ӵ�У���֪ͨONU��ʼ��Ƭ */

/*----------------------------------------------*
 * �����ӿڣ����Ǻ�����ֲ������ģ����           *
 *----------------------------------------------*/
#define HARDWARE_REV "A"

typedef struct tagULong_8
{
    UINT32 ulData[2];  
}ULONG_8;

/* ����ֵ���� */
#define VOS_OK                          0
#define VOS_ERR                         1
#define VOS_NOT_SUPPORT                 4
#define VOS_FULL                        5
#define VOS_EXIST                       6
#define VOS_NO_MATCH                    10
#define VOS_FINISH_OK                   11
#define VOS_FINISH_ERR                  12
#define VOS_CONTINUE                    21
#define VOS_ONU_BUSY                    0x55
#define VOS_NO_RESOURCE                 0x56
#define VOS_NOT_FOUND                   0xFFFF
#define VOS_FILE_TOO_LARGE              36
#define VOS_TIMEOUT                     37


#define VRP_YES                         1
#define VRP_NO                          0

#define _H3C_DEBUG

#if defined(_H3C_DEBUG)
#define EOAM_ASSERT(exp)     OAM_Assert((SHORT)(exp), __FILE__, __LINE__)
#else
#define EOAM_ASSERT(exp)
#endif

extern void print2console(char *fmt, ...);

#define EOAM_PRINT(fmt, args...) \
    do\
    {\
        print2console(fmt ,##args);\
    }while(0)

#ifndef BOOL_TRUE
#define BOOL_TRUE  ((BOOL_T)1)
#endif

#ifndef STATIC
#define STATIC static
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif


#ifndef BOOL_FALSE
#define BOOL_FALSE ((BOOL_T)0)
#endif

#define COMPARE_EQUAL       0
#define COMPARE_NOT_EQUAL   1

#define VRP_MODULE_SMLK VRP_NO


#define MAC_ADDR_LEN 6

#if 0 
#if defined(CONFIG_PRODUCT_EM200)
#define MAX_PORT_NUM            1
#else
#define MAX_PORT_NUM            4   /* MAX_SWITCH_PORTS */
#endif

/* UNI�˿���Ŀ */
#define MIN_UNI_PORT_ID         1
#define MAX_UNI_PORT_ID         MAX_PORT_NUM
#endif
/* ONU����ģʽ�궨�� */
#define ONU_WORK_MODE_C_MODE        100
#define ONU_WORK_MODE_H_MODE        101

#define NUMBER_OF_DOT1P             8

/* ���ȼ����и��� */
#define NUMBER_OF_SWITCH_PRI_QUEUE  4

/*�豸���������Ϣ�����ͺ궨�� */
#define IC_INFO_LOG            1
#define IC_INFO_TRAP           2
#define IC_INFO_DEBUG          3
#define IC_INFO_KIND           3

#define IC_LEVEL_TOP           0
#define IC_LEVEL_EMERG         0
#define IC_LEVEL_ALERT         1
#define IC_LEVEL_CRIT          2
#define IC_LEVEL_ERR           3
#define IC_LEVEL_WARN          4
#define IC_LEVEL_NOTICE        5
#define IC_LEVEL_INFO          6
#define IC_LEVEL_DEBUG         7
#define IC_MAX_LEVEL_NUM       7
#define IC_LEVEL_INVALID       0xFF

#define MAX_SEND_INFO_SIZE      128
#define TRAP_LEAF_SIZE          50

/* �˿ڽ�ɫRole */
#define RSTP_ROLE_DISABLEDPORT      0 /* �˿ڲ����� */
#define RSTP_ROLE_ROOTPORT          1 /* ���˿� */
#define RSTP_ROLE_DESIGNATEDPORT    2 /* ָ���˿� */
#define RSTP_ROLE_ALTERNATEPORT     3 /* ��ѡ�˿� */
#define RSTP_ROLE_BACKUPPORT        4 /* ���ö˿� */

#ifndef RSTP_LOGID_PFWD
#define RSTP_LOGID_PFWD             5 
#endif
#ifndef RSTP_LOGID_PDISC
#define RSTP_LOGID_PDISC            6 
#endif

/*
 * NTOHL: 32λ �����ֽ��� -> �����ֽ���
 * NTOHS: 16λ �����ֽ��� -> �����ֽ���
 * HTONL: 32λ �����ֽ��� -> �����ֽ���
 * HTONS: 16λ �����ֽ��� -> �����ֽ���
*/

/*lint -save -e572 */

#define HLTOBUF(buf, x)\
    do {\
        *((buf) + 0) = (UCHAR)(((ULONG)(x) & 0xFF000000) >> 24);\
        *((buf) + 1) = (UCHAR)(((ULONG)(x) & 0x00FF0000) >> 16);\
        *((buf) + 2) = (UCHAR)(((ULONG)(x) & 0x0000FF00) >> 8 );\
        *((buf) + 3) = (UCHAR)(((ULONG)(x) & 0x000000FF) >> 0 );\
    } while (0)
    
#define HSTOBUF(buf, x)\
    do {\
        *((buf) + 0) = (UCHAR)(((USHORT)(x) & 0xFF00) >> 8);\
        *((buf) + 1) = (UCHAR)(((USHORT)(x) & 0x00FF) >> 0);\
    } while (0)

#define BUFTOHL(buf, x)  \
    do { x =  (buf)[3] + \
          ((buf)[2] << 8) + \
          ((buf)[1] << 16) + \
          ((buf)[0] << 24);  \
    } while (0)

#define BUFTOTHL(buf, x, type)  \
    do { x =  (type)((buf)[3] + \
          ((buf)[2] << 8) + \
          ((buf)[1] << 16) + \
          ((buf)[0] << 24));  \
    } while (0)

#define BUFTOHS(buf, x)  \
    do { x =  (buf)[1] + \
          ((buf)[0] << 8); \
    } while (0) 

#define BUFTOTHS(buf, x, type)  \
    do { x =  (type)((buf)[1] + \
          ((buf)[0] << 8)); \
    } while (0)

#define MAC_COPY(pDest, pSrc) vosMemCpy(pDest, pSrc, MAC_ADDR_LEN)

#define STR_LENGTH(pcStr) (ULONG)(strlen((CHAR *)pcStr))

#define EOAMPORT_2_LPORT(ucEPort) ((ucEPort) + 1)
#define LPORT_2_EOAMPORT(ucLPort) ((ucLPort) - 1)

#define     DEFAULT_IP      0xC0A800F0
#define     DEFAULT_MASK    0xFFFFFF00
#define     DEFAULT_GW      0x00000000

#define DEFAULT_SVLAN_MEM_CFG_ENTRY 63
#define DEFAULT_SVLAN_ID            4095

/* VLAN ID��Χ */
#define VLAN_ID_0           0
#define MIN_VLAN_ID         1
#define MAX_VLAN_ID         4094
#define MAX_VLAN_NUM        4095

//#define PORT_LINKUP     1
//#define PORT_LINKDOWN   0

/*UNI��STP״̬����,added for onu-stp-trap
disabled(1),discarding(2),learning(3),forwarding(4)*/
typedef enum tagEPON_SUBPORT_STPSTATUS
{
    EPON_SUBPORT_STP_DISABLED = 1,
    EPON_SUBPORT_STP_DISCARDING,
    EPON_SUBPORT_STP_LEARNING,
    EPON_SUBPORT_STP_FORWARDING,
    EPON_SUBPORT_STP_BUTT
} EPON_SUBPORT_STPSTATUS_E;

/*PON�ڸ澯״̬����,added for onu-pon-trap
AlwaysLaserOn(1),AlwaysLaserRecover(2)*/
typedef enum tagEPON_ALERT_PONSTATUS
{
    EPON_ALERT_PON_ALWAYS_LASER_ON = 1,
    EPON_ALERT_PON_ALWAYS_LASER_RECOVER,
    EPON_ALERT_PON_BUTT
} EPON_ALERT_PONSTATUS_E;

/*ONUͨ��ONUר��ͨ���ϱ���trap������������Ҫ����Ϣ�ܾ��壬����ÿ��trap��Ϣ��Ҫ�����ṹ�壬
Ҳ����data�ֶε�һ��ULONGΪEOAM_ONUTRAP_TYPE_E,֮��Ϊ��Ӧtrap���ϱ��ṹ*/
typedef enum tagEOAM_ONUTRAP_TYPE
{
    EOAM_EVENT_ONUTRAP_UNI_UPDOWN,  /*UNI��up��down�ϱ���trap*/
    EOAM_EVENT_ONUTRAP_STPSTATE,    /* PON��UNI��STP״̬�ϱ� */
    EOAM_EVENT_ONUTRAP_ALAWAYS_LASER_ON = 3, /* PON������״̬trap�ϱ� */
    EOAM_EVENT_ONUTRAP_BUTT
}EOAM_ONUTRAP_TYPE_E;

typedef enum tagEPON_SUBPORT_LINKSTATUS
{
    EPON_SUBPORT_OPEN,   /*OLTƽ̨0��mib��h3cEponUniAdminStatus��Ӧ1��ONU�ǲ���Ҳ��1*/
    EPON_SUBPORT_CLOSE,  /*OLTƽ̨1��mib��h3cEponUniAdminStatus��Ӧ2��ONU�ǲ���Ҳ��2*/
    EPON_SUBPORT_BUTT
} EPON_SUBPORT_LINKSTATUS_E;

/* ONU���������ȼ�����ģʽö�ٶ��� */
typedef enum tagONU_PRI_TRUST_MODE
{
    ONU_PRI_TRUST_MODE_IP_TOS,     /* IP TOS ���ȼ�  */
    ONU_PRI_TRUST_MODE_DOT1P,      /* 802.1p���ȼ�   */
    ONU_PRI_TRUST_MODE_DSCP,       /* DSCP���ȼ�     */
    ONU_PRI_TRUST_MODE_PORT,       /* ���ζ˿����ȼ� */
    ONU_PRI_TRUST_MODE_BUTT
} ONU_PRI_TRUST_MODE_E;

/* ONU���е������ȼ�ö�ٶ��� */
typedef enum tagONU_QUEUE_SCHEDULER_MODE
{
    ONU_QUEUE_SCHEDULER_MODE_WFQ,    /* 8421��Ȩ���ȼ� */
    ONU_QUEUE_SCHEDULER_MODE_SPQ,    /* �������ȼ� */
    ONU_QUEUE_SCHEDULER_MODE_WRR,    /**/
    ONU_QUEUE_SCHEDULER_MODE_BUTT
} ONU_QUEUE_SCHEDULER_MODE_E;


/* UNI��ͳ������ */
typedef struct UNI_COUNTER
{
    /* ��ȷ����ͳ�� */
    ULONG InUnicast;    /* ���յ������� */
    ULONG InMulticast;  /* ���նಥ���� */
    ULONG InBroadcast;  /* ���չ㲥���� */
    ULONG InPause;      /* �������ر��� */
    ULONG OutUnicast;   /* ���͵������� */
    ULONG OutMulticast; /* ���Ͷಥ���� */
    ULONG OutBroadcast; /* ���͹㲥���� */
    ULONG OutPause;     /* �������ر��� */

    /* ������ͳ�� */
    ULONG InError;      /* ���մ��� */
    ULONG InOverflow;   /* ���Ĺ��� */
    ULONG InUnderflow;  /* ���Ĺ�С */
    ULONG InCrcError;   /* CRC����  */
    ULONG OutError;     /* ���ʹ��� */
    ULONG OutCollision; /* ��ͻ     */
    ULONG OutLate;      /* ʱ��     */
    ULONG ulInDropped;  /* ���ն��� */
    ULONG ulInFilter;   /* ���չ��� */
    ULONG ulOutFilter;  /* ���͹��� */


    /* �ֽ���ͳ�� */
    UINT64  uiRecOctets;        /* ���յ�����ȷ�ֽ�����*/
    ULONG   ulRevBadOctets;     /* ���յ��Ĵ����ֽ��� */
    UINT64  uiTransOctets;      /* ���͵��ֽ����� */
} UNI_COUNTER_S;

typedef struct tagPort_Mib_Counters
{
    UINT64 uiIfInOctets;
    UINT64 uiDdot3StatsFCSErrors;
    UINT64 uiDot3StatsSymbolErrors;
    UINT64 uiDot3InPauseFrames;
    UINT64 uiDot3ControlInUnknownOpcodes;
    UINT64 uiEtherStatsFragments;
    UINT64 uiEtherStatsJabbers;
    UINT64 uiIfInUcastPkts;
    UINT64 uiEtherStatsDropEvents;
    UINT64 uiEtherStatsOctets;
    UINT64 uiEtherStatsUndersizePkts;
    UINT64 uiEtherStatsOversizePkts;
    UINT64 uiEtherStatsPkts64Octets;
    UINT64 uiEtherStatsPkts65to127Octets;
    UINT64 uiEtherStatsPkts128to255Octets;
    UINT64 uiEtherStatsPkts256to511Octets;
    UINT64 uiEtherStatsPkts512to1023Octets;
    UINT64 uiEtherStatsPkts1024toMaxOctets;
    UINT64 uiEtherStatsMcastPkts;
    UINT64 uiEtherStatsBcastPkts;	
    UINT64 uiIfOutOctets;
    UINT64 uiDot3StatsSingleCollisionFrames;
    UINT64 uiDot3StatsMultipleCollisionFrames;
    UINT64 uiDot3StatsDeferredTransmissions;
    UINT64 uiDot3StatsLateCollisions;
    UINT64 uiEtherStatsCollisions;
    UINT64 uiDot3StatsExcessiveCollisions;
    UINT64 uiDot3OutPauseFrames;
    UINT64 uiDot1dBasePortDelayExceededDiscards;
    UINT64 uiDot1dTpPortInDiscards;
    UINT64 uiIfOutUcastPkts;
    UINT64 uiIfOutMulticastPkts;
    UINT64 uiIfOutBrocastPkts;
    UINT64 uiOutOampduPkts;
    UINT64 uiInOampduPkts;
    UINT64 uiPktgenPkts;
    UINT64 uiDot1dTpLearnEntryDiscard;
}PORT_MIB_COUNTER_S;

typedef struct tagPON_COUNTERS
{
    ULONG_8 ullRxPkt;           /* PON��ʵ�ʽ���֡�� */
    ULONG_8 ullTxPkt;           /* PON��ʵ�ʷ���֡�� */
    ULONG_8 ullDropPkt;         /* PON��ʵ�ʶ���֡�� */
    ULONG_8 ullCrcErr;          /* PON��ʵ��CRCУ�����֡�� */
    ULONG_8 ullRxErr;           /* PON��ʵ�ʽ��մ���֡��    */
    ULONG_8 ullRxByte;          /* PON��ʵ�ʽ����ֽ���      */
    ULONG_8 ullTxByte;          /* PON��ʵ�ʷ����ֽ���      */
    ULONG_8 ullAlgErr;          /* ֡ͬ������               */
    ULONG_8 ullTooLong;         /* ����֡                   */
} PON_COUNTERS_S;

typedef enum tagPortDuplexValue
{
    /* �������ĵ������� */
    OAM_PORT_DUPLEX_HALF    =   0,
    OAM_PORT_DUPLEX_FULL    =   1,
    OAM_PORT_DUPLEX_AUTO    =   2
}PORT_DUPLEX_MODE_E ;

/* UNI��MDIģʽ */
typedef enum tagUNI_MDI_MODE
{
    MDI_MODE_NORMAL,    /* ֱ����    */
    MDI_MODE_CROSS,     /* ������    */
    MDI_MODE_AUTO,      /* MDI��Э�� */
    MDI_MODE_BUTT
} UNI_MDI_MODE_E;

/* RXTX���� */
typedef enum tagRXTX_ACT
{
    RXTX_ACT_TRANS_EOAM,
    RXTX_ACT_RCV_EOAM,
    RXTX_ACT_TRANS_STP,
    RXTX_ACT_RCV_STP,
    RXTX_ACT_TRANS_IGMP,
    RXTX_ACT_RCV_IGMP,
    RXTX_ACT_TRANS_DHCP,
    RXTX_ACT_RCV_DHCP,
    RXTX_ACT_TRANS_IP,
    RXTX_ACT_RCV_IP,
    RXTX_ACT_TRANS_AGMP,
    RXTX_ACT_RCV_AGMP,
    RXTX_ACT_TRANS_ACMP,
    RXTX_ACT_RCV_ACMP,
    RXTX_ACT_FWD_CAR_ARP,
    RXTX_ACT_DROP_CAR_ARP,
    RXTX_ACT_FWD_CAR_STP,
    RXTX_ACT_DROP_CAR_STP,
    RXTX_ACT_FWD_CAR_DHCP,
    RXTX_ACT_DROP_CAR_DHCP,
    RXTX_ACT_RCV_PPPOE,
    RXTX_ACT_TRANS_PPPOE,
    RXTX_ACT_FWD_CAR_ONU_MAC,
    RXTX_ACT_DROP_CAR_ONU_MAC,
    RXTX_ACT_RX_ETH,    /* ��̫�������հ� */
    RXTX_ACT_TX_ETH,    /* ��̫���������� */
    RXTX_ACT_TX_PP_PKT,
    RXTX_ACT_RX_PP_PKT,
    RXTX_ACT_TRANS_SMLK,
    RXTX_ACT_TRANS_ONU,
    RXTX_ACT_BUTT
} RXTX_ACT_E;

typedef struct tagONU_BOARD_UID
{
    UCHAR aucBoardType[16];     /* ��������    */    
    UCHAR aucHostType[16];      /* ONU�������� */
    UCHAR aucHandShake[16];    
    UCHAR aucBoardVer[8];       /* Ӳ���汾    */    
    UCHAR aucSoftVer[8];        /* ����汾    */
    UCHAR aucEepromVer[8];      /* EEPROM�汾  */
    UCHAR aucBootVer[8];        /* boot�汾    */
} ONU_BOARD_UID_S;
typedef enum tagINF_ONU_BOARD_TYPE
{
    ONU_BORAD_TYPE_EPN104,    
    ONU_BOARD_TYPE_BUTT
} ONU_BOARD_TYPE_E;

typedef enum  tagEOAM_ONU_AUTHORIZE_STATUS
{
    EOAM_ONU_AUTHORIZE_STATUS_PASS = 0,   /*0��ͨ����Ȩ  1��ȡ����Ȩ*/
    EOAM_ONU_AUTHORIZE_STATUS_CANCEL,
    EOAM_ONU_AUTHORIZE_STATUS_BUTT
} EOAM_ONU_AUTHORIZE_STATUS_E;

/* EOAM:֪ͨONU�Ƿ�shut/undo shut����*/
typedef enum  tagEOAM_ONU_AUTHORIZE_REASON
{
    EOAM_ONU_AUTHORIZE_REASON_UNDOSHUTDOWN = 0,/*0��undo shut  1: shut*/
    EOAM_ONU_AUTHORIZE_REASON_SHUTDOWN,   
    EOAM_ONU_AUTHORIZE_REASON_BUTT
} EOAM_ONU_AUTHORIZE_REASON_E;

/*----------------------------------------------*
 * �ṹ�嶨��                                   *
 *----------------------------------------------*/
typedef int h3c_sem_t;

/* EOAM transmission control block */
typedef struct tagEOAM_CTL_BLOCK
{
    /* first byte */
    ULONG ucVer             : 2;    /* control block version        */
    ULONG ucReserved1       : 6;

    /* second byte */
    ULONG bNeedReplay       : 1;    /* this packet need a reply     */
    ULONG bLastPkt          : 1;    /* this is the last packet      */
    ULONG bSupportRetrans   : 1;    /* Peer support retransmission  */
    ULONG bNeedRetrans      : 1;    /* need to be retransmited      */
    ULONG ucRetransCnt      : 2;    /* retransmission counter       */
    ULONG ucReserved2       : 2;

    /* last two bytes */
    ULONG usPacCnt          : 11;   /* Total packet count or packet index */
    ULONG ucReserved3       : 4;
    ULONG bFirst            : 1;    /* this is the first packet     */
} EOAM_CTL_BLOCK_S;

/* EOAM pad information */
typedef struct tagEOAM_PAD_INFO
{
    UCHAR  *pucOui;
    UCHAR   ucPduType;
    USHORT  usReqId;
    UCHAR   ucErrCode;
    union
    {
        ULONG            ulReqCtr;
        EOAM_CTL_BLOCK_S stCtl;
    } unCtl;
} EOAM_PAD_INFO_S;

/* EOAM pad structure pointers */
typedef struct tagEOAM_PAD_PTR
{
    UCHAR  *pucPadHdr;              /* pointer to pad header  */
    UCHAR  *pucPadData;             /* pointer to pad data    */
    UCHAR  *pucEvtData;             /* pointer to evt pad hdr */
    USHORT  usPadLen;               /* pad/data length        */    
} EOAM_PAD_PTR_S;

/* EOAM receive packet buffer */
typedef struct tagEOAM_RECEIVE_PKT
{
    UCHAR           aucPadBuf[EOAM_PAD_MAX_SIZE];       /* pad buffer           */
    EOAM_PAD_INFO_S stInfo;                             /* pad/data information */
    EOAM_PAD_PTR_S  stPtr;                              /* pad pointers         */
    h3c_sem_t       stSem;                              /* semaphore            */
} EOAM_RECEIVE_PKT_S;

/* EOAM send packet buffer */
typedef struct tagEOAM_SEND_PKT
{
    UCHAR           aucEthHdr[EOAM_ETH_HDR_SIZE];       /* ethernet header      */
    UCHAR           aucPadBuf[EOAM_PAD_MAX_SIZE];       /* pad header buf       */
    EOAM_PAD_PTR_S  stPtr;                              /* pad pointers         */    
    h3c_sem_t       stSem;                              /* semaphore            */
} EOAM_SEND_PKT_S;

typedef enum tagACL_FILTER_TYPE
{
    FILTER_SRC_MAC,     /* ����ԴMAC����  */
    FILTER_DEST_MAC,    /* ����Ŀ��MAC����  */
    FILTER_ETHER_TYPE,  /* ������̫������   */
    FILTER_VLAN_ID,     /* ����VLAN ID����  */
    FILTER_SRC_IP,      /* ����ԴIP����     */
    FILTER_DEST_IP,     /* ����Ŀ��IP����   */
    BIND_SRC_IP,        /* ��ԴIP         */
    BIND_DEST_IP,       /* ��Ŀ��IP       */
    FILTER_SRC_TCP,     /* ����ԴTCP����     */
    FILTER_DEST_TCP,    /* ����Ŀ��TCP����   */
    FILTER_SRC_UDP,     /* ����ԴUDP����     */
    FILTER_DEST_UDP,    /* ����Ŀ��UDP����   */
    FILTER_BUTT
} ACL_FILTER_TYPE_E;

/* ���Ĺ���ȡֵ */
typedef union tagACL_FILTER_VALUE
{
    UCHAR  aucMacAddr[MAC_ADDR_LEN];   /* MAC��ַ    */
    ULONG usEtherType;                 /* ��̫������ */
    ULONG usVlanId;                    /* VLAN ID    */
    ULONG  ulIpAddr;                    /* IP��ַ     */
    ULONG usPort;                      /* TCP/UDP port */
} ACL_FILTER_VALUE_U;

typedef enum tagONU_DIRECTION
{
    DIRECTION_UPLINK,   /* ���з���   */
    DIRECTION_DOWNLINK, /* ���з���   */
    DIRECTION_BOTH,     /* �����з��� */
    DIRECTION_BUTT
} ONU_DIRECTION_E;


/* ���Ĺ��˹��� */
typedef struct tagACL_FILTER_RULE
{
    ACL_FILTER_TYPE_E   enType;     /* �������� */
    ONU_DIRECTION_E     enDirect;   /* ���˷��� */
    ACL_FILTER_VALUE_U  unValue;    /* ����ȡֵ */
    ULONG               uiRulePrecedence;
} ACL_FILTER_RULE_S;


/*used for eoam filter*/
typedef struct tagEoam_Filter
{
   ACL_FILTER_RULE_S stHead;     
   ACL_FILTER_RULE_S *stNext;    
   ULONG             ulNum; /**** Number Of Nodes In List ****/
}EOAM_FILTER_S;

typedef union tagACL_CLF_RMK_VAL
{
    UCHAR  aucMacAddr[MAC_ADDR_LEN];   /* MAC��ַ    */
    ULONG ulValue;                 
} ACL_CLF_RMK_VAL_EN;

/* EOAMЭ���OAMPDU�����ò������� */
typedef enum tagEOAM_REQEST_ID
{  
    EOAM_GET_UNISTATUS = 0,             /* 0 ��ѯONU 6301��UNI��״̬*/
    EOAM_SET_UNISTATUS,                 /* 1 ����ONU 6301��UNI��״̬*/ 
    EOAM_UPDATE_ONUSOFTWARE,            /* 2 Զ������ONU��� */
    EOAM_DOT1X_CTRL,                    /* 3 ����/�ر�802.1x��֤ */
    EOAM_MC_FILTER_CTRL,                /* 4 ʹ��/�ر��鲥���˹��� */
    EOAM_SET_RESETONU,                	/* 5 ��λONU */
    EOAM_GET_MIBINFO,                   /* 6 ��ѯONU��׼MIBͳ����Ϣ */
    EOAM_RESTORE_EEPROM,                /* 7 �ָ�EEPROM��Ĭ��ֵ */
    EOAM_GET_S3100MAC,                  /* 8 ��ѯ31������MAC��ַ��Ϣ */ 
    EOAM_SET_1XNAMEPASSWORD,            /* 9 ����802.1x���û��������� */ 
    EOAM_GET_1XNAMEPASSWORD,            /* a ��ѯ802.1x���û��������� */ 
    EOAM_SET_RSTONUCNT,                 /* b ���ONUͳ�Ƽ��� */
    EOAM_ADD_IGSPFILTER,                /* c ���һ���ಥ���˵�ַ */
    EOAM_DEL_IGSPFILTER,                /* d ɾ��һ���ಥ���˵�ַ */
    EOAM_NOTIFY_BRIGDEMAC,             	/* e ����65��Mac��ַ���ӿ� */
    EOAM_GET_IGSPFILTER,                /* f ��ȡ��ǰ�ಥ���˵�ַ */
    EOAM_GET_VERSION,                   /* 10 ��ȡONU����Ӳ���汾��Ϣ*/
    EOAM_STP_CTRL,                      /* 11 ʹ��/�ر�STP���� */
    EOAM_OPTION82_CTRL,                 /* 12 ʹ��/�ر�Option82���� */
    EOAM_WFQ_SPQ_SWITCH,                /* 13 QoSģʽ�л�*/
    EOAM_BIND_IPADDRESS,                /* 14 IP��ַ��*/
    EOAM_GET_CPURATE,                   /* 15 CPUռ���ʲ�ѯ*/
    EOAM_GET_UPDATERECORD,              /* 16 ��ѯ������¼*/
    EOAM_GET_SELFTESTRECORD,            /* 17 ��ѯ�Լ��¼*/
    EOAM_UPDATE_EEPROM,                 /* 18 ����ONU EEPROM*/
    EOAM_SAVE_EEPROM,                   /* 19 ����ONU EEPROM*/
    EOAM_PRIORITYMODE_CTRL,             /* 1a ���ȼ�ģʽ�л���IP TOS/COS/DSCP*/
    EOAM_SUBPORT_ENABLE,                /* 1b �Ӷ˿ڴ�/�ر�����*/
    EOAM_SUBPORT_VLANMODE_CTRL,         /* 1c �Ӷ˿�VLAN ģʽ�л�*/
    EOAM_SET_SUBPORT_VLAN,              /* 1d �Ӷ˿�pvid����*/
    EOAM_SET_SUBPORT_PRIORITY,          /* 1e �Ӷ˿����ȼ�����*/
    EOAM_SET_SUBPORT_BANDWIDTH,         /* 1f �Ӷ˿������д�������*/
    EOAM_SUBPORT_FLOW_CTRL,             /* 20 �Ӷ˿���������*/
    EOAM_SUBPORT_SPEED_CTRL,            /* 21 �Ӷ˿��ٶ�ģʽ����*/
    EOAM_SUBPORT_DUPLEX_CTRL,           /* 22 �Ӷ˿�˫��ģʽ����*/
    EOAM_GET_SUBPORT_DATA,              /* 23 ��ȡONU �Ӷ˿ڵ����á�״̬��ͳ������*/
    EOAM_SET_SUBPORT_TRUNKVLAN,         /* 24 ����ONU�Ӷ˿ڵ�trunk list*/
    EOAM_FLASH_DATATEST,                /* 25 Flash�������߲���*/
    EOAM_FLASH_ADDRTEST,                /* 26 Flash�ĵ�ַ�߲���*/
    EOAM_FLASH_BLOCKTEST,               /* 27 Flash�Ŀ����*/
    EOAM_WATCHDOG_TEST,                 /* 28 ���Ź�����*/
    EOAM_EEPROM_TEST,                   /* 29 EEPROM����*/
    EOAM_INNER_LOOPBACK,                /* 2a �ڻ��ز���*/
    EOAM_EXTERN_LOOPBACK,               /* 2b �⻷�ز���*/
    EOAM_OPTICS_STATUSTEST,             /* 2c ��ģ��״̬��ѯ����*/
    EOAM_GET_VERSION_EQUIP,             /* 2d ��ȡONU�İ汾��Ϣ���ԣ�װ������*/
    EOAM_GET_SDRAMTEST,                 /* 2e SDRAM������Ϣ��ѯ*/
    EOAM_GET_MANUINFO,                  /* 2f ��ȡ������Ϣ*/
    EOAM_SET_MANUINFO,                  /* 30 ����������Ϣ*/
    EOAM_SET_RSTSUBPORTCNT,             /* 31 ���ONU�Ӷ˿ڵļ�����*/
    EOAM_SET_SUBPORT_MDI,               /* 32 �Ӷ˿ڵ�MDIģʽ����*/
    EOAM_SUBPORT_QINQ_CTRL,             /* 33 ONU��QinQģʽ����*/
    EOAM_GET_IGSP_DATA,                 /* 34 ��ѯONU IGSPЭ����������*/
    EOAM_GET_STP_DATA,                  /* 35 ��ѯONU STPЭ����������*/
    EOAM_UPDATE_ONUBOOTROM,             /* 36 ����ONU��bootrom*/
    EOAM_IGSP_CTRL,                     /* 37 ʹ��/�ر�IGSP���� */
    EOAM_SET_DEBUG_DEST,                /* 38 ����ONU������Ϣ���Ŀ�� */
    EOAM_SET_ONU_MAC,                   /* 39 ����ONU��MAC��ַ*/
    EOAM_GET_ONU_MAC,                   /* 3a ��ȡONU��MAC��ַ*/
    EOAM_SET_ONU_CMD,                   /* 3b �·�ONU���������� */
    EOAM_DEL_6045_MAC_TBL,              /* 3c ɾ��6045 ��̬MAC��ַ�� */
    EOAM_SET_SUBPORT_CFG,               /* 3d �·��Ӷ˿����� */
    EOAM_SET_ONU_MSG_FILTER,            /* 3e ���ñ��Ĺ��˹��� */
    EOAM_SET_ONU_FLOW_CLASS,            /* 3f ������������� */
    EOAM_SET_ONU_QUEUE_SIZE,            /* 40 ����ONU���ȼ����еĴ�С */
    EOAM_SET_ONU_UNI_MAC_NUM,           /* 41 ����ONU UNI��MACѧϰ����*/
    EOAM_GET_ONU_PON_STATISTICS,        /* 42 ��ѯONU PON��ͳ�Ƽ��� */    
    EOAM_SET_LIGHT_MODE,                /* 43 ����ONU������״̬ */
    EOAM_SET_PQ_POLICER,                /* 44 ����pq policer */
    EOAM_DIAG_CONSOLE_TEST,             /* 45 ONU���ڻ�����ϲ��� */
    EOAM_ADD_STATIC_MAC,                /* 46 ���6301��̬MAC */
    EOAM_DEL_STATIC_MAC,                /* 47 ɾ��6301��̬MAC */
    EOAM_SET_SUBPORT_ISOLATE,           /* 48 ����ONU�Ӷ˿ڸ��� */
    EOAM_GET_ONU_LIGHT_MODE,            /* 49 ��ȡONU������״̬ */
    EOAM_SET_ONU_WORK_MODE ,            /* 4a ����ONU����ģʽ */
    EOAM_SET_ONU_IP_ADDR,               /* 4b ����ONU IP��ַ */
    EOAM_SET_UNI_BLOCK,                 /* 4c 75����stp״̬block onu�����п� */
    EOAM_SET_LED_CTRL,                  /* 4d LED���� */
    EOAM_SET_6045_TEST,                 /* 4e 6045���� */
    EOAM_GET_BOARD_INFO,                /* 4f ��ȡ�汾��Ϣ */
    EOAM_GET_OPTICMODE_TYPE,            /* 50 ��ȡONU��ģ������ */
    EOAM_PPPOE_PLUS_CTRL,               /* 51 ʹ��/�ر�PPPoE+���� */
    EOAM_SET_ONU_PORT_NUM,              /* 52 ����ONU����75��Ӧ�Ķ˿ں�(����:��λ��, OLT��, ��Ӧ�Ķ˿ں�) */
    EOAM_SET_COS_MAP_LOCAL,             /* 53 ����ONU�豸cos��local���ȼ�ӳ��*/
    EOAM_GET_DHCP_CLIENT_INFO,          /* 54 ��ʾdhcp-client��Ϣ */
    EOAM_SET_UNI_IGSP_FAST_LEAVE,       /* 55 �����鲥�����뿪���� */
    EOAM_SET_UNI_MIRROR,                /* 56 ���þ����� */
    EOAM_SET_UNI_MONITOR,               /* 57 ���ü��ӹ��� */
    EOAM_DHCP_CTRL,                     /* 58 ����DHCPЭ��ʹ�ܹ��� */
    EOAM_SET_MANAGEMENT_VLAN,           /* 59 ����ONU����VLAN */
    EOAM_SET_MANAGEMENT_VLAN_INTF,      /* 5a ���ù���VLAN�ӿ�״̬ */
    EOAM_SET_IGSP_PRO,                  /* 5b �����鲥�������� */
    EOAM_SET_DHCP_CLIENT,               /* 5c ����ONU DHCP clientʹ�� */
    EOAM_GET_DHCP_SNOOPING_INFO,        /* 5d ��ʾdhcp-snooping��Ϣ */
    EOAM_QINQ_CTRL,                     /* 5e ����QinQ�Ĳ��� */
    EOAM_NOTIFY_ONU_AUTHORIZE,          /* 5f ֪ͨONU��Ȩ״̬ */
    EOAM_NOTIFY_ACCESS_PVID,            /* 60 ����accessģʽ*/
    EOAM_GET_UNI_VCT,                   /* 61 ��ȡONU VCT���*/
    EOAM_SET_ONU_EVENT,                 /* 62 ����ONU�ϱ�������Ϣ�ĵȼ�*/
    EOAM_ADD_STATIC_MCAST_MAC,          /* 63 ���ONU��̬�鲥MAC */
    EOAM_DEL_STATIC_MCAST_MAC,          /* 64 ɾ��ONU��̬�鲥MAC */
    EOAM_SET_SUBPORT_QINQ,              /* 65 ����31Qinq VLAN*/
    EOAM_GET_MULTI_PON_PHY_INFO,        /* 66 ��ȡONU˫PON���������Ϣ*/
    EOAM_GET_ONU_SMART_LINK_INFO,       /* 67 ��ȡONU SMART LINK״̬��Ϣ*/
    EOAM_GET_S485_PROPERTYCFG,          /* 68 ��ѯONU 485��������������Ϣ*/
    EOAM_GET_S485_SESSIONCFG_G,         /* 69 ��ѯONU 485���ڻỰȫ��������Ϣ*/
    EOAM_GET_S485_SESSIONCFG_D,         /* 6a ��ѯONU 485���ڻỰ��ϸ������Ϣ*/
    EOAM_SET_S485_SESSIONCFG_D,         /* 6b �·�ONU 485���ڻỰ��ϸ����*/
    EOAM_SET_S485_PROPERTYCFG,          /* 6c �·�ONU 485��������������Ϣ*/
    EOAM_SET_SUBPORT_SPEED_1000,        /* 6d �����Ӷ˿�����Ϊ1000M */
    EOAM_SET_ONU_LUMINESCENCE_OFF,       /* 6e ���ùر�onu��ģ�鷢��һ��ʱ��*/
    
    EOAM_SET_ECONUVLANID   = 0x0100,    /* ����EC ONU�ӿ�VLANID */    
    EOAM_SET_ECONUPRIORITY = 0x0101,    /* ����EC ONU�ӿ��˿����ȼ� */
    EOAM_SET_ECONU_LED     = 0x0102,    /* ����EC ONU�ӿ���LED */
    EOAM_SET_ET704_VLANMODE = 0x0200,   /* ����ET704��VLANģʽ */
    EOAM_SET_ET704_SUBPORTCFG = 0x0201, /* �����·�ET704�Ӷ˿����� */
    EOAM_NOTIFY_LOOPBACK = 0x0202,      /* ֪ͨONU���л�· */
    EOAM_NOTIFY_NO_LOOPBACK = 0x0203,   /* ֪ͨONU�»�·��ʧ */
    EOAM_REQEST_BUTT,
    
    EOAM_GET_ONU_STRINGS = 0xFFFD,      /* ��ONU��ȡ�ַ�����Ϣ */
    EOAM_BATCH_GET = 0xEEEE,            /* �������ò�ѯ*/
    EOAM_UNI_BATCH_SET = 0xFFFE,        /* UNI������������ */
    EOAM_BATCH_SET = 0xFFFF             /* ������������*/
} EOAM_REQEST_ID_E;

/* EOAMЭ���¼����� */
typedef enum  tagEOAM_EVENT_TYPE
{
    EOAM_EVENT_SUBPORT_STATUS_CHANGE = 0,   /* UNI��״̬�仯 */
    EOAM_EVENT_UPDATE_SOFTWARE_FAILURE,     /* ONU�������ʧ�� */
    EOAM_EVENT_DEBUG_INFO_REPORT,           /* ONU�ϱ�������Ϣ */
    EOAM_EVENT_MC_MAC_IS_EMPTY,             /* �鲥MAC��Ϊ�� */
    EOAM_EVENT_SMLK_NEIGHBOR_FROM31,        /* Added for SMLK:OLT port receive flush packets */
    EOAM_EVENT_SET_TRUNK_FINISH,            /* trunk list��������ϱ� */    
    EOAM_EVENT_DEBUG_STRING,  /*�ϱ�debug��Ϣ����ʾ��Ҫ���Կ���*/
    EOAM_EVENT_LOG_STRING,    /*�ϱ�log��Ϣ����ʾ����Ҫ���Կ���*/
    EOAM_EVENT_TRAP_STRING,   /*�ϱ�trap��Ϣ����ʾ����Ҫ���Կ���*/
    EOAM_EVENT_LAST_HANDLER
} EOAM_EVENT_TYPE_E;

typedef enum tagEOAM_LED_CTRL_CODE
{
    EOAM_LED_CTRL_ON,               /* ���� */
    EOAM_LED_CTRL_OFF,              /* Ϩ�� */
    EOAM_LED_CTRL_DEFAULT,          /* ��ԭ */
    EOAM_LED_CTRL_BUTT              /* �ղ��� */
}EOAM_LED_CTRL_CODE_E;

/* ONU����ģʽ */
typedef enum tagLIGHT_MODE
{
    LIGHT_MODE_OFF    = 0,      /* ������   */
    LIGHT_MODE_ALWAYS = 1,      /* ������   */
    LIGHT_MODE_NOMAL  = 2,      /* �������� */
    LIGHT_MODE_BUTT
} LIGHT_MODE_E;

/* �����·����� */
#define IS_BATCH_SET_CFG(enReqId)                   \
      (   (enReqId == EOAM_STP_CTRL)                \
       || (enReqId == EOAM_IGSP_CTRL)               \
       || (enReqId == EOAM_OPTION82_CTRL)           \
       || (enReqId == EOAM_PRIORITYMODE_CTRL)       \
       || (enReqId == EOAM_WFQ_SPQ_SWITCH)          \
       || (enReqId == EOAM_BIND_IPADDRESS)          \
       || (enReqId == EOAM_SET_ONU_IP_ADDR)         \
       || (enReqId == EOAM_SET_ONU_MSG_FILTER)      \
       || (enReqId == EOAM_SET_ONU_FLOW_CLASS)      \
       || (enReqId == EOAM_SET_ONU_QUEUE_SIZE)      \
       || (enReqId == EOAM_SET_PQ_POLICER)          \
       || (enReqId == EOAM_SET_ONU_UNI_MAC_NUM)     \
       || (enReqId == EOAM_PPPOE_PLUS_CTRL)         \
       || (enReqId == EOAM_SET_ONU_PORT_NUM)        \
       || (enReqId == EOAM_SET_COS_MAP_LOCAL)       \
       || (enReqId == EOAM_SET_UNI_IGSP_FAST_LEAVE) \
       || (enReqId == EOAM_SET_UNI_MONITOR)         \
       || (enReqId == EOAM_DHCP_CTRL)               \
       || (enReqId == EOAM_SET_MANAGEMENT_VLAN)     \
       || (enReqId == EOAM_SET_MANAGEMENT_VLAN_INTF)\
       || (enReqId == EOAM_SET_IGSP_PRO)            \
       || (enReqId == EOAM_SET_DHCP_CLIENT)         \
       || (enReqId == EOAM_QINQ_CTRL)               \
       || (enReqId == EOAM_SET_SUBPORT_QINQ)        \
       || (enReqId == EOAM_NOTIFY_ONU_AUTHORIZE))

struct Pmc_Oam_Header {
	UCHAR	oui[EOAM_OUI_SIZE];
	UCHAR	ext_code;
}__attribute__((packed));
typedef struct Pmc_Oam_Header PMC_OAM_HEADER_S;

#define OAM_OAMPDU_CODE_SET_THRESH      0x70

typedef enum tagQOS_Queue_Schedue_Mode
{
    QOS_QUEUE_SCHEDULER_MODE_WFQ,    /* 8421��Ȩ���ȼ� */
    QOS_QUEUE_SCHEDULER_MODE_SPQ,    /* �������ȼ� */
    QOS_QUEUE_SCHEDULER_MODE_BUTT
} QOS_QUEUE_SCHEDULER_MODE_E;

typedef enum tagQOS_Pri_Trust_Mmode
{
    QOS_PRI_TRUST_MODE_IP_TOS,     /* IP TOS ���ȼ�  */
    QOS_PRI_TRUST_MODE_DOT1P,      /* 802.1p���ȼ�   */
    QOS_PRI_TRUST_MODE_DSCP,       /* DSCP���ȼ�     */
    QOS_PRI_TRUST_MODE_PORT,       /* ���ζ˿����ȼ� */
    QOS_PRI_TRUST_MODE_BUTT
} QOS_PRI_TRUST_MODE_E;

typedef enum tagPORT_Direct
{
    DIRECT_IN,
    DIRECT_OUT,
    DIRECT_BOTH,
    DIRECT_END_OAM
}PORT_DIRECT_E;

typedef enum tagPORT_Duplex
{
    DUPLEX_AUTO,
    DUPLEX_FULL,
    DUPLEX_HALF,
    DUPLEX_END
}PORT_DUPLEX_E;
typedef enum tag_ALARM_OAM_TYPE
{
    OAM_THRSD_ERRORED_SYM_PRD = 0x0000,
    OAM_THRSD_ERRORED_FRAME,       /*0x0001*/
    OAM_THRSD_ERRORED_FRAME_PRD,   /*0x0002*/
    OAM_THRSD_ERRORED_FRM_SEC,     /*0x0003*/
    OAM_THRSD_TYPE_END
}ALARM_OAM_TYPE_E;

/* �˿�UP/DOWN��̬ */
typedef enum tagPORT_STATUS
{
    STATUS_UP,      /* �˿�ΪUP   */
    STATUS_DOWN,    /* �˿�ΪDOWN */
    STATUS_BUTT
} PORT_STATUS_E;

typedef enum tagPORT_Speed
{
    SPEED_AUTO,
    SPEED_10M,
    SPEED_100M,
    SPEED_1000M,
    SPEED_END
}PORT_SPEED_E;

typedef enum tagPORT_Mdi
{
    MDI_AUTO,
    MDI_ACROSS,
    MDI_NORMAL,
    MDI_END
}PORT_MDI_E;

/* UNI����ģʽ */
typedef enum tagUNI_SPEED_MODE
{
    SPEED_MODE_AUTO  = 0,    /* ������Э�� */
    SPEED_MODE_10M   = 10,   /* 10M����    */
    SPEED_MODE_100M  = 100,  /* 100M����   */
    SPEED_MODE_1000M = 1000,
    SPEED_MODE_BUTT
} UNI_SPEED_MODE_E;

/* UNI˫��ģʽ */
typedef enum tagUNI_DUPLEX_MODE
{
    DUPLEX_MODE_HALF,   /* ��˫��     */
    DUPLEX_MODE_FULL,   /* ȫ˫��     */
    DUPLEX_MODE_AUTO,   /* ˫����Э�� */
    DUPLEX_MODE_BUTT
} UNI_DUPLEX_MODE_E;

#define CABLE_PAIR_NUM 4
#define     VCT_NOT_SUPPORT   0xFFFFFFFF      /*��֧�ָü����*/

typedef enum tagVCT_CABLE_STATUS
{
    VCT_TEST_FAIL,
    VCT_NORMAL_CABLE,
	VCT_IMPEDANCE_MISMATCH,
    VCT_OPEN_CABLE,
    VCT_SHORT_CABLE,
    VCT_SHORT_BUTT
}VCT_CABLE_STATUS_E;

typedef struct tagVCT_CABLE_DIAG
{
    VCT_CABLE_STATUS_E aenCableStatus[CABLE_PAIR_NUM]; /*4��˫���߶���*/
    USHORT ausCableLen[CABLE_PAIR_NUM];
}VCT_CABLE_DIAG_S;

typedef enum enVctTestStatus
{
    TEST_FAIL = 0,      /*���ʧ��*/
    NORMAL_CABLE,       /*��������*/
    OPEN_CABLE,         /*���¿�·*/
    SHORT_CABLE,        /*���¶�·*/
    ABNORMAL_CABLE,     /*���ֲ�����·���Ƕ�·*/
}VCT_TEST_STATUS_E;

typedef struct tagVctTestDiag
{
    VCT_TEST_STATUS_E enCableStatus;    /* ֧�֣�����״̬ */
    ULONG   ulCableLen;                 /* ֧�֣����ϵ�λ�û�linkʱ�߳� */
    ULONG   ulImpedanceMismatch;        /* VCT_NOT_SUPPORT or BOOL_TRUE or BOOL_FALSE����֧�ֵ�����VCT_NOT_SUPPORT����ͬ */
    ULONG   ulPairSkew;                 /* VCT_NOT_SUPPORT or value */
    ULONG   ulPairSwap;                 /* VCT_NOT_SUPPORT or BOOL_TRUE or BOOL_FALSE */
    ULONG   ulPolaritySwap;             /* VCT_NOT_SUPPORT or BOOL_TRUE or BOOL_FALSE */
    ULONG   ulInsertionLoss;            /* VCT_NOT_SUPPORT or value */
    ULONG   ulReturnLoss;               /* VCT_NOT_SUPPORT or value */
    ULONG   ulNearendCrosstalk;         /* VCT_NOT_SUPPORT or value */
}VCT_TEST_DIAG_S;

typedef struct tagRtctResult
{
    ULONG aulChannelShort[CABLE_PAIR_NUM];
    ULONG aulChannelOpen[CABLE_PAIR_NUM];
    ULONG aulChannelMismatch[CABLE_PAIR_NUM];
    ULONG aulChannelLen[CABLE_PAIR_NUM];
}RTCT_RESULT_S;

/*----------------------------------------------*
 * �ⲿ����                                     *
 *----------------------------------------------*/
extern const UCHAR g_aucH3cOui[EOAM_OUI_SIZE];
extern const UCHAR g_aucPmcOui[EOAM_OUI_SIZE];

/* �������־ */
extern UINT8 g_bLightAlways;
extern USHORT g_usBootLoaderVer;
extern UCHAR g_ucOnuUpgradeId;
/*----------------------------------------------*
 * �ⲿ����                                     *
 *----------------------------------------------*/

extern VOID EOAM_Init ( VOID );
extern VOID EOAM_Pkt_Handle (IN UCHAR *pucPkt, IN USHORT usPktLength);
extern VOID POam_Pkt_Handle (IN UCHAR *pucPkt, IN USHORT usPktLength);
extern ULONG EOAM_SendUpgradeResult (IN ULONG ulResult);
extern ULONG EOAM_SendDebugInfo (IN UCHAR *pucDebugInof, IN USHORT usInfoLength, IN UCHAR ucDebugLevel);
extern ULONG EOAM_SendLogInfo (IN UCHAR *pucLogInof, IN USHORT usInfoLength, IN UCHAR ucDebugLevel);
extern ULONG EOAM_SendUniTrapInfo (IN UCHAR ucUniNo, IN UCHAR ucUniState, IN UCHAR ucDebugLevel);
extern ULONG EOAM_SendStpTrapInfo (IN UCHAR ucUniNo, IN UCHAR ucUniState, IN UCHAR ucDebugLevel);
extern ULONG EOAM_SendPonTrapInfo (IN UCHAR ucPonState, IN UCHAR ucDebugLevel);
extern VOID PrintH3cPacket (IN RXTX_ACT_E enAct, IN UCHAR *pucBuf, IN USHORT usLength);
extern ULONG Drv_DispStpInfo (IN UCHAR ucSequence, IN CHAR *szString);
extern ULONG FUNC_GetPonCounter( PON_COUNTERS_S *pstCtrs );
extern ONU_BOARD_TYPE_E g_enOnuBoardType;
#ifdef __cplusplus

}

#endif /* __cplusplus */

#endif /* __H3C_EOAM_H__ */
