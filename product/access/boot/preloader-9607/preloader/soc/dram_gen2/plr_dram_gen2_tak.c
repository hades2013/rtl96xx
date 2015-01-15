#include <preloader.h>
#include "plr_dram_gen2.h"
#include "plr_dram_gen2_memctl_tak.h"
#include "plr_plat_dep.h"
#include "concur_test.h"
#include "bspchip_tak.h"

u32_t pll_query_freq(u32_t);

/* 140307,defines in plr_dram_gen2_memctl_tak.h are wrong about both masks. */
#undef DCR_BANKCNT_MASK
#undef DCR_DBUSWID_MASK
#define DCR_BANKCNT_MASK (0x3 << DCR_BANKCNT_FD_S)
#define DCR_DBUSWID_MASK (0x3 << DCR_DBUSWID_FD_S)

#define DCR_FAST_RX_FD_S (14)
#define DCR_BSTREF_FD_S  (13)
#define DCR_FAST_RX_MASK (0x1 << DCR_FAST_RX_FD_S)
#define DCR_BSTREF_MASK  (0x1 << DCR_BSTREF_FD_S)

#define plat_memctl_MEM_clock_MHz() pll_query_freq(PLL_DEV_MEM)
#define memctlc_ddr2_dll_reset()    memctlc_ddr2_mrs_setting()
#define memctlc_ddr3_dll_reset()    memctlc_ddr3_mrs_setting()
#define memctlc_ddr1_dll_reset()    memctlc_ddr1_mrs_setting()
#define plat_mem_clk_rev_check()

#define _set_dmcr                 tak__set_dmcr
#define memctlc_set_DRAM_buswidth tak_memctlc_set_DRAM_buswidth
#define memctlc_set_DRAM_colnum   tak_memctlc_set_DRAM_colnum
#define memctlc_set_DRAM_rownum   tak_memctlc_set_DRAM_rownum
#define memctlc_set_DRAM_banknum  tak_memctlc_set_DRAM_banknum
#define memctlc_set_DRAM_chipnum  tak_memctlc_set_DRAM_chipnum
#define memctl_dram_para_set      tak_memctl_dram_para_set
#define memctlc_dram_size_detect  tak_memctlc_dram_size_detect
#define memctlc_ZQ_calibration    tak_memctlc_ZQ_calibration
#define auto_cali_value           tak_auto_cali_value
#define get_memory_dram_ocd_parameters tak_get_memory_dram_ocd_parameters
#define get_memory_dram_odt_parameters tak_get_memory_dram_odt_parameters
#define show_dram_config          tak_show_dram_config
#define memctlc_set_dqm_delay     tak_memctlc_set_dqm_delay
#define memctl_dqm_pattern        tak_memctl_dqm_pattern
#define memctlc_set_dqm_delay_patch tak_memctlc_set_dqm_delay_patch
#define memctlc_config_DataFullMask tak_memctlc_config_DataFullMask
#define memctlc_config_DRAM_size  tak_memctlc_config_DRAM_size
#define memctlc_ZQ_config         tak_memctlc_ZQ_config
#define memctlc_DDR3_ZQ_long_calibration tak_memctlc_DDR3_ZQ_long_calibration
#define memctlc_ZQ_calibration    tak_memctlc_ZQ_calibration
#define tRFC_Spec_DDR2            tak_tRFC_Spec_DDR2
#define tRFC_Spec_DDR3            tak_tRFC_Spec_DDR3
#define memctlc_config_DTR        tak_memctlc_config_DTR
#define memctlc_DRAM_DLL_reset    tak_memctlc_DRAM_DLL_reset
#define memctlc_dram_phy_reset    tak_memctlc_dram_phy_reset
#define DDR1_setup_MRS            tak_DDR1_setup_MRS
#define DDR2_setup_MRS            tak_DDR2_setup_MRS
#define DDR3_setup_MRS            tak_DDR3_setup_MRS
#define get_dram_size             tak_get_dram_size
#define dram_software_calibration tak_dram_software_calibration
#define dram_display_param_info   tak_dram_display_param_info
#define dram_static_calibration   tak_dram_static_calibration
#define memctlc_DBFM_enable       tak_memctlc_DBFM_enable
#define dram_setup                tak_dram_setup
#define _memctl_delay_clkm_cycles tak__memctl_delay_clkm_cycles
#define _memctl_update_phy_param  tak__memctl_update_phy_param
#define get_memory_memctl_dq_write_delay_parameters tak_get_memory_memctl_dq_write_delay_parameters

#define memctl_sync_write_buf     tak_memctl_sync_write_buf
#define _memctl_set_phy_delay_all tak__memctl_set_phy_delay_all
#define _memctl_set_phy_delay_dqrf tak__memctl_set_phy_delay_dqrf
#define pat_ary                   tak_pat_ary
#define _write_pattern_1          tak__write_pattern_1
#define _verify_pattern_1         tak__verify_pattern_1
#define _write_pattern            tak__write_pattern
#define _verify_pattern           tak__verify_pattern
#define _memctl_result_to_DQ_RW_Array  tak__memctl_result_to_DQ_RW_Array
#define _memctl_find_proper_RW_dealy   tak__memctl_find_proper_RW_dealy
#define _DDR_Calibration_Full_Scan     tak__DDR_Calibration_Full_Scan
#define _DDR_Calibratoin_One_Dimension tak__DDR_Calibratoin_One_Dimension
#define _memctl_disable_hw_auto_cali   tak__memctl_disable_hw_auto_cali
#define _memctl_enable_hw_auto_cali    tak__memctl_enable_hw_auto_cali
#define DDR_Calibration           tak_DDR_Calibration
void _memctl_update_phy_param(void);

//#define DRAM_PARAM_DEBUG

static void _memctl_delay_clkm_cycles(unsigned int delay_cycles);
void memctlc_DDR3_ZQ_long_calibration(void);

void _DRAM_PLL_CLK_power_switch(unsigned char power_on) {
	volatile unsigned int *dpcpw;
	unsigned int    delay_tmp;

#ifdef FORCE_8BIT_MODE
		return;
#endif

	dpcpw = (unsigned int *)0xB8000204;
	if(power_on)
		*dpcpw &=  ~(1<<4);
	else  //power off
		*dpcpw |=  (1<<4);
	delay_tmp=0x3fff;
	while(delay_tmp--);
	return;
}

#if 0
void _set_dmcr(unsigned int dmcr_value)
{
    /* Error cheching here ?*/
    REG32(DMCR) = dmcr_value;
    while(REG32(DMCR) & 0x80000000);

    return;
}

void memctlc_set_DRAM_buswidth(unsigned int buswidth)
{
    volatile unsigned int *dcr;
    unsigned int t_cas, dmcr_value;
    /* get DCR value */
    dcr = (unsigned int *)DCR;


    switch (buswidth){
        case 8:
            if(memctlc_is_DDR()){
                t_cas = ((REG32(DTR0) & (~(DTR0_CAS_MASK))) >> DTR0_CAS_FD_S);
                /*0:2.5, 1:2, 2:3*/
                if(t_cas == 0)
                    dmcr_value = 0x00100062;
                else if(t_cas == 1)
                    dmcr_value = 0x00100022;
                else if(t_cas == 2)
                    dmcr_value = 0x00100032;
                else{
                    printf("%s, %d: Error t_cas value(%d)\n", __FUNCTION__, __LINE__, t_cas);
                    return;
                }
                _set_dmcr(dmcr_value);
            }
            *dcr = (*dcr & (~((unsigned int)DCR_DBUSWID_MASK)));
            break;

        case 16:
            if(memctlc_is_DDR()){
                t_cas = ((REG32(DTR0) & (~(DTR0_CAS_MASK))) >> DTR0_CAS_FD_S);
                /*0:2.5, 1:2, 2:3*/
                if(t_cas == 0)
                    dmcr_value = 0x00100062;
                else if(t_cas == 1)
                    dmcr_value = 0x00100022;
                else if(t_cas == 2)
                    dmcr_value = 0x00100032;
                else{
                    printf("%s, %d: Error t_cas value(%d)\n", __FUNCTION__, __LINE__, t_cas);
                    return;
                }
                _set_dmcr(dmcr_value);
            }
            *dcr = (*dcr & (~((unsigned int)DCR_DBUSWID_MASK))) | (unsigned int)(1<<DCR_DBUSWID_FD_S);
            break;

        default:
            _memctl_debug_printf("%s, %d: Error buswidth value(%d)\n", __FUNCTION__, __LINE__, buswidth);
            break;
    }

    return;
}


void memctlc_set_DRAM_colnum(unsigned int col_num)
{
    volatile unsigned int *dcr;

    /* get DCR value */
    dcr = (unsigned int *)DCR;


    switch (col_num){
        case 256:
            *dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK)));
            break;

        case 512:
            *dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(1<<DCR_COLCNT_FD_S);
            break;

        case 1024:
            *dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(2<<DCR_COLCNT_FD_S);
            break;

        case 2048:
            *dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(3<<DCR_COLCNT_FD_S);
            break;

        case 4096:
            *dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(4<<DCR_COLCNT_FD_S);
            break;

        default:
            _memctl_debug_printf("%s, %d: Error column number value(%d)\n", __FUNCTION__, __LINE__, col_num);
            break;
    }

    return;


}
void memctlc_set_DRAM_rownum(unsigned int row_num)
{
    volatile unsigned int *dcr;

    /* get DCR value */
    dcr = (unsigned int *)DCR;


    switch (row_num){
        case 2048:
            *dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK)));
            break;

        case 4096:
            *dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(1<<DCR_ROWCNT_FD_S);
            break;

        case 8192:
            *dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(2<<DCR_ROWCNT_FD_S);
            break;

        case 16384:
            *dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(3<<DCR_ROWCNT_FD_S);
            break;

        case (32*1024):
            *dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(4<<DCR_ROWCNT_FD_S);
            break;

        case (64*1024):
            *dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(5<<DCR_ROWCNT_FD_S);
            break;

        default:
            _memctl_debug_printf("%s, %d: Error row number value(%d)\n", __FUNCTION__, __LINE__, row_num);
            break;
    }

    return;


}
void memctlc_set_DRAM_banknum(unsigned int bank_num)
{
    volatile unsigned int *dcr, *dmcr;

    /* get DCR value */
    dcr = (unsigned int *)DCR;
    dmcr = (unsigned int *)DMCR;


    if(memctlc_is_DDR()){
        switch (bank_num){
            case 2:
                *dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK)));
                break;

            case 4:
                *dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK))) | (unsigned int)(1<<DCR_BANKCNT_FD_S);
                break;

            default:
                _memctl_debug_printf("%s, %d: Error DDR1 bank number value(%d)\n", __FUNCTION__, __LINE__, bank_num);
                break;
        }
    }else{
        switch (bank_num){
            case 4:
                *dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK))) | (unsigned int)(1<<DCR_BANKCNT_FD_S);
                break;

            case 8:
                *dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK))) | (unsigned int)(2<<DCR_BANKCNT_FD_S);
                break;

            default:
                _memctl_debug_printf("%s, %d: Error DDR2/3 bank number value(%d)\n", __FUNCTION__, __LINE__, bank_num);
                break;
        }
    }

    return;

}
void memctlc_set_DRAM_chipnum(unsigned int chip_num)
{
    volatile unsigned int *dcr;

    /* get DCR value */
    dcr = (unsigned int *)DCR;


    switch (chip_num){
        case 1:
            *dcr = (*dcr & (~((unsigned int)DCR_DCHIPSEL_MASK)));
            break;

        case 2:
            *dcr = (*dcr & (~((unsigned int)DCR_DCHIPSEL_MASK))) | (unsigned int)(1<<DCR_DCHIPSEL_FD_S);
            break;

        default:
            _memctl_debug_printf("%s, %d: Error chip number value(%d)\n", __FUNCTION__, __LINE__, chip_num);
            break;
    }

    return;

}


int memctl_dram_para_set(unsigned int width, unsigned int row, unsigned int column, \
             unsigned int bank,  unsigned int cs)
{
    //unsigned int dcr_value;
    //volatile unsigned int *p_dcr;


    _memctl_debug_printf("%s, %d: width(%d), row(%d), column(%d), bank(%d), cs(%d)\n",\
                 __FUNCTION__, __LINE__, width, row, column, bank, cs);
    memctlc_set_DRAM_buswidth(width);
    memctlc_set_DRAM_colnum(column);
    memctlc_set_DRAM_rownum(row);
    memctlc_set_DRAM_banknum(bank);
    memctlc_set_DRAM_chipnum(cs);

    /* Reset PHY FIFO pointer */
    memctlc_dram_phy_reset();

    return MEMCTL_DRAM_PARAM_OK;
}


/* Function Name: 
 *  memctlc_dram_size_detect
 * Descripton:
 *  Detect the size of current DRAM Chip
 * Input:
 *  None
 * Output:
 *  None
 * Return:
 *  The number of bytes of current DRAM chips
 * Note:
 *  None
 */
unsigned int memctlc_dram_size_detect(void)
{
    //unsigned int i;
    unsigned int width, row, col, bk, cs, max_bk;
    volatile unsigned int *p_dcr;
    volatile unsigned int *test_addr0, *test_addr1;
    volatile unsigned int test_v0, test_v1, tmp;
    p_dcr       = (volatile unsigned int *)DCR;

    /* Intialize DRAM parameters */
    width = MEMCTL_DRAM_MIN_WIDTH;
    row   = MEMCTL_DRAM_MIN_ROWS ;
    col   = MEMCTL_DRAM_MIN_COLS ;
    cs    = MEMCTL_DRAM_MIN_CS;

    /* Configure to the maximun bank number */
    if(memctlc_is_DDR()){
        bk = MEMCTL_DRAM_DDR_MIN_BANK_NUM;
    }else if(memctlc_is_DDR2()){
        bk = MEMCTL_DRAM_DDR2_MIN_BANK_NUM;
    }else{
        bk = MEMCTL_DRAM_DDR3_MIN_BANK_NUM;
    }   


    /* 0. Buswidth detection */
    test_addr0 = (volatile unsigned int *)(0xA0000000);
    test_v0 = 0x12345678;
    test_v1 = 0x00000000;
    if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(MEMCTL_DRAM_MAX_WIDTH, row, col, bk , cs)){
        memctlc_dram_phy_reset();
        *test_addr0 = test_v1;
        *test_addr0 = test_v0;
        //_memctl_debug_printf("test_addr(0x%08x)!= test_v0(0x%08x)\n", *test_addr0, test_v0);
        if( test_v0 !=  *test_addr0 ){
            width = MEMCTL_DRAM_MIN_WIDTH;
        }else{
            width = MEMCTL_DRAM_MAX_WIDTH;
        }
    }


    /* 1. Chip detection */
    test_addr0 = (volatile unsigned int *)(0xA0000000);
    test_v0 = 0xCACA0000;
    test_v1 = 0xACAC0000;
    if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, bk , MEMCTL_DRAM_MAX_CS)){
        while(cs < MEMCTL_DRAM_MAX_CS){
            memctlc_dram_phy_reset();
            test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col*row*bk*cs);
            *test_addr0 = 0x0;
            *test_addr1 = 0x0;
            *test_addr0 = test_v0;
            *test_addr1 = test_v1;
            if( test_v0 ==  *test_addr0 ){
                if( test_v1 ==  *test_addr1 ){
                    cs = cs << 1;
                    test_v0++;
                    test_v1++;
                    continue;
                }
            }
            break;
        }
    }

    /* 2. Bank detction */
    test_addr0 = (volatile unsigned int *)(0xA0000000);
    test_v0 = 0x33330000;
    test_v1 = 0xCCCC0000;
    if(memctlc_is_DDR()){
        max_bk = MEMCTL_DRAM_DDR_MAX_BANK_NUM;
    }else if(memctlc_is_DDR2()){
        max_bk = MEMCTL_DRAM_DDR2_MAX_BANK_NUM;
    }else{
        max_bk = MEMCTL_DRAM_DDR3_MAX_BANK_NUM;
    }

    //_memctl_debug_printf("%s,%d: width(%d), row(%d), col(%d), max_bk(%d), cs(%d)\n", __FUNCTION__, __LINE__, width, row, col, max_bk, cs);
    if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, max_bk, cs)){
        while(bk < max_bk){
            memctlc_dram_phy_reset();
            test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col*row*bk);
            _memctl_debug_printf("DCR(0x%08x):", *((volatile unsigned int *)DCR) );
            _memctl_debug_printf("BK:(%p)\n", test_addr1);
            *test_addr0 = 0x0;
            *test_addr1 = 0x0;
            *test_addr0 = test_v0;
            *test_addr1 = test_v1;
            tmp = *test_addr0;
            tmp = *test_addr1;
            //_memctl_debug_printf("test_addr0(%p):0x%x\n", test_addr0, *test_addr0);
            //_memctl_debug_printf("test_addr1(%p):0x%x\n", test_addr1, *test_addr1);
            if( test_v0 ==  *test_addr0 ){
                if( test_v1 ==  *test_addr1 ){
                    bk = bk << 1;
                    test_v0++;
                    test_v1++;
                    continue;
                }
            }
            break;
        }
    }

    /* 3. Row detction */
    test_addr0 = (volatile unsigned int *)(0xA0000000);
    test_v0 = 0xCAFE0000;
    test_v1 = 0xDEAD0000;
    if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, MEMCTL_DRAM_MAX_ROWS, col, bk, cs)){
        while(row < MEMCTL_DRAM_MAX_ROWS){
            memctlc_dram_phy_reset();
            test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col*row);
            _memctl_debug_printf("DCR(0x%08x):", *((volatile unsigned int *)DCR) );
            _memctl_debug_printf("row:(%p)\n", test_addr1);
            *test_addr0 = test_v0;
            *test_addr1 = test_v1;
            tmp = *test_addr0;
            tmp = *test_addr1;
            _memctl_debug_printf("test_addr0(%p):0x%x\n", test_addr0, *test_addr0);
            _memctl_debug_printf("test_addr1(%p):0x%x\n", test_addr1, *test_addr1);
            _memctl_debug_printf("row = %d\n", row);
            if( test_v0 ==  *test_addr0 ){
                if( test_v1 ==  *test_addr1 ){
                    row = row << 1;
                    test_v0++;
                    test_v1++;
                    continue;
                }
            }
            break;
        }
    }

    /* 4. Column detection */
    test_addr0 = (volatile unsigned int *)(0xA0000000);
    test_v0 = 0x5A5A0000;
    test_v1 = 0xA5A50000;
    if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, MEMCTL_DRAM_MAX_COLS, bk, cs)){
        while(col < MEMCTL_DRAM_MAX_COLS){
            memctlc_dram_phy_reset();
            test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col);
            *test_addr0 = 0x0;
            *test_addr1 = 0x0;
            _memctl_debug_printf("DCR(0x%08x):", *((volatile unsigned int *)DCR) );
            _memctl_debug_printf("col:(%p)\n", test_addr1);
            *test_addr0 = test_v0;
            *test_addr1 = test_v1;
            tmp = *test_addr0;
            tmp = *test_addr1;
            _memctl_debug_printf("test_addr0(%p):0x%x\n", test_addr0, *test_addr0);
            _memctl_debug_printf("test_addr1(%p):0x%x\n", test_addr1, *test_addr1);
            if( test_v0 ==  *test_addr0 ){
                if( test_v1 ==  *test_addr1 ){
                    col = col << 1;
                    test_v0++;
                    test_v1++;
                    continue;
                }
            }
            break;
        }
    }

    /* 5. Width detction */
    test_addr0 = (volatile unsigned int *)(0xA0000000);
    test_addr1 = (volatile unsigned int *)(0xA0000000);
    if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, bk, cs)){
        while(width < MEMCTL_DRAM_MAX_WIDTH){
        memctlc_dram_phy_reset();
        *test_addr0 = 0x3333CCCC;
        __asm__ __volatile__("": : :"memory");
        if( 0x3333CCCC !=  *test_addr0 ){
            width = width >> 1;
            continue;
        }
        __asm__ __volatile__("": : :"memory");
        *test_addr1 = 0x12345678;
        __asm__ __volatile__("": : :"memory");
        if( 0x12345678 !=  *test_addr1 ){
            width = width >> 1;
            continue;
        }
        break;
        }
    }



    memctlc_dram_phy_reset();
    _memctl_debug_printf("DCR(%p): 0x%08x\n", p_dcr, *p_dcr);
    if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, bk, cs)){
        _memctl_debug_printf("Width   : %d\n", width);
        _memctl_debug_printf("Row     : %d\n", row);
        _memctl_debug_printf("Column    : %d\n", col);
        _memctl_debug_printf("Bank    : %d\n", bk);
        _memctl_debug_printf("Chip    : %d\n", cs);
        _memctl_debug_printf("total size: %x\n", (unsigned int)((width/8)*row*col*bk*cs));
        goto test_pass;
    }else{
        _memctl_debug_printf("Error! memctl_dram_para_set failed, function: %s, line:%d", __FUNCTION__, __LINE__);
        goto test_fail;
    }

test_pass:
    return (unsigned int)((width/8)*row*col*bk*cs);
test_fail:
    return 0;
}
#endif /*#if 0*/

int memctlc_ZQ_calibration(unsigned int auto_cali_value);

#if 0
unsigned int auto_cali_value[] = {
                                        0x000010fC, /* ODT 75ohm    OCD 60ohm */
                                        0x000011be, /* ODT 50ohm    OCD 50ohm */
                                        0x0000107c, /* ODT 60ohm    OCD 60ohm */
                                        0x00000530, /* ODT 80ohm    OCD 80ohm */
                                        0x000010ef, /* ODT 75ohm    OCD 75ohm */
                                        0x00002c9e, /* ODT 85.5ohm  OCD 87.27ohm */
                                        0x00002828, /* ODT 92.31ohm OCD 96ohm */
                                        0x000008fC, /* ODT 150ohm   OCD 60ohm */
                                        0x000036fe, /* ODT 50ohm    OCD 48ohm */
                                        0x00002ffc, /* ODT 50ohm    OCD 60ohm */
                                        0x000023a3, /* ODT 100ohm   OCD 100ohm */
                                        0x000008a1  /* ODT 150ohm   OCD 150ohm */
                                 };

/* OCD is prepared at composer and feed into mrs directly */
unsigned int get_memory_dram_ocd_parameters(unsigned int *para_array)
{
         para_array[0] = 60;
         return 1;//OK
}

/* ODT is prepared at composer and feed into mrs directly */
unsigned int get_memory_dram_odt_parameters(unsigned int *para_array)
{
        para_array[0] = 75;
        return 1;//OK
}
#endif

//void memctlc_dram_phy_reset(void)
//{
//  volatile unsigned int *phy_ctl;
//
//  phy_ctl = (volatile unsigned int *)DACCR;
//  *phy_ctl = *phy_ctl & ((unsigned int) 0xFFFFFFEF);
//  *phy_ctl = *phy_ctl | ((unsigned int) 0x10);
//  _memctl_debug_printf("memctlc_dram_phy_reset: %p(0x%08x)\n", phy_ctl, *phy_ctl);
//
//  return;
//}

#if 0
void show_dram_config(void)
{
    unsigned int i;
    volatile unsigned int *phy_reg;
    _memctl_debug_printf_I("\n");
    _memctl_debug_printf_I("DDRKODL(0x%08x):0x%08x\n",\
                 DDRCKODL_A, REG32(DDRCKODL_A));
    _memctl_debug_printf_I("DCDR(0x%08x):0x%08x\n" "DDCR(0x%08x):0x%08x\n",\
                 DCDR, REG32(DCDR), DDCR, REG32(DDCR));
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
#endif /* #if 0 */
#if 0
int memctlc_set_dqm_delay(void)
{
    volatile unsigned int *dcdqmr;

    dcdqmr = (volatile unsigned int *)DCDQMR;
    *dcdqmr = DRAMI.static_cal_data_32;

    return 1;
}
#endif
//ccwei: patch from kevinchung

unsigned int memctl_dqm_pattern(unsigned int start_addr, unsigned int size, unsigned char data, int i)
{
	unsigned int src_len = size;
	unsigned char c_data = data;
	volatile unsigned char *src_addr;
	unsigned int result=0;

	src_addr = (volatile unsigned char *)start_addr;

	/* Write patterns */
	while(src_len){
		*src_addr++ = c_data++;
		src_len--;		
	}

	src_len = size;
	c_data = data;
	src_addr = (volatile unsigned char *)start_addr;

	/* Verify patterns */
	while(src_len){		
		if(*src_addr != c_data){
			result = 1;
			break;
		}
		if(i==0)
			*src_addr = 0x5a;
		else
			*src_addr = 0xa5;			
		src_addr+=2;
		c_data+=2;
		src_len-=2;
	}

	return result;
}


void memctlc_set_dqm_delay_patch(unsigned int mem_clk_mhz)
{
	volatile unsigned int *dacdqr;
	volatile unsigned int *dcdqmr;
	volatile unsigned int *ddrckodl;
	volatile unsigned int *dcr;				//kevinchung
	unsigned short dqmX_delay;
	unsigned int dqmX_window[2]={0,0};
	unsigned short dqmX_mid[2]={0,0};	
	unsigned int temp32;
	unsigned char clk_clkm_phase=0;
	unsigned short clk_delay=0,clkm_delay=0,clkm_org,i;
	short clkm90_delay=0;
	unsigned short R_flag=0,L_flag=31,fail_bit_count=0,dqm_done_bit=0,fail_count=0;
	//printf("DQM cali start...R_flag:%d, L_flag:%d\n",R_flag,L_flag);
	//printf("==>mem_clk_mhz=%d\n",mem_clk_mhz);
	dacdqr = (volatile unsigned int *)DACDQR;
	dcdqmr = (volatile unsigned int *)DCDQMR;
	dcr = (volatile unsigned int *)DCR;			//kevinchung
	ddrckodl = (volatile unsigned int *)SYSREG_DDRCKODL_REG;
	//clkm90_delay = DRAMI.clkm90_delay;			//kevinchung
	clkm_delay = DRAMI.clkm_delay;
//	printf("A==>clkm90_delay=%d clkm_delay=%d ddrckodl=0x%x\n",clkm90_delay,clkm_delay,*ddrckodl);
	//clkm90_delay = clkm_delay;
	if(memctlc_is_DDR2()){
		if(mem_clk_mhz==350){
			//Etron => CLK lead DQS0 377ps, CLK lead DQS1 ps @350MHz			
			clk_clkm_phase=7;		//RTL9601 => CLK lead DQS1 644ps @350MHz (condition 714ps ~357ps)		//kevinchung
		}else if(mem_clk_mhz==400){
			clk_clkm_phase=10;		//RTL9601 => CLK lead DQS1 544ps @400MHz (condition 625ps ~312ps)		//kevinchung
		}else{
			//Etron => CLK lead DQS0 330ps, CLK lead DQS1 355ps @300MHz
			clk_clkm_phase=4;		//RTL9601 => CLK lead DQS1 722ps @300MHz (condition 833ps ~416ps)		//kevinchung
		}
	}else if (memctlc_is_DDR3()){						//for DDR3 setting
		if(mem_clk_mhz==350){
			//Etron => CLK lead DQS0 188ps, CLK lead DQS1 122ps @350MHz
			clk_clkm_phase=10;		//RTL8696 => CLK lead DQS1 644ps @350MHz (condition 714ps ~357ps)		//kevinchung
		}else if(mem_clk_mhz==400){
			//Etron fail. RX DQ0 ~ 7 falling window too small.
			clk_clkm_phase=8;		
		}else{
			//Etron => CLK lead DQS0 270ps, CLK lead DQS1 177ps @300MHz
			clk_clkm_phase=8;		//RTL8696 => CLK lead DQS1 722ps @300MHz (condition 833ps ~416ps)		//kevinchung
		}
	}else{
		clk_clkm_phase=0;
	}
	clkm_org = (*ddrckodl & SYSREG_DDRCKODL_DDRCLM_TAP_MASK)
					>>SYSREG_DDRCKODL_DDRCLM_TAP_FD_S;	
	
	for(clkm_delay=clkm_org;clkm_delay<32;clkm_delay++)
	{
			//clkm90_delay = clkm_delay;
			if((clk_clkm_phase+clkm_delay) < 31)
				clk_delay=clkm_delay+clk_clkm_phase;
			else
				clk_delay=31;
			//*ddrckodl=((*ddrckodl)&0xffe0e0e0)|(clkm90_delay<<16)|(clkm_delay<<8)|clk_delay;

			_DRAM_PLL_CLK_power_switch(0);  //KevinChung,DDR PLL_CLK power down/up
			*ddrckodl=((*ddrckodl)& ~(SYSREG_DDRCKODL_DDRCLM_TAP_MASK |SYSREG_DDRCKODL_DDRCK_PHS_MASK))|
			(clkm_delay<<SYSREG_DDRCKODL_DDRCLM_TAP_FD_S)|clk_delay;			
			_DRAM_PLL_CLK_power_switch(1);  //KevinChung,DDR PLL_CLK power down/up

			/**********************************
			**	Calibrate DQM0, DQM1 windows first   **
			***********************************/			
			for(i=0;i<2;i++){
				/* Reset DQM0, DQM1 windows */
				dqmX_window[i] = 0;
				/* Find appropriate windows */
				for(dqmX_delay=0;dqmX_delay<32;dqmX_delay++){
					//printf("[%d]\n",dqmX_delay);
					*dcdqmr = (*dcdqmr & ~(DCDQMR_DQM1_PHASE_SHIFT_90_MASK << (1-i)*8)) | 
						(dqmX_delay << (DCDQMR_DQM1_PHASE_SHIFT_90_FD_S + (1-i)*8));
					//printf("*dcdqmr=0x%x\n",*dcdqmr);
					_memctl_update_phy_param();
					if(memctl_dqm_pattern((0xa0200001+i), 0x400, 0, i)==0){
						dqmX_window[i]|=(1<<dqmX_delay);
						//printf("dqmX_delay=%d, dqmX_window[%d]=0x%x\n",dqmX_delay,i,dqmX_window[i]);
					}	
				}
			//printf("clkm_delay=%d\t",clkm_delay);
			//printf("==>dqmX_window[%d]=0x%x\n",i,dqmX_window[i]);
			}
			/***************************************************************
			*	Check dqm0_window & dqm1_window max value of window. Analysis window *
			****************************************************************/			
			for(i=0;i<2;i++){
				
				L_flag = 31;
				R_flag = 0;
//printf("[%d]clkm_delay=%d\t",__LINE__,clkm_delay);
//printf("[%d]==>dqmX_window[%d]=0x%x dqmX_mid[%d]=%d\n",__LINE__,i,dqmX_window[i],i,dqmX_mid[i]);
					/* Analysis windows */
					if( dqmX_window[i] == 0xFFFFFFFF){

						/* Best case: 0xFFFFFFFF */
						dqm_done_bit=1;
						//dqmX_mid[i]=31;
						
					}else if( dqmX_window[i] == 0x0){
					#if	0		//kevinchung
						/* Worst case : 0x00000000*/
						if(clkm_delay<(clkm_org+5))
							fail_count++;
						else{
							dqm_done_bit=1;
						}
					#else
						;
					#endif		
					}else{
				
						while(((dqmX_window[i]>>L_flag)&0x1)==0){
							L_flag--;
						}
				
						while(((dqmX_window[i]>>R_flag)&0x1)==0){
							R_flag++;						
						}
				
						fail_bit_count=0;
						for(temp32=R_flag;temp32<=L_flag;temp32++){
							if(((dqmX_window[i]>>temp32)& 0x1)==0)
								fail_bit_count++;
						}
					}			
						/* Special case 1 : there are more than 1 zero slot between R_flag & L_flag */
					if(dqmX_window[i]!=0){			//kevinchung
						if(fail_bit_count>1)
							dqm_done_bit=2;
					
						/* Special case 2 : the window is too small */
						if((L_flag-R_flag)<2){
							if(clkm_delay<(clkm_org+5))
								fail_count++;
							else
								dqm_done_bit=4;
						}
				
						/* Special case 3 : R_flag is rising */
						if(R_flag>0)
							dqm_done_bit=8;
					}								//kevinchung
					
				
					if( (dqm_done_bit > 0 && clkm_delay > 4)||dqm_done_bit > 1 ){
						if(dqmX_window[0]!= 0x0 && dqmX_window[1]== 0x0){	//kevinchung
							*dcr &= ~(MEMCTL_DRAM_WIDTH_MASK);				//0xb8001004[n25..n24]==0b00, set 8bit mode		//kevinchung
							printf("8bit mode select\n");					//kevinchung
							}
						temp32=0;
						if(dqmX_mid[0]>24){
							temp32|=((10) << DCDQMR_DQM0_PHASE_SHIFT_90_FD_S);
						}	
						if(dqmX_mid[1]>24){
							temp32|=((10) << DCDQMR_DQM1_PHASE_SHIFT_90_FD_S);
						}							
						clkm_delay=(clkm_delay*3)/4;
						if((clk_clkm_phase+clkm_delay) < 31)
							clk_delay=clkm_delay+clk_clkm_phase;
						else
							clk_delay=31;
						//ccwei:test!!!!
						//clkm90_delay = clkm_delay-5;
						//clk_delay = clkm_delay+2;

						_DRAM_PLL_CLK_power_switch(0);  //KevinChung,DDR PLL_CLK power down/up
						*ddrckodl=((*ddrckodl)& ~(SYSREG_DDRCKODL_DDRCK_PHS_MASK |SYSREG_DDRCKODL_DDRCLM_TAP_MASK |
							SYSREG_DDRCKODL_DDRCLM90_TAP_MASK) )|
							(clkm90_delay << SYSREG_DDRCKODL_DDRCLM90_TAP_FD_S) |
							(clkm_delay << SYSREG_DDRCKODL_DDRCLM_TAP_FD_S)|
							(clk_delay << SYSREG_DDRCKODL_DDRCK_PHS_FD_S);
						_DRAM_PLL_CLK_power_switch(1);  //KevinChung,DDR PLL_CLK power down/up

						*dcdqmr = temp32;						
						_memctl_update_phy_param();
						return ;
					}
					dqmX_mid[i] = L_flag-R_flag;
			}

		}
}


/*ccwei: patch from czyao*/
/* Update from YUMC's verificatin on 6266, 
designer suggests turning on DRAM buffer full mask 
(data flow control) for DDR3 8/16 bit */
void memctlc_config_DataFullMask(void)
{
	volatile unsigned int *mcr;
	
    if(memctlc_is_DDR3()){
	mcr = (volatile unsigned int *)MCR;
	*mcr |= MCR_RBF_MAS; 
	}
	return;
}
/*################################*/
#if 0
void memctlc_config_DRAM_size(void)
{
    //volatile unsigned int *dcr, dummy_read;
    volatile unsigned int *dcr;

    dcr = (volatile unsigned int *)DCR;

    /* 140307,revised for DCR[31]: LX jitter tolerance. */
    *dcr = (*dcr & ~(DCR_MASK)) | (DRAMI.dcr & DCR_MASK);
#if 0 /* Remove size_auto_detection */
    if(DRAMI.size_auto_detection){
        memctlc_dram_size_detect();
        dummy_read = *dcr;
        *dcr = dummy_read;
    }else{
        *dcr = DRAMI.dcr;
    }
#endif /*#if 0*/

    /* if it's DDR3 16bit, enable DRAM buffer full mask */
    //if(memctlc_is_DDR3()){
    //    if(*dcr & 0x01000000){
    //        dcr = (volatile unsigned int *)MCR;
    //        *dcr = *dcr | 0x1e0;
    //    }else{
    //        dcr = (volatile unsigned int *)MCR;
    //        *dcr = *dcr & 0xFFFFFE1F;
    //    }
    //}
    /* Update from YUMC's verificatin on 6266, designer suggests turning on DRAM buffer full mask (data flow control) for DDR3 8/16 bit */
    if(memctlc_is_DDR3()){
        dcr = (volatile unsigned int *)MCR;
		*dcr = *dcr | MCR_RBF_MAS;
        //*dcr = *dcr | 0x1e0;
    }

    return ;
}
#endif
/*################################*/

#if 1
unsigned int _ddr2_8bit_size[] =  
	{ 0x90220000/* 32MB */, 0x90320000/* 64MB */, 0xA0320000/* 128MB */, 
	   0xA0420000/* 256MB */};
unsigned int _ddr3_8bit_size[] =  
	{ 0x0/* 32MB */, 0xA0220000/* 64MB */, 0xA0320000/* 128MB */, 
	   0xA0420000/* 256MB */};
unsigned int _ddr2_16bit_size[] = 
	{ 0x91210000/* 32MB */, 0x91220000/* 64MB */, 0xa1220000/* 128MB */, 
	   0xa1320000/* 256MB */, 0xa1420000/* 512MB */};
unsigned int _ddr3_16bit_size[] = 
	{ 0x0/* 32MB */, 0xA1120000/* 64MB */, 0xa1220000/* 128MB */, 
	   0xa1320000/* 256MB */, 0xa1420000/* 512MB */};
unsigned int dram_test_addr[] =
	{ 0xa6f01348/* 32MB*/, 0xa6f01748/* 64MB */, 0xaef01748/* 128MB */,
	   0xaff01748/* 256MB*/};
unsigned int dram_8bit_test_addr_ddr2[] =
	{ 0xA6780BA4/* 32MB*/, 0xA6F80BA4/* 64MB */, 0xAEF80BA4/* 128MB */,
	   0xAFF80BA4 /* 256MB*/};
unsigned int dram_8bit_test_addr_ddr3[] =
	{ 0x0/* 32MB*/, 0xAE780BA4/* 64MB */, 0xAEF80BA4/* 128MB */,
	   0xAFF80BA4/* 256MB*/};
unsigned int _dram_MCR_setting[4][4]=	
	{ 	{0x90220000/* 32MB_DDR2_08b */, 0x90320000/* 64MB_DDR2_08b */, 0xA0320000/* 128MB_DDR2_08b */, 0xA0420000/* 256MB_DDR2_08b */},
		{0x91210000/* 32MB_DDR2_16b */, 0x91220000/* 64MB_DDR2_16b */, 0xa1220000/* 128MB_DDR2_16b */, 0xa1320000/* 256MB_DDR2_16b */},
	 	{0x00000000/* 32MB_DDR3_08b */, 0xA0220000/* 64MB_DDR3_08b */, 0xA0320000/* 128MB_DDR3_08b */, 0xA0420000/* 256MB_DDR3_08b */},
		{0x00000000/* 32MB_DDR3_16b */, 0xA1120000/* 64MB_DDR3_16b */, 0xa1220000/* 128MB_DDR3_16b */, 0xa1320000/* 256MB_DDR3_16b */}		
	};
unsigned int _dram_type_setting[4][4]=		//format: n15: 1=16bit, n8..n4=tRFC, n2..n0=DRAM_type, n31..n16=DRAM_size
	{ 	{0x020004B2/* 32MB_DDR2_08b */, 0x04000692/* 64MB_DDR2_08b */, 0x08000802/* 128MB_DDR2_08b */, 0x10000C62/* 256MB_DDR2_08b */},
		{0x020084B2/* 32MB_DDR2_16b */, 0x04008692/* 64MB_DDR2_16b */, 0x08008802/* 128MB_DDR2_16b */, 0x10008C62/* 256MB_DDR2_16b */},
		{0x020005A3/* 32MB_DDR3_08b */, 0x040005A3/* 64MB_DDR3_08b */, 0x080006E3/* 128MB_DDR3_08b */, 0x10000A03/* 256MB_DDR3_08b */},
		{0x020085A3/* 32MB_DDR3_16b */, 0x040085A3/* 64MB_DDR3_16b */, 0x080086E3/* 128MB_DDR3_16b */, 0x10008A03/* 256MB_DDR3_16b */}
	};
unsigned int _dram_detection_addr[4][4]=	
	{ 	{0xA6780BA4/* 32MB_DDR2_08b */, 0xA6F80BA4/* 64MB_DDR2_08b */, 0xAEF80BA4/* 128MB_DDR2_08b */, 0xAFF80BA4/* 256MB_DDR2_08b */},
		{0xA6F01348/* 32MB_DDR2_16b */, 0xA6F01548/* 64MB_DDR2_16b */, 0xAEF01548/* 128MB_DDR2_16b */, 0xAFF01548/* 256MB_DDR2_16b */},
		{0xA0000000/* 32MB_DDR3_08b */, 0xAE780BA4/* 64MB_DDR3_08b */, 0xAEF80BA4/* 128MB_DDR3_08b */, 0xAFF80BA4/* 256MB_DDR3_08b */},
		{0xA0000000/* 32MB_DDR3_16b */, 0xAE701548/* 64MB_DDR3_16b */, 0xAEF01548/* 128MB_DDR3_16b */, 0xAFF01548/* 256MB_DDR3_16b */}
	};
/*end*/
unsigned int memctlc_config_DRAM_size(void)
{
    //volatile unsigned int *dcr, dummy_read;
    volatile unsigned int *dcr,*mcr;
	unsigned int *size_arry;
	unsigned int dcr_value=0, dram_size=0x2000000;
	volatile unsigned int *dram_addr;
	unsigned int i,DDR3_flag,DDR_width,loc;
	unsigned int detected_size=0;
	mcr = (volatile unsigned int *)MCR;	
    dcr = (volatile unsigned int *)DCR;
#if	1			//kevinchung
	if((((*mcr)>>28)&0x3)==0x1)		//DDR2
		DDR3_flag=0;
	else							//DDR3
		DDR3_flag=2;
	if((((*dcr)>>24)&0x3)==0x1)		//DDR bus width = 16
		DDR_width=16;
	else							//DDR bus width = 8
		DDR_width=8;
	loc=(DDR_width/8-1) + DDR3_flag;
	size_arry =  &_dram_MCR_setting[loc][0];
	*dcr = size_arry[3];
	printf("%s-%d *dcr=0x%x\n",__func__,__LINE__,*dcr);
	_memctl_update_phy_param();
	dram_addr = (volatile unsigned int *)_dram_detection_addr[loc][3];
	printf("%s-%d *dram_addr=0x%x\n",__func__,__LINE__,*dram_addr);
	*dram_addr = 0x5A0FF0A5;
	printf("%s-%d *dram_addr=0x%x\n",__func__,__LINE__,*dram_addr);
	udelay(5);
	for(i=0;i<4;i++){
		if( REG32(_dram_detection_addr[loc][i]) == 0x5A0FF0A5 ){
			dcr_value = _dram_MCR_setting[loc][i];
			dram_size = _dram_type_setting[loc][i];
			detected_size=1;
			break;
			}	
		}
	if(!detected_size){
		dcr_value = _dram_MCR_setting[loc][1];
		dram_size = _dram_type_setting[loc][1];
	}
	*dcr = dcr_value;
	printf("%s-%d *dcr=0x%x\n",__func__,__LINE__,*dcr);
	_memctl_update_phy_param();	
#else
	size_arry =  &_ddr2_16bit_size[0];
	/* Config DRAM as 1GBytes, 16bit width, bank=8, row=64K, column=1K*/

	/* 140313,revised for DCR[31]: LX jitter tolerance. */
	*dcr = (*dcr & ~(DCR_MASK)) | (size_arry[4] & DCR_MASK);
	//printf("%s-%d *dcr=0x%x\n",__func__,__LINE__,*dcr);
	_memctl_update_phy_param();
	
	dram_addr = (volatile unsigned int *)dram_test_addr[3];
	//printf("%s-%d *dram_addr=0x%x\n",__func__,__LINE__,*dram_addr);
	*dram_addr = 0x5A0FF0A5;
	//printf("%s-%d *dram_addr=0x%x\n",__func__,__LINE__,*dram_addr);

	for(i=0; i<(sizeof(dram_test_addr)/sizeof(unsigned int)); i++){
		//printf("%s-%d REG32(dram_test_addr[%d]=0x%x\n",__func__,__LINE__,i,REG32(dram_test_addr[i]));

		if( REG32(dram_test_addr[i]) == 0x5A0FF0A5 ){

			//dcr_value = _ddr2_16bit_size[i];		//kevinchung
			dcr_value = size_arry[i];			//kevinchung
			dram_size = (0x2000000) << i;
			detected_size=1;
			break;
		}
	}
	if(!detected_size)
		dcr_value = size_arry[0];
	//printf("AAA----------->dram_size=0x%x\n",dram_size);
	/* 140313,revised for DCR[31]: LX jitter tolerance. */
	*dcr = (*dcr & ~(DCR_MASK)) | (dcr_value & DCR_MASK);
	//printf("%s-%d *dcr=0x%x\n",__func__,__LINE__,*dcr);
	
	_memctl_update_phy_param();
#endif
    return dram_size;
}
#endif

init_result_t memctlc_ZQ_config(void)
{
    volatile unsigned int *dmcr;
    init_result_t ret;

    dmcr = (volatile unsigned int *)DMCR;
    ret = INI_RES_OK;

    /* Disable DRAM refresh operation */
    *dmcr = ((*dmcr | DMCR_DIS_DRAM_REF_MASK) & (~DMCR_MR_MODE_EN_MASK));

    if(DRAMI.zq_setting == 0xDEADDEAD) { /* Use magic number 0xDEADDEAD to patch ZQ HW bug */
        printf("Info: Applying ZQ result directly by plat_memctl_ZQ_force_config().\n");
        plat_memctl_ZQ_force_config();
    } else {
        /* ZQ calibration */
        if(MEMCTL_ZQ_CALI_PASS != memctlc_ZQ_calibration(DRAMI.zq_setting)) {
            ret = INI_RES_FAIL;
            printf("Error: ZQ calibration failed.\n");
        }
    }

    /* DDR3 SDRAM ZQ long calibration. */
    if(memctlc_is_DDR3()){
        memctlc_DDR3_ZQ_long_calibration();
    }

    /* Enable DRAM refresh operation */
    *dmcr = *dmcr &  (~DMCR_DIS_DRAM_REF_MASK) ;
    return ret;
}

void memctlc_DDR3_ZQ_long_calibration(void)
{
	volatile unsigned int *ddr3_zqccr;
	ddr3_zqccr = (volatile unsigned int *)D3ZQCCR;

	*ddr3_zqccr |= (1<<31);
	_memctl_delay_clkm_cycles(100);
	while(*ddr3_zqccr & (1<<31));

	return;

}
int memctlc_ZQ_calibration(unsigned int auto_cali_value)
{
    volatile unsigned int *daccr, *zq_cali_reg, *dpcr;
    volatile unsigned int *zq_cali_status_reg;
    volatile unsigned int *socpnr;
    unsigned int polling_limit;
    unsigned int reg_v, odtn, odtp, ocdn, ocdp;

    socpnr  = (volatile unsigned int *)SOCPNR;
    daccr   = (volatile unsigned int *)DACCR;
    dpcr    = (volatile unsigned int *)DPCR;
    zq_cali_reg         = (volatile unsigned int *)DDZQPCR;
    zq_cali_status_reg  = (volatile unsigned int *)DDZQPSR;

    /* Enable ZQ calibration model */
    plat_memctl_ZQ_model_en();

    if(*socpnr == MEMCTL_SOCPNR_0399){
        *zq_cali_reg = auto_cali_value | 0x40000000;
    }else{
        /* Trigger the calibration */
        *zq_cali_reg = auto_cali_value | 0x80000000;
    }

    /* Polling to ready */
    /* 0x10000 iterations take roughly 327K cycles. It is 327us when OCP runs on 1GHz. */
    polling_limit = 0x10000;
    while(*zq_cali_reg & 0x80000000){
        polling_limit--;
        if(polling_limit == 0){
#if (ZQ_TIMEOUT_RESET == 1)
	        printf("Warning: ZQ calibration timeout! Reset for try again.\n");
	        SYSTEM_RESET();
#else
	        printf("Warning: ZQ calibration timeout! Using default values instead.\n");
	        plat_memctl_ZQ_force_config();
	        goto static_zq_setting_done;
#endif
        }
    }

    /* Patch code for IO PAD */
    //plat_memctl_IO_PAD_patch();

    reg_v = *zq_cali_status_reg;
    if(reg_v & 0x20000000) {  
        odtp = ((reg_v >> 27) & 0x3);
        odtn = ((reg_v >> 25) & 0x3);
        ocdp = ((reg_v >> 23) & 0x3);
        ocdn = ((reg_v >> 21) & 0x3);
        printf("Result of ODTP/ODTN/OCDP/OCDN=%d/%d/%d/%d\n", odtp, odtn, ocdp, ocdn);  
        if((odtp != 0) || /* ODTP must be completed with no error */
           (odtn == 2) || /* ODTN must NOT be underflow (may tolerate code overflow error) */
           (ocdp == 1) || /* OCDP must NOT be overflow (may tolerate code underflow error) */
           (ocdn == 1))   /* OCDN must NOT be overflow (may tolerate code underflow error) */
        {
            printf("EE: ZQ calibration failed\n");
            return MEMCTL_ZQ_CALI_FAIL; /* Error, calibration fail. */
        }
    }

#if (ZQ_TIMEOUT_RESET == 0)
 static_zq_setting_done:
#endif
    return MEMCTL_ZQ_CALI_PASS; /* Pass, calibration pass.*/
}

//ccwei:patch from czyao
unsigned int tRFC_Spec_DDR2[] = {
			/* 256Mbit, 512Mbit,1Gbit, 2Gbit, 4Gbit	*/
			75,105,128,198,328
			};
unsigned int tRFC_Spec_DDR3[] = {
			/*  512Mbit,1Gbit, 2Gbit, 4Gbit, 8Gbit	*/
			90, 110, 160, 260, 350
			};
//end
void memctlc_config_DTR(unsigned int default_instruction, unsigned int dram_size)
{
	volatile unsigned int *dtr0, *dtr1, *dtr2;
	//ccwei:patch from czyao
	unsigned int dram_freq_mhz,dram_size_t;
	unsigned int dram_base_size=0, dtr2_temp = 0;
	unsigned int DRAM_capacity_index=0;
	unsigned int *tRFC;
	//end
	dtr0 = (volatile unsigned int *)DTR0_A;
	dtr1 = (volatile unsigned int *)DTR1_A;
	dtr2 = (volatile unsigned int *)DTR2_A;	
    printf("Setting DTR\n");
    //printf("DTR0:0x%x, DTR1:0x%x, DTR2:0x%x\n",DRAMI.dtr0,DRAMI.dtr1,DRAMI.dtr2);
	//ccwei: patch from kevinchung
	
	dram_freq_mhz = plat_memctl_MEM_clock_MHz();
	//printf("dram_freq_mhz=%d\n",dram_freq_mhz);
#if 1	
	if(default_instruction == 1){
		if(memctlc_is_DDR2())
			DRAM_capacity_index = 4;
		else if (memctlc_is_DDR3())
			DRAM_capacity_index = 4;
		else
			DRAM_capacity_index = 0;
	}else{
		/* Provide the minmun dram size as base */
		if(memctlc_is_DDR2())
			dram_base_size = 0x2000000;
		else if (memctlc_is_DDR3())
			dram_base_size = 0x4000000;
		else
			dram_base_size = 0x2000000;
		dram_size_t=dram_size&0xFFFF0000;
printf("%s-%d dram_size=0x%x\n",__func__,__LINE__,dram_size_t);		
		for(DRAM_capacity_index=0; DRAM_capacity_index<6; DRAM_capacity_index++){
			if(dram_size_t == ((dram_base_size) << DRAM_capacity_index)){
				break;
			}
		}		
	}
#endif	
    if(memctlc_is_DDR2()){
        _memctl_debug_printf("DDR2\n");
		//DRAM_capacity_index = 2;
		tRFC = &tRFC_Spec_DDR2[0];
		switch(dram_freq_mhz){
		case 400:
			*dtr0 = 0x56433630;
			*dtr1 = 0x05050313;
			dtr2_temp = 0x00012000;		
			break;
		case 350:	
			/* for 1Gbit */
			*dtr0 = 0x55422530;
			*dtr1 = 0x05050311;
			dtr2_temp = 0x00010000; 					
			break;
		default:
			*dtr0 = 0x54422820;
			*dtr1 = 0x0404030f;
			dtr2_temp = 0x0000E000; 		
		}		
    }else if(memctlc_is_DDR3()){
        _memctl_debug_printf("DDR3\n");
		//DRAM_capacity_index = 1;
		tRFC = &tRFC_Spec_DDR3[0];	
		switch(dram_freq_mhz){
		case 400:
			*dtr0 = 0x56444B20;
			*dtr1 = 0x07070516;
			dtr2_temp = 0x00011000;								
			break;
		case 350:	
			/* for 1Gbit */
			*dtr0 = 0x55433920;
			*dtr1 = 0x05050412;
			dtr2_temp = 0x0000F000;		
			break;
		default:
			*dtr0 = 0x54433820;
			*dtr1 = 0x0404030F;
			dtr2_temp = 0x0000D000;							
		}		
    }else{
		DRAM_capacity_index = 0;
		/* Set as default value */
		*dtr0 = 0x54422820;
		*dtr1 = 0x0404030F;
		dtr2_temp = 0x0000D000;				
        _memctl_debug_printf("DDR1\n");
    }
#if 0	
	printf("tRFC[%d]=%d\n",DRAM_capacity_index,tRFC[DRAM_capacity_index]);
	printf("DTR2_RFC_FD_S=%d\n",DTR2_RFC_FD_S);
	printf("dram_freq_mhz=%d\n",dram_freq_mhz);
	printf("====>%d %d\n",(((tRFC[DRAM_capacity_index]*dram_freq_mhz)/1000)+3),(tRFC[DRAM_capacity_index]*dram_freq_mhz));
	printf("0x%x\n",((((tRFC[DRAM_capacity_index]*dram_freq_mhz)/1000)+3)<<DTR2_RFC_FD_S));
#endif
	*dtr2=dtr2_temp | 
		((((tRFC[DRAM_capacity_index]*dram_freq_mhz)/1000)+3)<<DTR2_RFC_FD_S);
    printf("Setting DTR dtr0=0x%x dtr1=0x%x dtr2=0x%x\n",*dtr0,*dtr1,*dtr2);
	
	#if 0
    REG32(DTR0_A)    = DRAMI.dtr0;
    REG32(DTR1_A)    = DRAMI.dtr1;
    REG32(DTR2_A)    = DRAMI.dtr2;
	#endif
    return;
}


void memctlc_DRAM_DLL_reset(void)
{
#ifdef MEMCTL_DDR2_SUPPORT
    if(memctlc_is_DDR2()){
        _memctl_debug_printf("DDR2\n");
        memctlc_ddr2_dll_reset();
        return;
    }
#endif
#ifdef MEMCTL_DDR3_SUPPORT
    if(memctlc_is_DDR3()){
        _memctl_debug_printf("DDR3\n");
        memctlc_ddr3_dll_reset();
        return;
    }
#endif
#ifdef MEMCTL_DDR1_SUPPORT
    if(memctlc_is_DDR()){
        _memctl_debug_printf("DDR1\n");
        memctlc_ddr1_dll_reset();
        return;
    }
#endif
    return;
}


void memctlc_dram_phy_reset(void)
{
    REG32(DACCR_A) = REG32(DACCR_A) & ((u32_t) 0xFFFFFFEF);
    REG32(DACCR_A) = REG32(DACCR_A) | ((u32_t) 0x10);
    //_memctl_debug_printf("memctlc_dram_phy_reset: %p(0x%08x)\n", phy_ctl, *phy_ctl);
    
    return;
}


#if 0
#define MRS_DELAY_TIME 0x800
static void DDR1_setup_MRS(u32_t mr, u32_t emr) {
    u32_t delay_time;
    
    /* 1. Disable DLL */
    REG32(DMCR_A) = emr | DDR1_EMR1_DLL_DIS;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 2. Enable DLL */
    REG32(DMCR_A) = emr & (~DDR1_EMR1_DLL_DIS);
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 3. Reset DLL */
    REG32(DMCR_A) = mr | DDR1_MR_OP_RST_DLL ;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 4. Waiting 200 clock cycles */
    delay_time = MRS_DELAY_TIME;
    while(delay_time--);
    
    /* 5. Normal mode, avoid to reset DLL when updating phy params */
    REG32(DMCR_A) = mr;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 6. reset phy fifo */
    memctlc_dram_phy_reset();
}    


static void DDR2_setup_MRS(u32_t mr, u32_t emr1, u32_t emr2, u32_t emr3) {
    u32_t delay_time;

    /* 1. Disable DLL */
    REG32(DMCR_A) = emr1 | DDR2_EMR1_DLL_DIS;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 2. Enable DLL */
    REG32(DMCR_A) = emr1 & (~DDR2_EMR1_DLL_DIS);
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 3. Reset DLL */
    REG32(DMCR_A) = mr | DDR2_MR_DLL_RESET_YES ;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 4. Waiting 200 clock cycles */
    delay_time = MRS_DELAY_TIME;
    while(delay_time--);
    
    /* 5. Set EMR2 */
    REG32(DMCR_A) = emr2;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);

    REG32(DMCR_A) = emr3;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 6. reset phy fifo */
    memctlc_dram_phy_reset();
    
    return;

}

static void DDR3_setup_MRS(u32_t mr0, u32_t mr1, u32_t mr2, u32_t mr3) {
    u32_t delay_time;
    
    /* 1. Disable DLL */
    REG32(DMCR_A) = mr1 | DDR3_EMR1_DLL_DIS;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 2. Enable DLL */
    REG32(DMCR_A) = mr1 & (~DDR3_EMR1_DLL_DIS);
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 3. Reset DLL */
    REG32(DMCR_A) = mr0 | DDR3_MR_DLL_RESET_YES ;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 4. Waiting 200 clock cycles */
    delay_time = MRS_DELAY_TIME;
    while(delay_time--);
    
    /* 5. Set EMR2 */
    REG32(DMCR_A) = mr2;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 6. Set EMR3 */
    REG32(DMCR_A) = mr3;
    while(REG32(DMCR_A) & DMCR_MRS_BUSY);
    
    /* 7. reset phy fifo */
    memctlc_dram_phy_reset();
    
    return;
}
#endif 

/* Return DRAM size in byte. We need not this for now. */
#if 0
__attribute__((__unused__))
static u32_t get_dram_size(void) {
    const u32_t BUSWID_val[] = {1, 2, 4, 0};
    const u32_t ROWCNT_val[] = {2048, 4096, 8192, 16384};
    const u32_t COLCNT_val[] = {256, 512, 1024, 2048, 4096, 0, 0, 0};
    const u32_t BANKCNT_val[] = {4, 8};

    const u32_t DCR = DCR_REG;
    const u32_t EDTCR = EDTCR_REG;

    const u32_t BUSWID_BYTE = BUSWID_val[(DCR >> 28) & 0x3];
    const u32_t ROWCNT = ROWCNT_val[(DCR >> 25) & 0x3];
    const u32_t COLCNT = COLCNT_val[(DCR >> 22) & 0x7];
    const u32_t BANKCNT = BANKCNT_val[(EDTCR >> 30) & 0x3];

    return (BUSWID_BYTE*ROWCNT*COLCNT*BANKCNT);
}
#endif //#if 0

//ccwei: patch from kevinchung
//extern u32_t DDR_Calibration(void);
extern u32_t DDR_Calibration(unsigned char full_scan);
static u32_t dram_software_calibration(void)
{
    u32_t ret;
    
	unsigned int mem_clk_mhz;
    //printf("Start claibation test...");//_memctl_debug_printf_I("Start claibation test\n");

    plat_memctl_input_sig_delay(DRAMI.clkm_delay, DRAMI.clkm90_delay, DRAMI.tx_clk_phs_delay);
    
    /* Reset DRAM DLL */
    memctlc_DRAM_DLL_reset();

    //memctls_init();
    ret = DDR_Calibration(0);
    if(ret != 0) {
        return ret;
    }

	mem_clk_mhz = plat_memctl_MEM_clock_MHz();
    //memctlc_set_dqm_delay();
    memctlc_set_dqm_delay_patch(mem_clk_mhz);
    ret = DDR_Calibration(1);
    if(ret != 0) {
        return ret;
    }

    return ret;
}

#ifdef DRAM_PARAM_DEBUG
static void dram_display_param_info(void) {
    printf("\n");
    printf("mcr               =0x%08x\n", DRAMI.mcr);
    printf("dcr               =0x%08x\n", DRAMI.dcr);
    printf("mpmr0             =0x%08x\n", DRAMI.mpmr0);
    printf("mpmr1             =0x%08x\n", DRAMI.mpmr1);
    printf("dider             =0x%08x\n", DRAMI.dider);
    printf("d23oscr           =0x%08x\n", DRAMI.d23oscr);
    printf("daccr             =0x%08x\n", DRAMI.daccr);
    printf("dacspcr           =0x%08x\n", DRAMI.dacspcr);
    printf("dacspar           =0x%08x\n", DRAMI.dacspar);
    if(DRAMI.calibration_type == 0) { /* Static calibration */
        printf("static_cal_data_0 =0x%08x\n", DRAMI.static_cal_data_0 );
        printf("static_cal_data_1 =0x%08x\n", DRAMI.static_cal_data_1 );
        printf("static_cal_data_2 =0x%08x\n", DRAMI.static_cal_data_2 );
        printf("static_cal_data_3 =0x%08x\n", DRAMI.static_cal_data_3 );
        printf("static_cal_data_4 =0x%08x\n", DRAMI.static_cal_data_4 );
        printf("static_cal_data_5 =0x%08x\n", DRAMI.static_cal_data_5 );
        printf("static_cal_data_6 =0x%08x\n", DRAMI.static_cal_data_6 );
        printf("static_cal_data_7 =0x%08x\n", DRAMI.static_cal_data_7 );
        printf("static_cal_data_8 =0x%08x\n", DRAMI.static_cal_data_8 );
        printf("static_cal_data_9 =0x%08x\n", DRAMI.static_cal_data_9 );
        printf("static_cal_data_10=0x%08x\n", DRAMI.static_cal_data_10);
        printf("static_cal_data_11=0x%08x\n", DRAMI.static_cal_data_11);
        printf("static_cal_data_12=0x%08x\n", DRAMI.static_cal_data_12);
        printf("static_cal_data_13=0x%08x\n", DRAMI.static_cal_data_13);
        printf("static_cal_data_14=0x%08x\n", DRAMI.static_cal_data_14);
        printf("static_cal_data_15=0x%08x\n", DRAMI.static_cal_data_15);
        printf("static_cal_data_16=0x%08x\n", DRAMI.static_cal_data_16);
        printf("static_cal_data_17=0x%08x\n", DRAMI.static_cal_data_17);
        printf("static_cal_data_18=0x%08x\n", DRAMI.static_cal_data_18);
        printf("static_cal_data_19=0x%08x\n", DRAMI.static_cal_data_19);
        printf("static_cal_data_20=0x%08x\n", DRAMI.static_cal_data_20);
        printf("static_cal_data_21=0x%08x\n", DRAMI.static_cal_data_21);
        printf("static_cal_data_22=0x%08x\n", DRAMI.static_cal_data_22);
        printf("static_cal_data_23=0x%08x\n", DRAMI.static_cal_data_23);
        printf("static_cal_data_24=0x%08x\n", DRAMI.static_cal_data_24);
        printf("static_cal_data_25=0x%08x\n", DRAMI.static_cal_data_25);
        printf("static_cal_data_26=0x%08x\n", DRAMI.static_cal_data_26);
        printf("static_cal_data_27=0x%08x\n", DRAMI.static_cal_data_27);
        printf("static_cal_data_28=0x%08x\n", DRAMI.static_cal_data_28);
        printf("static_cal_data_29=0x%08x\n", DRAMI.static_cal_data_29);
        printf("static_cal_data_30=0x%08x\n", DRAMI.static_cal_data_30);
        printf("static_cal_data_31=0x%08x\n", DRAMI.static_cal_data_31);
        printf("static_cal_data_32=0x%08x\n", DRAMI.static_cal_data_32);
    }

    printf("zq_setting        =0x%08x\n", DRAMI.zq_setting);
    //printf("size_auto_detection =0x%02x\n", DRAMI.size_auto_detection  );
    printf("calibration_type    =0x%02x\n", DRAMI.calibration_type     );
    printf("tx_clk_phs_delay    =0x%02x\n", DRAMI.tx_clk_phs_delay     );
    printf("clkm_delay          =0x%02x\n", DRAMI.clkm_delay           );
    printf("clkm90_delay        =0x%02x\n", DRAMI.clkm90_delay         );
    printf("auto_calibration    =0x%02x\n", DRAMI.auto_calibration     );
    //printf("drv_strength        =0x%02x\n", DRAMI.drv_strength         );

    printf("dtr0                =0x%08x\n", DRAMI.dtr0);
    printf("dtr1                =0x%08x\n", DRAMI.dtr1);
    printf("dtr2                =0x%08x\n", DRAMI.dtr2);

    switch(REG32(MCR_A) & MCR_DRAMTYPE_MASK) {
    case  MCR_DRAMTYPE_DDR:
        printf("DDR1_mr  =0x%08x\n", DRAMI.DDR1_mr );
        printf("DDR1_emr =0x%08x\n", DRAMI.DDR1_emr);
        break;

    case  MCR_DRAMTYPE_DDR2:
        printf("DDR2_mr  =0x%08x\n", DRAMI.DDR2_mr );
        printf("DDR2_emr1=0x%08x\n", DRAMI.DDR2_emr1);
        printf("DDR2_emr2=0x%08x\n", DRAMI.DDR2_emr2);
        printf("DDR2_emr3=0x%08x\n", DRAMI.DDR2_emr3);
        break;

    case  MCR_DRAMTYPE_DDR3:
        printf("DDR3_mr0 =0x%08x\n", DRAMI.DDR3_mr0);
        printf("DDR3_mr1 =0x%08x\n", DRAMI.DDR3_mr1);
        printf("DDR3_mr2 =0x%08x\n", DRAMI.DDR3_mr2);
        printf("DDR3_mr3 =0x%08x\n", DRAMI.DDR3_mr3);
        break;

    default:
        printf("Error: Unknown DRAM TYPE strap pin setting.\n");
        break;      
    }
}
#endif //#ifdef DRAM_PARAM_DEBUG
    

static void dram_static_calibration(void) {
    u32_t i;
    const u32_t *src;
    u32_t *dst;
        
    /* Set clkm/m90/TX delay tap */
    plat_memctl_input_sig_delay(DRAMI.clkm_delay, DRAMI.clkm90_delay, DRAMI.tx_clk_phs_delay);

    src = &(DRAMI.static_cal_data_0);
    dst = (u32_t *)STATIC_CAL_DATA_BASE;
    for(i=0; i<STATIC_CAL_DATA_LEN; i++) {
        *dst++ = *src++;
    }

    memctlc_DRAM_DLL_reset();
#if 0
    /* Apply MRS */
    switch(REG32(MCR_A) & MCR_DRAMTYPE_MASK) {
    case  MCR_DRAMTYPE_DDR:
        printf("\nSetting MRS for DDR1\n");
        DDR1_setup_MRS(DRAMI.DDR1_mr, DRAMI.DDR1_emr);
        break;
    case  MCR_DRAMTYPE_DDR2:
        printf("\nSetting MRS for DDR2\n");
        DDR2_setup_MRS(DRAMI.DDR2_mr, DRAMI.DDR2_emr1, DRAMI.DDR2_emr2, DRAMI.DDR2_emr3);
        break;
    case  MCR_DRAMTYPE_DDR3:
        printf("\nSetting MRS for DDR3\n");
        DDR3_setup_MRS(DRAMI.DDR3_mr0, DRAMI.DDR3_mr1, DRAMI.DDR3_mr2, DRAMI.DDR3_mr3);
        break;
    default:
        printf("Error: Unknown DRAM TYPE strap pin setting.\n");
        break;      
    }
#endif /* #if 0 */    
}

void memctlc_DBFM_enable(void)
{
    volatile unsigned int *mcr;

    mcr = (volatile unsigned int *)MCR;
	*mcr = *mcr | MCR_RBF_MAS;
    //*mcr = *mcr |0x1e0;

    return;
}

void dram_setup(void) {
    u32_t reg_val;
    volatile unsigned int delay_loop;
	volatile unsigned int *ddrkodl_reg;
	unsigned int dram_size=0;
#ifdef DRAM_PARAM_DEBUG
    dram_display_param_info();
#endif //#ifdef DRAM_PARAM_DEBUG
    /* MCR is mixed with FLASH setting, using read-modify-write to update it */
    reg_val = REG32(MCR_A);
    reg_val &= ~(MCR_IPREF_MASK | MCR_DPREF_MASK);
    reg_val |= DRAMI.mcr;
    REG32(MCR_A) = reg_val;
    /* Has not been fully verified (suggested by YUMC) */
    //REG32(MPMR0_A)   = DRAMI.mpmr0;
    //REG32(MPMR1_A)   = DRAMI.mpmr1;
    REG32(DIDER_A)   = DRAMI.dider;
    //REG32(D23OSCR_A) = DRAMI.d23oscr;
    REG32(DACCR_A)   = DRAMI.daccr;
    REG32(DACSPCR_A) = DRAMI.dacspcr;
    REG32(DACSPAR_A) = DRAMI.dacspar;

    plat_mem_clk_rev_check();

    /* Enable DRAM clock */
    plat_memctl_dramclk_en();

    //memctlc_config_DRAM_size();
	memctlc_config_DataFullMask();

    /* Configure DRAM timing parameters */
    memctlc_config_DTR(1,dram_size);

    if(memctlc_ZQ_config() != INI_RES_OK) {
        parameters.dram_init_result = INI_RES_DRAM_ZQ_CALI_FAIL;
        goto done;
    }

    /* Set data buffer full mask, enabled when memclk > 200MHz */
    if(plat_memctl_MEM_clock_MHz() > 200) {
        memctlc_DBFM_enable();    
    }
	//ccwei: patch from czyao for dram auto parameters
    ddrkodl_reg = (volatile unsigned int *)SYSREG_DDRCKODL_REG;

    _DRAM_PLL_CLK_power_switch(0);  //KevinChung,DDR PLL_CLK power down/up
    *ddrkodl_reg = 0x00000208;
    _DRAM_PLL_CLK_power_switch(1);  //KevinChung,DDR PLL_CLK power down/up

    if(DRAMI.calibration_type == 0) { /* Static calibration */
        printf("II: DRAM is set by static calibration... ");
        dram_static_calibration();
    } else {/* software calibration */
        printf("II: DRAM is set by TAK software calibration...\n");
        if(dram_software_calibration() == 0) {
            printf("PASSED\n");
        } else {            
            printf("FAILED\n");
            parameters.dram_init_result = INI_RES_DRAM_SW_CALI_FAIL; //Add new enum for calibration failure
            goto done;
        }
    }

    memctlc_dram_phy_reset();

   	dram_size = memctlc_config_DRAM_size();
	//printf("%s-%d dram_size=0x%x\n",__func__,__LINE__,dram_size);
	//ccwei: patch form czyao
	memctlc_config_DTR(0,dram_size);

    delay_loop = 0x80000;
    while(delay_loop--);

    /* A simple test */
#if (SELFTEST == 1)
    printf("DD: Simple DRAM test... ");
    *((volatile u32_t *)0x80000000) = 0x5AFE5AFE;
    if (*((volatile u32_t *)0x80000000) != 0x5AFE5AFE) {
        parameters.dram_init_result = INI_RES_TEST_FAIL;
        goto done;
    }

    *((volatile u32_t *)0xA0000000) = 0x05D650C0;
    if (*((volatile u32_t *)0xA0000000) != 0x05D650C0) {
        parameters.dram_init_result = INI_RES_TEST_FAIL;
        goto done;
    } else {
        /* This could be kinda weird, but coming this far means both $ and un$ tests are passed. */
        printf("OK\n");
        parameters.dram_init_result = INI_RES_OK;
    }
#endif

    parameters.dram_init_result = INI_RES_OK;
    
done:
    plat_memctl_show_dram_config();
    return;
}

static void _memctl_delay_clkm_cycles(unsigned int delay_cycles)
{
    volatile unsigned int *mcr, read_tmp;

    mcr = (unsigned int *)MCR;

    while(delay_cycles--){
            read_tmp = *mcr;
    }

    return;
}

void _memctl_update_phy_param(void)
{
    volatile unsigned int *dmcr;
    volatile unsigned int *dcr;
    volatile unsigned int *dacr;
    volatile unsigned int dacr_tmp1, dacr_tmp2;
    volatile unsigned int dmcr_tmp;
    volatile unsigned int loop_lim = 0;
 
    dmcr = (unsigned int *)DMCR;
    dcr = (unsigned int *)DCR;
    dacr = (unsigned int *)DACCR;
 
    /* Write DMCR register to sync the parameters to phy control. */
    dmcr_tmp = *dmcr;
    *dmcr = dmcr_tmp;
    _memctl_delay_clkm_cycles(10);

    /* Waiting for the completion of the update procedure. */
    while((*dmcr & ((unsigned int)DMCR_MRS_BUSY)) != 0) {
	    if (loop_lim++ > 100) {
		    pblr_puts("EE: DMCR does not respond. Reset...\n");
		    SYSTEM_RESET();
	    }
    }

    __asm__ __volatile__("": : :"memory");
 
    /* reset phy buffer pointer */
    dacr_tmp1 = *dacr;
    dacr_tmp1 = dacr_tmp1 & (0xFFFFFFEF);
    dacr_tmp2 = dacr_tmp1 | (0x10);
    *dacr = dacr_tmp1 ;
 
    _memctl_delay_clkm_cycles(10);
    __asm__ __volatile__("": : :"memory");
    *dacr = dacr_tmp2 ;
 
    return;
}

unsigned int get_memory_memctl_dq_write_delay_parameters(unsigned int *para_array) 
{
    unsigned int ret;
    unsigned int reg_val;

    if( (*para_array) < 32) { 
        reg_val = *((&(DRAMI.static_cal_data_0)) + *para_array);
        *para_array = ( (reg_val & DACDQR_DQR_PHASE_SHIFT_90_MASK) >> DACDQR_DQR_PHASE_SHIFT_90_FD_S);
        ret = 1;
    } else {
        ret = 0;
    }

    return ret;
}

