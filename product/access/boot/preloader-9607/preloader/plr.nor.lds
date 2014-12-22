OUTPUT_FORMAT("elf32-tradbigmips", "elf32-tradbigmips", "elf32-tradbigmips")
OUTPUT_ARCH(mips)
SECTIONS
{
    . = PLR_BASE;
    .text : AT (LMA_base) {
        *(entry.text);
        . = ALIGN(SOC_CONF_OFFSET);
        *(header_info.text);
        . = ALIGN(4);
        *(.text);
        . = ALIGN(4);
        *(.reginfo);
    }
    . = ALIGN(16);
    end_of_text = .;

    start_of_frodata = .;
    .frodata  : AT(LMA_base + (start_of_frodata& 0xFFFFF)) { 
        platform/current/plr_flash.o(.rodata);
    }
    . = ALIGN(16);
    end_of_frodata = .;  
 
    start_of_data = .;
    .data  : AT(LMA_base + (start_of_data& 0xFFFFF)) { 
        LS_sw_patch_start = .;
        *(.soft_patch.0);
        *(.soft_patch.1);
        *(.soft_patch.2);
        LS_move_stack = .;
        *(.soft_patch.3);
        *(.soft_patch.4);
        *(.soft_patch.5);
        *(.soft_patch.6);
        LS_sw_patch_end = .;
        *(.data); 
        *(.sdata);
    }
    . = ALIGN(16);
    end_of_data = .;  
    
    end_of_sram = .;
    LS_preloader_in_SRAM_sz = ( end_of_sram & 0xFFFFF);

    start_of_bss = .;
    .bss  :  {
        *(.bss);
        *(.sbss); 
    }
    . = ALIGN(4);
    end_of_bss = .;

    . = end_of_sram + on_flash_vma_gap;
    begin_of_nor_only = .;
    .nor_only_area : AT (LMA_base + (begin_of_nor_only & 0xFFFFF)) {
        *(.rodata);
        *(.rodata.str1.4);
        . = ALIGN(4);

        nor_spare_header_begin = .;
        *(nor_spare_header);
        nor_spare_header_end = .;
        . = ALIGN(4);

        flash_text_begin = .;
        *(.flash_text);
        flash_text_end = .;
        . = ALIGN(4);

        flash_data_begin = .;
        *(.flash_data);
        flash_data_end = .;
        . = ALIGN(16);
        
        cli_ddr_para_begin = .;
        *(.cli_ddr_para);
        cli_ddr_para_end = .;

        cli_pll_para_begin = .;
        *(.cli_pll_para);
        cli_pll_para_end = .;

	cli_cpu_para_begin = .;
        *(.cli_cpu_para);
        cli_cpu_para_end = .;
    }
    . = ALIGN(4);
    . -= on_flash_vma_gap;
    end_of_nor_only = .;
    
    preloader_end = . ;
    LS_preloader_total_sz = ( preloader_end & 0xFFFFF);
}
