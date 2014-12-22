#include <preloader.h>
#include <cpu_utils.h>
#include <pblr.h>
#include <plr_pll_gen1.h>
#include "swp_util.h"
#include "swp_cli_util.h"
#include "swp_ddr_gen2_util.h"
#include "swp_pll_gen1_util.h"
#include "swp_cpu_util.h"
#include "swp_access_soc_type.h"

#if defined(CMU_DIVISOR)
void pll_setup(void);
#endif

// function types
typedef u32_t (func_void_t)(void) ;
typedef u32_t (func_int_t)(u32_t) ;
typedef u32_t (func_int_int_t)(u32_t, u32_t) ;
typedef u32_t (func_int_int_int_t)(u32_t, u32_t, u32_t) ;

SECTION_ON_FLASH static void
_show_init_result(void) {
    printf("DRAM initial: %s\n", init_result_string(parameters.dram_init_result));
    printf("FLASH initial: %s\n", init_result_string(parameters.flash_init_result));
    printf("CPU clock rate: %dMhz\n", pll_query_freq(PLL_DEV_CPU));
    printf("DSP clock rate: %dMhz\n", pll_query_freq(PLL_DEV_DSP));
    printf("LX clock rate: %dMhz\n", pll_query_freq(PLL_DEV_LX));
    printf("Memory clock rate: %dMhz\n", pll_query_freq(PLL_DEV_MEM));
}
void dram_setup(void);
void console_init(void);

#if 0
SECTION_ON_FLASH static cpr_t
_cli_show(int argc, char *argv[]) {
    if (argc<2) return CPR_INCOMPLETE_CMD;
    const char *fmt=NULL;
    const char *family=NULL;
    int verbose=0;
    char **_opt=argv;
    while(*(++_opt)!=NULL) {
        if (pblr_strcmp(*_opt, "-p")==0) fmt="    variable %s %s\n";
        else if (pblr_strcmp(*_opt, "-u")==0) fmt="    .%s = %s,\n";
        else if (pblr_strcmp(*_opt, "-v")==0) {fmt="%24s: %s\n"; verbose=1;}
        else family=*_opt;
    }

    int _dram_family=(pblr_strcmp(family, "dram_info")==0);
    int _pll_family=(!_dram_family)&&(pblr_strcmp(family, "pll_info")==0);
    int _init_family=(!_pll_family)&&(!_dram_family)&&(pblr_strcmp(family, "init_result")==0);
    
    if (_init_family) {
        _show_init_result();
    } else if (_dram_family) {
        if (fmt==NULL) fmt="^    set dram %s %s\n";
        dump_ddr_gen2_info(&para_ddr_info, fmt, verbose);
    } else if (_pll_family) {
        if (fmt==NULL) fmt="^    set pll %s %s\n";
        dump_pll_gen1_info(&para_pll_info, fmt, verbose);
    } else {
        return CPR_UNSUPPORT_PARAMETER;
    }
    return CPR_NEXT;
}
#endif

SECTION_ON_FLASH static cpr_t
_cli_show(int argc, char *argv[]) {
    if (argc<2) return CPR_INCOMPLETE_CMD;
    const char *fmt=NULL;
    const char *family=NULL;
    u32_t flags=0, flags_mask=DPT_OMIT_MASK;
    char **_opt=argv;
    
    while(*(++_opt)!=NULL) {
        if (pblr_strcmp(*_opt, "-p")==0) {
            fmt="    variable %s %s\n";
            flags=0;
            flags_mask=DPT_VIRTUAL_MASK;
        } else if (pblr_strcmp(*_opt, "-u")==0) {
            fmt="    .%s = %s,\n";
            flags=0;
            flags_mask=DPT_VIRTUAL_MASK;
        } else if (pblr_strcmp(*_opt, "-v")==0) {
            fmt="%24s: %s\n";
            flags=0;
            flags_mask=0;
        } else family=*_opt;
    }

    int _dram_family=(pblr_strcmp(family, "dram_info")==0);
    int _pll_family=(!_dram_family)&&(pblr_strcmp(family, "pll_info")==0);
    int _init_family=(!_pll_family)&&(!_dram_family)&&(pblr_strcmp(family, "init_result")==0);
    int _cpu_family=(pblr_strcmp(family, "cpu_info")==0);
    
    if (_init_family) {
        _show_init_result();
    } else if (_dram_family) {
        if (fmt==NULL) fmt="    set dram %s %s\n";
        dump_ddr_gen2_info(&para_ddr_info, fmt, flags, flags_mask);
    } else if (_pll_family) {
        if (fmt==NULL) fmt="    set pll %s %s\n";
        dump_pll_gen1_info(&para_pll_info, fmt, flags, flags_mask);
    } else if (_cpu_family) {
        if (fmt==NULL) fmt="    set cpu %s %s\n";
        dump_cpu_info(fmt, flags, flags_mask);
    } else {
        return CPR_UNSUPPORT_PARAMETER;
    }
    return CPR_NEXT;
}

SECTION_ON_FLASH static cpr_t
_cli_call(int argc, char *argv[]) {
    if (argc<2) return CPR_INCOMPLETE_CMD;
    const char *fname=argv[1];
    
    u32_t res=0, arg1, arg2, arg3;
    arg1=(argc>=3)?pblr_atoi(argv[2]):0;
    arg2=(argc>=4)?pblr_atoi(argv[3]):0;
    arg3=(argc>=5)?pblr_atoi(argv[4]):0;
    if (is_digit(*fname)==0) {
        u32_t faddr=pblr_atoi(fname);
        switch (argc) {
            case 2: res=((func_void_t*)faddr)(); break;
            case 3: res=((func_int_t*)faddr)(arg1); break;
            case 4: res=((func_int_int_t*)faddr)(arg1, arg2); break;
            case 5: res=((func_int_int_int_t*)faddr)(arg1, arg2, arg3); break;
            default: return CPR_UNSUPPORT_PARAMETER;
        }
        
    } else if (pblr_strcmp(fname, "dram_setup")==0) {
        dram_setup();
        _show_init_result();
        return CPR_NEXT;
    } else if (pblr_strcmp(fname, "pll_setup")==0) {
#if defined(CMU_DIVISOR)
	    pll_setup();
#else
        pll_gen1_setup();
		console_init();
#endif
        return CPR_NEXT;
    } else if (pblr_strcmp(fname, "write_pattern")==0) {
        if (argc<4) return CPR_INCOMPLETE_CMD;
        board_dram_write_pattern_generation(&para_ddr_info, arg1, arg2);
    } else if (pblr_strcmp(fname, "read_pattern")==0) {
        if (argc<3) return CPR_INCOMPLETE_CMD;
        board_dram_read_pattern_generation(&para_ddr_info, arg1);
    } else if (pblr_strcmp(fname, "read_auto_tune")==0) {
	board_dram_read_auto_tune(&para_ddr_info, arg1);
    } else return CPR_UNSUPPORT_PARAMETER;
    printf("$=0x%08x\n", res);
    return CPR_NEXT;
}

#if defined(CMU_DIVISOR)
u32_t cmu_division(u32_t divisor);
#endif

SECTION_ON_FLASH static cpr_t
_cli_set(int argc, char *argv[]) {
	if (argc<4) return CPR_INCOMPLETE_CMD;
	const char *family=argv[1];
	const char *vname=argv[2];
	const char *value1=argv[3];
	if (pblr_strcmp(family, "dram")==0) {
		if (set_ddr_gen2_para((dram_gen2_info_t*) &para_ddr_info, vname, value1)==0) return CPR_NEXT;
		// other style set command
		if (pblr_strcmp(vname, "ctl_odt_ocd")==0) {
			if (argc<5) return CPR_INCOMPLETE_CMD;
			u32_t v1=pblr_atoi(value1);
			u32_t v2=pblr_atoi(argv[4]);
			cli_dg2_memctl_set_ODT_OCD((dram_gen2_info_t*)&para_ddr_info, v1, v2);
		} else return CPR_UNSUPPORT_PARAMETER;
	} else if (pblr_strcmp(family, "pll")==0) {
		if (set_pll_gen1_para((pll_gen1_info_t*)&para_pll_info, vname, value1)==0) return CPR_NEXT;
		if (pblr_strcmp(vname, "clock")==0) {
			if (argc<7) return CPR_INCOMPLETE_CMD;
			u32_t v1=pblr_atoi(value1);
			u32_t v2=pblr_atoi(argv[4]);
			u32_t v3=pblr_atoi(argv[5]);
			u32_t v4=pblr_atoi(argv[6]);
			/*
			  pll_gen1_set((pll_gen1_info_t*)&para_pll_info, v1, v2, v3, v4);
			*/
			pll_gen1_mhz_t mhz={.mode=PLL_MODE_BY_SW};
			mhz.cpu=v1;
			mhz.dsp=v2;
			mhz.mem=v3;
			mhz.lx=v4;
			u32_t baud = query_pg1_uart_baudrate();
			pll_gen1_set_from_mhz((pll_gen1_info_t*)&para_pll_info, &mhz);
			set_pg1_uart_baud_div(baud);
		}
#if defined(CMU_DIVISOR)
	} else if (pblr_strcmp(family, "cmu") == 0) {
		if (pblr_strcmp(vname, "div") == 0) {
			u32_t v1 = pblr_atoi(value1);
			/* Wait for terminal to consume UART FIFO.
			   With baud-rate 115200, each byte requires 86.8us.
			   Since UART FIFO contains 16-byte, here delays 86.8 * 16 ~= 1500us. */
			udelay(1500);
			cmu_division(v1);
			((volatile peripheral_info_t *)&parameters.soc.peri_info)->baudrate_divisor =
				(pll_query_freq(PLL_DEV_LX) * 1000000) / (16 * query_pg1_uart_baudrate()) - 1;
			console_init();
		}
#endif
	} else if ((pblr_strcmp(family, "cpu")==0)) {
		if (set_cpu_para(vname, value1)==0) return CPR_NEXT;
    } else return CPR_UNSUPPORT_PARAMETER;
	return CPR_NEXT;
}

SECTION_ON_FLASH static cpr_t
_cli_get(int argc, char *argv[]) {
    if (argc<3) return CPR_INCOMPLETE_CMD;
    const char *family=argv[1];
    const char *vname=argv[2];
    char buf[32];
    if (pblr_strcmp(family, "dram")==0) {
        if (get_ddr_gen2_info(&para_ddr_info, vname, buf)!=0) return CPR_UNSUPPORT_PARAMETER;
    } else if (pblr_strcmp(family, "pll")==0) {
        if (get_pll_gen1_info(&para_pll_info, vname, buf)!=0) return CPR_UNSUPPORT_PARAMETER;
    } else if ((pblr_strcmp(family, "cpu")==0)) {
		if (get_cpu_info(vname, buf)!=0) return CPR_UNSUPPORT_PARAMETER;
    } else return CPR_UNSUPPORT_PARAMETER;
    printf("$=%s\n", buf);
    return CPR_NEXT;
}

SECTION_ON_FLASH static cpr_t
_cli_help(int argc, char *argv[]) {
    pblr_puts(
        "call <func-addr> [<int-arg1> [<int-arg2>]]\n"
        "call dram_setup\n"
        "call pll_setup\n"
        "call write_pattern <times> <pattern>\n"
        "call read_pattern <times>\n"
        "call read_auto_tune [<window_size>]\n"
        "dw/md <addr> [length]\n"
        "ew/mw <addr> <value>\n"
        "exit\n"
        "get <dram|pll|cpu> <parameter>\n"
        "mt <start address> <size> [-t/-times <n|forever>] [-p/-pattern <adr_rot_test>] [-p/-pattern <uls_test>] [-p/-pattern <mdram_test>]\n"
        "save_para\n"
        "set <dram|pll|cpu> <parameter> <value>\n"
        "set dram ctl_odt_ocd <odt-value> <ocd-value>\n"
        "set pll clock <cpu-clk> <dsp-clk> <mem-clk> <lx-clk>\n"
#if defined(CMU_DIVISOR)
        "set cmu div [1|2|4|8|16|32|64|128]\n"
#endif
        "show init_result\n"
        "show <dram_info|pll_info|cpu_info> [-u|-p|-v]\n"
        "timer [1udelay|10udelay|100udelay|get_timer [min]]\n"
#if (OTTO_PLR_CLI_XMODEM_SUPPORT == 1)
        "ximg\n"
#endif /* #if (DISABLE_OTTO_PLR_CLI_XMODEM_SUPPORT != 1) */
        );
    return CPR_NEXT;
}

cli_cmd_entry_t cli_cmd_list[]= {
    {"help", _cli_help},
    {"call", _cli_call},
    {"show", _cli_show},
    {"set", _cli_set},
    {"get", _cli_get},
    {"exit", cli_std_exit},
    {"ew", cli_std_store_word},
    {"mw", cli_std_store_word},
    {"dw", cli_std_load_word},
    {"md", cli_std_load_word},
    {"timer", cli_std_timer},
    {"mt", cli_std_mt},
    {"save_para", cli_std_save_para},
#if (DISABLE_OTTO_PLR_CLI_XMODEM_SUPPORT != 1) 
    {"ximg", cli_update_image_by_xmodem},
#endif /* #if (DISABLE_OTTO_PLR_CLI_XMODEM_SUPPORT != 1) */
    {NULL, NULL}
};

SECTION_ON_FLASH static cpr_t
line_parser(char *cmd_buf) {
    return cli_line_parser(cli_cmd_list, cmd_buf);
}


SECTION_ON_FLASH void
cli_entry(void) {
    // check if enter cli
    if ((parameters.dram_init_result==INI_RES_OK)&&
#if (OTTO_NOR_SPI_FLASH == 1)
        (parameters.soc.layout.bootloader1_addr!=0)&&
#endif
        (parameters.flash_init_result==INI_RES_OK)) {
        if (plr_tstc()==0) return;
        if (plr_getc()!='.') return;
    }

    // cli start
    pblr_puts("Preloader cli start\n]");    
    util_init();
    
    // cli main-loop
    cli_main_loop(line_parser);

}
PATCH_REG(cli_entry, 1);
