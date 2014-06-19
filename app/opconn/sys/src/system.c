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
* FILENAME:  system.c
*
* DESCRIPTION:
*
*
* Date Created: Apr 30, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/src/system.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "defs.h"
#include "system.h"
#include "utils.h"
#include "log.h"
#include <termios.h>
#include <vos_terminal.h>
#include "opconn_hw_reg.h"
#include "hal.h"
/* for pppoe config by xwliu */
#include <dirent.h>
/* for pppoe config by xwliu */
#include "oam.h"
//wfxu 0402 #include "dal_mac.h"
#include "opconn_usr_ioctrl.h"
#include "lw_drv_pub.h"
#include "lw_drv_req.h"

#define MODULE MOD_SYS

#define SYSTEM_CFG                  "/cfg/system.conf"

#define SYSTEM_CFG_GENERAL_SEC      "General"
#define SYSTEM_CFG_SYS_SN           "Serial No"
#define SYSTEM_CFG_SYS_NAME         "System Name"
#define SYSTEM_CFG_SYS_LOCATION     "System Location"

/* for pppoe config by xwliu */
#define SYSTEM_CFG_DHCP_ENABLE    "DHCP"
#define SYSTEM_CFG_PPP_ENABLE    "PPP"

#define SYSTEM_CFG_PPP_USERNAME     "PPP Username"
#define SYSTEM_CFG_PPP_PASSWORD     "PPP Password"
#define SYSTEM_CFG_PPP_INTERFACE    "PPP Interface"
#define SYSTEM_CFG_DHCP_INTERFACE    "DHCP Interface"

/* for pppoe config by xwliu */

#define SYSTEM_CFG_NTP_SERVICE      "NTP Service"
#define SYSTEM_CFG_NTP_SERVER       "NTP Server Address"
#define SYSTEM_CFG_TIME_ZONE        "Time Zone"
#define SYSTEM_CFG_DAY_LIGHT_SAVING "Day Light Saving"

#define SYSTEM_CFG_IF_SEC           "If%d"
#define SYSTEM_CFG_IF_MAC_ADDR      "MAC Address"
#define SYSTEM_CFG_IF_SHARE_MODE    "Share Mode"
#define SYSTEM_CFG_IF_IP_MODE       "IP Mode"
#define SYSTEM_CFG_IF_IP_ADDR       "IP Address"
#define SYSTEM_CFG_IF_IP_MASK       "IP Mask"
#define SYSTEM_CFG_IF_IP_GW         "Gateway"
#define SYSTEM_CFG_IF_IP_DNS        "DNS Server"
#define SYSTEM_CFG_IF_PPPOE_AUTH    "PPPoE Mode"
#define SYSTEM_CFG_IF_PPPOE_USER    "PPPoE User"
#define SYSTEM_CFG_IF_PPPOE_PASS    "PPPoE Pass"
#define SYSTEM_CFG_IF_QOS_MODE      "QoS Mode"
#define SYSTEM_CFG_IF_QOS_VAL       "QoS Value"
#define SYSTEM_CFG_IF_TAG_MODE      "Tag Mode"
#define SYSTEM_CFG_IF_TAG_CVLAN     "CVLAN"
#define SYSTEM_CFG_IF_TAG_PRI       "Priority"
#define SYSTEM_CFG_IF_TAG_SVLAN     "SVLAN"

#define SYSTEM_CFG_TEMP_DIR         "Temp Dir"
#define SYSTEM_CFG_MSQ_DIR          "Msq Dir"

#define SYSTEM_CFG_TELNET_SEC       "Telnet"
#define SYSTEM_CFG_TELNET_SERVICE   "Service"
#define SYSTEM_CFG_TELNET_PORT      "Port"
#define SYSTEM_CFG_TELNET_SESSIONS  "Session Number"
#define SYSTEM_CFG_SESSION_TIMEOUT  "Session Timeout"

#define SYSTEM_CFG_MTU_SEC       	"Mtu"
#define SYSTEM_CFG_MTU_SIZE  		"Cpupath Mtu Size"

#define SYSTEM_CFG_WEB_SEC          "Web"
#define SYSTEM_CFG_WEB_SERVICE      "Service"
#define SYSTEM_CFG_WEB_PORT         "Port"
#define SYSTEM_CFG_WEB_SESSIONS     "Session Number"

#define SYSTEM_CFG_UART1_COMM       "uart1_comm"
#define SYSTEM_CFG_SYS_MTU          "sysMtu"
#define SYSTEM_CFG_CPU_MTU          "cpuMtu"
#define DEFAULT_CPU_MTU             1500

#define HOSTPATH_MTU	((IS_ONU_RESPIN)?2000:1536)

#define SYSTEM_CFG_VOIP_PORT_BASE   "VoIP Port Base"
#define DEFAULT_VOIP_PORT_BASE      1

/* ip mode changed, notify voip */
#define IP_CHANGE_INACTIVE_VOIP 0
#define IP_CHANGE_ACTIVE_VOIP 1

typedef enum IP_MODE_CHANGE_TYPE_e
{
    IP_MODE_STAY_SAME = 0,
    IP_MODE_STATIC_TO_DHCP,
    IP_MODE_STATIC_TO_PPPOE,
    IP_MODE_DHCP_TO_STATIC,
    IP_MODE_DHCP_TO_PPPOE,
    IP_MODE_PPPOE_TO_STATIC,
    IP_MODE_PPPOE_TO_DHCP,
    IP_MODE_INIT_START,
    IP_MODE_CHANGE_TYPE_MAX,
}IP_MODE_CHANGE_TYPE_t;

typedef struct VoiceVlan_s
{
	UINT16 VoiceVlanId;
	UINT8  VoiceVlanCos;
	UINT8 VoipVlanDSCP;
	UINT8 VoipTagValid;
	UINT8 VoipPortNum;
  UINT16 usVlanTpid;	/* CVLAN Tpid */
  UINT8 VoipQinQEn;    /* 0 - disable QinQ, 1 - enable QinQ */
  UINT8  VoiceSVlanCos; /* SVLAN COS */
  UINT16 VoiceSVlanId; /* SVLAN */
  UINT16 usSVlanTpid;	/* SVLAN Tpid */
}VoiceVlan_t;
#define NR_START 4000 //_MIPS_SIM_ABI32:4000,316; _MIPS_SIM_ABI64:5000,275; _MIPS_SIM_NABI32:6000,279
#define NR_CFG_VOICEVLAN (NR_START + 320)

/* ip mode changed, notify voip */
extern const char *g_pcModule[MAX_MODULE];
extern const char *g_pcDebugLevel[DEBUG_LEVEL_MAX];
extern UINT32 oam_ver_type;
extern UINT8 oam_cfg_ver_type;
extern int OamStatusGet(u8_t *pState);

int g_anIpIfChildPid[INTERFACE_MAX*INTERFACE_MODE_MAX];
static VOS_THREAD_t g_pastDHCPThreadId[INTERFACE_MAX*INTERFACE_MODE_MAX] = {NULL};
static VOS_THREAD_t g_pstPPPoEThreadId[INTERFACE_MAX*INTERFACE_MODE_MAX] = {NULL};

static IF_MODE_VOIP_PARA_S *g_pastDHCPThreadPara[INTERFACE_MAX*INTERFACE_MODE_MAX] = {NULL};
static IF_MODE_VOIP_PARA_S *g_pstPPPoEThreadPara[INTERFACE_MAX*INTERFACE_MODE_MAX] = {NULL};

static VOS_MUTEX_t g_pastIpModeMutex;
static VOS_MUTEX_t g_pastIpModeThreadMutex;

const char *g_acIfStr[INTERFACE_MAX] = { "LAN", "WAN"};

#if defined(INTERFACE_THREE_WAN_THREE_LAN)
const char *g_acIfModeStr[INTERFACE_MODE_MAX] = {"Admin", "Signal", "Media"};
#elif defined(INTERFACE_TWO_WAN_TWO_LAN)
const char *g_acIfModeStr[INTERFACE_MODE_MAX] = {"Admin", "Signal"};
#elif defined(INTERFACE_ONE_WAN_ONE_LAN)
const char *g_acIfModeStr[INTERFACE_MODE_MAX] = {"Admin"};
#endif

const char *g_acIfShareModeStr[INTERFACE_SHARE_MODE_MAX] = {"Share", "Independent"};
const char *g_acIfIpModeStr[IF_IP_MODE_MAX] = {"Static", "DHCP", "PPPoE"};
const char *g_acPPPoEAuthStr[PPPOE_AUTH_TYPE_MAX] = {"Auto", "CHAP", "PAP"};
const char *g_acIfTagModeStr[IF_TAG_MODE_MAX] = {"Transparent", "Tag", "Vlan-Stacking"};

#if defined(INTERFACE_THREE_WAN_THREE_LAN)
const char *g_acIfEthNameStr[INTERFACE_MAX*INTERFACE_MODE_MAX] =
    {"eth0", "eth1", "eth2", "eth3", "eth4", "eth5"};
#elif defined(INTERFACE_TWO_WAN_TWO_LAN)
const char *g_acIfEthNameStr[INTERFACE_MAX*INTERFACE_MODE_MAX] =
    {"eth0", "eth1", "eth2", "eth3"};
#elif defined(INTERFACE_ONE_WAN_ONE_LAN)
const char *g_acIfEthNameStr[INTERFACE_MAX*INTERFACE_MODE_MAX] =
    {"eth0", "eth1"};
#endif

static char g_acIfPPPoENameStr[INTERFACE_MAX*INTERFACE_MODE_MAX][32];

#define IF_MODE_TO_INDEX(ucIf, ucIfMode) (ucIf * INTERFACE_MODE_MAX + ucIfMode)
#define INDEX_TO_IF_MODE(ucIndex, ucIf, ucIfMode) \
{ \
    (ucIf) = (ucIndex) / INTERFACE_MODE_MAX; \
    (ucIfMode) = (ucIndex) % INTERFACE_MODE_MAX; \
}

#define IS_INVALID_IF_INDEX(ucIndex) \
    (ucIndex >= INTERFACE_MAX*INTERFACE_MODE_MAX)


typedef struct
{
    int st_mon, st_day, end_mon, end_day;
} DST_ADJ_TABLE_S;

typedef struct
{
	UINT32 oldIp;
	UINT8 modeChangeType;
    UINT8 ucIf;
	UINT8 ucMode;
	UINT8 index;
} IF_MODE_INDEX_S;


/* Starting from 2005 */
DST_ADJ_TABLE_S Sys_dst_adj_table[20] =
{ { 4, 3, 10, 30 }, /* 2005 */
  { 4, 2, 10, 29 }, /* 2006 */
  { 3, 11, 11, 4 }, /* 2007 */
  { 3, 9, 11, 2 },  /* 2008 */
  { 3, 8, 11, 1 },  /* 2009 */
  { 3, 14, 11, 7 }, /* 2010 */
  { 3, 13, 11, 6 }, /* 2011 */
  { 3, 11, 11, 4 }, /* 2012 */
  { 3, 10, 11, 3 }, /* 2013 */
  { 3, 9, 11, 2 },  /* 2014 */
  { 3, 8, 11, 1 },  /* 2015 */
  { 3, 13, 11, 6 }, /* 2016 */
  { 3, 12, 11, 5 }, /* 2017 */
  { 3, 11, 11, 4 }, /* 2018 */
  { 3, 10, 11, 3 }, /* 2019 */
  { 3, 8, 11, 1 },  /* 2020 */
  { 3, 14, 11, 7 }, /* 2021 */
  { 3, 13, 11, 6 }, /* 2022 */
  { 3, 12, 11, 5 }, /* 2023 */
  { 3, 10, 11, 3 }, /* 2024 */
};

static UINT32 mNtpSec = 0;
static UINT32 mNtpNSec = 0;
static int mNtpTimeZone = 0; /* in second */
static UINT32 mUpTime = 0;

extern const char *build_date;
extern const char *build_time;

/* ip mode changed, notify voip */
int ipModeChangeNotifyVoip(enum net_state activeFlag, UINT8 modeFlag, UINT8 ucIf, UINT8 ucIfMode, IF_MODE_VOIP_PARA_S *pIfModeVoipPara);
int odmSysStaticIpSet(UINT8 ucIf, UINT8 ucIfMode, IF_MODE_VOIP_PARA_S *pIfModeVoipPara);
/* ip mode changed, notify voip */

void odmSysCfgInit(void)
{
    UINT8 ucIf, ucIfMode, ucIfShareMode, ucIfIpMode;
	UINT8	index;
    UINT8 	IfSectionBuff[32] = {0};
	UINT8   oldMacId[6] = {0};
	UINT8   MacId[6] = {0};
    char    acString[128] = {0};
	int ret=0;
    char    *pVal = NULL;
    UINT8   macIdString[20] = {0};
	UINT32  cpuMTU, ulIpAddress ;
	VoiceVlan_t SysVoiceVlan;
	UINT16 usCVlan;
	UINT8 ucPriority;
	UINT8 ucTagMode;
	UINT8	ucQinQ;
	UINT16	usSVlan;
	UINT8	ucSPriority;

    mNtpTimeZone = odmSysCfgTimeZoneGet();
    IF_MODE_VOIP_PARA_S ifModeChangePara;

    /* for now, telnet service is opened default */
    /*begin modified by liaohongjun 2012/9/3 of QID0015*/
    odmSysCfgTelnetServiceSet(ENABLED, NULL);//open by huangmingjian 2012/09/07 for EPN104QID0018
    /*end modified by liaohongjun 2012/9/3 of QID0015*/
    ifModeChangePara.modeChangeType = IP_MODE_INIT_START;

    vosSystem("touch /etc/resolv.conf");

    vosMutexCreate(&g_pastIpModeMutex);

    vosMutexCreate(&g_pastIpModeThreadMutex);

	/* bug 3108: remove the action about "ifconfig down and up" when configure lan and wan interface, or it will cause failure for the pppoe connection .*/

	/* config wan interface mac address */
	ucIf = INTERFACE_WAN;
	{
        for (ucIfMode =0; ucIfMode < INTERFACE_MODE_MAX; ucIfMode++)
        {
    		index = IF_MODE_TO_INDEX(ucIf, ucIfMode);

			vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
            pVal=vosConfigValueGet(SYSTEM_CFG,
        		IfSectionBuff,SYSTEM_CFG_IF_MAC_ADDR,"00:00:00:00:00:00");
			utilStrToMacAddr(pVal,MacId);
            if(memcmp(MacId,&oldMacId[0],MAC_LENGTH))
            {
#if 0 /* new kernel can not up and down interface */
                vosSprintf(acString, "ifconfig %s down", g_acIfEthNameStr[index]);
                vosSystem(acString);
#endif
                vosSprintf(macIdString,"%02x:%02x:%02x:%02x:%02x:%02x",
                    MacId[0],
                    MacId[1],
                    MacId[2],
                    MacId[3],
                    MacId[4],
                    MacId[5]);
                vosSprintf(acString, "ifconfig %s hw ether %s ", g_acIfEthNameStr[index],pVal);                
                vosSystem(acString);

#if 0 /* new kernel can not up and down interface */
                vosSprintf(acString, "ifconfig %s up", g_acIfEthNameStr[index]);
                vosSystem(acString);
#endif
            }
        }
	}

	/* config lan interface mac address */
	ucIf = INTERFACE_LAN;
	{
        for (ucIfMode =0; ucIfMode < INTERFACE_MODE_MAX; ucIfMode++)
        {
    		index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
			vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
            pVal=vosConfigValueGet(SYSTEM_CFG,
        		IfSectionBuff,SYSTEM_CFG_IF_MAC_ADDR,"00:00:00:00:00:00");
			utilStrToMacAddr(pVal,MacId);
        	vosHWAddrGet(g_acIfEthNameStr[index],&oldMacId[0]);
            if ((0 != memcmp(MacId,&oldMacId[0],MAC_LENGTH)) &&
                (0 != memcmp("00:00:00:00:00:00",pVal,17)))
            {
#if 0 /* new kernel can not up and down interface */
                vosSprintf(acString, "ifconfig %s down", g_acIfEthNameStr[index]);
                vosSystem(acString);
#endif
                vosSprintf(macIdString,"%02x:%02x:%02x:%02x:%02x:%02x",
                    MacId[0],
                    MacId[1],
                    MacId[2],
                    MacId[3],
                    MacId[4],
                    MacId[5]);
                vosSprintf(acString, "ifconfig %s hw ether %s ", g_acIfEthNameStr[index],pVal);
                vosSystem(acString);
#if 0 /* new kernel can not up and down interface */
                vosSprintf(acString, "ifconfig %s up", g_acIfEthNameStr[index]);
                vosSystem(acString);
#endif
                //dalArlMgmtMacRdt2Cpu(oldMacId, MacId);
            }
            else 
            {
                //dalArlMgmtMacRdt2Cpu(oldMacId, oldMacId);
            }
        }
	}
    ret = odmSysCPUPathMtuGet(&cpuMTU);
    if(OK == ret)
    {
        ret = odmSysCPUPathMtuSet(cpuMTU);	//change the CPU path MTU configuration API.
		if(OK != ret)
		{
			printf("set cpu mtu %d failed.\n", cpuMTU);
		}
		else
		{
            /* begin modifieded by liaohongjun of QID0017*/
			VOS_DBG("set cpu mtu %d success.\n", cpuMTU);
            /* end modifieded by liaohongjun of QID0017*/
		}
    }

	for (ucIf = 0; ucIf < INTERFACE_MAX; ucIf++)
	{
        for (ucIfMode =0; ucIfMode < INTERFACE_MODE_MAX; ucIfMode++)
        {
            index = IF_MODE_TO_INDEX(ucIf,ucIfMode);
            ifModeChangePara.ucIf = ucIf;
            ifModeChangePara.ucMode = ucIfMode;
            ifModeChangePara.index = index;

            odmSysCfgIfShareModeGet(ucIf, ucIfMode, &ucIfShareMode);

			/* for share mode, just admin mode can set mode value */
            if (ucIfMode == INTERFACE_MODE_ADMIN && ucIfShareMode == INTERFACE_SHARE_MODE_SHARE)
			{
    			vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index+1);
    			vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,
                    SYSTEM_CFG_IF_SHARE_MODE,ucIfShareMode);
            }
			
			/* for share mode, Skip media or signal interface */
            if (ucIfShareMode == INTERFACE_SHARE_MODE_SHARE && ucIfMode != INTERFACE_MODE_ADMIN )
			{
                continue;
            }

        	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);

            if (index != IF_MODE_TO_INDEX(INTERFACE_LAN,INTERFACE_MODE_ADMIN))
            {
                vosIPAddrSet((UINT8 *)g_acIfEthNameStr[index], "0.0.0.0");
#if 0 /*modify wfxu for lan ip set sucess*/
                ucIfIpMode = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
                    IfSectionBuff, SYSTEM_CFG_IF_IP_MODE, 0);
#endif                    
            }
           ucIfIpMode = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
                    IfSectionBuff, SYSTEM_CFG_IF_IP_MODE, 0);
           /*modifieded by liaohongjun of QID0017*/
            VOS_DBG("%s(%d): %s - %s, IP mode is %s\r\n",
                __FUNCTION__, __LINE__,
                g_acIfStr[ucIf],
                g_acIfModeStr[ucIfMode],
                g_acIfIpModeStr[ucIfIpMode]);
            if (ucIfIpMode == IF_IP_MODE_STATIC)
            {
                ulIpAddress = vosConfigUInt32Get(SYSTEM_CFG,
                    IfSectionBuff, SYSTEM_CFG_IF_IP_ADDR, 0);
            	if (0 != ulIpAddress)
            	{
                    cliIpToStr (ulIpAddress, acString);
                    /* Update ip address here */
                    vosIPAddrSet((UINT8 *)g_acIfEthNameStr[index], acString);
            	}
#if 0  /* deleted by Gan Zhiheng - 2010/04/24 */
                else if (ucIf == INTERFACE_LAN && ucIfMode == INTERFACE_MODE_ADMIN) {
                    printf("Set LAN administration interface to %s\r\n", 
                        DEFAULT_LAN_ADMIN_IP);
                    vosIPAddrSet((UINT8 *)g_acIfEthNameStr[index], DEFAULT_LAN_ADMIN_IP);
                }
#endif /* #if 0 */

                ulIpAddress = vosConfigUInt32Get(SYSTEM_CFG,
                    IfSectionBuff,SYSTEM_CFG_IF_IP_MASK,0);
            	if (0 != ulIpAddress)
            	{
                    cliIpToStr (ulIpAddress, acString);
                    /* Update ip mask here */
                    vosNetMaskSet((UINT8 *)g_acIfEthNameStr[index], acString);
            	}
#if 0  /* deleted by Gan Zhiheng - 2010/04/24 */
                else if (ucIf == INTERFACE_LAN && ucIfMode == INTERFACE_MODE_ADMIN) {
                    vosNetMaskSet((UINT8 *)g_acIfEthNameStr[index], DEFAULT_LAN_ADMIN_MASK);
                }
#endif /* #if 0 */

                ulIpAddress = vosConfigUInt32Get(SYSTEM_CFG,
                    IfSectionBuff,SYSTEM_CFG_IF_IP_GW,0);
            	if (0 != ulIpAddress)
            	{
                    cliIpToStr (ulIpAddress, macIdString);
                    vosSprintf(acString, "route add default gw %s dev %s", macIdString, g_acIfEthNameStr[index]);
            	    vosSystem(acString);
            	}

                ulIpAddress = vosConfigUInt32Get(SYSTEM_CFG,
                    IfSectionBuff,SYSTEM_CFG_IF_IP_DNS,0);
            	if (0 != ulIpAddress)
            	{
                    cliIpToStr (ulIpAddress, macIdString);
                    vosSprintf(acString, "\necho nameserver %s >> /etc/resolv.conf", macIdString);
            	    vosSystem(acString);
            	}
            }

			ucTagMode= vosConfigUInt32Get(SYSTEM_CFG,
				IfSectionBuff, SYSTEM_CFG_IF_TAG_MODE, 0);
			if(ucTagMode==IF_TAG_MODE_TAG)
			{
				usCVlan = vosConfigUInt32Get(SYSTEM_CFG,
                	IfSectionBuff, SYSTEM_CFG_IF_TAG_CVLAN, 0);
				ucPriority = vosConfigUInt32Get(SYSTEM_CFG,
                	IfSectionBuff, SYSTEM_CFG_IF_TAG_PRI, 0);
				if(usCVlan!=0 && ucPriority <= 7)
				{
					vosMemSet(&SysVoiceVlan,0,sizeof(VoiceVlan_t));
					SysVoiceVlan.VoiceVlanCos=ucPriority;
					SysVoiceVlan.VoipTagValid=1;
					SysVoiceVlan.VoipPortNum=index;
					SysVoiceVlan.VoiceVlanId=usCVlan;
					SysVoiceVlan.VoipQinQEn=0;
					printf("for interface %d cos %d vlan %d tag %d \n\n",index,ucPriority,usCVlan,SysVoiceVlan.VoipTagValid);
					syscall(NR_CFG_VOICEVLAN, (char*)&SysVoiceVlan, sizeof(SysVoiceVlan));
				}
        	}
			else if(ucTagMode==IF_TAG_MODE_VLAN_STACKING)
			{
				usCVlan = vosConfigUInt32Get(SYSTEM_CFG,
                	IfSectionBuff, SYSTEM_CFG_IF_TAG_CVLAN, 0);
				ucPriority = vosConfigUInt32Get(SYSTEM_CFG,
                	IfSectionBuff, SYSTEM_CFG_IF_TAG_PRI, 0);
				usSVlan = vosConfigUInt32Get(SYSTEM_CFG,
                	IfSectionBuff, SYSTEM_CFG_IF_TAG_SVLAN, 0);
				if(usCVlan!=0 && ucPriority <= 7)
				{
					vosMemSet(&SysVoiceVlan,0,sizeof(VoiceVlan_t));
					SysVoiceVlan.VoiceVlanCos=ucPriority;
					SysVoiceVlan.VoipTagValid=1;
					SysVoiceVlan.VoipPortNum=index;
					SysVoiceVlan.VoiceVlanId=usCVlan;
			        SysVoiceVlan.VoipQinQEn=1;
					SysVoiceVlan.VoiceSVlanCos=ucPriority;
					SysVoiceVlan.VoiceSVlanId=usSVlan;
					OP_DEBUG(DEBUG_LEVEL_INFO, "for interface %d cos %d vlan %d\n",index,ucPriority,usCVlan);
					syscall(NR_CFG_VOICEVLAN, (char*)&SysVoiceVlan, sizeof(SysVoiceVlan));
				}
			}

            if (IF_IP_MODE_STATIC == ucIfIpMode)
            {
                odmSysStaticIpSet(ucIf, ucIfMode, &ifModeChangePara);
            }
            else if (IF_IP_MODE_DHCP == ucIfIpMode)
            {
                odmSysDHCPSet(ucIf, ucIfMode, &ifModeChangePara);
            }
            else if (IF_IP_MODE_PPPOE == ucIfIpMode)
            {
                odmSysPPPoESet(ucIf, ucIfMode, &ifModeChangePara);
            }
        }
	}
}

UINT32 odmSysCfgSnSet(const char *pcSerialNo)
{
    int retVal;
    char *pcVal = NULL;

    if (pcSerialNo == NULL || (pcSerialNo != NULL && pcSerialNo[0] == '\0')) {
        return ERR_NULL_POINTER;
    }

	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,SYSTEM_CFG_SYS_SN);
    pcVal = vosConfigValueGet(SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,SYSTEM_CFG_SYS_SN,NULL);
	if((NO_ERROR == retVal && (vosStrCmp(pcSerialNo, pcVal)))||(NO_ERROR != retVal))
	{
		vosConfigValueSet(SYSTEM_CFG,
            SYSTEM_CFG_GENERAL_SEC,
            SYSTEM_CFG_SYS_SN,
            pcSerialNo);
	}

    return NO_ERROR;
}

UINT32 odmSysCfgNameSet(const char * pcName, OnChangedCallBack pCallBack)
{
    int rv;

    if (pcName == NULL || (pcName != NULL && pcName[0] == '\0')) {
        return INVALID_32;
    }

    rv = vosConfigValueSet(
        SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,
        SYSTEM_CFG_SYS_NAME,
        pcName);

    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)pcName, 2, 3, 4);
    }

    return NO_ERROR;
}

UINT32 odmSysCfgLocationSet(const char *pcLocation, OnChangedCallBack pCallBack)
{
    int rv;

    if (pcLocation == NULL || (pcLocation != NULL && pcLocation[0] == '\0')) {
        return INVALID_32;
    }

    rv = vosConfigValueSet(
        SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,
        SYSTEM_CFG_SYS_LOCATION,
        pcLocation);

    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)pcLocation, 2, 3, 4);
    }

    return NO_ERROR;
}


UINT32 odmSysCfgNTPServiceSet(UINT8 ucAction, OnChangedCallBack pCallBack)
{
    int rv;

    rv = vosConfigValueSet(
        SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,
        SYSTEM_CFG_NTP_SERVICE,
        (ucAction == ENABLED) ? STATE_ENABLE_STR : STATE_DISABLE_STR);
    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)ucAction, 2, 3, 4);
    }

    return NO_ERROR;
}

UINT32 odmSysCfgNTPServerSet(const char *pcAddr, OnChangedCallBack pCallBack)
{
    int rv;

    rv = vosConfigValueSet(
        SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,
        SYSTEM_CFG_NTP_SERVER,
        pcAddr);
    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)pcAddr, 2, 3, 4);
    }

    return NO_ERROR;

}

int getTimeDstAdj()
{
    DST_ADJ_TABLE_S * d;
    time_t adjTime;
    struct tm * t;
    int adj;
    time_t now;

    time(&now);

    adj = 0;
    if (ENABLED == odmSysCfgDayLightSavingGet())
    {
        adjTime = now + mNtpTimeZone;
        t = gmtime(&adjTime);
        t->tm_year -= 105;
        if ((t->tm_year < 0) || (t->tm_year > 19))
        {
            /*printf("<year %d!>", t->tm_year);*/
            return 0;
        }
        d = Sys_dst_adj_table + t->tm_year;
        if (t->tm_mon == d->st_mon)
        {
            if (t->tm_mday >= d->st_day)
            {
                adj = 1;
            }
            else
            {
                /*printf("<%d/%d!>", t->tm_mon, t->tm_mday);*/
            }
        }
        else if (t->tm_mon > d->st_mon)
        {
            if (t->tm_mon < d->end_mon)
            {
                adj = 1;
            }
            else if ((t->tm_mon == d->end_mon) && (t->tm_mday < d->end_day))
            {
                adj = 1;
            }
            else
            {
                /*printf("<%d:%d!>", t->tm_mon, t->tm_mday);*/
            }
        }
        else
        {
            /*printf("<%d-%d!>", t->tm_mon, t->tm_mday);*/
        }
    }
    else
    {
        /*printf("<.>");*/
    }

    if (adj)
    {
        adj = 3600;
    }

    return adj;
}


/*******************************************************************************
*
* odmSysCfgDateSet:	set system date
*
* DESCRIPTION:
* 	this function is used to set system date.
*
* INPUTS:
*	pcDate      - date string, format is yyyy-mm-dd
*   pCallBack   - on changed call back function
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO:
*/
UINT32 odmSysCfgDateSet(char *pcDate, OnChangedCallBack pCallBack)
{
    time_t          now, adjTime, newTime;
    int             nMaxRange;
    unsigned int    unYear, unMonth, unDay;
    unsigned int    unLeapYear;     /* leap year 1, normal 0 */
    char            arrchDayOfMon[2][12] =
                    {
                        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
                        31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
                    };
    struct tm       *t;
    struct timeval  tv;
    struct timezone tz;

    time(&now);
    adjTime = now + mNtpTimeZone + getTimeDstAdj();
    t = gmtime(&adjTime);

    if (ERROR == cliStrToDate(pcDate, &unYear, &unMonth, &unDay))
    {
        /* vosPrintf(pstEnv->nWriteFd, "Invalid date format input.\r\n"); */
        return ERR_INVALID_DATE;
    }

    /* check if the date is leap year */
    unLeapYear = (cliIsLeapYear(unYear) == TRUE) ? 1 : 0;
    /* get the days of the month */
    nMaxRange = arrchDayOfMon[unLeapYear][unMonth - 1];

    if (unDay > (unsigned int)nMaxRange)
    {
        /* vosPrintf(pstEnv->nWriteFd, "Invalid day input \r\n"); */
        return ERR_INVALID_DATE;
    }

    t->tm_year = unYear - 1900;
    t->tm_mon = unMonth - 1;
    t->tm_mday = unDay;

    newTime = mktime(t);
    tv.tv_sec = newTime;
    tv.tv_usec = 0;

    tz.tz_minuteswest = 0;
    tz.tz_dsttime = 0;

    /* vxWorks does not support this function, just for testing now */
    settimeofday(&tv, &tz);

    if (pCallBack) {
        pCallBack((UINT32)pcDate, 2, 3, 4);
    }

    return NO_ERROR;
}

UINT32 odmSysCfgTimeSet(char *pcTime, OnChangedCallBack pCallBack)
{
    time_t          now, adjTime, newTime;
    unsigned int    unHour, unMinute, unSecond;
    struct tm       *t;
    struct timeval  tv;
    struct timezone tz;

    time(&now);
    adjTime = now + mNtpTimeZone + getTimeDstAdj();
    t = gmtime(&adjTime);

    if (ERROR==cliStrToTime(pcTime, &unHour, &unMinute, &unSecond))
    {
        /* vosPrintf(pstEnv->nWriteFd, "Invalid time format input.\r\n"); */
        return ERR_INVALID_TIME;
    }

    t->tm_hour = unHour;
    t->tm_min = unMinute;
    t->tm_sec = unSecond;

    newTime = mktime(t);
    tv.tv_sec = newTime;
    tv.tv_usec = 0;

    tz.tz_minuteswest = 0;
    tz.tz_dsttime = 0;

    /* vxWorks does not support this function, just for testing now */
    settimeofday(&tv, &tz);

    if (pCallBack) {
        pCallBack((UINT32)pcTime, 2, 3, 4);
    }

    return NO_ERROR;
}

/*******************************************************************************
*
* odmSysCfgTimeZoneSet:
*
* DESCRIPTION:
*
*
* INPUTS:
*	pcTimeZone - the timezone format must be same as the following :
*       "-12:00"     - (GMT-12:00) International Date Line West
*       "-11:00"     - (GMT-11:00) Midway Island, Samoa
*       "-10:00"     - (GMT-10:00) Hawaii
*       "-09:00"     - (GMT-09:00) Alaska
*       "-08:00"     - (GMT-08:00) Pacific Time (US, Canada), Tijuana
*       "-07:00"     - (GMT-07:00) Arizona, La Paz, Mountain Time (US, Canada)
*       "-06:00"     - (GMT-06:00) Central Time (US, Canada), Mexico City, Tegucigalpa
*       "-05:00"     - (GMT-05:00) Bogota, Eastern Time (US, Canada), Indiana (East), Lima, Quito
*       "-04:00"     - (GMT-04:00) Atlantic Time (Canada), Caracas, La Paz, Santiago
*       "-03:30"     - (GMT-03:30) Newfoundland
*       "-03:00"     - (GMT-03:00) Brasilia, Buenos Aires, Georgetown, Greenland
*       "-02:00"     - (GMT-02:00) Mid-Atlantic
*       "-01:00"     - (GMT-01:00) Azores, Cape Verde Is.
*       "00:00"      - (GMT) Casablanca, Dublin, Edinburgh, Lisbon, London, Monrovia
*       "+01:00"     - (GMT+01:00) Amsterdam, Berlin, Rome, Bern, Stockholm, Vienna
*       "+01:00 (1)" - (GMT+01:00) Belgrade, Bratislava, Budapest, Ljubljana, Prague
*       "+01:00 (2)" - (GMT+01:00) Brussels, Copenhagen, Madrid, Paris, Vilnius
*       "+01:00 (3)" - (GMT+01:00) Sarajevo, Skopje, Sofia, Warsaw, Zagreb
*       "+01:00 (4)" - (GMT+01:00) West Central Africa
*       "+02:00"     - (GMT+02:00) Athens, Bucharest, Cairo, Istanbul, Minsk
*       "+02:00 (1)" - (GMT+02:00) Harare, Helsinki, Jerusalem, Riga, Kyev
*       "+02:00 (2)" - (GMT+02:00) Pretoria, Sofia, Tallinn, Vilnius
*       "+03:00"     - (GMT+03:00) Baghdad, Kuwait, Nairobi, Riyadh
*       "+03:00 (1)" - (GMT+03:00) Moscow, St. Petersburg, Volgograd
*       "+03:30"     - (GMT+03:30) Tehran
*       "+04:00"     - (GMT+04:00) Abu Dhabi, Baku, Muscat, Tbilisi, Yerevan
*       "+04:30"     - (GMT+04:30) Kabul
*       "+05:00"     - (GMT+05:00) Ekaterinburg, Islamabad, Karachi, Tashkent
*       "+05:30"     - (GMT+05:30) Chennai, Kolkata, Mumbai, New Delhi
*       "+05:45"     - (GMT+05:45) Kathmandu
*       "+06:00"     - (GMT+06:00) Almaty, Astana, Dhaka, Novosibirsk, Sri Jayawardenepura
*       "+06:30"     - (GMT+06:30) Rangoon
*       "+07:00"     - (GMT+07:00) Bangkok, Hanoi, Jakarta, Krasnoyarsk
*       "+08:00"     - (GMT+08:00) Beijing, Hong Kong, Taipei, Singapore
*       "+09:00"     - (GMT+09:00) Seoul, Tokyo, Yakutsk
*       "+09:30"     - (GMT+09:30) Adelaide, Darwin
*       "+10:00"     - (GMT+10:00) Canberra, Guam, Port Moresby, Vladivostok
*       "+11:00"     - (GMT+11:00) Magadan, New Caledonia, Solomon Is.
*       "+12:00"     - (GMT+12:00) Auckland, Fiji, Kamchatka, Marshall Is., Wellington
*       "+13:00"     - (GMT+13:00) Nuku'alofa, Tonga
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO:
*/
UINT32 odmSysCfgTimeZoneSet(const char *pcTimeZone, OnChangedCallBack pCallBack)
{
    int rv;

    if (FALSE == utilStrIsTimeZone(pcTimeZone))
    {
        return INVALID_32;
    }

    rv = vosConfigValueSet(
        SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,
        SYSTEM_CFG_TIME_ZONE,
        pcTimeZone);
    if (rv == -1) {
        return INVALID_32;
    }

    mNtpTimeZone = utilStrToTimeZone(pcTimeZone);

    if (pCallBack) {
        pCallBack((UINT32)pcTimeZone, 2, 3, 4);
    }

    return NO_ERROR;
}

#ifdef OPL_DMA0_ETH
extern UINT8 wanMacClsRule[6];
#endif
UINT32 odmSysIfMacAddrSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucMac)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
	UINT8   oldMacId[6] = {0};
    UINT8   macIdString[20] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    char    *pVal = NULL;
    char    acString[128] = {0};
	UINT8  fullmacMask[MAC_LENGTH] = {
	  0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF
	};

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

    if(NULL == pucMac)
    {
        return ERR_NULL_POINTER;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_MAC_ADDR);
    pVal = vosConfigValueGet(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_MAC_ADDR,"00:00:00:00:00:00");
    utilStrToMacAddr(pVal,oldMacId);
    vosSprintf(macIdString,"%02x:%02x:%02x:%02x:%02x:%02x",
        pucMac[0],
        pucMac[1],
        pucMac[2],
        pucMac[3],
        pucMac[4],
        pucMac[5]);
	if((NO_ERROR == retVal && (memcmp(pucMac,&oldMacId[0],MAC_LENGTH)))||(NO_ERROR != retVal))
	{
        //vosSprintf(acString, "ifconfig %s down", g_acIfEthNameStr[index]);
       // vosSystem(acString);
        //retVal = vosHWAddrSet((UINT8 *)g_acIfEthNameStr[index], pucMac);
        vosSprintf(acString, "ifconfig %s hw ether %s ", g_acIfEthNameStr[index],macIdString);
		vosSystem(acString);
       // vosSprintf(acString, "ifconfig %s up", g_acIfEthNameStr[index]);
       // vosSystem(acString);
		//printf("set index %d mode %d mac %s ret %d\n",ucIf,ucIfMode,
		//		macIdString,retVal);
		vosConfigValueSet(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_MAC_ADDR,macIdString);
		#ifdef OPL_DMA0_ETH
        if (index < ((INTERFACE_WAN+1)*INTERFACE_MODE_MAX))
		{
		  int i;
		  int delflg = 1;
		  UINT16 rule=0;
		  int ret;
		  CLS_CONFIG_INFO_t ClassItemInfoSin;

		  #ifdef OPL_NO_DMA1_ETH
		  for (i=0; i< INTERFACE_MAX*INTERFACE_MODE_MAX; i++)	      
		  #else
		  for (i=INTERFACE_MODE_MAX; i< INTERFACE_MAX*INTERFACE_MODE_MAX; i++)
		  #endif
		  {
		    if (i != index)
		    {
		      if (wanMacClsRule[i] == wanMacClsRule[index])
		      {
		        delflg = 0;
		      }
		    }
		  }
		  
		  if (1 == delflg)
		  {
		    dalVoipClsDelEntry(wanMacClsRule[index]);
		  }
		  
		  vosMemSet((char *)&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
		  ClassItemInfoSin.dstMacFlag = 1;
	      ClassItemInfoSin.dstMac.lowRange[0] = pucMac[0];
	      ClassItemInfoSin.dstMac.lowRange[1] = pucMac[1];
	      ClassItemInfoSin.dstMac.lowRange[2] = pucMac[2];
	      ClassItemInfoSin.dstMac.lowRange[3] = pucMac[3];
	      ClassItemInfoSin.dstMac.lowRange[4] = pucMac[4];
	      ClassItemInfoSin.dstMac.lowRange[5] = pucMac[5]; 
		  vosMemCpy(&(ClassItemInfoSin.dstMac.highRange),fullmacMask,MAC_LENGTH);
		  
	      #ifndef OPL_NO_DMA1_ETH
	  	  ClassItemInfoSin.bingress=0x01;
		  ClassItemInfoSin.ingressmask=0x01;
		  #endif
	      ClassItemInfoSin.t_act = (DROP_PKTS|COPY_PKTS_TO_CPU);
	      dalVoipClsRuleCtcAdd(&ClassItemInfoSin,&rule);
	      
	    wanMacClsRule[index] = rule;
    }
    
    #endif
        if (/*index > ((INTERFACE_WAN+1)*INTERFACE_MODE_MAX) &&*/ /*this is always if(0), and fix it.*/
            index < ((INTERFACE_LAN+1)*INTERFACE_MODE_MAX))
        {
            //dalArlMgmtMacRdt2Cpu(oldMacId, pucMac);
        }
	}
	return NO_ERROR;
}

UINT32 odmSysCfgIfShareModeSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 ucIfShareMode)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT8   ucOldIfShareMode = 0;
    char    acString[128] = {0};
    UINT8   ucIfIpMode = IF_IP_MODE_STATIC;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }
	
	/*only permit admin interface to set shared mode */
	if(ucIfShareMode==INTERFACE_SHARE_MODE_SHARE && ucIfMode!=INTERFACE_MODE_ADMIN )
	{
        return ERR_OUT_OF_RANGE;
	}
	
	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_SHARE_MODE);
    ucOldIfShareMode = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_SHARE_MODE,INTERFACE_SHARE_MODE_SHARE);

    if (ucOldIfShareMode == ucIfShareMode)
    {
        return NO_ERROR;
    }
	
	/*when shared only permit admin interface to change independent mode */
	if(ucOldIfShareMode==INTERFACE_SHARE_MODE_SHARE &&  ucIfMode!=INTERFACE_MODE_ADMIN )
	{
	    OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: change mode failed for index %d , errno = %u",
            __FUNCTION__, index,retVal);
        return ERR_OUT_OF_RANGE;
	}

	if((OK == retVal && (ucOldIfShareMode != ucIfShareMode))||(OK != retVal))
	{
		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_SHARE_MODE,ucIfShareMode);
		if(ucIfMode == INTERFACE_MODE_ADMIN)
		{			
			/*when shared mode,set media and signal also shared */	
		#if defined(INTERFACE_TWO_WAN_TWO_LAN)
			vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index+1);		
			vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_SHARE_MODE,ucIfShareMode);	
		#elif defined(INTERFACE_THREE_WAN_THREE_LAN)
			vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index+1);		
			vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_SHARE_MODE,ucIfShareMode);	
			
			vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index+2);		
			vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_SHARE_MODE,ucIfShareMode);	
		#endif		
		}
	}

    return NO_ERROR;
}

/* ip mode changed, notify voip */
UINT32 odmSysCfgIfIpModeSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 ucIpMode, IF_MODE_VOIP_PARA_S * pifModePara)
{
	STATUS  retVal = OK;
    UINT8   ucOldIpMode = 0;
    char    acString[128] = {0};
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT32 oldIp = 0;
    IF_MODE_VOIP_PARA_S IpModePara;

    memset(&IpModePara,0,sizeof(IF_MODE_VOIP_PARA_S));

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }
    odmSysCfgIfIpAddrGet(ucIf, ucIfMode, &oldIp);

    if (pifModePara != NULL ){
	    IpModePara.shareModeChanged = pifModePara->shareModeChanged;
	    IpModePara.newShareMode = pifModePara->newShareMode;
	    IpModePara.oldShareMode = pifModePara->oldShareMode;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_IP_MODE);
    ucOldIpMode = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_IP_MODE,IF_IP_MODE_STATIC);

     //IpModePara.oldIp = oldIp;
     IpModePara.modeChangeType = IP_MODE_STAY_SAME;
	 IpModePara.index = index;
	 IpModePara.ucIf = ucIf;
	 IpModePara.ucMode = ucIfMode;

	if((OK == retVal && (ucOldIpMode != ucIpMode))||(OK != retVal))
	{
        /* set to database must be first than the following */
		retVal = vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_IP_MODE,ucIpMode);
		if(OK != retVal)
		{
			return INVALID_32;
		}
	}

    vosMutexTake(&g_pastIpModeMutex);

    switch (ucIpMode)
    {
        case IF_IP_MODE_STATIC:
            if (IF_IP_MODE_DHCP == ucOldIpMode)
            {
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: IP_MODE_DHCP_TO_STATIC", __FUNCTION__);
                /* stop dhcp client , ip mode change:dhcp -> static*/
                //ipModeChangeNotifyVoip(0,3,ucIf, ucIfMode);
                IpModePara.modeChangeType = IP_MODE_DHCP_TO_STATIC;
                odmSysDHCPSet(ucIf, ucIfMode, &IpModePara);
            }
            else if (IF_IP_MODE_PPPOE == ucOldIpMode)
            {
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: IP_MODE_PPPOE_TO_STATIC", __FUNCTION__);
                /* stop pppoe client, ip mode change:pppoe -> static*/
                //ipModeChangeNotifyVoip(0,5,ucIf, ucIfMode);
                IpModePara.modeChangeType = IP_MODE_PPPOE_TO_STATIC;
                odmSysPPPoESet(ucIf, ucIfMode, &IpModePara);
            }else{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: IP_MODE_STAY_SAME(static)", __FUNCTION__);
            }
			odmSysClearIpInfo(ucIf, ucIfMode);
            break;

        case IF_IP_MODE_DHCP:
            if (IF_IP_MODE_PPPOE == ucOldIpMode)
            {
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: IP_MODE_PPPOE_TO_DHCP", __FUNCTION__);
                /* stop pppoe client, ip mode change: pppoe -> dhcp*/
                IpModePara.modeChangeType = IP_MODE_PPPOE_TO_DHCP;
                ipModeChangeNotifyVoip(NET_DOWN,IP_MODE_PPPOE_TO_DHCP,ucIf, ucIfMode,&IpModePara);
                odmSysPPPoESet(ucIf, ucIfMode, &IpModePara);

				/* clear original ip address set */
 				odmSysClearIpInfo(ucIf, ucIfMode);

            }else if(IF_IP_MODE_STATIC == ucOldIpMode){
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: IP_MODE_STATIC_TO_DHCP", __FUNCTION__);
                IpModePara.modeChangeType = IP_MODE_STATIC_TO_DHCP;
                ipModeChangeNotifyVoip(NET_DOWN,IP_MODE_STATIC_TO_DHCP,ucIf, ucIfMode,&IpModePara);

				/* clear original ip address set */
 				odmSysClearIpInfo(ucIf, ucIfMode);

           }else{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: IP_MODE_STAY_SAME(dhcp)", __FUNCTION__);
                ipModeChangeNotifyVoip(NET_DOWN,IP_MODE_STAY_SAME,ucIf, ucIfMode,&IpModePara);
            }
            odmSysDHCPSet(ucIf, ucIfMode, &IpModePara);
            break;

        case IF_IP_MODE_PPPOE:
            if (IF_IP_MODE_DHCP == ucOldIpMode)
            {
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: IP_MODE_DHCP_TO_PPPOE", __FUNCTION__);
                /* stop dhcp client , ip mode change:dhcp -> pppoe*/
                IpModePara.modeChangeType = IP_MODE_DHCP_TO_PPPOE;
                ipModeChangeNotifyVoip(NET_DOWN,IP_MODE_DHCP_TO_PPPOE,ucIf, ucIfMode,&IpModePara);
                odmSysDHCPSet(ucIf, ucIfMode, &IpModePara);

				/* clear original ip address set */
				odmSysClearIpInfo(ucIf, ucIfMode);
            }else if (IF_IP_MODE_STATIC == ucOldIpMode){
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: IP_MODE_STATIC_TO_PPPOE", __FUNCTION__);
                /* ip mode change:static -> pppoe*/
                IpModePara.modeChangeType = IP_MODE_STATIC_TO_PPPOE;
                ipModeChangeNotifyVoip(NET_DOWN,IP_MODE_STATIC_TO_PPPOE,ucIf, ucIfMode,&IpModePara);

				/* clear original ip address set */
				odmSysClearIpInfo(ucIf, ucIfMode);
            }else {
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: IP_MODE_STAY_SAME(pppoe)", __FUNCTION__);
                ipModeChangeNotifyVoip(NET_DOWN,IP_MODE_STAY_SAME,ucIf, ucIfMode,&IpModePara);
            }
            odmSysPPPoESet(ucIf, ucIfMode, &IpModePara);
            break;

        default:
            break;
    }

    vosMutexGive(&g_pastIpModeMutex);
    return NO_ERROR;
}
/* ip mode changed, notify voip */

UINT32 odmSysCfgIfIpAddrSet(UINT8 ucIf, UINT8 ucIfMode, UINT32 ulIpAddress)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT32  ucOldIpAddress = 0;
    char    acString[128] = {0};

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_IP_ADDR);
    ucOldIpAddress = vosConfigUInt32Get(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_IP_ADDR,0);

	if((OK == retVal && (ucOldIpAddress != ulIpAddress))||(OK != retVal))
	{
        cliIpToStr (ulIpAddress, acString);

        if (!cliIsValidIpAddress (acString) && 0 != ulIpAddress)
        {
            return ERR_INVALID_IP_ADDR;
        }

        /* Do not update LAN admin immediately */
        if (index != IF_MODE_TO_INDEX(INTERFACE_LAN,INTERFACE_MODE_ADMIN))
        {
            /* Update ip address here */
            if (OK != vosIPAddrSet((UINT8 *)g_acIfEthNameStr[index], acString))
            {
                return ERR_INVALID_IP_ADDR;
            }
        }
		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_IP_ADDR,ulIpAddress);
	}

    return NO_ERROR;
}

UINT32 odmSysCfgIfIpMaskSet(UINT8 ucIf, UINT8 ucIfMode, UINT32 ulIpMask)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT32  ucOldIpMask = 0;
    char    acString[128] = {0};

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_IP_MASK);
    ucOldIpMask = vosConfigUInt32Get(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_IP_MASK,0);
	if((OK == retVal && (ucOldIpMask != ulIpMask))||(OK != retVal))
	{
        cliIpToStr (ulIpMask, acString);

        if (!cliIsValidNetMask(acString) && 0 != ulIpMask)
        {
            return ERR_INVALID_IP_MASK;
        }

        /* Do not update LAN admin immediately */
        if (index != IF_MODE_TO_INDEX(INTERFACE_LAN,INTERFACE_MODE_ADMIN))
        {		
	        /* Update ip address here */
	        if (OK != vosNetMaskSet((UINT8 *)g_acIfEthNameStr[index], acString))
	        {
	            return ERR_INVALID_IP_ADDR;
	        }
        }
		
		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_IP_MASK,ulIpMask);
	}

    return NO_ERROR;
}

UINT32 odmSysCfgIfIpGatewaySet(UINT8 ucIf, UINT8 ucIfMode, UINT32 ipAddress)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT32  ucOldIpAddr = 0;
    char    acString[128] = {0};
    char    oldStrIp[20] = {0};
    char    strIp[20] = {0};

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_IP_GW);
    ucOldIpAddr = vosConfigUInt32Get(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_IP_GW,0);
	if((OK == retVal && (ucOldIpAddr != ipAddress))||(OK != retVal))
	{
        if (0 == ipAddress && 0 != ucOldIpAddr)
        {
            cliIpToStr (ucOldIpAddr, strIp);
            vosSprintf(acString, "route del default gw %s dev %s", strIp, g_acIfEthNameStr[index]);
    	    vosSystem(acString);
        }
        else if (0 != ipAddress)
		{
            cliIpToStr (ucOldIpAddr, oldStrIp);
            cliIpToStr (ipAddress, strIp);

            if (!cliIsValidIpAddress(strIp))
            {
                return ERR_INVALID_IP_ADDR;
            }
            vosSprintf(acString, "route del default gw %s dev %s", oldStrIp, g_acIfEthNameStr[index]);
    	    vosSystem(acString);
            /* Update ip address here */
            vosSprintf(acString, "route add default gw %s dev %s", strIp, g_acIfEthNameStr[index]);
    	    vosSystem(acString);
        }

		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_IP_GW,ipAddress);
	}

    return NO_ERROR;
}

UINT32 odmSysCfgIfIpDnsSet(UINT8 ucIf, UINT8 ucIfMode, UINT32 ipAddress)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT32  ucOldIpAddr = 0;
    char    acString[128] = {0};
    char    strIp[20] = {0};

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_IP_DNS);
    ucOldIpAddr = vosConfigUInt32Get(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_IP_DNS,0);
	if((OK == retVal && (ucOldIpAddr != ipAddress))||(OK != retVal))
	{
        cliIpToStr (ucOldIpAddr, strIp);
        vosSprintf(acString, "sed -e '/nameserver %s/d' /etc/resolv.conf > /etc/resolv.conf.new", strIp);
		vosSystem(acString);
		vosSystem("mv /etc/resolv.conf.new /etc/resolv.conf");
        if (0 != ipAddress)
        {
            cliIpToStr (ipAddress, strIp);

            if (!cliIsValidIpAddress(strIp) )
            {
                return ERR_INVALID_IP_ADDR;
            }
            vosSprintf(acString, "\necho nameserver %s >> /etc/resolv.conf", strIp);
    	    vosSystem(acString);
            vosSystem("cat /etc/resolv.conf");
        }
		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_IP_DNS,ipAddress);
	}

    return NO_ERROR;
}

UINT32 odmSysClearIpInfo(UINT8 ucIf, UINT8 ucIfMode)
{
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	/* clean up netmask */
	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_IP_MASK,0);

	odmSysCfgIfIpAddrSet(ucIf, ucIfMode, 0);
	odmSysCfgIfIpGatewaySet(ucIf, ucIfMode, 0);
	odmSysCfgIfIpDnsSet(ucIf, ucIfMode, 0);
}

UINT32 odmSysCfgIfPPPoEModeSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 ucIfPPPoEMode)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT8   ucOldIfPPPoEMode = 0;
    char    acString[128] = {0};

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_PPPOE_AUTH);
    ucOldIfPPPoEMode = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_PPPOE_AUTH,PPPOE_AUTH_TYPE_AUTO);
	if((OK == retVal && (ucOldIfPPPoEMode != ucIfPPPoEMode))||(OK != retVal))
	{
		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_PPPOE_AUTH,ucIfPPPoEMode);
	}

    return NO_ERROR;
}

UINT32 odmSysCfgIfPPPoEUserSet(UINT8 ucIf, UINT8 ucIfMode, char *pcUserName)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    char    *pVal = NULL;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

    if(NULL == pcUserName)
    {
        return ERR_NULL_POINTER;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_PPPOE_USER);
    pVal = vosConfigValueGet(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_PPPOE_USER,NULL);
	if((NO_ERROR == retVal && (vosStrCmp(pcUserName, pVal)))||(NO_ERROR != retVal))
	{
		vosConfigValueSet(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_PPPOE_USER,pcUserName);
	}
	return NO_ERROR;
}

UINT32 odmSysCfgIfPPPoEPassSet(UINT8 ucIf, UINT8 ucIfMode, char *pcPassword)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    char    *pVal = NULL;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

    if(NULL == pcPassword)
    {
        return ERR_NULL_POINTER;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_PPPOE_PASS);
    pVal = vosConfigValueGet(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_PPPOE_PASS,NULL);
	if((NO_ERROR == retVal && (vosStrCmp(pcPassword, pVal)))||(NO_ERROR != retVal))
	{
		vosConfigValueSet(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_PPPOE_PASS,pcPassword);
	}
	return NO_ERROR;
}

UINT32 odmSysCfgIfQoSModeSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 ucIfQoSMode)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT8   ucOldIfQoSMode = 0;
    char    acString[128] = {0};

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_QOS_MODE);
    ucOldIfQoSMode = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_QOS_MODE,IF_QOS_MODE_DSCP);
	if((OK == retVal && (ucOldIfQoSMode != ucIfQoSMode))||(OK != retVal))
	{
		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_QOS_MODE,ucIfQoSMode);
	}

    return NO_ERROR;
}

UINT32 odmSysCfgIfQoSValSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 ucQoSVal)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT8   ucOldQoSVal = 0;
    char    acString[128] = {0};

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_QOS_VAL);
    ucOldQoSVal = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_QOS_VAL,0);
	if((OK == retVal && (ucOldQoSVal != ucQoSVal))||(OK != retVal))
	{
		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_QOS_VAL,ucQoSVal);
	}

    return NO_ERROR;
}

UINT32 odmSysCfgIfTagModeSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 ucIfTagMode)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT8   ucOldIfTagMode = 0;
    char    acString[128] = {0};

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_TAG_MODE);
    ucOldIfTagMode = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_TAG_MODE,IF_TAG_MODE_TRANSPARENT);
	if((OK == retVal && (ucOldIfTagMode != ucIfTagMode))||(OK != retVal))
	{
		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_TAG_MODE,ucIfTagMode);
	}

    return NO_ERROR;
}


UINT32 odmSysCfgIfCVLANSet(UINT8 ucIf, UINT8 ucIfMode, UINT16 usCVLAN)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT16  usOldCCVLAN = 0;
    char    acString[128] = {0};

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_TAG_CVLAN);
    usOldCCVLAN = (UINT16)vosConfigUInt32Get(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_TAG_CVLAN,0);
	if((OK == retVal && (usOldCCVLAN != usCVLAN))||(OK != retVal))
	{
		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_TAG_CVLAN,usCVLAN);
	}

    return NO_ERROR;
}


UINT32 odmSysCfgIfPriSet(UINT8 ucIf, UINT8 ucIfMode, UINT8 ucPriority)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT8   ucOldPriority = 0;
    char    acString[128] = {0};

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_TAG_PRI);
    ucOldPriority = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_TAG_PRI,0);
	if((OK == retVal && (ucOldPriority != ucPriority))||(OK != retVal))
	{
		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_TAG_PRI,ucPriority);
	}

    return NO_ERROR;
}


UINT32 odmSysCfgIfSVLANSet(UINT8 ucIf, UINT8 ucIfMode, UINT16 usSVLAN)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT16  usOldSVLAN = 0;
    char    acString[128] = {0};

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
	retVal = vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_TAG_SVLAN);
    usOldSVLAN = (UINT16)vosConfigUInt32Get(SYSTEM_CFG,
        IfSectionBuff,SYSTEM_CFG_IF_TAG_SVLAN,0);
	if((OK == retVal && (usOldSVLAN != usSVLAN))||(OK != retVal))
	{
		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_TAG_SVLAN,usSVLAN);
	}

    return NO_ERROR;
}

/* ip mode changed, notify voip */
UINT32 odmSysIfConfigSet(ODM_SYS_IF_CONFIG_t *pstIpConfig)
{
    UINT32  retVal;
    char    acString[128] = {0};
    UINT32 oldIp = 0;
    IF_MODE_VOIP_PARA_S IpModePara;
    UINT8   index;
    VoiceVlan_t SysVoiceVlan;

    if (NULL == pstIpConfig)
    {
        return ERR_NULL_POINTER;
    }

    index = IF_MODE_TO_INDEX(pstIpConfig->ucIf, pstIpConfig->ucIfMode);
    odmSysCfgIfIpAddrGet(pstIpConfig->ucIf, pstIpConfig->ucIfMode, &oldIp);
    IpModePara.index = index;
    IpModePara.ucIf = pstIpConfig->ucIf;
    IpModePara.ucMode = pstIpConfig->ucIfMode;
    IpModePara.oldIp = oldIp;
    IpModePara.shareModeChanged = 0;
    odmSysCfgIfShareModeGet(pstIpConfig->ucIf, pstIpConfig->ucIfMode,&(IpModePara.oldShareMode));
	odmSysCfgIfTagModeGet(pstIpConfig->ucIf, pstIpConfig->ucIfMode,&(IpModePara.oldTagMode));
	if ( IpModePara.oldShareMode != pstIpConfig->ucIfShareMode ){
        IpModePara.newShareMode = pstIpConfig->ucIfShareMode;
        IpModePara.shareModeChanged = 1;
        printf("Node: ip shared mode changed old %d, new %d\n",IpModePara.oldShareMode,IpModePara.newShareMode);
    }

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

    retVal = odmSysCfgIfShareModeSet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, pstIpConfig->ucIfShareMode);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: set share mode failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    switch (pstIpConfig->ucIpMode)
    {
        case IF_IP_MODE_STATIC:
        case IF_IP_MODE_DHCP:
            break;
        case IF_IP_MODE_PPPOE:
            retVal = odmSysCfgIfPPPoEModeSet(pstIpConfig->ucIf,
                pstIpConfig->ucIfMode, pstIpConfig->ucPPPoEAuthType);
            if (NO_ERROR != retVal)
            {
                OP_DEBUG(DEBUG_LEVEL_WARNING,
                    "%s: set pppoe mode failed, errno = %u",
                    __FUNCTION__, retVal);
                return retVal;
            }

            retVal = odmSysCfgIfPPPoEUserSet(pstIpConfig->ucIf,
                pstIpConfig->ucIfMode, pstIpConfig->acPPPoEUser);
            if (NO_ERROR != retVal)
            {
                OP_DEBUG(DEBUG_LEVEL_WARNING,
                    "%s: set pppoe username failed, errno = %u",
                    __FUNCTION__, retVal);
                return retVal;
            }

            retVal = odmSysCfgIfPPPoEPassSet(pstIpConfig->ucIf,
                pstIpConfig->ucIfMode, pstIpConfig->acPPPoEPass);
            if (NO_ERROR != retVal)
            {
                OP_DEBUG(DEBUG_LEVEL_WARNING,
                    "%s: set pppoe password failed, errno = %u",
                    __FUNCTION__, retVal);
                return retVal;
            }
            break;
        default:
            break;
    }

    /* ip mode changed, notify voip */

    retVal = odmSysCfgIfQoSModeSet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, pstIpConfig->ucQoSMode);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: set qos mode failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfQoSValSet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, pstIpConfig->ucQoSVal);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: set qos value failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfTagModeSet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, pstIpConfig->ucTagged);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: set tag mode failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfCVLANSet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, pstIpConfig->usCVlan);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: set cvlan failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfPriSet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, pstIpConfig->ucPriority);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: set priority failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfSVLANSet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, pstIpConfig->usSVLAN);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: set svlan failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

	if(pstIpConfig->ucTagged==IF_TAG_MODE_TAG)
	{
		vosMemSet(&SysVoiceVlan,0,sizeof(VoiceVlan_t));
		SysVoiceVlan.VoiceVlanCos=pstIpConfig->ucPriority;
		SysVoiceVlan.VoipTagValid=1;
		SysVoiceVlan.VoipPortNum=index;
		SysVoiceVlan.VoiceVlanId=pstIpConfig->usCVlan;
        SysVoiceVlan.VoipQinQEn=0;
		OP_DEBUG(DEBUG_LEVEL_INFO, "for interface %d cos %d vlan %d\n",index,pstIpConfig->ucPriority,pstIpConfig->usCVlan);
		syscall(NR_CFG_VOICEVLAN, (char*)&SysVoiceVlan, sizeof(SysVoiceVlan));
	}
	else if(pstIpConfig->ucTagged==IF_TAG_MODE_VLAN_STACKING)
	{
		vosMemSet(&SysVoiceVlan,0,sizeof(VoiceVlan_t));
		SysVoiceVlan.VoiceVlanCos=pstIpConfig->ucPriority;
		SysVoiceVlan.VoipTagValid=1;
		SysVoiceVlan.VoipPortNum=index;
		SysVoiceVlan.VoiceVlanId=pstIpConfig->usCVlan;
        SysVoiceVlan.VoipQinQEn=1;
		SysVoiceVlan.VoiceSVlanCos=pstIpConfig->ucPriority;
		SysVoiceVlan.VoiceSVlanId=pstIpConfig->usSVLAN;
		OP_DEBUG(DEBUG_LEVEL_INFO, "for interface %d cos %d vlan %d\n",index,pstIpConfig->ucPriority,pstIpConfig->usCVlan);
		syscall(NR_CFG_VOICEVLAN, (char*)&SysVoiceVlan, sizeof(SysVoiceVlan));
	}
	else if(IpModePara.oldTagMode==IF_TAG_MODE_TAG||
        IpModePara.oldTagMode==IF_TAG_MODE_VLAN_STACKING)
	{
		vosMemSet(&SysVoiceVlan,0,sizeof(VoiceVlan_t));
		SysVoiceVlan.VoipPortNum=index;
		OP_DEBUG(DEBUG_LEVEL_INFO, "for interface %d cos %d vlan %d\n",index,pstIpConfig->ucPriority,pstIpConfig->usCVlan);
		syscall(NR_CFG_VOICEVLAN, (char*)&SysVoiceVlan, sizeof(SysVoiceVlan));
	}

    retVal = odmSysCfgIfIpModeSet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, pstIpConfig->ucIpMode, &IpModePara);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: set ip mode failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    if (pstIpConfig->ucIpMode == IF_IP_MODE_STATIC)
    {
        retVal = odmSysCfgIfIpAddrSet(pstIpConfig->ucIf,
            pstIpConfig->ucIfMode, pstIpConfig->ulIpAddr);
        if (NO_ERROR != retVal)
        {
            OP_DEBUG(DEBUG_LEVEL_WARNING,
                "%s: set ip address failed, errno = %u",
                __FUNCTION__, retVal);
            return retVal;
        }

        retVal = odmSysCfgIfIpMaskSet(pstIpConfig->ucIf,
            pstIpConfig->ucIfMode, pstIpConfig->ulIpMask);
        if (NO_ERROR != retVal)
        {
            OP_DEBUG(DEBUG_LEVEL_WARNING,
                "%s: set ip mask failed, errno = %u",
                __FUNCTION__, retVal);
            return retVal;
        }

        retVal = odmSysCfgIfIpGatewaySet(pstIpConfig->ucIf,
            pstIpConfig->ucIfMode, pstIpConfig->ulGwAddr);
        if (NO_ERROR != retVal)
        {
            OP_DEBUG(DEBUG_LEVEL_WARNING,
                "%s: set gateway failed, errno = %u",
                __FUNCTION__, retVal);
            return retVal;
        }

        retVal = odmSysCfgIfIpDnsSet(pstIpConfig->ucIf,
            pstIpConfig->ucIfMode, pstIpConfig->ulDnsAddr);
        if (NO_ERROR != retVal)
        {
            OP_DEBUG(DEBUG_LEVEL_WARNING,
                "%s: set dns failed, errno = %u",
                __FUNCTION__, retVal);
            return retVal;
        }
        odmSysStaticIpSet(pstIpConfig->ucIf, pstIpConfig->ucIfMode, &IpModePara);
    }
    return NO_ERROR;
}



UINT32 odmSysCfgDayLightSavingSet(UINT8 ucAction, OnChangedCallBack pCallBack)
{
    int rv;

    rv = vosConfigValueSet(
        SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,
        SYSTEM_CFG_DAY_LIGHT_SAVING,
        (ucAction == ENABLED) ? STATE_ENABLE_STR : STATE_DISABLE_STR);
    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)ucAction, 2, 3, 4);
    }

    return NO_ERROR;
}


UINT32 odmSysCfgTelnetServiceSet(UINT8 ucAction, OnChangedCallBack pCallBack)
{
    int rv;
    UINT32 ulStatus = NO_ERROR;

    if (odmSysCfgTelnetServiceGet() == ucAction)
    {
        return NO_ERROR;
    }

    if (ENABLED == ucAction)
    {
        ulStatus = cliTelnetdInit();
    }
    else
    {
        ulStatus = cliTelnetdShutdown();
    }

    if (NO_ERROR != ulStatus)
    {
        return ulStatus;
    }

    rv = vosConfigValueSet(
        SYSTEM_CFG,
        SYSTEM_CFG_TELNET_SEC,
        SYSTEM_CFG_TELNET_SERVICE,
        (ucAction == ENABLED) ? STATE_ENABLE_STR : STATE_DISABLE_STR);
    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)ucAction, 2, 3, 4);
    }

    return NO_ERROR;
}

UINT32 odmSysCfgTelnetPortSet(UINT16 usPort, OnChangedCallBack pCallBack)
{
    int rv;
    UINT32 ulStatus = NO_ERROR;

    if (odmSysCfgTelnetPortGet() == usPort)
    {
        return NO_ERROR;
    }

    /* restart telnetd */
    if (ENABLED == odmSysCfgTelnetServiceGet())
    {
        ulStatus = cliTelnetdShutdown();
        if (NO_ERROR != ulStatus)
        {
            return ulStatus;
        }

        ulStatus = cliTelnetdInit();
        if (NO_ERROR != ulStatus)
        {
            return ulStatus;
        }
    }

    rv = vosConfigUInt32Set(
        SYSTEM_CFG,
        SYSTEM_CFG_TELNET_SEC,
        SYSTEM_CFG_TELNET_PORT,
        usPort);
    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)usPort, 2, 3, 4);
    }

    return NO_ERROR;
}

UINT32 odmSysCfgTelnetSessionNumSet(UINT16 usSessionNum, OnChangedCallBack pCallBack)
{
    int rv;

    if (odmSysCfgTelnetSessionNumGet() == usSessionNum)
    {
        return NO_ERROR;
    }

    if (MAX_SESSION_NUM < usSessionNum)
    {
        return INVALID_32;
    }

    rv = vosConfigUInt32Set(
        SYSTEM_CFG,
        SYSTEM_CFG_TELNET_SEC,
        SYSTEM_CFG_TELNET_SESSIONS,
        usSessionNum);
    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)usSessionNum, 2, 3, 4);
    }

    return NO_ERROR;
}

UINT32 odmSysCfgSessionTimeoutSet(UINT32 ulTimeout, OnChangedCallBack pCallBack)
{
    int rv;

    rv = vosConfigUInt32Set(
        SYSTEM_CFG,
        SYSTEM_CFG_TELNET_SEC,
        SYSTEM_CFG_SESSION_TIMEOUT,
        ulTimeout);
    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)ulTimeout, 2, 3, 4);
    }

    return NO_ERROR;
}


UINT32 odmSysCfgWebServiceSet(UINT8 ucAction, OnChangedCallBack pCallBack)
{
    int rv;
    UINT32 ulStatus = NO_ERROR;

    if (odmSysCfgWebServiceGet() == ucAction)
    {
        return NO_ERROR;
    }

    if (ENABLED == ucAction)
    {
//        ulStatus = start_web_server_thread();
    }
    else
    {
//        ulStatus = stop_web_server_thread();
    }

    if (NO_ERROR != ulStatus)
    {
        return ulStatus;
    }

    rv = vosConfigValueSet(
        SYSTEM_CFG,
        SYSTEM_CFG_WEB_SEC,
        SYSTEM_CFG_WEB_SERVICE,
        (ucAction == ENABLED) ? STATE_ENABLE_STR : STATE_DISABLE_STR);
    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)ucAction, 2, 3, 4);
    }

    return NO_ERROR;
}

UINT32 odmSysCfgWebPortSet(UINT16 usPort, OnChangedCallBack pCallBack)
{
    int rv;
    UINT32 ulStatus = NO_ERROR;

    if (odmSysCfgWebPortGet() == usPort)
    {
        return NO_ERROR;
    }

    if (ENABLED == odmSysCfgWebServiceGet())
    {
//        ulStatus = restart_web_server_thread();
    }

    if (NO_ERROR != ulStatus)
    {
        return ulStatus;
    }

    rv = vosConfigUInt32Set(
        SYSTEM_CFG,
        SYSTEM_CFG_WEB_SEC,
        SYSTEM_CFG_WEB_PORT,
        usPort);
    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)usPort, 2, 3, 4);
    }

    return NO_ERROR;
}

UINT32 odmSysCfgWebSessionNumSet(UINT16 usSessionNum, OnChangedCallBack pCallBack)
{
    int rv;

    if (odmSysCfgWebSessionNumGet() == usSessionNum)
    {
        return NO_ERROR;
    }

    if (MAX_SESSION_NUM < usSessionNum)
    {
        return INVALID_32;
    }

    rv = vosConfigUInt32Set(
        SYSTEM_CFG,
        SYSTEM_CFG_WEB_SEC,
        SYSTEM_CFG_WEB_SESSIONS,
        usSessionNum);
    if (rv == -1) {
        return INVALID_32;
    }

    if (pCallBack) {
        pCallBack((UINT32)usSessionNum, 2, 3, 4);
    }

    return NO_ERROR;
}

const char * odmSysCfgNameGet()
{
    return vosConfigValueGet(
        SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,
        SYSTEM_CFG_SYS_NAME,
        DEFAULT_HOSTNAME);
}

const char * odmSysCfgLocationGet()
{
    return vosConfigValueGet(
        SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,
        SYSTEM_CFG_SYS_LOCATION,
        "Planet Earth");
}

UINT8 odmSysCfgNTPServiceGet()
{
    char *pcState;
    pcState = vosConfigValueGet(SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC, SYSTEM_CFG_NTP_SERVICE, (char *)NULL);
    if (pcState == NULL)
    {
        return DISABLED;
    }
    else
    {
        if (0 == vosStrCaseCmp(STATE_ENABLE_STR, pcState))
        {
            return ENABLED;
        }
        else {
            return DISABLED;
        }
    }
}

const char* odmSysCfgNTPServerGet()
{
    return vosConfigValueGet(
        SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,
        SYSTEM_CFG_NTP_SERVER,
        "pool.ntp.org");
}


/*******************************************************************************
*
* odmSysCfgDateGet:	get system date
*
* DESCRIPTION:
* 	this function is used to get system date.
*
* INPUTS:
*	pcDate      - date string, format is yyyy-mm-dd
*
* OUTPUTS:
*	n/a.
*
* RETURN:
*	n/a.
*
* SEE ALSO:
*/
UINT32 odmSysCfgDateGet(char *pcDate)
{
    struct tm * t;
    time_t now, adjTime;

    time(&now);
    adjTime = now + mNtpTimeZone + getTimeDstAdj();
    t = gmtime(&adjTime);
    if (t)
    {
        sprintf(pcDate, "%4d-%02d-%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    }
    else
    {
        strcpy(pcDate, "1970-01-01");
    }

    return NO_ERROR;
}

UINT32 odmSysCfgTimeGet(char * pcTime)
{
    struct tm * t;
    time_t now, adjTime;

    time(&now);
    adjTime = now + mNtpTimeZone + getTimeDstAdj();
    t = gmtime(&adjTime);
    if (t)
    {
        sprintf(pcTime, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
    }
    else
    {
        strcpy(pcTime, "00:00:00");
    }

    return NO_ERROR;
}

time_t odmSysCfgTimeGetT(time_t * pTime)
{
    time_t now;

    time(&now);
    *pTime = now + mNtpTimeZone + getTimeDstAdj();

    return *pTime;
}

const char* odmSysCfgTimeZoneGetS()
{
    return vosConfigValueGet(
        SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,
        SYSTEM_CFG_TIME_ZONE,
        "00:00");
}

UINT8 odmSysCfgTimeZoneGet()
{
    char *pcTimeZone;
    pcTimeZone = vosConfigValueGet(
        SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,
        SYSTEM_CFG_TIME_ZONE,
        "00:00");
    if (NULL == pcTimeZone)
    {
        return 0;
    }

    return utilStrToTimeZone(pcTimeZone);
}

UINT8 odmSysCfgDayLightSavingGet()
{
    char *pcState;
    pcState = vosConfigValueGet(SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC, SYSTEM_CFG_DAY_LIGHT_SAVING, (char *)NULL);
    if (pcState == NULL)
    {
        return DISABLED;
    }
    else
    {
        if (0 == vosStrCaseCmp(STATE_ENABLE_STR, pcState))
        {
            return ENABLED;
        }
        else {
            return DISABLED;
        }
    }
}

UINT32 odmSysIfMacAddrGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucMac)
{
	STATUS  retVal = OK;
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    char    *pVal = NULL;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

    if (NULL == pucMac)
    {
        return ERR_NULL_POINTER;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    pVal = vosConfigValueGet(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_MAC_ADDR, (char *)NULL);

    if (NULL == pVal)
    {
        retVal = vosHWAddrGet((UINT8 *)g_acIfEthNameStr[index], pucMac);
        if (OK == retVal)
        {
            return NO_ERROR;
        }
        else
        {
            return ERR_CONFIG_GET_FAIL;
        }
    }
	utilStrToMacAddr(pVal,pucMac);

    return NO_ERROR;
}

UINT32 odmSysCfgIfShareModeGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucIfShareMode)
{
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);

    if (NULL == pucIfShareMode)
    {
        return ERR_NULL_POINTER;
    }

    *pucIfShareMode = INTERFACE_SHARE_MODE_SHARE;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_SHARE_MODE))
    {
        *pucIfShareMode = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_SHARE_MODE,
            INTERFACE_SHARE_MODE_SHARE);
    }
    return NO_ERROR;
}

UINT32 odmSysCfgIfIpModeGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucIfIpMode)
{
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);

    if (NULL == pucIfIpMode)
    {
        return ERR_NULL_POINTER;
    }

    *pucIfIpMode = IF_IP_MODE_STATIC;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_IP_MODE))
    {
        *pucIfIpMode = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_IP_MODE,
            IF_IP_MODE_STATIC);
    }
    return NO_ERROR;
}

/* ip mode changed, notify voip */
UINT32 odmSysCfgIfIpAddrGet(UINT8 ucIf, UINT8 ucIfMode, UINT32 *pulIpAddress)
{
    UINT8 	IfSectionBuff[32] = {0};
    char    strIp[20];
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT8 ucIpMode;

    if (NULL == pulIpAddress)
    {
        return ERR_NULL_POINTER;
    }

    *pulIpAddress = 0;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

    odmSysCfgIfIpModeGet(ucIf, ucIfMode, &ucIpMode);
    if ( ucIpMode == IF_IP_MODE_PPPOE){
        vosIPAddrGet((UINT8 *)g_acIfPPPoENameStr[index], strIp);
        cliStrToIp(strIp, (ULONG *)pulIpAddress);
        return NO_ERROR;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_IP_ADDR))
    {
        *pulIpAddress = vosConfigUInt32Get(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_IP_ADDR,
            0);
        if (0 == *pulIpAddress)
        {
            vosIPAddrGet((UINT8 *)g_acIfEthNameStr[index], strIp);
            cliStrToIp(strIp, (ULONG *)pulIpAddress);
        }
        return NO_ERROR;
    }
    else {
        vosIPAddrGet((UINT8 *)g_acIfEthNameStr[index], strIp);
        cliStrToIp(strIp, (ULONG *)pulIpAddress);
        return NO_ERROR;
    }
}
/* ip mode changed, notify voip */

UINT32 odmSysCfgIfIpAddrGetS(UINT8 ucIf, UINT8 ucIfMode, char *pcIpAddr)
{
    UINT32 ipAddr = 0;
    UINT32 retVal;

    if (NULL == pcIpAddr)
        return ERR_NULL_POINTER;

    retVal = odmSysCfgIfIpAddrGet(ucIf, ucIfMode, &ipAddr);
    if (NO_ERROR != retVal)
    {
        return retVal;
    }

    cliIpToStr(ipAddr, pcIpAddr);
    return NO_ERROR;
}

UINT32 odmSysCfgIfIpMaskGet(UINT8 ucIf, UINT8 ucIfMode, UINT32 *pulIpAddress)
{
    UINT8 	IfSectionBuff[32] = {0};
    char    strIp[20];
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    UINT8 ucIpMode;

    if (NULL == pulIpAddress)
    {
        return ERR_NULL_POINTER;
    }

    *pulIpAddress = 0;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

    odmSysCfgIfIpModeGet(ucIf, ucIfMode, &ucIpMode);
    if ( ucIpMode == IF_IP_MODE_PPPOE){
        vosNetMaskGet((UINT8 *)g_acIfPPPoENameStr[index], strIp);
        cliStrToIp(strIp, (ULONG *)pulIpAddress);
        return NO_ERROR;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_IP_MASK))
    {
        *pulIpAddress = vosConfigUInt32Get(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_IP_MASK,
            0);
        if (0 == *pulIpAddress)
        {
            vosNetMaskGet((UINT8 *)g_acIfEthNameStr[index], strIp);
            cliStrToIp(strIp, (ULONG *)pulIpAddress);
        }
        return NO_ERROR;
    }
    else {
        vosNetMaskGet((UINT8 *)g_acIfEthNameStr[index], strIp);
        cliStrToIp(strIp, (ULONG *)pulIpAddress);
        return NO_ERROR;
    }
}

UINT32 odmSysCfgIfIpMaskGetS(UINT8 ucIf, UINT8 ucIfMode, char *pcIpMask)
{
    UINT32 netMask = 0;
    UINT32 retVal;

    if (NULL == pcIpMask)
        return ERR_NULL_POINTER;

    retVal = odmSysCfgIfIpMaskGet(ucIf, ucIfMode, &netMask);
    if (NO_ERROR != retVal)
    {
        return retVal;
    }

    cliIpToStr(netMask, pcIpMask);
    return NO_ERROR;
}

UINT32 odmSysCfgIfIpGatewayGet(UINT8 ucIf, UINT8 ucIfMode, UINT32 *pulIpAddress)
{
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);

    if (NULL == pulIpAddress)
    {
        return ERR_NULL_POINTER;
    }

    *pulIpAddress = 0;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_IP_GW))
    {
        *pulIpAddress = vosConfigUInt32Get(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_IP_GW,
            0);
    }
    return NO_ERROR;
}

UINT32 odmSysCfgIfIpGatewayGetS(UINT8 ucIf, UINT8 ucIfMode, char *pcIpAddr)
{
    UINT32 ipAddr = 0;
    UINT32 retVal;

    if (NULL == pcIpAddr)
        return ERR_NULL_POINTER;

    retVal = odmSysCfgIfIpGatewayGet(ucIf, ucIfMode, &ipAddr);
    if (NO_ERROR != retVal)
    {
        return retVal;
    }

    cliIpToStr(ipAddr, pcIpAddr);
    return NO_ERROR;
}

UINT32 odmSysCfgIfIpDnsGet(UINT8 ucIf, UINT8 ucIfMode, UINT32 *pulIpAddress)
{
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);

    if (NULL == pulIpAddress)
    {
        return ERR_NULL_POINTER;
    }

    *pulIpAddress = 0;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_IP_DNS))
    {
        *pulIpAddress = vosConfigUInt32Get(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_IP_DNS,
            0);
    }
    return NO_ERROR;
}

UINT32 odmSysCfgIfIpDnsGetS(UINT8 ucIf, UINT8 ucIfMode, char *pcIpAddr)
{
    UINT32 ipAddr = 0;
    UINT32 retVal;

    if (NULL == pcIpAddr)
        return ERR_NULL_POINTER;

    retVal = odmSysCfgIfIpDnsGet(ucIf, ucIfMode, &ipAddr);
    if (NO_ERROR != retVal)
    {
        return retVal;
    }

    cliIpToStr(ipAddr, pcIpAddr);
    return NO_ERROR;
}

UINT32 odmSysCfgIfPPPoEModeGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucPPPoEMode)
{
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);

    if (NULL == pucPPPoEMode)
    {
        return ERR_NULL_POINTER;
    }

    *pucPPPoEMode = PPPOE_AUTH_TYPE_AUTO;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_PPPOE_AUTH))
    {
        *pucPPPoEMode = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_PPPOE_AUTH,
            PPPOE_AUTH_TYPE_AUTO);
    }
    return NO_ERROR;
}

UINT32 odmSysCfgIfPPPoEUserGet(UINT8 ucIf, UINT8 ucIfMode, char *pcUserName)
{
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    char*   pcVal = NULL;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

    if (NULL == pcUserName)
    {
        return ERR_NULL_POINTER;
    }

    *pcUserName = '\0';

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_PPPOE_USER))
    {
        pcVal = vosConfigValueGet(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_PPPOE_USER,
            NULL);
        if (NULL != pcVal)
        {
            vosStrCpy(pcUserName, pcVal);
        }
    }
    return NO_ERROR;
}

UINT32 odmSysCfgIfPPPoEPassGet(UINT8 ucIf, UINT8 ucIfMode, char *pcPassword)
{
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);
    char*   pcVal = NULL;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

    if (NULL == pcPassword)
    {
        return ERR_NULL_POINTER;
    }

    *pcPassword = '\0';

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_PPPOE_PASS))
    {
        pcVal = vosConfigValueGet(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_PPPOE_PASS,
            NULL);
        if (NULL != pcVal)
        {
            vosStrCpy(pcPassword, pcVal);
        }
    }
    return NO_ERROR;
}

UINT32 odmSysCfgIfQoSModeGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucQoSMode)
{
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);

    if (NULL == pucQoSMode)
    {
        return ERR_NULL_POINTER;
    }

    *pucQoSMode = IF_QOS_MODE_DSCP;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_QOS_MODE))
    {
        *pucQoSMode = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_QOS_MODE,
            IF_QOS_MODE_DSCP);
    }
    return NO_ERROR;
}

UINT32 odmSysCfgIfQoSValGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucQoSVal)
{
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);

    if (NULL == pucQoSVal)
    {
        return ERR_NULL_POINTER;
    }

    *pucQoSVal = 0;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_QOS_VAL))
    {
        *pucQoSVal = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_QOS_VAL,
            0);
    }
    return NO_ERROR;
}

UINT32 odmSysCfgIfTagModeGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucTagMode)
{
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);

    if (NULL == pucTagMode)
    {
        return ERR_NULL_POINTER;
    }

    *pucTagMode = IF_TAG_MODE_TRANSPARENT;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_TAG_MODE))
    {
        *pucTagMode = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_TAG_MODE,
            IF_TAG_MODE_TRANSPARENT);
    }
    return NO_ERROR;
}

UINT32 odmSysCfgIfCVLANGet(UINT8 ucIf, UINT8 ucIfMode, UINT16 *pusCVLAN)
{
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);

    if (NULL == pusCVLAN)
    {
        return ERR_NULL_POINTER;
    }

    *pusCVLAN = 0;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_TAG_CVLAN))
    {
        *pusCVLAN = (UINT16)vosConfigUInt32Get(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_TAG_CVLAN,
            0);
    }
    return NO_ERROR;
}

UINT32 odmSysCfgIfPriGet(UINT8 ucIf, UINT8 ucIfMode, UINT8 *pucPri)
{
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);

    if (NULL == pucPri)
    {
        return ERR_NULL_POINTER;
    }

    *pucPri = 0;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_TAG_PRI))
    {
        *pucPri = (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_TAG_PRI,
            0);
    }
    return NO_ERROR;
}



UINT32 odmSysCfgIfSVLANGet(UINT8 ucIf, UINT8 ucIfMode, UINT16 *pusSVLAN)
{
    UINT8 	IfSectionBuff[32] = {0};
    UINT8   index = IF_MODE_TO_INDEX(ucIf, ucIfMode);

    if (NULL == pusSVLAN)
    {
        return ERR_NULL_POINTER;
    }

    *pusSVLAN = 0;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERR_OUT_OF_RANGE;
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);
    if (NO_ERROR == vosConfigKeyIsExisted(SYSTEM_CFG,
        IfSectionBuff, SYSTEM_CFG_IF_TAG_SVLAN))
    {
        *pusSVLAN = (UINT16)vosConfigUInt32Get(SYSTEM_CFG,
            IfSectionBuff, SYSTEM_CFG_IF_TAG_SVLAN,
            0);
    }
    return NO_ERROR;
}

UINT32 odmSysIfConfigGet(ODM_SYS_IF_CONFIG_t *pstIpConfig)
{
    UINT32  retVal;
    UINT8   index;

    if (NULL == pstIpConfig)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING, "%s: ERR_NULL_POINTER", __FUNCTION__);
        return ERR_NULL_POINTER;
    }

    index = IF_MODE_TO_INDEX(pstIpConfig->ucIf, pstIpConfig->ucIfMode);

    if (IS_INVALID_IF_INDEX(index))
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING, "%s: ERR_OUT_OF_RANGE", __FUNCTION__);
        return ERR_OUT_OF_RANGE;
    }

    retVal = odmSysCfgIfShareModeGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, &pstIpConfig->ucIfShareMode);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get share mode failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfIpModeGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, &pstIpConfig->ucIpMode);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get ip mode failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfIpAddrGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, &pstIpConfig->ulIpAddr);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get ip address failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfIpMaskGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, &pstIpConfig->ulIpMask);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get ip mask failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfIpGatewayGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, &pstIpConfig->ulGwAddr);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get gateway failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfIpDnsGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, &pstIpConfig->ulDnsAddr);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get dns failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfPPPoEModeGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, &pstIpConfig->ucPPPoEAuthType);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get pppoe mode failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfPPPoEUserGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, pstIpConfig->acPPPoEUser);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get pppoe username failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfPPPoEPassGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, pstIpConfig->acPPPoEPass);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get pppoe password failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfQoSModeGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, &pstIpConfig->ucQoSMode);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get qos mode failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfQoSValGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, &pstIpConfig->ucQoSVal);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get qos value failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfTagModeGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, &pstIpConfig->ucTagged);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get tag mode failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfCVLANGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, &pstIpConfig->usCVlan);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get cvlan failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfPriGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, &pstIpConfig->ucPriority);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get priority failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    retVal = odmSysCfgIfSVLANGet(pstIpConfig->ucIf,
        pstIpConfig->ucIfMode, &pstIpConfig->usSVLAN);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING,
            "%s: get svlan failed, errno = %u",
            __FUNCTION__, retVal);
        return retVal;
    }

    return NO_ERROR;
}


UINT8 odmSysCfgTelnetServiceGet()
{
    char *pcState;
    pcState = vosConfigValueGet(SYSTEM_CFG,
        SYSTEM_CFG_TELNET_SEC, SYSTEM_CFG_TELNET_SERVICE, (char *)NULL);
    if (pcState == NULL)
    {
        return DISABLED;
    }
    else
    {
        if (0 == vosStrCaseCmp(STATE_ENABLE_STR, pcState))
        {
            return ENABLED;
        }
        else {
            return DISABLED;
        }
    }
}

UINT16 odmSysCfgTelnetPortGet()
{
    return (UINT16)vosConfigUInt32Get(SYSTEM_CFG,
        SYSTEM_CFG_TELNET_SEC, SYSTEM_CFG_TELNET_PORT, DEFAULT_TELNET_PORT);
}

UINT16 odmSysCfgTelnetSessionNumGet()
{
    return (UINT16)vosConfigUInt32Get(SYSTEM_CFG,
        SYSTEM_CFG_TELNET_SEC, SYSTEM_CFG_TELNET_SESSIONS, DEFAULT_SESSION_NUM);
}

UINT32 odmSysCfgSessionTimeoutGet()
{
    return vosConfigUInt32Get(
        SYSTEM_CFG,
        SYSTEM_CFG_TELNET_SEC,
        SYSTEM_CFG_SESSION_TIMEOUT,
        DEFAULT_SESSION_TIMEOUT);
}

UINT8 odmSysCfgWebServiceGet()
{
    char *pcState;
    pcState = vosConfigValueGet(SYSTEM_CFG,
        SYSTEM_CFG_WEB_SEC, SYSTEM_CFG_WEB_SERVICE, (char *)NULL);
    if (pcState == NULL)
    {
        return DISABLED;
    }
    else
    {
        if (0 == vosStrCaseCmp(STATE_ENABLE_STR, pcState))
        {
            return ENABLED;
        }
        else {
            return DISABLED;
        }
    }
}

UINT16 odmSysCfgWebPortGet()
{
    return (UINT16)vosConfigUInt32Get(SYSTEM_CFG,
        SYSTEM_CFG_WEB_SEC, SYSTEM_CFG_WEB_PORT, DEFAULT_WEB_PORT);
}

UINT16 odmSysCfgWebSessionNumGet()
{
    return (UINT16)vosConfigUInt32Get(SYSTEM_CFG,
        SYSTEM_CFG_WEB_SEC, SYSTEM_CFG_WEB_SESSIONS, DEFAULT_SESSION_NUM);
}

void cliOamShow(int fd)
{
	UINT32 ret;
	char loid[25] = {0};
	int loid_len;
	char password[13] = {0};
    int password_len;
    UINT8 oam_status;
    UINT8 *oamCfgStateString[] = {"CTC 0.1","CTC 2.0","CTC 2.1","Auto",NULL};

    vosPrintf(fd, "  [%s]\r\n", "OAM Configuration");
    odmPonOAMVerGet(&oam_status);
    vosPrintf(fd, "    %-20s : %s\r\n", "OAM Version", oamCfgStateString[oam_status]);
    vosPrintf(fd, "\r\n");

    vosPrintf(fd, "  [%s]\r\n", "OAM Status");

    if (oam_ver_type == 2 ) {//OAM_VER_CTC_2_1_SUPPORT

	    vosPrintf(fd, "    %-20s : %s\r\n", "OAM Version", "CTC 2.1");

	    ret = odmPonLoidGet(&loid[0]);
	    if(ret == ERR_NULL_LOID)
	    {
		    vosPrintf(fd, "    %-20s : \r\n", "Logical ONU ID");
	    }
	    else
	    {
		    loid_len = vosStrLen(loid);
		    loid[loid_len] = '\0';
		    vosPrintf(fd, "    %-20s : %s\r\n", "Logical ONU ID", loid);
	    }

	    ret = odmPonPasswordGet(&password[0]);
	    if(ret == ERR_NULL_PASSWORD)
	    {
		    vosPrintf(fd, "    %-20s : \r\n", "ONU Password");
	    }
	    else
	    {
		    password_len = vosStrLen(password);
		    password[password_len] = '\0';
		  //modified by tanglin 2010-06-28 for the bug3005
		   // vosPrintf(fd, "    %-20s : %s\r\n", "ONU Password", password);
		  vosPrintf(fd, "    %-20s : %s\r\n", "ONU Password", "******"); 
	    }

#if 0
        // hide code for bug 2324
        OamStatusGet(&oam_status);
        if (oam_status == EX_DISCOVERY_SEND_ANY)
            vosPrintf(fd, "    %-20s : %s\r\n", "ONU Status", "Registered");
        else
            vosPrintf(fd, "    %-20s : %s\r\n", "ONU Status", "No Registered");
#endif

    }else if (0 == oam_ver_type ||
              1 == oam_ver_type)
    {

	    vosPrintf(fd, "    %-20s : %s\r\n", "OAM Version", "CTC 2.0");
        OamStatusGet(&oam_status);
        if (oam_status == EX_DISCOVERY_SEND_ANY)
            vosPrintf(fd, "    %-20s : %s\r\n", "ONU Status", "Registered");
        else
            vosPrintf(fd, "    %-20s : %s\r\n", "ONU Status", "No Registered");
    }

    vosPrintf(fd, "\r\n");

	return;
}

UINT32 odmSysInfoShow(ENV_t *pstEnv)
{
    int i;
    char acDate[STR_SIZE_32_BYTES] = {0};
    char acTime[STR_SIZE_32_BYTES] = {0};

    UINT8 ucIf, ucIfMode;
	UINT8 ucPrevIf = 0xFF;
    UINT8 acMac[6] = {0};
    char acIpAddr[STR_SIZE_32_BYTES] = {0};
    char acIpMask[STR_SIZE_32_BYTES] = {0};
    char acIpGw[STR_SIZE_32_BYTES] = {0};
    char acIpDns[STR_SIZE_32_BYTES] = {0};
    UINT8 ucIfShareMode;
    UINT8 ucIpMode;
    UINT8 ucPPPoEAuthType;
    char acPPPoEUser[STR_SIZE_32_BYTES] = {0};
    char acPPPoEPass[STR_SIZE_32_BYTES] = {0};
    UINT8 ucTagged;
    UINT16 usCVlan;
    UINT8 ucPriority;
    UINT16 usSVLAN;

    vosPrintf(pstEnv->nWriteFd, "System Information:\r\n");

    vosPrintf(pstEnv->nWriteFd, "  [%s]\r\n", SYSTEM_CFG_GENERAL_SEC);
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", SYSTEM_CFG_SYS_NAME, odmSysCfgNameGet());
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", SYSTEM_CFG_SYS_LOCATION, odmSysCfgLocationGet());
    odmSysCfgDateGet(acDate);
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", "Date", acDate);
    odmSysCfgTimeGet(acTime);
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", "Time", acTime);
    /*vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", SYSTEM_CFG_TIME_ZONE, odmSysCfgTimeZoneGetS());*/
    /*vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", SYSTEM_CFG_DAY_LIGHT_SAVING, STATE_STR_GET(odmSysCfgDayLightSavingGet()));*/
    vosPrintf(pstEnv->nWriteFd, "\r\n");

	vosPrintf(pstEnv->nWriteFd, "OAM Information:\r\n");
	cliOamShow(pstEnv->nWriteFd);

    vosPrintf(pstEnv->nWriteFd, "Interfaces:\r\n");
    for (i = 0; i < (INTERFACE_MAX*INTERFACE_MODE_MAX); i++)
    {
        INDEX_TO_IF_MODE(i, ucIf, ucIfMode);

        vosPrintf(pstEnv->nWriteFd,
            "  [%s - %s]\r\n", g_acIfStr[ucIf],
            g_acIfModeStr[ucIfMode]);

        odmSysIfMacAddrGet(ucIf, ucIfMode, acMac);
        vosPrintf(pstEnv->nWriteFd,
            "    %-20s : %02X:%02X:%02X:%02X:%02X:%02X\r\n",
            SYSTEM_CFG_IF_MAC_ADDR,
            acMac[0], acMac[1], acMac[2], acMac[3], acMac[4], acMac[5]);
		
        odmSysCfgIfShareModeGet(ucIf, ucIfMode, &ucIfShareMode);
        vosPrintf(pstEnv->nWriteFd,
            "    %-20s : %s\r\n",
            SYSTEM_CFG_IF_SHARE_MODE, g_acIfShareModeStr[ucIfShareMode]);

        odmSysCfgIfIpModeGet(ucIf, ucIfMode, &ucIpMode);
        vosPrintf(pstEnv->nWriteFd,
            "    %-20s : %s\r\n",
            SYSTEM_CFG_IF_IP_MODE, g_acIfIpModeStr[ucIpMode]);

        odmSysCfgIfIpAddrGetS(ucIf, ucIfMode, acIpAddr);
        vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n",
            SYSTEM_CFG_IF_IP_ADDR, acIpAddr);
        odmSysCfgIfIpMaskGetS(ucIf, ucIfMode, acIpMask);
        vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n",
            SYSTEM_CFG_IF_IP_MASK, acIpMask);
        odmSysCfgIfIpGatewayGetS(ucIf, ucIfMode, acIpGw);
        vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n",
            SYSTEM_CFG_IF_IP_GW, acIpGw);
        odmSysCfgIfIpDnsGetS(ucIf, ucIfMode, acIpDns);
        vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n",
            SYSTEM_CFG_IF_IP_DNS, acIpDns);

        if (IF_IP_MODE_PPPOE == ucIpMode)
        {
            odmSysCfgIfPPPoEModeGet(ucIf, ucIfMode, &ucPPPoEAuthType);
            vosPrintf(pstEnv->nWriteFd,
                "    %-20s : %s\r\n",
                SYSTEM_CFG_IF_PPPOE_AUTH, g_acPPPoEAuthStr[ucPPPoEAuthType]);

            odmSysCfgIfPPPoEUserGet(ucIf, ucIfMode, acPPPoEUser);
            vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n",
                SYSTEM_CFG_IF_PPPOE_USER, acPPPoEUser);
            odmSysCfgIfPPPoEPassGet(ucIf, ucIfMode, acPPPoEPass);
            vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n",
                SYSTEM_CFG_IF_PPPOE_PASS, acPPPoEPass);
        }

#if 0  /* deleted by James.Kan - 2009/10/20 */
        odmSysCfgIfTagModeGet(ucIf, ucIfMode, &ucTagged);
        vosPrintf(pstEnv->nWriteFd,
            "    %-20s : %s\r\n",
            SYSTEM_CFG_IF_TAG_MODE, g_acIfTagModeStr[ucTagged]);

        if (IF_TAG_MODE_TRANSPARENT != ucTagged)
        {
            odmSysCfgIfCVLANGet(ucIf, ucIfMode, &usCVlan);
            vosPrintf(pstEnv->nWriteFd,
                "    %-20s : %d\r\n",
                SYSTEM_CFG_IF_TAG_CVLAN, usCVlan);

            odmSysCfgIfPriGet(ucIf, ucIfMode, &ucPriority);
            vosPrintf(pstEnv->nWriteFd,
                "    %-20s : %d\r\n",
                SYSTEM_CFG_IF_TAG_PRI, ucPriority);

            if (IF_TAG_MODE_VLAN_STACKING == ucTagged)
            {
                odmSysCfgIfSVLANGet(ucIf, ucIfMode, &usSVLAN);
                vosPrintf(pstEnv->nWriteFd,
                    "    %-20s : %d\r\n",
                    SYSTEM_CFG_IF_TAG_SVLAN, usSVLAN);
            }
        }
#endif /* #if 0 */

		/* if share mode ,don't show signal and media configure information */
		if (ucIf != ucPrevIf && INTERFACE_MODE_ADMIN == ucIfMode && INTERFACE_SHARE_MODE_SHARE == ucIfShareMode)
		{
			i += INTERFACE_MODE_MAX - 1;
			continue;
		}
		ucPrevIf = ucIf;
    }
    vosPrintf(pstEnv->nWriteFd, "\r\n");

    vosPrintf(pstEnv->nWriteFd, "  [%s]\r\n", SYSTEM_CFG_TELNET_SEC);
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", SYSTEM_CFG_TELNET_SERVICE, STATE_STR_GET(odmSysCfgTelnetServiceGet()));
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %d\r\n", SYSTEM_CFG_TELNET_PORT, odmSysCfgTelnetPortGet());
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %d\r\n", SYSTEM_CFG_TELNET_SESSIONS, odmSysCfgTelnetSessionNumGet());
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %d\r\n", SYSTEM_CFG_SESSION_TIMEOUT, odmSysCfgSessionTimeoutGet());
    vosPrintf(pstEnv->nWriteFd, "\r\n");

	UINT32 mtuSize;
	odmSysCPUPathMtuGet(&mtuSize);
	vosPrintf(pstEnv->nWriteFd, "  [%s]\r\n", SYSTEM_CFG_MTU_SEC);
	vosPrintf(pstEnv->nWriteFd, "    %-20s : %d\r\n", SYSTEM_CFG_MTU_SIZE, mtuSize);
	vosPrintf(pstEnv->nWriteFd, "\r\n");

    vosPrintf(pstEnv->nWriteFd, "  [Log]\r\n");
    for (i = 0; i < MAX_MODULE; i++)
    {
        vosPrintf(pstEnv->nWriteFd, "    %-20s : %-10s - %s\r\n", "Debug Level",
            g_pcModule[i], g_pcDebugLevel[cliSessionDebugLevelGet(pstEnv->ucIndex, i)]);
    }
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", "Log File", STATE_STR_GET(sysLogGetLogEnable()));
    vosPrintf(pstEnv->nWriteFd, "\r\n");

    vosPrintf(pstEnv->nWriteFd, "Session Information:\r\n");
    cliSessionShow(pstEnv->nWriteFd);
}

UINT32 odmSysCfgShow(ENV_t *pstEnv)
{
    int i;
    char acDate[STR_SIZE_32_BYTES] = {0};
    char acTime[STR_SIZE_32_BYTES] = {0};

    UINT8 ucIf, ucIfMode;
	UINT8 ucPrevIf = 0xFF;	
    UINT8 acMac[6] = {0};
    char acIpAddr[STR_SIZE_32_BYTES] = {0};
    char acIpMask[STR_SIZE_32_BYTES] = {0};
    char acIpGw[STR_SIZE_32_BYTES] = {0};
    char acIpDns[STR_SIZE_32_BYTES] = {0};
    UINT8 ucIfShareMode;
    UINT8 ucIpMode;
    UINT8 ucPPPoEAuthType;
    char acPPPoEUser[STR_SIZE_32_BYTES] = {0};
    char acPPPoEPass[STR_SIZE_32_BYTES] = {0};
    UINT8 ucTagged;
    UINT16 usCVlan;
    UINT8 ucPriority;
    UINT16 usSVLAN;

    vosPrintf(pstEnv->nWriteFd, "System Configuration:\r\n");

    vosPrintf(pstEnv->nWriteFd, "  [%s]\r\n", SYSTEM_CFG_GENERAL_SEC);
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", SYSTEM_CFG_SYS_NAME, odmSysCfgNameGet());
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", SYSTEM_CFG_SYS_LOCATION, odmSysCfgLocationGet());

    odmSysCfgDateGet(acDate);
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", "Date", acDate);
    odmSysCfgTimeGet(acTime);
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", "Time", acTime);
    /*vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", SYSTEM_CFG_TIME_ZONE, odmSysCfgTimeZoneGetS());*/
    /*vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", SYSTEM_CFG_DAY_LIGHT_SAVING, STATE_STR_GET(odmSysCfgDayLightSavingGet()));*/
    vosPrintf(pstEnv->nWriteFd, "\r\n");

    vosPrintf(pstEnv->nWriteFd, "Interfaces:\r\n");
    for (i = 0; i < (INTERFACE_MAX*INTERFACE_MODE_MAX); i++)
    {
        INDEX_TO_IF_MODE(i, ucIf, ucIfMode);

        vosPrintf(pstEnv->nWriteFd,
            "  [%s - %s]\r\n", g_acIfStr[ucIf],
            g_acIfModeStr[ucIfMode]);

        odmSysIfMacAddrGet(ucIf, ucIfMode, acMac);
        vosPrintf(pstEnv->nWriteFd,
            "    %-20s : %02X:%02X:%02X:%02X:%02X:%02X\r\n",
            SYSTEM_CFG_IF_MAC_ADDR,
            acMac[0], acMac[1], acMac[2], acMac[3], acMac[4], acMac[5]);

		odmSysCfgIfShareModeGet(ucIf, ucIfMode, &ucIfShareMode);
        vosPrintf(pstEnv->nWriteFd,
            "    %-20s : %s\r\n",
            SYSTEM_CFG_IF_SHARE_MODE, g_acIfShareModeStr[ucIfShareMode]);

        odmSysCfgIfIpModeGet(ucIf, ucIfMode, &ucIpMode);
        vosPrintf(pstEnv->nWriteFd,
            "    %-20s : %s\r\n",
            SYSTEM_CFG_IF_IP_MODE, g_acIfIpModeStr[ucIpMode]);

        odmSysCfgIfIpAddrGetS(ucIf, ucIfMode, acIpAddr);
        vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n",
            SYSTEM_CFG_IF_IP_ADDR, acIpAddr);
        odmSysCfgIfIpMaskGetS(ucIf, ucIfMode, acIpMask);
        vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n",
            SYSTEM_CFG_IF_IP_MASK, acIpMask);
        odmSysCfgIfIpGatewayGetS(ucIf, ucIfMode, acIpGw);
        vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n",
            SYSTEM_CFG_IF_IP_GW, acIpGw);
        odmSysCfgIfIpDnsGetS(ucIf, ucIfMode, acIpDns);
        vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n",
            SYSTEM_CFG_IF_IP_DNS, acIpDns);

        if (IF_IP_MODE_PPPOE == ucIpMode)
        {
            odmSysCfgIfPPPoEModeGet(ucIf, ucIfMode, &ucPPPoEAuthType);
            vosPrintf(pstEnv->nWriteFd,
                "    %-20s : %s\r\n",
                SYSTEM_CFG_IF_PPPOE_AUTH, g_acPPPoEAuthStr[ucPPPoEAuthType]);

            odmSysCfgIfPPPoEUserGet(ucIf, ucIfMode, acPPPoEUser);
            vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n",
                SYSTEM_CFG_IF_PPPOE_USER, acPPPoEUser);
            odmSysCfgIfPPPoEPassGet(ucIf, ucIfMode, acPPPoEPass);
            vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n",
                SYSTEM_CFG_IF_PPPOE_PASS, acPPPoEPass);
        }

        odmSysCfgIfTagModeGet(ucIf, ucIfMode, &ucTagged);
        vosPrintf(pstEnv->nWriteFd,
            "    %-20s : %s\r\n",
            SYSTEM_CFG_IF_TAG_MODE, g_acIfTagModeStr[ucTagged]);

        if (IF_TAG_MODE_TRANSPARENT != ucTagged)
        {
            odmSysCfgIfCVLANGet(ucIf, ucIfMode, &usCVlan);
            vosPrintf(pstEnv->nWriteFd,
                "    %-20s : %d\r\n",
                SYSTEM_CFG_IF_TAG_CVLAN, usCVlan);

            odmSysCfgIfPriGet(ucIf, ucIfMode, &ucPriority);
            vosPrintf(pstEnv->nWriteFd,
                "    %-20s : %d\r\n",
                SYSTEM_CFG_IF_TAG_PRI, ucPriority);

            if (IF_TAG_MODE_VLAN_STACKING == ucTagged)
            {
                odmSysCfgIfSVLANGet(ucIf, ucIfMode, &usSVLAN);
                vosPrintf(pstEnv->nWriteFd,
                    "    %-20s : %d\r\n",
                    SYSTEM_CFG_IF_TAG_SVLAN, usSVLAN);
            }
        }

		/* if share mode ,don't show signal and media configure information */
		if (ucIf != ucPrevIf && INTERFACE_MODE_ADMIN == ucIfMode && INTERFACE_SHARE_MODE_SHARE == ucIfShareMode)
		{
			i += INTERFACE_MODE_MAX - 1;
			continue;
		}
		ucPrevIf = ucIf;		
    }
    vosPrintf(pstEnv->nWriteFd, "\r\n");

    vosPrintf(pstEnv->nWriteFd, "  [%s]\r\n", SYSTEM_CFG_TELNET_SEC);
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", SYSTEM_CFG_TELNET_SERVICE, STATE_STR_GET(odmSysCfgTelnetServiceGet()));
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %d\r\n", SYSTEM_CFG_TELNET_PORT, odmSysCfgTelnetPortGet());
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %d\r\n", SYSTEM_CFG_TELNET_SESSIONS, odmSysCfgTelnetSessionNumGet());
    vosPrintf(pstEnv->nWriteFd, "    %-20s : %d\r\n", SYSTEM_CFG_SESSION_TIMEOUT, odmSysCfgSessionTimeoutGet());

    vosPrintf(pstEnv->nWriteFd, "\r\n");
}

STATUS CheckRset(VOID)
{
    extern UINT8 g_bLightAlways;
    unsigned int uiPonOffFlag = 0;
    int iRet = 0;

    iRet = Ioctl_GetPonModeOffFlag(&uiPonOffFlag);
    if (0 != iRet)
    {
        uiPonOffFlag = FALSE;
    }
    static UINT8 ucForbidreset = FALSE;

    if ((TRUE == g_bLightAlways) &&
        (FALSE == uiPonOffFlag) &&
        (FALSE == ucForbidreset))
    {
        ucForbidreset = TRUE;
        return ERROR;
    }
    else
    {
        ucForbidreset = FALSE;
        return OK;
    }
}
STATUS odmSysReset(void)
{
    #if 1
 
    if (OK != CheckRset())
    {
        return OK;
    }
    #endif
	
    /*oplWdtEnable (0);*/
    OP_BCAST("System is rebooting now ... ");
    vosSleep(1);
    vosReboot();

	return OK;
}

STATUS odmSysCfgTempDirGet(char *pcDest)
{
    if (NULL == pcDest)
        return ERROR;

    const char* pcTempDir = vosConfigValueGet(
        SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,
        SYSTEM_CFG_TEMP_DIR,
        NULL);

    if (NULL == pcTempDir)
    {
        vosStrCpy(pcDest, DEFAULT_TEMP_PATH);
    }
    else
    {
        vosStrCpy(pcDest, pcTempDir);
    }

    return OK;
}

STATUS odmSysCfgMsqDirGet(char *pcDest)
{
    UINT32 ipAddr;

    if (NULL == pcDest)
        return ERROR;

    const char* pcTempDir = vosConfigValueGet(
        SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC,
        SYSTEM_CFG_MSQ_DIR,
        NULL);

    if (NULL == pcTempDir)
    {
        vosSprintf(pcDest, DEFAULT_TEMP_MSQ_PATH, vosThreadMainGet());
    }
    else
    {
        vosStrCpy(pcDest, pcTempDir);
    }

    return OK;
}

#ifdef OPL_COM_UART1
STATUS odmOnuUartCommEnableSet(UINT8 enable)
{
    switch (enable)
    {
        case TRUE:
            vosConfigUInt32Set(SYSTEM_CFG,SYSTEM_CFG_GENERAL_SEC,
                              SYSTEM_CFG_UART1_COMM,TRUE);
            break;
        case FALSE:
            vosConfigUInt32Set(SYSTEM_CFG,SYSTEM_CFG_GENERAL_SEC,
                              SYSTEM_CFG_UART1_COMM,FALSE);
            break;
        default :
            break;

    }
    return OK;
}

STATUS odmOnuUartCommEnableGet(UINT8* enable)
{
    if (NULL == enable)
    {
        return ERROR;
    }
    *enable = vosConfigUInt32Get(SYSTEM_CFG,SYSTEM_CFG_GENERAL_SEC,
                              SYSTEM_CFG_UART1_COMM,FALSE);
    return OK;
}
#endif

static STATUS odmPPPoEStatusCheck(UINT8 index)
{
    char acStatusStr[16] = {0};
    char *pStatusStr = NULL;
	char acFile[64] = {0};
	char acCommands[128] = {0};
    FILE *fd = NULL;

    if (IS_INVALID_IF_INDEX(index))
    {
		OP_DEBUG(DEBUG_LEVEL_WARNING, "%s: invalid index (%d)", __FUNCTION__, index);
        return ERROR;
    }

    vosSprintf(acCommands, 
            "pppoe-check /etc/ppp/pppoe%d.conf /etc/ppp/pppoe%d.status", 
            index, index);
    vosSystem(acCommands);
    vosSystem("sync");
    vosSleep(1);
	vosSprintf(acFile, "/etc/ppp/pppoe%d.status", index);
	fd = vosFOpen(acFile,"r");
    if ( fd == NULL ){
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING, 
            "%s: %s nonexisten! ", 
            __FUNCTION__, acFile);
        return ERROR;
    }
    fgets(acStatusStr,15,fd);
    acStatusStr[15] = '\0';
    fclose(fd);

    pStatusStr = utilStrTrim(acStatusStr);
    if (0 == vosStrLen(pStatusStr))
    {
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING, 
            "%s: 0 == vosStrLen(pStatusStr) ", 
            __FUNCTION__);
        return ERROR;
    }

    if (0 != vosStrCmp("down", pStatusStr))
    {
        vosStrCpy(g_acIfPPPoENameStr[index], pStatusStr);
        return OK;
    }
    else
    {
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING, 
            "%s: 0 == vosStrCmp(\"down\", pStatusStr) ", 
            __FUNCTION__);
        return ERROR;
    }
}

static void odmPPPoEThread(void *pvParam)
{
    int pid;
    int ret = 0;
    IF_MODE_VOIP_PARA_S *pIfModeVoipPara = (IF_MODE_VOIP_PARA_S *)pvParam;
    char userName[32] = {0};
    char passWord[32] = {0};
    char acFileName[32] = {0};
	char acCommands[128] = {0};

    char acIpAddr[STR_SIZE_32_BYTES] = {0};
    char acIpMask[STR_SIZE_32_BYTES] = {0};
    char acIpGw[STR_SIZE_32_BYTES] = {0};
    char dnsIpAddr[STR_SIZE_32_BYTES] = {0};
    UINT8 macAddr_str[6] = {0};
    UINT8 acMac[6] = {0};
    char dnsIpStr[16] = "";
    char ifName[32] = {0};
	char newIpStr[32] = {0};
    char *pIpStr = NULL;
    UINT32 dnsIp;
	UINT32 newIp;

    FILE *fd = NULL;
    UINT8 dnsBuff[128] = {0};
    UINT32 readSize = 0;

    UINT8 	IfSectionBuff[32] = {0};
    UINT8   retryCnt = 0;
	IF_MODE_VOIP_PARA_S IpModePara;
	UINT8 ucIf = pIfModeVoipPara->ucIf;
	UINT8 ucIfMode = pIfModeVoipPara->ucMode;

    UINT8 index = IF_MODE_TO_INDEX(pIfModeVoipPara->ucIf, pIfModeVoipPara->ucMode);

    odmSysCfgIfPPPoEUserGet(pIfModeVoipPara->ucIf, pIfModeVoipPara->ucMode, userName);
    odmSysCfgIfPPPoEPassGet(pIfModeVoipPara->ucIf, pIfModeVoipPara->ucMode, passWord);

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: enter ", __FUNCTION__);

    vosSprintf(acCommands, "pppoe-stop /etc/ppp/pppoe%d.conf >&/dev/null", index);
	vosSystem(acCommands);
    vosSleep(1);

	vosSprintf(acCommands, "pppoe-start /etc/ppp/pppoe%d.conf >&/dev/null", index);
	vosSystem(acCommands);

    vosSleep(1);

    IpModePara.modeChangeType = pIfModeVoipPara->modeChangeType;
	IpModePara.index = index;
	IpModePara.ucIf = ucIf;
	IpModePara.ucMode = ucIfMode;

ppp_restart:
    while (OK != odmPPPoEStatusCheck(index))
    {
        retryCnt++;
        vosSleep(1);
        if (retryCnt == 20)
        {
        	vosSprintf(acCommands, "pppoe-stop /etc/ppp/pppoe%d.conf >&/dev/null", index);
        	vosSystem(acCommands);
        	vosSprintf(acCommands, "pppoe-start /etc/ppp/pppoe%d.conf >&/dev/null", index);
        	vosSystem(acCommands);
            retryCnt = 0;
        }
    }

	vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);

	/* update dns */
    fd = vosFOpen("/etc/resolv.conf","r");
    if ( fd != NULL ){
        fgets(dnsBuff,64,fd);
        strcpy(dnsIpStr,dnsBuff+11);/* +11:"nameserver " */
    	dnsIpStr[15] = '\0';
        dnsIp = 0;
        pIpStr = utilStrTrim(dnsIpStr);
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: dnsIpStr = %s", __FUNCTION__, pIpStr);
    	cliStrToIp(pIpStr, (ULONG *)&dnsIp);
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: dnsIp = %x", __FUNCTION__, dnsIp);
    	odmSysCfgIfIpDnsSet(pIfModeVoipPara->ucIf, pIfModeVoipPara->ucMode,dnsIp);
        fclose(fd);
    }

	/* update ipaddress, let the odm get the low level driver */
	vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_IP_ADDR,0);

	/* update netmask */
	vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_IP_MASK,0);

	/* update gateway */
    newIpStr[0] = '\0';
    vosSprintf(acCommands, 
    	"route -n | grep UH | grep %s | awk -F\" \" '{print $1}' > /tmp/route.%s.conf",
		g_acIfPPPoENameStr[index],
		g_acIfEthNameStr[index]);
	vosSystem(acCommands);

    vosUSleep(300000);
	vosSprintf(acCommands, "/tmp/route.%s.conf", g_acIfEthNameStr[index]);
	fd = vosFOpen(acCommands,"r");
    if ( fd != NULL ){
        fgets(newIpStr,16,fd);
        newIpStr[31] = '\0';
        fclose(fd);
    	/*vosSprintf(acCommands, "rm -f /tmp/route.%s.conf", g_acIfEthNameStr[index]);
    	vosSystem(acCommands);*/
    	newIp = 0;
        pIpStr = utilStrTrim(newIpStr);
    	cliStrToIp(pIpStr, (ULONG *)&newIp);
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: update gateway %s ", __FUNCTION__, pIpStr);
    	vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_IP_GW,newIp);
    }

    /* ip mode changed, notify voip */
	ipModeChangeNotifyVoip(NET_UP,
	    IpModePara.modeChangeType,
	    ucIf, 
	    ucIfMode,
	    &IpModePara);

	memset(&IpModePara,0,sizeof(IF_MODE_VOIP_PARA_S));
    IpModePara.modeChangeType = IP_MODE_STAY_SAME;
	IpModePara.index = index;
	IpModePara.ucIf = ucIf;
	IpModePara.ucMode = ucIfMode;

    retryCnt = 0;
	while (1)
	{
		vosSleep(5);
		if (OK == odmPPPoEStatusCheck(index))
		{
			continue;
		}
		/* if pppoe status has changed,must notify voip again */
		vosSleep(1);
	    while (OK != odmPPPoEStatusCheck(index))
	    {
	        vosSleep(1);
            retryCnt++;
	        if (retryCnt == 20)
	        {
                ipModeChangeNotifyVoip(NET_DOWN,IP_MODE_STAY_SAME,ucIf, ucIfMode,&IpModePara);		
    	       	vosSprintf(acCommands, "pppoe-stop /etc/ppp/pppoe%d.conf >&/dev/null", index);
	        	vosSystem(acCommands);
	        	vosSprintf(acCommands, "pppoe-start /etc/ppp/pppoe%d.conf >&/dev/null", index);
	        	vosSystem(acCommands);
	            retryCnt = 0;
                goto ppp_restart;
	        }
	    }
	}
    
    vosMutexTake(&g_pastIpModeThreadMutex);

    if (NULL != g_pstPPPoEThreadPara[index])
    {
        vosFree(g_pstPPPoEThreadPara[index]);
        g_pstPPPoEThreadPara[index] = NULL;
    }

    vosMutexGive(&g_pastIpModeThreadMutex);
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: exit", __FUNCTION__);
    vosThreadExit(0);
}

static STATUS odmDHCPThreadCheck(UINT8 index)
{
    char acPidStr[16] = {0};
    char *pPidStr = NULL;
	char acPidFile[64] = {0};
	char acCommands[128] = {0};
    FILE *fd = NULL;
    VOS_DIR_t * pDir;

    if (IS_INVALID_IF_INDEX(index))
    {
		OP_DEBUG(DEBUG_LEVEL_WARNING, "%s: invalid index (%d)", __FUNCTION__, index);
        return ERROR;
    }

	vosSprintf(acPidFile, "/var/udhcpc.%s.pid", g_acIfEthNameStr[index]);
	fd = vosFOpen(acPidFile,"r");
    if ( fd == NULL ){
        return ERROR;
    }
    fgets(acPidStr,15,fd);
    acPidStr[15] = '\0';
    fclose(fd);

    pPidStr = utilStrTrim(acPidStr);
    if (0 == vosStrLen(pPidStr))
    {
        vosSprintf(acCommands, "rm -f /var/udhcpc.%s.pid", g_acIfEthNameStr[index]);
        vosSystem(acCommands);
        return ERROR;
    }

    vosSprintf(acPidFile, "/proc/%s", pPidStr);
    pDir = vosOpenDir(acPidFile);
    if (pDir != NULL)
    {
        return OK;
    }
    else
    {
        return ERROR;
    }
}


/* ip mode changed, notify voip */
static void odmDHCPThread(void *pvParam)
{
	int pid = 0;
	char acCommands[128] = {0};
	char acPidFile[64] = {0};
	char ipAddr[64] = {0};
    IF_MODE_VOIP_PARA_S *pifModeChangePara = NULL;
    UINT8 index;
    UINT8 ucIf = 0;
	UINT8 ucIfMode = 0;

    int res = 0;

    const char *ifModeStr[INTERFACE_MODE_MAX] = {"Share", "Signal", "Media"};

    char acIpAddr[STR_SIZE_32_BYTES] = {0};
    char acIpMask[STR_SIZE_32_BYTES] = {0};
    char acIpGw[STR_SIZE_32_BYTES] = {0};
    char dnsIpAddr[STR_SIZE_32_BYTES] = {0};
    UINT8 macAddr_str[6] = {0};
    UINT8 acMac[6] = {0};
    char dnsIpStr[16] = "";

    char ifName[32] = {0};
	char newIpStr[32] = {0};
    char *pIpStr = NULL;

    UINT32 dnsIp;
	UINT32 newIp;

    VOS_DIR_t * pDir;
    FILE *fd = NULL;
    UINT8 dnsBuff[128] = {0};
    UINT32 readSize = 0;

    UINT8 	IfSectionBuff[32] = {0};

    pifModeChangePara = (IF_MODE_VOIP_PARA_S *)pvParam;
    index = (UINT8)(pifModeChangePara->index);

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: enter", __FUNCTION__);

    vosSleep(1);

    vosSystem("umount nfs");
    vosSprintf(acCommands, "udhcpc -i %s -p /var/udhcpc.%s.pid >&/dev/null",
		g_acIfEthNameStr[index],
		g_acIfEthNameStr[index]);
	vosSystem(acCommands);

    newIpStr[0] = 0;
    while ((OK != vosIPAddrGet((UINT8 *)g_acIfEthNameStr[index], newIpStr)) ||
        0 == newIpStr[0] || 
        0 == vosStrCmp("0.0.0.0", newIpStr))
    {
        newIpStr[0] = 0;
        vosSleep(1);
    }
    newIpStr[0] = 0;

	/* set dns */
    pDir = vosOpenDir("/tmp");
    if (pDir != NULL)
    {
        vosCloseDir(pDir);

		vosSprintf(IfSectionBuff, SYSTEM_CFG_IF_SEC, index);

		/* update dns */
        fd = vosFOpen("/tmp/resolv.conf.auto","r");
        if ( fd != NULL ){
    	    fgets(dnsBuff,64,fd);
    	    strcpy(dnsIpStr,dnsBuff+11);/* +11:"nameserver " */
     	    dnsIpStr[15] = '\0';
            dnsIp = 0;
            pIpStr = utilStrTrim(dnsIpStr);
            OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: dnsIpStr = %s", __FUNCTION__, pIpStr);
    		cliStrToIp(pIpStr, (ULONG *)&dnsIp);
            OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: dnsIp = %x", __FUNCTION__, dnsIp);
    		odmSysCfgIfIpDnsSet(pifModeChangePara->ucIf, pifModeChangePara->ucMode,dnsIp);
            fclose(fd);
        }

		/* update ipaddress, let the odm get the low level driver */
		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_IP_ADDR,0);

		/* update netmask */
		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_IP_MASK,0);

		/* update gateway */
        newIpStr[0] = '\0';
	    vosSprintf(acCommands, 
	    	"route -n | grep 0.0.0.0 | grep UG | grep %s | awk -F\" \" '{print $2}' > /tmp/route.%s.conf",
			g_acIfEthNameStr[index],
			g_acIfEthNameStr[index]);
		vosSystem(acCommands);
		vosSprintf(acCommands, "/tmp/route.%s.conf", g_acIfEthNameStr[index]);
		fd = vosFOpen(acCommands,"r");
        if ( fd != NULL ){
    	    fgets(newIpStr,16,fd);
            newIpStr[31] = '\0';
            fclose(fd);
    		/*vosSprintf(acCommands, "rm -f /tmp/route.%s.conf", g_acIfEthNameStr[index]);
    		vosSystem(acCommands);*/
    		newIp = 0;
            pIpStr = utilStrTrim(newIpStr);
    		cliStrToIp(pIpStr, (ULONG *)&newIp);
            OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: update gateway %s ", __FUNCTION__, pIpStr);
    		vosConfigUInt32Set(SYSTEM_CFG,IfSectionBuff,SYSTEM_CFG_IF_IP_GW,newIp);
        }
    }
    /* notify voip */
    ipModeChangeNotifyVoip(NET_UP,
        pifModeChangePara->modeChangeType,
        pifModeChangePara->ucIf,
        pifModeChangePara->ucMode,
        pifModeChangePara);

    vosMutexTake(&g_pastIpModeThreadMutex);
    if (NULL != g_pastDHCPThreadPara[index])
    {
        vosFree(g_pastDHCPThreadPara[index]);
        g_pastDHCPThreadPara[index] = NULL;
    }
    vosMutexGive(&g_pastIpModeThreadMutex);

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: exit", __FUNCTION__);
    vosThreadExit(0);
}
/* ip mode changed, notify voip */


STATUS odmPPPoEThreadStart(IF_MODE_VOIP_PARA_S  *pifModeChangePara)
{
    STATUS retVal = OK;

    vosMutexTake(&g_pastIpModeThreadMutex);

    if (NULL == pifModeChangePara)
    {
		OP_DEBUG(DEBUG_LEVEL_WARNING, "%s: NULL == pifModeChangePara", __FUNCTION__);
        retVal = ERROR;
        goto exit_label;
    }

    if (OK == vosThreadVerify(g_pstPPPoEThreadId[pifModeChangePara->index])) {
        vosThreadDestroy(g_pstPPPoEThreadId[pifModeChangePara->index]);

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: destory g_pstPPPoEThreadId[%d] = %X", 
            __FUNCTION__, pifModeChangePara->index, 
            g_pstPPPoEThreadId[pifModeChangePara->index]);

        if (NULL != g_pstPPPoEThreadPara[pifModeChangePara->index])
        {
            vosFree(g_pstPPPoEThreadPara[pifModeChangePara->index]);
            g_pstPPPoEThreadPara[pifModeChangePara->index] = NULL;
        }
    }

    if (NULL == g_pstPPPoEThreadPara[pifModeChangePara->index])
    {
        g_pstPPPoEThreadPara[pifModeChangePara->index] = vosAlloc(sizeof(IF_MODE_VOIP_PARA_S));
    }

    vosMemCpy(g_pstPPPoEThreadPara[pifModeChangePara->index],
        pifModeChangePara, sizeof(IF_MODE_VOIP_PARA_S));

	g_pstPPPoEThreadId[pifModeChangePara->index] = vosThreadCreate("tPPPoECtrl", OP_VOS_THREAD_STKSZ, 60,
						(void *)odmPPPoEThread,
						(void *)g_pstPPPoEThreadPara[pifModeChangePara->index]);
	if (NULL == g_pstPPPoEThreadId[pifModeChangePara->index]) {
		OP_DEBUG(DEBUG_LEVEL_WARNING, "%s: tPPPoECtrl task create failed", __FUNCTION__);
		retVal = ERROR;
	}

    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: Created g_pstPPPoEThreadId[%d] = %X", 
            __FUNCTION__, pifModeChangePara->index, 
            g_pstPPPoEThreadId[pifModeChangePara->index]);

exit_label:
    vosMutexGive(&g_pastIpModeThreadMutex);
	return retVal;

}

STATUS odmPPPoEThreadStop(int index)
{
    char acCommand[128] = {0};
    STATUS retVal = OK;

    vosMutexTake(&g_pastIpModeThreadMutex);

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: enter", 
		__FUNCTION__);
	
	/* add by zttan */
	if (ERROR == vosThreadVerify(g_pstPPPoEThreadId[index]))
	{
		OP_DEBUG(DEBUG_LEVEL_WARNING, "%s: verify error g_pstPPPoEThreadId[%d] = %X", __FUNCTION__, index, g_pstPPPoEThreadId[index]);
        retVal = ERROR;
    }
    else {
        vosThreadDestroy(g_pstPPPoEThreadId[index]);

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: destroy g_pstPPPoEThreadId[%d] = %X", __FUNCTION__, index, g_pstPPPoEThreadId[index]);
        if (NULL != g_pstPPPoEThreadPara[index])
        {
            vosFree(g_pstPPPoEThreadPara[index]);
            g_pstPPPoEThreadPara[index] = NULL;
        }        
    }

    vosSprintf(acCommand, 
        "ps | grep -v grep | grep /etc/ppp/pppoe%d.conf | awk '{print $1}' | xargs kill -9 >&/dev/null", 
        index);
	vosSystem(acCommand);

    vosSprintf(acCommand, 
        "ps | grep -v grep | grep /var/run/pppoe%d | awk '{print $1}' | xargs kill -9 >&/dev/null", 
        index);
	vosSystem(acCommand);

    vosIPAddrSet((UINT8 *)g_acIfEthNameStr[index], "0.0.0.0");
    vosMemSet(g_acIfPPPoENameStr[index],0,32);

exit_label:	
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: exit", 
		__FUNCTION__);

	vosMutexGive(&g_pastIpModeThreadMutex);

    return retVal;
}


STATUS odmDHCPThreadStart(IF_MODE_VOIP_PARA_S  *pifModeChangePara)
{
    STATUS retVal = OK;

    vosMutexTake(&g_pastIpModeThreadMutex);

    if (NULL == pifModeChangePara)
    {
		OP_DEBUG(DEBUG_LEVEL_WARNING, "%s: NULL == pifModeChangePara", __FUNCTION__);
        retVal = ERROR;
        goto exit_label;
    }

	if (OK == vosThreadVerify(g_pastDHCPThreadId[pifModeChangePara->index])) {
        vosThreadDestroy(g_pastDHCPThreadId[pifModeChangePara->index]);

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: destroy g_pastDHCPThreadId[%d] = %X", 
            __FUNCTION__, pifModeChangePara->index, 
            g_pastDHCPThreadId[pifModeChangePara->index]);

        if (NULL != g_pastDHCPThreadPara[pifModeChangePara->index])
        {
            vosFree(g_pastDHCPThreadPara[pifModeChangePara->index]);
            g_pastDHCPThreadPara[pifModeChangePara->index] = NULL;
        }        
    }

    if (NULL == g_pastDHCPThreadPara[pifModeChangePara->index])
    {
        g_pastDHCPThreadPara[pifModeChangePara->index] = vosAlloc(sizeof(IF_MODE_VOIP_PARA_S));
    }

    vosMemCpy(g_pastDHCPThreadPara[pifModeChangePara->index],
        pifModeChangePara, sizeof(IF_MODE_VOIP_PARA_S));

	g_pastDHCPThreadId[pifModeChangePara->index] = vosThreadCreate("tDHCPCtrl", OP_VOS_THREAD_STKSZ, 150,
		(void *)odmDHCPThread, (void *)g_pastDHCPThreadPara[pifModeChangePara->index]);
	if (NULL == g_pastDHCPThreadId[pifModeChangePara->index]) {
		OP_DEBUG(DEBUG_LEVEL_WARNING, "%s: tDHCPCtrl task create failed", __FUNCTION__);
		retVal = ERROR;
	}

    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: Created g_pastDHCPThreadId[%d] = %X", 
            __FUNCTION__, pifModeChangePara->index, 
            g_pastDHCPThreadId[pifModeChangePara->index]);

exit_label:
	vosMutexGive(&g_pastIpModeThreadMutex);
	return retVal;
}

STATUS odmDHCPThreadStop(int index)
{
    char acCommand[128] = {0};
    STATUS retVal = OK;

    vosMutexTake(&g_pastIpModeThreadMutex);

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: enter", 
		__FUNCTION__);

    if (ERROR == vosThreadVerify(g_pastDHCPThreadId[index])) {
		OP_DEBUG(DEBUG_LEVEL_WARNING, "%s: verify error g_pastDHCPThreadId[%d] = %X",
            __FUNCTION__, index, g_pastDHCPThreadId[index]);
        retVal = ERROR;
    }
    else 
    {
        vosThreadDestroy(g_pastDHCPThreadId[index]);

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: destroy g_pastDHCPThreadId[%d] = %X", 
            __FUNCTION__, index, g_pastDHCPThreadId[index]);

        if (NULL != g_pastDHCPThreadPara[index])
        {
            vosFree(g_pastDHCPThreadPara[index]);
            g_pastDHCPThreadPara[index] = NULL;
        }
    }

    vosSprintf(acCommand, 
        "ps | grep -v grep | grep /var/udhcpc.%s.pid | awk '{print $1}' | xargs kill -9 >&/dev/null",
        g_acIfEthNameStr[index]);
    vosSystem(acCommand);

    vosIPAddrSet((UINT8 *)g_acIfEthNameStr[index], "0.0.0.0");

exit_label:
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: exit", 
		__FUNCTION__);

    vosMutexGive(&g_pastIpModeThreadMutex);

    return OK;
}


#if 0
/* for ip mode change, notify voip */
/*
Share or Admin

*/
void testPrintVoip( const VTP_CONFIG_NETWORK_T * pvtpConfigNetwork)
{
	UINT8 *pMac = (UINT8 *)pvtpConfigNetwork->local_mac;

	printf("\n\ntestPrintVoip:\n");

	printf("link_state: %d\n",pvtpConfigNetwork->link_state);
	printf("link_type: %d\n",pvtpConfigNetwork->link_type);
	printf("session_id: %d\n",pvtpConfigNetwork->session_id);
	printf("voip_vlan_type: %d\n",pvtpConfigNetwork->voip_vlan_type);
	printf("arp_enable: %d\n",pvtpConfigNetwork->arp_enable);
	printf("ifname: %s\n",pvtpConfigNetwork->ifname);
	printf("peer_mac: \n",pvtpConfigNetwork->peer_mac);
	printf("local_mac: %02x:%02x:%02x:%02x:%02x:%02x\n",pMac[0],pMac[1],pMac[2],pMac[3],pMac[4],pMac[5]);
	printf("ip: %s\n",pvtpConfigNetwork->ip);
	printf("mask: %s\n",pvtpConfigNetwork->mask);
	printf("gateway: %s\n",pvtpConfigNetwork->gateway);
	printf("dns1: %s\n",pvtpConfigNetwork->dns1);
	printf("dns2: %s\n",pvtpConfigNetwork->dns2);
	printf("dns3: %s\n",pvtpConfigNetwork->dns3);
	printf("qos_t: %d\n",pvtpConfigNetwork->qos_t);
	printf("vlan_enable: %d\n",pvtpConfigNetwork->vlan_enable);
	printf("vlan_id: %d\n",pvtpConfigNetwork->vlan_id);
	printf("vlan_cfi: %d\n",pvtpConfigNetwork->vlan_cfi);
	printf("vlan_priority: %d\n\n",pvtpConfigNetwork->vlan_priority);

}
void testPrint(char *state, char *res_type, char *link_type, char *session_id,
	char *mac_type, char *peer_mac_addr, char *local_mac, char *ip, char *netmask, char *gateway,
	char *dns1, char *dns2, char *qos, char *qos_value, char *vlan, char *vid, char *vlan_priority,
	char *ip_device)
{
	printf(":testPrint: \n");
	UINT8 *mac = NULL;

	if ( state != NULL ){
		printf("state(active/inactive) flag: %s\n",state);
	}else{
		printf("state(active/inactive) flag: NULL\n",state);
	}

	if ( res_type != NULL ){
		printf("res_type(demia) mode: %s\n",res_type);
	}else{
		printf("res_type(demia) mode: NULL\n",res_type);
	}

	if ( link_type != NULL ){
		printf("link_type flag: %s\n",link_type);
	}else{
		printf("link_type flag: NULL\n",link_type);
	}

	if ( session_id != NULL ){
		printf("session_id flag: %s\n",session_id);
	}else{
		printf("session_id flag: NULL\n",session_id);
	}

	if ( mac_type != NULL ){
		printf("mac_type(ARP) flag: %s\n",mac_type);
	}else{
		printf("mac_type flag: NULL\n",mac_type);
	}

	if ( peer_mac_addr != NULL ){
		printf("peer_mac_addr flag: %s\n",peer_mac_addr);
	}else{
		printf("peer_mac_addr flag: NULL\n",peer_mac_addr);
	}

	if ( local_mac != NULL ){
		mac = (UINT8 *)local_mac;
		printf("local_mac flag: %02x:%02x:%02x:%02x:%02x:%02x\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	}else{
		printf("local_mac flag: NULL\n",local_mac);
	}

	if ( ip != NULL ){
		printf("ip flag: %s\n",ip);
	}else{
		printf("ip flag: NULL\n",ip);
	}

	if ( netmask != NULL ){
		printf("netmask flag: %s\n",netmask);
	}else{
		printf("netmask flag: NULL\n",netmask);
	}

	if ( gateway != NULL ){
		printf("gateway flag: %s\n",gateway);
	}else{
		printf("gateway flag: NULL\n",gateway);
	}

	if ( dns1 != NULL ){
		printf("dns1 flag: %s\n",dns1);
	}else{
		printf("dns1 flag: NULL\n",dns1);
	}

	if ( dns2!= NULL ){
		printf("dns2 flag: %s\n",dns2);
	}else{
		printf("dns2 flag: NULL\n",dns2);
	}

	if ( qos != NULL ){
		printf("qos flag: %s\n",qos);
	}else{
		printf("qos flag: NULL\n",qos);
	}

	if ( qos_value != NULL ){
		printf("qos_value flag: %s\n",qos_value);
	}else{
		printf("qos_value flag: NULL\n",qos_value);
	}

	if ( vlan != NULL ){
		printf("vlan name flag: %s\n",*vlan);
	}else{
		printf("vlan name flag: NULL\n",*vlan);
	}

	if ( vid != NULL ){
		printf("vid flag: %d\n",vid);
	}else{
		printf("vid flag: NULL\n",vid);
	}

	if ( vlan_priority != NULL ){
		printf("vlan_priority flag: %s\n",vlan_priority);
	}else{
		printf("vlan_priority flag: NULL\n",vlan_priority);
	}

	if ( ip_device != NULL ){
		printf("ip_device flag: %s\n",ip_device);
	}else{
		printf("ip_device flag: NULL\n",ip_device);
	}

}
#endif

/*

activeFlag:0:INACTIVE; 1:ACTIVE
modeFlag:1:static->dhcp, 2:static->pppoe, 3:dhcp->pppoe, 4:pppoe->dhcp

*/
int ipModeChangeNotifyVoip(enum net_state activeFlag, UINT8 modeFlag, UINT8 ucIf, UINT8 ucIfMode, IF_MODE_VOIP_PARA_S *pIfModeVoipPara)
{
#if 0
    UINT8 index = IF_MODE_TO_INDEX(ucIf,ucIfMode);
    UINT8 ucIfShareMode;
    UINT8 macAddr_str[6] = {0};
    char ipAddr[STR_SIZE_32_BYTES] = {0};
    char acIpMask[STR_SIZE_32_BYTES] = {0};
    char acIpGw[STR_SIZE_32_BYTES] = {0};
    char dnsIpAddr[STR_SIZE_32_BYTES] = {0};
    UINT32 newIp = 0;
    UINT32 dnsIp;
    UINT8 dnsIpStr[20] = {0};
    UINT16 vlanId;
    const char *ifModeStr[INTERFACE_SHARE_MODE_MAX+INTERFACE_MODE_MAX] = {"Share","Ind","Admin","Signal","Media"};
    UINT8 *state[2] = {"INACTIVE","ACTIVE"};
    UINT8 ethType[IP_MODE_CHANGE_TYPE_MAX]  = {0};
    UINT8 qosMode = 0;
    UINT8 qosValue = 0;
	UINT8 ucIfIpMode = 0;

    ethType[IP_MODE_STATIC_TO_PPPOE] = LINK_PPPOE;
    ethType[IP_MODE_DHCP_TO_PPPOE] = LINK_PPPOE;

    /* skip admin interface */
    if (ucIfMode == INTERFACE_MODE_ADMIN)
    {
        return OK;
    }

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\r\n ipModeChangeNotifyVoip ++++++++++++++++ ");

    UINT32 i = 0;
    VTP_CONFIG_NETWORK_T vtpConfigNetwork;
    odmSysCfgIfIpAddrGet(ucIf,ucIfMode, &newIp);

	#if 0
    if ( activeFlag == NET_DOWN || newIp == 0 ){/* NOTE: requirement changed, when inactive, don't notify voip */
        return 0;
    }
	#endif


    if ( activeFlag != NET_DOWN )
    {
	    if (pIfModeVoipPara->modeChangeType == IP_MODE_STAY_SAME){
		if ( newIp == pIfModeVoipPara->oldIp && pIfModeVoipPara->shareModeChanged == 0 ){
	            return 0;
	        }
	    }
    }

    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, 
        "Note: ip mode changed: old ip %x, new ip %x",
        pIfModeVoipPara->oldIp,newIp);

    odmSysCfgIfShareModeGet(ucIf,ucIfMode, &ucIfShareMode);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, 
        "activeFlag: %d, modeFlag: %d, notify voip, ucIf: %d, ucIfMode: %d, ucIfShareMode: %d, index: %d",
        activeFlag, modeFlag,ucIf,ucIfMode, ucIfShareMode,index);
    odmSysCfgIfIpAddrGetS(ucIf,ucIfMode, ipAddr);

    odmSysCfgIfIpMaskGetS(ucIf,ucIfMode,acIpMask);
    odmSysCfgIfIpGatewayGetS(ucIf,ucIfMode,acIpGw);

    odmSysCfgIfIpDnsGet(ucIf,ucIfMode,&dnsIp);
    cliIpToStr(dnsIp,dnsIpAddr);

    odmSysIfMacAddrGet(ucIf,ucIfMode,macAddr_str);
    odmSysCfgIfCVLANGet(ucIf,ucIfMode,&vlanId);

    odmSysCfgIfQoSModeGet(ucIf,ucIfMode,&qosMode);
    odmSysCfgIfQoSValGet(ucIf,ucIfMode,&qosValue);

	odmSysCfgIfIpModeGet(ucIf,ucIfMode, &ucIfIpMode);
	
    vtpConfigNetwork.link_state = activeFlag;
    vtpConfigNetwork.link_type = ethType[modeFlag];
    vtpConfigNetwork.session_id = 0;

    if ( ucIfShareMode != INTERFACE_SHARE_MODE_SHARE &&
        ucIfMode == INTERFACE_MODE_ADMIN)
    {
        vtpConfigNetwork.voip_vlan_type = RES_MANAGER;
    }
    else if ( ucIfShareMode != INTERFACE_SHARE_MODE_SHARE &&
        ucIfMode == INTERFACE_MODE_SIGNAL)
    {
        vtpConfigNetwork.voip_vlan_type = RES_SIGNAL;
    }
    else if ( ucIfShareMode != INTERFACE_SHARE_MODE_SHARE &&
        ucIfMode == INTERFACE_MODE_MEDIA)
    {
        vtpConfigNetwork.voip_vlan_type = RES_MEDIA;
    }
    else  {
        vtpConfigNetwork.voip_vlan_type = RES_SHARE;
    }

    vtpConfigNetwork.arp_enable = 1;
	if(ucIfIpMode == IF_IP_MODE_PPPOE)
	{
        vosStrCpy(vtpConfigNetwork.ifname, g_acIfPPPoENameStr[index]);
	}
	else
	{
    		vosStrCpy(vtpConfigNetwork.ifname, g_acIfEthNameStr[index]);
	}
	
	vtpConfigNetwork.peer_mac[0] = 0;
    memcpy(vtpConfigNetwork.local_mac, macAddr_str,6);
    vosStrCpy(vtpConfigNetwork.ip,ipAddr);
    vosStrCpy(vtpConfigNetwork.mask, acIpMask);
    vosStrCpy(vtpConfigNetwork.gateway,acIpGw);
    vosStrCpy(vtpConfigNetwork.dns1, dnsIpAddr);
    vtpConfigNetwork.dns2[0] = 0;
    vtpConfigNetwork.dns3[0] = 0;

    vtpConfigNetwork.qos_t = qosMode;
    vtpConfigNetwork.qos_val = qosValue;

    vtpConfigNetwork.vlan_enable = VLAN_DIS;
    vtpConfigNetwork.vlan_id = vlanId;
    vtpConfigNetwork.vlan_cfi = 0;
    vtpConfigNetwork.vlan_priority = 0;

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERROR;
    }

    if ( CfmAgent_Init("127.0.0.1",0) < 0 ){
		printf("error: CfmAgent_Init\n");
	    return ERROR;
    };
 
	CfmAgent_UpdateNwkCfg( (const VTP_CONFIG_NETWORK_T *) &vtpConfigNetwork);

	CfmAgent_Exit();
#endif

    return OK;
}

/* ip mode changed, notify voip */

int odmSysStaticIpSet(UINT8 ucIf, UINT8 ucIfMode, IF_MODE_VOIP_PARA_S *pIfModeChangePara)
{
	int pid = 0;
	char acPidFile[64] = {0};
    UINT8 index = IF_MODE_TO_INDEX(ucIf,ucIfMode);
    UINT8 ucIfIpMode;
    char ipAddr[STR_SIZE_32_BYTES] = {0};

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERROR;
    }

    odmSysCfgIfIpModeGet(ucIf, ucIfMode, &ucIfIpMode);

    if (IF_IP_MODE_STATIC == ucIfIpMode)
    {
    	odmSysCfgIfIpAddrGetS(ucIf,ucIfMode, ipAddr);
        if ( vosStrCmp(ipAddr,"0.0.0.0") != 0 ){
            ipModeChangeNotifyVoip(NET_UP, pIfModeChangePara->modeChangeType, ucIf, ucIfMode,pIfModeChangePara);
        }
    }

}


STATUS odmSysDHCPSet(UINT8 ucIf, UINT8 ucIfMode, IF_MODE_VOIP_PARA_S *pIfModeVoipPara)
{
    UINT8 index = IF_MODE_TO_INDEX(ucIf,ucIfMode);
    UINT8 ucIfIpMode;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: enter", __FUNCTION__);

    if (IS_INVALID_IF_INDEX(index))
    {
        return ERROR;
    }

    odmSysCfgIfIpModeGet(ucIf, ucIfMode, &ucIfIpMode);

    if (IF_IP_MODE_DHCP == ucIfIpMode)
    {
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: stop dhcp thread", __FUNCTION__);
        odmDHCPThreadStop(index);
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: start dhcp thread", __FUNCTION__);
        odmDHCPThreadStart(pIfModeVoipPara);
    }
    else {
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: stop dhcp thread", __FUNCTION__);
        odmDHCPThreadStop(index);
    }

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: exit", __FUNCTION__);

    return OK;

}
/* ip mode changed, notify voip */

STATUS odmSysPPPoESet(UINT8 ucIf, UINT8 ucIfMode, IF_MODE_VOIP_PARA_S *pIfModeVoipPara)
{
	STATUS ret;
	int pid;
    UINT8 ucIfIpMode, ucIfPPPoEMode;
    char userName[34] = {0};
    char passWord[34] = {0};

	FILE *fp_config;
	char *tmp = NULL;
	char acFileName[32] = {0};
    char acCommand[128] = {0};

    VOS_DIR_t * pDir;

    UINT8 index = IF_MODE_TO_INDEX(ucIf, ucIfMode);

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: enter", __FUNCTION__);
    if (IS_INVALID_IF_INDEX(index))
    {
		OP_DEBUG(DEBUG_LEVEL_WARNING, "%s: IS_INVALID_IF_INDEX(%d)", __FUNCTION__, index);
        return ERR_OUT_OF_RANGE;
    }

    vosMemSet(g_acIfPPPoENameStr[index],0,32);
    odmSysCfgIfIpModeGet(ucIf, ucIfMode, &ucIfIpMode);
    odmSysCfgIfPPPoEModeGet(ucIf, ucIfMode, &ucIfPPPoEMode);

    if (IF_IP_MODE_PPPOE == ucIfIpMode)
    {
        pDir = vosOpenDir("/etc/ppp");
        if (pDir == NULL)
        {
            vosMakeDir("/etc/ppp");
        }
        else
        {
            vosCloseDir(pDir);
        }

        odmSysCfgIfPPPoEUserGet(ucIf, ucIfMode, userName);
    	odmSysCfgIfPPPoEPassGet(ucIf, ucIfMode, passWord);

    	vosSprintf(acFileName, "/etc/ppp/pppoe%d.conf", index);
    	fp_config = vosFOpen(acFileName,"w");
    	if ( fp_config == NULL ){
    		 return ERROR;
    	}
    	fprintf(fp_config,"ETH='%s'\n", g_acIfEthNameStr[index]);
    	fprintf(fp_config,"USER='%s'\n", userName);

    	/* recover the orignal config */
    	fprintf(fp_config,"DEMAND=no\n");
    	fprintf(fp_config,"DNSTYPE=SERVER\n");
    	fprintf(fp_config,"PEERDNS=yes\n");
    	fprintf(fp_config,"DNS1=\n");
    	fprintf(fp_config,"DNS2=\n");
    	fprintf(fp_config,"DEFAULTROUTE=yes\n");
    	fprintf(fp_config,"CONNECT_TIMEOUT=30\n");
    	fprintf(fp_config,"CONNECT_POLL=2\n");
    	fprintf(fp_config,"ACNAME=\n");
    	fprintf(fp_config,"SERVICENAME=\n");
    	fprintf(fp_config,"PING=\".\"\n");
    	fprintf(fp_config,"CF_BASE=pppoe%d\n", index);
    	fprintf(fp_config,"PIDFILE=\"/var/run/pppoe%d.pid\"\n", index);
    	fprintf(fp_config,"SYNCHRONOUS=no\n");
    	fprintf(fp_config,"CLAMPMSS=1412\n");
    	fprintf(fp_config,"LCP_INTERVAL=20\n");
    	fprintf(fp_config,"LCP_FAILURE=4\n");
    	fprintf(fp_config,"PPPOE_TIMEOUT=80\n");
    	fprintf(fp_config,"FIREWALL=NONE\n");
    	fprintf(fp_config,"LINUX_PLUGIN=\n");
    	fprintf(fp_config,"PPPOE_EXTRA=\"\"\n");
    	fprintf(fp_config,"PPPD_EXTRA=\"\"\n");
    	fprintf(fp_config,"RETRY_ON_FAILURE=yes\n");

    	fclose(fp_config);

    	switch (ucIfPPPoEMode){
    		case PPPOE_AUTH_TYPE_PAP:

                sprintf(acCommand, "sed -e '/%s\t/d' /etc/ppp/pap-secrets > /etc/ppp/pap-secrets.new", userName);
    			vosSystem(acCommand);
                sprintf(acCommand, "awk 'BEGIN{ printf(\"%s\\t*\\t%s\\t*\\n\") >> \"/etc/ppp/pap-secrets.new\";}'", userName,passWord);
    			vosSystem(acCommand);
    			vosSystem("mv /etc/ppp/pap-secrets.new /etc/ppp/pap-secrets");

                break;

            case PPPOE_AUTH_TYPE_CHAP:

                sprintf(acCommand, "sed -e '/%s\t/d' /etc/ppp/chap-secrets > /etc/ppp/chap-secrets.new", userName);
    			vosSystem(acCommand);
                sprintf(acCommand, "awk 'BEGIN{ printf(\"%s\\t*\\t%s\\t*\\n\") >> \"/etc/ppp/chap-secrets.new\";}'", userName,passWord);
    			vosSystem(acCommand);
    			vosSystem("mv /etc/ppp/chap-secrets.new /etc/ppp/chap-secrets");

    			break;
    		default:

                sprintf(acCommand, "sed -e '/%s\t/d' /etc/ppp/pap-secrets > /etc/ppp/pap-secrets.new", userName);
    			vosSystem(acCommand);
                sprintf(acCommand, "awk 'BEGIN{ printf(\"%s\\t*\\t%s\\t*\\n\") >> \"/etc/ppp/pap-secrets.new\";}'", userName,passWord);
    			//printf(acCommand);printf("\r\n");
    			vosSystem(acCommand);
    			vosSystem("mv /etc/ppp/pap-secrets.new /etc/ppp/pap-secrets");

                sprintf(acCommand, "sed -e '/%s\t/d' /etc/ppp/chap-secrets > /etc/ppp/chap-secrets.new", userName);
    			vosSystem(acCommand);
                sprintf(acCommand, "awk 'BEGIN{ printf(\"%s\\t*\\t%s\\t*\\n\") >> \"/etc/ppp/chap-secrets.new\";}'", userName,passWord);
    			//printf(acCommand);printf("\r\n");
    			vosSystem(acCommand);
    			vosSystem("mv /etc/ppp/chap-secrets.new /etc/ppp/chap-secrets");
    			break;
    	}

        odmPPPoEThreadStop(index);
    	ret = odmPPPoEThreadStart(pIfModeVoipPara);
    }
    else 
    {
	    ret = odmPPPoEThreadStop(index);
    }

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s: exit", __FUNCTION__);

	return ret;
}

/* OUTPUT format: YYYYMMDDHHMMSS */
UINT32 odmSysSoftwareBuildTimeGet(char * pcDateTime)
{
    char * pacMonth[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    int nYear, nMonth, nDay;
    char acMonth[5] = {0};
    int nHour, nMinute, nSecond;

    if (NULL == pcDateTime)
    {
        return ERR_NULL_POINTER;
    }

    sscanf(build_date, "%s %d %d", acMonth, &nDay, &nYear);
    sscanf(build_time, "%d:%d:%d", &nHour, &nMinute, &nSecond);

    for (nMonth = 0; nMonth < 12; nMonth++)
    {
        if (0 == vosStrCmp(pacMonth[nMonth], acMonth))
        {
            break;
        }
    }

    vosSprintf(pcDateTime, "%04d%02d%02d%02d%02d%02d",
        nYear, nMonth+1, nDay, nHour, nMinute, nSecond);

    return NO_ERROR;
}

UINT32 odmSysCPUPathMtuGet(UINT32 *cpuMtu)
{
    *cpuMtu = vosConfigUInt32Get(SYSTEM_CFG,
        SYSTEM_CFG_WEB_SEC,SYSTEM_CFG_CPU_MTU,
        DEFAULT_CPU_MTU);
    return NO_ERROR;
}

UINT32 odmSysCPUPathMtuSet(UINT32 cpuMtu)
{
    UINT8   IfSectionBuff[32] = {0};
    UINT32  ret = OK;
    UINT32  regVal;

    if( 68 >= cpuMtu)         /*bug3129: only 68~1500 is valid mtu size for linux os.*/
        cpuMtu = 68;
    else 
        if(HOSTPATH_MTU <= cpuMtu)
            cpuMtu = HOSTPATH_MTU;

#if defined(INTERFACE_THREE_WAN_THREE_LAN)
    ret += vosCpuMtuSet("eth0",cpuMtu);
    ret += vosCpuMtuSet("eth1",cpuMtu);
    ret += vosCpuMtuSet("eth2",cpuMtu);
    ret += vosCpuMtuSet("eth3",cpuMtu);
    ret += vosCpuMtuSet("eth4",cpuMtu);
    ret += vosCpuMtuSet("eth5",cpuMtu);
#elif defined(INTERFACE_TWO_WAN_TWO_LAN)
    ret += vosCpuMtuSet("eth0",cpuMtu);
    ret += vosCpuMtuSet("eth1",cpuMtu);
    ret += vosCpuMtuSet("eth2",cpuMtu);
    ret += vosCpuMtuSet("eth3",cpuMtu);
#elif defined(INTERFACE_ONE_WAN_ONE_LAN)
    ret += vosCpuMtuSet("eth0",cpuMtu);
    ret += vosCpuMtuSet("eth1",cpuMtu);
#endif

#if 0  /* move this setting to FEMAC driver */
	/* added 4 bytes for atheros header */
    ret += oplRegWrite(REG_FE_MAXFRAMLEN,cpuMtu+4);
	if(NO_ERROR != ret)
	{
    	return ret;
    }
#endif

    vosConfigUInt32Set(SYSTEM_CFG,
        SYSTEM_CFG_WEB_SEC,SYSTEM_CFG_CPU_MTU, cpuMtu);
    return NO_ERROR;
}

UINT8 odmSysVoIPPortBaseGet()
{
    return (UINT8)vosConfigUInt32Get(SYSTEM_CFG,
        SYSTEM_CFG_GENERAL_SEC, SYSTEM_CFG_VOIP_PORT_BASE,
        DEFAULT_VOIP_PORT_BASE);
}

UINT32 odmSysVoIPPortBaseSet(UINT8 ucBase)
{
    return (UINT32)vosConfigUInt32Set(SYSTEM_CFG,SYSTEM_CFG_GENERAL_SEC,
        SYSTEM_CFG_VOIP_PORT_BASE, (UINT32)ucBase);
}


