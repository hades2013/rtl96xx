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
 *
 * $Revision: 39386 $
 * $Date: 2013-05-13 17:21:00 +0800 (Mon, 13 May 2013) $
 *
 * Purpose : Definition those XXX command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <hal/common/halctrl.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>

#include <rtk_rg_struct.h>

//#include "../../../../uClinux-dist/linux-2.6.33.2/drivers/net/apollo/romeDriver/rtk_rg_define.h"
//#include "../../../../uClinux-dist/linux-2.6.33.2/drivers/net/apollo/romeDriver/rtk_rg_struct.h"


/* variable */
static rtk_rg_aclFilterAndQos_t acl_parameter;

/* local function */
static uint32 _diag_rg_aclFilter_show_field(rtk_rg_aclFilterAndQos_t *aclPara)
{		
	diag_util_printf("Current Temp ACL-filter entry:\n");
	diag_util_printf("Patterns: \n");	
	
	diag_util_printf("filter_fields:0x%x\n", aclPara->filter_fields);	
	diag_util_printf("ingress_port_mask:0x%x\n", aclPara->ingress_port_mask);	
	diag_util_printf("ingress_dscp:     %d\n", aclPara->ingress_dscp);			
	diag_util_printf("ingress_intf_idx: %d\n", aclPara->ingress_intf_idx);	
	diag_util_printf("egress_intf_idx:  %d\n", aclPara->egress_intf_idx);	
	diag_util_printf("ingress_ethertype:0x%x\n", aclPara->ingress_ethertype);			
	diag_util_printf("ingress_ctag_vid: %d\n", aclPara->ingress_ctag_vid);	
	diag_util_printf("ingress_ctag_pri: %d\n", aclPara->ingress_ctag_pri);	
	diag_util_printf("ingress_smac: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			aclPara->ingress_smac.octet[0],
			aclPara->ingress_smac.octet[1],
			aclPara->ingress_smac.octet[2],
			aclPara->ingress_smac.octet[3],
			aclPara->ingress_smac.octet[4],
			aclPara->ingress_smac.octet[5]);			
	diag_util_printf("ingress_dmac: %02X:%02X:%02X:%02X:%02X:%02X\n",
			aclPara->ingress_dmac.octet[0],
			aclPara->ingress_dmac.octet[1],
			aclPara->ingress_dmac.octet[2],
			aclPara->ingress_dmac.octet[3],
			aclPara->ingress_dmac.octet[4],
			aclPara->ingress_dmac.octet[5]);

	diag_util_printf("ingress_sip_low_bound: %s  ", diag_util_inet_ntoa(acl_parameter.ingress_src_ipv4_addr_start));
	diag_util_printf("ingress_sip_up_bound:  %s \n", diag_util_inet_ntoa(acl_parameter.ingress_src_ipv4_addr_end));

	diag_util_printf("ingress_dip_low_bound: %s  ", diag_util_inet_ntoa(acl_parameter.ingress_dest_ipv4_addr_start));
	diag_util_printf("ingress_dip_up_bound:  %s \n", diag_util_inet_ntoa(acl_parameter.ingress_dest_ipv4_addr_end));


	diag_util_printf("ingress_src_ipv6_addr_start: %02X%02X:",acl_parameter.ingress_src_ipv6_addr_start[0],acl_parameter.ingress_src_ipv6_addr_start[1]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_start[2],acl_parameter.ingress_src_ipv6_addr_start[3]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_start[4],acl_parameter.ingress_src_ipv6_addr_start[5]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_start[6],acl_parameter.ingress_src_ipv6_addr_start[7]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_start[8],acl_parameter.ingress_src_ipv6_addr_start[9]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_start[10],acl_parameter.ingress_src_ipv6_addr_start[11]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_start[12],acl_parameter.ingress_src_ipv6_addr_start[13]);
	diag_util_printf(":%02X%02X: \n",acl_parameter.ingress_src_ipv6_addr_start[14],acl_parameter.ingress_src_ipv6_addr_start[15]);

	diag_util_printf("ingress_src_ipv6_addr_end: %02X%02X:",acl_parameter.ingress_src_ipv6_addr_end[0],acl_parameter.ingress_src_ipv6_addr_end[1]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_end[2],acl_parameter.ingress_src_ipv6_addr_end[3]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_end[4],acl_parameter.ingress_src_ipv6_addr_end[5]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_end[6],acl_parameter.ingress_src_ipv6_addr_end[7]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_end[8],acl_parameter.ingress_src_ipv6_addr_end[9]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_end[10],acl_parameter.ingress_src_ipv6_addr_end[11]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_src_ipv6_addr_end[12],acl_parameter.ingress_src_ipv6_addr_end[13]);
	diag_util_printf(":%02X%02X: \n",acl_parameter.ingress_src_ipv6_addr_end[14],acl_parameter.ingress_src_ipv6_addr_end[15]);

	diag_util_printf("ingress_dest_ipv6_addr_start: %02X%02X:",acl_parameter.ingress_dest_ipv6_addr_start[0],acl_parameter.ingress_dest_ipv6_addr_start[1]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_start[2],acl_parameter.ingress_dest_ipv6_addr_start[3]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_start[4],acl_parameter.ingress_dest_ipv6_addr_start[5]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_start[6],acl_parameter.ingress_dest_ipv6_addr_start[7]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_start[8],acl_parameter.ingress_dest_ipv6_addr_start[9]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_start[10],acl_parameter.ingress_dest_ipv6_addr_start[11]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_start[12],acl_parameter.ingress_dest_ipv6_addr_start[13]);
	diag_util_printf(":%02X%02X: \n",acl_parameter.ingress_dest_ipv6_addr_start[14],acl_parameter.ingress_dest_ipv6_addr_start[15]);

	diag_util_printf("ingress_dest_ipv6_addr_end: %02X%02X:",acl_parameter.ingress_dest_ipv6_addr_end[0],acl_parameter.ingress_dest_ipv6_addr_end[1]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_end[2],acl_parameter.ingress_dest_ipv6_addr_end[3]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_end[4],acl_parameter.ingress_dest_ipv6_addr_end[5]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_end[6],acl_parameter.ingress_dest_ipv6_addr_end[7]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_end[8],acl_parameter.ingress_dest_ipv6_addr_end[9]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_end[10],acl_parameter.ingress_dest_ipv6_addr_end[11]);
	diag_util_printf(":%02X%02X:",acl_parameter.ingress_dest_ipv6_addr_end[12],acl_parameter.ingress_dest_ipv6_addr_end[13]);
	diag_util_printf(":%02X%02X: \n",acl_parameter.ingress_dest_ipv6_addr_end[14],acl_parameter.ingress_dest_ipv6_addr_end[15]);


	diag_util_printf("ingress_src_l4_port_low_bound: %d  ", acl_parameter.ingress_src_l4_port_start);
	diag_util_printf("ingress_src_l4_port_up_bound:  %d \n", acl_parameter.ingress_src_l4_port_end);

	diag_util_printf("ingress_dest_l4_port_low_bound: %d  ", acl_parameter.ingress_dest_l4_port_start);
	diag_util_printf("ingress_dest_l4_port_up_bound:  %d \n", acl_parameter.ingress_dest_l4_port_end);


	diag_util_printf("egress_sip_low_bound: %s  ", diag_util_inet_ntoa(acl_parameter.egress_src_ipv4_addr_start));
	diag_util_printf("egress_sip_up_bound:  %s \n", diag_util_inet_ntoa(acl_parameter.egress_src_ipv4_addr_end));

	diag_util_printf("egress_dip_low_bound: %s  ", diag_util_inet_ntoa(acl_parameter.egress_dest_ipv4_addr_start));
	diag_util_printf("egress_dip_up_bound:  %s \n", diag_util_inet_ntoa(acl_parameter.egress_dest_ipv4_addr_end));

	diag_util_printf("egress_src_l4_port_low_bound: %d  ", acl_parameter.egress_src_l4_port_start);
	diag_util_printf("egress_src_l4_port_up_bound:  %d \n", acl_parameter.egress_src_l4_port_end);

	diag_util_printf("egress_dest_l4_port_low_bound: %d  ", acl_parameter.egress_dest_l4_port_start);
	diag_util_printf("egress_dest_l4_port_up_bound:  %d \n", acl_parameter.egress_dest_l4_port_end);

	if(acl_parameter.filter_fields & INGRESS_L4_TCP_BIT){
		diag_util_printf("l4-protocal: tcp \n");
	}else if(acl_parameter.filter_fields & INGRESS_L4_UDP_BIT){
		diag_util_printf("l4-protocal: udp \n");
	}else if(acl_parameter.filter_fields & INGRESS_L4_ICMP_BIT){
		diag_util_printf("l4-protocal: icmp \n");
	}else{
		diag_util_printf("l4-protocal: not care \n");
	}


	diag_util_printf("\n");
	diag_util_printf("Actions: \n");
	switch(acl_parameter.action_type){
		case ACL_ACTION_TYPE_DROP:
			diag_util_printf("action_type: ACL_ACTION_TYPE_DROP \n");
			break;
		case ACL_ACTION_TYPE_PERMIT:
			diag_util_printf("action type: ACL_ACTION_TYPE_PERMIT \n");
			break;
		case ACL_ACTION_TYPE_TRAP:
			diag_util_printf("action type: ACL_ACTION_TYPE_TRAP \n");
			break;
		case ACL_ACTION_TYPE_TRAP_TO_PS:
			diag_util_printf("action type: ACL_ACTION_TYPE_TRAP_TO_PS \n");
			break;
		case ACL_ACTION_TYPE_QOS:
			diag_util_printf("action type: ACL_ACTION_TYPE_QOS \n");
			diag_util_printf("qos_actions_bits: 0x%x\n",acl_parameter.qos_actions);
			diag_util_printf("dot1p_remarking_pri: %d \n",acl_parameter.action_dot1p_remarking_pri);
			diag_util_printf("ip_precedence_remarking_pri: %d\n",acl_parameter.action_ip_precedence_remarking_pri);
			diag_util_printf("dot1p_remarking_pri: %d\n",acl_parameter.action_dscp_remarking_pri);
			diag_util_printf("queue_id: %d\n",acl_parameter.action_queue_id);
			diag_util_printf("share_meter: %d\n",acl_parameter.action_share_meter);		
			break;
	}
	
	return CPARSER_OK;
	
}




/*
 * rg show acl-filter
 */
cparser_result_t
cparser_cmd_rg_show_acl_filter(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	_diag_rg_aclFilter_show_field(&acl_parameter);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_acl_filter */

/*
 * rg clear acl-filter
 */
cparser_result_t
cparser_cmd_rg_clear_acl_filter(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&acl_parameter, 0x0, sizeof(rtk_rg_aclFilterAndQos_t));
	diag_util_mprintf("acl-filter entry clear!\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_acl_filter */

/*
 * rg add acl-filter entry
 */
cparser_result_t
cparser_cmd_rg_add_acl_filter_entry(
    cparser_context_t *context)
{
	int ret=0;
	int acl_filter_idx=0;
	
    DIAG_UTIL_PARAM_CHK();
	ret = rtk_rg_aclFilterAndQos_add(&acl_parameter, &acl_filter_idx);
	if(ret == RT_ERR_RG_OK )
		diag_util_mprintf("add acl-filter entry[%d] success!\n",acl_filter_idx);
	else
		diag_util_mprintf("add acl-filter entry failed! (ret=%d) \n",ret);
	

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_acl_filter_entry */

/*
 * rg get acl-filter entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_get_acl_filter_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret=0;
	int search_start_idx = *entry_index_ptr;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	
	osal_memset(&acl_parameter, 0x0, sizeof(rtk_rg_aclFilterAndQos_t));
	ret = rtk_rg_aclFilterAndQos_find(&acl_parameter,&search_start_idx);
	if(ret == RT_ERR_RG_OK){
		if(search_start_idx == (*entry_index_ptr)){
			_diag_rg_aclFilter_show_field(&acl_parameter);
			diag_util_mprintf("get acl-filter entry[%d] success!\n",*entry_index_ptr);
		}else{
			diag_util_mprintf("get acl-filter entry[%d] failed! acl-filter entry[%d] is empty!\n",*entry_index_ptr);
		}
		
	}else{
		diag_util_mprintf("find acl-filter entry[%d] failed!\n",*entry_index_ptr);
	}

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_get_acl_filter_entry_entry_index */

/*
 * rg del acl-filter entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_del_acl_filter_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
	ret = rtk_rg_aclFilterAndQos_del(*entry_index_ptr);

	if(ret == RT_ERR_RG_OK){
		diag_util_mprintf("del acl-filter entry[%d] success!\n",*entry_index_ptr);
	}else{
		diag_util_mprintf("del acl-filter entry[%d] failed!\n",*entry_index_ptr);
	}

	

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_del_acl_filter_entry_entry_index */

/*
 * rg set acl-filter pattern ingress_port_mask <HEX:portmask>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_port_mask_portmask(
    cparser_context_t *context,
    uint32_t  *portmask_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	acl_parameter.filter_fields |= INGRESS_PORT_BIT;
	acl_parameter.ingress_port_mask.portmask= *portmask_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_port_mask_portmask */

/*
 * rg set acl-filter pattern ingress_dscp <UINT:dscp>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_dscp_dscp(
    cparser_context_t *context,
    uint32_t  *dscp_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*dscp_ptr > RTK_VALUE_OF_DSCP_MAX), CPARSER_ERR_INVALID_PARAMS);
	acl_parameter.filter_fields |= INGRESS_DSCP_BIT;	
	acl_parameter.ingress_dscp= *dscp_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_dscp_dscp */

/*
 * rg set acl-filter pattern ingress_intf_idx <UINT:igr_intf>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_intf_idx_igr_intf(
    cparser_context_t *context,
    uint32_t  *igr_intf_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	acl_parameter.filter_fields |= INGRESS_INTF_BIT;	
	acl_parameter.ingress_intf_idx= *igr_intf_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_intf_idx_igr_intf */

/*
 * rg set acl-filter pattern egress_intf_idx <UINT:egr_intf>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_egress_intf_idx_egr_intf(
    cparser_context_t *context,
    uint32_t  *egr_intf_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	acl_parameter.egress_intf_idx= *egr_intf_ptr;
	acl_parameter.filter_fields |= EGRESS_INTF_BIT;	

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_egress_intf_idx_egr_intf */

/*
 * rg set acl-filter pattern ingress_ethertype <HEX:ethtype>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_ethertype_ethtype(
    cparser_context_t *context,
    uint32_t  *ethtype_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_PARAM_RANGE_CHK((*ethtype_ptr > RTK_ETHERTYPE_MAX), CPARSER_ERR_INVALID_PARAMS);
	acl_parameter.filter_fields |= INGRESS_ETHERTYPE_BIT;	
	acl_parameter.ingress_ethertype= *ethtype_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_ethertype_ethtype */

/*
 * rg set acl-filter pattern ingress_ctag_vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_ctag_vid_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_PARAM_RANGE_CHK((*vid_ptr > RTK_VLAN_ID_MAX), CPARSER_ERR_INVALID_PARAMS);
	acl_parameter.filter_fields |= INGRESS_CTAG_VID_BIT;
	acl_parameter.ingress_ctag_vid= *vid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_ctag_vid_vid */

/*
 * rg set acl-filter pattern ingress_ctag_pri <UINT:pri>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_ctag_pri_pri(
    cparser_context_t *context,
    uint32_t  *pri_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_PARAM_RANGE_CHK((*pri_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);
	acl_parameter.filter_fields |= INGRESS_CTAG_PRI_BIT;
	acl_parameter.ingress_ctag_pri= *pri_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_ctag_pri_pri */

/*
 * rg set acl-filter pattern ingress_smac <MACADDR:smac>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_smac_smac(
    cparser_context_t *context,
    cparser_macaddr_t  *smac_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	acl_parameter.filter_fields |= INGRESS_SMAC_BIT;
	osal_memcpy(&acl_parameter.ingress_smac.octet, smac_ptr->octet, ETHER_ADDR_LEN);	

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_smac_smac */

/*
 * rg set acl-filter pattern ingress_dmac <MACADDR:dmac>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_dmac_dmac(
    cparser_context_t *context,
    cparser_macaddr_t  *dmac_ptr)
{
    DIAG_UTIL_PARAM_CHK();	
	acl_parameter.filter_fields |= INGRESS_DMAC_BIT;
	osal_memcpy(&acl_parameter.ingress_dmac.octet, dmac_ptr->octet, ETHER_ADDR_LEN);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_dmac_dmac */

/*
 * rg set acl-filter pattern ingress_src_ipv4_addr_start <IPV4ADDR:igr_sip_low_bound> ingress_src_ipv4_addr_end <IPV4ADDR:igr_sip_up_bound>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_src_ipv4_addr_start_igr_sip_low_bound_ingress_src_ipv4_addr_end_igr_sip_up_bound(
    cparser_context_t *context,
    uint32_t  *igr_sip_low_bound_ptr,
    uint32_t  *igr_sip_up_bound_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	acl_parameter.filter_fields |= INGRESS_IPV4_SIP_RANGE_BIT;
    acl_parameter.ingress_src_ipv4_addr_start = (ipaddr_t)*igr_sip_low_bound_ptr;
    acl_parameter.ingress_src_ipv4_addr_end = (ipaddr_t)*igr_sip_up_bound_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_src_ipv4_addr_start_igr_sip_low_bound_ingress_src_ipv4_addr_end_igr_sip_up_bound */

/*
 * rg set acl-filter pattern ingress_dest_ipv4_addr_start  <IPV4ADDR:igr_dip_low_bound> ingress_dest_ipv4_addr_end  <IPV4ADDR:igr_dip_up_bound>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_dest_ipv4_addr_start_igr_dip_low_bound_ingress_dest_ipv4_addr_end_igr_dip_up_bound(
    cparser_context_t *context,
    uint32_t  *igr_dip_low_bound_ptr,
    uint32_t  *igr_dip_up_bound_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	acl_parameter.filter_fields |= INGRESS_IPV4_DIP_RANGE_BIT;
	acl_parameter.ingress_dest_ipv4_addr_start = (ipaddr_t)*igr_dip_low_bound_ptr;
    acl_parameter.ingress_dest_ipv4_addr_end = (ipaddr_t)*igr_dip_up_bound_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_dest_ipv4_addr_start_igr_dip_low_bound_ingress_dest_ipv4_addr_end_igr_dip_up_bound */

/*
 * rg set acl-filter pattern ingress_src_ipv6_addr_start  <IPV6ADDR:igr_sip6_low_bound> ingress_src_ipv6_addr_end <IPV6ADDR:igr_sip6_up_bound> 
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_src_ipv6_addr_start_igr_sip6_low_bound_ingress_src_ipv6_addr_end_igr_sip6_up_bound(
    cparser_context_t *context,
    char * *igr_sip6_low_bound_ptr,
    char * *igr_sip6_up_bound_ptr)
{
	int32  ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

	//FIX ME
	acl_parameter.filter_fields |= INGRESS_IPV6_SIP_RANGE_BIT;

 
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&acl_parameter.ingress_src_ipv6_addr_start[0], TOKEN_STR(5)), ret);
  	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&acl_parameter.ingress_src_ipv6_addr_end[0], TOKEN_STR(7)), ret);

	//osal_memcpy(&acl_parameter.ingress_src_ipv6_addr_start, *igr_sip6_low_bound_ptr, sizeof(uint8)*16);
	//osal_memcpy(&acl_parameter.ingress_src_ipv6_addr_end, *igr_sip6_up_bound_ptr, sizeof(uint8)*16);
	//diag_util_printf("[fix me]ipv6 address parse error in ipv6. \n");
	//return CPARSER_ERR_INVALID_PARAMS;


    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_src_ipv6_addr_start_igr_sip6_low_bound_ingress_src_ipv6_addr_end_igr_sip6_up_bound */

/*
 * rg set acl-filter pattern ingress_dest_ipv6_addr_start <IPV6ADDR:igr_dip6_low_bound> ingress_dest_ipv6_addr_end <IPV6ADDR:igr_sip6_up_bound> 
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_dest_ipv6_addr_start_igr_dip6_low_bound_ingress_dest_ipv6_addr_end_igr_sip6_up_bound(
    cparser_context_t *context,
    char * *igr_dip6_low_bound_ptr,
    char * *igr_sip6_up_bound_ptr)
{
	int32  ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

	//FIX ME
	acl_parameter.filter_fields |= INGRESS_IPV6_DIP_RANGE_BIT;

	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&acl_parameter.ingress_dest_ipv6_addr_start[0], TOKEN_STR(5)), ret);
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&acl_parameter.ingress_dest_ipv6_addr_end[0], TOKEN_STR(7)), ret);

	//osal_memcpy(&acl_parameter.ingress_dest_ipv6_addr_start, *igr_dip6_low_bound_ptr, sizeof(uint8)*16);
	//osal_memcpy(&acl_parameter.ingress_dest_ipv6_addr_end, *igr_sip6_up_bound_ptr, sizeof(uint8)*16);
	//diag_util_printf("[fix me]ipv6 address parse error in ipv6. \n");
	//return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_dest_ipv6_addr_start_igr_dip6_low_bound_ingress_dest_ipv6_addr_end_igr_sip6_up_bound */

/*
 * rg set acl-filter pattern ingress_src_l4_port_start <UINT:igr_l4sport_low_bound> ingress_src_l4_port_end <UINT:igr_l4sport_up_bound>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_src_l4_port_start_igr_l4sport_low_bound_ingress_src_l4_port_end_igr_l4sport_up_bound(
    cparser_context_t *context,
    uint32_t  *igr_l4sport_low_bound_ptr,
    uint32_t  *igr_l4sport_up_bound_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	acl_parameter.filter_fields |= INGRESS_L4_SPORT_RANGE_BIT;
	acl_parameter.ingress_src_l4_port_start = *igr_l4sport_low_bound_ptr;
	acl_parameter.ingress_src_l4_port_end = *igr_l4sport_up_bound_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_src_l4_port_start_igr_l4sport_low_bound_ingress_src_l4_port_end_igr_l4sport_up_bound */

/*
 * rg set acl-filter pattern ingress_dest_l4_port_start <UINT:igr_l4dport_low_bound> ingress_dest_l4_port_end <UINT:igr_l4dport_up_bound>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_dest_l4_port_start_igr_l4dport_low_bound_ingress_dest_l4_port_end_igr_l4dport_up_bound(
    cparser_context_t *context,
    uint32_t  *igr_l4dport_low_bound_ptr,
    uint32_t  *igr_l4dport_up_bound_ptr)
{
    DIAG_UTIL_PARAM_CHK();

	acl_parameter.filter_fields |= INGRESS_L4_DPORT_RANGE_BIT;
	acl_parameter.ingress_dest_l4_port_start = *igr_l4dport_low_bound_ptr;
	acl_parameter.ingress_dest_l4_port_end = *igr_l4dport_up_bound_ptr;


    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_dest_l4_port_start_igr_l4dport_low_bound_ingress_dest_l4_port_end_igr_l4dport_up_bound */

/*
 * rg set acl-filter pattern egress_src_ipv4_addr_start <IPV4ADDR:egr_sip_low_bound> egress_src_ipv4_addr_end <IPV4ADDR:egr_sip_up_bound>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_egress_src_ipv4_addr_start_egr_sip_low_bound_egress_src_ipv4_addr_end_egr_sip_up_bound(
    cparser_context_t *context,
    uint32_t  *egr_sip_low_bound_ptr,
    uint32_t  *egr_sip_up_bound_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	acl_parameter.filter_fields |= EGRESS_IPV4_SIP_RANGE_BIT;
    acl_parameter.egress_src_ipv4_addr_start = (ipaddr_t)*egr_sip_low_bound_ptr;
    acl_parameter.egress_src_ipv4_addr_end = (ipaddr_t)*egr_sip_up_bound_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_egress_src_ipv4_addr_start_egr_sip_low_bound_egress_src_ipv4_addr_end_egr_sip_up_bound */

/*
 * rg set acl-filter pattern egress_dest_ipv4_addr_start <IPV4ADDR:egr_dip_low_bound> egress_dest_ipv4_addr_end <IPV4ADDR:egr_dip_up_bound>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_egress_dest_ipv4_addr_start_egr_dip_low_bound_egress_dest_ipv4_addr_end_egr_dip_up_bound(
    cparser_context_t *context,
    uint32_t  *egr_dip_low_bound_ptr,
    uint32_t  *egr_dip_up_bound_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	acl_parameter.filter_fields |= EGRESS_IPV4_DIP_RANGE_BIT;
	acl_parameter.egress_dest_ipv4_addr_start = (ipaddr_t)*egr_dip_low_bound_ptr;
    acl_parameter.egress_dest_ipv4_addr_end = (ipaddr_t)*egr_dip_up_bound_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_egress_dest_ipv4_addr_start_egr_dip_low_bound_egress_dest_ipv4_addr_end_egr_dip_up_bound */

/*
 * rg set acl-filter pattern egress_src_l4_port_start <UINT:egr_l4sport_low_bound> egress_src_l4_port_end <UINT:egr_l4sport_up_bound>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_egress_src_l4_port_start_egr_l4sport_low_bound_egress_src_l4_port_end_egr_l4sport_up_bound(
    cparser_context_t *context,
    uint32_t  *egr_l4sport_low_bound_ptr,
    uint32_t  *egr_l4sport_up_bound_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	acl_parameter.filter_fields |= EGRESS_L4_SPORT_RANGE_BIT;
	acl_parameter.egress_src_l4_port_start = *egr_l4sport_low_bound_ptr;
	acl_parameter.egress_src_l4_port_end = *egr_l4sport_up_bound_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_egress_src_l4_port_start_egr_l4sport_low_bound_egress_src_l4_port_end_egr_l4sport_up_bound */

/*
 * rg set acl-filter pattern egress_dest_l4_port_start <UINT:egr_l4dport_low_bound> egress_dest_l4_port_end <UINT:igr_l4dport_up_bound>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_egress_dest_l4_port_start_egr_l4dport_low_bound_egress_dest_l4_port_end_igr_l4dport_up_bound(
    cparser_context_t *context,
    uint32_t  *egr_l4dport_low_bound_ptr,
    uint32_t  *igr_l4dport_up_bound_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	acl_parameter.filter_fields |= EGRESS_L4_DPORT_RANGE_BIT;
	acl_parameter.egress_dest_l4_port_start = *egr_l4dport_low_bound_ptr;
	acl_parameter.egress_dest_l4_port_end = *igr_l4dport_up_bound_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_pattern_egress_dest_l4_port_start_egr_l4dport_low_bound_egress_dest_l4_port_end_igr_l4dport_up_bound */

/*
 * rg set acl-filter pattern ingress_l4_protocal <UINT:protocal>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_pattern_ingress_l4_protocal_protocal(
    cparser_context_t *context,
    uint32_t  *protocal_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	if(*protocal_ptr==0){
		acl_parameter.filter_fields |= INGRESS_L4_UDP_BIT;
	}else if(*protocal_ptr==1){
		acl_parameter.filter_fields |= INGRESS_L4_TCP_BIT;
	}else if(*protocal_ptr==2){
		acl_parameter.filter_fields |= INGRESS_L4_ICMP_BIT;
	}else{
		diag_util_printf("no such l4-protocal type!");
		return CPARSER_ERR_INVALID_PARAMS;
	}

}    /* end of cparser_cmd_rg_set_acl_filter_pattern_ingress_l4_protocal_protocal */



/*
 * rg set acl-filter action action_type <UINT:actiontype>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_action_action_type_actiontype(
    cparser_context_t *context,
    uint32_t  *actiontype_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	acl_parameter.action_type = *actiontype_ptr;
    return CPARSER_OK;
	
}    /* end of cparser_cmd_rg_set_acl_filter_action_action_type_actiontype */

/*
 * rg set acl-filter action qos action_dot1p_remarking_pri <UINT:dot1p_remarking_pri>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_action_qos_action_dot1p_remarking_pri_dot1p_remarking_pri(
    cparser_context_t *context,
    uint32_t  *dot1p_remarking_pri_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	if(acl_parameter.action_type != ACL_ACTION_TYPE_QOS)
	{
		diag_util_printf("[ERROR!]acl action type must be ACL_ACTION_TYPE_QOS! \n");
		return CPARSER_ERR_INVALID_PARAMS;
	}
	else
	{
		DIAG_UTIL_PARAM_RANGE_CHK((*dot1p_remarking_pri_ptr > RTK_DOT1P_PRIORITY_MAX), CPARSER_ERR_INVALID_PARAMS);
		acl_parameter.qos_actions |= ACL_ACTION_1P_REMARKING_BIT;
		acl_parameter.action_dot1p_remarking_pri = *dot1p_remarking_pri_ptr;
	}
		
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_action_qos_action_dot1p_remarking_pri_dot1p_remarking_pri */

/*
 * rg set acl-filter action qos action_ip_precedence_remarking_pri  <UINT:ip_precedence_remarking_pri>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_action_qos_action_ip_precedence_remarking_pri_ip_precedence_remarking_pri(
    cparser_context_t *context,
    uint32_t  *ip_precedence_remarking_pri_ptr)
{
    DIAG_UTIL_PARAM_CHK();

	if(acl_parameter.action_type != ACL_ACTION_TYPE_QOS)
	{
		diag_util_printf("[ERROR!]acl action type must be ACL_ACTION_TYPE_QOS! \n");
		return CPARSER_ERR_INVALID_PARAMS;
	}
	else
	{
		DIAG_UTIL_PARAM_RANGE_CHK((*ip_precedence_remarking_pri_ptr > (RTK_VALUE_OF_DSCP_MAX>>3)), CPARSER_ERR_INVALID_PARAMS);
		acl_parameter.qos_actions |= ACL_ACTION_IP_PRECEDENCE_REMARKING_BIT;
		acl_parameter.action_ip_precedence_remarking_pri = *ip_precedence_remarking_pri_ptr;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_action_qos_action_ip_precedence_remarking_pri_ip_precedence_remarking_pri */

/*
 * rg set acl-filter action qos action_dscp_remarking_pri <UINT:dscp_remarking_pri>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_action_qos_action_dscp_remarking_pri_dscp_remarking_pri(
    cparser_context_t *context,
    uint32_t  *dscp_remarking_pri_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	
	if(acl_parameter.action_type != ACL_ACTION_TYPE_QOS)
	{
		diag_util_printf("[ERROR!]acl action type must be ACL_ACTION_TYPE_QOS! \n");
		return CPARSER_ERR_INVALID_PARAMS;
	}
	else
	{
		DIAG_UTIL_PARAM_RANGE_CHK((*dscp_remarking_pri_ptr > RTK_VALUE_OF_DSCP_MAX), CPARSER_ERR_INVALID_PARAMS);
		acl_parameter.qos_actions |= ACL_ACTION_DSCP_REMARKING_BIT;
		acl_parameter.action_dscp_remarking_pri = *dscp_remarking_pri_ptr;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_action_qos_action_dscp_remarking_pri_dscp_remarking_pri */

/*
 * rg set acl-filter action qos action_queue_id <UINT:queue_id>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_action_qos_action_queue_id_queue_id(
    cparser_context_t *context,
    uint32_t  *queue_id_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	
	if(acl_parameter.action_type != ACL_ACTION_TYPE_QOS)
	{
		diag_util_printf("[ERROR!]acl action type must be ACL_ACTION_TYPE_QOS! \n");
		return CPARSER_ERR_INVALID_PARAMS;
	}
	else
	{
		DIAG_UTIL_PARAM_RANGE_CHK((*queue_id_ptr > 7), CPARSER_ERR_INVALID_PARAMS);
		acl_parameter.qos_actions |= ACL_ACTION_QUEUE_ID_BIT;
		acl_parameter.action_queue_id = *queue_id_ptr;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_action_qos_action_queue_id_queue_id */

/*
 * rg set acl-filter action qos action_share_meter <UINT:share_meter>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_action_qos_action_share_meter_share_meter(
    cparser_context_t *context,
    uint32_t  *share_meter_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	
	if(acl_parameter.action_type != ACL_ACTION_TYPE_QOS)
	{
		diag_util_printf("[ERROR!]acl action type must be ACL_ACTION_TYPE_QOS! \n");
		return CPARSER_ERR_INVALID_PARAMS;
	}
	else
	{
		DIAG_UTIL_PARAM_RANGE_CHK((*share_meter_ptr >= HAL_MAX_NUM_OF_METERING()), CPARSER_ERR_INVALID_PARAMS);
		acl_parameter.qos_actions |= ACL_ACTION_SHARE_METER_BIT;
		acl_parameter.action_share_meter = *share_meter_ptr;
	}
	
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_acl_filter_action_qos_action_share_meter_share_meter */

/*
 * rg set acl-filter action qos action_stream_id <UINT:stream_id>
 */
cparser_result_t
cparser_cmd_rg_set_acl_filter_action_qos_action_stream_id_stream_id(
    cparser_context_t *context,
    uint32_t  *stream_id_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	
	if(acl_parameter.action_type != ACL_ACTION_TYPE_QOS)
	{
		diag_util_printf("[ERROR!]acl action type must be ACL_ACTION_TYPE_QOS! \n");
		return CPARSER_ERR_INVALID_PARAMS;
	}
	else
	{
		DIAG_UTIL_PARAM_RANGE_CHK((*stream_id_ptr >= HAL_CLASSIFY_SID_NUM()), CPARSER_ERR_INVALID_PARAMS);
		acl_parameter.qos_actions |= ACL_ACTION_STREAM_ID_OR_LLID_BIT;
		acl_parameter.action_stream_id_or_llid = *stream_id_ptr;
	}
	
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_set_acl_filter_action_qos_action_stream_id_stream_id */

                                                                                      

