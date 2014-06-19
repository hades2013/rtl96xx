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
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
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
#include <rtk/l34.h>
#include <dal/apollomp/raw/apollomp_raw_l34.h>


const char *ext_type_Str[L34_EXTIP_TYPE_END] = {
DIAG_STR_NAPT,
DIAG_STR_NAT,
DIAG_STR_LOCAL_PUBLIC
};


static void
stateToString(rtk_enable_t state,char *str){
	char tmpStr[128]="ENABLED";
	memset(str,0,sizeof(tmpStr));
	if(str!=NULL){
		if(state != ENABLED){
			strncpy(tmpStr,"DISABLED",sizeof("DISABLED"));
		}
		memcpy(str,(char*)tmpStr,sizeof(tmpStr));
	}
}

static void 
diag_print_napt(rtk_enable_t force,rtk_l34_naptOutbound_entry_t napt)
{
	char tmp[128]="";

	if(force || napt.valid)
	{
		diag_util_mprintf("NAPTR Table Id:\t%d\n",napt.hashIdx);
		stateToString(napt.priValid, tmp);
		diag_util_mprintf("Priority State:\t%s\n",tmp);
		diag_util_mprintf("Priority Value:\t%d\n",napt.priValue);
		stateToString(napt.valid, tmp);
		diag_util_mprintf("State:\t\t%s\n",tmp);
	}
}

static void
diag_print_naptr(rtk_enable_t force, rtk_l34_naptInbound_entry_t naptr)
{
	char tmp[128]="";

	if(force || naptr.valid)
	{
		diag_util_mprintf("ExtIP ID:\t%d\n",naptr.extIpIdx);
		diag_util_mprintf("ExtPortLSB:\t%d\n",naptr.extPortLSB);
		diag_util_ip2str((uint8*)tmp, (uint32)naptr.intIp);
		diag_util_mprintf("Internal IP:\t%s\n",tmp);
		diag_util_mprintf("Internal Port:\t%d\n",naptr.intPort);
		stateToString(naptr.isTcp, tmp);
		diag_util_mprintf("IS TCP:\t\t%s\n",tmp);
		stateToString(naptr.priValid, tmp);
		diag_util_mprintf("Priority State:\t%s\n",tmp);
		diag_util_mprintf("Priority Value:\t%d\n",naptr.priId);
		diag_util_mprintf("Remote Hash:\t%d\n",naptr.remHash);
		diag_util_mprintf("NAT mode:\t%d\n",naptr.valid);

	}
}

void _cparser_cmd_l34_arp_header_display(void)
{
    diag_util_printf("%-5s ","Index" );
    diag_util_printf("%-7s ","Valid");
    diag_util_mprintf("%-6s\n","NH_IDX");    
}


void _cparser_cmd_l34_arp_entry_display(uint32 index,rtk_l34_arp_entry_t *entry)
{
    diag_util_printf("%-5d ",index);

    if(entry->valid == 1)
    {
        /*valid*/
        diag_util_printf("%-7s ",DIAG_STR_VALID);      
        diag_util_mprintf("%-6d\n",entry->nhIdx);    
    }
    else
    {
        /*invalid*/
        diag_util_mprintf("%-7s \n",DIAG_STR_INVALID);      
    }
}

void _cparser_cmd_l34_external_ip_table_header_display(void)
{
    diag_util_mprintf("%-5s %-7s %-15s %-15s %-6s %-3s %-8s\n",
                                                             "Index",
                                                             "Valid",
                                                             "Ext_IP",
                                                             "Int_IP",
                                                             "TYPE",
                                                             "NHidx",
                                                             "priority");    
}    

void _cparser_cmd_l34_external_ip_table_display(uint32 index,rtk_l34_ext_intip_entry_t *entry)
{
    uint32  ip_tmp_buf[UTIL_IP_TMP_BUFFER_LENGTH];
    
    diag_util_printf("%-5d ",index);    
    
    
    if(entry->valid == 1)
    {
        /*valid*/
        diag_util_printf("%-7s ",DIAG_STR_VALID);      

        /*external IP address*/

        diag_util_ip2str((uint8*)ip_tmp_buf, (uint32)entry->extIpAddr);
        diag_util_printf("%-15s ",ip_tmp_buf);   

        diag_util_ip2str((uint8*)ip_tmp_buf, (uint32)entry->intIpAddr);
        diag_util_printf("%-15s ",ip_tmp_buf);   


        /*Table Type*/
        diag_util_printf("%-6s ",ext_type_Str[entry->type]);   
        
        /*next hop index*/
        diag_util_printf("%-8d ",entry->nhIdx);   
        
        /*priority*/
        if(0x1 == entry->prival)
            diag_util_mprintf("%-8d \n",entry->pri);   
        else
            diag_util_mprintf("%-8s \n",DIAG_STR_NO);   
          
    }
    else
    {
        diag_util_mprintf("%-7s \n",DIAG_STR_INVALID);      
    }
    
 
}


cparser_result_t
_cparser_cmd_l34_set_external_ip_set(
    cparser_context_t *context,
    uint32_t  *table_idx_ptr,
    uint32_t  *external_ip_ptr,
    uint32_t  *internal_ip_ptr,
    uint32_t  *nh_idx_ptr,
    int32_t  pri)
{
    rtk_l34_ext_intip_entry_t entry;
    int32   ret = RT_ERR_FAILED; 
    int32   index;
    DIAG_UTIL_PARAM_CHK();
    
    /*get index*/
    index = *table_idx_ptr;
    
    /*get type*/
    if('N' == TOKEN_CHAR(6,0))
    {
        if('T' == TOKEN_CHAR(6,2))    
            entry.type = L34_EXTIP_TYPE_NAT;            
        else
            entry.type = L34_EXTIP_TYPE_NAPT;            
        
    }
    else
    {
        entry.type = L34_EXTIP_TYPE_LP;            
    }

    /*get external IP address*/
    entry.extIpAddr = *external_ip_ptr;

    
    /*get internal IP address*/
    entry.intIpAddr = *internal_ip_ptr;

    /*get nexthop index*/
    entry.nhIdx = *nh_idx_ptr;
    
    /*get priority*/
    
    
    if(-1 == pri)
    {
        entry.prival = 0;
    }
    else
    {
        entry.prival = 1;
        entry.pri = pri;
    
    }     
    
    /*set vlid bit*/
    entry.valid = 1;
    
    
    if((ret = rtk_l34_extIntIPTable_set(index,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     
    return CPARSER_OK;
}    /* end of _cparser_cmd_l34_set_external_ip_set */

void _cparser_cmd_l34_routing_table_header_display(void)
{
    diag_util_mprintf("%-5s %-7s %-15s %-4s %-8s %-14s %-10s\n",
                                                             "Index",
                                                             "Valid",
                                                             "IP_Addr",
                                                             "MASK",
                                                             "Internal",
                                                             "ROUTE_TYPE",
                                                             "MISC_INFO");    
}    

void _cparser_cmd_l34_routing_table_display(uint32 index, rtk_l34_routing_entry_t *entry)
{
  
    diag_util_printf("%-5d ",index);    
    if(entry->valid == 1)
    {
        /*valid*/
        diag_util_printf("%-7s ",DIAG_STR_VALID);      

        /*ip address*/
        diag_util_printf("%-15s ",diag_util_inet_ntoa(entry->ipAddr));   


        /*mask*/
        diag_util_printf("%-4d ",entry->ipMask);   
        
        /*is internal*/
        if(entry->internal)
            diag_util_printf("%-8s ",DIAG_STR_YES);  
        else
            diag_util_printf("%-8s ",DIAG_STR_NO); 
        
        /*ROUTE_TYPE*/
        switch(entry->process)
        {
            case L34_PROCESS_CPU:
                diag_util_mprintf("%-14s \n",DIAG_STR_TRAP2CPU); 
                break;
            case L34_PROCESS_DROP:
                diag_util_mprintf("%-14s \n",DIAG_STR_DROP); 
                break;

            case L34_PROCESS_ARP:
                diag_util_printf("%-14s ",DIAG_STR_ROUTE_TYPE_LOCAL); 
                /*arp info*/
                diag_util_mprintf("netif_idx(%2d) arp_start(%3d) arp_end(%3d)\n",entry->netifIdx,entry->arpStart,entry->arpEnd); 
                break;
                
            case L34_PROCESS_NH:
                diag_util_printf("%-14s ",DIAG_STR_ROUTE_TYPE_GLOBAL); 
                /*next hop info*/
                diag_util_mprintf("nh_idx(%4d) nh_num(%2d) nh_algo(%1d) ip_doman_num(%d)\n",entry->nhStart,entry->nhNum,entry->nhAlgo,entry->ipDomain); 
               
                break;
            default:
                diag_util_mprintf("%-14s \n",DIAG_STR_INVALID); 
                break;

        }

    }
    else
    {
        diag_util_mprintf("%-7s \n",DIAG_STR_INVALID);      
    }
 
  
}

void _cparser_cmd_l34_netif_table_header_display(void)
{
    diag_util_mprintf("%-5s %-7s %-7s %-4s %-8s %-17s %-4s\n",
                                                             "Index",
                                                             "Valid",
                                                             "Vlan ID",
                                                             "MTU",
                                                             "EN_ROUTE",
                                                             "GATEWAY_MAC",
                                                             "MASK");    
}    

void _cparser_cmd_l34_netif_table_display(uint32 index,rtk_l34_netif_entry_t *entry)
{
  
    diag_util_printf("%-5d ",index);    

    /*valid*/
    diag_util_printf("%-7s ",DIAG_STR_VALID);      

    /*vlan id*/
    diag_util_printf("%-7d ",entry->vlan_id);   


    /*MTU*/
    diag_util_printf("%-4d ",entry->mtu);   
    
    /*EN_ROUTE*/
    if(entry->enable_rounting)
        diag_util_printf("%-8s ",DIAG_STR_YES);  
    else
        diag_util_printf("%-8s ",DIAG_STR_NO); 
    
    /*GATEWAY_MAC*/
    diag_util_printf("%-17s ",diag_util_inet_mactoa((uint8 *)&entry->gateway_mac));   
    
    /*MASK*/
    diag_util_mprintf("%-4d \n",entry->mac_mask); 

}


void _cparser_cmd_l34_nexthop_table_header_display(void)
{
    diag_util_mprintf("%-5s %-8s %-9s %-9s %-9s\n",
                      "Index",
                      "TYPE",
                      "NH_L2_IDX",
                      "IF_IDX",
                      "PPPoE_IDX");    
}    

void _cparser_cmd_l34_nexthop_table_display(uint32 index,rtk_l34_nexthop_entry_t *entry)
{
    /*index*/  
    diag_util_printf("%-5d ",index);    

    /*type*/
    diag_util_printf("%-8s ",diagStr_l34NexthopTypeStr[entry->type]);   

    /*NH_L2_IDX*/
    diag_util_printf("%-9d ",entry->nhIdx);   
        
    /*IF_IDX*/
    diag_util_printf("%-9d ",entry->ifIdx);   

    /*PPPoE_IDX*/
    diag_util_mprintf("%-9d \n",entry->pppoeIdx);   
   
  
}

void _cparser_cmd_l34_pppoe_table_header_display(void)
{
    diag_util_mprintf("%-5s %-12s\n",
                      "Index",
                      "Session_ID");    
}    

void _cparser_cmd_l34_pppoe_table_display(uint32 index,rtk_l34_pppoe_entry_t *entry)
{
    /*index*/  
    diag_util_printf("%-5d ",index);    

    /*PPPoE_IDX*/
    diag_util_mprintf("%-12d \n",entry->sessionID);   
}



void _cparser_cmd_l34_route6_table_header_display(void)
{
    diag_util_printf("%-5s ","Index" );
    diag_util_printf("%-7s ","Valid");
    diag_util_printf("%-30s","IPv6 Addr");    
    diag_util_printf("%-12s","IPv6 Prefix");    
    diag_util_printf("%-10s","NHorIfId");    
    diag_util_printf("%-10s","rt2waninf");    
    diag_util_mprintf("%-6s\n","type");    
}


void _cparser_cmd_l34_route6_table_display(uint32 index,rtk_ipv6Routing_entry_t *entry)
{
    char tmp[128]="";
	
    diag_util_printf("%-5d ",index);

    if(entry->valid == 1)
    {
        /*valid*/
        diag_util_printf("%-7s ",DIAG_STR_VALID);  
	 diag_util_ipv62str((uint8*)tmp, (uint8*)entry->ipv6Addr.ipv6_addr);
	 diag_util_printf("%-30s",tmp);    
        diag_util_printf("%-12d",entry->ipv6PrefixLen);    
        diag_util_printf("%-10d",entry->nhOrIfidIdx);    
        diag_util_printf("%-10d",entry->rt2waninf);    
        diag_util_mprintf("%-6d\n",entry->type);    
    }
    else
    {
        /*invalid*/
        diag_util_mprintf("%-7s \n",DIAG_STR_INVALID);      
    }
}

void _cparser_cmd_l34_neigh_table_header_display(void)
{
    diag_util_printf("%-5s ","Index" );
    diag_util_printf("%-7s ","Valid");
    diag_util_printf("%-20s","IFId");    
    diag_util_printf("%-10s","RouteId");    
    diag_util_mprintf("%-8s\n","L2Id");    
   
}


void _cparser_cmd_l34_neigh_table_display(uint32 index,rtk_ipv6Neighbor_entry_t *entry)
{
	
    diag_util_printf("%-5d ",index);
    char  tmp[128]="";

    if(entry->valid == 1)
    {
        /*valid*/
        diag_util_printf("%-7s ",DIAG_STR_VALID);  
	 diag_util_eui642str(tmp, &entry->ipv6Ifid);
	 diag_util_printf("%-20s",tmp);     
        diag_util_printf("%-10d",entry->ipv6RouteIdx);    
        diag_util_mprintf("%-8d\n",entry->l2Idx);    
    }
    else
    {
        /*invalid*/
        diag_util_mprintf("%-7s \n",DIAG_STR_INVALID);      
    }
}


void _cparser_cmd_l34_binding_table_header_display(void)
{
    diag_util_printf("%-5s ","Index" );
    diag_util_printf("%-10s ","wanTypeIdx");
    diag_util_printf("%-10s","bindProto");    
    diag_util_printf("%-12s","extPortMask");
    diag_util_printf("%-10s","portMask");    	
    diag_util_mprintf("%-6s\n","vidLan");    
   
}


void _cparser_cmd_l34_binding_table_display(uint32 index,rtk_binding_entry_t *entry)
{

	diag_util_printf("%-5d ",index);

	/*valid*/
	diag_util_printf("%-10d ",entry->wanTypeIdx);  
	diag_util_printf("%-10d",entry->bindProto);    
	diag_util_printf("%-12d",entry->extPortMask);    
	diag_util_printf("%-10d",entry->portMask);    
	diag_util_mprintf("%-6d \n",entry->vidLan);   

}


void _cparser_cmd_l34_wanType_table_header_display(void)
{
    diag_util_printf("%-5s ","Index" );
    diag_util_printf("%-6s ","nhIdx");
    diag_util_mprintf("%-6s\n","wanType");    
   
}


void _cparser_cmd_l34_wanType_table_display(uint32 index,rtk_wanType_entry_t *entry)
{

	diag_util_printf("%-5d ",index);

	/*valid*/
	diag_util_printf("%-6d ",entry->nhIdx);  
	diag_util_mprintf("%-6d\n",entry->wanType);    

}

static int32
_diag_util_bindingAct2Str(char *str, rtk_l34_bindAct_t act)
{
	switch(act){
	case L34_BIND_ACT_DROP:
		sprintf(str,"drop\n");
	break;
	case L34_BIND_ACT_TRAP:
		sprintf(str,"trap\n");
	break;
	case L34_BIND_ACT_FORCE_L2BRIDGE:
		sprintf(str,"fore L2 bridge\n");
	break;
	case L34_BIND_ACT_PERMIT_L2BRIDGE:
		sprintf(str,"premit and L2 bridge\n");
	break;
	case L34_BIND_ACT_IPV4_LOOKUPL4TABLE_IPV6_TRAP:
		sprintf(str,"ipv4 packet lookup and ipv6 packet trap\n");	
	break;
	case L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4:
		sprintf(str,"force L3 routing and skip lookup l4 table\n");
	break;
	case L34_BIND_ACT_FORCE_BINDL3:
		sprintf(str,"force L3 routing\n");
	break;
	case L34_BIND_ACT_NORMAL_LOOKUPL34:
		sprintf(str,"normal L34 lookup\n");
	break;
	default:
	break;
	}
	return RT_ERR_OK;
}

/*
 * l34 init
 */
cparser_result_t
cparser_cmd_l34_init(
    cparser_context_t *context)
{
	int32 ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_init(),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_lite_init */



/*
 * l34 reset table ( l3 | pppoe | nexthop | interface | external-ip | arp | naptr | napt )
 */
cparser_result_t
cparser_cmd_l34_reset_table_l3_pppoe_nexthop_interface_external_ip_arp_naptr_napt(
    cparser_context_t *context)
{
	 rtk_l34_table_type_t   l34_table;
	int32   ret = RT_ERR_FAILED;
	DIAG_UTIL_PARAM_CHK();

	switch(TOKEN_STR(3)[0])
	{
	    case 'l':
	        l34_table = L34_ROUTING_TABLE;
	        break;    
	    case 'p':
	        l34_table = L34_PPPOE_TABLE;
	        break;    
	    case 'i':
	        l34_table = L34_NETIF_TABLE;
	        break;    
	    case 'e':
	        l34_table = L34_INTIP_TABLE;
	        break;    
	    case 'a':
	        l34_table = L34_ARP_TABLE;
	        break;    
	    case 'n':
	        if(TOKEN_STR(3)[1]=='e')
	            l34_table = L34_NEXTHOP_TABLE;                
	        else 
	        {
	            if(strlen(TOKEN_STR(3)) == 5)
	                l34_table = L34_NAPTR_TABLE;
	            else
	                l34_table = L34_NAPT_TABLE;
	        }
	        break;    
	    default:
	        return CPARSER_NOT_OK;
	}

	if((ret = rtk_l34_table_reset(l34_table)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_reset_table_l3_pppoe_nexthop_interface_external_ip_arp_naptr_napt */

/*
 * l34 get arp
 */
cparser_result_t
cparser_cmd_l34_get_arp(
    cparser_context_t *context)
{
	uint32 index;
	rtk_l34_arp_entry_t entry;
	int32   ret = RT_ERR_FAILED;
	  
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	_cparser_cmd_l34_arp_header_display();
	for(index = 0; index <HAL_L34_ARP_ENTRY_MAX(); index++)
	{
	    if((ret = rtk_l34_arpTable_get(index,&entry)) != RT_ERR_OK)
	    {
	        DIAG_ERR_PRINT(ret);
	        return CPARSER_NOT_OK;
	    }
	    _cparser_cmd_l34_arp_entry_display(index,&entry);        
	        
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_arp */

/*
 * l34 get arp <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_arp_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_l34_arp_entry_t entry;
    int32   ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if((ret = rtk_l34_arpTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    _cparser_cmd_l34_arp_header_display();
    _cparser_cmd_l34_arp_entry_display(*index_ptr,&entry);

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_arp_index */

/*
 * l34 set arp <UINT:index> next-hop-table <UINT:nh_index>
 */
cparser_result_t
cparser_cmd_l34_set_arp_index_next_hop_table_nh_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *nh_index_ptr)
{
    rtk_l34_arp_entry_t entry;
    int32   ret = RT_ERR_FAILED;
  
    DIAG_UTIL_PARAM_CHK();
    
    if(*index_ptr >= HAL_L34_ARP_ENTRY_MAX()) 
        DIAG_ERR_PRINT(RT_ERR_ENTRY_INDEX);
    
    entry.valid = 1;
    entry.nhIdx = *nh_index_ptr;
    if((ret = rtk_l34_arpTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }        
        
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_arp_index_next_hop_table_nh_index */

/*
 * l34 del arp <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_arp_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32   ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    if(*index_ptr >= HAL_L34_ARP_ENTRY_MAX()) 
    {
        DIAG_ERR_PRINT(RT_ERR_ENTRY_INDEX);
        return CPARSER_NOT_OK;
    }

    if((ret = rtk_l34_arpTable_del(*index_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     
  
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_arp_index */

/*
 * l34 get external-ip 
 */
cparser_result_t
cparser_cmd_l34_get_external_ip(
    cparser_context_t *context)
{
    uint32_t index; /*tble index*/
    int32   ret = RT_ERR_FAILED;
    rtk_l34_ext_intip_entry_t entry;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    _cparser_cmd_l34_external_ip_table_header_display();
    for(index = 0; index < HAL_L34_EXTIP_ENTRY_MAX(); index++)
    {

        if((ret = rtk_l34_extIntIPTable_get(index,&entry)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        
        _cparser_cmd_l34_external_ip_table_display(index,&entry);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_external_ip */

/*
 * l34 get external-ip <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_external_ip_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_l34_ext_intip_entry_t entry;
    int32   ret = RT_ERR_FAILED;    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(*index_ptr >= HAL_L34_EXTIP_ENTRY_MAX()) 
    {
        DIAG_ERR_PRINT(RT_ERR_ENTRY_INDEX);
        return CPARSER_NOT_OK;
    }
    if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     
    _cparser_cmd_l34_external_ip_table_header_display();
    _cparser_cmd_l34_external_ip_table_display(*index_ptr,&entry);

  
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_external_ip_index */

/*
 * l34 set external-ip <UINT:index> type ( nat | napt | lp )
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_type_nat_napt_lp(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	rtk_l34_ext_intip_entry_t entry;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     

	if(!osal_strcmp(TOKEN_STR(5),"nat")){
		entry.type = L34_EXTIP_TYPE_NAT;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"napt")){
		entry.type = L34_EXTIP_TYPE_NAPT;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"lp")){
		entry.type = L34_EXTIP_TYPE_LP;
	}

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_type_nat_napt_lp */

/*
 * l34 set external-ip <UINT:index> external-ip <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_external_ip_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr)
{
   	int32 ret;
	rtk_l34_ext_intip_entry_t entry;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     

	entry.extIpAddr = *ip_ptr;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_external_ip_ip */

/*
 * l34 set external-ip <UINT:index> internal-ip <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_internal_ip_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr)
{
   	int32 ret;
	rtk_l34_ext_intip_entry_t entry;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     

	entry.intIpAddr = *ip_ptr;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_internal_ip_ip */

/*
 * l34 set external-ip <UINT:index> next-hop-table <UINT:nh_index>
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_next_hop_table_nh_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *nh_index_ptr)
{
   	int32 ret;
	rtk_l34_ext_intip_entry_t entry;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     

	entry.nhIdx  = *nh_index_ptr;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_next_hop_table_nh_index */

/*
 * l34 set external-ip <UINT:index> nat-priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_nat_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
   	int32 ret;
	rtk_l34_ext_intip_entry_t entry;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     

	if(!osal_strcmp(TOKEN_STR(6),"disable")){
		entry.prival = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(6),"enable")){
		entry.prival = ENABLED;
	}

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_nat_priority_state_disable_enable */

/*
 * l34 set external-ip <UINT:index> nat-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_nat_priority_priority(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *priority_ptr)
{
    	int32 ret;
	rtk_l34_ext_intip_entry_t entry;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     

	entry.pri= *priority_ptr;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_nat_priority_priority */

/*
 * l34 set external-ip <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_l34_ext_intip_entry_t entry;

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     

	if(!osal_strcmp(TOKEN_STR(5),"disable")){
		entry.valid = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"enable")){
		entry.valid = ENABLED;
	}

	if((ret = rtk_l34_extIntIPTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_state_disable_enable */

/*
 * l34 del external-ip <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_external_ip_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
     int32   ret = RT_ERR_FAILED; 
    DIAG_UTIL_PARAM_CHK();

    if((ret = rtk_l34_extIntIPTable_del((uint32_t)*index_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    } 
    
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_external_ip_index */

/*
 * l34 get routing 
 */
cparser_result_t
cparser_cmd_l34_get_routing(
    cparser_context_t *context)
{
     rtk_l34_routing_entry_t entry;
    int32 ret = RT_ERR_FAILED; 
    uint32 index;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    _cparser_cmd_l34_routing_table_header_display();

    for(index =0;index < HAL_L34_ROUTING_ENTRY_MAX();index++)
    {
        memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));
        if((ret = rtk_l34_routingTable_get(index,&entry)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }     
        
        _cparser_cmd_l34_routing_table_display(index,&entry);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_routing */

/*
 * l34 get routing <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_routing_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_l34_routing_entry_t entry;
    int32 ret = RT_ERR_FAILED; 

        
    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((*index_ptr >= HAL_L34_ROUTING_ENTRY_MAX()), CPARSER_ERR_INVALID_PARAMS);   

    DIAG_UTIL_OUTPUT_INIT();

    memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));
    if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     

    _cparser_cmd_l34_routing_table_header_display();
    _cparser_cmd_l34_routing_table_display(*index_ptr,&entry);
    
	return CPARSER_NOT_OK;

}    /* end of cparser_cmd_l34_get_routing_index */

/*
 * l34 set routing <UINT:index> ip <IPV4ADDR:ip> ip-mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_ip_ip_ip_mask_mask(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr,
    uint32_t  *mask_ptr)
{
     rtk_l34_routing_entry_t entry;
    int32   ret = RT_ERR_FAILED; 

    DIAG_UTIL_PARAM_CHK();
    
    RT_PARAM_CHK((*index_ptr >= HAL_L34_ROUTING_ENTRY_MAX()), CPARSER_ERR_INVALID_PARAMS);   
    RT_PARAM_CHK((*mask_ptr > 31), CPARSER_ERR_INVALID_PARAMS);   
    
    memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

    if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }    
   
   
    /*IP address*/
    if((ret = diag_util_str2ip((uint32 *)&entry.ipAddr,(uint8 *)TOKEN_STR(5))) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }


    /*IP Mask*/
    entry.ipMask = *mask_ptr; 
	
    /*call API to set entry*/            
    if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     
    
    
    return CPARSER_OK;
    
}    /* end of cparser_cmd_l34_set_routing_index_ip_ip_ip_mask_mask */

/*
 * l34 set routing <UINT:index> interface-type ( internal | external )
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_interface_type_internal_external(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	rtk_l34_routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();
	memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

	if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    

	if(!osal_strcmp(TOKEN_STR(5),"internal")){
		entry.internal = ENABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"external")){
		entry.internal = DISABLED;
	}

	/*call API to set entry*/            
	if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_interface_type_internal_external */

/*
 * l34 set routing <UINT:index> type ( drop | trap )
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_drop_trap(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	rtk_l34_routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();
	memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

	if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    

	if(!osal_strcmp(TOKEN_STR(5),"drop")){
		entry.process = L34_PROCESS_DROP;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"trap")){
		entry.internal = L34_PROCESS_CPU;
	}

	/*call API to set entry*/            
	if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_drop_trap */

/*
 * l34 set routing <UINT:index> type local-route destination-netif <UINT:netif_index>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_local_route_destination_netif_netif_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *netif_index_ptr)
{
    	rtk_l34_routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();
	memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

	if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    
	entry.netifIdx = *netif_index_ptr;

	/*call API to set entry*/            
	if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_local_route_destination_netif_netif_index */

/*
 * l34 set routing <UINT:index> type local-route arp-start-address <UINT:start_addr> arp-end-address <UINT:end_addr>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_local_route_arp_start_address_start_addr_arp_end_address_end_addr(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *start_addr_ptr,
    uint32_t  *end_addr_ptr)
{
    	rtk_l34_routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();
	memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

	if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    
	entry.arpStart = *start_addr_ptr;
	entry.arpEnd = *end_addr_ptr;

	/*call API to set entry*/            
	if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_local_route_arp_start_address_start_addr_arp_end_address_end_addr */

/*
 * l34 set routing <UINT:index> type global-route next-hop-table <UINT:nh_index>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_global_route_next_hop_table_nh_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *nh_index_ptr)
{
    	rtk_l34_routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();
	memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

	if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    
	entry.nhNxt = *nh_index_ptr;

	/*call API to set entry*/            
	if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_global_route_next_hop_table_nh_index */

/*
 * l34 set routing <UINT:index> type global-route next-hop-start <UINT:address> next-hop-number <UINT:nh_number>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_global_route_next_hop_start_address_next_hop_number_nh_number(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *address_ptr,
    uint32_t  *nh_number_ptr)
{
    	rtk_l34_routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();
	memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

	if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    
	entry.nhStart = *address_ptr;
	entry.nhNum = *nh_number_ptr;

	/*call API to set entry*/            
	if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_global_route_next_hop_start_address_next_hop_number_nh_number */

/*
 * l34 set routing <UINT:index> type global-route next-hop-algo ( per-packet | per-session | per-source_ip )
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_global_route_next_hop_algo_per_packet_per_session_per_source_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_global_route_next_hop_algo_per_packet_per_session_per_source_ip */

/*
 * l34 set routing <UINT:index> type global-route ip-domain-range <UINT:range>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_global_route_ip_domain_range_range(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *range_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_global_route_ip_domain_range_range */

/*
 * l34 set routing <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
   	rtk_l34_routing_entry_t entry;
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();
	memset(&entry,0x0,sizeof(rtk_l34_routing_entry_t));

	if((ret = rtk_l34_routingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}    
	if(!osal_strcmp(TOKEN_STR(5),"disable")){
		entry.valid = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"enable")){
		entry.valid = ENABLED;
	}

	/*call API to set entry*/            
	if((ret = rtk_l34_routingTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}     
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_state_disable_enable */

/*
 * l34 del routing <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_routing_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32   ret = RT_ERR_FAILED; 
	DIAG_UTIL_PARAM_CHK();

	if((ret = rtk_l34_routingTable_del((uint32_t)*index_ptr)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	} 
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_routing_index */

/*
 * l34 get netif 
 */
cparser_result_t
cparser_cmd_l34_get_netif(
    cparser_context_t *context)
{
    uint32 index;
    int32   ret = RT_ERR_FAILED;
    rtk_l34_netif_entry_t entry;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    _cparser_cmd_l34_netif_table_header_display();
    
    for(index = 0; index < HAL_L34_NETIF_ENTRY_MAX(); index++)
    {

        if((ret = rtk_l34_netifTable_get(index,&entry)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        
        _cparser_cmd_l34_netif_table_display(index,&entry);
    }    
  
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_netif */

/*
 * l34 get netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_netif_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
     int32   ret = RT_ERR_FAILED;
    rtk_l34_netif_entry_t entry;
  

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    
    _cparser_cmd_l34_netif_table_header_display();
    _cparser_cmd_l34_netif_table_display(*index_ptr,&entry);
    
    
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_netif_index */

/*
 * l34 set netif <UINT:index> gateway-mac <MACADDR:mac> mac-mask ( no-mask | 1bit-mask | 2bit-mask | 3bit-mask )
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_gateway_mac_mac_mac_mask_no_mask_1bit_mask_2bit_mask_3bit_mask(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    cparser_macaddr_t  *mac_ptr)
{
	int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}
	diag_util_str2mac(&entry.gateway_mac.octet[0],(uint8 *)mac_ptr);
	
	if(!osal_strcmp(TOKEN_STR(7),"no-mask")){
		entry.mac_mask = 7;
	}else
	if(!osal_strcmp(TOKEN_STR(7),"1bit-mask")){
		entry.mac_mask = 6;
	}else
	if(!osal_strcmp(TOKEN_STR(7),"2bit-mask")){
		entry.mac_mask = 4;
	}else
	if(!osal_strcmp(TOKEN_STR(7),"3bit-mask")){
		entry.mac_mask = 0;
	}

	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_gateway_mac_mac_mac_mask_no_mask_1bit_mask_2bit_mask_3bit_mask */

/*
 * l34 set netif <UINT:index> vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_vid_vid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *vid_ptr)
{
	int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}
	entry.vlan_id = *vid_ptr;
	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
		{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
		}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_vid_vid */

/*
 * l34 set netif <UINT:index> mtu <UINT:mtu>
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_mtu_mtu(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *mtu_ptr)
{
    	int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}
	entry.mtu = *mtu_ptr;
	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_mtu_mtu */

/*
 * l34 set netif <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}
	
	if(!osal_strcmp(TOKEN_STR(5),"disable")){
		entry.valid = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"enable")){
		entry.valid = ENABLED;
	}
	
	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_state_disable_enable */

/*
 * l34 set netif <UINT:index> l3-route state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_l3_route_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int ret;
	rtk_l34_netif_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
	if((ret = rtk_l34_netifTable_get(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}
	
	if(!osal_strcmp(TOKEN_STR(6),"disable")){
		entry.enable_rounting = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(6),"enable")){
		entry.enable_rounting = ENABLED;
	}
	
	if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    	{
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_l3_route_state_enable_disable */

/*
 * l34 del netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_netif_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32   ret = RT_ERR_FAILED;
    rtk_l34_netif_entry_t entry;
    
    DIAG_UTIL_PARAM_CHK();

    osal_memset(&entry,0x0,sizeof(rtk_l34_netif_entry_t));
    
    entry.valid = 0;
    
    if((ret = rtk_l34_netifTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_netif_index */

/*
 * l34 get nexthop 
 */
cparser_result_t
cparser_cmd_l34_get_nexthop(
    cparser_context_t *context)
{
     uint32 index;
    int32   ret = RT_ERR_FAILED;
    rtk_l34_nexthop_entry_t entry;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    _cparser_cmd_l34_nexthop_table_header_display();
    
    for(index = 0; index < HAL_L34_NH_ENTRY_MAX(); index++)
    {

        if((ret = rtk_l34_nexthopTable_get(index,&entry)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        
        _cparser_cmd_l34_nexthop_table_display(index,&entry);		
    }    
	return CPARSER_NOT_OK;
}    /* end of cparser_cmd_l34_get_nexthop */

/*
 * l34 get nexthop <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_nexthop_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32   ret = RT_ERR_FAILED;
    rtk_l34_nexthop_entry_t entry;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    

    if((ret = rtk_l34_nexthopTable_get(*index_ptr, &entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    _cparser_cmd_l34_nexthop_table_header_display();
        
    _cparser_cmd_l34_nexthop_table_display(*index_ptr,&entry);
  
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_nexthop_index */

/*
 * l34 set nexthop <UINT:index> netif <UINT:netif_index>
 */
cparser_result_t
cparser_cmd_l34_set_nexthop_index_netif_netif_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *netif_index_ptr)
{
	int32   ret = RT_ERR_FAILED;
	rtk_l34_nexthop_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if((ret = rtk_l34_nexthopTable_get(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}
	entry.ifIdx = *netif_index_ptr;
	if((ret = rtk_l34_nexthopTable_set(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nexthop_index_netif_netif_index */

/*
 * l34 set nexthop <UINT:index> l2 <UINT:l2_index>
 */
cparser_result_t
cparser_cmd_l34_set_nexthop_index_l2_l2_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *l2_index_ptr)
{
	int32   ret = RT_ERR_FAILED;
	rtk_l34_nexthop_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if((ret = rtk_l34_nexthopTable_get(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}
	entry.nhIdx = *l2_index_ptr;
	if((ret = rtk_l34_nexthopTable_set(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

    	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nexthop_index_l2_l2_index */

/*
 * l34 set nexthop <UINT:index> type ( ethernet | pppoe )
 */
cparser_result_t
cparser_cmd_l34_set_nexthop_index_type_ethernet_pppoe(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32   ret = RT_ERR_FAILED;
	rtk_l34_nexthop_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if((ret = rtk_l34_nexthopTable_get(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

	if(!osal_strcmp(TOKEN_STR(5),"ethernet")){
		entry.type = L34_NH_ETHER;
	}else
	if(!osal_strcmp(TOKEN_STR(5),"pppoe")){
		entry.type = L34_NH_PPPOE;
	}
	if((ret = rtk_l34_nexthopTable_set(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

    	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nexthop_index_type_ethernet_pppoe */

/*
 * l34 set nexthop <UINT:index> pppoe <UINT:pppoe_index>
 */
cparser_result_t
cparser_cmd_l34_set_nexthop_index_pppoe_pppoe_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *pppoe_index_ptr)
{
       int32   ret = RT_ERR_FAILED;
	rtk_l34_nexthop_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if((ret = rtk_l34_nexthopTable_get(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

	entry.pppoeIdx = *pppoe_index_ptr;
	
	if((ret = rtk_l34_nexthopTable_set(*index_ptr, &entry)) != RT_ERR_OK)
	{
	    DIAG_ERR_PRINT(ret);
	    return CPARSER_NOT_OK;
	}

    	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nexthop_index_pppoe_pppoe_index */

/*
 * l34 get pppoe 
 */
cparser_result_t
cparser_cmd_l34_get_pppoe(
    cparser_context_t *context)
{
     uint32 index;
    int32   ret = RT_ERR_FAILED;
    rtk_l34_pppoe_entry_t entry;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    _cparser_cmd_l34_pppoe_table_header_display();
    
    for(index = 0; index < HAL_L34_PPPOE_ENTRY_MAX(); index++)
    {

        if((ret = rtk_l34_pppoeTable_get(index,&entry)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        
        _cparser_cmd_l34_pppoe_table_display(index,&entry);
    }    
  
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_pppoe */

/*
 * l34 get pppoe <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_pppoe_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
   uint32 index = *index_ptr;
    int32 ret = RT_ERR_FAILED;
    rtk_l34_pppoe_entry_t entry;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


    if((ret = rtk_l34_pppoeTable_get(index,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    _cparser_cmd_l34_pppoe_table_header_display();
    _cparser_cmd_l34_pppoe_table_display(index,&entry);
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_pppoe_index */

/*
 * l34 set pppoe <UINT:index> session-id <UINT:session_id>
 */
cparser_result_t
cparser_cmd_l34_set_pppoe_index_session_id_session_id(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *session_id_ptr)
{
    int32   ret = RT_ERR_FAILED;
    rtk_l34_pppoe_entry_t entry;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&entry,0x0,sizeof(rtk_l34_pppoe_entry_t));
    
    /*session*/
    entry.sessionID = *session_id_ptr;
  
    if((ret = rtk_l34_pppoeTable_set(*index_ptr,&entry)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_pppoe_index_session_id_session_id */

/*
 * l34 get napt 
 */
cparser_result_t
cparser_cmd_l34_get_napt(
    cparser_context_t *context)
{
	rtk_l34_naptOutbound_entry_t napt;
	int32 ret;
	uint32 i;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	for(i=0;i<HAL_L34_NAPT_ENTRY_MAX();i++){
		DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_get(i,&napt),ret);
		diag_util_mprintf("NAPT Table Id:\t%d\n",i);
		diag_print_napt(DISABLED,napt);
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_napt */

/*
 * l34 get napt <UINT:index>
 */

cparser_result_t
cparser_cmd_l34_get_napt_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	rtk_l34_naptOutbound_entry_t napt;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_get(*index_ptr,&napt),ret);

	diag_util_mprintf("NAPT Table Id:\t%d\n",*index_ptr);
	diag_print_napt(ENABLED,napt);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_napt_index */

/*
 * l34 set napt <UINT:index> hash-index <UINT:hash_index>
 */
cparser_result_t
cparser_cmd_l34_set_napt_index_hash_index_hash_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *hash_index_ptr)
{
	int32 ret;
	rtk_l34_naptOutbound_entry_t napt;
	uint8 force = 1;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&napt,0,sizeof(rtk_l34_naptOutbound_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_get(*index_ptr,&napt),ret);
	napt.hashIdx = *hash_index_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_set(force, *index_ptr,&napt),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_napt_index_hash_index_hash_index */

/*
 * l34 set napt <UINT:index> napt-priority state ( disable | enable )  
 */
cparser_result_t
cparser_cmd_l34_set_napt_index_napt_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_l34_naptOutbound_entry_t napt;
	uint8 force = 1;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
   	 if('d' == TOKEN_CHAR(6, 0))
   	 {
   	 	state = DISABLED;
   	 }else{
		state = ENABLED;
	 }

	osal_memset(&napt,0,sizeof(rtk_l34_naptOutbound_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_get(*index_ptr,&napt),ret);
	napt.priValid = state;
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_set(force, *index_ptr,&napt),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_napt_index_napt_priority_state_disable_enable */

/*
 * l34 set napt <UINT:index> napt-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l34_set_napt_index_napt_priority_priority(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *priority_ptr)
{
    	int32 ret;
	rtk_l34_naptOutbound_entry_t napt;
	uint8 force = 1;
	uint32 priValue = *priority_ptr;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&napt,0,sizeof(rtk_l34_naptOutbound_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_get(*index_ptr,&napt),ret);
	napt.priValue= priValue;
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_set(force, *index_ptr,&napt),ret);
	return CPARSER_OK;

}    /* end of cparser_cmd_l34_set_napt_index_napt_priority_priority */

/*
 * l34 set napt <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_napt_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_l34_naptOutbound_entry_t napt;
	uint8 force = 1;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
   	 if('d' == TOKEN_CHAR(5, 0))
   	 {
   	 	state = DISABLED;
   	 }else{
		state = ENABLED;
	 }

	osal_memset(&napt,0,sizeof(rtk_l34_naptOutbound_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_get(*index_ptr,&napt),ret);
	napt.valid= state;
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_set(force, *index_ptr,&napt),ret);
	return CPARSER_OK;

}    /* end of cparser_cmd_l34_set_napt_index_state_disable_enable */

/*
 * l34 del napt <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_napt_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	rtk_l34_naptOutbound_entry_t napt;
	uint8 force = 1;

	DIAG_UTIL_PARAM_CHK();
	
	osal_memset(&napt,0,sizeof(rtk_l34_naptOutbound_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_naptOutboundTable_set(force, *index_ptr,&napt),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_napt_index */

/*
 * l34 get naptr 
 */
cparser_result_t
cparser_cmd_l34_get_naptr(
    cparser_context_t *context)
{
	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	uint32 i;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	for(i=0;i<HAL_L34_NAPTR_ENTRY_MAX();i++){
		osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));
		DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(i,&naptr),ret);
		diag_util_mprintf("NAPTR ID:\t%d\n",i);
		diag_print_naptr(DISABLED,naptr);

	}
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_naptr */

/*
 * l34 get naptr <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_naptr_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);
	diag_util_mprintf("NAPTR ID:\t%d\n",*index_ptr);
	diag_print_naptr(ENABLED,naptr);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_naptr_index */

/*
 * l34 set naptr <UINT:index> internal-ip <IPV4ADDR:ip> internal-port <UINT:port> 
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_internal_ip_ip_internal_port_port(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr,
    uint32_t  *port_ptr)
{
	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	naptr.intIp = (rtk_ip_addr_t)*ip_ptr;
	naptr.intPort = *port_ptr;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_internal_ip_ip_internal_port_port */

/*
 * l34 set naptr <UINT:index> protocol ( tcp | udp )
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_protocol_tcp_udp(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
   	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	 if('t' == TOKEN_CHAR(5, 0))
   	 {
		naptr.isTcp = ENABLED;
	 }else
	 {
	 	naptr.isTcp = DISABLED;
	 }
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_protocol_tcp_udp */

/*
 * l34 set naptr <UINT:index> external-ip <UINT:extip_index> external-port-lsb <UINT:export_lsb>
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_external_ip_extip_index_external_port_lsb_export_lsb(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *extip_index_ptr,
    uint32_t  *export_lsb_ptr)
{
   	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	naptr.extIpIdx = *extip_index_ptr;
	naptr.extPortLSB = *export_lsb_ptr;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_external_ip_extip_index_external_port_lsb_export_lsb */

/*
 * l34 set naptr <UINT:index> naptr-priority state ( disable | enable )  
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_naptr_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
   	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	 if('d' == TOKEN_CHAR(6, 0))
   	 {
   	 	state = DISABLED;
   	 }else{
		state = ENABLED;
	 }
	 naptr.priValid = state;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_naptr_priority_state_disable_enable */

/*
 * l34 set naptr <UINT:index> naptr-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_naptr_priority_priority(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *priority_ptr)
{
   	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	naptr.priId = *priority_ptr;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_naptr_priority_priority */

/*
 * l34 set naptr <UINT:index> remote-hash-type ( none | remote_ip | remote_ip_remote_port )
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_remote_hash_type_none_remote_ip_remote_ip_remote_port(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	 if('n' == TOKEN_CHAR(5, 0))
   	 {
   	 	naptr.valid = 1;
   	 }else 
   	 if('_' == TOKEN_CHAR(5, 9))
   	 {
   	 	naptr.valid = 2;
	 }else
	 {
	 	naptr.valid = 3;
	 }
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_remote_hash_type_none_remote_ip_remote_ip_remote_port */

/*
 * l34 set naptr <UINT:index> hash-value <UINT:value>
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_hash_value_value(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *value_ptr)
{
   	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	naptr.remHash = *value_ptr;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_hash_value_value */

/*
 * l34 set naptr <UINT:index> state disable
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_state_disable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_get(*index_ptr,&naptr),ret);

	naptr.valid = 0;
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_state_disable */

/*
 * l34 del naptr <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_naptr_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_l34_naptInbound_entry_t naptr;
	int8 force = 1;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));
	
	DIAG_UTIL_ERR_CHK(rtk_l34_naptInboundTable_set(force,*index_ptr,&naptr),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_naptr_index */

/*
 * l34 set port ( <PORT_LIST:ports> | all ) netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_set_port_ports_all_netif_index(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
	int32 ret;
	diag_portlist_t portlist;
	rtk_l34_portType_t type = L34_PORT_MAC ;
	rtk_port_t port;
	DIAG_UTIL_PARAM_CHK();

	DIAG_UTIL_PORTMASK_SCAN(portlist, port)
	{
		DIAG_UTIL_ERR_CHK(rtk_l34_lookupPortMap_set( type,port,*index_ptr),ret);
	}
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_port_ports_all_netif_index */

/*
 * l34 get port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l34_get_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
	int32 ret;
	diag_portlist_t portlist;
	rtk_l34_portType_t type = L34_PORT_MAC ;
	rtk_port_t port;
	uint32 wanId;
	DIAG_UTIL_PARAM_CHK();
	diag_util_mprintf("LAN interface Multilayer-Decision-Base Control, Port Base Set!\n");
	DIAG_UTIL_PORTMASK_SCAN(portlist, port)
	{
		DIAG_UTIL_ERR_CHK(rtk_l34_lookupPortMap_get( type,port,&wanId),ret);
		diag_util_mprintf("Port %d Map to WAN %d\n",port,wanId);
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_port_ports_all */

/*
 * l34 set ext ( <PORT_LIST:ext> | all ) netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_set_ext_ext_all_netif_index(
    cparser_context_t *context,
    char * *ext_ptr,
    uint32_t  *index_ptr)
{
    	int32 ret;
	diag_portlist_t portlist;
	rtk_l34_portType_t type = L34_PORT_EXTENSION;
	rtk_port_t port;
	DIAG_UTIL_PARAM_CHK();

	DIAG_UTIL_PORTMASK_SCAN(portlist, port)
	{
		DIAG_UTIL_ERR_CHK(rtk_l34_lookupPortMap_set( type,port,*index_ptr),ret);
	}
    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_ext_ext_all_netif_index */

/*
 * l34 get ext ( <PORT_LIST:ext> | all )
 */
cparser_result_t
cparser_cmd_l34_get_ext_ext_all(
    cparser_context_t *context,
    char * *ext_ptr)
{
    	int32 ret;
	diag_portlist_t portlist;
	rtk_l34_portType_t type = L34_PORT_EXTENSION ;
	rtk_port_t port;
	uint32 wanId;
	DIAG_UTIL_PARAM_CHK();
	diag_util_mprintf("LAN interface Multilayer-Decision-Base Control, Port Base Set!\n");
	DIAG_UTIL_PORTMASK_SCAN(portlist, port)
	{
		DIAG_UTIL_ERR_CHK(rtk_l34_lookupPortMap_get( type,port,&wanId),ret);
		diag_util_mprintf("Ext Port %d Map to WAN %d\n",port,wanId);
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_ext_ext_all */

/*
 * l34 set l4-fragment action ( trap-to-cpu | forward )
 */
cparser_result_t
cparser_cmd_l34_set_l4_fragment_action_trap_to_cpu_forward(
    cparser_context_t *context)
{
	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_FRAG2CPU_STATE ;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(4),"trap-to-cpu")){
		state = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(4),"forward")){
		state = ENABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_l4_fragment_action_trap_to_cpu_forward */

/*
 * l34 get l4-fragment
 */
cparser_result_t
cparser_cmd_l34_get_l4_fragment(
    cparser_context_t *context)
{
	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_FRAG2CPU_STATE ;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	diag_util_mprintf("L34 Global Setting:\n");
	if(state == DISABLED){
		    diag_util_mprintf("L4 Fragement Action: Trap to CPU\n");
	}else{
    		    diag_util_mprintf("L4 Fragement Action: Forward\n");
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l4_fragment */

/*
 * l34 set l3-checksum-error action ( forward | drop )
 */
cparser_result_t
cparser_cmd_l34_set_l3_checksum_error_action_forward_drop(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_L3CHKSERRALLOW_STATE;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(4),"drop")){
		state = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(4),"forward")){
		state = ENABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_l3_checksum_error_action_forward_drop */

/*
 * l34 get l3-checksum-error
 */
cparser_result_t
cparser_cmd_l34_get_l3_checksum_error(
    cparser_context_t *context)
{
   	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_L3CHKSERRALLOW_STATE ;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	diag_util_mprintf("L34 Global Setting:\n");
	if(state == DISABLED){
		    diag_util_mprintf("L3 Checksum Error Action: Drop\n");
	}else{
    		    diag_util_mprintf("L3 Checksum Error Action: Forward\n");
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l3_checksum_error */

/*
 * l34 set l4-checksum-error action ( forward | drop )
 */
cparser_result_t
cparser_cmd_l34_set_l4_checksum_error_action_forward_drop(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_L4CHKSERRALLOW_STATE;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(4),"drop")){
		state = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(4),"forward")){
		state = ENABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_l4_checksum_error_action_forward_drop */

/*
 * l34 get l4-checksum-error
 */
cparser_result_t
cparser_cmd_l34_get_l4_checksum_error(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_L4CHKSERRALLOW_STATE ;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	diag_util_mprintf("L34 Global Setting:\n");
	if(state == DISABLED){
		    diag_util_mprintf("L4 Checksum Error Action: Drop\n");
	}else{
    		    diag_util_mprintf("L4 Checksum Error Action: Forward\n");
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l4_checksum_error */

/*
 * l34 set ttl-minus state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_ttl_minus_state_enable_disable(
    cparser_context_t *context)
{
       int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_TTLMINUS_STATE;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(4),"disable")){
		state = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(4),"enable")){
		state = ENABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_ttl_minus_state_enable_disable */

/*
 * l34 get ttl-minus state
 */
cparser_result_t
cparser_cmd_l34_get_ttl_minus_state(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_TTLMINUS_STATE ;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	diag_util_mprintf("L34 Global Setting:\n");
	if(state == DISABLED){
		    diag_util_mprintf("TTL Minus State: Disable\n");
	}else{
    		    diag_util_mprintf("TTL Minus State: Enable\n");
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_ttl_minus_state */

/*
 * l34 set interface-decision-mode ( vlan-based | port-based | mac-based )
 */
cparser_result_t
cparser_cmd_l34_set_interface_decision_mode_vlan_based_port_based_mac_based(
    cparser_context_t *context)
{
   	int32 ret;
	rtk_l34_lookupMode_t mode;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(3),"vlan-based")){
		mode = L34_LOOKUP_VLAN_BASE;
	}else
	if(!osal_strcmp(TOKEN_STR(3),"port-based")){
		mode = L34_LOOKUP_PORT_BASE;
	}else{
		mode = L34_LOOKUP_MAC_BASE;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_lookupMode_set(mode),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_interface_decision_mode_vlan_based_port_based_mac_based */

/*
 * l34 get interface-decision-mode
 */
cparser_result_t
cparser_cmd_l34_get_interface_decision_mode(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_lookupMode_t mode;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_lookupMode_get(&mode),ret);
	diag_util_mprintf("L34 Global Setting:\n");
	switch(mode){
	case L34_LOOKUP_VLAN_BASE:
		diag_util_mprintf("LAN interface Multilayer-Decision-Base Control: VLAN Based\n");
	break;
	case L34_LOOKUP_PORT_BASE:
		diag_util_mprintf("LAN interface Multilayer-Decision-Base Control: PORT Based\n");
	break;
	case L34_LOOKUP_MAC_BASE:
		diag_util_mprintf("LAN interface Multilayer-Decision-Base Control: MAC Based\n");
	break;
	default:
		diag_util_mprintf("LAN interface Multilayer-Decision-Base Control: Not support\n");
	break;
	}
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_interface_decision_mode */

/*
 * l34 set nat-attack action ( drop | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l34_set_nat_attack_action_drop_trap_to_cpu(
    cparser_context_t *context)
{
     	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_NAT2LOG_STATE;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(4),"drop")){
		state = DISABLED;
	}else
	if(!osal_strcmp(TOKEN_STR(4),"trap-to-cpu")){
		state = ENABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nat_attack_action_drop_trap_to_cpu */

/*
 * l34 get nat-attack
 */
cparser_result_t
cparser_cmd_l34_get_nat_attack(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type =L34_GLOBAL_NAT2LOG_STATE ;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	diag_util_mprintf("L34 Global Setting:\n");
	if(state ==DISABLED){
		    diag_util_mprintf("NAT Attack Log: Disable\n");
	}else{
    		    diag_util_mprintf("NAT Attack Log: Enable\n");
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_nat_attack */

/*
 * l34 set wan-route action ( drop | trap-to-cpu | forward )
 */
cparser_result_t
cparser_cmd_l34_set_wan_route_action_drop_trap_to_cpu_forward(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_wanRouteMode_t act;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(4),"drop")){
		act = L34_WANROUTE_DROP;
	}else
	if(!osal_strcmp(TOKEN_STR(4),"trap-to-cpu")){
		act = L34_WANROUTE_FORWARD2CPU;
	}else{
		act = L34_WANROUTE_FORWARD;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_wanRoutMode_set(act),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_wan_route_action_drop_trap_to_cpu_forward */

/*
 * l34 get wan-route
 */
cparser_result_t
cparser_cmd_l34_get_wan_route(
    cparser_context_t *context)
{
	int32 ret;
	rtk_l34_wanRouteMode_t act;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_wanRoutMode_get(&act),ret);

	diag_util_mprintf("L34 Global Setting:\n");

	switch(act){
	case L34_WANROUTE_FORWARD:
		 diag_util_mprintf("WAN Route Action: Forward\n");
	break;
	case L34_WANROUTE_DROP:
		diag_util_mprintf("WAN Route Action: DROP\n");
	break;
	case L34_WANROUTE_FORWARD2CPU:
		diag_util_mprintf("WAN Route Action: Forward to CPU\n");
	break;
	default:
		diag_util_mprintf("WAN Route Action: Not Supported\n");
	break;
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_wan_route */

/*
 * l34 set route-mode ( l3 | l3-l4 | disable )
 */
cparser_result_t
cparser_cmd_l34_set_route_mode_l3_l3_l4_disable(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_enable_t  state;
	rtk_l34_globalStateType_t type;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(3),"l3")){
		state = ENABLED;
		type = L34_GLOBAL_L3NAT_STATE;
		DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
		state = DISABLED;
		type = L34_GLOBAL_L4NAT_STATE;
		DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	}else
	if(!osal_strcmp(TOKEN_STR(3),"l3-l4")){
		state = ENABLED;
		type = L34_GLOBAL_L4NAT_STATE;
		DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
		state = DISABLED;
		type = L34_GLOBAL_L3NAT_STATE;
		DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	}else{
		state = DISABLED;
		type = L34_GLOBAL_L3NAT_STATE;
		DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
		state = DISABLED;
		type = L34_GLOBAL_L4NAT_STATE;
		DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);
	}
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_route_mode_l3_l3_l4_disable */

/*
 * l34 get route-mode
 */
cparser_result_t
cparser_cmd_l34_get_route_mode(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_enable_t  state;
	char str[32] = "";
	rtk_l34_globalStateType_t type;
	DIAG_UTIL_PARAM_CHK();

	diag_util_mprintf("L34 Global Setting:\n");
	type = L34_GLOBAL_L3NAT_STATE;
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	stateToString(state,str);
	diag_util_mprintf("L3 NAT STATE: %s\n",str);
	type = L34_GLOBAL_L4NAT_STATE;
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_get(type,&state),ret);
	stateToString(state,str);
	diag_util_mprintf("L4 NAT STATE: %s\n",str);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_route_mode */

/*
 * l34 get pppoe-traffic-indicator
 */
cparser_result_t
cparser_cmd_l34_get_pppoe_traffic_indicator(
    cparser_context_t *context)
{
	int32 ret;
	uint32 i;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	for(i=0;i<HAL_L34_PPPOE_ENTRY_MAX();i++)
	{
		DIAG_UTIL_ERR_CHK(rtk_l34_pppTrfIndicator_get(i, &state),ret);
		diag_util_mprintf("PPP Id: %d, Indicator: %d\n",i,state);
	}
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_pppoe_traffic_indicator */

/*
 * l34 get arp-traffic-indicator index <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_arp_traffic_indicator_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l34_arpTrfIndicator_get(*index_ptr, &state),ret);
	diag_util_mprintf("ARP Id: %d, Indicator: %d\n",*index_ptr,state);
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_arp_traffic_indicator_index_index */

/*
 * l34 get arp-traffic-indicator
 */
cparser_result_t
cparser_cmd_l34_get_arp_traffic_indicator(
    cparser_context_t *context)
{
    	int32 ret;
	uint32 i;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	for(i=0;i<HAL_L34_ARP_ENTRY_MAX();i++)
	{
		DIAG_UTIL_ERR_CHK(rtk_l34_arpTrfIndicator_get(i, &state),ret);
		diag_util_mprintf("ARP Id: %d, Indicator: %d\n",i,state);
	}
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_arp_traffic_indicator */

/*
 * l34 reset arp-traffic-indicator ( table0 | table1 )
 */
cparser_result_t
cparser_cmd_l34_reset_arp_traffic_indicator_table0_table1(
    cparser_context_t *context)
{
	rtk_l34_arp_trf_t table;
	int32 ret;
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(3),"table0")){
		table = RTK_L34_ARPTRF_TABLE0;
	}else
	if(!osal_strcmp(TOKEN_STR(3),"table1")){
		table = RTK_L34_ARPTRF_TABLE1;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_hwArpTrfWrkTbl_Clear(table),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_reset_arp_traffic_indicator_table0_table1 */

/*
 * l34 select arp-traffic-indicator ( table0 | table1 )
 */
cparser_result_t
cparser_cmd_l34_select_arp_traffic_indicator_table0_table1(
    cparser_context_t *context)
{
    	rtk_l34_arp_trf_t table;
	int32 ret;
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(3),"table0")){
		table = RTK_L34_ARPTRF_TABLE0;
	}else
	if(!osal_strcmp(TOKEN_STR(3),"table1")){
		table = RTK_L34_ARPTRF_TABLE1;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_hwArpTrfWrkTbl_set(table),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_select_arp_traffic_indicator_table0_table1 */

/*
 * l34 get l4-traffic-indicator index <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_l4_traffic_indicator_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();


	DIAG_UTIL_ERR_CHK(rtk_l34_naptTrfIndicator_get(*index_ptr, &state),ret);
	diag_util_mprintf("NAPT Id: %d, Indicator: %d\n",*index_ptr,state);
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l4_traffic_indicator_index_index */

/*
 * l34 get l4-traffic-indicator
 */
cparser_result_t
cparser_cmd_l34_get_l4_traffic_indicator(
    cparser_context_t *context)
{
    	int32 ret;
	uint32 i;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	for(i=0;i<HAL_L34_NAPT_ENTRY_MAX();i++)
	{
		DIAG_UTIL_ERR_CHK(rtk_l34_naptTrfIndicator_get(i, &state),ret);
		diag_util_mprintf("NAPT Id: %d, Indicator: %d\n",i,state);
	}
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l4_traffic_indicator */

/*
 * l34 reset l4-traffic-indicator ( table0 | table1 )
 */
cparser_result_t
cparser_cmd_l34_reset_l4_traffic_indicator_table0_table1(
    cparser_context_t *context)
{
    	rtk_l34_l4_trf_t table;
	int32 ret;
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(3),"table0")){
		table = RTK_L34_L4TRF_TABLE0;
	}else
	if(!osal_strcmp(TOKEN_STR(3),"table1")){
		table = RTK_L34_L4TRF_TABLE1;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_hwL4TrfWrkTbl_Clear(table),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_reset_l4_traffic_indicator_table0_table1 */

/*
 * l34 select l4-traffic-indicator ( table0 | table1 )
 */
cparser_result_t
cparser_cmd_l34_select_l4_traffic_indicator_table0_table1(
    cparser_context_t *context)
{
    	rtk_l34_l4_trf_t table;
	int32 ret;
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(3),"table0")){
		table = RTK_L34_L4TRF_TABLE0;
	}else
	if(!osal_strcmp(TOKEN_STR(3),"table1")){
		table = RTK_L34_L4TRF_TABLE1;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_hwL4TrfWrkTbl_set(table),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_select_l4_traffic_indicator_table0_table1 */

/*
 * debug l34 set hsb l2-bridge <UINT:l2bridge> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_l2_bridge_l2bridge(
    cparser_context_t *context,
    uint32_t  *l2bridge_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_l2_bridge_l2bridge */

/*
 * debug l34 set hsb ip-fragments <UINT:is_fragments> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_ip_fragments_is_fragments(
    cparser_context_t *context,
    uint32_t  *is_fragments_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_ip_fragments_is_fragments */

/*
 * debug l34 set hsb ip-more-fragments <UINT:is_more> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_ip_more_fragments_is_more(
    cparser_context_t *context,
    uint32_t  *is_more_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_ip_more_fragments_is_more */

/*
 * debug l34 set hsb l4-checksum-ok <UINT:is_ok> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_l4_checksum_ok_is_ok(
    cparser_context_t *context,
    uint32_t  *is_ok_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_l4_checksum_ok_is_ok */

/*
 * debug l34 set hsb l3-checksum-ok <UINT:is_ok> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_l3_checksum_ok_is_ok(
    cparser_context_t *context,
    uint32_t  *is_ok_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_l3_checksum_ok_is_ok */

/*
 * debug l34 set hsb direct-tx <UINT:is_direct_tx> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_direct_tx_is_direct_tx(
    cparser_context_t *context,
    uint32_t  *is_direct_tx_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_direct_tx_is_direct_tx */

/*
 * debug l34 set hsb udp-no-chksum <UINT:udp_no_chk> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_udp_no_chksum_udp_no_chk(
    cparser_context_t *context,
    uint32_t  *udp_no_chk_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_udp_no_chksum_udp_no_chk */

/*
 * debug l34 set hsb parse-fail <UINT:parse_fail> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_parse_fail_parse_fail(
    cparser_context_t *context,
    uint32_t  *parse_fail_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_parse_fail_parse_fail */

/*
 * debug l34 set hsb pppoe-if <UINT:pppoe_if> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_pppoe_if_pppoe_if(
    cparser_context_t *context,
    uint32_t  *pppoe_if_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_pppoe_if_pppoe_if */

/*
 * debug l34 set hsb svlan-if <UINT:svlan_if> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_svlan_if_svlan_if(
    cparser_context_t *context,
    uint32_t  *svlan_if_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_svlan_if_svlan_if */

/*
 * debug l34 set hsb ttls <UINT:ttls> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_ttls_ttls(
    cparser_context_t *context,
    uint32_t  *ttls_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_ttls_ttls */

/*
 * debug l34 set hsb pkt-type <UINT:pkt_type> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_pkt_type_pkt_type(
    cparser_context_t *context,
    uint32_t  *pkt_type_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_pkt_type_pkt_type */

/*
 * debug l34 set hsb tcp-flag <UINT:tcp_flag> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_tcp_flag_tcp_flag(
    cparser_context_t *context,
    uint32_t  *tcp_flag_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_tcp_flag_tcp_flag */

/*
 * debug l34 set hsb cvlan-if <UINT:cvlan_if> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_cvlan_if_cvlan_if(
    cparser_context_t *context,
    uint32_t  *cvlan_if_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_cvlan_if_cvlan_if */

/*
 * debug l34 set hsb source-port <UINT:spa> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_source_port_spa(
    cparser_context_t *context,
    uint32_t  *spa_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_source_port_spa */

/*
 * debug l34 set hsb cvid <UINT:cvid> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_cvid_cvid(
    cparser_context_t *context,
    uint32_t  *cvid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_cvid_cvid */

/*
 * debug l34 set hsb packet-length <UINT:length> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_packet_length_length(
    cparser_context_t *context,
    uint32_t  *length_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_packet_length_length */

/*
 * debug l34 set hsb dport <UINT:dport> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_dport_dport(
    cparser_context_t *context,
    uint32_t  *dport_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_dport_dport */

/*
 * debug l34 set hsb pppoe-id <UINT:pppoe> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_pppoe_id_pppoe(
    cparser_context_t *context,
    uint32_t  *pppoe_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_pppoe_id_pppoe */

/*
 * debug l34 set hsb dip <IPV4ADDR:ip> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_dip_ip(
    cparser_context_t *context,
    uint32_t  *ip_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_dip_ip */

/*
 * debug l34 set hsb sip <IPV4ADDR:ip> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_sip_ip(
    cparser_context_t *context,
    uint32_t  *ip_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_sip_ip */

/*
 * debug l34 set hsb sport <UINT:sport> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_sport_sport(
    cparser_context_t *context,
    uint32_t  *sport_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_sport_sport */

/*
 * debug l34 set hsb dmac <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_dmac_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_dmac_mac */

/*
 * debug l34 get hsb
 */
cparser_result_t
cparser_cmd_debug_l34_get_hsb(
    cparser_context_t *context)
{
     int32 ret = RT_ERR_FAILED;
    rtk_l34_hsb_t hsb_data;
    uint32  ip_tmp_buf[UTIL_IP_TMP_BUFFER_LENGTH];
    char tmpStr[128]="";
    uint32_t spa_data;

    memset(&hsb_data,0x0,sizeof(rtk_l34_hsb_t));
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    /*call hsb API*/
    if((ret = rtk_l34_hsbData_get(&hsb_data)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("\n l2bridbe            :%-4d",hsb_data.l2bridge);
    diag_util_printf("\n ipfrags             :%-4d",hsb_data.ipfrag_s);
    diag_util_printf("\n ipmf                :%-4d",hsb_data.ipmf);
    diag_util_printf("\n l4chk               :%-4d",hsb_data.l4_chksum_ok);
    diag_util_printf("\n l3chk               :%-4d",hsb_data.l3_chksum_ok);
    diag_util_printf("\n direct_tx           :%-4d",hsb_data.cpu_direct_tx);
    diag_util_printf("\n udp_no_chk          :%-4d",hsb_data.udp_no_chksum);
    diag_util_printf("\n parse_fail          :%-4d",hsb_data.parse_fail);
    diag_util_printf("\n pppoe_if            :%-4d",hsb_data.pppoe_if);
    diag_util_printf("\n svlan_if            :%-4d",hsb_data.svlan_if);
    diag_util_printf("\n ttls                :%-4d",hsb_data.ttls);
    diag_util_printf("\n cvlan_if            :%-4d",hsb_data.cvlan_if);
    diag_util_printf("\n SPA                 :%-4d",hsb_data.spa);   
    diag_util_printf("\n cvid                :%-4d",hsb_data.cvid);
    diag_util_printf("\n len                 :%-4d",hsb_data.len);
    diag_util_printf("\n dport_l4chksum      :%-4d",hsb_data.dport_l4chksum);
    diag_util_printf("\n pppoe_id            :%-4d",hsb_data.pppoe_id);
    diag_util_printf("\n sport_icmpid_chksum :%-4d",hsb_data.sport_icmpid_chksum);
    diag_util_printf("\n tcp_flag            :0x%-4x",hsb_data.tcp_flag);

    /*DIP*/    
    diag_util_ipv62str((uint8*)ip_tmp_buf, (uint8*)hsb_data.dip.ipv6_addr);
    diag_util_printf("\n DIP                 :%s",ip_tmp_buf);

    /*SIP*/    
    diag_util_ip2str((uint8*)ip_tmp_buf, (uint32)hsb_data.sip);
    diag_util_printf("\n DIP                 :%s",ip_tmp_buf);
 
    /*DMAC*/
    diag_util_mac2str((uint8*)tmpStr,hsb_data.dmac.octet);
    diag_util_printf("\n DMAC                :%s \n",tmpStr);   
  
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_get_hsb */

/*
 * debug l34 get hsa
 */
cparser_result_t
cparser_cmd_debug_l34_get_hsa(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_l34_hsa_t hsa_data;
    uint32  ip_tmp_buf[UTIL_IP_TMP_BUFFER_LENGTH];
    uint32_t spa_data;

    memset(&hsa_data,0x0,sizeof(apollomp_raw_l34_hsa_t));
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    /*call API*/
    if((ret = rtk_l34_hsaData_get(&hsa_data)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    
    
    diag_util_printf("\n l34trans            :%-4d",hsa_data.l34trans);    
    diag_util_printf("\n l2trans             :%-4d",hsa_data.l2trans);    
    diag_util_printf("\n interVlan_if       :%-4d",hsa_data.interVlanIf);    
    diag_util_printf("\n l4_pri_valid        :%-4d",hsa_data.l4_pri_valid);    
    diag_util_printf("\n frag                :%-4d",hsa_data.frag);    
    diag_util_printf("\n pppoe_if            :%-4d",hsa_data.pppoe_if);    
    diag_util_printf("\n action              :%-4d",hsa_data.action);    
    diag_util_printf("\n pppid_idx           :%-4d",hsa_data.pppid_idx);    
    diag_util_printf("\n difid               :%-4d",hsa_data.difid);    
    diag_util_printf("\n l4_pri_sel          :%-4d",hsa_data.l4_pri_sel);    
    diag_util_printf("\n dvid                :%-4d",hsa_data.dvid);    
    diag_util_printf("\n reason              :%-4d",hsa_data.reason);
    diag_util_printf("\n nexthop_mac_idx     :%-4d",hsa_data.nexthop_mac_idx);    
    diag_util_printf("\n l4_chksum           :0x%-6x",hsa_data.l4_chksum);    
    diag_util_printf("\n l3_chksum           :0x%-4x",hsa_data.l3_chksum);    
    diag_util_printf("\n port                :%-4d",hsa_data.port);    
    /*IP address*/
    diag_util_ip2str((uint8*)ip_tmp_buf, (uint32)hsa_data.ip);
    diag_util_printf("\n IP                  :%s\n",ip_tmp_buf);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_get_hsa */

/*
 * debug l34 set hsba log-mode <UINT:mode>
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsba_log_mode_mode(
    cparser_context_t *context,
    uint32_t  *mode_ptr)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    
    DIAG_UTIL_PARAM_RANGE_CHK((*mode_ptr > 5),RT_ERR_INPUT);
    
    /*call API*/
    if((ret = rtk_l34_hsabMode_set(*mode_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsba_log_mode_mode */

/*
 * debug l34 get hsba log-mode 
 */
cparser_result_t
cparser_cmd_debug_l34_get_hsba_log_mode(
    cparser_context_t *context)
{
     int32 ret = RT_ERR_FAILED;
    uint32_t  mode;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

   
    /*call API*/
    if((ret = rtk_l34_hsabMode_get(&mode)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    
    diag_util_printf(" hsba log mode             :%d\n",mode);
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_get_hsba_log_mode */


/*
 * l34 get routing6  
 */
cparser_result_t
cparser_cmd_l34_get_routing6(
    cparser_context_t *context)
{
	uint32 index;
	int32   ret = RT_ERR_FAILED;
    	rtk_ipv6Routing_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	_cparser_cmd_l34_route6_table_header_display();

	for(index = 0; index < HAL_L34_IPV6_ROUTING_ENTRY_MAX(); index++)
	{
	    osal_memset(&entry,0,sizeof(rtk_ipv6Routing_entry_t));
	    if((ret = rtk_l34_ipv6RoutingTable_get(index,&entry)) != RT_ERR_OK)
	    {
	        DIAG_ERR_PRINT(ret);
	        return CPARSER_NOT_OK;
	    }
	    
	    _cparser_cmd_l34_route6_table_display(index,&entry);		
	}    
	return CPARSER_NOT_OK;
}    /* end of cparser_cmd_l34_get_routing6 */

/*
 * l34 get routing6 <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_routing6_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	rtk_ipv6Routing_entry_t entry;
	int32   ret = RT_ERR_FAILED;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	osal_memset(&entry,0,sizeof(rtk_ipv6Routing_entry_t));
	if((ret = rtk_l34_ipv6RoutingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}
	_cparser_cmd_l34_route6_table_header_display();
	_cparser_cmd_l34_route6_table_display(*index_ptr,&entry);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_routing6_index */

/*
 * l34 get neighbor  
 */
cparser_result_t
cparser_cmd_l34_get_neighbor(
    cparser_context_t *context)
{
	uint32 index;
	int32   ret = RT_ERR_FAILED;
	rtk_ipv6Neighbor_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	_cparser_cmd_l34_neigh_table_header_display();

	for(index = 0; index < HAL_L34_IPV6_NEIGHBOR_ENTRY_MAX(); index++)
	{
	    osal_memset(&entry,0,sizeof(rtk_ipv6Neighbor_entry_t));
	    if((ret = rtk_l34_ipv6NeighborTable_get(index,&entry)) != RT_ERR_OK)
	    {
	        DIAG_ERR_PRINT(ret);
	        return CPARSER_NOT_OK;
	    }
	    
	    _cparser_cmd_l34_neigh_table_display(index,&entry);		
	}    
	return CPARSER_NOT_OK;
}    /* end of cparser_cmd_l34_get_neighbor */

/*
 * l34 get neighbor <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_neighbor_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	rtk_ipv6Neighbor_entry_t entry;
	int32   ret = RT_ERR_FAILED;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	osal_memset(&entry,0,sizeof(rtk_ipv6Neighbor_entry_t));
	if((ret = rtk_l34_ipv6NeighborTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}
	_cparser_cmd_l34_neigh_table_header_display();
	_cparser_cmd_l34_neigh_table_display(*index_ptr,&entry);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_neighbor_index */

/*
 * l34 get binding  
 */
cparser_result_t
cparser_cmd_l34_get_binding(
    cparser_context_t *context)
{
    	uint32 index;
	int32   ret = RT_ERR_FAILED;
	rtk_binding_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	_cparser_cmd_l34_binding_table_header_display();

	for(index = 0; index < HAL_L34_BINDING_ENTRY_MAX(); index++)
	{
	    osal_memset(&entry,0,sizeof(rtk_binding_entry_t));
	    if((ret = rtk_l34_bindingTable_get(index,&entry)) != RT_ERR_OK)
	    {
	        DIAG_ERR_PRINT(ret);
	        return CPARSER_NOT_OK;
	    }
	    
	    _cparser_cmd_l34_binding_table_display(index,&entry);		
	}    
	return CPARSER_NOT_OK;
}    /* end of cparser_cmd_l34_get_binding */

/*
 * l34 get binding <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_binding_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	rtk_binding_entry_t entry;
	int32   ret = RT_ERR_FAILED;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&entry,0,sizeof(rtk_binding_entry_t));
	if((ret = rtk_l34_bindingTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}
	_cparser_cmd_l34_binding_table_header_display();
	_cparser_cmd_l34_binding_table_display(*index_ptr,&entry);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_binding_index */

/*
 * l34 get wan-type  
 */
cparser_result_t
cparser_cmd_l34_get_wan_type(
    cparser_context_t *context)
{
    	uint32 index;
	int32   ret = RT_ERR_FAILED;
	rtk_wanType_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	_cparser_cmd_l34_wanType_table_header_display();

	for(index = 0; index < HAL_L34_WAN_TYPE_ENTRY_MAX(); index++)
	{
	    osal_memset(&entry,0,sizeof(rtk_wanType_entry_t));
	    if((ret = rtk_l34_wanTypeTable_get(index,&entry)) != RT_ERR_OK)
	    {
	        DIAG_ERR_PRINT(ret);
	        return CPARSER_NOT_OK;
	    }
	    
	    _cparser_cmd_l34_wanType_table_display(index,&entry);		
	}    
	return CPARSER_NOT_OK;
}    /* end of cparser_cmd_l34_get_wan_type */

/*
 * l34 get wan-type <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_wan_type_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	rtk_wanType_entry_t entry;
	int32   ret = RT_ERR_FAILED;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	osal_memset(&entry,0,sizeof(rtk_wanType_entry_t));
	if((ret = rtk_l34_wanTypeTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}
	_cparser_cmd_l34_wanType_table_header_display();
	_cparser_cmd_l34_wanType_table_display(*index_ptr,&entry);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_wan_type_index */


/*
 * l34 set neighbor <UINT:index> ifId <UINT:ifIdValue>
 */
cparser_result_t
cparser_cmd_l34_set_neighbor_index_ifId_ifIdValue(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ifIdValue_ptr)
{
    	rtk_ipv6Neighbor_entry_t entry;
	int32   ret = RT_ERR_FAILED;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	osal_memset(&entry,0,sizeof(rtk_ipv6Neighbor_entry_t));
	if((ret = rtk_l34_ipv6NeighborTable_get(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}

	entry.ipv6Ifid = *ifIdValue_ptr;
	if((ret = rtk_l34_ipv6NeighborTable_set(*index_ptr,&entry)) != RT_ERR_OK)
	{
		DIAG_ERR_PRINT(ret);
		return CPARSER_NOT_OK;
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_neighbor_index_ifid_ifidvalue */


/*
 * l34 set binding <UINT:index>  wan-type <UINT:wanTypeId>
 */
cparser_result_t
cparser_cmd_l34_set_binding_index_wan_type_wanTypeId(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *wanTypeId_ptr)
{
	int32 ret;
	rtk_binding_entry_t entry;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&entry,0,sizeof(rtk_binding_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_get(*index_ptr, &entry),ret);

	entry.wanTypeIdx = *wanTypeId_ptr;

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_set(*index_ptr, &entry),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_index_wan_type_wantypeid */

/*
 * l34 set binding <UINT:index> protocol ( all | ipv4-other | ipv6-other | other-only  )
 */
cparser_result_t
cparser_cmd_l34_set_binding_index_protocol_all_ipv4_other_ipv6_other_other_only(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	rtk_binding_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_binding_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_get(*index_ptr, &entry),ret);

	if(!osal_strcmp(TOKEN_STR(5),"all")){
		entry.bindProto = L34_BIND_PROTO_ALL;
    	}else
    	if(!osal_strcmp(TOKEN_STR(5),"ipv4-other"))
	{
		entry.bindProto = L34_BIND_PROTO_NOT_IPV6;
	}else
    	if(!osal_strcmp(TOKEN_STR(5),"ipv6-other"))
	{
		entry.bindProto = L34_BIND_PROTO_NOT_IPV4;
	}else
    	if(!osal_strcmp(TOKEN_STR(5),"other-only"))
	{
		entry.bindProto = L34_BIND_PROTO_NOT_IPV4_IPV6;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_set(*index_ptr, &entry),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_index_protocol_all_ipv4_other_ipv6_other_other_only */

/*
 * l34 set binding <UINT:index> vid <UINT:vlanId>
 */
cparser_result_t
cparser_cmd_l34_set_binding_index_vid_vlanId(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *vlanId_ptr)
{
    	int32 ret;
	rtk_binding_entry_t entry;
	DIAG_UTIL_PARAM_CHK();
	osal_memset(&entry,0,sizeof(rtk_binding_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_get(*index_ptr, &entry),ret);

	entry.vidLan = *vlanId_ptr;

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_set(*index_ptr, &entry),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_index_vid_vlanid */

/*
 * l34 set binding <UINT:index> port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l34_set_binding_index_port_ports_all(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    char * *ports_ptr)
{
	int32 ret;
	diag_portlist_t portlist;
	rtk_binding_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

	osal_memset(&entry,0,sizeof(rtk_binding_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_get(*index_ptr, &entry),ret);

	RTK_PORTMASK_FROM_UINT_PORTMASK((&(entry.portMask)), &(portlist.portmask.bits[0]));
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_set(*index_ptr, &entry),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_index_port_ports_all */

/*
 * l34 set binding <UINT:index> ext-port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l34_set_binding_index_ext_port_ports_all(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    char * *ports_ptr)
{
    	int32 ret;
	diag_portlist_t portlist;
	rtk_binding_entry_t entry;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

	osal_memset(&entry,0,sizeof(rtk_binding_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_get(*index_ptr, &entry),ret);

	RTK_PORTMASK_FROM_UINT_PORTMASK((&(entry.extPortMask)), &(portlist.portmask.bits[0]));
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingTable_set(*index_ptr, &entry),ret);
	return CPARSER_OK;

}    /* end of cparser_cmd_l34_set_binding_index_ext_port_ports_all */

/*
 * l34 set wan-type <UINT:index> type ( l2-bridge | l3-routing | l34-nat-route | l34-customize )
 */
cparser_result_t
cparser_cmd_l34_set_wan_type_index_type_l2_bridge_l3_routing_l34_nat_route_l34_customize(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_wanType_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_wanType_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_wanTypeTable_get(*index_ptr, &entry),ret);

	if(!osal_strcmp(TOKEN_STR(5),"l2-bridge")){
		entry.wanType = L34_WAN_TYPE_L2_BRIDGE;
    	}else
    	if(!osal_strcmp(TOKEN_STR(5),"l3-routing"))
	{
		entry.wanType = L34_WAN_TYPE_L3_ROUTE;
	}else
    	if(!osal_strcmp(TOKEN_STR(5),"l34-nat-route"))
	{
		entry.wanType = L34_WAN_TYPE_L34NAT_ROUTE;
	}else
    	if(!osal_strcmp(TOKEN_STR(5),"l34-customize"))
	{
		entry.wanType = L34_WAN_TYPE_L34_CUSTOMIZED;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_wanTypeTable_set(*index_ptr, &entry),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_wan_type_index_type_l2_bridge_l3_routing_l34_nat_route_l34_customize */

/*
 * l34 set wan-type <UINT:index> next-hop <UINT:nexthopId>
 */
cparser_result_t
cparser_cmd_l34_set_wan_type_index_next_hop_nexthopId(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *nexthopId_ptr)
{
    	int32 ret;
	rtk_wanType_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&entry,0,sizeof(rtk_wanType_entry_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_wanTypeTable_get(*index_ptr, &entry),ret);
	entry.nhIdx = *nexthopId_ptr;
	DIAG_UTIL_ERR_CHK(rtk_l34_wanTypeTable_set(*index_ptr, &entry),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_wan_type_index_next_hop_nexthopid */



/*
 * l34 set binding unmatch-type ( l2tol3 | l2tol34 ) act ( drop | trap | force-l2-bridge ) 
 */
cparser_result_t
cparser_cmd_l34_set_binding_unmatch_type_l2tol3_l2tol34_act_drop_trap_force_l2_bridge(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	
	DIAG_UTIL_PARAM_CHK();


	if(!osal_strcmp(TOKEN_STR(4),"l2tol3")){
		type = L34_BIND_UNMATCHED_L2L3;
    	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l2tol34"))
	{
		type = L34_BIND_UNMATCHED_L2L34;
	}
	
	if(!osal_strcmp(TOKEN_STR(6),"drop")){
		act= L34_BIND_ACT_DROP;
    	}else
    	if(!osal_strcmp(TOKEN_STR(6),"trap"))
	{
		act = L34_BIND_ACT_TRAP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"force-l2-bridge"))
	{
		act= L34_BIND_ACT_FORCE_L2BRIDGE;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_set(type,act),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_unmatch_type_l2tol3_l2tol34_act_drop_trap_force_l2_bridge */

/*
 * l34 set binding unmatch-type ( l3tol2 | l34tol2 ) act ( drop | trap | permit-l2-bridge ) 
 */
cparser_result_t
cparser_cmd_l34_set_binding_unmatch_type_l3tol2_l34tol2_act_drop_trap_permit_l2_bridge(
    cparser_context_t *context)
{
   	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	
	DIAG_UTIL_PARAM_CHK();


	if(!osal_strcmp(TOKEN_STR(4),"l3tol2")){
		type = L34_BIND_UNMATCHED_L3L2;
    	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l34tol2"))
	{
		type = L34_BIND_UNMATCHED_L34L2;
	}
	
	if(!osal_strcmp(TOKEN_STR(6),"drop")){
		act= L34_BIND_ACT_DROP;
    	}else
    	if(!osal_strcmp(TOKEN_STR(6),"trap"))
	{
		act = L34_BIND_ACT_TRAP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"permit-l2-bridge"))
	{
		act= L34_BIND_ACT_PERMIT_L2BRIDGE;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_set(type,act),ret);

	return CPARSER_OK;
}  


/*
 * l34 set binding unmatch-type  l3tol34 act ( trap | force-l4-lookup ) 
 */
cparser_result_t
cparser_cmd_l34_set_binding_unmatch_type_l3tol34_act_trap_force_l4_lookup(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	
	DIAG_UTIL_PARAM_CHK();

	type = L34_BIND_UNMATCHED_L3L34;
	
    	if(!osal_strcmp(TOKEN_STR(6),"trap"))
	{
		act = L34_BIND_ACT_TRAP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"force-l4-lookup"))
	{
		act= L34_BIND_ACT_IPV4_LOOKUPL4TABLE_IPV6_TRAP;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_set(type,act),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_unmatch_type_l3tol34_act_trap_force_l4_lookup */

/*
 * l34 set binding unmatch-type  ( l34tol3 | l3tol3 ) act ( trap | force-l3 ) 
 */
cparser_result_t
cparser_cmd_l34_set_binding_unmatch_type_l34tol3_l3tol3_act_trap_force_l3(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	
	DIAG_UTIL_PARAM_CHK();


	if(!osal_strcmp(TOKEN_STR(4),"l34tol3")){
		type = L34_BIND_UNMATCHED_L34L3;
    	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l3tol3"))
	{
		type = L34_BIND_UNMATCHED_L3L3;
	}
	

    	if(!osal_strcmp(TOKEN_STR(6),"trap"))
	{
		act = L34_BIND_ACT_TRAP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"force-l3"))
	{
		act= L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_set(type,act),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_unmatch_type_l34tol3_l3tol3_act_trap_force_l3 */

/*
 * l34 set binding customized  l2 act ( trap | drop | permit-l2-bridge ) 
 */
cparser_result_t
cparser_cmd_l34_set_binding_customized_l2_act_trap_drop_permit_l2_bridge(
    cparser_context_t *context)
{
   	 int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	
	DIAG_UTIL_PARAM_CHK();

	type = L34_BIND_CUSTOMIZED_L2;
	
    	if(!osal_strcmp(TOKEN_STR(6),"trap"))
	{
		act = L34_BIND_ACT_TRAP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"drop"))
	{
		act= L34_BIND_ACT_DROP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"permit-l2-bridge"))
	{
		act= L34_BIND_ACT_PERMIT_L2BRIDGE;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_set(type,act),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_customized_l2_act_trap_drop_permit_l2_bridge */

/*
 * l34 set binding customized  l3 act ( trap | drop | force-l3 | force-L4-lookup ) 
 */
cparser_result_t
cparser_cmd_l34_set_binding_customized_l3_act_trap_drop_force_l3_force_L4_lookup(
    cparser_context_t *context)
{
     	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	
	DIAG_UTIL_PARAM_CHK();

	type = L34_BIND_CUSTOMIZED_L3;
	
    	if(!osal_strcmp(TOKEN_STR(6),"trap"))
	{
		act = L34_BIND_ACT_TRAP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"drop"))
	{
		act= L34_BIND_ACT_DROP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"force-l3"))
	{
		act= L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"force-L4-lookup"))
	{
		act= L34_BIND_ACT_IPV4_LOOKUPL4TABLE_IPV6_TRAP;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_set(type,act),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_customized_l3_act_trap_drop_force_l3_force_l4_lookup */

/*
 * l34 set binding customized  l34 act ( trap | drop | force-l3 | normal ) 
 */
cparser_result_t
cparser_cmd_l34_set_binding_customized_l34_act_trap_drop_force_l3_normal(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	
	DIAG_UTIL_PARAM_CHK();

	type = L34_BIND_CUSTOMIZED_L34;
	
    	if(!osal_strcmp(TOKEN_STR(6),"trap"))
	{
		act = L34_BIND_ACT_TRAP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"drop"))
	{
		act= L34_BIND_ACT_DROP;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"force-l3"))
	{
		act= L34_BIND_ACT_FORCE_BINDL3_SKIP_LOOKUPL4;
	}else
    	if(!osal_strcmp(TOKEN_STR(6),"normal"))
	{
		act= L34_BIND_ACT_NORMAL_LOOKUPL34;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_set(type,act),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_binding_customized_l34_act_trap_drop_force_l3_normal */

/*
 * l34 get binding unmatch-type (  l2tol3 | l2tol34 | l3tol2 | l3tol34 | l34tol2 | l34tol3 | l3tol3 )
 */
cparser_result_t
cparser_cmd_l34_get_binding_unmatch_type_l2tol3_l2tol34_l3tol2_l3tol34_l34tol2_l34tol3_l3tol3(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	char tmp[128]="";
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	if(!osal_strcmp(TOKEN_STR(4),"l2tol3")){
		type = L34_BIND_UNMATCHED_L2L3;
    	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l2tol34"))
	{
		type = L34_BIND_UNMATCHED_L2L34;
	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l3tol2"))
	{
		type = L34_BIND_UNMATCHED_L3L2;
	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l3tol34"))
	{
		type = L34_BIND_UNMATCHED_L3L34;
	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l34tol2"))
	{
		type = L34_BIND_UNMATCHED_L34L2;
	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l34tol3"))
	{
		type = L34_BIND_UNMATCHED_L34L3;
	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l3tol3"))
	{
		type = L34_BIND_UNMATCHED_L3L3;
	}


	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_get(type,&act),ret);
	_diag_util_bindingAct2Str(tmp,act);
	
	diag_util_mprintf("Action:\t%s\n",tmp);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_binding_unmatch_type_l2tol3_l2tol34_l3tol2_l3tol34_l34tol2_l34tol3_l3tol3 */


/*
 * l34 get binding customized (  l2 | l3 | l34 )
 */
cparser_result_t
cparser_cmd_l34_get_binding_customized_l2_l3_l34(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_bindAct_t act;
	rtk_l34_bindType_t type;
	char tmp[128]="";
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	if(!osal_strcmp(TOKEN_STR(4),"l2")){
		type = L34_BIND_CUSTOMIZED_L2;
    	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l3"))
	{
		type = L34_BIND_CUSTOMIZED_L3;
	}else
    	if(!osal_strcmp(TOKEN_STR(4),"l34"))
	{
		type = L34_BIND_CUSTOMIZED_L34;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_bindingAction_get(type,&act),ret);
	_diag_util_bindingAct2Str(tmp,act);
	
	diag_util_mprintf("Action:\t%s\n",tmp);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_binding_customized_l2_l3_l34 */

/*
 * l34 set map port <UINT:portId> wan <UINT:wanId> state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_map_port_portId_wan_wanId_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *portId_ptr,
    uint32_t  *wanId_ptr)
{
	int32 ret;
	rtk_l34_portWanMapType_t type = L34_PORTMAP_TYPE_PORT2WAN;
	rtk_l34_portWanMap_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	entry.port = *portId_ptr;
	entry.wanIdx = *wanId_ptr;

	if(!osal_strcmp(TOKEN_STR(8),"enable")){
		entry.act = L34_PORTMAP_ACT_PERMIT;
	}else
	{
		entry.act = L34_PORTMAP_ACT_DROP;
	}
	
	DIAG_UTIL_ERR_CHK(rtk_l34_portWanMap_set(type,entry),ret);
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_map_port_portid_wan_wanid_state_enable_disable */

/*
 * l34 set map ext <UINT:extId> wan <UINT:wanId> state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_map_ext_extId_wan_wanId_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *extId_ptr,
    uint32_t  *wanId_ptr)
{
    	int32 ret;
	rtk_l34_portWanMapType_t type = L34_PORTMAP_TYPE_EXT2WAN;
	rtk_l34_portWanMap_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	entry.port = *extId_ptr;
	entry.wanIdx = *wanId_ptr;

	if(!osal_strcmp(TOKEN_STR(8),"enable")){
		entry.act = L34_PORTMAP_ACT_PERMIT;
	}else
	{
		entry.act = L34_PORTMAP_ACT_DROP;
	}
	
	DIAG_UTIL_ERR_CHK(rtk_l34_portWanMap_set(type,entry),ret);
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_map_ext_extid_wan_wanid_state_enable_disable */

/*
 * l34 set map wan <UINT:wanId> port <UINT:portId> state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_map_wan_wanId_port_portId_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *wanId_ptr,
    uint32_t  *portId_ptr)
{
    	int32 ret;
	rtk_l34_portWanMapType_t type = L34_PORTMAP_TYPE_WAN2PORT;
	rtk_l34_portWanMap_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	entry.port = *portId_ptr;
	entry.wanIdx = *wanId_ptr;

	if(!osal_strcmp(TOKEN_STR(8),"enable")){
		entry.act = L34_PORTMAP_ACT_PERMIT;
	}else
	{
		entry.act = L34_PORTMAP_ACT_DROP;
	}
	
	DIAG_UTIL_ERR_CHK(rtk_l34_portWanMap_set(type,entry),ret);
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_map_wan_wanid_port_portid_state_enable_disable */

/*
 * l34 set map wan <UINT:wanId> ext <UINT:extId> state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_map_wan_wanId_ext_extId_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *wanId_ptr,
    uint32_t  *extId_ptr)
{
    	int32 ret;
	rtk_l34_portWanMapType_t type = L34_PORTMAP_TYPE_WAN2EXT;
	rtk_l34_portWanMap_entry_t entry;
	DIAG_UTIL_PARAM_CHK();

	entry.port = *extId_ptr;
	entry.wanIdx = *wanId_ptr;

	if(!osal_strcmp(TOKEN_STR(8),"enable")){
		entry.act = L34_PORTMAP_ACT_PERMIT;
	}else
	{
		entry.act = L34_PORTMAP_ACT_DROP;
	}
	
	DIAG_UTIL_ERR_CHK(rtk_l34_portWanMap_set(type,entry),ret);
	
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_map_wan_wanid_ext_extid_state_enable_disable */


/*
 * l34 get map ( port-to-wan | ext-to-wan | wan-to-port | wan-to-ext ) index <UINT:tableId>
 */
cparser_result_t
cparser_cmd_l34_get_map_port_to_wan_ext_to_wan_wan_to_port_wan_to_ext_index_tableId(
    cparser_context_t *context,
    uint32_t  *tableId_ptr)
{
    	int32 ret;
	rtk_l34_portWanMapType_t type;
	rtk_l34_portWanMap_entry_t entry;
	uint32 num=0,i;
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(3),"port-to-wan")){
		 type = L34_PORTMAP_TYPE_PORT2WAN;
		 num = HAL_L34_NETIF_ENTRY_MAX();
		 diag_util_mprintf("Port %d to WAN Mapping State:\n",*tableId_ptr);
	}else
	if(!osal_strcmp(TOKEN_STR(3),"ext-to-wan"))
	{
		type = L34_PORTMAP_TYPE_EXT2WAN;
		num = HAL_L34_NETIF_ENTRY_MAX();
		diag_util_mprintf("Ext-Port %d to WAN Mapping State:\n",*tableId_ptr);

	}else
	if(!osal_strcmp(TOKEN_STR(3),"wan-to-port"))
	{
		type = L34_PORTMAP_TYPE_WAN2PORT;
		num = HAL_GET_MAX_PORT();
		diag_util_mprintf("WAN %d to Port Mapping State:\n",*tableId_ptr);
	}else
	if(!osal_strcmp(TOKEN_STR(3),"wan-to-ext"))
	{
		type = L34_PORTMAP_TYPE_WAN2EXT;
		num = HAL_GET_MAX_EXT_PORT();
		diag_util_mprintf("WAN %d to Ext-Port Mapping State:\n",*tableId_ptr);

	}
	
	if(type == L34_PORTMAP_TYPE_WAN2EXT || type == L34_PORTMAP_TYPE_WAN2PORT)
	{
		entry.wanIdx = *tableId_ptr;
		for(i=0;i<num;i++)
		{	
			entry.port = i;
			DIAG_UTIL_ERR_CHK(rtk_l34_portWanMap_get(type,&entry),ret);
			if(entry.act==L34_PORTMAP_ACT_PERMIT){
				diag_util_mprintf("PortId:%d\t,State:Enable\n",i);
			}else
			{
				diag_util_mprintf("PortId:%d\t,State:Disable\n",i);
			}
		}
	}else
	{
		entry.port = *tableId_ptr;
		for(i=0;i<num;i++)
		{	
			entry.wanIdx= i;
			DIAG_UTIL_ERR_CHK(rtk_l34_portWanMap_get(type,&entry),ret);
		if(entry.act==L34_PORTMAP_ACT_PERMIT){
			diag_util_mprintf("WANId:%d\t,State:Enable\n",i);
		}else
		{
			diag_util_mprintf("WANId:%d\t,State:Disable\n",i);
		}
		}
	}


	return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_map_port_to_wan_ext_to_wan_wan_to_port_wan_to_ext_index_tableid */



