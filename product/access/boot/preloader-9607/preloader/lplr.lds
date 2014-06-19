OUTPUT_FORMAT("elf32-tradbigmips", "elf32-tradbigmips", "elf32-tradbigmips")
OUTPUT_ARCH(mips)
SECTIONS
{
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
        *(.soft_patch);
        LS_sw_patch_end = .;
    }

    . = ALIGN(4);
    .sbss  : { *(.sbss) }
    .bss  : { *(.bss) }

    . = ALIGN(4);
    .spare_for_nand : {
        nand_spare_header_begin = .;
        *(nand_spare_header);
        nand_spare_header_end = .;
    }

    . = ALIGN(4);
    end_of_bss = .;
    LS_preloader_in_SRAM_sz = ( . & 0xFFFFF);

    preloader_end = . ;
    LS_preloader_total_sz = ( . & 0xFFFFF);
}
