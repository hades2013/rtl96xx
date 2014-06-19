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


/** @name 
 *  @{
 */
/*--------------------------Macro definition------------------------- */


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


/** @}*/
 
/** @name local variable declaration and definition
 *  @{
 */
/*--------------local  varible declaration and definition------------- */

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
struct timer_list timer_test;
unsigned long time;
void timer_test_function(unsigned long data)
{
  printk(KERN_ALERT "jiffies is.... %d\n",data);
  del_timer(&timer_test);
  timer_test.data = jiffies;
  timer_test.function = timer_test_function;
  timer_test.expires = jiffies + 100;
  add_timer(&timer_test);
}
static int __init opl_timer_init(void)
{
  init_timer(&timer_test);
  time = jiffies;
  timer_test.data = time;
  timer_test.function = timer_test_function;
  timer_test.expires = jiffies + 100;
  add_timer(&timer_test);
  return 0;
}
/** description: it is called when the module removed. do the reverse of
    opl_reg_module_init. 
 *  @param void
 *  @return NULL

 *  @see 
 *  @deprecated 
 */
static void __exit opl_timer_exit(void)
{
  del_timer(&timer_test);
  return;
}

module_init(opl_timer_init);
module_exit(opl_timer_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("stephanxu <hxu@opulan.com>");
MODULE_DESCRIPTION("dedicated the module for mmap the opulan switch chip register.");
/** @}*/
/** @}*/
