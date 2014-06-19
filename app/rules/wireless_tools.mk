# Rules for making wireless_tools

WIRELESS_TOOLS_DIR = wireless_tools

wireless_tools-build:
	@$(MAKE) -C $(WIRELESS_TOOLS_DIR) \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		all
		
wireless_tools-install:
	@$(MAKE) -C $(WIRELESS_TOOLS_DIR) \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install
		
wireless_tools-clean:
	@$(MAKE) -C $(WIRELESS_TOOLS_DIR) clean
