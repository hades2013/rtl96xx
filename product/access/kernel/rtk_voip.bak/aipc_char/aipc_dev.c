#ifdef _AIPC_CPU_
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/major.h>
#include <linux/device.h> 
#include <linux/version.h>
#include <linux/kthread.h>
#include <linux/delay.h>


#include <asm/system.h>		/* cli(), *_flags */
#include <asm/uaccess.h>	/* copy_*_user */

#include "./include/dram_share.h"
#include "./include/aipc_dev.h"		/* local definitions */
#include "./include/aipc_reg.h"
#include "./include/soc_type.h"
#include "./include/aipc_mem.h"
#include "./include/aipc_irq.h"
#include "./include/aipc_mbox.h"

#include "gpio.h"

#define DEVICE_NAME                         "aipc_dev"

#define PROC_AIPC_DEV_DIR                   DEVICE_NAME

/* switch */
#define PROC_AIPC_DEV_DIR_SWITCH            "switch"
#define PROC_AIPC_DEV_DBG_PRINT             "dbg_print"

/* status */
#define PROC_AIPC_DEV_DIR_STATUS            "status"
#define PROC_AIPC_DEV_IPC_COUNTERS          "ipc_counters"
#define PROC_AIPC_DEV_THREAD          		"thread"
#define PROC_AIPC_DEV_REGISTER          	"register"
#define PROC_AIPC_DEV_SHM                   "shm"

#ifdef CONFIG_RTL8686_IPC_RECORD_DSP_LOG
/* dsp */
#define PROC_AIPC_DEV_DIR_DSP               "dsp"

/* dsp log*/
#define PROC_AIPC_DEV_DIR_DSP_LOG           "log"
#define PROC_AIPC_DEV_DSP_LOG_ENABLE        "enable"
#define PROC_AIPC_DEV_DSP_LOG_CLEAR         "clear"
#define PROC_AIPC_DEV_DSP_LOG_CONTENTS      "contents"
#define PROC_AIPC_DEV_DSP_LOG_INDEX         "index"
#endif

/* misc */
#define PROC_AIPC_DEV_DIR_MISC              "misc"
#define PROC_AIPC_DEV_OP                    "operations"
#define PROC_AIPC_DEV_PHYADDR               "phymem_addr"
#define PROC_AIPC_DEV_LOGADDR               "logmem_addr"


#define AIPC_BUF_MALLOC
#define AIPC_BUF_SIZE			1*1024

/*
*	Need to be modified as apollo setting
*/
static ul32_t *dst_addr = NULL;		//setup by ioctl
static ul32_t *src_addr = NULL;		//setup by ioctl
static ul32_t length = 0;			//setup by ioctl

#ifdef AIPC_BUF_MALLOC
static ul32_t *aipc_buf = NULL;
#else
static ul32_t  aipc_buf[(AIPC_BUF_SIZE)/(sizeof(ul32_t))] = {0};
#endif

static const size_t ul_size = sizeof(ul32_t);
static struct class *charmodule_class;

static int aipc_dev_major =   AIPC_DEV_MAJOR;
static int aipc_dev_minor =   0;
static int aipc_dev_nr_devs = AIPC_DEV_NR_DEVS;	/* number of bare aipc_dev devices */

module_param(aipc_dev_major, int, S_IRUGO);
module_param(aipc_dev_minor, int, S_IRUGO);
module_param(aipc_dev_nr_devs, int, S_IRUGO);

static struct proc_dir_entry *proc_aipc_dev_dir ;
static aipc_dev_t aipc_dev;
static aipc_ioc_t aipc_ioc;

#ifndef CONFIG_RTL8686_ASIC_TEST_IP
static void aipc_cpu_ipc_init(void);
#endif
extern int  aipc_dsp_boot(void);
extern int  aipc_cpu_irq_init(void);

#ifdef CONFIG_RTL8686_IPC_TEST_THREAD
static struct task_struct *aipc_startup_task;
static int aipc_task_init(void);
#endif
extern int aipc_ctrl_dump(char *buf);
extern int aipc_intq_dump(char *buf);
extern int aipc_mbox_dump(char *buf);

#ifdef CONFIG_RTL8686_IPC_RECORD_DSP_LOG
extern unsigned int *rec_dsp_log_enable;
extern unsigned int *rec_dsp_log_ins;
extern unsigned int *rec_dsp_log_del;
extern unsigned int *rec_dsp_log_touch;
extern const char   *rec_dsp_log_contents;
extern int aipc_record_dsp_log_empty(void);
extern int aipc_record_dsp_log_full(void);
extern unsigned int aipc_record_dsp_log_contents_use(void);
typedef int (*ft_aipc_dsp_log_add)(char);
extern ft_aipc_dsp_log_add   fp_aipc_dsp_log_add;
#endif

extern unsigned int SOC_ID, SOC_BOND_ID;
extern unsigned int GPIO_CTRL_0, GPIO_CTRL_1, GPIO_CTRL_2, GPIO_CTRL_4;

MODULE_AUTHOR("Darren Shiue <darren_shiue@realtek.com>");
MODULE_LICENSE("GPL");


#ifdef AIPC_MODULE_VOIP_SET_PCM_FS
#define REG_I2C0  (0xBB023018)
#define BIT_I2C0  BIT(13)
#define PIN_FS    26
#define PIN_PCM   27

void
aipc_module_voip_set_pcm_fs(void)
{
	if(SOC_ID!=0x0371){
		// disable I2C0
		printk( "REG I2C0 = %x\n" , REG32(REG_I2C0) );
		REG32(REG_I2C0) &= ~(BIT_I2C0);	
		printk( "REG I2C0 = %x\n" , REG32(REG_I2C0) );

		// disalbe GPIO 26 , 27 for FS and PCM respectively
		REG32(GPIO_CTRL_2) &= ~(1<<(PIN_FS  - 1));
		REG32(GPIO_CTRL_2) &= ~(1<<(PIN_PCM - 1));
	}
}
EXPORT_SYMBOL(aipc_module_voip_set_pcm_fs);
#endif

#ifdef CONFIG_RTL8686_SLIC_RESET
#if (CONFIG_RTL8686_SLIC_RESET_GPIO_NUM < 0) || (CONFIG_RTL8686_SLIC_RESET_GPIO_NUM > 71)
	#error "Wrong GPIO Number Setting"
#endif

#define SWITCH_GPIO_CTRL_0	(0xBB000100)
	#define SWITCH_GPIO_DATA_00_31	(SWITCH_GPIO_CTRL_0+0x0)
	#define SWITCH_GPIO_DATA_32_63	(SWITCH_GPIO_CTRL_0+0x4)
	#define SWITCH_GPIO_DATA_64_71	(SWITCH_GPIO_CTRL_0+0x8)

#define SWITCH_GPIO_CTRL_1	(0xBB00010C)

#define SWITCH_GPIO_CTRL_2	(0xBB000118)
	#define SWITCH_GPIO_ENABLE_00_31	(SWITCH_GPIO_CTRL_2+0x0)
	#define SWITCH_GPIO_ENABLE_32_63	(SWITCH_GPIO_CTRL_2+0x4)
	#define SWITCH_GPIO_ENABLE_64_71	(SWITCH_GPIO_CTRL_2+0x8)

#define SWITCH_GPIO_CTRL_3	(0xBB000124)

#define SWITCH_GPIO_CTRL_4	(0xBB0001D4)
	#define SWITCH_GPIO_DIR_00_31		(SWITCH_GPIO_CTRL_4+0x0)
	#define SWITCH_GPIO_DIR_32_63		(SWITCH_GPIO_CTRL_4+0x4)
	#define SWITCH_GPIO_DIR_64_71		(SWITCH_GPIO_CTRL_4+0x8)

static void
aipc_module_voip_slic_reset_0371(void)
{
	int count = 0;
	unsigned int gpio_data = 0, gpio_dir = 0 , gpio_enable = 0;

	printk( "%s(%d)\n" , __FUNCTION__ , __LINE__);

	if ((CONFIG_RTL8686_SLIC_RESET_GPIO_NUM >= 0) && (CONFIG_RTL8686_SLIC_RESET_GPIO_NUM <= 31)){
		count = CONFIG_RTL8686_SLIC_RESET_GPIO_NUM-1;

		gpio_data   = SWITCH_GPIO_DATA_00_31 ;
		gpio_dir    = SWITCH_GPIO_DIR_00_31 ;
		gpio_enable = SWITCH_GPIO_ENABLE_00_31 ;
	}
	else if ((CONFIG_RTL8686_SLIC_RESET_GPIO_NUM >= 32) && (CONFIG_RTL8686_SLIC_RESET_GPIO_NUM <= 63)){
		count = CONFIG_RTL8686_SLIC_RESET_GPIO_NUM % 32;

		gpio_data   = SWITCH_GPIO_DATA_32_63 ;
		gpio_dir    = SWITCH_GPIO_DIR_32_63 ;
		gpio_enable = SWITCH_GPIO_ENABLE_32_63 ;
	}
	else if ((CONFIG_RTL8686_SLIC_RESET_GPIO_NUM >= 64) && (CONFIG_RTL8686_SLIC_RESET_GPIO_NUM <= 71)){
		count = CONFIG_RTL8686_SLIC_RESET_GPIO_NUM % 32;
	
		gpio_data   = SWITCH_GPIO_DATA_64_71 ;
		gpio_dir    = SWITCH_GPIO_DIR_64_71 ;
		gpio_enable = SWITCH_GPIO_ENABLE_64_71 ;
	}
	else{
		printk("Wrong GPIO Number Setting\n");
	}

	printk( "Pull low GPIO %d in switch. count = %d\n" , CONFIG_RTL8686_SLIC_RESET_GPIO_NUM , count);
    printk( "data %x = 0x%x dir %x = 0x%x enable %x = 0x%x\n" , 
    		gpio_data    ,  REG32(gpio_data) , 
			gpio_dir     ,  REG32(gpio_dir) , 
    		gpio_enable  ,  REG32(gpio_enable));

    REG32(gpio_data)	&= ~(1<<(count));  //data
    REG32(gpio_dir)		|=  (1<<(count));  //dir
    REG32(gpio_enable)	|=  (1<<(count));  //enable

	mdelay(RESET_SLIC_DELAY_TIME);

    REG32(gpio_data) 	|=  (1<<(count));
    
	printk( "Pull high GPIO %d in switch\n" , CONFIG_RTL8686_SLIC_RESET_GPIO_NUM );
    printk( "data %x = 0x%x dir %x = 0x%x enable %x = 0x%x\n" , 
    		gpio_data    ,  REG32(gpio_data) , 
			gpio_dir     ,  REG32(gpio_dir) , 
    		gpio_enable  ,  REG32(gpio_enable));
}


static void
aipc_module_voip_slic_reset_others(void)
{
	int slic_reset = CONFIG_RTL8686_SLIC_RESET_GPIO_NUM;
	int value = 0;

	printk( "%s(%d)\n" , __FUNCTION__ , __LINE__);
    printk( "Pull low GPIO %d in switch\n" , CONFIG_RTL8686_SLIC_RESET_GPIO_NUM );
	
	gpioClear( slic_reset );
	gpioConfig( slic_reset, GPIO_FUNC_OUTPUT );
	value = gpioRead( slic_reset );
    printk( "GPIO value = %d\n" , value );
	
	mdelay(RESET_SLIC_DELAY_TIME);
    
    printk( "Pull high GPIO %d in switch\n" , CONFIG_RTL8686_SLIC_RESET_GPIO_NUM );
	gpioSet( slic_reset );
	
	value = gpioRead( slic_reset );
    printk( "GPIO value = %d\n" , value );
}

void
aipc_module_voip_slic_reset(void)
{
	if(SOC_ID==0x0371){
		aipc_module_voip_slic_reset_0371();
	} else {
		aipc_module_voip_slic_reset_others();
	}
}
EXPORT_SYMBOL(aipc_module_voip_slic_reset);
#endif

/*
 * Open and close
 */
int 
aipc_dev_open(struct inode *inode, struct file *filp)
{
	aipc_dev_t *dev; /* device information */

	dev = container_of(inode->i_cdev, aipc_dev_t, cdev);
	filp->private_data = dev; /* for other methods */
	return 0;          /* success */
}

int 
aipc_dev_release(struct inode *inode, struct file *filp)
{
	return 0;
}


ssize_t 
aipc_dev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	aipc_dev_t *dev = filp->private_data; 
	ssize_t retval = 0;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	copy_to_user(buf , dst_addr , count);

    *f_pos += count;
    retval = count;

//	SDEBUG("buf=%p count=%u f_pos=%p *f_pos=%lu\n", 
//			buf , count , f_pos , (ul32_t)*f_pos);

	up(&dev->sem);
	return retval;
}

#if 0		//add no padding byte in the end

ssize_t aipc_dev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	aipc_dev_t *dev = filp->private_data;
	ssize_t retval = -ENOMEM; 

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	SDEBUG("dst_addr=%p buf=%p count=%u f_pos=%p *f_pos=%lu\n", 
			dst_addr , buf , count , f_pos , (ul32_t)*f_pos);

	copy_from_user(dst_addr , buf , count);

    *f_pos += count;
    retval = count;

    /* update the size */
    if (dev->size < *f_pos)
        dev->size = *f_pos;

	SDEBUG("buf=%p count=%u f_pos=%p *f_pos=%lu\n", 
			buf , count , f_pos , (ul32_t)*f_pos);

	up(&dev->sem);
	return retval;
}

#else	//add padding byte in the end

ul32_t *
aipc_word_copy(ul32_t *dst , ul32_t *src , size_t count)
{
    ul32_t * plDst = dst;
    ul32_t const * plSrc = src;
	size_t len = count;

	if (count%ul_size != 0){
		SDEBUG("wrong count indication. count=%u sizeof(ul32_t)=%u\n", count, ul_size);
		return NULL;
	}

    while (len>=ul_size){
    	*plDst++ = *plSrc++;
		len -= ul_size;
	}
		
    return (dst);
}

ul32_t *
aipc_align_copy(ul32_t *dst , ul32_t *src , size_t count)
{
	ul32_t remainder = count%ul_size;
	u8_t   padding = 0;
	
	switch (remainder)
	{
		case 0:
			padding=0;
			break;
		case 1:
			padding=3;
			break;
		case 2:
			padding=2;
			break;
		case 3:
			padding=1;
			break;
		default:
			break;
	}
	return aipc_word_copy(dst , src , count+padding);
}

void
aipc_memcpy_padding( void *dst_addr , void *src_addr , ul32_t length)

{
	ul32_t *dp = dst_addr;			//dst pointer. should be change to final destination
	char   *bp = src_addr;	
	ul32_t wc = 0;
	ul32_t rest = length;
	
	#ifdef AIPC_BUF_MALLOC
	aipc_buf = (ul32_t *)kmalloc(AIPC_BUF_SIZE , GFP_KERNEL);
	if (aipc_buf==NULL){
		SDEBUG("malloc failed\n");
		return;
		}
	#endif
	
	for (wc=0 ; rest>=AIPC_BUF_SIZE ; wc+=AIPC_BUF_SIZE, rest-=AIPC_BUF_SIZE){
		#ifdef AIPC_BUF_MALLOC
		memcpy(aipc_buf  , bp , AIPC_BUF_SIZE);
		aipc_word_copy(dp , aipc_buf  , AIPC_BUF_SIZE);
		#else
		memcpy(&aipc_buf , bp , AIPC_BUF_SIZE);
		aipc_word_copy(dp , (ul32_t*)&aipc_buf , AIPC_BUF_SIZE);
		#endif
		
		bp += AIPC_BUF_SIZE;			//char * pointer
		dp += AIPC_BUF_SIZE/ul_size;	//ul32_t * pointer
	}

	if (rest>0 && rest<AIPC_BUF_SIZE){
		#ifdef AIPC_BUF_MALLOC
		memset(aipc_buf, 0 , AIPC_BUF_SIZE);	//make sure all 0 in buffer before copy
		memcpy(aipc_buf , bp , rest);
		aipc_align_copy(dp , aipc_buf , rest);	//for the 4 byte padding
		#else
		memset(&aipc_buf, 0 , AIPC_BUF_SIZE);	//make sure all 0 in buffer before copy
		memcpy(&aipc_buf , bp , rest);
		aipc_align_copy(dp , (ul32_t*)&aipc_buf , rest);	//for the 4 byte padding
		#endif
		bp += rest;
	}else{
		SDEBUG("rest=%lu\n", rest);
	}

	#ifdef AIPC_BUF_MALLOC
	if (aipc_buf){
		kfree(aipc_buf);
		}

	aipc_buf=NULL;
	#endif
		
	//SDEBUG("dst_addr=%p src_addr=%p length=%lu rest=%lu\n", 
	//		dst_addr , src_addr ,   length , rest);
}


ssize_t 
aipc_dev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	aipc_dev_t *dev = filp->private_data;
	ssize_t retval = -ENOMEM;

	ul32_t *dp = dst_addr;			//dst pointer. should be change to final destination
	const char __user *bp = buf;	//user space pointer
	ul32_t wc = 0;
	ul32_t rest = count;
	ul32_t tmp = 0;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

#ifdef AIPC_BUF_MALLOC
	aipc_buf = (ul32_t *)kmalloc(AIPC_BUF_SIZE , GFP_KERNEL);
	if (aipc_buf==NULL){
		SDEBUG("malloc failed\n");
		return -ERESTARTSYS;
		}
#endif

	if (aipc_ioc.write_file){
		for (wc=0 ; rest>=AIPC_BUF_SIZE ; wc+=AIPC_BUF_SIZE, rest-=AIPC_BUF_SIZE){
			#ifdef AIPC_BUF_MALLOC
			copy_from_user(aipc_buf  , bp , AIPC_BUF_SIZE);
			aipc_word_copy(dp , aipc_buf  , AIPC_BUF_SIZE);
			#else
			copy_from_user(&aipc_buf , bp , AIPC_BUF_SIZE);
			aipc_word_copy(dp , (ul32_t*)&aipc_buf , AIPC_BUF_SIZE);
			#endif
			
			bp += AIPC_BUF_SIZE;			//char * pointer
			dp += AIPC_BUF_SIZE/ul_size;	//ul32_t * pointer
		}

		if (rest>0 && rest<AIPC_BUF_SIZE){
			#ifdef AIPC_BUF_MALLOC
			memset(aipc_buf, 0 , AIPC_BUF_SIZE);	//make sure all 0 in buffer before copy
			copy_from_user(aipc_buf , bp , rest);
			aipc_align_copy(dp , aipc_buf , rest);	//for the 4 byte padding
			#else
			memset(&aipc_buf, 0 , AIPC_BUF_SIZE);	//make sure all 0 in buffer before copy
			copy_from_user(&aipc_buf , bp , rest);
			aipc_align_copy(dp , (ul32_t*)&aipc_buf , rest);	//for the 4 byte padding
			#endif
			bp += rest;
		}else{
			SDEBUG("rest=%lu\n", rest);
		}
	}
	else if (aipc_ioc.write_word){
		copy_from_user(&tmp , bp , ul_size);
		if(dst_addr){
			*dst_addr = tmp; 
			SDEBUG("write word=0x%lx tmp=%lu\n", *dst_addr , tmp);
		}
		else{
			SDEBUG("wrong dst_address\n");
		}
	}
	else if (aipc_ioc.bitop_and){
		copy_from_user(&tmp , bp , ul_size);
		if(dst_addr){
			*dst_addr &= tmp;
			SDEBUG("and result 0x%lx tmp=%lu\n", *dst_addr , tmp);
		}
		else{
			SDEBUG("wrong dst_address\n");
		}
	}
	else if (aipc_ioc.bitop_or){
		copy_from_user(&tmp , bp , ul_size);
		if(dst_addr){
			*dst_addr |= tmp;
			SDEBUG("or result 0x%lx tmp=%lu\n", *dst_addr , tmp);
		}
		else{
			SDEBUG("wrong dst_address\n");
		}
	}
	else if (aipc_ioc.bitop_xor){
		copy_from_user(&tmp , bp , ul_size);
		if(dst_addr){
			*dst_addr ^= tmp;
			SDEBUG("xor result 0x%lx tmp=%lu\n", *dst_addr , tmp);
		}
		else{
			SDEBUG("wrong dst_address\n");
		}
	}
	else if (aipc_ioc.bitop_not){
		copy_from_user(&tmp , bp , ul_size);
		if(dst_addr){
			*dst_addr &= ~tmp;
			SDEBUG("not result 0x%lx tmp=%lu\n", *dst_addr , tmp);
		}
		else{
			SDEBUG("wrong dst_address\n");
		}
	}else{
		SDEBUG("undefined write action\n");
		}
	

	#ifdef AIPC_BUF_MALLOC
	if (aipc_buf){
		kfree(aipc_buf);
		}

	aipc_buf=NULL;
	#endif

    *f_pos += count;
    retval = count;

    /* update the size */
    if (dev->size < *f_pos)
        dev->size = *f_pos;

//	SDEBUG("buf=%p bp=%p count=%u rest=%lu f_pos=%p *f_pos=%lu\n", 
//			buf , bp , count , rest , f_pos , (ul32_t)*f_pos);

	up(&dev->sem);

	return retval;
}

#endif

void
aipc_dev_memcpy( void *dst_addr , void *src_addr , ul32_t length , int add_padding )
{
	if (!add_padding)
		memcpy(dst_addr , src_addr , length);
	else{
		aipc_memcpy_padding(dst_addr , src_addr , length);
	}
}

/*
 * The ioctl() implementation
 */

int 
aipc_dev_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, ul32_t arg)
{
	int retval = 0;
	ul32_t rom_addr = 0;
	ul32_t tmp = 0;

	#ifdef IPC_HW_MUTEX_CCODE
	volatile ul32_t result = 0;
	#endif

	switch(cmd) {
	case IOCTL_DST_ADDR: /* Set: arg points to the value */
		dst_addr	= (ul32_t*)arg;
		#if 0
		SDEBUG("dst_addr=%p __va=%p __pa=%lx v2p=%x\n", 
			dst_addr , 
			__va(dst_addr) ,
			__pa(dst_addr) , 
			Virtual2Physical(dst_addr));
		SDEBUG("sdk macro: p2v=0x%08x v2n=0x%08x p2n=0x%08x\n", 
			Physical2Virtual(dst_addr) , 
			Virtual2NonCache(dst_addr) , 
			Physical2NonCache(dst_addr));
		SDEBUG("page.h macro: CAC_ADDR=0x%p  UNCAC_ADDR=0x%p\n\n\n" ,
			CAC_ADDR(dst_addr),
			UNCAC_ADDR(dst_addr));
		#endif
		break;

	case IOCTL_SRC_ADDR:
		src_addr	= (ul32_t*)arg;
		break;

	case IOCTL_LENGTH:
		length		= (ul32_t)arg;
		break;

	case IOCTL_COPY:
		aipc_dev_memcpy(dst_addr , src_addr , length , 0);
		break;

	case IOCTL_COPY_PADDING:
		aipc_dev_memcpy(dst_addr , src_addr , length , 1);
		break;

	case IOCTL_WF:	/* Set: Write File*/
		memset(&aipc_ioc , 0 , sizeof(aipc_ioc));
		aipc_ioc.write_file = 1;
		break;
	
	case IOCTL_WW:	/* Set: Write Word*/
		memset(&aipc_ioc , 0 , sizeof(aipc_ioc));
		aipc_ioc.write_word = 1;
		break;

	case IOCTL_BITOP_AND:
		memset(&aipc_ioc , 0 , sizeof(aipc_ioc));
		aipc_ioc.bitop_and  = 1;
		break;

	case IOCTL_BITOP_OR:
		memset(&aipc_ioc , 0 , sizeof(aipc_ioc));
		aipc_ioc.bitop_or   = 1;
		break;

	case IOCTL_BITOP_XOR:
		memset(&aipc_ioc , 0 , sizeof(aipc_ioc));
		aipc_ioc.bitop_xor  = 1;
		break;

	case IOCTL_BITOP_NOT:
		memset(&aipc_ioc , 0 , sizeof(aipc_ioc));
		aipc_ioc.bitop_not  = 1;
		break;

	case IOCTL_BOOT_DSP:
#ifndef CONFIG_RTL8686_ASIC_TEST_IP
#ifdef BOOT_DSP_DELAY
		//msleep(BOOT_DSP_DELAY_TIME);
		mdelay(BOOT_DSP_DELAY_TIME);
#endif
		aipc_zone_set(zp_dsp_boot);
		aipc_cpu_ipc_init();
#endif
		aipc_dsp_boot();
		SDEBUG("boot dsp\n");
		break;

	case IOCTL_CPU_DRAM_UNMAP:
#ifdef GPON_RESV
		aipc_cpu_dram_unmap(
			DRAM_SEG_IDX_1, 
			AIPC_CPU_IPC_SRAM_ADDR ,
#ifndef CONFIG_DESC_IN_SRAM
			DRAM_SIZE_32KB, 
#else
			DRAM_SIZE_8KB,
#endif
			DRAM_SEG_ENABLE , 
			0);
#else
		aipc_cpu_dram_unmap(
			DRAM_SEG_IDX_0 , 
			AIPC_CPU_IPC_SRAM_ADDR ,
			DRAM_SIZE_128KB , 
			DRAM_SEG_ENABLE , 
			0);
#endif
		break;
			
	case IOCTL_CPU_SRAM_MAP:
#ifdef GPON_RESV
		aipc_cpu_sram_map(
			SRAM_SEG_IDX_1,
			AIPC_CPU_IPC_SRAM_ADDR , 
#ifndef CONFIG_DESC_IN_SRAM
			SRAM_SIZE_32KB, 
#else
			SRAM_SIZE_8KB,
#endif
			SRAM_SEG_ENABLE , 
			0 , 
			0);
#else
		aipc_cpu_sram_map(
			SRAM_SEG_IDX_0 ,
			AIPC_CPU_IPC_SRAM_ADDR , 
			SRAM_SIZE_128KB , 
			SRAM_SEG_ENABLE , 
			0 , 
			0);
#endif
		break;

	case IOCTL_DSP_DRAM_UNMAP:
#ifdef GPON_RESV
		aipc_dsp_dram_unmap(
			DRAM_SEG_IDX_1, 
			(u32_t)AIPC_DSP_IPC_SRAM_ADDR , 
#ifndef CONFIG_DESC_IN_SRAM
			DRAM_SIZE_32KB, 
#else
			DRAM_SIZE_8KB,
#endif
			DRAM_SEG_ENABLE , 
			DRAM_LX_MATCH_ENABLE); 
#else
		aipc_dsp_dram_unmap(
			DRAM_SEG_IDX_0 , 
			(u32_t)AIPC_DSP_IPC_SRAM_ADDR , 
			DRAM_SIZE_128KB , 
			DRAM_SEG_ENABLE , 
			DRAM_LX_MATCH_ENABLE); 
#endif
		break;
			
	case IOCTL_DSP_SRAM_MAP:
#ifdef GPON_RESV
		aipc_dsp_sram_map(
			SRAM_SEG_IDX_1, 
			(u32_t)AIPC_DSP_IPC_SRAM_ADDR , 
#ifndef CONFIG_DESC_IN_SRAM
			SRAM_SIZE_32KB, 
#else
			SRAM_SIZE_8KB,
#endif
			SRAM_SEG_ENABLE , 
			0 ,
			SRAM_LX_MATCH_ENABLE);
#else
		aipc_dsp_sram_map(
			SRAM_SEG_IDX_0 , 
			(u32_t)AIPC_DSP_IPC_SRAM_ADDR , 
			SRAM_SIZE_128KB , 
			SRAM_SEG_ENABLE , 
			0 ,
			SRAM_LX_MATCH_ENABLE);
#endif
		break;

	case IOCTL_DSP_ENTRY:
		aipc_dsp_entry();
		break;
		
	case IOCTL_ZONE_SET:
		aipc_zone_set(zp_dsp_init);
		break;

	case IOCTL_TRIGGER:
		tmp = (ul32_t)arg;

		if (tmp==T_DSP){		//trigger DSP
			aipc_int_assert( T_DSP );
			SDEBUG("tirgger DSP\n");
		}
		else{					//trigger CPU
			aipc_int_assert( T_CPU );
			SDEBUG("trigger CPU\n");
		}
		break;

	case IOCTL_ROM_SET:
		rom_addr = (ul32_t)arg;
		
		aipc_rom_set(rom_addr);
		SDEBUG("set ROM. rom_addr=0x%lx\n" , rom_addr);

#ifndef GPON_RESV
#ifdef CONFIG_RTL8686_FPGA		//in FPGA phase
		if (rom_addr){
			//Set DSP SRAM Segment 0
			aipc_dsp_sram_map(
				SRAM_SEG_IDX_0 , 
				rom_addr , 
				SRAM_SIZE_128KB , 
				SRAM_SEG_ENABLE , 
				0 , 
				SRAM_LX_MATCH_ENABLE); 

			//Set DSP SRAM Segment 1
			aipc_dsp_sram_map(
				SRAM_SEG_IDX_1 , 
				rom_addr+ROM_BASE_128K , 
				SRAM_SIZE_128KB , 
				SRAM_SEG_ENABLE , 
				ROM_BASE_128K , 
				SRAM_LX_MATCH_ENABLE); 


			SDEBUG("set DSP SRAM map.\n");
			}
		else{
			SDEBUG("Wrong rom code address\n");
			}
#endif
#endif
		break;

	case IOCTL_INIT_IPC:	
		ACTSW.init |= INIT_OK;
		
#ifdef CONFIG_RTL8686_IPC_TEST_THREAD
		SDEBUG("Init IPC related tasks\n");
		aipc_task_init();
#endif
#ifdef CONFIG_RTL8686_READ_DRAM_THREAD
		extern int cpu_read_task_start(void);
		cpu_read_task_start();
#endif
		break;

	case IOCTL_RESET_SLIC:
		#ifdef AIPC_MODULE_VOIP_SET_PCM_FS
		aipc_module_voip_set_pcm_fs();
		#endif	
		#ifdef CONFIG_RTL8686_SLIC_RESET
		aipc_module_voip_slic_reset();
		#endif
		break;

	case IOCTL_CTRL_2DSP_SEND:
		ATHREAD.ctrl_2dsp_send = (u32_t)arg;
		break;

	case IOCTL_CTRL_2DSP_POLL:
		ATHREAD.ctrl_2dsp_poll = (u32_t)arg;
		break;

	case IOCTL_CTRL_2CPU_SEND:
		ATHREAD.ctrl_2cpu_send = (u32_t)arg;
		break;

	case IOCTL_CTRL_2CPU_POLL:
		ATHREAD.ctrl_2cpu_poll = (u32_t)arg;
		break;

	case IOCTL_MBOX_2DSP_SEND: 
		ATHREAD.mbox_2dsp_send = (u32_t)arg;
		break;

	case IOCTL_MBOX_2DSP_POLL:
		ATHREAD.mbox_2dsp_poll = (u32_t)arg;
		break;

	case IOCTL_MBOX_2CPU_SEND:
		ATHREAD.mbox_2cpu_send = (u32_t)arg;
		break;

	case IOCTL_MBOX_2CPU_RECV:
		ATHREAD.mbox_2cpu_recv = (u32_t)arg;
		break;
	
	case IOCTL_DBG_PRINT:
		tmp = (ul32_t)arg;

		if(!tmp){
			ACTSW.dbg_mask = 0;			//set as 0. turn off debug print
			}
		else{
			ACTSW.dbg_mask |= tmp;		//turn on debug print mask.
			}
		break;

	case IOCTL_DBG_DUMP:
		break;

#ifdef IPC_HW_MUTEX_CCODE
	case IOCTL_IPC_MUTEX_LOCK:
		tmp = (ul32_t)arg;
		
		if(tmp == PROCESSOR_ID_CPU){
			aipc_mutex_lock(CPU_LOCK);
		}
		else if(tmp==PROCESSOR_ID_DSP){
			aipc_mutex_lock(DSP_LOCK);
		}
		else{
			SDEBUG("wrong value\n");
		}

		break;

	case IOCTL_IPC_MUTEX_TRYLOCK:
		tmp = (ul32_t)arg;
		
		if(tmp == PROCESSOR_ID_CPU){
			result = aipc_mutex_trylock(CPU_LOCK);
		}
		else if(tmp==PROCESSOR_ID_DSP){
			result = aipc_mutex_trylock(DSP_LOCK);
		}
		else{
			SDEBUG("wrong value\n");
		}
		
		break;
		
	case IOCTL_IPC_MUTEX_UNLOCK:
		tmp = (ul32_t)arg;

		if(tmp==PROCESSOR_ID_CPU){
			aipc_mutex_unlock(CPU_LOCK);
		}
		else if(tmp==PROCESSOR_ID_DSP){
			aipc_mutex_unlock(DSP_LOCK);
		}
		else{
			SDEBUG("wrong value\n");
		}
	
		break;
		
	case IOCTL_IPC_MUTEX_OWN:
		aipc_mutex_own();

		break;
#endif

	default: 
		return -ENOTTY;
	}
	return retval;
}

loff_t 
aipc_dev_llseek(struct file *filp, loff_t off, int whence)
{
	aipc_dev_t *dev = filp->private_data;
	loff_t newpos;

	switch(whence) {
	  case 0: /* SEEK_SET */
		newpos = off;
		break;

	  case 1: /* SEEK_CUR */
		newpos = filp->f_pos + off;
		break;

	  case 2: /* SEEK_END */
		newpos = dev->size + off;
		break;

	  default: /* can't happen */
		return -EINVAL;
	}
	if (newpos < 0) return -EINVAL;
	filp->f_pos = newpos;
	return newpos;
}

struct file_operations aipc_dev_fops = {
	.owner =    THIS_MODULE,
	.llseek =   aipc_dev_llseek,
	.read =     aipc_dev_read,
	.write =    aipc_dev_write,
	.ioctl =    aipc_dev_ioctl,
	.open =     aipc_dev_open,
	.release =  aipc_dev_release,
};



/*
 * Set up the char_dev structure for this device.
 */
static void 
aipc_dev_setup_cdev(aipc_dev_t *dev, int index)
{
	int err, devno = MKDEV(aipc_dev_major, aipc_dev_minor + index);
    
	cdev_init(&dev->cdev, &aipc_dev_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &aipc_dev_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding aipc_dev%d\n", err, index);
}


/*
*	switch related
*/
static int 
proc_switch_dbg_print_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmpbuf[100] = {0};
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);
		
		if (flag == 0) // disable
			ACTSW.dbg_mask = 0;
		
		else if(flag > 0)
			ACTSW.dbg_mask = flag;
		
		else
			SDEBUG("wrong number\n");
	}

	printk("\nDebug mask:\n");
	printk("\tdbg_mask=0x%x\n" , ACTSW.dbg_mask);


	return count;
}

static int 
proc_switch_dbg_print_r (char *buf, char **start, off_t off, int count, int *eof, void *data)

{
    int n = 0;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }


	n += sprintf(buf , "dbg_mask=0x%x\n" , ACTSW.dbg_mask);

    *eof = 1;
	
    return n;
}


/*
*	status related
*/
static int 
proc_status_ipc_counters_w (struct file *file, const char *buffer, unsigned long count, void *data)
{

	char tmpbuf[100] = {0};
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);
		
		if (flag == 0){ // reset counters
			memset(&ASTATS ,  0 , sizeof(aipc_stats_t));
			
			printk("Clear status counters\n");
		}
		else
			SDEBUG("wrong number\n");
	}

	return count;
}

static int
aipc_dbg_thread_dump(char *buf)
{
	int n = 0;

	n += sprintf(buf   , "\nThread counters:\n");
	
	n += sprintf(buf+n , "\tctrl_2dsp_counter = %u\n" ,	ATHREAD.ctrl_2dsp_counter);
	n += sprintf(buf+n , "\tctrl_2cpu_counter = %u\n" ,	ATHREAD.ctrl_2cpu_counter);
		
	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_2
	n += sprintf(buf+n , "\tctrl_2dsp_counter_2 = %u\n" , ATHREAD.ctrl_2dsp_counter_2);
	n += sprintf(buf+n , "\tctrl_2cpu_counter_2 = %u\n" , ATHREAD.ctrl_2cpu_counter_2);
	#endif
	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_3
	n += sprintf(buf+n , "\tctrl_2dsp_counter_3 = %u\n" , ATHREAD.ctrl_2dsp_counter_3);
	n += sprintf(buf+n , "\tctrl_2cpu_counter_3 = %u\n" , ATHREAD.ctrl_2cpu_counter_3);
	#endif

	
	n += sprintf(buf+n , "\tmbox_2dsp_counter = %u\n" ,	ATHREAD.mbox_2dsp_counter);
	n += sprintf(buf+n , "\tmbox_2cpu_counter = %u\n" ,	ATHREAD.mbox_2cpu_counter);
	
	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_2
	n += sprintf(buf+n , "\tmbox_2dsp_counter_2 = %u\n" , ATHREAD.mbox_2dsp_counter_2);
	n += sprintf(buf+n , "\tmbox_2cpu_counter_2 = %u\n" , ATHREAD.mbox_2cpu_counter_2);
	#endif

	#ifdef	CONFIG_RTL8686_IPC_TEST_THREAD_3
	n += sprintf(buf+n , "\tmbox_2dsp_counter_3 = %u\n" , ATHREAD.mbox_2dsp_counter_3);
	n += sprintf(buf+n , "\tmbox_2cpu_counter_3 = %u\n" , ATHREAD.mbox_2cpu_counter_3);
	#endif

	#ifdef CONFIG_RTL8686_READ_DRAM_THREAD
	n += sprintf(buf+n , "\tcpu_read_cnt = %u\n" , ATHREAD.cpu_read_cnt);
	n += sprintf(buf+n , "\tdsp_read_cnt = %u\n" , ATHREAD.dsp_read_cnt);
	n += sprintf(buf+n , "\tall_read_cnt = %u\n" , ATHREAD.all_read_cnt);
	#endif

	return n;
}


static int
aipc_dbg_counter_dump(char *buf)
{
	int n = 0;

	n += sprintf(buf , "Statistics:\n");

    /*    
    *    data plane
    */
	n += sprintf(buf+n , "   Data plane \n");
    //    CPU->DSP
	n += sprintf(buf+n , "\t (CPU) data CPU->DSP: \n");
	n += sprintf(buf+n , "\t aipc_data_2dsp_alloc  = %u    BC_2DSP.cnt_del = %u\n"  , ASTATS.aipc_data_2dsp_alloc , BC_2DSP.cnt_del);
	n += sprintf(buf+n , "\t aipc_data_2dsp_send   = %u    MB_2DSP.cnt_ins = %u\n"  , ASTATS.aipc_data_2dsp_send  , MB_2DSP.cnt_ins);
	n += sprintf(buf+n , "\t (DSP) data CPU->DSP: \n");
	n += sprintf(buf+n , "\t aipc_data_2dsp_recv   = %u    MB_2DSP.cnt_del = %u\n"  , ASTATS.aipc_data_2dsp_recv  , MB_2DSP.cnt_del);
	n += sprintf(buf+n , "\t aipc_data_2dsp_ret    = %u    BC_2DSP.cnt_ins = %u\n"  , ASTATS.aipc_data_2dsp_ret   , BC_2DSP.cnt_ins);

    //    CPU<-DSP
	n += sprintf(buf+n , "\n\t (CPU) data CPU<-DSP: \n");
	n += sprintf(buf+n , "\t aipc_data_2cpu_recv   = %u    MB_2CPU.cnt_del = %u\n"  , ASTATS.aipc_data_2cpu_recv  , MB_2CPU.cnt_del);
	n += sprintf(buf+n , "\t aipc_data_2cpu_ret    = %u    BC_2CPU.cnt_ins = %u\n"  , ASTATS.aipc_data_2cpu_ret   , BC_2CPU.cnt_ins);
	n += sprintf(buf+n , "\t (DSP) data CPU<-DSP: \n");
	n += sprintf(buf+n , "\t aipc_data_2cpu_alloc  = %u    BC_2CPU.cnt_del = %u\n"  , ASTATS.aipc_data_2cpu_alloc , BC_2CPU.cnt_del);
	n += sprintf(buf+n , "\t aipc_data_2cpu_send   = %u    MB_2CPU.cnt_ins = %u\n"  , ASTATS.aipc_data_2cpu_send  , MB_2CPU.cnt_ins);

    /*    
    *    control plane plane
    */
	n += sprintf(buf+n , "\n   Control plane \n");    
    //	Control
	n += sprintf(buf+n , "\t (CPU) control CPU->DSP: \n");
	n += sprintf(buf+n , "\t aipc_ctrl_2dsp_nofbk_alloc  = %u  CMD_2DSP.cnt_del = %u\n"  , 
		ASTATS.aipc_ctrl_2dsp_nofbk_alloc ,      CMD_2DSP.cnt_del);
	n += sprintf(buf+n , "\t aipc_ctrl_2dsp_fbk_alloc    = %u  CMD_LOCAL_2DSP.cnt_del = %u\n"  ,
		ASTATS.aipc_ctrl_2dsp_fbk_alloc   ,      CMD_LOCAL_2DSP.cnt_del);

	#ifdef STATS_RETRY 
	n += sprintf(buf+n , "\t aipc_ctrl_2dsp_nofbk_alloc_retry = %u\n" ,
		ASTATS.aipc_ctrl_2dsp_nofbk_alloc_retry);
	n += sprintf(buf+n , "\t aipc_ctrl_2dsp_fbk_alloc_retry = %u\n" ,
		ASTATS.aipc_ctrl_2dsp_fbk_alloc_retry);
	#endif	
	
	n += sprintf(buf+n , "\t aipc_ctrl_2dsp_send         = %u  CMD_QUEUE_2DSP.cnt_ins = %u\n"  , 
		ASTATS.aipc_ctrl_2dsp_send ,             CMD_QUEUE_2DSP.cnt_ins);
   	n += sprintf(buf+n , "\t aipc_ctrl_2dsp_fbk_ret      = %u  CMD_LOCAL_2DSP.cnt_ins = %u\n"  , 
		ASTATS.aipc_ctrl_2dsp_fbk_ret ,          CMD_LOCAL_2DSP.cnt_ins);
	
    n += sprintf(buf+n , "\t (DSP) control CPU->DSP: \n");
    n += sprintf(buf+n , "\t aipc_ctrl_2dsp_recv         = %u  CMD_QUEUE_2DSP.cnt_del = %u\n"  , 
		ASTATS.aipc_ctrl_2dsp_recv ,             CMD_QUEUE_2DSP.cnt_del);
    n += sprintf(buf+n , "\t aipc_ctrl_2dsp_nofbk_ret    = %u  CMD_2DSP.cnt_ins = %u\n" ,    
		ASTATS.aipc_ctrl_2dsp_nofbk_ret ,        CMD_2DSP.cnt_ins );
    n += sprintf(buf+n , "\t aipc_ctrl_2dsp_fbk_fin      = %u  CBUF_2DSP.done_cnt = %u\n"  , 
		ASTATS.aipc_ctrl_2dsp_fbk_fin ,          CBUF_2DSP.done_cnt);

	//	Event
    n += sprintf(buf+n , "\n\t (CPU) event CPU<-DSP: \n");    
    n += sprintf(buf+n , "\t aipc_ctrl_2cpu_recv   = %u\n"  ,    ASTATS.aipc_ctrl_2cpu_recv);
    n += sprintf(buf+n , "\t aipc_ctrl_2cpu_ret    = %u  CBUF_EQ_2CPU.cnt_del = %u\n"  , 
		ASTATS.aipc_ctrl_2cpu_ret ,              CBUF_EQ_2CPU.cnt_del);
    
    n += sprintf(buf+n , "\t (DSP) event CPU<-DSP: \n");
    n += sprintf(buf+n , "\t aipc_ctrl_2cpu_alloc  = %u\n"  ,    ASTATS.aipc_ctrl_2cpu_alloc);

	#ifdef STATS_RETRY 
    n += sprintf(buf+n , "\t aipc_ctrl_2cpu_alloc_retry  = %u\n"  , ASTATS.aipc_ctrl_2cpu_alloc_retry);
	#endif
	
	n += sprintf(buf+n , "\t aipc_ctrl_2cpu_send   = %u  CBUF_EQ_2CPU.cnt_ins = %u\n"  , 
		ASTATS.aipc_ctrl_2cpu_send ,       CBUF_EQ_2CPU.cnt_ins);

    /*
    *    interrupt counters
    */
	n += sprintf(buf+n , "\n   Interrupt counters \n");
	n += sprintf(buf+n , "\t dsp_t_cpu = %u\n" , ASTATS.dsp_t_cpu);
	n += sprintf(buf+n , "\t INT_2CPU_HIQ.cnt_ins  = %u INT_2CPU_HIQ.cnt_del =%u \n"  , 
		       INT_2CPU_HIQ.cnt_ins  ,    INT_2CPU_HIQ.cnt_del);
	n += sprintf(buf+n , "\t INT_2CPU_LOWQ.cnt_ins = %u INT_2CPU_LOWQ.cnt_del =%u \n" , 
		       INT_2CPU_LOWQ.cnt_ins ,    INT_2CPU_LOWQ.cnt_del);

	n += sprintf(buf+n , "\t cpu_t_dsp = %u\n" , ASTATS.cpu_t_dsp);
	n += sprintf(buf+n , "\t INT_2DSP_HIQ.cnt_ins  = %u INT_2DSP_HIQ.cnt_del =%u \n"  , 
		       INT_2DSP_HIQ.cnt_ins  ,    INT_2DSP_HIQ.cnt_del);
	n += sprintf(buf+n , "\t INT_2DSP_LOWQ.cnt_ins = %u INT_2DSP_LOWQ.cnt_del =%u \n" , 
		       INT_2DSP_LOWQ.cnt_ins ,    INT_2DSP_LOWQ.cnt_del);


    /*
    *    shm notify counters
    */
#ifdef CONFIG_RTL8686_SHM_NOTIFY
	n += sprintf(buf+n , "\n   shm notify counters \n");
	n += sprintf(buf+n , "\t shm_notify_cpu = %u shm_notify_dsp = %u\n" , ASTATS.shm_notify_cpu , ASTATS.shm_notify_dsp);
#endif


	/*
	*	 error case
	*/
	n += sprintf(buf+n , "\n   error counters \n");
	n += sprintf(buf+n , "\t aipc_data_error = %u aipc_ctrl_error = %u\n"  , ASTATS.aipc_data_error,ASTATS.aipc_ctrl_error);

	/*
	*	exception case
	*/
	n += sprintf(buf+n , "\n   exception counters \n");
	n += sprintf(buf+n , "\t aipc_ctrl_2cpu_exception_send = %u\n" ,    ASTATS.aipc_ctrl_2cpu_exception_send);

	return n;
}

static int
aipc_thread_switch_dump(char *buf)
{
	int n = 0;
	
	n += sprintf(buf , "\nControl Switch:\n");

//control plane
	//CPU->DSP. RX direction
	n += sprintf(buf+n , "\tctrl_2dsp_send = 0x%x\n" , ATHREAD.ctrl_2dsp_send);
	n += sprintf(buf+n , "\tctrl_2dsp_poll = 0x%x\n" , ATHREAD.ctrl_2dsp_poll);
		
	//CPU<-DSP. TX direction
	n += sprintf(buf+n , "\tctrl_2cpu_send = 0x%x\n" , ATHREAD.ctrl_2cpu_send);
	n += sprintf(buf+n , "\tctrl_2cpu_poll = 0x%x\n" , ATHREAD.ctrl_2cpu_poll);
			
//data plane
	//CPU->DSP. RX direction
	n += sprintf(buf+n , "\tmbox_2dsp_send = 0x%x\n" , ATHREAD.mbox_2dsp_send);
	n += sprintf(buf+n , "\tmbox_2dsp_poll = 0x%x\n" , ATHREAD.mbox_2dsp_poll);
			
	//CPU<-DSP. TX direction
	n += sprintf(buf+n , "\tmbox_2cpu_send = 0x%x\n" , ATHREAD.mbox_2cpu_send);
	n += sprintf(buf+n , "\tmbox_2cpu_recv = 0x%x\n" , ATHREAD.mbox_2cpu_recv);

	return n;
}

static int 
proc_status_ipc_counters_r (char *buf, char **start, off_t off, int count, int *eof, void *data)

{
	int n = 0;

	switch(off){
	case 0:
		n += aipc_mbox_dump(buf);

		n += aipc_intq_dump(buf+n);

		n += aipc_ctrl_dump(buf+n);

		break;
		
	case 1:
		n += aipc_dbg_counter_dump(buf+n);

		break;
	}

	*start = (char *)1; 

	if (n==0)
	    *eof = 1;
	
	return n;
}

static int 
proc_status_thread_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmpbuf[100] = {0};
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);

		if (flag == 0){
			memset(&ATHREAD , 0 , sizeof(aipc_thread_t));
			
			printk("Clear thread counters\n");
		}
		else
			printk("wrong number\n");
	}

	return count;
}

static int 
proc_status_thread_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }

	n += aipc_thread_switch_dump(buf);
	
	n += aipc_dbg_thread_dump(buf+n);

    *eof = 1;
	
	return n;
}

static int
proc_status_register_w(struct file *file, const char *buffer, unsigned long count, void *data)
{
	return count;
}

static int
proc_status_register_r(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }

	n += sprintf(buf , "\nMemory Registers:\n");
	
	n += sprintf(buf+n , "\tC0DOR0  = 0x%x\n" , REG32(C0DOR0));
	n += sprintf(buf+n , "\tC0DMAR0 = 0x%x\n" , REG32(C0DMAR0));
	n += sprintf(buf+n , "\tC0DOR1  = 0x%x\n" , REG32(C0DOR1));
	n += sprintf(buf+n , "\tC0DMAR1 = 0x%x\n" , REG32(C0DMAR1));
	n += sprintf(buf+n , "\tC0DOR2  = 0x%x\n" , REG32(C0DOR2));
	n += sprintf(buf+n , "\tC0DMAR2 = 0x%x\n" , REG32(C0DMAR2));
	n += sprintf(buf+n , "\tC0RCR   = 0x%x\n" , REG32(C0RCR));
	n += sprintf(buf+n , "\tC0ILAR  = 0x%x\n" , REG32(C0ILAR));

	n += sprintf(buf+n , "\tC1DOR0  = 0x%x\n" , REG32(C1DOR0));
	n += sprintf(buf+n , "\tC1DMAR0 = 0x%x\n" , REG32(C1DMAR0));
	n += sprintf(buf+n , "\tC1DOR1  = 0x%x\n" , REG32(C1DOR1));
	n += sprintf(buf+n , "\tC1DMAR1 = 0x%x\n" , REG32(C1DMAR1));
	n += sprintf(buf+n , "\tC1DOR2  = 0x%x\n" , REG32(C1DOR2));
	n += sprintf(buf+n , "\tC1DMAR2 = 0x%x\n" , REG32(C1DMAR2));
	n += sprintf(buf+n , "\tC1RCR   = 0x%x\n" , REG32(C1RCR));
	n += sprintf(buf+n , "\tC1ILAR  = 0x%x\n" , REG32(C1ILAR));

	*eof = 1;

	return n;
}

static int
proc_status_shm_w(struct file *file, const char *buffer, unsigned long count, void *data)
{
	return count;
}

static int
proc_status_shm_r(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;
	extern const unsigned int *aipc_shm_notify_cpu;
	extern const unsigned int *aipc_shm_notify_dsp;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }

	n += sprintf(buf , "\nShared Memory Status:\n");
	
	n += sprintf(buf+n , "\nShared Memory Data Size:\n");
	n += sprintf(buf+n , "\t aipc_dram_t size  = 0x%x\n" , sizeof(aipc_dram_t));
	n += sprintf(buf+n , "\t aipc_sram_t size  = 0x%x\n" , sizeof(aipc_sram_t));

	n += sprintf(buf+n , "\nShared Memory Notification:\n");
	n += sprintf(buf+n , "\t aipc_shm_notify_cpu  = 0x%p\n" , aipc_shm_notify_cpu);
	n += sprintf(buf+n , "\t aipc_shm_notify_dsp  = 0x%p\n" , aipc_shm_notify_dsp);


	*eof = 1;

	return n;
}

#ifdef CONFIG_RTL8686_IPC_RECORD_DSP_LOG
static int
proc_dsp_log_enable_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmpbuf[100];
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);

		if (flag == 0){
			*rec_dsp_log_enable = RECORD_DSP_LOG_DISABLE;
			printk("Disable record dsp log\n");
		}
		else if (flag == 1){
			*rec_dsp_log_enable = RECORD_DSP_LOG_ENABLE;
			printk("Enable record dsp log\n");			
		}
		else
			printk("wrong number\n");
	}

	return count;
}

static int
proc_dsp_log_enable_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }

	n += sprintf(buf , "record dsp log = %d\n" , *rec_dsp_log_enable);	
	
    *eof = 1;

	return n;
}

static int
proc_dsp_log_clear_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmpbuf[100];
	int flag = 0;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) {

		sscanf(tmpbuf, "%d", &flag);

		if (flag == 0){
			*rec_dsp_log_del   = *rec_dsp_log_ins;
			*rec_dsp_log_touch = 0;
			
			printk("Clear record dsp log\n");
		}
		else
			printk("wrong number\n");
	}
	
	return count;
}

static int
proc_dsp_log_clear_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;

    *eof = 1;

	return n;
}

static int
proc_dsp_log_contents_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	return count;
}

static int
proc_dsp_log_contents_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;
	static unsigned int dsp_log_del = 0;
	static int remain = 0;

	if (*rec_dsp_log_enable != RECORD_DSP_LOG_ENABLE) {
        *eof = 1;
        n += sprintf( buf , "Record DSP log is disabled\n" );
        return n;
    }

	if (aipc_record_dsp_log_empty()) {
        *eof = 1;
        n += sprintf( buf , "Record DSP log is empty\n" );
        return n;
    }
    
    if (off==0){
    	dsp_log_del = *rec_dsp_log_del;
		remain = aipc_record_dsp_log_contents_use();
    }

	while( remain > 0 ){
		buf[n] = rec_dsp_log_contents[ dsp_log_del ];
		n++;
		dsp_log_del = (dsp_log_del+1) % RECORD_DSP_LOG_SIZE;
		remain--;

		if (n >= count)
			break;
	}

	*start = (char *)1;

	if (n==0)
	    *eof = 1;	

	//printk("count = %d remain = %u n = %d\n" , count , remain , n);

	return n;
}

static int
proc_dsp_log_index_w (struct file *file, const char *buffer, unsigned long count, void *data)
{
	return count;
}

static int
proc_dsp_log_index_r (char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int n = 0;
	unsigned int use  = 0;
	int full  = 0 ;
	int empty = 0;

    if( off ) { /* In our case, we write out all data at once. */
        *eof = 1;
        return 0;
    }
	
	use   = aipc_record_dsp_log_contents_use();
	full  = aipc_record_dsp_log_full();
	empty = aipc_record_dsp_log_empty();
	
	n += sprintf(buf , "\nIndex:\n");
	
	n += sprintf(buf+n , "\t enable      = %u\n"       , *rec_dsp_log_enable);
	n += sprintf(buf+n , "\t touch       = %u\n"       , *rec_dsp_log_touch);
	n += sprintf(buf+n , "\t ins         = %u\n"       , *rec_dsp_log_ins);
	n += sprintf(buf+n , "\t del         = %u\n"       , *rec_dsp_log_del);
	n += sprintf(buf+n , "\t use         = %u\n"       ,  use);
	
	if (full) {
		n += sprintf(buf+n , "\t full        = 1\n");
	}
	else if (empty) {
		n += sprintf(buf+n , "\t empty       = 1\n");
	}
	else if (use) {
		n += sprintf(buf+n , "\t middle      = 1\n");
	}
	else {
		n += sprintf(buf+n , "\t unexpected queue status \n");
	}
	
	n += sprintf(buf+n , "\t conts addr  = %p\n"       ,  rec_dsp_log_contents);
	n += sprintf(buf+n , "\t log size    = %u\n"       ,  RECORD_DSP_LOG_SIZE);

	*eof = 1;
	return n;
}

#endif

#if 0
static int 
proc_read_phymem_addr(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	return sprintf(buf, "%08lx\n", __pa(dst_addr));
}

static int 
proc_read_logmem_addr(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	return sprintf(buf, "%08lx\n", (ul32_t)dst_addr);
}

static struct file_operations aipc_dev_proc_ops = {
    .owner   = THIS_MODULE,
    .open    = NULL,
    .read    = NULL,
	.write	 = NULL,
    .llseek  = NULL,
    .release = NULL
};
#endif

static void 
aipc_dev_create_proc(void)
{
	struct proc_dir_entry *entry;
	
	/*
	*	create root directory
	*/
	proc_aipc_dev_dir = proc_mkdir(PROC_AIPC_DEV_DIR, NULL);
	if (proc_aipc_dev_dir == NULL) {
		printk("create proc root failed!\n");
		return;
	}

	/*
	*	create switch directory
	*/
	proc_aipc_dev_dir = proc_mkdir(PROC_AIPC_DEV_DIR "/" PROC_AIPC_DEV_DIR_SWITCH , NULL);
	if (proc_aipc_dev_dir == NULL) {
		printk("create proc aipc_dev/switch failed!\n");
		return;
	}

	if((entry = create_proc_entry(PROC_AIPC_DEV_DBG_PRINT, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/switch/dbg_print failed!\n");
		return;
	}
	entry->write_proc = proc_switch_dbg_print_w;
	entry->read_proc  = proc_switch_dbg_print_r;


	/*
	*	create status directory
	*/
	proc_aipc_dev_dir = proc_mkdir(PROC_AIPC_DEV_DIR "/" PROC_AIPC_DEV_DIR_STATUS, NULL);
	if (proc_aipc_dev_dir == NULL) {
		printk("create proc aipc_dev/status failed!\n");
		return;
	}

	if((entry = create_proc_entry(PROC_AIPC_DEV_IPC_COUNTERS, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/status/ipc_counters failed!\n");
		return;
	}
	entry->write_proc = proc_status_ipc_counters_w;
	entry->read_proc  = proc_status_ipc_counters_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_THREAD, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/status/thread failed!\n");
		return;
	}
	entry->write_proc = proc_status_thread_w;
	entry->read_proc  = proc_status_thread_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_REGISTER, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/status/register failed!\n");
		return;
	}
	entry->write_proc = proc_status_register_w;
	entry->read_proc  = proc_status_register_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_SHM, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/status/shm failed!\n");
		return;
	}
	entry->write_proc = proc_status_shm_w;
	entry->read_proc  = proc_status_shm_r;

#ifdef CONFIG_RTL8686_IPC_RECORD_DSP_LOG
	/*
	*	create dsp/log directory
	*/
	proc_aipc_dev_dir = proc_mkdir(PROC_AIPC_DEV_DIR "/" PROC_AIPC_DEV_DIR_DSP , NULL);
	if (proc_aipc_dev_dir == NULL) {
		printk("create proc aipc_dev/dsp failed!\n");
		return;
	}
	
	proc_aipc_dev_dir = proc_mkdir(PROC_AIPC_DEV_DIR "/" PROC_AIPC_DEV_DIR_DSP "/" PROC_AIPC_DEV_DIR_DSP_LOG , NULL);
	if (proc_aipc_dev_dir == NULL) {
		printk("create proc aipc_dev/dsp/log failed!\n");
		return;
	}
	
	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_LOG_ENABLE, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/log/enable failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_log_enable_w;
	entry->read_proc  = proc_dsp_log_enable_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_LOG_CLEAR, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/log/clear failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_log_clear_w;
	entry->read_proc  = proc_dsp_log_clear_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_LOG_CONTENTS, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/log/contents failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_log_contents_w;
	entry->read_proc  = proc_dsp_log_contents_r;

	if((entry = create_proc_entry(PROC_AIPC_DEV_DSP_LOG_INDEX, 0644, proc_aipc_dev_dir)) == NULL) {
		printk("create proc aipc_dev/dsp/log/index failed!\n");
		return;
	}
	entry->write_proc = proc_dsp_log_index_w;
	entry->read_proc  = proc_dsp_log_index_r;
#endif

	/*
	*	create misc directory
	*/
#if 0
	proc_aipc_dev_dir = proc_mkdir(PROC_AIPC_DEV_DIR "/" PROC_AIPC_DEV_DIR_MISC , NULL);
	if (proc_aipc_dev_dir == NULL) {
		printk("create proc aipc_dev/misc failed!\n");
		return;
	}

	if((entry = create_proc_entry(PROC_AIPC_DEV_OP, 0644, proc_aipc_dev_dir)) == NULL){
		printk("create proc aipc_dev/misc/operations failed!\n");
		return;
	}
	entry->proc_fops = &aipc_dev_proc_ops;

	create_proc_read_entry( PROC_AIPC_DEV_PHYADDR , 0644 , 
		proc_aipc_dev_dir , proc_read_phymem_addr , NULL );
	create_proc_read_entry( PROC_AIPC_DEV_LOGADDR , 0644 , 
		proc_aipc_dev_dir , proc_read_logmem_addr , NULL );
#endif
}

void 
aipc_dev_cleanup_module(void)
{
	dev_t devno = MKDEV(aipc_dev_major, aipc_dev_minor);
	
	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, aipc_dev_nr_devs);

	device_destroy(charmodule_class, MKDEV(aipc_dev_major, aipc_dev_minor));
	class_destroy(charmodule_class);
}

#ifndef CONFIG_RTL8686_ASIC_TEST_IP
static void
aipc_ipc_clear(void)
{
	memset(ADRAM, 0 , sizeof(aipc_dram_t));
	memset(ASRAM, 0 , sizeof(aipc_sram_t));
}

#if	defined(REFINE_SHM_NOTIFY) && defined(CONFIG_RTL8686_SHM_NOTIFY)
static void
aipc_shm_notify_init(void)
{
	extern const unsigned int *aipc_shm_notify_cpu;
	extern const unsigned int *aipc_shm_notify_dsp;

	REG32(aipc_shm_notify_cpu) = 0;
	REG32(aipc_shm_notify_dsp) = 0;
}
#elif !defined(REFINE_SHM_NOTIFY) && defined(CONFIG_RTL8686_SHM_NOTIFY)
static void
aipc_shm_notify_init(void)
{
	REG32(AIPC_SHM_NOTIFY_CPU) = 0;
	REG32(AIPC_SHM_NOTIFY_DSP) = 0;
}
#endif



static void 
aipc_cpu_ipc_init(void)
{
	extern void aipc_mb_bc_init(void);
	extern void aipc_intq_init(void);
	extern void aipc_ctrl_event_init(void);
#ifdef AIPC_BOOT
	extern int  aipc_boot_init(void);
#endif

	//clear ipc related data sturctures
	aipc_ipc_clear();
	
	//data plane init
	aipc_mb_bc_init();	//init mbox and bc
	aipc_intq_init();	//init int queue

#ifdef AIPC_BOOT
	aipc_boot_init();
#endif

	//control plane init
	aipc_ctrl_event_init();

#ifdef CONFIG_RTL8686_SHM_NOTIFY
	aipc_shm_notify_init();
#endif
	
	//ACTSW.init |= INIT_OK;
}
#endif

#ifdef CONFIG_RTL8686_IPC_TEST_THREAD
#define STARTUP_THREAD "IpcStartD"

static int 
aipc_startup_thread(void *p)
{
	extern int	mbox_2dsp_task_init(void);
	extern int	cpu_ctrl_task_init(void);

	static int init_done  = 0;

	SDEBUG("%s start\n" , STARTUP_THREAD);
	
	set_current_state(TASK_INTERRUPTIBLE);
	do {					
		msleep_interruptible(CPU_CHECK_START_PERIOD);
		//ADEBUG(DBG_SYSTEM, "%s executing ACTSW.init=%x addr=%p\n" , 
		//STARTUP_THREAD , ACTSW.init , &(ACTSW));

		if(!init_done){
			if((ACTSW.init & INIT_OK)){
				//control & data test thread init
				cpu_ctrl_task_init();
				mbox_2dsp_task_init();
				init_done = 1;
				break;
				}
			}
	} while (!kthread_should_stop());

	ADEBUG(DBG_SYSTEM, "%s exit\n" , STARTUP_THREAD);
	kthread_stop(aipc_startup_task);
	
	return OK;
}

static int
aipc_task_init(void)
{
	aipc_startup_task = kthread_run(
					aipc_startup_thread,
					NULL, 
					STARTUP_THREAD);

	if (!IS_ERR(aipc_startup_task)){
		printk("%s create successfully!\n" , STARTUP_THREAD);
		return OK;
	}

	printk("%s create failed!\n" , STARTUP_THREAD);
	return NOK;
}
#endif


#ifdef REBOOT_THREAD
#define REBOOT_THREAD_NAME "RebootD"
#define REBOOT_WAIT_TIME (20*HZ)

static struct task_struct *aipc_reboot_task;

static int 
aipc_reboot_thread(void *p)
{
	volatile unsigned int *dsp_reg;
	SDEBUG("%s start\n" , REBOOT_THREAD_NAME);
	
	set_current_state(TASK_INTERRUPTIBLE);
	do {
		SDEBUG("Schedule out ...\n");
		schedule_timeout_interruptible(REBOOT_WAIT_TIME);
		SDEBUG("Schedule back ...\n");		
		
		SDEBUG("Reset CPU\n");
		dsp_reg    =  (volatile unsigned int *)(R_AIPC_ASIC_RESET_CPU);
		*dsp_reg  |=  BIT_RESET_CPU;
		/* Use WDT to do full chip reset */
		dsp_reg    =  (volatile unsigned int *)(0xb8003268);
		*dsp_reg  |=  0x80000000;
		
	} while (!kthread_should_stop());
	kthread_stop(aipc_reboot_task);
	
	return OK;
}

static int
aipc_reboot_task_init(void)
{
	aipc_reboot_task = kthread_run(
					aipc_reboot_thread,
					NULL, 
					REBOOT_THREAD_NAME);

	if (!IS_ERR(aipc_reboot_task)){
		printk("%s create successfully!\n" , REBOOT_THREAD_NAME);
		return OK;
	}

	printk("%s create failed!\n" , REBOOT_THREAD_NAME);
	return NOK;
}
#endif

#ifdef AIPC_MODULE_INIT_ZONE_ENTRY
static void
aipc_module_init_zone_entry(void)
{
	aipc_zone_set(zp_dsp_init);
	aipc_dsp_entry();
	aipc_rom_set(DSP_ROMCODE_ADDR);
}
#endif

#ifdef AIPC_MODULE_UART1_PINMUX
#define UART1_PINMUX 0xbb02301c
static void
aipc_module_uart1_pinmux(void)
{
	REG32(UART1_PINMUX) = 0;
	SDEBUG("UART1_PINMUX = 0x%08x\n" , REG32(UART1_PINMUX));
}

#endif

#ifdef  AIPC_MODULE_DISALBE_WDOG
#define WDOG_ENABLE 0xb8003224
static void
aipc_module_disable_wdog(void)
{
	REG32(WDOG_ENABLE) &= ~WATCHDOG_ENABLE;
	SDEBUG("WDOG Setting = 0x%08x\n" , REG32(WDOG_ENABLE));
}
#endif

#ifdef AIPC_MODULE_VOIP_IP_ENABLE
static void
aipc_module_voip_ip_enable(void)
{
	REG32(R_AIPC_IP_ENABLE_CTRL) |= BIT_IP_ENABLE_VOIPFFT | \
									BIT_IP_ENABLE_VOIPACC | \
									BIT_IP_ENABLE_GDMA1 | \
									BIT_IP_ENABLE_GDMA0 | \
									BIT_IP_ENABLE_PCM | \
									BIT_IP_ENABLE_GMAC | \
									BIT_IP_ENABLE_PREI_VOIP;

	mdelay(100);
}
#endif


static void
aipc_module_pre_init(void)
{
#ifdef AIPC_MODULE_VOIP_IP_ENABLE
	aipc_module_voip_ip_enable();
#endif

#ifdef AIPC_MODULE_VOIP_SET_PCM_FS
	aipc_module_voip_set_pcm_fs();
#endif

#ifdef CONFIG_RTL8686_SLIC_RESET
	aipc_module_voip_slic_reset();
#endif

#ifdef AIPC_MODULE_UART1_PINMUX
	aipc_module_uart1_pinmux();
#endif

#ifdef AIPC_MODULE_DISALBE_WDOG
    aipc_module_disable_wdog();
#endif

#ifdef  AIPC_MODULE_INIT_ZONE_ENTRY
    aipc_module_init_zone_entry();
#endif
}

int 
aipc_dev_init_module(void)
{
	int result;
	dev_t dev = 0;
	struct device *dp;

	aipc_module_pre_init();

	memset(&aipc_dev , 0 , sizeof(aipc_dev));
	memset(&aipc_ioc , 0 , sizeof(aipc_ioc));

	#if 1
	result = alloc_chrdev_region(&dev, aipc_dev_minor, aipc_dev_nr_devs, DEVICE_NAME);
	aipc_dev_major = MAJOR(dev);
	aipc_dev_minor = MINOR(dev);
	#else
	if (aipc_dev_major) {
		dev = MKDEV(aipc_dev_major, aipc_dev_minor);
		result = register_chrdev_region(dev, aipc_dev_nr_devs, DEVICE_NAME);
	} else {
		result = alloc_chrdev_region(&dev, aipc_dev_minor, aipc_dev_nr_devs, DEVICE_NAME);
		aipc_dev_major = MAJOR(dev);
	}
	#endif

	printk("aipc: register chrdev(%d,%d)\n", aipc_dev_major, aipc_dev_minor);	

	if (result) {
		printk("aipc: can't get major %d\n", aipc_dev_major);
		goto fail;
	}

	charmodule_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(charmodule_class))
		return -EFAULT;

	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30))
	dp = device_create(charmodule_class, NULL , MKDEV(aipc_dev_major, aipc_dev_minor) , NULL , DEVICE_NAME);
	#else
	dp = device_create(charmodule_class, NULL , MKDEV(aipc_dev_major, aipc_dev_minor) , DEVICE_NAME);
	#endif

	if (IS_ERR(dp))
		printk( "aipc: create device failed\n" );
	else
		printk( "aipc: create device successed\n" );

	aipc_dev_create_proc();
	
	init_MUTEX(&aipc_dev.sem);
	
	aipc_dev_setup_cdev(&aipc_dev, 0);

	aipc_cpu_irq_init();

	printk( "aipc: init done %s %d\n" , __FUNCTION__ , __LINE__ );

	#ifdef REBOOT_THREAD
	aipc_reboot_task_init();
	#endif

	return 0; /* succeed */

fail:
	aipc_dev_cleanup_module();
	return result;
}

module_init(aipc_dev_init_module);
module_exit(aipc_dev_cleanup_module);

#endif

