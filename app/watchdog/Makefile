OBJS := app_watchdog.o

all: app_watchdog

clean:
	rm -f *.o *~ app_watchdog

install: all
	install -D app_watchdog $(ROOTFSDIR)/usr/sbin/app_watchdog

app_watchdog: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

