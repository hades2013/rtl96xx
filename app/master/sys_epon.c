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

void sys_epon_load(sys_epon_t *sys)
{
	int invalid = 0;
	unsigned char val[25];  

    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_EPON_LOID, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get epon_loid  faild");
    }

	if (&val[0])
    {
		safe_strncpy(sys->epon_loid, val, sizeof(sys->epon_loid));      
	}
    else 
    {
            invalid ++;
    }

    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_EPON_PASSWD, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get loop_update faild");
    }
	if (&val[0])
    {
		safe_strncpy(sys->epon_passwd, val, sizeof(sys->epon_passwd));      
	}
    else 
    {
        invalid ++;
    }

	if (invalid > 0){
		DBG_ASSERT(0, "Invalid EPON INFO");
	}

}


int sys_epon_update(sys_epon_t *cfg)
{
	sys_epon_t *sys = &sys_epon;
	int update = 0;
	if (memcmp(sys->epon_loid, cfg->epon_loid, sizeof(cfg->epon_loid)))
	{
		memcpy(sys->epon_loid, cfg->epon_loid, sizeof(cfg->epon_loid));
		update++;
	}
	if (memcmp(sys->epon_passwd, cfg->epon_passwd, sizeof(cfg->epon_passwd)))
	{
		memcpy(sys->epon_passwd, cfg->epon_passwd, sizeof(cfg->epon_passwd));
		update++;
	}
	return update;	
}

int sys_epon_apply(int apply_options)
{   
	return 0;
}


int sys_epon_commit(int apply_options)
{
    sys_epon_t *sys = &sys_epon;
    unsigned char val[25]={0};

    memset(val, 0, sizeof(val));
	sprintf(val,"%s",sys->epon_loid);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_EPON_LOID,val))
	{
		return -1;	
	}
	
	memset(val, 0, sizeof(val));
	sprintf(val,"%s",sys->epon_passwd);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_EPON_PASSWD,val))
	{
		return -1;	
	}		
	return 1;	
}

