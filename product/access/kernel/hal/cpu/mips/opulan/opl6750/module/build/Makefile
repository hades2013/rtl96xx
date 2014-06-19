###############################################################################
# Copyright (C) 2007 OPL, All Rights Reserved
#
# FILENAME: Driver/Src Makefile
#
# DESCRIPTION:
#
# Date Created (optional): 1/8/2007
#
# Authors (optional):
#
# Reviewed by (optional):
#
# Edit History:
#
# $Id: //depot/sw/abu/opconn/release/v3.2.1/os/modules/build/Makefile#1 $
# $Log: Makefile,v $
# Revision 1.2  2007/08/23 06:22:08  hxu
# *** empty log message ***
#
# Revision 1.1.1.1  2007/08/01 02:15:27  hxu
# add linux module for opulan project:including linux module,driver api and test app
#
# Revision 1.7  2007/06/20 07:35:03  schen
# Recover this file
#
# Revision 1.5  2007/05/22 05:05:17  rzhou
# Add make option ?
#
# Revision 1.4  2007/05/22 04:56:30  rzhou
# Add make option of drv and lib
#
# Revision 1.3  2007/05/21 09:27:05  rzhou
# Del bsp make part
#
# Revision 1.2  2007/05/21 06:46:11  xwang
# *** empty log message ***
#
# Revision 1.1.1.1  2007/05/18 07:47:45  schen
# IPMux-E temp cleanup
#
# Revision 1.2  2007/04/13 05:25:13  pho
# remove tst
#
# Revision 1.1  2007/04/10 00:57:47  pho
# add ipmux_e
#
# Revision 1.1.1.1  2007/04/09 06:07:12  pho
# Import OPsystem ipmux_e sources
##############################################################################
DRV_BASE_DIR = $(shell cd ../;/bin/pwd)
MODULE_DIR = $(shell cd ../;/bin/pwd)
COPY_TO_DIR_MODULES = /home/xwang/smb/share/plat/openwrt/kamikaze_7.09/opulan_files/modules
COPY_TO_DIR_APPS = /home/xwang/smb/share/plat/openwrt/kamikaze_7.09/opulan_files/apps

export DRV_BASE_DIR 
export MODULE_DIR



all:
	@echo "==== Compiling ipmux-e linux minte intr module and register mmap module for MIPS_4KC ====="
	cd ../opl_minte;make
	cd ../opl_reg;make
	cd ../opl_cpuif;make	
	cd ../opl_wdt; make
	cd ../opl_uptm_intr; make
	cd ../opl_dal_driver; make	
	cd ../opl_driver_api; make	
	@echo "===== Make linux moddule for minte and register mmap competed!!!====="
?:
	@echo "==== How to make ipmux-e linux driver module for MIPS_4KC ====="
	@echo "   >Make all  ;make Minte module, Register Memmap Module;"
	@echo "   >Make minte  ;make Minte module;"
	@echo "   >Make reg  ;make Register mmap module."
	@echo "	  >make cpuif ;make cpuif(dma) module"
	@echo "   >make misc; make misc module"
	@echo "   >make wdt; make wdt module"
	@echo "   >make opldaldriver; make opl dal module"	
i2c:
	@echo "==== Compiling ipmux-e linux i2c module for MIPS_4KC ====="
	cd ../i2c;make
	@echo "===== Make i2c module competed!!!====="
uptm:
	@echo "==== Compiling ipmux-e linux uptm module for MIPS_4KC ====="
	cd ../opl_uptm_intr;make
	@echo "===== Make uptm module competed!!!====="
minte:
	@echo "==== Compiling ipmux-e linux minte module for MIPS_4KC ====="
	cd ../opl_minte;make
	@echo "===== Make minte module competed!!!====="
reg:	
	@echo "==== Compiling ipmux-e linux module-reg mmap module for MIPS_4KC ====="
	cd ../opl_reg;make
	@echo "===== Make register mmap module  competed!!!====="

tmc:	
	@echo "==== Compiling ipmux-e linux timer0 module for MIPS_4KC ====="
	cd ../opl_tmc;make
	@echo "===== Make timer0 module  competed!!!====="
wdt:
	@echo "==== Compiling ipmux-e linux watchdog module for MIPS_4KC ====="
	cd ../opl_wdt;make
	@echo "===== Make watchdog module  competed!!!====="

cpuif:	
	@echo "==== Compiling ipmux-e linux cpuif(host dma0) module for MIPS_4KC ====="
	cd ../opl_cpuif;make
	@echo "===== Make cupif(host dma0) module  competed!!!====="
funcaddr:
	cd ../func_at_fix_addr;make
	cp ../func_at_fix_addr/*.ko /ucnfs/tmp/
onuint:
	cd ../onu_int;make
	cp ../onu_int/onu_int.ko /ucnfs/tmp/
sample:
	cd ../opl_module_sample;make
	cp ../opl_module_sample/opl_module_sample.ko /ucnfs/tmp/
test:
	cd ../opl_reg/test;make;make cp
	cd ../opl_minte/test;make;make cp
	cd ../opl_cpuif/test;make;make cp
tmm_drv:
	cd ../tmm_drv;make
	cp ../tmm_drv/tm.ko /ucnfs/home/ryang
		
opldaldriver:
	@echo "==== Compiling ipmux-e linux opl dal module for MIPS_4KC ====="
	cd ../opl_dal_driver;make
	@echo "===== Make opl dal module competed!!!====="

cp: 
	cp ../obj/linux-2.6/*.ko $(COPY_TO_DIR_MODULES)

clean:
	rm -rf ../obj/linux-2.4/*
	rm -rf ../obj/linux-2.6/*
	cd ../opl_minte;make clean
	cd ../opl_reg;make clean
	cd ../opl_cpuif;make clean
	cd ../opl_tmc;make clean
	cd ../opl_wdt;make clean
	cd ../i2c;make clean
	cd ../opl_uptm_intr;make clean
	cd ../opl_dal_driver;make clean
	@echo "===== Make clean Linux modules for minte and register mmap for MIPS_4KC competed!!!====="

