################################################################################
#
# Copyright (C) 2008 Opulan Technologies Corp., All Rights Reserved
#
# FILENAME: Makefile
#
# DESCRIPTION: cli src module
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
TARGETDIR = $(shell cd ../..;/bin/pwd)

# Include common and TARGET specific defines
include $(TARGETDIR)/Makefile.conf


# Library to be built
LIB = libcli.a

# Objects to build and included into library
OBJECTS = \
	cli_cmd_enable.o \
	cli_cmd_fdb.o \
	cli_cmd_global.o \
	cli_cmd_list.o \
	cli_cmd_log.o \
	cli_cmd_port.o \
	cli_cmd_pon.o \
	cli_cmd_qos.o \
	cli_cmd_storm.o \
	cli_cmd_system.o \
	cli_cmd_user.o \
	cli_cmd_vlan.o \
	cli_cmd_multicast.o \
	cli_cmd_stats.o \
	cli_engine.o \
	cli_main.o \
	cli_session.o \
	cli_telnetd.o \
	cli_utils.o \
	cli_text_config.o \
	cli_page.o \
	cli_cmd_debug.o	

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
