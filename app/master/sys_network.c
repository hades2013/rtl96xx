
#include "master.h"
#include "timer.h"
#include <ipc_protocol.h>
#include "build_time.h"
//#include "switch.h"
#include <shutils.h>
#include <str_utils.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time_zone.h>
#include "utils.h"
#include "sys_upgrade.h"
#include <drv_wtd_user.h>
#include <lw_type.h>
#include <lw_config_oid.h>
#include <lw_config_api.h>
#include <lw_if_pub.h>
#include <if_notifier.h>
#include <systemlog.h>			//	add by leijinbao 2013/09/15 for USER_LOG
static void kill_udhcpc_if_exist(void)
{
	FILE *fd;
	pid_t pid = 0;
	char buffer[128];
	fd = fopen(SYS_UDHCPC_PID_FILE, "r");
	if (fd != NULL){
		if(fgets(buffer, sizeof(buffer) - 1, fd)){
			pid = strtoul(buffer, NULL, 0);
		}
		fclose(fd);
	}

	if (pid > 0){
		kill(pid, SIGTERM);
	}
}

extern unsigned short LocalM_vlan;

/*****************************************************************
  Function: 	   sys_ip_do_apply
  Description:	  apply the vlan interface
  Author:			 feihuaxin
  Date: 		   2012/7/12
  INPUT:		   timer_element_t *timer, void *data
  OUTPUT:		  
  Return:		   
  Others:		   
*****************************************************************/
static int sys_ip_do_apply(timer_element_t *timer, void *data)
{
	sys_network_t *sys = &sys_network;
	char ipstr[24] = {0};
	char tmp[24] = {0};  
    //char buffer[256] = {0};
	FILE *fd = NULL;
	int i = 0,ret=0;
	//int has_default = 0;
	int metric = 0;
	ifindex_t ifindex;
	char val[256] = {0};
    sys_mvlan_t *info = &sys_mvlan;

	memset(val,0,sizeof(val));
	sprintf(val,"%s%d",IF_L3VLAN_NAMEPREFIX,info->m_vlan);
	ret=IF_GetByIFName(val,&ifindex);
	
	/* Set IP */
	strcpy(ipstr, inet_ntoa(sys->netif.ip));
	strcpy(tmp, inet_ntoa(sys->netif.subnet));

	ifm_call_notifiers(ifindex,IF_NOTIFY_GOING_DIALUP,NULL);
	//printf("sys_ip_do_apply %s\n",sys->ifname);
	/*Begin modify by jiangzhenhua 2012/7/24*/
	//eval("ifconfig", sys->ifname, ipstr, "netmask", tmp, "mtu", "1496");
	#if 0//defined(CONFIG_PRODUCT_EPN104W)
	memset(val,0,sizeof(val));
	sprintf(val,"ifconfig %s.%d %s %s",IF_L3VLAN_BASE,info->m_vlan,"mtu","1496");
	system(val);
	#if 0
	IF_GetAttr(ifindex,IF_MAC,sys->mac.octet,sizeof(sys->mac.octet));	
	memset(val,0,sizeof(val));
	sprintf(val,"ifconfig br0 %s %s %s %s %x:%x:%x:%x:%x:%x",ipstr,"netmask",tmp,"hw ether",
															sys->mac.octet[0],sys->mac.octet[1],
															sys->mac.octet[2],sys->mac.octet[3],
															sys->mac.octet[4],sys->mac.octet[5]);
	system(val);
	#else
	memset(val,0,sizeof(val));
	sprintf(val,"ifconfig br0 %s %s %s",ipstr,"netmask",tmp);
	system(val);
	#endif
	#else
	memset(val,0,sizeof(val));
	sprintf(val,"ifconfig %s.%d %s %s %s %s %s",IF_L3VLAN_BASE,info->m_vlan,ipstr,"netmask",tmp,"mtu","1496");
	system(val);
	#endif	
	USER_LOG(LangId,SYSTEM,LOG_NOTICE,M_IPCHANGE,"",ipstr);  //add by leijinbao 2013/09/17 for ip change log
	/*End modify by jiangzhenhua 2012/7/24*/
	IF_SetAttr(ifindex,IF_IP,&(sys->netif.ip.s_addr),sizeof(sys->netif.ip.s_addr));
	IF_SetAttr(ifindex,IF_MASK,&(sys->netif.subnet.s_addr),sizeof(sys->netif.subnet.s_addr));
	IF_SetAttr(ifindex,IF_MTU,&(sys->mtu),sizeof(sys->mtu));
    /*add by zhouguanhua 2013/6/4*/
	IF_SetAttr(ifindex,IF_GATEWAY,&(sys->netif.gateway.s_addr),sizeof(sys->netif.gateway.s_addr));
    /*end add by zhouguanhua 2013/6/4*/
	ifm_call_notifiers(ifindex,IF_NOTIFY_ADDRADD,NULL);
	//printf("sys_ip_do_apply exit\n");
	#if 1
	/*Begin modify by liaohongjun 2012/9/18 of QID0019*/		
	if(!strcmp(sys->interface_state, INTERFACE_STATE_DOWN))
	{ 
		eval("ifconfig", sys->ifname, "down");
	}	
	#endif
	/*End modify by liaohongjun 2012/9/18 of QID0019*/
	/* Del default route if exist */
	/*Begin modify by guguiyuan 2012/9/24 of QID0036*/		
	
	eval("route", "del", "default");
	
	/*End modify by guguiyuan 2012/9/24 of QID0036*/
	
	/* If has gateway , set */
	strcpy(ipstr, inet_ntoa(sys->netif.gateway));
	if (sys->netif.gateway.s_addr > 0)
	{
		sprintf(tmp, "%d", metric);
		//INFO_LOG("System Gateway:%s", ipstr); 
		eval("route", "add", "default", "gw", ipstr, "dev", sys->ifname, "metric", tmp);	
	}
	// Set DNS
	fd = fopen(SYS_RESOLV_CONF_TMP, "w+");
	DBG_ASSERT(fd, "Open %s failed", SYS_RESOLV_CONF_TMP);
	if (fd != NULL){
		if (sys->domain[0]){
			fprintf(fd, "search %s\n", sys->domain);
		}
		for (i = 0; (i < MAX_DNS_NUM) && (sys->netif.dns[i].s_addr > 0); i ++ ){
			fprintf(fd, "nameserver %s\n", inet_ntoa(sys->netif.dns[i]));
		}
		fclose(fd);
		if (rename(SYS_RESOLV_CONF_TMP, SYS_RESOLV_CONF) < 0){
			perror("rename");
			DBG_ASSERT(0, "Failed to rename %s to %s", SYS_RESOLV_CONF_TMP, SYS_RESOLV_CONF);
		}
	}
	return TIMER_RUN_ONCE;
}


/*****************************************************************
  Function:        sys_ip_apply_static
  Description:    
  Author: 	        
  Date:   		   2012/7/12
  INPUT:           timer_element_t *timer, void *data
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

static int sys_ip_apply_static(void)
{
	sys_network_t *sys = &sys_network;
	// copy static to config 
	memcpy(&sys->netif, &sys->netif_static, sizeof(sys->netif));
	sys->dns_static = 0;
	//sys_ip_do_apply(NULL, NULL);
	/*Begin modified by feihuaxin for vlan-interface 2012-7-12 */
	
	sys_ip_do_apply( NULL, NULL);
	/*End modified by feihuaxin for vlan-interface 2012-7-12 */
	return 1;
}

static int sys_ip_del_static(void)
{
	sys_network_t *sys = &sys_network;
	ifindex_t ifindex;
	int ret=0;
	char ipstr[24] = {0};
	int ip=0;
	ret=IF_GetByIFName(sys->ifname,&ifindex);
	ifm_call_notifiers(ifindex,IF_NOTIFY_ADDRREM,NULL);
	if (sys->netif.gateway.s_addr > 0)
	{
		strcpy(ipstr, inet_ntoa(sys->netif_static.gateway));
		eval("route", "del", "default", "gw", ipstr, "dev", sys->ifname, "metric", "0");	
	}
	ifm_call_notifiers(ifindex,IF_NOTIFY_DIALDOWN,NULL);	
	eval("ifconfig", sys->ifname, "0.0.0.0", "mtu", "1496");
    IF_SetAttr(ifindex,IF_IP,&ip,sizeof(sys->netif_static.ip));
	IF_SetAttr(ifindex,IF_MASK,&ip,sizeof(sys->netif_static.subnet));
	ifm_call_notifiers(ifindex,IF_NOTIFY_ALREADY_DIALDOWN,NULL);
	
}

/*****************************************************************
  Function:        sys_ip_apply_dynamic
  Description:    apply the vlan interface
  Author: 	        
  Date:   		   2013/5/22
  INPUT:           timer_element_t *timer, void *data
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/



static int sys_ip_apply_dynamic(void)
{
	sys_network_t *sys = &sys_network;
/* this will cause a Z-process */

	char exe_str[128];
	sprintf(exe_str, "udhcpc -a -i %s -p %s -H %s &", sys->ifname, SYS_UDHCPC_PID_FILE,
			sys->hostname[0] ? sys->hostname : "none");
	system(exe_str);
	USER_LOG(LangId,SYSTEM,LOG_NOTICE,M_IPCHANGE,"",inet_ntoa(sys->netif.ip));  //add by leijinbao 2013/09/17 for ip change log

	return 1;
}

/*****************************************************************
  Function:        sys_ip_dhcpc_event_apply
  Description:    
  Author: 	        
  Date:   		   2013/5/22
  INPUT:           timer_element_t *timer, void *data
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/


int sys_ip_dhcpc_event_apply(sys_dhcpc_t *dhcpc)
{
	sys_network_t *sys = &sys_network;
	int i;

	/* Debug */ 
# define dump_str(x) fprintf(stderr,"Dump "#x" = %s\n", x)
# define dump_int(x) fprintf(stderr,"Dump "#x" = %d\n", x)
# define dump_ip(x) fprintf(stderr,"Dump "#x" = %s\n", inet_ntoa(x))

	if (sys->ip_proto == IP_PROTO_STATIC){
		DBG_PRINTF("Receive DHCPC event in Static Mode");
		return 0;
	}


	if (!dhcpc->ifname[0]){
		DBG_ASSERT(0, "Empty ifname");
		return 0;
	}

	if (strcmp(dhcpc->ifname, sys->ifname)){
		DBG_ASSERT(0, "Invalid ifname %s", dhcpc->ifname);
		return 0;		
	}
	

	switch(dhcpc->event)
	{
		case DHCPC_DECONFIG:
			INFO_LOG("System IP:0.0.0.0");
			memset(&sys->netif, 0, sizeof(sys->netif));
			//eval("ifconfig", sys->ifname, "0.0.0.0");	
			sys_ip_del_static();
			break;
		case DHCPC_BOUND:
		case DHCPC_RENEW:
			if (dhcpc->ip.s_addr == 0){
				DBG_ASSERT(dhcpc->ip.s_addr, "Invalid IP address");
				return 0;
			}
			ip_set(sys->netif.ip, dhcpc->ip);
			ip_set(sys->netif.subnet, dhcpc->subnet);
			ip_set(sys->netif.gateway, dhcpc->gateway);
		
			for (i = 0; i < MAX_DNS_NUM; i ++){
				ip_set(sys->netif.dns[i], dhcpc->dns[i]);
			}

			safe_strncpy(sys->domain, dhcpc->domain, sizeof(sys->domain));
			sys->lease_time = dhcpc->lease_time;
			// apply all
			sys_ip_do_apply(NULL, NULL);
			break;	
		default:
			DBG_ASSERT(0, "Invalid dhcpc event %d", dhcpc->event);
			return 0;				
	}

	return 1;
}

/*****************************************************************
  Function:        sys_networking_load
  Description:    sys_networking_load
  Author: 	         feihuaxin
  Date:   		   2012/7/12
  INPUT:           sys_network_t *sys
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/
void sys_networking_load(sys_network_t *sys)
{
//	int items = 0; 
//    int i = 0;
	char val[BUF_SIZE_256] = {0};
//    char *vars[10] = {0};
	netif_param_t *netif = NULL;
    ifindex_t ifindex;
	sys_mvlan_t *info = &sys_mvlan;
	
	netif = &sys->netif_static;	
	
# define dbg_str(x, v) DBG_ASSERT(0, "Invalid @ "#x" = %s", v)

	/* Hostname */
	//safe_strncpy(sys->hostname, nvram_safe_get(nvram_handle, "sys_systemname"), sizeof(sys->hostname));
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSTEM_NAME, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get Hostname faild");
	}
	safe_strncpy(sys->hostname, val, sizeof(sys->hostname));
	if (!sys->hostname[0])
    {
		DBG_ASSERT(0, "Empty Hostname, Set to none");
		safe_strncpy(sys->hostname, "none", sizeof(sys->hostname));		
	}
    
    /*begin added by liaohongjun 2012/9/18 of QID0019*/

	memset(val,0,sizeof(val));
	sprintf(val,"%s%d",IF_L3VLAN_NAMEPREFIX,info->m_vlan);
       
    if(IF_GetByIFName(val,&ifindex) !=IF_OK)
    {
        DBG_ASSERT(0, "Get ifname faild");     
    }
    /* mac */
	if ((get_if_mac(SYS_NETIF_NAME, &(sys->mac))) < 0){
        DBG_PRINTF("get sys mac error");
	}    
    /*end added by liaohongjun 2012/9/18 of QID0019*/

    /*ifname*/
    IF_GetAttr(ifindex,IF_LINKNAME,sys->ifname,sizeof(sys->ifname));
      
    /* IP proto */         
    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_DIAL_TYPE, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get IP PROTO faild");
    }
    if (val[0])
    {
        sys->ip_proto = !strcmp(val, "static") ? IP_PROTO_STATIC : IP_PROTO_DYNAMIC;
    }
    else 
    {
        dbg_str(sys->ip_proto, val);
    }
    
    /* ipaddress */
    memset(val, 0, sizeof(val));        
    if(cfg_getval(ifindex,CONFIG_IP_ADDR,val,"",sizeof(val))!=0)
    {               
      DBG_ASSERT(0, "Get IP faild");
    } 
    if (!val[0] || !inet_aton(val, &netif->ip))
    {
        dbg_str(netif->ip, val);
    }

    /*ipmask*/
    memset(val, 0, sizeof(val));
    if((cfg_getval(ifindex,CONFIG_IP_MASK , (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get NETMASK faild");
    }
    if (!val[0] || !inet_aton(val, &netif->subnet))
    {
        dbg_str(netif->subnet, val);        
    }
    /*gateway*/
    memset(val, 0, sizeof(val));
    if((cfg_getval(ifindex, CONFIG_IP_GATEWAY, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get GATEWAY faild");
    }
    if (!val[0] || !inet_aton(val, &netif->gateway))
    {
        dbg_str(netif->gateway, val);       
    }

    #if 0
    /* vlan_interface_state */   
    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_INTERFACE_STATE, (void *)sys->interface_state, "", sizeof(sys->interface_state))) < 0)
    {
        DBG_ASSERT(0, "Get IP INTERFACE STATE faild");
    }    
	if (!sys->interface_state[0])
    {
		DBG_ASSERT(0, "Empty state, Set to up");
		safe_strncpy(sys->interface_state, "up", sizeof(sys->interface_state));		
	}
	#endif
    /*masterdns*/
    #if 0
    //begin modify by zhouguanhua 2012/8/31
    memset(val, 0, sizeof(val));
    if((cfg_getval(ifindex, CONFIG_IP_MSTDNS, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get DNS faild");
    }
    //end modify by zhouguanhua 2012/8/31	
	if (val[0])
	{
    	items = split_string(val, ',', vars);
    	items = (items > MAX_DNS_NUM) ? MAX_DNS_NUM : items;
    	for (i = 0 ; i < items; i ++)
    	{
    		inet_aton(vars[i], &netif->dns[i]);
    	}
	}
	#endif
    return;

}

int sys_networking_commit(int apply_options)
{
	sys_network_t *sys = &sys_network;
	//char str[128];
	//int i;	
    ifindex_t ifindex;
	char val[256] = {0};
    sys_mvlan_t *info = &sys_mvlan;
    
    /* Hostname */
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SYSTEM_NAME, sys->hostname))
	{
		return -1;	
	}

	memset(val,0,sizeof(val));
	sprintf(val,"%s%d",IF_L3VLAN_NAMEPREFIX,info->m_vlan);
	
    if(IF_GetByIFName(val,&ifindex) !=IF_OK)
    {
        return -1;	    
    }
   
    /* IP proto */ 
    if(0 !=master_cfg_setval(ifindex, CONFIG_DIAL_TYPE, (sys->ip_proto ? "dhcp" : "static")))
	{
		return -1;	
	}
    /* ipaddress */
	if(0 !=master_cfg_setval(ifindex, CONFIG_IP_ADDR, inet_ntoa(sys->netif_static.ip)))
	{
		return -1;	
	}
    /*ipmask*/
	if(0 !=master_cfg_setval(ifindex, CONFIG_IP_MASK, inet_ntoa(sys->netif_static.subnet)))
	{
		return -1;	
	}
    /*gateway*/
	if(0 !=master_cfg_setval(ifindex, CONFIG_IP_GATEWAY, inet_ntoa(sys->netif_static.gateway)))
	{
		return -1;	
	}
    #if 0
    /* vlan_interface_state */   
	if(0 !=master_cfg_setval(ifindex, CONFIG_INTERFACE_STATE, sys->interface_state))
	{
		return -1;	
	}
    #endif
	#if 0 //add by zhouguanhua 2013/6/16
    /*masterdns*/
    for (i = 0; i < MAX_DNS_NUM; i ++)
    {
		if (sys->netif.dns[i].s_addr > 0)
		{
			vlen += sprintf(str +vlen,  "%s%s", (i > 0) ? "," : "", inet_ntoa(sys->netif.dns[i]));
		}
	}
	if(0 !=master_cfg_setval(ifindex, CONFIG_IP_MSTDNS, str))
	{
		return -1;	
	}	
	#endif  //add by zhouguanhua 2013/6/16
	return 1;
}

int sys_networking_update(ipc_network_t *cfg)
{
	sys_network_t *sys = &sys_network;
	int ip_change = 0, hostname_change = 0,interface_state_change = 0, i;

/* Debug */	
# define dump_str(x) fprintf(stderr,"Dump "#x" = %s\n", x)
# define dump_int(x) fprintf(stderr,"Dump "#x" = %d\n", x)
# define dump_ip(x) fprintf(stderr,"Dump "#x" = %s\n", inet_ntoa(x))

	if (strcmp(sys->hostname, cfg->hostname)){
		safe_strncpy(sys->hostname, cfg->hostname, sizeof(sys->hostname));
		hostname_change = 1;
		set_update(hostname);
	}
    
    /*begin modified by liaohongjun 2012/9/18 of EPN104QID0019*/
	if (strcmp(sys->interface_state, cfg->interface_state)){
		safe_strncpy(sys->interface_state, cfg->interface_state, sizeof(sys->interface_state));
		interface_state_change = 1;
		set_update(interface_state);
	}

	if (cfg->ip_proto == IP_PROTO_STATIC){
        if(!interface_state_change){
    		if (!ip_equal(sys->netif.ip, cfg->netif.ip) 
    			|| !ip_equal(sys->netif.subnet, cfg->netif.subnet)
    			|| !ip_equal(sys->netif.gateway, cfg->netif.gateway)){
    			ip_change ++;
    		}
    		 /*begin modified by zhouguanhua 2012/8/31*/
            #if 1
    		for (i = 0; i < MAX_DNS_NUM; i ++){
    			if(!ip_equal(sys->netif.dns[i], cfg->netif.dns[i])){
    				ip_change ++;
    				break;
    			}
    		}	
            #endif
    		if ((ip_change > 0) || (sys->ip_proto != IP_PROTO_STATIC)){
    			sys->ip_proto = cfg->ip_proto; 
    			ip_set(sys->netif_static.ip, cfg->netif.ip);
    			ip_set(sys->netif_static.subnet, cfg->netif.subnet);
    			ip_set(sys->netif_static.gateway, cfg->netif.gateway);
                #if 0 //add by zhouguanhua 2013/6/16
    			/*begin modified by zhouguanhua 2012/8/31*/
                #if 1
    			for (i = 0; i < MAX_DNS_NUM; i ++){
    				ip_set(sys->netif_static.dns[i], cfg->netif.dns[i]);
    			}
                #endif
                /*end modified by zhouguanhua 2012/8/31*/
                #endif  //add by zhouguanhua 2013/6/16
    			set_update(ip);
    			if (!ip_change)
    				ip_change ++;			
    		}
        }
	}else if(cfg->ip_proto == IP_PROTO_DYNAMIC){
        if(!interface_state_change){	
    		if (sys->ip_proto == IP_PROTO_STATIC){
    			sys->ip_proto = cfg->ip_proto;
    			set_update(ip);
    			ip_change ++;
    		}
    		if (cfg->action == ACTION_RELEASE){
    			if(sys->ip_proto == IP_PROTO_DYNAMIC)
    			{
                    sys->ip_proto = IP_PROTO_STATIC;
        			set_update(ip);
        			ip_change ++;                
                }			
    		}else if (cfg->action == ACTION_RENEW){
    			set_update(ip);
    			ip_change ++;
    		}

        }	
	}
    
	return (ip_change || hostname_change ||interface_state_change) ? 1 : 0;
    /*end modified by liaohongjun 2012/9/18 of EPN104QID0019*/
}

int sys_networking_apply(int apply_options)
{
	sys_network_t *sys = &sys_network;
	int ret = 0;
	/*apply_options==0 mean init*/
	/* set mac address */


	/* set hostname */
	if (test_update(hostname)){
		sethostname(sys->hostname, strlen(sys->hostname));
		clr_update(hostname);
	}
   
    /*begin added by liaohongjun 2012/9/18 of EPN104QID0019*/
	if (test_update(interface_state)){
        (void)eval("ifconfig", sys->ifname, sys->interface_state);
        clr_update(interface_state);
	}  
    /*end added by liaohongjun 2012/9/18 of EPN104QID0019*/
   
	if (test_update(ip)){
		kill_udhcpc_if_exist();
		if (sys->ip_proto == IP_PROTO_STATIC){
			ret = sys_ip_apply_static();
		}else {
			ret = sys_ip_apply_dynamic();
		}
		clr_update(ip);
	}	
	return ret;
}

static struct notifier_block g_stNetwork;
int network_notifier(ifindex_t ifindex,struct notifier_block *nb, unsigned long event, void *arg)
{   
    int ret = 0; 
	unsigned char val[BUF_SIZE_64];
	
    if((cfg_getval(ifindex, CONFIG_DIAL_TYPE, (void *)val, "", sizeof(val))) < 0)
    {
        return IF_NOTIFY_ERR;
    }
    switch (event)
    {
      
        case IF_NOTIFY_PHYUP:
            {
                
               
               
				if(strcmp(val,"static")==0)
				{					
               		sys_ip_apply_static();
               	}else
				{
					sys_ip_apply_dynamic();
				}
            }
            break;
     
        case IF_NOTIFY_PHYDOWN:
            {			
                if(strcmp(val,"static")==0)
				{
					sys_ip_del_static();
				}
				else
				{
					kill_udhcpc_if_exist();
				}				
            }
            break;
        default:
            return IF_NOTIFY_OK;
            
    }
    if (ret)
    {        
        return IF_NOTIFY_ERR;
    }
    else
    {        
        return IF_NOTIFY_OK;
    }   
}

void network_init(void)
{
	int ret;
	g_stNetwork.ifindex=IF_INVALID_IFINDEX;
    g_stNetwork.iftypemask|=1<<IF_SUB_L3VLAN;
    g_stNetwork.notifier_call=network_notifier;
    g_stNetwork.priority=IF_ROUTE_ADDR_PRIO;
    g_stNetwork.eventmask=(if_event_t) IF_NOTIFY_PHYUP | IF_NOTIFY_PHYDOWN;

    ret=ifm_notifier_register(&g_stNetwork);	
}
