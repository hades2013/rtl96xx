#include <caffeine600mg.h>

parameter_to_bootloader_t parameters;

/* 0: 128KB, 1: 256KB, 2: 512KB */
#define ESZ_FACTOR_LIMIT 3
#define ESZ(x)           (128 << x)
uint32_t esz_factor;

#define DUMP16(X) do { \
	printf("II: %08x: %08x %08x %08x %08x\n",       \
	       X,                                       \
	       *((uint32_t *)X+0), *((uint32_t *)X+1),  \
	       *((uint32_t *)X+2), *((uint32_t *)X+3)); \
	} while(0)

static void live_beacon(void) {
	printf("__...---===^^| Caffeine %dmg |^^===---...__\n", CWEIGHT);
	printf("II: Estimated LX:  %dMHz\n", pll_query_freq(PLL_DEV_LX));
	printf("II: Estimated CP0 COUNT: %dMHz\n", pll_query_freq(PLL_DEV_CPU));
	return;
}

static void prompt(void) {
	uint32_t i = 0;
	pblr_puts("\r--------------------------------------------\n");
	pblr_puts("[1] Erasing size: ");
	for (i=0; i<ESZ_FACTOR_LIMIT; i++) {
		printf("[%c] %dKB ", (esz_factor == i)?'*':' ', ESZ(i));
	}
	pblr_putc('\n');

	printf("[3] Erase %dKB from 0x00000000\n", ESZ(esz_factor));
	pblr_puts("[5] CP0 COUNT verification\n");
	pblr_puts("[9] Load and program image to flash with XMODEM\n");
	pblr_puts("[R] Reset\n");
	return;
}

static void para_setup(void) {
	parameter_soc_rwp->flash_info.num_chips = 0x01;
	parameter_soc_rwp->flash_info.addr_mode = 0x03;
	parameter_soc_rwp->flash_info.prefer_divisor = 0x10;
	parameter_soc_rwp->flash_info.size_per_chip = 0x17;	/* 8MB */
	parameter_soc_rwp->flash_info.prefer_rx_delay0 = 0x00;
	parameter_soc_rwp->flash_info.prefer_rx_delay1 = 0x00;
	parameter_soc_rwp->flash_info.prefer_rx_delay2 = 0x00;
	parameter_soc_rwp->flash_info.prefer_rx_delay3 = 0x00;
	parameter_soc_rwp->flash_info.prefer_rd_cmd = 0x03;
	parameter_soc_rwp->flash_info.prefer_rd_cmd_io = 0x00;
	parameter_soc_rwp->flash_info.prefer_rd_dummy_c = 0x00;
	parameter_soc_rwp->flash_info.prefer_rd_addr_io = 0x00;
	parameter_soc_rwp->flash_info.prefer_rd_data_io = 0x00;
	parameter_soc_rwp->flash_info.wr_cmd = 0x02;
	parameter_soc_rwp->flash_info.wr_cmd_io = 0x00;
	parameter_soc_rwp->flash_info.wr_dummy_c = 0x00;
	parameter_soc_rwp->flash_info.wr_addr_io = 0x00;
	parameter_soc_rwp->flash_info.wr_data_io = 0x00;
	parameter_soc_rwp->flash_info.wr_boundary = 0x08;
	parameter_soc_rwp->flash_info.erase_cmd = 0xD8;	 /* block erase */
	parameter_soc_rwp->flash_info.erase_unit = 0x10; /* 64KB */
	parameter_soc_rwp->flash_info.pm_method = 0x00;
	parameter_soc_rwp->flash_info.pm_rdsr_cmd = 0x00;
	parameter_soc_rwp->flash_info.pm_rdsr2_cmd = 0x00;
	parameter_soc_rwp->flash_info.pm_wrsr_cmd = 0x00;
	parameter_soc_rwp->flash_info.pm_enable_cmd = 0x00;
	parameter_soc_rwp->flash_info.pm_enable_bits = 0x0000;
	parameter_soc_rwp->flash_info.pm_status_len = 0x00;
	parameter_soc_rwp->flash_info.rdbusy_cmd = 0x00;
	parameter_soc_rwp->flash_info.rdbusy_len = 0x00;
	parameter_soc_rwp->flash_info.rdbusy_loc = 0x00;
	parameter_soc_rwp->flash_info.rdbusy_polling_period = 0x00;
	parameter_soc_rwp->flash_info.id = 0xFFFFFF;

	/* detect four-byte address mode */
	parameter_soc_rwp->flash_info.addr_mode += (*((volatile uint32_t *)0xb8001204) >> 9) & 0x1;
	parameters.flash_init_result = INI_RES_UNINIT;

	return;
}

static void block_erase_cycle_time(void) {
	/* "two seconds" is the max cycle time of 64K block erase surveyed. */
	udelay(1000*1000);
	udelay(1000*1000);
	return;
}

static void flash_erase(uint32_t erase_sz_kb) {
	const uint32_t flash_sz_b = 1 << parameter_soc_rwp->flash_info.erase_unit;
	const uint32_t erase_sz_b = erase_sz_kb * 1024;
	uint32_t addr;

	printf("II: Erasing %dKB from 0x00000000...   0KB", erase_sz_kb);
	for (addr=0; addr<erase_sz_b; addr+=flash_sz_b) {
		flash_unit_erase(0, addr);
		block_erase_cycle_time();
		printf("\b\b\b\b\b%3dKB", (addr+flash_sz_b)/1024);
	}
	pblr_puts(" OK\n");
	return;
}

__attribute__ ((weak)) uint32_t
baudrate_divisor(void) {
	return pll_query_freq(PLL_DEV_LX)*1000000/16/BAUDRATE - 1;
}

int main(void) {
	uint32_t i;

	otto_NS16550_init(baudrate_divisor());
	assign_uart_fp();

	esz_factor = 0;

	live_beacon();
	para_setup();
	flash_init();
	prompt();

	do {
		i = otto_NS16550_getc();
		switch (i) {
		case '1':
			esz_factor = (esz_factor+1)%ESZ_FACTOR_LIMIT;
			prompt();
			break;
		case '3':
			flash_erase(ESZ(esz_factor));
			DUMP16(0xbfc00000);
			DUMP16(0xbfc00000 + (ESZ(esz_factor)*1024) - 16);
			prompt();
			break;
		case '5':
			pblr_puts("II: 5 seconds verification starts... ");
			for (i=0; i<5; i++) {
				udelay(1000*1000);
			}
			pblr_puts("done\r");
			prompt();
			break;
		case '9':
			flash_erase(ESZ(esz_factor));
			xmodem_2_sf(ESZ(esz_factor));
			prompt();
			break;
		case 'r':
		case 'R':
			/* for 8x */
			*((volatile u32_t *)0xbb000058) = 0x3;
			*((volatile u32_t *)0xbb000040) = 0x1;
			/* for 9x */
			*((volatile u32_t *)0xbb000014) = 0xffffffff;
			/* for 6266 */
			*((volatile u32_t *)0xbb000074) = 0x00000004;
			/* Software reset */
			pblr_puts("II: Hardware reset is not supported. Trying software reset...");
			udelay(1000*1000);
			__asm__ __volatile__("lui $8, 0xbfc0;\n"
			                     "jr  $8;\n"
			                     "nop;\n");
			break;
		case ' ':
			break;
		default:
			printf("EE: Unknown option: %c(%08x)\r", i, i);
		}
	} while (1);

	return 0;
}
