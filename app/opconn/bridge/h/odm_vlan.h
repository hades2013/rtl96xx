/*
=============================================================================
     Header Name: odm_vlan.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 lwang 				   2008/9/1		Initial Version
----------------   ------------  ----------------------------------------------
*/
#ifndef  __ODM_VLAN_H__
#define __ODM_VLAN_H__


#define DEFAULT_VLANID    1
#define DEFAULT_VLANNAME  "defvlan"

#define VLAN_CONFIG_NAME  "/cfg/vlan.conf"
#define VTT_CONFIG_NAME   "/cfg/vtt.conf"

#define VTT_MODE_SESSION  "vtt"
#define VTT_MODE_KEY      "vtt_mode"

#define VTT_TEMPLET       "vtt_tem%0.4d"
#define VTT_OLDVLAN_key   "o_vid"
#define VTT_NEWVLAN_KEY   "n_vid"

#define VTT_TPID          "vtt_tpid"
#define VTT_TPID_KEY      "tpid"

#define PORT_PVID          "port%0.4_pvid"
#define PORT_PVID_KEY      "pvid"
#define PORT_DEFPRI_KEY    "defpri"


#define VLAN_SESSION      "Vlan%0.4d"
#define VLAN_NAME         "VlanName"
#define VLAN_ID_KEY       "VlanId"
#define PORT_MAP_KEY      "PORTMAP"

#define VOICE_VLAN_ID       "VoiceVlan"
#define VOICE_VLAN_COS      "VoiceCos"
#define VOICE_VLAN_TotalPort"VoiceTotalPort" 
#define VOICE_VLAN_PORT     "VoiceMember" 

#define LOCAL_SWITCH_SEC  "LOCALSWITCH"
#define LOCAL_SWITCH_KEY  "LOCALSW_FLAG"
#define MCAST_VLAN_KEY    "mcast_vlan"

#define VLAN_AGGREGATION_TARGED_ID "VlanAggId" 
#define VLAN_AGGREGATION_AGGED_ID "AggedVlanId"
#define VLAN_AGGREGATION_ENGRESS_ID "EgressPort" 

typedef struct VLAN_AGG_DS_s {
  UINT16 egressPort;
  UINT16 targetVlan;
  UINT16 aggedVlan;
  UINT16 tmpVlan ;
  UINT8 used;
  UINT8 aggedMac[6];
} VLAN_AGG_DS_t;

typedef struct VoiceVlanInfo_s
{
	UINT16 VoiceVlanId;
	UINT16 VoicePort;	
	UINT16 VoiceVlanRuleId;	
	UINT8  VoiceVlanCos;
	UINT8 NumberVoipPort;
	UINT32 VoiceIpAddr; 
}VoiceVlanInfo_t ;

#if 1 /* add by zttan for TW project use for web stats */

typedef struct vlan_info_s
{
 int port;
 int member[4];
 int vid;
}vlan_info_t;

#endif
#define VOICE_VLAN_DEF_COS  6	
#define VOICE_VLAN_DEF_DSCP  46
#define VOICE_VLAN_MAX_PORT   8

#define ODM_MAX_NUM_OF_AGGRE_ENTRY 256

#if defined(ONU_4PORT_AR8228)
#define ODM_MAX_NUM_OF_AGGRE_MAC 15
#else
#define ODM_MAX_NUM_OF_AGGRE_MAC 128
#endif

//OPL_STATUS odmVlanAggLearnAction(UINT16 egressPort, UINT16 aggedVlan, UINT8 *aggedMac);

#endif

