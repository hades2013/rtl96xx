# Rules for making nvram
SHARED_DIR = shared
NOWTIME=$(shell date +%s)

shared-build:
	echo "#define SYS_BUILD_TIME ${NOWTIME}" > $(SHARED_DIR)/build_time.h 
	echo "#define SYS_BUILD_COUNT 9" >> $(SHARED_DIR)/build_time.h 
	@$(MAKE) -C $(SHARED_DIR) \
		CC="$(CROSS_COMPILE)gcc" \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		all
shared-install:
	@$(MAKE) -C $(SHARED_DIR) \
		CC="$(CROSS_COMPILE)gcc" \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install

shared-clean:
	@$(MAKE) -C $(SHARED_DIR) clean
	