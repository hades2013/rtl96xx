# Rules for making auth

AUTH_DIR = auth

auth-build:
	@$(MAKE) -C $(AUTH_DIR) \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		all
		
auth-install:
	@$(MAKE) -C $(AUTH_DIR) \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install
		
auth-clean:
	@$(MAKE) -C $(AUTH_DIR) clean