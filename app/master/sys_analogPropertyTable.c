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

void sys_analogPropertyTable_load(AnalogProperty_t *sys)
{
	int invalid = 0;
	int i = 0;
	char val[64]; 
	sys->index = 0;
	for(i=0; i< ANALOGALARM_TABLE; i++)
    {	
		sys->AnalogAlarmPropertyTable[i].analogAlarmState = 1;
	}	
    for(i=0; i< ANALOGALARM_TABLE; i++)
    {	
    	memset(val, 0, sizeof(val)); 
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_ANALOG_PROPERTY_ENABLED+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get analog property enabled faild");
            return;
        }		
		sys->AnalogAlarmPropertyTable[i].alarmEnable[0] = strtoul(val, NULL, 0);

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_ANALOG_PROPERTY_HIHI+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get analog property hihi  faild");
            return;
        }
		sys->AnalogAlarmPropertyTable[i].analogAlarmHIHI = strtoul(val, NULL, 0);

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_ANALOG_PROPERTY_HI+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get analog property hi  faild");
            return;
        }
		sys->AnalogAlarmPropertyTable[i].analogAlarmHI = strtoul(val, NULL, 0);
		
		memset(val, 0, sizeof(val));  
		if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_ANALOG_PROPERTY_LO+i+1, (void *)val, "", sizeof(val))) < 0)
		{
			DBG_ASSERT(0, "Get analog property lo  faild");
			return;
		}
		sys->AnalogAlarmPropertyTable[i].analogAlarmLO = strtoul(val, NULL, 0);

		memset(val, 0, sizeof(val));  
		if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_ANALOG_PROPERTY_LOLO+i+1, (void *)val, "", sizeof(val))) < 0)
		{
			DBG_ASSERT(0, "Get analog property lolo	faild");
			return;
		}
		sys->AnalogAlarmPropertyTable[i].analogAlarmLOLO = strtoul(val, NULL, 0);
		
		memset(val, 0, sizeof(val));  
		if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_ANALOG_PROPERTY_DEADBAND+i+1, (void *)val, "", sizeof(val))) < 0)
		{
			DBG_ASSERT(0, "Get analog property deadband	faild");
			return;
		}
		sys->AnalogAlarmPropertyTable[i].analogAlarmDeadband = strtoul(val, NULL, 0);
	}
	set_update(analogPropertyTable);
}

int sys_analogPropertyTable_update(AnalogProperty_t *cfg)
{
	AnalogProperty_t *sys = &sys_AnalogProperty;
	int i = 0;
	int update = 0;
	sys->index = cfg->index;
    if (sys->AnalogAlarmPropertyTable[cfg->index].alarmEnable[0]!= cfg->AnalogAlarmPropertyTable[cfg->index].alarmEnable[0])
    {   
		sys->AnalogAlarmPropertyTable[cfg->index].alarmEnable[0]= cfg->AnalogAlarmPropertyTable[cfg->index].alarmEnable[0];	
        update ++;
    }
	if (sys->AnalogAlarmPropertyTable[cfg->index].analogAlarmState!= cfg->AnalogAlarmPropertyTable[cfg->index].analogAlarmState)
    {   
		sys->AnalogAlarmPropertyTable[cfg->index].analogAlarmState= cfg->AnalogAlarmPropertyTable[cfg->index].analogAlarmState;	
        update ++;
    }
	if (sys->AnalogAlarmPropertyTable[cfg->index].analogAlarmHIHI!= cfg->AnalogAlarmPropertyTable[cfg->index].analogAlarmHIHI)
    {   
		sys->AnalogAlarmPropertyTable[cfg->index].analogAlarmHIHI= cfg->AnalogAlarmPropertyTable[cfg->index].analogAlarmHIHI;	
        update ++;
    }
	if (sys->AnalogAlarmPropertyTable[cfg->index].analogAlarmHI!= cfg->AnalogAlarmPropertyTable[cfg->index].analogAlarmHI)
    {   
		sys->AnalogAlarmPropertyTable[cfg->index].analogAlarmHI= cfg->AnalogAlarmPropertyTable[cfg->index].analogAlarmHI;	
        update ++;
    }
	if (sys->AnalogAlarmPropertyTable[cfg->index].analogAlarmLO!= cfg->AnalogAlarmPropertyTable[cfg->index].analogAlarmLO)
    {   
		sys->AnalogAlarmPropertyTable[cfg->index].analogAlarmLO= cfg->AnalogAlarmPropertyTable[cfg->index].analogAlarmLO;	
        update ++;
    }
	if (sys->AnalogAlarmPropertyTable[cfg->index].analogAlarmLOLO!= cfg->AnalogAlarmPropertyTable[cfg->index].analogAlarmLOLO)
    {   
		sys->AnalogAlarmPropertyTable[cfg->index].analogAlarmLOLO= cfg->AnalogAlarmPropertyTable[cfg->index].analogAlarmLOLO;	
        update ++;
    }
	if (sys->AnalogAlarmPropertyTable[cfg->index].analogAlarmDeadband!= cfg->AnalogAlarmPropertyTable[cfg->index].analogAlarmDeadband)
    {   
		sys->AnalogAlarmPropertyTable[cfg->index].analogAlarmDeadband= cfg->AnalogAlarmPropertyTable[cfg->index].analogAlarmDeadband;	
        update ++;    
    }
	if(update > 0)
	{
		set_update(analogPropertyTable);
	}
	return update;
}


int sys_analogPropertyTable_commit(int apply_options)
{
	AnalogProperty_t *sys = &sys_AnalogProperty;
    char val[5]={0};
#if 1
	memset(val, 0, sizeof(val));
    sprintf(val,"%d",sys->AnalogAlarmPropertyTable[sys->index].alarmEnable[0]);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_ANALOG_PROPERTY_ENABLED+sys->index+1, val))
	{
		DBG_ASSERT(0, "Set analog property enabled faild");
		return -1;	
	}
	
	memset(val, 0, sizeof(val));
    sprintf(val,"%d",sys->AnalogAlarmPropertyTable[sys->index].analogAlarmHIHI);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_ANALOG_PROPERTY_HIHI+sys->index+1, val))
	{
		DBG_ASSERT(0, "Set analog property hihi  faild");
		return -1;	
	}
	
	memset(val, 0, sizeof(val));
    sprintf(val,"%d",sys->AnalogAlarmPropertyTable[sys->index].analogAlarmHI);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_ANALOG_PROPERTY_HI+sys->index+1, val))
	{
		DBG_ASSERT(0, "Set analog property hi faild");
		return -1;	
	}
	
	memset(val, 0, sizeof(val));
    sprintf(val,"%d",sys->AnalogAlarmPropertyTable[sys->index].analogAlarmLO);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_ANALOG_PROPERTY_LO+sys->index+1, val))
	{
		DBG_ASSERT(0, "Set analog property lo faild");
		return -1;	
	}
	
	memset(val, 0, sizeof(val));
    sprintf(val,"%d",sys->AnalogAlarmPropertyTable[sys->index].analogAlarmLOLO);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_ANALOG_PROPERTY_LOLO+sys->index+1, val))
	{
		DBG_ASSERT(0, "Set analog property lolo  faild");
		return -1;	
	}
	
    memset(val, 0, sizeof(val));
    sprintf(val,"%d",sys->AnalogAlarmPropertyTable[sys->index].analogAlarmDeadband);	
	if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_ANALOG_PROPERTY_DEADBAND+sys->index+1, val))
	{
		DBG_ASSERT(0, "Set analog property deadband	faild");
		return -1;	
	}
#endif
	return 1;
}

