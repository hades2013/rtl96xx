/*
=============================================================================
     Header Name: ipmuxBootextend.c

     General Description:
        port U-boot for OPULAN boot
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
xWang                27/6/07            Initial Version
wwei                 27/6/07            Initial Version
*/
/* include */
#include <common.h>
#include <configs/luna.h>
#include <md5_onu.h>

#ifdef OPULAN_BOOT_MENU_MODE
#include <opulan_bootextend.h>

/* defines */
static char erase_seq[] = "\b \b";      /* erase sequence   */
static char tab_seq[] = "        "; /* used to expand TABs  */

#define nfsrootargs "setenv bootargs root=$(root) rootfstype=squashfs ip=$(ipaddr) mac=$(ethaddr) nfsroot=$(nfsserver):$(nfspath) mem=$(mem)M nofpu cpu_clk=$(cpu_clk)"
#define emmrootargs "setenv bootargs ip=$(ipaddr) mac=$(ethaddr) root=$(root) rootfstype=squashfs load_ramdisk=1 ramdisk_size=$(ramdisk_size) mem=$(mem)M nofpu console=ttyS0,$(baudrate)n8"

#define KEY_MASK_SURPORT 1
#ifdef KEY_MASK_SURPORT
#define KEY_MASK(x)     (x|0x100)
#define KEY_MASK_F1     KEY_MASK(1)
#define KEY_MASK_F2     KEY_MASK(2)
#define KEY_MASK_F3     KEY_MASK(3)
#define KEY_MASK_F4     KEY_MASK(4)
#define KEY_MASK_F5     KEY_MASK(5)
#define KEY_MASK_F6     KEY_MASK(6)
#define KEY_MASK_F7     KEY_MASK(7)
#define KEY_MASK_F8     KEY_MASK(8)
#define KEY_MASK_F9     KEY_MASK(9)
#define KEY_MASK_F10    KEY_MASK(0X0A)
#define KEY_MASK_F11    KEY_MASK(0x0B)
#define KEY_MASK_F12    KEY_MASK(0x0C)
#define KEY_MASK_UP     KEY_MASK(0x0D)
#define KEY_MASK_DOWN   KEY_MASK(0x0E)
#define KEY_MASK_LEFT   KEY_MASK(0x0F)
#define KEY_MASK_RIGH   KEY_MASK(0x10)
#define KEY_MASK_PGUP   KEY_MASK(0x11)
#define KEY_MASK_PGDN   KEY_MASK(0x12)
#define KEY_MASK_HOME   KEY_MASK(0x13)
#define KEY_MASK_END    KEY_MASK(0x14)
#define KEY_MASK_INSERT KEY_MASK(0X15)
#define KEY_MASK_DEL    KEY_MASK(0x16)
#define KEY_ESC 0x1B
#define KEY_max 0x100

#define PRODUCT_VER_STR_LEN_MAX 36
#define FMW_HEADLEN 0x60

struct fmw_hdr_t
{
	unsigned int headlen;   // 文件头的长度
	unsigned int imgsize;   // 整个文件大小，包含头
	unsigned int checksum;  // 校验值，包含头，算法：4字节相加，结果为0。
	unsigned int timestamp; // 升级文件制作的时间
	unsigned int kernellen; // 内核的长度
	unsigned int fslen;     // 文件系统的长度
			        // 因此文件必须4字节对齐，如果不对齐，则用0填充。
	
	unsigned char _model; // 产品类型 ER5200
	unsigned char _major; // 产品V版本号 V100
	unsigned char _minor; // 产品R版本号 R001
	unsigned char _patch; // 产品D版本号 D001
	unsigned char _limit; // 产品受限发布的版本号 L00
	
	
	unsigned char kernel_type;   // 软件类型：1：内核； 0xff:空
	unsigned char _kernel_major; // 内核V版本号 V100，  无相关信息时为0xff
	unsigned char _kernel_minor; // 内核R版本号 R001，  无相关信息时为0xff
	unsigned char _kernel_patch; // 内核D版本号 D001，  无相关信息时为0xff
	unsigned char _kernel_limit; // 内核受限发布的版本号 L00，  无相关信息时为0xff
	
	unsigned char fs_type;   // 软件类型：1：文件系统； 0xff:空
	unsigned char product_version_str[PRODUCT_VER_STR_LEN_MAX]; 
	
	unsigned char addition[FMW_HEADLEN-0x47]; // 保留，并填充0到固定长度
	
} __attribute__ ((aligned(1), packed));

void read_count(int *count, char *ch);
int get_key_press(char key);
#endif

/* local varialbes */
static MENU_ITEM_t * pNow;
static char oplbootConsoleBuffer[CONFIG_SYS_CBSIZE];
static MENU_ITEM_t menuMain[];
static MENU_ITEM_t netUpdateMenu[];

/* function declarations */
void oplbootMainloop(void);
void oplbootInitial(void);
void oplbootMenuShow(MENU_ITEM_t *pmenu);
void oplbootParseInput(MENU_ITEM_t *pnemu, int *pinput);
void oplbootMenuChange(int  value);
void oplbootDataType(void);
void oplbootNameSet( void );
void oplbootLoadSystem(void);
void oplbootSaveConfig(void);
int oplReadline(char *repeat_last);
void oplCommandLineInterface (void);
int MenuReadline(int num, int *pInput);
int bootPwCheck(int flag);
void menuPasswdmod(void);
void SetAppRestoreFlag(void);

static MENU_ITEM_t menuMain[]={                                                                                                
    /*ID                                   					next               		    command             	    envVarName         	  promt                         				type              		  header                                      */    
    {MENU_MAIN_REBOOT                    , OPL_NULL,          "reset",            OPL_NULL,          OPL_NULL                      ,OPL_NULL        ,"Reboot"                                    },
    {MENU_MAIN_BOOT_SYSTEM               , OPL_NULL,          OPL_NULL,           OPL_NULL,          OPL_NULL                      ,"boot"          ,"Boot System"                               },
    {MENU_MAIN_MODIFY_PASSWD             , OPL_NULL,          OPL_NULL,           OPL_NULL,          OPL_NULL                      ,"passwd"        ,"Modify main menu password"                    },
    {MENU_MAIN_APP_RESTORE_DEFAULT       , OPL_NULL,          OPL_NULL,           OPL_NULL,          OPL_NULL                      ,"restore"       ,"Restore default configurations of application"},
    {MENU_MAIN_NETUPDATE_SUBMENU         , netUpdateMenu,     OPL_NULL,           OPL_NULL,          OPL_NULL                      ,OPL_NULL        ,"Net Update Submenu"                             },
    {MENU_MAIN_XMODEM_SUBMENU            , OPL_NULL,          "update",           OPL_NULL,          OPL_NULL                      ,OPL_NULL        ,"Xmodem Update APP"                             },
 //   {MENU_MAIN_FLBOOT_SUBMENU             , flashBootMenu,     OPL_NULL,           OPL_NULL,          OPL_NULL                      ,'\0'            ,"Flash Boot Submenu"                             },
    {MENU_MAIN_SAVE_CHANGES              , OPL_NULL,          "saveenv",          OPL_NULL,          OPL_NULL                      ,"save"          ,"Save Changes"                              },
    {MENU_NULL                           , OPL_NULL,          OPL_NULL,           OPL_NULL,          "     Main Menu    "          ,OPL_NULL        ,OPL_NULL                                    }
};                                                                                                                                                     

static MENU_ITEM_t netUpdateMenu[]={       
    /*ID                                   					next               		    command             	    envVarName,        		promt,                        			      type             	    header                                      */    
    {MENU_FLASH_EXIT                     , menuMain,          OPL_NULL,           OPL_NULL            , OPL_NULL                      ,OPL_NULL      ,"Return To Main Menu"                       },
    {MENU_FLASH_OS_FILE_NAME             , OPL_NULL,          OPL_NULL,           "flash_file_name"   ,"INPUT a new file name"        ,"name"        ,"File Name"                                 },
    {MENU_OS1_UPDATE                     , OPL_NULL,          "uuapp",             OPL_NULL            , OPL_NULL                      ,OPL_NULL      ,"Update OS From Network"                    },
    {MENU_NULL                           , OPL_NULL,          OPL_NULL,           OPL_NULL            ,"Net Update Submenu"           ,OPL_NULL      ,OPL_NULL                                    }
};

/* function definitons */
/*******************************************************************************
*
* oplbootMainloop
*
* DESCRIPTION:
*   This function is the entry of the opl boot menu function.
*    -----------------       ---------------        ---------------
*   |initial menu item|  -> |prompt optation|  ->  |input a command|   --
*    -----------------       ---------------        ---------------      |
*                                 /\                                     |
*                                 |      ----------------                |
*                                  ---- |strore configure|   <-----------
*                                        ----------------
*
* INPUT:
*   N/A
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootMainloop(void)
{
    int input = 0;
    int *pInput = &input;

    /*initial menu item*/
    oplbootInitial();

	/*
	* Main Loop for Monitor Command Processing
	*/
    for (;;)
    {
        oplbootMenuShow(pNow);
        oplbootParseInput(pNow ,pInput); 	
        oplbootMenuChange(*pInput);	
    }
}

/*******************************************************************************
*
* oplbootStructInitial
*
* DESCRIPTION:
*   This function initialize MENU_ITEM_t menuMain[], menuBootConfig[], menuEtherConfig[], menuSerialConfig[]
*   menuFlashMenu[], menuBootFlashType[].
*
* INPUT:
*   N/A
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootInitial(void)
{
    char buf[20];
    pNow = menuMain;

//   printf("pnow = 0x%x\n",pNow);

    if( OPL_NULL == getenv("flash_boot_name"))
    {
        setenv("flash_boot_name","u-boot.bin");
    }
}

/*******************************************************************************
*
* oplbootMenuShow
*
* DESCRIPTION:
*   This function show oplboot menu.
*
* INPUT:
*   pmenu      - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootMenuShow(MENU_ITEM_t *pMenu)
{
    char *pValue;
    int i =0;  
	
    for (;pMenu->header != OPL_NULL; pMenu++)
    {
        i++;
    }
    //printf("\n============================<%-18s >=============================\n", pMenu->promt);
    
    printf("\n*****************************************************************\n");
    printf("*                      %-21s                    *\n",pMenu->promt);
    printf("*****************************************************************\n");
    for (;i > 0; i--)
    {
        pMenu--;
    }
	
	/*
	 * print every menu line
	 */ 
    while (OPL_NULL != pMenu->header)
    {
        pValue = getenv(pMenu->envVarName);
		
        if (OPL_NULL != pValue)
        { 
            printf("\r<%d> %-35s    [%s]\n", i, pMenu->header,pValue);
        }
        else if (OPL_NULL != pMenu->envVarName)
        {
            printf("\r<%d> %-35s    [current]\n", i, pMenu->header);
        }
        else
        {    
            printf("\r<%d> %-35s    \n", i, pMenu->header);
        }

        pMenu++;
        i++;
    }
    for(; i<=7; i++)
    {
        printf("\n");
    }
    //printf("==============================================================================\n");

}

/*******************************************************************************
*
* oplbootParseInput
*
* DESCRIPTION:
*   This function oplboot parse input, do some work associate with operate Menu.
*
* INPUT:
*   p           - 
*   pinput      - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootParseInput(MENU_ITEM_t *pMenu, int *pInput)
{
    int num = 0;
    char ch;
    int value = 0;
    
    while (OPL_NULL != pMenu->header)
    {
        num++;
        pMenu++;
    }
    num--;

    printf("\nEnter your choice <0-%d>: ",num);
    MenuReadline(num,pInput);
}

/*******************************************************************************
*
* oplbootMenuChange
*
* DESCRIPTION:
*   This function is to select configure target,and now find the target *pcnt
*   and now do the configure.meanwhile change the prompt menu.
* INPUT:
*   value       -user input 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A 
*
* SEE ALSO:
*/
void oplbootMenuChange(int  value)
{
    int i = 0;
    char ch;
    int iflag = 0;
    
    /****** Exit to upper Menu *********/
    if (value == 0)
    {
        if (OPL_NULL != pNow->next)
        {
            pNow = pNow->next;
        }
        else
        {       
            run_command("reset",0);
        }
    }
    else if (MENU_MAIN_COMMAND_LINE_INTERFACE == value)
    {
        printf("\n\rAre you sure to test your equipment? Yes or No[Y/N]:");
        while(1)
        {		
		if (tstc())
            {
                ch = getc();
                
                if('y' == ch||'n' == ch || 'Y' == ch||'N' == ch)
                {
                    putc(ch);
                    if('y' == ch||'Y' == ch)
                    {
                        printf("\r\n");
                        for(;;)
                        {
                            iflag = bootPwCheck(1);
                            if (iflag&&(i<MAX_PASSWD_ERROR_TIMES))
                            {
                                i++;
                            }
                            else if (iflag&&(i == MAX_PASSWD_ERROR_TIMES))
                            {
                                printf("Password error!\n");
                                break;
                            }
                            else
                            {
                                oplCommandLineInterface();
                                break;
                            }
                        }
                        break;
                    }
                    else
                    {
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
    /****** Enter to the down Menu ******/
    else
    {
        for (i=0; i<value; i++)
        {
            pNow++;
        }
		
        if (OPL_NULL == pNow->next)
        {
            /* do with date */
            oplbootDataType();
                
            if (OPL_NULL != pNow->command)
            {
                run_command(pNow->command,0);                
            }
            for (; i>0; i--)
            {
                pNow--;
            }
        }
        else
        {
            pNow = pNow->next;
        }
    }
}
/*******************************************************************************
*
* oplbootDataType
*
* DESCRIPTION:
*   This function do config set.
*
* INPUT:
*   N/A 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootDataType(void)
{ 
    if( OPL_NULL != pNow->promt)
    {
        printf("%s, kick 'ENTER' to canel : ",pNow->promt);
    }
	/*modify by shipeng 2013-03-21*/
	if(OPL_NULL != pNow->type)
	{
    	switch(*(pNow->type))
    	{
     /* case 'boot' */
        case 'b':
            oplbootLoadSystem();
            break;
     /* case 'save' */
        case 's':
            oplbootSaveConfig();
            break;
     /*case 'passwd'*/
        case 'p':
            menuPasswdmod();
            break;
    /*case "restore"*/
        case 'r':
            SetAppRestoreFlag();
            break;
    /* case 'name'*/
        case 'n':
            oplbootNameSet();
            break;
	/*delete by shipeng 2013-03-21*/
     /* case '\0'*/
        //case '\0':
        //    break;
        default:
            /*printf("ERROR! type'%s' is no disposal function for the 'type' defined in MENU_ITEM_t. \n",pNow->promt);*/
            break;
    	}
	}
}

/*******************************************************************************
*
* oplCommandLineInterface
* DESCRIPTION:
*   store name for struct MENU_ITEM_t.
*
* INPUT:
*   N/A
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplCommandLineInterface (void)
{   
    printf("\n==============================================================================\n\n");
    printf("Note: the environment variable 'bootargs' set here will be changed unexpected \n");
    printf("in menu interface. The other environment variables here are shared in the menu \ninterface.\n");
    
    for(;;)
    {
        printf(CONFIG_SYS_PROMPT);
        oplReadline(0);

        if('e'==oplbootConsoleBuffer[0] && 'x'==oplbootConsoleBuffer[1] && 'i'==oplbootConsoleBuffer[2] && 't'==oplbootConsoleBuffer[3])
        {
            break;
        }
        else if(( 'h'==oplbootConsoleBuffer[0] && 'e'==oplbootConsoleBuffer[1] && 'l'==oplbootConsoleBuffer[2] && 'p'==oplbootConsoleBuffer[3] && '\0'==oplbootConsoleBuffer[4])||'?'==oplbootConsoleBuffer[0])
        {
            run_command("help",0);
            printf("exit        - exit to menu interface.\n");
        }
        else
        {
            run_command(oplbootConsoleBuffer,0);
        }
    }
    oplbootSaveConfig();
}

/*******************************************************************************
*
* oplbootNameSet
*
* DESCRIPTION:
*   store name for struct MENU_ITEM_t.
*
* INPUT:
*   N/A
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootNameSet( void )
{ 
    oplReadline(0);
    if( '\0' != oplbootConsoleBuffer[0] )
    {
        setenv( pNow->envVarName,oplbootConsoleBuffer );
    }   
    
    /* save bootargs */  
    oplbootSaveConfig();
}

/*******************************************************************************
*
* oplbootSaveConfig
*
* DESCRIPTION:
*   This function do serial bau.
*
* INPUT:
*   N/A        - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootSaveConfig(void)
{
	run_command(emmrootargs,0);
}
/*******************************************************************************
*
* oplbootLoadSystem
*
* DESCRIPTION:
*   This function do serial bau.
*
* INPUT:
*   N/A        - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootLoadSystem(void)
{
    struct fmw_hdr_t app_head;
    ulong  addr;
	char cmd[50] = "";
	    
#ifdef CONFIG_BOOT_MULTI_APP
	if(0 == strcmp("0",getenv("bootflag")))
	{
    	printf("Loading linux from FLASH_OS1...\n");
		sprintf(cmd, "sf read 0x%x 0x%x 0x%x", &app_head, knl_1offset, sizeof(app_head));
		run_command(cmd,0);
		if(app_head.headlen != FMW_HEADLEN)
			goto no_os;
		run_command(boot_os1_cmd, 0);
    }
	else
	{
        printf("Loading linux from FLASH_OS2...\n");
		sprintf(cmd, "sf read 0x%x 0x%x 0x%x", &app_head, knl_2offset, sizeof(app_head));
		run_command(cmd,0);
		if(app_head.headlen != FMW_HEADLEN)
			goto no_os;
        run_command(boot_os2_cmd, 0);
    }
#else
    printf("Loading linux from FLASH_OS....\n");
	sprintf(cmd, "sf read 0x%x 0x%x 0x%x", &app_head, knl_1offset, sizeof(app_head));
	run_command(cmd,0);
	if(app_head.headlen != FMW_HEADLEN)
		goto no_os;
	run_command(boot_os1_cmd, 0);
#endif

no_os:
	printf("None OS have been founded, please configure 'Boot Config' at first before boot system.\n");
}

void menuPasswdmod(void)
{
    int i = 0;
	char NullChar = '\0';
    char *s;
    char buf[32] = {0};

    for (;;)
    {
        printf("old password: ");
        PwReadline(0);
        if (OPL_NULL == (s = getenv("menupasswd")))
        {
            s = &NullChar;
        }
        if (0 == strcmp(oplbootConsoleBuffer,s))
        {
            printf("new password: ");
            memset(oplbootConsoleBuffer,0,CONFIG_SYS_CBSIZE);
            PwReadline(0);
            strncpy(buf,oplbootConsoleBuffer,sizeof(oplbootConsoleBuffer));
            printf("confirm: ");
            memset(oplbootConsoleBuffer,0,CONFIG_SYS_CBSIZE);
            PwReadline(0);
            if (0 == strcmp(oplbootConsoleBuffer,buf))
            {
                if ('\0' == oplbootConsoleBuffer[0])
                {
                    run_command("setenv menupasswd",0);
                    printf("Current password has been changed successfully!\n");
                    break;
                }
                setenv("menupasswd",oplbootConsoleBuffer);
                printf("Current password has been changed successfully!\n");
                break;
            }
            else
            {
                printf("fail to change current password!\n");
                break;
            }
            
        }
        else if (i == MAX_OLD_PASSWD_ERROR_TIMES)
        {
            printf("old password wrong!\n");
            break;
        }
        else
        {
            i++;
            continue;
        }
    }
    
}

void SetAppRestoreFlag(void)
{
    setenv("restore_default","1");
    saveenv();
}

/*******************************************************************************
*
* delete_char
*
* DESCRIPTION:
*   This function delete_char.
*
* INPUT:
*   buffer      - 
*   p           - 
*   colp        - 
*   np          - 
*   plen        - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   char *      - 
*
* SEE ALSO:
*/
static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen)
{
    char *s;

    if (*np == 0) {
        return (p);
    }

    if (*(--p) == '\t') {           /* will retype the whole line   */
        while (*colp > plen) {
            puts (erase_seq);
            (*colp)--;
        }
        for (s=buffer; s<p; ++s) {
            if (*s == '\t') {
                puts (tab_seq+((*colp) & 07));
                *colp += 8 - ((*colp) & 07);
            } else {
                ++(*colp);
                putc (*s);
            }
        }
    } else {
        puts (erase_seq);
        (*colp)--;
    }
    (*np)--;
    return (p);
}
/*******************************************************************************
*
* oplReadline
*
* DESCRIPTION:
*   This function read a line.
*
* INPUT:
*   repeat_last - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   int         -   number of read characters
*                   -1 if break
*                   -2 if timed out
*                   -3 if print last command    
*
* SEE ALSO:
*/
int oplReadline(char *repeat_last)
{
    char   *p = oplbootConsoleBuffer;
    int n = 0;              /* buffer index     */
    int plen = 0;           /* prompt length    */
    int col;                /* output column cnt    */
    char    c;

    if (repeat_last)
    {
        p=oplbootConsoleBuffer+strlen(oplbootConsoleBuffer);
    }
    
    col = plen;

    for (;;) {
#ifdef CONFIG_BOOT_RETRY_TIME
        while (!tstc()) {   
            /* while no incoming data */
            /***** add code for Feed wtd by shipeng 2012-7-11*****/
		//GPIO_WTD_FEED();
		if (retry_time >= 0 && get_ticks() > endtime)
                return (-2);    /* timed out */
        }
#endif
        /* Trigger watchdog, if needed */
        /*
        WATCHDOG_RESET();       
        */

#ifdef CONFIG_SHOW_ACTIVITY
        while (!tstc()) {
		extern void show_activity(int arg);
            show_activity(0);
        }
#endif
        while (!tstc()) {
        }
        
        c = getc();
        
        //printf("key=0x%x\n",c);
        
        /*
         * Special character handling
         */
        switch (c) {
        case '\r':              /* Enter        */
        case '\n':
            *p ='\0' ;
            puts ("\r\n");
            return (p - oplbootConsoleBuffer);

        case '\0':              /* nul          */
            continue;
        case 0x60:
            return (-3);
        
        case 0x03:              /* ^C - break       */
            oplbootConsoleBuffer[0] = '\0'; /* discard input */
            puts ("\r\n");
            return (-1);

        case 0x15:              /* ^U - erase line  */
            while (col > plen) {
                puts (erase_seq);
                --col;
            }
            p = oplbootConsoleBuffer;
            n = 0;
            continue;

        case 0x17:              /* ^W - erase word  */
            p = delete_char(oplbootConsoleBuffer, p, &col, &n, plen);
            while ((n > 0) && (*p != ' ')) {
                p=delete_char(oplbootConsoleBuffer, p, &col, &n, plen);
            }
            continue;

        case 0x08:              /* ^H  - backspace  */
        case 0x7F:              /* DEL - backspace  */
            p=delete_char(oplbootConsoleBuffer, p, &col, &n, plen);
            continue;
        case KEY_ESC:
            get_key_press(c);
            continue;

        default:
            /*
             * Must be a normal character then
             */
            if (n < CONFIG_SYS_CBSIZE-2) {
                if (c == '\t') {    /* expand TABs      */
#ifdef CONFIG_AUTO_COMPLETE
                    /* if auto completion triggered just continue */
                    *p = '\0';
                    if (cmd_auto_complete(CONFIG_SYS_PROMPT, oplbootConsoleBuffer, &n, &col)) {
                        p = oplbootConsoleBuffer + n;   /* reset */
                        continue;
                    }
#endif
                    puts (tab_seq+(col&07));
                    col += 8 - (col&07);
                } else {
                    if  ((c>=0x20) && (c<=0x7F))
                    {   
                    ++col;      /* echo input       */
                    
                    putc (c);
                    }
                }
                *p++ = c;
                ++n;
            } else {            /* Buffer full      */
                putc ('\a');
            }
        }
    }
}

int MenuReadline(int num, int *pInput)
{
    char   *p = oplbootConsoleBuffer;
    int n = 0;              /* buffer index     */
    int plen = 0;           /* prompt length    */
    int col;                /* output column cnt    */
    char    c;

    col = plen;

    for (;;) 
    {			
        c = getc();
		
        switch (c) 
        {
            case '\r':              /* Enter        */
            case '\n':
                if (0 == col)
                {
                    printf("\nEnter your choice <0-%d>: ",num);
                    oplbootConsoleBuffer[0] = '\0';
                    continue;
                }
                *pInput = oplbootConsoleBuffer[0]-'0';
                *p ='\0' ;
                puts ("\r\n");
                return (p - oplbootConsoleBuffer);

            case 0x08:              /* ^H  - backspace  */
            case 0x7F:              /* DEL - backspace  */
                p=delete_char(oplbootConsoleBuffer, p, &col, &n, plen);
                continue;
            case KEY_ESC:
                get_key_press(c);
                continue;            
            case 0x14:
                *pInput = MENU_MAIN_COMMAND_LINE_INTERFACE;
                return 0;
            default:
                /* Must be a normal character then*/
                if ((n < CONFIG_SYS_CBSIZE-2)&&(col == 0))
                {
                    if ((c>='0')&&(c<=('0'+num)))
                    {   
                        ++col;      /* echo input       */

                        putc (c);
                        
                        //*pInput = (c-'0');
                        
                        *p++ = c;
                        ++n;
                    }                    
                } 
                else /* Buffer full      */
                {            
                    putc ('\a');
                }
        }
    }
}

void read_count(int *count, char *ch)
{
    int  sum = 0;
    char c = 0;

    c = *ch;
    for(;;)
    {
        sum = ((sum*10) + (c-'0'));
        c = getc();
        if(!((c>='0') && (c<='9')))
        {
            break;
        }
    }
    *ch = c;
    *count = sum;    
}

int get_key_press(char key)
{
    char key_press;
    int count;

    switch(key)
    {
        case KEY_ESC:
            repeat:
            key_press = getc();
            switch(key_press)
            {
                case '[':
                    key_press = getc();
                    if((key_press >= '0') && (key_press <= '9'))
                    {
                        read_count(&count , &key_press);
                        if('~' == key_press)
                        {
                            switch(count) 
                            {
                                case 1:
                                    return KEY_MASK_INSERT;
                                case 2:
                                    return KEY_MASK_HOME;
                                case 3:
                                    return KEY_MASK_UP;
                                case 4:
                                    return KEY_MASK_DOWN;
                                case 5:
                                    return KEY_MASK_END;
                                case 6:
                                    return KEY_MASK_PGDN;
                                case 11:
                                    return KEY_MASK_F1;
                                case 12:
                                    return KEY_MASK_F2;
                                case 13:
                                    return KEY_MASK_F3;
                                case 14: 
                                    return KEY_MASK_F4;
                                case 15:
                                    return KEY_MASK_F5;
                                case 17:
                                    return KEY_MASK_F6;
                                case 18:
                                    return KEY_MASK_F7;
                                case 19:
                                    return KEY_MASK_F8;
                                case 20:
                                    return KEY_MASK_F9;
                                case 21:
                                    return KEY_MASK_F10;
                                case 23:
                                    return KEY_MASK_F11;
                                case 24:    
                                    return KEY_MASK_F12;
                                default:
                                    return (int)key_press;    
                            }
                        }
                    }
                    else
                    {
                        switch(key_press)
                        {
                            case 'A':
                                return KEY_MASK_UP;
                            case 'B':
                                return KEY_MASK_DOWN;
                            case 'C':
                                return KEY_MASK_RIGH;
                            case 'D':  
                                return KEY_MASK_LEFT;
                            case 'F':
                                return KEY_MASK_HOME;
                            case 'H':  
                                return KEY_MASK_END;          
                            default:
                                return (int)key_press;
                        }
                    }
                    case 'O':
                        key_press = getc();
                        switch(key_press)
                        {
                            case 'P':
                                return KEY_MASK_F1;
                            case 'Q':
                                return KEY_MASK_F2;
                            case 'R':
                                return KEY_MASK_F3;
                            case 'S':
                                return KEY_MASK_F4;
                            default:
                                return (int)key_press; 
                        }
                    case KEY_ESC:
                        goto repeat;
                    default:
                        return (int)key_press;
            }
            default:
                return (int)key_press;
    }
}
int bootPwCheck(int flag)
{
    int i = 0;
    int j = 0;
    int num = 0;
    int result1 = 0;
    int result2 = 0;
    char *pPasswd;
    char *pMac;
    UCHAR output[16];
    char str[32];
    char cMac_addr[17];

    memset(output,0,sizeof(output));
    memset(str,0,sizeof(str));
    memset(cMac_addr,0,sizeof(cMac_addr));
    
    printf("Password: ");
    PwReadline(0);
	
    if (MAX_PASSWD_LEN < strlen(oplbootConsoleBuffer))
    {
        printf("The largest length of password is 32!\n");
        return 1;
    }
    
    if(1 == flag)
    {
        pPasswd = getenv("bootpasswd");//厂测密码检查
    }
    else if (0 == flag)
    {
        pPasswd = getenv("menupasswd");//主界面密码检查
    }
    
    if ((NULL == pPasswd)&&('\0' == oplbootConsoleBuffer[0])) //modify by dengjian 2012-07-23 for u-boot ST
    {
        result1 = 0;
    }
    else
    {
    	if(NULL == pPasswd)
			result1 = 1;
		else
        	result1 = strcmp(oplbootConsoleBuffer,pPasswd);
    }

    pMac = getenv("ethaddr");
    strcpy(cMac_addr,pMac);
    MD5Calc(output,(UCHAR *)cMac_addr,sizeof(cMac_addr));//超级密码md5计算
    /*md5输出的16进制数转换为字符*/
    for(i=0;i<16;i++)
    {
        num=output[i]/16;
        switch(num)
        {
            case 15:str[j++]='f';break;
            case 14:str[j++]='e';break;
            case 13:str[j++]='d';break;
            case 12:str[j++]='c';break;
            case 11:str[j++]='b';break;
            case 10:str[j++]='a';break;
            default:str[j++]=num+48;break;
        }
        num = output[i]%16;
        switch(num)
        {
            case 15:str[j++]='f';break;
            case 14:str[j++]='e';break;
            case 13:str[j++]='d';break;
            case 12:str[j++]='c';break;
            case 11:str[j++]='b';break;
            case 10:str[j++]='a';break;
            default:str[j++]=num+48;break;
        }
    }
    result2 = memcmp(oplbootConsoleBuffer,str,sizeof(str));
    if ((0 == result1)||(0 == result2))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int PwReadline(char *repeat_last)
{
    char   *p = oplbootConsoleBuffer;
    int n = 0;              /* buffer index     */
    int plen = 0;           /* prompt length    */
    int col;                /* output column cnt    */
    char    c;

    if (repeat_last)
    {
        p=oplbootConsoleBuffer+strlen(oplbootConsoleBuffer);
    }
    
    col = plen;

    for (;;) {
#ifdef CONFIG_BOOT_RETRY_TIME
        while (!tstc()) {   
            /* while no incoming data */
            /***** add code for Feed wtd by shipeng 2012-7-11*****/
		//GPIO_WTD_FEED();
		if (retry_time >= 0 && get_ticks() > endtime)
                return (-2);    /* timed out */
        }
#endif
        /* Trigger watchdog, if needed */
        /*
        WATCHDOG_RESET();       
        */

#ifdef CONFIG_SHOW_ACTIVITY
        while (!tstc()) {
		extern void show_activity(int arg);
            show_activity(0);
        }
#endif
        while (!tstc()) {
        }
        
        c = getc();
        
        /*printf("key=0x%x\n",c);*/
        
        /*
         * Special character handling
         */
        switch (c) {
        case '\r':              /* Enter        */
        case '\n':
            *p ='\0' ;
            puts ("\r\n");
            return (p - oplbootConsoleBuffer);


        case 0x08:              /* ^H  - backspace  */
        case 0x7F:              /* DEL - backspace  */
            p=delete_char(oplbootConsoleBuffer, p, &col, &n, plen);
            continue;
        case KEY_ESC:
            get_key_press(c);
            continue;

        default:
            /*
             * Must be a normal character then
             */
            if (n < CONFIG_SYS_CBSIZE-2) {
                    if  ((c>=0x30 && c<=0x39) || (c>=0x41 && c<=0x5a) || (c>=0x61 && c<=0x7a) || (c=='_') || (c=='-')) // modify by dengjian 2012-07-23 for u-boot ST
                    {   
                    ++col;      /* echo input       */
                    
                    putc ('*');
                    *p++ = c;
                    ++n;
                    }
                    
            } else {            /* Buffer full      */
                putc ('\a');
            }
        }
    }
}
#endif
