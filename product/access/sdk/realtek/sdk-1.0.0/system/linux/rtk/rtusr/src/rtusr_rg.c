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
 * Purpose : Definition of LiteRomeDriver RG API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) LiteRomeDriver System configuration
 *           (2) LiteRomeDriver Lan Interface configuration
 *           (3) LiteRomeDriver Wan Interface configuration
 *           (4) LiteRomeDriver Interface Utility configuration
 *           (5) LiteRomeDriver VLAN Binding configuration
 *           (6) LiteRomeDriver ALG configuration
 *           (7) LiteRomeDriver DMZ configuration
 *           (8) LiteRomeDriver VirtualServer configuration
 *           (9) LiteRomeDriver ACL Filter configuration
 *           (10) LiteRomeDriver Mac Filter configuration
 *           (11) LiteRomeDriver URL Filter configuration
 *           (12) LiteRomeDriver UPnP configuration
 *           (13) LiteRomeDriver NAPT configuration
 *           (14) LiteRomeDriver Multicast Flow configuration
 *           (15) LiteRomeDriver Layer2 configuration
*/


/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>

#include <common/error.h>
#include <common/rt_type.h>
#include <rtk/port.h>
#ifdef CONFIG_APOLLO_RLE0371
#include <hal/chipdef/apollo/apollo_def.h>
#else
#include <hal/chipdef/apollomp/apollomp_def.h>
#endif
#include <rtk_rg_define.h>
#include <rtk_rg_struct.h>

/*
 * Function Declaration
 */

/* Module Name    : LiteRomeDriver   */
/* Sub-module Name: System configuration */

/* Function Name:
 *      rtk_rg_driverVersion_get
 * Description:
 *      Get the RG rome driver version number.
 * Input:
 * Output:
 *      version_string - [in]<tab>The pointer of version structure.<nl>[out]<tab>The pointer to the stucture with version string.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NULL_POINTER - the buffer parameter may be NULL
 * Note:
 *      version_string.version_string - the char string of version, at most 20 characters.
 */
int32 
rtk_rg_driverVersion_get(rtk_rg_VersionString_t *version_string)
{
    rtdrv_rg_driverVersion_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == version_string), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.version_string, version_string, sizeof(rtk_rg_VersionString_t));
    GETSOCKOPT(RTDRV_RG_DRIVERVERSION_GET, &cfg, rtdrv_rg_driverVersion_get_t, 1);
    osal_memcpy(version_string, &cfg.version_string, sizeof(rtk_rg_VersionString_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_driverVersion_get */

/* Function Name:
 *      rtk_rg_initParam_get
 * Description:
 *      Get the initialized call-back functions.
 * Input:
 * Output:
 *      init_param - [in]<tab>The instance of rtk_rg_initParams_t.<nl>[out]<tab>The instance of rtk_rg_initParams_t with saved call-back function pointers.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NULL_POINTER - the buffer parameter may be NULL 
 *      RT_ERR_RG_INITPM_UNINIT - init parameter structure are all NULL pointers
 * Note:
 *      init_param.arpAddByHwCallBack - this call-back function pointer should be called when ARP entry added.<nl>
 *      init_param.arpDelByHwCallBack - this call-back function pointer should be called when ARP entry deleted.<nl>
 *      init_param.macAddByHwCallBack - this call-back function pointer should be called when MAC entry added.<nl>
 *      init_param.macDelByHwCallBack - this call-back function pointer should be called when MAC entry deleted.<nl>
 *      init_param.routingAddByHwCallBack - this call-back function pointer should be called when Routing entry added.<nl>
 *      init_param.routingDelByHwCallBack - this call-back function pointer should be called when Routing entry deleted.<nl>
 *      init_param.naptAddByHwCallBack - this call-back function pointer should be called when NAPT entry added.<nl>
 *      init_param.naptDelByHwCallBack - this call-back function pointer should be called when NAPT entry deleted.
 *      init_param.bindingAddByHwCallBack - this call-back function pointer should be called when Binding entry added.<nl>
 *      init_param.bindingDelByHwCallBack - this call-back function pointer should be called when Binding entry added.
 */
int32 
rtk_rg_initParam_get(rtk_rg_initParams_t *init_param)
{
    rtdrv_rg_initParam_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == init_param), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.init_param, init_param, sizeof(rtk_rg_initParams_t));
    GETSOCKOPT(RTDRV_RG_INITPARAM_GET, &cfg, rtdrv_rg_initParam_get_t, 1);
    osal_memcpy(init_param, &cfg.init_param, sizeof(rtk_rg_initParams_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_initParam_get */

/* Function Name:
 *      rtk_rg_initParam_set
 * Description:
 *      Set the initialized call-back functions, and reset all Global variables.
 * Input:
 * Output:
 *      init_param  - [in]<tab>The instance of rtk_rg_initParams_t with saved call-back function pointers.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NOT_INIT - The RG module is failed to initialize
 *      RT_ERR_RG_VLAN_SET_FAIL - Set up default CPU VLAN or LAN VLAN failed
 * Note:
 *      This function should be called before any other RG APIs.
 */
int32 
rtk_rg_initParam_set(rtk_rg_initParams_t *init_param)
{
	rtdrv_rg_initParam_set_t cfg;

	/* parameter check */
	//This API allow NULL parameter
	/* function body */
	if(init_param==NULL){
		osal_memset(&cfg.init_param,0xff,sizeof(rtk_rg_initParams_t));//translate parameter NULL to 0xffff....ffff
		GETSOCKOPT(RTDRV_RG_INITPARAM_SET, &cfg, rtdrv_rg_initParam_set_t, 1);
	}else{
		osal_memcpy(&cfg.init_param, init_param, sizeof(rtk_rg_initParams_t));
		GETSOCKOPT(RTDRV_RG_INITPARAM_SET, &cfg, rtdrv_rg_initParam_set_t, 1);
		osal_memcpy(init_param, &cfg.init_param, sizeof(rtk_rg_initParams_t));
	}
  
	return RT_ERR_OK;
}	/* end of rtk_rg_initParam_set */


//LAN Interface/Static Route/IPv4 DHCP Server

/* Sub-module Name: Lan Interface */

/* Function Name:
 *      rtk_rg_lanInterface_add
 * Description:
 *      Create one new LAN interface, add related entries into HW tables and Global variables.
 * Input:
 * Output:
 *      lan_info  - [in]<tab>LAN interface configuration structure.
 *      intf_idx - [out]<tab>Return the index of new created LAN interface.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NULL_POINTER - the input parameter may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *      RT_ERR_RG_NOT_INIT - the RG module didn't init
 *      RT_ERR_RG_ENTRY_FULL - the interface number is beyond predefined limitation
 *      RT_ERR_RG_MODIFY_LAN_AT_WAN_EXIST - the LAN interface can not add after WAN interface exist
 *      RT_ERR_RG_PORT_USED - the interface has port overlap with other interface
 *      RT_ERR_RG_ARP_FULL - ARP table is not available for this new interface
 *      RT_ERR_RG_INTF_GET_FAIL
 *      RT_ERR_RG_VLAN_SET_FAIL
 *      RT_ERR_RG_INTF_SET_FAIL
 *      RT_ERR_RG_ROUTE_SET_FAIL
 * Note:
 *      lan_info.gmac - the gateway MAC address of this LAN interface.<nl>
 *      lan_info.ip_addr - the IP address of this LAN interface.<nl>
 *      lan_info.ip_network_mask - the mask decides how many hosts in this LAN.<nl>
 *      lan_info.port_mask - which ports belong to this LAN.<nl>
 *      lan_info.untag_mask - which ports in this LAN should be egress untag.<nl>
 *      lan_info.extport_mask - which extension ports belong to this LAN.<nl>
 *      lan_info.intf_vlan_id - the default VLAN ID of this LAN.<nl>
 *      lan_info.mtu - the maximum transmission unit of this LAN interface.<nl>
 *      This function should be called after rtk_rg_initParam_set and before any WAN interface creation.
 */
int32 
rtk_rg_lanInterface_add(rtk_rg_lanIntfConf_t *lan_info,int *intf_idx)
{
    rtdrv_rg_lanInterface_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == lan_info), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == intf_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.lan_info, lan_info, sizeof(rtk_rg_lanIntfConf_t));
    osal_memcpy(&cfg.intf_idx, intf_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_LANINTERFACE_ADD, &cfg, rtdrv_rg_lanInterface_add_t, 1);
    osal_memcpy(lan_info, &cfg.lan_info, sizeof(rtk_rg_lanIntfConf_t));
    osal_memcpy(intf_idx, &cfg.intf_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_lanInterface_add */

/* Function Name:
 *      rtk_rg_dhcpServerStaticAlloc_add
 * Description:
 *      Add DHCP static allocated IP address for indicated MAC address.
 * Input:
 * Output:
 *      ipaddr - [in]<tab>What IP address DHCP server should allocate with the macaddr.
 *      macaddr - [in]<tab>DHCP server use this to decide which IP address should allocate.
 *      static_idx - [out]<tab>Return the index of the IP-MAC pair.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *      RT_ERR_RG_ENTRY_FULL - the static allocated is not enough for adding a new one
 * Note:
 *      macaddr.octet - divide MAC address into 6 octet array.
 */
//extern int32 rtk_rg_dhcpServerStaticAlloc_add(ipaddr_t ipaddr, rtk_mac_t *macaddr,int *static_idx);

/* Function Name:
 *      rtk_rg_dhcpServerStaticAlloc_del
 * Description:
 *      Delete the static IP address assignment.
 * Input:
 * Output:
 *      static_idx - [in]<tab>The index of static allocated address to be deleted.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information
 * Note:
 *      None
 */
//extern int32 rtk_rg_dhcpServerStaticAlloc_del(int static_idx);

/* Function Name:
 *      rtk_rg_dhcpServerStaticAlloc_find
 * Description:
 *      Get the IP address and MAC address according to the input index.
 * Input:
 * Output:
 *      ipaddr  - [out]<tab>DHCP server should give this address with macaddr.
 *	  macaddr - [out]<tab>DHCP server use this to decide which IP address should give.
 *      idx - [in]<tab>the index of static allocated address to find.<nl>[out]<tab>return the actually index of the record.(See Note below)
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *      RT_ERR_RG_STATIC_NOT_FOUND - there is no valid record
 * Note:
 *      If the input idx record is not valid, it will auto incress to next index, until return a valid record or meet the end.
 */
//extern int32 rtk_rg_dhcpServerStaticAlloc_find(ipaddr_t *ipaddr, rtk_mac_t *macaddr, int *idx);

/* Sub-module Name: Wan Interface */

/* Function Name:
 *      rtk_rg_wanInterface_add
 * Description:
 *      Add WAN interface.
 * Input:
 * Output:
 *      wanintf - [in]<tab>The configuration structure of WAN interface.
 *      wan_intf_idx - [out]<tab>The created new WAN interface index.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *      RT_ERR_RG_NOT_INIT - the RG module didn't init
 *      RT_ERR_RG_ENTRY_FULL - the table is full
 *      RT_ERR_RG_DEF_ROUTE_EXIST - the default internet WAN is exist
 *	  RT_ERR_RG_LAN_NOT_EXIST - WAN interface should be add after LAN interface created
 *      RT_ERR_RG_PON_INVALID - for chip A1, the PON port can not be set as WAN port
 * 	  RT_ERR_RG_CHIP_NOT_SUPPORT - the function is not support for the chip
 *      RT_ERR_RG_PORT_BIND_GET_FAIL
 *      RT_ERR_RG_ROUTE_GET_FAIL
 *      RT_ERR_RG_INTF_GET_FAIL
 *      RT_ERR_RG_VLAN_GET_FAIL
 *      RT_ERR_RG_VLAN_SET_FAIL
 *	  RT_ERR_RG_PPPOE_SET_FAIL
 *      RT_ERR_RG_NXP_SET_FAIL
 *      RT_ERR_RG_WANTYPE_SET_FAIL
 *      RT_ERR_RG_PORT_BIND_SET_FAIL
 * Note:
 *      wanintf.wan_type - this WAN interface type, can be Bridge mode, DHCP mode, PPPoE mode, etc.<nl>
 *      wanintf.gmac - the gateway MAC address of this WAN interface.<nl>
 *      wanintf.wan_port_idx - indicate which port this WAN interface belong to.<nl>
 *      wanintf.port_binding_mask - indicate which ports and extension ports should be bound to this WAN interface.<nl>
 *      wanintf.egress_vlan_tag_on - indicate the egress packet should carry VLAN CTAG or not.<nl>
 *      wanintf.egress_vlan_id - indicate what VLAN ID this WAN interface should carry.<nl>
 *      wanintf.egress_vlan_pri - indicate what VLAN priority this WAN interface should carry.<nl> 
 *      When adding WAN interface except Bridge mode, this function should be called before<nl>
 *      each sub function like rtk_rg_staticInfo_set, rtk_rg_dhcpClientInfo_set, rtk_rg_pppoeClientInfoBeforeDial_set, and rtk_rg_pppoeClientInfoAfterDial_set.
 */
int32 
rtk_rg_wanInterface_add(rtk_rg_wanIntfConf_t *wanintf, int *wan_intf_idx)
{
    rtdrv_rg_wanInterface_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == wanintf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == wan_intf_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.wanintf, wanintf, sizeof(rtk_rg_wanIntfConf_t));
    osal_memcpy(&cfg.wan_intf_idx, wan_intf_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_WANINTERFACE_ADD, &cfg, rtdrv_rg_wanInterface_add_t, 1);
    osal_memcpy(wanintf, &cfg.wanintf, sizeof(rtk_rg_wanIntfConf_t));
    osal_memcpy(wan_intf_idx, &cfg.wan_intf_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_wanInterface_add */



/* Function Name:
 *      rtk_rg_dhcpRequest_set
 * Description:
 *      Set DHCP request for the indexed WAN interface.
 * Input:
 * Output:
 *      wan_intf_idx - [in]<tab>The interface index of previous setup for DHCP mode.
 * Return:
 *      RT_ERR_RG_OK
 * Note:
 *      None
 */
int32
rtk_rg_dhcpRequest_set(int wan_intf_idx)
{
    rtdrv_rg_dhcpRequest_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.wan_intf_idx, &wan_intf_idx, sizeof(int));
    SETSOCKOPT(RTDRV_RG_DHCPREQUEST_SET, &cfg, rtdrv_rg_dhcpRequest_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_dhcpRequest_set */


/* Function Name:
 *      rtk_rg_staticInfo_set
 * Description:
 *      Set static mode information to the indexed WAN interface.
 * Input:
 * Output:
 *      wan_intf_idx - [in]<tab>The interface index of previous setup for static mode.
 *      static_info - [in]<tab>The configuration related to static mode.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *	  RT_ERR_RG_ENTRY_NOT_EXIST - the wan interface was not added before
 *	  RT_ERR_RG_ARP_FULL - ARP table is not available for this new interface
 *      RT_ERR_RG_ARP_NOT_FOUND - the remote gateway is not found or time-out
 *	  RT_ERR_RG_GW_MAC_NOT_SET - gateway mac should be set for Lite RomeDriver
 *      RT_ERR_RG_ENTRY_FULL
 *      RT_ERR_RG_ROUTE_GET_FAIL
 *      RT_ERR_RG_ROUTE_SET_FAIL
 *      RT_ERR_RG_EXTIP_GET_FAIL
 *      RT_ERR_RG_EXTIP_SET_FAIL
 * Note:
 *      static_info.napt_enable - indicate this WAN interface should turn on napt or not.<nl>
 *      static_info.ip_addr - the IP address of this WAN interface.<nl>
 *      static_info.ip_network_mask - the mask decides how many hosts in this WAN.<nl>
 *      static_info.default_gateway_on - indicate this WAN interface is default internet interface or not.<nl>
 *      static_info.gateway_ip_addr - indicate which gateway this WAN interface is heading to.<nl>
 *      static_info.mtu - the maximum transmission unit of this LAN interface.<nl>
 *      static_info.gw_mac_auto_learn - switch to turn on auto-ARP request for gateway MAC.<nl>
 *      static_info.gateway_mac_addr - pointer to gateway's mac if the auto_learn switch is off.<nl>
 *      The interface index should point to a valid WAN interface with matched type.
 */
int32 
rtk_rg_staticInfo_set(int wan_intf_idx, rtk_rg_ipStaticInfo_t *static_info)
{
    rtdrv_rg_staticInfo_set_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == static_info), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.wan_intf_idx, &wan_intf_idx, sizeof(int));
    osal_memcpy(&cfg.static_info, static_info, sizeof(rtk_rg_ipStaticInfo_t));
    GETSOCKOPT(RTDRV_RG_STATICINFO_SET, &cfg, rtdrv_rg_staticInfo_set_t, 1);
    osal_memcpy(static_info, &cfg.static_info, sizeof(rtk_rg_ipStaticInfo_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_staticInfo_set */

/* Function Name:
 *      rtk_rg_dhcpClientInfo_set
 * Description:
 *      Set DHCP mode information to the indexed WAN interface.
 * Input:
 * Output:
 *      wan_intf_idx - [in]<tab>The interface index of previous setup for DHCP mode.
 *      dhcpClient_info - [in]<tab>The configuration related to the interface.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *	  RT_ERR_RG_ENTRY_NOT_EXIST - the wan interface was not added before
 *	  RT_ERR_RG_ARP_FULL - ARP table is not available for this new interface
 *      RT_ERR_RG_ARP_NOT_FOUND - the remote gateway is not found or time-out
 *	  RT_ERR_RG_GW_MAC_NOT_SET - gateway mac should be set for Lite RomeDriver
 *      RT_ERR_RG_INTF_GET_FAIL
 *      RT_ERR_RG_ROUTE_GET_FAIL
 *      RT_ERR_RG_ENTRY_FULL
 *      RT_ERR_RG_ROUTE_SET_FAIL
 *      RT_ERR_RG_EXTIP_GET_FAIL
 *      RT_ERR_RG_EXTIP_SET_FAIL
 * Note:
 *      dhcpClient_info.hw_info - the static_info sturcture of this DHCP mode WAN interface.<nl>
 *      dhcpClient_info.status - the status of DHCP mode WAN interface as client, leased or released.<nl>
 *      The client_info contains the Hardware information which contains static mode settings, and the interface index should point to a WAN interface with matched type.
 */
int32 
rtk_rg_dhcpClientInfo_set(int wan_intf_idx, rtk_rg_ipDhcpClientInfo_t *dhcpClient_info)
{
    rtdrv_rg_dhcpClientInfo_set_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == dhcpClient_info), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.wan_intf_idx, &wan_intf_idx, sizeof(int));
    osal_memcpy(&cfg.dhcpClient_info, dhcpClient_info, sizeof(rtk_rg_ipDhcpClientInfo_t));
    GETSOCKOPT(RTDRV_RG_DHCPCLIENTINFO_SET, &cfg, rtdrv_rg_dhcpClientInfo_set_t, 1);
    osal_memcpy(dhcpClient_info, &cfg.dhcpClient_info, sizeof(rtk_rg_ipDhcpClientInfo_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_dhcpClientInfo_set */

/* Function Name:
 *      rtk_rg_pppoeClientInfoBeforeDial_set
 * Description:
 *      Set PPPoE mode information to the indexed WAN interface before dial. 
 * Input:
 * Output:
 *      wan_intf_idx - [in]<tab>The interface index of previous setup for PPPoE mode.
 *      app_info - [in]<tab>The configuration related to PPPoE mode.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 * Note:
 *      app_info.username - saved user account information for PPPoE server.<nl>
 *      app_info.password - saved account password for PPPoE server.<nl>
 *      app_info.auth_type - indicate the PPPoE server use PAP or CHAP.<nl>
 *      app_info.pppoe_proxy_enable - indicate the proxy of PPPoE server enable or not.<nl>
 *      app_info.max_pppoe_proxy_num - how many PPPoE proxy at most.<nl>
 *      app_info.auto_reconnect - indicate that we reconnect automatically no matter what.<nl>
 *      app_info.dial_on_demond - indicate that we connect only on demand.<nl>
 *      app_info.idle_timeout_secs - indicate how long we should turn off connection after idle.<nl>
 *      app_info.stauts - indicate the status of connection.<nl>
 *      app_info.dialOnDemondCallBack - this function would be called when the interface has traffic flow.<nl>
 *      app_info.idleTimeOutCallBack - this function would be called when the interface didn't have traffic for indicated timeout period.<nl>
 *      The required account information is kept and this function should be called before rtk_rg_pppoeClientInfoAfterDial_set.<nl>
 *      The interface index should point to a WAN interface with matched type.
 */
int32 
rtk_rg_pppoeClientInfoBeforeDial_set(int wan_intf_idx, rtk_rg_pppoeClientInfoBeforeDial_t *app_info)
{
    rtdrv_rg_pppoeClientInfoBeforeDial_set_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == app_info), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.wan_intf_idx, &wan_intf_idx, sizeof(int));
    osal_memcpy(&cfg.app_info, app_info, sizeof(rtk_rg_pppoeClientInfoBeforeDial_t));
    GETSOCKOPT(RTDRV_RG_PPPOECLIENTINFOBEFOREDIAL_SET, &cfg, rtdrv_rg_pppoeClientInfoBeforeDial_set_t, 1);
    osal_memcpy(app_info, &cfg.app_info, sizeof(rtk_rg_pppoeClientInfoBeforeDial_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_pppoeClientInfoBeforeDial_set */

/* Function Name:
 *      rtk_rg_pppoeClientInfoAfterDial_set
 * Description:
 *      Set PPPoE mode information to the indexed WAN interface after dial. 
 * Input:
 * Output:
 *      wan_intf_idx - [in]<tab>The interface index of previous setup for PPPoE mode.
 *      clientPppoe_info - [in]<tab>The configuration related to PPPoE mode.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *	  RT_ERR_RG_ENTRY_NOT_EXIST - the wan interface was not added before
 *	  RT_ERR_RG_ARP_FULL - ARP table is not available for this new interface
 *      RT_ERR_RG_ARP_NOT_FOUND - the remote gateway is not found or time-out
 *	  RT_ERR_RG_GW_MAC_NOT_SET - gateway mac should be set for Lite RomeDriver
 *      RT_ERR_RG_ROUTE_GET_FAIL
 *      RT_ERR_RG_ENTRY_FULL
 *      RT_ERR_RG_ROUTE_SET_FAIL
 *      RT_ERR_RG_EXTIP_GET_FAIL
 *      RT_ERR_RG_EXTIP_SET_FAIL
 * Note:
 *      clientPppoe_info.hw_info - the static_info sturcture of this PPPoE mode WAN interface.<nl>
 *      clientPppoe_info.sessionId - stored PPPoE session ID currently used.<nl>
 *      The client_info contains the Hardware information which contains static mode settings, and the interface index should point to a WAN interface with matched type.
 */
int32 
rtk_rg_pppoeClientInfoAfterDial_set(int wan_intf_idx, rtk_rg_pppoeClientInfoAfterDial_t *clientPppoe_info)
{
    rtdrv_rg_pppoeClientInfoAfterDial_set_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == clientPppoe_info), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.wan_intf_idx, &wan_intf_idx, sizeof(int));
    osal_memcpy(&cfg.clientPppoe_info, clientPppoe_info, sizeof(rtk_rg_pppoeClientInfoAfterDial_t));
    GETSOCKOPT(RTDRV_RG_PPPOECLIENTINFOAFTERDIAL_SET, &cfg, rtdrv_rg_pppoeClientInfoAfterDial_set_t, 1);
    osal_memcpy(clientPppoe_info, &cfg.clientPppoe_info, sizeof(rtk_rg_pppoeClientInfoAfterDial_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_pppoeClientInfoAfterDial_set */


/* Sub-module Name: Interface Utility */

/* Function Name:
 *      rtk_rg_interface_del
 * Description:
 *      Delete the indicated interface, may be LAN or WAN interface.
 * Input:
 * Output:
 *      lan_or_wan_intf_idx - [in]<tab>The index of interface.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *	  RT_ERR_RG_MODIFY_LAN_AT_WAN_EXIST - LAN interface should not be deleted when WAN interface existed
 *      RT_ERR_RG_INTF_GET_FAIL
 *      RT_ERR_RG_ROUTE_GET_FAIL
 *      RT_ERR_RG_ROUTE_SET_FAIL
 *      RT_ERR_RG_NXP_GET_FAIL
 *      RT_ERR_RG_PPPOE_SET_FAIL
 *      RT_ERR_RG_NXP_SET_FAIL
 *      RT_ERR_RG_ROUTE_SET_FAIL
 *      RT_ERR_RG_EXTIP_GET_FAIL
 *      RT_ERR_RG_EXTIP_SET_FAIL
 *      RT_ERR_RG_WANTYPE_GET_FAIL
 *      RT_ERR_RG_WANTYPE_SET_FAIL
 *      RT_ERR_RG_INTF_SET_FAIL
 *      RT_ERR_RG_PORT_BIND_GET_FAIL
 *      RT_ERR_RG_PORT_BIND_SET_FAIL
 * Note:
 *      Before deleting any interface, it should be created first.
 */
int32 
rtk_rg_interface_del(int lan_or_wan_intf_idx)
{
    rtdrv_rg_interface_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.lan_or_wan_intf_idx, &lan_or_wan_intf_idx, sizeof(int));
    SETSOCKOPT(RTDRV_RG_INTERFACE_DEL, &cfg, rtdrv_rg_interface_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_interface_del */

/* Function Name:
 *      rtk_rg_intfInfo_find
 * Description:
 *      Return the information structure of interface, eithor LAN or WAN.
 * Input:
 * Output:
 *      intf_info - [in]<tab>An empty buffer for storing the structure.<nl>[out]<tab>Returned valid interface information structure, eithor LAN or WAN.
 *      valid_lan_or_wan_intf_idx - [in]<tab>The index of interface to find.<nl>[out]<tab>Return the index of the valid record.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *      RT_ERR_RG_ENTRY_NOT_EXIST
 * Note:
 *      intf_info.intf_name - the name of found interface.<nl>
 *      intf_info.is_wan - indicate this interface is WAN interface or LAN interface.<nl>
 *      intf_info.lan_intf - if is_wan is 0, this structure contain all information about the LAN interface.<nl>
 *      intf_info.wan_intf - if is_wan is 1, this structure contain all information about the WAN interface.<nl>
 *      intf_info.ingress_packet_count - the count of how many packet had passed into this interface.<nl>
 *      intf_info.ingress_byte_count - the count of how many data had passed into this interface in bytes.<nl>
 *      intf_info.egress_packet_count - the count of how many packet had passed through this interface out.<nl>
 *      intf_info.egress_byte_count - the count of how many data had passed through this interface out in bytes.<nl>
 *      If the input idx interface is not exist, it will auto incress to next index, until return a valid one or meet end.<nl>
 *      Besides, if the input idx is -1, intf_info.lan_intf.ip_addr and intf_info.lan_intf.ip_network_mask is given, <nl>
 *      the matching interface information and it's index will be return. Bridge WAN interface could not be found <nl>
 *      through this mode.
 */
int32 
rtk_rg_intfInfo_find(rtk_rg_intfInfo_t *intf_info, int *valid_lan_or_wan_intf_idx)
{
    rtdrv_rg_intfInfo_find_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == intf_info), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == valid_lan_or_wan_intf_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.intf_info, intf_info, sizeof(rtk_rg_intfInfo_t));
    osal_memcpy(&cfg.valid_lan_or_wan_intf_idx, valid_lan_or_wan_intf_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_INTFINFO_FIND, &cfg, rtdrv_rg_intfInfo_find_t, 1);
    osal_memcpy(intf_info, &cfg.intf_info, sizeof(rtk_rg_intfInfo_t));
    osal_memcpy(valid_lan_or_wan_intf_idx, &cfg.valid_lan_or_wan_intf_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_intfInfo_find */


/* Sub-module Name: VLAN Binding */

/* Function Name:
 *      rtk_rg_vlanBinding_add
 * Description:
 *      Add Port-VLAN binding rule.
 * Input:
 * Output:
 *      vlan_binding_info - [in]<tab>The VLAN binding configuration about port index, interface, and VLAN ID
 *      vlan_binding_idx - [out]<tab>The index of added VLAN binding rule.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *      RT_ERR_RG_NOT_INIT - the RG module is not init
 *      RT_ERR_RG_ENTRY_FULL
 * Note:
 *      vlan_binding_idx.port_idx - each VLAN binding rule can only assign one single port or extension port (not CPU port).<nl>
 *      vlan_binding_idx.ingress_vid - the VLAN ID used to compare for binding.<nl>
 *      vlan_binding_idx.wan_intf_idx - which WAN interface the matched packet should go.<nl>
 *      The VLAN-binding rule can add only after the binding WAN interface is created at first.
 */
int32 
rtk_rg_vlanBinding_add(rtk_rg_vlanBinding_t *vlan_binding_info, int *vlan_binding_idx)
{
    rtdrv_rg_vlanBinding_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == vlan_binding_info), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == vlan_binding_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.vlan_binding_info, vlan_binding_info, sizeof(rtk_rg_vlanBinding_t));
    osal_memcpy(&cfg.vlan_binding_idx, vlan_binding_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_VLANBINDING_ADD, &cfg, rtdrv_rg_vlanBinding_add_t, 1);
    osal_memcpy(vlan_binding_info, &cfg.vlan_binding_info, sizeof(rtk_rg_vlanBinding_t));
    osal_memcpy(vlan_binding_idx, &cfg.vlan_binding_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_vlanBinding_add */

/* Function Name:
 *      rtk_rg_vlanBinding_del
 * Description:
 *      Delete Port-VLAN binding rule.
 * Input:
 * Output:
 *      vlan_binding_idx - [in]<tab>The index of VLAN binding rule.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *      RT_ERR_RG_VLAN_BIND_UNINIT - there is no vlan-binding rule added before
 * Note:
 *      None
 */
int32 
rtk_rg_vlanBinding_del(int vlan_binding_idx)
{
    rtdrv_rg_vlanBinding_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.vlan_binding_idx, &vlan_binding_idx, sizeof(int));
    SETSOCKOPT(RTDRV_RG_VLANBINDING_DEL, &cfg, rtdrv_rg_vlanBinding_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_vlanBinding_del */

/* Function Name:
 *      rtk_rg_vlanBinding_find
 * Description:
 *      Find Port-VLAN binding rule if any.
 * Input:
 * Output:
 *      vlan_binding_info - [in]<tab>The binding configuration of port and VLAN ID.
 *      valid_idx - [in]<tab>The index of the VLAN binding entry.<nl>[out]<tab>The index of first valid VLAN binding entry.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *      RT_ERR_RG_VLAN_BIND_UNINIT - there is no vlan-binding rule added before
 * Note:
 *      If the input parameter do not point to a valid record, it will continue to look for next valid one until end, and return the valid index by the passed input pointer.
 */
int32 
rtk_rg_vlanBinding_find(rtk_rg_vlanBinding_t *vlan_binding_info, int *valid_idx)
{
    rtdrv_rg_vlanBinding_find_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == vlan_binding_info), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == valid_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.vlan_binding_info, vlan_binding_info, sizeof(rtk_rg_vlanBinding_t));
    osal_memcpy(&cfg.valid_idx, valid_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_VLANBINDING_FIND, &cfg, rtdrv_rg_vlanBinding_find_t, 1);
    osal_memcpy(vlan_binding_info, &cfg.vlan_binding_info, sizeof(rtk_rg_vlanBinding_t));
    osal_memcpy(valid_idx, &cfg.valid_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_vlanBinding_find */


/* Sub-module Name: ALG configuration */

int rtk_rg_algApps_set(rtk_rg_alg_type_t alg_app);
int rtk_rg_algApps_get(rtk_rg_alg_type_t* alg_app);


/* Sub-module Name: DMZ configuration */

int rtk_rg_dmzHost_set(int wan_intf_idx, rtk_rg_dmzInfo_t *dmz_info);
int rtk_rg_dmzHost_get(int wan_intf_idx, rtk_rg_dmzInfo_t *dmz_info);


/* Sub-module Name: VirtualServer configuration */

/* Function Name:
 *      rtk_rg_virtualServer_add
 * Description:
 *      Add virtual server connection rule.
 * Input:
 * Output:
 *      *virtual_server - [in]<tab>the svirtual server data structure.
 *      *virtual_server_idx - [in]<tab>the index of virtual server table.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_INITPM_UNINIT - the virtual server table uninitialized.
 *		RT_ERR_RG_ENTRY_FULL - the virtual server table is full.
 * Note:
 *     	virtual_server.is_tcp - Layer 4 protocol is TCP or not.<nl>	
 *		virtual_server.wan_intf_idx - Wan interface index of server.<nl>	
 *		virtual_server.gateway_port_start - Gateway external port mapping start number.<nl>	
 *		virtual_server.local_ip - The translating local IP address.<nl>	
 *		virtual_server.local_port_start - The translating internal port mapping start number.<nl>	
 *		virtual_server.mappingRangeCnt - The port mapping range count.<nl>
 *		virtual_server.valid - This entry is valid or not.<nl>
 */
int32 
rtk_rg_virtualServer_add(rtk_rg_virtualServer_t *virtual_server, int *virtual_server_idx)
{
    rtdrv_rg_virtualServer_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == virtual_server), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == virtual_server_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.virtual_server, virtual_server, sizeof(rtk_rg_virtualServer_t));
    osal_memcpy(&cfg.virtual_server_idx, virtual_server_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_VIRTUALSERVER_ADD, &cfg, rtdrv_rg_virtualServer_add_t, 1);
    osal_memcpy(virtual_server, &cfg.virtual_server, sizeof(rtk_rg_virtualServer_t));
    osal_memcpy(virtual_server_idx, &cfg.virtual_server_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_virtualServer_add */

/* Function Name:
 *      rtk_rg_virtualServer_del
 * Description:
 *      Delete one server port connection rule.
 * Input:
 * Output:
 *      virtual_server_idx - [in]<tab>the index of virtual server entry for deleting.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_INVALID_PARAM - illegal server port rule index.
 * Note:
 *      None.
 */
int32 
rtk_rg_virtualServer_del(int virtual_server_idx)
{
    rtdrv_rg_virtualServer_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.virtual_server_idx, &virtual_server_idx, sizeof(int));
    SETSOCKOPT(RTDRV_RG_VIRTUALSERVER_DEL, &cfg, rtdrv_rg_virtualServer_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_virtualServer_del */

/* Function Name:
 *      rtk_rg_virtualServer_find
 * Description:
 *      Find entire virtual server table from valid_idx till valid one.
 * Input:
 * Output:
 *      *virtual_server - [in]<tab>An empty buffer for storing the virtual server entry data structure.<nl>[out]<tab>The data structure of found virtual server entry.
 *      *valid_idx - [in]<tab>The index which find from.<nl>[out]<tab>The existing entry index.
 * Return:
 *      RT_ERR_RG_OK
 * 	 	RT_ERR_RG_INITPM_UNINIT - the virtual server table uninitialized.
 * 	 	RT_ERR_RG_SVRPORT_SW_ENTRY_NOT_FOUND - can't find entry in virtual server table.
 * Note:
 *     	virtual_server.is_tcp - Layer 4 protocol is TCP or not.<nl>	
 *		virtual_server.wan_intf_idx - Wan interface index of server.<nl>	
 *		virtual_server.gateway_port_start - Gateway external port mapping start number.<nl>	
 *		virtual_server.local_ip - The translating local IP address.<nl>	
 *		virtual_server.local_port_start - The translating internal port mapping start number.<nl>	
 *		virtual_server.mappingRangeCnt - The port mapping range count.<nl>
 *		virtual_server.valid - This entry is valid or not.<nl>
 */
int32 
rtk_rg_virtualServer_find(rtk_rg_virtualServer_t *virtual_server, int *valid_idx)
{
    rtdrv_rg_virtualServer_find_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == virtual_server), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == valid_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.virtual_server, virtual_server, sizeof(rtk_rg_virtualServer_t));
    osal_memcpy(&cfg.valid_idx, valid_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_VIRTUALSERVER_FIND, &cfg, rtdrv_rg_virtualServer_find_t, 1);
    osal_memcpy(virtual_server, &cfg.virtual_server, sizeof(rtk_rg_virtualServer_t));
    osal_memcpy(valid_idx, &cfg.valid_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_virtualServer_find */



/* Sub-module Name: ACL Filter */


/* Function Name:
 *      rtk_rg_aclFilterAndQos_add
 * Description:
 *      Add acl rule.
 * Input:
 * Output:
 *	acl_filter - [in]<tab>assign the patterns which need to be filtered, and assign the related action(include drop, permit, Qos, and Trap to CPU).
 *      acl_filter_idx  - [out]<tab>the index of the added acl rule.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NOT_INIT - the rg has not been init. (rtk_rg_initParam_set() should be called first)  
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *      RT_ERR_RG_ACL_CF_FIELD_CONFLICT - acl_filter assigned some conflict patterns
 *      RT_ERR_RG_ACL_CF_FLOW_DIRECTION_ERROR - the assigned ingress interface and egress interface are not upstream or downstream
 *      RT_ERR_RG_ACL_ENTRY_FULL - the hardware ACL asic entry is full
 *      RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED - access the hardware ACL asic entry failed
 *      RT_ERR_RG_ACL_IPTABLE_FULL - the hardware asic ACL IP_RANGE_TABLE entry is full
 *      RT_ERR_RG_ACL_IPTABLE_ACCESS_FAILED - access the hardware asic ACL IP_RANGE_TABLE entry failed
 *      RT_ERR_RG_ACL_PORTTABLE_FULL - the hardware asic ACL PORT_RANGE_TABLE entry is full
 *      RT_ERR_RG_ACL_PORTTABLE_ACCESS_FAILED - access the hardware ACL asic PORT_RANGE_TABLE entry failed
 *      RT_ERR_RG_CF_ENTRY_FULL - the hardware Classification asic entry is full
 *      RT_ERR_RG_CF_ENTRY_ACCESS_FAILED - access the hardware Classification asic entry failed
 *      RT_ERR_RG_CF_IPTABLE_FULL - the hardware Classification asic IP_RANGE_TABLE entry is full
 *      RT_ERR_RG_CF_IPTABLE_ACCESS_FAILED - access the hardware Classification asic IP_RANGE_TABLE entry failed
 *      RT_ERR_RG_CF_PORTTABLE_FULL - the hardware Classification PORT_RANGE_TABLE asic entry is full
 *      RT_ERR_RG_CF_PORTTABLE_ACCESS_FAILED - access the hardware Classification asic PORT_RANGE_TABLE entry failed
 *      RT_ERR_RG_CF_DSCPTABLE_FULL - the hardware Classification DSCP_REMARKING_TABLE asic entry is full
 *      RT_ERR_RG_CF_DSCPTABLE_ACCESS_FAILED - access the hardware Classification asic DSCP_REMARKING_TABLE entry failed
 *      RT_ERR_RG_ACL_SW_ENTRY_FULL - the software ACL entry is full
 *      RT_ERR_RG_ACL_SW_ENTRY_ACCESS_FAILED - access the software ACL entry failed
 * Note:
 *      acl_filter.filter_fields - use to enable the filtered patterns. Each pattern bit should be "or" together.<nl>
 *      acl_filter.ingress_port_mask - assign the packet ingress physical port pattern(valid when INGRESS_PORT_BIT is enable)<nl>
 *      acl_filter.ingress_dscp - assign the packet ingress dscp(valid when INGRESS_DSCP_BIT is enable)<nl>
 *      acl_filter.ingress_intf_idx - assign the packet ingress interface index(valid when INGRESS_INTF_BIT is enable)<nl>
 *      acl_filter.egress_intf_idx - assign the packet egress interface index(valid when EGRESS_INTF_BIT is enable)<nl>
 *      acl_filter.ingress_ethertype - assign the packet ingress ethertype pattern(valid when INGRESS_ETHERTYPE_BIT is enable)<nl>
 *      acl_filter.ingress_ctag_vid - assign the packet ingress vlan id pattern(valid when INGRESS_CTAG_VID_BIT is enable)<nl>
 *      acl_filter.ingress_ctag_pri - assign the packet ingress vlan priority pattern(valid when INGRESS_CTAG_PRI_BIT is enable)<nl>
 *      acl_filter.ingress_smac - assign the packet ingress source mac pattern(valid when INGRESS_SMAC_BIT is enable)<nl>
 *      acl_filter.ingress_dmac - assign the packet ingress destination mac pattern(valid when INGRESS_DMAC_BIT is enable)<nl>
 *      acl_filter.ingress_src_ipv4_addr_start - assign the packet ingress source ipv4 lower bound(valid when INGRESS_IPV4_SIP_RANGE_BIT is enable)<nl>
 *      acl_filter.ingress_src_ipv4_addr_end - assign the packet ingress source ipv4 upper bound(valid when INGRESS_IPV4_SIP_RANGE_BIT is enable)<nl>
 *      acl_filter.ingress_dest_ipv4_addr_start - assign the packet ingress destination ipv4 lower bound(valid when INGRESS_IPV4_DIP_RANGE_BIT is enable)<nl>
 *      acl_filter.ingress_dest_ipv4_addr_end - assign the packet ingress destination ipv4 upper bound(valid when INGRESS_IPV4_DIP_RANGE_BIT is enable)<nl>
 *      acl_filter.ingress_src_ipv6_addr_start - assign the packet ingress source ipv6 lower bound(valid when INGRESS_IPV6_SIP_RANGE_BIT is enable)<nl>
 *      acl_filter.ingress_src_ipv6_addr_end - assign the packet ingress source ipv6 upper bound(valid when INGRESS_IPV6_SIP_RANGE_BIT is enable)<nl>
 *      acl_filter.ingress_dest_ipv6_addr_start - assign the packet ingress destination ipv6 lower bound(valid when INGRESS_IPV6_DIP_RANGE_BIT is enable)<nl>
 *      acl_filter.ingress_dest_ipv6_addr_end - assign the packet ingress destination ipv6 upper bound(valid when INGRESS_IPV6_DIP_RANGE_BIT is enable)<nl>
 *      acl_filter.ingress_src_l4_port_start - assign the packet ingress layer4 source port lower bound(valid when INGRESS_L4_SPORT_RANGE_BIT is enable)<nl>
 *      acl_filter.ingress_src_l4_port_end - assign the packet ingress layer4 source port upper bound(valid when INGRESS_L4_SPORT_RANGE_BIT is enable)<nl>
 *      acl_filter.ingress_dest_l4_port_start - assign the packet ingress layer4 destination port lower bound(valid when INGRESS_L4_DPORT_RANGE_BIT is enable)<nl>
 *      acl_filter.ingress_dest_l4_port_end - assign the packet ingress layer4 destination port upper bound(valid when INGRESS_L4_DPORT_RANGE_BIT is enable)<nl>
 *      acl_filter.egress_src_ipv4_addr_start - assign the packet egress source ipv4 lower bound(valid when EGRESS_IPV4_SIP_RANGE_BIT is enable)<nl>
 *      acl_filter.egress_src_ipv4_addr_end - assign the packet egress source ipv4 upper bound(valid when EGRESS_IPV4_SIP_RANGE_BIT is enable)<nl>
 *      acl_filter.egress_dest_ipv4_addr_start - assign the packet egress destination ipv4 lower bound(valid when EGRESS_IPV4_DIP_RANGE_BIT is enable)<nl>
 *      acl_filter.egress_dest_ipv4_addr_end - assign the packet egress destination ipv4 pattern(upper bound)(valid when EGRESS_IPV4_DIP_RANGE_BIT is enable)<nl>
 *      acl_filter.egress_src_l4_port_start - assign the packet egress layer4 source port lower bound(valid when EGRESS_L4_SPORT_RANGE_BIT is enable)<nl>
 *      acl_filter.egress_src_l4_port_end - assign the packet egress layer4 source port upper bound(valid when EGRESS_L4_SPORT_RANGE_BIT is enable)<nl>
 *      acl_filter.egress_dest_l4_port_start - assign the packet egress layer4 destination port lower bound(valid when EGRESS_L4_DPORT_RANGE_BIT is enable)<nl>
 *      acl_filter.egress_dest_l4_port_end - assign the packet egress layer4 destination port upper bound(valid when EGRESS_L4_DPORT_RANGE_BIT is enable)<nl>
 *      acl_filter.action_type - assign the action to the packets which satisfy the assgned patterns<nl>
 *      acl_filter.qos_actions - assign the qos action. Each action bit should be "or" together (triggered while action_type==ACL_ACTION_TYPE_QOS)<nl>
 *      acl_filter.action_dot1p_remarking_pri - assign the vlan priority value for remarking(vlaid when while ACL_ACTION_1P_REMARKING_BIT is enable)<nl>
 *      acl_filter.action_ip_precedence_remarking_pri - assign the ip precedence value for remarking(vlaid when  ACL_ACTION_IP_PRECEDENCE_REMARKING_BIT is enable)<nl>
 *      acl_filter.action_dscp_remarking_pri - assign the dscp value for remarking(vlaid when  ACL_ACTION_DSCP_REMARKING_BIT is enable)<nl>
 *      acl_filter.action_queue_id - assign the qid(vlaid when  ACL_ACTION_QUEUE_ID_BIT is enable)<nl>
 *      acl_filter.action_share_meter - assign the sharemeter(vlaid when  ACL_ACTION_SHARE_METER_BIT is enable)<nl>
 */
int32
rtk_rg_aclFilterAndQos_add(rtk_rg_aclFilterAndQos_t *acl_filter, int *acl_filter_idx)
{
    rtdrv_rg_aclFilterAndQos_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == acl_filter), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == acl_filter_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.acl_filter, acl_filter, sizeof(rtk_rg_aclFilterAndQos_t));
    osal_memcpy(&cfg.acl_filter_idx, acl_filter_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_ACLFILTERANDQOS_ADD, &cfg, rtdrv_rg_aclFilterAndQos_add_t, 1);
    osal_memcpy(acl_filter, &cfg.acl_filter, sizeof(rtk_rg_aclFilterAndQos_t));
    osal_memcpy(acl_filter_idx, &cfg.acl_filter_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_aclFilterAndQos_add */

/* Function Name:
 *      rtk_rg_aclFilterAndQos_del
 * Description:
 *      Delete acl rule.
 * Input:
 * Output:
 *	    acl_filter_idx - [in]<tab>the index of the acl rule which need to be delete
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NOT_INIT - the rg has not been init. (rtk_rg_initParam_set() should be called first)
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *      RT_ERR_RG_ACL_ENTRY_ACCESS_FAILED - access the hardware ACL asic entry failed
 *      RT_ERR_RG_ACL_IPTABLE_ACCESS_FAILED - access the hardware asic ACL IP_RANGE_TABLE entry failed
 *      RT_ERR_RG_ACL_PORTTABLE_ACCESS_FAILED - access the hardware ACL asic PORT_RANGE_TABLE entry failed
 *      RT_ERR_RG_CF_ENTRY_ACCESS_FAILED - access the hardware Classification asic entry failed
 *      RT_ERR_RG_CF_IPTABLE_ACCESS_FAILED - access the hardware Classification asic IP_RANGE_TABLE entry failed
 *      RT_ERR_RG_CF_PORTTABLE_ACCESS_FAILED - access the hardware Classification asic PORT_RANGE_TABLE entry failed
 *      RT_ERR_RG_CF_DSCPTABLE_ACCESS_FAILED - access the hardware Classification asic DSCP_REMARKING_TABLE entry failed
 *      RT_ERR_RG_ACL_SW_ENTRY_ACCESS_FAILED - access the software ACL entry failed
 * Note:
 *      None
 */
int32
rtk_rg_aclFilterAndQos_del(int acl_filter_idx)
{
    rtdrv_rg_aclFilterAndQos_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.acl_filter_idx, &acl_filter_idx, sizeof(int));
    SETSOCKOPT(RTDRV_RG_ACLFILTERANDQOS_DEL, &cfg, rtdrv_rg_aclFilterAndQos_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_aclFilterAndQos_del */

/* Function Name:
 *      rtk_rg_aclFilterAndQos_find
 * Description:
 *      fine acl rule.
 * Input:
 *	  valid_idx - the index of the acl rule which start to search
 * Output:
 *      acl_filter - [out]<tab>the acl rule which be found
 *      valid_idx - [in]<tab>the index of the acl rule which start to search.<nl>[out]<tab>the index of found acl rule
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NOT_INIT - the rg has not been init. (rtk_rg_initParam_set() should be called first)
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 *      RT_ERR_RG_ACL_SW_ENTRY_ACCESS_FAILED - access the software ACL entry failed
 *      RT_ERR_RG_ACL_SW_ENTRY_NOT_FOUND - can not find the assigned ACL entry
 * Note:
 *      this API search the software acl entry start from acl_filter_idx, and find the first exist acl entry. If all entry after the acl_filter_idx are empty, it will return RT_ERR_RG_ACL_SW_ENTRY_NOT_FOUND
 */
int32
rtk_rg_aclFilterAndQos_find(rtk_rg_aclFilterAndQos_t *acl_filter, int *valid_idx)
{
    rtdrv_rg_aclFilterAndQos_find_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == acl_filter), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == valid_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.acl_filter, acl_filter, sizeof(rtk_rg_aclFilterAndQos_t));
    osal_memcpy(&cfg.valid_idx, valid_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_ACLFILTERANDQOS_FIND, &cfg, rtdrv_rg_aclFilterAndQos_find_t, 1);
    osal_memcpy(acl_filter, &cfg.acl_filter, sizeof(rtk_rg_aclFilterAndQos_t));
    osal_memcpy(valid_idx, &cfg.valid_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_aclFilterAndQos_find */

//MAC Filter

/* Sub-module Name: Mac Filter */


/* Function Name:
 *      rtk_rg_macFilter_add
 * Description:
 *      add mac filter rule
 * Input:
 * Output:
 *        macFilterEntry.mac - [in]<tab>the mac address which need to be add to mac filter rule.
 *        macFilterEntry.direct - [in]<tab>the mac address which shouuld be filter in SMAC, DMAC, or BOTH.
 *      mac_filter_idx - [out]<tab>the index of added mac filter rule.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NOT_INIT - the rg has not been init. (rtk_rg_initParam_set() should be called first)
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 * Note:
 *      None
 */
int32
rtk_rg_macFilter_add(rtk_rg_macFilterEntry_t *macFilterEntry,int *mac_filter_idx)
{
    rtdrv_rg_macFilter_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == macFilterEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == mac_filter_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.macFilterEntry, macFilterEntry, sizeof(rtk_rg_macFilterEntry_t));
    osal_memcpy(&cfg.mac_filter_idx, mac_filter_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_MACFILTER_ADD, &cfg, rtdrv_rg_macFilter_add_t, 1);
    osal_memcpy(macFilterEntry, &cfg.macFilterEntry, sizeof(rtk_rg_macFilterEntry_t));
    osal_memcpy(mac_filter_idx, &cfg.mac_filter_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_macFilter_add */


/* Function Name:
 *      rtk_rg_macFilter_del
 * Description:
 *      delete mac filter rule
 * Input:
 * Output:
 *          mac_filter_idx - [in]<tab>the index of the mac filter rule which need to be deleted.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NOT_INIT - the rg has not been init. (rtk_rg_initParam_set() should be called first)
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 * Note:
 *      None
 */
int32
rtk_rg_macFilter_del(int mac_filter_idx)
{
    rtdrv_rg_macFilter_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.mac_filter_idx, &mac_filter_idx, sizeof(int));
    SETSOCKOPT(RTDRV_RG_MACFILTER_DEL, &cfg, rtdrv_rg_macFilter_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_macFilter_del */


/* Function Name:
 *      rtk_rg_macFilter_find
 * Description:
 *      find mac filter rule
 * Input:
 * Output:
 *      macFilterEntry.mac - [out]<tab>the mac address which be found.
 *        macFilterEntry.direct - [out]<tab>filter for SMAC, DMAC, or BOTH.
 *      valid_idx - [in]<tab>the index of mac filter rule which start to search.<nl>[out]<tab>the index of found mac filter rule.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NOT_INIT - the rg has not been init. (rtk_rg_initParam_set() should be called first)
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 * Note:
 *      None
 */
int32
rtk_rg_macFilter_find(rtk_rg_macFilterEntry_t *macFilterEntry, int *valid_idx)
{
    rtdrv_rg_macFilter_find_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == macFilterEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == valid_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.macFilterEntry, macFilterEntry, sizeof(rtk_rg_macFilterEntry_t));
    osal_memcpy(&cfg.valid_idx, valid_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_MACFILTER_FIND, &cfg, rtdrv_rg_macFilter_find_t, 1);
    osal_memcpy(macFilterEntry, &cfg.macFilterEntry, sizeof(rtk_rg_macFilterEntry_t));
    osal_memcpy(valid_idx, &cfg.valid_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_macFilter_find */



/* Sub-module Name: URL Filter */

/* Function Name:
 *      rtk_rg_urlFilterString_add
 * Description:
 *      add url filter rule
 * Input:
 * Output:
 *	    filter - [in]<tab>the url rule which need to be added to url filter rule.
 *      url_idx - [out]<tab>the index of added url filter rule.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NOT_INIT - the rg has not been init. (rtk_rg_initParam_set() should be called first)
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 * Note:
 *      filter.url_filter_string - the string which need to be filtered in url string.(ex: in url "http://www.sample.com/explain", "www.sample.com" is the url string)<nl>
 *      filter.path_filter_string - the string which need to be filtered in url path string.(ex: in url "http://www.sample.com/explain", "/explain" is the url path string)<nl>
 *      filter.path_exactly_match - the urlFilter will execute even the path_filter_string is part of url path string while path_exactly_match is 0. Else, the path_filter_string must exactly match the url path string to trigger urlFilter execution.<nl>
 *      filter.wan_intf - the index of the wan interface which should limited by this urlFilter.
 */
int32 
rtk_rg_urlFilterString_add(rtk_rg_urlFilterString_t *filter,int *url_idx)
{
    rtdrv_rg_urlFilterString_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == filter), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == url_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.filter, filter, sizeof(rtk_rg_urlFilterString_t));
    osal_memcpy(&cfg.url_idx, url_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_URLFILTERSTRING_ADD, &cfg, rtdrv_rg_urlFilterString_add_t, 1);
    osal_memcpy(filter, &cfg.filter, sizeof(rtk_rg_urlFilterString_t));
    osal_memcpy(url_idx, &cfg.url_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_urlFilterString_add */

/* Function Name:
 *      rtk_rg_urlFilterString_del
 * Description:
 *      delete url filter rule
 * Input:
 * Output:
 *	    url_idx - [in]<tab>the index of the url filter rule which need to be deleted.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NOT_INIT - the rg has not been init. (rtk_rg_initParam_set() should be called first)
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 * Note:
 *      None
 */
int32 
rtk_rg_urlFilterString_del(int url_idx)
{
    rtdrv_rg_urlFilterString_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.url_idx, &url_idx, sizeof(int));
    SETSOCKOPT(RTDRV_RG_URLFILTERSTRING_DEL, &cfg, rtdrv_rg_urlFilterString_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_urlFilterString_del */

/* Function Name:
 *      rtk_rg_urlFilterString_find
 * Description:
 *      find url filter rule
 * Input:
 * Output:
 *      filter - [out]<tab>the url filter rule which be found.
 *      valid_idx - [in]<tab>the index fo url filter rule which start to search.<nl>[out]<tab>the index of found url filter rule.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NOT_INIT - the rg has not been init. (rtk_rg_initParam_set() should be called first)
 *      RT_ERR_RG_NULL_POINTER - the input parameters may be NULL
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information or range
 * Note:
 *      filter.url_filter_string - the string which need to be filtered in url string.(ex: in url "http://www.sample.com/explain", "www.sample.com" is the url string)<nl>
 *      filter.path_filter_string - the string which need to be filtered in url path string.(ex: in url "http://www.sample.com/explain", "/explain" is the url path string)<nl>
 *      filter.path_exactly_match - the urlFilter will execute even the path_filter_string is part of url path string while path_exactly_match is 0. Else, the path_filter_string must exactly match the url path string to trigger urlFilter execution.<nl>
 *      filter.wan_intf - the index of the wan interface which should be limited by this urlFilter.
 */
int32 
rtk_rg_urlFilterString_find(rtk_rg_urlFilterString_t *filter, int *valid_idx)
{
    rtdrv_rg_urlFilterString_find_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == filter), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == valid_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.filter, filter, sizeof(rtk_rg_urlFilterString_t));
    osal_memcpy(&cfg.valid_idx, valid_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_URLFILTERSTRING_FIND, &cfg, rtdrv_rg_urlFilterString_find_t, 1);
    osal_memcpy(filter, &cfg.filter, sizeof(rtk_rg_urlFilterString_t));
    osal_memcpy(valid_idx, &cfg.valid_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_urlFilterString_find */


/* Sub-module Name: UPnP configuration */

/* Function Name:
 *      rtk_rg_upnpConnection_add
 * Description:
 *      Add UPNP connection rule.
 * Input:
 * Output:
 *      rtk_rg_upnpConnection_t - [in]<tab>the UPNP connection data structure.
 *      upnp_idx - [in]<tab>the index of UPNP table.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_NOT_INIT - the rg has not been init. (rtk_rg_initParam_set() should be called first)
 *      RT_ERR_RG_INITPM_UNINIT - the UPNP table uninitialized.
 *		RT_ERR_RG_ENTRY_FULL - the UPNP mapping table is full.
 * Note:
 *		upnp.is_tcp - Layer 4 protocol is TCP or not.<nl>
 *		upnp.valid - The UNPN mapping is valid or not.<nl>
 *		upnp.wan_intf_idx - Wan interface index.<nl>
 *		upnp.gateway_port - Gateway external port number.<nl>
 *		upnp.local_ip - Internal ip address.<nl>
 *		upnp.local_port - Internal port number.<nl>
 *		upnp.limit_remote_ip - The Restricted remote IP address.<nl>
 *		upnp.limit_remote_port - The Restricted remote port number.<nl>
 *		upnp.remote_ip - Remote IP address.<nl
 *		upnp.emote_port - Remote port number.<nl>
 *		upnp.type - One shot or persist.<nl>
 *		upnp.timeout - timeout value for auto-delete. Set 0 to disable auto-delete.
 */
int32 
rtk_rg_upnpConnection_add(rtk_rg_upnpConnection_t *upnp, int *upnp_idx)
{
    rtdrv_rg_upnpConnection_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == upnp), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == upnp_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.upnp, upnp, sizeof(rtk_rg_upnpConnection_t));
    osal_memcpy(&cfg.upnp_idx, upnp_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_UPNPCONNECTION_ADD, &cfg, rtdrv_rg_upnpConnection_add_t, 1);
    osal_memcpy(upnp, &cfg.upnp, sizeof(rtk_rg_upnpConnection_t));
    osal_memcpy(upnp_idx, &cfg.upnp_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_upnpConnection_add */

/* Function Name:
 *      rtk_rg_upnpConnection_del
 * Description:
 *      Delete UPNP connection rule.
 * Input:
 * Output:
 *      upnp_idx - [in]<tab>the index of UPNP table entry to be deleted.
 *      None.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_INVALID_PARAM - illegal UPNP rule index.
 * Note:
 *      None.
 */
int32 
rtk_rg_upnpConnection_del(int upnp_idx)
{
    rtdrv_rg_upnpConnection_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.upnp_idx, &upnp_idx, sizeof(int));
    SETSOCKOPT(RTDRV_RG_UPNPCONNECTION_DEL, &cfg, rtdrv_rg_upnpConnection_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_upnpConnection_del */

/* Function Name:
 *      rtk_rg_upnpConnection_find
 * Description:
 *      Find entire UPNP mapping table from upnp_idx till valid one.
 * Input:
 * Output:
 *      *upnp - [in]<tab>An empty buffer for storing the UPNP mapping entry data structure.<nl>[out]<tab>The data structure of found UPNP mapping entry.
 *      *valid_idx - [in]<tab>The index which find from.<nl>[out]<tab>The existing entry index.
 * Return:
 *      RT_ERR_RG_OK
 * 	 	RT_ERR_RG_INITPM_UNINIT - the UPNP table uninitialized.
 *      RT_ERR_RG_INVALID_PARAM - illegal UPNP rule index.
 * 	 	RT_ERR_RG_UPNP_SW_ENTRY_NOT_FOUND - can't find entry in UPNP mapping table.
 * Note:
 *		upnp.is_tcp - Layer 4 protocol is TCP or not.<nl>
 *		upnp.valid - The UNPN mapping is valid or not.<nl>
 *		upnp.wan_intf_idx - Wan interface index.<nl>
 *		upnp.gateway_port - Gateway external port number.<nl>
 *		upnp.local_ip - Internal ip address.<nl>
 *		upnp.local_port - Internal port number.<nl>
 *		upnp.limit_remote_ip - The Restricted remote IP address.<nl>
 *		upnp.limit_remote_port - The Restricted remote port number.<nl>
 *		upnp.remote_ip - Remote IP address.<nl
 *		upnp.emote_port - Remote port number.<nl>
 *		upnp.type - One shot or persist.<nl>
 *		upnp.timeout - timeout value for auto-delete. Set 0 to disable auto-delete.
 *		The condition fields will be ignore while it was set to zero.
 */
int32 
rtk_rg_upnpConnection_find(rtk_rg_upnpConnection_t *upnp, int *valid_idx)
{
    rtdrv_rg_upnpConnection_find_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == upnp), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == valid_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.upnp, upnp, sizeof(rtk_rg_upnpConnection_t));
    osal_memcpy(&cfg.valid_idx, valid_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_UPNPCONNECTION_FIND, &cfg, rtdrv_rg_upnpConnection_find_t, 1);
    osal_memcpy(upnp, &cfg.upnp, sizeof(rtk_rg_upnpConnection_t));
    osal_memcpy(valid_idx, &cfg.valid_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_upnpConnection_find */


// The following function is used for Lite-RomeDriver architecture only.

/* Sub-module Name: NAPT configuration */

/* Function Name:
 *      rtk_rg_naptConnection_add
 * Description:
 *      Add NAPT connection flow.
 * Input:
 * Output:
 *      *naptFlow - [in]<tab>the NAPT connection flow data structure.
 *      *flow_idx - [out]<tab>the NAPT flow index.
 * Return:
 *      RT_ERR_RG_OK
 *		RT_ERR_RG_NOT_INIT - system is not initiated. 
 *      RT_ERR_RG_INVALID_PARAM - illegal NAPT connection flow index.
 *		RT_ERR_RG_EXTIP_GET_FAIL - illegal wan_intf_idx.
 *		RT_ERR_RG_NAPT_FLOW_DUPLICATE - duplicate NAPT flow.
 *		RT_ERR_RG_NAPTR_OVERFLOW - NAPTR table collision.
 *		RT_ERR_RG_NAPT_OVERFLOW	- NAPT table collision.
 *		RT_ERR_RG_NAPTR_SET_FAIL -  write to NAPTR table failed.
 *		RT_ERR_RG_NAPT_SET_FAIL - write to NAPT table failed.
 * Note:
 *		naptFlow.is_tcp - Layer 4 protocol is TCP or not.<nl>
 *		naptFlow.local_ip - Internal IP address.<nl>
 *		naptFlow.remote_ip - Remote IP address.<nl>
 *		naptFlow.wan_intf_idx - Wan interface index.<nl>
 *		naptFlow.local_port - Internal port number.<nl>
 *		naptFlow.remote_port - Remote port number.<nl>
 *		naptFlow.external_port - Gateway external port number.<nl>
 *		naptFlow.pri_valid - NAPT priority remarking enable.<nl>
 *		naptFlow.priority - NAPT priority remarking value.<nl>
 */
int32 
rtk_rg_naptConnection_add(rtk_rg_naptEntry_t *naptFlow, int *flow_idx)
{
    rtdrv_rg_naptConnection_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == naptFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == flow_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.naptFlow, naptFlow, sizeof(rtk_rg_naptEntry_t));
    osal_memcpy(&cfg.flow_idx, flow_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_NAPTCONNECTION_ADD, &cfg, rtdrv_rg_naptConnection_add_t, 1);
    osal_memcpy(naptFlow, &cfg.naptFlow, sizeof(rtk_rg_naptEntry_t));
    osal_memcpy(flow_idx, &cfg.flow_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_naptConnection_add */

/* Function Name:
 *      rtk_rg_naptConnection_del
 * Description:
 *      Delete NAPT connection flow.
 * Input:
 * Output:
 *      flow_idx - [in]<tab>The index of NAPT connection flow table.
 *      None.
 * Return:
 *      RT_ERR_RG_OK
 *		RT_ERR_RG_NOT_INIT - system is not initiated. 
 *      RT_ERR_RG_INVALID_PARAM - illegal NAPT connection flow index.
 *		RT_ERR_RG_NAPT_SW_ENTRY_NOT_FOUND - the NAPT entry can not be found.
 *		RT_ERR_RG_NAPTR_SET_FAIL -  write to NAPTR table failed.
 *		RT_ERR_RG_NAPT_SET_FAIL - write to NAPT table failed.
 * Note:
 *		None.
 */
int32 
rtk_rg_naptConnection_del(int flow_idx)
{
    rtdrv_rg_naptConnection_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.flow_idx, &flow_idx, sizeof(int));
    SETSOCKOPT(RTDRV_RG_NAPTCONNECTION_DEL, &cfg, rtdrv_rg_naptConnection_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_naptConnection_del */

/* Function Name:
 *      rtk_rg_naptConnection_find
 * Description:
 *      Find entire NAPT table from index valid_idx till valid one.
 * Input:
 * Output:
 *      *naptInfo - [in]<tab>An empty buffer for storing the NAPT entry data structure.<nl>[out]<tab>The data structure of found NAPT entry.
 *      *valid_idx - [in]<tab>The index which find from.<nl>[out]<tab>The existing entry index.
 * Return:
 *      RT_ERR_RG_OK
 *		RT_ERR_RG_NOT_INIT - system is not initiated. 
 *		RT_ERR_RG_INVALID_PARAM - illegal NAPT connection flow index.
 *		RT_ERR_RG_NAPT_SW_ENTRY_NOT_FOUND - the NAPT entry can not be found.
 * Note:
 *		naptInfo.is_tcp - Layer 4 protocol is TCP or not.<nl>
 *		naptInfo.local_ip - Internal IP address.<nl>
 *		naptInfo.remote_ip - Remote IP address.<nl>
 *		naptInfo.wan_intf_idx - Wan interface index.<nl>
 *		naptInfo.local_port - Internal port number.<nl>
 *		naptInfo.remote_port - Remote port number.<nl>
 *		naptInfo.external_port - Gateway external port number.<nl>
 *		naptInfo.pri_valid - NAPT priority remarking enable.<nl>
 *		naptInfo.priority - NAPT priority remarking value.<nl>
 *		naptInfo.idleSecs - NAPT flow idle seconds.<nl>
 *		naptInfo.state - NAPT connection state, 0:INVALID 1:SYN_RECV 2:SYN_ACK_RECV 3:CONNECTED 4:FIN_RECV 5:RST_RECV<nl>
 */
int32 
rtk_rg_naptConnection_find(rtk_rg_naptInfo_t *naptInfo,int *valid_idx)
{
    rtdrv_rg_naptConnection_find_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == naptInfo), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == valid_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.naptInfo, naptInfo, sizeof(rtk_rg_naptInfo_t));
    osal_memcpy(&cfg.valid_idx, valid_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_NAPTCONNECTION_FIND, &cfg, rtdrv_rg_naptConnection_find_t, 1);
    osal_memcpy(naptInfo, &cfg.naptInfo, sizeof(rtk_rg_naptInfo_t));
    osal_memcpy(valid_idx, &cfg.valid_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_naptConnection_find */


/* Sub-module Name: Multicast Flow */

/* Function Name:
 *      rtk_rg_multicastFlow_add
 * Description:
 *      Add a Multicast flow entry into ASIC.
 * Input: 
  * Output: 
 *      *mcFlow - [in]<tab>Multicast flow entry content structure.
 *      *flow_idx - [out]<tab>Returned Multicast flow index.
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_NOT_INIT - System is not initiated.  
 *      RT_ERR_RG_INVALID_PARAM - The params is not accpeted.
 *      RT_ERR_RG_ENTRY_FULL - The MC entry is full. 
 *      RT_ERR_FAILED - Failed
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_IPV4_ADDRESS  - Invalid IPv4 address
 *      RT_ERR_VLAN_VID      - invalid vlan id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_INPUT         - invalid input parameter
 *
 * Note:
 *      mcFlow.multicast_ipv4_addr - The destination IP address of packet, must be class D address.<nl>
 *      mcFlow.src_intf_idx - This packet is from which interface index.<nl>
 *      mcFlow.port_mask - This MC packet will forward to those ports<nl>
 *      mcFlow.ext_port_mask - This MC packet will forward to those extension ports.
 */
int32 
rtk_rg_multicastFlow_add(rtk_rg_multicastFlow_t *mcFlow, int *flow_idx)
{
    rtdrv_rg_multicastFlow_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == mcFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == flow_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.mcFlow, mcFlow, sizeof(rtk_rg_multicastFlow_t));
    osal_memcpy(&cfg.flow_idx, flow_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_MULTICASTFLOW_ADD, &cfg, rtdrv_rg_multicastFlow_add_t, 1);
    osal_memcpy(mcFlow, &cfg.mcFlow, sizeof(rtk_rg_multicastFlow_t));
    osal_memcpy(flow_idx, &cfg.flow_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_multicastFlow_add */


 /* Function Name:
 *      rtk_rg_multicastFlow_del
 * Description:
 *      Delete an Multicast flow ASIC entry.
 * Input:
 * Output:
 *      flow_idx - [in]<tab>The Multicast flow entry index for deleting. 
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_NOT_INIT - system is not initiated. 
 *      RT_ERR_RG_INDEX_OUT_OF_RANGE - the index out of range.
 *      RT_ERR_RG_ENTRY_NOT_EXIST - the index is not exist.
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_L2_EMPTY_ENTRY   - Empty LUT entry.
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid) 
 * Note:
 *      None
 */
int32 
rtk_rg_multicastFlow_del(int flow_idx)
{
    rtdrv_rg_multicastFlow_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.flow_idx, &flow_idx, sizeof(int));
    SETSOCKOPT(RTDRV_RG_MULTICASTFLOW_DEL, &cfg, rtdrv_rg_multicastFlow_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_multicastFlow_del */

/* Function Name:
 *      rtk_rg_multicastFlow_find
 * Description:
 *      Find an exist Multicast flow ASIC entry.
 * Input:
 * Output: 
 *      *mcFlow - [in]<tab>An empty buffer for storing the structure.<nl>[out]<tab>An exist entry structure which index is valid_idx.
 *      *valid_idx - [in]<tab>The index which find from.<nl>[out]<tab>The exist entry index.
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_NOT_INIT - system is not initiated. 
 *      RT_ERR_RG_INDEX_OUT_OF_RANGE - the index out of range.
 *      RT_ERR_RG_NO_MORE_ENTRY_FOUND - no more exist entry is found.
 *      RT_ERR_FAILED - Failed
 *      RT_ERR_L2_EMPTY_ENTRY - Empty LUT entry.
 *      RT_ERR_INPUT  - Invalid input parameters. 
 * Note:
 *      None 
 */
int32 
rtk_rg_multicastFlow_find(rtk_rg_multicastFlow_t *mcFlow, int *valid_idx)
{
    rtdrv_rg_multicastFlow_find_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == mcFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == valid_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.mcFlow, mcFlow, sizeof(rtk_rg_multicastFlow_t));
    osal_memcpy(&cfg.valid_idx, valid_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_MULTICASTFLOW_FIND, &cfg, rtdrv_rg_multicastFlow_find_t, 1);
    osal_memcpy(mcFlow, &cfg.mcFlow, sizeof(rtk_rg_multicastFlow_t));
    osal_memcpy(valid_idx, &cfg.valid_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_multicastFlow_find */

/* Function Name:
 *      rtk_rg_multicastDataAlllowedPortMask_set
 * Description:
 *      Set allowed port mask for mutlicast data packets.
 * Input:
 * Output: 
 *      portmask - [in]<tab>set allowed port mask for multicast data packets.
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_NOT_INIT - system is not initiated.
 * Note:
 *      None
 */
int32 
rtk_rg_multicastDataIngressAlllowedPortMask_set(rtk_rg_mac_portmask_t *macPort_mask)
{
    rtdrv_rg_multicastDataIngressAlllowedPortMask_set_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == macPort_mask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.macPort_mask, macPort_mask, sizeof(rtk_rg_mac_portmask_t));
    GETSOCKOPT(RTDRV_RG_MULTICASTDATAINGRESSALLLOWEDPORTMASK_SET, &cfg, rtdrv_rg_multicastDataIngressAlllowedPortMask_set_t, 1);
    osal_memcpy(macPort_mask, &cfg.macPort_mask, sizeof(rtk_rg_mac_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_multicastDataIngressAlllowedPortMask_set */

/* Function Name:
 *      rtk_rg_multicastDataAlllowedPortMask_get
 * Description:
 *      Get allowed port mask for mutlicast data packets.
 * Input:
 * Output: 
 *      portmask - [out]<tab>Return allowed port mask for multicast data packets.
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_NOT_INIT - system is not initiated.
 * Note:
 *      None
 */
int32 
rtk_rg_multicastDataIngressAlllowedPortMask_get(rtk_rg_mac_portmask_t *macPort_mask)
{
    rtdrv_rg_multicastDataIngressAlllowedPortMask_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == macPort_mask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.macPort_mask, macPort_mask, sizeof(rtk_rg_mac_portmask_t));
    GETSOCKOPT(RTDRV_RG_MULTICASTDATAINGRESSALLLOWEDPORTMASK_GET, &cfg, rtdrv_rg_multicastDataIngressAlllowedPortMask_get_t, 1);
    osal_memcpy(macPort_mask, &cfg.macPort_mask, sizeof(rtk_rg_mac_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_multicastDataIngressAlllowedPortMask_get */


/* Sub-module Name: Layer2 configuration */

/* Function Name:
 *      rtk_rg_macEntry_add
 * Description:
 *      Add a MAC Entry into ASIC
 * Input:
 * Output:
 *      *macEntry - [in]<tab>MAC entry content structure.
 *      *entry_idx - [out]<tab>this MAC entry index.
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_NOT_INIT - system is not initiated. 
 * Note:
 *      macEntry.mac - The MAC address.<nl>
 *      macEntry.isIVL - The MAC is for IVL or SVL.<nl>
 *      macEntry.fid - The fid, only used in SVL.<nl>
 *      macEntry.vlan_id - The VLAN id.<nl>
 *      macEntry.port_idx - The port index,0~5 for phy. port, 6 for CPU port, 7~11 for ext. port.<nl>
 *      macEntry.arp_used - The entry is used for NAT/NAPT.<nl>
 *      macEntry.static_entry - The MAC is static or not.<nl>
 */
int32 
rtk_rg_macEntry_add(rtk_rg_macEntry_t *macEntry, int *entry_idx)
{
    rtdrv_rg_macEntry_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == macEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == entry_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.macEntry, macEntry, sizeof(rtk_rg_macEntry_t));
    osal_memcpy(&cfg.entry_idx, entry_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_MACENTRY_ADD, &cfg, rtdrv_rg_macEntry_add_t, 1);
    osal_memcpy(macEntry, &cfg.macEntry, sizeof(rtk_rg_macEntry_t));
    osal_memcpy(entry_idx, &cfg.entry_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_macEntry_add */


/* Function Name:
 *      rtk_rg_macEntry_del
 * Description:
 *      Delete an ASIC MAC Entry.
 * Input:
 * Output:
 *      entry_idx - [in]<tab>The MAC entry index for deleting. 
 * Return:
 *      RT_ERR_RG_OK - Success 
 *      RT_ERR_RG_NOT_INIT - system is not initiated. 
 *      RT_ERR_RG_INDEX_OUT_OF_RANGE - the index out of range.
 *      RT_ERR_RG_ENTRY_NOT_EXIST - the index is not exist.
 * Note:
 *      None
 */
int32 
rtk_rg_macEntry_del(int entry_idx)
{
    rtdrv_rg_macEntry_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.entry_idx, &entry_idx, sizeof(int));
    SETSOCKOPT(RTDRV_RG_MACENTRY_DEL, &cfg, rtdrv_rg_macEntry_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_macEntry_del */


/* Function Name:
 *      rtk_rg_macEntry_find
 * Description:
 *      Find an exist ASIC MAC Entry.
 * Input:
 * Output: 
 *      *macEntry - [in]<tab>An empty buffer for storing the MAC entry data structure.<nl>[out]<tab>The data structure of found MAC entry.
 *      *valid_idx - [in]<tab>The index which find from.<nl>[out]<tab>The existing entry index.
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_NOT_INIT - system is not initiated. 
 *      RT_ERR_RG_INDEX_OUT_OF_RANGE - the index out of range.
 *      RT_ERR_RG_NO_MORE_ENTRY_FOUND - no more exist entry is found.
 *      RT_ERR_RG_NULL_POINTER - Input parameter is NULL pointer.
 * Note:
 *      macEntry.mac - The MAC address.<nl>
 *      macEntry.isIVL - The MAC is for IVL or SVL.<nl>
 *      macEntry.fid - The fid, only used in SVL.<nl>
 *      macEntry.vlan_id - The VLAN id.<nl>
 *      macEntry.port_idx - The port index,0~5 for phy. port, 6 for CPU port, 7~11 for ext. port.<nl>
 *      macEntry.arp_used - The entry is used for NAT/NAPT.<nl>
 *      macEntry.static_entry - The MAC is static or not.<nl>
 */
int32 
rtk_rg_macEntry_find(rtk_rg_macEntry_t *macEntry,int *valid_idx)
{
    rtdrv_rg_macEntry_find_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == macEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == valid_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.macEntry, macEntry, sizeof(rtk_rg_macEntry_t));
    osal_memcpy(&cfg.valid_idx, valid_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_MACENTRY_FIND, &cfg, rtdrv_rg_macEntry_find_t, 1);
    osal_memcpy(macEntry, &cfg.macEntry, sizeof(rtk_rg_macEntry_t));
    osal_memcpy(valid_idx, &cfg.valid_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_macEntry_find */


/* Function Name:
 *      rtk_rg_arpEntry_add
 * Description:
 *      Add an ARP Entry into ASIC
 * Input:
 * Output:
 *      arpEntry - [in]<tab>Fill rtk_rg_arpEntry_t each fields for adding.
 *      arp_entry_idx - [out]<tab>Return ARP entry index.
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_NOT_INIT - system is not initiated. 
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information.
 *      RT_ERR_RG_L2_ENTRY_NOT_FOUND - L2 entry not found.
 * Note:
 *      arpEntry.macEntryIdx - this MAC entry index of this ARP entry.<nl>
 *      arpEntry.ipv4Addr - the IPv4 IP address of this ARP entry.<nl>
 *      arpEntry.static_entry - this entry is static ARP, and it will never age out.<nl>
 *      If the arpEntry.static_entry is set. The MAC entry must set static, too.
 */
int32 
rtk_rg_arpEntry_add(rtk_rg_arpEntry_t *arpEntry, int *arp_entry_idx)
{
    rtdrv_rg_arpEntry_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == arpEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == arp_entry_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.arpEntry, arpEntry, sizeof(rtk_rg_arpEntry_t));
    osal_memcpy(&cfg.arp_entry_idx, arp_entry_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_ARPENTRY_ADD, &cfg, rtdrv_rg_arpEntry_add_t, 1);
    osal_memcpy(arpEntry, &cfg.arpEntry, sizeof(rtk_rg_arpEntry_t));
    osal_memcpy(arp_entry_idx, &cfg.arp_entry_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_arpEntry_add */


/* Function Name:
 *      rtk_rg_arpEntry_del
 * Description:
 *      Delete an ASIC ARP Entry.
 * Input:
 * Output:
 *      arp_entry_idx - [out]<tab>The ARP entry index for deleting. 
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_NOT_INIT - system is not initiated. 
 *      RT_ERR_RG_INDEX_OUT_OF_RANGE - the index out of range.
 *      RT_ERR_RG_ENTRY_NOT_EXIST - the index is not exist.
 * Note:
 *      None
 */
int32 
rtk_rg_arpEntry_del(int arp_entry_idx)
{
    rtdrv_rg_arpEntry_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.arp_entry_idx, &arp_entry_idx, sizeof(int));
    SETSOCKOPT(RTDRV_RG_ARPENTRY_DEL, &cfg, rtdrv_rg_arpEntry_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_arpEntry_del */


/* Function Name:
 *      rtk_rg_arpEntry_find
 * Description:
 *      Find the entire ASIC ARP table from index arp_valid_idx till valid one.
 * Input:
 * Output: 
 *      *arpInfo - [in]<tab>An empty buffer for storing the ARP entry data structure.<nl>[out]<tab>The data structure of found ARP entry.
 *      *arp_valid_idx - [in]<tab>The index which find from.<nl>[out]<tab>The existing entry index.
 * Return:
 *      RT_ERR_RG_OK - Success 
 *      RT_ERR_RG_NOT_INIT - system is not initiated.
 *      RT_ERR_RG_NULL_POINTER - input buffer pointer is NULL.
 *      RT_ERR_RG_INDEX_OUT_OF_RANGE - the index out of range
 *      RT_ERR_RG_NO_MORE_ENTRY_FOUND - no more exist entry is found. 
 * Note:
 *      arpInfo.arpEntry - The ARP entry data structure.<nl>
 *      arpInfo.valid - The ARP entry is valid or not.<nl>
 *      arpEntry.idleSecs - The ARP entry idle time in seconds.<nl>
 */
int32 
rtk_rg_arpEntry_find(rtk_rg_arpInfo_t *arpInfo,int *arp_valid_idx)
{
    rtdrv_rg_arpEntry_find_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == arpInfo), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == arp_valid_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.arpInfo, arpInfo, sizeof(rtk_rg_arpInfo_t));
    osal_memcpy(&cfg.arp_valid_idx, arp_valid_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_ARPENTRY_FIND, &cfg, rtdrv_rg_arpEntry_find_t, 1);
    osal_memcpy(arpInfo, &cfg.arpInfo, sizeof(rtk_rg_arpInfo_t));
    osal_memcpy(arp_valid_idx, &cfg.arp_valid_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_arpEntry_find */

/* Sub-module Name: ALG(Application Level Gateway) configuration */

/* Function Name:
 *      rtk_rg_algAppsServerInLanIpAddr_add
 * Description:
 *      Add ServerInLan ALG function IP mapping.
 * Input:
 * Output:
 *      srvIpMapping - [in]<tab>The mapping of bitmask of ALG ServerInLan functions and server IP address.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_INVALID_PARAM
 *      RT_ERR_RG_ALG_SRV_IN_LAN_EXIST - the server ip address had been assigned
 * Note:
 *      srvIpMapping.algType - indicate which ALG service should assign to the serverAddress.<nl>
 *      srvIpMapping.serverAddress - indicate the server IP address.<nl>
 *      Before call rtk_rg_algApps_set to setup Server In Lan service, this IP mapping should be enter at first, otherwise rtk_rg_algApps_set will return failure.
 */
int32
rtk_rg_algServerInLanAppsIpAddr_add(rtk_rg_alg_serverIpMapping_t *srvIpMapping)
{
    rtdrv_rg_algServerInLanAppsIpAddr_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == srvIpMapping), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.srvIpMapping, srvIpMapping, sizeof(rtk_rg_alg_serverIpMapping_t));
    GETSOCKOPT(RTDRV_RG_ALGSERVERINLANAPPSIPADDR_ADD, &cfg, rtdrv_rg_algServerInLanAppsIpAddr_add_t, 1);
    osal_memcpy(srvIpMapping, &cfg.srvIpMapping, sizeof(rtk_rg_alg_serverIpMapping_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_algServerInLanAppsIpAddr_add */

/* Function Name:
 *      rtk_rg_algAppsServerInLanIpAddr_del
 * Description:
 *      Delete ServerInLan ALG function IP mapping.
 * Input:
 * Output:
 *       delServerMapping - [in]<tab>Delete the server IP address mapping.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_INVALID_PARAM
 * Note:
 *      None
 */
int32
rtk_rg_algServerInLanAppsIpAddr_del(rtk_rg_alg_type_t delServerMapping)
{
    rtdrv_rg_algServerInLanAppsIpAddr_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.delServerMapping, &delServerMapping, sizeof(rtk_rg_alg_type_t));
    SETSOCKOPT(RTDRV_RG_ALGSERVERINLANAPPSIPADDR_DEL, &cfg, rtdrv_rg_algServerInLanAppsIpAddr_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_algServerInLanAppsIpAddr_del */


/* Function Name:
 *      rtk_rg_algApps_set
 * Description:
 *      Set ALG functions by bitmask.
 * Input:
 * Output:
 *      alg_app - [in]<tab>The bitmask setting for all ALG functions.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_ALG_SRV_IN_LAN_NO_IP - before turn on Server In Lan services, the server ip has to be added by rtk_rg_algServerInLanAppsIpAddr_add
 * Note:
 *      Althrough the bitmask list all ALG functions here, the implemented functions depend on romeDriver's version. Please refer user document.
 */
int32
rtk_rg_algApps_set(rtk_rg_alg_type_t alg_app)
{
    rtdrv_rg_algApps_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.alg_app, &alg_app, sizeof(rtk_rg_alg_type_t));
    SETSOCKOPT(RTDRV_RG_ALGAPPS_SET, &cfg, rtdrv_rg_algApps_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_algApps_set */


/* Function Name:
 *      rtk_rg_algApps_get
 * Description:
 *      Get ALG functions by bitmask.
 * Input:
 * Output:
 *       alg_app - [out]<tab>Return the bitmask setting for ALG functions.
 * Return:
 *      RT_ERR_RG_OK
 * Note:
 *      Althrough the bitmask list all ALG functions here, the implemented functions depend on romeDriver's version. Please refer user document.
 */
int32
rtk_rg_algApps_get(rtk_rg_alg_type_t *alg_app)
{
    rtdrv_rg_algApps_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == alg_app), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.alg_app, alg_app, sizeof(rtk_rg_alg_type_t));
    GETSOCKOPT(RTDRV_RG_ALGAPPS_GET, &cfg, rtdrv_rg_algApps_get_t, 1);
    osal_memcpy(alg_app, &cfg.alg_app, sizeof(rtk_rg_alg_type_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_algApps_get */

int32
rtk_rg_dmzHost_set(int wan_intf_idx, rtk_rg_dmzInfo_t *dmz_info)
{
    rtdrv_rg_dmzHost_set_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == dmz_info), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.wan_intf_idx, &wan_intf_idx, sizeof(int));
    osal_memcpy(&cfg.dmz_info, dmz_info, sizeof(rtk_rg_dmzInfo_t));
    GETSOCKOPT(RTDRV_RG_DMZHOST_SET, &cfg, rtdrv_rg_dmzHost_set_t, 1);
    osal_memcpy(dmz_info, &cfg.dmz_info, sizeof(rtk_rg_dmzInfo_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_dmzHost_set */
int32
rtk_rg_dmzHost_get(int wan_intf_idx, rtk_rg_dmzInfo_t *dmz_info)
{
    rtdrv_rg_dmzHost_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == dmz_info), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.wan_intf_idx, &wan_intf_idx, sizeof(int));
    osal_memcpy(&cfg.dmz_info, dmz_info, sizeof(rtk_rg_dmzInfo_t));
    GETSOCKOPT(RTDRV_RG_DMZHOST_GET, &cfg, rtdrv_rg_dmzHost_get_t, 1);
    osal_memcpy(dmz_info, &cfg.dmz_info, sizeof(rtk_rg_dmzInfo_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_dmzHost_get */

//QoS


//IGMP/MLD snooping

//IPv6
/* Function Name:
 *      rtk_rg_neighborEntry_add
 * Description:
 *      Add an Neighbor Entry into ASIC
 * Input:
 * Output:
 *      neighborEntry - [in]<tab>Fill rtk_rg_neighborEntry_t each fields for adding.
 *      neighbor_idx - [out]<tab>Return Neighbor entry index.
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_NOT_INIT - system is not initiated. 
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information.
 *      RT_ERR_RG_L2_ENTRY_NOT_FOUND - L2 entry not found.
 *      RT_ERR_RG_NEIGHBOR_FULL - neighbor table is full
 * Note:
 *      neighborEntry.l2Idx - this MAC entry index of this Neighbor entry.<nl>
 *      neighborEntry.matchRouteIdx - the routing entry idx that match the ip address.<nl>
 *      neighborEntry.interfaceId - the 64 bits interface identifier of this IPv6 address.<nl>
 *      neighborEntry.staticEntry - this entry is static ARP, and it will never age out.<nl>
 *      neighborEntry.valid - this entry is valid.<nl>
 *      If the neighborEntry.static_entry is set. The MAC entry must set static, too.
 */
int32
rtk_rg_neighborEntry_add(rtk_rg_neighborEntry_t *neighborEntry,int *neighbor_idx)
{
    rtdrv_rg_neighborEntry_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == neighborEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == neighbor_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.neighborEntry, neighborEntry, sizeof(rtk_rg_neighborEntry_t));
    osal_memcpy(&cfg.neighbor_idx, neighbor_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_NEIGHBORENTRY_ADD, &cfg, rtdrv_rg_neighborEntry_add_t, 1);
    osal_memcpy(neighborEntry, &cfg.neighborEntry, sizeof(rtk_rg_neighborEntry_t));
    osal_memcpy(neighbor_idx, &cfg.neighbor_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_neighborEntry_add */

/* Function Name:
 *      rtk_rg_neighborEntry_del
 * Description:
 *      Delete an ASIC Neighbor Entry.
 * Input:
 * Output:
 *      neighbor_idx - [out]<tab>The Neighbor entry index for deleting. 
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_NOT_INIT - system is not initiated. 
 *      RT_ERR_RG_INDEX_OUT_OF_RANGE - the index out of range.
 *      RT_ERR_RG_ENTRY_NOT_EXIST - the index is not exist.
 * Note:
 *      None
 */
int32
rtk_rg_neighborEntry_del(int neighbor_idx)
{
    rtdrv_rg_neighborEntry_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.neighbor_idx, &neighbor_idx, sizeof(int));
    SETSOCKOPT(RTDRV_RG_NEIGHBORENTRY_DEL, &cfg, rtdrv_rg_neighborEntry_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_neighborEntry_del */

/* Function Name:
 *      rtk_rg_neighborEntry_find
 * Description:
 *      Find the entire ASIC Neighbor table from index neighbor_valid_idx till valid one.
 * Input:
 * Output: 
 *      *neighborInfo - [in]<tab>An empty buffer for storing the Neighbor entry data structure.<nl>[out]<tab>The data structure of found Neighbor entry.
 *      *neighbor_valid_idx - [in]<tab>The index which find from.<nl>[out]<tab>The existing entry index.
 * Return:
 *      RT_ERR_RG_OK - Success 
 *      RT_ERR_RG_NOT_INIT - system is not initiated.
 *      RT_ERR_RG_NULL_POINTER - input buffer pointer is NULL.
 *      RT_ERR_RG_INDEX_OUT_OF_RANGE - the index out of range
 *      RT_ERR_RG_NO_MORE_ENTRY_FOUND - no more exist entry is found. 
 *      RT_ERR_RG_NEIGHBOR_NOT_FOUND - the indicated neighbor ifid is not found.
 * Note:
 *      neighborInfo.arpEntry - The Neighbor entry data structure.<nl>
 *      neighborInfo.idleSecs - The Neighbor entry idle time in seconds.<nl>
 */
int32
rtk_rg_neighborEntry_find(rtk_rg_neighborInfo_t *neighborInfo,int *neighbor_valid_idx)
{
    rtdrv_rg_neighborEntry_find_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == neighborInfo), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == neighbor_valid_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.neighborInfo, neighborInfo, sizeof(rtk_rg_neighborInfo_t));
    osal_memcpy(&cfg.neighbor_valid_idx, neighbor_valid_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_NEIGHBORENTRY_FIND, &cfg, rtdrv_rg_neighborEntry_find_t, 1);
    osal_memcpy(neighborInfo, &cfg.neighborInfo, sizeof(rtk_rg_neighborInfo_t));
    osal_memcpy(neighbor_valid_idx, &cfg.neighbor_valid_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_neighborEntry_find */


/* Function Name:
 *      rtk_rg_qos_schedulingQueue_set
 * Description:
 *      Set the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 * Output:
 *      port_idx - [in]<tab>The port number
 *      q_weight - [in]<tab>The array of weights for WRR/WFQ queue (valid:1~128, 0 for STRICT_PRIORITY queue)
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_INVALID_PARAM
 * Note:
 *      The types of queue are: WFQ_WRR_PRIORITY or STRICT_PRIORITY.
 *      If the weight is 0 then the type is STRICT_PRIORITY, else the type is WFQ_W
RR_PRIORITY.
 */
int32
rtk_rg_qos_schedulingQueue_set(rtk_rg_mac_port_idx_t port_idx,rtk_rg_qos_queue_weights_t q_weight)
{
    rtdrv_rg_qos_schedulingQueue_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.port_idx, &port_idx, sizeof(rtk_rg_mac_port_idx_t));
    osal_memcpy(&cfg.q_weight, &q_weight, sizeof(rtk_rg_qos_queue_weights_t));
    SETSOCKOPT(RTDRV_RG_QOS_SCHEDULINGQUEUE_SET, &cfg, rtdrv_rg_qos_schedulingQueue_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_qos_schedulingQueue_set */


/* Function Name:
 *      rtk_rg_qos_schedulingQueue_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 * Output:
 *      port_num - [in]<tab>The port number.
 *      sa_learnLimit_info - [out]<tab>The array of weights for WRR/WFQ queue (valid:1~128, 0 for STRICT_PRIORITY queue)
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_INVALID_PARAM
 *      RT_ERR_RG_NULL_POINTER
 * Note:
 *      The types of queue are: WFQ_WRR_PRIORITY or STRICT_PRIORITY.
 *      If the weight is 0 then the type is STRICT_PRIORITY, else the type is WFQ_W
RR_PRIORITY.
 */
int32
rtk_rg_qos_schedulingQueue_get(rtk_rg_mac_port_idx_t port_idx,rtk_rg_qos_queue_weights_t* q_weight)
{
    rtdrv_rg_qos_schedulingQueue_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == q_weight), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.port_idx, &port_idx, sizeof(rtk_rg_mac_port_idx_t));
    osal_memcpy(&cfg.q_weight, q_weight, sizeof(rtk_rg_qos_queue_weights_t));
    GETSOCKOPT(RTDRV_RG_QOS_SCHEDULINGQUEUE_GET, &cfg, rtdrv_rg_qos_schedulingQueue_get_t, 1);
    osal_memcpy(q_weight, &cfg.q_weight, sizeof(rtk_rg_qos_queue_weights_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_qos_schedulingQueue_get */



/* Sub-module Name: DoS Functions */
/* Function Name:
 *      rtk_rg_dosPortMaskEnable_set
 * Description:
 *      Enable/Disable denial of service security port.
 * Input:
 * Output:
 *      dos_port_mask - [in]<tab>Security MAC port mask.
 * Return:
 *      RT_ERR_RG_OK - Success
 * Note:
 *      None
 */
int32
rtk_rg_dosPortMaskEnable_set(rtk_rg_mac_portmask_t dos_port_mask)
{
    rtdrv_rg_dosPortMaskEnable_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.dos_port_mask, &dos_port_mask, sizeof(rtk_rg_mac_portmask_t));
    SETSOCKOPT(RTDRV_RG_DOSPORTMASKENABLE_SET, &cfg, rtdrv_rg_dosPortMaskEnable_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_dosPortMaskEnable_set */

/* Function Name:
 *      rtk_rg_dosPortMaskEnable_get
 * Description:
 *      Get denial of service port security port state.
 * Input:
 * Output:
 *      *dos_port_mask - [out]<tab>Security MAC port mask.
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_NULL_POINTER - input port mask pointer is NULL.
 * Note:
 *      None
 */
int32
rtk_rg_dosPortMaskEnable_get(rtk_rg_mac_portmask_t *dos_port_mask)
{
    rtdrv_rg_dosPortMaskEnable_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == dos_port_mask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.dos_port_mask, dos_port_mask, sizeof(rtk_rg_mac_portmask_t));
    GETSOCKOPT(RTDRV_RG_DOSPORTMASKENABLE_GET, &cfg, rtdrv_rg_dosPortMaskEnable_get_t, 1);
    osal_memcpy(dos_port_mask, &cfg.dos_port_mask, sizeof(rtk_rg_mac_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_dosPortMaskEnable_get */

/* Function Name:
 *      rtk_rg_dosType_set
 * Description:
 *      Set denial of service type function.
 * Input:
 * Output:
 *      dos_type - [in]<tab>Port security type.
 *      dos_enabled - [in]<tab>Port security function enabled/disabled.
 *      dos_action - [in]<tab>Port security action.
 * Return:
 *      RT_ERR_RG_OK - Success
 *              RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information.
 * Note:
 *      dos_type.RTK_RG_DAEQSA_DENY - packets while SMAC is the same as DMAC.
 *      dos_type.RTK_RG_LAND_DENY - packets while SIP is the same as DIP(support IPv4 only).
 *      dos_type.RTK_RG_BLAT_DENY - packets while the TCP/UDP SPORT is the same as DPORT destination TCP/UDP port.
 *      dos_type.RTK_RG_SYNFIN_DENY - TCP packets while SYN and FIN bits are set.
 *      dos_type.RTK_RG_XMA_DENY - TCP packets while sequence number is zero and FIN,URG,PSH bits are set.
 *      dos_type.RTK_RG_NULLSCAN_DENY - TCP packets while sequence number is zero and all contorl bits are zeros.
 *      dos_type.RTK_RG_SYN_SPORTL1024_DENY - TCP SYN packets with source port less than 1024.
 *      dos_type.RTK_RG_TCPHDR_MIN_CHECK - the length of a TCP header carried in an unfragmented IP(IPv4 and IPv6) datagram or the first fragment of a fragmented IP(IPv4) d
atagram is less than MIN_TCP_Header_Size(20 bytes).
 *      dos_type.RTK_RG_TCP_FRAG_OFF_MIN_CHECK - the Frangment_Offset=1 in anyfragment of a fragmented IP datagram carrying part of TCP data.
 *      dos_type.RTK_RG_ICMP_FRAG_PKTS_DENY - ICMPv4/ICMPv6 data unit carried in a fragmented IP datagram.
 *      dos_type.RTK_RG_POD_DENY - IP packet size > 65535 bytes, ((IP offset *8) + (IP length) <A1>V (IPIHL*4))>65535.
 *      dos_type.RTK_RG_UDPDOMB_DENY - UDP length > IP payload length.
 *      dos_type.RTK_RG_SYNWITHDATA_DENY - 1. IP length > IP header + TCP header length while SYN flag is set 1. 2. IP More Fragment and Offset > 0 while SYN is set to 1.
 *      dos_action.RTK_RG_DOS_ACTION_DROP - Drop packet while hit DoS criteria.
 *      dos_action.RTK_RG_DOS_ACTION_TRAP - Trap packet to CPU while hit DoS criteria.
 */
int32
rtk_rg_dosType_set(rtk_rg_dos_type_t dos_type,int dos_enabled,rtk_rg_dos_action_t dos_action)
{
    rtdrv_rg_dosType_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.dos_type, &dos_type, sizeof(rtk_rg_dos_type_t));
    osal_memcpy(&cfg.dos_enabled, &dos_enabled, sizeof(int));
    osal_memcpy(&cfg.dos_action, &dos_action, sizeof(rtk_rg_dos_action_t));
    SETSOCKOPT(RTDRV_RG_DOSTYPE_SET, &cfg, rtdrv_rg_dosType_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_dosType_set */

/* Function Name:
 *      rtk_rg_dosType_get
 * Description:
 *      Get denial of service type function.
 * Input:
 * Output:
 *      dos_type - [in]<tab>Port security type.
 *      *dos_enabled - [out]<tab>Port security function enabled/disabled.
 *      *dos_action - [out]<tab>Port security action.
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_NULL_POINTER - input port mask pointer is NULL.
 * Note:
 *      dos_type.RTK_RG_DAEQSA_DENY - packets while SMAC is the same as DMAC.
 *      dos_type.RTK_RG_LAND_DENY - packets while SIP is the same as DIP(support IPv4 only).
 *      dos_type.RTK_RG_BLAT_DENY - packets while the TCP/UDP SPORT is the same as DPORT destination TCP/UDP port.
 *      dos_type.RTK_RG_SYNFIN_DENY - TCP packets while SYN and FIN bits are set.
 *      dos_type.RTK_RG_XMA_DENY - TCP packets while sequence number is zero and FIN,URG,PSH bits are set.
 *      dos_type.RTK_RG_NULLSCAN_DENY - TCP packets while sequence number is zero and all contorl bits are zeros.
 *      dos_type.RTK_RG_SYN_SPORTL1024_DENY - TCP SYN packets with source port less than 1024.
 *      dos_type.RTK_RG_TCPHDR_MIN_CHECK - the length of a TCP header carried in an unfragmented IP(IPv4 and IPv6) datagram or the first fragment of a fragmented IP(IPv4) d
atagram is less than MIN_TCP_Header_Size(20 bytes).
 *      dos_type.RTK_RG_TCP_FRAG_OFF_MIN_CHECK - the Frangment_Offset=1 in anyfragment of a fragmented IP datagram carrying part of TCP data.
 *      dos_type.RTK_RG_ICMP_FRAG_PKTS_DENY - ICMPv4/ICMPv6 data unit carried in a fragmented IP datagram.
 *      dos_type.RTK_RG_POD_DENY - IP packet size > 65535 bytes, ((IP offset *8) + (IP length) <A1>V (IPIHL*4))>65535.
 *      dos_type.RTK_RG_UDPDOMB_DENY - UDP length > IP payload length.
 *      dos_type.RTK_RG_SYNWITHDATA_DENY - 1. IP length > IP header + TCP header length while SYN flag is set 1. 2. IP More Fragment and Offset > 0 while SYN is set to 1.
 *      dos_action.RTK_RG_DOS_ACTION_DROP - Drop packet while hit DoS criteria.
 *      dos_action.RTK_RG_DOS_ACTION_TRAP - Trap packet to CPU while hit DoS criteria.
 */
int32
rtk_rg_dosType_get(rtk_rg_dos_type_t dos_type,int *dos_enabled,rtk_rg_dos_action_t *dos_action)
{
    rtdrv_rg_dosType_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == dos_enabled), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == dos_action), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.dos_type, &dos_type, sizeof(rtk_rg_dos_type_t));
    osal_memcpy(&cfg.dos_enabled, dos_enabled, sizeof(int));
    osal_memcpy(&cfg.dos_action, dos_action, sizeof(rtk_rg_dos_action_t));
    GETSOCKOPT(RTDRV_RG_DOSTYPE_GET, &cfg, rtdrv_rg_dosType_get_t, 1);
    osal_memcpy(dos_enabled, &cfg.dos_enabled, sizeof(int));
    osal_memcpy(dos_action, &cfg.dos_action, sizeof(rtk_rg_dos_action_t));
	
	return RT_ERR_OK;
}	/* end of rtk_rg_dosType_get */

/* Function Name:
 *      rtk_rg_dosFloodType_set
 * Description:
 *      Set denial of service flooding attack protection function.
 * Input:
 * Output:
 *      dos_type - [in]<tab>Port security type.
 *      dos_enabled - [in]<tab>Port security function enabled/disabled.
 *      dos_action - [in]<tab>Port security action.
 *              dos_threshold - [in]<tab>System-based SYN/FIN/ICMP flood threshold.
 * Return:
 *      RT_ERR_RG_OK - Success
 *              RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information.
 * Note:
 *      dos_type.RTK_RG_SYNFLOOD_DENY - Receiving TCP SYN packet number is over threshold.
 *      dos_type.RTK_RG_FINFLOOD_DENY - Receiving TCP FIN packet number is over threshold.
 *      dos_type.RTK_RG_ICMPFLOOD_DENY - Receiving ICMP packet number is over threshold.
 *      dos_action.RTK_RG_DOS_ACTION_DROP - Drop packet while hit DoS criteria.
 *      dos_action.RTK_RG_DOS_ACTION_TRAP - Trap packet to CPU while hit DoS criteria.
 *              dos_threshold - Allowable SYN/FIN/ICMP packet frame rate in 1k packets/sec.
 */
int32
rtk_rg_dosFloodType_set(rtk_rg_dos_type_t dos_type,int dos_enabled,rtk_rg_dos_action_t dos_action,int dos_threshold)
{
    rtdrv_rg_dosFloodType_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.dos_type, &dos_type, sizeof(rtk_rg_dos_type_t));
    osal_memcpy(&cfg.dos_enabled, &dos_enabled, sizeof(int));
    osal_memcpy(&cfg.dos_action, &dos_action, sizeof(rtk_rg_dos_action_t));
    osal_memcpy(&cfg.dos_threshold, &dos_threshold, sizeof(int));
    SETSOCKOPT(RTDRV_RG_DOSFLOODTYPE_SET, &cfg, rtdrv_rg_dosFloodType_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_dosFloodType_set */


/* Function Name:
 *      rtk_rg_dosFloodType_get
 * Description:
 *      Get denial of service port security setting.
 * Input:
 * Output:
 *      dos_type - [in]<tab>Port security type.
 *      *dos_enabled - [out]<tab>Port security function enabled/disabled.
 *      *dos_action - [out]<tab>Port security action.
 *              *dos_threshold - [out]<tab>System-based SYN/FIN/ICMP flood threshold.
 * Return:
 *      RT_ERR_RG_OK - Success
 *              RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information.
 *      RT_ERR_RG_NULL_POINTER - input port mask pointer is NULL.
 * Note:
 *      dos_type.RTK_RG_SYNFLOOD_DENY - Receiving TCP SYN packet number is over threshold.
 *      dos_type.RTK_RG_FINFLOOD_DENY - Receiving TCP FIN packet number is over threshold.
 *      dos_type.RTK_RG_ICMPFLOOD_DENY - Receiving ICMP packet number is over threshold.
 *      dos_action.RTK_RG_DOS_ACTION_DROP - Drop packet while hit DoS criteria.
 *      dos_action.RTK_RG_DOS_ACTION_TRAP - Trap packet to CPU while hit DoS criteria.
 *              dos_threshold - Allowable SYN/FIN/ICMP packet frame rate in 1k packets/sec.
 */
int32
rtk_rg_dosFloodType_get(rtk_rg_dos_type_t dos_type,int *dos_enabled,rtk_rg_dos_action_t *dos_action,int *dos_threshold)
{
    rtdrv_rg_dosFloodType_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == dos_enabled), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == dos_action), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == dos_threshold), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.dos_type, &dos_type, sizeof(rtk_rg_dos_type_t));
    osal_memcpy(&cfg.dos_enabled, dos_enabled, sizeof(int));
    osal_memcpy(&cfg.dos_action, dos_action, sizeof(rtk_rg_dos_action_t));
    osal_memcpy(&cfg.dos_threshold, dos_threshold, sizeof(int));
    GETSOCKOPT(RTDRV_RG_DOSFLOODTYPE_GET, &cfg, rtdrv_rg_dosFloodType_get_t, 1);
    osal_memcpy(dos_enabled, &cfg.dos_enabled, sizeof(int));
    osal_memcpy(dos_action, &cfg.dos_action, sizeof(rtk_rg_dos_action_t));
    osal_memcpy(dos_threshold, &cfg.dos_threshold, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_dosFloodType_get */


/* Function Name:
 *      rtk_rg_portMirror_set
 * Description:
 *      Enable portMirror for monitor Tx/Rx packet.
 * Input:
 * Output:
 *      rtk_rg_portMirrorInfo_t.monitorPort - [in]<tab>assign monitor port
 *      rtk_rg_portMirrorInfo_t.enabledPortMask - [in]<tab>enabled/disabled each port for mirror or not
 *      rtk_rg_portMirrorInfo_t.direct - [in]<tab>assign each port mirror tx/rx packet
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information.
 *      RT_ERR_RG_NULL_POINTER - input port mask pointer is NULL.
 * Note:
 *      None
 */
int32
rtk_rg_portMirror_set(rtk_rg_portMirrorInfo_t portMirrorInfo)
{
    rtdrv_rg_portMirror_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.portMirrorInfo, &portMirrorInfo, sizeof(rtk_rg_portMirrorInfo_t));
    SETSOCKOPT(RTDRV_RG_PORTMIRROR_SET, &cfg, rtdrv_rg_portMirror_set_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_rg_portMirror_set */



/* Function Name:
 *      rtk_rg_portMirror_get
 * Description:
 *      Get portMirror for monitor Tx/Rx packet information.
 * Input:
 * Output:
 *      rtk_rg_portMirrorInfo_t.monitorPort - [out]<tab>assigned monitor port
 *      rtk_rg_portMirrorInfo_t.enabledPortMask - [out]<tab>each port for mirror enabled/disabled information
 *      rtk_rg_portMirrorInfo_t.direct - [out]<tab>port mirror tx/rx packet information.
 * Return:
 *      RT_ERR_RG_OK - Success
 *      RT_ERR_RG_NULL_POINTER - input port mask pointer is NULL.
 * Note:
 *      None
 */
int32
rtk_rg_portMirror_get(rtk_rg_portMirrorInfo_t *portMirrorInfo)
{
    rtdrv_rg_portMirror_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == portMirrorInfo), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.portMirrorInfo, portMirrorInfo, sizeof(rtk_rg_portMirrorInfo_t));
    GETSOCKOPT(RTDRV_RG_PORTMIRROR_GET, &cfg, rtdrv_rg_portMirror_get_t, 1);
    osal_memcpy(portMirrorInfo, &cfg.portMirrorInfo, sizeof(rtk_rg_portMirrorInfo_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_portMirror_get */

/* Function Name:
 *      rtk_rg_portMirror_clear
 * Description:
 *      clear port mirror setting
 * Input:
 * Output:
 * Return:
 *      RT_ERR_RG_OK - Success
 * Note:
 *      None
 */
int32
rtk_rg_portMirror_clear(void)
{
    //rtdrv_rg_portMirror_clear_t cfg;

    /* function body */
    //SETSOCKOPT(RTDRV_RG_PORTMIRROR_CLEAR, &cfg, rtdrv_rg_portMirror_clear_t, 1);
    SETSOCKOPT(RTDRV_RG_PORTMIRROR_CLEAR, NULL, NULL, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_portMirror_clear */

/* Function Name:
 *      rtk_rg_cvlan_add
 * Description:
 *      Add customer VLAN setting.
 * Input:
 * Output:
 *      cvlan_info - [in]<tab>The VLAN configuration.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_INVALID_PARAM
 *      RT_ERR_RG_NOT_INIT
 *      RT_ERR_RG_VLAN_USED_BY_INTERFACE - the prefered VLAN ID had been used as interface VLAN ID
 *      RT_ERR_RG_VLAN_USED_BY_VLANBINDING - the prefered VLAN ID had been used as VLAN-binding ID
 *      RT_ERR_RG_CVLAN_CREATED - the VLAN ID had been created as customer VLAN before
 *      RT_ERR_RG_CVLAN_RESERVED - the VLAN ID is reserved for system use
 *      RT_ERR_RG_VLAN_SET_FAIL
 * Note:
 *      cvlan_info.vlanId - which VLAN identifier need to create, between 0 and 4095.<nl>
 *      cvlan_info.tagDecision - how to decide packet with this VLAN tag, by MAC learned or by VLAN untag setting.<nl>
 *      cvlan_info.memberPortMask - which port contained in this VLAN identifier.<nl>
 *      cvlan_info.untagPortMask - which port contained in this VLAN identifier should be untag.<nl>
 *      The untagPortMask only used when the tagDecision choose VLAN-based, otherwise it'll be forced to 0 in Mac-based.
 */
int32
rtk_rg_cvlan_add(rtk_rg_cvlan_info_t *cvlan_info)
{
    rtdrv_rg_cvlan_add_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == cvlan_info), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.cvlan_info, cvlan_info, sizeof(rtk_rg_cvlan_info_t));
    GETSOCKOPT(RTDRV_RG_CVLAN_ADD, &cfg, rtdrv_rg_cvlan_add_t, 1);
    osal_memcpy(cvlan_info, &cfg.cvlan_info, sizeof(rtk_rg_cvlan_info_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_cvlan_add */

/* Function Name:
 *      rtk_rg_cvlan_del
 * Description:
 *      Delete customer VLAN setting.
 * Input:
 * Output:
 *      cvlan_id - [in]<tab>The VLAN identifier needed to be deleted.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_VLAN_NOT_CREATED_BY_CVLAN - the deleting VLAN ID was not added as customer VLAN ID
 * Note:
 *      Here can not delete VLAN identifier used for interface or VLAN binding. Only the VLAN identifier created by rtk_rg_1qVlan_add can be deleted this way.
 */
int32
rtk_rg_cvlan_del(int cvlan_id)
{
    rtdrv_rg_cvlan_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.cvlan_id, &cvlan_id, sizeof(int));
    SETSOCKOPT(RTDRV_RG_CVLAN_DEL, &cfg, rtdrv_rg_cvlan_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_cvlan_del */

/* Sub-module Name: Source Address Learning Limit and Action Functions */
/* Function Name:
 *      rtk_rg_softwareSourceAddrLearningLimit_set
 * Description:
 *      Set source address learning limit and action when using software learning.
 * Input:
 * Output:
 *      port_num - [in]<tab>The port number to set source address learning information.
 *      sa_learnLimit_info - [in]<tab>The information entered for the dedicated port.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_INVALID_PARAM
 * Note:
 *      sa_learnLimit_info.learningLimitNumber - the maximum number can be learned in the port.<nl>
 *      sa_learnLimit_info.action - what to do if a packet is exceed the learning limit.<nl>
 */
int32
rtk_rg_softwareSourceAddrLearningLimit_set(rtk_rg_saLearningLimitInfo_t sa_learnLimit_info, rtk_rg_port_idx_t port_idx)
{
    rtdrv_rg_softwareSourceAddrLearningLimit_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.sa_learnLimit_info, &sa_learnLimit_info, sizeof(rtk_rg_saLearningLimitInfo_t));
    osal_memcpy(&cfg.port_idx, &port_idx, sizeof(rtk_rg_mac_port_idx_t));
    SETSOCKOPT(RTDRV_RG_SOFTWARESOURCEADDRLEARNINGLIMIT_SET, &cfg, rtdrv_rg_softwareSourceAddrLearningLimit_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_softwareSourceAddrLearningLimit_set */

/* Function Name:
 *      rtk_rg_softwareSourceAddrLearningLimit_get
 * Description:
 *      Get source address learning limit and action when using software learning by port number.
 * Input:
 * Output:
 *      port_num - [in]<tab>The port number to get source address learning information.
 *      sa_learnLimit_info - [out]<tab>The information contained of the dedicated port.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_INVALID_PARAM
 * Note:
 *      None
 */
int32
rtk_rg_softwareSourceAddrLearningLimit_get(rtk_rg_saLearningLimitInfo_t *sa_learnLimit_info, rtk_rg_port_idx_t port_idx)
{
    rtdrv_rg_softwareSourceAddrLearningLimit_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == sa_learnLimit_info), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.sa_learnLimit_info, sa_learnLimit_info, sizeof(rtk_rg_saLearningLimitInfo_t));
    osal_memcpy(&cfg.port_idx, &port_idx, sizeof(rtk_rg_mac_port_idx_t));
    GETSOCKOPT(RTDRV_RG_SOFTWARESOURCEADDRLEARNINGLIMIT_GET, &cfg, rtdrv_rg_softwareSourceAddrLearningLimit_get_t, 1);
    osal_memcpy(sa_learnLimit_info, &cfg.sa_learnLimit_info, sizeof(rtk_rg_saLearningLimitInfo_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_softwareSourceAddrLearningLimit_get */

/* Function Name:
 *      rtk_rg_portEgrBandwidthCtrlRate_set
 * Description:
 *      Set egress rate limitation for physical port.
 * Input:
 * Output:
 *      port - [in]<tab>assigned rate limit port
 *      rate - [in]<tab>assigned rate, unit Kbps
 * Return:
 *      RT_ERR_RG_OK - Success
 * Note:
 *      None
 */
int32
rtk_rg_portEgrBandwidthCtrlRate_set(rtk_rg_mac_port_idx_t port, uint32 rate)
{
    rtdrv_rg_portEgrBandwidthCtrlRate_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.port, &port, sizeof(rtk_rg_mac_port_idx_t));
    osal_memcpy(&cfg.rate, &rate, sizeof(uint32));
    SETSOCKOPT(RTDRV_RG_PORTEGRBANDWIDTHCTRLRATE_SET, &cfg, rtdrv_rg_portEgrBandwidthCtrlRate_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_portEgrBandwidthCtrlRate_set */

/* Function Name:
 *      rtk_rg_portIgrBandwidthCtrlRate_set
 * Description:
 *      Set ingress rate limitation for physical port.
 * Input:
 * Output:
 *      port - [in]<tab>assigned rate limit port
 *      rate - [in]<tab>assigned rate, unit Kbps
 * Return:
 *      RT_ERR_RG_OK - Success
 * Note:
 *      None
 */
int32
rtk_rg_portIgrBandwidthCtrlRate_set(rtk_rg_mac_port_idx_t port, uint32 rate)
{
    rtdrv_rg_portIgrBandwidthCtrlRate_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.port, &port, sizeof(rtk_rg_mac_port_idx_t));
    osal_memcpy(&cfg.rate, &rate, sizeof(uint32));
    SETSOCKOPT(RTDRV_RG_PORTIGRBANDWIDTHCTRLRATE_SET, &cfg, rtdrv_rg_portIgrBandwidthCtrlRate_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_portIgrBandwidthCtrlRate_set */

/* Function Name:
 *              rtk_rg_portEgrBandwidthCtrlRate_get
 * Description:
 *              Get egress rate limitation of physical port.
 * Input:
 * Output:
 *              port - [in]<tab>assigned rate limit port
 *              *rate - [out]<tab>assigned rate, unit Kbps
 * Return:
 *              RT_ERR_RG_OK - Success
 *              RT_ERR_RG_NULL_POINTER - input rate pointer is NULL.
 * Note:
 *              None
 */
int32
rtk_rg_portEgrBandwidthCtrlRate_get(rtk_rg_mac_port_idx_t port, uint32 *rate)
{
    rtdrv_rg_portEgrBandwidthCtrlRate_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == rate), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.port, &port, sizeof(rtk_rg_mac_port_idx_t));
    osal_memcpy(&cfg.rate, rate, sizeof(uint32));
    GETSOCKOPT(RTDRV_RG_PORTEGRBANDWIDTHCTRLRATE_GET, &cfg, rtdrv_rg_portEgrBandwidthCtrlRate_get_t, 1);
    osal_memcpy(rate, &cfg.rate, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_rg_portEgrBandwidthCtrlRate_get */

/* Function Name:
 *              rtk_rg_portIgrBandwidthCtrlRate_get
 * Description:
 *              Get ingress rate limitation of physical port.
 * Input:
 * Output:
 *              port - [in]<tab>assigned rate limit port
 *              *rate - [out]<tab>assigned rate, unit Kbps
 * Return:
 *              RT_ERR_RG_OK - Success
 *              RT_ERR_RG_NULL_POINTER - input rate pointer is NULL.
 * Note:
 *              None
 */
int32
rtk_rg_portIgrBandwidthCtrlRate_get(rtk_rg_mac_port_idx_t port, uint32 *rate)
{
    rtdrv_rg_portIgrBandwidthCtrlRate_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == rate), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.port, &port, sizeof(rtk_rg_mac_port_idx_t));
    osal_memcpy(&cfg.rate, rate, sizeof(uint32));
    GETSOCKOPT(RTDRV_RG_PORTIGRBANDWIDTHCTRLRATE_GET, &cfg, rtdrv_rg_portIgrBandwidthCtrlRate_get_t, 1);
    osal_memcpy(rate, &cfg.rate, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_rg_portIgrBandwidthCtrlRate_get */

/* Function Name:
 *       rtk_rg_phyPortForceAbility_set
 * Description:
 *       Force Set physical port status & ability.
 * Input:
 * Output:
 *       port - [in]<tab>assigned physical port
 *       ability.force_disable_phy - [in]<tab> ENABLED:force link-down phyPort
 *       ability.valid - [in]<tab> Enable/Disable Force assigned phyPort ability
 *       ability.speed - [in]<tab> Assign linkup speed
 *       ability.duplex - [in]<tab> Assign half duplex or full duplex
 *       ability.flowCtrl - [in]<tab> enable/disable flow control
 * Return:
 *       RT_ERR_RG_OK - Success
 * Note:
 *       None
 */
int32
rtk_rg_phyPortForceAbility_set(rtk_rg_mac_port_idx_t port, rtk_rg_phyPortAbilityInfo_t ability)
{
    rtdrv_rg_phyPortForceAbility_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.port, &port, sizeof(rtk_rg_mac_port_idx_t));
    osal_memcpy(&cfg.ability, &ability, sizeof(rtk_rg_phyPortAbilityInfo_t));
    SETSOCKOPT(RTDRV_RG_PHYPORTFORCEABILITY_SET, &cfg, rtdrv_rg_phyPortForceAbility_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_phyPortForceAbility_set */

 /* Function Name:
  *      rtk_rg_phyPortForceAbility_get
  * Description:
  *      Get physical port status & ability.
  * Input:
  * Output:
  *      port - [in]<tab>assigned physical port
  *      ability.force_disable_phy - [out]<tab> ENABLED:force link-down phyPort
  *      ability.valid - [out]<tab> Enabled/Disabled Force assigned phyPort ability
  *      ability.speed - [out]<tab> Assigned linkup speed
  *      ability.duplex - [out]<tab> Assigned half duplex or full duplex
  *      ability.flowCtrl - [out]<tab> enabled/disabled flow control
  * Return:
  *      RT_ERR_RG_OK - Success
  *      RT_ERR_RG_NULL_POINTER - input ability pointer is NULL.
  * Note:
  *      None
  */
int32
rtk_rg_phyPortForceAbility_get(rtk_rg_mac_port_idx_t port, rtk_rg_phyPortAbilityInfo_t *ability)
{
    rtdrv_rg_phyPortForceAbility_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == ability), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.port, &port, sizeof(rtk_rg_mac_port_idx_t));
    osal_memcpy(&cfg.ability, ability, sizeof(rtk_rg_phyPortAbilityInfo_t));
    GETSOCKOPT(RTDRV_RG_PHYPORTFORCEABILITY_GET, &cfg, rtdrv_rg_phyPortForceAbility_get_t, 1);
    osal_memcpy(ability, &cfg.ability, sizeof(rtk_rg_phyPortAbilityInfo_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_phyPortForceAbility_get */


/* Function Name:
*              rtk_rg_portMibInfo_get
* Description:
*              Get physical port mib data.
* Input:
* Output:
*              port - [in]<tab>assigned physical port
*              *mibInfo - [out]<tab> Enabled/Disabled Force assigned phyPort ability
* Return:
*              RT_ERR_RG_OK - Success
*              RT_ERR_RG_NULL_POINTER - input ability pointer is NULL.
*      RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information.
* Note:
*              mibInfo->ifInOctets - The total number of octets received on the interface<A1>A including framing characters.
*              mibInfo->ifInUcastPkts - The number of packets, delivered by this sub-layer to a higher (sub-)layer, which were not addressed to a multicast or broadcast address at this sub-layer.
*              mibInfo->ifInMulticastPkts - The number of packets<A1>A delivered by this sub-layer to a higher (sub-)layer<A1>A which were addressed to a multicast address at this sub-layer. For a MAC layer protocol<A1>A this includes both Group and Functional addresses.
*              mibInfo->ifInBroadcastPkts - The number of packets<A1>A delivered by this sub-layer to a higher (sub-)layer<A1>A which were addressed to a broadcast address at this sub-layer.
*              mibInfo->ifInDiscards - The number of inbound packets which were chosen to be discarded even though no errors had been detected to prevent their being transmitted. One possible reason for discarding such a packet could be to free up buffer space.
*              mibInfo->ifOutOctets - The total number of octets transmitted out of the interface<A1>A including framing characters.
*              mibInfo->ifOutDiscards - The number of outbound packets which were chosen to be discarded even though no errors had been detected to prevent their being transmitted. One possible reason for discarding such a packet could be to free up buffer space.
*              mibInfo->ifOutUcastPkts - The total number of packets that higher-level protocols requested be transmitted, and which were not addressed to a multicast or broadcast address at this sub-layer, including those that were discarded or not sent.
*              mibInfo->ifOutMulticastPkts - The total number of packets that higher-level protocols requested be transmitted, and which were addressed to a multicast address at this sub-layer, including those that were discarded or not sent.
*              mibInfo->ifOutBrocastPkts - The total number of packets that higher-level protocols requested be transmitted, and which were addressed to a broadcast address at this sub-layer, including those that were discarded or not sent.
*              mibInfo->dot1dBasePortDelayExceededDiscards - The number of transmitted frames that were discarded due to the maximum bridge transit delay being exceeded.
*              mibInfo->dot1dTpPortInDiscards - Count of valid frames received which were discarded (i.e., filtered) by the Forwarding Process.
*              mibInfo->dot1dTpHcPortInDiscards - The total number of Forwarding Database entries<A1>A which have been or would have been learnt<A1>A but have been discarded due to lack of space to store them in the Forwarding Database.
*              mibInfo->dot3InPauseFrames - A count of MAC Control frames received on this interface with an opcode indicating the PAUSE operation.
*              mibInfo->dot3OutPauseFrames - A count of MAC control frames transmitted on this interface with an opcode indicating the PAUSE operation.
*              mibInfo->dot3StatsAligmentErrors - The total number of bits of a received frame is not divisible by eight.
*              mibInfo->dot3StatsFCSErrors - The number of received frame that frame check sequence error.
*              mibInfo->dot3StatsSingleCollisionFrames - A count of frames that are involved in a single collision<A1>A and are subsequently transmitted successfully.
*              mibInfo->dot3StatsMultipleCollisionFrames - A count of frames that are involved in more than one collision and are subsequently transmitted successfully.
*              mibInfo->dot3StatsDeferredTransmissions - A count of frames for which the first transmission attempt on a particular interface is delayed because the medium is busy.
*              mibInfo->dot3StatsLateCollisions - The number of times that a collision is detected on a particular interface later than one slotTime into the transmission of a packet.
*              mibInfo->dot3StatsExcessiveCollisions - A count of frames for which transmission on a particular interface fails due to excessive collisions.
*              mibInfo->dot3StatsFrameTooLongs -  RX_etherStatsOversizePkt -  RX_etherStatsPkts1519toMaxOctets.
*			mibInfo->dot3StatsSymbolErrors - Number of data symbol errors.
* 			 mibInfo->dot3ControlInUnknownOpcodes - A count of MAC Control frames received on this interface that contain an opcode that is not supported by this device.
* 			 mibInfo->etherStatsDropEvents - Number of received packets dropped due to lack of resource.
* 			 mibInfo->etherStatsOctets - ifInOctets(bad+good) + ifOutOctets(bad+good).
* 			 mibInfo->etherStatsBcastPkts - The total number of packets that were directed to the broadcast address. Note that this does not include multicast packets.
* 			 mibInfo->etherStatsMcastPkts - The total number of packets that were directed to a multicast address. Note that this number does not include packets directed to the broadcast address.
* 			 mibInfo->etherStatsUndersizePkts - The total number of packets that were less than 64 octets long (excluding framing bits, but including FCS octets) and were otherwise well formed.
* 			 mibInfo->etherStatsOversizePkts - Number of valid packets whose size are more than 1518 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsFragments - Number of invalid (FCS error / alignment error) packets whose size are less than 64 bytes<A1>A including MAC header and FCS <A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsJabbers - Number of invalid (FCS error / alignment error) packets whose size are more than 1518 bytes<A1>A including MAC header and FCS <A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsCollisions - The best estimate of the total number of collisions on this Ethernet segment.
* 			 mibInfo->etherStatsCRCAlignErrors - The total number of packets that had a length (excluding framing bits, but including FCS octets) of between 64 and 1518 octets, inclusive, but had either a bad Frame Check Sequence (FCS) with an integral number of octets (FCS Error) or a bad FCS with a non-integral number of octets (Alignment Error).
* 			 mibInfo->etherStatsPkts64Octets - Number of all packets whose size are exactly 64 bytes<A1>Aincluding MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsPkts65to127Octets - Number of all packets whose size are between 65 ~ 127 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsPkts128to255Octets - Number of all packets whose size are between 128 ~ 255 bytes<A1>A including MAC header and FCS<A1>A excluding preambl e and SFD.
* 			 mibInfo->etherStatsPkts256to511Octets - Number of all packets whose size are between 256 ~ 511 bytes<A1>A including FCS<A1>A excluding MAC header<A1>A preamble and SFD.
* 			 mibInfo->etherStatsPkts512to1023Octets - Number of all packets whose size are between 512 ~ 1023 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsPkts1024to1518Octets - Number of all packets whose size are between 1024 ~ 1518 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsTxOctets - ifOutOctets.
* 			 mibInfo->etherStatsTxUndersizePkts - The total number of packets transmitted that were less than 64 octets long (excluding framing bits, but including FCS octets) and were otherwise well formed.
* 			 mibInfo->etherStatsTxOversizePkts - Number of transmitted valid packets whose size are more than 1518 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsTxPkts64Octets - Number of all transmitted packets whose size are exactly 64 bytes<A1>Aincluding MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsTxPkts65to127Octets - Number of all transmitted packets whose size are between 65 ~ 127 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsTxPkts128to255Octets - Number of all transmitted packets whose size are between 128 ~ 255 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsTxPkts256to511Octets - Number of all transmitted packets whose size are between 256 ~ 511 bytes<A1>A including FCS<A1>A excluding MAC header<A1>A preamble and SFD.
* 			 mibInfo->etherStatsTxPkts512to1023Octets - Number of all transmitted packets whose size are between 512 ~ 1023 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsTxPkts1024to1518Octets - - Number of all transmitted packets whose size are between 1024 ~ 1518 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsTxPkts1519toMaxOctets - The total number of packets (including bad packets) transmitted that were between 1519 and Max octets in length inclusive (excluding framing bits but including FCS octets).
* 			 mibInfo->etherStatsTxBcastPkts - The total number of good packets transmitted that were directed to the broadcast address. Note that this does not include multicast packets.
* 			 mibInfo->etherStatsTxMcastPkts - The total number of good packets transmitted that were directed to a multicast address. Note that this number does not include packets directed to the broadcast address.
* 			 mibInfo->etherStatsTxFragments - Number of transmitted invalid (FCS error / alignment error) packets whose size are less than 64 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsTxJabbers - Number of transmitted invalid (FCS error / alignment error) packets whose size are more than 1518 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsTxCRCAlignErrors - The total number of packets transmitted that had a length (excluding framing bits, but including FCS octets) of between 64 and 1518 octets, inclusive, but had either a bad Frame Check Sequence (FCS) with an integral number of octets (FCS Error) or a bad FCS with a non-integral number of octets (Alignment Error).
* 			 mibInfo->etherStatsRxUndersizePkts - The total number of packets received that were less than 64 octets long (excluding framing bits, but including FCS octets) and were otherwise well formed.
* 			 mibInfo->etherStatsRxUndersizeDropPkts - The total number of packets received that were less than 64 octets long (excluding framing bits, but including FCS octets) and were otherwise well formed.
* 			 mibInfo->etherStatsRxOversizePkts - Number of received valid packets whose size are more than 1518 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsRxPkts64Octets - Number of all received packets whose size are exactly 64 bytes<A1>Aincluding MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsRxPkts65to127Octets - Number of all received packets whose size are between 65 ~ 127 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsRxPkts128to255Octets - Number of all received packets whose size are between 128 ~ 255 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsRxPkts256to511Octets - Number of all received packets whose size are between 256 ~ 511 bytes<A1>A including FCS<A1>A excluding MAC header<A1>A preamble and SFD.
* 			 mibInfo->etherStatsRxPkts512to1023Octets - Number of all received packets whose size are between 512 ~ 1023 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsRxPkts1024to1518Octets - Number of all received packets whose size are between 1024 ~ 1518 bytes<A1>A including MAC header and FCS<A1>A excluding preamble and SFD.
* 			 mibInfo->etherStatsRxPkts1519toMaxOctets - The total number of packets (including bad packets) received that were between 1519 and Max octets in length inclusive (excluding framing bits but including FCS octets).
* 			 mibInfo->inOampduPkts - Number of received OAMPDUs.
* 			 mibInfo->outOampduPkts - A count of OAMPDUs transmitted on this interface.
*/
 int32
 rtk_rg_portMibInfo_get(rtk_rg_mac_port_idx_t port, rtk_rg_port_mib_info_t *mibInfo)
 {
	 rtdrv_rg_portMibInfo_get_t cfg;
 
	 /* parameter check */
	 RT_PARAM_CHK((NULL == mibInfo), RT_ERR_NULL_POINTER);
 
	 /* function body */
	 osal_memcpy(&cfg.port, &port, sizeof(rtk_rg_mac_port_idx_t));
	 osal_memcpy(&cfg.mibInfo, mibInfo, sizeof(rtk_rg_port_mib_info_t));
	 GETSOCKOPT(RTDRV_RG_PORTMIBINFO_GET, &cfg, rtdrv_rg_portMibInfo_get_t, 1);
	 osal_memcpy(mibInfo, &cfg.mibInfo, sizeof(rtk_rg_port_mib_info_t));
 
	 return RT_ERR_OK;
 }	 /* end of rtk_rg_portMibInfo_get */
 
/* Function Name:
* 			 rtk_rg_portMibInfo_clear
* Description:
* 			 Clear physical port mib data.
* Input:
* Output:
* 			 port - [in]<tab>assigned physical port
* Return:
* 			 RT_ERR_RG_OK - Success
* 	 RT_ERR_RG_INVALID_PARAM - the input parameter contains illegal information.
* Note:
* 			 None
*/
int32
rtk_rg_portMibInfo_clear(rtk_rg_mac_port_idx_t port)
{
	 rtdrv_rg_portMibInfo_clear_t cfg;
 
	 /* function body */
	 osal_memcpy(&cfg.port, &port, sizeof(rtk_rg_mac_port_idx_t));
	 SETSOCKOPT(RTDRV_RG_PORTMIBINFO_CLEAR, &cfg, rtdrv_rg_portMibInfo_clear_t, 1);
 
	 return RT_ERR_OK;
}	 /* end of rtk_rg_portMibInfo_clear */
 
/* Function Name:
* 	 rtk_rg_stormControl_add
* Description:
* 	Add stormControl.
* Input:
* Output:
* 	 stormInfo.valid - [in]<tab>enable stormCtrol
* 	 astormInfo.port  - [in]<tab> assigned port
* 	 stormInfo.stormType - [in]<tab> assigned stormType
* 	 stormInfo.meterIdx - [in]<tab> assigned rate by shareMeter
* Return:
* 	 RT_ERR_RG_OK - Success<nl>
* 	 RT_ERR_RG_NULL_POINTER - parameter is null. <nl>
* 	 RT_ERR_RG_INVALID_PARAM - parameters value is out of range.<nl>
* 	 RT_ERR_RG_STORMCONTROL_ENTRY_HAS_BEEN_SET - same stormInfo has been set before .<nl>
* 	 RT_ERR_RG_STORMCONTROL_TYPE_FULL - add stormType full(ASIC support at most 4 different types)<nl>
* Note:
* 			  None
*/
int32
rtk_rg_stormControl_add(rtk_rg_stormControlInfo_t *stormInfo,int *stormInfo_idx)
{
	 rtdrv_rg_stormControl_add_t cfg;
 
	 /* parameter check */
	 RT_PARAM_CHK((NULL == stormInfo), RT_ERR_NULL_POINTER);
	 RT_PARAM_CHK((NULL == stormInfo_idx), RT_ERR_NULL_POINTER);
 
	 /* function body */
	 osal_memcpy(&cfg.stormInfo, stormInfo, sizeof(rtk_rg_stormControlInfo_t));
	 osal_memcpy(&cfg.stormInfo_idx, stormInfo_idx, sizeof(int));
	 GETSOCKOPT(RTDRV_RG_STORMCONTROL_ADD, &cfg, rtdrv_rg_stormControl_add_t, 1);
	 osal_memcpy(stormInfo, &cfg.stormInfo, sizeof(rtk_rg_stormControlInfo_t));
	 osal_memcpy(stormInfo_idx, &cfg.stormInfo_idx, sizeof(int));
 
	 return RT_ERR_OK;
}	 /* end of rtk_rg_stormControl_add */

/* Function Name:
 *      rtk_rg_stormControl_del
 * Description:
 *     Delete stormControl.
 * Input:
 * Output:
 *      stormInfo_idx - [out]<tab>The index of stormInfo which should be delete.
 * Return:
 *      RT_ERR_RG_OK - Success<nl>
 *      RT_ERR_RG_INVALID_PARAM - parameters value is out of range.<nl>
 * Note:
 *               None
 */
int32
rtk_rg_stormControl_del(int stormInfo_idx)
{
    rtdrv_rg_stormControl_del_t cfg;

    /* function body */
    osal_memcpy(&cfg.stormInfo_idx, &stormInfo_idx, sizeof(int));
    SETSOCKOPT(RTDRV_RG_STORMCONTROL_DEL, &cfg, rtdrv_rg_stormControl_del_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_stormControl_del */

/* Function Name:
 *      rtk_rg_stormControl_find
 * Description:
 *     Find stormInfo index.
 * Input:
 * Output:
 *      astormInfo.port  - [in]<tab> assigned compare port
 *      stormInfo.stormType - [in]<tab> assigned compare stormType
 *      stormInfo.valid - [out]<tab>enable stormCtrol
 *      stormInfo.meterIdx - [out]<tab> assigned shareMeter index
 * Return:
 *      RT_ERR_RG_OK - Success<nl>
 *      RT_ERR_RG_NULL_POINTER - parameter is null. <nl>
 *      RT_ERR_RG_INVALID_PARAM - parameters value is out of range. <nl>
 *      RT_ERR_RG_STORMCONTROL_ENTRY_NOT_FOUND - assigned stormInfo not found<nl>
 * Note:
 *               None
 */
int32
rtk_rg_stormControl_find(rtk_rg_stormControlInfo_t *stormInfo,int *stormInfo_idx)
{
    rtdrv_rg_stormControl_find_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == stormInfo), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == stormInfo_idx), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.stormInfo, stormInfo, sizeof(rtk_rg_stormControlInfo_t));
    osal_memcpy(&cfg.stormInfo_idx, stormInfo_idx, sizeof(int));
    GETSOCKOPT(RTDRV_RG_STORMCONTROL_FIND, &cfg, rtdrv_rg_stormControl_find_t, 1);
    osal_memcpy(stormInfo, &cfg.stormInfo, sizeof(rtk_rg_stormControlInfo_t));
    osal_memcpy(stormInfo_idx, &cfg.stormInfo_idx, sizeof(int));

    return RT_ERR_OK;
}   /* end of rtk_rg_stormControl_find */

/* Function Name:
 *      rtk_rg_shareMeter_set
 * Description:
 *     Set shareMeter rate.
 * Input:
 * Output:
 *      index - [in]<tab>shared meter index
 *      rate  - [in]<tab> rate of share meter
 *      ifgInclude - [in]<tab> include IFG or not, ENABLE:include DISABLE:exclude
 * Return:
 *      RT_ERR_RG_OK - Success<nl>
 * Note:
 *      The API can set shared meter rate and ifg include for each meter.
 *      The rate unit is 1 kbps and the range is from 8k to 1048568k.
 *      The granularity of rate is 8 kbps. The ifg_include parameter is used
 *      for rate calculation with/without inter-frame-gap and preamble.
 */
int32
rtk_rg_shareMeter_set(uint32 index, uint32 rate, rtk_rg_enable_t ifgInclude)
{
    rtdrv_rg_shareMeter_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.index, &index, sizeof(uint32));
    osal_memcpy(&cfg.rate, &rate, sizeof(uint32));
    osal_memcpy(&cfg.ifgInclude, &ifgInclude, sizeof(rtk_rg_enable_t));
    SETSOCKOPT(RTDRV_RG_SHAREMETER_SET, &cfg, rtdrv_rg_shareMeter_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_shareMeter_set */

/* Function Name:
 *      rtk_rg_shareMeter_get
 * Description:
 *     Set shareMeter rate.
 * Input:
 * Output:
 *      index - [in]<tab>shared meter index
 *      rate  - [out]<tab> pointer of rate of share meter
 *      ifgInclude - [out]<tab>include IFG or not, ENABLE:include DISABLE:exclude
 * Return:
 *      RT_ERR_RG_OK - Success<nl>
 * Note:
 *              The API can get shared meter rate and ifg include for each meter.
 *              The rate unit is 1 kbps and the granularity of rate is 8 kbps.
 *              The ifg_include parameter is used for rate calculation with/without inter-frame-gap and preamble
 */
int32
rtk_rg_shareMeter_get(uint32 index, uint32 *pRate , rtk_rg_enable_t *pIfgInclude)
{
    rtdrv_rg_shareMeter_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIfgInclude), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.index, &index, sizeof(uint32));
    osal_memcpy(&cfg.pRate, pRate, sizeof(uint32));
    osal_memcpy(&cfg.pIfgInclude, pIfgInclude, sizeof(rtk_rg_enable_t));
    GETSOCKOPT(RTDRV_RG_SHAREMETER_GET, &cfg, rtdrv_rg_shareMeter_get_t, 1);
    osal_memcpy(pRate, &cfg.pRate, sizeof(uint32));
    osal_memcpy(pIfgInclude, &cfg.pIfgInclude, sizeof(rtk_rg_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_shareMeter_get */

/* Function Name:
 *      rtk_rg_qosPriMap_set
 * Description:
 *      Set the entry of internal priority to QID mapping table.
 * Input:
 * Output:
 *      pri2qid  - [in]array of internal priority on a queue
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Below is an example of internal priority to QID mapping table.
 *      -
 *      -
 *      - Priority   0   1   2   3   4   5   6   7
 *      -              ====================
 *      - queue     0   0   1   1   2   2   3   3
 *      -
 *      -for table index 0
 *      -    pPri2qid[0] = 0   internal priority 0 map to queue 0
 *      -    pPri2qid[1] = 0   internal priority 1 map to queue 0
 *      -    pPri2qid[2] = 1   internal priority 2 map to queue 1
 *      -    pPri2qid[3] = 1   internal priority 3 map to queue 1
 *      -    pPri2qid[4] = 2   internal priority 4 map to queue 2
 *      -    pPri2qid[5] = 2   internal priority 5 map to queue 2
 *      -    pPri2qid[6] = 3   internal priority 6 map to queue 3
 *      -    pPri2qid[7] = 3   internal priority 7 map to queue 3
 */
int32
rtk_rg_qosPriMap_set(rtk_rg_qos_pri2queue_t pri2qid)
{
    rtdrv_rg_qosPriMap_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.pri2qid, &pri2qid, sizeof(rtk_rg_qos_pri2queue_t));
    SETSOCKOPT(RTDRV_RG_QOSPRIMAP_SET, &cfg, rtdrv_rg_qosPriMap_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_qosPriMap_set */

/* Function Name:
 *      rtk_rg_qosPriMap_get
 * Description:
 *      Get the entry of internal priority to QID mapping table.
 * Input:
 * Output:
 *      pPri2qid  - [out]array of internal priority on a queue
 * Return:
 *      RT_ERR_RG_OK
 * Note:
 *              None
 */
int32
rtk_rg_qosPriMap_get(rtk_rg_qos_pri2queue_t *pPri2qid)
{
    rtdrv_rg_qosPriMap_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPri2qid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.pPri2qid, pPri2qid, sizeof(rtk_rg_qos_pri2queue_t));
    GETSOCKOPT(RTDRV_RG_QOSPRIMAP_GET, &cfg, rtdrv_rg_qosPriMap_get_t, 1);
    osal_memcpy(pPri2qid, &cfg.pPri2qid, sizeof(rtk_rg_qos_pri2queue_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_qosPriMap_get */

/* Function Name:
 *      rtk_rg_qosPriSelGroup_set
 * Description:
 *      Set weight of each priority assignment.
 * Input:
 * Output:
 *              weightOfPriSel - [in]weight of each priority assignment
 * Return:
 *      RT_ERR_RG_OK
 * Note:
 *      None
 */
int32
rtk_rg_qosPriSelGroup_set(rtk_rg_qos_priSelWeight_t weightOfPriSel)
{
    rtdrv_rg_qosPriSelGroup_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.weightOfPriSel, &weightOfPriSel, sizeof(rtk_rg_qos_priSelWeight_t));
    SETSOCKOPT(RTDRV_RG_QOSPRISELGROUP_SET, &cfg, rtdrv_rg_qosPriSelGroup_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_qosPriSelGroup_set */

/* Function Name:
 *      rtk_rg_qosPriSelGroup_get
 * Description:
 *      Get weight of each priority assignment.
 * Input:
 * Output:
 *        pWeightOfPriSel - [out]weight of each priority assignment
 * Return:
 *      RT_ERR_RG_OK
 * Note:
 *      None
 */
int32
rtk_rg_qosPriSelGroup_get(rtk_rg_qos_priSelWeight_t *pWeightOfPriSel)
{
    rtdrv_rg_qosPriSelGroup_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pWeightOfPriSel), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.pWeightOfPriSel, pWeightOfPriSel, sizeof(rtk_rg_qos_priSelWeight_t));
    GETSOCKOPT(RTDRV_RG_QOSPRISELGROUP_GET, &cfg, rtdrv_rg_qosPriSelGroup_get_t, 1);
    osal_memcpy(pWeightOfPriSel, &cfg.pWeightOfPriSel, sizeof(rtk_rg_qos_priSelWeight_t));

    return RT_ERR_OK;
}   /* end of rtk_rg_qosPriSelGroup_get */

/* Function Name:
 *      rtk_rg_qosDscpPriRemapGroup_set
 * Description:
 *      Set remapped internal priority of DSCP.
 * Input:
 * Output:
 *      dscp  - [in]DSCP
 *      intPri - [in]internal priority
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_INVALID_PARAM
 * Note:
 *      Apollo only support group 0
 */
int32
rtk_rg_qosDscpPriRemapGroup_set(uint32 dscp,uint32 intPri)
{
    rtdrv_rg_qosDscpPriRemapGroup_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.dscp, &dscp, sizeof(uint32));
    osal_memcpy(&cfg.intPri, &intPri, sizeof(uint32));
    SETSOCKOPT(RTDRV_RG_QOSDSCPPRIREMAPGROUP_SET, &cfg, rtdrv_rg_qosDscpPriRemapGroup_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_qosDscpPriRemapGroup_set */

/* Function Name:
 *      rtk_rg_qosDscpPriRemapGroup_get
 * Description:
 *      Get remapped internal priority of DSCP.
 * Input:
 * Output:
 *      dscp  - [in]DSCP
 *      pIntPri - [out]internal priority
 * Return:
 *      RT_ERR_RG_OK
 * Note:
 *      Apollo only support group 0
 */
int32
rtk_rg_qosDscpPriRemapGroup_get(uint32 dscp,uint32 *pIntPri)
{
    rtdrv_rg_qosDscpPriRemapGroup_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntPri), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.dscp, &dscp, sizeof(uint32));
    osal_memcpy(&cfg.pIntPri, pIntPri, sizeof(uint32));
    GETSOCKOPT(RTDRV_RG_QOSDSCPPRIREMAPGROUP_GET, &cfg, rtdrv_rg_qosDscpPriRemapGroup_get_t, 1);
    osal_memcpy(pIntPri, &cfg.pIntPri, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_rg_qosDscpPriRemapGroup_get */

/* Function Name:
 *      rtk_rg_qosPortPri_set
 * Description:
 *      Set internal priority of one port.
 * Input:
 * Output:
 *      port     - [in]port id
 *      intPri  - [in]Priorities assigment for specific port.
 * Return:
 *      RT_ERR_RG_OK
 *      RT_ERR_RG_INVALID_PARAM
 * Note:
 *    None
 */
int32
rtk_rg_qosPortPri_set(rtk_rg_mac_port_idx_t port,uint32 intPri)
{
    rtdrv_rg_qosPortPri_set_t cfg;

    /* function body */
    osal_memcpy(&cfg.port, &port, sizeof(rtk_rg_mac_port_idx_t));
    osal_memcpy(&cfg.intPri, &intPri, sizeof(uint32));
    SETSOCKOPT(RTDRV_RG_QOSPORTPRI_SET, &cfg, rtdrv_rg_qosPortPri_set_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_rg_qosPortPri_set */

/* Function Name:
 *      rtk_rg_qosPortPri_get
 * Description:
 *      Get internal priority of one port.
 * Input:
 * Output:
 *      port     - [in]port id
 *      pIntPri  - [out]Priorities assigment for specific port.
 * Return:
 *      RT_ERR_RG_OK
 * Note:
 *    None
 */
int32
rtk_rg_qosPortPri_get(rtk_rg_mac_port_idx_t port,uint32 *pIntPri)
{
    rtdrv_rg_qosPortPri_get_t cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntPri), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.port, &port, sizeof(rtk_rg_mac_port_idx_t));
    osal_memcpy(&cfg.pIntPri, pIntPri, sizeof(uint32));
    GETSOCKOPT(RTDRV_RG_QOSPORTPRI_GET, &cfg, rtdrv_rg_qosPortPri_get_t, 1);
    osal_memcpy(pIntPri, &cfg.pIntPri, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_rg_qosPortPri_get */

