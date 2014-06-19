#include <lw_type.h>
#include <lw_config_oid.h>
#include <lw_config_api.h>
#include <lw_if_pub.h>
#include "utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#include <timer.h>
#include <if_notifier.h>
#include <stdio.h>
#include <string.h>


static struct notifier_block stIfEventNotifier;



extern int master_cfg_del(int ifindex,unsigned int oid);

IF_RET_E if_delevent_proc(ifindex_t ifindex)
{
    IF_RET_E  iRet;   
	
	iRet= master_cfg_del(ifindex,0);
    if (0!=iRet)
    {    	 
        return IF_DELETE_ERR;
    }
    else
    {       
        return IF_OK;
    }
}


IF_CALLBACK_RET_T if_event_handler(ifindex_t ifindex,struct notifier_block *nb, unsigned long event, void *arg)
{
    INT32 iRet = IF_OK;
    (void)arg;
    char buf[64];
    IF_GetAttr(ifindex,IF_NAME,buf,sizeof(buf));
    switch (event)
    {      
        case IF_NOTIFY_INTF_ALREADYREM:		
        iRet= if_delevent_proc(ifindex);		
        break;
        default:
            return IF_NOTIFY_OK;
    }
    if (IF_OK!=iRet)
    {
        return IF_NOTIFY_ERR;
    }
    else
    {       
        return IF_NOTIFY_OK;
    }

}

void  master_ifcfg_init(void)
{ 
	stIfEventNotifier.priority = IF_CFG_PRIO;
	stIfEventNotifier.ifindex=IF_INVALID_IFINDEX;
	stIfEventNotifier.iftypemask=IF_ALL_TYPEMASK;
	stIfEventNotifier.notifier_call = if_event_handler;
	stIfEventNotifier.eventmask = (if_event_t)IF_NOTIFY_INTFADD|(if_event_t)IF_NOTIFY_INTF_ALREADYREM;  
	ifm_notifier_register(&stIfEventNotifier);

}

#ifdef  __cplusplus
}
#endif


