/** THE FORMAT is FOR DOXYGEN to Generate the document

  *  @defgroup MINTE minte module
  *  @copyright opulan.com
  *  @author stephanxu@sina.com
  *  @version v0.2
  *  @date June-29-2007
  *  @histroy 
  *  \nauthor   date        description
  *  \n-----------------------------------------------------------------
     \nhxu   June-29-2007   mainly from the old version and rewrite the opulan_read function.\n
     \nhxu   July-05-2007   distill the interrupt handle way from the old ipmux driver module.\n
     \nhxu   July-09-2007   change transfer global g_opl_chip_irq_pending to get_irq_pending() and clear_irq_pending().\n
     
  *  @{
  */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/errno.h>

#include "../include/opl_minte.h"
#include "../include/ipmux_interrupt.h"

/** @name 
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define OPL_MINTE_MAJOR    120
#define OPL_MINTE_DEVICE  "opl_minte"


/** @}*/

/** @name 
 *  @{
 */
/*--------------------------type definition------------------------- */

/** @}*/

/** @name the variable and function imported.
 *  @{
 */
/*-----------------global varible/function declaration--------------- */
extern u32 get_irq_pending(void);
extern void clear_irq_pending(void);
extern void ipmux_irq_enable(unsigned int irq);
extern void ipmux_irq_disable(unsigned int irq);
/** @}*/

/** @name the global variable declaration and definition.
 *  @{
 */
u32 g_opl_chip_irq_event = 0;
wait_queue_head_t g_opl_chip_waitq;

/** @}*/
   
/** @name local function declaration
 *  @{
 */
/*--------------local  function declaration------------- */
static int opl_minte_open(struct inode *inode, struct file *filp);         
static ssize_t opl_minte_read(struct file *filp, char  *buffer, size_t length, loff_t *offset);
static ssize_t opl_minte_write(struct file *filp, const char *buffer, size_t length, loff_t *offset);
static int opl_minte_release(struct inode *inode, struct file *filp);

/** @}*/
 
/** @name local variable declaration and definition
 *  @{
 */
/*--------------local  varible declaration and definition------------- */
static u32 opl_minte_open_count = 0;
static u32 opl_minte_is_open = 0;
struct file_operations opl_minte_fops = {
  .read = opl_minte_read,
  .write = opl_minte_write,
  .open = opl_minte_open,
  .release = opl_minte_release, 
};

#include "ipmux_interrupt.c"
/** @}*/
/** @defgroup minte function_description(opl_minte.c)"
 *  @{
 */
/*------------local  function declaration and definition-------------- */

/** description: copy the opl_chip_irq_pending(indicate which interrupt occured) to user App,
    which will handle its interrupt.if the no irq pending occured,the process will sleep until the isr wake up it.
 *  @param struct file *filp
 *  @param char *buffer
 *  @param size_t length
 *  @param loff_t offset
 *  @return the length of bytes process read.

 *  @see opl_minte_interrupt_handler
 *  @deprecated 
 */

static ssize_t opl_minte_read(struct file *filp, char  *buffer, size_t length, loff_t *offset)
{
  u32 pending = 0;
  if(test_bit(0,&opl_minte_is_open)){
#if 1
    /* TBD: add debug message */
    wait_event_interruptible(g_opl_chip_waitq,g_opl_chip_irq_event);
    if(!test_and_clear_bit(0,&g_opl_chip_irq_event)){
      /* TBD:add debug message ,if return 0,indicate more process read it.*/
    }
#else
    interruptible_sleep_on(&g_opl_chip_waitq);
#endif
    pending = get_irq_pending();
    /* should add lock later */
    clear_irq_pending();
    if(!(copy_to_user((void *)buffer,(void *)&pending,4))){
      /* TBD: add debug message. */
      return 4;
    }
  }
  return -ENODEV;
}
/** description: because the enable irq or disable irq will be in atomic mode,so it should be 
    operated in kernel mode is correct. the value in user *buffer it is the enable or disable.
    1. enable
    0. disable
    if the value is enable, to enble the ipmux interrupt, call the function in malta_int.c
 *  @param 
 *  @param const  char *buffer: contain the value(0-disable or 1-enable).
 *  @param 

 *  @retval 0 is success
 *  @retval !0 failure. the device is not open or copy *buf from user to kernel fails.

 *  @see 
 *  @deprecated 
 */

static ssize_t opl_minte_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
  int irq_enable = 0;
  if(test_bit(0,&opl_minte_is_open)){
    if(!(copy_from_user(&irq_enable,buffer,4))){
      if(irq_enable){
    ipmux_irq_enable(OPL_IPMUX_IRQ);
      }else{
    ipmux_irq_disable(OPL_IPMUX_IRQ);
      }
      return 0;
    }
  }
  return -ENODEV;
}


static int opl_minte_open(struct inode *inode, struct file *filp)         
{
  if (test_and_set_bit(0, &opl_minte_is_open))
    ;//printk(KERN_DEBUG "the/dev/opl_minte is opened\n");
  opl_minte_open_count ++;
  return 0;
}

static int opl_minte_release(struct inode *inode, struct file *filp)
{
  opl_minte_open_count --;
  if(!opl_minte_open_count)
    clear_bit(0, &opl_minte_is_open);
  return 0;
}
/** description: The driver module entry.to register the chrdev and 
    request irq. and some essential initialization.
 *  @param void
 *  @retval 0 success
 *  @retval !0  failure.the module cant be inserted into os.

 *  @see insmod rmmod
 *  @deprecated 
 */
static int __init opl_minte_init(void)
{
  int ret;
  
  ret = register_chrdev(OPL_MINTE_MAJOR,OPL_MINTE_DEVICE,&opl_minte_fops);
  if(ret<0){
    /* TBD add debug message */
    printk("error1\n");
    goto fail1;
  }
  init_waitqueue_head(&g_opl_chip_waitq);
  ret = ipmux_hw0_irqinit();
  if(ret){
    /*TBD add debug message  */
    goto fail2;
  }
  return 0;
 fail2:
  unregister_chrdev(OPL_MINTE_MAJOR,OPL_MINTE_DEVICE);
 fail1:  
  return ret;
}
/** description: it is called when the module removed. do the reverse of
    opl_minte_init(). 
 *  @param void
 *  @return NULL

 *  @see 
 *  @deprecated 
 */
static void __exit opl_minte_exit(void)
{
  int ret;
  ipmux_hw0_irqexit();
  ret = unregister_chrdev(OPL_MINTE_MAJOR,OPL_MINTE_DEVICE);
  if(ret < 0){
    /*TBD:add debug message  */
  }
  return;
}

module_init(opl_minte_init);
module_exit(opl_minte_exit);
#ifdef LINUX_24
MODULE_LICENSE("GPL2");
#else
MODULE_LICENSE("Dual BSD/GPL");
#endif
MODULE_AUTHOR("opulan inc");
MODULE_DESCRIPTION("control interface for opulan ipmux-e switch chip");
/** @}*/
/** @}*/
