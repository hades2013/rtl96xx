

APPDIR=$(shell pwd)/..
TOPDIR=$(APPDIR)/..
KERNELDIR=$(TOPDIR)/kernel
APOLLODIR=$(KERNELDIR)/linux-2.6.x/drivers/net/rtl86900

CC=$(TOPDIR)/toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/bin/rsdk-linux-gcc 


#include $(APOLLODIR)/Compiler_Flag
SDKDIR = $(APOLLODIR)/sdk
SDK_INCLUDE   = $(SDKDIR)/include
SYS_INCLUDE   = $(SDKDIR)/system/include
DRV_INCLUDE   = $(SDKDIR)/system/linux
UT_INCLUDE    = $(SDKDIR)/unittest

export SDKDIR SDK_INCLUDE SYS_INCLUDE DRV_INCLUDE UT_INCLUDE 
 
all:mytest

OBJS=link.o
#OBJS+=new.o

CFLAGS= -Wall -Os -I$(APPDIR)/include -I$(TOPDIR)/product/access/boot/preloader-9607/u-boot-9607/arch/otto/cpu/rlx5281/luna/rtl8367b \
				-I$(SDK_INCLUDE) \
				-I$(PLAT_INC_DIR) \
				-I$(PDT_INC_DIR) \
				-I$(SYS_INCLUDE) \
				-I$(SDKDIR)/include/rtk \
				-I$(SDKDIR)/system/include \

LDFLAGS= $(APPDIR)/shared/libshared.a  $(KERNELDIR)/linux-2.6.x/drivers/switch/sdk/src/app/lib/librtk.a

mytest: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)
	
	
clean:
	rm -rf mytest *.o

%.o:%.c
	$(CC) -o $@ $(CFLAGS) -c $<	