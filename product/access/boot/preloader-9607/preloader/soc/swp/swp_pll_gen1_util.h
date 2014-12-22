#ifndef SWP_PLL_GEN1_UTIL_H
#define SWP_PLL_GEN1_UTIL_H
int get_pll_gen1_info(const pll_gen1_info_t *info, const char *pname, char *buf);
int set_pll_gen1_para(pll_gen1_info_t *info, const char *pname, const char *value);
void dump_pll_gen1_info(const pll_gen1_info_t *info, const char *fmt, u32_t flags, u32_t flags_mask);
u32_t query_pg1_uart_baudrate(void); 
u32_t set_pg1_uart_baud_div(const u32_t baudrate);
#endif //SWP_PLL_GEN1_UTIL_H


