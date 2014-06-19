ifm-objs:=ifm/ker_if_main.lib ifm/ker_if_obj.lib ifm/ker_if_eth.lib ifm/ker_if_ethport.lib ifm/ker_if_l3if.lib ifm/ker_if_l3vlan.lib ifm_dev.lib
obj-$(CONFIG_IFM)+= ifm.o 
obj-m+= ifm.o
EXTRA_CFLAGS += -DMODVERSIONS -I$(LINUXDIR)/include/linux/modversions.h -I$(PDT_INC_DIR) -I$(PLAT_INC_DIR) 
