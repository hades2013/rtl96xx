# Rules for making nvram
MASTER_DIR = master

master-build:
	@$(MAKE) -C $(MASTER_DIR) \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		all
		
master-install:
	@$(MAKE) -C $(MASTER_DIR) \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install
#	@cp $(MASTER_DIR)/master /tftpboot

master-clean:
	@$(MAKE) -C $(MASTER_DIR) clean
	
