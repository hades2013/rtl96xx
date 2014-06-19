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
#if defined(CONFIG_ONU_COMPATIBLE)
void sys_compatible_load(compatible_setting_t *sys)
{
	int invalid = 0;
	char val[32];  
		
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_COMPATIBLE_MODE, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get compatible mode  faild");
    }

	if (val[0])
    {
		sys->enable = strtoul(val, NULL, 0);       
	}
    else 
    {
            invalid ++;
    }

	if (invalid > 0)
	{
		DBG_ASSERT(0, "Invalid compatible INFO");
	}
	set_update(compatible);
}

int sys_compatible_update(compatible_setting_t *cfg)
{
	compatible_setting_t *sys = &sys_compatible;

	int update = 0;

    if (sys->enable!= cfg->enable)
    {   
		sys->enable= cfg->enable;	
        update ++;
       
        set_update(compatible);
    }
	return update;
}


int sys_compatible_commit(int apply_options)
{
	compatible_setting_t *sys = &sys_compatible;
    char val[5]={0};

    memset(val, 0, sizeof(val));
    sprintf(val,"%d",sys->enable);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_COMPATIBLE_MODE, val))
	{
		return -1;	
	}	
	return 1;
}

#endif


