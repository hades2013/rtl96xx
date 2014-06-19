#ifndef _OPL_MOD_IF_H_
#define _OPL_MOD_IF_H_
/** THE FORMAT is FOR DOXYGEN to Generate the document automatically

  *  @defgroup 
  *  @copyright 
  *  @author 
  *  @version 
  *  @date 
  *  @histroy 
  *  author		 date		 description
  *  -------------------------------------------------------------------

  *  @{
  */
/** @name Macro definition
 *  @{
 */
/*--------------------------Macro definition------------------------- */





/** @}*/

/** @name type definition
 *  @{
 */
/*--------------------------type definition------------------------- */





/** @}*/

/** @name global variable and function imported
 *  @{
 */
/*-----------------global varible/function declaration--------------- */





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





/** @}*/
 

/** @name global variable and function exported
 *  @{
 */
/*------------------global variable and function exported-------------- */

void wait_for_host_dma0_interrupt(void);
#ifdef OPL_DMA0_ETH
int read_from_dma_rxbuf(int dmas,int bd,void *pkt,int *len);
#else
int read_from_dma_rxbuf(int dmas,int bd,void *pkt,int len);
#endif
int write_to_dma_txbuf(int dmas,int bd,void *pkt,int len);
void enable_host_dma0_interrupt(void);
void disable_host_dma0_interrupt(void );
void *malloc_dma_rxbuf(int dmas,int size);
void *malloc_dma_txbuf(int dmas,int size);
void set_dma_rxbuf_addr(int dmas,unsigned int  addr);
void set_dma_txbuf_addr(int dmas,unsigned int addr);
void free_dma_rxbuf(int dmas);
void free_dma_txbuf(int dmas);
int open_cpuif(void);
int close_cpuif(void);

void wait_for_ipmux_intr(unsigned  *irq_pending);
void enable_ipmux_intr(void);
void disable_ipmux_intr(void);
int open_minte(void);
int close_minte(void);

int opl_modules_init(void);
int opl_modules_deinit(void *buf,int len);
/** @}*/


/** @defgroup

 *  @{
 */
/*----------------------local  function definition--------------------- */






/** @}*/
/** @}*/
#endif /* _OPL_MOD_IF_H_ */
