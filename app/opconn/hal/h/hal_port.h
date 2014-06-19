/*
=============================================================================
     Header Name: hal_port.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu	   2009/03/17		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifndef _HAL_PORT_H_
#define _HAL_PORT_H_

#pragma pack (1)

typedef struct HAL_PORT_CFG_INFO_s{
    UINT32 portId;
    UINT32 admin;
    UINT32 speed;
    UINT32 duplex;
    UINT32 autoAdmin;
    UINT16 pvid;
    UINT8  defaultPri;
    UINT32 flowCtrol;
    UINT32 loopback;
    UINT32 linkStatus;
}HAL_PORT_CFG_INFO_t;

typedef struct HAL_PORT_RATE_LIMIT_CFG_s{
    UINT32 portId;
    UINT32 ingressRateLimitEnable;
    UINT32 ingressCir;
    UINT32 ingressCbs;
    UINT32 ingressEbs;
    UINT32 egressRateLimitEnable;
    UINT32 egressCir;
    UINT32 egressCbs;
    UINT32 egressEbs;
}HAL_PORT_RATE_LIMIT_CFG_t;

typedef struct HAL_PORT_VTT_TAB_s{
    UINT32 portId;
    UINT32 vttIndex;
    UINT32 cVlan;
    UINT32 cPri;
    UINT32 cTpid;
    UINT32 sVlan;
    UINT32 sPri;
    UINT32 sTpid;
	UINT32 retVal;
}HAL_PORT_VTT_TAB_t;

typedef struct HAL_PORT_VLAN_TRUNK_ENTRY_s{
	UINT32 portId;
	UINT32 vlanId;
	UINT32 entryIndex;
	UINT32 retVal;
}HAL_PORT_VLAN_TRUNK_ENTRY_t;

typedef struct HAL_PORT_MAC_FILTER_ENTRY_s{
	UINT32 portId;
	UINT16 vlanId;
	UINT8  mac[6];
	UINT32 retVal;
}HAL_PORT_MAC_FILTER_ENTRY_t;

typedef struct HAL_PORT_STATIC_MAC_ENTRY_s{
	UINT32 portId;
	UINT16 vlanId;
	UINT8  mac[6];
	UINT32 retVal;
}HAL_PORT_STATIC_MAC_ENTRY_t;

typedef struct HAL_PORT_BIND_MAC_ENTRY_s{
	UINT32 portId;
	UINT16 vlanId;
	UINT8  mac[6];
	UINT32 retVal;
}HAL_PORT_BIND_MAC_ENTRY_t;


typedef struct HAL_PORT_VLAN_CFG_s{
    UINT32 portId;
    UINT32 vlanMode;
    UINT32 numOfVttEntry;
	UINT32 numOfTrunkEntry;
	UINT32 retVal;
}HAL_PORT_VLAN_CFG_t;

typedef struct HAL_PORT_STORM_CFG_s{
    UINT32 portId;
    UINT32 stormType;
    UINT32 limitEds;
	UINT32 rateLimit;
}HAL_PORT_STORM_CFG_t;

typedef struct HAL_PORT_DLF_FILTER_EN_s{
  UINT32 portId;
  UINT32 enable;
}HAL_PORT_DLF_FILTER_EN_t;

typedef struct HAL_PORT_CTL_LOCK_DROP_EN_s{
  UINT32 portId;
  UINT32 enable;
}HAL_PORT_CTL_LOCK_DROP_EN_t;

typedef struct HAL_PORT_MIRROR_PORT_CFG_s{
  UINT32 portId;
}HAL_PORT_MIRROR_PORT_CFG_t;

typedef struct HAL_PORT_MIRROR_INGRESS_CFG_s{
  UINT32 portId;
  UINT32 state;
}HAL_PORT_MIRROR_INGRESS_CFG_t;

typedef struct HAL_PORT_MIRROR_ENGRESS_CFG_s{
  UINT32 portId;
  UINT32 state;
}HAL_PORT_MIRROR_ENGRESS_CFG_t;

#pragma pack ()


#define HAL_PORT_PRINTF(x)      printf x
#define HAL_PORT_TRACE()        HAL_PORT_PRINTF(("%s,%s,%d\n",__FILE__,__FUNCTION__,__LINE__))
#define HAL_PORT_DEBUG_TRACE()  

#endif
