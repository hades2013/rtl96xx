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

CFLAGS += $(DEBFLAGS) $(CPPFLAGS) -I$(TOPDIR)/shared/thread -D_BIG_ENDIAN
LDFLAGS	+= -lshared -L$(TOPDIR)/shared

OBJS =  sys_monitor.o monitor_alarm.o ponstate.o 
    
all: ponmonitor
	
ponmonitor: $(OBJS)
	@echo "+++++++++++ CC $@"
	$(CC) -o $@ $^ $(LDFLAGS)

install-fs: all
	$(STRIP) ponmonitor
	install -D ponmonitor $(CONFIG_PREFIX)/usr/sbin/ponmonitor

install:install-fs	

clean:
	rm -f ponmonitor *.o  .*.depend	


%.o: %.c .%.depend
	@echo " [cm] CC $@"
	@$(CC) $(CFLAGS) -o $@ -c $<


.%.depend: %.c
	@$(CC) $(CFLAGS) -M $< > $@

-include $(OBJS:%.o=.%.depend)
	

