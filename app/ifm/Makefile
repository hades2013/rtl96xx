#CLI_FILENAME = nvram

LIB_VERMAJOR = 0
LIB_VERMINOR = 1
LIB_FILENAME = libifm.so
LIB_STATIC = libifm.a
LIB_CFLAGS  = $(CFLAGS) -shared
#LIB_LDFLAGS =  -Wl,-soname,$(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR)

CLI_CFLAGS  = $(CFLAGS)
CLI_LDFLAGS = $(LDFLAGS)

HEAD = $(wildcard *.h)
SOURCE = $(wildcard *.c)
#OBJ = $(patsubst %.c, %.o, $(SOURCE))
ALL_SRCS += $(shell find ./  -name \*.c)
OBJS += $(ALL_SRCS:.c=.o)

all: libifm
	install -D $(LIB_FILENAME)  $(PRODUCTDIR)/lwlib/$(LIB_FILENAME)
	install -D $(LIB_FILENAME)  $(CONFIGDIR)/$(LIB_FILENAME)
	install -D $(LIB_STATIC)  $(PRODUCTDIR)/lwlib/$(LIB_STATIC)
	install -D $(LIB_STATIC)  $(CONFIGDIR)/$(LIB_STATIC)

#$(OBJ): 
#	$(CC) $(CLI_CFLAGS) -c -o $@ $<
libifm: $(OBJS)	
	$(AR) -r $(LIB_STATIC) $(OBJS)
	@$(CC)  $(CFLAGS) -shared  -o $(LIB_FILENAME) $^ 
	$(STRIP) $(LIB_FILENAME)
	$(STRIP) $(LIB_STATIC)	
clean:
	rm -f $(LIB_FILENAME)* *.o $(LIB_STATIC)

install: all


	install -D $(LIB_FILENAME)  $(ROOTFSDIR)/usr/lib/$(LIB_FILENAME)
%.o: %.c %.d
	@echo " [lib] CC $@"
	$(CC) $(CFLAGS) -o $@ -c $<

%.d: %.c
	@$(CC) $(CFLAGS) -M $< > $(@:.d=.tmp)
	@sed -e '/^ *\\/d' -e "s#.*: #$@: #" $(@:.d=.tmp) > $@
	@rm $(@:.d=.tmp)
