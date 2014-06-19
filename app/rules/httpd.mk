# Rules for making nvram
HTTPD_DIR = httpd
httpd-build:
	@$(MAKE) -C $(HTTPD_DIR) \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		all
		
httpd-install:
	@$(MAKE) -C $(HTTPD_DIR) \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install

httpd-clean:
	@$(MAKE) -C $(HTTPD_DIR) clean
	
