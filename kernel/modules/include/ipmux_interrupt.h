#ifndef _IPMUX_INTERRUPT_H_
#define _IPMUX_INTERRUPT_H_
/** THE FORMAT is FOR DOXYGEN to Generate the document

  *  @defgroup IPMUX ipmux module
  *  @copyright opulan.com
  *  @author stephanxu@sina.com
  *  @version v1.0
  *  @date June-29-2007\n
  *  @histroy \n
  *  author	    date		 description\n
  *  -------------------------------------------------------------------\n
      hxu       June-29-2007    mainly from the old version.\n
  *  @{
  */

/** @name Macro definition
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define REG_INT_EN_POS_UPT    0x10204
#define REG_INT_PEN_POS_UPT   0x10200

#define REG_INT_EN_ATB        0x11404
#define REG_INT_PEN_ATB       0x11400

#define REG_INT_EN_IWF        0x13E00
#define REG_INT_PEN_IWF       0x13E04

#define REG_INT_EN_BRG        0x21000
#define REG_INT_PEN_BRG       0x21004

#define REG_INT_EN_PPE        0x18180
#define REG_INT_PEN_PPE       0x18184

#define REG_INT_EN_UPTM       0x14404
#define REG_INT_PEN_UPTM      0x14400

#define REG_INT_EN_DNTM       0x14F04
#define REG_INT_PEN_DNTM      0x14F00

#define REG_INT_EN_MC         0x21404
#define REG_INT_PEN_MC        0x21400

#define REG_INT_EN_DMA0       0x2B58
#define REG_INT_PEN_DMA0      0x2B54


#define OPL_UART0_IRQ   23
#define OPL_UART1_IRQ   24
#define OPL_HOSTDMA0_IRQ  25
#define OPL_FEDMA1_IRQ    26
#define OPL_IPMUX_IRQ     29


/*
#define IPMUX_REG_BASE  0xbf000000
#define IPMUX_REG_ID2ADDR(regId)   ((u32)IPMUX_REG_BASE + (u32)(regId))
#define IPMUX_REG_ADDR2ID(addr)    ((u32)(addr) - (u32)IPMUX_REG_BASE)
*/
/** @}*/

/** @name 
 *  @{
 */
/*--------------------------type definition------------------------- */

/** @}*/

/** @name 
 *  @{
 */
/*-----------------global varible/function declaration--------------- */

/** @}*/
   
/** @name 
 *  @{
 */
/*--------------local  varible declaration and definition------------- */

/** @}*/
 

/** @defgroup  ipmux declaration_exported(ipmux_interrupt.h) 
 *  @{
 */
/*------------inline  function declaration and definition-------------- */
extern int ipmux_hw0_irqinit(void);
extern void ipmux_hw0_irqexit(void);

extern u32 g_opl_chip_irq_event;
extern wait_queue_head_t g_opl_chip_waitq;

/*
inline void ipMuxRegRead(u32 regId,u32 *val)
{
  *val = *(volatile u32 *)IPMUX_REG_ID2ADDR(regId);
} 

inline void ipMuxRegWrite(u32 regId,u32 val)
{
  *((volatile u32 *)IPMUX_REG_ID2ADDR(regId)) = (u32)(val);
}
*/

/** @}*/
/** @}*/
#endif /* _IPMUX_INTERRUPT_H_ */
