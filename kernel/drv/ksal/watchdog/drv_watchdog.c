//#include <linux/config.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/notifier.h>
#include <linux/watchdog.h>
#include <linux/sockios.h>
#include <linux/delay.h>
//#include <linux/overload.h>
#include <linux/net.h>
#include <lw_type.h>
#include <lw_config.h>
#include <lw_drv_pub.h>

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/
 
#if 1
#define FREE_WATCHDOD_COUNTER 50
#else
/*different cpu frequency*/
#endif

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define WDT_PERIOD_DEFAULT 0x8000	/* About 9sec, refer to the CN31XX manuals */

/*this define should reference the watchdog.h */
//#define	WDIOC_GETTIMEOUT2	_IOR(WATCHDOG_IOCTL_BASE, 11, int)
#define	WDIOC_GETTIMEOUT2       _IOR(WATCHDOG_IOCTL_BASE, 11, int)

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/
unsigned int g_uiIfPonStateErr = 0;
unsigned int g_uiIfPonModeOff = 0;
unsigned long long	eth_rcv_packets;

u32 wdt_enabled = 0;
u32 wdt_period = WDT_PERIOD_DEFAULT;

u32 ext_wdt_enabled = 0;
u32 ext_wdt_hw_reboot = 0;

uint32_t watchdog_counter = 0;
uint32_t watchdog_counter_backup = 0;
uint32_t my_timer_counter = 0;

int timer_feed_count = 0;

//#define DEBUG_WATCHDOG

#ifdef DEBUG_WATCHDOG
static unsigned char debug_flag = 0;
#endif

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*
 * ext_wdt_enable:
 */
static __inline__ void ext_wdt_enable(void)
{
    return;
}

/*
 * ext_wdt_write:
 */
static ssize_t ext_wdt_write
    (struct file *file, const char __user *buf,	size_t count, loff_t *ppos)
{
	return 0;
}

/*
 * ext_wdt_ioctl:
 */
static int ext_wdt_ioctl (struct inode *inode, struct file *file,
			    unsigned int cmd, unsigned long arg)
{
	switch(cmd)
    {
        case WDIOC_KEEPALIVE:
           watchdog_counter ++;
           break;	      	       
        case WDIOC_SETOPTIONS:
           if (ext_wdt_enabled == 0) {
               ext_wdt_enabled = 1;
           }
           /*printk (KERN_INFO "Extern Watchdog Enabled.\n");*/
           break;

        case WDIOC_SETTIMEOUT:
           if (ext_wdt_enabled ) {
               ext_wdt_enabled = 0;
           }
           printk (KERN_INFO "Extern Watchdog Disabled.\n");
           break;

        case WDIOC_GETTIMEOUT:    	  
           /*printk (KERN_INFO "Extern Watchdog Hw Reboot CPU.\n");*/
           ext_wdt_hw_reboot = 1;    /*watchdog hw reboot cpu. */
           break;
        case WDIOC_GETTIMEOUT2:    	  
           //printk_exp_head(EXP_TYPE_MP_OVER);
           //printk_exp_info("MP over, Extern Watchdog Hw Reboot CPU.\n");	      	       
           ext_wdt_hw_reboot = 1;    /*watchdog hw reboot cpu. */
           printk("Watchdog Time Out: Rebooting...\n");
           break;
        	       
        default:
           return -EOPNOTSUPP;
    }
	
	return 0;
}

 /* ext_wdt_open:
 */
static int ext_wdt_open (struct inode *inode, struct file *file)
{
	if (wdt_enabled == 0) {
		wdt_enabled = 1;
		ext_wdt_enable();
		/*
		printk (KERN_INFO "Watchdog Timer Enabled (wdt_period=%d)\n",
				wdt_period);
		*/
	}

	return 0;
}
 
static struct file_operations ext_wdt_fops = {
  .owner = THIS_MODULE,
  .llseek = no_llseek,
  .write = ext_wdt_write,
  .ioctl = ext_wdt_ioctl,
  .open = ext_wdt_open,
};

static struct miscdevice ext_wdt_miscdev = {
  .minor = WATCHDOG_MINOR,
  .name = "watchdog",
  .fops = &ext_wdt_fops,
};

static void __exit ext_wdt_exit(void)
{
	misc_deregister(&ext_wdt_miscdev);
}

static int __init drv_ext_wdt_init(void)
{
	int ret = 0;

	printk (KERN_INFO "Extern Watchdog Loaded\n");

	ret = misc_register(&ext_wdt_miscdev);
	if (ret) {
		printk (KERN_CRIT "Cannot register miscdev on minor=%d (err=%d)\n",
				WATCHDOG_MINOR, ret);
		return ret;
	}

    ext_wdt_hw_reboot = 0;
    
	return ret;
}

device_initcall(drv_ext_wdt_init);

/*****************************************************************************
 函 数 名  : hard_feed_watchdog
*****************************************************************************/
inline void hard_feed_watchdog(void)
{
	*((volatile unsigned int *)(0xb8003260)) |= (0x1 << 31);
#ifdef DEBUG_WATCHDOG
	if(debug_flag == 0)
	{
		debug_flag = 1;
		*((volatile unsigned int *)(0xBB0000F0)) |= (0x1 << 22);/*set GPIO 13 Enable*/
		*((volatile unsigned int *)(0xBB0000FC)) |= (0x1 << 22);/*set GPIO 13 Output*/
	}
	else if(debug_flag == 1)
	{
		debug_flag = 2;
		*((volatile unsigned int *)(0xBB0000D8)) |= (0x1 << 22);		
	}
	else if(debug_flag == 2)
	{
		debug_flag = 1;
		*((volatile unsigned int *)(0xBB0000D8)) &= (~(0x1 << 22));
	}
#endif
} 

void timer_feed_watchdog(void)
{
        uint8_t feed_wdg_check = 0;
        static ulong eth_rcv_count_backup = 0;

#define WTD_TIME_OUT (30)
#define MAX_TRANSFER_PACKETS 200 /* 200 */

        if (ext_wdt_enabled == 0)
        {
            my_timer_counter  = 0;
            feed_wdg_check = 1;
        } 
		else if (watchdog_counter != watchdog_counter_backup) 
		{
            //watchdog_counter_backup = watchdog_counter;
            watchdog_counter_backup = 0;
			watchdog_counter = 0;
            eth_rcv_count_backup = eth_rcv_packets;
            my_timer_counter  = 0;
            feed_wdg_check = 1;
       	} 
		else if (my_timer_counter <(WTD_TIME_OUT*(HZ))) 
		{
       
           if(0 == (my_timer_counter%FREE_WATCHDOD_COUNTER))
           {
               feed_wdg_check = 1;
		   //my_timer_counter=0;//add test for feed_wtd
           }
           my_timer_counter++;
       	} 
		else if ( (eth_rcv_packets - eth_rcv_count_backup ) > MAX_TRANSFER_PACKETS ) 
       	{
           my_timer_counter = 0;
           eth_rcv_count_backup = eth_rcv_packets;
           feed_wdg_check = 1;
       	}
		
       	if ((feed_wdg_check == 1) && (ext_wdt_hw_reboot == 0)) 
		{
            timer_feed_count = 0;               
            hard_feed_watchdog();
       	}       
       	else 
		{
            timer_feed_count++;
            if((ext_wdt_hw_reboot == 1)) 
			{
                while(1);//wait for reboot
            }
            else if(timer_feed_count == 300) 
			{
				hard_feed_watchdog();
            }
       } 
       

}
