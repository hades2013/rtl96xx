#
# Minimum environment and virtual path setup
#
SHELL		= /bin/bash
srcdir		= .
top_srcdir	= ..
VERSION		= 5.6.2


#
# Paths
#
prefix		= /opt/workspace/rtl9607/product/access/rootfs/../../../app/net-snmp-5.6.2/ipkg-install
exec_prefix	= ${prefix}
bindir		= ${exec_prefix}/bin
sbindir		= ${exec_prefix}/sbin
libdir		= ${exec_prefix}/lib
datarootdir	= ${prefix}/share
datadir		= ${datarootdir}
includedir	= ${prefix}/include/net-snmp
ucdincludedir	= ${prefix}/include/ucd-snmp
mandir		= ${datarootdir}/man
man1dir		= $(mandir)/man1
man3dir		= $(mandir)/man3
man5dir		= $(mandir)/man5
man8dir		= $(mandir)/man8
snmplibdir	= $(datadir)/snmp
mibdir		= $(snmplibdir)/mibs
persistentdir	= /tmp/snmp/
DESTDIR         = 
INSTALL_PREFIX  = $(DESTDIR)

#
# Programs
#
INSTALL		= $(LIBTOOL) --mode=install /usr/bin/install -c
UNINSTALL	= $(LIBTOOL) --mode=uninstall rm -f
LIBTOOLCLEAN	= $(LIBTOOL) --mode=clean rm -f
INSTALL_DATA    = ${INSTALL} -m 644
SED		= /bin/sed
LN_S		= ln -s
AUTOCONF	= :
AUTOHEADER	= :
PERL            = /usr/bin/perl
PYTHON          = /usr/bin/python
FIND            = find

#
# Compiler arguments
#
CFLAGS		= -fno-strict-aliasing -Os -Wall -pipe -mtune=mips32 -funit-at-a-time  -msoft-float -fpic  -I/opt/workspace/rtl9607/product/access/../../app/include -I/opt/workspace/rtl9607/product/access/include -I/opt/workspace/rtl9607/app/shared -Wall  -Ulinux -Dlinux=linux 
EXTRACPPFLAGS	= -x c
LDFLAGS		= -T/opt/workspace/rtl9607/product/access/../../app/rules/shared.lds   -lifm -L/opt/workspace/rtl9607/product/access/lwlib -lshared -L/opt/workspace/rtl9607/app/shared  -lcfg -L/opt/workspace/rtl9607/product/access/lwlib 
LIBTOOL		= $(SHELL) $(top_builddir)/libtool 
EXEEXT		= 

# Misc Compiling Stuff
CC	        = rsdk-linux-gcc 
LINKCC	        = rsdk-linux-gcc 

# use libtool versioning the way they recommend.
# The (slightly clarified) rules:
#
# - If any interfaces/structures have been removed or changed since the
#   last update, increment current (+5), and set age and revision to 0. Stop.
#
# - If any interfaces have been added since the last public release, then
#   increment current and age, and set revision to 0. Stop.
# 
# - If the source code has changed at all since the last update,
#   then increment revision (c:r:a becomes c:r+1:a). 
#
# Note: maintenance releases (eg 5.2.x) should never have changes
#       that would require a current to be incremented.
#
# policy: we increment major releases of LIBCURRENT by 5 starting at
# 5.3 was at 10, 5.4 is at 15, ...  This leaves some room for needed
# changes for past releases if absolutely necessary.
#
LIBCURRENT  = 25
LIBAGE      = 0
LIBREVISION = 2

LIB_LD_CMD      = $(LIBTOOL) --mode=link $(LINKCC) $(CFLAGS) -rpath $(libdir) -version-info $(LIBCURRENT):$(LIBREVISION):$(LIBAGE) -o
LIB_EXTENSION   = la
LIB_VERSION     =
LIB_LDCONFIG_CMD = $(LIBTOOL) --mode=finish $(libdir)
LINK		= $(LIBTOOL) --mode=link $(LINKCC)
# RANLIB 	= rsdk-linux-ranlib 
RANLIB		= :

# libtool definitions
.SUFFIXES: .c .o .lo .rc
.c.lo:
	$(LIBTOOL) --mode=compile $(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
.rc.lo:
	$(LIBTOOL) --mode=compile --tag=CC windres -o $@ -i $<

# include paths
#
SRC_TOP_INCLUDES            = -I$(top_srcdir)/include
SRC_SNMPLIB_INCLUDES        = -I$(top_srcdir)/snmplib
SRC_AGENT_INCLUDES          = -I$(top_srcdir)/agent
SRC_HELPER_INCLUDES         = -I$(top_srcdir)/agent/helpers
SRC_MIBGROUP_INCLUDES       = -I$(top_srcdir)/agent/mibgroup

BLD_TOP_INCLUDES            = -I$(top_builddir)/include $(SRC_TOP_INCLUDES)
BLD_SNMPLIB_INCLUDES        = -I$(top_builddir)/snmplib $(SRC_SNMPLIB_INCLUDES)
BLD_AGENT_INCLUDES          = -I$(top_builddir)/agent $(SRC_AGENT_INCLUDES)
BLD_HELPER_INCLUDES         = -I$(top_builddir)/agent/helpers $(SRC_HELPER_INCLUDES)
BLD_MIBGROUP_INCLUDES       = -I$(top_builddir)/agent/mibgroup $(SRC_MIBGROUP_INCLUDES)

TOP_INCLUDES            = $(SRC_TOP_INCLUDES)
SNMPLIB_INCLUDES        = $(SRC_SNMPLIB_INCLUDES)
AGENT_INCLUDES          = $(SRC_AGENT_INCLUDES)
HELPER_INCLUDES         = $(SRC_HELPER_INCLUDES)
MIBGROUP_INCLUDES       = $(SRC_MIBGROUP_INCLUDES)

#
# Makefile for testing
#

# use GNU vpath, if available, to only set a path for source and headers
# VPATH will pick up objects too, which is bad if you are sharing a
# source dir...
#vpath %.h $(srcdir)
#vpath %.c $(srcdir)
# fallback to regular VPATH for non-gnu...
VPATH = $(srcdir)

USELIBS		= ../snmplib/libsnmp.$(LIB_EXTENSION)$(LIB_VERSION)
LIBS		= -L../snmplib $(CC_RUNTIME_ARG) -lsnmp -lm 

PARSEOBJS	=

CPPFLAGS	= $(SNMPLIB_INCLUDES)  -I/opt/workspace/rtl9607/product/access/../../toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/usr/include  -I/opt/workspace/rtl9607/product/access/../../toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/include -I/opt/workspace/rtl9607/product/access/../../app/include -I/opt/workspace/rtl9607/product/access/include
CC		= rsdk-linux-gcc  $(CPPFLAGS)

all:
	@echo "Select one of the following targets to run:"
	@echo ""
	@echo "  make test        -- default test suites"
	@echo "  make testall     -- Run all available tests"
	@echo "  make testfailed  -- Run only the tests that failed last time."
	@echo "  make testsimple  -- Run tests directly with simple_run"
	@echo ""
	@echo "Set additional test parameters with TESTOPTS=args"
	@echo ""
	@echo "Also see the RUNFULLTESTS script for details"

test:
	@if test "x$(PERL)" = "x" \
	         -o "xyes" != xyes \
		 || { ! $(PERL) -e 'require TAP::Harness;' >/dev/null 2>&1 \
		    && ! $(PERL) -e 'require Test::Harness;' >/dev/null 2>&1; };\
	then \
		echo $(srcdir)/RUNTESTS ; \
		$(srcdir)/RUNTESTS ; \
	else \
		echo $(srcdir)/RUNFULLTESTS $(TESTOPTS) ; \
		$(PERL) $(srcdir)/RUNFULLTESTS $(TESTOPTS) ; \
	fi

testsimple:
	@(export srcdir=$(top_srcdir) ; \
		export builddir=$(top_builddir) ; \
		$(srcdir)/fulltests/support/simple_run $(TESTOPTS) \
	)

testall:
	$(srcdir)/RUNFULLTESTS -g all $(TESTOPTS)

testfailed:
	$(srcdir)/RUNFULLTESTS -f $(TESTOPTS)


test-mibs:
	cd $(srcdir)/rfc1213 ; ./run

etest:    etimetest.o $(PARSEOBJS) $(USELIBS)
	${CC} -o $@ etimetest.o $(PARSEOBJS) ${LDFLAGS} ${LIBS} 

ktest:    keymanagetest.o $(PARSEOBJS) $(USELIBS)
	${CC} -o $@ keymanagetest.o $(PARSEOBJS) ${LDFLAGS} ${LIBS} 

misctest:    misctest.o $(PARSEOBJS) $(USELIBS)
	${CC} -o $@ misctest.o $(PARSEOBJS) ${LDFLAGS} ${LIBS} 

stest:    scapitest.o $(PARSEOBJS) $(USELIBS)
	${CC} -o $@ scapitest.o $(PARSEOBJS) ${LDFLAGS} ${LIBS} 

clean: testclean
	rm -f *.o core *.core $(TARG)

testclean:
	-rm -fr /tmp/snmp-test*

install: installdirs $(TARG)
	@for i in $(TARG) ; \
		do $(INSTALL) $$i $(INSTALL_PREFIX)$(bindir) ; \
		echo "install:  installed $$i in $(INSTALL_PREFIX)$(bindir)" ; \
	done

installdirs:
	@$(SHELL) $(srcdir)/../mkinstalldirs $(INSTALL_PREFIX)$(snmplibdir) $(INSTALL_PREFIX)$(bindir)

dependlocal: 
	makedepend $(CPPFLAGS) -o.lo $(srcdir)/*.c

depend: dependlocal

nosysdepend: 
	makedepend $(CPPFLAGS) -o.lo *.c
	$(PERL) -n -i.bak $(top_srcdir)/makenosysdepend.pl Makefile

makefileindepend: nosysdepend
	$(PERL) $(top_srcdir)/makefileindepend.pl


# DO NOT DELETE THIS LINE -- make depend depends on it.


etimetest.lo: ../config.h ../m/generic.h ../s/generic.h ../s/linux.h
etimetest.lo: ../snmplib/asn1.h ../snmplib/callback.h ../snmplib/lcd_time.h
etimetest.lo: ../snmplib/snmp_api.h ../snmplib/snmp_debug.h ../snmplib/tools.h
etimetest.lo: ../s/sysv.h  
keymanagetest.lo: ../config.h ../m/generic.h ../s/generic.h ../s/linux.h
keymanagetest.lo: ../snmplib/asn1.h ../snmplib/callback.h ../snmplib/keytools.h
keymanagetest.lo: ../snmplib/scapi.h ../snmplib/snmp_api.h ../snmplib/tools.h
keymanagetest.lo: ../snmplib/transform_oids.h ../s/sysv.h 
misctest.lo: ../config.h ../m/generic.h ../s/generic.h ../s/linux.h
misctest.lo: ../snmplib/asn1.h ../snmplib/callback.h ../snmplib/snmp_api.h
misctest.lo: ../snmplib/tools.h ../snmplib/transform_oids.h ../s/sysv.h
scapitest.lo: ../config.h ../m/generic.h ../s/generic.h ../s/linux.h
scapitest.lo: ../snmplib/asn1.h ../snmplib/callback.h ../snmplib/keytools.h
scapitest.lo: ../snmplib/scapi.h ../snmplib/snmp_api.h ../snmplib/tools.h
scapitest.lo: ../snmplib/transform_oids.h ../s/sysv.h 
T.lo: ../config.h ../m/generic.h ../s/generic.h ../s/linux.h ../s/sysv.h
