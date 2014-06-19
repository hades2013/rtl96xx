# Makefile for lib8367m

LIB_VERMAJOR = 0
LIB_VERMINOR = 1
LIB_FILENAME = lib8367m.so

LIB_CFLAGS  = $(CFLAGS) -shared -fPIC -I. -DMDC_MDIO_OPERATION -D_LITTLE_ENDIAN -I$(TOPDIR)/shared
LIB_LDFLAGS = $(LDFLAGS) -Wl,-soname,$(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR)

CLI_CFLAGS  = $(CFLAGS) -I. -I$(TOPDIR)/shared
CLI_LDFLAGS = $(LDFLAGS)

CLI_OBJ = rtl8367.o
CLI_SRC = rtl8367.c
CLI_FILENAME = rtl8367

LIB_OBJS = rtk_api.o \
	rtl8370_asicdrv_acl.o \
	rtl8370_asicdrv.o \
	rtl8370_asicdrv_cputag.o \
	rtl8370_asicdrv_dot1x.o \
	rtl8370_asicdrv_eee.o \
	rtl8370_asicdrv_eeelldp.o \
	rtl8370_asicdrv_fc.o \
	rtl8370_asicdrv_green.o \
	rtl8370_asicdrv_igmp.o \
	rtl8370_asicdrv_inbwctrl.o \
	rtl8370_asicdrv_interrupt.o \
	rtl8370_asicdrv_led.o \
	rtl8370_asicdrv_lut.o \
	rtl8370_asicdrv_meter.o \
	rtl8370_asicdrv_mib.o \
	rtl8370_asicdrv_mirror.o \
	rtl8370_asicdrv_misc.o \
	rtl8370_asicdrv_oam.o \
	rtl8370_asicdrv_phy.o \
	rtl8370_asicdrv_port.o \
	rtl8370_asicdrv_portIsolation.o \
	rtl8370_asicdrv_qos.o \
	rtl8370_asicdrv_rldp.o \
	rtl8370_asicdrv_rma.o \
	rtl8370_asicdrv_rrcp.o \
	rtl8370_asicdrv_scheduling.o \
	rtl8370_asicdrv_specialCongest.o \
	rtl8370_asicdrv_storm.o \
	rtl8370_asicdrv_svlan.o \
	rtl8370_asicdrv_trunking.o \
	rtl8370_asicdrv_unknownMulticast.o \
	rtl8370_asicdrv_vlan.o \
	smi_mii.o

all: lib8367m cli

cli: lib8367m
	$(CC) $(CLI_CFLAGS) -c -o $(CLI_OBJ) $(CLI_SRC)
	$(CC) -o $(CLI_FILENAME) $(CLI_LDFLAGS) $(CLI_OBJ) \
		$(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR)

lib8367m: $(LIB_OBJS)
	$(CC) $(LIB_CFLAGS) $(LIB_LDFLAGS) \
		-o $(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR) $(LIB_OBJS)
	@rm -fr $(LIB_FILENAME)
	@ln -s $(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR) $(LIB_FILENAME)
	
clean:
	rm -fr $(LIB_FILENAME) *.o *.so $(CLI_FILENAME)

install: all
	$(CROSS_COMPILE)strip $(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR)
	$(CROSS_COMPILE)strip $(CLI_FILENAME)
	install -D $(CLI_FILENAME) $(CONFIG_PREFIX)/usr/sbin/$(CLI_FILENAME)
	install -D $(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR) $(CONFIG_PREFIX)/usr/lib/$(LIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR)

#Rules
%.o: %.c
	$(CC) $(LIB_CFLAGS) -c -o $@ $^
		