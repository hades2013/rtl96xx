#ifndef RTK_RG_STRUCT_H
#define RTK_RG_STRUCT_H
#ifdef __KERNEL__
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/semaphore.h>		//used to lock WAN interface L3 APIs
#include <linux/list.h>		//list_head and routines

#ifdef CONFIG_RTL8686NIC
#include <re8686.h>
#endif

#endif
#include <rtk/acl.h>
#include <rtk/classify.h>
#include <rtk/l2.h>
#include <rtk/l34.h>
#include <rtk_rg_define.h>



/* ERROR NO =============================================================== */
typedef enum rtk_rg_err_code_e
{
	RT_ERR_RG_OK=0,

	RT_ERR_RG_FAILED = (RT_ERR_COMMON_END+2), //0x10001
	RT_ERR_RG_NOT_SUPPORT_TESTCHIP,		//the API enable some feature not support by test chip			//unused
	RT_ERR_RG_BUF_OVERFLOW,	
	RT_ERR_RG_NULL_POINTER,				//input pointer is null
	RT_ERR_RG_INITPM_UNINIT,			//initparm's pointers is uninit					//0x10005

	RT_ERR_RG_PON_INVALID,				//PON port can not be set as WAN port in RLE0371
	RT_ERR_RG_INVALID_PARAM,			//parameters is invalid
	RT_ERR_RG_ENTRY_FULL,				//the entry table is full
	RT_ERR_RG_NOT_INIT,					//the module is not initl
	RT_ERR_RG_VLAN_BASED_OVERLAP_SUBNET,//diff VLAN-based interfaces can not have overlap subnet
	RT_ERR_RG_DEF_ROUTE_EXIST,			//the internet connection is exist				//0x1000b
	
	RT_ERR_RG_STATIC_NOT_FOUND,			//DHCP server index did not find				
	RT_ERR_RG_ARP_NOT_FOUND,			//ARP did not find
	RT_ERR_RG_ARP_FULL,					//ARP table overflow
	RT_ERR_RG_VLAN_BIND_UNINIT,			//vlan-binding is uninit
	RT_ERR_RG_PPPOE_UNINIT,				//pppoe_before is not called before				//0x10010
	
	RT_ERR_RG_CHIP_NOT_SUPPORT,			//the function is not supported in this chip version
	RT_ERR_RG_ENTRY_NOT_EXIST,		
	RT_ERR_RG_INDEX_OUT_OF_RANGE,											
	RT_ERR_RG_NO_MORE_ENTRY_FOUND,		
	RT_ERR_RG_MODIFY_LAN_AT_WAN_EXIST,	//deprecated, LAN intf can not add or delete when WAN exist		//0x10015
	
	RT_ERR_RG_LAN_NOT_EXIST,			//LAN intf did not created before WAN	
	RT_ERR_RG_GW_MAC_NOT_SET,			//for lite romeDriver, set WAN with default gw must has MAC address input
	RT_ERR_RG_VLAN_SET_FAIL,			//vlan set failed
	RT_ERR_RG_VLAN_GET_FAIL,			//vlan get failed
	RT_ERR_RG_INTF_SET_FAIL,			//interface set failed
	RT_ERR_RG_INTF_GET_FAIL,			//interface get failed							//0x1001b
	
	RT_ERR_RG_ROUTE_SET_FAIL,				//routing table set failed
	RT_ERR_RG_ROUTE_GET_FAIL,				//routing table get failed
	RT_ERR_RG_EXTIP_SET_FAIL,			//internal external IP table set failed
	RT_ERR_RG_EXTIP_GET_FAIL,			//internal external IP table get failed
	RT_ERR_RG_NXP_SET_FAIL,				//nexthop table set failed						//0x10020
	
	RT_ERR_RG_NXP_GET_FAIL,				//nexthop table get failed
	RT_ERR_RG_PPPOE_SET_FAIL,			//pppoe table set failed
	RT_ERR_RG_PPPOE_GET_FAIL,			//pppoe table get failed
	RT_ERR_RG_PORT_BIND_SET_FAIL,		//Port-binding set failed
	RT_ERR_RG_PORT_BIND_GET_FAIL,		//Port-binding get failed						//0x10025
	
	RT_ERR_RG_EXTPORT_BIND_SET_FAIL,	//extension Port-binding set failed
	RT_ERR_RG_EXTPORT_BIND_GET_FAIL,	//extension Port-binding get failed
	RT_ERR_RG_WANTYPE_SET_FAIL,			//WAN type table set failed
	RT_ERR_RG_WANTYPE_GET_FAIL,			//WAN type table get failed
	RT_ERR_RG_VLAN_BIND_SET_FAIL,		//VLAN-Port binding set failed					//0x1002a
	
	RT_ERR_RG_VLAN_BIND_GET_FAIL,		//VLAN-Port binding get failed
	RT_ERR_RG_ACL_CF_FIELD_CONFLICT,	//aclFilter assigned some conflict field(ex:ipv4 ipv6 at the same time, DMAC != ingress_INTF.gmac, CVID!= ingress_INTF.vid, can be reference to _rtk_rg_conflictField_and_flowDirection_check())
	RT_ERR_RG_ACL_CF_FLOW_DIRECTION_ERROR, //return this error while in lan_to_lan /wan_to_wan and assignde CF pattern (CF ASIC just support in upstream/dowmstream)
	RT_ERR_RG_ACL_ENTRY_FULL,			//ACL ASIC entry is full
	RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED,	//set/get ACL ASIC entry failed					//0x1002f
	
	RT_ERR_RG_ACL_IPTABLE_FULL,			//ACL IP range table ASIC full
	RT_ERR_RG_ACL_IPTABLE_ACCESS_FAILED,//set/get ACL IP range table ASIC failed 
	RT_ERR_RG_ACL_PORTTABLE_FULL,		//ACL port range table ASIC full
	RT_ERR_RG_ACL_PORTTABLE_ACCESS_FAILED,//set/get ACL port range table ASIC failed
	RT_ERR_RG_CF_ENTRY_FULL,			//CF ASIC entry is full						//0x10034
	
	RT_ERR_RG_CF_ENTRY_ACCESS_FAILED,	//set/get CF ASIC entry failed
	RT_ERR_RG_CF_IPTABLE_FULL,			//CF IP range table ASIC full
	RT_ERR_RG_CF_IPTABLE_ACCESS_FAILED,	//set/get CF IP range table ASIC failed 
	RT_ERR_RG_CF_PORTTABLE_FULL,		//CF port range table ASIC full
	RT_ERR_RG_CF_PORTTABLE_ACCESS_FAILED,//set/get ACL port range table ASIC failed			//0x10039
	
	RT_ERR_RG_CF_DSCPTABLE_FULL,		//CF dscp remarking table ASIC full
	RT_ERR_RG_CF_DSCPTABLE_ACCESS_FAILED,//set/get ACL dscp remarking table ASIC failed
	RT_ERR_RG_ACL_SW_ENTRY_FULL,		//software aclFilterEntry full
	RT_ERR_RG_ACL_SW_ENTRY_ACCESS_FAILED,//set/get ACL aclFilterEntry failed
	RT_ERR_RG_ACL_SW_ENTRY_NOT_FOUND,	//not found valid aclFilterEntry					//0x1003e
	
	RT_ERR_RG_ACL_SW_ENTRY_USED,		//set an aclFilterEntry which is in used(not used aclFilterEntry should clean to zero)
	RT_ERR_RG_L2_ENTRY_ACCESS_FAILED,		//set/get L2 ASIC entry failed
	RT_ERR_RG_L2_MACFILTER_ENTRY_ACCESS_FAILED,//access software macFulterEntry failed 
	RT_ERR_RG_L2_MACFILTER_ENTRY_FULL,		//software macFulterEntry is full (size defined by MAX_MAC_FILTER_ENTRY_SIZE)
	RT_ERR_RG_L2_MACFILTER_ENTRY_NOT_FOUND,	//not found valid macFilterEntry				//0x10043

	RT_ERR_RG_URLFILTER_ENTRY_ACCESS_FAILED, //access software urlFilterEntry failed 
	RT_ERR_RG_URLFILTER_ENTRY_FULL,		//software urlFulterEntry is full (size defined by MAX_URL_FILTER_ENTRY_SIZE)
	RT_ERR_RG_URLFILTER_ENTRY_NOT_FOUND,//not found valid software urlFilterEntry	
	RT_ERR_RG_SVRPORT_SW_ENTRY_NOT_FOUND,
	RT_ERR_RG_UPNP_SW_ENTRY_NOT_FOUND,											//0x10048
	
	RT_ERR_RG_NAPT_SW_ENTRY_NOT_FOUND,	//0x10049
	RT_ERR_RG_EXTIP_TYPE_MISMATCH,
	RT_ERR_RG_NAPT_SET_FAIL,				
	RT_ERR_RG_NAPT_GET_FAIL,
	RT_ERR_RG_NAPT_OVERFLOW,
	
	RT_ERR_RG_NAPTR_SET_FAIL,		//0x1004e
	RT_ERR_RG_NAPTR_GET_FAIL,
	RT_ERR_RG_NAPTR_OVERFLOW,
	RT_ERR_RG_NAPT_FLOW_DUPLICATE,
	RT_ERR_RG_NAPT_FLOW_OVERFLOW,
	
	RT_ERR_RG_ARP_MAPPING_OVERFLOW,	//0x10053
	RT_ERR_RG_ARP_ENTRY_STATIC,			//add ARP failed because ARP entry is exist and STATIC
	RT_ERR_RG_ARP_OVERFLOW,
	RT_ERR_RG_L2_ENTRY_NOT_FOUND,
	RT_ERR_RG_PPB_SET_FAILED,			//set port-and-protocol based vlan failed
	RT_ERR_RG_UNTAG_BRIDGEWAN_TWICE,	//set untag bridge WAN at same port twice
	RT_ERR_RG_NEIGHBOR_NOT_FOUND,				//Neighbor did not find
	RT_ERR_RG_NEIGHBOR_FULL,					//Neighbor table overflow				//0x1005a
	RT_ERR_RG_SUBNET_INTERFACE_ASYMMETRIC,		//new routing entry has same IP-range with other interface, but has different VLANID or MAC address
	
	RT_ERR_RG_VLAN_USED_BY_INTERFACE,			//the vlan entered is overlaped with interface setting
	RT_ERR_RG_VLAN_USED_BY_VLANBINDING,			//the vlan entered is overlaped with vlanBinding setting
	RT_ERR_RG_VLAN_USED_BY_CVLAN,				//the vlan entered is overlaped with customer vlan setting
	RT_ERR_RG_VLAN_NOT_CREATED_BY_CVLAN,				//the vlan entered is not created as customer vlan setting
	RT_ERR_RG_UNBIND_BDWAN_SHOULD_EQUAL_LAN_VLAN,		//un-bind bridge wan should equals to LAN's VLAN ID		//0x10060
	RT_ERR_RG_BIND_WITH_UNBIND_WAN,				//binding can not set to un-bind WAN
	RT_ERR_RG_CVLAN_CREATED,					//the customer vlan id had been created before
	RT_ERR_RG_CVLAN_RESERVED,					//the customer vlan id had been reserved by system
	RT_ERR_RG_ALG_SRV_IN_LAN_EXIST,				//the ALG Service In Lan service had been assigned
	RT_ERR_RG_ALG_SRV_IN_LAN_NO_IP,				//the ALG Service In Lan service did not assign server ip				//0x10065
	RT_ERR_RG_CREATE_GATEWAY_LUT_FAIL,			//get error when add gateway Mac entry in Interface_add
	RT_ERR_RG_DELETE_GATEWAY_LUT_FAIL,			//get error when del gateway Mac entry in Interface_del
	RT_ERR_RG_STORMCONTROL_TYPE_FULL,			//at most support 4 types
	RT_ERR_RG_STORMCONTROL_ENTRY_FULL,			//at most 16 entries(limit by sharemeter)
	RT_ERR_RG_STORMCONTROL_ENTRY_HAS_BEEN_SET,  //0x1006a
	RT_ERR_RG_STORMCONTROL_ENTRY_NOT_FOUND,
	RT_ERR_RG_SHAREMETER_SET_FAILED,
	RT_ERR_RG_SHAREMETER_GET_FAILED,
	RT_ERR_RG_SHAREMETER_INVALID_METER_INDEX,
	RT_ERR_RG_SHAREMETER_INVALID_RATE,	
	RT_ERR_RG_SHAREMETER_INVALID_INPUT,			//0x10070
	RT_ERR_RG_ADD_ARP_TO_SW_TABLE,				//the routing will direct go to fwdEngine for sw arp table
	RT_ERR_RG_ADD_ARP_MAC_FAILED,				//return FAIL when call _rtk_rg_arpAndMacEntryAdd
	RT_ERR_RG_PPPOEPASSTHROUGHT_NOT_SUPPORTED,
	RT_ERR_RG_IPMC_IP_LIST_OUT_OF_RANGE,		//IPMC include or exclude IP list out of range.
	RT_ERR_RG_IPMC_EXCLUDE_MODE_NOT_SUPPORT_EXTPORT,
}rtk_rg_err_code_t; 
/* End of ERROR NO ======================================================== */


/* DEBUG ================================================================= */

typedef enum rtk_rg_debug_level_e
{
	RTK_RG_DEBUG_LEVEL_DEBUG=0x1,
	RTK_RG_DEBUG_LEVEL_FIXME=0x2,
	RTK_RG_DEBUG_LEVEL_CALLBACK=0x4,
	RTK_RG_DEBUG_LEVEL_TRACE=0x8,
	RTK_RG_DEBUG_LEVEL_ACL=0x10,
	RTK_RG_DEBUG_LEVEL_ALL=0xffffffff,
} rtk_rg_debug_level_t;


typedef enum rtk_rg_debug_trace_filter_bitmask_e
{
	RTK_RG_DEBUG_TRACE_FILTER_SPA=1, //source port 0~5 phyiscal Port, 6:CPU, 7:EXT0, 8:EXT1
	RTK_RG_DEBUG_TRACE_FILTER_DA=2,	//DNAC
	RTK_RG_DEBUG_TRACE_FILTER_SA=4,	//SMAC
	RTK_RG_DEBUG_TRACE_FILTER_ETH=8, //ethertype
	RTK_RG_DEBUG_TRACE_FILTER_SIP=16, //src IP
	RTK_RG_DEBUG_TRACE_FILTER_DIP=32, //dest IP	
} rtk_rg_debug_trace_filter_bitmask_t;

typedef struct rtk_rgDebugTraceFilter_s
{
	uint16		spa;	
	rtk_mac_t	dmac;
	rtk_mac_t	dmac_mask;
	rtk_mac_t	smac;
	rtk_mac_t	smac_mask;
	uint16		ethertype;
	uint32		sip;
	uint32		dip;	
}rtk_rgDebugTraceFilter_t;




/* End of DEBUG ========================================================== */


/* RTK RG API ============================================================= */

typedef enum rtk_rg_mac_port_idx_e
{
#ifdef CONFIG_APOLLO_RLE0371
	RTK_RG_MAC_PORT0=0,
	RTK_RG_MAC_PORT1=1,
	RTK_RG_MAC_PORT_PON=3,
	RTK_RG_MAC_PORT_RGMII=2,
	RTK_RG_MAC_PORT2=4,
	RTK_RG_MAC_PORT3=5,
	RTK_RG_MAC_PORT_CPU=6,
	RTK_RG_MAC_PORT_MAX,
#else
	RTK_RG_MAC_PORT0=0,
	RTK_RG_MAC_PORT1,
	RTK_RG_MAC_PORT2,
	RTK_RG_MAC_PORT3,
	RTK_RG_MAC_PORT_PON,
	RTK_RG_MAC_PORT_RGMII,
	RTK_RG_MAC_PORT_CPU,
	RTK_RG_MAC_PORT_MAX,
#endif
} rtk_rg_mac_port_idx_t;

typedef struct rtk_rg_mac_portmask_s
{
	uint32 portmask;	//the portmask bit should be defined by rtk_rg_mac_port_idx_t.
}rtk_rg_mac_portmask_t;


typedef enum rtk_rg_port_idx_e
{
#ifdef CONFIG_APOLLO_RLE0371
	RTK_RG_PORT0=0,
	RTK_RG_PORT1=1,
	RTK_RG_PORT_PON=2,
	RTK_RG_PORT_RGMII=3,
	RTK_RG_PORT2=4,
	RTK_RG_PORT3=5,
	RTK_RG_PORT_CPU=6,
#else
	RTK_RG_PORT0=0,
	RTK_RG_PORT1,
	RTK_RG_PORT2,
	RTK_RG_PORT3,
	RTK_RG_PORT_PON,
	RTK_RG_PORT_RGMII,
	RTK_RG_PORT_CPU,
#endif
	RTK_RG_EXT_PORT0=7,
	RTK_RG_EXT_PORT1,
	RTK_RG_EXT_PORT2,
	RTK_RG_EXT_PORT3,
	RTK_RG_EXT_PORT4,
	RTK_RG_PORT_MAX,
} rtk_rg_port_idx_t;

typedef struct rtk_rg_portmask_e
{
	uint32 portmask;	//the portmask bit should be defined by rtk_rg_port_idx_t.
}rtk_rg_portmask_t;

typedef enum rtk_rg_binding_idx_e
{
	RTK_RG_BD_EXT_PORT0=0,
	RTK_RG_BD_EXT_PORT1=1,
	RTK_RG_BD_EXT_PORT2=2,
	RTK_RG_BD_EXT_PORT3=3,
	RTK_RG_BD_EXT_PORT4=4,
	RTK_RG_BD_PORT_MAX,
} rtk_rg_binding_idx_t;

//System
typedef enum rtk_rg_ip_version_e
{
	IPVER_V4ONLY=0,
	IPVER_V6ONLY=1,
	IPVER_V4V6=2,
}rtk_rg_ip_version_t;

typedef struct rtk_rg_VersionString_s
{
	char version_string[64];
} rtk_rg_VersionString_t;

typedef struct rtk_rg_macEntry_s
{
	rtk_mac_t mac;
	int isIVL; //0:SVL, 1:IVL
	int fid; //only used in SVL
	int vlan_id; //egress to this MAC, add a CVLAN tag. (vlan_id=0, untag)	
	rtk_rg_port_idx_t port_idx;
	int arp_used;
	int static_entry;
}rtk_rg_macEntry_t;

typedef struct rtk_rg_arpEntry_s
{
	int macEntryIdx;
	ipaddr_t ipv4Addr;
	int staticEntry;	
}rtk_rg_arpEntry_t;

typedef struct rtk_rg_arpInfo_s
{
	rtk_rg_arpEntry_t arpEntry;
	int valid;
	int idleSecs;
}rtk_rg_arpInfo_t;

typedef struct rtk_rg_neighborEntry_s
{
	unsigned int l2Idx;
	unsigned char matchRouteIdx;
	unsigned char interfaceId[8];
	unsigned char valid;
	unsigned char staticEntry;
}rtk_rg_neighborEntry_t;

typedef struct rtk_rg_neighborInfo_s
{
	rtk_rg_neighborEntry_t neighborEntry;
	int idleSecs;
}rtk_rg_neighborInfo_t;


typedef enum rtk_rg_portMirrorInfo_direction_e
{
	RTK_RG_MIRROR_TX_RX_BOTH=0,
	RTK_RG_MIRROR_RX_ONLY,
	RTK_RG_MIRROR_TX_ONLY,
	RTK_RG_MIRROR_END
} rtk_rg_portMirrorInfo_direction_t;

typedef struct rtk_rg_portMirrorInfo_s
{
	uint32 monitorPort;
	rtk_rg_mac_portmask_t enabledPortMask;
	rtk_rg_portMirrorInfo_direction_t direct;
}rtk_rg_portMirrorInfo_t;



typedef enum rtk_rg_enable_e
{
    RTK_RG_DISABLED = 0,
    RTK_RG_ENABLED,
    RTK_RG_ENABLE_END
} rtk_rg_enable_t;
typedef enum rtk_rg_port_speed_e
{
    RTK_RG_PORT_SPEED_10M = 0,
    RTK_RG_PORT_SPEED_100M,
    RTK_RG_PORT_SPEED_1000M,
    RTK_RG_PORT_SPEED_END,
} rtk_rg_port_speed_t;
typedef enum rtk_rg_port_duplex_e
{
    RTK_RG_PORT_HALF_DUPLEX = 0,
    RTK_RG_PORT_FULL_DUPLEX,
    RTK_RG_PORT_DUPLEX_END
} rtk_rg_port_duplex_t;
typedef struct rtk_rg_phyPortAbilityInfo_s
{
	rtk_rg_enable_t			force_disable_phy;
	rtk_rg_enable_t 		valid;
	rtk_rg_port_speed_t		speed;
	rtk_rg_port_duplex_t	duplex;
	rtk_rg_enable_t			flowCtrl;
}rtk_rg_phyPortAbilityInfo_t;


typedef enum rtk_rg_storm_type_e
{
    RTK_RG_STORM_TYPE_UNKNOWN_UNICAST = 0,
    RTK_RG_STORM_TYPE_UNKNOWN_MULTICAST,
    RTK_RG_STORM_TYPE_MULTICAST,
    RTK_RG_STORM_TYPE_BROADCAST,
    RTK_RG_STORM_TYPE_DHCP,
    RTK_RG_STORM_TYPE_ARP,
    RTK_RG_STORM_TYPE_IGMP_MLD,
    RTK_RG_STORM_TYPE_END
} rtk_rg_storm_type_t;

typedef struct rtk_rg_stormControlInfo_s
{
	rtk_rg_enable_t		valid;
	rtk_rg_port_idx_t	port;
	rtk_rg_storm_type_t stormType;
	uint32 				meterIdx;
}rtk_rg_stormControlInfo_t;

typedef struct rtk_rg_qos_queue_weights_s
{
    uint32 weights[RTK_RG_MAX_NUM_OF_QUEUE];
} rtk_rg_qos_queue_weights_t;


typedef struct rtk_rg_ipv4RoutingEntry_s
{
	ipaddr_t dest_ip;
	ipaddr_t ip_mask;
	ipaddr_t nexthop; //0:for Interface route
	int wan_intf_idx;
} rtk_rg_ipv4RoutingEntry_t;

typedef struct rtk_rg_ipv6RoutingEntry_s
{
	rtk_ipv6_addr_t dest_ip;
	int prefix_len;
	int NhOrIntfIdx;
	rtk_l34_ipv6RouteType_t type;
} rtk_rg_ipv6RoutingEntry_t;

typedef enum rtk_rg_binding_type_e
{
	BIND_TYPE_PORT,
	BIND_TYPE_VLAN,
} rtk_rg_binding_type_t;

typedef struct rtk_rg_vlanBindingEntry_e
{
	rtk_rg_port_idx_t vlan_bind_port_idx;
	int vlan_bind_vlan_id;
} rtk_rg_vlanBindingEntry_t;


typedef struct rtk_rg_bindingEntry_s
{
	rtk_rg_binding_type_t type;
	union
	{
		rtk_rg_portmask_t port_bind_pmask;
		rtk_rg_vlanBindingEntry_t vlan;
	};	
	int wan_intf_idx;
} rtk_rg_bindingEntry_t;

//NAPT Flow

typedef enum rtk_rg_naptState_e
{
	INVALID		=0,
	SYN_RECV	=1,
	UDP_FIRST	=2,
	SYN_ACK_RECV =3,
	UDP_SECOND	=4,
	TCP_CONNECTED	=5,
	UDP_CONNECTED	=6,	
	FIN_RECV	=7,
	RST_RECV	=8,	
} rtk_rg_naptState_t;


typedef struct rtk_rg_naptEntry_s
{
	int is_tcp;
	ipaddr_t local_ip;
	ipaddr_t remote_ip;
	int wan_intf_idx;
	unsigned short int local_port;
	unsigned short int remote_port;
	unsigned short int external_port;
	unsigned char outbound_pri_valid;
	unsigned char outbound_priority;
	unsigned char inbound_pri_valid;
	unsigned char inbound_priority;
	
} rtk_rg_naptEntry_t;


typedef enum  rtk_rg_naptDirection_e
{
	NAPT_DIRECTION_OUTBOUND	=0,
	NAPT_DIRECTION_INBOUND	=1,
}rtk_rg_naptDirection_t;

typedef struct rtk_rg_naptInfo_s
{
	rtk_rg_naptEntry_t naptTuples;
	uint32	idleSecs;
	rtk_rg_naptState_t	state;
} rtk_rg_naptInfo_t;

#ifdef RTK_RG_INGRESS_QOS_TESTING
typedef enum rtk_rg_ingress_qos_testing_type_s
{
	RTK_RG_INGRESS_QOS_ORIGINAL = 0,
	RTK_RG_INGRESS_QOS_ALL_HIGH_QUEUE,
	RTK_RG_INGRESS_QOS_STRICT_HIGH_QUEUE,
} rtk_rg_ingress_qos_testing_type_t;
#endif

#if 1
typedef enum rtk_rg_lease_time_type_e
{
	LEASE_TIME_TYPE_1WEEK=0,
	LEASE_TIME_TYPE_1DAY=1,
	LEASE_TIME_TYPE_1HOUR=2,
	LEASE_TIME_TYPE_1MIN=3,
} rtk_rg_lease_time_type_t;
#endif

typedef enum rtk_rg_house_keep_select_e
{
#if defined(CONFIG_RG_NAPT_AUTO_AGEOUT)
	RTK_RG_NAPT_HOUSE_KEEP_SELECT,
#endif
#if defined(CONFIG_RG_LAYER2_SOFTWARE_LEARN)
	RTK_RG_LAYER2_HOUSE_KEEP_SELECT,
#endif
#if defined(CONFIG_RG_ARP_AUTO_AGEOUT)
	RTK_RG_ARP_HOUSE_KEEP_SELECT,
#endif
	RTK_RG_MAX_HOUSE_KEEP_SELECT,
} rtk_rg_house_keep_select_t;


// LAN Interface
typedef struct rtk_rg_lanIntfConf_s
{	
	rtk_rg_ip_version_t ip_version;		//0: ipv4, 1: ipv6, 2:both v4 & v6
	rtk_mac_t gmac;
	ipaddr_t ip_addr;
	ipaddr_t ip_network_mask;		
	rtk_ipv6_addr_t ipv6_addr;
	int ipv6_network_mask_length;
	rtk_rg_portmask_t port_mask;
	rtk_rg_mac_portmask_t untag_mask;
	//rtk_portmask_t port_mask;
	//rtk_portmask_t extport_mask;
	int intf_vlan_id;
#if 0	
	int dhcp_server_enable; 
	rtk_rg_lease_time_type_t lease_time;
	ipaddr_t dhcp_start_ip_addr;
	ipaddr_t dhcp_end_ip_addr;
	rtk_rg_portmask_t dhcp_port_binding_mask;
	//rtk_portmask_t dhcp_port_binding_mask;
	//rtk_portmask_t dhcp_extport_binding_mask;
#endif		
	int mtu;
	//int pppoe_passThrough;		//1:turn on, 0:turn off
	int isIVL;		//0: SVL, 1:IVL
} rtk_rg_lanIntfConf_t;



//WAN Interface
typedef enum rtk_rg_wan_type_e
{
	RTK_RG_STATIC=0,
	RTK_RG_DHCP=1,
	RTK_RG_PPPoE=2,
	RTK_RG_BRIDGE=3,
	//IPV6_PPP=4,
	//IPV6_IP=5,
	//IPV4V6_PPP=6,
	//IPV4V6_IP=7
} rtk_rg_wan_type_t;

typedef struct rtk_rg_wanIntfConf_s
{	
	rtk_rg_wan_type_t wan_type;
	rtk_mac_t gmac;
	//rtk_portmask_t wan_port_mask;	//PON or RGMII
	rtk_rg_mac_port_idx_t wan_port_idx;
	rtk_rg_portmask_t port_binding_mask;
	//rtk_portmask_t port_binding_mask; //LAN port
	//rtk_portmask_t extport_binding_mask; //WLAN port
	int egress_vlan_tag_on;
	int egress_vlan_id;
	int egress_vlan_pri;
	int isIVL;		//0: SVL, 1:IVL
} rtk_rg_wanIntfConf_t;

typedef struct rtk_rg_ipStaticInfo_s
{
	rtk_rg_ip_version_t ip_version;		//0: ipv4, 1: ipv6, 2:both v4 & v6
	int napt_enable; // L3 or L4
	ipaddr_t ip_addr;
	ipaddr_t ip_network_mask;
	int ipv4_default_gateway_on;				//1:should set default route, 0:otherwise
	ipaddr_t gateway_ipv4_addr;
	rtk_ipv6_addr_t ipv6_addr;
	int ipv6_mask_length;
	int ipv6_default_gateway_on;			//1:should set default route, 0:otherwise
	rtk_ipv6_addr_t gateway_ipv6_addr;
	int mtu;
	int gw_mac_auto_learn_for_ipv4;
	int gw_mac_auto_learn_for_ipv6;
	rtk_mac_t gateway_mac_addr_for_ipv4;
	rtk_mac_t gateway_mac_addr_for_ipv6;
} rtk_rg_ipStaticInfo_t;

typedef enum rtk_rg_dhcp_status_e
{
	DHCP_STATUS_LEASED=0,
	DHCP_STATUS_RELEASED=1
} rtk_rg_dhcp_status_t;

typedef struct rtk_rg_ipDhcpClientInfo_s
{
	rtk_rg_ipStaticInfo_t hw_info;
	rtk_rg_dhcp_status_t stauts; //leased, released
} rtk_rg_ipDhcpClientInfo_t;

typedef enum rtk_rg_ppp_auth_type_e
{
	PPP_AUTH_TYPE_PAP=0,
	PPP_AUTH_TYPE_CHAP=1
} rtk_rg_ppp_auth_type_t;

typedef int (*p_dialOnDemondCallBack)(unsigned int);
typedef int (*p_idleTimeOutCallBack)(unsigned int);

typedef enum rtk_rg_ppp_status_e
{
	PPP_STATUS_DISCONNECT=0,
	PPP_STATUS_CONNECT=1
} rtk_rg_ppp_status_t;

typedef struct rtk_rg_pppoeClientInfoBeforeDial_s
{
	char username[32];
	char password[32]; 
	rtk_rg_ppp_auth_type_t auth_type;
	int pppoe_proxy_enable;
	int max_pppoe_proxy_num;
	int auto_reconnect;
	int dial_on_demond;
	int idle_timeout_secs;
	rtk_rg_ppp_status_t stauts; //connect/disconnect?
	p_dialOnDemondCallBack dialOnDemondCallBack;
	p_idleTimeOutCallBack idleTimeOutCallBack;
} rtk_rg_pppoeClientInfoBeforeDial_t;

typedef struct rtk_rg_pppoeClientInfoAfterDial_s
{
	rtk_rg_ipStaticInfo_t hw_info;
	unsigned short int sessionId;
} rtk_rg_pppoeClientInfoAfterDial_t;

typedef struct rtk_rg_ipPppoeClientInfo_s
{
	rtk_rg_pppoeClientInfoBeforeDial_t before_dial;
	rtk_rg_pppoeClientInfoAfterDial_t after_dial;
} rtk_rg_ipPppoeClientInfo_t;

typedef struct rtk_rg_wanIntfInfo_s
{
	rtk_rg_wanIntfConf_t wan_intf_conf;
	union{
		rtk_rg_ipStaticInfo_t static_info;
		rtk_rg_ipDhcpClientInfo_t	dhcp_client_info;
		rtk_rg_ipPppoeClientInfo_t pppoe_info;
	};
	rtk_mac_t next_hop_mac;
} rtk_rg_wanIntfInfo_t;

typedef struct rtk_rg_intfInfo_s
{
	char intf_name[32];
	int is_wan;
	union 
	{
		rtk_rg_lanIntfConf_t lan_intf;
		rtk_rg_wanIntfInfo_t wan_intf;
	};
	long long int ingress_packet_count;
	long long int ingress_byte_count;
	long long int egress_packet_count;
	long long int egress_byte_count;
} rtk_rg_intfInfo_t;

//VLAN function
typedef struct rtk_rg_cvlan_info_s
{
	int vlanId;
	int isIVL;		//0: SVL, 1:IVL
	rtk_rg_portmask_t memberPortMask;
	rtk_rg_mac_portmask_t untagPortMask;
	int priority;
}rtk_rg_cvlan_info_t;

//VLAN Binding
typedef struct rtk_rg_vlanBinding_s
{
	rtk_rg_port_idx_t port_idx;
	//rtk_portmask_t port_mask;
	//rtk_portmask_t ext_port_mask;
	int ingress_vid;
	int wan_intf_idx;
}rtk_rg_vlanBinding_t;

//DMZ
typedef struct rtk_rg_dmzInfo_s
{
	int enabled;
	int mac_mapping_enabled;
	union
	{
		ipaddr_t private_ip; //only used when mac_mapping_enabled=0
		rtk_mac_t mac; //only used when mac_mapping_enabled=1
	};
} rtk_rg_dmzInfo_t;

//VirtualServer(PortForward)
typedef struct rtk_rg_virtualServer_s
{
	int is_tcp;
	int wan_intf_idx; //for gateway ip
	int gateway_port_start;
	ipaddr_t local_ip;
	unsigned short int local_port_start;
	int mappingPortRangeCnt;
	int valid;
} rtk_rg_virtualServer_t;

//Multicast flow
typedef struct rtk_rg_multicastFlow_s
{
	ipaddr_t 	multicast_ipv4_addr;
	ipaddr_t	multicast_ipv6_addr[4];
	int		isIPv6;	
	int src_intf_idx;
	rtk_rg_portmask_t port_mask;
} rtk_rg_multicastFlow_t;

typedef struct rtk_rg_l2MulticastFlow_s
{
	rtk_mac_t mac;
	rtk_rg_portmask_t port_mask;
} rtk_rg_l2MulticastFlow_t;


typedef enum rtk_rg_ipv4MulticastFlowMode_e
{
	RTK_RG_IPV4MC_DONT_CARE_SRC=0,
	RTK_RG_IPV4MC_INCLUDE,
	RTK_RG_IPV4MC_EXCLUDE,
} rtk_rg_ipv4MulticastFlowMode_t;

#define MAX_IPMC_IP_LIST 8
typedef struct rtk_rg_ipv4MulticastFlow_s
{
	ipaddr_t groupIp;	
	rtk_rg_ipv4MulticastFlowMode_t srcFilterMode;
	int maxIpListNums;
	ipaddr_t includeOrExcludeIpList[MAX_IPMC_IP_LIST];
	union{
		rtk_rg_portmask_t includeModePortmaskList[MAX_IPMC_IP_LIST];
		rtk_rg_portmask_t excludeModePortmask;
		rtk_rg_portmask_t dontCareSipModePortmask;
	};	
} rtk_rg_ipv4MulticastFlow_t;


//ALG
typedef enum rtk_rg_alg_function_sequence_e
{
	//Server in WAN
	RTK_RG_ALG_SIP_TCP=0,		
	RTK_RG_ALG_SIP_UDP,
	RTK_RG_ALG_H323_TCP,
	RTK_RG_ALG_H323_UDP,
	RTK_RG_ALG_RTSP_TCP,
	RTK_RG_ALG_RTSP_UDP,
	RTK_RG_ALG_FTP_TCP,
	RTK_RG_ALG_FTP_UDP,			//7

	//Server in LAN
	RTK_RG_ALG_SIP_TCP_SRV_IN_LAN,
	RTK_RG_ALG_SIP_UDP_SRV_IN_LAN,
	RTK_RG_ALG_H323_TCP_SRV_IN_LAN,
	RTK_RG_ALG_H323_UDP_SRV_IN_LAN,
	RTK_RG_ALG_RTSP_TCP_SRV_IN_LAN,
	RTK_RG_ALG_RTSP_UDP_SRV_IN_LAN,
	RTK_RG_ALG_FTP_TCP_SRV_IN_LAN,
	RTK_RG_ALG_FTP_UDP_SRV_IN_LAN,		//15

	//Pass through
	RTK_RG_ALG_PPTP_TCP_PASSTHROUGH,
	RTK_RG_ALG_PPTP_UDP_PASSTHROUGH,
	RTK_RG_ALG_L2TP_TCP_PASSTHROUGH,
	RTK_RG_ALG_L2TP_UDP_PASSTHROUGH,
	RTK_RG_ALG_IPSEC_TCP_PASSTHROUGH,
	RTK_RG_ALG_IPSEC_UDP_PASSTHROUGH,		//21
	
	RTK_RG_ALG_PPPOE_PASSTHROUGH,
}rtk_rg_alg_function_sequence_t;

typedef enum rtk_rg_alg_type_e
{
	//Server in WAN
	RTK_RG_ALG_SIP_TCP_BIT					=0x1,
	RTK_RG_ALG_SIP_UDP_BIT					=0x2,
	RTK_RG_ALG_H323_TCP_BIT					=0x4,
	RTK_RG_ALG_H323_UDP_BIT					=0x8,
	RTK_RG_ALG_RTSP_TCP_BIT					=0x10,
	RTK_RG_ALG_RTSP_UDP_BIT					=0x20,
	RTK_RG_ALG_FTP_TCP_BIT					=0x40,
	RTK_RG_ALG_FTP_UDP_BIT					=0x80,			//8

	//Server in LAN
	RTK_RG_ALG_SIP_TCP_SRV_IN_LAN_BIT		=0x100,
	RTK_RG_ALG_SIP_UDP_SRV_IN_LAN_BIT		=0x200,
	RTK_RG_ALG_H323_TCP_SRV_IN_LAN_BIT		=0x400,
	RTK_RG_ALG_H323_UDP_SRV_IN_LAN_BIT		=0x800,
	RTK_RG_ALG_RTSP_TCP_SRV_IN_LAN_BIT		=0x1000,
	RTK_RG_ALG_RTSP_UDP_SRV_IN_LAN_BIT		=0x2000,
	RTK_RG_ALG_FTP_TCP_SRV_IN_LAN_BIT		=0x4000,
	RTK_RG_ALG_FTP_UDP_SRV_IN_LAN_BIT		=0x8000,		//16

	//Pass through
	RTK_RG_ALG_PPTP_TCP_PASSTHROUGH_BIT		=0x10000,
	RTK_RG_ALG_PPTP_UDP_PASSTHROUGH_BIT		=0x20000,
	RTK_RG_ALG_L2TP_TCP_PASSTHROUGH_BIT		=0x40000,
	RTK_RG_ALG_L2TP_UDP_PASSTHROUGH_BIT		=0x80000,
	RTK_RG_ALG_IPSEC_TCP_PASSTHROUGH_BIT	=0x100000,
	RTK_RG_ALG_IPSEC_UDP_PASSTHROUGH_BIT	=0x200000,		//22
	
	RTK_RG_ALG_PPPOE_PASSTHROUGH_BIT		=0x400000,
}rtk_rg_alg_type_t;

typedef enum rtk_rg_alg_tcpKnownPort_e
{
	RTK_RG_ALG_SIP_TCP_PORT=5060,
	RTK_RG_ALG_H323_TCP_PORT=1720,
	RTK_RG_ALG_RTSP_TCP_PORT=554,
	RTK_RG_ALG_PPTP_PASSTHROUGH_TCP_PORT=1723,
	RTK_RG_ALG_L2TP_PASSTHROUGH_TCP_PORT=1707,
	RTK_RG_ALG_IPSEC_PASSTHROUGH_TCP_PORT=1293,
	RTK_RG_ALG_FTP_TCP_PORT=21,
	RTK_RG_ALG_FTP_DATA_TCP_PORT=20,
}rtk_rg_alg_tcpKnownPort_t;

typedef enum rtk_rg_alg_udpKnownPort_e
{
	RTK_RG_ALG_SIP_UDP_PORT=5060,
	RTK_RG_ALG_H323_UDP_PORT=1719,
	RTK_RG_ALG_RTSP_UDP_PORT=554,
	RTK_RG_ALG_PPTP_PASSTHROUGH_UDP_PORT=1723,
	RTK_RG_ALG_L2TP_PASSTHROUGH_UDP_PORT=1701,
	RTK_RG_ALG_IPSEC_PASSTHROUGH_UDP_PORT=1293,
	RTK_RG_ALG_FTP_UDP_PORT=21,
	RTK_RG_ALG_FTP_DATA_UDP_PORT=20,
}rtk_rg_alg_udpKnownPort_t;

typedef int (*p_algRegisterFunction)(int,int,unsigned char*,unsigned char*);	//the third is sk_buff* ,the fourth is type rtk_rg_pktHdr_t*

typedef struct rtk_rg_alg_funcMapping_s
{
	unsigned short int portNum;
	p_algRegisterFunction registerFunction;
}rtk_rg_alg_funcMapping_t;

typedef struct rtk_rg_alg_serverIpMapping_s
{
	rtk_rg_alg_type_t algType;
	ipaddr_t serverAddress;
}rtk_rg_alg_serverIpMapping_t;

//ACL
typedef enum rtk_rg_acl_action_type_e
{
	ACL_ACTION_TYPE_DROP=0,
	ACL_ACTION_TYPE_PERMIT,
	ACL_ACTION_TYPE_TRAP,
	ACL_ACTION_TYPE_QOS,
	ACL_ACTION_TYPE_TRAP_TO_PS,
	ACL_ACTION_TYPE_END
} rtk_rg_acl_action_type_t;

typedef enum rtk_rg_acl_filter_and_qos_action_e
{
	ACL_ACTION_NOP_BIT=(1<<0),
	ACL_ACTION_1P_REMARKING_BIT=(1<<1),
	ACL_ACTION_IP_PRECEDENCE_REMARKING_BIT=(1<<2),
	ACL_ACTION_DSCP_REMARKING_BIT=(1<<3),
	ACL_ACTION_QUEUE_ID_BIT=(1<<4),
	ACL_ACTION_SHARE_METER_BIT=(1<<5),	
	ACL_ACTION_STREAM_ID_OR_LLID_BIT=(1<<6),	
	ACL_ACTION_END=(1<<7),
} rtk_rg_acl_qos_action_t;


typedef enum rtk_rg_acl_filter_fields_e
{
	INGRESS_PORT_BIT=0x1,
	INGRESS_INTF_BIT=0x2,
	EGRESS_INTF_BIT=0x4,		//not support in 0371 testchip
	INGRESS_ETHERTYPE_BIT=0x8,
	INGRESS_CTAG_PRI_BIT=0x10,
	INGRESS_CTAG_VID_BIT=0x20,
	INGRESS_SMAC_BIT=0x40,
	INGRESS_DMAC_BIT=0x80,
	INGRESS_DSCP_BIT=0x100,
	INGRESS_L4_TCP_BIT=0x200,	
	INGRESS_L4_UDP_BIT=0x400,	
	INGRESS_IPV6_SIP_RANGE_BIT=0x800,
	INGRESS_IPV6_DIP_RANGE_BIT=0x1000,
	INGRESS_IPV4_SIP_RANGE_BIT=0x2000,	
	INGRESS_IPV4_DIP_RANGE_BIT=0x4000,
	INGRESS_L4_SPORT_RANGE_BIT=0x8000,
	INGRESS_L4_DPORT_RANGE_BIT=0x10000,
	EGRESS_IPV4_SIP_RANGE_BIT=0x20000,	//not support in 0371 testchip
	EGRESS_IPV4_DIP_RANGE_BIT=0x40000,	//not support in 0371 testchip
	EGRESS_L4_SPORT_RANGE_BIT=0x80000,	//not support in 0371 testchip
	EGRESS_L4_DPORT_RANGE_BIT=0x100000,	//not support in 0371 testchip
	INGRESS_L4_ICMP_BIT=0x200000,	//not support in 0371 testchip
} rtk_rg_acl_filter_fields_t;

typedef struct rtk_rg_aclFilterAndQos_s
{
	unsigned int filter_fields;
	rtk_rg_portmask_t ingress_port_mask;
	int ingress_dscp;
	int ingress_intf_idx;
	int egress_intf_idx;
	int ingress_ethertype;
	int ingress_ctag_vid;
	int ingress_ctag_pri;
	rtk_mac_t ingress_smac;
	rtk_mac_t ingress_dmac;
	ipaddr_t ingress_src_ipv4_addr_start;
	ipaddr_t ingress_src_ipv4_addr_end;
	ipaddr_t ingress_dest_ipv4_addr_start;
	ipaddr_t ingress_dest_ipv4_addr_end;
	uint8 ingress_src_ipv6_addr_start[16];
	uint8 ingress_src_ipv6_addr_end[16];
	uint8 ingress_dest_ipv6_addr_start[16];
	uint8 ingress_dest_ipv6_addr_end[16];
	unsigned short int ingress_src_l4_port_start;
	unsigned short int ingress_src_l4_port_end;
	unsigned short int ingress_dest_l4_port_start;
	unsigned short int ingress_dest_l4_port_end;
	ipaddr_t egress_src_ipv4_addr_start;
	ipaddr_t egress_src_ipv4_addr_end;
	ipaddr_t egress_dest_ipv4_addr_start;
	ipaddr_t egress_dest_ipv4_addr_end;
	unsigned short int egress_src_l4_port_start;
	unsigned short int egress_src_l4_port_end;
	unsigned short int egress_dest_l4_port_start;
	unsigned short int egress_dest_l4_port_end;

	rtk_rg_acl_action_type_t action_type;
	rtk_rg_acl_qos_action_t qos_actions; /* only used for action_type=ACL_ACTION_TYPE_QOS */

	unsigned char action_dot1p_remarking_pri;
	unsigned char action_ip_precedence_remarking_pri;
	unsigned char action_dscp_remarking_pri;
	unsigned char action_queue_id;
	unsigned char action_share_meter;
	unsigned char action_stream_id_or_llid;
} rtk_rg_aclFilterAndQos_t;

//URL Filter
typedef struct rtk_rg_urlFilterString_s
{
	unsigned char url_filter_string[128];
	unsigned char path_filter_string[256];
	int path_exactly_match;
	int wan_intf;
} rtk_rg_urlFilterString_t;

//UPnP
typedef enum rtk_rg_upnp_type_e
{
	UPNP_TYPE_ONESHOT=0,
	UPNP_TYPE_PERSIST=1
} rtk_rg_upnp_type_t;

typedef struct rtk_rg_upnpConnection_s
{
	int is_tcp;
	int valid;
	int wan_intf_idx; //for gateway ip
	int gateway_port;
	ipaddr_t local_ip;
	unsigned short int local_port;
	int limit_remote_ip;
	int limit_remote_port;
	ipaddr_t remote_ip;
	unsigned short int remote_port;
	rtk_rg_upnp_type_t type; //one shot?
	int timeout; //auto-delete after timeout, 0:disable auto-delete
} rtk_rg_upnpConnection_t;

/* FTP */
//entry for each FTP session
typedef struct rtk_rg_ftpCtrlFlowEntry_s {
	unsigned int remoteIpAddr;		//remote host ip
	unsigned int internalIpAddr;			//NPI internal ip

	unsigned short int remotePort;	//remote port
	unsigned short int internalPort;		//internal port

	//if outbound enlarge packet, Delta is positive value;
	//if outbound shrink packet, Delta is negative value
	int Delta;								//used to sync acknowledgement
		
	struct rtk_rg_ftpCtrlFlowEntry_s *pNext,*pPrev;
}rtk_rg_ftpCtrlFlowEntry_t;


/* PPTP */
//PPTP message type
typedef enum rtk_rg_pptpCtrlMsgType_e
{
	PPTP_StartCtrlConnRequest 	= 1,
	PPTP_StartCtrlConnReply 	= 2,
	PPTP_StopCtrlConnRequest 	= 3,
	PPTP_StopCtrlConnReply 		= 4,
	PPTP_EchoRequest 			= 5,
	PPTP_EchoReply 				= 6,
	PPTP_OutCallRequest 		= 7,
	PPTP_OutCallReply 			= 8,
	PPTP_InCallRequest 			= 9,
	PPTP_InCallReply 			= 10,
	PPTP_InCallConn 			= 11,
	PPTP_CallClearRequest 		= 12,
	PPTP_CallDiscNotify 		= 13,
	PPTP_WanErrorNotify 		= 14,
	PPTP_SetLinkInfo 			= 15
}rtk_rg_pptpCtrlMsgType_t;

//Message structures 
typedef struct rtk_rg_pptpMsgHead_s
{
	unsigned short int    length;			/* total length */
	unsigned short int    msgType;			/* PPTP message type */
	unsigned int      	  magic;			/* magic cookie */
	unsigned short int    type;				/* control message type */
	unsigned short int    resv0;			/* reserved */
}rtk_rg_pptpMsgHead_t;

typedef struct rtk_rg_pptpCallIds_s
{
	unsigned short int    cid1;				/* Call ID field #1 */
	unsigned short int    cid2;				/* Call ID field #2 */
}rtk_rg_pptpCallIds_t;

typedef struct rtk_rg_pptpCodes_s
{
	unsigned char     resCode;				/* Result Code */
	unsigned char     errCode;				/* Error Code */
}rtk_rg_pptpCodes_t;

//GRE entry for each PPTP session
typedef struct rtk_rg_pptpGreEntry_s 
{
	unsigned int remoteIpAddr;
	rtk_mac_t remoteMacAddr;
	unsigned int internalIpAddr;
	rtk_mac_t internalMacAddr;

	unsigned short int remoteCallID;
	unsigned short int externalCallID;
	unsigned short int internalCallID;

	unsigned char valid;
}rtk_rg_pptpGreEntry_t;

typedef struct rtk_rg_pptpGreLinkList_s 
{
	rtk_rg_pptpGreEntry_t greEntry;
	struct rtk_rg_pptpGreLinkList_s *pPrev, *pNext;
}rtk_rg_pptpGreLinkList_t;


/* Init */
typedef int (*p_initByHwCallBack)(void);
typedef int (*p_arpAddByHwCallBack)(rtk_rg_arpInfo_t*);
typedef int (*p_arpDelByHwCallBack)(rtk_rg_arpInfo_t*);
typedef int (*p_macAddByHwCallBack)(rtk_rg_macEntry_t*);
typedef int (*p_macDelByHwCallBack)(rtk_rg_macEntry_t*);
typedef int (*p_routingAddByHwCallBack)(rtk_rg_ipv4RoutingEntry_t*);
typedef int (*p_routingDelByHwCallBack)(rtk_rg_ipv4RoutingEntry_t*);
typedef int (*p_naptAddByHwCallBack)(rtk_rg_naptInfo_t*);
typedef int (*p_naptDelByHwCallBack)(rtk_rg_naptInfo_t*);
typedef int (*P_bindAddByHwCallBack)(rtk_rg_bindingEntry_t*);
typedef int (*P_bindDelByHwCallBack)(rtk_rg_bindingEntry_t*);
typedef int (*p_interfaceAddByHwCallBack)(rtk_rg_intfInfo_t*,int*);
typedef int (*p_interfaceDelByHwCallBack)(rtk_rg_intfInfo_t*,int*);
typedef int (*p_neighborAddByHwCallBack)(rtk_rg_neighborInfo_t*);
typedef int (*p_neighborDelByHwCallBack)(rtk_rg_neighborInfo_t*);
typedef int (*p_v6RoutingAddByHwCallBack)(rtk_rg_ipv6RoutingEntry_t*);
typedef int (*p_v6RoutingDelByHwCallBack)(rtk_rg_ipv6RoutingEntry_t*);
typedef int (*p_pppoeBeforeDiagByHwCallBack)(rtk_rg_pppoeClientInfoBeforeDial_t*,int*);
typedef int (*p_dhcpRequestByHwCallBack)(int*);
typedef int (*p_naptInboundConnLookupFirstCallBack)(void*,ipaddr_t*,uint16*);
typedef int (*p_naptInboundConnLookupSecondCallBack)(void*,ipaddr_t*,uint16*);
typedef int (*p_naptInboundConnLookupThirdCallBack)(void*,ipaddr_t*,uint16*);

typedef struct rtk_rg_initParams_s
{
	uint32 igmpSnoopingEnable:1;
	uint32 macBasedTagDecision:1;		//control DMAC2CVID per-port and forced state registers
	
	p_initByHwCallBack initByHwCallBack;
	p_arpAddByHwCallBack arpAddByHwCallBack;
	p_arpDelByHwCallBack arpDelByHwCallBack;
	p_macAddByHwCallBack macAddByHwCallBack;
	p_macDelByHwCallBack macDelByHwCallBack;
	p_routingAddByHwCallBack routingAddByHwCallBack;
	p_routingDelByHwCallBack routingDelByHwCallBack;
	p_naptAddByHwCallBack naptAddByHwCallBack;
	p_naptDelByHwCallBack naptDelByHwCallBack;
	P_bindAddByHwCallBack bindingAddByHwCallBack;
	P_bindDelByHwCallBack bindingDelByHwCallBack;
	p_interfaceAddByHwCallBack interfaceAddByHwCallBack;
	p_interfaceDelByHwCallBack interfaceDelByHwCallBack;
	p_neighborAddByHwCallBack neighborAddByHwCallBack;
	p_neighborDelByHwCallBack neighborDelByHwCallBack;
	p_v6RoutingAddByHwCallBack v6RoutingAddByHwCallBack;
	p_v6RoutingDelByHwCallBack v6RoutingDelByHwCallBack;
	p_pppoeBeforeDiagByHwCallBack pppoeBeforeDiagByHwCallBack;
	p_naptInboundConnLookupFirstCallBack naptInboundConnLookupFirstCallBack;
	p_naptInboundConnLookupSecondCallBack naptInboundConnLookupSecondCallBack;
	p_naptInboundConnLookupThirdCallBack naptInboundConnLookupThirdCallBack;
	p_dhcpRequestByHwCallBack dhcpRequestByHwCallBack;
} rtk_rg_initParams_t;


/* internal */
#if 0
typedef enum rtk_rg_aclField_Teamplate_s
{
	//related to Template[0]
	TEMPLATE_DMAC0 	=0,
	TEMPLATE_DMAC1 	=1,
	TEMPLATE_DMAC2 	=2,
	TEMPLATE_EXTPORTMASK=3, 
	TEMPLATE_SMAC0 	=4,
	TEMPLATE_SMAC1 	=5,
	TEMPLATE_SMAC2 	=6,
	TEMPLATE_ETHERTYPE =7,
	//related to Template[1]
	TEMPLATE_CTAG 		=8,
	TEMPLATE_IPv4SIP0 	=9,
	TEMPLATE_IPv4SIP1	=10,
	TEMPLATE_DSCP		=11, 
	TEMPLATE_IPRANGE 	=12,
	TEMPLATE_PORTRANGE 	=13,
	TEMPLATE_IPv4DIP0 	=14,
	TEMPLATE_IPv4DIP1 	=15,	
	//related to Template[2]
	TEMPLATE_FIELDSELECT00 	=16, //l4_dst_port
	TEMPLATE_FIELDSELECT01 	=17, //l4_src_port
	TEMPLATE_FIELDSELECT02	=18, 
	TEMPLATE_FIELDSELECT03 	=19, //SIPv6[128:112]
	TEMPLATE_FIELDSELECT04 	=20, //DSCP
	TEMPLATE_FIELDSELECT05 	=21, 
	TEMPLATE_FIELDSELECT06 	=22,
	TEMPLATE_FIELDSELECT07 	=23,
	//related to Template[3]
	TEMPLATE_FIELDSELECT08 	=24, //DIPv6[127:112]
	TEMPLATE_FIELDSELECT09 	=25, //DIPv6[111:96]
	TEMPLATE_FIELDSELECT10	=26, //DIPv6[95:80]
	TEMPLATE_FIELDSELECT11 	=27, //DIPv6[79:64]
	TEMPLATE_FIELDSELECT12 	=28, //DIPv6[63:48]
	TEMPLATE_FIELDSELECT13 	=29, //DIPv6[47:32]
	TEMPLATE_FIELDSELECT14	=30, //IP protocal
	TEMPLATE_FIELDSELECT15 	=31, //SessionID
	TEMPLATE_END
}rtk_rg_aclField_Teamplate_t;
#else
typedef enum rtk_rg_aclField_Teamplate_s
{
	//related to Template[0]
	TEMPLATE_DMAC0 			=0,
	TEMPLATE_DMAC1 			=1,
	TEMPLATE_DMAC2 			=2,
	TEMPLATE_EXTPORTMASK	=3, 
	TEMPLATE_SMAC0 			=4,
	TEMPLATE_SMAC1 			=5,
	TEMPLATE_SMAC2 			=6,
	TEMPLATE_ETHERTYPE 		=7,
	//related to Template[1]
	TEMPLATE_FIELDSELECT00	=8,//l4_dst_port
	TEMPLATE_IPv4SIP0 		=9,
	TEMPLATE_IPv4SIP1		=10,
	TEMPLATE_FIELDSELECT01	=11, //l4_src_port
	TEMPLATE_FIELDSELECT14	=12,//IP protocal
	TEMPLATE_PORTRANGE 		=13,
	TEMPLATE_IPv4DIP0 		=14,
	TEMPLATE_IPv4DIP1 		=15,	
	//related to Template[2]
	TEMPLATE_CTAG	 		=16, 
	TEMPLATE_IPRANGE 		=17, 
	TEMPLATE_FIELDSELECT02	=18, //Not used pattern in ACL
	TEMPLATE_FIELDSELECT03 	=19, //SIPv6[128:112]
	TEMPLATE_FIELDSELECT04 	=20, //DSCP
	TEMPLATE_IPv6SIP1 		=21, //SIPv6[31:16]
	TEMPLATE_IPv6SIP0 		=22, //SIPv6[15:0]
	TEMPLATE_FIELDSELECT15 	=23, //SessionID
	//related to Template[3]
	TEMPLATE_FIELDSELECT08 	=24, //DIPv6[127:112]
	TEMPLATE_FIELDSELECT09 	=25, //DIPv6[111:96]
	TEMPLATE_FIELDSELECT10	=26, //DIPv6[95:80]
	TEMPLATE_FIELDSELECT11 	=27, //DIPv6[79:64]
	TEMPLATE_FIELDSELECT12 	=28, //DIPv6[63:48]
	TEMPLATE_FIELDSELECT13 	=29, //DIPv6[47:32]
	TEMPLATE_IPv6DIP1		=30, //DIPv6[31:16]
	TEMPLATE_IPv6DIP0 		=31, //DIPv6[15:0]
	TEMPLATE_END
}rtk_rg_aclField_Teamplate_t;
#endif

typedef enum rtk_rg_flow_direction_s
{
	FLOW_DIRECTION_CAN_NOT_DECIDE=0,
	FLOW_DIRECTION_UPSTREAM, 	//LAN_TO_WAN
	FLOW_DIRECTION_DOWNSTREAM, //WAN_TO_LAN
	FLOW_DIRECTION_LAN_TO_LAN,
	FLOW_DIRECTION_WAN_TO_WAN,
}rtk_rg_flow_direction_t;


typedef enum rtk_rg_aclSWEntry_used_tables_field_s
{
	ACL_USED_IPTABLE_IPV4SIP = 0x1,
	ACL_USED_IPTABLE_IPV4DIP = 0x2,
	ACL_USED_IPTABLE_IPV6SIP = 0x4,
	ACL_USED_IPTABLE_IPV6DIP = 0x8,
	ACL_USED_PORTTABLE_SPORT = 0x10,
	ACL_USED_PORTTABLE_DPORT = 0x20,
	CF_USED_IPTABLE_IPV4SIP = 0x40,
	CF_USED_IPTABLE_IPV4DIP = 0x80,
	CF_USED_PORTTABLE_SPORT = 0x100,
	CF_USED_PORTTABLE_DPORT = 0x200,
	CF_USED_DSCPTABLE = 0x400,
} rtk_rg_aclSWEntry_used_tables_field_t;

typedef enum rtk_rg_aclSWEntry_used_tables_index_s
{
	ACL_USED_IPTABLE_IPV4SIP_INDEX = 0,
	ACL_USED_IPTABLE_IPV4DIP_INDEX ,
	ACL_USED_IPTABLE_IPV6SIP_INDEX ,
	ACL_USED_IPTABLE_IPV6DIP_INDEX ,
	ACL_USED_PORTTABLE_SPORT_INDEX ,
	ACL_USED_PORTTABLE_DPORT_INDEX ,
	CF_USED_IPTABLE_IPV4SIP_INDEX ,
	CF_USED_IPTABLE_IPV4DIP_INDEX ,
	CF_USED_PORTTABLE_SPORT_INDEX ,
	CF_USED_PORTTABLE_DPORT_INDEX ,
	CF_USED_DSCPTABLE_INDEX,
	USED_TABLE_END
} rtk_rg_aclSWEntry_used_tables_index_t;


typedef enum rtk_rg_aclSWEntry_type_s
{
	UNUSE = 0,
	ACL_USE,
	QOS_USE,
	OTHERS_USE,
} rtk_rg_aclSWEntry_type_t;




typedef struct rtk_rg_aclFilterEntry_s
{
	uint32 hw_aclEntry_start;
	uint32 hw_aclEntry_size; 
	uint32 hw_cfEntry_start;
	uint32 hw_cfEntry_size;
	rtk_rg_aclSWEntry_used_tables_field_t hw_used_table;//record which range tables are used
	uint8 hw_used_table_index[USED_TABLE_END];//record  used range tables index
	rtk_rg_aclSWEntry_type_t type;
	rtk_rg_aclFilterAndQos_t acl_filter;
}rtk_rg_aclFilterEntry_t;


/*URL Module*/
typedef struct rtk_rg_urlFilterEntry_s
{
	int valid;
	rtk_rg_urlFilterString_t urlFilter;
}rtk_rg_urlFilterEntry_t;

#define MAX_URL_FILTER_ENTRY_SIZE 30

/*LUT Module*/
typedef enum rtk_rg_macfilter_interface_e
{
	INTERFACE_FOR_LAN=0,
	INTERFACE_FOR_WAN,
	INTERFACE_END
}rtk_rg_macfilter_interface_t;

typedef enum rtk_rg_macFilterEntry_direction_s
{
	RTK_RG_MACFILTER_FILTER_SRC_DEST_MAC_BOTH,
	RTK_RG_MACFILTER_FILTER_SRC_MAC_ONLY,
	RTK_RG_MACFILTER_FILTER_DEST_MAC_ONLY,
	RTK_RG_MACFILTER_FILTER_END
} rtk_rg_macFilterEntry_direction_t;

typedef struct rtk_rg_macFilterEntry_s
{
	rtk_mac_t mac;
	rtk_rg_macFilterEntry_direction_t direct;
}rtk_rg_macFilterEntry_t;

typedef struct rtk_rg_macFilterSWEntry_s
{
	int valid;
	int l2_table_entry_index;
	int l2_table_entry_index_for_lan;
	int l2_table_entry_index_for_wan;
	rtk_rg_macFilterEntry_t macFilterEntry;
}rtk_rg_macFilterSWEntry_t;

#define MAX_MAC_FILTER_ENTRY_SIZE 30

/* MIB counter */
typedef struct rtk_rg_port_mib_info_s
{
    uint64 ifInOctets;
    uint32 ifInUcastPkts;
    uint32 ifInMulticastPkts;
    uint32 ifInBroadcastPkts;
    uint32 ifInDiscards;
    uint64 ifOutOctets;
    uint32 ifOutDiscards;
    uint32 ifOutUcastPkts;
    uint32 ifOutMulticastPkts;
    uint32 ifOutBrocastPkts;
    uint32 dot1dBasePortDelayExceededDiscards;
    uint32 dot1dTpPortInDiscards;
    uint32 dot1dTpHcPortInDiscards;
    uint32 dot3InPauseFrames;
    uint32 dot3OutPauseFrames;
    //uint32 dot3OutPauseOnFrames;
    uint32 dot3StatsAligmentErrors;
    uint32 dot3StatsFCSErrors;
    uint32 dot3StatsSingleCollisionFrames;
    uint32 dot3StatsMultipleCollisionFrames;
    uint32 dot3StatsDeferredTransmissions;
    uint32 dot3StatsLateCollisions;
    uint32 dot3StatsExcessiveCollisions;
    uint32 dot3StatsFrameTooLongs;
    uint32 dot3StatsSymbolErrors;
    uint32 dot3ControlInUnknownOpcodes;
    uint32 etherStatsDropEvents;
    uint64 etherStatsOctets;
    uint32 etherStatsBcastPkts;
    uint32 etherStatsMcastPkts;
    uint32 etherStatsUndersizePkts;
    uint32 etherStatsOversizePkts;
    uint32 etherStatsFragments;
    uint32 etherStatsJabbers;
    uint32 etherStatsCollisions;
    uint32 etherStatsCRCAlignErrors;
    uint32 etherStatsPkts64Octets;
    uint32 etherStatsPkts65to127Octets;
    uint32 etherStatsPkts128to255Octets;
    uint32 etherStatsPkts256to511Octets;
    uint32 etherStatsPkts512to1023Octets;
    uint32 etherStatsPkts1024to1518Octets;
    uint64 etherStatsTxOctets;
    uint32 etherStatsTxUndersizePkts;
    uint32 etherStatsTxOversizePkts;
    uint32 etherStatsTxPkts64Octets;
    uint32 etherStatsTxPkts65to127Octets;
    uint32 etherStatsTxPkts128to255Octets;
    uint32 etherStatsTxPkts256to511Octets;
    uint32 etherStatsTxPkts512to1023Octets;
    uint32 etherStatsTxPkts1024to1518Octets;
    uint32 etherStatsTxPkts1519toMaxOctets;
    uint32 etherStatsTxBcastPkts;
    uint32 etherStatsTxMcastPkts;
    uint32 etherStatsTxFragments;
    uint32 etherStatsTxJabbers;
    uint32 etherStatsTxCRCAlignErrors;
    uint32 etherStatsRxUndersizePkts;
    uint32 etherStatsRxUndersizeDropPkts;
    uint32 etherStatsRxOversizePkts;
    uint32 etherStatsRxPkts64Octets;
    uint32 etherStatsRxPkts65to127Octets;
    uint32 etherStatsRxPkts128to255Octets;
    uint32 etherStatsRxPkts256to511Octets;
    uint32 etherStatsRxPkts512to1023Octets;
    uint32 etherStatsRxPkts1024to1518Octets;
    uint32 etherStatsRxPkts1519toMaxOctets;
    uint32 inOampduPkts;
    uint32 outOampduPkts;
}rtk_rg_port_mib_info_t;

/* Priority to queue mapping */
typedef struct rtk_rg_qos_pri2queue_s
{
	 uint32 pri2queue[RTK_MAX_NUM_OF_PRIORITY];
}rtk_rg_qos_pri2queue_t;

/* Weight of each priority source */
typedef struct rtk_rg_qos_priSelWeight_s
{
    uint32 weight_of_portBased;
    uint32 weight_of_dot1q;
    uint32 weight_of_dscp;
    uint32 weight_of_acl;
    uint32 weight_of_lutFwd;
    uint32 weight_of_saBaed;
    uint32 weight_of_vlanBased;
    uint32 weight_of_svlanBased;
    uint32 weight_of_l4Based;
}rtk_rg_qos_priSelWeight_t;



/* DOS Port Security*/
typedef enum rtk_rg_dos_type_e
{
    RTK_RG_DOS_DAEQSA_DENY = 0,
    RTK_RG_DOS_LAND_DENY,
    RTK_RG_DOS_BLAT_DENY,
    RTK_RG_DOS_SYNFIN_DENY,
    RTK_RG_DOS_XMA_DENY,
    RTK_RG_DOS_NULLSCAN_DENY,
    RTK_RG_DOS_SYN_SPORTL1024_DENY,
    RTK_RG_DOS_TCPHDR_MIN_CHECK,
    RTK_RG_DOS_TCP_FRAG_OFF_MIN_CHECK,
    RTK_RG_DOS_ICMP_FRAG_PKTS_DENY,
    RTK_RG_DOS_POD_DENY,
    RTK_RG_DOS_UDPDOMB_DENY,
    RTK_RG_DOS_SYNWITHDATA_DENY,
    RTK_RG_DOS_SYNFLOOD_DENY,
    RTK_RG_DOS_FINFLOOD_DENY,
    RTK_RG_DOS_ICMPFLOOD_DENY,
    RTK_RG_DOS_TYPE_MAX
}rtk_rg_dos_type_t;

typedef enum rtk_rg_dos_action_e
{
    RTK_RG_DOS_ACTION_DROP = 0,
    RTK_RG_DOS_ACTION_TRAP,
}rtk_rg_dos_action_t;

/* End of RTK RG API ====================================================== */

/* NIC ==================================================================== */
typedef struct rtk_rg_rxdesc_s{
	union{
		struct{
			uint32 own:1;//31
			uint32 eor:1;//30
			uint32 fs:1;//29
			uint32 ls:1;//28
			uint32 crcerr:1;//27
			uint32 ipv4csf:1;//26
			uint32 l4csf:1;//25
			uint32 rcdf:1;//24
			uint32 ipfrag:1;//23
			uint32 pppoetag:1;//22
			uint32 rwt:1;//21
			uint32 pkttype:4;//17~20
			uint32 l3routing:1;//16
			uint32 origformat:1;//15
			uint32 pctrl:1;//14
			uint32 rsvd:2;//12~13
			uint32 data_length:12;//0~11
		}bit;
		uint32 dw;//double word
	}opts1;
	uint32 addr;
	union{
		struct{
			uint32 cputag:1;//31
			uint32 ptp_in_cpu_tag_exist:1;//30
			uint32 svlan_tag_exist:1;//29
			uint32 rsvd_2:2;//27~28
			uint32 pon_stream_id:7;//20~26
			uint32 rsvd_1:3;//17~19
			uint32 ctagva:1;//16
			uint32 cvlan_tag:16;//0~15
		}bit;
		uint32 dw;//double word
	}opts2;
	union{
		struct{
			uint32 src_port_num:5;//27~31
			uint32 dst_port_mask:6;//21~26
			uint32 reason:8;//13~20
			uint32 internal_priority:3;//10~12
			uint32 ext_port_ttl_1:5;//5~9
			uint32 rsvd:5;//0~4
		}bit;
		uint32 dw;//double word
	}opts3;
}rtk_rg_rxdesc_t;

typedef struct rtk_rg_txdesc_s{
	union{
		struct{
			uint32 own:1;//31
			uint32 eor:1;//30
			uint32 fs:1;//29
			uint32 ls:1;//28
			uint32 ipcs:1;//27
			uint32 l4cs:1;//26
			uint32 keep:1;//25
			uint32 blu:1;//24
			uint32 crc:1;//23
			uint32 vsel:1;//22
			uint32 dislrn:1;//21
			uint32 cputag_ipcs:1;//20
			uint32 cputag_l4cs:1;//19
			uint32 cputag_psel:1;//18
			uint32 rsvd:1;//17
			uint32 data_length:17;//0~16
		}bit;
		uint32 dw;//double word
	}opts1;
	uint32 addr;
	union{
		struct{
			uint32 cputag:1;//31
			uint32 aspri:1;//30
			uint32 cputag_pri:3;//27~29
			uint32 tx_vlan_action:2;//25~26
			uint32 tx_pppoe_action:2;//23~24
			uint32 tx_pppoe_idx:3;//20~22
			uint32 efid:1;//19
			uint32 enhance_fid:3;//16~18
			uint32 vidl:8;//8~15
			uint32 prio:3;//5~7
			uint32 cfi:1;// 4
			uint32 vidh:4;//0~3
		}bit;
		uint32 dw;//double word
	}opts2;
	union{
		struct{
			uint32 extspa:3;//29~31
			uint32 tx_portmask:6;//23~28
			uint32 tx_dst_stream_id:7;//16~22
			uint32 reserved:14; // 2~15
			uint32 l34_keep:1;// 1
			uint32 PTP:1;//0
		}bit;
		uint32 dw;//double word
	}opts3;
	union{
		uint32 dw;
	}opts4;
}rtk_rg_txdesc_t;
/* End of NIC ============================================================= */


/* Forward Engine ========================================================= */
typedef enum rtk_rg_fwdEngineReturn_e
{
	RG_FWDENGINE_RET_TO_PS				=0x0,
	RG_FWDENGINE_RET_DIRECT_TX			=0x1,
	RG_FWDENGINE_RET_DROP				=0x2,
	RG_FWDENGINE_RET_UN_INIT			=0x3,
	RG_FWDENGINE_RET_QUEUE_FRAG			=0x4,
	RG_FWDENGINE_RET_FRAGMENT_ONE		=0x5,
	RG_FWDENGINE_RET_FRAGMENT			=0x6,
	RG_FWDENGINE_RET_NAPT_OK			=0x7,
	RG_FWDENGINE_RET_L2FORWARDED		=0x8,
	RG_FWDENGINE_RET_CONTINUE			=0x9,
	RG_FWDENGINE_RET_BROADCAST			=0xa,
	RG_FWDENGINE_RET_FRAG_ONE_PS		=0xb,
	RG_FWDENGINE_RET_FRAG_ONE_DROP		=0xc,
	RG_FWDENGINE_RET_HWLOOKUP			=0xd,
}rtk_rg_fwdEngineReturn_t;


#if 0
/* Protocol */
struct	rg_proto_ether_header {
	unsigned char	ether_dhost[6];
	unsigned char	ether_shost[6];
	unsigned short int	ether_type;
};

struct rg_proto_iphdr {
	unsigned char	ihl_protocol;
	unsigned char	tos;
	unsigned short int	tot_len;
	unsigned short int	id;
	unsigned short int	frag_off;
	unsigned char	ttl;
	unsigned char	protocol;
	unsigned short int	check;
	unsigned int	saddr;
	unsigned int	daddr;
};

struct rg_proto_tcphdr {
	unsigned short int	source;
	unsigned short int	dest;
	unsigned int	seq;
	unsigned int	ack_seq;
	unsigned char	doff;
	unsigned char	flag;
	unsigned short int	window;
	unsigned short int	check;
	unsigned short int	urg_ptr;
};

struct rg_proto_udphdr {
	unsigned short int	source;
	unsigned short int	dest;
	unsigned short int	len;
	unsigned short int	check;
};
#endif

/* End of Forward Engine ================================================== */



/* Parser ================================================================= */
typedef enum rtk_rg_pkthdr_tagif_e
{
	SVLAN_TAGIF=(1<<0),
	CVLAN_TAGIF=(1<<1),
	PPPOE_TAGIF=(1<<2),
	IPV4_TAGIF=(1<<3),
	IPV6_TAGIF=(1<<4),
	TCP_TAGIF=(1<<5),
	UDP_TAGIF=(1<<6),
	IGMP_TAGIF=(1<<7),
	PPTP_TAGIF=(1<<8),
	GRE_TAGIF=(1<<9),
	ICMPV6_TAGIF=(1<<10),
	ICMP_TAGIF=(1<<11),
} rtk_rg_pkthdr_tagif_t;


typedef struct rtk_rg_tcpFlags_s{

#ifdef _LITTLE_ENDIAN

	uint8	fin:1;
	uint8	syn:1;
	uint8	reset:1;
	uint8	push:1;
	uint8	ack:1;
	uint8	urg:1;
	uint8	reserved:2;
	
#else
	uint8	reserved:2;
	uint8	urg:1;
	uint8	ack:1;
	uint8	push:1;
	uint8	reset:1;
	uint8	syn:1;
	uint8	fin:1;

#endif	
}rtk_rg_tcpFlags_t;

typedef struct rtk_rg_aclHitAndAction_s
{
	int aclIgrHit[MAX_ACL_ENTRY_SIZE]; //0:no rule hit 1:ingress pattern hit rule
	int aclEgrHit[MAX_ACL_ENTRY_SIZE]; // 0:no rule hit 1:engress pattern hit rule
	int aclHit; //0:no any rule hit(can be added into short cut)  1:at lease one rule hit(can not add to short cut)

	/*final decided action*/
	rtk_rg_acl_action_type_t action_type;
	rtk_rg_acl_qos_action_t qos_actions; /* only used for action_type=ACL_ACTION_TYPE_QOS */

	unsigned char action_dot1p_remarking_pri;
	unsigned char action_ip_precedence_remarking_pri;
	unsigned char action_dscp_remarking_pri;
	unsigned char action_queue_id;
	unsigned char action_share_meter;
	unsigned char action_stream_id_or_llid;

}rtk_rg_aclHitAndAction_t;

typedef enum rtk_rg_algAction_e
{
	RG_ALG_ACT_NORMAL,				//non-ALG packets
	RG_ALG_ACT_TO_FWDENGINE,		//ALG handled by fwdEngine packets
}rtk_rg_algAction_t;

typedef struct rtk_rg_pktHdr_s
{
	/* NIC RX Desc */
	rtk_rg_rxdesc_t	*pRxDesc;

	/* Tag Info */
	rtk_rg_pkthdr_tagif_t	tagif;
	uint8 *pDmac;
	uint8 *pSmac;

	/* Packet Parsing Info */
	// STAG
	//uint8 *pSVlanTag;
	//uint8	stagPri;
	//uint8	stagDei;
	//uint16	stagVid;	

	// CTAG	
	//uint8 *pCVlanTag;
	uint8	ctagPri;
	uint8	ctagCfi;
	uint16	ctagVid;	
	uint16 etherType;

	// etherType=PPPoE(0x8863/0x8864)
	uint16 sessionId;
	
	// etherType=IPv4(0x0800),ARP(0806)
	uint8 *pTos;
	uint16 l3Offset;
	uint16 l3Len;
	uint16 *pL3Len;
	uint32 ipv4Sip;
	uint32 ipv4Dip;
	uint32 *pIpv4Sip;
	uint32 *pIpv4Dip;
	uint16 *pIpv4Checksum;
	uint8 ipv4FragPacket; //ipv4MoreFragment==1 or ipv4FragmentOffset!=0
	uint8 ipv4MoreFragment;
	uint16 ipv4FragmentOffset;
	uint16 *pIpv4Identification;
	uint8 *pIpv4TTL;

	// etherType=IPv6(0x086dd)
	uint16 ipv6PayloadLen;
	uint8 *pIPv6HopLimit;
	uint8 *pIpv6Sip;
	uint8 *pIpv6Dip;

	//ipv4/ipv6
	uint16 l4Offset;	
	uint8 ipProtocol;

	//ARP
	uint16	arpOpCode;
	
	// ipProtocol=TCP/UDP
	uint16 sport;
	uint16 dport;
	uint16 *pSport;
	uint16 *pDport;
	uint16 *pL4Checksum;	

	//TCP
	rtk_rg_tcpFlags_t tcpFlags;
	uint32	tcpSeq;
	uint32	tcpAck;
	uint32	*pTcpSeq;
	uint32	*pTcpAck;
	uint8 	*pL4Payload;
	uint8	headerLen;

	//HTTP(GET,POST) for URL Filter
	int		httpFirstPacket;

	//ICMPv4
	uint8	ICMPType;
	uint8	ICMPCode;
	uint16	ICMPIdentifier;
	uint8	ICMPOverMTU;		//if the packet is ICMP and L3 length is bigger than interface's MTU

	//ICMPv6
	uint8	ICMPv6Type;
	uint8	ICMPv6Flag;

	/* Routing decision */
	int		sipL3Idx; //decision in _rtk_rg_sip_classification
	int		dipL3Idx; //decision in _rtk_rg_dip_classification	
	int		netifIdx; //wan intf
	int		extipIdx;
	int		dipArpOrNBIdx;
	int		nexthopIdx;
	int		isGatewayPacket;

	int		dmacL2Idx;

	/* Binding decision */
	int 	bindLookUpFinished;
	int		bindNextHopIdx;

	/* PPTP decision*/
	uint16 pptpCtrlType;
	rtk_rg_pptpCallIds_t *pPptpCallId;
	rtk_rg_pptpCodes_t pptpCodes;

	/* VLAN decision*/
	int 	internalVlanID;

	/* Shortcut */
	int matchSC;

	/*ACL*/
	rtk_rg_aclHitAndAction_t aclDecision;

	/* Ingress Port */
	rtk_rg_port_idx_t ingressPort;

	/* ALG */
	rtk_rg_algAction_t algAction;
	int algFunctionMappingIdx;

	/* WLAN */
	int wlan_dev_idx; //0:root, 1:vap0, 2:vap1, 3:vap2, 4:vap3
} rtk_rg_pktHdr_t;
/* End of Parser ========================================================== */

/* ALE ==================================================================== */

typedef enum rtk_rg_ipClassification_e
{
	IP_CLASS_NPI =0,
	IP_CLASS_NI  =1,
	IP_CLASS_LP  =2,
	IP_CLASS_RP  =3,
	IP_CLASS_NPE =4,
	IP_CLASS_NE  =5,
	MAX_SIP_CLASS =4,
	MAX_DIP_CLASS =6
} rtk_rg_ipClassification_t;


typedef enum rtk_rg_sipDipClassification_e
{
	SIP_DIP_CLASS_ROUTING=0,
	SIP_DIP_CLASS_NAT=1,
	SIP_DIP_CLASS_NAPT=2,
	SIP_DIP_CLASS_NATR=3,
	SIP_DIP_CLASS_NAPTR=4,
	SIP_DIP_CLASS_CPU=5,	
	SIP_DIP_CLASS_DROP=6,	
} rtk_rg_sipDipClassification_t;


typedef enum rtk_rg_naptInType_e
{
	NAPT_IN_TYPE_INVALID=0,
	NAPT_IN_TYPE_FULL_CONE=1,
	NAPT_IN_TYPE_PORT_RESTRICTED_CONE=2,
	NAPT_IN_TYPE_RESTRICTED_CONE=3
} rtk_rg_naptInType_t;

/* End of ALE ============================================================== */

/* Tables ================================================================= */
/*typedef struct rtk_rg_table_mac_s
{
	//HW table
	rtk_mac_t macAddr; 
} rtk_rg_table_mac_t;*/

typedef struct rtk_rg_table_vlan_s
{
	//HW table
    rtk_portmask_t MemberPortmask;
    rtk_portmask_t UntagPortmask;
	rtk_portmask_t Ext_portmask;
	rtk_fid_t fid;
    rtk_fidMode_t fidMode;
	rtk_pri_t priority;
	rtk_enable_t priorityEn;

	//SW table specific
	int valid;
	int addedAsCustomerVLAN;		//used to check this vlan created by cvlan apis or not
} rtk_rg_table_vlan_t;

typedef struct rtk_rg_table_lut_s
{
	rtk_l2_addr_table_t rtk_lut;
	int valid;
	//unsigned int idleSecs;
} rtk_rg_table_lut_t;

typedef struct rtk_rg_table_netif_s
{
	rtk_l34_netif_entry_t	rtk_netif;
} rtk_rg_table_netif_t;

typedef struct rtk_rg_table_l3_s
{
	rtk_l34_routing_entry_t	rtk_l3;
	ipaddr_t	netmask;
} rtk_rg_table_l3_t;

typedef struct rtk_rg_table_extip_s
{
	rtk_l34_ext_intip_entry_t	rtk_extip;
} rtk_rg_table_extip_t;

typedef struct rtk_rg_table_nexthop_s
{
	rtk_l34_nexthop_entry_t	rtk_nexthop;
} rtk_rg_table_nexthop_t;

typedef struct rtk_rg_table_pppoe_s
{
	rtk_l34_pppoe_entry_t	rtk_pppoe;
} rtk_rg_table_pppoe_t;

typedef struct rtk_rg_table_arp_s
{
	rtk_l34_arp_entry_t	rtk_arp;
	ipaddr_t ipv4Addr;	
	int staticEntry;
	int idleSecs;
} rtk_rg_table_arp_t;

typedef struct rtk_rg_arp_linkList_s
{
	uint16 idx;		//from MAX_ARP_HW_TABLE_SIZE to (MAX_ARP_SW_TABLE_SIZE - MAX_ARP_HW_TABLE_SIZE)
	int8 routingIdx;
	
#ifdef __KERNEL__
	struct list_head arp_list;
#endif
}rtk_rg_arp_linkList_t;

typedef struct rtk_rg_table_wantype_s
{
	rtk_wanType_entry_t rtk_wantype;
} rtk_rg_table_wantype_t;

typedef struct rtk_rg_table_naptIn_s
{
	rtk_l34_naptInbound_entry_t	rtk_naptIn;
	ipaddr_t remoteIp;
	uint16 remotePort;
	uint16 hashIdx;
	uint32 idleSecs;
	int16 refCount;
} rtk_rg_table_naptIn_t;

typedef struct rtk_rg_table_naptOut_s
{
	rtk_l34_naptOutbound_entry_t	rtk_naptOut;
	rtk_rg_naptState_t state;
	uint16 extPort;
	uint16 hashIdx;
	uint32 idleSecs;	
} rtk_rg_table_naptOut_t;

typedef struct rtk_rg_table_naptOut_linkList_s
{
	struct rtk_rg_table_naptOut_linkList_s *pNext;
	uint16 idx;	
} rtk_rg_table_naptOut_linkList_t;

typedef struct rtk_rg_table_naptIn_linkList_s
{
	struct rtk_rg_table_naptIn_linkList_s *pNext;
	uint16 idx;	
} rtk_rg_table_naptIn_linkList_t;

typedef struct rtk_rg_table_icmp_flow_s
{
	uint8 valid;
	ipaddr_t internalIP;
	ipaddr_t remoteIP;
	uint16 IPID;		//identifier field in IP header
	uint16 inboundIPID;		//identifier field in IP header
	uint8 ICMPType;
	uint8 ICMPCode;
	uint16 ICMPID;		//identifier field in ICMP header (some type without this field)
}rtk_rg_table_icmp_flow_t;

typedef struct rtk_rg_table_icmp_linkList_s
{
	rtk_rg_table_icmp_flow_t icmpFlow;
	struct rtk_rg_table_icmp_linkList_s *pPrev, *pNext;
}rtk_rg_table_icmp_linkList_t;

typedef struct rtk_rg_table_bind_s
{
	rtk_binding_entry_t rtk_bind;
	
	//SW table specific
	int valid;
} rtk_rg_table_bind_t;

typedef struct rtk_rg_table_v6route_s
{
	rtk_ipv6Routing_entry_t rtk_v6route;
} rtk_rg_table_v6route_t;

typedef struct rtk_rg_table_v6neighbor_s
{
	rtk_ipv6Neighbor_entry_t rtk_v6neighbor;
	int staticEntry;
	int idleSecs;
} rtk_rg_table_v6neighbor_t;

/* End of Tables ============================================================== */

/* System ============================================================== */
typedef int (*p_GWMACRequestCallBack)(ipaddr_t, int);		//IP address, Lut table idx

typedef struct rtk_rg_arp_request_s
{
	ipaddr_t reqIp;
	int volatile finished; //used to indicate the ARP request return or not
	p_GWMACRequestCallBack	gwMacReqCallBack;
} rtk_rg_arp_request_t;

typedef int (*p_IPV6GWMACRequestCallBack)(unsigned char*, int);		//IP address, Lut table idx

typedef struct rtk_rg_neighbor_discovery_s
{
	rtk_ipv6_addr_t reqIp;
	int volatile finished; //used to indicate the Neighbor Discovery return or not
	p_IPV6GWMACRequestCallBack	ipv6GwMacReqCallBack;
} rtk_rg_neighbor_discovery_t;


typedef struct rtk_rg_interface_info_global_s
{
	rtk_rg_intfInfo_t storedInfo;
	int valid;
	int lan_or_wan_index;		//index of lan or wan group
	union{
		rtk_rg_lanIntfConf_t *p_lanIntfConf;
		rtk_rg_ipStaticInfo_t *p_wanStaticInfo;
	};
} rtk_rg_interface_info_global_t;

typedef struct rtk_rg_wan_interface_group_info_s
{
	rtk_rg_interface_info_global_t *p_intfInfo;
	int index;
	rtk_rg_wanIntfConf_t *p_wanIntfConf;
} rtk_rg_wan_interface_group_info_t;

typedef struct rtk_rg_lan_interface_group_info_s
{
	rtk_rg_interface_info_global_t *p_intfInfo;
	int index;
} rtk_rg_lan_interface_group_info_t;

typedef struct rtk_rg_virtual_server_info_s
{
	rtk_rg_virtualServer_t* p_virtualServer;
	int index;
} rtk_rg_virtual_server_info_t;

typedef struct rtk_rg_upnp_info_s
{
	rtk_rg_upnpConnection_t* p_upnp;
	int index;
} rtk_rg_upnp_info_t;

typedef struct rtk_rg_port_proto_vid_s
{
	rtk_vlan_protoVlanCfg_t protoVLANCfg[MAX_PORT_PROTO_GROUP_SIZE];
}rtk_rg_port_proto_vid_t;

typedef struct rtk_rg_routing_arpInfo_s
{
	int routingIdx;
	int intfIdx;
	ipaddr_t notMask;		//0000..111, the "NOT" result of network mask
	int bitNum;				//the bits number of "zero" in network mask, /24=8, /25=7,.../30=2 => 31 minus ipMask in routing_entry
	int arpStart;
	int	arpEnd;
	unsigned char isLan;		//for LAN:1, for WAN:0
} rtk_rg_routing_arpInfo_t;

typedef struct rtk_rg_routing_linkList_s		//used when transfer sw ARP to hw ARP
{
	uint8 idx;
	uint8 bitNum;
	
#ifdef __KERNEL__	
	struct list_head route_list;
#endif
}rtk_rg_routing_linkList_t;

typedef enum rtk_rg_sa_learning_exceed_action_e
{
	SA_LEARN_EXCEED_ACTION_PERMIT,
	SA_LEARN_EXCEED_ACTION_DROP,
	SA_LEARN_EXCEED_ACTION_END,
}rtk_rg_sa_learning_exceed_action_t;

typedef struct rtk_rg_glb_system_s
{
	/* System Module */
	rtk_rg_initParams_t initParam;
	unsigned int nxpRefCount[MAX_NEXTHOP_SW_TABLE_SIZE];		//store how many struct reference each nexthop entry
	//int bindToIntf[MAX_BIND_HW_TABLE_SIZE];					//store what interface index this binding connected with
	//int bindWithVLAN[MAX_BIND_HW_TABLE_SIZE];				//store what vlan this binding rule used
	rtk_rg_arp_request_t intfArpRequest[MAX_NETIF_SW_TABLE_SIZE];	//each interface may issue one ARP request
	rtk_rg_neighbor_discovery_t intfNeighborDiscovery[MAX_NETIF_SW_TABLE_SIZE];	//each interface may issue one Neighbor Discovery
	unsigned int vlanInit;
	unsigned int wanIntfTotalNum;
	unsigned int lanIntfTotalNum;
	unsigned int vlanBindTotalNum;			//indicate how many vlan-binding we have
	unsigned int ipv4FragmentQueueNum;		//indicate how many fragment packets in the queue
	unsigned int pppoeBeforeCalled;			//indicate if we had called PPPoE Before
	unsigned int defaultTrapLUTIdx;			//indicate the index of default LUT used to trap to CPU
	unsigned int wanInfoSet;				//indicate which wan has been set info after add
	unsigned int nicIgmpModuleIndex;		//indicate IGMP snooping module's index after registration
	int defaultRouteSet;					//indicate which interface had set default route
	int defaultIPV6RouteSet;				//indicate which interface had set ipv6 default route
	int wanIdxForReset;						//indicate which wan has been reset and need to re-add in the same index
	rtk_rg_interface_info_global_t interfaceInfo[MAX_NETIF_SW_TABLE_SIZE];		//store each interface information, LAN or WAN
	rtk_rg_lan_interface_group_info_t lanIntfGroup[MAX_NETIF_SW_TABLE_SIZE];
	rtk_rg_wan_interface_group_info_t wanIntfGroup[MAX_NETIF_SW_TABLE_SIZE];
	int untagBridgeWanSet[RTK_RG_MAC_PORT_MAX];		//indicate which port had set untag bridge WAN
	//int bridgeWanNum;								//how many bridge WAN interface in system
	int portBasedVID[RTK_RG_PORT_MAX];				//indicate which port-based VLAN ID should be used when untag(contain extension port)
	rtk_rg_port_proto_vid_t protoBasedVID[RTK_RG_PORT_MAX];	//indicate which port-and-protocol-based VLAN ID and GROUP ID would be used when match GROUP setting
	rtk_vlan_protoGroup_t protoGroup[MAX_PORT_PROTO_GROUP_SIZE];	//port and protocol group settings
	int layer2HouseKeepIndex;						//indicate the index last house keep end

	int routingArpInfoNum;	//numer of routing entry in routingArpInfoArray
	int *p_tempRoutingVlanInfoArray;
	int *p_routingVlanInfoArray;
	int routingVlanInfoArray_1[MAX_L3_SW_TABLE_SIZE];
	int routingVlanInfoArray_2[MAX_L3_SW_TABLE_SIZE];
	rtk_rg_routing_arpInfo_t *p_routingArpInfoArray;
	rtk_rg_routing_arpInfo_t *p_tempRoutingArpInfoArray;
	rtk_rg_routing_arpInfo_t routingArpInfoArray_1[MAX_L3_SW_TABLE_SIZE];		//used in ARP rearrangement mechanism
	rtk_rg_routing_arpInfo_t routingArpInfoArray_2[MAX_L3_SW_TABLE_SIZE];		//used in ARP rearrangement mechanism
	unsigned char arpTableCopied[MAX_ARP_HW_TABLE_SIZE];
	rtk_rg_table_arp_t tempArpTable[MAX_ARP_HW_TABLE_SIZE];
	rtk_l34_routing_entry_t tempL3Table[MAX_L3_SW_TABLE_SIZE];	

	//Control Path switches
	int ctrlPathByProtocolStack_broadcast;		//deafult 0, let fwdEngine handle broadcast packets
	int ctrlPathByProtocolStack_ICMP;			//default 0, let fwdEngine handle ICMP packets(except OverMTU packets)

	//SA learning limit number - per port and control action - per port
	int sourceAddrLearningLimitNumber[RTK_RG_PORT_MAX];
	rtk_rg_sa_learning_exceed_action_t sourceAddrLearningAction[RTK_RG_PORT_MAX];
#ifdef __KERNEL__
	atomic_t sourceAddrLearningCount[RTK_RG_PORT_MAX];
#endif

	unsigned int virtualServerTotalNum;
	unsigned int upnpTotalNum;
	rtk_rg_virtual_server_info_t virtualServerGroup[MAX_VIRTUAL_SERVER_SW_TABLE_SIZE];
	rtk_rg_upnp_info_t upnpGroup[MAX_UPNP_SW_TABLE_SIZE];

	int acl_SW_table_entry_size;
	rtk_rg_aclFilterEntry_t* acl_SW_table_entry;
	rtk_rg_aclFilterAndQos_t* acl_filter_temp; //for acl del entry
	rtk_rg_urlFilterEntry_t* urlFilter_table_entry;
	int urlFilter_valid_entry[MAX_URL_FILTER_ENTRY_SIZE];//promote urlFilter compare efficiency
	rtk_rg_macFilterSWEntry_t* macFilter_table_Entry;
	int aclEntry_for_pppoe_passthrought_downstream_perIntf_permit[MAX_NETIF_HW_TABLE_SIZE];
	int aclEntry_for_extPortTranslate[RESERVED_ACL_EXTPORT_TRANSLATE_SIZE];
	int hwnat_enable;
#ifdef RTK_RG_INGRESS_QOS_TESTING
	int qos_type;
	int qos_acl_patch[RTK_RG_MAC_PORT_MAX][5];
	int qos_acl_total_patch;
	int qos_ingress_rate[RTK_RG_MAC_PORT_MAX];
	int qos_ingress_total_rate;
	int qos_queue_mode[RTK_RG_MAC_PORT_MAX];
#endif
	int acl_filter_idx_for_hwnat;		//Default set to 0 now
	int not_disconnect_ppp;		//use for callback delete interface, in case "Server disconnect ppp" will not need to call spppctl.  
#ifdef CONFIG_RG_CALLBACK
	int callback_regist; //for check callback function is registered, then /bin/spppctl & /bin/udhcpc  should not do hwnat again! : Chuck
#endif	
	rtk_rg_stormControlInfo_t*  stormControlInfoEntry;

} rtk_rg_glb_system_t;

typedef struct rtk_rg_fragment_queue_s
{
#ifdef __KERNEL__
	struct sk_buff *queue_skb;	
	struct rx_info queue_rx_info;
	struct re_private queue_cp;
#endif
	rtk_rg_pktHdr_t queue_pktHdr;
	long queue_time;		//jiffies
	int8 occupied;
	rtk_rg_naptDirection_t direction;
} rtk_rg_fragment_queue_t;

typedef struct rtk_rg_ipv4_fragment_out_s
{
	union{
		struct{
			//for TCP, UDP
			int NaptOutboundEntryIndex;
			//rtk_l34_naptOutbound_entry_t *pNaptOutboundEntry;
			ipaddr_t intIp;
			//uint16 identification;
		}napt;
		struct{
			//uint16 identification;
			ipaddr_t intIp;
		}icmp;
	}pktInfo;
	rtk_rg_pkthdr_tagif_t layer4Type;
	rtk_rg_fwdEngineReturn_t fragAction;
	unsigned long beginIdleTime;
	unsigned char pktCount;
	unsigned char queueCount;
	unsigned short identification;		//IP header's identification
	
	struct rtk_rg_ipv4_fragment_out_s *pNext,*pPrev;
} rtk_rg_ipv4_fragment_out_t;

typedef struct rtk_rg_ipv4_fragment_in_s
{
	union{
		struct{
			//for TCP, UDP
			//int NaptInboundEntryIndex;
			int NaptOutboundEntryIndex;
			//rtk_l34_naptInbound_entry_t *pNaptInboundEntry;
			ipaddr_t remoteIp;
		}napt;
		struct{
			ipaddr_t remoteIp;
			ipaddr_t intIp;
		}icmp;
	}pktInfo;
	rtk_rg_pkthdr_tagif_t layer4Type;
	rtk_rg_fwdEngineReturn_t fragAction;
	unsigned long beginIdleTime;
	unsigned char pktCount;
	unsigned char queueCount;
	unsigned short identification;		//IP header's identification
	
	struct rtk_rg_ipv4_fragment_in_s *pNext,*pPrev;
} rtk_rg_ipv4_fragment_in_t;

typedef struct rtk_rg_napt_shortcut_s
{
	//pattern
	uint16	sport;
	uint16	dport;	
	ipaddr_t	sip;
	ipaddr_t	dip;
	int	isTcp;

	//L3 or L4
	int isNapt;

	//action
	int	direction;
	int new_lut_idx;	//for da	
	int new_intf_idx; //for sa
	int new_eip_idx; //for outbound sip
	int naptIdx; //for inbound dip,dport (naptInIdx), for outbound sport
} rtk_rg_napt_shortcut_t;

typedef struct rtk_rg_saLearningLimitInfo_s
{	
	int learningLimitNumber;
	rtk_rg_sa_learning_exceed_action_t action;
}rtk_rg_saLearningLimitInfo_t;

typedef struct rtk_rg_table_ipmcgrp_s
{
	ipaddr_t groupIp;	
	rtk_portmask_t portMsk;
	int valid;
} rtk_rg_table_ipmcgrp_t;

typedef struct rtk_rg_table_wlan_mbssid_s
{
	rtk_mac_t	mac;
	int			wlan_dev_idx;
} rtk_rg_table_wlan_mbssid_t;


/* End of System ============================================================== */


typedef struct rtk_rg_globalDatabase_s
{
	//rtk_rg_table_mac_t 			mac[MAX_LUT_SW_TABLE_SIZE];
	rtk_rg_table_vlan_t			vlan[MAX_VLAN_SW_TABLE_SIZE];
	rtk_rg_table_lut_t			lut[MAX_LUT_SW_TABLE_SIZE];
	rtk_rg_table_netif_t		netif[MAX_NETIF_SW_TABLE_SIZE];
	rtk_rg_table_l3_t			l3[MAX_L3_SW_TABLE_SIZE];
	rtk_rg_table_extip_t		extip[MAX_L3_SW_TABLE_SIZE];
	rtk_rg_table_nexthop_t		nexthop[MAX_L3_SW_TABLE_SIZE];
	rtk_rg_table_pppoe_t		pppoe[MAX_L3_SW_TABLE_SIZE];	
	rtk_rg_table_arp_t			arp[MAX_ARP_SW_TABLE_SIZE];
	rtk_rg_table_wantype_t  	wantype[MAX_WANTYPE_SW_TABLE_SIZE];
	rtk_rg_table_naptIn_t		naptIn[MAX_NAPT_IN_SW_TABLE_SIZE];	
	rtk_rg_table_naptOut_t		naptOut[MAX_NAPT_OUT_SW_TABLE_SIZE];
	rtk_rg_table_bind_t 		bind[MAX_BIND_SW_TABLE_SIZE];	
	rtk_rg_table_v6route_t 		v6route[MAX_IPV6_ROUTING_SW_TABLE_SIZE];
	rtk_rg_table_v6neighbor_t	v6neighbor[MAX_IPV6_NEIGHBOR_SW_TABLE_SIZE];
	rtk_rg_table_ipmcgrp_t		ipmcgrp[MAX_IPMCGRP_HW_TABLE_SIZE];
	rtk_rg_table_wlan_mbssid_t	wlanMbssid[MAX_WLAN_MBSSID_SW_TABLE_SIZE];
	int wlanMbssidHeadIdx; //the first lookup entry of the wlanMbssid table.

	rtk_rg_table_naptOut_linkList_t	*pNaptOutFreeListHead;
	rtk_rg_table_naptOut_linkList_t	naptOutFreeList[MAX_NAPT_OUT_SW_TABLE_SIZE-MAX_NAPT_OUT_HW_TABLE_SIZE];
	rtk_rg_table_naptOut_linkList_t	*pNaptOutHashListHead[MAX_NAPT_OUT_HW_TABLE_SIZE>>2]; 
	
	rtk_rg_table_naptIn_linkList_t	*pNaptInFreeListHead;
	rtk_rg_table_naptIn_linkList_t	naptInFreeList[MAX_NAPT_IN_SW_TABLE_SIZE-MAX_NAPT_IN_HW_TABLE_SIZE];
	rtk_rg_table_naptIn_linkList_t	*pNaptInHashListHead[MAX_NAPT_IN_HW_TABLE_SIZE>>2]; 

	rtk_rg_table_icmp_linkList_t *pICMPCtrlFlowHead;
	rtk_rg_table_icmp_linkList_t icmpCtrlFlowLinkList[MAX_ICMPCTRLFLOW_SIZE];

	uint32	naptTcpExternPortUsed[65536/32]; // naptTcpExternPortUsed[0] bit0==>TCP PORT 0, bit31==>TCP PORT 31, naptTcpExternPortUsed[1] bit0==>TCP PORT 32...
	uint32	naptUdpExternPortUsed[65536/32]; // naptUdpExternPortUsed[0] bit0==>UDP PORT 0, bit31==>UDP PORT 31, naptTcpExternPortUsed[1] bit0==>UDP PORT 32...
	uint16	naptTcpExternPortUsedRefCount[65535];
	uint16	naptUdpExternPortUsedRefCount[65535];

	uint32	arpTrfIndicator[512/32];//arpTrfIndicator[0] bit0==>ARP[0], bit31==>ARP[31], arpTrfIndicator[1] bit0==>ARP[32]...
	
	rtk_rg_sipDipClassification_t sipDipClass[MAX_SIP_CLASS][MAX_DIP_CLASS];
	uint32 naptValidSet[MAX_NAPT_OUT_SW_TABLE_SIZE/32];
	uint32 arpValidSet[MAX_ARP_HW_TABLE_SIZE/32];
	rtk_rg_upnpConnection_t upnp[MAX_UPNP_SW_TABLE_SIZE];
	rtk_rg_virtualServer_t virtualServer[MAX_VIRTUAL_SERVER_SW_TABLE_SIZE];
#ifdef CONFIG_RG_NAPT_DMZ_SUPPORT
	rtk_rg_dmzInfo_t dmzInfo[MAX_DMZ_TABLE_SIZE];
#endif
	rtk_rg_fragment_queue_t ipv4FragmentQueue[MAX_IPV4_FRAGMENT_QUEUE_SIZE];
	//rtk_rg_fragment_napt_out_t ipv4FragmentOutTable[MAX_NAPT_OUT_HW_TABLE_SIZE];
	rtk_rg_ipv4_fragment_out_t	*pFragOutFreeListHead;
	rtk_rg_ipv4_fragment_out_t	fragOutFreeList[MAX_FRAG_OUT_FREE_TABLE_SIZE];
	rtk_rg_ipv4_fragment_out_t	*pFragOutHashListHead[MAX_NAPT_OUT_HW_TABLE_SIZE>>2]; 	//4-way hashed, we just need MAX_NAPT_OUT_HW_TABLE_SIZE/4 indexes
	
	rtk_rg_ipv4_fragment_in_t	*pFragInFreeListHead;
	rtk_rg_ipv4_fragment_in_t	fragInFreeList[MAX_FRAG_IN_FREE_TABLE_SIZE];
	rtk_rg_ipv4_fragment_in_t	*pFragInHashListHead[MAX_NAPT_IN_HW_TABLE_SIZE>>2];		//4-way hashed, we just need MAX_NAPT_IN_HW_TABLE_SIZE/4 indexes
	
	rtk_rg_napt_shortcut_t naptShortCut[MAX_NAPT_SHORTCUT_SIZE];
	int	naptShortcutEndIdx;

#ifdef __KERNEL__
	struct list_head softwareArpFreeListHead;
	struct list_head softwareArpTableHead[MAX_ARP_SW_TABLE_HEAD]; 		//indexed by IP&0xff
#endif
	rtk_rg_arp_linkList_t softwareArpFreeList[MAX_ARP_SW_TABLE_SIZE-MAX_ARP_HW_TABLE_SIZE];

	rtk_rg_glb_system_t systemGlobal;

	//ALG module
	rtk_rg_alg_type_t algFunctionMask;	//1: enable, 0: disable
	uint32	algTcpExternPortEnabled[65536/32]; // algTcpExternPortEnabled[0] bit0==>TCP PORT 0 has alg, bit31==>TCP PORT 31 has alg, algTcpExternPortEnabled[1] bit0==>TCP PORT 32 has alg...
	uint32	algUdpExternPortEnabled[65536/32]; // algUdpExternPortEnabled[0] bit0==>UDP PORT 0 has alg, bit31==>UDP PORT 31 has alg, algUdpExternPortEnabled[1] bit0==>UDP PORT 32 has alg...
	uint32	algTcpExternPortEnabled_SrvInLan[65536/32]; // algTcpExternPortEnabled_SrvInLan[0] bit0==>TCP PORT 0 has alg, bit31==>TCP PORT 31 has alg, algTcpExternPortEnabled_SrvInLan[1] bit0==>TCP PORT 32 has alg...
	uint32	algUdpExternPortEnabled_SrvInLan[65536/32]; // algUdpExternPortEnabled_SrvInLan[0] bit0==>UDP PORT 0 has alg, bit31==>UDP PORT 31 has alg, algUdpExternPortEnabled_SrvInLan[1] bit0==>UDP PORT 32 has alg...
	rtk_rg_alg_funcMapping_t algTcpFunctionMapping[MAX_ALG_FUNCTIONS];
	rtk_rg_alg_funcMapping_t algUdpFunctionMapping[MAX_ALG_FUNCTIONS];
	uint32	algPPTPExtCallIDEnabled[65536/32]; // algPPTPExtCallIDEnabled[0] bit0==>callID 0 has been used, bit31==> callID 31 has been used, algPPTPExtCallIDEnabled[1] bit0==>callID 32 has been used...	
	rtk_rg_alg_type_t algServInLanIpMask;	//1: ip setuped, 0: no ip
	rtk_rg_alg_serverIpMapping_t algServInLanIpMapping[MAX_ALG_SERV_IN_LAN_NUM];

#ifdef CONFIG_APOLLO_ROMEDRIVER
	//initialized and used at fwdEngine only
	rtk_rg_ftpCtrlFlowEntry_t *pAlgFTPCtrlFlowHead;
	rtk_rg_ftpCtrlFlowEntry_t algFTPCtrlFlowList[MAX_FTP_CTRL_FLOW_SIZE];
		
	//rtk_rg_pptpGreEntry_t *pptpGreInboundHeadPtr[MAX_PPTP_TBL_SIZE]; 
	rtk_rg_pptpGreLinkList_t pptpGreOutboundLinkList[MAX_NETIF_SW_TABLE_SIZE][MAX_PPTP_SESSION_SIZE];
	rtk_rg_pptpGreLinkList_t * pPPTPGreOutboundHead[MAX_NETIF_SW_TABLE_SIZE];
#endif

	

} rtk_rg_globalDatabase_t;

//Global variable that used in Linux kernel or only modified when system begin
typedef struct rtk_rg_globalKernel_s
{
	rtk_rg_debug_level_t debug_level;
	rtk_rg_debug_trace_filter_bitmask_t trace_filter_bitmask;
	rtk_rgDebugTraceFilter_t trace_filter;
	int l2_hw_aging;
	
	//int ppp_diaged[MAX_NETIF_HW_TABLE_SIZE];//record which intf is PPPoE & diag on,  use when init disconnect pppoe judgement
#ifdef __KERNEL__
#ifdef CONFIG_RTL8686NIC 
	struct tx_info txDesc,txDescMask;
#else
	rtk_rg_txdesc_t txDesc,txDescMask;
#endif
	struct rx_info rxInfoFromWLAN; //for WIFI to fwdEngineInput
	struct proc_dir_entry *proc_rg;

	int timer_selector;
	struct timer_list fwdEngineHouseKeepingTimer;
	struct timer_list arpRequestTimer[MAX_NETIF_HW_TABLE_SIZE]; 
	int arpRequestTimerCounter[MAX_NETIF_HW_TABLE_SIZE];
	struct timer_list neighborDiscoveryTimer[MAX_NETIF_HW_TABLE_SIZE];	
	int neighborDiscoveryTimerCounter[MAX_NETIF_HW_TABLE_SIZE];

	struct semaphore wanStaticCalled;
	struct semaphore wanDHCPCalled;
	struct semaphore wanPPPOEAfterCalled;
	struct semaphore interfaceLock;			//used when interface add or del
	struct semaphore ipv4FragLock;			//used when ipv4 fragment packet add to or lookup table
	struct semaphore ipv4FragFreeLock;		//used when ipv4 fragment packet need to manipulate free list
	struct semaphore ipv4FragQueueLock;		//used when ipv4 fragment packet queue or dequeue
	struct semaphore igmpsnoopingLock;	
	//struct semaphore saLearningLimitLock;		//used when check or add source address learning count
	//struct semaphore linkChangeHandlerLock;		//used when check and clear link-down indicator register
#else
	rtk_rg_txdesc_t txDesc,txDescMask;

	void *wanStaticCalled;
	void *wanDHCPCalled;
	void *wanPPPOEAfterCalled;
	void *interfaceLock;
	void *ipv4FragLock;
	void *ipv4FragFreeLock;
	void *ipv4FragQueueLock;
	void *igmpsnoopingLock;
	//void *saLearningLimitLock;
	//void *linkChangeHandlerLock;
#endif
}rtk_rg_globalKernel_t;

/* End of Tables ========================================================== */




#endif

