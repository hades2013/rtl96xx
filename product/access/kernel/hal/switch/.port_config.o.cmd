cmd_drivers/lwdrv/hal/switch/port_config.o := rsdk-linux-gcc -Wp,-MD,drivers/lwdrv/hal/switch/.port_config.o.d  -nostdinc -isystem /opt/workspace/rtl9607/toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/bin/../lib/gcc/mips-linux/4.4.6/include -Iinclude  -I/opt/workspace/rtl9607/product/access/kernel/linux-2.6.x/arch/rlx/include -include include/linux/autoconf.h -Iinclude/soc -D__KERNEL__ -D"VMLINUX_LOAD_ADDRESS=0x80041000" -D"LOADADDR=0x80041000" -D"DATAOFFSET=0" -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-delete-null-pointer-checks -D__LUNA_KERNEL__ -Os -fno-inline -ffunction-sections -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -EB -UMIPSEB -U_MIPSEB -U__MIPSEB -U__MIPSEB__ -UMIPSEL -U_MIPSEL -U__MIPSEL -U__MIPSEL__ -DMIPSEB -D_MIPSEB -D__MIPSEB -D__MIPSEB__ -Iinclude/asm-rlx -Iarch/rlx/bsp_rtl8686/ -Idrivers/net/rtl86900/sdk/include/ -Idrivers/net/rtl86900/sdk/system/include -I/opt/workspace/rtl9607/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-dwarf2-cfi-asm -I/opt/workspace/rtl9607/product/access/../../app/include -I/opt/workspace/rtl9607/product/access/include -I/opt/workspace/rtl9607/product/access/sdk/realtek/sdk-1.0.0/system/include -I/opt/workspace/rtl9607/product/access/sdk/realtek/sdk-1.0.0/include/ -I/opt/workspace/rtl9607/product/access/../../kernel/drv/hal/switch   -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(port_config)"  -D"KBUILD_MODNAME=KBUILD_STR(port_config)"  -c -o drivers/lwdrv/hal/switch/port_config.o drivers/lwdrv/hal/switch/port_config.c

deps_drivers/lwdrv/hal/switch/port_config.o := \
  drivers/lwdrv/hal/switch/port_config.c \
  /opt/workspace/rtl9607/product/access/../../app/include/lw_type.h \
    $(wildcard include/config/product/epn104.h) \
    $(wildcard include/config/product/epn204.h) \
  /opt/workspace/rtl9607/product/access/../../app/include/lw_config.h \
    $(wildcard include/config/h.h) \
    $(wildcard include/config/product/epn105.h) \
    $(wildcard include/config/eoc/extend.h) \
    $(wildcard include/config/defaults/realtek.h) \
    $(wildcard include/config/vendor.h) \
    $(wildcard include/config/defaults/realtek/sdk.h) \
    $(wildcard include/config/sdk.h) \
    $(wildcard include/config/defaults/libc/uclibc/0/9/30.h) \
    $(wildcard include/config/libcdir.h) \
    $(wildcard include/config/product/name.h) \
    $(wildcard include/config/copyright/time.h) \
    $(wildcard include/config/copyright/string.h) \
    $(wildcard include/config/release/ver.h) \
    $(wildcard include/config/develop/ver.h) \
    $(wildcard include/config/develop/hardware/ver.h) \
    $(wildcard include/config/default/hostname.h) \
    $(wildcard include/config/product/series/and/factory/code.h) \
    $(wildcard include/config/product/name/priv.h) \
    $(wildcard include/config/release/ver/priv.h) \
    $(wildcard include/config/develop/ver/priv.h) \
    $(wildcard include/config/hexicom/menu/password.h) \
    $(wildcard include/config/hexicom/boot/password.h) \
    $(wildcard include/config/boot/restore/app.h) \
    $(wildcard include/config/boot/multi/app.h) \
    $(wildcard include/config/master.h) \
    $(wildcard include/config/vtysh.h) \
    $(wildcard include/config/lib.h) \
    $(wildcard include/config/httpd.h) \
    $(wildcard include/config/diag/debug.h) \
    $(wildcard include/config/catvcom.h) \
    $(wildcard include/config/ponmonitor.h) \
    $(wildcard include/config/loopd.h) \
    $(wildcard include/config/onu/compatible.h) \
  /opt/workspace/rtl9607/product/access/../../app/include/lw_drv_pub.h \
    $(wildcard include/config/product/epn104n.h) \
    $(wildcard include/config/product/epn104w.h) \
    $(wildcard include/config/product/epn104zg.h) \
    $(wildcard include/config/product/epn104zg/a.h) \
    $(wildcard include/config/product/gpn104n.h) \
    $(wildcard include/config/product/epn101r.h) \
    $(wildcard include/config/product/epn101zg.h) \
    $(wildcard include/config/product/5500.h) \
    $(wildcard include/config/s.h) \
    $(wildcard include/config/schedule/error.h) \
    $(wildcard include/config/schedule/finish.h) \
    $(wildcard include/config/schedule/pvlan.h) \
    $(wildcard include/config/schedule/return/sum/page.h) \
    $(wildcard include/config/wdt.h) \
    $(wildcard include/config/dhcpsp.h) \
  /opt/workspace/rtl9607/product/access/../../app/include/pdt_config.h \
    $(wildcard include/config/h/.h) \
    $(wildcard include/config/isouservlan.h) \
    $(wildcard include/config/hard/version.h) \
  /opt/workspace/rtl9607/product/access/../../app/include/pdt_priv_config.h \

drivers/lwdrv/hal/switch/port_config.o: $(deps_drivers/lwdrv/hal/switch/port_config.o)

$(deps_drivers/lwdrv/hal/switch/port_config.o):
