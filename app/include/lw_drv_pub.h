/*****************************************************************************
  --------------------------------------------------------------------------

*****************************************************************************/
#ifndef _LW_DRV_PUB_H_
#define _LW_DRV_PUB_H_

#ifdef  __cplusplus
extern "C"{
#endif

#include "lw_config.h"

#include "pdt_config.h"
/***********************************************************************/

/***********************************************************************/
#define IP_ADDR_LEN             4
#define IP_ADDR_STR_LEN         15
#define MAC_ADDR_LEN             6
#define MAX_MANUCODE_LEN        30
/*Begin add by huangmingjian 2013/03/15 for EPN204QID0033*/ 
#define LASEON_TEST_ENABLE  1
#define LASEON_TEST_DISABLE 0
/*End add by huangmingjian 2013/03/15 for EPN204QID0033*/


/*Begin modified by sunmingliang for bug 199*/
#define REBOOT_LOCK "/tmp/reboot.lock"
/*End modified by sunmingliang for bug 199*/
typedef char cli_system_name_t[MAX_SYSTEM_NAME_LEN + 1];
typedef char cli_password_t[MAX_PASSWORD_LEN + 1];
typedef char vlan_desc_t[MAX_VLAN_DESC_LEN + 1];
typedef char vlanif_desc_t[MAX_VLANIF_DESC_LEN + 1];
typedef char port_desc_t[MAX_PORT_DESC_LEN + 1];
typedef char ip_address_str_t[IP_ADDR_STR_LEN + 1];
typedef unsigned char ip_address_t[IP_ADDR_LEN];
typedef unsigned char mac_address_t[MAC_ADDR_LEN];   /* MAC 地址数据类型 (UINT8[])*/




/***********************************************************************/
/* 端口数定义 */
/***********************************************************************/
#define LOGIC_PORT_NO   MAX_PORT_NUM
#if defined(CONFIG_PRODUCT_EPN104)
#define LOGIC_CPU_PORT  (LOGIC_PORT_NO + 1)
#elif defined(CONFIG_PRODUCT_EPN104N) || defined(CONFIG_PRODUCT_EPN104W) || defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN105) || defined(CONFIG_PRODUCT_GPN104N)
#define LOGIC_CPU_PORT  (LOGIC_PORT_NO + 3)
#elif defined(CONFIG_PRODUCT_EPN101R) || defined(CONFIG_PRODUCT_EPN101ZG)
#define LOGIC_CPU_PORT  (LOGIC_PORT_NO + 6)
#elif defined(CONFIG_PRODUCT_5500)
#define LOGIC_CPU_PORT  (LOGIC_PORT_NO + 4)
#endif
#define INVALID_PORT    0xFFFFFFFF
#define STACK_PORT      0xFFFF0000
#define CHIP_NO         MAX_CHIP_NUM
#define DEVICE_NO       MAX_DEVICE_NUM
#define MAX_SLOT_NO     MAX_SLOT_NUM
#define DEVICE1_SLOT0_PORT_NO FE_PORT_NO
#define DEVICE1_SLOT1_PORT_NO DRV1_PER_COMBO_HAVE_PORT_NO
#define DEVICE1_SLOT2_PORT_NO DRV1_PER_COMBO_HAVE_PORT_NO
#define MIN_FIBER_PORT_NUM MIN_COMBO_PORT_NUM

/***********************************************************************/
/* 端口号和端口掩码定义 */
/***********************************************************************/


/*定义端口的物理ID*/
/*  | ChipId(8bit) | PortId(24bit) | */
typedef UINT32 phyid_t;

#define TOPHYID(_chipid, _portid)   (((((phyid_t)(_chipid))&0xff)<<24) | (((phyid_t)(_portid))&0xffffff))
#define CHIPID(_phyid)      ((((phyid_t)(_phyid))>>24)&0xff)
#define PORTID(_phyid)       (((phyid_t)(_phyid))&0xffffff)

typedef UINT32 sysWord_t;

#define PORT_MASK_STR_LENGTH (3 * LOGIC_PORT_NO)

#define PORT_MASK_WORD_WIDTH  (8 * sizeof(sysWord_t))

/*定义掩码的字数，根据最大端口数计算*/
#define PORT_MASK_WORD_MAX  ((LOGIC_PORT_NO+PORT_MASK_WORD_WIDTH-1)/PORT_MASK_WORD_WIDTH)

/*定义掩码的结构，驱动层使用的都是逻辑端口的掩码*/
typedef struct tagDrv_pmask {
	sysWord_t	pbits[PORT_MASK_WORD_MAX];
} DRV_PMASK_T;


/*定义驱动层使用的逻辑掩码类型*/
#define logic_pmask_t  DRV_PMASK_T

/*定义逻辑端口掩码的宽度*/
#define LOGIC_MASK_WIDTH    (8 * sizeof(logic_pmask_t))

/*此数组中定义每个芯片最大端口号*/
extern UINT32 g_szChipMaxPort[];

#define IfLgcMaskSet(lgcPMask_ptr, lport)\
    for((lport)=1; (lport)<=LOGIC_PORT_NO; (lport)++)\
        if(TstLgcMaskBit((lport), (lgcPMask_ptr)))

#define IfLgcMaskSetNoCheck(lgcPMask_ptr, lport)\
            for((lport)=1; (lport)<=(LOGIC_PORT_NO+1); (lport)++)\
                if(TstLgcMaskBitNoCheck((lport), (lgcPMask_ptr)))

#define IfPhyMaskSet(phyPMask_ptr, _chip, _pport)\
    for((_chip)=0; (_chip)<CHIP_NO; (_chip)++)\
    for((_pport)=0; (_pport)<(g_szChipMaxPort[(_chip)]); (_pport)++)\
        if(TstPhyMaskBit(TOPHYID(_chip, _pport), (phyPMask_ptr)))

#define LgcPortFor(lport)\
    for((lport)=1; (lport)<=LOGIC_PORT_NO; (lport)++)

#define LgcPortForNoCheck(lport, lgcPMask_ptr) \
            for ( lport = 1 ; lport <= LOGIC_CPU_PORT ; lport++ ) \
               if(TstLgcMaskBitNoCheck(lport, lgcPMask_ptr))

#define PhyPortFor(_chip,_pport)\
    for((_chip)=0; (_chip)<CHIP_NO; (_chip)++)\
    for((_pport)=0; (_pport)<(g_szChipMaxPort[(_chip)]); (_pport)++)

//#define IsValidLgcPort(_lport)  (((_lport) <= LOGIC_PORT_NO) && ((_lport) >= 1))

#define UnitFor(unit)\
    for(unit=0; unit<CHIP_NO; unit++)

#if 0
#define ForEachEthPort(_lport)                              \
    for((_lport)=1; (_lport)<=LOGIC_PORT_NO; (_lport)++)    \
        if(IS_ETH_PORT((_lport)) | IS_CABLE_PORT((_lport)))
#else
#define ForEachEthPort(_lport)                              \
    for((_lport)=1; (_lport)<=LOGIC_PORT_NO; (_lport)++)    \
        if(IS_ETH_PORT((_lport)))
#endif

/***********************************************************************/
/* 端口类型定义 */
/***********************************************************************/
#define SEC_MASK    0xff
#define PORT_TYPE_DEF(_res,_tp,_ud,_sp) ((((_res)&SEC_MASK)<<24) | (((_tp)&SEC_MASK)<<16) |(((_ud)&SEC_MASK)<<8) | ((_sp)&SEC_MASK))
#define PT_TP(_type)        (((_type)>>16)&SEC_MASK)
#define PT_UD(_type)        (((_type)>>8)&SEC_MASK)
#define PT_SP(_type)        ((_type)&SEC_MASK)


/*端口类型中的速率*/
#define SP_NULL         0
#define SP_100          1
#define SP_1000         2
#define SP_10G          3
#define SP_4G           4
/*端口类型中的类型*/
#define TP_COPPER       0
#define TP_FIBER        1
#define TP_COMBO        2
#define TP_CPU          3
#define TP_CABLE        5

/*端口类型中的属性*/
#define UD_DOWNLINK     0       /*下行端口*/
#define UD_UPLINK       1       /*上行端口*/

#define TO_USERPORT(_dev, _slot, _port)     ((((UINT32)((_dev)&0xff))<<24) | (((UINT32)((_slot)&0xff))<<16) | (((UINT32)((_port)&0xff))))
#define DEV_NO(_uport)      ((((UINT32)(_uport))>>24)&0xff)
#define SLOT_NO(_uport)     ((((UINT32)(_uport))>>16)&0xff)
#define PORT_NO(_uport)     (((UINT32)(_uport))&0xff)

typedef struct tagPortType{
    user_port_num_t userPortNum;
    UINT32 phyid;
    UINT32 type;
}PORT_TYPE_S;

/*定义端口类型数组，以逻辑端口号为索引*/
extern PORT_TYPE_S g_szPortType[];
extern PORT_TYPE_S g_szComboPortSecType[];
extern phyid_t g_szPhyToLogicMap[];
extern UINT32 g_szUserMaxPortNum[DEVICE_NO+1][MAX_SLOT_NO];


#define PORT_TYPE(_lport)   g_szPortType[(_lport)].type
#define PORT_PHYID(_lport)  g_szPortType[(_lport)].phyid
#define PORT_USERPORT(_lport)   g_szPortType[(_lport)].userPortNum
#define PORT_COMBO_SEC_USERPORT(_lport) g_szComboPortSecType[(_lport)].userPortNum
#define TOLGCPORT(_pport)   g_szPhyToLogicMap[(_pport)]
#define USER_PORT_MAX_NUM(_dev, _slot)  g_szUserMaxPortNum[(_dev)-1][(_slot)]


/*定义输出端口类型的字符串*/
#define PORT_TYPE_100M_STRING      "ethernet%d/%d/%d"
#define PORT_TYPE_1000M_STRING     "gigabitethernet%d/%d/%d"

#define PORT_TYPE_100M_CAP_STRING  "Ethernet%d/%d/%d"
#define PORT_TYPE_1000M_CAP_STRING "GigabitEthernet%d/%d/%d"

#define RX    0x01      /*端口接收方向进行限速*/
#define TX    0x02      /*端口发送方向进行限速*/

/*判断是否百兆端口*/
#define IS_100M_PORT(_lport)    (PT_SP(PORT_TYPE(_lport))==SP_100)

/*判断是否千兆端口*/
#define IS_1000M_PORT(_lport)      (PT_SP(PORT_TYPE(_lport))==SP_1000)

/*判断是否10G端口*/
#define IS_10G_PORT(_lport)      (PT_SP(PORT_TYPE(_lport))==SP_10G)
/*判断是否是电口*/
#define IS_COPPER_PORT(_lport)   (PT_TP(PORT_TYPE(_lport))==TP_COPPER)

/*判断光电复用口*/
#define IS_COMBO_PORT(_lport)   (PT_TP(PORT_TYPE(_lport))==TP_COMBO)

/*判断是否光口*/
#define IS_FIBER_PORT(_lport)   (PT_TP(PORT_TYPE(_lport))==TP_FIBER)

/*判断是否是下行口*/
#define IS_DOWNLINK_PORT(_lport)  (PT_UD(PORT_TYPE(_lport)) == UD_DOWNLINK)

/*判断是否是上行口*/
#define IS_UPLINK_PORT(_lport)  (PT_UD(PORT_TYPE(_lport)) == UD_UPLINK)

/*判断是网口*/
#define IS_ETH_PORT(_lport)  ((PT_TP(PORT_TYPE(_lport)) == TP_COPPER) || \
    (PT_TP(PORT_TYPE(_lport)) == TP_FIBER) || (PT_TP(PORT_TYPE(_lport)) == TP_COMBO))


/***********************************************************************/
/* 端口工作模式属性类型 */
/************************************************************************/
typedef  enum{     /*针对速率双工的所有的可能值*/
    PORT_10HALF     =   0,
    PORT_10FULL     =   1,
    PORT_10AUTO     =   2,

    PORT_100HALF    =   3,
    PORT_100FULL    =   4,
    PORT_100AUTO    =   5,

    PORT_1000HALF   =   6,
    PORT_1000FULL   =   7,
    PORT_1000AUTO   =   8,

    PORT_AUTOHALF   =   9,
    PORT_AUTOFULL   =   10,
    PORT_AUTOAUTO   =   11,
    PORT_MODE_MAX
}PortMode_E;

typedef enum{    /*针对命令的单个设置*/
    PORT_SPEED_10   =   0,
    PORT_SPEED_100  =   1,
    PORT_SPEED_1000 =   2,
    PORT_SPEED_AUTO =   3,
    PORT_SPEED_MAX
}PortSpeedValue_E;
#define PORT_SPEED_10_STR   "10"
#define PORT_SPEED_100_STR  "100"
#define PORT_SPEED_1000_STR "1000"
#define PORT_SPEED_AUTO_STR "auto"

#define INF_PORT_SPEED_AUTO  0
#define INF_PORT_SPEED_10M   10
#define INF_PORT_SPEED_100M  100
#define INF_PORT_SPEED_1000M 1000
#define INF_PORT_SPEED_10G   10000
#define INF_PORT_SPEED_12G   12000
#define INF_PORT_SPEED_24G   24000

typedef enum{    /*针对命令的单个设置*/
    PORT_DUPLEX_HALF    =   0,
    PORT_DUPLEX_FULL    =   1,
    PORT_DUPLEX_AUTO    =   2,
    PORT_DUPLEX_MAX
}PortDuplexValue_E;
#define PORT_DUPLEX_HALF_STR "half"
#define PORT_DUPLEX_FULL_STR "full"
#define PORT_DUPLEX_AUTO_STR "auto"


typedef enum{
    PORT_TYPE_NOT_FIBER     =   0,
    PORT_TYPE_FIBER_100     =   1,
    PORT_TYPE_FIBER_1000    =   2,
    PORT_TYPE_FIBER_UNKNOWN =   3,
}PortType_E;

#define PORT_TYPE_IS_COPPER 0
#define PORT_TYPE_IS_FIBER  1
#define PORT_TYPE_ALL       2

/* 端口配置属性 */
typedef enum
{
    PORT_ENABLE,
    PORT_DISABLE,
    PORT_SPEED,
    PORT_DUPLEX,
    PORT_DUPLEX_FIBER,
    PORT_JUMBO_FRAME,
    PORT_FLOWCON,
    PORT_MDI,
    PORT_LINKTYPE,
    PORT_TRUNKALL,
    PORT_STORMRATE,
    PORT_STORMTYPE,
    PORT_RATEIN,
    PORT_RATEOUT,
    PORT_PVID,
    PORT_QOS,
    PORT_MAC_LIMIT,
    PORT_MAC_LIMIT_DISABLE_FORWARD,
    PORT_CPU_ATTACK_DETECT,
    PORT_PORPERTY_END
}PortProperty_E;

typedef struct tagComboPortConfig{
    UINT32 enable;
    UINT32 autoneg_enable;
    UINT32 autoneg_advert;
    PortSpeedValue_E  force_speed;
    PortDuplexValue_E force_duplex;
    UINT32 mdix;
    UINT32 port_type;
} COMBO_PORT_CONFIG_S;

/*drv port*/
/*注意:本结构与PORT_CFG_STR_S结构一一对应*/
typedef struct tagPORT_CFG_VALUE
{
    UINT32 ulIfEnable;
    UINT32 ulPriority;
    UINT32 ulMdix;
    UINT32 ulFlowCtl;
    UINT32 ulJumboEn;
    UINT32 ulIsolateEn;
    UINT32 ulPowerDownEn;
    UINT32 ulEEEEn;
    PortSpeedValue_E stSpeed;
    PortDuplexValue_E stDuplex;
    port_desc_t   stIfDesc;
    UINT32 ulRateIn;
    UINT32 ulRateOut;
}PORT_CFG_VALUE_S;


typedef struct tagPORT_LINERATE_VALUE
{
    UINT32 ulRateIn;
    UINT32 ulRateOut;
} PORT_LINERATE_VALUES;


#define PORT_SET_NO_CHANGE  255

#define PORT_LINK_DOWN      0
#define PORT_LINK_UP        1

/*针对admin的两个值*/
#define PORT_ADMIN_DISABLE     0
#define PORT_ADMIN_ENABLE      1

/*针对流控的两个值*/
#define PORT_FLOWCTRL_DISABLE       0
#define PORT_FLOWCTRL_ENABLE        1
/*针对LOOPBACK的两个值*/
#define PORT_LOOPBACK_DISABLE     0
#define PORT_LOOPBACK_ENABLE      1

/*针对mdix的设置*/
#define PORT_MDI_ACROSS          0
#define PORT_MDI_AUTO            1
#define PORT_MDI_NORMAL          2
#define PORT_MDI_ACROSS_STR "across"
#define PORT_MDI_AUTO_STR   "auto"
#define PORT_MDI_NORMAL_STR "normal"

/*针对jumbo的设置值*/
#define PORT_JUMBO_DISABLE      0
#define PORT_JUMBO_ENABLE       1

/*针对端口隔离的两个值*/
#define PORT_ISOLATE_DISABLE       0
#define PORT_ISOLATE_ENABLE        1

/*针对端口节能的两个值*/
#define PORT_POWERDOWN_DISABLE       0
#define PORT_POWERDOWN_ENABLE        1

/*eee*/
#define PORT_EEE_DISABLE           0
#define PORT_EEE_ENABLE            1



#define ISVALID_SPEED(_sp_)     (((_sp_) >= PORT_SPEED_10) && ((_sp_) <= PORT_SPEED_AUTO))
#define ISVALID_DUPLEX(_du_)    (((_du_) >= PORT_DUPLEX_HALF) && ((_du_) <= PORT_DUPLEX_AUTO))
#define ISVALID_SPDUP(_du_)     (((_du_) >= PORT_10HALF) && ((_du_) <= PORT_AUTOAUTO))
#define ISVALID_MDIX(_mdix)     (((_mdix) == PORT_MDI_ACROSS) || ((_mdix) == PORT_MDI_AUTO) || ((_mdix) == PORT_MDI_NORMAL))
#define ISVALID_FC_VALUE(_fc)   (((_fc) == PORT_FLOWCTRL_DISABLE) || ((_fc) == PORT_FLOWCTRL_ENABLE))
#define ISVALID_JUMBO_VALUE(_jb) (((_jb) == PORT_JUMBO_DISABLE) || ((_jb) == PORT_JUMBO_ENABLE))
#define ISVALID_PORT_PRIORITY(_prio) (((UINT32)(_prio)) <= 7)
#define ISVALID_ADMIN_VALUE(_adm)   (((_adm) == PORT_ADMIN_DISABLE) || ((_adm) == PORT_ADMIN_ENABLE))
#define ISVALID_LOOPBACK_VALUE(_adm)   (((_adm) == PORT_LOOPBACK_DISABLE) || ((_adm) == PORT_LOOPBACK_ENABLE))


/* 端口的连接状态*/
#define LINK_MODE_SPEED_10              0x00
#define LINK_MODE_SPEED_100             0x01
#define LINK_MODE_SPEED_1000            0x02
#define LINK_MODE_SPEED_MASK            0x03
#define LINK_MODE_FDX_HALF              0x00
#define LINK_MODE_FDX_FULL              0x10
#define LINK_MODE_FDX_MASK              0x10
#define LINK_MODE_PAUSE_MASK            0x20
#define LINK_MODE_INT_LOOPBACK          0x80
#define LINK_MODE_DOWN                  0xFF
#define LINK_MODE_FDX_10                (LINK_MODE_SPEED_10 | LINK_MODE_FDX_MASK)
#define LINK_MODE_HDX_10                (LINK_MODE_SPEED_10)
#define LINK_MODE_FDX_100               (LINK_MODE_SPEED_100 | LINK_MODE_FDX_MASK)
#define LINK_MODE_HDX_100               (LINK_MODE_SPEED_100)
#define LINK_MODE_FDX_1000              (LINK_MODE_SPEED_1000 | LINK_MODE_FDX_MASK)
#define LINK_MODE_FDX_AND_PAUSE_MASK    (LINK_MODE_FDX_MASK | LINK_MODE_PAUSE_MASK)
#define LINK_MODE_SPEED_AND_FDX_MASK    (LINK_MODE_SPEED_MASK | LINK_MODE_FDX_MASK)

/*定义端口能力位*/
#define PORT_ABIL_10MB_HD         (1 << 0)
#define PORT_ABIL_10MB_FD         (1 << 1)
#define PORT_ABIL_100MB_HD        (1 << 2)
#define PORT_ABIL_100MB_FD        (1 << 3)
#define PORT_ABIL_1000MB_HD       (1 << 4)
#define PORT_ABIL_1000MB_FD       (1 << 5)
#define PORT_ABIL_2500MB_HD       (1 << 6)
#define PORT_ABIL_2500MB_FD       (1 << 7)
#define PORT_ABIL_10GB_HD         (1 << 8)
#define PORT_ABIL_10GB_FD         (1 << 9)
#define PORT_ABIL_PAUSE_TX        (1 << 10)       /* TX pause capable */
#define PORT_ABIL_PAUSE_RX        (1 << 11)       /* RX pause capable */
#define PORT_ABIL_PAUSE_ASYMM     (1 << 12)       /* Asymm pause capable (R/O) */
#define PORT_ABIL_TBI             (1 << 13)       /* TBI mode supported */
#define PORT_ABIL_MII             (1 << 14)       /* MII mode supported */
#define PORT_ABIL_GMII            (1 << 15)       /* GMII mode supported */
#define PORT_ABIL_SGMII           (1 << 16)       /* SGMII mode supported */
#define PORT_ABIL_XGMII           (1 << 17)       /* XGMII mode supported */
#define PORT_ABIL_LB_MAC          (1 << 18)       /* MAC loopback supported */
#define PORT_ABIL_LB_MAC_RMT      (1 << 19)       /* Remote MAC loopback supt */
#define PORT_ABIL_LB_PHY          (1 << 20)       /* PHY loopback supported */
#define PORT_ABIL_AN              (1 << 21)       /* Auto-negotiation */
#define PORT_ABIL_3000MB_HD       (1 << 29)
#define PORT_ABIL_3000MB_FD       (1 << 30)
#define PORT_ABIL_12GB_HD         (1 << 22)
#define PORT_ABIL_12GB_FD         (1 << 23)
#define PORT_ABIL_13GB_HD         (1 << 24)
#define PORT_ABIL_13GB_FD         (1 << 25)
#define PORT_ABIL_PAUSE           (PORT_ABIL_PAUSE_TX | PORT_ABIL_PAUSE_RX)

typedef enum tagPORTMSG_TYPE
{
    CMD_DRV_MULTISETPORT,
    CMD_DRV_SETPORTADMIN,
    CMD_DRV_SETPORTSPEED,
    CMD_DRV_SETPORTDUPLEX,
    CMD_DRV_SETPORTSPEEDDUPLEX,
    CMD_DRV_SETPORTMDI,
    CMD_DRV_SETPORTPRIORITY,
    CMD_DRV_SETPORTFLOWCTRL,
    CMD_DRV_SETPORTSRXTX,
    CMD_DRV_SETPORTJUMBO,
    CMD_DRV_MULTISETPORTJUMBO,
    CMD_DRV_PORTISOLATE_ADD,
    CMD_DRV_PORTISOLATE_DEL,
    CMD_DRV_CREATE_MIRRORGROUP,
    CMD_DRV_DELMIRRORGROUP,
    CMD_DRV_SETMIRRORGROUP_MONITOR,
    CMD_DRV_SETMIRRORGROUP_MIRROR,
    CMD_DRV_SETPORTLINERATE,       
    CMD_DRV_PORTPOWERDOWN_SET,
    CMD_DRV_EEE_SET,
    CMD_DRV_SETPORTSTORMCONTROL,
    CMD_DRV_SETQOSTRUSTMODE,
    CMD_DRV_SETQOSQUEUE,
}PORT_CMD_E;

/********************************************************************************/
/*                       端口镜像相关定义                                                */
/********************************************************************************/

#ifndef MAX_MIRROR_GROUP_ID
#define MAX_MIRROR_GROUP_ID 1
#endif
#define NOMONITOR 0
#ifndef ID_NULL
#define ID_NULL 0
#endif
typedef struct tagMIRROR_GROUP_STR
{
    UINT32 ulGroupId;
    port_num_t ulPortMonitor;
    logic_pmask_t stMirrPortInList;
    logic_pmask_t stMirrPortOutList;
}MIRROR_GROUP_S;
//广播抑制
typedef enum{
    STORM_DLF_CTRL,     /*unknown unicast*/
    STORM_MCAST_CTRL,   /*known multicast*/
    STORM_BCAST_CTRL,   /*broadcast*/
    STORM_ALL_CTRL      
}STORM_CTLTYPE_E;

typedef enum{
    STORM_RATE_KBPS,
    STORM_RATE_PPS,
    STORM_RATE_PERCENT,
    STORM_RATE_MAX
}STORM_RATETYPE_E;
#define STORM_RATE_KBPS_STR "kbps"
#define STORM_RATE_PPS_STR  "pps"
#define STORM_RATE_PERCENT_STR "ratio"
#define STORM_NOCONTROL 100

typedef struct{
    STORM_RATETYPE_E rateType;
    UINT32 value; /*0 means disable storm control*/
}STORM_CTLRATE_S;
/********************************************************************************/
/*                      QosQueueScheduler                                                */
/********************************************************************************/
typedef struct tagQueueMode
{
    UINT32 QueueMode;           /*QUEUE_MODE_XX*/
    #if (QUEUE_NUM <= 8)
    UINT8 Weight[8];            /*此处的设置是为了兼容以前代码的不合理之处*/
    #else
    UINT8 Weight[QUEUE_NUM];
    #endif
} QueueMode_S;

typedef struct tagPortInPara
{
    PORT_CMD_E cmd;    
    logic_pmask_t stPortList;
    port_num_t lport;
    UINT32 admin_value;
    UINT32 uiSpeed;
    UINT32 uiDuplex;
    UINT32 uiPriority;
    UINT32 uiFlowctrl;
    UINT32 uiMdi;
    UINT32 uiRxTxState;
    UINT32 uiJumbo;
    UINT32 uiPortType;
    MIRROR_GROUP_S stMirrGroup;
    UINT32 uiCIR;
    UINT32 ulDirection;
    UINT32 ulEnable;  
    STORM_CTLTYPE_E stormtype;
    STORM_CTLRATE_S stStrom;
    UINT32 ulTrustMode;
    QueueMode_S stQueue;
}PORT_INPUT_PARA_S;

typedef enum tagAGGR_CFG_CHANGE_MSG_TYPE
{
    AGGR_CFG_CHANGE_TYPE_CREATE,
    AGGR_CFG_CHANGE_TYPE_DELETE,
    AGGR_CFG_CHANGE_TYPE_AGGRTYPE_CHANGE,
    AGGR_CFG_CHANGE_TYPE_ADD_PORT,
    AGGR_CFG_CHANGE_TYPE_DELETE_PORT,
    AGGR_CFG_CHANGE_TYPE_END
}AGGR_CFG_CHANGE_MSG_TYPE_E;

typedef enum tagAGGR_INTERNAL_DATA_MSG_TYPE
{
    AGGR_INTERNAL_DATA_GET_AGGRINFO,
    AGGR_INTERNAL_DATA_GET_AGGRPORTINFO,
    AGGR_INTERNAL_DATA_GET_AGGRFSMPORTINFO,
    AGGR_INTERNAL_DATA_GET_AGGRLACPSYSMAC,
    AGGR_INTERNAL_DATA_GET_AGGRLACPPRI,
    AGGR_INTERNAL_DATA_GET_AGGRPORTMASK,
    AGGR_INTERNAL_DATA_GET_AGGRINFO_ALL,
    AGGR_INTERNAL_DATA_GET_AGGRPORTINFO_ALL,
    /*请在此之前添加*/
    AGGR_INTERNAL_DATA_END
}AGGR_INTERNAL_DATA_MSG_TYPE_E;


typedef struct tagAggrCfgChangeMsg
{
    AGGR_CFG_CHANGE_MSG_TYPE_E cmd;    
    UINT32 ulAggrNo;   
    UINT32 ulType;
    logic_pmask_t stPortMask;
}AGGR_CFG_CHANGE_MSG_S;

typedef struct tagAggrInternalDataMsg
{
    AGGR_INTERNAL_DATA_MSG_TYPE_E cmd;    
    UINT32 ulAggrNo;   
    UINT32 ulType;
    UINT32 ulPortNum;
}AGGR_GET_INTERNAL_DATA_PARA_S;

#define LACP_AGG_DYNAMICAL_STR  "dynamic"
#define LACP_AGG_STATIC_STR "static"
#define LACP_AGG_MANUAL_STR "manual"

#define LACP_BLOCK      1
#define LACP_UNBLOCK    2


#define SMAC_TRUNK_STR "source-mac"
#define DMAC_TRUNK_STR "destination-mac"
#define SADA_TRUNK_STR  "source-mac destination-mac"
#define SIPDIP_TRUNK_STR "source-ip destination-ip"
#define SADAIP_TRUNK_STR "source-mac destination-mac source-ip destination-ip"
typedef enum tagAggr_ReternValue
{
    AGGR_CFG_MSG_OK = 0,
    AGGR_CFG_MSG_ERR,
    AGGR_INTERNAL_DATA_OK,
    AGGR_INTERNAL_DATA_ERR
}AGGR_RET_E;


typedef struct tagStromctlMsg
{
    UINT8 uucasten;   
    UINT8 umcasten;  
    UINT8 bcasten;  
    UINT32 ratepercent;
    
}STROM_CTL_S;


/*定义芯片trunk组id转为系统聚合组号*/
#define TRUNKID2AGGRPNO(x)  ((x) + 1)
#define AGGRPNO2TRUNKID(x)  ((x) - 1)

void Drv_AggrInit(void);
BOOL_T Drv_SyncAggMaskBitExistAndClear(UINT32 ulAggID);
BOOL_T Drv_SyncMaskAggCfgCheckAndClear(VOID);


/* spanning tree state */
typedef enum tagDrvStp_state_e
{
    DRV_STP_STATE_DISABLED = 0,
    DRV_STP_STATE_BLOCKING,
    DRV_STP_STATE_LEARNING,
    DRV_STP_STATE_FORWARDING,
    DRV_STP_STATE_END
} DRV_STP_STATE_E;

#define STP_DEFAULT_INSTANCE 0


#define DRV_ERR_RET_NEGATIVE 0
/*定义驱动层的统一返回类型*/
typedef enum tagDrv_ReturnValue
{
    DRV_OK = 0,
    DRV_ERR_PARA,
    DRV_ERR_UNKNOW,
    DRV_NULL_POINTER,
    DRV_CFG_READ_ERROR,
    DRV_I2C_READ_ERROR,
    DRV_CFG_WRITE_ERROR,
    DRV_ERR_INIT_SWITCH ,
    DRV_INVALID_LPORT,
    DRV_INVALID_LPORTLIST,
    DRV_INVALID_VALUE,
    DRV_INVALID_SPEED_VALUE,
    DRV_INVALID_DUPLEX_VALUE,
    DRV_INVALID_SPDUP_VALUE,
    DRV_INVALID_FLOWCTRL_VALUE,
    DRV_INVALID_PRIORITY_VALUE,
    DRV_INVALID_MDI_VALUE,
    DRV_INVALID_ADMIN_VALUE,
    DRV_INVALID_JUMBO_VALUE,
    DRV_ERR_PORT_SPDUP_IS_AGGRE,
    DRV_ERR_PORT_CABLE_NOT_SUPPORT,
    DRV_ERR_PORT_FIBER_NOT_SUPPORT,
    DRV_ERR_PORT_NOT_SUPPORT_1000,
    DRV_ERR_PORT_NOT_SUPPORT_MDI_AUTO,
    DRV_ERR_PORT_NOT_SUPPORT_GE_MDI,
    DRV_ERR_PORT_NOT_SUPPORT_FIBER_MDI,
    DRV_ERR_PORT_PRIORITY_IS_AGGRE,
    DRV_ERR_PORT_NOT_SUPPORT_FLOWCTRL,
    DRV_ERR_PORT_FIBER_100_NOT_SUPPORT,
    DRV_ERR_PORT_FIBER_1000_NOT_SUPPORT,
    DRV_ERR_PORT_JUMBO_NOT_SUPPORT,
    DRV_ERR_PORT_SEQUENCE,
    DRV_ERR_AGGR_PORT_SINGLE,
    DRV_ERR_AGGR_NOT_SUPPORT_TYPE,
    DRV_ERR_AGGR_MAX_GROUP,
    DRV_ERR_AGGR_FE_GE_TOGETHER,
    DRV_ERR_AGGR_MAX_MEMBER,
    DRV_ERR_AGGR_WRONG_PORT_SPDUP,
    DRV_ERR_AGGR_PORT_IS_AGGRE,
    DRV_ERR_AGGR_PORT_IS_MONITOR,
    DRV_ERR_AGGR_PORT_IS_BIDING,
    DRV_ERR_AGGR_IS_NO_EXIST,
    DRV_ERR_PORT_NOT_CONTINUE,
    DRV_ERR_AGGR_NOT_MASTER,
    DRV_ERR_AGGR_SPEED_IS_AUTO,
    DRV_ERR_AGGR_DUPLEX_NOT_FULL,
    DRV_ERR_AGGR_PORT_IS_ISOLATE,
    DRV_ERR_AGGR_PORT_IS_ON_AUTH,
    DRV_ERR_AGGR_PORT_IS_ON_STP,
    DRV_ERR_AGGR_GROUP_IS_EXIST ,
    DRV_ERR_RATE_PARA,
    DRV_ERR_RATE_VALUE,
    DRV_ERR_PORT_INVALID_LPORT,
    DRV_ERR_STORM_VALUE,
    DRV_ERR_STORM_TYPE,
    DRV_ERR_MIR_GROUP_EXIST,
    DRV_ERR_MIR_GROUP_DOSE_NOT_EXIST,
    DRV_ERR_MIR_MONITOR_EXIST,
    DRV_ERR_MIR_MONITOR_IS_MIRROR,
    DRV_ERR_MIR_MONITOR_IS_AGGRE,
    DRV_ERR_MIR_MIRROR_IS_MONITOR,
    DRV_ERR_MIR_NO_MONITOR,
    DRV_ERR_MIR_NOT_SUPPORT_EGRESS,
    DRV_ERR_MIR_DUPLICATION_SET,
    DRV_ERR_MIR_DUPLICATION_DEL,
    DRV_ERR_MIR_MIR_EXIST,
    DRV_ERR_MIR_FULL,
    DRV_ERR_VLAN_INVALID_VLANID,
    DRV_ERR_VLAN_NOT_EXIST,
    DRV_ERR_VLAN_NOT_ALL_EXIST,
    DRV_ERR_VLAN_DEFAULT_VLAN,
    DRV_ERR_VLAN_MANAGE_VLAN,
    DRV_ERR_VLAN_MAXNUM,
    DRV_ERR_VLAN_IF_EXIST,
    DRV_ERR_VLAN_IF_NOT_EXIST,
    DRV_ERR_VLAN_ADD_MEMBER,
    DRV_ERR_VLAN_REMOVE_MEMBER,
    DRV_ERR_MAC_ADDR_MULTPORT_EXIST,
    DRV_ERR_MAC_ADDR_ALREADY_EXIST,
    DRV_ERR_MAC_ADDR_NO_FREE_ENTRY,
    DRV_ERR_MAC_ENTRY_NOT_EXIST,
    DRV_ERR_MAC_BLACKHOLE,
    DRV_ERR_MAC_STAGGR,
    DRV_ERR_MAC_MULTICAST,
    DRV_ERR_MAC_TAB_FULL,
    DRV_ERR_MAC_TAB_NOT_EXIST,
    DRV_ERR_MAC_TAB_ALREADY_EXIST,
    DRV_ERR_ARL_NO_FREE_ENTRY,
    DRV_ERR_MAC_ADDSELF,
    DRV_ERR_MAC_NOTINVLAN,
    DRV_ERR_MAC_LIMIT_ENABLED,
    DRV_ERR_ADDR_MANUAL_ADD,
    DRV_ERR_MAC_ADDR_NOT_FOUND,
    DRV_ERR_MAC_ADDR_OP_FAIL,/*ADDR_OP_FAIL*/
    DRV_ERR_AGGR_NOT_SUPPORT_MAC_LIMIT,
    DRV_ERR_PORT_BIND_NOT_SUPPORT_MAC_LIMIT,
    DRV_ERR_DOT1X_NOT_SUPPORT_MAC_LIMIT,
    DRV_ERR_PORT_NOT_SUPPORT,
    DRV_ERR_QOS_TRUST_MODE_ERR,
    DRV_ERR_QOS_WRONG_VLAUE,
    DRV_ERR_QOS_NOT_SUPPORT_MODE,
    DRV_ERR_QOS_WRONG_MAP,
    DRV_ERR_ACL_NOT_ENOUGH_RESOURCE,
    DRV_ERR_ALC_VLAN_INTF_FULL,
    DRV_ERR_ACL_VLAN_INTF_EXIST,
    DRV_ERR_ACL_VLAN_INTF_NOT_EXIST,
    DRV_ERR_ACL_PRIO_INVALID,                   /*无效的ACL优先级*/
    DRV_ERR_ACL_ALLREADY_EXIST,                 /*ACL已存在*/
    DRV_ERR_ACL_NAME_EMPTY,                     /*ACL NAME不能为空*/
    DRV_ERR_ACL_NAME_INVALID,                   /*ACL NAME中不能包含;，必须以0x00结尾*/
    DRV_ERR_ACL_NAME_EXIST,                     /*ACL NAME重复*/
    DRV_ERR_ACL_DESCRIPTION_INVALID,            /*ACL DESCRIPTION中不能包含;，必须以0x00结尾*/
    DRV_ERR_ACL_NOT_EXIST,                      /*ACL不存在*/
    DRV_ERR_ACL_RULE_ALLREADY_EXIST,            /*RULE已经存在*/
    DRV_ERR_ACL_RULE_INVALID_PRIO,              /*无效的RULE优先级*/
    DRV_ERR_ACL_MEM_ALLOC_FAILED,               /*ACL内存分配失败*/
    DRV_ERR_ACL_RULE_FULL,                      /*ACL已满*/
    DRV_ERR_ACL_RULE_INVALID_ACTION,            /*无效的ACTION*/
    DRV_ERR_ACL_IP_RULE_INVALID_SRCIPMASK,      /*无效的源IP掩码*/
    DRV_ERR_ACL_IP_RULE_INVALID_DSTIPMASK,      /*无效的目的IP掩码*/
    DRV_ERR_ACL_IP_RULE_INVALID_L4PORTMASK,     /*无效的L4端口号掩码*/
    DRV_ERR_ACL_INVALID_RULE_TYPE,              /*无效的RULE类型*/
    DRV_ERR_ACL_MAC_RULE_INVALID_SRCMACMASK,    /*无效的源MAC掩码*/
    DRV_ERR_ACL_MAC_RULE_INVALID_DSTMACMASK,    /*无效的目的MAC掩码*/
    DRV_ERR_ACL_MAC_RULE_INVALID_VLANID,        /*无效的VLAN ID*/
    DRV_ERR_ACL_RULE_NOT_FOUND,                 /*指定RULE不存在*/
    DRV_ERR_ACL_ALREADY_BOUND,                  /*指定端口已经绑定ACL*/
    DRV_ERR_ACL_NOT_BOUND,                      /*指定端口未绑定ACL*/
    DRV_ERR_ACL_NOT_EMPTY,                      /*指定ACL不为空*/
    DRV_ERR_ACL_EMPTY,                          /*指定ACL为空*/
    DRV_ERR_ACL_RULE_MIRROR_FULL,               /*ACL规则的镜像指定的端口，不同的端口号已设置满3个了*/
    ERR_DHCPSP_INVALID,
    ERR_DHCPSP_AGGPORT_NOT_IPFILTER,
    DRV_ERR_L3_ENTRY_FULL,
    DRV_ERR_L3_ENTRY_EXIST,
    DRV_ERR_L3_ENTRY_NOT_EXIST,
    DRV_ERR_PVLAN_NOT_SUPPORT_PORT_ISOLATE,
    DRV_ERR_FIBER_NOT_SUPPORT_HALF_DUPLEX,
    DRV_ERR_FIBER_NOT_SUPPORT_SPEED,
    DRV_ERR_OPEN_DEV,
    DRV_ERR_NO_MEM,
    DRV_ERR_MSG,
    DRV_IOCTL_ERROR,
    DRV_PARA_NUM_ERROR,
    DRV_INVALID_STATE,
    DRV_SDK_GEN_ERROR,
    DRV_SDK_INVALID_PARA,
    DRV_SDK_INVALID_UNIT,
    DRV_SDK_INVALID_PORTID,
    DRV_SDK_INVALID_MASK,
    DRV_SDK_PORT_LINKDOWN,
    DRV_SDK_INVALID_ENTRY_INDEX,
    DRV_SDK_NULL_POINTER,
    DRV_SDK_INVALID_QUEUE_ID,
    DRV_SDK_INVALID_QUEUE_NUM,
    DRV_SDK_BUSYWAIT_TIMEOUT,
    DRV_SDK_INVALID_MAC,
    DRV_SDK_OUT_OF_RANGE,
    DRV_SDK_CHIP_NOT_SUPPORTED,
    DRV_SDK_INVALID_SMI,
    DRV_SDK_NOT_INIT,
    DRV_SDK_CHIP_NOT_FOUND,
    DRV_SDK_NOT_ALLOWED,
    DRV_SDK_DRIVER_NOT_FOUND,
    DRV_SDK_SEM_LOCK_FAILED,
    DRV_SDK_SEM_UNLOCK_FAILED,
    DRV_SDK_THREAD_EXIST,
    DRV_SDK_THREAD_CREATE_FAILED,
    DRV_SDK_INVALID_FWD_ACTION,
    DRV_SDK_INVALID_IPV4_ADDRESS,
    DRV_SDK_INVALID_IPV6_ADDRESS,
    DRV_SDK_INVALID_PRIORITY,
    DRV_SDK_INVALID_FID,
    DRV_SDK_ENTRY_NOTFOUND,
    DRV_SDK_DROP_PRECEDENCE,
    DRV_SDK_NOT_FINISH,
    DRV_SDK_TIMEOUT,
    DRV_SDK_INVALID_REG_ARRAY_INDEX_1,
    DRV_SDK_INVALID_REG_ARRAY_INDEX_2,
    DRV_SDK_INVALID_ETHER_TYPE,
    DRV_SDK_ERR_PHY_FIBER_LINKUP,
    DRV_SDK_ERR_EEPROM_I2C_NOT_INIT,
    DRV_SDK_ERR_EEPROM_I2C_READ,
    DRV_SDK_ERR_READ_SFP_FORM_DRAM,
    DRV_SDK_ERR_READ_SFP_FORM_I2C,
    DRV_STP_ERR_PORT_STATE,
    DRV_L2_SEND_ERR,
    DRV_INVALID_LOOPBACK_VALUE
}DRV_RET_E;
#define LW_DRV_RET DRV_RET_E
#ifdef DRV_PORT_DEBUG
#define MW_DRV_DBG   MW_DBG_ERR
#else
#define MW_DRV_DBG(fmt,arg...)
#endif

#if defined(__KERNEL__)
DRV_RET_E K_Drv_PortInit(port_num_t lgcPort);
DRV_RET_E K_Drv_PortSetState(port_num_t lgcPort, UINT32 uiState);
DRV_RET_E K_Drv_PortGetState(port_num_t lgcPort, UINT32 *puiState);
DRV_RET_E K_Drv_PortSetSpeed(port_num_t lgcPort, UINT32 uiSpeed);
DRV_RET_E K_Drv_PortGetSpeed(port_num_t lgcPort, UINT32 *puiSpeed);
DRV_RET_E K_Drv_PortSetDuplex(port_num_t lgcPort, UINT32 uiDuplex);
DRV_RET_E K_Drv_PortGetDuplex(port_num_t lgcPort, UINT32 *puiDuplex);
DRV_RET_E K_Drv_PortSetMdix(port_num_t lgcPort, UINT32 uiMdix);
DRV_RET_E K_Drv_PortGetMdix(port_num_t lgcPort, UINT32 *puiMdix);
#else
DRV_RET_E Drv_GetDownLinkPortMask(logic_pmask_t *lmask_ptr);
DRV_RET_E Drv_GetFiberPortMask(logic_pmask_t *lmask_ptr);
DRV_RET_E Drv_GetGePortMask(logic_pmask_t *lmask_ptr);
DRV_RET_E Drv_GetPortState(port_num_t lport, UINT32 *uiState);
DRV_RET_E Drv_GetUpLinkPortMask(logic_pmask_t *lmask_ptr);
BOOL Drv_IsFePort(port_num_t lport);
PortType_E Drv_IsFiberPort(port_num_t lport);
BOOL Drv_IsGePort(port_num_t lport);
BOOL Drv_IsLinkUpPort(port_num_t lport);
DRV_RET_E Drv_MultiGetPortLinkState(logic_pmask_t *pstPortMask);
DRV_RET_E Drv_MultiGetPortMediaFiber(logic_pmask_t *pstPortMask);
DRV_RET_E Drv_MultiSetPort(logic_pmask_t *lportList,  UINT32 ulPortType, UINT32 admin_value, UINT32 uiSpeed, UINT32 uiDuplex, UINT32 uiPriority, UINT32 uiFlowctrl);
DRV_RET_E Drv_MultiSetPortJumbo(logic_pmask_t *lportList, UINT32 uiJumbo);
DRV_RET_E Drv_PortInit(port_num_t lport);
DRV_RET_E Drv_SetPortAdmin(port_num_t lport, UINT32 ulPortType, UINT32 uiAdmin);
DRV_RET_E Drv_SetPortDuplex(port_num_t lport, UINT32 ulPortType, UINT32 uiDuplex);
DRV_RET_E Drv_SetPortFlowctrl(port_num_t lport, UINT32 uiFlowctrl);
DRV_RET_E Drv_SetPortJumbo(port_num_t lport, UINT32 uiJumbo);
DRV_RET_E Drv_SetPortMdi(port_num_t  lport, UINT32 mdi_value, UINT32 ulPortType);
DRV_RET_E Drv_SetPortPriority(port_num_t  lport, UINT32 uiPriority);
DRV_RET_E Drv_SetPortSpdup(port_num_t lport, UINT32 ulPortType, UINT32 uiSpeed, UINT32 uiDuplex);
DRV_RET_E Drv_SetPortSpeed(port_num_t lport, UINT32 ulPortType, UINT32 uiSpeed);
DRV_RET_E Drv_SetPortsTxRx(logic_pmask_t *lmask_ptr, UINT32 state);
DRV_RET_E Drv_GetPortJumbo(port_num_t lport, UINT32 *puiJumbo);
DRV_RET_E Drv_GetPortFlowctrl(port_num_t lport, UINT32 *puiFc);
DRV_RET_E Drv_GetPortPriority(port_num_t lport, UINT32 *puiPrio);
DRV_RET_E Drv_GetPortMdix(port_num_t lport, UINT32 *puiState);
DRV_RET_E Drv_GetPortSupportFrameLen(port_num_t lport, UINT32 *puiLength);
DRV_RET_E Drv_SetPortSupportFrameLen(port_num_t lport, UINT32 uiLength);
DRV_RET_E Drv_PortSupportFrameLenInit(VOID);

#endif

BOOL IsValidLgcPort(port_num_t ucLogicPort);
void ClrLgcMaskAll(logic_pmask_t *lgcPMask_ptr);
void ClrLgcMaskBit(port_num_t ucLogicPort, logic_pmask_t *lgcPMask_ptr);
void ClrLgcMaskNotUsed(logic_pmask_t *lgcPMask_ptr);
port_num_t GetLgcMinPort(logic_pmask_t *lgcPMask_ptr);
INT32 GetLgcMskPortNum(logic_pmask_t *lgcPMask_ptr);
void LgcMaskAnd(logic_pmask_t *mska,logic_pmask_t *mskb);
void LgcMaskCopy(logic_pmask_t *dstMsk,logic_pmask_t *srcMsk);
BOOL LgcMaskIsFull(logic_pmask_t *lgcPMask_ptr);
void LgcMaskNegate(logic_pmask_t *mska,logic_pmask_t *mskb);
BOOL LgcMaskNotNull(logic_pmask_t *lgcPMask_ptr);
void LgcMaskOr(logic_pmask_t *mska,logic_pmask_t *mskb);
void LgcMaskRemove(logic_pmask_t *mska,logic_pmask_t *mskb);
void LgcMaskXor(logic_pmask_t *mska,logic_pmask_t *mskb);
void SetLgcMaskAll(logic_pmask_t *lgcPMask_ptr);
void SetLgcMaskBit(port_num_t ucLogicPort, logic_pmask_t *lgcPMask_ptr);
void SetLgcMaskBitNoCheck(port_num_t ucLogicPort, logic_pmask_t *lgcPMask_ptr);
BOOL TstLgcMaskAndIsNull( logic_pmask_t mska, logic_pmask_t mskb);
BOOL TstLgcMaskBit(port_num_t ucLogicPort, logic_pmask_t *lgcPMask_ptr);
BOOL TstLgcMaskBitNoCheck(port_num_t ucLogicPort, logic_pmask_t *lgcPMask_ptr);
BOOL TransUnit32MaskToLgcMask(unsigned int* pUint32PortMask, logic_pmask_t *lgcPMask_ptr);
BOOL TstLgcMaskRemoveIsNull( logic_pmask_t mska, logic_pmask_t mskb);
port_num_t PortUser2Logic(user_port_num_t stUsrPort);
BOOL IsComboFiberUserPort(user_port_num_t stUsrPort);
user_port_num_t PortLogic2User(port_num_t lport);
user_port_num_t PortLogic2ComboSecUser(port_num_t lport);
BOOL IsValidUserPort(user_port_num_t stUsrPort);
VOID Drv_Ports_Init(VOID);


/***********************************************************************/
/* drv_aggr相关定义 */
/************************************************************************/
#define AggreGrpFor(_grp)\
    for((_grp)=1; (_grp)<=AGGRE_GRP_NUM; (_grp)++)



typedef enum
{
	AGGR_TYPE_DYNAMICAL = 0,	/*动态聚合器*/
	AGGR_TYPE_STATIC,		    /*静态聚合器*/
	AGGR_TYPE_MANUAL,		    /*手工聚合器*/
	AGGR_TYPE_MAX_CNT,
}AGGR_TYPE_E;

/*Port Selection Arithmetic*/
typedef enum
{
    SA_TRUNK=0,
    DA_TRUNK,
    SADA_TRUNK,         /*SA/DA Trunking*/
    SAIPDAIP_TRUNK,     /*SAIP/DAIP Trunking*/
    SADAIP_TRUNK,       /*SACMAC/DSTMAC/IP Trunking*/
    TRUNK_TYPE_END
}AGGR_PSA_E ;

/*正确的聚合组ID*/
#define IsValidAggrGrpId(_grp)      ( (((_grp) >= 1) && ((_grp) <= AGGRE_GRP_NUM )) ? TRUE : FALSE )
#define IsValidAggrType(_type)      ((((_type) >= AGGR_TYPE_DYNAMICAL) && ((_type) < AGGR_TYPE_MAX_CNT)) ? TRUE : FALSE )
#define IsValidAggrPSAMode(_mode)   ((((_mode) >= SA_TRUNK) && ((_mode) < TRUNK_TYPE_END)) ? TRUE : FALSE )



/*add by dengjian*/
#define CONFIG_SCHEDULE_ERROR  1
#define CONFIG_SCHEDULE_FINISH 2
#define CONFIG_SCHEDULE_PVLAN  3
/*用于进度条显示的处理完成后是否要跳转到显示页面*/
#define CONFIG_SCHEDULE_RETURN_SUM_PAGE 1 
#define DISP_VLANNUM_PERPAGE     15


#define MIN_VLAN_ID      1
#define MAX_VLAN_ID      4094
#define MIN_VLAN_INDEX   MIN_VLAN_ID
#define MAX_VLAN_INDEX   MAX_VLAN_ID

#define MAX_VLAN            512

/*Begin add by huangmingjian 2013-09-07*/
#define MAC_AGINGTIME_DISABLE 	0
#define MAC_AGINGTIME_MIN 		45
#define MAC_AGINGTIME_DEFAULT 	300
#define MAC_AGINGTIME_MAX 		458
/*End add by huangmingjian 2013-09-07*/

#define NO_OF_PORTS  MAX_PORT_NUM

#define MIN_GROUP_ID    1
#define MAX_GROUP_ID    NO_OF_PORTS
#define MIN_GROUP_INDEX MIN_GROUP_ID
#define MAX_GROUP_INDEX MAX_GROUP_ID

#define ISOLATE_VLAN_MAX_NUM     2

#define ENABLE 1
#define DISABLE 0

/* 检查参数有效范围 */
#define   VALID_PORT( x )                           ( ( ( x >= 1) && ( x <= LOGIC_PORT_NO) ) ?1 : 0 )
#define   VALID_PORT_PORPERTY( x )         ( ( ( x >= 0) && ( x < PORT_PORPERTY_END) ) ?1 : 0 )
#define   VALID_PORT_PRIO( x )                 ( ( ( (x) >= 0 ) &&  ( (x) <= 7 ) ) ? 1 : 0 )
#define   VALID_VLAN_ID( x )                     ( ( ( (x) >= MIN_VLAN_ID ) && ( (x) <= MAX_VLAN_ID ) ) ? 1 : 0 )
#define   VALID_VLAN_INDEX( x )               ( ( ( (x) >= MIN_VLAN_INDEX ) &&  ( (x) <= MAX_VLAN_INDEX ) ) ? 1 : 0 )
#define   VALID_GROUP_ID( x )                     ( ( ( (x) >= MIN_GROUP_ID ) && ( (x) <= MAX_GROUP_ID ) ) ? 1 : 0 )
#define   VALID_GROUP_INDEX( x )               ( ( ( (x) >= MIN_GROUP_INDEX ) &&  ( (x) <= MAX_GROUP_INDEX ) ) ? 1 : 0 )
#define   VALID_ISOLATE_INDEX( x )         ( ( ( (x) >= 1 ) && ( (x) <= ISOLATE_VLAN_MAX_NUM) ) ? 1 : 0 )
#define   VALID_ENABLE_VALUE( x )           ( ( ( x == ENABLE ) || ( x == DISABLE ) ) ?1 : 0 )
#define   IS_MULTICAST(x)                    (x[0]&0x1)
#define   INVALID_VLAN_IDX                      0xffff

DRV_RET_E Drv_InitPortAggre(void);
DRV_RET_E Drv_SetPortAggre(logic_pmask_t* Lmask_ptr);
DRV_RET_E Drv_DelPortAggre(port_num_t main_lport);
DRV_RET_E Drv_DelAllAgg (void);
DRV_RET_E Drv_CheckPortCanAggre(logic_pmask_t* Lmask_ptr);
DRV_RET_E Drv_SetAggreMode(AGGR_PSA_E mode);
DRV_RET_E Drv_CheckAggreType(UCHAR ucAggType, agg_grp_num_t ulAggID);
DRV_RET_E Drv_GetAggreMaster( agg_grp_num_t ucGrpNo ,port_num_t *mPort_ptr);
DRV_RET_E Drv_GetGroupNum(port_num_t lPort, agg_grp_num_t *ucGrpNo );
DRV_RET_E Drv_GetAggreMasterByPort( port_num_t lport ,port_num_t *mPort_ptr);
DRV_RET_E Drv_GetAggreGrpMask(port_num_t lport, logic_pmask_t *lmask_ptr);
BOOL Drv_IsAggrePort(port_num_t lport);
DRV_RET_E Drv_GetAggreMaskAll(logic_pmask_t * lmask_ptr);
VOID Drv_NotifyAggCfgChange(ULONG ulAggIndex);
DRV_RET_E Drv_GetAggreMasterPortAll(logic_pmask_t * lmask_ptr);
UINT8 Drv_GetMasterForSoft(port_num_t lPort);
VOID Drv_SyncAggMaskBitSet(UINT32 ulAggID);
VOID Drv_SyncMaskAggCfg(VOID);
DRV_RET_E Drv_Lacp_Transmit(UINT32 ulPort_Index, void *pPacket, UINT32 ulLength);
/*gpio*/
typedef enum tagGPIO_PIN
{
    GPIO_PIN_SFP_SCL=0,
    GPIO_PIN_SFP0_SDA,
    GPIO_PIN_SFP1_SDA,
    GPIO_PIN_WATCHDOG,
    GPIO_PIN_END
}GPIO_PIN_E ;

/* define GPIO control function */
typedef enum tagGPIO_CONTROL
{
	GPIO_PIN_CTRLFUNC_NORMAL = 0,
	GPIO_PIN_CTRLFUNC_DEDICATE_PERIPHERAL,
	GPIO_PIN_CTRLFUNC_END
} GPIO_CONTROL_E;

/* define GPIO direction */
typedef enum tagGPIO_DIRECTION
{
	GPIO_PIN_DIR_IN = 0,
	GPIO_PIN_DIR_OUT,
	GPIO_PIN_DIR_END
} GPIO_DIRECTION_E;

/* define GPIO data */
typedef enum tagGPIO_DATA
{
	GPIO_PIN_DATA_LOW = 0,
	GPIO_PIN_DATA_HIGH,
	GPIO_PIN_DATA_END
} GPIO_DATA_E;
/***********************************************************************/
/* drv_vlan相关定义 */
/************************************************************************/
typedef struct
{
    logic_pmask_t portmask;
}vlan_tab_t;

typedef  enum
{
    UNTAGGED,
    TAGGED
}PortTag_E;

typedef enum {
    QVLAN_EG_UNTAGGED = 0,  /**<  egress transmit packets without vlan tag */
    QVLAN_EG_TAGGED,        /**<  egress transmit packets with vlan tag     */
    QVLAN_EG_TRANSPARENT ,  /**<  egress transmit packets in hybrid tag mode     */
    QVLAN_EG_END
} PortEGMode_t;

#define LINK_TYPE_ACCESS 0
#define LINK_TYPE_TRUNK  1
#define LINK_TYPE_HYBRID 2
#define LINK_TYPE_END 4

#define DEFAULT_VID 1
#define	VALID_VID(x)	((x > 0) && (x < 4094))


typedef enum{
	VLAN_MODE_8021Q = 0,
	VLAN_MODE_TRANSPARENT
}
vlan_mode_t;


#define	DEFAULT_SWITCH_PORT_MODE	LINK_TYPE_ACCESS
#define	DEFAULT_SWITCH_VLAN_MODE    VLAN_MODE_8021Q


typedef enum{
	VLAN_FLAG_NONE = 0,
	VLAN_FLAG_SET = 0x01,
	VLAN_FLAG_DEL = 0x02,
	VLAN_FLAG_NEW = 0x03,
	VLAN_FLAG_TAG = 0x80
/*	VLAN_FLAG_UNTAG = 0x40	*/
}
vlan_flag_t;
#define vlan_flag(x) ((x) & 0x0f)
#define vlan_tagged(x)  ((x) & VLAN_FLAG_TAG)
#define default_vlan(v)  ((v) == DEFAULT_VID)
#ifndef __packed 
#define __packed __attribute__((__packed__))
#endif 

typedef struct __packed{
	UINT16 start;
	UINT16 end;
}
vlan_id_range_t;


typedef struct __packed{
	UINT16 value;
	UINT16 flag;
}
vlan_id_back_t;

typedef struct __packed{
	logic_pmask_t member;
	union {
		vlan_id_back_t id;
		vlan_id_range_t id_range;
	};
}
switch_vlan_t;

typedef struct __packed{
	logic_pmask_t member;
	UINT8 flag;// private use, for vlan operation
	UINT16 count;
	switch_vlan_t vlans[];
} 
switch_vlan_group_t;

#define MAX_VLAN_GROUPS  ((MAX_PORT_NUM) * (MAX_PORT_NUM))
#define IFNAMSIZE 16
typedef struct __packed{
	char ifname[IFNAMSIZE];
	UINT8 phy;
	UINT16 pvid;	
	UINT8 mode;
	UINT8 count; // for access mode count = 0, trunk = 1, hybrid = 2
	switch_vlan_group_t	trunk_hybrid[];	
}
switch_vlan_interface_t;



typedef struct __packed {
	UINT8 mode;// transparent or 802.1Q
	UINT16 mvlan;// Read only
	struct {
		char ifname[IFNAMSIZE];
		UINT8 type;
	}ports[MAX_PORT_NUM+1];
}
switch_vlan_mode_t;

/*Begin add by huangmingjian 2013-09-08*/
typedef struct __packed {
	UINT32 aging_time;
}mac_setting_t;
/*End add by huangmingjian 2013-09-08*/

/*Begin add by linguobin 2013-12-06*/
typedef struct __packed {
	UINT8 enable;
}compatible_setting_t;
/*End add by linguobin 2013-12-06*/


#define QVLAN 1
#define PVLAN 2
#define TRSPVLAN 3

#define   VLAN_BUF_LEN   512
#define   VID_VOID 0xFFFF

#define UP 1
#define DOWN 0

typedef UINT8 vid_list_t[VLAN_BUF_LEN];
typedef UINT8 pvid_list_t[MAX_GROUP_ID];


#define BYTE_SET_BIT(_bit, _byte) do{                             \
    (_byte) |= (UINT8)((UINT8)1 << (UINT8)(_bit));            \
}while(0)

#define BYTE_CLR_BIT(_bit, _byte) do{                             \
    (_byte) &= (UINT8)(~((UINT8)((UINT8)1 << (UINT8)(_bit))));         \
}while(0)

#define TEST_BIT_8(_bit, _byte) (((UINT8)(_byte) & ((UINT8)1 << (UINT8)(_bit))) != 0)

#define VID_LIST_SETBIT(_vid, _pucBuf)  do{                                  \
    BYTE_SET_BIT((UINT32)(_vid) % 8, ((UINT8 *)(_pucBuf))[(_vid) / 8]);     \
}while(0)

#define VID_LIST_CLRBIT(_vid, _pucBuf)  do{                                  \
    BYTE_CLR_BIT((UINT32)(_vid) % 8, ((UINT8 *)(_pucBuf))[(_vid) / 8]);     \
}while(0)

#define TEST_VID_LIST(_vid, _pucBuf) (TEST_BIT_8((_vid) % 8, ((UINT8 *)(_pucBuf))[(_vid) / 8]))

#define ForEachListVlan(_vid, _vidListBuf)                     \
    for((_vid=1); (_vid)<=4094; (_vid)++)                       \
        if(TEST_VID_LIST((_vid), (_vidListBuf)))

#define ForEachListPortVlan(_pvid, _pvidListBuf)                     \
    for((_pvid=1); (_pvid)<=MAX_GROUP_ID; (_pvid)++)                       \
        if(TEST_VID_LIST((_pvid), (_pvidListBuf)))

void Drv_BuildExistVlanMask(UINT8 *vid_list_buf);
void Drv_MakeVidListMember (port_num_t ulPortNum, UINT8 * vid_list_buf);
void Drv_MakeTaggedVidListMember(port_num_t ulPortNum, UINT8 * vid_list_buf);
void Drv_MakeUnTaggedVidListMember(port_num_t ulPortNum, UINT8 * vid_list_buf);
UINT8 Drv_TestVlanExist(vlan_id_t tdVid, UINT8 * vid_list_buf);
UINT32 Drv_GetExistVlanNum(void);
vlan_id_t Drv_GetEndVid (vlan_id_t vid, UINT8 * vid_list_buf);
UINT8 Drv_TestPvlanExist(UINT8 tdVid, UINT8 * vid_list_buf);
UINT8 Drv_GetEndPVid(UINT8 vid, UINT8 * vid_list_buf);
DRV_RET_E Drv_Vlan_Enable(UINT32 ulVlanType);
DRV_RET_E Drv_Vlan_Create(vlan_id_t tdVid);
DRV_RET_E Drv_Vlan_Delete(vlan_id_t tdVid);
DRV_RET_E Drv_Vlan_DeleteByRange(vlan_id_t tdBeginVID, vlan_id_t tdEndVID);
DRV_RET_E Drv_Vlan_DelMember(logic_pmask_t *pstPortMask, vlan_id_t tdVid);
DRV_RET_E Drv_Vlan_AddMember(logic_pmask_t *pstPortMask, vlan_id_t tdVid);
DRV_RET_E Drv_GetVlanMember( vlan_id_t tdVid, logic_pmask_t * pstPortMask);
BOOL Drv_PortInVlan(port_num_t ulPort, vlan_id_t tdVid);
DRV_RET_E Drv_Port_SetVid (logic_pmask_t * pstPortMask, vlan_id_t tdVid);
DRV_RET_E API_SetPortPVID(port_num_t ulPort, vlan_id_t tdPVID);
DRV_RET_E Drv_Port_SetAccessVlan( port_num_t ulPortNum, vlan_id_t tdVid);
void Drv_Port_SetAccessVlanForAgg(logic_pmask_t *pstPortMask);
DRV_RET_E Drv_Port_SetTrunkVlan( port_num_t ulPortNum );
DRV_RET_E Drv_Port_DelTrunkVlan(port_num_t ulPortNum);
DRV_RET_E Drv_Port_SetHybridVlan( port_num_t ulPortNum);
DRV_RET_E Drv_Port_DelHybridVlan(port_num_t ulPortNum);
DRV_RET_E Drv_Check_VoiceVlan_mode(port_num_t ulPortNum, UINT8 * web_tagvid_list);
DRV_RET_E  API_AccessVlan_Set(vlan_id_t tdVid, logic_pmask_t * pstPortMask);
DRV_RET_E  API_HybridVlan_Set(port_num_t ulPortNum, vlan_id_t tdPVID, UINT8 * web_tagvid_list, UINT8 * web_untagvid_list);
DRV_RET_E  API_HybridVlan_Set_VoiceVlan(port_num_t ulPortNum, vlan_id_t tdPVID, UINT8 * web_tagvid_list, UINT8 * web_untagvid_list);
DRV_RET_E  API_TrunkVlan_Set(port_num_t ulPortNum, vlan_id_t tdPVID, UINT8 ucTrunkAll, UINT8 * web_tagvid_list);
DRV_RET_E  API_TrunkVlan_Set_VoiceVlan(port_num_t ulPortNum, vlan_id_t tdPVID, UINT8 ucTrunkAll, UINT8 * web_tagvid_list);
DRV_RET_E  API_Delete_Hybrid(port_num_t ulPortNum);
DRV_RET_E  API_Delete_Trunk(port_num_t ulPortNum);
DRV_RET_E  API_DelPortFromVlan(port_num_t ulPortNum, UINT8 * vid_list_buf);
DRV_RET_E  API_DelPortFromVlan_VoiceVlan(port_num_t ulPortNum, UINT8 * vid_list_buf);
void Drv_Vlan_Init(void);
DRV_RET_E Drv_Port_SetMRCTag( port_num_t stMrcPort, USHORT usClstVid);
DRV_RET_E Drv_Port_SetMRCUnTag( port_num_t stMrcPort, USHORT usClstVid);
DRV_RET_E Drv_Port_SetTaggedByPort( UINT8 ucValue, port_num_t  lport, vlan_id_t tdVid);
DRV_RET_E Drv_Vlan_AddMemberByPort(port_num_t lport, vlan_id_t tdVid);
DRV_RET_E Drv_Vlan_DefaultDesc(vlan_id_t tdVid);
DRV_RET_E Drv_GetGroupMember( UINT8 ucVid, logic_pmask_t *pstPortMsk);
DRV_RET_E Drv_PVlanDefaultSet(void);
DRV_RET_E Drv_PVlan_Delete(UINT8 ucVid);
DRV_RET_E Drv_PVlan_Create(UINT8 ucVid);
UINT8  Drv_Port_IsInGroup(UINT8 ucVid, port_num_t ulPortNo);
DRV_RET_E Drv_PVlan_Set(UINT8 ucVid, logic_pmask_t *pstPVlanMsk);
DRV_RET_E Drv_PVlan_AddPort(UINT8 ucVid, port_num_t ulPortNo);
DRV_RET_E Drv_PVlan_DelPort(UINT8 ucVid, port_num_t ulPortNo);
DRV_RET_E Drv_PVlan_UpdataConfig(void);
DRV_RET_E Drv_Port_SetTagged( UINT8 ucValue, logic_pmask_t * pstPortMask, vlan_id_t tdVid);
DRV_RET_E Drv_GetVlanStatus(vlan_id_t uiVid, UINT32 * puiStatus);

/***********************************************************************/
/* drv_mac相关定义 */
/************************************************************************/
#define MAC_UCAST               0x00
#define MAC_MCAST               0x01
#define MAC_L2PRTCL             0x02
#define MAC_BROAD               0x03
#define MAC_ZERO                0x04
#define MAC_MULTICAST           0x05
#define MAC_SELF                0x06
#define MAC_NORMAL              0x07

/*新增mac的方式*/
#define MACADD_TYPE_TAB         0x01    /*添加到cfg配置table*/
#define MACADD_TYPE_ARL         0x02    /*添加到芯片arl table*/

typedef struct
{
    vlan_id_t vid;
    mac_address_t mac_addr;
    logic_pmask_t port_mask; /* BLACKHOLE_PATTERN indicate blackhole */
}mac_tab_t;

#define DRV_MAC_DUMP_FLAG_VALID 0x01
#define DRV_MAC_DUMP_FLAG_AGGR 0x02
#define DRV_MAC_DUMP_FLAG_STATIC 0x04

typedef struct
{
    vlan_id_t vid;
    UINT8 flag;                     /* DRV_MAC_DUMP_FLAG_XX */
    mac_address_t mac_addr;
    port_num_t lport;           /* zero means BLACKHOLE */
    agg_grp_num_t aggrGroup;
}mac_dump_ucast_tab_t;

typedef struct _mac_dump_link_s
{
    mac_dump_ucast_tab_t *pstMacDump;
    struct _mac_dump_link_s *pstMacDumpLinkPre;
    struct _mac_dump_link_s *pstMacDumpLinkNext;
}mac_dump_ucast_link_t;

typedef struct
{
    mac_address_t mac_addr;
    vlan_id_t vid;
    logic_pmask_t port_mask; /*  全零 indicate blackhole */
    struct
    {
            UINT8 Valid ;
            UINT8 Static ;
            UINT8 MacType ;    // 0: 单播  1: 普通多播  2: 二层协议多播  3: 广播  4: 全 0 Mac
            UINT8 BlackHole ;
    }MacFlag;
    UINT16 mc_index;
}Mac_op_t;

typedef struct
{
    mac_address_t mac_addr;
    vlan_id_t tdVid;
    port_num_t lgcPort;         /*zero means blackhole*/
    UINT8 ucStatic;
    UINT8 ucIsAggr;             /*zero means not a aggr group MAC*/
    agg_grp_num_t aggr_group;
}mac_ucast_t;

typedef struct
{
    mac_address_t mac_addr;
    vlan_id_t tdVid;               /*zero means in valid*/
    logic_pmask_t port_mask;    /*NULL means blackhole*/
}mac_mcast_t;

typedef struct
{
    mac_address_t mac;
    vlan_id_t vid;
    port_num_t port;
    UINT8 ucMacType; /*0--UNICAST 1--MULTICAST*/
    UINT8 ucStatic;
    UINT8 ucIsAggr;
    agg_grp_num_t aggr_group;
    logic_pmask_t port_mask;
    UINT32 ulL2Idx; //idx of arl table
}mac_common_t;

typedef struct mac_delete_s
{
    UINT8    flushByVid;            /* ENABLE~DISABLE, not support flushStaticAddr*/
    UINT8    flushByPort;           /* ENABLE~DISABLE*/
    UINT8    flushByTunk;           /* ENABLE~DISABLE*/
    UINT8    flushByMac;            /* ENABLE~DISABLE*/
    UINT8    flushAll;              /* ENABLE~DISABLE*/

    vlan_id_t vid;                   /* used if set flushByVid ENABLE*/
    port_num_t port;                /* used if set flushByPort ENABLE*/
    agg_grp_num_t trunkGroup;        /* used if set flushByTunk ENABLE*/
    mac_address_t macAddr;          /* used if set flushByMac ENABLE, multicast or unicast, must give vid*/
    UINT8    flushStaticAddr;       /* set to flush static MAC address too, this flag is not valid for flushByMac*/
} mac_delete_t;

/* Type of management packet */
typedef enum mac_trap_mgmtType_e
{
    FRAME_TRAP_TYPE_RIP = 0,
    FRAME_TRAP_TYPE_ICMP,
    FRAME_TRAP_TYPE_ICMPV6,
    FRAME_TRAP_TYPE_ARP,
    FRAME_TRAP_TYPE_MLD,
    FRAME_TRAP_TYPE_IGMP,
    FRAME_TRAP_TYPE_BGP,
    FRAME_TRAP_TYPE_OSPFV2,
    FRAME_TRAP_TYPE_OSPFV3,
    FRAME_TRAP_TYPE_SNMP,
    FRAME_TRAP_TYPE_SSH,
    FRAME_TRAP_TYPE_FTP,
    FRAME_TRAP_TYPE_TFTP,
    FRAME_TRAP_TYPE_TELNET,
    FRAME_TRAP_TYPE_HTTP,
    FRAME_TRAP_TYPE_HTTPS, 
    /*RTK8328L只支持以下几种类型*/
    FRAME_TRAP_TYPE_DHCPV6,
    FRAME_TRAP_TYPE_DHCP,
    FRAME_TRAP_TYPE_DOT1X,
    FRAME_TRAP_TYPE_BPDU,
    FRAME_TRAP_TYPE_END
} mac_trap_mgmtType_t;

typedef enum mac_trap_action_e
{
    FRAME_TRAP_ACTION_FORWARD = 0,
    FRAME_TRAP_ACTION_DROP,
    FRAME_TRAP_ACTION_TRAP2CPU,
    FRAME_TRAP_ACTION_COPY2CPU,
    FRAME_TRAP_ACTION_TO_GUESTVLAN,
    FRAME_TRAP_ACTION_FLOOD_IN_VLAN,
    FRAME_TRAP_ACTION_FLOOD_IN_ALL_PORT,
    FRAME_TRAP_ACTION_FLOOD_IN_ROUTER_PORTS,
    FRAME_TRAP_ACTION_END
} mac_trap_action_t;

typedef enum rsv_mac_action_e
{
    PASS_TYPE_CPU_ONLY = 0,
    PASS_TYPE_ALLPORTS,
    PASS_TYPE_CPU_EXCLUDE,
    PASS_TYPE_DROP,
    PASS_TYPE_END
} rsv_mac_pass_action_t;


typedef enum mac_trap_priority_e
{
    FRAME_TRAP_PRIORITY_0 = 0,
    FRAME_TRAP_PRIORITY_1,
    FRAME_TRAP_PRIORITY_2,
    FRAME_TRAP_PRIORITY_3,
    FRAME_TRAP_PRIORITY_4,
    FRAME_TRAP_PRIORITY_5,
    FRAME_TRAP_PRIORITY_6,
    FRAME_TRAP_PRIORITY_7
} mac_trap_priority_t;

#define FRAME_TRAP_PORT_ALL 0

typedef struct mac_trap_set_s
{
    mac_trap_mgmtType_t frameType;
    mac_trap_action_t trapAction;
}mac_trap_set_t;


typedef enum mac_learn_enable_e
{
    MAC_LEARN_FWD_DISABLED = 0,
    MAC_LEARN_FWD_ENABLED,
    MAC_LEARN_END
} mac_learn_enable_t;
typedef enum {
    DRV_PACKET_PRI_LOWEST = 0,
    DRV_PACKET_PRI_ARP_CPU,
    DRV_PACKET_PRI_L2_CPU,
    DRV_PACKET_PRI_IP_CPU,
    DRV_PACKET_PRI_DHCP_CPU,
    DRV_PACKET_PRI_DOT1X_CPU,
    DRV_PACKET_PRI_BPDU_CPU,
    DRV_PACKET_PRI_HIGHEST
}trapCpuPri_E;
    
UINT8 l2_mac_cmp(UINT8 * mac1, UINT8 * mac2);
UINT8 l2_MacType(mac_address_t  mac );
BOOL IsMacAddrEqual(mac_address_t mac1, mac_address_t mac2);
void Drv_bpduPktInit(void);
DRV_RET_E Drv_MacInit(void);
DRV_RET_E Drv_AddSwitchMacAddr(UINT8 *ucMacAddr, vlan_id_t vid);
DRV_RET_E Drv_DelSwitchMacAddr(UINT8 *ucMacAddr, vlan_id_t tdMVid);
DRV_RET_E Drv_ForgetMacAll(void);
DRV_RET_E Drv_ForgetMacByVlan(vlan_id_t tdVid);
DRV_RET_E Drv_ForgetMacByAggr(agg_grp_num_t aggrGroup);
DRV_RET_E Drv_ForgetMacByPort(port_num_t lport);
DRV_RET_E Drv_ForgetTrunkSpecialMac(port_num_t lport);
DRV_RET_E Drv_DelTrunkPortSpecialMac(logic_pmask_t *pstTrunkPortMask);
DRV_RET_E Drv_ForgetMacByVlanPort(port_num_t lport, vlan_id_t tdVid);
DRV_RET_E Drv_SetAgeTime(UINT32 ulAgeTime);
DRV_RET_E Drv_DelMacAll(void);
UINT16 SSP_FindMacTab(mac_tab_t  *pstMacEntry);
DRV_RET_E Drv_DelMacAddr(Mac_op_t  *pstMacEntry);
DRV_RET_E SSP_DelAddressByMac(Mac_op_t *pstMacEntry);
DRV_RET_E SSP_AddMacTab(Mac_op_t  *pstMacEntry);
DRV_RET_E SSP_DelMacTab(Mac_op_t  *pstMacEntry);
DRV_RET_E SSP_FindConflictMacAddr (Mac_op_t  *pstMacEntry, Mac_op_t  *pstConfilictMacEntry);
DRV_RET_E SSP_AddMacAddr (Mac_op_t  *pstMacEntry);
DRV_RET_E Drv_DelMacByPort(logic_pmask_t  *pstPortMask);
DRV_RET_E Drv_DelMacByVid(vlan_id_t tdVid);
DRV_RET_E Drv_DelMacByPortVid(logic_pmask_t *pLPortMask, vlan_id_t tdVid);
DRV_RET_E Drv_DelMulticastAddr(void);
BOOL Drv_IsExistIpMultiMac(void);
DRV_RET_E SSP_DelMacTabForPort(port_num_t  lport);
DRV_RET_E SSP_DelMacTabForVlan(vlan_id_t vid);
DRV_RET_E SSP_DelMacTabForVlanPort (vlan_id_t vid, port_num_t lport);
DRV_RET_E Drv_FindMacAddrUnicast(vlan_id_t tdVlanId, mac_address_t macAdd, port_num_t *port_no);
DRV_RET_E Drv_FindMacAddr(vlan_id_t tdVlanId, mac_address_t  stMacAddress,   logic_pmask_t *pstPortMask);
DRV_RET_E Drv_EnablePortBinding(logic_pmask_t stLPortMask, UINT8 ucValue);
DRV_RET_E Drv_PortMacBinding(port_num_t lport, mac_address_t stMacAddr,vlan_id_t tdVid);
BOOL SSP_MacTabUMacExist (port_num_t lport);
DRV_RET_E SSP_GetMacTab(mac_address_t stMacAddr, const vlan_id_t tdVid, mac_tab_t  *pstMacTab);
DRV_RET_E Drv_MacUndoBinding (port_num_t lport,mac_address_t stMacAddr,vlan_id_t tdVid);
DRV_RET_E Drv_GetAgeTime(UINT32 *pulAgeTime);
void Drv_DelDynamicPortMac(logic_pmask_t stLPortMask);
void Drv_DelDynamicVlanPortMac(logic_pmask_t stLPortMask, vlan_id_t tdVid);
BOOL l2_MacIsBlackhole(mac_tab_t stMacTab);
DRV_RET_E SSP_GetMmCounter(UINT32 *pulMcastCount);
void Drv_DelMacExceptOneVlanPort(vlan_id_t tdVid, logic_pmask_t  stPortMask);
DRV_RET_E Drv_DelayAgingPort(logic_pmask_t *stLPortMask, int flag);
DRV_RET_E Drv_PortMacLimit(port_num_t lport, UINT32 lmt_cnt, UINT32 ulDisForward);
BOOL Drv_PortMacLearnIsFull(port_num_t lport);
DRV_RET_E SSP_SetMaxMacLimit (logic_pmask_t *pLgcPMask, UINT32 ulMaxCount, UINT32 ulDisForward);
DRV_RET_E Drv_SystemMacLimit(UINT32 lmt_cnt);
DRV_RET_E Drv_MacUcastRefreshCall(UINT32 uiInterval);
DRV_RET_E Drv_GetMacUcastDump(mac_dump_ucast_tab_t *pstMacUcastDump, UINT32 *puiCount);
DRV_RET_E Drv_GetMacMcastDump(mac_mcast_t *pstMacMcastDump, UINT32 *puiCount);
DRV_RET_E Drv_GetMacUcastDumpSorted(mac_dump_ucast_tab_t *pstMacUcastDump, UINT32 *puiCount);
DRV_RET_E Drv_TrapSpecifyFrame(port_num_t lport, mac_trap_set_t stTrapSet, trapCpuPri_E priority);
DRV_RET_E Drv_SetReservedMacTrap(UINT32 uiIndex, UINT32 priority, rsv_mac_pass_action_t action);
DRV_RET_E Drv_GetArlTable(UINT32 ulIdx, mac_common_t * pstMacUcastDump);
DRV_RET_E Drv_GetMacL2Idx(mac_address_t stMac, vlan_id_t tdVid, UINT32 * pulIndex);
UINT32 Drv_CalcArlIndex(mac_address_t stMac, vlan_id_t tdVid);
DRV_RET_E Drv_GetMacCount(UINT32 *pulTotalCnt);
DRV_RET_E Drv_AddTabMacAddr(Mac_op_t  *pstMacEntry, UINT32 ulAddType);     
DRV_RET_E Drv_GetMoreMac( UINT32 ulIdx, UINT32 uiGetCount, 
    mac_common_t *pstMacDump, UINT32 *puiCount, UINT32 *puiNextIdx);
DRV_RET_E Drv_setUnkonwMcastForward(UINT32 ulEnable);
VOID Drv_XMcastFwdInit(VOID);

VOID Drv_PortIsolate_Init(VOID);
BOOL Drv_IsPortIsolateEn(port_num_t lport);
DRV_RET_E Drv_PortIsolate_AddMember(logic_pmask_t *pstPortMask);
DRV_RET_E Drv_PortIsolate_DelMember(logic_pmask_t *pstPortMask);
VOID Drv_PortLed_Init(VOID);
DRV_RET_E Drv_SetPortLed(port_num_t lport, UINT32 ulLinkState, UINT32 ulPortType);
DRV_RET_E Drv_SendPortOptionMsg(PORT_INPUT_PARA_S *pstPara);




/***********************************************************************/
/* drv_mirror相关定义 */
/************************************************************************/
/*
*MIRROR MODE
*/
#define IN_MIRROR     1
#define OUT_MIRROR    2
#define BOTH_MIRROR   3

DRV_RET_E Drv_InitMirror(void);
DRV_RET_E Drv_CreateMirrorGroup(MIRROR_GROUP_S *pstMirrGroup);
DRV_RET_E Drv_SetMirrorGroupMonitor(MIRROR_GROUP_S *pstMirrGroup);
DRV_RET_E Drv_SetMirrorGroupMirror(MIRROR_GROUP_S *pstMirrGroup);
DRV_RET_E Drv_DelMirrorGroup(MIRROR_GROUP_S *pstMirrGroup);
DRV_RET_E Drv_GetMonitorPortAll(logic_pmask_t *port_mask);

/***********************************************************************/
/* 光模块 相关定义*/
/************************************************************************/
/*光模块定义*/
#define I2C_START                   0x00
#define I2C_TRANSCEIVER_CODES_ADDR  0x03
#define I2C_TRANSCEIVER_CODES_LEN   4
#define I2C_RB_NOMINAL_ADDR         0x0c  /*12*/
#define I2C_RB_NOMINAL_LEN          1
#define I2C_CONNECTOR_ADDR         0x02  /*2*/
#define I2C_TRANSFER_DISTANCE_ADDR 0x0e  /*14*/
#define I2C_TRANSFER_DISTANCE_LEN  6
#define I2C_TWISTED_PAIR_ADDR      0x12  /*18*/
#define I2C_WAVELENGTH_ADDR        0x3c  /*60*/
#define I2C_CENTER_WAVELENGTH_ADDR      0x75  /*117*/
#define I2C_DIAGNOSTIC_MONITORING_ADDR  0x5c  /*92*/
#define I2C_VENDOR_NAME_ADDR       0x14  /*20*/
#define I2C_VENDOR_NAME_LEN        16
#define I2C_PRODUCTOR_IDIOGRAPH_ADDR    0x60  /*96*/
#define I2C_PRODUCTOR_IDIOGRAPH_LEN     11
#define I2C_MAX_TX_POWER_ADDR      0x77  /*119*/
#define I2C_MIN_TX_POWER_ADDR      0x79  /*121*/
#define I2C_MIN_RX_POWER_ADDR      0x7b  /*123*/
#define I2C_MAX_RX_POWER_ADDR      0x7d  /*125*/
#define I2C_PART_NUMBER_ADDR       0x28  /*40*/
#define I2C_PART_NUMBER_LEN        16
#define I2C_REV_NUMBER_ADDR        0x38  /*56*/
#define I2C_REV_NUMBER_LEN         4
#define I2C_SERIAL_NUMBER_ADDR     0x44  /*68*/
#define I2C_SERIAL_NUMBER_LEN      16
#define I2C_PRODUCT_DATA_ADDR      0x54  /*84*/
#define I2C_PRODUCT_DATA_LEN       8

#define SFP_EEPROM1_DEVICE_NO   0

#define I2C_READ_SFP_FROM_DRAM   1  /*从内存中读取信息*/
#define I2C_READ_SFP_FROM_EEPROM 2  /*从eeprom中读取信息*/

/*SFP/GBIC/SFP+模块Transceiver Codes寄存器位和模块物理型号的映射关系*/
/*byet3 ~ 6*/
#define I2C_RANSCEIVER_CODES_MASK  0x700f7fff

#define SFP_STACK_PLUS      0x00000000    /*SFP+ 堆叠模块*/
#define SFP_STACK           0x000000FF  /*SFP 堆叠模块*/
#define SFP_1000_BASE_SX    0x00000001
#define SFP_1000_BASE_LX    0x00000002
#define SFP_1000_BASE_T_AN  0x00000008
#define SFP_1000_BASE_LX_AN 0x00000012   /*100/1000BASE-LX 自适应模块*/
#define SFP_100X_BASE_BX_Y  0x00000040  /*其中x和y是需要判断和替换的*/
#define SFP_1000_BASE_PX    0x00000080
#define SFP_100_BASE_SX1     0x00000100  /*某些100BASE-SX模块*/
#define SFP_100_BASE_SX2     0x00000020  /*某些100BASE-SX模块*/
#define SFP_100_BASE_SX3     0x00000120  /*某些100BASE-SX模块*/
#define SFP_100_BASE_LX1     0x00000200  /*某些100BASE-LX模块*/
#define SFP_100_BASE_LX2     0x00000010  /*某些100BASE-LX模块*/
#define SFP_100_BASE_LX3     0x00000210  /*某些100BASE-LX模块*/    
#define SFP_100_BASE_BX_Y   0x00000240  /*其中y是需要判断和替换的, 100BASE-BX模块*/
#define SFP_100_BASE_ZX     0x00000400
#define SFP_10G_BASE_SR     0x00100000
#define SFP_10G_BASE_LR     0x00200000
#define SFP_10G_BASE_LRM    0x00400000

#define SFP_100_BASE_BX_D      0
#define SFP_100_BASE_BX_U      1
#define SFP_1000_BASE_BX_D     2
#define SFP_1000_BASE_BX_U     3

#define SFP_100_BASE_LH   0
#define SFP_1000_BASE_LH  1
#define MAX_TRANSCEIVER_CODE_LEN  128  /*128*/
#define MAX_TRANSCEIVER_TYPE_LEN  24
#define MAX_SFP_VENDOR_LEN  20
#define MAX_CONNECTOR_TYPE_LEN  8
#define MAX_TRANSFER_DISTANCE_LEN  24
#define MAX_POWER_LEN  2
#define MAX_POWER_STR_LEN  8

#define MAX_WAVE_LEN 2000
#define MIN_WAVE_LEN 500

#define UNKNOWN  "UNKNOWN"
#define UNKNOWN_SFP  "UNKNOWN_SFP"
#define UNKNOWN_GBIC  "UNKNOWN_GBIC"

/*光模块类型*/
#define TRANSCEIVER_TYPE_SFP    0
#define TRANSCEIVER_TYPE_GBIC   1
#define TRANSCEIVER_TYPE_POS    2
#define TRANSCEIVER_TYPE_TP     3
#define TRANSCEIVER_TYPE_UNKNOWN    4
/*光模块硬件类型*/
#define TRANSCEIVER_HWTYPE_SM 0
#define TRANSCEIVER_HWTYPE_MM 1
/*光模块支持的光纤直径类型,MIB可使用*/
#define TRANSCEIVER_FIBER_9     1
#define TRANSCEIVER_FIBER_50    2
#define TRANSCEIVER_FIBER_625   3
#define TRANSCEIVER_COPPER      4
#define TRANSCEIVER_UNKOWN      65535
#define SFP_WARELENGTH_SHORT  1260
#define SFP_WARELENGTH_LONG   1360

#define KILOMETER  1000
#define TEN_METER  10

#define SFP_LH_SHORT 40
#define SFP_GE_LH_LONG 70
#define SFP_FE_LH_LONG 80

/*显示主要还是显示全部特征参数*/
#define DISPLAY_NORMAL 0
#define DISPLAY_SECRET 1

#define SFP_BIT_0  0x01
#define SFP_BIT_2  0x04
#define SFP_BIT_3  0x08
#define SFP_BIT_4  0x10
#define SFP_BIT_6  0x40
#define SFP_BIT_7  0x80

#define FIBERPORTCHECK 0
#define ISETHERNET 1

typedef struct tagTRANSCEIVER_TYPE
{
    CHAR        *c_name;
    INT32	    c_val;
} TRANSCEIVER_TYPE_S;

typedef struct tagSFP_TRANSFER_DISTANCE
{
    BOOL bIsSM;
    UINT16 uc9um_km;
    UINT16 uc9um;
    UINT16 uc50um;
    UINT16 uc625um;
    UINT16 ucCopper;
    UINT16 ucOM3;
}SFP_TRANSFER_DISTANCE_S;
typedef struct tagSFP_PRODUCT_DATE
{
    UINT8 ucYear;
    UINT8 ucMonth;
    UINT8 ucDay;
} SFP_PRODUCT_DATE_S;


typedef struct tagSFP_TXRX_POWER
{
    UINT8 aucMaxTxPower[MAX_POWER_LEN];
    UINT8 aucMinTxPower[MAX_POWER_LEN];
    UINT8 aucMaxRxPower[MAX_POWER_LEN];
    UINT8 aucMinRxPower[MAX_POWER_LEN];
} SFP_TXRX_POWER_S;

typedef struct tagSFP_TRANSCEIVER
{
    BOOL bIsTwistPair;    /*是否电口*/
    BOOL bSupDiagnostic;  /*是否支持光口诊断*/
    UINT32 ulModuleType;    /*模块类型*/
    UINT32 ulTrscvCode;     /*物理型号 值*/
    UINT8 aucTrscvType[MAX_TRANSCEIVER_TYPE_LEN];  /*物理型号 字符串信息*/
    UINT8 aucConnector[MAX_CONNECTOR_TYPE_LEN];   /*连接器型号*/
    UINT8 aucDistance[MAX_TRANSFER_DISTANCE_LEN];  /*传输距离字符串信息*/
    UINT8 aucVendorName[MAX_SFP_VENDOR_LEN];   /*模块制造商 信息*/
    UINT8 aucOriginal[MAX_SFP_VENDOR_LEN];    /*物理型号 字符串信息*/
    UINT8 aucPartNumber[MAX_SFP_VENDOR_LEN];    /*原始制造商 信息*/
    UINT8 aucRevNumber[MAX_SFP_VENDOR_LEN];
    UINT8 aucSerialNumber[MAX_SFP_VENDOR_LEN];    /*生产序列号字符串*/
    UINT8 aucProductDate[MAX_SFP_VENDOR_LEN];    /*生产日期字符串*/
    SFP_TXRX_POWER_S stPower;     /*最大最小光发送接收功率值*/
    SFP_PRODUCT_DATE_S stDate;    /*生产日期值*/
    SFP_TRANSFER_DISTANCE_S stDistance;    /*传输距离值*/
    UINT16 usWaveLen;    /*发送激光波长值*/
    UINT8 ucBXIndex; /*判断BX类型SFP模块的索引，使用前需保证是BX类型*/
} SFP_TRANSCEIVER_S;

#define ADDRESS_IN_USER_SPACE   1 /*地址是用户空间的地址*/
#define ADDRESS_IN_KERNEL_SPACE 2 /*地址是内核空间的地址*/

#if I2C_BUS_SUPPORT
VOID Drv_I2cInit(VOID);
UINT32 Drv_I2cGetFiberPortSpeed(port_num_t lport);
DRV_RET_E Drv_I2cPortRead(port_num_t ulport, UINT16 ucDevNo, UINT16 ucAddr, UINT8* pucData, UINT32 *pulLen, UINT8 ucReadType);
BOOL SFP_IsTP(UINT8 *pucData);
UINT32 SFP_GetModuleType(UINT8 *pucData);
VOID SFP_GetBxTypeIndex(UINT8 *pucData, UINT32 ulCodeVal, UINT8 *pucIndex);
DRV_RET_E SFP_GetTransceiverCode(UINT8 * pucData, UINT32 * pulCodeVal);
DRV_RET_E SFP_GetTransceiverType(UINT8 *pucData, UINT8 * pucTypeStr);
DRV_RET_E SFP_GetTransferDistance(UINT8 * pucData, UINT8 * pucDistance, SFP_TRANSFER_DISTANCE_S * pstTransfer);
DRV_RET_E SFP_GetConnectorType(UINT8 *pucData, UINT8 * pucTypeStr);
DRV_RET_E SFP_GetWaveLength(UINT8 *pucData, UINT16 * pusWaveLen);
DRV_RET_E SFP_GetDiagnostic(UINT8 *pucData, BOOL *pbSupDiagn);
DRV_RET_E SFP_GetVendorName(UINT8 * pucData, UINT8 * pucVdName, BOOL * bIsH3C);
DRV_RET_E SFP_GetOriginalfactory(UINT8 *pucData, UINT8 *pucOFName);
DRV_RET_E SFP_GetTXPower(UINT8 * pucData, UINT8 * pucMinTP, UINT8 * pucMaxTP, SFP_TXRX_POWER_S * pstTxPower);
DRV_RET_E SFP_GetRXPower(UINT8 * pucData, UINT8 * pucMinRP, UINT8 * pucMaxRP, SFP_TXRX_POWER_S * pstRxPower);
DRV_RET_E SFP_GetPartNumber(UINT8 *pucData, UINT8 *pucPartNum);
DRV_RET_E SFP_GetRevNumber(UINT8 *pucData, UINT8 *pucRevNum);
DRV_RET_E SFP_GetSerialNumber(UINT8 *pucData, UINT8 *pucSerialNum);
DRV_RET_E SFP_GetProductDate(UINT8 *pucData, UINT8 *pucDate);
DRV_RET_E SFP_GetInformationAll(port_num_t ulPort, SFP_TRANSCEIVER_S *pstSFPInfo);
#endif

/***********************************************************************/
/* drv_statistics相关定义 */
/************************************************************************/
typedef enum
{
    INOCTETS = 0,
    INUCAST,
    INNUCAST,
    INDISCARDS,
    INERRS,
    INUNKNOWN,
    OUTOCTETS,
    OUTUCAST,
    OUTNUCAST,
    OUTDISCARDS,
    OUTERRS,
    OUTQLEN,
    HCINOCTETS,
    HCINUCAST,
    HCINMCAST,
    HCINBCAST,
    HCOUTOCTETS,
    HCOUTUCAST,
    HCOUTMCAST,
    HCOUTBCAST,
    PORTDELAYEXEEDE_DISCARDS,
    PORTMTUEXCEED_DISCARDS,
    TPPPORT_IN_FRAMES,
    TPPORT_OUT_FRAMS,
    PORT_IN_DISCARDS,

    DOT3STATSALIGNERR,
    DOT3STATSFCSERR,
    DOT3STATSSINGLECOLLISION,
    DOT3STATSMULTICOLLISION,
    DOT3STATSSQETESTERR,
    DOT3STATSDEFERREDTX,
    DOT3STATSLATECOLLISION,
    DOT3STATSEXCOLLSION,
    DOT3STATSINTERMACTXERR,
    DOT3STATSCARRIERSENSEERR,
    DOT3STATSFRAMETOOLONG,
    DOT3STATSINTERMACRXERR,
    DOT3STATSSYMBOLERR
}MIB_IfStatType_t;

/*端口统计类型*/
typedef  enum
{
    INPUT_TOTAL_PACKETS = 0,
    INPUT_TOTAL_BYTES = 1,
    INPUT_TOTAL_BROADCASTS = 2,
    INPUT_TOTAL_MULTICASTS = 3,
    INPUT_ERRORS = 4,
    INPUT_PAUSE_PACKETS= 5,
    OUTPUT_TOTAL_PACKETS = 6,
    OUTPUT_TOTAL_BYTES = 7,
    OUTPUT_TOTAL_BROADCASTS = 8,
    OUTPUT_TOTAL_MULTICASTS = 9,
    OUTPUT_ERRORS = 10,
    OUTPUT_PAUSE_PACKETS = 11
}CounterName_E;

#ifdef CONFIG_PRODUCT_EPN204
typedef enum stat_reg_e
{
    //RX
    RxDropPkts=0,
    RxOctets=1,
    RxBroadcastPkts=2,
    RxMulticastPkts=3,
    RxSAChanges=4,
    RxUdersizePkts=5,
    RxOversizePkts=6,
    RxFragment=7,
    RxJabbers=8,
    RxUnicastPkts=9,
    RxAlignmentErrors=10,
    RxFCSErrors=11,
    RxGoodOctets=12,
    RxExcessSizeDisc=13,
    RxPausePkts=14,
    RxSymbolErrors=15,
    RxQosQPkt=16,
    RxQosQOctet=17,
    //TX
    TxDropPkts=18,
    TxOctets=19,
    TxBroadcastPkts=20,
    TxMulticastPkts=21,
    TxCollisions=22,
    TxUnicastPkts=23,
    TxSingleCollision=24,
    TxMultipleCollision=25,
    TxDeferredTransmit=26,
    TxLateCollision=27,
    TxPausePkts=28,
    TxFrameInDisc=29,
    TxQoSQPkt=30,
    TxQoSQOctet=31,
    Pkt64Octets=32,
    Pkt65to127Octets=33,
    Pkt128to255Octets=34,
    Pkt256to511Octets=35,
    Pkt512to1023Octets=36,
    Pkt1024to1522octets=37,
    RxErrPkts=38,
    TxErrPkts=39,
    END_OF_STAT_REG
}stat_reg_t;
#endif

#if 1
typedef enum stat_reg_e
{
    //RX
    RxDropPkts=0,
    RxOctets=1,
    RxUnicastPkts=2,
    RxBroadcastPkts=3,
    RxMulticastPkts=4,
    RxSAChanges=5,    
    RxFragment=6,
    RxJabbers=7,    
    RxAlignmentErrors=8,
    RxFCSErrors=9,
    RxGoodOctets=10,
    RxExcessSizeDisc=11,
    RxPausePkts=12,
    RxSymbolErrors=13,
    RxQosQPkt=14,
    RxQosQOctet=15,
	RxUdersizePkts=16,
    RxOversizePkts=17,
    RxPkt64Octets=18,
    RxPkt65to127Octets=19,
    RxPkt128to255Octets=20,
    RxPkt256to511Octets=21,
    RxPkt512to1023Octets=22,
    RxPkt1024to1518octets=23,
    RxPkt1519toMAXoctets=24,
    reserved1=25,
	reserved2=26,
	reserved3=27,
	reserved4=28,
	reserved5=29,
	reserved6=30,
    //TX
    TxDropPkts=31,
    TxOctets=32,
    TxUnicastPkts=33,
    TxBroadcastPkts=34,
    TxMulticastPkts=35,
    TxCollisions=36,   
    TxSingleCollision=37,
    TxMultipleCollision=38,
    TxDeferredTransmit=39,
    TxLateCollision=40,
    TxPausePkts=41,
    TxFrameInDisc=42,
    TxQoSQPkt=43,
    TxQoSQOctet=44,
    TxUdersizePkts=45,
    TxOversizePkts=46,
    TxPkt64Octets=47,
    TxPkt65to127Octets=48,
    TxPkt128to255Octets=49,
    TxPkt256to511Octets=50,
    TxPkt512to1023Octets=51,
    TxPkt1024to1518octets=52,
    TxPkt1519toMAXoctets=53,
    RxErrPkts=54,
    TxErrPkts=55,
    END_OF_STAT_REG
}stat_reg_t;
#endif

#define MIB_REG_NUM END_OF_STAT_REG
#define PORT_MIB_NUM 19
#ifndef __OPCONN_PORT_STAT__
#define __OPCONN_PORT_STAT__
typedef UINT64 port_statistics_t[MIB_REG_NUM];
#endif



DRV_RET_E Drv_InitStatistics(void);
DRV_RET_E Drv_GetPortCounter(port_num_t lport, CounterName_E counterName, UINT64 *returnValue);
DRV_RET_E Drv_GetPortStatistics(port_num_t lport, port_statistics_t *port_stat);
DRV_RET_E Drv_ClearPortStatistics(port_num_t lport);
DRV_RET_E Drv_SyncPortStatistics(void);
DRV_RET_E Drv_MIBGetPortStat(port_num_t lport, MIB_IfStatType_t PktType, UINT64 *p_ullStat);


/***********************************************************************/
/* drv_ratelimit相关定义 */
/************************************************************************/
#define   DRV_RATELIMIT_NONE  0x0
#define   DRV_RATELIMIT_RX    0x01      /*端口接收方向进行限速*/
#define   DRV_RATELIMIT_TX    0x02      /*端口发送方向进行限速*/
#define   MAX_100M_RATELEVEL  (UINT32)(100000/64)
#define   MAX_1000M_RATELEVEL (UINT32)(1000000/64)
#define   INVALID_RATELEVEL   (UINT32)((UINT32)-1/64)

DRV_RET_E Drv_InitLineRate(void);
DRV_RET_E Drv_SetLineRate(logic_pmask_t *pLgcPMask, UINT32 ulRateLevel,
    UINT32 ulDirection, UINT32 ulEnable);
DRV_RET_E Drv_InitStormControl(void);
DRV_RET_E Drv_SetStormControl(logic_pmask_t *pLgcPMask, STORM_CTLTYPE_E type, STORM_CTLRATE_S *pstStrom);

/***********************************************************************/
/* drv_qos相关定义 */
/************************************************************************/
#define QOS_MAX_1P_PRIORITY 8
#define QOS_MAX_DSCP_PRIORITY 64
#define QOS_MAX_INTERNAL_PRIORITY 8
#define QOS_MAX_1P_QUEUENO  8

#define QOS_COLOR_RED 2
#define QOS_COLOR_YELLOW 1
#define QOS_COLOR_GREEEN 0

#define QOS_COS           0
#define QOS_DSCP          1
#define QOS_PORT          2

#if defined(CHIPSET_RTL8305) || defined(CHIPSET_RTL9607) || defined(CHIPSET_RTL9601)
#define QOS_SVLAN         3
#endif

#define QOS_COS_STR       "cos"
#define QOS_DSCP_STR      "dscp"
#define QOS_PORT_STR      "port"
#define QOS_HQ_WRR_STR    "hq-wrr"
#define QOS_WRR_STR       "wrr"

/*1p priority map to internal priority */
typedef struct qos_8021p_to_intpri_s
{
    /*fill in internal priority, 0~7*/
    /*example: set 1p priority 1 map to internal priority 2, internalPri[1]=2;*/
    UINT8 internalPri[QOS_MAX_1P_PRIORITY];
} qos_8021p_to_intpri_t;

/*DSCP priority map to internal priority */
typedef struct qos_dscp_to_intpri_s
{
    /*fill in internal priority, 0~7*/
    /*example: set DSCP priority 1 map to internal priority 2, internalPri[1]=2;*/
    UINT8 internalPri[QOS_MAX_DSCP_PRIORITY];
} qos_dscp_to_intpri_t;


#define QUEUE_MODE_WRR 0
#define QUEUE_MODE_HQ_WRR 1
#define QUEUE_MODE_WFQ 2
#define QUEUE_MODE_WRR_STR  "wrr"
#define QUEUE_MODE_HQ_WRR_STR  "hq-wrr"
#define QUEUE_MODE_WFQ_STR  "wfq"
#define DEFAULT_QUEUE_MODE QUEUE_MODE_WRR
#define DEFAULT_QUEUE_WEIGHT_1 1
#define DEFAULT_QUEUE_WEIGHT_2 2
#define DEFAULT_QUEUE_WEIGHT_3 4
#define DEFAULT_QUEUE_WEIGHT_4 8
#define QUEUE_WEIGHT_COUNT 8

DRV_RET_E Drv_InitQoS(void);
DRV_RET_E Drv_SetQosTrustMode (UINT32 trustMode);
DRV_RET_E Drv_SetQoSQueueScheduler(QueueMode_S *stQueue);
DRV_RET_E Drv_SetQosDscpMap(qos_dscp_to_intpri_t *stDscpMap);
DRV_RET_E Drv_SetQosCosqMap(qos_8021p_to_intpri_t *stCosMap);

/********************************************************************************/
/*syslog相关定义                                                                */
/********************************************************************************/
/*针对info-center的所有开关设置值*/
#define IC_DISABLE     0
#define IC_ENABLE      1

#define IC_LOGHOST_LEN      16
#define IC_MODULE_DESC_LEN  512

typedef CHAR ic_loghost_t[IC_LOGHOST_LEN];
typedef CHAR ic_module_desc_t[IC_MODULE_DESC_LEN];



/********************************************************************************/
/*                       init                                                   */
/********************************************************************************/
INT32 Drv_SwitchInit(void);

/********************************************************************************/
/*                       drv_mvlan                                              */
/********************************************************************************/
DRV_RET_E Drv_VlanIf_DefaultDesc(vlan_id_t tdVid);
DRV_RET_E Drv_CPUPortToVlan(UINT32 ulAction, vlan_id_t tdVid);
DRV_RET_E Drv_MVlan_Init(void);
DRV_RET_E Drv_SetMVlan(vlan_id_t tdMVid);
DRV_RET_E Drv_VlanIfCreate(vlan_id_t tdMVid);
DRV_RET_E Drv_VlanIfDel(vlan_id_t tdMVid);

/********************************************************************************/
/*                       修改manage vlan接口                                         */
/********************************************************************************/
DRV_RET_E Drv_SetManageVlan(UINT32 uiVlanId);

#ifndef CONFIG_WDT
#define CONFIG_WDT 1
#endif

#ifdef  DRV_VLAN_DEBUG
#define Drv_debug   printf
#define Drv_debug_mask(ptr)    print_mask(ptr, 8)
#else
#define Drv_debug(str, _m...)
#define Drv_debug_mask(ptr)
#endif

/********************************************************************************/
/*                       drv_arp                                                */
/********************************************************************************/
DRV_RET_E Drv_CreateArpList(VOID * pstArpListHead);

/********************************************************************************/
/*                       drv_acl                                                */
/********************************************************************************/
typedef struct _ip_filter_s_ {
    ip_address_t ip;    
    port_num_t lPort;
    vlan_id_t vid;
    mac_address_t mac;
    UINT8 valid;
}ipFilter_t;

typedef struct _vlan_intf_s_ {
    mac_address_t int_mac;
	UINT32 uimacRuleid_ctag;
	UINT32 uiarpRuleid_ctag;
	UINT32 uidhcpbrdcreqRuleid_ctag;
	UINT32 uidhcpbrdsreqRuleid_ctag;
	UINT32 uidhcpbrdsackRuleid_ctag;
	UINT32 uidhcpbrdcackRuleid_ctag;
	UINT32 uidhcpbrdcreqRuleid_stag;
	UINT32 uidhcpbrdsreqRuleid_stag;
	UINT32 uidhcpbrdsackRuleid_stag;
	UINT32 uidhcpbrdcackRuleid_stag;
    vlan_id_t vid;
	int  valid;
}vlanIntf_t;


/*must same as realtek ACL_TRUST_MODE_E define*/
typedef enum DRV_tagACL_TrustMode
{
    DRV_ACL_TRUST_PORT = 1,
    DRV_ACL_TRUST_SMAC,
    DRV_ACL_TRUST_DMAC,
    DRV_ACL_TRUST_CTAG_PRIO,
    DRV_ACL_TRUST_ETHTYPE,
    DRV_ACL_TRUST_CTAG_VID,
    DRV_ACL_TRUST_IPV4_SIP,
    DRV_ACL_TRUST_IPV4_DIP,
    DRV_ACL_TRUST_IPV4_PRENCEDENCE,
    DRV_ACL_TRUST_IPV4_TOS,
    DRV_ACL_TRUST_IPV4_PROTOCOL,
    DRV_ACL_TRUST_TCP_SPORT,
    DRV_ACL_TRUST_TCP_DPORT,
    DRV_ACL_TRUST_UDP_SPORT,
    DRV_ACL_TRUST_UDP_DPORT,
    DRV_ACL_TRUST_STAG_VID,
    DRV_ACL_TRUST_END
}DRV_ACL_TRUST_MODE_E;


#define MAX_ACL_RULE_ID       65535      /*acl rule id*/

#define ACL_RULE_ADD  1
#define ACL_RULE_MOD  2
#define ACL_MOD_RULE_NOT_MIRROR  0

/*debug option*/
#define ACL_DEBUG_ENABLE     0

/* Values for drv_acl_ip_protocol_t from RFC 1700. */
#define DRV_ACL_IPPROTO_HOPBYHOP     0 /* IPv6 Hop-by-Hop option  */
#define DRV_ACL_IPPROTO_ICMP         1 /* Internet Control Message*/
#define DRV_ACL_IPPROTO_IGMP         2 /* Internet Group Management*/
#define DRV_ACL_IPPROTO_GGP          3 /* Gateway-to-Gateway */
#define DRV_ACL_IPPROTO_IP           4 /* IP in IP (encasulation) */
#define DRV_ACL_IPPROTO_ST           5 /* Stream */
#define DRV_ACL_IPPROTO_TCP          6 /* Transmission Control */
#define DRV_ACL_IPPROTO_UCL          7 /* UCL */
#define DRV_ACL_IPPROTO_EGP          8 /* Exterior Gateway Protocol */
#define DRV_ACL_IPPROTO_IGP          9 /* any private interior gateway */
#define DRV_ACL_IPPROTO_BBN_RCC_MON 10 /* BBN RCC Monitoring */
#define DRV_ACL_IPPROTO_NVP_II      11 /* Network Voice Protocol */
#define DRV_ACL_IPPROTO_PUP         12 /* PUP */
#define DRV_ACL_IPPROTO_ARGUS       13 /* ARGUS */
#define DRV_ACL_IPPROTO_EMCON       14 /* EMCON */
#define DRV_ACL_IPPROTO_XNET        15 /* Cross Net Debugger */
#define DRV_ACL_IPPROTO_CHAOS       16 /* Chaos */
#define DRV_ACL_IPPROTO_UDP         17 /* User Datagram */
#define DRV_ACL_IPPROTO_MUX         18 /* Multiplexing */
#define DRV_ACL_IPPROTO_DCN_MEAS    19 /* DCN Measurement Subsystems */
#define DRV_ACL_IPPROTO_HMP         20 /* Host Monitoring */
#define DRV_ACL_IPPROTO_PRM         21 /* Packet Radio Measurement */
#define DRV_ACL_IPPROTO_XNS_IDP     22 /* XEROX NS IDP */
#define DRV_ACL_IPPROTO_TRUNK_1     23 /* Trunk-1 */
#define DRV_ACL_IPPROTO_TRUNK_2     24 /* Trunk-2 */
#define DRV_ACL_IPPROTO_LEAF_1      25 /* Leaf-1 */
#define DRV_ACL_IPPROTO_LEAF_2      26 /* Leaf-2 */
#define DRV_ACL_IPPROTO_RDP         27 /* Reliable Data Protocol */
#define DRV_ACL_IPPROTO_IRTP        28 /* Internet Reliable Transaction */
#define DRV_ACL_IPPROTO_ISO_TP4     29 /* ISO Transport Protocol Class 4 */
#define DRV_ACL_IPPROTO_NETBLT      30 /* Bulk Data Transfer Protocol */
#define DRV_ACL_IPPROTO_MFE_NSP     31 /* MFE Network Services Protocol */
#define DRV_ACL_IPPROTO_MERIT_INP   32 /* MERIT Internodal Protocol */
#define DRV_ACL_IPPROTO_SEP         33 /* Sequential Exchange Protocol */
#define DRV_ACL_IPPROTO_3PC         34 /* Third Party Connect Protocol */
#define DRV_ACL_IPPROTO_IDPR        35 /* Inter-Domain Policy Routing Protocol */
#define DRV_ACL_IPPROTO_XTP         36 /* XTP */
#define DRV_ACL_IPPROTO_DDP         37 /* Datagram Delivery Protocol */
#define DRV_ACL_IPPROTO_IDPR_CMTP   38 /* IDPR Control Message Transport Proto */
#define DRV_ACL_IPPROTO_TP_P_P      39 /* TP++ Transport Protocol */
#define DRV_ACL_IPPROTO_IL          40 /* IL Transport Protocol */
#define DRV_ACL_IPPROTO_SIP         41 /* Simple Internet Protocol */
#define DRV_ACL_IPPROTO_SDRP        42 /* Source Demand Routing Protocol */
#define DRV_ACL_IPPROTO_SIP_SR      43 /* SIP Source Route */
#define DRV_ACL_IPPROTO_SIP_FRAG    44 /* SIP Fragment */
#define DRV_ACL_IPPROTO_IDRP        45 /* Inter-Domain Routing Protocol */
#define DRV_ACL_IPPROTO_RSVP        46 /* Reservation Protocol */
#define DRV_ACL_IPPROTO_GRE         47 /* General Routing Encapsulation */
#define DRV_ACL_IPPROTO_MHRP        48 /* Mobile Host Routing Protocol */
#define DRV_ACL_IPPROTO_BNA         49 /* BNA */
#define DRV_ACL_IPPROTO_SIPP_ESP    50 /* SIPP Encap Security Payload */
#define DRV_ACL_IPPROTO_SIPP_AH     51 /* SIPP Authentication Header */
#define DRV_ACL_IPPROTO_I_NLSP      52 /* Integrated Net Layer Security  TUBA */
#define DRV_ACL_IPPROTO_SWIPE       53 /* IP with Encryption */
#define DRV_ACL_IPPROTO_NHRP        54 /* NBMA Next Hop Resolution Protocol */
/* 55-60                 Unassigned*/
#define DRV_ACL_IPPROTO_AHIP        61 /* any host internal protocol */
#define DRV_ACL_IPPROTO_CFTP        62 /* CFTP */
#define DRV_ACL_IPPROTO_HI          63 /* Host internal protocol */
#define DRV_ACL_IPPROTO_SAT_EXPAK   64 /* SATNET and Backroom EXPAK */
#define DRV_ACL_IPPROTO_KRYPTOLAN   65 /* Kryptolan */
#define DRV_ACL_IPPROTO_RVD         66 /* MIT Remote Virtual Disk Protocol */
#define DRV_ACL_IPPROTO_IPPC        67 /* Internet Pluribus Packet Core */
#define DRV_ACL_IPPROTO_ADFS        68 /* any distributed file system */
#define DRV_ACL_IPPROTO_SAT_MON     69 /* SATNET Monitoring */
#define DRV_ACL_IPPROTO_VISA        70 /* VISA Protocol */
#define DRV_ACL_IPPROTO_IPCV        71 /* Internet Packet Core Utility */
#define DRV_ACL_IPPROTO_CPNX        72 /* Computer Protocol Network Executive */
#define DRV_ACL_IPPROTO_CPHB        73 /* Computer Protocol Heart Beat */
#define DRV_ACL_IPPROTO_WSN         74 /* Wang Span Network */
#define DRV_ACL_IPPROTO_PVP         75 /* Packet Video Protocol */
#define DRV_ACL_IPPROTO_BR_SAT_MON  76 /* Backroom SATNET Monitoring */
#define DRV_ACL_IPPROTO_SUN_ND      77 /* SUN ND PROTOCOL-Temporary */
#define DRV_ACL_IPPROTO_WB_MON      78 /* WIDEBAND Monitoring */
#define DRV_ACL_IPPROTO_WB_EXPAK    79 /* WIDEBAND EXPAK */
#define DRV_ACL_IPPROTO_ISO_IP      80 /* ISO Internet Protocol */
#define DRV_ACL_IPPROTO_VMTP        81 /* VMTP */
#define DRV_ACL_IPPROTO_SECURE_VMTP 82 /* SECURE-VMTP */
#define DRV_ACL_IPPROTO_VINES       83 /* VINES */
#define DRV_ACL_IPPROTO_TTP         84 /* TTP */
#define DRV_ACL_IPPROTO_NSFNET_IGP  85 /* NSFNET-IGP */
#define DRV_ACL_IPPROTO_DGP         86 /* Dissimilar Gateway Protocol */
#define DRV_ACL_IPPROTO_TCF         87 /* TCF */
#define DRV_ACL_IPPROTO_IGRP        88 /* IGRP */
#define DRV_ACL_IPPROTO_OSPFIGP     89 /* OSPFIGP */
#define DRV_ACL_IPPROTO_SPRITE_RPC  90 /* Sprite RPC Protocol */
#define DRV_ACL_IPPROTO_LARP        91 /* Locus Address Resolution Protocol */
#define DRV_ACL_IPPROTO_MTP         92 /* Multicast Transport Protocol */
#define DRV_ACL_IPPROTO_AX_25       93 /* AX.25 Frames */
#define DRV_ACL_IPPROTO_IPIP        94 /* IP-in-IP Encapsulation Protocol */
#define DRV_ACL_IPPROTO_MICP        95 /* Mobile Internetworking Control Pro. */
#define DRV_ACL_IPPROTO_SCC_SP      96 /* Semaphore Communications Sec. Pro. */
#define DRV_ACL_IPPROTO_ETHERIP     97 /* Ethernet-within-IP Encapsulation */
#define DRV_ACL_IPPROTO_ENCAP       98 /* Encapsulation Header */
#define DRV_ACL_IPPROTO_APES        99 /* any private encryption scheme */
#define DRV_ACL_IPPROTO_GMTP       100 /* GMTP */
#define DRV_ACL_IPPROTO_L2TP       115
#define DRV_ACL_IPPROTO_ISIS       124
/* 101-254                Unassigned */
#define DRV_ACL_IPPROTO_ANY        255 /* Reserved */


#define MAX_ACL_NAME_LEN    33

#define ACL_RULE_PRIO_MODE_AUTO  1
#define ACL_RULE_PRIO_MODE_MAN   2

#define ACL_TYPE_MAC_BASED       1
#define ACL_TYPE_IP_BASED        2

#define ACL_RULE_ACTION_DENY     1
#define ACL_RULE_ACTION_PERMIT   2
#define ACL_RULE_ACTION_MIRROR   3
#define ACL_RULE_ACTION_METER    4
#define ACL_RULE_ACTION_NEW_PRI  5

#define ACL_RULE_ACTION_PRI_LOWEST          0
#define ACL_RULE_ACTION_PRI_LOW             1
#define ACL_RULE_ACTION_PRI_HIGH            2
#define ACL_RULE_ACTION_PRI_HIGHEST         3

#define ACL_MACRULE_QUALIFY_SRC_MAC_MASK    0x01
#define ACL_MACRULE_QUALIFY_DST_MAC_MASK    0x02
#define ACL_MACRULE_QUALIFY_VLAN_ID_MASK    0x04
#define ACL_MACRULE_QUALIFY_ETH_TYPE_MASK   0x08
#define ACL_MACRULE_QUALIFY_COS_MASK        0x10

typedef struct drv_acl_list_macBased_s {
    UINT16 usAclPrio;                   /*ACL number*/
    UINT16 usRulePrio;                  /*1~65535, smaller number means higher priority*/
    vlan_id_t usVlanId;
    mac_address_t ucpSrcMac;
    mac_address_t ucpSrcMacMask;        /*set 1 to qualify*/
    mac_address_t ucpDstMac;
    mac_address_t ucpDstMacMask;        /*set 1 to qualify*/
    UINT8 ucCos;                        /*802.1p priority, 0~7, not valid in BCM534X*/
    UINT8 ucCosMask;                    /*0~7, 0 means "don't care", set 1 to qualify, not valid in BCM534X*/
    UINT16 usEtherType;
    UINT8 ucAction;                     /*permit deny*/    
    UINT8 fQualifyFlag;                 /*
                                        fQualifyFlag bit definition
                                        bit0:source mac address
                                        bit1:dest mac address  
                                        bit2:vlan id
                                        bit3:ethernet type
                                        bit4:cos
                                        */
    UINT32 ulIdentifier;
    UINT8 ucNewPri;                     /*
                                        ACL_RULE_ACTION_PRI_LOWEST
                                        ACL_RULE_ACTION_PRI_LOW
                                        ACL_RULE_ACTION_PRI_HIGH
                                        ACL_RULE_ACTION_PRI_HIGHEST
                                        */
    UINT32 ulMeterRate;                 /*rate limit (kbps)*/
    UINT8 mirrorDstPort;
} drv_acl_list_macBased_t;

#define ACL_IPRULE_QUALIFY_SRC_L4_PORT_MASK     0x01
#define ACL_IPRULE_QUALIFY_DST_L4_PORT_MASK     0x02
#define ACL_IPRULE_QUALIFY_TCP_FLAG_MASK        0x04
#define ACL_IPRULE_QUALIFY_SRC_IP_MASK          0x08
#define ACL_IPRULE_QUALIFY_DST_IP_MASK          0x10
#define ACL_IPRULE_QUALIFY_DSCP_MASK            0x20
#define ACL_IPRULE_QUALIFY_IP_PRECEDENCE_MASK   0x40
#define ACL_IPRULE_QUALIFY_IP_PROTO_MASK        0x80

#define ACL_IPRULE_QUALIFY_TCP_FIN_MASK         0x01
#define ACL_IPRULE_QUALIFY_TCP_SYN_MASK         0x02
#define ACL_IPRULE_QUALIFY_TCP_RST_MASK         0x04
#define ACL_IPRULE_QUALIFY_TCP_PUSH_MASK        0x08
#define ACL_IPRULE_QUALIFY_TCP_ACK_MASK         0x10
#define ACL_IPRULE_QUALIFY_TCP_URGENT_MASK      0x20

/*CFG 映射数据，29字节*/
typedef struct drv_acl_list_ipBased_s {
    UINT16 usAclPrio;                   /*ACL number*/
    UINT16 usRulePrio;                  /*1~65535, smaller number means higher priority*/
    UINT8 ucIpProtocol;                 /*see DRV_ACL_IPPROTO_XXX */
    UINT16 usSrcL4Port;                 /*
                                        TCP/UDP/ICMP/IGMP L4 field
                                        0~15bit:tcp or udp source port number when ucIpProtocol == DRV_ACL_IPPROTO_TCP
                                        0~7bit:ICMP/IGMP type when ucIpProtocol == DRV_ACL_IPPROTO_ICMP OR IGMP 
                                        8~15bit:ICMP/IGMP code when ucIpProtocol == DRV_ACL_IPPROTO_ICMP OR IGMP 
                                        */
    UINT16 usDstL4Port;                 /*tcp or udp dest port number*/
    ip_address_t ucSrcIpAdd;
    ip_address_t ucSrcIpAddMask;        /*set 1 to qualify*/
    ip_address_t ucDstIpAdd;
    ip_address_t ucDstIpAddMask;        /*set 1 to qualify*/
    UINT8 fTcpFlag;                     /*bit5:urgent; bit4:ack; bit3:push; bit2:rst; bit1:syn ;bit0:fin*/
    UINT8 fTcpFlagMask;
    UINT8 ucTos;                        /*DSCP & ip precedence & tos, related to fQualifyFlag, not valid in BCM534X*/
    UINT8 ucAction;                     /*permit deny*/
    UINT8 fQualifyFlag;                 /*
                                        fQualifyFlag bit definition
                                        bit0:source port  16bits
                                        bit1:dest port    16bits
                                        bit2:tcp flags    6bits
                                        bit3:source ip address   
                                        bit4:dest ip address     
                                        bit5:dscp                6bits
                                        bit6:ip precedence       3bits
                                        bint7:ip protocol        16bits
                                        */ 
    UINT32 ulIdentifier;
    UINT8 ucNewPri;                     /*
                                        ACL_RULE_ACTION_PRI_LOWEST
                                        ACL_RULE_ACTION_PRI_LOW
                                        ACL_RULE_ACTION_PRI_HIGH
                                        ACL_RULE_ACTION_PRI_HIGHEST
                                        */
    UINT32 ulMeterRate;                 /*rate limit (kbps)*/
    UINT8 mirrorDstPort;
} drv_acl_list_ipBased_t;

/*CFG 映射数据，20字节*/
typedef struct drv_acl_list_common_s {
    UINT16 usAclPrio;                       /*priority, 1~MAX_ACL_SUPPORT, smaller number means higher priority，0表示无效*/
    UINT8 bAclRulePrioMode;                 /*manual mode(ACL_RULE_PRIO_MODE_MAN); 
                                              auto mode(ACL_RULE_PRIO_MODE_AUTO, 
                                              more qulify bits means higher priority),
                                              匹配深度相同时，先添加RULE的优先级较高*/
    CHAR szAclName[MAX_ACL_NAME_LEN];      /*ACL  description*/
    UINT8 ucAclType;                        /*ACL_TYPE_MAC_BASED; ACL_TYPE_IP_BASED*/
} drv_acl_list_common_t;

/*CFG 映射数据，4字节*/
typedef struct drv_acl_bind_s {
    UINT16 usAclPrio;                       /*priority, 1~MAX_ACL_SUPPORT, smaller number means higher priority，0表示无效*/
    logic_pmask_t stPortMask;               /*ACL BINGDING port mask*/
    UINT16 usVid;
}drv_acl_bind_t;

typedef struct drv_acl_rule_link_s {
    void *pstRule;
    struct drv_acl_rule_link_s *pstRuleLinkPre;
    struct drv_acl_rule_link_s *pstRuleLinkNxt;
    UINT16 usKeyBitWide;
    UINT16 usPriority;
    UINT32 ulIdentifier;
}drv_acl_rule_link_t;

typedef struct drv_acl_list_common_link_s {
    drv_acl_rule_link_t *pstRuleHead;
    UINT16 usAclPrio;                       /*表示唯一的ACL，做为ACL ID使用*/    
    UINT16 usTotalRule;
    UINT8 ucRuleType;
} drv_acl_list_common_link_t;

typedef enum {
    WORM_FILTER_DROP,
    DOS_DROP,
    CASHER_CHANGE_PRI,
    IMP_BIND_DROPCANCEL,
    IMP_BIND_DROP,
    DOS_METER,
    ARP_TRUST_COPY,
    ARP_SNOOPING_EN,
    ACL_MIRROR,
    ACL_MIRROR_CANCEL,
    IP_FILTER_DROP,
    DHCP_CLIENT_PASS,
    DHCP_SERVER_DROP,
    DHCP_SERVER_COPY,
    MAC_ACL_BIND_ENTRY,
    IP_ACL_BIND_ENTRY,
    CPU_ATTACK_PREVENT,
    ARP_ATTACK_DROP,
    VOICE_VLAN_ACL,
    SWITCH_DSTMAC_COPY,
    CPU_ATTACK_BC,
    DOT1X_EAP_COPY,
    ACL_MODULE_TYPE_MAX
}field_entry_pri_type;

#define ACL_ALLOC 1
#define ACL_FREE  0

typedef struct acl_res_mng_s {
    UINT32 device_cnt;
    UINT32 group1_cnt;        /*ip+mac+port+vlan使用*/
    UINT32 group2_cnt;
    UINT32 group3_cnt;        /*ACL bind 使用*/
    UINT32 group4_cnt;        /*其他模块使用*/
    UINT32 group1_used;
    UINT32 group2_used;
    UINT32 group3_used;
    UINT32 group4_used;
    UINT32 acl_module_used[ACL_MODULE_TYPE_MAX];
}acl_res_mng_t;    

#if CONFIG_WDT                
void Drv_updateWatchdog(void);
#endif

DRV_RET_E Drv_AclInit(void);
DRV_RET_E Drv_AclTest(void);
DRV_RET_E drv_acl_get_empty(UINT16 *pusAclPrio);
DRV_RET_E drv_mac_acl_get_empty(UINT16 *pusAclPrio);
DRV_RET_E drv_ip_acl_get_empty(UINT16 *pusAclPrio);
DRV_RET_E drv_acl_create(drv_acl_list_common_t *pstAclEntry);
BOOL drv_acl_mbRule_isExist(UINT16 usAclPrio, UINT16 usRulePrio, UINT32 * ulRuleIdentifer);
BOOL drv_acl_ibRule_isExist(UINT16 usAclPrio, UINT16 usRulePrio, UINT32 * ulRuleIdentifer);
DRV_RET_E drv_acl_rule_remove_forCli(UINT16 usAclPrio, UINT16 usRulePrio);
DRV_RET_E drv_acl_set_name(UINT16 usAclPrio, CHAR * ucName);
DRV_RET_E drv_acl_remove_name(UINT16 usAclPrio);
DRV_RET_E drv_acl_copy(UINT16 usCopyFrom, UINT16 usNewAclPrio, CHAR *szAclName, UINT8 ucNameLen);
DRV_RET_E drv_acl_ibRule_mod(drv_acl_list_ipBased_t *pstAclIpRule, UINT32 ulRuleIdentifer);
DRV_RET_E drv_acl_mbRule_mod(drv_acl_list_macBased_t *pstAclMacRule, UINT32 ulRuleIdentifer);
DRV_RET_E drv_acl_ibRule_add(drv_acl_list_ipBased_t *pstAclIpRule, UINT32 *pulRuleIdentifer);
DRV_RET_E drv_acl_mbRule_add(drv_acl_list_macBased_t *pstAclMacRule, UINT32 *pulRuleIdentifer);
DRV_RET_E drv_acl_rule_remove(UINT16 usAclPrio, UINT32 ulRuleIdentifer);
DRV_RET_E drv_acl_rule_remove_all(UINT16 usAclPrio);
DRV_RET_E drv_acl_bind_port(logic_pmask_t *pbmp, UINT16 usAclPrio);
DRV_RET_E drv_acl_bind_vlan(vlan_id_t usVid, UINT16 usAclPrio);
DRV_RET_E drv_acl_unbind_vlan(vlan_id_t usVid, UINT16 usAclPrio);
DRV_RET_E drv_acl_unbind_port(logic_pmask_t *pbmp, UINT16 usAclPrio);
DRV_RET_E drv_acl_remove(UINT16 usAclPrio);
DRV_RET_E drv_acl_disp_rule(UINT16 usAclPrio);
DRV_RET_E VoiceVLAN_SetACL (port_num_t lport, UINT16 usVlanId, UINT8 ucOUI[6],
    UINT8 ucOUImask[6], UINT8 ucCos, UINT8 ucDscp);
DRV_RET_E VoiceVLAN_DelACL (port_num_t lport, UINT16 usVlanId, UINT8 ucOUI[6],
    UINT8 ucOUImask[6], UINT8 ucCos, UINT8 ucDscp);

DRV_RET_E drv_acl_create_BpduLimit(port_num_t lport, UINT8 ucLastDMacByte, 
    UINT8 ucLastDMacByteMask, UINT32 ulRate);
DRV_RET_E drv_acl_del_BpduLimit(port_num_t lport, UINT8 ucLastDMacByte, 
    UINT8 ucLastDMacByteMask, UINT32 ulRate);

DRV_RET_E drv_acl_update_cpu_mac(void);
DRV_RET_E drv_acl_update_cpu_ip(void);
#ifdef CONFIG_DHCPSP
DRV_RET_E drv_dhcpsp_trust_port(port_num_t lport, UINT8 enable);
DRV_RET_E drv_dhcpsp_enable(UINT8 enable);
DRV_RET_E drv_dhcpsp_init(void);
DRV_RET_E drv_dhcpsp_ipfilter_enable(port_num_t lport, UINT8 enable);
#endif
DRV_RET_E drv_dhcpsp_drop_arp(BOOL enable);

DRV_RET_E drv_get_aclEntry(UINT16 usIndex, drv_acl_list_common_t * pstaclEntry);
DRV_RET_E drv_get_aclBind(UINT16 usIndex, drv_acl_bind_t * pstaclBind);
UINT32 drv_acl_get_acl_cnt(UINT16 usAclPiro); 
DRV_RET_E drv_acl_sync_bind_clear(logic_pmask_t * pstPortMask);
DRV_RET_E drv_acl_sync_bind(port_num_t lPortNo, logic_pmask_t * pstPortMask);

/********************************************************************************/
/*                       drv_ieee8023az                                                */
/********************************************************************************/
DRV_RET_E Drv_InitEEE(void);
DRV_RET_E Drv_SetEEEEnable(logic_pmask_t *plportMask, BOOL enable);
DRV_RET_E Drv_InitAutoPwrDwn(void);
DRV_RET_E Drv_SetAutoPwrDwnEnable(logic_pmask_t *plportMask, BOOL enable);

/********************************************************************************/
/*                       drv_cablediag                                               */
/********************************************************************************/

/********************************************************************************/
/*                       opconn进程接口                                         */
/********************************************************************************/

int drv_opl_Connect(void);
int drv_opl_recvBuf(int sock, UCHAR *pBuf, int iBufLen);
int drv_opl_sendBuf(int sock, UCHAR *pBuf, int iBufLen);
void print2console(char *fmt, ...);


typedef enum
{
    CABLE_STATE_OK = 0,
    CABLE_STATE_OPEN,
    CABLE_STATE_SHORT,
    CABLE_STATE_OPENSHORT,
    CABLE_STATE_CROSSTALK,
    CABLE_STATE_UNKNOWN,
    CABLE_STATE_COUNT		/* last, as always */
}cable_state_t;

typedef struct cable_diag_s{
    cable_state_t	state;		    /* state of all pairs */
    UINT8           npairs;		    /* pair_* elements valid */
    cable_state_t	pair_state[4];	/* pair state */
    UINT16	        pair_len[4];	/* pair length in metres */
    UINT8           fuzz_len;	    /* len values +/- this */
}cable_diag_t;

DRV_RET_E Drv_getCableDiag(port_num_t lport, cable_diag_t *pstDiag);
#define L2_LOGIC_ENABLE				1
#define L2_LOGIC_DISABLE 			0
#define L2_NO_PRIORITY				0xff
typedef struct tagSendOp{    
    logic_pmask_t portmask; 					/*端口掩码*/
		UINT8 ucLogicEnable;					/*逻辑发送控制,L2_LOGIC_ENABLE 开启L2_LOGIC_DISABLE 关闭
											      * 若开启,则芯片按照l2 规则,rstp状态等决定报文的发送
											      *如果关闭,则直接发送此报文
												*/
	   	UINT16 usVid;							/*发送报文vlan*/
    	UINT8 ucPriority;						/*发送报文内部优先级*/
   		UINT8 ucDevid;							/*设备id,0,1,2,*/
}l2_send_op;
struct mw_l2_ctl_s
{
    unsigned short    sll_port;
    unsigned short    sll_vlanid; 
};

DRV_RET_E Drv_LoopBackExternal (port_num_t lport);
DRV_RET_E Drv_LoopBackInternal (port_num_t lport);



#if defined(CHIPSET_RTL8305) || defined(CHIPSET_RTL9607) || defined(CHIPSET_RTL9601)
/*Realtek header will be behind the src mac or befor the crc of frame.*/
#define SWITCH_HEADER_POSITION_BEHIND_SRC_MAC  0

//#define ACL_RULE_NUM_MAX RTL8367B_ACLRULEMAX
#define ACL_RULE_NUM_MAX 64
#define CLF_RULE_NUM_MAX 512

#define SWITCH_CPU_PHY_PORT           6
#define SWITCH_UPLINK_PHY_PORT        6

typedef struct  vlan_port_base_entry
{
    unsigned short 	evid;
	unsigned short 	mbr;
    unsigned short  fid_msti;
    unsigned short  envlanpol;
    unsigned short  meteridx;
    unsigned short  vbpen;
    unsigned short  vbpri;
}vlan_port_base_entry_t;
#else
#define ACL_RULE_NUM_MAX 0

#define SWITCH_CPU_PHY_PORT           0
#define SWITCH_UPLINK_PHY_PORT        0
#endif
#define ACL_RULE_ID_IVALLID          0xFFFF

#define DEFAULT_VLAN_ID              0
#define DEFAULT_SVLAN_MEM_CFG_ENTRY  63
#define DEFAULT_SVLAN_ID             4095
#define CTC_VLAN_TRANSL_PAIR_NUM_MAX 31
#define CTC_CLF_REMARK_RULE_NUM_MAX  8
#define EOAM_FILTER_RULE_NUM_MAX     32
#define MAX_QINQ_CTAG_STAG_ENTRY    8
#define MAX_QINQ_CTAG_NUM           32
#define QINQ_CLASS_NAME_LEN         32

#if defined(CONFIG_PRODUCT_EPN104)
#define LOGIC_PON_PORT    LOGIC_CPU_PORT
#elif defined(CONFIG_PRODUCT_EPN104N) || defined (CONFIG_PRODUCT_5500) || defined(CONFIG_PRODUCT_EPN104W) || defined(CONFIG_PRODUCT_EPN101R) || defined(CONFIG_PRODUCT_EPN101ZG) || defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN105) || defined(CONFIG_PRODUCT_GPN104N)
#define LOGIC_PON_PORT 	  LOGIC_PORT_NO + 1
#else
#define LOGIC_PON_PORT     INVALID_PORT  
#endif
#define LOGIC_UPPON_PORT  LOGIC_PON_PORT    /*for packet tx lport,see Hal_L2send()*/

typedef enum tagMCAST_UnknowAct
{
    MCAST_ACT_FORWARD = 0,
    MCAST_ACT_DROP,
    MCAST_ACT_TRAP2CPU,
    MCAST_ACT_ROUTER_PORT,
    MCAST_ACT_END
} MCAST_UNKNOW_ACT_E;

typedef enum tagACL_TrustMode
{
    ACL_TRUST_PORT = 1,
    ACL_TRUST_SMAC,
    ACL_TRUST_DMAC,
    ACL_TRUST_CTAG_PRIO,
    ACL_TRUST_ETHTYPE,
    ACL_TRUST_CTAG_VID,
    ACL_TRUST_IPV4_SIP,
    ACL_TRUST_IPV4_DIP,
    ACL_TRUST_IPV4_PRENCEDENCE,
    ACL_TRUST_IPV4_TOS,
    ACL_TRUST_IPV4_PROTOCOL,
    ACL_TRUST_TCP_SPORT,
    ACL_TRUST_TCP_DPORT,
    ACL_TRUST_UDP_SPORT,
    ACL_TRUST_UDP_DPORT,
    ACL_TRUST_STAG_VID,
    ACL_TRUST_END
}ACL_TRUST_MODE_E;

typedef enum tagACL_Action
{
    ACL_ACTION_REMATK_PRIORITY = 0,
    ACL_ACTION_MIRROR,
    ACL_ACTION_CVLAN_REMARK,
    ACL_ACTION_CVLAN_ASSIGN,
    ACL_ACTION_SVLAN_REMARK,
    ACL_ACTION_COPY_TO_PORTS,
    ACL_ACTION_REDIRECT_TO_PORTS,
    ACL_ACTION_COPY_TO_CPU,
    ACL_ACTION_TRAP_TO_CPU,
    ACL_ACTION_POLICING_0,
    ACL_ACTION_DROP,
    ACL_ACTION_END
}ACL_ACTION_E;

/*begin added by liaohongjun 2012/11/30 of EPN104QID0084*/
typedef enum tagDropLoopd_Operation
{
    ACL_DROPLOOPD_ADD_PORT,
    ACL_DROPLOOPD_REMOVE_PORT,
    ACL_DROPLOOPD_END
}DROP_LOOPD_OP_E;
/*end added by liaohongjun 2012/11/30 of EPN104QID0084*/

typedef enum tagACL_Direction
{
    ACL_DIRECTION_UPLINK,
    ACL_DIRECTION_DOWNLINK,
    ACL_DIRECTION_BOTH,
    ACL_DIRECTION_END
}ACL_DIRECTION_E;

typedef enum tagCTC_VlanMemMode
{
    VLAN_MEM_UNTAG,
    VLAN_MEM_TAG,
    VLAN_MEM_NO,
    VLAN_MEM_END
} CTC_VLAN_MEM_E;

typedef enum tagCTC_VlanMode
{
    CTC_VLAN_MODE_TRANSPARENT       = 0x01,
    CTC_VLAN_MODE_TAG               = 0x02,
    CTC_VLAN_MODE_TRANSLATION       = 0x03,
    CTC_VLAN_MODE_QINQ              = 0x04,
	CTC_VLAN_MODE_TRUNK				= 0x05,
    CTC_VLAN_MODE_END
} CTC_VLAN_MODE_E;

typedef struct tagCTC_VlanCfg
{
    CTC_VLAN_MODE_E      mode;
    UINT32          default_vlan;
    UINT32          uiPriority;
    UINT32          vlan_list[CTC_VLAN_TRANSL_PAIR_NUM_MAX];
    UINT32          number_of_entries;  /* This parameter describes how many entries in <vlan_list> are used, when <mode> == <CTC_VLAN_MODE_TRANSLATION> */
    UINT32          number_of_acl_rules;
    UINT32          acl_list[ACL_RULE_NUM_MAX];

} CTC_VLAN_CFG_S;

typedef struct tagPORT_ClfRemarkCfg
{
    UINT32           uiClfRmkRuleNum;
    /* L4-port classifier needs 2 rules(tcp-port and udp-port).
    Udp-port rules are stored in bellow array from index 8. */
    UINT32           auiAclList[CTC_CLF_REMARK_RULE_NUM_MAX*2];
    ACL_TRUST_MODE_E aenClfRemarkMode[CTC_CLF_REMARK_RULE_NUM_MAX*2];
} PORT_CLF_REMAERK_CFG_S;

typedef struct tagPORT_ClfPri2QueueCfg
{
    UINT32           uiClfRmkRuleNum;
    /* L4-port classifier needs 2 rules(tcp-port and udp-port).
    Udp-port rules are stored in bellow array from index 8. */
    UINT32           auiClfList[CTC_CLF_REMARK_RULE_NUM_MAX*2];
} PORT_CLF_PRI_TO_QUEUE_CFG_S;

typedef struct tagPORT_FilterCfg
{
    UINT32           uiFilterRuleNum;
    UINT32           auiAclList[EOAM_FILTER_RULE_NUM_MAX*2];
    ACL_TRUST_MODE_E aenFiletrkMode[EOAM_FILTER_RULE_NUM_MAX*2];
} PORT_FILTER_CFG_S;

//#ifdef CTC_MULTICAST_SURPORT
#if 1
#define CTC_MC_MAX_GROUP_NUM  64
#define CTC_MC_VLAN_ID_IVALID 0xFFFF
typedef struct tagPORT_McVlanCfg
{
    UINT32           uiMcVlanNum;
    UINT32           auiAclList[CTC_MC_MAX_GROUP_NUM];
    UINT32           auiVlanList[CTC_MC_MAX_GROUP_NUM];
} PORT_MC_VLAN_CFG_S;
#endif

typedef struct tagQinq_Ctag_Stag_Entry
{
    BOOL   bAllCtagVid;
    UINT32 uiCtagVidNum;
    UINT32 auiCtagVid[MAX_QINQ_CTAG_NUM];
    UINT32 uiStagVid;
    UINT32 uiStagPri;
    UINT32 uiAclClassNum;
    char   astrAclClassName[MAX_QINQ_CTAG_NUM+1][QINQ_CLASS_NAME_LEN];
}QINQ_CTAG_STAG_ENTRY_S;

typedef struct tagPort_Qinq
{
    BOOL                    bEnable;
    UINT32                  uiTpid;
    UINT32                  uiPvid;
    UINT32                  uiPriority;
    UINT32                  uiCtagStagEntryNum;
    QINQ_CTAG_STAG_ENTRY_S  astCtagStagEntry[MAX_QINQ_CTAG_STAG_ENTRY];
    //UINT32 uiAclRuleNum;
    //UINT32 auiAclRuleIdList[ACL_RULE_MAX_NUM];
}PORT_QINQ_S;

typedef enum tagPORT_Ingress_Mode
{
    PORT_IN_FRAM_BOTH = 0,
    PORT_IN_FRAM_TAGGED_ONLY,
    PORT_IN_FRAM_UNTAGGED_ONLY,
    PORT_IN_FRAM_END
} PORT_INGRESS_MODE_E;

typedef enum tagPORT_Egress_Mode
{
    PORT_EG_TAG_MODE_ORI = 0,
    PORT_EG_TAG_MODE_KEEP,
    PORT_EG_TAG_MODE_PRI_TAG,
    PORT_EG_TAG_MODE_REAL_KEEP,
    PORT_EG_TAG_MODE_END
} PORT_EGRESS_MODE_E;


/********************************************************************************/
/*                       drv rstp                                                                                                                         */
/********************************************************************************/
typedef enum tagPORT_Stp_state
{
    PORT_STATE_DISABLED = 0,
    PORT_STATE_BLOCKING,
    PORT_STATE_LEARNING,
    PORT_STATE_FORWARDING,
    PORT_STATE_END
}PORT_STP_STATE_E;


typedef enum tagPORT_ALERT_PONSTATUS
{
    PORT_ALERT_PON_ALWAYS_LASER_ON = 1,
    PORT_ALERT_PON_ALWAYS_LASER_RECOVER,
    PORT_ALERT_PON_BUTT
}PORT_ALERT_PONSTATUS_E;

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif  /* end of _LW_DRV_PUB_H_ */
