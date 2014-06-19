# Makefile for www

TOPDIR:=../
HTML_SOURCE = web_html
HTML_ASP = web_asp
PDT_MAIN_CONFIG_DIR = $(TOPDIR)../product/access
PRODUCT_DIR = $(shell pwd)/$(ENV_PRO)

ifeq ($(PDT_MAIN_CONFIG_DIR)/.config,$(wildcard $(PDT_MAIN_CONFIG_DIR)/.config))
include $(PDT_MAIN_CONFIG_DIR)/.config
endif

ifndef FTPROOT
WWW_ROOT:=$(TOPDIR)../product/access/rootfs/www
endif


#all: www.tar
#www.tar: $(TOPDIR)/shared/vendor.h
all:
	@echo "Preparing Web Pages..."
	mkdir -p $(PRODUCT_DIR)/www;cd $(PRODUCT_DIR)/www;mkdir -p web_asp;cd ../
	@cp -rf $(PRODUCT_DIR)/$(HTML_ASP)/* $(PRODUCT_DIR)/www/web_asp; cd $(PRODUCT_DIR)/www/web_asp;find . -name .svn|xargs rm -fr;cd ../../
	cd $(PRODUCT_DIR)/www/web_asp;rm -rf $(CONFIG_PRODUCT_NAME).config;echo > $(CONFIG_PRODUCT_NAME).config;chmod 777 $(CONFIG_PRODUCT_NAME).config;ln -fs /tmp/$(CONFIG_PRODUCT_NAME).config $(CONFIG_PRODUCT_NAME).config;ln -fs /var/log/messages  $(CONFIG_PRODUCT_NAME)-syslog.bin;cd ../../
#	@cp -r $(HTML_ASP)/* $(TOPDIR)../product/access/rootfs/www/
#	@cp -r web_um/* $(TOPDIR)../product/access/rootfs/www/
#	@cp  ../webs-2-5/LINUX/umconfig.txt $(TOPDIR)../product/access/rootfs/etc/
#	@cd www; find . -name .svn|xargs rm -fr; tar cf ../www.tar *
#	@rm -fr www
#	@rm -fr $(FTPROOT)/www.tar
#	@cp www.tar $(FTPROOT)
	 
#converter
#	@./convert.sh
#	@cp -r $(HTML_SOURCE)/images $(HTML_ASP)/
	
converter:
	@gcc -o html2asp html2asp.c

install: all
	@cp -rf $(PRODUCT_DIR)/www/web_asp $(WWW_ROOT)
	@rm -rf $(PRODUCT_DIR)/www

clean:
#	@rm -rf www
#	@rm -rf www.tar


	
