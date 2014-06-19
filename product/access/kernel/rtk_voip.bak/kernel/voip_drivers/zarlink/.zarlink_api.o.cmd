cmd_rtk_voip/voip_drivers/zarlink/zarlink_api.o := rsdk-linux-gcc -Wp,-MD,rtk_voip/voip_drivers/zarlink/.zarlink_api.o.d  -nostdinc -isystem /home/shipeng/EPN104N/trunk/toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/bin/../lib/gcc/mips-linux/4.4.6/include -Iinclude  -I/home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include -include include/linux/autoconf.h -Iinclude/soc -D__KERNEL__ -D"VMLINUX_LOAD_ADDRESS=0x80041000" -D"LOADADDR=0x80041000" -D"DATAOFFSET=0" -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-delete-null-pointer-checks -D__LUNA_KERNEL__ -Os -fno-inline -ffunction-sections -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -EB -UMIPSEB -U_MIPSEB -U__MIPSEB -U__MIPSEB__ -UMIPSEL -U_MIPSEL -U__MIPSEL -U__MIPSEL__ -DMIPSEB -D_MIPSEB -D__MIPSEB -D__MIPSEB__ -Iinclude/asm-rlx -Iarch/rlx/bsp_rtl8686/ -I/home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-dwarf2-cfi-asm -Irtk_voip/voip_drivers/zarlink/ve890/ -Irtk_voip/voip_drivers/zarlink/api_lib-2.16.1/vp890_api/ -Irtk_voip/voip_drivers/zarlink/ -Irtk_voip/voip_drivers/zarlink/common/ -Irtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/ -Irtk_voip/voip_drivers/zarlink/api_lib-2.16.1/profiles/ -Irtk_voip/voip_drivers/zarlink/api_lib-2.16.1/arch/rtl89xxb/ -Irtk_voip/include/ -Irtk_voip/voip_drivers/ -Irtk_voip/voip_drivers/spi/   -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(zarlink_api)"  -D"KBUILD_MODNAME=KBUILD_STR(zarlink_api)"  -c -o rtk_voip/voip_drivers/zarlink/zarlink_api.o rtk_voip/voip_drivers/zarlink/zarlink_api.c

deps_rtk_voip/voip_drivers/zarlink/zarlink_api.o := \
  rtk_voip/voip_drivers/zarlink/zarlink_api.c \
  rtk_voip/voip_drivers/zarlink/common/zarlinkCommonSlic.h \
    $(wildcard include/config/rtk/voip/drivers/slic/zarlink/on/new/arch.h) \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/arch/rtl89xxb/vp_api_types.h \
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
  include/linux/config.h \
    $(wildcard include/config/h.h) \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/arch/rtl89xxb/vp_api_profile_type.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_cfg.h \
    $(wildcard include/config/rtk/voip/slic/zarlink/880/series.h) \
    $(wildcard include/config/rtk/voip/slic/zarlink/890/series.h) \
  include/linux/kernel.h \
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
  /home/shipeng/EPN104N/trunk/toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/bin/../lib/gcc/mips-linux/4.4.6/include/stdarg.h \
  include/linux/linkage.h \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/linkage.h \
  include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/bitops.h \
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
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/irqflags.h \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/hazards.h \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cpu-features.h \
    $(wildcard include/config/cpu/has/ejtag.h) \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cpu.h \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cpu-info.h \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cache.h \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mach-generic/kmalloc.h \
    $(wildcard include/config/dma/coherent.h) \
  arch/rlx/bsp_rtl8686/bspcpu.h \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/barrier.h \
    $(wildcard include/config/cpu/has/sync.h) \
    $(wildcard include/config/cpu/has/wb.h) \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/break.h \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
    $(wildcard include/config/debug/bugverbose.h) \
    $(wildcard include/config/smp.h) \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/byteorder.h \
    $(wildcard include/config/cpu/big/endian.h) \
  include/linux/byteorder/big_endian.h \
  include/linux/swab.h \
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/swab.h \
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
  /home/shipeng/EPN104N/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/param.h \
    $(wildcard include/config/hz.h) \
  include/linux/dynamic_debug.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_cfg_int.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_option.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_event.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_lib_cfg.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_common.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_dev_term.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_test.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_cslac_seq.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp890_api.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/arch/rtl89xxb/vp_hal.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/arch/rtl89xxb/vp_api_types.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/arch/rtl89xxb/hbi_hal.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/arch/rtl89xxb/mpi_hal.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_dev_term.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/arch/rtl89xxb/sys_service.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_CSLAC_types.h \
    $(wildcard include/config/fr.h) \
    $(wildcard include/config/prof/table/size.h) \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_fxo_params.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_timer.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/profiles/Ve_profile.h \
    $(wildcard include/config/rtk/voip/slic/zarlink/890/series/wideband.h) \
    $(wildcard include/config/board/011.h) \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_common.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/profiles/Le890NB_LVHV.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/profiles/Le890WB_HV.h \
  rtk_voip/voip_drivers/zarlink/common/zarlinkCommon.h \
    $(wildcard include/config/rtk/voip/slic/num.h) \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_int.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_debug.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/arch/rtl89xxb/sys_service.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_debug_colors.h \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/vp890_api/vp890_api_int.h \
    $(wildcard include/config/wrt.h) \
    $(wildcard include/config/rd.h) \
    $(wildcard include/config/len.h) \
  rtk_voip/voip_drivers/zarlink/api_lib-2.16.1/includes/vp_api_option.h \
  rtk_voip/include/voip_debug.h \
  rtk_voip/include/voip_timer.h \
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
    $(wildcard include/config/rtk/voip/slic/si32176/nr.h) \
    $(wildcard include/config/rtk/voip/slic/si32176/cs/nr.h) \
    $(wildcard include/config/rtk/voip/slic/si32178/nr.h) \
    $(wildcard include/config/rtk/voip/slic/si32176/si32178/nr.h) \
    $(wildcard include/config/rtk/voip/slic/si3226/nr.h) \
    $(wildcard include/config/rtk/voip/slic/si3226x/nr.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/le88221/nr.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/le88111/nr.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/le89116/nr.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/le89316/nr.h) \
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
    $(wildcard include/config/rtk/voip/rtp/redundant.h) \
    $(wildcard include/config/rtk/voip/g7111.h) \
    $(wildcard include/config/rtk/voip/drivers/si3050.h) \
  rtk_voip/include/voip_debug.h \
  rtk_voip/include/voip_timer.h \
  rtk_voip/voip_drivers/snd_define.h \
  rtk_voip/voip_drivers/zarlink/common/zarlinkCommonDaa.h \

rtk_voip/voip_drivers/zarlink/zarlink_api.o: $(deps_rtk_voip/voip_drivers/zarlink/zarlink_api.o)

$(deps_rtk_voip/voip_drivers/zarlink/zarlink_api.o):
