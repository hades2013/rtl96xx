/** THE FORMAT is FOR DOXYGEN to Generate the document automatically

  *  @defgroup OPL_REG_TEST register mmap test module
  *  @copyright opulan.com
  *  @author stephanxu@sina.com
  *  @version v1.0
  *  @date July-07-2007
  *  @histroy
  *  author date     description
  *  -------------------------------------------------------------------
     hxu    July-07-2007  the initial version of the register mmap unit testing code
  *  @{
  */
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "../../include/opl_typedef.h"

#ifdef debug 
    #undef debug
#endif

#if 0
#define debug(x...) printf("  [DEBUG : %s @ %d]", __FUNCTION__, __LINE__);printf(x);
#else
#define debug(x...)
#endif

/** @name Macro definition
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define OPL_REG_DEVICE "/dev/opl_reg"
#define OPL_REG_SIZE 0x300000




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
u32 reg_read(char *reg_base,int offset);
void reg_write(char *reg_base, int offset, u32 value);
void opl_reg_main(int argc,char ** argv);
int opl_reg_init(int *opl_reg_initialized);
int opl_reg_exit(int *opl_reg_initialized);



/** @}*/


/** @name global variable and function exported
 *  @{
 */
/*------------------global variable and function exported-------------- */
/**HELP  */
char *opl_reg_base = NULL;
char *OPL_REG_USAGE = "\nreg usage:\n\
\topl_reg_test -r offset  or\n\
\topl_reg_test --read offset  or\n\
\topl_reg_test -w offset value   or\n\
\topl_reg_test --write offset value  \n\
Note: the offset and value can be decimal or hex!\n" ;


/** global fd of the "/dev/opl_reg"  */
int opl_reg_fd;

/** @}*/


/** @defgroup opl_regmmap function_description(opl_reg_test.c)

 *  @{
 */
/*----------------------local  function definition--------------------- */

/** description: use to read the value of (reg_base + offset);
 *  @param *reg_base: the base address of mmap().
 *  @param offset the offset of the reg_base.
 *  @return the value(u32) of the (reg_base +offset)
 *  @retval -1 indicate the offset is not alignment of 4. or the value is 0xffff,ffff.
 *  @retval

 *  @see
 *  @deprecated
 */
u32 reg_read(char *reg_base,int offset)
{
  u32 value = 0;
  if(offset%4){
    printf("the alignment is not 4 bytes \n");
    return -1;
  }

  value = *(volatile u32 *)(reg_base + offset);

  debug("reg_base 0x%x, offset 0x%x, address 0x%x, value 0x%x.\n", reg_base, offset, reg_base + offset, value);
  return value;
}
/** description: write the u32 value into regbase + offset,if the offset is not alignment of 4,
    it will return and print error message.
 *  @param *reg_base: the base address of mmap().
 *  @param offset the offset of the reg_base.
 *  @return void

 *  @see
 *  @deprecated
 */
void reg_write(char *reg_base, int offset, u32 value)
{
  if(offset%4){
    printf("the alignment is not 4 bytes \n");
    return ;
  }
  *(volatile u32 *)(reg_base + offset) = value;
}
/** description: the body unit testing of regmmap module ,it will be completed.
 *  @param int argc
 *  @param char **argv
 *  @return void


 *  @see
 *  @deprecated
 */
void opl_reg_main(int argc,char ** argv)
{
  int i = 0;
  int offset = 0;
  u32 value = 0;

  if(argc < 3){
    printf("ERROR:the number of argument is not correct\n");
    printf(OPL_REG_USAGE);
    return;
  }

  if((argv[2][0] == '0') && ((argv[2][1] == 'x')||(argv[2][1] == 'X'))){
    /* TBD:should be check the ranged */
    sscanf(argv[2],"%x",&offset);
  }else{
    sscanf(argv[2],"%d",&offset);
  }

  if(!strcmp(argv[1],"--read")||!strcmp(argv[1], "-r")){
    if(argc != 3){
      printf("ERROR:the number of argument is not correct\n");
      printf(OPL_REG_USAGE);
      return;
    }
    printf("%x\t0x%08x\n",offset,reg_read(opl_reg_base,offset));

  }if(!strcmp(argv[1],"--read4")||!strcmp(argv[1], "-r4")){
    if(argc != 3){
      printf("ERROR:the number of argument is not correct\n");
      printf(OPL_REG_USAGE);
      return;
    }
    printf("%x\t0x%08x\n",offset,reg_read(opl_reg_base,(offset*4)));

  }else if(!strcmp(argv[1],"--write")||!strcmp(argv[1], "-w")){
    if(argc != 4){
      printf("ERROR:the number of argument is not correct\n");
      printf(OPL_REG_USAGE);
      return;
    }
    if(argv[3][0] == '0' && ((argv[3][1] == 'x')||(argv[3][1] == 'X'))){
      /* TBD:should be check the ranged */
      sscanf(argv[3],"%x",&value);
    }else{
      sscanf(argv[3],"%d",&value);
    }
    reg_write(opl_reg_base,offset,value);
  }else if(!strcmp(argv[1],"--write4")||!strcmp(argv[1], "-w4")){
    if(argc != 4){
      printf("ERROR:the number of argument is not correct\n");
      printf(OPL_REG_USAGE);
      return;
    }
    if(argv[3][0] == '0' && ((argv[3][1] == 'x')||(argv[3][1] == 'X'))){
      /* TBD:should be check the ranged */
      sscanf(argv[3],"%x",&value);
    }else{
      sscanf(argv[3],"%d",&value);
    }
    reg_write(opl_reg_base,(offset*4),value);
  }
}

/** description: use to open the /dev/opl_reg device and do mmap() it.
 *  @param *opl_reg_initialized, 1 indicated open device and mmap successfully.
 *  @retval 0 success
 *  @retval -x failed.

 *  @see
 *  @deprecated
 */
int opl_reg_init(int *opl_reg_initialized)
{
  opl_reg_fd = open(OPL_REG_DEVICE,O_RDWR);
  if(opl_reg_fd<0){
    printf("ERROR:open device file failed\n",OPL_REG_DEVICE);
    return -1;
  }
  opl_reg_base = (char *)mmap(0,OPL_REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,opl_reg_fd,0);
  if(!opl_reg_base){
    printf("ERROR:mmap the opl register failed\n");
    close(opl_reg_fd);
    return -1;
  }

  *opl_reg_initialized = 1;
  return 0;
}
/** description: do close file and unmap the memory mmaped before.
 *  @param *opl_reg_initialized: if initialized,exit will close it and do unmap it.
 *  @retval 0 success
 *  @retval -1 failed

 *  @see
 *  @deprecated
 */
int opl_reg_exit(int *opl_reg_initialized)
{
  int ret = 0;
  if(*opl_reg_initialized == 1){
    *opl_reg_initialized = 0;
    munmap(opl_reg_base,OPL_REG_SIZE);
    ret = close(opl_reg_fd);
  }
  return ret;
}
/** description:the Entry of the unit testing for opl_regmmap module.
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
  opl_reg_init(&init);
  if(init){
    opl_reg_main(argc,argv);
  }
  opl_reg_exit(&init);
  return 0;
}

/** @}*/
/** @}*/
