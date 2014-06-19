
#
# arch/mips/boot/compressed/Makefile
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 1994 by Linus Torvalds
# Adapted for PowerPC by Gary Thomas
# modified by Cort (cort@cs.nmt.edu)
#
# Ported to MIPS by Pete Popov, ppopov@embeddedalley.com
#

boot           := arch/rlx/boot
compressed     := arch/rlx/boot/compressed

# added by t04123 for CONFIG_GCOV_ALL=y 
#CFLAGS += $(CFLAGS_NOGCOV)
#

EXTRA_CFLAGS += -D__linux__ -fno-builtin -D__BOOTER__ -I$(compressed)/include -mabicalls

BOOT_TARGETS   = zImage 

bootdir-y :=iDiSX
subdir-y                       := common lib images

.PHONY: $(BOOT_TARGETS) $(bootdir-y)

$(BOOT_TARGETS): $(bootdir-y)

$(bootdir-y): $(addprefix arch/rlx/boot/compressed/,$(subdir-y)) \
		$(addprefix arch/rlx/boot/compressed/,$(hostprogs-y))
	$(Q)$(MAKE) $(build)=$(obj)/$@ $(MAKECMDGOALS)
