#include <preloader.h>
#include "swp_util.h"

SECTION_ON_FLASH int
pblr_strcmp(const char *a, const char *b) {
    while (*a!='\0') {
        if (*a!=*b) break;
        a++;
        b++;
    } 
    return (u8_t)*a - (u8_t)*b;
}
SECTION_ON_FLASH void
pblr_strncpy(char *dst, const char *src, unsigned int n) {
    while ((*src!='\0')&&(n>0)) {
        *(dst++)=*(src++);
        n--;
    }
    *dst='\0';
}
inline static int
_ascii_value(char c, int base) {
    if ((c<='9') && (c>='0')) return c-'0';
    c&=~32;
    return ((base==16)&&(c<='F')&&(c>='A'))?(c-'A'+10):-1;
}
SECTION_ON_FLASH u32_t
pblr_atoi(const char *v) {
    int base=10, d, r=0;
    if (*v=='\0') return 0;
    if ((v[0]=='0') && ((v[1]&~32)=='X')) {
        v+=2;
        base=16;
    }
    while ((d=_ascii_value(*(v++), base))>=0) {
        r *= base;
        r += d;
    }
    return r;
}
extern const char *__lower_digits;
SECTION_ON_FLASH static char *
_pblr_tostr(char *buf, u32_t v, int base, int ndigit) {
    u32_t q, r;
    q=v/base;
    r=v%base;
    if ((v>=base)||(ndigit>0)) {
        buf=_pblr_tostr(buf, q, base, ndigit-1);
    }
    *(buf++)=__lower_digits[r];
    return buf;
}

SECTION_ON_FLASH char *
pblr_tostr(char *buf, u32_t v, int base, int ndigit) {
    if (base<0) {
        base=-base;
        if (v&0x80000000) {
            *(buf++)='-';
            v=-v;
        }
    }
    if (base==16) {
        *(buf++)='0';
        *(buf++)='x';
    } else if (base==8) {
        *(buf++)='0';
    }
    buf=_pblr_tostr(buf, v, base, ndigit-1);
    *buf='\0';
    return buf;
}
SECTION_ON_FLASH const char *
init_result_string(init_result_t res) {
    switch (res) {
        case INI_RES_OK: return "success";
        case INI_RES_UNKNOWN_MODE: return "unknown-mode";
        case INI_RES_UNKNOWN_MODEL: return "unknown-model";
        case INI_RES_FAIL: return "failure";
        case INI_RES_TEST_FAIL: return "test-failure";
        case INI_RES_UNINIT: return "uninitialized";
        case INI_RES_DRAM_ZQ_CALI_FAIL: return "DRAM ZQ calibration failed";
        case INI_RES_DRAM_SW_CALI_FAIL: return "DRAM software calibration failed";
        default: return "unknown-result";
    }
}

static inline u8_t 
_disp_chr(const u8_t chr) {
    return ((chr<32)||(chr>=128))?'.':chr;
}
SECTION_ON_FLASH void
memory_dump(const u8_t *buff, u32_t size) {
    u32_t i, p;
    u32_t bufadd=(u32_t)buff;
    if (size>4096) size=4096;
    u32_t bufend=bufadd+size;
    u32_t curr=bufadd&~0xf;

    pblr_puts("          00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F    0123456789ABCDEF\n");
    while (1) {
        printf("%08x  ", curr);
        for (i=0, p=curr; i<16;++i) {
            if ((p<bufadd)||(p>=bufend)) {
                pblr_puts("   ");
            } else {
                printf("%02x ", *(const u8_t *)p);
            }
            ++p;
        }
        pblr_puts("   ");
        for (i=0, p=curr; i<16;++i) {
            if ((p<bufadd)||(p>=bufend)) {
                pblr_puts(" ");
            } else {
                printf("%c", _disp_chr(*(const u8_t *)p));
            }
            ++p;
        }
        curr+=16;
        pblr_puts("\n");
        if (curr>=bufend) return;
    }
}

SECTION_ON_FLASH char *
cli_string_lookup(const str2int_entry_t *list, u32_t v, char *buf) {
    if (list==NULL) return NULL;
    while (list->value_str!=NULL) {
        if (list->value==v) {
            const char *p=list->value_str;
            do *(buf++)=*p;
            while(*(p++)!='\0');
            return buf;
        }
        ++list;
    }
    return NULL;
}
SECTION_ON_FLASH int 
cli_int_lookup(const str2int_entry_t *list, const char *value_str, u32_t *value) {
    if (list==NULL) return -1;
    while (list->value_str!=NULL) {
        if (pblr_strcmp(value_str, list->value_str)==0) {
            *value=list->value;
            return 0;
        }
        ++list;
    }
    return -1;
}
// -------------the following function should be initialized
static u32_t _accumulate_time_ms=0;
static u32_t _cpu_mhz=0;
SECTION_ON_FLASH int 
util_init(void) {
    _accumulate_time_ms = 0;
    _cpu_mhz=0;
    return 0;
}

u32_t 
pblr_get_cpu_mhz() {
    if (_cpu_mhz==0) {
        _cpu_mhz=plr_query_freq(PLL_DEV_CPU);
    }
    return _cpu_mhz;
}

#define CYCLES_PER_MS (pblr_get_cpu_mhz() * 1000)
u32_t
pblr_get_timer(u32_t base) {
    u32_t now = OTTO_CPU_CYCLE();
    u32_t cycle_count_per_ms = CYCLES_PER_MS;
    u32_t d = now - _accumulate_time_ms * cycle_count_per_ms;
    u32_t c = d / cycle_count_per_ms;
    
    _accumulate_time_ms += c;
    return _accumulate_time_ms - base;
}

