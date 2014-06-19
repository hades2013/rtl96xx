#
# Makefile for diag driver
#
# Copyright (C) 2005 Felix Fietkau <nbd@openwrt.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

KERNEL_DIR = $(LINUXDIR)
PWD := $(shell pwd)


obj-m := diag.o

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) INSTALL_MOD_PATH="$(INSTALL_MOD_PATH)" modules_install

clean:
	rm -fr *.o
	rm -fr *.ko