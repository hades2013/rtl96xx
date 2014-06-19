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
* FILENAME:  system.h
*
* DESCRIPTION:
*
*
* Date Created: May 15, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/h/system.h#1 $
* $Log:$
*
*
**************************************************************************/

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "defs.h"
#include <sys/time.h>
#include <time.h>

#pragma pack(1)		/* pragma pack definition */

typedef struct ODM_SYS_IF_CONFIG_s
{
    UINT8 ucIf;             /* Interface type: 0-WAN, 1-LAN */
    UINT8 ucIfMode;         /* Interface mode: 0-admin,1-signal,2-meida */
    UINT8 ucIfShareMode;    /* Interface mode: 1-independent,0-share */
    UINT8 ucIpMode;         /* Interface IP mode: 0-static,1-dhcp,2-pppoe */
    UINT32 ulIpAddr;        /* Interface static IP address */
    UINT32 ulIpMask;        /* Interface static IP net mask */
    UINT32 ulGwAddr;        /* Interface static IP gateway */
    UINT32 ulDnsAddr;       /* Interface static IP dns */
    UINT8 ucPPPoEAuthType;  /* PPPoE authenticatoin mode: 0-Auto 1-CHAP 2-PAP */
    char acPPPoEUser[32];   /* PPPoE user name */
    char acPPPoEPass[32];   /* PPPoE password */
    UINT8 ucQoSMode;        /* QoS Mode: 0-dscp, 1-tos */
    UINT8 ucQoSVal;         /* QoS value */
    UINT8 ucTagged;         /* Tagged flag: 0-transparent, 1-tag, 2-vlan stacking */
    UINT16 usCVlan;         /* Voice data CVlan: if ucTagged = 1,2 take effect */
    UINT8 ucPriority;       /* Voice data priority: if ucTagged = 1,2 take effect */
    UINT16 usSVLAN;         /* Voice data SVLAN: take effect only in ucTagged = 2 */
}ODM_SYS_IF_CONFIG_t;

enum net_state
{
      NET_DOWN   =           0,
      NET_UP     =           1
};

typedef struct
{
	UINT32 oldIp;
	UINT8 modeChangeType;
       UINT8 ucIf;
	UINT8 ucMode;
	UINT8 index;
	
	UINT8 shareModeChanged;
	UINT8 oldShareMode;
	UINT8 newShareMode;
	UINT8 oldTagMode;
	UINT8 rsved;

} IF_MODE_VOIP_PARA_S;

#pragma pack()
#if 0
void odmSysCfgInit(void);

UINT32 odmSysCfgSnSet(const char *pcSerialNo);

UINT32 odmSysCfgNameSet(const char * pcName, OnChangedCallBack pCallBack);
UINT32 odmSysCfgLocationSet(const char *pcLocation, OnChangedCallBack pCallBack);
UINT32 odmSysCfgNTPServiceSet(UINT8 ucAction, OnChangedCallBack pCallBack);
UINT32 odmSysCfgNTPServerSet(const char *pcAddr, OnChangedCallBack pCallBack);
UINT32 odmSysCfgDateSet(char *pcDate, OnChangedCallBack pCallBack);
UINT32 odmSysCfgTimeSet(char *pcTime, OnChangedCallBack pCallBack);
UINT32 odmSysCfgTimeZoneSet(const char *pcTimeZone, OnChangedCallBack pCallBack);
UINT32 odmSysCfgDayLightSavingSet(UINT8 ucAction, OnChangedCallBack pCallBack);

STATUS odmSysPPPoESet(UINT8 ucIf, UINT8 ucIfMode, IF_MODE_VOIP_PARA_S *pIfModeVoipPara);
STATUS odmSysDHCPSet(UINT8 ucIf, UINT8 ucIfMode, IF_MODE_VOIP_PARA_S *pIfModeVoipPara);

UINT32 odmSysCfgIfShareModeSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 ucIfShareMode);
UINT32 odmSysCfgIfIpModeSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 ucIpMode, IF_MODE_VOIP_PARA_S * pifModePara);
UINT32 odmSysCfgIfIpAddrSet(UINT8 ucIf, UINT8 ucIfMode, UINT32 ulIpAddress);
UINT32 odmSysCfgIfIpMaskSet(UINT8 ucIf, UINT8 ucIfMode, UINT32 ulIpMask);
UINT32 odmSysCfgIfIpGatewaySet(UINT8 ucIf, UINT8 ucIfMode, UINT32 ipAddress);
UINT32 odmSysCfgIfIpDnsSet(UINT8 ucIf, UINT8 ucIfMode, UINT32 ipAddress);
UINT32 odmSysCfgIfPPPoEModeSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 ucIfPPPoEMode);
UINT32 odmSysCfgIfPPPoEUserSet(UINT8 ucIf, UINT8 ucIfMode, char *pcUserName);
UINT32 odmSysCfgIfPPPoEPassSet(UINT8 ucIf, UINT8 ucIfMode, char *pcPassword);
UINT32 odmSysCfgIfQoSModeSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 ucIfQoSMode);
UINT32 odmSysCfgIfQoSValSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 ucQoSVal);
UINT32 odmSysCfgIfTagModeSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 ucIfTagMode);
UINT32 odmSysCfgIfCVLANSet(UINT8 ucIf, UINT8 ucIfMode, UINT16 usCVLAN);
UINT32 odmSysCfgIfPriSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 ucPriority);
UINT32 odmSysCfgIfSVLANSet(UINT8 ucIf, UINT8 ucIfMode, UINT16 usSVLAN);
UINT32 odmSysClearIpInfo(UINT8 ucIf, UINT8 ucIfMode);

/* odmSysCfgIfConfigSet() - the function which start or stop net service */
UINT32 odmSysIfConfigSet(ODM_SYS_IF_CONFIG_t *pstIpConfig);
UINT32 odmSysIfMacAddrSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucMac);

UINT32 odmSysCfgTelnetServiceSet(UINT8 ucAction, OnChangedCallBack pCallBack);
UINT32 odmSysCfgTelnetPortSet(UINT16 usPort, OnChangedCallBack pCallBack);
UINT32 odmSysCfgTelnetSessionNumSet(UINT16 usSessionNum, OnChangedCallBack pCallBack);
UINT32 odmSysCfgSessionTimeoutSet(UINT32 ulTimeout, OnChangedCallBack pCallBack);

UINT32 odmSysCfgWebServiceSet(UINT8 ucAction, OnChangedCallBack pCallBack);
UINT32 odmSysCfgWebPortSet(UINT16 usPort, OnChangedCallBack pCallBack);
UINT32 odmSysCfgWebSessionNumSet(UINT16 usSessionNum, OnChangedCallBack pCallBack);

const char * odmSysCfgNameGet();
const char * odmSysCfgLocationGet();
UINT8 odmSysCfgNTPServiceGet();
const char * odmSysCfgNTPServerGet();
UINT32 odmSysCfgDateGet(char *pcDate);
UINT32 odmSysCfgTimeGet(char * pcTime);
time_t odmSysCfgTimeGetT(time_t * pTime);

UINT32 odmSysCfgIfShareModeGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucIfShareMode);
UINT32 odmSysCfgIfIpModeGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucIfIpMode);
UINT32 odmSysCfgIfIpAddrGet(UINT8 ucIf, UINT8 ucIfMode, UINT32 *pulIpAddress);
UINT32 odmSysCfgIfIpAddrGetS(UINT8 ucIf, UINT8 ucIfMode, char *pcIpAddr);
UINT32 odmSysCfgIfIpMaskGet(UINT8 ucIf, UINT8 ucIfMode, UINT32 *pulIpAddress);
UINT32 odmSysCfgIfIpMaskGetS(UINT8 ucIf, UINT8 ucIfMode, char *pcIpMask);
UINT32 odmSysCfgIfIpGatewayGet(UINT8 ucIf, UINT8 ucIfMode, UINT32 *pulIpAddress);
UINT32 odmSysCfgIfIpGatewayGetS(UINT8 ucIf, UINT8 ucIfMode, char *pcIpAddr);
UINT32 odmSysCfgIfIpDnsGet(UINT8 ucIf, UINT8 ucIfMode, UINT32 *pulIpAddress);
UINT32 odmSysCfgIfIpDnsGetS(UINT8 ucIf, UINT8 ucIfMode, char *pcIpAddr);
UINT32 odmSysCfgIfPPPoEModeGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucPPPoEMode);
UINT32 odmSysCfgIfPPPoEUserGet(UINT8 ucIf, UINT8 ucIfMode, char *pcUserName);
UINT32 odmSysCfgIfPPPoEPassGet(UINT8 ucIf, UINT8 ucIfMode, char *pcPassword);
UINT32 odmSysCfgIfQoSModeGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucQoSMode);
UINT32 odmSysCfgIfQoSValGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucQoSVal);
UINT32 odmSysCfgIfTagModeGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucTagMode);
UINT32 odmSysCfgIfCVLANGet(UINT8 ucIf, UINT8 ucIfMode, UINT16 *pusCVLAN);
UINT32 odmSysCfgIfPriGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucPri);
UINT32 odmSysCfgIfSVLANGet(UINT8 ucIf, UINT8 ucIfMode, UINT16 *pusSVLAN);

/*  ucIf and ucIfMode are primary keys */
UINT32 odmSysIfConfigGet(ODM_SYS_IF_CONFIG_t *pstIpConfig);
UINT32 odmSysIfMacAddrGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucMac);

const char* odmSysCfgTimeZoneGetS();
UINT8 odmSysCfgTimeZoneGet();
UINT8 odmSysCfgDayLightSavingGet();

UINT8 odmSysCfgTelnetServiceGet();
UINT16 odmSysCfgTelnetPortGet();
UINT16 odmSysCfgTelnetSessionNumGet();
UINT32 odmSysCfgSessionTimeoutGet();

UINT8 odmSysCfgWebServiceGet();
UINT16 odmSysCfgWebPortGet();
UINT16 odmSysCfgWebSessionNumGet();

UINT32 odmSysInfoShow(ENV_t *pstEnv);
UINT32 odmSysCfgShow(ENV_t *pstEnv);

STATUS odmSysReset(void);

STATUS odmSysCfgTempDirGet(char *pcDest);
STATUS odmSysCfgLocalTempDirGet(char *pcDest);
STATUS odmSysCfgMsqDirGet(char *pcDest);

UINT32 odmSysSoftwareBuildTimeGet(char * pcDateTime);
UINT32 odmSysCPUPathMtuGet(UINT32 *cpuMtu);
UINT32 odmSysCPUPathMtuSet(UINT32 cpuMtu);

UINT32 odmSysMtuSet(UINT32 sysMtu);
UINT32 odmSysMtuGet(UINT32 *sysMtu);

UINT8 odmSysVoIPPortBaseGet();
UINT32 odmSysVoIPPortBaseSet(UINT8 ucBase);
#endif
#ifdef __cplusplus
}
#endif

#endif /* #ifndef __SYSTEM_H_ */

