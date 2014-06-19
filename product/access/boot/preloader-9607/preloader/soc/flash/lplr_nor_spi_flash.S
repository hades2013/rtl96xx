#include <preloader.h>
#include <mips-generic.h>

							.align 4
							.global	flash_to_sram
							.ent	flash_to_sram, 0
flash_to_sram:
/* Since there two subfunction calls here,
   backup return address. */
							move	s8, ra

							li		a0, UCSRAM_BASE
							li		a1, FLASH_BASE
							la		a2, LS_preloader_in_SRAM_sz
							REBASED_CALL(pblr_memcpy, FLASH_BASE)

/* Since code is copied from FLASH to SRAM, need to invalidate icache */
							jal		invalidate_icache_all
							nop

/* Restore return address before jump back. */
							move	ra, s8
							jr		ra
							nop

							.end	flash_to_sram
