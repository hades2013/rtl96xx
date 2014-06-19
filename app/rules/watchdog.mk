# Rules for making watchdog
WATCHDOG_DIR = watchdog

watchdog-build:
	@$(MAKE) -C $(WATCHDOG_DIR) \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		all
		
watchdog-install:
	@$(MAKE) -C $(WATCHDOG_DIR) \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install
#	@cp $(WATCHDOG_DIR)/app_watchdog /tftpboot

watchdog-clean:
	@$(MAKE) -C $(WATCHDOG_DIR) clean
	
