#ifndef RTK_RG_DEFINE_H
#define RTK_RG_DEFINE_H

/* System Module */
#define DEFAULT_CPU_VLAN 			1
#define DEFAULT_LAN_VLAN 			4000		//used for IP_version only_mode, as PVID to block traffic to other port
#define DEFAULT_WAN_PVID 			4001
#define DEFAULT_PPB_VLAN_WAN		4002		//used for wan port
#define DEFAULT_PPB_VLAN_START		4003		//used for each lan

#define DEFAULT_ROUTE_IDX		7
#define SLAVE_WIFI_ROUTE_IDX	6

#define LAN_FID					2
#define WAN_FID					2
#define NIC_RX_PRIORITY			16		//used to nic register for RX
#define Layer2HOUSE_KEEP_NUM	256		//how many entries should be check at once, MUST be the common divisor of LUT table

#define RTK_RG_INGRESS_QOS_TESTING	1

#define WIFI_FLOOD_INTF_RET		100

//#define PPPOE_DISCOVERY_GROUPID	0
//#define PPPOE_SESSION_GROUPID	1
/* Port and Protocol VLAN Group */
#define MAX_PORT_PROTO_GROUP_SIZE	4
#define RG_IPV4_GROUPID				0
#define RG_ARP_GROUPID				1
#define RG_IPV6_GROUPID				2
#define RG_IPV4_ETHERTYPE			0x0800
#define RG_ARP_ETHERTYPE			0x0806
#define RG_IPV6_ETHERTYPE			0x86dd
#define FRAGMENT_LIST_TIMEOUT	300

#define RTK_RG_SKB_PREALLOCATE	1

#if defined(CONFIG_DUALBAND_CONCURRENT)
#define SKB_BUF_SIZE  1800
#else
#define SKB_BUF_SIZE  1600
#endif

/* RTK RG Timeout Defination */
#ifdef __KERNEL__
#define TICKTIME_PERIOD		(CONFIG_HZ)	//How many jiffies per second
#endif
#define RTK_RG_FWDENGINE_HOUSE_KEEP_SECOND	2	//secs

#define RTK_RG_TCP_LONG_TIMEOUT 	60*60 // 1 hour
#define RTK_RG_TCP_SHORT_TIMEOUT	10	// 10 secs
#define RTK_RG_UDP_LONG_TIMEOUT 	10*60 // 10 mins
#define RTK_RG_UDP_SHORT_TIMEOUT	10	// 10 secs
#define RTK_RG_ARP_TIMEOUT 	300 // 300 secs


/* RTK RG API */
#define RTK_RG_MAX_MAC_PORT 7
#define RTK_RG_MAX_EXT_PORT (RTK_RG_PORT_MAX - RTK_RG_PORT_CPU)
#define RTK_RG_MAX_MAC_ALLPORTMASK ((1<<RTK_RG_MAC_PORT0)|(1<<RTK_RG_MAC_PORT1)|(1<<RTK_RG_MAC_PORT2)|(1<<RTK_RG_MAC_PORT3)|(1<<RTK_RG_MAC_PORT_PON)|(1<<RTK_RG_MAC_PORT_RGMII)|(1<<RTK_RG_MAC_PORT_CPU))
#define RTK_RG_MAX_EXT_ALLPORTMASK ((1<<RTK_RG_EXT_PORT0)|(1<<RTK_RG_EXT_PORT1)|(1<<RTK_RG_EXT_PORT2)|(1<<RTK_RG_EXT_PORT3)|(1<<RTK_RG_EXT_PORT4))

#define MAX_ASIC_NXTHOP_TBL_SIZE	32
#define MAX_ASIC_NETIF_TBL_SIZE		8	

/*ACL Module*/
	//setup ACL & CF ASIC
#define MIN_ACL_ENTRY_INDEX 	RESERVED_ACL_BEFORE
#define MAX_ACL_ENTRY_INDEX 	RESERVED_ACL_AFTER
#define MAX_ACL_ENTRY_SIZE 	RESERVED_ACL_AFTER //reserve default rule for drop/permit, url "GE", url "PO", PPPoE SessionID 0, PPPoE SessionID 1
#define MAX_ACL_IPRANGETABLE_SIZE 	8
#define MAX_ACL_PORTRANGETABLE_SIZE 16
#define MAX_ACL_TEMPLATE_SIZE 4 //this size also limit the acl multiple hit size(one aclSWEntry at most mapping to 4 acl ASIC entry)

#ifdef CONFIG_RG_PPPOE_PASSTHROUGHT
#define RESERVED_ACL_BEFORE 1
#define RESERVED_ACL_AFTER 45
#define RESERVED_ACL_TRAP_ALL 0 //TRAP ALL PKT TO CPU

#define RESERVED_ACL_CVLAN_TRANSLATE 46 //translate ingress CVLAN Tag for patch multicast in both cpu1 & cpu2 
#define RESERVED_ACL_EXTPORT_TRANSLATE_SIZE 2 //ACL[46~48]reserved for translate extPortmask
#define RESERVED_ACL_EXTPORT_TRANSLATE_BASE 49
#define RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_MAX_SIZE 8 //ACL[50~57] : permit pppoe downstream passthrough for each wanIntf  (index relate to RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE_FOR_ETHTER_8864)
#define RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE_FOR_ETHTER_8864 58 //ACL[58] remarking Ethertype 0x8864 to vid 1(bridge downstream passthrought). 
#define RESERVED_ACL_PPPoE_PASSTHROUGHT_UPSTREAM_FOR_ETHTER_8864 59 //ACL[59] trap all 0x8864 to CPU (all pppoe pass throught upstream handel by fwdEngine.)
#define RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE_FOR_ETHTER_8863 60 //ACL[60] trap Ethertype 0x8863 , (fwdEngine decide remarking or not)
#define RESERVED_ACL_MULTICAST_VID_TRANSLATE_FOR_IPV4 61
#define RESERVED_ACL_MULTICAST_VID_TRANSLATE_FOR_IPV6 62 
#define RESERVED_ACL_URLFILTER_ENTRY 63 //trap dport:80 pkt
#else //support for Mercury for saving ACL without using pppoe passthrought
#define RESERVED_ACL_BEFORE 1
#define RESERVED_ACL_AFTER 56
#define RESERVED_ACL_TRAP_ALL 0 //TRAP ALL PKT TO CPU

#define RESERVED_ACL_CVLAN_TRANSLATE 57 //translate ingress CVLAN Tag for patch multicast in both cpu1 & cpu2 
#define RESERVED_ACL_EXTPORT_TRANSLATE_SIZE 2 //ACL[56~58]reserved for translate extPortmask
#define RESERVED_ACL_EXTPORT_TRANSLATE_BASE 60
#define RESERVED_ACL_MULTICAST_VID_TRANSLATE_FOR_IPV4 61
#define RESERVED_ACL_MULTICAST_VID_TRANSLATE_FOR_IPV6 62 
#define RESERVED_ACL_URLFILTER_ENTRY 63 //trap dport:80 pkt
#endif



#define MIN_CF_ENTRY_SIZE 	0
#define MAX_CF_ENTRY_SIZE 	(64-4) //reserve default rule for drop/permit (need two entry, one for US, one for DS), binding mac learn need one entry
#define MAX_CF_IPRANGETABLE_SIZE 	8
#define MAX_CF_PORTRANGETABLE_SIZE  8
#define MAX_CF_DSCPTABLE_SIZE  8
#define RESERVED_CF_PPPOE_PASSTHROUGH_DOWNSTREAM_DMAC2CVID_ENTRY 60 //Latch with RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE_FOR_ETHTER_8864
#define RESERVED_CF_VLANBINDING_MAC_LEARN_ENTRY 61		//FPGA only contain 0~7, so modify this if runs on FPGA
#define RESERVED_CF_US_DEFAULT_ENTRY 62 
#define RESERVED_CF_DS_DEFAULT_ENTRY 63 

/*StormControl*/
#define MAX_STORMCONTROL_ENTRY_SIZE 32

/* Queue weights structure */
#define RTK_RG_MAX_NUM_OF_QUEUE 8


/* SVLAN Module*/
/* SVlan */
#define SVLANMBRTBL_SIZE 		64		// 64 member config
#define SVLANMC2STBL_SIZE 		8		// 8 MC2S, Multicast to SVID
#define SVLANC2STBL_SIZE 		128		// 128 C2S, CVID/PVID to SVID
#define SVLANSP2CTBL_SIZE 		128		// 128 SP2C, SVID to CVID/PVID

/* ALG Module */
#define MAX_ALG_FUNCTIONS		32
#define FTP_PORT_STR			0x504f5254
#define MAX_FTP_CTRL_FLOW_SIZE	16

#define PPTP_CTRL_MSG_TYPE		1		// 1 for Control Message
#define PPTP_MAGIC				0x1a2b3c4d
#define MAX_PPTP_SESSION_SIZE	16
#define MAX_ALG_SERV_IN_LAN_NUM		8


/* Table Database structure */
/* HW & SW Combine tables */
#define MAX_VLAN_SW_TABLE_SIZE	4096
#define MAX_VLAN_HW_TABLE_SIZE	4096
#define MAX_LUT_SW_TABLE_SIZE	2048
#define MAX_LUT_HW_TABLE_SIZE	2048
#define MAX_NETIF_SW_TABLE_SIZE	8
#define MAX_NETIF_HW_TABLE_SIZE	8
#define MAX_L3_SW_TABLE_SIZE	8
#define MAX_L3_HW_TABLE_SIZE	8
#define MAX_EXTIP_SW_TABLE_SIZE	8
#define MAX_EXTIP_HW_TABLE_SIZE	8
#define MAX_PPPOE_SW_TABLE_SIZE	8
#define MAX_PPPOE_HW_TABLE_SIZE	8
#define MAX_NEXTHOP_SW_TABLE_SIZE	16
#define MAX_NEXTHOP_HW_TABLE_SIZE	16
#define MAX_ARP_SW_TABLE_SIZE	1024
#define MAX_ARP_HW_TABLE_SIZE	512
#define MAX_NAPT_IN_SW_TABLE_SIZE	4096
#define MAX_NAPT_IN_HW_TABLE_SIZE	1024
#define MAX_NAPT_OUT_SW_TABLE_SIZE	4096
#define MAX_NAPT_OUT_HW_TABLE_SIZE	2048
#define MAX_BIND_SW_TABLE_SIZE	32
#define MAX_BIND_HW_TABLE_SIZE	32
#define MAX_WANTYPE_SW_TABLE_SIZE	8
#define MAX_WANTYPE_HW_TABLE_SIZE	8
#define MAX_IPV6_ROUTING_SW_TABLE_SIZE 4
#define MAX_IPV6_ROUTING_HW_TABLE_SIZE 4
#define MAX_IPV6_NEIGHBOR_SW_TABLE_SIZE 128
#define MAX_IPV6_NEIGHBOR_HW_TABLE_SIZE 128
#define MAX_IPMCGRP_HW_TABLE_SIZE 64
#define MAX_WLAN_MBSSID_SW_TABLE_SIZE 64


/* Software tables */
#define MAX_VIRTUAL_SERVER_SW_TABLE_SIZE	32
#define MAX_UPNP_SW_TABLE_SIZE				32
#define MAX_DMZ_TABLE_SIZE					MAX_NETIF_SW_TABLE_SIZE
#define MAX_IPV4_FRAGMENT_QUEUE_SIZE 		128		//TCP+UDP+ICMP
#define MAX_NAPT_SHORTCUT_SIZE 				16  //must power of 2
#define MAX_ICMPCTRLFLOW_SIZE				32
#define MAX_FRAG_IN_FREE_TABLE_SIZE			128
#define MAX_FRAG_OUT_FREE_TABLE_SIZE		128
#define MAX_ARP_SW_TABLE_HEAD				256
#define ARP_SW_TABLE_THRESHOLD				180
#define MAX_ARP_FOR_LAN_INTF				256		//preserve 256 entries in hw table for LAN intfs
#define MAX_ARP_FOR_WAN_INTF				256		//preserve 256 entries in hw table for WAN intfs
#define MAX_FRAGMENT_QUEUE_THRESHOLD		50		//if there are so many packets need to queue, the first packet may not come in anymore, so kick they all


/* IGMP/MLD snooping */
#define RTK_RG_MULTICAST_MODE_MACFID 0

/* common */
#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef FAIL
#define FAIL -1
#endif



/* debug */

#ifndef assert
#define assert(x)\
if(!(x)) {\
	if(rg_kernel.debug_level&RTK_RG_DEBUG_LEVEL_DEBUG) {\
		rtlglue_printf("\033[1;35m[ASSERT]'%s'\033[1;30m @%s:%s:%d\033[0m\n", #x,__FILE__,__FUNCTION__,__LINE__);\
	}\
}
#endif


#ifndef assert_ok
#define assert_ok(x)\
{ int assert_ret;\
	assert_ret=(x);\
	if(assert_ret!=0) { \
		if(rg_kernel.debug_level&RTK_RG_DEBUG_LEVEL_DEBUG) {\
        	rtlglue_printf("\033[1;35m[ASSERT_FAIL]'%s=0x%x'\033[1;30m @%s:%s:%d\033[0m\n",#x,assert_ret,__FILE__,__FUNCTION__,__LINE__);\
		}\
	}\
}
#endif

extern char mt_watch_tmp[512];

#ifndef COMMON_DUMP
#define COMMON_DUMP( bitmask, string, color,comment ,arg...) \
do {\
		if(rg_kernel.debug_level&bitmask)\
		{\
			int mt_trace_i;\
			sprintf( mt_watch_tmp, comment,## arg);\
			for(mt_trace_i=1;mt_trace_i<512;mt_trace_i++) \
			{ \
				if(mt_watch_tmp[mt_trace_i]==0) \
				{ \
					if(mt_watch_tmp[mt_trace_i-1]=='\n') mt_watch_tmp[mt_trace_i-1]=' '; \
					else break; \
				} \
			} \
			rtlglue_printf("\033[1;%dm[%s] %s \033[1;30m@%s:%d\033[0m\n",color,string,mt_watch_tmp,__FILE__,__LINE__); \
		} \
} while(0);
#endif

#ifndef DEBUG
#define DEBUG( comment ,arg...) COMMON_DUMP(RTK_RG_DEBUG_LEVEL_DEBUG,"DEBUG",33,comment,##arg)
#endif

#ifndef FIXME
#define FIXME( comment ,arg...) COMMON_DUMP(RTK_RG_DEBUG_LEVEL_FIXME,"FIXME",34,comment,##arg)
#endif

#ifndef TRACE
#define TRACE( comment ,arg...) \
{\
	int trace;\
	if(rg_kernel.debug_level&RTK_RG_DEBUG_LEVEL_TRACE)\
	{\
		trace=_rtk_rg_trace_filter_compare(skb);\
		if(trace==1)\
		{\
			COMMON_DUMP(RTK_RG_DEBUG_LEVEL_TRACE,"TRACE",35,comment,##arg);\
		}\
	}\
}
#endif

#ifndef ACL
#define ACL( comment ,arg...) COMMON_DUMP(RTK_RG_DEBUG_LEVEL_ACL,"ACL  ",36,comment,##arg)
#endif

#endif

