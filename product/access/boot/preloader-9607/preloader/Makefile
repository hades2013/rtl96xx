
ifneq ($(wildcard ../toolkit_path.in), ../toolkit_path.in)
  $(error EE: Missing "../toolkit_path.in". Refer "../toolkit_path.in.sample" to create your own "../toolkit_path.in")
else
  include ../toolkit_path.in
endif

-include ./platform/current/buildflags.in

ARCHCFG:=./platform/current/arch.h
ifneq ($(wildcard ./$(ARCHCFG)), ./$(ARCHCFG))
    $(error Missing $(ARCHCFG)! Run 'make preconfig_XXX' first!)
endif

#import parameters defined in arch.h
dummy:=$(shell \[ -f $(RELEASE_DIR)/soc.h \] && cpp -DTBASE=$(TBASE) -I$(RELEASE_DIR) $(UTIL_DIR)/etc/arch_care.inc | grep "\#\#arch_care" | tee arch_care.in)
include arch_care.in


TEXT_BASE               := $(aTEXT_BASE)
DECOMP_ALGO_GZIP        := $(aDECOMP_ALGO_GZIP)
DECOMP_ALGO_LZMA        := $(aDECOMP_ALGO_LZMA)
OTTO_NAND_FLASH         := $(aOTTO_NAND_FLASH)
OTTO_NOR_SPI_FLASH      := $(aOTTO_NOR_SPI_FLASH)
CACHELINE_SIZE          := $(aCACHELINE_SIZE)
ON_FLASH_VMA_GAP        := $(aON_FLASH_VMA_GAP)
LPLR_BSIZE_CONSTRAINT   := $(aLPLR_BSIZE_CONSTRAINT)
SOC_CONF_OFFSET         := $(aSOC_CONF_OFFSET)

ifeq ($(DECOMP_ALGO_GZIP), 1)
  DECOMP_ALGO:=\"gzip\"
  COMPOBJ:=tzlib.o
else ifeq ($(DECOMP_ALGO_LZMA), 1)
  DECOMP_ALGO:=\"lzma\"
  COMPOBJ:=tlzma.o LzmaDec.o
endif

# grouping object files
LPLR_C_SRC:=$(shell ls lplr_*.c ./platform/current/lplr_*.c ./platform/current/cpu/lplr_*.c 2> /dev/null)
LPLR_S_SRC:=$(shell ls lplr_*.S ./platform/current/lplr_*.S ./platform/current/cpu/lplr_*.S 2> /dev/null)
LPLR_OBJ:=$(LPLR_C_SRC:.c=.o) $(LPLR_S_SRC:.S=.o)

SWP_OBJ:=$(shell ls ./platform/current/swp_*.c 2> /dev/null)
SWP_OBJ:=$(SWP_OBJ:.c=.o)
VENDORSWP_OBJ:=$(shell ls ./vendor_swp/swp_*.c 2> /dev/null)
VENDORSWP_OBJ:=$(VENDORSWP_OBJ:.c=.o)
PLR_C_SRC:=$(shell ls plr_*.c ./platform/current/plr_*.c ./platform/current/dram/plr_*.c 2> /dev/null)
PLR_S_SRC:=$(shell ls plr_*.S ./platform/current/plr_*.S ./platform/current/dram/plr_*.S 2> /dev/null)
PLR_OBJ:=$(PLR_C_SRC:.c=.o) $(PLR_S_SRC:.S=.o) $(COMPOBJ) $(SWP_OBJ) $(VENDORSWP_OBJ)


ifeq ($(OTTO_NOR_SPI_FLASH), 1)
    PRELOADER_IMG  :=plr.img
    PRELOADER_BASE :=$(TEXT_BASE)
    PLR_OBJ:=$(LPLR_OBJ) $(PLR_OBJ)
    PLR_LDS := plr.nor.lds
else ifeq ($(OTTO_NAND_FLASH), 1)
    LPRELOADER_IMG :=lplr.img
    LPRELOADER_BASE:=$(TEXT_BASE)
    PRELOADER_IMG  :=plr.img
    PRELOADER_BASE :=$(TEXT_BASE)
    PLR_SRAM_DEF := --defsym=DMA_SIZE=$(LPLR_BSIZE_CONSTRAINT)
    PLR_LDS := plr.nand.lds
else
    $(error neither OTTO_NOR_SPI_FLASH nor OTTO_NAND_FLASH is selected)
endif

# compilation flags
CINCF=-I./include -I./platform/current/cpu -I./platform/current -I../release
CDEBUGF=-g -Wall -Wno-unused-parameter
CDEFF=-DSELFTEST=0 -DDECOMP_ALGO=$(DECOMP_ALGO) -DDATE_TAG="\"$(DATE_TAG)\"" -DTBASE=$(TBASE) -DREVISION="\"$(revision)\""
CFLAGS=$(CDEFF) $(CINCF) -nostdinc -fno-pic -fno-builtin $(CARCHF) $(CFLAGS_OPT) $(CDEBUGF)
LDEFF=--defsym=CACHELINE_SIZE=$(CACHELINE_SIZE) --defsym=SOC_CONF_OFFSET=$(SOC_CONF_OFFSET) --defsym=PLR_BASE=$(PRELOADER_BASE) --defsym=on_flash_vma_gap=$(ON_FLASH_VMA_GAP) --defsym=LMA_base=0 $(PLR_SRAM_DEF)
LFLAGS=-static $(LDEFF) 

# convenient macro
TARGET_BSIZE = `du --apparent-size -b $@ | cut -f1`

# look and feel parameters
ifeq ($(QUIET), 0)
COMPILE_PROMPT =
LINK_PROMPT    =
BUILD_PROMPT   =
OBJCPY_PROMPT  =
OBJDMP_PROMPT  =
else
COMPILE_PROMPT = @echo "[CC] $@" &&
LINK_PROMPT    = @echo "[LD] $@" &&
BUILD_PROMPT   = @echo "[BUILD] $@" &&
OBJCPY_PROMPT  = @echo "[OBJCPY] $@" &&
OBJDMP_PROMPT  = @echo "[OBJDMP] $@" &&
endif

all: ./include/zlib/zlib.h $(LPRELOADER_IMG) $(PRELOADER_IMG)
	@cp ./*.img ../release

# preloader
plr.img: plr.out
	$(OBJCPY_PROMPT) $(OC) --gap-fill=0xff -O binary $^ $@
	@echo "\n*'$@':" `du --apparent-size -h $@ | cut -f1` "(`du --apparent-size -b $@ | cut -f1` Bytes)"
	@echo "*in SRAM size:" `$(NM) --radix=d plr.out | grep LS_preloader_in_SRAM_sz | cut -d ' ' -f1` "Bytes (bss/data/ro included, but stack excluded)"

plr.out: $(PLR_OBJ)
	$(LINK_PROMPT) $(LD) $(LFLAGS) -T $(PLR_LDS) -e c_start_epilogue -o $@ $^
	$(OBJDMP_PROMPT) $(OD) -Dlxt $@ > $(@:.out=.code)

# lpreloader
lplr.img: lplr.out
	$(OBJCPY_PROMPT) $(OC) --gap-fill=0xff -O binary $^ $@
	@export T_BSIZE=$(TARGET_BSIZE) ;\
	 if [ "$${T_BSIZE}" -gt "$(LPLR_BSIZE_CONSTRAINT)" ] ;\
	 then \
	   echo "EE: size of $@, $${T_BSIZE} bytes, is larger than $(LPLR_BSIZE_CONSTRAINT) bytes." ;\
	   exit 1 ;\
	 else \
	   echo "\n*'$@':" `du --apparent-size -h $@ | cut -f1` "(`du --apparent-size -b $@ | cut -f1` Bytes)" ;\
	   echo *Text base: $(PRELOADER_BASE) ;\
	 fi

lplr.out: $(LPLR_OBJ)
	@echo "to link $(LPLR_OBJ);"
	$(LINK_PROMPT) $(LD) $(LFLAGS) -T lplr.lds -e pre_flash_info -Ttext=$(LPRELOADER_BASE) -o $@ $^
	$(OBJDMP_PROMPT) $(OD) -Dlxt $@ > $(@:.out=.code)

lplr_entry.o: lplr_entry.S ./platform/current/cpu/lplr_cpu.S ./platform/current/cpu/lplr_sram.S
	$(COMPILE_PROMPT) $(CC) $(CFLAGS) -c $<

#include/soc.h: $(SOC_IN_H) $(ARCH_H)
#	@echo -n "[MISC] Creating soc.h... "
#	@sed -e '/^\/\/OTTO_ARCH_H_ANCHOR/r ./platform/current/arch.h' -e '/OTTO_ARCH_H_ANCHOR/d' -e '/CAUTION: Do/d' $< > $@
#	@cp $@ ../release
#	@echo "OK"

# generate a fake uboot image for test
fake-u-boot.bin: fake_uboot.o plr_tutils_printf.o ./platform/current/plr_uart.o
	@echo -n "[MISC] Generating a FAKE U-Boot image for test... "
	@$(LD) $(LFLAGS) -e main -Ttext=0x80700000 -o fake_uboot.out $^
	@$(OC) --gap-fill=0xff -O binary fake_uboot.out fake_uboot.img
	@echo "$@ OK"

fake_uboot.o:
	@echo "#include <preloader.h>\nint main(void) { printf(\"I am U-Boot. I am running.\"); while(1); return 0;}" > fake_uboot.c
	@$(CC) $(CFLAGS) -c fake_uboot.c -o $@

# fundamental targets
ifeq ($(shell $(CC) -dumpversion | cut -d '.' -f1), 4)
SUPPRESS_STRUCT_INIT_WARN := -Wno-missing-field-initializers -Wno-override-init
endif
lplr_header_info.o: lplr_header_info.c
	$(COMPILE_PROMPT) $(CC) $(CFLAGS) $(SUPPRESS_STRUCT_INIT_WARN) -c $^ -o $@

swp%.o: swp%.c
  # 120724,JJ,the `grep' stuff extracts swp-defined compilation flags for compilation.
	$(COMPILE_PROMPT) $(CC) $(CFLAGS) `grep '^//OTTO_SWP_CFLAGS=' $^ | cut -d "=" -f2-` -c $^ -o $@

.S.o:
	$(COMPILE_PROMPT) $(CC) $(CFLAGS) -c $^ -o $@

.c.o:
	$(COMPILE_PROMPT) $(CC) $(CFLAGS) -c $^ -o $@

#arch_care.in: release/soc.h $(UTIL_DIR)/etc/arch_care.inc
#	@echo "" > arch_care.in
#	@\[ -f release/soc.h \] && cpp -DTBASE=$(TBASE) -I$(RELEASE_DIR) $(UTIL_DIR)/etc/arch_care.inc | grep "\#\#arch_care" > arch_care.in
#

#preconfig_clean: clean
#	@rm -rf ./platform/current
#	@rm -f ../release/soc.tcl ../release/pblr.h ../release/soc.h include/soc.h
#	@rm -f $(PLATFORM_NAME)
#
#$(PRE_CFG_LIST): preconfig_clean
#	@echo -n "[MISC] Preparing $(@:preconfig_%=%) environment... "
#	@cd platform && ln -s $(@:preconfig_%=%) current
#	@mkdir -p ../release
#	@cp ./platform/current/soc.tcl ../release
#	@cp ./include/pblr.h ../release
#	@echo "platform_name := $(@:preconfig_%=%)" > $(PLATFORM_NAME)
#	@echo "OK"
#	@$(MAKE) --no-print-directory include/soc.h
#clean:
#	@rm -f $(LPLR_OBJ) $(PLR_OBJ)
#	@rm -f *.o *.out *.img fake_uboot.*
#	@rm -rf images
#	@rm -f ../release/plr.img ../release/lplr.img
#distclean: preconfig_clean
## Left zlib.h alone to prevent re-built zlib every time. Just for a reference.
##	@echo "*** WARNING: RUNNING distclean CAUSES zlib-1.2.7 REBUILD AND MAY GENERATE BAD zlib.h. ***"
##	@echo "*** WARNING: MODIFY distclean IN Makefile IF YOU STILL WANT TO distclean. ***"
##	@rm -f ./include/zlib/zlib.h
##	@cd ./lib/zlib-1.2.7 && make distclean


# This target is left as a reference of how zlib built.
./include/zlib/zlib.h:
	$(BUILD_PROMPT)          \
		cd ./lib/zlib-1.2.7 && \
		CC=$(CROSS_COMPILE)gcc CFLAGS="-fno-builtin -march=4kec -mtune=4kec -Os" ./configure --static --solo > /dev/null && \
		make > /dev/null
	@echo "[CPY]" zlib headers && \
		cp ./lib/zlib-1.2.7/zlib.h ./include/zlib/zlib.h && \
		cp ./lib/zlib-1.2.7/zutil.h ./include/zlib/zutil.h && \
		cp ./lib/zlib-1.2.7/inftrees.h ./include/zlib/inftrees.h && \
		cp ./lib/zlib-1.2.7/inflate.h ./include/zlib/inflate.h && \
		cp ./lib/zlib-1.2.7/inffixed.h ./include/zlib/inffixed.h && \
		cp ./lib/zlib-1.2.7/zconf.h ./include/zlib/zconf.h

