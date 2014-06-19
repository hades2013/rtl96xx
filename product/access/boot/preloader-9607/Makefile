#
# prepare information before everything
#

ifndef CROSS_COMPILE
ifneq ($(wildcard ./toolkit_path.in), ./toolkit_path.in)
    $(error EE: Missing "./toolkit_path.in". Refer "./toolkit_path.in.sample" to create your own "./toolkit_path.in")
else
    include ./toolkit_path.in
endif
endif

BOOTLOADER_IMG=$(shell pwd)/u-boot-9607/u-boot.pimg

export BOOTLOADER_IMG

# global veriable
OTTO_ROOT := $(shell pwd)
UTIL_DIR := $(OTTO_ROOT)/util
PARTS_DB_DIR := $(UTIL_DIR)/parts_db
RELEASE_DIR := $(OTTO_ROOT)/release
PRELOADER_DIR := $(OTTO_ROOT)/preloader
CURRENT_PLATFORM := $(PRELOADER_DIR)/platform/current
LOADER_IMG := $(RELEASE_DIR)/loader.img
FINAL_IMG := $(RELEASE_DIR)/final.img
LPRELOADER := $(RELEASE_DIR)/lplr.img
PRELOADER := $(RELEASE_DIR)/plr.img

ifeq ($(wildcard ./Makefile.in), ./Makefile.in)
    include ./Makefile.in
endif

compose: $(BEFORE_COMPOSE)
	@cd release && $(UTIL_DIR)/otto_composer soc.tcl -no-final\
	    -e "set PARTS_DB_DIR [list $(PARTS_DB_DIR)]" \
	    -e "set loader_image [list $(LOADER_IMG)]" \
	    -e "set lpreloader_image [list $(LPRELOADER)]" \
	    -e "set preloader_image [list $(PRELOADER)]" \
	    -e "set bootloader_image [list $(BOOTLOADER_IMG)]" \
	    -e "set kernel_image [list $(KERNEL_IMG)]" \
	    -e "set rootfs_image [list $(ROOTFS_IMG)]" \
	    -e "source $(PARTS_DB_DIR)/otto_lib.tcl" \
	    && dd if=$(LOADER_IMG) of=$(LOADER_IMG).4k0 bs=4096 count=1

compose_all: $(BEFORE_COMPOSE)
	@cd release && $(UTIL_DIR)/otto_composer soc.tcl \
	    -e "set PARTS_DB_DIR [list $(PARTS_DB_DIR)]" \
	    -e "set final_image [list $(FINAL_IMG)]" \
	    -e "set loader_image [list $(LOADER_IMG)]" \
	    -e "set lpreloader_image [list $(LPRELOADER)]" \
	    -e "set preloader_image [list $(PRELOADER)]" \
	    -e "set bootloader_image [list $(BOOTLOADER_IMG)]" \
	    -e "set kernel_image [list $(KERNEL_IMG)]" \
	    -e "set rootfs_image [list $(ROOTFS_IMG)]" \
	    -e "source $(PARTS_DB_DIR)/otto_lib.tcl" \
	    && dd if=$(FINAL_IMG) of=$(FINAL_IMG).4k0 bs=4096 count=1

dumpcfg: $(BEFORE_COMPOSE)
	@[ -f $(LOADER_IMG) ] && \
	  $(UTIL_DIR)/otto_composer -d $(LOADER_IMG) || \
	  echo EE: $(LOADER_IMG) does not exist.

LIST_MODEL_CMD:=grep -v GENERIC | cut -d " " -f 3 | sort | sed -e "s|^|  |"
list_models:
	@echo "Supported NAND flashes:"
	@grep "^namespace" $(PARTS_DB_DIR)/parts_for_nand.tcl    | $(LIST_MODEL_CMD)
	@echo "Supported NOR SPI flashes:"
	@grep "^namespace" $(PARTS_DB_DIR)/parts_for_nor_spi.tcl | $(LIST_MODEL_CMD)
	@echo "Supported DRAMs:"
	@grep "^namespace" $(PARTS_DB_DIR)/parts_for_dram*.tcl   | $(LIST_MODEL_CMD)

.EXPORT_ALL_VARIABLES:
.PHONY: compose compose_all list_models dumpcfg
