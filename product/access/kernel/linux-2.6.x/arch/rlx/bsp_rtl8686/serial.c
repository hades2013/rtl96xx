/*
 * Copyright 2006, Realtek Semiconductor Corp.
 *
 * arch/rlx/rlxocp/serial.c
 *     RLXOCP serial port initialization
 *
 * Tony Wu (tonywu@realtek.com.tw)
 * Nov. 07, 2006
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>
#include <linux/string.h>
#include <asm/serial.h>
#include "bspchip.h"
#include "netlog.h"
#include "prom.h"

int prom_putchar(char c)
{
	unsigned int busy_cnt = 0;

#ifdef CONFIG_RTL867X_NETLOG
	netlog_emit_char(c);
#endif

	do
	{
		/* Prevent Hanging */
		if (busy_cnt++ >= 30000)
		{
			/* Reset Tx FIFO */
			REG8(_UART_FCR) = BSP_TXRST | BSP_CHAR_TRIGGER_14;
			return 0;
		}
	} while ((REG8(_UART_LSR) & BSP_LSR_THRE) == BSP_TxCHAR_AVAIL);

	/* Send Character */
	REG8(_UART_THR) = c;

	return 1;
}


void  __init prom_console_init(void)
{
	/* pin mux contrl register n.21& n.20 for enabling GPIO_SELUART1&GPIO_SELUART1_CTRL*/
	#ifdef CONFIG_USE_UART1	
	REG32(0xBB023018)|=1<<4;	
	#endif
	/* 8 bits, 1 stop bit, no parity. */
	REG8(BSP_UART0_LCR) = BSP_CHAR_LEN_8 | BSP_ONE_STOP | BSP_PARITY_DISABLE;

	/* Reset/Enable the FIFO */
	REG8(BSP_UART0_FCR) = BSP_FCR_EN | BSP_RXRST | BSP_TXRST | BSP_CHAR_TRIGGER_14;

	/* Disable All Interrupts */
	REG8(BSP_UART0_IER) = 0x00000000;

	/* Enable Divisor Latch */
	REG8(BSP_UART0_LCR) |= BSP_DLAB;

	/* Set Divisor */
	REG8(BSP_UART0_DLL) = (BSP_SYSCLK / (BSP_BAUDRATE * 16) - 1) & 0x00FF;
	REG8(BSP_UART0_DLM) = ((BSP_SYSCLK / (BSP_BAUDRATE * 16) - 1) & 0xFF00) >> 8;

	/* Disable Divisor Latch */
	REG8(BSP_UART0_LCR) &= (~BSP_DLAB);

	#ifdef CONFIG_USE_UART1	
	/* 8 bits, 1 stop bit, no parity. */
	REG8(BSP_UART1_LCR) = BSP_CHAR_LEN_8 | BSP_ONE_STOP | BSP_PARITY_DISABLE;

	/* Reset/Enable the FIFO */
	REG8(BSP_UART1_FCR) = BSP_FCR_EN | BSP_RXRST | BSP_TXRST | BSP_CHAR_TRIGGER_14;

	/* Disable All Interrupts */
	REG8(BSP_UART1_IER) = 0x00000000;

	/* Enable Divisor Latch */
	REG8(BSP_UART1_LCR) |= BSP_DLAB;

	/* Set Divisor */
	REG8(BSP_UART1_DLL) = (BSP_SYSCLK / (BSP_BAUDRATE * 16) - 1) & 0x00FF;
	REG8(BSP_UART1_DLM) = ((BSP_SYSCLK / (BSP_BAUDRATE * 16) - 1) & 0xFF00) >> 8;

	/* Disable Divisor Latch */
	REG8(BSP_UART1_LCR) &= (~BSP_DLAB);
	#endif
}

void __init bsp_serial_init(void)
{
	struct uart_port s;
	#ifdef CONFIG_USE_UART1	
	struct uart_port s2;
	#endif
	/* clear memory */
	memset(&s, 0, sizeof(s));

	s.line = 0;
	s.type = PORT_16550A;
	s.irq = _UART_IRQ;
	s.iotype = UPIO_MEM;
	s.regshift = 2;

	s.uartclk = BSP_SYSCLK - BSP_BAUDRATE * 24; 
	s.fifosize = 1;
	s.flags = UPF_SKIP_TEST /*| UPF_LOW_LATENCY*/ | UPF_SPD_CUST;
	s.membase = (unsigned char *)_UART_BASE;
	s.custom_divisor = BSP_SYSCLK / (BSP_BAUDRATE * 16) - 1;

	if (early_serial_setup(&s) != 0) {
		panic("RTL8672: bsp_serial_init failed!");
	}
	#ifdef CONFIG_USE_UART1	
	memset(&s2, 0, sizeof(s2));
	
	s2.line = 1;
	s2.type = PORT_16550A;
	s2.irq = BSP_UART0_IRQ;
	s2.iotype = UPIO_MEM;
	s2.regshift = 2;

	s2.uartclk = BSP_SYSCLK - BSP_BAUDRATE * 24; 
	s2.fifosize = 1;
	s2.flags = UPF_SKIP_TEST /*| UPF_LOW_LATENCY*/ | UPF_SPD_CUST;
	s2.membase = (unsigned char *)BSP_UART0_BASE;
	s2.custom_divisor = BSP_SYSCLK / (BSP_BAUDRATE * 16) - 1;
	if (early_serial_setup(&s2) != 0) {
		panic("RTL8672: bsp_serial_init s2 failed!");
	}
	#endif
}

