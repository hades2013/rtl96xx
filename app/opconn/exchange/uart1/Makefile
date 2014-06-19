####
#   UART
####
PWD = $(shell /bin/pwd)
OS_TYPE = linux-2.6
ifeq ($(OS_TYPE),linux-2.6)
CROSS_COMPILE  = mips-linux-uclibc-
endif

ifeq ($(OS_TYPE),linux-2.4)
CROSS_COMPILE  = mips_4KC-
endif

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
RANLIB          = $(CROSS_COMPILE)ranlib
CPU_TYPE	= mips
export AS LD CC CPP AR NM STRIP OBJCOPY OBJDUMP CROSS_COMPILE


module-objs = uart_drv.o
obj-m= uart_drv.o
MODULE_NAME = uart_drv.ko


################################LINUX 2.6###################################
ifeq ($(OS_TYPE),linux-2.6)
#KERNEL := /opt/openwrt/kamikaze_7.09/build_mips/linux-2.6-atheros/linux-2.6.21.5
KERNEL = /opt/openwrt/kamikaze_7.09/build_mips/linux-2.6-atheros/linux-2.6.21.5

default:
	$(MAKE) -C $(KERNEL) M=$(PWD) ARCH=mips CROSS_COMPILE=mips-linux-uclibc- modules
clean:
	rm .*.cmd *.o *.ko *.mod.c Module.symvers -rf .tmp*

endif
#############################################################################


################################LINUX 2.4####################################
ifeq ($(OS_TYPE),linux-2.4)
#KERNEL_PATH=/home/zzhu/work/1025/kernel/trunk/linux-2.4.20
KERNEL_PATH=/home/common/ipmux/linux-2.4.20
CFLAGS =-D__KERNEL__ -DMODULE -O2 -DLINUX_24
CFLAGS+=-mno-abicalls -fno-pic -mips32 -mlong-calls
CFLAGS+=-I${KERNEL_PATH}/include/
CFLAGS+=-I${KERNEL_PATH}/arch/mips/mips-boards/malta/
module-objs: uart_drv.c uart_drv.h
	$(CROSS_COMPILE)gcc $(CFLAGS) -c uart_drv.c
clean:
	rm -f *.o 

endif
################################################################################


