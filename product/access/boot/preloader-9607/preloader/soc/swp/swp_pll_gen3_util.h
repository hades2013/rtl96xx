#ifndef SWP_PLL_GEN3_UTIL_H
#define SWP_PLL_GEN3_UTIL_H
int get_pll_gen3_info(const pll_gen3_info_t *info, const char *pname, char *buf);
int set_pll_gen3_para(pll_gen3_info_t *info, const char *pname, const char *value);
void dump_pll_gen3_info(const pll_gen3_info_t *info, const char *fmt, u32_t flags, u32_t flags_mask);

#endif //SWP_PLL_GEN1_UTIL_H


