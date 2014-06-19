/*
 * (C) Copyright 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * RTC, Date & Time support: get and set date & time
 */
#include <common.h>
#include <command.h>
#include "../../../../include/lw_config.h" 
#include "../../../../include/pdt_config.h"


#ifdef CONFIG_CMD_UU
extern unsigned int tftp_file_size;
//unsigned char static g_OplHeadSwitch=1;
int promt_cancel(int second);
extern  int bootUpdateSimpleCheck(unsigned int fileaddr);

#define FMW_HEADLEN 0x60
#define PRODUCT_VER_STR_LEN_MAX 36
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

int fw_head_check(unsigned int fileaddr)
{     
    struct fmw_hdr_t *pstHead = (struct fmw_hdr_t *)fileaddr;
	unsigned int count = 0;
	unsigned int tmp_addr = fileaddr + FMW_HEADLEN;
	unsigned int checksum = 0;
	unsigned int checksum_tmp = 0;
	unsigned int checksum_tmp_i = 0;
	unsigned int *tmp_ptr;

	if(pstHead->headlen != FMW_HEADLEN)
	{
		printf("do not recognize fmw_hdr!");
        return -1;
	}

	/*计算数据的checksum*/
	while(count < (pstHead->imgsize - FMW_HEADLEN))
	{		
		checksum_tmp = (checksum_tmp<<8)|((unsigned int)(*(unsigned char *)tmp_addr++));
		checksum_tmp_i++;
		if (checksum_tmp_i >=4)
		{
    		checksum += checksum_tmp;
    		checksum_tmp_i = 0;
	    }
		count++;		
	}

	/*计算头的checksum, 除了成员pstHead->checksum*/
	count = FMW_HEADLEN;
	tmp_ptr=(unsigned int *)pstHead;		
	while(count)
	{
		checksum += *tmp_ptr;
		tmp_ptr++;
		count -=4;
	}
	checksum -= pstHead->checksum;
	
	if(pstHead->checksum != (~checksum))
    {
        printf("App FileCRC check error!");
        return -1;
    }
	
    return 0;    
}

int do_uuboot (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

    int rcode = 0;
    char filename[20]="u-boot.bin";
    char cmd[50] = "";
    char pstr[50] = "tftpboot 0x80600000 ";
    int  size  ;
    
    if( OPL_NULL != getenv("flash_boot_name"))
    {
        strcpy(filename,getenv("flash_boot_name")); 
    }   
    switch (argc) {
    case 2:         
        strcpy(filename,argv[1]);
        break;
    default:
        //printf ("Usage:\n%s\n", cmdtp->usage);
        rcode = 1;
    }

    printf("We allocate 0xbd000000-0xbd03ffff for U-boot, so max size of u-boot.bin should be 256KB!\n");
    printf("Are you sure to update U-boot? (Hit any key to abort!) ");
    if (promt_cancel(5)) return -1;
    
    strcat(pstr,filename);
    strcpy(cmd,pstr);
    printf("\n------ Download file \t------ \n");
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;

    size=tftp_file_size;
    
	#if 0
    if(0!=bootUpdateSimpleCheck(0x80600000))
	{
	    return -1;
    }
	#endif     
    if (tftp_file_size > UBOOT_MAX_SIZE) {
        printf("\nERROR: The U-boot size is larger than 0x%x\n", UBOOT_MAX_SIZE);
        return -1;
    }
	    
    printf("\n------ Erase flash \t------\n");
    sprintf(cmd, "sf erase 0x%x +0x%x", uboffset, UBOOT_MAX_SIZE);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;
    
    printf("\n------ Program flash \t------\n");
    sprintf(cmd, "sf write 0x%x 0x%x 0x%x", 0x80600000, uboffset, size);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;
	
    printf("********************************************************\n");
    printf("*   U-boot is updated, please power down the board!    *\n");
    printf("********************************************************\n");

    return 1;
}

int do_uuapp (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

    int size = 0;
    int rcode = 0;
    char filename[50]=""CONFIG_RELEASE_VER".bin";
    char cmd[50]="";
    char pstr[50] = "tftpb 0x80600000 ";
	unsigned int os_offset;
    
    if(OPL_NULL != getenv("flash_file_name")) 
    {
        strcpy(filename,getenv("flash_file_name")); 
    }
    switch (argc) {
    case 2:         
        strcpy(filename,argv[1]);
        break;
    default:
        //printf ("Usage:\n%s\n", cmdtp->usage);
        rcode = 1;
    }
#ifdef CONFIG_BOOT_MULTI_APP
    if (0 == strcmp("0",getenv("bootflag")))
    {
		os_offset = knl_2offset;
    }
    else if (0 == strcmp("1",getenv("bootflag")))
    {
		os_offset = knl_1offset;
    }
    else 
    {
        printf("\nERROR: wrong bootflag!\n");
        return -1;
    }
#else
	os_offset = knl_1offset;
#endif
    printf("Are you sure to update OS image? (Hit any key to abort!) ");


    if (promt_cancel(5)) return -1;

    strcat(pstr,filename);
    strcpy(cmd,pstr);
    printf("\n------ Download file \t------\n");
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;

    if (tftp_file_size > OS_MAX_SIZE) {
        printf("\nERROR: The Os update size is larger than 0x%x\n", OS_MAX_SIZE);
        return -1;
    }

	if (0 != fw_head_check(0x80600000))
    {
        return -1;
    }
	
    size = tftp_file_size<=OS_MAX_SIZE?tftp_file_size:OS_MAX_SIZE;
	
	printf("\n------ Erase flash \t------\n");
    sprintf(cmd, "sf erase 0x%x +0x%x", os_offset, size);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;

	printf("\n------ Program flash \t------\n");
    sprintf(cmd, "sf write 0x%x 0x%x 0x%x", 0x80600000, os_offset, size);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;
    
#ifdef CONFIG_BOOT_MULTI_APP
    if (0 == strcmp("0",getenv("bootflag")))
    {
        setenv("bootflag","1");
        saveenv();
    }
    else if (0 == strcmp("1",getenv("bootflag")))
    {
        setenv("bootflag","0");
        saveenv();
    }     
#endif
    printf("********************************************************\n");
    printf("*           Os image is updated successfully!          *\n");
    printf("********************************************************\n");    
    
    return 1;
}

int do_uuimage (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int size = 0;
    int rcode = 0;
    char filename[50]=""CONFIG_RELEASE_VER".img";
    char cmd[50]="";
    char pstr[50] = "tftpb 0x80600000 ";
    
    if(OPL_NULL != getenv("flash_image_name")) 
    {
        strcpy(filename,getenv("flash_image_name")); 
    }
    switch (argc) {
    case 2:         
        strcpy(filename,argv[1]);
        break;
    default:
        //printf ("Usage:\n%s\n", cmdtp->usage);
        rcode = 1;
    }
#ifdef CONFIG_BOOT_MULTI_APP
    printf("Are you sure to update the 16M image[0xbd000000~0xbdffffff]? (Hit any key to abort!) ");
#else
    printf("Are you sure to update the 8M image[0xbd000000~0xbd7fffff]? (Hit any key to abort!) ");
#endif
    if (promt_cancel(5)) return -1;

    strcat(pstr,filename);
    strcpy(cmd,pstr);
    printf("\n------ Download file \t------\n");
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;

	#if 0
    if(0!=bootUpdateSimpleCheck(0x80600000))
	{
	    return -1;
    }
	#endif
    if (tftp_file_size > FLASH_SIZE) {
        printf("\nERROR: The Os update size is larger than 0x%x\n", FLASH_SIZE);
        return -1;
    }
	
    size = tftp_file_size<=FLASH_SIZE?tftp_file_size:FLASH_SIZE;
	
	printf("\n------ Erase flash \t------\n");
    sprintf(cmd, "sf erase 0x%x +0x%x", uboffset, size);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;

	printf("\n------ Program flash \t------\n");
    sprintf(cmd, "sf write 0x%x 0x%x 0x%x", 0x80600000, uboffset, size);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;

    printf("******************************************************************\n");
    printf("**  image is updated successfully,please power down the board!  **\n");
    printf("******************************************************************\n");  
    
    return 1;
}

int promt_cancel(int second)
{
    u32 i;

    /*delay for 3s to abort*/
    for (i=0;i<second;i++)
    {
        if (tstc()) {   /* we got a key press   */
            (void) getc();  /* consume input    */
            puts ("aborted\n");
            return 1;
        }

        printf("%2x",(second-i));
        udelay(1000000);
		
        /***** add code for Feed wtd by shipeng 2012-7-11*****/
	  //GPIO_WTD_FEED();
		
        puts("\b\b");
    }

    printf("%2x",0);
    return 0;
    udelay(300000);
    puts("\n");
}

/***************************************************/

U_BOOT_CMD(
    uuboot, 2,  1,  do_uuboot,
    "update U-boot from tftp server.",
    "    - update file u-boot.bin from tftp server to flash.\n"
    "      the file size must be below 190k !\n"        
);


U_BOOT_CMD(
    uuapp,   2,  1,  do_uuapp,
    "update kernel Image from tftp server.",
    "    - update file kernel Image from tftp server to flash.\n"
    "      the file size must be below 3.5MB !\n"      
);
U_BOOT_CMD(
    uuimage,   2,  1,  do_uuimage,
    "update whole Image(8M) from tftp server.",
    "    - update file whole Image from tftp server to flash.\n"
    "      the file size must be 8MB !\n"      
);

#endif  /* CONFIG_CMD_UU */

