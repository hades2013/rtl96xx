## globale information
#set PARTS_DB_DIR               "../util/parts_db"
variable preloader_platform     luna_nand
variable flash_type             nand
variable nand_ecc               BCH6
variable plr_version            0x000a
variable padding_unit           2K
variable lplr_skip		1
## input image name, lpreloader_image preloader_image and bootloader, are overwriten by extra command
#variable lpreloader_image     "lplr.nand.img"
#variable preloader_image      "plr.nand.img"
#variable bootloader_image     "u-boot.bin"
## output image name, final_image and loader_image, are overwriten by command line 
#variable final_image          "final.img"
#variable loader_image         "loader.img"


## retrive nand flash database
source $PARTS_DB_DIR/parts_for_nand.tcl
#add_model nand TOSHIBA_TC58NVM9S3ETA00
add_model nand MXIC_MX30LF1G08AA 

## retrive dram gen2 database
source $PARTS_DB_DIR/parts_for_dram_gen2.tcl
# namespace eval WINBOND_W631GG6KB {
# ## ODT=150, OCD=50
#     variable zq_setting             0x000008be
#     variable tx_clk_phs_delay       0x00
#     variable clkm_delay             0x00
#     variable clkm90_delay           0x00
#     variable mrs_drv_strength       1          ;#(DDR-OCD) 0->240/6=40 ohm, 1->240/7=34 ohm 
#     variable mrs_odt                120        ;#(DDR1) none, (DDR2) 0, 50, 75, 150; (DDR3-ntt_nom) 0, 20, 30, 40, 60, 120
#     variable mrs_rtt_wr             120        ;#(DDR3) 0->disable, 60, 120
# }

add_model dram_gen2 WINBOND_W631GG6KB
#add_model dram_gen2 NANYA_NT5CB128MHP

## PLL gen1 information
# NOTE: when using "pin" in "set_by", the corresponding cpu/dsp/dram/lx clock 
#       should be correctly filled as some parameters passed to the preloader
#       are calculated respectively. (E.g., DRAM timer, UART clock divisor and etc) 
namespace eval rtk_pll_gen1 {
    variable set_by                 software
    variable cpu_clock_mhz          600
    variable dsp_clock_mhz          500
    variable dram_clock_mhz         300
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
namespace eval default_flash_layout {
    variable layout [subst {
        --runtime
        {   follow      kernel1  -reserve 4M }
        {   follow      rootfs1  -reserve 4M+512K}
        {   follow      opt1     -reserve 128K}
        {   follow      opt4     -reserve 128K}
        {   follow      end }
    }]
}
add_model flash_layout default_flash_layout

## debug info
#cout "PARTS_DB_DIR=$PARTS_DB_DIR"
#cout "loader_image=$loader_image"
#cout "final_image=$final_image"
#cout "lpreloader_image=$lpreloader_image"
#cout "preloader_image=$preloader_image"
#cout "bootloader_image=$bootloader_image"
#cout "kernel_image=$kernel_image"
#cout "rootfs_image=$rootfs_image"

