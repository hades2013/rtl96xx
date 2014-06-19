#ifndef __RAISECOM_H__
#define __RAISECOM_H__
#include "oam.h"

#define MAC_TYPE_STATIC 	0X0002
#define MAC_TYPE_DYNAMIC 	0X0001
#define MAC_TYPE_ALL 		0X0000

#define MAC_TYPE_PORT_UNI_1 	0X01
#define MAC_TYPE_PORT_UNI_2 	0X02
#define MAC_TYPE_PORT_UNI_3 	0X03
#define MAC_TYPE_PORT_UNI_4 	0X04
#define MAC_TYPE_PORT_UPLINK 	0XFE
#define MAC_TYPE_PORT_ALL 		0X00

#define GET_UPTIME_ERROR 		-1
#define GET_UPTIME_SUCCESS 		0
#define FILE_UPTIME "/proc/uptime"


#define RAISECOM_SET 0X01
#define RAISECOM_NO 0X00

#define RAISECOM_ENABLE 0X01
#define RAISECOM_DISABLE 0X02
#define MAC_BIND_SET 0X81
#define MAC_BIND_SET_UNDO 0X82
#define MAC_STATIC_DISP 0X83

#define RAISECOM_LEAF_RATE_LIMIT_UPLINK 	0x0000107c
#define RAISECOM_LEAF_FLOW_CONTROL_UPLINK 	0x0000107e
#define RAISECOM_LEAF_GET_TEMPARATURE		0x0000106a
#define RAISECOM_LEAF_GET_CURRENT			0x00001069
#define RAISECOM_LEAF_SET_HOSTNAME			0x00001001
#define RAISECOM_LEAF_GET_SHAPPING			0x000010c9
#define RAISECOM_LEAF_GET_HOSTNAME			0x00001001
#define RAISECOM_LEAF_SET_FILTER_BY_VLAN	0x00001019
#define RAISECOM_LEAF_GET_L2ADDRESS			0x0000101f
#define	RAISECOM_LEAF_GET_STATISTICS		0x00001026
#define	RAISECOM_LEAF_SET_MNG_VLAN			0x000073e9
#define	RAISECOM_LEAF_SET_STATIC_MAC		0x000073eb
#define	RAISECOM_LEAF_GET_MULTICAST_MAC		0x000073f2
#define	RAISECOM_LEAF_SET_LOOKBACK			0x00001023
#define	RAISECOM_LEAF_SET_PORT_MODE			0x00001012
#define	RAISECOM_LEAF_SET_MAC_AGING			0x00001008
#define	RAISECOM_LEAF_GET_PORT_MODE			0x00001011
#define	RAISECOM_LEAF_SET_PORT_ISOLATION	0x0000100f
#define	RAISECOM_LEAF_SET_STATISTICS_CLEAR	0x00001027
#define	RAISECOM_LEAF_GET_MCAST_MAC			0x00001017
#define	RAISECOM_LEAF_SET_TELNET			0x0000102d
#define	RAISECOM_LEAF_SET_MAC_CLEAR			0x00001021
#define	RAISECOM_LEAF_SET_DLF				0x00001079
#define	RAISECOM_LEAF_SET_UPLINK_FLOW		0x00001077
#define	RAISECOM_LEAF_GET_UPTIME			0x00001040
#define RAISECOM_LEAF_GET_DEVICE_INFO		0x00001006
#define RAISECOM_LEAF_SET_FASTLEAVE			0x00001016
#define RAISECOM_LEAF_SET_IGMP_QUERYCOUNT	0x0000101e
#define RAISECOM_LEAF_SET_IGMP_INTERVEL		0x0000101e
#define RAISECOM_LEAF_SET_MAC_LIMIT			0x00001013


#define RC_CONFIG_FLOW_CONTROL_MASK			(1<<14)
#define RC_CONFIG_PHY_STATE_MASK			(1<<10)
#define RC_CONFIG_POLICING_INGRESS_MASK		(1<<12)
#define RC_CONFIG_POLICING_EGRESS_MASK		(1<<8)

#define RC_CONFIG_VLAN_MODE_MASK			(3<<4)
#define RC_CONFIG_ISOLATION_MASK			(1<<2)
#define RC_CONFIG_MVLAN_STRIP_MASK			(1<<5)
#define RC_CONFIG_MVLAN_FASTLEAVE_MASK		(1<<6)


typedef enum{
	RC_PORT_AUTO = 1,
	RC_PORT_10HALF,
	RC_PORT_10FULL,
	RC_PORT_100HALF,
	RC_PORT_100FULL,
	RC_PORT_1000HALF,
	RC_PORT_1000FULL
}
rc_port_mode_t;

struct raisecom_hdr {
	u8_t	dest[6];
	u8_t	src[6];
	u32_t	dotvid;
	u8_t	length[2];
	u32_t	p_code;
	u8_t	reserved[12];
	u32_t	leaf;
	u32_t	sequence_l;
	u16_t	sequence_s;
}__attribute__((packed));
typedef struct raisecom_hdr raisecom_hdr_t;

struct raisecom_header_out {
	u8_t	dest[6];
	u8_t	src[6];
	u16_t	type;
}__attribute__((packed));
typedef struct raisecom_header_out raisecom_header_out_t;


struct raisecom_out {
	u32_t	p_code;
	u32_t	reserved1;
	u32_t	reserved2;
	u32_t	reserved3;
	u32_t	leaf;
	u32_t	sequence_l;
	u16_t	sequence_s;
}__attribute__((packed));
typedef struct raisecom_out raisecom_out_t;

#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

struct rc_mng_ip {
	u16_t	branch;
	u8_t	reserved[10];
	u32_t	leaf;
	struct in_addr ipaddr;
	struct in_addr mask;
	struct in_addr gateway;
}__attribute__((packed));
typedef struct rc_mng_ip rc_mng_ip_t;


struct rc_mng_ip_out {
	u32_t	reserved;
	u32_t	reserved2;
	struct in_addr ipaddr;
	struct in_addr mask;
	struct in_addr gateway;
	u16_t vlan;
	u32_t reserved3;
}__attribute__((packed));
typedef struct rc_mng_ip_out rc_mng_ip_out_t;

/*for 0x0030*/
struct rc_filter_part1 {
	u8_t	action;
	u8_t	reserved;
	u8_t	ip_protol;
	u16_t	reserved2;
	u8_t	port;
	u16_t	sport;
	u16_t	dport;
	u16_t	reserved3;
	u32_t	sip;
	u32_t	sip_mask;
	u32_t	dip;
	u32_t	dip_mask;
}__attribute__((packed));
typedef struct rc_filter_part1 rc_filter_part1_t;

/*for 0x0028*/
struct rc_filter_part2 {
	u8_t	action;
	u8_t	reserved;
	u32_t	port;
	u16_t	eth_type;
	u8_t	smac[6];
	u8_t	dmac[6];
}__attribute__((packed));
typedef struct rc_filter_part2 rc_filter_part2_t;

/*for 001c*/
struct rc_filter_part3 {
	u8_t	action;
	u8_t	reserved;
	u32_t	port;
	u16_t	vlan;
}__attribute__((packed));
typedef struct rc_filter_part3 rc_filter_part3_t;

/*for 002c*/
struct rc_mng_vlan {
	u16_t	reserved;
	u32_t	reserved2;
	u32_t	reserved3;
	u32_t	reserved4;
	u16_t	reserved5;
	u16_t	vlan;
}__attribute__((packed));
typedef struct rc_mng_vlan rc_mng_vlan_t;

struct rc_port_speed_duplex {
	u16_t	port;
	u8_t	mode;
}__attribute__((packed));
typedef struct rc_port_speed_duplex rc_port_speed_duplex_t;

struct rc_fast_leave{
	u16_t	port;
	u8_t	mode;
}__attribute__((packed));
typedef struct rc_fast_leave rc_fast_leave_t;


struct rc_port_mirror {
	u32_t	mode;
	u32_t	reserved;
	u8_t	ingress_mask;
	u8_t	reserved2[30];
	u8_t	ingress_mask_uplink;
	u8_t	egress_mask;
	u8_t	reserved3[30];
	u8_t	egress_mask_uplink;
}__attribute__((packed));
typedef struct rc_port_mirror rc_port_mirror_t;

struct rc_port_monitor {
	u32_t	mode;
	u16_t	port;
}__attribute__((packed));
typedef struct rc_port_monitor rc_port_monitor_t;

struct rc_mirror_act {
	u32_t	mode;
	u8_t	act;
}__attribute__((packed));
typedef struct rc_mirror_act rc_mirror_act_t;


struct rc_port_isolate {
	u16_t	port;
	u8_t	act;
}__attribute__((packed));
typedef struct rc_port_isolate rc_port_isolate_t;

struct rc_uni_clear {
	u8_t	act;
	u8_t	port;
}__attribute__((packed));
typedef struct rc_uni_clear rc_uni_clear_t;


struct rc_uni_mac_limit {
	u16_t	port;
	u16_t	reserved;
	u32_t	mac_limit;
}__attribute__((packed));
typedef struct rc_uni_mac_limit rc_uni_mac_limit_t;


struct rc_static_mac {
	u16_t	act;
	u32_t	reserved2;
	u32_t	reserved3;
	u8_t	mac[6];
	u16_t	port;
}__attribute__((packed));
typedef struct rc_static_mac rc_static_mac_t;

struct rc_mac_bind {
	u16_t	vlan;
	u16_t	port;
	u8_t	mac[6];
};
typedef struct rc_mac_bind rc_mac_bind_t;


struct rc_device {
	u8_t	reserved[12];
	u8_t	onu_mac[6];
	u8_t	hw_version[8];
	u8_t	sw_version[16];
	u8_t	fw_version[16];
	u8_t	hostname[32];
	u8_t	reserved2[50];
	u32_t	flash_size;
	u32_t	sdram;
	u8_t	reserved3[6];
	u16_t	switch_modole;
	u8_t	pf_version[100];
}__attribute__((packed));
typedef struct rc_device rc_device_t;


struct rc_mac_static_mul {
	u16_t	act;
	u32_t	reserved;
	u16_t	reserved2;
	u16_t	vlan;
	u8_t	mac[6];
	u8_t	port_mask;
}__attribute__((packed));
typedef struct rc_mac_static_mul rc_mac_static_mul_t;

struct rc_uni_statistic {
	u16_t	reserved;
	u32_t	port;
	u8_t	reserved2;
	u16_t	reserved3;
	u8_t	reserved4[0x1b];
	u8_t	ones[27];
	u8_t	zeros;
	
	u64_t   ifInUcastPkts;
	u64_t   ifInMcastPkts;
	u64_t   ifInBcastPkts;
	u64_t   ifInPausePkts;
	u64_t   ifInGoodOctets;
	u64_t   ifInBadOctets;
	u64_t   ifInUndersizePkts;

	u64_t   ifInFragmentsPkts;
	u64_t   ifInOversizePkts;
	u64_t   ifInJabbersPkts;
	u64_t   ifInFCSErrors;


	u64_t   ifIn_64_Octets;
	u64_t   ifIn_65_127_Octets;
	u64_t   ifIn_128_255_Octets;
	u64_t   ifIn_256_511_Octets;
	u64_t   ifIn_512_1023_Octets;
	u64_t   ifIn_1024_1518_Octets;

	u64_t   ifOutUcastPkts;
	u64_t   ifOutMcastPkts;
	u64_t   ifOutBcastPkts;
	u64_t   ifOutPausePkts;
	u64_t   ifOutGoodOctets;

	u64_t   ifInErrorPkts;
	u64_t   ifInDiscardPkts;
	u64_t   ifInUnkownProtos;
	u64_t	ifOutErrorPkts;
	u64_t	ifOutDiscardPkts;
	
}__attribute__((packed));
typedef struct rc_uni_statistic rc_uni_statistic_t;

struct rc_mac_out_static{
	u16_t	reserved;
	u32_t	reserved2;
	u8_t	mac[6];
	u16_t	port;
}__attribute__((packed));
typedef struct rc_mac_out_static rc_mac_out_static_t;


struct rc_mac_static {
	u8_t	mac[6];
	u16_t	port;
}__attribute__((packed));
typedef struct rc_mac_static rc_mac_static_t;

struct rc_lookback {
	u16_t	hello_time;
	u16_t	vlan;
}__attribute__((packed));
typedef struct rc_lookback rc_lookback_t;

struct rc_igmp{
	u16_t	reserved;
	u8_t	queryCount;
	u8_t	queryInterval;
	u16_t	agingTime;
}__attribute__((packed));
typedef struct rc_igmp rc_igmp_t;


struct rc_dhcp{
	u32_t	branch;
	u8_t	act;
}__attribute__((packed));
typedef struct rc_dhcp rc_dhcp_t;

struct rc_dhcp_act{
	u32_t	branch;
	u8_t	act;
}__attribute__((packed));
typedef struct rc_dhcp_act rc_dhcp_act_t;

struct rc_dhcp_snooping{
	u32_t	branch;
	u8_t	act;
}__attribute__((packed));
typedef struct rc_dhcp_snooping rc_dhcp_snooping_t;

struct rc_dhcp_option82_field{
	u32_t	branch;
	u32_t	reserved;
	u8_t	dataField[10];
}__attribute__((packed));
typedef struct rc_dhcp_option82_field rc_dhcp_option82_field_t;


struct rc_dhcp_option82_mode{
	u32_t	branch;
	u16_t	mode;
}__attribute__((packed));
typedef struct rc_dhcp_option82_mode rc_dhcp_option82_mode_t;

struct rc_l2mac_fieled{
	u8_t	mac[6];
	u16_t	reserved;
	u32_t	port;
	u8_t	reserved2;
	u8_t	type;
	u16_t	agingLeft;
}__attribute__((packed));
typedef struct rc_l2mac_fieled rc_l2mac_fieled_t;

struct rc_l2mac{
	u16_t macMax;
	u16_t macNum;
}__attribute__((packed));
typedef struct rc_l2mac rc_l2mac_t;

struct rc_l2mac_in{
	u32_t macType;
	u16_t sendMac;
	u16_t maxMac;
}__attribute__((packed));
typedef struct rc_l2mac_in rc_l2mac_in_t;

struct rc_port_mac{
	u8_t	mac[6];
	u32_t	port;
	u8_t	type;
	u16_t	agingLeft;
};
typedef struct rc_port_mac rc_port_mac_t;


struct rc_filter_acl {
	u8_t type;
	u8_t value[6];
	u8_t aclruleID;
};
typedef struct rc_filter_acl rc_filter_acl_t;


struct rc_config_down_onu
{
	u8_t reserved[42];
	u8_t mvlan_mode;
	u8_t reserved1[16];
	u8_t igmp_interval;
	u16_t igmp_agingTime;
	u8_t reserved2;
	u8_t igmp_query_count;
	u8_t reserved3[8];
}__attribute__((packed));
typedef struct rc_config_down_onu rc_config_down_onu_t;

struct rc_config_down_port
{
	u8_t reserved0[4];
	u16_t phy_admin_ctrl;
	u16_t nativeVlan;
	u16_t reserved1;
	u16_t vlan_translate_index[16];
	u16_t policing_in_cir;
	u16_t policing_in_cbs;
	u16_t policing_in_ebs;
	u32_t policing_ex_cir;
	u32_t policing_ex_pir;
	u8_t reserved2[4];
	u16_t mvlan;
	u8_t reserved3[14];
	u8_t mvlan_channel;
	u8_t mvlan_tag_strip;
	u8_t mode;
	u8_t reserved4[5];
	u8_t class_mark;
	u8_t reserved5[11];
}__attribute__((packed));
typedef struct rc_config_down_port rc_config_down_port_t;

struct rc_translation_pair
{
	u16_t index;
	u32_t oldTag;
	u32_t newTag;
	u8_t zeros[6];
}__attribute__((packed));
typedef struct rc_translation_pair rc_translation_pair_t;


struct rc_class_mark
{
	u32_t port_index;
	u8_t class_index;
	u8_t pri;
	u16_t value_index;
	u8_t queue_map;
	u8_t reserved[15];
}__attribute__((packed));
typedef struct rc_class_mark rc_class_mark_t;
struct rc_class_index
{
	u16_t uiIndex;
	u8_t class_type;
};
typedef struct rc_class_index rc_class_index_t;

struct rc_class_mark_l2
{
	u16_t value_index;
	u16_t reserved;
	u16_t eth_type;
	u8_t smac[6];
	u8_t dmac[6];
	u8_t reserved2[6];
}__attribute__((packed));
typedef struct rc_class_mark_l2 rc_class_mark_l2_t;

struct rc_class_mark_l3
{
	u16_t value_index;
	u8_t reserved;
	u8_t ip_type;
	u32_t sip;
	u32_t sip_mask;
	u16_t l4_sport;
	u16_t reserved2;
	u32_t dip;
	u32_t dip_mask;
	u16_t l4_dport;
	u16_t reserved3;
}__attribute__((packed));
typedef struct rc_class_mark_l3 rc_class_mark_l3_t;


extern void eopl_rc_pdu_receive(u8_t *p_in, u16_t length);

#endif

