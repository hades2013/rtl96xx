#
# Copyright(C) 2010 Hexicom
#

include $(PRODUCTDIR)/.config

include rules/config.mk

TOPDIR := $(shell pwd)
TARGETDIR :=$(ROOTFSDIR)
export TOPDIR TARGETDIR


ifeq (y,$(CONFIG_EOC_EXTEND))

CONFIG_BUSYBOX = y
CONFIG_SHARED = y
CONFIG_NVRAM = n
CONFIG_VTYSH = n
CONFIG_HTTPD = n
CONFIG_SNMPD = n
CONFIG_LIB8367M = n
CONFIG_MASTER = n
CONFIG_UTILS = n
CONFIG_WTD = n
CONFIG_NTPCLIENT = n
CONFIG_LOOPD = y

else

CONFIG_BUSYBOX = y
CONFIG_SHARED = y
CONFIG_NVRAM = y
#CONFIG_VTYSH = y
#CONFIG_HTTPD = y
#CONFIG_SNMPD = y
CONFIG_LIB8367M = y
CONFIG_MASTER = y
CONFIG_UTILS = y
CONFIG_WTD = y
CONFIG_NTPCLIENT = y
CONFIG_LOOPD = y

endif




















obj-$(CONFIG_LIB8367M) += lib8367m
obj-$(CONFIG_SHARED) += shared
obj-$(CONFIG_BUSYBOX) += busybox

obj-$(CONFIG_SSHD) += sshd 
obj-$(CONFIG_NVRAM) += nvram
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
obj-$(CONFIG_MASTER) += master
obj-$(CONFIG_UTILS) += utils
obj-$(CONFIG_WTD) += watchdog
obj-$(CONFIG_NTPCLIENT) += ntpclient
obj-$(CONFIG_LOOPD) += loopd


obj-build := $(foreach obj,$(obj-y),$(obj)-build)
obj-install := $(foreach obj,$(obj-y),$(obj)-install)
obj-clean := $(foreach obj,$(obj-y),$(obj)-clean)
includelist :=$(foreach obj,$(obj-y) $(obj-n),rules/$(obj).mk)
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
