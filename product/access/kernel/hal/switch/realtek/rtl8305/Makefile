

EXTRA_CFLAGS += -I$(SDKDIR)/ \
                -I$(LINUXDIR)/include/linux/ \
                -I$(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/include/ \
                -I$(KERNELDIR)/drv/hal/switch

#obj-y += hello.o
obj-y += rtk_drv_init.o rtk_l2_tx.o rtk_convert.o rtk_drv_vlan.o rtk_drv_statistics.o rtk_drv_special.o rtk_drv_port.o rtk_drv_ratelimit.o rtk_drv_qos.o
obj-y += rtk_drv_mirror.o rtk_drv_ieee8023az.o rtk_drv_cablediag.o rtk_drv_aggr.o rtk_drv_mac.o rtk_drv_dispatch.o rtk_drv_acl.o rtk_drv_rstp.o

#obj-y += rtk_drv_acl.o rtk_drv_gpio.o rtk_drv_eeprom.o

