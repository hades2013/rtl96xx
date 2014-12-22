#include <preloader.h>
#include <mips-generic.h>

							.text
							.align 4
							.set	noreorder
							.global	platform_init_phase1
							.ent	platform_init_phase1
platform_init_phase1:
/* Spill ra to s7, so others could invoke function.
   *NOTE* s7 is a callee-save register,
   make sure your assembly code obeys this. */
							move	s7, ra

							REBASED_CALL(cpu_init, UCFLASH_BASE)

							/* Bypass SRAM BIST (Patch obtained from YUMC)
							   NOTE: This is a patch for SRAM BIST issue.
							   SRAM BIST is only enabled by bonding.
							   The patch should be removed after SRAM BIST is disabled by ponding for all ICs */ 
							/* delay 350us */
							li      t0, 3500
1:
							addiu   t0, t0, -1
							bne     zero, t0, 1b
							nop

							li      t0, 0xBB000078
							li      t1, 0x100
							lw      t2, 0(t0)
							and     t2, t2, t1
							bne     zero, t2, 1f
							nop

							#Disable PON BIST
							li      t0, 0xBB00007C
							li      t1, 0x3
							sw      t1, 0(t0)
							nop
							1: /* skip */

#include "lplr_sram.H"

							jr		s7
							nop
							.end	platform_init_phase1
