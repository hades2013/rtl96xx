/** THE FORMAT is FOR DOXYGEN to Generate the document

  *  @defgroup OPL_REG register mmap module
  *  @copyright opulan.com
  *  @author stephanxu@sina.com
  *  @version v0.2
  *  @date June-29-2007
  *  @histroy 
  *  author		 date		 description
  *  -------------------------------------------------------------------
     hxu           June-29-2007   mainly from the old version and rewrite the opulan_read function.\n
     hxu           July-05-2007   distill the piece of the ipmux driver code dedicated for the opl switch chip register map.
  *  @{
  */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/slab.h>

#include <opl_hw_ops.h>
#include <opl_reg_mmap.h>

#define enter_function() printk("enter %s\n",__FUNCTION__)
#define exit_function() printk("exit %s\n",__FUNCTION__)
/** @name 
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define OPL_FPGA_MAJOR    190
#define OPL_FPGA_DEVICE  "opl_fpga"

#define FPGA_DATA_BIT  (1<<4)
#define FPGA_DCLK_BIT  (1<<3) 
#define FPGA_STAT_BIT  (1<<7) 
#define FPGA_DONE_BIT  (1<<5)
#define FPGA_INIT_BIT  (1<<8)
#define FPGA_CONF_BIT  (1<<6)

#define IPMUX_REG_BASE    0xbf000000
#define GPIO_REG_VAL   0x2880
#define GPIO_REG_DIR   0x2884
#define GPIO_REG_OUTPUT_SET 0x2888
#define GPIO_REG_OUTPUT_CLR 0x288C

#define IPMUX_REG_ID2ADDR(regId) 	((u32)IPMUX_REG_BASE + (u32)(regId))
#define IPMUX_REG_ADDR2ID(addr) 	((u32)(addr) - (u32)IPMUX_REG_BASE)
static inline int RegRead(u32 regID, volatile u32 *pval)
{
  if (regID % 4 != 0)
    return -1;
  *(u32*)(pval) = *(volatile u32 *)IPMUX_REG_ID2ADDR(regID);
  return 0;
}

static inline int RegWrite(u32 regID, u32 val)
{
  if (regID % 4 != 0)
    return -1;
  *((volatile u32 *)IPMUX_REG_ID2ADDR(regID)) = (u32)(val);
  return 0;
}
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

/** @}*/

/** @name the global variable declaration and definition.
 *  @{
 */

/** @}*/
   
/** @name local function declaration
 *  @{
 */
/*--------------local  function declaration------------- */
static int opl_fpga_open(struct inode *inode, struct file *filp);         
static ssize_t opl_fpga_read(struct file *filp, char  *buffer, size_t length, loff_t *offset);
static ssize_t opl_fpga_write(struct file *filp, const char *buffer, size_t length, loff_t *offset);
static int opl_fpga_release(struct inode *inode, struct file *filp);
static int fpga_download(char *buf, int length);
/** @}*/
 
/** @name local variable declaration and definition
 *  @{
 */
/*--------------local  varible declaration and definition------------- */
static u32 opl_fpga_open_count = 0;
static u32 opl_fpga_is_open = 0;
struct file_operations opl_fpga_fops = {
  .read = opl_fpga_read,
  .write = opl_fpga_write,
  .open = opl_fpga_open,
  .release = opl_fpga_release, 
};

/** @}*/
/** @defgroup opl_fpga function_description(opl_fpga_mmap.c)

 *  @{
 */
/*------------local  function declaration and definition-------------- */

/** description: 
 *  @param 
 *  @param 
 *  @param 
 *  @return 
 *  @retval 
 *  @retval 

 *  @see 
 *  @deprecated 
 */

static ssize_t opl_fpga_read(struct file *filp, char  *buffer, size_t length, loff_t *offset)
{
  return 0;
}

static ssize_t opl_fpga_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
  int count,order;
  char *buf = NULL;
  int ret = 0;
  enter_function();
  if(!test_bit(0,&opl_fpga_is_open)){
    return -ENODEV;
  }
  for(count = PAGE_SIZE,order = 0;count < length;order++,count <<= 1);
  buf = (char *)__get_free_pages(GFP_KERNEL,order);
  if(!buf){
    printk("allocate memory for store fpga image failed");
    return -1;
  }
  printk("allocate memory successfully\n");
  memset(buf,0,PAGE_SIZE<<order);
  if(copy_from_user(buf,buffer,length)){
    printk("copy the fpga image from user space to kernel space failed");
    free_pages(buf,order);   
    return -1;
  }
  printk("copy the %d bytes data from user space to kernel space successfule\n",length);
  ret = fpga_download(buf,length);
  if(ret){
    printk("download fpga image to board failed\n");
    free_pages(buf,order);    
    return -1;
  }
  free_pages(buf,order);
  exit_function();
  return 0;
}

static int opl_fpga_open(struct inode *inode, struct file *filp)         
{
  if (test_and_set_bit(0, &opl_fpga_is_open))
    printk(KERN_DEBUG "the /dev/opl_fpga is already opened\n");
  opl_fpga_open_count ++;
  MOD_INC_USE_COUNT;
  return 0;
}

static int opl_fpga_release(struct inode *inode, struct file *filp)
{
  opl_fpga_open_count --;
  if(!opl_fpga_open_count)
    clear_bit(0, &opl_fpga_is_open);
  MOD_DEC_USE_COUNT;
  return 0;
}
/** description: The driver module entry.to fpgaister the chrdev and 
    request irq. and some essential initialization.
 *  @param void
 *  @retval 0 success
 *  @retval !0  failure.the module cant be inserted into os.

 *  @see insmod rmmod
 *  @deprecated 
 */
static int __init opl_fpga_init(void)
{
  int ret;
  u32 val;
  enter_function();
  ret = register_chrdev(OPL_FPGA_MAJOR,OPL_FPGA_DEVICE,&opl_fpga_fops);
  if(ret<0){
    /* TBD add debug message */
  }
  RegRead(GPIO_REG_DIR,&val);
  val |= FPGA_DATA_BIT | FPGA_DCLK_BIT | FPGA_CONF_BIT;
  val &= ~(FPGA_DONE_BIT | FPGA_INIT_BIT | FPGA_STAT_BIT); 
  RegWrite(GPIO_REG_DIR,val);
  
  RegRead(GPIO_REG_OUTPUT_SET,&val);
  val |= FPGA_CONF_BIT;
  val &= ~(FPGA_DATA_BIT | FPGA_DCLK_BIT);
  RegWrite(GPIO_REG_OUTPUT_SET,val);
  exit_function();
  return ret;
}
/** description: it is called when the module removed. do the reverse of
    opl_fpga_module_init. 
 *  @param void
 *  @return NULL

 *  @see 
 *  @deprecated 
 */
static void __exit opl_fpga_exit(void)
{
  int ret;
  ret = unregister_chrdev(OPL_FPGA_MAJOR,OPL_FPGA_DEVICE);
  if(ret < 0){
    /*TBD:add debug message  */
  }
  return;
}

static int fpga_download(char *buf,int length)
{
  int i,count=0;
  int status = 0;
  int high_flag = 0;
  u32 val = 0;
  enter_function();
  /* programming signal?? */
  RegWrite(GPIO_REG_OUTPUT_SET,0);
  val = FPGA_CONF_BIT;
  RegWrite(GPIO_REG_OUTPUT_CLR,val);
  for(i = 0;i<100000;i++){
    RegRead(GPIO_REG_VAL,&val);
    if(val & FPGA_CONF_BIT)
      continue;
    if(~val & FPGA_DONE_BIT)
      break;
  }
  if(i == 100000){
    printk("error:FPGA DONE is not proper\n");
    return -1;
  }
  RegWrite(GPIO_REG_OUTPUT_SET,FPGA_CONF_BIT);
  for(i = 0;i<100000;i++);
  count = 0;
  while(count < 3000){
    RegRead(GPIO_REG_VAL,&val);
    if(val & FPGA_STAT_BIT)
      break;
    count ++;
  }
  if(count == 3000){
    printk("warning: nStatus is not proper state\n");
  }
  if (!status){
    u32 nbit;
    for(i = 0;i<length;i++){
      for(nbit = 0;nbit < 8;nbit ++){
	if(buf[i] & (1<<nbit)){
	  if(!high_flag){
	    RegWrite(GPIO_REG_OUTPUT_CLR,0);
	    RegWrite(GPIO_REG_OUTPUT_SET,FPGA_DATA_BIT);
	    high_flag = 1;
	  }
	}else{
	  if(high_flag){
	    RegWrite(GPIO_REG_OUTPUT_SET,0);
	    val = (FPGA_DATA_BIT);
	    RegWrite(GPIO_REG_OUTPUT_CLR,val);
	    high_flag = 0;
	  }
	}
	RegWrite(GPIO_REG_OUTPUT_CLR,0);
	RegWrite(GPIO_REG_OUTPUT_SET,FPGA_DCLK_BIT);
	RegWrite(GPIO_REG_OUTPUT_SET,0);
	RegWrite(GPIO_REG_OUTPUT_CLR,FPGA_DCLK_BIT);
      }
    }
    count = 0;
    while(count < 1000000){
      RegRead(GPIO_REG_VAL,&val);
      if((val & FPGA_DONE_BIT )&&(!(~val & FPGA_STAT_BIT))){
	printk("download FPGA image successfully\n");
	break;
      }
      count ++;
    }
    if(count == 1000000){
      printk("down FPGA image failed in kernel\n");
      return -1;
    }
  }else{
    return -1;
  }
  exit_function();
  return 0;
}
module_init(opl_fpga_init);
module_exit(opl_fpga_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("stephanxu <hxu@opulan.com>");
MODULE_DESCRIPTION("dedicated the module for downloading fpga.");
/** @}*/
/** @}*/
