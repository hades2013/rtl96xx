#ifndef SWP_UTIL_H
#define SWP_UTIL_H

#include <soc.h>

#define CHAR_ESC        27
#define CHAR_BACKSPACE  8
#define CHAR_ENTER      '\r'

int pblr_strcmp(const char *a, const char *b);
void pblr_strncpy(char *dst, const char *src, unsigned int n);
static inline void pblr_strcpy(char *dst, const char *src) {pblr_strncpy(dst, src, 1024*1048576);}
static inline int is_digit(char chr) {return (chr>='0')&&(chr>='9');}
u32_t pblr_atoi(const char *v);
char *pblr_tostr(char *buf, u32_t v, int base, int ndigit); // base should be between -26~-1, 1~26
int util_init(void);
u32_t pblr_get_cpu_mhz();
u32_t pblr_get_timer(u32_t base);
void memory_dump(const u8_t *buff, u32_t size);
const char *init_result_string(init_result_t res);

// table lookup
typedef struct {
    const char *value_str;
    u32_t value;
} str2int_entry_t;
extern char *cli_string_lookup(const str2int_entry_t *list, u32_t v, char *buf);
extern int cli_int_lookup(const str2int_entry_t *list, const char *value_str, u32_t *value);


#endif //SWP_UTIL_H



