# Makefile for EPN104 ponmonitor  
# edward.zhou
# Copyright (c) loso, Inc. 2012
#
TOPDIR := ..

# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y

#.PHONY: all utest utest-clean clean install

# Add debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DBGFLAGS = -O -g -static
else
  DBGFLAGS = -O2
endif

CFLAGS += $(DEBFLAGS) $(CPPFLAGS) -I$(TOPDIR)/shared -D_BIG_ENDIAN
LDFLAGS	+= -lshared -L$(TOPDIR)/shared -lpthread

OBJS =  loopd_main.o rlpp.o
    
all: loopd
	
loopd: $(OBJS)
	@echo "+++++++++++ CC $@"
	$(CC) -o $@ $^ $(LDFLAGS)

install-fs: all
	$(STRIP) loopd
	install -D loopd $(CONFIG_PREFIX)/usr/sbin/loopd

install:install-fs	

clean:
	rm -f loopd *.o  .*.depend
	
utest:
	$(MAKE) -C cross_utest all
	$(MAKE) -C utest all
utest-clean:
	$(MAKE) -C cross_utest clean
	$(MAKE) -C utest clean

%.o: %.c .%.depend
	@echo " [cm] CC $@"
	@$(CC) $(CFLAGS) -o $@ -c $<


.%.depend: %.c
	@$(CC) $(CFLAGS) -M $< > $@

-include $(OBJS:%.o=.%.depend)
	

