/*****************************************************************************
                                                                             
*****************************************************************************/
#ifndef _DRV_REQ_H_
#define _DRV_REQ_H_

#ifdef  __cplusplus
extern "C"{
#endif

#include <lw_config.h>
#define DEV_SWTICH "l2switch"
#define DEV_PORT_NA 1

#define SIOCDRVPRIVATE (SIOCDEVPRIVATE + 12)
#define TIOTTYCLOSE    (SIOCDEVPRIVATE + 19) /*see compat_ioctl.c*/

#define IOCTL_SUPPORT_MAX_PARA_NUM 3

#define DRV_SPEED_0M        0 /*down*/
#define DRV_SPEED_10M       1
#define DRV_SPEED_100M      2
#define DRV_SPEED_1000M     3
#define DRV_SPEED_AUTO      4

#define DRV_DEPLUX_HALF     1
#define DRV_DEPLUX_FULL     2
#define DRV_DEPLUX_AUTO     3

#define DRV_PORT_LINKDOWN   1
#define DRV_PORT_LINKUP     2


#define MONITOR_INIT 0xff
#define DBG_COM_LEN 4

/* GPIO 管脚定义 */
#define DRV_GPIO_0_SPI      0
#define DRV_GPIO_1_SPI      1
#define DRV_GPIO_2_SPI      2
#define DRV_GPIO_3_SS0      3
#define DRV_GPIO_4_UART     4
#define DRV_GPIO_5_UART     5
#define DRV_GPIO_6_I2C      6
#define DRV_GPIO_7_I2C      7
#define DRV_GPIO_8_SS1      8
#define DRV_GPIO_9_SS2      9
/*begin added by liaohongjun 2012/11/12 of EPN104QID0081*/
#define DRV_GPIO_9_PON_TX   9
/*end added by liaohongjun 2012/11/12 of EPN104QID0081*/
#define DRV_GPIO_10_TX      10
#define DRV_GPIO_11_VCCT    11

#define DRV_SYS_LED_FILED_OFFSET   8

/* GPIO 工作模式定义 */
#define DRV_GPIO_WORKMODE_GPIO  0
#define DRV_GPIO_WORKMODE_SPI   1
#define DRV_GPIO_WORKMODE_UART  1
#define DRV_GPIO_WORKMODE_I2C   1

/* GPIO 电平取值定义 */
#define DRV_GPIO_VALUE_LOW   0
#define DRV_GPIO_VALUE_HIGH  1

/* 地址的宽度*/
#define WIDE_BYTE      0
#define WIDE_HWORD     1
#define WIDE_WORD      2
#define WIDE_DWORD     3

#define OPERATION_TYPE_NO    0
#define OPERATION_TYPE_RDB   1
#define OPERATION_TYPE_RDH   2
#define OPERATION_TYPE_RD    3
#define OPERATION_TYPE_RDD   4
#define OPERATION_TYPE_CPB   5
#define OPERATION_TYPE_CPH   6
#define OPERATION_TYPE_CP    7
#define OPERATION_TYPE_CPD   8
#define OPERATION_TYPE_WRB   9
#define OPERATION_TYPE_WRH   10
#define OPERATION_TYPE_WR    11
#define OPERATION_TYPE_WRD   12
#define OPERATION_TYPE_ER    13
#define OPERATION_TYPE_NUM   14

#define OPERATION_VAL_MAX_BYTE     256
#define OPERATION_VAL_MAX_HWORD    (OPERATION_VAL_MAX_BYTE/2)
#define OPERATION_VAL_MAX_WORD     (OPERATION_VAL_MAX_BYTE/4)
#define OPERATION_VAL_MAX_DWORD    (OPERATION_VAL_MAX_BYTE/8)


typedef enum tagDrv_cmd{
    DRV_CMD_FIBER_CFG_INIT ,
    DRV_CMD_COPPER_CFG_INIT,
    DRV_CMD_SET_MEDIUM,
    DRV_CMD_SET_PRIORITY,
    DRV_CMD_SET_MDIX,
    DRV_CMD_SET_PORTENABLE,
    DRV_CMD_SET_JUMBO,
    DRV_CMD_SET_TXRX,
    DRV_CMD_SET_ABILITY,
    DRV_CMD_SET_AUTONEG,
       DRV_CMD_GET_AUTONEG,   
    DRV_CMD_SET_SPEED,
    DRV_CMD_SET_DUPLEX,
    DRV_CMD_SET_SPDLX ,
    DRV_CMD_GET_SPDLX_SET,
    DRV_CMD_SET_PHYREG,
    DRV_CMD_SET_PAUSE,
    DRV_CMD_GET_PORTMEDIUM,
    DRV_CMD_GET_CUR_LINK,
    DRV_CMD_GET_PORTABILITY,
    DRV_CMD_GET_PORTSPEED,
    DRV_CMD_GET_PORTDUPLEX,
    DRV_CMD_GET_PHYREG,
    DRV_CMD_GET_LINK_MASK ,
    DRV_CMD_GET_MEDIA_FIBER_MASK,
    DRV_CMD_GET_PORTMDIX,
    DRV_CMD_GET_PORTPRIO,
    DRV_CMD_GET_PORTFLOWCTRL,
    DRV_CMD_GET_PORTJUMBO,
    DRV_CMD_GET_PORTFRAMELEN,
	DRV_CMD_SET_PORTFRAMELEN,
    DRV_CMD_GET_FIBERPORTSPEED,
    DRV_CMD_SET_ETHDBG,   
    DRV_CMD_SET_AGGRMODE,
    DRV_CMD_SET_AGGRGROUP,
    DRV_CMD_GET_AGGRGROUP,
    DRV_CMD_SET_AGGR_GLOBAL_CFG_FLAG,
    DRV_CMD_GET_AGGR_GLOBAL_CFG_FLAG,   
    DRV_CMD_STP_INIT,
    DRV_CMD_STP_INSTANCE_CREATE,
    DRV_CMD_STP_INSTANCE_DESTORY,
    DRV_CMD_STP_INSTANCD_VLAN_SET,
    DRV_CMD_STP_SET_PORT_STATE,
    DRV_CMD_STP_GET_PORT_STATE,  
    DRV_CMD_SET_MANAGE_VLAN,
    /*Begin add by shipeng 2013-11-11*/
	DRV_CMD_SET_WIRELESS_UP_SERVICE_VLAN,
	/*End add by shipeng 2013-11-11*/
    DRV_CMD_GET_VLAN_NUM,
    DRV_CMD_SET_VLANMODE,
    DRV_CMD_SET_VLAN_ENTRY_CREATE,
    DRV_CMD_SET_VLAN_ENTRY_DELETE,
    DRV_CMD_SET_VLAN_MEMBER_REMOVE,
    DRV_CMD_SET_VLAN_MEMBER_ADD,
    DRV_CMD_SET_VLAN_PVID,
    DRV_CMD_SET_VLAN_PORT_VLAN_MEMBER,
    DRV_CMD_GET_VLAN_MEMBER, 
    DRV_CMD_SET_CPU_IN_VLAN,
    DRV_CMD_SET_CPU_OUT_VLAN,   
    DRV_CMD_SET_MAC_CPU_PORT_LEARM_ENABLE,
    DRV_CMD_SET_MAC_LEARN_ENABLE,
    DRV_CMD_GET_MAC_LEARN_ENABLE,
    DRV_CMD_SET_MAC_CPU_MAC_ADD,
    DRV_CMD_SET_MAC_FLUSH_UCAST_MAC,
    DRV_CMD_SET_MAC_ADD_UCAST_MAC,
    DRV_CMD_SET_MAC_DEL_UCAST_MAC,
    DRV_CMD_SET_MAC_ADD_MCAST_MAC,
    DRV_CMD_SET_MAC_DEL_MCAST_MAC,
    DRV_CMD_GET_MAC_AGE_TIME,
    DRV_CMD_SET_MAC_AGE_TIME,
    DRV_CMD_SET_MAC_LEARN_LIMIT,
    DRV_CMD_GET_MAC_UCAST_MAC,
    DRV_CMD_GET_MAC_MCAST_MAC,
    DRV_CMD_SET_MAC_SET_MCAST_MAC,
    DRV_CMD_SET_MAC_UCAST_LOOK_FAIL_FRD,
    DRV_CMD_GET_MAC_LEARN_COUNT,
    DRV_CMD_SET_MAC_REFRESH_UCAST,
    DRV_CMD_GET_MAC_DUMP_UCAST,
    DRV_CMD_GET_MAC_DUMP_MCAST,
    DRV_CMD_GET_MAC_DUMP_UCAST_SORTED,
    DRV_CMD_SET_MAC_TRAP_CPU,
    DRV_CMD_GET_MAC_CONFLICT,
    DRV_CMD_SET_RSV_MAC_TRAP,     
    DRV_CMD_GET_MAC_BYINDEX,
    DRV_CMD_GET_MAC_MORE_BYINDEX,
    DRV_CMD_GET_MAC_EXACT,
    DRV_CMD_GET_MCASTCOUNT,
    DRV_CMD_SET_MAC_DLF_MCAST_DROP, 	
	DRV_CMD_SET_IGMP_TRAP,   
    DRV_CMD_SET_PORT_ISOLATE_MASK,
    DRV_CMD_GET_ISOLATE_SET,   
    DRV_CMD_PORT_LED_INIT,
    DRV_CMD_SET_PORT_LED,   
    DRV_CMD_I2C_INIT,
    DRV_CMD_I2C_PORT_READ,   
    DRV_CMD_SET_MIRROR_GROUP,   
    DRV_CMD_GET_MIB_COUNT,
    DRV_CMD_SET_MIB_COUNT_SYNC,
    DRV_CMD_SET_MIB_COUNT_RESET,
    DRV_CMD_GET_PORT_COUNTER,
    DRV_CMD_GET_PORT_STATISTICS,  	
	DRV_CMD_GET_EPON_MIB_COUNTER,
	DRV_CMD_EPON_MIB_GLOBAL_RESET,
	DRV_CMD_GET_PONMAC_TRANSCEIVER,	
/*Begin add by huangmingjian 2014-01-13*/
#if defined(CONFIG_BOSA)	
	DRV_CMD_GET_PONFTOVERFLAG,
	DRV_CMD_SET_PONFTOVERFLAG,
	DRV_CMD_SET_LASER,
	DRV_CMD_GET_LASER,
	DRV_CMD_GET_APCSET,
	DRV_CMD_SET_APCSET,
	DRV_CMD_GET_IMODSET,
	DRV_CMD_SET_IMODSET,
#endif
/*End add by huangmingjian 2014-01-13*/	
    DRV_CMD_SET_RATELIMIT_INIT,
    DRV_CMD_SET_RATELIMIT_PORT,
    DRV_CMD_SET_RATELIMIT_STORM,   
    DRV_CMD_SET_QOS_INIT,
    DRV_CMD_SET_QOS_1P_REMAP,
    DRV_CMD_SET_QOS_2_QID,
    DRV_CMD_SET_QOS_DSCP_REMAP,
    DRV_CMD_SET_QOS_TRUST_MODE,
    DRV_CMD_SET_QOS_SCHEDULE,   
    DRV_CMD_REG_READ,
    DRV_CMD_REG_WRITE,
    DRV_CMD_MEM_READ,
    DRV_CMD_MEM_WRITE,   
    DRV_CMD_CPU_GET,
    DRV_CMD_CPU_SET,   
    DRV_CMD_SET_ACL_INIT,
    DRV_CMD_ADD_ACL_VLAN_INTF,
    DRV_CMD_DEL_ACL_VLAN_INTF,
    DRV_CMD_ADD_ACL_FILTER,
    DRV_CMD_DEL_ACL_FILTER,
    DRV_CMD_DEL_ACL_FILTER_BYPORT,
    DRV_CMD_ADD_CTC_CLF_BYPORT,    
    DRV_CMD_DEL_CTC_CLF_BYPREC,
    DRV_CMD_CLEAR_CTC_CLF_BYPORT,

	DRV_CMD_ADD_CTC_CLF_PRI_TO_QUEUE_BYPORT,    
    DRV_CMD_DEL_CTC_CLF_PRI_TO_QUEUE_BYPREC,
    DRV_CMD_CLEAR_CTC_CLF_PRI_TO_QUEUE_BYPORT,

    /*drv eee*/
    DRV_CMD_EEE_ENABLE,
    DRV_CMD_AUTO_PWR_DWN_ENABLE,    
    DRV_CMD_GET_CABLE_DIAG,   
	DRV_CMD_GET_STP_PORTSTATE,
	DRV_CMD_SET_STP_PORTSTATE,
    DRV_CMD_L2_SEND,
    DRV_CMD_SET_INTER_LOOPBACK, 
    DRV_CMD_GPIO_GET,
    DRV_CMD_GPIO_SET,
    DRV_CMD_GPIO_MULTIPLEX_GET,
    DRV_CMD_GPIO_MULTIPLEX_SET,
    DRV_CMD_GPIO_DIR_IN_SET,
    DRV_CMD_GPIO_DIR_OUT_SET,
    DRV_CMD_GPIO_MODE_LEVEL_SET,
    DRV_CMD_GPIO_MODE_EDGE_SET,
    DRV_CMD_GPIO_INT_LEVEL_HIGH_SET,
    DRV_CMD_GPIO_INT_LEVEL_LOW_SET,
    DRV_CMD_GPIO_INT_EDGE_SET,   
    DRV_CMD_GET_PON_STATUS_ERR_FLAG,
    DRV_CMD_SET_PON_STATUS_ERR_FLAG,
    DRV_CMD_GET_OAM_DISCOVERY_STATE,
	DRV_CMD_SET_OAM_DISCOVERY_STATE,
    DRV_CMD_GET_MAC_WRITE_FLAG,
	DRV_CMD_SET_MAC_WRITE_FLAG,
    DRV_CMD_GET_PON_INT_CHANGE_FLAG,
    DRV_CMD_SET_PON_INT_CHANGE_FLAG,
    DRV_CMD_OPL_INTREG_FIELD_READ,
    DRV_CMD_OPL_INTREG_FIELD_WRITE,
    DRV_CMD_GET_PON_MODE_OFF_FLAG,
    DRV_CMD_SET_PON_MODE_OFF_FLAG,   
    DRV_CMD_SET_SYS_LED,    
    DRV_CMD_CTC_PORT_BASE_VLAN_MEM_ADD_BY_ID,
    DRV_CMD_CTC_PORT_BASE_VLAN_MEM_DEL_BY_ID,
    DRV_CMD_CTC_PORT_BASE_VLAN_MEM_ADD,
    DRV_CMD_CTC_PORT_BASE_VLAN_MEM_DEL,
    DRV_CMD_CTC_PORT_BASE_VLAN_ENTY_CLR,
    DRV_CMD_CTC_MC_VLAN_ADD,   
    DRV_CMD_CTC_MC_MAC_GET,
    DRV_CMD_CTC_MC_MAC_SET,
    DRV_CMD_CTC_MC_MAC_DEL,
    DRV_CMD_CTC_MC_VLAN_MEM_RMV,
    DRV_CMD_CTC_MC_VLAN_MEM_GET,   
    DRV_CMD_CTC_ACL_FOR_TRANSP_VLAN_CREATE,
    DRV_CMD_CTC_ACL_FOR_TRANSP_VLAN_DELETE,
    DRV_CMD_CTC_ACL_FOR_TAG_VLAN_CREATE,
    DRV_CMD_CTC_ACL_FOR_TAG_VLAN_DELETE,
    DRV_CMD_CTC_ACL_FOR_TRANSL_VLAN_CREATE,
    DRV_CMD_CTC_ACL_FOR_TRANSL_VLAN_DELETE,   
    DRV_CMD_CTC_ACL_FOR_MC_VLAN_CREATE,
    DRV_CMD_CTC_ACL_FOR_MC_VLAN_DELETE,  
    DRV_CMD_CTC_PORT_INGRESS_VLAN_FILTER,
    DRV_CMD_CTC_PORT_INGRESS_VLAN_RECIEVE,
    DRV_CMD_CTC_PORT_EGRESS_VLAN_FILTER,
    DRV_CMD_CTC_GET_EMPTY_ACL_RULE_NUM,
    DRV_CMD_CTC_GET_PORT_VLAN_CFG,
    DRV_CMD_CTC_SET_PORT_VLAN_CFG,
    DRV_CMD_CTC_RESTART_AUTONEG,
    DRV_CMD_CTC_ENOUGH_VLAN_INDEX_CHECK,    
    DRV_CMD_IGMP_UNKNOW_IP4_ACT,   
    DRV_CMD_SINGLE_DBG_REG_GET,
    DRV_CMD_SINGLE_DBG_REG_SET,   
    DRV_CMD_FDB_ENTRY_GET,
    DRV_CMD_CVLAN_ENTRY_GET,
    DRV_CMD_SVLAN_ENTRY_GET,
    DRV_CMD_C2S_ENTRY_GET,
    DRV_CMD_S2C_ENTRY_GET,  
    DRV_CMD_PORT_EGRESS_MODE,   
    DRV_CMD_PORTS_INIT,      
    DRV_CMD_ACL_DROP_LOOPD_CREATE_ADD_PORT,
    DRV_CMD_ACL_DROP_LOOPD_CREATE_REMOVE_PORT,
    DRV_CMD_ACL_DROP_LOOPD_DELETE,   
    DRV_CMD_GET_PORTENABLE,   
    DRV_CMD_SET_LASERON_TEST_FLAG,
    DRV_CMD_GET_LASERON_TEST_FLAG,	
	DRV_CMD_SET_FECMODE,
	DRV_CMD_GET_FECMODE,	
	DRV_CMD_ACL_GET_EMPTY_ENTRY,
	DRV_CMD_ACL_DELETE_BY_ID,	
	DRV_CMD_ADDS2C_BY_PORT,
	DRV_CMD_DELS2C_BY_PORT,
	DRV_CMD_ADDC2S_BY_PORT,
	DRV_CMD_DELC2S_BY_PORT,
	DRV_CMD_ADDTRSLMC_MBR,
	DRV_CMD_DELTRSLMC_MBR,
	DRV_CMD_LOOKUP_MISS_FLOOD_SET,  /*Add by huangmingjian 2013-09-24 for Bug 234: flood-ports none for unkown multcast packet*/
    DRV_CMD_NUM
}DRV_CMD_E;

/*Begin add by huangmingjian 2013-08-27*/

#define RTK_EPON_MAX_QUEUE_PER_LLID 8
typedef struct rtk_epon_llidCounter_app_s
{
    unsigned int  queueTxFrames[RTK_EPON_MAX_QUEUE_PER_LLID];
    unsigned int  mpcpTxReport;
    unsigned int  mpcpRxGate;
    unsigned int  onuLlidNotBcst;
}rtk_epon_llidCounter_app_t;


typedef struct rtk_epon_counter_app_s
{
    rtk_epon_llidCounter_app_t  llidIdxCnt;
    unsigned char   llidIdx; /*indicate LLID relative counter is get from which LLID index*/
    unsigned int  mpcpRxDiscGate;
    unsigned int  fecCorrectedBlocks;    
    unsigned int  fecUncorrectedBlocks;
    unsigned int  fecCodingVio;
    unsigned int  notBcstBitNotOnuLlid;
    unsigned int  bcstBitPlusOnuLLid;
    unsigned int  bcstNotOnuLLid;
    unsigned int  crc8Err;
    unsigned int  mpcpTxRegRequest;
    unsigned int  mpcpTxRegAck;
}rtk_epon_counter_app_t;

#define TRANSCEIVER_LEN 24 
typedef struct rtk_transceiver_data_app_s{
    unsigned char buf[TRANSCEIVER_LEN];
}rtk_transceiver_data_app_t;

typedef enum rtk_transceiver_patameter_type_app_e{
    RTK_TRANSCEIVER_PARA_TYPE_VENDOR_NAME_ = 0,
    RTK_TRANSCEIVER_PARA_TYPE_VENDOR_PART_NUM_,
    RTK_TRANSCEIVER_PARA_TYPE_TEMPERATURE_,
    RTK_TRANSCEIVER_PARA_TYPE_VOLTAGE_,
    RTK_TRANSCEIVER_PARA_TYPE_TX_POWER_,
    RTK_TRANSCEIVER_PARA_TYPE_RX_POWER_,
    RTK_TRANSCEIVER_PARA_TYPE_MAX_
}rtk_transceiver_parameter_type_app_t;

/*End add by huangmingjian 2013-08-27*/


typedef struct tagDrvReq{
    DRV_CMD_E cmd;
    DRV_RET_E err;
    union {
        port_num_t lgcPort;
        UINT32     uiValue;
        STORM_CTLTYPE_E stormType;
        UCHAR *uiAddr;
    }para1_u;
    #define p_lport         para1_u.lgcPort
    #define p_group_num     para1_u.uiValue
    #define pmsg            para1_u.uiAddr
    union {        
        logic_pmask_t lgcMask;
        UINT32        uiValue;
        mac_address_t mac_address;
        mac_ucast_t stMacUcast;
        mac_mcast_t stMacMcast;
        mac_common_t stMacCommon;
        mac_delete_t stMacDelete;
        Mac_op_t stMacOp;
        MIB_IfStatType_t mibCountType;
        CounterName_E portCounterType;
        stat_reg_t portStatisticsType;
        mac_trap_set_t stTrapSet;
        qos_8021p_to_intpri_t st1pToIntPri;
        qos_dscp_to_intpri_t stDscpToIntPri;
        QueueMode_S stQueueSchedule;
        COMBO_PORT_CONFIG_S stComboCfg;
        ULONG ulValue;
        cable_diag_t stCableDiag;
		/*Begin add by huangmingjian 2013-08-27*/
		struct rtk_epon_counter_app_s *Counter;
		rtk_transceiver_data_app_t *pData;
		/*End add by huangmingjian 2013-08-27*/
        
        CTC_VLAN_CFG_S *pstCtcVlanCfg;
        
    }para2_u;    
    #define p_speed        para2_u.uiValue
    #define p_duplex       para2_u.uiValue
    #define p_spdlx        para2_u.uiValue
    #define p_ability      para2_u.uiValue
    #define p_jumbo        para2_u.uiValue
    #define p_pause_tx      para2_u.uiValue    
    #define p_mask         para2_u.lgcMask
    #define p_medium       para2_u.uiValue
    #define p_mdix         para2_u.uiValue
    #define p_prio          para2_u.uiValue
    #define p_cur_link         para2_u.uiValue
    #define p_enable       para2_u.uiValue
    #define p_flowctrl     para2_u.uiValue
    #define p_autoneg         para2_u.uiValue
    #define p_txrx         para2_u.uiValue
    #define aggr_mode     para2_u.uiValue
    #define vlan_mode     para2_u.uiValue
    #define p_ledstate    para2_u.uiValue
    #define p_frame_len  para2_u.uiValue
    #define p_msg_len      para2_u.uiValue
    union {
        logic_pmask_t lgcMask;  
        UINT32        uiValue;
        mac_address_t mac_address;
        Mac_op_t      stMacOp;
        UINT64        ui64Value;
        rsv_mac_pass_action_t uiAction;
        STORM_CTLRATE_S stStorm;
        l2_send_op    l2_op;
        UINT16        usValue; 
    }para3_u;  
    #define p_pause_rx      para3_u.uiValue
    #define p_porttype      para3_u.uiValue
    #define l2_op           para3_u.l2_op
    union {
        UINT32        uiValue;
    }para4_u;
    union {
        UINT32        uiValue;
		logic_pmask_t lgcMask;  
    }para5_u;     
}DRV_REQ_S;


/*Ioctl 部分基本公共函数*/
DRV_RET_E Drv_IoctlCmd(CHAR *pcDevName, DRV_REQ_S * drv_req );
DRV_RET_E Ioctl_SetSwitchU32Para(DRV_CMD_E cmd,INT32 paraNum,...);
DRV_RET_E Ioctl_GetSwitchU32Para(DRV_CMD_E cmd,UINT uiPort,UINT32 *pResult,UINT32 uiValue);
DRV_RET_E Ioctl_GetPortLinkMask(logic_pmask_t *pMask);
DRV_RET_E Ioctl_GetIsolateSet(logic_pmask_t *pMask); 
DRV_RET_E Ioctl_GetPortMediaFiberMask(logic_pmask_t *pMask);
DRV_RET_E Ioctl_GetPortMask(DRV_CMD_E ioctlCmd, UINT32 uiValue, logic_pmask_t *pMask);
DRV_RET_E Ioctl_GetUnionUint32(DRV_CMD_E ioctlCmd, UINT32 * puiValue);
DRV_RET_E Ioctl_SetUnionUint32(DRV_CMD_E ioctlCmd, UINT32 uiValue);
DRV_RET_E Ioctl_SetUnionUint32Uint32(DRV_CMD_E ioctlCmd, UINT32 uiValue, UINT32 uiValue2);
DRV_RET_E Ioctl_SetUnionUint32Uint32Uint32(DRV_CMD_E ioctlCmd, UINT32 uiValue, UINT32 uiValue2, UINT32 uiValue3);
DRV_RET_E Ioctl_GetUnionUint32ByUint32(DRV_CMD_E ioctlCmd, UINT32 uiValue, UINT32 *uiValue2);
DRV_RET_E Ioctl_SetUnionUint32Lpmask(DRV_CMD_E ioctlCmd, UINT32 uiValue, logic_pmask_t stLpMask);
DRV_RET_E Ioctl_GetUnionUint32MaskMask(DRV_CMD_E ioctlCmd, UINT32 uiValue, logic_pmask_t * pMask, logic_pmask_t * pMask2);
DRV_RET_E Ioctl_SetUnionUint32LpmaskLpmask(DRV_CMD_E ioctlCmd, UINT32 uiValue, logic_pmask_t stLpMask, logic_pmask_t stLpMask2);
DRV_RET_E Ioctl_SetUnionLportLpmaskLpmask(DRV_CMD_E ioctlCmd, port_num_t lport, logic_pmask_t stLpMask, logic_pmask_t stLpMask2);
DRV_RET_E Ioctl_SetUnionUint32MacAdd(DRV_CMD_E ioctlCmd, UINT32 uiValue, mac_address_t mac_address);
DRV_RET_E Ioctl_SetUnionMacDel(DRV_CMD_E ioctlCmd, mac_delete_t stMacDelete);
DRV_RET_E Ioctl_SetUnionMacUcast(DRV_CMD_E ioctlCmd, mac_ucast_t stMacUcast);
DRV_RET_E Ioctl_SetUnionMacMcast(DRV_CMD_E ioctlCmd, mac_mcast_t stMacMcast);
DRV_RET_E Ioctl_GetUnionMacVidUcast(DRV_CMD_E ioctlCmd, UINT32 ulVid, mac_address_t mac_addr, mac_ucast_t *pstMacUcast);
DRV_RET_E Ioctl_GetUnionMacVidUcast(DRV_CMD_E ioctlCmd, UINT32 ulVid, mac_address_t mac_addr, mac_ucast_t *pstMacMcast);
DRV_RET_E Ioctl_GetUnionMacVidMcast(DRV_CMD_E ioctlCmd, UINT32 ulVid, mac_address_t mac_addr, mac_mcast_t *pstMacMcast);
DRV_RET_E Ioctl_GetConflictMac(DRV_CMD_E ioctlCmd, Mac_op_t stMacOp, Mac_op_t *pstMacOp);
DRV_RET_E Ioctl_GetValidMacByIndex(DRV_CMD_E ioctlCmd, UINT32 ulStartIdx,  mac_common_t *pstTmpMac);
DRV_RET_E Ioctl_GetUnionMacMoreByIndex(DRV_CMD_E ioctlCmd, UINT32 ulIdx, UINT32 uiCount, ULONG ulPointer, UINT32 *puiRealCount, UINT32 *puiNextIdx);
DRV_RET_E Ioctl_GetUnionMacVidMacCommon(DRV_CMD_E ioctlCmd, UINT32 ulVid, mac_address_t mac_addr, mac_common_t *pstTmpMac);
DRV_RET_E Ioctl_GetUnionUint32ByUlong(DRV_CMD_E ioctlCmd, ULONG ulValue, UINT32 *uiValue);
DRV_RET_E Ioctl_GetUnionUint32Uint32Uint32ByUlong(DRV_CMD_E ioctlCmd, port_num_t lport, ULONG ulValue, UINT32 uiValue1, UINT32 uiValue2);
DRV_RET_E Ioctl_GetMIbCountByPort(DRV_CMD_E ioctlCmd, port_num_t lport, MIB_IfStatType_t mibCountType, UINT64 *ui64Value);
DRV_RET_E Ioctl_GetPortCounter(DRV_CMD_E ioctlCmd, port_num_t lport, CounterName_E portCountType, UINT64 *ui64Value);
DRV_RET_E Ioctl_GetPortStatistics(DRV_CMD_E ioctlCmd, port_num_t lport, stat_reg_t portStatisticsType, UINT64 *ui64Value);
DRV_RET_E Ioctl_SetPortRatelimit(DRV_CMD_E ioctlCmd, UINT32 direction, logic_pmask_t stLpMask, UINT32 uiRate);
DRV_RET_E Ioctl_SetTrapCpu(DRV_CMD_E ioctlCmd, port_num_t lport, mac_trap_set_t stTrapSet, UINT32 priority);
DRV_RET_E Ioctl_SetRsvMacTrap(DRV_CMD_E ioctlCmd, UINT32 uiIndex, UINT32 priority, rsv_mac_pass_action_t action);
DRV_RET_E Ioctl_SetQos1pRemapCtl(DRV_CMD_E ioctlCmd, qos_8021p_to_intpri_t st1pToIntPri);
DRV_RET_E Ioctl_SetQosDscpRemapCtl(DRV_CMD_E ioctlCmd, qos_dscp_to_intpri_t stDscpToIntPri);
DRV_RET_E Ioctl_SetQosQueueSchedulCtl(DRV_CMD_E ioctlCmd, QueueMode_S stQueueSchedule);
DRV_RET_E Ioctl_InitCmboPortConfig(DRV_CMD_E ioctlCmd, port_num_t lport, COMBO_PORT_CONFIG_S * pstCfg);
DRV_RET_E Ioctl_GetCableDiag(DRV_CMD_E ioctlCmd, port_num_t lport, cable_diag_t *pstCableDiag);
DRV_RET_E Ioctl_GetReg(DRV_CMD_E ioctlCmd, UINT32 uiValue, UINT32 uiValue2, UINT32 * uiValue3);
DRV_RET_E Ioctl_SetReg(DRV_CMD_E ioctlCmd, UINT32 uiValue, UINT32 uiValue2, UINT32 uiValue3);
DRV_RET_E Ioctl_SetPortStormlimit(DRV_CMD_E ioctlCmd, STORM_CTLTYPE_E type, logic_pmask_t *pLgcPMask, STORM_CTLRATE_S *pstStorm);
DRV_RET_E Ioctl_AddAclFilter(UINT32 lport, 
                             UINT32 uiRulePrecedence,
                             UINT32 uiAclRuleType,
                             UINT32 uiAclRuleDir,
                             VOID *pRuleValue);
DRV_RET_E Ioctl_DelAclFilterByPort(UINT32 lport, UINT32 uiRulePrecedence, UINT32 uiAclRuleDir);
DRV_RET_E Ioctl_AddCtcClfByPort(UINT32 lport, 
                             UINT32 uiRulePrecedence,
                             UINT32 uiAclRuleType,
                             VOID *pRuleValue,
                             UINT32 uiRemarkPri);
DRV_RET_E Ioctl_DelCtcClfByPrec(UINT32 lport, UINT32 uiRulePrecedence);
DRV_RET_E Ioctl_ClearCtcClfByPort(UINT32 lport);

/*begin added by liaohongjun 2012/11/30 of EPN104QID0084*/
DRV_RET_E Ioctl_AclDropLoopdCreateAddPort(UINT32 lport);
DRV_RET_E Ioctl_AclDropLoopdCreateRemovePort(UINT32 lport);
DRV_RET_E Ioctl_AclDropLoopdDelete(VOID);
/*end added by liaohongjun 2012/11/30 of EPN104QID0084*/

/*Begin add by huangmingjian 2013-09-24 for Bug 234: flood-ports none for unkown multcast packet*/
DRV_RET_E Ioctl_SetLookupMissFloodPortMask(DRV_CMD_E ioctlCmd, UINT32 type, logic_pmask_t flood_portmask);
/*End add by huangmingjian 2013-09-24 for Bug 234: flood-ports none for unkown multcast packet*/

/*驱动中需要用到的ioctl函数*/
#define Ioctl_SetPortMedium( _lport, _uiMedium)\
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_MEDIUM,2,_lport,_uiMedium)

#define Ioctl_SetPortPriority(_lport, _uiPrio) \
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_PRIORITY,2,_lport,_uiPrio)
    
#define Ioctl_SetPortMdix( _lport, _uiMdi)\
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_MDIX,2,_lport,_uiMdi)
    
#define Ioctl_SetPortEnable( _lport, _uiState)\
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_PORTENABLE,2,_lport,_uiState)
    
#define Ioctl_SetPortJumbo( _lport, _uiJumbo)\
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_JUMBO,2,_lport,_uiJumbo)
    
#define Ioctl_SetPortTxRx( _lport, _uiState)\
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_TXRX,2,_lport,_uiState)
    
#define Ioctl_SetPortAbility( _lport, _ability, _porttype)\
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_ABILITY,3,_lport,_ability, _porttype)
    
#define Ioctl_SetPortAutoneg( _lport, _uiState, _porttype)\
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_AUTONEG,3,_lport,_uiState, _porttype)

/*Begin add by shipeng 2013/05/27 for CTC*/ 
#define Ioctl_GetPortAutoneg( _lport, _puiAuto)\
    Ioctl_GetSwitchU32Para(DRV_CMD_GET_AUTONEG,_lport,(UINT32 *)(_puiAuto), 0)
/*End add by shipeng 2013/05/27 for CTC*/ 
    
#define Ioctl_SetPortSpeed( _lport, _uiSpeed, _porttype)\
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_SPEED,3,_lport,_uiSpeed, _porttype)
    
#define Ioctl_SetPortDuplex( _lport, _uiDuplex, _porttype)\
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_DUPLEX,3,_lport, _uiDuplex, _porttype)


#define Ioctl_SetPortSpdlx( _lport, _uiSpdlx)\
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_SPDLX,2,_lport, _uiSpdlx)

    
#define Ioctl_SetPortPause( _lport, _uiPauseTx, _uiPauseRx)\
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_PAUSE,3,_lport,_uiPauseTx,_uiPauseRx)

#define Ioctl_ComboPortFiberCfgInit(_lport, _pstCfg)\
    Ioctl_InitCmboPortConfig(DRV_CMD_FIBER_CFG_INIT, _lport, _pstCfg)
    
#define Ioctl_ComboPortCopperCfgInit(_lport, _pstCfg)\
    Ioctl_InitCmboPortConfig(DRV_CMD_COPPER_CFG_INIT, _lport, _pstCfg)
#define Ioctl_SetPortInternalLoopback(_lport,__enable) \
		Ioctl_SetSwitchU32Para(DRV_CMD_SET_INTER_LOOPBACK,2,_lport,__enable)


#define Ioctl_SetPhyReg( _lport, _reg_no, _reg_value)\
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_PHYREG,3,_lport, _reg_no, _reg_value)
#define Ioctl_SetPortFrameLen(_lport, _puiLength)\
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_PORTFRAMELEN,2, _lport, _puiLength)

/***************************************************************************/
/***下面get函数由于为兼容32位和64位系统,这里将指针统一转换为64位使用*/
/***************************************************************************/
    
#define Ioctl_GetPortMedium( _lport, _pMedium)\
    Ioctl_GetSwitchU32Para(DRV_CMD_GET_PORTMEDIUM,_lport,(UINT32 *)(_pMedium), 0)
/*begin added by liaohongjun 2012/12/10 of EPN104QID0086*/    
#define Ioctl_GetPortEnable( _lport, _uiState)\
    Ioctl_GetSwitchU32Para(DRV_CMD_GET_PORTENABLE,_lport,(UINT32 *)_uiState,0) 
/*end added by liaohongjun 2012/12/10 of EPN104QID0086*/    
#define Ioctl_GetPortCurrentLink( _lport, _pLink)\
    Ioctl_GetSwitchU32Para(DRV_CMD_GET_CUR_LINK,_lport,(UINT32 *)(_pLink), 0)

#define Ioctl_GetPortAbility( _lport, _pAbility, _porttype)\
    Ioctl_GetSwitchU32Para(DRV_CMD_GET_PORTABILITY,_lport,(UINT32 *)(_pAbility), _porttype)
    
#define Ioctl_GetPortSpeed( _lport, _puiSpeed)\
    Ioctl_GetSwitchU32Para(DRV_CMD_GET_PORTSPEED,_lport,(UINT32 *)(_puiSpeed), 0)
    
#define Ioctl_GetPortDuplex( _lport, _puiDuplex)\
    Ioctl_GetSwitchU32Para(DRV_CMD_GET_PORTDUPLEX,_lport,(UINT32 *)(_puiDuplex), 0)

#define Ioctl_GetPortMdix( _lport, _puiMdix)\
    Ioctl_GetSwitchU32Para(DRV_CMD_GET_PORTMDIX,_lport,(UINT32 *)(_puiMdix), 0)

#define Ioctl_GetPortPriority( _lport, _puiPrio)\
    Ioctl_GetSwitchU32Para(DRV_CMD_GET_PORTPRIO,_lport,(UINT32 *)(_puiPrio), 0)

#define Ioctl_GetPortFlowctrl( _lport, _puiFc)\
    Ioctl_GetSwitchU32Para(DRV_CMD_GET_PORTFLOWCTRL,_lport,(UINT32 *)(_puiFc), 0)

#define Ioctl_GetPortJumbo( _lport, _puiJumbo)\
    Ioctl_GetSwitchU32Para(DRV_CMD_GET_PORTJUMBO,_lport,(UINT32 *)(_puiJumbo), 0)
#define Ioctl_GetPortFrameLen( _lport, _puiLength)\
    Ioctl_GetSwitchU32Para(DRV_CMD_GET_PORTFRAMELEN,_lport,(UINT32 *)(_puiLength), 0)
#define Ioctl_GetFiberPortSpeed( _lport, _puiSpeed)\
    Ioctl_GetSwitchU32Para(DRV_CMD_GET_FIBERPORTSPEED,_lport,(UINT32 *)(_puiSpeed), 0)


#define Ioctl_GetPhyReg( _lport, _pVal, _reg_no)\
    Ioctl_GetSwitchU32Para(DRV_CMD_GET_PHYREG,_lport,(UINT32 *)(_pVal), _reg_no)

#define Ioctl_GetSpdlxSet( _lport, _pVal)\
    Ioctl_GetUnionUint32ByUint32(DRV_CMD_GET_SPDLX_SET, _lport,(UINT32 *)(_pVal))


/***************************************************************************/
/***drv_aggr ioctl*/
/***************************************************************************/
#define Ioctl_SetAggreMode(_aggr_mode) \
    Ioctl_SetUnionUint32(DRV_CMD_SET_AGGRMODE, (UINT32)_aggr_mode)
    
#define Ioctl_SetAggreGroup(_group_number, _lport_mask) \
    Ioctl_SetUnionUint32Lpmask(DRV_CMD_SET_AGGRGROUP, (UINT32)_group_number, _lport_mask)

#define Ioctl_GetAggreGroup(_group_number, _plport_mask) \
    Ioctl_GetPortMask(DRV_CMD_GET_AGGRGROUP, (UINT32)_group_number, (logic_pmask_t *)_plport_mask)

#define Ioctl_SetAggreGlobalCfgFlag(_g_cfg_flag) \
    Ioctl_SetUnionUint32(DRV_CMD_SET_AGGR_GLOBAL_CFG_FLAG, (UINT32)_g_cfg_flag)

#define Ioctl_GetAggreGlobalCfgFlag(_gp_cfg_flag)\
    Ioctl_GetUnionUint32ByUint32(DRV_CMD_GET_AGGR_GLOBAL_CFG_FLAG, 0, (UINT32 *)_gp_cfg_flag)

/***************************************************************************/
/***drv_stp ioctl*/
/***************************************************************************/
#define Ioctl_StpInit() \
    Ioctl_SetUnionUint32(DRV_CMD_STP_INIT, (UINT32)0)

#define Ioctl_StpInstanceCreate(_stp_inst) \
    Ioctl_SetUnionUint32(DRV_CMD_STP_INSTANCE_CREATE, (UINT32)_stp_inst)

#define Ioctl_StpInstanceDestory(_stp_inst) \
    Ioctl_SetUnionUint32(DRV_CMD_STP_INSTANCE_DESTORY, (UINT32)_stp_inst)

#define Ioctl_StpInstanceVlanSet(_stp_inst, _vlan_id) \
    Ioctl_SetUnionUint32Uint32(DRV_CMD_STP_INSTANCD_VLAN_SET, (UINT32)_stp_inst, (UINT32)_vlan_id)

#define Ioctl_SetStpPortState(_stp_inst, _port_number, _stp_state) \
    Ioctl_SetSwitchU32Para(DRV_CMD_STP_SET_PORT_STATE,3,_stp_inst,_port_number,_stp_state)
    
#define Ioctl_GetStpPortState(_stp_inst, _port_number, _p_stp_state)\
    Ioctl_GetSwitchU32Para(DRV_CMD_STP_GET_PORT_STATE, _stp_inst, (UINT32 *)(_p_stp_state), _port_number)
    
/***************************************************************************/
/***drv_vlan ioctl*/
/***************************************************************************/
#define Ioctl_GetVlanExistNum(_puiNum)\
    Ioctl_GetUnionUint32ByUint32(DRV_CMD_GET_VLAN_NUM, 0, (UINT32 *)_puiNum)

#define Ioctl_SetVlanMode(_vlan_mode)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_VLANMODE, (UINT32)_vlan_mode)
    
#define Ioctl_SetVlanEntryCreate(_vlan_id)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_VLAN_ENTRY_CREATE, (UINT32)_vlan_id)
    
#define Ioctl_SetVlanEntryDelete(_vlan_id)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_VLAN_ENTRY_DELETE, (UINT32)_vlan_id)
    
#define Ioctl_SetVlanMemberRemove(_vlan_id, _lport_mask)\
    Ioctl_SetUnionUint32Lpmask(DRV_CMD_SET_VLAN_MEMBER_REMOVE, (UINT32)_vlan_id, _lport_mask)
    
#define Ioctl_SetVlanMemberAdd(_vlan_id, _lport_mask, _lport_mask_untag)\
    Ioctl_SetUnionUint32LpmaskLpmask(DRV_CMD_SET_VLAN_MEMBER_ADD, (UINT32)_vlan_id,_lport_mask,_lport_mask_untag)

#define Ioctl_SetVlanPvid(_port_number, _pvid)\
    Ioctl_SetUnionUint32Uint32(DRV_CMD_SET_VLAN_PVID, (UINT32)_port_number, (UINT32)_pvid)

#define Ioctl_SetVlanPortVlanMember(_port_number, _lport_mask)\
    Ioctl_SetUnionUint32Lpmask(DRV_CMD_SET_VLAN_PORT_VLAN_MEMBER, (UINT32)_port_number, _lport_mask)

#define Ioctl_GetVlanMember(_vlan_id, _lport_mask, _lport_mask_untag)\
    Ioctl_GetUnionUint32MaskMask(DRV_CMD_GET_VLAN_MEMBER, (UINT32)_vlan_id,(logic_pmask_t *)_lport_mask,(logic_pmask_t *)_lport_mask_untag)


#define Ioctl_SetEthDbg(_eth_dbg_mode)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_ETHDBG, (UINT32)_eth_dbg_mode)


/***************************************************************************/
/***drv_mac ioctl*/
/***************************************************************************/
#define Ioctl_SetMacCpuPortMacLearnEnable(_enable)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_MAC_CPU_PORT_LEARM_ENABLE, (UINT32)_enable)
#define Ioctl_SetMacLearnEnable(_lport_mask, _enable)\
    Ioctl_SetUnionUint32Lpmask(DRV_CMD_SET_MAC_LEARN_ENABLE, (UINT32)_enable, _lport_mask)
#define Ioctl_GetMacLearnEnable(_lgcPort, _PEnable)\
    Ioctl_GetUnionUint32ByUint32(DRV_CMD_GET_MAC_LEARN_ENABLE, _lgcPort, (UINT32 *)_PEnable)
#define Ioctl_SetMacCpuMacAdd(_vid, _mac_address)\
    Ioctl_SetUnionUint32MacAdd(DRV_CMD_SET_MAC_CPU_MAC_ADD, (UINT32)_vid, _mac_address)
#define Ioctl_SetMacFlushUcastMac(_stMacDel)\
    Ioctl_SetUnionMacDel(DRV_CMD_SET_MAC_FLUSH_UCAST_MAC, (mac_delete_t)_stMacDel)
#define Ioctl_SetMacAddUcastMac(_mac_ucast)\
    Ioctl_SetUnionMacUcast(DRV_CMD_SET_MAC_ADD_UCAST_MAC, (mac_ucast_t)_mac_ucast)
#define Ioctl_SetMacDelUcastMac(_mac_ucast)\
    Ioctl_SetUnionMacUcast(DRV_CMD_SET_MAC_DEL_UCAST_MAC, (mac_ucast_t)_mac_ucast)
#define Ioctl_SetMacAddMcastMac(_mac_mcast)\
    Ioctl_SetUnionMacMcast(DRV_CMD_SET_MAC_ADD_MCAST_MAC, (mac_mcast_t)_mac_mcast)
#define Ioctl_SetMacDelMcastMac(_mac_mcast)\
    Ioctl_SetUnionMacMcast(DRV_CMD_SET_MAC_DEL_MCAST_MAC, (mac_mcast_t)_mac_mcast)
/*Begin add by huangmingjian 2013-09-07 for mac aging time*/
#define Ioctl_GetMacAgeTime(pAge_time)\
    Ioctl_GetUnionUint32(DRV_CMD_GET_MAC_AGE_TIME, (UINT32 *)pAge_time)
/*End add by huangmingjian 2013-09-07 for mac aging time*/
#define Ioctl_SetMacAgeTime(_age_time)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_MAC_AGE_TIME, (UINT32)_age_time)
#define Ioctl_SetMacLearnLimit(_lgcPort, _learn_limit, _dis_forward)\
    Ioctl_SetUnionUint32Uint32Uint32(DRV_CMD_SET_MAC_LEARN_LIMIT, (UINT32)_lgcPort, (UINT32)_learn_limit, (UINT32)_dis_forward)
#define Ioctl_GetMacUcastMac(_vid, _mac_address, _pmac_ucast)\
    Ioctl_GetUnionMacVidUcast(DRV_CMD_GET_MAC_UCAST_MAC, (UINT32)_vid, _mac_address, (mac_ucast_t *)_pmac_ucast)
#define Ioctl_GetMacMcastMac(_vid, _mac_address, _pmac_mcast)\
    Ioctl_GetUnionMacVidMcast(DRV_CMD_GET_MAC_MCAST_MAC, (UINT32)_vid, _mac_address, (mac_mcast_t *)_pmac_mcast)
#define Ioctl_SetMacSetMcastMac(_mac_mcast)\
    Ioctl_SetUnionMacMcast(DRV_CMD_SET_MAC_SET_MCAST_MAC, (mac_mcast_t)_mac_mcast)
#define Ioctl_SetMacUcastLookFailFrd(_lport_mask)\
    Ioctl_SetUnionUint32Lpmask(DRV_CMD_SET_MAC_UCAST_LOOK_FAIL_FRD, (UINT32)0, _lport_mask)
#define Ioctl_GetMacLearnCountByPort(_lgcPort, _pulCount)\
    Ioctl_GetUnionUint32ByUint32(DRV_CMD_GET_MAC_LEARN_COUNT, _lgcPort, (UINT32 *)_pulCount)
#define Ioctl_SetMacRefreshUcast(_terval)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_MAC_REFRESH_UCAST, (UINT32)_terval)
#define Ioctl_GetMacDumpUcast(_ulPointer, puiCount)\
    Ioctl_GetUnionUint32ByUlong(DRV_CMD_GET_MAC_DUMP_UCAST, (ULONG)_ulPointer, (UINT32 *)puiCount)
#define Ioctl_GetMacDumpMcast(_ulPointer, puiCount)\
    Ioctl_GetUnionUint32ByUlong(DRV_CMD_GET_MAC_DUMP_MCAST, (ULONG)_ulPointer, (UINT32 *)puiCount)
#define Ioctl_GetMacDumpUcastSorted(_ulPointer, puiCount)\
    Ioctl_GetUnionUint32ByUlong(DRV_CMD_GET_MAC_DUMP_UCAST_SORTED, (ULONG)_ulPointer, (UINT32 *)puiCount)
#define Ioctl_SetTrapToCpu(_lport, _trap_set, _priority)\
    Ioctl_SetTrapCpu(DRV_CMD_SET_MAC_TRAP_CPU, (port_num_t)_lport, (mac_trap_set_t)_trap_set, (UINT32)_priority)
#define Ioctl_GetMacConflict(_stMacOp, _pstMacOp)\
    Ioctl_GetConflictMac(DRV_CMD_GET_MAC_CONFLICT, (Mac_op_t)_stMacOp, (Mac_op_t *)_pstMacOp)
#define Ioctl_SetReservedMacTrap(_uiIndex, _uiPri, _action)\
    Ioctl_SetRsvMacTrap(DRV_CMD_SET_RSV_MAC_TRAP, (_uiIndex), (_uiPri), (_action))
#define Ioctl_GetMacByIdx(_ulStartIdx, _pCommonMac)\
    Ioctl_GetValidMacByIndex(DRV_CMD_GET_MAC_BYINDEX, _ulStartIdx, (mac_common_t *)_pCommonMac)
#define Ioctl_GetMacMoreByIdx(_ulStartIdx, _ulGetCount, _ulPointer, puiCount, puiNextIdx)\
    Ioctl_GetUnionMacMoreByIndex(DRV_CMD_GET_MAC_MORE_BYINDEX, _ulStartIdx, _ulGetCount, (ULONG)_ulPointer, (UINT32 *)puiCount, (UINT32 *)puiNextIdx)
#define Ioctl_GetMacExact(_vid, _mac_address, _pCommonMac)\
    Ioctl_GetUnionMacVidMacCommon(DRV_CMD_GET_MAC_EXACT, (UINT32)_vid, _mac_address, (mac_common_t *)_pCommonMac)
#define Ioctl_GetMcastCount(pulCnt)\
    Ioctl_GetUnionUint32(DRV_CMD_GET_MCASTCOUNT, (UINT32 *)pulCnt)
#define Ioctl_SetMcastLookupMissDrop(_enable)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_MAC_DLF_MCAST_DROP, (UINT32)_enable)
/***************************************************************************/
/***drv_igsp ioctl*/
/***************************************************************************/
#define Ioctl_SetIgmpPktAction(_action)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_IGMP_TRAP, (UINT32)_action)

/***************************************************************************/
/***drv_isolate ioctl*/
/***************************************************************************/
#define Ioctl_SetPortIsolate(_lport_mask)\
    Ioctl_SetUnionUint32Lpmask(DRV_CMD_SET_PORT_ISOLATE_MASK, (UINT32)0, _lport_mask)

/***************************************************************************/
/***drv_led ioctl*/
/***************************************************************************/
#define Ioctl_PortLedInit()\
    Ioctl_SetUnionUint32(DRV_CMD_PORT_LED_INIT, (UINT32)0)
    
#define Ioctl_SetPortLed(_lport, _linkstate, _porttype)\
    Ioctl_SetSwitchU32Para(DRV_CMD_SET_PORT_LED,3,_lport,_linkstate,_porttype)

/***************************************************************************/
/***drv_fiber ioctl*/
/***************************************************************************/
#define Ioctl_I2cInit()\
    Ioctl_SetUnionUint32(DRV_CMD_I2C_INIT, (UINT32)0)
#define Ioctl_I2cPortRead(_lport, _uiAddr, _ulPointer, _uiLen)\
    Ioctl_GetUnionUint32Uint32Uint32ByUlong(DRV_CMD_I2C_PORT_READ, (UINT32 )_lport, (ULONG)_ulPointer, (UINT32 )_uiAddr, (UINT32 )_uiLen)

/***************************************************************************/
/***drv_mirror ioctl*/
/***************************************************************************/
#define Ioctl_SetMirrorGroup(_mirror_dest_port, _ingress_lport_mask, _egress_lport_mask)\
    Ioctl_SetUnionLportLpmaskLpmask(DRV_CMD_SET_MIRROR_GROUP, _mirror_dest_port, _ingress_lport_mask, _egress_lport_mask)

/***************************************************************************/
/***drv_statistics ioctl*/
/***************************************************************************/
#define Ioctl_GetMibCountByPort(_lport, _type, _pui64Value)\
    Ioctl_GetMIbCountByPort(DRV_CMD_GET_MIB_COUNT, (port_num_t)_lport, (MIB_IfStatType_t)_type, (UINT64 *)_pui64Value)
#define Ioctl_SetMibCountSync()\
    Ioctl_SetUnionUint32(DRV_CMD_SET_MIB_COUNT_SYNC, 0)
#define Ioctl_SetMibCountReset(_lport)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_MIB_COUNT_RESET, (UINT32)_lport)
#define Ioctl_GetPortCounterByPort(_lport, _type, _pui64Value)\
    Ioctl_GetPortCounter(DRV_CMD_GET_PORT_COUNTER, (port_num_t)_lport, (CounterName_E)_type, (UINT64 *)_pui64Value)
#define Ioctl_GetPortStatisticsByPort(_lport, _type, _pui64Value)\
    Ioctl_GetPortStatistics(DRV_CMD_GET_PORT_STATISTICS, (port_num_t)_lport, (stat_reg_t)_type, (UINT64 *)_pui64Value)
	/*Begin add by huangmingjian 2013-08-27*/
#define Ioctl_Get_Epon_Mib_Counter(Counter)\
	Ioctl_GetEponMibCounter(DRV_CMD_GET_EPON_MIB_COUNTER,(rtk_epon_counter_app_t *)Counter)
#define Ioctl_EponMibGlobalReset()\
	Ioctl_Epon_Mib_Global_Reset(DRV_CMD_EPON_MIB_GLOBAL_RESET)
#define Ioctl_GetPonmacTransceiver(para_type, pData)\
	Ioctl_Get_Ponmac_Transceiver(DRV_CMD_GET_PONMAC_TRANSCEIVER, para_type, pData)
	/*End add by huangmingjian 2013-08-27*/
	/*Begin add by huangmingjian 2014-01-13*/
#if defined(CONFIG_BOSA)
#define Ioctl_GetPonftoverflag(FTOverFLAG)\
	Ioctl_Get_Ponftoverflag(DRV_CMD_GET_PONFTOVERFLAG, (UINT32*)FTOverFLAG)
#define Ioctl_SetPonftoverflag(FTOverFLAG)\
	Ioctl_Set_Ponftoverflag(DRV_CMD_SET_PONFTOVERFLAG, (UINT32)FTOverFLAG)
#define Ioctl_SetLaser(uiAction)\
	Ioctl_Set_Laser(DRV_CMD_SET_LASER, (UINT32)uiAction)
#define Ioctl_GetLaser(uiAction)\
	Ioctl_Get_Laser(DRV_CMD_GET_LASER, (UINT32 *)uiAction)	
#define Ioctl_GetApcset(pApcVal)\
	Ioctl_Get_Apcset(DRV_CMD_GET_APCSET, (UINT32 *)pApcVal)	
#define Ioctl_SetApcset(uiApcVal)\
	Ioctl_Set_Apcset(DRV_CMD_SET_APCSET, (UINT32)uiApcVal)	
#define Ioctl_GetImodset(pModVal)\
	Ioctl_Get_Imodset(DRV_CMD_GET_IMODSET, (UINT32 *)pModVal)
#define Ioctl_SetImodset(uiModVal)\
	Ioctl_Set_Imodset(DRV_CMD_SET_IMODSET, (UINT32)uiModVal)
#endif
	/*End add by huangmingjian 2014-01-13*/

/***************************************************************************/
/***drv_ratelimit ioctl*/
/***************************************************************************/
#define Ioctl_SetRateLimitInit()\
    Ioctl_SetUnionUint32(DRV_CMD_SET_RATELIMIT_INIT, 0)
#define Ioctl_SetRateLimitPort(_direction, _lPortMask, _ulKBps)\
    Ioctl_SetPortRatelimit(DRV_CMD_SET_RATELIMIT_PORT, _direction, _lPortMask, _ulKBps)
#define Ioctl_SetStormCtrlPort(_type, _lPortMask, _pstorm)\
    Ioctl_SetPortStormlimit(DRV_CMD_SET_RATELIMIT_STORM, _type, _lPortMask, _pstorm)

/***************************************************************************/
/***drv_qos ioctl*/
/***************************************************************************/
#define Ioctl_SetQosInit()\
    Ioctl_SetUnionUint32(DRV_CMD_SET_QOS_INIT, 0)
#define Ioctl_SetQos1pRemap(_st1pToIntPri)\
    Ioctl_SetQos1pRemapCtl(DRV_CMD_SET_QOS_1P_REMAP, _st1pToIntPri)
#define Ioctl_SetQosDscpRemap(_stDscpToIntPri)\
    Ioctl_SetQosDscpRemapCtl(DRV_CMD_SET_QOS_DSCP_REMAP, _stDscpToIntPri)
#define Ioctl_SetQosTrustMode(_mode)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_QOS_TRUST_MODE, _mode)
#define Ioctl_SetQosQueueSchedul(_stQueueSchedule)\
    Ioctl_SetQosQueueSchedulCtl(DRV_CMD_SET_QOS_SCHEDULE, _stQueueSchedule)
#define  Ioctl_SetQos2Qid(_st1pToIntPri)\
	Ioctl_SetQos1pRemapCtl(DRV_CMD_SET_QOS_2_QID, _st1pToIntPri)

/***************************************************************************/
/***register ioctl*/
/***************************************************************************/
#define Ioctl_ReadReg(_chip, _regIndex, _pulData)\
    Ioctl_GetReg(DRV_CMD_REG_READ, _chip, _regIndex, (UINT32*)_pulData)
#define Ioctl_WriteReg(_chip, _regIndex, _pulData)\
    Ioctl_SetReg(DRV_CMD_REG_WRITE, _chip, _regIndex, _pulData)
/***************************************************************************/
/***CPU address ioctl*/
/***************************************************************************/
#define Ioctl_GetCpu(_cpuaddr, _pulData)\
    Ioctl_GetUnionUint32ByUint32(DRV_CMD_CPU_GET, _cpuaddr, (UINT32*)_pulData)
#define Ioctl_SetCpu(_cpuaddr, _pulData)\
    Ioctl_SetUnionUint32Uint32(DRV_CMD_CPU_SET, _cpuaddr, _pulData)

/***************************************************************************/
/***drv_mvlan ioctl*/
/***************************************************************************/
#define Ioctl_AddCpuToVlanMember(_vlan_id)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_CPU_IN_VLAN, (UINT32)(_vlan_id))
#define Ioctl_RemoveCpuFromVlanMember(_vlan_id)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_CPU_OUT_VLAN, (UINT32)(_vlan_id))

/***************************************************************************/
/***drv_acl ioctl*/
/***************************************************************************/
#define Ioctl_SetAclInit()\
    Ioctl_SetUnionUint32(DRV_CMD_SET_ACL_INIT, 0)
#define Ioctl_AddVlanIntf(_vid, _mac_addr)\
    Ioctl_SetUnionUint32MacAdd(DRV_CMD_ADD_ACL_VLAN_INTF, (UINT32)_vid, _mac_addr)
#define Ioctl_DelVlanIntf(_vid)\
    Ioctl_SetUnionUint32(DRV_CMD_DEL_ACL_VLAN_INTF, (UINT32)_vid)

/***************************************************************************/
/***drv_eee ioctl*/
/***************************************************************************/
#define Ioctl_set_eee_enable(_portMask, _enable)\
    Ioctl_SetUnionUint32Lpmask(DRV_CMD_EEE_ENABLE, (UINT32)_enable, _portMask)
#define Ioctl_set_auto_pwr_dwn_enable(_portMask, _enable)\
    Ioctl_SetUnionUint32Lpmask(DRV_CMD_AUTO_PWR_DWN_ENABLE, (UINT32)_enable, _portMask)

/***************************************************************************/
/***drv cable diag ioctl*/
/***************************************************************************/
#define Ioctl_get_cable_diag(_lport, _pstCableDiag)\
    Ioctl_GetCableDiag(DRV_CMD_GET_CABLE_DIAG, _lport, _pstCableDiag)

/***************************************************************************/
/***drv rstp ioctl*/
/***************************************************************************/
#define Ioctl_GetRStpPortState(_lport, _puiState)\
	Ioctl_GetUnionUint32ByUint32(DRV_CMD_GET_STP_PORTSTATE, _lport, (UINT32 *)_puiState)
#define Ioctl_SetRStpPortState(_lport, _uiState)\
		Ioctl_SetUnionUint32Uint32(DRV_CMD_SET_STP_PORTSTATE, _lport, _uiState)
/***************************************************************************/
/***GPIO ioctl*/
/***************************************************************************/
#define Ioctl_GetGpioVal(_gpiopin, _pulData)\
    Ioctl_GetUnionUint32ByUint32(DRV_CMD_GPIO_GET, _gpiopin, (UINT32*)_pulData)
#define Ioctl_SetGpioVal(_gpiopin, _pulData)\
    Ioctl_SetUnionUint32Uint32(DRV_CMD_GPIO_SET, _gpiopin, _pulData)
#define Ioctl_GetGpioMulti(_gpiopin, _pulData)\
    Ioctl_GetUnionUint32ByUint32(DRV_CMD_GPIO_MULTIPLEX_GET, _gpiopin, (UINT32*)_pulData)
#define Ioctl_SetGpioMulti(_gpiopin, _pulData)\
    Ioctl_SetUnionUint32Uint32(DRV_CMD_GPIO_MULTIPLEX_SET, _gpiopin, _pulData)
#define Ioctl_SetGpioDirIn(_gpiopin)\
    Ioctl_SetUnionUint32(DRV_CMD_GPIO_DIR_IN_SET, _gpiopin)
#define Ioctl_SetGpioDirOut(_gpiopin)\
    Ioctl_SetUnionUint32(DRV_CMD_GPIO_DIR_OUT_SET, _gpiopin)
#define Ioctl_SetGpioTrigModeLevel(_gpiopin)\
    Ioctl_SetUnionUint32(DRV_CMD_GPIO_MODE_LEVEL_SET, _gpiopin)
#define Ioctl_SetGpioTrigModeEdge(_gpiopin)\
    Ioctl_SetUnionUint32(DRV_CMD_GPIO_MODE_EDGE_SET, _gpiopin)
#define Ioctl_SetGpioIntLevelHigh(_gpiopin)\
    Ioctl_SetUnionUint32(DRV_CMD_GPIO_INT_LEVEL_HIGH_SET, _gpiopin)
#define Ioctl_SetGpioIntLevelLow(_gpiopin)\
    Ioctl_SetUnionUint32(DRV_CMD_GPIO_INT_LEVEL_LOW_SET, _gpiopin)
#define Ioctl_SetGpioIntEdge(_gpiopin, _pulData)\
    Ioctl_SetUnionUint32Uint32(DRV_CMD_GPIO_INT_EDGE_SET, _gpiopin, _pulData)
#define Ioctl_GetPonStatusErrFlag(_errflag)\
    Ioctl_GetUnionUint32(DRV_CMD_GET_PON_STATUS_ERR_FLAG, (UINT32*)_errflag)
#define Ioctl_SetPonStatusErrFlag(_errflag)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_PON_STATUS_ERR_FLAG, _errflag)
#define Ioctl_GetPonModeOffFlag(_errflag)\
    Ioctl_GetUnionUint32(DRV_CMD_GET_PON_MODE_OFF_FLAG, (UINT32*)_errflag)
#define Ioctl_SetPonModeOffFlag(_errflag)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_PON_MODE_OFF_FLAG, _errflag)
/*Begin add by huangmingjian 2014/02/23*/ 
#define Ioctl_GetOamDiscoveryState(_oam_discovery_state)\
    Ioctl_GetUnionUint32(DRV_CMD_GET_OAM_DISCOVERY_STATE, (UINT32*)_oam_discovery_state)
#define Ioctl_SetOamDiscoveryState(_oam_discovery_state)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_OAM_DISCOVERY_STATE, _oam_discovery_state)
/*End add by huangmingjian 2014/02/23*/
/*Begin add by huangmingjian 2014/04/04 for Bug 500*/
#define Ioctl_GetMacWriteFlag(_write_flag)\
    Ioctl_GetUnionUint32(DRV_CMD_GET_MAC_WRITE_FLAG, (UINT32*)_write_flag)
#define Ioctl_SetMacWriteFlag(_write_flag)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_MAC_WRITE_FLAG, _write_flag)
/*End add by huangmingjian 2014/04/04 for Bug 500*/
#define Ioctl_GetPonIntChangeFlag(_changeflag)\
    Ioctl_GetUnionUint32(DRV_CMD_GET_PON_INT_CHANGE_FLAG, (UINT32*)_changeflag)
#define Ioctl_SetPonIntChangeFlag(_changeflag)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_PON_INT_CHANGE_FLAG, _changeflag)
#define Ioctl_GetGpioIntStatus(_gpiopin, _pulData)\
    Ioctl_GetUnionUint32ByUint32(DRV_CMD_OPL_INTREG_FIELD_READ, _gpiopin, (UINT32*)_pulData)
#define Ioctl_SetGpioIntStatus(_gpiopin, _pulData)\
    Ioctl_SetUnionUint32Uint32(DRV_CMD_OPL_INTREG_FIELD_WRITE, _gpiopin, _pulData)

/*begin modified by liaohongjun 2012/11/19 of EPN204QID0008*/
#define Ioctl_SetSysLed(_offset, _pulData)\
    Ioctl_SetUnionUint32Uint32(DRV_CMD_SET_SYS_LED, _offset, _pulData)
/*end modified by liaohongjun 2012/11/19 of EPN204QID0008*/

DRV_RET_E Ioctl_CtcDealPortBaseVlanMem(DRV_CMD_E ioctlCmd, UINT32 uiVid, logic_pmask_t *pLgcPMask);
DRV_RET_E Ioctl_CtcDelPortAclRule(DRV_CMD_E ioctlCmd, UINT32 uiLPort);
DRV_RET_E Ioctl_CtcPortIngrVlanFilter(DRV_CMD_E ioctlCmd, UINT32 uiLPort, BOOL bEnable);
DRV_RET_E Ioctl_CtcPortIngrVlanRcvMode(DRV_CMD_E ioctlCmd, UINT32 uiLPort, PORT_INGRESS_MODE_E enIngressMode);
DRV_RET_E Ioctl_CtcGetPortVlanCfg(DRV_CMD_E ioctlCmd, UINT32 uiLPort, CTC_VLAN_CFG_S *pstCtcVlanCfg);
DRV_RET_E Ioctl_CtcSetPortVlanCfg(DRV_CMD_E ioctlCmd, UINT32 uiLPort, CTC_VLAN_CFG_S *pstCtcVlanCfg);
DRV_RET_E Ioctl_CtcAddPortAclRuleForVlan(DRV_CMD_E ioctlCmd, UINT32 uiLPort, CTC_VLAN_CFG_S stCtcVlanCfg);
DRV_RET_E Ioctl_CtcCheckEnoughVlanEntry(DRV_CMD_E ioctlCmd, CTC_VLAN_CFG_S stCtcVlanCfg, UINT32 *puiEnough);
DRV_RET_E Ioctl_GetUnionUint32Uint32Uint32ByUint32(DRV_CMD_E ioctlCmd, UINT32 uiValue,  UINT32 *puiValue1, UINT32 *puiValue2, UINT32 *puiValue3);
DRV_RET_E Ioctl_GetUnionFdbEntryByIndex(DRV_CMD_E ioctlCmd, UINT32 uiIndex,  UINT32 *puiFid, mac_common_t *pstFdbEntry);

#define Ioctl_ctc_add_port_base_vlan_mem(_Vid, _lPortMask)\
    Ioctl_CtcDealPortBaseVlanMem(DRV_CMD_CTC_PORT_BASE_VLAN_MEM_ADD, _Vid, _lPortMask)
#define Ioctl_ctc_del_port_base_vlan_mem(_Vid, _pLPortMask)\
    Ioctl_CtcDealPortBaseVlanMem(DRV_CMD_CTC_PORT_BASE_VLAN_MEM_DEL, _Vid, _pLPortMask)
#define Ioctl_ctc_add_port_base_vlan_mem_by_id(_EntryId, _pLPortMask)\
    Ioctl_CtcDealPortBaseVlanMem(DRV_CMD_CTC_PORT_BASE_VLAN_MEM_ADD_BY_ID, _EntryId, _pLPortMask)
#define Ioctl_ctc_del_port_base_vlan_mem_by_id(_EntryId, _pLPortMask)\
    Ioctl_CtcDealPortBaseVlanMem(DRV_CMD_CTC_PORT_BASE_VLAN_MEM_DEL_BY_ID, _EntryId, _pLPortMask)
#define Ioctl_ctc_clr_port_base_vlan_entry_by_vid(_VlanId)\
    Ioctl_SetUnionUint32(DRV_CMD_CTC_PORT_BASE_VLAN_ENTY_CLR, _VlanId)
#define Ioctl_ctc_del_acl_rule_for_transp(_uiLPort)\
    Ioctl_CtcDelPortAclRule(DRV_CMD_CTC_ACL_FOR_TRANSP_VLAN_DELETE, _uiLPort)
#define Ioctl_ctc_del_acl_rule_for_tag(_uiLPort)\
    Ioctl_CtcDelPortAclRule(DRV_CMD_CTC_ACL_FOR_TAG_VLAN_DELETE, _uiLPort)

#define Ioctl_SetMcVlanMemberAdd(_vlan_id, _lport_mask, _lport_mask_untag)\
    Ioctl_SetUnionUint32LpmaskLpmask(DRV_CMD_CTC_MC_VLAN_ADD, (UINT32)_vlan_id,_lport_mask,_lport_mask_untag)
/* */
#define Ioctl_ctc_mc_get_mac(_vid, _mac_address, _pmac_mcast)\
    Ioctl_GetUnionMacVidMcast(DRV_CMD_CTC_MC_MAC_GET, (UINT32)_vid, _mac_address, (mac_mcast_t *)_pmac_mcast)
#define Ioctl_ctc_mc_set_mac(_mac_mcast)\
    Ioctl_SetUnionMacMcast(DRV_CMD_CTC_MC_MAC_SET, (mac_mcast_t)_mac_mcast)
#define Ioctl_ctc_mc_del_mac(_mac_mcast)\
    Ioctl_SetUnionMacMcast(DRV_CMD_CTC_MC_MAC_DEL, (mac_mcast_t)_mac_mcast)
#define Ioctl_ctc_mc_rmv_vlan_mem(_vlan_id, _lport_mask)\
    Ioctl_SetUnionUint32Lpmask(DRV_CMD_CTC_MC_VLAN_MEM_RMV, (UINT32)_vlan_id,_lport_mask)
#define Ioctl_ctc_mc_get_vlan_mem(_vlan_id, _lport_mask, _lport_mask_untag)\
    Ioctl_GetUnionUint32MaskMask(DRV_CMD_CTC_MC_VLAN_MEM_GET, (UINT32)_vlan_id,(logic_pmask_t *)_lport_mask,(logic_pmask_t *)_lport_mask_untag)
/* 013987 */
#define Ioctl_ctc_add_acl_rule_for_transp(_uiLPort, stCtcVlan)\
    Ioctl_CtcAddPortAclRuleForVlan(DRV_CMD_CTC_ACL_FOR_TRANSP_VLAN_CREATE, _uiLPort, stCtcVlan)
#define Ioctl_ctc_add_acl_rule_for_tag(_uiLPort, stCtcVlan)\
    Ioctl_CtcAddPortAclRuleForVlan(DRV_CMD_CTC_ACL_FOR_TAG_VLAN_CREATE, _uiLPort, stCtcVlan)
#define Ioctl_ctc_add_acl_rule_for_transl(_uiLPort, stCtcVlan)\
    Ioctl_CtcAddPortAclRuleForVlan(DRV_CMD_CTC_ACL_FOR_TRANSL_VLAN_CREATE, _uiLPort, stCtcVlan)

//#ifdef CTC_MULTICAST_SURPORT 
#if 1
#define Ioctl_ctc_add_acl_rule_for_mvlan(_uiLPort, _uiMcVlan)\
    Ioctl_SetUnionUint32Uint32(DRV_CMD_CTC_ACL_FOR_MC_VLAN_CREATE, _uiLPort, _uiMcVlan)
#define Ioctl_ctc_del_acl_rule_for_mvlan(_uiLPort, _uiMcVlan)\
    Ioctl_SetUnionUint32Uint32(DRV_CMD_CTC_ACL_FOR_MC_VLAN_DELETE, _uiLPort, _uiMcVlan)
#endif

#define Ioctl_ctc_get_acl_rule(_uiLPort, _pstCtcVlan)\
    Ioctl_CtcGetPortVlanCfg(DRV_CMD_CTC_GET_PORT_VLAN_CFG, _uiLPort, _pstCtcVlan)  
#define Ioctl_ctc_set_acl_rule(_uiLPort, _pstCtcVlan)\
    Ioctl_CtcSetPortVlanCfg(DRV_CMD_CTC_SET_PORT_VLAN_CFG, _uiLPort, _pstCtcVlan)  

#define Ioctl_ctc_del_acl_rule_for_transl(_uiLPort)\
    Ioctl_CtcDelPortAclRule(DRV_CMD_CTC_ACL_FOR_TRANSL_VLAN_DELETE, _uiLPort)
#define Ioctl_ctc_port_ingress_vlan_filter(_uiLPort, _bEnable)\
    Ioctl_CtcPortIngrVlanFilter(DRV_CMD_CTC_PORT_INGRESS_VLAN_FILTER, _uiLPort, _bEnable)
#define Ioctl_ctc_port_ingress_vlan_rcv_mod(_uiLPort, _enMode)\
    Ioctl_CtcPortIngrVlanRcvMode(DRV_CMD_CTC_PORT_INGRESS_VLAN_RECIEVE, _uiLPort, _enMode)
#define Ioctl_ctc_port_egress_vlan_mode(_uiLPort, _enMode)\
    Ioctl_SetUnionUint32Uint32(DRV_CMD_CTC_PORT_EGRESS_VLAN_FILTER, _uiLPort, _enMode)
#define Ioctl_ctc_get_empty_acl_num(_puiNum)\
    Ioctl_GetUnionUint32(DRV_CMD_CTC_GET_EMPTY_ACL_RULE_NUM, _puiNum)
#define Ioctl_ctc_restart_autoneg(_uiLPort)\
	Ioctl_SetUnionUint32(DRV_CMD_CTC_RESTART_AUTONEG, _uiLPort)
#define Ioctl_ctc_enough_vlan_index_check(_stCtcVlan, _puiVal)\
    Ioctl_CtcCheckEnoughVlanEntry(DRV_CMD_CTC_ENOUGH_VLAN_INDEX_CHECK, _stCtcVlan, _puiVal)

#define Ioctl_igmp_unknow_ip4_act(_uiActMode)\
	Ioctl_SetUnionUint32(DRV_CMD_IGMP_UNKNOW_IP4_ACT, _uiActMode)
/*  */
#define Ioctl_ctc_get_cvlan_entry(_uiCvid,_puiPPortMsk,_puiPPortMskUntag,_puiFid)\
    Ioctl_GetUnionUint32Uint32Uint32ByUint32(DRV_CMD_CVLAN_ENTRY_GET, _uiCvid, \
                                             _puiPPortMsk,_puiPPortMskUntag,_puiFid)
#define Ioctl_ctc_get_svlan_entry(_uiSvid,_puiPPortMsk,_puiPPortMskUntag,_puiFid)\
    Ioctl_GetUnionUint32Uint32Uint32ByUint32(DRV_CMD_SVLAN_ENTRY_GET, _uiSvid, \
                                             _puiPPortMsk,_puiPPortMskUntag,_puiFid)
#define Ioctl_ctc_get_c2s_entry(_uiIndx,_puiCvid,_puiSvid,_puiPPortMsk)\
    Ioctl_GetUnionUint32Uint32Uint32ByUint32(DRV_CMD_C2S_ENTRY_GET, _uiIndx, \
                                             _puiCvid,_puiSvid,_puiPPortMsk)   
#define Ioctl_ctc_get_s2c_entry(_uiIndx,_puiCvid,_puiSvid,_puiDstPPort)\
    Ioctl_GetUnionUint32Uint32Uint32ByUint32(DRV_CMD_S2C_ENTRY_GET, _uiIndx, \
                                             _puiCvid,_puiSvid,_puiDstPPort) 

#define Ioctl_get_fdb_entry(_uiIndex, _puiFid, _pstFdbEntry)\
    Ioctl_GetUnionFdbEntryByIndex(DRV_CMD_FDB_ENTRY_GET, _uiIndex, \
                                  _puiFid, _pstFdbEntry)
/* 014528 */


#define Ioctl_GetSingleDbgReg(_RgAdd, _pulData)\
    Ioctl_GetUnionUint32ByUint32(DRV_CMD_SINGLE_DBG_REG_GET, _RgAdd, (UINT32*)_pulData)
#define Ioctl_SetSingleDbgReg(_RgAdd, _ulData)\
    Ioctl_SetUnionUint32Uint32(DRV_CMD_SINGLE_DBG_REG_SET, _RgAdd, _ulData)



#define Ioctl_SetVlanPortEgressMode(_port_number, _enMode)\
    Ioctl_SetUnionUint32Uint32(DRV_CMD_PORT_EGRESS_MODE, (UINT32)_port_number, _enMode)

/*  */


/* 014325 */
/*  */
#define Ioctl_PortsInit()\
    Ioctl_SetUnionUint32(DRV_CMD_PORTS_INIT, 0)
/*014207 */

#define Ioctl_SetManageVlan(_vlanIndex)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_MANAGE_VLAN, _vlanIndex)

/*Begin add by shipeng 2013-11-11*/
#define Ioctl_SetWirelessUpServiceVlan(_vlanIndex)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_WIRELESS_UP_SERVICE_VLAN, _vlanIndex)
/*End add by shipeng 2013-11-11*/

/*Begin add by huangmingjian 2013/03/15 for EPN204QID0033*/ 
#define Ioctl_SetLaserOnTestFlag(data0)\
		Ioctl_SetLaserOnTest_Flag(DRV_CMD_SET_LASERON_TEST_FLAG, data0)
	
#define Ioctl_GetLaserOnTestFlag(data0)\
		Ioctl_GetLaserOnTest_Flag(DRV_CMD_GET_LASERON_TEST_FLAG, data0)

/*End add by huangmingjian 2013/03/15 for EPN204QID0033*/ 

/*Begin add by shipeng 2013/05/27 for CTC*/ 
#define Ioctl_SetFecMode(_enable)\
    Ioctl_SetUnionUint32(DRV_CMD_SET_FECMODE, (UINT32)_enable)

#define Ioctl_GetFecMode(_action)\
    Ioctl_GetUnionUint32(DRV_CMD_GET_FECMODE, (UINT32 *)_action)
/*End add by shipeng 2013/05/27 for CTC*/    

#define Ioctl_port_ingress_vlan_filter(_uiLPort, _bEnable)\
    Ioctl_CtcPortIngrVlanFilter(DRV_CMD_CTC_PORT_INGRESS_VLAN_FILTER, _uiLPort, _bEnable)
#define Ioctl_port_ingress_vlan_rcv_mod(_uiLPort, _enMode)\
		Ioctl_CtcPortIngrVlanRcvMode(DRV_CMD_CTC_PORT_INGRESS_VLAN_RECIEVE, _uiLPort, _enMode)
#define Ioctl_port_egress_vlan_mode(_uiLPort, _enMode)\
		Ioctl_SetUnionUint32Uint32(DRV_CMD_CTC_PORT_EGRESS_VLAN_FILTER, _uiLPort, _enMode)
		
/*Begin add by huangmingjian 2013-09-24 */
#define Ioctl_SetLookupMissFloodEnable(type, flood_portmask)\
    Ioctl_SetLookupMissFloodPortMask(DRV_CMD_LOOKUP_MISS_FLOOD_SET, type, flood_portmask)
/*End add by huangmingjian 2013-09-24 */

DRV_RET_E ioctl_SetMctransltVlanMemberRmv(UINT32 usrvlan,UINT32 mvlan, logic_pmask_t stLgcMask);
DRV_RET_E ioctl_SetMcTransltVlanMemberAdd(UINT32 usrvlan, UINT32 mvlan, logic_pmask_t stLgcMask, logic_pmask_t stLgcMaskUntag,logic_pmask_t stLgcMasksvlanUntag);
DRV_RET_E ioctl_ctcdelc2sentry(UINT32 uiLPort,UINT32 cvlan,UINT32 svlan);
DRV_RET_E ioctl_ctcaddc2sentry(UINT32 uiLPort,UINT32 cvlan,UINT32 svlan);
DRV_RET_E ioctl_ctcdels2centry(UINT32 uiLPort,UINT32 svlan);
DRV_RET_E ioctl_ctcadds2centry(UINT32 uiLPort,UINT32 svlan,UINT32 cvlan);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif  /* end of _DRV_REQ_H_ */
