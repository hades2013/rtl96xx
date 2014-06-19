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
#include "memshare.h"
#include <debug.h>
#include <systemlog.h>
PORT_LOOPD_STATE_S *g_stLoopdState = NULL;

void sys_loop_load(sys_loop_t *sys)
{
	int invalid = 0;
	char val[32];  

	/*Begin modified by huangmingjian 2012/11/27 for EPN104QID0083*/
	/* loop_enabled */		
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_LOOP_ENABLED, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get loop_enabled  faild");
    }

	if (val[0])
    {
		sys->loopback_enable = strtoul(val, NULL, 0);       
	}
    else 
    {
            invalid ++;
    }

	/* loop_update*/	
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_LOOP_UPDATE, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get loop_update faild");
    }
	if (val[0])
    {
		sys->loop_update = strtoul(val, NULL, 0);       
	}
    else 
    {
        invalid ++;
    }

	if (invalid > 0){
		DBG_ASSERT(0, "Invalid LOOP INFO");
	}

	/*End modified by huangmingjian 2012/11/27 for EPN104QID0083 */
}

/*****************************************************************
    Function:loopd_init
    Description: startup loopd if necessary ,and initialize the share memory
    Author:huangmingjian
    Date:2012/12/03
    Input:     
    Output:         
    Return:
    Other:
*****************************************************************/
/*begin modified by liaohongjun 2012/12/10 of EPN104QID0086*/
void loopd_init(void)
{
    port_num_t      lgcPort = 0;
    
    if(NULL == (g_stLoopdState = LoopdPortCurStateShmInit()))
    {
        DBG_PRINTF("loopd port state shared memory init error!");
        return ;        
    }

    /*logic port start from 1,we set port 0 status config to 0xFF as invalid value*/
    g_stLoopdState->port[0].curstate = 0xFF;
    /*init all port loopback status config to  PORT_STATE_DISABLED*/        
    for(lgcPort = 1; lgcPort < (MAX_PORT_NUM + 1); lgcPort++)
    {
            g_stLoopdState->port[lgcPort].curstate = PORT_STATE_DISABLED;     
    }   
    
	if(1 == sys_loop.loopback_enable)
	{
	    startup_process(LOOP_STARTUP_CMD);
	} 
    
	return ;
}
/*end modified by liaohongjun 2012/12/10 of EPN104QID0086*/

int sys_loop_update(sys_loop_t *cfg)
{
	sys_loop_t *sys = &sys_loop;
	int update = 0;
	
	/*Begin modified by huangmingjian 2012/11/27 for EPN104QID0083*/
	/* loopback  */	
	if (sys->loopback_enable != cfg->loopback_enable)
	{
		sys->loopback_enable = cfg->loopback_enable;
		update++;
		set_update(loop_enable);
	}
	if (sys->loop_update != cfg->loop_update)
	{
		sys->loop_update = cfg->loop_update;
		update++;
	}
	/*End modified by huangmingjian 2012/11/27 for EPN104QID0083 */
	return update;	
}

int sys_loop_apply(int apply_options)
{   

	sys_loop_t *sys = &sys_loop;
	/*Begin modified by huangmingjian 2012/11/28 for EPN104QID0083*/ 
	/* set loopback */
	if (test_update(loop_enable))
	{	
		if(0 != apply_options) /*avoid setting process status when the master start, otherwise it will reset again and again */
		{	
			if(set_process_status(LOOPD_NAME, LOOP_STARTUP_CMD, sys->loopback_enable) < 0)
	      	{
	       		DBG_PRINTF(" ERROR: set loopback status failed!");
	       		return  -1; 
	      	}	
		}
		clr_update(loop_enable);
	}
	/*End modified by huangmingjian 2012/11/28 for EPN104QID0083 */
	return 1;

}


int sys_loop_commit(int apply_options)
{
    sys_loop_t *sys = &sys_loop;
    char val[5]={0};

    memset(val, 0, sizeof(val));
	sprintf(val,"%d",sys->loopback_enable);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_LOOP_ENABLED,val))
	{
		return -1;	
	}
	memset(val, 0, sizeof(val));
	sprintf(val,"%d",sys->loop_update);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_LOOP_UPDATE,val))
	{
		return -1;	
	}
	/*add by leijinbao 2013/11/21 for 281*/
	if(ENABLE == sys->loopback_enable)
	{
		USER_LOG(LangId,LOOP,LOG_INFO,M_LOOP_STA,"",""); //add by leijinbao 2013/9/17
	}
	else
	{
		USER_LOG(LangId,LOOP,LOG_INFO,M_LOOP_EXIT,"",""); //add by leijinbao 2013/9/17
	}
	//end by leijinbao for 281
	return 1;	
}
