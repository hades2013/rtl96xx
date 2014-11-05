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
#include "lw_config.h"

#include <rtk/switch.h>
#include <rtk/led.h>
#include <rtk/svlan.h>
#include <rtk/vlan.h>
#include <rtk/qos.h>
#include <rtk/port.h>
#include <rtk/acl.h>
#include <rtk/cpu.h>
#include <rtk/classify.h>
#include <rtk/l2.h>
#include <common/type.h>
#include <rtk/gpio.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>
#if defined(CONFIG_BOSA)

#include "rtk_25l90.h"
#endif
#include <rtk/i2c.h>
#include <dal/apollomp/dal_apollomp_l2.h>
#include <linux/kernel.h>


/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/
extern unsigned char fe_addr[6];
extern unsigned int s_ui_management_vlan;
extern unsigned int wireless_up_service_vlan;
extern void _Hal_PortMonitorInit(void);
extern DRV_RET_E __Hal_InitPonLaserMonior(VOID);

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/
extern DRV_RET_E Hal_AddSvlanMember(UINT32 ulSvlanId, UINT32 ulSvlanPri, logic_pmask_t stLgcMask, logic_pmask_t stLgcMaskUntag, UINT32 ulSvlanFid);
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
	//g_LW_Drv_Ops.p_Hal_SetPortJumbo=Hal_SetPortJumbo;
	//g_LW_Drv_Ops.p_Hal_SetPortTxRx=Hal_SetPortTxRx;
	g_LW_Drv_Ops.p_Hal_SetPortAbility=Hal_SetPortAbility;
	g_LW_Drv_Ops.p_Hal_SetPortAutoneg=Hal_SetPortAutoneg;
	g_LW_Drv_Ops.p_Hal_GetPortAutoneg=Hal_GetPortAutoneg;	
	g_LW_Drv_Ops.p_Hal_SetPortSpeed=Hal_SetPortSpeed;	
	g_LW_Drv_Ops.p_Hal_SetPortDuplex=Hal_SetPortDuplex;	
	g_LW_Drv_Ops.p_Hal_SetPortSpdlx=Hal_SetPortSpdlx;	
	g_LW_Drv_Ops.p_Hal_SetPhyReg=Hal_SetPhyReg;	
	g_LW_Drv_Ops.p_Hal_GetPhyReg=Hal_GetPhyReg;
	//g_LW_Drv_Ops.p_Hal_SetEthdbg=Hal_SetEthdbg;
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
    g_LW_Drv_Ops.p_Hal_GetVlanMode=Hal_GetVlanMode;
	g_LW_Drv_Ops.p_Hal_SetVlanEntryCreate=Hal_SetVlanEntryCreate;
	g_LW_Drv_Ops.p_Hal_CheckVlanExist=Hal_CheckVlanExist;
	g_LW_Drv_Ops.p_Hal_SetVlanEntryDelete=Hal_SetVlanEntryDelete;	
	g_LW_Drv_Ops.p_Hal_SetVlanMemberRemove=Hal_SetVlanMemberRemove;
	g_LW_Drv_Ops.p_Hal_SetVlanMemberAdd=Hal_SetVlanMemberAdd;
    g_LW_Drv_Ops.p_Hal_SetVlanMember=Hal_SetVlanMember;
	g_LW_Drv_Ops.p_Hal_SetVlanPvid=Hal_SetVlanPvid;
    g_LW_Drv_Ops.p_Hal_GetVlanPvid=Hal_GetVlanPvid;
    g_LW_Drv_Ops.p_Hal_GetVlanPriority=Hal_GetVlanPriority;
    g_LW_Drv_Ops.p_Hal_SetVlanPriority=Hal_SetVlanPriority;
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
	g_LW_Drv_Ops.p_Hal_GetMacAgeTime=Hal_GetMacAgeTime; /*Add by huangmingjian 2013-09-07 for mac aging time*/
	g_LW_Drv_Ops.p_Hal_SetMacAgeTime=Hal_SetMacAgeTime;
	g_LW_Drv_Ops.p_Hal_SetMacLearnLimit=Hal_SetMacLearnLimit;
	g_LW_Drv_Ops.p_Hal_GetMacUcastMac=Hal_GetMacUcastMac;
	g_LW_Drv_Ops.p_Hal_GetMacMcastMac=Hal_GetMacMcastMac;
	g_LW_Drv_Ops.p_Hal_SetMacSetMcastMac=Hal_SetMacSetMcastMac;
	g_LW_Drv_Ops.p_Hal_SetMacLookFailFrd=Hal_SetMacLookFailFrd;
	g_LW_Drv_Ops.p_Hal_GetMacLearnCount=Hal_GetMacLearnCount;
	//g_LW_Drv_Ops.p_Hal_RefreshUCast=Hal_RefreshUCast;
	g_LW_Drv_Ops.p_Hal_SetReservedMacTrap=Hal_SetReservedMacTrap;
	g_LW_Drv_Ops.p_Hal_GetMacUcastDump=Hal_GetMacUcastDump;
	g_LW_Drv_Ops.p_Hal_GetMacMcastDump=Hal_GetMacMcastDump;
	g_LW_Drv_Ops.p_Hal_GetMacUcastDumpSorted=Hal_GetMacUcastDumpSorted;
	g_LW_Drv_Ops.p_Hal_SetMacTrapToCpu=Hal_SetMacTrapToCpu;
	g_LW_Drv_Ops.p_Hal_GetMacFindConflictMac=Hal_GetMacFindConflictMac;
	//g_LW_Drv_Ops.p_Hal_GetMacByIndex=Hal_GetMacByIndex;
	//g_LW_Drv_Ops.p_Hal_GetMacMoreByIndex=Hal_GetMacMoreByIndex;
	g_LW_Drv_Ops.p_Hal_GetMacExact=Hal_GetMacExact;
	g_LW_Drv_Ops.p_Hal_GetMcastCount=Hal_GetMcastCount;
	g_LW_Drv_Ops.p_Hal_SetMcastLookupMissDrop=Hal_SetMcastLookupMissDrop;
	g_LW_Drv_Ops.p_Hal_SetIgmpPktAction=Hal_SetIgmpPktAction;
	g_LW_Drv_Ops.p_Hal_SetPortIsolateMask=Hal_SetPortIsolateMask;
	g_LW_Drv_Ops.p_Hal_SetMirrorGroup=Hal_SetMirrorGroup;
    g_LW_Drv_Ops.p_Hal_GetMibPortStatisticsRaw=Hal_GetMibPortStatisticsRaw;
	g_LW_Drv_Ops.p_Hal_GetMibPortStatistics=Hal_GetMibPortStatistics;
	g_LW_Drv_Ops.p_Hal_SetMibCountSync=Hal_SetMibCountSync;
	g_LW_Drv_Ops.p_Hal_SetMibCountReset=Hal_SetMibCountReset;
	g_LW_Drv_Ops.p_Hal_GetPortCounter=Hal_GetPortCounter;
	g_LW_Drv_Ops.p_Hal_GetPortStatistics=Hal_GetPortStatistics;
    g_LW_Drv_Ops.p_Hal_SetEocLowLevelFunction=Hal_SetEocLowLevelFunction;
	g_LW_Drv_Ops.p_Hal_SetRatelimitInit=Hal_SetRatelimitInit;
    g_LW_Drv_Ops.p_Hal_SetRatelimitStormByPort=Hal_SetRatelimitStormByPort;
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
	//g_LW_Drv_Ops.p_Hal_SetEEEByPort=Hal_SetEEEByPort;
	//g_LW_Drv_Ops.p_Hal_SetAutoPwrDwnByPort=Hal_SetAutoPwrDwnByPort;
	g_LW_Drv_Ops.p_Hal_GetCableDiag=Hal_GetCableDiag;
	g_LW_Drv_Ops.p_Hal_GetStpPortState=Hal_GetStpPortState;
	g_LW_Drv_Ops.p_Hal_SetStpPortState=Hal_SetStpPortState;
	g_LW_Drv_Ops.p_Hal_L2send=Hal_L2send;
	g_LW_Drv_Ops.p_Hal_SetInternalLoopback=Hal_SetInternalLoopback;
	g_LW_Drv_Ops.p_Hal_GpioValGet=Hal_GpioValGet;
	g_LW_Drv_Ops.p_Hal_GpioOutputSet=Hal_GpioOutputSet;
	//g_LW_Drv_Ops.p_Hal_GpioMultiplexGet=Hal_GpioMultiplexGet;
	//g_LW_Drv_Ops.p_Hal_GpioMultiplexSet=Hal_GpioMultiplexSet;
	g_LW_Drv_Ops.p_Hal_GpioDirInSet=Hal_GpioDirInSet;
	g_LW_Drv_Ops.p_Hal_GpioDirOutSet=Hal_GpioDirOutSet;
	//g_LW_Drv_Ops.p_Hal_GpioTrigModeLevelSet=Hal_GpioTrigModeLevelSet;	
	//g_LW_Drv_Ops.p_Hal_GpioTrigModeEdgeSet=Hal_GpioTrigModeEdgeSet;
	//g_LW_Drv_Ops.p_Hal_GpioIntLevelHighSet=Hal_GpioIntLevelHighSet;
	//g_LW_Drv_Ops.p_Hal_GpioIntLevelLowSet=Hal_GpioIntLevelLowSet;
	//g_LW_Drv_Ops.p_Hal_GpioIntEdgeSet=Hal_GpioIntEdgeSet;
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
	g_LW_Drv_Ops.p_Hal_GetPortVlanIngressFilter=Hal_GetPortVlanIngressFilter;
	g_LW_Drv_Ops.p_Hal_GetPortVlanIngressMode=Hal_GetPortVlanIngressMode;
	g_LW_Drv_Ops.p_Hal_SetPortVlanEgressMode=Hal_SetPortVlanEgressMode;
    g_LW_Drv_Ops.p_Hal_GetPortVlanEgressMode=Hal_GetPortVlanEgressMode;
    g_LW_Drv_Ops.p_Hal_GetPortEgrTagKeepType=Hal_GetPortEgrTagKeepType;
	g_LW_Drv_Ops.p_Hal_AclRuleEmptyNumGet=Hal_AclRuleEmptyNumGet;
	g_LW_Drv_Ops.p_Hal_CfgPortCtcVlanGet=Hal_CfgPortCtcVlanGet;
	g_LW_Drv_Ops.p_Hal_CfgPortCtcVlanSet=Hal_CfgPortCtcVlanSet;	
	g_LW_Drv_Ops.p_Hal_RestartPortNeg=Hal_RestartPortNeg;
	g_LW_Drv_Ops.p_Hal_CtcVlanEnoughVlanIdxEntryCheck=Hal_CtcVlanEnoughVlanIdxEntryCheck;
	g_LW_Drv_Ops.p_Hal_SetUnKnowIp4McastAct=Hal_SetUnKnowIp4McastAct;
	//g_LW_Drv_Ops.p_Hal_GetFdbEntryByIndex=Hal_GetFdbEntryByIndex;
	g_LW_Drv_Ops.p_Hal_GetCvlanEntry=Hal_GetCvlanEntry;
	g_LW_Drv_Ops.p_Hal_GetSvlanEntry=Hal_GetSvlanEntry;
	g_LW_Drv_Ops.p_Hal_GetC2sEntryByIndex=Hal_GetC2sEntryByIndex;
	g_LW_Drv_Ops.p_Hal_GetSp2cEntryByIndex=Hal_GetSp2cEntryByIndex;
	g_LW_Drv_Ops.p_Hal_AddSp2cEntry2Port=Hal_AddSp2cEntry2Port;
	g_LW_Drv_Ops.p_Hal_DelSp2cEntryFromPort=Hal_DelSp2cEntryFromPort;
	g_LW_Drv_Ops.p_Hal_SetMcTransltVlanMemberAdd=Hal_SetMcTransltVlanMemberAdd;
	g_LW_Drv_Ops.p_Hal_SetMctransltVlanMemberRmv=Hal_SetMctransltVlanMemberRmv;
	g_LW_Drv_Ops.p_Hal_AddC2sEntry2Port=Hal_AddC2sEntry2Port;
	g_LW_Drv_Ops.p_Hal_DelC2sEntryFromPort=Hal_DelC2sEntryFromPort;
	g_LW_Drv_Ops.p_Hal_PortInit=Hal_PortInit;
    g_LW_Drv_Ops.p_Hal_SetVlanFilter=Hal_SetVlanFilter;
	g_LW_Drv_Ops.p_Hal_SetManageVlan=Hal_SetManageVlan;
	/*Begin add by shipeng 2013-11-11*/
	g_LW_Drv_Ops.p_Hal_SetWirelessUpServiceVlan=Hal_SetWirelessUpServiceVlan;
	/*End add by shipeng 2013-11-11*/
	g_LW_Drv_Ops.p_Hal_AclRuleForDropLoopdOperation=Hal_AclRuleForDropLoopdOperation;
	g_LW_Drv_Ops.p_Hal_AclRuleForDropLoopdRemove=Hal_AclRuleForDropLoopdRemove;
	g_LW_Drv_Ops.p_Hal_SetFecMode=Hal_SetFecMode;
	g_LW_Drv_Ops.p_Hal_GetFecMode=Hal_GetFecMode;
	g_LW_Drv_Ops.p_Hal_AclRuleEmptyIdGet=_Hal_AclRuleEmptyIdGet;
	g_LW_Drv_Ops.p_ACL_DeleteRuleByAclid=ACL_DeleteRuleByAclid;
	g_LW_Drv_Ops.p_Hal_GetMacLearnEnable=Hal_GetMacLearnEnable;
	g_LW_Drv_Ops.p_Hal_GetMcVlanMem=Hal_GetMcVlanMem;
	g_LW_Drv_Ops.p_Hal_VlanInterfaceAdd=Hal_VlanInterfaceAdd;
	g_LW_Drv_Ops.p_Hal_VlanInterfaceDelete=Hal_VlanInterfaceDelete;
	/*Begin add by huangmingjian 2013-08-27*/
	g_LW_Drv_Ops.p_Hal_GetEponMibCounter = Hal_GetEponMibCounter; 
	g_LW_Drv_Ops.p_Hal_EponMibGlobal_Reset = Hal_EponMibGlobal_Reset; 
	g_LW_Drv_Ops.p_Hal_GetPonmacTransceiver = Hal_GetPonmacTransceiver; 
	/*End add by huangmingjian 2013-08-27*/
	/*Begin add by huangmingjian 2014-01-13*/
#if defined(CONFIG_BOSA)
	g_LW_Drv_Ops.p_Hal_GetPonftoverflag = Hal_25l90FToverFlagGet;
	g_LW_Drv_Ops.p_Hal_SetPonftoverflag = Hal_25l90FToverFlagSet;
	g_LW_Drv_Ops.p_Hal_SetLaser = Hal_SetLaser;
	g_LW_Drv_Ops.p_Hal_GetLaser = Hal_GetLaser;
	g_LW_Drv_Ops.p_Hal_GetApcset = Hal_I2c25l90ApcGet;
	g_LW_Drv_Ops.p_Hal_SetApcset = Hal_I2c25l90ApcSet;
	g_LW_Drv_Ops.p_Hal_GetImodset = Hal_I2c25l90ImodGet;	
	g_LW_Drv_Ops.p_Hal_SetImodset = Hal_I2c25l90ImodSet;
#endif
	/*Begin add by huangmingjian 2014-01-13*/
	
	g_LW_Drv_Ops.p_Hal_ClfRuleForCtcClfPriToQueueCreate=Hal_ClfRuleForCtcClfPriToQueueCreate;
	g_LW_Drv_Ops.p_Hal_ClfRuleForCtcClfPriToQueueDelete=Hal_ClfRuleForCtcClfPriToQueueDelete;
	g_LW_Drv_Ops.p_Hal_ClfRuleForCtcClfPriToQueueClear=Hal_ClfRuleForCtcClfPriToQueueClear;
	g_LW_Drv_Ops.p_Hal_LookupMissFloodPortMaskSet = Hal_LookupMissFloodPortMaskSet;
    g_LW_Drv_Ops.p_Hal_SetPonLaserState = Hal_SetPonLaserState;
}

DRV_RET_E Hal_SwitchInit(void)
{

    const ether_addr_t stSwitchPauseMac = { {0x00, 0x0f, 0xe2, 0x12, 0x34, 0x56} };
	logic_pmask_t stLgcMask;
	logic_pmask_t stLgcMaskUntag;
	rtk_portmask_t stPPortMask;
	rtk_vlan_tagKeepType_t pType = TAG_KEEP_TYPE_CONTENT;
	rtk_led_config_t ledConfig;
	rtk_led_config_t ponLedConfig;
	unsigned int i;
	UINT32 uiLPortIndex;	
	phy_pmask_t stPhyMaskExt;

    rtk_port_linkStatus_t portStatus;

    //rtk_port_mac_ability_t mac_abi;
    rtk_portmask_t port_mask;
    #if defined (CHIPSET_RTL9607) || defined(CHIPSET_RTL9601)
    //extern void Hal_MdioRegisterInit(void);
    //extern void Hal_GpioInit(void);
    //Hal_MdioRegisterInit();
    //Hal_GpioInit();
    #endif
	Hal_CpuRegAccessInit();	

	Hal_Register();

    
    printk("\n\n\n\n\n liuanhua switch init ....\n\n\n\n\n\n\n");
	
    //rtk_SmiInit();
    /*013987 */
    
    (void)rtk_switch_init();
                        
    rtk_port_parm_init();
                        
    //rtk_shutdown_all_port();

    //port_mask.bits[0] = 0x0f;

    /*LED_OP_PARALLEL*/
    //(void)rtk_led_operation_set((rtk_led_operation_t)1);

    /*LED_GROUP_1*/
    //(void)rtk_led_enable_set((rtk_led_group_t)1, port_mask);

    /*LED_GROUP_1:LED_CONFIG_LINK_ACT*/
    //(void)rtk_led_groupConfig_set((rtk_led_group_t)1, (rtk_led_congig_t)2);
    
	(void)rtk_led_operation_set(LED_OP_PARALLEL);

	memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
	//ledConfig.ledEnable[LED_CONFIG_COL] = ENABLED;
	ledConfig.ledEnable[LED_CONFIG_TX_ACT] = ENABLED;
	ledConfig.ledEnable[LED_CONFIG_RX_ACT] = ENABLED;
	ledConfig.ledEnable[LED_CONFIG_SPD10ACT] = ENABLED;
	ledConfig.ledEnable[LED_CONFIG_SPD100ACT] = ENABLED;
	ledConfig.ledEnable[LED_CONFIG_SPD500ACT] = ENABLED;
	ledConfig.ledEnable[LED_CONFIG_SPD1000ACT] = ENABLED;
	//ledConfig.ledEnable[LED_CONFIG_DUP] = ENABLED;
	ledConfig.ledEnable[LED_CONFIG_SPD10] = ENABLED;
	ledConfig.ledEnable[LED_CONFIG_SPD100] = ENABLED;
	ledConfig.ledEnable[LED_CONFIG_SPD500] = ENABLED;
	ledConfig.ledEnable[LED_CONFIG_SPD1000] = ENABLED;
	//ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = ENABLED;

	memset(&ponLedConfig,0x0,sizeof(rtk_led_config_t));
	ponLedConfig.ledEnable[LED_CONFIG_TX_ACT] = ENABLED;
	ponLedConfig.ledEnable[LED_CONFIG_RX_ACT] = ENABLED;
	ponLedConfig.ledEnable[LED_CONFIG_PON_LINK] = ENABLED;
	//ponLedConfig.ledEnable[LED_CONFIG_PON_ALARM] = ENABLED;
	//ponLedConfig.ledEnable[LED_CONFIG_PON_WARNING] = ENABLED;
	//ponLedConfig.ledEnable[LED_CONFIG_FORCE_MODE] = ENABLED;
	
#if defined(CONFIG_PRODUCT_5500)
	(void)rtk_led_config_set(LAN0_LED,LED_TYPE_UTP0,&ledConfig);/*LAN0*/
	(void)rtk_led_parallelEnable_set(LAN0_LED,ENABLED);
	(void)rtk_led_config_set(LAN1_LED,LED_TYPE_UTP1,&ledConfig);/*LAN1*/
	(void)rtk_led_parallelEnable_set(LAN1_LED,ENABLED);
	(void)rtk_led_config_set(PON_LINK_LED,LED_TYPE_PON, &ponLedConfig);/*PON LINK*/
	//(void)rtk_led_parallelEnable_set(PON_LINK_LED,ENABLED);
	Hal_GpioDirOutSet(46);/*SYS LED*/

#elif defined(CONFIG_PRODUCT_EPN104N) || defined(CONFIG_PRODUCT_EPN104W) || defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN105) || defined(CONFIG_PRODUCT_EPN101ZG) || defined(CONFIG_PRODUCT_GPN104N)
	(void)rtk_led_config_set(LAN0_LED,LED_TYPE_UTP0,&ledConfig);/*LAN0*/
	(void)rtk_led_parallelEnable_set(LAN0_LED,ENABLED);
	#if !defined(CONFIG_PRODUCT_EPN101ZG)
	(void)rtk_led_config_set(LAN1_LED,LED_TYPE_UTP1,&ledConfig);/*LAN1*/
	(void)rtk_led_parallelEnable_set(LAN1_LED,ENABLED);
	(void)rtk_led_config_set(LAN2_LED,LED_TYPE_UTP2,&ledConfig);/*LAN2*/
	(void)rtk_led_parallelEnable_set(LAN2_LED,ENABLED);
	(void)rtk_led_config_set(LAN3_LED,LED_TYPE_UTP3,&ledConfig);/*LAN3*/
	(void)rtk_led_parallelEnable_set(LAN3_LED,ENABLED);
	#endif
	/*Begin modified for by huangmingjian 2014-01-17*/
	#if !defined(CONFIG_PRODUCT_EPN105)
	(void)rtk_led_config_set(PON_LINK_LED,LED_TYPE_PON, &ponLedConfig);/*PON LINK*/
	#else
	Hal_GpioDirOutSet(PON_LINK_GPIO);
	#endif
	/*End modified for by huangmingjian 2014-01-17*/
	//(void)rtk_led_parallelEnable_set(PON_LINK_LED,ENABLED);
  	rtk_led_parallelEnable_set(6,DISABLED);
    rtk_led_parallelEnable_set(8,DISABLED);
    rtk_gpio_state_set(13,DISABLED);
	rtk_gpio_state_set(24,DISABLED);
	
#elif defined(CONFIG_PRODUCT_EPN101R) 	
	(void)rtk_led_config_set(LAN2_LED,LED_TYPE_UTP2,&ledConfig);/*LAN2*/
	(void)rtk_led_parallelEnable_set(LAN2_LED,ENABLED);
	(void)rtk_led_config_set(PON_LINK_LED,LED_TYPE_PON, &ponLedConfig);/*PON LINK*/
	//(void)rtk_led_parallelEnable_set(9,ENABLED);
	Hal_GpioDirOutSet(46);/*SYS LED*/
#endif

	#ifdef ONU_STYLE
    (void)rtk_svlan_init();
	#endif 
    (void)rtk_vlan_init();	
    #ifdef ONU_STYLE
    /*Change TPID from 0x88a8 to 0x8100*/
    //(void)rtk_svlan_tpidEntry_set(0, 0x8100); //delete by luoruncai

    /*set uplink Port as service port*/
    //(void)rtk_svlan_servicePort_set(PHY_UPLINK_PORT, 1); //delete by luoruncai
	#endif
	/*Begin modified by sunmingliang for bug 260*/
	/*set vlan 0 learning mode ivl mode*/
	
	/*Begin modified by huangmingjian 2013-11-04:Bug 203,257*/	
	#ifdef ONU_STYLE
	rtk_vlan_fidMode_set(0, VLAN_FID_SVL);
	#else
	rtk_vlan_fidMode_set(0, VLAN_FID_IVL);
	#endif
	/*End modified by huangmingjian 2013-11-04:Bug 203,257*/	
	/*End modified by sunmingliang for bug 260*/
	/*set VID 0 and VID 4095 action*/
	(void)rtk_vlan_reservedVidAction_set(RESVID_ACTION_TAG,RESVID_ACTION_TAG);/*add by shipeng 2013-05-13*/
	#ifdef ONU_STYLE
	/*create svlan SVID 0*/
	//(void)rtk_svlan_create(0);/*add by shipeng 2013-04-28*/ //delete by luoruncai

    /*Unmatch packet assign to SVID 0*/
   //(void)rtk_svlan_unmatchAction_set(UNMATCH_ASSIGN,0); //delete by luoruncai
		
    /*Untag packet assign to SVID 0*/
    //(void)rtk_svlan_untagAction_set(UNTAG_ASSIGN,0); //delete by luoruncai
	#endif
	
    /*set cos to internal priority remarking*/
    for (i = 0; i < 7; i ++)
    {
       (void)rtk_qos_1pRemarkGroup_set(0,i,0,i);
    }
  
    rtk_port_phy_data_t uiVal;
    rtk_port_phy_ability_t stphybility;

    memset(&stphybility, 0, sizeof(stphybility));
    //stphybility.AutoNegotiation = 1;
    stphybility.Full_10 = 1;
    stphybility.Full_100 = 1;
    stphybility.Full_1000 = 1;
    stphybility.Half_10 = 1;
    stphybility.Half_100 = 1;
	stphybility.Half_1000 = 1;
 
	LgcPortFor(i)
    {	
    	if(PortLogic2PhyID(i) == 5 || PortLogic2PhyID(i) == 0)//cable port setting
    	{
			/*Cable Port , So Do Nothing*/
			rtk_port_phyForceModeAbility_set(i,2,1,1);// cable port set to 1000M Speed , Full duplex , flowControl enable
			printk(" === into the cable port config! === \n");
		}
		else
		{
	        /*disable flow ctrl*/
	        (void)rtk_port_phyReg_get(PortLogic2PhyID(i), 0, (rtk_port_phy_reg_t)4, &uiVal);
	        uiVal &= ~(1U << 10);
	        (void)rtk_port_phyReg_set(PortLogic2PhyID(i), 0, (rtk_port_phy_reg_t)4, uiVal);			

	        /* Close eee functionality. */
	        //(void)rtk_eee_portEnable_set((rtk_port_t)i, DISABLED);
	        (void)rtk_port_phyAutoNegoAbility_set(PortLogic2PhyID(i), &stphybility);
		}
    }


    /* Close green ethernet functionality. */
    //(void)rtk_switch_greenEthernet_set(DISABLED);   

    /*Add realtek header to broadcast arp/dhcp frame.*/	
	(void)rtk_cpu_init();/*add by shipeng 2013-05-22*/
//	(void)Hal_SetCpuHeader(TRUE);
    (void)Hal_SetCpuHeader(FALSE);


	(void)rtk_acl_init();/*add by shipeng 2013-05-22*/
    (void)Hal_AclRuleInit();

	#ifdef SWITCH_STYLE
	//(void)rtk_vlan_transparentEnable_set(ENABLED); delete by luoruncai
erw3r	//Hal_SetPortVlanIngressFilter(LOGIC_PON_PORT, FALSE);	 delete by luoruncai
	//Hal_SetPortVlanIngressMode(LOGIC_PON_PORT, PORT_IN_FRAM_BOTH); delete by luoruncai
	/*remove vlan 0*/
	//(void)rtk_vlan_destroy(0); delete by luoruncai
	/*set pon port to all uni port and all uni port to pon port transparent*/
	/*Modified by huangmingjian 2013-09-12 for let it to transfer untag packets on upstream*/	
	#if 0  //delete by luoruncai
	for (uiLPortIndex = 1; uiLPortIndex <= LOGIC_PORT_NO; uiLPortIndex++)
	{
		(void)_Hal_SetPortTransparent(LOGIC_UPPON_PORT, uiLPortIndex);
		(void)_Hal_SetPortTransparent(uiLPortIndex, LOGIC_UPPON_PORT);
	}
	#endif
	#endif

	#ifdef ONU_STYLE
	//(void)rtk_vlan_transparentEnable_set(ENABLED);/*add by shipeng 2013-04-28*/ //delete by luoruncai
    
    CTC_VLAN_CFG_S stVlanMode;

   // memset(&stVlanMode, 0, sizeof(stVlanMode));
    //stVlanMode.mode = CTC_VLAN_MODE_TRANSPARENT; //delete by luoruncai
    //for (uiLPortIndex = 1; uiLPortIndex <= LOGIC_PORT_NO; uiLPortIndex++)
    //{
    //    (void)Hal_SetPortTransparentMode(uiLPortIndex, &stVlanMode); //delete by luoruncai
    //}

	//(void)rtk_classify_init(); //delete by luoruncai

	//(void)Hal_ClfRuleInit(); //delete by luoruncai

	#endif

	#ifdef ONU_STYLE		
	/*remove cpu port from vlan 0*/
	ClrLgcMaskAll(&stLgcMask);
	ClrLgcMaskAll(&stLgcMaskUntag);
    SetLgcMaskBit(LOGIC_CPU_PORT, &stLgcMask);
	#if defined(CONFIG_PRODUCT_EPN104N)|| defined(CONFIG_PRODUCT_EPN101R) || defined(CONFIG_PRODUCT_EPN101ZG) || defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN105) || defined(CONFIG_PRODUCT_GPN104N)
	//Hal_SetVlanMemberRemove(0, stLgcMask);
	#elif defined(CONFIG_PRODUCT_EPN104W)
	#if 1
	Hal_SetVlanMemberRemove(0, stLgcMask);
	SetLgcMaskBit(LOGIC_PON_PORT, &stLgcMask);
	ClrPhyMaskAll(&stPhyMaskExt);
	SetPhyMaskBit(0, &stPhyMaskExt);
	Hal_SetVlanEntryCreate(wireless_up_service_vlan);
	Hal_SetVlanMemberAdd(wireless_up_service_vlan, stLgcMask, stLgcMaskUntag);
	rtk_vlan_extPort_set(wireless_up_service_vlan, &stPhyMaskExt.pmask[0]);
	Hal_AddSvlanMember(wireless_up_service_vlan, 0, stLgcMask, stLgcMaskUntag, 0);
	#else
	Hal_AddSvlanMember(0, 0, stLgcMask, stLgcMaskUntag, 0);
	#endif
	#endif
	#endif
	
	(void)rtk_vlan_portIgrFilterEnable_set(PortLogic2PhyID(LOGIC_CPU_PORT), DISABLED);	

	#ifdef ONU_STYLE
	/*set cpu port to all uni port transparent*/    //delete by luoruncai
	//for (uiLPortIndex = 1; uiLPortIndex <= LOGIC_PORT_NO; uiLPortIndex++)
    //{
    	//(void)rtk_switch_portMask_Clear(&stPPortMask);
    	//(void)rtk_vlan_portEgrTagKeepType_get(PortLogic2PhyID(uiLPortIndex), &stPPortMask, &pType);
		//stPPortMask.bits[0] |= 1U << PortLogic2PhyID(LOGIC_CPU_PORT);
        //(void)rtk_vlan_portEgrTagKeepType_set(PortLogic2PhyID(uiLPortIndex),&stPPortMask,TAG_KEEP_TYPE_CONTENT);
    //}
	#endif
	/*set cpu port to pon port transparent*/  //delete by luoruncai
	//(void)rtk_switch_portMask_Clear(&stPPortMask);
	//(void)rtk_vlan_portEgrTagKeepType_get(PortLogic2PhyID(LOGIC_PON_PORT), &stPPortMask, &pType);
	//stPPortMask.bits[0] |= 1U << PortLogic2PhyID(LOGIC_CPU_PORT);
    //(void)rtk_vlan_portEgrTagKeepType_set(PortLogic2PhyID(LOGIC_PON_PORT),&stPPortMask,TAG_KEEP_TYPE_CONTENT);	

	#ifdef CONFIG_PRODUCT_5500
	rtk_port_macAbility_t ext_macAbility;
	memset(&ext_macAbility, 0, sizeof(rtk_port_macAbility_t));
	ext_macAbility.speed = PORT_SPEED_1000M;
	ext_macAbility.duplex = PORT_FULL_DUPLEX;
	ext_macAbility.linkStatus = PORT_LINKUP;
	(void)rtk_port_macExtMode_set(5, EXT_MODE_RGMII);
	(void)rtk_port_macForceAbility_set(5, ext_macAbility);
	(void)rtk_port_macForceAbilityState_set(5, ENABLED);
	(void)rtk_port_macExtRgmiiDelay_set(5, 1, 3);/*rxdelay需要调试*/
	#endif
/* Set ext rgmii port can link up to switch. Add by Alan Lee.at 20140718 */
	#ifdef CONFIG_EOC_EXTEND
    #ifdef CONFIG_PRODUCT_EPN105   
	rtk_port_macAbility_t ext_macAbility;
	memset(&ext_macAbility, 0, sizeof(rtk_port_macAbility_t));
	ext_macAbility.speed = PORT_SPEED_1000M;
	ext_macAbility.duplex = PORT_FULL_DUPLEX;
	ext_macAbility.linkStatus = PORT_LINKUP;
	(void)rtk_port_macExtMode_set(5, EXT_MODE_RGMII);
	(void)rtk_port_macForceAbility_set(5, ext_macAbility);
	(void)rtk_port_macForceAbilityState_set(5, ENABLED);
	(void)rtk_port_macExtRgmiiDelay_set(5, 0, 3);/*rxdelay需要调试*/
    #endif
	#endif    
/* End */
	rtk_mac_t switch_mac;
#if 0
	memcpy(switch_mac.octet, stSwitchPauseMac.octet, ETHER_ADDR_LEN);
#else
	memcpy(switch_mac.octet, fe_addr, ETHER_ADDR_LEN);
#endif
	(void)rtk_switch_mgmtMacAddr_set(&switch_mac);

	rtk_l2_ucastAddr_t l2Addr;
	memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
#if defined(CONFIG_PRODUCT_5500)	
	l2Addr.port = 3;
#else
	l2Addr.port = 5;
#endif
	l2Addr.flags |= RTK_L2_UCAST_FLAG_STATIC;
	rtk_l2_addr_add(&l2Addr);

	//Hal_CpuRegWrite(0xBB023078, 0x0);/*Set backpressure off by shipeng 2013-11-05*/  //delete by luoruncai
	
	_Hal_PortMonitorInit();
/*Begin add by huangmingjian 2014-01-13*/
	(void)rtk_i2c_init(I2C_PORT_0);
#if defined(CONFIG_BOSA)
    (void)Hal_I2cPonMonitorInit();
#endif
/*End add by huangmingjian 2014-01-13*/ 

	/*Begin add by huangmingjian 2014-05-19 for Bug 587*/
	/*No set lookup miss flood for LOGIC_CPU_PORT==6*/
	UINT32 uiLPortId;
	UINT32 uiPPortMask;
	UINT32 uiPPort;
	rtk_portmask_t flood_portmask;
	
	LgcPortFor(uiLPortId)
    {
        uiPPort = PortLogic2PhyPortId(uiLPortId);
        uiPPortMask |= (1U << uiPPort);
    }
	uiPPort = PortLogic2PhyPortId(LOGIC_UPPON_PORT);
	uiPPortMask |= (1U << uiPPort);
	uiPPortMask |= (1U << 5);
	
	flood_portmask.bits[0] = uiPPortMask;
	//dal_apollomp_l2_lookupMissFloodPortMask_set(DLF_TYPE_UCAST, &flood_portmask);

	(void)ioal_mem32_write(0x1c0d4, 0x0);/* flowctrl set egress port 0 queue-id 0 queue-drop state enable??*/
	
	/*End add by huangmingjian 2014-05-19 for Bug 587*/
	
	__Hal_InitPonLaserMonior();

/* used to diag port status */
	for (i = 0 ; i < 6; i ++)
    {	
    	int ret;
		portStatus = 0;       
        ret = rtk_port_link_get(i, &portStatus);
        printk("rtk_port_link_get(%d) return %d, (status=%d)\n", i, ret, portStatus);
    }
    
/* end */

    return DRV_OK;
	

}

#ifdef  __cplusplus
}
#endif

