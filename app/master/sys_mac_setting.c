
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
#include <lw_drv_req.h>



/*****************************************************************
  Function:        sys_mac_load
  Description:     sys_mac_load
  Author: 	       huangmingjian
  Date:   		   2013/09/08
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

void sys_mac_setting_load(mac_setting_t *sys)
{
	int invalid = 0;
	char val[BUF_SIZE_256] = {0};
	char buf[16]= {0};

	sprintf(buf, "%d", MAC_AGINGTIME_DEFAULT);
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_MAC_SEETING_AGING_TIME, (void *)val, buf, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_MAC_SEETING_AGING_TIME faild!");
	}
	
	if (val[0])
	{
		sys->aging_time = strtoul(val, NULL, 0);
		if(0 != Ioctl_SetMacAgeTime(sys->aging_time*10))
		{	
			invalid ++;
		}
	}
	else 
	{
		invalid ++;
	}

	if (invalid > 0)
	{	
		DBG_ASSERT(0, "Invalid SYS MAC SETTING INFO");
	}    
}


/*****************************************************************
  Function:        sys_mac_setting_update
  Description:     sys_mac_setting_update
  Author: 	       huangmingjian
  Date:   		   2013/09/08
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/
int sys_mac_setting_update(mac_setting_t *cfg)
{
	mac_setting_t *sys = &sys_mac_setting;
	int update = 0;

	if (sys->aging_time!= cfg->aging_time){
		sys->aging_time = cfg->aging_time;
		update ++;
		set_update(mac_setting);
	}
	
	return update;
}


/*****************************************************************
  Function:        sys_mac_setting_apply
  Description:     sys_mac_setting_apply
  Author: 	       huangmingjian
  Date:   		   2013/09/08
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

int sys_mac_setting_apply(int apply_options)
{
	mac_setting_t *sys = &sys_mac_setting;

	if(0 != Ioctl_SetMacAgeTime(sys_mac_setting.aging_time*10))
	{
		return -1;
	}

	return 1;
}

/*****************************************************************
  Function:        sys_mac_setting_commit
  Description:     sys_mac_setting_commit
  Author: 	       huangmingjian
  Date:   		   2013/09/08
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

int sys_mac_setting_commit(int apply_options)
{	
    mac_setting_t *sys = &sys_mac_setting;
	char val[16] = {0};
	
    if (test_update(mac_setting))
    {	
    	sprintf(val, "%d", sys->aging_time);
        if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_MAC_SEETING_AGING_TIME, (void *)val))
        {
			clr_update(mac_setting);
            return -1;  
        }

        clr_update(mac_setting);
    }
	
	return 1;
}


