-include ../../config/.config 

EXEC = aipc_util
OBJS = aipc_util.o

CFLAGS += -Wall

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ 

%.o: %.c
	ln -sf $(KERNEL_DIR)/drivers/char/aipc/include/aipc_ioctl.h aipc_ioctl.h
	$(CC) -c -o $@ $(CFLAGS) -Wa,-a,-ad $< > $(@:.o=.lst)
	$(CC) -M $(CFLAGS) $< | sed -e "s#^$(*F).o[ :]*#$(@D)\/$(*F).o : #" > $(@:.o=.d)

romfs:
	$(ROMFSINST) $(EXEC)   /bin/$(EXEC)
	$(ROMFSINST) aipc/rc_boot_dsp				/etc/rc_boot_dsp
ifeq ($(CONFIG_USER_SLIC_ON_DSP_SILAB_SI3217X),y)
	$(ROMFSINST) aipc/appimg.bin-si3217x			/image/dsp.img
else ifeq ($(CONFIG_USER_SLIC_ON_DSP_SILAB_SI3226X),y)
	$(ROMFSINST) aipc/appimg.bin-si3226x			/image/dsp.img
else ifeq ($(CONFIG_USER_SLIC_ON_DSP_ZARLINK_LE89116),y)
	$(ROMFSINST) aipc/appimg.bin-le89116			/image/dsp.img
else ifeq ($(CONFIG_USER_SLIC_ON_HOST),y)
	#$(ROMFSINST) aipc/appimg.bin-noslic				/image/dsp.img
	$(ROMFSINST) aipc/appimg.bin				/image/dsp.img
else
	$(ROMFSINST) aipc/appimg.bin				/image/dsp.img
endif
	$(ROMFSINST) aipc/netcat-mips 				/bin/netcat

clean:
	rm -f $(EXEC) *.o *.d *.lst

.PHONY: all clean

