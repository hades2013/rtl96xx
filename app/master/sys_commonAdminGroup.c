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

void sys_commonAdminGroup_load(commonAdminGroup_t *sys)
{
	int invalid = 0;
	int i = 0;
	char val[64]; 
	#if 1	
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_AdminGroup_NELogicalID, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get commonNELogicalID faild");
        return;
    }
	strncpy(sys->commonNELogicalID, val, sizeof(sys->commonNELogicalID));

    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_AdminGroup_CheckCode, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get commonCheckCode faild");
        return;
    }
	sys->commonCheckCode = strtoul(val, NULL, 0);
	
	memset(val, 0, sizeof(val));  
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_AdminGroup_TrapCommunity, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get commonTrapCommunityString faild");
		return;
	}
	strncpy(sys->commonTrapCommunityString, val, sizeof(sys->commonTrapCommunityString));

	#endif
	set_update(commonAdminGroup);
}

int sys_commonAdminGroup_update(commonAdminGroup_t *cfg)
{
	commonAdminGroup_t *sys = &sys_commonAdminGroup;
	int i = 0;
	int update = 0;
#if 1
    if (0 != memcmp(sys->commonNELogicalID, cfg->commonNELogicalID, sizeof(sys->commonNELogicalID)))
    {   
		memcpy(sys->commonNELogicalID, cfg->commonNELogicalID, sizeof(sys->commonNELogicalID));
        update ++;
       
        set_update(commonAdminGroup);
    }
	if (sys->commonCheckCode!= cfg->commonCheckCode)
    {   
		sys->commonCheckCode= cfg->commonCheckCode;	
        update ++;
       
        set_update(commonAdminGroup);
    }
	if (0 != memcmp(sys->commonTrapCommunityString, cfg->commonTrapCommunityString, sizeof(sys->commonTrapCommunityString)))
    {   
		memcpy(sys->commonTrapCommunityString, cfg->commonTrapCommunityString, sizeof(sys->commonTrapCommunityString));
        update ++;
       
        set_update(commonAdminGroup);
    }
#endif
	return update;
}


int sys_commonAdminGroup_commit(int apply_options)
{
	commonAdminGroup_t *sys = &sys_commonAdminGroup;
    char val[16]={0};
#if 1
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_AdminGroup_NELogicalID, sys->commonNELogicalID))
	{
		DBG_ASSERT(0, "Set commonNELogicalID faild");
		return -1;	
	}
	
    memset(val, 0, sizeof(val));
    sprintf(val,"%d",sys->commonCheckCode);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_AdminGroup_CheckCode, val))
	{
		DBG_ASSERT(0, "Set commonCheckCode faild");
		return -1;	
	}

	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_AdminGroup_TrapCommunity, sys->commonTrapCommunityString))
	{
		DBG_ASSERT(0, "Set commonTrapCommunityString faild");
		return -1;	
	}
#endif
	return 1;
}

