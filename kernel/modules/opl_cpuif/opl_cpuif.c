/** THE FORMAT is FOR DOXYGEN to Generate the document automatically

  *  @defgroup CPUIF cpuif module
  *  @copyright opulan.com
  *  @author stephanxu@sina.com
  *  @version v0.1
  *  @date July-30-2007
  *  @histroy 
  *  \nauthor      date       description
  *  \n-------------------------------------------------------------------
     \nhxu    July-30-2007  initial version for cpuif module.begin data July-15-2007.
  *  @{
  */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/errno.h>


#include "../include/ipmux_interrupt.h"
#include "../include/opl_cpuif.h"
/** @name Macro definition
 *  @{
 */
/*--------------------------Macro definition------------------------- */

#define OPL_DMA0_ETH //add bu zgd for eth1

#define OPL_HOST_MAJOR  130
#define OPL_HOST_NAME   "opl_cpuif"

#define IPMUX_DMA_NUMS  9
#define OPL_DMA_VAL_MAX 2048

#undef OPL_DMA_DEBUG

#ifdef OPL_DMA_DEBUG
#define enter()  printk("enter %s:%s:%d\n",__FILE__,__func__,__LINE__)
#define exit()   printk("enter %s:%s:%d\n",__FILE__,__func__,__LINE__)
#define dbg(fmt...) printk(fmt)
#else
#define enter()
#define exit()
#define dbg(fmt...)
#endif
/** @}*/

/** @name type definition
 *  @{
 */
/*--------------------------type definition------------------------- */
typedef struct dma_buf_attr_s{
  int allocated;
  int count;
  int size;
  int order;
  u32 phys_addr;  
  u32 phys_addr_uncached;
}dma_buf_attr_t;



/** @}*/

/** @name global variable and function imported
 *  @{
 */
/*-----------------global varible/function declaration--------------- */
extern void ipmux_irq_enable(unsigned int irq);
extern void ipmux_irq_disable(unsigned int irq);

/** @}*/
   
/** @name local function declaration
 *  @{
 */
/*-------------------local  function declaration---------------------- */





/** @}*/
 

/** @name local variable declaration and definition
 *  @{
 */
/*--------------local  variable declaration and definition------------- */
static int opl_host_open(struct inode *inode, struct file *filp);         
static ssize_t opl_host_read(struct file *filp, char  *buffer, size_t length, loff_t *offset);
static ssize_t opl_host_write(struct file *filp, const char *buffer, size_t length, loff_t *offset);
static int opl_host_release(struct inode *inode, struct file *filp);
static int opl_host_ioctl(struct inode *inode,struct file *filp,unsigned int cmd,u32 arg);
#ifndef OPL_DMA0_ETH
#ifdef LINUX_24
static void host_dma0_isr(int irq,void *dev_id,struct pt_regs *regs);
#else
static irqreturn_t  host_dma0_isr(int irq,void *dev_id,struct pt_regs *regs);
#endif
#endif
static dma_buf_attr_t tx_attr[IPMUX_DMA_NUMS];
static dma_buf_attr_t rx_attr[IPMUX_DMA_NUMS];

static u32 opl_host_is_open = 0;
static u32 opl_host_open_count = 0;
#ifndef OPL_DMA0_ETH
static u32 opl_host_dma0_irq_event = 0;
static wait_queue_head_t opl_host_dma0_waitq;
#else
extern u32 opl_host_dma0_irq_event;
extern wait_queue_head_t opl_host_dma0_waitq;
#endif
struct file_operations opl_host_fops = {
  .owner = THIS_MODULE,
  .open = opl_host_open,
  .read = opl_host_read,
  .write = opl_host_write,
  .release = opl_host_release,
  .ioctl = opl_host_ioctl,
};


/** @}*/
 

/** @name global variable and function exported
 *  @{
 */
/*------------------global variable and function exported-------------- */


/** @}*/


/** @defgroup cpuif function-description(opl_cpuif.c)

 *  @{
 */
/*----------------------local  function definition--------------------- */
void malloc_for_dma(int is_rx,int dmas,int size)
{
  int count,order;
  char *buf = NULL;
  enter();
  for (count = PAGE_SIZE, order = 0; count < size; order++, count <<= 1)
    ;
  buf = (char*) __get_free_pages(GFP_KERNEL| GFP_DMA, order);
  dbg("the buffer address is 0x%x\n",buf);
  if(buf){
    memset(buf,0,PAGE_SIZE<<order);
    if(is_rx){
      rx_attr[dmas].allocated = 1;
      rx_attr[dmas].size = size;
      rx_attr[dmas].order = order;
      rx_attr[dmas].phys_addr = (u32)buf;
      rx_attr[dmas].phys_addr_uncached = rx_attr[dmas].phys_addr | 0xa0000000;
      dbg("the uncached address is 0x%x\n",rx_attr[dmas].phys_addr_uncached);    
    }else{
      tx_attr[dmas].allocated = 1;
      tx_attr[dmas].size = size;
      tx_attr[dmas].order = order;
      tx_attr[dmas].phys_addr = (u32)buf;
      tx_attr[dmas].phys_addr_uncached = tx_attr[dmas].phys_addr | 0xa0000000;
      dbg("the uncached address is 0x%x\n",tx_attr[dmas].phys_addr_uncached);
    }
  }else{
    printk("allocate memory for dma%d failed",dmas);
  }
  exit();
}

void free_for_dma(int is_rx,int dmas)
{
  enter();
  if(is_rx && rx_attr[dmas].phys_addr && rx_attr[dmas].allocated && (rx_attr[dmas].count<=0)){
    free_pages(rx_attr[dmas].phys_addr,rx_attr[dmas].order);
    memset(&rx_attr[dmas],0,sizeof(dma_buf_attr_t));
  }
  if(!is_rx && tx_attr[dmas].phys_addr && tx_attr[dmas].allocated && (rx_attr[dmas].count<=0)){
    free_pages(tx_attr[dmas].phys_addr,tx_attr[dmas].order);
    memset(&tx_attr[dmas],0,sizeof(dma_buf_attr_t));
  }
  exit();
}

/** description: the host dma0 interrupt handler, when the interrupt generated,
    it will wake up the process,which is waiting for the host dma0 interrupt.
 *  @param int irq:
 *  @param void *dev_id:
 *  @param struct pt_regs *regs:
 *  @return NULL

 *  @see 
 *  @deprecated 
 */
#ifndef OPL_DMA0_ETH
#ifdef LINUX_24
static void host_dma0_isr(int irq,void *dev_id,struct pt_regs *regs)
#else
static irqreturn_t  host_dma0_isr(int irq,void *dev_id,struct pt_regs *regs)
#endif
{
  if(test_and_set_bit(0,&opl_host_dma0_irq_event)){
    /* TBD: add some debug message here */
  }
  wake_up_interruptible(&opl_host_dma0_waitq);
#ifndef LINUX_24
  return IRQ_HANDLED;
#endif
}
#endif


static int opl_host_open(struct inode *inode,struct file *filp)
{
  if(test_and_set_bit(0,&opl_host_is_open)){
    dbg("the device is opened\n"); 
  }
  opl_host_open_count ++;
  return 0;
}

static int opl_host_release(struct inode *inode,struct file *filp)
{
  opl_host_open_count --;
  if(!opl_host_open_count)
    clear_bit(0,&opl_host_is_open);
  return 0;
}

static ssize_t opl_host_read(struct file *filp, char  *buffer, size_t length, loff_t *offset)
{
  return 0;
}

static ssize_t opl_host_write(struct file *filp,const char *buf,size_t length,loff_t *offset)
{
  return 0;
}

/** description: it is used for api to do something associated with ioctl command.
 *  @param struct inode *inode:
 *  @param struct file *filp:
 *  @param unsigned int cmd:
         $:GET_IPMUX_DMA_RX_PHYS_ADDR:return the dma buffer physical address to user(set in dma's base address register)
     $:GET_IPMUX_DMA_TX_PHYS_ADDR:now,it is not used.all merge in rx buffer. 
     $:GET_IPMUX_DMA_RX_BUF_DATA: receive the data to user,by dma_request_data_t struct.
     $:SET_IPMUX_DMA_TX_BUF_DATA: send the data to tx buffer, by dma_request_data_t struct.
     $:ENABLE_IPMUX_HOST_DMA0_INTERRUPT: enable the dma0 interrupt.
     $:DISABLE_IPMUX_HOST_DMA0_INTERRUPT: disable the dma0 interrupt.
     $:WAIT_FOR_IPMUX_HOST_DMA0_INTERRUPT: wait for dma0 interrupt generated.
 *  @param u32 arg:
         $for PHY_ADDR: it is dma_request_phys_addr_t
     $for BUF_DATA: it is dma_request_data_t
     $for INTERRUPT: NULL
 *  @retval 0 is success
 *  @retval !0 is fail

 *  @see 
 *  @deprecated 
 */
extern int get_ipmux_dma0_rx_buf_data(char *pbuf, int* plen);
extern int set_ipmux_dma0_tx_buf_data(char *appbuf, int len);
static int opl_host_ioctl(struct inode *inode,struct file *filp,unsigned int cmd,u32 arg)
{
  int status = 0;
  enter();
  if(!test_bit(0,&opl_host_is_open)){
    return -ENODEV;
  }
  switch(cmd){
  case GET_IPMUX_DMA_TX_PHYS_ADDR:
    {
      int dmas= 0;
      int size = 0;
      int is_rx = 0;
      u32 addr = 0;
      dma_request_phys_addr_t req;
      if(copy_from_user(&req,arg,sizeof(dma_request_phys_addr_t))){
      }
      dmas = req.dmas;
      size = req.size;
      if(tx_attr[dmas].phys_addr == 0)
    malloc_for_dma(is_rx,dmas,size);
      if(tx_attr[dmas].phys_addr == 0){
    printk("malloc memory for dma%d tx buffer failed\n",dmas);
      }else{
    tx_attr[dmas].count++;
    addr = tx_attr[dmas].phys_addr_uncached;
    if(copy_to_user(&((dma_request_phys_addr_t *)arg)->phys_addr,&addr,4)){
    }
      }
    }
    break;
  case GET_IPMUX_DMA_RX_PHYS_ADDR:
    {
      int dmas= 0;
      int size = 0;
      int is_rx = 1;
      u32 addr = 0;
      
      dma_request_phys_addr_t req;
      if(copy_from_user(&req,arg,sizeof(dma_request_phys_addr_t))){
      }
      dmas = req.dmas;
      size = req.size;
      dbg("dmas is %d,size is %d\n",dmas,size);
      if(rx_attr[dmas].phys_addr == 0)
    malloc_for_dma(is_rx,dmas,size);
      if(rx_attr[dmas].phys_addr == 0){
    printk("malloc memory for dma%d tx buffer failed\n",dmas);
      }else{
    rx_attr[dmas].count ++;
    addr = rx_attr[dmas].phys_addr_uncached;
    dbg("the addr is 0x%x\n",addr);
    if(copy_to_user(&((dma_request_phys_addr_t *)arg)->phys_addr,&addr,4)){
    }
      }
    }

    break;
  case SET_IPMUX_DMA_RX_PHYS_ADDR:
      {
    int dmas= 0;
    int size = 0;
    u32 addr = 0;
    dma_request_phys_addr_t req;
    if(copy_from_user(&req,arg,sizeof(dma_request_phys_addr_t))){
    }
    dmas = req.dmas;
    size = req.size;
    addr = req.phys_addr;
    if((addr&0xa0000000)&&(rx_attr[dmas].allocated==0)){
      rx_attr[dmas].phys_addr_uncached = addr;
      rx_attr[dmas].size = size;
    }else{
      printk("the input addr is invalid\n");
    }
      }
      break;
  case SET_IPMUX_DMA_TX_PHYS_ADDR:
    {      
      int dmas= 0;
      int size = 0;
      u32 addr = 0;
      dma_request_phys_addr_t req;
      if(copy_from_user(&req,arg,sizeof(dma_request_phys_addr_t))){
      }
      dmas = req.dmas;
      size = req.size;
      addr = req.phys_addr;
      if((addr&0xa0000000) && (tx_attr[dmas].allocated==0)){
    tx_attr[dmas].phys_addr_uncached = addr;
    tx_attr[dmas].size = size;
    dbg("set tx addr is 0x%x ok\n",addr);
      }else{
    printk("the input addr is invalid\n");
      }
    }
    break;
  case FREE_IPMUX_DMA_RX_PHYS_ADDR:
    {
      int dmas= 0;
      dma_request_phys_addr_t req;
      if(copy_from_user(&req,arg,sizeof(dma_request_phys_addr_t))){
      }
      dmas = req.dmas;
      rx_attr[dmas].count--;
      free_for_dma(1,dmas);
    }
    break;
    
  case FREE_IPMUX_DMA_TX_PHYS_ADDR:
    {
      int dmas= 0;
      dma_request_phys_addr_t req;
      if(copy_from_user(&req,arg,sizeof(dma_request_phys_addr_t))){
      }
      dmas = req.dmas;
      tx_attr[dmas].count--;
      free_for_dma(0,dmas);
    }
    break;
  case SET_IPMUX_DMA_TX_BUF_DATA:
    {
      dma_request_data_t req;
      if(copy_from_user(&req,(char *)arg,sizeof(dma_request_data_t))){
      }
      set_ipmux_dma0_tx_buf_data(req.buf, *(req.len));
    }
    break;
  case GET_IPMUX_DMA_RX_BUF_DATA:
    {
      dma_request_data_t req;
      
      if(copy_from_user(&req,(char *)arg,sizeof(dma_request_data_t))){
      }
      
      get_ipmux_dma0_rx_buf_data(req.buf, req.len);

    }
    break;
  case ENABLE_IPMUX_HOST_DMA0_INTERRUPT:
    {
      ipmux_irq_enable(OPL_HOSTDMA0_IRQ);
    }
    break;
  case DISABLE_IPMUX_HOST_DMA0_INTERRUPT:
    {
      ipmux_irq_disable(OPL_HOSTDMA0_IRQ);
    }
    break;
  case WAIT_FOR_IPMUX_HOST_DMA0_INTERRUPT:
    {
      wait_event_interruptible(opl_host_dma0_waitq,opl_host_dma0_irq_event);
      if(!test_and_clear_bit(0,&opl_host_dma0_irq_event)){
    dbg("clear opl_host_dma0_irq_event failed \n");
    return -1;
      }
    }
    break;
  default:
    break;
  }
  exit();
  return status;
}

/** description: the module's entry. register the char device,request irq for host dma0 and malloc memory for dma0's buffer.
 *  @param void
 *  @retval 0 is success.
 *  @retval !0 is fail

 *  @see 
 *  @deprecated 
 */
static int __init opl_host_dma_init(void)
{
  int i = 0;
  int ret = 0;
  int count = 0;
  int size = 2*64*OPL_DMA_VAL_MAX;

  ret = register_chrdev(OPL_HOST_MAJOR,OPL_HOST_NAME,&opl_host_fops);
  if(ret < 0){
    /* add debug Message TBD */
    return -1;
  }
  for(i=0;i<IPMUX_DMA_NUMS;i++){
    memset(&tx_attr[i],0,sizeof(dma_buf_attr_t));
    memset(&rx_attr[i],0,sizeof(dma_buf_attr_t));
  }
  
  #ifndef OPL_DMA0_ETH
  init_waitqueue_head(&opl_host_dma0_waitq);
  ret = request_irq(OPL_HOSTDMA0_IRQ,host_dma0_isr,0,"IPMux dma0",NULL);
  if(ret){
    /* add error message */
    goto fail;
  }
 #endif
  return 0;
 fail:
  ret = unregister_chrdev(OPL_HOST_MAJOR,OPL_HOST_NAME);
  return -1;
}

/** description:the module's exit. it is used for free resouces:irq,memory and unregister the device. 
 *  @param void
 *  @return void

 *  @see 
 *  @deprecated 
 */
static void __exit opl_host_dma_exit(void)
{
  int i = 0;
  int ret = 0;

  #ifndef OPL_DMA0_ETH
  free_irq(OPL_HOSTDMA0_IRQ,NULL);
  #endif
  for(i = 0;i<IPMUX_DMA_NUMS;i++){
    free_for_dma(0,i);
    free_for_dma(1,i);
  }
  ret = unregister_chrdev(OPL_HOST_MAJOR,OPL_HOST_NAME);
  if(ret<0){
    /* add debug Message TBD */
  }
}

module_init(opl_host_dma_init);
module_exit(opl_host_dma_exit);
#ifdef LINUX_24
MODULE_LICENSE("GPL2");
#else
MODULE_LICENSE("Dual BSD/GPL");
#endif
MODULE_AUTHOR("opulan Inc");
MODULE_DESCRIPTION("opulan IPMUX-e switch chip driver module");

/** @}*/
/** @}*/
