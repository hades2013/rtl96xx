#include <mips-generic.h>

  .text
  .align  4
  .set    noreorder
  .global cstart
  .ent    cstart
cstart:
/* reset CP0 CAUSE to enable CP0 COUNT */
  mtc0    zero, CP0_CAUSE

/* memory space from 0xbf003ff0 ~ 0xbf003fff are reservered for advance usages:
   0xbf003ff0: a magic number (0x05d550c0) for enabling this feature.
   0xbf003ff4: default erasing type, 0: 128KB, 1: 256KB, 2: 512KB.
   0xbf003ff8: reserved.
   0xbf003ffc: reserved. */
  la      sp, 0xBF003FE8
  j       main
  nop
  .end cstart
