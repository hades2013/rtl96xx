# Rules for making nvram

vtysh-build:
	@$(MAKE) -C vtysh \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		all
		
vtysh-install:
	@$(MAKE) -C vtysh \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install
#	@cp vtysh/vtysh /tftpboot

vtysh-clean:
	@$(MAKE) -C vtysh clean
	
