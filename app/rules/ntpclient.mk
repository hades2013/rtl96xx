# Rules for making nvram
NTPCLIENT_DIR = ntpclient

ntpclient-build:
	@$(MAKE) -C $(NTPCLIENT_DIR) \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		all
ntpclient-install:
	@$(MAKE) -C $(NTPCLIENT_DIR) \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install		
ntpclient-clean:
	@$(MAKE) -C $(NTPCLIENT_DIR) clean
	