/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include <linux/string.h>

#include "hal_common.h"
#include "rtk_api_ext.h"
#include "lw_config.h"

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/
extern void Hal_SetEthdbg(unsigned int eth_dbg);
extern DRV_RET_E Hal_SetManageVlan(unsigned int uiVlanIndex);
extern DRV_RET_E Hal_CpuRegRead(UINT32 regAddr,UINT32 *regVal);
extern DRV_RET_E Hal_CpuRegWrite(UINT32 regAddr,UINT32 regVal);
extern DRV_RET_E Hal_GpioDirInSet(UINT32 uiPin);
extern DRV_RET_E Hal_GpioDirOutSet(UINT32 uiPin);
extern DRV_RET_E Hal_GpioValGet(UINT32 uiPin, UINT32 *puiRegValue);
extern DRV_RET_E Hal_GpioOutputSet(UINT32 uiPin, UINT32 uiRegValue);
extern DRV_RET_E Hal_GpioMultiplexSet(UINT32 uiPin, UINT32 uiWorkMode);
extern DRV_RET_E Hal_GpioMultiplexGet(UINT32 uiPin, UINT32 * puiWorkMode);
extern DRV_RET_E Hal_GpioTrigModeLevelSet(UINT32 uiPin);
extern DRV_RET_E Hal_GpioTrigModeEdgeSet(UINT32 uiPin);
extern DRV_RET_E Hal_GpioIntLevelHighSet(UINT32 uiPin);
extern DRV_RET_E Hal_GpioIntLevelLowSet(UINT32 uiPin);
extern DRV_RET_E Hal_GpioIntEdgeSet(UINT32 uiPin, UINT32 uiEdgeType);
/*Begin add by huangmingjian 2013/03/15 for EPN204QID0033*/ 
extern DRV_RET_E Hal_LaserSetReg(DRV_REQ_S *pstDrvReq);
extern DRV_RET_E Hal_LaserGetReg(DRV_REQ_S *pstDrvReq);
/*End add by huangmingjian 2013/03/15 for EPN204QID0033*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/
extern void  rtk_port_parm_init(void);
extern DRV_RET_E Hal_AclRuleInit(void);
extern DRV_RET_E Hal_SetCpuHeader(BOOL bEnable);
extern void rtk_SmiInit(void);
/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 宏定义                                       *
*----------------------------------------------*/
void Hal_Register()
{
	memset(&g_LW_Drv_Ops,0,sizeof(g_LW_Drv_Ops));
	g_LW_Drv_Ops.p_fun_getporttype=Hal_GetPortType;
	g_LW_Drv_Ops.p_fun_getportlinkuptype=Hal_GetPortLinkupType;
	g_LW_Drv_Ops.p_Hal_SetPortMedium=Hal_SetPortMedium; 
	g_LW_Drv_Ops.p_Hal_SetPortPriority=Hal_SetPortPriority; 
	g_LW_Drv_Ops.p_Hal_SetPortMdix=Hal_SetPortMdix; 
	g_LW_Drv_Ops.p_Hal_SetPortEnable=Hal_SetPortEnable; 
	g_LW_Drv_Ops.p_Hal_GetPortEnable=Hal_GetPortEnable;
	g_LW_Drv_Ops.p_Hal_SetPortJumbo=Hal_SetPortJumbo;
	g_LW_Drv_Ops.p_Hal_SetPortTxRx=Hal_SetPortTxRx;
	g_LW_Drv_Ops.p_Hal_SetPortAbility=Hal_SetPortAbility;
	g_LW_Drv_Ops.p_Hal_SetPortAutoneg=Hal_SetPortAutoneg;
	//g_LW_Drv_Ops.p_Hal_GetPortAutoneg=Hal_GetPortAutoneg;	
	g_LW_Drv_Ops.p_Hal_SetPortSpeed=Hal_SetPortSpeed;	
	g_LW_Drv_Ops.p_Hal_SetPortDuplex=Hal_SetPortDuplex; 
	g_LW_Drv_Ops.p_Hal_SetPortSpdlx=Hal_SetPortSpdlx;	
	g_LW_Drv_Ops.p_Hal_SetPhyReg=Hal_SetPhyReg; 
	g_LW_Drv_Ops.p_Hal_GetPhyReg=Hal_GetPhyReg;
	g_LW_Drv_Ops.p_Hal_SetEthdbg=Hal_SetEthdbg;
	g_LW_Drv_Ops.p_Hal_GetSpdlxSet=Hal_GetSpdlxSet;
	g_LW_Drv_Ops.p_Hal_GetIsolateSet=Hal_GetIsolateSet;
	g_LW_Drv_Ops.p_Hal_AclRuleForEoamFilterCreate=Hal_AclRuleForEoamFilterCreate;
	g_LW_Drv_Ops.p_Hal_AclRuleForEoamFilterDelete=Hal_AclRuleForEoamFilterDelete;
	g_LW_Drv_Ops.p_Hal_AclRuleForCtcClfRmkCreate=Hal_AclRuleForCtcClfRmkCreate;
	g_LW_Drv_Ops.p_Hal_AclRuleForCtcClfRmkDelete=Hal_AclRuleForCtcClfRmkDelete;
	g_LW_Drv_Ops.p_Hal_AclRuleForCtcClfRmkClear=Hal_AclRuleForCtcClfRmkClear;
	g_LW_Drv_Ops.p_Hal_SetPortFlowctrl=Hal_SetPortFlowctrl;
	g_LW_Drv_Ops.p_Hal_GetPortMedium=Hal_GetPortMedium;
	g_LW_Drv_Ops.p_Hal_GetPortCurrentLink=Hal_GetPortCurrentLink;
	g_LW_Drv_Ops.p_Hal_GetPortAbility=Hal_GetPortAbility;
	g_LW_Drv_Ops.p_Hal_GetPortSpeed=Hal_GetPortSpeed;
	g_LW_Drv_Ops.p_Hal_GetPortDuplex=Hal_GetPortDuplex;
	g_LW_Drv_Ops.p_Hal_GetPortLinkMask=Hal_GetPortLinkMask;
	g_LW_Drv_Ops.p_Hal_GetPortMediaFiberMask=Hal_GetPortMediaFiberMask;
	g_LW_Drv_Ops.p_Hal_GetPortMdix=Hal_GetPortMdix;
	g_LW_Drv_Ops.p_Hal_GetPortPriority=Hal_GetPortPriority;
	g_LW_Drv_Ops.p_Hal_GetPortFlowctrl=Hal_GetPortFlowctrl;
	g_LW_Drv_Ops.p_Hal_GetPortJumbo=Hal_GetPortJumbo;
	g_LW_Drv_Ops.p_Hal_GetPortMaxFrameLen=Hal_GetPortMaxFrameLen;
	g_LW_Drv_Ops.p_Hal_SetPortMaxFrameLen=Hal_SetPortMaxFrameLen;
	g_LW_Drv_Ops.p_Hal_PortLedInit=Hal_PortLedInit;
	g_LW_Drv_Ops.p_Hal_SetPortLed=Hal_SetPortLed;
	//g_LW_Drv_Ops.p_Hal_I2cFiberInit=Hal_I2cFiberInit;
	//g_LW_Drv_Ops.p_Hal_I2cFiberPortRead=Hal_I2cFiberPortRead;
	g_LW_Drv_Ops.p_Hal_SetAggrMode=Hal_SetAggrMode;
	g_LW_Drv_Ops.p_Hal_SetAggrGroup=Hal_SetAggrGroup;
	g_LW_Drv_Ops.p_Hal_GetVlanExistNum=Hal_GetVlanExistNum;
	g_LW_Drv_Ops.p_Hal_SetVlanMode=Hal_SetVlanMode;
	g_LW_Drv_Ops.p_Hal_SetVlanEntryCreate=Hal_SetVlanEntryCreate;
	g_LW_Drv_Ops.p_Hal_SetVlanEntryDelete=Hal_SetVlanEntryDelete;	
	g_LW_Drv_Ops.p_Hal_SetVlanMemberRemove=Hal_SetVlanMemberRemove;
	g_LW_Drv_Ops.p_Hal_SetVlanMemberAdd=Hal_SetVlanMemberAdd;
	g_LW_Drv_Ops.p_Hal_SetVlanPvid=Hal_SetVlanPvid;
	g_LW_Drv_Ops.p_Hal_SetVlanPortVlanMember=Hal_SetVlanPortVlanMember;
	g_LW_Drv_Ops.p_Hal_GetVlanMember=Hal_GetVlanMember;
	g_LW_Drv_Ops.p_Hal_AddCpuToVlanMember=Hal_AddCpuToVlanMember;
	g_LW_Drv_Ops.p_Hal_RemoveCpuFromVlanMember=Hal_RemoveCpuFromVlanMember;
	g_LW_Drv_Ops.p_Hal_SetCpuPortMacLearnEnable=Hal_SetCpuPortMacLearnEnable;
	g_LW_Drv_Ops.p_Hal_SetMacLearnEnable=Hal_SetMacLearnEnable;
	g_LW_Drv_Ops.p_Hal_SetMacCpuMacAdd=Hal_SetMacCpuMacAdd;
	g_LW_Drv_Ops.p_Hal_SetMacFlushUcastMac=Hal_SetMacFlushUcastMac;
	g_LW_Drv_Ops.p_Hal_SetMacAddUcastMac=Hal_SetMacAddUcastMac;
	g_LW_Drv_Ops.p_Hal_SetMacDelUcastMac=Hal_SetMacDelUcastMac;
	g_LW_Drv_Ops.p_Hal_SetMacAddMcastMac=Hal_SetMacAddMcastMac;
	g_LW_Drv_Ops.p_Hal_SetMacDelMcastMac=Hal_SetMacDelMcastMac;
	g_LW_Drv_Ops.p_Hal_SetMacAgeTime=Hal_SetMacAgeTime;
	g_LW_Drv_Ops.p_Hal_SetMacLearnLimit=Hal_SetMacLearnLimit;
	g_LW_Drv_Ops.p_Hal_GetMacUcastMac=Hal_GetMacUcastMac;
	g_LW_Drv_Ops.p_Hal_GetMacMcastMac=Hal_GetMacMcastMac;
	g_LW_Drv_Ops.p_Hal_SetMacSetMcastMac=Hal_SetMacSetMcastMac;
	g_LW_Drv_Ops.p_Hal_SetMacLookFailFrd=Hal_SetMacLookFailFrd;
	g_LW_Drv_Ops.p_Hal_GetMacLearnCount=Hal_GetMacLearnCount;
	g_LW_Drv_Ops.p_Hal_RefreshUCast=Hal_RefreshUCast;
	g_LW_Drv_Ops.p_Hal_SetReservedMacTrap=Hal_SetReservedMacTrap;
	g_LW_Drv_Ops.p_Hal_GetMacUcastDump=Hal_GetMacUcastDump;
	g_LW_Drv_Ops.p_Hal_GetMacMcastDump=Hal_GetMacMcastDump;
	g_LW_Drv_Ops.p_Hal_GetMacUcastDumpSorted=Hal_GetMacUcastDumpSorted;
	g_LW_Drv_Ops.p_Hal_SetMacTrapToCpu=Hal_SetMacTrapToCpu;
	g_LW_Drv_Ops.p_Hal_GetMacFindConflictMac=Hal_GetMacFindConflictMac;
	g_LW_Drv_Ops.p_Hal_GetMacByIndex=Hal_GetMacByIndex;
	g_LW_Drv_Ops.p_Hal_GetMacMoreByIndex=Hal_GetMacMoreByIndex;
	g_LW_Drv_Ops.p_Hal_GetMacExact=Hal_GetMacExact;
	g_LW_Drv_Ops.p_Hal_GetMcastCount=Hal_GetMcastCount;
	g_LW_Drv_Ops.p_Hal_SetMcastLookupMissDrop=Hal_SetMcastLookupMissDrop;
	g_LW_Drv_Ops.p_Hal_SetIgmpPktAction=Hal_SetIgmpPktAction;
	g_LW_Drv_Ops.p_Hal_SetPortIsolateMask=Hal_SetPortIsolateMask;
	g_LW_Drv_Ops.p_Hal_SetMirrorGroup=Hal_SetMirrorGroup;
	g_LW_Drv_Ops.p_Hal_GetMibPortStatistics=Hal_GetMibPortStatistics;
	g_LW_Drv_Ops.p_Hal_SetMibCountSync=Hal_SetMibCountSync;
	g_LW_Drv_Ops.p_Hal_SetMibCountReset=Hal_SetMibCountReset;
	g_LW_Drv_Ops.p_Hal_GetPortCounter=Hal_GetPortCounter;
	g_LW_Drv_Ops.p_Hal_GetPortStatistics=Hal_GetPortStatistics;
	g_LW_Drv_Ops.p_Hal_SetRatelimitInit=Hal_SetRatelimitInit;
	g_LW_Drv_Ops.p_Hal_SetRatelimitByPort=Hal_SetRatelimitByPort;
	g_LW_Drv_Ops.p_Hal_SetRatelimitStormCtl=Hal_SetRatelimitStormCtl;
	g_LW_Drv_Ops.p_Hal_SetQosInit=Hal_SetQosInit;
	g_LW_Drv_Ops.p_Hal_Set1pIngressPriorityRemap=Hal_Set1pIngressPriorityRemap;
	g_LW_Drv_Ops.p_Hal_SetDscpIngressPriorityRemap=Hal_SetDscpIngressPriorityRemap;
	g_LW_Drv_Ops.p_Hal_SetQosTrustMode=Hal_SetQosTrustMode;
	g_LW_Drv_Ops.p_Hal_SetQosQueueSchedul=Hal_SetQosQueueSchedul;
	g_LW_Drv_Ops.p_Hal_SetCos2QidRemap=Hal_SetCos2QidRemap;
	g_LW_Drv_Ops.p_Hal_soc_reg_read=Hal_soc_reg_read;
	g_LW_Drv_Ops.p_Hal_soc_reg_write=Hal_soc_reg_write;
	g_LW_Drv_Ops.p_Hal_CpuRegRead=Hal_CpuRegRead;
	g_LW_Drv_Ops.p_Hal_CpuRegWrite=Hal_CpuRegWrite;
	g_LW_Drv_Ops.p_Hal_SetEEEByPort=Hal_SetEEEByPort;
	g_LW_Drv_Ops.p_Hal_SetAutoPwrDwnByPort=Hal_SetAutoPwrDwnByPort;
	g_LW_Drv_Ops.p_Hal_GetCableDiag=Hal_GetCableDiag;
	g_LW_Drv_Ops.p_Hal_GetStpPortState=Hal_GetStpPortState;
	g_LW_Drv_Ops.p_Hal_SetStpPortState=Hal_SetStpPortState;
	g_LW_Drv_Ops.p_Hal_L2send=Hal_L2send;
	g_LW_Drv_Ops.p_Hal_SetInternalLoopback=Hal_SetInternalLoopback;
	g_LW_Drv_Ops.p_Hal_GpioValGet=Hal_GpioValGet;
	g_LW_Drv_Ops.p_Hal_GpioOutputSet=Hal_GpioOutputSet;
	g_LW_Drv_Ops.p_Hal_GpioMultiplexGet=Hal_GpioMultiplexGet;
	g_LW_Drv_Ops.p_Hal_GpioMultiplexSet=Hal_GpioMultiplexSet;
	g_LW_Drv_Ops.p_Hal_GpioDirInSet=Hal_GpioDirInSet;
	g_LW_Drv_Ops.p_Hal_GpioDirOutSet=Hal_GpioDirOutSet;
	g_LW_Drv_Ops.p_Hal_GpioTrigModeLevelSet=Hal_GpioTrigModeLevelSet;	
	g_LW_Drv_Ops.p_Hal_GpioTrigModeEdgeSet=Hal_GpioTrigModeEdgeSet;
	g_LW_Drv_Ops.p_Hal_GpioIntLevelHighSet=Hal_GpioIntLevelHighSet;
	g_LW_Drv_Ops.p_Hal_GpioIntLevelLowSet=Hal_GpioIntLevelLowSet;
	g_LW_Drv_Ops.p_Hal_GpioIntEdgeSet=Hal_GpioIntEdgeSet;
	g_LW_Drv_Ops.p_Hal_AddPortBaseVlanMemByIndx=Hal_AddPortBaseVlanMemByIndx;
	g_LW_Drv_Ops.p_Hal_DelPortBaseVlanMemByIndx=Hal_DelPortBaseVlanMemByIndx;
	g_LW_Drv_Ops.p_Hal_AddPortBaseVlanMemb=Hal_AddPortBaseVlanMemb;
	g_LW_Drv_Ops.p_Hal_DelPortBaseVlanMemb=Hal_DelPortBaseVlanMemb;
	g_LW_Drv_Ops.p_Hal_ClrPortBaseVlanEntryByVid=Hal_ClrPortBaseVlanEntryByVid;
	g_LW_Drv_Ops.p_Hal_SetMcVlanMemberAdd=Hal_SetMcVlanMemberAdd;
	g_LW_Drv_Ops.p_Hal_GetMcMacByMacAndFid=Hal_GetMcMacByMacAndFid;
	g_LW_Drv_Ops.p_Hal_SetMcMacByMacAndFid=Hal_SetMcMacByMacAndFid;
	g_LW_Drv_Ops.p_Hal_DelMcMacByMacAndFid=Hal_DelMcMacByMacAndFid;
	g_LW_Drv_Ops.p_Hal_SetMcVlanMemberRmv=Hal_SetMcVlanMemberRmv;
	g_LW_Drv_Ops.p_Hal_SetPortTransparentMode=Hal_SetPortTransparentMode;
	g_LW_Drv_Ops.p_Hal_ResetPortTransparentMode=Hal_ResetPortTransparentMode;
	g_LW_Drv_Ops.p_Hal_SetPortTagMode=Hal_SetPortTagMode;	
	g_LW_Drv_Ops.p_Hal_ResetPortTagMode=Hal_ResetPortTagMode;
	g_LW_Drv_Ops.p_Hal_SetPortTranslationMode=Hal_SetPortTranslationMode;
	g_LW_Drv_Ops.p_Hal_ResetPortTranslationMode=Hal_ResetPortTranslationMode;
	g_LW_Drv_Ops.p_Hal_AclRuleForCtcMcVlanCreate=Hal_AclRuleForCtcMcVlanCreate;
	g_LW_Drv_Ops.p_Hal_AclRuleForCtcMcVlanDelete=Hal_AclRuleForCtcMcVlanDelete;
	g_LW_Drv_Ops.p_Hal_SetPortVlanIngressFilter=Hal_SetPortVlanIngressFilter;
	g_LW_Drv_Ops.p_Hal_SetPortVlanIngressMode=Hal_SetPortVlanIngressMode;
	g_LW_Drv_Ops.p_Hal_SetPortVlanEgressMode=Hal_SetPortVlanEgressMode;
	g_LW_Drv_Ops.p_Hal_AclRuleEmptyNumGet=Hal_AclRuleEmptyNumGet;
	g_LW_Drv_Ops.p_Hal_CfgPortCtcVlanGet=Hal_CfgPortCtcVlanGet;
	g_LW_Drv_Ops.p_Hal_CfgPortCtcVlanSet=Hal_CfgPortCtcVlanSet; 
	g_LW_Drv_Ops.p_Hal_RestartPortNeg=Hal_RestartPortNeg;
	g_LW_Drv_Ops.p_Hal_CtcVlanEnoughVlanIdxEntryCheck=Hal_CtcVlanEnoughVlanIdxEntryCheck;
	g_LW_Drv_Ops.p_Hal_SetUnKnowIp4McastAct=Hal_SetUnKnowIp4McastAct;
	g_LW_Drv_Ops.p_Hal_GetFdbEntryByIndex=Hal_GetFdbEntryByIndex;
	g_LW_Drv_Ops.p_Hal_GetCvlanEntry=Hal_GetCvlanEntry;
	g_LW_Drv_Ops.p_Hal_GetSvlanEntry=Hal_GetSvlanEntry;
	g_LW_Drv_Ops.p_Hal_GetC2sEntryByIndex=Hal_GetC2sEntryByIndex;
	g_LW_Drv_Ops.p_Hal_GetSp2cEntryByIndex=Hal_GetSp2cEntryByIndex;
	g_LW_Drv_Ops.p_Hal_PortInit=Hal_PortInit;
	g_LW_Drv_Ops.p_Hal_SetManageVlan=Hal_SetManageVlan;
	g_LW_Drv_Ops.p_Hal_AclRuleForDropLoopdOperation=Hal_AclRuleForDropLoopdOperation;
	g_LW_Drv_Ops.p_Hal_AclRuleForDropLoopdRemove=Hal_AclRuleForDropLoopdRemove;
	g_LW_Drv_Ops.p_Hal_LaserSetReg=Hal_LaserSetReg;
	g_LW_Drv_Ops.p_Hal_LaserGetReg=Hal_LaserGetReg;
	//g_LW_Drv_Ops.p_Hal_SetFecMode=Hal_SetFecMode;
	//g_LW_Drv_Ops.p_Hal_GetFecMode=Hal_GetFecMode;
	g_LW_Drv_Ops.p_Hal_AclRuleEmptyIdGet=_Hal_AclRuleEmptyIdGet;
	g_LW_Drv_Ops.p_ACL_DeleteRuleByAclid=ACL_DeleteRuleByAclid;
	g_LW_Drv_Ops.p_Hal_GetMacLearnEnable=Hal_GetMacLearnEnable;
	g_LW_Drv_Ops.p_Hal_GetMcVlanMem=Hal_GetMcVlanMem;
}

DRV_RET_E Hal_SwitchInit(void)
{    
    const ether_addr_t stSwitchPauseMac = { {0x00, 0x0f, 0xe2, 0x12, 0x34, 0x56} };

#if 0
    UINT32 uiUnit = 0;
    
    rtk_init(0);

    UnitFor(uiUnit)
    {
        Hal_SdkInitSpecial(uiUnit);
    }  
    Hal_SetCPUPortJumbo(1536);
    Hal_SetAclInit();
#else
    rtk_port_mac_ability_t mac_abi;
    rtk_portmask_t port_mask;
    #ifdef CHIPSET_RTL8305
    extern void Hal_GpioInit(void);
    extern void Hal_CpuRegAccessInit(void);
    extern void Hal_MdioRegisterInit(void);
    Hal_CpuRegAccessInit();
    Hal_MdioRegisterInit();
    Hal_GpioInit();
    #endif

	Hal_Register();
    
    rtk_SmiInit();
    /*013987 */
    
    (void)rtk_switch_init();
                        
    rtk_port_parm_init();
                        
    //rtk_shutdown_all_port();

    port_mask.bits[0] = 0x0f;

    /*LED_OP_PARALLEL*/
    (void)rtk_led_operation_set((rtk_led_operation_t)1);

    /*LED_GROUP_1*/
    (void)rtk_led_enable_set((rtk_led_group_t)1, port_mask);

    /*LED_GROUP_1:LED_CONFIG_LINK_ACT*/
    (void)rtk_led_groupConfig_set((rtk_led_group_t)1, (rtk_led_congig_t)2);

    /*For rtl8305
    ext0 work as RGMII PHY mode.
    */
    /*FULL_DUPLEX*/
    mac_abi.duplex = 1;
    /*MAC_FORCE*/
	mac_abi.forcemode = 1;
    /*PORT_LINKUP*/
	mac_abi.link = 1;
    /*DISABLED*/
	mac_abi.nway = 0;
    /*ENABLED*/
	mac_abi.txpause = DISABLED;
    /*ENABLED*/
	mac_abi.rxpause = DISABLED;
    /*SPD_1000M*/
	mac_abi.speed = 2;
    /*MODE_EXT_RGMII*/
	(void)rtk_port_macForceLinkExt0_set((rtk_mode_ext_t)1, &mac_abi);

    /* */
    (void)rtk_svlan_init();
    (void)rtk_vlan_init(); 
    
    /*Change TPID from 0x88a8 to 0x8100*/
    (void)rtk_svlan_tpidEntry_set(0x8100);

    /*set uplink Port as service port*/
    (void)rtk_svlan_servicePort_add(PHY_UPLINK_PORT);

    /*Unmatch packet assign to SVID 0*/
    (void)rtk_svlan_unmatch_action_set(UNMATCH_ASSIGN,0);
		
    /*Untag packet assign to SVID 0*/
    (void)rtk_svlan_untag_action_set(UNTAG_ASSIGN,0);

    /*set cos to internal priority remarking*/
    {
        rtk_pri_t i;
        for (i = 0; i < 7; i ++)
        {
           (void)rtk_qos_1pRemark_set(i,i);
        }
    }
    /* 014453 */

    /**/
    #if 1
    {
    int i;
    rtk_port_phy_data_t uiVal;
    rtk_port_phy_ability_t stphybility;

    memset(&stphybility, 0, sizeof(stphybility));
    stphybility.AutoNegotiation = 1;
    stphybility.Full_10 = 1;
    stphybility.Full_100 = 1;
    /*begin added by liaohongjun 2013/1/5 of EPN104QID0090*/
    #ifdef CONFIG_SWITCH_CHIP_RTL8365MB
    stphybility.Full_1000 = 1;
    #endif
    /*end added by liaohongjun 2013/1/5 of EPN104QID0090*/
    stphybility.Half_10 = 1;
    stphybility.Half_100 = 1;
    
    for (i = 0; i < 4; i++)
    {
        /*disable flow ctrl*/
        (void)rtk_port_phyReg_get((rtk_port_t)i, (rtk_port_phy_reg_t)4, &uiVal);
        uiVal &= ~(1U << 10);
        (void)rtk_port_phyReg_set((rtk_port_t)i, (rtk_port_phy_reg_t)4, uiVal);

        /* Close eee functionality. */
        (void)rtk_eee_portEnable_set((rtk_port_t)i, DISABLED);

        /**/
        (void)rtk_port_phyAutoNegoAbility_set((rtk_port_t)i, &stphybility);
       
       rtk_port_adminEnable_set((rtk_port_t)i, ENABLED);
        
    }

    /* Close green ethernet functionality. */
    (void)rtk_switch_greenEthernet_set(DISABLED);
    }
    #endif

    /*Add realtek header to broadcast arp/dhcp frame.*/
    (void)rtk_filter_igrAcl_init();
    (void)Hal_SetCpuHeader(TRUE);
    /*Add realtek header to the end of frame befor crc.*/
    #if (0 == SWITCH_HEADER_POSITION_BEHIND_SRC_MAC)
    extern ret_t rtl8367b_setAsicCputagPosition(rtk_uint32 postion);
    (void)rtl8367b_setAsicCputagPosition(1);
    #endif
    (void)Hal_AclRuleInit();
	#if defined(CHIP_RTL8305MB)
	(void)rtk_port_backpressureEnable_set(RTK_WHOLE_SYSTEM, ENABLED);	
	(void)rtk_set_pausemac(stSwitchPauseMac);
	#endif
#endif

    /* */
    {
    UINT32 uiLPortIndex;
    CTC_VLAN_CFG_S stVlanMode;

    memset(&stVlanMode, 0, sizeof(stVlanMode));
    stVlanMode.mode = CTC_VLAN_MODE_TRANSPARENT;
    for (uiLPortIndex = 1; uiLPortIndex <= LOGIC_PORT_NO; uiLPortIndex++)
    {
        (void)Hal_SetPortTransparentMode(uiLPortIndex, &stVlanMode);
    }
    }
    /* 013987 */
    return DRV_OK;
}





#ifdef  __cplusplus
}
#endif

