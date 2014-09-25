/*****************************************************************************
------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
  --------------------------------------------------------------------------
                                                                             
*****************************************************************************/
#ifndef _HAL_COMMON_H_
#define _HAL_COMMON_H_

#ifdef  __cplusplus
extern "C"{
#endif

#ifdef CHIPSET_RTL8328
#include <common/rt_type.h>
#endif
#include "lw_config.h"
#if 1
#include <common/rt_type.h>
#else
#include "rtk_api.h"
#endif
#include "lw_type.h"
#include "lw_drv_pub.h"
#include "lw_drv_req.h"

#define PHY_CPU_PORTID  PORTID(PORT_PHYID(LOGIC_CPU_PORT))
#define PHY_CPU_CHIPID  CHIPID(PORT_PHYID(LOGIC_CPU_PORT))


#define HAL_PORT_LINKUP_TYPE_DOWN      0
#define HAL_PORT_LINKUP_TYPE_COPPER_UP 1
#define HAL_PORT_LINKUP_TYPE_FIBER_UP  2



#if defined (CHIPSET_RTL8305) || defined (CHIPSET_RTL9607) || defined (CHIPSET_RTL9601)
#define RTL_HEADER_OFF        (2 * MAC_ADDR_LEN)
#define RTL_METER_ID_INVALID  0xFFFFFFFF

/* */
#if defined (CHIPSET_RTL9607) || defined (CHIPSET_RTL9601) 
#define PHY_UPLINK_PORT 4 /*modify by shipeng 2013-04-19*/
#elif defined (CHIPSET_RTL8305)
#define PHY_UPLINK_PORT 6
#endif
/* 014453 */

#define ACL_RULE_ID_VLAN_TRANSP (ACL_RULE_NUM_MAX - 1)
#define ACL_RULE_ID_LOOPD_TRAPCPU (ACL_RULE_NUM_MAX - 2)
typedef struct tagLW_DRV_OP_S
{	
	UINT32 (*p_fun_getporttype)(port_num_t lgcPort);
	
	DRV_RET_E (*p_fun_getportlinkuptype)(phyid_t phyid, UINT32 *pulLinkupType);
	
	DRV_RET_E (*p_Hal_SetPortMedium)(port_num_t lgcPort, UINT32 uiMedium);
	
	DRV_RET_E (*p_Hal_SetPortPriority)(port_num_t lgcPort, UINT32 uiPrio);
	
	DRV_RET_E (*p_Hal_SetPortMdix)(port_num_t lgcPort, UINT32 uiMdix);
	
	DRV_RET_E (*p_Hal_SetPortEnable)(port_num_t lgcPort, UINT32 uiEnable);
	
	DRV_RET_E (*p_Hal_GetPortEnable)(port_num_t lgcPort, UINT32 *uiEnable);

	DRV_RET_E (*p_Hal_SetPortJumbo)(port_num_t lgcPort, UINT32 uiJumbo);

	DRV_RET_E (*p_Hal_SetPortTxRx)(port_num_t lgcPort, UINT32 uiState);
	
	DRV_RET_E (*p_Hal_SetPortAbility)(port_num_t lgcPort, UINT32 uiAbility, UINT32 uiPortType);
	
	DRV_RET_E (*p_Hal_SetPortAutoneg)(port_num_t lgcPort, UINT32 uiAuton, UINT32 uiPortType);

	DRV_RET_E (*p_Hal_GetPortAutoneg)(port_num_t lgcPort, UINT32 *uiAuton);
	
	DRV_RET_E (*p_Hal_SetPortSpeed)(port_num_t lgcPort, UINT32 uiSpeed, UINT32 uiPortType);
	
	DRV_RET_E (*p_Hal_SetPortDuplex)(port_num_t lgcPort, UINT32 uiDuplex, UINT32 uiPortType);
	
	DRV_RET_E (*p_Hal_SetPortSpdlx)(port_num_t lgcPort, UINT32 uiSpdlx);
	
	DRV_RET_E (*p_Hal_SetPhyReg)(port_num_t lgcPort, UINT32 reg_no, UINT32 reg_val);
	
	DRV_RET_E (*p_Hal_GetPhyReg)(port_num_t lgcPort, UINT32* pRegVal, UINT32 reg_no);

	DRV_RET_E (*p_Hal_SetEthdbg)(UINT32 eth_dbg);
	
	DRV_RET_E (*p_Hal_GetSpdlxSet)(port_num_t lgcPort, UINT32* uiSpdlx);
	
	DRV_RET_E (*p_Hal_GetIsolateSet)(logic_pmask_t *pstPortMask);
	DRV_RET_E (*p_Hal_AclRuleForEoamFilterCreate)(UINT32 uiLPortId, 
                                   UINT32 uiRulePrecedence,
                                   ACL_TRUST_MODE_E uiAclRuleType,
                                   ACL_DIRECTION_E enAclDir,
                                   VOID *pRuleValue);
	
	DRV_RET_E (*p_Hal_AclRuleForEoamFilterDelete)(UINT32 uiLPortId, UINT32 uiRulePrecedence, ACL_DIRECTION_E enAclDir);
	DRV_RET_E (*p_Hal_AclRuleForCtcClfRmkCreate)(UINT32 uiLPortId, 
                                   UINT32 uiRulePrecedence,
                                   ACL_TRUST_MODE_E uiAclRuleType,
                                   VOID *pRuleValue,
                                   VOID *pRemarkPri);
	
	DRV_RET_E (*p_Hal_AclRuleForCtcClfRmkDelete)(UINT32 uiLPortId, UINT32 uiRulePrecedence);
	
	DRV_RET_E (*p_Hal_AclRuleForCtcClfRmkClear)(UINT32 uiLPortId);
	
	DRV_RET_E (*p_Hal_SetPortFlowctrl)(port_num_t lgcPort, UINT32 uiPauseTx, UINT32 uiPauseRx);
	
	DRV_RET_E (*p_Hal_GetPortMedium)(port_num_t lgcPort, UINT32 *puiMedium);
	
	DRV_RET_E (*p_Hal_GetPortCurrentLink)(port_num_t lgcPort, UINT32 *puiLink);
	
	DRV_RET_E (*p_Hal_GetPortAbility)(port_num_t lgcPort, UINT32 *puiAbility, UINT32 uiPortType);
	
	DRV_RET_E (*p_Hal_GetPortSpeed)(port_num_t lgcPort, UINT32 *puiSpeed);
	
	DRV_RET_E (*p_Hal_GetPortDuplex)(port_num_t lgcPort, UINT32 *puiDuplex);
	
	DRV_RET_E (*p_Hal_GetPortLinkMask)(logic_pmask_t *pstPortMask);
	
	DRV_RET_E (*p_Hal_GetPortMediaFiberMask)(logic_pmask_t *pstPortMask);
	
	DRV_RET_E (*p_Hal_GetPortMdix)(port_num_t lgcPort, UINT32 *puiMdix);
	
	DRV_RET_E (*p_Hal_GetPortPriority)(port_num_t lgcPort, UINT32 *puiPrio);
	
	DRV_RET_E (*p_Hal_GetPortFlowctrl)(port_num_t lgcPort, UINT32 *puiFlowctrl);
	
	DRV_RET_E (*p_Hal_GetPortJumbo)(port_num_t lgcPort, UINT32 *puiJumbo);
	
	DRV_RET_E (*p_Hal_GetPortMaxFrameLen)(port_num_t lgcPort, UINT32 *puiLen);
	
	DRV_RET_E (*p_Hal_SetPortMaxFrameLen)(port_num_t lgcPort, UINT32 uiLen);
	
	DRV_RET_E (*p_Hal_PortLedInit)(void);
	
	DRV_RET_E (*p_Hal_SetPortLed)(port_num_t lgcPort, UINT32 uiLinkState, UINT32 uiPortType);

	DRV_RET_E (*p_Hal_I2cFiberInit)();

	DRV_RET_E (*p_Hal_I2cFiberPortRead)(port_num_t lgcPort, UINT16 usAddr, UINT8* pucData, UINT32 ulLen, UINT32 ulPDataType);
	
	DRV_RET_E (*p_Hal_SetAggrMode)(UINT32 ulMode);
	
	DRV_RET_E (*p_Hal_SetAggrGroup)(agg_grp_num_t grpNum, logic_pmask_t stLgcMask);
	
	DRV_RET_E (*p_Hal_GetVlanExistNum)(UINT32 *puiNum);
	
	DRV_RET_E (*p_Hal_SetVlanMode)(UINT32 ulMode);
	
	DRV_RET_E (*p_Hal_SetVlanEntryCreate)(UINT32 ulVlanId);
	DRV_RET_E (*p_Hal_SetVlanEntryDelete)(UINT32 ulVlanId);	
	DRV_RET_E (*p_Hal_SetVlanMemberRemove)(UINT32 ulVlanId, logic_pmask_t stLgcMask);
	
	DRV_RET_E (*p_Hal_SetVlanMemberAdd)(UINT32 ulVlanId, logic_pmask_t stLgcMask, logic_pmask_t stLgcMaskUntag);
	
	DRV_RET_E (*p_Hal_SetVlanPvid)(UINT32 ulLgcPortNumber, UINT32 ulPvid);
	
	DRV_RET_E (*p_Hal_SetVlanPortVlanMember)(UINT32 ulLgcPortNumber, logic_pmask_t stLgcMask);
	
	DRV_RET_E (*p_Hal_GetVlanMember)(UINT32 ulVlanId, logic_pmask_t *pstLgcMask, logic_pmask_t *pstLgcMaskUntag);
	
	DRV_RET_E (*p_Hal_AddCpuToVlanMember)(UINT32 ulVlanId);
	
	DRV_RET_E (*p_Hal_RemoveCpuFromVlanMember)(UINT32 ulVlanId);
	
	DRV_RET_E (*p_Hal_SetCpuPortMacLearnEnable)(UINT32 ulEnable);
	
	DRV_RET_E (*p_Hal_SetMacLearnEnable)(UINT32 ulEnable, logic_pmask_t stPortMask);
	
	DRV_RET_E (*p_Hal_SetMacCpuMacAdd)(vlan_id_t tdVid, mac_address_t mac_address);
	
	DRV_RET_E (*p_Hal_SetMacFlushUcastMac)(mac_delete_t stMacDelete);
	DRV_RET_E (*p_Hal_SetMacAddUcastMac)(mac_ucast_t mac_ucast);
	
	DRV_RET_E (*p_Hal_SetMacDelUcastMac)(mac_ucast_t mac_ucast);
	
	DRV_RET_E (*p_Hal_SetMacAddMcastMac)(mac_mcast_t mac_mcast);
	
	DRV_RET_E (*p_Hal_SetMacDelMcastMac)(mac_mcast_t mac_mcast);
	
	DRV_RET_E (*p_Hal_GetMacAgeTime)(UINT32 *pulSecond);
		
	DRV_RET_E (*p_Hal_SetMacAgeTime)(UINT32 ulSecond);
	
	DRV_RET_E (*p_Hal_SetMacLearnLimit)(UINT32 ulLgcPort, UINT32 uLlimit, UINT32 ulDisForward);
	
	DRV_RET_E (*p_Hal_GetMacUcastMac)(vlan_id_t tdVid, mac_address_t mac_addr, mac_ucast_t *pstMacUcast);
	
	DRV_RET_E (*p_Hal_GetMacMcastMac)(vlan_id_t tdVid, mac_address_t mac_addr, mac_mcast_t *pstMacMcast);
	
	DRV_RET_E (*p_Hal_SetMacSetMcastMac)(mac_mcast_t stMacMcast);
	
	DRV_RET_E (*p_Hal_SetMacLookFailFrd)(logic_pmask_t stPortMask);
	
	DRV_RET_E (*p_Hal_GetMacLearnCount)(UINT32 ulLgcPort, UINT32 *pulCount);

	DRV_RET_E (*p_Hal_RefreshUCast)(UINT32 ulInterval);
	
	DRV_RET_E (*p_Hal_SetReservedMacTrap)(UINT32 uiIndex, UINT32 priority, rsv_mac_pass_action_t action);

	DRV_RET_E (*p_Hal_GetMacUcastDump)(ULONG userPointer, UINT32 *pulCountOut, UINT32 ulPDataType);
	
	DRV_RET_E (*p_Hal_GetMacMcastDump)(ULONG userPointer, UINT32 *pulCountOut, UINT32 ulPDataType);
	
	DRV_RET_E (*p_Hal_GetMacUcastDumpSorted)(ULONG userPointer, UINT32 *pulCountOut, UINT32 ulPDataType);
	
	DRV_RET_E (*p_Hal_SetMacTrapToCpu)(port_num_t lport, mac_trap_set_t stTrapSet, UINT32 priority);
	
	DRV_RET_E (*p_Hal_GetMacFindConflictMac)(Mac_op_t stMacOp, Mac_op_t *pstMacOp);

	DRV_RET_E (*p_Hal_GetMacByIndex)(UINT32 ulStartIdx, mac_common_t * pstTmpMacEntry);

	DRV_RET_E (*p_Hal_GetMacMoreByIndex)(UINT32 ulStartIdx, UINT32 ulGetCount, ULONG userPointer, UINT32* pulCountOut, UINT32* pulNextIdx, UINT32 ulPDataType);
	
	DRV_RET_E (*p_Hal_GetMacExact)(vlan_id_t tdVid, mac_address_t mac_addr, mac_common_t * pstTmpMac);
	
	DRV_RET_E (*p_Hal_GetMcastCount)(UINT32 * pulMcastCount);

	DRV_RET_E (*p_Hal_SetMcastLookupMissDrop)(UINT32 ulEnable);
	
	DRV_RET_E (*p_Hal_SetIgmpPktAction)(UINT32 ulAction);
	
	DRV_RET_E (*p_Hal_SetPortIsolateMask)(logic_pmask_t *pstPortMask);
	
	DRV_RET_E (*p_Hal_SetMirrorGroup)(port_num_t mirrorDestPort, logic_pmask_t ingressMask, logic_pmask_t egressMask);

    DRV_RET_E (*p_Hal_GetMibPortStatisticsRaw)(port_num_t lport, UINT32 mibType, UINT64 *p_ullStat);
        
	DRV_RET_E (*p_Hal_GetMibPortStatistics)(port_num_t lport, MIB_IfStatType_t PktType, UINT64 *p_ullStat);
	
	DRV_RET_E (*p_Hal_SetMibCountSync)(void);
	
	DRV_RET_E (*p_Hal_SetMibCountReset)(port_num_t lport);
	
	DRV_RET_E (*p_Hal_GetPortCounter)(port_num_t lport, CounterName_E counterName, UINT64 *returnValue);
	
	DRV_RET_E (*p_Hal_GetPortStatistics)(port_num_t lport, stat_reg_t statisticsName, UINT64 *returnValue);

    DRV_RET_E (*p_Hal_SetEocLowLevelFunction)(eoc_low_level_t *peocLowLevel);
	
	DRV_RET_E (*p_Hal_SetRatelimitInit)(void);

    DRV_RET_E (*p_Hal_SetRatelimitStormByPort)(PORT_STORM_TYPE_E stormType, logic_pmask_t lPortMask, UINT32 ulKBps);

	DRV_RET_E (*p_Hal_SetRatelimitByPort)(UINT32 direction, logic_pmask_t lPortMask, UINT32 ulKBps);
	
	DRV_RET_E (*p_Hal_SetRatelimitStormCtl)(STORM_CTLTYPE_E ctlType, logic_pmask_t lPortMask, STORM_CTLRATE_S stStorm);
	
	DRV_RET_E (*p_Hal_SetQosInit)(void);
	
	DRV_RET_E (*p_Hal_Set1pIngressPriorityRemap)(qos_8021p_to_intpri_t st1pToIntPri);
	
	DRV_RET_E (*p_Hal_SetDscpIngressPriorityRemap)(qos_dscp_to_intpri_t stDscpToIntPri);
	
	DRV_RET_E (*p_Hal_SetQosTrustMode)(UINT32 mode);
	
	DRV_RET_E (*p_Hal_SetQosQueueSchedul)(QueueMode_S stQueueSchedule);
	
	DRV_RET_E (*p_Hal_SetCos2QidRemap)(qos_8021p_to_intpri_t st1pPri2qid);
	
	DRV_RET_E (*p_Hal_soc_reg_read)(UINT32 ulChipNum, UINT32 ulRegIndex, UINT32 * pulData);
	
	DRV_RET_E (*p_Hal_soc_reg_write)(UINT32 ulChipNum, UINT32 ulRegIndex, UINT32 ulData);

	DRV_RET_E (*p_Hal_CpuRegRead)(UINT32 regAddr,UINT32 *regVal);

	DRV_RET_E (*p_Hal_CpuRegWrite)(UINT32 regAddr,UINT32 regVal);

	DRV_RET_E (*p_Hal_SetEEEByPort)(logic_pmask_t *plportMask, BOOL enable);

	DRV_RET_E (*p_Hal_SetAutoPwrDwnByPort)(logic_pmask_t *plportMask, BOOL enable);
	
	DRV_RET_E (*p_Hal_GetCableDiag)(port_num_t lport, cable_diag_t *pstDiag);
	
	DRV_RET_E (*p_Hal_GetStpPortState)(port_num_t lport, UINT32 * pulState);
	
	DRV_RET_E (*p_Hal_SetStpPortState)(port_num_t lport, UINT32 ulState);
	
	DRV_RET_E (*p_Hal_L2send)(unsigned char *pMsg,UINT uiLen,l2_send_op *pOp);
	
	DRV_RET_E (*p_Hal_SetInternalLoopback)(port_num_t lgcPort, UINT32 uiEnable);
	
	DRV_RET_E (*p_Hal_GpioValGet)(UINT32 uiPin, UINT32 *puiRegValue);
	
	DRV_RET_E (*p_Hal_GpioOutputSet)(UINT32 uiPin, UINT32 uiRegValue);
	
	DRV_RET_E (*p_Hal_GpioMultiplexGet)(UINT32 uiPin, UINT32 * puiWorkMode);
	
	DRV_RET_E (*p_Hal_GpioMultiplexSet)(UINT32 uiPin, UINT32 uiWorkMode);
	
	DRV_RET_E (*p_Hal_GpioDirInSet)(UINT32 uiPin);
	
	DRV_RET_E (*p_Hal_GpioDirOutSet)(UINT32 uiPin);
	
	DRV_RET_E (*p_Hal_GpioTrigModeLevelSet)(UINT32 uiPin);	
	
	DRV_RET_E (*p_Hal_GpioTrigModeEdgeSet)(UINT32 uiPin);
	DRV_RET_E (*p_Hal_GpioIntLevelHighSet)(UINT32 uiPin);
	
	DRV_RET_E (*p_Hal_GpioIntLevelLowSet)(UINT32 uiPin);
	
	DRV_RET_E (*p_Hal_GpioIntEdgeSet)(UINT32 uiPin, UINT32 uiEdgeType);
	
	DRV_RET_E (*p_Hal_AddPortBaseVlanMemByIndx)(UINT32 uiEntryId, logic_pmask_t stLgcMask);
	
	DRV_RET_E (*p_Hal_DelPortBaseVlanMemByIndx)(UINT32 uiEntryId, logic_pmask_t stLgcMask);
	
	DRV_RET_E (*p_Hal_AddPortBaseVlanMemb)(UINT32 uiVlanId, logic_pmask_t stLgcMask);
	
	DRV_RET_E (*p_Hal_DelPortBaseVlanMemb)(UINT32 uiVlanId, logic_pmask_t stLgcMask);
	
	DRV_RET_E (*p_Hal_ClrPortBaseVlanEntryByVid)(UINT32 uiVlanId);
	
	DRV_RET_E (*p_Hal_SetMcVlanMemberAdd)(UINT32 ulVlanId, logic_pmask_t stLgcMask, logic_pmask_t stLgcMaskUntag);

	DRV_RET_E (*p_Hal_GetMcMacByMacAndFid)(UINT32 uiVid, mac_address_t mac_addr, mac_mcast_t *pstMacMcast);

	DRV_RET_E (*p_Hal_SetMcMacByMacAndFid)(mac_mcast_t stMacMcast);

	DRV_RET_E (*p_Hal_DelMcMacByMacAndFid)(mac_mcast_t stMacMcast);
	
	DRV_RET_E (*p_Hal_SetMcVlanMemberRmv)(UINT32 ulVlanId, logic_pmask_t stLgcMask);
	
	DRV_RET_E (*p_Hal_SetPortTransparentMode)(UINT32 uiLPort, CTC_VLAN_CFG_S *pstVlanMode);
	
	DRV_RET_E (*p_Hal_ResetPortTransparentMode)(UINT32 uiLPort);
	
	DRV_RET_E (*p_Hal_SetPortTagMode)(UINT32 uiLPort, CTC_VLAN_CFG_S *pstVlanMode);
	
	DRV_RET_E (*p_Hal_ResetPortTagMode)(UINT32 uiLPort);
	
	DRV_RET_E (*p_Hal_SetPortTranslationMode)(UINT32 uiLPort, CTC_VLAN_CFG_S *pstVlanMode);
	
	DRV_RET_E (*p_Hal_ResetPortTranslationMode)(UINT32 uiLPort);
	
	DRV_RET_E (*p_Hal_AclRuleForCtcMcVlanCreate)(UINT32 uiLPortId, UINT32 uiMcVlan);
	
	DRV_RET_E (*p_Hal_AclRuleForCtcMcVlanDelete)(UINT32 uiLPortId, UINT32 uiMcVlan);
	
	DRV_RET_E (*p_Hal_SetPortVlanIngressFilter)(UINT32 uiLPort, BOOL bEnable);
	
	DRV_RET_E (*p_Hal_SetPortVlanIngressMode)( UINT32 uiLPort, PORT_INGRESS_MODE_E enIngressMode);
	
	DRV_RET_E (*p_Hal_SetPortVlanEgressMode)( UINT32 uiLPort, PORT_EGRESS_MODE_E enEgressMode);
	
	
	DRV_RET_E (*p_Hal_AclRuleEmptyNumGet)(UINT32 *pAclEmptyNum);
	
	DRV_RET_E (*p_Hal_CfgPortCtcVlanGet)(UINT32 uiLPortId, CTC_VLAN_CFG_S *pstCtcVlanMode);
	
	DRV_RET_E (*p_Hal_CfgPortCtcVlanSet)(UINT32 uiLPortId, CTC_VLAN_CFG_S *pstCtcVlanMode);
	
	DRV_RET_E (*p_Hal_RestartPortNeg)(port_num_t lgcPort);
	
	DRV_RET_E (*p_Hal_CtcVlanEnoughVlanIdxEntryCheck)(CTC_VLAN_CFG_S *pstVlanMode, UINT32 *pbEnough);

	DRV_RET_E (*p_Hal_SetUnKnowIp4McastAct)(MCAST_UNKNOW_ACT_E enActMode);

	DRV_RET_E (*p_Hal_GetFdbEntryByIndex)(UINT32 uiIndex, UINT32 *puiFid, mac_common_t *pstCommMac);
	
	DRV_RET_E (*p_Hal_GetCvlanEntry)(UINT32 uiCvlanId, UINT32 *puiPhyMsk, UINT32 *puiPhyMskUntag, UINT32 *puiFid);
	
	DRV_RET_E (*p_Hal_GetSvlanEntry)(UINT32 uiSvlanId, UINT32 *puiPhyMsk, UINT32 *puiPhyMskUntag, UINT32 *puiFid);
	
	DRV_RET_E (*p_Hal_GetC2sEntryByIndex)(UINT32 uiIndex, UINT32 *puiCvid, UINT32 *puiSvid, UINT32 *puiPPortMsk);
	
	DRV_RET_E (*p_Hal_GetSp2cEntryByIndex)(UINT32 uiIndex, UINT32 *puiCvid, UINT32 *puiSvid, UINT32 *puiDstPPort);
	
	DRV_RET_E (*p_Hal_PortInit)(void);

    DRV_RET_E (*p_Hal_SetVlanFilter)(BOOL bEnable);
    
	DRV_RET_E (*p_Hal_SetManageVlan)(unsigned int uiVlanIndex);

	/*Begin add by shipeng 2013-11-11*/
	DRV_RET_E (*p_Hal_SetWirelessUpServiceVlan)(unsigned int uiVlanIndex);
	/*End add by shipeng 2013-11-11*/
	
	DRV_RET_E (*p_Hal_AclRuleForDropLoopdOperation)(UINT32 lgcPort, DROP_LOOPD_OP_E uiDropLoopdOp);
	
	DRV_RET_E (*p_Hal_AclRuleForDropLoopdRemove)(VOID);
	
	DRV_RET_E (*p_Hal_SetFecMode)(UINT32 BothState);
	
	DRV_RET_E (*p_Hal_GetFecMode)(UINT32 *pBothState);
	
	DRV_RET_E (*p_Hal_AclRuleEmptyIdGet)(UINT32 *puiAclRuleId);
	
	DRV_RET_E (*p_ACL_DeleteRuleByAclid)(UINT32 uiAclRuleId);
	DRV_RET_E (*p_Hal_GetMacLearnEnable)(UINT32 ulLgcPort, UINT32 *pulEnable);
	
	DRV_RET_E (*p_Hal_GetMcVlanMem)(UINT32 uiVid, logic_pmask_t *pstLPortMsk, logic_pmask_t *pstUntagLPortMsk);
	
	DRV_RET_E (*p_Hal_VlanInterfaceAdd)(UINT32 vid, mac_address_t mac_address);
	
	DRV_RET_E (*p_Hal_VlanInterfaceDelete)(UINT32 vid);
	DRV_RET_E (*p_Hal_AddSp2cEntry2Port)(UINT32 uiLPort, UINT32 ulSvid, UINT32 ulCvid);
	DRV_RET_E (*p_Hal_DelSp2cEntryFromPort)(UINT32 uiLPort, UINT32 ulSvid);
	
	DRV_RET_E (*p_Hal_AddC2sEntry2Port)(UINT32 uiLPort, UINT32 ulCvid, UINT32 ulSvid);
	
	DRV_RET_E (*p_Hal_DelC2sEntryFromPort)(UINT32 uiLPort, UINT32 ulCvid, UINT32 ulSvid);
	
	DRV_RET_E (*p_Hal_SetMcTransltVlanMemberAdd)(UINT32 usrvlan, UINT32 mvlan, logic_pmask_t stLgcMask, logic_pmask_t stLgcMaskUntag,logic_pmask_t stLgcMasksvlanUntag);
	
	DRV_RET_E (*p_Hal_SetMctransltVlanMemberRmv)(UINT32 usrvlan,UINT32 mvlan, logic_pmask_t stLgcMask);
	/*Begin add by huangmingjian 2013-08-27 for pon statistic*/
	DRV_RET_E (*p_Hal_GetEponMibCounter)(rtk_epon_counter_app_t *pCounter);
	DRV_RET_E (*p_Hal_EponMibGlobal_Reset)(VOID);
	DRV_RET_E (*p_Hal_GetPonmacTransceiver)(rtk_transceiver_parameter_type_app_t para_type, rtk_transceiver_data_app_t *pData);
	/*Begin add by huangmingjian 2013-08-27 for pon statistic*/
	
	/*Begin add by huangmingjian 2014-01-13*/
#if defined(CONFIG_BOSA)
	DRV_RET_E (*p_Hal_GetPonftoverflag)(UINT32 *FTOverFLAG);
	DRV_RET_E (*p_Hal_SetPonftoverflag)(UINT32 FTOverFLAG);
	DRV_RET_E (*p_Hal_SetLaser)(UINT32 uiAction);
	DRV_RET_E (*p_Hal_GetLaser)(UINT32 *uiAction);
	DRV_RET_E (*p_Hal_GetApcset)(UINT32 *pApcVal);
	DRV_RET_E (*p_Hal_SetApcset)(UINT32 uiApcVal);
	DRV_RET_E (*p_Hal_GetImodset)(UINT32 *pModVal);
	DRV_RET_E (*p_Hal_SetImodset)(UINT32 uiModVal);
#endif
	/*Begin add by huangmingjian 2014-01-13*/
	DRV_RET_E (*p_Hal_ClfRuleForCtcClfPriToQueueCreate)(UINT32 uiLPortId, 
                                   UINT32 uiRulePrecedence,
                                   VOID *pRemarkPri);
	
	DRV_RET_E (*p_Hal_ClfRuleForCtcClfPriToQueueDelete)(UINT32 uiLPortId, UINT32 uiRulePrecedence);
	
	DRV_RET_E (*p_Hal_ClfRuleForCtcClfPriToQueueClear)(UINT32 uiLPortId);
	DRV_RET_E (*p_Hal_LookupMissFloodPortMaskSet)(UINT32 type, rtk_portmask_t *pFlood_portmask);
	
}LW_DRV_OP_S;
extern LW_DRV_OP_S g_LW_Drv_Ops;

typedef struct tagRTL_HEADER
{
#ifdef CONFIG_CPU_BIG_ENDIAN
	UINT16 usEthType;
	UINT8  ucProtocol;
	UINT8  ucReason;
	UINT8  ucEFID:1;
	UINT8  ucFID:3;
    UINT8  ucPrioritySelect:1;
    UINT8  ucPriority:3;
    UINT8  ucKeep:1;
    UINT8  ucVSEL:1;
    UINT8  ucDisableLearn:1;
    UINT8  ucVIDX:5;
    UINT16 usPPortMask;
#else
#endif
}RTL_HEADER_S;

#define BITMAP_IS_SET(array, index)\
    (array[index/(sizeof(array[0]) << 3)] & (0x1 << (index%(sizeof(array[0]) << 3))))
#define BITMAP_IS_CLEAR(array, index)\
    !(array[index/(sizeof(array[0]) << 3)] & (0x1 << (index%(sizeof(array[0]) << 3))))

#define BITMAP_SET(array, index) do {\
    array[index/(sizeof(array[0]) << 3)] = \
    (array[index/(sizeof(array[0]) << 3)] | (0x1 << (index%(sizeof(array[0]) << 3)))); } while(0)
#define BITMAP_CLEAR(array, index) do {\
    array[index/(sizeof(array[0]) << 3)] = \
    (array[index/(sizeof(array[0]) << 3)] & ~((UINT32)(0x1 << (index%(sizeof(array[0]) << 3))))); } while(0)

#define BITMAP_RESET(dstArray, length) \
do { \
    memset(dstArray, 0, length*4); \
} while(0)

#define BITMAP_AND(dstArray, srcArray, length) \
do { \
    UINT32  array_index;\
    for (array_index = 0; array_index < length; array_index++)\
    {\
        dstArray[array_index] &= srcArray[array_index];\
    }\
} while(0)

#define BITMAP_OR(dstArray, srcArray, length) \
do { \
    UINT32  array_index;\
    for (array_index = 0; array_index < length; array_index++)\
    {\
        dstArray[array_index] |= srcArray[array_index];\
    }\
} while(0)

#define BITMAP_XOR(dstArray, srcArray, length) \
do { \
    UINT32  array_index;\
    for (array_index = 0; array_index < length; array_index++)\
    {\
        dstArray[array_index] ^= srcArray[array_index];\
    }\
} while(0)

#define BITMAP_REMOVE(dstArray, srcArray, length) \
do { \
    UINT32  array_index;\
    for (array_index = 0; array_index < length; array_index++)\
    {\
        dstArray[array_index] &= ~srcArray[array_index];\
    }\
} while(0)

#define BITMAP_REVERT(dstArray, length) \
do { \
    UINT32  array_index;\
    for (array_index = 0; array_index < length; array_index++)\
    {\
        dstArray[array_index] = ~dstArray[array_index];\
    }\
} while(0)

#define RTK_PORTMASK_WORD_SET(portmask, word, value) \
    ((portmask).bits[(word)] = value)
#define RTK_PORTMASK_WORD_GET(portmask, word) \
    ((portmask).bits[(word)])

#define RTK_PORTMASK_PORT_SET(portmask, port) \
do {\
    if ((port) <= PHY_PORT_NO) {BITMAP_SET((portmask).bits, (port));}\
} while (0);\

#define RTK_PORTMASK_PORT_CLEAR(portmask, port) \
do {\
    if ((port) <= PHY_PORT_NO) {BITMAP_CLEAR((portmask).bits, (port));}\
} while (0);\

#define RTK_PORTMASK_IS_PORT_SET(portmask, port) \
    (((port) <= PHY_PORT_NO)?BITMAP_IS_SET((portmask).bits, (port)): 0)
#define RTK_PORTMASK_IS_PORT_CLEAR(portmask, port) \
    (((port) <= PHY_PORT_NO)?BITMAP_IS_CLEAR((portmask).bits, (port)): 0)
    
#define RTK_PORTMASK_RESET(portmask) \
    BITMAP_RESET((portmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)

#define RTK_PORTMASK_GET_PORT_COUNT(portmask)\
    (bitop_numberOfSetBitsInArray((portmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST))

#define RTK_PORTMASK_OR(dstPortmask, srcPortmask) \
    BITMAP_OR((dstPortmask).bits, (srcPortmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_AND(dstPortmask, srcPortmask) \
    BITMAP_AND((dstPortmask).bits, (srcPortmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_XOR(dstPortmask, srcPortmask) \
    BITMAP_XOR((dstPortmask).bits, (srcPortmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_REMOVE(dstPortmask, srcPortmask) \
    BITMAP_REMOVE((dstPortmask).bits, (srcPortmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#define RTK_PORTMASK_REVERT(portmask) \
    BITMAP_REVERT((portmask).bits, RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST)
#endif

#define LOGIC_PORT_MASK_CPARE(_lportmask1,_lportmask2,result) \
do { \
    UINT i; \
    result = TRUE; \
    for (i = 0; i < (PORT_MASK_WORD_MAX); i++) \
    { \
        if (_lportmask1.pbits[i] != _lportmask2.pbits[i]) \
        { \
            result = FALSE; \
            break; \
        } \
    } \
} while (0);


typedef struct tagPhyMask{
    rtk_portmask_t pmask[CHIP_NO];
}phy_pmask_t;

#define CHIPNMASK(_n,_phymask)    (((phy_pmask_t *)(_phymask))->pmask[(_n)])
#define PortLogic2PhyPortId(_lport)     PORTID(PortLogic2PhyID((_lport)))
#define PortLogci2ChipId(_lport)        CHIPID(PortLogic2PhyID((_lport)))


extern void ClrPhyMaskAll(phy_pmask_t *phyPMask_ptr);
extern void ClrPhyMaskBit(phyid_t uiPhyId, phy_pmask_t *phyPMask_ptr);
extern DRV_RET_E ErrorSdk2Drv(INT32 iRtkError);
extern BOOL IsValidPhyID(phyid_t uiPhyID);
extern void MaskLogic2Phy(logic_pmask_t *lgcPMask_ptr, phy_pmask_t *phyPMask_ptr);
extern void MaskPhy2Logic(phy_pmask_t *phyPMask_ptr, logic_pmask_t *lgcPMask_ptr);
extern void PhyMaskAddCpuPort(phy_pmask_t *mska);
extern void PhyMaskAddStackPort(phy_pmask_t *mska);
extern void PhyMaskAnd(phy_pmask_t *mska,phy_pmask_t *mskb);
extern void PhyMaskCopy(phy_pmask_t *dstMsk,phy_pmask_t *srcMsk);
extern void PhyMaskNegate(phy_pmask_t *mska,phy_pmask_t *mskb);
extern void PhyMaskOr(phy_pmask_t *mska,phy_pmask_t *mskb);
extern void PhyMaskRemove(phy_pmask_t *mska,phy_pmask_t *mskb);
extern void PhyMaskRemoveCpuPort(phy_pmask_t *mska);
extern void PhyMaskXor(phy_pmask_t *mska,phy_pmask_t *mskb);
extern phyid_t PortLogic2PhyID(port_num_t ucLogicPort);
extern port_num_t PortPhyID2Logic(phyid_t uiPhyId);
extern void SetPhyMaskAll(phy_pmask_t *phyPMask_ptr);
extern void SetPhyMaskBit(phyid_t uiPhyId, phy_pmask_t *phyPMask_ptr);
extern BOOL TstPhyMaskBit(phyid_t uiPhyId, phy_pmask_t *phyPMask_ptr);
extern phyid_t PhyGetStackPort(UINT32 ulUnit, UINT32 ulStackIndex);
extern BOOL IsStackPort(phyid_t uiPhyId);

/*hal drv port*/

UINT32 Hal_GetPortType(port_num_t lgcPort);

DRV_RET_E Hal_GetPortLinkupType(phyid_t phyid, UINT32 *pulLinkupType);

DRV_RET_E Hal_GetPortAbility(port_num_t lgcPort, UINT32 *puiAbility, UINT32 uiPortType);
DRV_RET_E Hal_GetPortCurrentLink(port_num_t lgcPort, UINT32 *puiLink);
DRV_RET_E Hal_GetPortDuplex(port_num_t lgcPort, UINT32 *puiDuplex);
DRV_RET_E Hal_GetPortFlowctrl(port_num_t lgcPort, UINT32 *puiFlowctrl);
DRV_RET_E Hal_GetPortEnable(port_num_t lgcPort, UINT32 *uiEnable);
DRV_RET_E Hal_GetPortJumbo(port_num_t lgcPort, UINT32 *puiJumbo);
DRV_RET_E Hal_GetPortMaxFrameLen(port_num_t lgcPort, UINT32 *puiLen);
/* 014538 */
DRV_RET_E Hal_SetPortMaxFrameLen(port_num_t lgcPort, UINT32 uiLen);
/*014538 */
DRV_RET_E Hal_GetPortLinkMask(logic_pmask_t *pstPortMask);
DRV_RET_E Hal_GetPortMediaFiberMask(logic_pmask_t *pstPortMask);
DRV_RET_E Hal_GetPortMdix(port_num_t lgcPort, UINT32 *puiMdix);

DRV_RET_E Hal_SetPortMedium(port_num_t lgcPort, UINT32 uiMedium);
DRV_RET_E Hal_GetPortMedium(port_num_t lgcPort, UINT32 *puiMedium);
DRV_RET_E Hal_GetPortPriority(port_num_t lgcPort, UINT32 *puiPrio);
DRV_RET_E Hal_GetPortSpeed(port_num_t lgcPort, UINT32 *puiSpeed);
DRV_RET_E Hal_SetPortAbility(port_num_t lgcPort, UINT32 uiAbility, UINT32 uiPortType);
DRV_RET_E Hal_SetPortAutoneg(port_num_t lgcPort, UINT32 uiAuton, UINT32 uiPortType);

/*Begin add by shipeng 2013/05/27 for CTC*/ 
DRV_RET_E Hal_GetPortAutoneg(port_num_t lgcPort, UINT32 *uiAuton);
/*End add by shipeng 2013/05/27 for CTC*/ 

DRV_RET_E Hal_SetPortDuplex(port_num_t lgcPort, UINT32 uiDuplex, UINT32 uiPortType);
DRV_RET_E Hal_SetPortEnable(port_num_t lgcPort, UINT32 uiEnable);
DRV_RET_E Hal_SetPortFlowctrl(port_num_t lgcPort, UINT32 uiPauseTx, UINT32 uiPauseRx);
DRV_RET_E Hal_SetPortJumbo(port_num_t lgcPort, UINT32 uiJumbo);
DRV_RET_E Hal_SetPortMdix(port_num_t lgcPort, UINT32 uiMdix);
DRV_RET_E Hal_SetPortPriority(port_num_t lgcPort, UINT32 uiPrio);
DRV_RET_E Hal_SetPortSpeed(port_num_t lgcPort, UINT32 uiSpeed, UINT32 uiPortType);
DRV_RET_E Hal_SetPortTxRx(port_num_t lgcPort, UINT32 uiState);
DRV_RET_E Hal_SetPortIsolateMask(logic_pmask_t *pstPortMask);
DRV_RET_E Hal_PortLedInit(void);
DRV_RET_E Hal_SetPortLed(port_num_t lgcPort, UINT32 uiLinkState, UINT32 uiPortType);
DRV_RET_E Hal_FiberPortCfgInit(port_num_t lgcPort, COMBO_PORT_CONFIG_S *pstCfg);
DRV_RET_E Hal_CopperPortCfgInit(port_num_t lgcPort, COMBO_PORT_CONFIG_S *pstCfg);
DRV_RET_E Hal_GetFiberSpeed(port_num_t lgcPort, UINT32 *puiSpeed);


DRV_RET_E Hal_SetPortSpdlx(port_num_t lgcPort, UINT32 uiSpdlx);
DRV_RET_E Hal_SetPhyReg(port_num_t lgcPort, UINT32 reg_no, UINT32 reg_val);
DRV_RET_E Hal_GetPhyReg(port_num_t lgcPort, UINT32* pRegVal, UINT32 reg_no);
DRV_RET_E Hal_GetSpdlxSet(port_num_t lgcPort, UINT32* uiSpdlx);
DRV_RET_E Hal_GetIsolateSet(logic_pmask_t *pstPortMask);


/*hal drv aggr*/
DRV_RET_E Hal_SetAggrMode(UINT32 ulMode);
DRV_RET_E Hal_SetAggrGroup(agg_grp_num_t grpNum, logic_pmask_t stLgcMask);

/*hal drv vlan*/
DRV_RET_E Hal_GetVlanExistNum(UINT32 *puiNum);
DRV_RET_E Hal_SetVlanMode(UINT32 ulMode);
DRV_RET_E Hal_SetVlanEntryCreate(UINT32 ulVlanEntry);
DRV_RET_E Hal_SetVlanEntryDelete(UINT32 ulVlanEntry);
DRV_RET_E Hal_SetVlanMemberRemove(UINT32 ulVlanId, logic_pmask_t stLgcMask);
DRV_RET_E Hal_SetVlanMemberAdd(UINT32 ulVlanId, logic_pmask_t stLgcMask, logic_pmask_t stLgcMaskUntag);
DRV_RET_E Hal_SetMcVlanMemberAdd(UINT32 ulVlanId, logic_pmask_t stLgcMask, logic_pmask_t stLgcMaskUntag);
DRV_RET_E Hal_SetVlanPvid(UINT32 ulLgcPortNumber, UINT32 ulPvid);
DRV_RET_E Hal_SetVlanPortVlanMember(UINT32 ulLgcPortNumber, logic_pmask_t stLgcMask);
DRV_RET_E Hal_GetVlanMember(UINT32 ulVlanId, logic_pmask_t * pstLgcMask, logic_pmask_t * pstLgcMaskUntag);
DRV_RET_E Hal_AddCpuToVlanMember(UINT32 ulVlanId);
DRV_RET_E Hal_RemoveCpuFromVlanMember(UINT32 ulVlanId);
#if 1

DRV_RET_E Hal_AclRuleForCtcTranspCreate(UINT32 uiLPortId, CTC_VLAN_CFG_S *pstVlanMode);
DRV_RET_E Hal_AclRuleForCtcTranspDelete(UINT32 uiLPortId);
DRV_RET_E Hal_AclRuleForCtcTagCreate(UINT32 uiLPortId, CTC_VLAN_CFG_S *pstVlanCfg);
DRV_RET_E Hal_AclRuleForCtcTagDelete(UINT32 uiLPortId);
DRV_RET_E Hal_AclRuleForCtcTranslCreate(UINT32 uiLPortId, CTC_VLAN_CFG_S *pstVlanMode);
DRV_RET_E Hal_AclRuleForCtcTranslDelete(UINT32 uiLPortId);
DRV_RET_E Hal_CfgPortCtcVlanSet(UINT32 uiLPortId, CTC_VLAN_CFG_S *pstCtcVlanMode);
DRV_RET_E Hal_DelPortBaseVlanMemb(UINT32 uiVlanId, logic_pmask_t stLgcMask);
DRV_RET_E Hal_AddPortBaseVlanMemb(UINT32 uiVlanId, logic_pmask_t stLgcMask);
DRV_RET_E Hal_AddPortBaseVlanMemByIndx(UINT32 uiEntryId, logic_pmask_t stLgcMask);
DRV_RET_E Hal_DelPortBaseVlanMemByIndx(UINT32 uiEntryId, logic_pmask_t stLgcMask);
DRV_RET_E Hal_ClrPortBaseVlanEntryByVid(UINT32 uiVlanId);
DRV_RET_E Hal_SetPortVlanIngressFilter(UINT32 uiLPort, BOOL bEnable);
DRV_RET_E Hal_SetPortVlanIngressMode( UINT32 uiLPort, PORT_INGRESS_MODE_E enIngressMode);
DRV_RET_E Hal_SetPortVlanEgressMode( UINT32 uiLPort, PORT_EGRESS_MODE_E enEgressMode);
DRV_RET_E Hal_AclRuleEmptyNumGet(UINT32 *pAclEmptyNum);
DRV_RET_E Hal_CtcVlanEnoughVlanIdxEntryCheck(CTC_VLAN_CFG_S *pstVlanMode, UINT32 *pbEnough);
//#ifdef CTC_MULTICAST_SURPORT
#if 1
DRV_RET_E Hal_AclRuleForCtcMcVlanCreate(UINT32 uiLPortId, UINT32 uiMcVlan);
DRV_RET_E Hal_AclRuleForCtcMcVlanDelete(UINT32 uiLPortId, UINT32 uiMcVlan);
#endif
#endif
/*hal drv mac*/
DRV_RET_E Hal_SetCpuPortMacLearnEnable(UINT32 ulEnable);
DRV_RET_E Hal_SetMacLearnEnable(UINT32 ulEnable, logic_pmask_t stPortMask);
DRV_RET_E Hal_GetMacLearnEnable(UINT32 ulLgcPort, UINT32 *pulEnable);
DRV_RET_E Hal_SetMacCpuMacAdd(vlan_id_t tdVid, mac_address_t mac_address);
DRV_RET_E Hal_SetMacFlushUcastMac(mac_delete_t stMacDelete);
DRV_RET_E Hal_SetMacAddUcastMac(mac_ucast_t mac_ucast);
DRV_RET_E Hal_SetMacDelUcastMac(mac_ucast_t mac_ucast);
DRV_RET_E Hal_SetMacAddMcastMac(mac_mcast_t mac_mcast);
DRV_RET_E Hal_SetMacDelMcastMac(mac_mcast_t mac_mcast);
DRV_RET_E Hal_GetMacAgeTime(UINT32 *pulSecond);
DRV_RET_E Hal_SetMacAgeTime(UINT32 ulSecond);
DRV_RET_E Hal_SetMacLearnLimit(UINT32 ulLgcPort, UINT32 uLlimit, UINT32 ulDisForward);
DRV_RET_E Hal_GetMacUcastMac(vlan_id_t tdVid, mac_address_t mac_addr, mac_ucast_t *pstMacUcast);
DRV_RET_E Hal_GetMacMcastMac(vlan_id_t tdVid, mac_address_t mac_addr, mac_mcast_t *pstMacMcast);
DRV_RET_E Hal_SetMacSetMcastMac(mac_mcast_t stMacMcast);
DRV_RET_E Hal_SetMacLookFailFrd(logic_pmask_t stPortMask);
DRV_RET_E Hal_GetMacLearnCount(UINT32 ulLgcPort, UINT32 *pulCount);
DRV_RET_E Hal_RefreshUCast(UINT32 ulInterval);

DRV_RET_E Hal_GetMacUcastDump(ULONG userPointer, UINT32 *pulCountOut, UINT32 ulPDataType);
DRV_RET_E Hal_GetMacNextValidUcast(UINT32 uiStartIndex, mac_dump_ucast_tab_t* pstUcastEntry, UINT32 *puiCurrentIndex);
DRV_RET_E Hal_GetMacMcastDump(ULONG userPointer, UINT32 *pulCountOut, UINT32 ulPDataType);
DRV_RET_E Hal_GetMacNextValidMcast(UINT32 uiStartIndex, mac_mcast_t* pstMcastEntry, UINT32 *puiCurrentIndex);
DRV_RET_E Hal_GetMacUcastDumpSorted(ULONG userPointer, UINT32 *pulCountOut, UINT32 ulPDataType);
DRV_RET_E Hal_SetMacTrapToCpu(port_num_t lport, mac_trap_set_t stTrapSet, UINT32 priority);
DRV_RET_E Hal_GetMacFindConflictMac(Mac_op_t stMacOp, Mac_op_t *pstMacOp);

DRV_RET_E Hal_SetReservedMacTrap(UINT32 uiIndex, UINT32 priority, rsv_mac_pass_action_t action);

DRV_RET_E Hal_GetMacByIndex(UINT32 ulStartIdx, mac_common_t * pstTmpMacEntry);
DRV_RET_E Hal_GetMacMoreByIndex(UINT32 ulStartIdx, UINT32 ulGetCount,
          ULONG userPointer, UINT32* pulCountOut, UINT32* pulNextIdx, UINT32 ulPDataType);

DRV_RET_E Hal_GetMacExact(vlan_id_t tdVid, mac_address_t mac_addr, mac_common_t * pstTmpMac);
DRV_RET_E Hal_GetMcastCount(UINT32 * pulMcastCount);
DRV_RET_E Hal_SetMcastLookupMissDrop(UINT32 ulEnable); 
DRV_RET_E Hal_SetIgmpPktAction(UINT32 ulAction);
DRV_RET_E Hal_SetUnKnowIp4McastAct(MCAST_UNKNOW_ACT_E enActMode);
/*hal drv mirror*/
DRV_RET_E Hal_SetMirrorGroup(port_num_t mirrorDestPort, logic_pmask_t ingressMask, logic_pmask_t egressMask);
void Hal_InitMirrorAclGrp(void);
DRV_RET_E Hal_SetMirrorAclGrp(port_num_t mirrorDstPort, UINT32 *puiMirrorGrpId);
DRV_RET_E Hal_DelMirrorAclGrp(port_num_t mirrorDstPort);

/*hal drv statistics*/
DRV_RET_E Hal_GetMibPortStatistics(port_num_t lport, MIB_IfStatType_t PktType, UINT64 *p_ullStat);
DRV_RET_E Hal_SetMibCountSync(void);
DRV_RET_E Hal_SetMibCountReset(port_num_t lport);
DRV_RET_E Hal_GetPortCounter(port_num_t lport, CounterName_E counterName, UINT64 *returnValue);
DRV_RET_E Hal_GetPortStatistics(port_num_t lport, stat_reg_t statisticsName, UINT64 *returnValue);

/*hal drv ratelimit*/
DRV_RET_E Hal_SetRatelimitInit(void);
DRV_RET_E Hal_SetRatelimitByPort(UINT32 direction, logic_pmask_t lPortMask, UINT32 ulKBps);
DRV_RET_E Hal_SetRatelimitStormCtl(STORM_CTLTYPE_E ctlType, logic_pmask_t lPortMask, STORM_CTLRATE_S stStorm);
DRV_RET_E Hal_SetRatelimitStormByPort(PORT_STORM_TYPE_E stormType, logic_pmask_t lPortMask, UINT32 ulKBps);


/*hal drv qos*/
DRV_RET_E Hal_SetQosInit(void);
DRV_RET_E Hal_Set1pIngressPriorityRemap(qos_8021p_to_intpri_t st1pToIntPri);
DRV_RET_E Hal_SetDscpIngressPriorityRemap(qos_dscp_to_intpri_t stDscpToIntPri);
DRV_RET_E Hal_SetQosTrustMode(UINT32 mode);
DRV_RET_E Hal_SetQosQueueSchedul(QueueMode_S stQueueSchedule);
DRV_RET_E Hal_SetCos2QidRemap(qos_8021p_to_intpri_t st1pPri2qid);

/*hal drv acl*/
DRV_RET_E Hal_SetAclInit(void);
DRV_RET_E Hal_SetVlanInterfaceAdd(vlan_id_t vid, mac_address_t mac_address);
DRV_RET_E Hal_SetVlanInterfaceDel(vlan_id_t vid);
DRV_RET_E Hal_AclTest(void);
DRV_RET_E Hal_AclRuleForEoamFilterCreate(UINT32 uiLPortId, 
                                   UINT32 uiRulePrecedence,
                                   ACL_TRUST_MODE_E uiAclRuleType,
                                   ACL_DIRECTION_E enAclDir,
                                   VOID *pRuleValue);
DRV_RET_E Hal_AclRuleForEoamFilterDelete(UINT32 uiLPortId, UINT32 uiRulePrecedence, ACL_DIRECTION_E enAclDir);
DRV_RET_E Hal_AclRuleForCtcClfRmkCreate(UINT32 uiLPortId, 
                                   UINT32 uiRulePrecedence,
                                   ACL_TRUST_MODE_E uiAclRuleType,
                                   VOID *pRuleValue,
                                   VOID *pRemarkPri);
DRV_RET_E Hal_AclRuleForCtcClfRmkDelete(UINT32 uiLPortId, UINT32 uiRulePrecedence);
DRV_RET_E Hal_AclRuleForCtcClfRmkClear(UINT32 uiLPortId);

typedef struct _l3_route_s_ {
    ip_address_t dest_ip;
    ip_address_t nexthop_ip;/*not valid with direct route*/
    mac_address_t mac_add;/*not valid with local vlan interface route*/
    UINT8 preference;/*priority 1~255, the smaller the higher, only valid if network route entries have same dest_ip and ip_mask_len*/
    UINT8 ip_mask_len;/*manual network route:1~32, auto direct route:32, default route:0*/
    vlan_id_t vid;
    UINT8 action;/*HAL_L3_ACT_ROUTE_XXX*/
}l3Route_t;

#define HAL_L3_ACT_ROUTE_HIT 0x01    /*this route has been hitted by fn_hash_lookup()*/
#define HAL_L3_ACT_ROUTE_ADD 0x02    /*this route has been added to harware table PIE*/
#define HAL_L3_ACT_ROUTE_CPU 0x04    /*this route redirect to cpu*/

BOOL Hal_IsNextHop(mac_address_t mac_add, vlan_id_t vid);
DRV_RET_E Hal_AddDirectUcastRoute(l3Route_t *pstRouteEntry);
DRV_RET_E Hal_DelDirectUcastRoute(ip_address_t dest_ip, vlan_id_t vid);
DRV_RET_E Hal_AddNetworkUcastRoute(l3Route_t *pstRouteEntry);
DRV_RET_E Hal_DelNetworkUcastRoute(l3Route_t *pstRouteEntry);
DRV_RET_E Hal_AgeNetworkUcastRouteNhop(ip_address_t nexthop_ip, vlan_id_t vid);
DRV_RET_E Hal_AddLocalRoute(l3Route_t *pstRouteEntry);
DRV_RET_E Hal_DelLocalRoute(l3Route_t *pstRouteEntry);
DRV_RET_E Hal_InstallMacAclRule
(logic_pmask_t *pstPortMask, vlan_id_t usVid, drv_acl_list_macBased_t *pstMbRule, BOOL bFlag);
DRV_RET_E Hal_InstallIpAclRule
(logic_pmask_t *pstPortMask, vlan_id_t usVid, drv_acl_list_ipBased_t *pstIbRule, BOOL bFlag);
DRV_RET_E Hal_ReinstallAcl(UINT16 usAclPrio, logic_pmask_t *lPortList);
DRV_RET_E Hal_RemoveAcl(UINT16 usAclPrio);
DRV_RET_E Hal_RemoveAclRuleByRuleId(UINT16 usAclPrio, UINT32 ulId, BOOL bFlag);
DRV_RET_E Hal_RemoveAclRuleByRulePrio(UINT16 usAclPrio, UINT16 usRulePrio);
DRV_RET_E Hal_AclEntryCanAlloc(field_entry_pri_type type, UINT32 count, UINT32 ulLogEn);
DRV_RET_E Hal_dhcpsp_drop_dhcps(logic_pmask_t *pstLportList);
DRV_RET_E Hal_dhcpsp_copy_dhcps(logic_pmask_t *pstLportList);
DRV_RET_E Hal_dhcpsp_ipfilter_enable(logic_pmask_t *pstLportMask);
DRV_RET_E Hal_dhcpsp_drop_arp(BOOL enable);
DRV_RET_E Hal_AgeRouteByPort(port_num_t lport);
DRV_RET_E Hal_AgeRouteByVlan(vlan_id_t vid);

/*hal drv eee*/
DRV_RET_E Hal_SetEEEByPort(logic_pmask_t *plportMask, BOOL enable);
DRV_RET_E Hal_SetAutoPwrDwnByPort(logic_pmask_t *plportMask, BOOL enable);

/*hal cable diag*/
DRV_RET_E Hal_GetCableDiag(port_num_t lport, cable_diag_t *pstDiag);

/*hal stp*/
DRV_RET_E Hal_GetStpPortState(port_num_t lport, UINT32 * pulState);
DRV_RET_E Hal_SetStpPortState(port_num_t lport, UINT32 ulState);


/*hal read/write register*/
DRV_RET_E Hal_soc_reg_read(UINT32 ulChipNum, UINT32 ulRegIndex, UINT32 * pulData);
DRV_RET_E Hal_soc_reg_write(UINT32 ulChipNum, UINT32 ulRegIndex, UINT32 ulData);
DRV_RET_E Hal_soc_mem_read(UINT32 ulChipNum, UINT32 ulTable, UINT32 ulAddr, UINT32 * pulEntryData);
DRV_RET_E Hal_soc_mem_write(UINT32 ulChipNum, UINT32 ulTable, UINT32 ulAddr, UINT32 * pulEntryData);
/*hal get/set cpu address*/
#ifdef CHIPSET_RTL8328
DRV_RET_E Hal_GetCpuAddr(UINT32 ulCpuAddr, UINT32 * pulData);
DRV_RET_E Hal_SetCpuAddr(UINT32 ulCpuAddr, UINT32 ulData);
#endif
DRV_RET_E Hal_SetCPUPortJumbo(UINT32 uiLen);

/******************************************************************/
/*        switch init                                             */
/******************************************************************/
DRV_RET_E Hal_SwitchInit(void);
DRV_RET_E Hal_PortInit(void);

/******************************************************************/
/*        special drv                                             */
/******************************************************************/
DRV_RET_E Hal_SdkInitSpecial(UINT32 unit);
#if 0
DRV_RET_E Hal_GpioInit(GPIO_PIN_E gpio_pin, GPIO_CONTROL_E gpio_ctl, GPIO_DIRECTION_E gpio_dir);
DRV_RET_E Hal_GpioDataSet(GPIO_PIN_E gpio_pin, GPIO_DATA_E gpio_data);
DRV_RET_E Hal_GpioDataGet(GPIO_PIN_E gpio_pin, GPIO_DATA_E *pgpio_data);
DRV_RET_E Hal_GpioDirectionSet(GPIO_PIN_E gpio_pin, GPIO_DIRECTION_E gpio_dir);
DRV_RET_E Hal_GpioDirectionGet(GPIO_PIN_E gpio_pin, GPIO_DIRECTION_E *pgpio_dir);
#endif
DRV_RET_E Hal_L2send(unsigned char *pMsg,UINT uiLen,l2_send_op *pOp);
DRV_RET_E Hal_SetInternalLoopback(port_num_t lgcPort, UINT32 uiEnable);

/*eeprom i2c*/
typedef enum 
{
    EepromOk=0,
    AckError,
    PointError,
    OutOfRang,
    OutOfAddr,
    GpioPinError
}EEPROM_RET;

typedef enum 
{
    EEPROM_I2C_SCL_PIN=0,
    EEPROM_I2C_SDA_PIN,
    EEPROM_I2C_PIN_END
}EEPROM_I2C_PIN_DEFINE;

DRV_RET_E Hal_I2cFiberInit(VOID);

DRV_RET_E Hal_I2cFiberPortRead(port_num_t lgcPort, UINT16 usAddr, UINT8* pucData, UINT32 ulLen, UINT32 ulPDataType);


#ifdef CHIPSET_OPL6750
DRV_RET_E Hal_CpuInit(void);
#endif

#if defined(CHIPSET_RTL8305) || defined(CHIPSET_RTL9607) || defined (CHIPSET_RTL9601)
DRV_RET_E Hal_RestartPortNeg(port_num_t lgcPort);
DRV_RET_E Hal_CfgPortCtcVlanGet(UINT32 uiLPortId, CTC_VLAN_CFG_S *pstCtcVlanMode);
#endif

/* */
DRV_RET_E _Hal_SetPortTransparent(UINT32 uiEgLPort, UINT32 uiIngLPort);
DRV_RET_E Hal_SetPortTransparentMode(UINT32 uiLPort, CTC_VLAN_CFG_S *pstVlanMode);
DRV_RET_E Hal_ResetPortTransparentMode(UINT32 uiLPort);
DRV_RET_E Hal_SetPortTranslationMode(UINT32 uiLPort, CTC_VLAN_CFG_S *pstVlanMode);
DRV_RET_E Hal_ResetPortTranslationMode(UINT32 uiLPort);
DRV_RET_E Hal_SetPortTagMode(UINT32 uiLPort, CTC_VLAN_CFG_S *pstVlanMode);
DRV_RET_E Hal_ResetPortTagMode(UINT32 uiLPort);
/* 014453 */

/*  */
typedef struct tag_VlanFid
{
    BOOL bValid;
    UINT32 uiVid;
    UINT32 uiFid;
    logic_pmask_t stLPortMsk;
    logic_pmask_t stUntagLPortMsk;
}VLAN_FID_S;

#define FID_INVALID_ID     0xFFFF

DRV_RET_E Hal_SetMcMacByMacAndFid(mac_mcast_t stMacMcast);
DRV_RET_E Hal_DelMcMacByMacAndFid(mac_mcast_t stMacMcast);
DRV_RET_E Hal_GetMcMacByMacAndFid(UINT32 uiVid, mac_address_t mac_addr, mac_mcast_t *pstMacMcast);
DRV_RET_E Hal_SetMcVlanMemberRmv(UINT32 ulVlanId, logic_pmask_t stLgcMask);
DRV_RET_E Hal_GetMcVlanMember(UINT32 ulVlanId, logic_pmask_t *pstLgcMask, logic_pmask_t *pstLgcMaskUntag);
DRV_RET_E Hal_GetMcVlanMem(UINT32 uiVid, logic_pmask_t *pstLPortMsk, logic_pmask_t *pstUntagLPortMsk);
DRV_RET_E Hal_GetTick(UINT64 *pullJiffies);
/* 013987 */

/*  */
DRV_RET_E Hal_GetFdbEntryByIndex(UINT32 uiIndex, UINT32 *puiFid, mac_common_t *pstCommMac);
DRV_RET_E Hal_GetCvlanEntry(UINT32 uiCvlanId, UINT32 *puiPhyMsk, UINT32 *puiPhyMskUntag, UINT32 *puiFid);
DRV_RET_E Hal_GetSvlanEntry(UINT32 uiSvlanId, UINT32 *puiPhyMsk, UINT32 *puiPhyMskUntag, UINT32 *puiFid);
DRV_RET_E Hal_GetC2sEntryByIndex(UINT32 uiIndex, UINT32 *puiCvid, UINT32 *puiSvid, UINT32 *puiPPortMsk);
DRV_RET_E Hal_GetSp2cEntryByIndex(UINT32 uiIndex, UINT32 *puiCvid, UINT32 *puiSvid, UINT32 *puiDstPPort);
/* 014528 */

/*begin added by liaohongjun 2012/11/30 of EPN104QID0084*/
DRV_RET_E Hal_AclRuleForDropLoopdOperation(UINT32 lgcPort, DROP_LOOPD_OP_E uiDropLoopdOp);
DRV_RET_E Hal_AclRuleForDropLoopdRemove(VOID);
/*end added by liaohongjun 2012/11/30 of EPN104QID0084*/

/*Begin add by shipeng 2013/05/27 for CTC*/ 
DRV_RET_E Hal_SetFecMode(UINT32 BothState);
DRV_RET_E Hal_GetFecMode(UINT32 *pBothState);
/*End add by shipeng 2013/05/27 for CTC*/

/*begin add by sunmingliang for packet droped*/
DRV_RET_E ACL_DeleteRuleByAclid(UINT32 uiAclRuleId);
DRV_RET_E _Hal_AclRuleEmptyIdGet(UINT32 *puiAclRuleId);
/*end add by sunmingliang for packet droped*/

/*Begin add by huangmingjian 2013-08-27*/
DRV_RET_E Hal_GetEponMibCounter(rtk_epon_counter_app_t *pCounter);
DRV_RET_E Hal_EponMibGlobal_Reset();
DRV_RET_E Hal_GetPonmacTransceiver(rtk_transceiver_parameter_type_app_t type, rtk_transceiver_data_app_t*pData);
/*End add by huangmingjian 2013-08-27*/

DRV_RET_E Hal_SetManageVlan(unsigned int uiVlanIndex);
DRV_RET_E Hal_SetWirelessUpServiceVlan(unsigned int uiVlanIndex);

void  rtk_port_parm_init(void);
DRV_RET_E Hal_AclRuleInit(void);
DRV_RET_E Hal_ClfRuleInit(void);
DRV_RET_E Hal_SetCpuHeader(BOOL bEnable);
void rtk_SmiInit(void);
DRV_RET_E Hal_VlanInterfaceAdd(UINT32 vid, mac_address_t mac_address);
DRV_RET_E Hal_VlanInterfaceDelete(UINT32 vid);

DRV_RET_E Hal_AddSp2cEntry2Port(UINT32 uiLPort, UINT32 ulSvid, UINT32 ulCvid);
DRV_RET_E Hal_DelSp2cEntryFromPort(UINT32 uiLPort, UINT32 ulSvid);
DRV_RET_E Hal_SetMcTransltVlanMemberAdd(UINT32 usrvlan, UINT32 mvlan, logic_pmask_t stLgcMask, logic_pmask_t stLgcMaskUntag, logic_pmask_t stLgcMasksvlanUntag);
DRV_RET_E Hal_SetMctransltVlanMemberRmv(UINT32 usrvlan,UINT32 mvlan, logic_pmask_t stLgcMask);
DRV_RET_E Hal_AddC2sEntry2Port(UINT32 uiLPort, UINT32 ulCvid, UINT32 ulSvid);
DRV_RET_E Hal_DelC2sEntryFromPort(UINT32 uiLPort, UINT32 ulCvid, UINT32 ulSvid);

void Hal_CpuRegAccessInit(void);
DRV_RET_E Hal_CpuRegRead(UINT32 regAddr,UINT32 *regVal);
DRV_RET_E Hal_CpuRegWrite(UINT32 regAddr,UINT32 regVal);

DRV_RET_E Hal_GpioDirInSet(UINT32 uiPin);
DRV_RET_E Hal_GpioDirOutSet(UINT32 uiPin);
DRV_RET_E Hal_GpioValGet(UINT32 uiPin, UINT32 *puiRegValue);
DRV_RET_E Hal_GpioOutputSet(UINT32 uiPin, UINT32 uiRegValue);

DRV_RET_E Hal_ClfRuleForCtcClfPriToQueueCreate(UINT32 uiLPortId, 
                                   UINT32 uiRulePrecedence,
                                   VOID *pRemarkPri);
DRV_RET_E Hal_ClfRuleForCtcClfPriToQueueDelete(UINT32 uiLPortId, UINT32 uiRulePrecedence);
DRV_RET_E Hal_ClfRuleForCtcClfPriToQueueClear(UINT32 uiLPortId);
DRV_RET_E Hal_LookupMissFloodPortMaskSet(UINT32 type, rtk_portmask_t *pFlood_portmask);

/* Modified by Einsn For EOC Porting 20130416 */
#ifdef CONFIG_EOC_EXTEND
DRV_RET_E Hal_SetVlanFilter(BOOL bEnable);
DRV_RET_E Hal_SetEocLowLevelFunction(eoc_low_level_t *peocLowLevel);
DRV_RET_E Hal_SetRatelimitStormCtrlByPort(PORT_STORM_TYPE_E stormType, logic_pmask_t lPortMask, UINT32 ulKBps);
DRV_RET_E Hal_GetMibPortStatisticsRaw(port_num_t lport, UINT32 mibType, UINT64 *p_ullStat);
#endif 
/* End */


#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif  /* end of _HAL_COMMON_H_ */
