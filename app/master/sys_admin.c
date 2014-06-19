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


void sys_admin_load(sys_admin_t *sys)
{
	int invalid = 0;
	char val[BUF_SIZE256] = {0};

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_USER_WEB_NAME+1, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get user web name faild");
	}
	if (val[0]){
		safe_strncpy(sys->users[0].name, val, sizeof(sys->users[0].name));
	}else {
		invalid ++;
	}

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_USER_WEB_PASSWORD+1, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get user web password faild");
	}
	if (val[0]){
		safe_strncpy(sys->users[0].pwd, val, sizeof(sys->users[0].pwd));
	}else {
		invalid ++;
	}

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WEB_USERNAME, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get web username faild");
	}
	if (val[0]){
		safe_strncpy(sys->users[1].name, val, sizeof(sys->users[1].name));
	}else {
		invalid ++;
	}

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WEB_PASSWORD, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get web password faild");
	}
	if (val[0]){
		safe_strncpy(sys->users[1].pwd, val, sizeof(sys->users[1].pwd));
	}else {
		invalid ++;
	}

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WEB_AUX_ENABLE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get web aux enable faild");
	}
	/*Begin modify by jiangzhenhua  2012/7/12*/
	if (!strcmp(val, "enable")){
		sys->users[1].enable = 1;
	}else {
		sys->users[1].enable = 0;
		invalid ++;
	}
	/*End modify by jiangzhenhua  2012/7/12*/

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WEB_IDLETIME, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get web idletime faild");
	}
	if (val[0]){
		sys->idle_time = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}	

	sys->users[0].enable = 1;
	sys->count = 2;

	if (invalid > 0){
		DBG_ASSERT(0, "Invalid SYS ADMIN INFO");
	}

}


int sys_admin_update(sys_admin_t *cfg)
{
	int update = 0, i, j;
	if (cfg->idle_time != sys_admin.idle_time){
		sys_admin.idle_time = cfg->idle_time;
		update ++;
	}

	for(i = 0; i < cfg->count; i ++){
		for (j = 0; j < sys_admin.count; j ++){
			if (!strcmp(cfg->users[i].name, sys_admin.users[j].name)){
				/*Begin modify by guguiyuan 2012/7/18*/
				//if (sys_admin.users[j].enable != cfg->users[i].enable){
				//	sys_admin.users[j].enable = cfg->users[i].enable;
				//	update ++;
				//} 
				//if (sys_admin.users[j].enable){
					safe_strncpy(sys_admin.users[j].name, cfg->users[i].name, sizeof(sys_admin.users[j].name));
					safe_strncpy(sys_admin.users[j].pwd, cfg->users[i].pwd, sizeof(sys_admin.users[j].pwd));					
					update ++;
				//}
				/*End modify by guguiyuan 2012/7/18*/
			}
		}
	}
	
	return update;
}


int sys_admin_apply(int apply_options)
{

	return 0;
}

int sys_admin_commit(int apply_options)
{	
	char val[16];
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_USER_WEB_NAME+1,sys_admin.users[0].name))
	{
		return -1;	
	}
	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_USER_WEB_PASSWORD+1,sys_admin.users[0].pwd))
	{
		return -1;	
	}

	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WEB_USERNAME,sys_admin.users[1].name))
	{
		return -1;	
	}

	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WEB_PASSWORD,sys_admin.users[1].pwd))
	{
		return -1;	
	}
	//nvram_set(nvram_handle, "user_web_name", sys_admin.users[0].name);
	//nvram_set(nvram_handle, "user_web_password", sys_admin.users[0].pwd);	
	//nvram_set(nvram_handle, "web_username_aux", sys_admin.users[1].name);
	//nvram_set(nvram_handle, "web_password_aux", sys_admin.users[1].pwd);
	//nvram_set(nvram_handle, "web_aux_enabled", sys_admin.users[1].enable ? "1" : "0");
	sprintf(val, "%u", sys_admin.idle_time);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WEB_IDLETIME,val))
	{
		return -1;	
	}
	//nvram_set(nvram_handle, "web_idle_time", val);
//	nvram_commit(nvram_handle);
//	nvram_update();	

	return 1;
}
