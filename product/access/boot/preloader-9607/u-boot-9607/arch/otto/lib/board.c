/*
 * (C) Copyright 2003
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
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <stdio_dev.h>
#include <version.h>
#include <net.h>
#include <environment.h>
//#include <nand.h>
#include <soc.h>
#include <pblr.h>
#include <asm/otto_pll.h>
#include <asm/arch/bspchip.h>

#include "../../../../../../include/lw_config.h"
#include "../../../../../../include/pdt_config.h"

DECLARE_GLOBAL_DATA_PTR;

extern int timer_init(void);
extern void spi_flash_init(void);
extern int incaip_set_cpuclk(void);
extern void set_exception_base(u32_t);

extern ulong uboot_end_data;
extern ulong uboot_end;
extern ulong load_addr;

ulong monitor_flash_len;
unsigned int gpio_state_orig_val = 0;
void boot_gpio_mode_set(unsigned int gpioId, unsigned int mode);
void boot_gpio_state_set(unsigned int gpioId, unsigned int enable);
void boot_gpio_databit_set(unsigned int gpioId, unsigned int data);
void boot_gpio_value_set(unsigned int gpio_pin, unsigned int databit);

static inline void soc_post_init(void);

static char *failed = "*** failed ***\n";

/*
 * mips_io_port_base is the begin of the address space to which x86 style
 * I/O ports are mapped.
 */
unsigned long mips_io_port_base = -1;

int __board_early_init_f(void)
{
	/*
	 * Nothing to do in this dummy implementation
	 */
	return 0;
}
int board_early_init_f(void)
	__attribute__((weak, alias("__board_early_init_f")));

static int init_func_ram(void)
{
#ifdef	CONFIG_BOARD_TYPES
	int board_type = gd->board_type;
#else
	int board_type = 0;	/* use dummy arg */
#endif
	puts("DRAM:  ");

	gd->ram_size = initdram(board_type);
	if (gd->ram_size > 0) {
		print_size(gd->ram_size, "\n");
		return 0;
	}
	puts(failed);
	return 1;
}

/*begin add by shipeng 2013-08-05*/
#define BUILDNUMBER 0

static void version_init(char *szVerStr)
{
    if (NULL == szVerStr) 
    {
        return;
    }
   	(void)sprintf(szVerStr, "%s (R%u ONU SPI @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, U_BOOT_DATE, U_BOOT_TIME);
}

static int display_banner(void)
{
	char szVerString[100];
    version_init(szVerString);    
    printf("\n***********************************************************************\n");
    printf("*                                                                     *\n");
    printf("*   %-60s*\n", szVerString);
	printf("*   %s.                      *\n",DEV_COPYRIGHT_STR);		
    printf("*                                                                     *\n");
    printf("***********************************************************************\n");
	printf("\n\n");
	return 0;
}


#ifndef CONFIG_SYS_NO_FLASH
static void display_flash_config(ulong size)
{
	puts("Flash: ");
	print_size(size, "\n");
}
#endif

static int init_baudrate(void)
{
#define	N_BAUDRATES (sizeof(baudrate_table) / sizeof(baudrate_table[0]))
	const unsigned int baudrate_table[] = CONFIG_SYS_BAUDRATE_TABLE;
	unsigned int br_from_divisor;
	unsigned int diff, prev_diff = 0xFFFFFFFF;
	int i;

	if (parameters.soc.peri_info.baudrate_divisor == 0x0) {
#ifdef CONFIG_FOLLOW_PLR_TO_DIS_UART
		gd->baudrate = 0;
#else
		gd->baudrate = getenv_ulong("baudrate", 10, CONFIG_BAUDRATE);
		parameters._uart_init(0); /* _uart_init is assign_uart_fp() to recover _uart function pointers */
#endif /* #ifdef FOLLOW_PLR_TO_DIS_UART */
	} else {
		br_from_divisor = (CONFIG_SYS_HZ /
		                   ((16 * parameters.soc.peri_info.baudrate_divisor) -
		                    (16 * CONFIG_UART_DIVISOR_MOD) -
		                    8));

		for (i=0; i<N_BAUDRATES; i++) {
			if (baudrate_table[i] > br_from_divisor) {
				diff = baudrate_table[i] - br_from_divisor;
			} else {
				diff = br_from_divisor - baudrate_table[i];
			}

			if (diff > prev_diff) {
				break;
			} else {
				prev_diff = diff;
			}
		}
		br_from_divisor = baudrate_table[i-1];
		gd->baudrate = getenv_ulong("baudrate", 10, br_from_divisor);
	}

	return 0;
}


/*
 * Breath some life into the board...
 *
 * The first part of initialization is running from Flash memory;
 * its main purpose is to initialize the RAM so that we
 * can relocate the monitor code to RAM.
 */

/*
 * All attempts to come up with a "common" initialization sequence
 * that works for all boards and architectures failed: some of the
 * requirements are just _too_ different. To get rid of the resulting
 * mess of board dependend #ifdef'ed code we now make the whole
 * initialization sequence configurable to the user.
 *
 * The requirements for any new initalization function is simple: it
 * receives a pointer to the "global data" structure as it's only
 * argument, and returns an integer return code, where 0 means
 * "continue" and != 0 means "fatal error, hang the system".
 */
typedef int (init_fnc_t)(void);

init_fnc_t *init_sequence[] = {
	board_early_init_f,
	timer_init,
	env_init,		/* initialize environment */
	//init_baudrate,		/* initialize baudrate settings */
	//serial_init,		/* serial communications setup */
	//console_init_f,
	display_banner,		/* say that we are here */
	checkboard,
	init_func_ram,
	NULL,
};

/*Begin add for by huangmingjian 2014-02-25*/

void boot_gpio_mode_set(unsigned int gpioId, unsigned int mode)
{	
	unsigned int reg,value,groupId,offset,tmp;

	groupId = gpioId / 32;
	offset	= gpioId % 32;

	reg = groupId?BOOT_GPIO_DIR_EFGHr:BOOT_GPIO_DIR_ABCDr;
	value = READ_MEM32(reg);
	tmp = 1 << offset;
	value = ~tmp & value;
	value = mode << offset | value;
	WRITE_MEM32(reg, value);
}  

void boot_gpio_state_set(unsigned int gpioId, unsigned int enable)
{
	unsigned int reg,value,groupId,offset,tmp, reg_com;

    groupId = gpioId / 32;
    offset  = gpioId % 32;

	reg_com = groupId?GPIO_STATE_REGADDR_COM_H:GPIO_STATE_REGADDR_COM_L;
	value = READ_MEM32(reg_com);
	tmp = 1 << offset;
	value = ~tmp & value;
	value = enable << offset | value;
	WRITE_MEM32(reg_com, value);

	reg = groupId?BOOT_GPIO_CTRL_EFGHr:BOOT_GPIO_CTRL_ABCDr;
	value = READ_MEM32(reg);
	tmp = 1 << offset;
	value = ~tmp & value;
	value = enable << offset | value;
	WRITE_MEM32(reg, value);
}  


void boot_gpio_databit_set(unsigned int gpioId, unsigned int data)
{

	unsigned int reg,value,groupId,offset,tmp;

	groupId = gpioId / 32;
	offset	= gpioId % 32;

	reg = groupId?BOOT_GPIO_DATA_EFGHr:BOOT_GPIO_DATA_ABCDr;
	value = READ_MEM32(reg);
	tmp = 1 << offset;
	value = ~tmp & value;
	value = data << offset | value;
	WRITE_MEM32(reg, value);
}  


void boot_gpio_value_set(unsigned int gpio_pin, unsigned int databit)
{
	boot_gpio_state_set(gpio_pin, GPIO_STATE_ENABLED);
	boot_gpio_mode_set(gpio_pin, GPIO_MODE_OUTPUT);
	boot_gpio_databit_set(gpio_pin, databit);
}

#define DELAY_MS 1000
void led_test(void)
{
#if defined(CONFIG_PRODUCT_EPN104N) || defined(CONFIG_PRODUCT_EPN104W) || defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN105) || defined(CONFIG_PRODUCT_EPN101ZG) || defined(CONFIG_PRODUCT_GPN104N)
	boot_gpio_value_set(LAN0_GPIO, GPIO_DATABIT_0);
#if !defined(CONFIG_PRODUCT_EPN101ZG)
	boot_gpio_value_set(LAN1_GPIO, GPIO_DATABIT_0);
	boot_gpio_value_set(LAN2_GPIO, GPIO_DATABIT_0);
	boot_gpio_value_set(LAN3_GPIO, GPIO_DATABIT_0);
#endif	
#if defined(CONFIG_PRODUCT_EPN105)
	boot_gpio_value_set(PON_LOS_GPIO, GPIO_DATABIT_0);
#endif
	boot_gpio_value_set(PON_LINK_GPIO, GPIO_DATABIT_0);
	boot_gpio_value_set(PON_ALARM_GPIO, GPIO_DATABIT_0);
	mdelay(DELAY_MS);
	
	boot_gpio_databit_set(LAN0_GPIO, GPIO_DATABIT_1);
#if !defined(CONFIG_PRODUCT_EPN101ZG)	
	boot_gpio_databit_set(LAN1_GPIO, GPIO_DATABIT_1);
	boot_gpio_databit_set(LAN2_GPIO, GPIO_DATABIT_1);
	boot_gpio_databit_set(LAN3_GPIO, GPIO_DATABIT_1);
#endif	
#if defined(CONFIG_PRODUCT_EPN105)
	boot_gpio_databit_set(PON_LOS_GPIO, GPIO_DATABIT_1);
#endif
	boot_gpio_databit_set(PON_LINK_GPIO, GPIO_DATABIT_1);
	boot_gpio_databit_set(PON_ALARM_GPIO, GPIO_DATABIT_1);

	
#elif defined(CONFIG_PRODUCT_EPN101R) 
	boot_gpio_value_set(LAN2_GPIO, GPIO_DATABIT_0);
	boot_gpio_value_set(PON_LINK_GPIO, GPIO_DATABIT_0);
	boot_gpio_value_set(PON_ALARM_GPIO, GPIO_DATABIT_0);
	mdelay(DELAY_MS);
	
	boot_gpio_databit_set(LAN2_GPIO, GPIO_DATABIT_1);
	boot_gpio_databit_set(PON_LINK_GPIO, GPIO_DATABIT_1);
	boot_gpio_databit_set(PON_ALARM_GPIO, GPIO_DATABIT_1);

	
#elif defined(CONFIG_PRODUCT_5500)
	boot_gpio_value_set(LAN0_GPIO, GPIO_DATABIT_0);
	boot_gpio_value_set(LAN1_GPIO, GPIO_DATABIT_0);
	boot_gpio_value_set(PON_LINK_GPIO, GPIO_DATABIT_0);
	boot_gpio_value_set(PON_ALARM_GPIO, GPIO_DATABIT_0);
	mdelay(DELAY_MS);
	
	boot_gpio_databit_set(LAN0_GPIO, GPIO_DATABIT_1);	
	boot_gpio_databit_set(LAN1_GPIO, GPIO_DATABIT_1);
	boot_gpio_databit_set(PON_LINK_GPIO, GPIO_DATABIT_1);
	boot_gpio_databit_set(PON_ALARM_GPIO, GPIO_DATABIT_1);
#endif
}

/*End add for by huangmingjian 2014-02-25*/

void board_init_f(ulong bootflag)
{
	gd_t gd_data, *id;
	bd_t *bd;
	init_fnc_t **init_fnc_ptr;

	/*
	   we should  poweroff the laser , or it may let other onus which is online  be down.
	*/
	boot_gpio_value_set(PON_VCC_GPIO, GPIO_DATABIT_0);/*Add by huangmingjian 2014-04-24 for EPN104ZG Bug 550 */
	/* set .bss to zero */
	memset(&uboot_end_data, 0, ((void *)&uboot_end) - ((void *)&uboot_end_data));

	/* Pointer is writable since we allocated a register for it.
	 */
	gd = &gd_data;
	/* compiler optimization barrier needed for GCC >= 3.4 */
	__asm__ __volatile__("" : : : "memory");

	memset((void *)gd, 0, sizeof(gd_t));

	/*Add by huangmingjian for test led 2014-01-17*/
	/*if reg GPIO_STATE_REGADDR_COM_L is used for setting gpio0-31 in uboot, 
	we should recover it, oterwise  setting gpio0-31 will be failed in user space.
	*/
	gpio_state_orig_val = READ_MEM32(GPIO_STATE_REGADDR_COM_L); 
	led_test();
	WRITE_MEM32(GPIO_STATE_REGADDR_COM_L, gpio_state_orig_val);
	/*End by huangmingjian for test led 2014-02-25*/
	
#if defined(CONFIG_PRODUCT_EPN105) || defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN104N) || defined(CONFIG_PRODUCT_EPN104W) || defined(CONFIG_PRODUCT_GPN104N) || defined(CONFIG_PRODUCT_EPN101ZG)
	boot_gpio_value_set(ENABLE_UART_GPIO, GPIO_DATABIT_1); /*Add by huangmingjian for test led 2014-01-17*/
#endif
/*begin add by shipeng for test led 2013-03-21*/
#if 0
	unsigned int tmp = 0;

	mdelay(3000);		
	tmp = READ_MEM32(0xBB0000D8);
	tmp = tmp & (~(0x1<<13));
	WRITE_MEM32(0xBB0000D8, tmp);
#endif
/*end add by shipeng for test led 2013-03-21*/

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0)
			hang();
	}

	mem_malloc_init((ulong)__builtin_alloca(TOTAL_MALLOC_LEN), TOTAL_MALLOC_LEN);

	bd = (bd_t *)__builtin_alloca(sizeof(bd_t));
	gd->bd = bd;

	id = (gd_t *)__builtin_alloca(sizeof(gd_t));

	bd->bi_boot_params = (unsigned long)__builtin_alloca(CONFIG_SYS_BOOTPARAMS_LEN);

	/*
	 * Save local variables to board info struct
	 */
	bd->bi_memstart	= CONFIG_SYS_SDRAM_BASE;	/* start of DRAM */
	bd->bi_memsize	= gd->ram_size;		/* size of DRAM in bytes */
	bd->bi_baudrate	= gd->baudrate;		/* Console Baudrate */

	memcpy(id, (void *)gd, sizeof(gd_t));

	/* Obtain kernel address from preloader. */
	load_addr = parameters.soc.layout.kernel1_addr;
	load_addr += CONFIG_SYS_FLASH_BASE;

	__asm__ __volatile__("" : : : "memory");

	board_init_r(id, CONFIG_SYS_TEXT_BASE);

	/* NOTREACHED - relocate_code() does not return */
}

/*
 * This is the next part if the initialization sequence: we are now
 * running from RAM and have a "normal" C environment, i. e. global
 * data can be written, BSS has been cleared, the stack size in not
 * that critical any more, etc.
 */

void board_init_r(gd_t *id, ulong dest_addr)
{
/*begin modify by shipeng 2013-08-05*/
	IPaddr_t serverip;
	char *cMac;
	char *s;
/*end modify by shipeng 2013-08-05*/
	
#ifndef CONFIG_SYS_NO_FLASH
	ulong size;
#endif
	extern void malloc_bin_reloc(void);
#ifndef CONFIG_ENV_IS_NOWHERE
	extern char *env_name_spec;
#endif
	bd_t *bd;

	/* Inform CPU the base address of exception handler. */
	set_exception_base(CONFIG_EXCEPTION_BASE);

	gd = id;
	gd->flags |= GD_FLG_RELOC;	/* tell others: relocation done */

	debug("Now running in RAM - U-Boot at: %08lx\n", dest_addr);

	/*begin add by shipeng 2013-08-07*/
	restore_flash();
	/*end add by shipeng 2013-08-07*/

	gd->reloc_off = 0;

	monitor_flash_len = (ulong)&uboot_end_data - dest_addr;

#if defined(CONFIG_NEEDS_MANUAL_RELOC)
	/*
	 * We have to relocate the command table manually
	 */
	fixup_cmdtable(&__u_boot_cmd_start,
		(ulong)(&__u_boot_cmd_end - &__u_boot_cmd_start));
#endif /* defined(CONFIG_NEEDS_MANUAL_RELOC) */

	/* there are some other pointer constants we must deal with */
#ifndef CONFIG_ENV_IS_NOWHERE
	env_name_spec += gd->reloc_off;
#endif

	bd = gd->bd;

#ifndef CONFIG_SYS_NO_FLASH
	size = flash_init();
	display_flash_config(size);
	bd->bi_flashsize = size;
#endif

#ifdef CONFIG_CMD_SF
	puts("SPI-F: ");
	spi_flash_init();
#endif

	bd->bi_flashstart = CONFIG_SYS_FLASH_BASE;
#if CONFIG_SYS_MONITOR_BASE == CONFIG_SYS_FLASH_BASE
	bd->bi_flashoffset = monitor_flash_len;	/* reserved area for U-Boot */
#else
	bd->bi_flashoffset = 0;
#endif
#ifdef CONFIG_CMD_NAND
	nand_init();		/* go init the NAND */
#endif

	/* relocate environment function pointers etc. */
	/*printf("Loading %dB env. variables from offset 0x%x\n",
	       CONFIG_ENV_SIZE, CONFIG_ENV_OFFSET);*/
	env_relocate();
	init_baudrate();	/* initialize baudrate settings */
	serial_init();		/* serial communications setup */
	console_init_f();

	/* IP Address */
	bd->bi_ip_addr = getenv_IPaddr("ipaddr");

#if defined(CONFIG_PCI)
	/*
	 * Do pci configuration
	 */
	pci_init();
#endif

/** leave this here (after malloc(), environment and PCI are working) **/
	/* Initialize stdio devices */
	stdio_init();

	jumptable_init();

	/* Initialize the console (after the relocation and devices init) */
	console_init_r();
/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

	/* Initialize from environment */
	load_addr = getenv_ulong("loadaddr", 16, load_addr);
#if defined(CONFIG_CMD_NET)
	{
		char *s = getenv("bootfile");

		if (s != NULL)
			copy_filename(BootFile, s, sizeof(BootFile));
	}
#endif

#ifdef CONFIG_CMD_SPI
	puts("SPI: ");
	spi_init();		/* go init the SPI */
	puts("ready\n");
#endif

#if defined(CONFIG_MISC_INIT_R)
	/* miscellaneous platform dependent initialisations */
	misc_init_r();
#endif

	soc_post_init();

/*begin modify by shipeng 2013-08-05*/
#if defined(CONFIG_CMD_NET)
	puts("Net:\t");
	eth_initialize(gd->bd);
	
	puts ("IP:\t\t");print_IPaddr(bd->bi_ip_addr); puts("\n");
    serverip=getenv_IPaddr ("serverip");
    puts ("Server IP:\t");print_IPaddr(serverip); puts("\n");
    cMac = getenv ("ethaddr");
    printf("MAC:\t\t%s",cMac); puts("\n");
#endif

	s = getenv ("mem");
	if (!s)
	{
		setenv ("mem",	"32");
		oplbootSaveConfig();
	}
/*end modify by shipeng 2013-08-05*/

/*begin add for led by shipeng 2013-08-15*/
#if CONFIG_PRODUCT_5500
	unsigned int cfg_led_value = 0;
	unsigned int tmp = 0;
	WRITE_MEM32(0xBB01E000, 0x1);/*led parallel mode*/
	cfg_led_value = (1 << 16)|0xffe;
	WRITE_MEM32((0xBB01E004 + LAN0_LED*0x4), cfg_led_value);/*LAN0*/
	cfg_led_value = (2 << 16)|0xffe;
	WRITE_MEM32((0xBB01E004 + LAN1_LED*0x4), cfg_led_value);/*LAN1*/
	tmp = READ_MEM32(0xBB01E0A0);
	tmp = tmp | (3<<LAN0_LED);
	WRITE_MEM32(0xBB01E0A0, tmp);/*led_en*/
	tmp = READ_MEM32(0xBB023014);
	tmp = tmp | (3<<LAN1_LED);
	WRITE_MEM32(0xBB023014, tmp);/*io_len_en*/
#endif


#if defined(CONFIG_PRODUCT_EPN104N) || defined(CONFIG_PRODUCT_EPN104W) || defined(CONFIG_PRODUCT_EPN104ZG) || defined(CONFIG_PRODUCT_EPN104ZG_A) || defined(CONFIG_PRODUCT_EPN105) || defined(CONFIG_PRODUCT_EPN101ZG) || defined(CONFIG_PRODUCT_GPN104N)
	unsigned int cfg_led_value = 0;
	unsigned int tmp = 0;
	WRITE_MEM32(0xBB01E000, 0x1);/*led parallel mode*/
	cfg_led_value = (1 << 16)|0xffe;
	WRITE_MEM32((0xBB01E004 + LAN0_LED*0x4), cfg_led_value);/*LAN0*/

#if !defined(CONFIG_PRODUCT_EPN101ZG)
	cfg_led_value = (2 << 16)|0xffe;
	WRITE_MEM32((0xBB01E004 + LAN1_LED*0x4), cfg_led_value);/*LAN1*/
	cfg_led_value = (3 << 16)|0xffe;
	WRITE_MEM32((0xBB01E004 + LAN2_LED*0x4), cfg_led_value);/*LAN2*/
	cfg_led_value = (4 << 16)|0xffe;
	WRITE_MEM32((0xBB01E004 + LAN3_LED*0x4), cfg_led_value);/*LAN3*/
#endif
	
	tmp = READ_MEM32(0xBB01E0A0);
#if !defined(CONFIG_PRODUCT_EPN101ZG)
	tmp = tmp|(1<<(LAN3_LED+1))|(1<<(LAN2_LED+1))|(1<<(LAN1_LED+1))|(1<<(LAN0_LED+1));
#else
	tmp = tmp|(1<<(LAN0_LED+1));
#endif
	WRITE_MEM32(0xBB01E0A0, tmp);/*led_en*/
	
	tmp = READ_MEM32(0xBB023014);
	
#if !defined(CONFIG_PRODUCT_EPN101ZG)
	tmp = tmp|(1<<LAN3_LED)|(1<<LAN2_LED)|(1<<LAN1_LED)|(1<<LAN0_LED);
#else
	tmp = tmp|(1<<LAN0_LED);
#endif
	WRITE_MEM32(0xBB023014, tmp);/*io_len_en*/	
#endif

#if defined(CONFIG_PRODUCT_EPN101R) 
	unsigned int cfg_led_value = 0;
	unsigned int tmp = 0;
	WRITE_MEM32(0xBB01E000, 0x1);/*led parallel mode*/
	cfg_led_value = (3 << 16)|0xffe;
	WRITE_MEM32((0xBB01E004 + LAN2_LED*0x4), cfg_led_value);/*LAN2*/
	tmp = READ_MEM32(0xBB01E0A0);
	tmp = tmp | (1<<(LAN2_LED+1));
	WRITE_MEM32(0xBB01E0A0, tmp);/*led_en*/
	tmp = READ_MEM32(0xBB023014);
	tmp = tmp | (1<<LAN2_LED);
	WRITE_MEM32(0xBB023014, tmp);/*io_len_en*/
#endif

#if CONFIG_PRODUCT_5500
	WRITE_MEM32(0xBB00014C, 0x1);/*ExtMode*/
	WRITE_MEM32((0xBB000090 + 5*0x4), 0x16);/*ForceAbility*/
	WRITE_MEM32(0xBB000120, 0x70);/*ForceAbilityState*/
	WRITE_MEM32(0xBB000004, 0xb);/*ExtRgmiiDelay*/
#endif
/*end add for led by shipeng 2013-08-15*/
	/*Begin add by huangmingjian 2014-05-09 for Bug 575*/
	/*flash bootversion and keep it  correct*/
	if(strcmp(U_BOOT_VERSION, getenv("bootversion")))
	{
		setenv("bootversion", U_BOOT_VERSION);
		saveenv();	
	}
	/*Begin add by huangmingjian 2014-05-09 for Bug 575*/
	/* main_loop() can return to retry autoboot, if so just run it again. */
	for (;;)
		main_loop();

	/* NOTREACHED - no way out of command loop except booting */
}

static inline void soc_post_init(void) {
	const char sf_probe_cmd[] = {"sf probe 0"};

	/* The `baudrate' env was originally set by
	   CONFIG_BAUDRATE. Since we removed CONFIG_BAUDRATE, we
	   set baudrate env manually. */
	if (getenv_ulong("baudrate", 10, 0xFFFFFFFF) == 0xFFFFFFFF) {
		setenv_ulong("baudrate", gd->baudrate);
	}

	/* Issue `sf probe 0' before entering CLI. Since our dirver
	   automatically switches to next CS when access across CS
	   boundary, it is lousy to do `sf probe 0' before other
	   `sf' commands can be used. */
	run_command(sf_probe_cmd, 0);

	return;
}

void hang(void)
{
	puts("### ERROR ### Please RESET the board ###\n");
	for (;;)
		;
}
