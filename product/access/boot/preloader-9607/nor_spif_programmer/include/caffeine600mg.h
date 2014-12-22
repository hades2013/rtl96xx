#ifndef FLASH_WRITER_H
#define FLASH_WRITER_H

#include <soc.h>
#include <pblr.h>

#define alloca(sz) __builtin_alloca(sz)

typedef unsigned int   uint32_t;
typedef          int    int32_t;
typedef unsigned short uint16_t;
typedef          short  int16_t;
typedef unsigned char   uint8_t;
typedef          char    int8_t;

uint32_t pblr_soc_printf(const char *fmt, ...);
#define printf(x, ...) pblr_soc_printf(x, ##__VA_ARGS__)

void pblr_udelay(u32_t);
#define udelay(x) pblr_udelay(x)

void pblr_puts (const char *s);
void pblr_putc(const char c);
uint32_t pll_query_freq(uint32_t dev);

void otto_NS16550_init(int);
void otto_NS16550_putc(char);
char otto_NS16550_getc(void);
int  otto_NS16550_tstc(void);
void assign_uart_fp(void);
int is_frame_error(void);

uint32_t xmodem_2_sf(uint32_t);

void flash_init(void);
int32_t flash_unit_erase(const uint32_t, uint32_t);
int32_t flash_unit_write(const uint32_t, uint32_t, const uint32_t, const void*);

#define SWBREAK() do {               \
		__asm__ __volatile__ ("sdbbp;"); \
	} while(0)

#define PLL_DEV_CPU 0
#define PLL_DEV_LX  1

#define BAUDRATE 115200

#endif
