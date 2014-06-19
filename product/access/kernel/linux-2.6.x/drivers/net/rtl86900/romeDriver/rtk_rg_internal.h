#ifndef RTK_RG_INTERNAL_H
#define RTK_RG_INTERNAL_H

#include <rtk_rg_define.h>
#include <rtk_rg_struct.h>


#if 0 //ysleu: I think it's not good to mix model & rome driver codes toghter
#include <hsModel.h>
#endif

// OS-dependent defination
#ifdef CONFIG_APOLLO_MODEL
#define rtlglue_printf printf
#ifndef bzero
#define bzero(p,s) memset(p,0,s)
#endif
#define rg_lock(x) do{}while(0)
#define rg_unlock(y) do{}while(0) 
#else
#ifdef __KERNEL__
#define rtlglue_printf printk
#ifndef bzero
#define bzero(p,s) memset(p,0,s)
#endif
#define rg_lock(x) down(x)//down_interruptible(x)
#define rg_unlock(y) up(y)
#endif
#endif

//Extern external functions
extern void *rtk_rg_malloc(int NBYTES);
extern void rtk_rg_free(void *APTR);
struct sk_buff *rtk_rg_skbCopyToPreAllocSkb(struct sk_buff *skb);
	
void memDump (void *start, unsigned int size, char * strHeader);

//Extern RG Global Variables
extern rtk_rg_globalKernel_t rg_kernel;
extern rtk_rg_intfInfo_t RG_GLB_INTF_INFO[8];

#ifndef RTN_PS
#define RTN_PS 2
#endif

#ifndef DISABLE
#define DISABLE 0
#endif

#ifndef ENABLE
#define ENABLE 1
#endif

#define UNKNOW_INTF 0
#define WAN_INTF 1
#define LAN_INTF 2

//NAPT/NAPT-R Traffic
#define REG_L4_TRF0	0x00800300
#define REG_L4_TRF1	0x00800400
#define REG_NAT_CTRL	0x00800010
#define CLR_L4_TRF_DONE	0x0
#define CLR_L4_TRF0	0x1
#define CLR_L4_TRF1	0x2
#define CLR_L4_TRF_BOTH	0x3
#define FIELD_L4_TRF_CLR_OFFSET 19
#define FIELD_L4_TRF_CHG_OFFSET 21
#define FIELD_L4_TRF_SEL_OFFSET 22

//ARP Traffic
#define REG_ARP_TRF0	0x00800500
#define REG_ARP_TRF1	0x00800600
#define CLR_ARP_TRF_DONE	0x0
#define CLR_ARP_TRF0	0x1
#define CLR_ARP_TRF1	0x2
#define CLR_ARP_TRF_BOTH	0x3
#define FIELD_ARP_TRF_CLR_OFFSET 15
#define FIELD_ARP_TRF_CHG_OFFSET 17
#define FIELD_ARP_TRF_SEL_OFFSET 18

#ifndef ASSERT_EQ
#define ASSERT_EQ(value1,value2)\
do {\
		int func_return;\
		int expect_return;\
		func_return=value1;\
		expect_return=value2;\
		if ((func_return) != (expect_return)) {\
				rtlglue_printf("\033[31;43m%s(%d): func_return=0x%x expect_return=0x%x, fail, so abort!\033[m\n", __FUNCTION__, __LINE__,(func_return),(expect_return));\
				return func_return; \
		}\
}while (0)
#endif

#define debug( comment ,arg...) \
do {\
	char mt_trace_head_str[32]="[RomeDriver]"; \
	{\
		char mt_watch_tmp[512];\
		int mt_trace_i;\
		sprintf( mt_watch_tmp, comment,## arg);\
		for(mt_trace_i=0;mt_trace_i<512;mt_trace_i++) \
		{\
			if(mt_watch_tmp[mt_trace_i]=='\n') mt_watch_tmp[mt_trace_i]=' '; \
			else if(mt_watch_tmp[mt_trace_i]==0) break; \
		}\
		rtlglue_printf("%s %s \033[1;30m@%s:%d\033[0m\n",mt_trace_head_str,mt_watch_tmp,__FILE__,__LINE__);\
	}\
} while(0);


//RTK API sync with rg_db MACRO
int32 RTK_L2_ADDR_ADD(rtk_l2_ucastAddr_t *pL2Addr);
int32 RTK_L2_ADDR_DEL(rtk_l2_ucastAddr_t *pL2Addr);
int32 RTK_L34_NETIFTABLE_SET(uint32 idx, rtk_l34_netif_entry_t *entry);
int32 RTK_L34_ROUTINGTABLE_SET(uint32 idx, rtk_l34_routing_entry_t *entry);
int32 RTK_L34_EXTINTIPTABLE_SET(uint32 idx, rtk_l34_ext_intip_entry_t *entry);
int32 RTK_L34_NEXTHOPTABLE_SET(uint32 idx, rtk_l34_nexthop_entry_t *entry);
int32 RTK_L34_PPPOETABLE_SET(uint32 idx, rtk_l34_pppoe_entry_t *entry);
int32 RTK_L34_ARPTABLE_SET(uint32 idx, rtk_l34_arp_entry_t *entry);
int32 RTK_L34_NAPTINBOUNDTABLE_SET(int8 forced,uint32 idx, rtk_l34_naptInbound_entry_t *entry);
int32 RTK_L34_NAPTOUTBOUNDTABLE_SET(int8 forced,uint32 idx, rtk_l34_naptOutbound_entry_t *entry);
int32 RTK_L34_WANTYPETABLE_SET(uint32 idx,rtk_wanType_entry_t * entry);
int32 RTK_VLAN_CREATE(rtk_vlan_t vid);
int32 RTK_VLAN_PORT_SET(rtk_vlan_t vid,rtk_portmask_t * pMember_portmask,rtk_portmask_t * pUntag_portmask);
int32 RTK_VLAN_EXTPORT_SET(rtk_vlan_t vid,rtk_portmask_t * pExt_portmask);
int32 RTK_VLAN_FID_SET(rtk_vlan_t vid,rtk_fid_t fid);
int32 RTK_VLAN_FIDMODE_SET(rtk_vlan_t vid,rtk_fidMode_t mode);
int32 RTK_VLAN_PRIORITY_SET(rtk_vlan_t vid,rtk_pri_t priority);
int32 RTK_VLAN_PRIORITYENABLE_SET(rtk_vlan_t vid,rtk_enable_t enable);
int32 RTK_VLAN_DESTROY(rtk_vlan_t vid);
int32 RTK_VLAN_PORTPVID_SET(rtk_port_t port,uint32 pvid);
int32 RTK_VLAN_EXTPORTPVID_SET(uint32 extPort,uint32 pvid);
int32 RTK_L34_BINDINGTABLE_SET(uint32 idx,rtk_binding_entry_t * bindEntry);
int32 RTK_L34_IPV6ROUTINGTABLE_SET(uint32 idx,rtk_ipv6Routing_entry_t * ipv6RoutEntry);
int32 RTK_L34_IPV6NEIGHBORTABLE_SET(uint32 idx,rtk_ipv6Neighbor_entry_t * ipv6NeighborEntry);


int _rtk_rg_search_acl_empty_Entry(int size, int* index);
int _rtk_rg_search_cf_empty_Entry(int size,int* index);
int _rtk_rg_search_acl_empty_portTableEntry(int* index);
int _rtk_rg_search_acl_empty_ipTableEntry(int* index);
int _rtk_rg_search_cf_empty_portTableEntry(int* index);
int _rtk_rg_search_cf_empty_ipTableEntry(int* index);
int _rtk_rg_search_cf_empty_dscpTableEntry(int* index);

int _rtk_rg_conflictField_and_flowDirection_check(rtk_rg_aclFilterAndQos_t* acl_filter, rtk_rg_flow_direction_t* flow_direction);
int _rtk_rg_free_cf_portTableEntry(int index);
int _rtk_rg_free_cf_ipTableEntry(int index);
int _rtk_rg_free_acl_portTableEntry(int index);
int _rtk_rg_free_acl_ipTableEntry(int index);
int _rtk_rg_free_cf_dscpTableEntry(int index);

int _rtk_rg_asic_defaultDropEntry_setup(void);
int _rtk_rg_asic_defaultDropEntry_remove(void);

//setup aclSWEntry
int _rtk_rg_acl_asic_init(void);
int _rtk_rg_classify_asic_init(void);
int _rtk_rg_portmask_translator(rtk_rg_portmask_t in_pmask, rtk_portmask_t* out_mac_pmask, rtk_portmask_t* out_ext_pmask);
int _rtk_rg_is_aclSWEntry_init(void);
int _rtk_rg_aclSWEntry_init(void);
int _rtk_rg_aclSWEntry_get(int index, rtk_rg_aclFilterEntry_t* aclSWEntry);
int _rtk_rg_aclFilterAndQos_check_testChip_feature(rtk_rg_aclFilterAndQos_t *acl_filter);
int _rtk_rg_aclFilterAndQos_check_notSupport_feature(rtk_rg_aclFilterAndQos_t *acl_filter);
int _rtk_rg_aclSWEntry_set(int index, rtk_rg_aclFilterEntry_t aclSWEntry);
int _rtk_rg_aclSWEntry_empty_find(int* index);
int _rtk_rg_aclSWEntry_free_asic(rtk_rg_aclFilterEntry_t aclSWEntry);
int _rtk_rg_aclSWEntry_rearrange_asic(void);
int _rtk_rg_aclSWEntry_and_asic_add(rtk_rg_aclFilterAndQos_t *acl_filter,rtk_rg_aclFilterEntry_t* aclSWEntry);
int _rtk_rg_aclSWEntry_reAdd(rtk_rg_aclFilterAndQos_t *acl_filter, int *acl_filter_idx);
int _rtk_rg_aclSWEntry_dump(void);
int _rtk_rg_is_stormControlEntry_init(void);
int _rtk_rg_stormControlEntry_init(void);

int32 _rtk_rg_acl_pppoe_passthrough_for_wanIntf_add(int intf_idx, rtk_mac_t gmac);
int32 _rtk_rg_acl_pppoe_passthrough_for_wanIntf_del(int intf_idx);
#if 0
int32 _rtk_rg_acl_trap_broadcast_add(void);
int32 _rtk_rg_acl_trap_broadcast_del(void);
#endif
int32 _rtk_rg_acl_pppoe_passthrough_add(unsigned int lan_pmask, unsigned int wan_pmsk, int remark_vid);
int32 _rtk_rg_acl_pppoe_passthrough_del(void);
int32 _rtk_rg_extPMaskTranslateRedirect_add(unsigned int igr_extPmsk,unsigned int egr_extPmsk);
int32 _rtk_rg_internalVidPriTranslate(uint32 in_cvid, uint32 in_cpri, uint32 tran_cvid, uint32 tran_pri);
int _rtk_rg_ingressACLPatternCheck(rtk_rg_pktHdr_t *pPktHdr);
int _rtk_rg_egressACLPatternCheck(int direct, int naptIdx, rtk_rg_pktHdr_t *pPktHdr,struct sk_buff *skb,int l3Modify,int l4Modify);
int _rtk_rg_egressACLAction(rtk_rg_pktHdr_t *pPktHdr);
int _rtk_rg_modifyPacketByACLAction(struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr);
int _rtk_rg_egressPacketSend(struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr);




/* MAC */
int _rtk_rg_is_macFilter_table_init(void);
int _rtk_rg_macFilter_table_init(void);
int _rtk_rg_is_urlFilter_table_init(void);
int _rtk_rg_urlFilter_table_init(void);


/* NIC Module */
#define RG_FWDENGINE_PKT_LEN	2048
#define RX_OFFSET	2


/* NAPT */
int _rtk_rg_l3lookup(ipaddr_t ip);
uint32 _rtk_rg_NAPTRIndex_get(uint16 isTCP, uint32 dip, uint16 dport);
unsigned int _rtk_rg_NAPTIndex_get(unsigned char isTCP, unsigned int srcAddr, unsigned short srcPort, unsigned int destAddr, unsigned short destPort);
unsigned int _rtk_rg_NAPTRemoteHash_get( unsigned int ip, unsigned int port);

/* IPv6 */
uint8 _rtk_rg_CompareIFID(uint8* dip, uint64 interfaceid);
int _rtk_rg_v6L3lookup(unsigned char *ipv6);


/*Other Module*/

//extern rtk_rg_arp_request_t	RG_GLB_ARP_REQUEST[MAX_ASIC_NETIF_TBL_SIZE];

//called back from Layer 2, return the IPaddr and mac for gateway 
//if the ARP request is not found or timeout, return L2Idx = -1
int32 _rtk_rg_internal_wanSet(int wan_intf_idx, rtk_rg_ipStaticInfo_t *hw_static_info);
int _rtk_rg_internal_GWMACSetup(ipaddr_t IPaddr, int L2Idx);	
int _rtk_rg_internal_GWMACSetup_stage2(int matchIdx, int l2Idx);
int _rtk_rg_internal_IPV6GWMACSetup(unsigned char *ipv6Addr, int L2Idx);	
int _rtk_rg_internal_IPV6GWMACSetup_stage2(int matchIdx, int l2Idx);
int _rtk_rg_globalVariableReset(void);
int _rtk_rg_fwdEngineGlobalVariableReset(void);
int _rtk_rg_layer2GarbageCollection(int l2Idx);
int _rtk_rg_layer2LeastRecentlyUsedReplace(int l2Idx);

#ifdef __KERNEL__
void _rtk_rg_switchLinkChangeHandler(void);
int _rtk_rg_broadcastForward(struct sk_buff *skb, unsigned int internalVlanID, unsigned int srcPort,unsigned int extSpa);
int _rtk_rg_broadcastForwardWithPkthdr(rtk_rg_pktHdr_t *pPktHdr, struct sk_buff *skb, unsigned int internalVlanID, unsigned int srcPort,unsigned int extSpa);
#ifdef CONFIG_RG_WLAN_HWNAT_ACCELERATION
int _rtk_rg_egressPacketSendWifi(struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr);
#endif
#endif


#define RG_ONE_COUNT(x)\
do {\
	x = (x & 0x55555555) + ((x & 0xaaaaaaaa) >> 1);\
	x = (x & 0x33333333) + ((x & 0xcccccccc) >> 2);\
	x = (x & 0x0f0f0f0f) + ((x & 0xf0f0f0f0) >> 4);\
    x = (x & 0x00ff00ff) + ((x & 0xff00ff00) >> 8);\
    x = (x & 0x0000ffff) + ((x & 0xffff0000) >> 16);\
} while (0)

typedef struct _rg_DHCPStaticEntry_s
{
	ipaddr_t ip;
	rtk_mac_t mac;
	int valid;
} _rg_DHCPStaticEntry_t;

unsigned int _rtk_rg_hash_mac_fid_efid(unsigned char *mac,unsigned int fid,unsigned int efid);
unsigned int _rtk_rg_hash_mac_vid_efid(unsigned char *mac,unsigned int vid,unsigned int efid);
unsigned int _rtk_rg_hash_sip_gip(unsigned int sip,unsigned int gip);


#ifdef CONFIG_RTL8686NIC
void _rtk_rg_interfaceVlanIDPriority(rtk_rg_intfInfo_t *pStoredInfo,struct tx_info *ptxInfo,struct tx_info *ptxInfoMask);
#else
void _rtk_rg_interfaceVlanIDPriority(rtk_rg_intfInfo_t *pStoredInfo,rtk_rg_txdesc_t *ptxInfo,rtk_rg_txdesc_t *ptxInfoMask);
#endif
void _rtk_rg_wanVlanTagged(int vlan_tag_on);
int _rtk_rg_arpGeneration(rtk_rg_intfInfo_t *pStoredInfo,ipaddr_t gwIpAddr,rtk_rg_arp_request_t *arpReq);
int _rtk_rg_NDGeneration(rtk_rg_intfInfo_t *pStoredInfo,rtk_ipv6_addr_t gwIpAddr,rtk_rg_neighbor_discovery_t *neighborDisc);
int _rtk_rg_arpAndMacEntryAdd(ipaddr_t sip,int sipL3Idx,uint8 *pSmac,int srcPortIdx, int *pL2Idx,int macEntryForceAdd,int cvid,int cvidForceAdd,int arpEntryForceAdd);
int _rtk_rg_naptConnection_add(int naptIdx, rtk_l34_naptOutbound_entry_t *asic_napt, rtk_l34_naptInbound_entry_t *asic_naptr);
uint32 _rtk_rg_naptTcpUdpOutHashIndex(int8 isTcp, ipaddr_t srcAddr, uint16 srcPort, ipaddr_t destAddr, uint16 destPort);
uint32 _rtk_rg_naptTcpUdpInHashIndex(uint16 isTcp, uint32 dip, uint16 dport);
int _rtk_rg_naptTcpUdpOutHashIndexLookup(int8 isTcp, ipaddr_t srcAddr, uint16 srcPort, ipaddr_t destAddr, uint16 destPort);
int _rtk_rg_naptTcpUdpOutHashIndexLookupByPktHdr(int8 isTcp, rtk_rg_pktHdr_t *pPktHdr);
int _rtk_rg_naptTcpUdpInHashIndexLookup(int8 isTcp, ipaddr_t remoteAddr, uint16 remotePort, ipaddr_t extAddr, uint16 extPort);
unsigned char _rtk_rg_IPv6NeighborHash(unsigned char *dip, unsigned char rtidx);
int _rtk_rg_neighborAndMacEntryAdd(unsigned char *sip,int sipL3Idx,uint8 *pSmac,int srcPortIdx, int *pNeighborIdx,int macEntryForceAdd);

int32 _rtk_rg_addArpRoutingArray(rtk_rg_routing_arpInfo_t *newAddingEntry, ipaddr_t newIpAddr, int intfVlanId);
int32 _rtk_rg_delArpRoutingArray(rtk_rg_routing_arpInfo_t *deletingEntry);
int32 _rtk_rg_arpRearrange(rtk_rg_routing_arpInfo_t *newAddingEntry, ipaddr_t newIpAddr, int routingARPNum);

int _rtk_rg_fwdEngineDirectTx(struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr);
int _rtk_rg_fwdengine_handleArpMiss(rtk_rg_pktHdr_t *pPktHdr);
int _rtk_rg_fwdEngine_upnpCheck(void*,ipaddr_t*,uint16*);
int _rtk_rg_fwdEngine_virtualServerCheck(void*,ipaddr_t*,uint16*);
int _rtk_rg_fwdEngine_dmzCheck(void*,ipaddr_t*,uint16*);
int _rtk_rg_eiplookup(ipaddr_t ip);
int _rtk_rg_softwareArpTableLookUp(unsigned short routingIdx, ipaddr_t ipAddr, rtk_rg_arp_linkList_t **pSoftwareArpEntry);

//Multicast
//1 FIXME: this declaration should be delete when the liteRomeDriver.h is ready!!
int32 rtk_rg_ipv4MultiCastFlow_add(rtk_rg_ipv4MulticastFlow_t *ipv4McFlow,int *flow_idx);


//IGMP
int32 rtl_flushAllIgmpRecord(void);


//ALG
int _rtk_rg_algCheckEnable(unsigned char isTCP,unsigned short checkPort);

//WIFI
extern int rtl8192cd_start_xmit(struct sk_buff *skb, struct net_device *dev);
void _rtk_rg_wlanMbssidLearning(u8* smac,int wlan_dev_idx);
void _rtk_rg_wlanMbssidLookup(u8 *dmac,int *wlan_dev_idx);
int _rtk_master_wlan_mbssid_tx(struct sk_buff *skb);
extern struct net_device *wlan_root_netdev;
extern struct net_device *wlan_vap_netdev[4];
//TOOL
void _rtk_rg_str2mac(unsigned char *mac_string,rtk_mac_t *pMacEntry);

//NIC
struct sk_buff *re8670_getAlloc(unsigned int size);
struct sk_buff *re8670_getBcAlloc(unsigned int size);
struct sk_buff *re8670_getMcAlloc(unsigned int size);

#endif

