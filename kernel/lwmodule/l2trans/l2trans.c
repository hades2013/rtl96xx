#ifdef  __cplusplus
    extern "C"{
#endif
#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/ethtool.h>
#include <linux/slab.h>
#include <linux/signal.h>
#include <linux/time.h>
#include <linux/spinlock.h>
#include <linux/spinlock_types.h>
//#include <linux/semaphore.h>
#include "lw_type.h"
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
/*lint -save -e155 -e69 -e64 -esym(552,pl2_hk_lock)  -e551*/
 DEFINE_SPINLOCK(pl2_hk_lock);
/*lint -restore*/

static struct list_head pl2_hk;		/* l2 fwd hook list */
/*****************************************************************************
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

void dev_add_l2trans(struct l2_hk_block *pt)
{

	spin_lock_bh(&pl2_hk_lock);
	
    list_add_rcu(&pt->list, &pl2_hk);	
	
	spin_unlock_bh(&pl2_hk_lock);
}
/*****************************************************************************
        Input:  
       Output: 
       Return:  void
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

static void __dev_remove_l2trans(struct l2_hk_block *pt)
{	
	struct l2_hk_block *pt1;

	spin_lock_bh(&pl2_hk_lock);
    
    /*lint -save -e666 -e155 -e413 -e831 -e613*/
	list_for_each_entry(pt1, &pl2_hk, list) {
		if (pt == pt1) {
			list_del_rcu(&pt->list);
			goto out;
		}
	}
    /*lint -restore*/
	printk(KERN_WARNING "__dev_remove_l2fwd: %p not found.\n", pt);
out:
	spin_unlock_bh(&pl2_hk_lock);
}
/*****************************************************************************
        Input:  
       Output: 
       Return:  NET_RX_SUCCESS / L2_RX_STOLEN
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

int l2_trans(struct sk_buff *skb)
{
    struct l2_hk_block *pblock;
    unsigned short type;
    int ret=NET_RX_SUCCESS;
    if(skb==NULL)
    {
        return NET_RX_SUCCESS;
    }
    type=skb->protocol;
      /*lint -save -e666 -e155 -e413 -e831 -e613 -e48 -e10 -e40*/
    list_for_each_entry_rcu(pblock,&pl2_hk,list)
    {         
        if((pblock->type == type||pblock->type==__constant_htons(ETH_P_ALL)) 
            &&(!pblock->dev || pblock->dev == skb->dev))
        {
            atomic_inc(&skb->users);
            ret=pblock->func(skb,skb->dev,pblock);  
            if(NET_RX_L2STOLEN==ret)
            {               
                return ret;
            }
        }        
    }
     /*lint -restore*/
    return ret;
}

/*****************************************************************************
        Input:  
       Output: 
       Return:  NET_RX_SUCCESS / L2_RX_STOLEN
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/


void dev_remove_l2trans(struct l2_hk_block *pt)
{
	__dev_remove_l2trans(pt);
	synchronize_net();
}


static int __init l2_trans_init(void)
{    
    INIT_LIST_HEAD(&pl2_hk);
    return 0;        
}
/****SAMPLE**************************************************/
/*
* 已下为内核转发样例代码，把所有设备收上来的vlan报文发往vlan2
* 内的13,15口
**************************************************************/
#if 0
static int l2_mc_forward_hook(struct sk_buff *skb,struct net_device *dev,struct l2_hk_block *pblock)
{    
    struct ethhdr *eth=NULL;
    struct vlan_ethhdr *veth=NULL;
  //  unsigned char bpdumac[ETH_ALEN] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x03};  
    l2_send_op op;   
    (void)pblock;
    (void)dev;  
    
    if((skb = skb_share_check(skb, GFP_ATOMIC)) == NULL)
    {
        return NET_RX_SUCCESS;
    }
    eth=eth_hdr(skb);     
    if(eth->h_proto==htons(ETH_P_8021Q))
    {     
        veth = (struct vlan_ethhdr *)(skb->data);    
        if(veth->h_vlan_encapsulated_proto!=htons(ETH_P_PAE))
        {
            return NET_RX_SUCCESS;
        }
        op.usVid=0;
        op.ucLogicEnable=L2_LOGIC_DISABLE;
        op.ucPriority=0xff;
        ClrLgcMaskAll(&op.portmask);       
        SetLgcMaskBit(13,&(op.portmask));       
      //  SetLgcMaskBit(15,&(op.portmask));    
        skb_push(skb,ETH_HLEN);       
        veth->h_vlan_TCI = htons(2);    
        k_l2_send_by_port(skb,&op);
        return NET_RX_L2STOLEN;
    }
    kfree_skb(skb);
    return NET_RX_SUCCESS;
}
static struct l2_hk_block mc_fwd =
{   
    .type   = __constant_htons(ETH_P_ALL),
    .dev    = NULL,
    .func    = l2_mc_forward_hook,  
    .list   =LIST_HEAD_INIT(mc_fwd.list),
};
static int __init l2_test_init(void)
{    
    dev_add_l2hook(&mc_fwd);
    return 0;        
}
module_init(l2_test_init);
/****SAMPLE******/
#endif
subsys_initcall(l2_trans_init);


#ifdef  __cplusplus
}
#endif


