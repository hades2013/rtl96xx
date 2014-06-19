#ifndef __DRV_LW_H__
#define __DRV_LW_H__


//#ifdef CTC_MULTICAST_SURPORT
typedef enum tagMULTICAST_Tag_Oper_Mode 
{
  MC_TAG_MODE_TRANSPARENT=0,
  MC_TAG_MODE_STRIP,
  MC_TAG_MODE_TRANSLATION,
  MC_TAG_MODE_END
} MC_TAG_MODE_E;
//#endif



typedef enum CNT_POLL_e
{
	CNT_GE=1,
	CNT_PON,
	CNT_FE,
	CNT_TM,
	CNT_CLE,
	CNT_BRG,
	CNT_MPCP,
	CNT_MARB,
	CNT_CPDMA,
	CNT_FEDMA,
	CNT_GEPARSER,
	CNT_CLEHIT,
	CNT_SWITCH
}CNT_POLL_t;

typedef struct tagPORT_CFG
{
    /* ethernet port config */
    UCHAR ucPortEnable;                     /* enable/disable */
    UCHAR ucSpeed;                          /* 100/10 */
    UCHAR ucDuplex;                         /* full/half/auto */
    UCHAR ucMdi;                            /* auto */
    UCHAR ucFlowCtrl;                       /* enable/diable */
    UCHAR ucAggMaster;                      /* true/false */
    UCHAR ucStormCtrl;                      /* 0 - 100 */
    UCHAR ucIsolateEnable;                  /* enable/disable */
    UCHAR ucLinkType;                       /* access/trunk/hybrid */
    UCHAR ucVlanPri;                        /* vlan priority */
    USHORT usPvid;                          /* default vlan of port */
    BOOL_T bAutoNegMode;
    ULONG ulLineRateIn;                     /* line-rate inbound */
    ULONG ulLineRateOut;                    /* line-rate outbound */
    ULONG ulQosTrustMode; /*014453 */
}PORT_CFG_S;

/*Port vct*/
#define VCT_PORT_INVALID      65535
#define CABLE_PAIR_NUM    4
#define CABLE_PAIR_0      0
#define CABLE_PAIR_1      1
#define CABLE_PAIR_2      2
#define CABLE_PAIR_3      3
#define VCT_CABLELEN_INVALID    0xFFFF

/* MIRROR MODE */
#define IN_MIRROR                       1
#define OUT_MIRROR                      2
#define BOTH_MIRROR                     3
#define NOT_MIRROR_PORT                 0xff

#define TPID_8100        0x8100
#define TPID_8200        0x8200
#define TPID_9100        0x9100
#define   VALID_PRIORITY_ID( x )  ( (x) <= 7 )

#ifndef RTL8370_VIDMAX
#define RTL8370_VIDMAX   0xFFF
#endif

/*光模块地址*/
#define IIC_OPTICS_TRANSCEIVER_ADDRESS1                       0x50
#define IIC_OPTICS_TRANSCEIVER_ADDRESS2                       0x51

#define IIC_OPTICS_TRANSCEIVER_VENDOR_PN_FIRST_REGISTER       0x28
#define IIC_OPTICS_TRANSCEIVER_TEMPERATURE_MSB_REGISTER       0x60
#define IIC_OPTICS_TRANSCEIVER_TEMPERATURE_LSB_REGISTER       0x61
#define IIC_OPTICS_TRANSCEIVER_TXPOWER_MSB_REGISTER           0x66
#define IIC_OPTICS_TRANSCEIVER_TXPOWER_LSB_REGISTER           0x67
#define IIC_OPTICS_TRANSCEIVER_RXPOWER_MSB_REGISTER           0x68
#define IIC_OPTICS_TRANSCEIVER_RXPOWER_LSB_REGISTER           0x69

#define OPTICS_TRANSCEIVER_RXPOWER_LOW_LIMIT                  (-30)
#define OPTICS_TRANSCEIVER_VENDOR_PN_LEN                      16

#define INVALID_OPTIC_POWER 0xFFFF  /* 无效的光功率值 */
#define MAX_RECIVE_OPTIC_POWER 5
#define MIN_RECIVE_OPTIC_POWER -40

#define OFFSET_MSB 0
#define OFFSET_LSB 1

#define CLF_RMK_RULES_IN_1_PRECEDENCE_MAX 1

#define OPTICS_POWER_CALCULATE(PowerData, pPower) \
do\
{\
    *(pPower) = (FLOAT)(((double)10) * ((log10((double)(PowerData))) - (double)4));\
}while(0)

#define CHECK_OPTIC_POWER(fOpticPwr)\
do\
{\
    if(fOpticPwr > MAX_RECIVE_OPTIC_POWER) \
    {\
        fOpticPwr = INVALID_OPTIC_POWER;\
    }\
    else if(fOpticPwr < MIN_RECIVE_OPTIC_POWER)\
    {\
        fOpticPwr = INVALID_OPTIC_POWER;\
    }\
}while(0)

#define MIN_PORT_BANDWIDTH   8
#define MAX_PORT_BANDWIDTH   (1000*1024)

#ifndef H3C_ASSERT
#if 1
#define H3C_ASSERT(x) \
do \
{\
   if(!(x))\
   {\
       printf("\nH3c assert fail!!! func:%s line:%d.\n",__FUNCTION__,__LINE__);\
       return ERROR;\
   }\
}while(0)
#else
#define H3C_ASSERT(x) 
#endif
#endif

#ifndef DRV_DEBUG_PRINT
#if 0
#define DRV_DEBUG_PRINT(format,args...) \
do                                      \
{                                       \
    printf(format, ## args);            \
}while(0)
#else
#define DRV_DEBUG_PRINT(format,args...) 
#endif
#endif

#define VALID_UNI_PORT( x )  ( ( ( x >= 1) && ( x <= MAX_PORT_NUM) ) ?1 : 0 )

#define MEM_SET_ZERO(pMem, ulLen)  memset((VOID *)(pMem), 0, (ULONG)(ulLen))
/* 内存比较 */
#define MEM_COMPARE(pDest, pSrc, ulLen) memcmp((VOID *)(pDest), (VOID *)(pSrc), (ULONG)(ulLen))

/* 镜像目的端口缺省值 */
#define MONITOR_PORT_DEFAULT_VALUE      0xff

#if 0
extern OPL_STATUS HalQinqClfSet(UINT16 usVid, UP_DOWN_STREAM_e enDirect);
extern OPL_STATUS HalQinqClfReset(VOID);
extern int CntClr(int iModule, UINT8 ucStart, UINT8 ucNum );
extern int i2cReadRandom (UINT8 slvAddr, UINT8 staRegAddr, UINT32 nByte, UINT8 *pDataBuf );
extern INT32 DRV_GetPortVlanCfg(UINT32 uiLPortId, UINT8 *pucMode, UINT8 *pucVlanCfg, UINT16 *pusLength);
extern INT32 DRV_ResetAllPortsStatistic(VOID);
extern INT32 DRV_GetPortMibCounter(UINT32 lport, UNI_COUNTER_S*  pMibCtrs);
extern INT32 DRV_GetPortLinkStatus (UINT32 lport, BOOL* pStatus);
extern INT32 DRV_GetPortDuplex (UINT32 lport, PORT_DUPLEX_E* pDuplexMode);
extern INT32 DRV_GetPortSpeed (UINT32 lport, PORT_SPEED_E* pSpeedMode);
extern INT32 DRV_GetPortMdiStatus(UINT32 lport,  PORT_MDI_E* pMdiMode);
extern INT32 DRV_ResetPortStatistic (UINT32 lport);
extern INT32 DRV_SetPortSpeed (UINT32 lport, PORT_SPEED_E enPortSpeed);
extern INT32 DRV_SetPortDuplex (UINT32 lport, PORT_DUPLEX_E enPortDuplex);
extern INT32 DRV_SetPortFlowCtrl(UINT32 lport,  BOOL bEnable);
extern INT32 DRV_SetPortLineRate(UINT32 lport,  PORT_DIRECT_E enDirect, UINT32 uiLineRate);
extern INT32 DRV_SetPortLinkState(UINT32 uiLPortId, BOOL bPortStatus);
extern INT32 DRV_PortRestartNeg(UINT32 uiLPortId);
extern INT32 DRV_SetPortAutoNeg(UINT32 uiLPortId, UINT8 ucAction);
extern INT32 DRV_SetPortIsolation (UINT32 lport,  BOOL bEnable);
extern INT32 DRV_SetCos2QidMap (UINT32* auiCos2QidMap);
extern INT32 DRV_SetPriTrustMode (QOS_PRI_TRUST_MODE_E enTrustMode);
extern INT32 DRV_SetPriSchedMode (QOS_QUEUE_SCHEDULER_MODE_E enPriSchedMode);
extern INT32 DRV_SetStpEnable(BOOL ulEnable);
extern INT32 DRV_AclAddFilterRule(ACL_FILTER_RULE_S* pRule);
extern INT32 DRV_AclDelFilterRule(ACL_FILTER_RULE_S* pRule);
extern INT32 DRV_CtcAddClfRmkByPrec(UINT32 lport,
	                         UINT32 precedenceOfRule,
	                         oam_clasmark_rulebody_t *pClsRule,
	                         oam_clasmark_fieldbody_t *pClsField);
extern INT32 DRV_CtcClearClfRmkByPort(UINT32 ucPortS);
extern INT32 DRV_CtcDeleteClfRmkByPrec(UINT32 lport, UINT32 precedenceOfRule);
extern INT32 DRV_SetMonitor(UINT8 lport, UINT8 lenable);
extern INT32 DRV_AddMirror(UINT8 ucPortIdx, UCHAR ucMirrorDirect);
extern INT32 DRV_DelMirror(UINT8 ucPortIdx, UCHAR ucMirrorDirect);
extern ULONG Drv_GetOpticsPower(FLOAT *pReceivPower, FLOAT *pTransPower);
extern ULONG Drv_GetOpticsTemperature(SHORT *psTempratrue, USHORT *psVoltage, USHORT *psBias);
extern ULONG Drv_GetOpticsTransmitPower(USHORT *pusTransPower);
extern ULONG Drv_GetOpticsReceivePower(USHORT *pusReceivPower);
extern INT32 DRV_GetPortQinqCfg(UINT32 uiLPortId, PORT_QINQ_S *pstQinqCfg);
extern ULONG Drv_SetStpEnable(ULONG ulEnable);
extern UCHAR CFG_GetStpEnable(ULONG *pulEnalbe);
extern ULONG RSTP_Comm_GetPortRole(ULONG ulPortIndex);
extern ULONG RSTP_Comm_GetPortStatus(ULONG ulPortIndex);
extern INT32 DRV_ResetQinqPvid(UINT32 uiLPortId);
extern INT32 DRV_ResetPortQinq(UINT32 uiLPortId);
extern INT32 DRV_EnablePortQinq(UINT32 uiLPortId, UINT32 uiTpid, UINT32 uiPvid, UINT32 uiPriority);
extern INT32 DRV_SetPortQinq(UINT32 uiLPortId, QINQ_CTAG_STAG_ENTRY_S *pstQinqCfg);
extern VOID DRV_InitQinqCfg(VOID);
extern INT32 DRV_GetPortVct(ULONG ulLPort, VCT_CABLE_DIAG_S *pstCableDiag );
extern INT32 DRV_BlockInnerUni(BOOL bUniEnable);
/* Begin  */
extern INT32 _CTC_GetUniIsolateEnableCfg( ULONG ulLPort, BOOL *pbEnable );
extern INT32 _CTC_SetUniIsolateEnableCfg( ULONG ulLPort, BOOL bEnable );
extern INT32 _CTC_GetUniSpeedCfg( ULONG ulLPort, UCHAR *pucSpeed );
extern INT32 _CTC_SetUniSpeedCfg( ULONG ulLPort, UCHAR ucSpeed );
extern INT32 _CTC_GetUniDuplexCfg( ULONG ulLPort, UCHAR *pucDuplex );
extern INT32 _CTC_SetUniDuplexCfg( ULONG ulLPort, UCHAR ucDuplex );
extern INT32 _CTC_GetUniMdiCfg( ULONG ulLPort, UCHAR *pucMdi );
extern INT32 _CTC_SetUniMdiCfg( ULONG ulLPort, UCHAR ucMdi );
extern INT32 _CTC_GetUniAutoNegModeCfg( ULONG ulLPort, BOOL *pbAutoNegMode );
extern INT32 _CTC_SetUniAutoNegModeCfg( ULONG ulLPort, BOOL bAutoNegMode );
extern INT32 _CTC_GetUniLineRateOutCfg( ULONG ulLPort, ULONG *pulLineRateOut );
extern INT32 _CTC_SetUniLineRateOutCfg( ULONG ulLPort, ULONG ulLineRateOut );
extern INT32 DRV_ClearAllFilterRules( VOID );

/* 014043 */
extern INT32 DRV_25L90ImodSet(UINT32 uiModVal);
extern INT32 DRV_25L90ApcSet(UINT32 uiApcVal);
#endif

#endif
