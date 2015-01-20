#include <preloader.h>
#include <pblr.h>
#include <plr_pll_gen1.h>
#include "bspchip.h"
#include "dram/plr_dram_gen2.h"

//#define FLASHI flash_info

extern void console_init(void);
extern void dram_setup(void);
extern void pll_setup(void);

struct soc_reg_remap_t {
	volatile u32_t *SOC_IO_MODE_EN; /* IO_MODE_EN : Enable interface IO */
	volatile u32_t *SOC_GPIO_CTRL_2;
};

const struct soc_reg_remap_t soc_reg_remap_a0 = {
	.SOC_IO_MODE_EN=(volatile u32_t *)0xBB023020,
	.SOC_GPIO_CTRL_2=(volatile u32_t *)0xBB000118,
	/*add remap register at here*/
};

/*soc 6266  address map table*/
const struct soc_reg_remap_t soc_reg_remap_b0 = {
	.SOC_IO_MODE_EN=(volatile u32_t *)0xBB023018,
	.SOC_GPIO_CTRL_2=(volatile u32_t *)0xBB0000F0,
	/*add remap register at here*/
};

static struct soc_reg_remap_t soc_reg_remap;

#define IO_MODE_EN (soc_reg_remap.SOC_IO_MODE_EN)
#define GPIO_CTRL_2 (soc_reg_remap.SOC_GPIO_CTRL_2)

static void feature_SOC_A0(u32_t chip_ver) {
	switch(parameters.bond_id){
	default:
	case CHIP_901:
	case CHIP_906_2:
	case CHIP_907:
	case CHIP_902:
	case CHIP_903:
	case CHIP_96:
		*IO_MODE_EN = *IO_MODE_EN & ~(1<<3); //bit[3] is 0
		*GPIO_CTRL_2 = *GPIO_CTRL_2 | (1<<1); //EN_GPIO[1] is 1
		break;
	case CHIP_906_1:
	case CHIP_98B:
	case CHIP_DBG:
		*IO_MODE_EN = *IO_MODE_EN | (1<<3); //bit[3] is 1
		*GPIO_CTRL_2 = *GPIO_CTRL_2 & ~(1<<1); //EN_GPIO[1] is 0
		break;
	}
	return;
}

static void feature_SOC_B0(u32_t chip_ver) {
	switch (chip_ver) {
	default:
	case 0x4:
		REG32(0xb8001074) |= (0x1 << 11); /* ADDRB29_LOCK_DIS */
	case 0x3:
		REG32(0xb8000108) |= (0x1 << 23); /* LX_ARB_DEL_EN */
		REG32(0xb8001074) |= (0x1 << 31); /* ARB_SEL */
	case 0x2:
	case 0x1:
		break;
	}

	/* For UART */
	switch(parameters.bond_id){
	default:
	case CHIP_901:
	case CHIP_906_2:
	case CHIP_907:
	case CHIP_902:
	case CHIP_903:
	case CHIP_96:
	case CHIP_2510:
	case CHIP_96P:
		*IO_MODE_EN = *IO_MODE_EN | (4<<3); //bit [5:3] is 100
		/*begin add by wanghuanyu for uart1*/
		*IO_MODE_EN = *IO_MODE_EN | (1<<4); //bit [5:3] is 100
		/*end add by wanghuanyu for uart1*/
		break;
	case CHIP_906_1:
	case CHIP_98B:
	case CHIP_DBG:
		*IO_MODE_EN = *IO_MODE_EN | (1<<3); //bit [5:3] is 001
		break;
	}

	return;
}

static u32_t soc_reg_init(void) {
	u32_t chip_ver;

	REG32(CHIP_ID_REG) = 0xa0000000;
	parameters.soc_id = (REG32(CHIP_ID_REG) & 0xffff);
	chip_ver = ((REG32(CHIP_ID_REG) >> 16) & 0x1f);

	REG32(BOND_CHIP_MODE) = 0xb0000000;
	parameters.bond_id = (REG32(BOND_CHIP_MODE) & 0xff);

	switch (parameters.soc_id) {
	default:
	case SOC_B0:
		soc_reg_remap = soc_reg_remap_b0;
		feature_SOC_B0(chip_ver);
		break;
	case SOC_A0:
		soc_reg_remap = soc_reg_remap_a0;
		feature_SOC_A0(chip_ver);
		break;
	}

	return chip_ver;
}

void pll_setup_info(void);


#define PLR_WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define PLR_READ_MEM32(addr)         (*(volatile unsigned int *) (addr))
#define PLR_WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define PLR_READ_MEM16(addr)         (*(volatile unsigned short *) (addr))
#define PLR_WRITE_MEM8(addr, val)    (*(volatile unsigned char *) (addr)) = (val)
#define PLR_READ_MEM8(addr)          (*(volatile unsigned char *) (addr))

void plr_led_set(int gpioid, int data)//data = 1 or 0
{
    unsigned int reg, flag, offset, value;

    offset = gpioid%32;
    flag = gpioid/32;

    reg = flag ? 0xBB0000F4 : 0xBB0000F0;
    value = PLR_READ_MEM32(reg);
    value |= (1ul<<offset);
    PLR_WRITE_MEM32(reg,value);

    reg = flag ? 0xB800331C : 0xB8003300;
    value = PLR_READ_MEM32(reg);
    value |= (1ul<<offset);
    PLR_WRITE_MEM32(reg,value);

    reg = flag ? 0xB8003324 : 0xB8003308;
    value = PLR_READ_MEM32(reg);
    value |= (1ul<<offset);
    PLR_WRITE_MEM32(reg,value);

    reg = flag ? 0xB8003328 : 0xB800330C;
    value = PLR_READ_MEM32(reg);
    if (data)
        value |= (1ul<<offset);
    else
        value &= ~(1ul<<offset);
    PLR_WRITE_MEM32(reg,value);
}

void platform_init_phase_2(void) {
	u32_t chip_ver;

	/* soc registetr remap */
	chip_ver = soc_reg_init();
    plr_led_set(58,0);//light lan0
    plr_led_set(60,0);//light lan3
	pll_setup();
    plr_led_set(46,0);//light lan2
	console_init();
    plr_led_set(23,0);//light los
	PRINT_PLR_INFO(chip_ver);
    plr_led_set(42,0);//light link
	
	/* Disable LX bus time out control */
	REG32(SYSREG_LX_BUS_TIMEOUT_CTRL_REG) &= ~SYSREG_LBTC_MASK;

	pll_setup_info();
	dram_setup();

	/* Turn on mapping SPI FLASH to 0xbd00_0000 */
	REG32(MCR_A) = REG32(MCR_A) & ~MCR_FLASH_MAP1_DIS_MASK;

	/* Disable WDT and force clearing its interrupt */
	REG32(WDTCTRLR_A) = 0x0;/* Disable */
	REG32(WDTINTRR_A) = (PH1_IP) | (PH2_IP); /* Clear interrupt(s) */

	return;
}
