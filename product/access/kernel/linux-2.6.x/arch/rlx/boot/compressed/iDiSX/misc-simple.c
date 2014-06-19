/*
 * arch/mips/zboot/common/misc-simple.c
 *
 * Misc. bootloader code for many machines.  This assumes you have are using
 * a 6xx/7xx/74xx CPU in your machine.  This assumes the chunk of memory
 * below 8MB is free.  Finally, it assumes you have a NS16550-style uart for 
 * your serial console.  If a machine meets these requirements, it can quite
 * likely use this code during boot.
 * 
 * Author: Matt Porter <mporter@mvista.com>
 * Derived from arch/ppc/boot/prep/misc.c
 *
 * Copyright 2001 MontaVista Software Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/types.h>
#include <linux/elf.h>
//#include <linux/autoconf.h>

//#include <asm/page.h>

//#include "zlib.h"

typedef unsigned long uint64_t;

char *avail_ram;
char *end_avail;
extern unsigned long _end[];
char *zimage_start;

#ifdef CONFIG_CMDLINE
#define CMDLINE CONFIG_CMDLINE
#else
#define CMDLINE ""
#endif
char cmd_preset[] = CMDLINE;
char cmd_buf[256];
char *cmd_line = cmd_buf;

/* The linker tells us where the image is.
*/
extern uint64_t __image_begin, __image_end;
extern uint64_t __ramdisk_begin, __ramdisk_end;
uint64_t initrd_size;

extern void puts(const char *);
extern void putc(const char c);
extern void puthex(unsigned long val);
extern void *memcpy(void * __dest, __const void * __src,
                           __kernel_size_t __n);
extern void gunzip(void *, int, unsigned char *, int *);
extern void udelay(long delay);
extern int tstc(void);
extern int getc(void);


void
decompress_kernel(void) 
{
       uint64_t     zimage_size;
       initrd_size = (uint64_t)(&__ramdisk_end) -
               (uint64_t)(&__ramdisk_begin);

       //puts("Decompressing...\n");
       
       /*
        * We link ourself to an arbitrary low address.  When we run, we
        * relocate outself to that address.  __image_being points to
        * the part of the image where the zImage is. -- Tom
        */
       zimage_start = (char *)(uint64_t)(&__image_begin);
       zimage_size = (uint64_t)(&__image_end) -
                       (uint64_t)(&__image_begin);

       /*
        * The zImage and initrd will be between start and _end, so they've
        * already been moved once.  We're good to go now. -- Tom
        */
        
       //puts("source from:     "); puthex((uint64_t)zimage_start);
       //puts(" to "); puthex((uint64_t)(zimage_size+zimage_start));
       //puts("\n");
       
       if ( initrd_size ) {
              // puts("initrd at:     ");
              // puthex((uint64_t)(&__ramdisk_begin));
              // puts(" "); puthex((uint64_t)(&__ramdisk_end));puts("\n");
       }

   // #if 0
       avail_ram = (char *)AVAIL_RAM_START;
       end_avail = (char *)AVAIL_RAM_END;
  //  #else
 //      avail_ram = (char *)CONFIG_AVAIL_RAM_START;
   //    end_avail = (char *)CONFIG_AVAIL_RAM_END;
  //  #endif
       /* Display standard Linux/MIPS boot prompt for kernel args */
       //puts("Uncompressing at load address ");       
    #if 0 
       puthex(LOADADDR);
       puts("\n");
       /* I don't like this hard coded gunzip size (fixme) */
       gunzip((void *)LOADADDR, 0x1500000, zimage_start, &zimage_size);
    #else
       
       //puthex(LOADADDR);
       
       #if  0
       puts("\n");
       #endif /* #if 0 */
       
       /* I don't like this hard coded gunzip size (fixme) */
       gunzip((void *)LOADADDR, 0x1500000, zimage_start, &zimage_size);
    #endif
}
