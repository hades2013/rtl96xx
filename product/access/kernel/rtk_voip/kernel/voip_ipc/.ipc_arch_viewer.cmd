cmd_rtk_voip/voip_ipc/ipc_arch_viewer := gcc -Wp,-MD,rtk_voip/voip_ipc/.ipc_arch_viewer.d -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer   -Irtk_voip/include -Iinclude -include include/linux/config.h  -o rtk_voip/voip_ipc/ipc_arch_viewer rtk_voip/voip_ipc/ipc_arch_viewer.c  

deps_rtk_voip/voip_ipc/ipc_arch_viewer := \
  rtk_voip/voip_ipc/ipc_arch_viewer.c \
  include/linux/config.h \
    $(wildcard include/config/h.h) \
  /usr/include/stdio.h \
  /usr/include/features.h \
  /usr/include/sys/cdefs.h \
  /usr/include/bits/wordsize.h \
  /usr/include/gnu/stubs.h \
  /usr/include/gnu/stubs-32.h \
  /usr/lib/gcc/i686-redhat-linux/4.4.6/include/stddef.h \
  /usr/include/bits/types.h \
  /usr/include/bits/typesizes.h \
  /usr/include/libio.h \
  /usr/include/_G_config.h \
  /usr/include/wchar.h \
  /usr/lib/gcc/i686-redhat-linux/4.4.6/include/stdarg.h \
  /usr/include/bits/stdio_lim.h \
  /usr/include/bits/sys_errlist.h \
  /usr/include/bits/stdio.h \
  /usr/include/signal.h \
  /usr/include/bits/sigset.h \
  /usr/include/bits/signum.h \
  /usr/include/time.h \
  /usr/include/bits/siginfo.h \
  /usr/include/bits/sigaction.h \
  /usr/include/bits/sigcontext.h \
  /usr/include/asm/sigcontext.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbd.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /usr/include/asm/types.h \
  /usr/include/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /usr/include/asm/posix_types.h \
  /usr/include/asm/posix_types_32.h \
  /usr/include/bits/sigstack.h \
  /usr/include/sys/ucontext.h \
  /usr/include/bits/pthreadtypes.h \
  /usr/include/bits/sigthread.h \
  /usr/include/stdlib.h \
  /usr/include/bits/waitflags.h \
  /usr/include/bits/waitstatus.h \
  /usr/include/endian.h \
  /usr/include/bits/endian.h \
  /usr/include/bits/byteswap.h \
  /usr/include/sys/types.h \
  /usr/include/sys/select.h \
  /usr/include/bits/select.h \
  /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h \
  /usr/include/alloca.h \
  /usr/include/netinet/in.h \
  /usr/include/stdint.h \
  /usr/include/bits/wchar.h \
  /usr/include/sys/socket.h \
  /usr/include/sys/uio.h \
  /usr/include/bits/uio.h \
  /usr/include/bits/socket.h \
  /usr/include/bits/sockaddr.h \
  /usr/include/asm/socket.h \
  /usr/include/asm-generic/socket.h \
  /usr/include/asm/sockios.h \
  /usr/include/asm-generic/sockios.h \
  /usr/include/bits/in.h \
  rtk_voip/voip_ipc/ipc_internal.h \
  rtk_voip/include/voip_types.h \
    $(wildcard include/config/rtl865xb.h) \
  rtk_voip/include/voip_ipc.h \
    $(wildcard include/config/rtk/voip/ipc/arch.h) \
  rtk_voip/include/voip_types.h \
  rtk_voip/include/voip_debug.h \

rtk_voip/voip_ipc/ipc_arch_viewer: $(deps_rtk_voip/voip_ipc/ipc_arch_viewer)

$(deps_rtk_voip/voip_ipc/ipc_arch_viewer):
