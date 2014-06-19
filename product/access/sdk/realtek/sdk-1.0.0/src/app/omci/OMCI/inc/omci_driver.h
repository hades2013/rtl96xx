/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 39101 $
 * $Date: 2013-06-24 04:35:27 -0500 (Fri, 03 May 2013) $
 *
 * Purpose : OMCI driver layer module defination
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) OMCI (G.984.4)
 *
 */
#ifndef __OMCI_DRIVER_H__
#define __OMCI_DRIVER_H__


/* Upstream Traffic Management Option referred by ONT-G */
typedef enum 
{
    PON_US_TRAFF_PRI_SCHE  = 0x0,
    PON_US_TRAFF_CELL_RATE = 0x1,
    PON_US_TRAFF_MIX       = 0x2,
} PON_US_TRAFF_MG_OPTION;


typedef enum
{
    PON_GEMPORT_DIRECTION_US = 0x1,
    PON_GEMPORT_DIRECTION_DS = 0x2,
    PON_GEMPORT_DIRECTION_BI = 0x3,
} PON_GEMPORT_DIRECTION;


typedef enum 
{
    PON_ONU_STATE_INITIAL            = 0x01,      /* O1 */
    PON_ONU_STATE_STANDBY            = 0x02,      /* O2 */
    PON_ONU_STATE_SERIAL_NUMBER      = 0x03,      /* O3 */
    PON_ONU_STATE_RANGING            = 0x04,      /* O4 */
    PON_ONU_STATE_OPERATION          = 0x05,      /* O5 */
    PON_ONU_STATE_POPUP              = 0x06,      /* O6 */ 
    PON_ONU_STATE_EMERGENCY_STOP     = 0x07,      /* O7 */ 
    PON_ONU_STATE_UNKNOWN            = 0x08, 
} PON_ONU_STATE;


typedef enum {
    PLOAM_DS_OVERHEAD        = 0x01,
    PLOAM_DS_SNMASK          = 0x02,
    PLOAM_DS_ASSIGNONUID     = 0x03,
    PLOAM_DS_RANGINGTIME     = 0x04,
    PLOAM_DS_DEACTIVEONU     = 0x05,
    PLOAM_DS_DISABLESN       = 0x06,
    PLOAM_DS_CFG_VPVC        = 0x07,
    PLOAM_DS_ENCRYPTPORT     = 0x08,
    PLOAM_DS_REQUESTPASSWORD = 0x09,
    PLOAM_DS_ASSIGNEDALLOCID = 0x0A,
    PLOAM_DS_NOMESSAGE       = 0x0B,
    PLOAM_DS_POPUP           = 0x0C,
    PLOAM_DS_REQUESTKEY      = 0x0D,
    PLOAM_DS_CONFIGPORT      = 0x0E,
    PLOAM_DS_PEE             = 0x0F,
    PLOAM_DS_POWERLEVEL      = 0x10,
    PLOAM_DS_PST             = 0x11,
    PLOAM_DS_BER_INTERVAL    = 0x12,
    PLOAM_DS_SWITCHINGKEY    = 0x13,
    PLOAM_DS_EXT_BURSTLENGTH = 0x14,
}GMac_Ploam_Ds_te;


/* MEs Class ID provision to Transwitch */
typedef enum 
{
    PON_ME_CLASS_UNKNOWN               = 0,
    PON_ME_CLASS_ONT_DATA              = 2,
    PON_ME_CLASS_CARDHOLDER            = 5,
    PON_ME_CLASS_CIRCUIT_PACK          = 6,
    PON_ME_CLASS_SW_IMAGE              = 7,
    PON_ME_CLASS_PPTP_ETH_UNI          = 11,
    PON_ME_CLASS_MAC_BRI_SERV_PROF     = 45,
    PON_ME_CLASS_MAC_BRI_PORT_CFG_DATA = 47,
    PON_ME_CLASS_VLAN_TAG_OP_CFG_DATA  = 78, 
    PON_ME_CLASS_VLAN_TAG_FILTER_DATA  = 84, 
    PON_ME_CLASS_802_1P_SERV_PROF      = 130, 
    PON_ME_CLASS_ONT_G                 = 256,
    PON_ME_CLASS_ONT2_G                = 257,
    PON_ME_CLASS_T_CONT                = 262,
    PON_ME_CLASS_ANI_G                 = 263,
    PON_ME_CLASS_GEM_IW_TP             = 266,
    PON_ME_CLASS_GEM_PORT_NETWORK_CTP  = 268,
    PON_ME_CLASS_PRI_Q_G               = 277,
    PON_ME_CLASS_TRAFFIC_SCHE_G        = 278,
    PON_ME_CLASS_MCAST_GEM_IW_TP       = 281,
}PON_ME_CLASS_ID;


typedef enum
{
    PON_ME_ACTION_NONE              = 0x0,
    PON_ME_ACTION_CREATE            = (1 << (4 - 1)),
    PON_ME_ACTION_DELETE            = (1 << (6 - 1)),
    PON_ME_ACTION_SET               = (1 << (8 - 1)),
    PON_ME_ACTION_GET               = (1 << (9 - 1)),
    PON_ME_ACTION_GET_ALLALARM      = (1 << (11 - 1)),
    PON_ME_ACTION_GET_ALLALARM_NEXT = (1 << (12 - 1)),
    PON_ME_ACTION_MIB_UPLOAD        = (1 << (13 - 1)),
    PON_ME_ACTION_MIB_UPLOAD_NEXT   = (1 << (14 - 1)),
    PON_ME_ACTION_MIB_RESET         = (1 << (15 - 1)),
    PON_ME_ACTION_TEST              = (1 << (18 - 1)),
    PON_ME_ACTION_SW_DOWNLOAD       = (1 << (19 - 1)),
    PON_ME_ACTION_SW_DOWNLOAD_SEC   = (1 << (20 - 1)),
    PON_ME_ACTION_SW_DOWNLOAD_END   = (1 << (21 - 1)),
    PON_ME_ACTION_ACT_SW            = (1 << (22 - 1)),
    PON_ME_ACTION_COMMIT_SW         = (1 << (23 - 1)),
    PON_ME_ACTION_SYNC_TIME         = (1 << (24 - 1)),
    PON_ME_ACTION_REBOOT            = (1 << (25 - 1)),
    PON_ME_ACTION_GET_NEXT          = (1 << (26 - 1)),
    PON_ME_ACTION_GET_CUR_DATA      = (1 << (28 - 1)),
} PON_ME_ACTION_TYPE;

typedef enum 
{
    PON_ME_INIT_TYPE_AUTO,      // Auto initialized
    PON_ME_INIT_TYPE_BY_OLT,    // Created by OLT with OMCI message
} PON_ME_INIT_TYPE;


typedef enum 
{
    PON_ME_STD_TYPE_STD, // Standard ME that defined in G.983 and G.984
    PON_ME_STD_TYPE_PRO, // Propriety ME that defined in other standards and should be published
    PON_ME_STD_TYPE_PRI, // Private ME that defined internally and cannot be accessed externally
} PON_ME_STD_TYPE;


typedef enum 
{
    PON_ME_OLT_READ          = 0x1,
    PON_ME_OLT_WRITE         = 0x2,
    PON_ME_OLT_SET_BY_CREATE = 0x4,
} PON_ME_OLT_ACC_TYPE;


typedef enum
{
    PON_ME_ATTR_OPT_NOT_SUPPORT   = 0x0,  // Attribute is optional and not supported
    PON_ME_ATTR_OPT_SUPPORT       = 0x1,  // Attribute is optional and supported
    PON_ME_ATTR_MANDATORY         = 0x11, // Attribute is mandatory
    PON_ME_ATTR_PRIVATE           = 0xFF, // Attributed is not defined in 984.4
} PON_ME_ATTR_OPT_TYPE;


/* Table 3/G.983.2 - Subscriber line card types */
typedef enum 
{
    PON_LCARD_TYPE_NO_LIM      = 0,
    PON_LCARD_TYPE_ATM_1_5     = 1,
    PON_LCARD_TYPE_ATM_2       = 2,
    PON_LCARD_TYPE_ATM_63      = 3,
    PON_LCARD_TYPE_ATM_6_3U    = 4,
    PON_LCARD_TYPE_ATM_8       = 5,
    PON_LCARD_TYPE_ATM_25      = 6,
    PON_LCARD_TYPE_ATM_34      = 7,
    PON_LCARD_TYPE_ATM_45      = 8,
    PON_LCARD_TYPE_ATM_34_45   = 9,
    PON_LCARD_TYPE_ATM_SMF     = 10,
    PON_LCARD_TYPE_ATM_MMF     = 11,
    PON_LCARD_TYPE_ATM_UTP     = 12,
    PON_LCARD_TYPE_DS1         = 13,   
    PON_LCARD_TYPE_E1          = 14,   
    PON_LCARD_TYPE_J2          = 15,   
    PON_LCARD_TYPE_E1_DS1      = 16,   
    PON_LCARD_TYPE_E1_DS1_J1   = 17,   
    PON_LCARD_TYPE_J2U         = 18,   
    PON_LCARD_TYPE_C192        = 19,   
    PON_LCARD_TYPE_E3          = 20,   
    PON_LCARD_TYPE_DS3         = 21,
    PON_LCARD_TYPE_ETH         = 22,
    PON_LCARD_TYPE_FE          = 23,
    PON_LCARD_TYPE_10_100_ETH  = 24,
    PON_LCARD_TYPE_TOKEN       = 25,   
    PON_LCARD_TYPE_FDDI        = 26,   
    PON_LCARD_TYPE_FR          = 27,   
    PON_LCARD_TYPE_J1          = 28,   
    PON_LCARD_TYPE_ATM_OC3_SMF = 29,    
    PON_LCARD_TYPE_ATM_OC3_MMF = 30,    
    PON_LCARD_TYPE_ATM_OC3_UTP = 31,    
    PON_LCARD_TYPE_POTS        = 32,
    PON_LCARD_TYPE_B_ISDN      = 33,    
    PON_LCARD_TYPE_GE_OPTICAL  = 34,
    PON_LCARD_TYPE_ADSL        = 35,    
    PON_LCARD_TYPE_SHDSL       = 36,    
    PON_LCARD_TYPE_VDSL        = 37,    
    PON_LCARD_TYPE_VEDIO       = 38,
    PON_LCARD_TYPE_LCD         = 39,
    PON_LCARD_TYPE_802_11      = 40,
    PON_LCARD_TYPE_ADSL_POTS   = 41,
    PON_LCARD_TYPE_VDSL_POTS   = 42,
    PON_LCARD_TYPE_COMMON      = 43,
    PON_LCARD_TYPE_VEDIO_PON   = 44,
    PON_LCARD_TYPE_MIXED       = 45,
    PON_LCARD_TYPE_GE          = 47,

    PON_LCARD_TYPE_PON_12440155 = 243,
    PON_LCARD_TYPE_PON_12440622 = 244,
    PON_LCARD_TYPE_PON_1244_SYM = 245,
    PON_LCARD_TYPE_PON_24880155 = 246,
    PON_LCARD_TYPE_PON_24880622 = 247,
    PON_LCARD_TYPE_PON_24881244 = 248,
    PON_LCARD_TYPE_PON_2488_SYM = 249,

    PON_LCARD_TYPE_PON_622_SYM = 252,   
    PON_LCARD_TYPE_PON_155     = 253,   
    PON_LCARD_TYPE_PON_622_ASY = 254,   
    PON_LCARD_TYPE_PNP         = 255, 

    PON_LCARD_TYPE_PRIVATE     = 0xFFFF,
}PON_LCARD_TYPE;




typedef enum 
{
    PON_ENABLE,
    PON_DISABLE,
    PON_UNKNOWN,
} PON_ENABLE_DISABLE;


typedef enum 
{
    PON_MAC_BRI_PORT_TP_TYPE_ETH    = 0x1,
    PON_MAC_BRI_PORT_TP_TYPE_ATM    = 0x2,
    PON_MAC_BRI_PORT_TP_TYPE_802    = 0x3,
    PON_MAC_BRI_PORT_TP_TYPE_IPHOST = 0x4,  
    PON_MAC_BRI_PORT_TP_TYPE_GEM    = 0x5,  
    PON_MAC_BRI_PORT_TP_TYPE_MGEM   = 0x6, 
    PON_MAC_BRI_PORT_TP_TYPE_XDSL   = 0x7,
    PON_MAC_BRI_PORT_TP_TYPE_VDSL   = 0x8,
    PON_MAC_BRI_PORT_TP_TYPE_ETHFLOW= 0x9,
    PON_MAC_BRI_PORT_TP_TYPE_80211  = 0xa,
    PON_MAC_BRI_PORT_TP_TYPE_UNKOWN = 0xb,
} PON_MAC_BRI_PORT_TP_TYPE;


typedef enum
{
    PON_VLANFILTER_FWDOP_A_A  = 0x00,     // A on tagged, A on untagged
    PON_VLANFILTER_FWDOP_C_A  = 0x01,     // C on tagged, A on untagged
    PON_VLANFILTER_FWDOP_A_E  = 0x02,     // A on tagged, E on untagged
    PON_VLANFILTER_FWDOP_FV_A = 0x03,     // FV on tagged, A on untagged
    PON_VLANFILTER_FWDOP_FV_E = 0x04,     // FV on tagged, E on untagged
    PON_VLANFILTER_FWDOP_GV_A = 0x05,     // GV on tagged, A on untagged
    PON_VLANFILTER_FWDOP_GV_E = 0x06,     // GV on tagged, E on untagged
    PON_VLANFILTER_FWDOP_FP_A = 0x07,     // FP on tagged, A on untagged
    PON_VLANFILTER_FWDOP_FP_E = 0x08,     // FP on tagged, E on untagged
    PON_VLANFILTER_FWDOP_GP_A = 0x09,     // GP on tagged, A on untagged
    PON_VLANFILTER_FWDOP_GP_E = 0x0A,     // GP on tagged, E on untagged
    PON_VLANFILTER_FWDOP_FT_A = 0x0B,     // FT on tagged, A on untagged
    PON_VLANFILTER_FWDOP_FT_E = 0x0C,     // FT on tagged, E on untagged
    PON_VLANFILTER_FWDOP_GT_A = 0x0D,     // GT on tagged, A on untagged
    PON_VLANFILTER_FWDOP_GT_E = 0x0E,     // GT on tagged, E on untagged
    PON_VLANFILTER_FWDOP_HV_A = 0x0F,     // HV on tagged, A on untagged
    PON_VLANFILTER_FWDOP_HV_E = 0x10,     // HV on tagged, E on untagged
    PON_VLANFILTER_FWDOP_HP_A = 0x11,     // HP on tagged, A on untagged
    PON_VLANFILTER_FWDOP_HP_E = 0x12,     // HP on tagged, E on untagged
    PON_VLANFILTER_FWDOP_HT_A = 0x13,     // HT on tagged, A on untagged
    PON_VLANFILTER_FWDOP_HT_E = 0x14,     // HT on tagged, E on untagged
} PON_VLANFILTER_FWD_OP;


/*classify difference VLAN flilter mode*/
typedef enum {
	OMCI_VLANFILTER_MODE_FORWARDALL,
	OMCI_VLANFILTER_MODE_DROPTAG_FOWARDUNTAG,
	OMCI_VLANFILTER_MODE_FORWARDTAG_DROPUNTAG,
	OMCI_VLANFILTER_MODE_FORWARDLIST_FORWARDUNTAG,
	OMCI_VLANFILTER_MODE_FORWARDLIST_DROPUNTAG,
	OMCI_VLANFILTER_MODE_NOTSUPPORT
}OMCI_VLANFILTER_MODE_T;




enum {
	OMCI_VID_FILTER_UNTAG  = 4095,
	OMCI_VID_FILTER_IGNORE = 4096 
};

enum { 
	OMCI_PRI_FILTER_UNTAG  			= 13,
    OMCI_PRI_FILTER_IGNORE			= 8,
    OMCI_PRI_FILTER_DEFAULT_FIELD 	= 14,
    OMCI_PRI_FILTER_IGNORE_FIELD 	= 15
};



typedef enum 
{
   PON_US_VLAN_OP_ASIS      = 0, // asis
   PON_US_VLAN_OP_MODIFY    = 1, // replace vid
   PON_US_VLAN_OP_INSERT    = 2, // add vid  
   PON_US_VLAN_OP_MAX,
} PON_US_VLAN_OP_MODE;


typedef enum 
{
   PON_DS_VLAN_OP_ASIS      = 0, // asis
   PON_DS_VLAN_OP_REMOVE    = 1, // remove vid
} PON_DS_VLAN_OP_MODE;


typedef enum 
{
    PON_SW_IMG_UNCOMMITTED,
    PON_SW_IMG_COMMITTED,
} PON_SW_IMG_COMMIT_STATE;


typedef enum
{
    PON_SW_IMG_INACTIVE,
    PON_SW_IMG_ACTIVE,
} PON_SW_IMG_ACTIVE_STATE;


typedef enum
{
    PON_SW_IMG_INVALID,
    PON_SW_IMG_VALID,
} PON_SW_IMG_VALID_STATE;



/* omci message definition */
typedef enum
{
    PON_OMCI_MSG_CREATE            = 4,
    PON_OMCI_MSG_DELETE            = 6,
    PON_OMCI_MSG_SET               = 8,
    PON_OMCI_MSG_GET               = 9,
    PON_OMCI_MSG_GET_ALL_ALARM	   = 11,
    PON_OMCI_MSG_GET_ALL_ALARM_NEXT= 12,
    PON_OMCI_MSG_MIB_UPLOAD        = 13,
    PON_OMCI_MSG_MIB_UPLOAD_NEXT   = 14,
    PON_OMCI_MSG_MIB_RESET         = 15,
    PON_OMCI_MSG_MIB_ALARM		   = 16,
    PON_OMCI_MSG_AVC               = 17,
    PON_OMCI_MSG_TEST			   = 18,
    PON_OMCI_MSG_START_SW_DOWNLOAD = 19,
    PON_OMCI_MSG_DOWNLOAD_SECTION  = 20,
    PON_OMCI_MSG_END_SW_DOWNLOAD   = 21,
    PON_OMCI_MSG_ACTIVATE_SW       = 22,
    PON_OMCI_MSG_COMMIT_SW         = 23,
    PON_OMCI_MSG_SYNC_TIME		   = 24,
    PON_OMCI_MSG_REBOOT            = 25,
    PON_OMCI_MSG_GET_NEXT          = 26,
    PON_OMCI_MSG_TEST_RESULT	   = 27,
    PON_OMCI_MSG_GET_CURRENT_DATA  = 28
}PON_OMCI_MSG_TYPE;


#define PON_OMCI_MSG_PRI_MASK           (0x8000)
#define PON_OMCI_MSG_PRI_BIT            (15)
#define PON_OMCI_MSG_TID_MASK           (0x7FFF)
#define PON_OMCI_MSG_TYPE_DB_MASK       (0x80)
#define PON_OMCI_MSG_TYPE_DB_BIT        (7)
#define PON_OMCI_MSG_TYPE_AR_MASK       (0x40)
#define PON_OMCI_MSG_TYPE_AR_BIT        (6)
#define PON_OMCI_MSG_TYPE_AK_MASK       (0x20)
#define PON_OMCI_MSG_TYPE_AK_BIT        (5)
#define PON_OMCI_MSG_TYPE_MT_MASK       (0x1F)


#define PON_OMCI_MSG_DEVICE_ID          (0x0A)
#define PON_OMCI_MSG_CONTENT_LEN        (32)


typedef enum 
{
    PON_OMCI_RSP_SUCCESS         = 0x0,
    PON_OMCI_RSP_PROC_ERROR      = 0x1,
    PON_OMCI_RSP_NOT_SUPPORT     = 0x2,
    PON_OMCI_RSP_PARAM_ERROR     = 0x3,
    PON_OMCI_RSP_UNKNOWN_ME      = 0x4,
    PON_OMCI_RSP_UNKNOWN_ME_INST = 0x5,
    PON_OMCI_RSP_DEVICE_BUSY     = 0x6,
    PON_OMCI_RSP_INST_EXIST      = 0x7,    
    PON_OMCI_RSP_UNDEFINE        = 0x8,
    PON_OMCI_RSP_ATTR_FAILED     = 0x9,
} PON_OMCI_MSG_RESULT;


typedef enum
{
	PON_OMCI_CMD_DUMPAVL,
	PON_OMCI_CMD_LOG,
	PON_OMCI_CMD_DUMPQUEUE,
	PON_OMCI_CMD_DUMPTREECONN,
	PON_OMCI_CMD_DUMPMIB,
	PON_OMCI_CMD_SN_SET,
	PON_OMCI_CMD_SN_GET,
	PON_OMCI_CMD_LOADFILE,
	PON_OMCI_CMD_END,
	
}PON_OMCI_CMD_TYPE;


typedef enum 
{
	OMCI_ERR_OK,
	OMCI_ERR_FAILED,
	OMCI_ERR_END
}OMCI_CHIP_ERROR_CODE;




typedef struct {
	int tcontId;
	int allocId;
} OMCI_TCONT_ts;

typedef struct
{
	unsigned int	flowId;
	unsigned int	portId;
	unsigned int	tcontEId; /*OMCI entity index*/
	unsigned int	tcontPId; /*CHIP Physical Tcont Index*/
	unsigned int	allocId;  /*Tcont Physical allocated Id*/
	unsigned int 	queueEId; /*queue entity index*/
	unsigned int 	queuePId; /*queue phycial index*/
	unsigned int	cir;
	unsigned int	pir;
	int             isOmcc;
	int             ena;
	PON_GEMPORT_DIRECTION dir;

} OMCI_GEM_FLOW_ts;

typedef struct
{
    int                  ena;
    PON_VLANFILTER_FWD_OP fwdOper;
    int                cTci[12];
} OMCI_VLAN_TAG_FLT_ts;

typedef struct
{
    int   ena;
    int tagOper;
    int sVlanId;
} OMCI_VLAN_TAG_OPER_ts;


typedef enum
{
	VLAN_OPER_MODE_FORWARD_ALL,
	VLAN_OPER_MODE_FORWARD_UNTAG,	
	VLAN_OPER_MODE_FORWARD_SINGLETAG,
	VLAN_OPER_MODE_FORWARD_INNER_PRI,
	VLAN_OPER_MODE_FILTER_SINGLETAG,
	VLAN_OPER_MODE_EXTVLAN,

}OMCI_VLAN_OPER_MODE_t;



typedef struct 
{
	unsigned int pri;
	unsigned int vid;
	unsigned int tpid;
	
}OMCI_VLAN_ts;

typedef enum
{
	VLAN_FILTER_NO_CARE,
	VLAN_FILTER_NO_TAG,
	VLAN_FILTER_VID,
	VLAN_FILTER_PRI,
	VLAN_FILTER_TCI,
	VLAN_FILTER_ETHTYPE
}OMCI_VLAN_FILTER_MODE_e;


typedef enum
{
	TPID_FILTER_NO_CARE=0,
	TPID_FILTER_8021Q=0x100,
	TPID_FILTER_INPUT=0x101,
	TPID_FILTER_INPUT_DEI0=0x110,
	TPID_FILTER_INPUT_DEI1=0x111,
}OMCI_TPID_FILTER_MODE_e;

typedef enum
{
	ETHTYPE_FILTER_NO_CARE=0,
	ETHTYPE_FILTER_IP=1,
	ETHTYPE_FILTER_PPPOE=2,
	ETHTYPE_FILTER_ARP=3
}OMCI_ETHTYPE_FILTER_MODE_e;



typedef struct
{
	OMCI_VLAN_ts filterSTag;	
	OMCI_VLAN_ts filterCTag;
	unsigned int filterEtherType;
	unsigned int filterTagNum;
	unsigned int stagAct;
	OMCI_VLAN_ts assignSTag;
	unsigned int ctagAct;
	OMCI_VLAN_ts assignCTag;	
	OMCI_VLAN_FILTER_MODE_e filterStagMode;
	OMCI_VLAN_FILTER_MODE_e filterCtagMode;

} OMCI_VLAN_OPER_ts;



typedef struct
{
	int queueId;
	int tcontId;
	int cir;
	int pir;
	int weight;
	PON_US_TRAFF_MG_OPTION scheduleType;
	
} OMCI_PRIQ_ts;





typedef enum {
	OMCI_IOCTL_TCONT_GET,
	OMCI_IOCTL_GEMPORT_SET,
	OMCI_IOCTL_PRIQ_SET,
	OMCI_IOCTL_CF_DEL,
	OMCI_IOCTL_CF_ADD,
	OMCI_IOCTL_SN_SET,
	OMCI_IOCTL_SN_GET,
	OMCI_IOCTL_MIB_RESET,
	OMCI_IOCTL_END
}OMCI_IOCTL_t;


#endif
