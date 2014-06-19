MODULE_DIR := $(shell if [ -f ~/opl_xmodem.config ]; then cat ~/opl_xmodem.config; else /bin/pwd > ~/opl_xmodem.config; /bin/pwd; fi)

opl_xmodem-objs = ufile.o xmodem.o
obj-m= opl_xmodem.o
MODULE_NAME = opl_xmodem.ko

include $(MODULE_DIR)/Makefile.conf


