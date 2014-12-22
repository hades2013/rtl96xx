#include <preloader.h>

#if (DECOMP_ALGO_GZIP == 1)
#  include <zlib/zlib.h>
#  define DECOMPRESS_OK Z_STREAM_END
#elif (DECOMP_ALGO_LZMA == 1)
#  include <lzma/LzmaDec.h>
#  define DECOMPRESS_OK SZ_OK
#endif

#define PIMG_MAGIC_NUM 0x27051956
#define BOOT_SIZE 0x80000  //uboot 512k


void uboot_setup(void) {
	s32_t res;
	uimage_header_t *uimg_hdr;
	void *deflated_blr_dram_base = NULL;
	void *heap_base, *blr_flash_base;

#if (OTTO_NOR_SPI_FLASH == 1)
#define LAYOUTI parameters.soc.layout
	uimg_hdr = (uimage_header_t *)(LAYOUTI.bootloader1_addr + FLASH_BASE);
	blr_flash_base = (void *)(uimg_hdr + 1);

	if (uimg_hdr->ih_comp != UIH_COMP_NONE) {
		deflated_blr_dram_base = alloca(uimg_hdr->ih_size);
		printf("II: Copying %dK deflated U-Boot (%p -> %p)... ",
		       uimg_hdr->ih_size>>10, blr_flash_base, deflated_blr_dram_base);
		memcpy(deflated_blr_dram_base, blr_flash_base, uimg_hdr->ih_size);
		printf("OK\n");
	}
#elif (OTTO_NAND_FLASH == 1)
	/* nursing_uboot() should supply location of uimg_hdr! */
//	uimg_hdr = NULL;

	/* 1. check health of U-Boot,
	   2. reassemble UBoot on uncached DRAM, and
	   3. fix health */

	nursing_uboot();// loader bootloader to DRAM_UBOOT_ADDR
	
	printf("II: Starting U-Boot...\n");
	uimg_hdr = (uimage_header_t *)DRAM_UBOOT_ADDR;
	//printf("uimg_hdr->ih_magic is %x\n",uimg_hdr->ih_magic);
	if(uimg_hdr->ih_magic!=PIMG_MAGIC_NUM){//normal uboot.bin		
		//printf("normal uboot.bin \n");
		memcpy(UBOOT_BIN_BASE_NAND, DRAM_UBOOT_ADDR, BOOT_SIZE);
		/* Set stack pointer to DRAM */
			  ((void_func *)UBOOT_BIN_BASE_NAND)();
	}
	deflated_blr_dram_base=blr_flash_base = (void *)(uimg_hdr + 1);

#endif

	if (uimg_hdr->ih_comp == UIH_COMP_LZMA) {
		heap_base = alloca(HEAP_SIZE);

		printf("II: Inflating U-Boot (%p -> %p)... ",
		       deflated_blr_dram_base, uimg_hdr->ih_load);

		res = decompress((u8_t *)deflated_blr_dram_base, (u8_t *)uimg_hdr->ih_load,
		                 heap_base);

		if (res != DECOMPRESS_OK) {
			printf("\nEE: decompress failed: %d\n", res);
			while (1);
		} else {
			printf("OK\n");
		}
	} else if (uimg_hdr->ih_comp == UIH_COMP_NONE) {
		printf("II: Copying %dK inflated U-Boot (%p -> %p)... ",
		       uimg_hdr->ih_size>>10, blr_flash_base, uimg_hdr->ih_load);
		memcpy((u8_t *)uimg_hdr->ih_load, blr_flash_base, uimg_hdr->ih_size);
		printf("OK\n");
	} else {
		printf("EE: unsupported bootloader format.\n");
		while(1);
	}

	printf("II: Starting U-Boot... \n");

	parameters._dcache_writeback_invalidate_all();
	((void_func *)uimg_hdr->ih_ep)();

	return;
}
