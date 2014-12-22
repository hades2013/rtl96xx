#define ICACHE_SIZE    0x4000
#define DCACHE_SIZE    0x4000
#define CACHELINE_SIZE 32

#define SRAM_BASE 0x9FC00000
#define SRAM_SIZE 0x20000 //128k
//#define SRAM_SIZE 0x10000 //64k


#if (TBASE == 1)
#define FLASH_BASE 0x81000000
#else
#define FLASH_BASE 0x9FC00000
#endif

/* Choose the decompress algorithm builtin */
#define DECOMP_ALGO_LZMA 1
#define DECOMP_ALGO_GZIP 0

#define OTTO_NAND_FLASH    1
#define OTTO_NOR_SPI_FLASH 0
#define LPLR_BSIZE_CONSTRAINT 4096

/*define flash page size */ 
#define FLASH_PAGESIZE_2048 1
#undef FLASH_PAGESIZE_512

/* load U-boot lzma file address */
#define UBOOT_BIN_BASE_NAND 0x83C00000
#define DRAM_UBOOT_ADDR 0x80000000 //luna_nand uboot temp address

// DDR controller generation
#define OTTO_DRAM_GEN 2
#define OTTO_PLL_GEN 1
/* Returns current CPU cycle. */
#define OTTO_CPU_CYCLE()                  \
	({ int __res;                           \
		__asm__ __volatile__("mfc0 %0, $9;"   \
		                     : "=r" (__res)); \
		__res;                                \
	})

#define SYSTEM_RESET() do { \
		*((volatile u32_t *)0xbb000074) = 0x00000004; \
	} while(0)

// using spare area to store more DRAM/FLASH database
#define ON_FLASH_VMA_GAP 0xc00000
#define SOC_NUM_DRAM_SPARE 0
#define SOC_NUM_FLASH_SPARE 0
#define PLR_ENABLE_PLL_SET
#define ZQ_TIMEOUT_RESET 1

