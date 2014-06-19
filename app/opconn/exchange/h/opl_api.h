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
* FILENAME:  opl_api.h
*
* DESCRIPTION: 
*	api called by customer.
*
* Date Created: Apr 30, 2008
*
* Authors(optional): zzhu
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/exchange/h/opl_api.h#1 $
* $Log:$
*
*
**************************************************************************/
#ifndef __OPL_API_H__
#define __OPL_API_H__
#include <opl_type.h>
#include <opl_pal.h>
#include <opl_msg_header.h>
#include <opl_msg_process.h>
#include <opl_msg_send.h>
#include <opl_api_pack.h>

#define OPL_ONU_ALARM_OFFSET    0x00
#define OPL_ONU_EVENT_OFFSET    0x80

/*#define _VXWORKS_*/

typedef enum
{
    OPL_S_OK,
    OPL_S_ERROR,
    OPL_S_INVALID,
    OPL_S_FAIL,
    OPL_S_NOT_SUPPORTED,
    OPL_S_NOT_INITIALIZED,
    OPL_S_OUT_OF_RANGE,
    OPL_S_BAD_PARAM,
    OPL_S_BAD_CONFIGURATION,
    OPL_S_NO_RESOURCES,
    OPL_S_NOT_FOUND,
    OPL_S_ALREADY_EXISTS,
    OPL_S_ILLEGAL_ADDRESS,
    OPL_S_NO_MEMORY,
    OPL_S_DISCARD,
    OPL_S_MISMATCH,
    OPL_S_WRONG_NUM_PARAMS,
    OPL_S_OS_ERROR,
    OPL_S_OVERFLOW,
    OPL_S_TIMEOUT,
    OPL_S_FUTURE_USE,
    OPL_S_LAST_STATUS
}OPL_STATUS_e;

typedef enum 
{
    OPL_LED_PON_RX,
    OPL_LED_PON_LINK,
    OPL_LED_PON_TX,
    OPL_LED_RX_ERR,
    OPL_LED_UNI1,
    OPL_LED_UNI2,
    OPL_LED_UNI3,
    OPL_LED_UNI4
}OPL_LED_e;

typedef enum
{
    OPL_LED_HW_DRIVEN,
    OPL_LED_FW_DRIVEN,
    OPL_LED_OFF,
    OPL_LED_ON,
    OPL_LED_SLOW_BLINK,
    OPL_LED_FAST_BLINK
}OPL_LED_ACT_e;  

typedef enum
{
    OPL_ERRORED_SYMBOLS_PERIOD_ALARM = OPL_ONU_ALARM_OFFSET,
    OPL_ERRORED_FRAME_ALARM          = OPL_ONU_ALARM_OFFSET + 1,
    OPL_ERRORED_FRAME_PERIOD_ALARM   = OPL_ONU_ALARM_OFFSET + 2,
    OPL_ERRORED_SECOND_SUMMURY_ALARM = OPL_ONU_ALARM_OFFSET + 3,
    OPL_DYING_GASP_ALARM             = OPL_ONU_ALARM_OFFSET + 4,
    OPL_LINK_FAULT_ALARM             = OPL_ONU_ALARM_OFFSET + 5,
    OPL_CRITICAL_EVENT_ALARM         = OPL_ONU_ALARM_OFFSET + 6,
    /* add alrams before MAX_ALARM_TYPE*/
    OPL_MAX_ALARM_TYPE,

    OPL_IGMP_JOIN_EVENT              = OPL_ONU_EVENT_OFFSET,
    OPL_IGMP_LEAVE_EVENT             = OPL_ONU_EVENT_OFFSET + 1,
    OPL_ENCRYPTION_KEY_EVENT         = OPL_ONU_EVENT_OFFSET + 2,
    OPL_ENCRYPTION_STATE_EVENT       = OPL_ONU_EVENT_OFFSET + 3,
    OPL_FEC_RX_MODE_EVENT            = OPL_ONU_EVENT_OFFSET + 4,
    OPL_ADDRESS_TBL_FULL_EVENT       = OPL_ONU_EVENT_OFFSET + 5,
    OPL_CONNECTION_STATE_EVENT       = OPL_ONU_EVENT_OFFSET + 6,
    OPL_UNI_LINK_EVENT               = OPL_ONU_EVENT_OFFSET + 7,
    OPL_PON_LOOPBACK_EVENT           = OPL_ONU_EVENT_OFFSET + 8,
    OPL_AUTHORIZATION_EVENT          = OPL_ONU_EVENT_OFFSET + 9,
    OPL_PRE_RESET_EVENT              = OPL_ONU_EVENT_OFFSET + 10,
    OPL_IMAGE_BURN_COMPLETE_EVENT    = OPL_ONU_EVENT_OFFSET + 11,
    OPL_EAPOL_RESPONSE_SENT_EVENT    = OPL_ONU_EVENT_OFFSET + 12,
    OPL_PASONU_RESERVED_EVENT_1      = OPL_ONU_EVENT_OFFSET + 13,
    OPL_PASONU_RESERVED_EVENT_2      = OPL_ONU_EVENT_OFFSET + 14,
    OPL_MPCP_NACK_EVENT              = OPL_ONU_EVENT_OFFSET + 15,
    OPL_MLD_JOIN_EVENT               = OPL_ONU_EVENT_OFFSET + 16,
    OPL_MLD_DONE_EVENT               = OPL_ONU_EVENT_OFFSET + 17,
    OPL_HOST_TIMEOUT_EVENT           = OPL_ONU_EVENT_OFFSET + 18,
    OPL_OAM_RECEIVE_EVENT            = OPL_ONU_EVENT_OFFSET + 19,
    OPL_GPIO_IRQ_EVENT               = OPL_ONU_EVENT_OFFSET + 20,
    OPL_PON_LOSS_EVENT               = OPL_ONU_EVENT_OFFSET + 21,

    OPL_ONU_ENABLE_EVENT = OPL_ONU_EVENT_OFFSET + 100,  /*host internal use event*/
    /* add events before MAX_EVENT_TYPE */
    OPL_MAX_EVENT_TYPE
}OPL_ALARM_EVENT_TYPE_e;

typedef enum
{
    OPL_ONU_RESET_NONE,
    OPL_ONU_RESET_SW,
    OPL_ONU_RESET_SW_ERROR
}OPL_ONU_RESET_REASON_e;

typedef enum
{
    EMAPPER_COOKIE_ID = 0,
    EMAPPER_EXT_RAM_READ_WS = 1,
    EMAPPER_EXT_RAM_WRITE_WS = 2,
    EMAPPER_EXT_RAM_SIZE = 3,
    EMAPPER_FIRST_FW_START_ADDR = 4,
    EMAPPER_SECOND_FW_START_ADDR = 5,
    EMAPPER_MANAGE_SECS_START_ADDR = 7,
    EMAPPER_EXT_FLASH_READ_WS = 8,
    EMAPPER_EXT_FLASH_WRITE_WS = 9,
    EMAPPER_FLASH_VENDOR = 10,
    EMAPPER_SDRAM_INFO_INDEX = 11,
    EMAPPER_SDRAM_REFRESH_CYCLE = 12,
    EMAPPER_SDRAM_TCAS = 13,
    EMAPPER_SDRAM_TRAS = 14,
    EMAPPER_SDRAM_WIDTH_MODE = 15,
    EMAPPER_SDRAM_RBC_OR_BRC = 16,
    EMAPPER_EXT_RAM_TYPE_IN_USE = 20,
    EMAPPER_BOOT_LOADER_MODE = 21,
    EMAPPER_UART_BAUD_INDEX = 22,
    EMAPPER_PON_LOSS_SIGNAL_POLARITY = 23,
    EMAPPER_PON_TBC_SIGNAL_POLARITY = 24,
    EMAPPER_LASER_TX_ENABLE_POLARITY = 25,
    EMAPPER_ZBT_USE = 28,
    EMAPPER_ZBT_CONFIG = 29,
    EMAPPER_PON_CONNECT_ON_POWER_UP = 30,
    EMAPPER_OUI_ADDR = 31,
    EMAPPER_EEPROM_MAC_ADDR = 32,
    EMAPPER_EEPROM_IP_ADDR = 33,
    EMAPPER_EEPROM_NET_MASK = 34,
    EMAPPER_LASER_TON = 35,
    EMAPPER_LASER_TOFF = 36,
    EMAPPER_UNI_PHY_ADVERTISING_ENABLE = 37,
    EMAPPER_UNI_BRIDGE_ENABLE = 38,
    EMAPPER_UNI_AUTONEG_ENABLE = 39,
    EMAPPER_UNI_MDIO_EXTERN_PHY_ADDR = 40,
    EMAPPER_UNI_MAC_TYPE = 41,
    EMAPPER_UNI_MASTER_MODE = 42,
    EMAPPER_UNI_ADVERTISE_1000T_MULTI_PORT = 43,
    EMAPPER_UNI_ADVERTISE_1000T_HALF_DUPLEX = 44,
    EMAPPER_UNI_ADVERTISE_1000T_FULL_DUPLEX = 45,
    EMAPPER_UNI_ADVERTISE_PAUSE_ASYMMETRIC = 46,
    EMAPPER_UNI_ADVERTISE_PAUSE_ENABLED = 47,
    EMAPPER_UNI_ADVERTISE_100T4 = 48,
    EMAPPER_UNI_ADVERTISE_100TX_FD = 49,
    EMAPPER_UNI_ADVERTISE_100TX_HD = 50,
    EMAPPER_UNI_ADVERTISE_10TX_FD = 51,
    EMAPPER_UNI_ADVERTISE_10TX_HD = 52,
    EMAPPER_THRESHOLD_MODE = 53,
    EMAPPER_UNIFY_THRESHOLD_MODE = 54,
    EMAPPER_POINT_TO_POINT_ENABLE = 55,
    EMAPPER_GRANTED_ALWAYS = 56,
    EMAPPER_LASER_ON_PERMANENTLY = 57,
    EMAPPER_PON_TX_DISABLE_DATA_FORMAT = 58,
    EMAPPER_IDLE_BYTE0 = 59,
    EMAPPER_IDLE_BYTE1 = 60,
    EMAPPER_IDLE_BYTE2 = 61,
    EMAPPER_IDLE_BYTE3 = 62,
    EMAPPER_IDLE_PREAMBLE_DATA = 63,
    EMAPPER_USER_NAME_802_1X = 64,
    EMAPPER_PASSWORD_802_1X = 65,
    EMAPPER_GENERAL_PURPOSE_FIELD1 = 66,
    EMAPPER_GENERAL_PURPOSE_FIELD2 = 67,
    EMAPPER_PON_CLOCK_FINE_TUNE = 68,
    EMAPPER_TIMESTAMP_DELAY_FEC = 69,
    EMAPPER_ARB_PON_TIMESTAMP_DELTA = 70,
    EMAPPER_PON_CLK_CALIB_TX = 71,
    EMAPPER_PON_CLK_CALIB_RX = 72,
    EMAPPER_DYING_GASP_POLARITY = 73,
    EMAPPER_LINK_FAULT_POLARITY = 74,
    EMAPPER_CRITICAL_EVENT_POLARITY = 75,
    EMAPPER_BOOT_LOADER_REBOOT_SWITCH_COUNT = 76,
    EMAPPER_HMII_MODE = 77,
    EMAPPER_LAST_SYSTEM_PARAM,
    EMAPPER_USER_PARAM_0 = 0x200,
    EMAPPER_USER_PARAM_1,
    EMAPPER_USER_PARAM_2,
    EMAPPER_USER_PARAM_3,
    EMAPPER_USER_PARAM_4,
    EMAPPER_USER_PARAM_5,
    EMAPPER_USER_PARAM_6,
    EMAPPER_USER_PARAM_7,
    EMAPPER_USER_PARAM_8,
    EMAPPER_USER_PARAM_9,
    EMAPPER_USER_PARAM_10,
    EMAPPER_USER_PARAM_11,
    EMAPPER_USER_PARAM_255,
    EMAPPER_LAST_PARAM
}OPL_ONU_EMAPPER_PARAM_e;

typedef enum
{
    OPL_ONU_DOWNSTREAM = 0,
    OPL_ONU_UPSTREAM = 1
}OPL_ONU_DIRECTION_e;

typedef enum
{
    TH_REPORTER_DISABLE,
    TH_REPORTER_SINGLE_THR,
    TH_REPORTER_THR_PER_QUEUE
}OPL_ONU_THRESHOLD_RPT_CFG_e;

typedef enum
{
    PQ_UPSTREAM,
    PQ_DOWNSTREAM,
    PQ_RX_CPU_PON,
    PQ_TX_CPU_PON,
    PQ_RX_CPU_UNI
}OPL_ONU_PQUEUE_e;
typedef enum
{
    OPL_MULTICAST_LINK_CONSTRAINT,       /*01-80-c2-00-00-xx*/
    OPL_MULTICAST_IPV4_LOCAL_NETWORK,    /*01-00-5e-00-00-xx*/
    OPL_MULTICAST_IPV4_MANAGEMENT,       /*01-00-5e-00-01-xx*/
    OPL_MULTICAST_IPV4_OTHER,            /*01-00-5e-xx-xx-xx*/
    OPL_MULTICAST_IPV6_LOCAL_NETWORK,    /*33-33-00-00-00-xx*/
    OPL_MULTICAST_IPV6_MANAGEMENT,       /*33-33-00-00-xx-xx*/
    OPL_MULTICAST_IPV6_OTHER,            /*33-33-xx-xx-xx-xx*/
    OPL_MULTICAST_OTHER_NON_IP,
    OPL_MULTICAST_MAX_NOT_SUPPORTED
}OPL_ONU_MULTICAST_TYPE_e;

typedef enum
{
    OPL_DONOT_PASS,
    OPL_PASS_DATAPATH,
    OPL_PASS_CPU,
    OPL_PASS_BOTH
} OPL_ONU_FOREARDING_ACTION_e;  
typedef struct ONU_UNI_CONFIG_s
{
    opuint8 MAC_type;
    opuint8 MII_rate;
    opuint8 duplex;
    opuint8 gmii_master_mode;
    opuint8 auto_negotiate;
    opuint8 mode;
    opuint8 advertise;
    opuint8 adv_10_half;
    opuint8 adv_10_full;
    opuint8 adv_100_half;
    opuint8 adv_100_full;
    opuint8 adv_100_t4;
    opuint8 adv_pause;
    opuint8 adv_asym_pause;
    opuint8 adv_1000_half;
    opuint8 adv_1000_full;
    opuint8 adv_port_type;
    opuint8 phy_address;
}__attribute__ ((packed)) ONU_UNI_CONFIG_t;

typedef struct OPL_ONU_THRESHOLDS_LEVEL_s
{
    opuint32 thresholds[8];
}__attribute__ ((packed)) OPL_ONU_THRESHOLDS_LEVEL_t;

typedef struct GEREAL_MAC_ADDR_s{
    opint8 macAddr[6];
}__attribute__ ((packed))  GEREAL_MAC_ADDR_t;

typedef struct OPL_ONU_VERSION_s
{
    opuint16 hw_major_ver;
    opuint16 hw_minor_ver;
    opuint16 fw_major_ver;
    opuint16 fw_minor_ver;
    opuint16 fw_build_num;
    opuint16 fw_maintenance_ver;
    opuint32 oamVersion;
}__attribute__ ((packed)) OPL_ONU_VERSION_t;

typedef struct OPL_ENCRYPTION_KEY_s
{
    opuint32 key[4];
}__attribute__ ((packed)) OPL_ENCRYPTION_KEY_t;
typedef enum
{
    DISCONNECT,
    CONNECT
} OPL_ONU_CONNECTION_e;
typedef enum
{
    TRIPLE_CHURNING,
    AES
} OPL_ONU_ENCRYPTION_e;
typedef struct OPL_ONU_STATUS_s
{
    OPL_ONU_CONNECTION_e connection;
    OPBOOL       oam_link_established;
    OPBOOL       authorization_state;
    OPBOOL       PON_Loopback;
    opuint8      mac_addr[6];
    opuint16     ONU_llid;
}__attribute__ ((packed)) OPL_ONU_STATUS_t;


typedef struct OPL_ONU_STATISTIC_s
{
    opuint32 TxTotalPkt;
    opuint32 TxBroad;
    opuint32 TxMulti;
    opuint32 TxPause;
    opuint32 TxMacCtrl;
    opuint32 Tx64byte;
    opuint32 Tx65to127;
    opuint32 Tx128to255;
    opuint32 Tx256to511;
    opuint32 Tx512to1023;
    opuint32 Tx1024to1518;
    opuint32 TxByte;
    opuint32 Tx1519toMTU;
    
    opuint32 RxGood;
    opuint32 RxDrop;
    opuint32 RxCRCAlignErr;
    opuint32 RxLenErr;
    opuint32 RxFragments;
    opuint32 RxJabbers;
    opuint32 RxTotalPks;
    opuint32 RxBroad;
    opuint32 RxMulti;
    opuint32 RxPause;
    opuint32 RxErrMacCtrl;

    opuint32 RxMACCtrl;
    opuint32 Rxshort;
    opuint32 Rxlong;
    opuint32 Rx64byte;
    opuint32 Rx65to127;
    opuint32 Rx128to255;
    opuint32 Rx256to511;
    opuint32 Rx512to1023;
    opuint32 Rx1024to1518;
    opuint32 Rx1519toMTU;
    opuint32 RxByte;
    opuint32 RxL3Multi;
    opuint32 RxRxERPkts;
    opuint32 RxIPGErr;
}__attribute__ ((packed)) OPL_ONU_STATISTIC_t;

/*******************************************************************************
* OnuMacIdSet
*
* DESCRIPTION:
*		set Onu pon mac address
*	INPUTS:
*		onu_id: onu id;
        macid:  mac address.
*	OUTPUTS:
*		
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuMacIdSet(opuint8 onu_id,opuint8 *macId);

/*******************************************************************************
* OnuMacIdGet
*
* DESCRIPTION:
*		get Onu pon mac address
*	INPUTS:
*		onu_id: onu id;        
*	OUTPUTS:
*		macid:  mac address.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuMacIdGet(opuint8 onu_id,opuint8 *macId);

/*******************************************************************************
* OnuLaserEnable
*
* DESCRIPTION:
*		set Onu pon Laser enable.
*	INPUTS:
*		onu_id: onu id;  
        enable:layser enalbe,TRUE/FALSE
*	OUTPUTS:
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuLaserEnable(opuint8 onu_id,OPBOOL enable);

/*******************************************************************************
* OnuLaserEnableGet
*
* DESCRIPTION:
*		set Onu pon Laser enable.
*	INPUTS:
*		onu_id: onu id;        
*	OUTPUTS:
        enable:layser enalbe,TRUE/FALSE
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuLaserEnableGet(opuint8 onu_id,OPBOOL *enable);

/*******************************************************************************
* OnuDevicerReset
*
* DESCRIPTION:
*		reset ONU device
*	INPUTS:
*		onu_id: onu id;  
        type:reset reason type.
*	OUTPUTS:
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuDevicerReset(opuint8 onu_id,OPL_ONU_RESET_REASON_e type);
/*******************************************************************************
* OnuVersionGet
*
* DESCRIPTION:
*		get onu device fw/sw//chip versionl.
*	INPUTS:
*		onu_id: onu id;  
*	OUTPUTS:
        version:version information.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuVersionGet(opuint8 onu_id,OPL_ONU_VERSION_t *version);

/*******************************************************************************
* OnuOamTransmit
*
* DESCRIPTION:
*		transmit oam packets.
*	INPUTS:
*		onu_id: onu id;  
        opcode:oam option code.
        p:packets buffer.
        len:packets length.
*	OUTPUTS:
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuOamTransmit(opuint8 onu_id,opuint32 opcode,opuint8 *p,opuint32 len);
/*******************************************************************************
* OnuLostGet
*
* DESCRIPTION:
*		get ONU link status.
*	INPUTS:
*		onu_id: onu id;   
*	OUTPUTS:
        status:link status.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuLostGet(opuint8 onu_id,OPBOOL *status);
/*******************************************************************************
* OnuUpTimeGet
*
* DESCRIPTION:
*		get ONU up time.
*	INPUTS:
*		onu_id: onu id;   
*	OUTPUTS:
        up_time_msec:up time value.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuUpTimeGet(opuint8 onu_id,opuint64 *up_time_msec);
/*******************************************************************************
* OnuExtendMTUSizeEnable
*
* DESCRIPTION:
*		set ONU extend MTU size enable.
*	INPUTS:
*		onu_id: onu id;  
        enable:enable/disable extend MTU size.
*	OUTPUTS:        
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuExtendMTUSizeEnable(opuint8 onu_id,OPBOOL enable);

/*******************************************************************************
* OnuExtendMTUSizeEnableGet
*
* DESCRIPTION:
*		Get ONU extend MTU size enable.
*	INPUTS:
*		onu_id: onu id;   
*	OUTPUTS:
        enable:enable/disable extend MTU size.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuExtendMTUSizeEnableGet(opuint8 onu_id,OPBOOL *enable);

/*******************************************************************************
* OnuImageBurnEx
*
* DESCRIPTION:
*		updata ONU image.
*	INPUTS:
*		onu_id: onu id;  
        image:image buffer.
        length:image size.
        block:updata type(block/unblock).
*	OUTPUTS:        
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuImageBurnEx(opuint8 onu_id,const opuint8 *image,const opuint32 length,OPBOOL block);

/*******************************************************************************
* OnuUNIConfigSet
*
* DESCRIPTION:
*		set UNI config.
*	INPUTS:
*		onu_id: onu id;  
        config: UNI config.
*	OUTPUTS:        
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuUNIConfigSet(opuint8 onu_id,ONU_UNI_CONFIG_t *config);

/*******************************************************************************
* OnuUNIConfigGet
*
* DESCRIPTION:
*		Get UNI config.
*	INPUTS:
*		onu_id: onu id        
*	OUTPUTS: 
        config: UNI config.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuUNIConfigGet(opuint8 onu_id,ONU_UNI_CONFIG_t *config);


/*******************************************************************************
* OnuConnectionSet
*
* DESCRIPTION:
*		set ONU connection.
*	INPUTS:
*		onu_id: onu id        
*	OUTPUTS: 
        connection: connection status.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuConnectionSet(opuint8 onu_id,OPL_ONU_CONNECTION_e connection);

/*******************************************************************************
* OnuUNIConfigGet
*
* DESCRIPTION:
*		Get ONU connection.
*	INPUTS:
*		onu_id: onu id        
*	OUTPUTS: 
        connection: connection status.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuConnectionGet(opuint8 onu_id,OPL_ONU_CONNECTION_e *connection);


/*******************************************************************************
* OnuStatusGet
*
* DESCRIPTION:
*		Get ONU status.
*	INPUTS:
*		onu_id: onu id        
*	OUTPUTS: 
        status: onu status.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuStatusGet(opuint8 onu_id,OPL_ONU_STATUS_t *status);


/*******************************************************************************
* OnuEncryptionSet
*
* DESCRIPTION:
*		set ONU Encryption.
*	INPUTS:
*		onu_id: onu id        
*	OUTPUTS: 
        mode: Encryption mode(AES/TRIPLE_CHURNING).
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuEncryptionSet(opuint8 onu_id,OPL_ONU_ENCRYPTION_e mode);



/*******************************************************************************
* OnuEncryptionGet
*
* DESCRIPTION:
*		Get ONU Encryption.
*	INPUTS:
*		onu_id: onu id        
*	OUTPUTS: 
        mode: Encryption mode(AES/TRIPLE_CHURNING).
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuEncryptionGet(opuint8 onu_id,OPL_ONU_ENCRYPTION_e *mode);


/*******************************************************************************
* OnuEncryptionActKeySet
*
* DESCRIPTION:
*		set ONU EncryptionActKey.
*	INPUTS:
*		onu_id: onu id        
*	OUTPUTS: 
        key_seq: EncryptionActKey.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuEncryptionActKeySet(opuint8 onu_id,opuint8 key_seq);



/*******************************************************************************
* OnuEncryptionActKeyGet
*
* DESCRIPTION:
*		Get ONU EncryptionAct.
*	INPUTS:
*		onu_id: onu id        
*	OUTPUTS: 
        key_seq: EncryptionActKey..
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuEncryptionActKeyGet(opuint8 onu_id,opuint8 *key_seq);

/*******************************************************************************
* OnuPonPortStatsGet
*
* DESCRIPTION:
*		Get ONU Pon port packets statstic.
*	INPUTS:
*		onu_id: onu id        
*	OUTPUTS: 
        stats: packets statstic.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuPonPortStatsGet(opuint8 onu_id,OPL_ONU_STATISTIC_t *stats);

/*******************************************************************************
* OnuUniPortStatsGet
*
* DESCRIPTION:
*		Get ONU user port packets statstic.
*	INPUTS:
*		onu_id: onu id        
*	OUTPUTS: 
        stats: packets statstic.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuUniPortStatsGet(opuint8 onu_id,opuint8 port,OPL_ONU_STATISTIC_t *stats);


/*******************************************************************************
* OnuEventHookRegister
*
* DESCRIPTION:
*		Register Onu event hook.
*	INPUTS:
*		onu_id: onu id   
        type  :event type.
        fun   :event hook handle.
*	OUTPUTS:  
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuEventHookRegister(opuint8 onu_id,OPL_ALARM_EVENT_TYPE_e type,PFUNCTION fun);


/*******************************************************************************
* OnuEventEnable
*
* DESCRIPTION:
*		enable Onu event hook.
*	INPUTS:
*		onu_id: onu id   
        type  :event type.
        enable:event hook enable flag.
*	OUTPUTS:  
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuEventEnable(opuint8 onu_id,OPL_ALARM_EVENT_TYPE_e type,OPBOOL enable);
/*******************************************************************************
* OnuHwRegisterRead
*
* DESCRIPTION:
*		read onu chip register value.
*	INPUTS:
*		onu_id: onu id   
        regaddr:register address.        
*	OUTPUTS: 
        value  :register value.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuHwRegisterRead(opuint8 onu_id,opuint32 regaddr,opuint32 *value);



/*******************************************************************************
* OnuHwRegisterWrite
*
* DESCRIPTION:
*		write onu chip register value.
*	INPUTS:
*		onu_id: onu id   
        regaddr:register address.
        value  :register value.
*	OUTPUTS:  
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuHwRegisterWrite(opuint8 onu_id,opuint32 regaddr,opuint32 value);

/*******************************************************************************
* OnuLEDSetConfig
*
* DESCRIPTION:
*		config ONU led action.
*	INPUTS:
*		onu_id: onu id   
        led  :LED type.
        act  :LED action.
*	OUTPUTS:  
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuLEDSetConfig(opuint8 onu_id,OPL_LED_e led,OPL_LED_ACT_e act);

/*******************************************************************************
* OnuIpMulticastEnable
*
* DESCRIPTION:
*		enable/disable forwarding ip multicast frame.
*	INPUTS:
*		onu_id: onu id   
        direction:stream direction.
        enable  :enable/disable flag.
*	OUTPUTS:  
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuIpMulticastEnable(opuint8 onu_id,OPL_ONU_DIRECTION_e direction,OPBOOL enable);

/*******************************************************************************
* OnuIpMulticastEnableGet
*
* DESCRIPTION:
*		Get  ip multicast frame forwarding enable status.
*	INPUTS:
*		onu_id: onu id   
        direction:stream direction.       
*	OUTPUTS:  
         enable  :enable/disable flag.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuIpMulticastEnableGet(opuint8 onu_id,OPL_ONU_DIRECTION_e direction,OPBOOL *enable);
/*******************************************************************************
* OnuClassifyMulticastConfigSet
*
* DESCRIPTION:
*		enable/disable the forwarding of IP multicast frames in the datapath in the designated direction.
*	INPUTS:
*		onu_id: onu id   
        mc_type:multicast type.
        direction:stream direction.
        act:stream action.
*	OUTPUTS:           
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuClassifyMulticastConfigSet(opuint8 onu_id,
                     OPL_ONU_MULTICAST_TYPE_e mc_type,
                     OPL_ONU_DIRECTION_e direction,
                     OPL_ONU_FOREARDING_ACTION_e act);
                     
/*******************************************************************************
* OnuClassifyMulticastConfigGet
*
* DESCRIPTION:
*		enable/disable the forwarding of IP multicast frames in the datapath in the designated direction.
*	INPUTS:
*		onu_id: onu id   
        mc_type:multicast type.
        direction:stream direction.        
*	OUTPUTS:  
        act:stream action.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuClassifyMulticastConfigGet(opuint8 onu_id,
                     OPL_ONU_MULTICAST_TYPE_e mc_type,
                     OPL_ONU_DIRECTION_e direction,
                     OPL_ONU_FOREARDING_ACTION_e *act);

/*******************************************************************************
* OnuPQUEUEIngressLimitGet
*
* DESCRIPTION:
*		Get the ingress limit of specified sub queue of a priority queue.
*	INPUTS:
*		onu_id: onu id   
        queue:queue type.
        pri:queue priority
*	OUTPUTS:  
         limit  :queue limit value.
* 	RETURNS:
* 	S_OK:successfule.others is error.	
* 	SEE ALSO: 
*/
opint32 OnuPQUEUEIngressLimitGet(opuint8 onu_id,OPL_ONU_PQUEUE_e queue,opuint8 pri,opuint16 *limit);
#endif
