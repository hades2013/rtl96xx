#ifndef RLX5281_H
#define RLX5281_H

#include "regdef.h"
#include "soc.h"

#define CCTL_REG $20
#define CACHE_OP_D_INV		(0x11)
#define CACHE_OP_D_WB_INV	(0x15)
#define CACHE_OP_D_WB		(0x19)
#define CACHE_OP_I_INV		(0x10)
#define DCACHE_LINE_SIZE	(CACHELINE_SIZE)
#define ICACHE_LINE_SIZE	(CACHELINE_SIZE)


#endif //RLX5281_H
