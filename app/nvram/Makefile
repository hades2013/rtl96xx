CLI_FILENAME = nvram

LIB_VERMAJOR = 0
LIB_VERMINOR = 1
LIB_FILENAME = libnvram.so

LIB_CFLAGS  = $(CFLAGS) -shared -fPIC
LIB_LDFLAGS = $(LDFLAGS) -Wl,-soname,$(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR)

CLI_CFLAGS  = $(CFLAGS)
CLI_LDFLAGS = $(LDFLAGS)

CLI_OBJ = cli.o
LIB_OBJ = crc.o nvram.o

all: cli libnvram

cli: libnvram
	$(CC) $(CLI_CFLAGS) -c -o cli.o cli.c
	$(CC) -o $(CLI_FILENAME) $(CLI_LDFLAGS) $(CLI_OBJ) \
		$(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR)

cli.o: cli.c
	$(CC) $(CLI_CFLAGS) -c -o $@ $<

libnvram:
	$(CC) $(LIB_CFLAGS) -c -o crc.o crc.c
	$(CC) $(LIB_CFLAGS) -c -o nvram.o nvram.c
	$(CC) $(LIB_CFLAGS) $(LIB_LDFLAGS) -o $(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR) $(LIB_OBJ)
	$(CC) $(LIB_CFLAGS) $(LIB_LDFLAGS) -o $(LIB_FILENAME) $(LIB_OBJ)

clean:
	rm -f $(CLI_FILENAME) $(LIB_FILENAME)* *.o

install: all
	install -D $(CLI_FILENAME) $(ROOTFSDIR)/usr/sbin/$(CLI_FILENAME)
	install -D $(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR) $(CONFIG_PREFIX)/usr/lib/$(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR)
