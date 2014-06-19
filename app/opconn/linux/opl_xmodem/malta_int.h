#ifndef _OPL_MALTA_INT_H_
#define _OPL_MALTA_INT_H_
/** THE FORMAT is FOR DOXYGEN to Generate the document\n

  *  @defgroup MALTA malta module
  *  declare the the ipmux interrupt controller RegisterBase,register offset,\n
  *  other constants and register operation macro.
  *  @copyright opluan.com
  *  @author hxu@opulan.com\n
  *  @version v1.0\n
  *  @date June-27-2007\n
  *  @histroy 
  *  author		 date		 description
  *  -------------------------------------------------------------------
     hxu             June-27-2007    reserve the essensial macro definition.In my mind: simple is beautiful.
  *  @{
  */
#include <linux/types.h>

/** @name Macro definition 
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define IPMUX_REG_SIZE		   32			/* 32 bits register for IPMux */
#define REG_ADDR_ALLIGNED	    4			/* 32bits word */
#define FIELD_BITS(n)	            (0xffffffff >> (IPMUX_REG_SIZE - (n)))

#define IPMUX_REG_BASE  0xbf000000
#define IPMUX_REG_ID2ADDR(regId)   ((u32)IPMUX_REG_BASE + (u32)(regId))
#define IPMUX_REG_ADDR2ID(addr)    ((u32)(addr) - (u32)IPMUX_REG_BASE)

#define ipMuxRegRead(regId, pval)  *(u32*)(pval) = *(volatile u32 *)IPMUX_REG_ID2ADDR(regId)
#define ipMuxRegWrite(regId, val)  *((volatile u32 *)IPMUX_REG_ID2ADDR(regId)) = (u32)(val)

#define REG_INTP  0x2a00
#define REG_INTE  0x2a08
#define REG_INTCFG  0x2A18

#define REG_BIT_INTCFG_IRQ0MD_OFF      15
#define REG_BIT_INTCFG_IRQ0MD	       (FIELD_BITS(1) << REG_BIT_INTCFG_IRQ0MD_OFF)
#define REG_BIT_INTCFG_MINTE_OFF       0
#define REG_BIT_INTCFG_MINTE	       (FIELD_BITS(1) << REG_BIT_INTCFG_MINTE_OFF)

#define REG_BIT_SYSCFG1_MINTE_OFF       0
#define REG_BIT_SYSCFG1_MINTE           (FIELD_BITS(1) << REG_BIT_SYSCFG1_MINTE_OFF)
#define IPMUX_MAX_NUM_OF_INT_LEVEL2	32

/** irq assignment   */
#define OPL_UART0_IRQ    23
#define OPL_UART1_IRQ    24
#define OPL_HOSTDMA0_IRQ 25
#define OPL_FEDMA1_IRQ   26
#define OPL_IPMUX_IRQ    29

/** @}*/

/** @name 
 *  @{

 */
/*--------------------------type definition------------------------- */

/** @}*/

/** @name global variable and function exported
 *  @{
 */
/*-----------------global varible/function declaration--------------- */
extern void malta_hw0_irqdispatch(void);
extern void corehi_irqdispatch(void);
extern void __init init_IRQ(void);

extern void ipmux_irq_disable(unsigned int irq);
extern void ipmux_irq_enable(unsigned int irq);
/** @}*/
   
/** @name 
 *  @{
 */
/*--------------local  varible declaration and definition------------- */

/** @}*/
 

/** @defgroup
 *  @{
 */
/*------------local  function declaration and definition-------------- */
/** @}*/
/** @}*/
#endif /*_OPL_MALTA_INT_H */

