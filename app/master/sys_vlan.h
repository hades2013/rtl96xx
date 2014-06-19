/*
 * switch.h
 *
 *  Created on: Dec 30, 2010
 *      Author: root
 */

#ifndef SWITCH_H_
#define SWITCH_H_

#include <basetype.h>
#include <net/if.h>
//#include "cable.h"



#ifndef __packed 
#define __packed __attribute__((__packed__))
#endif 


//#define PORT_MEMBER_ALL ((1 << CPU_PORT_NUMBER) | (1 << ETH0_PORT_NUMBER) | (1 << ETH1_PORT_NUMBER) | (1 << CLT0_PORT_NUMBER) | (1 << CLT1_PORT_NUMBER))

//#define PORT_MBRMASK(x) (1 << (x))

//#define MBRMASK_VALUE(x) (x.bits[0])




#if 0
typedef enum {
	ETH_SPEED_10 = 0,
	ETH_SPEED_100,
	ETH_SPEED_1000,
	ETH_SPEED_END
}
clt_eth_speed_t;

#define	DEFAULT_SWITCH_PORT_SPEED	ETH_SPEED_100

typedef enum {
	ETH_DUPLEX_HALF = 0,
	ETH_DUPLEX_FULL,
	ETH_DUPLEX_END
}
clt_eth_duplex_t;

typedef enum {
	ETH_FLOWCTRL_OFF = 0,
	ETH_FLOWCTRL_ON,
	ETH_FLOWCTRL_END
}
clt_eth_flowctrl_t;


typedef enum {
	ETH_PRIORITY_0 = 0,
	ETH_PRIORITY_1,
	ETH_PRIORITY_2,
	ETH_PRIORITY_3,		
	ETH_PRIORITY_4,
	ETH_PRIORITY_5,
	ETH_PRIORITY_6,
	ETH_PRIORITY_7,
	ETH_PRIORITY_OFF
}
clt_eth_priority_t;
#endif

#if 0

typedef enum {
	ETH_INGRESS_FILTER_OFF = 0,
	ETH_INGRESS_FILTER_ON
}
clt_eth_ingress_filter_t;

typedef enum {
	ETH_EGRESS_FILTER_OFF = 0,
	ETH_EGRESS_FILTER_ON
}
clt_eth_egress_filter_t;
#endif








//#define vlan_untag(x)  ((x) & VLAN_FLAG_UNTAG)


//typedef uint32_t vlan_member_t;






#if 0
typedef struct __packed {
	char ifname[IFNAMSIZ];
	uint8_t phy;
	uint8_t enabled;
	uint8_t autoneg;
	uint8_t speed;
	uint8_t duplex;
	uint8_t flowctrl;
	uint8_t priority;
	uint32_t ingress_limit; // 0 disabled 1k will be 1024  etc.. 
	uint32_t egress_limit;  // 0 disabled
	uint8_t ingress_filter;
	uint16_t pvid;// read only
}
switch_interface_t;


typedef struct __packed {
	char ifname[IFNAMSIZ];
	uint8_t link;
	uint8_t speed;
	uint8_t duplex;
}
switch_interface_status_t;

/* copy from rtk_api.h */

typedef enum 
{
    MIBS_IfInOctets = 0,
    MIBS_Dot3StatsFCSErrors,
    MIBS_Dot3StatsSymbolErrors,
    MIBS_Dot3InPauseFrames,
    MIBS_Dot3ControlInUnknownOpcodes,        
    MIBS_EtherStatsFragments,
    MIBS_EtherStatsJabbers,
    MIBS_IfInUcastPkts,
    MIBS_EtherStatsDropEvents,
    MIBS_EtherStatsOctets,
    MIBS_EtherStatsUnderSizePkts,
    MIBS_EtherOversizeStats,
    MIBS_EtherStatsPkts64Octets,
    MIBS_EtherStatsPkts65to127Octets,
    MIBS_EtherStatsPkts128to255Octets,
    MIBS_EtherStatsPkts256to511Octets,
    MIBS_EtherStatsPkts512to1023Octets,
    MIBS_EtherStatsPkts1024to1518Octets,
    MIBS_EtherStatsMulticastPkts,
    MIBS_EtherStatsBroadcastPkts,    
    MIBS_IfOutOctets,
    MIBS_Dot3StatsSingleCollisionFrames,
    MIBS_Dot3StatsMultipleCollisionFrames,
    MIBS_Dot3StatsDeferredTransmissions,
    MIBS_Dot3StatsLateCollisions,
    MIBS_EtherStatsCollisions,
    MIBS_Dot3StatsExcessiveCollisions,
    MIBS_Dot3OutPauseFrames,
    MIBS_Dot1dBasePortDelayExceededDiscards,
    MIBS_Dot1dTpPortInDiscards,
    MIBS_IfOutUcastPkts,
    MIBS_IfOutMulticastPkts,
    MIBS_IfOutBroadcastPkts,
    MIBS_OutOampduPkts,
    MIBS_InOampduPkts,
    MIBS_PktgenPkts,
    MIBS_PORT_CNTR_END
}switch_port_mibs_t;

 /*#ifndef __RTK_API_H__*/

#define EOC_PORT_MIBS_NUM  MIBS_PORT_CNTR_END

typedef struct __packed {
	char ifname[IFNAMSIZ];
	uint8_t phy;
	uint64_t mibs[EOC_PORT_MIBS_NUM];
}
switch_interface_mib_t;

typedef struct __packed{
	uint64_t in_octets;
	uint32_t in_pkts; // unicast 
	uint32_t in_mcast; // none unicast
	uint32_t in_error;
	uint64_t out_octets;
	uint32_t out_pkts;
	uint32_t out_mcast;
	uint32_t out_drops;
}
switch_interface_stats_t;

#endif

typedef struct{
	int valid;
	vlan_id_t vid;// not use now
	logic_pmask_t mbrmask;
	logic_pmask_t untagmask;	
	UINT32 fid;
}
vcfg_vlan_t;
typedef struct{
	int phy;
	vlan_id_t pvid;
	int priority;
	PORT_INGRESS_MODE_E accept_type;
	BOOL ingress_filter;
	PORT_EGRESS_MODE_E tag_mode;	
}
vcfg_port_t;

typedef struct{
	int vlanmode;
	vlan_id_t management_vlan;
	vcfg_vlan_t vlans[MAX_VLAN_NUMS]; 
	vcfg_port_t ports[MAX_PORT_NUM+1];
}
vlan_apply_t;

uint32_t switch_vlan_interface_data_size(void);
switch_vlan_interface_t *switch_vlan_interface_data_malloc(void);
uint16_t switch_vlan_interface_count(void);
void switch_vlan_interface_data_free(void *vlanif);
switch_vlan_interface_t *switch_vlan_interface_group_malloc(switch_vlan_interface_t *req_vlanif, uint16_t req_count, uint16_t *ret_count);
void switch_vlan_interface_group_free(void *vlanif);
int switch_vlan_interface_group_update(switch_vlan_interface_t *vlanif, uint16_t count);
int switch_vlan_interface_group_delete(switch_vlan_interface_t *vlanif, uint16_t count);
int switch_vlan_interface_group_new(switch_vlan_interface_t *vlanif, uint16_t count);
int switch_vlan_interface_apply(int apply_option);
int switch_vlan_interface_commit(int apply_option);


switch_vlan_group_t *switch_vlan_group_get(switch_vlan_group_t *vlan_groups, uint16_t count, int *ret_count);
//uint32_t switch_vlan_group_get(switch_vlan_group_t *vlan_groups, uint16_t count);
uint16_t switch_vlan_entry_count(void);

uint32_t switch_vlan_data_size(void);
switch_vlan_group_t *switch_vlan_data_malloc(void);
void switch_vlan_data_free(void *data);

int switch_vlan_group_delete(switch_vlan_group_t *vlan_groups, uint16_t count);
int switch_vlan_group_new(switch_vlan_group_t *vlan_groups, uint16_t count);
int switch_vlan_group_update(switch_vlan_group_t *vlan_groups, uint16_t count);
int switch_vlan_commit(int apply_option);

int switch_vlan_mode_set(switch_vlan_mode_t *vm);
void switch_vlan_mode_get(switch_vlan_mode_t *vm);
int switch_vlan_mode_commit( int apply_option);
int switch_vlan_mode_apply( int apply_option);
int switch_vlan_apply(int apply_option);


enum {
	VLAN_CHANGED = 0,
	MVLAN_CHANGED,
	VLANIF_CHANGED
};

int switch_vlan_config_apply(int reason);
int switch_vlan_apply_all(void);
//void switch_vlan_mode_get(switch_vlan_mode_t *vm);


//int switch_interface_mibs_update_all( int reset );
//switch_interface_mib_t *switch_interface_mibs_update(char *name, int reset);
//int switch_interface_status_update(void);

//int switch_interface_group_set(switch_interface_t *ifp, int count);
//int switch_interface_group_get(switch_interface_t *ifp, int count);

//int switch_interface_apply(int apply_option);
//int switch_interface_commit(int apply_option);


//int switch_init(void);
//int switch_apply(void);
//void switch_del_lut(ethernet_addr_t *mac);

//void switch_port_isolate(uint32_t ports);



#endif /* SWITCH_H_ */
