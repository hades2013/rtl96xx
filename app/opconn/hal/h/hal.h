/*************************************************************************
*
*  COPYRIGHT (C) 2003-2009 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*   used only with Opulan semi-conductor products. 
*
* FILENAME:  hal.h
*
* DESCRIPTION: 
*   
*
* Date Created: Mar 10, 2009
*
* Authors(optional): Yang Chuang Raymond
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/hal/h/hal.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __HAL_H_
#define __HAL_H_
 
#ifdef __cplusplus
extern "C" {
#endif

#include <vos.h>
#include "opconn_usr_ioctrl.h"
#include "lw_config.h"

#if defined(CONFIG_PRODUCT_EPN200)
#define PORT_PON_LOGIC                  2
#else
#define PORT_PON_LOGIC                  5    
#endif
 
typedef enum OPL_DEV_TYPE_e
{ 
    OPL_DEV_EOC,
    OPL_DEV_ONU,
    OPL_DEV_OLT,
    OPL_DEV_IPMUX 
}OPL_DEV_TYPE_t;

typedef enum HAL_API_INDEX_e
{

    /***************************************************************
     * GLOBAL API INDEX
     */
    HAL_API_GBL_BASE = 0,
    HAL_API_GBL_REG_READ,
    HAL_API_GBL_REG_WRITE,
    HAL_API_GBL_REG_FIELD_READ,
    HAL_API_GBL_REG_FIELD_WRITE,
    HAL_API_GBL_TAB_READ,
    HAL_API_GBL_TAB_WRITE,
    HAL_API_GBL_REG_BIT_TEST,
    HAL_API_GBL_REG_BIT_SET,
    HAL_API_GBL_REG_BIT_CLEAR,
    HAL_API_GBL_REG_BIT_TOGGLE,
    HAL_API_GBL_ORR,
    HAL_API_GBL_OWR,
    HAL_API_GBL_MRR,
    HAL_API_GBL_MWR,
    HAL_API_GBL_OST,
    HAL_API_GBL_ORT,
    HAL_API_GBL_OWT,
    HAL_API_GBL_CHIP_RESET,
    HAL_API_GBL_MDIO_REG_READ,
    HAL_API_GBL_MDIO_REG_WRITE,
    HAL_API_GBL_MDIO_REG_FIELD_READ,
    HAL_API_GBL_MDIO_REG_FIELD_WRITE,    


    /***************************************************************
     * VLAN API INDEX
     */
    HAL_API_VLAN_BASE = 50,
    HAL_API_VLAN_CREATE,
    HAL_API_VLAN_DELETE,
    HAL_API_VLAN_FLUSH,
    HAL_API_VLAN_MEMBERADD,
    HAL_API_VLAN_MEMBERDEL,
    HAL_API_VLAN_LOCAL_SWITCH_SET,

    
    /***************************************************************
     * CLS API INDEX
     */
    HAL_API_QOS_BASE = 150,
    /*l2*/
    HAL_API_CLS_DESTMAC_CFG,
    HAL_API_CLS_SRCMAC_CFG , 
    HAL_API_CLS_TYPELEN_CFG,
    HAL_API_CLS_FCOS_CFG,
    HAL_API_CLS_FVID_CFG,
    HAL_API_CLS_SCOS_CFG,
    HAL_API_CLS_SVID_CFG,

    /*l3*/
    HAL_API_CLS_DESTIP_CFG,
    HAL_API_CLS_SRCIP_CFG,
    HAL_API_CLS_IPTYPE_CFG,
    HAL_API_CLS_IPTOS_CFG,


    /*l4*/
    HAL_API_CLS_L4DESTPORT_CFG,
    HAL_API_CLS_L4SRCPORT_CFG,

    /*others*/
    HAL_API_CLS_INGRESSPORT_CFG,
    HAL_API_CLS_EGRESSPORT_CFG,
    HAL_API_CLS_COS_REMARK,
    HAL_API_CLS_VID_REMARK,
    HAL_API_CLS_ICOS_MAP,
    HAL_API_CLS_TRANACT,
    HAL_API_CLS_MASK_PRIORITY,
    HAL_API_CLS_DELETE,
    HAL_API_CLS_ENTRYSHOW,


    /***************************************************************
     * FDB API INDEX
     */
    HAL_API_FDB_BASE = 350,
    HAL_API_FDB_AGETIME_SET, 
    HAL_API_FDB_AGETIME_GET, 
    HAL_API_FDB_MAC_ADD, 
    HAL_API_FDB_MAC_DEL, 
    HAL_API_FDB_MAC_REMOVE, 
    HAL_API_FDB_DYNAMIC_FLUSH, 
    HAL_API_FDB_ALL_FLUSH, 
    HAL_API_FDB_ALL_FLUSH_BY_PORT, 
    HAL_API_FDB_LEARN_EN_SET, 
    HAL_API_FDB_LEARN_EN_GET, 
    HAL_API_FDB_SOFT_LEARN_EN_SET, 
    HAL_API_FDB_SOFT_LEARN_EN_GET, 
    HAL_API_FDB_AGE_EN_SET, 
    HAL_API_FDB_AGE_EN_GET, 
    HAL_API_FDB_MAC_NUM_LIMIT_SET, 
    HAL_API_FDB_MAC_MULTIPORT_ADD, 
    HAL_API_FDB_MAC_MULTIPORT_DEL, 
    HAL_API_FDB_ONE_SHOW, 
    HAL_API_FDB_ALL_SHOW, 
    HAL_API_FDB_MAC_MOVE, 


    /***************************************************************
     * PORT API INDEX
     */
    HAL_API_PORT_BASE = 450,
    HAL_API_PORT_INIT,     
    HAL_API_PORT_PVID_SET,        
    HAL_API_PORT_PVID_GET,        
    HAL_API_PORT_VLAN_MODE_SET,        
    HAL_API_PORT_VLAN_MODE_GET,        
    HAL_API_PORT_DEFAULT_PRI_SET,        
    HAL_API_PORT_DEFAULT_PRI_GET,        
    HAL_API_PORT_STATE_GET,        
    HAL_API_PORT_SPEED_SET,        
    HAL_API_PORT_SPEED_GET,        
    HAL_API_PORT_DUPLEX_SET,        
    HAL_API_PORT_DUPLEX_GET,        
    HAL_API_PORT_FLOWCONTROL_SET,        
    HAL_API_PORT_FLOWCONTROL_GET,        
    HAL_API_PORT_LOOPBACK_SET,        
    HAL_API_PORT_ADMIN_SET,        
    HAL_API_PORT_ADMIN_GET,        
    HAL_API_PORT_AUTO_ADMIN_SET,        
    HAL_API_PORT_AUTO_ADMIN_GET,        
    HAL_API_PORT_AUTONEG_RESTART,        
    HAL_API_PORT_USPOLICING_EN_SET,        
    HAL_API_PORT_USPOLICING_EN_GET,        
    HAL_API_PORT_US_CIR_SET,        
    HAL_API_PORT_Us_CIR_GET,        
    HAL_API_PORT_US_CBS_SET,        
    HAL_API_PORT_US_CBS_GET,        
    HAL_API_PORT_US_EBS_SET,        
    HAL_API_PORT_US_EBS_GET,        
    HAL_API_PORT_USPOLICING_SET,        
    HAL_API_PORT_USPOLICING_GET,        
    HAL_API_PORT_DS_POLICING_EN_SET,        
    HAL_API_PORT_DS_POLICING_EN_GET,        
    HAL_API_PORT_DS_CIR_SET,        
    HAL_API_PORT_DS_CIR_GET,        
    HAL_API_PORT_DS_CBS_SET,        
    HAL_API_PORT_DS_CBS_GET,        
    HAL_API_PORT_DS_EBS_SET,        
    HAL_API_PORT_DS_EBS_GET,        
    HAL_API_PORT_DSPOLICING_SET,        
    HAL_API_PORT_DSPOLICING_GET,        
    HAL_API_PORT_VTT_ENTRY_ADD,        
    HAL_API_PORT_VTT_ENTRY_DEL,        
    HAL_API_PORT_VTT_ENTRY_INIT,        
    HAL_API_PORT_VTT_ENTRY_GET,        
    HAL_API_PORT_VTT_ENTRY_COUNT_GET,
    HAL_API_PORT_VLAN_TRUNK_ENTRY_ADD,
    HAL_API_PORT_VLAN_TRUNK_ENTRY_DEL,
    HAL_API_PORT_VLAN_TRUNK_ENTRY_VALUE_GET,
    HAL_API_PORT_VLAN_TRUNK_ENTRY_INDEX_GET,
    HAL_API_PORT_VLAN_TRUNK_ENTRY_NUM_GET,
    HAL_API_PORT_VLAN_TRUNK_ENTRY_CLEAR,

    HAL_API_PORT_MAC_FILTER_ENTRY_ADD,
    HAL_API_PORT_MAC_FILTER_ENTRY_DEL,
    
	HAL_API_PORT_BIND_MAC_ENTRY_ADD,
    HAL_API_PORT_BIND_MAC_ENTRY_DEL,
    HAL_API_PORT_DLF_FILTER_ENABLE,

    HAL_API_PORT_CTL_LOCK_DROP_ENABLE,

    HAL_API_PORT_STATIC_MAC_ENTRY_ADD,
    HAL_API_PORT_STATIC_MAC_ENTRY_DEL,

    HAL_API_PORT_MIRROR_PORT_SET,
    HAL_API_PORT_MIRROR_PORT_GET,
    HAL_API_PORT_MIRROR_INGRESS_STATE_SET,
    HAL_API_PORT_MIRROR_INGRESS_STATE_GET,
    HAL_API_PORT_MIRROR_ENGRESS_STATE_SET,
    HAL_API_PORT_MIRROR_ENGRESS_STATE_GET,

	HAL_API_PORT_STORM_CTRL_RATE_SET,
	HAL_API_PORT_STORM_CTRL_RATE_GET,
	HAL_API_PORT_STORM_CTRL_FRAME_SET,
	HAL_API_PORT_STORM_CTRL_FRAME_GET,

    /***************************************************************
     * OAM API INDEX
     */
    HAL_API_OAM_BASE = 650,
    HAL_API_OAM_SEND_FRAME,
    HAL_API_OAM_CTC_RECEIVE,
    HAL_API_OAM_OUI_RECEIVE,
    HAL_API_OAM_H3C_RECEIVE,
    HAL_API_OAM_PMC_RECEIVE,

    /***************************************************************
     * TM API INDEX
     */
    HAL_API_TM_BASE = 850,
    HAL_API_TM_QUEUE_SHAPER_SET,
    HAL_API_TM_QUEUE_SHAPER_GET,
    HAL_API_TM_QUEUE_WRED_SET,
    HAL_API_TM_QUEUE_WRED_GET,
    HAL_API_TM_QUEUE_PRIORITY_SET,
    HAL_API_TM_QUEUE_PRIORITY_GET,
    HAL_API_TM_QUEUE_WEIGHT_SET,
    HAL_API_TM_QUEUE_WEIGHT_GET,

    /***************************************************************
     * RSTP API INDEX
     */
    HAL_API_RSTP_BASE = 1050,
    HAL_API_RSTP_MODE_SET, 
    HAL_API_RSTP_PORT_STATE_SET, 
    HAL_API_RSTP_PORT_STATE_GET, 
    HAL_API_RSTP_PORT_MAC_FLUSH,


    /***************************************************************
     * Multicast API INDEX
     */
    HAL_API_MCAST_BASE = 1150,
    HAL_API_MCAST_INIT,
    HAL_API_MCAST_ENABLE,
    HAL_API_MCAST_DISABLE,
    HAL_API_MCAST_ARL_ADD,
    HAL_API_MCAST_ARL_DEL,



    /***************************************************************
     * STATS API INDEX
     */
    HAL_API_STATS_BASE = 1250,
    HAL_API_STATS_GE,
	HAL_API_STATS_PON,
	HAL_API_STATS_FE,
	HAL_API_STATS_TM,
	HAL_API_STATS_CLE,
	HAL_API_STATS_BRG,
	HAL_API_STATS_MPCP,
	HAL_API_STATS_MARB,
	HAL_API_STATS_CPDMA,
	HAL_API_STATS_FEDMA,
	HAL_API_STATS_GEPARSER,
	HAL_API_STATS_CLEHIT,
	HAL_API_STATS_POLLENDIS,
	HAL_API_STATS_CLR,
	HAL_API_STATS_SWHPORT,

    /***************************************************************
     * DEVICE API INDEX: ONU
     */
    HAL_API_ONU_BASE = 1300,
    HAL_API_ONU_INIT,
    HAL_API_ONU_MACID_SET,
    HAL_API_ONU_MACID_GET,
    HAL_API_ONU_PHY_FEC_MODE_SET,
    HAL_API_ONU_PHY_FEC_MODE_GET,
    HAL_API_ONU_LASER_ONCNT_SET,
    HAL_API_ONU_LASER_ONCNT_GET,
    HAL_API_ONU_LASER_OFFCNT_SET,
    HAL_API_ONU_LASER_OFFCNT_GET,
    HAL_API_ONU_SYNCTIME_GET,
    HAL_API_ONU_LLID_GET,
    HAL_API_ONU_SERDES_DELAYCNT_SET,
    HAL_API_ONU_SERDES_DELAYCNT_GET,
    HAL_API_ONU_LINK_STATUS_GET,
    HAL_API_ONU_OAM_FINISHED_SET,
    HAL_API_ONU_PON_MAC_BER_GET,
    HAL_API_ONU_SECURITY_STATUS_SET,
    HAL_API_ONU_SECURITY_STATUS_GET,
    HAL_API_ONU_SECURITY_MODE_SET,
    HAL_API_ONU_SECURITY_MODE_GET,
    HAL_API_ONU_SECURITY_AES_MODE_SET,
    HAL_API_ONU_SECURITY_AES_MODE_GET,
    HAL_API_ONU_UNI_SECURITY_KEY_GET,
    HAL_API_ONU_SCB_SECURITY_KEY_SET,
    HAL_API_ONU_DBA_RPT_MODE_SET,
    HAL_API_ONU_DBA_RPT_MODE_GET,
    HAL_API_ONU_DBA_QSET_Q_THRESHOLD_SET,
    HAL_API_ONU_DBA_QSET_Q_THRESHOLD_GET,
    HAL_API_ONU_DBA_AGENT_SET,
    HAL_API_ONU_DBA_AGENT_GET,
    HAL_API_ONU_DBA_DLY_SET,
    HAL_API_ONU_DBA_DLY_GET,
    HAL_API_ONU_MPCP_HOLDOVER_CTRL_SET,
    HAL_API_ONU_MPCP_HOLDOVER_CTRL_GET,
    HAL_API_ONU_MPCP_HOLDOVER_TIME_SET,
    HAL_API_ONU_MPCP_HOLDOVER_TIME_GET,
    HAL_API_ONU_MTU_GET,
    HAL_API_ONU_MTU_SET,
    HAL_API_ONU_PONTX_POLARITY_CONFIG,
    HAL_API_ONU_PON_LOOPBACK_CONFIG,

    /***************************************************************
     * DEVICE API INDEX: OLT
     */
    HAL_API_OLT_BASE = 1500,


    /***************************************************************
     * DEVICE API INDEX: EOC
     */
    HAL_API_EOC_BASE = 1700,


    /***************************************************************
     * HOST API INDEX
     */
    HAL_API_HOST_BASE = 1900,
    HAL_API_HOST_RECEIVE,
    HAL_API_HOST_SEND,
    HAL_API_HOST_SEND_TO_UPLINK,
    HAL_API_HOST_SEND_TO_DOWNLINK,
    HAL_API_HOST_DUMP_EN,
    HAL_API_HOST_COUNTER_CLEAR,
    HAL_API_HOST_COUNTER_SHOW,
    

    /***************************************************************
     * INTERRUPT API INDEX
     */
    HAL_API_INT_BASE = 1950,
    HAL_API_INT_LEVEL1_ENABLE,
    HAL_API_INT_LEVEL1_ENABLE_STATE,
    HAL_API_INT_LEVEL2_ENABLE,
    HAL_API_INT_LEVEL2_ENABLE_STATE,
    HAL_API_INT_COUNTER_ENABLE,
    HAL_API_INT_COUNTER_ENABLE_STATE,
    HAL_API_INT_COUNTER_SHOW,


    HAL_API_MAX = 2000

}HAL_API_INDEX_t;



/*****************************************************************************
 * EVENT INDEX START
 */

typedef enum HAL_EVENT_INDEX_e
{

    /***************************************************************
     * HOST EVENT INDEX
     */
    HAL_EVENT_HOST_BASE = 0,
    HAL_EVENT_HOST_HANDLING,


    /***************************************************************
     * INTERRUPT EVENT INDEX
     */
    HAL_EVENT_INT_BASE = 100,
    HAL_EVENT_INT_HANDLING,

    
    HAL_EVENT_MAX = 2000

}HAL_EVENT_INDEX_t;


#define OPL_STACKSIZE   240 * 1024


typedef unsigned int OPL_EVENT_ID;
typedef unsigned int OPL_API_ID;
typedef unsigned int OPL_DEVICE_ID;
typedef unsigned int OPL_RESULT;        /* 0 - ok, otherwise - false */

typedef void (*OPL_EVENT_FUNC)(void* ptr);
typedef void (*OPL_API_FUNC)(void* ptr);



typedef struct OPL_EVENT_UNIT_s
{
    OPL_EVENT_ID        eventId;
    OPL_EVENT_FUNC      eventFunc;
} OPL_EVENT_UNIT_t;

typedef struct OPL_API_UNIT_s
{
    OPL_API_ID          apiId;
    OPL_API_FUNC        apiFunc;
} OPL_API_UNIT_t;


typedef struct OPL_DEV_DRV_s
{
    OPL_DEV_TYPE_t      type;
    OPL_DEVICE_ID       devId;
    OPL_API_UNIT_t      arrayAPI[HAL_API_MAX];
} OPL_DEV_DRV_t;

typedef struct OPL_DEV_APP_s
{
    OPL_DEV_TYPE_t      type;
    OPL_DEVICE_ID       devId;
    OPL_EVENT_UNIT_t    arrayEvent[HAL_EVENT_MAX];
} OPL_DEV_APP_t;


typedef struct OPL_EVENT_DATA_s
{
    OPL_EVENT_ID        eventId;
    OPL_EVENT_FUNC      eventFunc;  
    void                *param;
    unsigned int        length;    
} OPL_EVENT_DATA_t;

typedef struct OPL_API_DATA_s
{
    OPL_API_ID          apiId;
    unsigned int        length;
    void                *param;
    OPL_API_FUNC        eventFunc;  
} OPL_API_DATA_t;

OPL_RESULT  halInit();
OPL_RESULT  halDestroy();

OPL_RESULT  halDrvApiRegister(OPL_API_UNIT_t *pstApiUnit);
OPL_RESULT  halDrvEventRaise(OPL_EVENT_DATA_t *pstData);

OPL_RESULT  halAppEventRegister(OPL_EVENT_UNIT_t *pstEventUint);
OPL_RESULT  halAppEventIsExisted(OPL_EVENT_ID eventId);
OPL_RESULT  halAppApiCallSync( OPL_API_DATA_t *pstData );
OPL_RESULT  halAppApiCallAsync( OPL_API_DATA_t *pstData );


#include <hal_cls.h>
#include <hal_fdb.h>
#include <hal_global.h>
#include <hal_host.h>
#include <hal_interrupt.h>
#include <hal_multicast.h>
#include <hal_port.h>
#include <hal_rstp.h>
#include <hal_tm.h>
#include <hal_vlan.h>

#if defined(ONU_1PORT)||defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
#include <hal_onu.h>
#endif

 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __HAL_H_ */

