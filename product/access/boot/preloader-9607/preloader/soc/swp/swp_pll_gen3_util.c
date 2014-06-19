#include <preloader.h>
#include <cpu_utils.h>
#include <plr_pll_gen3.h>
#include "swp_pll_gen3_util.h"
#include "swp_util.h"

#define swp_info_t pll_gen3_info_t
#define PARA_SECTION __attribute__ ((section (".cli_pll_para")))
#include "swp_access_soc.h"

inline static u32_t
_swp_pg3_query_cpu_clk(const pll_gen3_info_t *i) {
    pll_gen3_mhz_t mhz;
    pll_gen3_get_to_mhz(i, &mhz);
    return mhz.cpu;
}
inline static u32_t
_swp_pg3_query_mem_clk(const pll_gen3_info_t *i) {
    pll_gen3_mhz_t mhz;
    pll_gen3_get_to_mhz(i, &mhz);
    return mhz.mem;
}

u32_t
swp_query_mem_clk(void){
    return _swp_pg3_query_mem_clk(&para_pll_info);
}

#define define_rv_para(name, _types) _define_rv_para(pll, name, _types)
#define define_rv_para_tab(name, table, _types) _define_rv_para_tab(pll, name, table, _types)
#define define_vv_para(name, _get_, _set_, _types) _define_vv_para(pll, name, _get_, _set_, _types)

define_rv_para(set_by, 0);
define_rv_para(pll_cpu_ctl0, 0);//CPU/DSP/MEM
define_rv_para(pll_cpu_ctl1, 0);//CPU/DSP/MEM
define_rv_para(pll_cpu_misc_ctrl, 0);//CPU/DSP/MEM
    define_vv_para(cpu_clk, _swp_pg3_query_cpu_clk(info), -1, DPT_DECIMAL);
define_rv_para(pll_mem_ctl0, 0);//CPU/DSP/MEM
define_rv_para(pll_mem_ctl1, 0);//CPU/DSP/MEM
define_rv_para(pll_mem_misc_ctrl, 0);//CPU/DSP/MEM
    define_vv_para(mem_clk, _swp_pg3_query_mem_clk(info), -1, DPT_DECIMAL);

extern as_para_t cli_pll_para_begin, cli_pll_para_end;
SECTION_ON_FLASH int
get_pll_gen3_info(const pll_gen3_info_t *info, const char *pname, char *buf) {
    return swp_access_soc_get(info, pname, buf, &cli_pll_para_begin, &cli_pll_para_end);
}
SECTION_ON_FLASH int
set_pll_gen3_para(pll_gen3_info_t *info, const char *pname, const char *value) {
    return swp_access_soc_set(info, pname, value, &cli_pll_para_begin, &cli_pll_para_end);
}
SECTION_ON_FLASH void
dump_pll_gen3_info(const pll_gen3_info_t *info, const char *fmt, u32_t flags, u32_t flags_mask) {
    swp_access_soc_dump(info, fmt, flags, flags_mask, &cli_pll_para_begin, &cli_pll_para_end,
        ((u32_t)&__pll_para_mem_clk)-((u32_t)&__pll_para_set_by));
}


