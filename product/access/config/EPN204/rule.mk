TOPDIR := $(shell pwd)
LINUXDIR := $(TOPDIR)/../../kernel/linux-2.6.21.5
LINUX_CONFIG :=$(TOPDIR)/../../kernel/linux-2.6.21.5/.config
RELEASE_DIR :=$(TOPDIR)/release/$(ENV_PRO)

CPUCHIPID=opl6750
CPUVENDOR=opulan
SWITCHID=rtl8305
SOCID = 63511
SVN_VERSION = $(shell cat ./version)

linux.preinit:
	 printf onu > $(LINUXDIR)/build_uimage/CPUID
linux:linux.preinit
#	$(MAKE) -C $(LINUXDIR) -j1 CROSS_COMPILE="mips-linux-uclibc-" ARCH="mips" CONFIG_SHELL="/bin/bash" CC="mips-linux-uclibc-gcc" V=1
	$(MAKE) -C $(LINUXDIR) -j1 CONFIG_SHELL="/bin/bash" V=1
	$(CROSS_COMPILE)objcopy -O binary -R .note -R .comment -S $(LINUXDIR)/vmlinux $(PRODUCTDIR)/release/$(ENV_PRO)/vmlinux.bin
	@cp -f $(RELEASE_DIR)/vmlinux.bin $(RELEASE_DIR)/vmlinux_org.bin
	$(TOOLS_DIR)/mk_linux.sh $(TOOLS_DIR) $(SOCID) $(SVN_VERSION) $(LINUXDIR) $(RELEASE_DIR)
	@rm -f $(RELEASE_DIR)/vmlinux_org.bin
	@mv $(RELEASE_DIR)/vmlinux.bix $(RELEASE_DIR)/vmlinux.bin
	$(MAKE) -C $(LINUXDIR) zImage -j1 CONFIG_SHELL="/bin/bash" V=1
	@cp $(LINUXDIR)/arch/mips/boot/compressed/images/zImage ./zImage
	@sh $(TOOLS_DIR)/calc.sh $(LINUXDIR)/include/linux/kernel_size.h
	@rm -f ./zImage
	@rm -f $(RELEASE_DIR)/vmlinux.bin
	@rm -f $(RELEASE_DIR)/vmlinux.bix

	$(MAKE) -C $(LINUXDIR) -j1 CONFIG_SHELL="/bin/bash" V=1
	$(CROSS_COMPILE)objcopy -O binary -R .note -R .comment -S $(LINUXDIR)/vmlinux $(PRODUCTDIR)/release/$(ENV_PRO)/vmlinux.bin
	@cp -f $(RELEASE_DIR)/vmlinux.bin $(RELEASE_DIR)/vmlinux_org.bin
	$(TOOLS_DIR)/mk_linux.sh $(TOOLS_DIR) $(SOCID) $(SVN_VERSION) $(LINUXDIR) $(RELEASE_DIR)
	@rm -f $(RELEASE_DIR)/vmlinux_org.bin
	@mv $(RELEASE_DIR)/vmlinux.bix $(RELEASE_DIR)/vmlinux.bin
	$(MAKE) -C $(LINUXDIR) zImage -j1 CONFIG_SHELL="/bin/bash" V=1
	@cp $(LINUXDIR)/arch/mips/boot/compressed/images/zImage $(RELEASE_DIR)/zImage
	@rm -f $(RELEASE_DIR)/vmlinux.bin
	@rm -f $(RELEASE_DIR)/vmlinux.bix
linux-clean:
#	$(MAKE) -C $(LINUXDIR) clean  -j1 CROSS_COMPILE="mips-linux-uclibc-" ARCH="mips" CONFIG_SHELL="/bin/bash" CC="mips-linux-uclibc-gcc" V=1
	$(MAKE) -C $(LINUXDIR) clean  -j1 
	echo "#define KERNEL_DYNAMIC_SIZE 0" > $(LINUXDIR)/include/linux/kernel_size.h
	rm -rf $(LINUXDIR)/arch/mips/boot/compressed/iDiSX/zvmlinux
	rm -rf $(LINUXDIR)/arch/mips/boot/compressed/iDiSX/zvmlinux.bin
	rm -rf $(LINUXDIR)/arch/mips/boot/compressed/iDiSX/zvmlinux.temp
	rm -rf $(LINUXDIR)/arch/mips/boot/compressed/images/vmlinux.bin
	rm -rf $(LINUXDIR)/arch/mips/boot/compressed/images/vmlinux.gz
	rm -rf $(LINUXDIR)/arch/mips/boot/compressed/images/zImage
linux-menuconfig:
	$(MAKE) -C $(LINUXDIR) menuconfig  -j1 
uboot:
	cd $(UBOOTDIR);	make f13;sh ./romversion.sh;sh ./mk.sh
	cp -f $(UBOOTDIR)/u-boot.bin $(PRODUCTDIR)/release/$(ENV_PRO)
uboot-clean:
	$(MAKE) -C $(UBOOTDIR) clean
busybox-menuconfig:
	$(MAKE) -C $(SOFTWAREDIR)/busybox-1.20.2 menuconfig
linux-modules:
	@$(MAKE) -C $(LINUXDIR) modules
	$(MAKE) -C $(LINUXDIR) SUBDIRS=$(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_dal_driver modules
	$(MAKE) -C $(LINUXDIR) SUBDIRS=$(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_cpuif modules
	$(MAKE) -C $(LINUXDIR) SUBDIRS=$(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_minte modules
	$(MAKE) -C $(LINUXDIR) SUBDIRS=$(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_reg modules
	$(MAKE) -C $(LINUXDIR) SUBDIRS=$(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_wdt modules
	$(MAKE) -C $(LINUXDIR) SUBDIRS=$(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_uptm_intr modules
linux-modules-install:
	$(MAKE) -C $(LINUXDIR) modules_install DEPMOD=/bin/true INSTALL_MOD_PATH=$(ROOTFSDIR)
	@find $(ROOTFSDIR)/lib/modules -type l | xargs rm -fr
linux-modules-clean:
	rm -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_dal_driver/*.ko 
	rm -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_cpuif/*.ko
	rm -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_minte/*.ko 
	rm -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_reg/*.ko 
	rm -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_wdt/*.ko 
	rm -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_uptm_intr/*.ko
	rm -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_dal_driver/*.o 
	rm -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_cpuif/*.o
	rm -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_minte/*.o 
	rm -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_reg/*.o 
	rm -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_wdt/*.o 
	rm -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_uptm_intr/*.o
rootfs:
	if [ ! -d $(PRODUCTDIR)/rootfs ]; then tar -zxvf $(CONFIGDIR)/rootfs.tgz -C $(PRODUCTDIR);mv root rootfs;fi
	@ echo "install app "
	$(MAKE) -C $(SOFTWAREDIR) install
	install -p $(CONFIGDIR)/rc $(ROOTFSDIR)/etc/
	install -p $(CONFIGDIR)/startup $(ROOTFSDIR)/etc/config/
	install -p $(CONFIGDIR)/ifm.ko $(ROOTFSDIR)/usr/lib
	install -p $(PRODUCTDIR)/lwlib/libcfg.so $(ROOTFSDIR)/usr/lib
	install -p $(PRODUCTDIR)/lwlib/libifm.so $(ROOTFSDIR)/usr/lib
	@ echo "install linux modules"
	make linux-modules-install
	cp -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_dal_driver/*.ko $(ROOTFSDIR)/modules/
	cp -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_cpuif/*.ko $(ROOTFSDIR)/modules/
	cp -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_minte/*.ko $(ROOTFSDIR)/modules/
	cp -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_reg/*.ko $(ROOTFSDIR)/modules/
	cp -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_wdt/*.ko $(ROOTFSDIR)/modules/
	cp -f $(KERNELDIR)/drv/hal/cpu/mips/$(CPUVENDOR)/$(CPUCHIPID)/module/opl_uptm_intr/*.ko $(ROOTFSDIR)/modules/
#endif
#	@echo "=== Preparing modpost "
#	@if [ ! -f $(LINUXDIR)/scripts/mod/modpost ]; then \
#		cd $(LINUXDIR);make -j1 CROSS_COMPILE="mips-linux-uclibc-" ARCH="mips" CONFIG_SHELL="/bin/bash" CC="mips-linux-uclibc-gcc" V=1 scripts;export CPUID=$(cat $(LINUXDIR)/build_uimage/CPUID);sh mklzma.sh; \
	fi
	@echo "=== Preparing UART1 modules "
#	-@cp $(TARGETDIR)/apps/opconn/linux/uart_drv.ko $(KERNEL_DIR)/linux-2.6.21.5/root/modules/
	@echo "=== Preparing xmodem "
#	-@cd $(TARGETDIR)/apps/opconn/linux/opl_xmodem; make clean;make
#	-@cp $(TARGETDIR)/apps/opconn/linux/opl_xmodem/opl_xmodem.ko $(KERNEL_DIR)/linux-2.6.21.5/root/modules/
	@echo "=== Preparing busybox "
#	cd $(TARGETDIR)/busybox/busybox-1.15.2/;make install;cp -rfP $(TARGETDIR)/busybox/busybox-1.15.2/_install/* $(KERNEL_DIR)/linux-2.6.21.5/root/
#	@echo "=== Preparing pppoe "
	@echo "=== Preparing modules"
#	@cd $(KERNEL_DIR)/modules/build;make clean;make
#	@cp $(KERNEL_DIR)/modules/opl_cpuif/opl_cpuif.ko $(KERNEL_DIR)/linux-2.6.21.5/root/modules/
#	@cp $(KERNEL_DIR)/modules/opl_reg/opl_reg_mmap.ko $(KERNEL_DIR)/linux-2.6.21.5/root/modules/
#	@cp $(KERNEL_DIR)/modules/opl_minte/opl_minte.ko $(KERNEL_DIR)/linux-2.6.21.5/root/modules/
#	@cp $(KERNEL_DIR)/modules/opl_uptm_intr/uptm_intr.ko $(KERNEL_DIR)/linux-2.6.21.5/root/modules/
#	@rm -rf $(KERNEL_DIR)/linux-2.6.21.5/root/modules/tm.ko
#	@if [ $(shell if [ -f $(SOFTWAREDIR)/opconn/project.config ]; then cat $(SOFTWAREDIR))/opconn/project.config; else printf ""; fi) == "ONU_1PORT" ] ; then \
#		cd $(KERNEL_DIR)/modules/build;make tmc; \
#	#	cp $(KERNEL_DIR)/modules/opl_tmc/tm.ko $(KERNEL_DIR)/linux-2.6.21.5/root/modules/; 
#	fi
	@echo "=== Strip unneeded"
	find $(ROOTFSDIR) -name ".svn" | xargs rm -rf
	#find $(KERNEL_DIR)/linux-2.6.21.5/root -type f ! -name "ld*" | xargs file | grep ELF | awk -F: '{print $$1}' | xargs -tl mips-linux-uclibc-strip --strip-unneeded
	@echo "=== Installing kernel "
#	@cd $(LINUXDIR);export CPUID=`cat $(LINUXDIR)/linux-2.6.21.5/build_uimage/CPUID`;sh mklzma.sh
#	cp -f $(KERNEL_DIR)/linux-2.6.21.5/wrt_uImage $(TARGETDIR)/build/opvos.bin
#	@echo "=== Installing u-boot "
#	-@cp -f $(TARGETDIR)/u-boot/u-boot.bin $(TARGETDIR)/build/

ramfs:
	cd ${ROOTFSDIR} && find . | cpio --format=newc -oF ${LINUXDIR}/usr/initramfs_data.cpio
	gzip -9 < ${LINUXDIR}/usr/initramfs_data.cpio > ${LINUXDIR}/usr/initramfs_data.cpio.gz	
	${MAKE} -C $(LINUXDIR) vmlinux  
	rm -rf ${LINUXDIR}/usr/initramfs_data.cpio.gz
	rm -rf ${LINUXDIR}/usr/initramfs_data.cpio
	cd $(LINUXDIR);export CPUID=`cat $(LINUXDIR)/build_uimage/CPUID`;sh mklzma.sh
	cp -f $(LINUXDIR)/wrt_uImage $(PRODUCTDIR)/release/$(ENV_PRO)/opvos.bin
.PHONY: mksquashfs_lzma
mksquashfs_lzma:
	${MAKE} -C ${TOOLS_DIR}/sqlzma all
squashfs-clean:
	${MAKE} -C ${TOOLS_DIR}/sqlzma clean
makeapp:	
	cp -f $(TOOLS_DIR)/makeapp.c ./makeapp.c
	gcc -o makeapp makeapp.c 
mkuboot:	
	${MAKE} -C ${TOOLS_DIR}/mkboot all
app: mksquashfs_lzma
	@echo "make app"
	@if egrep "^CONFIG_SQUASHFS_LZMA=y" $(LINUX_CONFIG) > /dev/null; then \
        ./tools/sqlzma/squashfs3.3/squashfs-tools/mksquashfs $(ROMFSDIR) $(RELEASE_DIR)/squashfs_lzma.bin -be -noappend -no-fragments; \
	fi   
	@cp -f $(LINUXDIR)/vmlinux $(RELEASE_DIR)/vmlinux
	@cp -f $(LINUXDIR)/System.map $(RELEASE_DIR)/System.map
	@if egrep "^CONFIG_MTD_DYNAMIC_FLASH_PARTITION=y" $(LINUX_CONFIG) > /dev/null; then \
        bash ${TOOLS_DIR}/mk_app.sh $(RELEASE_DIR) $(CONFIG_RELEASE_VER) $(TOOLS_DIR) 1;\
    	else \
        bash ${TOOLS_DIR}/mk_app.sh $(RELEASE_DIR) $(CONFIG_RELEASE_VER) $(TOOLS_DIR) 0;\
	fi	
	@echo "make app finish .."
image: mkuboot makeapp 
	cp -f $(PRODUCTDIR)/release/$(ENV_PRO)/u-boot.bin ./u-boot.bin
	cp -f $(TOOLS_DIR)/mkboot/mkuboot ./mkuboot
	${MAKE} -C ${TOOLS_DIR}/mkboot clean
	cp -f $(PRODUCTDIR)/release/$(ENV_PRO)/$(CONFIG_RELEASE_VER).bin ./temp_for_app.bin
	./makeapp 101 $(CONFIG_RELEASE_VER)  temp_for_app.bin  u-boot.bin 0 $(CONFIG_PRODUCT_NAME)
	./mkuboot u-boot.bin
	rm -f temp_for_app.bin
	rm -rf $(PRODUCTDIR)/release/$(ENV_PRO)/u-boot.bin
	mv *.bin *.img $(PRODUCTDIR)/release/$(ENV_PRO)
	rm -f *.bin makeapp.c makeapp mkuboot
platform.setup:
	rm -f $(SOFTWAREDIR)/shared/drv/port_config.c
	rm -f $(KERNELDIR)/drv/hal/switch/drv_common.c
	rm -f $(KERNELDIR)/drv/hal/switch/port_config.c
	rm -rf $(LINUXDIR)/drivers/lwdrv
	rm -rf $(LINUXDIR)/drivers/l2trans
	rm -f $(TOOLS_DIR)/mkimage-new
	rm -f $(PRODUCTDIR)/version
	rm -f $(PRODUCTDIR)/config.in
	rm -rf $(TOPDIR)/../../kernel/drv/hal
	rm -rf $(TOPDIR)/../../kernel/linux-2.6.21.5
	ln -sf $(TOPDIR)/kernel/hal $(TOPDIR)/../../kernel/drv/hal
	ln -sf $(TOPDIR)/kernel/linux-2.6.21.5 $(TOPDIR)/../../kernel/linux-2.6.21.5
	ln -s $(CONFIGDIR)/port_config.c $(SOFTWAREDIR)/shared/drv/port_config.c
	ln -s $(SOFTWAREDIR)/shared/drv/drv_common.c $(KERNELDIR)/drv/hal/switch/drv_common.c
	ln -s $(CONFIGDIR)/port_config.c $(KERNELDIR)/drv/hal/switch/port_config.c
	ln -sf $(KERNELDIR)/drv $(LINUXDIR)/drivers/lwdrv
	ln -s $(KERNELDIR)/lwmodule/l2trans   $(LINUXDIR)/drivers/l2trans
	ln -s $(LINUXDIR)/build_uimage/mkimage-new	$(TOOLS_DIR)/mkimage-new
	ln -s $(LINUXDIR)/build_uimage/version $(PRODUCTDIR)/version
	cp -rf $(CONFIGDIR)/config.in $(PRODUCTDIR)/config.in
	cp -rf $(CONFIGDIR)/main.config $(PRODUCTDIR)/.config
	cp -rf $(CONFIGDIR)/busybox.config $(SOFTWAREDIR)/busybox-1.20.2/.config
	$(MAKE) -C $(PRODUCTDIR) oldconfig
	$(MAKE) -C $(SOFTWAREDIR)/busybox-1.20.2 oldconfig
platform.distclean:
	rm -f $(SOFTWAREDIR)/shared/drv/port_config.c
	rm -f $(SOFTWAREDIR)/shared/drv/drv_managevlan.c
	rm -f $(KERNELDIR)/drv/hal/switch/drv_common.c
	rm -f $(KERNELDIR)/drv/hal/switch/port_config.c
	rm -rf $(LINUXDIR)/drivers/lwdrv
	rm -rf $(LINUXDIR)/drivers/l2trans
	rm -rf $(TOOLS_DIR)/mkimage-new
	rm -rf $(PRODUCTDIR)/version
	rm -f $(PRODUCTDIR)/config.in
	rm -rf $(TOPDIR)/../../kernel/drv/hal
	rm -rf $(TOPDIR)/../../kernel/linux-2.6.21.5
	rm -rf $(PDT_INC_DIR)/lw_config.h	
	rm -f $(PRODUCTDIR)/.config
	rm -f .env
product.setup:
	cp -f $(CONFIGDIR)/linux.config $(LINUXDIR)/.config
	$(MAKE) -C $(LINUXDIR) oldconfig
	if [ ! -d $(LINUXDIR)/drivers/switch ] ; then \
		mkdir $(LINUXDIR)/drivers/switch;\
	fi
	rm -rf $(LINUXDIR)/drivers/switch/sdk
	ln -sf $(SDKDIR) $(LINUXDIR)/drivers/switch/sdk
product.distclean:
	rm -rf $(LINUXDIR)/drivers/switch/sdk
	rm -rf $(LINUXDIR)/drivers/switch
