#include "master.h"
#include "timer.h"
#include <ipc_protocol.h>
#include "build_time.h"
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
#include "lw_drv_req.h"

#include <systemlog.h>

void sys_qos_load(sys_qos_t *sys)
{
    #if 1
    char val[BUF_SIZE32] = {0};
    char qosmode = 0;
    QueueMode_S qos_queuemode;

    //Ioctl_SetQosInit();
    memset(&qos_queuemode, 0, sizeof(QueueMode_S));
	memset(val, 0, sizeof(val));    
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_QOS_MODE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get QOS mode faild");
		return;
	}
	if (val[0])
	{
	    if(0==strcmp("COS", val))
        {
            sys->uiQosTrustmode = 0;
        }
        else
        {
             sys->uiQosTrustmode = 1;
        }
	}
	else 
	{
		DBG_ASSERT(0, "Get QOS mode faild.");
		return;
	}

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_QOS_QUEUEMODE, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get QOS queuemode faild");
		return;
	}
	if (val[0])
	{
        if(0==strcmp("WRR", val))
        {
            sys->Qosqueuemode.QueueMode = 0;
        }
        else
        {
            sys->Qosqueuemode.QueueMode = 1;
        }
	}
	else 
	{
		DBG_ASSERT(0, "Get QOS queuemode faild.");
		return;
	}

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_QOS_WEIGHT+1, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get QOS queueweight1 faild");
		return;
	}
	if (val[0])
	{
		sys->Qosqueuemode.Weight[0] = strtoul(val, NULL, 0); 
	}
	else 
	{
		DBG_ASSERT(0, "Get QOS queueweight1 faild.");
		return;
	}

	memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_QOS_WEIGHT+2, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get QOS queueweight2 faild");
		return;
	}
	if (val[0])
	{
		sys->Qosqueuemode.Weight[1]= strtoul(val, NULL, 0); 
	}
	else 
	{
		DBG_ASSERT(0, "Get QOS queueweight2 faild.");
		return;
	}


    memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_QOS_WEIGHT+3, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get QOS queueweight3 faild");
		return;
	}
	if (val[0])
	{
		sys->Qosqueuemode.Weight[2]= strtoul(val, NULL, 0); 
	}
	else 
	{
		DBG_ASSERT(0, "Get QOS queueweight3 faild.");
		return;
	}

    memset(val, 0, sizeof(val));
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_QOS_WEIGHT+4, (void *)val, "", sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get QOS queueweight4 faild");
		return;
	}
	if (val[0])
	{
		sys->Qosqueuemode.Weight[3]= strtoul(val, NULL, 0); 
	}
	else 
	{
		DBG_ASSERT(0, "Get QOS queueweight4 faild.");
		return;
	}
    #else
    sys->uiQosTrustmode = 0;
    sys->Qosqueuemode.QueueMode = 0;
    sys->Qosqueuemode.Weight[0] = 2;
    sys->Qosqueuemode.Weight[1] = 3;
    sys->Qosqueuemode.Weight[2] = 5;
    sys->Qosqueuemode.Weight[3] = 21;
    #endif
    
    return;

}

void sys_qos_apply(void)
{
    if(DRV_OK != Ioctl_SetQosTrustMode(sys_qos.uiQosTrustmode)) 
    {
        return;
    }
    
    if(DRV_OK != Ioctl_SetQosQueueSchedul(sys_qos.Qosqueuemode)) 
    {
        return;                 
    }
    USER_LOG(LangId,QOS,LOG_INFO,M_QOS_STA,"",""); //add by leijinbao 2013/9/17
    return;
}

INT sys_qosmode_update(UINT32 qosmode)
{
	sys_qos_t *sys = &sys_qos;
	int update = 0;


    if(sys->uiQosTrustmode != qosmode)
    {
        sys->uiQosTrustmode = qosmode;
		update ++;
		set_update(qos);
    }
    
	return update;
}

INT sys_queuesch_update(QueueMode_S *queuesch)
{
	sys_qos_t *sys = &sys_qos;
	int update = 0;

    if(sys->Qosqueuemode.QueueMode != queuesch->QueueMode)
    {
        sys->Qosqueuemode.QueueMode = queuesch->QueueMode;
		update ++;
		set_update(qos);
    }

    if(sys->Qosqueuemode.Weight[0] != queuesch->Weight[0])
    {
        sys->Qosqueuemode.Weight[0] = queuesch->Weight[0];
        update ++;
        set_update(qos);
    }
    if(sys->Qosqueuemode.Weight[1] != queuesch->Weight[1])
    {
        sys->Qosqueuemode.Weight[1] = queuesch->Weight[1];
        update ++;
        set_update(qos);
    }

    if(sys->Qosqueuemode.Weight[2] != queuesch->Weight[2])
    {
        sys->Qosqueuemode.Weight[2] = queuesch->Weight[2];
        update ++;
        set_update(qos);
    }

    if(sys->Qosqueuemode.Weight[3] != queuesch->Weight[3])
    {
        sys->Qosqueuemode.Weight[3] = queuesch->Weight[3];
        update ++;
        set_update(qos);
    }

	return update;

}

INT sys_qos_trustmode_commit(unsigned char apply_options)
{
    sys_qos_t *sys = &sys_qos;

    if (test_update(qos))
    {
        /*syslog_en*/
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_QOS_MODE, sys->uiQosTrustmode ? QOS_DSCP_STR : QOS_COS_STR))
        {
            return -1;  
        }
        clr_update(qos);
    }
    return 1;
}

INT sys_qos_queueschedul_commit(unsigned char apply_options)
{
    char val[30] = {0};
    sys_qos_t *sys = &sys_qos;

    if (test_update(qos))
    {
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_QOS_QUEUEMODE, sys->Qosqueuemode.QueueMode ? "HQ-WRR" : "WRR"))
        {
            return -1;  
        }
        memset(val, 0, sizeof(val));
        sprintf(val, "%d", sys->Qosqueuemode.Weight[0]);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_QOS_WEIGHT+1, val))
        {
            return -1;  
        }

        memset(val, 0, sizeof(val));
        sprintf(val, "%d", sys->Qosqueuemode.Weight[1]);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_QOS_WEIGHT+2, val))
        {
            return -1;  
        }
        
        memset(val, 0, sizeof(val));
        sprintf(val, "%d", sys->Qosqueuemode.Weight[2]);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_QOS_WEIGHT+3, val))
        {
            return -1;  
        }

        memset(val, 0, sizeof(val));
        sprintf(val, "%d", sys->Qosqueuemode.Weight[3]);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_QOS_WEIGHT+4, val))
        {
            return -1;  
        }

        clr_update(qos);
    }
    
	return 1;
}




