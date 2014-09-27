# Makefile for shared
TOPDIR := ../
include $(TOPDIR)/.config
include $(TOPDIR)/rules/config.mk


LIB_VERMAJOR = 0
LIB_VERMINOR = 1
LIB_FILENAME = libshared.so
LIB_STATIC   = libshared.a

LIB_SWAPI = libswapi.a

LIB_CFLAGS  = $(CFLAGS) -shared -fPIC -I. -DMDC_MDIO_OPERATION -D_LITTLE_ENDIAN
LIB_LDFLAGS = $(LDFLAGS) -Wl,-soname,$(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR)

CFLAGS	+= -I. 

#-I$(TOPDIR)/lib8367m
ALL_SRCS += $(shell find ./drv -name \*.c)
ALL_SRCS += $(shell find ./thread -name \*.c)

OBJS =ufile_process.o time_zone.o valids.o hex_utils.o ipc_client.o sysmon.o str_utils.o shutils.o ipc_funcs.o systemlog.o cipher.o\
drv_wtd_user.o memshare.o vlan_utils.o
OBJS += $(ALL_SRCS:.c=.o)

SW_SRCS += $(shell find ./drv -name \*.c)
OBJS_SW += $(SW_SRCS:.c=.o)

HEADERS =vendor.h ufile.h ufile_process.h time_zone.h valids.h  hex_utils.h ipc_protocol.h str_utils.h vlan_utils.h switch.h ipc_client.h user.h \
ipc_protocol.h memshare.h cipher.h

all: shared swapi

$(LIB_OBJS): $(HEADERS)

shared: $(OBJS)
	$(AR) -r $(LIB_STATIC) $(OBJS)
	@$(CC)  -shared $(CFLAGS) -o $(LIB_FILENAME) $^ 

swapi:
	$(AR) -r $(LIB_SWAPI) $(OBJS_SW)
	
clean:
	rm -fr $(LIB_FILENAME) *.o *.so
	rm -rf ./drv/*.o

install: all
	install -D $(LIB_FILENAME) $(CONFIG_PREFIX)/usr/lib/
#	$(CROSS_COMPILE)strip $(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR)
#	install -D $(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR) $(CONFIG_PREFIX)/usr/lib/$(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR)

