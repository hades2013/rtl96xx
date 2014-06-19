
/* $Id: types.h,v 1.1.1.1 2004/08/12 22:56:52 rfranz Exp $
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994, 1995, 1996, 1999 by Ralf Baechle
 * Copyright (C) 1999 Silicon Graphics, Inc.
 */
 #if 0
#ifndef _ASM_TYPES_H
#define _ASM_TYPES_H

typedef unsigned short umode_t;

/*
 * __xx is ok: it doesn't pollute the POSIX namespace. Use these in the
 * header files exported to user space
 */

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#if (_MIPS_SZLONG == 64)

typedef __signed__ long __s64;
typedef unsigned long __u64;

#else

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
typedef __signed__ long long __s64;
typedef unsigned long long __u64;
#endif

#endif

/*
 * These aren't exported outside the kernel to avoid name space clashes
 */
#ifdef __KERNEL__

typedef __signed char s8;
typedef unsigned char u8;

typedef __signed short s16;
typedef unsigned short u16;

typedef __signed int s32;
typedef unsigned int u32;

#if (_MIPS_SZLONG == 64)

typedef __signed__ long s64;
typedef unsigned long u64;

#else

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
typedef __signed__ long long s64;
typedef unsigned long long u64;
#endif

#endif

#define BITS_PER_LONG _MIPS_SZLONG

typedef unsigned long dma_addr_t;

#endif /* __KERNEL__ */

#endif /* _ASM_TYPES_H */

/*############################################################################*/

#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H

#ifdef	__KERNEL__
/*#include <linux/config.h>*/
#endif

/*#include <linux/posix_types.h>*/
/*#include <asm/types.h>*/

/* bsd */
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;

/* sysv */
typedef unsigned char		unchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;

#ifndef __BIT_TYPES_DEFINED__
#define __BIT_TYPES_DEFINED__

typedef		__u8		u_int8_t;
typedef		__s8		int8_t;
typedef		__u16		u_int16_t;
typedef		__s16		int16_t;
typedef		__u32		u_int32_t;
typedef		__s32		int32_t;

#endif /* !(__BIT_TYPES_DEFINED__) */

typedef		__u8		uint8_t;
typedef		__u16		uint16_t;
typedef		__u32		uint32_t;

typedef		__u64		uint64_t;
typedef		__u64		u_int64_t;
typedef		__s64		int64_t;


/*
 * Below are truly Linux-specific types that should never collide with
 * any application/library that wants linux/types.h.
 */

#endif /* _LINUX_TYPES_H */

/*############################################################################*/

#ifndef __OCTEON_SERIAL_H__
#define __OCTEON_SERIAL_H__

#define  OCTEON_MIO_UARTX_RBR(offset)                 (0x8001180000000800ull+(offset*1024))
#define  OCTEON_MIO_UARTX_THR(offset)                 (0x8001180000000840ull+(offset*1024))
#define  OCTEON_MIO_UARTX_DLL(offset)                 (0x8001180000000880ull+(offset*1024))
#define  OCTEON_MIO_UARTX_IER(offset)                 (0x8001180000000808ull+(offset*1024))
#define  OCTEON_MIO_UARTX_DLH(offset)                 (0x8001180000000888ull+(offset*1024))
#define  OCTEON_MIO_UARTX_IIR(offset)                 (0x8001180000000810ull+(offset*1024))
#define  OCTEON_MIO_UARTX_FCR(offset)                 (0x8001180000000850ull+(offset*1024))
#define  OCTEON_MIO_UARTX_LCR(offset)                 (0x8001180000000818ull+(offset*1024))
#define  OCTEON_MIO_UARTX_MCR(offset)                 (0x8001180000000820ull+(offset*1024))
#define  OCTEON_MIO_UARTX_LSR(offset)                 (0x8001180000000828ull+(offset*1024))
#define  OCTEON_MIO_UARTX_MSR(offset)                 (0x8001180000000830ull+(offset*1024))
#define  OCTEON_MIO_UARTX_SCR(offset)                 (0x8001180000000838ull+(offset*1024))
#define  OCTEON_MIO_UARTX_SRR(offset)                 (0x8001180000000A10ull+(offset*1024))

#define OCTEON_UART_NUM_PORTS     2
#define OCTEON_UART_TX_FIFO_SIZE  16
#define OCTEON_UART_RX_FIFO_SIZE  16

#endif /* __OCTEON_SERIAL_H__ */

/*############################################################################*/

#define uart_index (0)

#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN 4321
#endif

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#endif

#ifndef __BYTE_ORDER
#define __BYTE_ORDER  __BIG_ENDIAN
#endif

typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 56;      /**< Reserved */
        uint64_t ferr                    : 1;       /**< Error in Receiver FIFO bit */
        uint64_t temt                    : 1;       /**< Transmitter Empty bit */
        uint64_t thre                    : 1;       /**< Transmitter Holding Register Empty bit */
        uint64_t bi                      : 1;       /**< Break Interrupt bit */
        uint64_t fe                      : 1;       /**< Framing Error bit */
        uint64_t pe                      : 1;       /**< Parity Error bit */
        uint64_t oe                      : 1;       /**< Overrun Error bit */
        uint64_t dr                      : 1;       /**< Data Ready bit */
#else
        uint64_t dr                      : 1;
        uint64_t oe                      : 1;
        uint64_t pe                      : 1;
        uint64_t fe                      : 1;
        uint64_t bi                      : 1;
        uint64_t thre                    : 1;
        uint64_t temt                    : 1;
        uint64_t ferr                    : 1;
        uint64_t reserved                : 56;
#endif
    } s;
} cvmx_uart_lsr_t;


uint64_t octeon_read64(uint64_t csr_addr)
{

    /* volatiles on variables seems to be needed.... */
    volatile uint32_t val_low;
    volatile uint32_t val_high;

    volatile uint32_t addr_low  = csr_addr & 0xffffffff;
    volatile uint32_t addr_high = csr_addr  >> 32;

    asm volatile (
                  #if  0
                  "  .set mips64                       \n"
                  #endif /* #if 0 */
                  "  .set noreorder                    \n"
                  /* Standard twin 32 bit -> 64 bit construction */
                  "  dsll  %[addrh], 32                 \n"
                  "  dsll  %[addrl], 32          \n"
                  "  dsrl  %[addrl], 32          \n"
                  "  daddu %[addrh], %[addrh], %[addrl]   \n"
                  /* Combined value is in addrh */
                  "  ld    %[valh], 0(%[addrh])   \n"
                  "  dadd   %[vall], %[valh], $0   \n"
                  "  dsrl  %[valh], 32                 \n"
                  "  dsll  %[vall], 32          \n"
                  "  dsrl  %[vall], 32          \n"
                  "  .set reorder                      \n"
                  :[valh] "=&r" (val_high), [vall] "=&r" (val_low) : [addrh] "r" (addr_high), [addrl] "r" (addr_low): "memory");

    return(((uint64_t)val_high << 32) | val_low);

}



void octeon_write64(uint64_t csr_addr, uint64_t val)
{

    volatile uint32_t val_low  = val & 0xffffffff;
    volatile uint32_t val_high = val  >> 32;

    volatile uint32_t addr_low  = csr_addr & 0xffffffff;
    volatile uint32_t addr_high = csr_addr  >> 32;

    
    /* Using a 'dla tmp, 0xffffffff' to and the low variables with causes the compiler
    ** to use the v0 register, which u-boot uses internally
    */

    asm volatile (
      #if  0
      "  .set mips64                       \n"
      #endif /* #if 0 */
      "  .set noreorder                    \n"
      /* Standard twin 32 bit -> 64 bit construction */
      "  dsll  %[valh], 32                 \n"
      "  dsll  %[vall], 32          \n"
      "  dsrl  %[vall], 32          \n"
      "  daddu %[valh], %[valh], %[vall]   \n"
      /* Combined value is in valh */
      /* Standard twin 32 bit -> 64 bit construction */
      "  dsll  %[addrh], 32                 \n"
      "  dsll  %[addrl], 32          \n"
      "  dsrl  %[addrl], 32          \n"
      "  daddu %[addrh], %[addrh], %[addrl]   \n"
      /* Combined value is in addrh */
      "  sd %[valh], 0(%[addrh])   \n"
      "  .set reorder                      \n"
      : : [valh] "r" (val_high), [vall] "r" (val_low), [addrh] "r" (addr_high), [addrl] "r" (addr_low): "memory");


}


void serial_putc(char ch){
	
    cvmx_uart_lsr_t lsrval;

    /* Spin until there is room */
    do
    {
        lsrval.u64 = octeon_read64(OCTEON_MIO_UARTX_LSR(uart_index));
    }
    while (lsrval.s.thre == 0);

    /* Write the byte */
    octeon_write64(OCTEON_MIO_UARTX_THR(uart_index), ch);
    
}
#endif
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/serial_reg.h>
#include <linux/serial.h>
#include <linux/serial_core.h>

//#include <asm/mach-onu/onu.h>
//#include <asm/mach-onu/onu_reg.h>
//#include <asm/mach-onu/onu_irq.h>

//#include <prom.h>
//#include <platform.h>
#define EARLY_DEBUG 0

#define BSP_UART0_BASE	0xB8002000

#define UART_LSR		0x014
#define UART_LSR_THRE	0x20

#define UART_TX			0x000

static unsigned char serial_in(int offset)
{
        return  *(volatile unsigned char *)(BSP_UART0_BASE + offset) ;
}

static void serial_out(int offset, int value)
{
        *(volatile unsigned char *)(BSP_UART0_BASE + offset) =value;
}


int putPromChar(char c)
{

        while ((serial_in(UART_LSR) & UART_LSR_THRE) == 0)
                ;
        serial_out(UART_TX, c);

        return 1;
}


void prom_putchar(char c)
{
        putPromChar(c);
}



void serial_putc(char c)
{
    unsigned int busy_cnt = 0;
    
    putPromChar(c);
    #if 0
    do
    {
        /* Prevent Hanging */
        if (busy_cnt++ >= 30000)
        {
            /* Reset Tx FIFO */
            REG8(UART0_FCR) = TXRST | CHAR_TRIGGER_14;
            return 0;
        }
    } while ((REG8(UART0_LSR) & LSR_THRE) == TxCHAR_AVAIL);

    /* Send Character */
    REG8(UART0_THR) = c;
    #endif
    return 1;
}

