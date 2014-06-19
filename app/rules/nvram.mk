# Rules for making nvram
NVRAM_DIR = nvram

nvram-build:
	@$(MAKE) -C $(NVRAM_DIR) \
		CC="$(CROSS_COMPILE)gcc" \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		all
nvram-install:
	@$(MAKE) -C $(NVRAM_DIR) \
		CC="$(CROSS_COMPILE)gcc" \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install

nvram-clean:
	@$(MAKE) -C $(NVRAM_DIR) clean
	