#
# Makefile for the Linux network (ethercard) device drivers.
#

obj-$(CONFIG_MODULE_BUILD_IN)+=sdk/

ifeq ($(CONFIG_MODULE_BUILD_IN),m) 
ifeq ($(CONFIG_RTK_L34_ENABLE),y)
obj-m +=romeDriver/
endif
else
obj-$(CONFIG_RTK_L34_ENABLE)+=romeDriver/
endif

all:
	$(MAKE) -C sdk all;
	@if egrep "^CONFIG_RTK_L34_ENABLE=y" $(LINUXDIR)/.config > /dev/null; then \
		$(MAKE) -C romeDriver all; \
	fi

install:
	$(MAKE) -C sdk install;
	@if egrep "^CONFIG_RTK_L34_ENABLE=y" $(LINUXDIR)/.config > /dev/null; then \
		$(MAKE) -C romeDriver install; \
	fi

clean:
	$(MAKE) -C sdk clean;
	@if egrep "^CONFIG_RTK_L34_ENABLE=y" $(LINUXDIR)/.config > /dev/null; then \
		$(MAKE) -C romeDriver clean; \
	fi