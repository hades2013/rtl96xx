
# Copyright (C) 2006, 2007 Junjiro Okajima
# Copyright (C) 2006, 2007 Tomas Matejicek, slax.org
#
# LICENSE follows the described ones in lzma and squashfs.

# $Id: Makefile,v 1.23 2007/11/09 14:42:11 jro Exp $

# paths
CURDIR = $(shell pwd)
Sqlzma = ${CURDIR}

LzmaVer = lzma449

Lzma = ${Sqlzma}/${LzmaVer}
SqVer = squashfs3.3
Squashfs = ${Sqlzma}/${SqVer}
KVer = linux-2.6.20
SqFs = ${Squashfs}/kernel-patches/${KVer}/fs/squashfs
#SqFs = ${LINUX_DIR}/fs/squashfs_lzma
#KDir = /lib/modules/$(shell uname -r)/build

ifeq (${LzmaVer}, lzma443)
LzmaC = ${Lzma}/C/7zip/Compress/LZMA_C
LzmaAlone = ${Lzma}/C/7zip/Compress/LZMA_Alone
else
LzmaC = ${Lzma}/C/Compress/Lzma
LzmaAlone = ${Lzma}/CPP/7zip/Compress/LZMA_Alone
endif
SqTools = ${Squashfs}/squashfs-tools
CC=cc
# enable it if you want to add -g option when compiling
#UseDebugFlags =
# disable it if you don't want to compile squashfs kernel module here
#BuildSquashfs = 1
export
all:
	${MAKE} -C ${LzmaC} -f sqlzma.mk $@
	${MAKE} -C ${LzmaAlone} -f sqlzma.mk $@
	${MAKE} -C ${SqTools} $@
	cp ${SqTools}/mksquashfs ${CURDIR}/../mksquashfs_lzma

clean:
	${MAKE} -C ${LzmaC} -f sqlzma.mk $@
	${MAKE} -C ${LzmaAlone} -f sqlzma.mk $@
	${MAKE} -C ${SqTools} $@
	${RM} *~

########################################

-include priv.mk

ifdef BuildSquashfs
CONFIG_SQUASHFS = m
CONFIG_SQUASHFS_EMBEDDED =
CONFIG_SQUASHFS_FRAGMENT_CACHE_SIZE = 3
EXTRA_CFLAGS = -I${Sqlzma} -I${SqFs} -Wall -Werror
EXTRA_CFLAGS += -DCONFIG_SQUASHFS_MODULE -UCONFIG_SQUASHFS
EXTRA_CFLAGS += -UCONFIG_SQUASHFS_EMBEDDED -DCONFIG_SQUASHFS_FRAGMENT_CACHE_SIZE=3
# if you enabled CONFIG_PREEMPT, but make CPU to try concentrating
# the uncompression, then define UnsquashNoPreempt.
# if you don't define UnsquashNoPreempt, the behaviour follows
# the definition of CONFIG_PREEMPT.
EXTRA_CFLAGS += -DUnsquashNoPreempt
export
SqVars = /tmp/sqvars.mk

all: all_sq

FORCE:
${SqVars}: FORCE
	echo EXTRA_CFLAGS += ${EXTRA_CFLAGS} > $@
all_sq: ${SqVars}
	${MAKE} -C ${KDir} M=${SqFs} modules

clean: clean_sq
clean_sq:
	${MAKE} -C ${KDir} M=${SqFs} clean
endif

########################################

load:
	for i in ${LzmaC}/kmod/unlzma.ko ${LzmaC}/kmod/sqlzma.ko \
		${SqFs}/squashfs.ko; \
	do sudo insmod $$i; done

unload:
	-sudo rmmod squashfs sqlzma unlzma
