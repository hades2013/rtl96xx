cmd_rtk_voip/voip_drivers/snd_pin_cs.o := rsdk-linux-gcc -Wp,-MD,rtk_voip/voip_drivers/.snd_pin_cs.o.d  -nostdinc -isystem /home/shipeng/rtl86907/aaa/trunk/toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/bin/../lib/gcc/mips-linux/4.4.6/include -Iinclude  -I/home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include -include include/linux/autoconf.h -Iinclude/soc -D__KERNEL__ -D"VMLINUX_LOAD_ADDRESS=0x80041000" -D"LOADADDR=0x80041000" -D"DATAOFFSET=0" -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-delete-null-pointer-checks -D__LUNA_KERNEL__ -Os -fno-inline -ffunction-sections -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -EB -UMIPSEB -U_MIPSEB -U__MIPSEB -U__MIPSEB__ -UMIPSEL -U_MIPSEL -U__MIPSEL -U__MIPSEL__ -DMIPSEB -D_MIPSEB -D__MIPSEB -D__MIPSEB__ -Iinclude/asm-rlx -Iarch/rlx/bsp_rtl8686/ -Idrivers/net/rtl86900/sdk/include/ -Idrivers/net/rtl86900/sdk/system/include -I/home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-dwarf2-cfi-asm -Irtk_voip/include -Iarch/rlx/bsp -Irtk_voip/voip_dsp/dsp_r1/common/basic_op -Irtk_voip/voip_dsp/dsp_r1/common/util -Irtk_voip/voip_dsp/include -Irtk_voip/voip_dsp/cp3 -Irtk_voip/voip_dsp/v152 -Irtk_voip/voip_drivers/ -Irtk_voip/voip_drivers/spi -Irtk_voip/voip_dsp/dsp_r1/include -Irtk_voip/voip_drivers/zarlink -Irtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/ -Irtk_voip/voip_drivers/zarlink/api_lib-2.16.1/profiles/ -Irtk_voip/voip_drivers/zarlink/api_lib-2.16.1/arch/rtl89xxb/ -Irtk_voip/voip_drivers/zarlink/common/ -Irtk_voip/voip_drivers/zarlink/ve890/ -Irtk_voip/voip_drivers/zarlink/api_lib-2.16.1/vp890_api/ -DFEATURE_COP3_PROFILE -DFEATURE_COP3_PCMISR -DFEATURE_COP3_PCM_RX -DFEATURE_COP3_LEC -DMODULE_NAME=\"DRV\"   -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(snd_pin_cs)"  -D"KBUILD_MODNAME=KBUILD_STR(snd_pin_cs)"  -c -o rtk_voip/voip_drivers/snd_pin_cs.o rtk_voip/voip_drivers/snd_pin_cs.c

deps_rtk_voip/voip_drivers/snd_pin_cs.o := \
  rtk_voip/voip_drivers/snd_pin_cs.c \
    $(wildcard include/config/rtk/voip/slic/si32178/nr.h) \
    $(wildcard include/config/rtk/voip/slic/si32178/pin/cs/use/sw.h) \
    $(wildcard include/config/rtk/voip/slic/si32178/pin/cs.h) \
    $(wildcard include/config/rtk/voip/slic/si32176/nr.h) \
    $(wildcard include/config/rtk/voip/slic/si32176/pin/cs/use/sw.h) \
    $(wildcard include/config/rtk/voip/slic/si32176/pin/cs.h) \
    $(wildcard include/config/rtk/voip/slic/si32176/cs/nr.h) \
    $(wildcard include/config/rtk/voip/slic/si32176cs/pin/cs/use/sw.h) \
    $(wildcard include/config/rtk/voip/slic/si32176/cs/pin/cs.h) \
    $(wildcard include/config/rtk/voip/slic/si32176/si32178/nr.h) \
    $(wildcard include/config/rtk/voip/slic/si32176/si32178/pin/cs/use/sw.h) \
    $(wildcard include/config/rtk/voip/slic/si32176/si32178/pin/cs.h) \
    $(wildcard include/config/rtk/voip/slic/si3226x/nr.h) \
    $(wildcard include/config/rtk/voip/slic/si3226x/pin/cs/use/sw.h) \
    $(wildcard include/config/rtk/voip/slic/si3226x/pin/cs.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/le88111/nr.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/le88111/pin/cs.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/le88221/nr.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/le88221/pin/cs.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/le89116/nr.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/le89116/pin/cs/use/sw.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/le89116/pin/cs.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/le89316/nr.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/le89316/pin/cs/use/sw.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/le89316/pin/cs.h) \
    $(wildcard include/config/rtk/voip/software/spi.h) \
  rtk_voip/include/rtk_voip.h \
    $(wildcard include/config/rtk/voip.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm865xc.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8972b/family.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm89xxc.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8672.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8676.h) \
    $(wildcard include/config/rtk/voip/platform/8686.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm89xxd.h) \
    $(wildcard include/config/audiocodes/voip.h) \
    $(wildcard include/config/rtk/voip/drivers/ip/phone.h) \
    $(wildcard include/config/rtk/voip/ipc/arch/is/dsp.h) \
    $(wildcard include/config/rtk/voip/wideband/support.h) \
    $(wildcard include/config/rtk/voip/g7231.h) \
    $(wildcard include/config/rtk/voip/g729ab.h) \
    $(wildcard include/config/rtk/voip/slic/si3226/nr.h) \
    $(wildcard include/config/rtk/voip/dect/dspg/hs/nr.h) \
    $(wildcard include/config/rtk/voip/dect/sitel/hs/nr.h) \
    $(wildcard include/config/rtk/voip/ip/phone/ch/nr.h) \
    $(wildcard include/config/rtk/voip/dsp/device/nr.h) \
    $(wildcard include/config/rtk/voip/ipc/arch/is/host.h) \
    $(wildcard include/config/rtk/voip/slic/ch/nr/per/dsp.h) \
    $(wildcard include/config/rtk/voip/daa/ch/nr/per/dsp.h) \
    $(wildcard include/config/rtk/voip/drivers/mirror/slic/nr.h) \
    $(wildcard include/config/rtk/voip/drivers/mirror/daa/nr.h) \
    $(wildcard include/config/rtk/voip/drivers/daa/support.h) \
    $(wildcard include/config/rtk/voip/drivers/virtual/daa.h) \
    $(wildcard include/config/rtk/voip/drivers/virtual/daa/2/relay/support.h) \
    $(wildcard include/config/rtk/voip/con/ch/num.h) \
    $(wildcard include/config/rtk/voip/bus/pcm/ch/num.h) \
    $(wildcard include/config/rtk/voip/bus/iis/ch/num.h) \
    $(wildcard include/config/rtk/voip/drivers/8186v/router.h) \
    $(wildcard include/config/rtk/voip/wan/vlan.h) \
    $(wildcard include/config/rtk/voip/clone/mac.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8186.h) \
    $(wildcard include/config/rtk/voip/gpio/8962.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8671.h) \
    $(wildcard include/config/rtk/voip/gpio/8972b.h) \
    $(wildcard include/config/rtk/voip/gpio/8954c/v100.h) \
    $(wildcard include/config/rtk/voip/gpio/8954c/v200.h) \
    $(wildcard include/config/rtk/voip/slic/num/8.h) \
    $(wildcard include/config/rtk/voip/daa/num/8.h) \
    $(wildcard include/config/rtk/voip/led.h) \
    $(wildcard include/config/rtk/voip/drivers/fxo.h) \
    $(wildcard include/config/default/new/ec128.h) \
    $(wildcard include/config/rtk/voip/t38.h) \
    $(wildcard include/config/rtk/voip/ip/phone.h) \
    $(wildcard include/config/cwmp/tr069.h) \
    $(wildcard include/config/rtk/voip/g722/itu/use.h) \
    $(wildcard include/config/rtk/voip/rtp/redundant.h) \
    $(wildcard include/config/rtk/voip/g7111.h) \
    $(wildcard include/config/rtk/voip/drivers/si3050.h) \
  include/linux/version.h \
  rtk_voip/include/voip_debug.h \
  include/linux/kernel.h \
    $(wildcard include/config/lbd.h) \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/panic/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /home/shipeng/rtl86907/aaa/trunk/toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/bin/../lib/gcc/mips-linux/4.4.6/include/stdarg.h \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/linkage.h \
  include/linux/stddef.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/posix_types.h \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/sgidefs.h \
  include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/bitops.h \
    $(wildcard include/config/cpu/has/llsc.h) \
    $(wildcard include/config/cpu/rlx4181.h) \
    $(wildcard include/config/cpu/rlx5181.h) \
    $(wildcard include/config/cpu/has/radiax.h) \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
    $(wildcard include/config/rtl/819x.h) \
  include/linux/typecheck.h \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/irqflags.h \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/hazards.h \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cpu-features.h \
    $(wildcard include/config/cpu/has/ejtag.h) \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cpu.h \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cpu-info.h \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cache.h \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mach-generic/kmalloc.h \
    $(wildcard include/config/dma/coherent.h) \
  arch/rlx/bsp_rtl8686/bspcpu.h \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/barrier.h \
    $(wildcard include/config/cpu/has/sync.h) \
    $(wildcard include/config/cpu/has/wb.h) \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
    $(wildcard include/config/debug/bugverbose.h) \
    $(wildcard include/config/smp.h) \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/byteorder.h \
    $(wildcard include/config/cpu/big/endian.h) \
  include/linux/byteorder/big_endian.h \
  include/linux/swab.h \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/ffz.h \
  include/asm-generic/bitops/find.h \
  include/asm-generic/bitops/sched.h \
  include/asm-generic/bitops/hweight.h \
  include/asm-generic/bitops/ext2-non-atomic.h \
  include/asm-generic/bitops/le.h \
  include/asm-generic/bitops/ext2-atomic.h \
  include/asm-generic/bitops/minix.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/ratelimit.h \
  include/linux/param.h \
  /home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/param.h \
    $(wildcard include/config/hz.h) \
  include/linux/dynamic_debug.h \
  rtk_voip/include/voip_timer.h \
  rtk_voip/include/voip_init.h \
    $(wildcard include/config/defaults/kernel/2/6.h) \
    $(wildcard include/config/defaults/kernel/3/4.h) \
  include/linux/init.h \
    $(wildcard include/config/rtl8672.h) \
    $(wildcard include/config/rtl8681/ptm.h) \
    $(wildcard include/config/ethwan.h) \
    $(wildcard include/config/usb.h) \
    $(wildcard include/config/usb/rtl8187su/softap.h) \
    $(wildcard include/config/usb/rtl8192su/softap.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/linux/section-names.h \
  include/linux/stringify.h \
  rtk_voip/include/voip_debug.h \
  rtk_voip/voip_drivers/gpio/gpio.h \
    $(wildcard include/config/rtk/voip/drivers/pcm8651.h) \
    $(wildcard include/config/rtk/voip/soc/8686/cpu0.h) \
    $(wildcard include/config/rtk/voip/soc/8686/cpu1.h) \
  rtk_voip/include/voip_types.h \
    $(wildcard include/config/rtl865xb.h) \
  rtk_voip/voip_drivers/gpio/gpio_8686.h \
  rtk_voip/voip_drivers/snd_pin_cs.h \

rtk_voip/voip_drivers/snd_pin_cs.o: $(deps_rtk_voip/voip_drivers/snd_pin_cs.o)

$(deps_rtk_voip/voip_drivers/snd_pin_cs.o):
