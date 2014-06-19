/*****************************************************************************
           
*****************************************************************************/
#ifndef __LW_EOAM_H__
#define __LW_EOAM_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "lw_config.h"
/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define EOAM_ENABLE                  1
#define EOAM_DISABLE                 2

#define EOAM_ETH_HDR_SIZE           18      /* 普通OAMPDU头长度        */
#define EOAM_PAD_MIN_SIZE           42      /* OAM报文数据部分最小长度 */
#define EOAM_PAD_MAX_SIZE           1496    /* OAM报文数据部分最大长度 */
#define EOAM_PAD_HDR_SIZE           11      /* EOAMPDU部分自定义头长度 */
#define EOAM_PAD_EVT_HDR_SIZE       6       /* 事件上报报文头部长度    */
#define EOAM_PAD_DATA_MIN_SIZE      (EOAM_PAD_MIN_SIZE - EOAM_PAD_HDR_SIZE) /* EOAMPDU部分最小数据长度: 31   */
#define EOAM_PAD_DATA_MAX_SIZE      (EOAM_PAD_MAX_SIZE - EOAM_PAD_HDR_SIZE) /* EOAMPDU部分最大数据长度: 1485 */

#define EOAM_PKT_MAX_SIZE          (EOAM_PAD_MAX_SIZE + EOAM_ETH_HDR_SIZE)
#define EOAM_PKT_MIN_SIZE          (EOAM_PAD_MIN_SIZE + EOAM_ETH_HDR_SIZE)

/* EOAM报文操作类型 */
#define EOAM_GET_REQUEST            0x01
#define EOAM_GET_RESPONSE           0x02
#define EOAM_SET_REQUEST            0x03
#define EOAM_SET_RESPONSE           0x04
#define EOAM_EVENT                  0x05
#define EOAM_USER_DATAGRAM          0x06

/* EOAM报文字段偏移位置 */
#define EOAM_ETH_OFFSET_DA          0       /* 目的Mac地址偏移 */
#define EOAM_ETH_OFFSET_SA          6       /* 源Mac地址偏移 */
#define EOAM_ETH_OFFSET_TYPE        12      /* Type地址偏移 */
#define EOAM_ETH_OFFSET_SUBTYPE     14      /* Sub type地址偏移 */
#define EOAM_ETH_OFFSET_FLAGS       15      /* FLAG地址偏移 */
#define EOAM_ETH_OFFSET_CODE        17      /* OAM Code地址偏移 */

/* EOAM PAD字段偏移位置 */
#define EOAM_PAD_OFFSET_OUI         0       /* OUI地址偏移 */
#define EOAM_PAD_OFFSET_PDUTYPE     3       /* PDU Type地址偏移 */
#define EOAM_PAD_OFFSET_REQID       4       /* Reqest Id地址偏移 */
#define EOAM_PAD_OFFSET_ERRCODE     6       /* ONU操作返回值地址偏移 */
#define EOAM_PAD_OFFSET_CTRL        7       /* Request 计数地址偏移 */
#define EOAM_PAD_OFFSET_DATA        11      /* 数据部分地址偏移 */

#define EOAM_BATCH_CFG_TLV_OFF      6       /* 配置项在批量配置报文数据中的偏移 */
/* 子端口link状态 */
#define LINK_IS_UP                  1
#define LINK_IS_DOWN                0

/* 下发操作码 */
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

/* PASONU_transmit_frame函数最大允许传送的OAM长度 */
#define MAX_PERMIT_OAM_LEN          100

#define BATCH_HEAD_SIZE 6 

/*  ONU光模块类型的定义，根据OLT要求定义 */
#define _1000M_SFF_PX10_U            4
#define _1000M_SFF_PX20_U            5

/* OUI的长度 */
#define EOAM_OUI_SIZE 3  

/* 调用EOAM处理函数并break */
#define CALL_EOAM_FUNC_AND_BREAK(Type, EoamFunc)\
    case Type :\
        ulRet = EoamFunc(pucPduDataBuf, usPduDataLen);\
        break;

/* EOAM报文填充到42字节 */
#define FILL_MIN_PAD_DATA(usPadLen) ((usPadLen) = (usPadLen) < EOAM_PAD_MIN_SIZE ? EOAM_PAD_MIN_SIZE : (usPadLen))

#define NTOHL(x) ntohl((UINT32)(x))
#define NTOHS(x) ntohs((UINT16)(x))
#define HTONL(x) htonl((UINT32)(x))
#define HTONS(x) htons((UINT16)(x))

#define OAM_H3C_SUPPORT 1

/* ONU升级软件三步骤 */
#define    ONU_START_SEND_SOFTWARE  0x01     /* 通知ONU下一帧要发送ONU软件 */
#define    ONU_SENDING_SOFTWARE     0x02     /* 发送ONU软件 */
#define    ONU_STOP_SEND_SOFTWARE   0x03     /* 停止发送ONU软件并捎带校验和通知ONU开始烧片 */

/*----------------------------------------------*
 * 公共接口，考虑后续移植到公有模块上           *
 *----------------------------------------------*/
#define HARDWARE_REV "A"

typedef struct tagULong_8
{
    UINT32 ulData[2];  
}ULONG_8;

/* 返回值定义 */
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

/* UNI端口数目 */
#define MIN_UNI_PORT_ID         1
#define MAX_UNI_PORT_ID         MAX_PORT_NUM
#endif
/* ONU工作模式宏定义 */
#define ONU_WORK_MODE_C_MODE        100
#define ONU_WORK_MODE_H_MODE        101

#define NUMBER_OF_DOT1P             8

/* 优先级队列个数 */
#define NUMBER_OF_SWITCH_PRI_QUEUE  4

/*设备对外输出信息的类型宏定义 */
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

/* 端口角色Role */
#define RSTP_ROLE_DISABLEDPORT      0 /* 端口不可用 */
#define RSTP_ROLE_ROOTPORT          1 /* 根端口 */
#define RSTP_ROLE_DESIGNATEDPORT    2 /* 指定端口 */
#define RSTP_ROLE_ALTERNATEPORT     3 /* 备选端口 */
#define RSTP_ROLE_BACKUPPORT        4 /* 备用端口 */

#ifndef RSTP_LOGID_PFWD
#define RSTP_LOGID_PFWD             5 
#endif
#ifndef RSTP_LOGID_PDISC
#define RSTP_LOGID_PDISC            6 
#endif

/*
 * NTOHL: 32位 网络字节序 -> 主机字节序
 * NTOHS: 16位 网络字节序 -> 主机字节序
 * HTONL: 32位 主机字节序 -> 网络字节序
 * HTONS: 16位 主机字节序 -> 网络字节序
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

/* VLAN ID范围 */
#define VLAN_ID_0           0
#define MIN_VLAN_ID         1
#define MAX_VLAN_ID         4094
#define MAX_VLAN_NUM        4095

//#define PORT_LINKUP     1
//#define PORT_LINKDOWN   0

/*UNI口STP状态类型,added for onu-stp-trap
disabled(1),discarding(2),learning(3),forwarding(4)*/
typedef enum tagEPON_SUBPORT_STPSTATUS
{
    EPON_SUBPORT_STP_DISABLED = 1,
    EPON_SUBPORT_STP_DISCARDING,
    EPON_SUBPORT_STP_LEARNING,
    EPON_SUBPORT_STP_FORWARDING,
    EPON_SUBPORT_STP_BUTT
} EPON_SUBPORT_STPSTATUS_E;

/*PON口告警状态类型,added for onu-pon-trap
AlwaysLaserOn(1),AlwaysLaserRecover(2)*/
typedef enum tagEPON_ALERT_PONSTATUS
{
    EPON_ALERT_PON_ALWAYS_LASER_ON = 1,
    EPON_ALERT_PON_ALWAYS_LASER_RECOVER,
    EPON_ALERT_PON_BUTT
} EPON_ALERT_PONSTATUS_E;

/*ONU通过ONU专用通道上报的trap，由于网管需要的信息很具体，所以每个trap信息需要单独结构体，
也就是data字段第一个ULONG为EOAM_ONUTRAP_TYPE_E,之后为对应trap的上报结构*/
typedef enum tagEOAM_ONUTRAP_TYPE
{
    EOAM_EVENT_ONUTRAP_UNI_UPDOWN,  /*UNI口up、down上报的trap*/
    EOAM_EVENT_ONUTRAP_STPSTATE,    /* PON和UNI口STP状态上报 */
    EOAM_EVENT_ONUTRAP_ALAWAYS_LASER_ON = 3, /* PON长发光状态trap上报 */
    EOAM_EVENT_ONUTRAP_BUTT
}EOAM_ONUTRAP_TYPE_E;

typedef enum tagEPON_SUBPORT_LINKSTATUS
{
    EPON_SUBPORT_OPEN,   /*OLT平台0，mib的h3cEponUniAdminStatus对应1，ONU是不是也是1*/
    EPON_SUBPORT_CLOSE,  /*OLT平台1，mib的h3cEponUniAdminStatus对应2，ONU是不是也是2*/
    EPON_SUBPORT_BUTT
} EPON_SUBPORT_LINKSTATUS_E;

/* ONU二三层优先级信任模式枚举定义 */
typedef enum tagONU_PRI_TRUST_MODE
{
    ONU_PRI_TRUST_MODE_IP_TOS,     /* IP TOS 优先级  */
    ONU_PRI_TRUST_MODE_DOT1P,      /* 802.1p优先级   */
    ONU_PRI_TRUST_MODE_DSCP,       /* DSCP优先级     */
    ONU_PRI_TRUST_MODE_PORT,       /* 信任端口优先级 */
    ONU_PRI_TRUST_MODE_BUTT
} ONU_PRI_TRUST_MODE_E;

/* ONU队列调度优先级枚举定义 */
typedef enum tagONU_QUEUE_SCHEDULER_MODE
{
    ONU_QUEUE_SCHEDULER_MODE_WFQ,    /* 8421加权优先级 */
    ONU_QUEUE_SCHEDULER_MODE_SPQ,    /* 绝对优先级 */
    ONU_QUEUE_SCHEDULER_MODE_WRR,    /**/
    ONU_QUEUE_SCHEDULER_MODE_BUTT
} ONU_QUEUE_SCHEDULER_MODE_E;


/* UNI口统计数据 */
typedef struct UNI_COUNTER
{
    /* 正确报文统计 */
    ULONG InUnicast;    /* 接收单播报文 */
    ULONG InMulticast;  /* 接收多播报文 */
    ULONG InBroadcast;  /* 接收广播报文 */
    ULONG InPause;      /* 接收流控报文 */
    ULONG OutUnicast;   /* 发送单播报文 */
    ULONG OutMulticast; /* 发送多播报文 */
    ULONG OutBroadcast; /* 发送广播报文 */
    ULONG OutPause;     /* 发送流控报文 */

    /* 错误报文统计 */
    ULONG InError;      /* 接收错误 */
    ULONG InOverflow;   /* 报文过大 */
    ULONG InUnderflow;  /* 报文过小 */
    ULONG InCrcError;   /* CRC错误  */
    ULONG OutError;     /* 发送错误 */
    ULONG OutCollision; /* 冲突     */
    ULONG OutLate;      /* 时延     */
    ULONG ulInDropped;  /* 接收丢弃 */
    ULONG ulInFilter;   /* 接收过滤 */
    ULONG ulOutFilter;  /* 发送过滤 */


    /* 字节数统计 */
    UINT64  uiRecOctets;        /* 接收到的正确字节总数*/
    ULONG   ulRevBadOctets;     /* 接收到的错误字节数 */
    UINT64  uiTransOctets;      /* 发送的字节总数 */
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
    ULONG_8 ullRxPkt;           /* PON口实际接收帧数 */
    ULONG_8 ullTxPkt;           /* PON口实际发送帧数 */
    ULONG_8 ullDropPkt;         /* PON口实际丢弃帧数 */
    ULONG_8 ullCrcErr;          /* PON口实际CRC校验错误帧数 */
    ULONG_8 ullRxErr;           /* PON口实际接收错误帧数    */
    ULONG_8 ullRxByte;          /* PON口实际接收字节数      */
    ULONG_8 ullTxByte;          /* PON口实际发送字节数      */
    ULONG_8 ullAlgErr;          /* 帧同步出错               */
    ULONG_8 ullTooLong;         /* 超大帧                   */
} PON_COUNTERS_S;

typedef enum tagPortDuplexValue
{
    /* 针对命令的单个设置 */
    OAM_PORT_DUPLEX_HALF    =   0,
    OAM_PORT_DUPLEX_FULL    =   1,
    OAM_PORT_DUPLEX_AUTO    =   2
}PORT_DUPLEX_MODE_E ;

/* UNI口MDI模式 */
typedef enum tagUNI_MDI_MODE
{
    MDI_MODE_NORMAL,    /* 直连线    */
    MDI_MODE_CROSS,     /* 交叉线    */
    MDI_MODE_AUTO,      /* MDI自协商 */
    MDI_MODE_BUTT
} UNI_MDI_MODE_E;

/* RXTX动作 */
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
    RXTX_ACT_RX_ETH,    /* 以太网驱动收包 */
    RXTX_ACT_TX_ETH,    /* 以太网驱动发包 */
    RXTX_ACT_TX_PP_PKT,
    RXTX_ACT_RX_PP_PKT,
    RXTX_ACT_TRANS_SMLK,
    RXTX_ACT_TRANS_ONU,
    RXTX_ACT_BUTT
} RXTX_ACT_E;

typedef struct tagONU_BOARD_UID
{
    UCHAR aucBoardType[16];     /* 单板类型    */    
    UCHAR aucHostType[16];      /* ONU主机类型 */
    UCHAR aucHandShake[16];    
    UCHAR aucBoardVer[8];       /* 硬件版本    */    
    UCHAR aucSoftVer[8];        /* 软件版本    */
    UCHAR aucEepromVer[8];      /* EEPROM版本  */
    UCHAR aucBootVer[8];        /* boot版本    */
} ONU_BOARD_UID_S;
typedef enum tagINF_ONU_BOARD_TYPE
{
    ONU_BORAD_TYPE_EPN104,    
    ONU_BOARD_TYPE_BUTT
} ONU_BOARD_TYPE_E;

typedef enum  tagEOAM_ONU_AUTHORIZE_STATUS
{
    EOAM_ONU_AUTHORIZE_STATUS_PASS = 0,   /*0：通过鉴权  1：取消鉴权*/
    EOAM_ONU_AUTHORIZE_STATUS_CANCEL,
    EOAM_ONU_AUTHORIZE_STATUS_BUTT
} EOAM_ONU_AUTHORIZE_STATUS_E;

/* EOAM:通知ONU是否shut/undo shut操作*/
typedef enum  tagEOAM_ONU_AUTHORIZE_REASON
{
    EOAM_ONU_AUTHORIZE_REASON_UNDOSHUTDOWN = 0,/*0：undo shut  1: shut*/
    EOAM_ONU_AUTHORIZE_REASON_SHUTDOWN,   
    EOAM_ONU_AUTHORIZE_REASON_BUTT
} EOAM_ONU_AUTHORIZE_REASON_E;

/*----------------------------------------------*
 * 结构体定义                                   *
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
    FILTER_SRC_MAC,     /* 根据源MAC过滤  */
    FILTER_DEST_MAC,    /* 根据目的MAC过滤  */
    FILTER_ETHER_TYPE,  /* 根据以太网过滤   */
    FILTER_VLAN_ID,     /* 根据VLAN ID过滤  */
    FILTER_SRC_IP,      /* 根据源IP过滤     */
    FILTER_DEST_IP,     /* 根据目的IP过滤   */
    BIND_SRC_IP,        /* 绑定源IP         */
    BIND_DEST_IP,       /* 绑定目的IP       */
    FILTER_SRC_TCP,     /* 根据源TCP过滤     */
    FILTER_DEST_TCP,    /* 根据目的TCP过滤   */
    FILTER_SRC_UDP,     /* 根据源UDP过滤     */
    FILTER_DEST_UDP,    /* 根据目的UDP过滤   */
    FILTER_BUTT
} ACL_FILTER_TYPE_E;

/* 报文过滤取值 */
typedef union tagACL_FILTER_VALUE
{
    UCHAR  aucMacAddr[MAC_ADDR_LEN];   /* MAC地址    */
    ULONG usEtherType;                 /* 以太网类型 */
    ULONG usVlanId;                    /* VLAN ID    */
    ULONG  ulIpAddr;                    /* IP地址     */
    ULONG usPort;                      /* TCP/UDP port */
} ACL_FILTER_VALUE_U;

typedef enum tagONU_DIRECTION
{
    DIRECTION_UPLINK,   /* 上行方向   */
    DIRECTION_DOWNLINK, /* 下行方向   */
    DIRECTION_BOTH,     /* 上下行方向 */
    DIRECTION_BUTT
} ONU_DIRECTION_E;


/* 报文过滤规则 */
typedef struct tagACL_FILTER_RULE
{
    ACL_FILTER_TYPE_E   enType;     /* 过滤类型 */
    ONU_DIRECTION_E     enDirect;   /* 过滤方向 */
    ACL_FILTER_VALUE_U  unValue;    /* 过滤取值 */
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
    UCHAR  aucMacAddr[MAC_ADDR_LEN];   /* MAC地址    */
    ULONG ulValue;                 
} ACL_CLF_RMK_VAL_EN;

/* EOAM协议的OAMPDU中配置操作类型 */
typedef enum tagEOAM_REQEST_ID
{  
    EOAM_GET_UNISTATUS = 0,             /* 0 查询ONU 6301的UNI口状态*/
    EOAM_SET_UNISTATUS,                 /* 1 设置ONU 6301的UNI口状态*/ 
    EOAM_UPDATE_ONUSOFTWARE,            /* 2 远程升级ONU软件 */
    EOAM_DOT1X_CTRL,                    /* 3 启动/关闭802.1x认证 */
    EOAM_MC_FILTER_CTRL,                /* 4 使能/关闭组播过滤功能 */
    EOAM_SET_RESETONU,                	/* 5 复位ONU */
    EOAM_GET_MIBINFO,                   /* 6 查询ONU标准MIB统计信息 */
    EOAM_RESTORE_EEPROM,                /* 7 恢复EEPROM的默认值 */
    EOAM_GET_S3100MAC,                  /* 8 查询31交换机MAC地址信息 */ 
    EOAM_SET_1XNAMEPASSWORD,            /* 9 设置802.1x的用户名和密码 */ 
    EOAM_GET_1XNAMEPASSWORD,            /* a 查询802.1x的用户名和密码 */ 
    EOAM_SET_RSTONUCNT,                 /* b 清空ONU统计计数 */
    EOAM_ADD_IGSPFILTER,                /* c 添加一个多播过滤地址 */
    EOAM_DEL_IGSPFILTER,                /* d 删除一个多播过滤地址 */
    EOAM_NOTIFY_BRIGDEMAC,             	/* e 发送65桥Mac地址给子卡 */
    EOAM_GET_IGSPFILTER,                /* f 获取当前多播过滤地址 */
    EOAM_GET_VERSION,                   /* 10 获取ONU的软硬件版本信息*/
    EOAM_STP_CTRL,                      /* 11 使能/关闭STP功能 */
    EOAM_OPTION82_CTRL,                 /* 12 使能/关闭Option82功能 */
    EOAM_WFQ_SPQ_SWITCH,                /* 13 QoS模式切换*/
    EOAM_BIND_IPADDRESS,                /* 14 IP地址绑定*/
    EOAM_GET_CPURATE,                   /* 15 CPU占用率查询*/
    EOAM_GET_UPDATERECORD,              /* 16 查询升级记录*/
    EOAM_GET_SELFTESTRECORD,            /* 17 查询自检记录*/
    EOAM_UPDATE_EEPROM,                 /* 18 升级ONU EEPROM*/
    EOAM_SAVE_EEPROM,                   /* 19 导出ONU EEPROM*/
    EOAM_PRIORITYMODE_CTRL,             /* 1a 优先级模式切换，IP TOS/COS/DSCP*/
    EOAM_SUBPORT_ENABLE,                /* 1b 子端口打开/关闭设置*/
    EOAM_SUBPORT_VLANMODE_CTRL,         /* 1c 子端口VLAN 模式切换*/
    EOAM_SET_SUBPORT_VLAN,              /* 1d 子端口pvid设置*/
    EOAM_SET_SUBPORT_PRIORITY,          /* 1e 子端口优先级设置*/
    EOAM_SET_SUBPORT_BANDWIDTH,         /* 1f 子端口上下行带宽设置*/
    EOAM_SUBPORT_FLOW_CTRL,             /* 20 子端口流控设置*/
    EOAM_SUBPORT_SPEED_CTRL,            /* 21 子端口速度模式设置*/
    EOAM_SUBPORT_DUPLEX_CTRL,           /* 22 子端口双工模式设置*/
    EOAM_GET_SUBPORT_DATA,              /* 23 获取ONU 子端口的配置、状态、统计数据*/
    EOAM_SET_SUBPORT_TRUNKVLAN,         /* 24 设置ONU子端口的trunk list*/
    EOAM_FLASH_DATATEST,                /* 25 Flash的数据线测试*/
    EOAM_FLASH_ADDRTEST,                /* 26 Flash的地址线测试*/
    EOAM_FLASH_BLOCKTEST,               /* 27 Flash的块测试*/
    EOAM_WATCHDOG_TEST,                 /* 28 看门狗测试*/
    EOAM_EEPROM_TEST,                   /* 29 EEPROM测试*/
    EOAM_INNER_LOOPBACK,                /* 2a 内环回测试*/
    EOAM_EXTERN_LOOPBACK,               /* 2b 外环回测试*/
    EOAM_OPTICS_STATUSTEST,             /* 2c 光模块状态查询测试*/
    EOAM_GET_VERSION_EQUIP,             /* 2d 获取ONU的版本信息测试，装备命令*/
    EOAM_GET_SDRAMTEST,                 /* 2e SDRAM测试信息查询*/
    EOAM_GET_MANUINFO,                  /* 2f 获取制造信息*/
    EOAM_SET_MANUINFO,                  /* 30 设置制造信息*/
    EOAM_SET_RSTSUBPORTCNT,             /* 31 清空ONU子端口的计数器*/
    EOAM_SET_SUBPORT_MDI,               /* 32 子端口的MDI模式设置*/
    EOAM_SUBPORT_QINQ_CTRL,             /* 33 ONU的QinQ模式设置*/
    EOAM_GET_IGSP_DATA,                 /* 34 查询ONU IGSP协议的相关数据*/
    EOAM_GET_STP_DATA,                  /* 35 查询ONU STP协议的相关数据*/
    EOAM_UPDATE_ONUBOOTROM,             /* 36 升级ONU的bootrom*/
    EOAM_IGSP_CTRL,                     /* 37 使能/关闭IGSP功能 */
    EOAM_SET_DEBUG_DEST,                /* 38 设置ONU调试信息输出目标 */
    EOAM_SET_ONU_MAC,                   /* 39 设置ONU的MAC地址*/
    EOAM_GET_ONU_MAC,                   /* 3a 获取ONU的MAC地址*/
    EOAM_SET_ONU_CMD,                   /* 3b 下发ONU调试命令行 */
    EOAM_DEL_6045_MAC_TBL,              /* 3c 删除6045 动态MAC地址表 */
    EOAM_SET_SUBPORT_CFG,               /* 3d 下发子端口配置 */
    EOAM_SET_ONU_MSG_FILTER,            /* 3e 设置报文过滤规则 */
    EOAM_SET_ONU_FLOW_CLASS,            /* 3f 设置流分类规则 */
    EOAM_SET_ONU_QUEUE_SIZE,            /* 40 设置ONU优先级队列的大小 */
    EOAM_SET_ONU_UNI_MAC_NUM,           /* 41 设置ONU UNI口MAC学习限制*/
    EOAM_GET_ONU_PON_STATISTICS,        /* 42 查询ONU PON口统计计数 */    
    EOAM_SET_LIGHT_MODE,                /* 43 设置ONU长发光状态 */
    EOAM_SET_PQ_POLICER,                /* 44 设置pq policer */
    EOAM_DIAG_CONSOLE_TEST,             /* 45 ONU串口环回诊断测试 */
    EOAM_ADD_STATIC_MAC,                /* 46 添加6301静态MAC */
    EOAM_DEL_STATIC_MAC,                /* 47 删除6301静态MAC */
    EOAM_SET_SUBPORT_ISOLATE,           /* 48 设置ONU子端口隔离 */
    EOAM_GET_ONU_LIGHT_MODE,            /* 49 获取ONU长发光状态 */
    EOAM_SET_ONU_WORK_MODE ,            /* 4a 设置ONU工作模式 */
    EOAM_SET_ONU_IP_ADDR,               /* 4b 设置ONU IP地址 */
    EOAM_SET_UNI_BLOCK,                 /* 4c 75设置stp状态block onu的下行口 */
    EOAM_SET_LED_CTRL,                  /* 4d LED控制 */
    EOAM_SET_6045_TEST,                 /* 4e 6045测试 */
    EOAM_GET_BOARD_INFO,                /* 4f 获取版本信息 */
    EOAM_GET_OPTICMODE_TYPE,            /* 50 获取ONU光模块类型 */
    EOAM_PPPOE_PLUS_CTRL,               /* 51 使能/关闭PPPoE+功能 */
    EOAM_SET_ONU_PORT_NUM,              /* 52 设置ONU的在75对应的端口号(包括:槽位号, OLT号, 对应的端口号) */
    EOAM_SET_COS_MAP_LOCAL,             /* 53 设置ONU设备cos到local优先级映射*/
    EOAM_GET_DHCP_CLIENT_INFO,          /* 54 显示dhcp-client信息 */
    EOAM_SET_UNI_IGSP_FAST_LEAVE,       /* 55 设置组播快速离开功能 */
    EOAM_SET_UNI_MIRROR,                /* 56 设置镜像功能 */
    EOAM_SET_UNI_MONITOR,               /* 57 设置监视功能 */
    EOAM_DHCP_CTRL,                     /* 58 设置DHCP协议使能功能 */
    EOAM_SET_MANAGEMENT_VLAN,           /* 59 设置ONU管理VLAN */
    EOAM_SET_MANAGEMENT_VLAN_INTF,      /* 5a 设置管理VLAN接口状态 */
    EOAM_SET_IGSP_PRO,                  /* 5b 设置组播参数功能 */
    EOAM_SET_DHCP_CLIENT,               /* 5c 设置ONU DHCP client使能 */
    EOAM_GET_DHCP_SNOOPING_INFO,        /* 5d 显示dhcp-snooping信息 */
    EOAM_QINQ_CTRL,                     /* 5e 设置QinQ的参数 */
    EOAM_NOTIFY_ONU_AUTHORIZE,          /* 5f 通知ONU鉴权状态 */
    EOAM_NOTIFY_ACCESS_PVID,            /* 60 设置access模式*/
    EOAM_GET_UNI_VCT,                   /* 61 获取ONU VCT结果*/
    EOAM_SET_ONU_EVENT,                 /* 62 设置ONU上报调试信息的等级*/
    EOAM_ADD_STATIC_MCAST_MAC,          /* 63 添加ONU静态组播MAC */
    EOAM_DEL_STATIC_MCAST_MAC,          /* 64 删除ONU静态组播MAC */
    EOAM_SET_SUBPORT_QINQ,              /* 65 设置31Qinq VLAN*/
    EOAM_GET_MULTI_PON_PHY_INFO,        /* 66 获取ONU双PON相关物理信息*/
    EOAM_GET_ONU_SMART_LINK_INFO,       /* 67 获取ONU SMART LINK状态信息*/
    EOAM_GET_S485_PROPERTYCFG,          /* 68 查询ONU 485串口属性配置信息*/
    EOAM_GET_S485_SESSIONCFG_G,         /* 69 查询ONU 485串口会话全局配置信息*/
    EOAM_GET_S485_SESSIONCFG_D,         /* 6a 查询ONU 485串口会话详细配置信息*/
    EOAM_SET_S485_SESSIONCFG_D,         /* 6b 下发ONU 485串口会话详细配置*/
    EOAM_SET_S485_PROPERTYCFG,          /* 6c 下发ONU 485串口属性配置信息*/
    EOAM_SET_SUBPORT_SPEED_1000,        /* 6d 设置子端口速率为1000M */
    EOAM_SET_ONU_LUMINESCENCE_OFF,       /* 6e 设置关闭onu光模块发光一段时间*/
    
    EOAM_SET_ECONUVLANID   = 0x0100,    /* 设置EC ONU子卡VLANID */    
    EOAM_SET_ECONUPRIORITY = 0x0101,    /* 设置EC ONU子卡端口优先级 */
    EOAM_SET_ECONU_LED     = 0x0102,    /* 设置EC ONU子卡的LED */
    EOAM_SET_ET704_VLANMODE = 0x0200,   /* 设置ET704的VLAN模式 */
    EOAM_SET_ET704_SUBPORTCFG = 0x0201, /* 批量下发ET704子端口配置 */
    EOAM_NOTIFY_LOOPBACK = 0x0202,      /* 通知ONU下有环路 */
    EOAM_NOTIFY_NO_LOOPBACK = 0x0203,   /* 通知ONU下环路消失 */
    EOAM_REQEST_BUTT,
    
    EOAM_GET_ONU_STRINGS = 0xFFFD,      /* 从ONU获取字符串信息 */
    EOAM_BATCH_GET = 0xEEEE,            /* 批量配置查询*/
    EOAM_UNI_BATCH_SET = 0xFFFE,        /* UNI批量配置设置 */
    EOAM_BATCH_SET = 0xFFFF             /* 批量配置设置*/
} EOAM_REQEST_ID_E;

/* EOAM协议事件类型 */
typedef enum  tagEOAM_EVENT_TYPE
{
    EOAM_EVENT_SUBPORT_STATUS_CHANGE = 0,   /* UNI口状态变化 */
    EOAM_EVENT_UPDATE_SOFTWARE_FAILURE,     /* ONU软件升级失败 */
    EOAM_EVENT_DEBUG_INFO_REPORT,           /* ONU上报调试信息 */
    EOAM_EVENT_MC_MAC_IS_EMPTY,             /* 组播MAC表为空 */
    EOAM_EVENT_SMLK_NEIGHBOR_FROM31,        /* Added for SMLK:OLT port receive flush packets */
    EOAM_EVENT_SET_TRUNK_FINISH,            /* trunk list设置完成上报 */    
    EOAM_EVENT_DEBUG_STRING,  /*上报debug信息，显示需要调试开关*/
    EOAM_EVENT_LOG_STRING,    /*上报log信息，显示不需要调试开关*/
    EOAM_EVENT_TRAP_STRING,   /*上报trap信息，显示不需要调试开关*/
    EOAM_EVENT_LAST_HANDLER
} EOAM_EVENT_TYPE_E;

typedef enum tagEOAM_LED_CTRL_CODE
{
    EOAM_LED_CTRL_ON,               /* 点亮 */
    EOAM_LED_CTRL_OFF,              /* 熄灭 */
    EOAM_LED_CTRL_DEFAULT,          /* 还原 */
    EOAM_LED_CTRL_BUTT              /* 空操作 */
}EOAM_LED_CTRL_CODE_E;

/* ONU发光模式 */
typedef enum tagLIGHT_MODE
{
    LIGHT_MODE_OFF    = 0,      /* 不发光   */
    LIGHT_MODE_ALWAYS = 1,      /* 长发光   */
    LIGHT_MODE_NOMAL  = 2,      /* 正常发光 */
    LIGHT_MODE_BUTT
} LIGHT_MODE_E;

/* 批量下发配置 */
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
    QOS_QUEUE_SCHEDULER_MODE_WFQ,    /* 8421加权优先级 */
    QOS_QUEUE_SCHEDULER_MODE_SPQ,    /* 绝对优先级 */
    QOS_QUEUE_SCHEDULER_MODE_BUTT
} QOS_QUEUE_SCHEDULER_MODE_E;

typedef enum tagQOS_Pri_Trust_Mmode
{
    QOS_PRI_TRUST_MODE_IP_TOS,     /* IP TOS 优先级  */
    QOS_PRI_TRUST_MODE_DOT1P,      /* 802.1p优先级   */
    QOS_PRI_TRUST_MODE_DSCP,       /* DSCP优先级     */
    QOS_PRI_TRUST_MODE_PORT,       /* 信任端口优先级 */
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

/* 端口UP/DOWN端态 */
typedef enum tagPORT_STATUS
{
    STATUS_UP,      /* 端口为UP   */
    STATUS_DOWN,    /* 端口为DOWN */
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

/* UNI速率模式 */
typedef enum tagUNI_SPEED_MODE
{
    SPEED_MODE_AUTO  = 0,    /* 速率自协商 */
    SPEED_MODE_10M   = 10,   /* 10M速率    */
    SPEED_MODE_100M  = 100,  /* 100M速率   */
    SPEED_MODE_1000M = 1000,
    SPEED_MODE_BUTT
} UNI_SPEED_MODE_E;

/* UNI双工模式 */
typedef enum tagUNI_DUPLEX_MODE
{
    DUPLEX_MODE_HALF,   /* 半双工     */
    DUPLEX_MODE_FULL,   /* 全双工     */
    DUPLEX_MODE_AUTO,   /* 双工自协商 */
    DUPLEX_MODE_BUTT
} UNI_DUPLEX_MODE_E;

#define CABLE_PAIR_NUM 4
#define     VCT_NOT_SUPPORT   0xFFFFFFFF      /*不支持该检测项*/

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
    VCT_CABLE_STATUS_E aenCableStatus[CABLE_PAIR_NUM]; /*4是双绞线对数*/
    USHORT ausCableLen[CABLE_PAIR_NUM];
}VCT_CABLE_DIAG_S;

typedef enum enVctTestStatus
{
    TEST_FAIL = 0,      /*检测失败*/
    NORMAL_CABLE,       /*电缆正常*/
    OPEN_CABLE,         /*电缆开路*/
    SHORT_CABLE,        /*电缆短路*/
    ABNORMAL_CABLE,     /*区分不出开路还是短路*/
}VCT_TEST_STATUS_E;

typedef struct tagVctTestDiag
{
    VCT_TEST_STATUS_E enCableStatus;    /* 支持，电缆状态 */
    ULONG   ulCableLen;                 /* 支持，故障的位置或link时线长 */
    ULONG   ulImpedanceMismatch;        /* VCT_NOT_SUPPORT or BOOL_TRUE or BOOL_FALSE，不支持的项填VCT_NOT_SUPPORT，下同 */
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
 * 外部变量                                     *
 *----------------------------------------------*/
extern const UCHAR g_aucH3cOui[EOAM_OUI_SIZE];
extern const UCHAR g_aucPmcOui[EOAM_OUI_SIZE];

/* 长发光标志 */
extern UINT8 g_bLightAlways;
extern USHORT g_usBootLoaderVer;
extern UCHAR g_ucOnuUpgradeId;
/*----------------------------------------------*
 * 外部函数                                     *
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
