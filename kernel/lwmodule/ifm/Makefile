ifm-objs:=ifm/ker_if_main.o ifm/ker_if_obj.o ifm/ker_if_eth.o ifm/ker_if_ethport.o ifm/ker_if_l3if.o ifm/ker_if_l3vlan.o ifm_dev.o
obj-$(CONFIG_IFM)+= ifm.o 
obj-m+= ifm.o 
EXTRA_CFLAGS += -DMODVERSIONS -I$(LINUXDIR)/include/linux/modversions.h -I$(PDT_INC_DIR) -I$(PLAT_INC_DIR) 
