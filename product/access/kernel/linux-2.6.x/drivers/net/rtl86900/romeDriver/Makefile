include $(APOLLODIR)/Compiler_Flag

srctree=$(LINUXDIR)

EXTRA_CFLAGS += -Wall
EXTRA_CFLAGS += $(PON_CFLAGS)
EXTRA_CFLAGS += -I$(srctree)/drivers/net/rtl86900/romeDriver
EXTRA_CFLAGS += -I$(srctree)/drivers/net
EXTRA_CFLAGS += -DCONFIG_SDK_KERNEL_LINUX

LUNAR_SVN_VERSION=$(shell drivers/net/rtl86900/romeDriver/getSvnVersion.sh ..)
SWITCH_SVN_VERSION=$(shell drivers/net/rtl86900/romeDriver/getSvnVersion.sh ./drivers/net/rtl86900/sdk)
ROMEDRIVER_SVN_VERSION=$(shell drivers/net/rtl86900/romeDriver/getSvnVersion.sh ./drivers/net/rtl86900/romeDriver)
USER_SVN_VERSION=$(shell drivers/net/rtl86900/romeDriver/getSvnVersion.sh ../user)
EXTRA_CFLAGS +=  -DLUNAR_SVN_VERSION="\""$(LUNAR_SVN_VERSION)"\""
EXTRA_CFLAGS +=  -DSWITCH_SVN_VERSION="\""$(SWITCH_SVN_VERSION)"\""
EXTRA_CFLAGS +=  -DROMEDRIVER_SVN_VERSION="\""$(ROMEDRIVER_SVN_VERSION)"\""
EXTRA_CFLAGS +=  -DUSER_SVN_VERSION="\""$(USER_SVN_VERSION)"\""

EXTRA_CFLAGS += -I$(srctree)/drivers/net/rtl86900/sdk/include
EXTRA_CFLAGS += -I$(srctree)/drivers/net/rtl86900/sdk/system/include
EXTRA_CFLAGS += -I$(srctree)/drivers/net/rtl86900/sdk/src/app/diag_rtk/include

EXTRA_CFLAGS += -DCONFIG_APOLLO

ifeq ($(CONFIG_MODULE_BUILD_IN),y)

obj-$(CONFIG_RTK_L34_ENABLE) += rtk_rg_internal.o
obj-$(CONFIG_RTK_L34_ENABLE) += rtk_rg_liteRomeDriver.o
obj-$(CONFIG_APOLLO_ROMEDRIVER) += rtk_rg_fwdEngine.o
obj-$(CONFIG_RTL_IGMP_SNOOPING) += rtk_rg_igmpsnooping.o
obj-$(CONFIG_RG_DEBUG) += rtk_rg_debug.o
obj-$(CONFIG_RG_CALLBACK) += rtk_rg_callback.o
obj-y += rtk_rg_profile.o

all:

install:

else

MODULE_NAME=liteRomeDriver
obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-objs:= \
			rtk_rg_internal.o \
			rtk_rg_liteRomeDriver.o
ifeq ($(CONFIG_RG_DEBUG),y)
$(MODULE_NAME)-objs +=  rtk_rg_debug.o
endif

ifeq ($(CONFIG_RG_CALLBACK),y)
$(MODULE_NAME)-objs +=  rtk_rg_callback.o
endif


ifeq ($(CONFIG_APOLLO_ROMEDRIVER),y)
FWD_MODULE_NAME=fwdEngine
obj-m += $(FWD_MODULE_NAME).o
$(FWD_MODULE_NAME)-objs:= \
                        rtk_rg_fwdEngine.o \
                        rtk_rg_igmpsnooping.o
endif

all:
	$(MAKE) -C $(LINUXDIR) M=$(APOLLODIR)/romeDriver ARCH=rlx CROSS_COMPILE=$(CROSS_COMPILE)
	
install:
	cp -af $(MODULE_NAME).ko $(ROMFSDIR)/lib/modules
	if [ -f $(FWD_MODULE_NAME).ko ] ; then cp -af $(FWD_MODULE_NAME).ko $(ROMFSDIR)/lib/modules; fi

clean:
	rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions *.symvers *.ko
endif
