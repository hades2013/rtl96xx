#include <preloader.h>
#include <mips-generic.h>

GFUNC(cpu_init)
/* Backup return address */
							move	s6, ra

/* Clear watch registers */
							mtc0	zero, CP0_WATCHLO
							mtc0	zero, CP0_WATCHHI

/* STATUS register */
							mfc0	k0, CP0_STATUS
							li		k1, ~CP0_STATUS_IE
							and		k0, k1
							mtc0	k0, CP0_STATUS

/* (Watch Pending), SW0/1 should be cleared */
							mtc0	zero, CP0_CAUSE

/* Timer */
							mtc0	zero, CP0_COUNT
							mtc0	zero, CP0_COMPARE

#if (CONFIG_STANDALONE_UBOOT == 1)
							REBASED_CALL(rlx5281_cache_reset, UCSRAM_BASE)
							REBASED_CALL(rlx5281_dis_wb_buf_merge, UCSRAM_BASE)
#else
							UCFLASH_CALL(rlx5281_cache_reset)
							UCFLASH_CALL(rlx5281_dis_wb_buf_merge)
#endif

							jr		s6
END(cpu_init)
