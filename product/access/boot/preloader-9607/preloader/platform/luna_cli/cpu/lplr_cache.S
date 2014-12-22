#define __ASM__
#include "rlx5281.h"
#include "rlx5281_cache_op.h"

/*******************************************************************************
*
* rlx5281_cache_reset - low level initialisation of the primary caches
*
* Invalidate D/I-caches and turn off I/DMEM(0,1)
*
* RETURNS: N/A
*
*/

	.text
        .globl rlx5281_cache_reset
	.ent rlx5281_cache_reset
rlx5281_cache_reset:
	# Note: only one function trigger is allowed every transaction 0->1
	# I-Cache Inv
	mfc0	t0, CCTL_REG
	li	t1, (1<<1)
	not	t1, t1
	and	t0, t0, t1
	mtc0	t0, CCTL_REG
	li	t1, (1<<1)
	or	t0, t0, t1
	mtc0	t1, CCTL_REG
	nop
	nop
	nop
	nop

	# D-Cache Inv
	mfc0	t0, CCTL_REG
	li	t1, 0x1
	not	t1, t1
	and	t0, t0, t1
	mtc0	t0, CCTL_REG
	li	t1, 0x1
	or	t0, t0, t1
	mtc0	t1, CCTL_REG
	nop
	nop
	nop
	nop

	# L2 Cache off
	mfc0	t0, CCTL_REG, 1
	li	t1, (1<<25)
	not	t1, t1
	and	t0, t0, t1
	mtc0	t0, CCTL_REG, 1
	li	t1, (1<<25)
	or	t0, t0, t1
	mtc0	t1, CCTL_REG, 1
	nop
	nop
	nop
	nop

	#Clear write buffer
	li	t0, 0xa0000000
	lw	t1, 0(t0)
	nop

	jr		ra
	nop

	.end	rlx5281_cache_reset

	/*
	 * void rlx5281_write_back_invalidate_dcache_cctl(void)
	 * write back and invalidate all D-Cache
	 */
        .globl rlx5281_write_back_invalidate_dcache_cctl
	.ent  rlx5281_write_back_invalidate_dcache_cctl
rlx5281_write_back_invalidate_dcache_cctl:
	mfc0 	t8, CCTL_REG 
	li      t9, (1<<9)
	not	t9, t9 
	and	t8, t8, t9 
	mtc0 	t8, CCTL_REG 
	nop		 
	li	t9, (1<<9)
	or	t8, t8, t9 
	mtc0 	t8, CCTL_REG
	nop
	nop
	#li	t8, 0xa0000000
	#lw	t9, 0(t8)
	nop
	jr ra
	nop
	.end rlx5281_write_back_invalidate_dcache_cctl

	/* 
	 * void invalidate_icache(void)
	 * Invalidate I-Cache
	 */
	.text
        .globl invalidate_icache_all
	.ent invalidate_icache_all
invalidate_icache_all:
	mfc0 	t8, CCTL_REG 
	li      t9, (1<<1)
	not	t9, t9 
	and	t8, t8, t9 
	mtc0 	t8, CCTL_REG 
	nop		 
	li	t9, (1<<1)
	or	t8, t8, t9 
	mtc0 	t8, CCTL_REG
	nop
	nop
	jr ra
	nop
	.end invalidate_icache_all
			
/*******************************************************************************
*
* rlx5281_dis_wb_buf_merge - Disable RLX5281 write back buffer merge.
*
* RETURNS: N/A
*
*/
	.globl	rlx5281_dis_wb_buf_merge
        .text
	.ent	rlx5281_dis_wb_buf_merge
rlx5281_dis_wb_buf_merge:
	/* Turn off write buffer merge */
	mfc0	t0, CCTL_REG
	li	t1, 0xFFCFFFFF
	and	t0, t0, t1
	mtc0    t0, CCTL_REG
	nop
	li      t1, (1<<21) # bit 21 of COP0 reg 20, select 0
	or	t0, t0, t1
	mtc0    t0, CCTL_REG
	nop
	jr	ra
	nop
	.end	rlx5281_dis_wb_buf_merge



#if 0 /* un-used function */
	/*
	 * void rlx5281_invalidate_dcache_range(unsigned int base_addr, unsigned int end_addr)
	 * invalidate D-Cache from base_addr to end_addr
	 * a0: base_addr
	 * a1: end_addr
	 */
	.text
        .globl rlx5281_invalidate_dcache_range
	.ent rlx5281_invalidate_dcache_range
rlx5281_invalidate_dcache_range:
1:
	CACHE	CACHE_OP_D_INV, 0(a0)
	add	a0, a0, DCACHE_LINE_SIZE
	sltu	t0, a0, a1
	bne	zero, t0, 1b	
	nop

	jr	ra
	nop

	.end rlx5281_invalidate_dcache_range


	/*
	 * void rlx5281_invalidate_icache_range(unsigned int base_addr, unsigned int end_addr)
	 * invalidate I-Cache from base_addr to end_addr
	 * a0: base_addr
	 * a1: end_addr
	 */
	.text
        .globl rlx5281_invalidate_icache_range
	.ent rlx5281_invalidate_icache_range
rlx5281_invalidate_icache_range:
1:
	CACHE	CACHE_OP_I_INV, 0(a0)
	add	a0, a0, ICACHE_LINE_SIZE
	sltu	t0, a0, a1
	bne	zero, t0, 1b	
	nop

	jr	ra
	nop

	.end rlx5281_invalidate_icache_range


	/*
	 * void rlx5281_write_back_dcache_range(unsigned int base_addr, unsigned int end_addr)
	 * Write-back D-Cache from base_addr to end_addr
	 * a0: base_addr
	 * a1: end_addr
	 */
	.text
        .globl rlx5281_write_back_dcache_range
	.ent rlx5281_write_back_dcache_range
rlx5281_write_back_dcache_range:
1:
	CACHE	CACHE_OP_D_WB, 0(a0)
	add	a0, a0, DCACHE_LINE_SIZE
	sltu	t0, a0, a1
	bne	zero, t0, 1b	
	nop

	jr	ra
	nop

	.end rlx5281_write_back_dcache_range


	/*
	 * void rlx5281_write_back_inv_dcache_range(unsigned int base_addr, unsigned int end_addr)
	 * Write-back and invalidate D-Cache from base_addr to end_addr
	 * a0: base_addr
	 * a1: end_addr
	 */
	.text
        .globl rlx5281_write_back_inv_dcache_range
	.ent rlx5281_write_back_inv_dcache_range
rlx5281_write_back_inv_dcache_range:
1:
	CACHE	CACHE_OP_D_WB_INV, 0(a0)
	add	a0, a0, DCACHE_LINE_SIZE
	sltu	t0, a0, a1
	bne	zero, t0, 1b	
	nop

	jr	ra
	nop
	.end rlx5281_write_back_inv_dcache_range

	/* 
	 * void rlx5281_invalidate_dcache_cctl(void)
	 * Invalidate I-Cache
	 */
	.text
        .globl rlx5281_invalidate_dcache_cctl
	.ent rlx5281_invalidate_dcache_cctl
rlx5281_invalidate_dcache_cctl:
	mfc0 	t8, CCTL_REG 
	li      t9, 1
	not	t9, t9 
	and	t8, t8, t9 
	mtc0 	t8, CCTL_REG 
	nop		 
	li	t9, 1
	or	t8, t8, t9 
	mtc0 	t8, CCTL_REG
	nop
	nop
	jr ra
	nop
	.end rlx5281_invalidate_dcache_cctl

	/* 
	 * void rlx5281_write_back_dcache_cctl(void)
	 * write back all D-Cache
	 */
        .globl rlx5281_write_back_dcache_cctl
	.ent rlx5281_write_back_dcache_cctl
rlx5281_write_back_dcache_cctl:
	mfc0	t0, CCTL_REG
        li	t1, (1<<8) 		# bit 8 of COP0 reg 20
	not	t1, t1
	and	t0, t0,	t1
        mtc0 	t0, CCTL_REG
        li	t1, (1<<8) 			# bit 8 of COP0 reg 20
	or	t0, t0,	t1
        mtc0 	t0, CCTL_REG
        nop
        nop
        jr 	ra
        nop
	.end rlx5281_write_back_dcache_cctl

	/* 
	 * void rlx5281_enable_L2Cache(void)
	 * Enable L2 Cache
	 */
        .ent rlx5281_enable_L2Cache
        .globl rlx5281_enable_L2Cache
rlx5281_enable_L2Cache:
	mfc0	t0, CCTL_REG, 1
        li	t1, (1<<24) 		# bit 24 of COP0 reg 20, sel = 1
	not	t1, t1
	and	t0, t0,	t1
        mtc0 	t0, CCTL_REG, 1
        li	t1, (1<<24) 		# bit 24 of COP0 reg 20, sel = 1
	or	t0, t0,	t1
        mtc0 	t0, CCTL_REG, 1
        nop
        nop
        jr 	ra
        nop
        .end rlx5281_enable_L2Cache


	/* 
	 * void rlx5281_disable_L2Cache(void)
	 * Disable L2 Cache
	 */
        .ent rlx5281_disable_L2Cache
        .globl rlx5281_disable_L2Cache
rlx5281_disable_L2Cache:
	mfc0	t0, CCTL_REG, 1
        li	t1, (1<<25) 		# bit 25 of COP0 reg 20, sel = 1
	not	t1, t1
	and	t0, t0,	t1
        mtc0 	t0, CCTL_REG, 1
        li	t1, (1<<25) 		# bit 25 of COP0 reg 20, sel = 1
	or	t0, t0,	t1
        mtc0 	t0, CCTL_REG, 1
        nop
        nop
        jr 	ra
        nop
        .end rlx5281_disable_L2Cache
#endif 
