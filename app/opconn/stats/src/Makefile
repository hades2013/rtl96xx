################################################################################
#
# Copyright (C) 2008 Opulan Technologies Corp., All Rights Reserved
#
# FILENAME: Makefile
#
# DESCRIPTION: oamp src module
#
# Date Created (optional): 5/18/2008
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
TARGETDIR = $(shell cd ../..;/bin/pwd)

# Include common and TARGET specific defines
include $(TARGETDIR)/Makefile.conf


# Library to be built
LIB = libstats.a

# Objects to build and included into library
OBJECTS = stats.o stats_ar8306.o


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
