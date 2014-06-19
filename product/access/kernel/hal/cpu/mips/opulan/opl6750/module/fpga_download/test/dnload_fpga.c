/** THE FORMAT is FOR DOXYGEN to Generate the document automatically

  *  @defgroup OPL_REG_TEST register mmap test module
  *  @copyright opulan.com
  *  @author stephanxu@sina.com
  *  @version v1.0
  *  @date July-07-2007
  *  @histroy 
  *  author	date 	 description
  *  -------------------------------------------------------------------
     hxu    July-07-2007  the initial version of the register mmap unit testing code
  *  @{
  */
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>

#include "../include/opl_typedef.h"
/** @name Macro definition
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define OPL_FPGA_DEVICE "/dev/opl_fpga"

/** @}*/

/** @name type definitiong
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
u32 fpga_read(char *reg_base,int offset);
void fpga_write(char *reg_base, int offset, u32 value);
void opl_fpga_main(int argc,char ** argv);
int opl_fpga_init(int *opl_fpga_initialized);
int opl_fpga_exit(int *opl_fpga_initialized);



/** @}*/
 

/** @name global variable and function exported
 *  @{
 */
/*------------------global variable and function exported-------------- */
/**HELP  */
char *opl_fpga_base = NULL;
char *USAGE = "\ndnload_fpga usage:\n\
\t dnload_fpga <file>\n";



/** global fd of the "/dev/opl_fpga"  */
int opl_fpga_fd;

/** @}*/


/** @defgroup opl_fpgammap function_description(opl_fpga_test.c)

 *  @{
 */
/*----------------------local  function definition--------------------- */

void opl_fpga_main(int argc,char ** argv)
{
  FILE *fp; 
  char *buf;
  int length = 0;
  int status = 0;
  if(argc > 2){
    printf("too much parameter...\n");
    printf(USAGE);
    return;
  }else if(argc < 2){
    printf("miss parameter....\n");
    printf(USAGE);
    return;
  }
  fp = fopen(argv[1],"r+b");
  if(!fp){
    printf("printf open the file %s failed,please check the file name\n",argv[1]);
    return;
  }
  printf("open file %s successfully\n",argv[1]);
  buf = (char *)malloc(0x100000);
  if(!buf){
    printf("malloc memory for store the image in user space failed\n");
    fclose(fp);
    return;
  }
  printf("malloc 1MB memory successfully\n");
  memset(buf,0,0x100000);
  length = fread(buf,1,0x100000,fp);
  if(length > 0){
    printf("file size is %d bytes\n",length);
    status = write(opl_fpga_fd,buf,length);
    if(status)
      printf("download fpga image failed,please try again ...\n");
    else
      printf("successed\n");
    free(buf);
    fclose(fp);
  }
}

/** description: use to open the /dev/opl_fpga device and do mmap() it.
 *  @param *opl_fpga_initialized, 1 indicated open device and mmap successfully.
 *  @retval 0 success
 *  @retval -x failed.

 *  @see 
 *  @deprecated 
 */
int opl_fpga_init(int *opl_fpga_initialized)
{
  opl_fpga_fd = open(OPL_FPGA_DEVICE,O_RDWR);
  if(opl_fpga_fd<0){
    printf("ERROR:open device file failed\n",OPL_FPGA_DEVICE);
    return -1;
  }
  *opl_fpga_initialized = 1;
  return 0;
}
/** description: do close file and unmap the memory mmaped before.
 *  @param *opl_fpga_initialized: if initialized,exit will close it and do unmap it.
 *  @retval 0 success
 *  @retval -1 failed

 *  @see 
 *  @deprecated 
 */
int opl_fpga_exit(int *opl_fpga_initialized)
{
  int ret = 0;
  if(*opl_fpga_initialized == 1){
    *opl_fpga_initialized = 0;
    ret = close(opl_fpga_fd);
  }
  return ret;
}
/** description:the Entry of the unit testing for opl_fpgammap module. 
 *  @param 
 *  @param 
 *  @retval 
 *  @retval 

 *  @see 
 *  @deprecated 
 */

int main(int argc, char **argv)
{
  int init = 0;
  opl_fpga_init(&init);
  if(init){
    opl_fpga_main(argc,argv);
  }
  opl_fpga_exit(&init);
  return 0;
}

/** @}*/
/** @}*/
