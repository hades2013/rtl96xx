#include <common.h>
#include <soc.h>
#include <pblr.h>
#include <asm/otto_pll.h>

DECLARE_GLOBAL_DATA_PTR;

static int calc_divisor (void) {
#define MODE_X_DIV 16
	return ((CONFIG_SYS_HZ + (gd->baudrate * (MODE_X_DIV/2))) /
	        (MODE_X_DIV * gd->baudrate)) + (CONFIG_UART_DIVISOR_MOD);
}

int serial_init (void) {
	if (gd->baudrate == 0x0) {
	} else {
		serial_setbrg();
	}
	return (0);
}

void serial_putc(const char c) {
	if (c == '\n')
		parameters._uart_putc('\r');

	parameters._uart_putc(c);
	return;
}

void serial_putc_raw(const char c) {
	parameters._uart_putc(c);
	return;
}

void serial_puts(const char *s) {
	while (*s) {
		serial_putc(*s++);
	}
	return;
}

int serial_getc() {
	return parameters._uart_getc();
}

int serial_tstc() {
	return parameters._uart_tstc();
}

void serial_setbrg() {
	parameters._uart_init(calc_divisor());
	return;
}
