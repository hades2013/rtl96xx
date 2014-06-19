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
* FILENAME:  cli_cmd_list.h
*
* DESCRIPTION:
*
*
* Date Created: Aug 18, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/h/cli_cmd_list.h#1 $
* $Log:$
*
*
**************************************************************************/

#ifndef __CLI_CMD_LIST_H_
#define __CLI_CMD_LIST_H_

#include "cli_engine.h"

extern STATUS cliCmdHelp(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmdDisable(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmdEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmdQuit(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmdExiter(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmdShowHistory(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmdShowVersion(ENV_t *p,PARA_TABLE_t *pt);
extern STATUS cliCmdReset(ENV_t *pstEnv,PARA_TABLE_t *pt);
extern STATUS cliCmdLogDebugLevel(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdShowLog(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdLogEnable(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdLogClear(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdShowAlarm(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdAlarmClear(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdSuper(ENV_t *pstEnv, PARA_TABLE_t *psPara);
/* begin added by jiangmingli for host debug */
extern STATUS cliCmdHostDbgEn(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
/* end added by jiangmingli for host debug */

/* begin added by ltang for  debug */
extern STATUS cliExClsMatchAdd(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliExClsMatchDel(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
/* end added by ltang for  debug */

extern STATUS cliExClsMatchAdd(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliExClsMatchDel(ENV_t *pstEnv, PARA_TABLE_t *pstPt);

/*syslog cli commands */
extern STATUS cliCmdSyslogEnable(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdShowSyslogServer(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdSetSyslogServer(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdDelSyslogServer(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdSyslogTest(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
/*--------------------------------------------------------------------------
              system configuration module cli commands
----------------------------------------------------------------------------*/
extern STATUS cliCmdSystem(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmdSystemIpAddrSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdSystemTimeSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdSystemHostNameSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdSystemLocationSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdSystemTimeout(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdSystemCPUPathMtu(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdSystemKillSession(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdSystemShow(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdSystemShowCfg(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdSystemSave(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdSystemShowCfgAll(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdSystemErase(ENV_t *pstEnv,PARA_TABLE_t *pstPara);
extern STATUS cliCmdSystemLoadScript(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdSystemUart1Com(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmdSystemMacAddrSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdSystemDividepage(ENV_t *pstEnv, PARA_TABLE_t *pstPara);

/*--------------------------------------------------------------------------
              user configuration module cli commands
----------------------------------------------------------------------------*/
extern STATUS cliCmdUser(ENV_t *pstEnv,PARA_TABLE_t *pt);
extern STATUS cliCmdUserAdd(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdUserSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdUserDelete(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdUserShow(ENV_t *pstEnv, PARA_TABLE_t *pstPara);

/*--------------------------------------------------------------------------
              QoS configuration module cli commands
----------------------------------------------------------------------------*/
extern STATUS cliCmdQoS(ENV_t *pstEnv,PARA_TABLE_t *pt);
extern STATUS cliCmdClearAll(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmdQoSCreateClass(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSDeleteClass(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSShowClass(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSClassMode(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSAddClassMac(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSAddClassIp(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSAddClassCos(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSAddClassVlan(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSAddClassEthType(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSAddClassIpType(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSAddClassDscp(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSAddClassTos(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSAddClassL4Port(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSAddClassAct(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSDelClassAct(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdShowRunQos(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSAddClassRemarkQueue(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSAddClassRemarkEtherPri(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSAddClassRemarkRulePri(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSDelClassPra(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSClassApply(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSClassRemove(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSShowClassRule(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSShowClassMask(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSShowClassAddr(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSShowCfg(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSShowPre(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSShowClassList(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSShowClassAll(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSDelCfg(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSDelRule(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSRmkVid(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdQoSDeleteRulePre(ENV_t *pstEnv, PARA_TABLE_t *pstPt);

/*--------------------------------------------------------------------------
              FDB configuration module cli commands
----------------------------------------------------------------------------*/
extern STATUS cliCmdFDB(ENV_t *pstEnv,PARA_TABLE_t *pt);

/* begin added by jiangmingli, 2008-08-25 */
extern STATUS cliCmdFdbSetAgingTime(ENV_t *pstEnv,PARA_TABLE_t *pt);
extern STATUS cliCmdFdbAddFdbEntry(ENV_t *pstEnv,PARA_TABLE_t *pt);
extern STATUS cliCmdFdbDelFdbEntry(ENV_t *pstEnv,PARA_TABLE_t *pt);
extern STATUS cliCmdFdbDiscardFdbEntry(ENV_t *pstEnv,PARA_TABLE_t *pt);
extern STATUS cliCmdShowAgingTime(ENV_t *pstEnv,PARA_TABLE_t *pt);
extern STATUS cliCmdShowFdbAll(ENV_t *pstEnv,PARA_TABLE_t *pt);
extern STATUS cliCmdShowFdbStatic(ENV_t *pstEnv,PARA_TABLE_t *pt);
extern STATUS cliCmdShowFdbDiscard(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdShowFdbDynamic(ENV_t *pstEnv,PARA_TABLE_t *pt);
extern STATUS cliCmdShowFdbCfg(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdFdbSetSwLearn(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdFdbShowSwLearn(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdFdbMoveFdbEntry(ENV_t * pstEnv,PARA_TABLE_t * pstPt);

extern STATUS cliCmdPortMacFilterAdd(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdPortMacFilterDel(ENV_t *pstEnv, PARA_TABLE_t *pstPara);

extern STATUS cliCmdPortMacFilterClear(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdPortMacFilterShow(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdPortStaticMacAdd(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdPortStaticMacDel(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdPortStaticMacClear(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdPortStaticMacShow(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdPortBindMacAdd(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdPortBindMacDel(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdPortBindMacClear(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdPortBindMacShow(ENV_t *pstEnv, PARA_TABLE_t *pstPara);

/* end added by jiangmingli, 2008-08-25 */

/*--------------------------------------------------------------------------
              multicast configuration module cli commands
----------------------------------------------------------------------------*/
extern STATUS cliCmdMcast(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmdMulticastFastleaveSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastDebugSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastHostTimeoutSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastLastQueryIntervalSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastLastQueryCountSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastLastQueryIntervalSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastModeSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastControlTypeSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastMulticastVlanSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastMulticastVlanDelete(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastMulticastVlanClear(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastPortConfigShow(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastMulticastTagSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastMulticastTagSetTranslation(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastMulticastGroupLimit(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastCfgShow(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastConfigEnable(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastIgmpTest(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastVlan(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastStats(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastGroupLimitAll(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastMulticastGroupCreate(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastMulticastGroupDelete(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastMulticastGroupClear(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastMulticastGroupCreateByIp(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastMulticastGroupDeleteByIp(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdMulticastIgmpConfig(ENV_t *pstEnv, PARA_TABLE_t *pstPara);


/*--------------------------------------------------------------------------
              VLAN configuration module cli commands
----------------------------------------------------------------------------*/
extern STATUS cliCmdLocalSwitchEnable(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdVLAN(ENV_t *pstEnv,PARA_TABLE_t *pt);
extern STATUS cliCmdCreateVlanName(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdDeleteVlanName(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdClearVlan(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdSetVlanTag(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdAddVlanPort(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdDelVlanPort(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdShowVlan(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdShowVlanEntry(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdShowVlanlist(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdShowVlanAll(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdVlanAggAction(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdShowVlanAggAll(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdShowVlanAgg(ENV_t *pstEnv, PARA_TABLE_t *pstPt);



/*--------------------------------------------------------------------------
              port configuration module cli commands
----------------------------------------------------------------------------*/
extern STATUS cliCmdPort(ENV_t *pstEnv,PARA_TABLE_t *pt);

extern STATUS cliCmdPortAdminEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortAutoEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortSpeedSelect(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortDuplexSelect(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortFlowControlEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortLoopbackEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmdPortLearningEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortMacLimitNumEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortMacLimitNumDisable(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortUspolicingEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortUscirConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortUscbsConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortUsebsConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortDsRateLimitEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortDscirConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortDsCbsConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortMulticastTagStripEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortMaxMulticastGroupNumConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortVlanModeConfigTransparent(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortVlanModeConfigTag(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortVlanModeConfigTranslation(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortVlanModeConfigAggregation(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortVlanModeConfigTrunk(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortDefaultVlanConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortVttAdd(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortVttDel(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortVlanTrunkAdd(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortVlanTrunkDel(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortParameterShow(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortListMiscShow(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortMacLimitShow(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortAutonegStateShow(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmdPortVlanShow(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortRateLimitShow(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortVttShow(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortVlanTrunkShow(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPortDsCbsConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara);
/*--------------------------------------------------------------------------
              RSTP configuration module cli commands
----------------------------------------------------------------------------*/
extern STATUS cliCmdRSTP(ENV_t *pstEnv,PARA_TABLE_t *pt);

/*--------------------------------------------------------------------------
              PON configuration module cli commands
----------------------------------------------------------------------------*/
extern STATUS cliCmdPon(ENV_t *pstEnv,PARA_TABLE_t *pt);

extern STATUS cliCmdPonFecEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonLaserCtrlSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonLaserOnSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonLoopbackConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmdPonTxPolarityConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonLaserOffSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonSerdesDelaySet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonMacIdSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonDbaAgentAllParaSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonTxDelaySet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonReportModeSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonQsetThresholdSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonRegSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonRegGet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonMutiRegGet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonMutiTabGet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonTabRecSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonShowTabInfo(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonShowConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdPonParameterShow(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdOnuMpcpHoldoverContrlSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdOnuMpcpHoldoverTimeSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdOnuSilenceEnSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdOnuSilenceTimeSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

/*--------------------------------------------------------------------------
              storm configuration module cli commands
----------------------------------------------------------------------------*/
extern STATUS cliCmdStorm(ENV_t *pstEnv,PARA_TABLE_t *pt);

extern STATUS cliCmdStormEnableBroadcastLimit(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdStormDisableBroadcastLimit(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdStormEnableMulticastLimit(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdStormDisableMulticastLimit(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdStormEnableDlfLimit(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdStormDisableDlfLimit(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdStromShowLimitConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdStormCtrlFrameSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdStormCtrlRateSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

extern STATUS cliCmdStormShow(ENV_t *pstEnv, PARA_TABLE_t *psPara);

/*--------------------------------------------------------------------------
              stats module cli commands
----------------------------------------------------------------------------*/
extern STATUS CliShowCntGe(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS CliShowCntPon(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS CliShowCntFe(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS CliShowCntTm(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS CliShowCntCle(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS CliShowCntBrg(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS CliShowCntMpcp(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS CliShowCntMarb(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS CliShowCntCpdma(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS CliShowCntFedma(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS CliShowCntGeParser(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS CliShowCntCleHit(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS CliCntPollEndis(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS CliCntClr(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS CliShowCntSwhPort(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS CliOamDbgEndis(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS CliOamClearAlarm(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS CliOamDygaspnumConfig(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS CliOamReportAlarm(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS CliOamVersionSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
/*--------------------------------------------------------------------------
              rstp module cli commands
----------------------------------------------------------------------------*/
extern STATUS cliCmdRstpPort(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
/* begin: add by jiangmingli for RSTP process */
extern STATUS cliCmdRstpInfoShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdRstpCfgShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdRstpRecoverBrdSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdRstpRecoverPortSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdRstpPortSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
/* end: add by jiangmingli for RSTP process */
extern STATUS cliCmdRstpEnable(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
/* begin added by jiangmingli for tw_bug 2066 */
extern STATUS cliCmdRstpLoopDetect(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
/* end added by jiangmingli for tw_bug 2066 */
extern STATUS cliCmdRstpSetHello(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdRstpSetForward(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdRstpSetMaxage(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdRstpSetForver(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdRstpSetDiameter(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdRstpSetPriority(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
#if 0
extern STATUS cliCmdRstpPortEnable(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdRstpPortEdge(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdRstpPortP2plink(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdRstpPortPriority(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdRstpPortPathcost(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
#endif

extern STATUS cliCmdRstpHandleModeSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdRstpHandleModeShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt);

/*--------------------------------------------------------------------------
              mirror module cli commands
----------------------------------------------------------------------------*/
extern STATUS cliCmdMirrorGroupMode(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdMirrorPortApply(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdMirrorClear(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdShowPortMirrorAll(ENV_t *pstEnv, PARA_TABLE_t *pstPt);

extern STATUS cliTmCosIcosMapShow(int fd);
extern STATUS cliTmQidIcosMapShow(int fd);
extern STATUS cliTmInQueueCounterShow(int fd);
extern STATUS cliTmShaperConfigShow(int fd);
extern STATUS cliTmInQueueConfigShow(int fd);
extern STATUS cliTmWredConfigShow(int fd);
extern STATUS cliTmQueueWeightShow(int fd);
extern STATUS cliTmQueuePriorityShow(int fd);
extern STATUS cliTmRptValueOfQueueShow(int fd);
extern STATUS cliTmTotalPktsInQueueIncMpcpShow(int fd);
extern STATUS cliBrgGeParserCounterShow(int fd);
extern STATUS cliBrgPortCounterShow(int fd);
extern STATUS cliBrgCounterShow(int fd);
extern STATUS cliTmCounterShow(int fd);
extern STATUS cliPonMpcpCounterShow(int fd);
extern STATUS cliHostDmaCounterShow(int fd);
extern STATUS cliAllCounterShow(int fd);
extern STATUS cliIntStatusShow(int fd);

/**/
extern STATUS cliCmdOrr(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdOwr(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdMrr(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdMwr(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdOrt(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdOwt(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdOst(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdBrgVttAdd(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdBrgVttDel(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdBrgVttShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdVttShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdVlanShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdTmUsShaperSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdTmDsShaperSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdPortQueuePriWeightSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdTmQueueInfoSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdWredSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdTmTabShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdCounterShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdInterruptShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdInterruptSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdSystemProcess(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdSystemFirmwareSwitch(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdI2CTestStart(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdI2CTestStop(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdDDRTestStart(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdDDRTestStop(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdMdioRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdMdioWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdAtherosPhyRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdAtherosPhyWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdAtherosRegRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdAtherosRegWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdAtherosClassifyWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdAtherosMibShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdLinuxShell(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdShowThreadInfo(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS cliCmdOpticalModuleOnOff(ENV_t *pstEnv, PARA_TABLE_t *pstPt);

extern STATUS cliCmdOnuLogicalIdSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt);
extern STATUS  cliCmdLogin(ENV_t *pstEnv, PARA_TABLE_t *psPara);



extern STATUS   cliCmdTftp(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS   cliCmdFtp(ENV_t *pstEnv,PARA_TABLE_t *psPara);
extern STATUS   cliCmdDataPathMtuSizeSet(ENV_t *pstEnv,PARA_TABLE_t *psPara);
extern STATUS   cliCmdDataPathMtuSizeGet(ENV_t *pstEnv, PARA_TABLE_t *psPara);


extern STATUS cliCmdPonBufferResetConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmdQinQ(ENV_t *pstEnv, PARA_TABLE_t *pstPara);

extern STATUS cliCmdQinQMode(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdQinQBase(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdQinQTransparent(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmdQinQFlexible(ENV_t *pstEnv, PARA_TABLE_t *pstPara);
extern STATUS cliCmd25L90iModSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);
extern STATUS cliCmd25L90ApcSet(ENV_t *pstEnv, PARA_TABLE_t *psPara);

#endif  /* #ifdef __CLI_CMD_LIST_H_ */

