# Rules for making catvcom

catvcom-build:
	@$(MAKE) -C catvcom \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		all
		
catvcom-install:
	@$(MAKE) -C catvcom \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install
#	@cp catvcom/catvcom /tftpboot

catvcom-clean:
	@$(MAKE) -C catvcom clean
	
