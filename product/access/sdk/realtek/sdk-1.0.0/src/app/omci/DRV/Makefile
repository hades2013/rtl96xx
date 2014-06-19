#########################################################
#	Build Realtek OMCI modules		#
#########################################################
include $(APOLLODIR)/Compiler_Flag

MODULE_NAME=omcidrv

PWD=$(shell pwd)
SDKDIR=$(APOLLODIR)/sdk
SDK_INCLUDE   = $(SDKDIR)/include
SYS_INCLUDE   = $(SDKDIR)/system/include
DRV_INCLUDE   = $(SDKDIR)/system/linux
OMCIDIR=$(SDKDIR)/src/app/omci
MODULEDIR=$(SDKDIR)/src/module


EXTRA_CFLAGS += -I$(SDK_INCLUDE) \
		-I$(DRV_INCLUDE) \
		-I$(SYS_INCLUDE) \
		-I$(MODULEDIR) \
		-I$(OMCIDIR) \
		-DCONFIG_SDK_KERNEL_LINUX


obj-m := $(MODULE_NAME).o

$(MODULE_NAME)-objs:= omci_drv.o

all:
	@echo "   BUILD module $(MODULE_NAME)"
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) ARCH=rlx CROSS_COMPILE=$(CROSS_COMPILE)

install:
	@if [ ! -d $(ROMFSDIR)/lib/modules ]; \
        then \
                mkdir $(ROMFSDIR)/lib/modules ; \
        fi
	cp -af $(MODULE_NAME).ko $(ROMFSDIR)/lib/modules/

clean:
	rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions *.symvers *.ko
