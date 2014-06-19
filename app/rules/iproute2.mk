# Rules for making loopd
iproute2-build:
	@$(MAKE) -C iproute2 \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		all
		
iproute2-install:
	@$(MAKE) -C iproute2 \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		romfs
#	@cp loopd/loopd /tftpboot

iproute2-clean:
	@$(MAKE) -C iproute2 clean