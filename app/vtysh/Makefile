# Makefile for vtysh

TOPDIR := ..
include $(TOPDIR)/.config
#include $(TOPDIR)/rules/config.mk

#CFLAGS	+= -I. -Wall -I$(TOPDIR)/shared -I$(TOPDIR)/nvram
CFLAGS	+= -I. -Wall -I$(TOPDIR)/shared 
CFLAGS	+= -I$(CPPFLAGS)
LDFLAGS	+= -lshared -L$(TOPDIR)/shared -lcrypt -lrt
#LDFLAGS	+= -lnvram -L$(TOPDIR)/nvram

OBJS = \
	buffer.o \
	command.o \
	memory.o \
	linklist.o \
	vector.o \
	memtypes.o \
	sockunion.o \
	thread.o \
	log.o \
	hash.o \
	workqueue.o \
	sigevent.o \
	str.o \
	network.o \
	filter.o \
	prefix.o \
	vty.o \
	vtysh_main.o \
	eoc_utils.o \
	cmd_system.o \
	cmd_vlan.o \
	cmd_interface.o \
	cmd_networkinfo.o \
	cmd_user.o \
	cmd_startup.o
#	demo_interface.o \
#	ipc_debug.o \
#	superuser.o \
#	cnu.o \
#	vlan.o
	
HEADERS =cmd_eoc.h zebra.h buffer.h filter.h linklist.h memory.h thread.h vty.h command.h hash.h \
	log.h memtypes.h sockunion.h vector.h prefix.h sigevent.h if.h sigevent.h str.h network.h

all: vtysh

vtysh: $(OBJS) $(TOPDIR)/shared/libshared.a 
#$(TOPDIR)/nvram/libnvram.a
	$(CC) -o $@ $^ $(LDFLAGS)
#	cp vtysh /tftpboot
clean:
	rm -fr *.o
	rm -fr vtysh

install: vtysh
	$(STRIP) vtysh
	install -D vtysh $(CONFIG_PREFIX)/usr/sbin/vtysh
	

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -DHAVE_CONFIG_H -c -I. -o $@ $<
	

