#include <preloader.h>
#include <cpu_utils.h>

void c_start_prologue(void) {
	void_func *preloader;
	u32_t modified_sp;

	GET_SP(modified_sp);

	/* Since c_start_prologue() never return,
	   we could change SP here without damage. */
	SET_SP(STACK1_BASE);

	parameters_init();

#if (OTTO_NAND_FLASH == 1)
	/* 1. create BBT,
	   2. check sanity of preloader/U-Boot, and
	   3. reassemble preloader on uncached SRAM */
    extern void nand_loader_init();
	nand_loader_init();

	preloader = (void_func *)(SRAM_BASE + 4096);
#elif (OTTO_NOR_SPI_FLASH == 1)
	preloader = &c_start_epilogue;
#else
	preloader = VZERO;
#endif

	SET_SP(STACK1_BASE - (STACK0_BASE - modified_sp));
	preloader();
}
