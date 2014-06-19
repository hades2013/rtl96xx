CROSS_COMPILE ?=
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
CC = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)cpp
AR = $(CROSS_COMPILE)ar
NM = $(CROSS_COMPILE)nm
STRIP = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

DESTDIR=$(ROOTFSDIR)
#SBINDIR=/usr/sbin
SBINDIR=/bin
CONFDIR=/etc/iproute2
DOCDIR=/usr/share/doc/iproute2
MANDIR=/usr/share/man

#path to kernel include
KERNEL_INCLUDE=$(LINUX_DIR)/include

# Path to db_185.h include
DBM_INCLUDE:=/usr/include

DEFINES= -DRESOLVE_HOSTNAMES

#options if you have a bind>=4.9.4 libresolv (or, maybe, glibc)
LDLIBS=-lresolv
ADDLIB=

#options for decnet
ADDLIB+=dnet_ntop.o dnet_pton.o

#options for ipx
ADDLIB+=ipx_ntop.o ipx_pton.o

#CC = gcc
#HOSTCC = gcc
HOSTCC=$(CC)

CCOPTS = -D_GNU_SOURCE -O2 -Wstrict-prototypes -Wall
CFLAGS =$(EXTRA_CFLAGS) $(CCOPTS) -I../include $(DEFINES) 
YACCFLAGS = -d -t -v

LDLIBS += -L../lib -lnetlink -lutil
LDLIBS +=  -lnetlink -lutil

#Add begin: xufeng for Miniware QoS,2006/11/01
#LDLIBS +=$(EXTRA_CFLAGS) $(EXTRA_LDFLAGS)
#Add end: xufeng for Miniware QoS,2006/11/01

SUBDIRS=lib ip tc #misc netem

LIBNETLINK=../lib/libnetlink.a ../lib/libutil.a

all: Config
	@for i in $(SUBDIRS); \
	do $(MAKE) $(MFLAGS) -C $$i; done

Config:
	sh configure $(KERNEL_INCLUDE)
romfs:install-fs

install-fs: all
#	install -m 0755 -d $(DESTDIR)$(SBINDIR)
	install -m 0755 -d $(DESTDIR)$(CONFDIR)
#	install -m 0755 -d $(DESTDIR)$(DOCDIR)/examples
#	install -m 0755 -d $(DESTDIR)$(DOCDIR)/examples/diffserv
#	install -m 0644 README.iproute2+tc $(shell find examples -maxdepth 1 -type f) \
		$(DESTDIR)$(DOCDIR)/examples
#	install -m 0644 $(shell find examples/diffserv -maxdepth 1 -type f) \
		$(DESTDIR)$(DOCDIR)/examples/diffserv
	@for i in $(SUBDIRS); do $(MAKE) -C $$i install; done
	install -m 0644 $(shell find etc/iproute2 -maxdepth 1 -type f) $(DESTDIR)$(CONFDIR)
#	install -m 0755 -d $(DESTDIR)$(MANDIR)/man8
#	install -m 0644 $(shell find man/man8 -maxdepth 1 -type f) $(DESTDIR)$(MANDIR)/man8
#	ln -sf tc-pbfifo.8  $(DESTDIR)$(MANDIR)/man8/tc-bfifo.8
#	ln -sf tc-pbfifo.8  $(DESTDIR)$(MANDIR)/man8/tc-pfifo.8
#	install -m 0755 -d $(DESTDIR)$(MANDIR)/man3
#	install -m 0644 $(shell find man/man3 -maxdepth 1 -type f) $(DESTDIR)$(MANDIR)/man3

clean:
	rm -f cscope.*
	@for i in $(SUBDIRS); \
	do $(MAKE) $(MFLAGS) -C $$i clean; done

clobber: clean
	rm -f Config

distclean: clobber

cscope:
	cscope -b -q -R -Iinclude -sip -slib -smisc -snetem -stc

.EXPORT_ALL_VARIABLES:
