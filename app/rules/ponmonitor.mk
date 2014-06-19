# Rules for making nvram

ponmonitor-build:
	@$(MAKE) -C ponmonitor \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		all
		
ponmonitor-install:
	@$(MAKE) -C ponmonitor \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install
#	@cp ponmonitor/ponmonitor /tftpboot

ponmonitor-clean:
	@$(MAKE) -C ponmonitor clean
	
