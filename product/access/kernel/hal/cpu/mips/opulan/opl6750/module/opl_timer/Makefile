#
# Makefile for opulan minte interrupt module
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definition is now in the main makefile...
###############################################################
#O_TARGET := opl_minte.o
#export-objs := opl_minte_module.o
#obj-$(CONFIG_OPL_MINTE_INTR)  := opl_minte_intr.o
#obj-$(CONFIG_OPL_IPMUX_INTERRUPT) += ipmux_interrupt.o
#obj-m  :=
#include $(TOPDIR)/Rules.make
##############################################################
OPL_MOD_DIR = $(shell cd ../;/bin/pwd)

include $(OPL_MOD_DIR)/build/Makefile.conf

CUR_DIR= $(OPL_MOD_DIR)/opl_timer
TEST_DIR = $(CUR_DIR)/test
################# project files and bin name #############
MOD_TARGET = ipmux_timer.o
OBJS = opl_timer.o


FLAGS += -Wswitch-enum
CFLAGS += $(CFLAGS_KERNEL) 
CFLAGS += -I$(OPL_MOD_DIR)/include 
TEST_CFLAGS = -I$(OPL_MOD_DIR)/include 
DEPS =

vpath %.o $(CUR_DIR)
vpath %.d $(CUR_DIR)

%.d : %.c
	@echo "generating dependency file for $@"
	@$(SHELL) -ec '$(CC)  -M $(CFLAGS) $< | sed '\''s/$*\\.o[ :]*/& $@/g'\'' > $@'
%.o : %.c
	@echo "compiling $@"
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY : all
all: $(MOD_TARGET)
$(MOD_TARGET):$(OBJS) 
	cd $(CUR_DIR); $(LD) -r -o $(MOD_TARGET) $(OBJS)
	cd $(CUR_DIR); $(STRIP) -d $(MOD_TARGET); cp $(MOD_TARGET) $(OPL_MOD_DIR)/obj
clean:
	-rm -f $(CUR_DIR)/*.o
	-rm -f $(CUR_DIR)/*~
	-rm -f $(CUR_DIR)/\#*

