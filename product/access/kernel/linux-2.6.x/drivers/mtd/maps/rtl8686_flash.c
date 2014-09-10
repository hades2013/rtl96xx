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

#include <linux/kernel_size.h> //add by dengjian 2012/10/16
#include "../../../../../include/lw_config.h" //add by dengjian 2012-11-06 for QID0080

#define WINDOW_ADDR 0xbd000000
#ifdef CONFIG_SPANSION_16M_FLASH
#define WINDOW_SIZE 0x1000000
#define FLASH_BANK_SIZE 0x400000
#else
#ifdef CONFIG_4M_FLASH
#define WINDOW_SIZE 0x400000
#endif
#ifdef CONFIG_8M_FLASH
#define WINDOW_SIZE 0x800000
#endif
#ifdef CONFIG_16M_FLASH
#define WINDOW_SIZE 0x1000000
#endif
/*ccwei*/
#ifdef CONFIG_32M_FLASH
#define WINDOW_SIZE 0x2000000
#endif
#ifdef CONFIG_2M_FLASH
#define WINDOW_SIZE 0x200000
#endif
#endif //CONFIG_SPANSION_16M_FLASH
#define BUSWIDTH 2

static struct mtd_info *rtl8672_mtd = NULL;

__u8 rtl8672_map_read8(struct map_info *map, unsigned long ofs)
{
	//printk("enter %s %d\n",__FILE__,__LINE__);
	return __raw_readb((unsigned char*)(map->map_priv_1 + ofs));
}

__u16 rtl8672_map_read16(struct map_info *map, unsigned long ofs)
{
	//printk("enter %s %d\n",__FILE__,__LINE__);
	return __raw_readw((unsigned short*)(map->map_priv_1 + ofs));
}

__u32 rtl8672_map_read32(struct map_info *map, unsigned long ofs)
{
	//printk("enter %s %d\n",__FILE__,__LINE__);
	return __raw_readl((unsigned int*)(map->map_priv_1 + ofs));
}

void rtl8672_map_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	//printk("enter to %x from  %x len %d\n",to, map->map_priv_1+from , len);
	//11/15/05' hrchen, change the size to fit file systems block size if use different fs
	//4096 for cramfs, 1024 for squashfs
	if (from>0x10000)
	    memcpy(to, (unsigned char*)(map->map_priv_1 + from), (len<=1024)?len:1024);//len);
	else
	    memcpy(to, (unsigned char*)(map->map_priv_1 + from), (len<=4096)?len:4096);//len);
	//printk("enter %s %d\n", __FILE__,__LINE__);

}

void rtl8672_map_write8(struct map_info *map, __u8 d, unsigned long adr)
{
	__raw_writeb(d, (unsigned char*)(map->map_priv_1 + adr));
	mb();
}

void rtl8672_map_write16(struct map_info *map, __u16 d, unsigned long adr)
{
	__raw_writew(d, (unsigned short*)(map->map_priv_1 + adr));
	mb();
}

void rtl8672_map_write32(struct map_info *map, __u32 d, unsigned long adr)
{
	__raw_writel(d, (unsigned int*)(map->map_priv_1 + adr));
	mb();
}

void rtl8672_map_copy_to(struct map_info *map, unsigned long to, const void *from, ssize_t len)
{
	//printk("enter %s %d\n",__FILE__,__LINE__);
	memcpy_toio((unsigned char*)(map->map_priv_1 + to), from, len);
}

struct map_info rtl8672_map = {
	name: "Physically mapped flash",
	size: WINDOW_SIZE,
	bankwidth: BUSWIDTH,
	phys: WINDOW_ADDR
	/*
	read8: rtl8672_map_read8,
	read16: rtl8672_map_read16,
	read32: rtl8672_map_read32,
	copy_from: rtl8672_map_copy_from,
	write8: rtl8672_map_write8,
	write16: rtl8672_map_write16,
	write32: rtl8672_map_write32,
	copy_to: rtl8672_map_copy_to
	*/
};


#if 0
/////////////////////////////////////////////////////////////////////////////
//10/17/05' hrchen, kernel is removed
#define USR_CONFIG_SIZE 0x100000
#define TR069_SIZE 0x10000
#define CS_SIZE 0x20000
#define FIX_ROOTFS_OFFSET 0x00500000
static struct mtd_partition rtl8672_parts[] = {
	/*patch from linux 2.4*/
	{ name: "boot", 	offset: 0, size:  CONFIG_BOOT_SIZE, mask_flags: 0 },//bd000000~bd100000
	{ name: "config",	offset: CONFIG_BOOT_SIZE, size: USR_CONFIG_SIZE - TR069_SIZE - CS_SIZE, mask_flags:0},//bd100000~bd1f0000
	{ name: "CS",		offset: CONFIG_BOOT_SIZE + USR_CONFIG_SIZE - TR069_SIZE - CS_SIZE, size: CS_SIZE, mask_flags:0},//bd100000~bd1f0000
	{ name: "tr069",	offset: CONFIG_BOOT_SIZE + USR_CONFIG_SIZE - TR069_SIZE, size: TR069_SIZE , mask_flags:0},//bd1f0000~bd200000	
	{ name: "linux",	offset: CONFIG_BOOT_SIZE + USR_CONFIG_SIZE, size: FIX_ROOTFS_OFFSET - CONFIG_BOOT_SIZE - USR_CONFIG_SIZE , mask_flags:0},//bd200000~bd500000
	{ name: "rootfs",	offset: FIX_ROOTFS_OFFSET, size:0x00500000, mask_flags:0},
	{ name: "jffs2",	offset:0xa00000, size:0x00600000, mask_flags:0},
};
#endif

#define U_BOOT_SIZE     0x40000  //256kb
#define U_BOOT_ENV_SIZE 0x10000  //64kb
#define EXP_CFG_SIZE    0x10000  //64kb
//#define OS_SIZE  ((FLASH_SIZE - U_BOOT_SIZE - U_BOOT_ENV - OS_CFG)/2)  
/*Begin modify by dengjian 2012-11-06 for QID0080*/
#ifdef CONFIG_BOOT_MULTI_APP
#define OS_SIZE      0x380000 //3.5M
#else
#define OS_SIZE      0x300000 //3M
#endif
/*End modify by dengjian 2012-11-06 for QID0080*/

#ifdef CONFIG_PRODUCT_EPN105
#define NVRAM_SIZE   0x40000  //256kB
#define JFFS2_SIZE   0x60000  //384kb
#else
#define NVRAM_SIZE   0x20000  //128kb
#define JFFS2_SIZE   0x80000  //512kb
#endif

/*Begin add by dengjian 2012/10/16*/
#define UNIT_SIZE 65536

#ifdef CONFIG_MTD_DYNAMIC_FLASH_PARTITION
#define TOTAL_FMW_HEADLEN 0x60 
#define KERNEL_DYNAMIC_SIZE_ALIGN_64KB ((KERNEL_DYNAMIC_SIZE +TOTAL_FMW_HEADLEN+64*1024-1)&(~(64*1024-1)))
#endif

#ifdef CONFIG_MTD_DYNAMIC_FLASH_PARTITION
       #define KERNEL_SIZE         (KERNEL_DYNAMIC_SIZE_ALIGN_64KB)
#else
       #define KERNEL_SIZE         (UNIT_SIZE*24)                           //0x00180000   1.5m
#endif
#ifdef CONFIG_MTD_DYNAMIC_FLASH_PARTITION
      #define FS_SIZE             (OS_SIZE-KERNEL_SIZE)
#else
      #define FS_SIZE             (UNIT_SIZE*32)                            //0x00200000   2m
#endif
/*End add by dengjian 2012/10/16*/

#ifdef CONFIG_BOOT_MULTI_APP
static struct mtd_partition rtl8672_parts[] = {
	{
		.name = "bootloader",
		.size = U_BOOT_SIZE,
		//.offset = FLASH_START_ADDR,	
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
	/*Begin add by dengjian 2012/10/16*/
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
	/*End add by dengjian 2012/10/16*/      
    {
        .name = "jffs2",
        .size = JFFS2_SIZE,
        .offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+(2*OS_SIZE), 
    },
};
#else
static struct mtd_partition rtl8672_parts[] = {
	{
		.name = "bootloader",
		.size = U_BOOT_SIZE,
		//.offset = FLASH_START_ADDR,	
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
		.name = "nvram",
		.size = NVRAM_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+OS_SIZE,	
	},	
	{
		.name = "jffs2",
		.size = JFFS2_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV_SIZE+EXP_CFG_SIZE+OS_SIZE+NVRAM_SIZE,	
	},	
};
#endif

#ifdef CONFIG_SPANSION_16M_FLASH
enum GPIO_DEF {
	GPIO_A_0 = 0, GPIO_A_1, GPIO_A_2, GPIO_A_3, GPIO_A_4, GPIO_A_5, GPIO_A_6, GPIO_A_7,
	GPIO_B_0 = 8, GPIO_B_1, GPIO_B_2, GPIO_B_3, GPIO_B_4, GPIO_B_5, GPIO_B_6, GPIO_B_7,
	GPIO_C_0 = 16, GPIO_C_1, GPIO_C_2, GPIO_C_3, GPIO_C_4, GPIO_C_5, GPIO_C_6, GPIO_C_7,
	GPIO_D_0 = 24, GPIO_D_1, GPIO_D_2, GPIO_D_3, GPIO_D_4, GPIO_D_5, GPIO_D_6, GPIO_D_7
};
/*linux-2.6.19*/
extern void gpioConfig (int gpio_num, int gpio_func);
extern void gpioSet(int gpio_num);
extern void gpioClear(int gpio_num);


unsigned int flash_bank = 0;
u32 rtl8672_flash_bank(u32 address)
{
	u32 val, bank;
	bank = address >> 22;

	//setup GPA driving 4mA
	val = REG32(BSP_MISC_IO_DRIVING);
	REG32(BSP_MISC_IO_DRIVING) =  val|(1 << 12);


	//config gpa5,6
	gpioConfig(GPIO_A_5, 0x0002);
	gpioConfig(GPIO_A_6, 0x0002);

	switch (bank){
		case 1:
			gpioSet(GPIO_A_5);
			gpioClear(GPIO_A_6);
			break;
		case 2:
			gpioClear(GPIO_A_5);
			gpioSet(GPIO_A_6);
			break;
		case 3:
			gpioSet(GPIO_A_5);
			gpioSet(GPIO_A_6);
			break;
		case 0:
		default:
			gpioClear(GPIO_A_5);
			gpioClear(GPIO_A_6);
	}


	if (bank != flash_bank)
	{
		//printk("\r\n bank switch :%d --> %d",flash_bank, bank);
		//udelay(5000);//test
		flash_bank = bank ;
	}

	return (address & (FLASH_BANK_SIZE-1));

}
#endif



#if LINUX_VERSION_CODE < 0x20212 && defined(MODULE)
#define init_rtl8672_map init_module
#define cleanup_rtl8672_map cleanup_module
#endif

//#define mod_init_t  static int __init
//#define mod_exit_t  static void __exit

typedef enum {
	SPI_T = 0,
	NAND_T = 0x02
} FLASH_TYPE_T;

static int __init init_rtl8672_map(void)
{
	unsigned int mcr = REG32(BSP_MC_MCR);
    printk(KERN_NOTICE "flash device: 0x%x at 0x%x\n", WINDOW_SIZE, WINDOW_ADDR);
/*
	rtl8672_map.map_priv_1 =WINDOW_ADDR;

	if (!rtl8672_map.map_priv_1) {
		printk("Failed to ioremap\n");
		return -EIO;
	}
*/
	rtl8672_map.virt = (void *)WINDOW_ADDR; //ioremap(rtl8672_map.phys, rtl8672_map.size);
	if (!rtl8672_map.virt) {
		printk(KERN_ERR "Failed to ioremap_nocache\n");
		return -EIO;
		//rc = -EIO;
		//goto err2;
	}

	simple_map_init(&rtl8672_map);

	//printk("probe start %08x\n", rtl8672_map.virt);
	//spi or nor flash
	if (!rtl8672_mtd && ((mcr & BSP_BOOT_FLASH_STS) == SPI_T)) {
		printk("spi probe...\n");
		rtl8672_mtd = do_map_probe("spi_probe", &rtl8672_map);
	}

	//printk("probe ends\n");
	if (rtl8672_mtd) {
		rtl8672_mtd->owner = THIS_MODULE;
		add_mtd_partitions(rtl8672_mtd, rtl8672_parts, ARRAY_SIZE(rtl8672_parts));
		ROOT_DEV = MKDEV(MTD_BLOCK_MAJOR, 0);

		#ifdef CONFIG_HTTP_FILE
		do {
			int idx;
			extern void setup_http_file_flash(u32 flashsize);
			for (idx=0; idx < (sizeof(rtl8672_parts)/sizeof(struct mtd_partition)); idx++) {
				if (strcmp("rootfs",rtl8672_parts[idx].name))
					continue;
				setup_http_file_flash(rtl8672_parts[idx].size);
			}
		} while (0);
		#endif
		return 0;
	}
	else {
		printk("%s: probe failed! mcr=0x%08x\n", __func__, mcr);
	}
/*
	printk("probe start 2\n");
	rtl8672_mtd = do_map_probe("jedec_probe", &rtl8672_map);
	printk("probe ends 2\n");
	if (rtl8672_mtd) {
		rtl8672_mtd->owner = THIS_MODULE;
		add_mtd_partitions(rtl8672_mtd, rtl8672_parts, sizeof(rtl8672_parts)/sizeof(rtl8672_parts[0]));
		return 0;
	}

	printk("probe start 3\n");
	rtl8672_mtd = do_map_probe("map_rom", &rtl8672_map);
	printk("probe ends 3\n");
	if (rtl8672_mtd) {
		rtl8672_mtd->owner = THIS_MODULE;
		add_mtd_partitions(rtl8672_mtd, rtl8672_parts, sizeof(rtl8672_parts)/sizeof(rtl8672_parts[0]));
		return 0;
	}
*/
	iounmap((void *)rtl8672_map.virt);
	return -ENXIO;
}

static void __exit cleanup_rtl8672_map(void)
{
	if (rtl8672_mtd) {
		del_mtd_partitions(rtl8672_mtd);
		map_destroy(rtl8672_mtd);
	}
	if (rtl8672_map.virt) {
		iounmap((void *)rtl8672_map.virt);
		rtl8672_map.map_priv_1 = 0;
	}
}

MODULE_LICENSE("GPL");
module_init(init_rtl8672_map);
module_exit(cleanup_rtl8672_map);
