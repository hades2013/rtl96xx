/** THE FORMAT is FOR DOXYGEN to Generate the document automatically

  *  @defgroup MINTE_TEST  minte_test module
  *  @copyright opulan.com\n
  *  @author stephanxu@sina.com\n
  *  @version v1.0\n
  *  @date July-07-2007\n
  *  @histroy \n
  *  author  date        description
  *  \n-------------------------------------------------------------------
     \nhxu     July-07-2007 the initial version of testing the minte module.
  *  @{
  */
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../include/opl_typedef.h"
/** @name Macro definition
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define OPL_MINTE_DEVICE  "/dev/opl_minte"


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

/** the global fd of the "/dev/opl_minte" */
int opl_minte_fd;




/** @}*/
 

/** @name local variable declaration and definition
 *  @{
 */
/*--------------local  variable declaration and definition------------- */

int opl_minte_init(int *initialized);
void opl_minte_main(int argc,char **argv);
int opl_minte_exit(int *initialized);

/** @}*/
 

/** @name global variable and function exported
 *  @{
 */
/*------------------global variable and function exported-------------- */





/** @}*/


/** @defgroup minte_test function_description(opl_minte_test.c)

 *  @{
 */
/*----------------------local  function definition--------------------- */

/** description: it is used to open the /dev/opl_minte device with flag: RDWR|SYNC.
 *  @param *initialized is the output value to indicated it is initialized or not.
 *  @retval 0 success
 *  @retval -X failed.

 *  @see 
 *  @deprecated 
 */


int opl_minte_init(int *initialized)
{
  opl_minte_fd = open(OPL_MINTE_DEVICE,O_RDWR|O_SYNC);
  if(opl_minte_fd<0){
    printf("open the %s device failed\n",OPL_MINTE_DEVICE);
    return -1;
  }
  *initialized = 1;
  return 0;
}

/** description: close the opened file "/dev/opl_minte". 
 *  @param *initialized, that means close it or not depend on initialized value.then clear it.
 *  @retval 0 success
 *  @retval -X close the file failed.

 *  @see 
 *  @deprecated 
 */
int opl_minte_exit(int *initialized)
{
  int ret = 0;
  if(*initialized){
    ret = close(opl_minte_fd);
    *initialized = 0;
  }
  return ret;
}

/** description: it is the example of how to use it to check the value.
    and it is the body of unit testing programing,which will be completed.
 *  @param int argc
 *  @param char **argv
 *  @return void *  @return 

 *  @see 
 *  @deprecated 
 */
void opl_minte_main(int argc,char **argv)
{
  int len = 0;
  unsigned int irq_pending = 0;
  while(1){
    len = read(opl_minte_fd,&irq_pending,4);
    if(len == 4){
       printf("read the irq pending is correct. pending = 0x%x",irq_pending);
    }else{
      printf("read the irq pending error\n");
    }
  }
}
/** description: the testing entry for minte unit testing programming. 
 *  @param 
 *  @param 
 *  @retval  
 *  @retval 

 *  @see 
 *  @deprecated if the testing will be intergrated into main testing, it 
    will be removed.
 */

int main(int argc,char **argv)
{
  int initialized = 0;
  opl_minte_init(&initialized);
  if(initialized){
    opl_minte_main(argc,argv);
  }
  opl_minte_exit(&initialized);
  return 0;
}
/** @}*/
/** @}*/
