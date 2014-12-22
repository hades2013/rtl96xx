#include <preloader.h>
#include <cpu_utils.h>
#include <plr_pll_gen1.h>
#include <bspchip.h>

void cmu_division(u32_t divisor);
u32_t query_pg1_uart_baudrate(void);
void cmu_setup_info(void);

void pll_setup(void) {
#if defined(CMU_DIVISOR)
#if ((CMU_DIVISOR != 2)   && (CMU_DIVISOR != 4)  && (CMU_DIVISOR != 8)  && \
     (CMU_DIVISOR != 16)  && (CMU_DIVISOR != 32) && (CMU_DIVISOR != 64) && \
     (CMU_DIVISOR != 128))
#error CMU_DIVISOR must be 2, 4, 8, 16, 32, 64, or 128.
#endif
    cmu_division(CMU_DIVISOR);
#endif

    parameters._pll_query_freq = pll_query_freq;
    parameters._udelay = udelay;
    pll_gen1_setup();

#if defined(CMU_DIVISOR)
    ((volatile peripheral_info_t *)&parameters.soc.peri_info)->baudrate_divisor =
	    (pll_query_freq(PLL_DEV_LX) * 1000000) / (16 * query_pg1_uart_baudrate()) - 1;
#endif

    unsigned int *reg;
    unsigned int clk_rev;
    unsigned int mem_clk;
    unsigned int lx_clk;

    clk_rev = 0;
    reg = (unsigned int *)SYSREG_CMUCTLR_REG;

    mem_clk = pll_query_freq(PLL_DEV_MEM);
    lx_clk = pll_query_freq(PLL_DEV_LX);

    if(lx_clk < mem_clk){
        clk_rev = (clk_rev | SYSREG_LX0_SMALLER_MASK | SYSREG_LX1_SMALLER_MASK | SYSREG_LX2_SMALLER_MASK);
    }

    *reg = (*reg & 
           ~(SYSREG_LX0_SMALLER_MASK | SYSREG_LX1_SMALLER_MASK | SYSREG_LX2_SMALLER_MASK) )
           | clk_rev ;

    /* Enable LX jitter tolerance. */
    reg = (unsigned int *)0xB8001004;
    *reg = *reg | 0x80000000;

    return;
}

void pll_setup_info(void) {
	if (PLLI.set_by == 1) {
		pblr_puts("II: PLL is set by SW:");
	} else {
		pblr_puts("II: PLL is set by HW pins:");
    }

	printf(" RLX5281: %dMHz; RLX5181: %dMHz; MEM: %dMHz; LX: %dMHz\n",
	       pll_query_freq(PLL_DEV_CPU),
	       pll_query_freq(PLL_DEV_DSP),
	       pll_query_freq(PLL_DEV_MEM),
	       pll_query_freq(PLL_DEV_LX));

#if defined(CMU_DIVISOR)
	cmu_setup_info();
#endif
    return;
}
