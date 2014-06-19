#ifndef RLX_5281_CACHE_OP_H
#define RLX_5281_CACHE_OP_H


#ifndef __ASM__
/* Note: void rlx5281_cache_reset(void) is not allowed to run in cachable address space. */
void rlx5281_cache_reset(void);

#if 0 /*un-used function */
/* rlx5281_invalidate_dcache_range
 * base_addr	: The start data address of CPU virtual address to be invalidated , for example 0x80000000
 * end_addr	: The end data address of CPU virtual address to be invalidated , for example 0x80007fff
 * Note		: The overhead cycles is smaller than rlx5281_invalidate_dcache_cctl when we are going to do a samll
 *	 	  range of cache operation.
 */
void rlx5281_invalidate_dcache_range(unsigned int base_addr, unsigned int end_addr);


/* rlx5281_invalidate_icache_range
 * base_addr	: The start instruction address of CPU virtual address to be invalidated , for example 0x80000000
 * end_addr	: The end instruction address of CPU virtual address to be invalidated , for example 0x80007fff
 * Note		: The overhead cycles is smaller than invalidate_icache_all when we are going to do a samll
 *	 	  range of cache operation.
 */
void rlx5281_invalidate_icache_range(unsigned int base_addr, unsigned int end_addr);


/* rlx5281_write_back_dcache_range 
 * base_addr	: The start data address of CPU virtual address to be written back , for example 0x80000000
 * end_addr	: The end data address of CPU virtual address to be written back , for example 0x80007fff
 * Note		: The overhead cycles is smaller than rlx5281_write_back_dcache_cctl when we are going to do a samll
 *	 	  range of cache operation.
 */
void rlx5281_write_back_dcache_range(unsigned int base_addr, unsigned int end_addr);



/* rlx5281_invalidate_dcache_cctl
 * Invalidate all D-Cache with CCTL register 
 */
void rlx5281_invalidate_dcache_cctl(void);

/* rlx5281_write_back_dcache_cctl
 * Write back all D-Cache with CCTL register 
 */
void rlx5281_write_back_dcache_cctl(void);

/* 
 * rlx5281_enable_L2Cache: Enable L2 cache 
 */
void rlx5281_enable_L2Cache(void);

/* 
 * rlx5281_disable_L2Cache: Disable L2 cache 
 */
void rlx5281_disable_L2Cache(void);


/* rlx5281_write_back_dcache_range 
 * base_addr	: The start data address of CPU virtual address to be written back and invalidated, for example 0x80000000
 * end_addr	: The end data address of CPU virtual address to be written back and invalidated , for example 0x80007fff
 * Note		: The overhead cycles is smaller than rlx5281_write_back_dcache_cctl when we are going to do a samll
 *	 	  range of cache operation.
 */
void rlx5281_write_back_inv_dcache_range(unsigned int base_addr, unsigned int end_addr);

#endif

/* invalidate_icache_all
 * Invalidate all I-Cache with CCTL register 
 */
void invalidate_icache_all(void);

/* rlx5281_write_back_invalidate_dcache_cctl
 * Write back and invalidate all D-Cache with CCTL register 
 */
void rlx5281_write_back_invalidate_dcache_cctl(void);




/*
 * en_wb_buf_merge - Enable RLX5281 write back buffer merge.
 */
void en_wb_buf_merge(void);

/*
 * dis_wb_buf_merge - Disable RLX5281 write back buffer merge.
 */
void dis_wb_buf_merge(void);

/*
 * en_wb_buf - Enable RLX5281 write back buffer.
 */
void en_wb_buf(void);

/*
 * dis_wb_buf - Disable RLX5281 write back buffer.
 */
void dis_wb_buf(void);

/* 
 * en_cache_wa - Enable D-Cache write allocation. 
 */
void en_cache_wa(void);

/* 
 * dis_cache_wa - Disable D-Cache write allocation. 
 */
void dis_cache_wa(void);

 /* en_uncache_write_merg - Enable uncached write merge.
  * Not supported cause Taroko merge register data too. */
void en_uncache_write_merg(void);

/* 
 * dis_uncache_write_merg - Disable uncached write merge. 
 */
void dis_uncache_write_merg(void);

/* 
 * dis_bran_predic - Disable branch prediction 
 */
void dis_bran_predic(void);

/*
 * en_bran_predic - Enable branch prediction 
 */
void en_bran_predic(void);

/* 
 * en_LUB - Enable Load Used Buffer 
 */
void en_LUB(void);

/* 
 * dis_LUB - Disable Load Used Buffer 
 */
void dis_LUB(void);

#endif
#endif //RLX_5281_CACHE_OP_H
