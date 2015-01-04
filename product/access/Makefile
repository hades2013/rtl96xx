
ifeq (.config,$(wildcard .config))
include .config
endif
ifeq (.env,$(wildcard .env))
include .env
-include $(shell pwd)/config/$(ENV_PRO)/config.mk
-include $(shell pwd)/config/$(ENV_PRO)/rule.mk
-include $(shell pwd)/config/$(ENV_PRO)/main.config
endif

SOFTWAREDIR=$(shell pwd)/../../app
ALL_PRODUCT=EPN104 EPN204 EPON HCEPN104 HSEPN104 EPN104N 5500 EPN104W EPN101R EPN101ZG EPN104ZG EPN105 GPN104N EPN104ZG_A
TOPDIR=$(shell pwd)
PRODUCTDIR=$(TOPDIR)
CONFIGDIR=$(TOPDIR)/config/$(ENV_PRO)
ROOTFSDIR=$(TOPDIR)/rootfs
ROMFSDIR = $(ROOTFSDIR)
SCRIPTDIR=$(TOPDIR)/../../scripts
PDT_INC_DIR=$(TOPDIR)/include
PLAT_INC_DIR=$(SOFTWAREDIR)/include
KERNELDIR=$(TOPDIR)/../../kernel
SDK_VERSION=$(shell echo ${CONFIG_SDK} | sed s/\"//g)
SDKS_DIR=$(PRODUCTDIR)/sdk
VENDOR=$(shell echo ${CONFIG_VENDOR} | sed s/\"//g)
SDKDIR=$(SDKS_DIR)/$(VENDOR)/$(SDK_VERSION)
TOOLS_DIR=$(TOPDIR)/tools

export SOFTWAREDIR ENV_PRO PDT_INC_DIR PRODUCTDIR ROOTFSDIR CONFIGDIR PLAT_INC_DIR KERNELDIR SDKDIR VENDOR CPUVENDOR CPUCHIPID TOOLS_DIR ROMFSDIR SWITCHID
ALL_PRODUCT_ENVINIT=$(foreach n,$(ALL_PRODUCT),$(n)-init)
.PHONY: $(ALL_PRODUCT) $(ALL_PRODUCT_ENVINIT) all uboot software app image cleandist clean ramfs linux linux-clean software-clean uboot-clean \
menuconfig rootfs release distclean setup platform.setup product.setup platform.distclean product.distclean

$(ALL_PRODUCT):
	echo "tet"
	echo ENV_PRO=$@ > .env
	$(MAKE) -C ../access setup
	$(MAKE) -C ../access clean
#	$(MAKE) -C ../access uboot
	$(MAKE) -C ../access all 
	
all: uboot linux linux-modules ifm-modules software software-cfg software-ifm rootfs app image ramfs
	@echo "make all finish....."
	
setup:onu-support platform.setup product.setup

onu-support: software-cfg-clean software-ifm-clean software-cfg software-ifm ifm-modules

software:
	$(MAKE) -C $(SOFTWAREDIR) all
software-clean:
	$(MAKE) -C $(SOFTWAREDIR) clean
software-%:
	@if echo $@ | egrep "clean" > /dev/null;then \
		if	[  -f $(SOFTWAREDIR)/rules/$(patsubst %-clean,%.mk,$(patsubst software-%,%,$@)) ] ; then \
			$(MAKE) -C $(SOFTWAREDIR) $(patsubst %-clean,%,$(patsubst software-%,%,$@))-clean;\
		fi \
	else \
		echo $@;\
		$(MAKE) -C $(SOFTWAREDIR) $(patsubst software-%,%,$@)-build ;\
	fi	
	
release.pre:
	if [ ! -d $(TOPDIR)/release/$(ENV_PRO) ] ;then\
		mkdir $(TOPDIR)/release/$(ENV_PRO);\
	fi
release:release.pre app image
	
clean:uboot-clean ifm-modules-clean linux-modules-clean linux-clean software-clean squashfs-clean
#	rm -rf $(PRODUCTDIR)/rootfs
	@echo "make clean finish ...."

distclean:clean product.distclean platform.distclean
	@echo "make distclean finish ...."
	
ifm-modules:
	$(MAKE) -C $(LINUXDIR) SUBDIRS=$(KERNELDIR)/lwmodule/ifm modules
	cp $(KERNELDIR)/lwmodule/ifm/ifm.ko $(CONFIGDIR)/ifm.ko -f
ifm-modules-clean:
	$(MAKE) -C $(LINUXDIR) SUBDIRS=$(KERNELDIR)/lwmodule/ifm clean
	
.PHONY: menuconfig

INIT_KCONFIG=./config.in
conf:
	$(Q)$(MAKE) -C $(SCRIPTDIR) conf
	-@if [ ! -f .config ] ; then \
		touch .config; \
	fi
mconf:
	$(Q)$(MAKE) -C $(SCRIPTDIR) ncurses conf mconf
	-@if [ ! -f .config ] ; then \
		touch .config; \
	fi
menuconfig:  mconf $(INIT_KCONFIG)
	@[ -f .config ] || $(MAKE) $(MAKEFLAGS) defconfig
	@$(SCRIPTDIR)/mconf $(INIT_KCONFIG)
	@$(MAKE) $(MAKEFLAGS) oldconfig

oldconfig:conf $(INIT_KCONFIG)
	$(SCRIPTDIR)/conf -o $(INIT_KCONFIG)
	mv .tmpconfig.h $(PDT_INC_DIR)/lw_config.h
