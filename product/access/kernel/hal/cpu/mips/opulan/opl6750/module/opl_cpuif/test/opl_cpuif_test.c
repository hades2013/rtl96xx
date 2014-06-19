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
#include <unistd.h>

#include <opl_typedef.h>
#include <opl_modif.h>
/** @name Macro definition
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define OPL_CPUIF_DEVICE "/dev/opl_cpuif"




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
int opl_cpuif_test_init(int *initialized);
void opl_cpuif_test_main(int argc,char **argv);
int opl_cpuif_test_exit(int *initialized);



/** @}*/


/** @defgroup

 *  @{
 */
/*----------------------local  function definition--------------------- */



int opl_cpuif_test_init(int *initialized)
{
  u32 value = 0;
  *initialized = 1;
  return 0;
}

int opl_cpuif_test_exit(int *initialized)
{
  int ret = 0;
  if(*initialized){
    *initialized = 0;
  }
  return ret;
}

void opl_cpuif_test_main(int argc,char **argv)
{
  u32 phy_addr = 0;
  void *buf = NULL;
  //opl_modules_init(&buf,0x1000);
  printf("do the opl modules init ok\n");
  //phy_addr = (u32)malloc_dma_rxbuf(0,0x4000);
  printf("the address is 0x%x\n",phy_addr);
}

int main(int argc,char **argv)
{
  int initialized = 0;
  opl_cpuif_test_init(&initialized);
  if(initialized){
    opl_cpuif_test_main(argc,argv);
  }
  opl_cpuif_test_exit(&initialized);
}


/** @}*/
/** @}*/
