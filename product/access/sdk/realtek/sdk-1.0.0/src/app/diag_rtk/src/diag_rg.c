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
 * $Revision: 41237 $
 * $Date: 2013-07-17 15:13:59 +0800 (Wed, 17 Jul 2013) $
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
#include <stdlib.h>

#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <hal/common/halctrl.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>

#include <rtk_rg_struct.h>




/*varibles*/
rtk_rg_VersionString_t version_string;
rtk_rg_intfInfo_t intf_info;
rtk_rg_lanIntfConf_t lan_intf;
rtk_rg_wanIntfConf_t wan_intf;
rtk_rg_ipStaticInfo_t wan_intf_static_info;
rtk_rg_naptEntry_t naptFlow;
rtk_rg_macEntry_t macEntry;
rtk_rg_arpEntry_t arpEntry;
rtk_rg_arpInfo_t arpInfo;
rtk_rg_urlFilterString_t urlFilter;
rtk_rg_initParams_t init_param;
rtk_rg_alg_type_t alg_app;
rtk_rg_pppoeClientInfoAfterDial_t clientPppoe_info;
rtk_rg_pppoeClientInfoBeforeDial_t clientPppoe_beforeDiag_info;
rtk_rg_neighborEntry_t neighborEntry;
rtk_rg_neighborInfo_t neighborInfo;
rtk_rg_ipDhcpClientInfo_t dhcpClient_info;
rtk_rg_vlanBinding_t vlan_binding_info;
rtk_rg_macFilterEntry_t macFilterEntry;
rtk_rg_alg_serverIpMapping_t srvIpMapping;
rtk_rg_virtualServer_t virtual_server;
rtk_rg_cvlan_info_t cvlan_info;
rtk_rg_saLearningLimitInfo_t sa_learnLimit_info;
rtk_rg_phyPortAbilityInfo_t phyPort_ability;
rtk_rg_portMirrorInfo_t portMirrorInfo;
rtk_rg_upnpConnection_t upnpInfo;
rtk_rg_dmzInfo_t dmz_info;
rtk_rg_stormControlInfo_t stormInfo;
rtk_rg_qos_queue_weights_t q_weight;
rtk_rg_qos_pri2queue_t pri2qid;
rtk_rg_qos_priSelWeight_t weightOfPriSel;
rtk_rg_multicastFlow_t mcFlow;









/*internal function*/

void _diag_lanIntfShow(rtk_rg_lanIntfConf_t *lan_intf){
	diag_util_mprintf("ip_version: %d \n",lan_intf->ip_version);
	diag_util_mprintf("gateway-mac: %02X:%02X:%02X:%02X:%02X:%02X \n",
		lan_intf->gmac.octet[0],
		lan_intf->gmac.octet[1],
		lan_intf->gmac.octet[2],
		lan_intf->gmac.octet[3],
		lan_intf->gmac.octet[4],
		lan_intf->gmac.octet[5]);
	diag_util_mprintf("ip-addr: %s \n",diag_util_inet_ntoa(lan_intf->ip_addr));
	diag_util_mprintf("ip-mask: %s \n",diag_util_inet_ntoa(lan_intf->ip_network_mask));
	diag_util_mprintf("ipv6_addr: %s\n",diag_util_inet_n6toa( &lan_intf->ipv6_addr.ipv6_addr[0]));
	diag_util_mprintf("ipv6_network_mask_length: %d \n",lan_intf->ipv6_network_mask_length);
	diag_util_mprintf("port-mask: 0x%x \n",lan_intf->port_mask);
	diag_util_mprintf("untag_mask: 0x%x \n",lan_intf->untag_mask);
	diag_util_mprintf("intf-vlan_id: %d \n",lan_intf->intf_vlan_id);
	diag_util_mprintf("mtu: %d \n",lan_intf->mtu);
	diag_util_mprintf("isIVL: %d \n",lan_intf->isIVL);
}

void _diag_wanIntfShow(rtk_rg_wanIntfConf_t *wan_intf){
	diag_util_mprintf("wan_type: %d \n",wan_intf->wan_type);	
	diag_util_mprintf("gateway-mac: %02X:%02X:%02X:%02X:%02X:%02X \n",
		wan_intf->gmac.octet[0],
		wan_intf->gmac.octet[1],
		wan_intf->gmac.octet[2],
		wan_intf->gmac.octet[3],
		wan_intf->gmac.octet[4],
		wan_intf->gmac.octet[5]);
	diag_util_mprintf("wan-port: %d \n",wan_intf->wan_port_idx);
	diag_util_mprintf("port-binding-mask: 0x%x \n",wan_intf->port_binding_mask.portmask);
	diag_util_mprintf("egress-vlan-tag-on: %d \n",wan_intf->egress_vlan_tag_on);
	diag_util_mprintf("egress-vlan-id: %d \n",wan_intf->egress_vlan_id);
	diag_util_mprintf("egress-vlan-pri: %d \n",wan_intf->egress_vlan_pri);
	diag_util_mprintf("isIVL: %d \n",wan_intf->isIVL);
	//diag_util_mprintf("default-gateway-on: %d \n",wan_intf->default_gateway_on);
}

void _diag_wanIntfStaticInfoShow(rtk_rg_ipStaticInfo_t *wan_intf_static_info){
	diag_util_mprintf("ip_version: %d \n",wan_intf_static_info->ip_version);
	diag_util_mprintf("napt_enable: %d \n",wan_intf_static_info->napt_enable);
	diag_util_mprintf("ip-addr: %s \n",diag_util_inet_ntoa(wan_intf_static_info->ip_addr));
	diag_util_mprintf("ip_network_mask: %s \n",diag_util_inet_ntoa(wan_intf_static_info->ip_network_mask));
	diag_util_mprintf("ipv4_default_gateway_on: %d \n",wan_intf_static_info->ipv4_default_gateway_on);
	diag_util_mprintf("gateway_ipv4_addr: %s \n",diag_util_inet_ntoa(wan_intf_static_info->gateway_ipv4_addr));
	diag_util_mprintf("ipv6_addr: %s\n",diag_util_inet_n6toa( &wan_intf_static_info->ipv6_addr.ipv6_addr[0]));
	diag_util_mprintf("ipv6_mask_length: %d \n",wan_intf_static_info->ipv6_mask_length);
	diag_util_mprintf("ipv6_default_gateway_on: %d \n",wan_intf_static_info->ipv6_default_gateway_on);
	diag_util_mprintf("gateway_ipv6_addr: %s\n",diag_util_inet_n6toa( &wan_intf_static_info->gateway_ipv6_addr.ipv6_addr[0]));
	diag_util_mprintf("mtu: %d \n",wan_intf_static_info->mtu);
	
	int gw_mac_auto_learn_for_ipv4;
	int gw_mac_auto_learn_for_ipv6;
	rtk_mac_t gateway_mac_addr_for_ipv4;
	rtk_mac_t gateway_mac_addr_for_ipv6;
	diag_util_mprintf("gw_mac_auto_learn_for_ipv4: %d \n",wan_intf_static_info->gw_mac_auto_learn_for_ipv4);
		diag_util_mprintf("gateway_mac_addr_for_ipv4: %02X:%02X:%02X:%02X:%02X:%02X \n",
		wan_intf_static_info->gateway_mac_addr_for_ipv4.octet[0],
		wan_intf_static_info->gateway_mac_addr_for_ipv4.octet[1],
		wan_intf_static_info->gateway_mac_addr_for_ipv4.octet[2],
		wan_intf_static_info->gateway_mac_addr_for_ipv4.octet[3],
		wan_intf_static_info->gateway_mac_addr_for_ipv4.octet[4],
		wan_intf_static_info->gateway_mac_addr_for_ipv4.octet[5]);
	
	diag_util_mprintf("gateway_mac_addr_for_ipv6: %d \n",wan_intf_static_info->gateway_mac_addr_for_ipv6);
	diag_util_mprintf("gateway_mac_addr_for_ipv6: %02X:%02X:%02X:%02X:%02X:%02X \n",
		wan_intf_static_info->gateway_mac_addr_for_ipv6.octet[0],
		wan_intf_static_info->gateway_mac_addr_for_ipv6.octet[1],
		wan_intf_static_info->gateway_mac_addr_for_ipv6.octet[2],
		wan_intf_static_info->gateway_mac_addr_for_ipv6.octet[3],
		wan_intf_static_info->gateway_mac_addr_for_ipv6.octet[4],
		wan_intf_static_info->gateway_mac_addr_for_ipv6.octet[5]);
}

void _diag_macEntryShow(rtk_rg_macEntry_t *macEntry){
	diag_util_mprintf("mac: %02X:%02X:%02X:%02X:%02X:%02X \n",
		macEntry->mac.octet[0],
		macEntry->mac.octet[1],
		macEntry->mac.octet[2],
		macEntry->mac.octet[3],
		macEntry->mac.octet[4],
		macEntry->mac.octet[5]);
	diag_util_mprintf("isIVL: %d \n",macEntry->isIVL);
	diag_util_mprintf("fid: %d \n",macEntry->fid);
	diag_util_mprintf("vlan_id: %d \n",macEntry->vlan_id);
	diag_util_mprintf("port_idx: %d \n",macEntry->port_idx);
	diag_util_mprintf("static_entry: %d \n",macEntry->static_entry);
}

void _diag_arpEntryShow(rtk_rg_arpEntry_t *arpEntry){
	diag_util_mprintf("macEntryIdx: %d \n",arpEntry->macEntryIdx);
	diag_util_mprintf("ip: %s \n",diag_util_inet_ntoa(arpEntry->ipv4Addr));
	diag_util_mprintf("static_entry: %d \n",arpEntry->staticEntry);
}

void _diag_naptConnShow(rtk_rg_naptEntry_t *naptFlow){
	diag_util_mprintf("is_tcp: %d \n",naptFlow->is_tcp);
	diag_util_mprintf("local_ip: %s \n",diag_util_inet_ntoa(naptFlow->local_ip));
	diag_util_mprintf("remote_ip: %s \n",diag_util_inet_ntoa(naptFlow->remote_ip));
	diag_util_mprintf("wan_intf_idx: %d \n",naptFlow->wan_intf_idx);
	diag_util_mprintf("local_port: %d \n",naptFlow->local_port);
	diag_util_mprintf("remote_port: %d \n",naptFlow->remote_port);
	diag_util_mprintf("external_port: %d \n",naptFlow->external_port);
	diag_util_mprintf("outbound_pri_valid: %d \n",naptFlow->outbound_pri_valid);
	diag_util_mprintf("outbound_priority: %d \n",naptFlow->outbound_priority);
	diag_util_mprintf("inbound_pri_valid: %d \n",naptFlow->inbound_pri_valid);
	diag_util_mprintf("inbound_priority: %d \n",naptFlow->inbound_priority);
}

void _diag_urlFilterShow(rtk_rg_urlFilterString_t* urlFilter){
	diag_util_mprintf("url_filter_string: %s \n",urlFilter->url_filter_string);
	diag_util_mprintf("path_filter_string: %s \n",urlFilter->path_filter_string);
	diag_util_mprintf("path_exactly_match: %d \n",urlFilter->path_exactly_match);
	diag_util_mprintf("wan_intf: %d \n",urlFilter->wan_intf);
}

void _diag_callbackShow(rtk_rg_initParams_t* init_param){
	diag_util_mprintf("initByHwCallBack: 0x%x \n",init_param->initByHwCallBack);
	diag_util_mprintf("arpAddByHwCallBack: 0x%x \n",init_param->arpAddByHwCallBack);
	diag_util_mprintf("arpDelByHwCallBack: 0x%x \n",init_param->arpDelByHwCallBack);
	diag_util_mprintf("macAddByHwCallBack: 0x%x \n",init_param->macAddByHwCallBack);
	diag_util_mprintf("macDelByHwCallBack: 0x%x \n",init_param->macDelByHwCallBack);
	diag_util_mprintf("routingAddByHwCallBack: 0x%x \n",init_param->routingAddByHwCallBack);
	diag_util_mprintf("routingDelByHwCallBack: 0x%x \n",init_param->routingDelByHwCallBack);
	diag_util_mprintf("naptAddByHwCallBack: 0x%x \n",init_param->naptAddByHwCallBack);
	diag_util_mprintf("naptDelByHwCallBack: 0x%x \n",init_param->naptDelByHwCallBack);
	diag_util_mprintf("bindingAddByHwCallBack: 0x%x \n",init_param->bindingAddByHwCallBack);
	diag_util_mprintf("bindingDelByHwCallBack: 0x%x \n",init_param->bindingDelByHwCallBack);
	diag_util_mprintf("interfaceAddByHwCallBack: 0x%x \n",init_param->interfaceAddByHwCallBack);
	diag_util_mprintf("interfaceDelByHwCallBack: 0x%x \n",init_param->interfaceDelByHwCallBack);
	diag_util_mprintf("neighborAddByHwCallBack: 0x%x \n",init_param->neighborAddByHwCallBack);
	diag_util_mprintf("neighborDelByHwCallBack: 0x%x \n",init_param->neighborDelByHwCallBack);
	diag_util_mprintf("v6RoutingAddByHwCallBack: 0x%x \n",init_param->v6RoutingAddByHwCallBack);
	diag_util_mprintf("v6RoutingDelByHwCallBack: 0x%x \n",init_param->v6RoutingDelByHwCallBack);
	diag_util_mprintf("pppoeBeforeDiagByHwCallBack: 0x%x \n",init_param->pppoeBeforeDiagByHwCallBack);	

}
 
void _diag_algAppShow(rtk_rg_alg_type_t* alg_app){

}

void _diag_pppoeClientInfoShow(rtk_rg_pppoeClientInfoAfterDial_t* clientPppoe_info){
	diag_util_mprintf("SessionID: %d \n",clientPppoe_info->sessionId);
	_diag_wanIntfStaticInfoShow(&clientPppoe_info->hw_info);

}

void _diag_dhcpClientInfoShow(rtk_rg_ipDhcpClientInfo_t* dhcpClient_info){
	diag_util_mprintf("stauts: %d \n",dhcpClient_info->stauts);
	_diag_wanIntfStaticInfoShow(&dhcpClient_info->hw_info);
}

void _diag_pppoeClientInfoBeforeDiagShow(rtk_rg_pppoeClientInfoBeforeDial_t* clientPppoe_beforeDiag_info){
	diag_util_mprintf("username: %s \n",clientPppoe_beforeDiag_info->username);
	diag_util_mprintf("password: %s \n",clientPppoe_beforeDiag_info->password);
	diag_util_mprintf("auth_type: %d \n",clientPppoe_beforeDiag_info->auth_type);
	diag_util_mprintf("pppoe_proxy_enable: %d \n",clientPppoe_beforeDiag_info->pppoe_proxy_enable);
	diag_util_mprintf("max_pppoe_proxy_num: %d \n",clientPppoe_beforeDiag_info->max_pppoe_proxy_num);
	diag_util_mprintf("auto_reconnect: %d \n",clientPppoe_beforeDiag_info->auto_reconnect);
	diag_util_mprintf("dial_on_demond: %d \n",clientPppoe_beforeDiag_info->dial_on_demond);
	diag_util_mprintf("idle_timeout_secs: %d \n",clientPppoe_beforeDiag_info->idle_timeout_secs);
	diag_util_mprintf("stauts: %s \n",clientPppoe_beforeDiag_info->stauts);
	diag_util_mprintf("dialOnDemondCallBack: %p \n",clientPppoe_beforeDiag_info->dialOnDemondCallBack);
	diag_util_mprintf("idleTimeOutCallBack: %p \n",clientPppoe_beforeDiag_info->idleTimeOutCallBack);
			
}
 
void _diag_neighborEntryShow(rtk_rg_neighborEntry_t* neighborEntry){
	//wait for struct sure!
	diag_util_mprintf("l2Idx: %d \n",neighborEntry->l2Idx);
	diag_util_mprintf("matchRouteIdx: %d \n",neighborEntry->matchRouteIdx);
	diag_util_mprintf("interfaceId: %02x%02x%02x%02x:%02x%02x%02x%02x\n",
		neighborEntry->interfaceId[0],
		neighborEntry->interfaceId[1],
		neighborEntry->interfaceId[2],
		neighborEntry->interfaceId[3],
		neighborEntry->interfaceId[4],
		neighborEntry->interfaceId[5],
		neighborEntry->interfaceId[6],
		neighborEntry->interfaceId[7]);
	diag_util_mprintf("valid: %d \n",neighborEntry->valid);
	diag_util_mprintf("staticEntry: %d \n",neighborEntry->staticEntry);

}

void _diag_bindingShow(rtk_rg_vlanBinding_t *vlan_binding_info){
	diag_util_mprintf("port_idx: %d \n",vlan_binding_info->port_idx);
	diag_util_mprintf("ingress_vid: %d \n",vlan_binding_info->ingress_vid);
	diag_util_mprintf("wan_intf_idx: %d \n",vlan_binding_info->wan_intf_idx);
}

void _diag_macFilterEntryShow(rtk_rg_macFilterEntry_t *macFilterEntry){
	diag_util_mprintf("mac: %02X:%02X:%02X:%02X:%02X:%02X \n",
	macFilterEntry->mac.octet[0],
	macFilterEntry->mac.octet[1],
	macFilterEntry->mac.octet[2],
	macFilterEntry->mac.octet[3],
	macFilterEntry->mac.octet[4],
	macFilterEntry->mac.octet[5]);
	diag_util_mprintf("direct: %d \n",macFilterEntry->direct);
}

void _diag_virtualServerShow(rtk_rg_virtualServer_t* virtual_server){
	diag_util_mprintf("is_tcp: %d \n",virtual_server->is_tcp);
	diag_util_mprintf("wan_intf_idx: %d \n",virtual_server->wan_intf_idx);
	diag_util_mprintf("gateway_port_start: %d \n",virtual_server->gateway_port_start);
	diag_util_mprintf("local_ip: 0x%x \n",virtual_server->local_ip);
	diag_util_mprintf("local_port_start: %d \n",virtual_server->local_port_start);
	diag_util_mprintf("mappingPortRangeCnt: %d \n",virtual_server->mappingPortRangeCnt);
	diag_util_mprintf("valid: %d \n",virtual_server->valid);
}
void _diag_cvlanInfoShow(rtk_rg_cvlan_info_t* cvlan_info){
	diag_util_mprintf("vlanId: %d \n",cvlan_info->vlanId);
	diag_util_mprintf("isIVL: %d \n",cvlan_info->isIVL);
	diag_util_mprintf("memberPortMask: 0x%x \n",cvlan_info->memberPortMask);
	diag_util_mprintf("untagPortMask: 0x%x \n",cvlan_info->untagPortMask);
	diag_util_mprintf("priority: %d \n",cvlan_info->priority);
}

void _diag_upnpInfoShow(rtk_rg_upnpConnection_t* upnpInfo){
	diag_util_mprintf("is_tcp: %d \n",upnpInfo->is_tcp);
	diag_util_mprintf("valid: %d \n",upnpInfo->valid);
	diag_util_mprintf("wan_intf_idx: %d \n",upnpInfo->wan_intf_idx);
	diag_util_mprintf("gateway_port: %d \n",upnpInfo->gateway_port);
	diag_util_mprintf("local_ip: 0x%x \n",upnpInfo->local_ip);
	diag_util_mprintf("local_port: %d \n",upnpInfo->local_port);
	diag_util_mprintf("limit_remote_ip: %d \n",upnpInfo->limit_remote_ip);
	diag_util_mprintf("limit_remote_port: %d \n",upnpInfo->limit_remote_port);
	diag_util_mprintf("remote_ip: 0x%x \n",upnpInfo->remote_ip);
	diag_util_mprintf("remote_port: %d \n",upnpInfo->remote_port);
	diag_util_mprintf("type: %d \n",upnpInfo->type);
	diag_util_mprintf("timeout: %d \n",upnpInfo->timeout);
}
void _diag_stormControlInfoShow(rtk_rg_stormControlInfo_t *stormInfo){
	diag_util_mprintf("valid: %d \n",stormInfo->valid);
	diag_util_mprintf("port: %d \n",stormInfo->port);
	diag_util_mprintf("stormType: %d \n",stormInfo->stormType);
	diag_util_mprintf("meterIdx: %d \n",stormInfo->meterIdx);
}
void _diag_multicastFlowShow(rtk_rg_multicastFlow_t *mcFlow){
	diag_util_mprintf("multicast_ipv4_addr: %s \n",diag_util_inet_ntoa(mcFlow->multicast_ipv4_addr));
	diag_util_mprintf("multicast_ipv6_addr: %s\n",diag_util_inet_n6toa( &mcFlow->multicast_ipv6_addr[0]));
	diag_util_mprintf("isIPv6: %d \n",mcFlow->isIPv6);
	diag_util_mprintf("src_intf_idx: %d \n",mcFlow->src_intf_idx);
	diag_util_mprintf("port_mask: 0x%x \n",mcFlow->port_mask);	
}

void _diag_rgInitShow(rtk_rg_initParams_t *init_param){
	diag_util_mprintf("igmpSnoopingEnable: %d \n",init_param->igmpSnoopingEnable);	
	diag_util_mprintf("macBasedTagDecision: %d \n",init_param->macBasedTagDecision);

	if(init_param->initByHwCallBack!=NULL) diag_util_mprintf("initByHwCallBack: 0x%x \n",init_param->initByHwCallBack);
	if(init_param->arpAddByHwCallBack!=NULL) diag_util_mprintf("arpAddByHwCallBack: 0x%x \n",init_param->arpAddByHwCallBack);
	if(init_param->arpDelByHwCallBack!=NULL) diag_util_mprintf("arpDelByHwCallBack: 0x%x \n",init_param->arpDelByHwCallBack);
	if(init_param->macAddByHwCallBack!=NULL) diag_util_mprintf("macAddByHwCallBack: 0x%x \n",init_param->macAddByHwCallBack);
	if(init_param->macDelByHwCallBack!=NULL) diag_util_mprintf("macDelByHwCallBack: 0x%x \n",init_param->macDelByHwCallBack);
	if(init_param->routingAddByHwCallBack!=NULL) diag_util_mprintf("routingAddByHwCallBack: 0x%x \n",init_param->routingAddByHwCallBack);
	if(init_param->routingDelByHwCallBack!=NULL) diag_util_mprintf("routingDelByHwCallBack: 0x%x \n",init_param->routingDelByHwCallBack);
	if(init_param->naptAddByHwCallBack!=NULL) diag_util_mprintf("naptAddByHwCallBack: 0x%x \n",init_param->naptAddByHwCallBack);
	if(init_param->naptDelByHwCallBack!=NULL) diag_util_mprintf("naptDelByHwCallBack: 0x%x \n",init_param->naptDelByHwCallBack);
	if(init_param->bindingAddByHwCallBack!=NULL) diag_util_mprintf("bindingAddByHwCallBack: 0x%x \n",init_param->bindingAddByHwCallBack);
	if(init_param->bindingDelByHwCallBack!=NULL) diag_util_mprintf("bindingDelByHwCallBack: 0x%x \n",init_param->bindingDelByHwCallBack);
	if(init_param->interfaceAddByHwCallBack!=NULL) diag_util_mprintf("interfaceAddByHwCallBack: 0x%x \n",init_param->interfaceAddByHwCallBack);
	if(init_param->interfaceDelByHwCallBack!=NULL) diag_util_mprintf("interfaceDelByHwCallBack: 0x%x \n",init_param->interfaceDelByHwCallBack);
	if(init_param->neighborAddByHwCallBack!=NULL) diag_util_mprintf("neighborAddByHwCallBack: 0x%x \n",init_param->neighborAddByHwCallBack);
	if(init_param->neighborDelByHwCallBack!=NULL) diag_util_mprintf("neighborDelByHwCallBack: 0x%x \n",init_param->neighborDelByHwCallBack);
	if(init_param->v6RoutingAddByHwCallBack!=NULL) diag_util_mprintf("v6RoutingAddByHwCallBack: 0x%x \n",init_param->v6RoutingAddByHwCallBack);
	if(init_param->v6RoutingDelByHwCallBack!=NULL) diag_util_mprintf("v6RoutingDelByHwCallBack: 0x%x \n",init_param->v6RoutingDelByHwCallBack);
	if(init_param->pppoeBeforeDiagByHwCallBack!=NULL) diag_util_mprintf("pppoeBeforeDiagByHwCallBack: 0x%x \n",init_param->pppoeBeforeDiagByHwCallBack);
	if(init_param->naptInboundConnLookupFirstCallBack!=NULL) diag_util_mprintf("naptInboundConnLookupFirstCallBack: 0x%x \n",init_param->naptInboundConnLookupFirstCallBack);
	if(init_param->naptInboundConnLookupSecondCallBack!=NULL) diag_util_mprintf("naptInboundConnLookupSecondCallBack: 0x%x \n",init_param->naptInboundConnLookupSecondCallBack);
	if(init_param->naptInboundConnLookupThirdCallBack!=NULL) diag_util_mprintf("naptInboundConnLookupThirdCallBack: 0x%x \n",init_param->naptInboundConnLookupThirdCallBack);
	if(init_param->dhcpRequestByHwCallBack!=NULL) diag_util_mprintf("dhcpRequestByHwCallBack: 0x%x \n",init_param->dhcpRequestByHwCallBack);

}


/*
 * rg get version
 */
cparser_result_t
cparser_cmd_rg_get_version(
    cparser_context_t *context)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	ret = rtk_rg_driverVersion_get(&version_string);
	if(ret==RT_ERR_RG_OK){
		diag_util_mprintf("%s\n",version_string.version_string);	
	}else{
		return CPARSER_NOT_OK;
	}
    
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_get_version */

/*
 * rg init
 */
cparser_result_t
cparser_cmd_rg_init(
    cparser_context_t *context)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
	ret = rtk_rg_initParam_set(NULL);
	system("echo 0 > /proc/rg/callbackRegist");
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_init */


/*
 * rg get init
 */
cparser_result_t
cparser_cmd_rg_get_init(
    cparser_context_t *context)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&init_param, 0x0, sizeof(rtk_rg_initParams_t));	
	ret = rtk_rg_initParam_get(&init_param);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

	
    _diag_rgInitShow(&init_param);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_get_init */



/*
 * rg show lan-intf
 */
cparser_result_t
cparser_cmd_rg_show_lan_intf(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	_diag_lanIntfShow(&lan_intf);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_lan_intf */

/*
 * rg clear lan-intf
 */
cparser_result_t
cparser_cmd_rg_clear_lan_intf(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&lan_intf, 0x0, sizeof(rtk_rg_lanIntfConf_t));	
	
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_lan_intf */

/*
 * rg add lan-intf entry
 */
cparser_result_t
cparser_cmd_rg_add_lan_intf_entry(
    cparser_context_t *context)
{
	int ret;
	int intf_idx;
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	ret = rtk_rg_lanInterface_add(&lan_intf,&intf_idx);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add lan-intf failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add lan-intf[%d] success.\n",intf_idx);
	}
	
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_lan_intf_entry */


/*
 * rg set lan-intf ip-version <UINT:ip_version> gateway-mac <MACADDR:gmac> ip-addr <IPV4ADDR:ip> ip-mask <IPV4ADDR:mask> ipv6-addr <IPV6ADDR:ipv6_addr> ipv6_network_mask_length <UINT:ipv6_network_mask_length> port-mask <HEX:port_mask> untag-mask <HEX:untag_mask> intf-vlan_id <UINT:intf_vid> mtu <UINT:mtu> isIVL <UINT:isIVL>
 */
cparser_result_t
cparser_cmd_rg_set_lan_intf_ip_version_ip_version_gateway_mac_gmac_ip_addr_ip_ip_mask_mask_ipv6_addr_ipv6_addr_ipv6_network_mask_length_ipv6_network_mask_length_port_mask_port_mask_untag_mask_untag_mask_intf_vlan_id_intf_vid_mtu_mtu_isIVL_isIVL(
    cparser_context_t *context,
    uint32_t  *ip_version_ptr,
    cparser_macaddr_t  *gmac_ptr,
    uint32_t  *ip_ptr,
    uint32_t  *mask_ptr,
    char * *ipv6_addr_ptr,
    uint32_t  *ipv6_network_mask_length_ptr,
    uint32_t  *port_mask_ptr,
    uint32_t  *untag_mask_ptr,
    uint32_t  *intf_vid_ptr,
    uint32_t  *mtu_ptr,
    uint32_t  *isIVL_ptr)
{
   	int32  ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

	lan_intf.ip_version = *ip_version_ptr;
	osal_memcpy(lan_intf.gmac.octet, gmac_ptr->octet, ETHER_ADDR_LEN);
	lan_intf.ip_addr=*ip_ptr;
	lan_intf.ip_network_mask=*mask_ptr;
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&lan_intf.ipv6_addr.ipv6_addr[0], TOKEN_STR(12)), ret);
	lan_intf.ipv6_network_mask_length = *ipv6_network_mask_length_ptr;
	lan_intf.port_mask.portmask=*port_mask_ptr;
	lan_intf.untag_mask.portmask=*untag_mask_ptr;
	lan_intf.intf_vlan_id=*intf_vid_ptr;
	lan_intf.mtu=*mtu_ptr;
	lan_intf.isIVL=*isIVL_ptr;
}    /* end of cparser_cmd_rg_set_lan_intf_ip_version_ip_version_gateway_mac_gmac_ip_addr_ip_ip_mask_mask_ipv6_addr_ipv6_addr_ipv6_network_mask_length_ipv6_network_mask_length_port_mask_port_mask_untag_mask_untag_mask_intf_vlan_id_intf_vid_mtu_mtu_isivl_isivl */



/*
 * rg show wan-intf
 */
cparser_result_t
cparser_cmd_rg_show_wan_intf(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	_diag_wanIntfShow(&wan_intf);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_wan_intf */

/*
 * rg clear wan-intf
 */
cparser_result_t
cparser_cmd_rg_clear_wan_intf(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&wan_intf, 0x0, sizeof(rtk_rg_wanIntfConf_t));	

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_wan_intf */

/*
 * rg add wan-intf entry
 */
cparser_result_t
cparser_cmd_rg_add_wan_intf_entry(
    cparser_context_t *context)
{
	int ret;
	int intf_idx;
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	ret = rtk_rg_wanInterface_add(&wan_intf,&intf_idx);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add wan-intf failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add wan-intf[%d] success.\n",intf_idx);
	}
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_wan_intf_entry */


/*
 * rg set wan-intf wan-type <UINT:wan_type> gateway-mac <MACADDR:gmac> wan-port <UINT:wan_port_idx> port-binding-mask <HEX:port_binding_mask> egress-vlan-tag-on <UINT:tag_on> egress-vlan-id <UINT:vlan_id> egress-vlan-pri <UINT:vlan_pri> isIVL <UINT:isIVL>
 */
cparser_result_t
cparser_cmd_rg_set_wan_intf_wan_type_wan_type_gateway_mac_gmac_wan_port_wan_port_idx_port_binding_mask_port_binding_mask_egress_vlan_tag_on_tag_on_egress_vlan_id_vlan_id_egress_vlan_pri_vlan_pri_isIVL_isIVL(
    cparser_context_t *context,
    uint32_t  *wan_type_ptr,
    cparser_macaddr_t  *gmac_ptr,
    uint32_t  *wan_port_idx_ptr,
    uint32_t  *port_binding_mask_ptr,
    uint32_t  *tag_on_ptr,
    uint32_t  *vlan_id_ptr,
    uint32_t  *vlan_pri_ptr,
    uint32_t  *isIVL_ptr)
{
	DIAG_UTIL_PARAM_CHK();
	wan_intf.wan_type = *wan_type_ptr;
	osal_memcpy(wan_intf.gmac.octet, gmac_ptr->octet, ETHER_ADDR_LEN);
	wan_intf.wan_port_idx = *wan_port_idx_ptr;
	wan_intf.port_binding_mask.portmask= *port_binding_mask_ptr;
	wan_intf.egress_vlan_tag_on = *tag_on_ptr;
	wan_intf.egress_vlan_id = *vlan_id_ptr;
	wan_intf.egress_vlan_pri = *vlan_pri_ptr;
	wan_intf.isIVL=*isIVL_ptr;

	return CPARSER_OK;

}    /* end of cparser_cmd_rg_set_wan_intf_wan_type_wan_type_gateway_mac_gmac_wan_port_wan_port_idx_port_binding_mask_port_binding_mask_egress_vlan_tag_on_tag_on_egress_vlan_id_vlan_id_egress_vlan_pri_vlan_pri_isivl_isivl */




/*
 * rg get intf entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_get_intf_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	int intf_idx = *entry_index_ptr;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
	
	osal_memset(&intf_info, 0x0, sizeof(rtk_rg_intfInfo_t));	

	ret = rtk_rg_intfInfo_find(&intf_info, &intf_idx);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get intf failed. RG API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		if(intf_idx==*entry_index_ptr){
			diag_util_mprintf("intf_name: %s\n",intf_info.intf_name);
			diag_util_mprintf("is_wan: %d\n",intf_info.is_wan);
			if(intf_info.is_wan){
				diag_util_mprintf("wan_intf:\n");
				//FIX ME: there are more informations in wan structure
				_diag_wanIntfShow(&intf_info.wan_intf.wan_intf_conf);			
			}else{
				diag_util_mprintf("lan_intf:\n");
				_diag_lanIntfShow(&intf_info.lan_intf);
			}

			diag_util_mprintf("ingress_packet_count: %d\n",intf_info.ingress_packet_count);
			diag_util_mprintf("ingress_byte_count: %d\n",intf_info.ingress_byte_count);
			diag_util_mprintf("egress_packet_count: %d\n",intf_info.egress_packet_count);
			diag_util_mprintf("egress_byte_count: %d\n",intf_info.egress_byte_count);
			
		}
		else{
			diag_util_mprintf("intf[%d] is empty.\n",*entry_index_ptr);
			return CPARSER_NOT_OK;
		}
	}
 
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_get_intf_entry_entry_index */

/*
 * rg del intf entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_del_intf_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();

	ret = rtk_rg_interface_del(*entry_index_ptr);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_del_intf_entry_entry_index */

/*
 * rg show wan-intf-static-info
 */
cparser_result_t
cparser_cmd_rg_show_wan_intf_static_info(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	_diag_wanIntfStaticInfoShow(&wan_intf_static_info);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_wan_intf_static_info */

/*
 * rg clear wan-intf-static-info
 */
cparser_result_t
cparser_cmd_rg_clear_wan_intf_static_info(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&wan_intf_static_info, 0x0, sizeof(rtk_rg_ipStaticInfo_t));	

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_wan_intf_static_info */


/*
 * rg add wan-intf-static-info intf-index <UINT:index>
 */
cparser_result_t
cparser_cmd_rg_add_wan_intf_static_info_intf_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	ret = rtk_rg_staticInfo_set(*index_ptr, &wan_intf_static_info);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add wan-intf-static-info failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{	
		diag_util_mprintf("add static info to interface[%d] success. \n",*index_ptr);
	}
	
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_wan_intf_static_info_intf_index_index */



/*
 * rg set wan-intf-static-info ip-version <UINT:ip_version>
 */
cparser_result_t
cparser_cmd_rg_set_wan_intf_static_info_ip_version_ip_version(
    cparser_context_t *context,
    uint32_t  *ip_version_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	wan_intf_static_info.ip_version=*ip_version_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_wan_intf_static_info_ip_version_ip_version */


/*
 * rg set wan-intf-static-info ip-version <UINT:ip_version> napt_enable <UINT:napt_enable> ip_addr <IPV4ADDR:ip_addr> ip_network_mask <IPV4ADDR:ip_network_mask> ipv4_default_gateway_on <UINT:ipv4_default_gateway_on> gateway_ipv4_addr <IPV4ADDR:gateway_ipv4_addr> mtu <UINT:mtu>
 */
cparser_result_t
cparser_cmd_rg_set_wan_intf_static_info_ip_version_ip_version_napt_enable_napt_enable_ip_addr_ip_addr_ip_network_mask_ip_network_mask_ipv4_default_gateway_on_ipv4_default_gateway_on_gateway_ipv4_addr_gateway_ipv4_addr_mtu_mtu(
    cparser_context_t *context,
    uint32_t  *ip_version_ptr,
    uint32_t  *napt_enable_ptr,
    uint32_t  *ip_addr_ptr,
    uint32_t  *ip_network_mask_ptr,
    uint32_t  *ipv4_default_gateway_on_ptr,
    uint32_t  *gateway_ipv4_addr_ptr,
    uint32_t  *mtu_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	
	wan_intf_static_info.ip_version=*ip_version_ptr;
	wan_intf_static_info.napt_enable=*napt_enable_ptr;
	wan_intf_static_info.ip_addr=*ip_addr_ptr;
	wan_intf_static_info.ip_network_mask=*ip_network_mask_ptr;

	wan_intf_static_info.ipv4_default_gateway_on=*ipv4_default_gateway_on_ptr;
    wan_intf_static_info.gateway_ipv4_addr=*gateway_ipv4_addr_ptr;
	wan_intf_static_info.mtu=*mtu_ptr;
	
	//gw_mac_auto_learn default setting enable
	wan_intf_static_info.gw_mac_auto_learn_for_ipv4=1;
	memset(wan_intf_static_info.gateway_mac_addr_for_ipv4.octet, 0x0, ETHER_ADDR_LEN);		
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_set_wan_intf_static_info_ip_version_ip_version_napt_enable_napt_enable_ip_addr_ip_addr_ip_network_mask_ip_network_mask_ipv4_default_gateway_on_ipv4_default_gateway_on_gateway_ipv4_addr_gateway_ipv4_addr_mtu_mtu */

/*
 * rg set wan-intf-static-info ip-version <UINT:ip_version> napt_enable <UINT:napt_enable> ip_addr <IPV4ADDR:ip_addr> ip_network_mask <IPV4ADDR:ip_network_mask> ipv4_default_gateway_on <UINT:ipv4_default_gateway_on> gateway_ipv4_addr <IPV4ADDR:gateway_ipv4_addr> mtu <UINT:mtu> gw_mac_auto_learn_for_ipv4 <UINT:gw_mac_auto_learn_for_ipv4> gateway_mac_addr_for_ipv4 <MACADDR:gateway_mac_addr_for_ipv4>
 */
cparser_result_t
cparser_cmd_rg_set_wan_intf_static_info_ip_version_ip_version_napt_enable_napt_enable_ip_addr_ip_addr_ip_network_mask_ip_network_mask_ipv4_default_gateway_on_ipv4_default_gateway_on_gateway_ipv4_addr_gateway_ipv4_addr_mtu_mtu_gw_mac_auto_learn_for_ipv4_gw_mac_auto_learn_for_ipv4_gateway_mac_addr_for_ipv4_gateway_mac_addr_for_ipv4(
    cparser_context_t *context,
    uint32_t  *ip_version_ptr,
    uint32_t  *napt_enable_ptr,
    uint32_t  *ip_addr_ptr,
    uint32_t  *ip_network_mask_ptr,
    uint32_t  *ipv4_default_gateway_on_ptr,
    uint32_t  *gateway_ipv4_addr_ptr,
    uint32_t  *mtu_ptr,
    uint32_t  *gw_mac_auto_learn_for_ipv4_ptr,
    cparser_macaddr_t  *gateway_mac_addr_for_ipv4_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	
	wan_intf_static_info.ip_version=*ip_version_ptr;
	wan_intf_static_info.napt_enable=*napt_enable_ptr;
	wan_intf_static_info.ip_addr=*ip_addr_ptr;
	wan_intf_static_info.ip_network_mask=*ip_network_mask_ptr;

	wan_intf_static_info.ipv4_default_gateway_on=*ipv4_default_gateway_on_ptr;
    wan_intf_static_info.gateway_ipv4_addr=*gateway_ipv4_addr_ptr;
	wan_intf_static_info.mtu=*mtu_ptr;
	
	wan_intf_static_info.gw_mac_auto_learn_for_ipv4=*gw_mac_auto_learn_for_ipv4_ptr;
	osal_memcpy(wan_intf_static_info.gateway_mac_addr_for_ipv4.octet, gateway_mac_addr_for_ipv4_ptr->octet, ETHER_ADDR_LEN);
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_set_wan_intf_static_info_ip_version_ip_version_napt_enable_napt_enable_ip_addr_ip_addr_ip_network_mask_ip_network_mask_ipv4_default_gateway_on_ipv4_default_gateway_on_gateway_ipv4_addr_gateway_ipv4_addr_mtu_mtu_gw_mac_auto_learn_for_ipv4_gw_mac_auto_learn_for_ipv4_gateway_mac_addr_for_ipv4_gateway_mac_addr_for_ipv4 */



/*
 * rg set wan-intf-static-info-ipv6 ipv6_addr <IPV6ADDR:ipv6_addr> ipv6_mask_length <UINT:ipv6_mask_length> ipv6_default_gateway_on <UINT:ipv6_default_gateway_on> gateway_ipv6_addr <IPV6ADDR:gateway_ipv6_addr> mtu <UINT:mtu>
 */
cparser_result_t
cparser_cmd_rg_set_wan_intf_static_info_ipv6_ipv6_addr_ipv6_addr_ipv6_mask_length_ipv6_mask_length_ipv6_default_gateway_on_ipv6_default_gateway_on_gateway_ipv6_addr_gateway_ipv6_addr_mtu_mtu(
    cparser_context_t *context,
    char * *ipv6_addr_ptr,
    uint32_t  *ipv6_mask_length_ptr,
    uint32_t  *ipv6_default_gateway_on_ptr,
    char * *gateway_ipv6_addr_ptr,
    uint32_t  *mtu_ptr)
{
	int32  ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&wan_intf_static_info.ipv6_addr.ipv6_addr[0], TOKEN_STR(4)), ret);
    wan_intf_static_info.ipv6_mask_length=*ipv6_mask_length_ptr;
    wan_intf_static_info.ipv6_default_gateway_on=*ipv6_default_gateway_on_ptr;
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&wan_intf_static_info.gateway_ipv6_addr.ipv6_addr[0], TOKEN_STR(10)), ret);

	wan_intf_static_info.gw_mac_auto_learn_for_ipv6=1;
	memset(wan_intf_static_info.gateway_mac_addr_for_ipv6.octet, 0x0, ETHER_ADDR_LEN);	
	wan_intf_static_info.mtu=*mtu_ptr;
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_set_wan_intf_static_info_ipv6_ipv6_addr_ipv6_addr_ipv6_mask_length_ipv6_mask_length_ipv6_default_gateway_on_ipv6_default_gateway_on_gateway_ipv6_addr_gateway_ipv6_addr_mtu_mtu */


/*
 * rg set wan-intf-static-info-ipv6 ipv6_addr <IPV6ADDR:ipv6_addr> ipv6_mask_length <UINT:ipv6_mask_length> ipv6_default_gateway_on <UINT:ipv6_default_gateway_on> gateway_ipv6_addr <IPV6ADDR:gateway_ipv6_addr> mtu <UINT:mtu> gw_mac_auto_learn_for_ipv6 <UINT:gw_mac_auto_learn_for_ipv6> gateway_mac_addr_for_ipv6 <MACADDR:gateway_mac_addr_for_ipv6>
 */
cparser_result_t
cparser_cmd_rg_set_wan_intf_static_info_ipv6_ipv6_addr_ipv6_addr_ipv6_mask_length_ipv6_mask_length_ipv6_default_gateway_on_ipv6_default_gateway_on_gateway_ipv6_addr_gateway_ipv6_addr_mtu_mtu_gw_mac_auto_learn_for_ipv6_gw_mac_auto_learn_for_ipv6_gateway_mac_addr_for_ipv6_gateway_mac_addr_for_ipv6(
    cparser_context_t *context,
    char * *ipv6_addr_ptr,
    uint32_t  *ipv6_mask_length_ptr,
    uint32_t  *ipv6_default_gateway_on_ptr,
    char * *gateway_ipv6_addr_ptr,
    uint32_t  *mtu_ptr,
    uint32_t  *gw_mac_auto_learn_for_ipv6_ptr,
    cparser_macaddr_t  *gateway_mac_addr_for_ipv6_ptr)
{
	int32  ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&wan_intf_static_info.ipv6_addr.ipv6_addr[0], TOKEN_STR(4)), ret);
    wan_intf_static_info.ipv6_mask_length=*ipv6_mask_length_ptr;
    wan_intf_static_info.ipv6_default_gateway_on=*ipv6_default_gateway_on_ptr;
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&wan_intf_static_info.gateway_ipv6_addr.ipv6_addr[0], TOKEN_STR(10)), ret);
	wan_intf_static_info.mtu=*mtu_ptr;

	wan_intf_static_info.gw_mac_auto_learn_for_ipv6=*gw_mac_auto_learn_for_ipv6_ptr;
	osal_memcpy(wan_intf_static_info.gateway_mac_addr_for_ipv6.octet, gateway_mac_addr_for_ipv6_ptr->octet, ETHER_ADDR_LEN);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_wan_intf_static_info_ipv6_ipv6_addr_ipv6_addr_ipv6_mask_length_ipv6_mask_length_ipv6_default_gateway_on_ipv6_default_gateway_on_gateway_ipv6_addr_gateway_ipv6_addr_mtu_mtu_gw_mac_auto_learn_for_ipv6_gw_mac_auto_learn_for_ipv6_gateway_mac_addr_for_ipv6_gateway_mac_addr_for_ipv6 */

/*
 * rg show mac-entry
 */
cparser_result_t
cparser_cmd_rg_show_mac_entry(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	_diag_macEntryShow(&macEntry);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_mac_entry */

/*
 * rg clear mac-entry
 */
cparser_result_t
cparser_cmd_rg_clear_mac_entry(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

	osal_memset(&macEntry, 0x0, sizeof(rtk_rg_macEntry_t));	

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_mac_entry */

/*
 * rg add mac-entry entry
 */
cparser_result_t
cparser_cmd_rg_add_mac_entry_entry(
    cparser_context_t *context)
{
	int ret;
	int index;
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	ret = rtk_rg_macEntry_add(&macEntry, &index);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add macEntry failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}else{
		diag_util_mprintf("add macEntry[%d] success.\n",index);
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_mac_entry_entry */

/*
 * rg get mac-entry entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_get_mac_entry_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	int index = *entry_index_ptr;
	
	DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&macEntry, 0x0, sizeof(rtk_rg_macEntry_t));	
	ret = rtk_rg_macEntry_find(&macEntry,&index);
	
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get macEntry failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		if(index==*entry_index_ptr){	
			_diag_macEntryShow(&macEntry);
		}
		else{
			diag_util_mprintf("macEntry[%d] is empty.\n",*entry_index_ptr);
			return CPARSER_NOT_OK;
		}
	}
  
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_get_mac_entry_entry_entry_index */

/*
 * rg del mac-entry entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_del_mac_entry_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();

	ret = rtk_rg_macEntry_del(*entry_index_ptr);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_del_mac_entry_entry_entry_index */

/*
 * rg set mac-entry mac-address <MACADDR:mac> isIVL <UINT:isIVL> fid <UINT:fid> vlan_id <UINT:vlan_id> port_idx <UINT:port_idx> static_entry <UINT:static_entry>
 */
cparser_result_t
cparser_cmd_rg_set_mac_entry_mac_address_mac_isIVL_isIVL_fid_fid_vlan_id_vlan_id_port_idx_port_idx_static_entry_static_entry(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *isIVL_ptr,
    uint32_t  *fid_ptr,
    uint32_t  *vlan_id_ptr,
    uint32_t  *port_idx_ptr,
    uint32_t  *static_entry_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	
	osal_memcpy(&macEntry.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
	macEntry.isIVL=*isIVL_ptr;	macEntry.fid=*fid_ptr;
	macEntry.vlan_id=*vlan_id_ptr;	macEntry.port_idx=*port_idx_ptr;
	macEntry.static_entry=*static_entry_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_mac_entry_mac_address_mac_isivl_isivl_fid_fid_vlan_id_vlan_id_port_idx_port_idx_static_entry_static_entry */



/*
 * rg show arp-entry
 */
cparser_result_t
cparser_cmd_rg_show_arp_entry(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	 
	_diag_arpEntryShow(&arpEntry);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_arp_entry */

/*
 * rg clear arp-entry
 */
cparser_result_t
cparser_cmd_rg_clear_arp_entry(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&arpEntry, 0x0, sizeof(rtk_rg_arpEntry_t)); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_arp_entry */

/*
 * rg add arp-entry entry
 */
cparser_result_t
cparser_cmd_rg_add_arp_entry_entry(
    cparser_context_t *context)
{
	int ret;
	int index;
	
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	ret = rtk_rg_arpEntry_add(&arpEntry, &index);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add arpEntry failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add arpEntry[%d] success.\n",index);
	}
	
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_arp_entry_entry */

/*
 * rg get arp-entry entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_get_arp_entry_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	int index = *entry_index_ptr;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&arpInfo, 0x0, sizeof(rtk_rg_arpInfo_t));	
	ret = rtk_rg_arpEntry_find(&arpInfo,&index);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get arpEntry failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		if(index==*entry_index_ptr){	
			_diag_arpEntryShow(&arpInfo.arpEntry);
			diag_util_mprintf("valid: %d \n",arpInfo.valid);
			diag_util_mprintf("idleSecs: %d \n",arpInfo.idleSecs);
		}
		else{
			diag_util_mprintf("arpEntry[%d] is empty.\n",*entry_index_ptr);
			return CPARSER_NOT_OK;
		}
	}
  
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_get_arp_entry_entry_entry_index */

/*
 * rg del arp-entry entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_del_arp_entry_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
 
	 int ret;
	 DIAG_UTIL_PARAM_CHK();
	 
	 ret = rtk_rg_arpEntry_del(*entry_index_ptr);
	 if(ret!=RT_ERR_RG_OK)
		 return CPARSER_NOT_OK;
 
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_del_arp_entry_entry_entry_index */

/*
 * rg set arp-entry macEntryIdx <UINT:macEntryIdx> ip_addr <IPV4ADDR:ipv4> static_entry <UINT:static_entry> valid <UINT:valid>
 */
cparser_result_t
cparser_cmd_rg_set_arp_entry_macEntryIdx_macEntryIdx_ip_addr_ipv4_static_entry_static_entry_valid_valid(
    cparser_context_t *context,
    uint32_t  *macEntryIdx_ptr,
    uint32_t  *ipv4_ptr,
    uint32_t  *static_entry_ptr,
    uint32_t  *valid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

	arpEntry.macEntryIdx=*macEntryIdx_ptr;
	arpEntry.ipv4Addr=*ipv4_ptr;
	arpEntry.staticEntry=*static_entry_ptr;
	
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_arp_entry_macentryidx_macentryidx_ip_addr_ipv4_static_entry_static_entry_valid_valid */

/*
 * rg show napt-connection 
 */
cparser_result_t
cparser_cmd_rg_show_napt_connection(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	_diag_naptConnShow(&naptFlow);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_napt_connection */

/*
 * rg clear napt-connection 
 */
cparser_result_t
cparser_cmd_rg_clear_napt_connection(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&naptFlow, 0x0, sizeof(rtk_rg_naptEntry_t)); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_napt_connection */

/*
 * rg add napt-connection entry
 */
cparser_result_t
cparser_cmd_rg_add_napt_connection_entry(
    cparser_context_t *context)
{
	int ret;
	int index;
	
    DIAG_UTIL_PARAM_CHK();	
	DIAG_UTIL_OUTPUT_INIT();
	
	ret =  rtk_rg_naptConnection_add(&naptFlow, &index);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add naptConn failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add naptConn[%d] success.\n",index);
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_napt_connection_entry */

/*
 * rg get napt-connection entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_get_napt_connection_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	int index = *entry_index_ptr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
	osal_memset(&naptFlow, 0x0, sizeof(rtk_rg_naptEntry_t));	

	ret =  rtk_rg_naptConnection_find(&naptFlow,&index);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get naptConn failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		if(index==*entry_index_ptr){	
			_diag_naptConnShow(&naptFlow);
		}
		else{
			diag_util_mprintf("naptConn[%d] is empty.\n",*entry_index_ptr);
			return CPARSER_NOT_OK;
		}
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_get_napt_connection_entry_entry_index */

/*
 * rg del napt-connection entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_del_napt_connection_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	
    DIAG_UTIL_PARAM_CHK();
	ret = rtk_rg_naptConnection_del(*entry_index_ptr);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;
	
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_del_napt_connection_entry_entry_index */

/*
 * rg set napt-connection is_tcp <UINT:is_tcp> local_ip <IPV4ADDR:local_ip> remote_ip <IPV4ADDR:remote_ip> wan_intf_idx <UINT:wan_intf_idx> local_port <UINT:local_port> remote_port <UINT:remote_port> external_port <UINT:external_port> outbound_pri_valid <UINT:outbound_pri_valid> outbound_priority <UINT:outbound_priority> inbound_pri_valid <UINT:inbound_pri_valid> inbound_priority <UINT:inbound_priority>
 */
cparser_result_t
cparser_cmd_rg_set_napt_connection_is_tcp_is_tcp_local_ip_local_ip_remote_ip_remote_ip_wan_intf_idx_wan_intf_idx_local_port_local_port_remote_port_remote_port_external_port_external_port_outbound_pri_valid_outbound_pri_valid_outbound_priority_outbound_priority_inbound_pri_valid_inbound_pri_valid_inbound_priority_inbound_priority(
    cparser_context_t *context,
    uint32_t  *is_tcp_ptr,
    uint32_t  *local_ip_ptr,
    uint32_t  *remote_ip_ptr,
    uint32_t  *wan_intf_idx_ptr,
    uint32_t  *local_port_ptr,
    uint32_t  *remote_port_ptr,
    uint32_t  *external_port_ptr,
    uint32_t  *outbound_pri_valid_ptr,
    uint32_t  *outbound_priority_ptr,
    uint32_t  *inbound_pri_valid_ptr,
    uint32_t  *inbound_priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	naptFlow.is_tcp = *is_tcp_ptr;
	naptFlow.local_ip = *local_ip_ptr;
	naptFlow.remote_ip = *remote_ip_ptr;
	naptFlow.wan_intf_idx = *wan_intf_idx_ptr;
	naptFlow.local_port = *local_port_ptr;
	naptFlow.remote_port = *remote_port_ptr;
	naptFlow.external_port = *external_port_ptr;
	naptFlow.outbound_pri_valid = *outbound_pri_valid_ptr;
	naptFlow.outbound_priority = *outbound_priority_ptr;
	naptFlow.inbound_pri_valid = *inbound_pri_valid_ptr;
	naptFlow.inbound_priority = *inbound_priority_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_napt_connection_is_tcp_is_tcp_local_ip_local_ip_remote_ip_remote_ip_wan_intf_idx_wan_intf_idx_local_port_local_port_remote_port_remote_port_external_port_external_port_outbound_pri_valid_outbound_pri_valid_outbound_priority_outbound_priority_inbound_pri_valid_inbound_pri_valid_inbound_priority_inbound_priority */



/*
 * rg clear url-filter entry
 */
cparser_result_t
cparser_cmd_rg_clear_url_filter_entry(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&urlFilter, 0x0, sizeof(rtk_rg_urlFilterString_t)); 
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_url_filter_entry */

/*
 * rg add url-filter entry
 */
cparser_result_t
cparser_cmd_rg_add_url_filter_entry(
    cparser_context_t *context)
{
	int ret;
	int index;

    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	
	ret =  rtk_rg_urlFilterString_add(&urlFilter,&index);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add urlFilter failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add urlFilter[%d] success.\n",index);
	}
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_url_filter_entry */

/*
 * rg set url-filter url <STRING:url> path <STRING:path> path-exactly-match <UINT:match> wan-interface <UINT:wanintf>
 */
cparser_result_t
cparser_cmd_rg_set_url_filter_url_url_path_path_path_exactly_match_match_wan_interface_wanintf(
    cparser_context_t *context,
    char * *url_ptr,
    char * *path_ptr,
    uint32_t  *match_ptr,
    uint32_t  *wanintf_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	
	osal_memcpy(urlFilter.url_filter_string, *url_ptr, 128);	
	osal_memcpy(urlFilter.path_filter_string, *path_ptr, 256);	
	urlFilter.path_exactly_match = *match_ptr;
	urlFilter.wan_intf= *wanintf_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_url_filter_url_url_path_path_path_exactly_match_match_wan_interface_wanintf */

/*
 * rg get url-filter url-index <UINT:index>
 */
cparser_result_t
cparser_cmd_rg_get_url_filter_url_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

	int ret;
	int index = *index_ptr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
	osal_memset(&urlFilter, 0x0, sizeof(rtk_rg_urlFilterString_t));	

	ret =  rtk_rg_urlFilterString_find(&urlFilter,&index);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get urlFilter failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		if(index==*index_ptr){	
			_diag_urlFilterShow(&urlFilter);
		}
		else{
			diag_util_mprintf("naptConn[%d] is empty.\n",*index_ptr);
			return CPARSER_NOT_OK;
		}
	}
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_get_url_filter_url_index_index */

/*
 * rg del url-filter url-index <UINT:index>
 */
cparser_result_t
cparser_cmd_rg_del_url_filter_url_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int ret;
	
    DIAG_UTIL_PARAM_CHK();
	ret = rtk_rg_urlFilterString_del(*index_ptr);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;
	
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_del_url_filter_url_index_index */

/*
 * rg show callback
 */
cparser_result_t
cparser_cmd_rg_show_callback(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	_diag_callbackShow(&init_param);
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_callback */

/*
 * rg clear callback
 */
cparser_result_t
cparser_cmd_rg_clear_callback(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&init_param, 0x0, sizeof(rtk_rg_initParams_t)); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_callback */

/*
 * rg set callback initByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_initByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.initByHwCallBack = *addr_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_initbyhwcallback_addr */

/*
 * rg set callback arpAddByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_arpAddByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.arpAddByHwCallBack = *addr_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_arpaddbyhwcallback_addr */

/*
 * rg set callback arpDelByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_arpDelByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.arpDelByHwCallBack = *addr_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_arpdelbyhwcallback_addr */

/*
 * rg set callback macAddByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_macAddByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.macAddByHwCallBack = *addr_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_macaddbyhwcallback_addr */

/*
 * rg set callback macDelByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_macDelByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.macDelByHwCallBack = *addr_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_macdelbyhwcallback_addr */

/*
 * rg set callback naptAddByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_naptAddByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.naptAddByHwCallBack = *addr_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_naptaddbyhwcallback_addr */

/*
 * rg set callback naptDelByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_naptDelByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.naptDelByHwCallBack = *addr_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_naptdelbyhwcallback_addr */

/*
 * rg set callback routingAddByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_routingAddByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.routingAddByHwCallBack = *addr_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_routingaddbyhwcallback_addr */

/*
 * rg set callback routingDelByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_routingDelByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.routingDelByHwCallBack = *addr_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_routingdelbyhwcallback_addr */

/*
 * rg set callback bindingAddByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_bindingAddByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.bindingAddByHwCallBack = *addr_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_bindingaddbyhwcallback_addr */

/*
 * rg set callback bindingDelByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_bindingDelByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.bindingDelByHwCallBack = *addr_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_bindingdelbyhwcallback_addr */

/*
 * rg set callback interfaceAddByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_interfaceAddByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.interfaceAddByHwCallBack = *addr_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_interfaceaddbyhwcallback_addr */

/*
 * rg set callback interfaceDelByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_interfaceDelByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.interfaceDelByHwCallBack = *addr_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_interfacedelbyhwcallback_addr */

/*
 * rg set callback pppoeBeforeDiagByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_pppoeBeforeDiagByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.pppoeBeforeDiagByHwCallBack = *addr_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_pppoebeforediagbyhwcallback_addr */

/*
 * rg set callback neighborAddByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_neighborAddByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.neighborAddByHwCallBack = *addr_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_neighboraddbyhwcallback_addr */

/*
 * rg set callback neighborDelByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_neighborDelByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.neighborDelByHwCallBack = *addr_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_neighbordelbyhwcallback_addr */

/*
 * rg set callback v6RoutingAddByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_v6RoutingAddByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.v6RoutingAddByHwCallBack = *addr_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_v6routingaddbyhwcallback_addr */

/*
 * rg set callback v6RoutingDelByHwCallBack <HEX:addr>
 */
cparser_result_t
cparser_cmd_rg_set_callback_v6RoutingDelByHwCallBack_addr(
    cparser_context_t *context,
    uint32_t  *addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	init_param.v6RoutingDelByHwCallBack = *addr_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_callback_v6routingdelbyhwcallback_addr */


/*
 * rg init callback igmpSnoopingEnable <UINT:igmpSnoopingEnable> macBasedTagDecision <UINT:macBasedTagDecision>
 */
cparser_result_t
cparser_cmd_rg_init_callback_igmpSnoopingEnable_igmpSnoopingEnable_macBasedTagDecision_macBasedTagDecision(
    cparser_context_t *context,
    uint32_t  *igmpSnoopingEnable_ptr,
    uint32_t  *macBasedTagDecision_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
	init_param.igmpSnoopingEnable=*igmpSnoopingEnable_ptr;
	init_param.macBasedTagDecision=*macBasedTagDecision_ptr;
	ret = rtk_rg_initParam_set(&init_param);
	system("echo 1 > /proc/rg/callbackRegist");
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_init_callback_igmpsnoopingenable_igmpsnoopingenable_macbasedtagdecision_macbasedtagdecision */

/*
 * rg init callback default igmpSnoopingEnable <UINT:igmpSnoopingEnable> macBasedTagDecision <UINT:macBasedTagDecision>
 */
cparser_result_t
cparser_cmd_rg_init_callback_default_igmpSnoopingEnable_igmpSnoopingEnable_macBasedTagDecision_macBasedTagDecision(
    cparser_context_t *context,
    uint32_t  *igmpSnoopingEnable_ptr,
    uint32_t  *macBasedTagDecision_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&init_param, 0x0, sizeof(rtk_rg_initParams_t)); 
	init_param.igmpSnoopingEnable=*igmpSnoopingEnable_ptr; //default enable
	init_param.macBasedTagDecision=*macBasedTagDecision_ptr;//default disable
	init_param.initByHwCallBack = 0xfffffffe;
	ret = rtk_rg_initParam_set(&init_param);
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_init_callback_default_igmpsnoopingenable_igmpsnoopingenable_macbasedtagdecision_macbasedtagdecision */


/*
 * rg add algApps app_mask <HEX:appmask>
 */
cparser_result_t
cparser_cmd_rg_add_algApps_app_mask_appmask(
    cparser_context_t *context,
    uint32_t  *appmask_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
	alg_app = *appmask_ptr;
	ret = rtk_rg_algApps_set(alg_app);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_add_algapps_app_mask_appmask */

/*
 * rg get algApps
 */
cparser_result_t
cparser_cmd_rg_get_algApps(
    cparser_context_t *context)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
	ret = rtk_rg_algApps_get(&alg_app);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    _diag_algAppShow(&alg_app);
	diag_util_mprintf("alg_app=0x%x\n",alg_app);
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_get_algapps */


/*
 * rg set serverInLanAppsIpAddr algType <UINT:algType> serverAddress <IPV4ADDR:serverAddress>
 */
cparser_result_t
cparser_cmd_rg_set_serverInLanAppsIpAddr_algType_algType_serverAddress_serverAddress(
    cparser_context_t *context,
    uint32_t  *algType_ptr,
    uint32_t  *serverAddress_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	srvIpMapping.algType = *algType_ptr;
	srvIpMapping.serverAddress = *serverAddress_ptr;
		
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_serverinlanappsipaddr_algtype_algtype_serveraddress_serveraddress */

/*
 * rg add serverInLanAppsIpAddr entry
 */
cparser_result_t
cparser_cmd_rg_add_serverInLanAppsIpAddr_entry(
    cparser_context_t *context)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
	ret= rtk_rg_algServerInLanAppsIpAddr_add(&srvIpMapping);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_serverinlanappsipaddr_entry */

/*
 * rg del serverInLanAppsIpAddr algType <UINT:algType>
 */
cparser_result_t
cparser_cmd_rg_del_serverInLanAppsIpAddr_algType_algType(
    cparser_context_t *context,
    uint32_t  *algType_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
	ret= rtk_rg_algServerInLanAppsIpAddr_del(*algType_ptr);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_del_serverinlanappsipaddr_algtype_algtype */




/*
 * rg show pppoeClientInfoAfterDial
 */
cparser_result_t
cparser_cmd_rg_show_pppoeClientInfoAfterDial(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	_diag_pppoeClientInfoShow(&clientPppoe_info);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_pppoeclientinfoafterdial */

/*
 * rg add pppoeClientInfoAfterDial wan_intf_idx <UINT:wan_intf_idx>
 */
cparser_result_t
cparser_cmd_rg_add_pppoeClientInfoAfterDial_wan_intf_idx_wan_intf_idx(
    cparser_context_t *context,
    uint32_t  *wan_intf_idx_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
	ret = rtk_rg_pppoeClientInfoAfterDial_set(*wan_intf_idx_ptr,&clientPppoe_info);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_pppoeclientinfoafterdial_wan_intf_idx_wan_intf_idx */

/*
 * rg clear pppoeClientInfoAfterDial
 */
cparser_result_t
cparser_cmd_rg_clear_pppoeClientInfoAfterDial(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&clientPppoe_info, 0x0, sizeof(rtk_rg_pppoeClientInfoAfterDial_t)); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_pppoeclientinfoafterdial */

/*
 * rg set pppoeClientInfoAfterDial sessionId <UINT:sessionId> ip-version <UINT:ip_version> napt_enable <UINT:napt_enable> ip_addr <IPV4ADDR:ip_addr> ip_network_mask <IPV4ADDR:ip_network_mask> ipv4_default_gateway_on <UINT:ipv4_default_gateway_on> gateway_ipv4_addr <IPV4ADDR:gateway_ipv4_addr> mtu <UINT:mtu>
 */
cparser_result_t
cparser_cmd_rg_set_pppoeClientInfoAfterDial_sessionId_sessionId_ip_version_ip_version_napt_enable_napt_enable_ip_addr_ip_addr_ip_network_mask_ip_network_mask_ipv4_default_gateway_on_ipv4_default_gateway_on_gateway_ipv4_addr_gateway_ipv4_addr_mtu_mtu(
    cparser_context_t *context,
    uint32_t  *sessionId_ptr,
    uint32_t  *ip_version_ptr,
    uint32_t  *napt_enable_ptr,
    uint32_t  *ip_addr_ptr,
    uint32_t  *ip_network_mask_ptr,
    uint32_t  *ipv4_default_gateway_on_ptr,
    uint32_t  *gateway_ipv4_addr_ptr,
    uint32_t  *mtu_ptr)
{
	int32  ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
	clientPppoe_info.sessionId = *sessionId_ptr;
	clientPppoe_info.hw_info.ip_version=*ip_version_ptr;
	clientPppoe_info.hw_info.napt_enable=*napt_enable_ptr;
	clientPppoe_info.hw_info.ip_addr=*ip_addr_ptr;
	clientPppoe_info.hw_info.ip_network_mask=*ip_network_mask_ptr;

	clientPppoe_info.hw_info.ipv4_default_gateway_on=*ipv4_default_gateway_on_ptr;
    clientPppoe_info.hw_info.gateway_ipv4_addr=*gateway_ipv4_addr_ptr;
	clientPppoe_info.hw_info.mtu=*mtu_ptr;
	
	//gw_mac_auto_learn default setting enable
	clientPppoe_info.hw_info.gw_mac_auto_learn_for_ipv4=1;
	memset(clientPppoe_info.hw_info.gateway_mac_addr_for_ipv4.octet, 0x0, ETHER_ADDR_LEN);		
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_set_pppoeclientinfoafterdial_sessionid_sessionid_ip_version_ip_version_napt_enable_napt_enable_ip_addr_ip_addr_ip_network_mask_ip_network_mask_ipv4_default_gateway_on_ipv4_default_gateway_on_gateway_ipv4_addr_gateway_ipv4_addr_mtu_mtu */

/*
 * rg set pppoeClientInfoAfterDial sessionId <UINT:sessionId> ip-version <UINT:ip_version> napt_enable <UINT:napt_enable> ip_addr <IPV4ADDR:ip_addr> ip_network_mask <IPV4ADDR:ip_network_mask> ipv4_default_gateway_on <UINT:ipv4_default_gateway_on> gateway_ipv4_addr <IPV4ADDR:gateway_ipv4_addr> mtu <UINT:mtu> gw_mac_auto_learn_for_ipv4 <UINT:gw_mac_auto_learn_for_ipv4> gateway_mac_addr_for_ipv4 <MACADDR:gateway_mac_addr_for_ipv4>
 */
cparser_result_t
cparser_cmd_rg_set_pppoeClientInfoAfterDial_sessionId_sessionId_ip_version_ip_version_napt_enable_napt_enable_ip_addr_ip_addr_ip_network_mask_ip_network_mask_ipv4_default_gateway_on_ipv4_default_gateway_on_gateway_ipv4_addr_gateway_ipv4_addr_mtu_mtu_gw_mac_auto_learn_for_ipv4_gw_mac_auto_learn_for_ipv4_gateway_mac_addr_for_ipv4_gateway_mac_addr_for_ipv4(
    cparser_context_t *context,
    uint32_t  *sessionId_ptr,
    uint32_t  *ip_version_ptr,
    uint32_t  *napt_enable_ptr,
    uint32_t  *ip_addr_ptr,
    uint32_t  *ip_network_mask_ptr,
    uint32_t  *ipv4_default_gateway_on_ptr,
    uint32_t  *gateway_ipv4_addr_ptr,
    uint32_t  *mtu_ptr,
    uint32_t  *gw_mac_auto_learn_for_ipv4_ptr,
    cparser_macaddr_t  *gateway_mac_addr_for_ipv4_ptr)
{
 	int32  ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
	clientPppoe_info.sessionId = *sessionId_ptr;
	clientPppoe_info.hw_info.ip_version=*ip_version_ptr;
	clientPppoe_info.hw_info.napt_enable=*napt_enable_ptr;
	clientPppoe_info.hw_info.ip_addr=*ip_addr_ptr;
	clientPppoe_info.hw_info.ip_network_mask=*ip_network_mask_ptr;

	clientPppoe_info.hw_info.ipv4_default_gateway_on=*ipv4_default_gateway_on_ptr;
    clientPppoe_info.hw_info.gateway_ipv4_addr=*gateway_ipv4_addr_ptr;
	clientPppoe_info.hw_info.mtu=*mtu_ptr;
	
	clientPppoe_info.hw_info.gw_mac_auto_learn_for_ipv4=*gw_mac_auto_learn_for_ipv4_ptr;
	osal_memcpy(clientPppoe_info.hw_info.gateway_mac_addr_for_ipv4.octet, gateway_mac_addr_for_ipv4_ptr->octet, ETHER_ADDR_LEN);	
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_set_pppoeclientinfoafterdial_sessionid_sessionid_ip_version_ip_version_napt_enable_napt_enable_ip_addr_ip_addr_ip_network_mask_ip_network_mask_ipv4_default_gateway_on_ipv4_default_gateway_on_gateway_ipv4_addr_gateway_ipv4_addr_mtu_mtu_gw_mac_auto_learn_for_ipv4_gw_mac_auto_learn_for_ipv4_gateway_mac_addr_for_ipv4_gateway_mac_addr_for_ipv4 */


/*
 * rg set pppoeClientInfoAfterDial-ipv6 ipv6_addr <IPV6ADDR:ipv6_addr> ipv6_mask_length <UINT:ipv6_mask_length> ipv6_default_gateway_on <UINT:ipv6_default_gateway_on> gateway_ipv6_addr <IPV6ADDR:gateway_ipv6_addr>
 */
cparser_result_t
cparser_cmd_rg_set_pppoeClientInfoAfterDial_ipv6_ipv6_addr_ipv6_addr_ipv6_mask_length_ipv6_mask_length_ipv6_default_gateway_on_ipv6_default_gateway_on_gateway_ipv6_addr_gateway_ipv6_addr(
    cparser_context_t *context,
    char * *ipv6_addr_ptr,
    uint32_t  *ipv6_mask_length_ptr,
    uint32_t  *ipv6_default_gateway_on_ptr,
    char * *gateway_ipv6_addr_ptr)
{
	int32  ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&clientPppoe_info.hw_info.ipv6_addr.ipv6_addr[0], TOKEN_STR(4)), ret);
    clientPppoe_info.hw_info.ipv6_mask_length=*ipv6_mask_length_ptr;
    clientPppoe_info.hw_info.ipv6_default_gateway_on=*ipv6_default_gateway_on_ptr;
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&clientPppoe_info.hw_info.gateway_ipv6_addr.ipv6_addr[0], TOKEN_STR(10)), ret);
	
	//gw_mac_auto_learn default setting enable	
	clientPppoe_info.hw_info.gw_mac_auto_learn_for_ipv6=1;
	memset(clientPppoe_info.hw_info.gateway_mac_addr_for_ipv6.octet, 0x0, ETHER_ADDR_LEN);	
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_set_pppoeclientinfoafterdial_ipv6_ipv6_addr_ipv6_addr_ipv6_mask_length_ipv6_mask_length_ipv6_default_gateway_on_ipv6_default_gateway_on_gateway_ipv6_addr_gateway_ipv6_addr */

/*
 * rg set pppoeClientInfoAfterDial-ipv6 ipv6_addr <IPV6ADDR:ipv6_addr> ipv6_mask_length <UINT:ipv6_mask_length> ipv6_default_gateway_on <UINT:ipv6_default_gateway_on> gateway_ipv6_addr <IPV6ADDR:gateway_ipv6_addr> gw_mac_auto_learn_for_ipv6 <UINT:gw_mac_auto_learn_for_ipv6> gateway_mac_addr_for_ipv6 <MACADDR:gateway_mac_addr_for_ipv6>
 */
cparser_result_t
cparser_cmd_rg_set_pppoeClientInfoAfterDial_ipv6_ipv6_addr_ipv6_addr_ipv6_mask_length_ipv6_mask_length_ipv6_default_gateway_on_ipv6_default_gateway_on_gateway_ipv6_addr_gateway_ipv6_addr_gw_mac_auto_learn_for_ipv6_gw_mac_auto_learn_for_ipv6_gateway_mac_addr_for_ipv6_gateway_mac_addr_for_ipv6(
    cparser_context_t *context,
    char * *ipv6_addr_ptr,
    uint32_t  *ipv6_mask_length_ptr,
    uint32_t  *ipv6_default_gateway_on_ptr,
    char * *gateway_ipv6_addr_ptr,
    uint32_t  *gw_mac_auto_learn_for_ipv6_ptr,
    cparser_macaddr_t  *gateway_mac_addr_for_ipv6_ptr)
{
	int32  ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&clientPppoe_info.hw_info.ipv6_addr.ipv6_addr[0], TOKEN_STR(4)), ret);
    clientPppoe_info.hw_info.ipv6_mask_length=*ipv6_mask_length_ptr;
    clientPppoe_info.hw_info.ipv6_default_gateway_on=*ipv6_default_gateway_on_ptr;
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&clientPppoe_info.hw_info.gateway_ipv6_addr.ipv6_addr[0], TOKEN_STR(10)), ret);

	
	clientPppoe_info.hw_info.gw_mac_auto_learn_for_ipv6=*gw_mac_auto_learn_for_ipv6_ptr;
	osal_memcpy(clientPppoe_info.hw_info.gateway_mac_addr_for_ipv6.octet, gateway_mac_addr_for_ipv6_ptr->octet, ETHER_ADDR_LEN);
	
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_set_pppoeclientinfoafterdial_ipv6_ipv6_addr_ipv6_addr_ipv6_mask_length_ipv6_mask_length_ipv6_default_gateway_on_ipv6_default_gateway_on_gateway_ipv6_addr_gateway_ipv6_addr_gw_mac_auto_learn_for_ipv6_gw_mac_auto_learn_for_ipv6_gateway_mac_addr_for_ipv6_gateway_mac_addr_for_ipv6 */



/*
 * rg show pppoeClientInfoBeforeDial
 */
cparser_result_t
cparser_cmd_rg_show_pppoeClientInfoBeforeDial(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	_diag_pppoeClientInfoBeforeDiagShow(&clientPppoe_beforeDiag_info);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_pppoeclientinfobeforedial */

/*
 * rg add pppoeClientInfoBeforeDial wan_intf_idx <UINT:wan_intf_idx>
 */
cparser_result_t
cparser_cmd_rg_add_pppoeClientInfoBeforeDial_wan_intf_idx_wan_intf_idx(
    cparser_context_t *context,
    uint32_t  *wan_intf_idx_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
	ret = rtk_rg_pppoeClientInfoBeforeDial_set(*wan_intf_idx_ptr, &clientPppoe_beforeDiag_info);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_pppoeclientinfobeforedial_wan_intf_idx_wan_intf_idx */




/*
 * rg clear pppoeClientInfoBeforeDial
 */
cparser_result_t
cparser_cmd_rg_clear_pppoeClientInfoBeforeDial(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&clientPppoe_beforeDiag_info, 0x0, sizeof(rtk_rg_pppoeClientInfoBeforeDial_t)); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_pppoeclientinfobeforedial */

/*
 * rg set pppoeClientInfoBeforeDial username <STRING:username> password <STRING:password> auth_type <UINT:auth_type> pppoe_proxy_enable <UINT:pppoe_proxy_enable> max_pppoe_proxy_num <UINT:max_pppoe_proxy_num> auto_reconnect <UINT:auto_reconnect> dial_on_demond <UINT:dial_on_demond> idle_timeout_secs <UINT:idle_timeout_secs> status <UINT:status> dialOnDemondCallBack <HEX:dialOnDemondCallBack_addr> idleTimeOutCallBack <HEX:idleTimeOutCallBack_addr>
 */
cparser_result_t
cparser_cmd_rg_set_pppoeClientInfoBeforeDial_username_username_password_password_auth_type_auth_type_pppoe_proxy_enable_pppoe_proxy_enable_max_pppoe_proxy_num_max_pppoe_proxy_num_auto_reconnect_auto_reconnect_dial_on_demond_dial_on_demond_idle_timeout_secs_idle_timeout_secs_status_status_dialOnDemondCallBack_dialOnDemondCallBack_addr_idleTimeOutCallBack_idleTimeOutCallBack_addr(
    cparser_context_t *context,
    char * *username_ptr,
    char * *password_ptr,
    uint32_t  *auth_type_ptr,
    uint32_t  *pppoe_proxy_enable_ptr,
    uint32_t  *max_pppoe_proxy_num_ptr,
    uint32_t  *auto_reconnect_ptr,
    uint32_t  *dial_on_demond_ptr,
    uint32_t  *idle_timeout_secs_ptr,
    uint32_t  *status_ptr,
    uint32_t  *dialOnDemondCallBack_addr_ptr,
    uint32_t  *idleTimeOutCallBack_addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	
	osal_memcpy(clientPppoe_beforeDiag_info.username, *username_ptr, 32);	
	osal_memcpy(clientPppoe_beforeDiag_info.password, *password_ptr, 32);	
	clientPppoe_beforeDiag_info.auth_type= *auth_type_ptr;
	clientPppoe_beforeDiag_info.pppoe_proxy_enable= *pppoe_proxy_enable_ptr;
	clientPppoe_beforeDiag_info.max_pppoe_proxy_num= *max_pppoe_proxy_num_ptr;
	clientPppoe_beforeDiag_info.auto_reconnect= *auto_reconnect_ptr;
	clientPppoe_beforeDiag_info.dial_on_demond= *dial_on_demond_ptr;
	clientPppoe_beforeDiag_info.idle_timeout_secs= *idle_timeout_secs_ptr;
	clientPppoe_beforeDiag_info.stauts= *status_ptr;
	if(*dialOnDemondCallBack_addr_ptr==0x0){
		clientPppoe_beforeDiag_info.dialOnDemondCallBack=NULL;
	}else{
		clientPppoe_beforeDiag_info.dialOnDemondCallBack=*dialOnDemondCallBack_addr_ptr;
	}

	if(*idleTimeOutCallBack_addr_ptr=0x0){
		clientPppoe_beforeDiag_info.idleTimeOutCallBack=NULL;
	}else{
		clientPppoe_beforeDiag_info.idleTimeOutCallBack=*idleTimeOutCallBack_addr_ptr;
	}
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_pppoeclientinfobeforedial_username_username_password_password_auth_type_auth_type_pppoe_proxy_enable_pppoe_proxy_enable_max_pppoe_proxy_num_max_pppoe_proxy_num_auto_reconnect_auto_reconnect_dial_on_demond_dial_on_demond_idle_timeout_secs_idle_timeout_secs_status_status_dialondemondcallback_dialondemondcallback_addr_idletimeoutcallback_idletimeoutcallback_addr */

/*
 * rg show neighbor-entry
 */
cparser_result_t
cparser_cmd_rg_show_neighbor_entry(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	_diag_neighborEntryShow(&neighborEntry);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_neighbor_entry */

/*
 * rg clear neighbor-entry
 */
cparser_result_t
cparser_cmd_rg_clear_neighbor_entry(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&neighborEntry, 0x0, sizeof(rtk_rg_neighborEntry_t)); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_neighbor_entry */

/*
 * rg add neighbor-entry entry
 */
cparser_result_t
cparser_cmd_rg_add_neighbor_entry_entry(
    cparser_context_t *context)
{
	int ret;
	int index;
	
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	ret = rtk_rg_neighborEntry_add(&neighborEntry,&index);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add neighborEntry failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add neighborEntry[%d] success.\n",index);
	}
	
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_add_neighbor_entry_entry */

/*
 * rg get neighbor-entry entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_get_neighbor_entry_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	int index=*entry_index_ptr;

	DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
	osal_memset(&neighborInfo, 0x0, sizeof(rtk_rg_neighborInfo_t)); 

	ret = rtk_rg_neighborEntry_find(&neighborInfo,&index);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get neighborEntry failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		if(index==*entry_index_ptr){	
			_diag_neighborEntryShow(&neighborInfo.neighborEntry);
		}
		else{
			diag_util_mprintf("neighborEntry[%d] is empty.\n",*entry_index_ptr);
			return CPARSER_NOT_OK;
		}
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_get_neighbor_entry_entry_entry_index */

/*
 * rg del neighbor-entry entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_del_neighbor_entry_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	DIAG_UTIL_PARAM_CHK();
	
	ret = rtk_rg_neighborEntry_del(*entry_index_ptr);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;
	
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_del_neighbor_entry_entry_entry_index */

/*
 * rg set neighbor-entry l2Idx <UINT:l2Idx> matchRouteIdx <UINT:matchRouteIdx> interfaceId_up <HEX:ipv6_addr_63_32> interfaceId_low <HEX:ipv6_addr_31_0> staticEntry <UINT:staticEntry>
 */
cparser_result_t
cparser_cmd_rg_set_neighbor_entry_l2Idx_l2Idx_matchRouteIdx_matchRouteIdx_interfaceId_up_ipv6_addr_63_32_interfaceId_low_ipv6_addr_31_0_staticEntry_staticEntry(
    cparser_context_t *context,
    uint32_t  *l2Idx_ptr,
    uint32_t  *matchRouteIdx_ptr,
    uint32_t  *ipv6_addr_63_32_ptr,
    uint32_t  *ipv6_addr_31_0_ptr,
    uint32_t  *staticEntry_ptr)
{
    DIAG_UTIL_PARAM_CHK();
        neighborEntry.l2Idx = *l2Idx_ptr;
        neighborEntry.matchRouteIdx = *matchRouteIdx_ptr;
        osal_memcpy(&neighborEntry.interfaceId[0], ipv6_addr_63_32_ptr, 4);
        osal_memcpy(&neighborEntry.interfaceId[4], ipv6_addr_31_0_ptr, 4);
        neighborEntry.valid = 1;
        neighborEntry.staticEntry = *staticEntry_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_neighbor_entry_l2idx_l2idx_matchrouteidx_matchrouteidx_interfaceid_up_ipv6_addr_63_32_interfaceid_low_ipv6_addr_31_0_staticentry_staticentry */

/*
 * rg add dhcpRequest entry
 */
cparser_result_t
cparser_cmd_rg_add_dhcpRequest_entry(
    cparser_context_t *context)
{
	//directly use: rg set dhcpRequest 
    DIAG_UTIL_PARAM_CHK();
	return CPARSER_NOT_OK;
}    /* end of cparser_cmd_rg_add_dhcprequest_entry */

/*
 * rg set dhcpRequest wan-intf-idx <UINT:wan_intf_idx>
 */
cparser_result_t
cparser_cmd_rg_set_dhcpRequest_wan_intf_idx_wan_intf_idx(
    cparser_context_t *context,
    uint32_t  *wan_intf_idx_ptr)
{
 	int ret;
    DIAG_UTIL_PARAM_CHK();
	ret = rtk_rg_dhcpRequest_set(*wan_intf_idx_ptr);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;
}    /* end of cparser_cmd_rg_set_dhcprequest_wan_intf_idx_wan_intf_idx */

/*
 * rg show dhcpClientInfo
 */
cparser_result_t
cparser_cmd_rg_show_dhcpClientInfo(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	_diag_dhcpClientInfoShow(&dhcpClient_info);
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_dhcpclientinfo */

/*
 * rg add dhcpClientInfo wan_intf_idx <UINT:wan_intf_idx>
 */
cparser_result_t
cparser_cmd_rg_add_dhcpClientInfo_wan_intf_idx_wan_intf_idx(
    cparser_context_t *context,
    uint32_t  *wan_intf_idx_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
	ret = rtk_rg_dhcpClientInfo_set(*wan_intf_idx_ptr, &dhcpClient_info);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_dhcpclientinfo_wan_intf_idx_wan_intf_idx */

/*
 * rg clear dhcpClientInfo
 */
cparser_result_t
cparser_cmd_rg_clear_dhcpClientInfo(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&dhcpClient_info, 0x0, sizeof(rtk_rg_ipDhcpClientInfo_t)); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_dhcpclientinfo */

/*
 * rg set dhcpClientInfo stauts <UINT:stauts> ip-version <UINT:ip_version> napt_enable <UINT:napt_enable> ip_addr <IPV4ADDR:ip_addr> ip_network_mask <IPV4ADDR:ip_network_mask> ipv4_default_gateway_on <UINT:ipv4_default_gateway_on> gateway_ipv4_addr <IPV4ADDR:gateway_ipv4_addr> mtu <UINT:mtu>
 */
cparser_result_t
cparser_cmd_rg_set_dhcpClientInfo_stauts_stauts_ip_version_ip_version_napt_enable_napt_enable_ip_addr_ip_addr_ip_network_mask_ip_network_mask_ipv4_default_gateway_on_ipv4_default_gateway_on_gateway_ipv4_addr_gateway_ipv4_addr_mtu_mtu(
    cparser_context_t *context,
    uint32_t  *stauts_ptr,
    uint32_t  *ip_version_ptr,
    uint32_t  *napt_enable_ptr,
    uint32_t  *ip_addr_ptr,
    uint32_t  *ip_network_mask_ptr,
    uint32_t  *ipv4_default_gateway_on_ptr,
    uint32_t  *gateway_ipv4_addr_ptr,
    uint32_t  *mtu_ptr)
{
	int32  ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
	dhcpClient_info.stauts= *stauts_ptr;
	dhcpClient_info.hw_info.ip_version=*ip_version_ptr;
	dhcpClient_info.hw_info.napt_enable=*napt_enable_ptr;
	dhcpClient_info.hw_info.ip_addr=*ip_addr_ptr;
	dhcpClient_info.hw_info.ip_network_mask=*ip_network_mask_ptr;

	dhcpClient_info.hw_info.ipv4_default_gateway_on=*ipv4_default_gateway_on_ptr;
    dhcpClient_info.hw_info.gateway_ipv4_addr=*gateway_ipv4_addr_ptr;
	dhcpClient_info.hw_info.mtu=*mtu_ptr;
	
	//gw_mac_auto_learn default setting enable
	dhcpClient_info.hw_info.gw_mac_auto_learn_for_ipv4=1;
	memset(dhcpClient_info.hw_info.gateway_mac_addr_for_ipv4.octet, 0x0, ETHER_ADDR_LEN);		
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_set_dhcpclientinfo_stauts_stauts_ip_version_ip_version_napt_enable_napt_enable_ip_addr_ip_addr_ip_network_mask_ip_network_mask_ipv4_default_gateway_on_ipv4_default_gateway_on_gateway_ipv4_addr_gateway_ipv4_addr_mtu_mtu */

/*
 * rg set dhcpClientInfo stauts <UINT:stauts> ip-version <UINT:ip_version> napt_enable <UINT:napt_enable> ip_addr <IPV4ADDR:ip_addr> ip_network_mask <IPV4ADDR:ip_network_mask> ipv4_default_gateway_on <UINT:ipv4_default_gateway_on> gateway_ipv4_addr <IPV4ADDR:gateway_ipv4_addr> mtu <UINT:mtu> gw_mac_auto_learn_for_ipv4 <UINT:gw_mac_auto_learn_for_ipv4> gateway_mac_addr_for_ipv4 <MACADDR:gateway_mac_addr_for_ipv4>
 */
cparser_result_t
cparser_cmd_rg_set_dhcpClientInfo_stauts_stauts_ip_version_ip_version_napt_enable_napt_enable_ip_addr_ip_addr_ip_network_mask_ip_network_mask_ipv4_default_gateway_on_ipv4_default_gateway_on_gateway_ipv4_addr_gateway_ipv4_addr_mtu_mtu_gw_mac_auto_learn_for_ipv4_gw_mac_auto_learn_for_ipv4_gateway_mac_addr_for_ipv4_gateway_mac_addr_for_ipv4(
    cparser_context_t *context,
    uint32_t  *stauts_ptr,
    uint32_t  *ip_version_ptr,
    uint32_t  *napt_enable_ptr,
    uint32_t  *ip_addr_ptr,
    uint32_t  *ip_network_mask_ptr,
    uint32_t  *ipv4_default_gateway_on_ptr,
    uint32_t  *gateway_ipv4_addr_ptr,
    uint32_t  *mtu_ptr,
    uint32_t  *gw_mac_auto_learn_for_ipv4_ptr,
    cparser_macaddr_t  *gateway_mac_addr_for_ipv4_ptr)
{
	int32  ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
	dhcpClient_info.stauts= *stauts_ptr;
	dhcpClient_info.hw_info.ip_version=*ip_version_ptr;
	dhcpClient_info.hw_info.napt_enable=*napt_enable_ptr;
	dhcpClient_info.hw_info.ip_addr=*ip_addr_ptr;
	dhcpClient_info.hw_info.ip_network_mask=*ip_network_mask_ptr;

	dhcpClient_info.hw_info.ipv4_default_gateway_on=*ipv4_default_gateway_on_ptr;
    dhcpClient_info.hw_info.gateway_ipv4_addr=*gateway_ipv4_addr_ptr;
	dhcpClient_info.hw_info.mtu=*mtu_ptr;
	
	dhcpClient_info.hw_info.gw_mac_auto_learn_for_ipv4=*gw_mac_auto_learn_for_ipv4_ptr;
	osal_memcpy(dhcpClient_info.hw_info.gateway_mac_addr_for_ipv4.octet, gateway_mac_addr_for_ipv4_ptr->octet, ETHER_ADDR_LEN);	
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_set_dhcpclientinfo_stauts_stauts_ip_version_ip_version_napt_enable_napt_enable_ip_addr_ip_addr_ip_network_mask_ip_network_mask_ipv4_default_gateway_on_ipv4_default_gateway_on_gateway_ipv4_addr_gateway_ipv4_addr_mtu_mtu_gw_mac_auto_learn_for_ipv4_gw_mac_auto_learn_for_ipv4_gateway_mac_addr_for_ipv4_gateway_mac_addr_for_ipv4 */


/*
 * rg set dhcpClientInfo-ipv6 ipv6_addr <IPV6ADDR:ipv6_addr> ipv6_mask_length <UINT:ipv6_mask_length> ipv6_default_gateway_on <UINT:ipv6_default_gateway_on> gateway_ipv6_addr <IPV6ADDR:gateway_ipv6_addr>
 */
cparser_result_t
cparser_cmd_rg_set_dhcpClientInfo_ipv6_ipv6_addr_ipv6_addr_ipv6_mask_length_ipv6_mask_length_ipv6_default_gateway_on_ipv6_default_gateway_on_gateway_ipv6_addr_gateway_ipv6_addr(
    cparser_context_t *context,
    char * *ipv6_addr_ptr,
    uint32_t  *ipv6_mask_length_ptr,
    uint32_t  *ipv6_default_gateway_on_ptr,
    char * *gateway_ipv6_addr_ptr)
{
	int32  ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&dhcpClient_info.hw_info.ipv6_addr.ipv6_addr[0], TOKEN_STR(4)), ret);
    dhcpClient_info.hw_info.ipv6_mask_length=*ipv6_mask_length_ptr;
    dhcpClient_info.hw_info.ipv6_default_gateway_on=*ipv6_default_gateway_on_ptr;
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&dhcpClient_info.hw_info.gateway_ipv6_addr.ipv6_addr[0], TOKEN_STR(10)), ret);
	
	//gw_mac_auto_learn default setting enable	
	dhcpClient_info.hw_info.gw_mac_auto_learn_for_ipv6=1;
	memset(dhcpClient_info.hw_info.gateway_mac_addr_for_ipv6.octet, 0x0, ETHER_ADDR_LEN);	
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_set_dhcpclientinfo_ipv6_ipv6_addr_ipv6_addr_ipv6_mask_length_ipv6_mask_length_ipv6_default_gateway_on_ipv6_default_gateway_on_gateway_ipv6_addr_gateway_ipv6_addr */

/*
 * rg set dhcpClientInfo-ipv6 ipv6_addr <IPV6ADDR:ipv6_addr> ipv6_mask_length <UINT:ipv6_mask_length> ipv6_default_gateway_on <UINT:ipv6_default_gateway_on> gateway_ipv6_addr <IPV6ADDR:gateway_ipv6_addr> gw_mac_auto_learn_for_ipv6 <UINT:gw_mac_auto_learn_for_ipv6> gateway_mac_addr_for_ipv6 <MACADDR:gateway_mac_addr_for_ipv6>
 */
cparser_result_t
cparser_cmd_rg_set_dhcpClientInfo_ipv6_ipv6_addr_ipv6_addr_ipv6_mask_length_ipv6_mask_length_ipv6_default_gateway_on_ipv6_default_gateway_on_gateway_ipv6_addr_gateway_ipv6_addr_gw_mac_auto_learn_for_ipv6_gw_mac_auto_learn_for_ipv6_gateway_mac_addr_for_ipv6_gateway_mac_addr_for_ipv6(
    cparser_context_t *context,
    char * *ipv6_addr_ptr,
    uint32_t  *ipv6_mask_length_ptr,
    uint32_t  *ipv6_default_gateway_on_ptr,
    char * *gateway_ipv6_addr_ptr,
    uint32_t  *gw_mac_auto_learn_for_ipv6_ptr,
    cparser_macaddr_t  *gateway_mac_addr_for_ipv6_ptr)
{
	int32  ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&dhcpClient_info.hw_info.ipv6_addr.ipv6_addr[0], TOKEN_STR(4)), ret);
    dhcpClient_info.hw_info.ipv6_mask_length=*ipv6_mask_length_ptr;
    dhcpClient_info.hw_info.ipv6_default_gateway_on=*ipv6_default_gateway_on_ptr;
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&dhcpClient_info.hw_info.gateway_ipv6_addr.ipv6_addr[0], TOKEN_STR(10)), ret);

	
	dhcpClient_info.hw_info.gw_mac_auto_learn_for_ipv6=*gw_mac_auto_learn_for_ipv6_ptr;
	osal_memcpy(dhcpClient_info.hw_info.gateway_mac_addr_for_ipv6.octet, gateway_mac_addr_for_ipv6_ptr->octet, ETHER_ADDR_LEN);
	
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_set_dhcpclientinfo_ipv6_ipv6_addr_ipv6_addr_ipv6_mask_length_ipv6_mask_length_ipv6_default_gateway_on_ipv6_default_gateway_on_gateway_ipv6_addr_gateway_ipv6_addr_gw_mac_auto_learn_for_ipv6_gw_mac_auto_learn_for_ipv6_gateway_mac_addr_for_ipv6_gateway_mac_addr_for_ipv6 */

/*
 * rg show binding
 */
cparser_result_t
cparser_cmd_rg_show_binding(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	_diag_bindingShow(&vlan_binding_info);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_binding */

/*
 * rg clear binding
 */
cparser_result_t
cparser_cmd_rg_clear_binding(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&vlan_binding_info, 0x0, sizeof(rtk_rg_vlanBinding_t)); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_binding */

/*
 * rg add binding entry
 */
cparser_result_t
cparser_cmd_rg_add_binding_entry(
    cparser_context_t *context)
{
	int ret;
	int index;
	
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	ret = rtk_rg_vlanBinding_add(&vlan_binding_info, &index);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_vlanBinding_add failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_vlanBinding_add[%d] success.\n",index);
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_binding_entry */

/*
 * rg get binding entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_get_binding_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	int index=*entry_index_ptr;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&vlan_binding_info, 0x0, sizeof(rtk_rg_vlanBinding_t)); 

	ret = rtk_rg_vlanBinding_find(&vlan_binding_info, &index);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get vlan_binding_info failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		if(index==*entry_index_ptr){	
			_diag_bindingShow(&vlan_binding_info);
		}
		else{
			diag_util_mprintf("vlan_binding_info[%d] is empty.\n",*entry_index_ptr);
			return CPARSER_NOT_OK;
		}
	}
	
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_get_binding_entry_entry_index */

/*
 * rg del binding entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_del_binding_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
  
	int ret;
	DIAG_UTIL_PARAM_CHK();
	
	ret = rtk_rg_vlanBinding_del(*entry_index_ptr);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_del_binding_entry_entry_index */

/*
 * rg set binding port_idx <UINT:port_idx> ingress_vid <UINT:ingress_vid> wan_intf_idx <UINT:wan_intf_idx>
 */
cparser_result_t
cparser_cmd_rg_set_binding_port_idx_port_idx_ingress_vid_ingress_vid_wan_intf_idx_wan_intf_idx(
    cparser_context_t *context,
    uint32_t  *port_idx_ptr,
    uint32_t  *ingress_vid_ptr,
    uint32_t  *wan_intf_idx_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	vlan_binding_info.port_idx = *port_idx_ptr;
	vlan_binding_info.ingress_vid = *ingress_vid_ptr;
	vlan_binding_info.wan_intf_idx = *wan_intf_idx_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_binding_port_idx_port_idx_ingress_vid_ingress_vid_wan_intf_idx_wan_intf_idx */

           


/*
 * rg show macfilter
 */
cparser_result_t
cparser_cmd_rg_show_macfilter(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	_diag_macFilterEntryShow(&macFilterEntry);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_macfilter */

/*
 * rg clear macfilter
 */
cparser_result_t
cparser_cmd_rg_clear_macfilter(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&macFilterEntry, 0x0, sizeof(rtk_rg_macFilterEntry_t)); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_macfilter */

/*
 * rg add macfilter entry
 */
cparser_result_t
cparser_cmd_rg_add_macfilter_entry(
    cparser_context_t *context)
{
	int ret;
	int index;
	
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	ret = rtk_rg_macFilter_add(&macFilterEntry,&index);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_macFilter_add failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_macFilter_add[%d] success.\n",index);
	}

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_add_macfilter_entry */

/*
 * rg get macfilter entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_get_macfilter_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	int index=*entry_index_ptr;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&macFilterEntry, 0x0, sizeof(rtk_rg_vlanBinding_t)); 

	ret = rtk_rg_macFilter_find(&macFilterEntry,&index);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_macFilter_find failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		if(index==*entry_index_ptr){	
			_diag_macFilterEntryShow(&macFilterEntry);
		}
		else{
			diag_util_mprintf("macFilterEntry[%d] is empty.\n",*entry_index_ptr);
			return CPARSER_NOT_OK;
		}
	}
	
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_get_macfilter_entry_entry_index */

/*
 * rg del macfilter entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_del_macfilter_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
  
	int ret;
	DIAG_UTIL_PARAM_CHK();
	
	ret = rtk_rg_macFilter_del(*entry_index_ptr);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_del_macfilter_entry_entry_index */

/*
 * rg set macfilter mac <MACADDR:mac> direct <UINT:direct>
 */
cparser_result_t
cparser_cmd_rg_set_macfilter_mac_mac_direct_direct(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *direct_ptr)
{
    DIAG_UTIL_PARAM_CHK();

	osal_memcpy(macFilterEntry.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
	macFilterEntry.direct =  *direct_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_macfilter_mac_mac_direct_direct */


/*
 * rg show virtualServer
 */
cparser_result_t
cparser_cmd_rg_show_virtualServer(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	_diag_virtualServerShow(&virtual_server);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_virtualserver */

/*
 * rg clear virtualServer
 */
cparser_result_t
cparser_cmd_rg_clear_virtualServer(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&virtual_server, 0x0, sizeof(rtk_rg_virtualServer_t)); 

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_virtualserver */

/*
 * rg add virtualServer entry
 */
cparser_result_t
cparser_cmd_rg_add_virtualServer_entry(
    cparser_context_t *context)
{
 	int ret;
	int index;
	
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	ret = rtk_rg_virtualServer_add(&virtual_server, &index);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_virtualServer_add failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_virtualServer_add[%d] success.\n",index);
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_virtualserver_entry */

/*
 * rg get virtualServer entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_get_virtualServer_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	int index=*entry_index_ptr;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&virtual_server, 0x0, sizeof(rtk_rg_virtualServer_t)); 

	ret = rtk_rg_virtualServer_find(&virtual_server,&index);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_virtualServer_find failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		if(index==*entry_index_ptr){	
			_diag_virtualServerShow(&virtual_server);
		}
		else{
			diag_util_mprintf("virtual_server[%d] is empty.\n",*entry_index_ptr);
			return CPARSER_NOT_OK;
		}
	}
	
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_get_virtualserver_entry_entry_index */

/*
 * rg del virtualServer entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_del_virtualServer_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	DIAG_UTIL_PARAM_CHK();
	
	ret = rtk_rg_virtualServer_del(*entry_index_ptr);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_del_virtualserver_entry_entry_index */

/*
 * rg set virtualServer is_tcp <UINT:is_tcp> wan_intf_idx <UINT:wan_intf_idx> gateway_port_start <UINT:gateway_port_start> local_ip <IPV4ADDR:local_ip> local_port_start <UINT:local_port_start> mappingPortRangeCnt <UINT:mappingPortRangeCnt> valid <UINT:valid>
 */
cparser_result_t
cparser_cmd_rg_set_virtualServer_is_tcp_is_tcp_wan_intf_idx_wan_intf_idx_gateway_port_start_gateway_port_start_local_ip_local_ip_local_port_start_local_port_start_mappingPortRangeCnt_mappingPortRangeCnt_valid_valid(
    cparser_context_t *context,
    uint32_t  *is_tcp_ptr,
    uint32_t  *wan_intf_idx_ptr,
    uint32_t  *gateway_port_start_ptr,
    uint32_t  *local_ip_ptr,
    uint32_t  *local_port_start_ptr,
    uint32_t  *mappingPortRangeCnt_ptr,
    uint32_t  *valid_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	virtual_server.is_tcp = *is_tcp_ptr;
	virtual_server.wan_intf_idx = *wan_intf_idx_ptr;
	virtual_server.gateway_port_start = *gateway_port_start_ptr;
	virtual_server.local_ip = *local_ip_ptr;
	virtual_server.local_port_start = *local_port_start_ptr;
	virtual_server.mappingPortRangeCnt = *mappingPortRangeCnt_ptr;
	virtual_server.valid = *valid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_virtualserver_is_tcp_is_tcp_wan_intf_idx_wan_intf_idx_gateway_port_start_gateway_port_start_local_ip_local_ip_local_port_start_local_port_start_mappingportrangecnt_mappingportrangecnt_valid_valid */


/*
 * rg show cvlan
 */
cparser_result_t
cparser_cmd_rg_show_cvlan(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	_diag_cvlanInfoShow(&cvlan_info);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_cvlan */

/*
 * rg clear cvlan
 */
cparser_result_t
cparser_cmd_rg_clear_cvlan(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&cvlan_info, 0x0, sizeof(rtk_rg_cvlan_info_t));

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_cvlan */

/*
 * rg add cvlan entry
 */
cparser_result_t
cparser_cmd_rg_add_cvlan_entry(
    cparser_context_t *context)
{
 	int ret;
	int index;
	
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	ret =  rtk_rg_cvlan_add(&cvlan_info);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_cvlan_add failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_cvlan_add[%d] success.\n",cvlan_info.vlanId);
	}

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_add_cvlan_entry */

/*
 * rg set cvlan vlanId <UINT:vlanId> isIVL <UINT:isIVL> memberPortMask <HEX:memberPortMask> untagPortMask <HEX:untagPortMask> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_rg_set_cvlan_vlanId_vlanId_isIVL_isIVL_memberPortMask_memberPortMask_untagPortMask_untagPortMask_priority_priority(
    cparser_context_t *context,
    uint32_t  *vlanId_ptr,
    uint32_t  *isIVL_ptr,
    uint32_t  *memberPortMask_ptr,
    uint32_t  *untagPortMask_ptr,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	cvlan_info.vlanId=*vlanId_ptr;
	cvlan_info.isIVL=*isIVL_ptr;
	cvlan_info.memberPortMask.portmask=*memberPortMask_ptr;
	cvlan_info.untagPortMask.portmask=*untagPortMask_ptr;
	cvlan_info.priority=*priority_ptr;
	
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_set_cvlan_vlanid_vlanid_isivl_isivl_memberportmask_memberportmask_untagportmask_untagportmask_priority_priority */


/*
 * rg del cvlan vlanId <UINT:vlanId>
 */
cparser_result_t
cparser_cmd_rg_del_cvlan_vlanId_vlanId(
    cparser_context_t *context,
    uint32_t  *vlanId_ptr)
{
	int ret;
	DIAG_UTIL_PARAM_CHK();
	
	ret = rtk_rg_cvlan_del(*vlanId_ptr);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

}    /* end of cparser_cmd_rg_del_cvlan_vlanid_vlanid */


/*
 * rg set softwareSourceAddrLearningLimit learningLimitNumber <UINT:learningLimitNumber> action <UINT:action>
 */
cparser_result_t
cparser_cmd_rg_set_softwareSourceAddrLearningLimit_learningLimitNumber_learningLimitNumber_action_action(
    cparser_context_t *context,
    uint32_t  *learningLimitNumber_ptr,
    uint32_t  *action_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	sa_learnLimit_info.learningLimitNumber=*learningLimitNumber_ptr;
	sa_learnLimit_info.action=*action_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_softwaresourceaddrlearninglimit_learninglimitnumber_learninglimitnumber_action_action */


/*
 * rg add softwareSourceAddrLearningLimit port_idx <UINT:port_idx>
 */
cparser_result_t
cparser_cmd_rg_add_softwareSourceAddrLearningLimit_port_idx_port_idx(
    cparser_context_t *context,
    uint32_t  *port_idx_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	ret = rtk_rg_softwareSourceAddrLearningLimit_set(sa_learnLimit_info, *port_idx_ptr);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_softwareSourceAddrLearningLimit_set failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add sa_learnLimit_info success.\n");
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_softwaresourceaddrlearninglimit_port_idx_port_idx */

/*
 * rg get softwareSourceAddrLearningLimit port_idx <UINT:port_idx>
 */
cparser_result_t
cparser_cmd_rg_get_softwareSourceAddrLearningLimit_port_idx_port_idx(
    cparser_context_t *context,
    uint32_t  *port_idx_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&sa_learnLimit_info, 0x0, sizeof(rtk_rg_saLearningLimitInfo_t)); 

	ret = rtk_rg_softwareSourceAddrLearningLimit_get(&sa_learnLimit_info,*port_idx_ptr);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_softwareSourceAddrLearningLimit_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{	
		diag_util_mprintf("learningLimitNumber: %d\n",sa_learnLimit_info.learningLimitNumber);
		diag_util_mprintf("action: %d\n",sa_learnLimit_info.action);
		return CPARSER_OK;
	}
	
}    /* end of cparser_cmd_rg_get_softwaresourceaddrlearninglimit_port_idx_port_idx */

/*
 * rg get softwareSourceAddrLearningLimit port_idx all
 */
cparser_result_t
cparser_cmd_rg_get_softwareSourceAddrLearningLimit_port_idx_all(
    cparser_context_t *context)
{
	int ret, port;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


	for(port=0;port<RTK_RG_MAC_PORT_MAX;port++){
		osal_memset(&sa_learnLimit_info, 0x0, sizeof(rtk_rg_saLearningLimitInfo_t)); 
		ret = rtk_rg_softwareSourceAddrLearningLimit_get(&sa_learnLimit_info,port);
		if(ret!=RT_ERR_RG_OK)
		{
			diag_util_printf("get rtk_rg_softwareSourceAddrLearningLimit_get(port=%d) failed. rg API return %d \n",port,ret);
		}
		else
		{	
			diag_util_printf("port [%d] ",port);
			diag_util_printf("learningLimitNumber: %d ",sa_learnLimit_info.learningLimitNumber);
			diag_util_printf("action: %d\n",sa_learnLimit_info.action);
		}
	}
	return CPARSER_OK;
}    /* end of cparser_cmd_rg_get_softwaresourceaddrlearninglimit_port_idx_all */


/*
 * rg set phyPortForceAbility force_disable_phy <UINT:force_disable_phy> valid <UINT:valid> speed <UINT:speed> duplex <UINT:duplex> flowCtrl <UINT:flowCtrl>
 */
cparser_result_t
cparser_cmd_rg_set_phyPortForceAbility_force_disable_phy_force_disable_phy_valid_valid_speed_speed_duplex_duplex_flowCtrl_flowCtrl(
    cparser_context_t *context,
    uint32_t  *force_disable_phy_ptr,
    uint32_t  *valid_ptr,
    uint32_t  *speed_ptr,
    uint32_t  *duplex_ptr,
    uint32_t  *flowCtrl_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	phyPort_ability.force_disable_phy=*force_disable_phy_ptr;
	phyPort_ability.valid=*valid_ptr;
	phyPort_ability.speed=*speed_ptr;
	phyPort_ability.duplex=*duplex_ptr;
	phyPort_ability.flowCtrl=*flowCtrl_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_phyportforceability_force_disable_phy_force_disable_phy_valid_valid_speed_speed_duplex_duplex_flowctrl_flowctrl */


/*
 * rg add phyPortForceAbility port_idx <UINT:port_idx>
 */
cparser_result_t
cparser_cmd_rg_add_phyPortForceAbility_port_idx_port_idx(
    cparser_context_t *context,
    uint32_t  *port_idx_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	ret =  rtk_rg_phyPortForceAbility_set(*port_idx_ptr,phyPort_ability);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_phyPortForceAbility_set failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_phyPortForceAbility_set success.\n");
	}

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_add_phyportforceability_port_idx_port_idx */


/*
 * rg get phyPortForceAbility port_idx <UINT:port>
 */
cparser_result_t
cparser_cmd_rg_get_phyPortForceAbility_port_idx_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&phyPort_ability, 0x0, sizeof(rtk_rg_phyPortAbilityInfo_t)); 

	ret = rtk_rg_phyPortForceAbility_get(*port_ptr,&phyPort_ability);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_phyPortForceAbility_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{	
		diag_util_mprintf("force_disable_phy: %d\n",phyPort_ability.force_disable_phy);
		diag_util_mprintf("valid: %d\n",phyPort_ability.valid);
		diag_util_mprintf("speed: %d\n",phyPort_ability.speed);
		diag_util_mprintf("duplex: %d\n",phyPort_ability.duplex);
		diag_util_mprintf("flowCtrl: %d\n",phyPort_ability.flowCtrl);
		return CPARSER_OK;
	}

}    /* end of cparser_cmd_rg_get_phyportforceability_port_idx_port */


/*
 * rg set portMirror monitorPort <UINT:monitorPort> enabledPortMask <HEX:enabledPortMask> direct <UINT:direct>
 */
cparser_result_t
cparser_cmd_rg_set_portMirror_monitorPort_monitorPort_enabledPortMask_enabledPortMask_direct_direct(
    cparser_context_t *context,
    uint32_t  *monitorPort_ptr,
    uint32_t  *enabledPortMask_ptr,
    uint32_t  *direct_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	portMirrorInfo.monitorPort=*monitorPort_ptr;
	portMirrorInfo.enabledPortMask.portmask=*enabledPortMask_ptr;
	portMirrorInfo.direct=*direct_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_portmirror_monitorport_monitorport_enabledportmask_enabledportmask_direct_direct */

/*
 * rg add portMirror entry
 */
cparser_result_t
cparser_cmd_rg_add_portMirror_entry(
    cparser_context_t *context)
{
	int ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	ret =  rtk_rg_portMirror_set(portMirrorInfo);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_portMirror_set failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_portMirror_set success.\n");
	}
	return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_portmirror_entry */

/*
 * rg get portMirror port <UINT:port>
 */
cparser_result_t
cparser_cmd_rg_get_portMirror_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&portMirrorInfo, 0x0, sizeof(rtk_rg_portMirrorInfo_t)); 

	ret = rtk_rg_portMirror_get(&portMirrorInfo);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_portMirror_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{	
		diag_util_mprintf("monitorPort: %d\n",portMirrorInfo.monitorPort);
		diag_util_mprintf("speeenabledPortMaskd: 0x%x\n",portMirrorInfo.enabledPortMask);
		diag_util_mprintf("direct: %d\n",portMirrorInfo.direct);
		return CPARSER_OK;
	}

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_get_portmirror_port_port */

/*
 * rg add dosPortMaskEnable portmask <HEX:portmask>
 */
cparser_result_t
cparser_cmd_rg_add_dosPortMaskEnable_portmask_portmask(
    cparser_context_t *context,
    uint32_t  *portmask_ptr)
{
 	int ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	ret = rtk_rg_dosPortMaskEnable_set(*portmask_ptr);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_dosPortMaskEnable_set failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_dosPortMaskEnable_set success.\n");
		return CPARSER_OK;
	}
}    /* end of cparser_cmd_rg_add_dosportmaskenable_portmask_portmask */

/*
 * rg get dosPortMaskEnable
 */
cparser_result_t
cparser_cmd_rg_get_dosPortMaskEnable(
    cparser_context_t *context)
{
	int ret;
	rtk_rg_mac_portmask_t dos_port_mask;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	ret = rtk_rg_dosPortMaskEnable_get(&dos_port_mask);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_dosPortMaskEnable_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{	
		diag_util_mprintf("dos_port_mask: 0x%x\n",dos_port_mask.portmask);
		return CPARSER_OK;
	}

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_get_dosportmaskenable */

/*
 * rg add dosType dos_type <UINT:dos_type> dos_enabled <UINT:dos_enabled> dos_action  <UINT:dos_action>
 */
cparser_result_t
cparser_cmd_rg_add_dosType_dos_type_dos_type_dos_enabled_dos_enabled_dos_action_dos_action(
    cparser_context_t *context,
    uint32_t  *dos_type_ptr,
    uint32_t  *dos_enabled_ptr,
    uint32_t  *dos_action_ptr)
{
 	int ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	ret = rtk_rg_dosType_set(*dos_type_ptr,*dos_enabled_ptr,*dos_action_ptr);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_dosType_set failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_dosType_set success.\n");
		return CPARSER_OK;
	}

}    /* end of cparser_cmd_rg_add_dostype_dos_type_dos_type_dos_enabled_dos_enabled_dos_action_dos_action */

/*
 * rg get dosType dos_type <UINT:dos_type>
 */
cparser_result_t
cparser_cmd_rg_get_dosType_dos_type_dos_type(
    cparser_context_t *context,
    uint32_t  *dos_type_ptr)
{
	int ret;
	rtk_rg_dos_type_t dos_type;
	int dos_enabled;
	rtk_rg_dos_action_t dos_action;
	
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	dos_type = *dos_type_ptr;
	ret = rtk_rg_dosType_get(dos_type,&dos_enabled,&dos_action);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_dosType_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{	
		diag_util_mprintf("dos_type: %d\n",dos_type);
		diag_util_mprintf("dos_enabled: %d\n",dos_enabled);
		diag_util_mprintf("dos_action: %d\n",dos_action);
		return CPARSER_OK;
	}

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_get_dostype_dos_type_dos_type */

/*
 * rg add dosFloodType dos_type <UINT:dos_type> dos_enabled <UINT:dos_enabled> dos_action  <UINT:dos_action> dos_threshold <UINT:dos_threshold>
 */
cparser_result_t
cparser_cmd_rg_add_dosFloodType_dos_type_dos_type_dos_enabled_dos_enabled_dos_action_dos_action_dos_threshold_dos_threshold(
    cparser_context_t *context,
    uint32_t  *dos_type_ptr,
    uint32_t  *dos_enabled_ptr,
    uint32_t  *dos_action_ptr,
    uint32_t  *dos_threshold_ptr)
{
 	int ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	ret = rtk_rg_dosFloodType_set(*dos_type_ptr,*dos_enabled_ptr,*dos_action_ptr,*dos_threshold_ptr);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_dosFloodType_set failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_dosFloodType_set success.\n");
		return CPARSER_OK;
	}

}    /* end of cparser_cmd_rg_add_dosfloodtype_dos_type_dos_type_dos_enabled_dos_enabled_dos_action_dos_action_dos_threshold_dos_threshold */

/*
 * rg get dosFloodType dos_type <UINT:dos_type>
 */
cparser_result_t
cparser_cmd_rg_get_dosFloodType_dos_type_dos_type(
    cparser_context_t *context,
    uint32_t  *dos_type_ptr)
{
	int ret;
	rtk_rg_dos_type_t dos_type;
	int dos_enabled;
	rtk_rg_dos_action_t dos_action;
	int dos_threshold;

	
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
	dos_type = *dos_type_ptr;
	ret = rtk_rg_dosFloodType_get(dos_type,&dos_enabled,&dos_action,&dos_threshold);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_dosFloodType_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{	
		diag_util_mprintf("dos_type: %d\n",dos_type);
		diag_util_mprintf("dos_enabled: %d\n",dos_enabled);
		diag_util_mprintf("dos_action: %d\n",dos_action);
		diag_util_mprintf("dos_threshold: %d\n",dos_threshold);
		return CPARSER_OK;
	}

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_get_dosfloodtype_dos_type_dos_type */

/*
 * rg add portEgrBandwidthCtrlRate port <UINT:port> rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_rg_add_portEgrBandwidthCtrlRate_port_port_rate_rate(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    uint32_t  *rate_ptr)
{
 	int ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	ret =  rtk_rg_portEgrBandwidthCtrlRate_set(*port_ptr, *rate_ptr);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("netfilter:add rtk_rate_portEgrBandwidthCtrlRate_set failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rate_portEgrBandwidthCtrlRate_set success.\n");
		return CPARSER_OK;
	}

}    /* end of cparser_cmd_rg_add_portegrbandwidthctrlrate_port_port_rate_rate */

/*
 * rg add portIgrBandwidthCtrlRate port <UINT:port> rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_rg_add_portIgrBandwidthCtrlRate_port_port_rate_rate(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    uint32_t  *rate_ptr)
{
 	int ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	ret =  rtk_rg_portIgrBandwidthCtrlRate_set(*port_ptr, *rate_ptr);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("netfilter:add rtk_rg_portIgrBandwidthCtrlRate_set failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_portIgrBandwidthCtrlRate_set success.\n");
		return CPARSER_OK;
	}

}    /* end of cparser_cmd_rg_add_portigrbandwidthctrlrate_port_port_rate_rate */

/*
 * rg get portEgrBandwidthCtrlRate port <UINT:port>
 */
cparser_result_t
cparser_cmd_rg_get_portEgrBandwidthCtrlRate_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
	int ret;
	unsigned int rate;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	ret = rtk_rg_portEgrBandwidthCtrlRate_get(*port_ptr,&rate);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_portEgrBandwidthCtrlRate_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{	
		diag_util_mprintf("rate: %d\n",rate);
		return CPARSER_OK;
	}

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_get_portegrbandwidthctrlrate_port_port */

/*
 * rg get portIgrBandwidthCtrlRate port <UINT:port>
 */
cparser_result_t
cparser_cmd_rg_get_portIgrBandwidthCtrlRate_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
	int ret;
	unsigned int rate;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	ret = rtk_rg_portIgrBandwidthCtrlRate_get(*port_ptr,&rate);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_portIgrBandwidthCtrlRate_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{	
		diag_util_mprintf("rate: %d\n",rate);
		return CPARSER_OK;
	}

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_get_portigrbandwidthctrlrate_port_port */


/*
 * rg show upnpConnection
 */
cparser_result_t
cparser_cmd_rg_show_upnpConnection(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	_diag_upnpInfoShow(&upnpInfo);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_upnpconnection */

/*
 * rg clear upnpConnection
 */
cparser_result_t
cparser_cmd_rg_clear_upnpConnection(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&upnpInfo, 0x0, sizeof(rtk_rg_upnpConnection_t));

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_upnpconnection */

/*
 * rg add upnpConnection entry
 */
cparser_result_t
cparser_cmd_rg_add_upnpConnection_entry(
    cparser_context_t *context)
{
 	int ret;
	int index;
	
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	ret =  rtk_rg_upnpConnection_add(&upnpInfo, &index);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_upnpConnection_add failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_upnpConnection_add[%d] success.\n",index);
	}

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_add_upnpconnection_entry */

/*
 * rg get upnpConnection entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_get_upnpConnection_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	int index=*entry_index_ptr;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&upnpInfo, 0x0, sizeof(rtk_rg_upnpConnection_t));

	ret = rtk_rg_upnpConnection_find(&upnpInfo, &index);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_upnpConnection_find failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		if(index==*entry_index_ptr){	
			_diag_upnpInfoShow(&upnpInfo);
		}
		else{
			diag_util_mprintf("rtk_rg_upnpConnection_find[%d] is empty.\n",*entry_index_ptr);
			return CPARSER_NOT_OK;
		}
	}
	
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_get_upnpconnection_entry_entry_index */

/*
 * rg del upnpConnection entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_del_upnpConnection_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	DIAG_UTIL_PARAM_CHK();
	
	ret = rtk_rg_upnpConnection_del(*entry_index_ptr);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_del_upnpconnection_entry_entry_index */

/*
 * rg set upnpConnection is_tcp <UINT:is_tcp> valid <UINT:valid> wan_intf_idx <UINT:wan_intf_idx> gateway_port <UINT:gateway_port> local_ip <IPV4ADDR:local_ip> local_port <UINT:local_port> limit_remote_ip <UINT:limit_remote_ip> limit_remote_port <UINT:limit_remote_port> remote_ip <IPV4ADDR:remote_ip> remote_port <UINT:remote_port> type <UINT:type> timeout <UINT:timeout>
 */
cparser_result_t
cparser_cmd_rg_set_upnpConnection_is_tcp_is_tcp_valid_valid_wan_intf_idx_wan_intf_idx_gateway_port_gateway_port_local_ip_local_ip_local_port_local_port_limit_remote_ip_limit_remote_ip_limit_remote_port_limit_remote_port_remote_ip_remote_ip_remote_port_remote_port_type_type_timeout_timeout(
    cparser_context_t *context,
    uint32_t  *is_tcp_ptr,
    uint32_t  *valid_ptr,
    uint32_t  *wan_intf_idx_ptr,
    uint32_t  *gateway_port_ptr,
    uint32_t  *local_ip_ptr,
    uint32_t  *local_port_ptr,
    uint32_t  *limit_remote_ip_ptr,
    uint32_t  *limit_remote_port_ptr,
    uint32_t  *remote_ip_ptr,
    uint32_t  *remote_port_ptr,
    uint32_t  *type_ptr,
    uint32_t  *timeout_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	upnpInfo.is_tcp=*is_tcp_ptr;
	upnpInfo.valid=*valid_ptr;
	upnpInfo.wan_intf_idx=*wan_intf_idx_ptr;
	upnpInfo.gateway_port=*gateway_port_ptr;
	upnpInfo.local_ip=*local_ip_ptr;
	upnpInfo.local_port=*local_port_ptr;
	upnpInfo.limit_remote_ip=*limit_remote_ip_ptr;
	upnpInfo.limit_remote_port=*limit_remote_port_ptr;
	upnpInfo.remote_ip=*remote_ip_ptr;
	upnpInfo.remote_port=*remote_port_ptr;
	upnpInfo.type=*type_ptr;
	upnpInfo.timeout=*timeout_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_upnpconnection_is_tcp_is_tcp_valid_valid_wan_intf_idx_wan_intf_idx_gateway_port_gateway_port_local_ip_local_ip_local_port_local_port_limit_remote_ip_limit_remote_ip_limit_remote_port_limit_remote_port_remote_ip_remote_ip_remote_port_remote_port_type_type_timeout_timeout */


/*
 * rg set dmzHost enabled <UINT:enabled> private_ip <IPV4ADDR:private_ip>
 */
cparser_result_t
cparser_cmd_rg_set_dmzHost_enabled_enabled_private_ip_private_ip(
    cparser_context_t *context,
    uint32_t  *enabled_ptr,
    uint32_t  *private_ip_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&dmz_info, 0x0, sizeof(rtk_rg_dmzInfo_t));
	dmz_info.enabled = *enabled_ptr;
	dmz_info.private_ip = *private_ip_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_dmzhost_enabled_enabled_private_ip_private_ip */

/*
 * rg add dmzHost wan_intf_idx <UINT:wan_intf_idx>
 */
cparser_result_t
cparser_cmd_rg_add_dmzHost_wan_intf_idx_wan_intf_idx(
    cparser_context_t *context,
    uint32_t  *wan_intf_idx_ptr)
{
	int ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	ret = rtk_rg_dmzHost_set(*wan_intf_idx_ptr,&dmz_info);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_dmzHost_set failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_dmzHost_set success.\n");
		return CPARSER_OK;
	}
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_dmzhost_wan_intf_idx_wan_intf_idx */

/*
 * rg get dmzHost wan_intf_idx <UINT:wan_intf_idx>
 */
cparser_result_t
cparser_cmd_rg_get_dmzHost_wan_intf_idx_wan_intf_idx(
    cparser_context_t *context,
    uint32_t  *wan_intf_idx_ptr)
{
	int ret;
	rtk_rg_dmzInfo_t dmz_info;
	osal_memset(&dmz_info, 0x0, sizeof(rtk_rg_dmzInfo_t));

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
	
	ret = rtk_rg_dmzHost_get(*wan_intf_idx_ptr,&dmz_info);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_dmzHost_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{	
		diag_util_mprintf("enabled: %d\n",dmz_info.enabled);
		diag_util_mprintf("private_ip: 0x%x\n",dmz_info.private_ip);
		return CPARSER_OK;
	}

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_get_dmzhost_wan_intf_idx_wan_intf_idx */

/*
 * rg show stormControl
 */
cparser_result_t
cparser_cmd_rg_show_stormControl(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	_diag_stormControlInfoShow(&stormInfo);

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_show_stormcontrol */

/*
 * rg clear stormControl
 */
cparser_result_t
cparser_cmd_rg_clear_stormControl(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&stormInfo, 0x0, sizeof(rtk_rg_stormControlInfo_t));

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_stormcontrol */

/*
 * rg add stormControl entry
 */
cparser_result_t
cparser_cmd_rg_add_stormControl_entry(
    cparser_context_t *context)
{
 	int ret;
	int index;
	
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	ret =  rtk_rg_stormControl_add(&stormInfo, &index);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_stormControl_add failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_stormControl_add[%d] success.\n",index);
	}

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_add_stormcontrol_entry */

/*
 * rg get stormControl entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_get_stormControl_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	int index=*entry_index_ptr;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&stormInfo, 0x0, sizeof(rtk_rg_stormControlInfo_t));

	ret = rtk_rg_stormControl_find(&stormInfo, &index);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_stormControl_find failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		if(index==*entry_index_ptr){	
			_diag_stormControlInfoShow(&stormInfo);
		}
		else{
			diag_util_mprintf("rtk_rg_stormControl_find[%d] is empty.\n",*entry_index_ptr);
			return CPARSER_NOT_OK;
		}
	}
	
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_get_stormcontrol_entry_entry_index */

/*
 * rg del stormControl entry <UINT:entry_index>
 */
cparser_result_t
cparser_cmd_rg_del_stormControl_entry_entry_index(
    cparser_context_t *context,
    uint32_t  *entry_index_ptr)
{
	int ret;
	DIAG_UTIL_PARAM_CHK();
	
	ret = rtk_rg_stormControl_del(*entry_index_ptr);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_del_stormcontrol_entry_entry_index */

/*
 * rg set stormControl valid <UINT:valid> port <UINT:port> stormType <UINT:stormType> meterIdx <UINT:meterIdx>
 */
cparser_result_t
cparser_cmd_rg_set_stormControl_valid_valid_port_port_stormType_stormType_meterIdx_meterIdx(
    cparser_context_t *context,
    uint32_t  *valid_ptr,
    uint32_t  *port_ptr,
    uint32_t  *stormType_ptr,
    uint32_t  *meterIdx_ptr)
{
    DIAG_UTIL_PARAM_CHK();
	stormInfo.valid=*valid_ptr;
	stormInfo.port=*port_ptr;
	stormInfo.stormType=*stormType_ptr;
	stormInfo.meterIdx=*meterIdx_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_stormcontrol_valid_valid_port_port_stormtype_stormtype_meteridx_meteridx */

/*
 * rg add shareMeter index <UINT:index> rate <UINT:rate> ifgInclude <UINT:ifgInclude>
 */
cparser_result_t
cparser_cmd_rg_add_shareMeter_index_index_rate_rate_ifgInclude_ifgInclude(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *rate_ptr,
    uint32_t  *ifgInclude_ptr)
{
	int ret;
	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	ret = rtk_rg_shareMeter_set(*index_ptr, *rate_ptr, *ifgInclude_ptr);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_shareMeter_set failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_shareMeter_set success.\n");
		return CPARSER_OK;
	}

}    /* end of cparser_cmd_rg_add_sharemeter_index_index_rate_rate_ifginclude_ifginclude */

/*
 * rg get shareMeter index <UINT:index>
 */
cparser_result_t
cparser_cmd_rg_get_shareMeter_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int ret;
	uint32 rate;
	rtk_rg_enable_t ifgInclude;


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
	
	ret = rtk_rg_shareMeter_get(*index_ptr,&rate,&ifgInclude);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_shareMeter_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{	
		diag_util_mprintf("rate: %d\n",rate);
		diag_util_mprintf("ifgInclude: 0x%x\n",ifgInclude);
		return CPARSER_OK;
	}

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_get_sharemeter_index_index */


/*
 * rg add qosSchedulingQueue q_weight_0 <UINT:q_weight_0> q_weight_1 <UINT:q_weight_1> q_weight_2 <UINT:q_weight_2> q_weight_3 <UINT:q_weight_3> q_weight_4 <UINT:q_weight_4> q_weight_5 <UINT:q_weight_5> q_weight_6 <UINT:q_weight_6> q_weight_7 <UINT:q_weight_7> port <UINT:port>
 */
cparser_result_t
cparser_cmd_rg_add_qosSchedulingQueue_q_weight_0_q_weight_0_q_weight_1_q_weight_1_q_weight_2_q_weight_2_q_weight_3_q_weight_3_q_weight_4_q_weight_4_q_weight_5_q_weight_5_q_weight_6_q_weight_6_q_weight_7_q_weight_7_port_port(
    cparser_context_t *context,
    uint32_t  *q_weight_0_ptr,
    uint32_t  *q_weight_1_ptr,
    uint32_t  *q_weight_2_ptr,
    uint32_t  *q_weight_3_ptr,
    uint32_t  *q_weight_4_ptr,
    uint32_t  *q_weight_5_ptr,
    uint32_t  *q_weight_6_ptr,
    uint32_t  *q_weight_7_ptr,
    uint32_t  *port_ptr)
{
  	int ret;	
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	q_weight.weights[0]=*q_weight_0_ptr;
	q_weight.weights[1]=*q_weight_1_ptr;
	q_weight.weights[2]=*q_weight_2_ptr;
	q_weight.weights[3]=*q_weight_3_ptr;
	q_weight.weights[4]=*q_weight_4_ptr;
	q_weight.weights[5]=*q_weight_5_ptr;
	q_weight.weights[6]=*q_weight_6_ptr;
	q_weight.weights[7]=*q_weight_7_ptr;

	ret = rtk_rg_qos_schedulingQueue_set(*port_ptr, q_weight);
	if(ret!=RT_ERR_RG_OK){
	  diag_util_mprintf("add rtk_rg_qos_schedulingQueue_set failed. rg API return %d \n",ret);
	  return CPARSER_NOT_OK;
	}
	else{
	  diag_util_mprintf("add rtk_rg_qos_schedulingQueue_set success.\n");
	  return CPARSER_OK;
	}
		
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_qosschedulingqueue_q_weight_0_q_weight_0_q_weight_1_q_weight_1_q_weight_2_q_weight_2_q_weight_3_q_weight_3_q_weight_4_q_weight_4_q_weight_5_q_weight_5_q_weight_6_q_weight_6_q_weight_7_q_weight_7_port_port */



/*
 * rg get qosSchedulingQueue port <UINT:port>
 */
cparser_result_t
cparser_cmd_rg_get_qosSchedulingQueue_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
	int ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
	osal_memset(&q_weight, 0x0, sizeof(rtk_rg_qos_queue_weights_t));
	
	ret = rtk_rg_qos_schedulingQueue_get(*port_ptr,&q_weight);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_qos_schedulingQueue_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{	
		diag_util_mprintf("q_weight[0]: %d\n",q_weight.weights[0]);
		diag_util_mprintf("q_weight[1]: %d\n",q_weight.weights[1]);
		diag_util_mprintf("q_weight[2]: %d\n",q_weight.weights[2]);
		diag_util_mprintf("q_weight[3]: %d\n",q_weight.weights[3]);
		diag_util_mprintf("q_weight[4]: %d\n",q_weight.weights[4]);
		diag_util_mprintf("q_weight[5]: %d\n",q_weight.weights[5]);
		diag_util_mprintf("q_weight[6]: %d\n",q_weight.weights[6]);
		diag_util_mprintf("q_weight[7]: %d\n",q_weight.weights[7]);
		
		return CPARSER_OK;
	}

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_get_qosschedulingqueue_port_port */

 
/*
* rg add qosPriMap pri0-to-queue <UINT:queueid0> pri1-to-queue <UINT:queueid1>  pri2-to-queue <UINT:queueid2> pri3-to-queue <UINT:queueid3> pri4-to-queue <UINT:queueid4> pri5-to-queue <UINT:queueid5> pri6-to-queue <UINT:queueid6> pri7-to-queue <UINT:queueid7>
*/
cparser_result_t
cparser_cmd_rg_add_qosPriMap_pri0_to_queue_queueid0_pri1_to_queue_queueid1_pri2_to_queue_queueid2_pri3_to_queue_queueid3_pri4_to_queue_queueid4_pri5_to_queue_queueid5_pri6_to_queue_queueid6_pri7_to_queue_queueid7(
    cparser_context_t *context,
    uint32_t  *queueid0_ptr,
    uint32_t  *queueid1_ptr,
    uint32_t  *queueid2_ptr,
    uint32_t  *queueid3_ptr,
    uint32_t  *queueid4_ptr,
    uint32_t  *queueid5_ptr,
    uint32_t  *queueid6_ptr,
    uint32_t  *queueid7_ptr)
{

	int ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	memset(&pri2qid,0,sizeof(pri2qid));
	pri2qid.pri2queue[0]=*queueid0_ptr;
	pri2qid.pri2queue[1]=*queueid1_ptr;
	pri2qid.pri2queue[2]=*queueid2_ptr;
	pri2qid.pri2queue[3]=*queueid3_ptr;
	pri2qid.pri2queue[4]=*queueid4_ptr;
	pri2qid.pri2queue[5]=*queueid5_ptr;
	pri2qid.pri2queue[6]=*queueid6_ptr;
	pri2qid.pri2queue[7]=*queueid7_ptr;
	
	ret = rtk_rg_qosPriMap_set(pri2qid);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_qosPriMap_set failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_qosPriMap_set success.\n");
		return CPARSER_OK;
	}
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_add_qosprimap_pri0_to_queue_queueid0_pri1_to_queue_queueid1_pri2_to_queue_queueid2_pri3_to_queue_queueid3_pri4_to_queue_queueid4_pri5_to_queue_queueid5_pri6_to_queue_queueid6_pri7_to_queue_queueid7 */

/*
 * rg get qosPriMap
 */
cparser_result_t
cparser_cmd_rg_get_qosPriMap(
    cparser_context_t *context)
{
	int ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&pri2qid, 0x0, sizeof(pri2qid));

	ret = rtk_rg_qosPriMap_get(&pri2qid);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_qosPriMap_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{

		diag_util_mprintf("internalPri[0]-to-queue[%d]\n",pri2qid.pri2queue[0]);
		diag_util_mprintf("internalPri[1]-to-queue[%d]\n",pri2qid.pri2queue[1]);
		diag_util_mprintf("internalPri[2]-to-queue[%d]\n",pri2qid.pri2queue[2]);
		diag_util_mprintf("internalPri[3]-to-queue[%d]\n",pri2qid.pri2queue[3]);
		diag_util_mprintf("internalPri[4]-to-queue[%d]\n",pri2qid.pri2queue[4]);
		diag_util_mprintf("internalPri[5]-to-queue[%d]\n",pri2qid.pri2queue[5]);
		diag_util_mprintf("internalPri[6]-to-queue[%d]\n",pri2qid.pri2queue[6]);
		diag_util_mprintf("internalPri[7]-to-queue[%d]\n",pri2qid.pri2queue[7]);
		
		return CPARSER_OK;
		
	}
	
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_get_qosprimap */

/*
 * rg add qosPriSelGroup weight_of_portBased <UINT:weight_of_portBased> weight_of_dot1q <UINT:weight_of_dot1q> weight_of_dscp <UINT:weight_of_dscp> weight_of_acl <UINT:weight_of_acl> weight_of_lutFwd <UINT:weight_of_lutFwd>  weight_of_saBaed <UINT:weight_of_saBaed> weight_of_vlanBased <UINT:weight_of_vlanBased> weight_of_svlanBased <UINT:weight_of_svlanBased> weight_of_l4Based <UINT:weight_of_l4Based>
 */
cparser_result_t
cparser_cmd_rg_add_qosPriSelGroup_weight_of_portBased_weight_of_portBased_weight_of_dot1q_weight_of_dot1q_weight_of_dscp_weight_of_dscp_weight_of_acl_weight_of_acl_weight_of_lutFwd_weight_of_lutFwd_weight_of_saBaed_weight_of_saBaed_weight_of_vlanBased_weight_of_vlanBased_weight_of_svlanBased_weight_of_svlanBased_weight_of_l4Based_weight_of_l4Based(
    cparser_context_t *context,
    uint32_t  *weight_of_portBased_ptr,
    uint32_t  *weight_of_dot1q_ptr,
    uint32_t  *weight_of_dscp_ptr,
    uint32_t  *weight_of_acl_ptr,
    uint32_t  *weight_of_lutFwd_ptr,
    uint32_t  *weight_of_saBaed_ptr,
    uint32_t  *weight_of_vlanBased_ptr,
    uint32_t  *weight_of_svlanBased_ptr,
    uint32_t  *weight_of_l4Based_ptr)
{
	int ret;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
    weightOfPriSel.weight_of_portBased=*weight_of_portBased_ptr;
    weightOfPriSel.weight_of_dot1q=*weight_of_dot1q_ptr;
    weightOfPriSel.weight_of_dscp=*weight_of_dscp_ptr;
    weightOfPriSel.weight_of_acl=*weight_of_acl_ptr;
    weightOfPriSel.weight_of_lutFwd=*weight_of_lutFwd_ptr;
   	weightOfPriSel.weight_of_saBaed=*weight_of_saBaed_ptr;
    weightOfPriSel.weight_of_vlanBased=*weight_of_vlanBased_ptr;
    weightOfPriSel.weight_of_svlanBased=*weight_of_svlanBased_ptr;
    weightOfPriSel.weight_of_l4Based=*weight_of_l4Based_ptr;
	
	ret = rtk_rg_qosPriSelGroup_set(weightOfPriSel);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_qosPriSelGroup_set failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_qosPriSelGroup_set success.\n");
		return CPARSER_OK;
	}
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_add_qospriselgroup_weight_of_portbased_weight_of_portbased_weight_of_dot1q_weight_of_dot1q_weight_of_dscp_weight_of_dscp_weight_of_acl_weight_of_acl_weight_of_lutfwd_weight_of_lutfwd_weight_of_sabaed_weight_of_sabaed_weight_of_vlanbased_weight_of_vlanbased_weight_of_svlanbased_weight_of_svlanbased_weight_of_l4based_weight_of_l4based */

/*
 * rg get qosPriSelGroup
 */
cparser_result_t
cparser_cmd_rg_get_qosPriSelGroup(
    cparser_context_t *context)
{
	int ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&weightOfPriSel, 0x0, sizeof(weightOfPriSel));

	ret = rtk_rg_qosPriSelGroup_get(&weightOfPriSel);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_qosPriSelGroup_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		diag_util_mprintf("weight_of_portBased: %d\n",weightOfPriSel.weight_of_portBased);
		diag_util_mprintf("weight_of_dot1q: %d\n",weightOfPriSel.weight_of_dot1q);
		diag_util_mprintf("weight_of_dscp: %d\n",weightOfPriSel.weight_of_dscp);
		diag_util_mprintf("weight_of_acl: %d\n",weightOfPriSel.weight_of_acl);
		diag_util_mprintf("weight_of_lutFwd: %d\n",weightOfPriSel.weight_of_lutFwd);
		diag_util_mprintf("weight_of_saBaed: %d\n",weightOfPriSel.weight_of_saBaed);
		diag_util_mprintf("weight_of_vlanBased: %d\n",weightOfPriSel.weight_of_vlanBased);
		diag_util_mprintf("weight_of_svlanBased: %d\n",weightOfPriSel.weight_of_svlanBased);
		diag_util_mprintf("weight_of_l4Based: %d\n",weightOfPriSel.weight_of_l4Based);

		return CPARSER_OK;
		
	}
	

}    /* end of cparser_cmd_rg_get_qospriselgroup */

/*
 * rg add qosDscpPriRemapGroup dscp <UINT:dscp> intPri <UINT:intPri>
 */
cparser_result_t
cparser_cmd_rg_add_qosDscpPriRemapGroup_dscp_dscp_intPri_intPri(
    cparser_context_t *context,
    uint32_t  *dscp_ptr,
    uint32_t  *intPri_ptr)
{
	int ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();	
	ret = rtk_rg_qosDscpPriRemapGroup_set(*dscp_ptr,*intPri_ptr);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_qosDscpPriRemapGroup_set failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_qosDscpPriRemapGroup_set success.\n");
		return CPARSER_OK;
	}
    return CPARSER_OK;

}    /* end of cparser_cmd_rg_add_qosdscppriremapgroup_dscp_dscp_intpri_intpri */

/*
 * rg get qosDscpPriRemapGroup dscp <UINT:dscp>
 */
cparser_result_t
cparser_cmd_rg_get_qosDscpPriRemapGroup_dscp_dscp(
    cparser_context_t *context,
    uint32_t  *dscp_ptr)
{
	int ret;
	int intPri;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	ret = rtk_rg_qosDscpPriRemapGroup_get(*dscp_ptr,&intPri);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_qosDscpPriRemapGroup_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		diag_util_mprintf("dscp[%d] mapping to internal-pri[%d]: %d\n",*dscp_ptr,intPri);
		return CPARSER_OK;	
	}

}    /* end of cparser_cmd_rg_get_qosdscppriremapgroup_dscp_dscp */

/*
 * rg add qosPortPri port <UINT:port> intPri <UINT:intPri>
 */
cparser_result_t
cparser_cmd_rg_add_qosPortPri_port_port_intPri_intPri(
    cparser_context_t *context,
    uint32_t  *port_ptr,
    uint32_t  *intPri_ptr)
{
 	int ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();	
	ret = rtk_rg_qosPortPri_set(*port_ptr,*intPri_ptr);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_qosPortPri_set failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_qosPortPri_set success.\n");
		return CPARSER_OK;
	}
    return CPARSER_OK;
}    /* end of cparser_cmd_rg_add_qosportpri_port_port_intpri_intpri */

/*
 * rg get qosPortPri port <UINT:port>
 */
cparser_result_t
cparser_cmd_rg_get_qosPortPri_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
	int ret;
	int intPri;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	ret = rtk_rg_qosPortPri_get(*port_ptr,&intPri);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_qosPortPri_get failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		diag_util_mprintf("port[%d] mapping to internal-pri[%d]: %d\n",*port_ptr,intPri);
		return CPARSER_OK;	
	}

}    /* end of cparser_cmd_rg_get_qosportpri_port_port */

/*
 * rg show multicastFlow
 */
cparser_result_t
cparser_cmd_rg_show_multicastFlow(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();
	_diag_multicastFlowShow(&mcFlow);

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_show_multicastflow */

/*
 * rg clear multicastFlow
 */
cparser_result_t
cparser_cmd_rg_clear_multicastFlow(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
	osal_memset(&mcFlow, 0x0, sizeof(rtk_rg_multicastFlow_t));

    return CPARSER_OK;
}    /* end of cparser_cmd_rg_clear_multicastflow */

/*
 * rg add multicastFlow entry
 */
cparser_result_t
cparser_cmd_rg_add_multicastFlow_entry(
    cparser_context_t *context)
{
 	int ret;
	int index;
	
    DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	ret =  rtk_rg_multicastFlow_add(&mcFlow, &index);
	if(ret!=RT_ERR_RG_OK){
		diag_util_mprintf("add rtk_rg_multicastFlow_add failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else{
		diag_util_mprintf("add rtk_rg_multicastFlow_add[%d] success.\n",index);
	}

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_add_multicastflow_entry */


/*
 * rg get multicastFlow entry <UINT:flow_idx>
 */
cparser_result_t
cparser_cmd_rg_get_multicastFlow_entry_flow_idx(
    cparser_context_t *context,
    uint32_t  *flow_idx_ptr)
{
	int ret;
	int index=*flow_idx_ptr;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	osal_memset(&mcFlow, 0x0, sizeof(rtk_rg_multicastFlow_t));

	ret = rtk_rg_multicastFlow_find(&mcFlow, &index);
	if(ret!=RT_ERR_RG_OK)
	{
		diag_util_mprintf("get rtk_rg_multicastFlow_find failed. rg API return %d \n",ret);
		return CPARSER_NOT_OK;
	}
	else
	{
		if(index==*flow_idx_ptr){	
			_diag_multicastFlowShow(&mcFlow);
		}
		else{
			diag_util_mprintf("rtk_rg_multicastFlow_find[%d] is empty. index=%d\n",*flow_idx_ptr,index);
			return CPARSER_NOT_OK;
		}
	}

}    /* end of cparser_cmd_rg_get_multicastflow_entry_flow_idx */

/*
 * rg del multicastFlow entry <UINT:flow_idx>
 */
cparser_result_t
cparser_cmd_rg_del_multicastFlow_entry_flow_idx(
    cparser_context_t *context,
    uint32_t  *flow_idx_ptr)
{
	int ret;
	DIAG_UTIL_PARAM_CHK();
	
	ret = rtk_rg_multicastFlow_del(*flow_idx_ptr);
	if(ret!=RT_ERR_RG_OK)
		return CPARSER_NOT_OK;

    return CPARSER_OK;

}    /* end of cparser_cmd_rg_del_multicastflow_entry_flow_idx */


/*
 * rg set multicastFlow multicast_ipv4_addr <IPV4ADDR:multicast_ipv4_addr> multicast_ipv6_addr <IPV6ADDR:multicast_ipv6_addr> isIPv6 <UINT:isIPv6> src_intf_idx <UINT:src_intf_idx> port_mask <HEX:port_mask>
 */
cparser_result_t
cparser_cmd_rg_set_multicastFlow_multicast_ipv4_addr_multicast_ipv4_addr_multicast_ipv6_addr_multicast_ipv6_addr_isIPv6_isIPv6_src_intf_idx_src_intf_idx_port_mask_port_mask(
    cparser_context_t *context,
    uint32_t  *multicast_ipv4_addr_ptr,
    char * *multicast_ipv6_addr_ptr,
    uint32_t  *isIPv6_ptr,
    uint32_t  *src_intf_idx_ptr,
    uint32_t  *port_mask_ptr)
{
	int ret;
	DIAG_UTIL_PARAM_CHK();
	mcFlow.multicast_ipv4_addr=*multicast_ipv4_addr_ptr;
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&mcFlow.multicast_ipv6_addr[0], TOKEN_STR(6)), ret);
	mcFlow.isIPv6=*isIPv6_ptr;
	mcFlow.src_intf_idx=*src_intf_idx_ptr;
	mcFlow.port_mask.portmask=*port_mask_ptr;

	return CPARSER_OK;
}    /* end of cparser_cmd_rg_set_multicastflow_multicast_ipv4_addr_multicast_ipv4_addr_multicast_ipv6_addr_multicast_ipv6_addr_isipv6_isipv6_src_intf_idx_src_intf_idx_port_mask_port_mask */


