#include <common/rt_type.h>
#include <rtk/port.h>








int32 dump_tcpudp (void);
int32 dump_napt (void);
int32 dump_netif (void);
int32 dump_pppoe (void);
int32 dump_nexthop (void);
int32 dump_arp (void);
int32 dump_ip (void);
int32 dump_l3 (void);
int32 dump_l4hs (void);
int32 dump_hs (void);
int32 dump_lut(int idx);
int32 dump_lut_table (void);
int32 dump_ipmc_group (void);
int32 dump_ipmc_routing (void);
int32 dump_vlan_table (void);
int32 dump_vlan_mbr (void);
int32 dump_svlan_mbr (void);
int32 dump_svlan_c2s (void);
int32 dump_svlan_mc2s (void);
int32 dump_svlan_sp2c (void);
int32 dump_acl (void);
int32 dump_acl_template (void);
int32 dump_acl_vidRangeTable (void);
int32 dump_acl_ipRangeTable (void);
int32 dump_acl_portRangeTable (void);
int32 dump_acl_pktlenRangeTable (void);
int32 dump_cf (void);
int32 dump_reg (void);
int32 dump_l34_bind_table (void);
int32 dump_l34_wantype_table (void);
int32 dump_ipv6_route_table (void);
int32 dump_ipv6_neighbor_table (void);
int32 dump_piso (void);
int32 dump_4kVlan(void);
void dump_packet(u8 *pkt,u32 size,char *memo);
void dump_compare_packet(uint8 *output,uint8 *expect,int out_size,int expect_size);

int8 *_inet_ntoa(rtk_ip_addr_t ina);


int32 mibdump_lut(int idx);
int32 mibdump_lut_table (void);
int32 mibdump_ipv6_neighbor_table (void);
int32 mibdump_frag(void);
int _rtk_rg_trace_filter_compare(struct sk_buff *skb);


