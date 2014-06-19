cmd_rtk_voip/voip_drivers/gpio/gpio_8686.o := rsdk-linux-gcc -Wp,-MD,rtk_voip/voip_drivers/gpio/.gpio_8686.o.d  -nostdinc -isystem /home/shipeng/EPN104N/trunk/toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/bin/../lib/gcc/mips-linux/4.4.6/include -Iinclude  -I/home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include -include include/linux/autoconf.h -Iinclude/soc -D__KERNEL__ -D"VMLINUX_LOAD_ADDRESS=0x80041000" -D"LOADADDR=0x80041000" -D"DATAOFFSET=0" -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-delete-null-pointer-checks -D__LUNA_KERNEL__ -Os -fno-inline -ffunction-sections -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -EB -UMIPSEB -U_MIPSEB -U__MIPSEB -U__MIPSEB__ -UMIPSEL -U_MIPSEL -U__MIPSEL -U__MIPSEL__ -DMIPSEB -D_MIPSEB -D__MIPSEB -D__MIPSEB__ -Iinclude/asm-rlx -Iarch/rlx/bsp_rtl8686/ -I/home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-dwarf2-cfi-asm -Irtk_voip/include   -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(gpio_8686)"  -D"KBUILD_MODNAME=KBUILD_STR(gpio_8686)"  -c -o rtk_voip/voip_drivers/gpio/gpio_8686.o rtk_voip/voip_drivers/gpio/gpio_8686.c

deps_rtk_voip/voip_drivers/gpio/gpio_8686.o := \
  rtk_voip/voip_drivers/gpio/gpio_8686.c \
  rtk_voip/voip_drivers/gpio/gpio.h \
    $(wildcard include/config/rtk/voip/drivers/pcm8186.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8651.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8671.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8672.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8676.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm865xc.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8972b/family.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm89xxc.h) \
    $(wildcard include/config/rtk/voip/platform/8686.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm89xxd.h) \
    $(wildcard include/config/rtk/voip/soc/8686/cpu0.h) \
    $(wildcard include/config/rtk/voip/soc/8686/cpu1.h) \
  include/linux/config.h \
    $(wildcard include/config/h.h) \
  rtk_voip/include/voip_types.h \
    $(wildcard include/config/rtl865xb.h) \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbd.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/posix_types.h \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/sgidefs.h \
  include/linux/version.h \
  rtk_voip/voip_drivers/gpio/gpio_8686.h \

rtk_voip/voip_drivers/gpio/gpio_8686.o: $(deps_rtk_voip/voip_drivers/gpio/gpio_8686.o)

$(deps_rtk_voip/voip_drivers/gpio/gpio_8686.o):
