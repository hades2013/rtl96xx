#include <preloader.h>
#include <soc.h>
#include <plr_pll_gen1.h>

#include "rlx5281_cache_op.h"

#include "bspchip.h"
#include "dram/plr_dram_gen2.h"
#include "dram/plr_dram_gen2_memctl.h"
#include "dram/plr_plat_dep.h"


void rlx5281_write_back_invalidate_dcache_cctl();


/*
 * Function implementation 
 */

void plat_memctl_ZQ_model_en(void)
{
    if(memctlc_is_DDR3()){	// DDR 3
        REG32(SYSREG_DDR123_IO_MODE_CONTROL_REG) |= SYSREG_DDR123_IO_MODE_CONTROL_DDR3_MD_MASK;  
    }
}

void plat_memctl_ZQ_force_config(void)
{
    unsigned int zq_force_value0, zq_force_value1, zq_force_value2;
    volatile unsigned int *zq_pad_ctl_reg;
    //zq_force_value0 = 0x200267; /* 176 demo board*/
    //zq_force_value1 = 0x10001;
    //zq_force_value2 = 0x50000009;
    //zq_force_value0 = 0x002be738; /* 176 demo board*/
    //zq_force_value1 = 0x017d017d;
    //zq_force_value2 = 0x58785809;
    //zq_force_value0 = 0x0022b267; /*OCD 60, ODT 75*/
    zq_force_value0 = 0x0022b49f; /*OCD 60, ODT 50*/
    zq_force_value1 = 0x00570057;
    zq_force_value2 = 0x58282809;

    zq_pad_ctl_reg = (volatile unsigned int *)0xB8000118;

    *zq_pad_ctl_reg     = zq_force_value0;
    *(zq_pad_ctl_reg+1) = zq_force_value0;
    *(zq_pad_ctl_reg+2) = zq_force_value0;
    *(zq_pad_ctl_reg+3) = zq_force_value0;
    *(zq_pad_ctl_reg+6) = zq_force_value0;
    *(zq_pad_ctl_reg+7) = zq_force_value0;
    *(zq_pad_ctl_reg+8) = zq_force_value0;
    *(zq_pad_ctl_reg+9) = zq_force_value0;


    *(zq_pad_ctl_reg+4) = zq_force_value1;
    *(zq_pad_ctl_reg+5) = zq_force_value1;
    *(zq_pad_ctl_reg+10)= zq_force_value1;

    *(zq_pad_ctl_reg+11)= zq_force_value2;

    return; //return 0;

}
/*
 * setting clock reverse indication.
 * Can't run in DRAM.
 */
void plat_mem_clk_rev_check(void)
{
    unsigned int *clk_rev_ctl_reg;
    unsigned int clk_rev;
    unsigned int cpu_clk;
    unsigned int mem_clk;
    unsigned int lx_clk;

    clk_rev = 0;
    clk_rev_ctl_reg = (unsigned int *)SYSREG_CMUCTLR_REG;

    cpu_clk = plat_memctl_CPU_clock_MHz();
    mem_clk = plat_memctl_MEM_clock_MHz();
    lx_clk = plat_memctl_LX_clock_MHz();

    if(cpu_clk < mem_clk)
        clk_rev = (clk_rev | SYSREG_OCP0_SMALLER_MASK | SYSREG_OCP1_SMALLER_MASK);

    if(lx_clk < mem_clk){
        clk_rev = (clk_rev | SYSREG_LX0_SMALLER_MASK | SYSREG_LX1_SMALLER_MASK | SYSREG_LX2_SMALLER_MASK);
        //clk_rev = (clk_rev | LX1_SMALLER_MEM ); /* 20110830: We only can change LX1 freq. */
    }
    *clk_rev_ctl_reg = (*clk_rev_ctl_reg & 
                        ~(SYSREG_OCP0_SMALLER_MASK | SYSREG_OCP1_SMALLER_MASK | SYSREG_LX0_SMALLER_MASK | SYSREG_LX1_SMALLER_MASK | SYSREG_LX2_SMALLER_MASK) )
                        | clk_rev ;

    return;

}

 /* CPU related (cache flush) */
void plat_memctl_dcache_flush(void)
{

#if 1
//    rlx5281_write_back_invalidate_dcache_cctl();
parameters._dcache_writeback_invalidate_all();

#else    
    __asm__ volatile("mtc0    $0, $20\n\t"
             "nop\n\t"
             "li      $9, 512\n\t"
             "mtc0    $9, $20\n\t"
             "nop\n\t"
             : /* no output */
             : /* no input */
            );
#endif
    return;

}

#if 0
/* PAD related (Pad driving, static ZQ setting) */
void plat_memctl_dram_pad_drv(unsigned int drv_strength)
{
    unsigned int dummy;
    
    dummy = drv_strength;
}
void plat_memctl_dram_zq_setting(unsigned int is_static, unsigned int odt_ocd_ohm)
{
    unsigned int dummy;
    
    dummy = is_static;
    dummy = odt_ocd_ohm;

}
#endif 

//unsigned int _is_CKSEL_25MHz(void)
//{
//    if(REG32(SYSREG_PIN_STATUS_REG) & SYSREG_PIN_STATUS_CLSEL_MASK)
//        return 0;
//    else
//        return 1;
//}

 /* Platform clock related (CPU clock, LX bus clock, Memory clock) */
unsigned int plat_memctl_CPU_clock_MHz(void)
{
    unsigned int cpu_clk;
    
    cpu_clk = pll_query_freq(PLL_DEV_CPU);

    return cpu_clk;
}

unsigned int plat_memctl_DSP_clock_MHz(void)
{
    unsigned int dsp_clk;

    dsp_clk = pll_query_freq(PLL_DEV_DSP);
    

    return dsp_clk;

}
unsigned int plat_memctl_MEM_clock_MHz(void)
{
    unsigned int mem_clk;

    mem_clk = pll_query_freq(PLL_DEV_MEM);
    
    return mem_clk;

}
unsigned int plat_memctl_LX_clock_MHz(void)
{
    unsigned int lx_freq_mhz;

    lx_freq_mhz = pll_query_freq(PLL_DEV_LX);
    
    return  lx_freq_mhz;

}

 /* Memory controller input signal delay (CLKM delay, CLKM_90 delay, TX delay) */
unsigned int plat_memctl_input_sig_delay(unsigned int first_param, unsigned int second_param, unsigned int third_param)
{
    unsigned int target_ddrkodl_value;
    unsigned int clkm90, clkm, tx_clk;
    volatile unsigned int delay_loop;

    clkm   = first_param;    
    clkm90 = second_param;
    tx_clk = third_param;
 
    target_ddrkodl_value = (clkm90 << 16 | clkm << 8 | tx_clk );
    REG32(SYSREG_DDRCKODL_REG) = target_ddrkodl_value;
    while( REG32(SYSREG_DDRCKODL_REG) != target_ddrkodl_value ){
        _memctl_debug_printf_E("error: ddrkodl not equal write data(0x%08x)\n", target_ddrkodl_value);
        REG32(SYSREG_DDRCKODL_REG) = target_ddrkodl_value;
    }

    /* Delay a little bit for waiting for more stable of the DRAM clock.*/
    delay_loop = 0x10000;
    while(delay_loop--);

    return 0;
}


#if 0
void plat_memctl_choose_delay_value(unsigned int min_value, unsigned int max_value)
{
    unsigned int target_value;


    if( min_value < 8 ){
        min_value = 8;
    }

    if( max_value > 24 ){
        max_value = 24;
    }

    if(min_value != MEMCTL_INVALID_CLKM_DELAY_VALUE){
        target_value = min_value +(((max_value - min_value)) / 2);
    }else{
        target_value = 10;
    }
    plat_memctl_input_sig_delay(target_value, 0, 0);
}
#endif

void plat_memctl_dramclk_en(void)
{
    volatile unsigned int *sysreg_dram_clk_en_reg;
    volatile unsigned int delay_loop;

    sysreg_dram_clk_en_reg = (volatile unsigned int *)SYSREG_DRAM_CLK_EN_REG;
#if 0 /* To tackle instability on 0371 */
    while(*sysreg_dram_clk_en_reg != SYSREG_DRAM_CLK_EN_MASK ){
        *sysreg_dram_clk_en_reg = SYSREG_DRAM_CLK_EN_MASK;
    }
#endif

    /* Delay a little bit for waiting for more stable of the DRAM clock.*/
    delay_loop = 0x10000;
    while(delay_loop--);
}

#if 0
void plat_memctl_IO_PAD_patch(void)
{
    volatile unsigned int *sys_dram_clk_drv_reg; //, *socpnr;

    sys_dram_clk_drv_reg    = (volatile unsigned int *)SYS_DDRCLK_IO_DRI_CTL_REG;
    *sys_dram_clk_drv_reg = 0xe300db;
    return;
}
#endif

void plat_memctl_show_dram_config(void)
{
    unsigned int i;
    volatile unsigned int *phy_reg;
    _memctl_debug_printf_I("\n");
    _memctl_debug_printf_I("DDRKODL(0x%08x):0x%08x\n",\
                 DDRCKODL_A, REG32(DDRCKODL_A));
    _memctl_debug_printf_I("MCR (0x%08x):0x%08x, 0x%08x, 0x%08x, 0x%08x\n", \
                MCR, REG32(MCR), REG32(DCR), REG32(DTR0), REG32(DTR1));
    _memctl_debug_printf_I("DTR2(0x%08x):0x%08x\n", DTR2, REG32(DTR2));
    _memctl_debug_printf_I("PHY Registers(0x%08x):\n", DACCR);
    phy_reg = (volatile unsigned int *)DACCR;
    for(i=0;i<11;i++){
        _memctl_debug_printf_I("0x%08x:0x%08x, 0x%08x, 0x%08x, 0x%08x\n", \
                phy_reg, *(phy_reg), *(phy_reg+1), *(phy_reg+2), *(phy_reg+3) );
        phy_reg+=4;
    }
}

u32_t plat_memctl_calculate_dqrf_delay(u32_t max_w_seq_start, u32_t max_w_len, u32_t max_r_seq_start, u32_t max_r_len) 
{
    u32_t ret_val;

    ret_val = (((max_w_seq_start + (max_w_len/3)) & 0x1f) << 24) | 
              (((max_r_seq_start + max_r_len - 1) & 0x1f) << 16) | 
              (((max_r_seq_start + (max_r_len/2)) & 0x1f) << 8) | 
              (((max_r_seq_start) & 0x1f) << 0);

    return ret_val;
}


