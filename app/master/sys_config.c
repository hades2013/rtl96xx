

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
#include "sys_vlan.h"
#include <ipc_protocol.h> 

//#include <vos_config.h>
//#include <cfg_utils.h>


#define UPDATE_ALL 1
#define CLEAR_ALL 0

sys_config_t sys_config;
pid_t pid_vtysh = 0;
pid_t pid_httpd = 0;
pid_t pid_udhcpc = 0;
pid_t pid_ntpd = 0;
pid_t pid_snmpd = 0;
extern void master_cfg_do_commit(void);
extern int cfg_restore_for_file(char *file);
extern int CfgRestoreFactory(char *file);




#if 0
void sys_info_mac_reload(void)
{
	int ret,i, invalid = 0;
	char val[64];
	char nvname[CFG_NAME_SIZE];
	sys_info_t *info = &sys_info;
	
	unsigned int oid;

	for (i = 0; i < MAX_CLT_CHANNEL; i ++){
		sprintf(nvname, "port_mac_addr_cab%d", i);
		//val = nvram_safe_get(nvram_handle, nvname);
		if(0 != vosConfigName2Oid(nvname,&oid))
		{
			break;	
		}
		if(0 != cfg_getval(0,oid,val,"default",sizeof(val)))
		{
			break;
		}
		if (val[0]){
			ret = hexencode(info->cabmac[i].octet, sizeof(ethernet_addr_t), val);
			if (ret != sizeof(ethernet_addr_t)) {
				invalid ++;
				DBG_ASSERT(1, "Invalid MAC address");
				continue;
			}
		}
	}
	
}
#endif


static void sys_security_load(sys_security_t *sys)
{
	int invalid = 0;
	char val[64];
	
	//val = nvram_safe_get(nvram_handle, "web_ssl_enabled");
	memset(val, 0, sizeof(val));
	if(0 != cfg_getval(IF_ROOT_IFINDEX,CONFIG_WEB_SSL_ENABLE,val,"enable",sizeof(val)))
	{
		DBG_ASSERT(0, "Security: get ssl enable fail !");		
	}
	if (val[0]){
		sys->https_en = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
	
	memset(val, 0, sizeof(val));
    #ifdef CONFIG_SSHD
	if(0 != cfg_getval(IF_ROOT_IFINDEX,CONFIG_SSH_SERVICE_ENABLE,val,"enable",sizeof(val)))
	{
		DBG_ASSERT(0, "Security: get ssh enable fail !");
	}
	//val = nvram_safe_get(nvram_handle, "sys_ssh_enabled");
	if (val[0]){
		sys->ssh_en = (0==strcmp(ENABLE_STR, val)?1:0); 
	}else {
		invalid ++;
	}

	memset(val, 0, sizeof(val));
	if(0 != cfg_getval(IF_ROOT_IFINDEX,CONFIG_SSH_PORT,val,"22",sizeof(val)))
	{
		DBG_ASSERT(0, "Security: get ssh port fail !");
	}
	//val = nvram_safe_get(nvram_handle, "sys_ssh_port");
	if (val[0]){
		sys->ssh_port = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
    #endif
	memset(val, 0, sizeof(val));
	if(0 != cfg_getval(IF_ROOT_IFINDEX,CONFIG_TELNET_SERVICE_ENABLE,val,"enable",sizeof(val)))
	{
		DBG_ASSERT(0, "Security: get telnet enable fail !");
	}
	//val = nvram_safe_get(nvram_handle, "sys_telnet_enabled");
	if (val[0]){
		sys->telnet_en = (strcmp(val, ENABLE_STR)==0?1:0);
	}else {
		invalid ++;
	}

	memset(val, 0, sizeof(val));
	if(0 != cfg_getval(IF_ROOT_IFINDEX,CONFIG_TELNET_PORT,val,"23",sizeof(val)))
	{
		DBG_ASSERT(0, "Security: get telnet port fail !");
	}

	//val = nvram_safe_get(nvram_handle, "sys_telnet_port");
	if (val[0]){
		sys->telnet_port = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}

	memset(val, 0, sizeof(val));
	if(0 != cfg_getval(IF_ROOT_IFINDEX, CONFIG_MVLAN_VID, (void *)val, "1", sizeof(val)))
	{
		DBG_ASSERT(0, "Security: get mvlan fail !");
	}

	//val = nvram_safe_get(nvram_handle, "vlan_man_vid");
	if (val[0]){
		sys->mvlan = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
	
	if (invalid > 0){
		DBG_ASSERT(0, "Invalid SYS SECURITY INFO");
	}

}



int sys_security_update(sys_security_t *cfg, int update)
{	
	if (!update){
		update = SYS_HTTPS | SYS_SSH | SYS_TELNET | SYS_MVLAN;
	}
	if (update & SYS_HTTPS){
		if (cfg->https_en != sys_security.https_en){
			sys_security.https_en = cfg->https_en;
			set_update(https);
		}else {
			update &= ~(SYS_HTTPS);
		}
	}

	if (update & SYS_SSH){
		if ((cfg->ssh_en != sys_security.ssh_en) 
			|| (cfg->ssh_port != sys_security.ssh_port)){
			sys_security.ssh_en = cfg->ssh_en;
			sys_security.ssh_port = cfg->ssh_port;
			set_update(ssh);
		}else {	
			update &= ~(SYS_SSH);
		}
	}

	if (update & SYS_TELNET){
		if ((cfg->telnet_en != sys_security.telnet_en) 
			|| (cfg->telnet_port != sys_security.telnet_port)){
			sys_security.telnet_en = cfg->telnet_en;
			sys_security.telnet_port = cfg->telnet_port;	
			set_update(telnet);
		}else {	
			update &= ~(SYS_TELNET);
		}
	}

	if (update & SYS_MVLAN){
		if (cfg->mvlan != sys_security.mvlan){
			sys_security.mvlan = cfg->mvlan;
			sys_info.mvlan = cfg->mvlan;
			set_update(mvlan);
		}else{		
			update &= ~(SYS_MVLAN);
		}
	}

	return update;
}


int sys_security_commit(int apply_options, int update)
{
	char val[BUF_SIZE128];
	
	//unsigned int oid;
	
	if (update & SYS_HTTPS){

		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WEB_SSL_ENABLE,sys_security.https_en ? ENABLE_STR : DISABLE_STR))
		{
			return -1;	
		}
		//nvram_set(nvram_handle, "web_ssl_enabled", sys_security.https_en ? "1" : "0");		
	}
    #ifdef CONFIG_SSHD
	if (update & SYS_SSH){
		
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SSH_SERVICE_ENABLE, sys_security.ssh_en ? ENABLE_STR : DISABLE_STR))
		{
			return -1;	
		}		
		//nvram_set(nvram_handle, "sys_ssh_enabled", sys_security.ssh_en ? "1" : "0");
		sprintf(val, "%u", sys_security.ssh_port);
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SSH_PORT, val))
		{
			return -1;	
		}	
		//nvram_set(nvram_handle, "sys_ssh_port", val);
	}
    #endif
	if (update & SYS_TELNET){
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_TELNET_SERVICE_ENABLE, sys_security.telnet_en ? ENABLE_STR: DISABLE_STR))
		{
			return -1;	
		}	
		//nvram_set(nvram_handle, "sys_telnet_enabled", sys_security.telnet_en ? "1" : "0");
		sprintf(val, "%u", sys_security.telnet_port);
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_TELNET_PORT, val))
		{
			return -1;	
		}
		//nvram_set(nvram_handle, "sys_telnet_port", val);
	}

	if (update & SYS_MVLAN){
        #if 0
		if(0 != vosConfigName2Oid( "vlan_man_vid",&oid))
		{
			return -1;	
		}
		sprintf(val, "%u", sys_security.mvlan);
		if(0 !=master_cfg_setval(0, CONFIG_TELNET_PORT, val))
		{
			return -1;	
		}
		//nvram_set(nvram_handle, "vlan_man_vid", val);
        #endif
	}
	
	return update;
}


/*****************************************************************
    Function:shutdown_process
    Description:shutdown the process
    Author:huangmingjian
    Date: 2013-09-15
    Input:    sys_security_t *sys
    Output:         
    Return:
        error -1
        ok     0   
=================================================
    Note:
*****************************************************************/ 
int sshd_apply(sys_security_t *sys)
{	
	char ssh_cmd[64] = {0};
	pid_t pid = 0;
	char cmd[64] = {0};
	int ret = 0;
	
	sprintf(ssh_cmd, "%s -v -p %u", DROPBEAR_FULL_PATH, sys->ssh_port);
	pid = get_pid_by_name(DROPBEAR_NAME);
	if(pid>0)
	{	
		sprintf(cmd, "kill -9 %u", pid);
		ret = system(cmd);
		if(-1 == ret || 127 == ret)
		{	
			DBG_PRINTF("kill process fail!\n");
			return -1;
		}
	}
	
	if(set_process_status(DROPBEAR_NAME, ssh_cmd, sys->ssh_en) < 0)
  	{
   		DBG_PRINTF(" ERROR: set ssh status failed!");
   		return -1; 
  	}
	
	return 0;
}

extern pid_t get_pid_by_name(char *pname);

int sys_security_apply(int apply_options)
{
	pid_t pid;
	
	if (test_update(https)){
		//todo apply https
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WEB_SSL_ENABLE,sys_security.https_en ? ENABLE_STR : DISABLE_STR))
		{
			return -1;	
		}
		pid=get_pid_by_name("httpd");
		if(pid!=0)
		{
			 kill(pid, SIGUSR1);
		}
		clr_update(https);
	}
	/*Begin modified by huangmingjian 2013-09-15 for ssh enable on web*/
	if (test_update(ssh))
	{	
		clr_update(ssh);
		if(sshd_apply(&sys_security)<0)
		{
			DBG_PRINTF(" ERROR: sshd_apply failed!");
			return -1;
		}
	}
	/*End modified by huangmingjian 2013-09-15 for ssh enable on web*/

	if (test_update(telnet)){
		// to do apply telnet
		clr_update(telnet);
	}
    #if 0
	if (test_update(mvlan)){
		// Apply all vlan, mvlan change may casue pvid changes in access port
		switch_vlan_config_apply(MVLAN_CHANGED);
		switch_vlan_apply_all();
		clr_update(mvlan);
	}
	#endif
	return 1;
}


/*****************************************************************
  Function:        sys_create_vlanif
  Description:    create the vlan interface
  Author: 	         feihuaxin
  Date:   		   2012/7/16
  INPUT:           void
  OUTPUT:         void
  Return:   	   
  Others:	       
*****************************************************************/
void sys_create_vlanif(void)
{
   // eval("vconfig", "add", "eth0", SYS_VLANIF_NUM);
    return;
}

void httpd_start(void)
{
	char *argv[] = {"/usr/sbin/httpd", NULL};
	_eval(argv, NULL, 0, &pid_httpd);
}

void vtysh_start(void)
{/*
	pid_t pid;
	int sig, status;
	char *argv[] = {"/usr/sbin/vtysh", NULL};
	_eval(argv, NULL, 0, &pid_vtysh);
	*/
    system("/usr/sbin/vtysh &");
}




uint16_t sys_management_vlan(void)
{
	return sys_info.mvlan;
}

/*
	When mvlan was deleted by vlan removing, mvlan should be set a new one.
*/
int sys_management_vlan_update(uint16_t newvlan)
{
	if (newvlan != sys_security.mvlan){
		sys_security.mvlan = newvlan;
		sys_info.mvlan = newvlan;		
		return sys_security_commit(0, SYS_MVLAN);
	}	
	return 0;
}

void sys_mac_update(ethernet_addr_t *newmac)
{
	memcpy(sys_config.info.sysmac.octet, newmac->octet, sizeof(ethernet_addr_t));
//	memcpy(sys_config.network[0].mac.octet, newmac->octet, sizeof(ethernet_addr_t));	
	//nvram_set(nvram_handle, "lan_mac_addr", mac2str(newmac));
	master_cfg_setval(IF_TEMP_IFINDEX, CONFIG_LAN_MAC_ADDR, mac2str(newmac));
	//nvram_do_commit();
	//master_cfg_do_commit();
}

int sys_mac_apply(ethernet_addr_t *current_mac)
{
	//char val[BUF_SIZE_64] = {0};
//	int nv_invalid = 0;
	ethernet_addr_t  sys_addr;
	
	
	if (get_if_mac(SYS_NETIF_NAME, &sys_addr) < 0){
		return -1;
	}
	
	sys_mac_update(&sys_addr);
	
	if (current_mac){
		return get_if_mac(SYS_NETIF_NAME, current_mac);
	}
	return 0;
}
/*****************************************************************
    Function:shutdown_process
    Description:shutdown the process
    Author:huangmingjian
    Date:2012/11/27
    Input:     pid:  path: the file which the pid is inputted in.when shutdown ,we should delete it
    Output:         
    Return:
        error -1
        ok     0   
=================================================
    Note:
*****************************************************************/   

int shutdown_process(pid_t pid)
{	
	if(pid > 0)
	{	
		if(-1 == kill(pid, SIGKILL))
		{	
			DBG_PRINTF("kill pid:%d error\n", pid);
			return -1;
		}
		return 0;
	}
	return -1;
}

/*****************************************************************
    Function:startup_process
    Description:startup  the process
    Author:huangmingjian
    Date:2012/11/27
    Input:     cmd :the cmd which we make the process startup
    Output:         
    Return:
        error -1
        ok     0   
=================================================
    Note:
*****************************************************************/  
int startup_process(char *cmd)
{	
	int ret = 0;
	
	if(NULL == cmd)
	{
		return -1;
	}
	ret = system(cmd);
	if(-1 == ret || 127 == ret)
	{	
		DBG_PRINTF("startup process error!\n");
		return -1;
	}
	return 0;
}

/*****************************************************************
    Function:set_loopback_status
    Description:func to set loopback status:open or close
    Author:huangmingjian
    Date:2012/11/27
    Input:     
    Output:         
    Return:
        -1  error
        0   	ok   
        1    the process has been shutdown(or statup), we don't need to shutdown(or statup) it  again.
=================================================
    Note:
*****************************************************************/   
int  set_process_status(char *process_name , char *startup_cmd, int enable)
{
	pid_t pid = 0;
	
    if((enable < PROCESS_DISABLE) || (enable > PROCESS_ENABLE))
    {	
    	DBG_PRINTF("input status error\n");
        return -1;
    }
	if((NULL == process_name) || (NULL == startup_cmd))
	{
		DBG_PRINTF("input process name or startup cmd is NULL\n");
        return -1;
	}
	
	pid = get_pid_by_name(process_name);
    if(PROCESS_ENABLE == enable) 	         
    {	
    	if(-1 == pid )        
		{
			if(0 == startup_process(startup_cmd))
			{	
				return 0;
			}
		}
		else if(pid > 0)
		{
			return 1;
		}		
    }
	else
	{	
		if(pid > 0)  
		{
			if(0 == shutdown_process(pid))
			{
				return 0;
			}	
		}
		else if(-1 == pid )
		{
			return 1;
		}
		
	}
	return -1;
}
void epon_start(void)
{	
	if(!(get_pid_by_name("eponoamd") > 0))
	{
		system("eponoamd&");
	}
}

extern int switch_config_init(void);

void sys_config_init(void)
{
	memset(&sys_config, 0, sizeof(sys_config));
	sys_mvlan_load(&sys_mvlan);
	sys_info_load(&sys_info);
	sys_networking_load(&sys_network);
#ifdef CONFIG_LOOPD	
    sys_loop_load(&sys_loop);
#endif

	sys_snmp_load(&sys_snmp);
	sys_security_load(&sys_security);
	sys_time_load(&sys_time);
	sys_admin_load(&sys_admin);
    sys_syslog_load(&sys_syslog);
    sys_qos_load(&sys_qos);
    sys_port_load(&sys_port);
	sys_linerate_load(&sys_linerate);
	sys_epon_load(&sys_epon);
    sys_mirror_load(&sys_mirror);
    sys_stormctl_load(&sys_storm);
	sys_mac_setting_load(&sys_mac_setting);
#if defined(CONFIG_ONU_COMPATIBLE)
    sys_compatible_load(&sys_compatible);
#endif
	Syslog_Init();
#ifdef CONFIG_CATVCOM
	sys_catv_load(&sys_catv); 
	catv_init();
#endif
#ifdef CONFIG_WLAN_AP	
	sys_wlan_ap_load(&sys_wlan_ap);
#endif
#if defined(CONFIG_ZBL_SNMP)
	sys_analogPropertyTable_load(&sys_AnalogProperty);
	sys_commonAdminGroup_load(&sys_commonAdminGroup);
#endif
	sys_upgrade_state(1);
	set_update_all();
	clr_update(interface_state);
}

void sys_config_apply(void)
{  
	//sys_networking_apply(0);  //ifm event will trigger it
	sys_mvlan_apply(IPC_APPLY_NONE);//add by wanghuanyu for 216
	httpd_start();
	//vtysh_start();
#ifdef CONFIG_LOOPD	
	loopd_init();
#endif
	syslog_start();
	snmpd_init();	
	ntpd_start();//modified by zhougunhua 2012/9/24 of QID0042
	epon_start();
#ifdef CONFIG_PRODUCT_5500
    sys_port_apply();
	sys_qos_apply();
    sys_stormctl_apply();
    sys_linerate_apply();
    sys_mirror_apply();
#endif
#ifdef CONFIG_SSHD
	extern int sshd_start(void);
	sshd_start();
	/*Begin modified by huangmingjian 2013-09-15 for ssh enable on web*/	
	if(sshd_apply(&sys_security)<0)
	{
		DBG_PRINTF("ERROR: sshd apply failed!");
	}
	/*End modified by huangmingjian 2013-09-15 for ssh enable on web*/
#endif
#ifdef CONFIG_CATVCOM	
	catv_init_apply(&sys_catv); 
#endif
#ifdef CONFIG_WLAN_AP	
	wlan_ap_init_apply(&sys_wlan_ap);
#endif

	clr_update_all();
}

/*begin modified by liaohongjun 2012/7/30 of QID0002*/
void sys_reboot(void)
{
	pid_t i;
	char *argv[] = {"reboot", NULL};
    
    if(DRV_WTD_REBOOT_OK != Drv_Watchdog_Reboot())
    {    
    	_eval(argv, NULL, 0, &i);
    }

    return;
}

static int sys_do_reboot(timer_element_t *timer, void *data)
{
	FILE *fp;
	pid_t i;
	char *argv[] = {"/tmp/busybox", "reboot", NULL};
    
    if(DRV_WTD_REBOOT_OK != Drv_Watchdog_Reboot())
    {
        fp = fopen("/tmp/busybox", "r");
        if (fp){
            fclose(fp);
            _eval(argv, NULL, 0, &i);
            return 0;
        }
        sys_reboot();
    }
    
	return 0;
}
/*end modified by liaohongjun 2012/7/30 of QID0002*/

void sys_reboot_later(int sec)
{
	timer_register(sec * 1000, 0, (timer_func_t)sys_do_reboot, NULL, NULL, "REBOOT");	
}


static int timer_handle_sys_do(timer_element_t *timer, void *data)
{
	int act = (int)data;
	switch(act)
	{
		case SYS_DO_REBOOT:
			sys_reboot();
			break;
		case SYS_DO_RESTORE:
            /*begin modified by liaohongjun 2012/6/29*/
			//nvram_restore_default();
			//nvram_restore_for_file(DEFAULT_CFG_FILE);
			cfg_restore_for_file(CONFIG_DEFAULTFILE);
			break;
		case SYS_DO_RESTORE_AND_REBOOT:
			//nvram_restore_default();
			//nvram_restore_for_file(DEFAULT_CFG_FILE);
			CfgRestoreFactory(CONFIG_DEFAULTFILE);
            /*end modified by liaohongjun 2012/6/29*/
			sys_reboot();
			break;
		case SYS_DO_NVRAM_COMMIT:
			//nvram_do_commit();
			master_cfg_do_commit();
			break;
	}
	return TIMER_RUN_ONCE;
}


void sys_dealy_do(int action)
{
	timer_register(200, 0, (timer_func_t)timer_handle_sys_do, (void*)action, NULL, "SYSACTION"); 
}



extern int lwConfigDomainDelete(CFG_DOMAIN_E enDomain);
extern int lwConfigParserFile(char * pPath,CFG_DOMAIN_E enDomain);
extern void cfg_setupif(CFG_DOMAIN_E domain);

int  show_startupstart(VOID)
{
	lwConfigDomainDelete(CONFIG_VIRTUAL);
	if(lwConfigParserFile("/mnt/startup", CONFIG_VIRTUAL)!=0)
	{
		return 1;
	}	
	
	cfg_setupif(CONFIG_VIRTUAL);
	return 0;
}

int  show_startupend(VOID)
{
//	INT32 eSubType = 0;
    ifindex_t ifIdx = 0;
	int ret=0;
	
	lwConfigDomainDelete(CONFIG_VIRTUAL);
	
  		/*¨¦?3y¨º¡À¡ê?¡Á¡é¨°a??¡Á¨®¨°¨¤¨¤¦Ì*/
	   if(IF_OK == IF_GetFirstIfindexByDomain((IF_SUB_TYPE_E)IF_SUB_L3VLAN, CONFIG_VIRTUAL, &ifIdx))
	   {
		   
		   do{		
					ret=IF_DeleteInterface(ifIdx);					
		   }while((IF_OK == IF_GetFirstIfindexByDomain((IF_SUB_TYPE_E)IF_SUB_L3VLAN, CONFIG_VIRTUAL, &ifIdx))&&(ifIdx!=IF_INVALID_IFINDEX));
		   
	   }
	   if(IF_OK == IF_GetFirstIfindexByDomain((IF_SUB_TYPE_E)IF_SUB_ETH, CONFIG_VIRTUAL, &ifIdx))
	   {
		   
		   do{		
					ret=IF_DeleteInterface(ifIdx);					
		   }while((IF_OK == IF_GetFirstIfindexByDomain((IF_SUB_TYPE_E)IF_SUB_ETH, CONFIG_VIRTUAL, &ifIdx))&&(ifIdx!=IF_INVALID_IFINDEX));
		   
	   }

	   if(IF_OK == IF_GetFirstIfindexByDomain((IF_SUB_TYPE_E)IF_SUB_ETHPORT, CONFIG_VIRTUAL, &ifIdx))
	   {
		   
		   do{		
					ret=IF_DeleteInterface(ifIdx);					
		   }while((IF_OK == IF_GetFirstIfindexByDomain((IF_SUB_TYPE_E)IF_SUB_ETHPORT, CONFIG_VIRTUAL, &ifIdx))&&(ifIdx!=IF_INVALID_IFINDEX));
		   
	   }

	   if(IF_OK == IF_GetFirstIfindexByDomain((IF_SUB_TYPE_E)IF_SUB_ROOT, CONFIG_VIRTUAL, &ifIdx))
	   {
		   
		   do{		
					ret=IF_DeleteInterface(ifIdx);					
		   }while((IF_OK == IF_GetFirstIfindexByDomain((IF_SUB_TYPE_E)IF_SUB_ROOT, CONFIG_VIRTUAL, &ifIdx))&&(ifIdx!=IF_INVALID_IFINDEX));
		   
	   }
		return 0;
}

