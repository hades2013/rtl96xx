OUTPUT_FORMAT("elf32-tradbigmips", "elf32-tradbigmips", "elf32-tradbigmips")
OUTPUT_ARCH(mips)
SECTIONS
{
	.text   : { *(.text) }

	. = ALIGN(4);
	.rodata : { *(.rodata) }

	. = ALIGN(4);
	.data   : { *(.data) }

	. = ALIGN(4);
	.sdata  : { *(.sdata) }

	. = ALIGN(4);
	.sbss   : { *(.sbss) }
	.bss    : { *(.bss) }
}
