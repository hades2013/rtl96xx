#include <preloader.h>
#include <cpu_utils.h>
#include <plr_pll_gen1.h>
#include <bspchip.h>

void cmu_division(u32_t divisor);
u32_t query_pg1_uart_baudrate(void);
void cmu_setup_info(void);

void pll_setup(void) {
	volatile u32_t delay_loop = 0x10000000;

	while (delay_loop--);

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

#define CMUCTRL (*((volatile u32_t *)0xB8000308))
	unsigned int cmuctrl;
	unsigned int oc0_clk, oc1_clk, mem_clk, lx_clk;

	cmuctrl = CMUCTRL & (~(SYSREG_OCP0_SMALLER_MASK |
	                       SYSREG_OCP1_SMALLER_MASK |
	                       SYSREG_LX0_SMALLER_MASK  |
	                       SYSREG_LX1_SMALLER_MASK  |
	                       SYSREG_LX2_SMALLER_MASK));

	mem_clk = pll_query_freq(PLL_DEV_MEM);

	oc0_clk = pll_query_freq(PLL_DEV_CPU);
	oc1_clk = pll_query_freq(PLL_DEV_DSP);
	lx_clk  = pll_query_freq(PLL_DEV_LX);

	if (oc0_clk <= mem_clk) {
		cmuctrl |= SYSREG_OCP0_SMALLER_MASK;
	}

	if (oc1_clk <= mem_clk) {
		cmuctrl |= SYSREG_OCP1_SMALLER_MASK;
	}

	if (lx_clk <= mem_clk) {
		cmuctrl |= (SYSREG_LX0_SMALLER_MASK |
		            SYSREG_LX1_SMALLER_MASK |
		            SYSREG_LX2_SMALLER_MASK);
	}

	CMUCTRL = cmuctrl;
	udelay(100);

    REG32(WDTCNTRR_A) = (1ul<<31);// Clear watchdog counters
	REG32(WDTINTRR_A) = (PH1_IP) | (PH2_IP); /* Clear interrupt(s) */
    REG32(WDTCTRLR_A) = (1ul<<31) | (1ul<<29) | (0x1 << 22) | (0x1 << 15);/* Enable */

	/* Enable LX jitter tolerance. */
	REG32(0xB8001004) = REG32(0xB8001004) | 0x80000000;

    REG32(WDTCTRLR_A) = 0;/* Disable */

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
