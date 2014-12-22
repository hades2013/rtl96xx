#include <caffeine600mg.h>

#define UART_BASE_ADDR (0xb8002000)

int is_frame_error(void) {
	return (*((volatile unsigned int *)(UART_BASE_ADDR+0x14)) & (1 << 27));
}

void assign_uart_fp(void) {
	parameters._uart_init = otto_NS16550_init;
	parameters._uart_putc = otto_NS16550_putc;
	parameters._uart_getc = otto_NS16550_getc;
	parameters._uart_tstc = otto_NS16550_tstc;
	return;
}
