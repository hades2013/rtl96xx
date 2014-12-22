#include <preloader.h>
#include <mipsregs.h>
#include <regdef.h>

/************************************************/
							.section entry.text, "ax", @progbits
							.align 4
							.set	noreorder
							.global pre_flash_info
							.ent	pre_flash_info
pre_flash_info:
							//REBASED_CALL(lpreloader_start, UCFLASH_BASE)
							UCFLASH_CALL(lpreloader_start)
							b lpreloader_start
							.end	pre_flash_info

/***************************************************
  there is a "flash_info.text" section in-between
  "pre_flash_info.text" and ".text" sections to
  store flash related parameters.
 ***************************************************/

							.text
							.align 4
							.set	noreorder
							.global	lpreloader_start
							.ent	lpreloader_start
lpreloader_start:
							REBASED_CALL(platform_init_phase1, UCFLASH_BASE);

							REBASED_CALL(flash_to_sram, FLASH_BASE);

/* Setup stack on SRAM */
							la		sp, STACK0_BASE

							REBASED_CALL(c_start_prologue, SRAM_BASE)
/***************************************************
  From now on, code runs at where it was built to,
  and C code is allowed.
 ***************************************************/
dead_loop:		b			dead_loop
							nop

							.end	lpreloader_start
