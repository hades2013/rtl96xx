#
# Makefile for Apollo IPC
#
ifndef APP_DIR
-include $(DIR_LINUX)/.config
else
-include $(APP_DIR).config
endif

ifeq ($(CONFIG_RTL8686_ASIC),y)
EXTRA_CFLAGS += -DCONFIG_RTL8686_ASIC
else
EXTRA_CFLAGS += -DCONFIG_RTL8686_FPGA
endif

ifeq ($(CONFIG_RTL8686_IPC_DEBUG_MESSAGE),y)
EXTRA_CFLAGS += -DCONFIG_RTL8686_IPC_DEBUG_MESSAGE
endif

ifeq ($(CONFIG_RTL8686_SHM_NOTIFY),y)
EXTRA_CFLAGS += -DCONFIG_RTL8686_SHM_NOTIFY
endif

ifeq ($(CONFIG_RTL8686_IPC_TEST_THREAD),y)
EXTRA_CFLAGS += -DCONFIG_RTL8686_IPC_TEST_THREAD
endif
ifeq ($(CONFIG_RTL8686_IPC_TEST_THREAD_2),y)
EXTRA_CFLAGS += -DCONFIG_RTL8686_IPC_TEST_THREAD_2
endif
ifeq ($(CONFIG_RTL8686_IPC_TEST_THREAD_3),y)
EXTRA_CFLAGS += -DCONFIG_RTL8686_IPC_TEST_THREAD_3
endif

ifeq ($(CONFIG_RTL8686_READ_DRAM_THREAD),y)
EXTRA_CFLAGS += -DCONFIG_RTL8686_READ_DRAM_THREAD
endif

ifeq ($(CONFIG_RTL8686_DRAM_64_MB),y)
EXTRA_CFLAGS += -DCONFIG_RTL8686_DRAM_64_MB
else ifeq ($(CONFIG_RTL8686_DRAM_32_MB),y)
EXTRA_CFLAGS += -DCONFIG_RTL8686_DRAM_32_MB
else
$(error need to check DRAM size, we will stop here)
endif

ifeq ($(CONFIG_RTL8686_SLIC_RESET),y)
EXTRA_CFLAGS += -DCONFIG_RTL8686_SLIC_RESET
endif

ifeq ($(CONFIG_RTL8686_IPC_RECORD_DSP_LOG),y)
EXTRA_CFLAGS += -DCONFIG_RTL8686_IPC_RECORD_DSP_LOG
endif

ifeq ($(CONFIG_RTL8686_IPC_PROCESSOR), 0)

EXTRA_CFLAGS += -D_AIPC_CPU_

EXTRA_CFLAGS += -I../../../arch/rlx/bsp_rtl8686

obj-$(CONFIG_RTL8686_IPC_DEV)	+= aipc_dev.o
obj-$(CONFIG_RTL8686_IPC_DEV)	+= aipc_irq.o
obj-$(CONFIG_RTL8686_IPC_DEV)	+= aipc_intq.o
obj-$(CONFIG_RTL8686_IPC_DEV)	+= aipc_ctrl.o
obj-$(CONFIG_RTL8686_IPC_DEV)	+= aipc_mbox.o
obj-$(CONFIG_RTL8686_IPC_DEV)	+= aipc_mem.o
obj-$(CONFIG_RTL8686_IPC_DEV)	+= aipc_shm.o

else ifeq ($(CONFIG_RTL8686_IPC_PROCESSOR), 1)

# Build tag
#BUILD_TAG = echo `date` foo@`hostname`
BUILD_TIME = echo `date`
BUILD_HOST = echo foo@`hostname`

# Compiler release
COMPILER_RELEASE = echo `mips-elf-gcc --version | head -1`

# Whoami
UNAME = `whoami | cut -c 1-2`

ifndef APP_LIB
APP_LIB=../libapp.a
endif


ifeq ($(ECOS_INSTALL_DIR),)
#$(error ECOS_INSTALL_DIR not set!)
ECOS_INSTALL_DIR=../../rtl8196c-ap_install
endif
ECOS_INCLUDE_DIR = $(ECOS_INSTALL_DIR)/include
ECOS_LIB_DIR = $(ECOS_INSTALL_DIR)/lib
ECOS_SHELL_DIR=$(shell pwd)

include $(ECOS_INSTALL_DIR)/include/pkgconf/ecos.mak

#ARCH = mips-elf-
ARCH = $(ECOS_COMMAND_PREFIX)

# bintuils
CC = $(ARCH)gcc
CXX = $(ARCH)gcc
AR = $(ARCH)ar
LD = $(ARCH)ld

export CC CXX AR LD

# Separate C++ flags out from C flags.
ACTUAL_CFLAGS = $(ECOS_GLOBAL_CFLAGS)
ACTUAL_CFLAGS := $(subst -fno-rtti,,$(ACTUAL_CFLAGS))
ACTUAL_CFLAGS := $(subst -frtti,,$(ACTUAL_CFLAGS))
ACTUAL_CFLAGS := $(subst -Woverloaded-virtual,,$(ACTUAL_CFLAGS))
ACTUAL_CFLAGS := $(subst -fvtable-gc,,$(ACTUAL_CFLAGS))

ACTUAL_CXXFLAGS = $(subst -Wstrict-prototypes,,$(ECOS_GLOBAL_CFLAGS))

ifeq ($(WATCHDOG_SUPPORT), yes)
EXTRA_CFLAGS += -DCONFIG_WATCHDOG
endif

EXTRA_CFLAGS += -D_AIPC_DSP_

CFLAGS  = $(ACTUAL_CFLAGS) -D__ECOS $(EXTRA_CFLAGS)
CXXFLAGS = $(ACTUAL_CXXFLAGS) -D__ECOS $(EXTRA_CFLAGS)
LDFLAGS = $(ECOS_GLOBAL_LDFLAGS)

CFLAGS += -I$(ECOS_INCLUDE_DIR) -I$(ECOS_SHELL_DIR)
CXXFLAGS += -I$(ECOS_INCLUDE_DIR) -I$(ECOS_SHELL_DIR)
LDFLAGS += -L$(ECOS_LIB_DIR) -Ttarget.ld

ECOS_SHELL_CFLAGS=$(CFLAGS)
export ECOS_SHELL_CFLAGS

.SUFFIXES: .o .c .cpp .cxx .s .S


#CFLAGS +=	-DBUILD_TAG=\""$(shell $(BUILD_TAG))"\"
CFLAGS += -DBUILD_TIME=\""$(shell $(BUILD_TIME))"\" -DBUILD_HOST=\""$(shell $(BUILD_HOST))"\" \
	  -DCOMPILER_RELEASE=\""$(shell $(COMPILER_RELEASE))"\" \
	  -DBUILD_FLAGS="\"$(DEBUG_FLAGS) $(OPT_FLAGS)"\" \

NAME = aipc

OBJS = aipc_dev.o \
	aipc_irq.o \
	aipc_intq.o \
	aipc_ctrl.o \
	aipc_mbox.o \
	aipc_mem.o \
	aipc_shm.o 

all: $(NAME) Makefile
	$(AR) rcs $(APP_LIB) $(OBJS)

$(NAME): $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.cpp.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.cxx.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $<

endif

clean:
	rm -fr *.o .*.cmd modules.order *~

