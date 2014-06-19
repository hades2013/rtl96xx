# Rules for making webs
WEBS_DIR = webs-2-5

#ifdef CONFIG_HTTPS
HTTPS_BUILD = https-build
HTTPS_DIR = $(shell pwd)/$(WEBS_DIR)/matrixssl-3-1-3-open


#else
#HTTPS_BUILD = 
#endif

https-build:
	@$(MAKE) -C $(HTTPS_DIR) \
		all
	
webs-build: $(HTTPS_BUILD)
	@$(MAKE) -C $(WEBS_DIR)/LINUX \
		SSL_DIR=$(HTTPS_DIR) \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		all
		
webs-install:$(HTTPS_BUILD)
	@$(MAKE) -C $(WEBS_DIR)/LINUX \
		CONFIG_PREFIX="$(TARGETDIR)" \
		install
#	@cp $(WEBS_DIR)/LINUX/httpd /tftpboot

webs-clean:
	@$(MAKE) -C $(WEBS_DIR)/LINUX clean
	
