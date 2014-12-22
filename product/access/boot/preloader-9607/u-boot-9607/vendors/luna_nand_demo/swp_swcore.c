#include <common.h>
#include "swCore.h"
#include <asm/arch/bspchip.h>
#include "soc_remap.h"


#define PATCH_REG(x, lvl) \
	void_func * __swp_##x __attribute__ ((section (".soft_patch." #lvl))) = x



#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))

#define MACReg(offset, val)		(WRITE_MEM32(SWITCH_BASE + offset, val))

extern struct soc_reg_remap_t soc_reg_remap;
int swp_swCore_init(void)
{
/* Move init to Lan_RXENABLE() at re8670poll.c */


}


PATCH_REG(swp_swCore_init, 23);
