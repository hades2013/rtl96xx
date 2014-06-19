#include <plr_spare.h>

#ifdef USE_SOC_SPARE

// spare(s)
#if SOC_NUM_DRAM_SPARE > 0
dram_info_t dram_spare[SOC_NUM_DRAM_SPARE] SECTION_ON_FLASH;
#endif
#if SOC_NUM_FLASH_SPARE > 0
flash_info_t flash_spare[SOC_NUM_FLASH_SPARE] SECTION_ON_FLASH;
#endif

// spare header array
spare_header_t spare_headers[] SOC_SPARE_HEADER_SECTION = {
    #ifdef SOC_NUM_DRAM_SPARE
        {
            .type = SST_DRAM,
            .num_instances=SOC_NUM_DRAM_SPARE,
            .spare=(void*)&dram_spare
        },
    #endif
    #ifdef SOC_NUM_FLASH_SPARE
        {
            .type = SST_FLASH,
            .num_instances=SOC_NUM_FLASH_SPARE,
            .spare=(void*)&flash_spare
        },
    #endif
    {
        .type = SST_END,
        .num_instances=0,
        .spare=(void*)0
    }
};

#endif // USE_SOC_SPARE
