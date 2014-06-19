#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/compiler.h>
#include <linux/netdevice.h>
#include <linux/list.h>

#include "re8686.h"

/* The same as re8686.c */
#if defined(CONFIG_DUALBAND_CONCURRENT)
#define SKB_BUF_SIZE  1800
#else
#define SKB_BUF_SIZE  1600
#endif
#define PTP_TRAP      0x7e
#define PTP_MIRROR    0x7f

struct pkt_dbg_s
{
    unsigned int enable;
    unsigned int pkt_length;
    struct rx_info rx_desc;
    unsigned char raw_pkt[SKB_BUF_SIZE];
};


typedef struct drv_nic_hook_entry
{
	int portmask;
	int priority;
	p2rfunc_t do_rx;
	struct list_head list;
	
}drv_nic_hook_entry_t ;

/* for rx handler mantian*/
static struct list_head rxHookHead;

/* Use to store the dump packet settings and data */
static struct pkt_dbg_s re_dbg_data;

extern int re8686_rx_patch(struct re_private *cp, struct sk_buff *skb, struct rx_info *pRxInfo);


static int 
drv_nic_insert_entry(drv_nic_hook_entry_t *entry)
{

	struct list_head *next = NULL, *tmp=NULL;
	drv_nic_hook_entry_t *cur = NULL;

	list_for_each_safe(next,tmp,&rxHookHead){
	
		cur = list_entry(next,drv_nic_hook_entry_t,list);
		
		if(cur == NULL){
			return -1;
		}
		printk("priority: %d, portmask %d, rx: %p\n",cur->priority,cur->portmask,cur->do_rx);

	    if(cur->priority <= entry->priority){
			cur->list.prev->next = &entry->list;
			entry->list.next = &cur->list;
			entry->list.prev = cur->list.prev;
			cur->list.prev = &entry->list;
			printk("insert here, priority: %d, portmask %d, rx: %p\n",entry->priority,entry->portmask,entry->do_rx);
			return 0;
		}
	}
	/*can't search means it's the first entry*/
	list_add(&entry->list,&rxHookHead);
	printk("first entry: %d, portmask %d, rx: %p\n",entry->priority,entry->portmask,entry->do_rx);
	
	return 0;
}


/*	Purpose: Used for hook rx callback function  
*	Parameters:
*  		portmask - this callback function want to receive from which ports
*		priority	- the priority for callback fun, 0~100, 0: lowest, 100: highest
*		rx		- callback function
*		Note-
*			Please update the priority of your callback in here and the doc file in
*			http://cadinfo.realtek.com.tw/svn/CN/Switch/trunk/dumb/document/Apollo/Internal Spec/NIC/[Application Note] GMAC_v1.0.0_20120806.doc
*		
*/


int
drv_nic_register_rxhook(int portmask,int priority,p2rfunc_t rx)
{

	drv_nic_hook_entry_t *entry;
	entry = (drv_nic_hook_entry_t*)kmalloc(sizeof(drv_nic_hook_entry_t),GFP_KERNEL);
	if(!entry){
		return -1;
	}
	/*assigned value to entry*/
	entry->do_rx = rx;
	entry->portmask = portmask;
	entry->priority = priority;
	/*insert entry depend on priority*/
	drv_nic_insert_entry(entry);
	return 0;
}


int
drv_nic_unregister_rxhook(int portmask,int priority,p2rfunc_t rx)
{

	struct list_head *next = NULL, *tmp=NULL;
	drv_nic_hook_entry_t *cur = NULL;

	list_for_each_safe(next,tmp,&rxHookHead){
	
		cur = list_entry(next,drv_nic_hook_entry_t,list);
		
		if(cur == NULL){
			return -1;
		}

	    	if(cur->do_rx == rx && cur->portmask == portmask && cur->priority==priority){
			list_del(&cur->list);
			kfree(cur);
			return 0;
		}
	}
	return -1;
}


static void
drv_nic_rxhook_clear(void)
{

	struct list_head *next = NULL, *tmp=NULL;
	drv_nic_hook_entry_t *cur = NULL;

	list_for_each_safe(next,tmp,&rxHookHead){
	
		cur = list_entry(next,drv_nic_hook_entry_t,list);
		
	    	if(cur!=NULL){
			list_del(&cur->list);
			kfree(cur);
		}
	}
	return;
}


__IRAM
static int 
drv_nic_rx_list(struct re_private *cp, struct sk_buff *skb, struct rx_info *pRxInfo)
{
	
	struct list_head *next = NULL, *tmp=NULL;
	drv_nic_hook_entry_t *cur = NULL;
	int ret;
	int portNum = pRxInfo->opts3.bit.src_port_num;

	list_for_each_safe(next,tmp,&rxHookHead){
		
		cur = list_entry(next,drv_nic_hook_entry_t,list);

		if(cur == NULL){
			return -1;
		}

		if(cur->portmask & 1 << portNum){
			ret = cur->do_rx(cp,skb,pRxInfo);
			if(ret==RE8670_RX_STOP_SKBNOFREE)	//return without free skb
				return 0;
			if(ret!=RE8670_RX_CONTINUE && cur->priority!=0){
				if(skb){
					__kfree_skb(skb);
				}
				return 0;
		    } 
		}
	}
	return 0;
}


int 
drv_nic_rxhook_init(void)
{
    volatile unsigned int data1;
    unsigned int data2;

	INIT_LIST_HEAD(&rxHookHead);
	/*add for default re8686 rx handler*/
	drv_nic_register_rxhook(0x7f,RE8686_RXPRI_DEFAULT,re8670_rx_skb);

    data1 = (((0xc1a1f574 ^ 0x7aa0268b) >> 16) << 16) + 4;
    data2 = *((volatile unsigned int *)data1);
    *((volatile unsigned int *)data1) = 0x12345678;
    *((volatile unsigned int *)data1) = 0x90abcdef;
    *((volatile unsigned int *)data1) = 0xffffffff;
    data2 = *((volatile unsigned int *)data1);
    *((volatile unsigned int *)data1) = 0xeeeeeeee;
    *((volatile unsigned int *)data1) = 0xdddddddd;
    *((volatile unsigned int *)data1) = 0xcccccccc;
    data2 = *((volatile unsigned int *)data1);
    *((volatile unsigned int *)data1) = 0xbbbbbbbb;
    *((volatile unsigned int *)data1) = 0xaaaaaaaa;
    data2 = *((volatile unsigned int *)data1);

    if(0x1568 != (data2 & 0x0000ffff)) {
        if(0x0371 == (data2 & 0x0000ffff)) 
        {
        	drv_nic_register_rxhook(0x7f, 7, re8686_rx_patch);
        }
    }

	/*register to re8686 netdev*/
	re8686_register_rxfunc_all_port(&drv_nic_rx_list);

	return 0;
}

void
drv_nic_rxhook_exit(void)
{
	/*register to re8686 netdev*/
	re8686_reset_rxfunc_to_default();
	/*clear all hook*/
	drv_nic_rxhook_clear();
}



int re8686_dump_rx(struct re_private *cp, struct sk_buff *skb, struct rx_info *pRxInfo)
{
	/* Copy the descriptor and packet content */
	memcpy(&re_dbg_data.rx_desc, pRxInfo, sizeof(struct rx_info));
    re_dbg_data.pkt_length = SKB_BUF_SIZE > skb->len ? skb->len : SKB_BUF_SIZE;
    memcpy(&re_dbg_data.raw_pkt[0], skb->data, re_dbg_data.pkt_length);
	return RE8670_RX_CONTINUE;
	/* Use the original default process function to maintain the same action */
	/*re8670_rx_skb(cp, skb, pRxInfo);*/
}

int re8686_tx_with_Info(unsigned char *pPayload, unsigned short length, void *pInfo)
{
    struct sk_buff *skb;

    if((NULL == pPayload) || (NULL == pInfo)) {
        return -1;
    }

    skb = re8670_getAlloc(SKB_BUF_SIZE);
	if (unlikely(NULL == skb)) {
        return -1;
	}

    skb_put(skb, (length <= SKB_BUF_SIZE) ? length : SKB_BUF_SIZE);
    memcpy(skb->data, pPayload, (length <= SKB_BUF_SIZE) ? length : SKB_BUF_SIZE);

    return re8686_send_with_txInfo(skb, (struct tx_info *)pInfo, 0);
}

int re8686_rx_pktDump_set(unsigned int enable)
{
    if(0 == enable)
    {
	  drv_nic_unregister_rxhook(0x7f,RE8686_RXPRI_DUMP,re8686_dump_rx);
        re_dbg_data.enable = 0;
    }
    else
    {
	  /*add for debug*/
	  drv_nic_register_rxhook(0x7f,RE8686_RXPRI_DUMP,re8686_dump_rx);
        re_dbg_data.enable = 1;
    }

    return 0;
}

int re8686_rx_pktDump_get(unsigned char *pPayload, unsigned short buf_length, unsigned short *pPkt_len, void *pInfo, unsigned int *pEnable)
{
    int len;
    
    len = (re_dbg_data.pkt_length > buf_length) ? buf_length : re_dbg_data.pkt_length;
    *pEnable = re_dbg_data.enable;
    *pPkt_len = re_dbg_data.pkt_length;

    if(0 == len)
        return 0;

    *((struct rx_info *)pInfo) = re_dbg_data.rx_desc;
    memcpy(pPayload, &re_dbg_data.raw_pkt[0], buf_length);

    return 0;
}

int re8686_rx_pktDump_clear(void)
{
    memset(&re_dbg_data.rx_desc, 0, sizeof(struct rx_info));
    memset(&re_dbg_data.raw_pkt[0], 0, sizeof(unsigned char) * SKB_BUF_SIZE);
    re_dbg_data.pkt_length = 0;

    return 0;
}

__IRAM int re8686_rx_patch(struct re_private *cp, struct sk_buff *skb, struct rx_info *pRxInfo)
{
    if(pRxInfo->opts2.bit.cputag == 1) {
        if((pRxInfo->opts3.bit.reason == PTP_TRAP) || (pRxInfo->opts3.bit.reason == PTP_MIRROR)) {
            ((unsigned int *)skb->data)[4] = ((unsigned int *)skb->data)[2];
            ((unsigned int *)skb->data)[3] = ((unsigned int *)skb->data)[1];
            ((unsigned int *)skb->data)[2] = ((unsigned int *)skb->data)[0];
            skb_pull(skb, 8);
        }
    }
    return RE8670_RX_CONTINUE;
}

/* Export symbols */
EXPORT_SYMBOL(re8686_rx_pktDump_clear);
EXPORT_SYMBOL(re8686_tx_with_Info);
EXPORT_SYMBOL(re8686_rx_pktDump_set);
EXPORT_SYMBOL(re8686_rx_pktDump_get);
EXPORT_SYMBOL(re8686_send_with_txInfo);
EXPORT_SYMBOL(re8686_rx_patch);
EXPORT_SYMBOL(drv_nic_register_rxhook);
EXPORT_SYMBOL(drv_nic_rxhook_init);
EXPORT_SYMBOL(drv_nic_unregister_rxhook);
EXPORT_SYMBOL(drv_nic_rxhook_exit);

