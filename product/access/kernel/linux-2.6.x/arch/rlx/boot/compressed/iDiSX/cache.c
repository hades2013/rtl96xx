/*
 * Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
 * Copyright (C) 1997, 2001 Ralf Baechle (ralf@gnu.org)
 * Copyright (C) 2000, 2001, 2002, 2003 Broadcom Corporation
 * Copyright (C) 2004  Maciej W. Rozycki
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <linux/autoconf.h>
#include <linux/init.h>

//#include <asm/asm.h>
//#include <asm/bootinfo.h>   
//#include <asm/cacheops.h>
//#include <asm/cpu.h>
//#include "asm/rlxregs.h"
//#include <asm/mmu_context.h>    
//#include <asm/uaccess.h>     

//extern void _printk(const char *,...);     
/* These are probed at ld_mmu time */
static unsigned long icache_size;
static unsigned long dcache_size;

static unsigned short icache_line_size;
static unsigned short dcache_line_size;

static unsigned int icache_index_mask;
static unsigned int dcache_index_mask;

static unsigned short icache_assoc;
static unsigned short dcache_assoc;

static unsigned short icache_sets;
static unsigned short dcache_sets;

static unsigned int icache_range_cutoff;
static unsigned int dcache_range_cutoff;

#define cache16_unroll32(base,op)                                       \
        __asm__ __volatile__(                                           \
        "       .set push                                       \n"     \
        "       .set noreorder                                  \n"     \
        "       .set mips3                                      \n"     \
        "       cache %1, 0x000(%0); cache %1, 0x010(%0)        \n"     \
        "       cache %1, 0x020(%0); cache %1, 0x030(%0)        \n"     \
        "       cache %1, 0x040(%0); cache %1, 0x050(%0)        \n"     \
        "       cache %1, 0x060(%0); cache %1, 0x070(%0)        \n"     \
        "       cache %1, 0x080(%0); cache %1, 0x090(%0)        \n"     \
        "       cache %1, 0x0a0(%0); cache %1, 0x0b0(%0)        \n"     \
        "       cache %1, 0x0c0(%0); cache %1, 0x0d0(%0)        \n"     \
        "       cache %1, 0x0e0(%0); cache %1, 0x0f0(%0)        \n"     \
        "       cache %1, 0x100(%0); cache %1, 0x110(%0)        \n"     \
        "       cache %1, 0x120(%0); cache %1, 0x130(%0)        \n"     \
        "       cache %1, 0x140(%0); cache %1, 0x150(%0)        \n"     \
        "       cache %1, 0x160(%0); cache %1, 0x170(%0)        \n"     \
        "       cache %1, 0x180(%0); cache %1, 0x190(%0)        \n"     \
        "       cache %1, 0x1a0(%0); cache %1, 0x1b0(%0)        \n"     \
        "       cache %1, 0x1c0(%0); cache %1, 0x1d0(%0)        \n"     \
        "       cache %1, 0x1e0(%0); cache %1, 0x1f0(%0)        \n"     \
        "       .set pop                                        \n"     \
                :                                                       \
                : "r" (base),                                           \
                  "i" (op));

#define protected_cache_op(op,addr)                             \
        __asm__ __volatile__(                                   \
        "       .set    push                    \n"             \
        "       .set    noreorder               \n"             \
        "       .set    mips3                   \n"             \
        "1:     cache   %0, (%1)                \n"             \
        "2:     .set    pop                     \n"             \
        "       .section __ex_table,\"a\"       \n"             \
        "       "STR(PTR)" 1b, 2b               \n"             \
        "       .previous"                                      \
        :                                                       \
        : "i" (op), "r" (addr))


#define Index_Invalidate_I      0x00
#define Index_Writeback_Inv_D   0x01

/*
 * Writeback and invalidate the entire dcache
 */
static inline void dec_writeback_inv_dcache_all(void)
{

	unsigned char buf[icache_size];
	unsigned long addr = &buf[0];

	while (addr < dcache_line_size * dcache_sets) {
		cache16_unroll32(addr,Index_Writeback_Inv_D);
		addr += dcache_line_size;
	}
}

/*
 * Invalidate the entire icache
 */
static inline void dec_flush_icache_all(void)
{
	unsigned char buf[dcache_size];
	unsigned long addr = &buf[0];

	while (addr < icache_line_size * icache_sets)
    {
		cache16_unroll32(addr,Index_Invalidate_I);
		addr += icache_line_size;
	}
}

/*
 *  Cache set values (from the mips64 spec)
 * 0 - 64
 * 1 - 128
 * 2 - 256
 * 3 - 512
 * 4 - 1024
 * 5 - 2048
 * 6 - 4096
 * 7 - Reserved
 */

static unsigned int decode_cache_sets(unsigned int config_field)
{
	if (config_field == 7) {
		/* JDCXXX - Find a graceful way to abort. */
		return 0;
	}
	return (1<<(config_field + 6));
}

/*
 *  Cache line size values (from the mips64 spec)
 * 0 - No cache present.
 * 1 - 4 bytes
 * 2 - 8 bytes
 * 3 - 16 bytes
 * 4 - 32 bytes
 * 5 - 64 bytes
 * 6 - 128 bytes
 * 7 - Reserved
 */

static unsigned int decode_cache_line_size(unsigned int config_field)
{
	if (config_field == 0) {
		return 0;
	} else if (config_field == 7) {
		/* JDCXXX - Find a graceful way to abort. */
		return 0;
	}
	return (1<<(config_field + 1));
}

/*
 * Relevant bits of the config1 register format (from the MIPS32/MIPS64 specs)
 *
 * 24:22 Icache sets per way
 * 21:19 Icache line size
 * 18:16 Icache Associativity
 * 15:13 Dcache sets per way
 * 12:10 Dcache line size
 * 9:7   Dcache Associativity
 */

static char *way_string[] = {
	"direct mapped", "2-way", "3-way", "4-way",
	"5-way", "6-way", "7-way", "8-way",
};

#if defined(CONFIG_CPU_RLX5281) || defined(CONFIG_CPU_RLX4281)
#define __read_32bit_c0_register(source, sel)				\
({ int __res;								\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"mfc0\t%0, " #source "\n\t"			\
			: "=r" (__res));				\
	else								\
		__asm__ __volatile__(					\
			".set\tmips32\n\t"				\
			"mfc0\t%0, " #source ", " #sel "\n\t"		\
			".set\tmips0\n\t"				\
			: "=r" (__res));				\
	__res;								\
})
#else
#define __read_32bit_c0_register(source, sel)				\
({ int __res;								\
	__asm__ __volatile__(					\
		"mfc0\t%0, " #source "\n\t"			\
		: "=r" (__res));				\
	__res;								\
})
#endif

#define read_c0_config1() __read_32bit_c0_register($16, 1)

static void probe_cache_sizes(void)
{
	unsigned int config1;     

	config1 = read_c0_config1();

	icache_line_size = decode_cache_line_size((config1 >> 19) & 0x7);
	dcache_line_size = icache_line_size;      

	icache_sets = decode_cache_sets((config1 >> 22) & 0x7);

	dcache_sets = icache_sets;     
	icache_assoc = ((config1 >> 16) & 0x7) + 1;
	dcache_assoc = icache_assoc;     
	icache_size = icache_line_size * icache_sets * icache_assoc;
	dcache_size = dcache_line_size * dcache_sets * dcache_assoc;
	/* Need to remove non-index bits for index ops */
	icache_index_mask = (icache_sets - 1) * icache_line_size;
	dcache_index_mask = (dcache_sets - 1) * dcache_line_size;
	/*
	 * These are for choosing range (index ops) versus all.
	 * icache flushes all ways for each set, so drop icache_assoc.
	 * dcache flushes all ways and each setting of bit 12 for each
	 * index, so drop dcache_assoc and halve the dcache_sets.
	 */
	icache_range_cutoff = icache_sets * icache_line_size;
	dcache_range_cutoff = (dcache_sets / 2) * icache_line_size;

/*puts("Primary instruction cache:");
puthex(icache_size >> 10);
puts(",");
puts(way_string[icache_assoc - 1]);
puts(",");
puts("linesize");
puthex(icache_line_size);
puts("\r\n");
puts("Primary data cache:");
puthex(dcache_size >> 10);
puts(",");
puts(way_string[dcache_assoc - 1]);
puts(",");
puts("linesize");
puthex(dcache_line_size);
puts("\r\n");
*/
/*
	puts("Primary instruction cache %d kB, %s, linesize %d bytes.\n",
	       icache_size >> 10, way_string[icache_assoc - 1],
	       icache_line_size);
	puts("Primary data cache %d kB, %s, linesize %d bytes.\n",
	       dcache_size >> 10, way_string[dcache_assoc - 1],
	       dcache_line_size);
*/

}

void decompress_flush_caches(void){
    probe_cache_sizes();
    dec_writeback_inv_dcache_all();
    dec_flush_icache_all();
}
