#
# Copyright(C) 2010 Hexicom
#

include $(PRODUCTDIR)/.config

include rules/config.mk

TOPDIR := $(shell pwd)
TARGETDIR :=$(ROOTFSDIR)
export TOPDIR TARGETDIR

























#obj-y += lib8367m
obj-y += shared
#obj-$(CONFIG_BUSYBOX) += busybox
obj-y += busybox
obj-$(CONFIG_SSHD) += sshd 
#obj-$(CONFIG_NVRAM) += nvram
#obj-y += nvram
obj-$(CONFIG_VTYSH) += vtysh
obj-$(CONFIG_OPCONN) += opconn
obj-$(CONFIG_HTTPD) += webs
#obj-$(CONFIG_HTTPD) += matrixssl
obj-$(CONFIG_HTTPD) += www
#obj-$(CONFIG_SNMPD) += snmpd
#obj-$(CONFIG_BOSA) += ponmonitor
obj-$(CONFIG_PONMONITOR) += ponmonitor
obj-$(CONFIG_CATVCOM) += catvcom
obj-y += master
#obj-y += utils
obj-y += watchdog
obj-y += ntpclient
obj-$(CONFIG_LOOPD) += loopd
#obj-y += www
obj-y += test
obj-y += iproute2
obj-y += snmpd
obj-y += auth
obj-y += wireless_tools

obj-build := $(foreach obj,$(obj-y) $(obj-n),$(obj)-build)
obj-install := $(foreach obj,$(obj-y) $(obj-n),$(obj)-install)
obj-clean := $(foreach obj,$(obj-y) $(obj-n),$(obj)-clean)
includelist :=$(foreach obj,$(obj-y),rules/$(obj).mk)
includelist += rules/ifm.mk
includelist += rules/cfg.mk
include $(includelist)
#target_build: buildinfo  vendor $(obj-build) linux-modules-build 
#target_build: buildinfo vendor $(obj-build) dbg
target_build: $(obj-build)

target_install: $(obj-install)

target_clean: $(obj-clean)

all: target_build

install: target_install

clean: target_clean






	
#include rules/busybox.mk
#include rules/nvram.mk
#include rules/lib.mk
#include rules/snmpd.mk
#include rules/vtysh.mk
#include rules/httpd.mk
#include rules/lib8367m.mk
#include rules/master.mk
#include rules/shared.mk
#include rules/webs.mk
#include rules/www.mk
#include rules/utils.mk
#include rules/ntpclient.mk
