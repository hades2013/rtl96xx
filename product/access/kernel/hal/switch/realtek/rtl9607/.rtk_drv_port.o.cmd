cmd_drivers/lwdrv/hal/switch/realtek/rtl9607/rtk_drv_port.o := rsdk-linux-gcc -Wp,-MD,drivers/lwdrv/hal/switch/realtek/rtl9607/.rtk_drv_port.o.d  -nostdinc -isystem /home/wanghuanyu/EPN/ZG/hexicom/trunk/toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/bin/../lib/gcc/mips-linux/4.4.6/include -Iinclude  -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include -include include/linux/autoconf.h -Iinclude/soc -D__KERNEL__ -D"VMLINUX_LOAD_ADDRESS=0x80041000" -D"LOADADDR=0x80041000" -D"DATAOFFSET=0" -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-delete-null-pointer-checks -D__LUNA_KERNEL__ -Os -fno-inline -ffunction-sections -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -EB -UMIPSEB -U_MIPSEB -U__MIPSEB -U__MIPSEB__ -UMIPSEL -U_MIPSEL -U__MIPSEL -U__MIPSEL__ -DMIPSEB -D_MIPSEB -D__MIPSEB -D__MIPSEB__ -Iinclude/asm-rlx -Iarch/rlx/bsp_rtl8686/ -Idrivers/net/rtl86900/sdk/include/ -Idrivers/net/rtl86900/sdk/system/include -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-dwarf2-cfi-asm -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../kernel/linux-2.6.x/include/linux/ -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../kernel/drv/hal/switch/ -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../app/include/ -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/include/ -I/ -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/sdk/realtek/sdk-1.0.0/include/ -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/sdk/realtek/sdk-1.0.0/system/include -I/home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../kernel/linux-2.6.x/drivers/net/   -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(rtk_drv_port)"  -D"KBUILD_MODNAME=KBUILD_STR(rtk_drv_port)"  -c -o drivers/lwdrv/hal/switch/realtek/rtl9607/rtk_drv_port.o drivers/lwdrv/hal/switch/realtek/rtl9607/rtk_drv_port.c

deps_drivers/lwdrv/hal/switch/realtek/rtl9607/rtk_drv_port.o := \
  drivers/lwdrv/hal/switch/realtek/rtl9607/rtk_drv_port.c \
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
  include/linux/delay.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/delay.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../app/include/lw_type.h \
    $(wildcard include/config/product/epn104.h) \
    $(wildcard include/config/product/epn204.h) \
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
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../app/include/lw_drv_pub.h \
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
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/include/pdt_config.h \
    $(wildcard include/config/h/.h) \
    $(wildcard include/config/isouservlan.h) \
    $(wildcard include/config/hard/version.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/include/lw_config.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/include/pdt_priv_config.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../app/include/lw_drv_req.h \
    $(wildcard include/config/bosa.h) \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/string.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../kernel/drv/hal/switch/hal_common.h \
  drivers/net/rtl86900/sdk/include/common/rt_type.h \
  drivers/net/rtl86900/sdk/system/include/common/type.h \
  include/linux/spinlock_types.h \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/spinlock_types_up.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/generic/hardirqs.h) \
  include/linux/spinlock.h \
    $(wildcard include/config/preempt.h) \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/thread_info.h \
    $(wildcard include/config/kernel/stack/size/order.h) \
    $(wildcard include/config/debug/stack/usage.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/processor.h \
    $(wildcard include/config/cpu/has/sleep.h) \
  include/linux/cpumask.h \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/bitmap.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cachectl.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/rlxregs.h \
    $(wildcard include/config/cpu/rlx5281.h) \
    $(wildcard include/config/cpu/rlx4281.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/system.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/addrspace.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mach-generic/spaces.h \
    $(wildcard include/config/32bit.h) \
    $(wildcard include/config/dma/noncoherent.h) \
  include/linux/const.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cmpxchg.h \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
  include/linux/prefetch.h \
  include/linux/stringify.h \
  include/linux/bottom_half.h \
  include/linux/spinlock_up.h \
  include/linux/spinlock_api_up.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/atomic.h \
  include/asm-generic/atomic-long.h \
  include/linux/slab.h \
    $(wildcard include/config/slab/debug.h) \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/slob.h) \
    $(wildcard include/config/debug/slab.h) \
  include/linux/gfp.h \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/highmem.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/arch/populates/node/map.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/cgroup/mem/res/ctlr.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  include/linux/wait.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/current.h \
  include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
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
  include/linux/seqlock.h \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/linux/bounds.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/page.h \
  include/linux/pfn.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/io.h \
  include/asm-generic/iomap.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/pgtable-bits.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mach-generic/ioremap.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mach-generic/mangle-port.h \
    $(wildcard include/config/swap/io/space.h) \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/sparsemem/vmemmap.h) \
  include/asm-generic/getorder.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/memory/hotremove.h) \
  include/linux/notifier.h \
  include/linux/errno.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/errno.h \
  include/asm-generic/errno-base.h \
  include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/linux/rwsem-spinlock.h \
  include/linux/srcu.h \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
  include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/topology.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mach-generic/topology.h \
  include/asm-generic/topology.h \
  include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/debug/virtual.h) \
  include/linux/slab_def.h \
    $(wildcard include/config/kmemtrace.h) \
  include/trace/kmemtrace.h \
  include/linux/tracepoint.h \
    $(wildcard include/config/tracepoints.h) \
  include/linux/rcupdate.h \
    $(wildcard include/config/classic/rcu.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/preempt/rcu.h) \
  include/linux/completion.h \
  include/linux/rcuclassic.h \
    $(wildcard include/config/rcu/cpu/stall/detector.h) \
  include/linux/kmalloc_sizes.h \
  drivers/net/rtl86900/sdk/system/include/common/error.h \
  drivers/net/rtl86900/sdk/include/common/rt_error.h \
  drivers/net/rtl86900/sdk/include/hal/phy/phydef.h \
  drivers/net/rtl86900/sdk/system/include/common/rt_autoconf.h \
  drivers/net/rtl86900/sdk/include/rtk/port.h \
  drivers/net/rtl86900/sdk/include/rtk/vlan.h \
  drivers/net/rtl86900/sdk/include/hal/common/halctrl.h \
  drivers/net/rtl86900/sdk/include/common/util/rt_bitop.h \
  drivers/net/rtl86900/sdk/include/common/util/rt_util.h \
  drivers/net/rtl86900/sdk/system/include/common/util.h \
  drivers/net/rtl86900/sdk/system/include/osal/lib.h \
    $(wildcard include/config/sdk/kernel/linux.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../kernel/linux-2.6.x/include/linux/ctype.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/../../kernel/linux-2.6.x/include/linux/string.h \
  drivers/net/rtl86900/sdk/include/hal/chipdef/driver.h \
    $(wildcard include/config/sdk/apollo.h) \
    $(wildcard include/config/sdk/apollomp.h) \
    $(wildcard include/config/sdk/rtl9601b.h) \
  drivers/net/rtl86900/sdk/include/hal/chipdef/allreg.h \
    $(wildcard include/config/sdk/reg/dflt/val.h) \
  drivers/net/rtl86900/sdk/include/hal/chipdef/allmem.h \
  drivers/net/rtl86900/sdk/include/hal/chipdef/chip.h \
  drivers/net/rtl86900/sdk/include/rtk/qos.h \
  drivers/net/rtl86900/sdk/include/rtk/switch.h \
  drivers/net/rtl86900/sdk/include/rtk/led.h \
    $(wildcard include/config/col.h) \
    $(wildcard include/config/tx/act.h) \
    $(wildcard include/config/rx/act.h) \
    $(wildcard include/config/spd10act.h) \
    $(wildcard include/config/spd100act.h) \
    $(wildcard include/config/spd500act.h) \
    $(wildcard include/config/spd1000act.h) \
    $(wildcard include/config/dup.h) \
    $(wildcard include/config/spd10.h) \
    $(wildcard include/config/spd100.h) \
    $(wildcard include/config/spd500.h) \
    $(wildcard include/config/spd1000.h) \
    $(wildcard include/config/force/mode.h) \
    $(wildcard include/config/pon/link.h) \
    $(wildcard include/config/soc/link/ack.h) \
    $(wildcard include/config/pon/alarm.h) \
    $(wildcard include/config/pon/warning.h) \
    $(wildcard include/config/end.h) \
  include/linux/kthread.h \
  include/linux/err.h \
  include/linux/sched.h \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/no/hz.h) \
    $(wildcard include/config/detect/softlockup.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/posix/mqueue.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/user/sched.h) \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/x86/ptrace/bts.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/rtl/timer2.h) \
    $(wildcard include/config/preempt/bkl.h) \
    $(wildcard include/config/group/sched.h) \
    $(wildcard include/config/mm/owner.h) \
  include/linux/capability.h \
    $(wildcard include/config/security/file/capabilities.h) \
  include/linux/timex.h \
  include/linux/time.h \
  include/linux/math64.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/div64.h \
  include/asm-generic/div64.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/timex.h \
  include/linux/jiffies.h \
  include/linux/rbtree.h \
  include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/mmu/notifier.h) \
  include/linux/auxvec.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/auxvec.h \
  include/linux/prio_tree.h \
  include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mmu.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/ptrace.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/isadep.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/cputime.h \
  include/asm-generic/cputime.h \
  include/linux/sem.h \
  include/linux/ipc.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/ipcbuf.h \
  include/linux/kref.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/sembuf.h \
  include/linux/signal.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/signal.h \
    $(wildcard include/config/trad/signals.h) \
  include/asm-generic/signal-defs.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/sigcontext.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/siginfo.h \
  include/asm-generic/siginfo.h \
  include/linux/path.h \
  include/linux/pid.h \
  include/linux/percpu.h \
    $(wildcard include/config/have/dynamic/per/cpu/area.h) \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/percpu.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  include/linux/percpu-defs.h \
  include/linux/proportions.h \
  include/linux/percpu_counter.h \
  include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
  include/linux/rtmutex.h \
    $(wildcard include/config/debug/rt/mutexes.h) \
  include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  include/linux/resource.h \
  /home/wanghuanyu/EPN/ZG/hexicom/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/resource.h \
  include/asm-generic/resource.h \
  include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects/free.h) \
  include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  include/linux/kobject.h \
  include/linux/sysfs.h \
  include/linux/latencytop.h \
  include/linux/cred.h \
    $(wildcard include/config/security.h) \
  include/linux/key.h \
    $(wildcard include/config/sysctl.h) \
  include/linux/sysctl.h \
    $(wildcard include/config/rtl/nf/conntrack/garbage/new.h) \
  include/linux/aio.h \
    $(wildcard include/config/aio.h) \
  include/linux/workqueue.h \
  include/linux/aio_abi.h \
  include/linux/uio.h \

drivers/lwdrv/hal/switch/realtek/rtl9607/rtk_drv_port.o: $(deps_drivers/lwdrv/hal/switch/realtek/rtl9607/rtk_drv_port.o)

$(deps_drivers/lwdrv/hal/switch/realtek/rtl9607/rtk_drv_port.o):
