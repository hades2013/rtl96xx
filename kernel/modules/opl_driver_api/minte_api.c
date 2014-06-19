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
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <opl_typedef.h>
/** @name Macro definition
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define OPL_MINTE_DEVICE_NAME "/dev/opl_minte"




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
int g_opl_minte_fd;


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

void wait_for_ipmux_intr(u32 *irq_pending)
{
  int len = 0;
  len = read(g_opl_minte_fd,irq_pending,4);
  if( len != 4){
    printf("read the irq pending error,len=%d,pending=0x%x\n",len,*irq_pending);
  }
}

void enable_ipmux_intr(void)
{
  int enable = 1;
  if(write(g_opl_minte_fd,&enable,4)){
    printf("enable ipmux interrupt failed\n");
  }  
}
void disable_ipmux_intr(void)
{
  int enable = 0;
  if(write(g_opl_minte_fd,&enable,4)){
    printf("disable ipmux interrupt failed\n");
  }
}
int open_minte(void)
{
  g_opl_minte_fd = open(OPL_MINTE_DEVICE_NAME,O_RDWR|O_SYNC);
  if(g_opl_minte_fd < 0){
    printf("open %s device failed\n",OPL_MINTE_DEVICE_NAME);
  }
  return 0;
}

int close_minte(void)
{
  int status = 0;
  if(g_opl_minte_fd > 0){
    status = close(g_opl_minte_fd);
  }
  return status;
}

/** @}*/
/** @}*/
