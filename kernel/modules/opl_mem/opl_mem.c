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

/** @name 
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define OPL_MEM_MAJOR    150
#define OPL_MEM_DEVICE  "opl_mem"


/** @}*/

/** @name 
 *  @{
 */
/*--------------------------type definition------------------------- */
typedef struct opl_mem_read_req_s{
  u32 addr;
  u32 size;
  char buf[1024];
}opl_mem_read_req_t;

typedef struct opl_mem_write_req_s{
  u32 addr;
  u32 unit;
  u32 size;
  u32 value;
}opl_mem_write_req_t;
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
static int opl_mem_open(struct inode *inode, struct file *filp);         
static ssize_t opl_mem_read(struct file *filp, char  *buffer, size_t length, loff_t *offset);
static ssize_t opl_mem_write(struct file *filp, const char *buffer, size_t length, loff_t *offset);
static int opl_mem_mmap(struct file * file, struct vm_area_struct * vma);
static int opl_mem_release(struct inode *inode, struct file *filp);
static int opl_mem_ioctl(struct inode *inode,struct file *filp,unsigned int cmd,u32 arg);
/** @}*/
 
/** @name local variable declaration and definition
 *  @{
 */
/*--------------local  varible declaration and definition------------- */
static u32 opl_mem_open_count = 0;
static u32 opl_mem_is_open = 0;
struct file_operations opl_mem_fops = {
  .read = opl_mem_read,
  .write = opl_mem_write,
  .mmap = opl_mem_mmap,
  .open = opl_mem_open,
  .release = opl_mem_release, 
  .ioctl = opl_mem_ioctl,
};

/** @}*/
/** @defgroup opl_reg function_description(opl_reg_mmap.c)

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
static int opl_mem_ioctl(struct inode *inode,struct file *filp,unsigned int cmd,u32 arg)
{
  int status = 0;
  int i = 0;
  if(!test_bit(0,&opl_mem_is_open)){
    return -ENODEV;
  }
  switch(cmd){
  case 0://OPL_MEM_READ:
    {
      u32 addr;
      u32 size;
      char *buf,*p;
      opl_mem_read_req_t req;
      if(copy_from_user(&req,arg,sizeof(opl_mem_read_req_t))){
      }
      addr = req.addr;
      if(addr < 0x80000000){
	printk("the addr is invalid,the addr must be [0x80000000,0xffffffff]\n");
	return -1;
      }
      addr = (addr>>2)<<2;
      size = req.size;
      if(size > 1024){
	printk("the size is invalide,the size must be [1,1024] and should be number of 4\n");
	return -1;
      }
      size = size>>2;
      buf = (char *)kmalloc(GFP_KERNEL,size*4);
      if(!buf){
	printk("memory allocate in kernel space failed");
	return -1;
      }
      p = buf;
      memset(buf,0,size*4);
      for(i = 0;i<size;i++){
	*(u32 *)buf= *(u32 *)addr;
	addr += 4;
	(char *)buf+=4;
      }
      if(copy_to_user((char *)((opl_mem_read_req_t *)arg)->buf,(char *)p,size*4)){
	printk("copy to user error\n");
      }
      kfree(buf);
      break;
    }
  case 1: 			/* OPL_MEM_WRITE: 1byte, 2 byte,or 4 bytes.*/
    {
      opl_mem_write_req_t req;
      u32 addr;
      u32 unit;
      u32 value;
      if(copy_from_user(&req,arg,sizeof(opl_mem_write_req_t))){
      }
      addr = req.addr;
      unit = req.unit;
      value = req.value;
      printk("addr:0x%x\n",addr);
      printk("unit:0x%x\n",unit);
      printk("value:0x%x\n",value);
      if((unit != 1 )&&(unit != 2)&&(unit != 4)){
	printk("the unit value must be:1  2  or 4\n");
	return -1;
      }
      if(unit==4){
	if(addr%4 == 0){
	  *(u32 *)addr = value;
	  return 0;
	}else{
	  printk("when the unit=4,then addr must be 4 allignment\n");
	  return -1;
	}
      }
      if(unit == 2){
	if(addr%2==0){
	  *(u16 *) addr = (u16)value;
	  return 0;
	}else{
	  printk("when the unit=2,then addr must be 2 allignment\n");
	  return -1;
	}
      }
      *(char *)addr = (char)value;
      break;
    }
  case 2:			/* OPL_MEM_SET: 1byte, 2 byte, or 4bytes. */
    {
      opl_mem_write_req_t req;
      u32 addr;
      u32 unit;
      u32 value;
      u32 size;
      if(copy_from_user(&req,arg,sizeof(opl_mem_write_req_t))){
      }
      addr = req.addr;
      unit = req.unit;
      value = req.value;
      size = req.size;
      printk("addr:0x%x\n",addr);
      printk("unit:0x%x\n",unit);
      printk("value:0x%x\n",value);
      printk("size:0x%x\n",size );
      if((unit != 1 )&&(unit != 2)&&(unit != 4)){
	printk("the unit value must be:1  2  or 4\n");
	return -1;
      }
      if(unit==4){
	if(addr%4 == 0){
	  for(i = 0;i<size;i++){
	    *(u32 *)addr = value;
	    addr += 4;
	  }
	  return 0;
	}else{
	  printk("when unit= 4,the addr must be 4 allignment\n");
	  return -1;
	}
      }
      if(unit == 2){
	if(addr%2==0){
	  for(i = 0;i<size;i++){
	    *(u16 *) addr= (u16)value;
	    addr += 2;
	  }
	  return 0;
	}else{
	  printk("when the unit=2,then addr must be 2 allignment\n");
	  return -1;
	}
      }
      memset((char *)addr,(char)value,size);
      break;
    }
  default:
    printk("dont support this operations\n");
    status = -1;
    break;
  }
  return status;
}
static ssize_t opl_mem_read(struct file *filp, char  *buffer, size_t length, loff_t *offset)
{
  return 0;
}

static ssize_t opl_mem_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
  return 0;
}

/** description: memory map the ipmux register into user space to access it.
    which will reduce the spense on copy value between kernel space with user space.
    and reduce the dependency on interface of OS. 
    the ipmux register must be NOcached and NO swap out and NO Coredump.
 *  @param struct file *file
 *  @param struct vm_area_struct *vma
 *  @retval 0 success.
 *  @retval -(x) failure.

 *  @see mmap manual.
 *  @deprecated 
 */
static int opl_mem_mmap(struct file * file, struct vm_area_struct * vma)
{
#if 0
  unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
  /*
   * Accessing memory above the top the kernel knows about or
   * through a file pointer that was marked O_SYNC will be
   * done non-cached.
   */
  offset += 0x1f000000;
  if ((offset>__pa(high_memory)) || (file->f_flags & O_SYNC)) {
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
  }

  /* Don't try to swap out physical pages.. */
  vma->vm_flags |= VM_RESERVED;
  
  /* Don't dump addresses that are not real memory to a core file.*/
  if (offset >= __pa(high_memory) || (file->f_flags & O_SYNC))
    vma->vm_flags |= VM_IO;
  if (remap_page_range(vma->vm_start, offset, vma->vm_end-vma->vm_start,
		       vma->vm_page_prot)){
    return -EAGAIN;
  }
#endif
  return 0;
}

static int opl_mem_open(struct inode *inode, struct file *filp)         
{
  if (test_and_set_bit(0, &opl_mem_is_open))
    printk(KERN_DEBUG "the /dev/opl_mem is already opened\n");
  opl_mem_open_count ++;
  MOD_INC_USE_COUNT;
  return 0;
}

static int opl_mem_release(struct inode *inode, struct file *filp)
{
  opl_mem_open_count ++;
  if(!opl_mem_open_count)
    clear_bit(0, &opl_mem_is_open);
  MOD_DEC_USE_COUNT;
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
static int __init opl_mem_init(void)
{
  int ret;
  ret = register_chrdev(OPL_MEM_MAJOR,OPL_MEM_DEVICE,&opl_mem_fops);
  if(ret<0){
    /* TBD add debug message */
  }
  return ret;
}
/** description: it is called when the module removed. do the reverse of
    opl_reg_module_init. 
 *  @param void
 *  @return NULL

 *  @see 
 *  @deprecated 
 */
static void __exit opl_mem_exit(void)
{
  int ret;
  ret = unregister_chrdev(OPL_MEM_MAJOR,OPL_MEM_DEVICE);
  if(ret < 0){
    /*TBD:add debug message  */
  }
  return;
}

module_init(opl_mem_init);
module_exit(opl_mem_exit);
MODULE_LICENSE("GPL2");
MODULE_AUTHOR("stephanxu <hxu@opulan.com>");
MODULE_DESCRIPTION("dedicated the module for mmap the opulan switch chip register.");
/** @}*/
/** @}*/
