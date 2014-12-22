TKT_PATH_IN := ../toolkit_path.in
ifneq ($(wildcard $(TKT_PATH_IN)), $(TKT_PATH_IN))
    $(error EE: Missing "./toolkit_path.in". Refer "./toolkit_path.in.sample" to create your own "./toolkit_path.in")
else
    include $(TKT_PATH_IN)
endif

CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld

QUIET ?= 1
ifeq ($(QUIET), 1)
	QCC = @echo [CC] $@ && $(CC)
	QLD = @echo [LD] $@ && $(LD)
else
	QCC := $(CC)
	QLD := $(LD)
endif

PLAT_INFO := ../preloader/platform/current
ifeq ($(NO_PLL), 1)
	CWEIGHT := 600
else ifneq ($(wildcard $(PLAT_INFO)), $(PLAT_INFO))
	CWEIGHT := 601
	NO_PLL := 1
else
	PLAT := $(shell ls -l $(PLAT_INFO) | cut -d ' ' -f10 | cut -d '_' -f1)
	ifeq ("$(PLAT)", "luna")
		CWEIGHT := 666
	else ifeq ("$(PLAT)", "8380")
		CWEIGHT := 680
	else ifeq ("$(PLAT)", "8390")
		CWEIGHT := 690
	else
		CWEIGHT := 602
		NO_PLL := 1
	endif
endif

NO_PLL ?= 0
ifeq ($(NO_PLL), 0)
	PLL_FILE_LIST := plr_pll_genX.o plr_pll_genX_comp.o
else
	PLL_FILE_LIST := dummy_pll.o
endif

CINCF   := -I./include -I../preloader/include -I../release -I../preloader/soc/cpu/mips-generic
CDEBUGF := -g -Wall -Wextra
CDEFF   := -DCWEIGHT=$(CWEIGHT) -DDISABLE_FLASH_SPARE=1
CARCHF  := -mno-abicalls -mno-memcpy -EB
CFLAGS  := $(CDEFF) $(CINCF) -nostdinc -fno-pic -static -fno-builtin $(CARCHF) -Os $(CDEBUGF)

LFLAGS := -static -T caffeine600mg.lds -oformat=elf32-tradbigmips \
          -e cstart -Ttext=0xbf000000

all: caffeine600mg.out

caffeine600mg.out: cinit.o plr_tutils_printf.o \
                   plr_uart.o plr_uart_ext.o \
                   plr_tutils.o \
                   plr_nor_spi_flash.o \
                   $(PLL_FILE_LIST) \
                   xmodem.o crc16.o \
                   caffeine600mg.o
	$(QLD) $(LFLAGS) -o $@ $^
	@echo TOOLKIT: $(CROSS_COMPILE)

plr_nor_spi_flash.c: ../preloader/soc/flash/plr_nor_spi_flash.c
	@echo "[LN] $@ <- $^"
	@ln -s $^

plr_nor_spi_flash.o: plr_nor_spi_flash.c
	$(QCC) $(CFLAGS) -Wno-unused-parameter -Wno-unused-function -c $^ -o $@

plr_tutils.c: ../preloader/plr_tutils.c
	@echo "[LN] $@ <- $^"
	@ln -s $^

plr_tutils_printf.c: ../preloader/plr_tutils_printf.c
	@echo "[LN] $@ <- $^"
	@ln -s $^

plr_uart.c: ../preloader/soc/common/plr_uart.c
	@echo "[LN] $@ <- $^"
	@ln -s $^

plr_pll_genX.c: ../preloader/platform/current/plr_pll_gen?.c
	@echo "[LN] $@ <- $^"
	@ln -s $^ $@

plr_pll_genX_comp.c: ../preloader/platform/current/plr_pll_gen?_comp.c
	@echo "[LN] $@ <- $^"
	@ln -s $^ $@

.c.o:
	$(QCC) $(CFLAGS) -c $^ -o $@

.S.o:
	$(QCC) $(CFLAGS) -c $^ -o $@

clean:
	@rm -f plr_nor_spi_flash.c plr_uart.c plr_tutils_printf.c plr_tutils.c
	@rm -f plr_pll_genX.c plr_pll_genX_comp.c
	@rm -f *.o *.out
