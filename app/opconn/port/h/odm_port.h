/*
=============================================================================
     Header Name: odm_storm.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/14		Initial Version
----------------   ------------  ----------------------------------------------
*/

#ifndef  __BRG_PORT_H__
#define __BRG_PORT_H__

#include <vos.h>
#include "errors.h"

#define ODM_PORT_LOG_TRACE()  printf("%s,%d.\n",__FUNCTION__,__LINE__)

#if defined(ONU_1PORT)

#ifdef CTC_MULTICAST_SURPORT
#define 	ODM_NUM_OF_PORTS			4
#else
#define 	ODM_NUM_OF_PORTS			1
#endif

#elif defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6097)
#define 	ODM_NUM_OF_PORTS			4
#elif defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
#define 	ODM_NUM_OF_PORTS			4
#endif
#define 	ODM_MAX_NUM_OF_TRUNK_VLAN	8

#define 	ODM_START_PORT_NUN 		    1
#define     ODM_MAX_GRUP_NUM_PER_PORT   64

#define     ODM_DEF_VLAN_NUM_START      1
#define     ODM_DEF_VLAN_MAX            4094

#define     ODM_CONFIG_VLAN_NUM_START   1
#define     ODM_MAX_VLAN_NUM            4094

#define     ODM_MAX_ETH_PRI             0X7

#define 	ODM_US_CIR					128  		/*kbps*/
#define 	ODM_US_CBS					0X200000  	/*byte*/
#define 	ODM_US_EBS					0XFFFFFF  	/*byte*/

#define 	ODM_DS_CIR					100000		/*kbps*/
#define 	ODM_DS_CBS					256000		/*byte*/
#define 	ODM_DS_EBS					100000		/*byte*/

#if defined(ONU_1PORT)
#define     ODM_NUM_OF_VTT              11
#elif defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6097)
#define     ODM_NUM_OF_VTT              16
#elif defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)
#define     ODM_NUM_OF_VTT              16
#elif defined(ONU_4PORT_AR8327)
#define     ODM_NUM_OF_VTT              64
#endif
#ifdef ONU_1PORT
#define NUMBER_OF_PORTS  1 // rzhou debug 4
#else
#define NUMBER_OF_PORTS	 4
#endif

typedef enum ODM_MIRROR_TYPE_s
{
       ODM_MIRROR_BOTH = 0,
       ODM_MIRROR_INGRESS = 1,
       ODM_MIRROR_EGRESS = 2,
       ODM_INVALID_MIRROR_TYPE = 3
}ODM_MIRROR_TYPE_e;

typedef enum ODM_PORT_SPEED_s
{
	ODM_PORT_SPEED_10M = 10,
	ODM_PORT_SPEED_100M = 100,
	ODM_PORT_SPEED_1000M = 1000,
	ODM_PORT_SPEED_END
}ODM_PORT_SPEED_e;

typedef enum ODM_PORT_DUPLEX_s
{
	ODM_PORT_DUPLEX_HALF = 1,
	ODM_PORT_DUPLEX_FULL = 2,
	ODM_PORT_DUPLEX_END
}ODM_PORT_DUPLEX_e;

typedef enum ODM_PORT_VLAN_MODE_s{
	ODM_VLAN_TRANSPARENT,
	ODM_VLAN_TAG,
  	ODM_VLAN_TRANSLATION,
  	ODM_VLAN_AGGREGATION,
	ODM_VLAN_TRUNK,
	ODM_VLAN_END
}ODM_PORT_VLAN_MODE_e;

typedef struct eth_policing_s {

	UINT32	operation;
	UINT32	cir;
}eth_policing_t;

typedef struct eth_Ratelimit_s {
	UINT32	operation;
	UINT32	cir;
}eth_Ratelimit_t;

typedef struct CTC_STACK_config_s{
  UINT32 linkstate[NUMBER_OF_PORTS];
  eth_policing_t   eth_policing[NUMBER_OF_PORTS];
  eth_Ratelimit_t   eth_Ratelimit[NUMBER_OF_PORTS];
  int   PhyPrtMode[NUMBER_OF_PORTS];
  int   PhyAdmin[NUMBER_OF_PORTS];
  int   PhyPause[NUMBER_OF_PORTS];
  int   VlanMode[NUMBER_OF_PORTS];
  int   uc_vlan[NUMBER_OF_PORTS];
  int   mc_strip[NUMBER_OF_PORTS];
  int   mc_mode;
  int   mc_max_count[NUMBER_OF_PORTS];
  int   mc_fast_leave;
}CTC_STACK_config_t;

#define 	CFG_PORT_CFG				            "/cfg/port.conf"

#define	    CFG_PORT_SECTION                        "Port%0.4d"

#define 	CFG_PORT_ADMIN_STATUS             		"Portadmin"
#define 	CFG_PORT_AUTO_NEG_ENABLE			    "Autonegotiation"
#define 	CFG_PORT_SPEED							"Portspeed"
#define 	CFG_PORT_DUPLEX							"Portduplex"
#define	    CFG_PORT_FLOWCTRL						"Portflowctrol"
#define	    CFG_PORT_LB								"PortLb"
#define	    CFG_PORT_LEARNING						"Portlearning"
#define 	CFG_PORT_MAC_LIMIT						"Portmaclimit"
#define 	CFG_PORT_MAC_LIMIT_NUM				    "LimitNum"
#define 	CFG_PORT_USPOLICING						"Upstreampolicing"
#define 	CFG_PORT_US_POLICING_CIR				"Uscir"
#define 	CFG_PORT_US_POLICING_CBS			    "Uscbs"
#define 	CFG_PORT_US_POLICING_EBS			    "Usebs"
#define 	CFG_PORT_DS_RATE_LIMIT					"Dsratelimit"
#define 	CFG_PORT_DS_CIR							"Dscir"
#define 	CFG_PORT_DS_CBS							"Dscbs"
#define 	CFG_PORT_MULTICAST_TAG_STRIP		    "Multicasttagstrip"
#define     CFG_PORT_MAX_MULTICAST_GROUP	        "Maxmulticastgroup"
#define 	CFG_PORT_VLAN_MODE						"Vlanmode"
#define 	CFG_PORT_DEFAULT_VID					"Defaultvid"
#define     CFG_PORT_DEFAULT_ETHER_PRI              "DefaultPri"
#define 	CFG_PORT_VTT_OLD  						"Vtt.%d.old"
#define 	CFG_PORT_VTT_NEW						"Vtt.%d.new"
#define 	CFG_PORT_VLAN_TRUNK_ENTRY				"vlanTrunk.%d"
#define 	CFG_PORT_VLAN_TRUNK_NUM					"vlanTrunkNum"

#define     CFG_PORT_MIRROR_PORT                     "mirrorPort"
#define     CFG_PORT_MIRROR_PORT_SECTION             "mirrorPortSection"
#define     CFG_PORT_MIRROR_INGRESS_STATE            "ingressState"
#define     CFG_PORT_MIRROR_ENGRESS_STATE            "engressState"
#define     CFG_LOOP_DECTION_SEC                    "LoopDetectionEn"
#define     CFG_LOOP_DECTION_EN                     "Loop Detect En"
#define     CFG_LOOP_DECTION_HOLD_DOWN_TIME         "Loop Hold Down Time"

#define     CFG_MIRROR_CFG                               "/cfg/mirror.conf"
#define     CFG_MIRROR_MIRROR_PORT               "Mirrorport%0.4d"
#define     CFG_MIRROR_MONITOR_PORT            "Monitorport%0.4d"
#define     CFG_MIRROR_INGRESS                       "Mirroringress"
#define     CFG_MIRROR_EGRESS                         "Mirroregress"

#ifdef __cplusplus
extern "C" {
#endif

UINT32 odmPortRangeCheck(UINT32 portId);

UINT32 odmPortLinkStateGet(UINT32 portId,UINT32 *linkStatus);
UINT32 odmPhyAdminStateGet(UINT32 portId,UINT32 *phyStatus);
UINT32 odmPhyAdminStateSet(UINT32 portId,UINT32 phyStatus);

/*******************************************************************************
* odmPortAdminSet
*
* DESCRIPTION:
* this function is used to set admin status of the specified port.
*	 	portId: 			1 .....
*		adminstatus:    enable or disable
* INPUTS:
*
* OUTPUTS:
*		no
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
UINT32 odmPortAdminSet(UINT32 portId,UINT32 adminStatus);

UINT32 odmPortAdminGet(UINT32 portId,UINT32 *adminStatus);

/*******************************************************************************
* odmPortAutoEnableSet
*
* DESCRIPTION:
* this function is used to set auto negocitation status
*	 	portId: 			1 .....
*		adminstatus:    enable or disable
* INPUTS:
*
* OUTPUTS:
*		no
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
UINT32 odmPortAutoEnableSet(UINT32 portId,UINT32 adminStatus);

UINT32 odmPortAutoEnableGet(UINT32 portId,UINT32 *adminStatus);

UINT32 odmPortSpeedSet(UINT32 portId,UINT32 portSpeed);

UINT32 odmPortSpeedGet(UINT32 portId,UINT32 *portSpeed);

UINT32 odmPortDuplexSet(UINT32 portId,UINT32 duplex);

UINT32 odmPortDuplexGet(UINT32 portId,UINT32 *duplex);

UINT32 odmPortFlowCtrolEnableSet(UINT32 portId,UINT32 flowCtrlEnable);

UINT32 odmPortFlowCtrolEnableGet(UINT32 portId,UINT32 *flowCtrlEnable);

UINT32 odmPortLoopbackEnableSet(UINT32 portId,UINT32 lbEnable);

UINT32 odmPortLoopbackEnableGet(UINT32 portId,UINT32 *lbEnable);

UINT32 odmPortLearningEnableSet(UINT32 portId,UINT32 learningEnable);

UINT32 odmPortLearningEnableGet(UINT32 portId,UINT32 *enable);

UINT32 odmPortMacLimitNumEnableSet(UINT32 portId,UINT32 enable,UINT32 numOfMac);

UINT32 odmPortMacLimitNumEnableGet(UINT32 portId,UINT32 *enable,UINT32 *numOfMac);

UINT32 odmPortUsPolicingSet(UINT32 portId,UINT8 enable,UINT32 cir,UINT32 cbs,UINT32 ebs);

UINT32 odmPortUsPolicingGet(UINT32 portId,UINT8 *enable,UINT32 *cir,UINT32 *cbs,UINT32 *ebs);

UINT32 odmPortDsPolicingSet(UINT32 portId,UINT8 enable,UINT32 cir,UINT32 pir);

UINT32 odmPortDsPolicingGet(UINT32 portId,UINT8*enable,UINT32 *cir,UINT32 *pir);

UINT32 odmPortUsPolicingEnableSet(UINT32 portId,UINT32 enable);

UINT32 odmPortUsPolicingEnableGet(UINT32 portId,UINT32 *enable);

UINT32 odmPortUsCirSet(UINT32 portId,UINT32 cir);

UINT32 odmPortUsCirGet(UINT32 portId,UINT32 *cir);

UINT32 odmPortUsCbsSet(UINT32 portId,UINT32 cbs);

UINT32 odmPortUsCbsGet(UINT32 portId,UINT32 *cbs);

UINT32 odmPortUsEbsSet(UINT32 portId,UINT32 ebs);

UINT32 odmPortUsEbsGet(UINT32 portId,UINT32 *ebs);

UINT32 odmPortDsRateLimitEnableSet(UINT32 portId,UINT32 enable);

UINT32 odmPortDsRateLimitEnableGet(UINT32 portId,UINT32 *enable);

UINT32 odmPortDsCirSet(UINT32 portId,UINT32 cir);

UINT32 odmPortDsCirGet(UINT32 portId,UINT32 *cir);

UINT32 odmPortDsCbsSet(UINT32 portId,UINT32 cbs);

UINT32 odmPortDsCbsGet(UINT32 portId,UINT32 *cbs);

UINT32 odmPortMultcastTagStripEnableSet(UINT32 portId,UINT32 enable);

UINT32 odmPortMultcastTagStripEnableGet(UINT32 portId,UINT32 *enable);

UINT32 odmPortMaxMulticastGroupNumSet(UINT32 portId,UINT32 num);

UINT32 odmPortMaxMulticastGroupNumGet(UINT32 portId,UINT32 *num);

UINT32 odmPortVlanModeSet(UINT32 portId,UINT32 vlanMode);

UINT32 odmPortVlanModeGet(UINT32 portId,UINT32 *vlanMode);

UINT32 odmPortDefaultVlanSet(UINT32 portId,UINT32 defaultVlan);
UINT32 odmPortDefaultVlanSetToAny(UINT32 portId,UINT32 defaultVlan);

UINT32 odmPortDefaultEtherPriSet(UINT32 portId,UINT8 defaultEtherPri);

UINT32 odmPortDefaultEtherPriGet(UINT32 portId,UINT8 *defaultEtherPri);

UINT32 odmPortDefaultVlanGet(UINT32 portId,UINT32 *defaultVlan);

UINT32 odmPortVttEntryAdd(UINT32 portId,UINT32 oldTpid,UINT32 oldVlan,UINT32 newTpid,UINT32 newVlan);

UINT32 odmPortVttEntryGet(UINT32 portId,UINT32 entryIndex,UINT32 *valid,UINT32 *oldTpid,UINT32 *oldVlan,UINT32 *newTpid,UINT32 *newVlan);

UINT32 odmPortVttEntryDel(UINT32 portId,UINT32 oldTpid,UINT32 oldVlan,UINT32 newTpid,UINT32 newVlan);

UINT32 odmPortVttSpecifyEntryDel(UINT32 portId,UINT32 entryId);

UINT32 odmPortVttFlush(UINT32 portId);

UINT32 odmPortNumOfVttGet(UINT32 portId,UINT8 *numOfVttRecs);

UINT32 odmPortVlanTrunkEntryAdd(UINT32 portId, UINT32 vlanId);

UINT32 odmPortVlanTrunkEntryDel(UINT32 portId, UINT32 vlanId);

UINT32 odmPortVlanTrunkEntryValueGet(UINT32 portId, UINT32 entryIndex, UINT32 *pVlanId);

UINT32 odmPortVlanTrunkEntryNumGet(UINT32 portId, UINT32 *pEntryNum);

UINT32 odmPortVlanTrunkEntryClear(UINT32 portId);

UINT32 odmPortCfgInit(void);

UINT32 odmPortCtcStackConfigGet(CTC_STACK_config_t * ctcStackConfig);

UINT32 odmPortMirrorPortGet(UINT8 *pPortId);

UINT32 odmPortMirrorPortSet(UINT32 portId);

UINT32 odmPortMirrorIngPortGet(UINT32 portId,UINT32 *pState);

UINT32 odmPortMirrorIngPortSet(UINT32 portId,UINT32 state);

UINT32 odmPortMirrorEgPortGet(UINT32 portId,UINT32 *pState);

UINT32 odmPortMirrorEgPortSet(UINT32 portId,UINT32 state);

UINT32 odmLoopDetectionSet(UINT32 portId, UINT32 uiEnable);
UINT32 odmLoopDetectionGet(UINT32 portId, UINT32 *puiEnable);
UINT32 odmLoopDetectionHoldDownTimeSet(UINT32 portId, UINT32 uiTime);
UINT32 odmLoopDetectionHoldDownTimeGet(UINT32 portId, UINT32 *puiTime);

UINT32 odmMirrorPortSet(char *name, UINT8 portid, UINT8 type, UINT8 enable);
UINT32 odmMirrorPortGet(char *name, UINT8 portid, UINT8 *enable);
UINT32 odmMonitorPortSet(char *name, UINT8 portid, UINT8 enable);
UINT32 odmMonitorPortGet(char *name, UINT8 portid, UINT8 *enable);
UINT32 odmMirrorGroupNameSet(char *name);
UINT32 odmMirrorTypeGet(char *name, UINT8 *type);
UINT32 odmMirrorGroupClear(char *name);
UINT32 odmMirrorGroupCfgDel(char *name);
int odmCheckMirrorGroupExist(char *name);
void odmShowMirrorList(int fd);
#ifdef __cplusplus
}
#endif

#endif

