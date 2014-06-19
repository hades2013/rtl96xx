#include <common.h>
#include <command.h>
#include <environment.h>

#define CONFIG_ETHADDR       "00:E0:4C:86:70:01"

#ifdef CONFIG_CMD_RESTORE
int do_restore ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    char ch;
    char *tmp1;
    char *tmp2;
    char mac_addr[20] = {0};
    char sn[20] = {0};
    
    if (2 != argc)
    {
        printf("Usage: restore {bootenv|manufacture}.\n");
        return 0;
    }

    if (0 == strcmp("bootenv",argv[1]))
    {
        printf("Are you sure to load default environment? Yes or No[Y/N]:");
        while(1)
        {
		/***** add code for Feed wtd by shipeng 2012-7-11*****/
 	  	//GPIO_WTD_FEED();
		if (tstc())
            {
                ch = getc();
                
                if('y' == ch||'n' == ch || 'Y' == ch||'N' == ch)
                {
                    putc(ch);
                    if('y' == ch||'Y' == ch)
                    {
                        tmp1 = getenv("SN");
                        strcpy(sn,tmp1);
                        tmp2 = getenv("ethaddr");
                        strcpy(mac_addr,tmp2);
						/*begin modify by shipeng 2013-03-21*/
						env_import((char *)default_environment, 2);
						/*end modify by shipeng 2013-03-21*/
						setenv("SN",sn);
                        setenv("ethaddr",mac_addr);
                        saveenv();
                        break;
                    }
                    else
                    {
                        printf("\r\n");
                        break;
                    }
                }
                else
                {
                    printf("\n\rYou have to chose 'Yes' or 'No' [Y/N]:");
                    continue;                            
                }
            }
        }
    }
    else if (0 == strcmp("manufacture",argv[1]))
    {
        printf("Are you sure to load default MAC and SN? Yes or No[Y/N]:");
        while(1)
        {
		/***** add code for Feed wtd by shipeng 2012-7-11*****/
 	  	//GPIO_WTD_FEED();
		if (tstc())
            {
                ch = getc();
                
                if('y' == ch||'n' == ch || 'Y' == ch||'N' == ch)
                {
                    putc(ch);
                    if('y' == ch||'Y' == ch)
                    {
                        setenv("ethaddr",CONFIG_ETHADDR);
                        setenv("SN",CONFIG_SN);
                        saveenv();
                        break;
                    }
                    else
                    {
                        printf("\r\n");
                        break;
                    }
                }
                else
                {
                    printf("\n\rYou have to chose 'Yes' or 'No' [Y/N]:");
                    continue;                            
                }
            }
        }
    }
    else
    {
        printf("Usage: restore {bootenv|manufacture}.\n");
    }
    
	return 0;
}


/* -------------------------------------------------------------------- */

U_BOOT_CMD(
	restore,	2,	1,	do_restore,
	"load default configuration to U-BOOT.",
	NULL
);

#endif	/* CONFIG_CMD_RESTORE */

