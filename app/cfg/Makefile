TOPDIR := ../
include $(TOPDIR)/.config

LIB_VERMAJOR = 0
LIB_VERMINOR = 1
LIB_FILENAME = libcfg.so
LIB_STATIC = libcfg.a
CPPFLAGS += -I$(SOFTWAREDIR)/cfg/include
LIB_CFLAGS  = -shared  $(CFLAGS) 

ALL_SRCS += $(shell find ./  -name \*.c)
OBJS += $(ALL_SRCS:.c=.o)

all:$(OBJS)	
	$(AR) -r $(LIB_STATIC) $(OBJS)
	$(CC) $(LIB_CFLAGS) $(LIB_LDFLAGS) -o $(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR) $^ 
	$(CC) $(LIB_CFLAGS) $(LIB_LDFLAGS) -o $(LIB_FILENAME) $^ 
	$(STRIP) $(LIB_STATIC)
	$(STRIP) $(LIB_FILENAME)
	install -D $(LIB_FILENAME) $(PRODUCTDIR)/lwlib/$(LIB_FILENAME)
	install -D $(LIB_FILENAME) $(CONFIGDIR)/$(LIB_FILENAME)
	install -D $(LIB_STATIC) $(PRODUCTDIR)/lwlib/$(LIB_STATIC)
	install -D $(LIB_STATIC) $(CONFIGDIR)/$(LIB_STATIC)

install:
#	install -D $(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR) $(ROOTFSDIR)/usr/lib/$(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR)
	install -D $(LIB_FILENAME) $(PRODUCTDIR)/lwlib/$(LIB_FILENAME)
	install -D $(LIB_FILENAME) $(CONFIGDIR)/$(LIB_FILENAME)
	install -D $(LIB_STATIC) $(PRODUCTDIR)/lwlib/$(LIB_STATIC)
	install -D $(LIB_STATIC) $(CONFIGDIR)/$(LIB_STATIC)
		
clean:
	rm -f  $(LIB_FILENAME)* *.o  $(LIB_STATIC)
	rm -f  ./smalloc/*.o
	rm -f  ./newcfg/*.o

