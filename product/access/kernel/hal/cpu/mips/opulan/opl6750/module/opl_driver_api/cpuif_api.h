#ifndef _CPUIF_API_H_
#define _CPUIF_API_H_
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
#include <opl_typedef.h>
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
int read_from_dma_rxbuf(int dmas,int bd,void *pkt,int len);
int write_to_dma_txbuf(int dmas,int bd,void *pkt,int len);
void enable_host_dma0_intr(void);
void disable_host_dma0_intr(void );
void *malloc_dma_rxbuf(int dmas,int size);
void *malloc_dma_txbuf(int dmas,int size);
void set_dma_rxbuf_addr(int dmas,u32 addr);
void set_dma_txbuf_addr(int dmas,u32 addr);
void free_dma_rxbuf(int dmas);
void free_dma_txbuf(int dmas);
int open_cpuif(void);
int close_cpuif(void);



/** @}*/


/** @defgroup

 *  @{
 */
/*----------------------local  function definition--------------------- */






/** @}*/
/** @}*/
#endif /* _CPUIF_API_H_ */
