## global information
## input image name, lpreloader_image preloader_image and bootloader, are overwriten by extra command
variable preloader_platform     luna_cli
#variable preloader_image      "plr.nand.img"
#variable bootloader_image     "u-boot.bin"
## output image name, final_image and loader_image, are overwriten by command line 
#variable final_image          "final.img"
#variable loader_image         "loader.img"
variable flash_type             spi_nor
#variable preloader_image        "plr.img"
variable padding_unit           4K

## retrive NOR SPI flash database
source $PARTS_DB_DIR/parts_for_nor_spi.tcl
# !!ATTENTION MXIC_MX25L25735E_SIO and MXIC_MX25L25635F_SIO can be only included EXCLUSIVELY as having the same device ID
# add_model nor_spi MXIC_MX25L25735E_SIO
add_model nor_spi MXIC_MX25L25635F_SIO
add_model nor_spi SPI_NOR_GENERIC

## retrive dram gen2 database
source $PARTS_DB_DIR/parts_for_dram_gen2.tcl
namespace eval WINBOND_W971GG8JB-25 { 
		variable zq_setting             0x000010b0
		variable tx_clk_phs_delay       0x0
		variable clkm_delay             0x0
		variable clkm90_delay           0x0
}
namespace eval WINBOND_W9751G8KB-25 {
		variable zq_setting             0x000010b0
		variable tx_clk_phs_delay       0x0
		variable clkm_delay             0x0
		variable clkm90_delay           0x0
}

add_model dram_gen2 TAK_DDR3_GENERIC_300MHZ
add_model dram_gen2 TAK_DDR2_GENERIC_300MHZ
# add_model dram_gen2 WINBOND_W9751G8KB-25
# add_model dram_gen2 WINBOND_W971GG8JB-25

## PLL information
# NOTE: when using "pin" in "set_by", the corresponding cpu/dsp/dram/lx clock 
#       should be correctly filled as some parameters passed to the preloader
#       are calculated respectively. (E.g., DRAM timer, UART clock divisor and etc) 
namespace eval rtk_pll_gen1 {
    #variable set_by                 pin
    variable set_by                 software
    variable cpu_clock_mhz          700
    variable dsp_clock_mhz          500
    variable dram_clock_mhz         200
    variable lx_clock_mhz           200
}
add_model pll_gen1 rtk_pll_gen1 

## peripheral information
namespace eval rtk_peri {
    variable uart_baudrate          115200
}
add_model peri rtk_peri

## mac info
namespace eval rtk_mac {
    variable address                "11:22:33:44:55:66"
}
add_model mac rtk_mac

##flash layout
# env:  booloader1(U-Boot) environment storage
# opt1: config (HS/CS/tr069 storage, mounted as jffs2)
namespace eval default_flash_layout {
    variable layout [subst {
        { follow    bootloader1 "$bootloader_image" }
        --runtime
        { 144K  env       -reserve 16K }
        { 160K  opt1      -reserve 384K }
        { 544K  kernel1   -reserve 2048K }
        { 2592K rootfs1   -reserve 5600K }
        { 8M    end }
    }]
    # the unit could be -in_byte, and -in_nor_erase 
}
add_model flash_layout default_flash_layout

## debug info
#cout "PARTS_DB_DIR=$PARTS_DB_DIR"
#cout "loader_image=$loader_image"
#cout "final_image=$final_image"
#cout "preloader_image=$preloader_image"
#cout "bootloader_image=$bootloader_image"
#cout "kernel_image=$kernel_image"
#cout "rootfs_image=$rootfs_image"

