### module required MICRO 
obj-m := mod_sample.o

PWD := $(shell pwd)
KDIR := /home/xwang/smb/share/plat/openwrt/kamikaze_7.09_Origen/kamikaze_7.09/build_mips/linux-2.6-atheros/linux-2.6.21.5

### target 
all:
	@echo "DIRS:" $(DIRS)
	@echo "DIRFIELS:" $(DIRFILES)
	@echo "SOURCE:" $(SOURCE)
	@echo "OBJS:" $(OBJS)
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=mips CROSS_COMPILE=mips-linux-uclibc- modules
clean:
	rm -rf .*.cmd *.o *.mod.c *.ko

