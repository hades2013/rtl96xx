#include <preloader.h>
#include "../../../u-boot-9607/include/configs/autoconf.h"


#define LCR_BKSE 0x80                /* Bank select enable */
#define LCRVAL   LCR_8N1             /* 8 data, 1 stop, no parity */
#define MCRVAL   (MCR_DTR | MCR_RTS) /* RTS/DTR */
#define FCRVAL   0xC1                /* Clear & enable FIFOs */
#define LCR_8N1  0x03

#define MCR_DTR  0x01
#define MCR_RTS  0x02
#ifdef CONFIG_UART1
#define UART_BASE_ADDR (0xb8002100)
#else
#define UART_BASE_ADDR (0xb8002000)
#endif
void otto_NS16550_init(int baud_divisor);
void otto_NS16550_putc(char c);
char otto_NS16550_getc(void);
int  otto_NS16550_tstc(void);

/* dummy function for baudrate_divisor == 0, i.e., no UART. */
#if 0
    // this is ordinary implementation
    static void dummyv_i(int baud_divisor __attribute__((unused))) {return;}
    static void dummyv_c(char c __attribute__((unused))) {return;}
    static char dummyc_v(void) {return 0;}
    static int dummyi_v(void) {return 0;}
#else
    // this is a tricky implementation
    static int dummy_all(void) {return 0;}
    #define dummyv_i ((fpv_s32_t*)(dummy_all))
    #define dummyv_c ((fpv_s8_t*)(dummy_all))
    #define dummyc_v ((fps8_v*)(dummy_all))
    #define dummyi_v ((fps32_v*)(dummy_all))
#endif

static void assign_uart_fp(void){
	parameters._uart_init = otto_NS16550_init;
	parameters._uart_putc = otto_NS16550_putc;
	parameters._uart_getc = otto_NS16550_getc;
	parameters._uart_tstc = otto_NS16550_tstc;
}

void console_init(void) {
	if (parameters.soc.peri_info.baudrate_divisor == 0x0) {
		//parameters._uart_init = dummyv_i;
		parameters._uart_init = ((fpv_s32_t*)assign_uart_fp);
		parameters._uart_putc = dummyv_c;
		parameters._uart_getc = dummyc_v;
		parameters._uart_tstc = dummyi_v;
	} else {
		otto_NS16550_init(parameters.soc.peri_info.baudrate_divisor);
		assign_uart_fp();
	}
	return;
}

void otto_NS16550_init(int baud_divisor) {
	*((volatile u32_t *)(UART_BASE_ADDR+0x4))  = 0;
	*((volatile u32_t *)(UART_BASE_ADDR+0xc))  = ((LCR_BKSE | LCRVAL) << 24);
	*((volatile u32_t *)(UART_BASE_ADDR))      = (baud_divisor)      << 24;
	*((volatile u32_t *)(UART_BASE_ADDR+0x4))  = (baud_divisor >> 8) << 24;
	*((volatile u32_t *)(UART_BASE_ADDR+0xc))  = ((LCRVAL) << 24);
	*((volatile u32_t *)(UART_BASE_ADDR+0x10)) = ((MCRVAL) << 24);
	*((volatile u32_t *)(UART_BASE_ADDR+0x8))  = ((FCRVAL) << 24);
	*((volatile u32_t *)(UART_BASE_ADDR+0x10)) = ((MCRVAL) << 24) | (0x40000000);
	return;
}

int otto_NS16550_tstc(void) {
	/* 0x8F = 1000 1111. I.e., Check RFE, FE, PE, OE and DR bits. Only DR should be 1.
	   Otherwise, there is an error. */
	return ((*((volatile unsigned int *)(UART_BASE_ADDR+0x14)) & 0x8F000000) == 0x01000000);
}

char otto_NS16550_getc(void) {
	/* 0x8F = 1000 1111. I.e., Check RFE, FE, PE, OE and DR bits. Only DR should be 1.
	   Otherwise, there is an error. */
	while ((*((volatile unsigned int *)(UART_BASE_ADDR+0x14)) & 0x8F000000) != 0x01000000);
	return 	*((volatile unsigned char *)UART_BASE_ADDR);
}

void otto_NS16550_putc(char c) {
	while ((*((volatile unsigned int *)(UART_BASE_ADDR+0x14)) & 0x20000000) == 0);
	*((volatile unsigned char *)UART_BASE_ADDR) = c;
	return;
}
