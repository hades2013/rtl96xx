/*
*
*  COPYRIGHT (C) 2003-2009 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*   used only with Opulan semi-conductor products.
*
* mc_control.h
*
* DESCRIPTION:
*
* Date Created:
*
* Authors(optional):
*
* Reviewed by (optional):
*
* Edit History:
*
*/

/**
 * @defgroup mc_control MC_CONTROL
 * @{
 */

#ifndef MC_CONTROL_H
#define MC_CONTROL_H


/***** INCLUDE FILES *****/

#include <arpa/inet.h>

#include "defs.h"
#include "errors.h"
#include "log.h"

#include "opl_driver.h"

#include "opl_ethernet.h"

#include "opl_host.h"
#include "odm_port.h"

#include "opapi_comm_list.h"
#include "opapi_comm_inet.h"

#include "hal.h"
#include "hal_fdb.h"

#include "multicast_control.h"

#include "mc_control.h"

/***** DEFINES and ENUMS *****/

#define VLAN_DEFAULT_STPID    0x88a8
#define VLAN_DEFAULT_CTPID    0x8100

#define OP_MAX_VLAN_NUMBER            4096
#define MAC_ADDRESS_LENGTH            6

#define MC_ROUTER_PORT_NUM 1
#define MC_VLAN_TRANSLATION_ENTRY 1024
#define MC_MAX_HOST_NUM_PER_GROUP 64

#define PORT_PON_PHYSICAL                1
#define PORT_CPU_PHYSICAL                2
#define PORT_GE_PHYSICAL                 0

//#define PORT_PON_LOGIC                  5    //changed by wxian Max_port_num+1

#define OPL_ZERO            (0x00)
#define OPL_ERROR           (-1)
#define OPL_OK              (0x00) /* Operation succeeded                   */
#define OPL_BAD_PARA        (-2)   /* Illegal parameter in function called  */
#define OPL_NOT_SUPPORTED   (-3)   /* This request is not support           */
#define OPL_EXIST           (-4)   /* Tried to create existing item         */
#define OPL_NOT_EXIST       (-5)   /* Tried to create existing item         */
#define OPL_FAIL            (-6) /* Operation failed                      */
#define OPL_BAD_VALUE       (-7)   /* Illegal value (general)               */
#define OPL_NO_MEMORY       (-8)   /* Out of memory               */
#define OPL_NOT_FOUND       (-10)   /* Item not found                        */
#define OPL_NO_MORE         (-11)   /* No more items found                   */
#define OPL_NO_SUCH         (-12)   /* No such item                          */
#define OPL_TIMEOUT         (-13)   /* Time Out                              */
#define OPL_FULL            (-13)   /* full                              */

#define SMALL_BUF_LEN                     256
#define BIG_BUF_LEN                       2048

#define MODULE        MOD_MCAST

#define MULTICAST_ALL_HOST_GROUP      (unsigned int)inet_addr("224.0.0.1")
#define MULTICAST_ALL_ROUTERS_GROUP   (unsigned int)inet_addr("224.0.0.2")

#define IGMP_TYPE_GENERAL_QUERY       0x11
#define IGMP_TYPE_V1_REPORT           0x12
#define IGMP_TYPE_V2_REPORT           0x16
#define IGMP_TYPE_V2_LEAVE            0x17
#define IGMP_TYPE_V3_REPORT           0x22

#define IGMP_VERSION_1                0x1
#define IGMP_VERSION_2                0x2
#define IGMP_VERSION_3                0x3

/* default values */
#define IGMP_HOST_AGING_TIME_DEFAULT          260 /* 260s */
#define LAST_MEMBER_QUERY_INTERVAL_DEFAULT    1000 /* 1000ms */
#define LAST_MEMBER_QUERY_COUNT_DEFAULT       2
#define IGMP_MAX_GROUP_NUMBER_PER_PORT        8

/* configuration files */
#define CFGFILE_MULTICAST       "/cfg/multicast.conf"
#define CFGSECTION_MULTICAST_IGMP_MANAGEMENT  "IGMP"
#define CFGKEY_MULTICAST_IGMP_MANAGEMENT_IGMP_MODE "igmp_mode"
#define CFGKEY_MULTICAST_IGMP_MANAGEMENT_MODE "mode"
#define CFGKEY_MULTICAST_IGMP_MANAGEMENT_CTC_CONTROL_TYPE "ctc_control_type"
#define CFGKEY_MULTICAST_IGMP_MANAGEMENT_LEAVE_MODE "leave_mode"
#define CFGKEY_MULTICAST_IGMP_MANAGEMENT_HOST_AGING_TIME "host_aging_time"
#define CFGKEY_MULTICAST_IGMP_MANAGEMENT_LAST_QUERY_INTERVAL  "last_query_interval"
#define CFGKEY_MULTICAST_IGMP_MANAGEMENT_LAST_QUERY_COUNT  "last_query_count"
#define CFGSECTION_MULTICAST_IGMP_SNOOPING    "IGMP_Snooping"

#define CFGSECTION_MULTICAST_IGMP_PORT        "Port_"
#define CFGKEY_MULTICAST_IGMP_PORT_MULTICAST_VLAN   "multicast_vlan"
#define CFGKEY_MULTICAST_IGMP_PORT_TAGSTRIP   "tagstrip_mode"
#define CFGKEY_MULTICAST_IGMP_PORT_TAGSTRIP_TRANSLATION_VLAN  "translation_vlan_"
#define CFGKEY_MULTICAST_IGMP_PORT_MAX_GROUP_NUM "max_group_num"

#define CFGSECTION_MULTICAST_IGMP_GROUP       "Multicast_Group_"
#define CFGSECTION_MULTICAST_IGMP_GROUP_PORT       "port"
#define CFGSECTION_MULTICAST_IGMP_GROUP_VLAN       "multicast_vlan"
#define CFGSECTION_MULTICAST_IGMP_GROUP_DA_MAC       "da_mac"

/* BITMAP */
#define MC_BITMAP_GDA_MAC   (0x1<<0)
#define MC_BITMAP_GDA_IP    (0x1<<1)
#define MC_BITMAP_HOST_PORT_ID  (0x1<<2)
#define MC_BITMAP_VLAN_ID   (0x1<<3)
#define MC_BITMAP_SA_MAC    (0x1<<4)
#define MC_BITMAP_HOST_SRC_IP   (0x1<<5)
#define MC_BITMAP_HOST_SA_MAC   (0x1<<6)

#define MC_BITMAP mcControlBlock.bitmap
#define MC_HOST_BITMAP mcControlBlock.hostBitmap

/**
 * @brief TAG_TYPE_t follow ctc 2.1
 */
typedef enum TAG_TYPE_e {
  TAG_TYPE_UNTAG=0,
  TAG_TYPE_SINGLE_TAG,
  TAG_TYPE_DOUBLE_TAG
} TAG_TYPE_t;

/**
 * @brief MC_MODE_t follow ctc 2.1
 */
typedef enum GROUP_STATE_e {
  GROUP_STATE_LEARNING=0,
  GROUP_STATE_CONFIG_BY_CLI,
  GROUP_STATE_CONFIG_BY_OAM
} GROUP_STATE_t;

/**
 * @brief MC_MODE_t follow ctc 2.1
 */
typedef enum MC_MODE_e {
  MC_MODE_SNOOPING=0,
  MC_MODE_CTC,
  MC_MODE_PROXY,
  MC_MODE_STOP
} MC_MODE_t;

/**
 * @brief MULTICAST_LEAVE_MODE_t follow ctc 2.1
 */
typedef enum MC_LEAVE_MODE_e {
  MC_LEAVE_MODE_NON_FAST_LEAVE=1, /**< 0x00000001£ºdeactivate Fast Leave Function */
  MC_LEAVE_MODE_FAST /**< 0x00000002£ºactivate Fast Leave Function */
} MC_LEAVE_MODE_t;

typedef enum MULTICAST_TAG_OPER_MODE_e {
  MC_TAG_OPER_MODE_TRANSPARENT=0,
  MC_TAG_OPER_MODE_STRIP,
  MC_TAG_OPER_MODE_TRANSLATION
} MC_TAG_OPER_MODE_t;
#if 0
typedef enum CTC_CONTROL_TYPE_e {
  MC_CTL_GDA_MAC              = 0x00,   /* GDA MAC (DA MAC Only) Based */
  MC_CTL_GDA_MAC_VID        = 0x01,   /* GDA MAC + multicast Valn ID based */
  MC_CTL_GDA_GDA_SA_MAC    = 0x02,   /* GDA MAC + SA MAC (IGMP v3 optional) based */
  MC_CTL_GDA_GDA_IP_VID    = 0x03,
} CTC_CONTROL_TYPE_t;
#endif
typedef enum CTC_CONTROL_BITMAP_e {
  CTC_CONTROL_BITMAP_GDA_MAC=MC_BITMAP_GDA_MAC|MC_BITMAP_HOST_PORT_ID,
  CTC_CONTROL_BITMAP_GDA_MAC_VLAN_ID=MC_BITMAP_GDA_MAC|MC_BITMAP_HOST_PORT_ID,
  CTC_CONTROL_BITMAP_GDA_MAC_SA_MAC=MC_BITMAP_GDA_MAC|MC_BITMAP_SA_MAC|MC_BITMAP_HOST_PORT_ID, /* igmp v3 */
  CTC_CONTROL_BITMAP_GDA_IP_VLAN_ID=MC_BITMAP_GDA_IP|MC_BITMAP_HOST_PORT_ID
} CTC_CONTROL_TYPE_BITMAP_t;

typedef enum MC_GROUP_TYPE_e {
  MC_GROUP_TYPE_DYNAMIC=1,
  MC_GROUP_TYPE_STATIC
} MC_GROUP_TYPE_t;

typedef enum MC_DN_SWITCH_TYPE_e {
  MC_DN_SWITCH_TYPE_ONU_1PORT=1,
  MC_DN_SWITCH_TYPE_ONU_4PORT_88E6045,
  MC_DN_SWITCH_TYPE_ONU_4PORT_88E6046,
  MC_DN_SWITCH_TYPE_ONU_4PORT_88E6097,
  MC_DN_SWITCH_TYPE_ONU_4PORT_AR8306,
  MC_DN_SWITCH_TYPE_ONU_4PORT_AR8228,
  MC_DN_SWITCH_TYPE_ONU_4PORT_AR8327
} MC_DN_SWITCH_TYPE_t;

typedef struct MC_CONTROL_s {
  uint32 mcRecvEn; /* if the igmp packets send to cpu */
  uint32 mcMode; /* ctc, igmp snooping, igmp proxy ¡­ */
  uint32 leaveMode; /* fast leave, non fast leave */
  uint32 ctcControlType;
  uint32 bitmap; /* match mac,ip,src */
  uint32 hostBitmap; /* match srcMac,srcIp */

  uint32 dnSwitchType; /* marvell, asseros */

  uint32 hostAgingTime; /* 1s */
  uint32 lastQueryInterval; /* 1ms */
  uint32 lastQueryCount;

  uint32 maxGroupNum[ODM_NUM_OF_PORTS+1];

  uint32 stpid;
  uint32 ctpid;

  uint32 isInitialized;

  //uint32 portControl[OP_UNI_PORT_NUMBER+1]; /* the port id is from 1 to X */
  //uint32 vlanControl[OP_MAX_VLAN_NUMBER]; /* the vlan id is from 1 to 4095 */
  //uint32 routerAgingTime;
} MC_CONTROL_t;

typedef struct MC_PACKET_DESC_s {
  uint8 igmpGroupMac[MAC_ADDRESS_LENGTH];

  uint8 daMac[MAC_ADDRESS_LENGTH];
  uint8 saMac[MAC_ADDRESS_LENGTH];
  uint32 sTpid;
  uint32 sVlan; /* include cos */
  uint32 cTpid;
  uint32 cVlan; /* include cos */

  uint32 destIp;
  uint32 srcIp;

  uint32 igmpVersion; /* 1, 2, 3 */
  uint32 igmpType; /* join, leave, query */
  uint32 igmpMaxRespTime; /* 8bit */
  uint32 igmpGroupAdress; /* 32bit */

  uint32 ingressPacketLength;
  uint8 *ingressPacket; /* packet received from host */
  uint32 ingressPortId;
  uint32 ingressTagType; /*0,1,2 tags */
  uint32 ingressIsHaveIpOptions;

  //uint32 egressPacketLength;
  //uint8 *egressPacket; /* packet send to host */
  uint32 egressPortId;
  //uint32 egressTagType;
  //uint32 egressIsHaveIpOptions;

  TIMER_OBJ_t *nonFastLeaveTimer;

  uint32 nonFastLeaveQueryPacketLength;
  uint8 *nonFastLeaveQueryPacket;
  uint32 nonFastLeaveEgressTagType;
  uint32 nonFastLeaveEgressIsHaveIpOptions;

  uint32 isNonFastLeaveTimerStart;
} MC_PACKET_DESC_t;

typedef struct MC_HOST_s {
  uint8 saMac[MAC_ADDRESS_LENGTH]; /* the source mac in the igmp report packet */
  uint32 srcIp; /* the source ip in the igmp report packet */

  uint32 state; /* oam add/auto learning/cli add */

  uint32 hostAgingTime; /* the aging time for this host */
  uint32 isLastReportRecevied; /* use in non fast leave mode */
  uint32 isNonFastLeaveTimerStart;  /* in snooping mode, if the timer exists, the leave packets received is dropped */
} MC_HOST_t;

typedef struct MC_GROUP_s {
  uint8 groupMac[MAC_ADDRESS_LENGTH]; /* in ctc mode, ctcControlType=0x00 or 0x01 */
  uint8 saMac[MAC_ADDRESS_LENGTH]; /* igmpv3 */
  uint32 groupAddress; /* in ctc mode, ctcControlType=0x03 or in igmp snooping mode */
  uint32 mcVlan; /* multicast vlan */
  uint32 hostPortId;

  uint32 state; /* oam add/auto learning/cli add */

  clList *hostList;
  uint32 hostTotalNum;

  uint32 hostAgingTime; /* use in ctc mode */
  uint32 isLastReportRecevied; /* use in non fast leave mode */
  uint32 isNonFastLeaveTimerStart; /* in ctc mode, if the timer exists, the leave packets received is dropped */
  uint32 querycnt;
} MC_GROUP_t;

typedef struct MC_VLAN_TRANSLATION_ENTRY_s {
  uint32 enable;
  uint32 portId;
  uint32 mcVid;
  uint32 userVid;
} MC_VLAN_TRANSLATION_ENTRY_t;

typedef struct MC_ROUTER_PORT_ENTRY_s {
  uint32 enable;
  uint32 routerPortId;
} MC_ROUTER_PORT_ENTRY_t;

typedef struct MC_CONTROL_STATS_s
{
  /* IGMP Host Interface Statistics */
  uint32 generalQueryV1RxCount;
  uint32 generalQueryV2RxCount;
  uint32 groupSpecificV2QueryRxCount;
  uint32 otherHostRxUnknownCount;
  uint32 otherRouterRxUnknownCount;

  uint32 joinReportV1RxCount;
  uint32 joinReportV2RxCount;
  uint32 leaveV2RxCount;

  uint32 memAllocCount;
  uint32 memfreeCount;

} MC_CONTROL_STATS_t;

/* IGMP message encapsulated in Ethernet frame IP packet structure */
typedef struct eth_igmp_frm_none_tag_s
{
    eth_header_none_tag_t eth_hdr; /* Ethernet header with single tag */
    ipv4_header_t ip_hdr;              /* IP header */
    ipv4_router_alert_opt_t ip_opt;    /* IP option (router alert option) */
    igmp_msg_t igmp_msg;               /* IGMP message */

} __attribute__((packed)) eth_igmp_frm_none_tag_t;

/* IGMP message encapsulated in Ethernet frame IP packet structure */
typedef struct eth_igmp_frm_with_sgl_s
{
    eth_header_with_sgl_tag_t eth_hdr; /* Ethernet header with single tag */
    ipv4_header_t ip_hdr;              /* IP header */
    ipv4_router_alert_opt_t ip_opt;    /* IP option (router alert option) */
    igmp_msg_t igmp_msg;               /* IGMP message */

} __attribute__((packed)) eth_igmp_frm_with_sgl_t;

/* IGMP message encapsulated in Ethernet frame IP packet structure */
typedef struct eth_igmp_frm_none_tag_none_opt_s
{
    eth_header_none_tag_t eth_hdr; /* Ethernet header with single tag */
    ipv4_header_t ip_hdr;              /* IP header */
    igmp_msg_t igmp_msg;               /* IGMP message */

} __attribute__((packed)) eth_igmp_frm_none_tag_none_opt_t;

/* IGMP message encapsulated in Ethernet frame IP packet structure */
typedef struct eth_igmp_frm_with_sgl_none_opt_s
{
    eth_header_with_sgl_tag_t eth_hdr; /* Ethernet header with single tag */
    ipv4_header_t ip_hdr;              /* IP header */
    igmp_msg_t igmp_msg;               /* IGMP message */

} __attribute__((packed)) eth_igmp_frm_with_sgl_none_opt_t;

typedef struct MC_IGMP_MSG_s
{
    uint32  lSeqNo; /* message queue mtype, if vxworks we should skip this */
    uint8 *pBuff;
    uint32 len;
    uint32 portId;
} MC_IGMP_MSG_t;

typedef enum MC_TIMER_MSG_TYPE_s
{
    MC_HOST_AGING_TIMER_SNOOPING,
    MC_HOST_AGING_TIMER_CTC,
    MC_SNOOP_NON_FAST_LEAVE,
    MC_CTC_LEAVE_ACTIVE
} MC_TIMER_MSG_TYPE_e;

typedef struct MC_TIMER_MSG_s
{
    uint32  lSeqNo; /* message queue mtype, if vxworks we should skip this */
    MC_TIMER_MSG_TYPE_e msgType;
    uint8 *desc;
} MC_TIMER_MSG_t;


//#define RETURN_VAL_IF_FAIL(condition, retval) do { if(!(condition)){ printf("%s,%d, error no: 0x%08X, %d, %s\n", __FUNCTION__, __LINE__, (unsigned int)retval, retval, NULL); return (retval); } } while(0)

#define RETURN_VAL_IF_FAIL(condition, retval) do { if(!(condition)){ MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, error no: 0x%08X, %d.\n", __FUNCTION__, __LINE__, (unsigned int)retval, retval); return (retval); } } while(0)

#define GROUP_COMPARE(node, hostPortId, groupMac, groupAddress, mcVlan, bitmap, compareResult) \
  { \
    compareResult = OPL_FALSE; \
    if(bitmap&MC_BITMAP_HOST_PORT_ID) { \
      if(node->hostPortId == hostPortId) { \
        compareResult |= MC_BITMAP_HOST_PORT_ID; \
      } \
    } \
    if(bitmap&MC_BITMAP_GDA_MAC) { \
      if(!vosMemCmp(node->groupMac, groupMac, MAC_ADDRESS_LENGTH)) { \
        compareResult |= MC_BITMAP_GDA_MAC; \
      } \
    } \
    if(bitmap&MC_BITMAP_GDA_IP) { \
      if(node->groupAddress == groupAddress) { \
        compareResult |= MC_BITMAP_GDA_IP; \
      } \
    } \
    if(bitmap&MC_BITMAP_VLAN_ID) { \
      if(node->mcVlan == mcVlan) { \
        compareResult |= MC_BITMAP_VLAN_ID; \
      } \
    } \
  }

#define HOST_COMPARE(node, saMac, srcIp, hostBitmap, compareResult) \
  { \
    compareResult = OPL_FALSE; \
    if(hostBitmap&MC_BITMAP_HOST_SRC_IP) { \
      if(node->srcIp == srcIp) { \
        compareResult |= MC_BITMAP_HOST_SRC_IP; \
      } \
    } \
    if(hostBitmap&MC_BITMAP_HOST_SA_MAC) { \
      if(!vosMemCmp(node->saMac, saMac, MAC_ADDRESS_LENGTH)) { \
        compareResult |= MC_BITMAP_HOST_SA_MAC; \
      } \
    } \
  }

#define MC_DEBUG(LEVEL, FMT, ...) \
  OP_DEBUG(LEVEL, FMT, ##__VA_ARGS__)

#define MAX_VLAN_ID 4095
#define IS_VALID_VLAN_ID(vid) (((vid)<=MAX_VLAN_ID) ? OPL_TRUE : OPL_FALSE)
#define IS_VALID_PORT_ID(pid) (((pid)>=ODM_START_PORT_NUN&&(pid)<=ODM_NUM_OF_PORTS) ? OPL_TRUE : OPL_FALSE)

typedef void (*mcHostAgingTimerHandler)(MC_TIMER_MSG_TYPE_e msgType, MC_PACKET_DESC_t *desc);

/***** STRUCTURES and UNIONS *****/

/***** TYPEDEFS *****/

/***** EXTERNS *****/
extern MC_VLAN_TRANSLATION_ENTRY_t mcTagTranslationTable[MC_VLAN_TRANSLATION_ENTRY];

extern multicast_control_e odmMulticastControlTypeGet(void);

/***** PROTOTYPES *****/

#endif /* MC_CONTROL_H */

/**
 * @}
 */

/* END FILE */

