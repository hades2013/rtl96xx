#define ICACHE_SIZE    0x4000
#define DCACHE_SIZE    0x4000
#define CACHELINE_SIZE 32

#define SRAM_BASE 0x9F000000
//#define SRAM_SIZE 0x8000
#define SRAM_SIZE 0x10000

/* To strip compiler warning message */
#ifndef TBASE
#define TBASE 0
#endif

#if (TBASE == 1)
#define FLASH_BASE 0x81000000
#else
#define FLASH_BASE 0x9FC00000
#endif

#define SPI_NOR_FLASH_MMIO_BASE0 (0x9D000000)
#define SPI_NOR_FLASH_MMIO_SIZE0 (0x02000000) /* 32MB */
#define SPI_NOR_FLASH_MMIO_BASE1 (0x9FC00000)
#define SPI_NOR_FLASH_MMIO_SIZE1 (0x00400000) /* 4MB */

/* Choose the decompress algorithm builtin */
#define DECOMP_ALGO_LZMA 1
#define DECOMP_ALGO_GZIP 0

#define OTTO_NAND_FLASH    0
#define OTTO_NOR_SPI_FLASH 1

/* DDR controller generation */
#define OTTO_DRAM_GEN 2
#define OTTO_PLL_GEN 1

/* Returns current CPU cycle. */
#define OTTO_CPU_CYCLE()                  \
	({ int __res;                           \
		__asm__ __volatile__("mfc0 %0, $9;"   \
		                     : "=r" (__res)); \
		__res;                                \
	})

/* Get flash controller's address mode */
#define OTTO_FLASH_ADDR_MODE() \
	({ \
		int __res = 3; \
		if (REG32(0xB8000100) & (1 << 6)) { \
			__res = 4; \
		} \
		__res; \
	})

/* flash controller enable 4-byte address mode */
#define OTTO_FLASH_ENABLE_4BYTE_ADDR_MODE() \
	({ \
		tmp = *((volatile u32_t *)(0xB8000100));	\
		tmp = (tmp & 0xFFFFFFBF) | (0x40);	\
	        *((volatile u32_t *)(0xB8000100)) = tmp;	\
		SPI_CMD_SIMPLE(i, SFCMD_EN4B);\
	})

#define SYSTEM_RESET() do { \
		*((volatile u32_t *)0xbb000074) = 0x00000004; \
	} while(0)

// using spare area to store more DRAM/FLASH database
#define ON_FLASH_VMA_GAP 0xc00000
#define SOC_NUM_DRAM_SPARE 1
#define SOC_NUM_FLASH_SPARE 5
#define PRINTF_SECTION SECTION_ON_FLASH
#define LZMA_SECTION SECTION_ON_FLASH
#define PLR_ENABLE_PLL_SET
#define ZQ_TIMEOUT_RESET 1

