#ifdef __cplusplus
extern "C" {
#endif

#ifdef __OTTO_COMPOSER__
	#undef SECTION_ON_FLASH
	#define SECTION_ON_FLASH
#else
	#include <preloader.h>
#endif

#include <soc.h>
#include <plr_pll_gen1.h>

const u32_t sys_clk_control_dram25[] = { 0x4, 0x0, 0x4, 0x8, 0x0, 0x3, 0x4, 0x6, 0x8, 0x8, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8 };
const u32_t sys_clk_control_dram40[] = { 0x5, 0x0, 0x5, 0xa, 0x0, 0x4, 0x5, 0x7, 0xa, 0xa, 0x0, 0x1, 0x2, 0x4, 0x5, 0x7, 0x8, 0x9, 0xa };
const u32_t sys_clk_mckg_phase90[]   = { 0x3, 0x2, 0x2, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
const u32_t sys_clk_mckg_clk_div[]   = { 0x3, 0x2, 0x2, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

SECTION_ON_FLASH static void
sys_adj_clk_gen1(u32_t *sys_pll_ctl_oc25,
                 u32_t *sys_pll_ctl_oc40,
                 u32_t *sys_mckg_ph_sel,
                 u32_t *sys_mckg_freq_div,
                 u32_t *sys_lx_pll_sel_oc25,
                 u32_t *sys_lx_pll_sel_oc40,
                 u32_t cpu_clk,
                 u32_t dsp_clk,
                 u32_t mem_clk,
                 u32_t lx_clk) {
	u32_t cpu_field;
	u32_t dsp_field;
	u32_t mem_field;
	//u32_t sys_pll_ctl_value;
	u32_t src_oc_mhz;
	u32_t lx_clk_div_offset;

	/*
	 * 1. Leagel range checking
	 */
	/* This check is hoisted to _pll_gen1_set_from_mhz(). */

	/*
	 * 2. Compute the values for the input cpu/dsp/mem/lx clock setting.
	 */
	/* CPU/DSP/MEM: OC == 25MHz */
	mem_field = sys_clk_control_dram25[ ((mem_clk-50)/25) ];
	dsp_field = (dsp_clk/25)-20;
	cpu_field = (cpu_clk/25)-20;
	*sys_pll_ctl_oc25 = (cpu_field << 16) | (dsp_field << 8) | mem_field;

	/* CPU/DSP/MEM: OC == 40MHz */
	mem_field = sys_clk_control_dram40[ ((mem_clk-50)/25) ];
	dsp_field = (dsp_clk/20)-25;
	cpu_field = (cpu_clk/20)-25;
	*sys_pll_ctl_oc40 = (cpu_field << 16) | (dsp_field << 8) | mem_field;

	*sys_mckg_ph_sel = sys_clk_mckg_phase90[((mem_clk-50)/25)];
	*sys_mckg_freq_div = sys_clk_mckg_clk_div[((mem_clk-50)/25)];

	/* LX: OC == 25MHz */
	src_oc_mhz = 25;
	lx_clk_div_offset = 6;
	*sys_lx_pll_sel_oc25 = (((2*lx_clk)/src_oc_mhz) - 2) - lx_clk_div_offset;

	/* Lx: OC == 40MHz */
	src_oc_mhz = 40 / 2;
	lx_clk_div_offset = 8;
	*sys_lx_pll_sel_oc40 = (((2*lx_clk)/src_oc_mhz) - 2) - lx_clk_div_offset;

	return;
}

SECTION_ON_FLASH pll_result_t
_pll_gen1_get_to_mhz(const pll_info_t *pll_reg,
                     pll_gen1_mhz_t *pll_mhz,
                     const u32_t osc_mhz) {
	u32_t is_25MHz, tmp;
	const u32_t *sysclk_contr_reg, *sys_lx_pll_sel;
	u32_t lx_clk_div, lx_clk_div_offset, src_oc_mhz;
	pll_result_t res = PLL_RES_OK;

	if (osc_mhz == 25) {
		is_25MHz = 1;
	} else {
		is_25MHz = 0;
	}

	sysclk_contr_reg = is_25MHz ?
		&(pll_reg->sysclk_control_reg_25mhz):
		&(pll_reg->sysclk_control_reg_40mhz);

	/* for CPU */
	tmp = (*sysclk_contr_reg & SYSREG_SYSCLK_CONTROL_OCP0PLL_MASK) >> SYSREG_SYSCLK_CONTROL_OCP0PLL_FD_S;
	pll_mhz->cpu = is_25MHz ?
		((tmp + 20) * 25):
		((tmp + 25) * 20);

	/* for DSP */
	tmp = (*sysclk_contr_reg & SYSREG_SYSCLK_CONTROL_OCP1PLL_MASK) >> SYSREG_SYSCLK_CONTROL_OCP1PLL_FD_S;
	pll_mhz->dsp = is_25MHz ?
		((tmp + 20) * 25):
		((tmp + 25) * 20);

	/* for MEM */
	tmp = (*sysclk_contr_reg & SYSREG_SYSCLK_CONTROL_SDPLL_MASK) >> SYSREG_SYSCLK_CONTROL_SDPLL_FD_S;
	tmp = is_25MHz ? (tmp + 12) * 25 : (tmp + 15) * 20;
	pll_mhz->mem = tmp >> pll_reg->mckg_freq_div_reg;

	/* for LX */
	sys_lx_pll_sel = is_25MHz ?
		&(pll_reg->lx_pll_sel_reg_25mhz):
		&(pll_reg->lx_pll_sel_reg_40mhz);

	if (is_25MHz) {
		src_oc_mhz = 25;
		lx_clk_div_offset = 6;
	} else {
		src_oc_mhz = 40 / 2;
		lx_clk_div_offset = 8;
	}

	lx_clk_div = (*sys_lx_pll_sel) + lx_clk_div_offset;
	pll_mhz->lx = (src_oc_mhz * (lx_clk_div + 2))/2;

	return res;
}

SECTION_ON_FLASH pll_result_t
_pll_gen1_set_from_mhz(pll_info_t *pll_reg,
                       pll_gen1_mhz_t *pll_mhz) {
	pll_result_t res = PLL_RES_OK;

	if (pll_mhz->mode == PLL_MODE_BY_SW) {
		pll_reg->set_by = 1;
	} else if (pll_mhz->mode == PLL_MODE_BY_PIN) {
		pll_reg->set_by = 0;
	} else {
		res = PLL_RES_BAD_MODE;
		return res;
	}

	if (pll_mhz->cpu < 500) {
		pll_mhz->cpu = 500;
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	}
	if (pll_mhz->cpu > 700) {
		pll_mhz->cpu = 700;
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	}
	if (pll_mhz->dsp < 500) {
		pll_mhz->dsp = 500;
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	}
	if (pll_mhz->dsp > 550) {
		pll_mhz->dsp = 550;
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	}
	if (pll_mhz->mem < 50) {
		pll_mhz->mem = 50;
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	}
	if (pll_mhz->mem > 500) {
		pll_mhz->mem = 500;
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	}
	if (pll_mhz->lx < 100) {
		pll_mhz->lx = 100;
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	}
	if (pll_mhz->lx > 200) {
		pll_mhz->lx = 200;
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	}

	sys_adj_clk_gen1(&pll_reg->sysclk_control_reg_25mhz,
	                 &pll_reg->sysclk_control_reg_40mhz,
	                 &pll_reg->mckg_phs_sel_reg,
	                 &pll_reg->mckg_freq_div_reg,
	                 &pll_reg->lx_pll_sel_reg_25mhz,
	                 &pll_reg->lx_pll_sel_reg_40mhz,
	                 pll_mhz->cpu,
	                 pll_mhz->dsp,
	                 pll_mhz->mem,
	                 pll_mhz->lx);

	return res;
}

#ifdef __cplusplus
}
#endif
