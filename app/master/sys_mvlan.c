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
#include <ipc_protocol.h>
#include <systemlog.h>

unsigned short LocalM_vlan = 1;

void sys_mvlan_load(sys_mvlan_t *sys)
{
	int invalid = 0;
	char val[32];  

	/* vlan_manage_vid */		
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_MVLAN_VID, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get mvlan id  faild");
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
    }
	if (val[0])
    {
		sys->vlan_interface = strtoul(val, NULL, 0);       
	}
    else 
    {
        invalid ++;
    }

	if (invalid > 0)
	{
		DBG_ASSERT(0, "Invalid mvlan INFO");
	}
	set_update(mvlan);
}

int sys_mvlan_apply(int apply_options)
{   
	sys_mvlan_t *sys = &sys_mvlan;
	sys_network_t *network_info = &sys_network;
	char val[256] = {0};
    IF_INFO_S ifinfo;
	ifindex_t ifindex=0,oltifindex;
	ifindex_t pifindex=0;
	IF_RET_E ret=0;
//    char ip_val[16];
    if(test_update(mvlan))
    {				
        /*begin added by liaohongjun 2012/9/18 of EPN104QID0087*/
        (void)Drv_SetManageVlan((UINT32)sys->m_vlan);
        /*end added by liaohongjun 2012/9/18 of EPN104QID0087*/

		if(apply_options==IPC_APPLY_NONE)
		{
			LocalM_vlan = sys->m_vlan;
			#if 0//defined(CONFIG_PRODUCT_EPN104W)
			memset(val, 0, sizeof(val));
			sprintf(val, "brctl addif br0 %s.%d", IF_L3VLAN_BASE, LocalM_vlan);
			system(val);
			#endif
			return 0;
		}

		if(sys->m_vlan==LocalM_vlan)
		    return 0;
		
		/*delete old l3van*/
		memset(val,0,sizeof(val));
		sprintf(val,"%s%d",IF_L3VLAN_NAMEPREFIX,LocalM_vlan);
		IF_GetByIFName(val,&oltifindex);	
		
        
		/*create new l3van*/
		ifinfo.enSubType=IF_SUB_L3VLAN;
		IF_GetByIFName(IF_L3VLAN_BASE,&pifindex);
		sprintf(ifinfo.szIfName,"%s%d",IF_L3VLAN_NAMEPREFIX,sys->m_vlan);
		sprintf(ifinfo.szPseudoName,"%s%d",IF_L3VLAN_NAMEPREFIX,sys->m_vlan);
        ret=IF_GetByIFName(ifinfo.szIfName,&ifindex);
        if(IF_OK!=ret)
        {
			ifinfo.uiSpecIndex=IF_INVALID_IFINDEX;		
    		ifinfo.uiParentIfindex=pifindex;
			ret=IF_CreatInterface(&ifinfo,&ifindex);
		}
      
        IF_GetAttr(ifindex,IF_LINKNAME,network_info->ifname,sizeof(network_info->ifname)); 
		/*begin added by wanghuanyu for 216*/
		memset(val, 0, sizeof(val));
		if((cfg_getval(oltifindex, CONFIG_DIAL_TYPE, (void *)val, "", sizeof(val))) < 0)
		{
			DBG_ASSERT(0, "Get IP PROTO faild");
		}
		 master_cfg_setval(ifindex,CONFIG_DIAL_TYPE, val);	
		/* ipaddress */
		memset(val, 0, sizeof(val));		
		if(cfg_getval(oltifindex,CONFIG_IP_ADDR,val,"",sizeof(val))!=0)
		{				
		  DBG_ASSERT(0, "Get IP faild");
		} 
		master_cfg_setval(ifindex,CONFIG_IP_ADDR, val);		
		
		/*ipmask*/
		memset(val, 0, sizeof(val));
		if((cfg_getval(oltifindex,CONFIG_IP_MASK , (void *)val, "", sizeof(val))) < 0)
		{
			DBG_ASSERT(0, "Get NETMASK faild");
		}
		master_cfg_setval(ifindex,CONFIG_IP_MASK, val);	
			
		/*gateway*/
		memset(val, 0, sizeof(val));
		if((cfg_getval(oltifindex, CONFIG_IP_GATEWAY, (void *)val, "", sizeof(val))) < 0)
		{
			DBG_ASSERT(0, "Get GATEWAY faild");
		}
		master_cfg_setval(ifindex,CONFIG_IP_GATEWAY, val);
		
		IF_DeleteInterface(oltifindex);
		/*end added by wanghuanyu for 216*/
		
		#if 0//defined(CONFIG_PRODUCT_EPN104W)
		memset(val, 0, sizeof(val));
		sprintf(val, "brctl delif br0 %s.%d", IF_L3VLAN_BASE, LocalM_vlan);
		system(val);		
		LocalM_vlan = sys->m_vlan;		
		memset(val, 0, sizeof(val));
		sprintf(val, "brctl addif br0 %s.%d", IF_L3VLAN_BASE, LocalM_vlan);
		system(val);
		#else
		LocalM_vlan = sys->m_vlan;
		#endif
		
		printf("LocalM_vlan %d\n",LocalM_vlan);
        master_cfg_setval(ifindex,CONFIG_IF_ENABLE, ENUMTOSTR(IF_MOD_ENABLE));
		IF_Enable(ifindex);
        clr_update(mvlan);
		USER_LOG(LangId,MVLAN,LOG_INFO,M_MVLAN_STA,"",""); //add by leijinbao 2013/9/17
    }
    
	return 0;
}


int sys_mvlan_update(sys_mvlan_t *cfg)
{
	sys_mvlan_t *sys = &sys_mvlan;
//	sys_network_t *network_info = &sys_network;
	int update = 0;
  //  char val[256] = {0};
    if (sys->m_vlan != cfg->m_vlan)
    {   
        #if 0  
        eval("brctl", "delif", SYS_NETIF_BRIDGE, network_info->ifname);
        eval("vconfig", "rem", network_info->ifname);  

        sys->m_vlan = cfg->m_vlan;
        sys->vlan_interface = cfg->vlan_interface; 

        memset(val, 0x0, sizeof(val));
        sprintf(val, "eth0.%u", sys->vlan_interface); 
        memset(network_info->ifname, 0x0, sizeof(network_info->ifname));      
        safe_strncpy(network_info->ifname, val, sizeof(network_info->ifname));  

        update ++;
        set_update(mvlan);
         #endif  
		 #if 0
        eval("vconfig", "rem", network_info->ifname);  

        sys->m_vlan = cfg->m_vlan;
        sys->vlan_interface = cfg->vlan_interface; 
         
        memset(val, 0x0, sizeof(val));
        sprintf(val, "eth0.%u", sys->vlan_interface); 
        memset(network_info->ifname, 0x0, sizeof(network_info->ifname));      
        safe_strncpy(network_info->ifname, val, sizeof(network_info->ifname));  
		#endif
		sys->m_vlan = cfg->m_vlan;
        sys->vlan_interface = cfg->vlan_interface;
		sys_security.mvlan = cfg->m_vlan;
		sys_info.mvlan = cfg->m_vlan; 	
        update ++;
       
        set_update(mvlan);
    }
	return update;
}


int sys_mvlan_commit(int apply_options)
{
	sys_mvlan_t *sys = &sys_mvlan;
    char val[5]={0};

    memset(val, 0, sizeof(val));
    sprintf(val,"%d",sys->m_vlan);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_MVLAN_VID, val))
	{
		return -1;	
	}

    memset(val, 0, sizeof(val));
    sprintf(val,"%d",sys->vlan_interface);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_MVLAN_INTERFACE, val))
	{
		return -1;	
	}		
	return 1;
}




