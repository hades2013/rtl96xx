# Rules for making busybox

BUSYBOX_DIR = busybox-1.20.2

busybox-build:
#	echo $(TOOLCHAIN_DIR)
	@$(MAKE) -C $(BUSYBOX_DIR) \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		all
busybox-install:
	@$(MAKE) -C $(BUSYBOX_DIR) \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install

busybox-menuconfig:
	@$(MAKE) -C $(BUSYBOX_DIR) \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		menuconfig
busybox-clean:
	@$(MAKE) -C $(BUSYBOX_DIR) clean
