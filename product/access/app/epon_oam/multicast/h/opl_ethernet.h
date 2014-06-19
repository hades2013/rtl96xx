/*
 * ===============================================================================
 * Header Name: opl_ethernet.h
 *
 * General Description:
 *    Structure and constants defines for Ethernet networks interface.  
 *
 * ===============================================================================
 * $Copyright: (c) 2007 OPulan Corp.
 * All Rights Reserved.$
 * ===============================================================================
 *  
 * Revision History:
 *                 
 * Author                Date              Description of Changes
 * ----------------   ------------  ----------------------------------------------
 * rzhou              12/12/2007    initial
 * ----------------   ------------  ----------------------------------------------
 *
 * ===============================================================================
 */
 
#ifndef OPL_ETHERNET_H
#define	OPL_ETHERNET_H 

#include "opl_host.h"

#define ETH_MAC_ADDR_LEN             6       /* Ethernet MAC address length */

#define ETH_P_IP	                   0x0800	 /* Ethernet Protocol IPV4 */

#define ETH_8021Q_STAG_TYPE          0x8100  /* Ethernet 802.1Q Service Tag Protocol Identifier */
#define ETH_8021Q_CTAG_TYPE          0x88A8  /* Ethernet 802.1Q Service Tag Protocol Identifier */

#define INET_IP_VER                  4       /* IP Packet Version */

#define IPV4_PROTOCOL_IGMP           2       /* IP protocol type IGMP */
#define IPV4_IGMP_TTL                1       /* IGMP packet IP TTL */

#define IP_ROUTER_ALERT_OPTION_NUM   0x14    /* IP Router Alert Option Number */

/* Allignment = 1 bytes start #pragma pack (1) */

#pragma pack (1)

/* Ethernet vlan tag structure */
typedef struct eth_vlan_tag_s 
{
    UINT16   type;             /* Tag type */
#ifdef EOPL_LITTLE_BITFIELD                
    UINT16       vid:12,       /* Vlan ID */
                cfi:1,        /* CFI */
                pri:3;        /* Priority */
#else /* ifdef EOPL_BIG_BITFIELD */        
    UINT16       pri:3,        /* Priority */
                cfi:1,        /* CFI */
                vid:12;       /* Vlan ID */
#endif                                    
}eth_vlan_tag_t;

/* Ethernet header without dsa tag structure */
typedef struct eth_header_none_tag_s 
{
    UINT8	dest[ETH_MAC_ADDR_LEN];	     /* Destination Ethernet Address */
    UINT8	src[ETH_MAC_ADDR_LEN];	     /* Source Ethernet Address */
    UINT16	protocol;		                 /* Frame type */

}eth_header_none_tag_t;     

/* Ethernet header with single vlan tag structure */
typedef struct eth_header_with_sgl_tag_s 
{
    UINT8	dest[ETH_MAC_ADDR_LEN];	     /* Destination Ethernet Address */
    UINT8	src[ETH_MAC_ADDR_LEN];	     /* Source Ethernet Address */
    eth_vlan_tag_t vlan_tag;           /* Vlan tag */
    UINT16	protocol;		                 /* Frame type */

}eth_header_with_sgl_tag_t;   

/* Ethernet header for raisecom */
typedef struct eth_header_raisecom_s 
{
    UINT8	dest[ETH_MAC_ADDR_LEN];	     /* Destination Ethernet Address */
    UINT8	src[ETH_MAC_ADDR_LEN];	     /* Source Ethernet Address */
    UINT16	protocol;		                 /* Frame type */
	UINT16	llid;

}eth_header_raisecom_t;   

/* IP header strcture */
typedef struct ipv4_header_s 
{

#ifdef EOPL_LITTLE_BITFIELD
     UINT8	ihl:4,                   /* IP Header Length in 4 bytes */
	    version:4;                   /* IP version */ 
#else /* ifdef EOPL_BIG_BITFIELD */          
 UINT8 version:4,                   /* IP version */   
          ihl:4;                   /* IP Header Length in 4 bytes */
#endif                                      
    UINT8	tos;                     /* Type of Service */
    UINT16	total_len;               /* IP Total Length */
    UINT16	id;                      /* Identifier */ 
    UINT16	frag_off;                /* Fragment offset */
    UINT8	ttl;                     /* Time to live */
    UINT8  	protocol;                /* Protocol Type */
    UINT16	checksum;                /* Checksum */
    UINT32	src_addr;                /* Source Address */
    UINT32	dest_addr;               /* Destination Address */   

	/* The options start here. */      
}ipv4_header_t;                             

/* IP option strcture */                                            
typedef struct ipv4_header_opt_s
{

#ifdef EOPL_LITTLE_BITFIELD
    UINT8	option:5,               /* Option Number */
	       o_class:2,               /* Class */
           cflag:1;               /* Class Flag */   

#else /* EOPL_BIG_BITFIELD */              
     UINT8  cflag:1,               /* Class Flag */ 
	       o_class:2,               /* Class */
	        option:5;               /* Option Number */   
#endif          
                          
    UINT8  length;                   /* Option Length */ 
    
}ipv4_header_opt_t;                       

/* IP router alert option strcture */
typedef struct ipv4_router_alert_opt_s
{
    ipv4_header_opt_t type;         /* Option type */ 
    UINT8  value[2];                 /* Option value */ 

}ipv4_router_alert_opt_t;   

/* General IGMP Message Header */
typedef struct igmp_msg_s
{
    UINT8  type;           /* Type of IGMP message */
    UINT8  max_resp_time;  /* Maximum response time for IGMP v2 */
    UINT16 checksum;       /* IP-style checksum */
    UINT32 group;          /* Group address */

}igmp_msg_t;

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

/* Allignment = 1 bytes end #pragma pack () */

#endif  /* OPL_ETHERNET_H */

