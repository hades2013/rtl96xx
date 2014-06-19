/*
Linux Kernel Hacking:
	net/core/neighbour.c						// ARP
	net/ipv4/fib_hash.c							// ROUTE
	net/ipv4/netfilter/ip_conntrack_core.c		// NAPT (PATH*2)
	net/ipv4/netfilter/ip_nat_core.c			// NAPT (PATH*2)
	net/ipv4/ip_input.c							// FastPath_Enter()
	net/ipv4/ip_output.c						// FastPath_Track()
*/

#include "fastpath_core.h"
#include <linux/notifier.h>
#include <linux/netfilter.h>
#include <net/netfilter/nf_conntrack_ecache.h>

#ifdef CONFIG_RTL8672_MIPS16_IPV4
#include "../../mips16_lib.h"
#endif


struct timer_list fp_monitor_timer;
#define HEAVY_TRAFFIC	5000
#define BIT(nr)	(1UL << (nr))

enum {
	FP_HEAVY = BIT(0)
};



//#define ip_ct_tcp_timeout_established tcp_timeouts[TCP_CONNTRACK_ESTABLISHED]
//#define ip_ct_udp_timeout_stream nf_ct_udp_timeout_stream


#define	MODULE_NAME	"Realtek SD2-FastPath"
#define	MODULE_VERSION_FP	"v1.00beta_2.4.26-uc0"
//tylo, temp. remove code swap function
#define __SWAP

enum {
	ST_INIT =  0,
	ST_OPERATIONAL, 
	ST_ADMIN_DOWN
};

enum {
	EVT_SAR_UP = 1,
	EVT_SAR_DOWN,
	EVT_ADMIN_DOWN,
 	EVT_ADMIN_UP
};

//#if defined(CONFIG_RTL867X_IPTABLES_FAST_PATH) || defined(CONFIG_RTL867X_PACKET_PROCESSOR)
#ifdef CONFIG_DSL_CODESWAP
int fp_on=0;
static int fp_state = 0;
static int fp_admin = 1; // administrative status.
#else
int fp_on=2;
#endif
//#else
//int fp_on=0;
//#endif


#define	DEBUG_PROCFILE	/* Create ProcFile for debug */

/* ==================================================================================================== */
static __u8 fastpath_forward_flag = 1;		/* default: On */

/* --- ARP Table Structures --- */
struct Arp_Table
{
	CTAILQ_HEAD(Arp_list_entry_head, Arp_List_Entry) list[ARP_TABLE_LIST_MAX];
};

CTAILQ_HEAD(Arp_list_inuse_head, Arp_List_Entry) arp_list_inuse;
CTAILQ_HEAD(Arp_list_free_head, Arp_List_Entry) arp_list_free;

struct Arp_Table *table_arp;

/* --- Route Table Structures --- */
struct Route_Table
{
	CTAILQ_HEAD(Route_list_entry_head, Route_List_Entry) list[ROUTE_TABLE_LIST_MAX];
};

CTAILQ_HEAD(Route_list_inuse_head, Route_List_Entry) route_list_inuse;
CTAILQ_HEAD(Route_list_free_head, Route_List_Entry) route_list_free;

struct Route_Table *table_route;

/* --- NAPT Table Structures --- */
struct Napt_List_Entry
{
	__u8 valid;
	//cathy
        //enum NP_PROTOCOL protocol;
	__u16 protocol;
	ip_t intIp;
	__u32 intPort;
	ip_t extIp;
	__u32 extPort;
	ip_t remIp;
	__u32 remPort;
	enum NP_FLAGS flags;
	__u16 state;	//0-unreplied   1-established.  2-path exist
	__u16 refcnt;
    struct nf_conn * ct;
	CTAILQ_ENTRY(Napt_List_Entry) napt_link;
	CTAILQ_ENTRY(Napt_List_Entry) tqe_link;
};

struct Napt_Table
{
	CTAILQ_HEAD(Napt_list_entry_head, Napt_List_Entry) list[NAPT_TABLE_LIST_MAX];
};

CTAILQ_HEAD(Napt_list_inuse_head, Napt_List_Entry) napt_list_inuse;
CTAILQ_HEAD(Napt_list_free_head, Napt_List_Entry) napt_list_free;

#ifdef CONFIG_RTL8672
__DRAM
#endif
struct Napt_Table *table_napt;


struct Path_Table
{
	CTAILQ_HEAD(Path_list_entry_head, Path_List_Entry) list[PATH_TABLE_LIST_MAX];
};

CTAILQ_HEAD(Path_list_inuse_head, Path_List_Entry) path_list_inuse;
CTAILQ_HEAD(Path_list_free_head, Path_List_Entry) path_list_free;

#ifdef CONFIG_RTL8672
__DRAM
#endif 
struct Path_Table *table_path;

#if 0
/* --- InterFace Table Structures --- */
struct If_List_Entry
{
	__u8			valid;
	__u8			ifname[IFNAME_LEN_MAX];
	ip_t		ipAddr;
	ether_t	mac;
	__u32			mtu;
	enum IF_FLAGS	flags;
	CTAILQ_ENTRY(If_List_Entry) if_link;
	CTAILQ_ENTRY(If_List_Entry) tqe_link;
};

CTAILQ_HEAD(If_list_inuse_head, If_List_Entry) if_list_inuse;
CTAILQ_HEAD(If_list_free_head, If_List_Entry) if_list_free;
#endif

/* ==================================================================================================== */

static __u32 __SWAP FastPath_Hash_ARP_Entry(ip_t ip)
{
	//ql: maybe ip&0xF is better;
	//return (ip % 16);
	return (ip & 0xF);
}

static __u32 __SWAP 
FastPath_Hash_ROUTE_Entry(ip_t ip, ip_t mask)
{
	int i;
	ip_t tmp = (ip & mask);
	
	for(i=0; i<32; i++) {
		if (tmp & 0x00000001) {
			return (tmp + (__u32)i) % ROUTE_TABLE_LIST_MAX;
		}
		tmp = tmp >> 1;
	}
	
	return 0;
}

static __u32 __SWAP 
FastPath_Hash_NAPT_Entry(ip_t intIp,__u32 intPort,
			ip_t extIp, __u32 extPort,
			ip_t remIp, __u32 remPort)
{
	__u32 hash;

	hash = (0xff000000 & intIp) >> 24;
	hash ^= (0x00ff0000 & intIp) >> 16;
	hash ^= (0x0000ff00 & intIp) >> 8;
	hash ^= (0x000000ff & intIp);
	hash ^= (0x0000ff00 & intPort) >> 8;
	hash ^= (0x000000ff & intPort);
	
	hash ^= (0xff000000 & extIp) >> 24;
	hash ^= (0x00ff0000 & extIp) >> 16;
	hash ^= (0x0000ff00 & extIp) >> 8;
	hash ^= (0x000000ff & extIp);
	hash ^= (0x0000ff00 & extPort) >> 8;
	hash ^= (0x000000ff & extPort);

	hash ^= (0xff000000 & remIp) >> 24;
	hash ^= (0x00ff0000 & remIp) >> 16;
	hash ^= (0x0000ff00 & remIp) >> 8;
	hash ^= (0x000000ff & remIp);
	hash ^= (0x0000ff00 & remPort) >> 8;
	hash ^= (0x000000ff & remPort);
	
	// Kaohj
	//return 0x000003ff & (hash ^ (hash >> 12));
	return (NAPT_TABLE_LIST_MAX-1) & (hash ^ (hash >> 12));
}

__IRAM  inline static __u32
FastPath_Hash_PATH_Entry(ip_t sip, __u32 sport, ip_t dip, __u32 dport, __u16 proto)
{
	register __u32 hash;
	
	hash = ((sip>>16)^sip);
	hash ^= ((dip>>16)^dip);
	hash ^= sport;
	hash ^= dport;
//cathy
	hash ^= proto;
	// Kaohj
	//return 0x000003ff & (hash ^ (hash >> 12));
	return (PATH_TABLE_LIST_MAX-1) & (hash ^ (hash >> 12));
}


/* ==================================================================================================== */

#if 0
enum LR_RESULT
rtl865x_addFdbEntry(__u32 vid,
		__u32 fid,
		ether_t* mac,
		__u32 portmask,
		enum FDB_FLAGS flags)
{
	DEBUGP_API("addFdbEntry: vid=%u fid=%u mac=%p portmask=0x%08X flasg=0x%08X \n", vid, fid, mac, portmask, flags);
	
	return LR_SUCCESS;
}

enum LR_RESULT
rtl865x_delFdbEntry(__u32 vid,
		__u32 fid,
		ether_t* mac)
{
	DEBUGP_API("delFdbEntry: vid=%u fid=%u mac=%p \n", vid, fid, mac);
	
	return LR_SUCCESS;
}
#endif

enum LR_RESULT __SWAP 
rtl867x_addArp(ip_t ip,
		ether_t* mac,
		enum ARP_FLAGS flags)
{
	__u32 hash = FastPath_Hash_ARP_Entry(ip);
	struct Arp_List_Entry *ep;
	
	DEBUGP_API("addArp: ip=0x%08X mac=%02X:%02X:%02X:%02X:%02X:%02X flags=0x%08X Hash=%u \n", ip, MAC2STR(*mac), flags, hash);
	
	/* Lookup */
	CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
		if (ep->ip == ip) {
			DEBUGP_SYS("addArp: ERROR - arp(ip=0x%08X) already exist! \n", ip);
			return LR_EXIST;
		}
	}
	
	/* Create */
	if(!CTAILQ_EMPTY(&arp_list_free)) {
		struct Arp_List_Entry *entry_arp;
		entry_arp = CTAILQ_FIRST(&arp_list_free);
		entry_arp->ip = ip;
		entry_arp->mac = *mac;
		entry_arp->flags = flags;
		entry_arp->valid = 0xff;
		CTAILQ_REMOVE(&arp_list_free, entry_arp, tqe_link);
		CTAILQ_INSERT_TAIL(&arp_list_inuse, entry_arp, tqe_link);
		CTAILQ_INSERT_TAIL(&table_arp->list[hash], entry_arp, arp_link);
	} else {
		DEBUGP_SYS("addArp: ERROR - arp_list_free is empty! \n");
		return LR_FAILED;
	}
	
	return LR_SUCCESS;
}

enum LR_RESULT __SWAP 
rtl867x_modifyArp(ip_t ip,
		ether_t* mac,
		enum ARP_FLAGS flags)
{
	__u32 hash = FastPath_Hash_ARP_Entry(ip);
	struct Arp_List_Entry *ep;
	
	DEBUGP_API("modifyArp: ip=0x%08X mac=%02X:%02X:%02X:%02X:%02X:%02X flags=0x%08X \n", ip, MAC2STR(*mac), flags);
	
	/* Lookup */
	CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
		if (ep->ip == ip) {
			ep->mac = *mac;
			ep->flags = flags;
			
			return LR_SUCCESS;
		}
	}
	
	return LR_SUCCESS;
}

/*
	delArp() - Delete an entry of Arp Table.
*/
enum LR_RESULT __SWAP 
rtl867x_delArp(ip_t ip)
{
	__u32 hash = FastPath_Hash_ARP_Entry(ip);
	struct Arp_List_Entry *ep;
	
	DEBUGP_API("delArp: ip=0x%08X \n", ip);
	
	/* Lookup */
	CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
		if (ep->ip == ip) {
			ep->valid = 0x00;
			CTAILQ_REMOVE(&table_arp->list[hash], ep, arp_link);
			CTAILQ_REMOVE(&arp_list_inuse, ep, tqe_link);
			CTAILQ_INSERT_TAIL(&arp_list_free, ep, tqe_link);
			
			return LR_SUCCESS;
		}
	}
	
	return LR_NONEXIST;
}

//cathy
u32 LANsub[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
u32 LANmask[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
u32 routeIndex = 0;

__u32 ExistInLAN(ip_t ip, ip_t mask){
	__u32 i;
	if(ip){
		for( i=0; i<routeIndex&& routeIndex<=8; i++ ) 
			if( LANsub[i] == ip && LANmask[i] == mask ) 		
				return 1;	//found it, return 1
	}
	return 0; //doesnt exist, return 0
}

//#if defined(CONFIG_PPTP) || defined(CONFIG_PPPOL2TP) || defined(CONFIG_NET_IPIP)
u32 WANsub[8] = {[0 ... 7]=0};
u32 wanIndex=0;
__u32 ExistInWAN(ip_t ip)
{
	__u32 i;
	if (ip)
	{
		for (i=0; i<wanIndex && i<8; i++)
			if (WANsub[i] == ip)
				return 1;
	}
	return 0;
}
//#endif//end of CONFIG_PPTP || CONFIG_PPPOL2TP || CONFIG_NET_IPIP

enum LR_RESULT __SWAP 
rtl867x_addRoute(ip_t ip,
		ip_t mask,
		ip_t gateway,
		__u8* ifname,
		enum RT_FLAGS flags,
		int type)
{
	int ppplan=-1;
	__u32 hash = FastPath_Hash_ROUTE_Entry(ip, mask);
	
	/* For fixing the bug that system will be hang if keep changing secondary IP&netmask . */
        // Just skip the addition if node already existed in array 
	struct Route_List_Entry *ep;		
	/* Looking for the matched ip & netmask node in list */
	CTAILQ_FOREACH(ep, &route_list_inuse, tqe_link) {
		if(ep->ip==ip && ep->mask == mask)			
			return LR_SUCCESS;	
        }
	DEBUGP_API("addRoute: ip=0x%08X mask=0x%08X gateway=0x%08X ifname=%s flags=0x%08X Hash=%u type=%d\n", 
		ip, mask, gateway, ifname, flags, hash, type);
	
	if(!CTAILQ_EMPTY(&route_list_free)) {
		struct Route_List_Entry *entry_route;
		entry_route = CTAILQ_FIRST(&route_list_free);
		entry_route->ip = ip;
		entry_route->mask = mask;
		entry_route->gateway = gateway;
		memcpy(&entry_route->ifname, ifname, IFNAME_LEN_MAX - 1);
		entry_route->flags = flags;
		entry_route->valid = 0xff;
		CTAILQ_REMOVE(&route_list_free, entry_route, tqe_link);
		CTAILQ_INSERT_TAIL(&route_list_inuse, entry_route, tqe_link);
		CTAILQ_INSERT_TAIL(&table_route->list[hash], entry_route, route_link);
		//cathy, for multi-subnet
		if (!strncmp(ifname, "ppp", 3)) {
			sscanf(ifname, "ppp%d", &ppplan);
		}
		
		if( type == RTN_UNICAST && (strstr(ifname, "br")
			|| ((ppplan!=-1) && (ppplan>=8)
//#if defined(CONFIG_PPTP) || defined(CONFIG_PPPOL2TP) || defined(CONFIG_NET_IPIP)
				#ifdef CONFIG_PPPOE
				 && isPPPoEDev(ifname)
				#endif
//#endif//end of CONFIG_PPTP || CONFIG_PPPOL2TP || CONFIG_NET_IPIP
				)/*pppoe proxy lan itf*/
			) ) {
            /* For fixing the bug that system will be hang if keep changing secondary IP&netmask . */
 			//Add it if ip&mask we dont know
 			#if 0
			if(ExistInLAN(ip, mask)==0){
			#else
			if(ExistInLAN(ip, mask)==0 && routeIndex < 8){ //xl_yue:20100226 add restriction for routeIndex to avoid memory overlay
			#endif  
				LANsub[routeIndex] = ip & mask;
				LANmask[routeIndex] = mask;
				routeIndex++;
			}
		}
//#if defined(CONFIG_PPTP) || defined(CONFIG_PPPOL2TP) || defined(CONFIG_NET_IPIP)
		/*we must remember wan IP for every wan port */
		else if (type == RTN_UNICAST) {//wan interface
			ip_t addr;
			addr = getNetAddrbyName(ifname, ip);
			if (addr) {
				if (!ExistInWAN(addr) && wanIndex<8) {
					WANsub[wanIndex++] = addr;
				}
			}
		}
//#endif//end of CONFIG_PPTP || CONFIG_PPPOL2TP || CONFIG_NET_IPIP
	} else {
		DEBUGP_SYS("addRoute: ERROR - Route_list_free is empty! \n");
		
		return LR_FAILED;
	}
	
	return LR_SUCCESS;
}

enum LR_RESULT __SWAP 
rtl867x_modifyRoute(ip_t ip,
		ip_t mask,
		ip_t gateway,
		__u8* ifname,
		enum RT_FLAGS flags,
		int type)
{
	__u32 hash = FastPath_Hash_ROUTE_Entry(ip, mask);
	struct Route_List_Entry *ep;
	
	DEBUGP_API("modifyRoute: ip=0x%08X mask=0x%08X gateway=0x%08X ifname=%s flags=0x%08X \n", 
		ip, mask, gateway, ifname, flags);
		
	/* Lookup */
	CTAILQ_FOREACH(ep, &table_route->list[hash], route_link) {
		if (ep->ip == ip && ep->mask == mask) {
			ep->gateway = gateway;
			memcpy(&ep->ifname, ifname, IFNAME_LEN_MAX - 1);
			ep->flags = flags;
			CTAILQ_REMOVE(&table_route->list[hash], ep, route_link);
			CTAILQ_REMOVE(&route_list_inuse, ep, tqe_link);
			CTAILQ_INSERT_TAIL(&route_list_free, ep, tqe_link);
			return LR_SUCCESS;
		}
	}
	
	return LR_SUCCESS;
}

enum LR_RESULT __SWAP 
rtl867x_delRoute(ip_t ip, ip_t mask)
{
	__u32 hash = FastPath_Hash_ROUTE_Entry(ip, mask);
	struct Route_List_Entry *ep;
	int i;
	DEBUGP_API("delRoute: ip=0x%08X mask=0x%08X \n", ip, mask);
	
	/* Lookup */
	CTAILQ_FOREACH(ep, &table_route->list[hash], route_link) {
		if (ep->ip == ip && ep->mask == mask) {
			ep->valid = 0x00;
			CTAILQ_REMOVE(&table_route->list[hash], ep, route_link);
			CTAILQ_REMOVE(&route_list_inuse, ep, tqe_link);
			CTAILQ_INSERT_TAIL(&route_list_free, ep, tqe_link);
			//cathy, for multi-subnet
			for( i=0; i<routeIndex&& routeIndex<=8; i++ ) {
				if( (LANsub[i] == (ip&mask))  &&  (LANmask[i] == mask) ) {
					LANsub[i] = 0;
					LANmask[i] = 0;
					routeIndex--;
					break;
				}
			}
			for( ; i<routeIndex&&routeIndex<=8; i++ ) {
				LANsub[i] = LANsub[i+1];
				LANmask[i] = LANmask[i+1];
			}
//#if defined(CONFIG_PPTP) || defined(CONFIG_PPPOL2TP) || defined(CONFIG_NET_IPIP)
			{
				ip_t addr;
				addr = getNetAddrbyName(ep->ifname, ip);
				for (i=0; i<wanIndex && i<8; i++) {
					if (WANsub[i] == addr) {
						WANsub[i] = 0;
						wanIndex--;
						break;
					}
				}
				for (; i<wanIndex && i<8; i++)
					WANsub[i] = WANsub[i+1];
				for (; i<8; i++)
					WANsub[i] = 0;
			}
//#endif//end of CONFIG_PPTP || CONFIG_PPPOL2TP || CONFIG_NET_IPIP
			return LR_SUCCESS;
		}
	}
	
	return LR_NONEXIST;
}

enum LR_RESULT __SWAP 
rtl867x_addSession(__u8* ifname,
		enum SE_TYPE seType,
		__u32 sessionId,
		enum SE_FLAGS flags )
{
	return LR_SUCCESS;
}

enum LR_RESULT __SWAP 
rtl867x_delSession(__u8* ifname)
{
	return LR_SUCCESS;
}
//#ifdef CONFIG_RTL867X_PACKET_PROCESSOR
struct Arp_List_Entry *FastPath_Find_ARP_Entry(ip_t ipaddr)
{
	struct Arp_List_Entry *ep;
	__u32 hash = FastPath_Hash_ARP_Entry(ipaddr);
	
	CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
		if (ep->ip == ipaddr) {
			return ep;			
		}
	}
	return NULL;
}

struct Route_List_Entry *
rtl867x_lookupRoute(ip_t ip)		
{
	struct Route_List_Entry *ep,*default_route=NULL;
		
	/* Lookup */
	CTAILQ_FOREACH(ep, &route_list_inuse, tqe_link) 
	{
			if ((ep->valid==0xff)&&((ip & ep->mask) == (ep->ip & ep->mask)))
			{
				if(ep->mask!=0)
				{
					return ep;
				}
				else
				{
					default_route=ep;
				}
			}	

	}
	
	return default_route;
}
//#endif

enum LR_RESULT  __SWAP
fastpath_addNaptConnection(struct FP_NAPT_entry *napt, int state)
{
	struct Napt_List_Entry *ep;
	struct Napt_List_Entry *entry_napt;
	//unsigned long flags;
	__u8 *proto;
	__u32 hash;

	if( napt->protocol == IPPROTO_TCP ) {
		proto = "TCP";
	}
	else if( napt->protocol == IPPROTO_UDP ) {
		proto = "UDP";
	}
//#ifdef CONFIG_PPTP
	else if ( napt->protocol == IPPROTO_GRE ) {
		proto = "GRE";
	}
//#endif
	else {
		proto = "unknow";
	}

	hash = FastPath_Hash_NAPT_Entry(napt->intIp, napt->intPort, napt->extIp, napt->extPort, napt->remIp, napt->remPort);

#if 0
	printk("addNaptConnection1: P=%s int=%u.%u.%u.%u:%u ext=%u.%u.%u.%u:%u rem=%u.%u.%u.%u:%u F=%d (H=%u, Ha=%u, Hb=%u),state=%x\n", 
		proto, NIPQUAD(napt->intIp), napt->intPort, NIPQUAD(napt->extIp), napt->extPort, NIPQUAD(napt->remIp), 
		napt->remPort, napt->flags, hash, FastPath_Hash_PATH_Entry(napt->intIp, napt->intPort, napt->remIp, napt->remPort, napt->protocol), 
		FastPath_Hash_PATH_Entry(napt->remIp, napt->remPort, napt->extIp, napt->extPort, napt->protocol), state);
#endif
	/* Lookup */
	CTAILQ_FOREACH(ep, &table_napt->list[hash], napt_link) {
		if ((ep->protocol == napt->protocol) &&
			(ep->intIp == napt->intIp) &&
			(ep->intPort == napt->intPort) &&
			(ep->extIp == napt->extIp) &&
			(ep->extPort == napt->extPort) &&
			(ep->remIp == napt->remIp) &&
			(ep->remPort == napt->remPort)) {
			DEBUGP_SYS("addNaptConnection: ERROR - the entry already exist! \n");
#if 0			
			printk("addNaptConnection1: P=%s int=%u.%u.%u.%u:%u ext=%u.%u.%u.%u:%u rem=%u.%u.%u.%u:%u F=%d (H=%u, Ha=%u, Hb=%u),ref=%d, state=%d\n", 
				proto, NIPQUAD(napt->intIp), napt->intPort, NIPQUAD(napt->extIp), napt->extPort, NIPQUAD(napt->remIp), 
				napt->remPort, napt->flags, hash, FastPath_Hash_PATH_Entry(napt->intIp, napt->intPort, napt->remIp, napt->remPort, napt->protocol), 
				FastPath_Hash_PATH_Entry(napt->remIp, napt->remPort, napt->extIp, napt->extPort, napt->protocol), ep->refcnt, ep->state);
#endif
			if (ep->state != 1)
			{
				if (state==1) {//current is the establish conntrack.
					if (ep->state == 2) {//PATH already exist.
						ep->state = 1;
						return LR_SUCCESS;
					}
					else {
						ep->state = 1;
						entry_napt = ep;
						goto ADD_PATH;
					}
				}
				else {
					if (ep->state == 2)
						return LR_SUCCESS;
					if ((++(ep->refcnt)) >= 10) {
						ep->state = 2;
						entry_napt = ep;
#if 1						
						printk("addNaptConnection2: P=%s int=%u.%u.%u.%u:%u ext=%u.%u.%u.%u:%u rem=%u.%u.%u.%u:%u F=%d (H=%u, Ha=%u, Hb=%u)\n", 
							proto, NIPQUAD(napt->intIp), napt->intPort, NIPQUAD(napt->extIp), napt->extPort, NIPQUAD(napt->remIp), 
							napt->remPort, napt->flags, hash, FastPath_Hash_PATH_Entry(napt->intIp, napt->intPort, napt->remIp, napt->remPort, napt->protocol), 
							FastPath_Hash_PATH_Entry(napt->remIp, napt->remPort, napt->extIp, napt->extPort, napt->protocol));
#endif	
						goto ADD_PATH;
					}
				}
			}
			return LR_SUCCESS;
		}
	}

	if (state==0) {
	//	printk("only add napt entry.\n");
		if(!CTAILQ_EMPTY(&napt_list_free) && !CTAILQ_EMPTY(&path_list_free)) {
			entry_napt = CTAILQ_FIRST(&napt_list_free);
			entry_napt->protocol = napt->protocol;
			entry_napt->intIp = napt->intIp;
			entry_napt->intPort = napt->intPort;
			entry_napt->extIp = napt->extIp;
			entry_napt->extPort = napt->extPort;
			entry_napt->remIp = napt->remIp;
			entry_napt->remPort = napt->remPort;
			entry_napt->flags = napt->flags;
			entry_napt->state = 0;
			entry_napt->refcnt = 0;
			entry_napt->valid = 0xff;
            entry_napt->ct = napt->ct;
			CTAILQ_REMOVE(&napt_list_free, entry_napt, tqe_link);
			CTAILQ_INSERT_TAIL(&napt_list_inuse, entry_napt, tqe_link);
			CTAILQ_INSERT_TAIL(&table_napt->list[hash], entry_napt, napt_link);
		}
		return LR_SUCCESS;
	}

	if(!CTAILQ_EMPTY(&napt_list_free) && !CTAILQ_EMPTY(&path_list_free)) {
		entry_napt = CTAILQ_FIRST(&napt_list_free);
		entry_napt->protocol = napt->protocol;
		entry_napt->intIp = napt->intIp;
		entry_napt->intPort = napt->intPort;
		entry_napt->extIp = napt->extIp;
		entry_napt->extPort = napt->extPort;
		entry_napt->remIp = napt->remIp;
		entry_napt->remPort = napt->remPort;
		entry_napt->flags = napt->flags;
		entry_napt->state = 1;
		entry_napt->valid = 0xff;
        entry_napt->ct = napt->ct;
		CTAILQ_REMOVE(&napt_list_free, entry_napt, tqe_link);
		CTAILQ_INSERT_TAIL(&napt_list_inuse, entry_napt, tqe_link);
		CTAILQ_INSERT_TAIL(&table_napt->list[hash], entry_napt, napt_link);

		goto ADD_PATH;
	}
	else {
		DEBUGP_SYS("addNaptConnection: ERROR - Napt_list_free is empty! \n");
		return LR_FAILED;
	}

ADD_PATH:
	/* add Path Table Entry */
	if (1) {
		//__u32	hash;
		struct Path_List_Entry *entry_path;
		

		if (CTAILQ_EMPTY(&path_list_free)) {
			entry_napt->state = 0;
			return LR_SUCCESS;
		}

		/* course = 1 (Outbound) */
		hash = FastPath_Hash_PATH_Entry(napt->intIp, napt->intPort, napt->remIp, napt->remPort, napt->protocol);	//cathy
		entry_path = CTAILQ_FIRST(&path_list_free);
		entry_path->protocol	= &entry_napt->protocol;
		entry_path->in_sIp		= &entry_napt->intIp;
		entry_path->in_sPort	= &entry_napt->intPort;
		entry_path->in_dIp		= &entry_napt->remIp;
		entry_path->in_dPort	= &entry_napt->remPort;
		entry_path->out_sIp 	= &entry_napt->extIp;
		entry_path->out_sPort	= &entry_napt->extPort;
		entry_path->out_dIp 	= &entry_napt->remIp;
		entry_path->out_dPort	= &entry_napt->remPort;
		entry_path->out_ifname	= FastPath_Route(*entry_path->out_dIp);
		entry_path->arp_entry	= NULL;
//#ifdef CONFIG_NET_IPIP
		entry_path->arp_ignore	= 0;
//#endif//end of CONFIG_NET_IPIP
		entry_path->course		= 1;
		entry_path->valid		= 0xff;
		entry_path->dst 		= NULL;
		entry_path->mark		= 0;//QOS_DEFAULT_MARK;
		entry_path->type		= 0;	/* Init: Normal (Only Routing) */
        entry_path->ct = entry_napt->ct;
		entry_path->last_refresh_time  = jiffies;
		entry_path->add_into_asic_checked		= 0;
		entry_path->pps		= 0;
		if (*entry_path->in_sIp != *entry_path->out_sIp) {
			entry_path->type |= 1;	/* SNAT */
		}
		if (*entry_path->in_sPort != *entry_path->out_sPort) {
			entry_path->type |= 2;	/* SNPT */
		}
		CTAILQ_REMOVE(&path_list_free, entry_path, tqe_link);
		CTAILQ_INSERT_TAIL(&path_list_inuse, entry_path, tqe_link);
		CTAILQ_INSERT_TAIL(&table_path->list[hash], entry_path, path_link);
	
	//downstream
		if (CTAILQ_EMPTY(&path_list_free)) {
			entry_path->valid = 0x00;
			CTAILQ_REMOVE(&table_path->list[hash], entry_path, path_link);
			CTAILQ_REMOVE(&path_list_inuse, entry_path, tqe_link);
			CTAILQ_INSERT_TAIL(&path_list_free, entry_path, tqe_link);
			entry_napt->state = 0;
			return LR_SUCCESS;
		}
		/* course = 2 (Inbound) */
		hash = FastPath_Hash_PATH_Entry(napt->remIp, napt->remPort, napt->extIp, napt->extPort, napt->protocol);	//cathy
		entry_path = CTAILQ_FIRST(&path_list_free);
		entry_path->protocol	= &entry_napt->protocol;
		entry_path->in_sIp		= &entry_napt->remIp;
		entry_path->in_sPort	= &entry_napt->remPort;
		entry_path->in_dIp		= &entry_napt->extIp;
		entry_path->in_dPort	= &entry_napt->extPort;
		entry_path->out_sIp 	= &entry_napt->remIp;
		entry_path->out_sPort	= &entry_napt->remPort;
		entry_path->out_dIp 	= &entry_napt->intIp;
		entry_path->out_dPort	= &entry_napt->intPort;
		entry_path->out_ifname	= FastPath_Route(*entry_path->out_dIp);
		entry_path->arp_entry	= NULL;
//#ifdef CONFIG_NET_IPIP
		entry_path->arp_ignore	= 0;
//#endif //end of CONFIG_NET_IPIP
		entry_path->course		= 2;
		entry_path->valid		= 0xff;
		entry_path->dst 		= NULL;
		entry_path->type		= 0;	/* Init: Normal (Only Routing) */
		entry_path->mark		= 0;
        entry_path->ct = entry_napt->ct;
		entry_path->last_refresh_time  = jiffies;
		entry_path->add_into_asic_checked	= 0;
		entry_path->pps 	= 0;
		if (*entry_path->in_dIp != *entry_path->out_dIp) {
			entry_path->type |= 4;	/* DNAT */
		}
		if (*entry_path->in_dPort != *entry_path->out_dPort) {
			entry_path->type |= 8;	/* DNPT */
		}
		CTAILQ_REMOVE(&path_list_free, entry_path, tqe_link);
		CTAILQ_INSERT_TAIL(&path_list_inuse, entry_path, tqe_link);
		CTAILQ_INSERT_TAIL(&table_path->list[hash], entry_path, path_link);
	}
	
	return LR_SUCCESS;
}

enum LR_RESULT  __SWAP
fastpath_delNaptConnection(struct FP_NAPT_entry *napt)
{
	struct Napt_List_Entry *ep;
	__u8 *proto;
	__u32 hash;

	if( napt->protocol == IPPROTO_TCP ) {
		proto = "TCP";
	}
	else if( napt->protocol == IPPROTO_UDP ) {
		proto = "UDP";
	}
	else {
		proto = "unknow";
	}

	hash = FastPath_Hash_NAPT_Entry(napt->intIp, napt->intPort, napt->extIp, napt->extPort, napt->remIp, napt->remPort);


	//DEBUGP_API("delNaptConnection: P=%s int=%u.%u.%u.%u:%u ext=%u.%u.%u.%u:%u rem=%u.%u.%u.%u:%u \n",
	//	proto, NIPQUAD(napt->intIp), napt->intPort, NIPQUAD(napt->extIp), napt->extPort, NIPQUAD(napt->remIp), napt->remPort);

	/* Lookup */
	CTAILQ_FOREACH(ep, &table_napt->list[hash], napt_link) {
		if ((ep->protocol == napt->protocol) &&
			(ep->intIp == napt->intIp) &&
			(ep->intPort == napt->intPort) &&
			(ep->extIp == napt->extIp) &&
			(ep->extPort == napt->extPort) &&
			(ep->remIp == napt->remIp) &&
			(ep->remPort == napt->remPort)) {
			ep->valid = 0x00;
			CTAILQ_REMOVE(&table_napt->list[hash], ep, napt_link);
			CTAILQ_REMOVE(&napt_list_inuse, ep, tqe_link);
			CTAILQ_INSERT_TAIL(&napt_list_free, ep, tqe_link);
			
			/* del Path Table Entry */
			if (1) {
				__u32	hash;
				struct Path_List_Entry *entry_path;			
		
				/* course = 1 (Outbound) */
				hash = FastPath_Hash_PATH_Entry(napt->intIp, napt->intPort, napt->remIp, napt->remPort, napt->protocol);	//cathy
				CTAILQ_FOREACH(entry_path, &table_path->list[hash], path_link) {
					if ((entry_path->protocol == &ep->protocol) && (entry_path->course == 1)) {
						entry_path->valid = 0x00;
						if( entry_path->dst ) {
							dst_release(entry_path->dst);							
						}
						CTAILQ_REMOVE(&table_path->list[hash], entry_path, path_link);
						CTAILQ_REMOVE(&path_list_inuse, entry_path, tqe_link);
						CTAILQ_INSERT_TAIL(&path_list_free, entry_path, tqe_link);
						break;
					}
				}

				/* course = 2 (Inbound) */
				hash = FastPath_Hash_PATH_Entry(napt->remIp, napt->remPort, napt->extIp, napt->extPort, napt->protocol);	//cathy
				CTAILQ_FOREACH(entry_path, &table_path->list[hash], path_link) {
					if ((entry_path->protocol == &ep->protocol) && (entry_path->course == 2)) {
						entry_path->valid = 0x00;
						//cathy, fix dst destroyed before fastpath entry releases it
						if( entry_path->dst ) {
							dst_release(entry_path->dst);							
						}
						CTAILQ_REMOVE(&table_path->list[hash], entry_path, path_link);
						CTAILQ_REMOVE(&path_list_inuse, entry_path, tqe_link);
						CTAILQ_INSERT_TAIL(&path_list_free, entry_path, tqe_link);
						break;
					}
				}
			}
					
			return LR_SUCCESS;
		}
	}
	
	return LR_NONEXIST;
}

enum LR_RESULT	__SWAP
fastpath_updateNaptConnection(struct FP_NAPT_entry *napt, unsigned int mark, unsigned int mdscp)
{
	struct Napt_List_Entry *ep;
	__u8 *proto;
	__u32 hash;

	if( napt->protocol == IPPROTO_TCP ) {
		proto = "TCP";
	}
	else if( napt->protocol == IPPROTO_UDP ) {
		proto = "UDP";
	}
	else {
		proto = "unknow";
	}

	hash = FastPath_Hash_NAPT_Entry(napt->intIp, napt->intPort, napt->extIp, napt->extPort, napt->remIp, napt->remPort);

	/* Lookup */
	CTAILQ_FOREACH(ep, &table_napt->list[hash], napt_link) {
		if ((ep->protocol == napt->protocol) &&
			(ep->intIp == napt->intIp) &&
			(ep->intPort == napt->intPort) &&
			(ep->extIp == napt->extIp) &&
			(ep->extPort == napt->extPort) &&
			(ep->remIp == napt->remIp) &&
			(ep->remPort == napt->remPort)) {
			DEBUGP_API("updateNaptConnection: P=%s int=%u.%u.%u.%u:%u ext=%u.%u.%u.%u:%u rem=%u.%u.%u.%u:%u mark=%x\n", 
				proto, NIPQUAD(napt->intIp), napt->intPort, NIPQUAD(napt->extIp), napt->extPort, NIPQUAD(napt->remIp), napt->remPort, mark);
			
			/* update Path Table Entry */
			if (1) {
				__u32	hash;
				struct Path_List_Entry *entry_path;
				
				/* course = 1 (Outbound) */
				hash = FastPath_Hash_PATH_Entry(napt->intIp, napt->intPort, napt->remIp, napt->remPort, napt->protocol);	//cathy
				CTAILQ_FOREACH(entry_path, &table_path->list[hash], path_link) {
					if ((entry_path->protocol == &ep->protocol) && (entry_path->course == 1)) {
						entry_path->mark = mark;
						break;
					}
				}
#if 0
				/* course = 2 (Inbound) */
				hash = FastPath_Hash_PATH_Entry(napt->remIp, napt->remPort, napt->extIp, napt->extPort, napt->protocol);	//cathy
				CTAILQ_FOREACH(entry_path, &table_path->list[hash], path_link) {
					if ((entry_path->protocol == &ep->protocol) && (entry_path->course == 2)) {
						entry_path->mark = mark;
						break;
					}
				}
#endif
			}
					
			return LR_SUCCESS;
		}
	}
	
	return LR_NONEXIST;
}


/*delete concerning rules to filter chain*/
int rtl867x_clearFastPathEntry(void)
{
	struct Path_List_Entry *path_ep;
	struct Napt_List_Entry *napt_ep;
	__u32	hash;

REMOVE_PATH:
	CTAILQ_FOREACH(path_ep, &path_list_inuse, tqe_link) {
		path_ep->valid = 0;
		if (path_ep->dst)
			dst_release(path_ep->dst);
		hash = FastPath_Hash_PATH_Entry(*path_ep->in_sIp, *path_ep->in_sPort, *path_ep->in_dIp, *path_ep->in_dPort, *path_ep->protocol);
		CTAILQ_REMOVE(&table_path->list[hash], path_ep, path_link);
		CTAILQ_REMOVE(&path_list_inuse, path_ep, tqe_link);
		CTAILQ_INSERT_TAIL(&path_list_free, path_ep, tqe_link);
		goto REMOVE_PATH;
	}

REMOVE_NAPT:
	CTAILQ_FOREACH(napt_ep, &napt_list_inuse, tqe_link) {
		napt_ep->valid = 0;
		hash = FastPath_Hash_NAPT_Entry(napt_ep->intIp, napt_ep->intPort, napt_ep->extIp, napt_ep->extPort, napt_ep->remIp, napt_ep->remPort);
		CTAILQ_REMOVE(&table_napt->list[hash], napt_ep, napt_link);
		CTAILQ_REMOVE(&napt_list_inuse, napt_ep, tqe_link);
		CTAILQ_INSERT_TAIL(&napt_list_free, napt_ep, tqe_link);
		goto REMOVE_NAPT;
	}

	return 1;
}

/* ==================================================================================================== */
__u8 *
FastPath_Route(ip_t dIp)
{
	__u8 *ifname = NULL;
	__u32 mask_max = 0x0;
	struct Route_List_Entry *ep;
	
	/* Lookup */
	CTAILQ_FOREACH(ep, &route_list_inuse, tqe_link) {
		if ((ep->mask >= mask_max) && ((dIp & ep->mask) == ep->ip)) {
			ifname = &ep->ifname[0];
			mask_max = ep->mask;
		}
	}
	
	return ifname;
}


/* ==================================================================================================== */
        /* cached hardware header; allow for machine alignment needs.        */
#define HH_DATA_MOD     16
#define HH_DATA_ALIGN(__len) \
        (((__len)+(HH_DATA_MOD-1))&~(HH_DATA_MOD - 1))

__MIPS16 __IRAM_SYS_MIDDLE int FastPath_Process(void *pskb, struct iphdr *iph,struct net_bridge_port *br_port)
{
	__u32 sIp, dIp;
	__u16 sPort=0, dPort=0;
	int pppoe_proxy=0;//pppoe proxy and upstream pppoe
	void *dev=NULL;

	if (!fastpath_forward_flag) return 0;
	
	sIp = iph->saddr;
	dIp = iph->daddr;
	if(iph->frag_off & htons(0x3fff)){
		return 0;
	}

	switch (iph->protocol) {
		case IPPROTO_TCP: {
			struct tcphdr *tcph;
			__u32 hash;
			struct Path_List_Entry *entry_path;
			
			tcph = (struct tcphdr*)((__u32 *)iph + iph->ihl);
			sPort = tcph->source;
			dPort = tcph->dest;

			DEBUGP_PKT("==>> [%08X] SIP: %u.%u.%u.%u:%u  -> DIP: %u.%u.%u.%u:%u flag:0x%x<TCP>\n", 
				tcph->check,
				NIPQUAD(iph->saddr), tcph->source,
				NIPQUAD(iph->daddr), tcph->dest, 
				*(unsigned short *)((__u8 *)tcph + 12));
			
			if (tcph->fin || tcph->rst || tcph->syn) return 0;
			
			hash = FastPath_Hash_PATH_Entry(sIp, sPort, dIp, dPort, iph->protocol); //cathy
			CTAILQ_FOREACH(entry_path, &table_path->list[hash], path_link) {
				if ((*entry_path->in_sPort == sPort) && 
					(*entry_path->in_dPort == dPort) && 
					(*entry_path->in_sIp == sIp) &&
					(*entry_path->in_dIp == dIp) &&
					(*entry_path->protocol == IPPROTO_TCP)) {  //cathy
					if ((entry_path->out_ifname) && (entry_path->out_ifname[0]=='p') /*pppoe proxy*/
												  && 1 == entry_path->course) {
						//printk("%s %d pppoe xmit.\n", __func__, __LINE__);
						//dev = (void *)dev_get_by_name(&init_net,entry_path->out_ifname);
						dev = (void *)__dev_get_by_name(&init_net,entry_path->out_ifname);
						if (dev)
							pppoe_proxy = 1;
					}
					
					/* ARP Cache check */
					#if 0
					if ((entry_path->arp_entry && entry_path->arp_entry->valid && (entry_path->arp_entry->ip == *entry_path->out_dIp)) ||
						pppoe_proxy)
					#else
					if ((entry_path->arp_entry && entry_path->arp_entry->valid) || pppoe_proxy
//						#ifdef CONFIG_NET_IPIP
						|| entry_path->arp_ignore
//						#endif//end of CONFIG_NET_IPIP
						)
					#endif
					{
						if (!pppoe_proxy) {
							/* ARP Cache valid */
							if (entry_path->dst == NULL) {
								if(!fp_iproute_input(pskb, iph, entry_path->out_dIp))
									return 0;
								//entry_path->dst = pskb->dst;
								SetFPDst(pskb, &entry_path->dst);
							} else {
								//pskb->dst = entry_path->dst;
								setSkbDst(pskb, entry_path->dst);
							}
							if (isDestLo(pskb)) goto FINISH;

							FastPathHoldDst(pskb);
						}
						
						DEBUGP_PKT("FORWARD to [%s] \n", entry_path->out_ifname);
						switch(entry_path->type) {
							case 0: {	/* Only Routing */
								break;
							}				
							case 1: {	/* SNAT */
								FASTPATH_ADJUST_CHKSUM_NAT(*entry_path->out_sIp, iph->saddr, iph->check);
								FASTPATH_ADJUST_CHKSUM_NAT(*entry_path->out_sIp, iph->saddr, tcph->check);
								iph->saddr	= *entry_path->out_sIp;
								break;
							}
							case 2: /* SNPT */
							case 3: {	/* SNAPT */
								FASTPATH_ADJUST_CHKSUM_NAT(*entry_path->out_sIp, iph->saddr, iph->check);
								FASTPATH_ADJUST_CHKSUM_NAPT(*entry_path->out_sIp, iph->saddr, *entry_path->out_sPort, tcph->source, tcph->check);
								iph->saddr	= *entry_path->out_sIp;
								tcph->source		= *entry_path->out_sPort;
								break;
							}					
							case 4: {	/* DNAT */
								int org_tcp=0;
								org_tcp=tcph->check;
								FASTPATH_ADJUST_CHKSUM_NAT(*entry_path->out_dIp, iph->daddr, iph->check);
								FASTPATH_ADJUST_CHKSUM_NAT(*entry_path->out_dIp, iph->daddr, tcph->check);
								iph->daddr = *entry_path->out_dIp;
								break;
							}
							case 8: /* DNPT */
							case 12: {	/* DNAPT */ 					
								FASTPATH_ADJUST_CHKSUM_NAT(*entry_path->out_dIp, iph->daddr, iph->check);
								FASTPATH_ADJUST_CHKSUM_NAPT(*entry_path->out_dIp, iph->daddr, *entry_path->out_dPort, tcph->dest, tcph->check);
								iph->daddr = *entry_path->out_dIp;
								tcph->dest = *entry_path->out_dPort;
								break;
							}
							default: {
								FASTPATH_ADJUST_CHKSUM_NAT(*entry_path->out_sIp, iph->saddr, iph->check);
								FASTPATH_ADJUST_CHKSUM_NAT(*entry_path->out_dIp, iph->daddr, iph->check);
								FASTPATH_ADJUST_CHKSUM_NAPT(*entry_path->out_sIp, iph->saddr, *entry_path->out_sPort, tcph->source, tcph->check);
								FASTPATH_ADJUST_CHKSUM_NAPT(*entry_path->out_dIp, iph->daddr, *entry_path->out_dPort, tcph->dest, tcph->check);
								iph->saddr	= *entry_path->out_sIp;
								tcph->source = *entry_path->out_sPort;
								iph->daddr  = *entry_path->out_dIp;
								tcph->dest	= *entry_path->out_dPort;
								break;
							}
						}

                        
						if(br_port)
                        {
                            // Kevin, if the original source interface is under bridge (ex.br0) 
                            // update its aging time before the source MAC being modifeid in ip_finish_output3()                            
#if 0//krammer move this to fp_common.c for making lib clean
                            br_fdb_update(br_port->br, br_port
                                , eth_hdr(((struct sk_buff *)pskb))->h_source);
#else
							fp_br_fdb_update(br_port, pskb);
#endif
                        }

						//if (entry_path->course == 1) setQoSMark(pskb, entry_path->mark);
						entry_path->pps ++;
						if ((entry_path->pps > HEAVY_TRAFFIC) && !entry_path->add_into_asic_checked) {
							add_fastpath_to_asic(pskb, entry_path);
						}
						#ifdef CONFIG_PPPOE
						if (pppoe_proxy)
							pppoe_proxy_output(pskb, dev, entry_path->course);
						else
						#endif 
						{
							initSkbHdr(pskb);
							ip_finish_output3(pskb, entry_path);
						}
                        // Kevin,  refresh nf_conntrack timeout 
                        if( jiffies - entry_path->last_refresh_time > 20*HZ)
                        {
                            entry_path->last_refresh_time = jiffies;
							#if 1 //version 1
                            if (del_timer(&entry_path->ct->timeout)) 
                            {
    			                entry_path->ct->timeout.expires = jiffies + ip_ct_tcp_timeout_established;
    			                add_timer(&entry_path->ct->timeout);
                            }
							#endif
							#if 0 //version 2
							if(timer_pending(&entry_path->ct->timeout))
                            	mod_timer(&entry_path->ct->timeout, jiffies+ip_ct_tcp_timeout_established);
							#endif
                        } 
						
						return NET_RX_DROP;
					} else {
						/* Arp Cache update */
						struct Arp_List_Entry *ep;
						__u32 hash = FastPath_Hash_ARP_Entry(*entry_path->out_dIp);
						
						CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
							if (ep->ip == *entry_path->out_dIp) {
								entry_path->arp_entry = ep;
								goto FINISH;
							}
						}
						do {
						struct Route_List_Entry *rep;
						CTAILQ_FOREACH(rep, &route_list_inuse, tqe_link) {
							//printk("ROUTE_TABLE: ip:%x mask:%x gateway:%x\n", rep->ip, rep->mask, rep->gateway);
							//printk("entry_path out_dip:%x\n", *entry_path->out_dIp);
							if((rep->ip & rep->mask & 0xFFFFFFFF) == (*entry_path->out_dIp & rep->mask & 0xFFFFFFFF)) {
								//printk("this is the right route entry.");
								if (rep->gateway) {
									hash = FastPath_Hash_ARP_Entry(rep->gateway);
									CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
										//printk("ARP_TABLE: ip:%x gateway:%x\n", ep->ip, rep->gateway);
										if (ep->ip == rep->gateway) {
											entry_path->arp_entry = ep;
											//printk("arp entry found.\n");
											goto FINISH;
										}
									}
									break;
								}
							}
						}
						//printk("arp entry still not found.\n\n");
						} while(0);
//					#ifdef CONFIG_NET_IPIP
						if (entry_path->out_ifname) {
							dev = (void *)__dev_get_by_name(&init_net,entry_path->out_ifname);
							if (dev && isIPIPTunnelDev(dev)) {
								entry_path->arp_ignore = 1;
								//printk("arp_ignore.\n");
							}
						}
//					#endif//end of CONFIG_NET_IPIP
					}
					break;
				}
			}
			break;
		}
		case IPPROTO_UDP: {
			struct udphdr *udph;
			__u32 hash;
			struct Path_List_Entry *entry_path;
			
			udph = (struct udphdr*)((__u32 *)iph + iph->ihl);
			sPort = udph->source;
			dPort = udph->dest;
			
			DEBUGP_PKT("==>> [%08X] SIP: %u.%u.%u.%u:%u  -> DIP: %u.%u.%u.%u:%u <UDP> #0x%x\n", 
				udph->check,
				NIPQUAD(iph->saddr), udph->source, 
				NIPQUAD(iph->daddr), udph->dest, iph->frag_off);
			
			hash = FastPath_Hash_PATH_Entry(sIp, sPort, dIp, dPort, iph->protocol);  //cathy
			//printk("hash=%d\n", hash);
			CTAILQ_FOREACH(entry_path, &table_path->list[hash], path_link) {
				if ((*entry_path->in_sPort == sPort) && 
					(*entry_path->in_dPort == dPort) && 
					(*entry_path->in_sIp == sIp) &&
					(*entry_path->in_dIp == dIp) &&
					(*entry_path->protocol == IPPROTO_UDP)) {  //cathy
				//	printk("%s %d fp enter. \n", __func__, __LINE__);
					if ((entry_path->out_ifname) && (entry_path->out_ifname[0]=='p' && 1 == entry_path->course) /*pppoe proxy*/) {
						//printk("%s %d pppoe proxy packet.\n", __func__, __LINE__);
						//dev = (void *)dev_get_by_name(&init_net,entry_path->out_ifname);
						dev = (void *)__dev_get_by_name(&init_net,entry_path->out_ifname);
						if (dev)
							pppoe_proxy = 1;
					}
					/* ARP Cache check */
					if ((entry_path->arp_entry && entry_path->arp_entry->valid)||pppoe_proxy
//#ifdef CONFIG_NET_IPIP
						|| entry_path->arp_ignore
//#endif//end of CONFIG_NET_IPIP
                        ) 
				    {
						if ((!pppoe_proxy)
//							#ifdef CONFIG_PPPOL2TP
							&& !isSkbDstAssigned(pskb)
//							#endif //endof CONFIG_PPPOL2TP
							) {
							/* ARP Cache valid */
							if (entry_path->dst == NULL) {
								//if(ip_route_input(pskb, *entry_path->out_dIp, iph->saddr, iph->tos, pskb->dev))
								if(!fp_iproute_input(pskb, iph, entry_path->out_dIp))
									goto FINISH;
								//entry_path->dst = pskb->dst;
								SetFPDst(pskb, &entry_path->dst);
							} else {
								//pskb->dst = entry_path->dst;
								setSkbDst(pskb, entry_path->dst);
							}
							if (isDestLo(pskb)) goto FINISH;
							
							//pskb->dst->lastuse = jiffies;
							//dst_hold(pskb->dst); //cathy, fix dst cache full problem 					
							//pskb->dst->__use++;
							FastPathHoldDst(pskb);
						}
						
						DEBUGP_PKT("FORWARD to [%s] \n", entry_path->out_ifname);
						switch(entry_path->type) {
							case 0: {	/* Only Routing */
								break;
							}					
							case 1: {	/* SNAT */
								FASTPATH_ADJUST_CHKSUM_NAT_UDP(*entry_path->out_sIp, iph->saddr, iph->check);
								FASTPATH_ADJUST_CHKSUM_NAT_UDP(*entry_path->out_sIp, iph->saddr, udph->check);
								iph->saddr	= *entry_path->out_sIp;
								break;
							}
							case 2: /* SNPT */
							case 3: {	/* SNAPT */
								FASTPATH_ADJUST_CHKSUM_NAT_UDP(*entry_path->out_sIp, iph->saddr, iph->check);
								FASTPATH_ADJUST_CHKSUM_NAPT_UDP(*entry_path->out_sIp, iph->saddr, *entry_path->out_sPort, udph->source, udph->check);
								iph->saddr	= *entry_path->out_sIp;
								udph->source		= *entry_path->out_sPort;
								break;
							}					
							case 4: {	/* DNAT */
								FASTPATH_ADJUST_CHKSUM_NAT_UDP(*entry_path->out_dIp, iph->daddr, iph->check);
								FASTPATH_ADJUST_CHKSUM_NAT_UDP(*entry_path->out_dIp, iph->daddr, udph->check);
								iph->daddr	= *entry_path->out_dIp;
								break;
							}
							case 8: /* DNPT */
							case 12: {	/* DNAPT */
								FASTPATH_ADJUST_CHKSUM_NAT_UDP(*entry_path->out_dIp, iph->daddr, iph->check);
								FASTPATH_ADJUST_CHKSUM_NAPT_UDP(*entry_path->out_dIp, iph->daddr, *entry_path->out_dPort, udph->dest, udph->check);
								iph->daddr = *entry_path->out_dIp;
								udph->dest = *entry_path->out_dPort;
								break;
							}
							default: {
								FASTPATH_ADJUST_CHKSUM_NAT_UDP(*entry_path->out_sIp, iph->saddr, iph->check);
								FASTPATH_ADJUST_CHKSUM_NAT_UDP(*entry_path->out_dIp, iph->daddr, iph->check);
								FASTPATH_ADJUST_CHKSUM_NAPT_UDP(*entry_path->out_sIp, iph->saddr, *entry_path->out_sPort, udph->source, udph->check);
								FASTPATH_ADJUST_CHKSUM_NAPT_UDP(*entry_path->out_dIp, iph->daddr, *entry_path->out_dPort, udph->dest, udph->check);
								iph->saddr	= *entry_path->out_sIp;
								udph->source		= *entry_path->out_sPort;
								iph->daddr	= *entry_path->out_dIp;
								udph->dest			= *entry_path->out_dPort;
								break;
							}
						}
						
                        if(br_port)
                        {
                            // Kevin, if the original source interface is under bridge (ex.br0) 
                            // update its aging time before the source MAC being modifeid in ip_finish_output3()                                                                        
#if 0//krammer move this to fp_common.c for making lib clean
                            br_fdb_update(br_port->br, br_port
                                , eth_hdr(((struct sk_buff *)pskb))->h_source);
#else
							fp_br_fdb_update(br_port, pskb);
#endif
                        }
							
						//if (entry_path->course == 1) setQoSMark(pskb, entry_path->mark);
						entry_path->pps ++;
						if ((entry_path->pps > HEAVY_TRAFFIC) && !entry_path->add_into_asic_checked) {
							add_fastpath_to_asic(pskb, entry_path);
						}
						#ifdef CONFIG_PPPOE
						if (pppoe_proxy)
							pppoe_proxy_output(pskb, dev, entry_path->course);
						else
						#endif 
						{
							initSkbHdr(pskb);				
							ip_finish_output3(pskb,entry_path);
						}
						
                        // Kevin,  refresh nf_conntrack timeout                						
						if( jiffies - entry_path->last_refresh_time > 20*HZ)
                        {
                            entry_path->last_refresh_time = jiffies;
							#if 1 //version 1
                            if (del_timer(&entry_path->ct->timeout)) 
                            {
    			                entry_path->ct->timeout.expires = jiffies + ip_ct_udp_timeout_stream;
    			                add_timer(&entry_path->ct->timeout);
                            }
							#endif
							#if 0 //version 2
							if(timer_pending(&entry_path->ct->timeout))
                            	mod_timer(&entry_path->ct->timeout, jiffies+ip_ct_udp_timeout_stream);
							#endif
                        } 
						return NET_RX_DROP;
					} else {
						/* Arp Cache update */
						struct Arp_List_Entry *ep;
						//printk("%s %d out_dIP:%x\n", __func__, __LINE__, *entry_path->out_dIp);
						__u32 hash = FastPath_Hash_ARP_Entry(*entry_path->out_dIp);

						//printk("%s %d hash=%x\n", __func__, __LINE__, hash);
						CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
							//printk("%s %d ep->ip=%x out_dIP=%x\n", __func__, __LINE__, ep->ip, *entry_path->out_dIp);
							if (ep->ip == *entry_path->out_dIp) {
								entry_path->arp_entry = ep;
								goto FINISH;
							}
						}
						do {
						struct Route_List_Entry *rep;
						CTAILQ_FOREACH(rep, &route_list_inuse, tqe_link) {
							if((rep->ip & rep->mask) == (*entry_path->out_dIp & rep->mask)) {
								if (rep->gateway) {
									hash = FastPath_Hash_ARP_Entry(rep->gateway);
									CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
										if (ep->ip == rep->gateway) {
											entry_path->arp_entry = ep;
											goto FINISH;
										}
									}
									break;
								}
							}
						}
						//printk("arp entry still not found.\n\n");
						} while (0);
//#ifdef CONFIG_NET_IPIP
						if (entry_path->out_ifname) {
							dev = (void *)__dev_get_by_name(&init_net,entry_path->out_ifname);
							if (dev && isIPIPTunnelDev(dev)) {
								entry_path->arp_ignore = 1;
								//printk("arp_ignore.\n");
							}
						}
//#endif//end of CONFIG_NET_IPIP
					}
					break;
				}
			}
			//printk("no rule found.\n");
			break;
		}
//#ifdef CONFIG_PPTP
		case IPPROTO_GRE:
		{
			struct pptp_gre_header {
				__u8	flags;
				__u8	ver;
				__u16 protocol;
				__u16 payload_len;
				__u16 call_id;
				__u32 seq;
				__u32 ack;
			} *grehdr;
			__u16 callid;
			__u32 hash;
			struct Path_List_Entry *entry_path;
			
			grehdr = (struct pptp_gre_header*)((__u32 *)iph + iph->ihl);
			callid = grehdr->call_id;
			
			DEBUGP_PKT("==>> SIP: %u.%u.%u.%u  -> DIP: %u.%u.%u.%u callid:%d <GRE> #0x%x\n", 
				NIPQUAD(iph->saddr), NIPQUAD(iph->daddr), callid, iph->frag_off);
			
			hash = FastPath_Hash_PATH_Entry(sIp, callid, dIp, callid, iph->protocol);  //cathy
			//printk("hash=%d\n", hash);
			CTAILQ_FOREACH(entry_path, &table_path->list[hash], path_link) {
				//printk("entry:src %x/%d -> dst %x/%d proto:%d\n", *entry_path->in_sIp, *entry_path->in_sPort, *entry_path->in_dIp, 
				//	*entry_path->in_dPort, *entry_path->protocol);
				if ((*entry_path->in_sPort == callid) && 
					(*entry_path->in_dPort == callid) && 
					(*entry_path->in_sIp == sIp) &&
					(*entry_path->in_dIp == dIp) &&
					(*entry_path->protocol == IPPROTO_GRE)) {
					//printk("%s %d fp enter. \n", __func__, __LINE__);
					if ((entry_path->out_ifname) && (entry_path->out_ifname[0]=='p' && 1 == entry_path->course) /*pppoe proxy*/) {
						//printk("%s %d pppoe proxy packet.\n", __func__, __LINE__);
						//dev = (void *)dev_get_by_name(&init_net,entry_path->out_ifname);
						dev = (void *)__dev_get_by_name(&init_net,entry_path->out_ifname);
						if (dev)
							pppoe_proxy = 1;
					}
					/* ARP Cache check */
					if ((entry_path->arp_entry && entry_path->arp_entry->valid ) || pppoe_proxy) 
					{
						if (!pppoe_proxy) {
							/* ARP Cache valid */
							if (entry_path->dst == NULL) {
								if(!fp_iproute_output(pskb, iph))
									goto FINISH;
								//entry_path->dst = pskb->dst;
								SetFPDst(pskb, &entry_path->dst);
							} else {
								//pskb->dst = entry_path->dst;
								setSkbDst(pskb, entry_path->dst);
							}
							if (isDestLo(pskb)) goto FINISH;
							
							//pskb->dst->lastuse = jiffies;
							//dst_hold(pskb->dst); //cathy, fix dst cache full problem					
							//pskb->dst->__use++;
							FastPathHoldDst(pskb);
						}
						
						DEBUGP_PKT("FORWARD to [%s] \n", entry_path->out_ifname);
						
						if(br_port)
						{
							// Kevin, if the original source interface is under bridge (ex.br0) 
							// update its aging time before the source MAC being modifeid in ip_finish_output3()																		
#if 0//krammer move this to fp_common.c for making lib clean
							br_fdb_update(br_port->br, br_port
								, eth_hdr(((struct sk_buff *)pskb))->h_source);
#else
							fp_br_fdb_update(br_port, pskb);
#endif
						}
						
						//if (entry_path->course == 1) setQoSMark(pskb, entry_path->mark);
						entry_path->pps ++;
						if ((entry_path->pps > HEAVY_TRAFFIC) && !entry_path->add_into_asic_checked) {
							add_fastpath_to_asic(pskb, entry_path);
						}
						#ifdef CONFIG_PPPOE
						if (pppoe_proxy)
							pppoe_proxy_output(pskb, dev, entry_path->course);
						else
						#endif 
						{
							initSkbHdr(pskb);
							ip_finish_output3(pskb, entry_path);
						}
						// Kevin,  refresh nf_conntrack timeout 									
						if( jiffies - entry_path->last_refresh_time > 20*HZ)
						{
							entry_path->last_refresh_time = jiffies;
				#if 1 //version 1
							if (del_timer(&entry_path->ct->timeout)) 
							{
								entry_path->ct->timeout.expires = jiffies + ip_ct_udp_timeout_stream;
								add_timer(&entry_path->ct->timeout);
							}
				#endif
				#if 0 //version 2
							if(timer_pending(&entry_path->ct->timeout))
								mod_timer(&entry_path->ct->timeout, jiffies+ip_ct_udp_timeout_stream);
				#endif
						} 
						return NET_RX_DROP;
					} else {
						/* Arp Cache update */
						struct Arp_List_Entry *ep;
						__u32 hash = FastPath_Hash_ARP_Entry(*entry_path->out_dIp);

						//printk("%s %d out_dIP:%x\n", __func__, __LINE__, *entry_path->out_dIp);
						//printk("%s %d hash=%x\n", __func__, __LINE__, hash);
						CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
							//printk("%s %d ep->ip=%x out_dIP=%x\n", __func__, __LINE__, ep->ip, *entry_path->out_dIp);
							if (ep->ip == *entry_path->out_dIp) {
								entry_path->arp_entry = ep;
								goto FINISH;
							}
						}
						do {
						struct Route_List_Entry *rep;
						CTAILQ_FOREACH(rep, &route_list_inuse, tqe_link) {
							if((rep->ip & rep->mask) == (*entry_path->out_dIp & rep->mask)) {
								if (rep->gateway) {
									hash = FastPath_Hash_ARP_Entry(rep->gateway);
									CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
										if (ep->ip == rep->gateway) {
											entry_path->arp_entry = ep;
											goto FINISH;
										}
									}
									break;
								}
							}
						}
						//printk("arp entry still not found.\n\n");
						} while (0);
					}
					break;
				}
			}
			//printk("no rule found.\n");
			break;
		}
//#endif//end of CONFIG_PPTP
//#ifdef CONFIG_NET_IPIP
		case IPPROTO_IPIP:
		{
			__u32 hash;
			struct Path_List_Entry *entry_path;
			
			DEBUGP_PKT("==>> SIP: %u.%u.%u.%u  -> DIP: %u.%u.%u.%u <IPIP> #0x%x\n", 
				NIPQUAD(iph->saddr), NIPQUAD(iph->daddr), iph->frag_off);
			
			hash = FastPath_Hash_PATH_Entry(sIp, sPort, dIp, dPort, iph->protocol);  //cathy
			//printk("hash=%d\n", hash);
			CTAILQ_FOREACH(entry_path, &table_path->list[hash], path_link) {
				if ((*entry_path->in_sIp == sIp) &&
					(*entry_path->in_dIp == dIp) &&
					(*entry_path->protocol == IPPROTO_IPIP)) {  //cathy
					if ((entry_path->out_ifname) && (entry_path->out_ifname[0]=='p') /*pppoe proxy*/ && 1 == entry_path->course) {
						dev = (void *)__dev_get_by_name(&init_net,entry_path->out_ifname);
						if (dev)
							pppoe_proxy = 1;
					}
					/* ARP Cache check */
					if ((entry_path->arp_entry 
                        && entry_path->arp_entry->valid                   
                        )||pppoe_proxy) 
				    {
						if (!pppoe_proxy) {
							/* ARP Cache valid */
							void *dst = getSkbDst(pskb);
							
							if (entry_path->dst == NULL) {
								if (!fp_iproute_output(pskb, iph))
									goto FINISH;
								//entry_path->dst = pskb->dst;
								SetFPDst(pskb, &entry_path->dst);
							} else {
								//pskb->dst = entry_path->dst;
								setSkbDst(pskb, entry_path->dst);
							}
							if (isDestLo(pskb)) goto FINISH;
							
							//pskb->dst->lastuse = jiffies;
							//dst_hold(pskb->dst); //cathy, fix dst cache full problem 					
							//pskb->dst->__use++;
							FastPathHoldDst(pskb);
							
							if (ipip_sanity_check(pskb, dst) == 0) goto FINISH;
						}
						
						DEBUGP_PKT("FORWARD to [%s] \n", entry_path->out_ifname);
						
                        if(br_port)
                        {
                            // Kevin, if the original source interface is under bridge (ex.br0) 
                            // update its aging time before the source MAC being modifeid in ip_finish_output3()                                                                        
#if 0//krammer move this to fp_common.c for making lib clean
                            br_fdb_update(br_port->br, br_port
                                , eth_hdr(((struct sk_buff *)pskb))->h_source);
#else
							fp_br_fdb_update(br_port, pskb);
#endif
                        }
						entry_path->pps ++;
						if ((entry_path->pps > HEAVY_TRAFFIC) && !entry_path->add_into_asic_checked) {
							add_fastpath_to_asic(pskb, entry_path);
						}
						#ifdef CONFIG_PPPOE
						if (pppoe_proxy)
							pppoe_proxy_output(pskb, dev, entry_path->course);
						else
						#endif 
						{
							initSkbHdr(pskb);
							ip_finish_output3(pskb, entry_path);
						}
                        // Kevin,  refresh nf_conntrack timeout                						
						if( jiffies - entry_path->last_refresh_time > 20*HZ)
                        {
                            entry_path->last_refresh_time = jiffies;
							#if 1 //version 1
                            if (del_timer(&entry_path->ct->timeout)) 
                            {
    			                entry_path->ct->timeout.expires = jiffies + ip_ct_udp_timeout_stream;
    			                add_timer(&entry_path->ct->timeout);
                            }
							#endif
							#if 0 //version 2
							if(timer_pending(&entry_path->ct->timeout))
                            	mod_timer(&entry_path->ct->timeout, jiffies+ip_ct_udp_timeout_stream);
							#endif
                        } 
						return NET_RX_DROP;
					} else {
						/* Arp Cache update */
						struct Arp_List_Entry *ep;
						__u32 hash = FastPath_Hash_ARP_Entry(*entry_path->out_dIp);

						//printk("%s %d out_dIP:%x\n", __func__, __LINE__, *entry_path->out_dIp);
						//printk("%s %d hash=%x\n", __func__, __LINE__, hash);
						CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
							//printk("%s %d ep->ip=%x out_dIP=%x\n", __func__, __LINE__, ep->ip, *entry_path->out_dIp);
							if (ep->ip == *entry_path->out_dIp) {
								entry_path->arp_entry = ep;
								goto FINISH;
							}
						}
						do {
						struct Route_List_Entry *rep;
						CTAILQ_FOREACH(rep, &route_list_inuse, tqe_link) {
							if((rep->ip & rep->mask) == (*entry_path->out_dIp & rep->mask)) {
								if (rep->gateway) {
									hash = FastPath_Hash_ARP_Entry(rep->gateway);
									CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
										if (ep->ip == rep->gateway) {
											entry_path->arp_entry = ep;
											goto FINISH;
										}
									}
									break;
								}
							}
						}
						//printk("arp entry still not found.\n\n");
						} while (0);
					}
					break;
				}
			}
			//printk("no rule found.\n");
			break;
		}
//#endif//end of CONFIG_NET_IPIP
		case IPPROTO_ICMP:{  //cathy
			break;	
		}
		default: {	//cathy
			__u32 hash;
			struct Path_List_Entry *entry_path;

			hash = FastPath_Hash_PATH_Entry(sIp, sPort, dIp, dPort, iph->protocol);
			CTAILQ_FOREACH(entry_path, &table_path->list[hash], path_link) {
				if ((*entry_path->protocol == iph->protocol) && 
					(*entry_path->in_sIp == sIp) &&
					(*entry_path->in_dIp == dIp) ) {
					if ((entry_path->out_ifname) && (entry_path->out_ifname[0]=='p' && 1 == entry_path->course) /*pppoe proxy*/) {
						//printk("%s %d pppoe proxy packet.\n", __func__, __LINE__);
						//dev = (void *)dev_get_by_name(&init_net,entry_path->out_ifname);
						dev = (void *)__dev_get_by_name(&init_net,entry_path->out_ifname);
						if (dev)
							pppoe_proxy = 1;
					}
					/* ARP Cache check */
					if ((entry_path->arp_entry 
                        && entry_path->arp_entry->valid                 
                        )||	pppoe_proxy) {
						if (!pppoe_proxy) {
							/* ARP Cache valid */
							if (entry_path->dst == NULL) {
								//if(ip_route_input(pskb, *entry_path->out_dIp, iph->saddr, iph->tos, pskb->dev))
								if(!fp_iproute_input(pskb, iph, entry_path->out_dIp))
									return 0;
								//entry_path->dst = pskb->dst;
								SetFPDst(pskb, &entry_path->dst);
							} else {
								//pskb->dst = entry_path->dst;
								setSkbDst(pskb, entry_path->dst);
							}
							if (isDestLo(pskb)) goto FINISH;
							
							//pskb->dst->lastuse = jiffies;
							//dst_hold(pskb->dst); //cathy, fix dst cache full problem 					
							//pskb->dst->__use++;
							FastPathHoldDst(pskb);
						}
						
						DEBUGP_PKT("FORWARD to [%s] \n", entry_path->out_ifname);
						switch(entry_path->type) {
						case 0: {	/* Only Routing */
							break;
						}					
						case 4: {	/* DNAT */
							FASTPATH_ADJUST_CHKSUM_NAT(*entry_path->out_dIp, iph->daddr, iph->check);
							//FASTPATH_ADJUST_CHKSUM_NAT(*entry_path->out_dIp, pskb->nh.iph->daddr, udph->check);
							iph->daddr	= *entry_path->out_dIp;
							break;
						}
						default: {
							FASTPATH_ADJUST_CHKSUM_NAT(*entry_path->out_sIp, iph->saddr, iph->check);
							FASTPATH_ADJUST_CHKSUM_NAT(*entry_path->out_dIp, iph->daddr, iph->check);
							//FASTPATH_ADJUST_CHKSUM_NAPT(*entry_path->out_sIp, pskb->nh.iph->saddr, *entry_path->out_sPort, udph->source, udph->check);
							//FASTPATH_ADJUST_CHKSUM_NAPT(*entry_path->out_dIp, pskb->nh.iph->daddr, *entry_path->out_dPort, udph->dest, udph->check);
							iph->saddr	= *entry_path->out_sIp;
							//udph->source		= *entry_path->out_sPort;
							iph->daddr	= *entry_path->out_dIp;
							//udph->dest			= *entry_path->out_dPort;
							break;
						}
						}
						
						//pskb->ip_summed = 0x0;
						//pskb->dst->output(pskb);	/* ip_output() */
						//pskb->dev = pskb->dst->dev;

                        if(br_port)
                        {
                            // Kevin, if the original source interface is under bridge (ex.br0) 
                            // update its aging time before the source MAC being modifeid in ip_finish_output3()                                                                        
#if 0//krammer move this to fp_common.c for making lib clean
                            br_fdb_update(br_port->br, br_port
                                , eth_hdr(((struct sk_buff *)pskb))->h_source);
#else
							fp_br_fdb_update(br_port, pskb);
#endif
                        }
                        
						//if (entry_path->course == 1) setQoSMark(pskb, entry_path->mark);
						entry_path->pps ++;
						if ((entry_path->pps > HEAVY_TRAFFIC) && !entry_path->add_into_asic_checked) {
							add_fastpath_to_asic(pskb, entry_path);
						}
						#ifdef CONFIG_PPPOE
						if (pppoe_proxy)
							pppoe_proxy_output(pskb, dev, entry_path->course);
						else
						#endif 
						{
							initSkbHdr(pskb);
							ip_finish_output3(pskb, entry_path);
						}
                        // Kevin,  refresh nf_conntrack timeout 
                        if( jiffies - entry_path->last_refresh_time > 20*HZ)
                        {
                            entry_path->last_refresh_time = jiffies;
							#if 1 //version 1
                            if (del_timer(&entry_path->ct->timeout)) 
                            {
    			                entry_path->ct->timeout.expires = jiffies + ip_ct_udp_timeout_stream;
    			                add_timer(&entry_path->ct->timeout);
                            }
							#endif
							#if 0 //version 2
							if(timer_pending(&entry_path->ct->timeout))
                            	mod_timer(&entry_path->ct->timeout, jiffies+ip_ct_udp_timeout_stream);
							#endif
                        } 

						return NET_RX_DROP;
					} else {
						/* Arp Cache update */
						struct Arp_List_Entry *ep;
						__u32 hash = FastPath_Hash_ARP_Entry(*entry_path->out_dIp);
						
						CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
							if (ep->ip == *entry_path->out_dIp) {
								entry_path->arp_entry = ep;
								goto FINISH;
							}
						}
						do {
						struct Route_List_Entry *rep;
						CTAILQ_FOREACH(rep, &route_list_inuse, tqe_link) {
							if((rep->ip & rep->mask) == (*entry_path->out_dIp & rep->mask)) {
								if (rep->gateway) {
									hash = FastPath_Hash_ARP_Entry(rep->gateway);
									CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
										if (ep->ip == rep->gateway) {
											entry_path->arp_entry = ep;
											goto FINISH;
										}
									}
									break;
								}
							}
						}
						} while (0);
					}
					break;
				}
			}
			break;
		}
	}

FINISH:
	//if (dev)
	//	dev_put(dev);
	return 0;
}

#ifdef	DEBUG_PROCFILE
/*
static int fastpath_forward_proc(char *buffer, char **start, off_t offset, int length)
{
	int len=0;
	len += sprnitf(buffer + len, "%d\n", fastpath_forward_flag);
	return len;
}
*/
#if 0
static int __SWAP fastpath_table_arp(char *buffer, char **start, off_t offset, int length)
{
	struct Arp_List_Entry *ep;
	int len=0;
	
	CTAILQ_FOREACH(ep, &arp_list_inuse, tqe_link) {
		len += sprintf(buffer + len, "~Arp: ip=0x%08X mac=%02X:%02X:%02X:%02X:%02X:%02X flags=0x%08X \n", ep->ip, MAC2STR(ep->mac), ep->flags);
	}
	
	return len;
}
#endif

static int fastpath_table_arp_show(struct seq_file *m, void *v)
{
	struct Arp_List_Entry *ep;
	
	CTAILQ_FOREACH(ep, &arp_list_inuse, tqe_link) {
		seq_printf(m, "~Arp: ip=0x%08X mac=%02X:%02X:%02X:%02X:%02X:%02X flags=0x%08X \n", ep->ip, MAC2STR(ep->mac), ep->flags);
	}

	return 0;
}

static int fastpath_table_arp_open(struct inode *inode, struct file *file)
{
	return single_open(file, fastpath_table_arp_show, NULL);
}

static const struct file_operations fastpath_table_arp_proc_fops = {
	.open		= fastpath_table_arp_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.owner		= THIS_MODULE,
};



#if 0
static int __SWAP fastpath_table_route(char *buffer, char **start, off_t offset, int length)
{
	struct Route_List_Entry *ep;
	int len=0;
	
	CTAILQ_FOREACH(ep, &route_list_inuse, tqe_link) {
		len += sprintf(buffer + len, "~Route: ip=0x%08X mask=0x%08X gateway=0x%08X ifname=%-5s flags=0x%08X \n", 
			ep->ip, ep->mask, ep->gateway, ep->ifname, ep->flags);
	}
	
	return len;
}
#endif
static int fastpath_table_route_show(struct seq_file *m, void *v)
{
	struct Route_List_Entry *ep;
	
	CTAILQ_FOREACH(ep, &route_list_inuse, tqe_link) {
		#if 0
		seq_printf(m, "~Route: ip=0x%08X mask=0x%08X gateway=0x%08X ifname=%-5s flags=0x%08X \n", 
			ep->ip, ep->mask, ep->gateway, ep->ifname, ep->flags);
		#else
		printk("~Route: ip=0x%08X mask=0x%08X gateway=0x%08X ifname=%-5s flags=0x%08X \n", 
			ep->ip, ep->mask, ep->gateway, ep->ifname, ep->flags);
		#endif
	}

	return 0;
}

static int fastpath_table_route_open(struct inode *inode, struct file *file)
{
	return single_open(file, fastpath_table_route_show, NULL);
}

static const struct file_operations fastpath_table_route_proc_fops = {
	.open		= fastpath_table_route_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.owner		= THIS_MODULE,
};



#if 0
static int __SWAP fastpath_table_napt(char *buffer, char **start, off_t offset, int length)
{
	struct Napt_List_Entry *ep;
	unsigned int len=0, newlen=0;
	//cathy
	__u8 *proto;
	off_t upto = 0;	
	CTAILQ_FOREACH(ep, &napt_list_inuse, tqe_link) {
		if( upto++ < offset ) 
			continue;
		if( ep->protocol  == IPPROTO_TCP ) {
			proto = "TCP";
		}
		else if( ep->protocol  == IPPROTO_UDP ) {
			proto = "UDP";
		}
		else {
			proto = "unknow";
		}
		newlen = sprintf(buffer + len, "~Napt: [%s] int=0x%08X:%-5u ext=0x%08X:%-5u rem=0x%08X:%-5u flags=0x%08X \n", 
			proto,
			ep->intIp, ep->intPort, ep->extIp, ep->extPort, ep->remIp, ep->remPort,
			ep->flags);
		if ( (len + newlen) > length) {
			goto finished;			
		}
		else {
			len += newlen;
		}
	}
finished:
	/* `start' hack - see fs/proc/generic.c line ~165 */
	*start = (char *)((unsigned int)upto - offset);	
	return len;
}
#endif
static int fastpath_table_napt_show(struct seq_file *m, void *v)
{
	struct Napt_List_Entry *ep;
	//cathy
	__u8 *proto;
	CTAILQ_FOREACH(ep, &napt_list_inuse, tqe_link) {
		if( ep->protocol  == IPPROTO_TCP ) {
			proto = "TCP";
		}
		else if( ep->protocol  == IPPROTO_UDP ) {
			proto = "UDP";
		}
		else {
			proto = "unknow";
		}
		seq_printf(m, "~Napt: [%s] int=%u.%u.%u.%u:%u  ext=%u.%u.%u.%u:%u  rem=%u.%u.%u.%u:%u  flags=0x%08X state=%d \n", 
			proto,
			NIPQUAD(ep->intIp), ep->intPort, NIPQUAD(ep->extIp), ep->extPort, NIPQUAD(ep->remIp), ep->remPort,
			ep->flags,ep->state);
	}

	return 0;
}

static int fastpath_table_napt_open(struct inode *inode, struct file *file)
{
	return single_open(file, fastpath_table_napt_show, NULL);
}

static const struct file_operations fastpath_table_napt_proc_fops = {
	.open		= fastpath_table_napt_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.owner		= THIS_MODULE,
};



#if 0
static int __SWAP fastpath_table_path(char *buffer, char **start, off_t offset, int length)
{
	struct Path_List_Entry *ep;
	unsigned int len=0, newlen=0;
	//cathy
	__u8 *proto;
	off_t upto = 0;	
	CTAILQ_FOREACH(ep, &path_list_inuse, tqe_link) {
		if( upto++ < offset ) 
			continue;
		if( *ep->protocol  == IPPROTO_TCP ) {
			proto = "TCP";
		}
		else if( *ep->protocol  == IPPROTO_UDP ) {
			proto = "UDP";
		}
		else {
			proto = "unknow";
		}
		newlen = sprintf(buffer + len, "~Path: [%s] in-S=0x%08X:%-5u in-D=0x%08X:%-5u out-S=0x%08X:%-5u out-D=0x%08X:%-5u out-ifname=%-5s <%u> {%d}\n", 
			proto,
			*ep->in_sIp, *ep->in_sPort, *ep->in_dIp, *ep->in_dPort,
			*ep->out_sIp, *ep->out_sPort, *ep->out_dIp, *ep->out_dPort,
			ep->out_ifname, ep->course, ep->type);
		if ( (len + newlen) > length) {
			goto finished;			
		}
		else {
			len += newlen;
		}
	}
finished:
	/* `start' hack - see fs/proc/generic.c line ~165 */
	*start = (char *)((unsigned int)upto - offset);		
	return len;
}
#endif
static int fastpath_table_path_show(struct seq_file *m, void *v)
{
	struct Path_List_Entry *ep;
	//cathy
	__u8 *proto;
	CTAILQ_FOREACH(ep, &path_list_inuse, tqe_link) {
		if( *ep->protocol  == IPPROTO_TCP ) {
			proto = "TCP";
		}
		else if( *ep->protocol  == IPPROTO_UDP ) {
			proto = "UDP";
		}
//#ifdef CONFIG_PPTP
		else if( *ep->protocol  == IPPROTO_GRE ) {
			proto = "GRE";
		}
//#endif//end of CONFIG_PPTP
//#ifdef CONFIG_NET_IPIP
		else if( *ep->protocol  == IPPROTO_IPIP ) {
			proto = "IPIP";
		}
//#endif//end of CONFIG_NET_IPIP
		else {
			proto = "unknow";
		}
		seq_printf(m, "~Path: [%s] in-S=%u.%u.%u.%u:%u in-D=%u.%u.%u.%u:%u out-S=%u.%u.%u.%u:%u out-D=%u.%u.%u.%u:%u out-ifname=%-5s <%u> {%d}\n"
			"pps:%u"
			,
			proto,
			NIPQUAD(*ep->in_sIp), *ep->in_sPort, NIPQUAD(*ep->in_dIp), *ep->in_dPort,
			NIPQUAD(*ep->out_sIp), *ep->out_sPort, NIPQUAD(*ep->out_dIp), *ep->out_dPort,
			ep->out_ifname, ep->course, ep->type 
			,ep->pps
		);
	}

	return 0;
}

static int fastpath_table_path_open(struct inode *inode, struct file *file)
{
	return single_open(file, fastpath_table_path_show, NULL);
}

static const struct file_operations fastpath_table_path_proc_fops = {
	.open		= fastpath_table_path_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.owner		= THIS_MODULE,
};



#if 0
static int __SWAP fastpath_hash_path(char *buffer, char **start, off_t offset, int length)
{
	int i, len=0;
	
	for (i=0; i<PATH_TABLE_LIST_MAX; i++) {
		len += sprintf(buffer + len, "%5d ", CTAILQ_TOTAL(&table_path->list[i]));
		if (i%12 == 11) len += sprintf(buffer + len, "\n");
	}
	len += sprintf(buffer + len, "\n");	
	
	return len;
}
#endif
static int fastpath_hash_path_show(struct seq_file *m, void *v)
{
	int i;
	
	for (i=0; i<PATH_TABLE_LIST_MAX; i++) {
		seq_printf(m, "%5d ", CTAILQ_TOTAL(&table_path->list[i]));
		if (i%12 == 11) seq_printf(m, "\n");
	}
	seq_printf(m, "\n");	

	return 0;
}

static int fastpath_hash_path_open(struct inode *inode, struct file *file)
{
	return single_open(file, fastpath_hash_path_show, NULL);
}

static const struct file_operations fastpath_hash_path_proc_fops = {
	.open		= fastpath_hash_path_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.owner		= THIS_MODULE,
};
#endif	/* DEBUG_PROCFILE */

static struct proc_dir_entry *FP_Proc_File;
#define PROCFS_NAME 		"FastPath"
#define REALTEK_FASTPATH_VERSION  "Realtek FastPath-betaV1.00"       
#include <asm/uaccess.h>
int fp_proc_read(struct file *file, const char *buffer,
                      unsigned long count, void *data)
{
	//extern void Route_Iterate();

	#ifdef CONFIG_DSL_CODESWAP
	printk("admin:%d op:%d\n", fp_admin, fp_on);
	#else
    if(fp_on==2)
		printk("%s fastpath ON!(both up and downstream)\n",REALTEK_FASTPATH_VERSION);
	if(fp_on==1)
		printk("%s fastpath ON!(only downstream)\n",REALTEK_FASTPATH_VERSION);
	if(fp_on==0)
		printk("%s fastpath OFF!\n",REALTEK_FASTPATH_VERSION);
	#endif
	
	
	return -1;
}
#ifdef CONFIG_RTL867X_KERNEL_MIPS16_NET
__NOMIPS16
#endif
int fp_proc_write(struct file *file, const char *buffer,
                      unsigned long count, void *data)
{
	char proc_buffer[count];
	
	/* write data to the buffer */
	memset(proc_buffer, 0, sizeof(proc_buffer));
	if ( copy_from_user(proc_buffer, buffer, count) ) {
		return -EFAULT;
	}

	//DEBUGP_API("Old_State:%d ", fp_state);
	switch(proc_buffer[0]) {
	#ifdef CONFIG_DSL_CODESWAP
	case '0':
		fastpath_notify(EVT_ADMIN_DOWN);
		break;
	case '1':
		fastpath_notify(EVT_ADMIN_UP);
		break;
	/*
	case '2':
		fastpath_notify(EVT_SAR_UP);
		break;
	case '3':
		fastpath_notify(EVT_SAR_DOWN);
		break;
	*/
#else
	case '0': fp_on = 0;break;
	case '1':	fp_on = 1;break;
    case '2':	fp_on = 2;break;
#endif
	default:
		printk("Error setting!\n");
	}
	//DEBUGP_API("New_State:%d\n", fp_state);

	return -1;
}





#ifdef CONFIG_DSL_CODESWAP

//static int xxxx __attribute__((section (".DSPShowtime.data")));
static struct Arp_Table table_arp_0 __SWAP_DATA;
static struct Arp_List_Entry entry_arp_array[ARP_TABLE_ENTRY_MAX] __SWAP_DATA;
static struct Route_Table table_route_0 __SWAP_DATA;
static struct Route_List_Entry entry_route_array[ROUTE_TABLE_ENTRY_MAX] __SWAP_DATA;
static struct Napt_Table table_napt_0 __SWAP_DATA;
static struct Napt_List_Entry entry_napt_array[NAPT_TABLE_ENTRY_MAX] __SWAP_DATA;
static struct Path_Table table_path_0 __SWAP_DATA;
static struct Path_List_Entry entry_path_array[PATH_TABLE_ENTRY_MAX] __SWAP_DATA;
#endif
static int __SWAP fastpath_memory_init(void) 
{
	int i;	
	#ifdef CONFIG_DSL_CODESWAP
	/* Arp-Table Init */
	table_arp = &table_arp_0;
	CTAILQ_INIT(&arp_list_inuse);
	CTAILQ_INIT(&arp_list_free);
	for (i=0; i<ARP_TABLE_LIST_MAX; i++) {
		CTAILQ_INIT(&table_arp->list[i]);
	}
	/* Arp-List Init */
	for (i=0; i<ARP_TABLE_ENTRY_MAX; i++) {
		struct Arp_List_Entry *entry_arp;
		entry_arp = &entry_arp_array[i];
		CTAILQ_INSERT_TAIL(&arp_list_free, entry_arp, tqe_link);
	}
	
	/* Route-Table Init */
	table_route = &table_route_0;
	CTAILQ_INIT(&route_list_inuse);
	CTAILQ_INIT(&route_list_free);
	for (i=0; i<ROUTE_TABLE_LIST_MAX; i++) {
		CTAILQ_INIT(&table_route->list[i]);
	}
	/* Route-List Init */
	for (i=0; i<ROUTE_TABLE_ENTRY_MAX; i++) {
		struct Route_List_Entry *entry_route;
		entry_route = &entry_route_array[i];
		CTAILQ_INSERT_TAIL(&route_list_free, entry_route, tqe_link);
	}
	
	/* Napt-Table Init */
	table_napt = &table_napt_0;
	CTAILQ_INIT(&napt_list_inuse);
	CTAILQ_INIT(&napt_list_free);
	for (i=0; i<NAPT_TABLE_LIST_MAX; i++) {
		CTAILQ_INIT(&table_napt->list[i]);
	}
	/* Napt-List Init */
	for (i=0; i<NAPT_TABLE_ENTRY_MAX; i++) {
		struct Napt_List_Entry *entry_napt;
		entry_napt = &entry_napt_array[i];
		/*struct Napt_List_Entry *entry_napt = (struct Napt_List_Entry *)kmalloc(sizeof(struct Napt_List_Entry), GFP_ATOMIC);
		if (entry_napt == NULL) {
			DEBUGP_SYS("MALLOC Failed! (Napt Table Entry) \n");
			return -2;
		}*/
		CTAILQ_INSERT_TAIL(&napt_list_free, entry_napt, tqe_link);
	}
	
	/* Path-Table Init */
	table_path = &table_path_0;
	CTAILQ_INIT(&path_list_inuse);
	CTAILQ_INIT(&path_list_free);
	for (i=0; i<PATH_TABLE_LIST_MAX; i++) {
		CTAILQ_INIT(&table_path->list[i]);
	}
	/* Path-List Init */
	for (i=0; i<PATH_TABLE_ENTRY_MAX; i++) {
		struct Path_List_Entry *entry_path;
		entry_path = &entry_path_array[i];
		CTAILQ_INSERT_TAIL(&path_list_free, entry_path, tqe_link);
	}

	return 0;
	#else
	/* Arp-Table Init */
	table_arp = (struct Arp_Table *)kmalloc(sizeof(struct Arp_Table), GFP_ATOMIC);
	if (table_arp == NULL) {
		DEBUGP_SYS("MALLOC Failed! (Arp Table) \n");
		return -1;
	}
	CTAILQ_INIT(&arp_list_inuse);
	CTAILQ_INIT(&arp_list_free);
	for (i=0; i<ARP_TABLE_LIST_MAX; i++) {
		CTAILQ_INIT(&table_arp->list[i]);
	}
	/* Arp-List Init */
	for (i=0; i<ARP_TABLE_ENTRY_MAX; i++) {
		struct Arp_List_Entry *entry_arp = (struct Arp_List_Entry *)kmalloc(sizeof(struct Arp_List_Entry), GFP_ATOMIC);
		if (entry_arp == NULL) {
			DEBUGP_SYS("MALLOC Failed! (Arp Table Entry) \n");
			return -2;
		}
		CTAILQ_INSERT_TAIL(&arp_list_free, entry_arp, tqe_link);
	}
	
	/* Route-Table Init */
	table_route = (struct Route_Table *)kmalloc(sizeof(struct Route_Table), GFP_ATOMIC);
	if (table_route == NULL) {
		DEBUGP_SYS("MALLOC Failed! (Route Table) \n");
		return -1;
	}
	CTAILQ_INIT(&route_list_inuse);
	CTAILQ_INIT(&route_list_free);
	for (i=0; i<ROUTE_TABLE_LIST_MAX; i++) {
		CTAILQ_INIT(&table_route->list[i]);
	}
	/* Route-List Init */
	for (i=0; i<ROUTE_TABLE_ENTRY_MAX; i++) {
		struct Route_List_Entry *entry_route = (struct Route_List_Entry *)kmalloc(sizeof(struct Route_List_Entry), GFP_ATOMIC);
		if (entry_route == NULL) {
			DEBUGP_SYS("MALLOC Failed! (Route Table Entry) \n");
			return -2;
		}
		CTAILQ_INSERT_TAIL(&route_list_free, entry_route, tqe_link);
	}
	
	/* Napt-Table Init */
	table_napt = (struct Napt_Table *)kmalloc(sizeof(struct Napt_Table), GFP_ATOMIC);
	if (table_napt == NULL) {
		DEBUGP_SYS("MALLOC Failed! (Napt Table) \n");
		return -1;
	}
	CTAILQ_INIT(&napt_list_inuse);
	CTAILQ_INIT(&napt_list_free);
	for (i=0; i<NAPT_TABLE_LIST_MAX; i++) {
		CTAILQ_INIT(&table_napt->list[i]);
	}
	/* Napt-List Init */
	for (i=0; i<NAPT_TABLE_ENTRY_MAX; i++) {
		struct Napt_List_Entry *entry_napt = (struct Napt_List_Entry *)kmalloc(sizeof(struct Napt_List_Entry), GFP_ATOMIC);
		if (entry_napt == NULL) {
			DEBUGP_SYS("MALLOC Failed! (Napt Table Entry) \n");
			return -2;
		}
		CTAILQ_INSERT_TAIL(&napt_list_free, entry_napt, tqe_link);
	}
	
	/* Path-Table Init */
	table_path = (struct Path_Table *)kmalloc(sizeof(struct Path_Table), GFP_ATOMIC);
	if (table_path == NULL) {
		DEBUGP_SYS("MALLOC Failed! (Path Table) \n");
		return -1;
	}
	CTAILQ_INIT(&path_list_inuse);
	CTAILQ_INIT(&path_list_free);
	for (i=0; i<PATH_TABLE_LIST_MAX; i++) {
		CTAILQ_INIT(&table_path->list[i]);
	}
	/* Path-List Init */
	for (i=0; i<PATH_TABLE_ENTRY_MAX; i++) {
		struct Path_List_Entry *entry_path = (struct Path_List_Entry *)kmalloc(sizeof(struct Path_List_Entry), GFP_ATOMIC);
		if (entry_path == NULL) {
			DEBUGP_SYS("MALLOC Failed! (Path Table Entry) \n");
			return -2;
		}
		CTAILQ_INSERT_TAIL(&path_list_free, entry_path, tqe_link);
	}

	return 0;
	#endif
}


#ifdef CONFIG_DSL_CODESWAP

static void __SWAP
fastpath_swap_start(void) {
	extern void ARP_Fastpath_Init(void);
	extern void Route_Fastpath_Init(void);
	extern void IPCT_Fastpath_Init(void);

	fastpath_memory_init();

	ARP_Fastpath_Init();

	Route_Fastpath_Init();

	IPCT_Fastpath_Init();
}
// called by lower layer (ie. SAR) to enable FP, return OP status. (0 down, 1 up)
void fastpath_notify(int event) {
	DEBUGP_API("Event:%d ", event);
	switch (fp_state) {
	case ST_INIT:
		switch (event) {
		case EVT_SAR_UP:
			fp_on = 1;
			fastpath_swap_start();
			fp_state = ST_OPERATIONAL;
			break;
		case EVT_ADMIN_DOWN:
			fp_on = fp_admin = 0;
			fp_state = ST_ADMIN_DOWN;
			break;
		
		}
		break;
		
	case ST_OPERATIONAL:
		switch (event) {

		case EVT_SAR_DOWN:	
			fp_on = 0;
			fp_state = ST_INIT;
			break;
		case EVT_ADMIN_DOWN:
			fp_on = fp_admin = 0;
			fp_state = ST_ADMIN_DOWN;
			break;
		
		}
		break;
	case ST_ADMIN_DOWN:
		switch (event) {

		case EVT_ADMIN_UP:
			fp_admin = 1;
			if (DSPInShowtime) {				
				fp_on = 1;
				fp_state = ST_OPERATIONAL;
				fastpath_swap_start();
			} else {
				fp_state = ST_INIT;
			}
			break;
		
		}
		break;
	}
	
}
#endif


static int __init fastpath_init(void)
{
#ifdef	DEBUG_PROCFILE
	/* proc file for debug */
	proc_create("fp_arp", 0, init_net.proc_net, &fastpath_table_arp_proc_fops);
	proc_create("fp_route", 0, init_net.proc_net, &fastpath_table_route_proc_fops);
	proc_create("fp_napt", 0, init_net.proc_net, &fastpath_table_napt_proc_fops);
	proc_create("fp_path", 0, init_net.proc_net, &fastpath_table_path_proc_fops);
	proc_create("fp_hash_path", 0, init_net.proc_net, &fastpath_hash_path_proc_fops);
#endif	/* DEBUG_PROCFILE */
	
	
	
	printk("%s %s\n",MODULE_NAME, MODULE_VERSION_FP);
	
	//create proc
	FP_Proc_File= create_proc_entry(PROCFS_NAME, 0644, NULL);
	if (FP_Proc_File == NULL) {
		remove_proc_entry(PROCFS_NAME, NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
			PROCFS_NAME);
		return -ENOMEM;
	}

	FP_Proc_File->read_proc  = (read_proc_t *)fp_proc_read;
	FP_Proc_File->write_proc  = (write_proc_t *)fp_proc_write;
	FP_Proc_File->mode 	  = S_IFREG | S_IRUGO;
	FP_Proc_File->uid 	  = 0;
	FP_Proc_File->gid 	  = 0;
	FP_Proc_File->size 	  = 37;

	printk(KERN_INFO "/proc/%s created\n", PROCFS_NAME);
	
#ifndef CONFIG_DSL_CODESWAP
	fastpath_memory_init();
#endif

	init_timer(&fp_monitor_timer);
	fp_monitor_timer.function = fp_monitor_timeout;
	fp_monitor_timer.data = (unsigned long)NULL;
	mod_timer(&fp_monitor_timer, jiffies + HZ);
	
	return 0;
}

static void __exit fastpath_exit(void)
{
	printk("%s %s removed!\n", MODULE_NAME, MODULE_VERSION_FP);
	if(timer_pending(&fp_monitor_timer)){
		del_timer(&fp_monitor_timer);
	}
}

module_init(fastpath_init);
module_exit(fastpath_exit);
MODULE_LICENSE("GPL");

/*
2006-08/29:
	! Ignore TCP packet with FIN/RST/SYN flag (OR).
	! Ignore fragment of UDP packet.
2006-08/28:
	! NAT/NAPT bug fixed(RNAT/RNAPT NOT Working).
*/

