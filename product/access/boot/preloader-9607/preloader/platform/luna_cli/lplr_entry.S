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
							
							lui	a2,0xbb00
							lui	v0,0x400
							ori	a2,a2,0xf4
							lui	v1,0xb800
							lw	a3,0(a2)
							ori	a1,v1,0x331c
							ori	a0,v1,0x3324
							or	a3,a3,v0
							sw	a3,0(a2)
							ori	v1,v1,0x3328
							lw	a2,0(a1)
							or	a2,a2,v0
							sw	a2,0(a1)
							lw	a1,0(a0)
							or	v0,a1,v0
							sw	v0,0(a0)
							lui	v0,0xfbff
							lw	a0,0(v1)
							ori	v0,v0,0xffff
							and	v0,a0,v0
							sw	v0,0(v1)
							
							
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
