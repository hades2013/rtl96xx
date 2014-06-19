################################################################################
#
# Copyright (C) 2009 Opulan Technologies Corp., All Rights Reserved
#
# FILENAME: Makefile
#
# DESCRIPTION: hal src module
#
# Date Created (optional): 3/10/2009
#
# Authors (optional): 
#
# Reviewed by (optional):
#
# Edit History:
#
#
################################################################################

TARGET = opconn
TARGETDIR = $(shell cd ../..;/bin/pwd)

# Include common and TARGET specific defines
include $(TARGETDIR)/Makefile.conf


# Library to be built
LIB = libdvrs.a

# Objects to build and included into library
OBJECTS = hal.o \
	hal_fdb.o \
	hal_port.o \
	hal_vlan.o \
	hal_cls.o \
	hal_tm.o \
	hal_onu.o \
	hal_global.o \
	hal_interrupt.o \
	hal_host.o \
	hal_dma1.o \
	hal_multicast.o \
	hal_stats.o \
	hal_oam.o
        

#	

#	



# and place objects into LIBDIR
OBJECTS := $(addprefix $(LIBDIR)/, $(OBJECTS))


# One or more directories containig .h files specific to this module. If there are no local include directories, leave blank.
LOCALINCLUDE = \
	-I$(TARGETDIR)/vos/linux/h \
	-I$(TARGETDIR)/hal/h


all: $(LIBDIR)/$(LIB)


# Include dependencies
ifeq ($(findstring $(MAKECMDGOALS), clean cleanall ),)
-include $(OBJECTS:.o=.d)
endif

# Include other make rules.
include $(TARGETDIR)/make.rules
