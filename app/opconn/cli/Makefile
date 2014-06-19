################################################################################
#
# Copyright (C) 2008 Opulan Technologies Corp., All Rights Reserved
#
# FILENAME: Makefile
#
# DESCRIPTION: cli module
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
TARGETDIR = $(shell cd ..;/bin/pwd)

# Include common and TARGET specific defines
include $(TARGETDIR)/Makefile.conf

# Define a prefix for this Makefile, so that the common targets (COMMONTARGS defined in Makefile.conf),
# such as clean, etc. can be made in the subdirectories. Use the name of the current directory for COMMONPREFIX.
COMMONPREFIX = cli


# List of directories to make
SUBDIRS = src


# Define at least one rule before commonrules, otherwise the first commonrule will be the default.
# Here, default rule all: should be to build all SUBDIRS.
all: $(SUBDIRS)


# Rule to make common targets, such as clean, etc.
# This loop will propogate any of COMMONTARGS to the directories listed in SUBDIRS.
# This rule must live in each Makefile, as opposed to the common rules, so that the leaf node make doesn't confuse the
# rules $(COMMONPREFIX)clean with clean, etc.

$(COMMONTARGS):
	@for d in $(SUBDIRS); do \
		$(MAKE) -r $@ -C $$d; \
	done

# Include other make rules.
include $(TARGETDIR)/make.rules
