# Rules for making utils
UTILS_DIR = utils

utils-build:
	@$(MAKE) -C $(UTILS_DIR) \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		all
		
utils-install:
	@$(MAKE) -C $(UTILS_DIR) \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install

utils-clean:
	@$(MAKE) -C $(UTILS_DIR) clean
	
