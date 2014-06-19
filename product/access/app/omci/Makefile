# To change with the environment

#TOOLCHAIN_DIR  = /TXC/cig_pkg/3.1.1/usr/bin/mips_4KC-

OMCI_X86=n
CONFIG_NO_DELAY=y
CONFIG_SUPPORT_HWOLT=y

ifeq ($(OMCI_X86),y)
SDKDIR=../../../
CC = $(TOOLCHAIN_DIR)gcc
AR = $(TOOLCHAIN_DIR)ar
CONFIG_GPON_VERSION=2
endif

OMCI_DIR = OMCI
MIB_DIR  = MIB
GOS_DIR  = GOS
DRV_DIR  = DRV

RTK_INCLUDES = \
							-I$(MIB_DIR)/inc  \
							-I$(MIB_DIR)/src  \
							-I$(OMCI_DIR)/inc \
							-I$(OMCI_DIR)/src \
							-I$(GOS_DIR)/inc  \
							-I$(GOS_DIR)/src \
							-I$(SDKDIR)/src/app/pkt_redirect/inc

DRV_INCLUDES   = -I$(SDKDIR)/include \
		 -I$(SDKDIR)/system/include 
     
DEFINES  =
LIBS     = -lm -lpthread							
SOUR_OMCI = $(wildcard $(OMCI_DIR)/src/*.c)
SOUR_MIB  = $(wildcard $(MIB_DIR)/src/*.c) $(wildcard $(MIB_DIR)/src/tables/*.c)
SOUR_GOS  = $(wildcard $(GOS_DIR)/src/*.c)
SOUR_CMD  = $(OMCI_DIR)/src/omci_task.c omci_cmd.c

OBJS_OMCI = $(patsubst %.c, %.o, $(SOUR_OMCI))
OBJS_MIB  = $(patsubst %.c, %.o, $(SOUR_MIB))
OBJS_GOS  = $(patsubst %.c, %.o, $(SOUR_GOS))

TARG_OMCI = omci_app
TARG_MIB  = libmib.a
TARG_OMCI_CMD = omcicli

ifneq ($(OMCI_X86),y)
LIBS+= ../lib/libpr.a
LIBS+= $(SDKDIR)/src/app/lib/librtk.a
else
DEFINES+=-DOMCI_X86
endif

ifeq ($(CONFIG_NO_DELAY),y)
DEFINES+=-DCONFIG_NO_DELAY
endif


ifeq ($(CONFIG_SUPPORT_HWOLT),y)
DEFINES+=-DCONFIG_SUPPORT_HWOLT
endif

DEFINES+=-DCONFIG_GPON_VERSION=$(CONFIG_GPON_VERSION)



CFLAGS   = -Wall -pthread $(RTK_INCLUDES) $(DRV_INCLUDES) $(DEFINES)
################################
# for ES olt                   #
#			-- bridge configuration  #
################################
#DEFINES += -D__ES_OLT__
#DEFINES += -D__BDG_MODE__=3


TARG_DRV:
ifneq ($(OMCI_X86),y)
	$(MAKE) -C $(DRV_DIR) all
endif

all: $(TARG_OMCI) $(TARG_OMCI_CMD) TARG_DRV

$(TARG_OMCI): $(TARG_MIB)
	$(CC) $(CFLAGS) $(SOUR_OMCI) $(TARG_MIB) $(TARG_LIB) -o $(TARG_OMCI) $(LIBS)

$(TARG_MIB): $(OBJS_MIB) $(OBJS_GOS)
	$(AR) rcs $(TARG_MIB) $^

NO_LINK: $(OBJS_MIB) $(OBJS_OMCI)

mib: $(TARG_MIB)

$(TARG_OMCI_CMD): 
	$(CC) $(CFLAGS) $(SOUR_CMD) -o $@


SHOW:
	@echo I wanna know something like ... 

clean:
	rm -f $(MIB_DIR)/src/*.o
	rm -f $(GOS_DIR)/src/*.o
	rm -f $(OMCI_DIR)/src/*.o
	rm -f $(MIB_DIR)/src/tables/*.o
	rm -f $(TARG_OMCI)
	rm -f $(TARG_MIB)
	rm -f $(TARG_OMCI_CMD)
ifneq ($(OMCI_X86),y)
	$(MAKE) -C $(DRV_DIR) clean
endif

install:
	cp -af $(TARG_OMCI) $(BINDIR)
	cp -af $(TARG_OMCI_CMD) $(BINDIR)
ifneq ($(OMCI_X86),y)
	$(MAKE) -C $(DRV_DIR) install
endif
