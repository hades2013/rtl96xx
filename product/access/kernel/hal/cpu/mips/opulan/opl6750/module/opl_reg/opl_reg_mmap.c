/** THE FORMAT is FOR DOXYGEN to Generate the document

  *  @defgroup OPL_REG register mmap module
  *  @copyright opulan.com
  *  @author stephanxu@sina.com
  *  @version v0.2
  *  @date June-29-2007
  *  @histroy 
  *  author      date        description
  *  -------------------------------------------------------------------
     hxu           June-29-2007   mainly from the old version and rewrite the opulan_read function.\n
     hxu           July-05-2007   distill the piece of the ipmux driver code dedicated for the opl switch chip register map.
  *  @{
  */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/highmem.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/kmap_types.h>
#include <asm/page.h>
#include <asm/io.h>
#include <linux/slab.h>

#include "../include/opl_hw_ops.h"
#include "../include/opl_reg_mmap.h"

/** @name 
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define OPL_REG_MAJOR    110
#if 0
#define OPL_REG_DEVICE  "opl_reg"
#else
#define OPL_REG_DEVICE  "/dev/opl_reg"
#endif

#ifdef debug 
    #undef debug
#endif

#if 0
#define debug(x...) printk("  [DEBUG : %s @ %d]", __FUNCTION__, __LINE__);printk(x);
#else
#define debug(x...)
#endif

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
static int opl_reg_open(struct inode *inode, struct file *filp);         
static ssize_t opl_reg_read(struct file *filp, char  *buffer, size_t length, loff_t *offset);
static ssize_t opl_reg_write(struct file *filp, const char *buffer, size_t length, loff_t *offset);
static int opl_reg_mmap(struct file * file, struct vm_area_struct * vma);
static int opl_reg_release(struct inode *inode, struct file *filp);

/** @}*/
 
/** @name local variable declaration and definition
 *  @{
 */
/*--------------local  varible declaration and definition------------- */
static u32 opl_reg_open_count = 0;
static u32 opl_reg_is_open = 0;
struct file_operations opl_reg_fops = {
  .read = opl_reg_read,
  .write = opl_reg_write,
  .mmap = opl_reg_mmap,
  .open = opl_reg_open,
  .release = opl_reg_release, 
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

static ssize_t opl_reg_read(struct file *filp, char  *buffer, size_t length, loff_t *offset)
{
    return 0;
}

static ssize_t opl_reg_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
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
static int opl_reg_mmap(struct file * file, struct vm_area_struct * vma)
{
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
  
#ifdef LINUX_24
  debug(">>>>memmap : physical(offset) 0x%x to userspace(vma->vm_start) 0x%x ~ 0x%x\n", offset, vma->vm_start, vma->vm_end);
  if (remap_page_range(vma->vm_start, offset, vma->vm_end-vma->vm_start,
               vma->vm_page_prot))
#else
  offset = offset >> PAGE_SHIFT;
  debug(">>>>memmap : physical(offset) 0x%x to userspace(vma->vm_start) 0x%x ~ 0x%x\n", offset, vma->vm_start, vma->vm_end);
  if (remap_pfn_range(vma, vma->vm_start, offset, vma->vm_end-vma->vm_start,
               vma->vm_page_prot))
#endif
  {
    return -EAGAIN;
  }
  return 0;
}

static int opl_reg_open(struct inode *inode, struct file *filp)         
{
  if (test_and_set_bit(0, &opl_reg_is_open))
    printk(KERN_DEBUG "the /dev/opl_reg is already opened\n");
  opl_reg_open_count ++;
  return 0;
}

static int opl_reg_release(struct inode *inode, struct file *filp)
{
  opl_reg_open_count ++;
  if(!opl_reg_open_count)
    clear_bit(0, &opl_reg_is_open);
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
static int __init opl_reg_init(void)
{
  int ret;
  ret = register_chrdev(OPL_REG_MAJOR,OPL_REG_DEVICE,&opl_reg_fops);
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
static void __exit opl_reg_exit(void)
{
  int ret;
  ret = unregister_chrdev(OPL_REG_MAJOR,OPL_REG_DEVICE);
  if(ret < 0){
    /*TBD:add debug message  */
  }
  return;
}

module_init(opl_reg_init);
module_exit(opl_reg_exit);
#ifdef LINUX_24
MODULE_LICENSE("GPL2");
#else
MODULE_LICENSE("Dual BSD/GPL");
#endif
MODULE_AUTHOR("opulan inc");
MODULE_DESCRIPTION("dedicated the module for mmap the opulan switch chip register.");
/** @}*/
/** @}*/
