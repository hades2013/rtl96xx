#include <preloader.h>
#include <cpu_utils.h>
#include <plr_pll_gen1.h>
#include "swp_pll_gen1_util.h"
#include "swp_util.h"

#define swp_info_t pll_gen1_info_t
#define PARA_SECTION __attribute__ ((section (".cli_pll_para")))
#include "swp_access_soc.h"

inline static u32_t
_swp_pg1_query_cpu_clk(const pll_gen1_info_t *i) {
    pll_gen1_mhz_t mhz;
    pll_gen1_get_to_mhz(i, &mhz);
    return mhz.cpu;
}
inline static u32_t
_swp_pg1_query_dsp_clk(const pll_gen1_info_t *i) {
    pll_gen1_mhz_t mhz;
    pll_gen1_get_to_mhz(i, &mhz);
    return mhz.dsp;
}
inline static u32_t
_swp_pg1_query_mem_clk(const pll_gen1_info_t *i) {
    pll_gen1_mhz_t mhz;
    pll_gen1_get_to_mhz(i, &mhz);
    return mhz.mem;
}
inline static u32_t
_swp_pg1_query_lx_clk(const pll_gen1_info_t *i) {
    pll_gen1_mhz_t mhz;
    pll_gen1_get_to_mhz(i, &mhz);
    return mhz.lx;
}
u32_t
swp_query_mem_clk(void){
    return _swp_pg1_query_mem_clk(&para_pll_info);
}
u32_t
query_pg1_uart_baudrate(void) {
    /* Foumula: baudrate = LX_CLK_HZ/(16*divisor)
            <-->divisor = LX_CLK_HZ/(16*baudrate) */
    const u32_t rate[6] = {9600, 14400, 19200, 38400, 57600, 115200};
	u32_t idx=0, baudrate;
    pll_gen1_mhz_t mhz;
	pll_gen1_get_to_mhz(&para_pll_info, &mhz);
    baudrate = (mhz.lx * 1000000)/(16 * parameters.soc.peri_info.baudrate_divisor);

	for(idx=0; idx<6; idx++) {
        if ((idx==0) && (baudrate < rate[idx+1])) break;
        else if (idx>=5) break;
        else if ((idx<5) && (baudrate < rate[idx+1])
            && (baudrate >= rate[idx]) ) break;
    }
    //printf("%s = %d\n\n", __FUNCTION__, rate[idx]);
    return rate[idx];
}
u32_t
set_pg1_uart_baud_div(const u32_t baudrate) {
    /* Foumula: baudrate = LX_CLK_HZ/(16*divisor)
            <-->divisor = LX_CLK_HZ/(16*baudrate) */
    u32_t br_div;
    soc_t *rw_soc = parameter_soc_rwp;
    pll_gen1_mhz_t mhz;
	pll_gen1_get_to_mhz(&para_pll_info, &mhz);
    if (0 != baudrate) {
	    br_div = (mhz.lx * 1000000)/(16 * baudrate);
	    if (br_div>=0xffff)	{
	        printf("warning: the baudrate divider, %d, is too large, 0xffff is used\n", br_div);
	        br_div=0xffff;
	    }
    } else {
    	br_div = 0;
    }
	rw_soc->peri_info.baudrate_divisor = br_div;
	//printf("baudrate_divisor = %d\n\n", rw_soc->peri_info.baudrate_divisor);
    return 0;
}
#define define_rv_para(name, _types) _define_rv_para(pll, name, _types)
#define define_rv_para_tab(name, table, _types) _define_rv_para_tab(pll, name, table, _types)
#define define_vv_para(name, _get_, _set_, _types) _define_vv_para(pll, name, _get_, _set_, _types)

define_rv_para(set_by, 0);
define_rv_para(sysclk_control_reg_25mhz, 0);//CPU/DSP/MEM
define_rv_para(sysclk_control_reg_40mhz, 0);//CPU/DSP/MEM
    define_vv_para(cpu_clk, _swp_pg1_query_cpu_clk(info), -1, DPT_DECIMAL);
    define_vv_para(dsp_clk, _swp_pg1_query_dsp_clk(info), -1, DPT_DECIMAL);
define_rv_para(mckg_phs_sel_reg, 0);
define_rv_para(mckg_freq_div_reg, 0); 
    define_vv_para(mem_clk, _swp_pg1_query_mem_clk(info), -1, DPT_DECIMAL);
define_rv_para(lx_pll_sel_reg_25mhz, 0);
define_rv_para(lx_pll_sel_reg_40mhz, 0);
    define_vv_para(lx_clk, _swp_pg1_query_lx_clk(info), -1, DPT_DECIMAL);

extern as_para_t cli_pll_para_begin, cli_pll_para_end;
SECTION_ON_FLASH int
get_pll_gen1_info(const pll_gen1_info_t *info, const char *pname, char *buf) {
    return swp_access_soc_get(info, pname, buf, &cli_pll_para_begin, &cli_pll_para_end);
}
SECTION_ON_FLASH int
set_pll_gen1_para(pll_gen1_info_t *info, const char *pname, const char *value) {
    return swp_access_soc_set(info, pname, value, &cli_pll_para_begin, &cli_pll_para_end);
}
SECTION_ON_FLASH void
dump_pll_gen1_info(const pll_gen1_info_t *info, const char *fmt, u32_t flags, u32_t flags_mask) {
    return swp_access_soc_dump(info, fmt, flags, flags_mask, &cli_pll_para_begin, &cli_pll_para_end,
        ((u32_t)&__pll_para_lx_clk)-((u32_t)&__pll_para_set_by));
}


