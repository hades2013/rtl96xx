/***************************************************************************
*Ipmux_update.c
*
*                   this file used for updating program with urat .
*
*****************************************************************************/

#include "ipmux_xdown.c"
#include <common.h>
#include <command.h>
#include "../../../../include/pdt_config.h"
#include "../../../../include/lw_config.h"

#define UPUBOOT			0
#define UPENV        	1
#define UPKER_1    		2
#define UPCFG        	3
#define UPKER_2     	4

#define tmpaddr     0xa1000000
int bootUpdateSimpleCheck(unsigned int fileaddr);
extern int update(int  choice);
extern int fw_head_check(unsigned int fileaddr);

int update(int  choice)
{
    int src;
    int fsize;
    int rcode;
    char cmd[50]="";
    
	printf("flash base addr:0x%x\n",CONFIG_SYS_FLASH_BASE);
    
	if((choice<0)||(choice>4))
	{
		printf("error: invalid choice %d",choice);
		return -1;
	}
   
	fsize=Xdown(tmpaddr);
//   	printf("fsize=0x%x\n",fsize);
   
	if(fsize<=0)
	{
		printf("update falied: load the binary file unsuccess...\n");
		return -1;
	}
      
	switch (choice){
    case UPUBOOT:
        {
			#if 0
            if(0!=bootUpdateSimpleCheck(tmpaddr))
            {
                return -1;
            }
			#endif
            if (fsize > UBOOT_MAX_SIZE) 
            {
                printf("\nERROR: The U-boot size is larger than 0x%x\n", UBOOT_MAX_SIZE);
                return -1;
            }
			
			printf("\n------ Erase flash \t------\n");
            sprintf(cmd, "sf erase 0x%x +0x%x", uboffset, UBOOT_MAX_SIZE);
            rcode=run_command(cmd,0);
            if (1!=rcode) return -1;
            
            printf("\n------ Program flash \t------\n");
         	sprintf(cmd, "sf write 0x%x 0x%x 0x%x", tmpaddr, uboffset, fsize);
            rcode=run_command(cmd,0);
            if (1!=rcode) return -1;
         	break;
		}

    case UPKER_1:
        {
			if (0 != fw_head_check(tmpaddr))
	        {
	            return -1;
	        }
	        src = tmpaddr;
	        if (fsize > OS_MAX_SIZE) 
	        {
	            printf("\nERROR: The Os update size is larger than 0x%x\n", OS_MAX_SIZE);
	            return -1;
	        }

			printf("\n------ Erase flash \t------\n");
	        sprintf(cmd, "sf erase 0x%x +0x%x", knl_1offset, fsize);
	        rcode=run_command(cmd,0);
	        if (1!=rcode) return -1;

			printf("\n------ Program flash \t------\n");
	        sprintf(cmd, "sf write 0x%x 0x%x 0x%x", tmpaddr, knl_1offset, fsize);
	        rcode=run_command(cmd,0);
			if (1!=rcode) return -1;

	        /*update app1,set bootflag to 0 so next time update app2 and boot from app1*/
	        #ifdef CONFIG_BOOT_MULTI_APP
	            setenv("bootflag","0");
	            saveenv();
	        #endif
	        
	        break;
        }
    case UPENV:
        {
			printf("\n------ Erase flash \t------\n");
			sprintf(cmd, "sf erase 0x%x +0x%x", envoffset, fsize);
			rcode=run_command(cmd,0);
			if (1!=rcode) return -1;

			printf("\n------ Program flash \t------\n");
			sprintf(cmd, "sf write 0x%x 0x%x 0x%x", tmpaddr, envoffset, fsize);
			rcode=run_command(cmd,0);
			if (1!=rcode) return -1;

			break;
        }
    
	case UPCFG:
        {
			printf("\n------ Erase flash \t------\n");
			sprintf(cmd, "sf erase 0x%x +0x%x", cfg_offset, fsize);
			rcode=run_command(cmd,0);
			if (1!=rcode) return -1;

			printf("\n------ Program flash \t------\n");
			sprintf(cmd, "sf write 0x%x 0x%x 0x%x", tmpaddr, cfg_offset, fsize);
			rcode=run_command(cmd,0);
			if (1!=rcode) return -1;
			break;
        }
#ifdef CONFIG_BOOT_MULTI_APP
    case UPKER_2:
        {
			if (0 != fw_head_check(tmpaddr))
	        {
	            return -1;
	        }
            if (fsize > OS_MAX_SIZE) 
            {
                printf("\nERROR: The Os update size is larger than 0x%x\n", OS_MAX_SIZE);
                return -1;
            }

			printf("\n------ Erase flash \t------\n");
            sprintf(cmd, "sf erase 0x%x +0x%x", knl_2offset, fsize);
			rcode=run_command(cmd,0);
			if (1!=rcode) return -1;

			printf("\n------ Program flash \t------\n");
			sprintf(cmd, "sf write 0x%x 0x%x 0x%x", tmpaddr, knl_2offset, fsize);
			rcode=run_command(cmd,0);
			if (1!=rcode) return -1;

			/*update app2,set bootflag to 1 so next time update app1 and boot from app2*/
			setenv("bootflag","1");
			saveenv();			
            
            break;
        }
#endif
	default:
		{
			printf("warning:unsupport!!!\n") ;      
			return -1;
		}
    }
	printf("------update successfull------\n") ;
	return 0;
   
}

int bootUpdateSimpleCheck(unsigned int fileaddr)
{ 
      if (0x100000ff!=*(unsigned int*)fileaddr){
	 printf("*******************************************************\n");
	 printf("                    chu              cuo                  le\n\n");
	 printf("                            IMAGE CHECK ERROR!\n\n");
	 printf("                    chu              cuo                  le\n");
	 printf("*******************************************************\n");
	 return -1;
       }
	 return 0;
}

int do_update(cmd_tbl_t *cmdtp,int flag,int argc, char *argv[])
{

    unsigned int  option;   
    int rcode = 0;
    int status;
	
    if (argc == 2) {
        option = simple_strtoul(argv[1], NULL, 16);

    #ifndef CONFIG_BOOT_MULTI_APP
        if (UPKER_2 == option)
        {
            printf("Multi app is not support!\n");
            return 0;
        }
    #endif
    }
    else if (argc == 1)
    {
    #ifdef CONFIG_BOOT_MULTI_APP
        if (0 == strcmp("0",getenv("bootflag")))
        {
            option = UPKER_2;
        }
        else if (0 == strcmp("1",getenv("bootflag")))
        {
            option = UPKER_1;
        }
    #else
        option = UPKER_1;
    #endif
    }
	
    puts ("## Ready for update ...\n");  

    status = update (option);
    
    if(status==0)
     {
   /*    printf("##debug:after Xdown, Start addr=%x\n",addr); */
       rcode=0;
     }
    else
      rcode=-1;
    return rcode;

}


U_BOOT_CMD(
  update,    2,    0,     do_update,
 "update binary file over serial line (xmodem) 0:uboot 2:app1 4:app2.",
 "update(choice),option appoint the choice you want to update\n"
);
