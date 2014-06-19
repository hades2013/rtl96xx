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



//#include "errors.h"
//#include "log.h"

//#include "opl_driver.h"

//#include "opl_ethernet.h"

//#include "opl_host.h"
//#include "odm_port.h"

#include "opapi_comm_list.h"
#include "opapi_comm_inet.h"
#include "vos_timer.h"
#include <debug.h>

#include "lw_type.h"
#include "pdt_config.h"

//#include "hal.h"
//#include "hal_fdb.h"

//#include "multicast_control.h"

//#include "mc_control.h"
#define OPL_FALSE 0
#define OPL_TRUE 1

#define OPL_ENABLE 1
#define OPL_DISABLE 0
typedef int                                             OPL_BOOL;
typedef int                                                     OPL_STATUS;
#define PORT_PON_LOGIC                  LOGIC_PON_PORT    

#define ODM_START_PORT_NUN  1


/***** DEFINES and ENUMS *****/
#define ODM_NUM_OF_PORTS MAX_PORT_NUM

#define VLAN_DEFAULT_STPID    0x88a8
#define VLAN_DEFAULT_CTPID    0x8100

#define OP_MAX_VLAN_NUMBER            4096
#define MAC_ADDRESS_LENGTH            6

#define MC_ROUTER_PORT_NUM 1
#define MC_VLAN_TRANSLATION_ENTRY (16*ODM_NUM_OF_PORTS)
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

#define     ODM_MAX_GRUP_NUM_PER_PORT   64
#if 0
#define mc_printf printf
#else
#define mc_printf(fmt, ...) 
#endif
#define INVALID_32 ((unsigned int)0xffffffff)
#define NO_ERROR 0
#define MC_MAX_GROUP_NUM  ((ODM_NUM_OF_PORTS)*(ODM_MAX_GRUP_NUM_PER_PORT))
#define OPL_NULL                                ((void *) 0)
#define MIN_LOGIC_UNI_ID 1
#define MAX_LOGIC_UNI_ID MAX_PORT_NUM

#define IS_VALID_UNI_PORT(_port) ((_port >= MIN_LOGIC_UNI_ID) && (_port <= MAX_LOGIC_UNI_ID))
#define VALID_PORT_ID(_port) ((_port >= MIN_LOGIC_UNI_ID) && (_port <= MAX_LOGIC_PORT_NUM))

#pragma pack (1)
typedef enum tagMULTICAST_Tag_Oper_Mode
{
  MC_TAG_MODE_TRANSPARENT=0,
  MC_TAG_MODE_STRIP,
  MC_TAG_MODE_TRANSLATION,
  MC_TAG_MODE_END
} MC_TAG_MODE_E;

typedef enum DEBUG_LEVEL_s
{
    DEBUG_LEVEL_OFF,
    DEBUG_LEVEL_CRITICAL,
    DEBUG_LEVEL_WARNING,
    DEBUG_LEVEL_INFO,
    DEBUG_LEVEL_DEBUGGING,
    DEBUG_LEVEL_MAX
}DEBUG_LEVEL_e;


#define ETH_MAC_ADDR_LEN 6
#define ETH_MAC_ADDR_LEN 6
/* Ethernet vlan tag structure */
typedef struct eth_vlan_tag_s
{
    unsigned short   type;             /* Tag type */
#ifdef EOPL_LITTLE_BITFIELD
    unsigned short       vid:12,       /* Vlan ID */
                cfi:1,        /* CFI */
                pri:3;        /* Priority */
#else /* ifdef EOPL_BIG_BITFIELD */
    unsigned short       pri:3,        /* Priority */
                cfi:1,        /* CFI */
                vid:12;       /* Vlan ID */
#endif
}eth_vlan_tag_t;


typedef struct eth_header_with_sgl_tag_s
{
    unsigned char       dest[ETH_MAC_ADDR_LEN];      /* Destination Ethernet Address */
    unsigned char       src[ETH_MAC_ADDR_LEN];       /* Source Ethernet Address */
    eth_vlan_tag_t vlan_tag;           /* Vlan tag */
    unsigned short      protocol;                                /* Frame type */

}eth_header_with_sgl_tag_t;

typedef struct multicast_control_entry_s
{
        unsigned short port;                    /* Multicast Port or user ID*/
        unsigned short vid;                     /* Multicast vlan ID*/
        unsigned char  mmac[ETH_MAC_ADDR_LEN];   /* Group Destionation Address */

} multicast_control_entry_t;
#pragma pack ()

typedef enum multicast_control_s
{
        MC_CTL_GDA_MAC                          = 0x00,   /* GDA MAC (DA MAC Only) Based */
        MC_CTL_GDA_MAC_VID              = 0x01,   /* GDA MAC + multicast Valn ID based */
        MC_CTL_GDA_GDA_SA_MAC    = 0x02,   /* GDA MAC + SA MAC (IGMP v3 optional) based */
  MC_CTL_GDA_GDA_IP_VID    = 0x03,
        MC_CTL_END                                              /* End of multicast controls */

} multicast_control_e;

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
  CTC_CONTROL_BITMAP_GDA_MAC_VLAN_ID=MC_BITMAP_GDA_MAC|MC_BITMAP_HOST_PORT_ID|MC_BITMAP_VLAN_ID,/*Modified by huangmingjian 2013-12-12*/
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
  unsigned int mcRecvEn; /* if the igmp packets send to cpu */
  unsigned int mcMode; /* ctc, igmp snooping, igmp proxy ¡­ */
  unsigned int leaveMode; /* fast leave, non fast leave */
  unsigned int ctcControlType;
  unsigned int bitmap; /* match mac,ip,src */
  unsigned int hostBitmap; /* match srcMac,srcIp */

  unsigned int dnSwitchType; /* marvell, asseros */

  unsigned int hostAgingTime; /* 1s */
  unsigned int lastQueryInterval; /* 1ms */
  unsigned int lastQueryCount;

  unsigned int maxGroupNum[ODM_NUM_OF_PORTS+1];

  unsigned int stpid;
  unsigned int ctpid;

  unsigned int isInitialized;

  //unsigned int portControl[OP_UNI_PORT_NUMBER+1]; /* the port id is from 1 to X */
  //unsigned int vlanControl[OP_MAX_VLAN_NUMBER]; /* the vlan id is from 1 to 4095 */
  //unsigned int routerAgingTime;
} MC_CONTROL_t;

typedef struct MC_PACKET_DESC_s {
  unsigned char igmpGroupMac[MAC_ADDRESS_LENGTH];

  unsigned char daMac[MAC_ADDRESS_LENGTH];
  unsigned char saMac[MAC_ADDRESS_LENGTH];
  unsigned int sTpid;
  unsigned int sVlan; /* include cos */
  unsigned int cTpid;
  unsigned int cVlan; /* include cos */

  unsigned int destIp;
  unsigned int srcIp;

  unsigned int igmpVersion; /* 1, 2, 3 */
  unsigned int igmpType; /* join, leave, query */
  unsigned int igmpMaxRespTime; /* 8bit */
  unsigned int igmpGroupAdress; /* 32bit */

  unsigned int ingressPacketLength;
  unsigned char *ingressPacket; /* packet received from host */
  unsigned int ingressPortId;
  unsigned int ingressTagType; /*0,1,2 tags */
  unsigned int ingressIsHaveIpOptions;

  //unsigned int egressPacketLength;
  //unsigned char *egressPacket; /* packet send to host */
  unsigned int egressPortId;
  //unsigned int egressTagType;
  //unsigned int egressIsHaveIpOptions;

  TIMER_OBJ_t *nonFastLeaveTimer;

  unsigned int nonFastLeaveQueryPacketLength;
  unsigned char *nonFastLeaveQueryPacket;
  unsigned int nonFastLeaveEgressTagType;
  unsigned int nonFastLeaveEgressIsHaveIpOptions;

  unsigned int isNonFastLeaveTimerStart;
} MC_PACKET_DESC_t;

typedef struct MC_HOST_s {
  unsigned char saMac[MAC_ADDRESS_LENGTH]; /* the source mac in the igmp report packet */
  unsigned int srcIp; /* the source ip in the igmp report packet */

  unsigned int state; /* oam add/auto learning/cli add */

  unsigned int hostAgingTime; /* the aging time for this host */
  unsigned int isLastReportRecevied; /* use in non fast leave mode */
  unsigned int isNonFastLeaveTimerStart;  /* in snooping mode, if the timer exists, the leave packets received is dropped */
} MC_HOST_t;

typedef struct MC_GROUP_s {
  unsigned char groupMac[MAC_ADDRESS_LENGTH]; /* in ctc mode, ctcControlType=0x00 or 0x01 */
  unsigned char saMac[MAC_ADDRESS_LENGTH]; /* igmpv3 */
  unsigned int groupAddress; /* in ctc mode, ctcControlType=0x03 or in igmp snooping mode */
  unsigned int mcVlan; /* multicast vlan */
  unsigned int hostPortId;

  unsigned int state; /* oam add/auto learning/cli add */

  clList *hostList;
  unsigned int hostTotalNum;

  unsigned int hostAgingTime; /* use in ctc mode */
  unsigned int isLastReportRecevied; /* use in non fast leave mode */
  unsigned int isNonFastLeaveTimerStart; /* in ctc mode, if the timer exists, the leave packets received is dropped */
  unsigned int querycnt;
} MC_GROUP_t;

typedef struct MC_VLAN_TRANSLATION_ENTRY_s {
  unsigned int enable;
  unsigned int portId;
  unsigned int mcVid;
  unsigned int userVid;
} MC_VLAN_TRANSLATION_ENTRY_t;

typedef struct MC_ROUTER_PORT_ENTRY_s {
  unsigned int enable;
  unsigned int routerPortId;
} MC_ROUTER_PORT_ENTRY_t;

typedef struct MC_CONTROL_STATS_s
{
  /* IGMP Host Interface Statistics */
  unsigned int generalQueryV1RxCount;
  unsigned int generalQueryV2RxCount;
  unsigned int groupSpecificV2QueryRxCount;
  unsigned int otherHostRxUnknownCount;
  unsigned int otherRouterRxUnknownCount;

  unsigned int joinReportV1RxCount;
  unsigned int joinReportV2RxCount;
  unsigned int leaveV2RxCount;

  unsigned int memAllocCount;
  unsigned int memfreeCount;

} MC_CONTROL_STATS_t;
#pragma pack (1)

/* Ethernet header without dsa tag structure */
typedef struct eth_header_none_tag_s 
{
    unsigned char	dest[ETH_MAC_ADDR_LEN];	     /* Destination Ethernet Address */
    unsigned char	src[ETH_MAC_ADDR_LEN];	     /* Source Ethernet Address */
    unsigned short	protocol;		                 /* Frame type */

}eth_header_none_tag_t;     
/* IP header strcture */
typedef struct ipv4_header_s 
{

#ifdef EOPL_LITTLE_BITFIELD
     unsigned char	ihl:4,                   /* IP Header Length in 4 bytes */
	    version:4;                   /* IP version */ 
#else /* ifdef EOPL_BIG_BITFIELD */          
 unsigned char version:4,                   /* IP version */   
          ihl:4;                   /* IP Header Length in 4 bytes */
#endif                                      
    unsigned char	tos;                     /* Type of Service */
    unsigned short	total_len;               /* IP Total Length */
    unsigned short	id;                      /* Identifier */ 
    unsigned short	frag_off;                /* Fragment offset */
    unsigned char	ttl;                     /* Time to live */
    unsigned char  	protocol;                /* Protocol Type */
    unsigned short	checksum;                /* Checksum */
    unsigned int	src_addr;                /* Source Address */
    unsigned int	dest_addr;               /* Destination Address */   

	/* The options start here. */      
}ipv4_header_t;                             
/* IP option strcture */                                            
typedef struct ipv4_header_opt_s
{

#ifdef EOPL_LITTLE_BITFIELD
    unsigned char	option:5,               /* Option Number */
	       o_class:2,               /* Class */
           cflag:1;               /* Class Flag */   

#else /* EOPL_BIG_BITFIELD */              
     unsigned char  cflag:1,               /* Class Flag */ 
	       o_class:2,               /* Class */
	        option:5;               /* Option Number */   
#endif          
                          
    unsigned char  length;                   /* Option Length */ 
    
}ipv4_header_opt_t;                       


/* IP router alert option strcture */
typedef struct ipv4_router_alert_opt_s
{
    ipv4_header_opt_t type;         /* Option type */ 
    unsigned char  value[2];                 /* Option value */ 

}ipv4_router_alert_opt_t;   

/* General IGMP Message Header */
typedef struct igmp_msg_s
{
    unsigned char  type;           /* Type of IGMP message */
    unsigned char  max_resp_time;  /* Maximum response time for IGMP v2 */
    unsigned short checksum;       /* IP-style checksum */
    unsigned int group;          /* Group address */

}igmp_msg_t;

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
/* Host outbound header definition */
typedef struct host_outbound_hdr_s
{
        #ifdef EOPL_LITTLE_BITFIELD
        unsigned char eport:4,  /* Inbound  port index */
         icos:3,  /* ICOS */
     reserved:1;  /* Reserved */

  #else
        unsigned char reserved:1,  /* Reserved */
         icos:3,  /* ICOS */
        eport:4;  /* Inbound  port index */  /*changed by wxian for send port*/
  #endif

}host_outbound_hdr_t;

/* Host inbound header definition */
typedef struct host_inbound_hdr_s
{
        #ifdef EOPL_LITTLE_BITFIELD
   unsigned int iport:4,  /* Inbound  port index */
     reserved:28;  /* Reserved */
  #else
unsigned int reserved:28,  /* Reserved */
        iport:4;  /* Inbound  port index */
  #endif

}host_inbound_hdr_t;

/* IGMP message encapsulated in Ethernet frame IP packet structure */
typedef struct eth_igmp_frm_s
{
    host_outbound_hdr_t host_hdr;      /* Host outbound header */
    eth_header_with_sgl_tag_t eth_hdr; /* Ethernet header with single tag */
    ipv4_header_t ip_hdr;              /* IP header */
    ipv4_router_alert_opt_t ip_opt;    /* IP option (router alert option) */
    igmp_msg_t igmp_msg;               /* IGMP message */

} eth_igmp_frm_t;

/* IGMP message encapsulated in Ethernet frame IP packet structure */
typedef struct eth_igmp_frm_in_s
{
    host_inbound_hdr_t host_hdr;      /* Host outbound header */
    eth_header_with_sgl_tag_t eth_hdr; /* Ethernet header with single tag */
    ipv4_header_t ip_hdr;              /* IP header */
    ipv4_router_alert_opt_t ip_opt;    /* IP option (router alert option) */
    igmp_msg_t igmp_msg;               /* IGMP message */

} eth_igmp_frm_in_t;
#pragma pack ()


typedef struct MC_IGMP_MSG_s
{
    unsigned int  lSeqNo; /* message queue mtype, if vxworks we should skip this */
    unsigned char *pBuff;
    unsigned int len;
    unsigned int portId;
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
    unsigned int  lSeqNo; /* message queue mtype, if vxworks we should skip this */
    MC_TIMER_MSG_TYPE_e msgType;
    unsigned char *desc;
} MC_TIMER_MSG_t;
#if 0
#define OP_DEBUG(LEVEL, FMT, ...) \
    do { \
        printf(FMT, ##__VA_ARGS__); \
    } while (0)
#else
#define OP_DEBUG(LEVEL, FMT, ...)

#endif

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

#if 0
#define MC_DEBUG(LEVEL, FMT, ...) \
  OP_DEBUG(LEVEL, FMT, ##__VA_ARGS__)
#else
#define MC_DEBUG(LEVEL, FMT, ...)
#endif

#ifndef MAX_VLAN_ID
#define MAX_VLAN_ID 4095
#endif
#define IS_VALID_VLAN_ID(vid) (((vid)<=MAX_VLAN_ID) ? OPL_TRUE : OPL_FALSE)
#define IS_VALID_PORT_ID(pid) (((pid)>=ODM_START_PORT_NUN&&(pid)<=ODM_NUM_OF_PORTS) ? OPL_TRUE : OPL_FALSE)

typedef void (*mcHostAgingTimerHandler)(MC_TIMER_MSG_TYPE_e msgType, MC_PACKET_DESC_t *desc);

/***** STRUCTURES and UNIONS *****/

/***** TYPEDEFS *****/

/***** EXTERNS *****/
extern MC_VLAN_TRANSLATION_ENTRY_t mcTagTranslationTable[MC_VLAN_TRANSLATION_ENTRY];
extern UINT32 CTC_CheckValidVidForLgcPort(UINT32 lport, UINT32 uiMcVid, UINT8* pVlanMode,UINT8* pVlanExist);
extern multicast_control_e odmMulticastControlTypeGet(void);
extern int mctag_translation_check(unsigned int portId, unsigned int mcVid, unsigned int userVid);
/***** PROTOTYPES *****/

#endif /* MC_CONTROL_H */

/**
 * @}
 */

/* END FILE */

