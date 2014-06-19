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
	unsigned int SOC_IO_MODE_EN; /* IO_MODE_EN : Enable interface IO */
	unsigned int SOC_GPIO_CTRL_2;
	
};

const struct soc_reg_remap_t soc_reg_remap_a0 = {
	.SOC_IO_MODE_EN=0xBB023020,
	.SOC_GPIO_CTRL_2=0xbb000118,
	/*add remap register at here*/
};

/*soc 6266  address map table*/
const struct soc_reg_remap_t soc_reg_remap_b0 = {
	.SOC_IO_MODE_EN=0xBB023018,
	.SOC_GPIO_CTRL_2=0xBB0000F0,
	/*add remap register at here*/
};

static struct soc_reg_remap_t soc_reg_remap;

#define IO_MODE_EN (soc_reg_remap.SOC_IO_MODE_EN)
#define GPIO_CTRL_2 (soc_reg_remap.SOC_GPIO_CTRL_2)

u32_t soc_reg_init(void) {
	u32_t chip_ver;

	REG32(CHIP_ID_REG) = 0xa0000000;
	parameters.soc_id = (REG32(CHIP_ID_REG) & 0xffff);
	chip_ver = ((REG32(CHIP_ID_REG) >> 16) & 0x1f);

	REG32(BOND_CHIP_MODE) = 0xb0000000;
	parameters.bond_id = (REG32(BOND_CHIP_MODE) & 0xff);

	if (parameters.soc_id == SOC_A0) {
		soc_reg_remap = soc_reg_remap_a0;
	} else if (parameters.soc_id == SOC_B0) {
		soc_reg_remap = soc_reg_remap_b0; 
		switch (chip_ver) {
		case 0x4:
			REG32(0xb8001074) |= (0x1 << 11); /* ADDRB29_LOCK_DIS */
		case 0x3:
			REG32(0xb8000108) |= (0x1 << 23); /* LX_ARB_DEL_EN */
			REG32(0xb8001074) |= (0x1 << 31); /* ARB_SEL */
		case 0x2:
		case 0x1:
			break;
		}
	}

	return chip_ver;
}

void uart_mask_init(void){
	
	if(parameters.soc_id==SOC_B0){/*b0,b1*/
		
		switch(parameters.bond_id){
			case CHIP_901:
			case CHIP_906_2:
			case CHIP_907:
			case CHIP_902:
			case CHIP_903:
			case CHIP_96:
			case CHIP_2510:
			case CHIP_96P:
				*((volatile unsigned int *)IO_MODE_EN) = *((volatile unsigned int *)IO_MODE_EN) | (4<<3); //bit [5:3] is 100
				/*begin add by wanghuanyu for uart1*/
				*((volatile unsigned int *)IO_MODE_EN) = *((volatile unsigned int *)IO_MODE_EN) | (1<<4); //bit [5:3] is 100
				/*end add by wanghuanyu for uart1*/
				break;
	
			case CHIP_906_1:
			case CHIP_98B:
			case CHIP_DBG:
			default:
				*((volatile unsigned int *)IO_MODE_EN) = *((volatile unsigned int *)IO_MODE_EN) | (1<<3); //bit [5:3] is 001
				break;
		}
	
	}else{/*a0*/
	
		switch(parameters.bond_id){
			case CHIP_901:
			case CHIP_906_2:
			case CHIP_907:
			case CHIP_902:
			case CHIP_903:
			case CHIP_96:
				*((volatile unsigned int *)IO_MODE_EN) = *((volatile unsigned int *)IO_MODE_EN) & ~(1<<3);//bit[3] is 0
				*((volatile unsigned int *)GPIO_CTRL_2) =*((volatile unsigned int *)GPIO_CTRL_2) | (1<<1);//EN_GPIO[1] is 1
				break;
		
			case CHIP_906_1:
			case CHIP_98B:
			case CHIP_DBG:
			default:
				*((volatile unsigned int *)IO_MODE_EN) = *((volatile unsigned int *)IO_MODE_EN) | (1<<3); //bit[3] is 1
				*((volatile unsigned int *)GPIO_CTRL_2) =*((volatile unsigned int *)GPIO_CTRL_2) & ~(1<<1);//EN_GPIO[1] is 0
				break;
		}	
	}
	/* Enable UART0, UART1 and UART DECT */
	//*((volatile unsigned int *)0xbb023020) =	*((volatile unsigned int *)0xbb023020) | (7<<3);


}

static void apl_pll_pre_set(void) {
    //const pll_info_t *pll_param_p;
    u32_t sysclk_control_reg_25mhz, sysclk_control_reg_40mhz;
    
    /* Retrive PLL register value */
    //pll_param_p = &(parameters.soc.pll_info);

    if(PLLI.set_by == 1) { /* 1-software or 0-pin */
        /* Set CPU to 500MHz so DDR calibration can work correctly
           LX clock is also configured for the following console_init() call */
        sysclk_control_reg_25mhz =  PLLI.sysclk_control_reg_25mhz;
        sysclk_control_reg_25mhz &= ~SYSREG_SYSCLK_CONTROL_OCP0PLL_MASK;
        sysclk_control_reg_25mhz |= (APL_DRAM_CAL_SYSCLK_CONTROL_REG_25MHZ & SYSREG_SYSCLK_CONTROL_OCP0PLL_MASK);
        sysclk_control_reg_40mhz =  PLLI.sysclk_control_reg_40mhz;
        sysclk_control_reg_40mhz &= ~SYSREG_SYSCLK_CONTROL_OCP0PLL_MASK;
        sysclk_control_reg_40mhz |= (APL_DRAM_CAL_SYSCLK_CONTROL_REG_40MHZ & SYSREG_SYSCLK_CONTROL_OCP0PLL_MASK);
        otto_pll_gen1_set(sysclk_control_reg_25mhz, sysclk_control_reg_40mhz,
                          PLLI.mckg_phs_sel_reg, PLLI.mckg_freq_div_reg, 
                          PLLI.lx_pll_sel_reg_25mhz, PLLI.lx_pll_sel_reg_40mhz, 
                          (OTTO_PLL_CPU_SET | OTTO_PLL_DSP_SET | OTTO_PLL_MEM_SET | OTTO_PLL_LX_SET));
    }
    /* When PLLI.set_by is pin, OCP0 is 500MHz, so it doesn't need to be adjusted for calibration */
}

void platform_init_phase_2(void) {
    u32_t chip_ver;
    //const pll_info_t *pll_param_p;

    /* soc registetr remap */
    chip_ver = soc_reg_init();
    uart_mask_init();
    apl_pll_pre_set();
    console_init();
    PRINT_PLR_INFO(chip_ver);

    /* Retrive PLL register value */
    //pll_param_p = &(parameters.soc.pll_info);
    /* PLL patch function on APL platform */

    dram_setup();

    //otto_pll_gen1_set(pll_param_p->sysclk_control_reg_25mhz, pll_param_p->sysclk_control_reg_40mhz, 
    //                  pll_param_p->mckg_phs_sel_reg, pll_param_p->mckg_freq_div_reg, 
    //                  pll_param_p->lx_pll_sel_reg_25mhz, pll_param_p->lx_pll_sel_reg_40mhz, 
    //                  OTTO_PLL_CPU_SET);
    pll_setup();

    /* Turn on mapping SPI FLASH to 0xbd00_0000 */
    REG32(MCR_A) = REG32(MCR_A) & ~MCR_FLASH_MAP1_DIS_MASK;

    /* Disable WDT and force clearing its interrupt */
    REG32(WDTCTRLR_A) = 0x0;/* Disable */
    REG32(WDTINTRR_A) = (PH1_IP) | (PH2_IP); /* Clear interrupt(s) */

    /* Enable LX jitter tolerance. */
    REG32(0xb8001004) = REG32(0xb8001004) | 0x80000000;

    return;
}
