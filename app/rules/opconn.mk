# Rules for making busybox


opconn-build:
#	echo $(TOOLCHAIN_DIR)
	@$(MAKE) -C opconn \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		all
opconn-install:
	@$(MAKE) -C opconn \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install

opconn-clean:
	@$(MAKE) -C opconn clean
