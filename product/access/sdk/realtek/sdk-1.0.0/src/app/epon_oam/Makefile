#
# Copyright (C) 2011 Realtek Semiconductor Corp.
#
# Makefile for SDK EPON OAM protocol Source
#

APOLLO_SDK_PATH = $(SDKDIR)

include $(APOLLODIR)/Compiler_Flag


LIB += $(APOLLODIR)/object/src/librtk.a
LIB += ../lib/libpr.a
LIB += -l pthread -lrt

#ASIC mode
CFLAGS := -DRTK_X86_CLE -DRTK_X86_ASICDRV -DCONFIG_APOLLO_CMD -DCONFIG_SMIDA1011 -DDISABLE_VLAN_SHADOW -DCONFIG_LINUX_USER_SHELL


SDK_ROOT_DIR = $(APOLLO_SDK_PATH)
SDK_INCLUDE   = $(SDK_ROOT_DIR)/include
SYS_DIR = $(SDK_ROOT_DIR)/system/linux
SYS_INCLUDE = $(SDK_ROOT_DIR)/system/include
RTUSR_INC = $(SYS_DIR)/rtk/rtusr/include
COMMON_DIR = $(SDK_ROOT_DIR)/src/common

EPON_OAM_DIR := .
EPON_OAM_INC_DIR  = $(EPON_OAM_DIR)/inc
EPON_OAM_SRC_DIR := $(EPON_OAM_DIR)/src
USER_OAM_INC_DIR := $(EPON_OAM_INC_DIR)/user
USER_OAM_SRC_DIR := $(EPON_OAM_SRC_DIR)/user

INCLUDE += -I$(EPON_OAM_INC_DIR) \
           -I$(USER_OAM_INC_DIR) \
           -I$(SDK_INCLUDE) \
           -I$(SYS_INCLUDE) \
           -I$(SDK_INCLUDE)/hal/chipdef/apollo \
           -I$(SDKDIR)/src/app/pkt_redirect/inc

CFLAGS += -DCONFIG_RTL865X \
          -DCONFIG_RTL865XC \
          -DRTL865X_MODEL_USER \
          -D_LITTLE_ENDIAN \

EXEC = eponoamd
EXECLI = oamcli
BIN = bin

OAMCORE := \
       $(EPON_OAM_SRC_DIR)/epon_oam_main.o \
       $(EPON_OAM_SRC_DIR)/epon_oam_db.o \
       $(EPON_OAM_SRC_DIR)/epon_oam_rx.o \
       $(EPON_OAM_SRC_DIR)/epon_oam_tx.o \
       $(EPON_OAM_SRC_DIR)/epon_oam_state.o \
       $(EPON_OAM_SRC_DIR)/epon_oam_dbg.o

OAMCORE_INC := $(shell ls $(EPON_OAM_INC_DIR)/*.h)

OAMCLI := \
       $(EPON_OAM_SRC_DIR)/epon_oam_cli.o \

USEROAM := \
       $(USER_OAM_SRC_DIR)/ctc_oam.o \
       $(USER_OAM_SRC_DIR)/ctc_oam_var.o

USEROAM_INC := $(shell ls $(USER_OAM_INC_DIR)/*.h)

$(EXEC): $(OAMCORE) $(OAMCORE_INC) $(USEROAM) $(USEROAM_INC)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDE) $(OAMCORE) $(USEROAM) $(LIB) -o $(BIN)/$@
	$(STRIP) $(BIN)/$@

$(EXECLI): $(OAMCLI) $(OAMCORE_INC)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDE) $(OAMCLI) $(LIB) -o $(BIN)/$@
	$(STRIP) $(BIN)/$@

%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

# Force to rebuild all every time
all: clean $(EXEC) $(EXECLI)

install: $(EXEC) $(EXECLI)
	cp $(BIN)/$(EXEC) $(BINDIR)
	cp $(BIN)/$(EXECLI) $(BINDIR)

clean:
	rm -f $(OAMCORE) $(USEROAM) $(BIN)/$(EXEC) $(BIN)/$(EXECLI) *.gdb *.elf *.elf2flt
