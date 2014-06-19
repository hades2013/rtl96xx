# arch/mips/boot/compressed/iDiSxX/Makefile

boot           := arch/rlx/boot
compressed     := $(boot)/compressed
utils          := $(compressed)/utils
lib            := $(compressed)/lib
images         := $(compressed)/images
common         := $(compressed)/common

#    CC = mips64-octeon-linux-gnu-gcc
#    AR = mips64-octeon-linux-gnu-ar
#    LD = mips64-octeon-linux-gnu-ld
#    STRIP = mips64-octeon-linux-gnu-strip
#    OBJDUMP = mips64-octeon-linux-gnu-objdump
#    NM = mips64-octeon-linux-gnu-nm
#   OBJCOPY = mips64-octeon-linux-gnu-objcopy

#########################################################################
# START BOARD SPECIFIC VARIABLES



# These two variables control where the zImage is stored
# in flash and loaded in memory.  It only controls how the srec
# file is generated, the code is the same.  0xffffffff80500000, tftp to 804f0000;
RAM_RUN_ADDR = 0x80050000

# These two variables specify the free ram region
# that can be used for temporary malloc area
AVAIL_RAM_START=0x81600000
AVAIL_RAM_END=0x82b00000
#AVAIL_RAM_START=${CONFIG_AVAIL_RAM_START}
#AVAIL_RAM_END=${CONFIG_AVAIL_RAM_END}

# This one must match the LOADADDR in arch/mips/Makefile!
#LOADADDR=0xffffffff81100000

# If the elf file is compressed, then LOADADDR = 0x83500000
LOADADDR=0x81000000
#LOADADDR=${CONFIG_LOADADDR}

# END BOARD SPECIFIC VARIABLES
#########################################################################
OBJECTS := $(obj)/head.o  $(obj)/misc-simple.o $(obj)/cache.o  
LIBS := $(common)/lib.a $(lib)/lib.a

ENTRY := $(utils)/entry
OFFSET := $(utils)/offset
SIZE := $(utils)/size

EXTRA_CFLAGS += -I$(shell pwd)/include -mabicalls
AFLAGS += -mabicalls

LD_ARGS := -T $(compressed)/ld.script -Ttext $(RAM_RUN_ADDR) -Bstatic 

ifdef CONFIG_CPU_LITTLE_ENDIAN
OBJCOPY_ARGS = -O elf32-tradlittlemips
else
OBJCOPY_ARGS = -O elf32-tradbigmips
endif

all : zImage


$(obj)/head.o: $(src)/head.c $(TOPDIR)/vmlinux
	$(CC) $(AFLAGS) \
	-DKERNEL_ENTRY=$(shell sh $(ENTRY) $(NM) $(TOPDIR)/vmlinux ) \
	-c -o $(obj)/head.o $<

#head.o: 
#	cp vmlinux.64 vmlinux
#	$(CC) $(AFLAGS) \
#	-DKERNEL_ENTRY=0xb0200000 \
#	-c -o head.o head.c

$(obj)/misc-simple.o: $(src)/misc-simple.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -DINITRD_OFFSET=0 -DINITRD_SIZE=0 -DZIMAGE_OFFSET=0 \
		-DAVAIL_RAM_START=$(AVAIL_RAM_START) \
		-DAVAIL_RAM_END=$(AVAIL_RAM_END) \
		-DLOADADDR=$(LOADADDR) \
		-DZIMAGE_SIZE=0 -c -o $@ $<

$(obj)/cache.o: $(src)/cache.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -DINITRD_OFFSET=0 -DINITRD_SIZE=0 -DZIMAGE_OFFSET=0 \
		-DAVAIL_RAM_START=$(AVAIL_RAM_START) \
		-DAVAIL_RAM_END=$(AVAIL_RAM_END) \
		-DLOADADDR=$(LOADADDR) \
		-DZIMAGE_SIZE=0 -D__KERNEL__ -c -o $@ $<

#misc-common.o: misc-common.c
#	$(CC) $(CFLAGS) -c -o $@ $<
#		
#no_initrd.o: no_initrd.c
#	$(CC) $(CFLAGS) -c -o $@ $<
#	
#dummy.o: dummy.c
#	$(CC) $(CFLAGS) -c -o $@ $<
#	
#idisx_uart.o: idisx_uart.c
#	$(CC) $(CFLAGS) -c -o $@ $<
	
$(obj)/zvmlinux: $(OBJECTS) $(LIBS) $(srctree)/$(compressed)/ld.script $(images)/vmlinux.gz $(common)/dummy.o
	$(OBJCOPY) \
		--add-section=.image=$(images)/vmlinux.gz \
		--set-section-flags=.image=contents,alloc,load,readonly,data \
		$(common)/dummy.o $(common)/image.o
	$(LD) $(LD_ARGS) -o $@ $(OBJECTS) $(common)/image.o $(LIBS)
	$(OBJCOPY) -O binary $@ $@.temp
	$(OBJCOPY) -O binary -R .note -R .comment -S  $@ $(obj)/zvmlinux.bin 

# Here we manipulate the image in order to get it the necessary
# srecord file we need.
zImage: $(obj)/zvmlinux
	cp $(obj)/zvmlinux.bin $(images)/zImage
#	$(OBJCOPY) -O elf64-tradbigmips $(OBJCOPYFLAGS) $(images)/zImage $(images)/zImage.64
#	$(OBJCOPY) -O srec $(images)/zImage.64 $(images)/zImage.srec
#	$(OBJCOPY) -O binary $(images)/zImage.64 $(images)/zImage.bin

clean:
	rm -fr head.o misc-simple.o cache.o

