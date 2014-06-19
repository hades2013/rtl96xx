SHELL=/bin/bash

APP_DIR=$(shell pwd)
APP_HEAD_DIR=$(APP_DIR)/../../config

.PHONY: all
all: 
	#ln -f -s ${APP_HEAD_DIR}/app_head.h $(APP_DIR)/app_head.h
	gcc -o mkfw mkfw.c
	$(APP_DIR)/mkfw ${app} -k zImage -f squashfs_lzma.bin  -s $(kernel_size)
######################################################################
# Clean out everything
######################################################################
.PHONY: clean
clean: 
	rm -f ./mkfw
	rm -f ./app_head.h
	

