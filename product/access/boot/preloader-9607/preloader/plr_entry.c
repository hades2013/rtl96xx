#include <preloader.h>
#include <plr_spare.h>
#include <cpu_utils.h>

/* Defined in preloader.lds, points to SW-patch section. */
extern sw_patch_t *LS_sw_patch_start;
extern sw_patch_t *LS_move_stack;
extern sw_patch_t *LS_sw_patch_end;

extern u32_t start_of_bss, end_of_bss;
extern spare_header_t spare_headers[];

#if (OTTO_NAND_FLASH == 1)
__attribute__ ((section ("entry.text"))) 
#endif
void c_start_epilogue(void) 
{
	// clean bss;
	pbldr_wzero_range(&start_of_bss, &end_of_bss);
	
	// initialize parameters
	parameters.dram_init_result=INI_RES_UNINIT;
	parameters.flash_init_result=INI_RES_UNINIT;
	parameters._pblr_printf=printf;

#if SOC_NUM_DRAM_SPARE > 0
#define DRAMI (parameters.soc.dram_info)
    u32_t sel, di_num;
	dram_info_t *di;

    sel = dram_model_select();
    di = get_dram_spare(&di_num);
    if ((sel >= 0) && (sel <= di_num)) {
        if(sel!=0){
	        memcpy((void *)&DRAMI, (void *)(di+(sel-1)), sizeof(dram_info_t));
        }
        platform_init_phase_2();
        printf("II: Selected DRAM model #%d.\n", sel);
	} else {
		platform_init_phase_2();	// need console init for following printf
        printf("EE: Bad DRAM model #%d from dram_model_select()."
            " Only 0 ~ %d are configured in soc.tcl.\n",
            sel, di_num);
        while (1);
	}
#else
    platform_init_phase_2();
#endif

	flash_init();

	/* Issue software patch: level = 0~2 */
	sw_patch_t **sw_patch = &LS_sw_patch_start;
	while (sw_patch!=&LS_move_stack) {
	    (*sw_patch)();
	    ++sw_patch;
	}
#if 0    
    /* Set stack pointer to DRAM */
    if(parameters.dram_init_result == INI_RES_OK) {
    	SET_SP(STACK2_BASE);
    	printf("II: Stack @ %p\n", STACK2_BASE);
    } else {
        pblr_puts("II: unable move stack to DRAM\n");
    }
	/* Issue software patch: level = 3~7 */
	sw_patch = &LS_move_stack;
	while (sw_patch!=&LS_sw_patch_end) {
	    (*sw_patch)();
	    ++sw_patch;
	}
#endif

	// while(1) for any fail
	if(parameters.dram_init_result != INI_RES_OK) {
		//printf("II: DRAM failed with error=%d\n", parameters.dram_init_result);
		while(1);
	}
	if(parameters.flash_init_result != INI_RES_OK) {
		//printf("II: FLASH failed with error=%d\n", parameters.flash_init_result);
		while(1);
	}
	
	/* Set stack pointer to DRAM */
	SET_SP(STACK2_BASE);
	printf("II: Stack @ %p\n", STACK2_BASE);
	uboot_setup();

	while(1);
}
