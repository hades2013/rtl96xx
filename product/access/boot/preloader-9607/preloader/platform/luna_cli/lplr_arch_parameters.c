#include <preloader.h>
#include "rlx5281_cache_op.h"

void rlx5281_write_back_invalidate_dcache_cctl();
void invalidate_icache_all(void);

void arch_parameters(void) {
	parameters._dcache_writeback_invalidate_all = &rlx5281_write_back_invalidate_dcache_cctl;
	parameters._icache_invalidate_all = &invalidate_icache_all;

	return;
}
