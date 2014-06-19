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
#include <systemlog.h>

#define NTP_NAME         "ntpclient" 
#define NTP_STARTUP_CMD   "ntpclient -s "
#if 0 //add by zhouguanhua 2013/6/5
static void kill_ntpd_if_exist(void)
{
    /*Begin modify by zhouguanhua 2012-08-18 for kill ntpclient */
	FILE *fd;
	pid_t pid = 0;
	char buffer[128];
	fd = fopen(SYS_NTPCLIENT_PID_FILE, "r");
	if (fd == NULL)
	{
	    DBG_PRINTF("ntpclient kill is fail\n");   
	}
	else
	{
		if(fgets(buffer, sizeof(buffer) - 1, fd)){
			pid = strtoul(buffer, NULL, 0);
		}
		fclose(fd);
	}
	if (pid > 0){
		kill(pid, SIGTERM);
		DBG_PRINTF("TODO:kill ntpclient\n");
	}
	/*End modify by zhouguanhua 2012-08-18 for kill ntpclient */
}
#endif  //add by zhouguanhua 2013/6/5

/*Begin modify by zhouguanhua 2012-9-1 chat ntpclient synchronization*/
int get_ntp_syn_state(void)
{
    FILE *fd;	
	int sysn = 0;
	char buffer[2]; 
	if (access(SYS_NTPCLIENT_OK_FILE, F_OK) != 0)
	{
	    return 0;    
	}
	fd = fopen(SYS_NTPCLIENT_OK_FILE, "r");
	if (fd == NULL)
	{
	    DBG_PRINTF("ntpclient fail to sysn\n");
    }
	else
	{
		if(fgets(buffer, 2, fd))
		{
			sysn = strtoul(buffer, NULL, 0);
        }
		fclose(fd);
	}
	if (sysn > 0){
		return 1;//add by zhouguanhua 2012/8/20
	}
	else
	{
	    return 0;   
	}
}
 /*end modify by zhouguanhua 2012-9-1 chat ntpclient synchronization*/

static int get_ntp_status(timer_element_t *timer, void *data)
{
    sys_time.ntp_ok=get_ntp_syn_state();
    return TIMER_RUN_ONCE;
}

 void ntpd_start(void)
{
	char exe_str[128];
    
	if(1==sys_time.ntp_en)
	{
      //  USER_LOG("NTP",LOG_INFO,"ntpstart.");
        USER_LOG(LangId,NTP,LOG_INFO,M_NTP_STA,"",""); //add by leijinbao 2013/9/17
    	sprintf(exe_str, "%s -h %s &",NTP_STARTUP_CMD,sys_time.server);
    	startup_process(exe_str);    
        //sleep(1);      
        timer_register(500, 0, (timer_func_t)get_ntp_status, NULL, NULL, "ntpstart");      
    }
}

void sys_time_load(sys_time_t *sys)
{
	int invalid = 0;
	char val[BUF_SIZE256] = {0};

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_NTP_SERVER, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get NTP server faild");
	}
	if (val[0]){
		strncpy(sys->server, val, sizeof(sys->server));
	}else {
		invalid ++;
	}
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_NTP_ZONE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get NTP zone faild");
	}
	if (val[0]){
		sys->zone = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_NTP_TYPE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get NTP type faild");
	}
	if (val[0]){
		sys->ntp_en = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}	
	
	if (invalid > 0){
		DBG_ASSERT(0, "Invalid SYS TIME INFO");
	}
}



int sys_time_update(sys_time_t *cfg)
{
	sys_time_t *sys = &sys_time;
	int update = 0;

	if (sys->zone != cfg->zone){
		sys->zone = cfg->zone;
		update ++;
		set_update(time);
	}
	
	if (sys->ntp_en != cfg->ntp_en){
		sys->ntp_en = cfg->ntp_en;
		update ++;
		set_update(time);		
	}

	if (sys->ntp_en && strcmp(sys->server, cfg->server)){
		safe_strncpy(sys->server, cfg->server, sizeof(sys->server));
		update ++;
		set_update(time);
	}else {
		update ++; // time changed
		sys->time = utc_time(cfg->time, sys->zone);
	}
	return update;
}


int sys_time_commit(int apply_options)
{
	char val[16];
	if (test_update(time)){
		
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_NTP_SERVER, sys_time.server))
		{
			return -1;	
		}
		//nvram_set(nvram_handle, "ntp_time_server", sys_time.server);
		sprintf(val, "%u", sys_time.zone);
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_NTP_ZONE, val))
		{
			return -1;	
		}
		//nvram_set(nvram_handle, "ntp_time_zone", val);
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_NTP_TYPE, sys_time.ntp_en ? "1" : "0"))
		{
			return -1;	
		}
		//nvram_set(nvram_handle, "ntp_type", sys_time.ntp_en ? "1" : "0");	
		clr_update(time);
	}
//	nvram_commit(nvram_handle);
//	nvram_update();	

	return 1;
}


int sys_time_apply(int apply_options)
{
	char cmd[128], tstr[64];
	struct tm tm;

	//kill_ntpd_if_exist();

    if(set_process_status(NTP_NAME, NTP_STARTUP_CMD, PROCESS_DISABLE) < 0)//shut down process if it lives
    {
        DBG_PRINTF(" ERROR: do syslogd process failed!");
        return -1; 
    } 
	if (sys_time.ntp_en)
	{
		//INFO_LOG("System Time: Enabled NTP @ %s", sys_time.server);//add by zhouguanhua 2012/9/25	
		ntpd_start();
	}
	else 
	{	
		/*Begin add by huangmingjian 2013/9/06 Bug 517*/
		/*ntp is down, so clear up ntpclient.txt flag*/
		sprintf(cmd, "echo 0 > %s", SYS_NTPCLIENT_OK_FILE);
		system(cmd);
		/*End add by huangmingjian 2013/9/06 Bug 517*/
		
		sys_time.time_before_change = time(NULL);/*Add by huangmingjian 2013/9/06 for logout when setting time on web*/
		sys_time.ntp_ok = 0;//add by zhouguanhua 2012/8/20
		gmtime_r(&sys_time.time, &tm);
		sprintf(tstr, "%04u.%02u.%02u-%02u:%02u:%02u", 
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec);
		//INFO_LOG("System Time: %s", tstr);//add by zhouguanhua 2012/9/25	
		sprintf(cmd, "date -s %s", tstr);
		system(cmd);
		//system("hwclock -w");
		// need to reset all timers
		timer_reschedule_all();			
	}
	return 1;
}
