#include <preloader.h>
#include "swp_util.h"
#include "swp_cli_util.h"
#if (OTTO_NAND_FLASH == 1)
#include "soc_nand_flash.h"
#endif


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
    char cmd_buf[132], *p;
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
            // execute command
            *p='\0'; 
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
        if ((p-cmd_buf)<128) *(p++)=chr;
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
    u32_t addr=pblr_atoi(argv[1]);
    printf("$=0x%08x\n", ACCESS_REG(addr));
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
cli_std_mdump(int argc, char *argv[]) {
    if (argc<2) return CPR_INCOMPLETE_CMD;
    const u8_t *addr=(const u8_t *)pblr_atoi(argv[1]);
    u32_t len=256;
    if (argc>=3) len=pblr_atoi(argv[2])*4;
    memory_dump(addr, len);
    return CPR_NEXT;
}

/**************************
  * DRAM prefetch machanism declaration
  * (DDR1/2/3 Combo PHY Controller)
  *************************/ 
#define MCR_PREFETCH_INS_SIDE  (0x2)
#define MCR_PREFETCH_DATA_SIDE (0x1)
#define MCR_PREFETCH_DIS_IMASK   (0xFF7FFFFF)
#define MCR_PREFETCH_DIS_DMASK   (0xFFBFFFFF)
#define MCR_PREFETCH_ENABLE_INS  (0x00800000)
#define MCR_PREFETCH_ENABLE_DATA (0x00400000)

/**************************
  * Memory Test structures
  *************************/ 
typedef struct {
	u8_t  patternNum;
	u32_t writeAddr;
	u32_t readAddr;
	u32_t testSize;
} memory_test_info_t;
enum CASE_NUMBER
{
	MT_CASE_BYTE=8,
	MT_CASE_HALF=16,
	MT_CASE_WORD=32,
};
static u32_t mt_patterns[] =
{
	0x89abcdef,
	0x5a5aa5a5,
	0x76543210, 
	0xa5a5a5a5,
	0x55555555, 
	0xaaaaaaaa,
	0x01234567, 
	0xfedcba98,
	0xffffffff,
	0xff00ff00,
	0x00ff00ff,
	0x0000ffff,
	0xffff0000,
};

/**************************
  * Macros for memory test
  *************************/ 
extern void plat_memctl_dcache_flush(void);
#define _DCache_flush_invalidate         plat_memctl_dcache_flush
#define PAT32_MASK   (0x1FFFFFFF)
#define WORD_SIZE	 (sizeof(u32_t))
#define SEQ_ADR_TEST (1<<0)

void error_printf(enum CASE_NUMBER err, u32_t pat,u32_t patTrans32, u32_t addr, u32_t readval)
{
	switch(err)
	{
		case MT_CASE_BYTE:
			pblr_puts(">>>>>Byte ERROR<<<<<\n");
			break;
		case MT_CASE_HALF:
			pblr_puts(">>>>>Half ERROR<<<<<\n");
			break;
		case MT_CASE_WORD:
			pblr_puts(">>>>>Word ERROR<<<<<\n");
			break;
	}
	printf("  mt_patterns=0x%x\n",pat);
	printf("  patTrans32=0x%x\n",patTrans32);
	printf("  *(0x%x)=0x%x\n",addr,readval);
	while(1);
}
u32_t rotate_1bit(u32_t v) 
{
	return (((v) << 1) | ((v & 0x80000000)>>31));
}
void word_access_test(memory_test_info_t *info)
{
	u8_t  patIdx;
	u32_t mt_pat32,patTrans32, i;
	volatile u32_t *test_addr_word;

	for(patIdx=0; patIdx<info->patternNum ; patIdx++)
	{
		mt_pat32 = mt_patterns[patIdx];
		_DCache_flush_invalidate();

		/*Write Word*/
		test_addr_word = (volatile u32_t *)(info->writeAddr);
		for(i=0;i<info->testSize/WORD_SIZE;i++)
		{
			patTrans32 = (((u32_t)test_addr_word & PAT32_MASK) ^ rotate_1bit(mt_pat32));
			*(test_addr_word++) = patTrans32;
		}
		
		/*Read Word*/
		test_addr_word = (volatile u32_t *)(info->readAddr);
		for(i=0;i<info->testSize/WORD_SIZE;i++)
		{
			patTrans32 = (((u32_t)test_addr_word & PAT32_MASK) ^ rotate_1bit(mt_pat32));
			if(*(test_addr_word) != patTrans32)
				error_printf(MT_CASE_WORD,mt_pat32,patTrans32,(u32_t)test_addr_word,*(test_addr_word));
			test_addr_word++;
		}
	}
	pblr_puts("Word_OK ");
}
void half_access_test(memory_test_info_t *info)
{
	u8_t  patIdx;
	u32_t patTrans32, mt_pat32,i;
	u16_t wr16_1, wr16_2, rd16_1, rd16_2;
	volatile u16_t *test_addr_half;

	for(patIdx=0; patIdx<info->patternNum ; patIdx++)
	{		
		mt_pat32 = mt_patterns[patIdx];
		_DCache_flush_invalidate();

		/*Write half-word*/
		test_addr_half = (volatile u16_t *)(info->writeAddr);
		for(i=0;i<info->testSize/WORD_SIZE;i++)
		{
			patTrans32 = (((u32_t)test_addr_half & PAT32_MASK) ^ rotate_1bit(mt_pat32));
			wr16_1 = (patTrans32 & 0xffff);
			wr16_2 = (patTrans32>>16) & 0xffff;
			*(test_addr_half++) = wr16_1;
			*(test_addr_half++)	= wr16_2;
		}
		
		/*Read half-word*/
		test_addr_half = (volatile u16_t *)(info->readAddr);
		for(i=0;i<info->testSize/WORD_SIZE;i++)
		{
			patTrans32 = (((u32_t)test_addr_half & PAT32_MASK) ^ rotate_1bit(mt_pat32));
			wr16_1 = (patTrans32 & 0xffff);
			wr16_2 = (patTrans32>>16) & 0xffff;
			
			rd16_1 = *(test_addr_half);
			if(rd16_1 != wr16_1)
				error_printf(MT_CASE_HALF,mt_pat32,wr16_1,(u32_t)test_addr_half,rd16_1);
						
			rd16_2 = *(++test_addr_half);
			if(rd16_2 != wr16_2)
				error_printf(MT_CASE_HALF,mt_pat32,wr16_2,(u32_t)test_addr_half,rd16_2);

			test_addr_half++;
		}
	}
	pblr_puts("Half_OK ");
}

void byte_access_test(memory_test_info_t *info)
{
	u8_t  patIdx;
	u32_t patTrans32,mt_pat32,i;
	u8_t  wr8_1,wr8_2,wr8_3,wr8_4,rd8_1,rd8_2,rd8_3,rd8_4;
	volatile u8_t *test_addr_byte;

	for(patIdx=0; patIdx<info->patternNum ; patIdx++)
	{	
		mt_pat32 = mt_patterns[patIdx];
		_DCache_flush_invalidate();
		
		/*Write Byte*/
		test_addr_byte = (volatile u8_t *)(info->writeAddr);
		for(i=0 ; i<info->testSize/WORD_SIZE; i++)
		{
			patTrans32 = (((u32_t)test_addr_byte & PAT32_MASK) ^ rotate_1bit(mt_pat32));
			wr8_1 = (patTrans32>>0) & 0xffff;
			wr8_2 = (patTrans32>>8) & 0xffff;
			wr8_3 = (patTrans32>>16)& 0xffff;
			wr8_4 = (patTrans32>>24)& 0xffff;
			*(test_addr_byte++) = wr8_1;
			*(test_addr_byte++) = wr8_2;
			*(test_addr_byte++) = wr8_3;
			*(test_addr_byte++) = wr8_4;
		}
		
		/*Read Byte*/
		test_addr_byte = (volatile u8_t *)(info->readAddr);
		for(i=0 ; i<info->testSize/WORD_SIZE; i++)
		{
			patTrans32 = (((u32_t)test_addr_byte & PAT32_MASK) ^ rotate_1bit(mt_pat32));
			wr8_1 = (patTrans32>>0) & 0xffff;
			wr8_2 = (patTrans32>>8) & 0xffff;
			wr8_3 = (patTrans32>>16)& 0xffff;
			wr8_4 = (patTrans32>>24)& 0xffff;

			rd8_1 = *(test_addr_byte);
			if(rd8_1 != wr8_1)
				error_printf(MT_CASE_BYTE, mt_pat32,wr8_1,(u32_t)test_addr_byte,rd8_1); 	
			
			rd8_2 = *(++test_addr_byte);
			if(rd8_2 != wr8_2)
				error_printf(MT_CASE_BYTE, mt_pat32,wr8_2,(u32_t)test_addr_byte,rd8_2);
			
			rd8_3 = *(++test_addr_byte);
			if(rd8_3 != wr8_3)
				error_printf(MT_CASE_BYTE, mt_pat32,wr8_3,(u32_t)test_addr_byte,rd8_3);
			
			rd8_4 = *(++test_addr_byte);
			if(rd8_4 != wr8_4)
				error_printf(MT_CASE_BYTE, mt_pat32,wr8_4,(u32_t)test_addr_byte,rd8_4);

			test_addr_byte++;
		}
	}
	pblr_puts("Byte_OK");
}
void disable_DRAM_prefech(u32_t side_id)
{
	volatile u32_t *reg_mcr;

	reg_mcr = (volatile u32_t *)0xB8001000;

	if( side_id & MCR_PREFETCH_INS_SIDE )
		*reg_mcr =*reg_mcr & ((unsigned int)MCR_PREFETCH_DIS_IMASK);

	if( side_id & MCR_PREFETCH_DATA_SIDE)
		*reg_mcr =*reg_mcr & ((unsigned int)MCR_PREFETCH_DIS_DMASK);
}

void enable_DRAM_prefech(u32_t side_id)
{
	volatile u32_t *reg_mcr;

	reg_mcr = (volatile u32_t *)0xB8001000;

	if( side_id & MCR_PREFETCH_INS_SIDE )
	{
		disable_DRAM_prefech(MCR_PREFETCH_INS_SIDE);
		*reg_mcr = *reg_mcr | MCR_PREFETCH_ENABLE_INS;
	}
	
	if( side_id & MCR_PREFETCH_DATA_SIDE )
	{
		disable_DRAM_prefech(MCR_PREFETCH_DATA_SIDE);
		*reg_mcr = *reg_mcr | MCR_PREFETCH_ENABLE_DATA;
	}
}

const char *char_wr_cache_case[] = {
	"  [Uncache-W/Uncache-R]",
	"  [Uncache-W/Cache-R  ]",
	"  [Cache-W  /Cache-R  ]",
};
void dram_adr_rotate_test(u32_t startAddr, u32_t size)
{
	memory_test_info_t mt_info;
	u8_t  caseIdx;
	u32_t dram_write_addr[3];
	u32_t dram_read_addr[3];

	dram_write_addr[0] = (startAddr&PAT32_MASK)|0xA0000000;
	dram_write_addr[1] = (startAddr&PAT32_MASK)|0xA0000000;
	dram_write_addr[2] = (startAddr&PAT32_MASK)|0x80000000;
	dram_read_addr[0] = (startAddr&PAT32_MASK)|0xA0000000;
	dram_read_addr[1] = (startAddr&PAT32_MASK)|0x80000000;
	dram_read_addr[2] = (startAddr&PAT32_MASK)|0x80000000;
	mt_info.patternNum = 1;
	mt_info.testSize = size;

	for(caseIdx=0 ; caseIdx<3 ; caseIdx++)
	/* Case0: Uncached-W / Uncached-R
	  * Case1: Uncached-W / Cached-R
	  * Case2: Cached-W / Cached-R
	  */		
	{
		printf("\r%s ",char_wr_cache_case[caseIdx]);
		mt_info.writeAddr = dram_write_addr[caseIdx];
		mt_info.readAddr  = dram_read_addr[caseIdx];
		word_access_test(&mt_info);
		half_access_test(&mt_info);
		byte_access_test(&mt_info);
		if(caseIdx==2)
		{
			/*Data Prefetch*/
			pblr_puts("\r(Enable Data Prefetch)");
			enable_DRAM_prefech(MCR_PREFETCH_DATA_SIDE);
			word_access_test(&mt_info);
			half_access_test(&mt_info);
			byte_access_test(&mt_info);
			disable_DRAM_prefech( MCR_PREFETCH_INS_SIDE | MCR_PREFETCH_DATA_SIDE ); 	

			/*Instuction Prefetch*/
			pblr_puts("\r(Enable Instuction Prefetch)");
			enable_DRAM_prefech(MCR_PREFETCH_INS_SIDE);
			word_access_test(&mt_info);
			half_access_test(&mt_info);
			byte_access_test(&mt_info);
			disable_DRAM_prefech(MCR_PREFETCH_INS_SIDE|MCR_PREFETCH_DATA_SIDE);
			
			/*Data & Instuction Prefetch*/
			pblr_puts("\r(Enable Data & Instuction Prefetch)");
			enable_DRAM_prefech(MCR_PREFETCH_DATA_SIDE|MCR_PREFETCH_INS_SIDE);				
			word_access_test(&mt_info);
			half_access_test(&mt_info);
			byte_access_test(&mt_info);
			disable_DRAM_prefech(MCR_PREFETCH_INS_SIDE|MCR_PREFETCH_DATA_SIDE);
		}
	}
}

SECTION_ON_FLASH cpr_t
cli_std_mt(int argc, char *argv[]) {
/*
"mt <start address> <size> [-t/-times <n|forever>]  [-p/-pattern <seq_adr_test|...|all>]\n"
 */
    if (argc<3) return CPR_INCOMPLETE_CMD;

	u32_t start_addr=0;
	u32_t size=0;
	u32_t times=1;
	u32_t testCases=SEQ_ADR_TEST;

	u32_t i=1;
	while(i<argc)
	{
		if((pblr_strcmp(argv[i], "-t")==0)||(pblr_strcmp(argv[i], "-times")==0)){
			if(pblr_strcmp(argv[i+1], "forever")==0){
				times = 0xFFFFFFFF;
				i++;
			}else if(pblr_atoi(argv[i+1])!=0){
				times = pblr_atoi(argv[++i]);	
			}else{
				return CPR_INCOMPLETE_CMD;
			}
		}else if((pblr_strcmp(argv[i], "-p")==0)||(pblr_strcmp(argv[i], "-pattern")==0)){
			if(pblr_strcmp(argv[++i], "seq_adr_test")==0){
				testCases = testCases|SEQ_ADR_TEST;
			}else{
				return CPR_UNSUPPORT_PARAMETER;
			}
		}else{
			if((argv[i]==NULL)||(argv[i+1]==NULL)){
				return CPR_INCOMPLETE_CMD;
			}else{
				start_addr = pblr_atoi(argv[i]);
				size = pblr_atoi(argv[++i]);
			}
		}
		i++;
	}
	if((start_addr==0)||(size==0)){
		return CPR_INCOMPLETE_CMD;
	}

	#if(times==0xFFFFFFFF)
		while(1){		
	#else
		for(i=0;i<times;i++){
	#endif
			if((testCases&SEQ_ADR_TEST)==SEQ_ADR_TEST){
				dram_adr_rotate_test(start_addr,size);			
				printf("\r<<< dram_adr_rotate_test %d times >>>\n",i+1);
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
    
    memcpy(org_soc, para_soc, sizeof(soc_t));
    pblr_nor_spi_erase(0, 0);
    for(i=0; i<4096; i+=ws){
        pblr_nor_spi_write(0, i, ws, (const void *) (SRAM_BASE+i));
    }
    pblr_puts("earse...write...done\n");
    return CPR_NEXT;
}
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
#endif //(OTTO_NAND_FLASH == 1)
