TOPDIR := $(shell pwd)
LINUXDIR := $(TOPDIR)/../../kernel/linux-2.6.x
LINUX_CONFIG :=$(TOPDIR)/../../kernel/linux-2.6.x/.config
RELEASE_DIR :=$(TOPDIR)/release/$(ENV_PRO)

LWDRV_DIR=$(TOPDIR)/../../kernel/drv
APOLLODIR=$(LINUXDIR)/drivers/net/rtl86900
KERNEL_DIR=$(LINUXDIR)
CPUCHIPID=RTL9607
CPUVENDOR=realtek
SWITCHID=rtl9607

export LWDRV_DIR APOLLODIR LINUXDIR KERNEL_DIR

SOCID = 0
SVN_VERSION = $(shell cat ./version)

linux:
#	$(MAKE) -C $(LINUXDIR) -j1 CROSS_COMPILE="mips-linux-uclibc-" ARCH="mips" CONFIG_SHELL="/bin/bash" CC="mips-linux-uclibc-gcc" V=1
	$(MAKE) -C $(LINUXDIR) -j1 CONFIG_SHELL="/bin/bash" V=1
	$(CROSS_COMPILE)objcopy -O binary -R .note -R .comment -S $(LINUXDIR)/vmlinux $(PRODUCTDIR)/release/$(ENV_PRO)/vmlinux.bin
	@cp -f $(RELEASE_DIR)/vmlinux.bin $(RELEASE_DIR)/vmlinux_org.bin
	$(TOOLS_DIR)/mk_linux.sh $(TOOLS_DIR) $(SOCID) $(SVN_VERSION) $(LINUXDIR) $(RELEASE_DIR)
	@rm -f $(RELEASE_DIR)/vmlinux_org.bin
	@mv $(RELEASE_DIR)/vmlinux.bix $(RELEASE_DIR)/vmlinux.bin
	$(MAKE) -C $(LINUXDIR) zImage -j1 CONFIG_SHELL="/bin/bash" V=1
	@cp $(LINUXDIR)/arch/rlx/boot/compressed/images/zImage ./zImage
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
	@cp $(LINUXDIR)/arch/rlx/boot/compressed/images/zImage $(RELEASE_DIR)/zImage
	@rm -f $(RELEASE_DIR)/vmlinux.bin
	@rm -f $(RELEASE_DIR)/vmlinux.bix
linux-clean:
	$(MAKE) -C $(LINUXDIR) clean  -j1
	echo "#define KERNEL_DYNAMIC_SIZE 0" > $(LINUXDIR)/include/linux/kernel_size.h
	rm -rf $(LINUXDIR)/arch/rlx/boot/compressed/iDiSX/zvmlinux
	rm -rf $(LINUXDIR)/arch/rlx/boot/compressed/iDiSX/zvmlinux.bin
	rm -rf $(LINUXDIR)/arch/rlx/boot/compressed/iDiSX/zvmlinux.temp
	rm -rf $(LINUXDIR)/arch/rlx/boot/compressed/images/vmlinux.bin
	rm -rf $(LINUXDIR)/arch/rlx/boot/compressed/images/vmlinux.gz
	rm -rf $(LINUXDIR)/arch/rlx/boot/compressed/images/zImage
linux-menuconfig:
	$(MAKE) -C $(LINUXDIR) menuconfig  -j1 

uboot:uboot-clean
	$(MAKE) -C $(PRELOADERDIR) all -j1
	$(MAKE) -C $(UBOOTDIR) -j1
	$(MAKE) -C $(PRELOADERDIR) compose -j1
#	cp -rf $(UBOOTDIR)/tools/mkimage $(LINUXDIR)/
	cp -rf $(PRELOADERDIR)/release/loader.img $(PRODUCTDIR)/release/$(ENV_PRO)/u-boot.bin
	
uboot-menuconfig:
	$(MAKE) -C $(UBOOTDIR) menuconfig -j1
	
uboot-clean:
	$(MAKE) -C $(PRELOADERDIR) clean -j1
	$(MAKE) -C $(UBOOTDIR) clean -j1
	
busybox-menuconfig:
	$(MAKE) -C $(SOFTWAREDIR)/busybox-1.20.2 menuconfig
	
linux-modules:
#	@$(MAKE) -C $(LINUXDIR) modules
	@$(MAKE) -C $(LINUXDIR)/drivers/net/rtl86900 all 

linux-modules-install:
#	$(MAKE) -C $(LINUXDIR) modules_install DEPMOD=/bin/true INSTALL_MOD_PATH=$(ROOTFSDIR)
	@rm -rf $(ROOTFSDIR)/lib/modules
	@mkdir $(ROOTFSDIR)/lib/modules
	$(MAKE) -C $(LINUXDIR)/drivers/net/rtl86900 install
	@find $(ROOTFSDIR)/lib/modules -type l | xargs rm -fr
	
linux-modules-clean:sdk-app-clean
	$(MAKE) -C $(LINUXDIR)/drivers/net/rtl86900 clean

sdk-app:
#To make diag we must: (1)make src all > librtk.a (2)make unittest all > libut.a
	echo $(CONFIGDIR)
	echo $(ROOTFSDIR)
	$(MAKE) -C $(LINUXDIR)/drivers/net/rtl86900/sdk/src all
	$(MAKE) -C $(LINUXDIR)/drivers/net/rtl86900/object/unittest all
	$(MAKE) -C $(PRODUCTDIR)/app all

oam-vendor:
	if [ -d $(PRODUCTDIR)/app/epon_oam/src/vendor ];then $(MAKE) -C $(PRODUCTDIR)/app/epon_oam LIBVENDOR;fi
oam-vendor-clean:
	if [ -d $(PRODUCTDIR)/app/epon_oam/src/vendor ];then $(MAKE) -C $(PRODUCTDIR)/app/epon_oam LIBVENDOR-clean;fi
sdk-app-clean:
	$(MAKE) -C $(PRODUCTDIR)/app clean
	$(MAKE) -C $(LINUXDIR)/drivers/net/rtl86900/sdk/src clean

rootfs:oam-vendor-clean oam-vendor sdk-app
	if [ ! -d $(PRODUCTDIR)/rootfs ]; then tar -zxvf $(CONFIGDIR)/rootfs.tgz -C $(PRODUCTDIR);mv root rootfs;fi
	@ echo "install app "
	$(MAKE) -C $(SOFTWAREDIR) install
	echo $(CONFIGDIR)
	echo $(ROOTFSDIR)
	install -p $(CONFIGDIR)/rc $(ROOTFSDIR)/etc/
	install -p $(CONFIGDIR)/inittab $(ROOTFSDIR)/etc/inittab
	install -p $(CONFIGDIR)/startup $(ROOTFSDIR)/etc/config/
	install -p $(KERNELDIR)/lwmodule/ifm/ifm.ko $(ROOTFSDIR)/usr/lib
	install -p $(PRODUCTDIR)/lwlib/libcfg.so $(ROOTFSDIR)/usr/lib
	install -p $(CONFIGDIR)/libifm.so $(ROOTFSDIR)/usr/lib
	@ echo "install linux modules"
	make linux-modules-install
#	mv $(ROOTFSDIR)/lib/modules/*.ko $(ROOTFSDIR)/modules/
	@ echo "install sdk app"
	$(MAKE) -C $(PRODUCTDIR)/app install
	cp -rf $(PRODUCTDIR)/app/bin/* $(ROOTFSDIR)/usr/sbin/
#	cp -rf $(PRODUCTDIR)/lwlib/*.so $(ROOTFSDIR)/lib/
	@echo "=== Strip unneeded ==="
	find $(ROOTFSDIR) -name ".svn" | xargs rm -rf
	touch $(ROOTFSDIR)/.init_enable_core

ramfs:
	cd ${ROOTFSDIR} && find . | cpio --format=newc -oF ${LINUXDIR}/usr/initramfs_data.cpio
	gzip -9 < ${LINUXDIR}/usr/initramfs_data.cpio > ${LINUXDIR}/usr/initramfs_data.cpio.gz	
	${MAKE} -C $(LINUXDIR) vmlinux  
	rm -rf ${LINUXDIR}/usr/initramfs_data.cpio.gz
	rm -rf ${LINUXDIR}/usr/initramfs_data.cpio
	$(CROSS_COMPILE)objcopy -O binary -R .note -R .comment -S $(LINUXDIR)/vmlinux $(RELEASE_DIR)/vmlinux_org.bin
	$(TOOLS_DIR)/mk_linux.sh $(TOOLS_DIR) $(SOCID) $(SVN_VERSION) $(LINUXDIR) $(RELEASE_DIR)
	rm -f $(RELEASE_DIR)/vmlinux_org.bin
	mv $(RELEASE_DIR)/vmlinux.bix $(RELEASE_DIR)/initrd
	
.PHONY: mksquashfs_lzma
mksquashfs_lzma:
	${MAKE} -C ${TOOLS_DIR}/squashfs4.2/squashfs-tools all
#	${MAKE} -C ${TOOLS_DIR}/squashfs4.0/squashfs-tools all
squashfs-clean:
	${MAKE} -C ${TOOLS_DIR}/squashfs4.2/squashfs-tools clean
#	${MAKE} -C ${TOOLS_DIR}/squashfs4.0/squashfs-tools clean
makeapp:	
	cp -f $(TOOLS_DIR)/makeapp.c ./makeapp.c
	gcc -o makeapp makeapp.c
app: mksquashfs_lzma
	@echo "make app"
	@if egrep "^CONFIG_SQUASHFS_LZMA=y" $(LINUX_CONFIG) > /dev/null; then \
		./tools/squashfs4.2/squashfs-tools/mksquashfs $(ROMFSDIR) $(RELEASE_DIR)/squashfs_lzma.bin -noappend -no-fragments; \
#		./tools/squashfs4.0/squashfs-tools/mksquashfs $(ROMFSDIR) $(RELEASE_DIR)/squashfs_lzma.bin -noappend -no-fragments; \
	fi
	@cp -f $(LINUXDIR)/vmlinux $(RELEASE_DIR)/vmlinux
	@cp -f $(LINUXDIR)/System.map $(RELEASE_DIR)/System.map
	@if egrep "^CONFIG_MTD_DYNAMIC_FLASH_PARTITION=y" $(LINUX_CONFIG) > /dev/null; then \
        bash ${TOOLS_DIR}/mk_app.sh $(RELEASE_DIR) $(CONFIG_RELEASE_VER) $(TOOLS_DIR) 1;\
    	else \
        bash ${TOOLS_DIR}/mk_app.sh $(RELEASE_DIR) $(CONFIG_RELEASE_VER) $(TOOLS_DIR) 0;\
	fi	
	@echo "make app finish .."
image: makeapp
	cp -f $(PRODUCTDIR)/release/$(ENV_PRO)/u-boot.bin ./u-boot.bin
	cp -f $(PRODUCTDIR)/release/$(ENV_PRO)/$(CONFIG_RELEASE_VER).bin ./temp_for_app.bin
	./makeapp 101 $(CONFIG_RELEASE_VER) temp_for_app.bin u-boot.bin 0 $(ENV_PRO)
	rm -f u-boot.bin temp_for_app.bin
	mv *.img $(PRODUCTDIR)/release/$(ENV_PRO)
	rm -f makeapp.c makeapp
platform.setup:
	rm -f $(SOFTWAREDIR)/shared/drv/port_config.c
	rm -f $(LWDRV_DIR)/hal/switch/drv_common.c
	rm -f $(LWDRV_DIR)/hal/switch/port_config.c
	rm -rf $(LINUXDIR)/drivers/lwdrv
	rm -rf $(LINUXDIR)/drivers/l2trans
	rm -rf $(TOOLS_DIR)/mkimage-new
	rm -f $(PRODUCTDIR)/version
	rm -f $(PRODUCTDIR)/config.in
	rm -rf $(TOPDIR)/../../kernel/drv/hal
	rm -rf $(TOPDIR)/../../kernel/linux-2.6.x
	rm -f $(PRODUCTDIR)/include/pdt_priv_config.h
	ln -s $(CONFIGDIR)/pdt_priv_config.h  $(PRODUCTDIR)/include
	ln -sf $(TOPDIR)/kernel/hal $(TOPDIR)/../../kernel/drv/hal
	ln -sf $(TOPDIR)/kernel/linux-2.6.x $(TOPDIR)/../../kernel/linux-2.6.x
	ln -s $(CONFIGDIR)/port_config.c $(SOFTWAREDIR)/shared/drv/port_config.c
	ln -s $(SOFTWAREDIR)/shared/drv/drv_common.c $(LWDRV_DIR)/hal/switch/drv_common.c	
	ln -s $(CONFIGDIR)/port_config.c $(LWDRV_DIR)/hal/switch/port_config.c
	ln -sf $(LWDRV_DIR) $(LINUXDIR)/drivers/lwdrv
	ln -s $(KERNELDIR)/lwmodule/l2trans $(LINUXDIR)/drivers/l2trans
	ln -sf $(LINUXDIR)/mkimage $(TOOLS_DIR)/mkimage-new
	cp -rf $(CONFIGDIR)/version $(PRODUCTDIR)/version
	cp -rf $(CONFIGDIR)/libvendor.a $(PRODUCTDIR)/app/epon_oam
	cp -rf $(CONFIGDIR)/config.in $(PRODUCTDIR)/config.in
	cp -rf $(CONFIGDIR)/main.config $(PRODUCTDIR)/.config
	cp -rf $(CONFIGDIR)/busybox.config $(SOFTWAREDIR)/busybox-1.20.2/.config
	cp -rf $(CONFIGDIR)/uboot.config $(UBOOTDIR)/.config	
	cp -rf $(CONFIGDIR)/libifm.* $(PRODUCTDIR)/lwlib
	cp -rf $(CONFIGDIR)/soc.tcl $(PRELOADERDIR)/vendors/luna_cli_demo/soc.tcl
	$(MAKE) -C $(PRODUCTDIR) oldconfig
	$(MAKE) -C $(PRELOADERDIR) preconfig_luna_cli_demo -j1
	$(MAKE) -C $(UBOOTDIR) oldconfig
	$(MAKE) -C $(SOFTWAREDIR)/busybox-1.20.2 oldconfig
platform.distclean:
	rm -f $(SOFTWAREDIR)/shared/drv/port_config.c
	rm -f $(LWDRV_DIR)/hal/switch/port_config.c
	rm -f $(LWDRV_DIR)/hal/switch/drv_common.c	
	rm -rf $(LINUXDIR)/drivers/lwdrv
	rm -rf $(LINUXDIR)/drivers/l2trans
	rm -rf $(TOOLS_DIR)/mkimage-new
	rm -f $(PRODUCTDIR)/version
	rm -f $(PRODUCTDIR)/config.in
	rm -rf $(TOPDIR)/../../kernel/drv/hal
	rm -rf $(TOPDIR)/../../kernel/linux-2.6.x
	rm -rf $(PDT_INC_DIR)/lw_config.h
	rm -f $(PRODUCTDIR)/.config
	rm -f .env
product.setup:
	rm -rf $(LINUXDIR)/drivers/net/rtl86900/Compiler_Flag
	rm -rf $(LINUXDIR)/drivers/net/rtl86900/sdk
	ln -sf	$(CONFIGDIR)/Compiler_Flag $(LINUXDIR)/drivers/net/rtl86900/Compiler_Flag
	ln -sf $(SDKDIR) $(LINUXDIR)/drivers/net/rtl86900/sdk
	rm -rf $(LINUXDIR)/rtk_voip
	rm -rf $(LINUXDIR)/drivers/char/aipc
	ln -sf $(TOPDIR)/kernel/rtk_voip/kernel $(LINUXDIR)/rtk_voip
	ln -sf $(TOPDIR)/kernel/rtk_voip/aipc_char $(LINUXDIR)/drivers/char/aipc
#	@cd $(ROOTDIR)/user          ; ln -sf ../rtk_voip/solar_1.6        rtk_voip
#	@cd $(ROOTDIR)/user          ; ln -sf ../rtk_voip/aipc_util        aipc_util
	rm -rf $(LINUXDIR)/.config
	cp -rf $(CONFIGDIR)/linux.config $(LINUXDIR)/.config
	$(MAKE) -C $(LINUXDIR) oldconfig
#	if [ ! -d $(LINUXDIR)/drivers/switch ] ; then \
#		mkdir $(LINUXDIR)/drivers/switch;\
#	fi
#	rm -rf $(LINUXDIR)/drivers/switch/sdk
#	ln -sf $(SDKDIR) $(LINUXDIR)/drivers/switch/sdk
product.distclean:
	rm -rf $(LINUXDIR)/drivers/net/rtl86900/Compiler_Flag
	rm -rf $(LINUXDIR)/drivers/net/rtl86900/sdk
	rm -rf $(LINUXDIR)/rtk_voip
	rm -rf $(LINUXDIR)/drivers/char/aipc
	rm -rf $(LINUXDIR)/.config
#	rm -rf $(LINUXDIR)/drivers/switch/sdk
#	rm -rf $(LINUXDIR)/drivers/switch
