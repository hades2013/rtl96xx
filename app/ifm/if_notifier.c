/*****************************************************************************
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif

#include <lw_type.h>
#include <lw_list.h>
#include <stdlib.h>
#include <lw_if_pub.h>
#include <lw_if_type.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <string.h>
#include <stdio.h>
#include "if_notifier.h"

#define rcu_assign_pointer(p, v)        ({ (p) = (v); \
                                        })
#define rcu_dereference(p)     ({ \
                                    typeof(p) _________p1 = p; \
                                    (_________p1); \
                                    })
                                    
#define RAW_NOTIFIER_INIT(name) {                               \
                    .head = NULL }
    
#define RAW_NOTIFIER_HEAD(name)                                 \
        struct raw_notifier_head name =                         \
                RAW_NOTIFIER_INIT(name)

static RAW_NOTIFIER_HEAD(ifm_chain);

static int notifier_chain_register(struct notifier_block **nl,
                struct notifier_block *n)
{
        while ((*nl) != NULL) {
                if (n->priority > (*nl)->priority)
                        break;
                nl = &((*nl)->next);
        }
        n->next = *nl;
        rcu_assign_pointer(*nl, n);
        return 0;
}
static int notifier_chain_unregister(struct notifier_block **nl,
                struct notifier_block *n)
{
        while ((*nl) != NULL) {
                if ((*nl) == n) {
                        rcu_assign_pointer(*nl, n->next);
                        return 0;
                }
                nl = &((*nl)->next);
        }
        return -1;
}
static int  notifier_call_chain(ifindex_t ifindex,struct notifier_block **nl,
                unsigned long val, void *v)
{
        int ret = IF_NOTIFY_OK;
        struct notifier_block *nb, *next_nb;

        nb = rcu_dereference(*nl);
        while (nb) {
                next_nb = rcu_dereference(nb->next);
                if((nb->ifindex!=IF_INVALID_IFINDEX)&& (nb->ifindex==ifindex))
                {
                    if(nb->eventmask&val)
                    {
                        ret = nb->notifier_call(ifindex,nb, val, v);
                    }
                }else if(nb->iftypemask&(1<<IF_SUB_TYPE(ifindex)))
                {
                    if(nb->eventmask&val)
                    {
                        ret = nb->notifier_call(ifindex,nb, val, v);
                    }
                }
                if (ret==IF_NOTIFY_STOP||ret==IF_NOTIFY_PAUSE||ret==IF_NOTIFY_ERR)
                        break;
                nb = next_nb;
        }
        return ret;
}

int raw_notifier_call_chain(ifindex_t ifindex,struct raw_notifier_head *nh,
                unsigned long val, void *v)
{
        return notifier_call_chain(ifindex,&nh->head, val, v);
}

int raw_notifier_chain_register(struct raw_notifier_head *nh,
                struct notifier_block *n)
{
        return notifier_chain_register(&nh->head, n);
}

int raw_notifier_chain_unregister(struct raw_notifier_head *nh,
                struct notifier_block *n)
{
        return notifier_chain_unregister(&nh->head, n);
}
int ifm_notifier_register(struct notifier_block *nb)
{
    if(nb==NULL)
    {
        return -1;
    }
    return raw_notifier_chain_register(&ifm_chain, nb);
}
int ifm_notifier_unregister(struct notifier_block *nb)
{
    if(nb==NULL)
    {
        return -1;
    }
    return raw_notifier_chain_unregister(&ifm_chain, nb);
}
int ifm_call_notifiers(ifindex_t ifindex,unsigned long val, void *v)
{
        return raw_notifier_call_chain(ifindex,&ifm_chain, val, v);
}

#ifdef  __cplusplus
}
#endif
