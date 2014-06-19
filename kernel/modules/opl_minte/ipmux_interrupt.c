/** THE FORMAT is FOR DOXYGEN to Generate the document

  *  @defgroup IPMUX ipmux module
  *  @copyright opulan.com\n
  *  @author stephanxu@sina.com\n
  *  @version v1.0\n
  *  @date June-29-2007\n
  *  @histroy 
  *  \nauthor	    date		 description
  *  \n-------------------------------------------------------------------\n
     \nhxu         June-29-2007      mainly from the old version and make some modification.         
  *  @{
  */
#include <linux/wait.h>
#include <asm/bitops.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#ifndef LINUX_24
#include <linux/irqreturn.h>
#endif
#include "../include/ipmux_interrupt.h"
#include "../include/opl_hw_ops.h"
/** @name Macro definition.
 *  @{
 */
/*--------------------------Macro definition------------------------- */


/** @}*/

/** @name 
 *  @{
 */
/*--------------------------type definition------------------------- */

/** @}*/

/** @name global variable imported
 *  @{
 */
/*-----------------global varible/function declaration--------------- */

/** @}*/
   
/** @name local variable declaration and definition.
 *  @{
 */
/*--------------local  varible declaration and definition------------- */
/* why I reserve it? consistency.change from int to char. */
static char intr_status[8][32] = {
/*   bit 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31	*/
/*upt*/	{0, 0, 0, 0,-1,-1,-1,-1, 0, 0, 0, 0,-1,-1,-1,-1, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0,-1, 0},
/*misc*/{0, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*iwf*/	{0, 0, 0, 0, 0,-1, 0, 0,-1,-1,-1, 0,-1, 0,-1, 0,-1, 0, 0,-1, 0, 0, 0, 0,-1, 0, 0,-1, 0, 0,-1,-1},
/*brg*/	{0, 0, 0,-1, 0, 0, 0, 0,-1,-1, 0, 0,-1, 0,-1, 0, 0, 0, 0, 0,-1, 0, 0, 0,-1,-1,-1, 0, 0, 0, 0,-1},
/*ppe*/	{0, 0, 0, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*uptm*/{0,-1, 0,-1,-1,-1,-1,-1,-1,-1, 0, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*dntm*/{0,-1, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
/*mc*/	{0, 0, 0, 0,-1, 0, 0, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};



static void turn_on_level2_intr(u32 regID,char *bitmap);
static void turn_off_level2_intr(u32 regID);
static u32 get_mask(char *bitmap);

/** @}*/
 
void impux_interrupt_handler(int irq,void *dev_id,struct pt_regs *regs);


/** @defgroup  ipmux function_description(ipmux_interrupt.c)
 *  @{
 */
/*------------local  function declaration and definition-------------- */
/** description: the ipmux interrupt handler,only wake up the process,which to read the irqpending.
 *  @param int irq
 *  @param void *dev_id
 *  @param struct pt_regs regs
 *  @return NULL

 *  @see ipmux_hw0_irqinit
 *  @deprecated 
 */
#ifdef LINUX_24
void ipmux_interrupt_handler(int irq,void *dev_id,struct pt_regs *regs)
#else
irqreturn_t ipmux_interrupt_handler(int irq,void *dev_id,struct pt_regs *regs)
#endif
{
#if 1
  if(test_and_set_bit(0,&g_opl_chip_irq_event)){
    /* TBD: add some debug message.if it is set, some bugs exist. */
  }
#endif
  wake_up_interruptible(&g_opl_chip_waitq);
#ifndef LINUX_24
  return IRQ_HANDLED;
#endif
}

static u32 get_mask(char *bitmap)
{
  int i = 0;
  u32 mask = 0;
  for (i = 0; i < 32; ++i) {
    if (bitmap[i] == -1) {
      mask |= (1<<i);
    }
  }
  return ~mask;
}

/** description: To enable the level2 interrupt.
 *  @param unsigned long regID it is the register address offset.
 *  @param *bitmap transfer the bitmap[] into 32bit mask.
 *  @return NULL

 *  @see 
 *  @deprecated 
 */
static void turn_on_level2_intr(u32 regID, char *bitmap)
{
  u32 mask = get_mask(bitmap);
  ipMuxRegWrite(regID,mask);
}
/** description: 
 *  @param unsigned long regID it is the register address offset.
 *  @return NULL

 *  @see 
 *  @deprecated 
 */
static void turn_off_level2_intr(u32 regID)
{
  ipMuxRegWrite(regID,0);
}
/** description: request the ipmux irq and enable the its intr.
 *  @param void
 *  @retval 0 success
 *  @retval -1 failure

 *  @see ipmux_module_init
 *  @deprecated 
 */
int ipmux_hw0_irqinit(void)
{
  /* request the ipmux_irq for ipmux switch,all of them share the same irq */
  if(request_irq(OPL_IPMUX_IRQ,ipmux_interrupt_handler,
		 0,"IPMUX interrupt",NULL) != 0){
    return -1;
  }
  /* turn on irqs of ipmux:pos_upt,atb,iwf,bridge,ppe,uptom,dmtm and mc */
  /*
    turn_on_level2_intr(REG_INT_EN_POS_UPT, intr_status[0]);
    turn_on_level2_intr(REG_INT_EN_ATB, intr_status[1]);
    turn_on_level2_intr(REG_INT_EN_IWF, intr_status[2]);
    turn_on_level2_intr(REG_INT_EN_BRG, intr_status[3]);
    turn_on_level2_intr(REG_INT_EN_PPE, intr_status[4]);
    turn_on_level2_intr(REG_INT_EN_UPTM, intr_status[5]);
    turn_on_level2_intr(REG_INT_EN_DNTM, intr_status[6]);
    turn_on_level2_intr(REG_INT_EN_MC, intr_status[7]);
    */
  return 0;
}
/** description: free irq,and disable ipmux interrupt.
 *  @param void
 *  @return NULL

 *  @see ipmux_module_exit()
 *  @deprecated 
 */
void ipmux_hw0_irqexit(void)
{
  /* turn off the irqs of ipmux:pos,atb,iwf,bridge,ppe,uptm,dntm and mc */
  /*
    turn_off_level2_intr(REG_INT_EN_POS_UPT);
    turn_off_level2_intr(REG_INT_EN_ATB);
    turn_off_level2_intr(REG_INT_EN_IWF);
    turn_off_level2_intr(REG_INT_EN_BRG);
    turn_off_level2_intr(REG_INT_EN_PPE);
    turn_off_level2_intr(REG_INT_EN_UPTM);
    turn_off_level2_intr(REG_INT_EN_DNTM);
    turn_off_level2_intr(REG_INT_EN_MC);
  */
  /*free the irq.  */
  free_irq(OPL_IPMUX_IRQ,NULL);
}

/** @}*/
/** @}*/
