/*=============================================================================
FILE - oam.h
    oam stack description

DESCRIPTION
    Implements the OAM protocol stack

MODIFICATION DETAILS
=============================================================================*/


#ifndef __OAM_H__
#define __OAM_H__

#include "lw_config.h"

#define FALSE	0
#define TRUE	1

#define OPL_OK	0

typedef unsigned char	   u8_t;
typedef unsigned short	   u16_t;
typedef unsigned int	   u32_t;
typedef unsigned long long u64_t;

typedef int				   bool_t;
typedef int             i32_t;

#ifndef ENABLE
#define ENABLE              1
#endif

#ifndef DISABLE
#define DISABLE             0
#endif

/* For OLB define EPOPL_OLB,else undef it*/

#define MAX_PON_WAIT_TIMES    30

#define EOPL_OAM_LOST_LINK_TIMER		50	 /* 5s, 100millisecond per unit*/
#define EOPL_OAM_PDU_TIMER			     10  /* 5s, 100millisecond per unit*/


#define OAM_CODE_INFO            0
#define OAM_CODE_EVENT           1
#define OAM_CODE_VARIABLE_REQ    2
#define OAM_CODE_VARIABLE_RSP    3
#define OAM_CODE_LOOPBACK        4
#define OAM_CODE_ORG_SPEC        0xFE

#define OAM_EXT_CODE_GET_REQUEST           0x01
#define OAM_EXT_CODE_GET_RESPONSE          0x02
#define OAM_EXT_CODE_SET_REQUEST           0x03
#define OAM_EXT_CODE_SET_RESPONSE          0x04
#define OAM_EXT_CODE_AUTHENTICATION        0x05
#define OAM_EXT_CODE_DOWNLOAD              0x06
#define OAM_EXT_CODE_CHURNING              0x09
#define OAM_EXT_CODE_DBA                   0x0A
#define OAM_OPL_OPCODE_AES                 0x80
#define OAM_OPL_OPCODE_AES_SCB             0x81
#define OAM_OPL_OPCODE_TRI_SCB             0x82


#define OAM_BRANCH_STANDARD_ATTRIB		0x07
#define OAM_BRANCH_STANDARD_ACTION		0x09
#define OAM_BRANCH_EXTENDED_ATTRIB		0xC7
#define OAM_BRANCH_EXTENDED_ACTION		0xC9
#define OAM_BRANCH_INSTANCE_V_2_0				0x36
#define OAM_BRANCH_INSTANCE_V_2_1				0x37
#define OAM_BRANCH_PMC_ATTRIB		    0x87
#define OAM_BRANCH_INSTANCE_NTT

/* Std attribution & action definition */
#define OAM_ATTR_PHY_ADMIN				0x0025
#define OAM_ATTR_AUTONEG_ADMIN_STATE	0x004F
#define OAM_ATTR_AUTONEG_LOCAL_ABILITY	0x0052
#define OAM_ATTR_AUTONEG_ADV_ABILITY	0x0053
#define OAM_ATTR_FEC_ABILITY			0x0139
#define OAM_ATTR_FEC_MODE				0x013A
#define OAM_ACTION_PHY_ADMIN_CONTROL		0x0005
#define OAM_ACTION_AUTONEG_RST_AUTOCONFIG	0x000B
#define OAM_ACTION_AUTONEG_ADMIN_CONTROL	0x000C


#define OAM_INSTANCE_PORT            0x0001
#define OAM_INSTANCE_LLID            0x0002

#define OAM_ATTR_ONU_SN              0x0001
#define OAM_ATTR_ONU_FIMWAREVER      0x0002
#define OAM_ATTR_CHIPSET_ID          0x0003
#define OAM_ATTR_ONU_CAPABILITIES1		0x0004
#define OAM_ATTR_OPTICS_DIAG			0x0005
#define OAM_ATTR_SERVICE_SLA			0x0006
#define OAM_ATTR_ONU_CAPABILITIES2		0x0007
#define OAM_ATTR_HOLDOVER_CONFIG		0x0008
#define OAM_ATTR_MXU_MNG_GLOBAL_PARA	0x0009
#define OAM_ATTR_MXU_MNG_SNMP_PARA		0x000a
#define OAM_ATTR_ACTIVE_PON_IF_ADMIN	0x000b
/* #define OAM_ATTR_OPTI_ALRM_WARN			0x0006 */
/* #define OAM_ATTR_OPTI_AW_TH				0x0007 */
#define OAM_ATTR_ETH_LINKSTATE       0x0011
#define OAM_ATTR_ETH_PORTPAUSE       0x0012
#define OAM_ATTR_ETH_PORT_US_POLICING	0x0013
#define OAM_ATTR_VOIP_PORT				0x0014
#define OAM_ATTR_E1_PORT				0x0015
#define OAM_ATTR_ETH_PORT_DS_POLICING	0x0016
#define OAM_ATTR_PORT_LOOP_DETECT		0x0017
#define OAM_ATTR_VLAN                0x0021
#define OAM_ATTR_CLAMARK             0x0031
#define OAM_ATTR_MCAST_VLAN          0x0041
#define OAM_ATTR_MCAST_TAG_STRIP		0x0042
#define OAM_ATTR_MCAST_SWITCH        0x0043
#define OAM_ATTR_MCAST_CONTROL       0x0044
#define OAM_ATTR_MCAST_GRP_NUMMAX		0x0045
#define OAM_ATTR_MCAST_GFASTLEAVE_ABL	0x0046
#define OAM_ATTR_MCAST_GFASTLEAVE_ADMIN	0x0047
#define OAM_ATTR_FASTLEAVE_CONTROL   0x0048
#define OAM_ATTR_LLID_QUEUE_CONFIG		0x0051

#define OAM_ATTR_IAD_INFO				0x0061
#define OAM_ATTR_GLOBAL_PARA_CONFIG		0x0062//
#define OAM_ATTR_H248_PARA_CONFIG		0x0063//
#define OAM_ATTR_H248_USER_TID_CONFIG	0x0064//
#define OAM_ATTR_H248_RTP_TID_CONFIG	0x0065//
#define OAM_ATTR_H248_RTP_TID_INFO		0x0066
#define OAM_ATTR_SIP_PARA_CONFIG		0x0067
#define OAM_ATTR_SIP_USER_PARA_CONFIG	0x0068
#define OAM_ATTR_FAX_MODEM_CONFIG		0x0069
#define OAM_ATTR_IAD_OPERATION_STATUS	0x006a
#define OAM_ATTR_POTS_STATUS			0x006b
#define OAM_ATTR_IAD_OPERATION			0x006c
#define OAM_ATTR_SIP_DIGIT_MAP			0x006d

#define OAM_ATTR_ALARM_ADMIN_STATE		0x0081
#define OAM_ATTR_ALARM_THRESHOLD        0x0082
#define OAM_ACTION_RESET_ONU			0x0001
#define OAM_ACTION_FAST_LEAVE_ADMIN		0x0048
#define OAM_ACTION_MULTI_LLID_ADMIN		0x0202
#define OAM_ACTION_RESET_CARD			0x0401

/* ONUTxPowerSupplyControl */
#define OAM_ATTR_TX_POWER_SUPPLY_CTRL	0x00A1

#define OAM_GET_DBA_REQUEST          0x00
#define OAM_GET_DBA_RESPONSE         0x01
#define OAM_SET_DBA_REQUEST          0x02
#define OAM_SET_DBA_RESPONSE         0x03

#define OAM_NEW_KEY_REQUEST          0x00
#define OAM_NEW_CHURNING_KEY         0x01

#define OAM_ATTR_FRM_TX_OK              0x0002
#define OAM_ATTR_FRM_RX_OK              0x0005
#define OAM_ATTR_PHY_ADMIN_CONTROL      0x0005
#define OAM_ATTR_AUTONEG_RST_AUTOCONFIG 0x000B
#define OAM_ATTR_AUTONEG_ADMIN_CONTROL  0x000C

#define OAM_OPERATOR_NEVER_MATCH        0x00
#define OAM_OPERATOR_EQUAL              0x01
#define OAM_OPERATOR_NOT_EQUAL          0x02
#define OAM_OPERATOR_LESS_EQUAL         0x03
#define OAM_OPERATOR_BIG_EQUAL          0x04
#define OAM_OPERATOR_EXISTS             0x05
#define OAM_OPERATOR_NOT_EXISTS         0x06
#define OAM_OPERATOR_ALWAYS_MATCH       0x07

#define OAM_SET_OK                      0x80
#define OAM_SET_BAD_PARAMETERS          0x86
#define OAM_SET_NO_RESOURCE             0x87

#define OAM_NO_PARAM                    0xCB
#define OAM_BAD_PARAM_VALUE             0xCC

#define OAM_GET_OK                      0x80
#define OAM_GET_BAD_PARA                0x86
#define OAM_GET_NO_RESOURCE             0x87

#define INFO_LOCAL_END        0
#define INFO_LOCAL_TYPE       1
#define INFO_REMOTE_TYPE      2
#define INFO_ORG_SPEC_TYPE    0xFE

#define OAM_DEFAULT_ERR_SYMBOL_WINDOW       1
#define OAM_DEFAULT_ERR_SYMBOL_THRESH       20

#define OAM_DEFAULT_ERR_FRAME_WINDOW        10
#define OAM_DEFAULT_ERR_FRAME_THRESH        20

#define OAM_DEFAULT_ERR_FR_PERIOD_WINDOW    1
#define OAM_DEFAULT_ERR_FR_PERIOD_THRESH    20

#define OAM_DEFAULT_ERR_SEC_SUMMRY_WINDOW   600
#define OAM_DEFAULT_ERR_SEC_SUMMRY_THRESH   1


#define STAT_IEEE_BRANCH            (OAM_BRANCH_STANDARD_ATTRIB << 16 )
#define STAT_PMC_BRANCH             (OAM_BRANCH_PMC_ATTRIB << 16 )
#define STAT_ATTR_BROD_TX_RRM        (STAT_IEEE_BRANCH|0x13)
#define STAT_ATTR_BROD_RX_RRM        (STAT_IEEE_BRANCH|0x16)
#define STAT_ATTR_PON_PAUSE_RX       (STAT_PMC_BRANCH |0x1C)
#define STAT_ATTR_SYS_PAUSE_RX       (STAT_PMC_BRANCH |0x1D)
#define STAT_ATTR_SYS_PAUSE_TX       (STAT_PMC_BRANCH |0x1E)
#define STAT_ATTR_PON_PAUSE_TX       (STAT_PMC_BRANCH |0x31)


#define SYM_ERROR_TYPE      0x01
#define SYM_ERROR_LEN       0x28
#define FRM_ERROR_TYPE      0x02
#define FRM_ERROR_LEN       0x1A
#define FRM_PRD_TYPE        0x03
#define FRM_PRD_LEN         0x1C
#define SEC_ERROR_TYPE      0x04
#define SEC_ERROR_LEN       0x12

#define FIRM_LENGTH             2

#define OAM_MAXIMUM_PDU_SIZE    1518
#define OAM_MINIMIZE_PDU_SIZE   60 /*64*/

#define CTC_MIN_SUBPORT_NUM   1

#if defined(CONFIG_PRODUCT_EM200)
#define CTC_MAX_SUBPORT_NUM   1
#else
#define CTC_MAX_SUBPORT_NUM   4
#endif

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
#define OAM_MAX_INSTANCE_ID     0x04
#else
#define OAM_MAX_INSTANCE_ID     CTC_MAX_SUBPORT_NUM
#endif


#define EOPL_PARAM_ERROR                0x81
#define EOPL_NOT_SUPPORT                0x82


#define OAM_MAX_E1_PORT_ID 		0x04
#define OAM_MAX_VOIP_PORT_ID	0x04

#define EOPL_OK                         0x00000000
#define PARAMETER_INPUT_POINTER_NULL    0x80001000
#define PARAMETER_RANGE_ERROR           0x80001001

#define OAM_CLASSIFY_ACTION_DEL     0x00
#define OAM_CLASSIFY_ACTION_ADD     0x01
#define OAM_CLASSIFY_ACTION_CLR     0x02
#define OAM_CLASSIFY_ACTION_SHOW    0x03
#define OAM_CLASSIFY_MAX_ENTRY      128

#define OAM_CTC_VLAN_TRANSPARENT    0x00
#define OAM_CTC_VLAN_TAG            0x01
#define OAM_CTC_VLAN_TRANSLATE	    0x02
#define OAM_CTC_VLAN_AGGREGATION	0x03
#define OAM_CTC_VLAN_TRUNK			0x04

#define OAM_LINK_LOST_TIME  5

#define OAM_OPL_RST_CFG     0x4039
#define OAM_OPL_RST_PAY     0x403A

#define OAM_SOFT_TYPE_TFTP  0x01
#define OAM_SOFT_TYPE_CRC   0x02
#define OAM_SOFT_TYPE_LOAD  0x03
#define OAM_SOFT_TYPE_COMMIT 0x04


#define OAM_FIRMWARE_WRITE_SUCCESS 0x0
#define OAM_FIRMWARE_WRITING       0x1
#define OAM_FIRMWARE_VERIFY_ERROR  0x2
#define TFTP_R_REQ          0x0001
#define TFTP_W_REQ          0x0002
#define TFTP_DATA           0x0003
#define TFTP_ACK            0x0004
#define TFTP_ERR            0x0005
#define TFTP_END_DOWNLOAD_REQ	0x0006
#define TFTP_END_DOWNLOAD_RSP	0x0007
#define TFTP_ACT_IMG_REQ    0x0008
#define TFTP_ACT_IMG_RSP    0x0009
#define TFTP_COMMIT_IMG_REQ 	0x000a
#define TFTP_COMMIT_IMG_RSP 	0x000b

#define DOWN_RSP_OK_BURN    0x00
#define DOWN_RSP_BURNING    0x01
#define DOWN_RSP_CHECKERR   0x02
#define DOWN_RSP_PARERR     0x03
#define DOWN_RSP_UNSUPPORT  0x04

#define LOAD_NEW_IMG      	0x00

#define COMMIT_NEW_IMG      0x00
#define ACTIVE_MARK         0x00
#define ACTIVE_REBOOT       0x01

#define FILENAME_OFFSET		2
/* #define FILENAME_OFFSET		12 */

#define ONU_USR_ID_LEN      24
#define ONU_PASS_WD_LEN     12
#define ONU_AUTHCODE_START      0x10
#define ONU_AUTHCODE_REQ        0x01
#define ONU_AUTHCODE_RSP        0x02
#define ONU_AUTHCODE_SUCCESS    0x03
#define ONU_AUTHCODE_FAIL       0x04
#define ONU_AUTHTYPE_IDPW       0x01

/* CTC ONU type definition */
#define OAM_CTC_ONU_TYPE_SFU   0x00000000
#define OAM_CTC_ONU_TYPE_HGU   0x00000001
#define OAM_CTC_ONU_TYPE_SBU   0x00000002
#define OAM_CTC_ONU_TYPE_MDU_1 0x00000003
#define OAM_CTC_ONU_TYPE_MDU_2 0x00000004
#define OAM_CTC_ONU_TYPE_MDU_3 0x00000005
#define OAM_CTC_ONU_TYPE_MDU_4 0x00000006
#define OAM_CTC_ONU_TYPE_MTU   0x00000007

/* CTC ONU multi-LLID definition */
#define OAM_CTC_SINGLE_LLID  0x1
#define OAM_CTC_MULTI_LLID	 0x0

/* CTC ONU protection type definition */
#define OAM_CTC_PROTECTION_NONE   0x0
#define OAM_CTC_PROTECTION_TYPE_C 0x1
#define OAM_CTC_PROTECTION_TYPE_D 0x2

/* CTC module type definition */
#define OAM_CTC_MODULE_GE    0x0
#define OAM_CTC_MODULE_FE    0x1
#define OAM_CTC_MODULE_VOIP  0x2
#define OAM_CTC_MODULE_TDM   0x3
#define OAM_CTC_MODULE_ADSL2 0x4
#define OAM_CTC_MODULE_VDSL2 0x5
#define OAM_CTC_MODULE_WIFI  0x6
#define OAM_CTC_MODULE_USB   0x7

#define EOAM_DHCP_CLIENT_LEN 512


#define OAM_CLF_EQUAL           0x1/* 014043 */
#define OAM_CLF_ALWAYS_MATCH    0x7

typedef enum LOCAL_LINK_STATUS_e
{
	LINK_FAIL,
	LINK_OK
}LOCAL_LINK_STATUS_t;

enum local_pdu_state_t {
	PDU_LF_INFO = 1,
	PDU_RX_INFO,
	PDU_INFO,
	PDU_ANY,
	PDU_ANY_NO_Ext
};
enum oam_mode_t{
	OAM_ACTIVE = 1,
	OAM_PASSIVE
};

enum discovery_state_t {
	DISCOVERY_FAULT = 1,
	DISCOVERY_ACTIVE_SEND_LOCAL,
	DISCOVERY_PASSIVE_WAIT,
	DISCOVERY_SEND_LOCAL_REMOTE,
	DISCOVERY_SEND_LOCAL_REMOTE_OK,
	DISCOVERY_SEND_ANY
};

typedef enum EX_DISCOVERY_STATE_e
{
	EX_DISCOVERY_PASSIVE_WAIT = DISCOVERY_SEND_ANY + 1,
	EX_DISCOVERY_SEND_EXT_INFO,
	EX_DISCOVERY_SEND_OAM_EXT_INFO_ACK,
	EX_DISCOVERY_SEND_OAM_EXT_INFO_NACK,
	EX_DISCOVERY_SEND_ANY,
	EX_DISCOVERY_SEND_ANY_NO_EXT
}EX_DISCOVERY_STATE_t;

enum eopl_oam_event_t {
	EOPL_LOCAL_LINK_STATUS_CHANGE = 1,
	EOPL_PDU_TIMER,
	EOPL_LOST_LINK_TIMER,
	EOPL_OAM_PDU_RX
};

typedef enum
{
    CTC_STACK_TECHNOLOGY_ABILITY_UNDEFINED      = 1,
    CTC_STACK_TECHNOLOGY_ABILITY_UNKNOWN        = 2,
    CTC_STACK_TECHNOLOGY_ABILITY_10_BASE_T      = 14,
    CTC_STACK_TECHNOLOGY_ABILITY_10_BASE_TFD    = 142,
    CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_T4    = 23,
    CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_TX    = 25,
    CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_TXFD  = 252,
    CTC_STACK_TECHNOLOGY_ABILITY_FDX_PAUSE      = 312,
    CTC_STACK_TECHNOLOGY_ABILITY_FDX_A_PAUSE    = 313,
    CTC_STACK_TECHNOLOGY_ABILITY_FDX_S_PAUSE    = 314,
    CTC_STACK_TECHNOLOGY_ABILITY_FDX_B_PAUSE    = 315,
    CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_T2    = 32,
    CTC_STACK_TECHNOLOGY_ABILITY_100_BASE_T2FD  = 322,
    CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_X    = 36,
    CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_XFD  = 362,
    CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_T    = 40,
    CTC_STACK_TECHNOLOGY_ABILITY_1000_BASE_TFD  = 402,
    CTC_STACK_TECHNOLOGY_ABILITY_REM_FAULT1     = 37,
    CTC_STACK_TECHNOLOGY_ABILITY_REM_FAULT2     = 372,
    CTC_STACK_TECHNOLOGY_ABILITY_ISO_ETHERNET   = 8029,
} CTC_STACK_technology_ability_t;

#define SET_BITS(dest, mask)   \
    { \
        (dest) |= (mask); \
    } \

#define CLEAR_BITS(dest, mask)   \
    { \
        (dest) &= ~(mask); \
    } \

#define WRITE_BITS(dest, mask, value)   \
    { \
        (dest) &= ~(mask); \
        (dest) |= (value) & (mask); \
    } \

#define READ_BITS(src, mask)    \
    ( \
        (src) & (mask) \
    ) \

#define TEST_BITS(src, mask, value) \
    ( \
        ((src) & (mask)) == (value) ? TRUE : FALSE \
    ) \

#define TOGGLE_BITS(dest, mask)   \
    { \
        (dest) = ((dest) & ~(mask)) | ((~dest) & (mask)); \
    } \

#define OAMDBG(string) {\
                                            if(g_iOamSetDbg)\
                                            {\
                                            	printf string;\
                                            }\
                                           }


/* flags in the OAMS format:
15:7 reserved
6	remote stable
5   remote evaluating
4   local stable
3   local evaluating
2   critical event
1   dying gasp
0   link fault
*/
struct oam_info_tlv {
	u8_t	info_type;
	u8_t	info_length;
	u8_t	oam_version;
	u16_t	revision;
	u8_t	state;
	u8_t	oam_config;
	u16_t	oampdu_config;
	u8_t	oui[3];
	u8_t	vendor_info[4];
}__attribute__((packed));
typedef struct oam_info_tlv oam_info_tlv_t;

struct EX_OAM_INFO_TLV_s
{
	u8_t	ucType;
	u8_t	ucLength;
	u8_t	aucOui[3];
	u8_t	ucSupport;
	u8_t	ucVersion;
	u8_t	aucVersionList[1];
}__attribute__((packed));
typedef struct EX_OAM_INFO_TLV_s EX_OAM_INFO_TLV_t;

struct oam_state
{
	enum local_pdu_state_t	local_pdu;
	enum oam_mode_t			local_oam_mode;
	bool_t					local_link_timer_running;
	u8_t					action_state; /* parser multiplexer action */
	u8_t					remote_action_state;
	u16_t					flags;
	u8_t					oam_config;
	u8_t					remote_oam_config;
	u16_t					oampdu_config;
	u16_t					remote_oampdu_config;
	u8_t					state;	/* state machine state */
	bool_t					OAM_Ext_Status;
	bool_t					remote_state_valid;
	bool_t					remote_oui_satisfied;
	bool_t					local_satisfied;
	bool_t					remote_stable;
	bool_t					local_link_status;
	bool_t					local_stable;
	u16_t					usLInfoTlvVer;
	u16_t					usRInfoTlvVer;
	u8_t					ucOamVersion;
	u8_t					ucRmtOamVer;
	u8_t					aucOui[3];
	u8_t					aucRmtOui[3];
	u8_t					ucExVer;
	u8_t					aucVendor[4];
	u8_t					aucRmtVendor[4];
	u8_t					aucExOuiSpt[3];
	u8_t					ucExOuiVer;
};
typedef struct oam_state oam_sate_t;

/* 18 bytes */
struct oampdu_hdr {
	u8_t	dest[6];
	u8_t	src[6];
	u8_t	length[2];
	u8_t	subtype;
	u16_t	flags;
	u8_t	pcode;
}__attribute__((packed));


typedef struct oampdu_hdr oampdu_hdr_t;


struct ctc_header {
	u8_t	oui[3];
	u8_t	ext_code;
}__attribute__((packed));
typedef struct ctc_header ctc_header_t;

struct ctc_tftp_header{
	u8_t data_type;
	u16_t len;
	u16_t tid;
}__attribute__((packed));
typedef struct ctc_tftp_header ctc_tftp_header_t;


struct oam_variable_container {
	u8_t	variable_branch;
	u16_t vairable_leaf;
	u8_t	width;
	u8_t	value[4];
}__attribute__((packed));
typedef struct oam_variable_container oam_variable_container_t;

enum oam_loopback_cmd{
	oam_loopback_enable = 1,
	oam_loopback_disable
};
typedef enum oam_loopback_cmd oam_loopback_cmd_t;

struct oam_onu_sn {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	vendor_id[4];
	u8_t	model[4];
	u8_t	onu_id[6];
	u8_t	hware_ver[8];
	u8_t	sware_ver[16];
}__attribute__((packed));
typedef struct oam_onu_sn oam_onu_sn_t;

struct oam_fware_ver {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	fware_ver[2]; /* [16] */
}__attribute__((packed));
typedef struct oam_fware_ver oam_fware_ver_t;

struct oam_chipset_id {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	vendor_id[2];
	u8_t	model[2];
	u8_t	revision;
	u8_t	ic_version[3];
}__attribute__((packed));
typedef struct oam_chipset_id oam_chipset_id_t;

struct oam_onu_capabilites {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	serv_supported;
	u8_t	number_GE;
	u8_t	bitmap_GE[8];
	u8_t	number_FE;
	u8_t	bitmap_FE[8];
	u8_t	number_POTS;
	u8_t	number_E1;
	u8_t	number_usque;
	u8_t	quemax_us;
	u8_t	number_dsque;
	u8_t	quemax_ds;
	u8_t	batteryBackup;
}__attribute__((packed));
typedef struct oam_onu_capabilites oam_onu_capabilites_t;

struct oam_rsp_indication {
	u8_t	branch;
	u16_t	leaf;
	u8_t	var_indication;
}__attribute__((packed));
typedef struct oam_rsp_indication oam_rsp_indication_t;


struct oam_onu_capabilites2 {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	onu_type;
	u8_t	multiLlid;
	u8_t    protection_type;
	u8_t    number_of_PONIf;
	u8_t    number_of_slot;
	u8_t 	number_of_interface_type;
	u32_t   interface_type;
	u16_t   number_of_port;
	u8_t    battery_backup;
}__attribute__((packed));
typedef struct oam_onu_capabilites2 oam_onu_capabilites2_t;
struct oam_tw_onu_capabilites2{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	onu_type;
	u8_t	multiLlid;
	u8_t    protection_type;
	u8_t    number_of_PONIf;
	u8_t    number_of_slot;
	u8_t 	number_of_interface_type;
	u32_t   fe_interface_type;
	u16_t   number_of_fe_port;
	u32_t   voip_interface_type;
	u16_t   number_of_voip_port;
	u8_t    battery_backup;
}__attribute__((packed));
typedef struct oam_tw_onu_capabilites2 oam_tw_onu_capabilites2_t;

struct oam_onu_holdover_config{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t 	holdover_state;
	u32_t 	holdover_time;
}__attribute__((packed));
typedef struct oam_onu_holdover_config oam_onu_holdover_config_t;

struct oam_active_pon_if_admin{
	u8_t 	branch;
	u16_t 	leaf;
	u8_t	width;
	u8_t	active_port_id;
}__attribute__((packed));
typedef struct oam_active_pon_if_admin oam_active_pon_if_admin_t;

struct oam_optical_transceiver_diag{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u16_t	temperature;
	u16_t	supply_vcc;
	u16_t	tx_bias_current;
	u16_t	tx_power;
	u16_t	rx_power;
}__attribute__((packed));
typedef struct oam_optical_transceiver_diag oam_optical_transceiver_diag_t;

struct oam_service_sla{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t    operation;
}__attribute__((packed));
typedef struct oam_service_sla oam_service_sla_t;

struct oam_service_sla_body_hdr{
	u8_t    schedule_scheme;
	u8_t    high_pri_boundry;
	u32_t   cycle_len;
	u8_t    service_num;
}__attribute__((packed));
typedef struct oam_service_sla_body_hdr oam_service_sla_body_hdr_t;

struct oam_service_sla_body_entry {
	u8_t	queue_id;
	u16_t	fix_size;
	u16_t	fix_bw;
	u16_t	guaranteed_bw;
	u16_t	best_effor_bw;
	u8_t    wrr_weight;
}__attribute__((packed));
typedef struct oam_service_sla_body_entry oam_service_sla_body_entry_t;


struct oam_eth_linkstate {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	linkstate;
}__attribute__((packed));
typedef struct oam_eth_linkstate oam_eth_linkstate_t;

struct oam_eth_portpause {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	back_oper;
}__attribute__((packed));
typedef struct oam_eth_portpause oam_eth_portpause_t;

struct oam_eth_portpolicing {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	operation;
	u8_t	cir[3];
	u8_t	cbs[3];
	u8_t	ebs[3];
}__attribute__((packed));
typedef struct oam_eth_portpolicing oam_eth_portpolicing_t;

struct oam_voip_port{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	lock;
}__attribute__((packed));
typedef struct oam_voip_port oam_voip_port_t;

struct oam_e1_port{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	lock;
}__attribute__((packed));
typedef struct oam_e1_port oam_e1_port_t;

struct oam_eth_portdspolicing {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	operation;
	u8_t	cir[3];
	u8_t	pir[3];
}__attribute__((packed));
typedef struct oam_eth_portdspolicing oam_eth_portdspolicing_t;

struct oam_vlan {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	mode;
	u8_t	pars[12];
}__attribute__((packed));
typedef struct oam_vlan oam_vlan_t;
struct oam_clasmark {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	action;
	u8_t	numOfRule;
}__attribute__((packed));
typedef struct oam_clasmark oam_clasmark_t;

struct oam_clasmark_rulebody {
    u8_t    precedenceOfRule;
    u8_t    lenOfRule;
    u8_t    queueMapped;
    u8_t    ethPriMark;
    u8_t    numOfField;
}__attribute__((packed));
typedef struct oam_clasmark_rulebody oam_clasmark_rulebody_t;

struct oam_clasmark_fieldbody {
	u8_t	fieldSelect;
	u8_t	matchValue[6];
	u8_t	operator;
}__attribute__((packed));
typedef struct oam_clasmark_fieldbody oam_clasmark_fieldbody_t;

struct oam_mcast_vlan {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	operation;
}__attribute__((packed));
typedef struct oam_mcast_vlan oam_mcast_vlan_t;

struct oam_mcast_tagstripe {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	tagstriped;
}__attribute__((packed));
typedef struct oam_mcast_tagstripe oam_mcast_tagstripe_t;

struct oam_mcast_vlan_translation_entry{
	u16_t   mcast_vlan;
	u16_t   iptv_vlan;
}__attribute__((packed));
typedef struct oam_mcast_vlan_translation_entry oam_mcast_vlan_translation_entry_t;

struct oam_mcast_switch {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	action;
}__attribute__((packed));
typedef struct oam_mcast_switch oam_mcast_switch_t;

struct oam_mcast_control {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	action;
	u8_t	control_type;
	u8_t	number;
}__attribute__((packed));
typedef struct oam_mcast_control oam_mcast_control_t;

struct oam_mcast_control_entry {
	u16_t	port_id;
	u16_t	vlan_id;
	u8_t	gda[6];
}__attribute__((packed));
typedef struct oam_mcast_control_entry oam_mcast_control_entry_t;

struct oam_group_num_max {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	num_max;
}__attribute__((packed));
typedef struct oam_group_num_max oam_group_num_max_t;

struct oam_fastleave_abl {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	num_of_mode;
	u32_t   mode_1; /* 	u32_t   fastLeaveMode1;
 */
	u32_t   mode_2;
	u32_t   mode_3;
	u32_t   mode_4;
}__attribute__((packed));
typedef struct oam_fastleave_abl oam_fastleave_abl_t;

struct oam_fastleave_adm {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	value;
}__attribute__((packed));
typedef struct oam_fastleave_adm oam_fastleave_adm_t;

struct oam_multi_llid_ctrl{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t   llid_num;
}__attribute__((packed));
typedef struct oam_multi_llid_ctrl oam_multi_llid_ctrl_t;

struct oam_alarm_admin_hdr{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
}__attribute__((packed));
typedef struct oam_alarm_admin_hdr oam_alarm_admin_hdr_t;

struct oam_alarm_admin_entry{
	u16_t   alarm_id;
	u32_t   alarm_config;
}__attribute__((packed));
typedef struct oam_alarm_admin_entry oam_alarm_admin_entry_t;

struct oam_alarm_threshold_hdr{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
}__attribute__((packed));
typedef struct oam_alarm_threshold_hdr oam_alarm_threshold_hdr_t;

struct oam_alarm_threshold_entry{
	u16_t   alarm_id;
	i32_t   raising_threshold;
	i32_t   falling_threshold;
}__attribute__((packed));
typedef struct oam_alarm_threshold_entry oam_alarm_threshold_entry_t;

struct alarm_cfg{
	u16_t alarm_id;
	u32_t enable_status;
	u32_t raising_threshold;
	u32_t falling_threshold;
}__attribute__((packed));
typedef struct alarm_cfg alarm_cfg_t;


struct oam_iad_info{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    mac[6];
	u8_t    protocol_support;
	u8_t    iad_sw_version[32];
	u8_t    iad_sw_time[32];
	u8_t    voip_user_count;
}__attribute__((packed));
typedef struct oam_iad_info oam_iad_info_t;

struct oam_global_para_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    voice_ip_mode;
	u32_t   iad_ipaddr;
	u32_t   iad_netmask;
	u32_t   iad_defaultgw;
	u8_t    pppoe_mode;
	u8_t    pppoe_username[32];
	u8_t    pppoe_passwd[32];
	u8_t    tagged_flag;
	u16_t   voice_cvlan;
	u16_t   voice_svlan;
	u8_t    voice_priority;
}__attribute__((packed));
typedef struct oam_global_para_cfg oam_global_para_cfg_t;

struct oam_h248_para_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u16_t   mg_port_no;
	u32_t   mgc_ip;
	u16_t   mgc_com_port_no;
	u32_t   backup_mgc_ip;
	u16_t   backup_mgc_com_port_no;
	u8_t    active_mgc;
	u8_t    reg_mode;
	u8_t    mid[64];
	u8_t    heart_beat_mode;
	u16_t 	heart_beat_cycle;
	u8_t	heart_beat_count;
}__attribute__((packed));
typedef struct oam_h248_para_cfg oam_h248_para_cfg_t;

struct oam_h248_user_tid_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    user_tid_name[16];
}__attribute__((packed));
typedef struct oam_h248_user_tid_cfg oam_h248_user_tid_cfg_t;

struct oam_h248_rtp_tid_cfg{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	num_of_rtp_tid;
	u8_t	rtp_tid_prefix[16];
	u8_t	rtp_tid_digit_begin[8];
	u8_t	rtp_tid_mode;
	u8_t	rtp_tid_digit_len;
}__attribute__((packed));
typedef struct oam_h248_rtp_tid_cfg oam_h248_rtp_tid_cfg_t;

struct oam_h248_rtp_tid_info{
	u8_t 	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	num_of_rtp_tid;
	u8_t	first_rtp_tid_name[32];
}__attribute__((packed));
typedef struct oam_h248_rtp_tid_info oam_h248_rtp_tid_info_t;

struct oam_sip_para_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u16_t   mg_port_no;
	u32_t   sip_proxy_server_ip;
	u16_t   sip_proxy_server_com_port_no;
	u32_t   backup_sip_proxy_serv_ip;
	u16_t   backup_sip_proxy_serv_com_port_no;
	u32_t	active_sip_proxy_server;
	u32_t   sip_reg_server_ip;
	u16_t   sip_reg_server_com_port_no;
	u32_t   backup_sip_reg_server_ip;
	u16_t   backup_sip_reg_server_com_port_no;
	u32_t   outbound_server_ip;
	u16_t   outbound_server_port_no;
	u32_t   sip_reg_interval;
	u8_t    heartbeat_switch;
	u16_t   heartbeat_cycle;
	u16_t   heartbeat_count;
}__attribute__((packed));
typedef struct oam_sip_para_cfg oam_sip_para_cfg_t;

struct oam_sip_user_para_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    user_account[16];
	u8_t    user_name[32];
	u8_t    user_passwd[16];
}__attribute__((packed));
typedef struct oam_sip_user_para_cfg oam_sip_user_para_cfg_t;

struct oam_fax_modem_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t   voice_t38_enable;
	u8_t   voice_fax_modem_control;
}__attribute__((packed));
typedef struct oam_fax_modem_cfg oam_fax_modem_cfg_t;

struct oam_h248_iad_oper_status{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   iad_oper_status;
}__attribute__((packed));
typedef struct oam_h248_iad_oper_status oam_h248_iad_oper_status_t;

struct oam_pots_status{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	iad_port_status;
	u32_t	iad_port_service_state;
	u32_t	iad_port_codec_mode;
}__attribute__((packed));
typedef struct oam_pots_status oam_pots_status_t;

struct oam_iad_operation{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	iad_operation;
}__attribute__((packed));
typedef struct oam_iad_operation oam_iad_operation_t;

struct oam_sip_digit_map{
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	sip_digit_map[1024];
}__attribute__((packed));
typedef struct oam_sip_digit_map oam_sip_digit_map_t;


struct oam_llid_queue_cfg{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    num_of_queue;
}__attribute__((packed));
typedef struct oam_llid_queue_cfg oam_llid_queue_cfg_t;

struct oam_llid_queue_cfg_entry{
	u16_t queue_id;
	u16_t queue_weight;
}__attribute__((packed));
typedef struct oam_llid_queue_cfg_entry oam_llid_queue_cfg_entry_t;

struct oam_mxu_mng_global_para{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u32_t   mng_ip_addr;
	u32_t   mng_ip_mask;
	u32_t   mng_gw;
	u16_t   mng_data_cvlan;
	u16_t   mng_data_svlan;
	u8_t    mng_data_priority;
}__attribute__((packed));
typedef struct oam_mxu_mng_global_para oam_mxu_mng_global_para_t;

struct oam_mxu_mng_snmp_para{
	u8_t    branch;
	u16_t   leaf;
	u8_t    width;
	u8_t    snmp_ver;
	u32_t   trap_host_ip;
	u16_t   trap_port;
	u16_t   snmp_port;
	u8_t    security_name[32];
	u8_t    community_for_read[32];
	u8_t	community_for_write[32];
}__attribute__((packed));
typedef struct oam_mxu_mng_snmp_para oam_mxu_mng_snmp_para_t;

struct oam_phy_adminstate {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	value;
}__attribute__((packed));
typedef struct oam_phy_adminstate oam_phy_adminstate_t;

struct oam_phy_admincontrol {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	value;
}__attribute__((packed));
typedef struct oam_phy_admincontrol oam_phy_admincontrol_t;

struct oam_autoneg_adminstate {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	value;
}__attribute__((packed));
typedef struct oam_autoneg_adminstate oam_autoneg_adminstate_t;

struct oam_autoneg_localability {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	number;
	u32_t	first;
	u32_t	second;
	u32_t	third;
	u32_t	fourth;
	u32_t	fifth;
	u32_t	sixth;
	u32_t	seventh;
	u32_t	eighth;	
}__attribute__((packed));
typedef struct oam_autoneg_localability oam_autoneg_localability_t;

struct oam_autoneg_advability {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	number;
	u32_t	first;
	u32_t	second;
	u32_t	third;
	u32_t	fourth;
	u32_t	fifth;
	u32_t	sixth;
	u32_t	seventh;
	u32_t	eighth;
}__attribute__((packed));
typedef struct oam_autoneg_advability oam_autoneg_advability_t;

struct oam_autoneg_restart {
	u8_t	branch;
	u16_t	leaf;
}__attribute__((packed));
typedef struct oam_autoneg_restart oam_autoneg_restart_t;

struct oam_autoneg_admincontrol {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	indication;
}__attribute__((packed));
typedef struct oam_autoneg_admincontrol oam_autoneg_admincontrol_t;

struct oam_fec_capability {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	indication;
}__attribute__((packed));
typedef struct oam_fec_capability oam_fec_capability_t;

struct oam_fec_mode {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	value;
}__attribute__((packed));
typedef struct oam_fec_mode oam_fec_mode_t;

struct oam_instance_tlv {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t	value;
}__attribute__((packed));
typedef struct oam_instance_tlv oam_instance_tlv_t;

struct oam_instance_tlv_2_0 {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	value;
}__attribute__((packed));
typedef struct oam_instance_tlv_2_0 oam_instance_tlv_t_2_0;

struct oam_variable_descriptor {
	u8_t	branch;
	u16_t	leaf;
}__attribute__((packed));
typedef struct oam_variable_descriptor oam_variable_descriptor_t;

struct oam_set_response {
	u8_t	branch;
	u16_t	leaf;
	u8_t	indication;
}__attribute__((packed));
typedef struct oam_set_response oam_set_response_t;

struct oam_dba_queue {
	u8_t	bitmap;
	u16_t	threshold[8];
}__attribute__((packed));
typedef struct oam_dba_queue oam_dba_queue_t;

struct oam_port_loop_detect{
	u8_t 	branch;
	u16_t 	leaf;
	u8_t	width;
	u32_t 	state;
}__attribute__((packed));
typedef struct oam_port_loop_detect oam_port_loop_detect_t;

struct oam_vlan_config {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u8_t	mode;
}__attribute__((packed));
typedef struct oam_vlan_config oam_vlan_config_t;
struct oam_vlan_translation {
	u32_t	old_entry;
	u32_t   new_entry;
}__attribute__((packed));
typedef struct oam_vlan_translation oam_vlan_translation_t;

struct oam_ctc_auth_req{
	u8_t    auth_code;
	u16_t   datalen;
	u8_t    auth_type;
}__attribute__((packed));
typedef struct oam_ctc_auth_req oam_ctc_auth_req_t;

struct oam_ctc_auth_rsp {
	u8_t	auth_code;
	u16_t	data_len;
	u8_t    auth_type;
	u8_t	loid[24];
	u8_t    password[12];
}__attribute__((packed));
typedef struct oam_ctc_auth_rsp oam_ctc_auth_rsp_t;

struct oam_onu_tx_power_supply_ctrl {
	u8_t	branch;
	u16_t	leaf;
	u8_t	width;
	u32_t   action;
	u8_t	onu_id[6];
	u32_t	optical_transmitter_id;
}__attribute__((packed));
typedef struct oam_onu_tx_power_supply_ctrl oam_onu_tx_power_supply_ctrl_t;

typedef struct TIMER_UNIT_s
{
	volatile u32_t	uiCnt;
	int				iFlag;
	volatile int	iStart;
}TIMER_UNIT_t;

typedef struct TFTP_DATA_s
{
	u8_t aucDataBlock[1400];
	u16_t usLen;
	struct TFTP_DATA_s *pstNext;
}TFTP_DATA_t;

typedef struct SOFT_DOWNLOAD_HD_MASK_s
{
	u8_t ucType;
	u16_t usLen;
	u16_t usTid;
	u8_t aucMsg[1];
}__attribute__((packed))SOFT_DOWNLOAD_HD_MASK_t;

typedef struct TFTP_FILE_s
{
	u8_t	aucFileName[256];
	u8_t	ucNameLen;
	u16_t	usBlock1;
    u16_t	usBlock2;
	u8_t	ucDownload;
	u8_t	ucFlash;
	u32_t	uiFileSize;
	TFTP_DATA_t *pstData;
	TFTP_DATA_t *pstDataEnd;
}TFTP_FILE_t;

struct oam_envet_hdr{
	u8_t event_type;
	u8_t event_len;
	u16_t event_stamp;    
}__attribute__((packed));
typedef struct oam_envet_hdr oam_envet_hdr_t;


struct alarm_oampdu_hdr {
	u8_t	dest[6];
	u8_t	src[6];
	u8_t	length[2];
	u8_t	subtype;
	u16_t	flags;
	u8_t	pcode;
    u16_t   seqno;
}__attribute__((packed));
typedef struct alarm_oampdu_hdr alarm_oampdu_hdr_t;

struct oam_err_sym_period_hdr{
	u64_t sym_window;
	u64_t sym_thrsh;
	u64_t sym_err_value;
	u64_t err_total;
    u32_t event_total;
}__attribute__((packed));
typedef struct oam_err_sym_period_hdr oam_err_sym_period_hdr_t;

struct oam_err_frm_period_hdr{
	u32_t frm_window;
	u32_t frm_thrsh;
	u32_t frm_err_value;
	u64_t err_total;
    u32_t event_total;
}__attribute__((packed));
typedef struct oam_err_frm_period_hdr oam_err_frm_period_hdr_t;

struct oam_err_frm_hdr{
    u16_t frm_interval_window;
	u32_t frm_thrsh;
	u32_t frm_err_value;
	u64_t err_total;
    u32_t event_total;
}__attribute__((packed));
typedef struct oam_err_frm_hdr oam_err_frm_hdr_t;

struct oam_err_frm_sec_hdr{
    u16_t frm_sec_window;
	u16_t frm_sec_thrsh;
	u16_t frm_sec_err_value;
	u32_t sec_err_total;
    u32_t event_total;
}__attribute__((packed));
typedef struct oam_err_frm_sec_hdr oam_err_frm_sec_hdr_t;

typedef struct alarm_oam_window
{
    u64_t ullSym_err_prd_window;
    u32_t ulFrm_err_prd_window;
    u16_t usErr_frm_window;
    u16_t usErr_frm_sec_window;
}alarm_oam_window_t;

typedef struct alarm_oam_thrsh
{
    u64_t ullSym_err_prd_thrsh;
    u32_t ulFrm_err_prd_thrsh;
    u32_t ulErr_frm_thrsh;
    u16_t usErr_frm_sec_thrsh;
}alarm_oam_thrsh_t;

typedef struct
{
  u8_t   ucDataLength; /* Number of shorts */
  u32_t  ulData;
} attr_statistics_t;


#if 0
typedef struct CLS_CONFIG_INFO_s
{
	u8_t valid;
    u8_t  rulePri;

    u32_t dstMacFlag:1;
    u32_t srcMacFlag:1;
    u32_t etherPriFlag:1;
    u32_t vlanIdFlag:1;
    u32_t dstIpFlag:1;
    u32_t srcIpFlag:1;
    u32_t ipTypeFlag:1;
    u32_t ipV4DscpFlag:1;
    u32_t ipV6PreFlag:1;
    u32_t srcL4PortFlag:1;
    u32_t dstL4PortFlag:1;
    u32_t rsvFlag:21;

	struct {
		u8_t lowRange[6];
		u8_t highRange[6];
	}dstMac;

	struct {
		u8_t lowRange[6];
		u8_t highRange[6];
	}srcMac;

	struct {
		u8_t lowRange;
		u8_t highRange;
	}etherPri;

	struct {
		u16_t lowRange;
		u16_t highRange;
	}vlanId;

	struct {
		u32_t lowRange;
		u32_t highRange;
	}dstIp;

	struct {
		u32_t lowRange;
		u32_t highRange;
	}srcIp;

	struct {
		u8_t lowRange;
		u8_t highRange;
	}ipType;

	struct {
		u8_t lowRange;
		u8_t highRange;
	}ipV4Dscp;

	struct {
		u8_t lowRange;
		u8_t highRange;
	}ipV6Pre;

	struct {
		u16_t lowRange;
		u16_t highRange;
	}srcL4Port;

	struct {
		u16_t lowRange;
		u16_t highRange;
	}dstL4Port;

    u8_t dstMacOp;
    u8_t srcMacOp;
    u8_t etherPriOp;
    u8_t vlanIdOp;
    u8_t dstIpOp;
    u8_t srcIpOp;
    u8_t ipTypeOp;
    u8_t ipV4DscpOp;
    u8_t ipV6PreOp;
    u8_t srcL4PortOp;
    u8_t dstL4PortOp;
}CLS_CONFIG_INFO_t;
#endif
typedef enum
{
	ONU_DSTMAC,
	ONU_SRCMAC,
	ONU_COS,
	ONU_VID,
	ONU_TYPE,
	ONU_DSTIP,
	ONU_SRCIP,
	ONU_IPPRO,
	ONU_IPTOS,
	ONU_IPPRE,
	ONU_SRCPORT,
	ONU_DSTPORT
}ONU_CLASS_FIELD_ID;

enum OAM_VER_SUPPORT_e{
    OAM_VER_CTC_0_1_SUPPORT ,
    OAM_VER_CTC_2_0_SUPPORT ,
    OAM_VER_CTC_2_1_SUPPORT ,
    OAM_VER_CTC_20_21_MIX_SUPPORT ,    /* Support  both version 2.0 and 2.1 */
    OAM_VER_NTT_SUPPORT ,
    OAM_VER_MAX ,
};

typedef enum OAM_VER_SUPPORT_e OAM_VER_SUPPORT_t;
extern int eopl_oam_pdu_receive (u8_t *pucFrameIn, u16_t usFrameLen);
/*
  * Function:
  * eopl_oam_info_tlv
  * Purpose:
  * routine to set the oam local info tlv
  * Parameters:
  * Input - ptr,type
  *			ptr - pointer to the data buffer of the local info
  *			type - type to be set for this lcoal info
  * Output  - none
  * Returns:
  *     None
  */
extern void eopl_oam_info_tlv(u8_t *ptr, u8_t type);
/*
  * Function:
  * eopl_oam_pdu_hdr
  * Purpose:
  * routine to set the oam pdu header
  * Parameters:
  * Input - ptr,flags,pcode
  *			ptr - pointer to the data buffer of pdu header
  *			flags - flags to be set for this pdu header
  *			pcode - code to be set for this pdu header
  * Output  - none
  * Returns:
  *     None
  */

extern void eopl_oam_pdu_hdr(u8_t *ptr, u16_t flags, u8_t pcode);
/*
  * Function:
  * oam_pdu_padding
  * Purpose:
  * fault state routine of oam state machine
  * Parameters:
  * Input - ptr,len
  *			ptr - pointer to the part to be padded  with zero of the oam pdu
  *			len - len of the part to be padded  with zero of the oam pdu
  * Output  - none
  * Returns:
  *     None
  */
extern void oam_pdu_padding( u8_t *ptr, u16_t len );

/*
  * Function:
  * eopl_oam_fault_state
  * Purpose:
  * fault state routine of oam state machine
  * Parameters:
  * Input - event
  *			input event for the fault state
  * Output  - none
  * Returns:
  *     None
  */
extern void eopl_oam_fault_state(enum eopl_oam_event_t event);
/*
  * Function:
  * eopl_oam_timer_process
  * Purpose:
  * timer routine for oam state machine
  * Parameters:
  * Input - none
  * Output  - none
  * Returns:
  *     None
  */
extern void eopl_oam_timer_process(void);
/*
  * Function:
  * eopl_oam_init
  * Purpose:
  * init routine for oam state machine
  * Parameters:
  * Input - none
  * Output  - none
  * Returns:
  *     None
  */
extern void eopl_oam_init(void);
/*
  * Function:
  * eopl_oam_pdu_process
  * Purpose:
  * process routine for incoming oam pdu
  * Parameters:
  * Input - none
  * Output  - none
  * Returns:
  *     None
  */
extern void eopl_oam_pdu_process(u16_t framelen);
/*
  * Function:
  * eopl_oam_state_machine
  * Purpose:
  * 802.3 oam state machine
  * Parameters:
  * Input - event
  *		event - input event to the state machine;
  * Output  - none
  * Returns:
  *     None
  */
extern void eopl_oam_state_machine(enum eopl_oam_event_t event);
extern void eopl_ctc_pdu_process(u8_t *p_byte,u16_t length);
extern int OamFrameSend(u8_t *pucFrame, u16_t usFrmLen);
extern int eopl_set_onu_classify_set(u8_t*  p_in, u8_t ucPort);
extern int StartLinkLostTimer (void);
extern int StopLinkLostTimer (void);
extern void LinkLostHandler (int signal_number);
extern int OamDbgEnable (u8_t ucEnable);
extern int OamVarReq (u8_t *pucData);
extern int OamLoopback(u8_t *pucData);
extern int OamVarAttr (u8_t *pucData);
extern void OamGetFrmTxOk(void);
extern int OamEnableLp(void);
extern int OamDisableLp(void);
extern void OamTimerThread(void);
extern void TimerUnit(int signal);
extern int OamOplRstCfg(u8_t ucEndis, u16_t usFlag, u8_t ucCode, u32_t uiPayLoad);
extern int OamDygaspCfg(void);
extern int OamStatusGet(u8_t *pState);
#if 0
#define OAM_DBG(FMT, ...)  \
    do { \
        sysLogDebug(MODULE, DEBUG_LEVEL_DEBUGGING, FMT, ##__VA_ARGS__); \
    } while (0)

#define OAM_INFO(FMT, ...) \
    do { \
         sysLogDebug(MODULE, DEBUG_LEVEL_DEBUGGING, FMT, ##__VA_ARGS__); \
    } while (0)

#define OAM_WRN(FMT, ...) \
    do { \
        sysLogDebug(MODULE, DEBUG_LEVEL_DEBUGGING, FMT, ##__VA_ARGS__); \
    } while (0)
#define OAM_TRACE() OAM_DBG("\r\n#######     Enter  %s   [%d] #######\r\n",__FUNCTION__,__LINE__ )
#else
#define OAM_DBG(FMT, ...)
#define OAM_INFO(FMT, ...)
#define OAM_WRN(FMT, ...)
#define OAM_TRACE()
#endif

/* Eric Yang add for the OAM-CALLBACK-Function Registered */
#if 0
#ifndef OAM_SUPPORT_REG_CALLBACK
#define OAM_SUPPORT_REG_CALLBACK
#endif
#endif

typedef enum tagOLT_FLAG
{
    OLT_FLAG_CTC = 0,
    OLT_FLAG_H3C,
    OLT_FLAG_BUTT
}OLT_FLAG_E;

typedef enum OAM_CTC_CLS_FIELD_TYPE_s
{
    OAM_CTC_CLS_FIELD_TYPE_DA_MAC,
    OAM_CTC_CLS_FIELD_TYPE_SA_MAC,
    OAM_CTC_CLS_FIELD_TYPE_ETH_PRI,
    OAM_CTC_CLS_FIELD_TYPE_VLAN_ID,
    OAM_CTC_CLS_FIELD_TYPE_ETHER_TYPE,
    OAM_CTC_CLS_FIELD_TYPE_DST_IP,
    OAM_CTC_CLS_FIELD_TYPE_SRC_IP,
    OAM_CTC_CLS_FIELD_TYPE_IP_TYPE,
    OAM_CTC_CLS_FIELD_TYPE_IP_DSCP,
    OAM_CTC_CLS_FIELD_TYPE_IP_PRECEDENCE,
    OAM_CTC_CLS_FIELD_TYPE_L4_SRC_PORT,
    OAM_CTC_CLS_FIELD_TYPE_L4_DST_PORT
}OAM_CTC_CLS_FIELD_TYPE_s;
typedef enum OAM_CTC_CLS_FIELD_OPERATOR_s
{
    OAM_CTC_CLS_FIELD_OPERATOR_NEVER_MATCH,
    OAM_CTC_CLS_FIELD_OPERATOR_EQUAL,
    OAM_CTC_CLS_FIELD_OPERATOR_NOT_EQUAL,
    OAM_CTC_CLS_FIELD_OPERATOR_LESS_OR_EQUAL,
    OAM_CTC_CLS_FIELD_OPERATOR_GRATER_OR_EQUAL,
    OAM_CTC_CLS_FIELD_OPERATOR_EXIST,
    OAM_CTC_CLS_FIELD_OPERATOR_NOT_EXIST,
    OAM_CTC_CLS_FIELD_OPERATOR_ALWAYS_MATCH,
}OAM_CTC_CLS_FIELD_OPERATOR_e;

extern unsigned short m_abAutoNegEnable[CTC_MAX_SUBPORT_NUM + 1];
extern void OamClrPeirodStats(void);

#ifdef OAM_SUPPORT_REG_CALLBACK

#define IN
#define OUT
/*append some dbg trace function */
#define OAM_DBG(FMT, ...)  \
    do { \
        sysLogDebug(MODULE, DEBUG_LEVEL_DEBUGGING, FMT, ##__VA_ARGS__); \
    } while (0)

#define OAM_INFO(FMT, ...) \
    do { \
        sysLogDebug(MODULE, DEBUG_LEVEL_DEBUGGING, FMT, ##__VA_ARGS__); \
    } while (0)

#define OAM_WRN(FMT, ...) \
    do { \
        sysLogDebug(MODULE, DEBUG_LEVEL_DEBUGGING, FMT, ##__VA_ARGS__); \
    } while (0)

#define OAM_TRACE() OAM_DBG("\r\n#######     Enter  %s   [%d] #######\r\n",__FUNCTION__,__LINE__ )

/* the parameter p_byte points to the data area of out_data */
typedef int (* event_callback)(u8_t * p_in_buf,u8_t * p_out_buf);

typedef int (* oam_send_handler)(u8_t * buf,u16_t len) ;

typedef void (* oam_process_entry)(u8_t * buf,u16_t len);
typedef struct
{
    u8_t oam_ver;/* the version of OAM,0--ver 2.0 ,1--ver 2.1 */
    u8_t oam_ext_code;/* Ext Opcode of OAM */
    u16_t oam_leaf_id; /* leaf value of OAMPDU */
    u8_t oam_branch_id; /* branch id of OAMPDU */
    u8_t dummy1;
    u16_t dummy2;
}OAM_EV_CB_ID_t ,*P_OAM_EV_CB_ID_t;

extern u32_t OamRegisterSendHandler(oam_send_handler snd_hdl);


extern u32_t OamEventCalllBackRegister( IN const OAM_EV_CB_ID_t value,
                                         IN const event_callback pcb);
extern u32_t OamEventCalllBackDeregister( IN const OAM_EV_CB_ID_t value);

extern u32_t OamEventCallBackInit(IN const u16_t hash_size);
extern u32_t OamEventCallBackCleanUp();
extern u32_t OamEventQueryCount();
extern u32_t OamEventCallBack(IN const OAM_EV_CB_ID_t value,
                               IN const  u8_t * in_byte,
                               IN OUT u8_t * out_byte);

extern u32_t OamRegisterProcessEntry(u8_t * oui,oam_process_entry entry );

extern u32_t OamDeRegisterProcessEntry(u8_t * oui);
#endif
/* ! EricYang*/

#if defined(CTC_VOIP_DISPATCH)
extern int oamMsqInit(void);
extern int oamMsqDestroy(void);
extern int oamMsqSend(void *msgBuf, unsigned int msgsz);
extern unsigned int oamMsqReceive(void *msgBuf, unsigned int msgsz);
#endif
/* 014035 */
#if !defined(CONFIG_PRODUCT_EPN200)
extern OLT_FLAG_E OamGetOltFlag(void);
#endif
/* 014035 */
#endif /* __OAM_H__ */
