# toolchain configuration

CROSS_COMPILE = mips-linux-uclibc-
UCLIBC_DIR = $(shell pwd)/../../toolchains/opulan_toolchain/openwrt
TOOLCHAIN_DIR =$(shell pwd)/../../toolchains/opulan_toolchain/openwrt
ARCH = mips
PATH+=:$(shell pwd)/../../toolchains/opulan_toolchain/openwrt/bin:$(shell pwd)/../../toolchains/opulan_toolchain/eldk/usr/bin
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
UBOOTDIR=$(shell pwd)/boot/u-boot-6752
export CROSS_COMPILE UCLIBC_DIR TOOLCHAIN_DIR ARCH AR AS LD CC GCC CXX RANLIB STRIP OBJCOPY OBJDUMP SIZE 
export CFLAGS CXXFLAGS CPPFLAGS LDFLAGS PATH UBOOTDIR
