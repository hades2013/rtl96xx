# Rules for making nvram
LIB8367M_DIR = lib8367m

lib8367m-build:
	@$(MAKE) -C $(LIB8367M_DIR) \
		CC="$(CROSS_COMPILE)gcc" \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		all
lib8367m-install:
	@$(MAKE) -C $(LIB8367M_DIR) \
		CC="$(CROSS_COMPILE)gcc" \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install

lib8367m-clean:
	@$(MAKE) -C $(LIB8367M_DIR) clean
	