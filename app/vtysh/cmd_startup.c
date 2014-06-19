/*****************************************************************************

      File name:cmd_starup.c
      Description:for cmd show startup-config
      Author:liaohongjun
      Date:
              2012/8/21
==========================================================
      Note:
*****************************************************************************/

#ifdef  __cplusplus
    extern "C"{
#endif

#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include "eoc_utils.h"
#include "debug.h"
#include "cmd_eoc.h"
#include "user.h"
#include "vty.h"
//#include "nvram.h"
#include <lw_config_oid.h>
#include <lw_config_api.h>
#include <lw_if_pub.h>
#include <if_notifier.h>

typedef enum{
    STARTUP_OK = 0,
    STARTUP_ERROR, 
    STARTUP_ERROR_PARA,
    STARTUP_ERROR_UNKNOWN
}RET_STARTUP_E;
#if 0 //add by zhouguanhua 2013/5/31

static nvram_handle_t *startup_nvram_handle = NULL;

static RET_STARTUP_E startup_nvram_get_vlan_interface_info(sys_network_t *info);

/*****************************************************************
    Function:startup_nvram_handle_init
    Description: 从flash 中获取系统保存的配置
    Author:liaohongjun
    Date:2012/8/21
    Input:   
    Output:         
    Return:
        STARTUP_ERROR
        STARTUP_OK
=================================================
    Note:
*****************************************************************/
static RET_STARTUP_E startup_nvram_handle_init(void)
{
    startup_nvram_handle = nvram_open_staging(NVRAM_STAGING_TMP);
    if(!startup_nvram_handle)
    {
        DBG_PRINTF("startup_nvram_handle init failed!");
        return STARTUP_ERROR; 
    }

    return STARTUP_OK;
}



/*****************************************************************
    Function:func_host_info_show_startup
    Description: func to show startup info about host 
    Author:huangmingjian
    Date:2012/8/24
    Input:struct vty *vty   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/

static int func_host_info_show_startup(struct vty *vty)
{
	char *val = NULL;
    
    val = nvram_safe_get(startup_nvram_handle, "sys_systemname");
    if(val)
    {
        if(0 != strcmp(DEFAULT_VAL_HOSTNAME, val))
        {
            vty_out(vty, " hostname %s %s", val, VTY_NEWLINE);
            vty_out(vty, "!%s", VTY_NEWLINE);
        }
    }
    else
    {
        vty_out(vty, " ERROR: get hostname failed!", val, VTY_NEWLINE);
        return CMD_WARNING; 
    }


	val = NULL;
	val = nvram_safe_get(startup_nvram_handle, "super_password");
    if(val)
    {
        if(0 != strcmp(DEFAULT_VAL_SUPER, val))
        {	 	
			/*Begin add by huangmingjian 2012/09/25 for EPN104QID0046*/ 
			if(vty->node != VIEW_NODE)
			{	
				vty_out(vty, " super password %s %s", val, VTY_NEWLINE);
            	vty_out(vty, "!%s", VTY_NEWLINE);
			}
			/*End add by huangmingjian 2012/09/25 for EPN104QID0046 */
        }
    }
    else
    {
        vty_out(vty, " ERROR: get super password failed!", val, VTY_NEWLINE);
        return CMD_WARNING; 
    }
    return CMD_SUCCESS;
	#if 0 //add by zhouguanhua 2013/6/16
    host_init_t info;
	if (IPC_STATUS_OK != ipc_get_host_info(&info))
	{
	    if(0 != strcmp(DEFAULT_VAL_HOSTNAME, info.ht_name))
        {
            vty_out(vty, " hostname %s %s", val, VTY_NEWLINE);
            vty_out(vty, "!%s", VTY_NEWLINE);
        }
	}
    else
    {
        vty_out(vty, " ERROR: get hostname failed!", val, VTY_NEWLINE);
        return CMD_WARNING; 
    }
	return CMD_SUCCESS;
	#endif  //add by zhouguanhua 2013/6/16
}


/*****************************************************************
    Function:func_telnet_service_show_startup
    Description: func to show startup info about telnet service
    Author:liaohongjun
    Date:2012/8/20
    Input:struct vty *vty   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
static int func_telnet_service_show_startup(struct vty *vty)
{
    char *val = NULL;
    
    if(NULL == vty)
    {
        return CMD_WARNING;
    }    
    
    val = nvram_safe_get(startup_nvram_handle, "telnet_service_enabled");
    if(val)
    {
        if(strcmp(DEFAULT_TELNET_SERVICE, val))
        {
            vty_out(vty, " telnet service %s %s", val, VTY_NEWLINE);
            vty_out(vty, "!%s", VTY_NEWLINE);
        }
    }
    else
    {
        vty_out(vty, " ERROR: get telnet service info failed!", val, VTY_NEWLINE);
        return CMD_WARNING; 
    }
    
    return CMD_SUCCESS;
}
/*****************************************************************
    Function:func_manage_vlan_show_startup
    Description: func to show startup info about management vlan
    Author:liaohongjun
    Date:2012/11/2
    Input:struct vty *vty   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
static int func_manage_vlan_show_startup(struct vty *vty)
{
    sys_network_t network_info;
    
    if(NULL == vty)
    {
        return CMD_WARNING;
    }   
    
    memset(&network_info, 0x0, sizeof(sys_network_t));
    if(STARTUP_OK != startup_nvram_get_vlan_interface_info(&network_info))
    {
        DBG_PRINTF("get sys network error");
        return CMD_WARNING;
    }    
    
    if(DEFAULT_MANAGEMENT_VLAN != network_info.m_vlan)
    {
        vty_out(vty, " management-vlan %d %s", network_info.m_vlan, VTY_NEWLINE);
        vty_out(vty, "!%s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}
#endif  //add by zhouguanhua 2013/5/31

/*****************************************************************
    Function:func_loopback_show_startup
    Description: func to show startup info about loopback detection
    Author:huangmingjian
    Date:2012/11/28
    Input:struct vty *vty   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
static int func_loopback_show_startup(struct vty *vty)
{
#if 0 //add by zhouguanhua 2013/5/31
    sys_network_t network_info;
    
    if(NULL == vty)
    {
        return CMD_WARNING;
    }   
    
    memset(&network_info, 0x0, sizeof(sys_network_t));
    if(STARTUP_OK != startup_nvram_get_vlan_interface_info(&network_info))
    {
        DBG_PRINTF("get sys network error");
        return CMD_WARNING;
    }    
    
    if(DEFAULT_LOOPBACK_ENABLE != network_info.loopback_enable)
    {
        vty_out(vty, " loopback-detection enable %s", VTY_NEWLINE);
        vty_out(vty, "!%s", VTY_NEWLINE);
    }

#endif  //add by zhouguanhua 2013/5/31 
    return CMD_SUCCESS;
#if 0 //add by zhouguanhua 2013/5/31
    sys_loop_t loop_info;
    char val[5]={0};
    if(NULL == vty)
    {
        return CMD_WARNING;
    }   
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_NOSAVE_IFINDEX, CONFIG_LOOP_ENABLED, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get LOOP_ENABLED faild");
		return CMD_WARNING;
	}
	if (val[0])
	{
		loop_info.loopback_enable = strtoul(val, NULL, 0);
	}
	else
	{
	    return CMD_WARNING;  
	}
    if(DEFAULT_LOOPBACK_ENABLE != loop_info.loopback_enable)
    {
        vty_out(vty, " loopback-detection enable %s", VTY_NEWLINE);
        vty_out(vty, "!%s", VTY_NEWLINE);
    }
    return CMD_SUCCESS;
 #endif  //add by zhouguanhua 2013/5/31 
}



/*****************************************************************
    Function:func_enable_node_show_startup
    Description: 显示enable node 下所有保存的配置，各模块
                      需要自己实现接口供本接口调用
    Author:liaohongjun
    Date:2012/8/21
    Input:struct vty *vty   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS
=================================================
    Note:
*****************************************************************/
static int func_enable_node_show_startup(struct vty *vty)
{
    int iRet = CMD_WARNING;
    
    if(NULL == vty)
    {
        return CMD_WARNING;
    }
	
    /*startup config of host info,include hostname and super password */
    iRet = func_host_info_show_startup(vty);
	/*startup config of telnet service */
	iRet |= func_telnet_service_show_startup(vty);
    /*startup config of management vlan */
    iRet |= func_manage_vlan_show_startup(vty);
	/*startup config of loopback */
    iRet |= func_loopback_show_startup(vty); /*Add by huangmingjian 2012/11/28 for EPN104QID0083*/ 
    
    return iRet;

}
#if 0 //add by zhouguanhua 2013/5/31

/*****************************************************************
    Function:startup_nvram_handle_init
    Description: 获取保存的vty user 配置信息
    Author:liaohongjun
    Date:2012/8/21
    Input:VTY_USER_INFO_S *user_info   
    Output:         
    Return:
        STARTUP_ERROR_PARA
        STARTUP_OK
=================================================
    Note:
*****************************************************************/
static RET_STARTUP_E startup_nvram_get_user_info(VTY_USER_INFO_S *user_info)
{
    int vty_index = 0;
    char name[BUF_SIZE_64] = {0};
    
    if(NULL == user_info)
    {
        return STARTUP_ERROR_PARA;
    }
    
    for(vty_index = MIN_VTY_USER_NO; vty_index < MAX_VTY_COUNT; vty_index++)
    {    
        /*begin modified by liaohongjun 2012/9/3 of QID0015*/
        memset(name ,0x0, sizeof(name));
        sprintf(name ,PARTS_VTY_USERNAME_ITEM,vty_index);
        safe_strncpy(user_info[vty_index].username, nvram_safe_get(startup_nvram_handle, name), sizeof(user_info[vty_index].username));
        memset(name ,0x0, sizeof(name));
        sprintf(name ,PARTS_VTY_AUTHPASSWD_ITEM,vty_index);
        safe_strncpy(user_info[vty_index].passwd, nvram_safe_get(startup_nvram_handle, name), sizeof(user_info[vty_index].passwd));
        memset(name ,0x0, sizeof(name));
        sprintf(name ,PARTS_VTY_AUTHMODE_ITEM,vty_index);
        safe_strncpy(user_info[vty_index].authmode, nvram_safe_get(startup_nvram_handle, name), sizeof(user_info[vty_index].authmode)); 
        memset(name ,0x0, sizeof(name));
        sprintf(name ,PARTS_VTY_PASSWD_TYPE_ITEM,vty_index);
        safe_strncpy(user_info[vty_index].passwd_type, nvram_safe_get(startup_nvram_handle, name), sizeof(user_info[vty_index].passwd_type)); 
        memset(name ,0x0, sizeof(name));
        sprintf(name ,PARTS_VTY_TIMEOUT_ITEM,vty_index);
        user_info[vty_index].timeout = strtoul(nvram_safe_get(startup_nvram_handle, name), NULL, 10);
        /*end modified by liaohongjun 2012/9/3 of QID0015*/
    }

    return STARTUP_OK;
}
#endif //add by zhouguanhua 2013/5/31
/*****************************************************************
    Function:func_vty_show_startup
    Description: 显示保存的虚拟终端用户 配置信息
    Author:liaohongjun
    Date:2012/8/21
    Input:struct vty *vty   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS
=================================================
    Note:
*****************************************************************/
static int func_vty_show_startup(struct vty *vty)
{
    int vty_index = 0;
    RET_STARTUP_E ret = STARTUP_ERROR;
    unsigned long timeout_min = 0;
    unsigned long timeout_sec = 0;
    VTY_USER_INFO_S vty_user_info[MAX_VTY_COUNT] = {0};
    
    if(NULL == vty)
    {
        return CMD_WARNING;
    }

    memset(vty_user_info, 0x0, (MAX_VTY_COUNT*sizeof(VTY_USER_INFO_S)));
    ret = startup_nvram_get_user_info(vty_user_info);
    if(STARTUP_OK != ret)
    {
        vty_out(vty, " ERROR: get vty user info failed! %s",VTY_NEWLINE);
        return CMD_WARNING; 
    }

    for(vty_index = MIN_VTY_USER_NO; vty_index < MAX_VTY_COUNT; vty_index++)
    {
        if(VTY_USER_NO_OF_CONSOLE == vty_index)
        {
            
            vty_out(vty, "user-config vty %d for console user%s", vty_index, VTY_NEWLINE);

            if(0 != strcmp(vty_user_info[vty_index].authmode, DEFAULT_CONSOL_AUTH_MODE))
            {
                vty_out(vty, " enable auth-mode %s%s", vty_user_info[vty_index].authmode, VTY_NEWLINE);
            }
     
        }
        else
        {
            vty_out(vty, "user-config vty %d for telnet user%s", vty_index, VTY_NEWLINE);
            if(0 != strcmp(vty_user_info[vty_index].authmode, DEFAULT_TELNET_AUTH_MODE))
            {
                vty_out(vty, " enable auth-mode %s %s", vty_user_info[vty_index].authmode, VTY_NEWLINE);
            }
        }
        if(0 != strcmp(vty_user_info[vty_index].passwd_type, DEFAULT_PASSWD_TYPE) || \
            0 != strcmp(vty_user_info[vty_index].passwd, DEFAULT_VTY_PASSWD))
        {
            vty_out(vty, " password %s %s %s", vty_user_info[vty_index].passwd_type, \
                vty_user_info[vty_index].passwd, VTY_NEWLINE);
        }               
        if((unsigned long)strtoul(DEFAULT_VTY_TIMEOUT, NULL, 10) != vty_user_info[vty_index].timeout)
        {
            timeout_min = vty_user_info[vty_index].timeout/60;
            timeout_sec = vty_user_info[vty_index].timeout%60;
            if(timeout_sec)
            {
                vty_out(vty, " exec-timeout %ld %ld %s", timeout_min, timeout_sec, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, " exec-timeout %ld %s", timeout_min, VTY_NEWLINE);
            }
        }
        if(0 != strcmp(vty_user_info[vty_index].username, DEFAULT_USERNAME))
        {
            vty_out(vty, " username %s %s", vty_user_info[vty_index].username, VTY_NEWLINE);
        }        
    }
    
    vty_out(vty, "!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
}
/*begin added by liaohongjun 2012/9/18 of EPN104QID0019*/
/*****************************************************************
    Function:startup_nvram_get_vlan_interface_info
    Description: 获取保存的vlan interface 配置信息
    Author:liaohongjun
    Date:2012/9/18
    Input:sys_network_t *info   
    Output:         
    Return:
        STARTUP_ERROR_PARA
        STARTUP_ERROR
        STARTUP_OK
=================================================
    Note:
*****************************************************************/
static RET_STARTUP_E startup_nvram_get_vlan_interface_info(sys_network_t *info)
{   
    int items = 0; 
    int i = 0;    
    char *val = NULL;
    char *vars[10] = {0};
    
    if(NULL == info)
    {
        return STARTUP_ERROR_PARA;
    }
    #if 0 //add by zhouguanhua 2013/5/31
    safe_strncpy(info->hostname, nvram_safe_get(startup_nvram_handle, "sys_systemname"), sizeof(info->hostname));
    safe_strncpy(info->interface_state, nvram_safe_get(startup_nvram_handle, "vlan_interface_state"), sizeof(info->interface_state));
    val = nvram_safe_get(startup_nvram_handle, "vlan_interface_proto");
    if (val[0])
    {
        info->ip_proto= !strcmp(val, "static") ? IP_PROTO_STATIC : IP_PROTO_DYNAMIC;	
    }
    else
    {
        DBG_PRINTF("startup get interface proto error");	
        return STARTUP_ERROR;
    }
    
    val = nvram_safe_get(startup_nvram_handle, "vlan_interface_ip");
    if (!val[0] || !inet_aton(val, &info->netif.ip))
    {
        DBG_PRINTF("startup get ip error");	
        return STARTUP_ERROR;
    }
    
    val = nvram_safe_get(startup_nvram_handle, "vlan_manage_vid");
    if (!val[0])
    {
        DBG_PRINTF("startup get management vlan error");	
        return STARTUP_ERROR;
    }  
    info->m_vlan = strtoul(val, NULL, 0);

	/*Begin add by huangmingjian 2012/11/28 for EPN104QID0083*/ 
	val = nvram_safe_get(startup_nvram_handle, "loop_enabled");
    if (!val[0])
    {
        DBG_PRINTF("startup get loop enable error");	
        return STARTUP_ERROR;
    }  
    info->loopback_enable = strtoul(val, NULL, 0);
	/*End add by huangmingjian 2012/11/28 for EPN104QID0083 */

    val = nvram_safe_get(startup_nvram_handle, "vlan_interface");
    if (!val[0])
    {
        DBG_PRINTF("startup get vlan interface error");	
        return STARTUP_ERROR;
    }
    info->vlan_interface = strtoul(val, NULL, 0);
    
    val = nvram_safe_get(startup_nvram_handle, "vlan_interface_netmask");
    if (!val[0] || !inet_aton(val, &info->netif.subnet))
    {
        DBG_PRINTF("startup get subnet mask error");
        return STARTUP_ERROR;
    }
    
    val = nvram_safe_get(startup_nvram_handle, "vlan_interface_gateway");
    if (!val[0] || !inet_aton(val, &info->netif.gateway))
    {
        DBG_PRINTF("startup get gateway error");	
        return STARTUP_ERROR;
    }
    
    val = nvram_safe_get(startup_nvram_handle, "vlan_interface_dns");
    if (val[0])
    {
        items = split_string(val, ',', vars);
        items = (items > MAX_DNS_NUM) ? MAX_DNS_NUM : items;
        for (i = 0 ; i < items; i ++){
            inet_aton(vars[i], &info->netif.dns[i]);
        }        
    }
#endif  //add by zhouguanhua 2013/5/31

    return STARTUP_OK;
}

/*****************************************************************
    Function:func_vlan_interface_show_startup
    Description: 显示保存的虚接口 配置信息
    Author:liaohongjun
    Date:2012/9/18
    Input:struct vty *vty   
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS
=================================================
    Note:
*****************************************************************/
static int func_vlan_interface_show_startup(struct vty *vty)
{
    sys_network_t network_info;

    if(NULL == vty)
    {
        return CMD_WARNING;
    }
    
    memset(&network_info, 0x0, sizeof(sys_network_t));
    if(STARTUP_OK != startup_nvram_get_vlan_interface_info(&network_info))
    {
        DBG_PRINTF("get sys network error");
        return CMD_WARNING;
    }
#if 0 //add by zhouguanhua 2013/5/31

    vty_out(vty, "interface vlan-interface %d %s", network_info.vlan_interface, VTY_NEWLINE);
      
    if(!strcmp(network_info.interface_state, INTERFACE_STATE_DOWN))
    {
        vty_out(vty, " shutdown %s", VTY_NEWLINE);
    }
#endif  //add by zhouguanhua 2013/5/31     
    if(IP_PROTO_DYNAMIC == network_info.ip_proto)
    {
        vty_out(vty, " ip dynamic %s", VTY_NEWLINE);
    }
    else if(IP_PROTO_STATIC == network_info.ip_proto)
    {
        if((strcmp(DEFAULT_MANAGEIP, inet_ntoa(network_info.netif.ip)) || strcmp(DEFAULT_MANAGEMASK, inet_ntoa(network_info.netif.subnet))) && \
            (!strcmp(DEFAULT_MANAGEGW, inet_ntoa(network_info.netif.gateway)) || (0 == network_info.netif.gateway.s_addr)))
        {
            vty_out(vty, " ip address %s",inet_ntoa(network_info.netif.ip));            
            vty_out(vty, " %s %s", inet_ntoa(network_info.netif.subnet), VTY_NEWLINE);
        }
        else 
        {
            if((0 != network_info.netif.gateway.s_addr)&& (strcmp(DEFAULT_MANAGEGW, inet_ntoa(network_info.netif.gateway))))
            {
                vty_out(vty, " ip address %s",inet_ntoa(network_info.netif.ip));  
                vty_out(vty, " %s",inet_ntoa(network_info.netif.subnet));
                vty_out(vty, " %s %s",inet_ntoa(network_info.netif.gateway),VTY_NEWLINE);
            }
        }   
    }
    
    vty_out(vty, "!%s", VTY_NEWLINE);     
    return CMD_SUCCESS;
}
/*end added by liaohongjun 2012/9/18 of EPN104QID0019*/
#if 0
/*****************************************************************
    Function:show_startup_config
    Description: 显示系统当前保存的配置信息统一接口,
                    其余各模块需要自己实现获取保存的配置
                    信息接口,并由此接口统一调用
    Author:liaohongjun
    Date:2012/8/21
    Input:  
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS
=================================================
    Note:
*****************************************************************/
DEFUN (show_startup_config,
        show_startup_config_cmd,
        "show startup-config",
        SHOW_DESC_STR
        "Contentes of startup configuration\n")
{
    int iRet = CMD_WARNING;
#if 0 //add by zhouguanhua 2013/5/31    
    if(STARTUP_OK != startup_nvram_handle_init())
    {
        return CMD_WARNING;        
    }
    
    vty_out(vty, "!%s", VTY_NEWLINE);
    
    /*each function remember to output a ' !' before return*/
    
    /*startup config of enable node*/
    iRet = func_enable_node_show_startup(vty);
    /*startup config of vty node*/
    iRet = func_vty_show_startup(vty);
    /*startup config of vlan interface node*/
    iRet = func_vlan_interface_show_startup(vty);    
    vty_out(vty, "end%s", VTY_NEWLINE);
    nvram_close(startup_nvram_handle);
    unlink(NVRAM_STAGING_TMP);
   #endif  
    return iRet;
}

#endif
#ifdef  __cplusplus
}
#endif

