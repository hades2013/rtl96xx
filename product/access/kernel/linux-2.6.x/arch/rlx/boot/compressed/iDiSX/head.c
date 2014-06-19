/**

  Assumptions:

- The caches were just flushed by iMon. We don't need to do it again
  for addresses that haven't been used (80200000....).

- The serial port is set up by iMon for polling mode. I can just
  scribble on it to get characters.

- 
  
 */
 
/* $Id: types.h,v 1.1.1.1 2004/08/12 22:56:52 rfranz Exp $
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994, 1995, 1996, 1999 by Ralf Baechle
 * Copyright (C) 1999 Silicon Graphics, Inc.
 */
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

 
extern uint64_t _edata;
extern uint64_t _end;
extern void decompress_kernel(void);
typedef void (*KEP)(uint64_t, char **, char **, unsigned);
extern void sys_flush_caches(void);

#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))

void start(uint64_t argc, char **argv, char **envp, unsigned mem_size){
    KEP kernel_entry=(KEP)(KERNEL_ENTRY);
    uint64_t *i;
	#if 0
	unsigned int tmp = 0;
		
	tmp = READ_MEM32(0xBB0000F0);
	tmp = tmp | (0x1<<13);
	WRITE_MEM32(0xBB0000F0, tmp);

	tmp = READ_MEM32(0xBB0000FC);
	tmp = tmp | (0x1<<13);
	WRITE_MEM32(0xBB0000FC, tmp);
	
	tmp = READ_MEM32(0xBB0000D8);
	tmp = tmp | (0x1<<13);
	WRITE_MEM32(0xBB0000D8, tmp);
	#endif
    //*(char *)0xbf18000 = 'H';
   // *(char *)0xb8002100 = 'H';
    
  //  for(i=&_edata; i<&_end;++i){
	//*i=0;
   // }
    //puts("initrd at:     ");
  //  *(char *)0xb8002000 = 'H';
     
    #if 1
    decompress_kernel();
    //*(char *)0xbf180000 = 'H';
   // decompress_flush_caches();   
    #endif;

	//tmp = READ_MEM32(0xBB0000D8);
	//tmp = tmp & (~(0x1<<13));
	//WRITE_MEM32(0xBB0000D8, tmp);
	
    //_printk("Jumping to kernel_entry: %x\n",kernel_entry);
    //kernel_entry(argc,argv,envp,mem_size);
}
