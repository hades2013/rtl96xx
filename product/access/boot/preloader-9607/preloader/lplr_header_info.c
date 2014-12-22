#include <soc.h>
#include "plr_spare.h"

#if defined(OTTO_DRAM_GEN) && defined(OTTO_PLL_GEN) 
#else
    #error "OTTO_DRAM_GEN and OTTO_PLL_GEN should be defined in platform/<platform-name>/arch.h"
#endif

soc_configuration_t soc_configuration_in_flash 
    __attribute__ ((section ("header_info.text"))) = {
    .signature1=FC_SIG1,
    .signature2=FC_SIG2,


#ifdef  USE_SOC_SPARE
    .soc.spare_headers=spare_headers,
#else
    .soc.spare_headers=(spare_header_p)0,
#endif


    .soc.header_ver=SOC_HEADER_VERSION,
    .soc.header_type=
        FIHT_NORMAL|
        ((OTTO_DRAM_GEN)<<(FIHT_DRAMCTRL_SHIFT))|
        ((OTTO_PLL_GEN)<<(FIHT_PLL_SHIFT))|
        (OTTO_NAND_FLASH?(FIHT_NAND|FIHT_PARALLEL):0)|
        (OTTO_NOR_SPI_FLASH?(FIHT_NOR|FIHT_SPI):0)
};
