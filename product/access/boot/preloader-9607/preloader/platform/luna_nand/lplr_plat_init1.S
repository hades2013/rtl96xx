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

							jal		cpu_init
							nop

#include "lplr_sram.H"

							jr		s7
							nop
							.end	platform_init_phase1
