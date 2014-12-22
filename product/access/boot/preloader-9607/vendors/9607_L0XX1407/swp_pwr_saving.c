#include <preloader.h>
#include <plr_spare.h>
#include <cpu_utils.h>
#include <pblr.h>
//#include "plr_dram_gen2.h"
#define MPMR0_A             (0xB8001040)


void luna_pwr_saving_entry(void) {
    u32_t reg_val;

    reg_val = REG32(MPMR0_A);
    /* Enable DRAM automatic power down */ 
    reg_val = ((reg_val & ~(0x3 << 28)) | (0x1 << 28));
    /* Count 16 DRAM clocks before activating power down */ 
    reg_val = ((reg_val & ~(0x3FF << 12)) | (0xF << 12));
    REG32(MPMR0_A) = reg_val;
    

    /* Disable CPU2 */
    REG32(0xb8000204) |= (0x1 << 5);

    reg_val = REG32(0xb8000600);
    printf("0xb8000600=0x%08x\n", reg_val);
    /* Disable all IPs */
    reg_val &= ~( (0x3 << 30) | (0xFFFF));
    /* Enable GMAC */
    reg_val |= (0x1 << 1);
    REG32(0xb8000600) = reg_val;
    printf("0xb8000600=0x%08x\n", REG32(0xb8000600));
}
PATCH_REG(luna_pwr_saving_entry, 1);
