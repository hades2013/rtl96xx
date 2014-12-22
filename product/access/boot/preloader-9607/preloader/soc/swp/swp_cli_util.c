#include <preloader.h>
#include "swp_util.h"
#include "swp_cli_util.h"
#if (OTTO_NAND_FLASH == 1)
#include "soc_nand_flash.h"
#endif
#if defined(CMU_DIVISOR)
#include <plr_pll_gen1.h>
#include "swp_pll_gen1_util.h"
#endif

#if (DISABLE_OTTO_PLR_CLI_XMODEM_SUPPORT != 1)
#include "swp_xmodem.h"
#endif /* #if (DISABLE_OTTO_PLR_CLI_XMODEM_SUPPORT != 1) */

SECTION_ON_FLASH char *
get_token(char **cmd_buf) {
    char *r=NULL;
    char *p=*cmd_buf;
    if (p==NULL) return NULL;
    
    // skip leading space
    while ((*p==' ') || (*p=='\t')) ++p;
    if (*p!='\0') {
        // first character
        r=p;
        //looking end of token
        while ((*p!=' ') && (*p!='\t') && (*p!='\0')) ++p;
        if (*p!='\0') *(p++)='\0';
    }
    *cmd_buf=p;
    return r;
}


SECTION_ON_FLASH cpr_t 
cli_main_loop(line_parser_t *line_parser) {
    char cmd_buf[CMD_INP_BUF_SZ+4], *p, cmd_backup[CMD_INP_BUF_SZ];
    cmd_backup[0]='\0';
    
    p=cmd_buf;
    while(1) {
        char chr=plr_getc();
        if (chr==0) continue;
            
        // echo char
        if (((chr>=32)&&(chr<='z'))||chr==CHAR_BACKSPACE) {
        	pblr_putc(chr);
        }
        
        if (chr==CHAR_ESC) {
            pblr_puts("\n]");
            p=cmd_buf;
            continue;
        }
        if (chr==CHAR_BACKSPACE) {
            if (p!=cmd_buf) {
                pblr_putc(' ');
                pblr_putc(CHAR_BACKSPACE);
                --p;
            }
            continue;
        }
        if (chr==0x0d) {
            *p='\0'; 
            memcpy((void*)cmd_backup, (const void*)cmd_buf, CMD_INP_BUF_SZ);
        } else if (chr==0x7f) {
            // repeat
            memcpy((void*)cmd_buf, (const void*)cmd_backup, CMD_INP_BUF_SZ);
            if (p!=cmd_buf) pblr_putc(';');
            pblr_puts(cmd_buf);
            chr=0x0d;
        }
        if (chr==0x0d) {
            // execute command
            p=cmd_buf;
            
            // command echo            
            #if 0
            pblr_puts("exec:");
            pblr_puts(cmd_buf);
            char *x=cmd_buf, cc;
            while (1) {
                if ((cc=*(x++))=='\0') break;
                printf("%c(%02d)", cc, (u32_t)cc);
            }
            #endif
            pblr_putc('\n');

            // command parsing
            if (cmd_buf[0]=='.') continue;
            cpr_t r=(*line_parser)(cmd_buf);
            switch (r) {
                case CPR_EXIT: 
                    return CPR_EXIT;
                case CPR_UNKNOWN_CMD:
                    pblr_puts("unknown command\n]");
                    break;
                case CPR_UNSUPPORT_PARAMETER:
                    pblr_puts("unsupported parameters\n]");
                    break;
                case CPR_INCOMPLETE_CMD:
                    pblr_puts("incomplete comand\n]");
                    break;
                default:
                case CPR_NEXT: 
                    pblr_puts("]");
                    break;
            }
            continue;
        }
        if ((chr<32)||(chr>'z')) {
            printf("(%02x)", (unsigned char)chr);
            continue;
        }
        if ((p-cmd_buf)<CMD_INP_BUF_SZ) *(p++)=chr;
    }
}

SECTION_ON_FLASH cpr_t
cli_line_parser(const cli_cmd_entry_t *list, char *cmd_buf) {
    int _argc=0;
    char *_argv[MAX_ARGV+1];
    char *tkn;
    
    // check argument
    if (list==NULL) return CPR_NEXT;
    if (cmd_buf==NULL) return CPR_NEXT;

    // parse cmd into tokens into argc,argv
    while (((tkn=get_token(&cmd_buf))!=NULL)&&(_argc<MAX_ARGV))
        _argv[_argc++]=tkn;
    if (_argc==0) return CPR_NEXT;
    const char *cmd_name=_argv[0];
    _argv[_argc]=NULL;
    
    // find the cmd_entry and exec
    while (list->cmd_name!=NULL) {
        if (pblr_strcmp(cmd_name, list->cmd_name)==0) {
            if (list->cli_cmd!=NULL) return (list->cli_cmd)(_argc, _argv);
        }
        ++list;
    }
    return CPR_UNKNOWN_CMD;
}


SECTION_ON_FLASH void
test_udelay(u32_t unit, u32_t min) {
    u32_t sec_loop=1000000/unit;
    u32_t outer_loop=min*60;
    u32_t i, j;
    pblr_puts("00:00");
    for (i=0; i<outer_loop; ++i) {
        for (j=0; j<sec_loop; ++j) {
            udelay(unit);
        }
        printf("\r%02d:%02d", i/60, i%60);
    }
    pblr_puts(": done\n");
}
SECTION_ON_FLASH void
test_get_timer(u32_t min) {
    u32_t sec_delay=min*60;
    u32_t last_sec=0, sec;
    pblr_puts("00:00");
    
    u32_t start_ms=pblr_get_timer(0);
    
    do {
        sec=(pblr_get_timer(start_ms))/1000;
        if (sec!=last_sec) {
            last_sec=sec;
            printf("\r%02d:%02d", last_sec/60, last_sec%60);
        }
    } while (sec<sec_delay);
    pblr_puts(": done\n");
}

SECTION_ON_FLASH cpr_t cli_std_exit(int argc, char *argv[]) {return CPR_EXIT; }
SECTION_ON_FLASH cpr_t 
cli_std_store_word(int argc, char *argv[]) {
    if (argc<3) return CPR_INCOMPLETE_CMD;
    u32_t addr=pblr_atoi(argv[1]);
    u32_t arg=pblr_atoi(argv[2]);
    ACCESS_REG(addr)=arg;
    return CPR_NEXT;
}
SECTION_ON_FLASH cpr_t 
cli_std_load_word(int argc, char *argv[]) {
    if (argc<2) return CPR_INCOMPLETE_CMD;
    else if(argc==2) {
    	u32_t addr=pblr_atoi(argv[1]);
    	printf("$=0x%08x\n", ACCESS_REG(addr));
    } else{
	const u8_t *addr=(const u8_t *)pblr_atoi(argv[1]);
	u32_t len=pblr_atoi(argv[2])*4;
	memory_dump(addr, len);
    }
    return CPR_NEXT;
}
SECTION_ON_FLASH cpr_t 
cli_std_timer(int argc, char *argv[]) {
    u32_t min=1;
    if (argc>=3) min=pblr_atoi(argv[2]);

    const char *method=argv[1];
    if ((method==NULL)||(pblr_strcmp(method, "get_timer")==0)) {
        test_get_timer(min);
    } else if (pblr_strcmp(method, "1udelay")==0) {
            test_udelay(1, min);
    } else if (pblr_strcmp(method, "10udelay")==0) {
        test_udelay(10, min);
    } else if (pblr_strcmp(method, "100udelay")==0) {
        test_udelay(100, min);
    } else {
        return CPR_UNSUPPORT_PARAMETER;
    }
    return CPR_NEXT;
}

SECTION_ON_FLASH cpr_t
cli_std_mt(int argc, char *argv[]) {
/*
  *  "mt <start address> <size> [-t/-times <n|forever>]  [-p/-pattern <adr_rot_test|uls_test|mdram_test|...|all>]\n"
 */
    if (argc<3) return CPR_INCOMPLETE_CMD;

	u32_t start_addr=0;
	u32_t size=0;
	u32_t times=1;
	u32_t testCases=0;
	u32_t i=1;

	while(i<argc){
		if((pblr_strcmp(argv[i],"-t") == 0) || (pblr_strcmp(argv[i],"-times") == 0)){
			if(pblr_strcmp(argv[i+1],"forever") == 0){
				times = 0xFFFFFFFF;
			}else if(pblr_atoi(argv[i+1]) != 0){
				times = pblr_atoi(argv[i+1]);	
			}else{
				return CPR_INCOMPLETE_CMD;
			}
		}else if((pblr_strcmp(argv[i],"-p") == 0)||(pblr_strcmp(argv[i],"-pattern") == 0)){
			if(pblr_strcmp(argv[i+1],"adr_rot_test") == 0){
				testCases = testCases|CLI_MT_CASE_ADR_ROT_TEST;
			}else if(pblr_strcmp(argv[i+1],"uls_test") == 0){
				testCases = testCases|CLI_MT_CASE_ULS_TEST;			
			}else if(pblr_strcmp(argv[i+1],"mdram_test") == 0){
				testCases = testCases|CLI_MT_CASE_MDRAM_TEST;			
			}else{
				return CPR_UNSUPPORT_PARAMETER;
			}
		}else{
			if((argv[i] == NULL) || (argv[i+1] == NULL)){
				return CPR_INCOMPLETE_CMD;
			}else{
				start_addr = pblr_atoi(argv[i]);
				size = pblr_atoi(argv[i+1]);
				if((start_addr == 0) || (size == 0)){
					return CPR_UNKNOWN_CMD;
				}
			}
		}
		i+=2;
	}
	
	if(0 == testCases){
		testCases = CLI_MT_CASE_ADR_ROT_TEST|CLI_MT_CASE_ULS_TEST|CLI_MT_CASE_MDRAM_TEST;
	}
	
#if(times == 0xFFFFFFFF)
	while(1){		
#else
	for(i=0;i<times;i++){
#endif
		if((testCases&CLI_MT_CASE_ADR_ROT_TEST) == CLI_MT_CASE_ADR_ROT_TEST){
			if(CLI_MT_FAIL == dram_adr_rotate_test(start_addr,size))
				HANDLE_FAIL;
			printf("<<< dram_adr_rotate_test %d times >>>\n",i+1);
		}
		if((testCases&CLI_MT_CASE_ULS_TEST) == CLI_MT_CASE_ULS_TEST){
			if(CLI_MT_FAIL == unaligned_test(start_addr,size))
				HANDLE_FAIL;
			printf("<<< unaligned_test %d times >>>\n",i+1);
		}
		if((testCases&CLI_MT_CASE_MDRAM_TEST) == CLI_MT_CASE_MDRAM_TEST){
			if(CLI_MT_FAIL == mdram_test(start_addr,size))
				HANDLE_FAIL;
			printf("<<< mdram_test %d times >>>\n",i+1);
		}
	}
    return CPR_NEXT;
}
#if (OTTO_NOR_SPI_FLASH == 1)
SECTION_ON_FLASH cpr_t 
cli_std_save_para(int argc, char *argv[]) {
    if (para_flash_info.erase_unit!=12) {
        // check if 4096 byte erase function supported
        pblr_puts("abort: no 4K erase command supported\n");
        return CPR_NEXT;
    }
    void *org_soc=(void *)&((soc_configuration_t *)(SRAM_BASE + SOC_CONF_OFFSET))->soc;
    void *para_soc=(void *)(&parameters.soc);
    u32_t i, ws=(0x1 << para_flash_info.wr_boundary);
    
#if defined(CMU_DIVISOR)
    set_pg1_uart_baud_div(query_pg1_uart_baudrate());
#endif

    memcpy(org_soc, para_soc, sizeof(soc_t));
    pblr_nor_spi_erase(0, 0);
    for(i=0; i<4096; i+=ws){
        pblr_nor_spi_write(0, i, ws, (const void *) (SRAM_BASE+i));
    }

#if defined(CMU_DIVISOR)
	((volatile peripheral_info_t *)&parameters.soc.peri_info)->baudrate_divisor =
		(pll_query_freq(PLL_DEV_LX) * 1000000) / (16 * query_pg1_uart_baudrate()) - 1;
#endif

    pblr_puts("earse...write...done\n");
    return CPR_NEXT;
}

#if (DISABLE_OTTO_PLR_CLI_XMODEM_SUPPORT != 1)
cpr_t
cli_update_image_by_xmodem(int argc, char *argv[]) {
    u32_t i;
    u32_t es = (0x1 << para_flash_info.erase_unit);/* Erase size */
    u32_t bls;/* bootloader size */
    u8_t re_burn;

    bls = para_flash_layout.env_addr; 

    if ((bls % para_flash_info.erase_unit) != 0) {
        pblr_puts("abort: bootloader is not aligned with flash erase unit\n");
        return CPR_NEXT;
    }
 
    printf("Start erasing flash 0x0 ~ 0x%x\n", bls);
start_download:
    /* Erase bootloader */
    for(i=0; i<bls; i+=es){ 
        pblr_nor_spi_erase(0, i);
    }

    /* Afterwards, we cannot use pblr_puts() function as its parameters is located at flash area which were just erased */
    /* Call xmodem to retrive the image and write it to the destination */
    if(xmodem_2_flash(0, bls) != 0) {
        pblr_putc('F');
        pblr_putc('a');
        pblr_putc('i');
        pblr_putc('l');
        pblr_putc('e');
        pblr_putc('d');
        pblr_putc('\n');
    } else {
        pblr_putc('d');
        pblr_putc('o');
        pblr_putc('n');
        pblr_putc('e');
        pblr_putc('\n');
    }

re_burn_chk:
    pblr_putc('R');
    pblr_putc('e');
    pblr_putc('b');
    pblr_putc('u');
    pblr_putc('r');
    pblr_putc('n');
    pblr_putc('(');
    pblr_putc('y');
    pblr_putc('/');
    pblr_putc('n');
    pblr_putc(')');
    pblr_putc('?');
    pblr_putc('\n');

    while (!plr_tstc());
    re_burn = plr_getc();
    pblr_putc(re_burn);
    pblr_putc('\n');

    if((re_burn == 'N') || (re_burn == 'n')) {
        /* Reboot */
        pblr_putc('\n');
        pblr_putc('R');
        pblr_putc('e');
        pblr_putc('b');
        pblr_putc('o');
        pblr_putc('o');
        pblr_putc('t');
        pblr_putc('i');
        pblr_putc('n');
        pblr_putc('g');
        pblr_putc('.');
        pblr_putc('.');
        pblr_putc('.');
        pblr_putc('\n');
 
        /* Use WDT HW reset */
        REG32(0xb8003268) = (0x1 << 31); 
        while(1);
    } else if((re_burn == 'Y') || (re_burn == 'y')) {
        goto start_download;
    } else {
        goto re_burn_chk;
    }

    return CPR_NEXT;
}
#endif /* #if (DISABLE_OTTO_PLR_CLI_XMODEM_SUPPORT != 1) */

#elif (OTTO_NAND_FLASH == 1)

#define sizeof_nand_dma_buf _sizeof_nand_dma_buf(para_chunk_size)

static int
nand_write_chunk(u8_t *chunk_buf, spare_u *spare, u32_t block_id, u32_t bchunk_id) {
    // 1. D-cache flush among dma_buf
    return (parameters._nand_write_chunk)(chunk_buf, 
        spare, block_id*para_num_chunk_per_block+bchunk_id);
}

static inline int
nand_write_dma_buf(nand_dma_buf_t *dma_buf, u32_t block_id, u32_t bchunk_id) {
    return nand_write_chunk(dma_buf->chunk_buf, &(dma_buf->spare), block_id, bchunk_id);
}

cpr_t 
cli_std_save_para(int argc, char *argv[]) {

    // buffer for nand dma
    u8_t *_dma_buf=alloca(sizeof_nand_dma_buf+CACHELINE_SIZE-4);
    nand_dma_buf_t *dma_buf=(nand_dma_buf_t *)CACHE_ALIGN(_dma_buf);
    u8_t *chunk_buf=dma_buf->chunk_buf;

    // 1. copy the current soc_t back to the SRAM_BASE
    void *org_soc=(void *)&((soc_configuration_t *)(SRAM_BASE + SOC_CONF_OFFSET))->soc;
    void *para_soc=(void *)(&parameters.soc);
    memcpy(org_soc, para_soc, sizeof(soc_t));
    pblr_bset(_dma_buf, 0xff, sizeof_nand_dma_buf+CACHELINE_SIZE-4);
    // 2. check if first copy of lpreloader is 'SKIP' by test if DMA read correct
    // 3. erase block 0
    
    parameters._nand_erase_block(0);
   //first 4k not need write ,all 0xff for reset issue.
    
    // 4. write first copy (from SRAM_BASE) by 4K or PIO write a 'SKIP'
    //test for 2k
    memcpy(chunk_buf, (void *)SRAM_BASE, para_chunk_size);
    nand_write_dma_buf(dma_buf,0,0);

    nand_write_dma_buf(dma_buf,0,2);
    nand_write_dma_buf(dma_buf,0,4);
    nand_write_dma_buf(dma_buf,0,6);
    
    memcpy(chunk_buf, (void *)(SRAM_BASE+para_chunk_size), para_chunk_size);
    nand_write_dma_buf(dma_buf,0,1);
    nand_write_dma_buf(dma_buf,0,3);    
    nand_write_dma_buf(dma_buf,0,5);
    nand_write_dma_buf(dma_buf,0,7);
    // 5. write second~forth copy (from SRAM_BASE) by 4K
    
//  pblr_puts("save to nand ok !\n");
    pblr_puts("earse...write...done\n");

    return CPR_NEXT;
}

#if (DISABLE_OTTO_PLR_CLI_XMODEM_SUPPORT != 1)
cpr_t
cli_update_image_by_xmodem(int argc, char *argv[]) {
    
    u32_t bls;/* bootloader size */
    u8_t re_burn;
    u32_t cs; /* chunk size */
    u32_t bbn; /* bootloader block number */
    u32_t cpb; /* chunks per block */
    u32_t i;

    cs  = parameters.chunk_size;
    cpb = parameters.num_chunk_per_block;
    bbn = parameters.end_pblr_block;
#define DEV_CODE
#ifdef DEV_CODE
    printf("parameters.chunk_size=%d\n", cs);
    printf("parameters.num_chunk_per_block=%d\n", cpb);
    printf("parameters.end_pblr_block=%d\n", bbn);
#endif /* #ifdef DEV_CODE */


    if(cs == 2048) {
        /* Include OOB size into chunk size to save efforts in the following calcualtion */
        cs += NAND_SPARE_AREA_SIZE;
    } else {
        printf("Error, NAND chunk size =%u, but only 2048 is supported\n", cs);
        return CPR_NEXT;
    }
    /* Calculate the size of bootloader area */
    bls = bbn * cpb * cs;
start_download:
    printf("Start erasing flash 0x0 ~ 0x%x(size=%d bytes)\n", bls, bls);
    /* Erase bootloader */
    for(i=0; i<bbn; i++) {
        printf("Erasing NAND block %u\n", i);
        parameters._nand_erase_block(i*cpb);
    }
    
    /* Call xmodem to retrive the image and write it to the destination */
    if(xmodem_2_flash(0, bls) != 0) {
        printf("Failed\n");
    } else {
        printf("Done\n");
    }

re_burn_chk:
    printf("Reburn(y/n)?\n");

    while (!plr_tstc());
    re_burn = plr_getc();
    pblr_putc(re_burn);
    pblr_putc('\n');

    if((re_burn == 'N') || (re_burn == 'n')) {
        /* Reboot */
        printf("\nRebooting...\n\n\n");

        /* Use system reset */
        REG32(0xbb000074) = (0x1 << 2); 
        while(1);
    } else if((re_burn == 'Y') || (re_burn == 'y')) {
        goto start_download;
    } else {
        goto re_burn_chk;
    }

    return CPR_NEXT;
}
#endif /* #if (DISABLE_OTTO_PLR_CLI_XMODEM_SUPPORT != 1) */

#endif //(OTTO_NAND_FLASH == 1)
