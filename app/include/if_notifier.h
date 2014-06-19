#ifndef _IF_NOTIFY_H_
#define _IF_NOTIFY_H_

#ifdef  __cplusplus
extern "C"{
#endif

struct notifier_block {
        int (*notifier_call)(ifindex_t ifindex,struct notifier_block *, unsigned long, void *);//handle
        struct notifier_block *next;
        int priority;//此注册块的优先级，数值越小优先级越高
        ifindex_t ifindex;  //要关注的接口，如果为IF_INVALID_IFINDEX，表示所有 
        unsigned int iftypemask;//要关注的接口类型 
        unsigned int eventmask; //要关注的事件类型
};


struct raw_notifier_head {
        struct notifier_block *head;
};
#if 0
#define NOTIFY_DONE             0x0000          /* Don't care */
#define NOTIFY_OK               0x0001          /* Suits me */
#define NOTIFY_STOP_MASK        0x8000          /* Don't call further */
#define NOTIFY_BAD              (NOTIFY_STOP_MASK|0x0002)
#endif                                              /* Bad/Veto action */

#define     IF_NOTIFY_OK    0          
#define     IF_NOTIFY_ERR   1         
#define     IF_NOTIFY_STOP  2        
#define     IF_NOTIFY_PAUSE 3   

/*
 * Clean way to return from the notifier and stop further calls.
 */
 
/*事件处理定义*/
typedef UINT32 if_event_t;

enum if_notifier_priority {
    IF_IFATTR_PRIO = -1500, 
    IF_INTF8021Q_PRIO = -1400,              /* 802.1q for interface, higher than mac-clone */
    IF_MACCLONE_PRIO = -1300,               /* MAC clone, should higher than mp_dev */
    IF_MSTDEV_INIT_PRIO = -1000,             /* MP's device managment */
    IF_MWAN_PHYUP_PRIO = -850,              /* Multi-wan physical up, higher than dial module */
    IF_DNS_PRIO = -800,                     /* Dnsmasq, set route entry */
    IF_LINKSPY_PRIO = -700,                 /* Link spy, higher than multi-wan */
    IF_MWAN_REST_PRIO = -600,               /* Multi-wan address-add, address-remove & phy-down*/
    IF_MSTDIAL_PRIO = -500,                  /* dial module */
    IF_MSTUPNP_PRIO = -450,                  /* for upnp lan interface event only */
    IF_SRCRT_PRIO = -450,                   /* for source route */
    IF_MSTDEV_DOWN_PRIO = -400,              /* MP's device managment for down */
    IF_MSTARP_PRIO = -300,                   /* arp */
    IF_MSTDHCPD_PRIO = -200,                 /* dhcpd server */
    IF_MSTACL_PRIO = -100,                   /* acl */
    IF_MSTROUTE_PRIO = -90,
    IF_NOTIFIER_PRIO_NORMAL = 0,            /* normal priority */ 
    IF_ROUTE_ADDR_PRIO = 500,               /* multi-wan, policy route, dst route, addr add & rem priority, lower than all applications */
    IF_DIAL_INET_FORWARDING_PRIO = 1600,    
    IF_CFG_PRIO = 2000
};

enum if_event_shift {
    IF_NOTIFY_PHYUP_SHIFT = 0, 
    IF_NOTIFY_PHYDOWN_SHIFT, 
    IF_NOTIFY_ADDRADD_SHIFT, 
    IF_NOTIFY_ADDRREM_SHIFT, 
    IF_NOTIFY_INTFADD_SHIFT, 
    IF_NOTIFY_INTFREM_SHIFT,
    IF_NOTIFY_INTFENABLE_SHIFT, 
    IF_NOTIFY_INTFDISABLE_SHIFT, 
    IF_NOTIFY_BRIDGEATTACH_SHIFT,
    IF_NOTIFY_BRIDGEDETACH_SHIFT, 
    IF_NOTIFY_GOING_DIALUP_SHIFT,        /*for sub-intf */
    IF_NOTIFY_DIALUP_SHIFT, 
    IF_NOTIFY_DIALDOWN_SHIFT, 
    IF_NOTIFY_ALREADY_DIALDOWN_SHIFT,    /*for sub-intf */
    IF_NOTIFY_INTF_ALREADYREM_SHIFT,
    IF_NOTIFY_INTF_DIALSTART_SHIFT,
    IF_NOTIFY_INTF_DIALEND_SHIFT,
    IF_NOTIFY_ALREADY_INIT_SHIFT,
    /********在此前添加************/
    IF_NOTIFY_MAXCOUNT_SHIFT        
};

enum if_event {
    IF_NOTIFY_PHYUP = (1 << IF_NOTIFY_PHYUP_SHIFT), 
    IF_NOTIFY_PHYDOWN = (1 << IF_NOTIFY_PHYDOWN_SHIFT), 
    IF_NOTIFY_ADDRADD = (1 << IF_NOTIFY_ADDRADD_SHIFT), 
    IF_NOTIFY_ADDRREM = (1 << IF_NOTIFY_ADDRREM_SHIFT), 
    IF_NOTIFY_INTFADD = (1 << IF_NOTIFY_INTFADD_SHIFT), 
    IF_NOTIFY_INTFREM = (1 << IF_NOTIFY_INTFREM_SHIFT),
    IF_NOTIFY_INTFENABLE = (1 << IF_NOTIFY_INTFENABLE_SHIFT), 
    IF_NOTIFY_INTFDISABLE = (1 << IF_NOTIFY_INTFDISABLE_SHIFT), 
    IF_NOTIFY_BRIDGEATTACH = (1 << IF_NOTIFY_BRIDGEATTACH_SHIFT), 
    IF_NOTIFY_BRIDGEDETACH = (1 << IF_NOTIFY_BRIDGEDETACH_SHIFT), 
    IF_NOTIFY_GOING_DIALUP = (1 << IF_NOTIFY_GOING_DIALUP_SHIFT),
    IF_NOTIFY_DIALUP = (1 << IF_NOTIFY_DIALUP_SHIFT),
    IF_NOTIFY_DIALDOWN = (1 << IF_NOTIFY_DIALDOWN_SHIFT),
    IF_NOTIFY_ALREADY_DIALDOWN = (1 << IF_NOTIFY_ALREADY_DIALDOWN_SHIFT),
    IF_NOTIFY_INTF_ALREADYREM = (1 << IF_NOTIFY_INTF_ALREADYREM_SHIFT),
    IF_NOTIFY_INTF_DIALSTART = (1 << IF_NOTIFY_INTF_DIALSTART_SHIFT),
    IF_NOTIFY_INTF_DIALEND = (1 << IF_NOTIFY_INTF_DIALEND_SHIFT),
    IF_NOTIFY_ALREADY_INIT = (1 << IF_NOTIFY_ALREADY_INIT_SHIFT), 
    IF_NOTIFY_MAXCOUNT = (1 << IF_NOTIFY_MAXCOUNT_SHIFT), 
    IF_NOTIFY_ALL = 0xffffffff
};
typedef INT32 IF_CALLBACK_RET_T;
#define NOTIFY_STOP             (NOTIFY_OK|NOTIFY_STOP_MASK)

extern int ifm_notifier_register(struct notifier_block *nb);
extern int ifm_notifier_unregister(struct notifier_block *nb);
extern int ifm_call_notifiers(ifindex_t ifindex,unsigned long val, void *v);

#define IF_ALL_TYPEMASK (~0x0UL)

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif  /* end of _IF_NOTIFY_H_ */
