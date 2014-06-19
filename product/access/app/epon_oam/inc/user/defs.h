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
* FILENAME:  defs.h
*
* DESCRIPTION: 
*	
*
* Date Created: Apr 22, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/h/defs.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __DEFS_H_
#define __DEFS_H_

//#include <vos.h>
#include "errors.h"

#ifndef STR_SIZE_16_BYTES
#define STR_SIZE_16_BYTES           16
#endif

#ifndef STR_SIZE_32_BYTES
#define STR_SIZE_32_BYTES           32
#endif

#ifndef STR_SIZE_64_BYTES
#define STR_SIZE_64_BYTES           64
#endif

#ifndef STR_SIZE_128_BYTES
#define STR_SIZE_128_BYTES          128
#endif

#ifndef STR_SIZE_256_BYTES
#define STR_SIZE_256_BYTES          256
#endif

#ifndef MAX_USER_NUM
#define MAX_USER_NUM                256
#endif

#ifndef ENABLED
#define ENABLED                     1
#endif

#ifndef DISABLED
#define DISABLED                    0
#endif

#define USER_NAME_MAX_SIZE          40
#define USER_PASSWD_MAX_SIZE        40

#ifndef TIME_STR_LEN
#define TIME_STR_LEN            20
#endif

#ifndef MAX_HISTORY
#define MAX_HISTORY             31
#endif

#define STATE_DISABLE_STR       "Disable"
#define STATE_ENABLE_STR        "Enable"

#define STATE_STR_GET(state)    ((ENABLED == (state)) ? STATE_ENABLE_STR : STATE_DISABLE_STR)

#define DEFAULT_TELNET_PORT     2323
#define DEFAULT_WEB_PORT        80
#define DEFAULT_SESSION_NUM     5
#define DEFAULT_SESSION_TIMEOUT 180
#define DEFAULT_USER_PORT_NUM   24
#define DEFAULT_LAN_ADMIN_IP    "192.168.1.1"
#define DEFAULT_LAN_ADMIN_MASK  "255.255.255.0"

#define DEFAULT_HOSTNAME        "opulan"

#define DEFAULT_CONFIG_FILE     "/cfg/startup-config"
#define DEFAULT_CONFIG_FILE_SIZ 524288
#define MAX_SESSION_NUM         32

#define DEFAULT_TEMP_PATH           "/tmp"
#define DEFAULT_TEMP_MSQ_PATH       "/tmp/msq%08x"
#define SLASH_STR                   "/"

#define DEFAULT_LOG_FILE_NAME       "onu.log"
#define DEFAULT_BAK_LOG_FILE_NAME   "onu.bak.log"

#define DEFAULT_ALARM_FILE_NAME       "onu.alarm"
#define DEFAULT_BAK_ALARM_FILE_NAME   "onu.bak.alarm"

#define DEFAULT_SYSLOG_SERVER_FILE_NAME       "onu.syslog"
#define DEFAULT_SYSLOG_SERVER_BAK_FILE_NAME       "onu.syslog.bak"

#pragma pack(1)		/* pragma pack definition */

#define IH_MAGIC	0x27051956	/* Image Magic Number		*/
#define IH_NMLEN		32	/* Image Name Length		*/

/*
 * all data in network byte order (aka natural aka bigendian)
 */

typedef struct image_header {
	UINT32	ih_magic;	/* Image Header Magic Number	*/
	UINT32	ih_hcrc;	/* Image Header CRC Checksum	*/
	UINT32	ih_time;	/* Image Creation Timestamp	*/
	UINT32	ih_size;	/* Image Data Size		*/
	UINT32	ih_load;	/* Data	 Load  Address		*/
	UINT32	ih_ep;		/* Entry Point Address		*/
	UINT32	ih_dcrc;	/* Image Data CRC Checksum	*/
	UINT8	ih_os;		/* Operating System		*/
	UINT8	ih_arch;	/* CPU architecture		*/
	UINT8	ih_type;	/* Image Type			*/
	UINT8	ih_comp;	/* Compression Type		*/
	UINT8	ih_name[IH_NMLEN];	/* Image Name		*/
#ifdef OPULAN_IMAGE_HEADER
	UINT32   ih_chipid;  /*Opulan chip Id*/
	UINT32   ih_revnum;  /*SVN revsion number*/
	UINT32   ih_flags;  /* reservd now*/
#endif
} image_header_t;


/*********************************************************
* enum definition start
*/

typedef enum MODULE_s
{
    MOD_DRIVERS = 0,
    MOD_SYS = 1,
    MOD_USER = 2,
    MOD_QOS = 3,
    MOD_FDB = 4,
    MOD_MCAST = 5,
    MOD_VLAN = 6,
    MOD_PORT = 7,
    MOD_RSTP = 8,
    MOD_PON = 9,
    MOD_LOG = 10,
    MOD_STORM = 11,
    MOD_OAM,
    MOD_STATS,
    ALL_MODULE,
    MAX_MODULE
}MODULE_e;
/*
typedef enum DEBUG_LEVEL_s
{
    DEBUG_LEVEL_OFF,
    DEBUG_LEVEL_CRITICAL,
    DEBUG_LEVEL_WARNING,
    DEBUG_LEVEL_INFO,
    DEBUG_LEVEL_DEBUGGING,
    DEBUG_LEVEL_MAX
}DEBUG_LEVEL_e;
*/
typedef enum ACCESS_LEVEL_s
{
    ACCESS_LEVEL_GUEST,
    ACCESS_LEVEL_USER,
    ACCESS_LEVEL_ADMINISTRATION,
    ACCESS_LEVEL_SUPER,
    ACCESS_LEVEL_INVALID
}ACCESS_LEVEL_e;

typedef enum ACCESS_TYPE_s
{
    ACCESS_TYPE_CONSOLE,
    ACCESS_TYPE_TELNET,
    ACCESS_TYPE_WEB,
    ACCESS_TYPE_SNMP,
    ACCESS_TYPE_INVALID
}ACCESS_TYPE_e;


typedef enum LANGUAGE_s
{
    ENGLISH,
    CHINESE_SIMPLIFIED,
    CHINESE_TRADITIONAL,
    LANGUAGE_MAX
} LANGUAGE_e;

typedef enum INTERFACE_s
{
    INTERFACE_LAN,
    INTERFACE_WAN,
    INTERFACE_MAX
}INTERFACE_e;

//#define INTERFACE_ONE_WAN_ONE_LAN
#define INTERFACE_THREE_WAN_THREE_LAN

#if defined(INTERFACE_THREE_WAN_THREE_LAN)

typedef enum INTERFACE_MODE_s
{
    INTERFACE_MODE_ADMIN,
	INTERFACE_MODE_SIGNAL,
    INTERFACE_MODE_MEDIA,
    INTERFACE_MODE_MAX
}INTERFACE_MODE_e;

#elif defined(INTERFACE_TWO_WAN_TWO_LAN)

typedef enum INTERFACE_MODE_s
{
    INTERFACE_MODE_ADMIN,
	INTERFACE_MODE_SIGNAL,
    INTERFACE_MODE_MAX
}INTERFACE_MODE_e;

#elif defined(INTERFACE_ONE_WAN_ONE_LAN)

typedef enum INTERFACE_MODE_s
{
    INTERFACE_MODE_ADMIN,
    INTERFACE_MODE_MAX
}INTERFACE_MODE_e;

#endif

typedef enum INTERFACE_SHARE_MODE_s
{
    INTERFACE_SHARE_MODE_SHARE,
    INTERFACE_SHARE_MODE_IND, /* independent */
    INTERFACE_SHARE_MODE_MAX
}INTERFACE_SHARE_MODE_e;

typedef enum IF_IP_MODE_s
{
    IF_IP_MODE_STATIC,
    IF_IP_MODE_DHCP,
    IF_IP_MODE_PPPOE,
    IF_IP_MODE_MAX
}IF_IP_MODE_e;

typedef enum PPPOE_AUTH_TYPE_s
{
    PPPOE_AUTH_TYPE_AUTO,
    PPPOE_AUTH_TYPE_CHAP,
    PPPOE_AUTH_TYPE_PAP,
    PPPOE_AUTH_TYPE_MAX
}PPPOE_AUTH_TYPE_e;

typedef enum IF_QOS_MODE_s
{
    IF_QOS_MODE_DSCP,
    IF_QOS_MODE_TOS,
    IF_QOS_MODE_MAX
}IF_QOS_MODE_e;

typedef enum IF_TAG_MODE_s
{
    IF_TAG_MODE_TRANSPARENT,
    IF_TAG_MODE_TAG,
    IF_TAG_MODE_VLAN_STACKING,
    IF_TAG_MODE_MAX
}IF_TAG_MODE_e;

typedef UINT32 (* OnChangedCallBack)(UINT32 ulArg1, UINT32 ulArg2, UINT32 ulArg3, UINT32 ulArg4);

#ifndef BUFSIZ
#define BUFSIZ 4096
#endif

#ifndef MAC_LENGTH
#define MAC_LENGTH 6
#endif

typedef enum VOIP_PROTOCOL_TYPE_s{
    VOIP_PROTOCOL_H248,
    VOIP_PROTOCOL_SIP
}VOIP_PROTOCOL_TYPE_e;
typedef enum VOIP_H248_ACTIVE_MGC_s{
    VOIP_H248_ACTIVE_MGC_ALTERNATE,
    VOIP_H248_ACTIVE_MGC_PRIMARY
}VOIP_H248_ACTIVE_MGC_e;
typedef enum VOIP_H248_REG_MODE_s{
    VOIP_H248_REG_MODE_IP,
    VOIP_H248_REG_MODE_DOMAIN,
    VOIP_H248_REG_MODE_DEVICENAME
}VOIP_H248_REG_MODE_e;

typedef enum VOIP_H248_RTP_TID_MODE_s{
    VOIP_H248_RTP_TID_MODE_ALIGNMENT,
    VOIP_H248_RTP_TID_MODE_UNALIGNMENT
}VOIP_H248_RTP_TID_MODE_e;

typedef enum VOIP_FAX_T38_ENABLE_s{
    VOIP_FAX_T30,
    VOIP_FAX_T38
}VOIP_FAX_T38_ENABLE_e;

typedef enum VOIP_FAX_CTRL_s{
    VOIP_FAX_CTRL_NEGOTIATE,
    VOIP_FAX_CTRL_AutoVBD
}VOIP_FAX_CTRL_e;
typedef enum VOIP_IAD_H248_OPER_STATUS_s{
    VOIP_IAD_H248_OPER_STATUS_REGISTERING,
    VOIP_IAD_H248_OPER_STATUS_REGISTERED,
    VOIP_IAD_H248_OPER_STATUS_FAULT,
    VOIP_IAD_H248_OPER_STATUS_UNREGISTERED,
    VOIP_IAD_H248_OPER_STATUS_RESETING
}VOIP_IAD_H248_OPER_STATUS_s;

typedef enum VOIP_IAD_PORT_STATUS_s{
    VOIP_IAD_PORT_STATUS_REGISTERING,
    VOIP_IAD_PORT_STATUS_IDLE,
    VOIP_IAD_PORT_STATUS_HOOKOFF,
    VOIP_IAD_PORT_STATUS_DIALING,
    VOIP_IAD_PORT_STATUS_INRINGING,
    VOIP_IAD_PORT_STATUS_OUTRINGING,
    VOIP_IAD_PORT_STATUS_CONNECTING,
    VOIP_IAD_PORT_STATUS_CONNECTED,
    VOIP_IAD_PORT_STATUS_RELEASED,
    VOIP_IAD_PORT_STATUS_REGISTER_FAILED,
    VOIP_IAD_PORT_STATUS_INACTIVE
}VOIP_IAD_PORT_STATUS_e;

typedef enum VOIP_IAD_PORT_SERVICESTATE_s{
    VOIP_IAD_PORT_SERVICESTATE_END_LOCAL,
    VOIP_IAD_PORT_SERVICESTATE_END_REMOTE,
    VOIP_IAD_PORT_SERVICESTATE_END_AUTO,
    VOIP_IAD_PORT_SERVICESTATE_NORMAL
}VOIP_IAD_PORT_SERVICESTATE_e;

typedef enum VOIP_IAD_PORT_CODECMODE_s{
    VOIP_IAD_PORT_CODECMODE_G711A,
    VOIP_IAD_PORT_CODECMODE_G729,
    VOIP_IAD_PORT_CODECMODE_G711U,
    VOIP_IAD_PORT_CODECMODE_G723,
    VOIP_IAD_PORT_CODECMODE_G726,
    VOIP_IAD_PORT_CODECMODE_T38
}VOIP_IAD_PORT_CODECMODE_e;

typedef enum VOIP_IAD_OPERATION_s{
    VOIP_IAD_OPERATION_REREG,
    VOIP_IAD_OPERATION_DEREG,
    VOIP_IAD_OPERATION_RESET
}VOIP_IAD_OPERATION_e;
#define VOIP_POTS_NUM 2
#define VOIP_H248_DEF_HEARTBEAT_COUNT 3
#define VOIP_H248_DEF_RTP_TID_NUM 2
#define VOIP_H248_DEF_RTP_TID_DIGIT_LEN 0

#define VOIP_IADInfor_MAC "MAC Address"
#define VOIP_IADInfor_ProtocolSupported "ProtocolSupported"
#define VOIP_IADInfor_SwVersion "Iad Software Version"
#define VOIP_IADInfor_SwTime "Iad software time"
#define VOIP_IADInfor_PotsNum   "VoipUserCount"

#define VOIP_ProtocolInfo_ProtocolCfg "Protocol Config"

#define VOIP_GlobalParameterConfig_Share "ShareMode"
#define VOIP_GlobalParameterConfig_IpMode "Voice ip Mode"
#define VOIP_GlobalParameterConfig_IpAddr "iadIpAddr"
#define VOIP_GlobalParameterConfig_NetMask "iadNetMask"
#define VOIP_GlobalParameterConfig_GW "iadDefaultGW"
#define VOIP_GlobalParameterConfig_DNS "iadDnsServer"
#define VOIP_GlobalParameterConfig_PPPoEMode "PPPoE Mode"
#define VOIP_GlobalParameterConfig_PPPoEUser "PPPOE username"
#define VOIP_GlobalParameterConfig_PPPoEPass "PPPoE password"
#define VOIP_GlobalParameterConfig_TagMode "Tagged flag"
#define VOIP_GlobalParameterConfig_CVlan "Voice CVlan"
#define VOIP_GlobalParameterConfig_SVLAN "Voice SVLAN"
#define VOIP_GlobalParameterConfig_PRI "Voice Priority"

#define VOIP_H248ParamCfg_MGPortNo "H_RGW_PORT"
#define VOIP_H248ParamCfg_MGIP "H_MGC1_ADDR"
#define VOIP_H248ParamCfg_MgcComPortNo "H_MGC1_PORT"
#define VOIP_H248ParamCfg_BackupMgcIp "H_MGC2_ADDR"
#define VOIP_H248ParamCfg_BackupMgcComPortNo "H_MGC2_PORT"
#define VOIP_H248ParamCfg_ActiveMGC "ActiveMGC"
#define VOIP_H248ParamCfg_RegMode "RegMode"
#define VOIP_H248ParamCfg_MID "H_RGW_NAME"
#define VOIP_H248ParamCfg_HeartbeatMode "H_HEARTBEAT"
#define VOIP_H248ParamCfg_HeartbeatCycle "H_MIT"
#define VOIP_H248ParamCfg_HeartbeatCount "HeartbeatCount"

#define VOIP_H248UserTIDInfo_UserTIDNamePrefix "H_PHY_TERM_PREFIX"
#define VOIP_H248UserTIDInfo_UserTIDNameSlash "/"
#define VOIP_H248UserTIDInfo_UserTIDNameBase "PHY_TERM_BASE"

#define VOIP_H248RTPTIDConfig_Num "Number of RTP TID"
#define VOIP_H248RTPTIDConfig_Prefix "H_RTP_TERM_PREFIX"
#define VOIP_H248RTPTIDConfig_DigitBegin "H_RTP_TERM_BASE"
#define VOIP_H248RTPTIDConfig_Mode "RTP TID Mode"
#define VOIP_H248RTPTIDConfig_DigitLength "RTP TID Digit Length"

#define VOIP_H248RTPTIDInfo_Num "Number of RTP TID"
#define VOIP_H248RTPTIDInfo_FirstNamePrefix "H_RTP_TERM_PREFIX"
#define VOIP_H248RTPTIDInfo_FirstNameBase "H_RTP_TERM_BASE"

#define VOIP_SIPParameterConfig_MGPortNo "MGPortNo"
#define VOIP_SIPParameterConfig_SipProxyServIp "SipProxyServIp"
#define VOIP_SIPParameterConfig_SipProxyServComPortNo "SipProxyServComPortNo"
#define VOIP_SIPParameterConfig_BackupSipProxyServIp "BackupSipProxyServIp"
#define VOIP_SIPParameterConfig_BackupSipProxyServComPortNo "BackupSipProxyServComPortNo"
#define VOIP_SIPParameterConfig_ActiveSipProxyServer "Active SipProxyServer"
#define VOIP_SIPParameterConfig_SipRegServIP "SipRegServIP"
#define VOIP_SIPParameterConfig_SipRegServComPortNo "SipRegServComPortNo"
#define VOIP_SIPParameterConfig_BackupSipRegServIp "BackupSipRegServIp"
#define VOIP_SIPParameterConfig_BackupSipRegServComPortNo "BackupSipRegServComPortNo" 
#define VOIP_SIPParameterConfig_OutBoundServIP "OutBoundServIP" 
#define VOIP_SIPParameterConfig_OutBoundServPortNo "OutBoundServPortNo" 
#define VOIP_SIPParameterConfig_SipRegIntervalSIP "SipRegInterval SIP" 
#define VOIP_SIPParameterConfig_HeartbeatSwitch "HeartbeatSwitch" 
#define VOIP_SIPParameterConfig_HeartbeatCycle "HeartbeatCycle" 
#define VOIP_SIPParameterConfig_HeartbeatCount "HeartbeatCount" 

#define VOIP_SIPUserParameterConfig_UserAccount "User Account" 
#define VOIP_SIPUserParameterConfig_Username "User name" 
#define VOIP_SIPUserParameterConfig_UserPassword "User Password" 

#define VOIP_FaxModemConfig_T38Enable "VoiceT38Enable"  
#define VOIP_FaxModemConfig_Mode "VoiceFax/ModemControl"
/*
 * 注意：应调用接口 CfmAgent_GetStatus，
 * 输出value对应的值为: 
 *  "Registered "和 "UnRegistered"，
 *  Registered表示注册成功，UnRegistered表示注销
 */
#define VOIP_H248IADOperationStatus_IadOperStatus "E_REG_STATUS"
#define VOIP_H248IADOperationStatus_Registered "Registered"
#define VOIP_H248IADOperationStatus_UnRegistered "UnRegistered"
/* 
 * 注意：调用接口CfmAgent_GetStatus，输出value对应的值为
 * "on hook"和"off hook"，on hook表示挂机，off hook表示摘机
 */
#define VOIP_POTSStatus_IADPortStauts "E_HOOK_STATUS"
#define VOIP_POTSStatus_IADPortStauts_Onhook "on hook"
#define VOIP_POTSStatus_IADPortStauts_Offhook "off hook"

#define VOIP_POTSStatus_iadPortServiceState "iadPortServiceState"
#define VOIP_POTSStatus_iadPortCodecMode "iadPortCodecMode"

#define VOIP_IADOperation "iadOperation"

#define VOIP_SIPDigitMap "SIP DigitalMap"

/* Hardware definition */
#define GPIO_TX_POWER_CTRL 9

#pragma pack()
 
#endif /* #ifndef __DEFS_H_ */
 

