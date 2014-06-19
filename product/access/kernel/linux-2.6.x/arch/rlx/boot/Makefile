#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 1995, 1998, 2001, 2002 by Ralf Baechle
# Copyright (C) 2004  Maciej W. Rozycki
#

E2EFLAGS =

#
# Drop some uninteresting sections in the kernel.
# This is only relevant for ELF kernels but doesn't hurt a.out
#
drop-sections	= .reginfo .mdebug .comment .note .pdr .options .MIPS.options
strip-flags	= $(addprefix --remove-section=,$(drop-sections))

VMLINUX = vmlinux

#all: vmlinux.ecoff vmlinux.srec

#Begin add by shipeng 2013-03-22
ZBOOT_TARGETS  = zImage
bootdir-y      := compressed
all:zImage
#End add by shipeng 2013-03-22

vmlinux.ecoff: $(obj)/elf2ecoff $(VMLINUX)
	$(obj)/elf2ecoff $(VMLINUX) vmlinux.ecoff $(E2EFLAGS)

$(obj)/elf2ecoff: $(obj)/elf2ecoff.c
	$(HOSTCC) -o $@ $^

vmlinux.bin: $(VMLINUX)
	$(OBJCOPY) -O binary $(strip-flags) $(VMLINUX) $(obj)/vmlinux.bin

vmlinux.srec: $(VMLINUX)
	$(OBJCOPY) -S -O srec $(strip-flags) $(VMLINUX) $(obj)/vmlinux.srec

clean-files += elf2ecoff \
	       vmlinux.bin \
	       vmlinux.ecoff \
	       vmlinux.srec
#Begin add by shipeng 2013-03-22
.PHONY: $(ZBOOT_TARGETS) $(bootdir-y)
$(ZBOOT_TARGETS): $(bootdir-y)
$(bootdir-y): $(addprefix $(obj)/,$(subdir-y)) $(addprefix $(obj)/,$(hostprogs-y))
	echo $(MAKECMDGOALS)
	$(Q)$(MAKE) $(build)=$(obj)/$@ $(MAKECMDGOALS)
#End add by shipeng 2013-03-22