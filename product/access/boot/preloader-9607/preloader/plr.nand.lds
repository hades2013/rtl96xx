OUTPUT_FORMAT("elf32-tradbigmips", "elf32-tradbigmips", "elf32-tradbigmips")
OUTPUT_ARCH(mips)
SECTIONS
{

    . = PLR_BASE + DMA_SIZE;
    .text : {
        *(entry.text)

        . = ALIGN(SOC_CONF_OFFSET);
        *(header_info.text)

        . = ALIGN(CACHELINE_SIZE);
        *(.text)
    }

    . = ALIGN(4);
    .rodata  : { *(.rodata) }

    . = ALIGN(4);
    .data  : { *(.data) }

    . = ALIGN(4);
    .sdata  : { *(.sdata) }

    . = ALIGN(4);
    .soft_patch : {
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
    }

    . = ALIGN(16);
    .cli_para_list : {
        cli_ddr_para_begin = .;
        *(.cli_ddr_para);
        cli_ddr_para_end = .;
    
        cli_pll_para_begin = .;
        *(.cli_pll_para);
        cli_pll_para_end = .;
    }

    . = ALIGN(4);
    .spare_for_nand : {
        nand_spare_header_begin = .;
        *(nand_spare_header);
        nand_spare_header_end = .;
    }

    . = ALIGN(4);
    start_of_bss = .;
    .sbss  : { *(.sbss) }
    .bss  : { *(.bss) }
    . = ALIGN(4);
    end_of_bss = .;


    LS_preloader_in_SRAM_sz = ( . & 0xFFFFF);

    preloader_end = . ;
    LS_preloader_total_sz = ( . & 0xFFFFF);
}
