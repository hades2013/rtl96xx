/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 41959 $
 * $Date: 2013-08-09 18:25:45 +0800 (星期五, 09 八月 2013) $
 *
 * Purpose : Export the public APIs in lower layer module in the SDK.
 *
 * Feature : Export the public APIs in lower layer module
 *
 */

/*
 * Include Files
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>

#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/print.h>
#include <rtk/init.h>
#include <apollo_reg_struct.h>
#include <hal/mac/reg.h>


#include <rtk/acl.h>
#include <rtk/classify.h>
#include <rtk/cpu.h>
#include <rtk/cpu_tag.h>
#include <rtk/debug.h>
#include <rtk/default.h>
#include <rtk/dot1x.h>
#include <rtk/intr.h>
#include <rtk/irq.h>
#include <rtk/l2.h>
#include <rtk/l34.h>
#include <rtk/led.h>
#include <rtk/mirror.h>
#include <rtk/oam.h>
#include <rtk/ponmac.h>
#include <rtk/port.h>
#include <rtk/qos.h>
#include <rtk/rate.h>
#include <rtk/rldp.h>
#include <rtk/sec.h>
#include <rtk/stat.h>
#include <rtk/stp.h>
#include <rtk/svlan.h>
#include <rtk/switch.h>
#include <rtk/trap.h>
#include <rtk/trunk.h>
#include <rtk/vlan.h>
#include <rtk/time.h>
#include <rtk/gpio.h>
#include <rtk/i2c.h>
#include <rtk/l34lite.h>
#include <rtk/epon.h>

#ifdef CONFIG_GPON_FEATURE
#if CONFIG_GPON_VERSION > 1
#include <rtk/gponv2.h>
#else
#include <rtk/gpon.h>
#endif
#endif

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */

/*
 * Macro Definition
 */
#define REG32(reg)	(*(volatile unsigned int   *)((unsigned int)reg))

/*
 * Function Declaration
 */
static int __init rtk_sdk_init(void)
{
    osal_printf("Init RTK Driver Module....");

    if(RT_ERR_OK != rtk_system_init())
        osal_printf("FAIL\n");
    else
        osal_printf("OK\n");

	return 0;
}

static void __exit rtk_sdk_exit(void)
{
    osal_printf("Exit RTK Driver Module....");

    if(RT_ERR_OK != rtk_deinit())
        osal_printf("FAIL\n");
    else
        osal_printf("OK\n");
}

module_init(rtk_sdk_init);
module_exit(rtk_sdk_exit);

MODULE_DESCRIPTION("Switch SDK RTK Driver Module");

/* RTK functions */

EXPORT_SYMBOL(rtk_init);
EXPORT_SYMBOL(rtk_acl_init);
EXPORT_SYMBOL(rtk_acl_template_set);
EXPORT_SYMBOL(rtk_acl_template_get);
EXPORT_SYMBOL(rtk_acl_fieldSelect_set);
EXPORT_SYMBOL(rtk_acl_fieldSelect_get);
EXPORT_SYMBOL(rtk_acl_igrRuleEntry_get);
EXPORT_SYMBOL(rtk_acl_igrRuleField_add);
EXPORT_SYMBOL(rtk_acl_igrRuleEntry_add);
EXPORT_SYMBOL(rtk_acl_igrRuleEntry_del);
EXPORT_SYMBOL(rtk_acl_igrRuleEntry_delAll);
EXPORT_SYMBOL(rtk_acl_igrUnmatchAction_set);
EXPORT_SYMBOL(rtk_acl_igrUnmatchAction_get);
EXPORT_SYMBOL(rtk_acl_igrState_set);
EXPORT_SYMBOL(rtk_acl_igrState_get);
EXPORT_SYMBOL(rtk_acl_ipRange_set);
EXPORT_SYMBOL(rtk_acl_ipRange_get);
EXPORT_SYMBOL(rtk_acl_vidRange_set);
EXPORT_SYMBOL(rtk_acl_vidRange_get);
EXPORT_SYMBOL(rtk_acl_portRange_set);
EXPORT_SYMBOL(rtk_acl_portRange_get);
EXPORT_SYMBOL(rtk_acl_packetLengthRange_set);
EXPORT_SYMBOL(rtk_acl_packetLengthRange_get);
EXPORT_SYMBOL(rtk_acl_igrRuleMode_set);
EXPORT_SYMBOL(rtk_acl_igrRuleMode_get);
EXPORT_SYMBOL(rtk_acl_igrPermitState_set);
EXPORT_SYMBOL(rtk_acl_igrPermitState_get);
EXPORT_SYMBOL(rtk_acl_usrRuleField_add);
EXPORT_SYMBOL(rtk_acl_usrRuleField_free);

EXPORT_SYMBOL(rtk_trunk_init);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithm_get);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithm_set);
EXPORT_SYMBOL(rtk_trunk_port_get);
EXPORT_SYMBOL(rtk_trunk_port_set);
EXPORT_SYMBOL(rtk_trunk_hashMappingTable_get);
EXPORT_SYMBOL(rtk_trunk_hashMappingTable_set);
EXPORT_SYMBOL(rtk_trunk_mode_get);
EXPORT_SYMBOL(rtk_trunk_mode_set);
EXPORT_SYMBOL(rtk_trunk_trafficSeparate_get);
EXPORT_SYMBOL(rtk_trunk_trafficSeparate_set);
EXPORT_SYMBOL(rtk_trunk_portQueueEmpty_get);
EXPORT_SYMBOL(rtk_trunk_trafficPause_get);
EXPORT_SYMBOL(rtk_trunk_trafficPause_set);
EXPORT_SYMBOL(rtk_trap_init);
EXPORT_SYMBOL(rtk_trap_reasonTrapToCpuPriority_get);
EXPORT_SYMBOL(rtk_trap_reasonTrapToCpuPriority_set);
EXPORT_SYMBOL(rtk_trap_igmpCtrlPkt2CpuEnable_get);
EXPORT_SYMBOL(rtk_trap_igmpCtrlPkt2CpuEnable_set);
EXPORT_SYMBOL(rtk_trap_mldCtrlPkt2CpuEnable_get);
EXPORT_SYMBOL(rtk_trap_mldCtrlPkt2CpuEnable_set);
EXPORT_SYMBOL(rtk_trap_portIgmpMldCtrlPktAction_get);
EXPORT_SYMBOL(rtk_trap_portIgmpMldCtrlPktAction_set);
EXPORT_SYMBOL(rtk_trap_ipMcastPkt2CpuEnable_get);
EXPORT_SYMBOL(rtk_trap_ipMcastPkt2CpuEnable_set);
EXPORT_SYMBOL(rtk_trap_l2McastPkt2CpuEnable_get);
EXPORT_SYMBOL(rtk_trap_l2McastPkt2CpuEnable_set);
EXPORT_SYMBOL(rtk_trap_rmaAction_get);
EXPORT_SYMBOL(rtk_trap_rmaAction_set);
EXPORT_SYMBOL(rtk_trap_rmaPri_get);
EXPORT_SYMBOL(rtk_trap_rmaPri_set);
EXPORT_SYMBOL(rtk_trap_oamPduAction_get);
EXPORT_SYMBOL(rtk_trap_oamPduAction_set);
EXPORT_SYMBOL(rtk_trap_oamPduPri_get);
EXPORT_SYMBOL(rtk_trap_oamPduPri_set);
EXPORT_SYMBOL(rtk_switch_init);
EXPORT_SYMBOL(rtk_switch_deviceInfo_get);
EXPORT_SYMBOL(rtk_switch_phyPortId_get);
EXPORT_SYMBOL(rtk_switch_logicalPort_get);
EXPORT_SYMBOL(rtk_switch_port2PortMask_set);
EXPORT_SYMBOL(rtk_switch_port2PortMask_clear);
EXPORT_SYMBOL(rtk_switch_portIdInMask_check);
EXPORT_SYMBOL(rtk_switch_portMask_Clear);
EXPORT_SYMBOL(rtk_switch_allPortMask_set);
EXPORT_SYMBOL(rtk_switch_allExtPortMask_set);
EXPORT_SYMBOL(rtk_switch_nextPortInMask_get);
EXPORT_SYMBOL(rtk_switch_maxPktLenLinkSpeed_get);
EXPORT_SYMBOL(rtk_switch_maxPktLenLinkSpeed_set);
EXPORT_SYMBOL(rtk_switch_mgmtMacAddr_get);
EXPORT_SYMBOL(rtk_switch_mgmtMacAddr_set);
EXPORT_SYMBOL(rtk_switch_version_get);
EXPORT_SYMBOL(rtk_switch_chip_reset);
EXPORT_SYMBOL(rtk_svlan_init);
EXPORT_SYMBOL(rtk_svlan_create);
EXPORT_SYMBOL(rtk_svlan_destroy);
EXPORT_SYMBOL(rtk_svlan_portSvid_get);
EXPORT_SYMBOL(rtk_svlan_portSvid_set);
EXPORT_SYMBOL(rtk_svlan_servicePort_get);
EXPORT_SYMBOL(rtk_svlan_servicePort_set);
EXPORT_SYMBOL(rtk_svlan_memberPort_set);
EXPORT_SYMBOL(rtk_svlan_memberPort_get);
EXPORT_SYMBOL(rtk_svlan_tpidEntry_get);
EXPORT_SYMBOL(rtk_svlan_tpidEntry_set);
EXPORT_SYMBOL(rtk_svlan_priorityRef_set);
EXPORT_SYMBOL(rtk_svlan_priorityRef_get);
EXPORT_SYMBOL(rtk_svlan_memberPortEntry_set);
EXPORT_SYMBOL(rtk_svlan_memberPortEntry_get);
EXPORT_SYMBOL(rtk_svlan_ipmc2s_add);
EXPORT_SYMBOL(rtk_svlan_ipmc2s_del);
EXPORT_SYMBOL(rtk_svlan_ipmc2s_get);
EXPORT_SYMBOL(rtk_svlan_l2mc2s_add);
EXPORT_SYMBOL(rtk_svlan_l2mc2s_del);
EXPORT_SYMBOL(rtk_svlan_l2mc2s_get);
EXPORT_SYMBOL(rtk_svlan_sp2c_add);
EXPORT_SYMBOL(rtk_svlan_sp2c_get);
EXPORT_SYMBOL(rtk_svlan_sp2c_del);
EXPORT_SYMBOL(rtk_svlan_dmacVidSelState_set);
EXPORT_SYMBOL(rtk_svlan_dmacVidSelState_get);
EXPORT_SYMBOL(rtk_svlan_unmatchAction_set);
EXPORT_SYMBOL(rtk_svlan_unmatchAction_get);
EXPORT_SYMBOL(rtk_svlan_untagAction_set);
EXPORT_SYMBOL(rtk_svlan_untagAction_get);
EXPORT_SYMBOL(rtk_svlan_c2s_add);
EXPORT_SYMBOL(rtk_svlan_c2s_del);
EXPORT_SYMBOL(rtk_svlan_c2s_get);
EXPORT_SYMBOL(rtk_svlan_trapPri_get);
EXPORT_SYMBOL(rtk_svlan_trapPri_set);
EXPORT_SYMBOL(rtk_svlan_deiKeepState_get);
EXPORT_SYMBOL(rtk_svlan_deiKeepState_set);
EXPORT_SYMBOL(rtk_svlan_lookupType_get);
EXPORT_SYMBOL(rtk_svlan_lookupType_set);
EXPORT_SYMBOL(rtk_svlan_sp2cUnmatchCtagging_get);
EXPORT_SYMBOL(rtk_svlan_sp2cUnmatchCtagging_set);
EXPORT_SYMBOL(rtk_svlan_priority_get);
EXPORT_SYMBOL(rtk_svlan_priority_set);
EXPORT_SYMBOL(rtk_svlan_fid_get);
EXPORT_SYMBOL(rtk_svlan_fid_set);
EXPORT_SYMBOL(rtk_svlan_fidEnable_get);
EXPORT_SYMBOL(rtk_svlan_fidEnable_set);
EXPORT_SYMBOL(rtk_svlan_enhancedFid_get);
EXPORT_SYMBOL(rtk_svlan_enhancedFid_set);
EXPORT_SYMBOL(rtk_svlan_enhancedFidEnable_get);
EXPORT_SYMBOL(rtk_svlan_enhancedFidEnable_set);
EXPORT_SYMBOL(rtk_svlan_dmacVidSelForcedState_set);
EXPORT_SYMBOL(rtk_svlan_dmacVidSelForcedState_get);

EXPORT_SYMBOL(rtk_stp_init);
EXPORT_SYMBOL(rtk_stp_mstpState_get);
EXPORT_SYMBOL(rtk_stp_mstpState_set);
EXPORT_SYMBOL(rtk_stat_init);
EXPORT_SYMBOL(rtk_stat_global_reset);
EXPORT_SYMBOL(rtk_stat_port_reset);
EXPORT_SYMBOL(rtk_stat_log_reset);
EXPORT_SYMBOL(rtk_stat_rstCntValue_set);
EXPORT_SYMBOL(rtk_stat_rstCntValue_get);
EXPORT_SYMBOL(rtk_stat_global_get);
EXPORT_SYMBOL(rtk_stat_global_getAll);
EXPORT_SYMBOL(rtk_stat_port_get);
EXPORT_SYMBOL(rtk_stat_port_getAll);
EXPORT_SYMBOL(rtk_stat_log_get);
EXPORT_SYMBOL(rtk_stat_logCtrl_set);
EXPORT_SYMBOL(rtk_stat_logCtrl_get);
EXPORT_SYMBOL(rtk_stat_mibCntMode_get);
EXPORT_SYMBOL(rtk_stat_mibCntMode_set);
EXPORT_SYMBOL(rtk_stat_mibLatchTimer_get);
EXPORT_SYMBOL(rtk_stat_mibLatchTimer_set);
EXPORT_SYMBOL(rtk_sec_init);
EXPORT_SYMBOL(rtk_sec_portAttackPreventState_get);
EXPORT_SYMBOL(rtk_sec_portAttackPreventState_set);
EXPORT_SYMBOL(rtk_sec_attackPrevent_get);
EXPORT_SYMBOL(rtk_sec_attackPrevent_set);
EXPORT_SYMBOL(rtk_sec_attackFloodThresh_get);
EXPORT_SYMBOL(rtk_sec_attackFloodThresh_set);
EXPORT_SYMBOL(rtk_rldp_init);
EXPORT_SYMBOL(rtk_rldp_config_set);
EXPORT_SYMBOL(rtk_rldp_config_get);
EXPORT_SYMBOL(rtk_rldp_portConfig_set);
EXPORT_SYMBOL(rtk_rldp_portConfig_get);
EXPORT_SYMBOL(rtk_rldp_status_get);
EXPORT_SYMBOL(rtk_rldp_portStatus_get);
EXPORT_SYMBOL(rtk_rldp_portStatus_clear);
EXPORT_SYMBOL(rtk_rlpp_init);
EXPORT_SYMBOL(rtk_rlpp_trapType_set);
EXPORT_SYMBOL(rtk_rlpp_trapType_get);
EXPORT_SYMBOL(rtk_rate_init);
EXPORT_SYMBOL(rtk_rate_portIgrBandwidthCtrlRate_get);
EXPORT_SYMBOL(rtk_rate_portIgrBandwidthCtrlRate_set);
EXPORT_SYMBOL(rtk_rate_portIgrBandwidthCtrlIncludeIfg_get);
EXPORT_SYMBOL(rtk_rate_portIgrBandwidthCtrlIncludeIfg_set);
EXPORT_SYMBOL(rtk_rate_portEgrBandwidthCtrlRate_get);
EXPORT_SYMBOL(rtk_rate_portEgrBandwidthCtrlRate_set);
EXPORT_SYMBOL(rtk_rate_egrBandwidthCtrlIncludeIfg_get);
EXPORT_SYMBOL(rtk_rate_egrBandwidthCtrlIncludeIfg_set);
EXPORT_SYMBOL(rtk_rate_portEgrBandwidthCtrlIncludeIfg_get);
EXPORT_SYMBOL(rtk_rate_portEgrBandwidthCtrlIncludeIfg_set);
EXPORT_SYMBOL(rtk_rate_egrQueueBwCtrlEnable_get);
EXPORT_SYMBOL(rtk_rate_egrQueueBwCtrlEnable_set);
EXPORT_SYMBOL(rtk_rate_egrQueueBwCtrlMeterIdx_get);
EXPORT_SYMBOL(rtk_rate_egrQueueBwCtrlMeterIdx_set);
EXPORT_SYMBOL(rtk_rate_stormControlMeterIdx_get);
EXPORT_SYMBOL(rtk_rate_stormControlMeterIdx_set);
EXPORT_SYMBOL(rtk_rate_stormControlPortEnable_get);
EXPORT_SYMBOL(rtk_rate_stormControlPortEnable_set);
EXPORT_SYMBOL(rtk_rate_stormControlEnable_get);
EXPORT_SYMBOL(rtk_rate_stormControlEnable_set);
EXPORT_SYMBOL(rtk_rate_stormBypass_set);
EXPORT_SYMBOL(rtk_rate_stormBypass_get);
EXPORT_SYMBOL(rtk_rate_shareMeter_set);
EXPORT_SYMBOL(rtk_rate_shareMeter_get);
EXPORT_SYMBOL(rtk_rate_shareMeterBucket_set);
EXPORT_SYMBOL(rtk_rate_shareMeterBucket_get);
EXPORT_SYMBOL(rtk_rate_shareMeterExceed_get);
EXPORT_SYMBOL(rtk_rate_shareMeterExceed_clear);
EXPORT_SYMBOL(rtk_qos_init);
EXPORT_SYMBOL(rtk_qos_priSelGroup_get);
EXPORT_SYMBOL(rtk_qos_priSelGroup_set);
EXPORT_SYMBOL(rtk_qos_portPri_get);
EXPORT_SYMBOL(rtk_qos_portPri_set);
EXPORT_SYMBOL(rtk_qos_dscpPriRemapGroup_get);
EXPORT_SYMBOL(rtk_qos_dscpPriRemapGroup_set);
EXPORT_SYMBOL(rtk_qos_1pPriRemapGroup_get);
EXPORT_SYMBOL(rtk_qos_1pPriRemapGroup_set);
EXPORT_SYMBOL(rtk_qos_priMap_get);
EXPORT_SYMBOL(rtk_qos_priMap_set);
EXPORT_SYMBOL(rtk_qos_portPriMap_get);
EXPORT_SYMBOL(rtk_qos_portPriMap_set);
EXPORT_SYMBOL(rtk_qos_1pRemarkEnable_get);
EXPORT_SYMBOL(rtk_qos_1pRemarkEnable_set);
EXPORT_SYMBOL(rtk_qos_1pRemarkGroup_get);
EXPORT_SYMBOL(rtk_qos_1pRemarkGroup_set);
EXPORT_SYMBOL(rtk_qos_dscpRemarkEnable_get);
EXPORT_SYMBOL(rtk_qos_dscpRemarkEnable_set);
EXPORT_SYMBOL(rtk_qos_dscpRemarkGroup_get);
EXPORT_SYMBOL(rtk_qos_dscpRemarkGroup_set);
EXPORT_SYMBOL(rtk_qos_portDscpRemarkSrcSel_get);
EXPORT_SYMBOL(rtk_qos_portDscpRemarkSrcSel_set);
EXPORT_SYMBOL(rtk_qos_dscp2DscpRemarkGroup_get);
EXPORT_SYMBOL(rtk_qos_dscp2DscpRemarkGroup_set);
EXPORT_SYMBOL(rtk_qos_fwd2CpuPriRemap_get);
EXPORT_SYMBOL(rtk_qos_fwd2CpuPriRemap_set);
EXPORT_SYMBOL(rtk_qos_schedulingQueue_get);
EXPORT_SYMBOL(rtk_qos_schedulingQueue_set);
EXPORT_SYMBOL(rtk_qos_portPriSelGroup_get);
EXPORT_SYMBOL(rtk_qos_portPriSelGroup_set);
EXPORT_SYMBOL(rtk_port_init);
EXPORT_SYMBOL(rtk_port_link_get);
EXPORT_SYMBOL(rtk_port_speedDuplex_get);
EXPORT_SYMBOL(rtk_port_flowctrl_get);
EXPORT_SYMBOL(rtk_port_phyAutoNegoEnable_get);
EXPORT_SYMBOL(rtk_port_phyAutoNegoEnable_set);
EXPORT_SYMBOL(rtk_port_phyAutoNegoAbility_get);
EXPORT_SYMBOL(rtk_port_phyAutoNegoAbility_set);
EXPORT_SYMBOL(rtk_port_phyForceModeAbility_get);
EXPORT_SYMBOL(rtk_port_phyForceModeAbility_set);
EXPORT_SYMBOL(rtk_port_phyReg_get);
EXPORT_SYMBOL(rtk_port_phyReg_set);
EXPORT_SYMBOL(rtk_port_phyMasterSlave_get);
EXPORT_SYMBOL(rtk_port_phyMasterSlave_set);
EXPORT_SYMBOL(rtk_port_phyTestMode_get);
EXPORT_SYMBOL(rtk_port_phyTestMode_set);
EXPORT_SYMBOL(rtk_port_cpuPortId_get);
EXPORT_SYMBOL(rtk_port_isolation_get);
EXPORT_SYMBOL(rtk_port_isolation_set);
EXPORT_SYMBOL(rtk_port_isolationExt_get);
EXPORT_SYMBOL(rtk_port_isolationExt_set);
EXPORT_SYMBOL(rtk_port_isolationL34_get);
EXPORT_SYMBOL(rtk_port_isolationL34_set);
EXPORT_SYMBOL(rtk_port_isolationExtL34_get);
EXPORT_SYMBOL(rtk_port_isolationExtL34_set);
EXPORT_SYMBOL(rtk_port_isolationEntry_get);
EXPORT_SYMBOL(rtk_port_isolationEntry_set);
EXPORT_SYMBOL(rtk_port_isolationEntryExt_get);
EXPORT_SYMBOL(rtk_port_isolationEntryExt_set);
EXPORT_SYMBOL(rtk_port_isolationCtagPktConfig_get);
EXPORT_SYMBOL(rtk_port_isolationCtagPktConfig_set);
EXPORT_SYMBOL(rtk_port_isolationL34PktConfig_get);
EXPORT_SYMBOL(rtk_port_isolationL34PktConfig_set);
EXPORT_SYMBOL(rtk_port_isolationIpmcLeaky_get);
EXPORT_SYMBOL(rtk_port_isolationIpmcLeaky_set);
EXPORT_SYMBOL(rtk_port_isolationPortLeaky_get);
EXPORT_SYMBOL(rtk_port_isolationPortLeaky_set);
EXPORT_SYMBOL(rtk_port_isolationLeaky_get);
EXPORT_SYMBOL(rtk_port_isolationLeaky_set);
EXPORT_SYMBOL(rtk_port_macRemoteLoopbackEnable_get);
EXPORT_SYMBOL(rtk_port_macRemoteLoopbackEnable_set);
EXPORT_SYMBOL(rtk_port_macLocalLoopbackEnable_get);
EXPORT_SYMBOL(rtk_port_macLocalLoopbackEnable_set);
EXPORT_SYMBOL(rtk_port_adminEnable_get);
EXPORT_SYMBOL(rtk_port_adminEnable_set);
EXPORT_SYMBOL(rtk_port_specialCongest_get);
EXPORT_SYMBOL(rtk_port_specialCongest_set);
EXPORT_SYMBOL(rtk_port_specialCongestStatus_get);
EXPORT_SYMBOL(rtk_port_specialCongestStatus_clear);
EXPORT_SYMBOL(rtk_port_greenEnable_get);
EXPORT_SYMBOL(rtk_port_greenEnable_set);
EXPORT_SYMBOL(rtk_port_phyCrossOverMode_get);
EXPORT_SYMBOL(rtk_port_phyCrossOverMode_set);
EXPORT_SYMBOL(rtk_port_enhancedFid_get);
EXPORT_SYMBOL(rtk_port_enhancedFid_set);
EXPORT_SYMBOL(rtk_port_rtctResult_get);
EXPORT_SYMBOL(rtk_port_rtct_start);
EXPORT_SYMBOL(rtk_port_macForceAbility_get);
EXPORT_SYMBOL(rtk_port_macForceAbility_set);
EXPORT_SYMBOL(rtk_port_macForceAbilityState_get);
EXPORT_SYMBOL(rtk_port_macForceAbilityState_set);
EXPORT_SYMBOL(rtk_port_macExtMode_set);
EXPORT_SYMBOL(rtk_port_macExtMode_get);
EXPORT_SYMBOL(rtk_port_macExtRgmiiDelay_set);
EXPORT_SYMBOL(rtk_port_macExtRgmiiDelay_get);
EXPORT_SYMBOL(rtk_port_gigaLiteEnable_set);
EXPORT_SYMBOL(rtk_port_gigaLiteEnable_get);

EXPORT_SYMBOL(rtk_ponmac_init);
EXPORT_SYMBOL(rtk_ponmac_queue_add);
EXPORT_SYMBOL(rtk_ponmac_queue_get);
EXPORT_SYMBOL(rtk_ponmac_queue_del);
EXPORT_SYMBOL(rtk_ponmac_flow2Queue_set);
EXPORT_SYMBOL(rtk_ponmac_flow2Queue_get);
EXPORT_SYMBOL(rtk_ponmac_mode_set);
EXPORT_SYMBOL(rtk_ponmac_mode_get);
EXPORT_SYMBOL(rtk_ponmac_transceiver_get);
EXPORT_SYMBOL(rtk_oam_init);
EXPORT_SYMBOL(rtk_oam_parserAction_set);
EXPORT_SYMBOL(rtk_oam_parserAction_get);
EXPORT_SYMBOL(rtk_oam_multiplexerAction_set);
EXPORT_SYMBOL(rtk_oam_multiplexerAction_get);
EXPORT_SYMBOL(rtk_mirror_init);
EXPORT_SYMBOL(rtk_mirror_portBased_set);
EXPORT_SYMBOL(rtk_mirror_portBased_get);
EXPORT_SYMBOL(rtk_mirror_portIso_set);
EXPORT_SYMBOL(rtk_mirror_portIso_get);
EXPORT_SYMBOL(rtk_led_init);
EXPORT_SYMBOL(rtk_led_operation_get);
EXPORT_SYMBOL(rtk_led_operation_set);
EXPORT_SYMBOL(rtk_led_serialMode_get);
EXPORT_SYMBOL(rtk_led_serialMode_set);
EXPORT_SYMBOL(rtk_led_blinkRate_get);
EXPORT_SYMBOL(rtk_led_blinkRate_set);
EXPORT_SYMBOL(rtk_led_config_set);
EXPORT_SYMBOL(rtk_led_config_get);
EXPORT_SYMBOL(rtk_led_modeForce_get);
EXPORT_SYMBOL(rtk_led_modeForce_set);
EXPORT_SYMBOL(rtk_led_parallelEnable_get);
EXPORT_SYMBOL(rtk_led_parallelEnable_set);
EXPORT_SYMBOL(rtk_led_ponAlarm_get);
EXPORT_SYMBOL(rtk_led_ponAlarm_set);
EXPORT_SYMBOL(rtk_led_ponWarning_get);
EXPORT_SYMBOL(rtk_led_ponWarning_set);
EXPORT_SYMBOL(rtk_l34_init);
EXPORT_SYMBOL(rtk_l34_netifTable_set);
EXPORT_SYMBOL(rtk_l34_netifTable_get);
EXPORT_SYMBOL(rtk_l34_arpTable_set);
EXPORT_SYMBOL(rtk_l34_arpTable_get);
EXPORT_SYMBOL(rtk_l34_arpTable_del);
EXPORT_SYMBOL(rtk_l34_routingTable_set);
EXPORT_SYMBOL(rtk_l34_routingTable_get);
EXPORT_SYMBOL(rtk_l34_routingTable_del);
EXPORT_SYMBOL(rtk_l34_nexthopTable_set);
EXPORT_SYMBOL(rtk_l34_nexthopTable_get);
EXPORT_SYMBOL(rtk_l34_extIntIPTable_set);
EXPORT_SYMBOL(rtk_l34_extIntIPTable_get);
EXPORT_SYMBOL(rtk_l34_extIntIPTable_del);
EXPORT_SYMBOL(rtk_l34_naptInboundTable_set);
EXPORT_SYMBOL(rtk_l34_naptInboundTable_get);
EXPORT_SYMBOL(rtk_l34_naptOutboundTable_set);
EXPORT_SYMBOL(rtk_l34_naptOutboundTable_get);
EXPORT_SYMBOL(rtk_l34_ipmcTransTable_set);
EXPORT_SYMBOL(rtk_l34_ipmcTransTable_get);
EXPORT_SYMBOL(rtk_l34_table_reset);
EXPORT_SYMBOL(rtk_l34_bindingAction_set);
EXPORT_SYMBOL(rtk_l34_bindingAction_get);
EXPORT_SYMBOL(rtk_l34_bindingTable_set);
EXPORT_SYMBOL(rtk_l34_bindingTable_get);
EXPORT_SYMBOL(rtk_l34_pppoeTable_set);
EXPORT_SYMBOL(rtk_l34_pppoeTable_get);
EXPORT_SYMBOL(rtk_l34_wanTypeTable_set);
EXPORT_SYMBOL(rtk_l34_wanTypeTable_get);
EXPORT_SYMBOL(rtk_l34_ipv6RoutingTable_set);
EXPORT_SYMBOL(rtk_l34_ipv6RoutingTable_get);
EXPORT_SYMBOL(rtk_l34_ipv6NeighborTable_set);
EXPORT_SYMBOL(rtk_l34_ipv6NeighborTable_get);
EXPORT_SYMBOL(rtk_l34_hsabMode_set);
EXPORT_SYMBOL(rtk_l34_hsabMode_get);
EXPORT_SYMBOL(rtk_l34_hsaData_get);
EXPORT_SYMBOL(rtk_l34_hsbData_get);
EXPORT_SYMBOL(rtk_l34_portWanMap_set);
EXPORT_SYMBOL(rtk_l34_portWanMap_get);
EXPORT_SYMBOL(rtk_l34_globalState_set);
EXPORT_SYMBOL(rtk_l34_globalState_get);
EXPORT_SYMBOL(rtk_l34_lookupMode_set);
EXPORT_SYMBOL(rtk_l34_lookupMode_get);
EXPORT_SYMBOL(rtk_l34_lookupPortMap_set);
EXPORT_SYMBOL(rtk_l34_lookupPortMap_get);
EXPORT_SYMBOL(rtk_l34_wanRoutMode_set);
EXPORT_SYMBOL(rtk_l34_wanRoutMode_get);
EXPORT_SYMBOL(rtk_l34_arpTrfIndicator_get);
EXPORT_SYMBOL(rtk_l34_naptTrfIndicator_get);
EXPORT_SYMBOL(rtk_l34_pppTrfIndicator_get);
EXPORT_SYMBOL(rtk_l34_neighTrfIndicator_get);
EXPORT_SYMBOL(rtk_l34_hsdState_set);
EXPORT_SYMBOL(rtk_l34_hsdState_get);
EXPORT_SYMBOL(rtk_l34_hwL4TrfWrkTbl_set);
EXPORT_SYMBOL(rtk_l34_hwL4TrfWrkTbl_get);
EXPORT_SYMBOL(rtk_l34_l4TrfTb_get);
EXPORT_SYMBOL(rtk_l34_hwL4TrfWrkTbl_Clear);
EXPORT_SYMBOL(rtk_l34_hwArpTrfWrkTbl_set);
EXPORT_SYMBOL(rtk_l34_hwArpTrfWrkTbl_get);
EXPORT_SYMBOL(rtk_l34_arpTrfTb_get);
EXPORT_SYMBOL(rtk_l34_hwArpTrfWrkTbl_Clear);
EXPORT_SYMBOL(rtk_l34_naptTrfIndicator_get_all);
EXPORT_SYMBOL(rtk_l34_arpTrfIndicator_get_all);


EXPORT_SYMBOL(rtk_l2_init);
EXPORT_SYMBOL(rtk_l2_flushLinkDownPortAddrEnable_get);
EXPORT_SYMBOL(rtk_l2_flushLinkDownPortAddrEnable_set);
EXPORT_SYMBOL(rtk_l2_ucastAddr_flush);
EXPORT_SYMBOL(rtk_l2_table_clear);
EXPORT_SYMBOL(rtk_l2_limitLearningOverStatus_get);
EXPORT_SYMBOL(rtk_l2_limitLearningOverStatus_clear);
EXPORT_SYMBOL(rtk_l2_learningCnt_get);
EXPORT_SYMBOL(rtk_l2_limitLearningCnt_get);
EXPORT_SYMBOL(rtk_l2_limitLearningCnt_set);
EXPORT_SYMBOL(rtk_l2_limitLearningCntAction_get);
EXPORT_SYMBOL(rtk_l2_limitLearningCntAction_set);
EXPORT_SYMBOL(rtk_l2_portLimitLearningOverStatus_get);
EXPORT_SYMBOL(rtk_l2_portLimitLearningOverStatus_clear);
EXPORT_SYMBOL(rtk_l2_portLearningCnt_get);
EXPORT_SYMBOL(rtk_l2_portLimitLearningCnt_get);
EXPORT_SYMBOL(rtk_l2_portLimitLearningCnt_set);
EXPORT_SYMBOL(rtk_l2_portLimitLearningCntAction_get);
EXPORT_SYMBOL(rtk_l2_portLimitLearningCntAction_set);
EXPORT_SYMBOL(rtk_l2_aging_get);
EXPORT_SYMBOL(rtk_l2_aging_set);
EXPORT_SYMBOL(rtk_l2_portAgingEnable_get);
EXPORT_SYMBOL(rtk_l2_portAgingEnable_set);
EXPORT_SYMBOL(rtk_l2_lookupMissAction_get);
EXPORT_SYMBOL(rtk_l2_lookupMissAction_set);
EXPORT_SYMBOL(rtk_l2_portLookupMissAction_get);
EXPORT_SYMBOL(rtk_l2_portLookupMissAction_set);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMask_get);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMask_set);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMask_add);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMask_del);
EXPORT_SYMBOL(rtk_l2_newMacOp_get);
EXPORT_SYMBOL(rtk_l2_newMacOp_set);
EXPORT_SYMBOL(rtk_l2_nextValidAddr_get);
EXPORT_SYMBOL(rtk_l2_nextValidAddrOnPort_get);
EXPORT_SYMBOL(rtk_l2_nextValidMcastAddr_get);
EXPORT_SYMBOL(rtk_l2_nextValidIpMcastAddr_get);
EXPORT_SYMBOL(rtk_l2_nextValidEntry_get);
EXPORT_SYMBOL(rtk_l2_addr_add);
EXPORT_SYMBOL(rtk_l2_addr_del);
EXPORT_SYMBOL(rtk_l2_addr_get);
EXPORT_SYMBOL(rtk_l2_addr_delAll);
EXPORT_SYMBOL(rtk_l2_mcastAddr_add);
EXPORT_SYMBOL(rtk_l2_mcastAddr_del);
EXPORT_SYMBOL(rtk_l2_mcastAddr_get);
EXPORT_SYMBOL(rtk_l2_illegalPortMoveAction_get);
EXPORT_SYMBOL(rtk_l2_illegalPortMoveAction_set);
EXPORT_SYMBOL(rtk_l2_ipmcMode_get);
EXPORT_SYMBOL(rtk_l2_ipmcMode_set);
EXPORT_SYMBOL(rtk_l2_ipmcGroupLookupMissHash_get);
EXPORT_SYMBOL(rtk_l2_ipmcGroupLookupMissHash_set);
EXPORT_SYMBOL(rtk_l2_ipmcGroup_add);
EXPORT_SYMBOL(rtk_l2_ipmcGroup_del);
EXPORT_SYMBOL(rtk_l2_ipmcGroup_get);
EXPORT_SYMBOL(rtk_l2_portIpmcAction_get);
EXPORT_SYMBOL(rtk_l2_portIpmcAction_set);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_add);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_del);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_get);
EXPORT_SYMBOL(rtk_l2_srcPortEgrFilterMask_get);
EXPORT_SYMBOL(rtk_l2_srcPortEgrFilterMask_set);
EXPORT_SYMBOL(rtk_l2_extPortEgrFilterMask_get);
EXPORT_SYMBOL(rtk_l2_extPortEgrFilterMask_set);
EXPORT_SYMBOL(rtk_irq_isr_register);
EXPORT_SYMBOL(rtk_irq_isr_unregister);
EXPORT_SYMBOL(rtk_intr_init);
EXPORT_SYMBOL(rtk_intr_polarity_set);
EXPORT_SYMBOL(rtk_intr_polarity_get);
EXPORT_SYMBOL(rtk_intr_imr_set);
EXPORT_SYMBOL(rtk_intr_imr_get);
EXPORT_SYMBOL(rtk_intr_ims_get);
EXPORT_SYMBOL(rtk_intr_ims_clear);
EXPORT_SYMBOL(rtk_intr_speedChangeStatus_get);
EXPORT_SYMBOL(rtk_intr_speedChangeStatus_clear);
EXPORT_SYMBOL(rtk_intr_linkupStatus_get);
EXPORT_SYMBOL(rtk_intr_linkupStatus_clear);
EXPORT_SYMBOL(rtk_intr_linkdownStatus_get);
EXPORT_SYMBOL(rtk_intr_linkdownStatus_clear);
EXPORT_SYMBOL(rtk_intr_gphyStatus_get);
EXPORT_SYMBOL(rtk_intr_gphyStatus_clear);
EXPORT_SYMBOL(rtk_intr_imr_restore);
/*EXPORT_SYMBOL(rtk_init);*/
EXPORT_SYMBOL(rtk_core_init);
EXPORT_SYMBOL(rtk_all_module_init);
#ifdef CONFIG_GPON_FEATURE
#if CONFIG_GPON_VERSION > 1
EXPORT_SYMBOL(rtk_gpon_init);
EXPORT_SYMBOL(rtk_gpon_resetState_set);
EXPORT_SYMBOL(rtk_gpon_resetDoneState_get);
EXPORT_SYMBOL(rtk_gpon_macVersion_get);
EXPORT_SYMBOL(rtk_gpon_test_get);
EXPORT_SYMBOL(rtk_gpon_test_set);
EXPORT_SYMBOL(rtk_gpon_topIntrMask_get);
EXPORT_SYMBOL(rtk_gpon_topIntrMask_set);
EXPORT_SYMBOL(rtk_gpon_topIntr_get);
EXPORT_SYMBOL(rtk_gpon_topIntr_disableAll);
EXPORT_SYMBOL(rtk_gpon_gtcDsIntr_get);
EXPORT_SYMBOL(rtk_gpon_gtcDsIntrDlt_get);
EXPORT_SYMBOL(rtk_gpon_gtcDsIntrMask_get);
EXPORT_SYMBOL(rtk_gpon_gtcDsIntrMask_set);
EXPORT_SYMBOL(rtk_gpon_onuId_set);
EXPORT_SYMBOL(rtk_gpon_onuId_get);
EXPORT_SYMBOL(rtk_gpon_onuState_set);
EXPORT_SYMBOL(rtk_gpon_onuState_get);
EXPORT_SYMBOL(rtk_gpon_dsBwmapCrcCheckState_set);
EXPORT_SYMBOL(rtk_gpon_dsBwmapCrcCheckState_get);
EXPORT_SYMBOL(rtk_gpon_dsBwmapFilterOnuIdState_set);
EXPORT_SYMBOL(rtk_gpon_dsPlendStrictMode_set);
EXPORT_SYMBOL(rtk_gpon_dsPlendStrictMode_get);
EXPORT_SYMBOL(rtk_gpon_dsScrambleState_set);
EXPORT_SYMBOL(rtk_gpon_dsScrambleState_get);
EXPORT_SYMBOL(rtk_gpon_dsFecBypass_set);
EXPORT_SYMBOL(rtk_gpon_dsFecBypass_get);
EXPORT_SYMBOL(rtk_gpon_dsFecThrd_set);
EXPORT_SYMBOL(rtk_gpon_dsFecThrd_get);
EXPORT_SYMBOL(rtk_gpon_extraSnTxTimes_set);
EXPORT_SYMBOL(rtk_gpon_extraSnTxTimes_get);
EXPORT_SYMBOL(rtk_gpon_dsPloamNomsg_set);
EXPORT_SYMBOL(rtk_gpon_dsPloamNomsg_get);
EXPORT_SYMBOL(rtk_gpon_dsPloamOnuIdFilterState_set);
EXPORT_SYMBOL(rtk_gpon_dsPloamOnuIdFilterState_get);
EXPORT_SYMBOL(rtk_gpon_dsPloamBcAcceptState_set);
EXPORT_SYMBOL(rtk_gpon_dsPloamBcAcceptState_get);
EXPORT_SYMBOL(rtk_gpon_dsPloamDropCrcState_set);
EXPORT_SYMBOL(rtk_gpon_dsPloamDropCrcState_get);
EXPORT_SYMBOL(rtk_gpon_dsBwmapFilterOnuIdState_get);
EXPORT_SYMBOL(rtk_gpon_cdrLosStatus_get);
EXPORT_SYMBOL(rtk_gpon_optLosStatus_get);
EXPORT_SYMBOL(rtk_gpon_losCfg_set);
EXPORT_SYMBOL(rtk_gpon_losCfg_get);
EXPORT_SYMBOL(rtk_gpon_dsPloam_get);
EXPORT_SYMBOL(rtk_gpon_usTcont_get);
EXPORT_SYMBOL(rtk_gpon_usTcont_set);
EXPORT_SYMBOL(rtk_gpon_usTcont_del);
EXPORT_SYMBOL(rtk_gpon_dsGemPort_get);
EXPORT_SYMBOL(rtk_gpon_dsGemPort_set);
EXPORT_SYMBOL(rtk_gpon_dsGemPort_del);
EXPORT_SYMBOL(rtk_gpon_dsGemPortPktCnt_get);
EXPORT_SYMBOL(rtk_gpon_dsGemPortByteCnt_get);
EXPORT_SYMBOL(rtk_gpon_dsGtcMiscCnt_get);
EXPORT_SYMBOL(rtk_gpon_dsOmciPti_set);
EXPORT_SYMBOL(rtk_gpon_dsOmciPti_get);
EXPORT_SYMBOL(rtk_gpon_dsEthPti_set);
EXPORT_SYMBOL(rtk_gpon_dsEthPti_get);
EXPORT_SYMBOL(rtk_gpon_aesKeySwitch_set);
EXPORT_SYMBOL(rtk_gpon_aesKeySwitch_get);
EXPORT_SYMBOL(rtk_gpon_aesKeyWord_set);
EXPORT_SYMBOL(rtk_gpon_irq_get);
EXPORT_SYMBOL(rtk_gpon_dsGemPortEthRxCnt_get);
EXPORT_SYMBOL(rtk_gpon_dsGemPortEthFwdCnt_get);
EXPORT_SYMBOL(rtk_gpon_dsGemPortMiscCnt_get);
EXPORT_SYMBOL(rtk_gpon_dsGemPortFcsCheckState_get);
EXPORT_SYMBOL(rtk_gpon_dsGemPortFcsCheckState_set);
EXPORT_SYMBOL(rtk_gpon_dsGemPortBcPassState_set);
EXPORT_SYMBOL(rtk_gpon_dsGemPortBcPassState_get);
EXPORT_SYMBOL(rtk_gpon_dsGemPortNonMcPassState_set);
EXPORT_SYMBOL(rtk_gpon_dsGemPortNonMcPassState_get);
EXPORT_SYMBOL(rtk_gpon_dsGemPortMacFilterMode_set);
EXPORT_SYMBOL(rtk_gpon_dsGemPortMacFilterMode_get);
EXPORT_SYMBOL(rtk_gpon_dsGemPortMacForceMode_set);
EXPORT_SYMBOL(rtk_gpon_dsGemPortMacForceMode_get);
EXPORT_SYMBOL(rtk_gpon_dsGemPortMacEntry_set);
EXPORT_SYMBOL(rtk_gpon_dsGemPortMacEntry_get);
EXPORT_SYMBOL(rtk_gpon_dsGemPortMacEntry_del);
EXPORT_SYMBOL(rtk_gpon_dsGemPortFrameTimeOut_set);
EXPORT_SYMBOL(rtk_gpon_dsGemPortFrameTimeOut_get);
EXPORT_SYMBOL(rtk_gpon_ipv4McAddrPtn_get);
EXPORT_SYMBOL(rtk_gpon_ipv4McAddrPtn_set);
EXPORT_SYMBOL(rtk_gpon_ipv6McAddrPtn_get);
EXPORT_SYMBOL(rtk_gpon_ipv6McAddrPtn_set);
EXPORT_SYMBOL(rtk_gpon_gtcUsIntr_get);
EXPORT_SYMBOL(rtk_gpon_gtcUsIntrDlt_get);
EXPORT_SYMBOL(rtk_gpon_gtcUsIntrMask_get);
EXPORT_SYMBOL(rtk_gpon_gtcUsIntrMask_set);
EXPORT_SYMBOL(rtk_gpon_forceLaser_set);
EXPORT_SYMBOL(rtk_gpon_forceLaser_get);
EXPORT_SYMBOL(rtk_gpon_ploamState_set);
EXPORT_SYMBOL(rtk_gpon_ploamState_get);
EXPORT_SYMBOL(rtk_gpon_indNrmPloamState_set);
EXPORT_SYMBOL(rtk_gpon_indNrmPloamState_get);
EXPORT_SYMBOL(rtk_gpon_dbruState_set);
EXPORT_SYMBOL(rtk_gpon_dbruState_get);
EXPORT_SYMBOL(rtk_gpon_usScrambleState_set);
EXPORT_SYMBOL(rtk_gpon_usScrambleState_get);
EXPORT_SYMBOL(rtk_gpon_usBurstPolarity_set);
EXPORT_SYMBOL(rtk_gpon_usBurstPolarity_get);
EXPORT_SYMBOL(rtk_gpon_eqd_set);
EXPORT_SYMBOL(rtk_gpon_laserTime_set);
EXPORT_SYMBOL(rtk_gpon_laserTime_get);
EXPORT_SYMBOL(rtk_gpon_burstOverhead_set);
EXPORT_SYMBOL(rtk_gpon_usPloam_set);
EXPORT_SYMBOL(rtk_gpon_usAutoPloam_set);
EXPORT_SYMBOL(rtk_gpon_usPloamCrcGenState_set);
EXPORT_SYMBOL(rtk_gpon_usPloamCrcGenState_get);
EXPORT_SYMBOL(rtk_gpon_usPloamOnuIdFilterState_set);
EXPORT_SYMBOL(rtk_gpon_usPloamOnuIdFilter_get);
EXPORT_SYMBOL(rtk_gpon_usPloamBuf_flush);
EXPORT_SYMBOL(rtk_gpon_usGtcMiscCnt_get);
EXPORT_SYMBOL(rtk_gpon_rdi_set);
EXPORT_SYMBOL(rtk_gpon_rdi_get);
EXPORT_SYMBOL(rtk_gpon_usSmalSstartProcState_set);
EXPORT_SYMBOL(rtk_gpon_usSmalSstartProcState_get);
EXPORT_SYMBOL(rtk_gpon_usSuppressLaserState_set);
EXPORT_SYMBOL(rtk_gpon_usSuppressLaserState_get);
EXPORT_SYMBOL(rtk_gpon_gemUsIntr_get);
EXPORT_SYMBOL(rtk_gpon_gemUsIntrMask_get);
EXPORT_SYMBOL(rtk_gpon_gemUsIntrMask_set);
EXPORT_SYMBOL(rtk_gpon_gemUsForceIdleState_set);
EXPORT_SYMBOL(rtk_gpon_gemUsForceIdleState_get);
EXPORT_SYMBOL(rtk_gpon_gemUsPtiVector_set);
EXPORT_SYMBOL(rtk_gpon_gemUsPtiVector_get);
EXPORT_SYMBOL(rtk_gpon_gemUsEthCnt_get);
EXPORT_SYMBOL(rtk_gpon_gemUsGemCnt_get);
EXPORT_SYMBOL(rtk_gpon_gemUsPortCfg_set);
EXPORT_SYMBOL(rtk_gpon_gemUsPortCfg_get);
EXPORT_SYMBOL(rtk_gpon_gemUsDataByteCnt_get);
EXPORT_SYMBOL(rtk_gpon_gemUsIdleByteCnt_get);
EXPORT_SYMBOL(rtk_gpon_dbruPeriod_set);
EXPORT_SYMBOL(rtk_gpon_dbruPeriod_get);
EXPORT_SYMBOL(rtk_gpon_portMacForceMode_set);
EXPORT_SYMBOL(rtk_gpon_port_get);
EXPORT_SYMBOL(rtk_gpon_gtcDsIntrDlt_check);
EXPORT_SYMBOL(rtk_gpon_gtcUsIntrDlt_check);
EXPORT_SYMBOL(rtk_gpon_gemUsIntrDlt_check);
EXPORT_SYMBOL(rtk_gpon_rogueOnt_set);
EXPORT_SYMBOL(rtk_gpon_drainOutDefaultQueue_set);
EXPORT_SYMBOL(rtk_gpon_autoDisTx_set);
#else
EXPORT_SYMBOL(rtk_gpon_driver_initialize);
EXPORT_SYMBOL(rtk_gpon_driver_deInitialize);
EXPORT_SYMBOL(rtk_gpon_device_initialize);
EXPORT_SYMBOL(rtk_gpon_device_deInitialize);
EXPORT_SYMBOL(rtk_gpon_evtHdlStateChange_reg);
EXPORT_SYMBOL(rtk_gpon_evtHdlDsFecChange_reg);
EXPORT_SYMBOL(rtk_gpon_evtHdlUsFecChange_reg);
EXPORT_SYMBOL(rtk_gpon_evtHdlUsPloamUrgEmpty_reg);
EXPORT_SYMBOL(rtk_gpon_evtHdlUsPloamNrmEmpty_reg);
EXPORT_SYMBOL(rtk_gpon_evtHdlPloam_reg);
EXPORT_SYMBOL(rtk_gpon_evtHdlOmci_reg);
EXPORT_SYMBOL(rtk_gpon_callbackQueryAesKey_reg);
EXPORT_SYMBOL(rtk_gpon_evtHdlAlarm_reg);
EXPORT_SYMBOL(rtk_gpon_serialNumber_set);
EXPORT_SYMBOL(rtk_gpon_serialNumber_get);
EXPORT_SYMBOL(rtk_gpon_password_set);
EXPORT_SYMBOL(rtk_gpon_password_get);
EXPORT_SYMBOL(rtk_gpon_parameter_set);
EXPORT_SYMBOL(rtk_gpon_parameter_get);
EXPORT_SYMBOL(rtk_gpon_activate);
EXPORT_SYMBOL(rtk_gpon_deActivate);
EXPORT_SYMBOL(rtk_gpon_ponStatus_get);
EXPORT_SYMBOL(rtk_gpon_isr_entry);
EXPORT_SYMBOL(rtk_gpon_tcont_create);
EXPORT_SYMBOL(rtk_gpon_tcont_destroy);
EXPORT_SYMBOL(rtk_gpon_tcont_get);
EXPORT_SYMBOL(rtk_gpon_dsFlow_set);
EXPORT_SYMBOL(rtk_gpon_dsFlow_get);
EXPORT_SYMBOL(rtk_gpon_usFlow_set);
EXPORT_SYMBOL(rtk_gpon_usFlow_get);
EXPORT_SYMBOL(rtk_gpon_ploam_send);
EXPORT_SYMBOL(rtk_gpon_broadcastPass_set);
EXPORT_SYMBOL(rtk_gpon_broadcastPass_get);
EXPORT_SYMBOL(rtk_gpon_nonMcastPass_set);
EXPORT_SYMBOL(rtk_gpon_nonMcastPass_get);
EXPORT_SYMBOL(rtk_gpon_multicastAddrCheck_set);
EXPORT_SYMBOL(rtk_gpon_multicastAddrCheck_get);
EXPORT_SYMBOL(rtk_gpon_macFilterMode_set);
EXPORT_SYMBOL(rtk_gpon_macFilterMode_get);
EXPORT_SYMBOL(rtk_gpon_mcForceMode_set);
EXPORT_SYMBOL(rtk_gpon_mcForceMode_get);
EXPORT_SYMBOL(rtk_gpon_macEntry_add);
EXPORT_SYMBOL(rtk_gpon_macEntry_del);
EXPORT_SYMBOL(rtk_gpon_macEntry_get);
EXPORT_SYMBOL(rtk_gpon_rdi_set);
EXPORT_SYMBOL(rtk_gpon_rdi_get);
EXPORT_SYMBOL(rtk_gpon_powerLevel_set);
EXPORT_SYMBOL(rtk_gpon_powerLevel_get);
EXPORT_SYMBOL(rtk_gpon_alarmStatus_get);
EXPORT_SYMBOL(rtk_gpon_version_get);
EXPORT_SYMBOL(rtk_gpon_txForceLaser_set);
EXPORT_SYMBOL(rtk_gpon_txForceLaser_get);
EXPORT_SYMBOL(rtk_gpon_txForceIdle_set);
EXPORT_SYMBOL(rtk_gpon_txForceIdle_get);
EXPORT_SYMBOL(rtk_gpon_dsFecSts_get);
EXPORT_SYMBOL(rtk_gpon_version_show);
EXPORT_SYMBOL(rtk_gpon_devInfo_show);
EXPORT_SYMBOL(rtk_gpon_gtc_show);
EXPORT_SYMBOL(rtk_gpon_tcont_show);
EXPORT_SYMBOL(rtk_gpon_dsFlow_show);
EXPORT_SYMBOL(rtk_gpon_usFlow_show);
EXPORT_SYMBOL(rtk_gpon_macTable_show);
EXPORT_SYMBOL(rtk_gpon_globalCounter_show);
EXPORT_SYMBOL(rtk_gpon_tcontCounter_show);
EXPORT_SYMBOL(rtk_gpon_flowCounter_show);
EXPORT_SYMBOL(rtk_gpon_omci_tx);
EXPORT_SYMBOL(rtk_gpon_omci_rx);
EXPORT_SYMBOL(rtk_gpon_autoTcont_set);
EXPORT_SYMBOL(rtk_gpon_autoTcont_get);
EXPORT_SYMBOL(rtk_gpon_autoBoh_set);
EXPORT_SYMBOL(rtk_gpon_autoBoh_get);
EXPORT_SYMBOL(rtk_gpon_eqdOffset_set);
EXPORT_SYMBOL(rtk_gpon_eqdOffset_get);
EXPORT_SYMBOL(gpon_dbg_enable);

#if defined(OLD_FPGA_DEFINED)
EXPORT_SYMBOL(rtk_gpon_pktGen_cfg_set);
EXPORT_SYMBOL(rtk_gpon_pktGen_buf_set);
#endif
EXPORT_SYMBOL(rtk_gpon_globalCounter_get);
EXPORT_SYMBOL(rtk_gpon_tcontCounter_get);
EXPORT_SYMBOL(rtk_gpon_flowCounter_get);
#endif
#endif
EXPORT_SYMBOL(rtk_dot1x_init);
EXPORT_SYMBOL(rtk_dot1x_unauthPacketOper_get);
EXPORT_SYMBOL(rtk_dot1x_unauthPacketOper_set);
EXPORT_SYMBOL(rtk_dot1x_portBasedEnable_get);
EXPORT_SYMBOL(rtk_dot1x_portBasedEnable_set);
EXPORT_SYMBOL(rtk_dot1x_portBasedAuthStatus_get);
EXPORT_SYMBOL(rtk_dot1x_portBasedAuthStatus_set);
EXPORT_SYMBOL(rtk_dot1x_portBasedDirection_get);
EXPORT_SYMBOL(rtk_dot1x_portBasedDirection_set);
EXPORT_SYMBOL(rtk_dot1x_macBasedEnable_get);
EXPORT_SYMBOL(rtk_dot1x_macBasedEnable_set);
EXPORT_SYMBOL(rtk_dot1x_macBasedDirection_get);
EXPORT_SYMBOL(rtk_dot1x_macBasedDirection_set);
EXPORT_SYMBOL(rtk_dot1x_guestVlan_get);
EXPORT_SYMBOL(rtk_dot1x_guestVlan_set);
EXPORT_SYMBOL(rtk_dot1x_guestVlanBehavior_get);
EXPORT_SYMBOL(rtk_dot1x_guestVlanBehavior_set);
EXPORT_SYMBOL(rtk_dot1x_trapPri_get);
EXPORT_SYMBOL(rtk_dot1x_trapPri_set);
EXPORT_SYMBOL(rtk_cpu_init);
EXPORT_SYMBOL(rtk_cpu_awarePortMask_set);
EXPORT_SYMBOL(rtk_cpu_awarePortMask_get);
EXPORT_SYMBOL(rtk_cpu_tagFormat_set);
EXPORT_SYMBOL(rtk_cpu_tagFormat_get);
EXPORT_SYMBOL(rtk_cpu_trapInsertTag_set);
EXPORT_SYMBOL(rtk_cpu_trapInsertTag_get);
EXPORT_SYMBOL(rtk_classify_init);
EXPORT_SYMBOL(rtk_classify_cfgEntry_add);
EXPORT_SYMBOL(rtk_classify_cfgEntry_get);
EXPORT_SYMBOL(rtk_classify_cfgEntry_del);
EXPORT_SYMBOL(rtk_classify_field_add);
EXPORT_SYMBOL(rtk_classify_unmatchAction_set);
EXPORT_SYMBOL(rtk_classify_unmatchAction_get);
EXPORT_SYMBOL(rtk_classify_portRange_set);
EXPORT_SYMBOL(rtk_classify_portRange_get);
EXPORT_SYMBOL(rtk_classify_ipRange_set);
EXPORT_SYMBOL(rtk_classify_ipRange_get);
EXPORT_SYMBOL(rtk_classify_cfSel_set);
EXPORT_SYMBOL(rtk_classify_cfSel_get);
EXPORT_SYMBOL(rtk_classify_cfPri2Dscp_set);
EXPORT_SYMBOL(rtk_classify_cfPri2Dscp_get);
EXPORT_SYMBOL(rtk_classify_permit_sel_set);
EXPORT_SYMBOL(rtk_classify_permit_sel_get);
EXPORT_SYMBOL(rtk_classify_us1pRemarkPrior_set);
EXPORT_SYMBOL(rtk_classify_us1pRemarkPrior_get);
EXPORT_SYMBOL(rtk_vlan_init);
EXPORT_SYMBOL(rtk_vlan_create);
EXPORT_SYMBOL(rtk_vlan_destroy);
EXPORT_SYMBOL(rtk_vlan_destroyAll);
EXPORT_SYMBOL(rtk_vlan_fid_get);
EXPORT_SYMBOL(rtk_vlan_fid_set);
EXPORT_SYMBOL(rtk_vlan_fidMode_get);
EXPORT_SYMBOL(rtk_vlan_fidMode_set);
EXPORT_SYMBOL(rtk_vlan_port_get);
EXPORT_SYMBOL(rtk_vlan_port_set);
EXPORT_SYMBOL(rtk_vlan_extPort_get);
EXPORT_SYMBOL(rtk_vlan_extPort_set);
EXPORT_SYMBOL(rtk_vlan_stg_get);
EXPORT_SYMBOL(rtk_vlan_stg_set);
EXPORT_SYMBOL(rtk_vlan_priority_get);
EXPORT_SYMBOL(rtk_vlan_priority_set);
EXPORT_SYMBOL(rtk_vlan_priorityEnable_get);
EXPORT_SYMBOL(rtk_vlan_priorityEnable_set);
EXPORT_SYMBOL(rtk_vlan_policingEnable_get);
EXPORT_SYMBOL(rtk_vlan_policingEnable_set);
EXPORT_SYMBOL(rtk_vlan_policingMeterIdx_get);
EXPORT_SYMBOL(rtk_vlan_policingMeterIdx_set);
EXPORT_SYMBOL(rtk_vlan_portAcceptFrameType_get);
EXPORT_SYMBOL(rtk_vlan_portAcceptFrameType_set);
EXPORT_SYMBOL(rtk_vlan_vlanFunctionEnable_get);
EXPORT_SYMBOL(rtk_vlan_vlanFunctionEnable_set);
EXPORT_SYMBOL(rtk_vlan_portIgrFilterEnable_get);
EXPORT_SYMBOL(rtk_vlan_portIgrFilterEnable_set);
EXPORT_SYMBOL(rtk_vlan_leaky_get);
EXPORT_SYMBOL(rtk_vlan_leaky_set);
EXPORT_SYMBOL(rtk_vlan_portLeaky_get);
EXPORT_SYMBOL(rtk_vlan_portLeaky_set);
EXPORT_SYMBOL(rtk_vlan_keepType_get);
EXPORT_SYMBOL(rtk_vlan_keepType_set);
EXPORT_SYMBOL(rtk_vlan_portPvid_get);
EXPORT_SYMBOL(rtk_vlan_portPvid_set);
EXPORT_SYMBOL(rtk_vlan_extPortPvid_get);
EXPORT_SYMBOL(rtk_vlan_extPortPvid_set);
EXPORT_SYMBOL(rtk_vlan_protoGroup_get);
EXPORT_SYMBOL(rtk_vlan_protoGroup_set);
EXPORT_SYMBOL(rtk_vlan_portProtoVlan_get);
EXPORT_SYMBOL(rtk_vlan_portProtoVlan_set);
EXPORT_SYMBOL(rtk_vlan_tagMode_get);
EXPORT_SYMBOL(rtk_vlan_tagMode_set);
EXPORT_SYMBOL(rtk_vlan_portFid_get);
EXPORT_SYMBOL(rtk_vlan_portFid_set);
EXPORT_SYMBOL(rtk_vlan_portPriority_get);
EXPORT_SYMBOL(rtk_vlan_portPriority_set);
EXPORT_SYMBOL(rtk_vlan_portEgrTagKeepType_get);
EXPORT_SYMBOL(rtk_vlan_portEgrTagKeepType_set);
EXPORT_SYMBOL(rtk_vlan_transparentEnable_get);
EXPORT_SYMBOL(rtk_vlan_transparentEnable_set);
EXPORT_SYMBOL(rtk_vlan_cfiKeepEnable_get);
EXPORT_SYMBOL(rtk_vlan_cfiKeepEnable_set);
EXPORT_SYMBOL(rtk_vlan_reservedVidAction_get);
EXPORT_SYMBOL(rtk_vlan_reservedVidAction_set);
EXPORT_SYMBOL(rtk_vlan_tagModeIp4mc_get);
EXPORT_SYMBOL(rtk_vlan_tagModeIp4mc_set);
EXPORT_SYMBOL(rtk_vlan_tagModeIp6mc_get);
EXPORT_SYMBOL(rtk_vlan_tagModeIp6mc_set);
EXPORT_SYMBOL(rtk_time_portTransparentEnable_set);
EXPORT_SYMBOL(rtk_time_portTransparentEnable_get);
EXPORT_SYMBOL(rtk_time_init);
EXPORT_SYMBOL(rtk_time_portPtpEnable_get);
EXPORT_SYMBOL(rtk_time_portPtpEnable_set);
EXPORT_SYMBOL(rtk_time_curTime_get);
EXPORT_SYMBOL(rtk_time_curTime_latch);
EXPORT_SYMBOL(rtk_time_refTime_get);
EXPORT_SYMBOL(rtk_time_refTime_set);
EXPORT_SYMBOL(rtk_time_frequency_set);
EXPORT_SYMBOL(rtk_time_frequency_get);
EXPORT_SYMBOL(rtk_time_ptpIgrMsgAction_set);
EXPORT_SYMBOL(rtk_time_ptpIgrMsgAction_get);
EXPORT_SYMBOL(rtk_time_ptpEgrMsgAction_set);
EXPORT_SYMBOL(rtk_time_ptpEgrMsgAction_get);
EXPORT_SYMBOL(rtk_time_meanPathDelay_set);
EXPORT_SYMBOL(rtk_time_meanPathDelay_get);
EXPORT_SYMBOL(rtk_time_rxTime_set);
EXPORT_SYMBOL(rtk_time_rxTime_get);

EXPORT_SYMBOL(rtk_gpio_init);
EXPORT_SYMBOL(rtk_gpio_state_set);
EXPORT_SYMBOL(rtk_gpio_state_get);
EXPORT_SYMBOL(rtk_gpio_mode_set);
EXPORT_SYMBOL(rtk_gpio_mode_get);
EXPORT_SYMBOL(rtk_gpio_databit_set);
EXPORT_SYMBOL(rtk_gpio_databit_get);
EXPORT_SYMBOL(rtk_gpio_intr_set);
EXPORT_SYMBOL(rtk_gpio_intr_get);

EXPORT_SYMBOL(rtk_i2c_init);
EXPORT_SYMBOL(rtk_i2c_enable_set);
EXPORT_SYMBOL(rtk_i2c_enable_get);
EXPORT_SYMBOL(rtk_i2c_width_set);
EXPORT_SYMBOL(rtk_i2c_width_get);
EXPORT_SYMBOL(rtk_i2c_write);
EXPORT_SYMBOL(rtk_i2c_read);
EXPORT_SYMBOL(rtk_i2c_clock_set);
EXPORT_SYMBOL(rtk_i2c_clock_get);
EXPORT_SYMBOL(rtk_i2c_eepMirror_set);
EXPORT_SYMBOL(rtk_i2c_eepMirror_get);
EXPORT_SYMBOL(rtk_i2c_eepMirror_read);
EXPORT_SYMBOL(rtk_i2c_eepMirror_write);

EXPORT_SYMBOL(rtk_l34_lite_init);
EXPORT_SYMBOL(rtk_l34_netif_create);
EXPORT_SYMBOL(rtk_l34_netifPPPoE_set);
EXPORT_SYMBOL(rtk_l34_netifVlan_set);
EXPORT_SYMBOL(rtk_l34_netifRoutingState_set);
EXPORT_SYMBOL(rtk_l34_netifMtu_set);
EXPORT_SYMBOL(rtk_l34_netifIpaddr_set);
EXPORT_SYMBOL(rtk_l34_netifNat_set);
EXPORT_SYMBOL(rtk_l34_netifState_set);
EXPORT_SYMBOL(rtk_l34_netif_get);
EXPORT_SYMBOL(rtk_l34_netif_del);
EXPORT_SYMBOL(rtk_l34_arp_add);
EXPORT_SYMBOL(rtk_l34_arp_get);
EXPORT_SYMBOL(rtk_l34_arp_del);
EXPORT_SYMBOL(rtk_l34_route_add);
EXPORT_SYMBOL(rtk_l34_route_get);
EXPORT_SYMBOL(rtk_l34_route_del);
EXPORT_SYMBOL(rtk_l34_connectTrack_add);
EXPORT_SYMBOL(rtk_l34_connectTrack_get);
EXPORT_SYMBOL(rtk_l34_connectTrack_del);
EXPORT_SYMBOL(rtk_l34_globalCfg_get);
EXPORT_SYMBOL(rtk_l34_route6_add);
EXPORT_SYMBOL(rtk_l34_route6_del);
EXPORT_SYMBOL(rtk_l34_route6_get);
EXPORT_SYMBOL(rtk_l34_neigh6_add);
EXPORT_SYMBOL(rtk_l34_neigh6_del);
EXPORT_SYMBOL(rtk_l34_neigh6_get);
EXPORT_SYMBOL(rtk_l34_netifIp6addr_add);
EXPORT_SYMBOL(rtk_l34_netifIp6addr_del);
EXPORT_SYMBOL(rtk_l34_netifGateway_set);
EXPORT_SYMBOL(rtk_l34_netif_set);
EXPORT_SYMBOL(rtk_l34_netifMac_set);
/*EPON*/
EXPORT_SYMBOL(rtk_epon_init);
EXPORT_SYMBOL(rtk_epon_intrMask_get);
EXPORT_SYMBOL(rtk_epon_intrMask_set);
EXPORT_SYMBOL(rtk_epon_intr_get);
EXPORT_SYMBOL(rtk_epon_intr_disableAll);
EXPORT_SYMBOL(rtk_epon_llid_entry_set);
EXPORT_SYMBOL(rtk_epon_llid_entry_get);
EXPORT_SYMBOL(rtk_epon_forceLaserState_set);
EXPORT_SYMBOL(rtk_epon_forceLaserState_get);
EXPORT_SYMBOL(rtk_epon_laserTime_set);
EXPORT_SYMBOL(rtk_epon_laserTime_get);
EXPORT_SYMBOL(rtk_epon_syncTime_get);
EXPORT_SYMBOL(rtk_epon_registerReq_get);
EXPORT_SYMBOL(rtk_epon_registerReq_set);
EXPORT_SYMBOL(rtk_epon_churningKey_get);
EXPORT_SYMBOL(rtk_epon_churningKey_set);
EXPORT_SYMBOL(rtk_epon_usFecState_get);
EXPORT_SYMBOL(rtk_epon_usFecState_set);
EXPORT_SYMBOL(rtk_epon_dsFecState_get);
EXPORT_SYMBOL(rtk_epon_dsFecState_set);
EXPORT_SYMBOL(rtk_epon_mibCounter_get);
EXPORT_SYMBOL(rtk_epon_mibGlobal_reset);
EXPORT_SYMBOL(rtk_epon_mibLlidIdx_reset);
EXPORT_SYMBOL(rtk_epon_losState_get);
//EXPORT_SYMBOL(rtk_epon_llidEntryNum_get);

EXPORT_SYMBOL(rtk_ponmac_queueDrainOut_set);
EXPORT_SYMBOL(rtk_vlan_lutSvlanHashState_get);
EXPORT_SYMBOL(rtk_vlan_lutSvlanHashState_set);
EXPORT_SYMBOL(rtk_qos_schedulingType_set);
EXPORT_SYMBOL(rtk_qos_schedulingType_get);

EXPORT_SYMBOL(rt_log);


