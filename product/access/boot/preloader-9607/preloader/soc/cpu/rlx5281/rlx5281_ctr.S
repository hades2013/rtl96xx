#define __ASM__
#include "rlx5281.h"
#include "rlx5281_cache_op.h"

/*******************************************************************************
*
* en_wb_buf_merge - Enable RLX5281 write back buffer merge.
*
* RETURNS: N/A
*
*/
	.globl	en_wb_buf_merge
        .text
	.ent	en_wb_buf_merge
en_wb_buf_merge:
	/* Turn on write buffer write buffer merge */
	mfc0	t0, CCTL_REG
	li	t1, 0xFFCFFFFF
	and	t0, t0, t1
	mtc0    t0, CCTL_REG
	nop
	li      t1, (1<<20) # bit 20 of COP0 reg 20, select 0
	or	t0, t0, t1
	mtc0    t0, CCTL_REG
	nop
	jr	ra
	nop
	.end	en_wb_buf_merge

/*******************************************************************************
*
* dis_wb_buf_merge - Disable RLX5281 write back buffer merge.
*
* RETURNS: N/A
*
*/
	.globl	dis_wb_buf_merge
        .text
	.ent	dis_wb_buf_merge
dis_wb_buf_merge:
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
	.end	dis_wb_buf_merge


/*******************************************************************************
*
* en_wb_buf - Enable RLX5281 write back buffer.
*
* RETURNS: N/A
*
*/
	.globl	en_wb_buf
        .text
	.ent	en_wb_buf
en_wb_buf:
	/* Turn on write buffer */
	mfc0	t0, CCTL_REG
	li	t1, 0xFF3FFFFF
	and	t0, t0, t1
	mtc0    t0, CCTL_REG
	nop
	li      t1, 0x00400000 # bit 22 of COP0 reg 20, select 0
	or	t0, t0, t1
	mtc0    t0, CCTL_REG
	nop
	jr	ra
	nop

	.end	en_wb_buf


/*******************************************************************************
*
* dis_wb_buf - Disable RLX5281 write back buffer.
*
* RETURNS: N/A
*
*/
	.globl	dis_wb_buf
        .text
	.ent	dis_wb_buf
dis_wb_buf:
	/* Turn off write buffer */
	mfc0	t0, CCTL_REG
	li	t1, 0xFF3FFFFF
	and	t0, t0, t1
	mtc0    t0, CCTL_REG
	nop
	li      t1, (1<<23) # bit 23 of COP0 reg 20, select 0
	or	t0, t0, t1
	mtc0    t0, CCTL_REG
	nop
	jr	ra
	nop

	.end	dis_wb_buf



/*******************************************************************************
*
* en_cache_wa - Enable cache write allocation.
*
* RETURNS: N/A
*
*/
	.globl	en_cache_wa
        .text
	.ent	en_cache_wa
en_cache_wa:
	/* Turn on cache Write allocation */
	mfc0	t0, CCTL_REG
	li      t1, (1<<7) # bit 7 of COP0 reg 20, select 0
	not	t1, t1
	and	t0, t0, t1
	mtc0    t0, CCTL_REG
	nop
	li      t1, (1<<7) # bit 7 of COP0 reg 20, select 0
	or	t0, t0, t1
	mtc0    t0, CCTL_REG
	nop
	jr	ra
	nop
	.end	en_cache_wa



/*******************************************************************************
*
* dis_cache_wa - Disable cache write allocation.
*
* RETURNS: N/A
*
*/
	.globl	dis_cache_wa
        .text
	.ent	dis_cache_wa
dis_cache_wa:
	/* Turn off cache Write allocation */
	mfc0	t0, CCTL_REG
	li      t1, (1<<15) # bit 15 of COP0 reg 20, select 0
	not	t1, t1
	and	t0, t0, t1
	mtc0    t0, CCTL_REG
	nop
	li      t1, (1<<15) # bit 15 of COP0 reg 20, select 0
	or	t0, t0, t1
	mtc0    t0, CCTL_REG
	nop
	jr	ra
	nop
	.end	dis_cache_wa




/*******************************************************************************
* Not supported cause Taroko merge register data too.
* en_uncache_write_merg - Enable uncached write merge.
*
* RETURNS: N/A
*
*/
	.globl	en_uncache_write_merg
        .text
	.ent	en_uncache_write_merg
en_uncache_write_merg:
	mfc0	t0, CCTL_REG, 1
	li	t1, (1<<20) # bit 20 of COP0 reg 20, select 1
	not	t1, t1
	and	t0, t0, t1
	mtc0	t0, CCTL_REG, 1
	nop
	li      t1, (1<<20) # bit 20 of COP0 reg 20, select 1
	or	t0, t0, t1
	mtc0	t0, CCTL_REG, 1
	nop
	nop
	jr 	ra
	nop
	.end	en_uncache_write_merg


/*******************************************************************************
*
* dis_uncache_write_merg - Disable uncached write merge.
* Not supported cause Taroko merge register data too.
*
* RETURNS: N/A
*
*/
	.globl	dis_uncache_write_merg
        .text
	.ent	dis_uncache_write_merg
dis_uncache_write_merg:
	mfc0	t0, CCTL_REG, 1
	li	t1, (1<<21) # bit 21 of COP0 reg 20, select 1
	not	t1, t1
	and	t0, t0, t1
	mtc0	t0, CCTL_REG, 1
	nop
	li      t1, (1<<21) # bit 21 of COP0 reg 20, select 1
	or	t0, t0, t1
	mtc0	t0, CCTL_REG, 1
	nop
	nop
	jr 	ra
	nop
	.end	dis_uncache_write_merg


/*******************************************************************************
*
* dis_bran_predic - Disable branch prediction
*
* RETURNS: N/A
*
*/
	.globl	dis_bran_predic
        .text
	.ent	dis_bran_predic
dis_bran_predic:
	mtlxc0 zero, $4
	nop
	jr 	ra
	nop
	.end	dis_bran_predic


/*******************************************************************************
*
* en_bran_predic - Enable branch prediction
*
* RETURNS: N/A
*
*/
	.globl	en_bran_predic
        .text
	.ent	en_bran_predic
en_bran_predic:
	li 	t0, 0x38
	mtlxc0	t0, $4
	nop
	jr 	ra
	nop
	.end	en_bran_predic


/*******************************************************************************
*
* en_LUB - Enable Load Used Buffer
*
* RETURNS: N/A
*
*/
	.globl	en_LUB
        .text
	.ent	en_LUB
en_LUB:
	mfc0	t0, CCTL_REG
	li	t1, (1<<27) # bit 27 of COP0 reg 20, select 1
	not	t1, t1
	and	t0, t0, t1
	mtc0	t0, CCTL_REG
	nop
	li      t1, (1<<27) # bit 27 of COP0 reg 20, select 1
	or	t0, t0, t1
	mtc0	t0, CCTL_REG
	nop
	nop
	jr 	ra
	nop
	.end	en_LUB


/*******************************************************************************
*
* dis_LUB - Disable Load Used Buffer
*
* RETURNS: N/A
*
*/
	.globl	dis_LUB
        .text
	.ent	dis_LUB
dis_LUB:
	mfc0	t0, CCTL_REG
	li	t1, (1<<28) # bit 28 of COP0 reg 20, select 1
	not	t1, t1
	and	t0, t0, t1
	mtc0	t0, CCTL_REG
	nop
	li      t1, (1<<28) # bit 28 of COP0 reg 20, select 1
	or	t0, t0, t1
	mtc0	t0, CCTL_REG
	nop
	nop
	jr 	ra
	nop
	.end	dis_LUB

