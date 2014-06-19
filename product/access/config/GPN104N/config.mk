# toolchain configuration

CROSS_COMPILE = rsdk-linux-
CROSS_KERNEL_COMPILE = rsdk-linux-
UCLIBC_DIR = $(shell pwd)/../../toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915
TOOLCHAIN_DIR =$(shell pwd)/../../toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915
ARCH = mips
PATH+=:$(shell pwd)/../../toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/bin
AR = $(CROSS_COMPILE)ar
AS = $(CROSS_COMPILE)gcc -c -Os -Wall -pipe -mips32 -mtune=mips32 -msoft-float -funit-at-a-time  -fpic
LD = $(CROSS_COMPILE)ld 
NM=$(CROSS_COMPILE)nm 
CC = $(CROSS_COMPILE)gcc 
GCC= $(CROSS_COMPILE)gcc 
CXX= $(CROSS_COMPILE)g++ 
RANLIB = $(CROSS_COMPILE)ranlib 
STRIP = $(CROSS_COMPILE)strip 
OBJCOPY = $(CROSS_COMPILE)objcopy 
OBJDUMP = $(CROSS_COMPILE)objdump 
SIZE = $(CROSS_COMPILE)size 
#CFLAGS = -Os -Wall -pipe -mips32 -mtune=mips32 -msoft-float -funit-at-a-time  -fpic
#CXXFLAGS = -Os -Wall -pipe -mips32 -mtune=mips32 -funit-at-a-time  -msoft-float -fpic
PRELOADERDIR=$(shell pwd)/boot/preloader-9607
UBOOTDIR=$(PRELOADERDIR)/u-boot-9607
export CROSS_COMPILE CROSS_KERNEL_COMPILE UCLIBC_DIR TOOLCHAIN_DIR ARCH AR AS LD CC GCC CXX RANLIB STRIP OBJCOPY OBJDUMP SIZE 
export CFLAGS CXXFLAGS CPPFLAGS LDFLAGS PATH UBOOTDIR PRELOADERDIR