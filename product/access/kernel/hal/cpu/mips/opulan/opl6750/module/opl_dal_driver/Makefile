opl_dal_driver-objs:= bridgemac.o bridgeport.o bridgevlan.o classify.o hw_interface.o i2c.o \
	phy.o interrupt.o  opconn_lib.o opconnmisc.o opl_debug.o \
	opl_errno.o  opl_utils.o peripheral.o \
	pon.o tm.o wdt.o  uart.o dal_stats.o \
	dal_cls.o dal_dba.o dal_lib.o dal_mac.o dal_multicast.o \
	dal_pon.o dal_port.o dal_ver.o dal_vtt.o dal_storm.o \
	dal_vlan.o dal_rstp.o \
	opconn_ioctl.o \
	opldrv_module.o \
	ponmonitor.o
obj-m := opl_dal_driver.o
EXTRA_CFLAGS	+= -I$(TOPDIR)  -I$(TOPDIR)/expo -I$(PDT_INC_DIR)


