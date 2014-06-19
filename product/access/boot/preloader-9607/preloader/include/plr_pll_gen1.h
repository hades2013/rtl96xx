#ifdef __cplusplus
extern "C" {
#endif

#ifndef PLR_PLL_GEN1_H
#define PLR_PLL_GEN1_H

/* Macro definitions */
#define APL_DRAM_CAL_SYSCLK_CONTROL_REG_25MHZ 0x00000000
#define APL_DRAM_CAL_SYSCLK_CONTROL_REG_40MHZ 0x00000000
#define APL_DRAM_CAL_LX_PLL_SEL_REG_25MHZ     0x00000008
#define APL_DRAM_CAL_LX_PLL_SEL_REG_40MHZ     0x0000000a
#define APL_DRAM_CAL_MCKG_PHS_SEL_REG             0x00000002
#define APL_DRAM_CAL_MCKG_FREQ_DIV_REG            0x00000000

#define OTTO_PLL_CPU_SET        (1 << 0)
#define OTTO_PLL_DSP_SET        (1 << 1)
#define OTTO_PLL_MEM_SET        (1 << 2)
#define OTTO_PLL_LX_SET         (1 << 3)

#define PLL_MODE_BY_PIN 0
#define PLL_MODE_BY_SW  1
typedef struct {
	u32_t mode;
	u32_t cpu;
	u32_t lx;
	u32_t mem;
	u32_t dsp;
} pll_gen1_mhz_t;

pll_result_t _pll_gen1_get_to_mhz(const pll_info_t *pll_reg,
                                  pll_gen1_mhz_t *pll_mhz,
                                  const u32_t osc_mhz);
pll_result_t _pll_gen1_set_from_mhz(pll_info_t *pll_reg,
                                    pll_gen1_mhz_t *pll_mhz);

pll_result_t pll_gen1_get_to_mhz(const pll_info_t *pll_reg,
                                 pll_gen1_mhz_t *pll_mhz);
#if (defined(PLR_ENABLE_PLL_SET) || \
     defined(CONFIG_STANDALONE_UBOOT))
pll_result_t pll_gen1_set_from_mhz(pll_info_t *pll_reg,
                                   pll_gen1_mhz_t *pll_mhz);
#endif

#define SYSREG_SYSTEM_STATUS_REG_A        (0xB8000044)
#define SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_FD_S  (2)
#define SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_MASK  (1 << SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_FD_S)
#define SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_OCP0PLL SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_MASK
#define SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_LXPLL (0 << SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_FD_S)

#define SYSREG_PIN_STATUS_REG_A           (0xB8000100)
#define SYSREG_DRAM_CLK_EN_FD_S         (0)
#define SYSREG_DRAM_CLK_EN_MASK         (1 << SYSREG_DRAM_CLK_EN_FD_S)
#define SYSREG_PIN_STATUS_CLSEL__FD_S       (5)
#define SYSREG_PIN_STATUS_CLSEL_MASK        (1 << SYSREG_PIN_STATUS_CLSEL__FD_S)
#define SYSREG_LX_DEFAULT_MHZ           (200)
#define SYSREG_MCKG_FREQ_DIV_FD_S           (0)
#define SYSREG_MCKG_FREQ_DIV_MASK           (0x3 << SYSREG_MCKG_FREQ_DIV_FD_S)


#define SYSREG_SYSCLK_CONTROL_REG_A       (0xB8000200)
#define SYSREG_MCKG_PHS_SEL_REG_A         (0xB8000220)
#define SYSREG_MCKG_FREQ_DIV_REG_A        (0xB8000224)
#define SYSREG_LX_PLL_SEL_REG_A           (0xB8000228)
#define SYSREG_MCKG_PHS_SEL_PHS_FD_S        (0)
#define SYSREG_MCKG_PHS_SEL_PHS_MASK        (0x7 << SYSREG_MCKG_PHS_SEL_PHS_FD_S)
#define SYSREG_SYSCLK_CONTROL_SDPLL_FD_S    (0)
#define SYSREG_SYSCLK_CONTROL_SDPLL_MASK    (0x1f << SYSREG_SYSCLK_CONTROL_SDPLL_FD_S)
#define SYSREG_SYSCLK_CONTROL_OCP1PLL_FD_S  (8)
#define SYSREG_SYSCLK_CONTROL_OCP1PLL_MASK  (0x1f << SYSREG_SYSCLK_CONTROL_OCP1PLL_FD_S)
#define SYSREG_SYSCLK_CONTROL_OCP0PLL_FD_S  (16)
#define SYSREG_SYSCLK_CONTROL_OCP0PLL_MASK  (0x1f << SYSREG_SYSCLK_CONTROL_OCP0PLL_FD_S)
#define SYSREG_DDRCKODL_DDRCLM_TAP_FD_S     (8)
#define SYSREG_DDRCKODL_DDRCLM_TAP_MASK     (0x1f << SYSREG_DDRCKODL_DDRCLM_TAP_FD_S)
#define SYSREG_DDRCKODL_PH90_TAP_FD_S       (16)
#define SYSREG_DDRCKODL_PH90_TAP_MASK       (0x1f << SYSREG_DDRCKODL_PH90_TAP_FD_S)

#define PLLI (parameters.soc.pll_info)
/* Funciton prototypes */
void otto_pll_gen1_set( u32_t sys_pll_ctl_oc25,    u32_t sys_pll_ctl_oc40,\
                        u32_t sys_mckg_ph_sel,     u32_t sys_mckg_freq_div,\
                        u32_t sys_lx_pll_sel_oc25, u32_t sys_lx_pll_sel_oc40,\
                        u32_t flag);
extern void pll_gen1_setup(void);
extern u32_t pll_query_freq(u32_t dev);
extern u32_t pll_gen1_is_CKSEL_25MHz(void);
#endif /* #ifndef PLR_PLL_GEN1_H */

#ifdef __cplusplus
}
#endif
