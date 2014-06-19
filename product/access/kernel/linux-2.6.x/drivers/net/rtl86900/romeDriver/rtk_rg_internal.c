void *rtk_rg_malloc(int NBYTES);
void rtk_rg_free(void *APTR);

#include <rtk_rg_define.h>
#include <rtk_rg_internal.h>

// OS-dependent defination
#ifndef CONFIG_APOLLO_MODEL
#ifdef __linux__
#include <linux/slab.h>
#include <linux/skbuff.h>

void *rtk_rg_malloc(int NBYTES) 
{
	if(NBYTES==0) return NULL;
	return (void *)kmalloc(NBYTES,GFP_KERNEL);
}
void rtk_rg_free(void *APTR) 
{
	kfree(APTR);
}

struct sk_buff *rtk_rg_skbCopyToPreAllocSkb(struct sk_buff *skb)
{
	struct sk_buff *new_skb;
#if RTK_RG_SKB_PREALLOCATE
	if(skb->data[0]&1)
	{
		if(skb->data[0]==0xff)			
			new_skb=re8670_getBcAlloc(SKB_BUF_SIZE);
		else
			new_skb=re8670_getMcAlloc(SKB_BUF_SIZE);		
	}
	else
	{
		new_skb=re8670_getAlloc(SKB_BUF_SIZE);
	}
		
	if(new_skb==NULL) return NULL;
	skb_put(new_skb, (skb->len <= SKB_BUF_SIZE) ? skb->len : SKB_BUF_SIZE);
	memcpy(new_skb->data,skb->data,(skb->len <= SKB_BUF_SIZE) ? skb->len : SKB_BUF_SIZE);
#else
	//bcSkb = dev_alloc_skb(skb->len);
	new_skb=skb_clone(skb,GFP_ATOMIC);		
#endif
	return new_skb;
}


#endif
#else //MODEL CODE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

void *rtk_rg_malloc(int NBYTES)
{
	return malloc(NBYTES);
}
void rtk_rg_free(void *APTR){
	free(APTR);
}

#endif
