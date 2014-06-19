

#include <zebra.h>
#include "command.h"
#include "vty.h"

#include "ipc_client.h"
#include "eoc_utils.h"
#include <ipc_protocol.h>
#include "memory.h" 
#include <lw_config_api.h>
#include <lw_config_oid.h>
#include <lw_if_pub.h>
#include "build_time.h"
#include <vlan_utils.h>


/*Begin add by huangmingjian 2012/08/23 for EPN104QID0008*/ 
#define ITEM_HOSTNAME         "sys_systemname"
#define ITME_SUPER_PASSWOED   "super_password"
/*End add by huangmingjian 2012/08/23 for EPN104QID0008 */

/*Begin add by huangmingjian 2012/09/25 for EPN104QID0058*/ 
int update_prompt_from_web();
extern host_init_t ht_init;
/*End add by huangmingjian 2012/09/25 for EPN104QID0058 */

char *runtime_str(time_t tm)
{
	static char str[64];
	uint32_t day, hour, min, sec;
	day = tm / (60*60*24);
	tm = tm % (60*60*24);
	hour = tm / (60*60);
	tm = tm % (60*60);
	min = tm / 60;
	sec = tm % 60;
	sprintf(str, "%d %s %d %s %d %s %d %s", day, (day > 1) ? "days" : "day", 
		hour, (day > 1) ? "hours" : "hour",
		min, (min > 1) ? "minutes" : "minute",
		sec, (sec > 1) ? "seconds" : "second");
	return str;
}


/*add by zhouguanhua 2013/6/5*/

int get_info_val(sys_info_t *info)
{
    int invalid=0;
    char val[BUF_SIZE256] = {0};
    version_t sys_version = {version_string, version_count, version_date};

    if(info == NULL)
    {
        return IPC_STATUS_FAIL;
    }
     /* mac */ 
	if ((get_if_mac(SYS_NETIF_NAME, &(info->sysmac))) < 0)
	{
        DBG_PRINTF("get sys mac error");
        return IPC_STATUS_FAIL;
	}     
    
	//version_date
#if defined(PRIV_PRODUCT_NAME_SUPPORT)	
	safe_strncpy(info->sw_version,	CONFIG_RELEASE_VER_PRIV, sizeof(info->sw_version));
#else	
	safe_strncpy(info->sw_version,  sys_version.toString(), sizeof(info->sw_version));
#endif
	//buildtime	
	safe_strncpy(info->buildtime,sys_version.toDate() , sizeof(info->buildtime));

    //HWREV
    memset(val,0,sizeof(val));
    if(GetHardVersion(val,sizeof(val)) !=-1)
    {      
        safe_strncpy(info->hw_version,val,sizeof(info->hw_version));     
    }
    else
    {
        return IPC_STATUS_FAIL;
    } 

    //uptime
    info->uptime = sys_get_uptime();  
    return IPC_STATUS_OK;
}
/*add by zhouguanhua 2013/6/5*/

#if 0 //add by zhouguanhua 2013/6/5
int ipc_get_sys_info(sys_info_t *info)
{
	ipc_sys_info_ack_t *pack;
	int ret;
	pack = ipc_sys_info(ipc_fd);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		if (info){
			memcpy(info, &pack->info, sizeof(*info));
		}
		ret = 0;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}
#endif  //add by zhouguanhua 2013/6/5

int ipc_get_sys_security(sys_security_t *info)
{
	ipc_sys_security_ack_t *pack;
	int ret;
	pack = ipc_sys_security(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL, 0);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		if (info){
			memcpy(info, &pack->security, sizeof(*info));
		}
		ret = 0;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_get_sys_networking(sys_network_t *info)
{
	ipc_sys_network_ack_t *pack;
	int ret;
	pack = ipc_sys_networking(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		if (info){
			memcpy(info, &pack->network, sizeof(*info));
		}
		ret = 0;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	if(pack) free(pack);		
	return ret;
}
int get_if_mac(char *ifname, ethernet_addr_t *mac)
{
	int sock, ret;
	struct ifreq ifr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	DBG_ASSERT(sock >= 0, "create socket: %s", strerror(errno));
	if(sock < 0) return -1;
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name) - 1);
	ret = ioctl(sock, SIOCGIFHWADDR, &ifr);
	DBG_ASSERT(ret >= 0, "ioctl: %s", strerror(errno));
	if(ret < 0) return -1;
	memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof(ethernet_addr_t));
	close(sock);
	return 0;
}

int ipc_get_sys_mvlan(sys_mvlan_t *mvlan)
{	
	ipc_sys_mvlan_ack_t *pack;
	int ret;
	pack = ipc_sys_mvlan(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) {
		if (mvlan){
			memcpy(mvlan, &pack->mvlan, sizeof(*mvlan));
		}
		ret = 0;
	}else if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}	
	if(pack) free(pack);

	return ret;
}

/*****************************************************************
  Function:       get_sys_networking_value
  Description:    get networking value
  Author:         zhouguanhua
  Date:           2013/6/3
  INPUT:          sys_network_t *sys
  OUTPUT:         
  Return:          
  Others:          
*****************************************************************/
/*begin modified by wanghuanyu for 228*/
int get_sys_networking_value(sys_network_t *sys,int cfgdomain)
{
    char val[120] = {0};
	 char mvlan[120] = {0};
    netif_param_t *netif = NULL;
    ifindex_t ifindex;
    int invalid = 0;
	sys_mvlan_t info;
	int ret = NO_ERROR;
   
#define dbg_str(x, v) DBG_ASSERT(0, "Invalid @ "#x" = %s", v)

    netif = &sys->netif; 
    
    if(NULL==sys)
    {
        return IPC_STATUS_FAIL;
    }

	memset(&info, 0, sizeof(sys_mvlan_t));
	ret = ipc_get_sys_mvlan(&info);
	if(IPC_STATUS_OK != ret)	
	{
		return ERROR;		  
	}
	memset(val,0,sizeof(val));
	

    if(0 == cfgdomain)////read from ram
    {
    	sprintf(val,"%s%d",IF_L3VLAN_NAMEPREFIX,info.m_vlan);
        if(IF_GetByIFName(val,&ifindex) !=IF_OK)
        {
            DBG_ASSERT(0, "Get ifname faild"); 
            invalid++;
        }

        /* ipaddress */

        if(IF_OK !=IF_GetAttr(ifindex,IF_IP,&sys->netif.ip.s_addr,sizeof(sys->netif.ip.s_addr)))
        {               
          DBG_ASSERT(0, "Get IP faild");
          invalid++;
        }  

        /*ipmask*/
        if(IF_OK !=IF_GetAttr(ifindex,IF_MASK,&sys->netif.subnet.s_addr,sizeof(sys->netif.subnet.s_addr)))
        {
            DBG_ASSERT(0, "Get NETMASK faild");
            invalid++;
        }
        /*gateway*/
        if(IF_OK !=IF_GetAttr(ifindex,IF_GATEWAY,&sys->netif.gateway.s_addr,sizeof(sys->netif.gateway.s_addr)))
        {               
          DBG_ASSERT(0, "Get gateway faild");
          invalid++;
        }

        #if 0
        IF_STATUS_E logstate = IFM_MAX; 
        /*查看虚接口状态*/
        IF_GetAttr(ifindex, IF_STATE, &logstate, sizeof(IF_STATUS_E));
        /*虚接口UP*/              
        if(IF_UP(logstate))
        {
            safe_strncpy(sys->interface_state, INTERFACE_STATE_UP, sizeof(sys->interface_state));
        }
        else
        {
            safe_strncpy(sys->interface_state, INTERFACE_STATE_DOWN, sizeof(sys->interface_state));            
        }
        #endif
		  /*homename*/
		if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSTEM_NAME, sys->hostname, "", sizeof(sys->hostname))) < 0)
		{
			DBG_ASSERT(0, "Get Hostname faild");
			invalid++;
		}
    }
    else   //read from flash
    {
       // ifindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_VIRTUAL,0);
       
	    if((cfg_getval(IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_VIRTUAL,0), CONFIG_MVLAN_VID, (void *)mvlan, "", sizeof(mvlan))) < 0)
	    {
	        DBG_ASSERT(0, "Get mvlan id  faild");
	    }
		sprintf(val,"%s%s",IF_L3VLAN_NAMEPREFIX,mvlan);
		info.m_vlan=strtoul(mvlan,NULL,0);
        ret=IF_GetByIFNameDomain(val,IF_DOMAIN_VIRTUAL,&ifindex);
	    
        /* ipaddress */
        memset(val, 0, sizeof(val));        
        if(cfg_getval(ifindex,CONFIG_IP_ADDR,(void *)val,"",sizeof(val))!=0)
        { 
          DBG_ASSERT(0, "Get IP faild");
        } 
        if (!val[0] || !inet_aton(val, &sys->netif.ip))
        {
            dbg_str(sys->netif.ip, val);
        }

        /*ipmask*/
        memset(val, 0, sizeof(val));
        if((cfg_getval(ifindex,CONFIG_IP_MASK , (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get NETMASK faild");
        }
        if (!val[0] || !inet_aton(val, &sys->netif.subnet))
        {
            dbg_str(sys->netif.subnet, val);        
        }
        /*gateway*/
        memset(val, 0, sizeof(val));
        if((cfg_getval(ifindex, CONFIG_IP_GATEWAY, (void *)val, "", sizeof(val))) < 0)
        {   
            DBG_ASSERT(0, "Get GATEWAY faild");
        }
        if (!val[0] || !inet_aton(val, &sys->netif.gateway))
        {
            dbg_str(sys->netif.gateway, val);       
        } 

       
        /* vlan_interface_state */  
        #if 0
        if((cfg_getval(ifindex, CONFIG_INTERFACE_STATE, sys->interface_state, "", sizeof(sys->interface_state))) < 0)
        {
            DBG_ASSERT(0, "Get INTERFACE STATE faild");
            invalid++;
        }
        #endif
		if((cfg_getval(IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_VIRTUAL,0), CONFIG_SYSTEM_NAME, sys->hostname, "", sizeof(sys->hostname))) < 0)
		{
			DBG_ASSERT(0, "Get Hostname faild");
			invalid++;
		}
    }
    
     /* mac */
    if ((get_if_mac(SYS_NETIF_NAME, &(sys->mac))) < 0)
    {
        DBG_PRINTF("get sys mac error");
        invalid++;
    }     
        
  
 
    /* IP proto */     
    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_DIAL_TYPE, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get IP PROTO faild");
        invalid++;
    }
    if (val[0])
    {
        sys->ip_proto = !strcmp(val, "static") ? IP_PROTO_STATIC : IP_PROTO_DYNAMIC;
    }
    else 
    {
        dbg_str(sys->ip_proto, val);
        invalid++;
    }  

	if (invalid > 0)
	{
    	DBG_ASSERT(0, "Invalid networking INFO");
    	return IPC_STATUS_FAIL;
	}
	
    return IPC_STATUS_OK;
}
/*end modified by wanghuanyu for 228*/

/*****************************************************************
  Function:       ipc_set_sys_loop
  Description:    set loop value
  Author:         zhouguanhua
  Date:           2013/6/3
  INPUT:         sys_loop_t *info
  OUTPUT:         
  Return:          
  Others:          
*****************************************************************/

int ipc_set_sys_loop(sys_loop_t *info)
{
	ipc_sys_loop_ack_t *pack;
	int ret;
	pack = ipc_sys_loop(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, info);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}

/*****************************************************************
  Function:       ipc_get_sys_loop
  Description:    get loop value
  Author:         zhouguanhua
  Date:           2013/6/3
  INPUT:         sys_loop_t *info
  OUTPUT:         
  Return:          
  Others:          
*****************************************************************/

int ipc_get_sys_loop(sys_loop_t *info)
{
    ipc_sys_loop_ack_t *pack;
    int ret;
    pack = ipc_sys_loop(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);

    if (pack && pack->hdr.status == IPC_STATUS_OK) {
        if (info){
            memcpy(info, &pack->loop, sizeof(*info));
        }
        ret = 0;
    }else if (pack){
        ret = pack->hdr.status;
    }else {
        ret = IPC_STATUS_FAIL;
    }

    if(pack) free(pack);        
    return ret;
}

/*****************************************************************
*/
#if 1

int ipc_set_sys_epon(sys_epon_t *info)
{
	ipc_sys_epon_ack_t *pack;
	int ret;
	pack = ipc_sys_epon(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, info);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}
int ipc_get_sys_epon(sys_epon_t *info)
{
    ipc_sys_epon_ack_t *pack;
    int ret;
    pack = ipc_sys_epon(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);

    if (pack && pack->hdr.status == IPC_STATUS_OK) {
        if (info){
            memcpy(info, &pack->epon_info, sizeof(*info));
        }
        ret = 0;
    }else if (pack){
        ret = pack->hdr.status;
    }else {
        ret = IPC_STATUS_FAIL;
    }

    if(pack) free(pack);        
    return ret;
}
#endif
/***************************************************************
  Function:       ipc_set_syslog
  Description:    set syslog value
  Author:         zhouguanhua
  Date:           2013/6/3
  INPUT:         sys_loop_t *info
  OUTPUT:         
  Return:          
  Others:          
*****************************************************************/

int ipc_set_syslog(sys_syslog_t *info)
{
	ipc_syslog_ack_t *pack;
	int ret;
	pack = ipc_sys_syslog(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, info);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int get_syslog_value(sys_syslog_t *sys,int cfgdomain )
{
	int invalid = 0;
	char val[64];  
    ifindex_t ifindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_VIRTUAL,0);

    if(sys == NULL)
    {
        return IPC_STATUS_FAIL;
    }

    if(cfgdomain == 0)
    {
        ifindex=IF_ROOT_IFINDEX;
    }
	/* syslog_en */		
    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SYSLOG_ENABLED, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get syslog_en  faild");
        return IPC_STATUS_FAIL;
    }

	if (val[0])
    {
		sys->syslog_en = strtoul(val, NULL, 0);       
	}
    else 
    {
            invalid ++;
    }
    /*syslog_ip*/
	memset(val, 0, sizeof(val));
	if((cfg_getval(ifindex, CONFIG_SYSLOG_IP, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get SYSLOG_IP faild");
		return IPC_STATUS_FAIL;
	}
	if (val[0]){
		strncpy(sys->syslog_ip, val, sizeof(sys->syslog_ip));
	}else {
		invalid ++;
	}	
    
	
	/*syslog_degree*/
	memset(val, 0, sizeof(val));
	if((cfg_getval(ifindex, CONFIG_SYSLOG_DEGREE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get SYSLOG_DEGREE faild");
		return IPC_STATUS_FAIL;
	}
	if (val[0]){
		sys->syslog_degree = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
		
	/*syslog_save_degree*/
	memset(val, 0, sizeof(val));
	if((cfg_getval(ifindex, CONFIG_SYSLOG_SAVE_DEGREE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get SYSLOG_SAVE_DEGREE faild");
		return IPC_STATUS_FAIL;
	}
	if (val[0]){
		sys->syslog_save_degree  = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}

	if (invalid > 0)
	{
		DBG_ASSERT(0, "Invalid LOG INFO");
		return IPC_STATUS_FAIL;
	}  	
	return IPC_STATUS_OK;
}

int ipc_set_sys_mvlan(sys_mvlan_t *info)
{
	ipc_sys_mvlan_ack_t *pack;
	int ret;
	pack = ipc_sys_mvlan(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, info);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	if(pack) free(pack);		
	return ret;
}
int get_sys_mvlan_value(sys_mvlan_t *sys)
{
	int invalid = 0;
	char val[10];  
    if(sys == NULL)
    {
        return IPC_STATUS_FAIL;
    }
	/* vlan_manage_vid */		
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_MVLAN_VID, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get mvlan id  faild");
        invalid ++;
    }

	if (val[0])
    {
		sys->m_vlan = strtoul(val, NULL, 0);       
	}
    else 
    {
            invalid ++;
    }
	/* vlan_interface*/	
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_MVLAN_INTERFACE, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get vlan_interface faild");
        invalid ++;
    }
	if (val[0])
    {
		sys->vlan_interface = strtoul(val, NULL, 0);       
	}
    else 
    {
        invalid ++;
    }

	if (invalid > 0){
		DBG_ASSERT(0, "Invalid mvlan INFO");
		return IPC_STATUS_FAIL;
	}
	
	return IPC_STATUS_OK;
}


/**********************************************************
**  Copyright (C) 2012-7-11, LOSOTECH Co. Ltd.
**  All rights reserved.
**  
**  FileName:       eoc_utils.c
**  Function:        ipc_get_host_info()
**  Description:    get the host_init value 
**  Author:           huangmingjian
**  Date:             2012/08/23
**  Others:          
**********************************************************/
int ipc_get_host_info(host_init_t *info )
{
    #if 0
	ipc_nvram_access_ack_t *pack = NULL;
	int ret = IPC_STATUS_FAIL;
	if(NULL == info)
	{   
		return ret;
	}
	pack = ipc_nvram_access(ipc_fd, NVRAM_ACCESS_GET, ITEM_HOSTNAME, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) 
	{
		if (info)
		{
			memcpy(info->ht_name, pack->value, sizeof(info->ht_name));
		}
		ret = IPC_STATUS_OK;
	}
	else if (pack)
	{
		ret = pack->hdr.status;
	}
	else 
	{
		ret = IPC_STATUS_FAIL;
	}

	if(pack) 
	{
		free(pack);
	}
	pack = NULL;
	pack = ipc_nvram_access(ipc_fd, NVRAM_ACCESS_GET, ITME_SUPER_PASSWOED, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) 
	{
		if (info)
		{
			memcpy(info->ht_super, pack->value, sizeof(info->ht_super));
		}
		ret |= IPC_STATUS_OK;
	}
	else if (pack)
	{
		ret |= pack->hdr.status;
	}
	else 
	{
		ret |= IPC_STATUS_FAIL;
	}
	
	if(pack) 
	{
		free(pack);	
	}
	return ret;
    #endif 
    int ret = IPC_STATUS_FAIL;
    char val[256] = {0};
    if(NULL == info)
    {   
        return ret;
    }
    memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSTEM_NAME, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get SYSTEM NAME faild");
		return ret = IPC_STATUS_FAIL;
	}
	safe_strncpy(info->ht_name, val, sizeof(info->ht_name));
    memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SUPER_PASSWORD, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get SUPER PASSWORD faild");
		return ret = IPC_STATUS_FAIL;
	}
	safe_strncpy(info->ht_super, val, sizeof(info->ht_super));  
	return ret = IPC_STATUS_OK;
}




/**********************************************************
**  Copyright (C) 2012-7-11, LOSOTECH Co. Ltd.
**  All rights reserved.
**  
**  FileName:       eoc_utils.c
**  Function:        ipc_set_host_info()
**  Description:     set the value to what we need 
**  Author:           huangmingjian
**  Date:             2012/08/23
**  Others:          
**********************************************************/

int ipc_set_host_info(host_init_t *info, int element)
{
    #if 0 //add by zhouguanhua 2013/5/31
	ipc_nvram_access_ack_t *pack;
	if(NULL == info)
	{	
		return IPC_STATUS_FAIL;
	}

    switch(element)
  	{
		 case HOSTNAME:
			pack = ipc_nvram_access(ipc_fd, NVRAM_ACCESS_SET, ITEM_HOSTNAME, info->ht_name);
			if ( pack->hdr.status != IPC_STATUS_OK) 
			{   
				if(pack) 
				{
					free(pack);
				}
				return IPC_STATUS_FAIL;
			}
			break;
			
		case SUPER_PASSWD:
			pack = ipc_nvram_access(ipc_fd, NVRAM_ACCESS_SET, ITME_SUPER_PASSWOED, info->ht_super);
			if ( pack->hdr.status != IPC_STATUS_OK) 
			{	
				if(pack) 
				{
					free(pack);
				}
				return  IPC_STATUS_FAIL;
			}
			break;
	}
	if(pack) 
	{
		free(pack);
	}	
	return IPC_STATUS_OK;
	#endif //add by zhouguanhua 2013/5/31
    sys_network_t sys;
    ipc_network_t ipc_set;
	if(NULL == info)
	{	
		return IPC_STATUS_FAIL;
	}
    switch(element)
  	{
		 case HOSTNAME:
		    /*begin add by zhouguanhua 2013/8/1 bug 135*/
            memset(&sys, 0, sizeof(sys_network_t));
            if(IPC_STATUS_OK != ipc_get_sys_networking(&sys))    
            {
                return IPC_STATUS_FAIL;
            }
            
            sethostname(info->ht_name, strlen(info->ht_name));

            if(strcmp(sys.hostname,info->ht_name) !=0)
            {
                safe_strncpy(ipc_set.hostname, info->ht_name, sizeof(ipc_set.hostname));  
                safe_strncpy(ipc_set.interface_state, sys.interface_state, sizeof(ipc_set.interface_state));

                ipc_set.ip_proto = sys.ip_proto;
                ip_set(ipc_set.netif.ip, sys.netif.ip);
    			ip_set(ipc_set.netif.subnet, sys.netif.subnet);
    			ip_set(ipc_set.netif.gateway, sys.netif.gateway);

                if(IPC_STATUS_OK != ipc_set_sys_networking(&ipc_set))    
                {
		            return IPC_STATUS_FAIL;      
                }                                  
            }

		     /*End add by zhouguanhua 2013/8/1 bug 135*/
            if (ipc_cfg_set(IF_ROOT_IFINDEX,CONFIG_SYSTEM_NAME,info->ht_name)!= IPC_STATUS_OK) 
            {   
            	return IPC_STATUS_FAIL;
            }
            break;
		case SUPER_PASSWD:
			if (ipc_cfg_set(IF_ROOT_IFINDEX,CONFIG_SUPER_PASSWORD,info->ht_super)!= IPC_STATUS_OK) 
			{   
				return IPC_STATUS_FAIL;
			}
			break;
	}	
	return IPC_STATUS_OK;
}


int ipc_set_sys_networking(ipc_network_t *info)
{
	ipc_sys_network_ack_t *pack;
	int ret;
	pack = ipc_sys_networking(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, info);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_set_sys_security(sys_security_t *info, int mask)
{
	ipc_sys_security_ack_t *pack;
	int ret;
	pack = ipc_sys_security(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, info, mask);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_system_request(int handle)
{
	ipc_system_ack_t *pack;
	int ret;
	pack = ipc_system_req(ipc_fd, handle, 0);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}

/*****************************************************************
    Function:ipc_get_telnet_service
    Description:get telnet service funtion
    Author:liaohongjun
    Date:2012/8/17
    Input:
    Output:char *service         
    Return:
=================================================
    Note:
*****************************************************************/
char *ipc_get_telnet_service(char *service)
{
    #if 0 //add by zhouguanhua 2013/6/5
    ipc_nvram_access_ack_t *pack = NULL;

    if(NULL == service)
    {
        return NULL;
    }
    pack = ipc_nvram_access(ipc_fd, NVRAM_ACCESS_GET, "telnet_service_enabled", NULL);
	if (pack && (pack->hdr.status == IPC_STATUS_OK))
    {
        memcpy(service, pack->value, sizeof(pack->value));
	}
    else
    {
        if(pack) 
        {
            free(pack);
        }
		return NULL;
	}
	
    if(pack) 
    {
        free(pack);
    }		
	return service;
	#endif  //add by zhouguanhua 2013/6/5
    char val[MAX_CONFIG_VAL_SIZE]={0};
    if(NULL == service)
    {
        return NULL;
    }
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_TELNET_SERVICE_ENABLE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get TELNET_SERVICE_ENABLE faild");
		return NULL;
	}
	safe_strncpy(service, val, sizeof(val));
	return service;
}

/*****************************************************************
    Function:ipc_get_vty_user_info_all
    Description: get all vty user infomation 
    Author:liaohongjun
    Date:2012/8/17
    Input:ipc_all_vty_user_info_t *vty_info     
    Output:         
    Return:
        IPC_STATUS_FAIL
        IPC_STATUS_OK
=================================================
    Note:
*****************************************************************/
int ipc_get_vty_user_info_all(ipc_all_vty_user_info_t *vty_info)
{
    #if 0 //add by zhouguanhua 2013/6/3
    ipc_get_user_info_ack_t *pack = NULL;
	int ret = IPC_STATUS_FAIL;
    
    if(NULL == vty_info)
    {
        return ret;
    }
    
    pack = ipc_nvram_get_user_info(ipc_fd, USER_VTY_INFO_ALL_GET, vty_info);
	if (pack && (pack->hdr.status == IPC_STATUS_OK))
    {
        memcpy(vty_info, pack->vty_user_info.vty_user, sizeof(ipc_all_vty_user_info_t));
		ret = pack->hdr.status;
	}
    else
    {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
	#endif //add by zhouguanhua 2013/6/3
	ipc_all_vty_user_info_t vty_user_info;
    int i=0,invalid=0;
    char val[20];  
    if(vty_info == NULL)
    {
        return IPC_STATUS_FAIL;
}
    for(i = MIN_VTY_USER_NO; i < MAX_VTY_COUNT; i++)
    {
        if(0 != cfg_getval(IF_ROOT_IFINDEX,CONFIG_USER_VTY_NAME+i+1,vty_user_info.vty_user[i].username,"default",sizeof(vty_user_info.vty_user[i].username)))
        {
            invalid++;
        }
        if(0 != cfg_getval(IF_ROOT_IFINDEX,CONFIG_USER_VTY_PASSWORD+i+1,vty_user_info.vty_user[i].passwd,"default",sizeof(vty_user_info.vty_user[i].passwd)))
        {
            invalid++;
        }
        if(0 != cfg_getval(IF_ROOT_IFINDEX,CONFIG_USER_VTY_AUTHMODE+i+1,vty_user_info.vty_user[i].authmode,"default",sizeof(vty_user_info.vty_user[i].authmode)))
        {
            invalid++;
        }
        if(0 != cfg_getval(IF_ROOT_IFINDEX,CONFIG_USER_VTY_PASSWORD_TYPE+i+1,vty_user_info.vty_user[i].passwd_type,"default",sizeof(vty_user_info.vty_user[i].passwd_type)))
        {
            invalid++;
        }
        if(0 != cfg_getval(IF_ROOT_IFINDEX,CONFIG_USER_VTY_TIMEOUT+i+1,(void *)val,"default",sizeof(val)))
        {
            invalid++;
        }

        if (val[0])
        {
    		vty_user_info.vty_user[i].timeout = strtoul(val, NULL, 0);       
    	}
        else 
        {
            invalid ++;
        }
        if (invalid > 0)
        {
    		DBG_ASSERT(0, "Invalid user info");
    		return IPC_STATUS_FAIL;
	    }        
    }    
    memcpy(vty_info,vty_user_info.vty_user, sizeof(ipc_all_vty_user_info_t));
    return IPC_STATUS_OK;
}
#if 0 //add by zhouguanhua
int ipc_set_vlan(int access, switch_vlan_group_t *vg)
{
	ipc_clt_vlan_config_ack_t *pack;
	int ret;
	
	pack = ipc_clt_vlan_config(ipc_fd, access, IPC_APPLY_NOW, vg, 1);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}







int ipc_get_interface(switch_interface_t *info)
{
	ipc_clt_interface_config_ack_t *pack;
	int ret;
	
	pack = ipc_clt_interface_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, info, 1);
	if (pack && (pack->hdr.status == IPC_STATUS_OK) && (pack->count == 1)) {
		if (info){
			memcpy(info, &pack->ifs, sizeof(*info));
		}
		ret = 0;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_set_interface(switch_interface_t *info)
{
	ipc_clt_interface_config_ack_t *pack;
	int ret;
	
	pack = ipc_clt_interface_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, info, 1);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}



int ipc_get_interface_statistics(char *ifname, switch_interface_mib_t *info)
{
	ipc_clt_interface_mibs_ack_t *pack;
	ipc_ifmibs_t req;
	int ret;

	safe_strncpy(req.ifname, ifname, sizeof(req.ifname));
	req.reset = 0;
		
	pack = ipc_clt_interface_mibs(ipc_fd, IPC_MIBS_GET, &req, 1);
	if (pack && (pack->hdr.status == IPC_STATUS_OK) && (pack->count == 1)) {
		if (info){
			memcpy(info, &pack->ifmibs, sizeof(*info));
		}
		ret = 0;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}

	
int ipc_get_interface_status(char *ifname, switch_interface_status_t *info)
{
	ipc_clt_interface_status_ack_t *pack;
	int ret;

	pack = ipc_clt_interface_status(ipc_fd, ifname);
	if (pack && (pack->hdr.status == IPC_STATUS_OK) && (pack->count == 1)) {
		if (info){
			memcpy(info, pack->status, sizeof(*info));
		}
		ret = 0;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_reset_interface_statistics(char *ifname)
{
	ipc_clt_interface_mibs_ack_t *pack;
	ipc_ifmibs_t req;
	int ret;

	safe_strncpy(req.ifname, ifname, sizeof(req.ifname));
	req.reset = 0;
	pack = ipc_clt_interface_mibs(ipc_fd, IPC_MIBS_RESET, &req, 1);
	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


#endif

int ipc_set_interface_switchport(switch_vlan_interface_t *vlanif)
{
	ipc_clt_vlan_interface_config_ack_t *pack;
	int ret;

	pack = ipc_clt_vlan_interface_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, vlanif, 1);

	if (pack){
		ret = pack->hdr.status;
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_get_cnu_info(int clt, int cnu,  ipc_cnu_info_t *info, int *cnu_status)
{
	ipc_cnu_info_ack_t *pack;
	int ret;
	
	*cnu_status = IPC_STATUS_OK;
		
	pack = ipc_cnu_info(ipc_fd, FILTER_TYPE_ID, clt, cnu, NULL);
	if (pack && (pack->hdr.status == IPC_STATUS_OK)) {
		if (info){
			memcpy(info, &pack->cnu_info, sizeof(*info));
		}
		ret = IPC_STATUS_OK;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		if ((pack->hdr.status == IPC_STATUS_NOCNU) 
			|| (pack->hdr.status == IPC_STATUS_NOCLT)){
			*cnu_status = pack->hdr.status;
			ret = IPC_STATUS_OK;
		}else {
			ret = pack->hdr.status;
		}
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_get_cnu_link_stats(int clt, int cnu,  cable_link_stats_t *info, int *cnu_status)
{
	ipc_cnu_link_stats_ack_t *pack;
	int ret;

	*cnu_status = IPC_STATUS_OK;
		
	pack = ipc_cnu_link_stats(ipc_fd, FILTER_TYPE_ID, clt, cnu, NULL);
	if (pack && (pack->hdr.status == IPC_STATUS_OK)) {
		if (info){
			memcpy(info, &pack->stats, sizeof(*info));
		}
		ret = IPC_STATUS_OK;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		if ((pack->hdr.status == IPC_STATUS_NOCNU) 
			|| (pack->hdr.status == IPC_STATUS_NOCLT)){
			*cnu_status = pack->hdr.status;
			ret = IPC_STATUS_OK;
		}else {
			ret = pack->hdr.status;
		}
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}


int ipc_get_cnu_port_stats(int clt, int cnu, ipc_cnu_port_mib_t *info, int *port_num, int *cnu_status)
{
	ipc_cnu_mib_ack_t *pack;
	int ret;

	*cnu_status = IPC_STATUS_OK;
		
	pack = ipc_cnu_mib(ipc_fd, 0, FILTER_TYPE_ID, clt, cnu, NULL);
	if (pack && (pack->hdr.status == IPC_STATUS_OK)) {
		if (info){
			*port_num = pack->ports;
			memcpy(info, pack->cnu_mib, sizeof(*info) * pack->ports);
		}
		ret = IPC_STATUS_OK;
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		if ((pack->hdr.status == IPC_STATUS_NOCNU) 
			|| (pack->hdr.status == IPC_STATUS_NOCLT)
			|| (pack->hdr.status == IPC_STATUS_CNUDOWN)
			|| (pack->hdr.status == IPC_STATUS_CNUNOSUPP)
			|| (pack->hdr.status == IPC_STATUS_CNUNORESP)){
			*cnu_status = pack->hdr.status;
			ret = IPC_STATUS_OK;
		}else {
			ret = pack->hdr.status;
		}
	}else {
		ret = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);		
	return ret;
}



#define VTY_MAX_VLAN_STRING_SIZE_PER_LINE 50
#if 0 //add by zhouguanhua

void vty_out_vlans(struct vty* vty, const char *prefix, switch_vlan_group_t *vg)
{
	int i;
	char buffer[4094*4];
	char *cp, *prev, *p;
	char bfix[256];
	int prefix_len = strlen(prefix);

	for (i = 0; (i < sizeof(bfix) - 1) && (i < prefix_len); i ++){
		bfix[i] = ' ';
	}
	bfix[i] = '\0';

	vlan_group_sprintf(buffer, vg);
	p = prev = buffer;
	while((cp = strchr(p, ',')) != NULL){
		if (cp - prev > VTY_MAX_VLAN_STRING_SIZE_PER_LINE){
			*cp = 0;
			vty_out(vty, "%s %s%s", (prev == buffer) ? prefix : bfix, prev, VTY_NEWLINE);
			prev = cp + 1;
		}
		p = cp + 1;
	}
	// don't forget the last line
	vty_out(vty, "%s %s%s", (prev == buffer) ?  prefix : bfix, prev, VTY_NEWLINE);
}

#endif  //add by zhouguanhua


/*
void clt_vlan_group_dump(switch_vlan_group_t *vlan_groups, uint16_t count)
{
	int i, v;
	switch_vlan_group_t *group;
	group = vlan_groups;
	for (i = 0; i < count; i ++){
		fprintf(stderr, "Group<%d>\n", i);
		fprintf(stderr, "\tMember:%s\n", vlan_member_string(group->member));
		fprintf(stderr, "\tVlans :");
		for (v = 0; v < group->count; v ++){
			if (is_vlan_range(&group->vlans[v])){
				fprintf(stderr, "%d-%d[%X],", 
					group->vlans[v].id_range.start,
					group->vlans[v].id_range.end,
					group->vlans[v].member);
			}else{
				fprintf(stderr, "%d[%X]%s,", 
					group->vlans[v].id.value, 
					group->vlans[v].member,
					group->vlans[v].id.flag ? "*" : "");
			}
			if (v && !(v & 0x0f)) fprintf(stderr, "\n\t       ");
		}
		fprintf(stderr, "\n");			
		group = vlan_group_to_next(group);
	}
}

*/

static int vlan_item_check(const char *item)
{
	int mi, ma, ret = 0;
	char *idstr = strdup(item);
	char *p = strchr(idstr, '-');
	if (p != NULL){
		*p = 0;
		mi = strtoul(idstr, NULL, 0);
		ma = strtoul(p + 1, NULL, 0);
		if ((mi <= ma) && (mi > 0) && (ma < 4095)){
			ret = 1;
		}
	}else {
		mi = strtoul(idstr, NULL, 0);
		if (mi > 0 && mi < 4095){
			ret = 1;
		}
	}
	free(idstr);
	return ret;
}


int vlan_string_check(const char *vstr)
{
	char *vars[4096];
	int items, i, ret = 1;
	char *vs = strdup(vstr);
	char *p, *pm, *pn;
/*
	var vstr= /^((\d{1,4}\-\d{1,4}|\d{1,4})\,)*(\d{1,4}\-\d{1,4}|\d{1,4})$/; 
	if (!vstr.test(s)) return false;
*/
	// Step 1, string check
	p = vs;
	do {
		pm = strchr(p, ',');
		pn = strchr(p, '-');
		if ((pn != NULL) && (pm != NULL)){
			pm = (pn < pm) ? pn : pm;
		}else if ((pn != NULL) && (pm == NULL)){
			pm = pn;
		}
		if (pm != NULL){
			if (pm == vs)ret = 0;
			if ((pm - p) > 4)ret = 0;
			for (pn = p; pn < pm; pn ++){
				if (!isdigit(*pn)) { ret = 0; break;}
			}
			if (*(pm + 1) == '\0') ret = 0;
			p = pm + 1;	
		}else {
			for (pn = p; *pn > 0; pn ++){
				if (!isdigit(*pn)) { ret = 0; break;}
			}			
		}
	}
	while((pm != NULL) && (ret > 0));
	

	// Step 2, items check
	if (ret > 0){
		items = split_string(vs, ',', vars);	
		for (i = 0; i < items; i ++){
			if (!vlan_item_check(vars[i])){
				ret = 0;
				break;
			}
		}
	}
	
	free(vs);
	return ret;
}



int all_vlan_exist(switch_vlan_group_t *vg, int *ipc_assert)
{
	int ipc = IPC_STATUS_OK, ret = 0;
	ipc_clt_vlan_config_ack_t *pack;

	pack = ipc_clt_vlan_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW, NULL, 0);
	
	if (pack && (pack->hdr.status == IPC_STATUS_OK)) {
		ret = all_vlan_in_groups(vg, pack->vlan_groups, pack->count);
	}else if (pack && (pack->hdr.status != IPC_STATUS_OK)){
		ipc = pack->hdr.status;
	}else {
		ipc = IPC_STATUS_FAIL;
	}
	
	if(pack) free(pack);

	if (ipc_assert) *ipc_assert = ipc;

	return ret;	
}

int vlan_exist(int vid, int *ipc_assert)
{
    //add by zhouguanhua
	char buffer[sizeof_vlan_group(1)];
	switch_vlan_group_t *vg = (switch_vlan_group_t *)buffer;
	memset(buffer, 0, sizeof(buffer));
	vg->count = 1;
	vg->vlans[0].id.flag = 0;
	vg->vlans[0].id.value = vid;
	return all_vlan_exist(vg, ipc_assert);
	
}
#if 0 //add by zhouguanhua 2013/5/31
/*****************************************************************
    Function:ipc_vtysh_nvram_set
    Description: 命令行下发配置接口，也可以直接调用
                        ipc_nvram_access接口，调用ipc_nvram_access
                        接口要保证释放分配的内存.
    Author:liaohongjun
    Date:2012/9/5
    Input:char *pName, char *pValue       
    Output:         
    Return:
        IPC_STATUS_OK
        IPC_STATUS_FAIL
=================================================
    Note:added for QID0015
*****************************************************************/
int ipc_vtysh_nvram_set(char *pName, char *pValue)
{
    ipc_nvram_access_ack_t *pack = NULL;
    int iRet = IPC_STATUS_FAIL;
    
    if(NULL == pName)
    {
        return IPC_STATUS_FAIL;
    }
    
    pack = ipc_nvram_access(ipc_fd, NVRAM_ACCESS_SET, pName, pValue);
    if(pack && (IPC_STATUS_OK != pack->hdr.status))
    {
        iRet = pack->hdr.status;
    }
    else
    {
        iRet = IPC_STATUS_OK;
    }
    
    if(pack) 
    {
        free(pack);
    }
    return iRet;
}
#endif //add by zhouguanhua 2013/5/31

/*****************************************************************
    Function:ipc_vtysh_nvram_set
    Description: 命令行下发配置接口，也可以直接调用
    Author:zhouguanhua
    Date:2013/6/5
    Input:char *pName, char *pValue       
    Output:         
    Return:
        IPC_STATUS_OK
        IPC_STATUS_FAIL
=================================================
*****************************************************************/
int ipc_cfg_set_user(char *pName, char *pValue)
{

    char name[64]={0};
    int i=0;
    int invalid=0;
    if(NULL == pName || NULL == pValue)
    {   
        return IPC_STATUS_FAIL;
    }

    for(i = MIN_VTY_USER_NO; i < MAX_VTY_COUNT; i++)
    {
        memset (name,0,sizeof(name));
        sprintf(name, PARTS_VTY_USERNAME_ITEM,i);

        if(!strcmp(name,pName))
        {
            if (ipc_cfg_set(IF_ROOT_IFINDEX,CONFIG_USER_VTY_NAME+i+1,pValue) != IPC_STATUS_OK)
            {
                invalid++;
            }
            break;
        }
        
        memset (name,0,sizeof(name));
        sprintf(name, PARTS_VTY_AUTHMODE_ITEM,i);
        if(!strcmp(name,pName))
        {
            if (ipc_cfg_set(IF_ROOT_IFINDEX,CONFIG_USER_VTY_AUTHMODE+i+1,pValue) != IPC_STATUS_OK)
            {
                invalid++;
            }
            break;
        }

        memset (name,0,sizeof(name));
        sprintf(name, PARTS_VTY_AUTHPASSWD_ITEM,i);
        if(!strcmp(name,pName))
        {
            if (ipc_cfg_set(IF_ROOT_IFINDEX,CONFIG_USER_VTY_PASSWORD+i+1,pValue) != IPC_STATUS_OK)
            {
                invalid++;
            }
            break;
        }

        memset (name,0,sizeof(name));
        sprintf(name, PARTS_VTY_PASSWD_TYPE_ITEM,i);
        if(!strcmp(name,pName))
        {
            if (ipc_cfg_set(IF_ROOT_IFINDEX,CONFIG_USER_VTY_PASSWORD_TYPE+i+1,pValue) != IPC_STATUS_OK)
            {
                invalid++;
            }
            break;
        }
        
        memset (name,0,sizeof(name));
        sprintf(name, PARTS_VTY_TIMEOUT_ITEM,i);
        if(!strcmp(name,pName))
        {
            if (ipc_cfg_set(IF_ROOT_IFINDEX,CONFIG_USER_VTY_TIMEOUT+i+1,pValue) != IPC_STATUS_OK)
            {
                invalid++;
            }
            break;
        }      
    } 
    
    if (invalid > 0)
    {
        DBG_ASSERT(0, "Invalid user info");
        return IPC_STATUS_FAIL;
    }
    return IPC_STATUS_OK;    
}



/*****************************************************************
    Function:ipc_cfg_set_telnetService
    Description: 
    Author:zhouguanhua
    Date:2013/6/5
    Input:char *pName, char *pValue       
    Output:         
    Return:
        IPC_STATUS_OK
        IPC_STATUS_FAIL
=================================================
*****************************************************************/
int ipc_cfg_set_telnetService(char *pName, char *pValue)
{
    if(!strcmp("telnet_service_enabled",pName))
    {
        if (ipc_cfg_set(IF_ROOT_IFINDEX,CONFIG_TELNET_SERVICE_ENABLE,pValue) != IPC_STATUS_OK)
        {
            return IPC_STATUS_FAIL;   
        } 
    }
    else
    {
        return IPC_STATUS_FAIL;    
    }
    return IPC_STATUS_OK;    
}

 
 /*****************************************************************
     Function:ipc_get_sys_snmp_value
     Description: 
     Author:linguobin
     Date:2013/10/23
     Input:sys_snmp_t *sys, int cfgdomain   
     Output:         
     Return:
         IPC_STATUS_OK
         IPC_STATUS_FAIL
 =================================================
 *****************************************************************/
 int  ipc_get_sys_snmp_value(sys_snmp_t *sys,int cfgdomain)
{
	int invalid = 0;
	uint8_t i = 0;
	char val[1024]; 
    ifindex_t ifindex=IF_ROOT_IFINDEX;
    if(sys == NULL)
    {
        return IPC_STATUS_FAIL;
    }

    if(0 == cfgdomain)
    {
        ifindex=IF_ROOT_IFINDEX;
    }
    else
    {
        ifindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_VIRTUAL,0);
    }      
	/*agent */
    /*snmp_enabled*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SNMP_ENABLED, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_en  faild");
    }
	if(!strcmp(val, "enable"))
	{
		sys->snmp_en = 1;
	}
	else if(!strcmp(val, "disable"))
	{
		sys->snmp_en = 0;
	}
	else
	{
		invalid++;
	}
	
    /*snmp_engineID*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SNMP_ENGINEID, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_engineID  faild");
    }
    if(val[0])
    {       
        safe_strncpy(sys->snmp_engineID, val, sizeof(sys->snmp_engineID));
    }
    else
    {
        invalid++;
    }
    /*snmp_max_packet_len*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SNMP_MAX_PACKET_LEN, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_max_packet_len  faild");
    }
	if (val[0])
	{
		sys->snmp_max_packet_len = strtoul(val, NULL, 0);
	}
	else 
	{
		invalid++;
	}

	/*snmp_location*/
    if((cfg_getval(ifindex, CONFIG_SNMP_LOCATION, (void *)sys->snmp_location, "", sizeof(sys->snmp_location))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_location  faild");
    }

    /*snmp_contact*/
    if((cfg_getval(ifindex, CONFIG_SNMP_CONTACT, (void *)sys->snmp_contact, "", sizeof(sys->snmp_contact))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_contact  faild");
    }

    /*snmp_version*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SNMP_VERSION, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_version  faild");
    }
	if (val[0])
	{
		sys->snmp_version = strtoul(val, NULL, 0);
        if(NULL != strstr(val, "V1"))
        {
            sys->snmp_version= SNMP_V1_VAL;
        }
        else if(NULL != strstr(val, "V2c"))
        {
            sys->snmp_version= SNMP_V2c_VAL;
        }
        else if(NULL != strstr(val, "V3"))
        {
            sys->snmp_version= SNMP_V3_VAL;
        }		
	}
	else 
	{
		invalid++;
	}
    
	/*snmp_trust_host*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SNMP_TRUST_HOST, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trust_host  faild");
    }
    if(val[0])
    {       
        safe_strncpy(sys->snmp_trust_host, val, sizeof(sys->snmp_trust_host));
    }
    else
    {
        invalid++;
    }
	
	/*add community*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SNMP_COMM_CURRENT_COUNT, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_comm_current_count  faild");
    }
	if (val[0])
	{
		sys->snmp_comm_current_count = strtoul(val, NULL, 0);
	}
	else 
	{
		invalid++;
	}
	
	for(i=0; i< MAX_SNMP_COMM_COUNT; i++)
	{	
        if((cfg_getval(ifindex, CONFIG_SNMP_COMM_NAME+i+1, (void *)sys->snmp_comm[i].snmp_comm_name, "", sizeof(sys->snmp_comm[i].snmp_comm_name))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_comm_name  faild");
            return IPC_STATUS_FAIL;
        }

        memset(val, 0, sizeof(val));  
        if((cfg_getval(ifindex, CONFIG_SNMP_ACCESS_MODE+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_access_mode  faild");
            return IPC_STATUS_FAIL;
        }

		if(!strcmp(val, "read_write"))
		{
			sys->snmp_comm[i].snmp_access_mode = SNMP_ACESS_MODE_RW;
		}
		else if(!strcmp(val, "read_only"))
		{
			sys->snmp_comm[i].snmp_access_mode = SNMP_ACESS_MODE_RO;
		}
		else 
		{
		    sys->snmp_comm[i].snmp_access_mode=SNMP_ACESS_MODE_NULL;//NULL
		}
	}

	
	/*add*/
	sys->snmp_add_which_num = NO_ADD_NUM; 
	/*del */
	sys->snmp_del_which_num = NO_DEL_NUM; 

	sys->snmp_set_flag = 0;

	/*trap*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SNMP_TRAP_EN, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_en  faild");
    }

	if (val[0]){
		sys->snmp_trap_en = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
	
    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SNMP_TRAP_IP, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_ip  faild");
    }

	if (!val[0] || !inet_aton(val, &sys->snmp_trap_ip)){
		invalid ++;		
	}

    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SNMP_TRAP_COM, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_com  faild");
    }
    /*snmp_trap_com*/

	if (val[0]){
        sys->snmp_trap_com = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}

    /*snmp_trap_community*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SNMP_TRAP_COMMUNITY, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_community  faild");
    }

	if (val[0]){
		strncpy(sys->snmp_trap_community, val, sizeof(sys->snmp_trap_community));
	}else {
		invalid ++;
	}

    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SNMP_TRAP_VERSION, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_version  faild");
    }

	if (val[0]){
		sys->snmp_trap_version = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}

    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SNMP_TRAP_SAFE, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_safe  faild");
    }

	if (val[0]){
        sys->snmp_trap_safe = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}

    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SNMP_LAST_SAVE_SAFE_NUM, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_last_save_safe_num  faild");
    }

	if (val[0]){
        sys->snmp_last_save_safe_num = strtoul(val, NULL, 0);
	}else {
		invalid ++;
    }

     memset(val, 0, sizeof(val));  
     if((cfg_getval(ifindex, CONFIG_SNMP_GROUP_CURRENT_COUNT, (void *)val, "", sizeof(val))) < 0)
     {
         DBG_ASSERT(0, "Get snmp_group_current_count  faild");
     }

	if (val[0])
	{
		sys->snmp_group_current_count = strtoul(val, NULL, 0);
	}
	else 
	{
		invalid++;
	}
	
	for(i=0; i< MAX_SNMP_GROUP_COUNT; i++)
	{	
         if((cfg_getval(ifindex, CONFIG_SNMP_GROUP_NAME+i+1, (void *)sys->snmp_group[i].snmp_group_name, "", sizeof(sys->snmp_group[i].snmp_group_name))) < 0)
         {
             DBG_ASSERT(0, "Get snmp_group_current_count  faild");
             return IPC_STATUS_FAIL;
         }		

        memset(val, 0, sizeof(val));  
        if((cfg_getval(ifindex, CONFIG_SNMP_GROUP_SECURITY+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_group_current_count  faild");
            return IPC_STATUS_FAIL;
        }
		if(!strcmp(val, "noauthnopriv"))
		{
			sys->snmp_group[i].snmp_group_security = SNMP_GROUP_SECU_NOAUTHNOENCRYPT;
		}
		else if(!strcmp(val, "authnopriv"))
		{
			sys->snmp_group[i].snmp_group_security = SNMP_GROUP_SECU_AUTHNOENCRYPT;
		}
		else if(!strcmp(val, "authpriv"))
		{
			sys->snmp_group[i].snmp_group_security = SNMP_GROUP_SECU_AUTHENCRYPT;
		}
		else
		{
			sys->snmp_group[i].snmp_group_security = SNMP_GROUP_SECU_NULL;
		}

        memset(val, 0, sizeof(val));  
        if((cfg_getval(ifindex, CONFIG_SNMP_GROUP_MODE+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_group_mode  faild");
            return IPC_STATUS_FAIL;
        }

		if(!strcmp(val, "read_write"))
		{
			sys->snmp_group[i].snmp_group_mode = SNMP_ACESS_MODE_RW;
		}
		else if(!strcmp(val, "read_only"))
		{
			sys->snmp_group[i].snmp_group_mode = SNMP_ACESS_MODE_RO;
		}
		else
		{   
		    sys->snmp_group[i].snmp_group_mode = SNMP_ACESS_MODE_NULL;
		}
	}	

    memset(val, 0, sizeof(val));  
    if((cfg_getval(ifindex, CONFIG_SNMP_USER_CURRENT_COUNT, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_user_current_count  faild");
    }

	if (val[0])
	{
		sys->snmp_user_current_count = strtoul(val, NULL, 0);
	}
	else 
	{
		invalid++;
	}

	for(i=0; i< MAX_SNMP_USER_COUNT; i++)
	{	
        memset(val, 0, sizeof(val));  
        if((cfg_getval(ifindex, CONFIG_SNMP_USER_NMAE+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_name  faild");
            return IPC_STATUS_FAIL;
        }	
		safe_strncpy(sys->snmp_user[i].snmp_user_name, val, sizeof(sys->snmp_user[i].snmp_user_name));

        memset(val, 0, sizeof(val));  
        if((cfg_getval(ifindex, CONFIG_SNMP_USER_GROUP+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_group  faild");
            return IPC_STATUS_FAIL;
        }	
		safe_strncpy(sys->snmp_user[i].snmp_user_group, val, sizeof(sys->snmp_user[i].snmp_user_group));

        memset(val, 0, sizeof(val));  
        if((cfg_getval(ifindex, CONFIG_SNMP_PASSWORD+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_password  faild");
            return IPC_STATUS_FAIL;
        }	
		safe_strncpy(sys->snmp_user[i].snmp_user_password, val, sizeof(sys->snmp_user[i].snmp_user_password));

        memset(val, 0, sizeof(val));  
        if((cfg_getval(ifindex, CONFIG_SNMP_ENPASSWORD+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_enpassword  faild");
            return IPC_STATUS_FAIL;
        }	
		safe_strncpy(sys->snmp_user[i].snmp_user_enpassword, val, sizeof(sys->snmp_user[i].snmp_user_enpassword));

        memset(val, 0, sizeof(val));  
        if((cfg_getval(ifindex, CONFIG_SNMP_SECURITY+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_name  faild");
            return IPC_STATUS_FAIL;
        }	
        
		if(!strcmp(val, "noauthnopriv"))
		{
			sys->snmp_user[i].snmp_user_security = SNMP_USER_SECU_NOAUTHNOENCRYPT;
		}
		else if(!strcmp(val, "authnopriv"))
		{
			sys->snmp_user[i].snmp_user_security = SNMP_USER_SECU_AUTHNOENCRYPT;
		}
		else if(!strcmp(val, "authpriv"))
		{
			sys->snmp_user[i].snmp_user_security = SNMP_USER_SECU_AUTHENCRYPT;
		}
		else 
		{
			sys->snmp_user[i].snmp_user_security = SNMP_USER_SECU_NULL;
		}


	}	

	if (invalid > 0)
	{
		DBG_ASSERT(0, "Invalid SYS SNMP INFO");
		return IPC_STATUS_FAIL;
	}
	
	return IPC_STATUS_OK;
}


#if 0 //add by zhouguanhua 2013/6/5 
/*****************************************************************
    Function:ipc_vtysh_nvram_get
    Description: 命令行获取配置接口，也可以直接调用
                        ipc_nvram_access接口，调用ipc_nvram_access
                        接口要保证释放分配的内存.
    Author:liaohongjun
    Date:2012/9/5
    Input:char *pName, CFG_VALUE_S *pstValue       
    Output:         
    Return:
        IPC_STATUS_OK
        IPC_STATUS_FAIL
=================================================
    Note:added for QID0015
*****************************************************************/
int ipc_vtysh_nvram_get(char *pName, CFG_VALUE_S *pstValue)
{
    ipc_nvram_access_ack_t *pack = NULL;
    int iRet = IPC_STATUS_FAIL;
    
    if((NULL == pName) || (NULL == pstValue))
    {
        return IPC_STATUS_FAIL;
    }
    
    pack = ipc_nvram_access(ipc_fd, NVRAM_ACCESS_GET, pName, NULL);
    if(pack && (IPC_STATUS_OK != pack->hdr.status))
    {
        iRet = pack->hdr.status;
    }
    else
    {
        memcpy(pstValue->value, pack->value, sizeof(*pstValue));
        iRet = IPC_STATUS_OK;
    }
    
    if(pack) 
    {
        free(pack);
    }
    return iRet;
}

/*****************************************************************
    Function:ipc_vtysh_nvram_unset
    Description: 命令行取消配置接口，也可以直接调用
                        ipc_nvram_access接口，调用ipc_nvram_access
                        接口要保证释放分配的内存.
    Author:liaohongjun
    Date:2012/9/5
    Input:char *pName      
    Output:         
    Return:
        IPC_STATUS_OK
        IPC_STATUS_FAIL    
=================================================
    Note:added for QID0015
*****************************************************************/
int ipc_vtysh_nvram_unset(char *pName)
{
    ipc_nvram_access_ack_t *pack = NULL;
    int iRet = IPC_STATUS_FAIL;
    
    if(NULL == pName)
    {
        return IPC_STATUS_FAIL;
    }  
    
    pack = ipc_nvram_access(ipc_fd, NVRAM_ACCESS_UNSET, pName, NULL);
    if(pack && (IPC_STATUS_OK != pack->hdr.status))
    {
        iRet = pack->hdr.status;
    }
    else
    {
        iRet = IPC_STATUS_OK;
    }
    
    if(pack) 
    {
        free(pack);
    }
    return iRet;
}
#endif  //add by zhouguanhua 2013/6/5 

char *BYTES_STR(uint64_t n)
{
	static char buffer[128];
	double f, v;
	f = n*1.0;
	
	if (n < (1024*1024*1024UL)){
		v = f / (1024*1024UL);
		sprintf(buffer, "%llu (%.3fMbps)", n, v);
	}else {
		v = f / (1024*1024*1024UL);		
		sprintf(buffer, "%llu (%.2fGbps)", n, v);
	}
	return buffer;
}


char *RATE_STR(unsigned long n)
{
	static char buffer[64];
	if (n == 0){
		return "Off";
	}else {
		sprintf(buffer, "%luMbps", n);
		return buffer;
	}
}
//add by zhouguanhua  2013/5/31
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

typedef enum {
	ETH_SPEED_10 = 0,
	ETH_SPEED_100,
	ETH_SPEED_1000,
	ETH_SPEED_END
}
clt_eth_speed_t;

//end add 2013/5/31

char *PRIORITY_STR(int n)
{
	static char buffer[64];
	if (n == ETH_PRIORITY_OFF){
		return "Off";
	}else {
		sprintf(buffer, "%d", n);
		return buffer;
	}
}


char *SPEED_STR(int n)
{
	if (n == ETH_SPEED_10){
		return "10M";
	}else if (n == ETH_SPEED_100){
		return "100M";
	}else if (n == ETH_SPEED_1000){
		return "1000M";
	}else {
		return "";
	}
}

char *FLOAT_STR(float  v, char *tail)
{
 static char buffer[24];
 sprintf(buffer, "%.3f%s", v, tail);
 return buffer;
}

char *FLOATT_STR(float  v, char *tail)
{
 static char buffer[24];
 sprintf(buffer, "%s%6.3f%s", (v >= 60) ? ">=" : "", v, tail);
 return buffer;
}


char *CNU_LINK_STR(int link)
{
	switch(link)
	{
		case CNU_LINK_UP:
			return "Up";
		case CNU_LINK_DENY:
			return "Deny";
		case CNU_LINK_DOWN:
			return "Down";
		case CNU_LINK_INVALID:
			return "Invalid";
	}
	return "Unknown";
}



char *CNU_AUTH_STR(int auth)
{
	switch(auth)
	{
		case CNU_AUTH_ANONACCEPT:
			return "Anonoymous";
		case CNU_AUTH_ACCEPT:
			return "Accepted";
		case CNU_AUTH_DENY:
			return "Deny";
	}
	return "Unknown";

}

char *CNU_DEVID_STR(int id)
{
	static char buffer[64];
	if (id < 0){
		return "Unknown";
	}
	sprintf(buffer, "%d", id);
	return buffer;
}

char *CNU_TMPLID_STR(int id)
{
	static char buffer[64];
	if (id < 0){
		return "Unknown";
	}
	sprintf(buffer, "%d", id);
	return buffer;
}


/**********************************************************
**  Copyright (C) 2012-7-11, LOSOTECH Co. Ltd.
**  All rights reserved.
**  
**  FileName:       command.c
** Function:         get_hostname_from_master
**  Description:    get hostname from master when we hit any key  (EPN104QID0058)  
**  Author:           huangmingjian
**  Date:             2012/9/25
**  Others:          
**********************************************************/

int update_prompt_from_web()
{	
	sys_network_t info;
    char val[256]={0};
	memset(&info, 0, sizeof(info));
	//ret = ipc_get_sys_networking(&info);  
    memset(val, 0, sizeof(val));
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSTEM_NAME, (void *)val, "", sizeof(val))) < 0)
    {
        return -1;
    }    
    safe_strncpy(info.hostname, val, sizeof(info.hostname));
	if(0 != strcmp(host.name, info.hostname))	
	{	
		strncpy(ht_init.ht_name, info.hostname, sizeof(ht_init.ht_name) - 1); 
		if (host.name)
		{
			XFREE (MTYPE_HOST, host.name);
		}
		host.name = NULL;	
		host.name = XSTRDUP (MTYPE_HOST, info.hostname);
	}
	return 0;
}

