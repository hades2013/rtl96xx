# Realtek Semiconductor Corp.
#
# Tony Wu (tonywu@realtek.com)
# Oct. 18, 2008
#

#
# RTL8672
#
core-y   += arch/rlx/bsp_rtl8686/
cflags-y += -Iinclude/asm-rlx -Iarch/rlx/bsp_rtl8686/ -Idrivers/net/rtl86900/sdk/include/ -Idrivers/net/rtl86900/sdk/system/include 
load-y   += 0x80041000

extra-y  += vmlinux.lds

obj-y := prom.o setup.o irq.o timer.o serial.o promlib.o cmdline.o
obj-y += gpio.o led-generic.o pushbutton.o rtl_gpio.o rtl_usb_phy.o
ifeq ($(CONFIG_PRODUCT),luna)
ifeq ($(CONFIG_LUNA_DUAL_LINUX),y)
obj-y += board-$(CONFIG_PRD_SIGNATURE)_$(CONFIG_PRJ_SIGNATURE)_master.o
else
obj-y += board-$(CONFIG_PRD_SIGNATURE)_$(CONFIG_PRJ_SIGNATURE).o
endif
else
obj-$(CONFIG_RTL8686_BOARD_001) += board-001.o
obj-$(CONFIG_RTL8686_BOARD_002) += board-002.o
obj-$(CONFIG_RTL8686_BOARD_003) += board-003.o
obj-$(CONFIG_RTL8686_BOARD_004) += board-004.o
obj-$(CONFIG_RTL8686_BOARD_005) += board-005.o
endif
obj-$(CONFIG_GPIO_LED_TR068) += led-tr068.o
obj-$(CONFIG_WPS_LED) += led-wps.o
obj-y += watchdog.o
obj-$(CONFIG_PCI) += pci.o
obj-$(CONFIG_RTL867X_NETLOG) += netlog.o
# 2010-11-19 paula
ifeq ($(CONFIG_RTL8672_SW_USB_LED), y)
obj-y    += led-usb.o
endif
obj-y += kernel_soc_parameters.o

EXTRA_AFLAGS := $(CFLAGS)
#EXTRA_CFLAGS += -I$(DIR_LINUX)/drivers/net/rtl819x/AsicDriver
#EXTRA_CFLAGS += -I$(DIR_LINUX)/drivers/net/rtl819x/common
EXTRA_CFLAGS += -I$(PLAT_INC_DIR)/  -I$(PDT_INC_DIR)/
#krammer
ARCH_VMLINUX  := arch/$(ARCH)/bsp_rtl8686/vmlinux.lds
