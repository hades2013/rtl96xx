################################################################################
#
# Copyright (C) 2008 Opulan Technologies Corp., All Rights Reserved
#
# FILENAME: Makefile
#
# DESCRIPTION: vos linux src module
#
# Date Created (optional): 4/15/2008
#
# Authors (optional): Gan Zhiheng
#
# Reviewed by (optional):
#
# Edit History:
#
#
################################################################################

TARGET = opconn
TARGETDIR = $(shell cd ../../..;/bin/pwd)

# Include common and TARGET specific defines
include $(TARGETDIR)/Makefile.conf


# Library to be built
LIB = libvos.a

# Objects to build and included into library
OBJECTS = \
	vos.o \
	vos_alloc.o \
	vos_bitmap.o \
	vos_config.o \
	vos_hash.o \
	vos_io.o \
	vos_libc.o \
	vos_match.o \
	vos_msq.o \
	vos_socket.o \
	vos_sync.o \
	vos_terminal.o \
	vos_thread.o \
	vos_timer.o \
	vos_time.o \
	vos_ipc.o \
	vos_tlv.o


# and place objects into LIBDIR
OBJECTS := $(addprefix $(LIBDIR)/, $(OBJECTS))


# One or more directories containig .h files specific to this module. If there are no local include directories, leave blank.
LOCALINCLUDE =


all: $(LIBDIR)/$(LIB)


# Include dependencies
ifeq ($(findstring $(MAKECMDGOALS), clean cleanall ),)
-include $(OBJECTS:.o=.d)
endif

# Include other make rules.
include $(TARGETDIR)/make.rules
