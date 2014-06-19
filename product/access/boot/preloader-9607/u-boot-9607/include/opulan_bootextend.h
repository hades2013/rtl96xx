/*
=============================================================================
     Header Name: opulan_bootextend.h

     General Description:
        head file for port U-boot for OPULAN boot.
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 xwang             23/4/07       Initial Version
----------------   ------------  ----------------------------------------------
*/
#define getChAndEcho getche
/* List all menu set : one set means that shown to user in one screen */
typedef enum OPLBOOT_MENUSET_s
{
    MENU_MAIN_BOOT_SYSTEM,
    MENU_MAIN_BOOT_CONFIG,
    MENU_MAIN_ETHERNET_SUBMENU,
    MENU_MAIN_SERIAL_SUBMENU,
    MENU_MAIN_NETUPDATE_SUBMENU,
    MENU_MAIN_XMODEM_SUBMENU,
    MENU_MAIN_SAVE_CHANGES,
    MENU_MAIN_REBOOT,
    
  //  MENU_FLASH_INFO,
    MENU_FLASH_BOOT_FILE_NAME,
    MENU_FLASH_OS_FILE_NAME ,
    MENU_FLASH_BOOT_UPDATE,
    MENU_OS1_UPDATE,
    MENU_OS2_UPDATE,
    MENU_JFFS2_UPDATE,
    MENU_FLASH_EXIT,
    
    MENU_SERIAL_BAU,  
    MENU_SERIAL_EXIT,
    
    MENU_ETH_LOC_IP, 
    MENU_ETH_SER_IP,      
    MENU_ETH_MAC_ADDR,//added by kxu
    MENU_ETH_EXIT,
    
    MENU_BOOT_LAC_NET,
 //   MENU_BOOT_LAC_FLA,
    MENU_BOOT_OS1_FLA,
    MENU_BOOT_OS2_FLA,
    
    MENU_BOOT_TYPE_VX,
    MENU_BOOT_TYPE_LINUX_NFS,     
    MENU_BOOT_TYPE_LINUX_EMM,
    
    MENU_BOOT_FILE_NAME ,  
    MENU_BOOT_FILE_TYPE ,  
    MENU_BOOT_FILE_LOC  ,   
    MENU_BOOT_ARGS_VX   ,  
    MENU_BOOT_ARGS_LINUX_NFS, 
    MENU_BOOT_ARGS_LINUX_EMM,  
	MENU_BOOT_CPU_CLK,  
    MENU_BOOT_EXIT,
    
    MENU_BOOT_NFS_EXIT,
    MENU_BOOT_NFS_SERVER,
    MENU_BOOT_NFS_PATH,
    
    MENU_BOOT_FLASH_TYPE_EMM,
    MENU_BOOT_FLASH_TYPE_NFS,
    MENU_BOOT_FLASH_TYPE_VX,
    MENU_BOOT_FLASH_TYPE_EXIT,
    MENU_BOOT_FLASH_TYPE_LINUX_NFS,
    MENU_BOOT_FLASH_TYPE_LINUX_RAMDISK_EMBEDED,
    MENU_XUPDATE_EXIT    , 
    MENU_FLASH_UBOOT_UPDATE     ,
// MENU_FLASH_ENV_UPDATE ,       
    MENU_FLASH_KERNEL1_UPDATE   , 
    MENU_FLASH_KERNEL2_UPDATE  , 
    MENU_FLASH_JFFS2_UPDATE    , 
    MENU_NULL,
	/*Begin add by dengjian*/
    MENU_MAIN_MODIFY_PASSWD,
    MENU_MAIN_APP_RESTORE_DEFAULT,
    MENU_MAIN_COMMAND_LINE_INTERFACE,
    MENU_APP_UPDATE,
    MENU_FLASH_APP_FILE_NAME
	/*End add by dengjian*/
} OPLBOOT_MENUSET_e;

typedef struct MENU_INPUT_s
{
    char *envVarName;
    char *promt;
    char *value;
} MENU_INPUT_t;


/*
 *  when one menu item select, 3 types of action valid:
 *      1. id          : menu id
 *      2. child       : print child menu
 *      3. command     : command to execute
 *      4. envVarName  : envionment variables
 *      5. promt       : promt for operate
 *      6. value       : value from user
 *      7. type        : value type
 *      8. header      : struct MENU_ITEM_s message
 *  check from 1 to 8, just select one.  actually, 1 and 2 can be merged.
 */     
typedef struct MENU_ITEM_s
{
    OPLBOOT_MENUSET_e ID;
    struct MENU_ITEM_s *next;
    char *command;
    char *envVarName;
    char *promt;
    char *type;
    char *header;
    
} MENU_ITEM_t;


#define DEBUG_OPL_BOOT 1
