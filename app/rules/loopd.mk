# Rules for making loopd

loopd-build:
	@$(MAKE) -C loopd \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		all
		
loopd-install:
	@$(MAKE) -C loopd \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install
#	@cp loopd/loopd /tftpboot

loopd-clean:
	@$(MAKE) -C loopd clean
	
