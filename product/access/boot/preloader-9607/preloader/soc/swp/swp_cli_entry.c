#include <preloader.h>
#include <cpu_utils.h>
#include "swp_xmodem.h"
#include "swp_util.h"

#define XBUF_SZ 8192

#ifdef TEST_XMODEM_RECEIVE
int main(void)
{
	int st;

	printf ("Send data using the xmodem protocol from your terminal emulator now...\n");
	/* the following should be changed for your environment:
	   0x30000 is the download address,
	   65536 is the maximum size to be written at this address
	 */
	st = xmodem_receive((char *)0x30000, 65536);
	if (st < 0) {
		printf ("Xmodem receive error: status: %d\n", st);
	}
	else  {
		printf ("Xmodem successfully received %d bytes\n", st);
	}

	return 0;
}
#endif


// command parsers result
typedef enum {
    CPR_NEXT,
    CPR_EXIT,
    CPR_INCOMPLETE_CMD,
    CPR_UNSUPPORT_PARAMETER,
    CPR_UNKNOWN_CMD    
} cpr_t;



// function types
typedef u32_t (func_void_t)(void) ;
typedef u32_t (func_int_t)(u32_t) ;
typedef u32_t (func_int_int_t)(u32_t, u32_t) ;

SECTION_ON_FLASH static char *
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
#if 0
SECTION_ON_FLASH static int
xmodem_download_first4K(char *xbuf) {
    pbldr_wzero_range((u32_t*)xbuf, (u32_t*)(xbuf+XBUF_SZ));
    int st = xmodem_receive(xbuf, XBUF_SZ);
    udelay(5000000);
    printf("\nreceive result=%d\n", st);
    memory_dump(xbuf, 512);
    // TODO:
    return st;
}
#else
static int 
_get_char(u32_t msec_delay) {
    u32_t start_ms=pblr_get_timer(0);
    do {
        if (plr_tstc()) 
            return plr_getc();
        udelay(10);
    } while (pblr_get_timer(start_ms)<msec_delay);
    return -1;
}

static u32_t xmodem_count, xmodem_retry, xmodem_prefix, xmodem_retry_prefix;

static int
xmodem_download_first4K(char *xbuf) {
    pbldr_wzero_range((u32_t*)xbuf, (u32_t*)(xbuf+XBUF_SZ));
    int chr;
    u32_t c=0, d=0, e=0, n=0;
    
    while(1) {
        // try to get first character
        while(1) {
            plr_putc('C');
            if ((chr=_get_char(990))>=0) break;
            ++d;
            if (n!=0) ++e;
        }
        
        // wait following
        while(1) {
            *(xbuf++)=(char)chr;
            if ((++c)>=16) {
                xmodem_count=c;
                xmodem_prefix=d;
                xmodem_retry_prefix=e;
                xmodem_retry=n;
                return 0;
            }
            if ((chr=_get_char(2*1000))<0) break;
        }
        ++n;
    }
}
#endif

SECTION_ON_FLASH static void
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
SECTION_ON_FLASH static void
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

SECTION_ON_FLASH static void
show_cli_help(void) {
    pblr_puts(
        "exit\n"
        "xmodem 0 4096\n"
        "show init_result\n"
        "show xbuf\n"
        "store_word/sw <addr> <value>\n"
        "load_word/lw <addr>\n"
        "call <func-addr> [<int-arg1> [<int-arg2>]]\n"
        "call dram_setup\n"
        "test 1udelay <min>\n"
        "test 10udelay <min>\n"
        "test 100udelay <min>\n"
        "test get_timer <min>\n"
        );
        
        
}
SECTION_ON_FLASH static void
_show_init_result(void) {
    printf("DRAM initial: %s\n", init_result_string(parameters.dram_init_result));
    printf("FLASH initial: %s\n", init_result_string(parameters.flash_init_result));
}
void dram_setup(void);
SECTION_ON_FLASH static cpr_t
cmd_parser(char *cmd_buf, u8_t *xbuf) {
    char *tkn=get_token(&cmd_buf);
    if (tkn==NULL) return CPR_NEXT;
    if (*tkn=='.') return CPR_NEXT;
    
    if (pblr_strcmp(tkn, "exit")==0) return CPR_EXIT;
    if (pblr_strcmp(tkn, "help")==0) {
        show_cli_help();
        return CPR_NEXT;
    }
    if (pblr_strcmp(tkn, "xmodem")==0) {
        if ((tkn=get_token(&cmd_buf))==NULL) return CPR_INCOMPLETE_CMD;
        u32_t addr=pblr_atoi(tkn);
        if ((tkn=get_token(&cmd_buf))==NULL) return CPR_INCOMPLETE_CMD;
        u32_t length=pblr_atoi(tkn);
        if (addr!=0) return CPR_UNSUPPORT_PARAMETER;
        if (length!=4096) return CPR_UNSUPPORT_PARAMETER;
        xmodem_download_first4K(xbuf);
        return CPR_NEXT;
    }
    if (pblr_strcmp(tkn, "show")==0) {
        if ((tkn=get_token(&cmd_buf))==NULL) return CPR_INCOMPLETE_CMD;
        if (pblr_strcmp(tkn, "init_result")==0) {
            _show_init_result();
        } else if (pblr_strcmp(tkn, "xbuf")==0) {
            memory_dump(xbuf, 128);
            printf("xmodem-count=%d\nxmodem_retry=%d\n"
                "xmodem_prefix=%d\nxmodem_retry_prefix=%d\n", 
                xmodem_count, xmodem_retry, xmodem_prefix, xmodem_retry_prefix);
        } else return CPR_UNSUPPORT_PARAMETER;
        return CPR_NEXT;
    }
    
    if ((pblr_strcmp(tkn, "sw")==0)||(pblr_strcmp(tkn, "store_word")==0)) {
        if ((tkn=get_token(&cmd_buf))==NULL) return CPR_INCOMPLETE_CMD;
        u32_t addr=pblr_atoi(tkn);
        if ((tkn=get_token(&cmd_buf))==NULL) return CPR_INCOMPLETE_CMD;
        u32_t arg=pblr_atoi(tkn);
        ACCESS_REG(addr)=arg;
        return CPR_NEXT;
    }
    if ((pblr_strcmp(tkn, "lw")==0)||(pblr_strcmp(tkn, "load_word")==0)) {
        if ((tkn=get_token(&cmd_buf))==NULL) return CPR_INCOMPLETE_CMD;
        u32_t addr=pblr_atoi(tkn);
        printf("$=0x%08x\n", ACCESS_REG(addr));
        return CPR_NEXT;
    }
    if (pblr_strcmp(tkn, "call")==0) {
        if ((tkn=get_token(&cmd_buf))==NULL) return CPR_INCOMPLETE_CMD;
        if (pblr_strcmp(tkn, "dram_setup")==0) {
            dram_setup();
            _show_init_result();
        } else {
            u32_t faddr=pblr_atoi(tkn);
            u32_t res=0;
            if ((tkn=get_token(&cmd_buf))==NULL) {
                // no argument
                res=((func_void_t*)faddr)();
            } else {
                u32_t arg1=pblr_atoi(tkn);
                if ((tkn=get_token(&cmd_buf))==NULL) {
                    // one argument
                    res=((func_int_t*)faddr)(arg1);
                } else {
                    // two arguments
                    u32_t arg2=pblr_atoi(tkn);
                    res=((func_int_int_t*)faddr)(arg1, arg2);
                }
            }
            printf("$=0x%08x\n", res);
        }
        return CPR_NEXT;
    }
    if (pblr_strcmp(tkn, "test")==0) {
        if ((tkn=get_token(&cmd_buf))==NULL) return CPR_INCOMPLETE_CMD;
        char *min_tkn=get_token(&cmd_buf);
        u32_t min=1;
        if (min_tkn!=NULL) min=pblr_atoi(min_tkn);

        if (pblr_strcmp(tkn, "1udelay")==0) {
            test_udelay(1, min);
        } else if (pblr_strcmp(tkn, "10udelay")==0) {
            test_udelay(10, min);
        } else if (pblr_strcmp(tkn, "100udelay")==0) {
            test_udelay(100, min);
        } else if (pblr_strcmp(tkn, "get_timer")==0) {
            test_get_timer(min);
        } else {
            return CPR_UNSUPPORT_PARAMETER;
        }
        return CPR_NEXT;
    }

    return CPR_UNKNOWN_CMD;
}

SECTION_ON_FLASH void
cli_entry(void) {
    // check if enter cli
    if ((parameters.dram_init_result==INI_RES_OK)&&(parameters.flash_init_result==INI_RES_OK)) {
        if (plr_tstc()==0) return;
        if (plr_getc()!='.') return;
    }

    // cli start
    char cmd_buf[132], *p;
    u8_t _xbuf[XBUF_SZ+12];
    u8_t *xbuf=(char*)ADDR_ALIGN(_xbuf, 16);
    pblr_puts("Preloader cli start\n]");    
    util_init();
    p=cmd_buf;
    while(1) {
        char chr=plr_getc();
        if (chr==0) continue;
            
        // echo char
        pblr_putc(chr);
        
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
            pblr_puts("exec:");
            u8_t *x=cmd_buf, cc;
            while (1) {
                if ((cc=*(x++))=='\0') break;
                printf("%c(%02d)", cc, (u32_t)cc);
            }
            pblr_putc('\n');
            
            // command parsing
            cpr_t r=cmd_parser(cmd_buf, xbuf);
            switch (r) {
                case CPR_EXIT: 
                    return ;
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
PATCH_REG(cli_entry);
