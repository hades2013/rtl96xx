cmd_drivers/lwdrv/ksal/switch/ksal_ioctl.o := rsdk-linux-gcc -Wp,-MD,drivers/lwdrv/ksal/switch/.ksal_ioctl.o.d  -nostdinc -isystem /home/wanghuanyu/EPN/ZG/hexicom/trunk/toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/bin/../lib/gcc/mips-linux/4.4.6/include -Iinclude  -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include -include include/linux/autoconf.h -Iinclude/soc -D__KERNEL__ -D"VMLINUX_LOAD_ADDRESS=0x80041000" -D"LOADADDR=0x80041000" -D"DATAOFFSET=0" -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-delete-null-pointer-checks -D__LUNA_KERNEL__ -Os -fno-inline -ffunction-sections -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -EB -UMIPSEB -U_MIPSEB -U__MIPSEB -U__MIPSEB__ -UMIPSEL -U_MIPSEL -U__MIPSEL -U__MIPSEL__ -DMIPSEB -D_MIPSEB -D__MIPSEB -D__MIPSEB__ -Iinclude/asm-rlx -Iarch/rlx/bsp_rtl8686/ -Idrivers/net/rtl86900/sdk/include/ -Idrivers/net/rtl86900/sdk/system/include -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-dwarf2-cfi-asm -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/include -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../app/include -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../kernel/drv/hal/switch/ -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/sdk/realtek/sdk-1.0.0/include/ -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/sdk/realtek/sdk-1.0.0/system/include/   -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(ksal_ioctl)"  -D"KBUILD_MODNAME=KBUILD_STR(ksal_ioctl)"  -c -o drivers/lwdrv/ksal/switch/ksal_ioctl.o drivers/lwdrv/ksal/switch/ksal_ioctl.c

deps_drivers/lwdrv/ksal/switch/ksal_ioctl.o := \
  drivers/lwdrv/ksal/switch/ksal_ioctl.c \
    $(wildcard include/config/l2/handle.h) \
    $(wildcard include/config/bosa.h) \
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
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/bin/../lib/gcc/mips-linux/4.4.6/include/stdarg.h \
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
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/linkage.h \
  include/linux/stddef.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/posix_types.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/sgidefs.h \
  include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/bitops.h \
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
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/irqflags.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/hazards.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cpu-features.h \
    $(wildcard include/config/cpu/has/ejtag.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cpu.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cpu-info.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cache.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mach-generic/kmalloc.h \
    $(wildcard include/config/dma/coherent.h) \
  arch/rlx/bsp_rtl8686/bspcpu.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/barrier.h \
    $(wildcard include/config/cpu/has/sync.h) \
    $(wildcard include/config/cpu/has/wb.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
    $(wildcard include/config/debug/bugverbose.h) \
    $(wildcard include/config/smp.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/byteorder.h \
    $(wildcard include/config/cpu/big/endian.h) \
  include/linux/byteorder/big_endian.h \
  include/linux/swab.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/swab.h \
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
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/param.h \
    $(wildcard include/config/hz.h) \
  include/linux/dynamic_debug.h \
  arch/rlx/bsp_rtl8686/bspchip.h \
    $(wildcard include/config/rtl8686.h) \
  arch/rlx/bsp_rtl8686/bspchip_8686.h \
    $(wildcard include/config/use/uart1.h) \
    $(wildcard include/config/use/uart0.h) \
    $(wildcard include/config/rtl8192cd.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../kernel/drv/hal/switch/hal_common.h \
    $(wildcard include/config/s.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/include/lw_config.h \
    $(wildcard include/config/h.h) \
    $(wildcard include/config/product/epn105.h) \
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
  drivers/net/rtl86900/sdk/include/common/rt_type.h \
  drivers/net/rtl86900/sdk/system/include/common/type.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../app/include/lw_type.h \
    $(wildcard include/config/product/epn104.h) \
    $(wildcard include/config/product/epn204.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../app/include/lw_drv_pub.h \
    $(wildcard include/config/product/epn104n.h) \
    $(wildcard include/config/product/epn104w.h) \
    $(wildcard include/config/product/epn104zg.h) \
    $(wildcard include/config/product/epn104zg/a.h) \
    $(wildcard include/config/product/gpn104n.h) \
    $(wildcard include/config/product/epn101r.h) \
    $(wildcard include/config/product/epn101zg.h) \
    $(wildcard include/config/product/5500.h) \
    $(wildcard include/config/schedule/error.h) \
    $(wildcard include/config/schedule/finish.h) \
    $(wildcard include/config/schedule/pvlan.h) \
    $(wildcard include/config/schedule/return/sum/page.h) \
    $(wildcard include/config/wdt.h) \
    $(wildcard include/config/dhcpsp.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/include/pdt_config.h \
    $(wildcard include/config/h/.h) \
    $(wildcard include/config/isouservlan.h) \
    $(wildcard include/config/hard/version.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/include/lw_config.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/include/pdt_priv_config.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../app/include/lw_drv_req.h \

drivers/lwdrv/ksal/switch/ksal_ioctl.o: $(deps_drivers/lwdrv/ksal/switch/ksal_ioctl.o)

$(deps_drivers/lwdrv/ksal/switch/ksal_ioctl.o):
