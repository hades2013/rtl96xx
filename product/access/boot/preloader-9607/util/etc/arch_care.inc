/* 
    This file is used to transform the parameters from preloader/platform/current/arch.h or release/soc.h to 
    the format of Makefile taking.
    The following parameters are those preloader/Makfile may be interested
*/

#include <soc.h>

aTEXT_BASE := SRAM_BASE##arch_care
aDECOMP_ALGO_GZIP := DECOMP_ALGO_GZIP##arch_care
aDECOMP_ALGO_LZMA := DECOMP_ALGO_LZMA##arch_care
aCACHELINE_SIZE := CACHELINE_SIZE##arch_care
#ifdef OTTO_NAND_FLASH
  aOTTO_NAND_FLASH := OTTO_NAND_FLASH##arch_care
#endif
#ifdef OTTO_NOR_SPI_FLASH
  aOTTO_NOR_SPI_FLASH := OTTO_NOR_SPI_FLASH##arch_care
#endif

#ifdef SOC_CONF_OFFSET
  aSOC_CONF_OFFSET := SOC_CONF_OFFSET##arch_care
#endif
#ifdef ON_FLASH_VMA_GAP
  #if (TBASE == 1)
    aON_FLASH_VMA_GAP := "0x0000"##arch_care
  #else
    aON_FLASH_VMA_GAP := ON_FLASH_VMA_GAP##arch_care
  #endif
#else
    aON_FLASH_VMA_GAP := "0x0"##arch_care
#endif
#ifdef LPLR_BSIZE_CONSTRAINT
  aLPLR_BSIZE_CONSTRAINT := LPLR_BSIZE_CONSTRAINT##arch_care
#else
    aLPLR_BSIZE_CONSTRAINT := 4096
#endif

