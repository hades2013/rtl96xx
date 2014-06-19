#ifndef _MINTE_API_H_
#define _MINTE_API_H_
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
void wait_for_ipmux_intr(u32 *irq_pending);
void enable_ipmux_intr(void);
void disable_ipmux_intr(void);
int open_minte(void);
int close_minte(void);


/** @}*/


/** @defgroup

 *  @{
 */
/*----------------------local  function definition--------------------- */






/** @}*/
/** @}*/
#endif /* _MINTE_API_H_ */
