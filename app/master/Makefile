TOPDIR := ../
include $(TOPDIR)/.config
#include $(TOPDIR)/rules/config.mk
-include ../../product/access/.config

OBJS := main.o ipc_server.o \
	utils.o  timer.o signal_process.o  ipc_packet.o ifm_process.o ifm_state.o sys_network.o sys_admin.o sys_info.o \
	sys_time.o sys_snmp.o sys_loop.o sys_mvlan.o sys_syslog.o sys_qos.o sys_port.o\
	sys_config.o \
	dhcpc_event.o \
	sys_upgrade.o \
	sys_epon.o \
	mtd_utils.o sys_vlan.o sys_intf.o sys_mac_setting.o sys_sshd.o sys_wlan_ap.o sys_compatible.o
ifeq ($(CONFIG_ZBL_SNMP),y) 
	OBJS +=  sys_catv_zg.o sys_analogPropertyTable.o sys_commonAdminGroup.o
else
	OBJS +=  sys_catv.o
endif
	
HEADERS := master.h\
	utils.h \
	timer.h \
	signal_process.h \
	ipc_packet.h \
	sys_upgrade.h \
	mtd_utils.h \
	$(TOPDIR)/shared/vendor.h \
	$(TOPDIR)/shared/build_time.h \
	$(TOPDIR)/shared/debug.h \
	$(TOPDIR)/shared/basetype.h 
	

#cfg/nvram_defaults.o: cfg/vendor_defaults.c $(TOPDIR)/vendors/vendor.cfg
#cnu_service.o: $(TOPDIR)/vendors/vendor.cfg
#sys_config.o: $(TOPDIR)/shared/build_time.h
	
SHARED := ../shared/libshared.a

CFLAGS	+= -I.  -I$(TOPDIR)/shared  -I$(TOPDIR)/cfg/include -D_BIG_ENDIAN
# -DTEST_MODE
#LDFLAGS	+= -lnvram -L$(TOPDIR)/nvram -l8367m -L$(TOPDIR)/lib8367m -lshared -L$(TOPDIR)/shared -lcfg -L$(TOPDIR)/cfg/newcfg -lsmalloc -L$(TOPDIR)/cfg/smalloc 
#LDFLAGS	+= -l8367m -L$(TOPDIR)/lib8367m 

all: master

clean:
	rm -f *.o *~ master

install: all
	$(STRIP) master
	install -D dhcpc.script $(ROOTFSDIR)/usr/share/udhcpc/default.script
	install -D master $(ROOTFSDIR)/usr/sbin/master
	
$(OBJS): $(HEADERS)

master: $(OBJS) $(MMEOBJS) $(LIBMME) $(SHARED)
	$(CC) -o $@ $^ $(LDFLAGS)



