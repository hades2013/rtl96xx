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

#include "../include/opl_typedef.h"
/** @name Macro definition
 *  @{
 */
/*--------------------------Macro definition------------------------- */
#define OPL_MEM_DEVICE "/dev/opl_mem"
#define OPL_MEM_SIZE 0x300000




/** @}*/

/** @name type definition
 *  @{
 */
/*--------------------------type definition------------------------- */
typedef struct opl_mem_read_req_s{
  u32 addr;
  u32 size;
  char buf[1024];
}opl_mem_read_req_t;
typedef struct opl_mem_write_req_s{
  u32 addr;
  u32 unit;
  u32 size;
  u32 value;
}opl_mem_write_req_t;



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
u32 mem_read(char *reg_base,int offset);
void mem_write(char *reg_base, int offset, u32 value);
void opl_mem_main(int argc,char ** argv);
int opl_mem_init(int *opl_reg_initialized);
int opl_mem_exit(int *opl_reg_initialized);



/** @}*/
 

/** @name global variable and function exported
 *  @{
 */
/*------------------global variable and function exported-------------- */
/**HELP  */
char *opl_reg_base = NULL;
char *OPL_REG_USAGE ="\n\
opl_mem_test <-r|--read> <addr> <size>\n\
opl_mem_test <-w|--write> <addr> <unit> <value>\n\
opl_mem_test <--memset> <addr> <unit> <value> <size>\n\
unit: must be 1 | 2 | 4,which as byte,short or long.\n\
addr:must be from 0x80000000 to 0xffffffff.\n\
size: if unit is specified, the bytes = size*unit.\n\
all the numberic can be hex or decimal\n\
";



/** global fd of the "/dev/opl_reg"  */
int opl_mem_fd;

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
u32 mem_read(char *reg_base,int offset)
{
  u32 value = 0;
  if(offset%4){
    printf("the alignment is not 4 bytes \n");
    return -1;
  }
  value = *(volatile u32 *)(reg_base + offset);
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
void mem_write(char *reg_base, int offset, u32 value)
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
void opl_mem_main(int argc,char ** argv)
{
  int i = 0;
  int cmd = 0;
  u32 addr = 0;
  u32 size = 0;
  u32 val = 0;
  u32 unit = 0;
  u32 *value = NULL;
  
  if(!strcmp(argv[1],"--help")||!strcmp(argv[1], "-h")){
    printf(OPL_REG_USAGE);
    return;
  }
  if(argc < 3){
    printf("ERROR:the number of argument is not correct\n");
    printf(OPL_REG_USAGE);
    return;
  }
  if(!strcmp(argv[1],"--read")||!strcmp(argv[1], "-r")){ 
    opl_mem_read_req_t req;  
    if((argv[2][0] == '0') && ((argv[2][1] == 'x')||(argv[2][1] == 'X'))){
      /* TBD:should be check the ranged */
      sscanf(argv[2],"%x",&addr);
    }else{
      sscanf(argv[2],"%d",&addr);
    }
    
    if((argv[3][0] == '0') && ((argv[3][1] == 'x')||(argv[3][1] == 'X'))){
      /* TBD:should be check the ranged */
      sscanf(argv[3],"%x",&size);
    }else{
      sscanf(argv[3],"%d",&size);
    }
    addr = ((addr+3)>>2)<<2;
    size = ((size+3)>>2)<<2;
    req.addr = addr;
    req.size = size;
    memset(req.buf,0,1024);
    if(argc != 4){
      printf("ERROR:the number of argument is not correct\n");
      printf(OPL_REG_USAGE);
      return;
    }
    if(ioctl(opl_mem_fd,0,&req)){
      printf("read the addr:0x%x failed\r\n",addr);
      return;
    }
    size = size >> 2;
    value = (u32 *)req.buf;
    printf("\t\t+0\t\t+4\t\t+8\t\t+c\r\n");
    for(i = 0;i < size; i++){
      if(i % 4 == 0){
	printf("%8x:",addr);
	addr += 16;
      }
      printf("0x%08x\t",*value++);
      if(i % 4 == 3)
	printf("\r\n");
    }
    printf("\n");
  }else if(!strcmp(argv[1],"--write")||!strcmp(argv[1], "-w")){
    opl_mem_write_req_t req;
    if(argc != 5){
      printf("ERROR:the number of argument is not correct\n");
      printf(OPL_REG_USAGE);
      return;
    }
    if(argv[2][0] == '0' && ((argv[2][1] == 'x')||(argv[2][1] == 'X'))){
      /* TBD:should be check the ranged */
      sscanf(argv[2],"%x",&addr);
    }else{
      sscanf(argv[2],"%d",&addr);
    }

    if(argv[3][0] == '0' && ((argv[3][1] == 'x')||(argv[3][1] == 'X'))){
      /* TBD:should be check the ranged */
      sscanf(argv[3],"%x",&unit);
    }else{
      sscanf(argv[3],"%d",&unit);
    }
    if((unit != 1)&&(unit != 2)&&(unit != 4)){
      printf(OPL_REG_USAGE);
      return;
    }
    if(unit == 4){
      if(addr%4 != 0){
	printf("when unit=4,the address must be 4 alignment\n");
	return ;
      }
    }
    if(unit == 2){
      if(addr%2 != 0){
	printf("when unit=2,the address must be 2 aligment\n");
	return;
      }
    }
    if(argv[4][0] == '0' && ((argv[4][1] == 'x')||(argv[4][1] == 'X'))){
      /* TBD:should be check the ranged */
      sscanf(argv[4],"%x",&val);
    }else{
      sscanf(argv[4],"%d",&val);
    }
    req.addr = addr;
    req.unit = unit;
    req.value = val;
    req.size = size;
    printf("addr:0x%x\n",addr);
    printf("unit:0x%x\n",unit);
    printf("value:0x%x\n",val);
    printf("size:0x%x\n",size );
    if(ioctl(opl_mem_fd,1,&req)){
      printf("write date 0x%x to addr 0x%x failed\n",val,addr);
    }
  }else if(!strcmp(argv[1],"--memset")){
    opl_mem_write_req_t req;
    if(argc != 6){
      printf("ERROR:the number of argument is not correct\n");
      printf(OPL_REG_USAGE);
      return;
    }
    if(argv[2][0] == '0' && ((argv[2][1] == 'x')||(argv[2][1] == 'X'))){
      /* TBD:should be check the ranged */
      sscanf(argv[2],"%x",&addr);
    }else{
      sscanf(argv[2],"%d",&addr);
    }

    if(argv[3][0] == '0' && ((argv[3][1] == 'x')||(argv[3][1] == 'X'))){
      /* TBD:should be check the ranged */
      sscanf(argv[3],"%x",&unit);
    }else{
      sscanf(argv[3],"%d",&unit);
    }
    if((unit != 1)&&(unit != 2)&&(unit != 4)){
      printf(OPL_REG_USAGE);
      return;
    }
    if(unit == 4){
      if(addr%4 != 0){
	printf("when unit=4,the address must be 4 alignment\n");
	return ;
      }
    }
    if(unit == 2){
      if(addr%2 != 0){
	printf("when unit=2,the address must be 2 aligment\n");
	return;
      }
    }
    if(argv[4][0] == '0' && ((argv[4][1] == 'x')||(argv[4][1] == 'X'))){
      /* TBD:should be check the ranged */
      sscanf(argv[4],"%x",&val);
    }else{
      sscanf(argv[4],"%d",&val);
    }
    if(argv[5][0] == '0' && ((argv[5][1] == 'x')||(argv[5][1] == 'X'))){
      /* TBD:should be check the ranged */
      sscanf(argv[5],"%x",&size);
    }else{
      sscanf(argv[5],"%d",&size);
    }
    if(size == 0){
      printf("the size must be more than 0\n");
      printf(OPL_REG_USAGE);
      return ;
    }
    req.addr = addr;
    req.unit = unit;
    req.value = val;
    req.size = size;
    if(ioctl(opl_mem_fd,2,&req)){
      printf("memset from 0x%x to 0x%x + 0x%x as 0x%x failed\n",addr,addr,size,value);
      return;
    }
  }else{
    printf("cant support other operations\n");
    printf(OPL_REG_USAGE);
  }
}

/** description: use to open the /dev/opl_reg device and do mmap() it.
 *  @param *opl_reg_initialized, 1 indicated open device and mmap successfully.
 *  @retval 0 success
 *  @retval -x failed.

 *  @see 
 *  @deprecated 
 */
int opl_mem_init(int *opl_mem_initialized)
{
  opl_mem_fd = open(OPL_MEM_DEVICE,O_RDWR);
  if(opl_mem_fd<0){
    printf("ERROR:open device file failed\n",OPL_MEM_DEVICE);
    return -1;
  }
  *opl_mem_initialized = 1;
  return 0;
}
/** description: do close file and unmap the memory mmaped before.
 *  @param *opl_reg_initialized: if initialized,exit will close it and do unmap it.
 *  @retval 0 success
 *  @retval -1 failed

 *  @see 
 *  @deprecated 
 */
int opl_mem_exit(int *opl_mem_initialized)
{
  int ret = 0;
  if(*opl_mem_initialized == 1){
    *opl_mem_initialized = 0;
    ret = close(opl_mem_fd);
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
  opl_mem_init(&init);
  if(init){
    opl_mem_main(argc,argv);
  }
  opl_mem_exit(&init);
  return 0;
}

/** @}*/
/** @}*/
