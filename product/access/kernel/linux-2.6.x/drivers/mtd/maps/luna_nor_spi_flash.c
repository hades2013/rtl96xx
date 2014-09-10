#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/root_dev.h>
#include <linux/mtd/partitions.h>
#include <linux/config.h>
#include <linux/delay.h>
/*linux-2.6.19*/
#include <linux/version.h>
//ql
//#include "../../../../config/autoconf.h"

#include <bspchip.h>
#include <kernel_soc.h>

#include <linux/kernel_size.h>
#include "../../../../../include/lw_config.h"

#define BUSWIDTH 2

static struct mtd_info *luna_nor_spi_mtd = NULL;

struct map_info luna_nor_spi_map = {
	name:	RTK_MTD_DEV_NAME,
	// size:	WINDOW_SIZE,//Filled after probing spi nor flash
	bankwidth:BUSWIDTH,
	phys:	SPI_NOR_FLASH_START_ADDR
};

#if 0
#define USR_CONFIG_SIZE   0x00100000
#define TR069_SIZE        0x00010000
#define CS_SIZE           0x00020000
#define FIX_ROOTFS_OFFSET 0x00500000
static struct mtd_partition luna_nor_spi_parts[] = {
	//bd000000~bd100000
	{name: "boot", offset: 0, size: CONFIG_BOOT_SIZE, mask_flags:0}, 
	//bd100000~bd1d0000
	{name: "config", offset: CONFIG_BOOT_SIZE, size: USR_CONFIG_SIZE - TR069_SIZE - CS_SIZE, mask_flags:0}, 
	//bd1d0000~bd1f0000
	{name: "CS", offset: CONFIG_BOOT_SIZE + USR_CONFIG_SIZE - TR069_SIZE - CS_SIZE, size: CS_SIZE, mask_flags:0}, 
	//bd1f0000~bd200000   
	{name: "tr069", offset: CONFIG_BOOT_SIZE + USR_CONFIG_SIZE - TR069_SIZE, size: TR069_SIZE, mask_flags:0}, 
	//bd200000~bd500000
	{name: "linux", offset: CONFIG_BOOT_SIZE + USR_CONFIG_SIZE, size: FIX_ROOTFS_OFFSET - CONFIG_BOOT_SIZE - USR_CONFIG_SIZE, mask_flags:0}, 
	//bd500000~bda00000
	{name: "rootfs", offset: FIX_ROOTFS_OFFSET, size: 0x00500000, mask_flags:0},
	{name: "jffs2", offset: 0x00a00000, size: 0x00600000, mask_flags:0},
};
#endif

#if defined(CONFIG_PRODUCT_EPN104N) || defined(CONFIG_PRODUCT_EPN104W) || defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN101ZG) || defined(CONFIG_PRODUCT_GPN104N)
#define U_BOOT_SIZE     0x40000  //256kb
#define U_BOOT_ENV_SIZE 0x10000  //64kb
#define EXP_CFG_SIZE    0x10000  //64kb
#define OS_SIZE      	0x700000 //7M
#define JFFS2_SIZE   	0xA0000  //640kb
#endif

#ifdef CONFIG_PRODUCT_5500
#define U_BOOT_SIZE     0x40000  //256kb
#define U_BOOT_ENV_SIZE 0x10000  //64kb
#define EXP_CFG_SIZE    0x10000  //64kb
#define OS_SIZE      	0x480000 //4.5M *2
#define JFFS2_SIZE   	0x6A0000 //6M+640KB
#endif

#ifdef CONFIG_PRODUCT_EPN101R
#define U_BOOT_SIZE     0x40000  //256kb
#define U_BOOT_ENV_SIZE 0x10000  //64kb
#define EXP_CFG_SIZE    0x10000  //64kb
#define OS_SIZE      	0xF80000 //15.5M *2
#define JFFS2_SIZE   	0xA0000 //640KB
#endif

/* Add by Alan Lee ,at 20140711 */
#ifdef CONFIG_PRODUCT_EPN105
#define U_BOOT_SIZE     0x40000  //256kb
#define U_BOOT_ENV_SIZE 0x10000  //64kb
#define EXP_CFG_SIZE    0x10000  //64kb
#define OS_SIZE      0x390000 //3.6M * 2
#define NVRAM_SIZE   0x40000  //256kB
#define JFFS2_SIZE   0x60000  //384kb
#endif
/*End*/

#ifdef CONFIG_MTD_DYNAMIC_FLASH_PARTITION
#define TOTAL_FMW_HEADLEN 0x60 
#define KERNEL_DYNAMIC_SIZE_ALIGN_64KB ((KERNEL_DYNAMIC_SIZE +TOTAL_FMW_HEADLEN+64*1024-1)&(~(64*1024-1)))
#define KERNEL_SIZE         (KERNEL_DYNAMIC_SIZE_ALIGN_64KB)
#define FS_SIZE             (OS_SIZE-KERNEL_SIZE)  
#else
#define UNIT_SIZE 65536
#define KERNEL_SIZE         (UNIT_SIZE*24)    //0x00180000   1.5m                      
#define FS_SIZE             (UNIT_SIZE*32)    //0x00200000   2m
#endif

#if defined(CONFIG_PRODUCT_EPN104N) || defined(CONFIG_PRODUCT_EPN104W)|| defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN101ZG) || defined(CONFIG_PRODUCT_GPN104N)
static struct mtd_partition luna_nor_spi_parts[] = {
	{
		.name = "bootloader",
		.size = U_BOOT_SIZE,
		.offset = 0x0,
	},    
	{
		.name = "bootenv",
		.size = U_BOOT_ENV_SIZE,
		.offset = U_BOOT_SIZE,		
	},	
	{
		.name = "expcfg",
		.size = EXP_CFG_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE,		
	},	
 	{
		.name =	"kernel ",
		.size =	KERNEL_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE,
	},
	{
		.name =	"fs",
		.size =	FS_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+KERNEL_SIZE,
	},	
	{
		.name = "jffs2",
		.size = JFFS2_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+OS_SIZE,	
	},	
};
#endif

#if defined(CONFIG_PRODUCT_5500) || defined(CONFIG_PRODUCT_EPN101R)
static struct mtd_partition luna_nor_spi_parts[] = {
	{
		.name = "bootloader",
		.size = U_BOOT_SIZE,
		.offset = 0x0,	
	},    
	{
		.name = "bootenv",
		.size = U_BOOT_ENV_SIZE,
		.offset = U_BOOT_SIZE,		
	},	
	{
		.name = "expcfg",
		.size = EXP_CFG_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE,		
	},	
	{
		.name =	"kernel1",
		.size =	KERNEL_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE,
	},
	{
		.name =	"fs1",
		.size =	FS_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+KERNEL_SIZE,
	},
	{
		.name =	"kernel2",
		.size =	KERNEL_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+OS_SIZE,
	},
	{
		.name = "fs2",
		.size = FS_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+OS_SIZE+KERNEL_SIZE,
	},      
	{
		.name = "jffs2",
		.size = JFFS2_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+(2*OS_SIZE),	
	},
};
#endif


#if defined(CONFIG_PRODUCT_EPN105)
static struct mtd_partition luna_nor_spi_parts[] = {
	{
		.name = "bootloader",
		.size = U_BOOT_SIZE,
		.offset = 0x0,	
	},    
	{
		.name = "bootenv",
		.size = U_BOOT_ENV_SIZE,
		.offset = U_BOOT_SIZE,		
	},	
	{
		.name = "expcfg",
		.size = EXP_CFG_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE,		
	},	
	{
		.name =	"kernel1",
		.size =	KERNEL_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE,
	},
	{
		.name =	"fs1",
		.size =	FS_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+KERNEL_SIZE,
	},
	{
	   .name = "nvram1",
	   .size = NVRAM_SIZE,
	  // .offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+(2*OS_SIZE), 
	  .offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+OS_SIZE,
	},  
	{
	   .name = "nvram2",
	   .size = NVRAM_SIZE,
	  // .offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+(2*OS_SIZE)+NVRAM_SIZE, 
	  .offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+OS_SIZE+NVRAM_SIZE,
	},             
	{
		.name =	"kernel2",
		.size =	KERNEL_SIZE,
		//.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+OS_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+OS_SIZE+(2*NVRAM_SIZE), 
	},
	{
		.name = "fs2",
		.size = FS_SIZE,
		//.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+OS_SIZE+KERNEL_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+OS_SIZE+(2*NVRAM_SIZE)+KERNEL_SIZE, 
	},     
};

#endif

typedef enum {
	SPI_T        = 0x0,
	I2C_EEPROM_T = 0x1,
	NAND_T       = 0x2
} FLASH_TYPE_T;

#define DELIMITER_LINE "===============================================================================\n" 
__init static int init_luna_nor_spi_map(void)
{
	unsigned int mcr = REG32(BSP_MC_MCR);

	printk(KERN_NOTICE DELIMITER_LINE);
	printk(KERN_NOTICE "%s: flash map at 0x%x\n", __FUNCTION__, (u32)luna_nor_spi_map.phys);

	luna_nor_spi_map.virt = (void *)SPI_NOR_FLASH_START_ADDR;
	if (!luna_nor_spi_map.virt) {
		printk(KERN_ERR "Failed to ioremap_nocache\n");
		return -EIO;
		//rc = -EIO;
		//goto err2;
	}

	simple_map_init(&luna_nor_spi_map);

	/* We only support SPI NOR FLASH */
	if (!luna_nor_spi_mtd && ((mcr & BSP_BOOT_FLASH_STS) == SPI_T)) {
		printk("luna spi probe...\n");
		luna_nor_spi_mtd = do_map_probe("spi_probe", &luna_nor_spi_map);

		//printk("probe ends\n");
		if (luna_nor_spi_mtd) {
			struct  mtd_partition *parts;
			int nr_parts = 0;
			const char*part_probes[] = {"cmdlinepart", NULL,};

			nr_parts = parse_mtd_partitions(luna_nor_spi_mtd, part_probes, &parts, 0);

			printk("add luna nor spi partition\n");
			luna_nor_spi_mtd->owner = THIS_MODULE;
			if(nr_parts <= 0) {
				printk("MTD partitions obtained from built-in array\n");
				add_mtd_partitions(luna_nor_spi_mtd, luna_nor_spi_parts,
						   ARRAY_SIZE(luna_nor_spi_parts));
			} else {
				printk("MTD partitions obtained from kernel command line\n");
				add_mtd_partitions(luna_nor_spi_mtd, parts, nr_parts);
			}
			ROOT_DEV = MKDEV(MTD_BLOCK_MAJOR, 0);

			printk(KERN_NOTICE DELIMITER_LINE);
			return 0;
		}
		printk("ERROR: luna nor spi partition invalid\n");
	} else {
		printk("%s: probe failed! mcr=0x%08x\n", __func__, mcr);
	}

	iounmap((void *)luna_nor_spi_map.virt);
	printk(KERN_NOTICE DELIMITER_LINE);
	return -ENXIO;
}

__exit static void cleanup_luna_nor_spi_map(void)
{
	if (luna_nor_spi_mtd) {
		del_mtd_partitions(luna_nor_spi_mtd);
		map_destroy(luna_nor_spi_mtd);
	}
	if (luna_nor_spi_map.virt) {
		iounmap((void *)luna_nor_spi_map.virt);
		luna_nor_spi_map.map_priv_1 = 0;
	}
}

MODULE_LICENSE("GPL");
module_init(init_luna_nor_spi_map);
module_exit(cleanup_luna_nor_spi_map);
