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
#define GET_IPMUX_DMA_TX_PHYS_ADDR  1000
#define GET_IPMUX_DMA_RX_PHYS_ADDR  2000

#define SET_IPMUX_DMA_TX_PHYS_ADDR  1100
#define SET_IPMUX_DMA_RX_PHYS_ADDR  2100

#define FREE_IPMUX_DMA_TX_PHYS_ADDR 1200
#define FREE_IPMUX_DMA_RX_PHYS_ADDR 2200

#define GET_IPMUX_DMA_RX_BUF_DATA       3000
#define SET_IPMUX_DMA_TX_BUF_DATA       4000



#define ENABLE_IPMUX_HOST_DMA0_INTERRUPT   5000
#define DISABLE_IPMUX_HOST_DMA0_INTERRUPT  6000
#define WAIT_FOR_IPMUX_HOST_DMA0_INTERRUPT 7000





/** @}*/

/** @name type definition
 *  @{
 */
/*--------------------------type definition------------------------- */

typedef struct dma_request_phys_addr_s{
  int dmas;
  int size;
  unsigned long phys_addr;
}dma_request_phys_addr_t;

typedef struct dma_request_data_s{
  int dmas;
  int bd;
  int *len;
  char *buf;
}dma_request_data_t;



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





/** @}*/


/** @defgroup

 *  @{
 */
/*----------------------local  function definition--------------------- */






/** @}*/
/** @}*/
