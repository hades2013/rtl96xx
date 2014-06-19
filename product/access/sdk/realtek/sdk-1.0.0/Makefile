#
# Makefile for the Linux Wireless network device drivers.
#

include $(APOLLODIR)/Compiler_Flag
SDKDIR = $(APOLLODIR)/sdk
SDK_RTCOREDIR = $(SDKDIR)/system/linux/rtcore
SDK_RTKDIR    = $(SDKDIR)/src
SDK_UTDIR     = $(SDKDIR)/unittest
SDK_RTDRVDIR  = $(SDKDIR)/system/linux/rtdrv
SDK_INCLUDE   = $(SDKDIR)/include
SYS_INCLUDE   = $(SDKDIR)/system/include
DRV_INCLUDE   = $(SDKDIR)/system/linux
UT_INCLUDE    = $(SDKDIR)/unittest
SDK_MODULE    = $(SDKDIR)/src/module
SDK_ETCDIR    = $(SDKDIR)/rtk.etc

ifeq ($(CONFIG_DIAGSHELL_MODE),CONFIG_DIAGSHELL_RTK)
DIAGSHELL_DIR := $(SDKDIR)/src/app/diag_rtk
endif

ifeq ($(CONFIG_DIAGSHELL_MODE),CONFIG_DIAGSHELL_RAW)
DIAGSHELL_DIR := $(SDKDIR)/src/app/diag
endif


CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE=y


export SDKDIR SDK_INCLUDE SYS_INCLUDE DRV_INCLUDE UT_INCLUDE
export CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE


obj-y += src/
obj-y += unittest/
obj-y += system/linux/rtcore/
obj-y += system/linux/rtdrv/
ifeq ($(CONFIG_SDK_MODULES),y)
obj-y += src/module/
endif


all:
	$(MAKE) -C $(SDK_MODULE)    SRC_DIR=$(SDK_MODULE)       all;

install:
	$(MAKE) -C $(SDK_MODULE)    SRC_DIR=$(SDK_MODULE)       install;


ifeq ($(CONFIG_RTK_L34_ENABLE),y)
DEF += -D CONFIG_RTK_L34_ENABLE
endif


clean:
	$(MAKE) -C $(SDK_RTKDIR) clean
	$(MAKE) -C $(SDK_RTCOREDIR) clean
	$(MAKE) -C $(SDK_UTDIR) clean
	rm -rf *.o


shell_tree:
	cd $(DIAGSHELL_DIR)/config; \
        ./script/mk_parser.py $(DEF) diag_shell.cli -fd ../include/flag/ -f cmd_flag.h; \
        mv ../config/cparser_tree.c ../src/parser/cparser_tree.c; \
        mv ../config/cparser_tree.h ../include/parser/cparser_tree.h; \
        cd - ;

