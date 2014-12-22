#include <preloader.h>
#include "swp_util.h"
#include "rlx5281.h"
#include "rlx5281_cache_op.h"

#define swp_info_t soc_t        // soc_t doesn't have cpu related info
#define PARA_SECTION __attribute__ ((section (".cli_cpu_para")))
#include "swp_access_soc.h"

#define CCTL0_DWALLOC_ON		(1 << 7)
#define CCTL0_DWALLOC_OFF		(1 << 15)
#define CCTL0_LUB_ON			(1 << 26)
#define CCTL0_LUB_OFF			(1 << 27)

#define CCTL3_F_DWALLOC			(1 << 12)
#define CCTL3_F_LUB				(1 << 25)
      
#define en_dcache_wa() ({			\
	asm_mtc0(0, CCTL_REG);				\
    asm_mtc0(CCTL0_DWALLOC_ON, CCTL_REG);})

#define dis_dcache_wa() ({		\
    asm_mtc0(0, CCTL_REG); 				\
    asm_mtc0(CCTL0_DWALLOC_OFF, CCTL_REG);})

#define query_dcache_wa(val) (val=(CCTL3_F_DWALLOC==(asm_mfc0_3(CCTL_REG)&CCTL3_F_DWALLOC)))

#define en_load_used_buff()	({		\
	asm_mtc0(0, CCTL_REG);				\
    asm_mtc0(CCTL0_LUB_ON, CCTL_REG);})

#define dis_load_used_buff() ({		\
    asm_mtc0(0, CCTL_REG); 				\
    asm_mtc0(CCTL0_LUB_OFF, CCTL_REG);})

#define query_load_used_buff(val) (val=(CCTL3_F_LUB==(asm_mfc0_3(CCTL_REG)&CCTL3_F_LUB)))

/* BPCTL (LX CP0 Reg4)
    | 31-20 |  19-18   |  17-16  |  15-6 |   5   |   4  |   3   |    2     |   1    |    0     |
    | Rsrvd | WayMask? | BTBSize | Rsrvd | CBPOn | RSOn | BTBOn | CBPInval | Rsrvd  | BTBInval |
*/
#define en_bran_predic()  (asm_mtlxc0(0x38, BPCTL_REG))

#define dis_bran_predic() (asm_mtlxc0(0x0, BPCTL_REG)) 

#define query_bran_predic(val) (val=(0x38==asm_mflxc0(BPCTL_REG)))

SECTION_ON_FLASH static unsigned int 
cli_cpu_get_bran_predic(void)
{
	unsigned int res;
	query_bran_predic(res);
	return res;
}

SECTION_ON_FLASH static int 
cli_cpu_set_bran_predic(unsigned int val)
{
    if(1==val) {
        en_bran_predic();
    } else {
        dis_bran_predic();
    }
    return 0;
}

SECTION_ON_FLASH static unsigned int 
cli_cpu_get_load_used_buff(void)
{
    unsigned int res;
    query_load_used_buff(res);
    return res;
}


SECTION_ON_FLASH static int 
cli_cpu_set_load_used_buff(unsigned int val)
{
    if(1==val){
      	en_load_used_buff();
    } else {
    	dis_load_used_buff();
    }
	return 0;
}

SECTION_ON_FLASH static unsigned int 
cli_cpu_get_dcache_wa(void)
{
    unsigned int res;
    query_dcache_wa(res);
    return res;
}

SECTION_ON_FLASH static int 
cli_cpu_set_dcache_wa(unsigned int val)
{
    if(1==val) {
        en_dcache_wa();
    } else {
        dis_dcache_wa();
    }
    return 0;
}

#define define_vv_para(name, _get_, _set_, _types) _define_vv_para(cpu, name, _get_, _set_, _types)

/* parameters declare here */
    
define_vv_para(branch_prediction,
    cli_cpu_get_bran_predic(), 
    cli_cpu_set_bran_predic(v), DPT_DECIMAL);
define_vv_para(dcache_write_alloc, 
    cli_cpu_get_dcache_wa(), 
    cli_cpu_set_dcache_wa(v), DPT_DECIMAL);
define_vv_para(load_used_buff,
    cli_cpu_get_load_used_buff(), 
    cli_cpu_set_load_used_buff(v), DPT_DECIMAL);

extern as_para_t cli_cpu_para_begin, cli_cpu_para_end;
SECTION_ON_FLASH int
get_cpu_info(const char *pname, char *buf) {
    return swp_access_soc_get(NULL, pname, buf, &cli_cpu_para_begin, &cli_cpu_para_end);
}
SECTION_ON_FLASH int
set_cpu_para(const char *pname, const char *value) {
    return swp_access_soc_set(NULL, pname, value, &cli_cpu_para_begin, &cli_cpu_para_end);
}
SECTION_ON_FLASH void
dump_cpu_info(const char *fmt, u32_t flags, u32_t flags_mask) {
    return swp_access_soc_dump(NULL, fmt, flags, flags_mask, &cli_cpu_para_begin, &cli_cpu_para_end, 
    ((u32_t)&__cpu_para_dcache_write_alloc)-((u32_t)&__cpu_para_branch_prediction));
}

