/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
*/



#ifdef CONFIG_APOLLO_MODEL
#else
#ifdef __linux__
#include <linux/init.h>
#include <linux/slab.h> //for kmalloc/kfree
#include <linux/delay.h> //for mdelay
#include <linux/skbuff.h>
#include <linux/timer.h>
#include <linux/proc_fs.h> //for create proc
#include <linux/config.h>
#include <linux/netdevice.h>

#endif
#endif

#include <common/error.h>
#include <rtk/init.h>
#include <rtk/l34_bind_config.h>
#include <rtk/svlan.h>
#include <dal/apollomp/raw/apollomp_raw_hwmisc.h>
#include <rtk/sec.h>
#include <rtk/stat.h>
#include <rtk/ponmac.h>
#ifdef CONFIG_APOLLO_RLE0371
#include <dal/apollo/dal_apollo_l34.h>
#endif
//#include <dal/apollomp/dal_apollomp_l34.h>	//FIXME: because RTK do not have binding related APIs
//#include <hal/common/halctrl.h>
//#include <hal/chipdef/apollo/apollo_reg_struct.h>
//#include <hal/mac/reg.h>
#ifdef CONFIG_RG_LAYER2_SOFTWARE_LEARN
#include <rtk/irq.h>		//for register link-change event
#include <rtk/intr.h>		//for get and clear link-down indicator register
#endif



#include <rtk_rg_liteRomeDriver.h>
#include <rtk_rg_internal.h>
#include <rtk_rg_fwdEngine.h>
#include <rtk_rg_define.h>
#include <rtk_rg_struct.h>

#ifdef CONFIG_RG_DEBUG
#include <rtk_rg_debug.h>
#endif
#include <rtk_rg_callback.h>


#if 0 //ysleu: it's not good to mix apollo testing codes and rome driver toghter, the staff we need was redefined in rtk_rg_internal.h
#include <rtl_glue.h>
#include <rtl_utils.h>
#endif

#ifdef __KERNEL__
#endif
//#include "types.h"

/* Module Name: System*/
//rtk_rg_intfInfo_t RG_GLB_INTF_INFO[8];		//store each interface information, LAN or WAN
//int volatile RG_GLB_ARP_REQUEST_FINISHED[MAX_NETIF_SW_TABLE_SIZE];	//used to indicate the ARP request return or not





#ifdef CONFIG_APOLLO_RLE0371
//short RG_GLB_WAN_TYPE[MAX_NETIF_SW_TABLE_SIZE];		//software WAN TYPE table for test chip
#endif
#if 0
#define STATIC_DHCP_ALLOC_NUM 	100
_rg_DHCPStaticEntry_t _DHCP_STATIC[STATIC_DHCP_ALLOC_NUM];
#endif

/*the option for rtk_init setting by virtualmac or not: 0:normal, 1:virtualmac*/
int virtualmacEnable = DISABLE;


/* ALL global Variables & Tables */
rtk_rg_globalDatabase_t	rg_db;
rtk_rg_globalKernel_t rg_kernel;

char mt_watch_tmp[512];

int32 apollo_mac_init(void);


int32 RTK_L2_ADDR_ADD(rtk_l2_ucastAddr_t *pL2Addr)
{
	int ret;
	ret=rtk_l2_addr_add(pL2Addr);
	if(ret==RT_ERR_OK)
	{
		rg_db.lut[pL2Addr->index].rtk_lut.entryType=RTK_LUT_L2UC;
		memcpy(&rg_db.lut[pL2Addr->index].rtk_lut.entry.l2UcEntry,pL2Addr,sizeof(rtk_l2_ucastAddr_t));
		rg_db.lut[pL2Addr->index].valid=1;
	}
	return ret;
}

int32 RTK_L2_ADDR_DEL(rtk_l2_ucastAddr_t *pL2Addr)
{
	int ret;
	ret=rtk_l2_addr_del(pL2Addr);
	if(ret==RT_ERR_OK)
	{
		//------------------ Critical Section start -----------------------//
		//rg_lock(&rg_kernel.saLearningLimitLock);
		if(pL2Addr->port>=RTK_RG_PORT_CPU)
			atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[pL2Addr->ext_port+RTK_RG_PORT_CPU]);
		else
			atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[pL2Addr->port]);
		//------------------ Critical Section End -----------------------//
		//rg_unlock(&rg_kernel.saLearningLimitLock);
		
		memset(&rg_db.lut[pL2Addr->index],0,sizeof(rtk_rg_table_lut_t));
	}
	return ret;
}



	
int32 RTK_L2_IPMCASTADDR_ADD(rtk_l2_ipMcastAddr_t *pIpmcastAddr)
{
	int ret;
	ret=rtk_l2_ipMcastAddr_add(pIpmcastAddr);
	if(ret==RT_ERR_OK)
	{
		rg_db.lut[pIpmcastAddr->index].rtk_lut.entryType=RTK_LUT_L3MC;
		memcpy(&rg_db.lut[pIpmcastAddr->index].rtk_lut.entry.ipmcEntry,pIpmcastAddr,sizeof(rtk_l2_ipMcastAddr_t));
		rg_db.lut[pIpmcastAddr->index].valid=1;
	}
	return ret;
}

int32 RTK_L2_IPMCASTADDR_DEL(rtk_l2_ipMcastAddr_t *pIpmcastAddr)
{
	int ret;
	ret=rtk_l2_ipMcastAddr_del(pIpmcastAddr);
	if(ret==RT_ERR_OK)
	{	
		memset(&rg_db.lut[pIpmcastAddr->index],0,sizeof(rtk_rg_table_lut_t));
	}
	return ret;
}

int32 RTK_L2_MCASTADDR_ADD(rtk_l2_mcastAddr_t *pMcastAddr)
{
	int ret;
	ret=rtk_l2_mcastAddr_add(pMcastAddr);
	if(ret==RT_ERR_OK)
	{
		rg_db.lut[pMcastAddr->index].rtk_lut.entryType=RTK_LUT_L2MC;
		memcpy(&rg_db.lut[pMcastAddr->index].rtk_lut.entry.l2McEntry,pMcastAddr,sizeof(rtk_l2_mcastAddr_t));
		rg_db.lut[pMcastAddr->index].valid=1;
	}
	return ret;

}

int32 RTK_L2_MCASTADDR_DEL(rtk_l2_mcastAddr_t *pMcastAddr)
{
	int ret;
	ret=rtk_l2_mcastAddr_del(pMcastAddr);
	if(ret==RT_ERR_OK)
	{	
		memset(&rg_db.lut[pMcastAddr->index],0,sizeof(rtk_rg_table_lut_t));
	}
	return ret;
}

int32 RTK_L34_NETIFTABLE_SET(uint32 idx, rtk_l34_netif_entry_t *entry)
{
	int ret;
	ret=rtk_l34_netifTable_set(idx,entry);
	if(ret==RT_ERR_OK)
	{
		memcpy(&rg_db.netif[idx].rtk_netif,entry,sizeof(*entry));
	}
	return ret;
}


int32 RTK_L34_ROUTINGTABLE_SET(uint32 idx, rtk_l34_routing_entry_t *entry)
{
	int ret;
	
	ret=rtk_l34_routingTable_set(idx,entry);
	if(ret==RT_ERR_OK)
	{
		memcpy(&rg_db.l3[idx].rtk_l3,entry,sizeof(*entry));
		if(entry->ipMask==0)
			rg_db.l3[idx].netmask=0;
		else
			rg_db.l3[idx].netmask=~((1<<(31-entry->ipMask))-1);
	}
	return ret;
}

int32 RTK_L34_EXTINTIPTABLE_SET(uint32 idx, rtk_l34_ext_intip_entry_t *entry)
{
	int ret;
	ret=rtk_l34_extIntIPTable_set(idx,entry);
	if(ret==RT_ERR_OK)
	{
		memcpy(&rg_db.extip[idx].rtk_extip,entry,sizeof(*entry));
	}
	return ret;
}

int32 RTK_L34_NEXTHOPTABLE_SET(uint32 idx, rtk_l34_nexthop_entry_t *entry)
{
	int ret;
	ret=rtk_l34_nexthopTable_set(idx,entry);
	if(ret==RT_ERR_OK)
	{
		memcpy(&rg_db.nexthop[idx].rtk_nexthop,entry,sizeof(*entry));
	}
	return ret;
}

int32 RTK_L34_PPPOETABLE_SET(uint32 idx, rtk_l34_pppoe_entry_t *entry)
{
	int ret;
	ret=rtk_l34_pppoeTable_set(idx,entry);
	if(ret==RT_ERR_OK)
	{
		memcpy(&rg_db.pppoe[idx].rtk_pppoe,entry,sizeof(*entry));
	}
	return ret;
}

int32 RTK_L34_ARPTABLE_SET(uint32 idx, rtk_l34_arp_entry_t *entry)
{
	int ret;
	ret=rtk_l34_arpTable_set(idx,entry);
	if(ret==RT_ERR_OK)
	{
		memcpy(&rg_db.arp[idx].rtk_arp,entry,sizeof(*entry));
	}
	return ret;
}

int32 RTK_L34_NAPTINBOUNDTABLE_SET(int8 forced,uint32 idx, rtk_l34_naptInbound_entry_t *entry)
{
	int ret=RT_ERR_OK;
	if(idx<MAX_NAPT_IN_HW_TABLE_SIZE)	
		ret=rtk_l34_naptInboundTable_set(forced,idx,entry);
	
	if(ret==RT_ERR_OK)
	{
		memcpy(&rg_db.naptIn[idx].rtk_naptIn,entry,sizeof(*entry));
	}
	return ret;
}

int32 RTK_L34_NAPTOUTBOUNDTABLE_SET(int8 forced,uint32 idx, rtk_l34_naptOutbound_entry_t *entry)
{
	int ret=RT_ERR_OK;
	if(idx<MAX_NAPT_OUT_HW_TABLE_SIZE)		
		ret=rtk_l34_naptOutboundTable_set(forced,idx,entry);
	
	if(ret==RT_ERR_OK)
	{
		memcpy(&rg_db.naptOut[idx].rtk_naptOut,entry,sizeof(*entry));
	}
	return ret;
}

int32 RTK_L34_WANTYPETABLE_SET(uint32 idx,rtk_wanType_entry_t * entry)
{	
	int ret;
#ifdef CONFIG_APOLLO_RLE0371
	ret=RT_ERR_OK;
#else
	ret=rtk_l34_wanTypeTable_set(idx,entry);
#endif
	if(ret==RT_ERR_OK)
	{
		memcpy(&rg_db.wantype[idx].rtk_wantype,entry,sizeof(*entry));
	}
	return ret;
}

int32 RTK_VLAN_CREATE(rtk_vlan_t vid)
{
	int ret;
	ret=rtk_vlan_create(vid);
	if(ret==RT_ERR_OK)
		rg_db.vlan[vid].valid = 1;		//Enable software MIB record
	return ret;
}

int32 RTK_VLAN_PORT_SET(rtk_vlan_t vid,rtk_portmask_t * pMember_portmask,rtk_portmask_t * pUntag_portmask)
{
	int ret;
	ret=rtk_vlan_port_set(vid,pMember_portmask,pUntag_portmask);
	if(ret==RT_ERR_OK)
	{
		//Copy to software MIB
		memcpy(&rg_db.vlan[vid].MemberPortmask,pMember_portmask,sizeof(rtk_portmask_t));
		memcpy(&rg_db.vlan[vid].UntagPortmask,pUntag_portmask,sizeof(rtk_portmask_t));
	}
	return ret;
}

int32 RTK_VLAN_EXTPORT_SET(rtk_vlan_t vid,rtk_portmask_t * pExt_portmask)
{
	int ret;
	ret=rtk_vlan_extPort_set(vid,pExt_portmask);
	if(ret==RT_ERR_OK)
	{
		//Copy to software MIB
		memcpy(&rg_db.vlan[vid].Ext_portmask,pExt_portmask,sizeof(rtk_portmask_t));
	}
	return ret;
}

int32 RTK_VLAN_FID_SET(rtk_vlan_t vid,rtk_fid_t fid)
{
	int ret;
	ret=rtk_vlan_fid_set(vid,fid);
	if(ret==RT_ERR_OK)
	{
		//Copy to software MIB
		memcpy(&rg_db.vlan[vid].fid,&fid,sizeof(rtk_fid_t));
	}
	return ret;
}

int32 RTK_VLAN_FIDMODE_SET(rtk_vlan_t vid,rtk_fidMode_t mode)
{
	int ret;
	ret=rtk_vlan_fidMode_set(vid,mode);
	if(ret==RT_ERR_OK)
	{
		//Copy to software MIB
		memcpy(&rg_db.vlan[vid].fidMode,&mode,sizeof(rtk_fidMode_t));
	}
	return ret;
}

int32 RTK_VLAN_PRIORITY_SET(rtk_vlan_t vid,rtk_pri_t priority)
{
	int ret;
	ret=rtk_vlan_priority_set(vid,priority);
	if(ret==RT_ERR_OK)
	{
		//Copy to software MIB
		memcpy(&rg_db.vlan[vid].priority,&priority,sizeof(rtk_pri_t));
	}
	return ret;
}

int32 RTK_VLAN_PRIORITYENABLE_SET(rtk_vlan_t vid,rtk_enable_t enable)
{
	int ret;
	ret=rtk_vlan_priorityEnable_set(vid,enable);
	if(ret==RT_ERR_OK)
	{
		//Copy to software MIB
		memcpy(&rg_db.vlan[vid].priorityEn,&enable,sizeof(rtk_enable_t));
	}
	return ret;
}

int32 RTK_VLAN_DESTROY(rtk_vlan_t vid)
{
	int ret;
	ret=rtk_vlan_destroy(vid);
	if(ret==RT_ERR_OK)
		bzero(&rg_db.vlan[vid],sizeof(rtk_rg_table_vlan_t));		//Disable software MIB record
	return ret;
}

int32 RTK_VLAN_PORTPVID_SET(rtk_port_t port,uint32 pvid)
{
	int ret;
	ret=rtk_vlan_portPvid_set(port,pvid);
	if(ret==RT_ERR_OK)
		rg_db.systemGlobal.portBasedVID[port]=pvid;
	return ret;
}

int32 RTK_VLAN_EXTPORTPVID_SET(uint32 extPort,uint32 pvid)
{
	int ret;
	ret=rtk_vlan_extPortPvid_set(extPort,pvid);
	if(ret==RT_ERR_OK)
		rg_db.systemGlobal.portBasedVID[extPort+RTK_RG_PORT_CPU]=pvid;
	return ret;
}

int32 RTK_VLAN_PROTOGROUP_SET(uint32 protoGroupIdx,rtk_vlan_protoGroup_t *pProtoGroup)
{
	int ret;
	ret=rtk_vlan_protoGroup_set(protoGroupIdx,pProtoGroup);
	if(ret==RT_ERR_OK)
	{
		//Copy to software MIB
		memcpy(&rg_db.systemGlobal.protoGroup[protoGroupIdx],pProtoGroup,sizeof(rtk_vlan_protoGroup_t));
	}
	return ret;
}

int32 RTK_VLAN_PORTPROTOVLAN_SET(rtk_port_t port,uint32 protoGroupIdx,rtk_vlan_protoVlanCfg_t * pVlanCfg)
{
	int ret;
	ret=rtk_vlan_portProtoVlan_set(port,protoGroupIdx,pVlanCfg);
	if(ret==RT_ERR_OK)
	{
		//Copy to software MIB
		memcpy(&rg_db.systemGlobal.protoBasedVID[port].protoVLANCfg[protoGroupIdx],pVlanCfg,sizeof(rtk_vlan_protoVlanCfg_t));
	}
	return ret;
}

int32 RTK_L34_BINDINGTABLE_SET(uint32 idx,rtk_binding_entry_t * bindEntry)
{
	int ret;
	ret=rtk_l34_bindingTable_set(idx,bindEntry);
	if(ret==RT_ERR_OK)
	{
		//Copy to software MIB
		memcpy(&rg_db.bind[idx].rtk_bind,bindEntry,sizeof(rtk_binding_entry_t));
	}
	return ret;
}

int32 RTK_L34_IPV6ROUTINGTABLE_SET(uint32 idx,rtk_ipv6Routing_entry_t * ipv6RoutEntry)
{
	int ret;
	ret=rtk_l34_ipv6RoutingTable_set(idx,ipv6RoutEntry);
	if(ret==RT_ERR_OK)
	{
		//Copy to software MIB
		memcpy(&rg_db.v6route[idx].rtk_v6route,ipv6RoutEntry,sizeof(rtk_ipv6Routing_entry_t));
	}
	return ret;
}

int32 RTK_L34_IPV6NEIGHBORTABLE_SET(uint32 idx,rtk_ipv6Neighbor_entry_t * ipv6NeighborEntry)
{
	int ret;
	ret=rtk_l34_ipv6NeighborTable_set(idx,ipv6NeighborEntry);
	if(ret==RT_ERR_OK)
	{
		//Copy to software MIB
		memcpy(&rg_db.v6neighbor[idx].rtk_v6neighbor,ipv6NeighborEntry,sizeof(rtk_ipv6Neighbor_entry_t));
	}
	return ret;
}

int32 RTK_L2_IPMCGROUP_ADD(ipaddr_t gip, rtk_portmask_t *pPortmask)
{
	int i;

	//Is entry exist?
	for(i=0;i<MAX_IPMCGRP_HW_TABLE_SIZE;i++)
	{
		if((rg_db.ipmcgrp[i].groupIp==gip)&&(rg_db.ipmcgrp[i].valid==TRUE))
		{
			return RT_ERR_OK;
		}		
	}

	//to find an empty entry.
	for(i=0;i<MAX_IPMCGRP_HW_TABLE_SIZE;i++)
	{
		if(rg_db.ipmcgrp[i].valid==FALSE)
		{
			int r;			
			r=rtk_l2_ipmcGroup_add(gip,pPortmask);			
			if(r==RT_ERR_OK)
			{
				rg_db.ipmcgrp[i].groupIp=gip;
				rg_db.ipmcgrp[i].portMsk.bits[0]=pPortmask->bits[0];
				rg_db.ipmcgrp[i].valid=TRUE;
			}
			return r;
		}		
	}

	return RT_ERR_FAILED;

}

int32 RTK_L2_IPMCGROUP_DEL(ipaddr_t gip)
{
	int i;
	for(i=0;i<MAX_IPMCGRP_HW_TABLE_SIZE;i++)
	{
		if((rg_db.ipmcgrp[i].groupIp==gip)&&(rg_db.ipmcgrp[i].valid==TRUE))
		{
			int r;
			r=rtk_l2_ipmcGroup_del(gip);
			if(r==RT_ERR_OK) rg_db.ipmcgrp[i].valid=FALSE;
			return r;
		}		
	}
	return RT_ERR_FAILED;
}



#define rtk_l2_addr_add "PLEASE USE MACRO FUNCTION!!"
#define rtk_l2_addr_del "PLEASE USE MACRO FUNCTION!!"
#define rtk_l2_mcastAddr_add "PLEASE USE MACRO FUNCTION!!"
#define rtk_l2_mcastAddr_del "PLEASE USE MACRO FUNCTION!!"
#define rtk_l2_ipMcastAddr_add "PLEASE USE MACRO FUNCTION!!"
#define rtk_l2_ipMcastAddr_del "PLEASE USE MACRO FUNCTION!!"
#define rtk_l34_netifTable_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_l34_routingTable_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_l34_extIntIPTable_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_l34_nexthopTable_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_l34_pppoeTable_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_l34_arpTable_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_l34_naptInboundTable_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_l34_naptOutboundTable_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_l34_wanTypeTable_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_vlan_create "PLEASE USE MACRO FUNCTION!!"
#define rtk_vlan_port_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_vlan_extport_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_vlan_fid_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_vlan_fidMode_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_vlan_priority_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_vlan_priorityEnable_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_vlan_destroy "PLEASE USE MACRO FUNCTION!!"
#define rtk_vlan_portPvid_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_vlan_extPortPvid_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_vlan_protoGroup_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_vlan_portProtoVlan_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_l34_bindingTable_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_l34_ipv6RoutingTable_set "PLEASE USE MACRO FUNCTION!!"
#define rtk_l34_ipv6NeighborTable_set "PLEASE USE MACRO FUNCTION!!"


int _rtk_rg_acl_reserved_multicastVidTranslate(void){

	//translate ipv4 multicast vid to 1
	rtk_acl_ingress_entry_t aclRule;
	rtk_acl_field_t aclField;
	bzero(&aclRule,sizeof(aclRule));
	bzero(&aclField,sizeof(aclField));
	
	aclRule.valid=ENABLED;
	aclRule.index=RESERVED_ACL_MULTICAST_VID_TRANSLATE_FOR_IPV4;
	
	aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField.fieldUnion.pattern.fieldIdx = 7;//template[1],field[7] = DIP[31:16]
	aclField.fieldUnion.data.value=0xe000; //DIP start with 1110 ...
	aclField.fieldUnion.data.mask=0xf000;
	assert_ok(rtk_acl_igrRuleField_add(&aclRule, &aclField));
		
	aclRule.activePorts.bits[0]=0x10;//PON only
	aclRule.templateIdx=1; /*use DIP[31:16]: template[1],field[7]*/
	aclRule.act.enableAct[ACL_IGR_CVLAN_ACT]=ENABLED;
	aclRule.act.cvlanAct.act=ACL_IGR_CVLAN_IGR_CVLAN_ACT;
	aclRule.act.cvlanAct.cvid=DEFAULT_CPU_VLAN; //remark to 1
	assert_ok(rtk_acl_igrRuleEntry_add(&aclRule));		



	//translate ipv6 multicast vid to 1
	bzero(&aclRule,sizeof(aclRule));
	bzero(&aclField,sizeof(aclField));

	aclRule.valid=ENABLED;
	aclRule.index=RESERVED_ACL_MULTICAST_VID_TRANSLATE_FOR_IPV6;
	
	aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField.fieldUnion.pattern.fieldIdx = 5;//template[3],field[5] = v6DIP[127:112] use ACL_FIELD_USER_DEFINED13
	aclField.fieldUnion.data.value=0xff00; //v6DIP start with 1111 1111 ...
	aclField.fieldUnion.data.mask=0xff00;
	assert_ok(rtk_acl_igrRuleField_add(&aclRule, &aclField));
		
	aclRule.activePorts.bits[0]=0x10;//PON only
	aclRule.templateIdx=3; /*use DIP[127:120]: template[3],field[5]*/
	aclRule.act.enableAct[ACL_IGR_CVLAN_ACT]=ENABLED;
	aclRule.act.cvlanAct.act=ACL_IGR_CVLAN_IGR_CVLAN_ACT;
	aclRule.act.cvlanAct.cvid=DEFAULT_CPU_VLAN; //remark to 1
	assert_ok(rtk_acl_igrRuleEntry_add(&aclRule));		

	return RT_ERR_RG_OK;
}

int _rtk_rg_acl_asic_init(void)
{
    int i=0,ret;
	rtk_acl_igr_rule_mode_t pMode ;
	rtk_filter_unmatch_action_type_t pAction;
	rtk_acl_template_t aclTemplate;
    rtk_acl_rangeCheck_ip_t iprangeEntry;
    rtk_acl_rangeCheck_l4Port_t prtRangeEntry;
#ifdef CONFIG_APOLLO_MODEL
#else
		rtk_acl_field_entry_t fieldSel;
#endif


    //set ACL_MODE
    pMode = ACL_IGR_RULE_MODE_0;
    ASSERT_EQ(rtk_acl_igrRuleMode_set(pMode),RT_ERR_OK);

    //set ACL_EN
    ASSERT_EQ(rtk_acl_igrState_set(RTK_RG_PORT0,ENABLED),RT_ERR_OK);
    ASSERT_EQ(rtk_acl_igrState_set(RTK_RG_PORT1,ENABLED),RT_ERR_OK);
    ASSERT_EQ(rtk_acl_igrState_set(RTK_RG_PORT2,ENABLED),RT_ERR_OK);
    ASSERT_EQ(rtk_acl_igrState_set(RTK_RG_PORT3,ENABLED),RT_ERR_OK);
    ASSERT_EQ(rtk_acl_igrState_set(RTK_RG_PORT_PON,ENABLED),RT_ERR_OK);
    ASSERT_EQ(rtk_acl_igrState_set(RTK_RG_PORT_RGMII,ENABLED),RT_ERR_OK);
    ASSERT_EQ(rtk_acl_igrState_set(RTK_RG_PORT_CPU,ENABLED),RT_ERR_OK);

    //set ACL_PERMIT
    pAction = FILTER_UNMATCH_PERMIT;
    ASSERT_EQ(rtk_acl_igrUnmatchAction_set(RTK_RG_PORT0,pAction),RT_ERR_OK);
    ASSERT_EQ(rtk_acl_igrUnmatchAction_set(RTK_RG_PORT1,pAction),RT_ERR_OK);
    ASSERT_EQ(rtk_acl_igrUnmatchAction_set(RTK_RG_PORT2,pAction),RT_ERR_OK);
    ASSERT_EQ(rtk_acl_igrUnmatchAction_set(RTK_RG_PORT3,pAction),RT_ERR_OK);
    ASSERT_EQ(rtk_acl_igrUnmatchAction_set(RTK_RG_PORT_PON,pAction),RT_ERR_OK);
    ASSERT_EQ(rtk_acl_igrUnmatchAction_set(RTK_RG_PORT_RGMII,pAction),RT_ERR_OK);
    ASSERT_EQ(rtk_acl_igrUnmatchAction_set(RTK_RG_PORT_CPU,pAction),RT_ERR_OK);

 
    /*init TEMPLATE & field selector*/

#ifdef CONFIG_APOLLO_MODEL
#else
	//setup field selector[03] for IPv6 Src[128:121]
    bzero(&fieldSel,sizeof(fieldSel));
	fieldSel.index = 3;
	fieldSel.format = ACL_FORMAT_IPV6;
	fieldSel.offset = 8;
	if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
		return ret;

	//setup field selector[04] for aclFilter "DSCP"
    bzero(&fieldSel,sizeof(fieldSel));
    fieldSel.index = 4;
    fieldSel.format = ACL_FORMAT_IPV4;
    fieldSel.offset = 0;//will get 16 bits (version(4) + headerLength(4) + Tos(8))
    if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
        return ret;


	//setup field selector[15] for liteRomeDriver "PPPoE SessionID"
	bzero(&fieldSel,sizeof(fieldSel));
	fieldSel.index = 15;
	fieldSel.format = ACL_FORMAT_RAW;
	fieldSel.offset = 16;//pass DA, SA , Ether_Type, PPPoE Header(0x8864), PPPoE Ethernet frame
	if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
		return ret;
#endif


    //aclTemplate[0]
    bzero((void*) &aclTemplate, sizeof(aclTemplate));
    aclTemplate.index = 0;
    aclTemplate.fieldType[0] = ACL_FIELD_DMAC0;
    aclTemplate.fieldType[1] = ACL_FIELD_DMAC1;
    aclTemplate.fieldType[2] = ACL_FIELD_DMAC2;
	aclTemplate.fieldType[3] = ACL_FIELD_EXT_PORTMASK;
    aclTemplate.fieldType[4] = ACL_FIELD_SMAC0;
    aclTemplate.fieldType[5] = ACL_FIELD_SMAC1;
    aclTemplate.fieldType[6] = ACL_FIELD_SMAC2;
    aclTemplate.fieldType[7] = ACL_FIELD_ETHERTYPE;
    if((ret = rtk_acl_template_set(&aclTemplate))!= RT_ERR_OK)
        return ret;

	//rtlglue_printf("%s(%d):set template[0] done \n",__func__,__LINE__);



    //aclTemplate[1]
    bzero((void*) &aclTemplate, sizeof(aclTemplate));
    aclTemplate.index = 1;
    aclTemplate.fieldType[0] = ACL_FIELD_USER_DEFINED01;//l4_dport
    aclTemplate.fieldType[1] = ACL_FIELD_IPV4_SIP0;
    aclTemplate.fieldType[2] = ACL_FIELD_IPV4_SIP1;
    aclTemplate.fieldType[3] = ACL_FIELD_USER_DEFINED00;//l4_sport
    aclTemplate.fieldType[4] = ACL_FIELD_USER_DEFINED14;//IP_protocal
    aclTemplate.fieldType[5] = ACL_FIELD_PORT_RANGE;
    aclTemplate.fieldType[6] = ACL_FIELD_IPV4_DIP0;
    aclTemplate.fieldType[7] = ACL_FIELD_IPV4_DIP1;
    if((ret = rtk_acl_template_set(&aclTemplate))!= RT_ERR_OK)
        return ret;
	
    //rtlglue_printf("%s(%d):set template[1] done \n",__func__,__LINE__);



#ifdef CONFIG_APOLLO_MODEL
#else
	//aclTemplate[2]
    bzero((void*) &aclTemplate, sizeof(aclTemplate));
    aclTemplate.index = 2;
    aclTemplate.fieldType[0] = ACL_FIELD_CTAG;
    aclTemplate.fieldType[1] = ACL_FIELD_GEMPORT; //Abel team patch
    aclTemplate.fieldType[2] = ACL_FIELD_USER_DEFINED02;//RESERVED TEMPLATE FIELD
    aclTemplate.fieldType[3] = ACL_FIELD_USER_DEFINED03;//SIPv6[127:112]
    aclTemplate.fieldType[4] = ACL_FIELD_USER_DEFINED04;//DSCP
    aclTemplate.fieldType[5] = ACL_FIELD_IPV6_SIP1;//SIPv6[31:16]
    aclTemplate.fieldType[6] = ACL_FIELD_IPV6_SIP0;//SIPv6[15:0]
    aclTemplate.fieldType[7] = ACL_FIELD_USER_DEFINED15;//SessionID
    if((ret = rtk_acl_template_set(&aclTemplate))!= RT_ERR_OK)
        return ret;

#endif

#ifdef CONFIG_APOLLO_MODEL
#else
	//aclTemplate[3]
    bzero((void*) &aclTemplate, sizeof(aclTemplate));
    aclTemplate.index = 3;
    aclTemplate.fieldType[0] = ACL_FIELD_USER_DEFINED08;//DIPv6[47:32]
    aclTemplate.fieldType[1] = ACL_FIELD_USER_DEFINED09;//DIPv6[63:48]
    aclTemplate.fieldType[2] = ACL_FIELD_USER_DEFINED10;//DIPv6[79:64]
    aclTemplate.fieldType[3] = ACL_FIELD_USER_DEFINED11;//DIPv6[95:80]
    aclTemplate.fieldType[4] = ACL_FIELD_USER_DEFINED12;//DIPv6[111:96]
    aclTemplate.fieldType[5] = ACL_FIELD_USER_DEFINED13;//DIPv6[127:112] 
    aclTemplate.fieldType[6] = ACL_FIELD_IPV6_DIP1;//DIPv6[31:16]
    aclTemplate.fieldType[7] = ACL_FIELD_IPV6_DIP0;//DIPv6[15:0] 
    if((ret = rtk_acl_template_set(&aclTemplate))!= RT_ERR_OK)
        return ret;
#endif

#if 0

	rtlglue_printf("%s(%d):dump fieldSelect:\n",__func__,__LINE__);
	for(i=0;i<16;i++){
		bzero(&fieldSel,sizeof(fieldSel));
		fieldSel.index = i;
        if((ret = rtk_acl_fieldSelect_get(&fieldSel))!= RT_ERR_OK)
            return ret;
		else
			rtlglue_printf("%s(%d):fieldSelect[%d]:format=%d  offset=%d \n",__func__,__LINE__,fieldSel.index,fieldSel.format,fieldSel.offset);
	}
#endif


    /*init IP_RNG_TABLE*/
    bzero((void*) &iprangeEntry, sizeof(iprangeEntry));
    for(i =0; i<MAX_ACL_IPRANGETABLE_SIZE; i++)
    {
        iprangeEntry.index=i;
        if((ret = rtk_acl_ipRange_set(&iprangeEntry))!= RT_ERR_OK)
            return ret;
    }


    /*init PORT_RNG_TABLE*/
    bzero((void*) &prtRangeEntry, sizeof(prtRangeEntry));
    for(i =0; i<MAX_ACL_PORTRANGETABLE_SIZE; i++)
    {
        prtRangeEntry.index=i;
        if((ret = rtk_acl_portRange_set(&prtRangeEntry))!= RT_ERR_OK)
            return ret;
    }

	/*reserve for SessionID to VID_remarking*/
	for(i=0;i<MAX_NETIF_HW_TABLE_SIZE;i++)
		rg_db.systemGlobal.aclEntry_for_pppoe_passthrought_downstream_perIntf_permit[i]=0;


	for(i=0;i<RESERVED_ACL_EXTPORT_TRANSLATE_SIZE;i++)
		rg_db.systemGlobal.aclEntry_for_extPortTranslate[i]=-1;



	assert_ok(_rtk_rg_acl_reserved_multicastVidTranslate());
#if 0
	/*init default rule for change multicast packet to vid=1 (Let it across lan/wan)*/
	{
		rtk_acl_ingress_entry_t aclRule;
		rtk_acl_field_t aclField1;
		rtk_acl_field_t aclField2;

		//add acl[50] for reamrking multicast packet to vid=1 
		bzero(&aclRule,sizeof(aclRule));
		bzero(&aclField1,sizeof(aclField1));
		bzero(&aclField2,sizeof(aclField2));
		aclRule.valid=ENABLED;
		aclRule.index=RESERVED_ACL_MULTICAST_VID_TRANSLATE;
			
		aclField1.fieldType = ACL_FIELD_PATTERN_MATCH;
		aclField1.fieldUnion.pattern.fieldIdx = 1;//template[0],field[1] = DA[31:16]
		aclField1.fieldUnion.data.value=0x5e00;
		aclField1.fieldUnion.data.mask=0xff00;
		assert_ok(rtk_acl_igrRuleField_add(&aclRule, &aclField1));

		aclField2.fieldType = ACL_FIELD_PATTERN_MATCH;
		aclField2.fieldUnion.pattern.fieldIdx = 2;//template[0],field[2] = DA[47:32]
		aclField2.fieldUnion.data.value=0x0100;
		aclField2.fieldUnion.data.mask=0xffff;
		assert_ok(rtk_acl_igrRuleField_add(&aclRule, &aclField2));

			
		aclRule.activePorts.bits[0]=0x7f;
		aclRule.templateIdx=0; /*use DA: template[],field[0~2]*/
		aclRule.act.enableAct[ACL_IGR_CVLAN_ACT]=ENABLED;
		aclRule.act.cvlanAct.act=ACL_IGR_CVLAN_IGR_CVLAN_ACT;
		aclRule.act.cvlanAct.cvid=1; //remark to 1
		assert_ok(rtk_acl_igrRuleEntry_add(&aclRule));		

	}
#endif

    return RT_ERR_RG_OK;

}

int _rtk_rg_classify_asic_init(void)
{
    int i,ret;
	rtk_classify_rangeCheck_l4Port_t prtRngEntry;
    rtk_classify_rangeCheck_ip_t ipRngEntry;
    /*clear port_range_check table*/
    bzero((void*) &prtRngEntry, sizeof(prtRngEntry));
    for(i =0; i<MAX_CF_PORTRANGETABLE_SIZE; i++)
    {
        prtRngEntry.index=i;
        prtRngEntry.lowerPort= 0xffff;
        if((ret = rtk_classify_portRange_set(&prtRngEntry))!= RT_ERR_OK)
            return ret;
    }

    /*clear ip_range_check table*/
    bzero((void*) &ipRngEntry, sizeof(ipRngEntry));
    ipRngEntry.lowerIp=0xffffffff;
    for(i =0; i<MAX_CF_IPRANGETABLE_SIZE; i++)
    {
        ipRngEntry.index=i;
        if((ret = rtk_classify_ipRange_set(&ipRngEntry))!= RT_ERR_OK)
            return ret;
    }

    /*clear cfPri2Dscp table*/
    for(i =0; i<MAX_CF_DSCPTABLE_SIZE; i++)
    {
        if((ret = rtk_classify_cfPri2Dscp_set(i,0))!= RT_ERR_OK)
            return ret;
    }

    return RT_ERR_RG_OK;
}


int32 rtk_rg_driverVersion_get(rtk_rg_VersionString_t *version_string)
{
    //Check the parameter
    if(version_string == NULL)
        return RT_ERR_RG_NULL_POINTER;

    //Clear and initialization
    memset(version_string->version_string, 0, sizeof(rtk_rg_VersionString_t));

    //Return the version code    
	sprintf(version_string->version_string,"Lunar:%s Switch:%s RG:%s User:%s",LUNAR_SVN_VERSION,SWITCH_SVN_VERSION,ROMEDRIVER_SVN_VERSION,USER_SVN_VERSION);
    
    //sprintf(version_string->version_string,"%s",ROMEDRIVER_VERSION);


    return RT_ERR_RG_OK;
}

int32 rtk_rg_initParam_get(rtk_rg_initParams_t *init_param)
{
	//Check the parameter
	if(init_param == NULL)
		return RT_ERR_RG_NULL_POINTER;

	//Copy from rg_db
	memcpy(init_param, &rg_db.systemGlobal.initParam, sizeof(rtk_rg_initParams_t));
#if 0
	//Checking for parameters initialized
	if(rg_db.systemGlobal.initParam.arpAddByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.arpDelByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.macAddByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.macDelByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.naptAddByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.naptDelByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.routingAddByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.routingDelByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.bindingAddByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.bindingDelByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.naptInboundConnLookupFirstCallBack == NULL &&
			rg_db.systemGlobal.initParam.naptInboundConnLookupSecondCallBack == NULL &&
			rg_db.systemGlobal.initParam.naptInboundConnLookupThirdCallBack == NULL &&
			rg_db.systemGlobal.initParam.interfaceAddByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.interfaceDelByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.neighborAddByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.neighborDelByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.v6RoutingAddByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.v6RoutingDelByHwCallBack == NULL &&
			rg_db.systemGlobal.initParam.pppoeBeforeDiagByHwCallBack == NULL)
		return RT_ERR_RG_INITPM_UNINIT;

	//Return each function pointer
	init_param->arpAddByHwCallBack = rg_db.systemGlobal.initParam.arpAddByHwCallBack;
	init_param->arpDelByHwCallBack = rg_db.systemGlobal.initParam.arpDelByHwCallBack;
	init_param->macAddByHwCallBack = rg_db.systemGlobal.initParam.macAddByHwCallBack;
	init_param->macDelByHwCallBack = rg_db.systemGlobal.initParam.macDelByHwCallBack;
	init_param->naptAddByHwCallBack = rg_db.systemGlobal.initParam.naptAddByHwCallBack;
	init_param->naptDelByHwCallBack = rg_db.systemGlobal.initParam.naptDelByHwCallBack;
	init_param->routingAddByHwCallBack = rg_db.systemGlobal.initParam.routingAddByHwCallBack;
	init_param->routingDelByHwCallBack = rg_db.systemGlobal.initParam.routingDelByHwCallBack;
	init_param->bindingAddByHwCallBack = rg_db.systemGlobal.initParam.bindingAddByHwCallBack;
	init_param->bindingDelByHwCallBack = rg_db.systemGlobal.initParam.bindingDelByHwCallBack;
	init_param->naptInboundConnLookupFirstCallBack = rg_db.systemGlobal.initParam.naptInboundConnLookupFirstCallBack;
	init_param->naptInboundConnLookupSecondCallBack = rg_db.systemGlobal.initParam.naptInboundConnLookupSecondCallBack;
	init_param->naptInboundConnLookupThirdCallBack = rg_db.systemGlobal.initParam.naptInboundConnLookupThirdCallBack;
	init_param->interfaceAddByHwCallBack = rg_db.systemGlobal.initParam.interfaceAddByHwCallBack;
	init_param->interfaceDelByHwCallBack = rg_db.systemGlobal.initParam.interfaceDelByHwCallBack;
	init_param->neighborAddByHwCallBack = rg_db.systemGlobal.initParam.neighborAddByHwCallBack;
	init_param->neighborDelByHwCallBack = rg_db.systemGlobal.initParam.neighborDelByHwCallBack;
	init_param->v6RoutingAddByHwCallBack = rg_db.systemGlobal.initParam.v6RoutingAddByHwCallBack;
	init_param->v6RoutingDelByHwCallBack = rg_db.systemGlobal.initParam.v6RoutingDelByHwCallBack;
	init_param->pppoeBeforeDiagByHwCallBack = rg_db.systemGlobal.initParam.pppoeBeforeDiagByHwCallBack;
#endif
	return RT_ERR_RG_OK;
}


int32 rtk_rg_initParam_set(rtk_rg_initParams_t *init_param)
{
    int ret,i;
    rtk_portmask_t mbpmsk, utpmsk, etpmsk;
#ifdef CONFIG_RG_WLAN_HWNAT_ACCELERATION
	rtk_portmask_t srcExtPortFilterMmsk;
#endif
	rtk_rg_macEntry_t macEt;
	rtk_l34_routing_entry_t rtEntry;
	rtk_vlan_protoGroup_t protoGroupCfg;
	rtk_enable_t vlanFiltering;
	rtk_port_macAbility_t cpuAbility;
	rtk_rg_ipv4MulticastFlow_t ipv4Mc;
    //rtk_classify_cfg_t cfEntry;

    //Checking for input parameter - if here we pass NULL, means we just reset the Global variables
    //if(init_param == NULL)
    //return RT_ERR_RG_NULL_POINTER;

	//=============== Clear all rg_db variables =========================
	ASSERT_EQ(_rtk_rg_globalVariableReset(),RT_ERR_RG_OK);
#ifdef CONFIG_APOLLO_ROMEDRIVER
	ASSERT_EQ(_rtk_rg_fwdEngineGlobalVariableReset(),RT_ERR_RG_OK);
#endif

    /* Initialize SDK */
    //rtlglue_printf("RTK RG initialize.....%d\n",RG_GLB_VLAN_INIT);

    ASSERT_EQ(rtk_init(),RT_ERR_OK);
	ASSERT_EQ(rtk_l34_init(),RT_ERR_OK);
	ASSERT_EQ(rtk_l2_init(),RT_ERR_OK);		
	ASSERT_EQ(rtk_l2_addr_delAll(ENABLED),RT_ERR_OK);
#ifdef CONFIG_APOLLO_TESTING
	// inited by rtk_rg_module_init, do not do rtk_init() again!!! it will make virtualmac disabled!
	if(virtualmacEnable==ENABLE)
	{
		apollo_mac_init();	
	}
#endif

#ifdef CONFIG_APOLLO_MODEL	
#else
	//HSD debug
	assert_ok(rtk_l34_hsdState_set(ENABLED));
#endif

#ifdef CONFIG_RG_NAPT_AUTO_AGEOUT
	//Clear traffic
	rtk_l34_hwL4TrfWrkTbl_Clear(0);
	rtk_l34_hwL4TrfWrkTbl_Clear(1);
#endif	

#ifdef CONFIG_RTL_IGMP_SNOOPING
	//igmp init 
	for(i=0;i<RTK_RG_MAC_PORT_CPU;i++)
	{
		if(i==RTK_RG_MAC_PORT_PON) continue;
		ASSERT_EQ(rtk_trap_portIgmpMldCtrlPktAction_set(i,IGMPMLD_TYPE_IGMPV1,ACTION_TRAP2CPU),RT_ERR_OK);
		ASSERT_EQ(rtk_trap_portIgmpMldCtrlPktAction_set(i,IGMPMLD_TYPE_IGMPV2,ACTION_TRAP2CPU),RT_ERR_OK);
		ASSERT_EQ(rtk_trap_portIgmpMldCtrlPktAction_set(i,IGMPMLD_TYPE_IGMPV3,ACTION_TRAP2CPU),RT_ERR_OK);
		ASSERT_EQ(rtk_trap_portIgmpMldCtrlPktAction_set(i,IGMPMLD_TYPE_MLDV1,ACTION_TRAP2CPU),RT_ERR_OK);
		ASSERT_EQ(rtk_trap_portIgmpMldCtrlPktAction_set(i,IGMPMLD_TYPE_MLDV2,ACTION_TRAP2CPU),RT_ERR_OK);
	}
#endif

	//Init ACL Template & Field Selector & RangeTable Value 
	ASSERT_EQ(_rtk_rg_acl_asic_init(),RT_ERR_RG_OK);
#ifdef CONFIG_APOLLO_RLE0371
#else
	//Init Classify RangeTable Value	
	ASSERT_EQ(_rtk_rg_classify_asic_init(),RT_ERR_RG_OK);
#endif
	//init acllFilter for SW maintain info
	ASSERT_EQ(_rtk_rg_aclSWEntry_init(),RT_ERR_RG_OK);

	//init stormControl  for SW maintain info
	ASSERT_EQ(_rtk_rg_stormControlEntry_init(),RT_ERR_RG_OK);

	//init urlFilter for SW maintain info
	ASSERT_EQ(_rtk_rg_urlFilter_table_init(),RT_ERR_RG_OK);

	//init macFilter for SW maintain info
	ASSERT_EQ(_rtk_rg_macFilter_table_init(),RT_ERR_RG_OK);
	
#ifdef CONFIG_RG_LAYER2_SOFTWARE_LEARN
	//clear link-down indicator from the beginning
	ASSERT_EQ(rtk_intr_linkdownStatus_clear(),RT_ERR_OK);
	/*register link-change ISR bh handler*/
	ASSERT_EQ(rtk_irq_isr_register(INTR_TYPE_LINK_CHANGE,_rtk_rg_switchLinkChangeHandler),RT_ERR_OK);
	//turn on link-change ISR mask
	ASSERT_EQ(rtk_intr_imr_set(INTR_TYPE_LINK_CHANGE,ENABLED),RT_ERR_OK);

	//Turn off Lut auto-learning
	for(i=0;i<RTK_RG_MAC_PORT_CPU;i++)		//CPU port will use auto-learning, and do not turn on DMAC2CViD function
	{
		ASSERT_EQ(rtk_l2_portLimitLearningCnt_set(i,0),RT_ERR_OK);
		ASSERT_EQ(rtk_l2_portLimitLearningCntAction_set(i,LIMIT_LEARN_CNT_ACTION_TO_CPU),RT_ERR_OK);	
		ASSERT_EQ(rtk_svlan_dmacVidSelState_set(i, DISABLED), RT_ERR_OK);	//FIXME: we should check global variable setting for this!!
	}
	//Turn off CPU port LUT auto-learning, and set Action to Forward
	ASSERT_EQ(rtk_l2_portLimitLearningCnt_set(RTK_RG_MAC_PORT_CPU,0),RT_ERR_OK);
	ASSERT_EQ(rtk_l2_portLimitLearningCntAction_set(RTK_RG_MAC_PORT_CPU,LIMIT_LEARN_CNT_ACTION_FORWARD),RT_ERR_OK);
#endif

	//Turn on Forced_DMAC2CVID
#ifdef CONFIG_APOLLO_RLE0371				
#else
	ASSERT_EQ(rtk_svlan_dmacVidSelForcedState_set(DISABLED), RT_ERR_OK);		//FIXME: we should check global variable setting for this!!
#endif

	//=============== Global variables initilization =========================	

	//ALG initialization
	i=0;
	rg_db.algServInLanIpMapping[i++].algType=RTK_RG_ALG_SIP_TCP_SRV_IN_LAN_BIT;
	rg_db.algServInLanIpMapping[i++].algType=RTK_RG_ALG_SIP_UDP_SRV_IN_LAN_BIT;
	rg_db.algServInLanIpMapping[i++].algType=RTK_RG_ALG_H323_TCP_SRV_IN_LAN_BIT;
	rg_db.algServInLanIpMapping[i++].algType=RTK_RG_ALG_H323_UDP_SRV_IN_LAN_BIT;
	rg_db.algServInLanIpMapping[i++].algType=RTK_RG_ALG_RTSP_TCP_SRV_IN_LAN_BIT;
	rg_db.algServInLanIpMapping[i++].algType=RTK_RG_ALG_RTSP_UDP_SRV_IN_LAN_BIT;
	rg_db.algServInLanIpMapping[i++].algType=RTK_RG_ALG_FTP_TCP_SRV_IN_LAN_BIT;
	rg_db.algServInLanIpMapping[i++].algType=RTK_RG_ALG_FTP_UDP_SRV_IN_LAN_BIT;
	
	for(i=0;i<MAX_NETIF_SW_TABLE_SIZE;i++)
	{
		//bzero(&rg_db.systemGlobal.interfaceInfo[i], sizeof(rtk_rg_interface_info_global_t));
		//rg_db.systemGlobal.interfaceInfo[i].p_lanIntfConf=NULL;
		//rg_db.systemGlobal.interfaceInfo[i].p_wanStaticInfo=NULL;
		
		//rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo=NULL;
		//rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo=NULL;
		
		rg_db.systemGlobal.intfArpRequest[i].finished = 1;
		rg_db.systemGlobal.intfNeighborDiscovery[i].finished = 1;
	}
	/*for(i=0;i<MAX_BIND_HW_TABLE_SIZE;i++)
	{
		rg_db.systemGlobal.bindToIntf[i]=-1;
		rg_db.systemGlobal.bindWithVLAN[i]=-1;
	}*/

	for(i=0;i<RTK_RG_PORT_MAX;i++)
		rg_db.systemGlobal.portBasedVID[i]=DEFAULT_CPU_VLAN;		//reset port-based VLAN in rg_db
		
	for(i=0;i<RTK_RG_PORT_MAX;i++)
		rg_db.systemGlobal.sourceAddrLearningLimitNumber[i]=2048;		//reset port-based SA learning limit (software learning)

#if 0
	for(i=0;i<STATIC_DHCP_ALLOC_NUM;i++)
		_DHCP_STATIC[i].valid=0;
#endif

	/* Virtual Server initialization 
	for(i=0;i<MAX_VIRTUAL_SERVER_SW_TABLE_SIZE;i++)
	{
		rg_db.systemGlobal.virtualServerGroup[i].p_virtualServer = NULL;
	}*/
	/* UPNP initialization 
	for(i=0;i<MAX_UPNP_SW_TABLE_SIZE;i++)
	{
		rg_db.systemGlobal.upnpGroup[i].p_upnp = NULL;
	}*/

	//rg_db.systemGlobal.lanIntfTotalNum = 0;			//how many LAN interface added
	//rg_db.systemGlobal.wanIntfTotalNum = 0;			//how many WAN interface added
	//rg_db.systemGlobal.wanInfoSet = 0;			//which WAN interface had been set
	//rg_db.systemGlobal.vlanBindTotalNum = 0;
	//rg_db.systemGlobal.pppoeBeforeCalled = 0;
	rg_db.systemGlobal.defaultRouteSet = -1;		//keep which interface is default route
	rg_db.systemGlobal.defaultIPV6RouteSet = -1;	//keep which interface is IPv6 default route
	rg_db.systemGlobal.wanIdxForReset = -1;			//only use when wan need reset
	//rg_db.systemGlobal.virtualServerTotalNum = 0;
	//rg_db.systemGlobal.upnpTotalNum = 0;
	rg_db.systemGlobal.p_routingArpInfoArray=rg_db.systemGlobal.routingArpInfoArray_1;
	rg_db.systemGlobal.p_tempRoutingArpInfoArray=rg_db.systemGlobal.routingArpInfoArray_2;
	rg_db.systemGlobal.p_routingVlanInfoArray=rg_db.systemGlobal.routingVlanInfoArray_1;	//store interface vlan id
	rg_db.systemGlobal.p_tempRoutingVlanInfoArray=rg_db.systemGlobal.routingVlanInfoArray_2;

/*
	//Set up PPPoE pass through Protocol Group
	protoGroupCfg.frametype=FRAME_TYPE_ETHERNET;
	protoGroupCfg.framevalue=0x8863;				//PPPoE discovery stage
	ret = rtk_vlan_protoGroup_set(PPPOE_DISCOVERY_GROUPID,&protoGroupCfg);
	if(ret!=RT_ERR_OK)return ret;

	protoGroupCfg.frametype=FRAME_TYPE_ETHERNET;
	protoGroupCfg.framevalue=0x8864;				//PPPoE session stage
	ret = rtk_vlan_protoGroup_set(PPPOE_SESSION_GROUPID,&protoGroupCfg);
	if(ret!=RT_ERR_OK)return ret;
*/
	//Set up Protocol Group for IPv4/v6 separately
	protoGroupCfg.frametype=FRAME_TYPE_ETHERNET;
	protoGroupCfg.framevalue=RG_IPV4_ETHERTYPE;				//IPv4
	ret = RTK_VLAN_PROTOGROUP_SET(RG_IPV4_GROUPID,&protoGroupCfg);
	if(ret!=RT_ERR_OK)return ret;
	protoGroupCfg.frametype=FRAME_TYPE_ETHERNET;
	protoGroupCfg.framevalue=RG_ARP_ETHERTYPE;				//ARP
	ret = RTK_VLAN_PROTOGROUP_SET(RG_ARP_GROUPID,&protoGroupCfg);
	if(ret!=RT_ERR_OK)return ret;
	protoGroupCfg.frametype=FRAME_TYPE_ETHERNET;
	protoGroupCfg.framevalue=RG_IPV6_ETHERTYPE;				//IPv6
	ret = RTK_VLAN_PROTOGROUP_SET(RG_IPV6_GROUPID,&protoGroupCfg);
	if(ret!=RT_ERR_OK)return ret;

	//Set up CPU VLAN
	ret = RTK_VLAN_CREATE(DEFAULT_CPU_VLAN);
	if(ret == RT_ERR_NOT_INIT)
	{
		//Initialize VLAN module
		ret = rtk_vlan_init();
		if(ret!=RT_ERR_OK)return RT_ERR_RG_NOT_INIT;
		ret = RTK_VLAN_CREATE(DEFAULT_CPU_VLAN);
		if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
	}
	else if (ret == RT_ERR_VLAN_EXIST)
	{
		//DEBUG("DEFAULT_CPU_VLAN had created..");
		rg_db.vlan[DEFAULT_CPU_VLAN].valid = 1;
	}
	mbpmsk.bits[0]=0x7f;	//all port
	utpmsk.bits[0]=0x7f;	//all untag
	etpmsk.bits[0]=0x3f;	//all extension port
	
	ret = RTK_VLAN_FID_SET(DEFAULT_CPU_VLAN, LAN_FID);
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
	ret = RTK_VLAN_FIDMODE_SET(DEFAULT_CPU_VLAN, VLAN_FID_SVL);		//This is used for ALL LAN interface
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
	ret = RTK_VLAN_PORT_SET(DEFAULT_CPU_VLAN, &mbpmsk, &utpmsk);
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
	ret = RTK_VLAN_EXTPORT_SET(DEFAULT_CPU_VLAN, &etpmsk);
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;

	//Set up LAN SVL VLAN for multicast forwarding
	ret = RTK_VLAN_CREATE(DEFAULT_LAN_VLAN);
	if(ret==RT_ERR_VLAN_EXIST)
		rg_db.vlan[DEFAULT_LAN_VLAN].valid = 1;
	else if(ret!=RT_ERR_OK)
		return RT_ERR_RG_VLAN_SET_FAIL;
	mbpmsk.bits[0]=(0x1<<RTK_RG_MAC_PORT_CPU);	//CPU port
	utpmsk.bits[0]=0x7f;						//all untag
	etpmsk.bits[0]=0x1;							//extension CPU port
	
	ret = RTK_VLAN_FID_SET(DEFAULT_LAN_VLAN, LAN_FID);
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
	ret = RTK_VLAN_FIDMODE_SET(DEFAULT_LAN_VLAN, VLAN_FID_SVL);		//This is used for ALL LAN interface
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
	ret = RTK_VLAN_PORT_SET(DEFAULT_LAN_VLAN, &mbpmsk, &utpmsk);
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
	ret = RTK_VLAN_EXTPORT_SET(DEFAULT_LAN_VLAN, &etpmsk);
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;

	//Set up default WAN PVID
	ret = RTK_VLAN_CREATE(DEFAULT_WAN_PVID);
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
	mbpmsk.bits[0]=1<<RTK_RG_PORT_CPU;	//CPU port
	utpmsk.bits[0]=0x7f;				//all untag
	etpmsk.bits[0]=0x1;					//extension CPU port
	
	ret = RTK_VLAN_FID_SET(DEFAULT_WAN_PVID, LAN_FID);
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
	ret = RTK_VLAN_FIDMODE_SET(DEFAULT_WAN_PVID, VLAN_FID_SVL);		//This is used for ALL LAN interface
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
	ret = RTK_VLAN_PORT_SET(DEFAULT_WAN_PVID, &mbpmsk, &utpmsk);
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
	ret = RTK_VLAN_EXTPORT_SET(DEFAULT_WAN_PVID, &etpmsk);
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
	
#ifdef CONFIG_APOLLO_RLE0371
	//Patch for multicast failure problem
	vlanFiltering=DISABLE;
#else
	vlanFiltering=ENABLE;
#endif
	ret = rtk_vlan_vlanFunctionEnable_set(vlanFiltering);			//vlan igr/egr filter switch
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;

	for(i=0;i<RTK_RG_MAX_MAC_PORT;i++)
	{
		ret = rtk_vlan_portIgrFilterEnable_set(i, vlanFiltering);
		if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
		ret = RTK_VLAN_PORTPVID_SET(i, DEFAULT_CPU_VLAN);
		if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;		
	}
	for(i=RTK_RG_EXT_PORT0;i<RTK_RG_PORT_MAX;i++)
	{
		ret = RTK_VLAN_EXTPORTPVID_SET(i-RTK_RG_PORT_CPU,DEFAULT_CPU_VLAN);
		if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
	}
	
	ASSERT_EQ(rtk_l34_globalState_set(L34_GLOBAL_L34_STATE,ENABLED),RT_ERR_OK);		
	ASSERT_EQ(rtk_l34_globalState_set(L34_GLOBAL_L3NAT_STATE,ENABLED),RT_ERR_OK);
	ASSERT_EQ(rtk_l34_globalState_set(L34_GLOBAL_L4NAT_STATE,ENABLED),RT_ERR_OK);
					
#ifdef CONFIG_APOLLO_RLE0371
	//patch for 0371 icmp checksum error bug
	ASSERT_EQ(rtk_l34_globalState_set(L34_GLOBAL_L3CHKSERRALLOW_STATE,ENABLED),RT_ERR_OK);		
	ASSERT_EQ(rtk_l34_globalState_set(L34_GLOBAL_L4CHKSERRALLOW_STATE,ENABLED),RT_ERR_OK);

	//patch for 0371 CRC error (must remarking)
	for(i=0;i<RTK_RG_MAX_MAC_PORT;i++)
	{
		ret=rtk_qos_dscpRemarkEnable_set(i, ENABLED);
		if(ret!=RT_ERR_OK) FIXME("ret=%d\n",ret);	

		ret=rtk_qos_portDscpRemarkSrcSel_set(i,DSCP_RMK_SRC_DSCP); //set dscp remark mode (0:by intpri/1:by dscp/2:by userpri) 	
		if(ret!=RT_ERR_OK) FIXME("ret=%d\n",ret);				
	}		

	for(i=0;i<64;i++)
	{
		ret=rtk_qos_dscp2DscpRemarkGroup_set(0,i,i); //old dscp i --> new dscp i
		if(ret!=RT_ERR_OK) FIXME("ret=%d\n",ret);
	}		

#else
	ASSERT_EQ(rtk_l34_globalState_set(L34_GLOBAL_L3CHKSERRALLOW_STATE,DISABLED),RT_ERR_OK);		
	ASSERT_EQ(rtk_l34_globalState_set(L34_GLOBAL_L4CHKSERRALLOW_STATE,DISABLED),RT_ERR_OK);
	
	ASSERT_EQ(rtk_l34_globalState_set(L34_GLOBAL_BIND_STATE,ENABLED),RT_ERR_OK);		//turn on binding for 0601
	ASSERT_EQ(rtk_l34_globalState_set(L34_GLOBAL_PPPKEEP_STATE,DISABLED),RT_ERR_OK);	//turn off PPPoE keep in default
	
	//set up PON and RGMII enable for binding
	ASSERT_EQ(rtk_classify_cfSel_set(RTK_RG_MAC_PORT_PON, CLASSIFY_CF_SEL_ENABLE),RT_ERR_OK);
	ASSERT_EQ(rtk_classify_cfSel_set(RTK_RG_MAC_PORT_RGMII, CLASSIFY_CF_SEL_ENABLE),RT_ERR_OK);

	//initialize unmatch action register
	ASSERT_EQ(rtk_l34_bindingAction_set(L34_BIND_UNMATCHED_L2L3,L34_BIND_ACT_DROP),RT_ERR_OK);
	ASSERT_EQ(rtk_l34_bindingAction_set(L34_BIND_UNMATCHED_L2L34,L34_BIND_ACT_DROP),RT_ERR_OK);

	ASSERT_EQ(rtk_l34_bindingAction_set(L34_BIND_UNMATCHED_L3L2,L34_BIND_ACT_DROP),RT_ERR_OK);
	ASSERT_EQ(rtk_l34_bindingAction_set(L34_BIND_UNMATCHED_L3L3,L34_BIND_ACT_FORCE_BINDL3),RT_ERR_OK);
	ASSERT_EQ(rtk_l34_bindingAction_set(L34_BIND_UNMATCHED_L3L34,L34_BIND_ACT_IPV4_LOOKUPL4TABLE_IPV6_TRAP),RT_ERR_OK);

	ASSERT_EQ(rtk_l34_bindingAction_set(L34_BIND_UNMATCHED_L34L2,L34_BIND_ACT_DROP),RT_ERR_OK);
	ASSERT_EQ(rtk_l34_bindingAction_set(L34_BIND_UNMATCHED_L34L3,L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4),RT_ERR_OK);
	
	ASSERT_EQ(rtk_l34_bindingAction_set(L34_BIND_CUSTOMIZED_L2,L34_BIND_ACT_PERMIT_L2BRIDGE),RT_ERR_OK);
	ASSERT_EQ(rtk_l34_bindingAction_set(L34_BIND_CUSTOMIZED_L3,L34_BIND_ACT_FORCE_BINDL3),RT_ERR_OK);
	ASSERT_EQ(rtk_l34_bindingAction_set(L34_BIND_CUSTOMIZED_L34,L34_BIND_ACT_NORMAL_LOOKUPL34),RT_ERR_OK);
#endif
	ASSERT_EQ(rtk_l34_globalState_set(L34_GLOBAL_NAT2LOG_STATE,ENABLED),RT_ERR_OK);	
	ASSERT_EQ(rtk_l34_globalState_set(L34_GLOBAL_TTLMINUS_STATE,ENABLED),RT_ERR_OK);
	ASSERT_EQ(rtk_l34_globalState_set(L34_GLOBAL_FRAG2CPU_STATE,ENABLED),RT_ERR_OK);
	ASSERT_EQ(rtk_l34_globalState_set(L34_GLOBAL_KEEP_ORG_STATE,ENABLED),RT_ERR_OK);
	ASSERT_EQ(rtk_l34_wanRoutMode_set(L34_WANROUTE_FORWARD),RT_ERR_OK);
	ASSERT_EQ(rtk_l34_lookupMode_set(L34_LOOKUP_MAC_BASE),RT_ERR_OK);
	ASSERT_EQ(rtk_l34_hsabMode_set(L34_HSBA_LOG_ALL),RT_ERR_OK);			//turn on all hsba log

	//Turn on CPU port flow control
	ASSERT_EQ(rtk_port_macForceAbility_get(RTK_RG_MAC_PORT_CPU,&cpuAbility),RT_ERR_OK);
	cpuAbility.txFc=ENABLED;
	cpuAbility.rxFc=ENABLED;
	ASSERT_EQ(rtk_port_macForceAbility_set(RTK_RG_MAC_PORT_CPU,cpuAbility),RT_ERR_OK);
	ASSERT_EQ(rtk_port_macForceAbilityState_set(RTK_RG_MAC_PORT_CPU,ENABLED),RT_ERR_OK);

	if(ret!=RT_ERR_OK)return ret;

	//Assign parameter to global one
	/*rg_db.systemGlobal.initParam.initByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.arpAddByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.arpDelByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.macAddByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.macDelByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.naptAddByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.naptDelByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.routingAddByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.routingDelByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.bindingAddByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.bindingDelByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.interfaceAddByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.interfaceDelByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.neighborAddByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.neighborDelByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.v6RoutingAddByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.v6RoutingDelByHwCallBack=NULL;
	rg_db.systemGlobal.initParam.pppoeBeforeDiagByHwCallBack=NULL;*/
#ifdef CONFIG_RG_NAPT_UPNP_SUPPORT
	rg_db.systemGlobal.initParam.naptInboundConnLookupFirstCallBack=_rtk_rg_fwdEngine_upnpCheck;
#else
	rg_db.systemGlobal.initParam.naptInboundConnLookupFirstCallBack=NULL;
#endif
#ifdef CONFIG_RG_NAPT_VIRTUAL_SERVER_SUPPORT
	rg_db.systemGlobal.initParam.naptInboundConnLookupSecondCallBack=_rtk_rg_fwdEngine_virtualServerCheck;
#else
	rg_db.systemGlobal.initParam.naptInboundConnLookupSecondCallBack=NULL;
#endif
#ifdef CONFIG_RG_NAPT_DMZ_SUPPORT
	rg_db.systemGlobal.initParam.naptInboundConnLookupThirdCallBack=_rtk_rg_fwdEngine_dmzCheck;
#else
	rg_db.systemGlobal.initParam.naptInboundConnLookupThirdCallBack=NULL;
#endif

	if(init_param != NULL)
	{
		memcpy(&rg_db.systemGlobal.initParam,init_param,sizeof(rtk_rg_initParams_t));
	}
	else //default Init Value	
	{
		rg_db.systemGlobal.initParam.igmpSnoopingEnable=1;
	}


	if(rg_db.systemGlobal.initParam.igmpSnoopingEnable)
	{
		//printk("IGMP Snooping enable...\n");
		for(i=0;i<RTK_RG_PORT_CPU;i++)
		{
			ret=rtk_l2_portLookupMissAction_set(i,DLF_TYPE_IPMC,ACTION_DROP);
			if(ret!=RT_ERR_RG_OK) return ret;
			ret=rtk_l2_portLookupMissAction_set(i,DLF_TYPE_IP6MC,ACTION_TRAP2CPU); //to fix ICMPv6 can't to ping issue.(from LAN to Gateway)  
			if(ret!=RT_ERR_RG_OK) return ret;
			ret=rtk_l2_portLookupMissAction_set(i,DLF_TYPE_MCAST,ACTION_TRAP2CPU);
			if(ret!=RT_ERR_RG_OK) return ret;					
		}

		//Create IPMC flow for SSDP 239.255.255.250 to trap from LAN port(because if igmpSnooping is on, the LAN port's IPMC will DROP)
		memset(&ipv4Mc,0,sizeof(rtk_rg_ipv4MulticastFlow_t));
		ipv4Mc.srcFilterMode=RTK_RG_IPV4MC_DONT_CARE_SRC;
		ipv4Mc.groupIp=0xeffffffa;	//239.255.255.250
		ipv4Mc.dontCareSipModePortmask.portmask=0x1<<RTK_RG_PORT_CPU;
		ASSERT_EQ(rtk_rg_ipv4MultiCastFlow_add(&ipv4Mc,&ret),RT_ERR_RG_OK);
	}
	else
	{

		//printk("IGMP Snooping disable...\n");	
		for(i=0;i<RTK_RG_PORT_CPU;i++)
		{
			ret=rtk_l2_portLookupMissAction_set(i,DLF_TYPE_IPMC,ACTION_TRAP2CPU);
			if(ret!=RT_ERR_RG_OK) return ret;
			ret=rtk_l2_portLookupMissAction_set(i,DLF_TYPE_IP6MC,ACTION_TRAP2CPU);
			if(ret!=RT_ERR_RG_OK) return ret;
			ret=rtk_l2_portLookupMissAction_set(i,DLF_TYPE_MCAST,ACTION_TRAP2CPU);
			if(ret!=RT_ERR_RG_OK) return ret;			
		}
	}

	// flush all igmp entry
    rtl_flushAllIgmpRecord();

	// For IPMC Server in WLAN case
	//ret=rtk_l2_portLookupMissAction_set(RTK_RG_PORT_CPU,DLF_TYPE_IPMC,ACTION_FORWARD);
	ret=rtk_l2_portLookupMissAction_set(RTK_RG_PORT_CPU,DLF_TYPE_IPMC,ACTION_TRAP2CPU);
	if(ret!=RT_ERR_RG_OK) return ret;	
	//ret=rtk_l2_portLookupMissAction_set(RTK_RG_PORT_CPU,DLF_TYPE_IP6MC,ACTION_FORWARD);
	ret=rtk_l2_portLookupMissAction_set(RTK_RG_PORT_CPU,DLF_TYPE_IP6MC,ACTION_TRAP2CPU);
	if(ret!=RT_ERR_RG_OK) return ret;

	ret=rtk_l2_portLookupMissAction_set(RTK_RG_PORT_CPU,DLF_TYPE_MCAST,ACTION_TRAP2CPU);
	if(ret!=RT_ERR_RG_OK) return ret;

	mbpmsk.bits[0]=1<<RTK_RG_MAC_PORT_CPU;
	ret=rtk_l2_lookupMissFloodPortMask_set(DLF_TYPE_MCAST,&mbpmsk);		//set multicast flooding port
	if(ret!=RT_ERR_RG_OK) return ret;

	for(i=0;i<=RTK_RG_PORT_CPU;i++)
	{
		ret=rtk_trap_portIgmpMldCtrlPktAction_set(i, IGMPMLD_TYPE_IGMPV1,ACTION_TRAP2CPU) ;
		if(ret!=RT_ERR_RG_OK) return ret;
		ret=rtk_trap_portIgmpMldCtrlPktAction_set(i, IGMPMLD_TYPE_IGMPV2,ACTION_TRAP2CPU) ;
		if(ret!=RT_ERR_RG_OK) return ret;
		ret=rtk_trap_portIgmpMldCtrlPktAction_set(i, IGMPMLD_TYPE_IGMPV3,ACTION_TRAP2CPU) ;
		if(ret!=RT_ERR_RG_OK) return ret;
		ret=rtk_trap_portIgmpMldCtrlPktAction_set(i, IGMPMLD_TYPE_MLDV1,ACTION_TRAP2CPU) ;
		if(ret!=RT_ERR_RG_OK) return ret;
		ret=rtk_trap_portIgmpMldCtrlPktAction_set(i, IGMPMLD_TYPE_MLDV2,ACTION_TRAP2CPU) ;
		if(ret!=RT_ERR_RG_OK) return ret;
	}

	rg_db.systemGlobal.vlanInit = 1;

	//Set Default routing to CPU
	bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
    rtEntry.valid=1;
    rtEntry.process=L34_PROCESS_CPU;		//set all packets to CPU if WAN is default route
    
    ret = RTK_L34_ROUTINGTABLE_SET(DEFAULT_ROUTE_IDX, &rtEntry);		//set default route
    if(ret!=RT_ERR_OK)return ret;

	//Create routing to CPU for Wifi2 subnet
	bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
	rtEntry.ipAddr=0x0afdfd00;	//10.253.253.0
	rtEntry.ipMask=29;	// 2bits,  total has 4 addresses
    rtEntry.valid=1;
    rtEntry.process=L34_PROCESS_CPU;		//set all packets to CPU if WAN is default route

    ret = RTK_L34_ROUTINGTABLE_SET(SLAVE_WIFI_ROUTE_IDX, &rtEntry);		//set default route
    if(ret!=RT_ERR_OK)return ret;

	//Create dummy LUT entry for TRAP to CPU
	/*i=rg_db.systemGlobal.defaultTrapLUTIdx;
	ret = rtk_rg_macEntry_find(&macEt, &i);

	if(ret==RT_ERR_RG_OK && i==rg_db.systemGlobal.defaultTrapLUTIdx)		//delete old record if had
		rtk_rg_macEntry_del(rg_db.systemGlobal.defaultTrapLUTIdx);*/

	bzero(&macEt, sizeof(rtk_rg_macEntry_t));
	macEt.isIVL=1;		//set to IVL
	macEt.fid=0;
	macEt.vlan_id=DEFAULT_CPU_VLAN;
	macEt.port_idx=RTK_RG_PORT_CPU;
	macEt.static_entry=1;		//static for not age-out, didn't turn on ARP_USED flag for it to TRAP
	ret = rtk_rg_macEntry_add(&macEt, &rg_db.systemGlobal.defaultTrapLUTIdx);
	if(ret!=RT_ERR_RG_OK)return ret;

	//add to SA learning count
	//------------------ Critical Section start -----------------------//
	//rg_lock(&rg_kernel.saLearningLimitLock);
	atomic_inc(&rg_db.systemGlobal.sourceAddrLearningCount[RTK_RG_MAC_PORT_CPU]);
	//------------------ Critical Section End -----------------------//
	//rg_unlock(&rg_kernel.saLearningLimitLock);	
	//bzero(&RG_GLB_KEEP_DEFAULT_ROUTE, sizeof(rtk_l34_routing_entry_t));

	//Set the downstream CF rule for VLAN binding
	/*memset(&cfEntry, 0, sizeof(cfEntry));
	cfEntry.index=RG_GLB_VLAN_BINDING_CFIDX;
	cfEntry.direction=CLASSIFY_DIRECTION_DS;
    cfEntry.valid=0;		//Disabled in begining, if there is Vlan binding, it will become valid
    cfEntry.act.dsAct.cAct=CLASSIFY_DS_CACT_ADD_CTAG_8100;
    cfEntry.act.dsAct.cVidAct=CLASSIFY_DS_VID_ACT_FROM_LUT;
    cfEntry.act.dsAct.cPriAct=CLASSIFY_DS_PRI_ACT_NOP;
    cfEntry.act.dsAct.uniAct=CLASSIFY_DS_UNI_ACT_NOP;		//all port should be classified
    ret = rtk_classify_cfgEntry_add(&cfEntry);
    if(ret!=RT_ERR_OK)return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;*/


	//for Multicast PATH3 (LUT PATH3, CVID+GIP)
	#if RTK_RG_MULTICAST_MODE_MACFID	
	/* path 2*/
	ret=rtk_l2_ipmcMode_set(LOOKUP_ON_MAC_AND_VID_FID);
	if(ret!=RT_ERR_RG_OK) return ret;
	#else
	/* path 3 */
//	ret=rtk_l2_ipmcMode_set(LOOKUP_ON_DIP_AND_VID);
	ret=rtk_l2_ipmcMode_set(LOOKUP_ON_DIP_AND_SIP);



	
	if(ret!=RT_ERR_RG_OK) return ret;
	
	ret=rtk_l2_ipmcGroupLookupMissHash_set(HASH_DIP_ONLY);
	if(ret!=RT_ERR_RG_OK) return ret;
	#endif

	/* IPv4 broadcast and unicast unknown DA flooding setting: trap to CPU */
	ret=rtk_l2_lookupMissFloodPortMask_set(DLF_TYPE_BCAST,&mbpmsk);		//set broadcast
	if(ret!=RT_ERR_RG_OK) return ret;
	//ret=rtk_l2_lookupMissFloodPortMask_set(DLF_TYPE_UCAST,&mbpmsk);		//set unicast unknown DA flooding mask
	//if(ret!=RT_ERR_RG_OK) return ret;
	ret=rtk_l2_lookupMissAction_set(DLF_TYPE_UCAST,ACTION_TRAP2CPU);	//set unicast unknown DA action
	if(ret!=RT_ERR_RG_OK) return ret;
	//ret=rtk_l2_portLookupMissAction_set(RTK_RG_MAC_PORT_CPU,DLF_TYPE_UCAST,ACTION_FORWARD);		//set unicast unknown DA to flooding if from CPU
	//if(ret!=RT_ERR_RG_OK) return ret;

	/* IPv4 multicsat : hwacc (drop data stream at switch directly if no any clinet listening)*/
	//ret=rtk_l2_lookupMissAction_set(DLF_TYPE_IPMC, ACTION_DROP);
	//if(ret!=RT_ERR_RG_OK) return ret;

	/* IPv6 multicsat : 
		ICMPv6 Neighbor Solicitation is viewed as IPv6 multicsat too, so trap to CPU instead dropping.
	*/	
	//ret=rtk_l2_lookupMissAction_set(DLF_TYPE_IP6MC, ACTION_TRAP2CPU);
	//if(ret!=RT_ERR_RG_OK) return ret;

	//Set action of CPU port for IPv6 unknown multicast address to FORWARD
	//ret=rtk_l2_portLookupMissAction_set(RTK_RG_MAC_PORT_CPU,DLF_TYPE_IP6MC,ACTION_TRAP2CPU); //fixed for: neighbor solicitation forward DA=MC can't trap to CPU again.
	//if(ret!=RT_ERR_RG_OK) return ret;

	
	//add init callback to sync protocal-stack
	{
		if(rg_db.systemGlobal.initParam.initByHwCallBack != NULL)
		{
			rg_db.systemGlobal.initParam.initByHwCallBack();
		}
	}

	//set 1Q-Base Priority ReMapping to internal Priority
	for(i=0;i<8;i++)
	{
		ASSERT_EQ(rtk_qos_1pPriRemapGroup_set(0,i,i,0),SUCCESS);
	}


#ifdef CONFIG_RG_WLAN_HWNAT_ACCELERATION

	// set CPU port's SA learning limit action to TRAP
	ASSERT_EQ(rtk_l2_portLimitLearningCntAction_set(RTK_RG_MAC_PORT_CPU,LIMIT_LEARN_CNT_ACTION_TO_CPU),RT_ERR_OK);
	// disable CPU port & all extension port src filter
	srcExtPortFilterMmsk.bits[0]=0x3f;
    ASSERT_EQ(rtk_l2_srcPortEgrFilterMask_set(&srcExtPortFilterMmsk),SUCCESS);
	srcExtPortFilterMmsk.bits[0]=0; // src filter ext port by software
    ASSERT_EQ(rtk_l2_extPortEgrFilterMask_set(&srcExtPortFilterMmsk),SUCCESS);

	//set internal pri to CPU pri mapping
	for(i=0;i<8;i++)
	{
		ASSERT_EQ(rtk_qos_fwd2CpuPriRemap_set(i,i),SUCCESS);
	}

	//set pri weight
	{
		rtk_qos_priSelWeight_t weight;
		memset(&weight,0,sizeof(weight));
	    weight.weight_of_portBased=1;
	    weight.weight_of_dot1q=2;
		weight.weight_of_dscp=0; //disable DSCP pri-decision
	    weight.weight_of_acl=15;
    	weight.weight_of_lutFwd=14;
	    weight.weight_of_saBaed=13;
	    weight.weight_of_vlanBased=10;
	    weight.weight_of_svlanBased=9;
    	weight.weight_of_l4Based=11;
		ASSERT_EQ(rtk_qos_priSelGroup_set(0, &weight),SUCCESS);
		ASSERT_EQ(rtk_qos_priSelGroup_set(1, &weight),SUCCESS);
	}

	for(i=0;i<RTK_RG_MAC_PORT_MAX;i++)
	{
		ASSERT_EQ(rtk_qos_portPriMap_set(i, 3),SUCCESS); //per port use table3 for internal-pri <=> queue mapping 
	}


	ASSERT_EQ(_rtk_rg_extPMaskTranslateRedirect_add(0x2,0x8),SUCCESS); //from EXT1, destExtPortMask=0x8 (patch: NIC RX haven't EXT-SPA filed)
#ifdef CONFIG_DUALBAND_CONCURRENT	
	ASSERT_EQ(_rtk_rg_extPMaskTranslateRedirect_add(0x4,0x10),SUCCESS);//from EXT2, destExtPortMask=0x10 (patch: NIC RX haven't EXT-SPA filed)
	ASSERT_EQ(_rtk_rg_internalVidPriTranslate(CONFIG_DEFAULT_TO_SLAVE_GMAC_VID,CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI,DEFAULT_CPU_VLAN,CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI),SUCCESS); // patch for Slave GMAC packets recvice by special 1Q VID and PRI
	if(CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI==7)
	{
		ASSERT_EQ(rtk_qos_1pPriRemapGroup_set(0,CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI,6,0),SUCCESS); //patch: Pri=4 is reserved for Slave GMAC, So 1Q Pri=4 is mapping to Pri=5.		
	}
	else	
	{
		ASSERT_EQ(rtk_qos_1pPriRemapGroup_set(0,CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI,CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI+1,0),SUCCESS); //patch: Pri=4 is reserved for Slave GMAC, So 1Q Pri=4 is mapping to Pri=5.
	}

	// SET SLAVE CPU STATIC MAC ADDRESS
	{
		rtk_rg_macEntry_t macEntry;
		int entry_idx;
		memset(&macEntry,0,sizeof(macEntry));
		macEntry.arp_used=0;		
		macEntry.isIVL=0;
		_rtk_rg_str2mac(CONFIG_DEFAULT_SLAVE_IPC_MAC_ADDRESS,&macEntry.mac);		
		//printk("######### %02x:%02x:%02x:%02x:%02x:%02x ###########\n",macEntry.mac.octet[0],macEntry.mac.octet[1]
		//	,macEntry.mac.octet[2],macEntry.mac.octet[3],macEntry.mac.octet[4],macEntry.mac.octet[5]);

		macEntry.port_idx=RTK_RG_EXT_PORT1;
		macEntry.static_entry=DEFAULT_CPU_VLAN;
		macEntry.fid=LAN_FID;
		macEntry.vlan_id=0;	
		rtk_rg_macEntry_add(&macEntry,&entry_idx);
	}

	// SET MASTER CPU STATIC MAC ADDRESS
	{
		rtk_rg_macEntry_t macEntry;
		int entry_idx;
		memset(&macEntry,0,sizeof(macEntry));
		macEntry.arp_used=0;		
		macEntry.isIVL=0;
		_rtk_rg_str2mac(CONFIG_DEFAULT_MASTER_IPC_MAC_ADDRESS,&macEntry.mac);		
		//printk("######### %02x:%02x:%02x:%02x:%02x:%02x ###########\n",macEntry.mac.octet[0],macEntry.mac.octet[1]
		//	,macEntry.mac.octet[2],macEntry.mac.octet[3],macEntry.mac.octet[4],macEntry.mac.octet[5]);

		macEntry.port_idx=RTK_RG_PORT_CPU;
		macEntry.static_entry=DEFAULT_CPU_VLAN;
		macEntry.fid=LAN_FID;
		macEntry.vlan_id=0;	
		rtk_rg_macEntry_add(&macEntry,&entry_idx);
	}
	
	
#endif	

/*
	//just for debug
	{
		rtk_portmask_t mirroredRxPortmask;
		rtk_portmask_t mirroredTxPortmask;
		mirroredRxPortmask.bits[0] =(1<<RTK_RG_PORT_CPU);
		mirroredTxPortmask.bits[0] =(1<<RTK_RG_PORT_CPU);
		ASSERT_EQ(rtk_mirror_portBased_set(RTK_RG_PORT0, &mirroredRxPortmask, &mirroredTxPortmask),SUCCESS);
	}
*/	
#endif

	{
		int reg;

		//Enable PON port bandwidth control
		assert_ok(ioal_mem32_read(0x2d138,&reg));
		assert_ok(ioal_mem32_write(0x2d138,reg|0x2));

		//Enable PON port bandwidth control
		assert_ok(ioal_mem32_read(0x2308c,&reg));
		reg &= ~0xf0;
		reg |= 0xd0;
		assert_ok(ioal_mem32_write(0x2308c,reg));
	}

#ifdef RTK_RG_INGRESS_QOS_TESTING
	rg_db.systemGlobal.qos_ingress_total_rate=0;
	rg_db.systemGlobal.qos_acl_total_patch=0;
	for(i=0;i<RTK_RG_MAC_PORT_PON;i++)
	{
		int j;
		for(j=0;j<5;j++)
			rg_db.systemGlobal.qos_acl_patch[i][j]=0;
		rg_db.systemGlobal.qos_ingress_rate[i]=1048568;
		rg_db.systemGlobal.qos_ingress_total_rate+=1048568;
		rg_db.systemGlobal.qos_queue_mode[i]=0x1;
	}
	ASSERT_EQ(rtk_rg_shareMeter_set(0,1048568,0),RT_ERR_RG_OK);
#endif
	
    return RT_ERR_RG_OK;
}



int _rtk_rg_portmask_translator(rtk_rg_portmask_t in_pmask, rtk_portmask_t* out_mac_pmask, rtk_portmask_t* out_ext_pmask){
	int i;
	int EXT_CPU_PORT_flag = DISABLED;
	bzero(out_mac_pmask,sizeof(rtk_portmask_t));
	bzero(out_ext_pmask,sizeof(rtk_portmask_t));

	if(out_mac_pmask==NULL||out_ext_pmask==NULL )
		return RT_ERR_RG_NULL_POINTER;

	//set mac portmask
	for(i=0;i<RTK_RG_MAX_MAC_PORT;i++){
		if(in_pmask.portmask & (1<<i)){
			out_mac_pmask->bits[0] |= (1<<i);
		}
	}

	//set ext portmask
	for(i=0;i<RTK_RG_MAX_MAC_PORT;i++){
		if(in_pmask.portmask & (1<<(i+RTK_RG_MAX_MAC_PORT))){
			out_ext_pmask->bits[0] |= (1<<(i+1));
			EXT_CPU_PORT_flag = ENABLED;//ENABLE MAC_CPU_PORT if any EXT_PORT is ENABLED
		}
	}
	
	if(EXT_CPU_PORT_flag==ENABLED)
		out_mac_pmask->bits[0] |= (1<<RTK_RG_MAC_PORT_CPU);

	if(in_pmask.portmask & (1<<RTK_RG_MAC_PORT_CPU)){
		out_ext_pmask->bits[0] |= 1; //enable extCPU port
	}
	

	return RT_ERR_RG_OK;

}

//LAN Interface/Static Route/IPv4 DHCP Server
int _rtk_rg_softwareArpTableLookUp(unsigned short routingIdx, ipaddr_t ipAddr, rtk_rg_arp_linkList_t **pSoftwareArpEntry)
{
	rtk_rg_arp_linkList_t *pArpEntry;

	list_for_each_entry(pArpEntry,&rg_db.softwareArpTableHead[ipAddr&0xff],arp_list)
	{
		if(pArpEntry->routingIdx==routingIdx && rg_db.arp[pArpEntry->idx].ipv4Addr==ipAddr)
		{
			DEBUG("Found!ARP[%d] is match with %x",pArpEntry->idx,ipAddr);
			//Reset idle time
			rg_db.arp[pArpEntry->idx].idleSecs=0;
			*pSoftwareArpEntry=pArpEntry;
			return RT_ERR_RG_OK;
		}
	}

	//not found
	*pSoftwareArpEntry=NULL;
	return RT_ERR_RG_OK;
}

int _rtk_rg_softwareArpTableDel(rtk_rg_arp_linkList_t *pDelArpEntry)
{
	int ret,l2Idx;
	//unsigned char hashIdx;
	//rtk_rg_macEntry_t macEntry;

	l2Idx=rg_db.arp[pDelArpEntry->idx].rtk_arp.nhIdx;

	//keep original data, only toggle arp_used to 0
	if(rg_db.lut[l2Idx].valid)
	{
#if 1
		//Since ARP is not exist, the LUT is no need anymore, so we delete it
		ret=rtk_rg_macEntry_del(l2Idx);
#else
		memcpy(macEntry.mac.octet,rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN);
		macEntry.fid=rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.fid;
		macEntry.isIVL=(rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_IVL)>0?1:0;
		macEntry.port_idx=rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.port;
		macEntry.vlan_id=rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.vid;			
		macEntry.static_entry=(rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_STATIC)>0?1:0;			
		DEBUG("### disable arp_used in l2[%d]=%02x:%02x:%02x:%02x:%02x:%02x ###\n",l2Idx,macEntry.mac.octet[0],macEntry.mac.octet[1],macEntry.mac.octet[2],macEntry.mac.octet[3],macEntry.mac.octet[4],macEntry.mac.octet[5]);
		macEntry.arp_used=0;			
		ret=rtk_rg_macEntry_add(&macEntry,&l2Idx);
#endif
		assert_ok(ret);
	}

	//Delete from head list
	list_del_init(&pDelArpEntry->arp_list);

	//Clear data
	pDelArpEntry->routingIdx=-1;
	memset(&rg_db.arp[pDelArpEntry->idx],0,sizeof(rtk_rg_table_arp_t));

	//Add back to free list
	list_add(&pDelArpEntry->arp_list,&rg_db.softwareArpFreeListHead);

	return RT_ERR_RG_OK;
}

int _rtk_rg_freeRecentlyNotUsedArpList(void)
{
	unsigned short count=0;
	unsigned char i;
	rtk_rg_arp_linkList_t *pArpEntry,*pNextEntry;

	for(i=0;i<MAX_ARP_SW_TABLE_HEAD;i++)
	{
		list_for_each_entry_safe(pArpEntry,pNextEntry,&rg_db.softwareArpTableHead[i],arp_list)
		{	
			if(rg_db.arp[pArpEntry->idx].staticEntry==0 && rg_db.arp[pArpEntry->idx].idleSecs>=ARP_SW_TABLE_THRESHOLD)	//not be accessed in ARP_SW_TABLE_THRESHOLD time
			{
				_rtk_rg_softwareArpTableDel(pArpEntry);
				count++;
			}
		}
	}

	if(count==0)		//no recently not used ARP entry....
		return RT_ERR_RG_FAILED;

	return RT_ERR_RG_OK;
}

int _rtk_rg_softwareArpTableAdd(unsigned short routingIdx, ipaddr_t ipv4Addr, int l2Idx, int staticEntry)
{
	rtk_rg_arp_linkList_t *pNewArpEntry,*pNextArpEntry;

	//Check if we have not-used free arp list
	if(list_empty(&rg_db.softwareArpFreeListHead))
	{
		DEBUG("all free ARP list are allocated...");
		//Clear all recently not used entries
		if(_rtk_rg_freeRecentlyNotUsedArpList()!=RT_ERR_RG_OK)
			return RT_ERR_RG_FAILED;
	}

	//Get one from free list
	list_for_each_entry_safe(pNewArpEntry,pNextArpEntry,&rg_db.softwareArpFreeListHead,arp_list)		//just return the first entry right behind of head
	{	
		list_del_init(&pNewArpEntry->arp_list);
		break;
	}
	//DEBUG("the free ARP %p idx is %d, routing=%d",pNewArpEntry,pNewArpEntry->idx,pNewArpEntry->routingIdx);

	//Setup ARP information
	pNewArpEntry->routingIdx=routingIdx;
	rg_db.arp[pNewArpEntry->idx].rtk_arp.nhIdx=l2Idx;
	rg_db.arp[pNewArpEntry->idx].rtk_arp.valid=1;
	rg_db.arp[pNewArpEntry->idx].ipv4Addr=ipv4Addr;
	rg_db.arp[pNewArpEntry->idx].staticEntry=staticEntry;
	rg_db.arp[pNewArpEntry->idx].idleSecs=0;

	//DEBUG("the arp[%d] has ip=%x, static=%d, nhIdx=%d",pNewArpEntry->idx,rg_db.arp[pNewArpEntry->idx].ipv4Addr,rg_db.arp[pNewArpEntry->idx].staticEntry,rg_db.arp[pNewArpEntry->idx].rtk_arp.nhIdx);

	//Add to hash head list
	list_add(&pNewArpEntry->arp_list,&rg_db.softwareArpTableHead[ipv4Addr&0xff]);

	return RT_ERR_RG_OK;
}

int32 _rtk_rg_arpRearrange(rtk_rg_routing_arpInfo_t *newAddingEntry, ipaddr_t newIpAddr, int routingARPNum)
{
	unsigned int i,j,k,arpNum=0,lanArpTotal=0,wanArpTotal=0,arpStart=0,inRangeIdx=0,routingIdx=0,prevRoutingIdx=0;	
	ipaddr_t comparingArpIP=0;

	//init
	bzero(rg_db.systemGlobal.tempArpTable,sizeof(rtk_rg_table_arp_t)*MAX_ARP_HW_TABLE_SIZE);
	bzero(rg_db.systemGlobal.tempL3Table,sizeof(rtk_l34_routing_entry_t)*MAX_L3_SW_TABLE_SIZE);
	bzero(rg_db.systemGlobal.arpTableCopied,sizeof(unsigned char)*MAX_ARP_HW_TABLE_SIZE);

	// TODO:We need to stop the packets flow from here!!
	
	//Count the ARP number all routing entries needed, and caculate each entry's ARP Start and End index
	for(i=0;i<routingARPNum;i++)
	{
		//newAdding one has not add to routing table yet
		arpNum=0x1<<rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].bitNum;
		routingIdx=rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].routingIdx;
		//DEBUG("i=%d, arpNum is %d, routingIdx is %d",i,arpNum,routingIdx);
		
		if(rg_db.l3[routingIdx].rtk_l3.valid==0)
			comparingArpIP=newIpAddr;
		else
			comparingArpIP=rg_db.l3[routingIdx].rtk_l3.ipAddr;

		for(j=0;j<i;j++)
		{
			//DEBUG("j is %d",j);
			prevRoutingIdx=rg_db.systemGlobal.p_tempRoutingArpInfoArray[j].routingIdx;
			if(rg_db.l3[prevRoutingIdx].rtk_l3.valid)
			{
				if((comparingArpIP&rg_db.l3[prevRoutingIdx].netmask) == rg_db.l3[prevRoutingIdx].rtk_l3.ipAddr)
				{
					//DEBUG("HIT!!comparingArpIP(%x)&rg_db.l3[%d].netmask=%x, rg_db.l3[%d].rtk_l3.ipAddr=%x",comparingArpIP,prevRoutingIdx,comparingArpIP&rg_db.l3[prevRoutingIdx].netmask,prevRoutingIdx,rg_db.l3[prevRoutingIdx].rtk_l3.ipAddr);
					break;
				}
			}
			else	//the routing entry has not added to table yet, therefore it must be the newAddingEntry
			{
				if((comparingArpIP&(~newAddingEntry->notMask)) == (newIpAddr&(~newAddingEntry->notMask)))
				{
					//DEBUG("HIT!!comparingArpIP(%x)&(~newAddingEntry.notMask)=%x, newIpAddr(%x)&(~newAddingEntry.notMask))=%x",comparingArpIP,comparingArpIP&(~newAddingEntry->notMask),newIpAddr,newIpAddr&(~newAddingEntry->notMask));
					break;
				}			
			}
		}
		
		if(j==i)//unmatch any IP-range before, create new IP-range
		{
//DEBUG("add new IP-range");
			if(rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].isLan)
				lanArpTotal+=arpNum;
			else
				wanArpTotal+=arpNum;
			rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpStart=arpStart;
			arpStart+=(arpNum>>0x2);
			rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpEnd=arpStart-1;	//each ARP index has four entries
		}
		else if(rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].bitNum==rg_db.systemGlobal.p_tempRoutingArpInfoArray[j].bitNum)
		{
//DEBUG("in the same IP-range");		
			if(rg_db.vlan[rg_db.systemGlobal.p_tempRoutingVlanInfoArray[i]].fidMode==VLAN_FID_IVL || 
				rg_db.vlan[rg_db.systemGlobal.p_tempRoutingVlanInfoArray[j]].fidMode==VLAN_FID_IVL)		//IVL interfaces can not overlap their ip subnet, otherwise routing table will always hit first add one
				return RT_ERR_RG_VLAN_BASED_OVERLAP_SUBNET;
			//the Ith entry is in the Jth entry's IP-range and Ith entry is as big as Jth entry
			rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpStart=rg_db.systemGlobal.p_tempRoutingArpInfoArray[j].arpStart;
			rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpEnd=rg_db.systemGlobal.p_tempRoutingArpInfoArray[j].arpEnd;
		}
		else
		{
//DEBUG("the %d(%d) is inside %d(%d) IP-range",i,rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].bitNum,j,rg_db.systemGlobal.p_tempRoutingArpInfoArray[j].bitNum);
			if(rg_db.vlan[rg_db.systemGlobal.p_tempRoutingVlanInfoArray[i]].fidMode==VLAN_FID_IVL || 
				rg_db.vlan[rg_db.systemGlobal.p_tempRoutingVlanInfoArray[j]].fidMode==VLAN_FID_IVL)		//IVL interfaces can not overlap their ip subnet, otherwise routing table will always hit first add one
				return RT_ERR_RG_VLAN_BASED_OVERLAP_SUBNET;

			//the Ith entry is inside the Jth entry's IP-range, so the Jth ARP start is referenced
			inRangeIdx=(comparingArpIP&rg_db.systemGlobal.p_tempRoutingArpInfoArray[j].notMask)>>rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].bitNum;
			rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpStart=rg_db.systemGlobal.p_tempRoutingArpInfoArray[j].arpStart+(inRangeIdx<<(rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].bitNum-2)); //each ARP index has four entries
			rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpEnd=rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpStart+(arpNum>>0x2)-1;	
		}

		//Check ARP number is over hardware limitation or not
		//DEBUG("Lan ARP total is %d, Wan ARP total is %d",lanArpTotal,wanArpTotal);
		if(newIpAddr!=0)	//in deleting, these check is not necessary
		{
			if((newAddingEntry->isLan && lanArpTotal>MAX_ARP_FOR_LAN_INTF) ||
				(newAddingEntry->isLan==0 && wanArpTotal>MAX_ARP_FOR_WAN_INTF))
			{
				//DEBUG("The hardware ARP table is not enough for the new routing entry...add to sw table!");
				return RT_ERR_RG_ADD_ARP_TO_SW_TABLE;	//add to sw table when needed
				//return RT_ERR_RG_ARP_FULL;
			}
		}
		//DEBUG("add to hardware ARP table for the new routing entry!");

		//Return the newArpRouting's ARP Start and End
		if(newIpAddr!=0 && rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].intfIdx==newAddingEntry->intfIdx)
		{
			newAddingEntry->arpStart=rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpStart;
			newAddingEntry->arpEnd=rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpEnd;
		}
		
		//Check if ARP rearrangement is needed
		j=rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpStart<<0x2;
		if(rg_db.l3[routingIdx].rtk_l3.valid==1)
		{
			//Keep routing table entry in tempRouting table
			memcpy(&rg_db.systemGlobal.tempL3Table[routingIdx],&rg_db.l3[routingIdx].rtk_l3,sizeof(rtk_l34_routing_entry_t));
//DEBUG("j = %d, tmp start %d, routing start %d, arpNUm= %d",j,rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpStart,rg_db.l3[routingIdx].rtk_l3.arpStart,arpNum);
			if(rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpStart != rg_db.l3[routingIdx].rtk_l3.arpStart)
			{
//DEBUG("rearrange!!");
				//Rearrange old ARP records in tempArpTable
				for(k=rg_db.l3[routingIdx].rtk_l3.arpStart<<0x2;k<((rg_db.l3[routingIdx].rtk_l3.arpEnd+1)<<0x2);k++)
					memcpy(&rg_db.systemGlobal.tempArpTable[j++],&rg_db.arp[k],sizeof(rtk_rg_table_arp_t));

				//Modify routing table
				rg_db.systemGlobal.tempL3Table[routingIdx].arpStart=rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpStart;
				rg_db.systemGlobal.tempL3Table[routingIdx].arpEnd=rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpEnd;
			}
			else if(rg_db.systemGlobal.arpTableCopied[j] == 0)
			{
//DEBUG("no..i am not moving..");							
				memcpy(&rg_db.systemGlobal.tempArpTable[j],&rg_db.arp[j],sizeof(rtk_rg_table_arp_t)*arpNum);
			}

			//Painted the arpTableCopied for recognize the ARP entries copied or not
			memset(&rg_db.systemGlobal.arpTableCopied[rg_db.systemGlobal.p_tempRoutingArpInfoArray[i].arpStart<<0x2],1,sizeof(unsigned char)*arpNum);
		}
	}

	//Write the tempRouting table to hardware L3 table
	for(i=0;i<MAX_L3_SW_TABLE_SIZE;i++)
	{
		//Only the modified routing entry need to be overrided
		if(rg_db.systemGlobal.tempL3Table[i].valid==1)
			ASSERT_EQ(RTK_L34_ROUTINGTABLE_SET(i,&rg_db.systemGlobal.tempL3Table[i]),RT_ERR_OK);
	}
	//Write the tempArp table to hardware ARP table
	for(i=0;i<MAX_ARP_HW_TABLE_SIZE;i++)
	{
		/*DEBUG("i = %d, valid = %d, l3idx = %d, nhidx= %d",i,
			rg_db.systemGlobal.tempArpTable[i].rtk_arp.valid,
			rg_db.systemGlobal.tempArpTable[i].rtk_arp.index,
			rg_db.systemGlobal.tempArpTable[i].rtk_arp.nhIdx);*/
		ASSERT_EQ(RTK_L34_ARPTABLE_SET(i,&rg_db.systemGlobal.tempArpTable[i].rtk_arp),RT_ERR_OK);
	}
	// TODO:We need to restart the packets flow from here!!
//DEBUG("after arp rearrange!!");
	return RT_ERR_RG_OK;
}

int32 _rtk_rg_addArpRoutingArray(rtk_rg_routing_arpInfo_t *newAddingEntry, ipaddr_t newIpAddr, int intfVlanId)
{
	unsigned int ret,i=0;
	rtk_rg_routing_arpInfo_t *pTemporary;
	int *pVlanTemporary;

	//init
	bzero(rg_db.systemGlobal.p_tempRoutingArpInfoArray,sizeof(rtk_rg_routing_arpInfo_t)*MAX_L3_SW_TABLE_SIZE);
	bzero(rg_db.systemGlobal.p_tempRoutingVlanInfoArray,sizeof(int)*MAX_L3_SW_TABLE_SIZE);

	//Add New entry with other old ARP routing entries in routingArpInfoArray to tempRoutingArpInfoArray by the order of IP-range size.
	//The software ARP routing will NOT add here, the new entry will add first since we will decide it can be added in hw or not in _rtk_rg_arpRearrange.
	for(i=0;i<rg_db.systemGlobal.routingArpInfoNum;i++)
	{
		if(newAddingEntry->bitNum>rg_db.systemGlobal.p_routingArpInfoArray[i].bitNum)
		{
			memcpy(&rg_db.systemGlobal.p_tempRoutingArpInfoArray[i],newAddingEntry,sizeof(rtk_rg_routing_arpInfo_t));
			memcpy(&rg_db.systemGlobal.p_tempRoutingArpInfoArray[i+1],&rg_db.systemGlobal.p_routingArpInfoArray[i],sizeof(rtk_rg_routing_arpInfo_t)*(rg_db.systemGlobal.routingArpInfoNum-i));

			rg_db.systemGlobal.p_tempRoutingVlanInfoArray[i]=intfVlanId;
			memcpy(&rg_db.systemGlobal.p_tempRoutingVlanInfoArray[i+1],&rg_db.systemGlobal.p_routingVlanInfoArray[i],sizeof(int)*(rg_db.systemGlobal.routingArpInfoNum-i));
			break;
		}
		else
		{
			memcpy(&rg_db.systemGlobal.p_tempRoutingArpInfoArray[i],&rg_db.systemGlobal.p_routingArpInfoArray[i],sizeof(rtk_rg_routing_arpInfo_t));
			rg_db.systemGlobal.p_tempRoutingVlanInfoArray[i]=rg_db.systemGlobal.p_routingVlanInfoArray[i];
		}
	}
	//the newAdding is the smallest one, so add it at the end of array
	if(i==rg_db.systemGlobal.routingArpInfoNum)
	{
		memcpy(&rg_db.systemGlobal.p_tempRoutingArpInfoArray[i],newAddingEntry,sizeof(rtk_rg_routing_arpInfo_t));
		rg_db.systemGlobal.p_tempRoutingVlanInfoArray[i]=intfVlanId;
	}

	//Count and check ARP table distribution
	ret=_rtk_rg_arpRearrange(newAddingEntry,newIpAddr,rg_db.systemGlobal.routingArpInfoNum+1);
	if(ret!=RT_ERR_RG_OK)
		return ret;
	
	//Global variable modification
	rg_db.systemGlobal.routingArpInfoNum++;
	pTemporary=rg_db.systemGlobal.p_routingArpInfoArray;
	rg_db.systemGlobal.p_routingArpInfoArray=rg_db.systemGlobal.p_tempRoutingArpInfoArray;
	rg_db.systemGlobal.p_tempRoutingArpInfoArray=pTemporary;
	
	pVlanTemporary=rg_db.systemGlobal.p_routingVlanInfoArray;
	rg_db.systemGlobal.p_routingVlanInfoArray=rg_db.systemGlobal.p_tempRoutingVlanInfoArray;
	rg_db.systemGlobal.p_tempRoutingVlanInfoArray=pVlanTemporary;

	return RT_ERR_RG_OK;	
}

int32 _rtk_rg_convertSwArpToHwTable(rtk_rg_routing_arpInfo_t *deletingEntry)
{
	int ret,i=0,arpNum,bitNum,arpIdx,intfVlanId;
	unsigned char inserted=0;

	struct list_head swRoutingHead;
	rtk_rg_routing_linkList_t swRoutingList[MAX_L3_HW_TABLE_SIZE];
	rtk_rg_routing_linkList_t *pSwRoutingList;
	
	rtk_rg_arp_linkList_t *pSwArpInfo,*pNextSwArpInfo;
	rtk_rg_arpEntry_t arpEntry;
	rtk_rg_routing_arpInfo_t newAddingRoute;
	ipaddr_t newIpAddr;
	rtk_l34_routing_entry_t rtEntry;

	//init
	INIT_LIST_HEAD(&swRoutingHead);
	for(i=0;i<MAX_L3_HW_TABLE_SIZE;i++)
	{
		INIT_LIST_HEAD(&swRoutingList[i].route_list);
		swRoutingList[i].idx=i;
		swRoutingList[i].bitNum=0;
	}

	//Check if the empty ARP table can accommodate the same size software ARP table routing
	//If so, copy from software ARP link list to hardware ARP table
	for(i=0;i<MAX_L3_HW_TABLE_SIZE;i++)
	{
		if(rg_db.l3[i].rtk_l3.valid && rg_db.l3[i].rtk_l3.ipAddr>0 && rg_db.l3[i].rtk_l3.process==L34_PROCESS_CPU)
		{
			bitNum=31-rg_db.l3[i].rtk_l3.ipMask;
			if(rg_db.l3[i].rtk_l3.rt2waninf!=deletingEntry->isLan && bitNum<=deletingEntry->bitNum)	//convert sw bitNum bigger than deleting hw entry is impossible
			{
				DEBUG("add to candidate list!!i is %d",i);
				swRoutingList[i].bitNum=bitNum;
				
				if(list_empty(&swRoutingHead))
				{
					DEBUG("add to list head!");
					list_add(&swRoutingList[i].route_list,&swRoutingHead);
				}
				else
				{
					//insert the new candidate entry by its bitNum, bigger is close to head, smaller is far from it.
					list_for_each_entry(pSwRoutingList,&swRoutingHead,route_list)
					{
						if(bitNum>pSwRoutingList->bitNum)
						{
							inserted=1;
							break;
						}
					}

					if(inserted)
					{
						DEBUG("insert new routing list(%d) before pSwRoutingList(%d)",bitNum,pSwRoutingList->bitNum);
						list_add_tail(&swRoutingList[i].route_list,&pSwRoutingList->route_list);
					}
					else
					{
						DEBUG("append new routing list(%d) before the Head",bitNum);
						list_add_tail(&swRoutingList[i].route_list,&swRoutingHead);
					}
				}
			}
		}
	}

	arpNum=0x1<<deletingEntry->bitNum;
	
	if(!list_empty(&swRoutingHead))
	{
		DEBUG("total can add %d arp entries",arpNum);
		list_for_each_entry(pSwRoutingList,&swRoutingHead,route_list)
		{
			//if the sw entry size is accommodate into the deleting range, 
			//transfer sw ARP to hw ARP, add to tempRoutingArray and rearrange ARP table
			//count arp range
			newAddingRoute.arpStart=deletingEntry->arpStart;
			arpNum-=0x1<<pSwRoutingList->bitNum;
			DEBUG("there are %d last arp entries can be add..",arpNum);
			if(arpNum<0)
				break;
			
			if(pSwRoutingList->bitNum<=2)
				deletingEntry->arpStart+=1;
			else
				deletingEntry->arpStart+=(0x1<<(pSwRoutingList->bitNum-2));
			newAddingRoute.arpEnd=deletingEntry->arpStart-1;
			DEBUG("    new arpStart=%d, arpEnd=%d...deleing arpStart=%d, arpEnd=%d",newAddingRoute.arpStart,newAddingRoute.arpEnd,deletingEntry->arpStart,deletingEntry->arpEnd);

			newAddingRoute.routingIdx=pSwRoutingList->idx;
			newAddingRoute.intfIdx=rg_db.l3[pSwRoutingList->idx].rtk_l3.netifIdx;
			newAddingRoute.bitNum=pSwRoutingList->bitNum;
			
			//Rearrange ARP table
			newIpAddr=rg_db.l3[pSwRoutingList->idx].rtk_l3.ipAddr;
			if(rg_db.l3[pSwRoutingList->idx].rtk_l3.rt2waninf)
			{
				newAddingRoute.isLan=0;
				newAddingRoute.notMask=~(rg_db.systemGlobal.interfaceInfo[newAddingRoute.intfIdx].p_wanStaticInfo->ip_network_mask);
				intfVlanId=rg_db.systemGlobal.interfaceInfo[newAddingRoute.intfIdx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;
			}
			else
			{
				newAddingRoute.isLan=1;
				newAddingRoute.notMask=~(rg_db.systemGlobal.interfaceInfo[newAddingRoute.intfIdx].p_lanIntfConf->ip_network_mask);
				intfVlanId=rg_db.systemGlobal.interfaceInfo[newAddingRoute.intfIdx].p_lanIntfConf->intf_vlan_id;
			}
			ret=_rtk_rg_addArpRoutingArray(&newAddingRoute,newIpAddr,intfVlanId);
			if(ret==RT_ERR_RG_OK)
			{
				//change routing table's process from CPU to ARP
				memcpy(&rtEntry,&rg_db.l3[newAddingRoute.routingIdx].rtk_l3,sizeof(rtk_l34_routing_entry_t));
				rtEntry.process = L34_PROCESS_ARP;
				rtEntry.arpStart = newAddingRoute.arpStart;
				rtEntry.arpEnd = newAddingRoute.arpEnd;
				DEBUG("    Convert sw ARP to hw!! newArpStart is %d, newArpEnd is %d",rtEntry.arpStart,rtEntry.arpEnd);
				ret = RTK_L34_ROUTINGTABLE_SET(newAddingRoute.routingIdx, &rtEntry);
				if(ret!=RT_ERR_OK)
					break;

				//Convert software ARP to hw ARP table
				for(i=0;i<MAX_ARP_SW_TABLE_HEAD;i++)
				{
					list_for_each_entry_safe(pSwArpInfo,pNextSwArpInfo,&rg_db.softwareArpTableHead[i],arp_list)
					{
						if(pSwArpInfo->routingIdx==newAddingRoute.routingIdx)	//Hit,transfer to hw arp
						{
							arpIdx=(newAddingRoute.arpStart<<2);
							arpIdx+=rg_db.arp[pSwArpInfo->idx].ipv4Addr&newAddingRoute.notMask;
							arpEntry.ipv4Addr=rg_db.arp[pSwArpInfo->idx].ipv4Addr;
							arpEntry.macEntryIdx=rg_db.arp[pSwArpInfo->idx].rtk_arp.nhIdx;
							arpEntry.staticEntry=rg_db.arp[pSwArpInfo->idx].staticEntry;		//keep static character
							DEBUG("    convert sw ARP[%d]->l2:%d from sw to hw[%d]!!",pSwArpInfo->idx,arpEntry.macEntryIdx,arpIdx);
							assert_ok(rtk_rg_arpEntry_add(&arpEntry,&arpIdx));		

							//Free software ARP list
							DEBUG("    free sw ARP[%d] in array[%d]!",pSwArpInfo->idx,i);
							_rtk_rg_softwareArpTableDel(pSwArpInfo);
						}	
					}
					/*pSwArpInfo=rg_db.pSoftwareArpTableHead[i];
					while(pSwArpInfo!=NULL)
					{
						pNextSwArpInfo=pSwArpInfo->pNext;
						if(pSwArpInfo->routingIdx==pSwRoutingHead->swRoutingEntry.routingIdx)		//Hit,transfer to hw arp
						{
							arpIdx=(pSwRoutingHead->swRoutingEntry.arpStart<<2);
							arpIdx+=pSwArpInfo->ipv4Addr&pSwRoutingHead->swRoutingEntry.notMask;
							arpEntry.ipv4Addr=pSwArpInfo->ipv4Addr;
							arpEntry.macEntryIdx=pSwArpInfo->nhIdx;
							arpEntry.staticEntry=pSwArpInfo->staticEntry;		//keep static character
							DEBUG("add ARP from sw to hw[%d]!!",arpIdx);
							assert_ok(rtk_rg_arpEntry_add(&arpEntry,&arpIdx));		

							//Free software ARP list
							DEBUG("free sw ARP Info in array[%d]!",i);
							_rtk_rg_softwareArpTableDel(pSwArpInfo);
						}
						pSwArpInfo=pNextSwArpInfo;
					}*/
				}
			}
			else
				break;	//failed to convert, just return
		}
		/*while(pSwRoutingHead!=NULL)
		{
			//if the sw entry size is accommodate into the deleting range, 
			//transfer sw ARP to hw ARP, add to tempRoutingArray and rearrange ARP table
			//count arp range
			pSwRoutingHead->swRoutingEntry.arpStart=deletingEntry->arpStart;
			arpNum-=0x1<<pSwRoutingHead->swRoutingEntry.bitNum;
			DEBUG("there are %d last arp entries can be add..",arpNum);
			if(arpNum<0)
				break;
			
			if(pSwRoutingHead->swRoutingEntry.bitNum<=2)
				deletingEntry->arpStart+=1;
			else
				deletingEntry->arpStart+=(0x1<<(pSwRoutingHead->swRoutingEntry.bitNum-2));
			pSwRoutingHead->swRoutingEntry.arpEnd=deletingEntry->arpStart;
			DEBUG("new arpStart=%d, arpEnd=%d...deleing arpStart=%d, arpEnd=%d",pSwRoutingHead->swRoutingEntry.arpStart,pSwRoutingHead->swRoutingEntry.arpEnd,
				deletingEntry->arpStart,deletingEntry->arpEnd);

			//Rearrange ARP table
			newIpAddr=rg_db.l3[pSwRoutingHead->swRoutingEntry.routingIdx].rtk_l3.ipAddr;
			if(pSwRoutingHead->swRoutingEntry.isLan)
				intfVlanId=rg_db.systemGlobal.interfaceInfo[pSwRoutingHead->swRoutingEntry.intfIdx].p_lanIntfConf->intf_vlan_id;
			else
				intfVlanId=rg_db.systemGlobal.interfaceInfo[pSwRoutingHead->swRoutingEntry.intfIdx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;
			ret=_rtk_rg_addArpRoutingArray(&pSwRoutingHead->swRoutingEntry,newIpAddr,intfVlanId);
			if(ret==RT_ERR_RG_OK)
			{
				//change routing table's process from CPU to ARP
				memcpy(&rtEntry,&rg_db.l3[pSwRoutingHead->swRoutingEntry.routingIdx].rtk_l3,sizeof(rtk_l34_routing_entry_t));
				rtEntry.process = L34_PROCESS_ARP;
				rtEntry.arpStart = pSwRoutingHead->swRoutingEntry.arpStart;
				rtEntry.arpEnd = pSwRoutingHead->swRoutingEntry.arpEnd;
				DEBUG("Convert sw ARP to hw!! newArpStart is %d, newArpEnd is %d",rtEntry.arpStart,rtEntry.arpEnd);
				ret = RTK_L34_ROUTINGTABLE_SET(pSwRoutingHead->swRoutingEntry.routingIdx, &rtEntry);
				if(ret!=RT_ERR_OK)
					break;

				for(i=0;i<MAX_ARP_SW_TABLE_HEAD;i++)
				{
					pSwArpInfo=rg_db.pSoftwareArpTableHead[i];
					while(pSwArpInfo!=NULL)
					{
						pNextSwArpInfo=pSwArpInfo->pNext;
						if(pSwArpInfo->routingIdx==pSwRoutingHead->swRoutingEntry.routingIdx)		//Hit,transfer to hw arp
						{
							arpIdx=(pSwRoutingHead->swRoutingEntry.arpStart<<2);
							arpIdx+=pSwArpInfo->ipv4Addr&pSwRoutingHead->swRoutingEntry.notMask;
							arpEntry.ipv4Addr=pSwArpInfo->ipv4Addr;
							arpEntry.macEntryIdx=pSwArpInfo->nhIdx;
							arpEntry.staticEntry=pSwArpInfo->staticEntry;		//keep static character
							DEBUG("add ARP from sw to hw[%d]!!",arpIdx);
							assert_ok(rtk_rg_arpEntry_add(&arpEntry,&arpIdx));		

							//Free software ARP list
							DEBUG("free sw ARP Info in array[%d]!",i);
							_rtk_rg_softwareArpTableDel(pSwArpInfo);
						}
						pSwArpInfo=pNextSwArpInfo;
					}
				}
			}
			else
				break;	//failed to convert, just return

			pSwRoutingHead=pSwRoutingHead->pNext;
		}*/
	}
	
	return RT_ERR_RG_OK;
}

int32 _rtk_rg_delArpRoutingArray(rtk_rg_routing_arpInfo_t *deletingEntry)
{
	unsigned int ret,i=0;
	rtk_rg_routing_arpInfo_t *pTemporary;
	int *pVlanTemporary;
	

	//init
	bzero(rg_db.systemGlobal.p_tempRoutingArpInfoArray,sizeof(rtk_rg_routing_arpInfo_t)*MAX_L3_SW_TABLE_SIZE);
	bzero(rg_db.systemGlobal.p_tempRoutingVlanInfoArray,sizeof(int)*MAX_L3_SW_TABLE_SIZE);

	//if the entry is deleting, we do not add it in tmpArray
	for(i=0;i<rg_db.systemGlobal.routingArpInfoNum;i++)
	{
		if(deletingEntry->routingIdx==rg_db.systemGlobal.p_routingArpInfoArray[i].routingIdx)
		{
			memcpy(&rg_db.systemGlobal.p_tempRoutingArpInfoArray[i],&rg_db.systemGlobal.p_routingArpInfoArray[i+1],sizeof(rtk_rg_routing_arpInfo_t)*(rg_db.systemGlobal.routingArpInfoNum-i-1));
			memcpy(&rg_db.systemGlobal.p_tempRoutingVlanInfoArray[i],&rg_db.systemGlobal.p_routingVlanInfoArray[i+1],sizeof(int)*(rg_db.systemGlobal.routingArpInfoNum-i-1));
			break;
		}
		else
		{
			memcpy(&rg_db.systemGlobal.p_tempRoutingArpInfoArray[i],&rg_db.systemGlobal.p_routingArpInfoArray[i],sizeof(rtk_rg_routing_arpInfo_t));
			rg_db.systemGlobal.p_tempRoutingVlanInfoArray[i]=rg_db.systemGlobal.p_routingVlanInfoArray[i];
		}
	}

	//Count and check ARP table distribution
	ret=_rtk_rg_arpRearrange(NULL,0,rg_db.systemGlobal.routingArpInfoNum-1);
	if(ret!=RT_ERR_RG_OK)
		return ret;
	
	//Global variable modification
	rg_db.systemGlobal.routingArpInfoNum--;
	pTemporary=rg_db.systemGlobal.p_routingArpInfoArray;
	rg_db.systemGlobal.p_routingArpInfoArray=rg_db.systemGlobal.p_tempRoutingArpInfoArray;
	rg_db.systemGlobal.p_tempRoutingArpInfoArray=pTemporary;

	
	pVlanTemporary=rg_db.systemGlobal.p_routingVlanInfoArray;
	rg_db.systemGlobal.p_routingVlanInfoArray=rg_db.systemGlobal.p_tempRoutingVlanInfoArray;
	rg_db.systemGlobal.p_tempRoutingVlanInfoArray=pVlanTemporary;

	//Convert sw ARP list to hw ARP table if any
	_rtk_rg_convertSwArpToHwTable(deletingEntry);
	
	return RT_ERR_RG_OK;	
}

void _rtk_rg_refreshPPPoEPassThroughLanOrWanPortMask(void)
{
	if((rg_db.algFunctionMask&RTK_RG_ALG_PPPOE_PASSTHROUGH_BIT) > 0)
	{
		//Turn off ACL first and re-enable Pass through to refresh the LAN or WAN port mask
		rg_db.algFunctionMask&=(~RTK_RG_ALG_PPPOE_PASSTHROUGH_BIT);
		rtk_rg_algApps_set(rg_db.algFunctionMask);
		rg_db.algFunctionMask&=RTK_RG_ALG_PPPOE_PASSTHROUGH_BIT;
		rtk_rg_algApps_set(rg_db.algFunctionMask);
	}
}

int32 _rtk_rg_createGatewayMacEntry(uint8 *gatewayMac, int vlanID, uint32 untagSet)
{
	rtk_rg_macEntry_t macEntry;
	int ret,l2Idx,search_index,count=0;
	
	memset(&macEntry,0,sizeof(rtk_rg_macEntry_t));

	macEntry.vlan_id=vlanID;
	macEntry.fid=rg_db.vlan[macEntry.vlan_id].fid;
	//DEBUG("the internalVlanID is %d, fid is %d",macEntry.vlan_id,macEntry.fid);

	if(rg_db.vlan[macEntry.vlan_id].fidMode==VLAN_FID_IVL)
	{
		macEntry.isIVL=1;
		l2Idx=_rtk_rg_hash_mac_vid_efid(gatewayMac,macEntry.vlan_id,0);		//FIXME:EFID is 0 now
	}
	else
	{
ADD_SVL_LUT:
		count=0;
		macEntry.isIVL=0;
		if((untagSet&(0x1<<RTK_RG_MAC_PORT_CPU))>0)		//cpu is in untag set
			macEntry.vlan_id=0;		//untag for DMAC2CVID
		l2Idx=_rtk_rg_hash_mac_fid_efid(gatewayMac,macEntry.fid,0);			//FIXME:EFID is 0 now
	}
	
	l2Idx<<=2;
	do
	{
		search_index = l2Idx+count;
		//DEBUG("search_idx is %d\n",search_index);
		if(rg_db.lut[search_index].valid==0)
			break;	//empty, just add
						
		if(rg_db.lut[search_index].rtk_lut.entryType==RTK_LUT_L2UC &&
			(memcmp(&rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,gatewayMac,ETHER_ADDR_LEN)==0))
		{
			if(((macEntry.isIVL==1) && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.vid==macEntry.vlan_id) ||
			((macEntry.isIVL==0) && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.fid==macEntry.fid))
			{
				//DEBUG("match!!");
				/*
				//FIXME: here reserved for WiFi interface may also need to handle port-moving in the future.
				if(rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port==RTK_RG_MAC_PORT_CPU)
				{

				}
				else */if(rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port!=RTK_RG_MAC_PORT_CPU)
				{
					//Mac port-moving, update LUT table without change ARP_USED flag
					//------------------ Critical Section start -----------------------//
					//rg_lock(&rg_kernel.saLearningLimitLock);
					if(rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port>=RTK_RG_PORT_CPU)
						atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.ext_port+RTK_RG_PORT_CPU]);
					else
						atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port]);
					//------------------ Critical Section End -----------------------//
					//rg_unlock(&rg_kernel.saLearningLimitLock);
	
					//macEntry.arp_used=((rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_ARP_USED)>0)?1:0;
					
					//if there is arp_used LUT equal to the gateway LUT entry, we just replace it by gateway's entry, therefore arp_used will always set to zero here.
					//macEntry.arp_used=1;
					//DEBUG("the port is moving..arp used is %d\n",macEntry.arp_used);
					DEBUG("froced replace LUT entry[%d] for gateway entry!!",search_index);
					break;
				}
				else if(rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.ext_port!=0)		//WLAN's entry
				{
					if(rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port>=RTK_RG_PORT_CPU)
						atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.ext_port+RTK_RG_PORT_CPU]);
					else
						atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.port]);
					//macEntry.arp_used=1;
					DEBUG("froced replace WLAN's LUT entry[%d] for gateway entry!!",search_index);
					break;
				}

				if(macEntry.isIVL)goto ADD_SVL_LUT;		//check SVL,too
				return SUCCESS;		//exist, do nothing
			}
		}

		count++; //search from next entry
	}while(count < 4);

	if(count==4)
		count=_rtk_rg_layer2GarbageCollection(l2Idx);		//check if there is asynchronus between software and hardware table

	//Check per port SA learning limit
	//------------------ Critical Section start -----------------------//
	//rg_lock(&rg_kernel.saLearningLimitLock);
	if(rg_db.systemGlobal.sourceAddrLearningLimitNumber[RTK_RG_MAC_PORT_CPU]==atomic_read(&rg_db.systemGlobal.sourceAddrLearningCount[RTK_RG_MAC_PORT_CPU]))		//no way to learn
	{
		//------------------ Critical Section End -----------------------//
		//rg_unlock(&rg_kernel.saLearningLimitLock);
		DEBUG("Port CPU SA learning limit is reached(%d)...action is %s!!",rg_db.systemGlobal.sourceAddrLearningLimitNumber[RTK_RG_MAC_PORT_CPU],
			rg_db.systemGlobal.sourceAddrLearningAction[RTK_RG_MAC_PORT_CPU]==SA_LEARN_EXCEED_ACTION_PERMIT?"Permit and Forward":"Drop");
		if(rg_db.systemGlobal.sourceAddrLearningAction[RTK_RG_MAC_PORT_CPU]==SA_LEARN_EXCEED_ACTION_PERMIT)
			return SUCCESS;
		else
			return FAIL;
	}
	//------------------ Critical Section End -----------------------//
	//rg_unlock(&rg_kernel.saLearningLimitLock);

	//Replace the least recently used entry for new entry
	if(count==4)
	{
		search_index=_rtk_rg_layer2LeastRecentlyUsedReplace(l2Idx);
		if(search_index==-1)
		{
			FIXME("must add software LUT entry for LUT entry full.");
			return FAIL;
		}
	}
	else
		search_index=l2Idx+count;

	memcpy(macEntry.mac.octet,gatewayMac,ETHER_ADDR_LEN);
	macEntry.port_idx=RTK_RG_MAC_PORT_CPU;
	macEntry.static_entry=1;	//since this lut entry should exist till interface dead, we have to create it statically
	ret=rtk_rg_macEntry_add(&macEntry,&search_index);
	DEBUG("### add l2[%d]=%02x:%02x:%02x:%02x:%02x:%02x ###\n",search_index,gatewayMac[0],gatewayMac[1],gatewayMac[2],gatewayMac[3],gatewayMac[4],gatewayMac[5]);
	assert_ok(ret);

	//add to SA learning count
	//------------------ Critical Section start -----------------------//
	//rg_lock(&rg_kernel.saLearningLimitLock);
	atomic_inc(&rg_db.systemGlobal.sourceAddrLearningCount[RTK_RG_MAC_PORT_CPU]);
	//------------------ Critical Section End -----------------------//
	//rg_unlock(&rg_kernel.saLearningLimitLock);

	if(macEntry.isIVL)goto ADD_SVL_LUT;		//add SVL,too
	return SUCCESS;
}

int32 _rtk_rg_deleteGatewayMacEntry(uint8 *gatewayMac, int vlanID, uint32 untagSet)
{
	rtk_rg_macEntry_t macEntry;
	int ret,l2Idx,search_index,count=0,i;
	
	memset(&macEntry,0,sizeof(rtk_rg_macEntry_t));

	macEntry.vlan_id=vlanID;
	macEntry.fid=rg_db.vlan[macEntry.vlan_id].fid;
	//DEBUG("the internalVlanID is %d, fid is %d",macEntry.vlan_id,macEntry.fid);

	if(rg_db.vlan[macEntry.vlan_id].fidMode==VLAN_FID_IVL)
	{
		macEntry.isIVL=1;
		l2Idx=_rtk_rg_hash_mac_vid_efid(gatewayMac,macEntry.vlan_id,0);		//FIXME:EFID is 0 now
	}
	else
	{
DEL_SVL_LUT:
		count=0;
		macEntry.isIVL=0;
		if((untagSet&(0x1<<RTK_RG_MAC_PORT_CPU))>0)		//cpu is in untag set
			macEntry.vlan_id=0;		//untag for DMAC2CVID
		l2Idx=_rtk_rg_hash_mac_fid_efid(gatewayMac,macEntry.fid,0);			//FIXME:EFID is 0 now
	}
	
	l2Idx<<=2;
	do
	{
		search_index = l2Idx+count;
		//DEBUG("search_idx is %d\n",search_index);						
		if(rg_db.lut[search_index].rtk_lut.entryType==RTK_LUT_L2UC &&
			(memcmp(&rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,gatewayMac,ETHER_ADDR_LEN)==0))
		{
			if(((macEntry.isIVL==1) && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.vid==macEntry.vlan_id) ||
			((macEntry.isIVL==0) && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.fid==macEntry.fid))
			{
				//DEBUG("match!!");
				break;
			}
		}

		count++; //search from next entry
	}while(count < 4);

	if(count==4)
	{
		DEBUG("gateway %s lut is not exist anymore...",macEntry.isIVL?"VLAN-based":"MAC-based");
		if(macEntry.isIVL)goto DEL_SVL_LUT;		//del SVL,too
		return SUCCESS;
	}

	//Check if there are LAN or WAN interfaces use the same MAC and VLANID
	for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
	{
		//if the interface are IVL, VLAN and MAC are all the same, we just keep the IVL one, but SVL should check again
		if(macEntry.isIVL==1)
		{
			if(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->intf_vlan_id==macEntry.vlan_id && rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->isIVL && memcmp(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->gmac.octet,rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN)==0)
				goto DEL_SVL_LUT;
		}
		else		//check the VLANID to insure that tag/untag will be same
		{
			if(macEntry.vlan_id==0)	//untag
			{
				if((rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->untag_mask.portmask&(0x1<<RTK_RG_MAC_PORT_CPU))>0 && macEntry.vlan_id==0 &&	//cpu is in untag set
					memcmp(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->gmac.octet,rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN)==0)
					goto KEEP_MAC;
			}
			else	//tag: compare VID
			{
				if(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->intf_vlan_id==macEntry.vlan_id && 
					memcmp(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->gmac.octet,rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN)==0)
					goto KEEP_MAC;
			}
		}
	}
	for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
	{
		//if the interface are IVL, VLAN and MAC are all the same, we just keep the IVL one, but SVL should check again
		if(macEntry.isIVL==1)
		{
			if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_id==macEntry.vlan_id && rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->isIVL && memcmp(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->gmac.octet,rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN)==0)
				goto DEL_SVL_LUT;
		}
		else
		{
			if(macEntry.vlan_id==0)	//untag
			{
				if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_tag_on==0 &&	//cpu is in untag set
					memcmp(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->gmac.octet,rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN)==0)
					goto KEEP_MAC;
			}
			else	//tag: compare VID
			{
				if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_id==macEntry.vlan_id &&
					memcmp(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->gmac.octet,rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN)==0)	
					goto KEEP_MAC;
			}
			
		}
	}
	
	ret=rtk_rg_macEntry_del(search_index);
	if(ret==RT_ERR_RG_ENTRY_NOT_EXIST)		//not in hardware anymore
	{
		//------------------ Critical Section start -----------------------//
		//rg_lock(&rg_kernel.saLearningLimitLock);
		atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[RTK_RG_MAC_PORT_CPU]);	
		//------------------ Critical Section End -----------------------//
		//rg_unlock(&rg_kernel.saLearningLimitLock);
		
		memset(&rg_db.lut[search_index],0,sizeof(rtk_rg_table_lut_t));
	}

	if(macEntry.isIVL)goto DEL_SVL_LUT;		//del SVL,too

KEEP_MAC:	
	return SUCCESS;
}

void _rtk_rg_cleanPortAndProtocolSettings(rtk_port_t port)
{
	int ret;
	rtk_vlan_protoVlanCfg_t protoVlanCfg;

	//Clean all Group of port-and-protocol based VID
	protoVlanCfg.valid=0;
	protoVlanCfg.vid=DEFAULT_CPU_VLAN;
	protoVlanCfg.pri=0;			//FIXME: should I change this?
	protoVlanCfg.dei=0;
	ret = RTK_VLAN_PORTPROTOVLAN_SET(port,RG_IPV4_GROUPID,&protoVlanCfg);
	assert_ok(ret);

	protoVlanCfg.valid=0;
	protoVlanCfg.vid=DEFAULT_CPU_VLAN;
	protoVlanCfg.pri=0;			//FIXME: should I change this?
	protoVlanCfg.dei=0;
	ret = RTK_VLAN_PORTPROTOVLAN_SET(port,RG_ARP_GROUPID,&protoVlanCfg);
	assert_ok(ret);

	protoVlanCfg.valid=0;
	protoVlanCfg.vid=DEFAULT_CPU_VLAN;
	protoVlanCfg.pri=0;			//FIXME: should I change this?
	protoVlanCfg.dei=0;
	ret = RTK_VLAN_PORTPROTOVLAN_SET(port,RG_IPV6_GROUPID,&protoVlanCfg);
	assert_ok(ret);
}

int _rtk_rg_updatePortBasedVIDByLanOrder(rtk_portmask_t mac_pmask, rtk_portmask_t etp_pmask)
{
	int i,j,setPVid,ret,IPVerAll,IPVerV4,IPVerV6;//firstIPVer;
	rtk_vlan_protoVlanCfg_t protoVlanCfg;
	//rtk_portmask_t mbpmsk,etpmsk,update_mbpmsk,update_etpmsk;

	//DEBUG("mac_pmsk is %x, ext_pmsk is %x",mac_pmask.bits[0],etp_pmask.bits[0]);

	//Union all LAN port into DEFAULT_LAN_VLAN
	/*bzero(&update_mbpmsk,sizeof(rtk_portmask_t));
	bzero(&update_etpmsk,sizeof(rtk_portmask_t));
	for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
	{	
		_rtk_rg_portmask_translator(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->port_mask,&mbpmsk,&etpmsk);
		update_mbpmsk.bits[0]|=mbpmsk.bits[0];
		update_etpmsk.bits[0]|=etpmsk.bits[0];
	}
	//filter WAN port
	update_mbpmsk.bits[0]&=0x4f;		
	
	//update DEFAULT_LAN_VLAN
	ret = RTK_VLAN_PORT_SET(DEFAULT_LAN_VLAN, &update_mbpmsk, &rg_db.vlan[DEFAULT_LAN_VLAN].UntagPortmask);
	assert_ok(ret);
	ret = RTK_VLAN_EXTPORT_SET(DEFAULT_LAN_VLAN, &update_etpmsk);
	assert_ok(ret);*/

	//Check member port of VLAN ID
	for(i=0;i<RTK_RG_MAC_PORT_MAX;i++)
	{
		_rtk_rg_cleanPortAndProtocolSettings(i);
			
		if((mac_pmask.bits[0]&(0x1<<i)) > 0 /*&& i != RTK_RG_MAC_PORT_CPU*/)
		{
			//init
			IPVerAll=0;
			IPVerV4=0;
			IPVerV6=0;
			setPVid=DEFAULT_CPU_VLAN;
			for(j=0;j<rg_db.systemGlobal.lanIntfTotalNum;j++)
			{
				if((rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->port_mask.portmask&(0x1<<i)) > 0)
				{
					if(rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->ip_version==IPVER_V4ONLY)
					{
						if(IPVerV4==0)		//first add V4 ony inteface, add to PPB setting
						{
							DEBUG("@@@ add IPv4 PPB as %d in port %d",rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->intf_vlan_id,i);
							IPVerV4=1;
							//add V4 PPB
							protoVlanCfg.valid=1;
							protoVlanCfg.vid=rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->intf_vlan_id;
							protoVlanCfg.pri=0;			//FIXME: should I change this?
							protoVlanCfg.dei=0;
							ret = RTK_VLAN_PORTPROTOVLAN_SET(i,RG_IPV4_GROUPID,&protoVlanCfg);
							assert_ok(ret);
							
							//add ARP PPB
							protoVlanCfg.valid=1;
							protoVlanCfg.vid=rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->intf_vlan_id;
							protoVlanCfg.pri=0;			//FIXME: should I change this?
							protoVlanCfg.dei=0;
							ret = RTK_VLAN_PORTPROTOVLAN_SET(i,RG_ARP_GROUPID,&protoVlanCfg);
							assert_ok(ret);

							if(IPVerAll==0)
							{
								//if no other interface set to "ALL", the PVID should set to DEFAULT_LAN_VLAN 
								//to block all other protocol packet from going to other port.
								setPVid=DEFAULT_LAN_VLAN;
							}	
						}
					}
					else if(rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->ip_version==IPVER_V6ONLY)
					{
						if(IPVerV6==0)
						{
							DEBUG("@@@ add IPv6 PPB as %d in port %d",rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->intf_vlan_id,i);
							IPVerV6=1;
							//add V6 PPB
							protoVlanCfg.valid=1;
							protoVlanCfg.vid=rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->intf_vlan_id;
							protoVlanCfg.pri=0;			//FIXME: should I change this?
							protoVlanCfg.dei=0;
							ret = RTK_VLAN_PORTPROTOVLAN_SET(i,RG_IPV6_GROUPID,&protoVlanCfg);
							assert_ok(ret);

							if(IPVerAll==0)
							{
								//if no other interface set to "ALL", the PVID should set to DEFAULT_LAN_VLAN 
								//to block all other protocol packet from going to other port.
								setPVid=DEFAULT_LAN_VLAN;
							}
						}
					}
					else	//all
					{
						if(IPVerAll==0)
						{
							DEBUG("@@@ set pvid of port %d as %d",i,rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->intf_vlan_id);
							setPVid=rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->intf_vlan_id;
							IPVerAll=1;
						}
					}
				}
			}
#if 0			
			firstIPVer=-1;
			setPVid=DEFAULT_CPU_VLAN;
			for(j=0;j<rg_db.systemGlobal.lanIntfTotalNum;j++)
			{	
				if((rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->port_mask.portmask&(0x1<<i)) > 0)
				{
					if(firstIPVer<0)		//first interface's IPVersion
					{
						setPVid=rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->intf_vlan_id;
						firstIPVer=rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->ip_version;
					}
					else		//the j is after first interface, so check if we need to add port and protocol based VLAN
					{
						if(firstIPVer==IPVER_V4V6 || rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->ip_version != firstIPVer)
						{
							if(firstIPVer==IPVER_V4ONLY)
							{
								//add V6 PPB
								protoVlanCfg.valid=1;
								protoVlanCfg.vid=rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->intf_vlan_id;
								protoVlanCfg.pri=0;			//FIXME: should I change this?
								protoVlanCfg.dei=0;
								ret = RTK_VLAN_PORTPROTOVLAN_SET(i,RG_IPV6_GROUPID,&protoVlanCfg);
								assert_ok(ret);
							}
							else if(firstIPVer==IPVER_V6ONLY)
							{
								//add V4 PPB
								protoVlanCfg.valid=1;
								protoVlanCfg.vid=rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->intf_vlan_id;
								protoVlanCfg.pri=0;			//FIXME: should I change this?
								protoVlanCfg.dei=0;
								ret = RTK_VLAN_PORTPROTOVLAN_SET(i,RG_IPV4_GROUPID,&protoVlanCfg);
								assert_ok(ret);
								
								//add ARP PPB
								protoVlanCfg.valid=1;
								protoVlanCfg.vid=rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->intf_vlan_id;
								protoVlanCfg.pri=0;			//FIXME: should I change this?
								protoVlanCfg.dei=0;
								ret = RTK_VLAN_PORTPROTOVLAN_SET(i,RG_ARP_GROUPID,&protoVlanCfg);
								assert_ok(ret);
							}
							break;
						}
					}
				}
			}
#endif
			//DEBUG("MAC port %d vlan is %d",i,setPVid);
			ret = RTK_VLAN_PORTPVID_SET(i, setPVid);
			assert_ok(ret);
		}
		if(i==0)continue;	//extension port 0 is CPU port, no port-based VID
		if((etp_pmask.bits[0]&(0x1<<i)) > 0 && i+RTK_RG_PORT_CPU < RTK_RG_PORT_MAX)
		{
			setPVid=DEFAULT_CPU_VLAN;
			for(j=0;j<rg_db.systemGlobal.lanIntfTotalNum;j++)
			{	
				if((rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->port_mask.portmask&(0x1<<(i+RTK_RG_PORT_CPU))) > 0)
				{
					if(rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->ip_version==IPVER_V4V6)
					{
						setPVid=rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->intf_vlan_id;
						break;
					}
					else
					{
						setPVid=DEFAULT_LAN_VLAN;		//V4 only or V6 only, the PPB is set on CPU port.
					}
				}
			}
			//DEBUG("EXT port %d vlan is %d",i,setPVid);
			ret = RTK_VLAN_EXTPORTPVID_SET(i, setPVid);		//ext port index is 0-4, means ext port 1-5
			assert_ok(ret);
			
			//1 FIXME: in 6266, port and protocol based VLAN do not have extension port settings!!
		}
	}

	return RT_ERR_RG_OK;
}

int32 rtk_rg_lanInterface_add(rtk_rg_lanIntfConf_t *lan_info,int *intf_idx)
{
	int ret,vlan_exist=0,errorno=RT_ERR_RG_OK,tmpVid,/*tmpPVid,*/ipv4Enable=0,ipv6Enable=0;
	int i,j,intfIdx,rtIdx=-1,rtv6Idx=-1,input_ipmsk,v6TrapIdx=-1;
	rtk_l34_netif_entry_t intfEntry;
	rtk_l34_routing_entry_t rtEntry;
	rtk_ipv6Routing_entry_t rtv6Entry;
	rtk_portmask_t ori_pmsk,ori_utmsk,ori_etpmsk;
	rtk_fidMode_t ori_mode=VLAN_FID_IVL;
	rtk_portmask_t out_mac_pmask,out_ext_pmask;
	rtk_portmask_t untag_mac_pmask;
	//rtk_portmask_t ori_CPU_member_mask,ori_CPU_untag_mask,ori_CPU_ext_mask;
	rtk_rg_ipv4RoutingEntry_t cb_routEt;
	rtk_rg_ipv6RoutingEntry_t cb_routv6Et;
	//rtk_vlan_protoVlanCfg_t protoVlanCfg;
	rtk_rg_routing_arpInfo_t newAddingEntry;
	rtk_fidMode_t fidMode;
	
	//Checking for input parameter
	//if(rg_db.systemGlobal.wanIntfTotalNum > 0)
			//return RT_ERR_RG_MODIFY_LAN_AT_WAN_EXIST;		//LAN intf can not add after WAN created
	if(lan_info == NULL || intf_idx == NULL)
		return RT_ERR_RG_NULL_POINTER;
	if(lan_info->gmac.octet[0] == 0 && lan_info->gmac.octet[1] == 0 && lan_info->gmac.octet[2] == 0 &&
		lan_info->gmac.octet[3] == 0 && lan_info->gmac.octet[4] == 0 && lan_info->gmac.octet[5] == 0)
		return RT_ERR_RG_INVALID_PARAM;
	if(rg_db.systemGlobal.initParam.macBasedTagDecision && lan_info->isIVL)		//IVL can not be set when DMAC2CVID is trun on
		return RT_ERR_RG_INVALID_PARAM;

	//Check VLAN init
	if(rg_db.systemGlobal.vlanInit==0)
		return RT_ERR_RG_NOT_INIT;
	
	//Check IP version
	if((lan_info->ip_version==IPVER_V4ONLY || lan_info->ip_version==IPVER_V4V6) && lan_info->ip_network_mask>0)
		ipv4Enable=1;
	if((lan_info->ip_version==IPVER_V6ONLY || lan_info->ip_version==IPVER_V4V6) && lan_info->ipv6_network_mask_length>0)
		ipv6Enable=1;
	
	/*if(ipv4Enable==1 && (lan_info->ip_addr == 0 || lan_info->ip_network_mask == 0))
		return RT_ERR_RG_INVALID_PARAM;
	if(ipv6Enable==1)
	{
		if(lan_info->ipv6_network_mask_length== 0)
			return RT_ERR_RG_INVALID_PARAM;
		//Check for valid IPv6 address
		if(*(unsigned int *)lan_info->ipv6_addr.ipv6_addr == 0 &&
			*(unsigned int *)(lan_info->ipv6_addr.ipv6_addr+4) == 0 &&
			*(unsigned int *)(lan_info->ipv6_addr.ipv6_addr+8) == 0 &&
			*(unsigned int *)(lan_info->ipv6_addr.ipv6_addr+12) == 0)
			return RT_ERR_RG_INVALID_PARAM;
	}*/
	
	if(lan_info->port_mask.portmask == 0 || 
		lan_info->mtu == 0 || 
		lan_info->intf_vlan_id == DEFAULT_CPU_VLAN || 
		/*lan_info->intf_vlan_id == DEFAULT_LAN_VLAN ||*/
		(lan_info->untag_mask.portmask>>RTK_RG_MAC_PORT_MAX)>0)		//untag set didn't contain extension port
		return RT_ERR_RG_INVALID_PARAM;
	if(lan_info->intf_vlan_id >= DEFAULT_PPB_VLAN_START && lan_info->intf_vlan_id < (DEFAULT_PPB_VLAN_START+MAX_NETIF_SW_TABLE_SIZE))
		return RT_ERR_RG_INVALID_PARAM;
	//different Lan interface can have same port
	/*for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
	{
		//Check if there is any LAN interface has the same port with the adding intf
		if((rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->port_mask.portmask&lan_info->port_mask.portmask) > 0)
			return RT_ERR_RG_PORT_USED;			
	}*/
#ifdef CONFIG_APOLLO_RLE0371
#else
	//Check VLAN-binding use this VLAN or not
	for(i=0;i<MAX_BIND_SW_TABLE_SIZE;i++)
		if(rg_db.bind[i].valid==1 && rg_db.bind[i].rtk_bind.vidLan==lan_info->intf_vlan_id)
			return RT_ERR_RG_VLAN_USED_BY_VLANBINDING;
	//Check Customer VLAN use this VLAN or not
	if(rg_db.vlan[lan_info->intf_vlan_id].valid && rg_db.vlan[lan_info->intf_vlan_id].addedAsCustomerVLAN)
		return RT_ERR_RG_VLAN_USED_BY_CVLAN;
#endif
/*#if defined(CONFIG_APOLLO_RLE0371) && !defined(CONFIG_APOLLO_TESTING)
	if(lan_info->pppoe_passThrough == 1)
		return RT_ERR_RG_CHIP_NOT_SUPPORT;
#endif*/

#if 0
	if(lan_info->dhcp_server_enable)
	{
		if(lan_info->dhcp_start_ip_addr == 0 || lan_info->dhcp_end_ip_addr == 0)
			return RT_ERR_RG_INVALID_PARAM;
	}
#endif

	//------------------ Critical Section start -----------------------//
	rg_lock(&rg_kernel.interfaceLock);

	//rtlglue_printf("RTK RG lan add.....%d\n",RG_GLB_VLAN_INIT);
	//Transfer RG portmask to RTK portmask
	_rtk_rg_portmask_translator(lan_info->port_mask,&out_mac_pmask,&out_ext_pmask);

	//Check interface table available or not
	for(i=0;i<MAX_NETIF_SW_TABLE_SIZE;i++)
	{
		//bzero(&intfEntry, sizeof(rtk_l34_netif_entry_t));
		//ret = rtk_l34_netifTable_get(i, &intfEntry);
		//if(ret!=RT_ERR_OK)return RT_ERR_RG_INTF_GET_FAIL;
		
		//if(intfEntry.valid == 0)
		if(rg_db.systemGlobal.interfaceInfo[i].valid == 0)
			break;
	}
	if(i==MAX_NETIF_SW_TABLE_SIZE)
		return RT_ERR_RG_ENTRY_FULL;

	intfIdx=i;		//keep

	//Check routing table available or not
	if(ipv4Enable==1)
	{		
		rtIdx=MAX_L3_SW_TABLE_SIZE;
		for(i=0;i<MAX_L3_SW_TABLE_SIZE - 1;i++)	//because idx 7 is reserved for default route
		{
			//if(rg_db.l3[i].rtk_l3.valid == 0 && rtIdx==MAX_L3_SW_TABLE_SIZE)
				//rtIdx=i;		//keep the first valid entry
			if(rg_db.l3[i].rtk_l3.valid == 0)
			{
				rtIdx=i;		//keep the first valid entry
				break;
			}

			//if(rg_db.l3[i].rtk_l3.process == L34_PROCESS_ARP && rg_db.l3[i].rtk_l3.arpEnd > last_arp)		//find the end ARP address
				//last_arp = rg_db.l3[i].rtk_l3.arpEnd;
		}
		if(rtIdx==MAX_L3_SW_TABLE_SIZE)
			return RT_ERR_RG_ENTRY_FULL;
	}
	if(ipv6Enable==1)
	{
		rtv6Idx=MAX_IPV6_ROUTING_SW_TABLE_SIZE;
		for(i=0;i<MAX_IPV6_ROUTING_SW_TABLE_SIZE - 1;i++)	//because idx 3 is reserved for default route
		{
			if(rg_db.v6route[i].rtk_v6route.valid == 0)
			{
				rtv6Idx=i;		//keep the first valid entry
				break;
			}
		}
		if(rtv6Idx==MAX_IPV6_ROUTING_SW_TABLE_SIZE)
			return RT_ERR_RG_ENTRY_FULL;

		v6TrapIdx=MAX_IPV6_ROUTING_SW_TABLE_SIZE;
		for(i=0;i<MAX_IPV6_ROUTING_SW_TABLE_SIZE - 1;i++)	//because idx 3 is reserved for default route
		{
			if(rg_db.v6route[i].rtk_v6route.valid==0 && i!=rtv6Idx)
			{
 				v6TrapIdx=i;		//keep the first valid entry not rtv6Idx
 				break;
			}
		}
		if(v6TrapIdx==MAX_IPV6_ROUTING_SW_TABLE_SIZE)
			return RT_ERR_RG_ENTRY_FULL;
	}

	//Set up Interface table
	intfEntry.valid=1;
	memcpy(intfEntry.gateway_mac.octet, lan_info->gmac.octet,ETHER_ADDR_LEN);
	intfEntry.mac_mask=0x7;	//no mask
	intfEntry.vlan_id=lan_info->intf_vlan_id;		//for LAN DMAC2CVID to replace this SVL CVID
	intfEntry.enable_rounting=1;
	intfEntry.mtu=lan_info->mtu;

#if 0
	//Set Default LAN VLAN
	//ret = rtk_vlan_port_get(DEFAULT_LAN_VLAN, &ori_CPU_member_mask, &ori_CPU_untag_mask);
	//if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_GET_FAIL;
	memcpy(&ori_CPU_member_mask, &rg_db.vlan[DEFAULT_LAN_VLAN].MemberPortmask,sizeof(rtk_portmask_t));
	memcpy(&ori_CPU_untag_mask, &rg_db.vlan[DEFAULT_LAN_VLAN].UntagPortmask,sizeof(rtk_portmask_t));
	//ret = rtk_vlan_extPort_get(DEFAULT_LAN_VLAN, &ori_CPU_ext_mask);
	//if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_GET_FAIL;
	memcpy(&ori_CPU_ext_mask, &rg_db.vlan[DEFAULT_LAN_VLAN].Ext_portmask,sizeof(rtk_portmask_t));
	
	errorno=RT_ERR_RG_VLAN_SET_FAIL;
	/*if(rg_db.systemGlobal.lanIntfTotalNum==0)		//This is first created LAN interface
	{
		//only change member port and extension port since other settings won't necessary	
		ori_pmsk.bits[0]|=out_mac_pmask.bits[0];
		ori_pmsk.bits[0]|=(0x1<<RTK_RG_MAC_PORT_CPU); //add CPU port to vlan

		ori_etpmsk.bits[0]|=out_ext_pmask.bits[0];
		ori_etpmsk.bits[0]|=0x1;		//add ext-CPU port to vlan
		
		ret = RTK_VLAN_PORT_SET(DEFAULT_LAN_VLAN, &ori_pmsk, &ori_CPU_untag_mask);	
		if(ret!=RT_ERR_OK)goto RET_DEF_VLAN_ERR;
		ret = RTK_VLAN_EXTPORT_SET(DEFAULT_LAN_VLAN, &ori_etpmsk);
		if(ret!=RT_ERR_OK)goto RET_DEF_VLAN_ERR;
	}
	else
	{*/
		//Add more member port and extension port since other settings won't necessary	
		ori_pmsk.bits[0]=ori_CPU_member_mask.bits[0];
		ori_pmsk.bits[0]|=out_mac_pmask.bits[0];

		//add bridge WAN port, if any
		for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
		{
			if(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE)
			{
				ori_pmsk.bits[0]|=(0x1<<rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.wan_port_idx);
			}
		}

		ori_etpmsk.bits[0]=ori_CPU_ext_mask.bits[0];
		ori_etpmsk.bits[0]|=out_ext_pmask.bits[0];
		
		ret = RTK_VLAN_PORT_SET(DEFAULT_LAN_VLAN, &ori_pmsk, &ori_CPU_untag_mask);	
		if(ret!=RT_ERR_OK)goto RET_DEF_VLAN_ERR;
		ret = RTK_VLAN_EXTPORT_SET(DEFAULT_LAN_VLAN, &ori_etpmsk);
		if(ret!=RT_ERR_OK)goto RET_DEF_VLAN_ERR;
	//}
#endif
	//Set VLAN
	ori_pmsk.bits[0]=out_mac_pmask.bits[0];		//initial port mask
	ori_etpmsk.bits[0]=out_ext_pmask.bits[0];	//initial ext-port mask
	ori_utmsk.bits[0]=lan_info->untag_mask.portmask;	//initial untag set mask

	//pppoe passthrough VLAN, noneed anymore
	/*if(lan_info->pppoe_passThrough == 1)
	{
		tmpVid=DEFAULT_PPB_VLAN_START+intfIdx;
		RTK_VLAN_CREATE(tmpVid);
		RTK_VLAN_FIDMODE_SET(tmpVid, VLAN_FID_IVL);
		RTK_VLAN_FID_SET(tmpVid, LAN_FID);
		RTK_VLAN_PORT_SET(tmpVid, &ori_pmsk, &ori_utmsk);	
		RTK_VLAN_EXTPORT_SET(tmpVid, &ori_etpmsk);
	}*/

	tmpVid=lan_info->intf_vlan_id;
	ret = RTK_VLAN_CREATE(tmpVid);
	if(ret==RT_ERR_VLAN_EXIST)
	{
		//errorno=RT_ERR_RG_VLAN_GET_FAIL;
		//ret = rtk_vlan_port_get(tmpVid, &ori_pmsk, &ori_utmsk);
		//if(ret!=RT_ERR_OK)goto RET_DEF_VLAN_ERR;
		memcpy(&ori_pmsk, &rg_db.vlan[tmpVid].MemberPortmask,sizeof(rtk_portmask_t));
		memcpy(&ori_utmsk, &rg_db.vlan[tmpVid].UntagPortmask,sizeof(rtk_portmask_t));
		//ret = rtk_vlan_extPort_get(tmpVid, &ori_etpmsk);
		//if(ret!=RT_ERR_OK)goto RET_DEF_VLAN_ERR;
		memcpy(&ori_etpmsk, &rg_db.vlan[tmpVid].Ext_portmask,sizeof(rtk_portmask_t));
		//ret = rtk_vlan_fidMode_get(tmpVid, &ori_mode);
		//if(ret!=RT_ERR_OK)goto RET_DEF_VLAN_ERR;
		ori_mode=rg_db.vlan[tmpVid].fidMode;
		
		vlan_exist=1;
	}
	else if(ret!=RT_ERR_OK)
	{
		errorno=ret;
		goto RET_VLAN_ERR;
	}

	//decide to use IVL or SVL for VLAN tag decision, IVL by untag set; SVL by DMAC2CVID
	if(lan_info->isIVL)
		fidMode=VLAN_FID_IVL;
	else
		fidMode=VLAN_FID_SVL;
	
	ret = RTK_VLAN_FIDMODE_SET(tmpVid, fidMode);		//Patch 20121129
	if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
	ret = RTK_VLAN_FID_SET(tmpVid, LAN_FID);
	if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;

	out_mac_pmask.bits[0]|=ori_pmsk.bits[0];		//add LAN port to vlan

	untag_mac_pmask.bits[0]=ori_utmsk.bits[0];			
	untag_mac_pmask.bits[0]|=lan_info->untag_mask.portmask;		//add untag set to LAN

	if((out_mac_pmask.bits[0]&(0x1<<RTK_RG_MAC_PORT_CPU))==0)		//if user didn't add CPU port, we set it to untag
	{
		out_mac_pmask.bits[0]|=(0x1<<RTK_RG_MAC_PORT_CPU);		//add CPU port to vlan
		untag_mac_pmask.bits[0]|=(0x1<<RTK_RG_MAC_PORT_CPU);	//otherwise,add CPU port to vlan untag set by user himself
	}
		
	//add bridge WAN port, if any.
	for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
	{
		if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_type==RTK_RG_BRIDGE)
		{
			//Check if there is any LAN equals the WAN's VLANID, because macBasedTagDecision is off
			errorno=RT_ERR_RG_UNBIND_BDWAN_SHOULD_EQUAL_LAN_VLAN;
			if(rg_db.systemGlobal.initParam.macBasedTagDecision==0 && rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_id!=lan_info->intf_vlan_id)
			{
				//Check if there is any other LAN equals the WAN's VLANID
				for(j=0;j<rg_db.systemGlobal.lanIntfTotalNum;j++)
				{
					if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_id==rg_db.systemGlobal.lanIntfGroup[j].p_intfInfo->p_lanIntfConf->intf_vlan_id)
						break;
				}
				if(j==rg_db.systemGlobal.lanIntfTotalNum)
					goto RET_VLAN_ERR;
			}					
				
			out_mac_pmask.bits[0]|=(0x1<<rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx);

			//use the OLDEST bridge wan's egress_vlan_tag_on to set wan port's untag setting
			if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_tag_on)			//egress tagged packet
				untag_mac_pmask.bits[0]&=(~(0x1<<rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx));	//set WAN port to 0 in untag set (tagging)
			else
				untag_mac_pmask.bits[0]|=0x1<<rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx;		//set WAN port to 1 in untag set (untagging)
		}
	}
		
	ret = RTK_VLAN_PORT_SET(tmpVid, &out_mac_pmask, &untag_mac_pmask);	
	if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;

	out_ext_pmask.bits[0]|=ori_etpmsk.bits[0];		//add LAN ext-port to vlan
	out_ext_pmask.bits[0]|=0x1;		//add ext-CPU port to vlan
	ret = RTK_VLAN_EXTPORT_SET(tmpVid, &out_ext_pmask);
	if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;	

#if 0
	//Set up port-based and extport-based VLAN ID
	tmpVid=0;
	/*protoVlanCfg.valid=1;
	protoVlanCfg.vid=DEFAULT_PPB_VLAN_START+intfIdx;
	protoVlanCfg.pri=0;			//FIXME: should I change this?
	protoVlanCfg.dei=0;
	//if this LAN has extension port (except ext-CPU port)
	if(lan_info->pppoe_passThrough == 1 && out_ext_pmask.bits[0] > 0x1)		
	{
		errorno = rtk_vlan_portProtoVlan_set(RTK_RG_MAC_PORT_CPU,PPPOE_DISCOVERY_GROUPID,&protoVlanCfg);
		if(errorno!=RT_ERR_OK)goto RET_VLAN_ERR;
		errorno = rtk_vlan_portProtoVlan_set(RTK_RG_MAC_PORT_CPU,PPPOE_SESSION_GROUPID,&protoVlanCfg);
		if(errorno!=RT_ERR_OK)goto RET_VLAN_ERR;
	}*/
	for(i=0;i<RTK_RG_PORT_MAX;i++)
	{
		if((out_mac_pmask.bits[0]&(0x1<<i)) > 0 && i != RTK_RG_MAC_PORT_CPU)
		{
			errorno=RT_ERR_RG_VLAN_GET_FAIL;
			ret = rtk_vlan_portPvid_get(i, &tmpVid);
			if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
			tmpPVid=rg_db.systemGlobal.portBasedVID[i];		//get port-based VLAN in rg_db
			errorno=RT_ERR_RG_VLAN_SET_FAIL;
			ret = rtk_vlan_portPvid_set(i, lan_info->intf_vlan_id);
			rg_db.systemGlobal.portBasedVID[i]=lan_info->intf_vlan_id;
			if(ret!=RT_ERR_OK)
			{
				rg_db.systemGlobal.portBasedVID[i]=tmpPVid;	//recovery old pvid in rg_db
				rtk_vlan_portPvid_set(i, tmpVid);			//recovery old pvid
				goto RET_VLAN_ERR;
			}

			//Set up PPPoE pass through
			/*if(lan_info->pppoe_passThrough == 1)
			{
				errorno = rtk_vlan_portProtoVlan_set(i,PPPOE_DISCOVERY_GROUPID,&protoVlanCfg);
				if(errorno!=RT_ERR_OK)goto RET_VLAN_ERR;
				errorno = rtk_vlan_portProtoVlan_set(i,PPPOE_SESSION_GROUPID,&protoVlanCfg);
				if(errorno!=RT_ERR_OK)goto RET_VLAN_ERR;
			}*/
		}
		if(i==0)continue;	//extension port 0 is CPU port, no port-based VID
		if((out_ext_pmask.bits[0]&(0x1<<i)) > 0 && i+RTK_RG_EXT_PORT0 < RTK_RG_PORT_MAX)
		{
			errorno=RT_ERR_RG_VLAN_GET_FAIL;
			ret = rtk_vlan_extPortPvid_get(i, &tmpVid);
			if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
			tmpPVid=rg_db.systemGlobal.portBasedVID[i+RTK_RG_EXT_PORT0];		//get extport-based VLAN in rg_db
			errorno=RT_ERR_RG_VLAN_SET_FAIL;
			ret = rtk_vlan_extPortPvid_set(i, lan_info->intf_vlan_id);
			rg_db.systemGlobal.portBasedVID[i+RTK_RG_EXT_PORT0]=lan_info->intf_vlan_id;
			if(ret!=RT_ERR_OK)
			{
				rg_db.systemGlobal.portBasedVID[i+RTK_RG_EXT_PORT0]=tmpPVid;	//recovery old ext pvid in rg_db
				rtk_vlan_extPortPvid_set(i, tmpVid);		//recovery old pvid
				goto RET_VLAN_ERR;
			}
		}
	}
#endif

	//Check for bridge WAN, add Lan member to their VLAN
	for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
	{
		if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_type==RTK_RG_BRIDGE)
		{
			//ori_pmsk.bits[0]|=(0x1<<rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.wan_port_idx);
			tmpVid=rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_id;
			memcpy(&ori_pmsk, &rg_db.vlan[tmpVid].MemberPortmask,sizeof(rtk_portmask_t));
			memcpy(&ori_utmsk, &rg_db.vlan[tmpVid].UntagPortmask,sizeof(rtk_portmask_t));
			memcpy(&ori_etpmsk, &rg_db.vlan[tmpVid].Ext_portmask,sizeof(rtk_portmask_t));

			//Let LAN port become active in WAN's VLAN member port mask
			out_mac_pmask.bits[0]|=ori_pmsk.bits[0];
			out_ext_pmask.bits[0]|=ori_etpmsk.bits[0];
			//We do not add Lan port to untag set
			
			errorno=RT_ERR_RG_VLAN_SET_FAIL;
			ret = RTK_VLAN_PORT_SET(tmpVid, &out_mac_pmask, &ori_utmsk);
			if(ret!=RT_ERR_OK)goto RET_BD_WAN_VLAN_ERR;
			ret = RTK_VLAN_EXTPORT_SET(tmpVid,&out_ext_pmask);
			if(ret!=RT_ERR_OK)goto RET_BD_WAN_VLAN_ERR;
		}
	}

	//Set up interface table
	errorno=RT_ERR_RG_INTF_SET_FAIL;
	ret = RTK_L34_NETIFTABLE_SET(intfIdx, &intfEntry);
	if(ret!=RT_ERR_OK)goto RET_INTF_ERR;

	//Set up Routing table
	if(ipv4Enable==1)
	{
		bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
		rtEntry.netifIdx=intfIdx;
		rtEntry.valid=1;
		rtEntry.process=L34_PROCESS_CPU;		//default add to sw table
		rtEntry.internal=1;
		rtEntry.ipAddr=lan_info->ip_addr&lan_info->ip_network_mask;		//20130301-store IP addr after masked
		rtEntry.rt2waninf=0;
		input_ipmsk=lan_info->ip_network_mask;
		RG_ONE_COUNT(input_ipmsk);
		rtEntry.ipMask=input_ipmsk-1;
		/*input_ipmsk=lan_info->ip_network_mask;
		for(i=32;i>0;i--)
		{
			input_ipmsk>>=1;
			if((input_ipmsk&0x1) == 1)
			{
				rtEntry.ipMask = i-1;
				break;
			}
		}*/
		
		//Check for ARP table for enough entry
		// TODO:Check for ARP range and add to rg_db.systemGlobal.routingArpInfoArray
		bzero(&newAddingEntry,sizeof(rtk_rg_routing_arpInfo_t));
		newAddingEntry.routingIdx=rtIdx;
		newAddingEntry.intfIdx=intfIdx;
		newAddingEntry.notMask=~lan_info->ip_network_mask;
		newAddingEntry.bitNum=32-input_ipmsk;
		newAddingEntry.isLan=1;

		if(newAddingEntry.bitNum <= 8)	//if need more than or equal to 512 entries, recorded in fwdEngine
		{
			errorno=_rtk_rg_addArpRoutingArray(&newAddingEntry,lan_info->ip_addr,lan_info->intf_vlan_id);
			if(errorno==RT_ERR_RG_OK)
			{
				rtEntry.process = L34_PROCESS_ARP;
				rtEntry.arpStart = newAddingEntry.arpStart;
				rtEntry.arpEnd = newAddingEntry.arpEnd;
			}
			else if(errorno!=RT_ERR_RG_ADD_ARP_TO_SW_TABLE)		//for sw table, routing entry just set process to CPU
				goto RET_INVALID_ARP;
		}
		else
			DEBUG("HW table is not enough...will add to software ARP table!");
		
		errorno=RT_ERR_RG_ROUTE_SET_FAIL;
		ret = RTK_L34_ROUTINGTABLE_SET(rtIdx, &rtEntry);
		if(ret!=RT_ERR_OK)goto RET_ROUTING_ERR;

		//1 FIXME: patch for DA==GatewayMac will hit layer2 unknown DA, if action is trap
		//Create Lan gateway STATIC MAC
		errorno=RT_ERR_RG_CREATE_GATEWAY_LUT_FAIL;
		ret = _rtk_rg_createGatewayMacEntry(lan_info->gmac.octet,lan_info->intf_vlan_id,lan_info->untag_mask.portmask);
		if(ret==FAIL)goto RET_ROUTING_ERR;
	}
	if(ipv6Enable==1)
	{
		bzero(&rtv6Entry, sizeof(rtk_ipv6Routing_entry_t));
		rtv6Entry.valid=1;
		rtv6Entry.type=L34_IPV6_ROUTE_TYPE_LOCAL;
		rtv6Entry.nhOrIfidIdx=intfIdx;
		rtv6Entry.ipv6PrefixLen=lan_info->ipv6_network_mask_length;
		memcpy(&rtv6Entry.ipv6Addr,&lan_info->ipv6_addr,sizeof(rtk_ipv6_addr_t));
		rtv6Entry.rt2waninf=0;	//local route, routing to LAN

		errorno=RT_ERR_RG_ROUTE_SET_FAIL;
		ret = RTK_L34_IPV6ROUTINGTABLE_SET(rtv6Idx,&rtv6Entry);
		if(ret!=RT_ERR_OK)goto RET_ROUTING_ERR;

		//Set up link-local address routing to CPU
		bzero(&rtv6Entry, sizeof(rtk_ipv6Routing_entry_t));
		rtv6Entry.valid=1;
		rtv6Entry.type=L34_IPV6_ROUTE_TYPE_TRAP;
		rtv6Entry.nhOrIfidIdx=intfIdx;
		
		rtv6Entry.ipv6PrefixLen=64;
		rtv6Entry.ipv6Addr.ipv6_addr[0]=0xfe;
		rtv6Entry.ipv6Addr.ipv6_addr[1]=0x80;
		
		rtv6Entry.rt2waninf=0;	//local route, routing to LAN

		errorno=RT_ERR_RG_ROUTE_SET_FAIL;
		ret = RTK_L34_IPV6ROUTINGTABLE_SET(v6TrapIdx,&rtv6Entry);
		if(ret!=RT_ERR_OK)goto RET_ROUTING_ERR;
	}

	// TODO:Call the initParam's routingAddByHwCallBack
	if(ipv4Enable==1)
	{	
		if(rg_db.systemGlobal.initParam.routingAddByHwCallBack != NULL)
		{
			cb_routEt.dest_ip=lan_info->ip_addr;
			cb_routEt.ip_mask=lan_info->ip_network_mask;
			cb_routEt.nexthop=0;	//interface route
			cb_routEt.wan_intf_idx=intfIdx;
			rg_db.systemGlobal.initParam.routingAddByHwCallBack(&cb_routEt);
		}
	}
	// TODO:Call the initParam's v6RoutingAddByHwCallBack
	if(ipv6Enable==1)
	{	
		if(rg_db.systemGlobal.initParam.v6RoutingAddByHwCallBack != NULL)
		{
			memcpy(&cb_routv6Et.dest_ip,&lan_info->ipv6_addr,sizeof(rtk_ipv6_addr_t));
			cb_routv6Et.prefix_len=lan_info->ipv6_network_mask_length;
			cb_routv6Et.NhOrIntfIdx=intfIdx;
			cb_routv6Et.type=L34_IPV6_ROUTE_TYPE_LOCAL;
			rg_db.systemGlobal.initParam.v6RoutingAddByHwCallBack(&cb_routv6Et);
		}
	}
	
	*intf_idx = intfIdx;

	//store information in Global variable
	rg_db.systemGlobal.lanIntfGroup[rg_db.systemGlobal.lanIntfTotalNum].index=intfIdx;
	rg_db.systemGlobal.lanIntfGroup[rg_db.systemGlobal.lanIntfTotalNum].p_intfInfo=&rg_db.systemGlobal.interfaceInfo[intfIdx];
	rg_db.systemGlobal.interfaceInfo[intfIdx].lan_or_wan_index=rg_db.systemGlobal.lanIntfTotalNum;
	rg_db.systemGlobal.lanIntfTotalNum++;

	rg_db.systemGlobal.interfaceInfo[intfIdx].valid=1;
	sprintf(rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.intf_name, "LAN%d",intfIdx);
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.is_wan=0;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.ingress_byte_count=0;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.ingress_packet_count=0;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.egress_byte_count=0;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.egress_packet_count=0;

	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.ip_version=lan_info->ip_version;
	memcpy(rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.gmac.octet, lan_info->gmac.octet,ETHER_ADDR_LEN);
	if(ipv4Enable==1)
	{
		rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.ip_addr=lan_info->ip_addr;
		rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.ip_network_mask=lan_info->ip_network_mask;
	}
	if(ipv6Enable==1)
	{
		memcpy(&rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.ipv6_addr,&lan_info->ipv6_addr,sizeof(rtk_ipv6_addr_t));
		rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.ipv6_network_mask_length=lan_info->ipv6_network_mask_length;
	}
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.port_mask=lan_info->port_mask;
	//rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.extport_mask=lan_info->extport_mask;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.intf_vlan_id=lan_info->intf_vlan_id;
	//rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.pppoe_passThrough=lan_info->pppoe_passThrough;
#if 0
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.dhcp_server_enable=lan_info->dhcp_server_enable;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.lease_time=lan_info->lease_time;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.dhcp_start_ip_addr=lan_info->dhcp_start_ip_addr;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.dhcp_end_ip_addr=lan_info->dhcp_end_ip_addr;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.dhcp_port_binding_mask=lan_info->dhcp_port_binding_mask;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.dhcp_extport_binding_mask=lan_info->dhcp_extport_binding_mask;
#endif
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.mtu=lan_info->mtu;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf.isIVL=lan_info->isIVL;


	rg_db.systemGlobal.interfaceInfo[intfIdx].p_lanIntfConf=&rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.lan_intf; 	//short-cut of lan interface structure

	//Check PPPoE Pass through
	_rtk_rg_refreshPPPoEPassThroughLanOrWanPortMask();

	//Update PVID
	_rtk_rg_updatePortBasedVIDByLanOrder(rg_db.vlan[lan_info->intf_vlan_id].MemberPortmask, rg_db.vlan[lan_info->intf_vlan_id].Ext_portmask);

	//add lan-interfcae callback to sync protocal-stack
	if(rg_db.systemGlobal.initParam.interfaceAddByHwCallBack != NULL)
	{
		//rtk_rg_intfInfo_t intfInfo;
		//bzero(&intfInfo,sizeof(intfInfo));
		//memcpy(&intfInfo, &rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo, sizeof(intfInfo));
		rg_db.systemGlobal.initParam.interfaceAddByHwCallBack(&rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo,&intfIdx);
	}

	errorno=RT_ERR_RG_OK;
	goto RET_SUCCESS;

RET_ROUTING_ERR:
    //Delete the routing entry
    if(rtIdx>=0)
	{
	    bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
	    RTK_L34_ROUTINGTABLE_SET(rtIdx, &rtEntry);	
    }
	if(rtv6Idx>=0)
	{
		bzero(&rtv6Entry, sizeof(rtk_ipv6Routing_entry_t));
		RTK_L34_IPV6ROUTINGTABLE_SET(rtv6Idx,&rtv6Entry);
	}
	if(v6TrapIdx>=0)
	{
		bzero(&rtv6Entry, sizeof(rtk_ipv6Routing_entry_t));
		RTK_L34_IPV6ROUTINGTABLE_SET(v6TrapIdx,&rtv6Entry);
	}
RET_INVALID_ARP:
RET_INTF_ERR:
    //Delete the interface entry
    bzero(&intfEntry, sizeof(rtk_l34_netif_entry_t));
    RTK_L34_NETIFTABLE_SET(intfIdx, &intfEntry);
RET_BD_WAN_VLAN_ERR:
	RTK_VLAN_PORT_SET(tmpVid, &ori_pmsk, &ori_utmsk);
    RTK_VLAN_EXTPORT_SET(tmpVid, &ori_etpmsk);
RET_VLAN_ERR:
    //Delete the VLAN created, or recovery its member port if exist
	/*if(lan_info->pppoe_passThrough == 1)
	{
		bzero(&protoVlanCfg,sizeof(rtk_vlan_protoVlanCfg_t));
		if(out_ext_pmask.bits[0] > 0x1)		
		{		
			rtk_vlan_portProtoVlan_set(RTK_RG_MAC_PORT_CPU,PPPOE_DISCOVERY_GROUPID,&protoVlanCfg);
			rtk_vlan_portProtoVlan_set(RTK_RG_MAC_PORT_CPU,PPPOE_SESSION_GROUPID,&protoVlanCfg);
		}
		for(i=0;i<RTK_RG_PORT_CPU;i++)
		{
			if((out_mac_pmask.bits[0]&(0x1<<i)) > 0)
			{
				rtk_vlan_portProtoVlan_set(i,PPPOE_DISCOVERY_GROUPID,&protoVlanCfg);
				rtk_vlan_portProtoVlan_set(i,PPPOE_SESSION_GROUPID,&protoVlanCfg);
			}
		}
	}*/
    if(vlan_exist)
    {
        RTK_VLAN_PORT_SET(intfEntry.vlan_id, &ori_pmsk, &ori_utmsk);
        RTK_VLAN_EXTPORT_SET(intfEntry.vlan_id, &ori_etpmsk);
        RTK_VLAN_FIDMODE_SET(intfEntry.vlan_id, ori_mode);
    }
    else
    {
        RTK_VLAN_DESTROY(intfEntry.vlan_id);
    }
/*RET_DEF_VLAN_ERR:
	//Recovery Default VLAN setting
	RTK_VLAN_PORT_SET(DEFAULT_LAN_VLAN, &ori_CPU_member_mask, &ori_CPU_untag_mask);*/

RET_SUCCESS:

	//------------------ Critical Section End -----------------------//
	rg_unlock(&rg_kernel.interfaceLock);

    return errorno;
}

#if 0
int32 rtk_rg_dhcpServerStaticAlloc_add(ipaddr_t ipaddr, rtk_mac_t *macaddr,int *static_idx)
{
    int i;

    //Check input param
    if(static_idx == NULL || macaddr == NULL)
        return RT_ERR_RG_NULL_POINTER;
    if(ipaddr == 0)
        return RT_ERR_RG_INVALID_PARAM;
    if(macaddr->octet[0]==0 && macaddr->octet[1]==0 && macaddr->octet[2]==0 &&
            macaddr->octet[3]==0 && macaddr->octet[4]==0 && macaddr->octet[5]==0)
        return RT_ERR_RG_INVALID_PARAM;

    //Store ip and macaddr in structure
    for(i=0; i<STATIC_DHCP_ALLOC_NUM; i++)
    {
        if(_DHCP_STATIC[i].valid != 0)
            continue;

        _DHCP_STATIC[i].ip=ipaddr;
        memcpy(_DHCP_STATIC[i].mac.octet, macaddr->octet, 6);
        _DHCP_STATIC[i].valid=1;
    }
    if(i == STATIC_DHCP_ALLOC_NUM)return RT_ERR_RG_ENTRY_FULL;

    //Return the index of new added ip-mac structure
    *static_idx = i;

    return RT_ERR_RG_OK;
}

int32 rtk_rg_dhcpServerStaticAlloc_del(int static_idx)
{
    //Check param
    if(static_idx<0 || static_idx>=STATIC_DHCP_ALLOC_NUM)
        return RT_ERR_RG_INVALID_PARAM;
    if(_DHCP_STATIC[static_idx].valid == 0)
        return RT_ERR_RG_INVALID_PARAM;

    _DHCP_STATIC[static_idx].valid=0;		//we just turn off the valid bit

    return RT_ERR_RG_OK;
}

int32 rtk_rg_dhcpServerStaticAlloc_find(ipaddr_t *ipaddr, rtk_mac_t *macaddr, int *idx)
{
    //Check param
    int i;
    if(idx == NULL || ipaddr == NULL || macaddr == NULL)
        return RT_ERR_RG_NULL_POINTER;
    if(*idx<0 || *idx>=STATIC_DHCP_ALLOC_NUM)
        return RT_ERR_RG_INVALID_PARAM;

    //Find the first available one from idx
    for(i=*idx; i<STATIC_DHCP_ALLOC_NUM; i++)
    {
        if(_DHCP_STATIC[i].valid==0)
            continue;

        *ipaddr=_DHCP_STATIC[i].ip;
        memcpy(macaddr->octet, _DHCP_STATIC[i].mac.octet, 6);
    }
    if(i==STATIC_DHCP_ALLOC_NUM)return RT_ERR_RG_STATIC_NOT_FOUND;

    //Return the first available index from idx
    *idx = i;

    return RT_ERR_RG_OK;
}
#endif

//WAN Interface
int32 rtk_rg_wanInterface_add(rtk_rg_wanIntfConf_t *wanintf, int *wan_intf_idx)
{
	int ret,i,vlanID,errorno,vlan_exist=0,tmpVid,tmpPVid,rgPVid;
	unsigned int intfIdx,tmppmsk,tmpexpmsk,nxpIdx=0;
	rtk_portmask_t mbpmsk,utpmsk,etpmsk,all_lan_pmsk,all_lan_etpmsk,all_lan_utagpmsk,wanPmsk;	//member, untag, extension port masks
	rtk_portmask_t ori_pmsk,ori_utmsk,ori_etpmsk;
	//rtk_portmask_t ori_CPU_member_mask,ori_CPU_untag_mask;//,ori_CPU_ext_mask;
	rtk_l34_netif_entry_t intfEntry;
	//rtk_l34_routing_entry_t rtEntry,ori_rtEt;
	rtk_fidMode_t ori_mode=VLAN_FID_IVL;
	rtk_enable_t ori_prien=DISABLED;
	rtk_pri_t ori_pri=0;
	rtk_portmask_t out_mac_pmask,out_ext_pmask;
	rtk_l34_nexthop_entry_t nxpEt;
	rtk_l34_pppoe_entry_t pppoeEt;
	rtk_wanType_entry_t wantEt;
	rtk_fidMode_t fidMode;
	//rtk_vlan_protoVlanCfg_t protoVlanCfg;
#ifdef CONFIG_APOLLO_RLE0371
#else
	unsigned int j=0,count=0, bindIdx=0;
	rtk_binding_entry_t pbindEt;
	rtk_rg_bindingEntry_t cb_bindEt;
#endif
	
	//Check parameter
	//if(rg_db.systemGlobal.lanIntfTotalNum==0)		//Check if LAN added before WAN creation
		//return RT_ERR_RG_LAN_NOT_EXIST;
	if(wanintf == NULL || wan_intf_idx == NULL)
		return RT_ERR_RG_NULL_POINTER;
	if(wanintf->egress_vlan_id <= 0 || wanintf->egress_vlan_id >= 4095 ||
		wanintf->egress_vlan_id == DEFAULT_CPU_VLAN /*|| wanintf->egress_vlan_id == DEFAULT_LAN_VLAN*/ || wanintf->egress_vlan_id == DEFAULT_WAN_PVID)		//invalid vid
		return RT_ERR_RG_INVALID_PARAM;
	if(wanintf->egress_vlan_id >= DEFAULT_PPB_VLAN_START && wanintf->egress_vlan_id < (DEFAULT_PPB_VLAN_START+MAX_NETIF_SW_TABLE_SIZE))
		return RT_ERR_RG_INVALID_PARAM;
	if(wanintf->wan_port_idx < RTK_RG_MAC_PORT0 || wanintf->wan_port_idx >= RTK_RG_MAC_PORT_MAX)
		return RT_ERR_RG_INVALID_PARAM;
	if(rg_db.systemGlobal.initParam.macBasedTagDecision)
	{
		if(wanintf->isIVL)		//IVL can not be set when DMAC2CVID is trun on
			return RT_ERR_RG_INVALID_PARAM;
	}
	else if(wanintf->port_binding_mask.portmask>0)		//set port-binding but global switch is off
		return RT_ERR_RG_BIND_WITH_UNBIND_WAN;
#ifdef CONFIG_APOLLO_RLE0371
	//patch for 0371 problem with PON port
	if(wanintf->wan_port_idx == RTK_RG_MAC_PORT_PON)
		return RT_ERR_RG_PON_INVALID;
#endif
#if 0
	//test chip should not restrict wan port to PON or RGMII, since it do not has binding function
	if(wanintf->wan_port_idx<RTK_RG_MAC_PORT_PON || wanintf->wan_port_idx>RTK_RG_MAC_PORT_RGMII)
		return RT_ERR_RG_INVALID_PARAM;
#endif
	/*if((wanintf->wan_port_idx==RTK_RG_MAC_PORT_PON && RG_GLB_WAN_PON_USED == 1) || 			//wan port used
		(wanintf->wan_port_idx==RTK_RG_MAC_PORT_RGMII && RG_GLB_WAN_RGMII_USED == 1))
		return RT_ERR_RG_INVALID_PARAM;*/
	if(wanintf->port_binding_mask.portmask>=(1<<RTK_RG_PORT_MAX))
		return RT_ERR_RG_INVALID_PARAM;
	if((wanintf->port_binding_mask.portmask&(1<<RTK_RG_MAC_PORT_CPU)) == 0x1)	//port-binding should not be CPU port
		return RT_ERR_RG_INVALID_PARAM;
	
	//Check if there is default route setup before
	//if(wanintf->default_gateway_on==1 && rg_db.systemGlobal.defaultRouteSet!=-1)
		//return RT_ERR_RG_DEF_ROUTE_EXIST;

	//Check if we set two untag bridge WAN at the same port
	if(wanintf->egress_vlan_tag_on==0 && wanintf->wan_type==RTK_RG_BRIDGE)
	{
		for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
		{
			if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_tag_on==0 &&
				rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_type==RTK_RG_BRIDGE &&
				rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx==wanintf->wan_port_idx)
				return RT_ERR_RG_UNTAG_BRIDGEWAN_TWICE;
		}
	}

	//Check if we didn't add LAN interface before un-binding bridge WAN
	//if(wanintf->wan_type==RTK_RG_BRIDGE && wanintf->port_binding_mask.portmask==0 && rg_db.systemGlobal.lanIntfTotalNum==0)
		//return RT_ERR_RG_LAN_NOT_EXIST;

	//Check if VLAN init
	if(rg_db.systemGlobal.vlanInit==0)
		return RT_ERR_RG_NOT_INIT;

#ifdef CONFIG_APOLLO_RLE0371
#else
	//Check VLAN-binding use this VLAN or not
	for(i=0;i<MAX_BIND_SW_TABLE_SIZE;i++)
		if(rg_db.bind[i].valid==1 && rg_db.bind[i].rtk_bind.vidLan==wanintf->egress_vlan_id)
			return RT_ERR_RG_VLAN_USED_BY_VLANBINDING;
	//Checl Customer VLAN use this VLAN or not
	if(rg_db.vlan[wanintf->egress_vlan_id].valid && rg_db.vlan[wanintf->egress_vlan_id].addedAsCustomerVLAN)
		return RT_ERR_RG_VLAN_USED_BY_CVLAN;
#endif

	//------------------ Critical Section start -----------------------//
	rg_lock(&rg_kernel.interfaceLock);
	
	wanPmsk.bits[0]=0x1<<wanintf->wan_port_idx;	//pon is the 4th bit and RGMII is 5th
	all_lan_pmsk=wanPmsk;
	all_lan_etpmsk.bits[0]=0;
	all_lan_utagpmsk.bits[0]=0;

	//Convert RG portmask to RTK portmask
	_rtk_rg_portmask_translator(wanintf->port_binding_mask,&out_mac_pmask,&out_ext_pmask);
	tmppmsk=out_mac_pmask.bits[0];
	tmpexpmsk=out_ext_pmask.bits[0];	
#ifdef CONFIG_APOLLO_RLE0371
#else
	//Check if there is other interface set same port as binding port
	for(i=0;i<MAX_BIND_SW_TABLE_SIZE;i++)
	{
		//bzero(&pbindEt, sizeof(rtk_binding_entry_t));
		//ret = dal_apollomp_l34_bindingTable_get(i, &pbindEt);		//FIXME:no RTK APIs
		//ret = rtk_l34_bindingTable_get(i, &pbindEt);
		//if(ret==RT_ERR_CHIP_NOT_SUPPORTED)return RT_ERR_RG_CHIP_NOT_SUPPORT;
		//if(ret!=RT_ERR_OK)return RT_ERR_RG_PORT_BIND_GET_FAIL;

		//we should not assign the same port binding with different Wan interface
		if(rg_db.bind[i].valid==1 && (rg_db.bind[i].rtk_bind.portMask.bits[0]&tmppmsk)>0 && rg_db.bind[i].rtk_bind.vidLan==0) 
			return RT_ERR_RG_INVALID_PARAM;
		if(rg_db.bind[i].valid==1 && (rg_db.bind[i].rtk_bind.extPortMask.bits[0]&tmpexpmsk)>0 && rg_db.bind[i].rtk_bind.vidLan==0)
			return RT_ERR_RG_INVALID_PARAM;
	}
#endif

	//Check interface table available or not
	if(rg_db.systemGlobal.wanIdxForReset == -1)
	{
		for(i=0;i<MAX_NETIF_SW_TABLE_SIZE;i++)
		{
			//bzero(&intfEntry, sizeof(rtk_l34_netif_entry_t));
			//ret = rtk_l34_netifTable_get(i, &intfEntry);
			//if(ret!=RT_ERR_OK)return RT_ERR_RG_INTF_GET_FAIL;
			
			//if(intfEntry.valid == 0)
			if(rg_db.systemGlobal.interfaceInfo[i].valid == 0)
				break;
		}
		if(i==MAX_NETIF_SW_TABLE_SIZE)
			return RT_ERR_RG_ENTRY_FULL;

		intfIdx=i;
		//rtlglue_printf("rg_db.systemGlobal.wanIdxForReset == -1(%d): intfIdx=%d \n",__LINE__,intfIdx);
	}
	else
	{
		//we are reset the added wan interface, so give me the same index!!
		intfIdx = rg_db.systemGlobal.wanIdxForReset;
		rg_db.systemGlobal.wanIdxForReset = -1;
		
		//rtlglue_printf("else(%d): intfIdx=%d \n",__LINE__,intfIdx);
	}

	//Set up interface table
	errorno=RT_ERR_RG_INTF_SET_FAIL;
	bzero(&intfEntry, sizeof(rtk_l34_netif_entry_t));
	vlanID=wanintf->egress_vlan_id;
	intfEntry.valid=1;
	memcpy(intfEntry.gateway_mac.octet, wanintf->gmac.octet,ETHER_ADDR_LEN);
	intfEntry.mac_mask=0x7;	//no mask
	intfEntry.vlan_id=vlanID;
	intfEntry.enable_rounting=1;
	intfEntry.mtu=1500;				//This dummy value should be fine since we don't care MTU in L2 bridging!!
#ifdef CONFIG_APOLLO_RLE0371
#else
	//Patch for 0601 and 6266, when binding to interface happened,
	//the packet size have 2 byte would't decrease, causing TRAP reason 224.
	//therefore the hardware setting should be set as preferred value plus 2 here
	if(rg_db.systemGlobal.initParam.macBasedTagDecision)
		intfEntry.mtu=1502;				
#endif

	DEBUG("Add NETIF[%d] VLAN[%d]\n",intfIdx,intfEntry.vlan_id);
	ret = RTK_L34_NETIFTABLE_SET(intfIdx, &intfEntry);
	DEBUG("ret:%x\n",ret);
	if(ret!=RT_ERR_OK)goto RET_INTF_ERR;

	//Check pppoe pass through
	/*errorno=RT_ERR_RG_VLAN_SET_FAIL;
	tmpVid=0;
	mbpmsk.bits[0]=wanPmsk.bits[0];
	etpmsk.bits[0]=0;
	for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
	{
		if(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->pppoe_passThrough == 1)
		{
			//add WAN port to their PPB VLAN
			tmpVid=DEFAULT_PPB_VLAN_START+rg_db.systemGlobal.lanIntfGroup[i].index;
			memcpy(&ori_pmsk, &rg_db.vlan[tmpVid].MemberPortmask,sizeof(rtk_portmask_t));
			memcpy(&ori_utmsk, &rg_db.vlan[tmpVid].UntagPortmask,sizeof(rtk_portmask_t));
			memcpy(&ori_etpmsk, &rg_db.vlan[tmpVid].Ext_portmask,sizeof(rtk_portmask_t));

			//keep lan mbr and extension port
			mbpmsk.bits[0]|=ori_pmsk.bits[0];
			etpmsk.bits[0]|=ori_etpmsk.bits[0];

			if(wanintf->wan_port_idx <= RTK_RG_PORT_CPU)
			{
				ori_pmsk.bits[0]|=wanPmsk.bits[0];
				ori_utmsk.bits[0]|=wanPmsk.bits[0];
			}
			else
			{
				ori_etpmsk.bits[0]|=(0x1<<(wanintf->wan_port_idx-RTK_RG_EXT_PORT0));
				ori_utmsk.bits[0]|=(0x1<<RTK_RG_PORT_CPU);
			}

			ret = RTK_VLAN_PORT_SET(tmpVid, &ori_pmsk, &ori_utmsk);
			if(ret!=RT_ERR_OK)goto RET_PPPOE_PASS_ERR;
			ret = RTK_VLAN_EXTPORT_SET(tmpVid, &ori_etpmsk);			
			if(ret!=RT_ERR_OK)goto RET_PPPOE_PASS_ERR;
		}
	}
	if(tmpVid!=0)	//has LAN turn on pppoe pass through 
	{
		protoVlanCfg.valid=1;
		protoVlanCfg.vid=DEFAULT_PPB_VLAN_WAN;
		protoVlanCfg.pri=0;			//FIXME: should I change this?
		protoVlanCfg.dei=0;
		if(rg_db.systemGlobal.wanIntfTotalNum==0)	//first wan at all
		{
			tmpVid=DEFAULT_PPB_VLAN_WAN;
			RTK_VLAN_CREATE(tmpVid);
			RTK_VLAN_FIDMODE_SET(tmpVid, VLAN_FID_IVL);
			//RTK_VLAN_FID_SET(tmpVid, LAN_FID);	//IVL no need FID
			RTK_VLAN_PORT_SET(tmpVid, &mbpmsk, &mbpmsk);	//all ports here are untag
			RTK_VLAN_EXTPORT_SET(tmpVid, &etpmsk);

			rtk_vlan_portProtoVlan_set(wanintf->wan_port_idx,PPPOE_DISCOVERY_GROUPID,&protoVlanCfg);
			rtk_vlan_portProtoVlan_set(wanintf->wan_port_idx,PPPOE_SESSION_GROUPID,&protoVlanCfg);
		}
		else
		{
			ret=0;
			for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
			{
				if(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.wan_port_idx == wanintf->wan_port_idx)
				{
					ret=1;
					break;
				}
			}
			if(ret==0)	//first wan used this port
			{
				mbpmsk.bits[0]=rg_db.vlan[DEFAULT_PPB_VLAN_WAN].MemberPortmask.bits[0];
				mbpmsk.bits[0]|=wanPmsk.bits[0];
				RTK_VLAN_PORT_SET(tmpVid, &mbpmsk, &mbpmsk);	//add this wan port to the vlan

				rtk_vlan_portProtoVlan_set(wanintf->wan_port_idx,PPPOE_DISCOVERY_GROUPID,&protoVlanCfg);
				rtk_vlan_portProtoVlan_set(wanintf->wan_port_idx,PPPOE_SESSION_GROUPID,&protoVlanCfg);
			}
			
		}
	}*/

	//Check if Bridge mode
	if(wanintf->wan_type == RTK_RG_BRIDGE)
	{
		//Check un-bind bridge WAN's VLANID equals to LAN's VLANID or not
		if(rg_db.systemGlobal.initParam.macBasedTagDecision==0 && rg_db.systemGlobal.lanIntfTotalNum>0)
		{
			errorno=RT_ERR_RG_UNBIND_BDWAN_SHOULD_EQUAL_LAN_VLAN;
			for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
			{
				if(wanintf->egress_vlan_id == rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->intf_vlan_id)
					break;
			}
			if(i==rg_db.systemGlobal.lanIntfTotalNum)	//not match 
				goto RET_INTF_ERR;
		}
		//Let fwdEngine to do the broadcast and unknown UC flooding to multiwan
		/*if(rg_db.systemGlobal.bridgeWanNum==0)
		{
			mbpmsk.bits[0]=0x1<<RTK_RG_PORT_CPU;
			assert_ok(rtk_l2_lookupMissFloodPortMask_set(DLF_TYPE_BCAST,&mbpmsk));		//set broadcast
			assert_ok(rtk_l2_lookupMissFloodPortMask_set(DLF_TYPE_UCAST,&mbpmsk));
		}*/
		
		/*//Check if there is LAN interface has same VLAN id with me
		for(i=0;i<8;i++)
		{
			if(rg_db.systemGlobal.interfaceInfo[i].is_wan==0 && rg_db.systemGlobal.interfaceInfo[i].storedInfo.lan_intf.intf_vlan_id != 0)
			{
				if(rg_db.systemGlobal.interfaceInfo[i].storedInfo.lan_intf.intf_vlan_id == vlanID)
					break;//match
			}
		}
		if(i==8)goto RET_INTF_ERR;*/

#if 0
		//Add WAN port to DEFAULT LAN VLAN
		//errorno=RT_ERR_RG_VLAN_GET_FAIL;
		//mbpmsk.bits[0]=0;
		//ret = rtk_vlan_port_get(DEFAULT_LAN_VLAN, &ori_CPU_member_mask, &ori_CPU_untag_mask);
		//if(ret!=RT_ERR_OK)goto RET_INTF_ERR;
		memcpy(&ori_CPU_member_mask, &rg_db.vlan[DEFAULT_LAN_VLAN].MemberPortmask,sizeof(rtk_portmask_t));
		memcpy(&ori_CPU_untag_mask, &rg_db.vlan[DEFAULT_LAN_VLAN].UntagPortmask,sizeof(rtk_portmask_t));

		mbpmsk.bits[0]=ori_CPU_member_mask.bits[0];
		mbpmsk.bits[0]|=wanPmsk.bits[0];		//add to DEFAULT LAN VLAN

		errorno=RT_ERR_RG_VLAN_SET_FAIL;
		ret = RTK_VLAN_PORT_SET(DEFAULT_LAN_VLAN, &mbpmsk, &ori_CPU_untag_mask);
		if(ret!=RT_ERR_OK)goto RET_DEFALT_VLAN_ERR;
#endif 
		if(rg_db.systemGlobal.initParam.macBasedTagDecision==1)		//only add to LAN interface if MAC-based
		{
			//Get all Lan interface and add WAN port to their VLAN
			mbpmsk.bits[0]=0;
			for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
			{
				//rtlglue_printf("@@@@@@@@@@@@@@@@@@@@@@@@@@ i = %d\n",i);		
				//rtlglue_printf("@@@@@@@@@@@@@@@@@@@@@@@@@@ vid = %d\n",rg_db.systemGlobal.interfaceInfo[i].storedInfo.lan_intf.intf_vlan_id);
				//errorno=RT_ERR_RG_VLAN_GET_FAIL;
				//ret = rtk_vlan_port_get(rg_db.systemGlobal.interfaceInfo[i].storedInfo.lan_intf.intf_vlan_id, &mbpmsk, &utpmsk);
				//if(ret!=RT_ERR_OK)goto RET_BRIDGE_ERR;
				tmpVid=rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->intf_vlan_id;
				memcpy(&mbpmsk, &rg_db.vlan[tmpVid].MemberPortmask,sizeof(rtk_portmask_t));
				memcpy(&utpmsk, &rg_db.vlan[tmpVid].UntagPortmask,sizeof(rtk_portmask_t));
				//ret = rtk_vlan_extPort_get(rg_db.systemGlobal.interfaceInfo[i].storedInfo.lan_intf.intf_vlan_id,&etpmsk);
				//if(ret!=RT_ERR_OK)goto RET_BRIDGE_ERR;
				memcpy(&etpmsk, &rg_db.vlan[tmpVid].Ext_portmask,sizeof(rtk_portmask_t));

				//Let WAN port become active in LAN's VLAN member port mask
				//mbpmsk.bits[0] |= wanintf->wan_port_mask.bits[0];
				mbpmsk.bits[0] |= wanPmsk.bits[0];
				//add wan port to lan's untag set by egress_tag_on setting
				for(j=0;j<rg_db.systemGlobal.wanIntfTotalNum;j++)
				{
					if(rg_db.systemGlobal.wanIntfGroup[j].p_wanIntfConf->wan_type==RTK_RG_BRIDGE && wanPmsk.bits[0]&(0x1<<rg_db.systemGlobal.wanIntfGroup[j].p_wanIntfConf->wan_port_idx))
						break;
				}
				if(j==rg_db.systemGlobal.wanIntfTotalNum)		//no before bridge WAN added at this port
				{
					if(wanintf->egress_vlan_tag_on)			//egress tagged packet
						utpmsk.bits[0]&=(~(wanPmsk.bits[0]));	//set WAN port to 0 in untag set (tagging)
					else
						utpmsk.bits[0]|=wanPmsk.bits[0];		//set WAN port to 1 in untag set (untagging)
				}
				
				//rtlglue_printf("the mbpmsk is %x\n",mbpmsk.bits[0]);
				//rtlglue_printf("the wanpmsk is %x\n",wanPmsk.bits[0]);
				//rtlglue_printf("the wan port is %d, mask=%x\n",wanintf->wan_port_idx,0x1<<wanintf->wan_port_idx);

				//Keep Lan's member port mask in all_lan_pmsk and all_lan_etpmsk
				all_lan_pmsk.bits[0] |= mbpmsk.bits[0];
				all_lan_etpmsk.bits[0] |= etpmsk.bits[0];
				all_lan_utagpmsk.bits[0] |= utpmsk.bits[0];

				errorno=RT_ERR_RG_VLAN_SET_FAIL;
				ret = RTK_VLAN_PORT_SET(tmpVid, &mbpmsk, &utpmsk);
				if(ret!=RT_ERR_OK)goto RET_BRIDGE_ERR;
			}
		}
	}

#ifdef CONFIG_APOLLO_RLE0371
	//Patch for testchip: egress port filter will reference ingress vlan id,
	//therefore all LAN PVID should add wan port to their member port and untag set
	//Get all Lan interface and add WAN port to their VLAN
	for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
	{
		tmpVid=rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->intf_vlan_id;
		mbpmsk.bits[0]=0;
		//errorno=RT_ERR_RG_VLAN_GET_FAIL;
		//ret = rtk_vlan_port_get(rg_db.systemGlobal.interfaceInfo[i].storedInfo.lan_intf.intf_vlan_id, &mbpmsk, &utpmsk);
		//if(ret!=RT_ERR_OK)goto RET_BRIDGE_ERR;
		memcpy(&mbpmsk, &rg_db.vlan[tmpVid].MemberPortmask,sizeof(rtk_portmask_t));
		memcpy(&utpmsk, &rg_db.vlan[tmpVid].UntagPortmask,sizeof(rtk_portmask_t));

		//Let WAN port become active in LAN's VLAN member port mask and untag set
		mbpmsk.bits[0] |= wanPmsk.bits[0];
		//utpmsk.bits[0] |= wanPmsk.bits[0];

		//Keep Lan's member port mask in all_lan_pmsk
		all_lan_pmsk.bits[0] |= mbpmsk.bits[0];

		errorno=RT_ERR_RG_VLAN_SET_FAIL;
		ret = RTK_VLAN_PORT_SET(tmpVid, &mbpmsk, &utpmsk);
		if(ret!=RT_ERR_OK)goto RET_BRIDGE_ERR;
	}
#endif

	//L3 routing interface (Static or DHCP or PPPOE) will reach here directly
	//Set up VLAN
	ori_pmsk.bits[0]=0;		//initial port mask
	ori_etpmsk.bits[0]=0;	//initial ext-port mask
	ori_utmsk.bits[0]=0;
	mbpmsk=wanPmsk;			//WAN port mask
	etpmsk.bits[0]=0;
	utpmsk.bits[0]=0;
	
	errorno=RT_ERR_RG_VLAN_SET_FAIL;
	ret = RTK_VLAN_CREATE(vlanID);
	if(ret == RT_ERR_VLAN_EXIST)
	{
		//errorno=RT_ERR_RG_VLAN_GET_FAIL;
		//ret = rtk_vlan_port_get(vlanID, &ori_pmsk, &ori_utmsk);
		//if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
		memcpy(&ori_pmsk, &rg_db.vlan[vlanID].MemberPortmask,sizeof(rtk_portmask_t));
		memcpy(&ori_utmsk, &rg_db.vlan[vlanID].UntagPortmask,sizeof(rtk_portmask_t));
		//ret = rtk_vlan_extPort_get(vlanID, &ori_etpmsk);
		//if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
		memcpy(&ori_etpmsk, &rg_db.vlan[vlanID].Ext_portmask,sizeof(rtk_portmask_t));
		//ret = rtk_vlan_fidMode_get(vlanID, &ori_mode);
		//if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
		ori_mode=rg_db.vlan[vlanID].fidMode;
		//ret = rtk_vlan_priorityEnable_get(vlanID, &ori_prien);
		//if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
		ori_prien=rg_db.vlan[vlanID].priorityEn;
		//ret = rtk_vlan_priority_get(vlanID, &ori_pri);
		//if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
		ori_pri=rg_db.vlan[vlanID].priority;

		//if((ori_pmsk.bits[0]&wanPmsk.bits[0]) > 0)	//overlap
			//return RT_ERR_RG_PORT_USED;
	
		vlan_exist=1;
	} else if(ret!=RT_ERR_OK)
		goto RET_VLAN_ERR;

	//Set up its member port, extension port set, and FID mode
	//decide to use IVL or SVL for VLAN tag decision, IVL by untag set; SVL by DMAC2CVID
	if(wanintf->isIVL)
		fidMode=VLAN_FID_IVL;		//vlan-based
	else
		fidMode=VLAN_FID_SVL;		//mac-based
	
	ret = RTK_VLAN_FIDMODE_SET(vlanID, fidMode);		//Patch 20121130
	if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
	ret = RTK_VLAN_FID_SET(vlanID, WAN_FID);		//Patch 20121130
	if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;

	mbpmsk.bits[0]|=ori_pmsk.bits[0];
	etpmsk.bits[0]|=ori_etpmsk.bits[0];
	utpmsk.bits[0]|=ori_utmsk.bits[0];
	
	if(wanintf->wan_type == RTK_RG_BRIDGE)
	{	
		//Patch for flooding 20121129, noneed, because we change to svl at all
		/*if(vlan_exist==0)
		{
			ret = RTK_VLAN_FIDMODE_SET(vlanID, VLAN_FID_SVL);
			if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
			ret = RTK_VLAN_FID_SET(vlanID, LAN_FID);
			if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
		}*/
		mbpmsk.bits[0]|=all_lan_pmsk.bits[0];		//add LAN port into WAN VLAN member port mask
		etpmsk.bits[0]|=all_lan_etpmsk.bits[0];		//add LAN ext-port into WAN VLAN ext-port mask
		utpmsk.bits[0]|=all_lan_utagpmsk.bits[0];	//add LAN untag-port into WAN VLAN untag-port mask
	}
	mbpmsk.bits[0]|=0x1<<RTK_RG_MAC_PORT_CPU;		//CPUport always on, or TRAP will failed
	etpmsk.bits[0]|=0x1;							//CPUport always on, or TRAP will failed
		
	if(wanintf->egress_vlan_tag_on)			//egress tagged packet
	{
		utpmsk.bits[0]&=(~(wanPmsk.bits[0]));	//set WAN port to 0 in untag set (tagging)
	}
	else
	{
		utpmsk.bits[0]|=wanPmsk.bits[0];		//set WAN port to 1 in untag set (untagging)
		//utpmsk.bits[0]|=0x1<<RTK_RG_MAC_PORT_CPU;	//set CPU port to 1 in untag set (untagging)
	}

#ifdef CONFIG_APOLLO_RLE0371
	//patch for WAN to LAN will cause L4 checksum error problem
	utpmsk.bits[0] |= all_lan_pmsk.bits[0];
#endif

	ret = RTK_VLAN_PORT_SET(vlanID, &mbpmsk, &utpmsk);
	if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
	ret = RTK_VLAN_EXTPORT_SET(vlanID, &etpmsk);
	if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
	if(wanintf->egress_vlan_pri != 0)
	{
		ret = RTK_VLAN_PRIORITY_SET(vlanID, wanintf->egress_vlan_pri);
		if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
		ret = RTK_VLAN_PRIORITYENABLE_SET(vlanID, ENABLED);
		if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
	}		

	//Set up port-based VLAN ID
	errorno=RT_ERR_RG_VLAN_SET_FAIL;
	memcpy(&ori_pmsk, &rg_db.vlan[DEFAULT_WAN_PVID].MemberPortmask,sizeof(rtk_portmask_t));
	memcpy(&ori_utmsk, &rg_db.vlan[DEFAULT_WAN_PVID].UntagPortmask,sizeof(rtk_portmask_t));
	memcpy(&ori_etpmsk, &rg_db.vlan[DEFAULT_WAN_PVID].Ext_portmask,sizeof(rtk_portmask_t));

	mbpmsk=wanPmsk;									//WAN port mask
	mbpmsk.bits[0]|=ori_pmsk.bits[0];				//add WAN port to the default PVID
	mbpmsk.bits[0]|=0x1<<RTK_RG_MAC_PORT_CPU;		//CPUport always on, or TRAP will failed

	etpmsk=ori_etpmsk;
	etpmsk.bits[0]|=0x1;							//EXT-CPUport always on, or TRAP will failed
	utpmsk.bits[0]=0x7f;							//all port untaged

	ret = RTK_VLAN_PORT_SET(DEFAULT_WAN_PVID, &mbpmsk, &utpmsk);
	if(ret!=RT_ERR_OK)goto RET_PVID_ERR;
	ret = RTK_VLAN_EXTPORT_SET(DEFAULT_WAN_PVID, &etpmsk);
	if(ret!=RT_ERR_OK)goto RET_PVID_ERR;

	errorno=RT_ERR_RG_VLAN_GET_FAIL;
	ret=rg_db.systemGlobal.untagBridgeWanSet[wanintf->wan_port_idx];
		
	/*for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
	{
		if((rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.wan_port_idx==wanintf->wan_port_idx) &&
			(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on==0) && 
			(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE))
			ret=1;	//if there is untag bridge WAN at the same port, do not change PVID!!
	}*/

	if(ret==0)
	{
		if(wanintf->egress_vlan_tag_on==0 && wanintf->wan_type==RTK_RG_BRIDGE)	//untag bridge WAN
		{
			rg_db.systemGlobal.untagBridgeWanSet[wanintf->wan_port_idx]=1;
			tmpPVid=vlanID;
		}
		else
		{
			tmpPVid=DEFAULT_WAN_PVID;
			for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
			{	
				if((rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->port_mask.portmask&(0x1<<wanintf->wan_port_idx)) > 0 &&
					rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->ip_version==IPVER_V4V6)
				{
					tmpPVid=0;
					break;
				}
			}
		}

		if(tmpPVid>0)	//either untag bridge WAN or non-LAN WAN port will assign PVID here
		{
			rgPVid=rg_db.systemGlobal.portBasedVID[wanintf->wan_port_idx];
			errorno=RT_ERR_RG_VLAN_SET_FAIL;
			ret = RTK_VLAN_PORTPVID_SET(wanintf->wan_port_idx, tmpPVid);
			//rg_db.systemGlobal.portBasedVID[wanintf->wan_port_idx]=tmpPVid;
			if(ret!=RT_ERR_OK)
			{
				//rg_db.systemGlobal.portBasedVID[wanintf->wan_port_idx]=rgPVid;	//recovery original pvid in rg_db
				RTK_VLAN_PORTPVID_SET(wanintf->wan_port_idx, rgPVid);		//recovery original pvid
				goto RET_PVID_ERR;
			}
		}
		/*}
		else
		{
			//FIXME: till now the WAN port can not be set in extension port
			ret = rtk_vlan_extPortPvid_get(wanintf->wan_port_idx-RTK_RG_EXT_PORT0, &tmpVid);
			if(ret!=RT_ERR_OK)goto RET_PVID_ERR;
			rgPVid=rg_db.systemGlobal.portBasedVID[wanintf->wan_port_idx];
			errorno=RT_ERR_RG_VLAN_SET_FAIL;
			ret = RTK_VLAN_EXTPORTPVID_SET(wanintf->wan_port_idx-RTK_RG_EXT_PORT0, tmpPVid);
			rg_db.systemGlobal.portBasedVID[wanintf->wan_port_idx]=tmpPVid;
			if(ret!=RT_ERR_OK)
			{
				rg_db.systemGlobal.portBasedVID[wanintf->wan_port_idx]=rgPVid;		//recovery old ext pvid in rg_db
				RTK_VLAN_EXTPORTPVID_SET(wanintf->wan_port_idx-RTK_RG_EXT_PORT0, tmpVid);		//recovery original ext pvid
				goto RET_PVID_ERR;
			}
		}*/
	}
	/*if(wanintf->wan_port_idx == RTK_RG_MAC_PORT_PON)		//PON
	{
		ret = rtk_vlan_portPvid_get(RTK_RG_MAC_PORT_PON, &tmpVid);
		if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
		ret = rtk_vlan_portPvid_set(RTK_RG_MAC_PORT_PON, vlanID);
		if(ret!=RT_ERR_OK)
		{
			rtk_vlan_portPvid_set(RTK_RG_MAC_PORT_PON, tmpVid);		//recovery original pvid
			goto RET_VLAN_ERR;
		}
	}
	else if(wanintf->wan_port_idx == RTK_RG_MAC_PORT_RGMII)		//RGMII
	{
		ret = rtk_vlan_portPvid_get(RTK_RG_MAC_PORT_RGMII, &tmpVid);
		if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
		ret = rtk_vlan_portPvid_set(RTK_RG_MAC_PORT_RGMII, vlanID);
		if(ret!=RT_ERR_OK)
		{
			rtk_vlan_portPvid_set(RTK_RG_MAC_PORT_RGMII, tmpVid);		//recovery original pvid
			goto RET_VLAN_ERR;
		}
	}*/

	//Set up WAN type and NXP hop table here, if wan is pppoe, pppoe table is also set up
	if(wanintf->wan_type == RTK_RG_PPPoE)
	{
		//Clear PPPoE table entry
		errorno=RT_ERR_RG_PPPOE_SET_FAIL;
		bzero(&pppoeEt, sizeof(rtk_l34_pppoe_entry_t));
		ret = RTK_L34_PPPOETABLE_SET(intfIdx, &pppoeEt);
		if(ret!=RT_ERR_OK)goto RET_PVID_ERR;
	}

	//Check for empty entry
	errorno=RT_ERR_RG_ENTRY_FULL;
	for(i=0;i<MAX_NEXTHOP_SW_TABLE_SIZE;i++)
	{
		if(rg_db.systemGlobal.nxpRefCount[i] == 0)
			break;
	}
	if(i==MAX_NEXTHOP_SW_TABLE_SIZE)goto RET_PVID_ERR;

	nxpIdx = i;		//Keep

	//Setup Nexthop table in nxtidx
	errorno=RT_ERR_RG_NXP_SET_FAIL;
	nxpEt.ifIdx=intfIdx;
	if(wanintf->wan_type == RTK_RG_PPPoE)
		nxpEt.type=L34_NH_PPPOE;
	else
		nxpEt.type=L34_NH_ETHER;
	
	// FIXME: here should to use binding remote host mac index, if port-binding is set
	nxpEt.nhIdx=rg_db.systemGlobal.defaultTrapLUTIdx;		//use this DUMMY index to force packet TRAP to CPU				
	nxpEt.pppoeIdx=intfIdx;			//using interface idx as PPPoE table idx
	nxpEt.keepPppoe=0;
	
	ret = RTK_L34_NEXTHOPTABLE_SET(nxpIdx, &nxpEt);
	if(ret!=RT_ERR_OK)goto RET_NEXTHOP_ERR;
	
//#ifndef CONFIG_APOLLO_RLE0371	
	//Add WAN type table, each interface has at most one entry
	errorno=RT_ERR_RG_WANTYPE_SET_FAIL;
	bzero(&wantEt, sizeof(rtk_wanType_entry_t));
	wantEt.nhIdx=nxpIdx;
	if(wanintf->wan_type==RTK_RG_BRIDGE)
		wantEt.wanType=L34_WAN_TYPE_L2_BRIDGE;
	else
		wantEt.wanType=L34_WAN_TYPE_L3_ROUTE;		//this value should be modified if NAPT
		
	//ret = dal_apollomp_l34_wanTypeTable_set(intfIdx, &wantEt);		//FIXME:no RTK APIs
	ret = RTK_L34_WANTYPETABLE_SET(intfIdx, &wantEt);
	if(ret==RT_ERR_CHIP_NOT_SUPPORTED)
	{
		errorno=RT_ERR_RG_CHIP_NOT_SUPPORT;
		goto RET_WANTYPE_ERR; 
	}
	if(ret!=RT_ERR_OK)goto RET_WANTYPE_ERR;
//#else
	//Add software WAN type table, each interface has at most one entry
	//RG_GLB_WAN_TYPE[intfIdx]=nxpIdx;
//#endif

#if 0
	//Check if default route was set
	if(wanintf->default_gateway_on == 1)
	{		
		bzero(&ori_rtEt, sizeof(rtk_l34_routing_entry_t));
		ret = rtk_l34_routingTable_get(7, &ori_rtEt);	//get default route setting
		if(ret!=RT_ERR_OK)return RT_ERR_RG_ROUTE_GET_FAIL;
		if(ori_rtEt.valid==1)return RT_ERR_RG_DEF_ROUTE_EXIST;

		//Setup Routing table for default route
		errorno=RT_ERR_RG_ROUTE_SET_FAIL;
		bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
	    rtEntry.valid=1;
	    rtEntry.process=L34_PROCESS_CPU;		//set all packets to CPU if WAN is default route
	    rtEntry.internal=0;		//external host from outside
	    rtEntry.ipAddr=0;
	    rtEntry.ipMask=0;
	    // TODO:if load-balance is needed, here should be changed
	    rtEntry.nhStart=nxtidx; /*exact index*/
		rtEntry.nhNxt=nxtidx;
	    rtEntry.nhNum=0;	//0 means 1 entry, 1 means 2 entries, 2 means 4 entries...
	    rtEntry.nhAlgo=0x2;		//PER-SIP
	    rtEntry.ipDomain=6;		//Entry 0~7
	    rtEntry.rt2waninf=1;

	    ret = RTK_L34_ROUTINGTABLE_SET(7, &rtEntry);		//set default route
	    if(ret!=RT_ERR_OK)goto RET_DEF_ROUTE_ERR;
	}
#endif

#ifdef CONFIG_APOLLO_RLE0371
#else
	//Set up port-binding for WAN interface
	//DEBUG("start");
	if(tmppmsk>0 || tmpexpmsk>0)
	{
		//MAC Port
		count=tmppmsk;
		RG_ONE_COUNT(count);
		//DEBUG("MAC port count is %d",count);
		for(j=0;j<count;j++)
		{
			errorno=RT_ERR_RG_ENTRY_FULL;
			for(i=MAX_BIND_SW_TABLE_SIZE - 1;i>=0;i--)		//Port-binding start from the bottom of Binding Table
			{
				/*memset(&pbindEt, 0, sizeof(pbindEt));
				ret = dal_apollomp_l34_bindingTable_get(i, &pbindEt);	//FIXME:no RTK APIs
				if(ret!=RT_ERR_OK)return RT_ERR_RG_PORT_BIND_FAIL;

				if(pbindEt.portMask.bits[0]==0 && pbindEt.extPortMask.bits[0]==0)
					break;*/
				//if(rg_db.systemGlobal.bindToIntf[i] == -1)
				if(rg_db.bind[i].valid == 0)
					break;
			}
			if(i==-1)goto RET_WANTYPE_ERR;

			bindIdx = i;		//Keep
			//DEBUG("%d is available",bindIdx);
			//rg_db.systemGlobal.bindToIntf[bindIdx]=intfIdx;

			//Add port binding entry once a time
			errorno=RT_ERR_RG_PORT_BIND_SET_FAIL;
			bzero(&pbindEt, sizeof(rtk_binding_entry_t));
			pbindEt.extPortMask.bits[0]=0;
			pbindEt.vidLan=0;
			pbindEt.wanTypeIdx=intfIdx;
			pbindEt.bindProto=L34_BIND_PROTO_ALL;		//ALL protocol in L3, and L2

			//DEBUG("before: tmppmsk is %x",tmppmsk);	
			if((tmppmsk&(0x1<<RTK_RG_PORT0)) >0)			//PORT0
			{
				tmppmsk&=~(0x1<<RTK_RG_PORT0);
				pbindEt.portMask.bits[0]=(0x1<<RTK_RG_PORT0);
			}
			else if((tmppmsk&(0x1<<RTK_RG_PORT1)) >0)		//PORT1
			{
				tmppmsk&=~(0x1<<RTK_RG_PORT1);
				pbindEt.portMask.bits[0]=(0x1<<RTK_RG_PORT1);
			}
			else if((tmppmsk&(0x1<<RTK_RG_PORT2)) >0)		//PORT2
			{
				tmppmsk&=~(0x1<<RTK_RG_PORT2);
				pbindEt.portMask.bits[0]=(0x1<<RTK_RG_PORT2);
			}
			else if((tmppmsk&(0x1<<RTK_RG_PORT3)) >0)		//PORT3
			{
				tmppmsk&=~(0x1<<RTK_RG_PORT3);
				pbindEt.portMask.bits[0]=(0x1<<RTK_RG_PORT3);
			}
			else if((tmppmsk&(0x1<<RTK_RG_PORT_PON)) >0)		//PORT4
			{
				tmppmsk&=~(0x1<<RTK_RG_PORT_PON);
				pbindEt.portMask.bits[0]=(0x1<<RTK_RG_PORT_PON);
			}
			else if((tmppmsk&(0x1<<RTK_RG_PORT_RGMII)) >0)		//PORT5
			{
				tmppmsk&=~(0x1<<RTK_RG_PORT_RGMII);
				pbindEt.portMask.bits[0]=(0x1<<RTK_RG_PORT_RGMII);
			}
			else if((tmppmsk&(0x1<<RTK_RG_PORT_CPU)) >0)		//PORT CPU
			{
				tmppmsk&=~(0x1<<RTK_RG_PORT_CPU);
				pbindEt.portMask.bits[0]=(0x1<<RTK_RG_PORT_CPU);
			}
			//ret = dal_apollomp_l34_bindingTable_set(bindIdx, &pbindEt);		//FIXME:no RTK APIs
			ret = RTK_L34_BINDINGTABLE_SET(bindIdx, &pbindEt);
			if(ret==RT_ERR_CHIP_NOT_SUPPORTED)
			{
				errorno=RT_ERR_RG_CHIP_NOT_SUPPORT;
				goto RET_BINDING_ERR; 
			}
			if(ret!=RT_ERR_OK)goto RET_BINDING_ERR;
			
			rg_db.bind[bindIdx].valid = 1;

			// TODO:Call the initParam's bindingAddByHwCallBack
			if(rg_db.systemGlobal.initParam.bindingAddByHwCallBack != NULL)
			{
				cb_bindEt.type=BIND_TYPE_PORT;
				cb_bindEt.port_bind_pmask.portmask=pbindEt.portMask.bits[0];
				cb_bindEt.wan_intf_idx=intfIdx;
				rg_db.systemGlobal.initParam.bindingAddByHwCallBack(&cb_bindEt);
			}
		}	

		//Extension port
		count=tmpexpmsk;
		RG_ONE_COUNT(count);
		for(j=0;j<count;j++)
		{
			errorno=RT_ERR_RG_ENTRY_FULL;
			for(i=MAX_BIND_SW_TABLE_SIZE - 1;i>=0;i--)		//Port-binding start from the bottom of Binding Table
			{
				/*memset(&pbindEt, 0, sizeof(pbindEt));
				ret = dal_apollomp_l34_bindingTable_get(i, &pbindEt);	//FIXME:no RTK APIs
				if(ret!=RT_ERR_OK)return RT_ERR_RG_PORT_BIND_FAIL;

				if(pbindEt.portMask.bits[0]==0 && pbindEt.extPortMask.bits[0]==0)
					break;*/
				//if(rg_db.systemGlobal.bindToIntf[i] == -1)
				if(rg_db.bind[i].valid == 0)
					break;
			}
			if(i==-1)goto RET_BINDING_ERR;
			
			bindIdx = i;		//Keep
			//rg_db.systemGlobal.bindToIntf[bindIdx]=intfIdx;

			//Add port binding entry once a time
			errorno=RT_ERR_RG_EXTPORT_BIND_SET_FAIL;
			bzero(&pbindEt, sizeof(rtk_binding_entry_t));
			pbindEt.portMask.bits[0]=0;
			pbindEt.vidLan=0;
			pbindEt.wanTypeIdx=intfIdx;
			pbindEt.bindProto=L34_BIND_PROTO_ALL;		//ALL protocol in L3, and L2

			if((tmpexpmsk&(0x1<<RTK_RG_BD_EXT_PORT0)) >0)			//EXTPORT0
			{
				tmpexpmsk&=~(0x1<<RTK_RG_BD_EXT_PORT0);
				pbindEt.extPortMask.bits[0]=(0x1<<RTK_RG_BD_EXT_PORT0);				
			}
			else if((tmpexpmsk&(0x1<<RTK_RG_BD_EXT_PORT1)) >0)		//EXTPORT1
			{
				tmpexpmsk&=~(0x1<<RTK_RG_BD_EXT_PORT1);
				pbindEt.extPortMask.bits[0]=(0x1<<RTK_RG_BD_EXT_PORT1);
			}
			else if((tmpexpmsk&(0x1<<RTK_RG_BD_EXT_PORT2)) >0)		//EXTPORT2
			{
				tmpexpmsk&=~(0x1<<RTK_RG_BD_EXT_PORT2);
				pbindEt.extPortMask.bits[0]=(0x1<<RTK_RG_BD_EXT_PORT2);
			}
			else if((tmpexpmsk&(0x1<<RTK_RG_BD_EXT_PORT3)) >0)		//EXTPORT3
			{
				tmpexpmsk&=~(0x1<<RTK_RG_BD_EXT_PORT3);
				pbindEt.extPortMask.bits[0]=(0x1<<RTK_RG_BD_EXT_PORT3);
			}
			else if((tmpexpmsk&(0x1<<RTK_RG_BD_EXT_PORT4)) >0)		//EXTPORT4
			{
				tmpexpmsk&=~(0x1<<RTK_RG_BD_EXT_PORT4);
				pbindEt.extPortMask.bits[0]=(0x1<<RTK_RG_BD_EXT_PORT4);
			}
			//ret = dal_apollomp_l34_bindingTable_set(bindIdx, &pbindEt);		//FIXME:no RTK APIs
			ret = RTK_L34_BINDINGTABLE_SET(bindIdx, &pbindEt);
			if(ret==RT_ERR_CHIP_NOT_SUPPORTED)
			{
				errorno=RT_ERR_RG_CHIP_NOT_SUPPORT;
				goto RET_BINDING_ERR; 
			}
			if(ret!=RT_ERR_OK)goto RET_BINDING_ERR;

			rg_db.bind[bindIdx].valid = 1;

			// TODO:Call the initParam's bindingAddByHwCallBack
			if(rg_db.systemGlobal.initParam.bindingAddByHwCallBack != NULL)
			{
				cb_bindEt.type=BIND_TYPE_PORT;
				cb_bindEt.port_bind_pmask.portmask=pbindEt.portMask.bits[0];
				cb_bindEt.wan_intf_idx=intfIdx;
				rg_db.systemGlobal.initParam.bindingAddByHwCallBack(&cb_bindEt);
			}
		}	
	}
#endif

	//1 FIXME: patch for DA==GatewayMac will hit layer2 unknown DA, if action is trap
	//Create Wan gateway STATIC MAC
	errorno=RT_ERR_RG_CREATE_GATEWAY_LUT_FAIL;
	if(wanintf->egress_vlan_tag_on)	//cpu tagged
		utpmsk.bits[0]=0;
	else
		utpmsk.bits[0]=0x1<<RTK_RG_MAC_PORT_CPU;
	ret = _rtk_rg_createGatewayMacEntry(wanintf->gmac.octet,wanintf->egress_vlan_id,utpmsk.bits[0]);
	if(ret==FAIL)goto RET_BINDING_ERR;

	//Set Global variables
	//if(wanintf->default_gateway_on==1)
		//rg_db.systemGlobal.defaultRouteSet = intfIdx;
	rg_db.systemGlobal.nxpRefCount[nxpIdx]++;		//nexthop reference by WAN type table
	rg_db.systemGlobal.wanIntfGroup[rg_db.systemGlobal.wanIntfTotalNum].index=intfIdx;
	rg_db.systemGlobal.wanIntfGroup[rg_db.systemGlobal.wanIntfTotalNum].p_intfInfo=&rg_db.systemGlobal.interfaceInfo[intfIdx];
	rg_db.systemGlobal.wanIntfGroup[rg_db.systemGlobal.wanIntfTotalNum].p_wanIntfConf=&rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.wan_intf_conf;
	rg_db.systemGlobal.interfaceInfo[intfIdx].lan_or_wan_index=rg_db.systemGlobal.wanIntfTotalNum;
	rg_db.systemGlobal.wanIntfTotalNum++;		//add WAN interface number	

	*wan_intf_idx = intfIdx;
	//rtlglue_printf("wan_intf_idx get = %d",*wan_intf_idx);

	//store information in Global variable
	rg_db.systemGlobal.interfaceInfo[intfIdx].valid=1;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.is_wan=1;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.wan_intf_conf.wan_type=wanintf->wan_type;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.wan_intf_conf.isIVL=wanintf->isIVL;
	if(wanintf->wan_type == RTK_RG_STATIC)
		rg_db.systemGlobal.interfaceInfo[intfIdx].p_wanStaticInfo=&rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.static_info;		//short-cut of wan static info structure
	else if(wanintf->wan_type == RTK_RG_DHCP)
		rg_db.systemGlobal.interfaceInfo[intfIdx].p_wanStaticInfo=&rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.dhcp_client_info.hw_info;		//short-cut of wan static info structure
	else if(wanintf->wan_type == RTK_RG_PPPoE)
		rg_db.systemGlobal.interfaceInfo[intfIdx].p_wanStaticInfo=&rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.pppoe_info.after_dial.hw_info;		//short-cut of wan static info structure
	else
	{	
		//bridge WAN
		//rg_db.systemGlobal.bridgeWanNum++;
		rg_db.systemGlobal.interfaceInfo[intfIdx].p_wanStaticInfo=NULL;
	}
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx=wanintf->wan_port_idx;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.wan_intf_conf.port_binding_mask=wanintf->port_binding_mask;
	//rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.wan_intf_conf.extport_binding_mask=wanintf->extport_binding_mask;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on=wanintf->egress_vlan_tag_on;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id=wanintf->egress_vlan_id;
	rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_pri=wanintf->egress_vlan_pri;
	//_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.wan_intf_conf.default_gateway_on=wanintf->default_gateway_on;
	memcpy(rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.wan_intf_conf.gmac.octet, wanintf->gmac.octet,ETHER_ADDR_LEN);

	//Check PPPoE Pass through
	_rtk_rg_refreshPPPoEPassThroughLanOrWanPortMask();

	if(rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE){
		//add wan-interfcae callback to sync protocal-stack
		if(rg_db.systemGlobal.initParam.interfaceAddByHwCallBack != NULL)
		{
			//rtk_rg_intfInfo_t intfInfo;
			//bzero(&intfInfo,sizeof(intfInfo));
			//memcpy(&intfInfo, &rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo, sizeof(intfInfo));
			rg_db.systemGlobal.initParam.interfaceAddByHwCallBack(&rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo,&intfIdx);
		}
	}

	errorno=RT_ERR_RG_OK;
	goto RET_SUCCESS;
	
#ifdef CONFIG_APOLLO_RLE0371
#else
RET_BINDING_ERR:
	//Delete each port-binding entry in binding table (vlan=0)
	for(i=MAX_BIND_SW_TABLE_SIZE - 1;i>=0;i--)
	{
		//if(rg_db.systemGlobal.bindToIntf[i] == intfIdx)
		if(rg_db.bind[i].valid == 1 && rg_db.bind[i].rtk_bind.wanTypeIdx == intfIdx)
		{
			bzero(&pbindEt, sizeof(rtk_binding_entry_t));
			bzero(&cb_bindEt,sizeof(rtk_rg_bindingEntry_t));
			//dal_apollomp_l34_bindingTable_get(i, &pbindEt);		//FIXME:no RTK APIs
			//rtk_l34_bindingTable_get(i, &pbindEt);
			//if(pbindEt.vidLan==0)		//vlan != 0 means port-vlan binding, which we don't care here
			if(rg_db.bind[i].rtk_bind.vidLan == 0)	//vlan != 0 means port-vlan binding, which we don't care here
			{
				//dal_apollomp_l34_bindingTable_set(bindIdx, &pbindEt);		//FIXME:no RTK APIs
				cb_bindEt.port_bind_pmask.portmask=rg_db.bind[i].rtk_bind.portMask.bits[0];
				
				RTK_L34_BINDINGTABLE_SET(i, &pbindEt);
				//rg_db.systemGlobal.bindToIntf[i]=-1;
				rg_db.bind[i].valid = 0;

				// TODO:Call the initParam's bindingDelByHwCallBack
				if(rg_db.systemGlobal.initParam.bindingDelByHwCallBack != NULL)
				{
					cb_bindEt.type=BIND_TYPE_PORT;
					cb_bindEt.wan_intf_idx=intfIdx;
					rg_db.systemGlobal.initParam.bindingDelByHwCallBack(&cb_bindEt);
				}
			}
		}
	}
#endif
#if 0
RET_DEF_ROUTE_ERR:
	//Recovery default setting
	RTK_L34_ROUTINGTABLE_SET(7, &ori_rtEt);
#endif
//#ifndef CONFIG_APOLLO_RLE0371
RET_WANTYPE_ERR:
	//Delete WAN type entry
	bzero(&wantEt, sizeof(rtk_wanType_entry_t));
	//dal_apollomp_l34_wanTypeTable_set(intfIdx, &wantEt);		//FIXME:no RTK APIs
	RTK_L34_WANTYPETABLE_SET(intfIdx, &wantEt);
//#endif
RET_NEXTHOP_ERR:
	//Delete nexthop entry
	bzero(&nxpEt, sizeof(rtk_l34_nexthop_entry_t));
	RTK_L34_NEXTHOPTABLE_SET(nxpIdx, &nxpEt);
RET_PVID_ERR:
	RTK_VLAN_PORT_SET(DEFAULT_WAN_PVID, &ori_pmsk, &ori_utmsk);
	RTK_VLAN_EXTPORT_SET(DEFAULT_WAN_PVID, &ori_etpmsk);
RET_VLAN_ERR:
	//Recovery VLAN setting
	if(vlan_exist)
	{
		RTK_VLAN_PORT_SET(vlanID, &ori_pmsk, &ori_utmsk);
		RTK_VLAN_EXTPORT_SET(vlanID, &ori_etpmsk);
		RTK_VLAN_FIDMODE_SET(vlanID, ori_mode);
		RTK_VLAN_PRIORITYENABLE_SET(vlanID, ori_prien);
		RTK_VLAN_PRIORITY_SET(vlanID, ori_pri);
	}
	else
	{
		RTK_VLAN_DESTROY(vlanID);
	}
RET_BRIDGE_ERR:
	//Recovery all Lan interface's VLAN member port mask
	if(wanintf->wan_type == RTK_RG_BRIDGE)
	{
		for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
		{
			tmpVid=rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->intf_vlan_id;
			//rtk_vlan_port_get(rg_db.systemGlobal.interfaceInfo[i].storedInfo.lan_intf.intf_vlan_id, &mbpmsk, &utpmsk);
			memcpy(&mbpmsk, &rg_db.vlan[tmpVid].MemberPortmask,sizeof(rtk_portmask_t));
			memcpy(&utpmsk, &rg_db.vlan[tmpVid].UntagPortmask,sizeof(rtk_portmask_t));
			mbpmsk.bits[0] &= (~(wanPmsk.bits[0]));		//negative the WAN PORT in LAN's VLAN member port mask
			RTK_VLAN_PORT_SET(tmpVid, &mbpmsk, &utpmsk);
		}
	}
#if 0	
RET_DEFALT_VLAN_ERR:
	//Recovery DEFAULT LAN VLAN
	RTK_VLAN_PORT_SET(DEFAULT_LAN_VLAN, &ori_CPU_member_mask, &ori_CPU_untag_mask);
#endif

/*RET_PPPOE_PASS_ERR:
	ret=0;
	for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
	{
		if(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.wan_port_idx == wanintf->wan_port_idx)
			ret++;
	}
	if(ret==0)		//only this WAN used the wan port
	{
		for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
		{
			if(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->pppoe_passThrough == 1)
			{
				//remove WAN port to their PPB VLAN
				tmpVid=DEFAULT_PPB_VLAN_START+rg_db.systemGlobal.lanIntfGroup[i].index;
				memcpy(&ori_pmsk, &rg_db.vlan[tmpVid].MemberPortmask,sizeof(rtk_portmask_t));
				memcpy(&ori_utmsk, &rg_db.vlan[tmpVid].UntagPortmask,sizeof(rtk_portmask_t));
				memcpy(&ori_etpmsk, &rg_db.vlan[tmpVid].Ext_portmask,sizeof(rtk_portmask_t));

				if(wanintf->wan_port_idx <= RTK_RG_PORT_CPU)
				{
					ori_pmsk.bits[0]&=~(0x1<<wanintf->wan_port_idx);
					ori_utmsk.bits[0]&=~(0x1<<wanintf->wan_port_idx);
				}
				else
				{
					ori_etpmsk.bits[0]&=~(0x1<<(wanintf->wan_port_idx-RTK_RG_EXT_PORT0));
					ori_utmsk.bits[0]&=~(0x1<<RTK_RG_PORT_CPU);
				}

				RTK_VLAN_PORT_SET(tmpVid, &ori_pmsk, &ori_utmsk);
				RTK_VLAN_EXTPORT_SET(tmpVid, &ori_etpmsk);			
			}
		}
	}
*/
RET_INTF_ERR:
	//Delete interface entry
	bzero(&intfEntry, sizeof(rtk_l34_netif_entry_t));
	RTK_L34_NETIFTABLE_SET(intfIdx, &intfEntry);

RET_SUCCESS:
	
	//------------------ Critical Section End -----------------------//
	rg_unlock(&rg_kernel.interfaceLock);
	
    return errorno;
}

int _rtk_rg_decreaseNexthopReference(int nexthopIdx)
{
	int ret;
	rtk_l34_pppoe_entry_t pppEt;
	rtk_l34_nexthop_entry_t nxpEt;
		
	if(rg_db.systemGlobal.nxpRefCount[nexthopIdx] > 0)
        rg_db.systemGlobal.nxpRefCount[nexthopIdx]--;

    if(rg_db.systemGlobal.nxpRefCount[nexthopIdx] == 0)
    {
        //Delete Nexthop entry, since nobody use it
        if(rg_db.nexthop[nexthopIdx].rtk_nexthop.type==L34_NH_PPPOE)
        {
            //Delete PPPoE table
            bzero(&pppEt, sizeof(rtk_l34_pppoe_entry_t));
            ret = RTK_L34_PPPOETABLE_SET(rg_db.nexthop[nexthopIdx].rtk_nexthop.pppoeIdx, &pppEt);
            if(ret!=RT_ERR_OK)
				return RT_ERR_RG_PPPOE_SET_FAIL;
#ifdef CONFIG_APOLLO_RLE0371
			ret = RTK_L34_PPPOETABLE_SET(0, &pppEt);
			if(ret!=RT_ERR_OK)
				return RT_ERR_RG_PPPOE_SET_FAIL;
#endif
        }
        bzero(&nxpEt, sizeof(rtk_l34_nexthop_entry_t));
        ret = RTK_L34_NEXTHOPTABLE_SET(nexthopIdx, &nxpEt);

        if(ret!=RT_ERR_OK)
			return RT_ERR_RG_NXP_SET_FAIL;
    }

	return RT_ERR_RG_OK;
}

int32 rtk_rg_interface_del(int lan_or_wan_intf_idx)
{
    int i,j,ret,nh_num,match_intf=0,napt_en=0,tmpVid,tmpPvid,errorno,l2Idx,count=0,search_index;
	unsigned int wan_set_mask;
    rtk_l34_netif_entry_t intfEt;
    rtk_l34_routing_entry_t rtEntry;
	rtk_ipv6Routing_entry_t rtv6Entry;
    rtk_l34_nexthop_entry_t nxpEt;
    rtk_l34_ext_intip_entry_t extipEt;
	rtk_portmask_t mbpmsk,utpmsk,etpmsk;
	rtk_portmask_t pvid_mac_pmask,pvid_ext_pmask;
	rtk_portmask_t all_lan_pmsk,all_lan_etpmsk,all_lan_utagpmsk;
	rtk_rg_ipv4RoutingEntry_t cb_routEt;
	rtk_rg_ipv6RoutingEntry_t cb_routv6Et;
	rtk_wanType_entry_t wantEt;	
	rtk_rg_macEntry_t macEt,secondMacEt;
	rtk_rg_arpInfo_t arpInfo;
	rtk_rg_arp_linkList_t *pSwArpList,*pNextSwArpList;
	rtk_l34_pppoe_entry_t pppoeEt;
	rtk_rg_neighborInfo_t neighborInfo;
	//rtk_vlan_protoVlanCfg_t protoVlanCfg;
	rtk_rg_routing_arpInfo_t deletingEntry;
#ifdef CONFIG_APOLLO_RLE0371
#else
    rtk_binding_entry_t pbindEt;
	rtk_rg_bindingEntry_t cb_bindEt;
#endif
	rtk_rg_intfInfo_t keep_store_info;		//for callback

    //Check parameter
    if(lan_or_wan_intf_idx<0 || lan_or_wan_intf_idx>=MAX_NETIF_SW_TABLE_SIZE){
		rtlglue_printf("intf lan_or_wan_intf_idx=%d\n",lan_or_wan_intf_idx);
		return RT_ERR_RG_INVALID_PARAM;
    }

	//Check the interface had created or not
	if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].valid == 0){
		rtlglue_printf("intf valid=%d\n",rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].valid);
		return RT_ERR_RG_INVALID_PARAM;
	}
    //bzero(&intfEt, sizeof(rtk_l34_netif_entry_t));
    //ret = rtk_l34_netifTable_get(lan_or_wan_intf_idx, &intfEt);
    //if(ret!=RT_ERR_OK)return RT_ERR_RG_INTF_GET_FAIL;
    //if(intfEt.valid != 1)return RT_ERR_RG_INVALID_PARAM;

	//------------------ Critical Section start -----------------------//
	rg_lock(&rg_kernel.interfaceLock);

	//Reset all Lan interface to delete WAN port to their VLAN (only for Bridge mode WAN)
	if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.is_wan == 1)
	{
		if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo!=NULL)	//not bridge WAN
		{
			//Stop ARP request timer if this WAN interface is ipv4 default route
			if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->ipv4_default_gateway_on == 1)
				rg_db.systemGlobal.intfArpRequest[lan_or_wan_intf_idx].finished = 1;

			//Stop Neighbor Discovery request timer if this WAN interface is ipv6 default route
			if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->ipv6_default_gateway_on == 1)
				rg_db.systemGlobal.intfNeighborDiscovery[lan_or_wan_intf_idx].finished = 1;
		}
#ifdef CONFIG_RG_PPPOE_PASSTHROUGHT		
		//Delete ACL setting for PPPoE Pass through
		if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type == RTK_RG_PPPoE)
			_rtk_rg_acl_pppoe_passthrough_for_wanIntf_del(lan_or_wan_intf_idx);
#endif
		
#ifdef CONFIG_APOLLO_RLE0371
#else
		//Delete the VLAN-Binding entries which binding to the deleting interface first
		for(i=0;i<MAX_BIND_SW_TABLE_SIZE;i++)
		{
			//if(rg_db.systemGlobal.bindToIntf[i]==lan_or_wan_intf_idx && rg_db.systemGlobal.bindWithVLAN[i]!=-1)		//the binding rule points to the deleting interface
			if(rg_db.bind[i].valid==1 && rg_db.bind[i].rtk_bind.wanTypeIdx==lan_or_wan_intf_idx)
			{
				if(rg_db.bind[i].rtk_bind.vidLan!=0)		//the binding rule points to the deleting interface
				{
					errorno = rtk_rg_vlanBinding_del(i);
					if(errorno!=RT_ERR_RG_OK)goto RET_ERR;
				}
				else
				{
					//Delete Binding table for port-binding entries
					cb_bindEt.type=BIND_TYPE_PORT;
					cb_bindEt.port_bind_pmask.portmask=rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.port_binding_mask.portmask;
					cb_bindEt.wan_intf_idx=lan_or_wan_intf_idx;			
					//the binding entries with deleting WAN interface will be deleted, and its portmask will always 
					//match the binding mask of interface.
					//if(((rg_db.bind[i].rtk_bind.portMask.bits[0]&rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.port_binding_mask.portmask)>0)||
					//((rg_db.bind[i].rtk_bind.extPortMask.bits[0]&(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.port_binding_mask.portmask>>RTK_RG_EXT_PORT0))>0))
					//{
						bzero(&pbindEt, sizeof(rtk_binding_entry_t));
						//ret = dal_apollomp_l34_bindingTable_set(i, &pbindEt); //FIXME:no RTK APIs
						ret = RTK_L34_BINDINGTABLE_SET(i, &pbindEt);
						errorno=RT_ERR_RG_CHIP_NOT_SUPPORT;
						if(ret==RT_ERR_CHIP_NOT_SUPPORTED)goto RET_ERR;
						errorno=RT_ERR_RG_PORT_BIND_SET_FAIL;
						if(ret!=RT_ERR_OK)goto RET_ERR;

						//rg_db.systemGlobal.bindToIntf[i]=-1;		//reset to unused
						rg_db.bind[i].valid = 0;
					//}
					// TODO:Call the initParam's bindingDelByHwCallBack
					if(rg_db.systemGlobal.initParam.bindingDelByHwCallBack != NULL)	
						rg_db.systemGlobal.initParam.bindingDelByHwCallBack(&cb_bindEt);
				}
			}
		}
	
#endif

		//Check if there is any other WAN used the same WAN port		
		ret = 0;
		tmpVid = 0;
		for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
		{
			if(rg_db.systemGlobal.wanIntfGroup[i].index != lan_or_wan_intf_idx && 
				rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx==rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx)
				ret++;	//there are other WAN using the same WAN port	
		}
		if(ret==0)	//means there is only the deleting interface using this WAN port
		{		
			errorno=RT_ERR_RG_VLAN_SET_FAIL;
			//remove WAN port from DEFAULT_WAN_PVID
			memcpy(&mbpmsk, &rg_db.vlan[DEFAULT_WAN_PVID].MemberPortmask,sizeof(rtk_portmask_t));
			memcpy(&utpmsk, &rg_db.vlan[DEFAULT_WAN_PVID].UntagPortmask,sizeof(rtk_portmask_t));
			memcpy(&etpmsk, &rg_db.vlan[DEFAULT_WAN_PVID].Ext_portmask,sizeof(rtk_portmask_t));
			if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx <= RTK_RG_PORT_CPU)
				mbpmsk.bits[0]&=~(0x1<<rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx);
			else
				etpmsk.bits[0]&=~(0x1<<(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx-RTK_RG_EXT_PORT0));

			ret = RTK_VLAN_PORT_SET(DEFAULT_WAN_PVID, &mbpmsk, &utpmsk);
			if(ret!=RT_ERR_OK)goto RET_ERR;
			ret = RTK_VLAN_EXTPORT_SET(DEFAULT_WAN_PVID, &etpmsk);
			if(ret!=RT_ERR_OK)goto RET_ERR;			

			//remove WAN port from all PPPoE pass through enable LAN interface
			/*for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
			{
				if(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->pppoe_passThrough == 1)
				{
					//remove WAN port to their PPB VLAN
					tmpVid=DEFAULT_PPB_VLAN_START+rg_db.systemGlobal.lanIntfGroup[i].index;
					memcpy(&mbpmsk, &rg_db.vlan[tmpVid].MemberPortmask,sizeof(rtk_portmask_t));
					memcpy(&utpmsk, &rg_db.vlan[tmpVid].UntagPortmask,sizeof(rtk_portmask_t));
					memcpy(&etpmsk, &rg_db.vlan[tmpVid].Ext_portmask,sizeof(rtk_portmask_t));

					if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx <= RTK_RG_PORT_CPU)
						mbpmsk.bits[0]&=~(0x1<<rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx);
					else
						etpmsk.bits[0]&=~(0x1<<(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx-RTK_RG_EXT_PORT0));

					ret = RTK_VLAN_PORT_SET(tmpVid, &mbpmsk, &utpmsk);
					if(ret!=RT_ERR_OK)goto RET_ERR;
					ret = RTK_VLAN_EXTPORT_SET(tmpVid, &etpmsk);
					if(ret!=RT_ERR_OK)goto RET_ERR;
				}
			}

			//reset PPB if needed
			if(tmpVid!=0)
			{
				errorno=RT_ERR_RG_PPB_SET_FAILED;
				bzero(&protoVlanCfg,sizeof(rtk_vlan_protoVlanCfg_t));
				protoVlanCfg.vid=DEFAULT_CPU_VLAN;		//fixme: rtk api won't accept vid=0, so we assign a dummy one
				ret = rtk_vlan_portProtoVlan_set(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx,PPPOE_DISCOVERY_GROUPID,&protoVlanCfg);
				if(ret!=RT_ERR_OK)goto RET_ERR;
				ret = rtk_vlan_portProtoVlan_set(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx,PPPOE_SESSION_GROUPID,&protoVlanCfg);
				if(ret!=RT_ERR_OK)goto RET_ERR;

				//destory VLAN if last one
				tmpVid=DEFAULT_PPB_VLAN_WAN;
				if(rg_db.systemGlobal.wanIntfTotalNum==1)
				{
					errorno=RT_ERR_RG_VLAN_SET_FAIL;
					ret = RTK_VLAN_DESTROY(tmpVid);
					if(ret!=RT_ERR_OK)goto RET_ERR;
				}
				else
				{
					//remove from WLAN
					memcpy(&mbpmsk, &rg_db.vlan[tmpVid].MemberPortmask,sizeof(rtk_portmask_t));
					memcpy(&utpmsk, &rg_db.vlan[tmpVid].UntagPortmask,sizeof(rtk_portmask_t));
					mbpmsk.bits[0]&=~(0x1<<rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx);
					utpmsk.bits[0]&=~(0x1<<rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx);
					ret = RTK_VLAN_PORT_SET(tmpVid, &mbpmsk, &utpmsk);
					if(ret!=RT_ERR_OK)goto RET_ERR;
				}
			}*/
		}

		//Reset wan port-based VLAN if we are the untag bridge WAN, or there is no untag bridge WAN at same port,
		//if no other WAN at same port, set PVID to DEFAULT_CPU_VLAN, otherwise set to DEFAULT_WAN_PVID.
		ret=0;
		if((rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on==0 && 
			rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE) ||
			(rg_db.systemGlobal.untagBridgeWanSet[rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx]==0))
		{
			tmpPvid=DEFAULT_CPU_VLAN;
			for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
			{	
				if(rg_db.systemGlobal.wanIntfGroup[i].index!=lan_or_wan_intf_idx &&
					rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx==rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx)
					tmpPvid=DEFAULT_WAN_PVID;	//if there is any other WAN at the same port, use DEFAULT_WAN_PVID
			}

			errorno=RT_ERR_RG_VLAN_SET_FAIL;
			i=rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx;
			//if(i <= RTK_RG_PORT_CPU)
			//{
				ret = RTK_VLAN_PORTPVID_SET(i, tmpPvid);
				if(ret!=RT_ERR_OK)
					goto RET_ERR;
			/*}
			else
			{
				//FIXME: till now the WAN port can not be set in extension port
				ret = RTK_VLAN_EXTPORTPVID_SET(i-RTK_RG_EXT_PORT0, tmpPvid);
				if(ret!=RT_ERR_OK)
					goto RET_ERR;
			}*/
			
			//rg_db.systemGlobal.portBasedVID[i]=tmpPvid;		//reset port-based VLAN in rg_db
		}

		if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE)
		{
			//Check if there is any other bridge WAN used the same WAN port
			all_lan_pmsk.bits[0]=0;
			all_lan_etpmsk.bits[0]=0;
			/*ret = 0;
			for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
			{
				if(rg_db.systemGlobal.wanIntfGroup[i].index != lan_or_wan_intf_idx && 
					rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_type==RTK_RG_BRIDGE &&
					rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx==rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx)
					ret++;	//there are other Bridge mode WAN using the same WAN port	
			}*/
			//if(ret==0)	//means there is only the deleting interface using this WAN port
			//{				
#if 0
			//Take off WAN port from DEFAULT LAN VLAN member port
			//mbpmsk.bits[0]=0;
			//utpmsk.bits[0]=0;
			//etpmsk.bits[0]=0;
			//ret = rtk_vlan_port_get(DEFAULT_LAN_VLAN, &mbpmsk, &utpmsk);
			//errorno=RT_ERR_RG_VLAN_GET_FAIL;
			//if(ret!=RT_ERR_OK)goto RET_ERR;
			memcpy(&mbpmsk, &rg_db.vlan[DEFAULT_LAN_VLAN].MemberPortmask,sizeof(rtk_portmask_t));
			memcpy(&utpmsk, &rg_db.vlan[DEFAULT_LAN_VLAN].UntagPortmask,sizeof(rtk_portmask_t));

			mbpmsk.bits[0]&=(~(0x1<<rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx));

			ret = RTK_VLAN_PORT_SET(DEFAULT_LAN_VLAN, &mbpmsk, &utpmsk);
			errorno=RT_ERR_RG_VLAN_SET_FAIL;
			if(ret!=RT_ERR_OK)goto RET_ERR;
#endif								
			//mbpmsk.bits[0]=0;
			//utpmsk.bits[0]=0;
			//etpmsk.bits[0]=0;

			//reset all LAN interface's VLAN settings
			for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
			{
				tmpVid=rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->intf_vlan_id;
				//Take off WAN port from LAN VLAN member port
				memcpy(&mbpmsk, &rg_db.vlan[tmpVid].MemberPortmask,sizeof(rtk_portmask_t));
				memcpy(&utpmsk, &rg_db.vlan[tmpVid].UntagPortmask,sizeof(rtk_portmask_t));
				//ret = rtk_vlan_extPort_get(rg_db.systemGlobal.interfaceInfo[i].storedInfo.lan_intf.intf_vlan_id, &etpmsk);
				//if(ret!=RT_ERR_OK)goto RET_ERR;
				memcpy(&etpmsk, &rg_db.vlan[tmpVid].Ext_portmask,sizeof(rtk_portmask_t));

				//reset wan port untag set in LAN's vlan by its original setting
				if(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->untag_mask.portmask&(0x1<<rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx))		//original set wan port
					utpmsk.bits[0]|=0x1<<rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx;		//set WAN port to 1 in untag set (untagging)
				else
					utpmsk.bits[0]&=(~(0x1<<rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx));	//set WAN port to 0 in untag set (tagging)
					
				//if there is other bridge WAN at same port, reset untag set by oldest interface
				ret=0;
				for(j=0;j<rg_db.systemGlobal.wanIntfTotalNum;j++)
				{
					//rtlglue_printf("$$$$$$$$$$$$ j is %d, index is %d\n",j,rg_db.systemGlobal.wanIntfGroup[j].index);
					if(rg_db.systemGlobal.wanIntfGroup[j].index != lan_or_wan_intf_idx && 
						rg_db.systemGlobal.wanIntfGroup[j].p_wanIntfConf->wan_type==RTK_RG_BRIDGE &&
						rg_db.systemGlobal.wanIntfGroup[j].p_wanIntfConf->wan_port_idx==rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx)
					{
						//rtlglue_printf("hit!!\n");
						ret++;
						if(rg_db.systemGlobal.wanIntfGroup[j].p_wanIntfConf->egress_vlan_tag_on)			//egress tagged packet
							utpmsk.bits[0]&=(~(0x1<<rg_db.systemGlobal.wanIntfGroup[j].p_wanIntfConf->wan_port_idx));	//set WAN port to 0 in untag set (tagging)
						else
							utpmsk.bits[0]|=0x1<<rg_db.systemGlobal.wanIntfGroup[j].p_wanIntfConf->wan_port_idx;		//set WAN port to 1 in untag set (untagging)
						break;
					}
				}
				//rtlglue_printf("utpmsk is %x",utpmsk.bits[0]);

				if(ret==0)	//means there is only the deleting interface using this WAN port
					mbpmsk.bits[0] &= (~(0x1<<rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx));		//negative the WAN PORT in LAN's VLAN member port mask
					
				all_lan_pmsk.bits[0]|=mbpmsk.bits[0];
				all_lan_etpmsk.bits[0]|=etpmsk.bits[0];
			
				//if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on)
					//utpmsk.bits[0]|=(0x1<<rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx);		//positive the WAN PORT in LAN's VLAN untag set

				ret = RTK_VLAN_PORT_SET(tmpVid, &mbpmsk, &utpmsk);
				errorno=RT_ERR_RG_VLAN_SET_FAIL;
				if(ret!=RT_ERR_OK)goto RET_ERR;
			}
			//}

			//Unset all LAN's member port and ext port
			memcpy(&mbpmsk, &rg_db.vlan[rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id].MemberPortmask,sizeof(rtk_portmask_t));
			memcpy(&utpmsk, &rg_db.vlan[rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id].UntagPortmask,sizeof(rtk_portmask_t));
			memcpy(&etpmsk, &rg_db.vlan[rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id].Ext_portmask,sizeof(rtk_portmask_t));

			mbpmsk.bits[0]&=(~(all_lan_pmsk.bits[0]));		//negative all LAN PORT in WAN's VLAN member port mask
			etpmsk.bits[0]&=(~(all_lan_etpmsk.bits[0]));	//negative all LAN ext PORT in WAN's VLAN member port mask
			
			ret = RTK_VLAN_PORT_SET(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id, &mbpmsk, &utpmsk);
			errorno=RT_ERR_RG_VLAN_SET_FAIL;
			if(ret!=RT_ERR_OK)goto RET_ERR;
			ret = RTK_VLAN_EXTPORT_SET(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id, &etpmsk);
			if(ret!=RT_ERR_OK)goto RET_ERR;
		}	
		else if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE)
		{
			//delete the pppoe table entry
			errorno=RT_ERR_RG_PPPOE_SET_FAIL;
			pppoeEt.sessionID=0;
			ret=RTK_L34_PPPOETABLE_SET(lan_or_wan_intf_idx,&pppoeEt);
			if(ret!=RT_ERR_OK)goto RET_ERR;
		}

		//1 FIXME: patch for DA==GatewayMac will hit layer2 unknown DA, if action is trap
		errorno=RT_ERR_RG_DELETE_GATEWAY_LUT_FAIL;
		if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on)	//cpu tagged
			utpmsk.bits[0]=0;
		else
			utpmsk.bits[0]=0x1<<RTK_RG_MAC_PORT_CPU;
		//Delete the interface MAC entry
		_rtk_rg_deleteGatewayMacEntry(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet,rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id,utpmsk.bits[0]);
	}
	else		//LAN interface
	{
		//The LAN interface can not be deleted when WAN interface had added
		//errorno=RT_ERR_RG_MODIFY_LAN_AT_WAN_EXIST;
		//if(rg_db.systemGlobal.wanIntfTotalNum>0)
			//goto RET_ERR;
			
		//re-gather all Lan's VLAN information
		all_lan_pmsk.bits[0]=0;
		all_lan_etpmsk.bits[0]=0;
		all_lan_utagpmsk.bits[0]=0;
		for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
		{
			if(rg_db.systemGlobal.lanIntfGroup[i].index==lan_or_wan_intf_idx)
				continue;
			tmpVid=rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->intf_vlan_id;
			//DEBUG("i = %d, vid = %d\n",i,tmpVid);		
			
			all_lan_pmsk.bits[0] |= rg_db.vlan[tmpVid].MemberPortmask.bits[0];
			all_lan_etpmsk.bits[0] |= rg_db.vlan[tmpVid].Ext_portmask.bits[0];
			all_lan_utagpmsk.bits[0] |= rg_db.vlan[tmpVid].UntagPortmask.bits[0];
		}

		//reset bridge WAN's VLAN setting, since there is LAN deleting
		for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
		{
			if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_type==RTK_RG_BRIDGE)
			{
				//add untag set by wan setting
				utpmsk.bits[0]=all_lan_utagpmsk.bits[0];
				if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_tag_on)			//egress tagged packet
					utpmsk.bits[0]&=(~(0x1<<rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx));	//set WAN port to 0 in untag set (tagging)
				else
				{
					utpmsk.bits[0]|=0x1<<rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx;		//set WAN port to 1 in untag set (untagging)
					//utpmsk.bits[0]|=0x1<<RTK_RG_MAC_PORT_CPU;	//set CPU port to 1 in untag set (untagging)
				}

				if(all_lan_pmsk.bits[0]==0 && all_lan_etpmsk.bits[0]==0)		//the last LAN intf is deleting
				{
					//reset to WAN port only
					all_lan_pmsk.bits[0]|=0x1<<rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx;
					errorno=RT_ERR_RG_VLAN_SET_FAIL;
					ret = RTK_VLAN_PORT_SET(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_id, &all_lan_pmsk, &utpmsk);
					if(ret!=RT_ERR_OK)goto RET_ERR;
					ret = RTK_VLAN_EXTPORT_SET(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_id, &all_lan_etpmsk);
					if(ret!=RT_ERR_OK)goto RET_ERR;
				}
				else
				{
					errorno=RT_ERR_RG_VLAN_SET_FAIL;
					ret = RTK_VLAN_PORT_SET(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_id, &all_lan_pmsk, &utpmsk);
					if(ret!=RT_ERR_OK)goto RET_ERR;
					ret = RTK_VLAN_EXTPORT_SET(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_id, &all_lan_etpmsk);
					if(ret!=RT_ERR_OK)goto RET_ERR;
				}
			}
		}

#if 0
		//If this is the last LAN interface, reset DEFAULT LAN VLAN to default value
		if(rg_db.systemGlobal.lanIntfTotalNum == 1)
		{
			mbpmsk.bits[0]=0x7f;	//all port
			utpmsk.bits[0]=0x7f;	//all untag
			etpmsk.bits[0]=0x3f;	//all extension port
		
			ret = RTK_VLAN_PORT_SET(DEFAULT_LAN_VLAN, &mbpmsk, &utpmsk);
			errorno=RT_ERR_RG_VLAN_SET_FAIL;
			if(ret!=RT_ERR_OK)goto RET_ERR;
			ret = RTK_VLAN_EXTPORT_SET(DEFAULT_LAN_VLAN, &etpmsk);
			if(ret!=RT_ERR_OK)goto RET_ERR;
		}
		else
		{
			//take off this LAN interface's member port and extension port from DEFAULT LAN VLAN
			//mbpmsk.bits[0]=0;
			//utpmsk.bits[0]=0;
			//etpmsk.bits[0]=0;
			//ret = rtk_vlan_port_get(DEFAULT_LAN_VLAN, &mbpmsk, &utpmsk);
			//errorno=RT_ERR_RG_VLAN_GET_FAIL;
			//if(ret!=RT_ERR_OK)goto RET_ERR;
			memcpy(&mbpmsk, &rg_db.vlan[DEFAULT_LAN_VLAN].MemberPortmask,sizeof(rtk_portmask_t));
			memcpy(&utpmsk, &rg_db.vlan[DEFAULT_LAN_VLAN].UntagPortmask,sizeof(rtk_portmask_t));
			//ret = rtk_vlan_extPort_get(DEFAULT_LAN_VLAN, &etpmsk);
			//if(ret!=RT_ERR_OK)goto RET_ERR;
			memcpy(&etpmsk, &rg_db.vlan[DEFAULT_LAN_VLAN].Ext_portmask,sizeof(rtk_portmask_t));

			
			//Transfer RG portmask to RTK portmask
			_rtk_rg_portmask_translator(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->port_mask,&out_mac_pmask,&out_ext_pmask);

			mbpmsk.bits[0]&=(~(out_mac_pmask.bits[0]));
			etpmsk.bits[0]&=(~(out_ext_pmask.bits[0]));

			ret = RTK_VLAN_PORT_SET(DEFAULT_LAN_VLAN, &mbpmsk, &utpmsk);
			errorno=RT_ERR_RG_VLAN_SET_FAIL;
			if(ret!=RT_ERR_OK)goto RET_ERR;
			ret = RTK_VLAN_EXTPORT_SET(DEFAULT_LAN_VLAN, &etpmsk);
			if(ret!=RT_ERR_OK)goto RET_ERR;
		}

		//Reset PPB setting, if any
		/*errorno=RT_ERR_RG_PPB_SET_FAILED;
		if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->pppoe_passThrough == 1)
		{
			bzero(&protoVlanCfg,sizeof(rtk_vlan_protoVlanCfg_t));
			protoVlanCfg.vid=DEFAULT_CPU_VLAN;		//fixme: rtk api won't accept vid=0, so we assign a dummy one
			if(out_ext_pmask.bits[0] > 0x1)		
			{		
				ret = rtk_vlan_portProtoVlan_set(RTK_RG_MAC_PORT_CPU,PPPOE_DISCOVERY_GROUPID,&protoVlanCfg);
				if(ret!=RT_ERR_OK)goto RET_ERR;
				ret = rtk_vlan_portProtoVlan_set(RTK_RG_MAC_PORT_CPU,PPPOE_SESSION_GROUPID,&protoVlanCfg);
				if(ret!=RT_ERR_OK)goto RET_ERR;
			}
			for(i=0;i<RTK_RG_PORT_CPU;i++)
			{
				if((out_mac_pmask.bits[0]&(0x1<<i)) > 0)
				{
					ret = rtk_vlan_portProtoVlan_set(i,PPPOE_DISCOVERY_GROUPID,&protoVlanCfg);
					if(ret!=RT_ERR_OK)goto RET_ERR;
					ret = rtk_vlan_portProtoVlan_set(i,PPPOE_SESSION_GROUPID,&protoVlanCfg);
					if(ret!=RT_ERR_OK)goto RET_ERR;
				}
			}
			errorno=RT_ERR_RG_VLAN_SET_FAIL;
			ret = RTK_VLAN_DESTROY(DEFAULT_PPB_VLAN_START+lan_or_wan_intf_idx);
			if(ret!=RT_ERR_OK)goto RET_ERR;
		}*/

		//Set back port-based and extport-based VLAN ID
		errorno=RT_ERR_RG_VLAN_SET_FAIL;
		for(i=0;i<RTK_RG_PORT_MAX;i++)
		{
			if((rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->port_mask.portmask&(0x1<<i)) > 0)
			{
				if(i<=RTK_RG_PORT_CPU)
				{				
					ret = rtk_vlan_portPvid_set(i, DEFAULT_CPU_VLAN);
					if(ret!=RT_ERR_OK)
						goto RET_ERR;			
				}
				else
				{
					ret = rtk_vlan_extPortPvid_set(i-RTK_RG_PORT_CPU, DEFAULT_CPU_VLAN);
					if(ret!=RT_ERR_OK)
						goto RET_ERR;
				}
				
				rg_db.systemGlobal.portBasedVID[i]=DEFAULT_CPU_VLAN;		//reset port-based and ext-port-based VLAN in rg_db
			}
		}
#endif

		//1 FIXME: patch for DA==GatewayMac will hit layer2 unknown DA, if action is trap
		errorno=RT_ERR_RG_DELETE_GATEWAY_LUT_FAIL;
		//Delete the interface MAC entry
		_rtk_rg_deleteGatewayMacEntry(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->gmac.octet,rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->intf_vlan_id,rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->untag_mask.portmask);
	}

    //Delete routing table entry, decrease Nexthop table ref count, if zero, delete nexthop entry
    for(i=0; i<MAX_L3_SW_TABLE_SIZE; i++)
    {
        //bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
        //ret = rtk_l34_routingTable_get(i, &rtEntry);
		errorno=RT_ERR_RG_ROUTE_GET_FAIL;
        //if(ret!=RT_ERR_OK)goto RET_ERR;

        if(rg_db.l3[i].rtk_l3.process == L34_PROCESS_ARP && rg_db.l3[i].rtk_l3.netifIdx == lan_or_wan_intf_idx)
        {
        	//Delete the ARP table entries referenced by routing table
        	for(j=(rg_db.l3[i].rtk_l3.arpStart<<2);j<((rg_db.l3[i].rtk_l3.arpEnd+1)<<2);j++)
        	{
        		bzero(&macEt, sizeof(rtk_rg_macEntry_t));
				bzero(&secondMacEt, sizeof(rtk_rg_macEntry_t));
				bzero(&arpInfo,sizeof(rtk_rg_arpInfo_t));
				count=0;
				
				ret = rtk_rg_arpEntry_find(&arpInfo,&j);
				if(ret==RT_ERR_RG_NO_MORE_ENTRY_FOUND)		//arp after j is all invalid
					break;
				if(j>=((rg_db.l3[i].rtk_l3.arpEnd+1)<<2))	//means this routing entry didn't has valid ARP
					break;
				
				ret = rtk_rg_arpEntry_del(j);
				if(ret!=RT_ERR_RG_OK)return ret;

				//Delete LUT table entries referenced by ARP:
				//If the VLAN is IVL, find if there is SVL LUT, delete it also, and vice versa.
				if(rg_db.lut[arpInfo.arpEntry.macEntryIdx].valid && rg_db.lut[arpInfo.arpEntry.macEntryIdx].rtk_lut.entryType==RTK_LUT_L2UC)
				{
					//Check fidmode
					if(rg_db.lut[arpInfo.arpEntry.macEntryIdx].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_IVL)
					{
						//delete SVL, too
						secondMacEt.isIVL=0;
						secondMacEt.fid=rg_db.lut[arpInfo.arpEntry.macEntryIdx].rtk_lut.entry.l2UcEntry.fid;
						//DEBUG("the SVL(%d) lut has to be deleted,too",secondMacEt.fid);
						l2Idx=_rtk_rg_hash_mac_fid_efid(rg_db.lut[arpInfo.arpEntry.macEntryIdx].rtk_lut.entry.l2UcEntry.mac.octet,secondMacEt.fid,rg_db.lut[arpInfo.arpEntry.macEntryIdx].rtk_lut.entry.l2UcEntry.efid);
					}
					else
					{
						//delete IVL, too
						secondMacEt.isIVL=1;

						//because the SVL LUT will set VID as 0 if untag, therefore VLAN ID should get for interface setting
						if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.is_wan==1)	//wan interface
							secondMacEt.vlan_id = rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;
						else	//lan interface
							secondMacEt.vlan_id = rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->intf_vlan_id;
						//DEBUG("the IVL(%d) lut has to be deleted,too",secondMacEt.vlan_id);
						l2Idx=_rtk_rg_hash_mac_vid_efid(rg_db.lut[arpInfo.arpEntry.macEntryIdx].rtk_lut.entry.l2UcEntry.mac.octet,secondMacEt.vlan_id,rg_db.lut[arpInfo.arpEntry.macEntryIdx].rtk_lut.entry.l2UcEntry.efid);
					}
					l2Idx<<=2;
			   		do
					{
						search_index = l2Idx+count;
						//DEBUG("search_idx is %d",search_index);
						if(rg_db.lut[search_index].valid==0)
						{
							count++;
							continue;	//empty
						}
										
						if(rg_db.lut[search_index].rtk_lut.entryType==RTK_LUT_L2UC &&
							(!memcmp(&rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,rg_db.lut[arpInfo.arpEntry.macEntryIdx].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN)))
						{
							if((secondMacEt.isIVL==1 && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.vid==secondMacEt.vlan_id) ||
								(secondMacEt.isIVL==0 && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.fid==secondMacEt.fid))
							{
								//DEBUG("%s MAC is exist @ %d, deleting...",secondMacEt.isIVL?"IVL":"SVL",search_index);
								rtk_rg_macEntry_del(search_index);
								break;
							}
						}
						else
							count++; //search from next entry
					}
					while(count < 4);

					//Delete original one
					ret = rtk_rg_macEntry_del(arpInfo.arpEntry.macEntryIdx);	
					//if the MAC is not exist, continue
					//if(ret!=RT_ERR_RG_OK)return ret;
				}
        	}

            //Deleting the routing entry
            bzero(&deletingEntry, sizeof(rtk_rg_routing_arpInfo_t));
            deletingEntry.routingIdx=i;
			if(rg_db.l3[i].rtk_l3.rt2waninf)		//routing to WAN
				deletingEntry.isLan=0;
			else
				deletingEntry.isLan=1;
			deletingEntry.bitNum=31-rg_db.l3[i].rtk_l3.ipMask;
			deletingEntry.arpStart=rg_db.l3[i].rtk_l3.arpStart;
			deletingEntry.arpEnd=rg_db.l3[i].rtk_l3.arpEnd;
            bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
			cb_routEt.dest_ip=rg_db.l3[i].rtk_l3.ipAddr;
			cb_routEt.ip_mask=rg_db.l3[i].rtk_l3.ipMask;
            ret = RTK_L34_ROUTINGTABLE_SET(i, &rtEntry);
			errorno=RT_ERR_RG_ROUTE_SET_FAIL;
            if(ret!=RT_ERR_OK)goto RET_ERR;

			//Rerrange ARP table
			ret=_rtk_rg_delArpRoutingArray(&deletingEntry);
			if(ret!=RT_ERR_OK)goto RET_ERR;

			// TODO:Call the initParam's routingDelByHwCallBack
			if(rg_db.systemGlobal.initParam.routingDelByHwCallBack != NULL)
			{
				cb_routEt.nexthop=0;
				cb_routEt.wan_intf_idx=lan_or_wan_intf_idx;
				rg_db.systemGlobal.initParam.routingDelByHwCallBack(&cb_routEt);
			}
        }
		else if(rg_db.l3[i].rtk_l3.process == L34_PROCESS_CPU && rg_db.l3[i].rtk_l3.ipAddr>0 && rg_db.l3[i].rtk_l3.netifIdx == lan_or_wan_intf_idx)		//delete routing which added sw ARP table
		{
			//Delete the ARP table entries referenced by routing table
			for(j=0;j<MAX_ARP_SW_TABLE_HEAD;j++)
			{
				list_for_each_entry_safe(pSwArpList,pNextSwArpList,&rg_db.softwareArpTableHead[j],arp_list)
				{
					if(pSwArpList->routingIdx==i)
					{
						//Delete LUT table entries referenced by ARP:
						//If the VLAN is IVL, find if there is SVL LUT, delete it also, and vice versa.
						if(rg_db.lut[rg_db.arp[pSwArpList->idx].rtk_arp.nhIdx].valid && rg_db.lut[rg_db.arp[pSwArpList->idx].rtk_arp.nhIdx].rtk_lut.entryType==RTK_LUT_L2UC)
						{
							//Check fidmode
							if(rg_db.lut[rg_db.arp[pSwArpList->idx].rtk_arp.nhIdx].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_IVL)
							{
								//delete SVL, too
								secondMacEt.isIVL=0;
								secondMacEt.fid=rg_db.lut[rg_db.arp[pSwArpList->idx].rtk_arp.nhIdx].rtk_lut.entry.l2UcEntry.fid;
								//DEBUG("the SVL(%d) lut has to be deleted,too",secondMacEt.fid);
								l2Idx=_rtk_rg_hash_mac_fid_efid(rg_db.lut[rg_db.arp[pSwArpList->idx].rtk_arp.nhIdx].rtk_lut.entry.l2UcEntry.mac.octet,secondMacEt.fid,rg_db.lut[rg_db.arp[pSwArpList->idx].rtk_arp.nhIdx].rtk_lut.entry.l2UcEntry.efid);
							}
							else
							{
								//delete IVL, too
								secondMacEt.isIVL=1;

								//because the SVL LUT will set VID as 0 if untag, therefore VLAN ID should get from interface setting
								if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.is_wan==1)	//wan interface
									secondMacEt.vlan_id = rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;
								else	//lan interface
									secondMacEt.vlan_id = rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->intf_vlan_id;
								//DEBUG("the IVL(%d) lut has to be deleted,too",secondMacEt.vlan_id);
								l2Idx=_rtk_rg_hash_mac_vid_efid(rg_db.lut[rg_db.arp[pSwArpList->idx].rtk_arp.nhIdx].rtk_lut.entry.l2UcEntry.mac.octet,secondMacEt.vlan_id,rg_db.lut[rg_db.arp[pSwArpList->idx].rtk_arp.nhIdx].rtk_lut.entry.l2UcEntry.efid);
							}
							l2Idx<<=2;
					   		do
							{
								search_index = l2Idx+count;
								//DEBUG("search_idx is %d",search_index);
								if(rg_db.lut[search_index].valid==0)
								{
									count++;
									continue;	//empty
								}
												
								if(rg_db.lut[search_index].rtk_lut.entryType==RTK_LUT_L2UC &&
									(!memcmp(&rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,rg_db.lut[rg_db.arp[pSwArpList->idx].rtk_arp.nhIdx].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN)))
								{
									if((secondMacEt.isIVL==1 && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.vid==secondMacEt.vlan_id) ||
										(secondMacEt.isIVL==0 && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.fid==secondMacEt.fid))
									{
										//DEBUG("%s MAC is exist @ %d, deleting...",secondMacEt.isIVL?"IVL":"SVL",search_index);
										rtk_rg_macEntry_del(search_index);
										break;
									}
								}
								else
									count++; //search from next entry
							}
							while(count < 4);

							//Delete original one
							ret = rtk_rg_macEntry_del(rg_db.arp[pSwArpList->idx].rtk_arp.nhIdx);	
							//if the MAC is not exist, continue
							//if(ret!=RT_ERR_RG_OK)return ret;
						}
						
						//Free software ARP list
						_rtk_rg_softwareArpTableDel(pSwArpList);
					}
				}
			}

			//Deleting the routing entry
            bzero(&deletingEntry, sizeof(rtk_rg_routing_arpInfo_t));
            deletingEntry.routingIdx=i;
			if(rg_db.l3[i].rtk_l3.rt2waninf)		//routing to WAN
				deletingEntry.isLan=0;
			else
				deletingEntry.isLan=1;
			deletingEntry.bitNum=31-rg_db.l3[i].rtk_l3.ipMask;
			deletingEntry.arpStart=rg_db.l3[i].rtk_l3.arpStart;
			deletingEntry.arpEnd=rg_db.l3[i].rtk_l3.arpEnd;
            bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
			cb_routEt.dest_ip=rg_db.l3[i].rtk_l3.ipAddr;
			cb_routEt.ip_mask=rg_db.l3[i].rtk_l3.ipMask;
            ret = RTK_L34_ROUTINGTABLE_SET(i, &rtEntry);
			errorno=RT_ERR_RG_ROUTE_SET_FAIL;
            if(ret!=RT_ERR_OK)goto RET_ERR;

			//no need to rerrange ARP table
			//ret=_rtk_rg_delArpRoutingArray(&deletingEntry);
			//if(ret!=RT_ERR_OK)goto RET_ERR;

			// TODO:Call the initParam's routingDelByHwCallBack
			if(rg_db.systemGlobal.initParam.routingDelByHwCallBack != NULL)
			{
				cb_routEt.nexthop=0;
				cb_routEt.wan_intf_idx=lan_or_wan_intf_idx;
				rg_db.systemGlobal.initParam.routingDelByHwCallBack(&cb_routEt);
			}
		}
        else if(rg_db.l3[i].rtk_l3.process == L34_PROCESS_NH)
        {
        	//FIXME:if load-balance is used, then here should modified for it!!
            //Lookup Nexthop table for checking related interface
            nh_num = rg_db.l3[i].rtk_l3.nhStart;
			nh_num += rg_db.l3[i].rtk_l3.nhNum;
			errorno=RT_ERR_RG_ROUTE_GET_FAIL;
            if(nh_num > MAX_NEXTHOP_HW_TABLE_SIZE)goto RET_ERR;

            for(j=rg_db.l3[i].rtk_l3.nhStart; j<nh_num; j++)
            {
                bzero(&nxpEt, sizeof(rtk_l34_nexthop_entry_t));
                ret = rtk_l34_nexthopTable_get(j, &nxpEt);
				errorno=RT_ERR_RG_NXP_GET_FAIL;
                if(ret!=RT_ERR_OK)goto RET_ERR;

                if(nxpEt.ifIdx == lan_or_wan_intf_idx)
                {
                	ret=_rtk_rg_decreaseNexthopReference(j);
					if(ret!=RT_ERR_RG_OK)goto RET_ERR;
 
                    match_intf=1;//match_intf++;
                }
            }

            //Deleting the routing entry
            if(match_intf == 1)//if(match_intf == (nh_num-rtEntry.nhStart))
            {
                bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
				cb_routEt.dest_ip=rg_db.l3[i].rtk_l3.ipAddr;
				cb_routEt.ip_mask=rg_db.l3[i].rtk_l3.ipMask;
                ret = RTK_L34_ROUTINGTABLE_SET(i, &rtEntry);
				errorno=RT_ERR_RG_ROUTE_SET_FAIL;
                if(ret!=RT_ERR_OK)goto RET_ERR;

				// TODO:Call the initParam's routingDelByHwCallBack
				if(rg_db.systemGlobal.initParam.routingDelByHwCallBack != NULL)
				{					
					cb_routEt.wan_intf_idx=lan_or_wan_intf_idx;
					cb_routEt.nexthop=rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->gateway_ipv4_addr;
					rg_db.systemGlobal.initParam.routingDelByHwCallBack(&cb_routEt);
				}
            }
        }
    }

	//Delete ipv6 routing table entry, decrease Nexthop table ref count, if zero delete nexthop entry
	for(i=0; i<MAX_IPV6_ROUTING_SW_TABLE_SIZE; i++)
	{
		errorno=RT_ERR_RG_ROUTE_GET_FAIL;
		if((rg_db.v6route[i].rtk_v6route.type == L34_IPV6_ROUTE_TYPE_LOCAL || rg_db.v6route[i].rtk_v6route.type == L34_IPV6_ROUTE_TYPE_TRAP) && rg_db.v6route[i].rtk_v6route.nhOrIfidIdx == lan_or_wan_intf_idx)		//FIXME:the trap routing entry need to be deleted
		{
			//Delete the Neighbor table entries referenced by routing table
			for(j=0;j<MAX_IPV6_NEIGHBOR_SW_TABLE_SIZE;j++)
			{
				bzero(&neighborInfo, sizeof(rtk_rg_neighborInfo_t));
				ret = rtk_rg_neighborEntry_find(&neighborInfo,&j);
				if(ret==RT_ERR_RG_NO_MORE_ENTRY_FOUND)		//neighbor after j is all invalid
					break;
				if(neighborInfo.neighborEntry.matchRouteIdx==i)
				{
					//Delete LUT table entries referenced by Neighbor
					ret = rtk_rg_neighborEntry_del(j);
					if(ret!=RT_ERR_RG_OK)return ret;
					
					//If the VLAN is IVL, find if there is SVL LUT, delete it also, and vice versa.
					if(rg_db.lut[neighborInfo.neighborEntry.l2Idx].valid && rg_db.lut[neighborInfo.neighborEntry.l2Idx].rtk_lut.entryType==RTK_LUT_L2UC)
					{
						//Check fidmode
						if(rg_db.lut[neighborInfo.neighborEntry.l2Idx].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_IVL)
						{
							//delete SVL, too
							secondMacEt.isIVL=0;
							secondMacEt.fid=rg_db.lut[neighborInfo.neighborEntry.l2Idx].rtk_lut.entry.l2UcEntry.fid;
							//DEBUG("the SVL(%d) lut has to be deleted,too",secondMacEt.fid);
							l2Idx=_rtk_rg_hash_mac_fid_efid(rg_db.lut[neighborInfo.neighborEntry.l2Idx].rtk_lut.entry.l2UcEntry.mac.octet,secondMacEt.fid,rg_db.lut[neighborInfo.neighborEntry.l2Idx].rtk_lut.entry.l2UcEntry.efid);
						}
						else
						{
							//delete IVL, too
							secondMacEt.isIVL=1;

							//because the SVL LUT will set VID as 0 if untag, therefore VLAN ID should get for interface setting
							if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.is_wan==1)	//wan interface
								secondMacEt.vlan_id = rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;
							else	//lan interface
								secondMacEt.vlan_id = rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->intf_vlan_id;
							//DEBUG("the IVL(%d) lut has to be deleted,too",secondMacEt.vlan_id);
							l2Idx=_rtk_rg_hash_mac_vid_efid(rg_db.lut[neighborInfo.neighborEntry.l2Idx].rtk_lut.entry.l2UcEntry.mac.octet,secondMacEt.vlan_id,rg_db.lut[neighborInfo.neighborEntry.l2Idx].rtk_lut.entry.l2UcEntry.efid);
						}
						l2Idx<<=2;
				   		do
						{
							search_index = l2Idx+count;
							//DEBUG("search_idx is %d",search_index);
							if(rg_db.lut[search_index].valid==0)
							{
								count++;
								continue;	//empty
							}
											
							if(rg_db.lut[search_index].rtk_lut.entryType==RTK_LUT_L2UC &&
								(!memcmp(&rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,rg_db.lut[neighborInfo.neighborEntry.l2Idx].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN)))
							{
								if((secondMacEt.isIVL==1 && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.vid==secondMacEt.vlan_id) ||
									(secondMacEt.isIVL==0 && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.fid==secondMacEt.fid))
								{
									//DEBUG("%s MAC is exist @ %d, deleting...",secondMacEt.isIVL?"IVL":"SVL",search_index);
									rtk_rg_macEntry_del(search_index);
									break;
								}
							}
							else
								count++; //search from next entry
						}
						while(count < 4);

						//Delete original one
						ret = rtk_rg_macEntry_del(neighborInfo.neighborEntry.l2Idx);
						//if L2 is not valid, do nothing
						//if(ret!=RT_ERR_RG_OK)return ret;
					}
				}
			}

			//Deleting the routing entry
			bzero(&rtv6Entry, sizeof(rtk_ipv6Routing_entry_t));
			memcpy(cb_routv6Et.dest_ip.ipv6_addr,rg_db.v6route[i].rtk_v6route.ipv6Addr.ipv6_addr,IPV6_ADDR_LEN);
			cb_routv6Et.prefix_len=rg_db.v6route[i].rtk_v6route.ipv6PrefixLen;
			cb_routv6Et.NhOrIntfIdx=rg_db.v6route[i].rtk_v6route.nhOrIfidIdx;
			cb_routv6Et.type=rg_db.v6route[i].rtk_v6route.type;
			ret = RTK_L34_IPV6ROUTINGTABLE_SET(i, &rtv6Entry);

			errorno=RT_ERR_RG_ROUTE_SET_FAIL;
			if(ret!=RT_ERR_OK)goto RET_ERR;

			// TODO:Call the initParam's v6routingDelByHwCallBack
			if(rg_db.systemGlobal.initParam.v6RoutingDelByHwCallBack != NULL)
			{
				rg_db.systemGlobal.initParam.v6RoutingDelByHwCallBack(&cb_routv6Et);
			}
		}
		else if(rg_db.v6route[i].rtk_v6route.type == L34_IPV6_ROUTE_TYPE_GLOBAL)		//nexthop
		{
			//FIXME:if load-balance is used, then here should modified for it!!
			//Lookup Nexthop table for checking related interface
			nh_num = rg_db.v6route[i].rtk_v6route.nhOrIfidIdx;

			errorno=RT_ERR_RG_ROUTE_GET_FAIL;
			if(nh_num > MAX_NEXTHOP_SW_TABLE_SIZE)goto RET_ERR;

			bzero(&nxpEt, sizeof(rtk_l34_nexthop_entry_t));
			ret = rtk_l34_nexthopTable_get(nh_num, &nxpEt);
			errorno=RT_ERR_RG_NXP_GET_FAIL;
			if(ret!=RT_ERR_OK)goto RET_ERR;

			if(nxpEt.ifIdx == lan_or_wan_intf_idx)
			{
				ret=_rtk_rg_decreaseNexthopReference(nh_num);
				if(ret!=RT_ERR_RG_OK)goto RET_ERR;

				match_intf=1;
			}

			//Deleting the routing entry
			if(match_intf == 1)
			{
				bzero(&rtv6Entry, sizeof(rtk_ipv6Routing_entry_t));
				memcpy(cb_routv6Et.dest_ip.ipv6_addr,rg_db.v6route[i].rtk_v6route.ipv6Addr.ipv6_addr,IPV6_ADDR_LEN);
				cb_routv6Et.prefix_len=rg_db.v6route[i].rtk_v6route.ipv6PrefixLen;
				cb_routv6Et.NhOrIntfIdx=rg_db.v6route[i].rtk_v6route.nhOrIfidIdx;
				cb_routv6Et.type=rg_db.v6route[i].rtk_v6route.type;
				ret = RTK_L34_IPV6ROUTINGTABLE_SET(i, &rtv6Entry);
				errorno=RT_ERR_RG_ROUTE_SET_FAIL;
				if(ret!=RT_ERR_OK)goto RET_ERR;

				// TODO:Call the initParam's v6routingDelByHwCallBack
				if(rg_db.systemGlobal.initParam.v6RoutingDelByHwCallBack != NULL)
				{					
					rg_db.systemGlobal.initParam.v6RoutingDelByHwCallBack(&cb_routv6Et);
				}
			}
		}
	}

    //Delete Internal External IP table entry, if any,decrease Nexthop table ref count, if zero delete nexthop entry
    if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.is_wan==1 &&
		rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type!=RTK_RG_BRIDGE)
    {
		//Reset Default route to CPU if ipv4_default_gateway_on is 1
		if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->ipv4_default_gateway_on == 1)
		{
			bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
			rtEntry.process=L34_PROCESS_CPU;
			rtEntry.valid=1;
            ret = RTK_L34_ROUTINGTABLE_SET(7, &rtEntry);
			errorno=RT_ERR_RG_ROUTE_SET_FAIL;
			if(ret!=RT_ERR_OK)goto RET_ERR;		
		}

		//Reset Default route to CPU if ipv6_default_gateway_on is 1
		if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->ipv6_default_gateway_on == 1)
		{
			bzero(&rtv6Entry, sizeof(rtk_ipv6Routing_entry_t));
			rtv6Entry.type=L34_IPV6_ROUTE_TYPE_TRAP;
			rtv6Entry.valid=1;
			ret = RTK_L34_IPV6ROUTINGTABLE_SET(3,&rtv6Entry);
			errorno=RT_ERR_RG_ROUTE_SET_FAIL;
			if(ret!=RT_ERR_OK)goto RET_ERR;		
		}

		napt_en=rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->napt_enable;
    
	    if(napt_en != 0)
	    {
			// TODO:$$$$$$$$$$$$$$$$$$$ Delete NAPTR, NAPT table with deleting interface using NAPT $$$$$$$$$$$$$$$$$
			
	        //bzero(&extipEt, sizeof(rtk_l34_ext_intip_entry_t));
	        //ret = rtk_l34_extIntIPTable_get(lan_or_wan_intf_idx, &extipEt);
			//errorno=RT_ERR_RG_EXTIP_GET_FAIL;
	        //if(ret!=RT_ERR_OK)goto RET_ERR;

	        //j=extipEt.nhIdx;
	        j=rg_db.extip[lan_or_wan_intf_idx].rtk_extip.nhIdx;
			ret=_rtk_rg_decreaseNexthopReference(j);
			if(ret!=RT_ERR_RG_OK)goto RET_ERR;

	        //Since the interface table is sync with IP table, we just delete the same index one here
	        bzero(&extipEt, sizeof(rtk_l34_ext_intip_entry_t));
	        ret = RTK_L34_EXTINTIPTABLE_SET(lan_or_wan_intf_idx, &extipEt);
			errorno=RT_ERR_RG_EXTIP_SET_FAIL;
	        if(ret!=RT_ERR_OK)goto RET_ERR;
	    }

	    //Delete WAN type table entry, decrease Nexthop table ref count, if zero delete nexthop entry
/*#ifndef CONFIG_APOLLO_RLE0371
        bzero(&wantEt, sizeof(rtk_wanType_entry_t));
        //ret = dal_apollomp_l34_wanTypeTable_get(lan_or_wan_intf_idx, &wantEt);		//FIXME:no RTK APIs
        ret = rtk_l34_wanTypeTable_get(lan_or_wan_intf_idx, &wantEt);
        if(ret==RT_ERR_CHIP_NOT_SUPPORTED)return RT_ERR_RG_CHIP_NOT_SUPPORT;
        if(ret!=RT_ERR_OK)return RT_ERR_RG_WANTYPE_GET_FAIL;
		
		j=wantEt.nhIdx;
#else*/
		//get next hop index from software WAN TYPE table
		//j=RG_GLB_WAN_TYPE[lan_or_wan_intf_idx];
		j = rg_db.wantype[lan_or_wan_intf_idx].rtk_wantype.nhIdx;
//#endif
        
		ret=_rtk_rg_decreaseNexthopReference(j);
		if(ret!=RT_ERR_RG_OK)goto RET_ERR;
        
//#ifndef CONFIG_APOLLO_RLE0371
        //Since the interface table is sync with WAN type table, we just delete the same index one here
        bzero(&wantEt, sizeof(rtk_wanType_entry_t));
        //ret = dal_apollomp_l34_wanTypeTable_set(lan_or_wan_intf_idx, &wantEt);		//FIXME:no RTK APIs
        ret = RTK_L34_WANTYPETABLE_SET(lan_or_wan_intf_idx, &wantEt);
		errorno=RT_ERR_RG_CHIP_NOT_SUPPORT;
        if(ret==RT_ERR_CHIP_NOT_SUPPORTED)goto RET_ERR;
		errorno=RT_ERR_RG_WANTYPE_SET_FAIL;
        if(ret!=RT_ERR_OK)goto RET_ERR;
/*#else
		//Reset software WAN TYPE table
		RG_GLB_WAN_TYPE[lan_or_wan_intf_idx]=-1;
#endif*/
		//Reset VLAN priority
		if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_pri != 0)
		{
			ret = RTK_VLAN_PRIORITY_SET(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id, 0);
			errorno=RT_ERR_RG_VLAN_SET_FAIL;
			if(ret!=RT_ERR_OK)goto RET_ERR;
			ret = RTK_VLAN_PRIORITYENABLE_SET(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id, DISABLED);
			errorno=RT_ERR_RG_VLAN_SET_FAIL;
			if(ret!=RT_ERR_OK)goto RET_ERR;
		}
	}

	//Delete the deleting interface's VLAN setting, if there is no other interface or binding rule use it
	ret = 0;
	if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.is_wan==1)	//wan interface
		tmpVid = rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;
	else	//lan interface
	{
		tmpVid = rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->intf_vlan_id;
		memcpy(&pvid_mac_pmask,&rg_db.vlan[tmpVid].MemberPortmask,sizeof(rtk_portmask_t));
		memcpy(&pvid_ext_pmask,&rg_db.vlan[tmpVid].Ext_portmask,sizeof(rtk_portmask_t));
	}

	for(i=0;i<MAX_NETIF_SW_TABLE_SIZE;i++)
	{
		if(i != lan_or_wan_intf_idx && rg_db.systemGlobal.interfaceInfo[i].valid == 1)
		{
			if(rg_db.systemGlobal.interfaceInfo[i].storedInfo.is_wan==1)	//wan interface
			{
				if(rg_db.systemGlobal.interfaceInfo[i].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id==tmpVid)
					ret++;
			}
			else	//lan interface
			{
				if(rg_db.systemGlobal.interfaceInfo[i].p_lanIntfConf->intf_vlan_id==tmpVid)
					ret++;
			}
		}
	}
	for(i=0;i<MAX_BIND_SW_TABLE_SIZE;i++)
	{
		//if(rg_db.systemGlobal.bindWithVLAN[i]==tmpVid)
		if(rg_db.bind[i].valid == 1 && rg_db.bind[i].rtk_bind.vidLan==tmpVid)
			ret++;
	}
	if(ret==0)
	{
		ret = RTK_VLAN_DESTROY(tmpVid);
		errorno=RT_ERR_RG_VLAN_SET_FAIL;
		if(ret!=RT_ERR_OK)goto RET_ERR;
	}

    //Delete interface table entry
    bzero(&intfEt, sizeof(rtk_l34_netif_entry_t));
    ret = RTK_L34_NETIFTABLE_SET(lan_or_wan_intf_idx, &intfEt);
	errorno=RT_ERR_RG_INTF_SET_FAIL;
    if(ret!=RT_ERR_OK)goto RET_ERR;

    /*for(i=0;i<8;i++)
    {
    	//Lookup IP table for checking related interface
    	memset(&extipEt, 0, sizeof(extipEt));
    	ret = rtk_l34_extIntIPTable_get(i, &extipEt);
    	if(ret!=RT_ERR_OK)return RT_ERR_RG_EXTIP_FAIL;

    	//Lookup nexthop table
    	memset(&nxpEt, 0, sizeof(nxpEt));
    	ret = rtk_l34_nexthopTable_get(extipEt.nhIdx, &nxpEt);
    	if(ret!=RT_ERR_OK)return RT_ERR_RG_NXP_FAIL;

    	if(nxpEt.ifIdx == lan_or_wan_intf_idx)
    	{
    		rg_db.systemGlobal.nxpRefCount[j]--;
    		match_intf = 1;

    		if(rg_db.systemGlobal.nxpRefCount[j]==0)
    		{
    			//Delete Nexthop entry, since nobody use it
    			if(nxpEt.type==L34_NH_PPPOE)
    			{
    				//Delete PPPoE table
    				memset(&pppEt, 0, sizeof(pppEt));
    				RTK_L34_PPPOETABLE_SET(nxpEt.pppoeIdx, &pppEt);
    				if(ret!=RT_ERR_OK)return RT_ERR_RG_PPPOE_FAIL;
    			}
    			memset(&nxpEt, 0, sizeof(nxpEt));
    			ret = RTK_L34_NEXTHOPTABLE_SET(j, &nxpEt);
    			if(ret!=RT_ERR_OK)return RT_ERR_RG_NXP_FAIL;
    		}
    	}
    }*/

	//keep Global variable structure for callback function
	bzero(&keep_store_info,sizeof(rtk_rg_intfInfo_t));
	memcpy(&keep_store_info,&rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo,sizeof(rtk_rg_intfInfo_t));

    //Reset Global variable structure    
    rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].valid=0;
    bzero(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.intf_name, 32);
    rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.ingress_packet_count=0;
    rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.ingress_byte_count=0;
    rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.egress_packet_count=0;
    rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.egress_byte_count=0;
    if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.is_wan == 1)
    {
    	//Reset untag bridge WAN set and bridge WAN number
		if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE)
		{
			if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on==0)
				rg_db.systemGlobal.untagBridgeWanSet[rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx]=0;
			//rg_db.systemGlobal.bridgeWanNum--;

			//Reset broadcast and unknown unicast flood port mask
			/*if(rg_db.systemGlobal.bridgeWanNum==0)
			{
				mbpmsk.bits[0]=0x7f;	//all MAC port
				assert_ok(rtk_l2_lookupMissFloodPortMask_set(DLF_TYPE_BCAST,&mbpmsk));		//set broadcast
				assert_ok(rtk_l2_lookupMissFloodPortMask_set(DLF_TYPE_UCAST,&mbpmsk));
			}*/
		}
		
    	//Reset WAN set mask
    	wan_set_mask=0x1<<lan_or_wan_intf_idx;
    	rg_db.systemGlobal.wanInfoSet &= ~(wan_set_mask);

		//Decrease Global WAN interface count
		rg_db.systemGlobal.wanIntfTotalNum--;		//decrease WAN interface number
#if 1
		//Reset WAN group entry, defragmentation if needed
		if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].lan_or_wan_index != rg_db.systemGlobal.wanIntfTotalNum)
		{	
			for(i=rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].lan_or_wan_index;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
			{
				memcpy(&rg_db.systemGlobal.wanIntfGroup[i],&rg_db.systemGlobal.wanIntfGroup[i+1],sizeof(rtk_rg_wan_interface_group_info_t));
				//rg_db.systemGlobal.wanIntfGroup[i].index = rg_db.systemGlobal.wanIntfGroup[i+1].index;
				//rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo = rg_db.systemGlobal.wanIntfGroup[i+1].p_intfInfo;
				//rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf = rg_db.systemGlobal.wanIntfGroup[i+1].p_wanIntfConf;
				rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->lan_or_wan_index--;
			}
		}
#else		
		//Reset WAN group entry, defragmentation if needed
		//since interface order is not important, we just move the last one to the deleting index
		if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].lan_or_wan_index != rg_db.systemGlobal.wanIntfTotalNum)
		{
			rg_db.systemGlobal.wanIntfGroup[rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].lan_or_wan_index].index = 
				rg_db.systemGlobal.wanIntfGroup[rg_db.systemGlobal.wanIntfTotalNum].index;
			rg_db.systemGlobal.wanIntfGroup[rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].lan_or_wan_index].p_intfInfo= 
				rg_db.systemGlobal.wanIntfGroup[rg_db.systemGlobal.wanIntfTotalNum].p_intfInfo;
		}
#endif
		rg_db.systemGlobal.wanIntfGroup[rg_db.systemGlobal.wanIntfTotalNum].index=0;
		rg_db.systemGlobal.wanIntfGroup[rg_db.systemGlobal.wanIntfTotalNum].p_intfInfo=NULL;
		rg_db.systemGlobal.wanIntfGroup[rg_db.systemGlobal.wanIntfTotalNum].p_wanIntfConf=NULL;

        //Clear WAN structure
        bzero(&rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac, sizeof(rtk_mac_t));
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx=0;
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.port_binding_mask.portmask=0;
        //rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.extport_binding_mask.bits[0]=0;
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on=0;		
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id=0;
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_pri=0;
		
		
		bzero(&rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.next_hop_mac, sizeof(rtk_mac_t));
		if(rg_db.systemGlobal.defaultRouteSet == lan_or_wan_intf_idx)
			rg_db.systemGlobal.defaultRouteSet=-1;
		if(rg_db.systemGlobal.defaultIPV6RouteSet == lan_or_wan_intf_idx)
			rg_db.systemGlobal.defaultIPV6RouteSet=-1;
		/*if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.default_gateway_on==1)
		{
			rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.default_gateway_on=0;
			rg_db.systemGlobal.defaultRouteSet=-1;
		}*/

		if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo!=NULL)	//not bridge WAN
		{
			rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->ip_version=0;		//default ipv4
			rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->napt_enable=0;
	        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->ip_addr=0;
	        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->ip_network_mask=0;
			bzero(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->ipv6_addr.ipv6_addr,IPV6_ADDR_LEN);
			rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->ipv6_mask_length=0;
			rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->ipv4_default_gateway_on=0;
			rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->gateway_ipv4_addr=0;
			rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->ipv6_default_gateway_on=0;
			bzero(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->gateway_ipv6_addr.ipv6_addr,IPV6_ADDR_LEN);
	        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->mtu=0;
			rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->gw_mac_auto_learn_for_ipv4=0;
			bzero(&rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->gateway_mac_addr_for_ipv4,sizeof(rtk_mac_t));
			rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->gw_mac_auto_learn_for_ipv6=0;
			bzero(&rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo->gateway_mac_addr_for_ipv6,sizeof(rtk_mac_t));
		}
		
        if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_DHCP)
        {
            rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.dhcp_client_info.stauts=0;
        }
        else if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE)
        {
            rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.pppoe_info.after_dial.sessionId=0;

            bzero(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.username, 4);
            bzero(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.password, 4);
            rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.auth_type=0;
            rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.pppoe_proxy_enable=0;
            rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.max_pppoe_proxy_num=0;
            rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.auto_reconnect=0;
            rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.dial_on_demond=0;
            rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.idle_timeout_secs=0;
            rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.stauts=0;
            rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.dialOnDemondCallBack=NULL;
            rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.idleTimeOutCallBack=NULL;
        }

		rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_wanStaticInfo=NULL;
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type=0;
    }
    else
    {
		//Decrease Global LAN interface count
		rg_db.systemGlobal.lanIntfTotalNum--;		//decrease LAN interface number
#if 1		
    	//Reset LAN group entry, defragmentation if needed
		if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].lan_or_wan_index != rg_db.systemGlobal.lanIntfTotalNum)
		{	
			for(i=rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].lan_or_wan_index;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
			{
				rg_db.systemGlobal.lanIntfGroup[i].index = rg_db.systemGlobal.lanIntfGroup[i+1].index;
				rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo = rg_db.systemGlobal.lanIntfGroup[i+1].p_intfInfo;
				rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->lan_or_wan_index--;
			}
		}
#else
		//Reset LAN group entry, defragmentation if needed
		//since interface order is not important, we just move the last one to the deleting index
		if(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].lan_or_wan_index != rg_db.systemGlobal.lanIntfTotalNum)
		{
			rg_db.systemGlobal.lanIntfGroup[rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].lan_or_wan_index].index = 
				rg_db.systemGlobal.lanIntfGroup[rg_db.systemGlobal.lanIntfTotalNum].index;
			rg_db.systemGlobal.lanIntfGroup[rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].lan_or_wan_index].p_intfInfo= 
				rg_db.systemGlobal.lanIntfGroup[rg_db.systemGlobal.lanIntfTotalNum].p_intfInfo;
		}
#endif
		rg_db.systemGlobal.lanIntfGroup[rg_db.systemGlobal.lanIntfTotalNum].index=0;
		rg_db.systemGlobal.lanIntfGroup[rg_db.systemGlobal.lanIntfTotalNum].p_intfInfo=NULL;
		
        //Clear LAN structure
        bzero(&rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->gmac, sizeof(rtk_mac_t));
		rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->ip_version=0;
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->ip_addr=0;
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->ip_network_mask=0;
		bzero(rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->ipv6_addr.ipv6_addr,IPV6_ADDR_LEN);
		rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->ipv6_network_mask_length=0;
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->port_mask.portmask=0;
        //rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.lan_intf.extport_mask.bits[0]=0;
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->intf_vlan_id=0;
		//rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->pppoe_passThrough=0;
#if 0
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.lan_intf.dhcp_server_enable=0;
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.lan_intf.lease_time=0;
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.lan_intf.dhcp_start_ip_addr=0;
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.lan_intf.dhcp_end_ip_addr=0;
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.lan_intf.dhcp_port_binding_mask.bits[0]=0;
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo.lan_intf.dhcp_extport_binding_mask.bits[0]=0;
#endif
        rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf->mtu=0;
		rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].p_lanIntfConf=NULL;

		//Update PVID
		_rtk_rg_updatePortBasedVIDByLanOrder(pvid_mac_pmask, pvid_ext_pmask);
    }

	errorno=RT_ERR_RG_OK;

	//Check PPPoE Pass through
	_rtk_rg_refreshPPPoEPassThroughLanOrWanPortMask();

	//del wan-interfcae callback to sync protocal-stack
	{
		if(rg_db.systemGlobal.initParam.interfaceDelByHwCallBack != NULL)
		{
			//rg_db.systemGlobal.interfaceInfo[lan_or_wan_intf_idx].storedInfo  has been reset! so, use keep_store_info
			rg_db.systemGlobal.initParam.interfaceDelByHwCallBack(&keep_store_info,&lan_or_wan_intf_idx);
		}
	}

RET_ERR:
	//------------------ Critical Section End -----------------------//
	rg_unlock(&rg_kernel.interfaceLock);
    return errorno;
}

int32 rtk_rg_intfInfo_find(rtk_rg_intfInfo_t *intf_info, int *valid_lan_or_wan_intf_idx)
{
    int i,/*ret,*/valid_idx;
	//ipaddr_t search_ip, search_mask;
    //rtk_l34_netif_entry_t intfEt;
    rtk_ipv6_addr_t zeroV6Addr;

    //Check parameter
    if(intf_info == NULL || valid_lan_or_wan_intf_idx == NULL)
        return RT_ERR_RG_NULL_POINTER;
    valid_idx = *valid_lan_or_wan_intf_idx;
    if(valid_idx<-1 || valid_idx>7)
        return RT_ERR_RG_INVALID_PARAM;

	if(valid_idx == -1)
	{	
		//Check IP address valid or not
		bzero(zeroV6Addr.ipv6_addr,IPV6_ADDR_LEN);
		if(intf_info->lan_intf.ip_addr == 0 && memcmp(intf_info->lan_intf.ipv6_addr.ipv6_addr,zeroV6Addr.ipv6_addr,IPV6_ADDR_LEN) == 0)
			return RT_ERR_RG_INVALID_PARAM;

		
		//search_ip=intf_info->lan_intf.ip_addr;
		//search_mask=intf_info->lan_intf.ip_network_mask;
				
		//Find interface by ip
		//Search Wan first, then Lan

		for(i=0; i<rg_db.systemGlobal.wanIntfTotalNum; i++)
		{
			//Bridge WAN won't be compared with
			if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_type==RTK_RG_BRIDGE)
				continue;


			if(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->p_wanStaticInfo->ip_addr == intf_info->lan_intf.ip_addr ||
				memcmp(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->p_wanStaticInfo->ipv6_addr.ipv6_addr,intf_info->lan_intf.ipv6_addr.ipv6_addr,IPV6_ADDR_LEN)==0)

			{
				valid_idx=rg_db.systemGlobal.wanIntfGroup[i].index;
				goto MATCHING_IDX;
			}
		}

		for(i=0; i<rg_db.systemGlobal.lanIntfTotalNum; i++)
		{
			if(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->ip_addr == intf_info->lan_intf.ip_addr ||
				memcmp(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->ipv6_addr.ipv6_addr,intf_info->lan_intf.ipv6_addr.ipv6_addr,IPV6_ADDR_LEN)==0)
			{
				valid_idx=rg_db.systemGlobal.lanIntfGroup[i].index;
				goto MATCHING_IDX;
			}
		}

		return RT_ERR_RG_ENTRY_NOT_EXIST;
	}
	else
	{
	    //Find the first valid interface from valid_lan_or_wan_intf_idx
	    for(i=valid_idx; i<MAX_NETIF_SW_TABLE_SIZE; i++)
	    {
	        //bzero(&intfEt, sizeof(rtk_l34_netif_entry_t));
	        //ret = rtk_l34_netifTable_get(i, &intfEt);
	        //if(ret!=RT_ERR_OK)return RT_ERR_RG_INTF_GET_FAIL;

	        //if(intfEt.valid != 0)
	        if(rg_db.systemGlobal.interfaceInfo[i].valid != 0)
	        {
	            valid_idx = i;
	            break;
	        }
	    }
	    if(i==MAX_NETIF_SW_TABLE_SIZE)
			return RT_ERR_RG_ENTRY_NOT_EXIST;
	}

MATCHING_IDX:

	bzero(intf_info,sizeof(rtk_rg_intfInfo_t));
	memcpy(intf_info,&rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo,sizeof(rtk_rg_intfInfo_t));
#if 0	
    memcpy(intf_info->intf_name, rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.intf_name, 32);
    intf_info->is_wan=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.is_wan;
    if(intf_info->is_wan)
    {
        intf_info->wan_intf.wan_intf_conf.wan_type=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.wan_intf_conf.wan_type;
        memcpy(&intf_info->wan_intf.wan_intf_conf.gmac, &rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.wan_intf_conf.gmac, sizeof(rtk_mac_t));
        intf_info->wan_intf.wan_intf_conf.wan_port_idx=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx;
        intf_info->wan_intf.wan_intf_conf.port_binding_mask.portmask=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.wan_intf_conf.port_binding_mask.portmask;
        intf_info->wan_intf.wan_intf_conf.egress_vlan_tag_on=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on;
        intf_info->wan_intf.wan_intf_conf.egress_vlan_id=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;
        intf_info->wan_intf.wan_intf_conf.egress_vlan_pri=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_pri;

		if(intf_info->wan_intf.wan_intf_conf.wan_type != RTK_RG_BRIDGE)
		{
			intf_info->wan_intf.static_info.ip_version=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->ip_version;
			intf_info->wan_intf.static_info.napt_enable=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->napt_enable;
	        intf_info->wan_intf.static_info.ip_addr=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->ip_addr;
	        intf_info->wan_intf.static_info.ip_network_mask=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->ip_network_mask;
			memcpy(intf_info->wan_intf.static_info.ipv6_addr.ipv6_addr,rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->ipv6_addr.ipv6_addr,IPV6_ADDR_LEN);
	        intf_info->wan_intf.static_info.ipv6_mask_length=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->ipv6_mask_length;
			intf_info->wan_intf.static_info.ipv4_default_gateway_on=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->ipv4_default_gateway_on;
	        intf_info->wan_intf.static_info.gateway_ipv4_addr=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->gateway_ipv4_addr;
			intf_info->wan_intf.static_info.ipv6_default_gateway_on=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->ipv6_default_gateway_on;
	        intf_info->wan_intf.static_info.gateway_ipv6_addr=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->gateway_ipv6_addr;
	        intf_info->wan_intf.static_info.dns_ip_addr1=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->dns_ip_addr1;
	        intf_info->wan_intf.static_info.dns_ip_addr2=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->dns_ip_addr2;
	        intf_info->wan_intf.static_info.dns_ip_addr3=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->dns_ip_addr3;
	        intf_info->wan_intf.static_info.mtu=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->mtu;
			intf_info->wan_intf.static_info.gw_mac_auto_learn_for_ipv4=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->gw_mac_auto_learn_for_ipv4;
			memcpy(&intf_info->wan_intf.static_info.gateway_mac_addr_for_ipv4,&rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->gateway_mac_addr_for_ipv4,sizeof(rtk_mac_t));
			intf_info->wan_intf.static_info.gw_mac_auto_learn_for_ipv4=rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->gw_mac_auto_learn_for_ipv4;
			memcpy(&intf_info->wan_intf.static_info.gateway_mac_addr_for_ipv6,&rg_db.systemGlobal.interfaceInfo[valid_idx].p_wanStaticInfo->gateway_mac_addr_for_ipv6,sizeof(rtk_mac_t));

			if(intf_info->wan_intf.wan_intf_conf.wan_type == RTK_RG_DHCP)
	        {
	            intf_info->wan_intf.dhcp_client_info.stauts=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.dhcp_client_info.stauts;
	        }
	        else if(intf_info->wan_intf.wan_intf_conf.wan_type == RTK_RG_PPPoE)
	        {
	            intf_info->wan_intf.pppoe_info.after_dial.sessionId=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.pppoe_info.after_dial.sessionId;

	            bzero(intf_info->wan_intf.pppoe_info.before_dial.username, 4);
	            memcpy(intf_info->wan_intf.pppoe_info.before_dial.username, rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.pppoe_info.before_dial.username, 4);
	            bzero(intf_info->wan_intf.pppoe_info.before_dial.password, 4);
	            memcpy(intf_info->wan_intf.pppoe_info.before_dial.password, rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.pppoe_info.before_dial.password, 4);
	            intf_info->wan_intf.pppoe_info.before_dial.auth_type=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.pppoe_info.before_dial.auth_type;
	            intf_info->wan_intf.pppoe_info.before_dial.pppoe_proxy_enable=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.pppoe_info.before_dial.pppoe_proxy_enable;
	            intf_info->wan_intf.pppoe_info.before_dial.max_pppoe_proxy_num=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.pppoe_info.before_dial.max_pppoe_proxy_num;
	            intf_info->wan_intf.pppoe_info.before_dial.auto_reconnect=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.pppoe_info.before_dial.auto_reconnect;
	            intf_info->wan_intf.pppoe_info.before_dial.dial_on_demond=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.pppoe_info.before_dial.dial_on_demond;
	            intf_info->wan_intf.pppoe_info.before_dial.idle_timeout_secs=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.pppoe_info.before_dial.idle_timeout_secs;
	            intf_info->wan_intf.pppoe_info.before_dial.stauts=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.pppoe_info.before_dial.stauts;
	            intf_info->wan_intf.pppoe_info.before_dial.dialOnDemondCallBack=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.pppoe_info.before_dial.dialOnDemondCallBack;
	            intf_info->wan_intf.pppoe_info.before_dial.idleTimeOutCallBack=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.wan_intf.pppoe_info.before_dial.idleTimeOutCallBack;
	        }
		}
    }
    else
    {
		intf_info->lan_intf.ip_version=rg_db.systemGlobal.interfaceInfo[valid_idx].p_lanIntfConf->ip_version;
        memcpy(&intf_info->lan_intf.gmac, &rg_db.systemGlobal.interfaceInfo[valid_idx].p_lanIntfConf->gmac, sizeof(rtk_mac_t));
        intf_info->lan_intf.ip_addr=rg_db.systemGlobal.interfaceInfo[valid_idx].p_lanIntfConf->ip_addr;
        intf_info->lan_intf.ip_network_mask=rg_db.systemGlobal.interfaceInfo[valid_idx].p_lanIntfConf->ip_network_mask;
		memcpy(intf_info->lan_intf.ipv6_addr.ipv6_addr,rg_db.systemGlobal.interfaceInfo[valid_idx].p_lanIntfConf->ipv6_addr.ipv6_addr,IPV6_ADDR_LEN);
        intf_info->lan_intf.ipv6_network_mask_length=rg_db.systemGlobal.interfaceInfo[valid_idx].p_lanIntfConf->ipv6_network_mask_length;
        intf_info->lan_intf.port_mask.portmask=rg_db.systemGlobal.interfaceInfo[valid_idx].p_lanIntfConf->port_mask.portmask;
		intf_info->lan_intf.untag_mask.portmask=rg_db.systemGlobal.interfaceInfo[valid_idx].p_lanIntfConf->untag_mask.portmask;
        intf_info->lan_intf.intf_vlan_id=rg_db.systemGlobal.interfaceInfo[valid_idx].p_lanIntfConf->intf_vlan_id;
#if 0
        intf_info->lan_intf.dhcp_server_enable=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.lan_intf.dhcp_server_enable;
        intf_info->lan_intf.lease_time=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.lan_intf.lease_time;
        intf_info->lan_intf.dhcp_start_ip_addr=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.lan_intf.dhcp_start_ip_addr;
        intf_info->lan_intf.dhcp_end_ip_addr=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.lan_intf.dhcp_end_ip_addr;
        intf_info->lan_intf.dhcp_port_binding_mask.bits[0]=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.lan_intf.dhcp_port_binding_mask.bits[0];
        intf_info->lan_intf.dhcp_extport_binding_mask.bits[0]=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.lan_intf.dhcp_extport_binding_mask.bits[0];
#endif
        intf_info->lan_intf.mtu=rg_db.systemGlobal.interfaceInfo[valid_idx].p_lanIntfConf->mtu;
		intf_info->lan_intf.pppoe_passThrough=rg_db.systemGlobal.interfaceInfo[valid_idx].p_lanIntfConf->pppoe_passThrough;
    }

    intf_info->ingress_packet_count=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.ingress_packet_count;
    intf_info->ingress_byte_count=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.ingress_byte_count;
    intf_info->egress_packet_count=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.egress_packet_count;
    intf_info->egress_byte_count=rg_db.systemGlobal.interfaceInfo[valid_idx].storedInfo.egress_byte_count;
#endif
    //Return the valid index
    *valid_lan_or_wan_intf_idx = valid_idx;

    return RT_ERR_RG_OK;
}

uint32 _rtk_rg_NAPTRIndex_get(uint16 isTCP, uint32 dip, uint16 dport)
{

	uint32 eidx=0;
    /*
        SUM = added by 1's complementary arithmetic( DIP[7:0] + DIP[15:8] + DIP[23:16] + DIP[31:24] + DPOR[7:0] )
        NAPTR hash ID[7:0] = SUM[7:0] ^ DPORT[15:8] ^ ( TCP << 7)A!KA!K.. HashIN
    */
	eidx = ((dip&0xff) + ((dip>>8)&0xff) + ((dip>>16)&0xff) + ((dip>>24)&0xff) + (dport&0xff));
	eidx = ((eidx&0xff) + ((eidx>>8)&0xff)) & 0xff;
	eidx = (eidx&0xff) ^ ((dport>>8)&0xff) ^ (isTCP << 7);

	return eidx;
}

unsigned int _rtk_rg_NAPTIndex_get(unsigned char isTCP, unsigned int srcAddr, unsigned short srcPort, unsigned int destAddr, unsigned short destPort) 
{
	unsigned int eidx;

	eidx = (((destAddr&0x3)<<16) | srcPort) + ((destAddr>>2)&0x3ffff);
	eidx = (eidx&0x3ffff) + (eidx>>18);
    eidx += (((srcAddr&0x3f)<<12) | (destAddr>>20));
	eidx = (eidx&0x3ffff) + (eidx>>18);
	eidx += ((srcAddr>>6)&0x3ffff);
	eidx = (eidx&0x3ffff) + (eidx>>18);
	eidx += (((destPort&0x3ff)<<8) | (srcAddr>>24));
    eidx = (eidx&0x3ffff) + (eidx>>18);
	eidx = (eidx&0x1ff) + (eidx>>9);
	eidx = eidx     + ((isTCP<<8) | ((destPort>>10)<<2));
	eidx = ((eidx&0x1ff) + (eidx>>9))&0x1ff;

    return eidx;
}

unsigned int _rtk_rg_NAPTRemoteHash_get( unsigned int ip, unsigned int port)
{
        unsigned short hash_value=0;

        hash_value = ((ip&0xffff) ^ ((ip>>16)&0xffff) ^ (port));

        return hash_value;
}

unsigned char _rtk_rg_IPv6NeighborHash(unsigned char *ifid, unsigned char rtidx)
{
	/*
	
	nb_8bhash_idx[7:0] = pkt.dip[7:0] ^ pkt.dip[15:8] ^  pkt.dip[23:16] ^ pkt.dip[31:24] ^
				  	     pkt.dip[39:32] ^ pkt.dip[47:40] ^  pkt.dip[55:48] ^ pkt.dip[63:56] ;
	nb_hash_idx[3:0] = nb_8bhash_idx[3:0] ^ nb_8bhash_idx[7:4] ^ 
					{ Ipv6_rtidx[0], Ipv6_rtidx[1], Ipv6_rtidx[1:0] } ;
	*/
	
	unsigned char pre_idx;
	unsigned char rightHalf;
	unsigned char leftHalf;
	unsigned char rtidx_0;
	unsigned char rtidx_1;
	unsigned char newrtidx;
	unsigned char hash_idx;
	
	pre_idx = ifid[7]^ifid[6]^ifid[5]^ifid[4]^ifid[3]^ifid[2]^ifid[1]^ifid[0];

	rightHalf = pre_idx&0xf;
	leftHalf = (pre_idx>>4)&0xf;

	rtidx_0 = (rtidx&0x1)<<3;
	rtidx_1 = (rtidx&0x2)<<1;
	newrtidx = rtidx_0 | rtidx_1 | rtidx;
	hash_idx = rightHalf^leftHalf^newrtidx;
	
	return hash_idx;
}

#ifdef __KERNEL__
void txinfo_debug(struct tx_info *pTxInfo);
int _rtk_rg_egressPacketSend(struct sk_buff *skb, rtk_rg_pktHdr_t *pPktHdr)
{
	//Modify Packet by ACL actions
	_rtk_rg_modifyPacketByACLAction(skb,pPktHdr);

	//Call NIC driver api to send packet
	return re8686_send_with_txInfo_and_mask(skb,&rg_kernel.txDesc,0,&rg_kernel.txDescMask);
}

int _rtk_rg_broadcastForward(struct sk_buff *skb, unsigned int internalVlanID, unsigned int srcPort,unsigned extSpa)
{
	return _rtk_rg_broadcastForwardWithPkthdr(NULL,skb,internalVlanID,srcPort,extSpa);
}

int _rtk_rg_broadcastForwardWithPkthdr(rtk_rg_pktHdr_t *pPktHdr, struct sk_buff *skb, unsigned int internalVlanID, unsigned int srcPort,unsigned int extSpa)
{
	struct sk_buff *bcSkb=NULL,*cpSkb=NULL;
	int i,isGatewayMac=0;
	unsigned char *pSourceMac;
	unsigned int dpMask,wanPortMask=0;
	//Return RT_ERR_RG_FAILED to protocol stack, return RT_ERR_RG_OK after packets sended

#if 1
	if(memcmp(skb->data,"\x01\x80\xc2",3)==0)
	{
		FIXME("trap 01:80:C2:XX:XX:XX!");
		return RG_FWDENGINE_RET_TO_PS;
	}
#endif

	cpSkb=rtk_rg_skbCopyToPreAllocSkb(skb);
	if(cpSkb==NULL)	goto OUT_OF_MEM;

	// clear old configed fields.
	rg_kernel.txDescMask.opts1.dw=0;
	rg_kernel.txDescMask.opts2.dw=0;	
	rg_kernel.txDescMask.opts3.dw=0;
	//rg_kernel.txDescMask.opts4.dw=0;	//not used now!
	rg_kernel.txDesc.opts1.dw=0;
	rg_kernel.txDesc.opts2.dw=0;	
	rg_kernel.txDesc.opts3.dw=0;	
	//rg_kernel.txDesc.opts4.dw=0;	//not used now!
	
	
	rg_kernel.txDescMask.opts1.bit.ipcs=1;
	rg_kernel.txDescMask.opts1.bit.l4cs=1;
	rg_kernel.txDescMask.opts3.bit.tx_portmask=0x3f;
	rg_kernel.txDesc.opts1.bit.ipcs=1;
	rg_kernel.txDesc.opts1.bit.l4cs=1;

	//DEBUG("in %s, the internalVlanId is %d, srcport is %d",__FUNCTION__,internalVlanID,srcPort);
	//Check for internalVLAN contains WAN port or not
	if(pPktHdr==NULL)
		pSourceMac=skb->data+ETHER_ADDR_LEN;		//SA
	else
		pSourceMac=pPktHdr->pSmac;

	for(i=0;i<MAX_NETIF_SW_TABLE_SIZE;i++)
	{
		if(rg_db.systemGlobal.interfaceInfo[i].valid)
		{
			if(rg_db.systemGlobal.interfaceInfo[i].storedInfo.is_wan)
			{
				//WAN interface
				if(memcmp(rg_db.systemGlobal.interfaceInfo[i].storedInfo.wan_intf.wan_intf_conf.gmac.octet,pSourceMac,ETHER_ADDR_LEN)==0)
				{
					isGatewayMac=1;
					break;
				}				
			}
#if 0 //BC packet from LAN should not send to WAN
			else
			{
				//LAN interface
				if(memcmp(rg_db.systemGlobal.interfaceInfo[i].storedInfo.lan_intf.gmac.octet,pSourceMac,ETHER_ADDR_LEN)==0)
				{
					isGatewayMac=1;
					break;
				}
			}
#endif
		}
	}
	//if((skb->data[0]&1)&&(skb->data[0]!=0xff))
	//{
	//	TRACE("Multicast packet don't send to WAN");
	//}
	//else
	{
		for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
		{
			//if((WANMask&(0x1<<(rg_db.systemGlobal.wanIntfGroup[i].index)))>0)
			if(srcPort!=rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx)		//src block
			{
				dpMask=0x1<<rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx;			
				wanPortMask|=dpMask;
				if(((rg_db.algFunctionMask & RTK_RG_ALG_PPPOE_PASSTHROUGH_BIT) > 0) ||	//if pppoe pass through is turn on, all WAN has to be sended
					(srcPort==RTK_RG_MAC_PORT_CPU && isGatewayMac) ||	//if src port is CPU and SA==GMAC, send it
					((rg_db.vlan[internalVlanID].MemberPortmask.bits[0]&dpMask)>0 && rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_type==RTK_RG_BRIDGE))		//otherwise only boardcast to bridge wan							
				{
					TRACE("Broadcast to WAN[%d]!!",rg_db.systemGlobal.wanIntfGroup[i].index);

					//Copy packet, each WAN interface need one packet, therefore need n packets to be created
					//bcSkb = dev_alloc_skb(skb->len);	


					//bcSkb=rtk_rg_skbCopyToPreAllocSkb(skb);
					bcSkb=skb_clone(skb,GFP_ATOMIC);
					if(bcSkb==NULL) goto OUT_OF_MEM;


					//DEBUG("wan port is %d",rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.wan_port_idx);
					rg_kernel.txDesc.opts3.bit.tx_portmask=dpMask;
					
					//decision of VLAN tagging
					_rtk_rg_interfaceVlanIDPriority(&rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo,&rg_kernel.txDesc,&rg_kernel.txDescMask);
					_rtk_rg_wanVlanTagged(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_tag_on);

					//Transfer mirror packet with dedicated VLAN and tagging to each WAN interface
					if(pPktHdr==NULL)
						re8686_send_with_txInfo_and_mask(bcSkb,&rg_kernel.txDesc,0,&rg_kernel.txDescMask);																					
					else
						_rtk_rg_egressPacketSend(bcSkb,pPktHdr);


				}
			}
			/*else
			{
				//handle broadcast packet from WAN
				if((rg_db.vlan[internalVlanID].MemberPortmask.bits[0]&(0x1<<RTK_RG_PORT_CPU))>0)		//contain CPU port, return to PS
				{
					DEBUG("WAN to WAN broadcast, trap to protocol stack...");
					return RG_FWDENGINE_RET_TO_PS;
				}
			}*/
		}
	}
	
	//Lan port can be sended in two packet with txpmsk, one tagged, another untag	
	rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0x3;

	//untag LAN port
	dpMask=rg_db.vlan[internalVlanID].MemberPortmask.bits[0]&rg_db.vlan[internalVlanID].UntagPortmask.bits[0];
	dpMask&=(~(0x1<<srcPort));
	dpMask&=(~(0x1<<RTK_RG_PORT_CPU));		//FIXME:not sending to CPU, but how to WLAN?
	//dpMask&=(~wanPortMask);
	if(dpMask!=0)
	{
		//bcSkb=rtk_rg_skbCopyToPreAllocSkb(skb);
		bcSkb=skb_clone(skb,GFP_ATOMIC);
		if(bcSkb==NULL) goto OUT_OF_MEM;


		//printk("s3 %02x-%02x-%02x-%02x-%02x-%02x ref=%d usr=%d\n",bcSkb->data[0],bcSkb->data[1],bcSkb->data[2],bcSkb->data[3],bcSkb->data[4],bcSkb->data[5], atomic_read(&skb_shinfo(bcSkb)->dataref),atomic_read(&bcSkb->users));
		//memcpy(bcSkb->data,skb->data,skb->len);
		//bcSkb->len=skb->len;
		rg_kernel.txDescMask.opts2.bit.vidl=0x0;
		rg_kernel.txDescMask.opts2.bit.vidh=0x0;
		rg_kernel.txDescMask.opts2.bit.prio=0x0;
		rg_kernel.txDescMask.opts3.bit.tx_portmask=0x3f;;
			
		rg_kernel.txDesc.opts3.bit.tx_portmask=dpMask;
		rg_kernel.txDesc.opts2.bit.tx_vlan_action=0x2;		//removing
		TRACE("Broadcast to untag LAN(VLAN %d) portmask %x",internalVlanID,dpMask);
		//dump_packet(bcSkb->data,bcSkb->len,"broadcast packet");
		if(pPktHdr==NULL)
			re8686_send_with_txInfo_and_mask(bcSkb,&rg_kernel.txDesc,0,&rg_kernel.txDescMask);
		else
			_rtk_rg_egressPacketSend(bcSkb,pPktHdr);



	}

	//tagged LAN port
	dpMask=rg_db.vlan[internalVlanID].MemberPortmask.bits[0]&(~(rg_db.vlan[internalVlanID].UntagPortmask.bits[0]));
	dpMask&=(~(0x1<<srcPort));
	dpMask&=(~(0x1<<RTK_RG_PORT_CPU));		//FIXME:not sending to CPU, but how to WLAN?
	//dpMask&=(~wanPortMask);
	if(dpMask!=0)
	{
		//bcSkb=rtk_rg_skbCopyToPreAllocSkb(skb);
		bcSkb=skb_clone(skb,GFP_ATOMIC);
		if(bcSkb==NULL) goto OUT_OF_MEM;


		//printk("s4 %02x-%02x-%02x-%02x-%02x-%02x ref=%d usr=%d\n",bcSkb->data[0],bcSkb->data[1],bcSkb->data[2],bcSkb->data[3],bcSkb->data[4],bcSkb->data[5], atomic_read(&skb_shinfo(bcSkb)->dataref),atomic_read(&bcSkb->users));
	
		//memcpy(bcSkb->data,skb->data,skb->len);
		//bcSkb->len=skb->len;
		rg_kernel.txDescMask.opts2.bit.vidl=0xff;
		rg_kernel.txDescMask.opts2.bit.vidh=0xf;
		rg_kernel.txDescMask.opts3.bit.tx_portmask=0x3f;;
					
		rg_kernel.txDesc.opts2.bit.vidl=(internalVlanID&0xff);
		rg_kernel.txDesc.opts2.bit.vidh=((internalVlanID&0xf00)>>8);
		rg_kernel.txDesc.opts3.bit.tx_portmask=dpMask;
		rg_kernel.txDesc.opts2.bit.tx_vlan_action=0x3;		//1 FIXME: here use remarking tag, not add tag
	
		//set up priority
		if(rg_db.vlan[internalVlanID].priorityEn==1)
		{
			rg_kernel.txDescMask.opts2.bit.prio=0x7;
			rg_kernel.txDesc.opts2.bit.prio=rg_db.vlan[internalVlanID].priority&0x7;
		}
		TRACE("Broadcast to tagged LAN(VLAN %d) portmask %x",internalVlanID,dpMask);
		if(pPktHdr==NULL)
			re8686_send_with_txInfo_and_mask(bcSkb,&rg_kernel.txDesc,0,&rg_kernel.txDescMask);
		else
			_rtk_rg_egressPacketSend(bcSkb,pPktHdr);



	}


#ifdef CONFIG_RG_WLAN_HWNAT_ACCELERATION
	if((rg_db.vlan[internalVlanID].MemberPortmask.bits[0]&(1<<RTK_RG_PORT_CPU))&&
		(rg_db.vlan[internalVlanID].Ext_portmask.bits[0]&(1<<(RTK_RG_EXT_PORT0-RTK_RG_PORT_CPU))))
	{
		
		//1 FIXME: from Master to Master, this should not be filtered!!
		//if((srcPort!=RTK_RG_PORT_CPU)||((srcPort==RTK_RG_PORT_CPU)&&(extSpa!=RTK_RG_EXT_PORT0))) //from phyiscal port OR ext1,2,3,4
		//{
			int intf_idx;
			//bcSkb=rtk_rg_skbCopyToPreAllocSkb(skb);
			bcSkb=skb_clone(skb,GFP_ATOMIC);
			if(bcSkb==NULL) goto OUT_OF_MEM;
				
			intf_idx=_rtk_master_wlan_mbssid_tx(bcSkb);
			if(intf_idx==FAIL)
			{
				if(bcSkb) dev_kfree_skb_any(bcSkb);
			}
			if(intf_idx==WIFI_FLOOD_INTF_RET)
			{
				TRACE("Broadcast to master WLAN(flooding)");
			}
			else
			{
				TRACE("Broadcast to master WLAN(intf=%d)",intf_idx);
			}
		//}
	}
	
#ifdef CONFIG_DUALBAND_CONCURRENT
// send broadcast to slave wifi
	if((rg_db.vlan[internalVlanID].MemberPortmask.bits[0]&(1<<RTK_RG_PORT_CPU))&&
		(rg_db.vlan[internalVlanID].Ext_portmask.bits[0]&(1<<(RTK_RG_EXT_PORT1-RTK_RG_PORT_CPU))))
	{

		if((srcPort!=RTK_RG_PORT_CPU)||((srcPort==RTK_RG_PORT_CPU)&&(extSpa!=RTK_RG_EXT_PORT1))) //from phyiscal port OR ext0,2,3,4
		{
			// WLAN port (tag & untag)
			//bcSkb=rtk_rg_skbCopyToPreAllocSkb(skb);
			bcSkb=skb_clone(skb,GFP_ATOMIC);
			if(bcSkb==NULL) goto OUT_OF_MEM;
	
			rg_kernel.txDescMask.opts1.bit.dislrn=1;
			rg_kernel.txDescMask.opts2.bit.vidl=0xff;
			rg_kernel.txDescMask.opts2.bit.vidh=0xf;
			rg_kernel.txDescMask.opts2.bit.prio=0x7;
			rg_kernel.txDescMask.opts3.bit.tx_portmask=0x3f; //no cpu port
			rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0;// no-action
			
			rg_kernel.txDesc.opts1.bit.dislrn=1; // patch for reason 192
			rg_kernel.txDesc.opts2.bit.vidl=CONFIG_DEFAULT_TO_SLAVE_GMAC_VID&0xff;
			rg_kernel.txDesc.opts2.bit.vidh=CONFIG_DEFAULT_TO_SLAVE_GMAC_VID>>8;
			rg_kernel.txDesc.opts2.bit.prio=CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI;		
			rg_kernel.txDesc.opts3.bit.tx_portmask=0; //HWLOOKUP (because: HW do not have extension port & CPU port bit)
			//rg_kernel.txDesc.opts2.bit.tx_vlan_action=0;// no-action
			rg_kernel.txDesc.opts2.bit.tx_vlan_action=3;// remarking

			//txinfo_debug(&rg_kernel.txDesc);
			//memDump(bcSkb->data,bcSkb->len,"BC-to-WIFI2");
			
			TRACE("Broadcast to slave WLAN by GMAC(VID=%d,PRI=%d,HWLOOKUP)",CONFIG_DEFAULT_TO_SLAVE_GMAC_VID,CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI);
			if(pPktHdr==NULL)
				re8686_send_with_txInfo_and_mask(bcSkb,&rg_kernel.txDesc,0,&rg_kernel.txDescMask);
			else
				_rtk_rg_egressPacketSend(bcSkb,pPktHdr);

		}
	}
#endif
#endif
	

	//Check VLAN contain CPU port or not
	dpMask=rg_db.vlan[internalVlanID].MemberPortmask.bits[0]&(~(0x1<<srcPort));
	if(((dpMask&(0x1<<RTK_RG_PORT_CPU))>0)||(extSpa>RTK_RG_PORT_CPU)) //from EXT Port shall not be src block.
	{
		//the original packet continue to protocol stack
		//DEBUG("the original packet continue to protocol stack!");
		if(cpSkb) dev_kfree_skb_any(cpSkb);
		return RG_FWDENGINE_RET_TO_PS;
	}
	else
	{
		//dev_kfree_skb_any(skb);
		DEBUG("the original packet stop here...internalVlanID=%d dpMask=%x",internalVlanID,dpMask);
		if(cpSkb) dev_kfree_skb_any(cpSkb);
		return RG_FWDENGINE_RET_DROP;
	}

	if(bcSkb) dev_kfree_skb_any(bcSkb);
	if(cpSkb) dev_kfree_skb_any(cpSkb);
	return RG_FWDENGINE_RET_DROP;

OUT_OF_MEM:

#if RTK_RG_SKB_PREALLOCATE	
	FIXME("Out of pre-alloc memory(%s:%d)\n",__FUNCTION__,__LINE__);
#else
	FIXME("Out of memory(%s:%d)\n",__FUNCTION__,__LINE__);
#endif
	if(bcSkb) dev_kfree_skb_any(bcSkb);
	if(cpSkb) dev_kfree_skb_any(cpSkb);
	return RG_FWDENGINE_RET_DROP;
	
	//return RG_FWDENGINE_RET_DIRECT_TX;
}


#endif

int _rtk_rg_layer2GarbageCollection(int l2Idx)
{
	int search_idx,count=0,ret,i,invalidNum=0,smallestIdx=4;
	rtk_l2_addr_table_t l2Entry;
	DEBUG("_rtk_rg_layer2GarbageCollection, l2Idx is %d",l2Idx);
	//Compare from l2Idx, if the entry in software table is not in hardware table anymore, the entry will be used for new entry
	do
	{
		search_idx=l2Idx+count;
		ret=rtk_l2_nextValidEntry_get(&search_idx,&l2Entry);
		if(ret!=RT_ERR_OK)return 4;

		if(search_idx>l2Idx+4 || search_idx<l2Idx)	//no valid entry in this 4-way hased address
		{
			//reset lasting software LUT table to invalid
			invalidNum=4-count;
			DEBUG("the index after %d is invalid in hw table, so invalid the lasting %d software entries!!",l2Idx+count,invalidNum);
			
			//------------------ Critical Section start -----------------------//
			//rg_lock(&rg_kernel.saLearningLimitLock);
			if(rg_db.lut[l2Idx+count].rtk_lut.entry.l2UcEntry.port>=6)
				atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[l2Idx+count].rtk_lut.entry.l2UcEntry.ext_port+RTK_RG_PORT_CPU]);
			else
				atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[l2Idx+count].rtk_lut.entry.l2UcEntry.port]);
			//------------------ Critical Section End -----------------------//
			//rg_unlock(&rg_kernel.saLearningLimitLock);
			
			memset(&rg_db.lut[l2Idx+count],0,(sizeof(rtk_rg_table_lut_t)*invalidNum));
			if(smallestIdx>count)
				smallestIdx=count;
			break;
		}

		invalidNum=search_idx-(l2Idx+count);		//how many invalid entries between start_idx and return_idx
		for(i=0;i<invalidNum;i++)
		{
			if(smallestIdx>(count+i))
				smallestIdx=count+i;
			DEBUG("invalid the index %d in software table",l2Idx+count);
			
			//------------------ Critical Section start -----------------------//
			//rg_lock(&rg_kernel.saLearningLimitLock);
			if(rg_db.lut[l2Idx+count].rtk_lut.entry.l2UcEntry.port>=RTK_RG_PORT_CPU)
				atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[l2Idx+count].rtk_lut.entry.l2UcEntry.ext_port+RTK_RG_PORT_CPU]);
			else
				atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[l2Idx+count].rtk_lut.entry.l2UcEntry.port]);
			//------------------ Critical Section End -----------------------//
			//rg_unlock(&rg_kernel.saLearningLimitLock);
			
			memset(&rg_db.lut[l2Idx+count],0,sizeof(rtk_rg_table_lut_t));		//clean invalid entries
		}

		count+=(invalidNum+1);
	}while(count<4);
	
	DEBUG("the return of smallestIdx is %d",smallestIdx);
	return smallestIdx;
}

int _rtk_rg_layer2LeastRecentlyUsedReplace(int l2Idx)
{
	//check each LUT entry which hashed to same index, if the age is smallest, the entry is least recently used one
	//we choose the LRU entry to be replace for the new entry;if the age are all the same, we choose the biggest index,
	//because the smallest one always means it is first add into the table, since they have the same age value, it indicate
	//that first added one has traffic as well as the last added one, therefore we choose the last added one.
	int search_idx,LRU_age=8,LRU_index,ret;
	rtk_l2_ucastAddr_t *pL2Addr;

	search_idx=l2Idx+3;
	LRU_index=search_idx;
	do
	{
		if(rg_db.lut[search_idx].valid==1 && 
			rg_db.lut[search_idx].rtk_lut.entryType==RTK_LUT_L2UC &&
			(!(rg_db.lut[search_idx].rtk_lut.entry.l2UcEntry.flags & (RTK_L2_UCAST_FLAG_STATIC|RTK_L2_UCAST_FLAG_ARP_USED))) &&
			rg_db.lut[search_idx].rtk_lut.entry.l2UcEntry.age<LRU_age)
		{
			LRU_age=rg_db.lut[search_idx].rtk_lut.entry.l2UcEntry.age;
			LRU_index=search_idx;
		}
		else if(rg_db.lut[search_idx].valid==0)
		{
			return LRU_index;
		}		
		search_idx--;
	}while(search_idx>=l2Idx);


	if(rg_db.lut[LRU_index].valid==1 && 
		rg_db.lut[LRU_index].rtk_lut.entryType==RTK_LUT_L2UC &&
		(!(rg_db.lut[LRU_index].rtk_lut.entry.l2UcEntry.flags & (RTK_L2_UCAST_FLAG_STATIC|RTK_L2_UCAST_FLAG_ARP_USED))))
	{
		//invalid the LRU entry, otherwise the new entry won't add
		pL2Addr=&rg_db.lut[LRU_index].rtk_lut.entry.l2UcEntry;
		ret=RTK_L2_ADDR_DEL(pL2Addr);
		if(ret==RT_ERR_OK)
		{
			//------------------ Critical Section start -----------------------//
			//rg_lock(&rg_kernel.saLearningLimitLock);
			if(rg_db.lut[pL2Addr->index].rtk_lut.entry.l2UcEntry.port>=RTK_RG_PORT_CPU)
				atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[pL2Addr->index].rtk_lut.entry.l2UcEntry.ext_port+RTK_RG_PORT_CPU]);
			else
				atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[pL2Addr->index].rtk_lut.entry.l2UcEntry.port]);
			//------------------ Critical Section End -----------------------//
			//rg_unlock(&rg_kernel.saLearningLimitLock);
			memset(&rg_db.lut[pL2Addr->index],0,sizeof(rtk_rg_table_lut_t));
		}

		return LRU_index;
	}

	return -1;
}

#ifdef CONFIG_RTL8686NIC
void _rtk_rg_interfaceVlanIDPriority(rtk_rg_intfInfo_t *pStoredInfo,struct tx_info *ptxInfo,struct tx_info *ptxInfoMask)
#else
void _rtk_rg_interfaceVlanIDPriority(rtk_rg_intfInfo_t *pStoredInfo,rtk_rg_txdesc_t *ptxInfo,rtk_rg_txdesc_t *ptxInfoMask)
#endif
{
	//DEBUG("%s",__FUNCTION__);
	//Here we just decide VLANID and priority, tag or untag will postpone to _rtk_rg_fwdEngineDMAC2CVIDTransfer
	if(pStoredInfo->is_wan==0)
	{
		//FIXME: here should consider both LAN and WAN interface
		ptxInfoMask->opts2.bit.tx_vlan_action=0x3;
		ptxInfoMask->opts2.bit.vidl=0xff;
		ptxInfoMask->opts2.bit.vidh=0xf;
		ptxInfoMask->opts2.bit.prio=0x7;
		
		ptxInfo->opts2.bit.tx_vlan_action = 0x2;		//removing
		ptxInfo->opts2.bit.vidl=(pStoredInfo->lan_intf.intf_vlan_id&0xff);
		ptxInfo->opts2.bit.vidh=((pStoredInfo->lan_intf.intf_vlan_id&0xf00)>>8);

		/*if(rg_db.vlan[pStoredInfo->lan_intf.intf_vlan_id].UntagPortmask&)
		{
			ptxInfo->opts2.bit.tx_vlan_action = 0x3;		//remarking tag
			DEBUG("tagged with %d",pStoredInfo->wan_intf.wan_intf_conf.egress_vlan_id);
		}*/

		//set up priority
		if(rg_db.vlan[pStoredInfo->lan_intf.intf_vlan_id].priorityEn)
			ptxInfo->opts2.bit.prio=rg_db.vlan[pStoredInfo->lan_intf.intf_vlan_id].priority&0x7;
		else
			ptxInfo->opts2.bit.prio=0;
	}
	else
	{
		ptxInfoMask->opts2.bit.tx_vlan_action=0x3;
		ptxInfoMask->opts2.bit.vidl=0xff;
		ptxInfoMask->opts2.bit.vidh=0xf;
		ptxInfoMask->opts2.bit.prio=0x7;
		
		ptxInfo->opts2.bit.tx_vlan_action = 0x2;		//removing
		ptxInfo->opts2.bit.vidl=(pStoredInfo->wan_intf.wan_intf_conf.egress_vlan_id&0xff);
		ptxInfo->opts2.bit.vidh=((pStoredInfo->wan_intf.wan_intf_conf.egress_vlan_id&0xf00)>>8);

		//DEBUG("wanVID is%d, the vidl is %x, vidh is %x",pStoredInfo->wan_intf.wan_intf_conf.egress_vlan_id,ptxInfo->opts2.bit.vidl,ptxInfo->opts2.bit.vidh);
		/*if(pStoredInfo->wan_intf.wan_intf_conf.egress_vlan_tag_on)
		{
			ptxInfo->opts2.bit.tx_vlan_action = 0x3;		//remarking tag
			DEBUG("tagged with %d",pStoredInfo->wan_intf.wan_intf_conf.egress_vlan_id);
		}*/

		//set up priority
		if(rg_db.vlan[pStoredInfo->wan_intf.wan_intf_conf.egress_vlan_id].priorityEn)
			ptxInfo->opts2.bit.prio=pStoredInfo->wan_intf.wan_intf_conf.egress_vlan_pri&0x7;
		else
			ptxInfo->opts2.bit.prio=0;
	}
}

void _rtk_rg_wanVlanTagged(int vlan_tag_on)
{
	int VLANId;
	VLANId=rg_kernel.txDesc.opts2.bit.vidh;
	VLANId<<=8;
	VLANId|=rg_kernel.txDesc.opts2.bit.vidl;
	//DEBUG("the VLANId decide in _rtk_rg_interfaceVlanTagged is %d",VLANId);
	if(rg_db.vlan[VLANId].fidMode==VLAN_FID_IVL)
	{
		//tag/untag by VLAN untag setting
		if(rg_db.vlan[VLANId].UntagPortmask.bits[0]&rg_kernel.txDesc.opts3.bit.tx_portmask)
		{
			//DEBUG("IVL:untagged!");
			rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0x3;
			rg_kernel.txDesc.opts2.bit.tx_vlan_action = 0x2;		//removing
		}
		else
		{
			//DEBUG("IVL:tagged!");
			rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0x3;
			rg_kernel.txDesc.opts2.bit.tx_vlan_action = 0x3;		//remarking tag
		}
	}
	else
	{		
		//Vlan tagged or not by interface setting
		rg_kernel.txDescMask.opts2.bit.tx_vlan_action=0x3;
		if(vlan_tag_on)
			rg_kernel.txDesc.opts2.bit.tx_vlan_action=0x3;		//remarking tag
		else
			rg_kernel.txDesc.opts2.bit.tx_vlan_action=0x2;		//removing tag
	}
}

int _rtk_rg_arpGeneration(rtk_rg_intfInfo_t *pStoredInfo,ipaddr_t gwIpAddr,rtk_rg_arp_request_t *arpReq)
{
#ifdef __KERNEL__
	//struct tx_info txInfo,txInfoMask;
	struct sk_buff *skb;
	unsigned char *bufptr;
	int ret;
	DEBUG("arp send: request ip=%x\n",arpReq->reqIp);
	skb=dev_alloc_skb(RG_FWDENGINE_PKT_LEN);	
	skb_reserve(skb, RX_OFFSET);
	bufptr=skb->data;

	/* Construct destination MAC */
	memset(bufptr,0xff, ETHER_ADDR_LEN);

	/* Construct source MAC */
	memcpy(bufptr + 6,(int8 *)&pStoredInfo->wan_intf.wan_intf_conf.gmac.octet[0],ETHER_ADDR_LEN);

	/* construct Ethtype+ARP header */
	*(uint32 *)(bufptr + 12) = htonl(0x08060001);
	*(uint32 *)(bufptr + 16) = htonl(0x08000604);
	*(uint16 *)(bufptr + 20) = htons(0x0001);
	memcpy(bufptr + 22,(int8 *)&pStoredInfo->wan_intf.wan_intf_conf.gmac.octet[0],ETHER_ADDR_LEN);

	*(uint8 *)(bufptr + 28)=(uint8)((gwIpAddr>>24)&0xff);
	*(uint8 *)(bufptr + 29)=(uint8)((gwIpAddr>>16)&0xff);
	*(uint8 *)(bufptr + 30)=(uint8)((gwIpAddr>>8)&0xff);
	*(uint8 *)(bufptr + 31)=(uint8)((gwIpAddr)&0xff);	
		
	*(uint32 *)(bufptr + 28) = htonl(gwIpAddr);	
	*(uint32 *)(bufptr + 38) = htonl(arpReq->reqIp);

	skb->len = 60;

	/*memset(&txInfo,0,sizeof(txInfo));
	memset(&txInfoMask,0,sizeof(txInfoMask));

	//arp request with vlan tag
	_rtk_rg_interfaceVlanTagged(pStoredInfo,&txInfo,&txInfoMask);

	//FIXME: lookup by vlan table

	txInfoMask.opts2.bit.cputag=1;
	txInfo.opts2.bit.cputag=1;

	txInfoMask.opts3.bit.l34_keep=1;
	txInfoMask.opts3.bit.tx_portmask=0x3f;
#ifdef CONFIG_APOLLO_RLE0371
	//patch bug for: ARP tx data error, when directTX
	txInfo.opts3.bit.l34_keep=0;
	txInfo.opts3.bit.tx_portmask=0;
#else
	txInfo.opts3.bit.l34_keep=1;	
	txInfo.opts3.bit.tx_portmask=0;		//let hardware to auto look up
	//txInfo.opts3.bit.tx_portmask=(1<<RTK_RG_MAC_PORT0)|(1<<RTK_RG_MAC_PORT_RGMII);
#endif*/	
	*(u32*)(skb->data+skb->len)=0; //save null point into end of skb data.(for trace filter debug)

	ret=_rtk_rg_broadcastForward(skb,pStoredInfo->wan_intf.wan_intf_conf.egress_vlan_id,RTK_RG_MAC_PORT_CPU,0);
	if(ret==RG_FWDENGINE_RET_DROP)
		dev_kfree_skb_any(skb);

	//memDump(bufptr,skb->len,"ARPGEN");	
	//rtk_rg_fwdEngine_xmit(skb,&txInfo,NULL);
	//re8686_send_with_txInfo(skb,&txInfo,0);
	//re8686_send_with_txInfo_and_mask(skb,&txInfo,0,&txInfoMask);
#else
	rtk_rg_txdesc_t txInfo,txInfoMask;

	//FIXME:in module code, we need some other api to send packets
#endif
	

	return 0;
}

unsigned short _rtk_rg_checkSumICMPv6(unsigned short *sip,unsigned short *dip,unsigned short payloadLength,unsigned short nextHeader,unsigned short *buffer, int size)
{
    unsigned long cksum=0;
	int ipSize;
    while(size >1)
    {
        cksum+=*buffer++;
        size -=sizeof(unsigned short);
    }
    if(size)
        cksum += *(unsigned char*)buffer;

	//caculate IPv6 pseudo header
	ipSize=IPV6_ADDR_LEN;	//bytes
	while(ipSize >1)
    {
        cksum+=*sip++;
        ipSize -=sizeof(unsigned short);
    }
    if(ipSize)
        cksum += *(unsigned char*)sip;
	
	ipSize=IPV6_ADDR_LEN;	//bytes
	while(ipSize >1)
    {
        cksum+=*dip++;
        ipSize -=sizeof(unsigned short);
    }
    if(ipSize)
        cksum += *(unsigned char*)dip;

	cksum+=payloadLength;
	cksum+=nextHeader;

    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
	
    return (unsigned short)(~cksum);
}

int _rtk_rg_NDGeneration(rtk_rg_intfInfo_t *pStoredInfo,rtk_ipv6_addr_t gwIpAddr,rtk_rg_neighbor_discovery_t *neighborDisc)
{

#ifdef __KERNEL__
	//struct tx_info txInfo,txInfoMask;
	unsigned short tmpChksum;
	int ret;
	
	struct sk_buff *skb;
	unsigned char *bufptr;
	DEBUG("neighbor send: request ip=%08x:%08x:%08x:%08x\n",*(unsigned int *)neighborDisc->reqIp.ipv6_addr,
		*(unsigned int *)(neighborDisc->reqIp.ipv6_addr+4),
		*(unsigned int *)(neighborDisc->reqIp.ipv6_addr+8),
		*(unsigned int *)(neighborDisc->reqIp.ipv6_addr+12));
	
	skb=dev_alloc_skb(RG_FWDENGINE_PKT_LEN);	
	skb_reserve(skb, RX_OFFSET);
	bufptr=skb->data;

	// Construct destination MAC: 
	//33:33:ff plus IPv6 address last 24bits
	*(unsigned char *)(bufptr)=0x33;
	*(unsigned char *)(bufptr+1)=0x33;
	*(unsigned char *)(bufptr+2)=0xff;
	memcpy(bufptr + 3,&neighborDisc->reqIp.ipv6_addr[13],3);

	// Construct source MAC
	memcpy(bufptr + 6,&pStoredInfo->wan_intf.wan_intf_conf.gmac.octet[0],ETHER_ADDR_LEN);

	// construct IPv6 header
	*(unsigned int *)(bufptr + 12) = htonl(0x86dd6000);			//etherType, ipv6 version
	*(unsigned int *)(bufptr + 16) = htonl(0x00000020);			//payload length: 32bytes
	*(unsigned short *)(bufptr + 20) = htons(0x3aff);			//next header: ICMPv6(58), Hop limit: 255
	memcpy(bufptr + 22,&gwIpAddr.ipv6_addr,16);					//source IP
	*(unsigned int *)(bufptr + 38) = htonl(0xff020000);			//Destination IP: ff02::1:ff00/104 + ipv6 last 24bits
	*(unsigned int *)(bufptr + 42) = htonl(0x00000000);			//Destination IP: ff02::1:ff00/104 + ipv6 last 24bits
	*(unsigned int *)(bufptr + 46) = htonl(0x00000001);			//Destination IP: ff02::1:ff00/104 + ipv6 last 24bits
	*(unsigned char *)(bufptr+50)=0xff;							//Destination IP: ff02::1:ff00/104 + ipv6 last 24bits
	memcpy(bufptr + 51,&neighborDisc->reqIp.ipv6_addr[13],3);	//Destination IP: ff02::1:ff00/104 + ipv6 last 24bits

	// construct ICMPv6 for Neighbor Solicitation
	*(unsigned int *)(bufptr + 54) = htonl(0x87000000);		//type:neighbor solicitation(135), code:0, checksum: 0 at first
	*(unsigned int *)(bufptr + 58) = htonl(0x00000000);		//reserved:0
	memcpy(bufptr + 62,&neighborDisc->reqIp,16);			//Destination IP
	*(unsigned short *)(bufptr + 78) = htons(0x0101);		//ICMPv6 optional:type=1,source link-layer address, length=1(8 bytes)
	memcpy(bufptr + 80,&pStoredInfo->wan_intf.wan_intf_conf.gmac.octet[0],ETHER_ADDR_LEN);

	// Caculate checksum
	tmpChksum=_rtk_rg_checkSumICMPv6((unsigned short *)(bufptr+22),(unsigned short *)(bufptr+38),0x0020,0x003a,(unsigned short *)(bufptr+54),32);
	*(unsigned short *)(bufptr + 56) = htons(tmpChksum);

	skb->len = 86;	//plus CRC length or not?

	/*memset(&txInfo,0,sizeof(txInfo));
	memset(&txInfoMask,0,sizeof(txInfoMask));

	//neighbor discovery with vlan tag
	_rtk_rg_interfaceVlanTagged(pStoredInfo,&txInfo,&txInfoMask);

	txInfoMask.opts2.bit.cputag=1;
	txInfoMask.opts3.bit.l34_keep=1;
	txInfoMask.opts3.bit.tx_portmask=0x3f;

	txInfo.opts2.bit.cputag=1;
#ifdef CONFIG_APOLLO_RLE0371
	txInfo.opts3.bit.l34_keep=0;
	//patch bug for: ARP tx data error, when directTX
	txInfo.opts3.bit.tx_portmask=0;
#else
	txInfoMask.opts1.bit.ipcs=1;
	txInfoMask.opts1.bit.l4cs=1;	
	txInfoMask.opts1.bit.cputag_ipcs=1;
	txInfoMask.opts1.bit.cputag_l4cs=1;

	txInfo.opts3.bit.l34_keep=1;
	txInfo.opts3.bit.tx_portmask=0x1<<pStoredInfo->wan_intf.wan_intf_conf.wan_port_idx;		//since we are multicast packet, we should use DirectTX
	//txInfo.opts3.bit.tx_portmask=(1<<RTK_RG_MAC_PORT0)|(1<<RTK_RG_MAC_PORT_RGMII);
	txInfo.opts1.bit.ipcs=1;
	txInfo.opts1.bit.l4cs=1;
	txInfo.opts1.bit.cputag_ipcs=1;
	txInfo.opts1.bit.cputag_l4cs=1;
#endif	*/

	//memDump(bufptr,skb->len,"ARPGEN");	
	//rtk_rg_fwdEngine_xmit(skb,&txInfo,NULL);
	//re8686_send_with_txInfo(skb,&txInfo,0);
	//re8686_send_with_txInfo_and_mask(skb,&txInfo,0,&txInfoMask);
	*(u32*)(skb->data+skb->len)=0; //save null point into end of skb data.(for trace filter debug)
	ret=_rtk_rg_broadcastForward(skb,pStoredInfo->wan_intf.wan_intf_conf.egress_vlan_id,RTK_RG_MAC_PORT_CPU,0);
	if(ret==RG_FWDENGINE_RET_DROP)
		dev_kfree_skb_any(skb);
#else
	rtk_rg_txdesc_t txInfo,txInfoMask;

	//FIXME:in module code, we need some other api to send packets
#endif
	

	return 0;
}


int _rtk_rg_internal_GWMACSetup_stage2(int matchIdx, int l2Idx)
{
	int nxtidx,ret,errorno;
	int ori_l2Idx=rg_db.systemGlobal.defaultTrapLUTIdx;
	int napt_enable=0, default_route=0;
	rtk_l34_routing_entry_t rtEntry;
    rtk_l34_ext_intip_entry_t extipEt;
    rtk_l34_nexthop_entry_t nxpEt;
	//rtk_l34_pppoe_entry_t pppoeEt;
    ipaddr_t wan_ext_ip=0;
	rtk_rg_ipv4RoutingEntry_t cb_routEt;
	
	rtk_rg_macEntry_t macEt;
	//int valid_macIdx;
	//int ori_wantype=0;
	//rtk_wanType_entry_t wantEt;

	wan_ext_ip=rg_db.systemGlobal.interfaceInfo[matchIdx].p_wanStaticInfo->ip_addr;
	napt_enable=rg_db.systemGlobal.interfaceInfo[matchIdx].p_wanStaticInfo->napt_enable;
	default_route=rg_db.systemGlobal.interfaceInfo[matchIdx].p_wanStaticInfo->ipv4_default_gateway_on;
	
	//nxtidx=RG_GLB_WAN_TYPE[matchIdx];
	nxtidx=rg_db.wantype[matchIdx].rtk_wantype.nhIdx;
	//DEBUG("the wan_ip is %x, napt_enable is %d, default_route is %d  nxtidx is %d...",wan_ext_ip,napt_enable,default_route,nxtidx);
//#endif
	//Check if we had set nexthop before
	if(rg_db.nexthop[nxtidx].rtk_nexthop.nhIdx!=l2Idx)
	{
		//errorno=RT_ERR_RG_NXP_GET_FAIL;
		bzero(&nxpEt, sizeof(rtk_l34_nexthop_entry_t));
		memcpy(&nxpEt, &rg_db.nexthop[nxtidx].rtk_nexthop,sizeof(rtk_l34_nexthop_entry_t));
		//ret = rtk_l34_nexthopTable_get(nxtidx, &nxpEt);
		//if(ret!=RT_ERR_OK)goto RET_ERR;

	    //Check for routing table
	    //errorno=RT_ERR_RG_ROUTE_GET_FAIL;
	    //ret = rtk_l34_routingTable_get(7, &rtEntry);	//get default route setting
	    //if(ret!=RT_ERR_OK)goto RET_ERR;
		//errorno=RT_ERR_RG_DEF_ROUTE_EXIST;

		//Setup Nexthop table in nxtidx
	    errorno=RT_ERR_RG_NXP_SET_FAIL;
		ori_l2Idx = nxpEt.nhIdx;	//Keep
	    nxpEt.nhIdx = l2Idx;			// TODO:LUT table index point to Gateway

	    ret = RTK_L34_NEXTHOPTABLE_SET(nxtidx, &nxpEt);
	    if(ret!=RT_ERR_OK)goto RET_NEXTHOP_ERR;
	}

    //Check and set up Internal External IP table for NAPT
    if(napt_enable == 1 && rg_db.extip[matchIdx].rtk_extip.valid == 0)
    {
		//Since EIP table is 1-on-1 mapping with netif table, we don't need to loop
        //extipIdx = matchIdx;		//Keep

        errorno=RT_ERR_RG_EXTIP_SET_FAIL;
        extipEt.intIpAddr=0;		//napt special
        extipEt.extIpAddr=wan_ext_ip;
        extipEt.nhIdx=nxtidx;
        extipEt.prival=0;
        extipEt.pri=0;
        extipEt.type=L34_EXTIP_TYPE_NAPT;
        extipEt.valid=1;

        ret = RTK_L34_EXTINTIPTABLE_SET(matchIdx, &extipEt);
        if(ret!=RT_ERR_OK)goto RET_EXTIP_ERR;

		rg_db.systemGlobal.nxpRefCount[nxtidx]++;			//nexthop reference by IP table

//#ifndef CONFIG_APOLLO_RLE0371
#if 0
		//Modified the wan type to NAPT	   
        errorno=RT_ERR_RG_WANTYPE_SET_FAIL;
        bzero(&wantEt, sizeof(rtk_wanType_entry_t));
		memcpy(&wantEt,&rg_db.wantype[matchIdx].rtk_wantype,sizeof(rtk_wanType_entry_t));
		//ret = rtk_l34_wanTypeTable_get(matchIdx, &wantEt);
		//if(ret!=RT_ERR_OK)goto RET_WAN_TYPE_ERR;

		ori_wantype = wantEt.wanType;
        wantEt.wanType = L34_WAN_TYPE_L34NAT_ROUTE;

        ret = RTK_L34_WANTYPETABLE_SET(matchIdx, &wantEt);
        if(ret==RT_ERR_CHIP_NOT_SUPPORTED)
		{
			errorno=RT_ERR_RG_CHIP_NOT_SUPPORT;
			goto RET_WAN_TYPE_ERR; 
		}
        if(ret!=RT_ERR_OK)goto RET_WAN_TYPE_ERR;
#endif
//#endif
    }

	//Check and setup Routing table for default route
    if(default_route == 1 && rg_db.l3[7].rtk_l3.process == L34_PROCESS_CPU)
    {
	    errorno=RT_ERR_RG_ROUTE_SET_FAIL;
		bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
	    rtEntry.valid=1;
	    rtEntry.process=L34_PROCESS_NH;
		if(napt_enable == 1)
	    	rtEntry.internal=0;		//external host from outside
	    else
			rtEntry.internal=1;		//pure routing
	    rtEntry.ipAddr=0;
	    rtEntry.ipMask=0;
	    // TODO:if load-balance is needed, here should be changed
	    rtEntry.nhStart=nxtidx; /*exact index*/
		rtEntry.nhNxt=nxtidx;
	    rtEntry.nhNum=1;		//exect Next hop number 1,2,4,8,16
	    rtEntry.nhAlgo=0x2;		//PER-SIP
	    rtEntry.ipDomain=6;		//Entry 0~7
	    rtEntry.rt2waninf=1;
		
	    ret = RTK_L34_ROUTINGTABLE_SET(7, &rtEntry);		//set default route
	    if(ret!=RT_ERR_OK)goto RET_DEF_ROUTE_ERR;

		// TODO:Call the initParam's routngAddByHwCallBack
		if(rg_db.systemGlobal.initParam.routingAddByHwCallBack != NULL)
		{
			cb_routEt.dest_ip=0;
			cb_routEt.ip_mask=0;
			cb_routEt.nexthop=nxtidx;
			cb_routEt.wan_intf_idx=matchIdx;
			rg_db.systemGlobal.initParam.routingAddByHwCallBack(&cb_routEt);
		}
		
		rg_db.systemGlobal.nxpRefCount[nxtidx]++;		//nexthop reference by routing table
	}

    //store information in Global variable
	memcpy(&rg_db.systemGlobal.interfaceInfo[matchIdx].storedInfo.wan_intf.next_hop_mac, &macEt.mac, sizeof(rtk_mac_t));
    
    return RT_ERR_RG_OK;

//#ifndef CONFIG_APOLLO_RLE0371
#if 0
RET_WAN_TYPE_ERR:
    //Delete binding related WAN Type table entry
    if(napt_enable)
    {
	    wantEt.wanType=ori_wantype;
	    RTK_L34_WANTYPETABLE_SET(matchIdx, &wantEt);
    }
//#endif
#endif
RET_DEF_ROUTE_ERR:
    //Delete the default route entry
    bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
    RTK_L34_ROUTINGTABLE_SET(7, &rtEntry);

RET_EXTIP_ERR:
    //Delete the IP table entry
    if(napt_enable)
    {
        bzero(&extipEt, sizeof(rtk_l34_ext_intip_entry_t));
        RTK_L34_EXTINTIPTABLE_SET(matchIdx, &extipEt);
    }
/*RET_PPPOE_ERR:
	//Delete the pppoe entry
	if(pppoeID!=0)
	{
		bzero(&pppoeEt, sizeof(rtk_l34_pppoe_entry_t));
		RTK_L34_PPPOETABLE_SET(matchIdx, &pppoeEt);
	}*/
RET_NEXTHOP_ERR:
	//Recover original L2 idx
	nxpEt.nhIdx=ori_l2Idx;
	RTK_L34_NEXTHOPTABLE_SET(nxtidx, &nxpEt);
	
	return errorno;
}

int _rtk_rg_internal_GWMACSetup(ipaddr_t ipAddr, int l2Idx)
{
    int i,matchIdx=-1,/*tmpIdx,nxtidx,ret,napt_enable=0,*/errorno;
	//int pppoeID=-1;//,ori_l2Idx=rg_db.systemGlobal.defaultTrapLUTIdx;
	//rtk_l34_routing_entry_t rtEntry;
    //rtk_l34_ext_intip_entry_t extipEt;
    //rtk_l34_nexthop_entry_t nxpEt;
	//rtk_l34_pppoe_entry_t pppoeEt;
    //ipaddr_t wan_ext_ip=0;
	//rtk_rg_ipv4RoutingEntry_t cb_routEt;
	rtk_rg_macEntry_t macEt;
	int valid_macIdx;
	//int ori_wantype=0;
	//rtk_wanType_entry_t wantEt;

    // TODO:After the Gateway mac is learned, we can finish add routing entry
    // TODO:and modify nexthop entry to correct LUT index
	
    //Check l2Idx for success or timeout
    errorno=RT_ERR_RG_ARP_NOT_FOUND;
    if(l2Idx == -1)goto RET_ERR;

	//Get Mac address
	valid_macIdx=l2Idx;
	errorno=rtk_rg_macEntry_find(&macEt, &valid_macIdx);
	if(errorno!=RT_ERR_RG_OK || valid_macIdx!=l2Idx)goto RET_ERR;
	
    //Check each wan interface for matching IPaddr
    for(i=0; i<rg_db.systemGlobal.wanIntfTotalNum; i++)
    {
    	//Bridge WAN won't be compared with
		if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_type==RTK_RG_BRIDGE)
			continue;
		
		if(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->p_wanStaticInfo->gateway_ipv4_addr == ipAddr)
		{			
			matchIdx=rg_db.systemGlobal.wanIntfGroup[i].index;
			
			//DEBUG("before GWMACsetup_stage 2 !!! ip is %x matchidx is %d, l2idx is %d",ipAddr,matchIdx,l2Idx);
			errorno = _rtk_rg_internal_GWMACSetup_stage2(matchIdx, l2Idx);
			if(errorno!=RT_ERR_RG_OK)goto RET_ERR;
			
			rg_db.systemGlobal.intfArpRequest[matchIdx].finished = 1;		
		}
    }
	errorno=RT_ERR_RG_INVALID_PARAM;
    if(matchIdx == -1)goto RET_ERR;

	return RT_ERR_RG_OK;
	
RET_ERR:
	rg_db.systemGlobal.intfArpRequest[matchIdx].finished = -1;
	
    return errorno;
}

int _rtk_rg_internal_IPV6GWMACSetup_stage2(int matchIdx, int l2Idx)
{
	int nxtidx,ret,errorno;
	int ori_l2Idx=rg_db.systemGlobal.defaultTrapLUTIdx;
	int default_route=0;
	rtk_ipv6Routing_entry_t rtv6Entry;
    rtk_l34_nexthop_entry_t nxpEt;
	//rtk_l34_pppoe_entry_t pppoeEt;
    ipaddr_t wan_ext_ip=0;
	rtk_rg_ipv6RoutingEntry_t cb_routv6Et;
	rtk_rg_macEntry_t macEt;
	//int valid_macIdx;
	//int ori_wantype=0;
	//rtk_wanType_entry_t wantEt;

	wan_ext_ip=rg_db.systemGlobal.interfaceInfo[matchIdx].p_wanStaticInfo->ip_addr;
	default_route=rg_db.systemGlobal.interfaceInfo[matchIdx].p_wanStaticInfo->ipv6_default_gateway_on;
	
	nxtidx=rg_db.wantype[matchIdx].rtk_wantype.nhIdx;

	bzero(&nxpEt, sizeof(rtk_l34_nexthop_entry_t));
	memcpy(&nxpEt, &rg_db.nexthop[nxtidx].rtk_nexthop,sizeof(rtk_l34_nexthop_entry_t));

	//Setup Nexthop table in nxtidx
    errorno=RT_ERR_RG_NXP_SET_FAIL;
	ori_l2Idx=nxpEt.nhIdx;	//Keep
    nxpEt.nhIdx=l2Idx;			// TODO:LUT table index point to Gateway

    ret = RTK_L34_NEXTHOPTABLE_SET(nxtidx, &nxpEt);
    if(ret!=RT_ERR_OK)goto RET_NEXTHOP_ERR;

	//Setup Routing table for default route
    if(default_route == 1)
    {
	    errorno=RT_ERR_RG_ROUTE_SET_FAIL;
		bzero(&rtv6Entry, sizeof(rtk_ipv6Routing_entry_t));
	    rtv6Entry.valid=1;
	    rtv6Entry.type=L34_IPV6_ROUTE_TYPE_GLOBAL;
		rtv6Entry.nhOrIfidIdx=nxtidx;
		rtv6Entry.rt2waninf=1;
		
	    ret=RTK_L34_IPV6ROUTINGTABLE_SET(3, &rtv6Entry);		//set default route
	    if(ret!=RT_ERR_OK)goto RET_DEF_ROUTE_ERR;

		// TODO:Call the initParam's v6RoutingAddByHwCallBack
		if(rg_db.systemGlobal.initParam.v6RoutingAddByHwCallBack != NULL)
		{
			bzero(&cb_routv6Et,sizeof(rtk_rg_ipv6RoutingEntry_t));
			cb_routv6Et.NhOrIntfIdx=nxtidx;
			cb_routv6Et.type=rtv6Entry.type;
			rg_db.systemGlobal.initParam.v6RoutingAddByHwCallBack(&cb_routv6Et);
		}
		
		rg_db.systemGlobal.nxpRefCount[nxtidx]++;		//nexthop reference by routing table
	}
	
    //store information in Global variable
	memcpy(&rg_db.systemGlobal.interfaceInfo[matchIdx].storedInfo.wan_intf.next_hop_mac, &macEt.mac, sizeof(rtk_mac_t));
    
    return RT_ERR_RG_OK;

RET_DEF_ROUTE_ERR:
    //Delete the default route entry
    bzero(&rtv6Entry, sizeof(rtk_ipv6Routing_entry_t));
    RTK_L34_IPV6ROUTINGTABLE_SET(3, &rtv6Entry);

RET_NEXTHOP_ERR:
	//Recover original L2 idx
	nxpEt.nhIdx=ori_l2Idx;
	RTK_L34_NEXTHOPTABLE_SET(nxtidx, &nxpEt);
	
	return errorno;
}

int _rtk_rg_internal_IPV6GWMACSetup(unsigned char *ipv6Addr, int l2Idx)
{
    int i,matchIdx=-1,errorno;
	rtk_rg_macEntry_t macEt;
	int valid_macIdx;

    // TODO:After the Gateway mac is learned, we can finish add routing entry
    // TODO:and modify nexthop entry to correct LUT index
	//DEBUG("in _rtk_rg_internal_IPV6GWMACSetup, l2idx is %d",l2Idx);
    //Check l2Idx for success or timeout
    errorno=RT_ERR_RG_ARP_NOT_FOUND;
    if(l2Idx == -1)goto RET_ERR;

	//Get Mac address
	valid_macIdx=l2Idx;
	errorno=rtk_rg_macEntry_find(&macEt, &valid_macIdx);
	if(errorno!=RT_ERR_RG_OK || valid_macIdx!=l2Idx)goto RET_ERR;
	
    //Check each wan interface for matching IPaddr
    for(i=0; i<rg_db.systemGlobal.wanIntfTotalNum; i++)
    {
		//Bridge WAN won't be compared with
		if(rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_type==RTK_RG_BRIDGE)
			continue;
		
		if(memcmp(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->p_wanStaticInfo->gateway_ipv6_addr.ipv6_addr,ipv6Addr,IPV6_ADDR_LEN)==0)
		{
			matchIdx=rg_db.systemGlobal.wanIntfGroup[i].index;
			break;
		}
    }

	errorno=RT_ERR_RG_INVALID_PARAM;
    if(matchIdx == -1)goto RET_ERR;
	//DEBUG("before IPV6GWMACsetup_stage 2 !!! matchidx is %d, l2idx is %d",matchIdx,l2Idx);
	errorno = _rtk_rg_internal_IPV6GWMACSetup_stage2(matchIdx, l2Idx);
	if(errorno!=RT_ERR_RG_OK)goto RET_ERR;

	rg_db.systemGlobal.intfNeighborDiscovery[matchIdx].finished = 1;

	return errorno;
	
RET_ERR:
	rg_db.systemGlobal.intfNeighborDiscovery[matchIdx].finished = -1;
	
    return errorno;
}


void _rtk_rg_arpRequestTimerFunc(unsigned long netIfIdx)
{
#ifdef __KERNEL__
	ipaddr_t ipAddr=0;
	rtk_l34_routing_entry_t rtEntry;
	rtk_rg_ipv4RoutingEntry_t cb_routEt;
	int i;
	if(netIfIdx>MAX_NETIF_SW_TABLE_SIZE) return;

	if(rg_db.systemGlobal.intfArpRequest[netIfIdx].finished==0)
	{
		if(rg_db.systemGlobal.interfaceInfo[netIfIdx].valid == 1 && rg_db.systemGlobal.interfaceInfo[netIfIdx].storedInfo.is_wan == 1)
			ipAddr=rg_db.systemGlobal.interfaceInfo[netIfIdx].p_wanStaticInfo->ip_addr;
		else
			return;

		_rtk_rg_arpGeneration(&rg_db.systemGlobal.interfaceInfo[netIfIdx].storedInfo,ipAddr,&rg_db.systemGlobal.intfArpRequest[netIfIdx]);
		rg_kernel.arpRequestTimerCounter[netIfIdx]++;
		
		//if(rg_kernel.arpRequestTimerCounter[netIfIdx]<10)
		if(1) //nerver timeout (always send arp): until finished=1
		{
			mod_timer(&rg_kernel.arpRequestTimer[netIfIdx], jiffies+200);
		}
		else
		{
			//error happen..recovery what we did before
			//Check which ARP routing entry we added
	        for(i=0; i<MAX_L3_SW_TABLE_SIZE - 1; i++)	//because idx 7 is reserved for default route
	        {
	            //bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
	            //rtk_l34_routingTable_get(i, &rtEntry);
	            //if(rtEntry.ipAddr == ipAddr && rtEntry.process == L34_PROCESS_ARP)
	            if(rg_db.l3[i].rtk_l3.ipAddr == ipAddr && rg_db.l3[i].rtk_l3.process == L34_PROCESS_ARP)
	            {
	            	//Delete the routing entry added and call callback function
	            	bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
					cb_routEt.dest_ip=rg_db.l3[i].rtk_l3.ipAddr;
					cb_routEt.ip_mask=rg_db.l3[i].rtk_l3.ipMask;
		            RTK_L34_ROUTINGTABLE_SET(i, &rtEntry);	
	            	if(rg_db.systemGlobal.initParam.routingDelByHwCallBack != NULL)
					{
						cb_routEt.nexthop=0;
						cb_routEt.wan_intf_idx=netIfIdx;
						rg_db.systemGlobal.initParam.routingDelByHwCallBack(&cb_routEt);
					}
					break;
	            }
	        }

			//reset Global variable
			bzero(&rg_db.systemGlobal.interfaceInfo[netIfIdx].storedInfo.wan_intf.static_info, sizeof(rtk_rg_ipStaticInfo_t));
			rg_db.systemGlobal.intfArpRequest[netIfIdx].finished=-1;

			rtlglue_printf("the ARP request failed when set up WAN interface..\n");
		}
	}
#endif
}

int _rtk_rg_skipARPLearningOrNot(int l3Idx, ipaddr_t sip, int srcPortIdx)
{
	//Gateway IP should not add to ARP table
	if(rg_db.l3[l3Idx].rtk_l3.valid)
	{
		if(rg_db.systemGlobal.interfaceInfo[rg_db.l3[l3Idx].rtk_l3.netifIdx].valid)
		{
			if(rg_db.systemGlobal.interfaceInfo[rg_db.l3[l3Idx].rtk_l3.netifIdx].storedInfo.is_wan)		//WAN interface
			{	
				if(rg_db.systemGlobal.interfaceInfo[rg_db.l3[l3Idx].rtk_l3.netifIdx].p_wanStaticInfo->ip_addr==sip)
				{
					DEBUG("source IP equals to WAN interface[%d]'s IP...skip learning",rg_db.l3[l3Idx].rtk_l3.netifIdx);
					return FAIL;
				}
				else 
				{
					//Check Src port in interface's VLAN member or not
					if(srcPortIdx>=RTK_RG_PORT_CPU)
					{
						if((rg_db.vlan[rg_db.systemGlobal.interfaceInfo[rg_db.l3[l3Idx].rtk_l3.netifIdx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id].Ext_portmask.bits[0]&(0x1<<(srcPortIdx-RTK_RG_PORT_CPU)))==0)	//extension port not exist
						{
							DEBUG("source Extension Port %d is not in WAN interface[%d]'s VLAN...skip learning",srcPortIdx,rg_db.systemGlobal.interfaceInfo[rg_db.l3[l3Idx].rtk_l3.netifIdx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id);
							return FAIL;
						}
					}
					else if((rg_db.vlan[rg_db.systemGlobal.interfaceInfo[rg_db.l3[l3Idx].rtk_l3.netifIdx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id].MemberPortmask.bits[0]&(0x1<<srcPortIdx))==0)	//utp port not exist
					{
						DEBUG("source Port %d is not in WAN interface[%d]'s VLAN...skip learning",srcPortIdx,rg_db.systemGlobal.interfaceInfo[rg_db.l3[l3Idx].rtk_l3.netifIdx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id);
						return FAIL;
					}
				}
			}
			else		//LAN interface
			{
				if(rg_db.systemGlobal.interfaceInfo[rg_db.l3[l3Idx].rtk_l3.netifIdx].p_lanIntfConf->ip_addr==sip)
				{
					DEBUG("source IP equals to LAN interface[%d]'s IP...skip learning",rg_db.l3[l3Idx].rtk_l3.netifIdx);
					return FAIL;
				}
				else
				{
					//Check Src port in interface's VLAN member or not
					if(srcPortIdx>=RTK_RG_PORT_CPU)
					{
						if((rg_db.vlan[rg_db.systemGlobal.interfaceInfo[rg_db.l3[l3Idx].rtk_l3.netifIdx].storedInfo.lan_intf.intf_vlan_id].Ext_portmask.bits[0]&(0x1<<(srcPortIdx-RTK_RG_PORT_CPU)))==0)	//extension port not exist
						{
							DEBUG("source Extension Port %d is not in LAN interface[%d]'s VLAN...skip learning",srcPortIdx,rg_db.systemGlobal.interfaceInfo[rg_db.l3[l3Idx].rtk_l3.netifIdx].storedInfo.lan_intf.intf_vlan_id);
							return FAIL;
						}
					}
					else if((rg_db.vlan[rg_db.systemGlobal.interfaceInfo[rg_db.l3[l3Idx].rtk_l3.netifIdx].storedInfo.lan_intf.intf_vlan_id].MemberPortmask.bits[0]&(0x1<<srcPortIdx))==0)	//utp port not exist
					{
						DEBUG("source Port %d is not in LAN interface[%d]'s VLAN...skip learning",srcPortIdx,rg_db.systemGlobal.interfaceInfo[rg_db.l3[l3Idx].rtk_l3.netifIdx].storedInfo.lan_intf.intf_vlan_id);
						return FAIL;
					}
				}
			}
			
			return SUCCESS;
		}
	}
	
	return FAIL;
}

int _rtk_rg_arpAndMacEntryAdd(ipaddr_t sip, int sipL3Idx, uint8 *pSmac, int srcPortIdx, int *pL2Idx, int macEntryForceAdd, int cvid, int cvidForceAdd, int arpEntryForceAdd)
{
	rtk_rg_arpEntry_t arpEntry;
	rtk_rg_macEntry_t macEntry;
	rtk_rg_arp_linkList_t *pSwArpList;
	int arpIdx,l2Idx;
	short ret;	
	short l3Idx,search_index;
	short count=0;
	char arp_valid=0,mac_exist=0,addArp=1;//,dmac2CVID_Untag=0;

	if(sipL3Idx==FAIL)
		l3Idx=_rtk_rg_l3lookup(sip);
	else
		l3Idx=sipL3Idx;

	//init
	memset(&macEntry,0,sizeof(rtk_rg_macEntry_t));
	
	if(rg_db.l3[l3Idx].rtk_l3.process==L34_PROCESS_ARP)
	{
		//Check if we skip ARP learning or not
		ret=_rtk_rg_skipARPLearningOrNot(l3Idx,sip,srcPortIdx);
		if(ret!=SUCCESS)return ret;
		
		arpIdx=(rg_db.l3[l3Idx].rtk_l3.arpStart<<2)+(sip & ((1<<(31-rg_db.l3[l3Idx].rtk_l3.ipMask))-1));
		arp_valid = rg_db.arp[arpIdx].rtk_arp.valid;
		if(arp_valid)
		{
			if(arpEntryForceAdd)
			{
				if(rg_db.arp[arpIdx].staticEntry)		//the static entry should not be replaced
				{
					DEBUG("ARP entry is valid and STATIC...skip add ARP");
					//don't add ARP, but create new MAC entry
					addArp=0;
					if(pL2Idx!=NULL)*pL2Idx=rg_db.arp[arpIdx].rtk_arp.nhIdx;	//return the l2 idx which pointed by arp entry
				}
			}
			else
			{
				//don't add ARP, but create new MAC entry
				addArp=0;
				if(pL2Idx!=NULL)*pL2Idx=rg_db.arp[arpIdx].rtk_arp.nhIdx;	//return the l2 idx which pointed by arp entry
			}
		}

		//get VID
		macEntry.vlan_id=rg_db.netif[rg_db.l3[l3Idx].rtk_l3.netifIdx].rtk_netif.vlan_id;
	}
	else if(rg_db.l3[l3Idx].rtk_l3.process==L34_PROCESS_CPU)
	{
		if(rg_db.l3[l3Idx].rtk_l3.ipAddr>0)		//non-default route's TRAP routing should add to sw ARP table
		{
			//Check if we skip ARP learning or not
			ret=_rtk_rg_skipARPLearningOrNot(l3Idx,sip,srcPortIdx);
			if(ret!=SUCCESS)return ret;
			
			//FIXME:till now default route to CPU is to protocol, if sw routing link-list is implement, 
			//the default route in hw will means routing should check sw routing link-list, too.			
			_rtk_rg_softwareArpTableLookUp(l3Idx,sip,&pSwArpList);
			if(pSwArpList!=NULL)
			{
				if(arpEntryForceAdd)		//need to replace ARP link-list
				{
					if(rg_db.arp[pSwArpList->idx].staticEntry==0)
					{
						//Delete old link-list first
						DEBUG("delete the old dynamic ARP link-list since arpEntryForceAdd==1");
						_rtk_rg_softwareArpTableDel(pSwArpList);

						addArp=2;	//add to sw link-list
					}
					else
					{
						DEBUG("software ARP link-list is valid and STATIC...skip add ARP");
						arp_valid=1;
						addArp=0;
						if(pL2Idx!=NULL)*pL2Idx=rg_db.arp[pSwArpList->idx].rtk_arp.nhIdx;	//return the l2 idx which pointed by arp entry
					}
				}
				else		//did not add to software ARP table
				{
					DEBUG("software ARP entry is added...skip add ARP");
					arp_valid=1;
					addArp=0;
					if(pL2Idx!=NULL)*pL2Idx=rg_db.arp[pSwArpList->idx].rtk_arp.nhIdx;	//return the l2 idx which pointed by arp entry
				}
			}
			else
				addArp=2;	//add to sw link-list

			//get VID
			macEntry.vlan_id=rg_db.netif[rg_db.l3[l3Idx].rtk_l3.netifIdx].rtk_netif.vlan_id;
		}
		else
		{
			DEBUG("%x from default route with ingress VID %d!!...skip add ARP",sip,cvid);
			//from default route, do not add ARP
			addArp=0;

			//get VID from ingress VID
			if(rg_db.vlan[cvid].valid)
				macEntry.vlan_id=cvid;
			else
				return FAIL;	//VLAN not exist
		}
	}
	else if(rg_db.l3[l3Idx].rtk_l3.process==L34_PROCESS_NH)		//default route should not add to ARP table, and non-default gateway host should not enable arp_used field
	{
		addArp=0;
		DEBUG("from NH interface!!");
		if(rg_db.lut[rg_db.nexthop[rg_db.l3[l3Idx].rtk_l3.nhNxt].rtk_nexthop.nhIdx].valid &&
			rg_db.lut[rg_db.nexthop[rg_db.l3[l3Idx].rtk_l3.nhNxt].rtk_nexthop.nhIdx].rtk_lut.entryType==RTK_LUT_L2UC &&
			memcmp(rg_db.lut[rg_db.nexthop[rg_db.l3[l3Idx].rtk_l3.nhNxt].rtk_nexthop.nhIdx].rtk_lut.entry.l2UcEntry.mac.octet,pSmac,ETHER_ADDR_LEN)==0)
			return SUCCESS;		//default already added

		DEBUG("non-default gateway host....add MAC without arp_used!!");

		//get VID
		macEntry.vlan_id=rg_db.netif[rg_db.nexthop[rg_db.l3[l3Idx].rtk_l3.nhNxt].rtk_nexthop.ifIdx].rtk_netif.vlan_id;
	}
	
	//if((macEntryForceAdd==0) && (arp_valid==1)) return SUCCESS;

	//Find interface and check VLAN mode
	//DEBUG("macEntry.vlan_id is %d, l3Idx is %d",macEntry.vlan_id,l3Idx);
	macEntry.isIVL=rg_db.vlan[macEntry.vlan_id].fidMode==VLAN_FID_IVL?1:0;	//fidMode is IVL, isIVL should be 1
	macEntry.fid=rg_db.vlan[macEntry.vlan_id].fid;

	if(macEntry.isIVL)
	{
		l2Idx=_rtk_rg_hash_mac_vid_efid(pSmac,macEntry.vlan_id,0);		//FIXME;current efid is always 0
	}
	else
	{
ADD_SVL_LUT:
		macEntry.isIVL=0;
		macEntry.arp_used=0;
		count=0;
		mac_exist=0;
		arp_valid=0;	//forced to add SVL MAC
		l2Idx=_rtk_rg_hash_mac_fid_efid(pSmac,macEntry.fid,0);		//FIXME;current efid is always 0
	}
	
	l2Idx<<=2;
	do
	{
		search_index = l2Idx+count;
		//DEBUG("search_idx is %d",search_index);
		if(rg_db.lut[search_index].valid==0)
			break;	//empty
			
		if(rg_db.lut[search_index].rtk_lut.entryType==RTK_LUT_L2UC &&
			(!memcmp(&rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,pSmac,ETHER_ADDR_LEN)))
		{
			if((macEntry.isIVL==1 && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.vid==macEntry.vlan_id) ||
				(macEntry.isIVL==0 && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.fid==macEntry.fid))
			{
				//DEBUG("MAC is exist!");
				mac_exist=1;
				break;
			}
		}
		else
			count++; //search from next entry
	}
	while(count < 4);

	if(count==4)		//no enough space for new lut entry
	{
		count=_rtk_rg_layer2GarbageCollection(l2Idx);		//check if there is asynchronus between software and hardware table
		if(count==4)
		{
			//Check per port SA learning limit
			//------------------ Critical Section start -----------------------//
			//rg_lock(&rg_kernel.saLearningLimitLock);
			if(rg_db.systemGlobal.sourceAddrLearningLimitNumber[srcPortIdx]==atomic_read(&rg_db.systemGlobal.sourceAddrLearningCount[srcPortIdx]))		//no way to learn
			{
				//------------------ Critical Section End -----------------------//
				//rg_unlock(&rg_kernel.saLearningLimitLock);
				DEBUG("Port %d SA learning limit is reached(%d)...won't add MAC!!",srcPortIdx,rg_db.systemGlobal.sourceAddrLearningLimitNumber[srcPortIdx]);
				return SUCCESS;
				if(search_index==-1) 
				{
					FIXME("must add software LUT entry for LUT entry full.");
					return FAIL;
				}
				
			}
			//------------------ Critical Section End -----------------------//
			//rg_unlock(&rg_kernel.saLearningLimitLock);
			
			search_index=_rtk_rg_layer2LeastRecentlyUsedReplace(l2Idx);		//replace the least recently used entry for new entry			
			if(search_index==-1) 
			{
				FIXME("must add software LUT entry for LUT entry full.");
				return FAIL;
			}
		}
		else
			search_index=l2Idx+count;
	}
	
	l2Idx=search_index;

	//DEBUG("l2Idx is %d, arp_valid %d, macEntryForceAdd %d, arpEntryForceAdd %d, mac_exist %d",l2Idx,arp_valid,macEntryForceAdd,arpEntryForceAdd,mac_exist);
	
	if(arp_valid==0 || macEntryForceAdd==1 || arpEntryForceAdd==1)
	{
		if(mac_exist==0 || macEntryForceAdd==1)
		{
			//Check per port SA learning limit
			//------------------ Critical Section start -----------------------//
			//rg_lock(&rg_kernel.saLearningLimitLock);
			if(rg_db.systemGlobal.sourceAddrLearningLimitNumber[srcPortIdx]==atomic_read(&rg_db.systemGlobal.sourceAddrLearningCount[srcPortIdx]))		//no way to learn
			{
				//------------------ Critical Section End -----------------------//
				//rg_unlock(&rg_kernel.saLearningLimitLock);
				DEBUG("Port %d SA learning limit is reached(%d)...won't add MAC!!",srcPortIdx,rg_db.systemGlobal.sourceAddrLearningLimitNumber[srcPortIdx]);
				return SUCCESS;
			}
			//------------------ Critical Section End -----------------------//
			//rg_unlock(&rg_kernel.saLearningLimitLock);
		
			memcpy(macEntry.mac.octet,pSmac,ETHER_ADDR_LEN);
			//Use interface infomation set MAC entry
			macEntry.port_idx=srcPortIdx;
			macEntry.static_entry=0;
			if(cvidForceAdd==1)
			{
				macEntry.vlan_id=cvid;			
			}
			else if((rg_db.vlan[macEntry.vlan_id].UntagPortmask.bits[0]&(0x1<<srcPortIdx))>0 && macEntry.isIVL==0)
			{
				macEntry.vlan_id=0;
				/*for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
				{
					if((rg_db.systemGlobal.lanIntfGroup->p_intfInfo->p_lanIntfConf->ip_addr&
						rg_db.systemGlobal.lanIntfGroup->p_intfInfo->p_lanIntfConf->ip_network_mask)==
						rg_db.l3[l3Idx].rtk_l3.ipAddr)		//20130301-store IP addr after masked
					{
						macEntry.vlan_id=rg_db.systemGlobal.lanIntfGroup->p_intfInfo->p_lanIntfConf->intf_vlan_id;
						//Because DMAC2CVID is enabled, therefore non-zero CVID will always tagged, ignored untag set!!
						if((rg_db.vlan[macEntry.vlan_id].UntagPortmask.bits[0]&(0x1<<srcPortIdx))>0)
							dmac2CVID_Untag=1;
						//FIXME("vid get from lan config=%d",macEntry.vlan_id);
						break;
					}
				}

				if(macEntry.vlan_id==0)
				{	
					for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
					{		
						//Bridge WAN won't be compared with
						if(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->p_wanStaticInfo==NULL)
							continue;
			
						if((rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->p_wanStaticInfo->ip_addr&
							rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->p_wanStaticInfo->ip_network_mask)==
							rg_db.l3[l3Idx].rtk_l3.ipAddr)		//20130301-store IP addr after masked
							{
								macEntry.vlan_id=rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;
								//Because DMAC2CVID is enabled, therefore non-zero CVID will always tagged, ignored untag set!!
								if(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on==0)
									dmac2CVID_Untag=1;
								//FIXME("vid get from wan config=%d",macEntry.vlan_id);
								break;
							}					
					}
				}*/
			}
			//if(dmac2CVID_Untag==1)
				//macEntry.vlan_id=0;
			//FIXME("vid=%d\n",macEntry.vlan_id);
			DEBUG("### add l2[%d]=%02x:%02x:%02x:%02x:%02x:%02x ###\n",l2Idx,pSmac[0],pSmac[1],pSmac[2],pSmac[3],pSmac[4],pSmac[5]);
			if(addArp)macEntry.arp_used=1;			
			ret=rtk_rg_macEntry_add(&macEntry,&l2Idx);
			assert_ok(ret);

			//add to SA learning count
			//------------------ Critical Section start -----------------------//
			//rg_lock(&rg_kernel.saLearningLimitLock);
			atomic_inc(&rg_db.systemGlobal.sourceAddrLearningCount[srcPortIdx]);
			//------------------ Critical Section End -----------------------//
			//rg_unlock(&rg_kernel.saLearningLimitLock);
		}
		else if((rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_ARP_USED)==0 && addArp)
		{
			//keep original data, only toggle arp_used to 1
			memcpy(macEntry.mac.octet,rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN);
			//set SVL for lanIntf, patched in 201221203
			macEntry.fid=rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.fid;
			macEntry.isIVL=(rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_IVL)>0?1:0;
			macEntry.port_idx=rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.port;			
			if(rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.port==RTK_RG_PORT_CPU)
				macEntry.port_idx+=rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.ext_port;

			macEntry.vlan_id=rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.vid;			
			macEntry.static_entry=(rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_STATIC)>0?1:0;
			DEBUG("### enable arp_used in l2[%d]=%02x:%02x:%02x:%02x:%02x:%02x at port=%d ###\n",l2Idx,pSmac[0],pSmac[1],pSmac[2],pSmac[3],pSmac[4],pSmac[5],macEntry.port_idx);
			macEntry.arp_used=1;			
			ret=rtk_rg_macEntry_add(&macEntry,&l2Idx);
			assert_ok(ret);
		}

		if(addArp==1)		//arp will point to IVL one, or SVL
		{
			//DEBUG("add ARP %x! l2Idx is %d",sip,l2Idx);
			arpEntry.ipv4Addr=sip;
			arpEntry.macEntryIdx=l2Idx;
			arpEntry.staticEntry=0;
			if(pL2Idx!=NULL)*pL2Idx=l2Idx;	//return the l2 idx which pointed by arp entry
			assert_ok(rtk_rg_arpEntry_add(&arpEntry,&arpIdx));		
			if(macEntry.isIVL)
			{
				addArp=0;	//arp already added
				goto ADD_SVL_LUT;	//add svl lut, too
			}
		}
		else if(addArp==2)		//add arp entry to sw link-list
		{
			DEBUG("add software ARP %x! l2Idx is %d",sip,l2Idx);
			if(pL2Idx!=NULL)*pL2Idx=l2Idx;	//return the l2 idx which pointed by arp entry
			assert_ok(_rtk_rg_softwareArpTableAdd(l3Idx,sip,l2Idx,0));
			if(macEntry.isIVL)
			{
				addArp=0;	//sw-arp already added
				goto ADD_SVL_LUT;	//add svl lut, too
			}
		}
		return SUCCESS;		
	}
	
	return SUCCESS;
}

void _rtk_rg_neighborDiscoveryTimerFunc(unsigned long netIfIdx)
{
#ifdef __KERNEL__
	rtk_ipv6_addr_t ipAddr;
	//rtk_l34_routing_entry_t rtEntry;
	//rtk_rg_ipv4RoutingEntry_t cb_routEt;
	//int i;
	if(netIfIdx>MAX_NETIF_SW_TABLE_SIZE) return;

	bzero(ipAddr.ipv6_addr,IPV6_ADDR_LEN);

	if(rg_db.systemGlobal.intfNeighborDiscovery[netIfIdx].finished==0)
	{
		if(rg_db.systemGlobal.interfaceInfo[netIfIdx].valid == 1 && rg_db.systemGlobal.interfaceInfo[netIfIdx].storedInfo.is_wan == 1)
			memcpy(&ipAddr,&rg_db.systemGlobal.interfaceInfo[netIfIdx].p_wanStaticInfo->ipv6_addr,sizeof(rtk_ipv6_addr_t));
		else
			return;
		
		_rtk_rg_NDGeneration(&rg_db.systemGlobal.interfaceInfo[netIfIdx].storedInfo,ipAddr,&rg_db.systemGlobal.intfNeighborDiscovery[netIfIdx]);
		rg_kernel.neighborDiscoveryTimerCounter[netIfIdx]++;
		
		//if(rg_kernel.arpRequestTimerCounter[netIfIdx]<10)
		if(1) //nerver timeout (always send arp): until finished=1
		{
			mod_timer(&rg_kernel.neighborDiscoveryTimer[netIfIdx], jiffies+200);
		}
		else
		{
			//error happen..recovery what we did before			
			rtlglue_printf("the Neighbor Discovery failed when set up WAN interface..\n");
		}
	}
#endif
}

uint8 _rtk_rg_CompareIFID(uint8* dip, uint64 interfaceid)
{
	uint8 res = 0;
	
	if(dip[0] != ((interfaceid>>56)&0xff))
	{
		return res;
	}
	else if(dip[1] != ((interfaceid>>48)&0xff))
	{
		return res;
	}
	else if(dip[2] != ((interfaceid>>40)&0xff))
	{
		return res;
	}
	else if(dip[3] != ((interfaceid>>32)&0xff))
	{
		return res;
	}
	else if(dip[4] != ((interfaceid>>24)&0xff))
	{
		return res;
	}
	else if(dip[5] != ((interfaceid>>16)&0xff))
	{
		return res;
	}
	else if(dip[6] != ((interfaceid>>8)&0xff))
	{
		return res;
	}
	else if(dip[7] != (interfaceid&0xff))
	{
		return res;
	}

	return 1;
}

int _rtk_rg_neighborAndMacEntryAdd(unsigned char *sip,int sipL3Idx,uint8 *pSmac,int srcPortIdx, int *pNeighborIdx,int macEntryForceAdd)
{
	rtk_rg_neighborEntry_t neighborEntry;
	rtk_rg_macEntry_t macEntry;
	int l2Idx,ret;	
	int l3Idx;
	int i,count=0;
	int hashIdx,neighbor_valid_idx;
	int mac_exist=0,search_index,addNeighbor=1;//,dmac2CVID_Untag=0;
	//int prefix;
	//unsigned short idx,bitMask;

	if(sipL3Idx==FAIL)
		l3Idx=_rtk_rg_v6L3lookup(sip);
	else
		l3Idx=sipL3Idx;

	if(l3Idx==-1)	//look up fail
		return FAIL;

	if(rg_db.v6route[l3Idx].rtk_v6route.type==L34_IPV6_ROUTE_TYPE_LOCAL)
	{
		//Find interface and check VLAN mode
		memset(&macEntry,0,sizeof(rtk_rg_macEntry_t));
		macEntry.vlan_id=rg_db.netif[rg_db.v6route[l3Idx].rtk_v6route.nhOrIfidIdx].rtk_netif.vlan_id;
		macEntry.isIVL=rg_db.vlan[macEntry.vlan_id].fidMode==VLAN_FID_IVL?1:0;	//fidMode is IVL, isIVL should be 1
		macEntry.fid=rg_db.vlan[macEntry.vlan_id].fid;
		
		if(macEntry.isIVL)
		{
			l2Idx=_rtk_rg_hash_mac_vid_efid(pSmac,macEntry.vlan_id,0);		//FIXME;current efid is always 0
		}
		else
		{
ADD_SVL_LUT:
			macEntry.isIVL=0;
			count=0;
			mac_exist=0;
			l2Idx=_rtk_rg_hash_mac_fid_efid(pSmac,macEntry.fid,0);		//FIXME;current efid is always 0
		}
    	l2Idx<<=2;
   		do
		{
			search_index = l2Idx+count;
			//rtlglue_printf("search_idx is %d\n",search_index);
			if(rg_db.lut[search_index].valid==0)
				break;	//empty
							
			if(rg_db.lut[search_index].rtk_lut.entryType==RTK_LUT_L2UC &&
				(!memcmp(&rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.mac.octet,pSmac,ETHER_ADDR_LEN)))
			{
				if((macEntry.isIVL==1 && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.vid==macEntry.vlan_id) ||
					(macEntry.isIVL==0 && rg_db.lut[search_index].rtk_lut.entry.l2UcEntry.fid==macEntry.fid))
				{
					mac_exist=1;
					break;
				}
			}
			else
				count++; //search from next entry
		}
		while(count < 4);

		if(count==4)		//no enough space for new lut entry
		{
			count=_rtk_rg_layer2GarbageCollection(l2Idx);		//check if there is asynchronus between software and hardware table
			if(count==4)
			{
				//Check per port SA learning limit
				//------------------ Critical Section start -----------------------//
				//rg_lock(&rg_kernel.saLearningLimitLock);
				if(rg_db.systemGlobal.sourceAddrLearningLimitNumber[srcPortIdx]==atomic_read(&rg_db.systemGlobal.sourceAddrLearningCount[srcPortIdx]))		//no way to learn
				{
					//------------------ Critical Section End -----------------------//
					//rg_unlock(&rg_kernel.saLearningLimitLock);
					DEBUG("Port %d SA learning limit is reached(%d)...won't add MAC!!",srcPortIdx,rg_db.systemGlobal.sourceAddrLearningLimitNumber[srcPortIdx]);
					return SUCCESS;
				}
				//------------------ Critical Section End -----------------------//
				//rg_unlock(&rg_kernel.saLearningLimitLock);
				
				search_index=_rtk_rg_layer2LeastRecentlyUsedReplace(l2Idx);		//replace the least recently used entry for new entry
				if(search_index==-1)
				{
					FIXME("must add software LUT entry for LUT entry full.");
					return FAIL;
				}				
			}			
			else
				search_index=l2Idx+count;
		}

		l2Idx=search_index;

		if(mac_exist==0 || macEntryForceAdd==1)
		{
			//Check per port SA learning limit
			//------------------ Critical Section start -----------------------//
			//rg_lock(&rg_kernel.saLearningLimitLock);
			if(rg_db.systemGlobal.sourceAddrLearningLimitNumber[srcPortIdx]==atomic_read(&rg_db.systemGlobal.sourceAddrLearningCount[srcPortIdx]))		//no way to learn
			{
				//------------------ Critical Section End -----------------------//
				//rg_unlock(&rg_kernel.saLearningLimitLock);
				DEBUG("Port %d SA learning limit is reached(%d)...won't add MAC!!",srcPortIdx,rg_db.systemGlobal.sourceAddrLearningLimitNumber[srcPortIdx]);
				return SUCCESS;
			}
			//------------------ Critical Section End -----------------------//
			//rg_unlock(&rg_kernel.saLearningLimitLock);
		
			memcpy(macEntry.mac.octet,pSmac,ETHER_ADDR_LEN);

			//Use interface infomation set MAC entry
			macEntry.port_idx=srcPortIdx;
			macEntry.static_entry=0;
			/*for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
			{
				prefix=rg_db.v6route[l3Idx].rtk_v6route.ipv6PrefixLen;

				if(prefix==128&&memcmp(rg_db.v6route[l3Idx].rtk_v6route.ipv6Addr.ipv6_addr,rg_db.systemGlobal.lanIntfGroup->p_intfInfo->p_lanIntfConf->ipv6_addr.ipv6_addr,IPV6_ADDR_LEN))
				{
					//host route
					macEntry.vlan_id=rg_db.systemGlobal.lanIntfGroup->p_intfInfo->p_lanIntfConf->intf_vlan_id;
					DEBUG("host route, vid get from lan config=%d",macEntry.vlan_id);
					if((rg_db.vlan[macEntry.vlan_id].UntagPortmask.bits[0]&(0x1<<srcPortIdx))>0)
						dmac2CVID_Untag=1;
					break;
				}
					
				idx=(prefix>>3)&0xff;
				
				if((prefix&0x7)==0)
					bitMask=0xff;
				else
					bitMask=(0xff<<(8-(prefix&0x7)))&0xff;
				//DEBUG("prefix = %d, idx = %d, bitMask =%02x",prefix,idx,bitMask);
				
				//DEBUG("ip=%x mask=%d iplookup=%x\n",rg_db.l3[i].rtk_l3.ipAddr,rg_db.l3[i].rtk_l3.ipMask,ip);
				if(memcmp(rg_db.v6route[l3Idx].rtk_v6route.ipv6Addr.ipv6_addr,rg_db.systemGlobal.lanIntfGroup->p_intfInfo->p_lanIntfConf->ipv6_addr.ipv6_addr,(idx-1))==0 &&
					((rg_db.v6route[l3Idx].rtk_v6route.ipv6Addr.ipv6_addr[idx]&bitMask)==(rg_db.systemGlobal.lanIntfGroup->p_intfInfo->p_lanIntfConf->ipv6_addr.ipv6_addr[idx]&bitMask)))
				{
					macEntry.vlan_id=rg_db.systemGlobal.lanIntfGroup->p_intfInfo->p_lanIntfConf->intf_vlan_id;
					DEBUG("vid get from lan config=%d",macEntry.vlan_id);
					if((rg_db.vlan[macEntry.vlan_id].UntagPortmask.bits[0]&(0x1<<srcPortIdx))>0)
						dmac2CVID_Untag=1;
					break;
				}
			}

			if(macEntry.vlan_id==0)
			{	
				for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
				{
					//Bridge WAN won't be compared with
					if(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->p_wanStaticInfo==NULL)
						continue;

					prefix=rg_db.v6route[l3Idx].rtk_v6route.ipv6PrefixLen;

					if(prefix==128&&memcmp(rg_db.v6route[l3Idx].rtk_v6route.ipv6Addr.ipv6_addr,rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->p_wanStaticInfo->ipv6_addr.ipv6_addr,IPV6_ADDR_LEN))
					{
						//host route
						macEntry.vlan_id=rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;
						DEBUG("host route, vid get from wan config=%d",macEntry.vlan_id);
						if(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on==0)
							dmac2CVID_Untag=1;
						break;
					}
						
					idx=(prefix>>3)&0xff;
					
					if((prefix&0x7)==0)
						bitMask=0xff;
					else
						bitMask=(0xff<<(8-(prefix&0x7)))&0xff;
					//DEBUG("prefix = %d, idx = %d, bitMask =%02x",prefix,idx,bitMask);
					
					//DEBUG("ip=%x mask=%d iplookup=%x\n",rg_db.l3[i].rtk_l3.ipAddr,rg_db.l3[i].rtk_l3.ipMask,ip);
					if(memcmp(rg_db.v6route[l3Idx].rtk_v6route.ipv6Addr.ipv6_addr,rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->p_wanStaticInfo->ipv6_addr.ipv6_addr,(idx-1))==0 &&
						((rg_db.v6route[l3Idx].rtk_v6route.ipv6Addr.ipv6_addr[idx]&bitMask)==(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->p_wanStaticInfo->ipv6_addr.ipv6_addr[idx]&bitMask)))
					{
						macEntry.vlan_id=rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;
						DEBUG("vid get from wan config=%d",macEntry.vlan_id);
						if(rg_db.systemGlobal.wanIntfGroup[i].p_intfInfo->storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on==0)
							dmac2CVID_Untag=1;
						break;
					}		
				}
			}
			
			if(dmac2CVID_Untag==1)
				macEntry.vlan_id=0;*/
			if((rg_db.vlan[macEntry.vlan_id].UntagPortmask.bits[0]&(0x1<<srcPortIdx))>0)
				macEntry.vlan_id=0;
			//FIXME("vid=%d\n",macEntry.vlan_id);	
			DEBUG("### add l2[%d]=%02x:%02x:%02x:%02x:%02x:%02x ###\n",l2Idx,pSmac[0],pSmac[1],pSmac[2],pSmac[3],pSmac[4],pSmac[5]);
			if(addNeighbor)macEntry.arp_used=1;
			ret=rtk_rg_macEntry_add(&macEntry,&l2Idx);				
			assert_ok(ret);

			//add to SA learning count
			//------------------ Critical Section start -----------------------//
			//rg_lock(&rg_kernel.saLearningLimitLock);
			atomic_inc(&rg_db.systemGlobal.sourceAddrLearningCount[srcPortIdx]);
			//------------------ Critical Section End -----------------------//
			//rg_unlock(&rg_kernel.saLearningLimitLock);
		}
		else if((rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_ARP_USED)==0 && addNeighbor) 	//Check if this LUT entry has not yet enable arp_used
		{
			//keep original data, only toggle arp_used to 1
			memcpy(macEntry.mac.octet,rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.mac.octet,ETHER_ADDR_LEN);
			//set SVL for lanIntf, patched in 201221203
			macEntry.fid=rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.fid;
			macEntry.isIVL=(rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_IVL)>0?1:0;
			macEntry.port_idx=rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.port;
			if(macEntry.port_idx==RTK_RG_PORT_CPU)
				macEntry.port_idx+=rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.ext_port;
			
			macEntry.vlan_id=rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.vid;			
			macEntry.static_entry=(rg_db.lut[l2Idx].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_STATIC)>0?1:0;
			DEBUG("### enable arp_used in l2[%d]=%02x:%02x:%02x:%02x:%02x:%02x ###\n",l2Idx,pSmac[0],pSmac[1],pSmac[2],pSmac[3],pSmac[4],pSmac[5]);
			macEntry.arp_used=1;
			ret=rtk_rg_macEntry_add(&macEntry,&l2Idx);				
			assert_ok(ret);
			if(macEntry.isIVL)goto ADD_SVL_LUT;		//add ARP_USED to svl lut, too
		}
		
		if(addNeighbor)
		{
			hashIdx=_rtk_rg_IPv6NeighborHash(sip,l3Idx);
			hashIdx<<=3;
			for(i=0;i<8;i++)	//8-way hash
			{
				//check for matching
				neighbor_valid_idx = hashIdx+i;
	 			if((rg_db.v6neighbor[neighbor_valid_idx].rtk_v6neighbor.valid==1) &&
					(rg_db.v6neighbor[neighbor_valid_idx].rtk_v6neighbor.ipv6RouteIdx==l3Idx) &&
					(_rtk_rg_CompareIFID(sip+8, rg_db.v6neighbor[neighbor_valid_idx].rtk_v6neighbor.ipv6Ifid)==1))
					break;		//added before 			
				
				if(rg_db.v6neighbor[neighbor_valid_idx].rtk_v6neighbor.valid==0)
				{
					bzero(&neighborEntry,sizeof(rtk_rg_neighborEntry_t));
					neighborEntry.l2Idx=l2Idx;
					neighborEntry.matchRouteIdx=l3Idx;
					memcpy(neighborEntry.interfaceId,sip+8,8);	//interface ID is the 64~127bits of IPv6 ip address
					neighborEntry.valid=1;
					neighborEntry.staticEntry=0;

					ret=rtk_rg_neighborEntry_add(&neighborEntry,&neighbor_valid_idx);
					assert_ok(ret);
					*pNeighborIdx=neighbor_valid_idx;
					break;
				}
			}
			if(i==8)		//no enough space for new neighbor entry
				return FAIL;
			if(macEntry.isIVL)
			{
				addNeighbor=0;		//already added
				goto ADD_SVL_LUT;
			}
		}
	}
	return SUCCESS;
}

int32 _rtk_rg_internal_wanSet(int wan_intf_idx, rtk_rg_ipStaticInfo_t *hw_static_info)
{
	int ret,i,rtidx=-1,rtv6idx=-1,errorno,routingAdded=0,v6RoutingAdd=0,arpMissed=0,neighborMissed=0,ipv4Enable=0,ipv6Enable=0;
	int arp_valid_idx,neighbor_valid_idx,l2Idx;//,subnet_same_idx;
    unsigned int input_ipmsk,wan_set_mask;
//    rtk_l34_netif_entry_t intfEt;
    rtk_l34_routing_entry_t rtEntry;
    //rtk_l34_ext_intip_entry_t extipEt;
    rtk_ipv6Routing_entry_t rtv6Entry;
	rtk_rg_ipv4RoutingEntry_t cb_routEt;
	rtk_rg_ipv6RoutingEntry_t cb_routv6Et;
	rtk_rg_macEntry_t macEntry;
	rtk_mac_t zeroMAC;
	rtk_ipv6_addr_t zeroIPv6;
#if 0
	rtk_rg_macEntry_t defaultGatewayMAC;
	int defaultGatewayMAC_idx;
	rtk_rg_arpEntry_t defaultGatewayARP;
#endif
	int defaultGatewayNEIGHBOR_idx;
	//unsigned short ipv6HashIdx;
	rtk_rg_wanIntfConf_t wanConfiguration;
	rtk_wanType_entry_t wantEt;
	rtk_rg_neighborInfo_t neighborInfo;
	rtk_rg_routing_arpInfo_t newAddingEntry;
	rtk_l34_ext_intip_entry_t extipEntry;

	//Check input parameters
	if(hw_static_info == NULL)
		return RT_ERR_RG_NULL_POINTER;
	if(wan_intf_idx < 0 || wan_intf_idx >= MAX_NETIF_SW_TABLE_SIZE)
		return RT_ERR_RG_INVALID_PARAM;
	if(hw_static_info->mtu == 0)
			return RT_ERR_RG_INVALID_PARAM;

	//Check IP version
	if(hw_static_info->ip_version==IPVER_V4ONLY || hw_static_info->ip_version==IPVER_V4V6)
	{
		ipv4Enable=1;
		//Check parameters
		if(hw_static_info->ip_addr == 0 || hw_static_info->ip_network_mask == 0)
			return RT_ERR_RG_INVALID_PARAM;
	}
	if(hw_static_info->ip_version==IPVER_V6ONLY || hw_static_info->ip_version==IPVER_V4V6)
	{
		ipv6Enable=1;
		//Check parameters
		if(*(unsigned int *)hw_static_info->ipv6_addr.ipv6_addr == 0 &&
			*(unsigned int *)(hw_static_info->ipv6_addr.ipv6_addr+4) == 0 &&
			*(unsigned int *)(hw_static_info->ipv6_addr.ipv6_addr+8) == 0 &&
			*(unsigned int *)(hw_static_info->ipv6_addr.ipv6_addr+12) == 0)
			return RT_ERR_RG_INVALID_PARAM;
		if(hw_static_info->ipv6_mask_length == 0)
			return RT_ERR_RG_INVALID_PARAM;
	}

	//Check if default gateway is on, gateway ip should be valid
	//if(hw_static_info->default_gateway_on == 1 && hw_static_info->gateway_ipv4_addr == 0)		//without gw ip, we can't set default route
		//return RT_ERR_RG_INVALID_PARAM;

	bzero(&zeroMAC, sizeof(rtk_mac_t));
	bzero(&zeroIPv6, sizeof(rtk_ipv6_addr_t));
#ifdef CONFIG_APOLLO_LITEROMEDRIVER
	//for lite-romeDriver, the auto_learn should not be turn on, and MAC address should not be invalid
	if(hw_static_info->gw_mac_auto_learn_for_ipv4 == 1 || (ipv4Enable==1&&memcmp(&hw_static_info->gateway_mac_addr_for_ipv4,&zeroMAC,sizeof(rtk_mac_t))==0))
		return RT_ERR_RG_GW_MAC_NOT_SET;
	if(hw_static_info->gw_mac_auto_learn_for_ipv6 == 1 || (ipv6Enable==1&&memcmp(&hw_static_info->gateway_mac_addr_for_ipv6,&zeroMAC,sizeof(rtk_mac_t))==0))
		return RT_ERR_RG_GW_MAC_NOT_SET;
#else
	//for romeDriver, if you do not want to send ARP automatically, you must assign one valid MAC address for the valid IP address
	if(hw_static_info->gw_mac_auto_learn_for_ipv4 == 0)
	{
		if((ipv4Enable==1&&hw_static_info->gateway_ipv4_addr>0&&memcmp(&hw_static_info->gateway_mac_addr_for_ipv4,&zeroMAC,sizeof(rtk_mac_t))==0))
			return RT_ERR_RG_GW_MAC_NOT_SET;
	}
	else
	{
		//turn on auto-learn, but we are unable to get gateway mac without  ARP routing
		if((ipv4Enable==1&&hw_static_info->ip_network_mask==0xffffffff))	//host route
			return RT_ERR_RG_GW_MAC_NOT_SET;			
	}
	if(hw_static_info->gw_mac_auto_learn_for_ipv6 == 0)
	{
		if((ipv6Enable==1&&memcmp(hw_static_info->gateway_ipv6_addr.ipv6_addr,zeroIPv6.ipv6_addr,IPV6_ADDR_LEN)&&memcmp(&hw_static_info->gateway_mac_addr_for_ipv6,&zeroMAC,sizeof(rtk_mac_t))==0))
			return RT_ERR_RG_GW_MAC_NOT_SET;
	}
	else
	{
		//turn on auto-learn, but we are unable to get gateway mac without  ARP routing
		if((ipv6Enable==1&&hw_static_info->ipv6_mask_length==128))	//host route
			return RT_ERR_RG_GW_MAC_NOT_SET;			
	}
#endif

	//Check if we are set the same WAN interface twice
	if((rg_db.systemGlobal.wanInfoSet & (0x1<<wan_intf_idx)) > 0)
	{
		//Get, Del, Re-add wan interface before re-set it
		bzero(&wanConfiguration,sizeof(rtk_rg_wanIntfConf_t));
		memcpy(&wanConfiguration,&rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf,sizeof(rtk_rg_wanIntfConf_t));

		//Don't disconnect PPP interface in protocol stack, since we just reset the IP and SessionID into hw table
		if(wanConfiguration.wan_type==RTK_RG_PPPoE)
			rg_db.systemGlobal.not_disconnect_ppp=1;

		ret=rtk_rg_interface_del(wan_intf_idx);
		if(ret!=RT_ERR_RG_OK)return ret;

		//Keep wan interface index before add
		rg_db.systemGlobal.wanIdxForReset=wan_intf_idx;
		ret=rtk_rg_wanInterface_add(&wanConfiguration,&i);
		if(ret!=RT_ERR_RG_OK)return ret;
	}

	//Setup WAN type table
	if(hw_static_info->napt_enable==1)		//L4
	{
		errorno=RT_ERR_RG_WANTYPE_SET_FAIL;
		memcpy(&wantEt, &rg_db.wantype[wan_intf_idx].rtk_wantype, sizeof(rtk_wanType_entry_t));
		wantEt.wanType=L34_WAN_TYPE_L34NAT_ROUTE;		
		ret = RTK_L34_WANTYPETABLE_SET(wan_intf_idx, &wantEt);
		if(ret!=RT_ERR_OK)return errorno;
	}

	//Setup hw MTU
	rg_db.netif[wan_intf_idx].rtk_netif.mtu=hw_static_info->mtu;
#if 0
#ifdef CONFIG_APOLLO_RLE0371
#else
	//Patch for 0601 and 6266, when WAN is pppoe, the L34 will minus extra 8 bytes,
	//therefore we have to patch hardware here to add more 8 bytes for it to subtract
	if(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_PPPoE)
		rg_db.netif[wan_intf_idx].rtk_netif.mtu=hw_static_info->mtu + 8;
#endif
#endif

#if 0
#ifdef CONFIG_APOLLO_RLE0371
	rg_db.netif[wan_intf_idx].rtk_netif.mtu=hw_static_info->mtu;
#else
	//Patch for 0601 and 6266, when binding to interface happened,
	//the packet size have 2 byte would't decrease, causing TRAP reason 224.
	//therefore the hardware setting should be set as preferred value plus 2 here
	rg_db.netif[wan_intf_idx].rtk_netif.mtu=hw_static_info->mtu + 2;
#endif
#endif
	ret = RTK_L34_NETIFTABLE_SET(wan_intf_idx, &rg_db.netif[wan_intf_idx].rtk_netif);
	if(ret!=RT_ERR_OK)return RT_ERR_RG_INTF_SET_FAIL;

	//if 255.255.255.255, we don't need to add routing entry
    if(ipv4Enable==1 && hw_static_info->ip_network_mask!=0xffffffff)
    {
        //Check routing table available or not
        rtidx = MAX_L3_SW_TABLE_SIZE;
		bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
		//subnet_same_idx = MAX_L3_SW_TABLE_SIZE;
        for(i=0; i<MAX_L3_SW_TABLE_SIZE - 1; i++)	//because idx 7 is reserved for default route
        {
            if(rg_db.l3[i].rtk_l3.valid == 0)// && rtidx == MAX_L3_SW_TABLE_SIZE)
            {
                rtidx = i;	//keep
                break;
            }
			else
			{
				//Check if there is any same IP-range routing entry added, 
				//and the routing entry must point to the netif entry which VLAN and MAC address are ALL THE SAME
				if(rg_db.l3[i].rtk_l3.process==L34_PROCESS_ARP &&
					((hw_static_info->ip_addr&hw_static_info->ip_network_mask)==rg_db.l3[i].rtk_l3.ipAddr) &&
					(hw_static_info->ip_network_mask==rg_db.l3[i].netmask))
				{
					errorno=RT_ERR_RG_SUBNET_INTERFACE_ASYMMETRIC;
					if(rg_db.netif[wan_intf_idx].rtk_netif.vlan_id != rg_db.netif[rg_db.l3[i].rtk_l3.netifIdx].rtk_netif.vlan_id ||
					(memcmp(&rg_db.netif[wan_intf_idx].rtk_netif.gateway_mac.octet,&rg_db.netif[rg_db.l3[i].rtk_l3.netifIdx].rtk_netif.gateway_mac.octet,ETHER_ADDR_LEN)!=0))
						goto RET_CHECK_ERR;

					routingAdded=1;
					rtEntry.arpStart=rg_db.l3[i].rtk_l3.arpStart;
					rtEntry.arpEnd=rg_db.l3[i].rtk_l3.arpEnd;
					break;
				}
			}
        }
		
		if(routingAdded==0)
		{	
			errorno=RT_ERR_RG_ENTRY_FULL;
	        if(rtidx == MAX_L3_SW_TABLE_SIZE)goto RET_CHECK_ERR;

	        //Set up Routing table -- ARP
	        rtEntry.netifIdx=wan_intf_idx;
	        rtEntry.valid=1;
	        rtEntry.process=L34_PROCESS_CPU;		//default to sw table
			if(hw_static_info->napt_enable)
	        	rtEntry.internal=0;		//external host from outside, NAPT or NAT
	        else
				rtEntry.internal=1;		//pure routing
	        rtEntry.ipAddr=hw_static_info->ip_addr&hw_static_info->ip_network_mask;		//20130301-store IP addr after masked
	        rtEntry.rt2waninf=1;
	        input_ipmsk=hw_static_info->ip_network_mask;
	        RG_ONE_COUNT(input_ipmsk);
	        rtEntry.ipMask=input_ipmsk-1;
	        
			//Check for ARP table for enough entry
			// TODO:Check for ARP range and add to rg_db.systemGlobal.routingArpInfoArray
			bzero(&newAddingEntry,sizeof(rtk_rg_routing_arpInfo_t));
			newAddingEntry.routingIdx=rtidx;
			newAddingEntry.intfIdx=wan_intf_idx;
			newAddingEntry.notMask=~hw_static_info->ip_network_mask;
			newAddingEntry.bitNum=32-input_ipmsk;
			newAddingEntry.isLan=0;	//WAN

			if(newAddingEntry.bitNum <= 8)	//if need more than or equal to 512 entries, recorded in fwdEngine	
			{
				errorno=_rtk_rg_addArpRoutingArray(&newAddingEntry,hw_static_info->ip_addr,rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id);
				if(errorno==RT_ERR_RG_OK)
				{
					rtEntry.arpStart = newAddingEntry.arpStart;
					rtEntry.arpEnd = newAddingEntry.arpEnd;
					rtEntry.process=L34_PROCESS_ARP;
				}
				else if(errorno!=RT_ERR_RG_ADD_ARP_TO_SW_TABLE)	//for sw table, routing entry just set process to CPU
					goto RET_CHECK_ERR;
			}
			else
				DEBUG("HW table is not enough...will add to software ARP table!");
	        
	        errorno=RT_ERR_RG_ROUTE_SET_FAIL;
	        ret = RTK_L34_ROUTINGTABLE_SET(rtidx, &rtEntry);
	        if(ret!=RT_ERR_OK)goto RET_ROUTE_ERR;

			routingAdded=1;
		}
    }

	if(ipv6Enable==1 && hw_static_info->ipv6_mask_length!=128)
    {
        //Check routing table available or not
        rtv6idx = MAX_IPV6_ROUTING_SW_TABLE_SIZE;
        for(i=0; i<MAX_IPV6_ROUTING_SW_TABLE_SIZE - 1; i++)	//because idx 3 is reserved for default route
        {
            if(rg_db.v6route[i].rtk_v6route.valid == 0)
            {
                rtv6idx = i;	//keep
                break;
            }
        }
		errorno=RT_ERR_RG_ENTRY_FULL;
        if(rtv6idx == MAX_IPV6_ROUTING_SW_TABLE_SIZE)goto RET_CHECK_ERR;

        //Set up Routing table -- ARP
		bzero(&rtv6Entry, sizeof(rtk_ipv6Routing_entry_t));
		rtv6Entry.valid=1;
		rtv6Entry.type=L34_IPV6_ROUTE_TYPE_LOCAL;
		rtv6Entry.nhOrIfidIdx=wan_intf_idx;
		rtv6Entry.ipv6PrefixLen=hw_static_info->ipv6_mask_length;
		memcpy(&rtv6Entry.ipv6Addr,&hw_static_info->ipv6_addr,sizeof(rtk_ipv6_addr_t));
		rtv6Entry.rt2waninf=1;	//local route, routing to WAN

		errorno=RT_ERR_RG_ROUTE_SET_FAIL;
		ret = RTK_L34_IPV6ROUTINGTABLE_SET(rtv6idx,&rtv6Entry);
		if(ret!=RT_ERR_OK)goto RET_ROUTE_ERR;

		v6RoutingAdd=1;
    }

	//store information in Global variable
	rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->ip_version=hw_static_info->ip_version;
    rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->napt_enable=hw_static_info->napt_enable;
    rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->mtu=hw_static_info->mtu;
	rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->gw_mac_auto_learn_for_ipv4=hw_static_info->gw_mac_auto_learn_for_ipv4;
	rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->gw_mac_auto_learn_for_ipv6=hw_static_info->gw_mac_auto_learn_for_ipv6;
	if(ipv4Enable==1)
	{
	    rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->ip_addr=hw_static_info->ip_addr;
	    rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->ip_network_mask=hw_static_info->ip_network_mask;
		rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->ipv4_default_gateway_on=hw_static_info->ipv4_default_gateway_on;
    	rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->gateway_ipv4_addr=hw_static_info->gateway_ipv4_addr;
		memcpy(&rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->gateway_mac_addr_for_ipv4,&hw_static_info->gateway_mac_addr_for_ipv4, sizeof(rtk_mac_t));
	}
	if(ipv6Enable==1)
	{
		memcpy(&rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->ipv6_addr,&hw_static_info->ipv6_addr,sizeof(rtk_ipv6_addr_t));
	    rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->ipv6_mask_length=hw_static_info->ipv6_mask_length;
		rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->ipv6_default_gateway_on=hw_static_info->ipv6_default_gateway_on;
    	memcpy(&rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->gateway_ipv6_addr,&hw_static_info->gateway_ipv6_addr,sizeof(rtk_ipv6_addr_t));
		memcpy(&rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo->gateway_mac_addr_for_ipv6,&hw_static_info->gateway_mac_addr_for_ipv6, sizeof(rtk_mac_t));
	}

	
	//Save default gateway wan interface to this index
	if(hw_static_info->ipv4_default_gateway_on == 1)		
		rg_db.systemGlobal.defaultRouteSet = wan_intf_idx;	
	if(hw_static_info->ipv6_default_gateway_on == 1)
		rg_db.systemGlobal.defaultIPV6RouteSet = wan_intf_idx;
	
	bzero(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.intf_name,32);
    sprintf(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.intf_name,"%d_%s_%s_%c",
                  wan_intf_idx,
                  hw_static_info->ipv4_default_gateway_on==1?"V4INTERNET":"V4OTHER",
                  hw_static_info->ipv6_default_gateway_on==1?"V6INTERNET":"V6OTHER",
                  rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE?'B':'R'
                 );
	//Set WAN set mask
	rg_db.systemGlobal.wanInfoSet |= (0x1<<wan_intf_idx);
	
	//Check ARP table first if we add route before, otherwise call ARP request after that
	if(ipv4Enable==1)
	{
		bzero(&macEntry,sizeof(rtk_rg_macEntry_t));
		if(routingAdded==1)
		{
			if(hw_static_info->gw_mac_auto_learn_for_ipv4 == 0)
			{
				if(hw_static_info->gateway_ipv4_addr == 0)
				{
					memcpy(macEntry.mac.octet,hw_static_info->gateway_mac_addr_for_ipv4.octet,ETHER_ADDR_LEN);
					//set SVL for lanIntf, patched in 20121203
					macEntry.fid=LAN_FID;
					macEntry.isIVL=0;
					macEntry.port_idx=rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx;
					macEntry.vlan_id=rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;	
					//macEntry.static_entry=1;	//won't age out
					macEntry.arp_used=1;		//pointed by nexthop entry		
					ret=rtk_rg_macEntry_add(&macEntry,&l2Idx);
					if(ret!=RT_ERR_RG_OK)
						goto RET_GLB_ERR;

					DEBUG("### add l2[%d]=%02x:%02x:%02x:%02x:%02x:%02x ###\n",l2Idx,macEntry.mac.octet[0],macEntry.mac.octet[1],macEntry.mac.octet[2],
						macEntry.mac.octet[3],macEntry.mac.octet[4],macEntry.mac.octet[5]);

					ret = _rtk_rg_internal_GWMACSetup_stage2(wan_intf_idx,l2Idx);
					errorno=ret;
				}
				else
				{
					DEBUG("add ipv4 gateway ARP and MAC entry..");
					errorno=RT_ERR_RG_ADD_ARP_MAC_FAILED;
					ret = _rtk_rg_arpAndMacEntryAdd(hw_static_info->gateway_ipv4_addr,rtidx,hw_static_info->gateway_mac_addr_for_ipv4.octet,rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx,NULL,1,0,0,1);
				}

				if(ret!=RT_ERR_RG_OK)
					goto RET_GLB_ERR;	
			}

			//Check if ARP and MAC had auto learned before
			if(hw_static_info->gateway_ipv4_addr != 0)
			{
				arp_valid_idx=rtEntry.arpStart<<0x2;
				while(arpMissed==0)
				{				
					if(arp_valid_idx < ((rtEntry.arpEnd+1)<<0x2))		//in the range
					{
						if(rg_db.arp[arp_valid_idx].ipv4Addr==hw_static_info->gateway_ipv4_addr)	//Hit
						{
							//DEBUG("the %x had been add ARP entry!",hw_static_info->gateway_ipv4_addr);
							errorno = _rtk_rg_internal_GWMACSetup(hw_static_info->gateway_ipv4_addr, rg_db.arp[arp_valid_idx].rtk_arp.nhIdx);
							if(errorno!=RT_ERR_RG_OK)
								goto RET_GLB_ERR;
							else
								break;		//arpMissed=0
						}
					}
					else	//out of range
					{
						arpMissed=1;
					}
					arp_valid_idx++;
				}
			}
			else
			{
				//Set up Internal External IP table for NAPT
			   	if(hw_static_info->napt_enable == 1)
			   	{
				 	//Since EIP table is 1-on-1 mapping with netif table, we don't need to loop
				   	//extipIdx = matchIdx;	   //Keep
			
				   	errorno=RT_ERR_RG_EXTIP_SET_FAIL;
					bzero(&extipEntry,sizeof(rtk_l34_ext_intip_entry_t));
				   	extipEntry.intIpAddr=0; 	   //napt special
				   	extipEntry.extIpAddr=hw_static_info->ip_addr;
				   	extipEntry.nhIdx=rg_db.wantype[wan_intf_idx].rtk_wantype.nhIdx;
				   	extipEntry.prival=0;
				   	extipEntry.pri=0;
				   	extipEntry.type=L34_EXTIP_TYPE_NAPT;
				   	extipEntry.valid=1;
			
				   	ret = RTK_L34_EXTINTIPTABLE_SET(wan_intf_idx, &extipEntry);
				   	if(ret!=RT_ERR_OK)goto RET_GLB_ERR;
			
			   		//DEBUG("set ext ip table %d as %x",wan_intf_idx, hw_static_info->ip_addr);
				   	rg_db.systemGlobal.nxpRefCount[rg_db.wantype[wan_intf_idx].rtk_wantype.nhIdx]++;		   //nexthop reference by IP table
			   	}
			}
		}
		else
		{
			//if we are host route(255.255.255.255), must be manual add MAC

			//only add IP table, NEXTHOP table, and l2 table
			memcpy(macEntry.mac.octet,hw_static_info->gateway_mac_addr_for_ipv4.octet,ETHER_ADDR_LEN);
			macEntry.fid=rg_db.vlan[rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id].fid;
			if(rg_db.vlan[rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id].fidMode==VLAN_FID_IVL)
			{
				macEntry.isIVL=1;
				//IVL should refer VLAN's untag setting to decide tag or not
				macEntry.vlan_id=rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;
			}
			else
			{
				macEntry.isIVL=0;
				//Because Forced_DMAC2CVID is turn on, the LUT's VLANID should enter zero if untag!!
				if(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on)
					macEntry.vlan_id=rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;	
				else
					macEntry.vlan_id=0;
			}
			macEntry.port_idx=rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx;
			
			//macEntry.static_entry=1;	//won't age out
			macEntry.arp_used=1;		//pointed by nexthop entry
			ret=rtk_rg_macEntry_add(&macEntry,&l2Idx);
			if(ret!=RT_ERR_RG_OK)
				goto RET_GLB_ERR;

			DEBUG("### add l2[%d]=%02x:%02x:%02x:%02x:%02x:%02x ###",l2Idx,macEntry.mac.octet[0],macEntry.mac.octet[1],macEntry.mac.octet[2],macEntry.mac.octet[3],macEntry.mac.octet[4],macEntry.mac.octet[5]);						
			
			errorno = _rtk_rg_internal_GWMACSetup_stage2(wan_intf_idx,l2Idx);

			if(errorno!=RT_ERR_RG_OK)
				goto RET_GLB_ERR;

		}
	}

	// TODO:IPv6 Neighbor Discovery has to be done here!!!!
	//Check Neighbor table first if we add them before, otherwise call Neighbor Discovery after that
	if(ipv6Enable==1)
	{
		bzero(&macEntry,sizeof(rtk_rg_macEntry_t));
		if(v6RoutingAdd==1)
		{
			bzero(&zeroIPv6,sizeof(rtk_ipv6_addr_t));
			if(hw_static_info->gw_mac_auto_learn_for_ipv6== 0)
			{
				if(memcmp(&hw_static_info->gateway_ipv6_addr,&zeroIPv6,sizeof(rtk_ipv6_addr_t))==0)	//ipv6 == 0
				{
					//Add gateway mac and Default route
					memcpy(macEntry.mac.octet,hw_static_info->gateway_mac_addr_for_ipv6.octet,ETHER_ADDR_LEN);
					//set SVL for lanIntf, patched in 20121203
					macEntry.fid=LAN_FID;
					macEntry.isIVL=0;
					macEntry.port_idx=rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx;
					macEntry.vlan_id=rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;	
					//macEntry.static_entry=1;	//won't age out
					macEntry.arp_used=1;		//pointed by nexthop entry
					ret=rtk_rg_macEntry_add(&macEntry,&l2Idx);
					DEBUG("### add l2[%d]=%02x:%02x:%02x:%02x:%02x:%02x ###\n",l2Idx,macEntry.mac.octet[0],macEntry.mac.octet[1],
						macEntry.mac.octet[2],macEntry.mac.octet[3],macEntry.mac.octet[4],macEntry.mac.octet[5]);
					if(ret!=RT_ERR_RG_OK)
						goto RET_GLB_ERR;

					errorno = _rtk_rg_internal_IPV6GWMACSetup_stage2(wan_intf_idx,l2Idx);
				}
				else
					errorno = _rtk_rg_neighborAndMacEntryAdd(hw_static_info->gateway_ipv6_addr.ipv6_addr,rtv6idx,hw_static_info->gateway_mac_addr_for_ipv6.octet,rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx,&defaultGatewayNEIGHBOR_idx,1);

				if(errorno!=RT_ERR_RG_OK)
					goto RET_GLB_ERR;	
			}

			//Check if Neighbor and MAC had auto learned before
			if(memcmp(&hw_static_info->gateway_ipv6_addr,&zeroIPv6,sizeof(rtk_ipv6_addr_t))!=0)	//ipv6 != 0
			{
				//get hash index
				//ipv6HashIdx = _rtk_rg_IPv6NeighborHash(hw_static_info->gateway_ipv6_addr.ipv6_addr+8, rtv6idx);
				memcpy(neighborInfo.neighborEntry.interfaceId,hw_static_info->gateway_ipv6_addr.ipv6_addr+8,8);
				neighborInfo.neighborEntry.matchRouteIdx=rtv6idx;
				neighbor_valid_idx=-1;
				ret=rtk_rg_neighborEntry_find(&neighborInfo,&neighbor_valid_idx);
				if(ret==RT_ERR_RG_OK)
				{
					//Found
					//DEBUG("Found old neighbor!!");
					errorno = _rtk_rg_internal_IPV6GWMACSetup(hw_static_info->gateway_ipv6_addr.ipv6_addr, rg_db.v6neighbor[neighbor_valid_idx].rtk_v6neighbor.l2Idx);
					if(errorno!=RT_ERR_RG_OK)
						goto RET_GLB_ERR;
				}
				else
					neighborMissed=1;
			}
		}
		else
		{
			//if we are IPv6 host route(128), do nothing

			//only add NEXTHOP table, and l2 table
			memcpy(macEntry.mac.octet,hw_static_info->gateway_mac_addr_for_ipv6.octet,ETHER_ADDR_LEN);
			//set SVL for lanIntf, patched in 20121203
			macEntry.fid=LAN_FID;
			macEntry.isIVL=0;
			macEntry.port_idx=rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_port_idx;
			macEntry.vlan_id=rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id;	
			//macEntry.static_entry=1;	//won't age out
			macEntry.arp_used=1;		//pointed by nexthop entry
			ret=rtk_rg_macEntry_add(&macEntry,&l2Idx);
			DEBUG("### add l2[%d]=%02x:%02x:%02x:%02x:%02x:%02x ###\n",l2Idx,macEntry.mac.octet[0],macEntry.mac.octet[1],
				macEntry.mac.octet[2],macEntry.mac.octet[3],macEntry.mac.octet[4],macEntry.mac.octet[5]);
			if(ret!=RT_ERR_RG_OK)
				goto RET_GLB_ERR;

			errorno = _rtk_rg_internal_IPV6GWMACSetup_stage2(wan_intf_idx,l2Idx);

			if(errorno!=RT_ERR_RG_OK)
				goto RET_GLB_ERR;

		}
	}

	// TODO:Call the initParam's routngAddByHwCallBack
	if(routingAdded==1 && rg_db.systemGlobal.initParam.routingAddByHwCallBack!=NULL)
	{
		cb_routEt.dest_ip=rtEntry.ipAddr;
		cb_routEt.ip_mask=hw_static_info->ip_network_mask;
		cb_routEt.nexthop=0;
		cb_routEt.wan_intf_idx=wan_intf_idx;
		rg_db.systemGlobal.initParam.routingAddByHwCallBack(&cb_routEt);
	}
	// TODO:Call the initParam's v6RoutingAddByHwCallBack
	if(v6RoutingAdd==1 && rg_db.systemGlobal.initParam.v6RoutingAddByHwCallBack!=NULL)
	{
		memcpy(&cb_routv6Et.dest_ip,&rtv6Entry.ipv6Addr,sizeof(rtk_ipv6_addr_t));
		cb_routv6Et.prefix_len=rtv6Entry.ipv6PrefixLen;
		cb_routv6Et.NhOrIntfIdx=wan_intf_idx;
		cb_routv6Et.type=rtv6Entry.type;
		rg_db.systemGlobal.initParam.v6RoutingAddByHwCallBack(&cb_routv6Et);
	}

	// TODO:Use ARP protocol to find out default gateway's mac save in L2idx
	if(arpMissed==1)
	{	
#ifdef CONFIG_APOLLO_ROMEDRIVER
		rg_db.systemGlobal.intfArpRequest[wan_intf_idx].finished=0;
		rg_db.systemGlobal.intfArpRequest[wan_intf_idx].reqIp=hw_static_info->gateway_ipv4_addr;
		rg_db.systemGlobal.intfArpRequest[wan_intf_idx].gwMacReqCallBack=_rtk_rg_internal_GWMACSetup;

#ifdef __KERNEL__
		init_timer(&rg_kernel.arpRequestTimer[wan_intf_idx]);
		rg_kernel.arpRequestTimer[wan_intf_idx].data = (unsigned long)wan_intf_idx;
		rg_kernel.arpRequestTimer[wan_intf_idx].function = _rtk_rg_arpRequestTimerFunc;
		rg_kernel.arpRequestTimerCounter[wan_intf_idx]=0;
		DEBUG("arp miss, request arp=%x\n",rg_db.systemGlobal.intfArpRequest[wan_intf_idx].reqIp);
		mod_timer(&rg_kernel.arpRequestTimer[wan_intf_idx], jiffies+100);
#endif
#else
		//for lite romeDriver, here should not be reached
		errorno=RT_ERR_RG_GW_MAC_NOT_SET;
		goto RET_GLB_ERR;
#endif
	}

	// TODO:IPv6 Neighbor Discovery has to be done here!!!!
	if(neighborMissed==1)
	{
#ifdef CONFIG_APOLLO_ROMEDRIVER
		rg_db.systemGlobal.intfNeighborDiscovery[wan_intf_idx].finished=0;
		memcpy(&rg_db.systemGlobal.intfNeighborDiscovery[wan_intf_idx].reqIp,&hw_static_info->gateway_ipv6_addr,sizeof(rtk_ipv6_addr_t));
		rg_db.systemGlobal.intfNeighborDiscovery[wan_intf_idx].ipv6GwMacReqCallBack=_rtk_rg_internal_IPV6GWMACSetup;
		//DEBUG("wan intf is %d, function pointer is %p",wan_intf_idx,rg_db.systemGlobal.intfNeighborDiscovery[wan_intf_idx].ipv6GwMacReqCallBack);
#ifdef __KERNEL__
		init_timer(&rg_kernel.neighborDiscoveryTimer[wan_intf_idx]);
		rg_kernel.neighborDiscoveryTimer[wan_intf_idx].data = (unsigned long)wan_intf_idx;
		rg_kernel.neighborDiscoveryTimer[wan_intf_idx].function = _rtk_rg_neighborDiscoveryTimerFunc;
		rg_kernel.neighborDiscoveryTimerCounter[wan_intf_idx]=0;
		DEBUG("neighbor miss, discovery neighbor =%08x:%08x:%08x:%08x\n",*(unsigned int *)(rg_db.systemGlobal.intfNeighborDiscovery[wan_intf_idx].reqIp.ipv6_addr),
			*(unsigned int *)(rg_db.systemGlobal.intfNeighborDiscovery[wan_intf_idx].reqIp.ipv6_addr+4),
			*(unsigned int *)(rg_db.systemGlobal.intfNeighborDiscovery[wan_intf_idx].reqIp.ipv6_addr+8),
			*(unsigned int *)(rg_db.systemGlobal.intfNeighborDiscovery[wan_intf_idx].reqIp.ipv6_addr+12));
		mod_timer(&rg_kernel.neighborDiscoveryTimer[wan_intf_idx], jiffies+100);
#endif
#else
		//for lite romeDriver, here should not be reached
		errorno=RT_ERR_RG_GW_MAC_NOT_SET;
		goto RET_GLB_ERR;
#endif

	}

	//add wan-interfcae callback to sync protocal-stack
	if(rg_db.systemGlobal.initParam.interfaceAddByHwCallBack != NULL)
	{
		//rtk_rg_intfInfo_t intfInfo;
		//bzero(&intfInfo,sizeof(intfInfo));
		//memcpy(&intfInfo, &rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo, sizeof(intfInfo));
		rg_db.systemGlobal.initParam.interfaceAddByHwCallBack(&rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo,&wan_intf_idx);
	}

	return RT_ERR_RG_OK;

RET_GLB_ERR:
	//Clear global variable
	wan_set_mask=0x1<<wan_intf_idx;
   	rg_db.systemGlobal.wanInfoSet &= ~(wan_set_mask);
   	bzero(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.intf_name, 32);
	
   	if(hw_static_info->ipv4_default_gateway_on == 1)		//recovery default route interface index
		rg_db.systemGlobal.defaultRouteSet=-1;
	if(hw_static_info->ipv6_default_gateway_on == 1)		//recovery default route interface index
		rg_db.systemGlobal.defaultIPV6RouteSet=-1;
	
	bzero(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].p_wanStaticInfo, sizeof(rtk_rg_ipStaticInfo_t));
RET_ROUTE_ERR:
    //Delete the routing table entry
    if(rtidx>=0)
    {
        bzero(&rtEntry, sizeof(rtk_l34_routing_entry_t));
        RTK_L34_ROUTINGTABLE_SET(rtidx, &rtEntry);
    }
	if(rtv6idx>=0)
    {
        bzero(&rtv6Entry, sizeof(rtk_ipv6Routing_entry_t));
        RTK_L34_IPV6ROUTINGTABLE_SET(rtv6idx, &rtv6Entry);
    }
RET_CHECK_ERR:

	return errorno;
}

int32 rtk_rg_staticInfo_set(int wan_intf_idx, rtk_rg_ipStaticInfo_t *static_info)
{
	int errorno;//,i,rtidx=-1,errorno,routingAdd=0,arp_valid_idx,arpMissed=0;
	
//	unsigned int input_ipmsk,last_arp=0;
//	rtk_l34_netif_entry_t intfEt;
//	rtk_l34_routing_entry_t rtEntry;
	//rtk_l34_ext_intip_entry_t extipEt;
//	rtk_rg_ipv4RoutingEntry_t cb_routEt;
	//Call after rtk_rg_wanInterface_add, so check if we had already add interface
	if(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].valid == 0 || 
		rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.is_wan != 1 ||
		rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type != RTK_RG_STATIC)	
			return RT_ERR_RG_ENTRY_NOT_EXIST;

	//Check if we are reentried
	rg_lock(&rg_kernel.wanStaticCalled);
		
	errorno = _rtk_rg_internal_wanSet(wan_intf_idx, static_info);

	//Set up Internal External IP table for NAPT - we will do this at _rtk_rg_internal_GWMACSetup
#if 0
	if(static_info->napt_enable)
	{
		//interface table is 1-by-1 mapping to iP table
		//therefore we do not need to go through whole table

		//Checking table for availability
		/*for(i=0;i<8;i++)
		{
			memset(&extipEt, 0, sizeof(extipEt));
			ret = rtk_l34_extIntIPTable_get(i, &extipEt);
			if(ret!=RT_ERR_OK)return RT_ERR_RG_EXTIP_FAIL;

			if(extipEt.valid == 0)
				break;
		}
		if(i==8 && extipEt.valid == 1)
			return RT_ERR_RG_ENTRY_FULL;*/
		errorno=RT_ERR_RG_EXTIP_GET_FAIL;
		bzero(&extipEt, sizeof(rtk_l34_ext_intip_entry_t));
		ret = rtk_l34_extIntIPTable_get(wan_intf_idx, &extipEt);
		if(ret!=RT_ERR_OK || extipEt.valid==1)goto RET_ROUTE_ERR;

		extipEt.intIpAddr=0;		//napt special
		extipEt.extIpAddr=static_info->ip_addr;
		extipEt.nhIdx=0;		//used 0 as reserved entry
		extipEt.prival=0;
		extipEt.pri=0;
		extipEt.type=L34_EXTIP_TYPE_NAPT;
		extipEt.valid=1;

		errorno=RT_ERR_RG_EXTIP_SET_FAIL;
		ret = RTK_L34_EXTINTIPTABLE_SET(wan_intf_idx, &extipEt);
		if(ret!=RT_ERR_OK)goto RET_IPTABLE_ERR;
	}
#endif

	

	//RG_GLB_STATIC_CALLED=0;		//clear the lock flag
	//rg_unlock(&RG_GLB_STATIC_CALLED);

	//return RT_ERR_RG_OK;
#if 0
RET_IPTABLE_ERR:
	//Delete the ip table entry
	if(static_info->napt_enable)
	{
		bzero(&extipEt, sizeof(rtk_l34_ext_intip_entry_t));
		RTK_L34_EXTINTIPTABLE_SET(wan_intf_idx, &extipEt);
	}
#endif

	

	//RG_GLB_STATIC_CALLED=0;		//clear the lock flag
	rg_unlock(&rg_kernel.wanStaticCalled);
	
	return errorno;
}

int32 rtk_rg_dhcpRequest_set(int wan_intf_idx)
{
	debug("%s is called!!!",__func__);
	//add dhcpRequestByHwCallBack callback to call dhcpc
	{
		if(rg_db.systemGlobal.initParam.dhcpRequestByHwCallBack != NULL)
		{			
			rg_db.systemGlobal.initParam.dhcpRequestByHwCallBack(&wan_intf_idx);
		}
	}
	return RT_ERR_RG_OK;
}

int32 rtk_rg_dhcpClientInfo_set(int wan_intf_idx, rtk_rg_ipDhcpClientInfo_t *dhcpClient_info)
{
#if 0
    int i,ret,rtidx=-1,errorno,routingAdd=0,arp_valid_idx,arpMissed=0;
    unsigned int input_ipmsk,last_arp=0;
    rtk_l34_routing_entry_t rtEntry;
    //rtk_l34_ext_intip_entry_t extipEt;
	rtk_rg_ipv4RoutingEntry_t cb_routEt;
#endif
    //rtk_l34_netif_entry_t intfEntry;
	int errorno;

    //Check parameter
    //if(dhcpClient_info == NULL)
        //return RT_ERR_RG_NULL_POINTER;

    //if(wan_intf_idx<0 || wan_intf_idx>7)
        //return RT_ERR_RG_INVALID_PARAM;
	//if(dhcpClient_info->hw_info.ip_addr == 0 || dhcpClient_info->hw_info.ip_network_mask == 0 || dhcpClient_info->hw_info.mtu == 0)
        //return RT_ERR_RG_INVALID_PARAM;

    //Call after rtk_rg_wanInterface_add, so check if we had already add interface
    //bzero(&intfEntry, sizeof(rtk_l34_netif_entry_t));
    //ret = rtk_l34_netifTable_get(wan_intf_idx, &intfEntry);
    if(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].valid == 0 ||
		rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.is_wan != 1 ||
        rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type != RTK_RG_DHCP)
        return RT_ERR_RG_ENTRY_NOT_EXIST;

	//Check if we are reentried
	rg_lock(&rg_kernel.wanDHCPCalled);

	errorno = _rtk_rg_internal_wanSet(wan_intf_idx, &dhcpClient_info->hw_info);

    if(errorno==RT_ERR_RG_OK)
		rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.dhcp_client_info.stauts=dhcpClient_info->stauts;

	//RG_GLB_DHCP_CALLED=0;		//clear the lock flag
	rg_unlock(&rg_kernel.wanDHCPCalled);
	
    return errorno;
}

int32 rtk_rg_pppoeClientInfoBeforeDial_set(int wan_intf_idx, rtk_rg_pppoeClientInfoBeforeDial_t *app_info)
{
    //int ret;
    //rtk_l34_netif_entry_t intfEntry;
	//rtk_l34_routing_entry_t rtEntry;

    //Check parameter
    if(app_info == NULL)
        return RT_ERR_RG_NULL_POINTER;
    if(wan_intf_idx<0 || wan_intf_idx>7)
        return RT_ERR_RG_INVALID_PARAM;

    //Call after rtk_rg_wanInterface_add, so check if we had already add interface
    //bzero(&intfEntry, sizeof(rtk_l34_netif_entry_t));
    //ret = rtk_l34_netifTable_get(wan_intf_idx, &intfEntry);
    if(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].valid == 0 ||
		rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.is_wan != 1 ||
        rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type != RTK_RG_PPPoE)
        return RT_ERR_RG_ENTRY_NOT_EXIST;

	//Check if there is default route set up before, if so, we have to keep original setting here and set
	//default route to TRAP	
	/*bzero(&rtEntry,sizeof(rtk_l34_routing_entry_t));
	rtEntry.process=L34_PROCESS_CPU;
	rtEntry.valid=1;		//turn on
	
	ret = RTK_L34_ROUTINGTABLE_SET(7, &rtEntry);	//set default route setting to TRAP, keep other setting
	if(ret!=RT_ERR_OK)return RT_ERR_RG_ROUTE_SET_FAIL;*/

    //Save in global variable
    //bzero(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.username, 4);
    //memcpy(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.username, app_info->username, 4);
	
    bzero(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.username, 32);
	memcpy(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.username, app_info->username, 32);
	//bzero(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.password, 4);
    //memcpy(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.password, app_info->password, 4);	
	bzero(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.password, 32);
	memcpy(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.password, app_info->password, 32);
    rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.auth_type=app_info->auth_type;
    rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.pppoe_proxy_enable=app_info->pppoe_proxy_enable;
    rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.max_pppoe_proxy_num=app_info->max_pppoe_proxy_num;
    rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.auto_reconnect=app_info->auto_reconnect;
    rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.dial_on_demond=app_info->dial_on_demond;
    rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.idle_timeout_secs=app_info->idle_timeout_secs;
    rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.stauts=app_info->stauts;
    rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.dialOnDemondCallBack=app_info->dialOnDemondCallBack;
    rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial.idleTimeOutCallBack=app_info->idleTimeOutCallBack;

	rg_db.systemGlobal.pppoeBeforeCalled = 1;


	//add pppoeBeforeDiagByHwCCallBack callback to call spppdctl
	{
		if(rg_db.systemGlobal.initParam.pppoeBeforeDiagByHwCallBack != NULL)
		{			
			rtk_rg_pppoeClientInfoBeforeDial_t before_dial;
			bzero(&before_dial,sizeof(rtk_rg_pppoeClientInfoBeforeDial_t));
			memcpy(&before_dial, &rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.before_dial, sizeof(rtk_rg_pppoeClientInfoBeforeDial_t));
			rg_db.systemGlobal.initParam.pppoeBeforeDiagByHwCallBack(&before_dial,&wan_intf_idx);
		}
	}

	
    return RT_ERR_RG_OK;
}

int32 rtk_rg_pppoeClientInfoAfterDial_set(int wan_intf_idx, rtk_rg_pppoeClientInfoAfterDial_t *clientPppoe_info)
{
#if 0
    int i,ret,rtidx=-1,errorno,routingAdd=0,arp_valid_idx,arpMissed=0;
    unsigned int input_ipmsk,last_arp=0;
    rtk_l34_netif_entry_t intfEntry;
    rtk_l34_routing_entry_t rtEntry;
    //rtk_l34_ext_intip_entry_t extipEt;
	rtk_rg_ipv4RoutingEntry_t cb_routEt;
#endif
    //rtk_l34_netif_entry_t intfEntry;
	rtk_l34_pppoe_entry_t pppoeEt;
	int ret,errorno;

    //Check parameter
//    if(rg_db.systemGlobal.pppoeBeforeCalled == 0)		//did not call pppoe_before in the past period
//		return RT_ERR_RG_PPPOE_UNINIT;
    //if(clientPppoe_info == NULL)
        //return RT_ERR_RG_NULL_POINTER;
    //if(wan_intf_idx<0 || wan_intf_idx>7)
        //return RT_ERR_RG_INVALID_PARAM;
	//if(clientPppoe_info->hw_info.ip_addr == 0 || clientPppoe_info->hw_info.ip_network_mask == 0 || clientPppoe_info->hw_info.mtu == 0)
        //return RT_ERR_RG_INVALID_PARAM;

    //Call after rtk_rg_wanInterface_add, so check if we had already add interface
    //bzero(&intfEntry, sizeof(rtk_l34_netif_entry_t));
    //ret = rtk_l34_netifTable_get(wan_intf_idx, &intfEntry);
    if(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].valid == 0 ||
		rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.is_wan != 1 ||
        rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.wan_type != RTK_RG_PPPoE)
        return RT_ERR_RG_ENTRY_NOT_EXIST;
	
	//Check if we are reentried
	rg_lock(&rg_kernel.wanPPPOEAfterCalled);

    errorno = _rtk_rg_internal_wanSet(wan_intf_idx, &clientPppoe_info->hw_info);

    if(errorno==RT_ERR_RG_OK)
    {
    	rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.pppoe_info.after_dial.sessionId=clientPppoe_info->sessionId;

		//Set up PPPoE table and next hop table for this interface
		errorno=RT_ERR_RG_PPPOE_SET_FAIL;
		bzero(&pppoeEt, sizeof(rtk_l34_pppoe_entry_t));
		pppoeEt.sessionID=clientPppoe_info->sessionId;
		ret = RTK_L34_PPPOETABLE_SET(wan_intf_idx, &pppoeEt);
		if(ret!=RT_ERR_OK)goto RET_PPPOE_ERR;
	
#ifdef CONFIG_APOLLO_RLE0371
		//Patch for 0371:PPPoE will always use the table index 0
		bzero(&pppoeEt, sizeof(rtk_l34_pppoe_entry_t));
		pppoeEt.sessionID=clientPppoe_info->sessionId;
		ret = RTK_L34_PPPOETABLE_SET(0, &pppoeEt);
		if(ret!=RT_ERR_OK)goto RET_PPPOE_ERR;
#endif

#ifdef CONFIG_RG_PPPOE_PASSTHROUGHT

		//Check if LAN has PPPoE Pass Through
		/*if(rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_tag_on == 0)
		{
			for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
			{
				if(rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->pppoe_passThrough == 1)
					break;
			}
			if(i<rg_db.systemGlobal.lanIntfTotalNum)
			{*/
				//add ACL to transfer WAN to LAN packet with WAN's VLAN
				//1 FIXME: Patch for 201305171900, pppoe pass through has to be disabled!!
				//errorno=RT_ERR_OK;
				//errorno = _rtk_rg_aclFilterSessionID_and_VIDRemarking_add(wan_intf_idx, rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id);
				errorno = _rtk_rg_acl_pppoe_passthrough_for_wanIntf_add(wan_intf_idx,rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac);
				if(errorno!=RT_ERR_OK)goto RET_PPPOE_ERR;
				DEBUG("add VID logging success!! to Gmac_%02x:%02x:%02x:%02x:%02x:%02x will be logging(NOP)",
					rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[0],
					rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[1],
					rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[2],
					rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[3],
					rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[4],
					rg_db.systemGlobal.interfaceInfo[wan_intf_idx].storedInfo.wan_intf.wan_intf_conf.gmac.octet[5]);
			//}
		//}
#endif

   	}

	goto RET_SUCCESS;

RET_PPPOE_ERR:
	//Delete the pppoe etnry
	bzero(&pppoeEt, sizeof(rtk_l34_pppoe_entry_t));
	RTK_L34_PPPOETABLE_SET(wan_intf_idx, &pppoeEt);
#ifdef CONFIG_APOLLO_RLE0371
	RTK_L34_PPPOETABLE_SET(0, &pppoeEt);
#endif

RET_SUCCESS:
	//RG_GLB_PPPOE_AFTER_CALLED=0;		//clear the lock flag	
	rg_unlock(&rg_kernel.wanPPPOEAfterCalled);
    return errorno;
}

//VLAN function
int32 rtk_rg_cvlan_add(rtk_rg_cvlan_info_t *cvlan_info)
{
	int i,ret,errorno;
	rtk_vlan_t vlanID;
	rtk_fidMode_t fidMode;
	rtk_portmask_t mac_pmask,ext_pmask,untag_pmask;

	//Check parameter
	if(rg_db.systemGlobal.initParam.macBasedTagDecision && cvlan_info->isIVL)		//IVL can not be set when DMAC2CVID is trun on
		return RT_ERR_RG_INVALID_PARAM;
	
	//Check if VLAN init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

	vlanID=cvlan_info->vlanId;
	//Check the VLAN ID, it can not be used in interface setting or vlan-binding
	for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
		if(vlanID==rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->intf_vlan_id)
			return RT_ERR_RG_VLAN_USED_BY_INTERFACE;
	for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
		if(vlanID==rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_id)
			return RT_ERR_RG_VLAN_USED_BY_INTERFACE;
	if(rg_db.vlan[vlanID].valid)
	{
		if(rg_db.vlan[vlanID].addedAsCustomerVLAN)		//created before
			return RT_ERR_RG_CVLAN_CREATED;
		else
			return RT_ERR_RG_VLAN_USED_BY_VLANBINDING;			//used in vlan-binding
	}

	//Transfer RG portmask to RTK portmask
	_rtk_rg_portmask_translator(cvlan_info->memberPortMask,&mac_pmask,&ext_pmask);
	memset(&untag_pmask,0,sizeof(rtk_portmask_t));
	untag_pmask.bits[0]|=cvlan_info->untagPortMask.portmask;

	ret = RTK_VLAN_CREATE(vlanID);
	if(ret==RT_ERR_VLAN_EXIST)
		return RT_ERR_RG_CVLAN_RESERVED;		//the vlan had been created for system use

	//Indicate that this vlan is created by customer vlan APIs
	rg_db.vlan[vlanID].addedAsCustomerVLAN=1;
	
	//Setting VLAN
	errorno=RT_ERR_RG_VLAN_SET_FAIL;
	if(cvlan_info->isIVL)
		fidMode=VLAN_FID_IVL;
	else
		fidMode=VLAN_FID_SVL;
	
	ret = RTK_VLAN_FIDMODE_SET(vlanID, fidMode);
	if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
	ret = RTK_VLAN_FID_SET(vlanID, LAN_FID);
	if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;

	ret = RTK_VLAN_PORT_SET(vlanID, &mac_pmask, &untag_pmask);	
	if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
	ret = RTK_VLAN_EXTPORT_SET(vlanID, &ext_pmask);
	if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;	

	//Setting up priority, if have
	if(cvlan_info->priority>0)
	{
		ret = RTK_VLAN_PRIORITYENABLE_SET(vlanID,ENABLED);
		if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
		ret = RTK_VLAN_PRIORITY_SET(vlanID,cvlan_info->priority);
		if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;	
	}

	return RT_ERR_RG_OK;

RET_VLAN_ERR:
	//Delete the customer VLAN created
    RTK_VLAN_DESTROY(vlanID);
	
    return errorno;
}

int32 rtk_rg_cvlan_del(int cvlan_id)
{
	//Check if the VLAN ID is created by customer vlan API before
	if(rg_db.vlan[cvlan_id].addedAsCustomerVLAN==0)
		return RT_ERR_RG_VLAN_NOT_CREATED_BY_CVLAN;

	//Delete the customer VLAN created
    RTK_VLAN_DESTROY(cvlan_id);

	return RT_ERR_RG_OK;
}

//VLAN Binding
int32 rtk_rg_vlanBinding_add(rtk_rg_vlanBinding_t *vlan_binding_info, int *vlan_binding_idx)
{
	int errorno=RT_ERR_RG_OK;
#ifdef CONFIG_APOLLO_RLE0371
#else
    int i,ret,bdIdx=0,vlan_exist=0,vlanID,intfIdx;//,wantype_exist=0,nxpIdx=0;
    //unsigned int tmppmsk,tmpexpmsk;
    //rtk_l34_netif_entry_t intfEntry;
    rtk_binding_entry_t vbindEt;
    //rtk_classify_cfg_t cfEntry;
    rtk_portmask_t mbpmsk,etpmsk;
    rtk_portmask_t ori_pmsk,ori_utmsk,ori_etpmsk;
	rtk_portmask_t port_mask,ext_port_mask; 
    rtk_fidMode_t ori_mode;
    rtk_enable_t ori_prien;
    rtk_pri_t ori_pri;
	rtk_rg_bindingEntry_t cb_bindEt;
	//rtk_wanType_entry_t wantEt;
	//rtk_l34_nexthop_entry_t nxpEt;

    //Check parameter
    if(vlan_binding_info == NULL || vlan_binding_idx == NULL)
        return RT_ERR_RG_NULL_POINTER;
    //if(vlan_binding_info->port_mask.bits[0]==0 && vlan_binding_info->ext_port_mask.bits[0]==0)
    if(vlan_binding_info->port_idx < RTK_RG_PORT0 || vlan_binding_info->port_idx > RTK_RG_EXT_PORT4 ||
		vlan_binding_info->port_idx == RTK_RG_PORT_CPU)
        return RT_ERR_RG_INVALID_PARAM;
    if(vlan_binding_info->ingress_vid==0)
        return RT_ERR_RG_INVALID_PARAM;
	if(rg_db.systemGlobal.initParam.macBasedTagDecision==0)		//binding is turn off
		return RT_ERR_RG_BIND_WITH_UNBIND_WAN;
	if(rg_db.systemGlobal.vlanBindTotalNum==MAX_BIND_SW_TABLE_SIZE)
		return RT_ERR_RG_ENTRY_FULL;

	//Check if VLAN init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;
	
	vlanID=vlan_binding_info->ingress_vid;

	//Check if vlanID has been used in LAN, WAN or 1Q VLAN
	for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
		if(vlanID==rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->intf_vlan_id)
			return RT_ERR_RG_VLAN_USED_BY_INTERFACE;
	for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
		if(vlanID==rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->egress_vlan_id)
			return RT_ERR_RG_VLAN_USED_BY_INTERFACE;
	if(rg_db.vlan[vlanID].valid && rg_db.vlan[vlanID].addedAsCustomerVLAN)
		return RT_ERR_RG_VLAN_USED_BY_CVLAN;
	
    intfIdx=vlan_binding_info->wan_intf_idx;
	//Turn port index to mask 
	if(vlan_binding_info->port_idx <= RTK_RG_PORT_CPU)	//MAC port
	{
		port_mask.bits[0]=0x1<<(vlan_binding_info->port_idx-RTK_RG_PORT0);
		ext_port_mask.bits[0]=0x0;
	}
    else		//EXT port
	{
		ext_port_mask.bits[0]=0x1<<(vlan_binding_info->port_idx-RTK_RG_EXT_PORT0);
		port_mask.bits[0]=0x0;
	}

	//Check binding table if there is any entry contain the same port with the same vlanID
	for(i=0;i<MAX_BIND_SW_TABLE_SIZE;i++)
	{
		//bzero(&vbindEt, sizeof(rtk_binding_entry_t));
		//ret = dal_apollomp_l34_bindingTable_get(i, &vbindEt);		//FIXME:no RTK APIs
		//ret = rtk_l34_bindingTable_get(i, &vbindEt);
		//if(ret==RT_ERR_CHIP_NOT_SUPPORTED)return RT_ERR_RG_CHIP_NOT_SUPPORT;
		//if(ret!=RT_ERR_OK)return RT_ERR_RG_PORT_BIND_GET_FAIL;		

		//we should not assign the same port binding and vlan with different Wan interface
		if((rg_db.bind[i].rtk_bind.portMask.bits[0]&port_mask.bits[0])>0 && rg_db.bind[i].rtk_bind.vidLan==vlanID) 
			return RT_ERR_RG_INVALID_PARAM;
		if((rg_db.bind[i].rtk_bind.extPortMask.bits[0]&ext_port_mask.bits[0])>0 && rg_db.bind[i].rtk_bind.vidLan==vlanID)
			return RT_ERR_RG_INVALID_PARAM;
	}

    //Have to call after rtk_rg_wanInterface_add, so check if we had already add interface and it's binding WAN
    /*bzero(&intfEntry, sizeof(rtk_l34_netif_entry_t));
    ret = rtk_l34_netifTable_get(intfIdx, &intfEntry);
    if(ret!=RT_ERR_OK || intfEntry.valid == 0 || */
    if(rg_db.systemGlobal.interfaceInfo[intfIdx].valid==0 || rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.is_wan!=1)
        return RT_ERR_RG_INVALID_PARAM;

	//Patch for binding L2 bug
	//unnecessary here, since we had patched when WAN interface added.
	/*if(rg_db.systemGlobal.interfaceInfo[intfIdx].storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE)
	{
		intfEntry.mtu=1502;					
		ret = RTK_L34_NETIFTABLE_SET(intfIdx, &intfEntry);
		if(ret!=RT_ERR_OK)return RT_ERR_RG_INTF_SET_FAIL;
	}*/

    //Set up VLAN
    ori_utmsk.bits[0]=0x0;	//no untag port
    ori_pmsk=port_mask;
    ori_etpmsk=ext_port_mask;
    ori_mode=VLAN_FID_SVL;		//always SVL, to use DMAC2CVID function
    ori_prien=0;
    ori_pri=0;

    errorno=RT_ERR_RG_VLAN_SET_FAIL;
    ret = RTK_VLAN_CREATE(vlanID);
    if(ret == RT_ERR_VLAN_EXIST)
    {
        //ret = rtk_vlan_port_get(vlanID, &ori_pmsk, &ori_utmsk);
        //if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_GET_FAIL;
		memcpy(&ori_pmsk, &rg_db.vlan[vlanID].MemberPortmask,sizeof(rtk_portmask_t));
		memcpy(&ori_utmsk, &rg_db.vlan[vlanID].UntagPortmask,sizeof(rtk_portmask_t));
		//ret = rtk_vlan_extPort_get(vlanID, &ori_etpmsk);
        //if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_GET_FAIL;
		memcpy(&ori_etpmsk, &rg_db.vlan[vlanID].Ext_portmask,sizeof(rtk_portmask_t));
		//ret = rtk_vlan_fidMode_get(vlanID, &ori_mode);
        //if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_GET_FAIL;
		ori_mode=rg_db.vlan[vlanID].fidMode;
		//ret = rtk_vlan_priorityEnable_get(vlanID, &ori_prien);
        //if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_GET_FAIL;
		ori_prien=rg_db.vlan[vlanID].priorityEn;
		//ret = rtk_vlan_priority_get(vlanID, &ori_pri);
        //if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_GET_FAIL;
        ori_pri=rg_db.vlan[vlanID].priority;
	
        vlan_exist=1;
    }
    else if(ret!=RT_ERR_OK)
        goto RET_VLAN_ERR;

    //Set up its member port, extension port set, and FID mode
    ret = RTK_VLAN_FIDMODE_SET(vlanID, VLAN_FID_SVL);
    if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;

    mbpmsk.bits[0]=port_mask.bits[0]|ori_pmsk.bits[0];
    etpmsk.bits[0]=ext_port_mask.bits[0]|ori_etpmsk.bits[0];
    ret = RTK_VLAN_PORT_SET(vlanID, &mbpmsk, &ori_utmsk);	//don't touch untag set
    if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;
    ret = RTK_VLAN_EXTPORT_SET(vlanID, &etpmsk);
    if(ret!=RT_ERR_OK)goto RET_VLAN_ERR;

    //Find the biggest index right now (RG_GLB_VLANBD_IDX start from -1)
/*    for(i=0; i<BINDING_MAX_IDX; i++)
    {
        //if(RG_GLB_VLANBD_IDX[i] > bdIdx)
            //bdIdx=RG_GLB_VLANBD_IDX[i]+1;		//Keep
        if(RG_GLB_VLANBD_IDX[i] == -1)
			break;
    }
	errorno=RT_ERR_RG_ENTRY_FULL;
	if(i==BINDING_MAX_IDX)goto RET_VLAN_ERR;
	bdIdx=i;	//Keep*/

	//Pick one binding entry that not occupied before
	for(i=0; i<MAX_BIND_SW_TABLE_SIZE; i++)		//Port-vlan binding start from the top of Binding Table
    {
        //if(rg_db.systemGlobal.bindToIntf[i] == -1)
        if(rg_db.bind[i].valid == 0)
            break;
    }
    if(i==MAX_BIND_SW_TABLE_SIZE)goto RET_VLAN_ERR;

	bdIdx=i;	//Keep

	//rg_db.systemGlobal.bindToIntf[bdIdx]=intfIdx;
	//rg_db.systemGlobal.bindWithVLAN[bdIdx]=vlanID;		//save the vlan
    //RG_GLB_VLANBD_IDX[bdIdx]=i;

	//Add binding entry once a time
    errorno=RT_ERR_RG_PORT_BIND_SET_FAIL;
    bzero(&vbindEt, sizeof(rtk_binding_entry_t));
    vbindEt.vidLan=vlanID;
    vbindEt.wanTypeIdx=intfIdx;
    vbindEt.bindProto=L34_BIND_PROTO_ALL;		//ALL protocol in L3, and L2
	vbindEt.portMask=port_mask;
	vbindEt.extPortMask=ext_port_mask;
	
    //ret = dal_apollomp_l34_bindingTable_set(i, &vbindEt);		//FIXME:no RTK APIs
    ret = RTK_L34_BINDINGTABLE_SET(bdIdx, &vbindEt);
    if(ret==RT_ERR_CHIP_NOT_SUPPORTED)
	{
		errorno=RT_ERR_RG_CHIP_NOT_SUPPORT;
		goto RET_BINDING_ERR; 
	}
    if(ret!=RT_ERR_OK)goto RET_BINDING_ERR;

	rg_db.bind[bdIdx].valid = 1;

	// TODO:Call the initParam's bindingAddByHwCallBack
	if(rg_db.systemGlobal.initParam.bindingAddByHwCallBack != NULL)
	{
		cb_bindEt.type=BIND_TYPE_VLAN;
		cb_bindEt.vlan.vlan_bind_port_idx=vlan_binding_info->port_idx;
		cb_bindEt.vlan.vlan_bind_vlan_id=vlanID;
		cb_bindEt.wan_intf_idx=intfIdx;
		rg_db.systemGlobal.initParam.bindingAddByHwCallBack(&cb_bindEt);
	}
	
#if 0
    tmppmsk=port_mask.bits[0];
    tmpexpmsk=ext_port_mask.bits[0];

    //Add port-binding we have to set

    count=tmppmsk;
    RG_ONE_COUNT(count);
    for(j=0; j<count; j++)
    {
        errorno=RT_ERR_RG_ENTRY_FULL;
        for(i=0; i<32; i++)		//Port-vlan binding start from the top of Binding Table
        {
            /*memset(&pbindEt, 0, sizeof(pbindEt));
            ret = dal_apollomp_l34_bindingTable_get(i, &pbindEt);	//FIXME:no RTK APIs
            if(ret!=RT_ERR_OK)return RT_ERR_RG_PORT_BIND_FAIL;

            if(pbindEt.portMask.bits[0]==0 && pbindEt.extPortMask.bits[0]==0)
            	break;*/
            if(rg_db.systemGlobal.bindToIntf[i] == -1)
                break;
        }
        if(i==32)goto RET_VLAN_ERR;

        rg_db.systemGlobal.bindToIntf[i]=intfIdx;
        RG_GLB_VLANBD_IDX[i]=bdIdx;

        //Add binding entry once a time
        errorno=RT_ERR_RG_PORT_BIND_SET_FAIL;
        memset(&vbindEt, 0, sizeof(vbindEt));
        vbindEt.extPortMask.bits[0]=0;
        vbindEt.vidLan=vlanID;
        vbindEt.wanTypeIdx=intfIdx;
        vbindEt.bindProto=L34_BIND_PROTO_NOT_IPV6;		//IPv4 only

        if((tmppmsk&0x1)==1)			//PORT0
        {
            tmppmsk&=0xfffffffe;
            vbindEt.portMask.bits[0]=0x1;
        }
        else if((tmppmsk&0x2)==1)		//PORT1
        {
            tmppmsk&=0xfffffffd;
            vbindEt.portMask.bits[0]=0x2;
        }
        else if((tmppmsk&0x4)==1)		//PORT2
        {
            tmppmsk&=0xfffffffb;
            vbindEt.portMask.bits[0]=0x4;
        }
        else if((tmppmsk&0x8)==1)		//PORT3
        {
            tmppmsk&=0xfffffff7;
            vbindEt.portMask.bits[0]=0x8;
        }
        else if((tmppmsk&0x10)==1)		//PORT4
        {
            tmppmsk&=0xffffffef;
            vbindEt.portMask.bits[0]=0x10;
        }
        else if((tmppmsk&0x20)==1)		//PORT5
        {
            tmppmsk&=0xffffffdf;
            vbindEt.portMask.bits[0]=0x20;
        }
        ret = dal_apollomp_l34_bindingTable_set(i, &vbindEt);		//FIXME:no RTK APIs
        if(ret!=RT_ERR_OK)goto RET_BINDING_ERR;
    }

    //Add extport-binding we have to set
    count=tmpexpmsk;
    RG_ONE_COUNT(count);
    for(j=0; j<count; j++)
    {
        errorno=RT_ERR_RG_ENTRY_FULL;
        for(i=0; i<32; i++)		//Port-vlan binding start from the top of Binding Table
        {
            /*memset(&pbindEt, 0, sizeof(pbindEt));
            ret = dal_apollomp_l34_bindingTable_get(i, &pbindEt);	//FIXME:no RTK APIs
            if(ret!=RT_ERR_OK)return RT_ERR_RG_PORT_BIND_FAIL;

            if(pbindEt.portMask.bits[0]==0 && pbindEt.extPortMask.bits[0]==0)
            	break;*/
            if(rg_db.systemGlobal.bindToIntf[i] == -1)
                break;
        }
        if(i==32)goto RET_BINDING_ERR;

        rg_db.systemGlobal.bindToIntf[i]=intfIdx;
        RG_GLB_VLANBD_IDX[i]=bdIdx;

        //Add binding entry once a time
        errorno=RT_ERR_RG_EXTPORT_BIND_SET_FAIL;
        memset(&vbindEt, 0, sizeof(vbindEt));
        vbindEt.portMask.bits[0]=0;
        vbindEt.vidLan=vlanID;
        vbindEt.wanTypeIdx=intfIdx;
        vbindEt.bindProto=L34_BIND_PROTO_NOT_IPV6;		//IPv4 only

        if((tmpexpmsk&0x1)==1)			//EXTPORT1
        {
            tmpexpmsk&=0xfffffffe;
            vbindEt.extPortMask.bits[0]=0x1;
        }
        else if((tmpexpmsk&0x2)==1)		//EXTPORT2
        {
            tmpexpmsk&=0xfffffffd;
            vbindEt.extPortMask.bits[0]=0x2;
        }
        else if((tmpexpmsk&0x4)==1)		//EXTPORT3
        {
            tmpexpmsk&=0xfffffffb;
            vbindEt.extPortMask.bits[0]=0x4;
        }
        else if((tmpexpmsk&0x8)==1)		//EXTPORT4
        {
            tmpexpmsk&=0xfffffff7;
            vbindEt.extPortMask.bits[0]=0x8;
        }
        else if((tmpexpmsk&0x10)==1)	//EXTPORT5
        {
            tmpexpmsk&=0xfffffff7;
            vbindEt.extPortMask.bits[0]=0x10;
        }
        ret = dal_apollomp_l34_bindingTable_set(i, &vbindEt);		//FIXME:no RTK APIs
        if(ret!=RT_ERR_OK)goto RET_BINDING_ERR;

    }
#endif
    //Return the binding index 
    *vlan_binding_idx = bdIdx;

    //Turn on CF rule for downstream packets to tag as it outbound vlan tag
    if(rg_db.systemGlobal.vlanBindTotalNum == 0)
    {
        /*memset(&cfEntry, 0, sizeof(cfEntry));
        cfEntry.index=RG_GLB_VLAN_BINDING_CFIDX;

        errorno=RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
        ret = rtk_classify_cfgEntry_get(&cfEntry);
        if(ret!=RT_ERR_OK)goto RET_BINDING_ERR;

        cfEntry.valid=1;

        ret = rtk_classify_cfgEntry_add(&cfEntry);
        if(ret!=RT_ERR_OK)goto RET_BINDING_ERR;*/
        /*memset(&cfEntry, 0, sizeof(cfEntry));
        cfEntry.index=RESERVED_CF_VLANBINDING_MAC_LEARN_ENTRY;		//defined in internal.h
        cfEntry.direction=CLASSIFY_DIRECTION_DS;
        cfEntry.valid=1;
        cfEntry.act.dsAct.cAct=CLASSIFY_DS_CACT_ADD_CTAG_8100;
        cfEntry.act.dsAct.cVidAct=CLASSIFY_DS_VID_ACT_FROM_LUT;
        cfEntry.act.dsAct.cPriAct=CLASSIFY_DS_PRI_ACT_NOP;
        cfEntry.act.dsAct.uniAct=CLASSIFY_DS_UNI_ACT_NOP;		//all port should be classified
        ret = rtk_classify_cfgEntry_add(&cfEntry);
        if(ret!=RT_ERR_OK)return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;*/
    }

    rg_db.systemGlobal.vlanBindTotalNum++;		//count how many vlan-binding index we have

    return RT_ERR_RG_OK;

RET_BINDING_ERR:
    //Delete all set up binding rule
    for(i=0; i<MAX_BIND_SW_TABLE_SIZE; i++)
    {
        //if(rg_db.systemGlobal.bindToIntf[i]==intfIdx && rg_db.systemGlobal.bindWithVLAN[i]==vlanID)
        if(rg_db.bind[i].valid==1&&
			rg_db.bind[i].rtk_bind.wanTypeIdx==intfIdx&&
			rg_db.bind[i].rtk_bind.vidLan==vlanID)
        {
            bzero(&vbindEt, sizeof(rtk_binding_entry_t));
            //dal_apollomp_l34_bindingTable_get(i, &vbindEt);		//FIXME:no RTK APIs
            /*rtk_l34_bindingTable_get(i, &vbindEt);
            if(vbindEt.vidLan==vlanID)
            {*/
                //dal_apollomp_l34_bindingTable_set(i, &vbindEt);		//FIXME:no RTK APIs
                RTK_L34_BINDINGTABLE_SET(i, &vbindEt);
                //rg_db.systemGlobal.bindToIntf[i]=-1;
				//rg_db.systemGlobal.bindWithVLAN[i]=-1;

				rg_db.bind[i].valid=0;
            //}
        }

        //Reset the "index"
        /*if(RG_GLB_VLANBD_IDX[i]==bdIdx)
        {
            RG_GLB_VLANBD_IDX[i]=-1;
        }*/
    }
#if 0
RET_WANTYPE_ERR:
	//Delete WAN type entry
	if(wantype_exist==0)
	{
		bzero(&wantEt, sizeof(rtk_wanType_entry_t));
		//dal_apollomp_l34_wanTypeTable_set(intfIdx, &wantEt);		//FIXME:no RTK APIs
		rtk_l34_wanTypeTable_set(intfIdx, &wantEt);
	}
RET_NEXTHOP_ERR:
	//Delete nexthop entry
	if(wantype_exist==0)
	{
		bzero(&nxpEt, sizeof(rtk_l34_nexthop_entry_t));
		RTK_L34_NEXTHOPTABLE_SET(nxpIdx, &nxpEt);	
	}
#endif
RET_VLAN_ERR:
    //recovery vlan setting
    if(vlan_exist==1)
    {
        RTK_VLAN_PORT_SET(vlanID, &ori_pmsk, &ori_utmsk);
        RTK_VLAN_EXTPORT_SET(vlanID, &ori_etpmsk);
        RTK_VLAN_FIDMODE_SET(vlanID, ori_mode);
        RTK_VLAN_PRIORITYENABLE_SET(vlanID, ori_prien);
        RTK_VLAN_PRIORITY_SET(vlanID, ori_pri);
    }
    else
    {
        RTK_VLAN_DESTROY(vlanID);
    }
#endif
    return errorno;
}

int32 rtk_rg_vlanBinding_del(int vlan_binding_idx)
{
#ifdef CONFIG_APOLLO_RLE0371
#else
    int i,idx,ret;
	rtk_l34_netif_entry_t intfEntry;
    rtk_binding_entry_t vbindEt;
	rtk_rg_bindingEntry_t cb_bindEt;
    //rtk_classify_cfg_t cfEntry;

    //Check parameter
    if(vlan_binding_idx<0 && vlan_binding_idx>=MAX_BIND_SW_TABLE_SIZE)
        return RT_ERR_RG_INVALID_PARAM;
    if(rg_db.systemGlobal.vlanBindTotalNum==0 || rg_db.bind[vlan_binding_idx].valid==0 || rg_db.bind[vlan_binding_idx].rtk_bind.vidLan==0)	//never delete vlan-binding before add one
        return RT_ERR_RG_VLAN_BIND_UNINIT;
	//Check if VLAN init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

    //Check for matching index in RG_GLB_VLANBD_IDX array, delete HW binding table,
    //and reset RG_GLB_VLANBD_IDX
#if 0
    for(i=0; i<BINDING_MAX_IDX; i++)
    {
        if(RG_GLB_VLANBD_IDX[i]==vlan_binding_idx)
        {
            memset(&vbindEt, 0, sizeof(vbindEt));
            ret = dal_apollomp_l34_bindingTable_set(i, &vbindEt);		//FIXME:no RTK APIs
            if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_BIND_SET_FAIL;

            //Reset the index
            RG_GLB_VLANBD_IDX[i]=-1;
        }
    }
#endif

	/*for(i=vlan_binding_idx;i<BINDING_MAX_IDX;i++)
	{
		if(rg_db.systemGlobal.bindToIntf[i]!=-1)	//not valid
			break;
	}
	if(i==BINDING_MAX_IDX)return RT_ERR_RG_VLAN_BIND_UNINIT;
	idx=i;	//Keep*/
	idx=vlan_binding_idx;

	//Patch for binding L2 bug
	if(rg_db.systemGlobal.interfaceInfo[rg_db.bind[idx].rtk_bind.wanTypeIdx].storedInfo.wan_intf.wan_intf_conf.wan_type==RTK_RG_BRIDGE)
	{
		//Check binding table if there is any other entry has the same interface idx, including vlan-binding and port-binding
		//P.S. wanTypeIdx is equal to interface idx here
		for(i=0;i<MAX_BIND_SW_TABLE_SIZE;i++)
		{
			if(i!=idx && rg_db.bind[i].rtk_bind.wanTypeIdx==rg_db.bind[idx].rtk_bind.wanTypeIdx) 
				break;
		}
		if(i==MAX_BIND_SW_TABLE_SIZE)		//no other vlan-binding using same interface
		{
			bzero(&intfEntry, sizeof(rtk_l34_netif_entry_t));
			ret = rtk_l34_netifTable_get(rg_db.bind[idx].rtk_bind.wanTypeIdx, &intfEntry);
			if(ret!=RT_ERR_OK)return RT_ERR_RG_INTF_GET_FAIL;

			intfEntry.mtu=1500;			
			ret = RTK_L34_NETIFTABLE_SET(rg_db.bind[idx].rtk_bind.wanTypeIdx, &intfEntry);
			if(ret!=RT_ERR_OK)return RT_ERR_RG_INTF_SET_FAIL;
		}
	}
	
	//idx=RG_GLB_VLANBD_IDX[vlan_binding_idx];
	//bzero(&vbindEt, sizeof(rtk_binding_entry_t));
	//ret = rtk_l34_bindingTable_get(idx, &vbindEt);
	//if(ret==RT_ERR_CHIP_NOT_SUPPORTED)return RT_ERR_RG_CHIP_NOT_SUPPORT;
	//if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_BIND_GET_FAIL;

	//Set up for callback function
	cb_bindEt.type=BIND_TYPE_VLAN;
	if(rg_db.bind[idx].rtk_bind.portMask.bits[0]>0)
	{
		if((rg_db.bind[idx].rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT0)) > 0)				//RTK_RG_PORT0
			cb_bindEt.vlan.vlan_bind_port_idx=RTK_RG_PORT0;
		else if((rg_db.bind[idx].rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT1)) > 0)			//RTK_RG_PORT1
			cb_bindEt.vlan.vlan_bind_port_idx=RTK_RG_PORT1;
		else if((rg_db.bind[idx].rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT2)) > 0)			//RTK_RG_PORT2
			cb_bindEt.vlan.vlan_bind_port_idx=RTK_RG_PORT2;
		else if((rg_db.bind[idx].rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT3)) > 0)			//RTK_RG_PORT3
			cb_bindEt.vlan.vlan_bind_port_idx=RTK_RG_PORT3;
		else if((rg_db.bind[idx].rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT_PON)) > 0)		//RTK_RG_PORT_PON
			cb_bindEt.vlan.vlan_bind_port_idx=RTK_RG_PORT_PON;
		else if((rg_db.bind[idx].rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT_RGMII)) > 0)	//RTK_RG_PORT_RGMII
			cb_bindEt.vlan.vlan_bind_port_idx=RTK_RG_PORT_RGMII;
		else if((rg_db.bind[idx].rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT_CPU)) > 0)		//RTK_RG_PORT_CPU
			cb_bindEt.vlan.vlan_bind_port_idx=RTK_RG_PORT_CPU;
	}
	else
	{
		if((rg_db.bind[idx].rtk_bind.extPortMask.bits[0]&(0x1<<RTK_RG_BD_EXT_PORT0)) > 0)			//RTK_RG_EXT_PORT0
			cb_bindEt.vlan.vlan_bind_port_idx=RTK_RG_EXT_PORT0;
		else if((rg_db.bind[idx].rtk_bind.extPortMask.bits[0]&(0x1<<RTK_RG_BD_EXT_PORT1)) > 0)		//RTK_RG_EXT_PORT1
			cb_bindEt.vlan.vlan_bind_port_idx=RTK_RG_EXT_PORT1;
		else if((rg_db.bind[idx].rtk_bind.extPortMask.bits[0]&(0x1<<RTK_RG_BD_EXT_PORT2)) > 0)		//RTK_RG_EXT_PORT2
			cb_bindEt.vlan.vlan_bind_port_idx=RTK_RG_EXT_PORT2;
		else if((rg_db.bind[idx].rtk_bind.extPortMask.bits[0]&(0x1<<RTK_RG_BD_EXT_PORT3)) > 0)		//RTK_RG_EXT_PORT3
			cb_bindEt.vlan.vlan_bind_port_idx=RTK_RG_EXT_PORT3;
		else if((rg_db.bind[idx].rtk_bind.extPortMask.bits[0]&(0x1<<RTK_RG_BD_EXT_PORT4)) > 0)		//RTK_RG_EXT_PORT4
			cb_bindEt.vlan.vlan_bind_port_idx=RTK_RG_EXT_PORT4;
	}
	cb_bindEt.vlan.vlan_bind_vlan_id=rg_db.bind[idx].rtk_bind.vidLan;

	bzero(&vbindEt, sizeof(rtk_binding_entry_t));
	//ret = dal_apollomp_l34_bindingTable_set(idx, &vbindEt);		//FIXME:no RTK APIs
	ret = RTK_L34_BINDINGTABLE_SET(idx, &vbindEt);
	if(ret==RT_ERR_CHIP_NOT_SUPPORTED)return RT_ERR_RG_CHIP_NOT_SUPPORT;
	if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_BIND_SET_FAIL;

	//Delete VLAN if there is no other interface or binding rule used
	ret=0;
	for(i=0;i<MAX_NETIF_SW_TABLE_SIZE;i++)
	{
		if(rg_db.systemGlobal.interfaceInfo[i].storedInfo.is_wan==1)	//WAN
		{
			if(rg_db.systemGlobal.interfaceInfo[i].storedInfo.wan_intf.wan_intf_conf.egress_vlan_id==rg_db.bind[idx].rtk_bind.vidLan)
				ret++;
		}
		else	//LAN
		{
			if(rg_db.systemGlobal.interfaceInfo[i].p_lanIntfConf->intf_vlan_id==rg_db.bind[idx].rtk_bind.vidLan)
				ret++;
		}
	}
	for(i=0;i<MAX_BIND_SW_TABLE_SIZE;i++)
	{
		if(i!=idx && rg_db.bind[i].valid==1 &&
			rg_db.bind[i].rtk_bind.vidLan==rg_db.bind[idx].rtk_bind.vidLan)
			ret++;
	}
	if(ret==0)		//no other used this VLAN right now
	{
		ret = RTK_VLAN_DESTROY(rg_db.bind[idx].rtk_bind.vidLan);
		if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_SET_FAIL;
	}

	// TODO:Call the initParam's bindingDelByHwCallBack
	if(rg_db.systemGlobal.initParam.bindingDelByHwCallBack != NULL)
	{
		rg_db.systemGlobal.initParam.bindingDelByHwCallBack(&cb_bindEt);
	}

	//Reset the index
	//rg_db.systemGlobal.bindWithVLAN[idx]=-1;
	//rg_db.systemGlobal.bindToIntf[idx]=-1;
	rg_db.bind[idx].valid=0;
	
    if(rg_db.systemGlobal.vlanBindTotalNum>0)
        rg_db.systemGlobal.vlanBindTotalNum--;

    //Turn off CF rule for downstream packets to tag as it outbound vlan tag if there is no vlan-binding
    if(rg_db.systemGlobal.vlanBindTotalNum==0)
    {
        /*memset(&cfEntry, 0, sizeof(cfEntry));
        cfEntry.index=VLAN_BINDING_CFIDX;

        ret = rtk_classify_cfgEntry_get(&cfEntry);
        if(ret!=RT_ERR_OK)return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;

        cfEntry.valid=0;

        ret = rtk_classify_cfgEntry_add(&cfEntry);
        if(ret!=RT_ERR_OK)return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;*/
        /*ret = rtk_classify_cfgEntry_del(RESERVED_CF_VLANBINDING_MAC_LEARN_ENTRY);
        if(ret!=RT_ERR_OK)return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;*/
    }
#endif

    return RT_ERR_RG_OK;
}

int32 rtk_rg_vlanBinding_find(rtk_rg_vlanBinding_t *vlan_binding_info, int *valid_idx)
{
#ifdef CONFIG_APOLLO_RLE0371
#else
    int i,/*ret,*/vbindIdx,/*vlanID,wtIdx,*/portIdx=-1;
    //rtk_binding_entry_t vbindEt;
    //rtk_portmask_t mbpmsk,etpmsk;

    //Check parameter
    if(vlan_binding_info==NULL || valid_idx==NULL)
        return RT_ERR_RG_NULL_POINTER;
    if(rg_db.systemGlobal.vlanBindTotalNum==0)		//never find vlan-binding before add one
        return RT_ERR_RG_VLAN_BIND_UNINIT;
	//Check if VLAN init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;
	
    vbindIdx=*valid_idx;
    if(vbindIdx<0 && vbindIdx>=MAX_BIND_SW_TABLE_SIZE)
        return RT_ERR_RG_INVALID_PARAM;
#if 0
    for(i=vbindIdx; i<BINDING_MAX_IDX; i++)
    {
        for(j=0; j<32; j++)
        {
            if(RG_GLB_VLANBD_IDX[i]==vbindIdx)	//this index is valid
            {
                isValid=1;
                break;
            }
        }
        if(isValid==1)
            break;
    }
    if(i==32)return RT_ERR_RG_VLAN_BIND_GET_FAIL;
#endif
	for(i=vbindIdx; i<MAX_BIND_SW_TABLE_SIZE; i++)
	{
		//if(rg_db.systemGlobal.bindWithVLAN[i]!=-1)	//this index is valid
		if(rg_db.bind[i].valid == 1)
			break;
	}
	if(i==MAX_BIND_SW_TABLE_SIZE)return RT_ERR_RG_VLAN_BIND_UNINIT;

	vbindIdx=i;		//Keep
	
    //now vbindIdx is contain a valid index for binding rules

    //Get binding rule with valid_idx, and save port and extport mask in info
    //bzero(&vbindEt, sizeof(rtk_binding_entry_t));
	//memcpy(&vbindEt, &rg_db.bind[vbindIdx].rtk_bind, sizeof(rtk_binding_entry_t));
    //ret = dal_apollomp_l34_bindingTable_get(vbindIdx, &vbindEt);		//FIXME:no RTK APIs
    //ret = rtk_l34_bindingTable_get(vbindIdx, &vbindEt);
    //if(ret==RT_ERR_CHIP_NOT_SUPPORTED)return RT_ERR_RG_CHIP_NOT_SUPPORT;
    //if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_BIND_GET_FAIL;
#if 0
    mbpmsk.bits[0]=0x0;
    etpmsk.bits[0]=0x0;
    for(i=0; i<32; i++)
    {
        if(RG_GLB_VLANBD_IDX[i]==vbindIdx)
        {
            memset(&vbindEt, 0, sizeof(vbindEt));
            ret = dal_apollomp_l34_bindingTable_get(i, &vbindEt);		//FIXME:no RTK APIs
            if(ret!=RT_ERR_OK)return RT_ERR_RG_VLAN_BIND_GET_FAIL;

            //Union all port and extension port mask
            mbpmsk.bits[0]|=vbindEt.portMask.bits[0];
            etpmsk.bits[0]|=vbindEt.extPortMask.bits[0];
        }
    }
#endif

    //vlanID=rg_db.bind[vbindIdx].rtk_bind.vidLan;//vbindEt.vidLan;
    //wtIdx=rg_db.bind[vbindIdx].rtk_bind.wanTypeIdx;//vbindEt.wanTypeIdx;

	if(rg_db.bind[vbindIdx].rtk_bind.portMask.bits[0]>0)
	{
		//mac port is set up
		if((rg_db.bind[vbindIdx].rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT0)) > 0)			
            portIdx=RTK_RG_PORT0;
        else if((rg_db.bind[vbindIdx].rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT1)) > 0)		
            portIdx=RTK_RG_PORT1;
        else if((rg_db.bind[vbindIdx].rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT2)) > 0)		
            portIdx=RTK_RG_PORT2;
        else if((rg_db.bind[vbindIdx].rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT3)) > 0)		
        	portIdx=RTK_RG_PORT3;
        else if((rg_db.bind[vbindIdx].rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT_PON)) > 0)	
            portIdx=RTK_RG_PORT_PON;
		else if((rg_db.bind[vbindIdx].rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT_RGMII)) > 0)	
            portIdx=RTK_RG_PORT_RGMII;
		else if((rg_db.bind[vbindIdx].rtk_bind.portMask.bits[0]&(0x1<<RTK_RG_PORT_CPU)) > 0)	
            portIdx=RTK_RG_PORT_CPU;
	}
	else
	{
		//extension port is set up
		if((rg_db.bind[vbindIdx].rtk_bind.extPortMask.bits[0]&(0x1<<RTK_RG_BD_EXT_PORT0)) > 0)			
            portIdx=RTK_RG_EXT_PORT0;
        else if((rg_db.bind[vbindIdx].rtk_bind.extPortMask.bits[0]&(0x1<<RTK_RG_BD_EXT_PORT1)) > 0)		
            portIdx=RTK_RG_EXT_PORT1;
        else if((rg_db.bind[vbindIdx].rtk_bind.extPortMask.bits[0]&(0x1<<RTK_RG_BD_EXT_PORT2)) > 0)		
            portIdx=RTK_RG_EXT_PORT2;
        else if((rg_db.bind[vbindIdx].rtk_bind.extPortMask.bits[0]&(0x1<<RTK_RG_BD_EXT_PORT3)) > 0)		
        	portIdx=RTK_RG_EXT_PORT3;
        else if((rg_db.bind[vbindIdx].rtk_bind.extPortMask.bits[0]&(0x1<<RTK_RG_BD_EXT_PORT4)) > 0)	
            portIdx=RTK_RG_EXT_PORT4;
	}
	
	if(portIdx==-1)return RT_ERR_RG_VLAN_BIND_GET_FAIL;

    //Return the valid index
    *valid_idx = vbindIdx;
	vlan_binding_info->port_idx=portIdx;
    vlan_binding_info->ingress_vid=rg_db.bind[vbindIdx].rtk_bind.vidLan;
    vlan_binding_info->wan_intf_idx=rg_db.bind[vbindIdx].rtk_bind.wanTypeIdx;
#endif

    return RT_ERR_RG_OK;
}

//ALG
void _rtk_rg_alg_setPort(unsigned short int portNum, int TCP, int enable)
{
	int algIdx;
	unsigned int algBitValue;

	if(portNum<=0)return;

	if(TCP==1)
	{
		algIdx=portNum>>5;
		algBitValue=0x1<<(portNum&0x1f);
		//DEBUG("before TCP enable[%d] is %x, algbitvalue = %x",algIdx,rg_db.algTcpExternPortEnabled[algIdx],algBitValue);
		if(enable)
			rg_db.algTcpExternPortEnabled[algIdx]|=algBitValue;
		else
			rg_db.algTcpExternPortEnabled[algIdx]&=(~algBitValue);
		//DEBUG("after TCP enable[%d] is %x, algbitvalue = %x",algIdx,rg_db.algTcpExternPortEnabled[algIdx],algBitValue);
	}
	else
	{
		algIdx=portNum>>5;
		algBitValue=0x1<<(portNum&0x1f);
		//DEBUG("before UDP enable[%d] is %x, algbitvalue = %x",algIdx,rg_db.algUdpExternPortEnabled[algIdx],algBitValue);
		if(enable)
			rg_db.algUdpExternPortEnabled[algIdx]|=algBitValue;
		else
			rg_db.algUdpExternPortEnabled[algIdx]&=(~algBitValue);
		//DEBUG("after UDP enable[%d] is %x, algbitvalue = %x",algIdx,rg_db.algUdpExternPortEnabled[algIdx],algBitValue);
	}
}

void _rtk_rg_alg_setSrvInLanPort(unsigned short int portNum, int TCP, int enable)
{
	int algIdx;
	unsigned int algBitValue;

	if(portNum<=0)return;

	if(TCP==1)
	{
		algIdx=portNum>>5;
		algBitValue=0x1<<(portNum&0x1f);
		//DEBUG("before TCP enable[%d]@SrvInLAN is %x, algbitvalue = %x",algIdx,rg_db.algTcpExternPortEnabled_SrvInLan[algIdx],algBitValue);
		if(enable)
			rg_db.algTcpExternPortEnabled_SrvInLan[algIdx]|=algBitValue;
		else
			rg_db.algTcpExternPortEnabled_SrvInLan[algIdx]&=(~algBitValue);
		//DEBUG("after TCP enable[%d]@SrvInLAN is %x, algbitvalue = %x",algIdx,rg_db.algTcpExternPortEnabled_SrvInLan[algIdx],algBitValue);
	}
	else
	{
		algIdx=portNum>>5;
		algBitValue=0x1<<(portNum&0x1f);
		//DEBUG("before UDP enable[%d]@SrvInLAN is %x, algbitvalue = %x",algIdx,rg_db.algUdpExternPortEnabled_SrvInLan[algIdx],algBitValue);
		if(enable)
			rg_db.algUdpExternPortEnabled_SrvInLan[algIdx]|=algBitValue;
		else
			rg_db.algUdpExternPortEnabled_SrvInLan[algIdx]&=(~algBitValue);
		//DEBUG("after UDP enable[%d]@SrvInLAN is %x, algbitvalue = %x",algIdx,rg_db.algUdpExternPortEnabled_SrvInLan[algIdx],algBitValue);
	}
}

int32 rtk_rg_algServerInLanAppsIpAddr_add(rtk_rg_alg_serverIpMapping_t *srvIpMapping)
{
	int i;
	
	//Check param
	if(srvIpMapping->algType<RTK_RG_ALG_SIP_TCP_SRV_IN_LAN_BIT || srvIpMapping->algType>RTK_RG_ALG_FTP_UDP_SRV_IN_LAN_BIT)
		return RT_ERR_RG_INVALID_PARAM;
	if(srvIpMapping->serverAddress==0x0)
		return RT_ERR_RG_INVALID_PARAM;

	//Setup service/IP mapping in rg_db
	for(i=0;i<MAX_ALG_SERV_IN_LAN_NUM;i++)
	{
		if(rg_db.algServInLanIpMapping[i].algType==srvIpMapping->algType)
		{
			if(rg_db.algServInLanIpMapping[i].serverAddress)		//exist
				return RT_ERR_RG_ALG_SRV_IN_LAN_EXIST;
			else
			{
				rg_db.algServInLanIpMapping[i].serverAddress=srvIpMapping->serverAddress;
				rg_db.algServInLanIpMask|=srvIpMapping->algType;
				break;
			}
		}
	}

	return RT_ERR_RG_OK;
}

int32 rtk_rg_algServerInLanAppsIpAddr_del(rtk_rg_alg_type_t delServerMapping)
{
	int i;
	//Check Param
	if((rg_db.algServInLanIpMask&delServerMapping)==0)
		return RT_ERR_RG_INVALID_PARAM;

	//Delete service/IP mapping in rg_db
	for(i=0;i<MAX_ALG_SERV_IN_LAN_NUM;i++)
	{
		if((delServerMapping&rg_db.algServInLanIpMapping[i].algType)>0)
		{
			rg_db.algServInLanIpMapping[i].serverAddress=0;
			rg_db.algServInLanIpMask&=(~rg_db.algServInLanIpMapping[i].algType);
		}
	}
	
	return RT_ERR_RG_OK;
}

int32 rtk_rg_algApps_set(rtk_rg_alg_type_t alg_app)
{
#ifdef CONFIG_RG_PPPOE_PASSTHROUGHT
	int ret,i,lan_pmsk=0,wan_pmask=0;
#endif
	int pppoe_switch=0;

	rtk_rg_alg_type_t serverInLanMask,checkServerInLanMask;

	if((rg_db.algFunctionMask & RTK_RG_ALG_PPPOE_PASSTHROUGH_BIT) > 0){ 	//Check PPPoE pass through function enabled or not
		pppoe_switch=1;
	}
	
	checkServerInLanMask = (alg_app>>8)&0xff;		//only care about serverInLan settings
	serverInLanMask = rg_db.algServInLanIpMask;
	//DEBUG("the checkServerInLanMask is %x, serverInLanMask is %x",checkServerInLanMask,serverInLanMask);
	//Check param
	//in Lan server's ip should be setuped before enable ALG service
	if((checkServerInLanMask|serverInLanMask)!=rg_db.algServInLanIpMask)
		return RT_ERR_RG_ALG_SRV_IN_LAN_NO_IP;

#ifndef CONFIG_RG_PPPOE_PASSTHROUGHT
	if((alg_app & RTK_RG_ALG_PPPOE_PASSTHROUGH_BIT) > 0){ 	//Check PPPoE pass through function enabled or not
		rtlglue_printf("PPPoE PassThrought not supported!\n");
		return RT_ERR_RG_PPPOEPASSTHROUGHT_NOT_SUPPORTED;
	}
#endif

	rg_db.algFunctionMask = alg_app;


	//Check ServerInWAN settings
	_rtk_rg_alg_setPort(RTK_RG_ALG_SIP_TCP_PORT,1,(rg_db.algFunctionMask & RTK_RG_ALG_SIP_TCP_BIT));		//enable or disable SIP TCP port
	_rtk_rg_alg_setPort(RTK_RG_ALG_SIP_UDP_PORT,0,(rg_db.algFunctionMask & RTK_RG_ALG_SIP_UDP_BIT));		//enable or disable SIP UDP port
	_rtk_rg_alg_setPort(RTK_RG_ALG_H323_TCP_PORT,1,(rg_db.algFunctionMask & RTK_RG_ALG_H323_TCP_BIT));		//enable or disable H323 TCP port
	_rtk_rg_alg_setPort(RTK_RG_ALG_H323_UDP_PORT,0,(rg_db.algFunctionMask & RTK_RG_ALG_H323_UDP_BIT));		//enable or disable H323 UDP port
	_rtk_rg_alg_setPort(RTK_RG_ALG_RTSP_TCP_PORT,1,(rg_db.algFunctionMask & RTK_RG_ALG_RTSP_TCP_BIT));		//enable or disable RTSP TCP port
	_rtk_rg_alg_setPort(RTK_RG_ALG_RTSP_UDP_PORT,0,(rg_db.algFunctionMask & RTK_RG_ALG_RTSP_UDP_BIT));		//enable or disable RTSP UDP port
	_rtk_rg_alg_setPort(RTK_RG_ALG_FTP_TCP_PORT,1,(rg_db.algFunctionMask & RTK_RG_ALG_FTP_TCP_BIT));		//enable or disable FTP TCP port
	_rtk_rg_alg_setPort(RTK_RG_ALG_FTP_UDP_PORT,0,(rg_db.algFunctionMask & RTK_RG_ALG_FTP_UDP_BIT));		//enable or disable FTP UDP port

	//Check ServerInLAN settings
	_rtk_rg_alg_setSrvInLanPort(RTK_RG_ALG_SIP_TCP_PORT,1,(rg_db.algFunctionMask & RTK_RG_ALG_SIP_TCP_SRV_IN_LAN_BIT));			//enable or disable SIP Server in LAN TCP port
	_rtk_rg_alg_setSrvInLanPort(RTK_RG_ALG_SIP_UDP_PORT,0,(rg_db.algFunctionMask & RTK_RG_ALG_SIP_UDP_SRV_IN_LAN_BIT));			//enable or disable SIP Server in LAN UDP port
	_rtk_rg_alg_setSrvInLanPort(RTK_RG_ALG_H323_TCP_PORT,1,(rg_db.algFunctionMask & RTK_RG_ALG_H323_TCP_SRV_IN_LAN_BIT));		//enable or disable H323 Server in LAN TCP port
	_rtk_rg_alg_setSrvInLanPort(RTK_RG_ALG_H323_UDP_PORT,0,(rg_db.algFunctionMask & RTK_RG_ALG_H323_UDP_SRV_IN_LAN_BIT));		//enable or disable H323 Server in LAN UDP port
	_rtk_rg_alg_setSrvInLanPort(RTK_RG_ALG_RTSP_TCP_PORT,1,(rg_db.algFunctionMask & RTK_RG_ALG_RTSP_TCP_SRV_IN_LAN_BIT));		//enable or disable RTSP Server in LAN TCP port
	_rtk_rg_alg_setSrvInLanPort(RTK_RG_ALG_RTSP_UDP_PORT,0,(rg_db.algFunctionMask & RTK_RG_ALG_RTSP_UDP_SRV_IN_LAN_BIT));		//enable or disable RTSP Server in LAN UDP port
	_rtk_rg_alg_setSrvInLanPort(RTK_RG_ALG_FTP_TCP_PORT,1,(rg_db.algFunctionMask & RTK_RG_ALG_FTP_TCP_SRV_IN_LAN_BIT));			//enable or disable FTP Server in LAN TCP port
	_rtk_rg_alg_setSrvInLanPort(RTK_RG_ALG_FTP_UDP_PORT,0,(rg_db.algFunctionMask & RTK_RG_ALG_FTP_UDP_SRV_IN_LAN_BIT));			//enable or disable FTP Server in LAN UDP port

	//Check Pass through settings
	_rtk_rg_alg_setPort(RTK_RG_ALG_PPTP_PASSTHROUGH_TCP_PORT,1,(rg_db.algFunctionMask & RTK_RG_ALG_PPTP_TCP_PASSTHROUGH_BIT));			//enable or disable PPTP TCP port
	_rtk_rg_alg_setPort(RTK_RG_ALG_PPTP_PASSTHROUGH_UDP_PORT,0,(rg_db.algFunctionMask & RTK_RG_ALG_PPTP_UDP_PASSTHROUGH_BIT));			//enable or disable PPTP UDP port
	_rtk_rg_alg_setPort(RTK_RG_ALG_L2TP_PASSTHROUGH_TCP_PORT,1,(rg_db.algFunctionMask & RTK_RG_ALG_L2TP_TCP_PASSTHROUGH_BIT));			//enable or disable L2TP TCP port
	_rtk_rg_alg_setPort(RTK_RG_ALG_L2TP_PASSTHROUGH_UDP_PORT,0,(rg_db.algFunctionMask & RTK_RG_ALG_L2TP_UDP_PASSTHROUGH_BIT));			//enable or disable L2TP UDP port
	_rtk_rg_alg_setPort(RTK_RG_ALG_IPSEC_PASSTHROUGH_TCP_PORT,1,(rg_db.algFunctionMask & RTK_RG_ALG_IPSEC_TCP_PASSTHROUGH_BIT));		//enable or disable IPSEC TCP port
	_rtk_rg_alg_setPort(RTK_RG_ALG_IPSEC_PASSTHROUGH_UDP_PORT,0,(rg_db.algFunctionMask & RTK_RG_ALG_IPSEC_UDP_PASSTHROUGH_BIT));		//enable or disable IPSEC UDP port


#ifdef CONFIG_RG_PPPOE_PASSTHROUGHT

	//PPPoE Passthrough is special case, it do not have Laye4 port
	if((rg_db.algFunctionMask & RTK_RG_ALG_PPPOE_PASSTHROUGH_BIT) > 0)		//enable PPPoE pass through function
	{	
		//1 FIXME: Patch for 201305171900, pppoe pass through has to be disabled!!
		if(pppoe_switch==0)
		{
			//Gather all LAN port
			for(i=0;i<rg_db.systemGlobal.lanIntfTotalNum;i++)
				lan_pmsk|=rg_db.systemGlobal.lanIntfGroup[i].p_intfInfo->p_lanIntfConf->port_mask.portmask;
			//Gather all WAN port
			for(i=0;i<rg_db.systemGlobal.wanIntfTotalNum;i++)
				wan_pmask|=(0x1<<rg_db.systemGlobal.wanIntfGroup[i].p_wanIntfConf->wan_port_idx);
			ret=_rtk_rg_acl_pppoe_passthrough_add(lan_pmsk,wan_pmask,DEFAULT_CPU_VLAN);
			if(ret!=SUCCESS)return ret;
			//_rtk_rg_aclFilterSessionID_and_VIDRemarking_add(-1,DEFAULT_PASSTHROUGH);
		}
	}
	else if(pppoe_switch==1)
	{
		//1 FIXME: Patch for 201305171900, pppoe pass through has to be disabled!!
		_rtk_rg_acl_pppoe_passthrough_del();
		//_rtk_rg_acl_pppoe_passthrough_for_wanIntf_del(-1);
	}
#endif

    return RT_ERR_RG_OK;
}

int32 rtk_rg_algApps_get(rtk_rg_alg_type_t *alg_app)
{
	*alg_app=rg_db.algFunctionMask;
    return RT_ERR_RG_OK;
}

//DMZ
int32 rtk_rg_dmzHost_set(int wan_intf_idx, rtk_rg_dmzInfo_t *dmz_info)
{
#ifdef CONFIG_RG_NAPT_DMZ_SUPPORT
	//DEBUG("Set DMZ[%d] IP[%x]\n",wan_intf_idx,dmz_info->private_ip);
	if((wan_intf_idx < 0) || (wan_intf_idx > MAX_NETIF_SW_TABLE_SIZE)) return RT_ERR_RG_INVALID_PARAM;

	rg_db.dmzInfo[wan_intf_idx].enabled = dmz_info->enabled;
	rg_db.dmzInfo[wan_intf_idx].mac_mapping_enabled = dmz_info->mac_mapping_enabled;
	if(dmz_info->mac_mapping_enabled)
		memcpy(&rg_db.dmzInfo[wan_intf_idx].mac.octet[0],&dmz_info->mac.octet[0],6);
	else
		rg_db.dmzInfo[wan_intf_idx].private_ip = dmz_info->private_ip;
#endif
	
    return RT_ERR_RG_OK;
}

int32 rtk_rg_dmzHost_get(int wan_intf_idx, rtk_rg_dmzInfo_t *dmz_info)
{
#ifdef CONFIG_RG_NAPT_DMZ_SUPPORT
	if((wan_intf_idx < 0) || (wan_intf_idx > MAX_NETIF_SW_TABLE_SIZE)) return RT_ERR_RG_INVALID_PARAM;
	
	dmz_info->enabled = rg_db.dmzInfo[wan_intf_idx].enabled;
	memcpy(&dmz_info->mac.octet[0],&rg_db.dmzInfo[wan_intf_idx].mac.octet[0],6);
	dmz_info->mac_mapping_enabled = rg_db.dmzInfo[wan_intf_idx].mac_mapping_enabled;
	dmz_info->private_ip = rg_db.dmzInfo[wan_intf_idx].private_ip;
#endif
	
    return RT_ERR_RG_OK;
}

//VirtualServer(PortForward)
int32 rtk_rg_virtualServer_add(rtk_rg_virtualServer_t *virtual_server, int *virtual_server_idx)
{
	int i=0;

	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

	// Get free entry
	for(i=0;i<MAX_VIRTUAL_SERVER_SW_TABLE_SIZE;i++)
	{
		if(rg_db.virtualServer[i].valid) continue;
		else break;
	}
	if(i==MAX_VIRTUAL_SERVER_SW_TABLE_SIZE) return RT_ERR_RG_ENTRY_FULL;

	// Add virtual server setting
	*virtual_server_idx = i;
	rg_db.virtualServer[i].is_tcp = virtual_server->is_tcp;
	rg_db.virtualServer[i].local_ip = virtual_server->local_ip;
	rg_db.virtualServer[i].local_port_start = virtual_server->local_port_start;
	rg_db.virtualServer[i].gateway_port_start = virtual_server->gateway_port_start;
	rg_db.virtualServer[i].mappingPortRangeCnt = virtual_server->mappingPortRangeCnt;
	rg_db.virtualServer[i].wan_intf_idx = virtual_server->wan_intf_idx;
	rg_db.virtualServer[i].valid = 1;

	rg_db.systemGlobal.virtualServerGroup[rg_db.systemGlobal.virtualServerTotalNum].index = *virtual_server_idx;
	rg_db.systemGlobal.virtualServerGroup[rg_db.systemGlobal.virtualServerTotalNum].p_virtualServer = &rg_db.virtualServer[i];
	rg_db.systemGlobal.virtualServerTotalNum++;
	
    return RT_ERR_RG_OK;
}

int32 rtk_rg_virtualServer_del(int virtual_server_idx)
{
	int i=0;
	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

	if( (virtual_server_idx<0) || (virtual_server_idx>=MAX_VIRTUAL_SERVER_SW_TABLE_SIZE)) return RT_ERR_RG_INVALID_PARAM;

	memset(&rg_db.virtualServer[virtual_server_idx],0,sizeof(rtk_rg_virtualServer_t));

	/* Virtual Server Group table */
	for(i=0;i<rg_db.systemGlobal.virtualServerTotalNum;i++)
	{
		if(rg_db.systemGlobal.virtualServerGroup[i].index == virtual_server_idx)
		{
			if(i==(rg_db.systemGlobal.virtualServerTotalNum-1))
			{
				//The last entry
				rg_db.systemGlobal.virtualServerGroup[i].index = 0;
				rg_db.systemGlobal.virtualServerGroup[i].p_virtualServer = NULL;
			}
			else
			{
				//Replace the deleteing entry by last one
				rg_db.systemGlobal.virtualServerGroup[i].index = rg_db.systemGlobal.virtualServerGroup[rg_db.systemGlobal.virtualServerTotalNum-1].index;
				rg_db.systemGlobal.virtualServerGroup[i].p_virtualServer = rg_db.systemGlobal.virtualServerGroup[rg_db.systemGlobal.virtualServerTotalNum-1].p_virtualServer;
				rg_db.systemGlobal.virtualServerGroup[rg_db.systemGlobal.virtualServerTotalNum-1].index = 0;
				rg_db.systemGlobal.virtualServerGroup[rg_db.systemGlobal.virtualServerTotalNum-1].p_virtualServer = NULL;
			}
			rg_db.systemGlobal.virtualServerTotalNum--;
			break;
		}
	}
	
    return RT_ERR_RG_OK;
}

int32 rtk_rg_virtualServer_find(rtk_rg_virtualServer_t *virtual_server, int *valid_idx)
{
	int idx=0;

	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

	if((virtual_server->wan_intf_idx < 0) || (virtual_server->wan_intf_idx >=MAX_EXTIP_HW_TABLE_SIZE)) return RT_ERR_RG_INVALID_PARAM;

	//Find server port mapping
	for(idx=*valid_idx;idx<MAX_VIRTUAL_SERVER_SW_TABLE_SIZE;idx++)
	{
		if(rg_db.virtualServer[idx].valid)
		{
			*valid_idx=idx;
			memcpy(virtual_server,&rg_db.virtualServer[idx],sizeof(rtk_rg_virtualServer_t));
			return RT_ERR_RG_OK;
		}
	}
    return RT_ERR_RG_SVRPORT_SW_ENTRY_NOT_FOUND;
}

/* Module Name    : ACL              */
//static rtk_rg_aclswEntry_t* acl_SW_table_entry=NULL;
//static rtk_rg_aclFilterAndQos_t* acl_filter_temp=NULL;

int _rtk_rg_search_acl_empty_Entry(int size, int* index)
{
    int i=0;
    int continue_size=0;
    rtk_acl_ingress_entry_t aclRule;
    bzero(&aclRule, sizeof(aclRule));

    if(size == 0) //no need acl entry
    {
        return RT_ERR_RG_OK;
    }
    for(i=MIN_ACL_ENTRY_INDEX; i<MAX_ACL_ENTRY_INDEX; i++)
    {
        aclRule.index = i;
        if(rtk_acl_igrRuleEntry_get(&aclRule))
        {
            debug("get acl[%d] failed",i);
            return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
        }
        if(aclRule.valid==DISABLED)
        {

            continue_size++;
            if(continue_size == size)
            {
                *index = ((i+1) - size);
                //debug("get empty entry[%d] continue_size=%d index=%d",i,continue_size,*index);

                return RT_ERR_RG_OK;
            }
            continue;
        }
        else
        {
            continue_size =0;
            continue;
        }
    }

    return RT_ERR_RG_ACL_ENTRY_FULL;
}
int _rtk_rg_search_cf_empty_Entry(int size,int* index)
{
    int i;
    rtk_classify_cfg_t cfRule;
    bzero(&cfRule, sizeof(cfRule));
    for(i=MIN_CF_ENTRY_SIZE; i<MAX_CF_ENTRY_SIZE; i++)
    {
        cfRule.index = i;
        if(rtk_classify_cfgEntry_get(&cfRule))
        {
            return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
        }
        if(size==1)
        {
            if(cfRule.valid==DISABLED)
            {
                *index = i;
                return RT_ERR_RG_OK;
            }
            else
            {
                continue;
            }
        }
        else if(size==2)
        {
            if(cfRule.valid==DISABLED)
            {
                cfRule.index = i+1;
                if(rtk_classify_cfgEntry_get(&cfRule))
                {
                    return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
                }
                if(cfRule.valid==DISABLED)
                {
                    *index = i;
                    return RT_ERR_RG_OK;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }

    }
    return RT_ERR_RG_CF_ENTRY_FULL;
}

int _rtk_rg_search_acl_empty_portTableEntry(int* index)
{
    int i;
    rtk_acl_rangeCheck_l4Port_t aclPortRangeEntry;
    bzero(&aclPortRangeEntry, sizeof(aclPortRangeEntry));

    for(i=0; i<MAX_ACL_PORTRANGETABLE_SIZE; i++)
    {
        aclPortRangeEntry.index = i;
        if(rtk_acl_portRange_get(&aclPortRangeEntry))
        {
            return RT_ERR_RG_ACL_PORTTABLE_ACCESS_FAILED;
        }
        else
        {
            if(aclPortRangeEntry.type==PORTRANGE_UNUSED && aclPortRangeEntry.upper_bound==0x0 && aclPortRangeEntry.lower_bound==0x0)
            {
                *index = i;
                return RT_ERR_RG_OK;
            }
            else
            {
                continue;
            }
        }
    }
    return RT_ERR_RG_ACL_PORTTABLE_FULL;

}


int _rtk_rg_search_acl_empty_ipTableEntry(int* index)
{
    int i;
    rtk_acl_rangeCheck_ip_t aclIpRangeEntry;
    bzero(&aclIpRangeEntry, sizeof(aclIpRangeEntry));
    for(i=0; i<MAX_ACL_IPRANGETABLE_SIZE; i++)
    {
        aclIpRangeEntry.index = i;
        if(rtk_acl_ipRange_get(&aclIpRangeEntry))
        {
            return RT_ERR_RG_ACL_IPTABLE_ACCESS_FAILED;
        }
        else
        {
            if(aclIpRangeEntry.type==IPRANGE_UNUSED && aclIpRangeEntry.upperIp==0x0 && aclIpRangeEntry.lowerIp==0x0)
            {
                *index = i;
                return RT_ERR_RG_OK;
            }
            else
            {
                continue;
            }
        }
    }
    return RT_ERR_RG_ACL_IPTABLE_FULL;

}

int _rtk_rg_search_cf_empty_portTableEntry(int* index)
{
    int i;
    rtk_classify_rangeCheck_l4Port_t cfPortRangeEntry;
    bzero(&cfPortRangeEntry, sizeof(cfPortRangeEntry));

    for(i=0; i<MAX_CF_PORTRANGETABLE_SIZE; i++)
    {
        cfPortRangeEntry.index= i;
        if(rtk_classify_portRange_get(&cfPortRangeEntry))
        {
            return RT_ERR_RG_CF_PORTTABLE_ACCESS_FAILED;
        }
        else
        {
            if(cfPortRangeEntry.type==0x0 && cfPortRangeEntry.lowerPort==0xffff && cfPortRangeEntry.upperPort==0x0)
            {
                *index = i;
                return RT_ERR_RG_OK;
            }
            else
            {
                continue;
            }
        }
    }
    return RT_ERR_RG_CF_PORTTABLE_FULL;

}

int _rtk_rg_search_cf_empty_ipTableEntry(int* index)
{
    int i;
    rtk_classify_rangeCheck_ip_t cfIpRangeEntry;
    bzero(&cfIpRangeEntry, sizeof(cfIpRangeEntry));

    for(i=0; i<MAX_CF_IPRANGETABLE_SIZE; i++)
    {
        cfIpRangeEntry.index= i;
        if(rtk_classify_ipRange_get(&cfIpRangeEntry))
        {
            return RT_ERR_RG_CF_IPTABLE_ACCESS_FAILED;
        }
        else
        {
            if(cfIpRangeEntry.type==0x0 && cfIpRangeEntry.lowerIp==0xffffffff && cfIpRangeEntry.upperIp==0x0)
            {
                *index = i;
                return RT_ERR_RG_OK;
            }
            else
            {
                continue;
            }
        }
    }
    return RT_ERR_RG_CF_IPTABLE_FULL;

}
int _rtk_rg_search_cf_empty_dscpTableEntry(int* index)
{
    int i,ret;
	rtk_dscp_t dscp;
    for(i=0; i<8; i++)
    {
        ret = rtk_classify_cfPri2Dscp_get(i,&dscp);
        if(ret!= RT_ERR_RG_OK)
            return RT_ERR_RG_CF_DSCPTABLE_ACCESS_FAILED;

        if(dscp == 0)
        {
            *index = i;
            return RT_ERR_RG_OK;
        }
    }
    return RT_ERR_RG_CF_DSCPTABLE_FULL;
}

	
int _rtk_rg_conflictField_and_flowDirection_check(rtk_rg_aclFilterAndQos_t* acl_filter, rtk_rg_flow_direction_t* flow_direction)
{

    int ingress_intf_idx=0, egress_intf_idx=0, i=0, ret=0;
    int ingress_intf_type=UNKNOW_INTF, egress_intf_type=UNKNOW_INTF;
    rtk_portmask_t mac_pmsk;
	rtk_portmask_t ext_pmsk;

	
	rtk_rg_intfInfo_t *ingress_intf_info, *egress_intf_info, *empty_intf_info;

    ingress_intf_info = (rtk_rg_intfInfo_t *)(unsigned long)rtk_rg_malloc(sizeof(rtk_rg_intfInfo_t));
    if(ingress_intf_info==NULL) return RT_ERR_RG_FAILED;
    bzero(ingress_intf_info,sizeof(rtk_rg_intfInfo_t));

    egress_intf_info = (rtk_rg_intfInfo_t *)(unsigned long)rtk_rg_malloc(sizeof(rtk_rg_intfInfo_t));
    if(egress_intf_info==NULL) return RT_ERR_RG_FAILED;
    bzero(egress_intf_info,sizeof(rtk_rg_intfInfo_t));

    empty_intf_info = (rtk_rg_intfInfo_t *)(unsigned long)rtk_rg_malloc(sizeof(rtk_rg_intfInfo_t));
    if(empty_intf_info==NULL) return RT_ERR_RG_FAILED;
    bzero(empty_intf_info,sizeof(rtk_rg_intfInfo_t));



	ASSERT_EQ(_rtk_rg_portmask_translator(acl_filter->ingress_port_mask, &mac_pmsk, &ext_pmsk),RT_ERR_RG_OK);


	//TCP & UDP can not enable at the same time
	if((acl_filter->filter_fields & INGRESS_L4_TCP_BIT) &&(acl_filter->filter_fields & INGRESS_L4_UDP_BIT))
		return RT_ERR_RG_ACL_CF_FIELD_CONFLICT;

    //ipv6 & ipv4  ip can not use at the same time
    if( ((acl_filter->filter_fields & INGRESS_IPV6_SIP_RANGE_BIT)||(acl_filter->filter_fields & INGRESS_IPV6_DIP_RANGE_BIT))&&
            ((acl_filter->filter_fields & INGRESS_IPV4_SIP_RANGE_BIT)||(acl_filter->filter_fields & EGRESS_IPV4_SIP_RANGE_BIT)||(acl_filter->filter_fields & INGRESS_IPV4_DIP_RANGE_BIT)||(acl_filter->filter_fields & EGRESS_IPV4_DIP_RANGE_BIT)))
    {   
    	debug("IPV4/IPV6 conflict");
		debug("INGRESS_IPV6_SIP_RANGE_BIT =0x%x",acl_filter->filter_fields & INGRESS_IPV6_SIP_RANGE_BIT);
		debug("INGRESS_IPV6_DIP_RANGE_BIT =0x%x",acl_filter->filter_fields & INGRESS_IPV6_DIP_RANGE_BIT);
		debug("INGRESS_IPV4_SIP_RANGE_BIT =0x%x",acl_filter->filter_fields & INGRESS_IPV4_SIP_RANGE_BIT);
		debug("EGRESS_IPV4_SIP_RANGE_BIT =0x%x",acl_filter->filter_fields & EGRESS_IPV4_SIP_RANGE_BIT);
		debug("INGRESS_IPV4_DIP_RANGE_BIT =0x%x",acl_filter->filter_fields & INGRESS_IPV4_DIP_RANGE_BIT);
		debug("EGRESS_IPV4_DIP_RANGE_BIT =0x%x",acl_filter->filter_fields & EGRESS_IPV4_DIP_RANGE_BIT);
        return RT_ERR_RG_ACL_CF_FIELD_CONFLICT;
    }
    //CF do not change the source destination (ip/port) at the same time(NAPT only change one side of src/dest)
    if( (acl_filter->filter_fields & EGRESS_IPV4_SIP_RANGE_BIT)&&(acl_filter->filter_fields & EGRESS_IPV4_DIP_RANGE_BIT))
        return RT_ERR_RG_ACL_CF_FIELD_CONFLICT;

    if( (acl_filter->filter_fields & EGRESS_L4_SPORT_RANGE_BIT)&&(acl_filter->filter_fields & EGRESS_L4_DPORT_RANGE_BIT))
        return RT_ERR_RG_ACL_CF_FIELD_CONFLICT;


    //ingress_netif CVID, DMAC, ingress_portmask should sync with acl_filter assigned!
    if(acl_filter->filter_fields & INGRESS_INTF_BIT)
    {
        ingress_intf_idx = acl_filter->ingress_intf_idx;
        ret = rtk_rg_intfInfo_find(ingress_intf_info, &ingress_intf_idx);
        if(ret!=RT_ERR_RG_OK)
        {
            return ret;
        }
        if(ingress_intf_idx != acl_filter->ingress_intf_idx)
        {
            //did not get the assigned interface
            return RT_ERR_RG_INTF_GET_FAIL;
        }

        if(ingress_intf_info->is_wan) //ingress interface is wan
        {

            //get ingress_netif is wan/lan
            ingress_intf_type = WAN_INTF;

            //Wan Interface
            if(ingress_intf_info->wan_intf.wan_intf_conf.wan_type == RTK_RG_BRIDGE)
            {
                //wan bridge(L2) mode: judge CVID
                if(ingress_intf_info->wan_intf.wan_intf_conf.egress_vlan_tag_on)
                {
                    if(acl_filter->filter_fields & INGRESS_CTAG_VID_BIT)
                    {
                        //prevent error setting: user set CVID is not same with ingress_wan_interface VID=> conflict!!!
                        if(acl_filter->ingress_ctag_vid!=ingress_intf_info->wan_intf.wan_intf_conf.egress_vlan_id)
                            return RT_ERR_RG_ACL_CF_FIELD_CONFLICT;
                    }
                }
                else
                {
                    //prevent error setting: user set CVID but infress_wan_interface do not need to tag on=> conflict!!!
                    if(acl_filter->filter_fields & INGRESS_CTAG_VID_BIT)
                        return RT_ERR_RG_ACL_CF_FIELD_CONFLICT;
                }
            }
            else
            {
                //wan route(L34) mode: judge CVID + DMAC
                if(ingress_intf_info->wan_intf.wan_intf_conf.egress_vlan_tag_on)
                {
                    if(acl_filter->filter_fields & INGRESS_CTAG_VID_BIT)
                    {
                        //prevent error setting: user set CVID is not same with ingress_wan_interface VID=> conflict!!!
                        if(acl_filter->ingress_ctag_vid!=ingress_intf_info->wan_intf.wan_intf_conf.egress_vlan_id)
                            return RT_ERR_RG_ACL_CF_FIELD_CONFLICT;
                    }

                    if(acl_filter->filter_fields & INGRESS_DMAC_BIT)
                    {
                        //prevent error setting: user set DMAC is not same with ingress_wan_interface gmac=> conflict!!!
                        if(memcmp(&(acl_filter->ingress_dmac),&(ingress_intf_info->wan_intf.wan_intf_conf.gmac),sizeof(rtk_mac_t)))
                            return RT_ERR_RG_ACL_CF_FIELD_CONFLICT;
                    }
                }
                else
                {
                    //prevent error setting: user set CVID but ingress_wan_interface do not need to tag on=> conflict!!!
                    if(acl_filter->filter_fields & INGRESS_CTAG_VID_BIT)
                        return RT_ERR_RG_ACL_CF_FIELD_CONFLICT;

                    if(acl_filter->filter_fields & INGRESS_DMAC_BIT)
                    {
                        //prevent error setting: user set DMAC is not same with ingress_wan_interface gmac=> conflict!!!
                        if(memcmp(&(acl_filter->ingress_dmac),&(ingress_intf_info->wan_intf.wan_intf_conf.gmac),sizeof(rtk_mac_t)))
                            return RT_ERR_RG_ACL_CF_FIELD_CONFLICT;
                    }
                }
            }
        }
        else//ingress interface is lan
        {
            //get ingress_netif is wan/lan
            ingress_intf_type = LAN_INTF;

            //prevent error setting: user set portmask is not in lan_intf.port_mask=> conflict!!!
            if(acl_filter->filter_fields & INGRESS_PORT_BIT)
            {
                for(i=0; i<RTK_RG_MAX_MAC_PORT; i++)
                {
                    if( ( ((mac_pmsk.bits[0]>>i)&0x1)==0x1)  &&  (((ingress_intf_info->lan_intf.port_mask.portmask>>i)&0x1)==0x0) )
                        return RT_ERR_RG_ACL_CF_FIELD_CONFLICT;
                }
            }
        }
    }



    if(acl_filter->filter_fields & EGRESS_INTF_BIT)
    {
        egress_intf_idx = acl_filter->egress_intf_idx;
        ret = rtk_rg_intfInfo_find(egress_intf_info, &egress_intf_idx);
        if(ret!=RT_ERR_RG_OK)
        {
            return ret;
        }
        if(egress_intf_idx != acl_filter->egress_intf_idx)
        {
            //did not get the assigned interface
            return RT_ERR_RG_INTF_GET_FAIL;
        }

        //get egress_netif is wan/lan
        if(egress_intf_info->is_wan)
        {
            egress_intf_type = WAN_INTF;
        }
        else
        {
            egress_intf_type = LAN_INTF;
        }
    }


    //judge the flow direction
    if(ingress_intf_type == LAN_INTF && egress_intf_type == WAN_INTF)
    {
        *flow_direction = FLOW_DIRECTION_UPSTREAM;
    }
    else if(ingress_intf_type == WAN_INTF && egress_intf_type == LAN_INTF)
    {
        *flow_direction = FLOW_DIRECTION_DOWNSTREAM;
    }
    else if(ingress_intf_type == LAN_INTF && egress_intf_type == LAN_INTF)
    {
        *flow_direction = FLOW_DIRECTION_LAN_TO_LAN;
    }
    else if(ingress_intf_type == WAN_INTF && egress_intf_type == WAN_INTF)
    {
        *flow_direction = FLOW_DIRECTION_WAN_TO_WAN;
    }
    else
    {
        *flow_direction = FLOW_DIRECTION_CAN_NOT_DECIDE;
    }

   	rtk_rg_free(ingress_intf_info);
	rtk_rg_free(egress_intf_info);
	rtk_rg_free(empty_intf_info);


    return RT_ERR_RG_OK;
}

int _rtk_rg_free_cf_portTableEntry(int index)
{
    rtk_classify_rangeCheck_l4Port_t cfPortRangeEntry;
    bzero(&cfPortRangeEntry,sizeof(cfPortRangeEntry));
    cfPortRangeEntry.index=index;
    cfPortRangeEntry.lowerPort = 0xffff;

    if( rtk_classify_portRange_set(&cfPortRangeEntry))
    {
        debug("free cf porttable failed");
        return RT_ERR_RG_CF_PORTTABLE_ACCESS_FAILED;
    }
    return RT_ERR_RG_OK;
}
int _rtk_rg_free_cf_ipTableEntry(int index)
{
    rtk_classify_rangeCheck_ip_t cfIpRangeEntry;
    bzero(&cfIpRangeEntry,sizeof(cfIpRangeEntry));
    cfIpRangeEntry.index=index;
    cfIpRangeEntry.lowerIp=0xffffffff;

    if( rtk_classify_ipRange_set(&cfIpRangeEntry))
    {
        debug("free cf iptable failed");
        return RT_ERR_RG_CF_IPTABLE_ACCESS_FAILED;
    }
    return RT_ERR_RG_OK;
}


int _rtk_rg_free_cf_dscpTableEntry(int index)
{
    int ret;
    ret = rtk_classify_cfPri2Dscp_set(index,0);
    if(ret != RT_ERR_RG_OK)
        return RT_ERR_RG_CF_DSCPTABLE_ACCESS_FAILED;

    return RT_ERR_RG_OK;
}



int _rtk_rg_free_acl_portTableEntry(int index)
{
	int ret;
    rtk_acl_rangeCheck_l4Port_t aclPortRangeEntry;
    bzero(&aclPortRangeEntry,sizeof(aclPortRangeEntry));
    aclPortRangeEntry.index=index;
	ret = rtk_acl_portRange_set(&aclPortRangeEntry);
    if(ret!= RT_ERR_RG_OK)
    {
        debug("free acl porttable failed, ret(rtk)=%d",ret);
        return RT_ERR_RG_ACL_PORTTABLE_ACCESS_FAILED;
    }
    return RT_ERR_RG_OK;
}

int _rtk_rg_free_acl_ipTableEntry(int index)
{
	int ret;
    rtk_acl_rangeCheck_ip_t aclIpRangeEntry;
    bzero(&aclIpRangeEntry,sizeof(aclIpRangeEntry));
    aclIpRangeEntry.index=index;
	ret = rtk_acl_ipRange_set(&aclIpRangeEntry);
    if(ret!= RT_ERR_RG_OK)
    {
        debug("free acl iptable failed, ret(rtk)=%d",ret);
        return RT_ERR_RG_ACL_IPTABLE_ACCESS_FAILED;
    }
    return RT_ERR_RG_OK;
}

int _rtk_rg_asic_defaultDropEntry_setup(void)
{

#if 0


    rtk_acl_ingress_entry_t aclRule;
#ifndef CONFIG_APOLLO_RLE0371	
    rtk_classify_cfg_t cfUsRule, cfDsRule;
#endif
    bzero(&aclRule,sizeof(aclRule));

#ifndef CONFIG_APOLLO_RLE0371
	bzero(&cfUsRule,sizeof(cfUsRule));
    bzero(&cfDsRule,sizeof(cfDsRule));
#endif

    aclRule.index = RESERVED_ACL_DEFAULT_ENTRY;
    aclRule.valid = ENABLE;
    aclRule.activePorts.bits[0] = RTK_RG_MAX_MAC_ALLPORTMASK;
    aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLE;
    aclRule.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
    aclRule.act.forwardAct.portMask.bits[0]= 0x0;


    if(rtk_acl_igrRuleEntry_add(&aclRule))
    {
        debug("adding default aclRule[%d] drop failed",RESERVED_ACL_DEFAULT_ENTRY);
        return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
    }

#ifndef CONFIG_APOLLO_RLE0371
    cfUsRule.index = RESERVED_CF_US_DEFAULT_ENTRY;
    cfUsRule.valid = ENABLE;
    cfUsRule.direction = CLASSIFY_DIRECTION_US;
    cfUsRule.act.usAct.drop = CLASSIFY_DROP_ACT_ENABLE;
    if(rtk_classify_cfgEntry_add(&cfUsRule))
    {
        debug("adding default cfUsEntry[%d] drop failed",RESERVED_CF_US_DEFAULT_ENTRY);
        return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
    }

    cfDsRule.index = RESERVED_CF_DS_DEFAULT_ENTRY;
    cfDsRule.valid = ENABLE;
    cfDsRule.direction = CLASSIFY_DIRECTION_DS;
    cfDsRule.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_FORCE_FORWARD; //DS drop
    cfDsRule.act.dsAct.uniMask.bits[0] = 0x0;
    if(rtk_classify_cfgEntry_add(&cfDsRule))
    {
        debug("adding default cfDsEntry[%d] drop failed",RESERVED_CF_DS_DEFAULT_ENTRY);
        return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
    }
#endif

#endif
    return RT_ERR_RG_OK;
}

int _rtk_rg_asic_defaultDropEntry_remove(void)
{
#if 0

    if(rtk_acl_igrRuleEntry_del(RESERVED_ACL_DEFAULT_ENTRY))
    {
        debug("remove default aclRule[%d] drop failed",RESERVED_ACL_DEFAULT_ENTRY);
        return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
    }
#ifndef CONFIG_APOLLO_RLE0371
    if(rtk_classify_cfgEntry_del(RESERVED_CF_US_DEFAULT_ENTRY))
    {
        debug("remove default cfUsEntry[%d] drop failed",RESERVED_CF_US_DEFAULT_ENTRY);
        return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
    }

    if(rtk_classify_cfgEntry_del(RESERVED_CF_DS_DEFAULT_ENTRY))
    {
        debug("remove default cfDsEntry[%d] drop failed",RESERVED_CF_DS_DEFAULT_ENTRY);
        return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
    }
#endif



#endif

    return RT_ERR_RG_OK;

}

int _rtk_rg_is_aclSWEntry_init(void)
{
    if(rg_db.systemGlobal.acl_SW_table_entry==NULL || rg_db.systemGlobal.acl_filter_temp==NULL)
        return RT_ERR_RG_NULL_POINTER;
    else
        return RT_ERR_RG_OK;
}

int _rtk_rg_aclSWEntry_init(void)
{

	if(rg_db.systemGlobal.acl_SW_table_entry!=NULL)
		rtk_rg_free(rg_db.systemGlobal.acl_SW_table_entry);

	
	if(rg_db.systemGlobal.acl_filter_temp!=NULL)
		rtk_rg_free(rg_db.systemGlobal.acl_filter_temp);
	
    rg_db.systemGlobal.acl_SW_table_entry = rtk_rg_malloc(sizeof(rtk_rg_aclFilterEntry_t)*MAX_ACL_ENTRY_SIZE);
    if(rg_db.systemGlobal.acl_SW_table_entry==NULL)
    {
        return RT_ERR_RG_NULL_POINTER;
    }
    else
    {
        bzero(rg_db.systemGlobal.acl_SW_table_entry, sizeof(rtk_rg_aclFilterEntry_t)*MAX_ACL_ENTRY_SIZE);
    }


    rg_db.systemGlobal.acl_filter_temp = rtk_rg_malloc(sizeof(rtk_rg_aclFilterAndQos_t)*MAX_ACL_ENTRY_SIZE);
    if(rg_db.systemGlobal.acl_filter_temp==NULL)
    {
        return RT_ERR_RG_NULL_POINTER;
    }
    else
    {
        bzero(rg_db.systemGlobal.acl_filter_temp, sizeof(rtk_rg_aclFilterAndQos_t)*MAX_ACL_ENTRY_SIZE);
    }

	rg_db.systemGlobal.acl_SW_table_entry_size=0;

    return RT_ERR_RG_OK;
}


int _rtk_rg_aclSWEntry_get(int index, rtk_rg_aclFilterEntry_t* aclSWEntry)
{
    //check the acl_SW_Entry has been allocate
    if(_rtk_rg_is_aclSWEntry_init())
    {
        if(_rtk_rg_aclSWEntry_init())
            return RT_ERR_RG_NULL_POINTER;
    }
    *aclSWEntry = rg_db.systemGlobal.acl_SW_table_entry[index];
    return RT_ERR_RG_OK;

}
int _rtk_rg_aclSWEntry_set(int index, rtk_rg_aclFilterEntry_t aclSWEntry)
{
    //check the acl_SW_Entry has been allocate
    if(_rtk_rg_is_aclSWEntry_init())
    {
        if(_rtk_rg_aclSWEntry_init())
            return RT_ERR_RG_NULL_POINTER;
    }

    rg_db.systemGlobal.acl_SW_table_entry[index] = aclSWEntry;
    return RT_ERR_RG_OK;
}

int _rtk_rg_aclSWEntry_empty_find(int* index)
{

    int i;
    rtk_rg_aclFilterEntry_t aclSWEntry, empty_aclSWEntry;
    bzero(&aclSWEntry, sizeof(aclSWEntry));
    bzero(&empty_aclSWEntry, sizeof(empty_aclSWEntry));


    //check the acl_SW_Entry has been allocate
    if(_rtk_rg_is_aclSWEntry_init())
    {
        if(_rtk_rg_aclSWEntry_init())
            return RT_ERR_RG_NULL_POINTER;
    }

    for(i=0; i<MAX_ACL_ENTRY_SIZE; i++)
    {
        if(_rtk_rg_aclSWEntry_get(i,&aclSWEntry))
            return RT_ERR_RG_ACL_SW_ENTRY_ACCESS_FAILED;

        if(!memcmp(&aclSWEntry,&empty_aclSWEntry,sizeof(rtk_rg_aclFilterEntry_t)))
        {
            *index = i;
            break;
        }

        //not found empty entry
        if(i==(MAX_ACL_ENTRY_SIZE-1))
            return RT_ERR_RG_ACL_SW_ENTRY_FULL;
    }

    return RT_ERR_RG_OK;

}

int _rtk_rg_aclSWEntry_free_asic(rtk_rg_aclFilterEntry_t aclSWEntry)
{


    int i,ret;
    int acl_start,acl_size,cf_start, cf_size;
    rtk_rg_aclFilterEntry_t empty_aclSWEntry;
    bzero(&empty_aclSWEntry,sizeof(empty_aclSWEntry));


    //delete acl&cf  tables
    if(aclSWEntry.hw_used_table & ACL_USED_IPTABLE_IPV4SIP)
        _rtk_rg_free_acl_ipTableEntry(aclSWEntry.hw_used_table_index[ACL_USED_IPTABLE_IPV4SIP_INDEX]);

    if(aclSWEntry.hw_used_table & ACL_USED_IPTABLE_IPV4DIP)
        _rtk_rg_free_acl_ipTableEntry(aclSWEntry.hw_used_table_index[ACL_USED_IPTABLE_IPV4DIP_INDEX]);

    if(aclSWEntry.hw_used_table & ACL_USED_IPTABLE_IPV6SIP)
        _rtk_rg_free_acl_ipTableEntry(aclSWEntry.hw_used_table_index[ACL_USED_IPTABLE_IPV6SIP_INDEX]);

    if(aclSWEntry.hw_used_table & ACL_USED_IPTABLE_IPV6DIP)
        _rtk_rg_free_acl_ipTableEntry(aclSWEntry.hw_used_table_index[ACL_USED_IPTABLE_IPV6DIP_INDEX]);

    if(aclSWEntry.hw_used_table & ACL_USED_PORTTABLE_SPORT)
        _rtk_rg_free_acl_portTableEntry(aclSWEntry.hw_used_table_index[ACL_USED_PORTTABLE_SPORT_INDEX]);

    if(aclSWEntry.hw_used_table & ACL_USED_PORTTABLE_DPORT)
        _rtk_rg_free_acl_portTableEntry(aclSWEntry.hw_used_table_index[ACL_USED_PORTTABLE_DPORT_INDEX]);

    if(aclSWEntry.hw_used_table & CF_USED_IPTABLE_IPV4SIP)
        _rtk_rg_free_cf_ipTableEntry(aclSWEntry.hw_used_table_index[CF_USED_IPTABLE_IPV4SIP_INDEX]);

    if(aclSWEntry.hw_used_table & CF_USED_IPTABLE_IPV4DIP)
        _rtk_rg_free_cf_ipTableEntry(aclSWEntry.hw_used_table_index[CF_USED_IPTABLE_IPV4DIP_INDEX]);

    if(aclSWEntry.hw_used_table & CF_USED_PORTTABLE_SPORT)
        _rtk_rg_free_cf_portTableEntry(aclSWEntry.hw_used_table_index[CF_USED_PORTTABLE_SPORT_INDEX]);

    if(aclSWEntry.hw_used_table & CF_USED_PORTTABLE_DPORT)
        _rtk_rg_free_cf_portTableEntry(aclSWEntry.hw_used_table_index[CF_USED_PORTTABLE_DPORT_INDEX]);

    if(aclSWEntry.hw_used_table & CF_USED_DSCPTABLE)
        _rtk_rg_free_cf_dscpTableEntry(aclSWEntry.hw_used_table_index[CF_USED_DSCPTABLE_INDEX]);

    //delete acl&cf Asic
    acl_start = aclSWEntry.hw_aclEntry_start;
    acl_size = aclSWEntry.hw_aclEntry_size;
    cf_start = aclSWEntry.hw_cfEntry_start;
    cf_size = aclSWEntry.hw_cfEntry_size;
    for(i=0; i<acl_size; i++)
    {
    	ret = rtk_acl_igrRuleEntry_del(acl_start+i);
        if(ret!=RT_ERR_RG_OK)
        {
        	debug("free acl ASIC[%d] failed, ret(rtk)=%d",(acl_start+i),ret);
            return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
        }
    }

    for(i=0; i<cf_size; i++)
    {
    	ret = rtk_classify_cfgEntry_del(cf_start+i);
        if(ret!=RT_ERR_RG_OK)
        {
        	debug("free cf ASIC[%d] failed, ret(rtk)=%d",(acl_start+i),ret);
            return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
        }
    }

    return RT_ERR_RG_OK;
}

int _rtk_rg_aclSWEntry_rearrange_asic(void)
{

    int i;
    rtk_rg_aclFilterEntry_t aclSWEntry,empty_aclSWEntry;
    rtk_rg_aclFilterAndQos_t empty_aclFilter;
    bzero(&aclSWEntry, sizeof(aclSWEntry));
    bzero(&empty_aclSWEntry, sizeof(rtk_rg_aclFilterEntry_t));
    bzero(&empty_aclFilter, sizeof(empty_aclFilter));
    bzero(rg_db.systemGlobal.acl_filter_temp, sizeof(rtk_rg_aclFilterAndQos_t)*MAX_ACL_ENTRY_SIZE);

    //backup all acl_filter for reAdd, and clean all aclSWEntry
    for(i=0; i<MAX_ACL_ENTRY_SIZE; i++)
    {
        //backup acl_filter
        ASSERT_EQ(_rtk_rg_aclSWEntry_get(i, &aclSWEntry),RT_ERR_RG_OK);
        rg_db.systemGlobal.acl_filter_temp[i] = aclSWEntry.acl_filter;
        //clean aclSWEntry
        ASSERT_EQ(_rtk_rg_aclSWEntry_set(i, empty_aclSWEntry),RT_ERR_RG_OK);
    }

    //delete ACL & CF tables
    for(i=0; i<MAX_ACL_ENTRY_SIZE; i++)
    {
        if(rtk_acl_igrRuleEntry_del(i))
        {
            return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
        }
    }
#ifndef CONFIG_APOLLO_RLE0371
    for(i=0; i<MAX_CF_ENTRY_SIZE; i++)
    {
        if(rtk_classify_cfgEntry_del(i))
        {
            return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
        }
    }
#endif
    for(i=0; i<MAX_ACL_IPRANGETABLE_SIZE; i++)
    {
        if(_rtk_rg_free_acl_ipTableEntry(i))
        {
            return RT_ERR_RG_ACL_IPTABLE_ACCESS_FAILED;
        }
    }
    for(i=0; i<MAX_ACL_PORTRANGETABLE_SIZE; i++)
    {
        if(_rtk_rg_free_acl_portTableEntry(i))
        {
            return RT_ERR_RG_ACL_PORTTABLE_ACCESS_FAILED;
        }
    }
#ifndef CONFIG_APOLLO_RLE0371
    for(i=0; i<MAX_CF_IPRANGETABLE_SIZE; i++)
    {
        if(_rtk_rg_free_cf_ipTableEntry(i))
        {
            return RT_ERR_RG_CF_IPTABLE_ACCESS_FAILED;
        }
    }

    for(i=0; i<MAX_CF_PORTRANGETABLE_SIZE; i++)
    {
        if(_rtk_rg_free_cf_portTableEntry(i))
        {
            return RT_ERR_RG_CF_PORTTABLE_ACCESS_FAILED;
        }
    }
    for(i=0; i<MAX_CF_DSCPTABLE_SIZE; i++)
    {
        if(_rtk_rg_free_cf_dscpTableEntry(i))
        {
            return RT_ERR_RG_CF_DSCPTABLE_ACCESS_FAILED;
        }
    }
#endif
    //the default drop asic entry will be  readd, if there are any permit aclSWEntry
    ASSERT_EQ(_rtk_rg_asic_defaultDropEntry_remove(),RT_ERR_RG_OK);

    //reAdd all reserve aclSWEntry
    for(i=0; i<MAX_ACL_ENTRY_SIZE; i++)
    {
        if(memcmp(&rg_db.systemGlobal.acl_filter_temp[i],&empty_aclFilter,sizeof(empty_aclFilter)))
        {
            ASSERT_EQ(_rtk_rg_aclSWEntry_reAdd(&rg_db.systemGlobal.acl_filter_temp[i], &i),RT_ERR_RG_OK);
        }
    }

    return RT_ERR_RG_OK;
}


int _rtk_rg_maskLength_get_by_ipv6_range(uint8* ipv6_start, uint8* ipv6_end, int* maskLength){
	int i,j,k;
	int length;

	if(ipv6_start[0]==ipv6_end[0] && ipv6_start[1]==ipv6_end[1] &&
		ipv6_start[2]==ipv6_end[2] && ipv6_start[3]==ipv6_end[3] &&
		ipv6_start[4]==ipv6_end[4] && ipv6_start[5]==ipv6_end[5] &&
		ipv6_start[6]==ipv6_end[6] && ipv6_start[7]==ipv6_end[7] &&
		ipv6_start[8]==ipv6_end[8] && ipv6_start[9]==ipv6_end[9] &&
		ipv6_start[10]==ipv6_end[10] && ipv6_start[11]==ipv6_end[11] &&
		ipv6_start[12]==ipv6_end[12] && ipv6_start[13]==ipv6_end[13] &&
		ipv6_start[14]==ipv6_end[14] && ipv6_start[15]==ipv6_end[15]){
		//SINGLE IP
		*maskLength = 0;

	}else{ //RANGE IP
		for(i=0;i<16;i++){
			if(memcmp(&ipv6_start[i],&ipv6_end[i],1)){//i: get the first different byte
				break;
			}
		}
		for(j=0;j<8;j++){ // j: get the first different bit of the byte
			if(((ipv6_start[i]<<j)&0x80) != ((ipv6_end[i]<<j)&0x80))
				break;
		}
		length = (i*8)+j;
		(*maskLength) = 128 - length;


		

		/*make sure the IP range is mask aligned*/
		for(k=0;k<8;k++){
			if(k>j){
				if((((ipv6_start[i]<<k)&0x80)!=0x0)){rtlglue_printf("IP Range not in mask alignment(1) \n"); return RT_ERR_RG_FAILED;}//rest bit should be 0
				if((((ipv6_end[i]<<k)&0x80)!=0x80)){rtlglue_printf("IP Range not in mask alignment(2) \n"); return RT_ERR_RG_FAILED;}//rest bit should be 1
			}
		}
		
		for(k=i+1;k<16;k++){
			if(ipv6_start[k]!=0x0){rtlglue_printf("IP Range not in mask alignment(3) \n"); return RT_ERR_RG_FAILED;}//rest bytes should be 0x00
			if(ipv6_end[k]!=0xff){rtlglue_printf("IP Range not in mask alignment(4) \n"); return RT_ERR_RG_FAILED;}//rest bytes should be 0xff
		}
	}
	return RT_ERR_RG_OK;
	
}

int _rtk_rg_maskLength_get_by_ipv4_range(ipaddr_t ipv4_addr_start, ipaddr_t ipv4_addr_end, int* maskLength){

	int i;
	int length=0;
	
	for(i=0;i<32;i++){
		if(((ipv4_addr_start<<i)&0x80000000)!=((ipv4_addr_end<<i)&0x80000000)){
			length = i;
			break;
		}
	}

	//check rest bit in range
	for(i=length;i<32;i++){
		if(((ipv4_addr_start<<i)&0x80000000)!=0){rtlglue_printf("IP Range not in mask alignment \n"); return RT_ERR_RG_FAILED;}
		if(((ipv4_addr_end<<i)&0x80000000)!=0x80000000){rtlglue_printf("IP Range not in mask alignment \n"); return RT_ERR_RG_FAILED;}
	}
	
	*maskLength = (32-length);
	
	return RT_ERR_RG_OK;
	
}


//ysleu: for linux-2.6.30, warning: the frame size of 1808 bytes is larger than 1024 bytes
int flag_add_aclRule[MAX_ACL_TEMPLATE_SIZE]= {DISABLE};
rtk_acl_ingress_entry_t* aclRule;
rtk_acl_field_t* aclField;
rtk_acl_igr_act_t acl_action;
rtk_acl_field_t empty_aclField;
rtk_classify_field_t* classifyField_1;
rtk_classify_field_t* classifyField_2;
rtk_classify_cfg_t cfRule_1, cfRule_2; //cfRule_1 for upstream. cfRule_2 for downstream
rtk_classify_field_t empty_classifyField;
rtk_rg_aclFilterEntry_t empty_aclSWEntry;
rtk_acl_rangeCheck_ip_t aclSIPv4RangeEntry,aclDIPv4RangeEntry,aclSIPv6RangeEntry,aclDIPv6RangeEntry;
rtk_acl_rangeCheck_l4Port_t aclSportRangeEntry,aclDportRangeEntry;
rtk_classify_rangeCheck_ip_t cfIpRangeEntry;
rtk_classify_rangeCheck_l4Port_t cfPortRangeEntry;
rtk_rg_intfInfo_t ingress_intf_info, egress_intf_info;
rtk_rg_flow_direction_t flow_direction;
rtk_classify_field_t pClassifyField_acl;
int _rtk_rg_aclSWEntry_and_asic_add(rtk_rg_aclFilterAndQos_t *acl_filter,rtk_rg_aclFilterEntry_t* aclSWEntry)
{
    int i=0, j=0;
    int table_index=0;
    int acl_entry_index=0,acl_entry_index_tmp=0;
    int acl_entry_size=0;
    int flag_acl_first_entry;
    int cf_entry_index=0;
    int flag_add_cfRule=DISABLE;
    int ingress_intf_idx=0, egress_intf_idx=0;
	int ipv6_unmask_length=0;
	int ipv4_unmask_length=0;
	int flag_ipv6_sip_need_to_trap=DISABLE;
	int flag_ipv6_dip_need_to_trap=DISABLE;
	


	rtk_portmask_t mac_pmsk;
	rtk_portmask_t ext_pmsk;
	ASSERT_EQ(_rtk_rg_portmask_translator(acl_filter->ingress_port_mask, &mac_pmsk, &ext_pmsk),RT_ERR_RG_OK);

	for(i=0;i<MAX_ACL_TEMPLATE_SIZE;i++){
		flag_add_aclRule[i]=DISABLE;
	}


    //rtk_acl_ingress_entry_t aclRule[MAX_ACL_TEMPLATE_SIZE];
    aclRule = (rtk_acl_ingress_entry_t *)(unsigned long)rtk_rg_malloc(MAX_ACL_TEMPLATE_SIZE*sizeof(rtk_acl_ingress_entry_t));
    if(aclRule==NULL) return RT_ERR_RG_FAILED;
    bzero(aclRule,sizeof(rtk_acl_ingress_entry_t)*MAX_ACL_TEMPLATE_SIZE);


    //rtk_acl_field_t aclField[RTK_MAX_NUM_OF_ACL_RULE_FIELD*MAX_ACL_TEMPLATE_SIZE];//0~7 for aclRule[0], 8~15 for aclRule[1],16~23 for aclRule[2],24~31 for aclRule[3]
    aclField = (rtk_acl_field_t *)(unsigned long)rtk_rg_malloc(RTK_MAX_NUM_OF_ACL_RULE_FIELD*MAX_ACL_TEMPLATE_SIZE*sizeof(rtk_acl_field_t));
    if(aclField==NULL) return RT_ERR_RG_FAILED;
    bzero(aclField,sizeof(rtk_acl_field_t)*RTK_MAX_NUM_OF_ACL_RULE_FIELD*MAX_ACL_TEMPLATE_SIZE);


    //rtk_classify_field_t classifyField_1[CLASSIFY_FIELD_END];
    classifyField_1=(rtk_classify_field_t *)(unsigned long)rtk_rg_malloc(CLASSIFY_FIELD_END*sizeof(rtk_classify_field_t));
    if(classifyField_1==NULL) return RT_ERR_RG_FAILED;
    bzero(classifyField_1, CLASSIFY_FIELD_END*sizeof(rtk_classify_field_t));


    //rtk_classify_field_t classifyField_2[CLASSIFY_FIELD_END];
    classifyField_2=(rtk_classify_field_t *)(unsigned long)rtk_rg_malloc(CLASSIFY_FIELD_END*sizeof(rtk_classify_field_t));
    if(classifyField_2==NULL) return RT_ERR_RG_FAILED;
    bzero(classifyField_2, CLASSIFY_FIELD_END*sizeof(rtk_classify_field_t));

    //for(i=0; i<MAX_ACL_TEMPLATE_SIZE; i++)
    //bzero(&aclRule[i], sizeof(aclRule[i]));

    bzero(&acl_action,sizeof(acl_action));
    bzero(&empty_aclField,sizeof(empty_aclField));
    //bzero(aclField,sizeof(rtk_acl_field_t)*RTK_MAX_NUM_OF_ACL_RULE_FIELD*MAX_ACL_TEMPLATE_SIZE);
    bzero(&cfRule_1,sizeof(cfRule_1));
    bzero(&cfRule_2,sizeof(cfRule_2));
    bzero(&empty_classifyField,sizeof(empty_classifyField));

    for(i=0; i<CLASSIFY_FIELD_END; i++)
    {
        //bzero(&classifyField_1[i],sizeof(classifyField_1[i]));
        //bzero(&classifyField_2[i],sizeof(classifyField_2[i]));
    }
    bzero(&empty_aclSWEntry,sizeof(empty_aclSWEntry));

    bzero(&aclSIPv4RangeEntry, sizeof(aclSIPv4RangeEntry));
    bzero(&aclDIPv4RangeEntry, sizeof(aclDIPv4RangeEntry));
    bzero(&aclSIPv6RangeEntry, sizeof(aclSIPv6RangeEntry));
    bzero(&aclDIPv6RangeEntry, sizeof(aclDIPv6RangeEntry));
    bzero(&aclSportRangeEntry, sizeof(aclSportRangeEntry));
    bzero(&aclDportRangeEntry, sizeof(aclDportRangeEntry));
    bzero(&cfIpRangeEntry, sizeof(cfIpRangeEntry));
    bzero(&cfPortRangeEntry, sizeof(cfPortRangeEntry));

    bzero(&ingress_intf_info, sizeof(ingress_intf_info));
    bzero(&egress_intf_info, sizeof(egress_intf_info));
    bzero(&flow_direction, sizeof(flow_direction));

    //debug("acl_filter->filter_fields = %x",acl_filter->filter_fields);
    //check the acl conflict field & get flow direction!
    ASSERT_EQ(_rtk_rg_conflictField_and_flowDirection_check(acl_filter, &flow_direction),RT_ERR_RG_OK);

    if(acl_filter->filter_fields & INGRESS_INTF_BIT)
    {
        //use intf idex to get  Datastruct(for check is_wan or is_lan)
        ingress_intf_idx = acl_filter->ingress_intf_idx;
        ASSERT_EQ(rtk_rg_intfInfo_find(&ingress_intf_info, &ingress_intf_idx),RT_ERR_RG_OK);

        if(ingress_intf_info.is_wan)
        {
            //Wan Interface
            if(ingress_intf_info.wan_intf.wan_intf_conf.wan_type == RTK_RG_BRIDGE)//wan bridge(L2) mode
            {

                if(ingress_intf_info.wan_intf.wan_intf_conf.egress_vlan_tag_on)
                {
                    //this bridge wan have ctag => pkt must have ctag & compare CVID
                    for(i=0; i<MAX_ACL_TEMPLATE_SIZE; i++)
                    {
                        aclRule[i].careTag.tags[ACL_CARE_TAG_CTAG].value = 1;
                        aclRule[i].careTag.tags[ACL_CARE_TAG_CTAG].mask = 0xffff;
                    }
                    //setup Ctag
                    aclField[TEMPLATE_CTAG].fieldType = ACL_FIELD_PATTERN_MATCH;
                    aclField[TEMPLATE_CTAG].fieldUnion.pattern.fieldIdx = TEMPLATE_CTAG;
                    aclField[TEMPLATE_CTAG].fieldUnion.data.value = ingress_intf_info.wan_intf.wan_intf_conf.egress_vlan_id;
                    aclField[TEMPLATE_CTAG].fieldUnion.data.mask = 0x0fff; //do not compare with CPRI[15:3] & CFI[12]

                }
                else
                {
                    //this bridge wan do not have ctag =>  pkt must must not have ctag
                    for(i=0; i<MAX_ACL_TEMPLATE_SIZE; i++)
                    {
                        aclRule[i].careTag.tags[ACL_CARE_TAG_CTAG].value = 0;
                        aclRule[i].careTag.tags[ACL_CARE_TAG_CTAG].mask = 0xffff;
                    }
                }


            }
            else //wan route(L34) mode
            {

                if(ingress_intf_info.wan_intf.wan_intf_conf.egress_vlan_tag_on)
                {
                    //this bridge wan have ctag
                    for(i=0; i<MAX_ACL_TEMPLATE_SIZE; i++)
                    {
                        aclRule[i].careTag.tags[ACL_CARE_TAG_CTAG].value = 1;
                        aclRule[i].careTag.tags[ACL_CARE_TAG_CTAG].mask = 0xffff;
                    }

                    //setup Ctag
                    aclField[TEMPLATE_CTAG].fieldType = ACL_FIELD_PATTERN_MATCH;
                    aclField[TEMPLATE_CTAG].fieldUnion.pattern.fieldIdx = TEMPLATE_CTAG;
                    aclField[TEMPLATE_CTAG].fieldUnion.data.value = ingress_intf_info.wan_intf.wan_intf_conf.egress_vlan_id;
                    aclField[TEMPLATE_CTAG].fieldUnion.data.mask = 0x0fff; //do not compare with CPRI[15:3] & CFI[12]


                    //setup gmac
                    aclField[TEMPLATE_DMAC0].fieldType = ACL_FIELD_PATTERN_MATCH;
                    aclField[TEMPLATE_DMAC0].fieldUnion.pattern.fieldIdx = TEMPLATE_DMAC0;
                    aclField[TEMPLATE_DMAC0].fieldUnion.data.value = (ingress_intf_info.wan_intf.wan_intf_conf.gmac.octet[4]<<8) | (ingress_intf_info.wan_intf.wan_intf_conf.gmac.octet[5]);
                    aclField[TEMPLATE_DMAC0].fieldUnion.data.mask = 0xffff;

                    aclField[TEMPLATE_DMAC1].fieldType = ACL_FIELD_PATTERN_MATCH;
                    aclField[TEMPLATE_DMAC1].fieldUnion.pattern.fieldIdx = TEMPLATE_DMAC1;
                    aclField[TEMPLATE_DMAC1].fieldUnion.data.value = (ingress_intf_info.wan_intf.wan_intf_conf.gmac.octet[2]<<8) | (ingress_intf_info.wan_intf.wan_intf_conf.gmac.octet[3]);
                    aclField[TEMPLATE_DMAC1].fieldUnion.data.mask = 0xffff;

                    aclField[TEMPLATE_DMAC2].fieldType = ACL_FIELD_PATTERN_MATCH;
                    aclField[TEMPLATE_DMAC2].fieldUnion.pattern.fieldIdx = TEMPLATE_DMAC2;
                    aclField[TEMPLATE_DMAC2].fieldUnion.data.value = (ingress_intf_info.wan_intf.wan_intf_conf.gmac.octet[0]<<8) | (ingress_intf_info.wan_intf.wan_intf_conf.gmac.octet[1]);
                    aclField[TEMPLATE_DMAC2].fieldUnion.data.mask = 0xffff;

                }
                else
                {
                    //this bridge wan do not have ctag
                    for(i=0; i<MAX_ACL_TEMPLATE_SIZE; i++)
                    {
                        aclRule[i].careTag.tags[ACL_CARE_TAG_CTAG].value = 0;
                        aclRule[i].careTag.tags[ACL_CARE_TAG_CTAG].mask = 0xffff;
                    }

                    //setup gmac
                    aclField[TEMPLATE_DMAC0].fieldType = ACL_FIELD_PATTERN_MATCH;
                    aclField[TEMPLATE_DMAC0].fieldUnion.pattern.fieldIdx = TEMPLATE_DMAC0;
                    aclField[TEMPLATE_DMAC0].fieldUnion.data.value = (ingress_intf_info.wan_intf.wan_intf_conf.gmac.octet[4]<<8) | (ingress_intf_info.wan_intf.wan_intf_conf.gmac.octet[5]);
                    aclField[TEMPLATE_DMAC0].fieldUnion.data.mask = 0xffff;

                    aclField[TEMPLATE_DMAC1].fieldType = ACL_FIELD_PATTERN_MATCH;
                    aclField[TEMPLATE_DMAC1].fieldUnion.pattern.fieldIdx = TEMPLATE_DMAC1;
                    aclField[TEMPLATE_DMAC1].fieldUnion.data.value = (ingress_intf_info.wan_intf.wan_intf_conf.gmac.octet[2]<<8) | (ingress_intf_info.wan_intf.wan_intf_conf.gmac.octet[3]);
                    aclField[TEMPLATE_DMAC1].fieldUnion.data.mask = 0xffff;

                    aclField[TEMPLATE_DMAC2].fieldType = ACL_FIELD_PATTERN_MATCH;
                    aclField[TEMPLATE_DMAC2].fieldUnion.pattern.fieldIdx = TEMPLATE_DMAC2;
                    aclField[TEMPLATE_DMAC2].fieldUnion.data.value = (ingress_intf_info.wan_intf.wan_intf_conf.gmac.octet[0]<<8) | (ingress_intf_info.wan_intf.wan_intf_conf.gmac.octet[1]);
                    aclField[TEMPLATE_DMAC2].fieldUnion.data.mask = 0xffff;
                }

            }

        }
        else
        {
            //lan Interface  => lan intf by judge active port
            for(i=0; i<MAX_ACL_TEMPLATE_SIZE; i++)
            {
            
            	for(j=0;j<RTK_RG_MAX_MAC_ALLPORTMASK;j++){
					if((ingress_intf_info.lan_intf.port_mask.portmask&(1<<j)))
					{
						aclRule[i].activePorts.bits[0]|=(1<<j);
					}
				}
            }
        }
    }


    //assigned INGRESS_PORT is prior than INGRESS_INTF related ports
    if(acl_filter->filter_fields & INGRESS_PORT_BIT)
    {
    	//add active port in all related(0~3) aclRule
        for(i=0; i<MAX_ACL_TEMPLATE_SIZE; i++)
        {
            aclRule[i].activePorts = mac_pmsk;			
        }
		//if there are any ext_port add ext_port pattern
		if(ext_pmsk.bits[0]){	
			aclField[TEMPLATE_EXTPORTMASK].fieldType = ACL_FIELD_PATTERN_MATCH;
			aclField[TEMPLATE_EXTPORTMASK].fieldUnion.pattern.fieldIdx = TEMPLATE_EXTPORTMASK;
			aclField[TEMPLATE_EXTPORTMASK].fieldUnion.data.value = 0x0;
			for(i=0;i<RTK_RG_MAX_EXT_PORT;i++){
				if(!(ext_pmsk.bits[0]&(1<<i))){
					aclField[TEMPLATE_EXTPORTMASK].fieldUnion.data.mask |= (1<<i); //band not allowed ext_port										
				}
			}
		}
    }else{
    	//default should enable all port (acll EXT_PORT are include in CPU_PORT)
        for(i=0; i<MAX_ACL_TEMPLATE_SIZE; i++)
        {
            aclRule[i].activePorts.bits[0] = RTK_RG_MAX_MAC_ALLPORTMASK;//(1<<APOLLO_PORT0)|(1<<APOLLO_PORT1)|(1<<APOLLO_PORT2)|(1<<APOLLO_PORT3)|(1<<APOLLO_PORT_PON)|(1<<APOLLO_PORT_RGMII)|(1<<APOLLO_PORT_CPU);
        }
    }

    if(acl_filter->filter_fields & EGRESS_INTF_BIT)
    {
        egress_intf_idx = acl_filter->egress_intf_idx;
        ASSERT_EQ(rtk_rg_intfInfo_find(&egress_intf_info, &egress_intf_idx),RT_ERR_RG_OK);

        classifyField_1[CLASSIFY_FIELD_WAN_IF].fieldType =CLASSIFY_FIELD_WAN_IF;
        classifyField_1[CLASSIFY_FIELD_WAN_IF].classify_pattern.fieldData.value=acl_filter->egress_intf_idx;
        classifyField_1[CLASSIFY_FIELD_WAN_IF].classify_pattern.fieldData.mask=0x7;

        classifyField_2[CLASSIFY_FIELD_WAN_IF].fieldType =CLASSIFY_FIELD_WAN_IF;
        classifyField_2[CLASSIFY_FIELD_WAN_IF].classify_pattern.fieldData.value=acl_filter->egress_intf_idx;
        classifyField_2[CLASSIFY_FIELD_WAN_IF].classify_pattern.fieldData.mask=0x7;

    }
    if(acl_filter->filter_fields & INGRESS_ETHERTYPE_BIT)
    {
        aclField[TEMPLATE_ETHERTYPE].fieldType = ACL_FIELD_PATTERN_MATCH;
        aclField[TEMPLATE_ETHERTYPE].fieldUnion.pattern.fieldIdx = TEMPLATE_ETHERTYPE;
        aclField[TEMPLATE_ETHERTYPE].fieldUnion.data.value = acl_filter->ingress_ethertype;
        aclField[TEMPLATE_ETHERTYPE].fieldUnion.data.mask = 0xffff;
    }
    if((acl_filter->filter_fields & INGRESS_CTAG_VID_BIT) || (acl_filter->filter_fields & INGRESS_CTAG_PRI_BIT))
    {
        aclField[TEMPLATE_CTAG].fieldType = ACL_FIELD_PATTERN_MATCH;
        aclField[TEMPLATE_CTAG].fieldUnion.pattern.fieldIdx = TEMPLATE_CTAG;

        if(acl_filter->filter_fields & INGRESS_CTAG_VID_BIT)
        {
            aclField[TEMPLATE_CTAG].fieldUnion.data.value |= (acl_filter->ingress_ctag_vid);
            aclField[TEMPLATE_CTAG].fieldUnion.data.mask |= 0x0fff;//do not care CFI[13]
        }

        if(acl_filter->filter_fields & INGRESS_CTAG_PRI_BIT)
        {
            aclField[TEMPLATE_CTAG].fieldUnion.data.value |= ((acl_filter->ingress_ctag_pri)<<13);
            aclField[TEMPLATE_CTAG].fieldUnion.data.mask |= 0xe000;
        }
    }

    if(acl_filter->filter_fields & INGRESS_SMAC_BIT)
    {
        aclField[TEMPLATE_SMAC0].fieldType = ACL_FIELD_PATTERN_MATCH;
        aclField[TEMPLATE_SMAC0].fieldUnion.pattern.fieldIdx = TEMPLATE_SMAC0;
        aclField[TEMPLATE_SMAC0].fieldUnion.data.value =((acl_filter->ingress_smac.octet[4]<<8) | (acl_filter->ingress_smac.octet[5]));
        aclField[TEMPLATE_SMAC0].fieldUnion.data.mask = 0xffff;

        aclField[TEMPLATE_SMAC1].fieldType = ACL_FIELD_PATTERN_MATCH;
        aclField[TEMPLATE_SMAC1].fieldUnion.pattern.fieldIdx = TEMPLATE_SMAC1;
        aclField[TEMPLATE_SMAC1].fieldUnion.data.value =((acl_filter->ingress_smac.octet[2]<<8) | (acl_filter->ingress_smac.octet[3]));
        aclField[TEMPLATE_SMAC1].fieldUnion.data.mask = 0xffff;

        aclField[TEMPLATE_SMAC2].fieldType = ACL_FIELD_PATTERN_MATCH;
        aclField[TEMPLATE_SMAC2].fieldUnion.pattern.fieldIdx = TEMPLATE_SMAC2;
        aclField[TEMPLATE_SMAC2].fieldUnion.data.value =((acl_filter->ingress_smac.octet[0]<<8) | (acl_filter->ingress_smac.octet[1]));
        aclField[TEMPLATE_SMAC2].fieldUnion.data.mask = 0xffff;

    }
    if(acl_filter->filter_fields & INGRESS_DMAC_BIT)
    {

        aclField[TEMPLATE_DMAC0].fieldType = ACL_FIELD_PATTERN_MATCH;
        aclField[TEMPLATE_DMAC0].fieldUnion.pattern.fieldIdx = TEMPLATE_DMAC0;
        aclField[TEMPLATE_DMAC0].fieldUnion.data.value =((acl_filter->ingress_dmac.octet[4]<<8) | (acl_filter->ingress_dmac.octet[5]));
        aclField[TEMPLATE_DMAC0].fieldUnion.data.mask = 0xffff;

        aclField[TEMPLATE_DMAC1].fieldType = ACL_FIELD_PATTERN_MATCH;
        aclField[TEMPLATE_DMAC1].fieldUnion.pattern.fieldIdx = TEMPLATE_DMAC1;
        aclField[TEMPLATE_DMAC1].fieldUnion.data.value =((acl_filter->ingress_dmac.octet[2]<<8) | (acl_filter->ingress_dmac.octet[3]));
        aclField[TEMPLATE_DMAC1].fieldUnion.data.mask = 0xffff;

        aclField[TEMPLATE_DMAC2].fieldType = ACL_FIELD_PATTERN_MATCH;
        aclField[TEMPLATE_DMAC2].fieldUnion.pattern.fieldIdx = TEMPLATE_DMAC2;
        aclField[TEMPLATE_DMAC2].fieldUnion.data.value =((acl_filter->ingress_dmac.octet[0]<<8) | (acl_filter->ingress_dmac.octet[1]));
        aclField[TEMPLATE_DMAC2].fieldUnion.data.mask = 0xffff;
    }
    if(acl_filter->filter_fields & INGRESS_DSCP_BIT)
    {

        aclField[TEMPLATE_FIELDSELECT04].fieldType = ACL_FIELD_PATTERN_MATCH;
        aclField[TEMPLATE_FIELDSELECT04].fieldUnion.pattern.fieldIdx = TEMPLATE_FIELDSELECT04;
        aclField[TEMPLATE_FIELDSELECT04].fieldUnion.data.value = (acl_filter->ingress_dscp)<<2;
        aclField[TEMPLATE_FIELDSELECT04].fieldUnion.data.mask = 0x00fc;
    }
    if(acl_filter->filter_fields & INGRESS_L4_TCP_BIT)
    {
        for(i=0; i<MAX_ACL_TEMPLATE_SIZE; i++)
        {
            aclRule[i].careTag.tags[ACL_CARE_TAG_TCP].value=1;
            aclRule[i].careTag.tags[ACL_CARE_TAG_TCP].mask=0xffff;
        }

    }
    if(acl_filter->filter_fields & INGRESS_L4_UDP_BIT)
    {
        for(i=0; i<MAX_ACL_TEMPLATE_SIZE; i++)
        {
            aclRule[i].careTag.tags[ACL_CARE_TAG_UDP].value=1;
            aclRule[i].careTag.tags[ACL_CARE_TAG_UDP].mask=0xffff;
        }
    }
    if(acl_filter->filter_fields & INGRESS_L4_ICMP_BIT)
    {
		aclField[TEMPLATE_FIELDSELECT14].fieldType = ACL_FIELD_PATTERN_MATCH;
		aclField[TEMPLATE_FIELDSELECT14].fieldUnion.pattern.fieldIdx = TEMPLATE_FIELDSELECT14;
		aclField[TEMPLATE_FIELDSELECT14].fieldUnion.data.value =0x01; //ICMP protocal
		aclField[TEMPLATE_FIELDSELECT14].fieldUnion.data.mask =0xff;//mask 8 bit only
    }

    if(acl_filter->filter_fields & INGRESS_IPV6_SIP_RANGE_BIT)
    {
    	ipv6_unmask_length = 0;
		ASSERT_EQ(_rtk_rg_maskLength_get_by_ipv6_range(acl_filter->ingress_src_ipv6_addr_start,acl_filter->ingress_src_ipv6_addr_end,&ipv6_unmask_length),RT_ERR_RG_OK);
		if(ipv6_unmask_length > 112){
			rtlglue_printf("ipv6 src range too wide (only supported maskLength less than 112bit)!!!\n");
			return RT_ERR_RG_INVALID_PARAM;
		}

		if(ipv6_unmask_length ==0){
			//compare SIPv6[31:0]
			
			aclField[TEMPLATE_IPv6SIP0].fieldType = ACL_FIELD_PATTERN_MATCH;
			aclField[TEMPLATE_IPv6SIP0].fieldUnion.pattern.fieldIdx = TEMPLATE_IPv6SIP0;
			aclField[TEMPLATE_IPv6SIP0].fieldUnion.data.value = (acl_filter->ingress_src_ipv6_addr_end[14]<<8)|(acl_filter->ingress_src_ipv6_addr_end[15]);
			aclField[TEMPLATE_IPv6SIP0].fieldUnion.data.mask = 0xffff;

			aclField[TEMPLATE_IPv6SIP1].fieldType = ACL_FIELD_PATTERN_MATCH;
			aclField[TEMPLATE_IPv6SIP1].fieldUnion.pattern.fieldIdx = TEMPLATE_IPv6SIP1;
			aclField[TEMPLATE_IPv6SIP1].fieldUnion.data.value = (acl_filter->ingress_src_ipv6_addr_end[12]<<8)|(acl_filter->ingress_src_ipv6_addr_end[13]);
			aclField[TEMPLATE_IPv6SIP1].fieldUnion.data.mask = 0xffff;
	
        }
		else if(ipv6_unmask_length <=16){
			//compare SIPv6[31:16]
			aclField[TEMPLATE_IPv6SIP1].fieldType = ACL_FIELD_PATTERN_MATCH;
			aclField[TEMPLATE_IPv6SIP1].fieldUnion.pattern.fieldIdx = TEMPLATE_IPv6SIP1;
			aclField[TEMPLATE_IPv6SIP1].fieldUnion.data.value = (acl_filter->ingress_src_ipv6_addr_end[12]<<8)|(acl_filter->ingress_src_ipv6_addr_end[13]);
			aclField[TEMPLATE_IPv6SIP1].fieldUnion.data.mask = 0xffff;

		}
		else if(ipv6_unmask_length <= 32){//IPv6 SIP[31:0] can be compared
			//mask range is larger than 32bit, don't care SIP[31:0]
#if 0        
	        ASSERT_EQ(_rtk_rg_search_acl_empty_ipTableEntry(&table_index),RT_ERR_RG_OK);

	        bzero(&aclSIPv6RangeEntry, sizeof(aclSIPv6RangeEntry));
	        aclSIPv6RangeEntry.index= table_index;
	        aclSIPv6RangeEntry.upperIp=(acl_filter->ingress_src_ipv6_addr_end[12]<<24)|(acl_filter->ingress_src_ipv6_addr_end[13]<<16)|(acl_filter->ingress_src_ipv6_addr_end[14]<<8)|(acl_filter->ingress_src_ipv6_addr_end[15]);
	        aclSIPv6RangeEntry.lowerIp=(acl_filter->ingress_src_ipv6_addr_start[12]<<24)|(acl_filter->ingress_src_ipv6_addr_start[13]<<16)|(acl_filter->ingress_src_ipv6_addr_start[14]<<8)|(acl_filter->ingress_src_ipv6_addr_start[15]);
	        aclSIPv6RangeEntry.type = IPRANGE_IPV6_SIP;


	        aclField[TEMPLATE_IPRANGE].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_IPRANGE].fieldUnion.pattern.fieldIdx = TEMPLATE_IPRANGE;
	        aclField[TEMPLATE_IPRANGE].fieldUnion.data.value |= (1<<table_index);
	        aclField[TEMPLATE_IPRANGE].fieldUnion.data.mask |= (1<<table_index);

	        aclSWEntry->hw_used_table |= ACL_USED_IPTABLE_IPV6SIP;
	        aclSWEntry->hw_used_table_index[ACL_USED_IPTABLE_IPV6SIP_INDEX] = table_index;

	        if( rtk_acl_ipRange_set(&aclSIPv6RangeEntry))
	        {
	            debug("adding acl Sipv6Range table failed");
	            return RT_ERR_RG_ACL_IPTABLE_ACCESS_FAILED;
	        }
	        else
	        {
	            //debug("adding acl Sipv6Range to iptable[%d]",aclSIPv6RangeEntry.index);
	        }
#endif

        }

		//IPv6 SIP[128:113] should alway be compared. 
		aclField[TEMPLATE_FIELDSELECT03].fieldType = ACL_FIELD_PATTERN_MATCH;
		aclField[TEMPLATE_FIELDSELECT03].fieldUnion.pattern.fieldIdx = TEMPLATE_FIELDSELECT03;
		aclField[TEMPLATE_FIELDSELECT03].fieldUnion.data.value = (acl_filter->ingress_src_ipv6_addr_end[0]<<8)|(acl_filter->ingress_src_ipv6_addr_end[1]);
		aclField[TEMPLATE_FIELDSELECT03].fieldUnion.data.mask = 0xffff;
		
		flag_ipv6_sip_need_to_trap = ENABLE;

    }
    if(acl_filter->filter_fields & INGRESS_IPV6_DIP_RANGE_BIT)
    {
    	uint16 dipv6_first_diff_byte_mask=0;
    	ipv6_unmask_length = 0;	
		ASSERT_EQ(_rtk_rg_maskLength_get_by_ipv6_range(acl_filter->ingress_dest_ipv6_addr_start,acl_filter->ingress_dest_ipv6_addr_end,&ipv6_unmask_length),RT_ERR_RG_OK);
		if(ipv6_unmask_length > 112){
			rtlglue_printf("ipv6 dest range too wide (only supported maskLength less than 112bit)!!!\n");
			return RT_ERR_RG_INVALID_PARAM;
		}
#if 0
		if(ipv6_mask_length > 32){
			 flag_ipv6_dip_need_to_trap=ENABLE;
		}
	
       	if(ipv6_mask_length < 32){ //IPv6 DIP[31:0]
	        ASSERT_EQ(_rtk_rg_search_acl_empty_ipTableEntry(&table_index),RT_ERR_RG_OK);

	        bzero(&aclDIPv6RangeEntry, sizeof(aclDIPv6RangeEntry));
	        aclDIPv6RangeEntry.index= table_index;
	        aclDIPv6RangeEntry.upperIp=(acl_filter->ingress_dest_ipv6_addr_end[12]<<24)|(acl_filter->ingress_dest_ipv6_addr_end[13]<<16)|(acl_filter->ingress_dest_ipv6_addr_end[14]<<8)|(acl_filter->ingress_dest_ipv6_addr_end[15]);
	        aclDIPv6RangeEntry.lowerIp=(acl_filter->ingress_dest_ipv6_addr_start[12]<<24)|(acl_filter->ingress_dest_ipv6_addr_start[13]<<16)|(acl_filter->ingress_dest_ipv6_addr_start[14]<<8)|(acl_filter->ingress_dest_ipv6_addr_start[15]);
	        aclDIPv6RangeEntry.type = IPRANGE_IPV6_DIP;

	        aclField[TEMPLATE_IPRANGE].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_IPRANGE].fieldUnion.pattern.fieldIdx = TEMPLATE_IPRANGE;
	        aclField[TEMPLATE_IPRANGE].fieldUnion.data.value |= (1<<table_index);
	        aclField[TEMPLATE_IPRANGE].fieldUnion.data.mask |= (1<<table_index);

	        aclSWEntry->hw_used_table |= ACL_USED_IPTABLE_IPV6DIP;
	        aclSWEntry->hw_used_table_index[ACL_USED_IPTABLE_IPV6DIP_INDEX] = table_index;

	        if( rtk_acl_ipRange_set(&aclDIPv6RangeEntry))
	        {
	            debug("adding acl Sipv6Range table failed");
	            return RT_ERR_RG_ACL_IPTABLE_ACCESS_FAILED;
	        }
	        else
	        {
	            //debug("adding acl Sipv6Range to iptable[%d]",aclDIPv6RangeEntry.index);
	        }
       	}
#endif


		//set each  DIP pattern
		if(ipv6_unmask_length < 16){
	        aclField[TEMPLATE_IPv6DIP0].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_IPv6DIP0].fieldUnion.pattern.fieldIdx = TEMPLATE_IPv6DIP0;
	        aclField[TEMPLATE_IPv6DIP0].fieldUnion.data.value = (acl_filter->ingress_dest_ipv6_addr_end[14]<<8)|(acl_filter->ingress_dest_ipv6_addr_end[15]);//v6DIP[15:0]
	        aclField[TEMPLATE_IPv6DIP0].fieldUnion.data.mask = 0xffff;
		}

		if(ipv6_unmask_length < 32){
	        aclField[TEMPLATE_IPv6DIP1].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_IPv6DIP1].fieldUnion.pattern.fieldIdx = TEMPLATE_IPv6DIP1;
	        aclField[TEMPLATE_IPv6DIP1].fieldUnion.data.value = (acl_filter->ingress_dest_ipv6_addr_end[12]<<8)|(acl_filter->ingress_dest_ipv6_addr_end[13]);//v6DIP[31:16]
			aclField[TEMPLATE_IPv6DIP1].fieldUnion.data.mask = 0xffff;
			
		}


        //IPv6 DIP[127:32], use field selector 8~13,		
		if(ipv6_unmask_length < 48){
	        aclField[TEMPLATE_FIELDSELECT08].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_FIELDSELECT08].fieldUnion.pattern.fieldIdx = TEMPLATE_FIELDSELECT08;
	        aclField[TEMPLATE_FIELDSELECT08].fieldUnion.data.value = (acl_filter->ingress_dest_ipv6_addr_end[10]<<8)|(acl_filter->ingress_dest_ipv6_addr_end[11]);//v6DIP[47:32]
			aclField[TEMPLATE_FIELDSELECT08].fieldUnion.data.mask = 0xffff;
		}

		if(ipv6_unmask_length < 64){
	        aclField[TEMPLATE_FIELDSELECT09].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_FIELDSELECT09].fieldUnion.pattern.fieldIdx = TEMPLATE_FIELDSELECT09;
	        aclField[TEMPLATE_FIELDSELECT09].fieldUnion.data.value = (acl_filter->ingress_dest_ipv6_addr_end[8]<<8)|(acl_filter->ingress_dest_ipv6_addr_end[9]);//v6DIP[63:48]
			aclField[TEMPLATE_FIELDSELECT09].fieldUnion.data.mask = 0xffff;
		}

		if(ipv6_unmask_length < 80){
	        aclField[TEMPLATE_FIELDSELECT10].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_FIELDSELECT10].fieldUnion.pattern.fieldIdx = TEMPLATE_FIELDSELECT10;
	        aclField[TEMPLATE_FIELDSELECT10].fieldUnion.data.value = (acl_filter->ingress_dest_ipv6_addr_end[6]<<8)|(acl_filter->ingress_dest_ipv6_addr_end[7]);//v6DIP[79:64]
			aclField[TEMPLATE_FIELDSELECT10].fieldUnion.data.mask = 0xffff;
		}

		if(ipv6_unmask_length < 96){
	        aclField[TEMPLATE_FIELDSELECT11].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_FIELDSELECT11].fieldUnion.pattern.fieldIdx = TEMPLATE_FIELDSELECT11;
	        aclField[TEMPLATE_FIELDSELECT11].fieldUnion.data.value = (acl_filter->ingress_dest_ipv6_addr_end[4]<<8)|(acl_filter->ingress_dest_ipv6_addr_end[5]);//v6DIP[95:80]
			aclField[TEMPLATE_FIELDSELECT11].fieldUnion.data.mask = 0xffff;
		}

		if(ipv6_unmask_length < 112){
	        aclField[TEMPLATE_FIELDSELECT12].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_FIELDSELECT12].fieldUnion.pattern.fieldIdx = TEMPLATE_FIELDSELECT12;
	        aclField[TEMPLATE_FIELDSELECT12].fieldUnion.data.value = (acl_filter->ingress_dest_ipv6_addr_end[2]<<8)|(acl_filter->ingress_dest_ipv6_addr_end[3]);//v6DIP[111:96]
			aclField[TEMPLATE_FIELDSELECT12].fieldUnion.data.mask = 0xffff;
		}

		//IPv6 DIP[127:112]alway be compared
        aclField[TEMPLATE_FIELDSELECT13].fieldType = ACL_FIELD_PATTERN_MATCH;
        aclField[TEMPLATE_FIELDSELECT13].fieldUnion.pattern.fieldIdx = TEMPLATE_FIELDSELECT13;
        aclField[TEMPLATE_FIELDSELECT13].fieldUnion.data.value = (acl_filter->ingress_dest_ipv6_addr_end[0]<<8)|(acl_filter->ingress_dest_ipv6_addr_end[1]);//v6DIP[127:112]
        aclField[TEMPLATE_FIELDSELECT13].fieldUnion.data.mask = 0xffff;

		//set first different DIP pattern mask
		for(i=0;i<16;i++){
			if(i>=(ipv6_unmask_length%16))
				dipv6_first_diff_byte_mask |= (1<<i);
		}
		
		if(0<=ipv6_unmask_length && ipv6_unmask_length<16){
			aclField[TEMPLATE_IPv6DIP0].fieldUnion.data.mask = dipv6_first_diff_byte_mask;
		}else if(16<=ipv6_unmask_length && ipv6_unmask_length<32){
			aclField[TEMPLATE_IPv6DIP1].fieldUnion.data.mask = dipv6_first_diff_byte_mask;
		}else if(32<=ipv6_unmask_length && ipv6_unmask_length<48){
			aclField[TEMPLATE_FIELDSELECT08].fieldUnion.data.mask = dipv6_first_diff_byte_mask;
		}else if(48<=ipv6_unmask_length && ipv6_unmask_length<64){
			aclField[TEMPLATE_FIELDSELECT09].fieldUnion.data.mask = dipv6_first_diff_byte_mask;
		}else if(64<=ipv6_unmask_length && ipv6_unmask_length<80){
			aclField[TEMPLATE_FIELDSELECT11].fieldUnion.data.mask = dipv6_first_diff_byte_mask;
		}else if(80<=ipv6_unmask_length && ipv6_unmask_length<96){
			aclField[TEMPLATE_FIELDSELECT11].fieldUnion.data.mask = dipv6_first_diff_byte_mask;
		}else if(96<=ipv6_unmask_length && ipv6_unmask_length<112){
			aclField[TEMPLATE_FIELDSELECT12].fieldUnion.data.mask = dipv6_first_diff_byte_mask;
		}
		
	
    }
    if(acl_filter->filter_fields & INGRESS_IPV4_SIP_RANGE_BIT)
    {

		if(acl_filter->ingress_src_ipv4_addr_start == acl_filter->ingress_src_ipv4_addr_end ){
			//SINGLE IP
	        aclField[TEMPLATE_IPv4SIP0].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_IPv4SIP0].fieldUnion.pattern.fieldIdx = TEMPLATE_IPv4SIP0;
	        aclField[TEMPLATE_IPv4SIP0].fieldUnion.data.value = (acl_filter->ingress_src_ipv4_addr_start & 0xffff);//SIP[15:0]
	        aclField[TEMPLATE_IPv4SIP0].fieldUnion.data.mask = 0xffff;
			
	        aclField[TEMPLATE_IPv4SIP1].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_IPv4SIP1].fieldUnion.pattern.fieldIdx = TEMPLATE_IPv4SIP1;
	        aclField[TEMPLATE_IPv4SIP1].fieldUnion.data.value = (acl_filter->ingress_src_ipv4_addr_start & 0xffff0000)>>16;//SIP[31:16]
	        aclField[TEMPLATE_IPv4SIP1].fieldUnion.data.mask = 0xffff;

		}else{
			//IP RANGE
			int i;
			uint32 mask=0;
			ASSERT_EQ(_rtk_rg_maskLength_get_by_ipv4_range(acl_filter->ingress_src_ipv4_addr_start,acl_filter->ingress_src_ipv4_addr_end,&ipv4_unmask_length),RT_ERR_RG_OK);
		
			//build IPv4 Range mask
			for(i=0;i<32;i++){
				if(i>=ipv4_unmask_length){mask|=(1<<i);}
			}
	
			aclField[TEMPLATE_IPv4SIP0].fieldType = ACL_FIELD_PATTERN_MATCH;
			aclField[TEMPLATE_IPv4SIP0].fieldUnion.pattern.fieldIdx = TEMPLATE_IPv4SIP0;
			aclField[TEMPLATE_IPv4SIP0].fieldUnion.data.value = (acl_filter->ingress_src_ipv4_addr_start & 0xffff);//SIP[15:0]
			aclField[TEMPLATE_IPv4SIP0].fieldUnion.data.mask = (mask&0x0000ffff);

			aclField[TEMPLATE_IPv4SIP1].fieldType = ACL_FIELD_PATTERN_MATCH;
			aclField[TEMPLATE_IPv4SIP1].fieldUnion.pattern.fieldIdx = TEMPLATE_IPv4SIP1;
			aclField[TEMPLATE_IPv4SIP1].fieldUnion.data.value = (acl_filter->ingress_src_ipv4_addr_start & 0xffff0000)>>16;//SIP[31:16]
			aclField[TEMPLATE_IPv4SIP1].fieldUnion.data.mask = (mask >> 16);
	
#if 0	
			
			/*original way using IP_RANGE table*/
	        ASSERT_EQ(_rtk_rg_search_acl_empty_ipTableEntry(&table_index),RT_ERR_RG_OK);

	        bzero(&aclSIPv4RangeEntry, sizeof(aclSIPv4RangeEntry));
	        aclSIPv4RangeEntry.index= table_index;
	        aclSIPv4RangeEntry.upperIp=acl_filter->ingress_src_ipv4_addr_end;
	        aclSIPv4RangeEntry.lowerIp=acl_filter->ingress_src_ipv4_addr_start;
	        aclSIPv4RangeEntry.type = IPRANGE_IPV4_SIP;


	        aclField[TEMPLATE_IPRANGE].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_IPRANGE].fieldUnion.pattern.fieldIdx = TEMPLATE_IPRANGE;
	        aclField[TEMPLATE_IPRANGE].fieldUnion.data.value |= (1<<table_index);
	        aclField[TEMPLATE_IPRANGE].fieldUnion.data.mask |= (1<<table_index);

	        aclSWEntry->hw_used_table |= ACL_USED_IPTABLE_IPV4SIP;
	        aclSWEntry->hw_used_table_index[ACL_USED_IPTABLE_IPV4SIP_INDEX] = table_index;

	        if( rtk_acl_ipRange_set(&aclSIPv4RangeEntry))
	        {
	            debug("adding acl Sipv4Range table failed");
	            return RT_ERR_RG_ACL_IPTABLE_ACCESS_FAILED;
	        }
	        else
	        {
	            //debug("adding acl Sipv4Range to iptable[%d]",aclSIPv4RangeEntry.index);
	        }
#endif
		}
    }
    if(acl_filter->filter_fields & INGRESS_IPV4_DIP_RANGE_BIT)
    {

		if(acl_filter->ingress_dest_ipv4_addr_start == acl_filter->ingress_dest_ipv4_addr_end ){
			//SINGLE IP
	        aclField[TEMPLATE_IPv4DIP0].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_IPv4DIP0].fieldUnion.pattern.fieldIdx = TEMPLATE_IPv4DIP0;
	        aclField[TEMPLATE_IPv4DIP0].fieldUnion.data.value = (acl_filter->ingress_dest_ipv4_addr_start & 0xffff);//DIP[15:0]
	        aclField[TEMPLATE_IPv4DIP0].fieldUnion.data.mask = 0xffff;
			
	        aclField[TEMPLATE_IPv4DIP1].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_IPv4DIP1].fieldUnion.pattern.fieldIdx = TEMPLATE_IPv4DIP1;
	        aclField[TEMPLATE_IPv4DIP1].fieldUnion.data.value = (acl_filter->ingress_dest_ipv4_addr_start & 0xffff0000)>>16;//DIP[31:16]
	        aclField[TEMPLATE_IPv4DIP1].fieldUnion.data.mask = 0xffff;

		}else{
			//IP RANGE
			int i;
			uint32 mask=0;
			ASSERT_EQ(_rtk_rg_maskLength_get_by_ipv4_range(acl_filter->ingress_dest_ipv4_addr_start,acl_filter->ingress_dest_ipv4_addr_end,&ipv4_unmask_length),RT_ERR_RG_OK);
		
			//build IPv4 Range mask
			for(i=0;i<32;i++){
				if(i>=ipv4_unmask_length){mask|=(1<<i);}
			}
	
			aclField[TEMPLATE_IPv4DIP0].fieldType = ACL_FIELD_PATTERN_MATCH;
			aclField[TEMPLATE_IPv4DIP0].fieldUnion.pattern.fieldIdx = TEMPLATE_IPv4DIP0;
			aclField[TEMPLATE_IPv4DIP0].fieldUnion.data.value = (acl_filter->ingress_dest_ipv4_addr_start & 0xffff);//SIP[15:0]
			aclField[TEMPLATE_IPv4DIP0].fieldUnion.data.mask = (mask&0x0000ffff);

			aclField[TEMPLATE_IPv4DIP1].fieldType = ACL_FIELD_PATTERN_MATCH;
			aclField[TEMPLATE_IPv4DIP1].fieldUnion.pattern.fieldIdx = TEMPLATE_IPv4DIP1;
			aclField[TEMPLATE_IPv4DIP1].fieldUnion.data.value = (acl_filter->ingress_dest_ipv4_addr_start & 0xffff0000)>>16;//SIP[31:16]
			aclField[TEMPLATE_IPv4DIP1].fieldUnion.data.mask = (mask >> 16);

#if 0	
			/*original way using IP_RANGE table*/
	        ASSERT_EQ(_rtk_rg_search_acl_empty_ipTableEntry(&table_index),RT_ERR_RG_OK);

	        bzero(&aclDIPv4RangeEntry, sizeof(aclDIPv4RangeEntry));
	        aclDIPv4RangeEntry.index = table_index;
	        aclDIPv4RangeEntry.upperIp=acl_filter->ingress_dest_ipv4_addr_end;
	        aclDIPv4RangeEntry.lowerIp=acl_filter->ingress_dest_ipv4_addr_start;
	        aclDIPv4RangeEntry.type = IPRANGE_IPV4_DIP;


	        aclField[TEMPLATE_IPRANGE].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_IPRANGE].fieldUnion.pattern.fieldIdx = TEMPLATE_IPRANGE;
	        aclField[TEMPLATE_IPRANGE].fieldUnion.data.value |= (1<<table_index);
	        aclField[TEMPLATE_IPRANGE].fieldUnion.data.mask |= (1<<table_index);

	        aclSWEntry->hw_used_table |= ACL_USED_IPTABLE_IPV4DIP;
	        aclSWEntry->hw_used_table_index[ACL_USED_IPTABLE_IPV4DIP_INDEX] = table_index;

	        if( rtk_acl_ipRange_set(&aclDIPv4RangeEntry))
	        {
	            debug("adding acl Dipv4Range table failed");
	            return RT_ERR_RG_ACL_IPTABLE_ACCESS_FAILED;
	        }
	        else
	        {
	            //debug("adding acl Sipv4Range to iptable[%d]",aclDIPv4RangeEntry.index);
	        }
#endif
		}
    }
    if(acl_filter->filter_fields & INGRESS_L4_SPORT_RANGE_BIT)
    {

		if(acl_filter->ingress_src_l4_port_start == acl_filter->ingress_src_l4_port_end ){
			//SINGLE IPort
	        aclField[TEMPLATE_FIELDSELECT01].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_FIELDSELECT01].fieldUnion.pattern.fieldIdx = TEMPLATE_FIELDSELECT01;
	        aclField[TEMPLATE_FIELDSELECT01].fieldUnion.data.value = acl_filter->ingress_src_l4_port_start;
	        aclField[TEMPLATE_FIELDSELECT01].fieldUnion.data.mask = 0xffff;

		}else{
			//Range Port
	        ASSERT_EQ(_rtk_rg_search_acl_empty_portTableEntry(&table_index),RT_ERR_RG_OK);

	        bzero(&aclSportRangeEntry, sizeof(aclSportRangeEntry));
	        aclSportRangeEntry.index = table_index;
	        aclSportRangeEntry.upper_bound=acl_filter->ingress_src_l4_port_end;
	        aclSportRangeEntry.lower_bound=acl_filter->ingress_src_l4_port_start;
	        aclSportRangeEntry.type = PORTRANGE_SPORT;


	        aclField[TEMPLATE_PORTRANGE].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_PORTRANGE].fieldUnion.pattern.fieldIdx = TEMPLATE_PORTRANGE;
	        aclField[TEMPLATE_PORTRANGE].fieldUnion.data.value |= (1<<table_index);
	        aclField[TEMPLATE_PORTRANGE].fieldUnion.data.mask |= (1<<table_index);

	        aclSWEntry->hw_used_table |= ACL_USED_PORTTABLE_SPORT;
	        aclSWEntry->hw_used_table_index[ACL_USED_PORTTABLE_SPORT_INDEX] = table_index;


	        if(rtk_acl_portRange_set(&aclSportRangeEntry))
	        {
	            debug("adding acl SportRange table failed");
	            return RT_ERR_RG_ACL_PORTTABLE_ACCESS_FAILED;
	        }
	        else
	        {
	            //debug("adding acl SportRange to porttable[%d]",aclSportRangeEntry.index);
	        }
		}

    }
    if(acl_filter->filter_fields & INGRESS_L4_DPORT_RANGE_BIT)
    {

		if(acl_filter->ingress_dest_l4_port_start == acl_filter->ingress_dest_l4_port_end ){
			//SINGLE IPort
	        aclField[TEMPLATE_FIELDSELECT00].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_FIELDSELECT00].fieldUnion.pattern.fieldIdx = TEMPLATE_FIELDSELECT00;
	        aclField[TEMPLATE_FIELDSELECT00].fieldUnion.data.value = acl_filter->ingress_dest_l4_port_start;
	        aclField[TEMPLATE_FIELDSELECT00].fieldUnion.data.mask = 0xffff;

		}else{
	        ASSERT_EQ(_rtk_rg_search_acl_empty_portTableEntry(&table_index),RT_ERR_RG_OK);

	        bzero(&aclDportRangeEntry, sizeof(aclDportRangeEntry));
	        aclDportRangeEntry.index = table_index;
	        aclDportRangeEntry.upper_bound=acl_filter->ingress_dest_l4_port_end;
	        aclDportRangeEntry.lower_bound=acl_filter->ingress_dest_l4_port_start;
	        aclDportRangeEntry.type = PORTRANGE_DPORT;

	        aclField[TEMPLATE_PORTRANGE].fieldType = ACL_FIELD_PATTERN_MATCH;
	        aclField[TEMPLATE_PORTRANGE].fieldUnion.pattern.fieldIdx = TEMPLATE_PORTRANGE;
	        aclField[TEMPLATE_PORTRANGE].fieldUnion.data.value |= (1<<table_index);
	        aclField[TEMPLATE_PORTRANGE].fieldUnion.data.mask |= (1<<table_index);

	        aclSWEntry->hw_used_table |= ACL_USED_PORTTABLE_DPORT;
	        aclSWEntry->hw_used_table_index[ACL_USED_PORTTABLE_DPORT_INDEX] = table_index;

	        if(rtk_acl_portRange_set(&aclDportRangeEntry))
	        {
	            debug("adding acl DportRange table failed");
	            return RT_ERR_RG_ACL_PORTTABLE_ACCESS_FAILED;
	        }
	        else
	        {
	            //debug("adding acl DportRange to porttable[%d]",aclDportRangeEntry.index);
	        }
		}

    }
    if(acl_filter->filter_fields & EGRESS_IPV4_SIP_RANGE_BIT)
    {
        ASSERT_EQ(_rtk_rg_search_cf_empty_ipTableEntry(&table_index),RT_ERR_RG_OK);

        bzero(&cfIpRangeEntry, sizeof(cfIpRangeEntry));
        cfIpRangeEntry.index = table_index;
        cfIpRangeEntry.upperIp=acl_filter->egress_src_ipv4_addr_end;
        cfIpRangeEntry.lowerIp=acl_filter->egress_src_ipv4_addr_start;
        cfIpRangeEntry.type = CLASSIFY_IPRANGE_IPV4_SIP;

        classifyField_1[CLASSIFY_FIELD_IP_RANGE].fieldType =CLASSIFY_FIELD_IP_RANGE;
        classifyField_1[CLASSIFY_FIELD_IP_RANGE].classify_pattern.fieldData.value=((1<<3) | table_index); //(1<<3) is the valid bit
        classifyField_1[CLASSIFY_FIELD_IP_RANGE].classify_pattern.fieldData.mask=0xf;


        classifyField_2[CLASSIFY_FIELD_IP_RANGE].fieldType =CLASSIFY_FIELD_IP_RANGE;
        classifyField_2[CLASSIFY_FIELD_IP_RANGE].classify_pattern.fieldData.value=((1<<3) | table_index); //(1<<3) is the valid bit
        classifyField_2[CLASSIFY_FIELD_IP_RANGE].classify_pattern.fieldData.mask=0xf;

        aclSWEntry->hw_used_table |= CF_USED_IPTABLE_IPV4SIP;
        aclSWEntry->hw_used_table_index[CF_USED_IPTABLE_IPV4SIP_INDEX] = table_index;
    }
    if(acl_filter->filter_fields & EGRESS_IPV4_DIP_RANGE_BIT)
    {
        ASSERT_EQ(_rtk_rg_search_cf_empty_ipTableEntry(&table_index),RT_ERR_RG_OK);

        bzero(&cfIpRangeEntry, sizeof(cfIpRangeEntry));
        cfIpRangeEntry.index = table_index;
        cfIpRangeEntry.upperIp=acl_filter->egress_dest_ipv4_addr_end;
        cfIpRangeEntry.lowerIp=acl_filter->egress_dest_ipv4_addr_start;
        cfIpRangeEntry.type = CLASSIFY_IPRANGE_IPV4_DIP;

        classifyField_1[CLASSIFY_FIELD_IP_RANGE].fieldType =CLASSIFY_FIELD_IP_RANGE;
        classifyField_1[CLASSIFY_FIELD_IP_RANGE].classify_pattern.fieldData.value=((1<<3) | table_index); //(1<<3) is the valid bit
        classifyField_1[CLASSIFY_FIELD_IP_RANGE].classify_pattern.fieldData.mask=0xf;

        classifyField_2[CLASSIFY_FIELD_IP_RANGE].fieldType =CLASSIFY_FIELD_IP_RANGE;
        classifyField_2[CLASSIFY_FIELD_IP_RANGE].classify_pattern.fieldData.value=((1<<3) | table_index); //(1<<3) is the valid bit
        classifyField_2[CLASSIFY_FIELD_IP_RANGE].classify_pattern.fieldData.mask=0xf;

        aclSWEntry->hw_used_table |= CF_USED_IPTABLE_IPV4DIP;
        aclSWEntry->hw_used_table_index[CF_USED_IPTABLE_IPV4DIP_INDEX] = table_index;
    }
    if(acl_filter->filter_fields & EGRESS_L4_SPORT_RANGE_BIT)
    {
        ASSERT_EQ(_rtk_rg_search_cf_empty_portTableEntry(&table_index),RT_ERR_RG_OK);

        bzero(&cfPortRangeEntry, sizeof(cfPortRangeEntry));
        cfPortRangeEntry.index = table_index;
        cfPortRangeEntry.upperPort=acl_filter->egress_src_l4_port_end;
        cfPortRangeEntry.lowerPort=acl_filter->egress_src_l4_port_start;
        cfPortRangeEntry.type = CLASSIFY_PORTRANGE_SPORT;


        classifyField_1[CLASSIFY_FIELD_PORT_RANGE].fieldType = CLASSIFY_FIELD_PORT_RANGE;
        classifyField_1[CLASSIFY_FIELD_PORT_RANGE].classify_pattern.fieldData.value = ((1<<3) | table_index); //(1<<3) is the valid bit
        classifyField_1[CLASSIFY_FIELD_PORT_RANGE].classify_pattern.fieldData.mask=0xf;

        classifyField_2[CLASSIFY_FIELD_PORT_RANGE].fieldType = CLASSIFY_FIELD_PORT_RANGE;
        classifyField_2[CLASSIFY_FIELD_PORT_RANGE].classify_pattern.fieldData.value = ((1<<3) | table_index); //(1<<3) is the valid bit
        classifyField_2[CLASSIFY_FIELD_PORT_RANGE].classify_pattern.fieldData.mask=0xf;

        aclSWEntry->hw_used_table |= CF_USED_PORTTABLE_SPORT;
        aclSWEntry->hw_used_table_index[CF_USED_PORTTABLE_SPORT_INDEX] = table_index;

    }
    if(acl_filter->filter_fields & EGRESS_L4_DPORT_RANGE_BIT)
    {
        ASSERT_EQ(_rtk_rg_search_cf_empty_portTableEntry(&table_index),RT_ERR_RG_OK);

        bzero(&cfPortRangeEntry, sizeof(cfPortRangeEntry));
        cfPortRangeEntry.index = table_index;
        cfPortRangeEntry.upperPort=acl_filter->egress_dest_l4_port_end;
        cfPortRangeEntry.lowerPort=acl_filter->egress_dest_l4_port_start;
        cfPortRangeEntry.type = CLASSIFY_PORTRANGE_DPORT;

        classifyField_1[CLASSIFY_FIELD_PORT_RANGE].fieldType = CLASSIFY_FIELD_PORT_RANGE;
        classifyField_1[CLASSIFY_FIELD_PORT_RANGE].classify_pattern.fieldData.value = ((1<<3) | table_index); //(1<<3) is the valid bit
        classifyField_1[CLASSIFY_FIELD_PORT_RANGE].classify_pattern.fieldData.mask=0xf;

        classifyField_2[CLASSIFY_FIELD_PORT_RANGE].fieldType = CLASSIFY_FIELD_PORT_RANGE;
        classifyField_2[CLASSIFY_FIELD_PORT_RANGE].classify_pattern.fieldData.value = ((1<<3) | table_index); //(1<<3) is the valid bit
        classifyField_2[CLASSIFY_FIELD_PORT_RANGE].classify_pattern.fieldData.mask=0xf;

        aclSWEntry->hw_used_table |= CF_USED_PORTTABLE_DPORT;
        aclSWEntry->hw_used_table_index[CF_USED_PORTTABLE_DPORT_INDEX] = table_index;

    }



    //fieldadd field[0~7], for aclRule_0.
    for(i=0; i<RTK_MAX_NUM_OF_ACL_RULE_FIELD; i++)
    {	
        if(memcmp(&aclField[i],&empty_aclField,sizeof(rtk_acl_field_t)))
        {
            //debug("adding field[%d] to aclRule[0]",i);
            flag_add_aclRule[0] = ENABLE;
            if(rtk_acl_igrRuleField_add(&aclRule[0], &aclField[i]))
            {

                //debug("adding aclRule[0] field[%d] failed",i);
                return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
            }
        }
    }
    // field[8~15] for aclRule_1,
    for(i=RTK_MAX_NUM_OF_ACL_RULE_FIELD; i<RTK_MAX_NUM_OF_ACL_RULE_FIELD*2; i++)
    {
        if(memcmp(&aclField[i],&empty_aclField,sizeof(rtk_acl_field_t)))
        {
            //debug("adding field[%d] to aclRule[1]",i);
            flag_add_aclRule[1] = ENABLE;
            //fix the aclRule_1 shift field index
            aclField[i].fieldUnion.pattern.fieldIdx = aclField[i].fieldUnion.pattern.fieldIdx - RTK_MAX_NUM_OF_ACL_RULE_FIELD;
            if(rtk_acl_igrRuleField_add(&aclRule[1], &aclField[i]))
            {
                //debug("adding aclRule[1] field[%d] failed",i);
                return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
            }
        }
    }
    // field[16~23] for aclRule_2
    for(i=RTK_MAX_NUM_OF_ACL_RULE_FIELD*2; i<RTK_MAX_NUM_OF_ACL_RULE_FIELD*3; i++)
    {
        if(memcmp(&aclField[i],&empty_aclField,sizeof(rtk_acl_field_t)))
        {
            //debug("adding field[%d] to aclRule[2]",i);
            flag_add_aclRule[2] = ENABLE;
            //fix the aclRule_2 shift field index
            aclField[i].fieldUnion.pattern.fieldIdx = aclField[i].fieldUnion.pattern.fieldIdx - RTK_MAX_NUM_OF_ACL_RULE_FIELD*2;
            if(rtk_acl_igrRuleField_add(&aclRule[2], &aclField[i]))
            {
                //debug("adding aclRule[2] field[%d] failed",i);
                return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
            }
        }
    }

    // field[24~31] for aclRule_3
    for(i=RTK_MAX_NUM_OF_ACL_RULE_FIELD*3; i<RTK_MAX_NUM_OF_ACL_RULE_FIELD*4; i++)
    {
        if(memcmp(&aclField[i],&empty_aclField,sizeof(rtk_acl_field_t)))
        {
            //debug("adding field[%d] to aclRule[3]",i);
            flag_add_aclRule[3] = ENABLE;
            //fix the aclRule_3 shift field index
            aclField[i].fieldUnion.pattern.fieldIdx = aclField[i].fieldUnion.pattern.fieldIdx - RTK_MAX_NUM_OF_ACL_RULE_FIELD*3;
            if(rtk_acl_igrRuleField_add(&aclRule[3], &aclField[i]))
            {
                //debug("adding aclRule[3] field[%d] failed",i);
                return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
            }
        }
    }

	//if only Activeport or FrameType are valid, set in aclRule_1.
	if(flag_add_aclRule[0]==DISABLE && flag_add_aclRule[1]==DISABLE && flag_add_aclRule[2]==DISABLE && flag_add_aclRule[3]==DISABLE){
		//any Activeport are valid
		if(aclRule[0].activePorts.bits[0]!=0x0)
			flag_add_aclRule[0] = ENABLE;

		//any FrameType are valid
		for(i=0;i<ACL_CARE_TAG_END;i++){
			if(aclRule[0].careTag.tags[i].value==1)
				flag_add_aclRule[0] = ENABLE;
		}
	}
	


    //fieldadd classifyField[all] for cfRule
    for(i=0; i<CLASSIFY_FIELD_END; i++)
    {
        if(memcmp(&classifyField_1[i],&empty_classifyField,sizeof(rtk_classify_field_t)) || memcmp(&classifyField_2[i],&empty_classifyField,sizeof(rtk_classify_field_t)))
        {
            //debug("adding field[%d] to cfRule_1 & cfRule_2",i);
            flag_add_cfRule = ENABLE;

            if(i == CLASSIFY_FIELD_PORT_RANGE)
            {
                if(rtk_classify_portRange_set( &cfPortRangeEntry))
                {
                    debug("adding cf portRange table failed");
                    return RT_ERR_RG_CF_PORTTABLE_ACCESS_FAILED;
                }
                else
                {
                    //debug("adding cf portRange table [%d]",cfPortRangeEntry.index);
                }
            }
            if(i == CLASSIFY_FIELD_IP_RANGE)
            {
                if(rtk_classify_ipRange_set( &cfIpRangeEntry))
                {
                    debug("adding cf ipRange table failed");
                    return RT_ERR_RG_CF_IPTABLE_ACCESS_FAILED;
                }
                else
                {
                    //debug("adding cf ipRange table [%d]",cfIpRangeEntry.index);
                }
            }

			if(i == CLASSIFY_FIELD_ACL_HIT){
				continue;
			}

            if(rtk_classify_field_add(&cfRule_1, &classifyField_1[i]))
            {
                debug("adding cfRule_1 field failed");
                return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
            }

            if(rtk_classify_field_add(&cfRule_2, &classifyField_2[i]))
            {
                debug("adding cfRule_2 field failed");
                return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
            }



        }
    }


    //setup action
	if(flag_ipv6_dip_need_to_trap==ENABLE ||flag_ipv6_sip_need_to_trap==ENABLE){//patch for IPv6 SIP range check: not enought field selector, so trap to CPU
		if(flag_add_cfRule)
		{
			acl_action.enableAct[ACL_IGR_INTR_ACT] = ENABLE;
			acl_action.aclLatch = ENABLED;
		
			cfRule_1.act.usAct.log = CLASSIFY_US_LOG_ACT_ENABLE;
			cfRule_1.act.usAct.logCntIdx = 0; //[fix me] which logging counter index?
		
			//[fix me]use UNI force to CPU port, but need to verify the CPU tag reason is 0?
			cfRule_2.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_FORCE_FORWARD;
			cfRule_2.act.dsAct.uniMask.bits[0]= (1<<RTK_RG_MAC_PORT_CPU);
		}
		else
		{
			//add log counter
			acl_action.enableAct[ACL_IGR_LOG_ACT] = ENABLE;
			acl_action.logAct.act= ACL_IGR_LOG_MIB_ACT;
			acl_action.logAct.mib=0; //[fix me] choose mib0 or another?
			//trap to CPU
			acl_action.enableAct[ACL_IGR_FORWARD_ACT] = ENABLE;
			acl_action.forwardAct.act= ACL_IGR_FORWARD_TRAP_ACT;		
		}
	}else{//other normal case
	    switch(acl_filter->action_type)
	    {
	    case ACL_ACTION_TYPE_DROP:
	        if(flag_add_cfRule)
	        {
	            acl_action.enableAct[ACL_IGR_INTR_ACT] = ENABLE;
	            acl_action.aclLatch = ENABLED;

	            cfRule_1.act.usAct.drop = CLASSIFY_DROP_ACT_ENABLE;

	            cfRule_2.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_FORCE_FORWARD; //DS drop
	            cfRule_2.act.dsAct.uniMask.bits[0] = 0x0;
	        }
	        else
	        {
	            acl_action.enableAct[ACL_IGR_FORWARD_ACT] = ENABLE;
	            acl_action.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
	            acl_action.forwardAct.portMask.bits[0]= 0x0;
	        }
	        break;
	    case ACL_ACTION_TYPE_PERMIT:
	        //need to add a default drop aclRule in last entry!
	        //=> but should consider when to delete this entry (currently, reAdd will not add this default entry while no acl permit rule)

	        //setup default drop asic entry: marked by current request. if need this feature, enable downstaire code
	        //ASSERT_EQ(_rtk_rg_asic_defaultDropEntry_setup(),RT_ERR_RG_OK);


			if(flag_add_cfRule)
			{
				acl_action.enableAct[ACL_IGR_INTR_ACT] = ENABLE;
				acl_action.aclLatch = ENABLED;

				//avoid hit other drop action rule!!!
		        acl_action.enableAct[ACL_IGR_FORWARD_ACT] = ENABLE;
		        acl_action.forwardAct.act= ACL_IGR_FORWARD_COPY_ACT;
				acl_action.forwardAct.portMask.bits[0]= 0x0;

				//Permit, CF do not need to do any action.
			}else{
				//avoid hit drop action by default rule!!!
		        acl_action.enableAct[ACL_IGR_FORWARD_ACT] = ENABLE;
		        acl_action.forwardAct.act= ACL_IGR_FORWARD_COPY_ACT;
				acl_action.forwardAct.portMask.bits[0]= 0x0;
			}
	        break;
	    case ACL_ACTION_TYPE_TRAP:
	        if(flag_add_cfRule)
	        {
	            acl_action.enableAct[ACL_IGR_INTR_ACT] = ENABLE;
	            acl_action.aclLatch = ENABLED;

	            cfRule_1.act.usAct.log = CLASSIFY_US_LOG_ACT_ENABLE;
	            cfRule_1.act.usAct.logCntIdx = 0; //[fix me] which logging counter index?

	            //[fix me]use UNI force to CPU port, but need to verify the CPU tag reason is 0?
	            cfRule_2.act.dsAct.uniAct = CLASSIFY_DS_UNI_ACT_FORCE_FORWARD;
	            cfRule_2.act.dsAct.uniMask.bits[0]= (1<<RTK_RG_MAC_PORT_CPU);
	        }
	        else
	        {
	        	//log to counter
	            acl_action.enableAct[ACL_IGR_LOG_ACT] = ENABLE;
	            acl_action.logAct.act= ACL_IGR_LOG_MIB_ACT;
	            acl_action.logAct.mib=0; //[fix me] choose mib0 or another?

				//trap to CPU
				acl_action.enableAct[ACL_IGR_FORWARD_ACT] = ENABLE;
				acl_action.forwardAct.act= ACL_IGR_FORWARD_TRAP_ACT;	

	        }
	        break;
	    case ACL_ACTION_TYPE_QOS: //need to prevent conflict actions
	        //*****ACL action allot:*****//
	        //***CACT=>1p remarking, SACT=>IP or DSCP remarking, PoliceACT=> sharemeter, PRIACT(aclPri)=>QID***//

	        for(i=0; i<32 ; i++)
	        {
	            switch((1<<i)&acl_filter->qos_actions)
	            {
	            case ACL_ACTION_NOP_BIT:
	                break;
	            case ACL_ACTION_1P_REMARKING_BIT:
	                if(flag_add_cfRule)
	                {
	                    acl_action.enableAct[ACL_IGR_INTR_ACT] = ENABLE;
	                    acl_action.aclLatch = ENABLED;

	                    //[fix me]: "CF PATCH" for US, because we don't know whethere QOS_1P_remarking Enable or not
	                    //so, we both set CACT 1p reamrking, and cfPri
	                    cfRule_1.act.usAct.cAct = CLASSIFY_US_CACT_ADD_CTAG_8100;
	                    cfRule_1.act.usAct.cVidAct = CLASSIFY_US_VID_ACT_FROM_INTERNAL;
	                    cfRule_1.act.usAct.cPriAct = CLASSIFY_US_PRI_ACT_ASSIGN;
	                    cfRule_1.act.usAct.cTagPri = acl_filter->action_dot1p_remarking_pri;

	                    cfRule_1.act.usAct.interPriAct = CLASSIFY_CF_PRI_ACT_ASSIGN;
	                    cfRule_1.act.usAct.cfPri = acl_filter->action_dot1p_remarking_pri;


	                    cfRule_2.act.dsAct.cAct = CLASSIFY_DS_CACT_ADD_CTAG_8100;
	                    //[fix me] apolloMP CF DS do not have CVID from internal, so use LUT_MAC_LEARN this option.  but this may assign wrong cvid while not binding.
	                    cfRule_2.act.dsAct.cVidAct = CLASSIFY_DS_VID_ACT_FROM_LUT;
	                    cfRule_2.act.dsAct.cPriAct = CLASSIFY_DS_PRI_ACT_ASSIGN;
	                    cfRule_2.act.dsAct.cTagPri = acl_filter->action_dot1p_remarking_pri;

	                }
	                else
	                {
	                    acl_action.enableAct[ACL_IGR_CVLAN_ACT] = ENABLE;
	                    acl_action.cvlanAct.act= ACL_IGR_CVLAN_1P_REMARK_ACT;
	                    acl_action.cvlanAct.dot1p=acl_filter->action_dot1p_remarking_pri;
	                }
	                break;
	            case ACL_ACTION_IP_PRECEDENCE_REMARKING_BIT:
	                //[fix me] IP_PRECEDENCE_REMARKING as dscp remarking problem
	                if(flag_add_cfRule)
	                {
	                    acl_action.enableAct[ACL_IGR_INTR_ACT] = 1;
	                    acl_action.aclLatch = 1;


	                    ASSERT_EQ(_rtk_rg_search_cf_empty_dscpTableEntry(&table_index),RT_ERR_RG_OK);

	                    //record to aclSWEntry
	                    aclSWEntry->hw_used_table |= CF_USED_DSCPTABLE;
	                    aclSWEntry->hw_used_table_index[CF_USED_DSCPTABLE_INDEX] = table_index;

	                    //setup to CF
	                    if(rtk_classify_cfPri2Dscp_set(table_index, (acl_filter->action_ip_precedence_remarking_pri<<3)))
	                    {
	                        return RT_ERR_RG_CF_DSCPTABLE_ACCESS_FAILED;
	                    }


						debug("set dscp_entry[%d] acl_filter->action_ip_precedence_remarking_pri=%x (acl_filter->action_ip_precedence_remarking_pri<<3)=%x",table_index,acl_filter->action_ip_precedence_remarking_pri,(acl_filter->action_ip_precedence_remarking_pri<<3));
						
	                    cfRule_1.act.usAct.dscp = CLASSIFY_DSCP_ACT_ENABLE;
	                    cfRule_1.act.usAct.cfPri= table_index;

	                    cfRule_2.act.dsAct.dscp = CLASSIFY_DSCP_ACT_ENABLE;
	                    cfRule_2.act.dsAct.cfPri = table_index;

	                }
	                else
	                {
	                    acl_action.enableAct[ACL_IGR_SVLAN_ACT] = ENABLE;
	                    acl_action.svlanAct.act = ACL_IGR_SVLAN_DSCP_REMARK_ACT;
	                    acl_action.svlanAct.dscp = (acl_filter->action_ip_precedence_remarking_pri << 3);
	                }
	                break;
	            case ACL_ACTION_DSCP_REMARKING_BIT:
	                if(flag_add_cfRule)
	                {
	                    acl_action.enableAct[ACL_IGR_INTR_ACT] = ENABLE;
	                    acl_action.aclLatch = ENABLED;

	                    ASSERT_EQ(_rtk_rg_search_cf_empty_dscpTableEntry(&table_index),RT_ERR_RG_OK);

	                    //record to aclSWEntry
	                    aclSWEntry->hw_used_table |= CF_USED_DSCPTABLE;
	                    aclSWEntry->hw_used_table_index[CF_USED_DSCPTABLE_INDEX] = table_index;

	                    //setup to CF
	                    if(rtk_classify_cfPri2Dscp_set(table_index, acl_filter->action_dscp_remarking_pri))
	                    {
	                        return RT_ERR_RG_CF_DSCPTABLE_ACCESS_FAILED;
	                    }
	                    cfRule_1.act.usAct.dscp = CLASSIFY_DSCP_ACT_ENABLE;
	                    cfRule_1.act.usAct.cfPri = table_index;

	                    cfRule_2.act.dsAct.dscp = CLASSIFY_DSCP_ACT_ENABLE;
	                    cfRule_1.act.dsAct.cfPri = table_index;

	                }
	                else
	                {
	                    acl_action.enableAct[ACL_IGR_SVLAN_ACT] = ENABLE;
	                    acl_action.svlanAct.act = ACL_IGR_SVLAN_DSCP_REMARK_ACT;
	                    acl_action.svlanAct.dscp = acl_filter->action_dscp_remarking_pri;
	                }
	                break;
	            case ACL_ACTION_QUEUE_ID_BIT:
	                if(flag_add_cfRule)
	                {

	                    acl_action.enableAct[ACL_IGR_INTR_ACT] = ENABLE;
	                    acl_action.aclLatch = ENABLED;
		
	          	
						//US by QID_ACT
						//cfRule_1.act.usAct.sidQidAct= CLASSIFY_US_SQID_ACT_ASSIGN_QID; //do not have this action!!!
	                    //cfRule_1.act.usAct.sidQid = acl_filter->action_queue_id;
						cfRule_1.act.usAct.interPriAct = CLASSIFY_CF_PRI_ACT_ASSIGN;
						cfRule_1.act.usAct.cfPri =  acl_filter->action_queue_id;
	
						//DS by cfPriACT
	                    cfRule_2.act.dsAct.interPriAct = CLASSIFY_CF_PRI_ACT_ASSIGN;
	                    cfRule_2.act.dsAct.cfPri = acl_filter->action_queue_id;
						
	                }
	                else
	                {
						acl_action.enableAct[ACL_IGR_PRI_ACT] = ENABLE;
						acl_action.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
						acl_action.priAct.aclPri = acl_filter->action_queue_id;

	                }
	                break;
	            case ACL_ACTION_SHARE_METER_BIT:
	                if(flag_add_cfRule)
	                {
						//can not use in cf, return error
						debug("sharemeter action should not use with cf pattern, please check the pattern do not include egress_ip egress_port egress_intf!");
						return RT_ERR_RG_ACL_CF_FIELD_CONFLICT;
	                }
	                else
	                {
						acl_action.enableAct[ACL_IGR_LOG_ACT] = ENABLE;
						acl_action.logAct.act = ACL_IGR_LOG_POLICING_ACT;
						acl_action.logAct.meter = acl_filter->action_share_meter;
	                }
	                break;

				case ACL_ACTION_STREAM_ID_OR_LLID_BIT:
					if(flag_add_cfRule)
	                {
	                    acl_action.enableAct[ACL_IGR_INTR_ACT] = ENABLE;
	                    acl_action.aclLatch = ENABLED;
						//US by SID_ACT
						cfRule_1.act.usAct.sidQidAct = CLASSIFY_US_SQID_ACT_ASSIGN_SID;
						cfRule_1.act.usAct.sidQid =  acl_filter->action_stream_id_or_llid;
						//DS without SID, don't care

					}  
					else
	                {
						acl_action.enableAct[ACL_IGR_INTR_ACT] = ENABLE;
						acl_action.extendAct.act = ACL_IGR_EXTEND_SID_ACT;
						acl_action.extendAct.index= acl_filter->action_stream_id_or_llid;
	                }
					break;
	          
	           	default:
	                break;
	            }

	        }
	        break;
	    default:
	        break;
	    }
	}

    //setup ACL ASIC
    for(i=0; i<MAX_ACL_TEMPLATE_SIZE; i++)
    {
        if(flag_add_aclRule[i])
            acl_entry_size++;
    }
    ASSERT_EQ(_rtk_rg_search_acl_empty_Entry(acl_entry_size, &acl_entry_index),RT_ERR_RG_OK);
    //setup aclSWEntry
    aclSWEntry->hw_aclEntry_start = acl_entry_index;
    aclSWEntry->hw_aclEntry_size = acl_entry_size;
    //debug("got empty start aclEntry[%d] for %d rules",acl_entry_index,acl_entry_size);

    flag_acl_first_entry = ENABLE;
	acl_entry_index_tmp = acl_entry_index;
    for(i=0; i<MAX_ACL_TEMPLATE_SIZE; i++)
    {
        if(flag_add_aclRule[i])
        {
        	int ret;
            if(flag_acl_first_entry)
            {
                //only first acl entry have to set action
                flag_acl_first_entry = DISABLE;
                aclRule[i].act = acl_action;
            }
            aclRule[i].index = acl_entry_index_tmp;
            aclRule[i].valid = ENABLE;
            aclRule[i].templateIdx = i;
            acl_entry_index_tmp++;
	
			ret = rtk_acl_igrRuleEntry_add(&aclRule[i]);
            if(ret)
            {
                debug("adding aclRule[%d] to aclEntry[%d] failed. ret=0x%x",i,acl_entry_index_tmp,ret);
                return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
            }
        }
    }
    bzero(&pClassifyField_acl,sizeof(pClassifyField_acl));
    // judge is there ACL latch here => fieldadd classifyField[all]
    if((flag_add_aclRule[0] || flag_add_aclRule[1] || flag_add_aclRule[2] || flag_add_aclRule[3]) && flag_add_cfRule)
    {

        classifyField_1[CLASSIFY_FIELD_ACL_HIT].fieldType = CLASSIFY_FIELD_ACL_HIT;
        classifyField_1[CLASSIFY_FIELD_ACL_HIT].classify_pattern.fieldData.value = ((1<<7) | acl_entry_index); //(1<<7) is the valid bit
        classifyField_1[CLASSIFY_FIELD_ACL_HIT].classify_pattern.fieldData.mask=0xff;


        classifyField_2[CLASSIFY_FIELD_ACL_HIT].fieldType = CLASSIFY_FIELD_ACL_HIT;
        classifyField_2[CLASSIFY_FIELD_ACL_HIT].classify_pattern.fieldData.value = ((1<<7) | acl_entry_index); //(1<<7) is the valid bit
        classifyField_2[CLASSIFY_FIELD_ACL_HIT].classify_pattern.fieldData.mask=0xff;

        pClassifyField_acl.fieldType =CLASSIFY_FIELD_ACL_HIT;
        pClassifyField_acl.classify_pattern.fieldData.value=0x81;
        pClassifyField_acl.classify_pattern.fieldData.mask=0xff;

        if(rtk_classify_field_add(&cfRule_1, &classifyField_1[CLASSIFY_FIELD_ACL_HIT]))
        {
            debug("adding cfRule_1 field[CLASSIFY_FIELD_ACL_HIT] failed");
            return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
        }

        if(rtk_classify_field_add(&cfRule_2, &classifyField_2[CLASSIFY_FIELD_ACL_HIT]))
        {
            debug("adding cfRule_2 field[CLASSIFY_FIELD_ACL_HIT] failed");
            return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
        }

    }



    //setup CF ASIC
    if(flag_add_cfRule==ENABLE)
    {
    	int ret;
        switch(flow_direction)
        {
        case FLOW_DIRECTION_CAN_NOT_DECIDE:
            ASSERT_EQ(_rtk_rg_search_cf_empty_Entry(2,&cf_entry_index),RT_ERR_RG_OK);

            aclSWEntry->hw_cfEntry_start = cf_entry_index;
            aclSWEntry->hw_cfEntry_size = 2;

            //debug("got empty cfEntry[%d] & cfEntry[%d] for cfRule",cf_entry_index,cf_entry_index+1);
            cfRule_1.index = cf_entry_index;
            cfRule_1.valid = 1;
            cfRule_1.direction = CLASSIFY_DIRECTION_US;
			

            cfRule_2.index = cf_entry_index+1;
            cfRule_2.valid = 1;
            cfRule_2.direction = CLASSIFY_DIRECTION_DS;

            if((ret = rtk_classify_cfgEntry_add(&cfRule_1))!=RT_ERR_RG_OK)
            {
                debug("adding cfEntry(%d) failed ret(rtk API)=0x%x",cf_entry_index,ret);
                return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
            }
            if((ret = rtk_classify_cfgEntry_add(&cfRule_2))!=RT_ERR_RG_OK)
            {
                debug("adding cfEntry(%d) failed ret(rtk API)=0x%x",cf_entry_index,ret);
                return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
            }

            break;
        case FLOW_DIRECTION_UPSTREAM:
            ASSERT_EQ(_rtk_rg_search_cf_empty_Entry(1,&cf_entry_index),RT_ERR_RG_OK);

            aclSWEntry->hw_cfEntry_start = cf_entry_index;
            aclSWEntry->hw_cfEntry_size = 1;

            debug("got empty cfEntry[%d] for cfRule",cf_entry_index);
            cfRule_1.index = cf_entry_index;
            cfRule_1.valid = 1;
            cfRule_1.direction = CLASSIFY_DIRECTION_US;
            if(rtk_classify_cfgEntry_add(&cfRule_1))
            {
                debug("adding cfEntry(%d) failed",cf_entry_index);
                return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
            }

            break;
        case FLOW_DIRECTION_DOWNSTREAM:
            ASSERT_EQ(_rtk_rg_search_cf_empty_Entry(1,&cf_entry_index),RT_ERR_RG_OK);

            aclSWEntry->hw_cfEntry_start = cf_entry_index;
            aclSWEntry->hw_cfEntry_size = 1;

            debug("got empty cfEntry[%d] for cfRule",cf_entry_index);
            cfRule_2.index = cf_entry_index;
            cfRule_2.valid = 1;
            cfRule_2.direction = CLASSIFY_DIRECTION_DS;
            if(rtk_classify_cfgEntry_add(&cfRule_1))
            {
                debug("adding cfEntry(%d) failed",cf_entry_index);
                return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
            }

            break;

        case FLOW_DIRECTION_LAN_TO_LAN:
            return RT_ERR_RG_ACL_CF_FLOW_DIRECTION_ERROR;
            break;

        case FLOW_DIRECTION_WAN_TO_WAN:
            return RT_ERR_RG_ACL_CF_FLOW_DIRECTION_ERROR;
            break;

        default:
            return RT_ERR_RG_ACL_CF_FLOW_DIRECTION_ERROR;
            break;

        }
    }

    rtk_rg_free(aclRule);
    rtk_rg_free(aclField);
    rtk_rg_free(classifyField_1);
    rtk_rg_free(classifyField_2);

    return RT_ERR_RG_OK;
}



int _rtk_rg_aclSWEntry_reAdd(rtk_rg_aclFilterAndQos_t *acl_filter, int *acl_filter_idx)
{
    rtk_rg_aclFilterEntry_t aclSWEntry,empty_aclSWEntry;
    bzero(&aclSWEntry,sizeof(aclSWEntry));
    bzero(&empty_aclSWEntry,sizeof(empty_aclSWEntry));

    //check input parameter
    if(acl_filter == NULL
            || acl_filter_idx==NULL)
        return RT_ERR_RG_NULL_POINTER;

    if(acl_filter->filter_fields == 0x0)
        return RT_ERR_RG_INITPM_UNINIT;


    //check the aclSWEntry has been used, reAdd must add aclSWEtry in the assigned acl_filter_idx
    ASSERT_EQ(_rtk_rg_aclSWEntry_get(*acl_filter_idx, &aclSWEntry),RT_ERR_RG_OK);

    if(memcmp(&aclSWEntry,&empty_aclSWEntry,sizeof(rtk_rg_aclFilterEntry_t)))
    {
        return RT_ERR_RG_ACL_SW_ENTRY_USED;
    }


    //parse acl_filter field to setup aclSWEntry & ASIC
    ASSERT_EQ(_rtk_rg_aclSWEntry_and_asic_add(acl_filter,&aclSWEntry),RT_ERR_RG_OK);
    aclSWEntry.acl_filter = *acl_filter;
    aclSWEntry.type = ACL_USE;


    //reAdd must add aclSWEtry in the assigned acl_filter_idx
    ASSERT_EQ(_rtk_rg_aclSWEntry_set(*acl_filter_idx,aclSWEntry),RT_ERR_RG_OK);

    //_rtk_rg_aclSWEntry_dump();

    return RT_ERR_RG_OK;
}



int _rtk_rg_aclSWEntry_dump(void)
{
    int i;
    rtk_rg_aclFilterEntry_t aclSWEntry, empty_aclSWEntry;
    bzero(&aclSWEntry,sizeof(aclSWEntry));
    bzero(&empty_aclSWEntry,sizeof(empty_aclSWEntry));


  debug("dump aclSWEntry");
    for(i=0; i<MAX_ACL_ENTRY_SIZE; i++)
    {
        _rtk_rg_aclSWEntry_get(i,&aclSWEntry);
        if(memcmp(&aclSWEntry,&empty_aclSWEntry,sizeof(rtk_rg_aclFilterEntry_t)))
            debug("aclSWEntry[%d]: aclstart=%d aclsize=%d cfstart=%d cfsize=%d type=%d\n",i,aclSWEntry.hw_aclEntry_start,aclSWEntry.hw_aclEntry_size,aclSWEntry.hw_cfEntry_start,aclSWEntry.hw_cfEntry_size,aclSWEntry.type);
    }
    return RT_ERR_RG_OK;
}

int _rtk_rg_aclFilterAndQos_check_testChip_feature(rtk_rg_aclFilterAndQos_t *acl_filter){

	rtk_portmask_t mac_pmsk;
	rtk_portmask_t ext_pmsk;

	if((acl_filter->filter_fields &EGRESS_INTF_BIT)|| 
		(acl_filter->filter_fields &EGRESS_IPV4_SIP_RANGE_BIT)||
		(acl_filter->filter_fields &EGRESS_IPV4_DIP_RANGE_BIT)||
		(acl_filter->filter_fields &EGRESS_L4_SPORT_RANGE_BIT)||
		(acl_filter->filter_fields &EGRESS_L4_DPORT_RANGE_BIT)){
		debug("TEST CHIP NOT SUPPORT CLASSIFY FIELD(EGRESS_IP, EGRESS_L4_PORT)!!!");
		return RT_ERR_RG_CHIP_NOT_SUPPORT;
	}

	if((acl_filter->filter_fields &INGRESS_PORT_BIT)){		
		ASSERT_EQ(_rtk_rg_portmask_translator(acl_filter->ingress_port_mask, &mac_pmsk, &ext_pmsk),RT_ERR_RG_OK);
		if(ext_pmsk.bits[0]!=0){
			debug("TEST CHIP NOT SUPPORT ACL EXT_PORT!!!");
			return RT_ERR_RG_CHIP_NOT_SUPPORT;
		}
	}

	return RT_ERR_RG_OK;
}

int _rtk_rg_aclFilterAndQos_check_notSupport_feature(rtk_rg_aclFilterAndQos_t *acl_filter){
	if((acl_filter->action_type==ACL_ACTION_TYPE_TRAP) &&((acl_filter->filter_fields &EGRESS_INTF_BIT)||(acl_filter->filter_fields &EGRESS_IPV4_SIP_RANGE_BIT)||(acl_filter->filter_fields &EGRESS_IPV4_DIP_RANGE_BIT)||(acl_filter->filter_fields &EGRESS_L4_SPORT_RANGE_BIT)||(acl_filter->filter_fields &EGRESS_L4_DPORT_RANGE_BIT))){
		rtlglue_printf("%s(%d):TRAP TO CPU NOT SUPPORT EGRESS FEATURE!!!",__func__,__LINE__);
		return RT_ERR_RG_CHIP_NOT_SUPPORT;
	}

	if(((acl_filter->filter_fields &INGRESS_IPV6_SIP_RANGE_BIT)||(acl_filter->filter_fields &INGRESS_IPV6_DIP_RANGE_BIT))&&((acl_filter->filter_fields &EGRESS_INTF_BIT)||(acl_filter->filter_fields &EGRESS_IPV4_SIP_RANGE_BIT)||(acl_filter->filter_fields &EGRESS_IPV4_DIP_RANGE_BIT)||(acl_filter->filter_fields &EGRESS_L4_SPORT_RANGE_BIT)||(acl_filter->filter_fields &EGRESS_L4_DPORT_RANGE_BIT))){
		rtlglue_printf("%s(%d):IPV6 SRC/DEST IP NOT SUPPORT WITH EGRESS PATTERN!!!",__func__,__LINE__);
		return RT_ERR_RG_CHIP_NOT_SUPPORT;
	}
	
	return RT_ERR_RG_OK;
}

#if 0
int32 _rtk_rg_acl_trap_broadcast_add(void)
{
	rtk_acl_ingress_entry_t aclRule;
	rtk_acl_field_t aclField0;
	rtk_acl_field_t aclField1;
	rtk_acl_field_t aclField2;
	bzero(&aclRule,sizeof(aclRule));
	bzero(&aclField0,sizeof(aclField0));
	bzero(&aclField1,sizeof(aclField1));
	bzero(&aclField2,sizeof(aclField2));
	aclRule.valid=ENABLED;
	aclRule.index = RESERVED_ACL_TRAP_BROADCAST;

	//DMAC ff:ff:ff:ff:ff:ff
	aclField0.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField0.fieldUnion.pattern.fieldIdx = 0;//template[0],field[0]
	aclField0.fieldUnion.data.value =0xffff;
	aclField0.fieldUnion.data.mask = 0xffff;
	assert_ok(rtk_acl_igrRuleField_add(&aclRule, &aclField0));

	aclField1.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField1.fieldUnion.pattern.fieldIdx = 1;//template[0],field[1]
	aclField1.fieldUnion.data.value =0xffff;
	aclField1.fieldUnion.data.mask = 0xffff;
	assert_ok(rtk_acl_igrRuleField_add(&aclRule, &aclField1));

	aclField2.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField2.fieldUnion.pattern.fieldIdx = 2;//template[0],field[2]
	aclField2.fieldUnion.data.value =0xffff;
	aclField2.fieldUnion.data.mask = 0xffff;
	assert_ok(rtk_acl_igrRuleField_add(&aclRule, &aclField2));


	aclRule.activePorts.bits[0]=RTK_RG_MAX_MAC_ALLPORTMASK;
	aclRule.templateIdx=0; /*use Ethertype: template[0],field[7]*/
	
	aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]=ENABLED;
	aclRule.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;	
	assert_ok(rtk_acl_igrRuleEntry_add(&aclRule));

	return RT_ERR_RG_OK;
}

int32 _rtk_rg_acl_trap_broadcast_del(void)
{
	int aclEntry;
	
	aclEntry = RESERVED_ACL_TRAP_BROADCAST;
	assert_ok(rtk_acl_igrRuleEntry_del(aclEntry));

	return RT_ERR_RG_OK;
}
#endif

/* patch for Slave GMAC packets recvice by special 1Q VID and PRI */
int32 _rtk_rg_internalVidPriTranslate(uint32 in_cvid, uint32 in_cpri, uint32 tran_cvid, uint32 tran_pri){

	rtk_acl_ingress_entry_t aclRule;
	rtk_acl_field_t aclField;

	bzero(&aclRule,sizeof(aclRule));
	bzero(&aclField,sizeof(aclField));

	aclRule.valid=ENABLED;
	aclRule.index=RESERVED_ACL_CVLAN_TRANSLATE;
	aclRule.activePorts.bits[0]=0x40;//filter packets from CPU port only(case for cpu1 sent to cpu2)
	aclRule.templateIdx=2; /*use :ACL_FIELD_CTAG => template[1],field[0]*/

	aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField.fieldUnion.pattern.fieldIdx = 0;/*template[2],field[0]*/
	aclField.fieldUnion.data.value = (in_cpri<<13)|in_cvid;
	aclField.fieldUnion.data.mask = 0xefff;
	assert_ok(rtk_acl_igrRuleField_add(&aclRule, &aclField));

	//translate cvid
	aclRule.act.enableAct[ACL_IGR_CVLAN_ACT]=ENABLED;
	aclRule.act.cvlanAct.act=ACL_IGR_CVLAN_IGR_CVLAN_ACT;
	aclRule.act.cvlanAct.cvid=tran_cvid;

	//translate internal pri (by aclPri)
	aclRule.act.enableAct[ACL_IGR_PRI_ACT]=ENABLED;
	aclRule.act.priAct.act=ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
	aclRule.act.priAct.aclPri=tran_pri;
	
	assert_ok(rtk_acl_igrRuleEntry_add(&aclRule));


	return RT_ERR_RG_OK;
	
}

int32 _rtk_rg_extPMaskTranslateRedirect_add(unsigned int igr_extPmsk,unsigned int egr_extPmsk){

	int i,j;
	rtk_acl_ingress_entry_t aclRule;
	rtk_acl_field_t aclField;
	//get empty ACL(from ACL[47~49])
	for(i=(RESERVED_ACL_EXTPORT_TRANSLATE_BASE-RESERVED_ACL_EXTPORT_TRANSLATE_SIZE);i<RESERVED_ACL_EXTPORT_TRANSLATE_BASE;i++){
		if(rg_db.systemGlobal.aclEntry_for_extPortTranslate[i-(RESERVED_ACL_EXTPORT_TRANSLATE_BASE-RESERVED_ACL_EXTPORT_TRANSLATE_SIZE)]==-1)
			break;
	}
	//reserved ACL is not enought
	if(i==RESERVED_ACL_EXTPORT_TRANSLATE_BASE){
		rtlglue_printf("ACL reserved for extPMaskTranslate is full!\n");
		return RT_ERR_RG_ACL_ENTRY_FULL;
	}


	//add acl fortranslate igr_extPmsk to egr_extPmsk
	bzero(&aclRule,sizeof(aclRule));
	bzero(&aclField,sizeof(aclField));
	aclRule.valid=ENABLED;
	aclRule.index=i;
	aclRule.activePorts.bits[0]=0x40;//filter packets from CPU port only
	aclRule.templateIdx=0; /*use :ACL_FIELD_EXT_PORTMASK => template[0],field[3]*/

	aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField.fieldUnion.pattern.fieldIdx = 3;
	aclField.fieldUnion.data.value = 0x0;
	for(j=0;j<RTK_RG_MAX_EXT_PORT;j++){
		if(!(igr_extPmsk&(1<<j))){
			aclField.fieldUnion.data.mask |= (1<<j); //band not allowed ext_port										
		}
	}
	assert_ok(rtk_acl_igrRuleField_add(&aclRule, &aclField));

	
	aclRule.act.enableAct[ACL_IGR_INTR_ACT]=ENABLED;
	aclRule.act.aclInterrupt=ENABLED;
	aclRule.act.extendAct.act=ACL_IGR_EXTEND_EXT_ACT;
	aclRule.act.extendAct.portMask.bits[0]=egr_extPmsk;
	assert_ok(rtk_acl_igrRuleEntry_add(&aclRule));

	rg_db.systemGlobal.aclEntry_for_extPortTranslate[i-(RESERVED_ACL_EXTPORT_TRANSLATE_BASE-RESERVED_ACL_EXTPORT_TRANSLATE_SIZE)]=i;
	rtlglue_printf("add extPMaskTranslate in ACL[%d]\n",i);

	return RT_ERR_RG_OK;
}


int32 _rtk_rg_extPMaskTranslateRedirect_del(int aclIdx){
	if(aclIdx>RESERVED_ACL_EXTPORT_TRANSLATE_BASE ||  aclIdx<(RESERVED_ACL_EXTPORT_TRANSLATE_BASE-RESERVED_ACL_EXTPORT_TRANSLATE_SIZE))
		return RT_ERR_RG_INVALID_PARAM;
	
	rg_db.systemGlobal.aclEntry_for_extPortTranslate[aclIdx-(RESERVED_ACL_EXTPORT_TRANSLATE_BASE-RESERVED_ACL_EXTPORT_TRANSLATE_SIZE)]=-1;
	assert_ok(rtk_acl_igrRuleEntry_del(aclIdx));

	return RT_ERR_RG_OK;
}

#ifdef CONFIG_RG_PPPOE_PASSTHROUGHT
int32 _rtk_rg_acl_pppoe_passthrough_add(unsigned int lan_pmask, unsigned int wan_pmsk, int remark_vid)
{
	rtk_acl_ingress_entry_t aclRule;
	rtk_acl_field_t aclField;
	rtk_classify_cfg_t cfRule;
	rtk_classify_field_t cfField;
	rtk_classify_ds_act_t dsAct;

	//add acl[58] for reamrking ethertype 0x8864 to bridged vid.(downstream)
	bzero(&aclRule,sizeof(aclRule));
	bzero(&aclField,sizeof(aclField));
	aclRule.valid=ENABLED;
	aclRule.index=RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE_FOR_ETHTER_8864;
		//just filter pppoe packet, do not care sessionID
	aclRule.activePorts.bits[0]=wan_pmsk;		//filter packets from WAN port only
	aclRule.careTag.tags[ACL_CARE_TAG_PPPOE].value=ENABLED;
	aclRule.careTag.tags[ACL_CARE_TAG_PPPOE].mask=0xffff;
	aclRule.templateIdx=3; /*use fieldSelector[15]: template[3],field[7]*/
	aclRule.act.enableAct[ACL_IGR_CVLAN_ACT]=ENABLED;
	aclRule.act.cvlanAct.act=ACL_IGR_CVLAN_IGR_CVLAN_ACT;
	aclRule.act.cvlanAct.cvid=remark_vid;
	aclRule.act.enableAct[ACL_IGR_INTR_ACT] = ENABLE; //latch to cf[RESERVED_CF_PPPOE_PASSTHROUGH_DOWNSTREAM_DMAC2CVID_ENTRY]
	aclRule.act.aclLatch = ENABLED;	
	assert_ok(rtk_acl_igrRuleEntry_add(&aclRule));

	//add cf[RESERVED_CF_PPPOE_PASSTHROUGH_DOWNSTREAM_DMAC2CVID_ENTRY] for force all downstream packet DMAC=>CVID action (downstream)	
	bzero(&cfRule,sizeof(cfRule));
	bzero(&dsAct,sizeof(dsAct));
    dsAct.cAct=CLASSIFY_DS_CACT_ADD_CTAG_8100;
    dsAct.cVidAct=CLASSIFY_DS_VID_ACT_FROM_LUT;
    dsAct.cPriAct=CLASSIFY_DS_PRI_ACT_FROM_INTERNAL;
    cfRule.index=RESERVED_CF_PPPOE_PASSTHROUGH_DOWNSTREAM_DMAC2CVID_ENTRY;
    cfRule.direction=CLASSIFY_DIRECTION_DS;
    cfRule.valid=ENABLED;
    cfRule.act.dsAct=dsAct;
	//latch by ACL[RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE_FOR_ETHTER_8864]
	cfField.fieldType = CLASSIFY_FIELD_ACL_HIT;
	cfField.classify_pattern.fieldData.value = ((1<<7) | RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE_FOR_ETHTER_8864); //(1<<7) is the valid bit
	cfField.classify_pattern.fieldData.mask=0xff;
	if(rtk_classify_field_add(&cfRule, &cfField))
	{
		return RT_ERR_RG_CF_ENTRY_ACCESS_FAILED;
	}
    assert_ok(rtk_classify_cfgEntry_add(&cfRule));


	
	//add acl[59] for pass all ethertype=0x8864 to fwdEngine (upstream)
	bzero(&aclRule,sizeof(aclRule));
	bzero(&aclField,sizeof(aclField));
	aclRule.valid=ENABLED;
	aclRule.index=RESERVED_ACL_PPPoE_PASSTHROUGHT_UPSTREAM_FOR_ETHTER_8864;
		//just filter pppoeTag packet
	aclRule.activePorts.bits[0]=lan_pmask&(~(0x1<<RTK_RG_MAC_PORT_CPU));		//filter packets from LAN port only, exclude CPU port
	aclRule.careTag.tags[ACL_CARE_TAG_PPPOE].value=ENABLED;
	aclRule.careTag.tags[ACL_CARE_TAG_PPPOE].mask=0xffff;
	aclRule.templateIdx=3; //unuse any field.
	aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]=ENABLED;
	aclRule.act.forwardAct.act=ACL_IGR_FORWARD_TRAP_ACT;//ACL_IGR_FORWARD_COPY_ACT;
	//aclRule.act.forwardAct.portMask.bits[0]=wan_pmsk;
	assert_ok(rtk_acl_igrRuleEntry_add(&aclRule));


	//add acl[60] for trap all ethertype=0x8863 to fwdEngine (upstream and downstream)
	bzero(&aclRule,sizeof(aclRule));
	bzero(&aclField,sizeof(aclField));
	aclRule.valid=ENABLED;
	aclRule.index=RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE_FOR_ETHTER_8863;
		//Ethertype 0x8863
	aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField.fieldUnion.pattern.fieldIdx = 7;//template[0],field[7]
	aclField.fieldUnion.data.value=0x8863;
	aclField.fieldUnion.data.mask=0xffff;
	assert_ok(rtk_acl_igrRuleField_add(&aclRule, &aclField));
	aclRule.activePorts.bits[0]=RTK_RG_MAX_MAC_ALLPORTMASK&(~(0x1<<RTK_RG_MAC_PORT_CPU));		//from CPU port should not trap again
	aclRule.templateIdx=0; /*use Ethertype: template[0],field[7]*/
	aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]=ENABLED;
	aclRule.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;	
	assert_ok(rtk_acl_igrRuleEntry_add(&aclRule));

	return SUCCESS;

}
int32 _rtk_rg_acl_pppoe_passthrough_del(void)
{
	int index;
	index = RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE_FOR_ETHTER_8864;
	assert_ok(rtk_acl_igrRuleEntry_del(index));
	index = RESERVED_CF_PPPOE_PASSTHROUGH_DOWNSTREAM_DMAC2CVID_ENTRY;
	assert_ok(rtk_classify_cfgEntry_del(index));
	index = RESERVED_ACL_PPPoE_PASSTHROUGHT_UPSTREAM_FOR_ETHTER_8864;
	assert_ok(rtk_acl_igrRuleEntry_del(index));
	index = RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE_FOR_ETHTER_8863;
	assert_ok(rtk_acl_igrRuleEntry_del(index));
	return SUCCESS;
}

int32 _rtk_rg_acl_pppoe_passthrough_for_wanIntf_add(int intf_idx, rtk_mac_t gmac)
{
	int i;
	rtk_acl_ingress_entry_t aclRule;
	rtk_acl_field_t aclField[3];
//	int aclEntry_used0=0,aclEntry_used1=0;
	int aclOccupy_size; 
	int aclOccupy_idx;
	int aclOccupy[RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_MAX_SIZE]={0};
	int aclRule_idx;
	bzero(&aclRule,sizeof(rtk_acl_ingress_entry_t));

	//for PPPoE downstream passthrought permit, use ACL[ 50~57, RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE(-1 ~-MAX_SIZE)]
	
	if(rg_db.systemGlobal.interfaceInfo[intf_idx].valid != 1){
		rtlglue_printf("%s(%d):intf_idx %d not valid\n",__func__,__LINE__,intf_idx);
		return RT_ERR_RG_INVALID_PARAM;
	}

	/*check the intf has been set relate acl*/
	if(rg_db.systemGlobal.aclEntry_for_pppoe_passthrought_downstream_perIntf_permit[intf_idx]!=0){//aclEntry_for_pppoe_passthrought_downstream_perIntf_permit[] record each interface used ACL[index].
		rtlglue_printf("%s(%d):intf[%d] has been permit by acl[%d]\n",__func__,__LINE__,intf_idx,rg_db.systemGlobal.aclEntry_for_pppoe_passthrought_downstream_perIntf_permit[intf_idx]);
		return RT_ERR_RG_FAILED;
	}

	/*check occupied acl[index] by aclOccupy[bool] */
	aclOccupy_size = 1; //at least reserve RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE
	aclOccupy_idx = 0;
	for(i=0;i<MAX_NETIF_HW_TABLE_SIZE;i++){
		if(rg_db.systemGlobal.aclEntry_for_pppoe_passthrought_downstream_perIntf_permit[i]!=0){
			aclOccupy_size++;
			if(aclOccupy_size >= RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_MAX_SIZE){
				return RT_ERR_RG_ACL_ENTRY_FULL;
			}else{
				//use aclOccupy[] to record used acl
				aclOccupy_idx = (RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE_FOR_ETHTER_8864 - rg_db.systemGlobal.aclEntry_for_pppoe_passthrought_downstream_perIntf_permit[i]);
				aclOccupy[aclOccupy_idx] = ENABLED;
			}
		}
	}
	/*search empty acl*/
	aclRule_idx=-1;
	for(i=1;i<RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_MAX_SIZE;i++){
		//i start from 1, i=0 reserved for RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE_FOR_ETHTER_8864
		if(aclOccupy[i]==DISABLED){
			aclRule_idx = RESERVED_ACL_PPPoE_SESSIONID_REMARKING_ENTRY_BASE_FOR_ETHTER_8864 - i;
			rg_db.systemGlobal.aclEntry_for_pppoe_passthrought_downstream_perIntf_permit[intf_idx] = aclRule_idx;
			break;
		}	
	}

	/*start to set ACL rule*/
	bzero(&aclRule,sizeof(aclRule));
	bzero(&aclField[0],sizeof(aclField));
	bzero(&aclField[1],sizeof(aclField));
	bzero(&aclField[2],sizeof(aclField));
	aclRule.valid=ENABLED;
	aclRule.index = aclRule_idx;
	aclRule.activePorts.bits[0]=RTK_RG_MAX_MAC_ALLPORTMASK;
	aclRule.careTag.tags[ACL_CARE_TAG_PPPOE].value=ENABLED; //filter 0x8864 
	aclRule.careTag.tags[ACL_CARE_TAG_PPPOE].mask=0xffff;
	aclRule.templateIdx=0; 
	//setup gmac
	aclField[0].fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField[0].fieldUnion.pattern.fieldIdx = 0; //DA[15:0]: template[0] field[0]
	aclField[0].fieldUnion.data.value = (gmac.octet[4]<<8) | (gmac.octet[5]);
	aclField[0].fieldUnion.data.mask = 0xffff;
	assert_ok(rtk_acl_igrRuleField_add(&aclRule, &aclField[0]));

	aclField[1].fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField[1].fieldUnion.pattern.fieldIdx = 1; //DA[31:16]: template[0] field[1]
	aclField[1].fieldUnion.data.value = (gmac.octet[2]<<8) | (gmac.octet[3]);
	aclField[1].fieldUnion.data.mask = 0xffff;
	assert_ok(rtk_acl_igrRuleField_add(&aclRule, &aclField[1]));

	aclField[2].fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField[2].fieldUnion.pattern.fieldIdx = 2; //DA[47:32]: template[0] field[2]
	aclField[2].fieldUnion.data.value = (gmac.octet[0]<<8) | (gmac.octet[1]);
	aclField[2].fieldUnion.data.mask = 0xffff;
	assert_ok(rtk_acl_igrRuleField_add(&aclRule, &aclField[2]));


	aclRule.act.enableAct[ACL_IGR_CVLAN_ACT]=ENABLED;
	aclRule.act.cvlanAct.act = ACL_IGR_CVLAN_MIB_ACT;
	aclRule.act.cvlanAct.mib = 32; //assigned a unused counter
	
	assert_ok(rtk_acl_igrRuleEntry_add(&aclRule));
	
	return RT_ERR_RG_OK;

}


int32 _rtk_rg_acl_pppoe_passthrough_for_wanIntf_del(int intf_idx){

	int aclEntry;
	/*check the intf is not empty*/
	if(rg_db.systemGlobal.aclEntry_for_pppoe_passthrought_downstream_perIntf_permit[intf_idx]==0){
		return RT_ERR_RG_INVALID_PARAM;
	}else{
		aclEntry = rg_db.systemGlobal.aclEntry_for_pppoe_passthrought_downstream_perIntf_permit[intf_idx];
	}
	assert_ok(rtk_acl_igrRuleEntry_del(aclEntry));
	rg_db.systemGlobal.aclEntry_for_pppoe_passthrought_downstream_perIntf_permit[intf_idx] = 0;
	return RT_ERR_RG_OK;
}
#endif

int32 _rtk_rg_shortCut_clear(void)
{
	int i;
	for(i=0;i<MAX_NAPT_SHORTCUT_SIZE;i++){
		bzero(&rg_db.naptShortCut[i],sizeof(rtk_rg_napt_shortcut_t));
	}
	return RT_ERR_RG_OK;
}
int32 rtk_rg_aclFilterAndQos_add(rtk_rg_aclFilterAndQos_t *acl_filter, int *acl_filter_idx)
{

    int acl_SWEntry_index=0;
    rtk_rg_aclFilterEntry_t aclSWEntry,empty_aclSWEntry;
    bzero(&aclSWEntry,sizeof(aclSWEntry));
    bzero(&empty_aclSWEntry,sizeof(empty_aclSWEntry));

	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;


    //check input parameter
    if(acl_filter == NULL
            || acl_filter_idx==NULL)
        return RT_ERR_RG_NULL_POINTER;

    if(acl_filter->filter_fields == 0x0)
        return RT_ERR_RG_INVALID_PARAM;

#ifdef CONFIG_APOLLO_RLE0371
	ASSERT_EQ(_rtk_rg_aclFilterAndQos_check_testChip_feature(acl_filter),RT_ERR_RG_OK);
#endif
	ASSERT_EQ(_rtk_rg_aclFilterAndQos_check_notSupport_feature(acl_filter),RT_ERR_RG_OK);


    ASSERT_EQ(_rtk_rg_aclSWEntry_and_asic_add(acl_filter,&aclSWEntry),RT_ERR_RG_OK);
    aclSWEntry.acl_filter = *acl_filter;
    aclSWEntry.type = ACL_USE;

    ASSERT_EQ(_rtk_rg_aclSWEntry_empty_find(&acl_SWEntry_index), RT_ERR_RG_OK);
    ASSERT_EQ(_rtk_rg_aclSWEntry_set(acl_SWEntry_index,aclSWEntry),RT_ERR_RG_OK);
	*acl_filter_idx = acl_SWEntry_index;


	rg_db.systemGlobal.acl_SW_table_entry_size++;
	debug("add aclSWEntry[%d]",*acl_filter_idx);
	assert_ok(_rtk_rg_shortCut_clear());
	
	
    return RT_ERR_RG_OK;
}

int32 rtk_rg_aclFilterAndQos_del(int acl_filter_idx)
{
	
    rtk_rg_aclFilterEntry_t aclSWEntry,empty_aclSWEntry;
    bzero(&aclSWEntry,sizeof(aclSWEntry));
    bzero(&empty_aclSWEntry,sizeof(empty_aclSWEntry));

	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

    ASSERT_EQ(_rtk_rg_aclSWEntry_get(acl_filter_idx, &aclSWEntry),RT_ERR_RG_OK);
    ASSERT_EQ(_rtk_rg_aclSWEntry_free_asic(aclSWEntry),RT_ERR_RG_OK);

    //clean aclSWEntry
    ASSERT_EQ(_rtk_rg_aclSWEntry_set(acl_filter_idx, empty_aclSWEntry),RT_ERR_RG_OK);


    //rearrange the ACL & CF ASIC to avoid discontinuous entry
    ASSERT_EQ(_rtk_rg_aclSWEntry_rearrange_asic(),RT_ERR_RG_OK);

	rg_db.systemGlobal.acl_SW_table_entry_size--;
	debug("delete aclSWEntry[%d]",acl_filter_idx);
	assert_ok(_rtk_rg_shortCut_clear());
    //_rtk_rg_aclSWEntry_dump();
    return RT_ERR_RG_OK;
}

int32 rtk_rg_aclFilterAndQos_find(rtk_rg_aclFilterAndQos_t *acl_filter, int *valid_idx)
{
    //search for the first not empty entry after valid_idx.
    int i;
    rtk_rg_aclFilterEntry_t aclSWEntry, empty_aclSWEntry;

    bzero(&aclSWEntry, sizeof(aclSWEntry));
    bzero(&empty_aclSWEntry, sizeof(empty_aclSWEntry));

	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;


	if(*valid_idx==-1){
		for(i=0; i<MAX_ACL_ENTRY_SIZE; i++){
			ASSERT_EQ(_rtk_rg_aclSWEntry_get(i,&aclSWEntry),RT_ERR_RG_OK);
			if(!memcmp(&(aclSWEntry.acl_filter),&acl_filter,sizeof(rtk_rg_aclFilterAndQos_t)))//search the same with acl_filter
	        {
	            *valid_idx=i;
	            *acl_filter = aclSWEntry.acl_filter;
				debug("rtk_rg_aclFilterAndQos_find index=%d",*valid_idx);
	            return RT_ERR_RG_OK;
	        }

		}
	}else{
	    for(i=*valid_idx; i<MAX_ACL_ENTRY_SIZE; i++)
	    {
	        ASSERT_EQ(_rtk_rg_aclSWEntry_get(i,&aclSWEntry),RT_ERR_RG_OK);

	        if(memcmp(&aclSWEntry,&empty_aclSWEntry,sizeof(rtk_rg_aclFilterEntry_t)))//search the different with empty_aclSWEntry
	        {
	            *valid_idx=i;
	            *acl_filter = aclSWEntry.acl_filter;
				debug("rtk_rg_aclFilterAndQos_find index=%d",*valid_idx);
	            return RT_ERR_RG_OK;
	        }
	    }
		debug("rtk_rg_aclFilterAndQos_find failed");
	}
	//not found
    return RT_ERR_RG_ACL_SW_ENTRY_NOT_FOUND;
}





//MAC Filter
//static rtk_rg_macFilterSWEntry_t* macFilter_table_Entry=NULL;

int _rtk_rg_is_macFilter_table_init(void)
{
    if(rg_db.systemGlobal.macFilter_table_Entry==NULL)
        return RT_ERR_RG_NULL_POINTER;
    else
        return RT_ERR_RG_OK;
}

int _rtk_rg_macFilter_table_init(void)
{
	if(rg_db.systemGlobal.macFilter_table_Entry!=NULL)
		rtk_rg_free(rg_db.systemGlobal.macFilter_table_Entry);

    rg_db.systemGlobal.macFilter_table_Entry = (rtk_rg_macFilterSWEntry_t *)(unsigned long)rtk_rg_malloc(MAX_MAC_FILTER_ENTRY_SIZE*sizeof(rtk_rg_macFilterSWEntry_t));
    if(rg_db.systemGlobal.macFilter_table_Entry==NULL)
    {
        return RT_ERR_RG_NULL_POINTER;
    }
    else
    {
        bzero(rg_db.systemGlobal.macFilter_table_Entry, sizeof(rtk_rg_macFilterSWEntry_t)*MAX_MAC_FILTER_ENTRY_SIZE);
    }

    return RT_ERR_RG_OK;
}


int32 rtk_rg_macFilter_add(rtk_rg_macFilterEntry_t *macFilterEntry,int *mac_filter_idx)
{
	int i=0,j=0;
    int macFilter_index;
    int index, search_index;
    uint32 default_fid[INTERFACE_END]={2,2}; //current lan/wan use SVL mode, both fid are 2.
    uint32 default_efid[INTERFACE_END]={0,0};
	uint32 fid;
	uint32 efid;
    rtk_l2_addr_table_t lut;
    bzero(&lut,sizeof(lut));

	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

    if(macFilterEntry==NULL || mac_filter_idx==NULL)
        return RT_ERR_RG_NULL_POINTER;

    if(_rtk_rg_is_macFilter_table_init())
        ASSERT_EQ(_rtk_rg_macFilter_table_init(),RT_ERR_RG_OK);

    //search empty macFilterEntry
    for(macFilter_index=0; macFilter_index<MAX_MAC_FILTER_ENTRY_SIZE; macFilter_index++)
    {
        if(rg_db.systemGlobal.macFilter_table_Entry[macFilter_index].valid==DISABLE)
            break;
        if(macFilter_index == (MAX_MAC_FILTER_ENTRY_SIZE-1))
            return RT_ERR_RG_L2_MACFILTER_ENTRY_FULL;
    }

	//for(i=0;i<INTERFACE_END;i++){/* i==0 for lan_intf, i==1 for wan_intf*/
	for(i=0;i<INTERFACE_FOR_WAN;i++){ /*Lan/Wan use same fid, efid, vid and both are svl in current architecture(same lut index)=>just add/del once */
		switch(i){
			case INTERFACE_FOR_LAN:
				fid = default_fid[INTERFACE_FOR_LAN];
				efid = default_efid[INTERFACE_FOR_LAN];
				break;
			case INTERFACE_FOR_WAN:
				fid = default_fid[INTERFACE_FOR_WAN];
				efid = default_efid[INTERFACE_FOR_WAN];
				break;	
			default:
				return RT_ERR_RG_L2_MACFILTER_ENTRY_ACCESS_FAILED;
				break;	
		}

	    //find lut index
		index=_rtk_rg_hash_mac_fid_efid(macFilterEntry->mac.octet,fid,efid);
	    index<<=2;
		
	    search_index = index;
		j=0;
		do
	    {
	        if(rtk_l2_nextValidEntry_get(&search_index,&lut)){
				return RT_ERR_RG_L2_ENTRY_ACCESS_FAILED;
	        }

		    if((!memcmp(&lut.entry.l2UcEntry.mac, &(macFilterEntry->mac), sizeof(rtk_mac_t) )) && lut.entry.l2UcEntry.fid==fid){
				break;
	        }else{
	            search_index = lut.entry.l2UcEntry.index +1; //search from next entry
			}
			j++;
		}	
	    while(j < 4);//at most search 4 times.

		if((!memcmp(&lut.entry.l2UcEntry.mac, &(macFilterEntry->mac), sizeof(rtk_mac_t) )) && lut.entry.l2UcEntry.fid==fid)
	    {
	        //the l2 entry is found, change some flags & auth
	        lut.entry.l2UcEntry.auth=1;

			if(macFilterEntry->direct==RTK_RG_MACFILTER_FILTER_SRC_MAC_ONLY){
				lut.entry.l2UcEntry.flags|=RTK_L2_UCAST_FLAG_STATIC|RTK_L2_UCAST_FLAG_SA_BLOCK;
			}else if(macFilterEntry->direct==RTK_RG_MACFILTER_FILTER_DEST_MAC_ONLY){
				lut.entry.l2UcEntry.flags|=RTK_L2_UCAST_FLAG_STATIC|RTK_L2_UCAST_FLAG_DA_BLOCK;
			}else if(macFilterEntry->direct==RTK_RG_MACFILTER_FILTER_SRC_DEST_MAC_BOTH){
				lut.entry.l2UcEntry.flags|=RTK_L2_UCAST_FLAG_STATIC|RTK_L2_UCAST_FLAG_SA_BLOCK|RTK_L2_UCAST_FLAG_DA_BLOCK;
			}else{//not defined value
			}

		}
	    else
	    {
	        //the l2 entry is not found, set the l2 entry.
	        bzero(&lut,sizeof(lut));
	        lut.entryType = RTK_LUT_L2UC;
	        lut.entry.l2UcEntry.vid=0;
		    lut.entry.l2UcEntry.fid=fid;
		    lut.entry.l2UcEntry.efid=efid;
	        lut.entry.l2UcEntry.mac = macFilterEntry->mac;
	        lut.entry.l2UcEntry.auth=1;
	        if(macFilterEntry->direct==RTK_RG_MACFILTER_FILTER_SRC_MAC_ONLY){
				lut.entry.l2UcEntry.flags|=RTK_L2_UCAST_FLAG_STATIC|RTK_L2_UCAST_FLAG_SA_BLOCK;
			}else if(macFilterEntry->direct==RTK_RG_MACFILTER_FILTER_DEST_MAC_ONLY){
				lut.entry.l2UcEntry.flags|=RTK_L2_UCAST_FLAG_STATIC|RTK_L2_UCAST_FLAG_DA_BLOCK;
			}else if(macFilterEntry->direct==RTK_RG_MACFILTER_FILTER_SRC_DEST_MAC_BOTH){
				lut.entry.l2UcEntry.flags|=RTK_L2_UCAST_FLAG_STATIC|RTK_L2_UCAST_FLAG_SA_BLOCK|RTK_L2_UCAST_FLAG_DA_BLOCK;
			}else{//not defined value
			}
	    }
	    ASSERT_EQ(RTK_L2_ADDR_ADD(&lut.entry.l2UcEntry),RT_ERR_OK);
		//memcpy(rg_db.mac[lut.entry.l2UcEntry.index].macAddr.octet,lut.entry.l2UcEntry.mac.octet,6);
	    DEBUG("add mac filter at lut[%d]",lut.entry.l2UcEntry.index);

	    //setup software macFilterEntry
	    *mac_filter_idx = macFilter_index;
	    rg_db.systemGlobal.macFilter_table_Entry[macFilter_index].valid=ENABLE;
	    rg_db.systemGlobal.macFilter_table_Entry[macFilter_index].macFilterEntry = *macFilterEntry;
		rg_db.systemGlobal.macFilter_table_Entry[macFilter_index].l2_table_entry_index = lut.entry.l2UcEntry.index;
		//if(i==INTERFACE_FOR_LAN)
	    	//rg_db.systemGlobal.macFilter_table_Entry[macFilter_index].l2_table_entry_index_for_lan= lut.entry.l2UcEntry.index;//not used
		//if(i==INTERFACE_FOR_WAN)
			//rg_db.systemGlobal.macFilter_table_Entry[macFilter_index].l2_table_entry_index_for_wan= lut.entry.l2UcEntry.index;//not used

	}

    return RT_ERR_RG_OK;
}

int32 rtk_rg_macFilter_del(int mac_filter_idx)
{		
    uint32 default_fid[INTERFACE_END]={2,2};
    uint32 default_efid[INTERFACE_END]={0,0};
	uint32 default_vid[INTERFACE_END]={0,0};

    rtk_l2_ucastAddr_t lut;
    bzero(&lut,sizeof(lut));

	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

    if(_rtk_rg_is_macFilter_table_init())
        ASSERT_EQ(_rtk_rg_macFilter_table_init(),RT_ERR_RG_OK);

    if((mac_filter_idx > MAX_MAC_FILTER_ENTRY_SIZE) || (rg_db.systemGlobal.macFilter_table_Entry[mac_filter_idx].valid==DISABLE))
        return RT_ERR_RG_INVALID_PARAM;


	//delete index for lan		
    bzero(&lut,sizeof(lut));
    lut.mac = rg_db.systemGlobal.macFilter_table_Entry[mac_filter_idx].macFilterEntry.mac;
    lut.fid = default_fid[INTERFACE_FOR_LAN];
    lut.efid = default_efid[INTERFACE_FOR_LAN];
    lut.vid = default_vid[INTERFACE_FOR_LAN];

	//lut.index = rg_db.systemGlobal.macFilter_table_Entry[mac_filter_idx].l2_table_entry_index_for_lan;
	lut.index = rg_db.systemGlobal.macFilter_table_Entry[mac_filter_idx].l2_table_entry_index;
	DEBUG("del lut[%d]",lut.index);
	ASSERT_EQ(RTK_L2_ADDR_DEL(&lut),RT_ERR_OK);

	
#if 0 //Lan/Wan use same fid, efid, vid and both are svl in current architecture (same lut index)=>just add/del once
	//delete index for wan		
    bzero(&lut,sizeof(lut));
    lut.mac = rg_db.systemGlobal.macFilter_table_Entry[mac_filter_idx].macFilterEntry.mac;
    lut.fid = default_fid[INTERFACE_FOR_WAN];
    lut.efid = default_efid[INTERFACE_FOR_WAN];
    lut.vid = default_vid[INTERFACE_FOR_WAN];
	//lut.index = rg_db.systemGlobal.macFilter_table_Entry[mac_filter_idx].l2_table_entry_index_for_wan;
	lut.index = rg_db.systemGlobal.macFilter_table_Entry[mac_filter_idx].l2_table_entry_index;
	DEBUG("del lut[%d]",lut.index);
	ASSERT_EQ(RTK_L2_ADDR_DEL(&lut),RT_ERR_OK);
#endif

    rg_db.systemGlobal.macFilter_table_Entry[mac_filter_idx].valid =DISABLE;
    bzero(&rg_db.systemGlobal.macFilter_table_Entry[mac_filter_idx].macFilterEntry,sizeof(rtk_rg_macFilterEntry_t));
	rg_db.systemGlobal.macFilter_table_Entry[mac_filter_idx].l2_table_entry_index = 0;
	//rg_db.systemGlobal.macFilter_table_Entry[mac_filter_idx].l2_table_entry_index_for_lan= 0;//not used
	//rg_db.systemGlobal.macFilter_table_Entry[mac_filter_idx].l2_table_entry_index_for_wan= 0;//not used

    return RT_ERR_RG_OK;
}

int32 rtk_rg_macFilter_find(rtk_rg_macFilterEntry_t *macFilterEntry, int *valid_idx)
{
    int macFilter_index;

	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

    if(macFilterEntry==NULL || valid_idx==NULL)
        return RT_ERR_RG_NULL_POINTER;

    if(_rtk_rg_is_macFilter_table_init())
        ASSERT_EQ(_rtk_rg_macFilter_table_init(),RT_ERR_RG_OK);

    //search empty macFilterEntry
    for(macFilter_index=*valid_idx; macFilter_index<MAX_MAC_FILTER_ENTRY_SIZE; macFilter_index++)
    {
        if(rg_db.systemGlobal.macFilter_table_Entry[macFilter_index].valid==ENABLE)
        {
            *valid_idx = macFilter_index;
            *macFilterEntry = rg_db.systemGlobal.macFilter_table_Entry[macFilter_index].macFilterEntry;
            return RT_ERR_RG_OK;
        }
    }

    return RT_ERR_RG_L2_MACFILTER_ENTRY_NOT_FOUND;
}

//URL Filter
#if 1
//rtk_rg_urlFilterEntry_t *urlFilter_table_entry;


int _rtk_rg_is_urlFilter_table_init(void)
{
    if(rg_db.systemGlobal.urlFilter_table_entry==NULL)
        return RT_ERR_RG_NULL_POINTER;
    else
        return RT_ERR_RG_OK;
}

int _rtk_rg_urlFilter_table_init(void)
{
	int i;

	if(rg_db.systemGlobal.urlFilter_table_entry!=NULL)
		rtk_rg_free(rg_db.systemGlobal.urlFilter_table_entry);

    rg_db.systemGlobal.urlFilter_table_entry = (rtk_rg_urlFilterEntry_t *)(unsigned long)rtk_rg_malloc(MAX_URL_FILTER_ENTRY_SIZE*sizeof(rtk_rg_urlFilterEntry_t));
    if(rg_db.systemGlobal.urlFilter_table_entry==NULL)
    {
        return RT_ERR_RG_NULL_POINTER;
    }
    else
    {
        bzero(rg_db.systemGlobal.urlFilter_table_entry, sizeof(rtk_rg_urlFilterEntry_t)*MAX_URL_FILTER_ENTRY_SIZE);
    }

	for(i=0;i<MAX_URL_FILTER_ENTRY_SIZE;i++){
		rg_db.systemGlobal.urlFilter_valid_entry[i] = -1;
	}
	
    return RT_ERR_RG_OK;
} 

int32 rtk_rg_urlFilterString_add(rtk_rg_urlFilterString_t *filter,int *url_idx)
{
	
	rtk_acl_ingress_entry_t aclRule;
	rtk_acl_field_t aclField;
	int i,urlFilter_index;	

	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;
	
	if(filter==NULL || url_idx==NULL)
		return RT_ERR_RG_NULL_POINTER;

    if(_rtk_rg_is_urlFilter_table_init())
        ASSERT_EQ(_rtk_rg_urlFilter_table_init(),RT_ERR_RG_OK);

	 //search empty urlFilterEntry
	for(urlFilter_index=0;urlFilter_index<MAX_URL_FILTER_ENTRY_SIZE;urlFilter_index++){
		if(rg_db.systemGlobal.urlFilter_table_entry[urlFilter_index].valid==DISABLE){		
			rg_db.systemGlobal.urlFilter_table_entry[urlFilter_index].urlFilter= *filter;
			rg_db.systemGlobal.urlFilter_table_entry[urlFilter_index].valid = ENABLE;
			break;
		}
			
		if(urlFilter_index == (MAX_URL_FILTER_ENTRY_SIZE-1))
            return RT_ERR_RG_URLFILTER_ENTRY_FULL;
	}


	//add acl rule for trap dport:80
	bzero(&aclRule,sizeof(aclRule));
	bzero(&aclField,sizeof(aclField));
/*
	bzero(&portRangeEntry,sizeof(portRangeEntry));

	portRangeEntry.index=15;
	portRangeEntry.lower_bound=80;
	portRangeEntry.upper_bound=80;
	portRangeEntry.type=PORTRANGE_DPORT;
	assert_ok(rtk_acl_portRange_set(&portRangeEntry));
*/
	aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField.fieldUnion.pattern.fieldIdx = 0; //portRange in template[1]:field[0]
	aclField.fieldUnion.data.value = 80; //dport:80 in rangeTable[15]
	aclField.fieldUnion.data.mask = 0xffff;
	if(rtk_acl_igrRuleField_add(&aclRule, &aclField))
    {
        return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
    }
	
	aclRule.valid = ENABLE;
	aclRule.index = RESERVED_ACL_URLFILTER_ENTRY;
	aclRule.templateIdx = 1;
	aclRule.activePorts.bits[0] = RTK_RG_MAX_MAC_ALLPORTMASK & (~(1<<RTK_RG_MAC_PORT_CPU));
	//trap to cpu action
	aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLE;
	aclRule.act.forwardAct.act= ACL_IGR_FORWARD_TRAP_ACT;		
    if(rtk_acl_igrRuleEntry_add(&aclRule))
    {
        debug("adding aclRule to aclEntry[%d] failed",aclRule.index);
        return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
    }else{
		*url_idx = urlFilter_index;
		for(i=0;i<MAX_URL_FILTER_ENTRY_SIZE;i++){
			if(rg_db.systemGlobal.urlFilter_valid_entry[i]==-1){
				rg_db.systemGlobal.urlFilter_valid_entry[i]=urlFilter_index;
				break;
			}
		}
	}


	//set ALG http tcp for avoiding flow connect in shortcut
	_rtk_rg_alg_setPort(80,1,ENABLED);		//enable HTTP TCP port:80


#if 0 //url can not handeled by HW, because the auto fragment
	rtk_acl_ingress_entry_t aclRule;
	rtk_acl_field_t aclField;
	int i,urlFilter_index;	
	bzero(&aclRule,sizeof(aclRule));
	bzero(&aclField,sizeof(aclField));

	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;
	
	if(filter==NULL || url_idx==NULL)
		return RT_ERR_RG_NULL_POINTER;

    if(_rtk_rg_is_urlFilter_table_init())
        ASSERT_EQ(_rtk_rg_urlFilter_table_init(),RT_ERR_RG_OK);

	 //search empty urlFilterEntry
	for(urlFilter_index=0;urlFilter_index<MAX_URL_FILTER_ENTRY_SIZE;urlFilter_index++){
		if(rg_db.systemGlobal.urlFilter_table_entry[urlFilter_index].valid==DISABLE){		
			rg_db.systemGlobal.urlFilter_table_entry[urlFilter_index].urlFilter= *filter;
			rg_db.systemGlobal.urlFilter_table_entry[urlFilter_index].valid = ENABLE;
			break;
		}
			
		if(urlFilter_index == (MAX_URL_FILTER_ENTRY_SIZE-1))
            return RT_ERR_RG_URLFILTER_ENTRY_FULL;
	}

	//add rule for acl field selector 3(in template[2]) for keyword "GE" 
	bzero(&aclRule,sizeof(aclRule));
	bzero(&aclField,sizeof(aclField));
	aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField.fieldUnion.pattern.fieldIdx = 3; //field selector[03] in template[2]
	aclField.fieldUnion.data.value = 0x4745; //keyword "GE"
	aclField.fieldUnion.data.mask = 0xffff;

	if(rtk_acl_igrRuleField_add(&aclRule, &aclField))
    {
        debug("adding aclRule field[3] for pattern \"GE\" failed");
        return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
    }
	
	aclRule.valid = ENABLE;
	aclRule.index = RESERVED_ACL_URLFILTER_GET_ENTRY;
	aclRule.templateIdx = 2;
	aclRule.activePorts.bits[0] = RTK_RG_MAX_MAC_ALLPORTMASK & (~(1<<RTK_RG_MAC_PORT_CPU));
	//trap to cpu action
	aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLE;
	aclRule.act.forwardAct.act= ACL_IGR_FORWARD_TRAP_ACT;		
    if(rtk_acl_igrRuleEntry_add(&aclRule))
    {
        debug("adding aclRule to aclEntry[%d] failed",aclRule.index);
        return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
    }


	
	//add rule for acl field selector 4(in template[2]) for keyword "PO" 
	bzero(&aclRule,sizeof(aclRule));
	bzero(&aclField,sizeof(aclField));
	aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
	aclField.fieldUnion.pattern.fieldIdx = 3; //field selector[03] in template[2]
	aclField.fieldUnion.data.value = 0x504F;//keyword "PO"
	aclField.fieldUnion.data.mask = 0xffff;

	if(rtk_acl_igrRuleField_add(&aclRule, &aclField))
    {
        debug("adding aclRule field[4] for pattern \"PO\" failed");
        return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
    }
	
	aclRule.valid = ENABLE;
	aclRule.index = RESERVED_ACL_URLFILTER_POST_ENTRY;
	aclRule.templateIdx = 2;
	aclRule.activePorts.bits[0] = RTK_RG_MAX_MAC_ALLPORTMASK & (~(1<<RTK_RG_MAC_PORT_CPU));
	//trap to cpu action
	aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLE;
	aclRule.act.forwardAct.act= ACL_IGR_FORWARD_TRAP_ACT;		
    if(rtk_acl_igrRuleEntry_add(&aclRule))
    {
        debug("adding aclRule to aclEntry[%d] failed",aclRule.index);
        return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
    }else{
		*url_idx = urlFilter_index;
		for(i=0;i<MAX_URL_FILTER_ENTRY_SIZE;i++){
			if(rg_db.systemGlobal.urlFilter_valid_entry[i]==-1){
				rg_db.systemGlobal.urlFilter_valid_entry[i]=urlFilter_index;
				break;
			}
		}
	}
#endif	
    return RT_ERR_RG_OK;
}

int32 rtk_rg_urlFilterString_del(int url_idx)
{
	int i,index=0;
	int empty_flag = DISABLED; // avoid access urlFilter_valid_entry[-1] makes kernal panic while call rtk_rg_urlFilterString_del() when no any rule has been set.
	
	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

	if(_rtk_rg_is_urlFilter_table_init())
		 ASSERT_EQ(_rtk_rg_urlFilter_table_init(),RT_ERR_RG_OK);

	/*delete the assigned Entry*/
	bzero(&rg_db.systemGlobal.urlFilter_table_entry[url_idx],sizeof(rg_db.systemGlobal.urlFilter_table_entry[url_idx]));

	/*delete urlFilter_valid_entry record*/
	//search the relate index
	for(i=0;i<MAX_URL_FILTER_ENTRY_SIZE;i++){
		if(rg_db.systemGlobal.urlFilter_valid_entry[i]==url_idx){
			index=i;//record the valid entry index
			break;
		}
	
	}

	//if index is not found, do not access urlFilter_valid_entry[index]
	if(i==MAX_URL_FILTER_ENTRY_SIZE){
		empty_flag = ENABLED; //index not found
		debug("%s: valid index not found",__func__);
	}

	if(empty_flag != ENABLED){
		for(i=0;i<MAX_URL_FILTER_ENTRY_SIZE;i++){
			if(rg_db.systemGlobal.urlFilter_valid_entry[i]==-1){
				debug("%s: index=%d i=%d",__func__,index,i);
				rg_db.systemGlobal.urlFilter_valid_entry[index]=rg_db.systemGlobal.urlFilter_valid_entry[i-1];//replace by the last valid entry
				rg_db.systemGlobal.urlFilter_valid_entry[i-1]=-1;//clean last valid entry
				break;
			}
		}
	}
		
	/* search and check is there any urlFilterEntry Enabled, if not, delete the ACL for dport:80 */
	for(i=0;i<MAX_URL_FILTER_ENTRY_SIZE;i++){
		if(rg_db.systemGlobal.urlFilter_table_entry[i].valid==ENABLE){
			break;
		}

		//no urlFilter is need, delete ACL
		if(i == (MAX_URL_FILTER_ENTRY_SIZE-1)){
			if(rtk_acl_igrRuleEntry_del(RESERVED_ACL_URLFILTER_ENTRY)){
				return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
			}
			_rtk_rg_alg_setPort(80,1,DISABLED);		//enable HTTP TCP port:80
		}	
	}


#if 0 //url can not handeled by HW, because the auto fragment
	int i,index=0;
	int empty_flag = DISABLED; // avoid access urlFilter_valid_entry[-1] makes kernal panic while call rtk_rg_urlFilterString_del() when no any rule has been set.
	
	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

	if(_rtk_rg_is_urlFilter_table_init())
		 ASSERT_EQ(_rtk_rg_urlFilter_table_init(),RT_ERR_RG_OK);

	/*delete the assigned Entry*/
	bzero(&rg_db.systemGlobal.urlFilter_table_entry[url_idx],sizeof(rg_db.systemGlobal.urlFilter_table_entry[url_idx]));

	/*delete urlFilter_valid_entry record*/
	//search the relate index
	for(i=0;i<MAX_URL_FILTER_ENTRY_SIZE;i++){
		if(rg_db.systemGlobal.urlFilter_valid_entry[i]==url_idx){
			index=i;//record the valid entry index
			break;
		}
	
	}

	//if index is not found, do not access urlFilter_valid_entry[index]
	if(i==MAX_URL_FILTER_ENTRY_SIZE){
		empty_flag = ENABLED; //index not found
		debug("%s: valid index not found",__func__);
	}

	if(empty_flag != ENABLED){
		for(i=0;i<MAX_URL_FILTER_ENTRY_SIZE;i++){
			if(rg_db.systemGlobal.urlFilter_valid_entry[i]==-1){
				debug("%s: index=%d i=%d",__func__,index,i);
				rg_db.systemGlobal.urlFilter_valid_entry[index]=rg_db.systemGlobal.urlFilter_valid_entry[i-1];//replace by the last valid entry
				rg_db.systemGlobal.urlFilter_valid_entry[i-1]=-1;//clean last valid entry
				break;
			}
		}
	}
		
	/* search and check is there any urlFilterEntry Enabled, if not, delete the ACL for keyword "GET " & "POST" */
	for(i=0;i<MAX_URL_FILTER_ENTRY_SIZE;i++){
		if(rg_db.systemGlobal.urlFilter_table_entry[i].valid==ENABLE){
			break;
		}

		//no urlFilter is need, delete ACL
		if(i == (MAX_URL_FILTER_ENTRY_SIZE-1)){
			if(rtk_acl_igrRuleEntry_del(RESERVED_ACL_URLFILTER_GET_ENTRY)){
				return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
			}

			if(rtk_acl_igrRuleEntry_del(RESERVED_ACL_URLFILTER_POST_ENTRY)){
				return RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED;
			}
		}	
	}
#endif

    return RT_ERR_RG_OK;
}

int32 rtk_rg_urlFilterString_find(rtk_rg_urlFilterString_t *filter, int *valid_idx)
{
	int urlFilter_index;
	
	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

	if(filter==NULL || valid_idx==NULL)
		return RT_ERR_RG_NULL_POINTER;

	if(_rtk_rg_is_urlFilter_table_init())
		 ASSERT_EQ(_rtk_rg_urlFilter_table_init(),RT_ERR_RG_OK);

	for(urlFilter_index=*valid_idx;urlFilter_index<MAX_URL_FILTER_ENTRY_SIZE;urlFilter_index++){
		if(rg_db.systemGlobal.urlFilter_table_entry[urlFilter_index].valid==ENABLE){
			*valid_idx = urlFilter_index;
			*filter = rg_db.systemGlobal.urlFilter_table_entry[urlFilter_index].urlFilter;
			return RT_ERR_RG_OK;
		}
	}

    return RT_ERR_RG_URLFILTER_ENTRY_NOT_FOUND;
}
#endif

//UPnP
int32 rtk_rg_upnpConnection_add(rtk_rg_upnpConnection_t *upnp, int *upnp_idx)
{
	int i=0;

	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

	// Get free entry
	for(i=0;i<MAX_UPNP_SW_TABLE_SIZE;i++)
	{
		if(rg_db.upnp[i].valid) continue;
		else break;
	}
	if(i==MAX_UPNP_SW_TABLE_SIZE) return RT_ERR_RG_ENTRY_FULL;

	// Add UPNP mapping
	*upnp_idx = i;
	rg_db.upnp[i].is_tcp = upnp->is_tcp;
	rg_db.upnp[i].wan_intf_idx = upnp->wan_intf_idx;
	rg_db.upnp[i].gateway_port = upnp->gateway_port;
	rg_db.upnp[i].local_ip = upnp->local_ip;
	rg_db.upnp[i].local_port = upnp->local_port;
	rg_db.upnp[i].limit_remote_ip = upnp->limit_remote_ip;
	rg_db.upnp[i].limit_remote_port = upnp->limit_remote_port;
	rg_db.upnp[i].remote_ip = upnp->remote_ip;
	rg_db.upnp[i].remote_port = upnp->remote_port;
	rg_db.upnp[i].type = upnp->type;
	rg_db.upnp[i].timeout = upnp->timeout;
	rg_db.upnp[i].valid = 1;

	rg_db.systemGlobal.upnpGroup[rg_db.systemGlobal.upnpTotalNum].index = *upnp_idx;
	rg_db.systemGlobal.upnpGroup[rg_db.systemGlobal.upnpTotalNum].p_upnp = &rg_db.upnp[i];
	rg_db.systemGlobal.upnpTotalNum++;
	
    return RT_ERR_RG_OK;
}

int32 rtk_rg_upnpConnection_del(int upnp_idx)
{
	int i=0;
	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

	if( (upnp_idx<0) || (upnp_idx>=MAX_UPNP_SW_TABLE_SIZE)) return RT_ERR_RG_INVALID_PARAM;

	memset(&rg_db.upnp[upnp_idx],0,sizeof(rtk_rg_upnpConnection_t));

	/* UPNP Group table */
	for(i=0;i<rg_db.systemGlobal.upnpTotalNum;i++)
	{
		if(rg_db.systemGlobal.upnpGroup[i].index == upnp_idx)
		{
			if(i==(rg_db.systemGlobal.upnpTotalNum-1))
			{
				//The last entry
				rg_db.systemGlobal.upnpGroup[i].index = 0;
				rg_db.systemGlobal.upnpGroup[i].p_upnp = NULL;
			}
			else
			{
				//Replace the deleteing entry by last one
				rg_db.systemGlobal.upnpGroup[i].index = rg_db.systemGlobal.upnpGroup[rg_db.systemGlobal.upnpTotalNum-1].index;
				rg_db.systemGlobal.upnpGroup[i].p_upnp = rg_db.systemGlobal.upnpGroup[rg_db.systemGlobal.upnpTotalNum-1].p_upnp;
				rg_db.systemGlobal.upnpGroup[rg_db.systemGlobal.upnpTotalNum-1].index = 0;
				rg_db.systemGlobal.upnpGroup[rg_db.systemGlobal.upnpTotalNum-1].p_upnp = NULL;
			}
			rg_db.systemGlobal.upnpTotalNum--;
			break;
		}
	}
	
    return RT_ERR_RG_OK;
}

int32 rtk_rg_upnpConnection_find(rtk_rg_upnpConnection_t *upnp, int *valid_idx)
{
	int idx=0;
	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

	if((upnp->wan_intf_idx < 0) || (upnp->wan_intf_idx >=MAX_EXTIP_HW_TABLE_SIZE)) return RT_ERR_RG_INVALID_PARAM;

	//Find UPNP mapping
	for(idx=*valid_idx;idx<MAX_UPNP_SW_TABLE_SIZE;idx++)
	{
		if(rg_db.upnp[idx].valid)
		{
			*valid_idx=idx;
			memcpy(upnp,&rg_db.upnp[idx],sizeof(rtk_rg_upnpConnection_t));
			return RT_ERR_RG_OK;
		}
	}
    return RT_ERR_RG_UPNP_SW_ENTRY_NOT_FOUND;
}

uint32 _rtk_rg_naptTcpUdpInHashIndex(uint16 isTcp, uint32 dip, uint16 dport)
{
	uint32 eidx=0;

	/*
		SUM = added by 1's complementary arithmetic( DIP[7:0] + DIP[15:8] + DIP[23:16] + DIP[31:24] + DPOR[7:0] )
		NAPTR hash ID[7:0] = SUM[7:0] ^ DPORT[15:8] ^ ( TCP << 7)A!KA!K.. HashIN
	*/
	eidx = ((dip&0xff) + ((dip>>8)&0xff) + ((dip>>16)&0xff) + ((dip>>24)&0xff) + (dport&0xff));
	eidx = ((eidx&0xff) + ((eidx>>8)&0xff)) & 0xff;
	eidx = (eidx&0xff) ^ ((dport>>8)&0xff) ^ (isTcp << 7);

	return eidx;
}

uint32 _rtk_rg_naptTcpUdpOutHashIndex(int8 isTcp, ipaddr_t srcAddr, uint16 srcPort, ipaddr_t destAddr, uint16 destPort) 
{
	uint32 eidx;

	eidx = (((destAddr&0x3)<<16) | srcPort) + ((destAddr>>2)&0x3ffff);
	eidx = (eidx&0x3ffff) + (eidx>>18);
	eidx += (((srcAddr&0x3f)<<12) | (destAddr>>20));
	eidx = (eidx&0x3ffff) + (eidx>>18);
	eidx += ((srcAddr>>6)&0x3ffff);
	eidx = (eidx&0x3ffff) + (eidx>>18);
	eidx += (((destPort&0x3ff)<<8) | (srcAddr>>24));
	eidx = (eidx&0x3ffff) + (eidx>>18);
	eidx = (eidx&0x1ff) + (eidx>>9);
	eidx = eidx + ((isTcp<<8) | ((destPort>>10)<<2));
	eidx = ((eidx&0x1ff) + (eidx>>9))&0x1ff;

    assert((eidx<<2) < MAX_NAPT_OUT_HW_TABLE_SIZE);
    return eidx;
}

int32 _rtk_rg_naptConnection_add(int outIdx, rtk_l34_naptOutbound_entry_t *asic_napt, rtk_l34_naptInbound_entry_t *asic_naptr)
{
	//Write to ASIC
	int retval = 0;
	rtk_rg_naptInfo_t naptInfo;
	int inIdx = asic_napt->hashIdx;
	if((inIdx<MAX_NAPT_IN_HW_TABLE_SIZE) && (outIdx<MAX_NAPT_OUT_HW_TABLE_SIZE))
	{
		//NAPT-R
		retval = RTK_L34_NAPTINBOUNDTABLE_SET(1,inIdx,asic_naptr);
		if(retval!=RT_ERR_OK) return RT_ERR_RG_NAPTR_SET_FAIL;
		//NAPT
		retval = RTK_L34_NAPTOUTBOUNDTABLE_SET(1,outIdx,asic_napt);
		if(retval!=RT_ERR_OK) return RT_ERR_RG_NAPT_SET_FAIL;
	}

	rg_db.naptIn[inIdx].idleSecs=0;
	rg_db.naptOut[outIdx].idleSecs=0;
	if(rg_db.naptIn[inIdx].rtk_naptIn.isTcp)
		rg_db.naptOut[outIdx].state=TCP_CONNECTED;
	else
		rg_db.naptOut[outIdx].state=UDP_CONNECTED;
	rg_db.naptValidSet[outIdx>>5] |= (0x1<<(outIdx&31));

	if(rg_db.systemGlobal.initParam.naptAddByHwCallBack	!= NULL)
	{
		memset(&naptInfo,0,sizeof(naptInfo));
		naptInfo.naptTuples.is_tcp = rg_db.naptIn[inIdx].rtk_naptIn.isTcp;
		naptInfo.naptTuples.local_ip = rg_db.naptIn[inIdx].rtk_naptIn.intIp;
		naptInfo.naptTuples.local_port = rg_db.naptIn[inIdx].rtk_naptIn.intPort;
		naptInfo.naptTuples.remote_ip = rg_db.naptIn[inIdx].remoteIp;
		naptInfo.naptTuples.remote_port = rg_db.naptIn[inIdx].remotePort;
		naptInfo.naptTuples.wan_intf_idx = rg_db.naptIn[inIdx].rtk_naptIn.extIpIdx;
		naptInfo.naptTuples.external_port = rg_db.naptOut[outIdx].extPort;
		naptInfo.naptTuples.outbound_pri_valid = rg_db.naptOut[outIdx].rtk_naptOut.priValid;
		naptInfo.naptTuples.outbound_priority = rg_db.naptOut[outIdx].rtk_naptOut.priValue;
		naptInfo.idleSecs = rg_db.naptOut[outIdx].idleSecs;
		naptInfo.state = rg_db.naptOut[outIdx].state;
		rg_db.systemGlobal.initParam.naptAddByHwCallBack(&naptInfo);
	}
	return RT_ERR_RG_OK;
}


//NAPT Flow
int32 rtk_rg_naptConnection_add(rtk_rg_naptEntry_t *naptFlow, int *flow_idx)
{
	int i=0;
	int retval=0;
	int outIdx=0,inIdx=0;
	rtk_l34_ext_intip_entry_t eip;
	rtk_l34_naptInbound_entry_t asic_naptr;
	rtk_l34_naptOutbound_entry_t asic_napt;

	//Check input parameters
	if(rg_db.systemGlobal.vlanInit==0) return RT_ERR_RG_NOT_INIT;
	if((naptFlow->is_tcp!=0) && (naptFlow->is_tcp!=1)) return RT_ERR_RG_INVALID_PARAM;

	if(rg_db.extip[naptFlow->wan_intf_idx].rtk_extip.valid==0) return RT_ERR_RG_EXTIP_GET_FAIL;
	memcpy(&eip,&rg_db.extip[naptFlow->wan_intf_idx].rtk_extip,sizeof(rtk_l34_ext_intip_entry_t));

	outIdx = _rtk_rg_NAPTIndex_get(naptFlow->is_tcp,naptFlow->local_ip,naptFlow->local_port,naptFlow->remote_ip,naptFlow->remote_port);
		
	//Check duplication
	for(i=outIdx<<2;i<(outIdx<<2)+4;i++)
	{
		if(rg_db.naptOut[i].rtk_naptOut.valid)
		{
			inIdx=rg_db.naptOut[i].rtk_naptOut.hashIdx;
			if((rg_db.naptIn[inIdx].rtk_naptIn.isTcp == naptFlow->is_tcp) &&
				(rg_db.naptIn[inIdx].rtk_naptIn.intIp == naptFlow->local_ip) &&
				(rg_db.naptIn[inIdx].rtk_naptIn.intPort== naptFlow->local_port) &&
				(rg_db.naptIn[inIdx].remoteIp == naptFlow->remote_ip) &&
				(rg_db.naptIn[inIdx].remotePort == naptFlow->remote_port))
			{
				rg_db.naptIn[inIdx].rtk_naptIn.priId=naptFlow->inbound_priority;
				rg_db.naptIn[inIdx].rtk_naptIn.priValid=naptFlow->inbound_pri_valid;
				rg_db.naptOut[i].rtk_naptOut.priValue=naptFlow->outbound_priority;
				rg_db.naptOut[i].rtk_naptOut.priValid=naptFlow->outbound_pri_valid;
				retval = _rtk_rg_naptConnection_add(i,&rg_db.naptOut[i].rtk_naptOut,&rg_db.naptIn[inIdx].rtk_naptIn);
				return RT_ERR_RG_NAPT_FLOW_DUPLICATE;
			}
		}
	}

	//Get free Inbound entry
	inIdx = _rtk_rg_NAPTRIndex_get(naptFlow->is_tcp,eip.extIpAddr,naptFlow->external_port);	
	for(i=0;i<4;i++)
	{
		if(rg_db.naptIn[(inIdx<<2)+i].rtk_naptIn.valid==0)
		{
			rg_db.naptIn[(inIdx<<2)+i].remoteIp = naptFlow->remote_ip;
			rg_db.naptIn[(inIdx<<2)+i].remotePort = naptFlow->remote_port;
			memset(&asic_naptr,0,sizeof(rtk_l34_naptInbound_entry_t));
			asic_naptr.intIp = naptFlow->local_ip;
			asic_naptr.intPort = naptFlow->local_port;
			//asic_naptr.remHash = _rtk_rg_NAPTRemoteHash_get((naptFlow->valid==0x2)?naptFlow->remote_ip:0,(naptFlow->valid==0x1)?naptFlow->remote_port:0);
			asic_naptr.remHash = _rtk_rg_NAPTRemoteHash_get(naptFlow->remote_ip,naptFlow->remote_port);
			asic_naptr.extIpIdx = naptFlow->wan_intf_idx;
			asic_naptr.extPortLSB = naptFlow->external_port;
			asic_naptr.isTcp = naptFlow->is_tcp;
			asic_naptr.valid = NAPT_IN_TYPE_PORT_RESTRICTED_CONE;
			asic_naptr.priValid = naptFlow->inbound_pri_valid;
			asic_naptr.priId = naptFlow->inbound_priority;
			inIdx=(inIdx<<2)+i;
			break;
		}		
	}
	if(i==4) return RT_ERR_RG_NAPTR_OVERFLOW; //full

	
	//NAPT entry
	for(i=0;i<4;i++)
	{
		if(rg_db.naptOut[(outIdx<<2)+i].rtk_naptOut.valid==0)
		{
			rg_db.naptOut[(outIdx<<2)+i].extPort = naptFlow->external_port;
			memset(&asic_napt,0,sizeof(rtk_l34_naptOutbound_entry_t));
			asic_napt.hashIdx = inIdx;
			asic_napt.priValid = naptFlow->outbound_pri_valid;
			asic_napt.priValue = naptFlow->outbound_priority;
			asic_napt.valid = 1;
			outIdx=(outIdx<<2)+i;
			break;
		}
	}
	if(i==4) return RT_ERR_RG_NAPT_OVERFLOW;

	*flow_idx = outIdx;

	retval = _rtk_rg_naptConnection_add(outIdx,&asic_napt,&asic_naptr);

	if(rg_db.naptIn[inIdx].rtk_naptIn.isTcp) 
		rg_db.naptTcpExternPortUsedRefCount[rg_db.naptOut[outIdx].extPort]++;
	else 
		rg_db.naptUdpExternPortUsedRefCount[rg_db.naptOut[outIdx].extPort]++;

	//rtlglue_printf("Add NAPT: [isTCP:%d] [local %x:%x] [gateway %x:%x] [remote %x:%x]\n",naptFlow->is_tcp,naptFlow->local_ip,naptFlow->local_port,eip.extIpAddr,naptFlow->external_port,naptFlow->remote_ip,naptFlow->remote_port);

    return retval;
}

void _rtk_rg_naptExtPortFree(int isTcp,uint16 port)
{
	int idx;
	uint32 bitValue;
	int i;

	i=port;
	idx=i>>5; // =port/32
	bitValue=1<<(i&0x1f);
	if(isTcp)
	{
		if((rg_db.naptTcpExternPortUsed[idx]&bitValue)>0)
		{
			rg_db.naptTcpExternPortUsedRefCount[port]--;
			if(rg_db.naptTcpExternPortUsedRefCount[port] <= 0)
			rg_db.naptTcpExternPortUsed[idx]&=(~bitValue);
		}			
	}
	else
	{
		if((rg_db.naptUdpExternPortUsed[idx]&bitValue)>0)
		{
			rg_db.naptUdpExternPortUsedRefCount[port]--;
			if(rg_db.naptUdpExternPortUsedRefCount[port] <= 0)
			rg_db.naptUdpExternPortUsed[idx]&=(~bitValue);
		}
	}
}

int _rtk_rg_naptTcpUdpOutHashIndexLookup(int8 isTcp, ipaddr_t srcAddr, uint16 srcPort, ipaddr_t destAddr, uint16 destPort)
{
	int i;
	uint32 naptHashOutIdx;
	uint32 naptOutIdx;
	rtk_rg_table_naptOut_linkList_t *pNaptOutList;
	naptHashOutIdx=_rtk_rg_naptTcpUdpOutHashIndex(isTcp,srcAddr,srcPort,destAddr,destPort);

#if 1	
	naptOutIdx=naptHashOutIdx<<2;
	for(i=naptOutIdx;i<naptOutIdx+4;i++)
	{
		if(rg_db.naptOut[i].rtk_naptOut.valid!=0) 
		{
			int naptInIdx=rg_db.naptOut[i].rtk_naptOut.hashIdx;
			if((srcAddr==rg_db.naptIn[naptInIdx].rtk_naptIn.intIp)&&
				(srcPort==rg_db.naptIn[naptInIdx].rtk_naptIn.intPort)&&
				(isTcp==rg_db.naptIn[naptInIdx].rtk_naptIn.isTcp)&&
				(_rtk_rg_NAPTRemoteHash_get(destAddr,destPort)==rg_db.naptIn[naptInIdx].rtk_naptIn.remHash)) //for PORT_REST ONLY
			{
				naptOutIdx=i;	
				return naptOutIdx;
			}
		}
	}
	//FIXME("NAPT 4-ways not found!");
#endif	

	//Found NAPT out flow in software link list (idx >= 2048)
	pNaptOutList=rg_db.pNaptOutHashListHead[naptHashOutIdx];
	while(pNaptOutList!=NULL)
	{		
		if(rg_db.naptOut[pNaptOutList->idx].rtk_naptOut.valid!=0) 
		{
			int naptInIdx=rg_db.naptOut[pNaptOutList->idx].rtk_naptOut.hashIdx;
			if((srcAddr==rg_db.naptIn[naptInIdx].rtk_naptIn.intIp)&&
				(srcPort==rg_db.naptIn[naptInIdx].rtk_naptIn.intPort)&&
				(isTcp==rg_db.naptIn[naptInIdx].rtk_naptIn.isTcp)&&
				(_rtk_rg_NAPTRemoteHash_get(destAddr,destPort)==rg_db.naptIn[naptInIdx].rtk_naptIn.remHash)) //for PORT_REST ONLY
			{
				naptOutIdx=pNaptOutList->idx;
				rg_db.naptOut[naptOutIdx].idleSecs = 0;
				rg_db.naptIn[rg_db.naptOut[naptOutIdx].rtk_naptOut.hashIdx].idleSecs = 0;				
				//rtlglue_printf("naptOutIdx=%d\n",naptOutIdx);
				//DEBUG("naptOutIdx=%d\n",naptOutIdx);
				return naptOutIdx;
			}
		}
		pNaptOutList=pNaptOutList->pNext;
	}
	
	//FIXME("NAPT OUT flow not found in FwdEngine!%p",rg_db.pNaptOutFreeListHead);
	return FAIL;
}

#ifdef CONFIG_APOLLO_ROMEDRIVER
int _rtk_rg_naptTcpUdpOutHashIndexLookupByPktHdr(int8 isTcp, rtk_rg_pktHdr_t *pPktHdr)
{
	uint32 naptHashOutIdx;
	uint32 naptOutIdx;
	rtk_rg_table_naptOut_linkList_t *pNaptOutList;
	ipaddr_t srcAddr=pPktHdr->ipv4Sip;
	uint16 srcPort=pPktHdr->sport;
	ipaddr_t destAddr=pPktHdr->ipv4Dip;
	uint16 destPort=pPktHdr->dport;

	if(pPktHdr->algAction==RG_ALG_ACT_NORMAL)
	{
		return _rtk_rg_naptTcpUdpOutHashIndexLookup(isTcp,srcAddr,srcPort,destAddr,destPort);
	}
	else //RG_ALG_ACT_TO_FWDENGINE, only add to software link-list
	{
		naptHashOutIdx=_rtk_rg_naptTcpUdpOutHashIndex(isTcp,srcAddr,srcPort,destAddr,destPort);
		//Found NAPT out flow in software link list (idx >= 2048)
		pNaptOutList=rg_db.pNaptOutHashListHead[naptHashOutIdx];
		while(pNaptOutList!=NULL)
		{		
			if(rg_db.naptOut[pNaptOutList->idx].rtk_naptOut.valid!=0) 
			{
				int naptInIdx=rg_db.naptOut[pNaptOutList->idx].rtk_naptOut.hashIdx;
				if((srcAddr==rg_db.naptIn[naptInIdx].rtk_naptIn.intIp)&&
					(srcPort==rg_db.naptIn[naptInIdx].rtk_naptIn.intPort)&&
					(isTcp==rg_db.naptIn[naptInIdx].rtk_naptIn.isTcp)&&
					(_rtk_rg_NAPTRemoteHash_get(destAddr,destPort)==rg_db.naptIn[naptInIdx].rtk_naptIn.remHash)) //for PORT_REST ONLY
				{
					naptOutIdx=pNaptOutList->idx;
					rg_db.naptOut[naptOutIdx].idleSecs = 0;
					rg_db.naptIn[rg_db.naptOut[naptOutIdx].rtk_naptOut.hashIdx].idleSecs = 0;				
					//rtlglue_printf("naptOutIdx=%d\n",naptOutIdx);
					//DEBUG("naptOutIdx=%d\n",naptOutIdx);
					return naptOutIdx;
				}
			}
			pNaptOutList=pNaptOutList->pNext;
		}
		
		//FIXME("NAPT OUT flow not found in FwdEngine!%p",rg_db.pNaptOutFreeListHead);
		return FAIL;
	}
}
#endif

int _rtk_rg_naptTcpUdpOutIdxLookupByInIdx(int8 isTcp, ipaddr_t remoteAddr, uint16 remotePort, ipaddr_t extAddr, uint16 extPort,int inIdx)
{
	int naptInIdx;
	int naptOutIdx,naptHashOutIdx;
	int j;
	
	if(rg_db.naptIn[inIdx].rtk_naptIn.valid==NAPT_IN_TYPE_PORT_RESTRICTED_CONE)
	{
		//DEBUG("LSB=%x remotePort=%x remHash=%x testHash=%x\n",rg_db.naptIn[i].rtk_naptIn.extPortLSB,extPort,rg_db.naptIn[i].rtk_naptIn.remHash,_rtk_rg_NAPTRemoteHash_get(remoteAddr,remotePort));			
		if((rg_db.naptIn[inIdx].rtk_naptIn.extPortLSB==(extPort&0xff))&&
			(rg_db.naptIn[inIdx].rtk_naptIn.isTcp==isTcp)&&
			(rg_db.naptIn[inIdx].rtk_naptIn.remHash==_rtk_rg_NAPTRemoteHash_get(remoteAddr,remotePort)))
		{							
			naptInIdx=inIdx;			
			naptHashOutIdx=_rtk_rg_naptTcpUdpOutHashIndex(isTcp,rg_db.naptIn[inIdx].rtk_naptIn.intIp,rg_db.naptIn[inIdx].rtk_naptIn.intPort,remoteAddr,remotePort);
			naptOutIdx=naptHashOutIdx<<2;
			for(j=naptOutIdx;j<naptOutIdx+4;j++)
			{
				if(rg_db.naptOut[j].rtk_naptOut.valid!=0) 
				{
					if(rg_db.naptOut[j].rtk_naptOut.hashIdx==naptInIdx)
					{
						return j; //naptOutIdx
					}
				}
			}

			//not found in 4way, found from link list.
			{
				rtk_rg_table_naptOut_linkList_t *pNaptOutList;

				pNaptOutList=rg_db.pNaptOutHashListHead[naptHashOutIdx];
				while(pNaptOutList!=NULL)
				{
					if(rg_db.naptOut[pNaptOutList->idx].rtk_naptOut.hashIdx==naptInIdx)
					{
						return pNaptOutList->idx; //naptOutIdx
					}
					pNaptOutList=pNaptOutList->pNext;
				}
			}
			
			FIXME("NAPTR found[%d], but NAPT 4-ways[%d~%d] and link list not found! rip=0x%x rport=%d eip=0x%x eport=%d",naptInIdx,naptOutIdx,naptOutIdx+3,remoteAddr,remotePort,extAddr,extPort);
			return FAIL;
		
		}
	}
	return FAIL;
}


int _rtk_rg_naptTcpUdpInHashIndexLookup(int8 isTcp, ipaddr_t remoteAddr, uint16 remotePort, ipaddr_t extAddr, uint16 extPort)
{
	//return outIdx
	int naptHashInIdx;
	int naptInIdx,naptOutIdx;
	int i;
	rtk_rg_table_naptIn_linkList_t	*pNaptInLinkList;
	
	naptHashInIdx=_rtk_rg_naptTcpUdpInHashIndex(isTcp,extAddr,extPort);
	naptInIdx=naptHashInIdx<<2;
	for(i=naptInIdx;i<naptInIdx+4;i++)
	{
		naptOutIdx=_rtk_rg_naptTcpUdpOutIdxLookupByInIdx(isTcp,remoteAddr,remotePort,extAddr,extPort,i);
		if(naptOutIdx!=FAIL)
			return naptOutIdx;
	}

	//DEBUG("NAPTR 4-ways[%d~%d] not found! rip=0x%x rport=%d eip=0x%x eport=%d",naptInIdx,naptInIdx+3,remoteAddr,remotePort,extAddr,extPort);
	//found in link list
	pNaptInLinkList=rg_db.pNaptInHashListHead[naptHashInIdx];
	while(pNaptInLinkList!=NULL)
	{
		naptOutIdx=_rtk_rg_naptTcpUdpOutIdxLookupByInIdx(isTcp,remoteAddr,remotePort,extAddr,extPort,pNaptInLinkList->idx);
		if(naptOutIdx!=FAIL)
			return naptOutIdx;
		pNaptInLinkList=pNaptInLinkList->pNext;
	}	

	DEBUG("NAPTR 4-ways[%d~%d] and link list not found! rip=0x%x rport=%d eip=0x%x eport=%d",naptInIdx,naptInIdx+3,remoteAddr,remotePort,extAddr,extPort);
	return FAIL;

}


int32 rtk_rg_naptConnection_del(int flow_idx)
{

	int retval=0;
	rtk_l34_naptInbound_entry_t asic_naptr;
	rtk_l34_naptOutbound_entry_t asic_napt;
	int inIdx;
	rtk_rg_naptInfo_t naptInfo;
	int i;

	//Check input parameters
	if(rg_db.systemGlobal.vlanInit==0) return RT_ERR_RG_NOT_INIT;
	if( (flow_idx<0) || (flow_idx>=MAX_NAPT_OUT_SW_TABLE_SIZE)) return RT_ERR_RG_INVALID_PARAM;

	//Find ASIC NAPT entry
	//Check whether entry is inused
	if(rg_db.naptOut[flow_idx].state==0)
		return RT_ERR_RG_NAPT_SW_ENTRY_NOT_FOUND;
	inIdx = rg_db.naptOut[flow_idx].rtk_naptOut.hashIdx;

	// Free ext port
	_rtk_rg_naptExtPortFree(rg_db.naptIn[inIdx].rtk_naptIn.isTcp,rg_db.naptOut[flow_idx].extPort);

	//Fill NAPT info
	if(rg_db.systemGlobal.initParam.naptDelByHwCallBack	!= NULL)
	{
		memset(&naptInfo,0,sizeof(rtk_rg_naptInfo_t));	
		naptInfo.naptTuples.is_tcp = rg_db.naptIn[inIdx].rtk_naptIn.isTcp;
		naptInfo.naptTuples.local_ip = rg_db.naptIn[inIdx].rtk_naptIn.intIp;
		naptInfo.naptTuples.local_port = rg_db.naptIn[inIdx].rtk_naptIn.intPort;
		naptInfo.naptTuples.remote_ip = rg_db.naptIn[inIdx].remoteIp;
		naptInfo.naptTuples.remote_port = rg_db.naptIn[inIdx].remotePort;
		naptInfo.naptTuples.wan_intf_idx = rg_db.naptIn[inIdx].rtk_naptIn.extIpIdx;
		naptInfo.naptTuples.inbound_pri_valid = rg_db.naptIn[inIdx].rtk_naptIn.priValid;
		naptInfo.naptTuples.inbound_priority = rg_db.naptIn[inIdx].rtk_naptIn.priId;		
		naptInfo.naptTuples.external_port = rg_db.naptOut[flow_idx].extPort;
		naptInfo.naptTuples.outbound_pri_valid = rg_db.naptOut[flow_idx].rtk_naptOut.priValid;
		naptInfo.naptTuples.outbound_priority = rg_db.naptOut[flow_idx].rtk_naptOut.priValue;
		naptInfo.idleSecs = rg_db.naptOut[flow_idx].idleSecs;
		naptInfo.state = rg_db.naptOut[flow_idx].state;
	}

	//Delete shortcut entry
	for(i=0;i<MAX_NAPT_SHORTCUT_SIZE;i++)
	{
		rtk_rg_napt_shortcut_t *pNaptSc;
		pNaptSc=&rg_db.naptShortCut[i];

		if(pNaptSc->isNapt==0) continue;
		if((rg_db.naptIn[inIdx].rtk_naptIn.isTcp)^(pNaptSc->isTcp)) continue;

		//Out
		if((pNaptSc->direction==NAPT_DIRECTION_OUTBOUND) && 
			(rg_db.naptIn[inIdx].rtk_naptIn.intIp==pNaptSc->sip) && (rg_db.naptIn[inIdx].rtk_naptIn.intPort==pNaptSc->sport) &&
			(rg_db.naptIn[inIdx].remoteIp==pNaptSc->dip) && (rg_db.naptIn[inIdx].remotePort==pNaptSc->dport))
		{
			memset(pNaptSc,0,sizeof(rtk_rg_napt_shortcut_t));
			DEBUG("Clear NAPT shortcut[%d].\n",i);
		}

		//IN
		if((pNaptSc->direction==NAPT_DIRECTION_INBOUND) && 
			(rg_db.naptIn[inIdx].remoteIp==pNaptSc->sip) && (rg_db.naptIn[inIdx].remotePort==pNaptSc->sport) &&
			(rg_db.extip[rg_db.naptIn[inIdx].rtk_naptIn.extIpIdx].rtk_extip.extIpAddr==pNaptSc->dip) && (rg_db.naptOut[flow_idx].extPort==pNaptSc->dport))
		{
			memset(pNaptSc,0,sizeof(rtk_rg_napt_shortcut_t));
			DEBUG("Clear NAPT shortcut[%d].\n",i);
		}
	}

	if(flow_idx>=MAX_NAPT_OUT_HW_TABLE_SIZE)
	{
		int hashIdx=rg_db.naptOut[flow_idx].hashIdx;
		//DEBUG("del out flow_idx=%d",flow_idx);
		
		//remove from link list
		rtk_rg_table_naptOut_linkList_t *pNaptOutList,*pPreNaptOutList;
		pPreNaptOutList=rg_db.pNaptOutHashListHead[hashIdx];
		pNaptOutList=pPreNaptOutList;
		while(pNaptOutList!=NULL)
		{
			if(pNaptOutList->idx==flow_idx)
			{
				//remove from hashIdx list
				if(pPreNaptOutList==pNaptOutList)
					rg_db.pNaptOutHashListHead[hashIdx]=pNaptOutList->pNext;
				else
					pPreNaptOutList->pNext=pNaptOutList->pNext;
				
				//add to free list
				pNaptOutList->pNext=rg_db.pNaptOutFreeListHead;
				rg_db.pNaptOutFreeListHead=pNaptOutList;				
				break;
			}
			pPreNaptOutList=pNaptOutList;
			pNaptOutList=pNaptOutList->pNext;
		}		
	}

	//Sync to ASIC NAPT table
	memset(&asic_napt,0,sizeof(rtk_l34_naptOutbound_entry_t));
	retval = RTK_L34_NAPTOUTBOUNDTABLE_SET(1,flow_idx,&asic_napt);
	if(retval!=RT_ERR_OK) return RT_ERR_RG_NAPT_SET_FAIL;

	//Sync to SW NAPT table
	rg_db.naptOut[flow_idx].state=0;
	rg_db.naptOut[flow_idx].extPort=0;
	rg_db.naptOut[flow_idx].hashIdx=0;	
	rg_db.naptOut[flow_idx].idleSecs=0;
	rg_db.naptValidSet[flow_idx>>5] &= ~(0x1<<(flow_idx&31));

	if(inIdx>=MAX_NAPT_IN_HW_TABLE_SIZE)
	{
		int hashIdx=rg_db.naptIn[inIdx].hashIdx;
		
		//DEBUG("del in flow_idx=%d",inIdx);
		//remove from link list
		rtk_rg_table_naptIn_linkList_t *pNaptInList,*pPreNaptInList;
		pPreNaptInList=rg_db.pNaptInHashListHead[hashIdx];
		pNaptInList=pPreNaptInList;
		while(pNaptInList!=NULL)
		{
			if(pNaptInList->idx==inIdx)
			{
				//remove from hashIdx list
				if(pPreNaptInList==pNaptInList)
					rg_db.pNaptInHashListHead[hashIdx]=pNaptInList->pNext;
				else
					pPreNaptInList->pNext=pNaptInList->pNext;

				
				//add to free list
				pNaptInList->pNext=rg_db.pNaptInFreeListHead;
				rg_db.pNaptInFreeListHead=pNaptInList;				
				break;
			}
			pPreNaptInList=pNaptInList;
			pNaptInList=pNaptInList->pNext;
		}		

	}

	//Sync. to ASIC NAPT-R table
	memset(&asic_naptr,0,sizeof(rtk_l34_naptInbound_entry_t));
	retval = RTK_L34_NAPTINBOUNDTABLE_SET(1,inIdx,&asic_naptr);
	if(retval!=RT_ERR_OK) return RT_ERR_RG_NAPTR_SET_FAIL;


	//Sync. to SW NAPT-R table	
	rg_db.naptIn[inIdx].remoteIp=0;
	rg_db.naptIn[inIdx].remotePort=0;
	rg_db.naptIn[inIdx].hashIdx=0;
	rg_db.naptIn[inIdx].idleSecs=0;
	
	//call callback function 
	if(rg_db.systemGlobal.initParam.naptDelByHwCallBack	!= NULL)
	{
		rg_db.systemGlobal.initParam.naptDelByHwCallBack(&naptInfo);
	}

    return RT_ERR_RG_OK;
}

int32 rtk_rg_naptConnection_find(rtk_rg_naptInfo_t *naptEntry,int *valid_idx)
{
	int i;
	int inIdx=0;
	rtk_rg_naptEntry_t *naptFlow;
	naptFlow=&naptEntry->naptTuples;	
	
	//Check input parameters
	if(rg_db.systemGlobal.vlanInit==0) return RT_ERR_RG_NOT_INIT;

	if(*valid_idx==-1)
	{
		int outIdx = _rtk_rg_naptTcpUdpOutHashIndexLookup(naptEntry->naptTuples.is_tcp,naptEntry->naptTuples.local_ip,naptEntry->naptTuples.local_port,naptEntry->naptTuples.remote_ip,naptEntry->naptTuples.remote_port);
		if(outIdx!=FAIL)
		{
			inIdx=rg_db.naptOut[outIdx].rtk_naptOut.hashIdx;
			naptEntry->state = rg_db.naptOut[outIdx].state;
			naptEntry->idleSecs = rg_db.naptOut[outIdx].idleSecs;
			naptEntry->naptTuples.wan_intf_idx = rg_db.naptIn[inIdx].rtk_naptIn.extIpIdx;
			naptEntry->naptTuples.inbound_pri_valid = rg_db.naptIn[inIdx].rtk_naptIn.priValid;
			naptEntry->naptTuples.inbound_priority = rg_db.naptIn[inIdx].rtk_naptIn.priId;			
			naptEntry->naptTuples.external_port = rg_db.naptOut[outIdx].extPort;
			naptEntry->naptTuples.outbound_pri_valid = rg_db.naptOut[outIdx].rtk_naptOut.priValid;
			naptEntry->naptTuples.outbound_priority = rg_db.naptOut[outIdx].rtk_naptOut.priValue;
			
			*valid_idx = outIdx;
			return RT_ERR_RG_OK;
		}

		return RT_ERR_RG_NAPT_SW_ENTRY_NOT_FOUND;
	}

	for(i=*valid_idx;i<MAX_NAPT_OUT_HW_TABLE_SIZE;i++)
	{
		if(rg_db.naptOut[i].rtk_naptOut.valid==0) continue;		
		inIdx=rg_db.naptOut[i].rtk_naptOut.hashIdx;

		naptFlow->external_port=rg_db.naptOut[i].extPort;
		naptFlow->is_tcp=rg_db.naptIn[inIdx].rtk_naptIn.isTcp;
		naptFlow->local_ip=rg_db.naptIn[inIdx].rtk_naptIn.intIp;
		naptFlow->remote_ip=rg_db.naptIn[inIdx].remoteIp;
		naptFlow->wan_intf_idx=rg_db.naptIn[inIdx].rtk_naptIn.extIpIdx;
		naptFlow->local_port=rg_db.naptIn[inIdx].rtk_naptIn.intPort;
		naptFlow->remote_port=rg_db.naptIn[inIdx].remotePort;
		naptFlow->inbound_pri_valid=rg_db.naptIn[inIdx].rtk_naptIn.priValid;
		naptFlow->inbound_priority=rg_db.naptIn[inIdx].rtk_naptIn.priId;
		naptFlow->outbound_pri_valid=rg_db.naptOut[i].rtk_naptOut.priValid;
		naptFlow->outbound_priority=rg_db.naptOut[i].rtk_naptOut.priValue;
		naptEntry->idleSecs=rg_db.naptOut[i].idleSecs;
		naptEntry->state=rg_db.naptOut[i].state;
		*valid_idx=i;
		return RT_ERR_RG_OK;		
	}		
    return RT_ERR_RG_NAPT_SW_ENTRY_NOT_FOUND;
}


int32 _rtk_rg_findAndReclamL2mcEntry(rtk_mac_t *mac,int fid)
{
	int l2Idx,search_index,count=0;
	l2Idx=_rtk_rg_hash_mac_fid_efid(mac->octet,fid,0);			//FIXME:EFID is 0 now

	l2Idx<<=2;
	do
	{
		search_index = l2Idx+count;
		//DEBUG("search_idx is %d\n",search_index);
		if(rg_db.lut[search_index].valid==0)
			break;	//empty, just add
						
		if(rg_db.lut[search_index].rtk_lut.entryType==RTK_LUT_L2MC &&
			(memcmp(&rg_db.lut[search_index].rtk_lut.entry.l2McEntry.mac.octet,mac->octet,ETHER_ADDR_LEN)==0) &&
			rg_db.lut[search_index].rtk_lut.entry.l2McEntry.fid==fid)
		{
//			DEBUG("match!! froced replace LUT entry[%d] for MC entry!!",search_index);
			break;				
		}

		count++; //search from next entry
	}while(count < 4);

	if(count==4)
		count=_rtk_rg_layer2GarbageCollection(l2Idx);		//check if there is asynchronus between software and hardware table
		
	if(count==4)
	{			
		search_index=_rtk_rg_layer2LeastRecentlyUsedReplace(l2Idx); 	//replace the least recently used entry for new entry			
		if(search_index==-1) 
		{
			FIXME("must add software LUT entry for LUT entry full.");
			return FAIL;
		}
	}
	else
		search_index=l2Idx+count;

	return search_index;

}


int32 rtk_rg_l2MultiCastFlow_add(rtk_rg_l2MulticastFlow_t *l2McFlow,int *flow_idx)
{
	int search_index;
	rtk_l2_mcastAddr_t lut;
	memset(&lut,0,sizeof(rtk_l2_mcastAddr_t));

	lut.fid = LAN_FID;
	_rtk_rg_portmask_translator(l2McFlow->port_mask,&lut.portmask,&lut.ext_portmask);
	memcpy(lut.mac.octet,l2McFlow->mac.octet,6);

#ifdef CONFIG_DUALBAND_CONCURRENT
	if((lut.portmask.bits[0]==(1<<RTK_RG_MAC_PORT_CPU))&&(lut.ext_portmask.bits[0]==4)) //Only to EXT1(slave wifi), modify priority.
	{
		lut.flags|=RTK_L2_MCAST_FLAG_FWD_PRI;
		lut.priority=CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI;
	}
#endif

	search_index=_rtk_rg_findAndReclamL2mcEntry(&lut.mac,LAN_FID);

	DEBUG("### L2 MULTICAST add at %d: %02x:%02x:%02x:%02x:%02x:%02x PortMask:0x%x ExtPortMask:0x%x ###\n",search_index,
	lut.mac.octet[0],lut.mac.octet[1],lut.mac.octet[2],lut.mac.octet[3],lut.mac.octet[4],lut.mac.octet[5],
	lut.portmask.bits[0],lut.ext_portmask.bits[0]);

	ASSERT_EQ(RTK_L2_MCASTADDR_ADD(&lut),RT_ERR_OK);		
	ASSERT_EQ(search_index,lut.index);
	*flow_idx=lut.index;

	return RT_ERR_RG_OK;
}

int32 _rtk_rg_findAndReclamIpmcEntry(ipaddr_t sip,ipaddr_t dip)
{
	int l2Idx,search_index,count=0;
	l2Idx=_rtk_rg_hash_sip_gip(sip,dip);			//FIXME:EFID is 0 now		
	l2Idx<<=2;
	do
	{
		search_index = l2Idx+count;
		//DEBUG("search_idx is %d\n",search_index);
		if(rg_db.lut[search_index].valid==0)
			break;	//empty, just add
						
		if((rg_db.lut[search_index].rtk_lut.entryType==RTK_LUT_L3MC) &&
			(rg_db.lut[search_index].rtk_lut.entry.ipmcEntry.dip==dip)&&
			(rg_db.lut[search_index].rtk_lut.entry.ipmcEntry.sip==sip))
		{
//			DEBUG("match!! froced replace LUT entry[%d] for MC entry!!",search_index);
			break;				
		}
	
		count++; //search from next entry
	}while(count < 4);
	
	if(count==4)
		count=_rtk_rg_layer2GarbageCollection(l2Idx);		//check if there is asynchronus between software and hardware table
		
	if(count==4)
	{			
		search_index=_rtk_rg_layer2LeastRecentlyUsedReplace(l2Idx); 	//replace the least recently used entry for new entry			
		if(search_index==-1) 
		{
			FIXME("must add software LUT entry for LUT entry full.");
			return FAIL;
		}
	}
	else
		search_index=l2Idx+count;

	return search_index;

}

int32 rtk_rg_ipv4MultiCastFlow_add(rtk_rg_ipv4MulticastFlow_t *ipv4McFlow,int *flow_idx)
{
	int search_index;
	rtk_l2_ipMcastAddr_t lut;
	memset(&lut,0,sizeof(rtk_l2_ipMcastAddr_t));
	
	
	if((ipv4McFlow->srcFilterMode==RTK_RG_IPV4MC_INCLUDE)||(ipv4McFlow->srcFilterMode==RTK_RG_IPV4MC_EXCLUDE))
	{		
		rtk_portmask_t pmsk;
		int i;
		memset(&pmsk,0,sizeof(rtk_portmask_t));
		
		
		if((ipv4McFlow->maxIpListNums>MAX_IPMC_IP_LIST)||(ipv4McFlow->maxIpListNums<0))
			return RT_ERR_RG_IPMC_IP_LIST_OUT_OF_RANGE;

		lut.dip=ipv4McFlow->groupIp;
		lut.flags=RTK_L2_IPMCAST_FLAG_STATIC;

		if(ipv4McFlow->srcFilterMode==RTK_RG_IPV4MC_EXCLUDE)
		{

			if((ipv4McFlow->maxIpListNums>MAX_IPMC_IP_LIST)||(ipv4McFlow->maxIpListNums<0))
				return RT_ERR_RG_IPMC_IP_LIST_OUT_OF_RANGE;		
			_rtk_rg_portmask_translator(ipv4McFlow->excludeModePortmask,&lut.portmask,&lut.ext_portmask);			
			pmsk.bits[0]=lut.portmask.bits[0];
			lut.portmask.bits[0]=0;
			lut.ext_portmask.bits[0]=0;
		}

		for(i=0;i<ipv4McFlow->maxIpListNums;i++)
		{
			lut.sip=ipv4McFlow->includeOrExcludeIpList[i];
			search_index=_rtk_rg_findAndReclamIpmcEntry(lut.sip,lut.dip);
			if(ipv4McFlow->srcFilterMode==RTK_RG_IPV4MC_INCLUDE)
			{
				_rtk_rg_portmask_translator(ipv4McFlow->includeModePortmaskList[i],&lut.portmask,&lut.ext_portmask);
#ifdef CONFIG_DUALBAND_CONCURRENT
				if((lut.portmask.bits[0]==(1<<RTK_RG_MAC_PORT_CPU))&&(lut.ext_portmask.bits[0]==4)) //Only to EXT1(slave wifi), modify priority.
				{
					lut.flags|=RTK_L2_MCAST_FLAG_FWD_PRI;
					lut.priority=CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI;
				}
#endif
			}
			ASSERT_EQ(RTK_L2_IPMCASTADDR_ADD(&lut),RT_ERR_OK);
			ASSERT_EQ(search_index,lut.index);
		}
		
		ASSERT_EQ(RTK_L2_IPMCGROUP_ADD(ipv4McFlow->groupIp,&pmsk),RT_ERR_OK);
		
	}	
	else if(ipv4McFlow->srcFilterMode==RTK_RG_IPV4MC_DONT_CARE_SRC)
	{
		lut.dip=ipv4McFlow->groupIp;
		lut.sip=0;
		lut.flags=RTK_L2_IPMCAST_FLAG_STATIC|RTK_L2_IPMCAST_FLAG_DIP_ONLY;
		_rtk_rg_portmask_translator(ipv4McFlow->dontCareSipModePortmask,&lut.portmask,&lut.ext_portmask);
		
#ifdef CONFIG_DUALBAND_CONCURRENT
		if((lut.portmask.bits[0]==(1<<RTK_RG_MAC_PORT_CPU))&&(lut.ext_portmask.bits[0]==4)) //Only to EXT1(slave wifi), modify priority.
		{
			lut.flags|=RTK_L2_MCAST_FLAG_FWD_PRI;
			lut.priority=CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI;
		}
#endif
	
		search_index=_rtk_rg_findAndReclamIpmcEntry(lut.sip,lut.dip);
		ASSERT_EQ(RTK_L2_IPMCASTADDR_ADD(&lut),RT_ERR_OK);
		ASSERT_EQ(search_index,lut.index);
	}

	*flow_idx=lut.index;

	return RT_ERR_RG_OK;
}

int32 rtk_rg_multicastFlow_add(rtk_rg_multicastFlow_t *mcFlow, int *flow_idx)
{

	if(mcFlow->isIPv6)
	{
		rtk_rg_l2MulticastFlow_t l2Mc;
		int idx;
		memset(&l2Mc,0,sizeof(rtk_rg_l2MulticastFlow_t));		
		l2Mc.mac.octet[0] = 0x33;
		l2Mc.mac.octet[1] = 0x33;
		l2Mc.mac.octet[2] = (mcFlow->multicast_ipv6_addr[3]&0xFF000000)>>24;
		l2Mc.mac.octet[3] = (mcFlow->multicast_ipv6_addr[3]&0x00FF0000)>>16;
		l2Mc.mac.octet[4] = (mcFlow->multicast_ipv6_addr[3]&0x0000FF00)>>8;
		l2Mc.mac.octet[5] = (mcFlow->multicast_ipv6_addr[3]&0x000000FF);
		l2Mc.port_mask=mcFlow->port_mask;
		ASSERT_EQ(rtk_rg_l2MultiCastFlow_add(&l2Mc,&idx),RT_ERR_RG_OK);
		*flow_idx=idx;
		return RT_ERR_RG_OK;
	}
	else
	{
		rtk_rg_ipv4MulticastFlow_t ipv4Mc;
		int idx;
		memset(&ipv4Mc,0,sizeof(rtk_rg_ipv4MulticastFlow_t));
		ipv4Mc.srcFilterMode=RTK_RG_IPV4MC_DONT_CARE_SRC;
//		ipv4Mc.maxIpListNums=0;
//		ipv4Mc.IncludeOrExcludeList[0]=0xc0a8966f;		
//		ipv4Mc.IncludeOrExcludeList[1]=0xc0a8016f;
		ipv4Mc.groupIp=mcFlow->multicast_ipv4_addr;
		ipv4Mc.dontCareSipModePortmask=mcFlow->port_mask;			
		ASSERT_EQ(rtk_rg_ipv4MultiCastFlow_add(&ipv4Mc,&idx),RT_ERR_RG_OK);
		*flow_idx=idx;	
		return RT_ERR_RG_OK;
	}	

}


int32 rtk_rg_multicastFlow_del(int flow_idx)
{
		int ret;
		rtk_l2_addr_table_t lut;
		int found=flow_idx;	
		if(flow_idx>=2048) return RT_ERR_RG_INDEX_OUT_OF_RANGE;
	
		ret=rtk_l2_nextValidEntry_get(&found,&lut);
		if(ret) return ret;
	
		if(found==flow_idx)
		{
		//rtlglue_printf("flow====%d dip=%x\n",flow_idx,lut.entry.ipmcEntry.dip);




			if(lut.entryType==RTK_LUT_L2MC)
			{
				ret=RTK_L2_MCASTADDR_DEL(&lut.entry.l2McEntry);
				DEBUG("### L2 MULTICAST del: %02x:%02x:%02x:%02x:%02x:%02x PortMask:0x%x ExtPortMask:0x%x ###\n",			
				lut.entry.l2McEntry.mac.octet[0],lut.entry.l2McEntry.mac.octet[1],lut.entry.l2McEntry.mac.octet[2],lut.entry.l2McEntry.mac.octet[3],lut.entry.l2McEntry.mac.octet[4],lut.entry.l2McEntry.mac.octet[5],
				lut.entry.l2McEntry.portmask.bits[0],lut.entry.l2McEntry.ext_portmask.bits[0]);				
			}
			else if(lut.entryType==RTK_LUT_L3MC)
			{
				ret=RTK_L2_IPMCASTADDR_DEL(&lut.entry.ipmcEntry);
				DEBUG("### IP MULTICAST del: DIP:%d.%d.%d.%d SIP:%d.%d.%d.%d PortMask:0x%x ExtPortMask:0x%x ###\n",			
				(lut.entry.ipmcEntry.dip>>24)&0xff,(lut.entry.ipmcEntry.dip>>16)&0xff,(lut.entry.ipmcEntry.dip>>8)&0xff,(lut.entry.ipmcEntry.dip)&0xff,
				(lut.entry.ipmcEntry.sip>>24)&0xff,(lut.entry.ipmcEntry.sip>>16)&0xff,(lut.entry.ipmcEntry.sip>>8)&0xff,(lut.entry.ipmcEntry.sip)&0xff,
				lut.entry.ipmcEntry.portmask.bits[0],lut.entry.ipmcEntry.ext_portmask.bits[0]);				
			}

			if(ret==RT_ERR_OK)
				return RT_ERR_RG_OK;
			else
				return ret;
		}
		else
			return RT_ERR_RG_ENTRY_NOT_EXIST;

}

int32 rtk_rg_multicastFlow_find(rtk_rg_multicastFlow_t *mcFlow, int *valid_idx)
{
	int ret=0;
	rtk_l2_addr_table_t	data;
	int input=*valid_idx;
	int output=input;
	int i=0;
	//rtlglue_printf("valid_idx=%d input=%d output=%d\n",*valid_idx,input,output);
	if(*valid_idx>=2048) return RT_ERR_RG_INDEX_OUT_OF_RANGE;
	while(1)
	{
		ret=rtk_l2_nextValidEntry_get(&output,&data);

		if(ret!=RT_ERR_OK) return RT_ERR_RG_NO_MORE_ENTRY_FOUND;

		
		if(mcFlow==NULL) return RT_ERR_RG_NULL_POINTER;
		
		memset(mcFlow,0,sizeof(rtk_rg_multicastFlow_t));

		if(data.entryType==RTK_LUT_L3MC)
		{
			mcFlow->multicast_ipv4_addr=data.entry.ipmcEntry.dip;
			mcFlow->port_mask.portmask=(data.entry.ipmcEntry.portmask.bits[0]&0x3f)|(data.entry.ipmcEntry.ext_portmask.bits[0]<<6);
			//mcFlow->src_intf_idx;  //FIXME			
			*valid_idx=data.entry.ipmcEntry.index;	
			//rtlglue_printf("index=%d \n",data.entry.l2UcEntry.index);
			
			return RT_ERR_RG_OK;
		}else if(data.entryType==RTK_LUT_L2MC){
			*valid_idx=output;
			mcFlow->multicast_ipv4_addr=data.entry.ipmcEntry.dip;
			mcFlow->port_mask.portmask=(data.entry.ipmcEntry.portmask.bits[0]&0x3f)|(data.entry.ipmcEntry.ext_portmask.bits[0]<<6);
			//rtlglue_printf("Get entry:multicast_ipv4_addr=0x%x portmask=0x%x *valid_idx=%d entryType=%d\n",mcFlow->multicast_ipv4_addr,mcFlow->port_mask.portmask,*valid_idx,data.entryType);
			//FIXME
			//Need isIPv6, src_intf_idx
			mcFlow->multicast_ipv4_addr = (0xe0<<24) | 
			                              (data.entry.l2McEntry.mac.octet[3]<<16)|
			                              (data.entry.l2McEntry.mac.octet[4]<<8)|
			                              (data.entry.l2McEntry.mac.octet[5]);		
			return RT_ERR_RG_OK;
		}
		output++;
		i++;
		if(i>2048){return RT_ERR_RG_NO_MORE_ENTRY_FOUND;/*find at most 2048 times, avoid infinite loop*/}
		
	}	

}


//MAC
int32 rtk_rg_macEntry_add(rtk_rg_macEntry_t *macEntry, int *entry_idx)
{
	rtk_l2_ucastAddr_t lut;
	memset(&lut,0,sizeof(rtk_l2_ucastAddr_t));
	memcpy(lut.mac.octet,macEntry->mac.octet,6);
	lut.fid=macEntry->fid;
	lut.vid=macEntry->vlan_id;
	lut.age=7;

	//Check input parameters
	if(rg_db.systemGlobal.vlanInit==0) return RT_ERR_RG_NOT_INIT;
	
	if(macEntry->port_idx>=RTK_RG_PORT_CPU)
	{
		lut.port=RTK_RG_PORT_CPU;
		lut.ext_port=macEntry->port_idx-RTK_RG_PORT_CPU;
#ifdef CONFIG_DUALBAND_CONCURRENT
#if 1 	//20130722: if the packet is from ext0, this packet will hit ACL rule.(modify dpmask to 8)
		// the GMAC hw will reference CPU_RRING_ROUTING.		
		if(macEntry->port_idx==RTK_RG_EXT_PORT1)
		{
			//lut.flags|=(RTK_L2_UCAST_FLAG_FWD_PRI|RTK_L2_UCAST_FLAG_STATIC); // must set static becasue unknow DA can't forward to CPU2(trap to CPU1 again)
			lut.flags|=(RTK_L2_UCAST_FLAG_FWD_PRI); //2013071
			lut.priority=CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI;		
		}
#endif		
#endif		
	}
	else
	{
		lut.port=macEntry->port_idx;
	}

	if(macEntry->static_entry)
	{
		lut.flags|=RTK_L2_UCAST_FLAG_STATIC; 
	}
	if(macEntry->isIVL)
	{
		lut.flags|=RTK_L2_UCAST_FLAG_IVL; 
	}
	if(macEntry->arp_used)
	{
		lut.flags|=RTK_L2_UCAST_FLAG_ARP_USED; 
	}	

	ASSERT_EQ(RTK_L2_ADDR_ADD(&lut),RT_ERR_OK);
	//memcpy(rg_db.mac[lut.index].macAddr.octet,macEntry->mac.octet,6);

	*entry_idx=lut.index;

	return RT_ERR_RG_OK;
// *      RT_ERR_RG_ENTRY_FULL - the MAC entry is full.

	
}

int32 rtk_rg_macEntry_del(int entry_idx)
{
	rtk_l2_addr_table_t     l2Entry;
	int found=entry_idx;	

	//Check input parameters
	if(rg_db.systemGlobal.vlanInit==0) return RT_ERR_RG_NOT_INIT;
	if(entry_idx>=2048) return RT_ERR_RG_INDEX_OUT_OF_RANGE;

	ASSERT_EQ(rtk_l2_nextValidEntry_get(&found,&l2Entry),RT_ERR_OK);

	if(found==entry_idx)
	{
#if 0	
		rtlglue_printf("found at %d, mac=%02x-%02x-%02x-%02x-%02x-%02x ivl=%d\n",entry_idx,l2Entry.entry.l2UcEntry.mac.octet[0]
			,l2Entry.entry.l2UcEntry.mac.octet[1]
			,l2Entry.entry.l2UcEntry.mac.octet[2]
			,l2Entry.entry.l2UcEntry.mac.octet[3]
			,l2Entry.entry.l2UcEntry.mac.octet[4]
			,l2Entry.entry.l2UcEntry.mac.octet[5]
			,l2Entry.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_IVL			
		);
#endif	
		ASSERT_EQ(RTK_L2_ADDR_DEL(&l2Entry.entry.l2UcEntry),RT_ERR_OK);
		//memset(rg_db.mac[entry_idx].macAddr.octet,0,6);

		return RT_ERR_RG_OK;
	}
	else
		return RT_ERR_RG_ENTRY_NOT_EXIST;

//*	   RT_ERR_RG_NOT_INIT - system is not initiated. 	
}

int32 rtk_rg_macEntry_find(rtk_rg_macEntry_t *macEntry,int *valid_idx)
{
	int ret=0;
	rtk_l2_addr_table_t	data;
	int input=*valid_idx;
	int output=input;

	//Check input parameters
	if(rg_db.systemGlobal.vlanInit==0) return RT_ERR_RG_NOT_INIT;
	if((*valid_idx>=2048) || (*valid_idx<-1)) return RT_ERR_RG_INDEX_OUT_OF_RANGE;

	//Search by MAC
	if(*valid_idx==-1) output=input=0;

	while(1)
	{
		ret=rtk_l2_nextValidEntry_get(&output,&data);
	
		if(ret!=RT_ERR_OK) return RT_ERR_RG_NO_MORE_ENTRY_FOUND;

		if(macEntry==NULL) return RT_ERR_RG_NULL_POINTER;
		
		//Search by MAC
		if(*valid_idx==-1)
		{
			if(memcmp(&macEntry->mac.octet[0],data.entry.l2UcEntry.mac.octet,6)!=0)
			{
				output++;
				continue;
			}
		}
		
		memset(macEntry,0,sizeof(rtk_rg_macEntry_t));

		if(data.entryType==RTK_LUT_L2UC)
		{
			macEntry->fid=data.entry.l2UcEntry.fid;
			memcpy(macEntry->mac.octet,data.entry.l2UcEntry.mac.octet,6);
			if(data.entry.l2UcEntry.port==RTK_RG_PORT_CPU)
			{
				macEntry->port_idx=RTK_RG_PORT_CPU+data.entry.l2UcEntry.ext_port;
			}
			else
			{
				macEntry->port_idx=data.entry.l2UcEntry.port;
			}
			if(data.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_STATIC)	 macEntry->static_entry=1;
			macEntry->vlan_id=data.entry.l2UcEntry.vid;
			macEntry->isIVL = (data.entry.l2UcEntry.flags & RTK_L2_UCAST_FLAG_IVL)?1:0;
			macEntry->arp_used = (data.entry.l2UcEntry.flags & RTK_L2_UCAST_FLAG_ARP_USED)?1:0;
			*valid_idx=data.entry.l2UcEntry.index;
			//rtlglue_printf("index=%d \n",data.entry.l2UcEntry.index);
			return RT_ERR_RG_OK;
		}
		output++;		
	}	
	

// *      RT_ERR_RG_NOT_INIT - system is not initiated. 	

}

int _rtk_rg_portBindingLookup(int srcPort, int srcExtPort, int vid,rtk_rg_sipDipClassification_t sipDipClass)
{
	int i;
	//src port binding
	for(i=0;i<MAX_BIND_SW_TABLE_SIZE;i++)
	{
		if(rg_db.bind[i].valid==1)			
		{			
			int wanTypeIdx=rg_db.bind[i].rtk_bind.wanTypeIdx;
			int match=0;
			//DEBUG("srcPort is %d, bt[%d].portMask is %x, vid is %d, vidLan is %d",srcPort,i,rg_db.bind[i].rtk_bind.portMask,vid,rg_db.bind[i].rtk_bind.vidLan);
			if(srcPort==RTK_RG_PORT_CPU)
			{
				if(rg_db.bind[i].rtk_bind.extPortMask.bits[0]&(1<<srcExtPort)) 
				{
					if(rg_db.bind[i].rtk_bind.vidLan==vid) match=1;
				}
			}
			else			
			{
				if(rg_db.bind[i].rtk_bind.portMask.bits[0]&(1<<srcPort))
				{
					if(rg_db.bind[i].rtk_bind.vidLan==vid) match=1;
				}
			}

			if(match==1)
			{
				if(((sipDipClass==SIP_DIP_CLASS_NAPT) && (rg_db.wantype[wanTypeIdx].rtk_wantype.wanType==L34_WAN_TYPE_L34NAT_ROUTE))
					|| ((sipDipClass==SIP_DIP_CLASS_ROUTING) && (rg_db.wantype[wanTypeIdx].rtk_wantype.wanType==L34_WAN_TYPE_L3_ROUTE)))
				{
					//DEBUG("binding look up success! nhidx = %d",rg_db.wantype[wanTypeIdx].rtk_wantype.nhIdx);
					return rg_db.wantype[wanTypeIdx].rtk_wantype.nhIdx; //nextHopIdx for pPktHdr->bindNextHopIdx
				}	
				else
				{
					FIXME("Unmatched binding action!\n");
				}
			}
		}		
	}		
	//DEBUG("binding look up failed!!");
	return FAIL;
}

int _rtk_rg_eiplookup(ipaddr_t ip)
{
#if 0 //CONFIG_APOLLO_MODEL
	return 0;
#else
 	int i;

	for(i=0;i<MAX_EXTIP_HW_TABLE_SIZE;i++)
	{
		if(rg_db.extip[i].rtk_extip.valid)
		{		
			if(rg_db.extip[i].rtk_extip.extIpAddr==ip)
			{
				return i; //wan interface
			}
		}
	}
	return -1; 
#endif
}

int _rtk_rg_l3lookup(ipaddr_t ip)
{
#if 0 //CONFIG_APOLLO_MODEL
	return 0;
#else
 	int i;

	for(i=0;i<MAX_L3_SW_TABLE_SIZE-1;i++)
	{
		if(rg_db.l3[i].rtk_l3.valid==1)
		{		
			//DEBUG("ip=%x mask=%d iplookup=%x\n",rg_db.l3[i].rtk_l3.ipAddr,rg_db.l3[i].rtk_l3.ipMask,ip);
			if((rg_db.l3[i].rtk_l3.ipAddr>>(31-rg_db.l3[i].rtk_l3.ipMask))==(ip>>(31-rg_db.l3[i].rtk_l3.ipMask)))
			{
				//DEBUG("return %d",i);
				return i; //interface route
			}
		}
	}
	return 7; //return hit index (default route)
#endif
}

int _rtk_rg_v6L3lookup(unsigned char *ipv6)
{
 	int i,prefix;
	unsigned short idx,bitMask;
	//DEBUG("_rtk_rg_v6L3lookup");
	for(i=0;i<MAX_IPV6_ROUTING_SW_TABLE_SIZE-1;i++)
	{
		if(rg_db.v6route[i].rtk_v6route.valid==1)
		{		
			prefix=rg_db.v6route[i].rtk_v6route.ipv6PrefixLen;

			if(prefix==128&&memcmp(rg_db.v6route[i].rtk_v6route.ipv6Addr.ipv6_addr,ipv6,IPV6_ADDR_LEN))
				return i;	//host route
				
			idx=(prefix>>3)&0xff;
			
			if((prefix&0x7)==0)
				bitMask=0;
			else
				bitMask=(0xff<<(8-(prefix&0x7)))&0xff;
			//DEBUG("prefix = %d, idx = %d, bitMask =%02x",prefix,idx,bitMask);
			
			//DEBUG("ip=%x mask=%d iplookup=%x\n",rg_db.l3[i].rtk_l3.ipAddr,rg_db.l3[i].rtk_l3.ipMask,ip);
			if(memcmp(rg_db.v6route[i].rtk_v6route.ipv6Addr.ipv6_addr,ipv6,idx)==0 &&
				((rg_db.v6route[i].rtk_v6route.ipv6Addr.ipv6_addr[idx]&bitMask)==(ipv6[idx]&bitMask)))
			{
				//DEBUG("Match %d!!",i);
				return i; //interface route
			}
		}
	}
	if(rg_db.v6route[3].rtk_v6route.valid==0)	//if default route is invalid, return -1
		return -1;
	else
		return 3; //return hit index (default route)
}


int32 rtk_rg_arpEntry_add(rtk_rg_arpEntry_t *arpEntry, int *arp_entry_idx)
{

	int arpIdx;
	int l3Idx;
	int l2Idx;
	rtk_l2_addr_table_t asic_l2_entry;
	rtk_l34_arp_entry_t asic_arp_entry;
	rtk_rg_arp_linkList_t *pSwArpList;
	rtk_rg_arpInfo_t arpInfo;

	//Check input parameters
	if(rg_db.systemGlobal.vlanInit==0) return RT_ERR_RG_NOT_INIT;

	memset(&arpInfo,0,sizeof(arpInfo));
	
	//Get Gateway IP
	l3Idx=_rtk_rg_l3lookup(arpEntry->ipv4Addr);
	if(l3Idx==7) return RT_ERR_RG_INVALID_PARAM;
	
	if(rg_db.l3[l3Idx].rtk_l3.process==L34_PROCESS_ARP)
	{
		//Check ARP exist or not
		arpIdx=(rg_db.l3[l3Idx].rtk_l3.arpStart<<2)+(arpEntry->ipv4Addr & ((1<<(31-rg_db.l3[l3Idx].rtk_l3.ipMask))-1));
		if(arpIdx>=MAX_ARP_HW_TABLE_SIZE) return RT_ERR_RG_INVALID_PARAM;
		if(rg_db.arp[arpIdx].rtk_arp.valid && rg_db.arp[arpIdx].staticEntry)return RT_ERR_RG_ARP_ENTRY_STATIC;
		
		l2Idx = arpEntry->macEntryIdx;
		if(rtk_l2_nextValidEntry_get(&l2Idx,&asic_l2_entry)) return RT_ERR_RG_L2_ENTRY_NOT_FOUND;
		if(l2Idx!=arpEntry->macEntryIdx) return RT_ERR_RG_L2_ENTRY_NOT_FOUND;

		if(asic_l2_entry.entryType==RTK_LUT_L2UC)
		{
			if((asic_l2_entry.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_ARP_USED)==0)
			{
				asic_l2_entry.entry.l2UcEntry.flags|=RTK_L2_UCAST_FLAG_ARP_USED;
				ASSERT_EQ(RTK_L2_ADDR_ADD(&asic_l2_entry.entry.l2UcEntry),RT_ERR_OK);
				//memcpy(rg_db.mac[asic_l2_entry.entry.l2UcEntry.index].macAddr.octet,asic_l2_entry.entry.l2UcEntry.mac.octet,6);
			}	
		}

		asic_arp_entry.index=arpIdx;
		asic_arp_entry.nhIdx=arpEntry->macEntryIdx;
		asic_arp_entry.valid=1;
		ASSERT_EQ(RTK_L34_ARPTABLE_SET(arpIdx,&asic_arp_entry),RT_ERR_OK);
		rg_db.arp[arpIdx].staticEntry=arpEntry->staticEntry;
		rg_db.arp[arpIdx].idleSecs=0;
		rg_db.arp[arpIdx].ipv4Addr=arpEntry->ipv4Addr;
		rg_db.arpValidSet[arpIdx>>5] |= (0x1<<(arpIdx&31));
		DEBUG("### add arp[%d], ip=0x%x l2=%d l3idx=%d ###\n",arpIdx,arpEntry->ipv4Addr,l2Idx,l3Idx);
		*arp_entry_idx=arpIdx;
		arpInfo.valid = rg_db.arp[arpIdx].rtk_arp.valid;
	}
#ifdef CONFIG_APOLLO_ROMEDRIVER
	else if(rg_db.l3[l3Idx].rtk_l3.process==L34_PROCESS_CPU && rg_db.l3[l3Idx].rtk_l3.ipAddr>0)		//software ARP table 
	{	
		//Check software ARP exist or not
		_rtk_rg_softwareArpTableLookUp(l3Idx,arpEntry->ipv4Addr,&pSwArpList);
		if(pSwArpList!=NULL)
		{
				if(rg_db.arp[pSwArpList->idx].staticEntry==0)
				{
					//Delete old link-list first
					DEBUG("delete the old dynamic ARP link-list since arpEntryForceAdd==1");
					_rtk_rg_softwareArpTableDel(pSwArpList);
				}
				else
				{
					DEBUG("software ARP entry is valid and STATIC...skip add ARP");
					return RT_ERR_RG_ARP_ENTRY_STATIC;
				}					
		}

		l2Idx = arpEntry->macEntryIdx;
		if(rtk_l2_nextValidEntry_get(&l2Idx,&asic_l2_entry)) return RT_ERR_RG_L2_ENTRY_NOT_FOUND;
		if(l2Idx!=arpEntry->macEntryIdx) return RT_ERR_RG_L2_ENTRY_NOT_FOUND;

		if(asic_l2_entry.entryType==RTK_LUT_L2UC)
		{
			if((asic_l2_entry.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_ARP_USED)==0)
			{
				asic_l2_entry.entry.l2UcEntry.flags|=RTK_L2_UCAST_FLAG_ARP_USED;
				ASSERT_EQ(RTK_L2_ADDR_ADD(&asic_l2_entry.entry.l2UcEntry),RT_ERR_OK);
				//memcpy(rg_db.mac[asic_l2_entry.entry.l2UcEntry.index].macAddr.octet,asic_l2_entry.entry.l2UcEntry.mac.octet,6);
			}	
		}

		DEBUG("add software ARP %x! l2Idx is %d",arpEntry->ipv4Addr,l2Idx);
		assert_ok(_rtk_rg_softwareArpTableAdd(l3Idx,arpEntry->ipv4Addr,l2Idx,arpEntry->staticEntry));
	}
#endif
	else
		return RT_ERR_RG_INVALID_PARAM;

	if(rg_db.systemGlobal.initParam.arpAddByHwCallBack != NULL)
	{
		memcpy(&arpInfo.arpEntry,arpEntry,sizeof(rtk_rg_arpEntry_t));	
		rg_db.systemGlobal.initParam.arpAddByHwCallBack(&arpInfo);
	}
	return RT_ERR_RG_OK;
}

int32 rtk_rg_arpEntry_del(int arp_entry_idx)
{

	int retval=0,l2Idx=0,i;
	rtk_l2_addr_table_t asic_l2_entry;
	rtk_l34_arp_entry_t asic_arp_entry;
	rtk_rg_arpInfo_t arpInfo;
	rtk_rg_arp_linkList_t *pArpList,*pNextArpList;

	//Check input parameters
	if(rg_db.systemGlobal.vlanInit==0) return RT_ERR_RG_NOT_INIT;
	if(arp_entry_idx<0 ||arp_entry_idx>=MAX_ARP_SW_TABLE_SIZE)return RT_ERR_RG_INVALID_PARAM;

	//init
	memset(&arpInfo,0,sizeof(rtk_rg_arpInfo_t));

	if(arp_entry_idx<MAX_ARP_HW_TABLE_SIZE)
	{
		//Check whether entry is inused
		if(rg_db.arp[arp_entry_idx].rtk_arp.valid==0)return RT_ERR_RG_ARP_NOT_FOUND;

		//Sync to ARP info.
		arpInfo.arpEntry.ipv4Addr = rg_db.arp[arp_entry_idx].ipv4Addr;
		arpInfo.arpEntry.macEntryIdx = rg_db.arp[arp_entry_idx].rtk_arp.nhIdx;
		arpInfo.arpEntry.staticEntry = rg_db.arp[arp_entry_idx].staticEntry;

		//Sync to LUT
		l2Idx = rg_db.arp[arp_entry_idx].rtk_arp.nhIdx;

		memset(&asic_l2_entry,0,sizeof(rtk_l2_addr_table_t));
		retval = rtk_l2_nextValidEntry_get(&l2Idx,&asic_l2_entry);
		if((retval==RT_ERR_OK) && (rg_db.arp[arp_entry_idx].rtk_arp.nhIdx == asic_l2_entry.entry.l2UcEntry.index) && (asic_l2_entry.entry.l2UcEntry.flags & RTK_L2_UCAST_FLAG_STATIC)==0)
		{
#if 1
			retval = RTK_L2_ADDR_DEL(&asic_l2_entry.entry.l2UcEntry);
			ASSERT_EQ(retval,RT_ERR_OK);
#else
			asic_l2_entry.entry.l2UcEntry.flags &= (~RTK_L2_UCAST_FLAG_ARP_USED);
			retval = RTK_L2_ADDR_ADD(&asic_l2_entry.entry.l2UcEntry);
			//memcpy(rg_db.mac[asic_l2_entry.entry.l2UcEntry.index].macAddr.octet,asic_l2_entry.entry.l2UcEntry.mac.octet,6);
			ASSERT_EQ(retval,RT_ERR_OK);
#endif
		}
		//If L2 entry is not valid, do nothing
		//else
		//{
			//return RT_ERR_RG_L2_ENTRY_ACCESS_FAILED;
		//}

		//Clear ASIC and software ARP table entry
		memset(&asic_arp_entry,0,sizeof(rtk_l34_arp_entry_t));
		retval = RTK_L34_ARPTABLE_SET(arp_entry_idx,&asic_arp_entry);
		ASSERT_EQ(retval,RT_ERR_OK);
		rg_db.arp[arp_entry_idx].ipv4Addr=0;
		rg_db.arp[arp_entry_idx].staticEntry=0;
		rg_db.arp[arp_entry_idx].idleSecs=0;
	}
#ifdef CONFIG_APOLLO_ROMEDRIVER
	else	//deleting software ARP entry
	{
		//Check whether entry is inused
		if(rg_db.arp[arp_entry_idx].rtk_arp.valid==0)return RT_ERR_RG_ARP_NOT_FOUND;
	
		//Sync to ARP info.
		arpInfo.arpEntry.ipv4Addr = rg_db.arp[arp_entry_idx].ipv4Addr;
		arpInfo.arpEntry.macEntryIdx = rg_db.arp[arp_entry_idx].rtk_arp.nhIdx;
		arpInfo.arpEntry.staticEntry = rg_db.arp[arp_entry_idx].staticEntry;

		//Delete it
		retval=1;
		for(i=0;i<MAX_ARP_SW_TABLE_HEAD;i++)
		{
			list_for_each_entry_safe(pArpList,pNextArpList,&rg_db.softwareArpTableHead[i],arp_list)
			{
				if(pArpList->idx==arp_entry_idx)
				{
					_rtk_rg_softwareArpTableDel(pArpList);
					retval=0;
					break;
				}
			}
		}
		if(retval)
			return RT_ERR_RG_ARP_NOT_FOUND;

		//Clear ARP table entry
		memset(&rg_db.arp[arp_entry_idx],0,sizeof(rtk_rg_table_arp_t));
	}
#else
	else
		return RT_ERR_RG_INVALID_PARAM;
#endif
	rg_db.arpValidSet[arp_entry_idx>>5] &= ~(0x1<<(arp_entry_idx&31));

	if(rg_db.systemGlobal.initParam.arpDelByHwCallBack != NULL)
	{
		rg_db.systemGlobal.initParam.arpDelByHwCallBack(&arpInfo);
	}
	
	return RT_ERR_RG_OK;
}

int32 rtk_rg_arpEntry_find(rtk_rg_arpInfo_t *arpInfo,int *arp_valid_idx)
{
	rtk_rg_arpEntry_t *arpEntry;
	int arpIdx=0;
#ifdef CONFIG_APOLLO_ROMEDRIVER
	int threshold=MAX_ARP_SW_TABLE_SIZE;		//contains software ARP range
#else
	int threshold=MAX_ARP_HW_TABLE_SIZE;
#endif

	arpEntry=&arpInfo->arpEntry;

	//Check input parameters
	if(rg_db.systemGlobal.vlanInit==0) return RT_ERR_RG_NOT_INIT;

	//Check NULL ARP entry
	if(arpInfo==NULL) return RT_ERR_RG_NULL_POINTER;
	if((*arp_valid_idx>=threshold) && (*arp_valid_idx!=-1)) return RT_ERR_RG_INDEX_OUT_OF_RANGE;

	arpIdx = *arp_valid_idx;

	//Search by IP
	if(*arp_valid_idx==-1) arpIdx=0;
	//Check whether entry is inused
nextArpEntry:	
	if(arpIdx>=threshold) return RT_ERR_RG_NO_MORE_ENTRY_FOUND;
	
	if(rg_db.arp[arpIdx].rtk_arp.valid==0)
	{
		arpIdx++;
		goto nextArpEntry;
	}
	//Search by IP
	if(*arp_valid_idx==-1)
	{
		if(arpInfo->arpEntry.ipv4Addr != rg_db.arp[arpIdx].ipv4Addr)
		{
			arpIdx++;
			goto nextArpEntry;
		}
	}

	//Arp entry
	arpEntry->ipv4Addr=rg_db.arp[arpIdx].ipv4Addr;
	arpEntry->macEntryIdx=rg_db.arp[arpIdx].rtk_arp.nhIdx;
	arpEntry->staticEntry=rg_db.arp[arpIdx].staticEntry;
	arpInfo->idleSecs=rg_db.arp[arpIdx].idleSecs;
	arpInfo->valid=rg_db.arp[arpIdx].rtk_arp.valid;
	*arp_valid_idx=arpIdx;
	return RT_ERR_RG_OK;
}

int32 rtk_rg_neighborEntry_add(rtk_rg_neighborEntry_t *neighborEntry,int *neighbor_idx)
{

	int i,neighborIdx,hashValue,l3Idx,l2Idx;
	rtk_l2_addr_table_t asic_l2_entry;
	rtk_ipv6Neighbor_entry_t asic_neighbor_entry;
	rtk_rg_neighborInfo_t neighborInfo;

	//Check input parameters
	if(rg_db.systemGlobal.vlanInit==0) return RT_ERR_RG_NOT_INIT;

	//Get Gateway IP
	l3Idx=neighborEntry->matchRouteIdx;
	if(rg_db.v6route[l3Idx].rtk_v6route.type!=L34_IPV6_ROUTE_TYPE_LOCAL) return RT_ERR_RG_INVALID_PARAM;
	if(l3Idx>=MAX_IPV6_ROUTING_SW_TABLE_SIZE) return RT_ERR_RG_INVALID_PARAM;

	l2Idx=neighborEntry->l2Idx;
	if(rtk_l2_nextValidEntry_get(&l2Idx,&asic_l2_entry)!=RT_ERR_OK) return RT_ERR_RG_L2_ENTRY_NOT_FOUND;
	if(l2Idx!=neighborEntry->l2Idx) return RT_ERR_RG_L2_ENTRY_NOT_FOUND;

	if(asic_l2_entry.entryType==RTK_LUT_L2UC)
	{
		if((asic_l2_entry.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_ARP_USED)==0)
		{
			asic_l2_entry.entry.l2UcEntry.flags|=RTK_L2_UCAST_FLAG_ARP_USED;
			ASSERT_EQ(RTK_L2_ADDR_ADD(&asic_l2_entry.entry.l2UcEntry),RT_ERR_OK);
			//memcpy(rg_db.mac[asic_l2_entry.entry.l2UcEntry.index].macAddr.octet,asic_l2_entry.entry.l2UcEntry.mac.octet,6);
		}	
	}
	hashValue=_rtk_rg_IPv6NeighborHash(neighborEntry->interfaceId,l3Idx);
	neighborIdx=(hashValue<<3);
	for(i=0;i<8;i++)		//8-way hash
	{
		if(rg_db.v6neighbor[neighborIdx+i].rtk_v6neighbor.valid==0)
			break;
	}
	if(i==8) return RT_ERR_RG_NEIGHBOR_FULL;

	bzero(&asic_neighbor_entry,sizeof(rtk_ipv6Neighbor_entry_t));
	asic_neighbor_entry.valid=neighborEntry->valid;
	asic_neighbor_entry.l2Idx=l2Idx;
	asic_neighbor_entry.ipv6RouteIdx=l3Idx;
	asic_neighbor_entry.ipv6Ifid=neighborEntry->interfaceId[0];
	asic_neighbor_entry.ipv6Ifid<<=8;
	asic_neighbor_entry.ipv6Ifid|=neighborEntry->interfaceId[1];
	asic_neighbor_entry.ipv6Ifid<<=8;
	asic_neighbor_entry.ipv6Ifid|=neighborEntry->interfaceId[2];
	asic_neighbor_entry.ipv6Ifid<<=8;
	asic_neighbor_entry.ipv6Ifid|=neighborEntry->interfaceId[3];
	asic_neighbor_entry.ipv6Ifid<<=8;
	asic_neighbor_entry.ipv6Ifid|=neighborEntry->interfaceId[4];
	asic_neighbor_entry.ipv6Ifid<<=8;
	asic_neighbor_entry.ipv6Ifid|=neighborEntry->interfaceId[5];
	asic_neighbor_entry.ipv6Ifid<<=8;
	asic_neighbor_entry.ipv6Ifid|=neighborEntry->interfaceId[6];
	asic_neighbor_entry.ipv6Ifid<<=8;
	asic_neighbor_entry.ipv6Ifid|=neighborEntry->interfaceId[7];
	ASSERT_EQ(RTK_L34_IPV6NEIGHBORTABLE_SET(neighborIdx,&asic_neighbor_entry),RT_ERR_OK);

	rg_db.v6neighbor[neighborIdx].staticEntry=neighborEntry->staticEntry;
	rg_db.v6neighbor[neighborIdx].idleSecs=0;
	DEBUG("### add neighbor[%d], ifid=0x%016llx l2=%d l3idx=%d ###\n",neighborIdx,asic_neighbor_entry.ipv6Ifid,l2Idx,l3Idx);
	*neighbor_idx=neighborIdx;

	// TODO:Call the initParam's neighborAddByHwCallBack
	if(rg_db.systemGlobal.initParam.neighborAddByHwCallBack!= NULL)
	{
		bzero(&neighborInfo,sizeof(rtk_rg_neighborInfo_t));
		memcpy(&neighborInfo.neighborEntry,neighborEntry,sizeof(rtk_rg_neighborEntry_t));
		rg_db.systemGlobal.initParam.neighborAddByHwCallBack(&neighborInfo);
	}
	return RT_ERR_RG_OK;
}

int32 rtk_rg_neighborEntry_del(int neighbor_idx)
{
	int retval = 0,i,l2Idx = 0;
	rtk_l2_addr_table_t asic_l2_entry;
	rtk_ipv6Neighbor_entry_t asic_neighbor_entry;
	rtk_rg_neighborInfo_t neighborInfo;

	//Check input parameters
	if(rg_db.systemGlobal.vlanInit==0) return RT_ERR_RG_NOT_INIT;

	//Check whether entry is inused
	if(rg_db.v6neighbor[neighbor_idx].rtk_v6neighbor.valid==0)
		return RT_ERR_RG_NEIGHBOR_NOT_FOUND;

	//Sync to Neighbor info.
	bzero(&neighborInfo,sizeof(rtk_rg_neighborInfo_t));
	neighborInfo.neighborEntry.l2Idx=rg_db.v6neighbor[neighbor_idx].rtk_v6neighbor.l2Idx;
	neighborInfo.neighborEntry.matchRouteIdx=rg_db.v6neighbor[neighbor_idx].rtk_v6neighbor.ipv6RouteIdx;
	neighborInfo.neighborEntry.valid=rg_db.v6neighbor[neighbor_idx].rtk_v6neighbor.valid;
	neighborInfo.neighborEntry.staticEntry=rg_db.v6neighbor[neighbor_idx].staticEntry;
	for(i=0;i<8;i++)
		neighborInfo.neighborEntry.interfaceId[i]=(rg_db.v6neighbor[neighbor_idx].rtk_v6neighbor.ipv6Ifid>>(56-(8*i)))&0xff;
	neighborInfo.idleSecs=rg_db.v6neighbor[neighbor_idx].idleSecs;

	//Sync to LUT
	l2Idx = rg_db.v6neighbor[neighbor_idx].rtk_v6neighbor.l2Idx;

	memset(&asic_l2_entry,0,sizeof(rtk_l2_addr_table_t));
	retval = rtk_l2_nextValidEntry_get(&l2Idx,&asic_l2_entry);
	if(retval==RT_ERR_OK && rg_db.v6neighbor[neighbor_idx].rtk_v6neighbor.l2Idx == asic_l2_entry.entry.l2UcEntry.index)
	{
		asic_l2_entry.entry.l2UcEntry.flags &= (~RTK_L2_UCAST_FLAG_ARP_USED);
		retval = RTK_L2_ADDR_ADD(&asic_l2_entry.entry.l2UcEntry);
		//memcpy(rg_db.mac[asic_l2_entry.entry.l2UcEntry.index].macAddr.octet,asic_l2_entry.entry.l2UcEntry.mac.octet,6);
		ASSERT_EQ(retval,RT_ERR_OK);
	}
	//If L2 entry is not valid, do nothing

	//Delete from ASIC
	bzero(&asic_neighbor_entry,sizeof(rtk_ipv6Neighbor_entry_t));
	ASSERT_EQ(RTK_L34_IPV6NEIGHBORTABLE_SET(neighbor_idx,&asic_neighbor_entry),RT_ERR_OK);
	rg_db.v6neighbor[neighbor_idx].staticEntry=0;
	rg_db.v6neighbor[neighbor_idx].idleSecs=0;

	// TODO:Call the initParam's neighborDelByHwCallBack
	if(rg_db.systemGlobal.initParam.neighborDelByHwCallBack != NULL)
	{
		rg_db.systemGlobal.initParam.neighborDelByHwCallBack(&neighborInfo);
	}
	return RT_ERR_RG_OK;
}

int32 rtk_rg_neighborEntry_find(rtk_rg_neighborInfo_t *neighborInfo,int *neighbor_valid_idx)
{
	rtk_rg_neighborEntry_t *neighborEntry;
	int neighborIdx=0,count=0;
	unsigned char ipv6Ifid[8];

	//Check input parameters
	if(rg_db.systemGlobal.vlanInit==0) return RT_ERR_RG_NOT_INIT;

	//Check NULL Neighbor entry
	if(neighborInfo==NULL || neighbor_valid_idx==NULL) return RT_ERR_RG_NULL_POINTER;
	if((*neighbor_valid_idx>=MAX_IPV6_NEIGHBOR_SW_TABLE_SIZE) || (*neighbor_valid_idx<-1)) return RT_ERR_RG_INDEX_OUT_OF_RANGE;

	neighborEntry = &neighborInfo->neighborEntry;
	neighborIdx = *neighbor_valid_idx;

	//Search by MAC
	if(neighborIdx==-1) 
	{
		neighborIdx=_rtk_rg_IPv6NeighborHash(neighborInfo->neighborEntry.interfaceId,neighborInfo->neighborEntry.matchRouteIdx)<<3;
		memcpy(ipv6Ifid,neighborInfo->neighborEntry.interfaceId,8);
	}
	//Check whether entry is valid
nextNeighborEntry:	

	if(neighborIdx>=MAX_IPV6_NEIGHBOR_SW_TABLE_SIZE) return RT_ERR_RG_NO_MORE_ENTRY_FOUND;
	if(count==8)return RT_ERR_RG_NEIGHBOR_NOT_FOUND;

	//If this entry is invalid, goto next one
	if(rg_db.v6neighbor[neighborIdx].rtk_v6neighbor.valid==0)
	{
		neighborIdx++;
		if(*neighbor_valid_idx==-1)
			count++;
		goto nextNeighborEntry;
	}
	
	//Search by MAC
	if(*neighbor_valid_idx==-1)
	{
		if(memcmp(ipv6Ifid,(unsigned char *)&rg_db.v6neighbor[neighborIdx].rtk_v6neighbor.ipv6Ifid,8)||
			neighborInfo->neighborEntry.matchRouteIdx!=rg_db.v6neighbor[neighborIdx].rtk_v6neighbor.ipv6RouteIdx)
		{
			neighborIdx++;
			count++;
			goto nextNeighborEntry;
		}
	}

	//Neighbor Entry
	neighborEntry->l2Idx=rg_db.v6neighbor[neighborIdx].rtk_v6neighbor.l2Idx;
	neighborEntry->matchRouteIdx=rg_db.v6neighbor[neighborIdx].rtk_v6neighbor.ipv6RouteIdx;
	neighborEntry->valid=rg_db.v6neighbor[neighborIdx].rtk_v6neighbor.valid;
	neighborEntry->staticEntry=rg_db.v6neighbor[neighborIdx].staticEntry;
	for(count=0;count<8;count++)
		neighborInfo->neighborEntry.interfaceId[count]=(rg_db.v6neighbor[neighborIdx].rtk_v6neighbor.ipv6Ifid>>(56-(8*count)))&0xff;
	neighborInfo->idleSecs=rg_db.v6neighbor[neighborIdx].idleSecs;

	*neighbor_valid_idx=neighborIdx;
	return RT_ERR_RG_OK;
}


int rtk_rg_multicastDataIngressAlllowedPortMask_set(rtk_rg_mac_portmask_t *macPort_mask)
{
	return RT_ERR_RG_OK;
}

int rtk_rg_multicastDataIngressAlllowedPortMask_get(rtk_rg_mac_portmask_t *macPort_mask)
{
	return RT_ERR_RG_OK;
}


int32 rtk_rg_portMirror_set(rtk_rg_portMirrorInfo_t portMirrorInfo)
{
	rtk_portmask_t mirroredRxPortmask;
	rtk_portmask_t mirroredTxPortmask;
	memset(&mirroredRxPortmask,0,sizeof(rtk_portmask_t));
	memset(&mirroredTxPortmask,0,sizeof(rtk_portmask_t));

	if(portMirrorInfo.direct==RTK_RG_MIRROR_RX_ONLY){
		mirroredRxPortmask.bits[0] = portMirrorInfo.enabledPortMask.portmask;
		mirroredTxPortmask.bits[0] = 0x0;
	}else if(portMirrorInfo.direct==RTK_RG_MIRROR_TX_ONLY){
		mirroredRxPortmask.bits[0] = 0x0;
		mirroredTxPortmask.bits[0] = portMirrorInfo.enabledPortMask.portmask;
	}else{//RTK_RG_MIRROR_TX_RX_BOTH
		mirroredRxPortmask.bits[0] = portMirrorInfo.enabledPortMask.portmask;
		mirroredTxPortmask.bits[0] = portMirrorInfo.enabledPortMask.portmask;
	}
	ASSERT_EQ(rtk_mirror_portBased_set(portMirrorInfo.monitorPort, &mirroredRxPortmask, &mirroredTxPortmask),SUCCESS);
	
	return RT_ERR_RG_OK;
}

int32 rtk_rg_portMirror_get(rtk_rg_portMirrorInfo_t *portMirrorInfo)
{
	rtk_portmask_t mirroredRxPortmask;
	rtk_portmask_t mirroredTxPortmask;
	memset(&mirroredRxPortmask,0,sizeof(rtk_portmask_t));
	memset(&mirroredTxPortmask,0,sizeof(rtk_portmask_t));
	ASSERT_EQ(rtk_mirror_portBased_get(&(portMirrorInfo->monitorPort), &mirroredRxPortmask, &mirroredTxPortmask),SUCCESS);
	if(mirroredRxPortmask.bits[0]==0x0 && mirroredTxPortmask.bits[0]!=0x0){
		portMirrorInfo->enabledPortMask.portmask = mirroredRxPortmask.bits[0] ;
		portMirrorInfo->direct = RTK_RG_MIRROR_RX_ONLY;
	}else if(mirroredRxPortmask.bits[0]!=0x0 && mirroredTxPortmask.bits[0]==0x0){
		portMirrorInfo->enabledPortMask.portmask= mirroredTxPortmask.bits[0] ;
		portMirrorInfo->direct = RTK_RG_MIRROR_TX_ONLY;
	}else if(mirroredRxPortmask.bits[0]!=0x0 && mirroredTxPortmask.bits[0]!=0x0){
		portMirrorInfo->enabledPortMask.portmask= (mirroredRxPortmask.bits[0] | mirroredTxPortmask.bits[0]) ;
		portMirrorInfo->direct = RTK_RG_MIRROR_TX_RX_BOTH;
	}
	

	return RT_ERR_RG_OK;
}

int32 rtk_rg_portMirror_clear(void)
{
	rtk_portmask_t mirroredRxPortmask;
	rtk_portmask_t mirroredTxPortmask;
	memset(&mirroredRxPortmask,0,sizeof(rtk_portmask_t));
	memset(&mirroredTxPortmask,0,sizeof(rtk_portmask_t));
	mirroredRxPortmask.bits[0] = 0x0;
	mirroredTxPortmask.bits[0] = 0x0;
	ASSERT_EQ(rtk_mirror_portBased_set(0x0, &mirroredRxPortmask, &mirroredTxPortmask),SUCCESS);
	return RT_ERR_RG_OK;
}


int32 rtk_rg_portEgrBandwidthCtrlRate_set(rtk_rg_mac_port_idx_t port, uint32 rate)
{
	if(rate < 8 || rate > 1048568){
		if(rate==0)
			rate = 1048568; //rate==0 means unlimit in rtk API
		else
			return RT_ERR_RG_INVALID_PARAM;
	}
	ASSERT_EQ(rtk_rate_portEgrBandwidthCtrlRate_set(port,rate),SUCCESS);
	return RT_ERR_RG_OK;
}

#ifdef RTK_RG_INGRESS_QOS_TESTING
int _rtk_rg_qos_acl_flush(void)
{
	int i,j;
	for(i=0;i<RTK_RG_MAC_PORT_PON;i++)
	{
		for(j=4;j>=0;j--)
		{
			if(rg_db.systemGlobal.qos_acl_patch[i][j])
			{
				DEBUG("DEL ACL index:%d\n",rg_db.systemGlobal.qos_acl_patch[i][j]-1);
				rtk_rg_aclFilterAndQos_del(rg_db.systemGlobal.qos_acl_patch[i][j]-1);
				rg_db.systemGlobal.qos_acl_patch[i][j]=0;
			}
		}
	}
	rg_db.systemGlobal.qos_acl_total_patch=0;
	return SUCCESS;
}

int _rtk_rg_qos_acl_patch(rtk_rg_mac_port_idx_t port, uint32 rate)
{
	int i;
	
	rg_db.systemGlobal.qos_ingress_total_rate-=rg_db.systemGlobal.qos_ingress_rate[port];
	rg_db.systemGlobal.qos_ingress_rate[port]=rate;
	rg_db.systemGlobal.qos_ingress_total_rate+=rate;

	//If total ingress rate < 1000 Mbps, clear all patch
	DEBUG("[QOS PATCH]Total rate:%d\n",rg_db.systemGlobal.qos_ingress_total_rate);

	if((rg_db.systemGlobal.qos_type==RTK_RG_INGRESS_QOS_ORIGINAL) || (port>=RTK_RG_MAC_PORT_PON))
	{
		//Normal ingress bandwidth control
		DEBUG("[QoS PATCH]Nomal mode.");
		ASSERT_EQ(rtk_rg_shareMeter_set(0,1048568,0),RT_ERR_RG_OK);
		ASSERT_EQ(rtk_rate_portIgrBandwidthCtrlRate_set(port,rate),SUCCESS);
	}
	else if(rg_db.systemGlobal.qos_type==RTK_RG_INGRESS_QOS_ALL_HIGH_QUEUE)
	{	
		//All queue of rate limit port re-map to high queue
		rtk_rg_aclFilterAndQos_t acl_filter;
		int aclIdx;

		DEBUG("[QoS PATCH]Patch 1...");
		ASSERT_EQ(rtk_rg_shareMeter_set(0,1048568,0),RT_ERR_RG_OK);
		if(rg_db.systemGlobal.qos_acl_patch[port][0])
		{
			DEBUG("DEL ACL index:%d\n",rg_db.systemGlobal.qos_acl_patch[port][0]-1);
			ASSERT_EQ(rtk_rg_aclFilterAndQos_del(rg_db.systemGlobal.qos_acl_patch[port][0]-1),RT_ERR_RG_OK);
			rg_db.systemGlobal.qos_acl_patch[port][0]=0;
			rg_db.systemGlobal.qos_acl_total_patch--;
		}
		if((rate<=250000) && (rg_db.systemGlobal.qos_acl_total_patch<3))
		{
			DEBUG("[QoS PATCH 1]...");
			//if(rg_db.systemGlobal.qos_acl_total_patch==4) goto NORMAL_QOS;
			memset(&acl_filter,0,sizeof(rtk_rg_aclFilterAndQos_t));
			acl_filter.filter_fields = INGRESS_PORT_BIT;
			acl_filter.ingress_port_mask.portmask=(0x1<<port);
			acl_filter.action_type=ACL_ACTION_TYPE_QOS;
			acl_filter.qos_actions=ACL_ACTION_QUEUE_ID_BIT;
			acl_filter.action_queue_id=6-rg_db.systemGlobal.qos_acl_total_patch;
			ASSERT_EQ(rtk_rg_aclFilterAndQos_add(&acl_filter,&aclIdx),RT_ERR_RG_OK);
			rg_db.systemGlobal.qos_acl_patch[port][0]=aclIdx+1; //0 for invalid
			rg_db.systemGlobal.qos_acl_total_patch++;
		}
		ASSERT_EQ(rtk_rate_portIgrBandwidthCtrlRate_set(port,rate),SUCCESS);
	}
	else if(rg_db.systemGlobal.qos_type==RTK_RG_INGRESS_QOS_STRICT_HIGH_QUEUE)
	{
		DEBUG("[QoS PATCH]Patch 2...");
		DEBUG("[QoS PATCH 2]Queue mode[%d]=0x%x\n",port,rg_db.systemGlobal.qos_queue_mode[port]);
		//High queue of rate limit port re-map to share meter rate limit queue,others re-map to low queue.
		rtk_rg_aclFilterAndQos_t acl_filter;
		int aclIdx;
		int i;
		int dscp[4]={48,32,16,0};
		int int_pri;

		//Find DSCP value
		for(i=63;i>=0;i--)
		{
			ASSERT_EQ(rtk_rg_qosDscpPriRemapGroup_get(i,&int_pri),RT_ERR_RG_OK);
			if((int_pri>=0)&&(int_pri<4))
				dscp[int_pri]=i;
		}

		//Add ACL rule
		if(rg_db.systemGlobal.qos_acl_patch[port][4])
		{
			for(i=4;i>=0;i--)
			{
				if(rg_db.systemGlobal.qos_acl_patch[port][i])
				{
					DEBUG("DEL ACL index:%d\n",rg_db.systemGlobal.qos_acl_patch[port][i]-1);
					ASSERT_EQ(rtk_rg_aclFilterAndQos_del(rg_db.systemGlobal.qos_acl_patch[port][i]-1),RT_ERR_RG_OK);
					rg_db.systemGlobal.qos_acl_patch[port][i]=0;
				}
			}
			rg_db.systemGlobal.qos_acl_total_patch--;
		}
		if(((rate/4)<250000) && (rg_db.systemGlobal.qos_acl_total_patch<4))
		{
			DEBUG("[QOS PATCH 2]...");
			int hq_num=rg_db.systemGlobal.qos_queue_mode[port];
			ASSERT_EQ(rtk_rate_portIgrBandwidthCtrlRate_set(port,1048568),SUCCESS);

			for(int_pri=3;int_pri>=0;int_pri--)
			{
				hq_num--;
				if(hq_num<0) break;
				DEBUG("rg_db.systemGlobal.qos_queue_mode[port]:%x : %x\n",rg_db.systemGlobal.qos_queue_mode[port],(0x1<<int_pri));
				DEBUG("DSCP[%d]=%d\n",int_pri,dscp[int_pri]);
				if(dscp[int_pri]>=0)
				{
					memset(&acl_filter,0,sizeof(rtk_rg_aclFilterAndQos_t));
					acl_filter.filter_fields = INGRESS_PORT_BIT | INGRESS_DSCP_BIT;
					acl_filter.ingress_port_mask.portmask=(0x1<<port);
					acl_filter.ingress_dscp=dscp[int_pri];
					acl_filter.action_type=ACL_ACTION_TYPE_QOS;
					acl_filter.qos_actions=ACL_ACTION_SHARE_METER_BIT | ACL_ACTION_QUEUE_ID_BIT;
					acl_filter.action_share_meter=6-rg_db.systemGlobal.qos_acl_total_patch;
					acl_filter.action_queue_id=6-rg_db.systemGlobal.qos_acl_total_patch;
					ASSERT_EQ(rtk_rg_aclFilterAndQos_add(&acl_filter,&aclIdx),RT_ERR_RG_OK);
					rg_db.systemGlobal.qos_acl_patch[port][int_pri]=aclIdx+1; //0 for invalid
				}
			}

			ASSERT_EQ(rtk_rg_shareMeter_set(6-rg_db.systemGlobal.qos_acl_total_patch,rate,0),RT_ERR_RG_OK);

			memset(&acl_filter,0,sizeof(rtk_rg_aclFilterAndQos_t));
			acl_filter.filter_fields = INGRESS_PORT_BIT;
			acl_filter.ingress_port_mask.portmask=(0x1<<port);
			acl_filter.action_type=ACL_ACTION_TYPE_QOS;
			acl_filter.qos_actions=ACL_ACTION_QUEUE_ID_BIT | ACL_ACTION_SHARE_METER_BIT;
			acl_filter.action_queue_id=0;
			acl_filter.action_share_meter=0;
			ASSERT_EQ(rtk_rg_aclFilterAndQos_add(&acl_filter,&aclIdx),RT_ERR_RG_OK);
			rg_db.systemGlobal.qos_acl_patch[port][4]=aclIdx+1; //0 for invalid
			rg_db.systemGlobal.qos_acl_total_patch++;

			//low queue drop
			if(rg_db.systemGlobal.qos_ingress_total_rate<1000000)
				ASSERT_EQ(rtk_rg_shareMeter_set(0,8,0),RT_ERR_RG_OK);
			else
				ASSERT_EQ(rtk_rg_shareMeter_set(0,1048568,0),RT_ERR_RG_OK);
		}
		else
		{
			ASSERT_EQ(rtk_rate_portIgrBandwidthCtrlRate_set(port,rate),SUCCESS);
		}
	}
	return SUCCESS;
}

int rtk_rg_qos_queue_mode(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int i,j;
//	int orig_mode[RTK_RG_MAC_PORT_PON];
	unsigned char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;

#if 0
	for(i=0;i<RTK_RG_MAC_PORT_PON;i++)
		orig_mode[i] = rg_db.systemGlobal.qos_queue_mode[i];
#endif

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
		int queue_mode=simple_strtoul(tmpBuf, NULL, 0);
		for(i=0;i<RTK_RG_MAC_PORT_PON;i++)
			rg_db.systemGlobal.qos_queue_mode[i]=(queue_mode>>(i<<2))&0xf;

		if(rg_db.systemGlobal.qos_type==RTK_RG_INGRESS_QOS_STRICT_HIGH_QUEUE)
		{
			_rtk_rg_qos_acl_flush();
			for(i=0;i<RTK_RG_MAC_PORT_PON;i++)
			{
//				if(orig_mode[i]!=rg_db.systemGlobal.qos_queue_mode[i])
				{
					_rtk_rg_qos_acl_patch(i,rg_db.systemGlobal.qos_ingress_rate[i]);
				}
			}
		}
		return count;
	}
	return -EFAULT;
}

int rtk_rg_qos_low_queue_drop( struct file *filp, const char *buff,unsigned long len, void *data )
{
	char	*tmpbuf;	
	char	*strptr=NULL;
	int i;

	tmpbuf=rtk_rg_malloc(512);
	if(tmpbuf==NULL) return 0;
	
	if (buff && !copy_from_user(tmpbuf, buff, len)) 
	{		
		tmpbuf[len] = '\0'; 			
		strptr=tmpbuf;
	}

	if(rg_db.systemGlobal.qos_type==RTK_RG_INGRESS_QOS_STRICT_HIGH_QUEUE)
	{
		if(strncmp(strptr, "0",1) == 0)
			ASSERT_EQ(rtk_rg_shareMeter_set(0,1048568,0),RT_ERR_RG_OK);
		else if(strncmp(strptr, "1",1) == 0)
			ASSERT_EQ(rtk_rg_shareMeter_set(0,8,0),RT_ERR_RG_OK);
	}
	
	rtk_rg_free(tmpbuf);
	return len;

}


int rtk_rg_qos_type_sel( struct file *filp, const char *buff,unsigned long len, void *data )
{
	char	*tmpbuf;	
	char	*strptr=NULL;
	int i;

	tmpbuf=rtk_rg_malloc(512);
	if(tmpbuf==NULL) return 0;
	
	if (buff && !copy_from_user(tmpbuf, buff, len))	
	{		
		tmpbuf[len] = '\0';				
		strptr=tmpbuf;
	}

	if(strncmp(strptr, "0",1) == 0)
		rg_db.systemGlobal.qos_type=RTK_RG_INGRESS_QOS_ORIGINAL;
	else if(strncmp(strptr, "1",1) == 0)
		rg_db.systemGlobal.qos_type=RTK_RG_INGRESS_QOS_ALL_HIGH_QUEUE;
	else if(strncmp(strptr, "2",1) == 0)
		rg_db.systemGlobal.qos_type=RTK_RG_INGRESS_QOS_STRICT_HIGH_QUEUE;
	
	_rtk_rg_qos_acl_flush();

	//Re-config ingress rate
	for(i=0;i<RTK_RG_MAC_PORT_PON;i++)
	{
		if(rg_db.systemGlobal.qos_ingress_rate[i])
		{
			_rtk_rg_qos_acl_patch(i,rg_db.systemGlobal.qos_ingress_rate[i]);
		}
	}
	
	rtk_rg_free(tmpbuf);
	return len;

}
#endif

int32 rtk_rg_portIgrBandwidthCtrlRate_set(rtk_rg_mac_port_idx_t port, uint32 rate)
{
	if(rate < 8 || rate > 1048568){
		if(rate==0)
			rate = 1048568; //rate==0 means unlimit.
		else
			return RT_ERR_RG_INVALID_PARAM;
	}
#ifdef RTK_RG_INGRESS_QOS_TESTING
	ASSERT_EQ(_rtk_rg_qos_acl_patch(port,rate),SUCCESS);;
#else
	ASSERT_EQ(rtk_rate_portIgrBandwidthCtrlRate_set(port,rate),SUCCESS);
#endif
	return RT_ERR_RG_OK;
}

int32 rtk_rg_portEgrBandwidthCtrlRate_get(rtk_rg_mac_port_idx_t port, uint32 *rate)
{
	if(rate==NULL) return RT_ERR_RG_NULL_POINTER;
	ASSERT_EQ(rtk_rate_portEgrBandwidthCtrlRate_get(port,rate),SUCCESS);
	return RT_ERR_RG_OK;
}

int32 rtk_rg_portIgrBandwidthCtrlRate_get(rtk_rg_mac_port_idx_t port, uint32 *rate)
{
	if(rate==NULL) return RT_ERR_RG_NULL_POINTER;
	ASSERT_EQ(rtk_rate_portIgrBandwidthCtrlRate_get(port,rate),SUCCESS);
	return RT_ERR_RG_OK;
}


int32 rtk_rg_phyPortForceAbility_set(rtk_rg_mac_port_idx_t port, rtk_rg_phyPortAbilityInfo_t ability)
{	

	uint32 reg;
	rtk_port_phy_ability_t  auto_ability;
	bzero(&auto_ability,sizeof(rtk_port_phy_ability_t));

	//parameter check
	if(port>=RTK_RG_MAC_PORT_MAX) return RT_ERR_RG_INVALID_PARAM;
	if(ability.speed>=RTK_RG_PORT_SPEED_END) return RT_ERR_RG_INVALID_PARAM;	
	if(ability.duplex==RTK_RG_PORT_HALF_DUPLEX && ability.flowCtrl==RTK_RG_ENABLED) return RT_ERR_RG_INVALID_PARAM;

	if(ability.force_disable_phy==RTK_RG_ENABLED){
		assert_ok(rtk_port_phyReg_get(port,0,0,&reg)); //get original phy reg data
		reg|=0x800; //force disabled phy
		assert_ok(rtk_port_phyReg_set(port,0,0,reg));
	}else{
		assert_ok(rtk_port_phyReg_get(port,0,0,&reg)); //get original phy reg data
		reg&=~(0x800); //enabled phy (bit11) & reset
		assert_ok(rtk_port_phyReg_set(port,0,0,reg));
	}

	if(ability.valid==RTK_RG_ENABLED){
		if(ability.speed==RTK_RG_PORT_SPEED_10M && ability.duplex==RTK_RG_PORT_HALF_DUPLEX){
			auto_ability.Half_10 = ENABLED;
		}else if(ability.speed==RTK_RG_PORT_SPEED_10M && ability.duplex==RTK_RG_PORT_FULL_DUPLEX){
			auto_ability.Full_10 = ENABLED;
		}else if(ability.speed==RTK_RG_PORT_SPEED_100M && ability.duplex==RTK_RG_PORT_HALF_DUPLEX){
			auto_ability.Half_100 = ENABLED;
		}else if(ability.speed==RTK_RG_PORT_SPEED_100M && ability.duplex==RTK_RG_PORT_FULL_DUPLEX){
			auto_ability.Full_100 = ENABLED;
		}else if(ability.speed==RTK_RG_PORT_SPEED_1000M && ability.duplex==RTK_RG_PORT_FULL_DUPLEX){
			auto_ability.Full_1000 = ENABLED;
		}else {
			return RT_ERR_RG_INVALID_PARAM;
		}

		if(ability.flowCtrl==RTK_RG_ENABLED){
			auto_ability.FC=ENABLED;
			auto_ability.AsyFC=ENABLED;
		}else{
			auto_ability.FC=DISABLED;
			auto_ability.AsyFC=DISABLED;
		}	
		
		assert_ok(rtk_port_phyAutoNegoAbility_set(port,&auto_ability));
	}else{
		//set auto_ability to default (enable all speed & AsyFC)
		auto_ability.AsyFC = ENABLED;
		auto_ability.FC = ENABLED;
		auto_ability.Half_10 = ENABLED;
		auto_ability.Full_10= ENABLED;
		auto_ability.Half_100 = ENABLED;
		auto_ability.Full_100 = ENABLED;
		auto_ability.Half_1000 = DISABLED; 
		auto_ability.Full_1000 = ENABLED;
		assert_ok(rtk_port_phyAutoNegoAbility_set(port,&auto_ability));
	}

	return RT_ERR_RG_OK;	
}

int32 rtk_rg_phyPortForceAbility_get(rtk_rg_mac_port_idx_t port, rtk_rg_phyPortAbilityInfo_t *ability)
{
	uint32 	reg;

	rtk_port_phy_ability_t  auto_ability;
	bzero(&auto_ability,sizeof(rtk_port_phy_ability_t));
	
	assert_ok(rtk_port_phyReg_get(port,0,0,&reg)); 
	if(reg&0x800){
		ability->force_disable_phy = RTK_RG_ENABLED;
	}else{
		ability->force_disable_phy = RTK_RG_DISABLED;
	}

	assert_ok(rtk_port_phyAutoNegoAbility_get(port,&auto_ability));

	if(auto_ability.Half_10==ENABLED &&
		auto_ability.Full_10==ENABLED &&
		auto_ability.Half_100==ENABLED &&
		auto_ability.Full_100==ENABLED &&
		auto_ability.Full_1000==ENABLED &&
		auto_ability.FC==ENABLED &&
		auto_ability.AsyFC==ENABLED){//default setting
		
		ability->valid=DISABLED;
		ability->speed=RTK_RG_PORT_SPEED_1000M;
		ability->duplex=RTK_RG_PORT_FULL_DUPLEX;
		ability->flowCtrl=ENABLED;

	}else{
		ability->valid=ENABLED;
 		if(auto_ability.Half_10==ENABLED){ability->speed=RTK_RG_PORT_SPEED_10M; ability->duplex=RTK_RG_PORT_HALF_DUPLEX;}
		if(auto_ability.Full_10==ENABLED){ability->speed=RTK_RG_PORT_SPEED_10M; ability->duplex=RTK_RG_PORT_FULL_DUPLEX; }
		if(auto_ability.Half_100==ENABLED){ability->speed=RTK_RG_PORT_SPEED_100M; ability->duplex=RTK_RG_PORT_HALF_DUPLEX; }
		if(auto_ability.Full_100==ENABLED){ability->speed=RTK_RG_PORT_SPEED_100M; ability->duplex=RTK_RG_PORT_FULL_DUPLEX; }
		if(auto_ability.Full_1000==ENABLED){ability->speed=RTK_RG_PORT_SPEED_1000M; ability->duplex=RTK_RG_PORT_FULL_DUPLEX; }
		if(auto_ability.AsyFC==ENABLED && auto_ability.FC==ENABLED){ability->flowCtrl=ENABLED;}
	}

	return RT_ERR_RG_OK;
}



int _rtk_rg_is_stormControlEntry_init(void)
{
    if(rg_db.systemGlobal.stormControlInfoEntry==NULL)
        return RT_ERR_RG_NULL_POINTER;
    else
        return RT_ERR_RG_OK;
}

int _rtk_rg_stormControlEntry_init(void)
{

	if(rg_db.systemGlobal.stormControlInfoEntry!=NULL)
		rtk_rg_free(rg_db.systemGlobal.stormControlInfoEntry);
	
    rg_db.systemGlobal.stormControlInfoEntry = rtk_rg_malloc(sizeof(rtk_rg_stormControlInfo_t)*MAX_STORMCONTROL_ENTRY_SIZE);
    if(rg_db.systemGlobal.stormControlInfoEntry==NULL)
    {
        return RT_ERR_RG_NULL_POINTER;
    }
    else
    {
        bzero(rg_db.systemGlobal.stormControlInfoEntry, sizeof(rtk_rg_stormControlInfo_t)*MAX_STORMCONTROL_ENTRY_SIZE);
    }

    return RT_ERR_RG_OK;
}

int32 _rtk_rg_stormControl_search_empty(int* stormInfo_idx){
	int i;

	
	if(_rtk_rg_is_stormControlEntry_init())
		_rtk_rg_stormControlEntry_init();

	for(i=0;i<MAX_STORMCONTROL_ENTRY_SIZE;i++){
		if(rg_db.systemGlobal.stormControlInfoEntry[i].valid==RTK_RG_DISABLED){
			*stormInfo_idx=i;
			break;
		}
	}

	if(i==MAX_STORMCONTROL_ENTRY_SIZE){
		*stormInfo_idx = -1;
		return RT_ERR_RG_STORMCONTROL_ENTRY_FULL;
	}
	
	return RT_ERR_RG_OK;
}

int32 _rtk_rg_stormControl_duplex_check(rtk_rg_stormControlInfo_t *stormInfo){
	int i;

	if(_rtk_rg_is_stormControlEntry_init())
		_rtk_rg_stormControlEntry_init();

	for(i=0;i<MAX_STORMCONTROL_ENTRY_SIZE;i++){
		if(rg_db.systemGlobal.stormControlInfoEntry[i].valid==RTK_RG_ENABLED){
			
			if((rg_db.systemGlobal.stormControlInfoEntry[i].stormType==stormInfo->stormType) &&
				(rg_db.systemGlobal.stormControlInfoEntry[i].port==stormInfo->port)){
				return RT_ERR_RG_STORMCONTROL_ENTRY_HAS_BEEN_SET;
			}
		}
	}
	
	return RT_ERR_RG_OK;
}


int32 rtk_rg_stormControl_add(rtk_rg_stormControlInfo_t *stormInfo,int *stormInfo_idx)
{
	
	int ret;
	rtk_rate_storm_group_ctrl_t stormTypeEnable;
	//Check rg has been init
    if(rg_db.systemGlobal.vlanInit==0)
        return RT_ERR_RG_NOT_INIT;

    //check input parameter
    if(stormInfo == NULL|| stormInfo_idx==NULL)
        return RT_ERR_RG_NULL_POINTER;

	if(stormInfo->valid==RTK_RG_DISABLED)
		return RT_ERR_RG_INVALID_PARAM;
	if(stormInfo->port >= RTK_RG_MAC_PORT_MAX)
        return RT_ERR_RG_INVALID_PARAM;
	if(stormInfo->meterIdx >= MAX_STORMCONTROL_ENTRY_SIZE)
		return RT_ERR_RG_INVALID_PARAM;
	if(stormInfo->stormType >= RTK_RG_STORM_TYPE_END)
		return RT_ERR_RG_INVALID_PARAM;
	

	ASSERT_EQ(_rtk_rg_stormControl_duplex_check(stormInfo),RT_ERR_RG_OK);
	ASSERT_EQ(_rtk_rg_stormControl_search_empty(stormInfo_idx),RT_ERR_RG_OK);

	bzero(&stormTypeEnable,sizeof(rtk_rate_storm_group_ctrl_t));
	switch(stormInfo->stormType){
		case RTK_RG_STORM_TYPE_UNKNOWN_UNICAST:  
			assert_ok(rtk_rate_stormControlEnable_get(&stormTypeEnable));
			stormTypeEnable.unknown_unicast_enable=RTK_RG_ENABLED; 
			break;
	    case RTK_RG_STORM_TYPE_UNKNOWN_MULTICAST: 
			assert_ok(rtk_rate_stormControlEnable_get(&stormTypeEnable));
			stormTypeEnable.unknown_multicast_enable=RTK_RG_ENABLED; 
			break;
	    case RTK_RG_STORM_TYPE_MULTICAST: 
			assert_ok(rtk_rate_stormControlEnable_get(&stormTypeEnable));
			stormTypeEnable.multicast_enable=RTK_RG_ENABLED; 
			break;
	    case RTK_RG_STORM_TYPE_BROADCAST: 
			assert_ok(rtk_rate_stormControlEnable_get(&stormTypeEnable));
			stormTypeEnable.broadcast_enable=RTK_RG_ENABLED; 
			break;
	    case RTK_RG_STORM_TYPE_DHCP: 
			assert_ok(rtk_rate_stormControlEnable_get(&stormTypeEnable));
			stormTypeEnable.dhcp_enable=RTK_RG_ENABLED; 
			break;
	    case RTK_RG_STORM_TYPE_ARP: 
			assert_ok(rtk_rate_stormControlEnable_get(&stormTypeEnable));
			stormTypeEnable.arp_enable=RTK_RG_ENABLED; 
			break;
	    case RTK_RG_STORM_TYPE_IGMP_MLD: 
			assert_ok(rtk_rate_stormControlEnable_get(&stormTypeEnable));
			stormTypeEnable.igmp_mld_enable=RTK_RG_ENABLED; 
			break;
		default:break;
	}

	ret=rtk_rate_stormControlEnable_set(&stormTypeEnable);	
	if(ret!=RT_ERR_OK){
		//rtlglue_printf("4 different types are supported at most!");
		return RT_ERR_RG_STORMCONTROL_TYPE_FULL;
	}
	assert_ok(rtk_rate_stormControlPortEnable_set(stormInfo->port,stormInfo->stormType,ENABLED));
	assert_ok(rtk_rate_stormControlMeterIdx_set(stormInfo->port,stormInfo->stormType,stormInfo->meterIdx));

	//maintain in FwdEngine
	memcpy(&rg_db.systemGlobal.stormControlInfoEntry[*stormInfo_idx],stormInfo,sizeof(rtk_rg_stormControlInfo_t));

	
	return RT_ERR_RG_OK;
}
int32 rtk_rg_stormControl_del(int stormInfo_idx)
{
	int i;

	rtk_rate_storm_group_ctrl_t stormTypeEnable;

	if(stormInfo_idx >= MAX_STORMCONTROL_ENTRY_SIZE || MAX_STORMCONTROL_ENTRY_SIZE < 0)
		return RT_ERR_RG_INVALID_PARAM;
	

	if(rg_db.systemGlobal.stormControlInfoEntry[stormInfo_idx].valid==RTK_RG_DISABLED){
		bzero(&rg_db.systemGlobal.stormControlInfoEntry[stormInfo_idx],sizeof(rtk_rg_stormControlInfo_t));
		return RT_ERR_RG_OK;
	}else{	
		assert_ok(rtk_rate_stormControlPortEnable_set(rg_db.systemGlobal.stormControlInfoEntry[stormInfo_idx].port,rg_db.systemGlobal.stormControlInfoEntry[stormInfo_idx].stormType,DISABLED));
		assert_ok(rtk_rate_stormControlMeterIdx_set(rg_db.systemGlobal.stormControlInfoEntry[stormInfo_idx].port,rg_db.systemGlobal.stormControlInfoEntry[stormInfo_idx].stormType,0));
		//check if other rule use same stormType
		for(i=0;i<MAX_STORMCONTROL_ENTRY_SIZE;i++){
			if(i==stormInfo_idx){
				continue;
			}else{
				if(rg_db.systemGlobal.stormControlInfoEntry[i].stormType==rg_db.systemGlobal.stormControlInfoEntry[stormInfo_idx].stormType)
					break;
			}
		}
	}

	if(i==MAX_STORMCONTROL_ENTRY_SIZE){ //no other rule use same stormType, clear it.
		bzero(&stormTypeEnable,sizeof(rtk_rate_storm_group_ctrl_t));
		switch(rg_db.systemGlobal.stormControlInfoEntry[stormInfo_idx].stormType){
			case RTK_RG_STORM_TYPE_UNKNOWN_UNICAST:
				assert_ok(rtk_rate_stormControlEnable_get(&stormTypeEnable));
				stormTypeEnable.unknown_unicast_enable=RTK_RG_DISABLED; 
				break;
			case RTK_RG_STORM_TYPE_UNKNOWN_MULTICAST: 
				assert_ok(rtk_rate_stormControlEnable_get(&stormTypeEnable));
				stormTypeEnable.unknown_multicast_enable=RTK_RG_DISABLED; 
				break;
			case RTK_RG_STORM_TYPE_MULTICAST:
				assert_ok(rtk_rate_stormControlEnable_get(&stormTypeEnable));
				stormTypeEnable.multicast_enable=RTK_RG_DISABLED; 
				break;
			case RTK_RG_STORM_TYPE_BROADCAST:
				assert_ok(rtk_rate_stormControlEnable_get(&stormTypeEnable));
				stormTypeEnable.broadcast_enable=RTK_RG_DISABLED; 
				break;
			case RTK_RG_STORM_TYPE_DHCP:
				assert_ok(rtk_rate_stormControlEnable_get(&stormTypeEnable));
				stormTypeEnable.dhcp_enable=RTK_RG_DISABLED; 
				break;
			case RTK_RG_STORM_TYPE_ARP:
				assert_ok(rtk_rate_stormControlEnable_get(&stormTypeEnable));
				stormTypeEnable.arp_enable=RTK_RG_DISABLED; 
				break;
			case RTK_RG_STORM_TYPE_IGMP_MLD:
				assert_ok(rtk_rate_stormControlEnable_get(&stormTypeEnable));
				stormTypeEnable.igmp_mld_enable=RTK_RG_DISABLED; 
				break;
			default:break;
		}

		assert_ok(rtk_rate_stormControlEnable_set(&stormTypeEnable));
		
	}

	//clear stormInfo in fwdEngine
	bzero(&rg_db.systemGlobal.stormControlInfoEntry[stormInfo_idx],sizeof(rtk_rg_stormControlInfo_t));
	
	return RT_ERR_RG_OK;
}
int32 rtk_rg_stormControl_find(rtk_rg_stormControlInfo_t *stormInfo,int *stormInfo_idx)
{
	int i;
	   //check input parameter
    if(stormInfo == NULL|| stormInfo_idx==NULL)
        return RT_ERR_RG_NULL_POINTER;
	if(*stormInfo_idx >= MAX_STORMCONTROL_ENTRY_SIZE || *stormInfo_idx < 0)
		return RT_ERR_RG_INVALID_PARAM;

	
	for(i=(*stormInfo_idx);i<MAX_STORMCONTROL_ENTRY_SIZE;i++){
		if(rg_db.systemGlobal.stormControlInfoEntry[i].valid==RTK_RG_ENABLED){
			stormInfo->valid = RTK_RG_ENABLED;
			stormInfo->port = rg_db.systemGlobal.stormControlInfoEntry[i].port;
			stormInfo->stormType = rg_db.systemGlobal.stormControlInfoEntry[i].stormType;
			stormInfo->meterIdx = rg_db.systemGlobal.stormControlInfoEntry[i].meterIdx;
			*stormInfo_idx = i;
			break;
		}
	}
	if(i==MAX_STORMCONTROL_ENTRY_SIZE){
		*stormInfo_idx = -1;
		return RT_ERR_RG_STORMCONTROL_ENTRY_NOT_FOUND;
	}
		
	return RT_ERR_RG_OK;
}

int32 rtk_rg_shareMeter_set(uint32 index, uint32 rate, rtk_rg_enable_t ifgInclude){
	int ret;

	ret = rtk_rate_shareMeter_set(index, rate, ifgInclude);

	if(ret==RT_ERR_OK){ return RT_ERR_RG_OK;}
	else if(ret == RT_ERR_FAILED){ return RT_ERR_RG_SHAREMETER_SET_FAILED;}
	else if(ret == RT_ERR_FILTER_METER_ID){ return RT_ERR_RG_SHAREMETER_INVALID_METER_INDEX;}
	else if(ret == RT_ERR_RATE){ return RT_ERR_RG_SHAREMETER_INVALID_RATE;}
	else if(ret == RT_ERR_INPUT){ return RT_ERR_RG_SHAREMETER_INVALID_INPUT;}
	else{  return RT_ERR_RG_SHAREMETER_SET_FAILED;}

}

int32 rtk_rg_shareMeter_get(uint32 index, uint32 *pRate , rtk_rg_enable_t *pIfgInclude){
	int ret;
	ret =rtk_rate_shareMeter_get(index, pRate , (void*)pIfgInclude);
	if(ret==RT_ERR_OK){ return RT_ERR_RG_OK;}
	else if(ret == RT_ERR_FILTER_METER_ID){ return RT_ERR_RG_SHAREMETER_INVALID_METER_INDEX;}
	else {return RT_ERR_RG_SHAREMETER_GET_FAILED;}

	return RT_ERR_RG_OK;
}


unsigned int _rtk_rg_hash_mac_fid_efid(unsigned char *mac,unsigned int fid,unsigned int efid)
{
	unsigned char hashidx[9]={0};
/*
	hashidx[0]=((mac[5]>>0)&1)^((mac[4]>>1)&1)^((mac[3]>>2)&1)^((mac[2]>>3)&1)^((mac[1]>>4)&1)^((mac[0]>>5)&1)^0;
	hashidx[1]=((mac[5]>>1)&1)^((mac[4]>>2)&1)^((mac[3]>>3)&1)^((mac[2]>>4)&1)^((mac[1]>>5)&1)^((mac[0]>>6)&1)^0;	
	hashidx[2]=((mac[5]>>2)&1)^((mac[4]>>3)&1)^((mac[3]>>4)&1)^((mac[2]>>5)&1)^((mac[1]>>6)&1)^((mac[0]>>7)&1)^0;
	hashidx[3]=((mac[5]>>3)&1)^((mac[4]>>4)&1)^((mac[3]>>5)&1)^((mac[2]>>6)&1)^((mac[1]>>7)&1)^((fid>>0)&1)^0;
	hashidx[4]=((mac[5]>>4)&1)^((mac[4]>>5)&1)^((mac[3]>>6)&1)^((mac[2]>>7)&1)^((mac[0]>>0)&1)^((fid>>1)&1)^0;	
	hashidx[5]=((mac[5]>>5)&1)^((mac[4]>>6)&1)^((mac[3]>>7)&1)^((mac[1]>>0)&1)^((mac[0]>>1)&1)^((fid>>2)&1)^0;		
	hashidx[6]=((mac[5]>>6)&1)^((mac[4]>>7)&1)^((mac[2]>>0)&1)^((mac[1]>>1)&1)^((mac[0]>>2)&1)^((fid>>3)&1)^((efid>>0)&1);			
	hashidx[7]=((mac[5]>>7)&1)^((mac[3]>>0)&1)^((mac[2]>>1)&1)^((mac[1]>>2)&1)^((mac[0]>>3)&1)^0^((efid>>1)&1);			
	hashidx[8]=((mac[4]>>0)&1)^((mac[3]>>1)&1)^((mac[2]>>2)&1)^((mac[1]>>3)&1)^((mac[0]>>4)&1)^0^((efid>>2)&1);
	return ((hashidx[8]<<8)|(hashidx[7]<<7)|(hashidx[6]<<6)|(hashidx[5]<<5)|(hashidx[4]<<4)|(hashidx[3]<<3)|(hashidx[2]<<2)|(hashidx[1]<<1)|(hashidx[0]));
*/
	hashidx[0]=(mac[5]^(mac[4]>>1)^(mac[3]>>2)^(mac[2]>>3)^(mac[1]>>4)^(mac[0]>>5)^0)&1;
	hashidx[1]=((mac[5]>>1)^(mac[4]>>2)^(mac[3]>>3)^(mac[2]>>4)^(mac[1]>>5)^(mac[0]>>6)^0)&1;	
	hashidx[2]=((mac[5]>>2)^(mac[4]>>3)^(mac[3]>>4)^(mac[2]>>5)^(mac[1]>>6)^(mac[0]>>7)^0)&1;
	hashidx[3]=((mac[5]>>3)^(mac[4]>>4)^(mac[3]>>5)^(mac[2]>>6)^(mac[1]>>7)^fid^0)&1;
	hashidx[4]=((mac[5]>>4)^(mac[4]>>5)^(mac[3]>>6)^(mac[2]>>7)^mac[0]^(fid>>1)^0)&1;	
	hashidx[5]=((mac[5]>>5)^(mac[4]>>6)^(mac[3]>>7)^mac[1]^(mac[0]>>1)^(fid>>2)^0)&1;		
	hashidx[6]=((mac[5]>>6)^(mac[4]>>7)^mac[2]^(mac[1]>>1)^(mac[0]>>2)^(fid>>3)^(efid))&1;			
	hashidx[7]=((mac[5]>>7)^mac[3]^(mac[2]>>1)^(mac[1]>>2)^(mac[0]>>3)^0^(efid>>1))&1;			
	hashidx[8]=(mac[4]^(mac[3]>>1)^(mac[2]>>2)^(mac[1]>>3)^(mac[0]>>4)^0^(efid>>2))&1;
	return ((hashidx[8]<<8)|(hashidx[7]<<7)|(hashidx[6]<<6)|(hashidx[5]<<5)|(hashidx[4]<<4)|(hashidx[3]<<3)|(hashidx[2]<<2)|(hashidx[1]<<1)|(hashidx[0]));	
}

unsigned int _rtk_rg_hash_mac_vid_efid(unsigned char *mac,unsigned int vid,unsigned int efid)
{
	unsigned char hashidx[9]={0};
	/*hashidx[0]=(mac[5]&1)^((mac[4]>>1)&1)^((mac[3]>>2)&1)^((mac[2]>>3)&1)^((mac[1]>>4)&1)^((mac[0]>>5)&1)^((vid>>6)&1);
	hashidx[1]=((mac[5]>>1)&1)^((mac[4]>>2)&1)^((mac[3]>>3)&1)^((mac[2]>>4)&1)^((mac[1]>>5)&1)^((mac[0]>>6)&1)^((vid>>7)&1);	
	hashidx[2]=((mac[5]>>2)&1)^((mac[4]>>3)&1)^((mac[3]>>4)&1)^((mac[2]>>5)&1)^((mac[1]>>6)&1)^((mac[0]>>7)&1)^((vid>>8)&1);
	hashidx[3]=((mac[5]>>3)&1)^((mac[4]>>4)&1)^((mac[3]>>5)&1)^((mac[2]>>6)&1)^((mac[1]>>7)&1)^(vid&1)^((vid>>9)&1);
	hashidx[4]=((mac[5]>>4)&1)^((mac[4]>>5)&1)^((mac[3]>>6)&1)^((mac[2]>>7)&1)^(mac[0]&1)^((vid>>1)&1)^((vid>>10)&1);	
	hashidx[5]=((mac[5]>>5)&1)^((mac[4]>>6)&1)^((mac[3]>>7)&1)^(mac[1]&1)^((mac[0]>>1)&1)^((vid>>2)&1)^((vid>>11)&1);		
	hashidx[6]=((mac[5]>>6)&1)^((mac[4]>>7)&1)^(mac[2]&1)^((mac[1]>>1)&1)^((mac[0]>>2)&1)^((vid>>3)&1)^(efid&1);			
	hashidx[7]=((mac[5]>>7)&1)^(mac[3]&1)^((mac[2]>>1)&1)^((mac[1]>>2)&1)^((mac[0]>>3)&1)^((vid>>4)&1)^((efid>>1)&1);			
	hashidx[8]=(mac[4]&1)^((mac[3]>>1)&1)^((mac[2]>>2)&1)^((mac[1]>>3)&1)^((mac[0]>>4)&1)^((vid>>5)&1)^((efid>>2)&1);*/
	hashidx[0]=((mac[5])^(mac[4]>>1)^(mac[3]>>2)^(mac[2]>>3)^(mac[1]>>4)^(mac[0]>>5)^(vid>>6))&1;
	hashidx[1]=((mac[5]>>1)^(mac[4]>>2)^(mac[3]>>3)^(mac[2]>>4)^(mac[1]>>5)^(mac[0]>>6)^(vid>>7))&1;
	hashidx[2]=((mac[5]>>2)^(mac[4]>>3)^(mac[3]>>4)^(mac[2]>>5)^(mac[1]>>6)^(mac[0]>>7)^(vid>>8))&1;
	hashidx[3]=((mac[5]>>3)^(mac[4]>>4)^(mac[3]>>5)^(mac[2]>>6)^(mac[1]>>7)^vid^(vid>>9))&1;
	hashidx[4]=((mac[5]>>4)^(mac[4]>>5)^(mac[3]>>6)^(mac[2]>>7)^mac[0]^(vid>>1)^(vid>>10))&1;	
	hashidx[5]=((mac[5]>>5)^(mac[4]>>6)^(mac[3]>>7)^mac[1]^(mac[0]>>1)^(vid>>2)^(vid>>11))&1;		
	hashidx[6]=((mac[5]>>6)^(mac[4]>>7)^mac[2]^(mac[1]>>1)^(mac[0]>>2)^(vid>>3)^efid)&1;			
	hashidx[7]=((mac[5]>>7)^mac[3]^(mac[2]>>1)^(mac[1]>>2)^(mac[0]>>3)^(vid>>4)^(efid>>1))&1;			
	hashidx[8]=(mac[4]^(mac[3]>>1)^(mac[2]>>2)^(mac[1]>>3)^(mac[0]>>4)^(vid>>5)^(efid>>2))&1;
	return ((hashidx[8]<<8)|(hashidx[7]<<7)|(hashidx[6]<<6)|(hashidx[5]<<5)|(hashidx[4]<<4)|(hashidx[3]<<3)|(hashidx[2]<<2)|(hashidx[1]<<1)|(hashidx[0]));
}


unsigned int _rtk_rg_hash_sip_gip(unsigned int sip,unsigned int gip)
{
	u8 hashidx[9]={0};
	hashidx[8]=((gip>>8)^(gip>>17)^(gip>>26)^(sip>>7)^(sip>>16)^(sip>>25))&1;
	hashidx[7]=((gip>>7)^(gip>>16)^(gip>>25)^(sip>>6)^(sip>>15)^(sip>>24))&1;
	hashidx[6]=((gip>>6)^(gip>>15)^(gip>>24)^(sip>>5)^(sip>>14)^(sip>>23))&1;	
	hashidx[5]=((gip>>5)^(gip>>14)^(gip>>23)^(sip>>4)^(sip>>13)^(sip>>22)^(sip>>31))&1;
	hashidx[4]=((gip>>4)^(gip>>13)^(gip>>22)^(sip>>3)^(sip>>12)^(sip>>21)^(sip>>30))&1;	
	hashidx[3]=((gip>>3)^(gip>>12)^(gip>>21)^(sip>>2)^(sip>>11)^(sip>>20)^(sip>>29))&1;
	hashidx[2]=((gip>>2)^(gip>>11)^(gip>>20)^(sip>>1)^(sip>>10)^(sip>>19)^(sip>>28))&1;		
	hashidx[1]=((gip>>1)^(gip>>10)^(gip>>19)^(sip>>0)^(sip>>9)^(sip>>18)^(sip>>27))&1;
	hashidx[0]=((gip>>0)^(gip>>9)^(gip>>18)^(gip>>27)^(sip>>8)^(sip>>17)^(sip>>26))&1;	
	return ((hashidx[8]<<8)|(hashidx[7]<<7)|(hashidx[6]<<6)|(hashidx[5]<<5)|(hashidx[4]<<4)|(hashidx[3]<<3)|(hashidx[2]<<2)|(hashidx[1]<<1)|(hashidx[0]<<0));
}


//Source Address Learning Limit and Action Functions
int32 rtk_rg_softwareSourceAddrLearningLimit_set(rtk_rg_saLearningLimitInfo_t sa_learnLimit_info, rtk_rg_port_idx_t port_idx)
{
	//Check param
	if(port_idx < RTK_RG_PORT0 || port_idx >= RTK_RG_PORT_MAX)
		return RT_ERR_RG_INVALID_PARAM;
	if(sa_learnLimit_info.learningLimitNumber<0 || sa_learnLimit_info.learningLimitNumber>MAX_LUT_SW_TABLE_SIZE)
		return RT_ERR_RG_INVALID_PARAM;
	if(sa_learnLimit_info.action>=SA_LEARN_EXCEED_ACTION_END)
		return RT_ERR_RG_INVALID_PARAM;

	//Set number and action in rg_db
	rg_db.systemGlobal.sourceAddrLearningLimitNumber[port_idx]=sa_learnLimit_info.learningLimitNumber;
	rg_db.systemGlobal.sourceAddrLearningAction[port_idx]=sa_learnLimit_info.action;

	return RT_ERR_RG_OK;
}

int32 rtk_rg_softwareSourceAddrLearningLimit_get(rtk_rg_saLearningLimitInfo_t *sa_learnLimit_info, rtk_rg_port_idx_t port_idx)
{
	//Check param
	if(port_idx < RTK_RG_PORT0 || port_idx > RTK_RG_PORT_MAX)
		return RT_ERR_RG_INVALID_PARAM;

	//Get number and action in rg_db
	sa_learnLimit_info->learningLimitNumber=rg_db.systemGlobal.sourceAddrLearningLimitNumber[port_idx];
	sa_learnLimit_info->action=rg_db.systemGlobal.sourceAddrLearningAction[port_idx];

	return RT_ERR_RG_OK;
}

int32  rtk_rg_qos_schedulingQueue_set(rtk_rg_mac_port_idx_t port_idx,rtk_rg_qos_queue_weights_t q_weight){

	int i,ret;
	rtk_qos_queue_weights_t rtk_q_weight;
	rtk_ponmac_queue_t rtk_pon_q;
	rtk_ponmac_queueCfg_t rtk_pon_q_cfg;
	bzero(&rtk_q_weight,sizeof(rtk_qos_queue_weights_t));
	bzero(&rtk_pon_q,sizeof(rtk_ponmac_queue_t));
	bzero(&rtk_pon_q_cfg,sizeof(rtk_ponmac_queueCfg_t));

	for(i=0;i<RTK_RG_MAX_NUM_OF_QUEUE;i++)
		rtk_q_weight.weights[i]=q_weight.weights[i];

	//Exclude IFG
	ret=rtk_rate_egrBandwidthCtrlIncludeIfg_set(DISABLED);
	DEBUG("ret=%x\n",ret);
	if(ret!=RT_ERR_OK)
		return RT_ERR_RG_FAILED;

	if(port_idx==RTK_RG_MAC_PORT_PON)
	{
		int qid=0;
		for(qid=0;qid<RTK_RG_MAX_NUM_OF_QUEUE;qid++)
		{
			//Before set, just get(unknown problem)
			rtk_pon_q.queueId=qid;
			rtk_pon_q.schedulerId=0;
			rtk_ponmac_queue_get(&rtk_pon_q, &rtk_pon_q_cfg);

			rtk_pon_q_cfg.cir=0;
			rtk_pon_q_cfg.pir=0x1ffff;
			if(rtk_q_weight.weights[qid])
				rtk_pon_q_cfg.type=WFQ_WRR_PRIORITY;
			else
				rtk_pon_q_cfg.type=STRICT_PRIORITY;
			rtk_pon_q_cfg.weight=rtk_q_weight.weights[qid];
			ret=rtk_ponmac_queue_add(&rtk_pon_q, &rtk_pon_q_cfg);
			DEBUG("ret=%x\n",ret);
			if(ret!=RT_ERR_OK)
				return RT_ERR_RG_INVALID_PARAM;
		}
	}
	else
	{
		ret=rtk_qos_schedulingQueue_set(port_idx,&rtk_q_weight);
		DEBUG("ret=%x\n",ret);
		if(ret!=RT_ERR_OK)
			return RT_ERR_RG_INVALID_PARAM;
	}

	return RT_ERR_RG_OK;

}

int32  rtk_rg_qos_schedulingQueue_get(rtk_rg_mac_port_idx_t port_idx,rtk_rg_qos_queue_weights_t *q_weight){

	int i,ret;
	rtk_qos_queue_weights_t rtk_q_weight;
	rtk_ponmac_queue_t rtk_pon_q;
	rtk_ponmac_queueCfg_t rtk_pon_q_cfg;
	bzero(&rtk_q_weight,sizeof(rtk_qos_queue_weights_t));
	bzero(&rtk_pon_q,sizeof(rtk_ponmac_queue_t));
	bzero(&rtk_pon_q_cfg,sizeof(rtk_ponmac_queueCfg_t));

	if(q_weight==NULL)
		return RT_ERR_RG_NULL_POINTER;

	if(port_idx==RTK_RG_MAC_PORT_PON)
	{
		int qid=0;
		for(qid=0;qid<RTK_RG_MAX_NUM_OF_QUEUE;qid++)
		{
			//Before set, just get(unknown problem)
			rtk_pon_q.queueId=qid;
			rtk_pon_q.schedulerId=0;
			ret=rtk_ponmac_queue_get(&rtk_pon_q, &rtk_pon_q_cfg);
			if(ret!=RT_ERR_OK)
				return RT_ERR_RG_INVALID_PARAM;
			if(rtk_pon_q_cfg.type==STRICT_PRIORITY)
				rtk_q_weight.weights[qid] = 0;
			else
				rtk_q_weight.weights[qid] = rtk_pon_q_cfg.weight;
		}
	}
	else
	{
		ret=rtk_qos_schedulingQueue_get(port_idx, &rtk_q_weight);
		if(ret!=RT_ERR_OK)
			return RT_ERR_RG_INVALID_PARAM;
	}
	
	//memcpy(q_weight->weights,rtk_q_weight.weights,sizeof(uint32)*RTK_RG_MAX_NUM_OF_QUEUE);

	for(i=0;i<RTK_RG_MAX_NUM_OF_QUEUE;i++)
		q_weight->weights[i]=rtk_q_weight.weights[i];
	

	return RT_ERR_RG_OK;
}



/* ============================
	   DoS Port Security
   ============================ */

int dosThresholdMapping[]={1,2,3,4,6,7,8,9,10,11,12,13,14,16,17,18,19,20,21,22,23,24,26,27,28,29,30,31,32,33,34,36,37,38,39,40,41,42,43,44,
							45,46,47,48,50,51,52,53,54,55,56,57,58,59,61,62,63,64,65,66,67,68,69,70,71,72,74,75,76,77,78,90,80,81,82,84,85,86,87,88,
							89,90,91,92,94,95,96,97,98,99,100,101,102,104,105,106,107,108,109,110,111,112,114,115,116,117,118,119,120,121,122,124,
							125,126,127,128,129,130,131,132,134,135,136,137,138,139,140,141,142,144,145,146,147,148,
							149,150,151,152,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,
							173,174,175,176,177,178,179,180,181,183,184,185,186,187,188,189,190,191,193,194,195,196,
							197,198,199,200,201,203,204,205,206,207,208,209,210,211,213,214,215,216,217,218,219,220,
							221,223,224,225,226,227,228,229,230,231,233,234,235,236,237,238,239,240,241,242,243,244,
							245,246,247,248,249,250,253,253,254,255};

int rtk_rg_dosPortMaskEnable_set(rtk_rg_mac_portmask_t dos_port_mask)
{
	int ret;
	int i;

	//Configure security ports
	for(i=0;i<RTK_RG_MAC_PORT_MAX;i++)
	{
		if(dos_port_mask.portmask & (0x1<<i))
			ret = rtk_sec_portAttackPreventState_set(i,ENABLED);
		else
			ret = rtk_sec_portAttackPreventState_set(i,DISABLED);
	}

	assert_ok(ret);

	return RT_ERR_RG_OK;
}

int rtk_rg_dosPortMaskEnable_get(rtk_rg_mac_portmask_t *dos_port_mask)
{
	int ret;
	int i;
	rtk_enable_t dos_enabled;

	if(dos_port_mask==NULL) return RT_ERR_RG_NULL_POINTER;
	

	//Get security port state
	dos_port_mask->portmask=0;
	for(i=0;i<RTK_RG_MAC_PORT_MAX;i++)
	{
		ret = rtk_sec_portAttackPreventState_get(i,&dos_enabled);
		assert_ok(ret);
		if(dos_enabled) dos_port_mask->portmask|=(0x1<<i);
		else dos_port_mask->portmask&=~(0x1<<i);
	}

	return RT_ERR_RG_OK;
}

int _rtk_rg_dosType_set(rtk_rg_dos_type_t dos_type,int dos_enabled,rtk_rg_dos_action_t dos_action)
{
	int ret;
	int enabled;
	rtk_rg_dos_action_t act;

	/* RG DoS Action:
	    RTK_RG_DOS_ACTION_DROP = 0,
    	RTK_RG_DOS_ACTION_TRAP,
    */
    int dosActionMapping[]={
		ACTION_DROP,
		ACTION_TRAP2CPU
	};
    
	/* RG DoS type:
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
	*/
	int dosTypeMapping[]={
		DAEQSA_DENY,
		LAND_DENY,
		BLAT_DENY,
		SYNFIN_DENY,
		XMA_DENY,
		NULLSCAN_DENY,
		SYN_SPORTL1024_DENY,
		TCPHDR_MIN_CHECK,
		TCP_FRAG_OFF_MIN_CHECK,
		ICMP_FRAG_PKTS_DENY,
		POD_DENY,
		UDPDOMB_DENY,
		SYNWITHDATA_DENY,
		SYNFLOOD_DENY,
		FINFLOOD_DENY,
		ICMPFLOOD_DENY
	};

	ret = rtk_rg_dosType_get(dos_type,&enabled,&act);
	if(dos_enabled!=-1) enabled=dos_enabled;
	if(dos_action!=-1) act=dos_action;

	//Configure DoS action
	if(enabled)
		ret = rtk_sec_attackPrevent_set(dosTypeMapping[dos_type],dosActionMapping[act]);
	else
		ret = rtk_sec_attackPrevent_set(dosTypeMapping[dos_type],ACTION_FORWARD);

	assert_ok(ret);
	
	return RT_ERR_RG_OK;
}

int rtk_rg_dosType_set(rtk_rg_dos_type_t dos_type,int dos_enabled,rtk_rg_dos_action_t dos_action)
{
	int ret;

	//Check parameters
	if((dos_type<DAEQSA_DENY) || (dos_type>RTK_RG_DOS_SYNWITHDATA_DENY))
		return RT_ERR_RG_INVALID_PARAM;

	if((dos_enabled!=1) && (dos_enabled!=0) && (dos_enabled!=-1))
		return RT_ERR_RG_INVALID_PARAM;

	if((dos_action!=RTK_RG_DOS_ACTION_DROP) && (dos_action!=RTK_RG_DOS_ACTION_TRAP) && (dos_action!=-1))
		return RT_ERR_RG_INVALID_PARAM;

	ret = _rtk_rg_dosType_set(dos_type,dos_enabled,dos_action);

	return RT_ERR_RG_OK;
}

int _rtk_rg_dosType_get(rtk_rg_dos_type_t dos_type,int *dos_enabled,rtk_rg_dos_action_t *dos_action)
{
	int ret;
	rtk_action_t act;

	/* RG DoS type:
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
	*/
	int dosTypeMapping[]={
		DAEQSA_DENY,
		LAND_DENY,
		BLAT_DENY,
		SYNFIN_DENY,
		XMA_DENY,
		NULLSCAN_DENY,
		SYN_SPORTL1024_DENY,
		TCPHDR_MIN_CHECK,
		TCP_FRAG_OFF_MIN_CHECK,
		ICMP_FRAG_PKTS_DENY,
		POD_DENY,
		UDPDOMB_DENY,
		SYNWITHDATA_DENY,
		SYNFLOOD_DENY,
		FINFLOOD_DENY,
		ICMPFLOOD_DENY
	};
	
	//Get DoS action
	ret = rtk_sec_attackPrevent_get(dosTypeMapping[dos_type],&act);
	assert_ok(ret);
	if(act==ACTION_FORWARD)
	{
		*dos_enabled=0;
		*dos_action=0;
	}
	else
	{
		*dos_enabled=1;
		*dos_action=(act==ACTION_DROP)?RTK_RG_DOS_ACTION_DROP:RTK_RG_DOS_ACTION_TRAP;
	}
	
	return RT_ERR_RG_OK;
}


int rtk_rg_dosType_get(rtk_rg_dos_type_t dos_type,int *dos_enabled,rtk_rg_dos_action_t *dos_action)
{
	int ret;

	if((dos_enabled==NULL) || (dos_action==NULL)) return RT_ERR_RG_NULL_POINTER;
	
	ret = _rtk_rg_dosType_get(dos_type,dos_enabled,dos_action);
	assert_ok(ret);
	
	return RT_ERR_RG_OK;
}


int rtk_rg_dosFloodType_set(rtk_rg_dos_type_t dos_type,int dos_enabled,rtk_rg_dos_action_t dos_action,int dos_threshold)
{
	int ret;
	int mapIdx;

	//Check parameters
	if((dos_type<RTK_RG_DOS_SYNFLOOD_DENY) || (dos_type>RTK_RG_DOS_ICMPFLOOD_DENY))
		return RT_ERR_RG_INVALID_PARAM;

	if((dos_enabled!=1) && (dos_enabled!=0) && (dos_enabled!=-1))
		return RT_ERR_RG_INVALID_PARAM;

	if((dos_action!=RTK_RG_DOS_ACTION_DROP) && (dos_action!=RTK_RG_DOS_ACTION_TRAP) && (dos_action!=-1))
		return RT_ERR_RG_INVALID_PARAM;

	if(((dos_threshold<1) || (dos_threshold>232)) && (dos_threshold!=-1))
		return RT_ERR_RG_INVALID_PARAM;

	ret = _rtk_rg_dosType_set(dos_type,dos_enabled,dos_action);
	assert_ok(ret);

	//Don't change threshold
	if(dos_threshold==-1) return RT_ERR_RG_OK;

	//Set DoS flooding attack protection threshold
	mapIdx=dos_threshold-1;
	if(dos_type==RTK_RG_DOS_SYNFLOOD_DENY)
		ret = rtk_sec_attackFloodThresh_set(SEC_SYNCFLOOD,dosThresholdMapping[mapIdx]);
	else if(dos_type==RTK_RG_DOS_FINFLOOD_DENY)
		ret = rtk_sec_attackFloodThresh_set(SEC_FINFLOOD,dosThresholdMapping[mapIdx]);
	else if(dos_type==RTK_RG_DOS_ICMPFLOOD_DENY)
		ret = rtk_sec_attackFloodThresh_set(SEC_ICMPFLOOD,dosThresholdMapping[mapIdx]);

	assert_ok(ret);

	return RT_ERR_RG_OK;
}

int rtk_rg_dosFloodType_get(rtk_rg_dos_type_t dos_type,int *dos_enabled,rtk_rg_dos_action_t *dos_action,int *dos_threshold)
{
	int ret;
	int i;
	int internal_threshold;

	if((dos_type<RTK_RG_DOS_SYNFLOOD_DENY) || (dos_type>RTK_RG_DOS_ICMPFLOOD_DENY))
		return RT_ERR_RG_INVALID_PARAM;

	if((dos_enabled==NULL) || (dos_action==NULL) || (dos_threshold==NULL)) 
		return RT_ERR_RG_NULL_POINTER;
	
	ret = _rtk_rg_dosType_get(dos_type,dos_enabled,dos_action);
	assert_ok(ret);

	//Get DoS threshold
	internal_threshold=0;
	if(dos_type==RTK_RG_DOS_SYNFLOOD_DENY)
		ret = rtk_sec_attackFloodThresh_get(SEC_SYNCFLOOD,&internal_threshold);
	else if(dos_type==RTK_RG_DOS_FINFLOOD_DENY)
		ret = rtk_sec_attackFloodThresh_get(SEC_FINFLOOD,&internal_threshold);
	else if(dos_type==RTK_RG_DOS_ICMPFLOOD_DENY)
		ret = rtk_sec_attackFloodThresh_get(SEC_ICMPFLOOD,&internal_threshold);
	else
		return RT_ERR_RG_OK;
	assert_ok(ret);

	//Recovery the setting value
	for(i=0;i<232;i++)
		if(internal_threshold==dosThresholdMapping[i]) *dos_threshold=i+1;
 
	return RT_ERR_RG_OK;
}

int rtk_rg_portMibInfo_get(rtk_rg_mac_port_idx_t port, rtk_rg_port_mib_info_t *mibInfo)
{
	int ret;
	rtk_stat_port_cntr_t cntr;
	
	//Check param
	if(port < RTK_RG_MAC_PORT0 || port > RTK_RG_MAC_PORT_MAX)
		return RT_ERR_RG_INVALID_PARAM;
	if(mibInfo==NULL) 
		return RT_ERR_RG_NULL_POINTER;

	//Get MIB data
	ret = rtk_stat_port_getAll( port,&cntr);
	assert_ok(ret);

	//Assign to RG data structure
	mibInfo->ifInOctets=cntr.ifInOctets;
	mibInfo->ifInUcastPkts=cntr.ifInUcastPkts;
	mibInfo->ifInMulticastPkts=cntr.ifInMulticastPkts;
	mibInfo->ifInBroadcastPkts=cntr.ifInBroadcastPkts;
	mibInfo->ifInDiscards=cntr.ifInDiscards;
	mibInfo->ifOutOctets=cntr.ifOutOctets;
	mibInfo->ifOutDiscards=cntr.ifOutDiscards;
	mibInfo->ifOutUcastPkts=cntr.ifOutUcastPkts;
	mibInfo->ifOutMulticastPkts=cntr.ifOutMulticastPkts;
	mibInfo->ifOutBrocastPkts=cntr.ifOutBrocastPkts;
	mibInfo->dot1dBasePortDelayExceededDiscards=cntr.dot1dBasePortDelayExceededDiscards;
	mibInfo->dot1dTpPortInDiscards=cntr.dot1dTpPortInDiscards;
	mibInfo->dot1dTpHcPortInDiscards=cntr.dot1dTpHcPortInDiscards;
	mibInfo->dot3InPauseFrames=cntr.dot3InPauseFrames;
	mibInfo->dot3OutPauseFrames=cntr.dot3OutPauseFrames;
//	mibInfo->dot3OutPauseOnFrames=cntr.dot3OutPauseOnFrames;
	mibInfo->dot3StatsAligmentErrors=cntr.dot3StatsAligmentErrors;
	mibInfo->dot3StatsFCSErrors=cntr.dot3StatsFCSErrors;
	mibInfo->dot3StatsSingleCollisionFrames=cntr.dot3StatsSingleCollisionFrames;
	mibInfo->dot3StatsMultipleCollisionFrames=cntr.dot3StatsMultipleCollisionFrames;
	mibInfo->dot3StatsDeferredTransmissions=cntr.dot3StatsDeferredTransmissions;
	mibInfo->dot3StatsLateCollisions=cntr.dot3StatsLateCollisions;
	mibInfo->dot3StatsExcessiveCollisions=cntr.dot3StatsExcessiveCollisions;
	mibInfo->dot3StatsFrameTooLongs=cntr.dot3StatsFrameTooLongs;
	mibInfo->dot3StatsSymbolErrors=cntr.dot3StatsSymbolErrors;
	mibInfo->dot3ControlInUnknownOpcodes=cntr.dot3ControlInUnknownOpcodes;
	mibInfo->etherStatsDropEvents=cntr.etherStatsDropEvents;
	mibInfo->etherStatsOctets=cntr.etherStatsOctets;
	mibInfo->etherStatsBcastPkts=cntr.etherStatsBcastPkts;
	mibInfo->etherStatsMcastPkts=cntr.etherStatsMcastPkts;
	mibInfo->etherStatsUndersizePkts=cntr.etherStatsUndersizePkts;
	mibInfo->etherStatsOversizePkts=cntr.etherStatsOversizePkts;
	mibInfo->etherStatsFragments=cntr.etherStatsFragments;
	mibInfo->etherStatsJabbers=cntr.etherStatsJabbers;
	mibInfo->etherStatsCollisions=cntr.etherStatsCollisions;
	mibInfo->etherStatsCRCAlignErrors=cntr.etherStatsCRCAlignErrors;
	mibInfo->etherStatsPkts64Octets=cntr.etherStatsPkts64Octets;
	mibInfo->etherStatsPkts65to127Octets=cntr.etherStatsPkts65to127Octets;
	mibInfo->etherStatsPkts128to255Octets=cntr.etherStatsPkts128to255Octets;
	mibInfo->etherStatsPkts256to511Octets=cntr.etherStatsPkts256to511Octets;
	mibInfo->etherStatsPkts512to1023Octets=cntr.etherStatsPkts512to1023Octets;
	mibInfo->etherStatsPkts1024to1518Octets=cntr.etherStatsPkts1024to1518Octets;
	mibInfo->etherStatsTxOctets=cntr.etherStatsTxOctets;
	mibInfo->etherStatsTxUndersizePkts=cntr.etherStatsTxUndersizePkts;
	mibInfo->etherStatsTxOversizePkts=cntr.etherStatsTxOversizePkts;
	mibInfo->etherStatsTxPkts64Octets=cntr.etherStatsTxPkts64Octets;
	mibInfo->etherStatsTxPkts65to127Octets=cntr.etherStatsTxPkts65to127Octets;
	mibInfo->etherStatsTxPkts128to255Octets=cntr.etherStatsTxPkts128to255Octets;
	mibInfo->etherStatsTxPkts256to511Octets=cntr.etherStatsTxPkts256to511Octets;
	mibInfo->etherStatsTxPkts512to1023Octets=cntr.etherStatsTxPkts512to1023Octets;
	mibInfo->etherStatsTxPkts1024to1518Octets=cntr.etherStatsTxPkts1024to1518Octets;
	mibInfo->etherStatsTxPkts1519toMaxOctets=cntr.etherStatsTxPkts1519toMaxOctets;
	mibInfo->etherStatsTxBcastPkts=cntr.etherStatsTxBcastPkts;
	mibInfo->etherStatsTxMcastPkts=cntr.etherStatsTxMcastPkts;
	mibInfo->etherStatsTxFragments=cntr.etherStatsTxFragments;
	mibInfo->etherStatsTxJabbers=cntr.etherStatsTxJabbers;
	mibInfo->etherStatsTxCRCAlignErrors=cntr.etherStatsTxCRCAlignErrors;
	mibInfo->etherStatsRxUndersizePkts=cntr.etherStatsRxUndersizePkts;
	mibInfo->etherStatsRxUndersizeDropPkts=cntr.etherStatsRxUndersizeDropPkts;
	mibInfo->etherStatsRxOversizePkts=cntr.etherStatsRxOversizePkts;
	mibInfo->etherStatsRxPkts64Octets=cntr.etherStatsRxPkts64Octets;
	mibInfo->etherStatsRxPkts65to127Octets=cntr.etherStatsRxPkts65to127Octets;
	mibInfo->etherStatsRxPkts128to255Octets=cntr.etherStatsRxPkts128to255Octets;
	mibInfo->etherStatsRxPkts256to511Octets=cntr.etherStatsRxPkts256to511Octets;
	mibInfo->etherStatsRxPkts512to1023Octets=cntr.etherStatsRxPkts512to1023Octets;
	mibInfo->etherStatsRxPkts1024to1518Octets=cntr.etherStatsRxPkts1024to1518Octets;
	mibInfo->etherStatsRxPkts1519toMaxOctets=cntr.etherStatsRxPkts1519toMaxOctets;
	mibInfo->inOampduPkts=cntr.inOampduPkts;
	mibInfo->outOampduPkts=cntr.outOampduPkts;

	return RT_ERR_RG_OK;
}

int rtk_rg_portMibInfo_clear(rtk_rg_mac_port_idx_t port)
{
	int ret;
	
	//Check param
	if(port < RTK_RG_MAC_PORT0 || port > RTK_RG_MAC_PORT_MAX)
		return RT_ERR_RG_INVALID_PARAM;

	ret = rtk_stat_port_reset(port);
	assert_ok(ret);

	return RT_ERR_RG_OK;
}


int32 rtk_rg_qosPriMap_set(rtk_rg_qos_pri2queue_t pri2qid){
	//set group[3]: internal<=>queue mapping. rg only use group[3]
	int i;
	rtk_qos_pri2queue_t rtk_pri2qid;
	for(i=0;i<RTK_MAX_NUM_OF_PRIORITY;i++){
		rtk_pri2qid.pri2queue[i]=pri2qid.pri2queue[i];
	}
	ASSERT_EQ(rtk_qos_priMap_set(3, &rtk_pri2qid),RT_ERR_OK);
	return RT_ERR_RG_OK;
}

int32 rtk_rg_qosPriMap_get(rtk_rg_qos_pri2queue_t *pPri2qid){
	int i;
	rtk_qos_pri2queue_t pri2qid;
	bzero(&pri2qid,sizeof(pri2qid));
	ASSERT_EQ(rtk_qos_priMap_get(3, &pri2qid),RT_ERR_OK);
	for(i=0;i<RTK_MAX_NUM_OF_PRIORITY;i++){
		pPri2qid->pri2queue[i]=pri2qid.pri2queue[i];
	}
	return RT_ERR_RG_OK;
}

int32 rtk_rg_qosPriSelGroup_set(rtk_rg_qos_priSelWeight_t weightOfPriSel){
	rtk_qos_priSelWeight_t weight;
	memset(&weight,0,sizeof(weight));


	
    weight.weight_of_portBased=weightOfPriSel.weight_of_portBased;
    weight.weight_of_dot1q=weightOfPriSel.weight_of_dot1q;
	weight.weight_of_dscp=weightOfPriSel.weight_of_dscp; 
    weight.weight_of_acl=weightOfPriSel.weight_of_acl;
	weight.weight_of_lutFwd=weightOfPriSel.weight_of_lutFwd;
    weight.weight_of_saBaed=weightOfPriSel.weight_of_saBaed;
    weight.weight_of_vlanBased=weightOfPriSel.weight_of_vlanBased;
    weight.weight_of_svlanBased=weightOfPriSel.weight_of_svlanBased;
	weight.weight_of_l4Based=weightOfPriSel.weight_of_l4Based;
	ASSERT_EQ(rtk_qos_priSelGroup_set(0, &weight),RT_ERR_OK);
	ASSERT_EQ(rtk_qos_priSelGroup_set(1, &weight),RT_ERR_OK);
	return RT_ERR_RG_OK;
}

int32 rtk_rg_qosPriSelGroup_get(rtk_rg_qos_priSelWeight_t *pWeightOfPriSel){
	rtk_qos_priSelWeight_t weight;
	memset(&weight,0,sizeof(weight));
	ASSERT_EQ(rtk_qos_priSelGroup_get(0, &weight),RT_ERR_OK);
    pWeightOfPriSel->weight_of_portBased=weight.weight_of_portBased;
    pWeightOfPriSel->weight_of_dot1q=weight.weight_of_dot1q;
	pWeightOfPriSel->weight_of_dscp=weight.weight_of_dscp; 
    pWeightOfPriSel->weight_of_acl=weight.weight_of_acl;
	pWeightOfPriSel->weight_of_lutFwd=weight.weight_of_lutFwd;
    pWeightOfPriSel->weight_of_lutFwd=weight.weight_of_saBaed;
    pWeightOfPriSel->weight_of_vlanBased=weight.weight_of_vlanBased;
    pWeightOfPriSel->weight_of_svlanBased=weight.weight_of_svlanBased;
	pWeightOfPriSel->weight_of_l4Based=weight.weight_of_l4Based;

	return RT_ERR_RG_OK;
}

int32 rtk_rg_qosDscpPriRemapGroup_set(uint32 dscp,uint32 intPri){

#ifdef CONFIG_DUALBAND_CONCURRENT	
	/*internal-priority CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI is reserved for send packet from master to slave in dual wifi architechture*/
	if(intPri==CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI)
		return RT_ERR_RG_INVALID_PARAM;
#endif
	ASSERT_EQ(rtk_qos_dscpPriRemapGroup_set(0,dscp,intPri,0),RT_ERR_OK);
	return RT_ERR_RG_OK;
}

int32 rtk_rg_qosDscpPriRemapGroup_get(uint32 dscp,uint32 *pIntPri){	
	uint32 dp;
	//ASSERT_EQ(rtk_qos_dscpPriRemapGroup_get(0,dscp,pIntPri,0),RT_ERR_OK);
	ASSERT_EQ(rtk_qos_dscpPriRemapGroup_get(0,dscp,pIntPri,&dp),RT_ERR_OK);
	return RT_ERR_RG_OK;
}

int32 rtk_rg_qosPortPri_set(rtk_rg_mac_port_idx_t port,uint32 intPri){
#ifdef CONFIG_DUALBAND_CONCURRENT	
		/*internal-priority CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI is reserved for send packet from master to slave in dual wifi architechture*/
		if(intPri==CONFIG_DEFAULT_TO_SLAVE_GMAC_PRI)
			return RT_ERR_RG_INVALID_PARAM;
#endif
	
	ASSERT_EQ(rtk_qos_portPri_set(port, intPri),RT_ERR_OK);
	return RT_ERR_RG_OK;
}

int32 rtk_rg_qosPortPri_get(rtk_rg_mac_port_idx_t port,uint32 *pIntPri){
	ASSERT_EQ(rtk_qos_portPri_get(port, pIntPri),RT_ERR_OK);
	return RT_ERR_RG_OK;
}


#if 0 //MIB Proc
int rtt_rg_proc_mibInfo_get(struct file *filp, const char *buff,unsigned long len, void *data )
{
	int ret;
	rtk_rg_port_mib_info_t mib;
	char	*tmpbuf=NULL;	
	char	*strptr=NULL;
	int val;

	tmpbuf=rtk_rg_malloc(512);
	if(tmpbuf==NULL) return 0;
	
	if (buff && !copy_from_user(tmpbuf, buff, len)) 
	{		
		tmpbuf[len] = '\0'; 			
		strptr=tmpbuf;
	}

	sscanf(strptr,"%d",&val);

	ret = rtk_rg_portMibInfo_get(val,&mib);
	assert_ok(ret);

	rtlglue_printf("[Port:%d]\n",val);
    rtlglue_printf("ifInOctets:                          %lld\n",mib.ifInOctets);
    rtlglue_printf("ifInUcastPkts:                       %d\n",mib.ifInUcastPkts);
    rtlglue_printf("ifInMulticastPkts:                   %d\n",mib.ifInMulticastPkts);
    rtlglue_printf("ifInBroadcastPkts:                   %d\n",mib.ifInBroadcastPkts);
    rtlglue_printf("ifInDiscards:                        %d\n",mib.ifInDiscards);
    rtlglue_printf("ifOutOctets:                         %lld\n",mib.ifOutOctets);
    rtlglue_printf("ifOutDiscards:                       %d\n",mib.ifOutDiscards);
    rtlglue_printf("ifOutUcastPkts:                      %d\n",mib.ifOutUcastPkts);
    rtlglue_printf("ifOutMulticastPkts:                  %d\n",mib.ifOutMulticastPkts);
    rtlglue_printf("ifOutBrocastPkts:                    %d\n",mib.ifOutBrocastPkts);
    rtlglue_printf("dot1dBasePortDelayExceededDiscards:  %d\n",mib.dot1dBasePortDelayExceededDiscards);
    rtlglue_printf("dot1dTpPortInDiscards:               %d\n",mib.dot1dTpPortInDiscards);
    rtlglue_printf("dot1dTpHcPortInDiscards:             %d\n",mib.dot1dTpHcPortInDiscards);
    rtlglue_printf("dot3InPauseFrames:                   %d\n",mib.dot3InPauseFrames);
    rtlglue_printf("dot3OutPauseFrames:                  %d\n",mib.dot3OutPauseFrames);
//    rtlglue_printf("dot3OutPauseOnFrames:                %d\n",mib.dot3OutPauseOnFrames);
    rtlglue_printf("dot3StatsAligmentErrors:             %d\n",mib.dot3StatsAligmentErrors);
    rtlglue_printf("dot3StatsFCSErrors:                  %d\n",mib.dot3StatsFCSErrors);
    rtlglue_printf("dot3StatsSingleCollisionFrames:      %d\n",mib.dot3StatsSingleCollisionFrames);
    rtlglue_printf("dot3StatsMultipleCollisionFrames:    %d\n",mib.dot3StatsMultipleCollisionFrames);
    rtlglue_printf("dot3StatsDeferredTransmissions:      %d\n",mib.dot3StatsDeferredTransmissions);
    rtlglue_printf("dot3StatsLateCollisions:             %d\n",mib.dot3StatsLateCollisions);
    rtlglue_printf("dot3StatsExcessiveCollisions:        %d\n",mib.dot3StatsExcessiveCollisions);
    rtlglue_printf("dot3StatsFrameTooLongs:              %d\n",mib.dot3StatsFrameTooLongs);
    rtlglue_printf("dot3StatsSymbolErrors:               %d\n",mib.dot3StatsSymbolErrors);
    rtlglue_printf("dot3ControlInUnknownOpcodes:         %d\n",mib.dot3ControlInUnknownOpcodes);
    rtlglue_printf("etherStatsDropEvents:                %d\n",mib.etherStatsDropEvents);
    rtlglue_printf("etherStatsOctets:                    %lld\n",mib.etherStatsOctets);
    rtlglue_printf("etherStatsBcastPkts:                 %d\n",mib.etherStatsBcastPkts);
    rtlglue_printf("etherStatsMcastPkts:                 %d\n",mib.etherStatsMcastPkts);
    rtlglue_printf("etherStatsUndersizePkts:             %d\n",mib.etherStatsUndersizePkts);
    rtlglue_printf("etherStatsOversizePkts:              %d\n",mib.etherStatsOversizePkts);
    rtlglue_printf("etherStatsFragments:                 %d\n",mib.etherStatsFragments);
    rtlglue_printf("etherStatsJabbers:                   %d\n",mib.etherStatsJabbers);
    rtlglue_printf("etherStatsCollisions:                %d\n",mib.etherStatsCollisions);
    rtlglue_printf("etherStatsCRCAlignErrors:            %d\n",mib.etherStatsCRCAlignErrors);
    rtlglue_printf("etherStatsPkts64Octets:              %d\n",mib.etherStatsPkts64Octets);
    rtlglue_printf("etherStatsPkts65to127Octets:         %d\n",mib.etherStatsPkts65to127Octets);
    rtlglue_printf("etherStatsPkts128to255Octets:        %d\n",mib.etherStatsPkts128to255Octets);
    rtlglue_printf("etherStatsPkts256to511Octets:        %d\n",mib.etherStatsPkts256to511Octets);
    rtlglue_printf("etherStatsPkts512to1023Octets:       %d\n",mib.etherStatsPkts512to1023Octets);
    rtlglue_printf("etherStatsPkts1024to1518Octets:      %d\n",mib.etherStatsPkts1024to1518Octets);
    rtlglue_printf("etherStatsTxOctets:                  %lld\n",mib.etherStatsTxOctets);
    rtlglue_printf("etherStatsTxUndersizePkts:           %d\n",mib.etherStatsTxUndersizePkts);
    rtlglue_printf("etherStatsTxOversizePkts:            %d\n",mib.etherStatsTxOversizePkts);
    rtlglue_printf("etherStatsTxPkts64Octets:            %d\n",mib.etherStatsTxPkts64Octets);
    rtlglue_printf("etherStatsTxPkts65to127Octets:       %d\n",mib.etherStatsTxPkts65to127Octets);
    rtlglue_printf("etherStatsTxPkts128to255Octets:      %d\n",mib.etherStatsTxPkts128to255Octets);
    rtlglue_printf("etherStatsTxPkts256to511Octets:      %d\n",mib.etherStatsTxPkts256to511Octets);
    rtlglue_printf("etherStatsTxPkts512to1023Octets:     %d\n",mib.etherStatsTxPkts512to1023Octets);
    rtlglue_printf("etherStatsTxPkts1024to1518Octets:    %d\n",mib.etherStatsTxPkts1024to1518Octets);
    rtlglue_printf("etherStatsTxPkts1519toMaxOctets:     %d\n",mib.etherStatsTxPkts1519toMaxOctets);
    rtlglue_printf("etherStatsTxBcastPkts:               %d\n",mib.etherStatsTxBcastPkts);
    rtlglue_printf("etherStatsTxMcastPkts:               %d\n",mib.etherStatsTxBcastPkts);
    rtlglue_printf("etherStatsTxFragments:               %d\n",mib.etherStatsTxFragments);
    rtlglue_printf("etherStatsTxJabbers:                 %d\n",mib.etherStatsTxJabbers);
    rtlglue_printf("etherStatsTxCRCAlignErrors:          %d\n",mib.etherStatsTxCRCAlignErrors);
    rtlglue_printf("etherStatsRxUndersizePkts:           %d\n",mib.etherStatsRxUndersizePkts);
    rtlglue_printf("etherStatsRxUndersizeDropPkts:       %d\n",mib.etherStatsRxUndersizeDropPkts);
    rtlglue_printf("etherStatsRxOversizePkts:            %d\n",mib.etherStatsRxOversizePkts);
    rtlglue_printf("etherStatsRxPkts64Octets:            %d\n",mib.etherStatsRxPkts64Octets);
    rtlglue_printf("etherStatsRxPkts65to127Octets:       %d\n",mib.etherStatsRxPkts65to127Octets);
    rtlglue_printf("etherStatsRxPkts128to255Octets:      %d\n",mib.etherStatsRxPkts128to255Octets);
    rtlglue_printf("etherStatsRxPkts256to511Octets:      %d\n",mib.etherStatsRxPkts256to511Octets);
    rtlglue_printf("etherStatsRxPkts512to1023Octets:     %d\n",mib.etherStatsRxPkts512to1023Octets);
    rtlglue_printf("etherStatsRxPkts1024to1518Octets:    %d\n",mib.etherStatsRxPkts1024to1518Octets);
    rtlglue_printf("etherStatsRxPkts1519toMaxOctets:     %d\n",mib.etherStatsRxPkts1519toMaxOctets);
    rtlglue_printf("inOampduPkts:                        %d\n",mib.inOampduPkts);
    rtlglue_printf("outOampduPkts:                       %d\n",mib.outOampduPkts);

	rtk_rg_free(tmpbuf);
	return len;
}

int rtt_rg_proc_mibInfo_clear(struct file *filp, const char *buff,unsigned long len, void *data )
{
	int ret;
	rtk_rg_port_mib_info_t mib;
	char	*tmpbuf=NULL;	
	char	*strptr=NULL;
	int val;

	tmpbuf=rtk_rg_malloc(512);
	if(tmpbuf==NULL) return 0;
	
	if (buff && !copy_from_user(tmpbuf, buff, len)) 
	{		
		tmpbuf[len] = '\0'; 			
		strptr=tmpbuf;
	}

	sscanf(strptr,"%d",&val);

	ret = rtk_rg_portMibInfo_clear(val);
	assert_ok(ret);

	rtk_rg_free(tmpbuf);
	return len;
}

#endif

#if 0 //DoS Proc
int rtk_rg_proc_dos_port_set( struct file *filp, const char *buff,unsigned long len, void *data )
{
	char	*tmpbuf=NULL;	
	char	*strptr=NULL;
	int val;
	int i;
	int ret;
	rtk_rg_mac_portmask_t pmsk;

	tmpbuf=rtk_rg_malloc(512);
	if(tmpbuf==NULL) return 0;
	
	if (buff && !copy_from_user(tmpbuf, buff, len)) 
	{		
		tmpbuf[len] = '\0'; 			
		strptr=tmpbuf;
	}

	sscanf(strptr,"0x%x",&val);

	pmsk.portmask = val;
	ret = rtk_rg_dosPortMaskEnable_set(pmsk);
	assert_ok(ret);

	for(i=0;i<RTK_RG_MAC_PORT_MAX;i++)
	{
		rtk_enable_t valid;
		ret = rtk_sec_portAttackPreventState_get(i,&valid);
		assert_ok(ret);
		rtlglue_printf("Port %d : %s.\n",i,(valid==1)?"ENABLED":"DISABLED");
	}
	
	rtk_rg_free(tmpbuf);
	return len;
}

int rtk_rg_proc_dos_port_get(void)
{
	int ret;
	int i;
	rtk_rg_mac_portmask_t pmsk;

	ret = rtk_rg_dosPortMaskEnable_get(&pmsk);
	assert_ok(ret);

	for(i=0;i<RTK_RG_MAC_PORT_MAX;i++)
		rtlglue_printf("Port %d security is %s\n",i,(pmsk.portmask&(0x1<<i))?"enabled":"disabled");

	return RT_ERR_RG_OK;
}

int rtk_rg_proc_dos_type_set( struct file *filp, const char *buff,unsigned long len, void *data )
{
	char	*tmpbuf=NULL;	
	char	*strptr=NULL;
	int val[3];
	int ret;

	tmpbuf=rtk_rg_malloc(512);
	if(tmpbuf==NULL) return 0;
	
	if (buff && !copy_from_user(tmpbuf, buff, len)) 
	{		
		tmpbuf[len] = '\0'; 			
		strptr=tmpbuf;
	}

	sscanf(strptr,"%d %d %d",&val[0],&val[1],&val[2]);

	ret = rtk_rg_dosType_set(val[0],val[1],val[2]);
	assert_ok(ret);

	rtk_rg_free(tmpbuf);
	return len;
}

int rtk_rg_proc_dos_type_get(void)
{
	int i;
	int ret;
	char* strType[] = {"DAEQSA_DENY","LAND_DENY","BLAT_DENY","SYNFIN_DENY","XMA_DENY","NULLSCAN_DENY",
			"SYN_SPORTL1024_DENY","TCPHDR_MIN_CHECK","TCP_FRAG_OFF_MIN_CHECK","ICMP_FRAG_PKTS_DENY",
			"POD_DENY","UDPDOMB_DENY","SYNWITHDATA_DENY","SYNFLOOD_DENY","FINFLOOD_DENY","ICMPFLOOD_DENY"};

	for(i=0;i<=RTK_RG_DOS_SYNWITHDATA_DENY;i++)
	{
		int dos_enable=-1;
		rtk_rg_dos_action_t dos_action=-1;
		ret = rtk_rg_dosType_get(i,&dos_enable,&dos_action);
		assert_ok(ret);
		rtlglue_printf("[%s] [%s] [Action:%s] \n",strType[i],(dos_enable==1)?"Enabled":"Disabled",
						(dos_action==RTK_RG_DOS_ACTION_DROP)?"Drop":"Trap");
	}

	return RT_ERR_RG_OK;
}

int rtk_rg_proc_dos_flood_set( struct file *filp, const char *buff,unsigned long len, void *data )
{
	char	*tmpbuf=NULL;	
	char	*strptr=NULL;
	int val[4];
	int ret;

	tmpbuf=rtk_rg_malloc(512);
	if(tmpbuf==NULL) return 0;
	
	if (buff && !copy_from_user(tmpbuf, buff, len)) 
	{		
		tmpbuf[len] = '\0'; 			
		strptr=tmpbuf;
	}

	sscanf(strptr,"%d %d %d %d",&val[0],&val[1],&val[2],&val[3]);

	ret = rtk_rg_dosFlood_set(val[0],val[1],val[2],val[3]);
	assert_ok(ret);

	rtk_rg_free(tmpbuf);
	return len;
}

int rtk_rg_proc_dos_flood_get(void)
{
	int i;
	int ret;
	char* strType[] = {"DAEQSA_DENY","LAND_DENY","BLAT_DENY","SYNFIN_DENY","XMA_DENY","NULLSCAN_DENY",
			"SYN_SPORTL1024_DENY","TCPHDR_MIN_CHECK","TCP_FRAG_OFF_MIN_CHECK","ICMP_FRAG_PKTS_DENY",
			"POD_DENY","UDPDOMB_DENY","SYNWITHDATA_DENY","SYNFLOOD_DENY","FINFLOOD_DENY","ICMPFLOOD_DENY"};

	for(i=RTK_RG_DOS_SYNFLOOD_DENY;i<=RTK_RG_DOS_ICMPFLOOD_DENY;i++)
	{
		int dos_enable=-1,dos_action=-1,dos_threshold=-1;
		ret = rtk_rg_dosFlood_get(i,&dos_enable,&dos_action,&dos_threshold);
		assert_ok(ret);
		rtlglue_printf("[%s] [%s] [Action:%s] [Threshold:%d]\n",strType[i],(dos_enable==1)?"Enabled":"Disabled",
						(dos_action==RTK_RG_DOS_ACTION_DROP)?"Drop":"Trap",dos_threshold);
	}

	return RT_ERR_RG_OK;
}

#endif

#ifdef CONFIG_RG_WLAN_HWNAT_ACCELERATION

void _rtk_rg_wlanMbssidLearning(u8* smac,int wlan_dev_idx)
{
	int i;
	for(i=rg_db.wlanMbssidHeadIdx;i<rg_db.wlanMbssidHeadIdx+MAX_WLAN_MBSSID_SW_TABLE_SIZE;i++)
	{
		int idx=i%MAX_WLAN_MBSSID_SW_TABLE_SIZE;
		if(memcmp(rg_db.wlanMbssid[idx].mac.octet,smac,6)==0) //the MAC is finded in table.
		{
			if(rg_db.wlanMbssid[idx].wlan_dev_idx!=wlan_dev_idx)
			{
				rg_db.wlanMbssid[idx].wlan_dev_idx=wlan_dev_idx; //update ingress device
				DEBUG("#### UPDATE MBSSID INFO, MAC=%02x:%02x:%02x:%02x:%02x:%02x WLAN_DEV_IDX=%d ####",
					smac[0],smac[1],smac[2],smac[3],smac[4],smac[5],wlan_dev_idx);					
			}
			return;			
		}
	}
	
	//not found in table
	rg_db.wlanMbssidHeadIdx=(rg_db.wlanMbssidHeadIdx+MAX_WLAN_MBSSID_SW_TABLE_SIZE-1)%MAX_WLAN_MBSSID_SW_TABLE_SIZE;
	rg_db.wlanMbssid[rg_db.wlanMbssidHeadIdx].wlan_dev_idx=wlan_dev_idx;
	memcpy(rg_db.wlanMbssid[rg_db.wlanMbssidHeadIdx].mac.octet,smac,6);
	DEBUG("#### LEARNING MBSSID, MAC=%02x:%02x:%02x:%02x:%02x:%02x WLAN_DEV_IDX=%d ####",
		smac[0],smac[1],smac[2],smac[3],smac[4],smac[5],wlan_dev_idx);		

	return;
}

void _rtk_rg_wlanMbssidLookup(u8 *dmac,int *wlan_dev_idx)
{
	int i;
	for(i=rg_db.wlanMbssidHeadIdx;i<rg_db.wlanMbssidHeadIdx+MAX_WLAN_MBSSID_SW_TABLE_SIZE;i++)
	{
		int idx=i%MAX_WLAN_MBSSID_SW_TABLE_SIZE;
		if(memcmp(rg_db.wlanMbssid[idx].mac.octet,dmac,6)==0) //the MAC is finded in table.
		{	
			int newIdx;
			rtk_rg_table_wlan_mbssid_t tmpBuf;
			*wlan_dev_idx=rg_db.wlanMbssid[idx].wlan_dev_idx;
			if(idx==rg_db.wlanMbssidHeadIdx) return;
			//LRU: Swaping this entry to first lookup index.
			newIdx=(rg_db.wlanMbssidHeadIdx+MAX_WLAN_MBSSID_SW_TABLE_SIZE-1)%MAX_WLAN_MBSSID_SW_TABLE_SIZE;
			memcpy(&tmpBuf,&rg_db.wlanMbssid[newIdx],sizeof(tmpBuf));
			memcpy(&rg_db.wlanMbssid[newIdx],&rg_db.wlanMbssid[idx],sizeof(tmpBuf));
			memcpy(&rg_db.wlanMbssid[idx],&tmpBuf,sizeof(tmpBuf));
			rg_db.wlanMbssidHeadIdx=newIdx;
			return;
		}
	}
	*wlan_dev_idx=0;
}

int _rtk_master_wlan_mbssid_tx(struct sk_buff *skb)
{
	int wlan_dev_idx;
	int flooding=0;
	int i;
	struct sk_buff *new_skb=skb;

	if(skb->data[0]&1)
	{
		flooding=1;		
	}
	else
	{
		_rtk_rg_wlanMbssidLookup(skb->data,&wlan_dev_idx);
		if(wlan_dev_idx>4) return FAIL;
	}

	for(i=0;i<5;i++)
	{
		if(flooding==0)
		{
			i=wlan_dev_idx;
		}
		
		switch(i)
		{
			case 0:				
				if(wlan_root_netdev) 
				{
					if(test_bit(__LINK_STATE_START, &wlan_root_netdev->state))
					{

						if(flooding==1)
						{
							//new_skb=rtk_rg_skbCopyToPreAllocSkb(skb);
							new_skb=skb_clone(skb,GFP_ATOMIC);
							if(new_skb==NULL) goto OUT_OF_MEM;
						}					
						//TRACE("Send to Master WIFI root intf");				
						rtl8192cd_start_xmit(new_skb,wlan_root_netdev);
						if(flooding==0) return 0;
					}
				}
				break;
			default:				
				if(wlan_vap_netdev[i-1]) 
				{
					if(test_bit(__LINK_STATE_START, &wlan_vap_netdev[i-1]->state))
					{
						if(flooding==1)
						{
							//new_skb=rtk_rg_skbCopyToPreAllocSkb(skb);	
							new_skb=skb_clone(skb,GFP_ATOMIC);
							if(new_skb==NULL) goto OUT_OF_MEM;
						}
						//TRACE("Send to Master WIFI vap[%d] intf",wlan_dev_idx-1);
						rtl8192cd_start_xmit(new_skb,wlan_vap_netdev[i-1]);
						if(flooding==0) return i;
					}
				}
				break;
		}

		if(flooding==0)
		{
			return FAIL;
		}

	}	
	if(flooding==1)
	{
		dev_kfree_skb_any(skb);
		return WIFI_FLOOD_INTF_RET;
	}
	return FAIL;
OUT_OF_MEM:
	FIXME("out of mem(%s:%d)\n",__FUNCTION__,__LINE__);
	return FAIL;

}
#endif

#ifdef CONFIG_RG_ARP_AUTO_AGEOUT
int _rtk_rg_arpTraffic_get(uint32 *trafficSet)
{
	int32 i;
#ifdef __KERNEL__	
	uint32 pValue;
	int newTable,oldTable;
	uint32 CLR_ARP_TRF[2] = {CLR_ARP_TRF0,CLR_ARP_TRF1};

	/* parameter check */
	RT_PARAM_CHK((NULL == trafficSet), RT_ERR_RG_NULL_POINTER);

	/*get current work table*/
	ioal_mem32_read((uint32)(REG_NAT_CTRL),&pValue);
	oldTable = (pValue>>FIELD_ARP_TRF_SEL_OFFSET)&0x1;

	newTable = !oldTable;

	/*swap*/
	ioal_mem32_read((uint32)(REG_NAT_CTRL),&pValue);
	if(newTable)
		pValue |= (0x1<<FIELD_ARP_TRF_SEL_OFFSET);
	else
		pValue &= ~(0x1<<FIELD_ARP_TRF_SEL_OFFSET);
	ioal_mem32_write((uint32)(REG_NAT_CTRL),pValue);
	while(1)
	{
		//Check swap is done
		ioal_mem32_read((uint32)(REG_NAT_CTRL),&pValue);
		if(!((pValue>>FIELD_ARP_TRF_CHG_OFFSET)&0x1)) break;
	}

	/*get*/
	for(i=0;i<(MAX_ARP_HW_TABLE_SIZE/32);i++)
	{
		if(rg_db.arpValidSet[i])
		{
			if(oldTable)
				ioal_mem32_read((uint32)(REG_ARP_TRF1+(i<<2)),&trafficSet[i]);
			else
				ioal_mem32_read((uint32)(REG_ARP_TRF0+(i<<2)),&trafficSet[i]);

			//DEBUG("valid[%d]:%x traffic[%d]:%x\n",i,rg_db.arpValidSet[i],i,trafficSet[i]);
		}
	}

	/* Clear table */
	ioal_mem32_read((uint32)(REG_NAT_CTRL),&pValue);
	pValue |= CLR_ARP_TRF[oldTable]<<FIELD_ARP_TRF_CLR_OFFSET;
	ioal_mem32_write((uint32)REG_NAT_CTRL,pValue);
#else
	for(i=0;i<(MAX_ARP_HW_TABLE_SIZE/32);i++)
	{
		trafficSet[i]=0;
	}	
#endif

	return RT_ERR_RG_OK;
}

#endif

#ifdef CONFIG_RG_NAPT_AUTO_AGEOUT

int _rtk_rg_naptTraffic_get(uint32 *validSet, uint32 *trafficSet)
{
	int32 i;
#ifdef __KERNEL__	
	uint32 pValue;
	int newTable,oldTable;
	uint32 CLR_L4_TRF[2] = {CLR_L4_TRF0,CLR_L4_TRF1};

	/* parameter check */
	RT_PARAM_CHK((NULL == validSet), RT_ERR_RG_NULL_POINTER);
	RT_PARAM_CHK((NULL == trafficSet), RT_ERR_RG_NULL_POINTER);

	/*get current work table*/
	ioal_mem32_read((uint32)(REG_NAT_CTRL),&pValue);
	oldTable = (pValue>>FIELD_L4_TRF_SEL_OFFSET)&0x1;

	newTable = !oldTable;

	/*swap*/
	ioal_mem32_read((uint32)(REG_NAT_CTRL),&pValue);
	if(newTable)
		pValue |= (0x1<<FIELD_L4_TRF_SEL_OFFSET);
	else
		pValue &= ~(0x1<<FIELD_L4_TRF_SEL_OFFSET);
	ioal_mem32_write((uint32)(REG_NAT_CTRL),pValue);
	while(1)
	{
		//Check swap is done
		ioal_mem32_read((uint32)(REG_NAT_CTRL),&pValue);
		if(!((pValue>>FIELD_L4_TRF_CHG_OFFSET)&0x1)) break;
	}

	/*get*/
	for(i=0;i<(2048/32);i++)
	{
		if(validSet[i])
		{
			if(oldTable)
				ioal_mem32_read((uint32)(REG_L4_TRF1+(i<<2)),&trafficSet[i]);
			else
				ioal_mem32_read((uint32)(REG_L4_TRF0+(i<<2)),&trafficSet[i]);

			//DEBUG("valid[%d]:%x traffic[%d]:%x\n",i,rg_db.naptValidSet[i],i,trafficSet[i]);
		}
	}

	/* Clear table */
	ioal_mem32_read((uint32)(REG_NAT_CTRL),&pValue);
	pValue |= CLR_L4_TRF[oldTable]<<FIELD_L4_TRF_CLR_OFFSET;
	ioal_mem32_write((uint32)REG_NAT_CTRL,pValue);
#else
	for(i=0;i<(2048/32);i++)
	{
		trafficSet[i]=0;
	}	
#endif

	return RT_ERR_RG_OK;
}

void _rtk_rg_naptTimeoutCheckByOutIdx(int naptIdx)
{
	int isTimeout = 0;	
	if(rg_db.naptOut[naptIdx].state==INVALID)
	{
		//NO action
	}
	else
	{
		switch(rg_db.naptOut[naptIdx].state)
		{
			case INVALID:
				break;
			case TCP_CONNECTED:
				if(rg_db.naptOut[naptIdx].idleSecs>=RTK_RG_TCP_LONG_TIMEOUT) 
				isTimeout = 1;
				break;
			case UDP_CONNECTED:
				if(rg_db.naptOut[naptIdx].idleSecs>=RTK_RG_UDP_LONG_TIMEOUT)
				isTimeout = 1;
				break;
			case SYN_RECV:
			case SYN_ACK_RECV:
			case FIN_RECV:
			case RST_RECV:
				if(rg_db.naptOut[naptIdx].idleSecs>=RTK_RG_TCP_SHORT_TIMEOUT) 
				isTimeout = 1;
				break;
			case UDP_FIRST:
			case UDP_SECOND:
				if(rg_db.naptOut[naptIdx].idleSecs>=RTK_RG_UDP_SHORT_TIMEOUT)
				isTimeout = 1;
				break;
		}
	}

	//Timeout check
	if(isTimeout)
	{
		assert_ok(rtk_rg_naptConnection_del(naptIdx));
	}
}

int _rtk_rg_naptTimeoutCheck(void)
{
	int i=0,j=0;
	int inIdx,outIdx,ret;
	int elapsedTime = (RTK_RG_FWDENGINE_HOUSE_KEEP_SECOND*RTK_RG_MAX_HOUSE_KEEP_SELECT); //TICKTIME_PERIOD_SECOND; //secs
	uint32 notIdleSet[2048/32];
	rtk_rg_table_naptOut_linkList_t *pNaptOutList;

	//rtlglue_printf("TICK at %ld . Check NAPT timeout.\n",jiffies);
	//Read traffic bits
	ret = _rtk_rg_naptTraffic_get(&rg_db.naptValidSet[0],&notIdleSet[0]);
	if(ret!=RT_ERR_RG_OK) return ret;

	//Check idle
	for(i=0;i<(2048/32);i++)
	{
		if(rg_db.naptValidSet[i])
		{
			for(j=0;j<32;j++)
			{
				if(rg_db.naptValidSet[i] & (0x1<<j))
				{
					outIdx = (i<<5)+j;
					inIdx = rg_db.naptOut[outIdx].rtk_naptOut.hashIdx;
					if(notIdleSet[i]&(0x1<<j))
					{
						rg_db.naptOut[outIdx].idleSecs = 0;
						rg_db.naptIn[inIdx].idleSecs = 0;
					}
					else
					{
						rg_db.naptOut[outIdx].idleSecs += elapsedTime;
						rg_db.naptIn[inIdx].idleSecs += elapsedTime;
					}			
					_rtk_rg_naptTimeoutCheckByOutIdx(outIdx);
				}
			}
		}
	}

	//Check software Link List
	for(i=0;i<MAX_NAPT_OUT_HW_TABLE_SIZE>>2;i++)
	{
		pNaptOutList=rg_db.pNaptOutHashListHead[i];
		while(pNaptOutList!=NULL)
		{
			outIdx = pNaptOutList->idx;
			inIdx = rg_db.naptOut[outIdx].rtk_naptOut.hashIdx;
			rg_db.naptOut[outIdx].idleSecs += elapsedTime;
			rg_db.naptIn[inIdx].idleSecs += elapsedTime;
			_rtk_rg_naptTimeoutCheckByOutIdx(outIdx);
			pNaptOutList=pNaptOutList->pNext;
		}
	}
		
	return RT_ERR_RG_OK;
}

#endif

#ifdef CONFIG_RG_ARP_AUTO_AGEOUT

int _rtk_rg_arpTimeoutCheck(void)
{
	int i=0,j=0;
	int idx,ret;
	int elapsedTime = (RTK_RG_FWDENGINE_HOUSE_KEEP_SECOND*RTK_RG_MAX_HOUSE_KEEP_SELECT); //TICKTIME_PERIOD_SECOND; //secs
	uint32 notIdleSet[(MAX_ARP_HW_TABLE_SIZE/32)]={0};

	//rtlglue_printf("TICK at %ld . Check ARP timeout.\n",jiffies);
	//Read traffic bits
	ret = _rtk_rg_arpTraffic_get(&notIdleSet[0]);
	if(ret!=RT_ERR_RG_OK) return ret;

	//Check idle
	for(i=0;i<(MAX_ARP_HW_TABLE_SIZE>>5);i++)
	{
		if(rg_db.arpValidSet[i])
		{
			//rtlglue_printf("Not Idle Set[%d]:%x\n",i,notIdleSet[i]);
			for(j=0;j<32;j++)
			{
				if(rg_db.arpValidSet[i] & (0x1<<j))
				{
					idx = (i<<5)+j;
					if(notIdleSet[i]&(0x1<<j))
						rg_db.arp[idx].idleSecs = 0;
					else
						rg_db.arp[idx].idleSecs += elapsedTime;

					if(rg_db.arp[idx].idleSecs>=RTK_RG_ARP_TIMEOUT && rg_db.arp[idx].staticEntry==0) 
						assert_ok(rtk_rg_arpEntry_del(idx));
				}
			}
		}
	}
		
	return RT_ERR_RG_OK;
}

int _rtk_rg_swArpTimeoutCheck(void)
{
	int i;
	rtk_rg_arp_linkList_t *pArpList,*pNextArpList;

	for(i=0;i<MAX_ARP_SW_TABLE_HEAD;i++)
	{
		if(!list_empty(&rg_db.softwareArpTableHead[i]))		//not empty, check each entry
		{
			list_for_each_entry_safe(pArpList,pNextArpList,&rg_db.softwareArpTableHead[i],arp_list)
			{
				rg_db.arp[pArpList->idx].idleSecs+=(RTK_RG_FWDENGINE_HOUSE_KEEP_SECOND*RTK_RG_MAX_HOUSE_KEEP_SELECT);

				if(rg_db.arp[pArpList->idx].idleSecs>=RTK_RG_ARP_TIMEOUT && rg_db.arp[pArpList->idx].staticEntry==0) 
				{
					//Clear ARP and LUT table entry
					assert_ok(rtk_rg_macEntry_del(rg_db.arp[pArpList->idx].rtk_arp.nhIdx));
					_rtk_rg_softwareArpTableDel(pArpList);
				}
			}
		}
	}

	return RT_ERR_RG_OK;
}

#endif

#ifdef CONFIG_RG_LAYER2_SOFTWARE_LEARN
int	_rtk_rg_layer2SoftwareAgeOut(void)
{
	int i,ret,indexCheck;

	if(rg_db.systemGlobal.layer2HouseKeepIndex > MAX_LUT_HW_TABLE_SIZE-Layer2HOUSE_KEEP_NUM)
	{
		FIXME("the layer2Housekeepindex is %d",rg_db.systemGlobal.layer2HouseKeepIndex);
		rg_db.systemGlobal.layer2HouseKeepIndex=0;
	}
	//each time we check Layer2HOUSE_KEEP_NUM entries
	//FIXME: if there is bCAM used, it will become 2048+64 entries to check at most
	indexCheck=rg_db.systemGlobal.layer2HouseKeepIndex;
	for(i=0;i<Layer2HOUSE_KEEP_NUM;i++)
	{
		if(rg_db.lut[indexCheck].valid==1 && 
			rg_db.lut[indexCheck].rtk_lut.entryType==RTK_LUT_L2UC &&
			(rg_db.lut[indexCheck].rtk_lut.entry.l2UcEntry.flags&RTK_L2_UCAST_FLAG_STATIC)==0)	//check dynamic entries only
		{
			//Check the hardware table's age field, if it is zero, invalid this software entry
			ret=rtk_l2_addr_get(&rg_db.lut[indexCheck].rtk_lut.entry.l2UcEntry);
			if(ret==RT_ERR_L2_ENTRY_NOTFOUND || rg_db.lut[indexCheck].rtk_lut.entry.l2UcEntry.age==0)
			{
				DEBUG("invalid the age zero entry %d !",indexCheck);
				//------------------ Critical Section start -----------------------//
				//rg_lock(&rg_kernel.saLearningLimitLock);
				atomic_dec(&rg_db.systemGlobal.sourceAddrLearningCount[rg_db.lut[indexCheck].rtk_lut.entry.l2UcEntry.port]);
				//------------------ Critical Section End -----------------------//
				//rg_unlock(&rg_kernel.saLearningLimitLock);
			
				rg_db.lut[indexCheck].valid=0;
			}
		}
		indexCheck++;
	}
	if(indexCheck==MAX_LUT_HW_TABLE_SIZE)
	{
		indexCheck=0;
#ifdef CONFIG_RG_LAYER2_SOFTWARE_LEARN
		if(rg_kernel.l2_hw_aging==1)
		{
			FIXME("enable auto learning in a bit time.(2ms)");
			// 20130725: patch for N2X traffic full loading will cause LUT timeout and the packets will out of sequence.
			for(i=0;i<RTK_RG_MAC_PORT_CPU;i++)
			{
				ASSERT_EQ(rtk_l2_portLimitLearningCnt_set(i,2112),RT_ERR_OK);	
			}
			mdelay(2);
			for(i=0;i<RTK_RG_MAC_PORT_CPU;i++)
			{
				ASSERT_EQ(rtk_l2_portLimitLearningCnt_set(i,0),RT_ERR_OK);	
			}
		}
#endif

		
	}
	rg_db.systemGlobal.layer2HouseKeepIndex=indexCheck;
	return RT_ERR_RG_OK;
}
#endif

void rtk_rg_fwdEngineHouseKeepingTimerFunc(unsigned long task_priv)
{
	//struct rtl8190_priv *priv = (struct rtl8190_priv *)task_priv;
	//rtlglue_printf("%s\n",__func__);
	//DEBUG("Into HouseKeeping...");
#ifdef CONFIG_RG_NAPT_AUTO_AGEOUT
	//NAPT Timeout
	if(rg_kernel.timer_selector==RTK_RG_NAPT_HOUSE_KEEP_SELECT)
	{
//		DEBUG("NAPT Tick!\n");
		_rtk_rg_naptTimeoutCheck();
	}
#endif
#ifdef CONFIG_RG_ARP_AUTO_AGEOUT
	//ARP Timeout
	if(rg_kernel.timer_selector==RTK_RG_ARP_HOUSE_KEEP_SELECT)
	{
//		DEBUG("ARP Tick!\n");
		_rtk_rg_arpTimeoutCheck();
		_rtk_rg_swArpTimeoutCheck();
	}
#endif
#ifdef CONFIG_RG_LAYER2_SOFTWARE_LEARN
	if(rg_kernel.timer_selector==RTK_RG_LAYER2_HOUSE_KEEP_SELECT)
	{
//		DEBUG("LUT Tick!\n");
		_rtk_rg_layer2SoftwareAgeOut();
	}
#endif
#ifdef __KERNEL__
	mod_timer(&rg_kernel.fwdEngineHouseKeepingTimer, jiffies+(RTK_RG_FWDENGINE_HOUSE_KEEP_SECOND*TICKTIME_PERIOD));
#endif
	if(RTK_RG_MAX_HOUSE_KEEP_SELECT>0)
		rg_kernel.timer_selector = ((rg_kernel.timer_selector+1)%RTK_RG_MAX_HOUSE_KEEP_SELECT);
}

#ifdef __KERNEL__


int rtk_rg_fwdEngine_pppoe(void)
{
	int lanIntfIdx;
	int wanIntfIdx;
	rtk_rg_lanIntfConf_t lan_info;
	rtk_rg_wanIntfConf_t wan_info;
	//rtk_rg_ipStaticInfo_t staticInfo;
	
	rtk_rg_pppoeClientInfoBeforeDial_t pppoeClientInfoB;
	rtk_rg_pppoeClientInfoAfterDial_t pppoeClientInfoA;

	assert_ok(rtk_rg_initParam_set(NULL));
	

	memset(&wan_info, 0, sizeof(wan_info));
	memset(&pppoeClientInfoB, 0, sizeof(pppoeClientInfoB));
	memset(&pppoeClientInfoA, 0, sizeof(pppoeClientInfoA));


	memset(&lan_info,0,sizeof(lan_info));
	
	lan_info.gmac.octet[0]=0x00;
	lan_info.gmac.octet[1]=0xe0;
	lan_info.gmac.octet[2]=0x4c;
	lan_info.gmac.octet[3]=0x86;
	lan_info.gmac.octet[4]=0x70;
	lan_info.gmac.octet[5]=0x01;
	lan_info.intf_vlan_id=9;
	lan_info.ip_addr=htonl(0xc0a80101);

	lan_info.ip_network_mask=htonl(0xffffff00); 

	lan_info.mtu=1500;
	lan_info.port_mask.portmask=((1<<RTK_RG_PORT0)|(1<<RTK_RG_PORT1)|(1<<RTK_RG_PORT2)|(1<<RTK_RG_PORT3));
	lan_info.untag_mask.portmask=((1<<RTK_RG_MAC_PORT0)|(1<<RTK_RG_MAC_PORT1)|(1<<RTK_RG_MAC_PORT2)|(1<<RTK_RG_MAC_PORT_CPU));


	assert_ok(rtk_rg_lanInterface_add(&lan_info,&lanIntfIdx));

	memset(&wan_info,0,sizeof(wan_info));
	wan_info.egress_vlan_id=8;
	wan_info.egress_vlan_pri=0;
	wan_info.egress_vlan_tag_on=0;
	wan_info.gmac.octet[0]=0x00;
	wan_info.gmac.octet[1]=0xe0;
	wan_info.gmac.octet[2]=0x4c;
	wan_info.gmac.octet[3]=0x86;
	wan_info.gmac.octet[4]=0x70;
	wan_info.gmac.octet[5]=0x02;
	
	
	//_rtk_rg_strtomac((void *)&wan_info.gmac.octet[0], romeWAN1_HOST_mac);

	wan_info.port_binding_mask.portmask=0;
	wan_info.wan_port_idx=RTK_RG_PORT_PON;
//	wan_info.wan_port_idx=RTK_RG_MAC_PORT3;	
	wan_info.wan_type=RTK_RG_PPPoE;
	assert_ok(rtk_rg_wanInterface_add(&wan_info,&wanIntfIdx));
	assert_ok(rtk_rg_pppoeClientInfoBeforeDial_set(wanIntfIdx, &pppoeClientInfoB));

	pppoeClientInfoA.hw_info.napt_enable = 1;
	pppoeClientInfoA.hw_info.ip_addr = htonl(0xc0a805c9);
	pppoeClientInfoA.hw_info.ip_network_mask = htonl(0xffffff00); 
	pppoeClientInfoA.hw_info.ipv4_default_gateway_on = 1;
	pppoeClientInfoA.hw_info.gateway_ipv4_addr = htonl(0xc0a805c8);
	pppoeClientInfoA.hw_info.mtu = 1500;
	pppoeClientInfoA.sessionId = 5;
	pppoeClientInfoA.hw_info.gw_mac_auto_learn_for_ipv4 = 0;
	pppoeClientInfoA.hw_info.gateway_mac_addr_for_ipv4.octet[0]=0x00;
	pppoeClientInfoA.hw_info.gateway_mac_addr_for_ipv4.octet[1]=0x00;
	pppoeClientInfoA.hw_info.gateway_mac_addr_for_ipv4.octet[2]=0x00;
	pppoeClientInfoA.hw_info.gateway_mac_addr_for_ipv4.octet[3]=0x00;
	pppoeClientInfoA.hw_info.gateway_mac_addr_for_ipv4.octet[4]=0x00;
	pppoeClientInfoA.hw_info.gateway_mac_addr_for_ipv4.octet[5]=0x02;
	//_rtk_rg_strtomac((void *)&pppoeClientInfoA.hw_info.gateway_mac_addr.octet[0], romeWAN2_HOST_mac);

	assert_ok(rtk_rg_pppoeClientInfoAfterDial_set(wanIntfIdx, &pppoeClientInfoA));
	return 0;
}


//#define PPPOE_WAN_TEST
#define STATIC_WAN_TEST
//#define LUKE_MAC_IP
//#define LUKE_PPB_TEST
//#define LUKE_ALG_TEST
//#define PORTBINDING_TEST
//#define LUKE_IPV6_TEST
//#define LUKE_WAN_PON_PORT
//#define LUKE_FIND_TEST

int rtk_rg_fwdEngine_start(void)
{
#if defined(LUKE_PPB_TEST)
	int i,retval;
	//rtk_l34_pppoe_entry_t pppEntry;
#endif
#ifdef LUKE_ALG_TEST
	rtk_rg_alg_type_t algGet;
#endif
	int lanIntfIdx;
	int wanIntfIdx;
	rtk_rg_lanIntfConf_t lan_info;
	rtk_rg_wanIntfConf_t wan_info;
	rtk_rg_ipStaticInfo_t staticInfo;
#ifdef 	PPPOE_WAN_TEST
	rtk_rg_pppoeClientInfoBeforeDial_t pppoeBefore;
	rtk_rg_pppoeClientInfoAfterDial_t pppoeAfter;
#endif	
#ifdef LUKE_IPV6_TEST
	rtk_rg_neighborEntry_t neighborEntry;
	rtk_rg_neighborInfo_t neighborInfo;

	rtk_rg_macEntry_t macEntry;
	int l2Idx,ret,neighbor_valid_idx;
	unsigned int tmpInteger;
#endif
#ifdef LUKE_FIND_TEST
	rtk_rg_intfInfo_t findIntfInfo;
	int intf_valid_idx;
#endif

	assert_ok(rtk_rg_initParam_set(NULL));

#ifdef LUKE_PPB_TEST	
	//DSCP remarking enable
	for(i=0;i<7;i++)
	{
		//retval = ioal_mem32_write(0x231cc+(i*4),0x1);
		//assert(retval==RT_ERR_OK);
		//DEBUG("DSCP on %d",i);

		retval = rtk_qos_dscpRemarkEnable_set(i,DISABLED);
		assert(retval==RT_ERR_OK);

		retval = rtk_qos_1pRemarkEnable_set(i,DISABLED);
		assert(retval==RT_ERR_OK);

		//pppEntry.sessionID=0x5+i;
		//retval = RTK_L34_PPPOETABLE_SET(i,&pppEntry);
		//DEBUG("PPPOE TABLE %d is %d",i,pppEntry.sessionID);
	}
#endif	

	memset(&lan_info,0,sizeof(lan_info));
	lan_info.gmac.octet[0]=0x00;
	lan_info.gmac.octet[1]=0xa1;
#ifdef LUKE_MAC_IP
	lan_info.gmac.octet[1]=0x21;
#endif
	lan_info.gmac.octet[2]=0x4c;
	lan_info.gmac.octet[3]=0x86;
	lan_info.gmac.octet[4]=0x70;
	lan_info.gmac.octet[5]=0x01;
	lan_info.intf_vlan_id=9;
	lan_info.ip_addr=htonl(0xc0a80101); //192.168.1.1
	lan_info.ip_network_mask=htonl(0xffffff00);
	lan_info.mtu=1500;
	lan_info.port_mask.portmask=((1<<RTK_RG_PORT0)|(1<<RTK_RG_PORT1)|(1<<RTK_RG_PORT2)|(1<<RTK_RG_PORT3));
	lan_info.untag_mask.portmask=((1<<RTK_RG_MAC_PORT0)|(1<<RTK_RG_MAC_PORT1)|(1<<RTK_RG_MAC_PORT2)|(1<<RTK_RG_MAC_PORT_CPU));

#ifdef LUKE_PPB_TEST	
	//lan_info.pppoe_passThrough=1;
#endif
#if defined(PORTBINDING_TEST) || defined(LUKE_PPB_TEST) || defined(LUKE_MAC_IP)
	lan_info.intf_vlan_id=100;
	lan_info.ip_network_mask=htonl(0xffffff00);
	lan_info.port_mask.portmask=((1<<RTK_RG_PORT0)|(1<<RTK_RG_PORT1)|(1<<RTK_RG_PORT2));
#endif
#ifdef LUKE_IPV6_TEST
	lan_info.ip_version=IPVER_V4V6;
	tmpInteger=htonl(0xfe800000);
	memcpy(lan_info.ipv6_addr.ipv6_addr,&tmpInteger,4);
	tmpInteger=htonl(0x00000101);
	memcpy(lan_info.ipv6_addr.ipv6_addr+4,&tmpInteger,4);
	tmpInteger=htonl(0x12345678);
	memcpy(lan_info.ipv6_addr.ipv6_addr+8,&tmpInteger,4);
	tmpInteger=htonl(0x9abcdeff);
	memcpy(lan_info.ipv6_addr.ipv6_addr+12,&tmpInteger,4);
	lan_info.ipv6_network_mask_length=64;
#endif
	assert_ok(rtk_rg_lanInterface_add(&lan_info,&lanIntfIdx));

	memset(&wan_info,0,sizeof(wan_info));
	wan_info.egress_vlan_id=8;
	wan_info.egress_vlan_pri=0;
	wan_info.egress_vlan_tag_on=0;
	wan_info.gmac.octet[0]=0x00;
	wan_info.gmac.octet[1]=0xa1;
#ifdef LUKE_MAC_IP
	wan_info.gmac.octet[1]=0x21;
	wan_info.egress_vlan_id=200;
#endif
	wan_info.gmac.octet[2]=0x4c;
	wan_info.gmac.octet[3]=0x86;
	wan_info.gmac.octet[4]=0x70;
	wan_info.gmac.octet[5]=0x02;
	wan_info.port_binding_mask.portmask=0;
	wan_info.wan_port_idx=RTK_RG_MAC_PORT3;
#ifdef LUKE_WAN_PON_PORT
	wan_info.wan_port_idx=RTK_RG_MAC_PORT_PON;
#endif
#ifdef STATIC_WAN_TEST
	wan_info.wan_type=RTK_RG_STATIC;
#endif

#ifdef PPPOE_WAN_TEST
	wan_info.wan_type=RTK_RG_PPPoE;
#endif
	assert_ok(rtk_rg_wanInterface_add(&wan_info,&wanIntfIdx));

	memset(&staticInfo,0,sizeof(staticInfo));
	staticInfo.ipv4_default_gateway_on=1;
	staticInfo.gateway_ipv4_addr=htonl(0xc0a89675); //wan ip:192.168.150.117
	staticInfo.ip_addr=htonl(0xc0a89674); //wan ip:192.168.150.116
	
	staticInfo.ip_network_mask=htonl(0xffffff00); 
	staticInfo.mtu=1500;
	staticInfo.napt_enable=1;	
	staticInfo.gw_mac_auto_learn_for_ipv4=1;

#ifdef LUKE_MAC_IP
	staticInfo.gateway_ipv4_addr=htonl(0xc0a89727); //wan ip:192.168.151.39
	staticInfo.ip_addr=htonl(0xc0a89720); //wan ip:192.168.151.32
	staticInfo.ip_network_mask=htonl(0xfffffff8); 
#endif
#ifdef LUKE_IPV6_TEST
	staticInfo.ip_version=IPVER_V4V6;
	staticInfo.ipv6_default_gateway_on=1;
	tmpInteger=htonl(0xfe800000);
	memcpy(staticInfo.ipv6_addr.ipv6_addr,&tmpInteger,4);
	tmpInteger=htonl(0x01010101);
	memcpy(staticInfo.ipv6_addr.ipv6_addr+4,&tmpInteger,4);
	tmpInteger=htonl(0x44444444);
	memcpy(staticInfo.ipv6_addr.ipv6_addr+8,&tmpInteger,4);
	tmpInteger=htonl(0x55555555);
	memcpy(staticInfo.ipv6_addr.ipv6_addr+12,&tmpInteger,4);
	staticInfo.ipv6_mask_length=64;
	
	/*tmpInteger=htonl(0x00556611);
	memcpy(staticInfo.gateway_mac_addr_for_ipv6.octet,&tmpInteger,4);
	staticInfo.gateway_mac_addr_for_ipv6.octet[4]=0x22;
	staticInfo.gateway_mac_addr_for_ipv6.octet[5]=0x33;*/
	
	tmpInteger=htonl(0xfe800000);
	memcpy(staticInfo.gateway_ipv6_addr.ipv6_addr,&tmpInteger,4);
	tmpInteger=htonl(0x01010101);
	memcpy(staticInfo.gateway_ipv6_addr.ipv6_addr+4,&tmpInteger,4);
	tmpInteger=htonl(0x22222222);
	memcpy(staticInfo.gateway_ipv6_addr.ipv6_addr+8,&tmpInteger,4);
	tmpInteger=htonl(0x33333333);
	memcpy(staticInfo.gateway_ipv6_addr.ipv6_addr+12,&tmpInteger,4);
	staticInfo.gw_mac_auto_learn_for_ipv6=0;
	tmpInteger=htonl(0xc860008c);
	memcpy(staticInfo.gateway_mac_addr_for_ipv6.octet,&tmpInteger,4);
	staticInfo.gateway_mac_addr_for_ipv6.octet[4]=0x10;
	staticInfo.gateway_mac_addr_for_ipv6.octet[5]=0x0d;
#endif

#ifdef STATIC_WAN_TEST	
	assert_ok(rtk_rg_staticInfo_set(wanIntfIdx,&staticInfo));
#endif

#ifdef LUKE_IPV6_TEST
	//mac add
	bzero(&macEntry,sizeof(rtk_rg_macEntry_t));
	tmpInteger=htonl(0x00113333);
	memcpy(macEntry.mac.octet,&tmpInteger,4);
	macEntry.mac.octet[4]=0x33;
	macEntry.mac.octet[5]=0x33;
	macEntry.fid=LAN_FID;
	macEntry.static_entry=1;
	macEntry.arp_used=1;
	assert_ok(rtk_rg_macEntry_add(&macEntry,&l2Idx));
	//find for nothing
	bzero(&neighborInfo,sizeof(rtk_rg_neighborInfo_t));
	neighbor_valid_idx=-1;
	tmpInteger=htonl(0x12345678);
	memcpy(neighborInfo.neighborEntry.interfaceId,&tmpInteger,4);
	tmpInteger=htonl(0x9abcdeff);
	memcpy(neighborInfo.neighborEntry.interfaceId+4,&tmpInteger,4);
	neighborInfo.neighborEntry.matchRouteIdx=0;
	ret=rtk_rg_neighborEntry_find(&neighborInfo,&neighbor_valid_idx);
	DEBUG("ret of first find: %x, idx=%d",ret,neighbor_valid_idx);
	//neighbor table add
	bzero(&neighborEntry,sizeof(rtk_rg_neighborEntry_t));
	tmpInteger=htonl(0x12345678);
	memcpy(neighborEntry.interfaceId,&tmpInteger,4);
	tmpInteger=htonl(0x9abcdeff);
	memcpy(neighborEntry.interfaceId+4,&tmpInteger,4);
	neighborEntry.l2Idx=l2Idx;
	neighborEntry.matchRouteIdx=0;
	neighborEntry.staticEntry=1;
	neighborEntry.valid=1;
	assert_ok(rtk_rg_neighborEntry_add(&neighborEntry,&l2Idx));
	//find for something
	bzero(&neighborInfo,sizeof(rtk_rg_neighborInfo_t));
	neighbor_valid_idx=-1;
	memcpy(neighborInfo.neighborEntry.interfaceId,neighborEntry.interfaceId,8);
	neighborInfo.neighborEntry.matchRouteIdx=neighborEntry.matchRouteIdx;
	ret=rtk_rg_neighborEntry_find(&neighborInfo,&neighbor_valid_idx);
	DEBUG("ret of second find: %x, idx=%d",ret,neighbor_valid_idx);
	//delete neighbor
	ret=rtk_rg_neighborEntry_del(neighbor_valid_idx);
	DEBUG("ret of del: %x",ret);
	//find for nothing
	bzero(&neighborInfo,sizeof(rtk_rg_neighborInfo_t));
	neighbor_valid_idx=-1;
	tmpInteger=htonl(0x12345678);
	memcpy(neighborInfo.neighborEntry.interfaceId,&tmpInteger,4);
	tmpInteger=htonl(0x9abcdeff);
	memcpy(neighborInfo.neighborEntry.interfaceId+4,&tmpInteger,4);
	neighborInfo.neighborEntry.matchRouteIdx=0;
	ret=rtk_rg_neighborEntry_find(&neighborInfo,&neighbor_valid_idx);
	DEBUG("ret of last find: %x, idx=%d",ret,neighbor_valid_idx);
#endif

#ifdef LUKE_FIND_TEST
	bzero(&findIntfInfo,sizeof(rtk_rg_intfInfo_t));
	intf_valid_idx=1;
	ret=rtk_rg_intfInfo_find(&findIntfInfo,&intf_valid_idx);
	DEBUG("ret=%d",ret);
	if(memcmp(&findIntfInfo,&rg_db.systemGlobal.interfaceInfo[1].storedInfo,sizeof(rtk_rg_intfInfo_t))==0)
	{
		DEBUG("find interface %d is success!!",intf_valid_idx);
	}
	else
	{
		DEBUG("find interface %d failed...",intf_valid_idx);
	}

	bzero(&findIntfInfo,sizeof(rtk_rg_intfInfo_t));
	intf_valid_idx=-1;
	findIntfInfo.lan_intf.ip_addr=htonl(0xc0a89720);
	ret=rtk_rg_intfInfo_find(&findIntfInfo,&intf_valid_idx);
	DEBUG("ret=%d",ret);
	if(memcmp(&findIntfInfo,&rg_db.systemGlobal.interfaceInfo[intf_valid_idx].storedInfo,sizeof(rtk_rg_intfInfo_t))==0)
	{
		DEBUG("find interface %d by ip 0xc0a89674 is success!!",intf_valid_idx);
	}
	else
	{
		DEBUG("find interface %d by ip 0xc0a89674 failed...",intf_valid_idx);
	}

#ifdef LUKE_IPV6_TEST
	bzero(&findIntfInfo,sizeof(rtk_rg_intfInfo_t));
	intf_valid_idx=-1;
	tmpInteger=htonl(0xfe800000);
	memcpy(findIntfInfo.lan_intf.ipv6_addr.ipv6_addr,&tmpInteger,4);
	tmpInteger=htonl(0x01010101);
	memcpy(findIntfInfo.lan_intf.ipv6_addr.ipv6_addr+4,&tmpInteger,4);
	tmpInteger=htonl(0x44444444);
	memcpy(findIntfInfo.lan_intf.ipv6_addr.ipv6_addr+8,&tmpInteger,4);
	tmpInteger=htonl(0x55555555);
	memcpy(findIntfInfo.lan_intf.ipv6_addr.ipv6_addr+12,&tmpInteger,4);
	ret=rtk_rg_intfInfo_find(&findIntfInfo,&intf_valid_idx);
	DEBUG("ret=%d",ret);
	if(memcmp(&findIntfInfo,&rg_db.systemGlobal.interfaceInfo[intf_valid_idx].storedInfo,sizeof(rtk_rg_intfInfo_t))==0)
	{
		DEBUG("find interface %d by ipv6 0xfe800000010101014444444455555555 is success!!",intf_valid_idx);
	}
	else
	{
		DEBUG("find interface %d by ipv6 0xfe800000010101014444444455555555 failed...",intf_valid_idx);
	}
#endif
#endif

#ifdef PPPOE_WAN_TEST
	memset(&pppoeBefore,0,sizeof(pppoeBefore));
	assert_ok(rtk_rg_pppoeClientInfoBeforeDial_set(wanIntfIdx,&pppoeBefore));
	memset(&pppoeAfter,0,sizeof(pppoeAfter));	
	memcpy(&pppoeAfter.hw_info,&staticInfo,sizeof(staticInfo));
	pppoeAfter.sessionId=50;
	pppoeAfter.hw_info.ipv6_mask_length=128;		//pppoe WAN do not need interface route
	assert_ok(rtk_rg_pppoeClientInfoAfterDial_set(wanIntfIdx,&pppoeAfter));
#endif
//==================================================================
#ifdef PORTBINDING_TEST
	wanIntfIdx=0;

	memset(&wan_info,0,sizeof(wan_info));
	wan_info.egress_vlan_id=300;
	wan_info.egress_vlan_pri=0;
	wan_info.egress_vlan_tag_on=1;
	wan_info.gmac.octet[0]=0x00;
	wan_info.gmac.octet[1]=0xa1;
#ifdef LUKE_MAC_IP
	wan_info.gmac.octet[1]=0x21;
#endif
	wan_info.gmac.octet[2]=0x4c;
	wan_info.gmac.octet[3]=0x86;
	wan_info.gmac.octet[4]=0x70;
	wan_info.gmac.octet[5]=0x03;
	wan_info.port_binding_mask.portmask = 1<<RTK_RG_MAC_PORT1;		//port 1 will bind this wan
	wan_info.wan_port_idx=RTK_RG_MAC_PORT_PON;
#ifdef STATIC_WAN_TEST
	wan_info.wan_type=RTK_RG_STATIC;
#endif

#ifdef PPPOE_WAN_TEST
	wan_info.wan_type=RTK_RG_PPPoE;
#endif

	assert_ok(rtk_rg_wanInterface_add(&wan_info,&wanIntfIdx));

	memset(&staticInfo,0,sizeof(staticInfo));
	staticInfo.ipv4_default_gateway_on=0;
	staticInfo.gateway_ipv4_addr=htonl(0xc0a89675); //wan ip:192.168.150.117
	staticInfo.ip_addr=htonl(0xc0a896f0); //wan ip:192.168.150.240
	staticInfo.ip_network_mask=htonl(0xfffffff8); 
	staticInfo.mtu=1500;
	staticInfo.napt_enable=1;	
	staticInfo.gw_mac_auto_learn=0;
	staticInfo.gateway_mac_addr.octet[0]=0x68;
	staticInfo.gateway_mac_addr.octet[1]=0x05;
	staticInfo.gateway_mac_addr.octet[2]=0xCA;
	staticInfo.gateway_mac_addr.octet[3]=0x0F;
	staticInfo.gateway_mac_addr.octet[4]=0x8B;
	staticInfo.gateway_mac_addr.octet[5]=0x73;
	assert_ok(rtk_rg_staticInfo_set(wanIntfIdx,&staticInfo));
#endif


//test for alg
#ifdef LUKE_ALG_TEST
	assert_ok(rtk_rg_algApps_get(&algGet));
	DEBUG("the algGet is %x",algGet);
	assert_ok(rtk_rg_algApps_set(RTK_RG_ALG_FTP_BIT | RTK_RG_ALG_RTSP_BIT));
	assert_ok(rtk_rg_algApps_get(&algGet));
	DEBUG("after set, the algGet is %x",algGet);
#endif

//test for inbound connection
#ifdef YSLEU_INBOUND_NAPT_TEST
        int lanIntfIdx;
        int wanIntfIdx;
        rtk_rg_lanIntfConf_t lan_info;
        rtk_rg_wanIntfConf_t wan_info;
        rtk_rg_ipStaticInfo_t staticInfo;

        assert_ok(rtk_rg_initParam_set(NULL));
		rtk_rg_rome_driver_init();

        memset(&lan_info,0,sizeof(lan_info));
        lan_info.gmac.octet[0]=0x00;
        lan_info.gmac.octet[1]=0xE0;
        lan_info.gmac.octet[2]=0x4C;
        lan_info.gmac.octet[3]=0x86;
        lan_info.gmac.octet[4]=0x70;
        lan_info.gmac.octet[5]=0x01;
        lan_info.intf_vlan_id=9;
        lan_info.ip_addr=htonl(0xc0a80101); //192.168.1.1
        lan_info.ip_network_mask=htonl(0xffffff00);
        lan_info.mtu=1500;
        lan_info.port_mask.portmask=((1<<RTK_RG_PORT0)|(1<<RTK_RG_PORT1)|(1<<RTK_RG_PORT2));
        lan_info.untag_mask.portmask=((1<<RTK_RG_MAC_PORT0)|(1<<RTK_RG_MAC_PORT1)|(1<<RTK_RG_MAC_PORT2)|(1<<RTK_RG_MAC_PORT_CPU));
        assert_ok(rtk_rg_lanInterface_add(&lan_info,&lanIntfIdx));

        memset(&wan_info,0,sizeof(wan_info));
        wan_info.egress_vlan_id=8;
        wan_info.egress_vlan_pri=0;
        wan_info.egress_vlan_tag_on=0;
#if 1
        wan_info.gmac.octet[0]=0x00;
        wan_info.gmac.octet[1]=0xE0;
        wan_info.gmac.octet[2]=0x4C;
        wan_info.gmac.octet[3]=0x86;
        wan_info.gmac.octet[4]=0x70;
        wan_info.gmac.octet[5]=0x04;
#else
        wan_info.gmac.octet[0]=0x00;
        wan_info.gmac.octet[1]=0x00;
        wan_info.gmac.octet[2]=0x00;
        wan_info.gmac.octet[3]=0x01;
        wan_info.gmac.octet[4]=0x00;
        wan_info.gmac.octet[5]=0x02;
#endif
        wan_info.port_binding_mask.portmask=0;
        wan_info.wan_port_idx=RTK_RG_MAC_PORT3;
#ifdef STATIC_WAN_TEST
        wan_info.wan_type=RTK_RG_STATIC;
#endif

        assert_ok(rtk_rg_wanInterface_add(&wan_info,&wanIntfIdx));

        memset(&staticInfo,0,sizeof(staticInfo));
        staticInfo.ipv4_default_gateway_on=1;
        staticInfo.gateway_ipv4_addr=htonl(0xc0a80216);
        staticInfo.ip_addr=htonl(0xc0a80201);

        staticInfo.ip_network_mask=htonl(0xffffff00);
        staticInfo.mtu=1500;
        staticInfo.napt_enable=1;
        staticInfo.gw_mac_auto_learn_for_ipv4=1;


#ifdef STATIC_WAN_TEST
        assert_ok(rtk_rg_staticInfo_set(wanIntfIdx,&staticInfo));
#endif

#ifdef CONFIG_RG_NAPT_DMZ_SUPPORT
	{
		rtk_rg_dmzInfo_t dmzInfo;
		memset(&dmzInfo,0,sizeof(rtk_rg_dmzInfo_t));
		dmzInfo.enabled = 1;
		dmzInfo.private_ip = 0xc0a8010b;
		rtk_rg_dmzHost_set(1,&dmzInfo);
	}
#endif
#ifdef CONFIG_RG_NAPT_UPNP_SUPPORT
	{
		int upnpIdx;
		rtk_rg_upnpConnection_t upnp;
		memset(&upnp,0,sizeof(rtk_rg_upnpConnection_t));
		upnpIdx=0;
		upnp.is_tcp = 1;
		upnp.valid = 1;
		upnp.wan_intf_idx = 1;
		upnp.gateway_port = 1001;
		upnp.local_ip = 0xc0a8010b;
		upnp.local_port = 10001;
		upnp.limit_remote_ip = 1;
		upnp.limit_remote_port = 0;
		upnp.remote_ip = 0xc0a80216;
		upnp.remote_port = 0;
		upnp.type = UPNP_TYPE_ONESHOT;
		upnp.timeout = 0;
		rtk_rg_upnpConnection_add(&upnp,&upnpIdx);
		memset(&upnp,0,sizeof(rtk_rg_upnpConnection_t));
		upnpIdx=1;
		upnp.is_tcp = 0;
		upnp.valid = 1;
		upnp.wan_intf_idx = 1;
		upnp.gateway_port = 1002;
		upnp.local_ip = 0xc0a8010b;
		upnp.local_port = 10002;
		upnp.limit_remote_ip = 1;
		upnp.limit_remote_port = 0;
		upnp.remote_ip = 0xc0a80216;
		upnp.remote_port = 0;
		upnp.type = UPNP_TYPE_PERSIST;
		upnp.timeout = 0;
		rtk_rg_upnpConnection_add(&upnp,&upnpIdx);
	}
#endif
#ifdef CONFIG_RG_NAPT_VIRTUAL_SERVER_SUPPORT
	{
		int vsIdx;
		rtk_rg_virtualServer_t vs;
		vsIdx=1;
		memset(&vs,0,sizeof(rtk_rg_virtualServer_t));
		vs.gateway_port_start = 2001;
		vs.is_tcp = 1;
		vs.local_ip = 0xc0a8010b;
		vs.local_port_start = 20001;
		vs.mappingPortRangeCnt = 100;
		vs.valid = 1;
		vs.wan_intf_idx = 1;
		rtk_rg_virtualServer_add(&vs,&vsIdx);
		memset(&vs,0,sizeof(rtk_rg_virtualServer_t));
		vsIdx=2;
		vs.gateway_port_start = 2002;
		vs.is_tcp = 0;
		vs.local_ip = 0xc0a8010b;
		vs.local_port_start = 20002;
		vs.mappingPortRangeCnt = 100;
		vs.valid = 1;
		vs.wan_intf_idx = 1;
		rtk_rg_virtualServer_add(&vs,&vsIdx);
	}
#endif

#endif

	return SUCCESS;
}

int start_wan_config_type=0;
int rtk_rg_start_wan_config( struct file *filp, const char *buff,unsigned long len, void *data )
{
	char	*tmpbuf=NULL;	
	char	*strptr=NULL;

	tmpbuf=rtk_rg_malloc(512);
	if(tmpbuf==NULL) return 0;
	
	if (buff && !copy_from_user(tmpbuf, buff, len))	
	{		
		tmpbuf[len] = '\0';				
		strptr=tmpbuf;
	}

	if(strncmp(strptr, "ipoe",4) == 0 || strncmp(strptr, "0",1) == 0){
		rtk_rg_fwdEngine_start();
		start_wan_config_type=0;
	}else if(strncmp(strptr, "pppoe",5) == 0||strncmp(strptr, "1",1) == 0){
		rtk_rg_fwdEngine_pppoe();
		start_wan_config_type=1;
	}else
	{
		printk("(0:ipoe,1:pppoe)\n");	
	}
	rtk_rg_free(tmpbuf);
	return len;
}

int rtk_rg_start_wan_config_type(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len=0;	

	len = sprintf(page, "%d(0:ipoe,1:pppoe)\n",start_wan_config_type);
	
	if (len <= off+count) *eof = 1; 	 
	*start = page + off;	   
	len -= off;		
	if (len>count) len = count; 	 
	if (len<0) len = 0;	  
	return len;
}


void _rtk_rg_hwnatACLManipulate(rtk_enable_t natSwitch)
{
	rtk_rg_aclFilterAndQos_t acl_filter,acl_filter_temp;
	bzero(&acl_filter,sizeof(acl_filter));
	acl_filter.filter_fields |= INGRESS_PORT_BIT;
	acl_filter.ingress_port_mask.portmask = 0x3f;
	acl_filter.action_type = ACL_ACTION_TYPE_TRAP;	
	bzero(&acl_filter_temp,sizeof(acl_filter_temp));

	

	if(natSwitch==ENABLED)
	{	
		assert_ok(rtk_acl_igrRuleEntry_del(0));
	}
	else
	{
		//rtk_l2_portLimitLearningCnt_set(RTK_RG_MAC_PORT_CPU,0); //tysu: patch for l2 pure software (auto learn from CPU port)

		rtk_acl_ingress_entry_t aclRule;
		rtk_acl_field_t aclField;
		
		//add acl[0] for trap all packet to CPU
		bzero(&aclRule,sizeof(aclRule));
		bzero(&aclField,sizeof(aclField));
		aclRule.valid=ENABLED;
		aclRule.index=RESERVED_ACL_TRAP_ALL;
		aclRule.activePorts.bits[0]=0x3f;		
		aclRule.act.enableAct[ACL_IGR_FORWARD_ACT]=ENABLED;
		aclRule.act.forwardAct.act=ACL_IGR_FORWARD_TRAP_ACT;
		assert_ok(rtk_acl_igrRuleEntry_add(&aclRule));

	}
}

int rtk_rg_hwnat_enable( struct file *filp, const char *buff,unsigned long len, void *data )
{
	char	*tmpbuf;	
	char	*strptr=NULL;

	tmpbuf=rtk_rg_malloc(512);
	if(tmpbuf==NULL) return 0;
	
	if (buff && !copy_from_user(tmpbuf, buff, len))	
	{		
		tmpbuf[len] = '\0';				
		strptr=tmpbuf;
	}
	
	if(strncmp(strptr, "1",1) == 0)//enable: remove the ACL rule
	{
		rg_db.systemGlobal.hwnat_enable=ENABLE;
		//from 0 to 1, enable the hwnat function
		//from 2 to 1, enable the hwnat function
		_rtk_rg_hwnatACLManipulate(ENABLED);
	}
	else if(strncmp(strptr, "0",1) == 0)//disable:add a ACL rule to trap all packet to CPU
	{
		//from 1 to 0, disable the hwnat function
		//from 2 to 0, change state only
		if(rg_db.systemGlobal.hwnat_enable==ENABLE)
			_rtk_rg_hwnatACLManipulate(DISABLED);
		rg_db.systemGlobal.hwnat_enable=DISABLE;
	}
	else if(strncmp(strptr, "2",1) == 0)//rtn PS		
	{
		//from 1 to 2, disable the hwnat function
		//from 0 to 2, change state only
		if(rg_db.systemGlobal.hwnat_enable==ENABLE)
			_rtk_rg_hwnatACLManipulate(DISABLED);
		rg_db.systemGlobal.hwnat_enable=RTN_PS;
	}

	rtk_rg_free(tmpbuf);
	return len;

}

int rtk_rg_hwnat_is_enabled(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len=0;	
	
	if(rg_db.systemGlobal.hwnat_enable==ENABLE){
		len = sprintf(page, "1:hwnat ENABLED, fwdEngine ENABLED\n");
	}else if(rg_db.systemGlobal.hwnat_enable==DISABLE){
		len = sprintf(page, "0:hwnat DISABLED, fwdEngine ENABLED\n");
	}else if(rg_db.systemGlobal.hwnat_enable==RTN_PS){
		len = sprintf(page, "2:hwnat DISABLED, fwdEngine DISABLED\n");
	}

	if (len <= off+count) *eof = 1; 	 
	*start = page + off;	   
	len -= off;		
	if (len>count) len = count; 	 
	if (len<0) len = 0;	  
	return len;
}

int rtk_rg_debug_level_show(void)
{
	rtlglue_printf("RomeDriver Debug level=0x%x [0x1:DEBUG=%s][0x2:FIXME=%s][0x4:CALLBACK=%s][0x8:TRACE=%s][0x10:ACL=%s]\n",rg_kernel.debug_level,
		(rg_kernel.debug_level&RTK_RG_DEBUG_LEVEL_DEBUG)?"on":"off",
		(rg_kernel.debug_level&RTK_RG_DEBUG_LEVEL_FIXME)?"on":"off",
		(rg_kernel.debug_level&RTK_RG_DEBUG_LEVEL_CALLBACK)?"on":"off",
		(rg_kernel.debug_level&RTK_RG_DEBUG_LEVEL_TRACE)?"on":"off",
		(rg_kernel.debug_level&RTK_RG_DEBUG_LEVEL_ACL)?"on":"off");
	return 0;
}

#ifdef CONFIG_RG_CALLBACK
int rtk_rg_callback_show(void)
{
	rtlglue_printf("[rg callback functions]\t\t\t\t [Address] \n");
	rtlglue_printf("_rtk_rg_initParameterSetByHwCallBack\t\t 0x%p\n",&_rtk_rg_initParameterSetByHwCallBack);
	rtlglue_printf("_rtk_rg_interfaceAddByHwCallBack\t\t 0x%p\n",&_rtk_rg_interfaceAddByHwCallBack);
	rtlglue_printf("_rtk_rg_interfaceDelByHwCallBack\t\t 0x%p\n",&_rtk_rg_interfaceDelByHwCallBack);
	rtlglue_printf("_rtk_rg_pppoeBeforeDiagByHwCallBack\t\t 0x%p\n",&_rtk_rg_pppoeBeforeDiagByHwCallBack);
	rtlglue_printf("_rtk_rg_dhcpRequestByHwCallBack\t\t\t 0x%p\n",&_rtk_rg_dhcpRequestByHwCallBack);
	rtlglue_printf("_rtk_rg_arpAddByHwCallBack\t\t\t 0x%p\n",&_rtk_rg_arpAddByHwCallBack);
	rtlglue_printf("_rtk_rg_arpDelByHwCallBack\t\t\t 0x%p\n",&_rtk_rg_arpDelByHwCallBack);
	rtlglue_printf("_rtk_rg_macAddByHwCallBack\t\t\t 0x%p\n",&_rtk_rg_macAddByHwCallBack);
	rtlglue_printf("_rtk_rg_macDelByHwCallBack\t\t\t 0x%p\n",&_rtk_rg_macDelByHwCallBack);
	rtlglue_printf("_rtk_rg_naptAddByHwCallBack\t\t\t 0x%p\n",&_rtk_rg_naptAddByHwCallBack);
	rtlglue_printf("_rtk_rg_naptDelByHwCallBack\t\t\t 0x%p\n",&_rtk_rg_naptDelByHwCallBack);
	rtlglue_printf("_rtk_rg_routingAddByHwCallBack\t\t\t 0x%p\n",&_rtk_rg_routingAddByHwCallBack);
	rtlglue_printf("_rtk_rg_routingDelByHwCallBack\t\t\t 0x%p\n",&_rtk_rg_routingDelByHwCallBack);
	rtlglue_printf("_rtk_rg_bindingAddByHwCallBack\t\t\t 0x%p\n",&_rtk_rg_bindingAddByHwCallBack);
	rtlglue_printf("_rtk_rg_bindingDelByHwCallBack\t\t\t 0x%p\n",&_rtk_rg_bindingDelByHwCallBack);

	rtlglue_printf("RESERVED_ACL_AFTER=%d\n",RESERVED_ACL_AFTER);

	
	return 0;
}


int rtk_rg_callbackRegist_write( struct file *filp, const char *buff,unsigned long len, void *data )
{
	char	*tmpbuf;	
	tmpbuf=rtk_rg_malloc(8);
	if(tmpbuf==NULL) return 0;
	if (buff && !copy_from_user(tmpbuf, buff, len))	
	{		
		tmpbuf[len] = '\0';
	}

	if(strncmp(tmpbuf, "1",1) == 0){
		rg_db.systemGlobal.callback_regist=ENABLE;
	}else if(strncmp(tmpbuf, "0",1) == 0){
		rg_db.systemGlobal.callback_regist=DISABLE;
	}
	
	rtk_rg_free(tmpbuf);
	return len;

}

int rtk_rg_callbackRegist_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len=0;	
	
	if(rg_db.systemGlobal.callback_regist==ENABLE){
		len = sprintf(page, "1\n");
	}else if(rg_db.systemGlobal.callback_regist==DISABLE){
		len = sprintf(page, "0\n");
	}


	if (len <= off+count) *eof = 1; 	 
	*start = page + off;	   
	len -= off;		
	if (len>count) len = count; 	 
	if (len<0) len = 0;	  
	return len;
}

#endif



int rtk_rg_debug_level_change(struct file *file, const char *buffer, unsigned long count, void *data)
{
	unsigned char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
		rg_kernel.debug_level=simple_strtoul(tmpBuf, NULL, 16);
		rtk_rg_debug_level_show();
		return count;
	}
	return -EFAULT;
}

int rtk_rg_l2HwAgingShow(void)
{
	rtlglue_printf("RomeDriver L2 HW Aging:0x%x\n",rg_kernel.l2_hw_aging);
	return 0;
}

int rtk_rg_l2HwAgingChange(struct file *file, const char *buffer, unsigned long count, void *data)
{
	unsigned char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
		rg_kernel.l2_hw_aging=simple_strtoul(tmpBuf, NULL, 0);
		rtk_rg_l2HwAgingShow();
		return count;
	}
	return -EFAULT;
}


int rtk_rg_traceFilterShow(void)
{
	rtlglue_printf("RomeDriver Trace Filter:0x%x\n",rg_kernel.trace_filter_bitmask);
	if(rg_kernel.trace_filter_bitmask&RTK_RG_DEBUG_TRACE_FILTER_SPA)
		rtlglue_printf("  SPA:%d\n",rg_kernel.trace_filter.spa);
	if(rg_kernel.trace_filter_bitmask&RTK_RG_DEBUG_TRACE_FILTER_DA)
		rtlglue_printf("  DA:%02x:%02x:%02x:%02x:%02x:%02x MASK:%02x:%02x:%02x:%02x:%02x:%02x\n"
		,rg_kernel.trace_filter.dmac.octet[0]
		,rg_kernel.trace_filter.dmac.octet[1]
		,rg_kernel.trace_filter.dmac.octet[2]
		,rg_kernel.trace_filter.dmac.octet[3]
		,rg_kernel.trace_filter.dmac.octet[4]
		,rg_kernel.trace_filter.dmac.octet[5]
		,rg_kernel.trace_filter.dmac_mask.octet[0]
		,rg_kernel.trace_filter.dmac_mask.octet[1]
		,rg_kernel.trace_filter.dmac_mask.octet[2]
		,rg_kernel.trace_filter.dmac_mask.octet[3]
		,rg_kernel.trace_filter.dmac_mask.octet[4]
		,rg_kernel.trace_filter.dmac_mask.octet[5]);
	if(rg_kernel.trace_filter_bitmask&RTK_RG_DEBUG_TRACE_FILTER_SA)
		rtlglue_printf("  SA:%02x:%02x:%02x:%02x:%02x:%02x MASK:%02x:%02x:%02x:%02x:%02x:%02x\n"
		,rg_kernel.trace_filter.smac.octet[0]
		,rg_kernel.trace_filter.smac.octet[1]
		,rg_kernel.trace_filter.smac.octet[2]
		,rg_kernel.trace_filter.smac.octet[3]
		,rg_kernel.trace_filter.smac.octet[4]
		,rg_kernel.trace_filter.smac.octet[5]
		,rg_kernel.trace_filter.smac_mask.octet[0]
		,rg_kernel.trace_filter.smac_mask.octet[1]
		,rg_kernel.trace_filter.smac_mask.octet[2]
		,rg_kernel.trace_filter.smac_mask.octet[3]
		,rg_kernel.trace_filter.smac_mask.octet[4]
		,rg_kernel.trace_filter.smac_mask.octet[5]);
	if(rg_kernel.trace_filter_bitmask&RTK_RG_DEBUG_TRACE_FILTER_ETH)
		rtlglue_printf("  ETH:0x%04x\n",rg_kernel.trace_filter.ethertype);
	if(rg_kernel.trace_filter_bitmask&RTK_RG_DEBUG_TRACE_FILTER_SIP)
		rtlglue_printf("  SIP:%d.%d.%d.%d\n"
		,(rg_kernel.trace_filter.sip>>24)&0xff
		,(rg_kernel.trace_filter.sip>>16)&0xff
		,(rg_kernel.trace_filter.sip>>8)&0xff
		,(rg_kernel.trace_filter.sip)&0xff);
	if(rg_kernel.trace_filter_bitmask&RTK_RG_DEBUG_TRACE_FILTER_DIP)
		rtlglue_printf("  DIP:%d.%d.%d.%d\n"
		,(rg_kernel.trace_filter.dip>>24)&0xff
		,(rg_kernel.trace_filter.dip>>16)&0xff
		,(rg_kernel.trace_filter.dip>>8)&0xff
		,(rg_kernel.trace_filter.dip)&0xff);

	rtlglue_printf("\nexample command:\n  echo \"SPA 7 DA 00:11:22:33:44:55 ff:ff:ff:ff:ff:ff SA 00:11:22:33:44:66 ff:ff:ff:ff:ff:ff ETH 0800 SIP 192.168.1.1 DIP 192.168.1.2\" > /proc/rg/trace_filter\n");
	return 0;
}


int rtk_rg_traceFilterChange(struct file *file, const char *buffer, unsigned long count, void *data)
{
	unsigned char tmpBuf[160] = {0};
	int len = (count > 160) ? 160 : count;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
		//rg_kernel.debug_level=simple_strtoul(tmpBuf, NULL, 16);
		char *strptr,*split_str;		
		rg_kernel.trace_filter_bitmask=0;
		memset(&rg_kernel.trace_filter,0,sizeof(rtk_rgDebugTraceFilter_t));
		tmpBuf[count] = '\0';
		strptr=tmpBuf;

		while(1)
		{
			split_str=strsep(&strptr," ");
next_token:			
			//printk("%d:%s\n",i++,split_str);
			
			if(strcasecmp(split_str,"SPA")==0)
			{
				split_str=strsep(&strptr," ");				
				rg_kernel.trace_filter_bitmask|=RTK_RG_DEBUG_TRACE_FILTER_SPA;
				rg_kernel.trace_filter.spa=simple_strtol(split_str, NULL, 0);
			}
			else if(strcasecmp(split_str,"DA")==0)
			{
				split_str=strsep(&strptr," ");
				rg_kernel.trace_filter_bitmask|=RTK_RG_DEBUG_TRACE_FILTER_DA;
				_rtk_rg_str2mac(split_str,&rg_kernel.trace_filter.dmac);
				if(strptr==NULL) 
				{
					memset(rg_kernel.trace_filter.dmac_mask.octet,0xff,6);
					break;
				}
				
				split_str=strsep(&strptr," ");
				if(strlen(split_str)<10) goto next_token;
				_rtk_rg_str2mac(split_str,&rg_kernel.trace_filter.dmac_mask);	
			}
			else if(strcasecmp(split_str,"SA")==0)
			{
				split_str=strsep(&strptr," ");
				rg_kernel.trace_filter_bitmask|=RTK_RG_DEBUG_TRACE_FILTER_SA;
				_rtk_rg_str2mac(split_str,&rg_kernel.trace_filter.smac);
				if(strptr==NULL) 
				{
					memset(rg_kernel.trace_filter.smac_mask.octet,0xff,6);
					break;
				}				
				split_str=strsep(&strptr," ");
				if(strlen(split_str)<10) goto next_token;
				_rtk_rg_str2mac(split_str,&rg_kernel.trace_filter.smac_mask);	
			}	
			else if(strcasecmp(split_str,"ETH")==0)
			{
				split_str=strsep(&strptr," ");				
				rg_kernel.trace_filter_bitmask|=RTK_RG_DEBUG_TRACE_FILTER_ETH;
				rg_kernel.trace_filter.ethertype=simple_strtol(split_str, NULL, 16);
			}
			else if(strcasecmp(split_str,"SIP")==0)
			{
				char *ip_token,*split_ip_token,j;
				split_str=strsep(&strptr," ");				
				rg_kernel.trace_filter_bitmask|=RTK_RG_DEBUG_TRACE_FILTER_SIP;
				ip_token=split_str;
				rg_kernel.trace_filter.sip=0;
				for(j=0;j<4;j++)
				{
					split_ip_token=strsep(&ip_token,".");
					rg_kernel.trace_filter.sip|=(simple_strtol(split_ip_token, NULL, 0)<<((3-j)<<3));
					if(ip_token==NULL) break;
				}
			}			
			else if(strcasecmp(split_str,"DIP")==0)
			{
				char *ip_token,*split_ip_token,j;
				split_str=strsep(&strptr," ");				
				rg_kernel.trace_filter_bitmask|=RTK_RG_DEBUG_TRACE_FILTER_DIP;
				ip_token=split_str;
				rg_kernel.trace_filter.dip=0;
				for(j=0;j<4;j++)
				{
					split_ip_token=strsep(&ip_token,".");
					rg_kernel.trace_filter.dip|=(simple_strtol(split_ip_token, NULL, 0)<<((3-j)<<3));
					if(ip_token==NULL) break;
				}
			}

			if (strptr==NULL) break;
		}
		rtk_rg_traceFilterShow();
		return count;
	}
	return -EFAULT;
}





//==================================================================
//Temporality add for IPv6 testing
#if 0

/*
 * Convert an ASCII string to a
 * binary representation of mac address
*/
static int32 strToMac(uint8 *pMac, int8 *pStr)
{
	int8 *ptr;
	uint32 k;

	assert (pMac != NULL);
	assert (pStr != NULL);

	bzero(pMac, sizeof(rtk_mac_t));
	ptr = pStr;

	for ( k = 0 ; *ptr ; ptr ++ )
	{
		if (*ptr == ' ')
		{
		} else if ( (*ptr == ':') || (*ptr == '-') )
		{
			k ++;
		} else if ( ('0' <= *ptr) && (*ptr <= '9') )
		{
			pMac[k] = (pMac[k]<<4) + (*ptr-'0');
		} else if ( ('a' <= *ptr) && (*ptr <= 'f') )
		{
			pMac[k] = (pMac[k]<<4) + (*ptr-'a'+10);
		} else if( ('A' <= *ptr) && (*ptr <= 'F') )
		{
			pMac[k] = (pMac[k]<<4) + (*ptr-'A'+10);
		} else
		{
			break;
		}
	}

	if (k != 5)
	{
		return -1;
	}

	return 0;
}

uint8* _rtk_rg_strtomac(rtk_mac_t *mac, int8 *str)
{
	strToMac((uint8*)mac, str);
	return (uint8*)mac;
}


int32 _rtk_rg_testV6NeighborIdxValid(uint64 dip, uint8 rt_idx)
{
	uint8 ifid[8];
	uint16 hashidx=0;
	uint8 cout;
	uint32 neighbor_idx;
	int8 res=-1;
	rtk_ipv6Neighbor_entry_t entry;

	ifid[0]=(dip>>56)&0xff;
	ifid[1]=(dip>>48)&0xff;
	ifid[2]=(dip>>40)&0xff;
	ifid[3]=(dip>>32)&0xff;
	ifid[4]=(dip&0xff000000)>>24;
	ifid[5]=(dip&0xff0000)>>16;
	ifid[6]=(dip&0xff00)>>8;
	ifid[7]=(dip&0xff);
	
	//get hash index
	hashidx = _rtk_rg_IPv6NeighborHash(ifid, rt_idx);

	for(cout=0;cout<8;cout++)
	{
		//check for available
		neighbor_idx = (hashidx<<3)+cout;
		assert_ok(rtk_l34_ipv6NeighborTable_get(neighbor_idx, &entry));
		if(entry.valid==0)
		{
			res=1;
			break;
		}
		else
		{
			bzero((void *)&entry, sizeof(entry));
		}
	}
	if(res==-1)
	{
		printk("the neighbor_idx reached end at %d\n",neighbor_idx);
		return res;
	}
	else
		return neighbor_idx;	
}


int rtk_rg_ipv6_show_routing(void)
{
#ifdef CONFIG_RG_DEBUG
	dump_ipv6_route_table();
#endif
	return 0;
}


int rtk_rg_ipv6_add_defaultRoute(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char 		tmpbuf[96];
	
	char		*strptr;
	unsigned int i,ret,intfIdx,l2Idx,priority;
	char		*tmp_ptr;
	//unsigned long tmp_num;
	
	rtk_ipv6Routing_entry_t routeEntry;
	rtk_l34_netif_entry_t intfEntry;
	rtk_rg_macEntry_t macEntry;
	rtk_l34_nexthop_entry_t nxpEntry;
	rtk_l34_pppoe_entry_t pppoeEntry;

	bzero(&routeEntry,sizeof(rtk_ipv6Routing_entry_t));
	bzero(&intfEntry,sizeof(rtk_l34_netif_entry_t));
	bzero(&macEntry,sizeof(rtk_rg_macEntry_t));
	bzero(&nxpEntry,sizeof(rtk_l34_nexthop_entry_t));
	bzero(&pppoeEntry,sizeof(rtk_l34_pppoe_entry_t));

	if (buffer && !copy_from_user(tmpbuf, buffer, count))
	{
		tmpbuf[count] = '\0';
		//printk("the string you enter is \"%s\"\n",tmpbuf);
	
		strptr=tmpbuf;

		//Get IPv6 Addr
		/*for(i=0;i<4;i++)
		{
			tmp_ptr = strsep(&strptr," ");

			if (tmp_ptr==NULL)
			{
				goto errout;
			}
			tmp_num=simple_strtol(tmp_ptr, NULL, 0);

			routeEntry.ipv6Addr.ipv6_addr[i<<2]  =(tmp_num>>24)&0xff;
			routeEntry.ipv6Addr.ipv6_addr[(i<<2)+1]=(tmp_num>>16)&0xff;
			routeEntry.ipv6Addr.ipv6_addr[(i<<2)+2]=(tmp_num>>8)&0xff;
			routeEntry.ipv6Addr.ipv6_addr[(i<<2)+3]=tmp_num&0xff;
		}*/
		for(i=0;i<16;i++)
			routeEntry.ipv6Addr.ipv6_addr[i]=0;
		
		//Get interface gwMac
		tmp_ptr = strsep(&strptr," ");
		if (tmp_ptr==NULL)
		{
			goto errout;
		}
		_rtk_rg_strtomac((void *)&intfEntry.gateway_mac,tmp_ptr);

		//Get PPPoE ID
		tmp_ptr = strsep(&strptr," ");
		if (tmp_ptr==NULL)
		{
			goto errout;
		}
		pppoeEntry.sessionID=simple_strtol(tmp_ptr, NULL, 0);

		//Get Lut remote WAN port
		tmp_ptr = strsep(&strptr," ");
		if (tmp_ptr==NULL)
		{
			goto errout;
		}
		macEntry.port_idx=simple_strtol(tmp_ptr, NULL, 0);

		//Get Lut remote gwMac
		tmp_ptr = strsep(&strptr," ");
		if (tmp_ptr==NULL)
		{
			goto errout;
		}
		_rtk_rg_strtomac((void *)&macEntry.mac,tmp_ptr);

		//Get WAN's VLAN id
		tmp_ptr = strsep(&strptr," ");
		if (tmp_ptr==NULL)
		{
			goto errout;
		}
		intfEntry.vlan_id=simple_strtol(tmp_ptr, NULL, 0);

		if(intfEntry.vlan_id==0)
			intfEntry.vlan_id=1;
		
		//Get WAN's VLAN priority
		tmp_ptr = strsep(&strptr," ");
		if (tmp_ptr==NULL)
		{
			goto errout;
		}
		priority=simple_strtol(tmp_ptr, NULL, 0);
		if(priority>0 && intfEntry.vlan_id>1)
		{
			rg_db.vlan[intfEntry.vlan_id].priorityEn=1;
			rg_db.vlan[intfEntry.vlan_id].priority=priority;
		}	

		//######################################################
		//set Lut
		macEntry.fid=LAN_FID;
		macEntry.static_entry=1;
		macEntry.arp_used=1;
		ret=rtk_rg_macEntry_add(&macEntry,&l2Idx);
		DEBUG("### add l2[%d]=%02x:%02x:%02x:%02x:%02x:%02x ###\n",l2Idx,macEntry.mac.octet[0],macEntry.mac.octet[1],macEntry.mac.octet[2],
			macEntry.mac.octet[3],macEntry.mac.octet[4],macEntry.mac.octet[5]);
		assert_ok(ret);

		//set interface
		for(i=0;i<MAX_NETIF_SW_TABLE_SIZE;i++)
		{
			if(rg_db.systemGlobal.interfaceInfo[i].valid == 0)
				break;
		}
		if(i==MAX_NETIF_SW_TABLE_SIZE)
		{
			printk("no available interface entry in table...\n");
			goto errout;
		}
		intfIdx=i;	//keep

		intfEntry.valid=1;
		intfEntry.mac_mask=0x7;	//no mask
		//intfEntry.vlan_id=DEFAULT_LAN_VLAN;
		intfEntry.enable_rounting=1;
		intfEntry.mtu=1500;
		ret = RTK_L34_NETIFTABLE_SET(intfIdx, &intfEntry);		
		assert_ok(ret);
		rg_db.systemGlobal.interfaceInfo[intfIdx].valid=1;

		//set pppoe
		if(pppoeEntry.sessionID != 0)
		{
			ret = RTK_L34_PPPOETABLE_SET(intfIdx, &pppoeEntry);
			assert_ok(ret);
		}
		
		//set nexthop
		nxpEntry.ifIdx=intfIdx;
		nxpEntry.type=L34_NH_ETHER;
		if(pppoeEntry.sessionID != 0)
		{
			nxpEntry.type=L34_NH_PPPOE;
			nxpEntry.pppoeIdx=intfIdx;			//using interface idx as PPPoE table idx
		}
		nxpEntry.nhIdx=l2Idx;	//L2 index
		nxpEntry.keepPppoe=0;
		ret = RTK_L34_NEXTHOPTABLE_SET(intfIdx, &nxpEntry);
		assert_ok(ret);
		
		//set ipv6 route
		routeEntry.nhOrIfidIdx=intfIdx;
		routeEntry.type=L34_IPV6_ROUTE_TYPE_GLOBAL;
		routeEntry.valid=1;
		routeEntry.rt2waninf=1;

		ret = RTK_L34_IPV6ROUTINGTABLE_SET(3, &routeEntry);		//Default route
		assert_ok(ret);
		
		return count;
	}
	else
	{
errout:
		printk("interfaceGMAC PPPoEID WANport RemoteMAC\n");
	}

	return count;
}


int rtk_rg_ipv6_add_interfaceRoute(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char 		tmpbuf[96];
	
	char		*strptr;
	unsigned int i,ret,intfIdx,routeIdx;
	char		*tmp_ptr;
	unsigned long tmp_num;
	
	rtk_ipv6Routing_entry_t routeEntry,tmpEty;
	rtk_l34_netif_entry_t intfEntry;
	rtk_rg_macEntry_t macEntry;
	rtk_l34_nexthop_entry_t nxpEntry;
	rtk_l34_pppoe_entry_t pppoeEntry;

	bzero(&routeEntry,sizeof(rtk_ipv6Routing_entry_t));
	bzero(&intfEntry,sizeof(rtk_l34_netif_entry_t));
	bzero(&macEntry,sizeof(rtk_rg_macEntry_t));
	bzero(&nxpEntry,sizeof(rtk_l34_nexthop_entry_t));
	bzero(&pppoeEntry,sizeof(rtk_l34_pppoe_entry_t));

	if (buffer && !copy_from_user(tmpbuf, buffer, count))
	{
		tmpbuf[count] = '\0';
		//printk("the string you enter is \"%s\"\n",tmpbuf);
	
		strptr=tmpbuf;

		//Get IPv6 Addr
		for(i=0;i<4;i++)
		{
			tmp_ptr = strsep(&strptr," ");

			if (tmp_ptr==NULL)
			{
				goto errout;
			}
			tmp_num=simple_strtol(tmp_ptr, NULL, 0);

			routeEntry.ipv6Addr.ipv6_addr[i<<2]  =(tmp_num>>24)&0xff;
			routeEntry.ipv6Addr.ipv6_addr[(i<<2)+1]=(tmp_num>>16)&0xff;
			routeEntry.ipv6Addr.ipv6_addr[(i<<2)+2]=(tmp_num>>8)&0xff;
			routeEntry.ipv6Addr.ipv6_addr[(i<<2)+3]=tmp_num&0xff;
		}

		//Get Prefix len
		tmp_ptr = strsep(&strptr," ");

		if (tmp_ptr==NULL)
		{
			goto errout;
		}
		routeEntry.ipv6PrefixLen=simple_strtol(tmp_ptr, NULL, 0);

		//Get WAN or not
		tmp_ptr = strsep(&strptr," ");

		if (tmp_ptr==NULL)
		{
			goto errout;
		}
		routeEntry.rt2waninf=simple_strtol(tmp_ptr, NULL, 0);

		//Get VLAN id
		if(routeEntry.rt2waninf==1)
		{
			tmp_ptr = strsep(&strptr," ");

			if (tmp_ptr==NULL)
			{
				goto errout;
			}
			intfEntry.vlan_id=simple_strtol(tmp_ptr, NULL, 0);
		}
		else
		{
			//LAN will use 4000
			intfEntry.vlan_id=DEFAULT_LAN_VLAN;
		}
		
		//Get interface gwMac
		tmp_ptr = strsep(&strptr," ");
		if (tmp_ptr==NULL)
		{
			goto errout;
		}
		_rtk_rg_strtomac((void *)&intfEntry.gateway_mac,tmp_ptr);

		//######################################################
		//set interface
		for(i=0;i<MAX_NETIF_SW_TABLE_SIZE;i++)
		{
			if(rg_db.systemGlobal.interfaceInfo[i].valid == 0)
				break;
		}
		if(i==MAX_NETIF_SW_TABLE_SIZE)
		{
			printk("no available interface entry in table...\n");
			goto errout;
		}
		intfIdx=i;	//keep
		
		intfEntry.valid=1;
		intfEntry.mac_mask=0x7;	//no mask
		//intfEntry.vlan_id=DEFAULT_LAN_VLAN;
		intfEntry.enable_rounting=1;
		intfEntry.mtu=1500;
		ret = RTK_L34_NETIFTABLE_SET(intfIdx, &intfEntry);		
		assert_ok(ret);
		rg_db.systemGlobal.interfaceInfo[intfIdx].valid=1;
		
		//set ipv6 route
		for(i=0;i<3;i++)		//last one is for default route
		{
			ret = rtk_l34_ipv6RoutingTable_get(i,&tmpEty);
			assert_ok(ret);
			if(tmpEty.valid==0)
				break;
		}
		if(i==3)
		{
			printk("ipv6 routing table is full...\n");
			goto errout;
		}

		routeIdx=i;		//keep
		
		routeEntry.nhOrIfidIdx=intfIdx;
		routeEntry.type=L34_IPV6_ROUTE_TYPE_LOCAL;
		routeEntry.valid=1;
		ret = RTK_L34_IPV6ROUTINGTABLE_SET(routeIdx, &routeEntry);		//Default route
		assert_ok(ret);
		
		return count;
	}
	else
	{
errout:
		printk("ip_addr1(0x..) ip_addr2(0x..) ip_addr3(0x..) ip_addr4(0x..) prefix wan_or_not interfaceGMAC \n");
	}

	return count;
}

int rtk_rg_ipv6_show_neighbor(void)
{
#ifdef CONFIG_RG_DEBUG
	dump_ipv6_neighbor_table();
#endif
	return 0;
}


int rtk_rg_ipv6_add_neighbor(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char 		tmpbuf[96];
	
	char		*strptr;
	unsigned int ret,l2Idx;
	char		*tmp_ptr;
	unsigned long tmp_num;
	
	rtk_ipv6Routing_entry_t routeEntry;
	rtk_ipv6Neighbor_entry_t neighborEntry;
	rtk_l34_netif_entry_t intfEntry;
	rtk_rg_macEntry_t macEntry;
	rtk_l34_nexthop_entry_t nxpEntry;
	rtk_l34_pppoe_entry_t pppoeEntry;

	bzero(&routeEntry,sizeof(rtk_ipv6Routing_entry_t));
	bzero(&intfEntry,sizeof(rtk_l34_netif_entry_t));
	bzero(&macEntry,sizeof(rtk_rg_macEntry_t));
	bzero(&nxpEntry,sizeof(rtk_l34_nexthop_entry_t));
	bzero(&pppoeEntry,sizeof(rtk_l34_pppoe_entry_t));
	
	if (buffer && !copy_from_user(tmpbuf, buffer, count))
	{
		
		tmpbuf[count] = '\0';
		//printk("the string you enter is \"%s\"\n",tmpbuf);
	
		strptr=tmpbuf;

		//Get IPv6 IFID  (64bits)
		tmp_ptr = strsep(&strptr," ");

		if (tmp_ptr==NULL)
		{
			goto errout;
		}
		tmp_num=simple_strtol(tmp_ptr, NULL, 0);

		neighborEntry.ipv6Ifid=tmp_num;		//high part
		neighborEntry.ipv6Ifid=neighborEntry.ipv6Ifid<<32;

		tmp_ptr = strsep(&strptr," ");

		if (tmp_ptr==NULL)
		{
			goto errout;
		}
		tmp_num=simple_strtol(tmp_ptr, NULL, 0);

		neighborEntry.ipv6Ifid+=tmp_num;		//low part

		//Get RTIdx
		tmp_ptr = strsep(&strptr," ");

		if (tmp_ptr==NULL)
		{
			goto errout;
		}
		neighborEntry.ipv6RouteIdx=simple_strtol(tmp_ptr, NULL, 0);
		
		//Get Lut remote WAN port
		tmp_ptr = strsep(&strptr," ");
		if (tmp_ptr==NULL)
		{
			goto errout;
		}
		macEntry.port_idx=simple_strtol(tmp_ptr, NULL, 0);

		//Get Lut remote gwMac
		tmp_ptr = strsep(&strptr," ");
		if (tmp_ptr==NULL)
		{
			goto errout;
		}
		_rtk_rg_strtomac((void *)&macEntry.mac,tmp_ptr);

		//######################################################
		//set Lut
		macEntry.fid=LAN_FID;
		macEntry.static_entry=1;
		macEntry.arp_used=1;
		ret=rtk_rg_macEntry_add(&macEntry,&l2Idx);
		DEBUG("### add l2[%d]=%02x:%02x:%02x:%02x:%02x:%02x ###\n",l2Idx,macEntry.mac.octet[0],macEntry.mac.octet[1],macEntry.mac.octet[2],
			macEntry.mac.octet[3],macEntry.mac.octet[4],macEntry.mac.octet[5]);
		assert_ok(ret);

		//set neighbor table
		neighborEntry.l2Idx=l2Idx;

		ret = _rtk_rg_testV6NeighborIdxValid(neighborEntry.ipv6Ifid, neighborEntry.ipv6RouteIdx);
		if(ret==-1)
		{
			printk("the neighbor is overflow..failed!\n");
			goto errout;
		}
		else
		{
			ret = RTK_L34_IPV6NEIGHBORTABLE_SET(ret, &neighborEntry);
			assert_ok(ret);
		}


	}
	else
	{
errout:
		printk("ipaddr_1(0x..) ipaddr_2(0x..) Match_RoutingIdx port_idx L2MACAddr\n");
	}
	
	return count;
}
#endif

//==================================================================

void rg_system_proc_init(void){

	struct proc_dir_entry *p,*start;

	if(rg_kernel.proc_rg==NULL)
		rg_kernel.proc_rg = proc_mkdir("rg", NULL); 
/*	
	if ( create_proc_read_entry ("start", 0644, rg_kernel.proc_rg,
			(read_proc_t *)rtk_rg_fwdEngine_start, (void *)NULL) == NULL ) {
		printk("create proc rg/start failed!\n");
	}	
*/
	
	start = create_proc_entry("start", 0644, rg_kernel.proc_rg);
	if (start){
		start->write_proc = (void *)rtk_rg_start_wan_config;
		start->read_proc = (void *)rtk_rg_start_wan_config_type;
	}else{
		printk("create proc rg/start failed!\n");
	}
	p = create_proc_entry("hwnat", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtk_rg_hwnat_enable;
		p->read_proc = (void *)rtk_rg_hwnat_is_enabled;
	}else{
		printk("create proc rg/hwnat failed!\n");
	}

	p = create_proc_entry("debug_level", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtk_rg_debug_level_change;
		p->read_proc = (void *)rtk_rg_debug_level_show;
	}else{
		printk("create proc rg/hwnat failed!\n");
	}

	p = create_proc_entry("trace_filter", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtk_rg_traceFilterChange;
		p->read_proc = (void *)rtk_rg_traceFilterShow;
	}else{
		printk("create proc rg/hwnat failed!\n");
	}	

#ifdef RTK_RG_INGRESS_QOS_TESTING
	p = create_proc_entry("qos_type", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtk_rg_qos_type_sel;
	}else{
		printk("create proc rg/qos_type failed!\n");
	}

	p = create_proc_entry("qos_low_queue_drop", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtk_rg_qos_low_queue_drop;
	}else{
		printk("create proc rg/qos_low_queue_drop failed!\n");
	}
	
	p = create_proc_entry("qos_queue_mode", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtk_rg_qos_queue_mode;
	}else{
		printk("create proc rg/qos_queue_mode failed!\n");
	}
#endif
	p = create_proc_entry("l2_hw_aging", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtk_rg_l2HwAgingChange;
		p->read_proc = (void *)rtk_rg_l2HwAgingShow;
	}else{
		printk("create proc rg/hwnat failed!\n");
	}	

#if 0 //MIB testing
	p = create_proc_entry("mib_port_get", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtt_rg_proc_mibInfo_get;
	}else{
		printk("create proc/rg/mib_port_get failed!\n");
	}

	p = create_proc_entry("mib_port_clear", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtt_rg_proc_mibInfo_clear;
	}else{
		printk("create proc/rg/mib_port_clear failed!\n");
	}

#endif

#if 0 //DoS testing
	p = create_proc_entry("dos_port", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtk_rg_proc_dos_port_set;
		p->read_proc = (void *)rtk_rg_proc_dos_port_get;
	}else{
		printk("create proc/rg/dos_port failed!\n");
	}
	p = create_proc_entry("dos_type", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtk_rg_proc_dos_type_set;
		p->read_proc = (void *)rtk_rg_proc_dos_type_get;
	}else{
		printk("create proc/rg/dos_type failed!\n");
	}
	p = create_proc_entry("dos_flood", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtk_rg_proc_dos_flood_set;
		p->read_proc = (void *)rtk_rg_proc_dos_flood_get;
	}else{
		printk("create proc/rg/dos_flood failed!\n");
	}
#endif
#if 0
	//temporality add for IPv6 testing
	p = create_proc_entry("ip6DefaultRoute", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtk_rg_ipv6_add_defaultRoute;
		p->read_proc = (void *)rtk_rg_ipv6_show_routing;
	}else{
		printk("create proc rg/ip6defaultRoute failed!\n");
	}
	p = create_proc_entry("ip6InterfaceRoute", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtk_rg_ipv6_add_interfaceRoute;
		p->read_proc = (void *)rtk_rg_ipv6_show_routing;
	}else{
		printk("create proc rg/ip6InterfaceRoute failed!\n");
	}
	p = create_proc_entry("ip6Neighbor", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtk_rg_ipv6_add_neighbor;
		p->read_proc = (void *)rtk_rg_ipv6_show_neighbor;
	}else{
		printk("create proc rg/ip6Neighbor failed!\n");
	}
#endif
#ifdef CONFIG_RG_CALLBACK
	p = create_proc_entry("callback", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = NULL;
		p->read_proc = (void *)rtk_rg_callback_show;
	}else{
		printk("create proc rg/callback failed!\n");
	}
#endif

#ifdef CONFIG_RG_CALLBACK
	p = create_proc_entry("callbackRegist", 0644, rg_kernel.proc_rg);
	if (p){
		p->write_proc = (void *)rtk_rg_callbackRegist_write;
		p->read_proc = (void *)rtk_rg_callbackRegist_read;
	}else{
		printk("create proc rg/callbackRegist failed!\n");
	}
#endif


}
#endif
int _rtk_rg_globalVariableReset()
{
	int i;
	rtk_rg_sipDipClassification_t	sipDipClass[MAX_SIP_CLASS][MAX_DIP_CLASS]=
 	/* DIP:             NPI,                   NI,                    LP,                    RP,                    NPE,                  NE */ 		
    {{SIP_DIP_CLASS_ROUTING, SIP_DIP_CLASS_ROUTING, SIP_DIP_CLASS_ROUTING, SIP_DIP_CLASS_NAPT   , SIP_DIP_CLASS_CPU    ,SIP_DIP_CLASS_CPU}, ///NPI
     {SIP_DIP_CLASS_ROUTING, SIP_DIP_CLASS_ROUTING, SIP_DIP_CLASS_ROUTING, SIP_DIP_CLASS_NAT    , SIP_DIP_CLASS_CPU    ,SIP_DIP_CLASS_CPU}, ///NI
     {SIP_DIP_CLASS_ROUTING, SIP_DIP_CLASS_ROUTING, SIP_DIP_CLASS_ROUTING, SIP_DIP_CLASS_ROUTING, SIP_DIP_CLASS_ROUTING,SIP_DIP_CLASS_ROUTING}, ///LP
     {SIP_DIP_CLASS_CPU    , SIP_DIP_CLASS_CPU    , SIP_DIP_CLASS_ROUTING, SIP_DIP_CLASS_ROUTING, SIP_DIP_CLASS_NAPTR  ,SIP_DIP_CLASS_NATR}}; ///RP

	//Clean all rg_db variables!!
	bzero(&rg_db,sizeof(rtk_rg_globalDatabase_t));

	//Reset what we need here
	memcpy(rg_db.sipDipClass,sipDipClass,sizeof(sipDipClass));

#ifdef CONFIG_APOLLO_MODEL	
	//init hwnat to DISABLE
	rg_db.systemGlobal.hwnat_enable=DISABLE;
#else
	//init hwnat to ENABLE
	rg_db.systemGlobal.hwnat_enable=ENABLE;
#endif	

#ifdef __KERNEL__
	for(i=0;i<MAX_NETIF_SW_TABLE_SIZE;i++)
	{
		del_timer(&rg_kernel.arpRequestTimer[i]);
		del_timer(&rg_kernel.neighborDiscoveryTimer[i]);
	}

	//init software arp free link list
	INIT_LIST_HEAD(&rg_db.softwareArpFreeListHead);
	//init software arp head table
	for(i=0;i<MAX_ARP_SW_TABLE_HEAD;i++)
		INIT_LIST_HEAD(&rg_db.softwareArpTableHead[i]);

	for(i=0;i<(MAX_ARP_SW_TABLE_SIZE-MAX_ARP_HW_TABLE_SIZE);i++)
	{
		INIT_LIST_HEAD(&rg_db.softwareArpFreeList[i].arp_list);
		rg_db.softwareArpFreeList[i].routingIdx=-1;
		rg_db.softwareArpFreeList[i].idx=MAX_ARP_HW_TABLE_SIZE+i;		//software index after hw index

		//add free list to free list head
		list_add_tail(&rg_db.softwareArpFreeList[i].arp_list,&rg_db.softwareArpFreeListHead);
	}
	
#endif

	return RT_ERR_RG_OK;
}


void _rtk_rg_str2mac(unsigned char *mac_string,rtk_mac_t *pMacEntry)
{
	int i,j=0,k=0;
	memset(pMacEntry,0,sizeof(rtk_mac_t));
	for(i=0;i<strlen(mac_string);i++)
	{			
		if(mac_string[i]==':') 
		{
			j=0;
			continue;
		}
		else if((mac_string[i]>='A')&&(mac_string[i]<='F'))
			pMacEntry->octet[k]+=(mac_string[i]-'A'+10);
		else if((mac_string[i]>='a')&&(mac_string[i]<='f'))
			pMacEntry->octet[k]+=(mac_string[i]-'a'+10);
		else if((mac_string[i]>='0')&&(mac_string[i]<='9'))
			pMacEntry->octet[k]+=(mac_string[i]-'0');
		else 
			DEBUG("str2mac MAC string parsing error!");
		if(j==0) pMacEntry->octet[k]<<=4;
		if(j==1) k++;
		j++;
	}
}


#ifdef CONFIG_APOLLO_MODEL
int rtk_rg_api_module_init(void)	
#else

int __init rtk_rg_api_module_init(void)
#endif
{	
#ifdef __KERNEL__
	//Clear All Kernel related variables
	bzero(&rg_kernel,sizeof(rtk_rg_globalKernel_t));
	rg_kernel.rxInfoFromWLAN.opts1.bit.fs=1;
	rg_kernel.rxInfoFromWLAN.opts1.bit.ls=1;
	rg_kernel.rxInfoFromWLAN.opts3.bit.src_port_num=6;
	rg_kernel.rxInfoFromWLAN.opts3.bit.dst_port_mask=0x8; //from EXT1
#endif

 	//replace rtk_rg_initParam_set init, for CONFIG_APOLLO_TESTING disabled.
	virtualmacEnable = DISABLE;
#if 0
    assert_ok(rtk_init());
	assert_ok(rtk_l34_init());
	assert_ok(rtk_l2_init());
	assert_ok(rtk_l2_addr_delAll(ENABLED));
#endif
	
#ifdef CONFIG_APOLLO_MODEL	
#else
	rg_system_proc_init();
#endif	
#ifdef CONFIG_RG_DEBUG
	//init proc system
	rg_proc_init();
#endif


#ifdef __KERNEL__
#if defined(CONFIG_RG_NAPT_AUTO_AGEOUT) || defined(CONFIG_RG_LAYER2_SOFTWARE_LEARN) || defined(CONFIG_RG_ARP_AUTO_AGEOUT)
	init_timer(&rg_kernel.fwdEngineHouseKeepingTimer);
	rg_kernel.fwdEngineHouseKeepingTimer.function = rtk_rg_fwdEngineHouseKeepingTimerFunc;
	mod_timer(&rg_kernel.fwdEngineHouseKeepingTimer, jiffies+(RTK_RG_FWDENGINE_HOUSE_KEEP_SECOND*TICKTIME_PERIOD));
#endif
	//init semaphores used in liteRomeDriver
	/*sema_init(&rg_kernel.wanStaticCalled, 1);
	sema_init(&rg_kernel.wanDHCPCalled, 1);
	sema_init(&rg_kernel.wanPPPOEAfterCalled, 1);
	sema_init(&rg_kernel.interfaceLock, 1);*/
	init_MUTEX(&rg_kernel.wanStaticCalled);
	init_MUTEX(&rg_kernel.wanDHCPCalled);
	init_MUTEX(&rg_kernel.wanPPPOEAfterCalled);
	init_MUTEX(&rg_kernel.interfaceLock);
	//init SA learning lock
	//sema_init(&rg_kernel.saLearningLimitLock, 1);
	//init link-down indicator lock
	//sema_init(&rg_kernel.linkChangeHandlerLock, 1);
#endif	

	//Clear all software related variables
	_rtk_rg_globalVariableReset();

	return SUCCESS;
}
#ifdef __KERNEL__
void __exit rtk_rg_api_module_exit(void)
{
}

module_init(rtk_rg_api_module_init);
module_exit(rtk_rg_api_module_exit);

EXPORT_SYMBOL(rtk_rg_multicastDataIngressAlllowedPortMask_get);
EXPORT_SYMBOL(rtk_rg_wanInterface_add);
EXPORT_SYMBOL(rtk_rg_urlFilterString_find);
EXPORT_SYMBOL(rtk_rg_vlanBinding_add);
EXPORT_SYMBOL(rtk_rg_urlFilterString_add);
EXPORT_SYMBOL(rtk_rg_intfInfo_find);
EXPORT_SYMBOL(rtk_rg_initParam_get);
EXPORT_SYMBOL(rtk_rg_multicastDataIngressAlllowedPortMask_set);
EXPORT_SYMBOL(rtk_rg_macEntry_del);
EXPORT_SYMBOL(rtk_rg_interface_del);
EXPORT_SYMBOL(rtk_rg_macEntry_find);
EXPORT_SYMBOL(rtk_rg_vlanBinding_del);
EXPORT_SYMBOL(rtk_rg_naptConnection_del);
EXPORT_SYMBOL(rtk_rg_aclFilterAndQos_del);
EXPORT_SYMBOL(rtk_rg_staticInfo_set);
EXPORT_SYMBOL(rtk_rg_macFilter_add);
EXPORT_SYMBOL(rtk_rg_initParam_set);
EXPORT_SYMBOL(rtk_rg_aclFilterAndQos_find);
EXPORT_SYMBOL(rtk_rg_macFilter_del);
EXPORT_SYMBOL(rtk_rg_virtualServer_add);
EXPORT_SYMBOL(rtk_rg_macEntry_add);
EXPORT_SYMBOL(rtk_rg_upnpConnection_add);
EXPORT_SYMBOL(rtk_rg_pppoeClientInfoAfterDial_set);
EXPORT_SYMBOL(rtk_rg_macFilter_find);
EXPORT_SYMBOL(rtk_rg_upnpConnection_del);
EXPORT_SYMBOL(rtk_rg_arpEntry_del);
EXPORT_SYMBOL(rtk_rg_multicastFlow_find);
EXPORT_SYMBOL(rtk_rg_virtualServer_find);
EXPORT_SYMBOL(rtk_rg_urlFilterString_del);
EXPORT_SYMBOL(rtk_rg_vlanBinding_find);
EXPORT_SYMBOL(rtk_rg_pppoeClientInfoBeforeDial_set);
EXPORT_SYMBOL(rtk_rg_dhcpClientInfo_set);
EXPORT_SYMBOL(rtk_rg_driverVersion_get);
EXPORT_SYMBOL(rtk_rg_multicastFlow_del);
EXPORT_SYMBOL(rtk_rg_virtualServer_del);
EXPORT_SYMBOL(rtk_rg_lanInterface_add);
EXPORT_SYMBOL(rtk_rg_aclFilterAndQos_add);
EXPORT_SYMBOL(rtk_rg_upnpConnection_find);
EXPORT_SYMBOL(rtk_rg_arpEntry_add);
EXPORT_SYMBOL(rtk_rg_naptConnection_find);
EXPORT_SYMBOL(rtk_rg_naptConnection_add);
EXPORT_SYMBOL(rtk_rg_arpEntry_find );
EXPORT_SYMBOL(rtk_rg_multicastFlow_add);

EXPORT_SYMBOL(_rtk_rg_NAPTRemoteHash_get);
EXPORT_SYMBOL(_rtk_rg_arpAndMacEntryAdd);
EXPORT_SYMBOL(_rtk_rg_arpGeneration);
EXPORT_SYMBOL(_rtk_rg_naptConnection_add);
EXPORT_SYMBOL(rg_db);
EXPORT_SYMBOL(_rtk_rg_l3lookup);
EXPORT_SYMBOL(_rtk_rg_naptTcpUdpOutHashIndex);
EXPORT_SYMBOL(_rtk_rg_naptTcpUdpOutHashIndexLookup);
EXPORT_SYMBOL(_rtk_rg_naptTcpUdpInHashIndex);

#endif
