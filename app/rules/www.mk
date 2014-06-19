# Rules for making www

WWW_DIR = www

www-build:
	$(MAKE) -C $(WWW_DIR)
	
www-install:
	@$(MAKE) -C $(WWW_DIR) \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install
www-clean:
	$(MAKE) -C $(WWW_DIR) clean
