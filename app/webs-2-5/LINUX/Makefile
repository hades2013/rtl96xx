#
# Makefile for the GoAhead web server reference source base
#  for the Linux OS
#
# Copyright (c) GoAhead Software, Inc. 1995-2010
# Copyright (c) PeerSec Networks, Inc. 2010
#
#

all:	compile 

ARCH	= libwebs.a
NAME	= httpd

# User Management switch
#UMSW	= -DUSER_MANAGEMENT_SUPPORT

# Digest Access switch
#DASW	= -DDIGEST_ACCESS_SUPPORT

#
# This is set by the debug target below
#
ifdef WEBS_DEBUG
DFLAGS  = -g -Wall -Winline -Wdisabled-optimization -DDEBUG
else
DFLAGS  = -Os
endif

debug:
	@$(MAKE) compile "WEBS_DEBUG = 1"

gold:
	@$(MAKE) compile

default: gold



#
#   Enable SSL build if MatrixSSL package found in root source directory
#   (if automation is not working, simply define SSLINC, SSLLIB, and SSLSW)
#
matrixsslDir:=$(SSL_DIR)
ifdef matrixsslDir
SSLINC = $(matrixsslDir)
SSLLIB = $(matrixsslDir)/libmatrixssl.a
SSLSW = -DWEBS_TRACE_SUPPORT -DWEBS_SSL_SUPPORT -DPOSIX -I$(SSLINC)
OPT_FILES = ../matrixSSLSocket.o ../websSSL.o
else
OPT_FILES = ../md5c.o
endif

# If-Modified-Support switches (requires math library, libm.a)
# IFMODSW = -DWEBS_IF_MODIFIED_SUPPORT
# IFMODLIB = /usr/lib/libm.a

# Dependencies
DEPEND_FILES	= $(OPT_FILES) ../asp.o ../balloc.o ../mime64.o \
		../cgi.o ../default.o  ../ejlex.o ../ejparse.o ../form.o \
		../h.o ../handler.o ../mime.o ../misc.o ../page.o  \
	 	../ringq.o ../rom.o  ../sock.o ../sockGen.o \
		../security.o ../sym.o ../uemf.o ../url.o ../value.o \
		../um.o ../umui.o ../websda.o ../emfdb.o \
		../webrom.o ../webs.o ../websuemf.o \
		../aspfunc.o ../asp_variable.o \
		../form_command.o \
                ../upldForm.o \
		main.o ../asp_vlan.o ../asp_port.o ../asp_wlan_ap.o ../asp_catv.o

CFLAGS	+= -DWEBS -DOS="LINUX" -DLINUX $(UMSW) $(DASW) $(SSLSW) $(IFMODSW)
CFLAGS	+= -I$(CPPFLAGS)
OTHERS	= -DB_STATS -DB_FILL 
CFLAGS 	+= -I.. -I../../nvram -I../../shared
LDFLAGS	+= $(SSLLIB) $(IFMODLIB) -L../../shared -lshared


compile: $(NAME)

#
#	Build archive of objects
#
$(ARCH): $(DEPEND_FILES)
	$(AR) $(ARFLAGS) $(ARCH) $?

#
#	Primary link
#
$(NAME): Makefile $(ARCH) ../../shared/libshared.a
	$(CC) -o $(NAME) $(DFLAGS) \
		$(CFLAGS) $(IFLAGS) \
		main.o ../../shared/libshared.a $(ARCH) $(LDFLAGS) 
		
install: 
	$(STRIP) $(NAME)
	install -D $(NAME) $(CONFIG_PREFIX)/usr/sbin/$(NAME)
#	echo $(shell pwd)
	cp  certSrv.pem  $(CONFIG_PREFIX)/etc
	cp privkeySrv.pem  $(CONFIG_PREFIX)/etc


clean:
	echo $(matrixsslDir)
	rm -f $(NAME) $(ARCH) $(DEPEND_FILES)

#
#	Dependencies
#
VENDORCFG := $(TOPDIR)/vendors/vendor.cfg	

../asp_port.o: ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h ../asp_variable.h  $(VENDORCFG)
../aspfunc.o: ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h ../asp_variable.h  $(VENDORCFG)
../asp_vlan.o: ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h ../asp_variable.h  $(VENDORCFG)
../form_command.o: ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h ../asp_variable.h  $(VENDORCFG)

../asp_variable.o: ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h ../asp_variable.h $(VENDORCFG)

../asp.o:  ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../balloc.o: ../balloc.c ../uemf.h

../mime64.o:  ../mime64.c ../webs.h ../wsIntrn.h  ../ej.h ../ejIntrn.h ../uemf.h

../cgi.o:  ../webs.h ../wsIntrn.h ../uemf.h

../default.o:  ../default.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../ejlex.o:  ../ejlex.c ../ej.h ../ejIntrn.h ../uemf.h

../ejparse.o:  ../ejparse.c ../ej.h ../ejIntrn.h ../uemf.h

../emfdb.o:  ../emfdb.h ../wsIntrn.h ../uemf.h

../form.o:  ../form.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../h.o:  ../h.c ../uemf.h

../handler.o:  ../handler.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../md5c.o:  ../md5.h ../wsIntrn.h ../uemf.h

../mime.o:  ../mime.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../misc.o:  ../misc.c ../uemf.h

../page.o:  ../page.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../ringq.o:  ../ringq.c ../uemf.h

../rom.o:  ../rom.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../security.o:  ../security.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../sock.o:  ../sock.c ../uemf.h

../sockGen.o:  ../sockGen.c ../uemf.h

../sym.o:  ../sym.c ../uemf.h

../uemf.o:  ../uemf.c ../uemf.h

../um.o:  ../webs.h ../wsIntrn.h ../um.h ../uemf.h

../umui.o:  ../webs.h ../wsIntrn.h ../um.h ../uemf.h

../url.o:  ../url.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../value.o:  ../value.c ../uemf.h

../webrom.o:  ../webrom.c ../webs.h ../wsIntrn.h ../uemf.h

../webs.o:  ../webs.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../websda.o:  ../webs.h ../wsIntrn.h ../websda.h ../uemf.h

../websuemf.o:  ../websuemf.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../websSSL.o:  ../websSSL.c ../websSSL.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../matrixSSLSocket.o:  ../matrixSSLSocket.c ../matrixSSLSocket.h

../upldForm.o:  ../upldForm.c ../webs.h

main.o:  main.c ../wsIntrn.h ../webs.h ../ej.h ../ejIntrn.h ../uemf.h

#
#	Transition rules (add -o to put object in right directory)
#
#.c.o:
#	$(CC) -c -o $@ $(DFLAGS) $(CFLAGS) $(IFLAGS) $<
