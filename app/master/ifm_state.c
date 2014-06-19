
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

static struct notifier_block g_stIfmStateUp;
static struct notifier_block g_stIfmStateDown;


static  int ifm_setstate(ifindex_t ifindex, INT32 logstate)
{
    
    char name[IFNAMESIZE] = {0};

    memset(name, 0, sizeof(name));
   
    IF_GetAttr(ifindex, IF_PSEUDONAME, name, sizeof(name));
   
    return IF_SetAttr(ifindex, IF_STATE, &logstate, sizeof(logstate));
}

int ifm_state_notifier(ifindex_t ifindex,struct notifier_block *nb, unsigned long event, void *arg)
{   
    INT32 ret = 0; 
    
    switch (event)
    {
        case IF_NOTIFY_INTFADD:
            {
                
                INT32 logstate = IFM_PHYLINKDOWN;

                /* sanity check */
                ret = IF_GetAttr(ifindex, IF_STATE, &logstate, sizeof(logstate));

                if (ret != 0)
                {               
                    return 0;
                }

                ret= ifm_setstate(ifindex, IFM_PHYLINKDOWN);
            }
            break;
        case IF_NOTIFY_ADDRADD:
            {
                ret = ifm_setstate(ifindex, IFM_LINKVALID);
            }
            break;
        case IF_NOTIFY_ADDRREM:
            {
                ret = ifm_setstate(ifindex, IFM_LINKINVALID);
            }
            break;
        case IF_NOTIFY_PHYUP:
            {
                
                ret= ifm_setstate(ifindex,IFM_NOIPGET);
               
            }
            break;
        case IF_NOTIFY_DIALUP:
            {
                ret= ifm_setstate(ifindex,IFM_LINKINVALID);
            }
            break;
		case IF_NOTIFY_PHYDOWN:
            ret = ifm_setstate(ifindex, IFM_PHYLINKDOWN);
			break;
        case IF_NOTIFY_DIALDOWN:
            {
                ret= ifm_setstate(ifindex,IFM_NOIPGET);    
            }
            break;
        default:
            return IF_NOTIFY_ERR;
            
    }
    if (ret)
    {        
        return IF_NOTIFY_ERR;
    }
    else
    {        
        return IF_NOTIFY_OK;
    }   
}
void ifm_state_init(void)
{
    g_stIfmStateUp.ifindex=IF_INVALID_IFINDEX;
    g_stIfmStateUp.iftypemask=IF_ALL_TYPEMASK;
    g_stIfmStateUp.notifier_call=ifm_state_notifier;
    g_stIfmStateUp.priority=IF_MSTDEV_INIT_PRIO;
    g_stIfmStateUp.eventmask=(if_event_t)IF_NOTIFY_INTFADD | IF_NOTIFY_ADDRADD | IF_NOTIFY_PHYUP | IF_NOTIFY_DIALUP;

    ifm_notifier_register(&g_stIfmStateUp);
    
    g_stIfmStateDown.ifindex=IF_INVALID_IFINDEX;
    g_stIfmStateDown.iftypemask=IF_ALL_TYPEMASK;
    g_stIfmStateDown.notifier_call=ifm_state_notifier;
    g_stIfmStateDown.priority=IF_MSTDEV_DOWN_PRIO;
    g_stIfmStateDown.eventmask=(if_event_t)IF_NOTIFY_ADDRREM | IF_NOTIFY_PHYDOWN | IF_NOTIFY_DIALDOWN;
    ifm_notifier_register(&g_stIfmStateDown);
}
