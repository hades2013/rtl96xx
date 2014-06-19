################################################################################
#
# Copyright (C) 2008 Opulan Technologies Corp., All Rights Reserved
#
# FILENAME: Makefile
#
# DESCRIPTION: rstp src module
#
# Date Created (optional): 4/18/2008
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
LIB = librstp.a

OBJECTS = rstp_bridge.o rstp_to.o rstp_in.o rstp_statmch.o rstp_stpm.o rstp_port.o rstp_mgmt.o rstp_rolesel.o \
	rstp_vector.o rstp_times.o rstp_cli.o\
	rstp_portinfo.o rstp_roletrns.o rstp_sttrans.o rstp_topoch.o rstp_migrate.o rstp_transmit.o \
	rstp_p2p.o rstp_portrcv.o rstp_bridect.o rstp_pcost.o

# Objects to build and included into library


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
