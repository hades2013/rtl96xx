# Under Solaris, you need to 
#    CFLAGS += -xO2 -Xc
#    LDLIBS += -lnsl -lsocket
# Some versions of Linux may need
#    CFLAGS += -D_GNU_SOURCE
# To cross-compile
#    CC = arm-linux-gcc
CROSS_COMPILE?=
CC=$(CROSS_COMPILE)gcc
STRIP=$(CROSS_COMPILE)strip
#LD=$(CROSS_COMPILE)ld -melf32btsmipn32

# To check for lint
# -Wundef not recognized by gcc-2.7.2.3
#CFLAGS += -Wall -Wpointer-arith -Wcast-align -Wcast-qual -Wshadow \
CFLAGS += -Wall -Wcast-align -Wcast-qual -Wshadow \
 -Waggregate-return -Wnested-externs -Winline -Wwrite-strings \
 -Wstrict-prototypes

CFLAGS += -O

CFLAGS+=$(EXTRA_CFLAGS) 
all: ntpclient

test: ntpclient
	./ntpclient -d -r <test.dat

ntpclient: ntpclient.o phaselock.o 
	${CC} $(CFLAGS) -L$(SOFT_DIR)/lib/  -o $@ $^

adjtimex: adjtimex.o

install: all
	install -D ntpclient $(ROOTFSDIR)/usr/sbin/ntpclient
clean:
	rm -f ntpclient adjitime *.o


