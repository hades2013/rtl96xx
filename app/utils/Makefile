TOPDIR := ../
include $(TOPDIR)/.config
include $(TOPDIR)/rules/config.mk

OBJS := mctrl.o 
	


CFLAGS	+= -I. -I$(TOPDIR)/shared -I$(TOPDIR)/lib8367m -D_LITTLE_ENDIAN
LDFLAGS	+= -lnvram -L$(TOPDIR)/nvram -l8367m -L$(TOPDIR)/lib8367m -lshared -L$(TOPDIR)/shared

all: mctrl

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *~ mctrl

install: all
	$(STRIP) mctrl
	install -D mctrl $(CONFIG_PREFIX)/usr/sbin/mctrl
	
mctrl: $(OBJS) 
	$(CC) -o $@ $^ $(LDFLAGS)
