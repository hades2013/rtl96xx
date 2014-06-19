######################################
#
# Generic makefile
#
# by George Foot
# email: george.foot@merton.ox.ac.uk
#
# Copyright (c) 1997 George Foot
# All rights reserved.
#
# No warranty, no liability;
# you use this at your own risk.
#
# You are free to modify and
# distribute this without giving
# credit to the original author.
#
######################################

### Customising
#
# Adjust the following if necessary; EXECUTABLE is the target
# executable's filename, and LIBS is a list of libraries to link in
# (e.g. alleg, stdcx, iostr, etc). You can override these on make's
# command line of course, if you prefer to do it that way.
#

EXECUTABLE := opl_reg
LIBS :=
CROSS_COMPILE := mips-linux-uclibc-
CFLAG_DEBUG :=
COPY_TO_DIR := /home/xwang/smb/share/plat/openwrt/kamikaze_7.09/opulan_files/apps

# Now alter any implicit rules' variables if you like, e.g.:
#
CXXFLAGS := $(CFLAGS)

CFLAGS_KERNEL := -D__KERNEL__ -DIPMUX_BIG_ENDIAN -Wall -Wstrict-prototypes \
	-Wno-trigraphs -O2 -fno-strict-aliasing -fno-common \
	-fomit-frame-pointer -G 0 -mno-abicalls -fno-pic -pipe -mips2 \
	-Wa,--trap -Wa,--trap -D__linux__ -D_MIPS_SZLONG=32 -D_MIPS_SZPTR=32 \
	-D_MIPS_SZINT=32 -g -DMODULE -D_MIPS_SZLONG=32 -mlong-calls \
 	-nostdinc -I$(GCCLIBH) -I$(KERNELH) -I.

CFLAGS_APP := -D__linux__ -DIPMUX_BIG_ENDIAN  -D_MIPS_SZPTR=32 -D_MIPS_SZINT=32

CFLAGS +=  -D__linux__ -DIPMUX_BIG_ENDIAN  -D_MIPS_SZPTR=32 -D_MIPS_SZINT=32
CFLAGS +=  -DMAKE_MIPS -DOPULAN_RTOS_LINUX

CFLAGS +=-Wall -Wstrict-prototypes -Wno-trigraphs -O2
CFLAGS +=-fomit-frame-pointer -fno-strict-aliasing -fno-common -pipe
CFLAGS +=-Wswitch -enum
CFLAGS +=$(CFLAGS_APP)
CFLAGS +=$(CFLAG_DEBUG)

# The next bit checks to see whether rm is in your djgpp bin
# directory; if not it uses del instead, but this can cause (harmless)
# `File not found' error messages. If you are not using DOS at all,
# set the variable to something which will unquestioningly remove
# files.
#

ifneq ($(wildcard $(DJDIR)/bin/rm),)
RM-F := rm -f
else
RM-F := del
endif

# Add the directory here.
DIRS := app drv

# compilers
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

export AS LD CC CPP AR NM STRIP OBJCOPY OBJDUMP

# compile flags


# You shouldn't need to change anything below this point.
#
#
DIRFILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
SOURCE := $(wildcard *.c) $(wildcard *.cc) $(DIRFILES)
OBJS := $(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(SOURCE)))
DEPS := $(patsubst %.o,%.d,$(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.c,$(MISSING_DEPS)) \
$(patsubst %.d,%.cc,$(MISSING_DEPS)))
CPPFLAGS += -MD

.PHONY : everything deps objs clean veryclean rebuild

%.o : %.c
	@echo "compiling $@"
	$(CC) $(CFLAGS) -c -o $@ $<

everything : $(EXECUTABLE)

deps : $(DEPS)

objs : $(OBJS)

t :
	echo $(DIRFILES)
	$(foreach dir,$(DIRS),echo $(wildcard $(DIRS)/*.c))

cp : $(EXECUTABLE)
	@cp $(EXECUTABLE) $(COPY_TO_DIR)/
	@ls -lh $(COPY_TO_DIR)/$(EXECUTABLE)

clean :
	@$(RM-F) *.o
	@$(RM-F) *.d

veryclean: clean
	@$(RM-F) $(EXECUTABLE)

rebuild: veryclean everything

ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS) :
	@$(RM-F) $(patsubst %.d,%.o,$@)
endif

-include $(DEPS)

$(EXECUTABLE) : $(OBJS)
	$(CC) -o $(EXECUTABLE) $(OBJS) $(addprefix -l,$(LIBS))
	$(STRIP) $(EXECUTABLE)
	@echo ===============================================
	@echo      OUTPUT Executable File : $(EXECUTABLE)
	@echo ===============================================

