#for test

TOPDIR :=..
include $(TOPDIR)/.config
#include $(TOPDIR)/rules/config.mk

NAME = test
#LIB_DIR = $(SOFTWAREDIR)/cfg

#LDFLAGS	+=    -lifm -L$(TOPDIR)/ifm -lshared -L$(TOPDIR)/shared -lcfg -L$(TOPDIR)/cfg
ALL_SRCS += $(shell find ./  -name \*.c)
OBJS += $(ALL_SRCS:.c=.o)

#$(OBJS):$(ALL_SRCS)
#	$(CC)  $(CFLAGS)  -c -o $@ $<
all:$(OBJS)	
	$(CC)  $(LDFLAGS) -o $(NAME)  $^   
install:
	$(STRIP) $(NAME)
	install -D $(NAME) $(ROOTFSDIR)/usr/sbin/$(NAME)
	cd $(ROOTFSDIR)/usr/sbin && ln -sf test drv
	cd $(ROOTFSDIR)/usr/sbin && ln -sf test ifm
	cd $(ROOTFSDIR)/usr/sbin && ln -sf test cfg

clean:
	rm -f $(NAME) *.o
