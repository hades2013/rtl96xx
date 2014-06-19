################################################################################
#
# Copyright (C) 2008 Opulan Technologies Corp., All Rights Reserved
#
# FILENAME: Makefile
#
# DESCRIPTION: sys src module
#
# Date Created (optional): 4/15/2008
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
LIB = libexchange.a

# Objects to build and included into library
OBJECTS = opl_api.o opl_api_pack.o opl_msg_process.o opl_msg_send.o opl_pal.o
        

#	

#	



# and place objects into LIBDIR
OBJECTS := $(addprefix $(LIBDIR)/, $(OBJECTS))


# One or more directories containig .h files specific to this module. If there are no local include directories, leave blank.
LOCALINCLUDE = \
	-I$(TARGETDIR)/vos/linux/h


all: $(LIBDIR)/$(LIB)


# Include dependencies
ifeq ($(findstring $(MAKECMDGOALS), clean cleanall ),)
-include $(OBJECTS:.o=.d)
endif

# Include other make rules.
include $(TARGETDIR)/make.rules
