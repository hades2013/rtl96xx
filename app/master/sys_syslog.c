
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

/*begin by leijinbao 2013/09/05*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
/*end by leijinbao 2013/09/05*/ 

#define SYS_SYSLOG_PID_FILE "/var/run/syslogd.pid"
#define	SYSLOG_FILE_PATH		"/var/log/messages"
#define SYSLOG_NAME        "syslogd" 
#define SYSLOG_STARTUP_CMD   "syslogd -n -L -s 200" //-s 日志容量200K，
/*begin by leijinbao 2013/09/05*/
#define SYSLOG_CONFIG_PATH  "/etc/syslog.conf"
#define SYSLOG_CONSOLE_PATH "/dev/console"
/*****************************************************************
  Function:        sys_syslog_load
  Description:     sys_syslog_load
  Author: 	         zhouguanhua
  Date:   		   2013/4/7
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

void sys_syslog_load(sys_syslog_t *sys)
{
	int invalid = 0;
	char val[BUF_SIZE_256] = {0};

	/*syslog_en*/
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_ENABLED, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get SYSLOG_ENABLED faild");
	}
	if (val[0]){
		sys->syslog_en = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
    	
    /*syslog_ip*/
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_IP, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get SYSLOG_IP faild");
	}
	if (val[0]){
		strncpy(sys->syslog_ip, val, sizeof(sys->syslog_ip));
	}else {
		invalid ++;
	}	
 	
	/*syslog_degree*/
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_DEGREE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get SYSLOG_DEGREE faild");
	}
	if (val[0]){
		sys->syslog_degree = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
	
	
	/*syslog_save_degree*/
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_SAVE_DEGREE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get SYSLOG_SAVE_DEGREE faild");
	}
	if (val[0]){
		sys->syslog_save_degree  = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
   /*syslog_port add by leijinbao 2013/8/21*/
//#ifdef CONFIG_PRODUCT_5500
   	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_PORT, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get SYSLOG_PORT faild");
	}
	if (val[0]){
		sys->syslog_port= strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
#if 0

	
	/*syslog_console_en*/
	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_CONSOLE_ENABLED, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get SYSLOG_CONSOLE_ENABLED faild");
	}
	if (val[0]){
		sys->syslog_console_en = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
#endif	
	/*end by leijinbao 2013/8/21*/
	if (invalid > 0){
		DBG_ASSERT(0, "Invalid SYS TIME INFO");
	}    
}

/*****************************************************************
  Function:        sys_log_update
  Description:     sys_log_update
  Author: 	         zhouguanhua
  Date:   		   2013/4/7
  INPUT:           cfg
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

int sys_syslog_update(sys_syslog_t *cfg)
{
  
	sys_syslog_t *sys = &sys_syslog;
	int update = 0;

	if (sys->syslog_en!= cfg->syslog_en){
		sys->syslog_en = cfg->syslog_en;
		update ++;
		set_update(syslog);
	}
	
	if (sys->syslog_degree != cfg->syslog_degree){
		sys->syslog_degree = cfg->syslog_degree;
		update ++;
		set_update(syslog);		
	}
	if (sys->syslog_save_degree != cfg->syslog_save_degree){
		sys->syslog_save_degree = cfg->syslog_save_degree;
		update ++;
		set_update(syslog);		
	}

	if (strcmp(sys->syslog_ip, cfg->syslog_ip))
	{
		safe_strncpy(sys->syslog_ip, cfg->syslog_ip, sizeof(sys->syslog_ip));
		update ++;
		set_update(syslog);
	}
	/*add by leijinbao 2013/8/21 for 5500*/
//#ifdef CONFIG_PRODUCT_5500
	if (sys->syslog_port != cfg->syslog_port){
			sys->syslog_port = cfg->syslog_port;
			update ++;
			set_update(syslog); 	
	}

	if (sys->syslog_console_en!= cfg->syslog_console_en){
			sys->syslog_console_en = cfg->syslog_console_en;
			update ++;
			set_update(syslog);	
	}

	if (sys->syslog_console_save_degree != cfg->syslog_console_save_degree){
		sys->syslog_console_save_degree = cfg->syslog_console_save_degree;
		update ++;
		set_update(syslog);		
	}	
	/*end by leijinbao 2013/9/2*/
	return update;
}
/*begin by leijinbao 2013/09/05*/
char *int_to_char(uint8_t m,char *str, unsigned int size)
{
	uint8_t i = m;
	char *ptr=NULL;
	switch(i)
	{
		case 0:	ptr = "emerg"; break;
		case 1:	ptr = "alert"; break;
		case 2:	ptr = "crit"; break;
		case 3:	ptr = "err"; break;
		case 4:	ptr = "warning"; break;
		case 5:	ptr = "notice"; break;
		case 6:	ptr = "info"; break;
		case 7:	ptr = "debug"; break;
	}
	//return buf;
	safe_strncpy(str, ptr, size);
}
void write_date(char *tmp)
{
	int fd = 0;
	if(NULL == tmp)
	{
		return -1;
	}
	if((fd=open(SYSLOG_CONFIG_PATH,O_RDWR|O_CREAT|O_TRUNC,777))==-1)
	{
		perror("open");
		return -1;
	}
	else
	{	
		//lseek(fd,-len,SEEK_SET);
		if((write(fd,tmp,strlen(tmp)))<0)
		{
			perror("write");
			return -1;
		}
	}
	close(fd);
}
void syslog_start(void)
{
    char temp[1024]={0};
	char dtmp[64]={0};
	char sdtmp[64]={0};
	char codtmp[64]={0};
	char cmd[64]={0};
    struct in_addr  ip;

	sys_syslog.syslog_console_en = ENABLE;
	sys_syslog.syslog_console_save_degree = LOG_DEBUG;
	
	if(1==sys_syslog.syslog_en)
	{
		int_to_char(sys_syslog.syslog_degree,dtmp,sizeof(dtmp));
		int_to_char(sys_syslog.syslog_save_degree,sdtmp,sizeof(sdtmp));
		int_to_char(sys_syslog.syslog_console_save_degree,codtmp,sizeof(codtmp));
	
		inet_aton(sys_syslog.syslog_ip,&ip);
		#if 1
		if(1==sys_syslog.syslog_console_en)
		{
			 sprintf(temp,"*.%s	%s\n",codtmp,SYSLOG_CONSOLE_PATH);
		}
		else
		{
			 sprintf(temp,"#*.%s	%s\n",codtmp,SYSLOG_CONSOLE_PATH);
		}
		write_date(temp);
		#endif
        if(ip.s_addr ==0)//ip为空
        {
            sprintf(temp+strlen(temp),"*.%s	%s\n",sdtmp,SYSLOG_FILE_PATH);
			sprintf(temp+strlen(temp),"#*.%s	@%s:%d",dtmp,sys_syslog.syslog_ip,sys_syslog.syslog_port);
        }
        else //传送日志到到远程主机
        {

		 	 sprintf(temp+strlen(temp),"*.%s	%s\n",sdtmp,SYSLOG_FILE_PATH);
		 	 sprintf(temp+strlen(temp),"*.%s	@%s:%d",dtmp,sys_syslog.syslog_ip,sys_syslog.syslog_port);
        }
		write_date(temp);
		sprintf(cmd,"%s &",SYSLOG_STARTUP_CMD);
		startup_process(cmd);
    }
}

/*****************************************************************
  Function:        sys_syslog_apply
  Description:     sys_syslog_apply
  Author: 	       zhouguanhua
  Date:   		   2013/4/7
  INPUT:           cfg
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/
int sys_syslog_apply(int apply_options)
{
    
	sys_syslog_t *sys = &sys_syslog;
	    
    if(set_process_status(SYSLOG_NAME, SYSLOG_STARTUP_CMD, PROCESS_DISABLE) < 0)//shut down process if it lives
    {
        DBG_PRINTF(" ERROR: do syslogd process failed!");
        return -1; 
    } 
    syslog_start();//if shut down process,do nothing
    
	return 1;
}

/*****************************************************************
  Function:        sys_log_commit
  Description:     sys_log_commit
  Author: 	         zhouguanhua
  Date:   		   2013/4/7
  INPUT:           apply_options
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/
int sys_syslog_commit(int apply_options)
{	
    char val[30];
    sys_syslog_t *sys = &sys_syslog;

    if (test_update(syslog))
    {
        /*syslog_en*/
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_ENABLED,sys->syslog_en ? "1" : "0"))
        {
            return -1;  
        }

        sprintf(val, "%u", sys->syslog_degree);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_DEGREE, val))
        {
            return -1;  
        }

        memset(val,0,sizeof(val));
        sprintf(val, "%u", sys->syslog_save_degree);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_SAVE_DEGREE, val))
        {
            return -1;  
        }
 
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_IP, sys->syslog_ip))
        {
            return -1;  
        }
		/*add by leijinbao for 5500*/

//#ifdef CONFIG_PRODUCT_5500		
		memset(val,0,sizeof(val));
        sprintf(val, "%d", sys->syslog_port);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_PORT, val))
        {
			return -1;  
        }
#if 0

		/*syslog_console_en*/
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_CONSOLE_ENABLED,sys->syslog_console_en ? "1" : "0"))
        {
            return -1;  
        }

		/*syslog_console_save_degree*/
		memset(val,0,sizeof(val));
        sprintf(val, "%u", sys->syslog_console_save_degree);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SYSLOG_CONSOLE_SAVE_DEGREE, val))
        {
			return -1;  
        }
#endif	

        clr_update(syslog);
    }
	return 1;
}


