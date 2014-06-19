include ../../../.linux_config

CFLAGS += -g -I../../../includes
#LDFLAGS += --static
#LIBS = ../../../voip_manager/libvoip_manager.a
LDFLAGS += -L../../../voip_manager -lvoip_manager

ifdef CONFIG_RTK_VOIP_IP_PHONE
CFLAGS += -I../../../ui/include
endif

ifdef CONFIG_RTK_VOIP_DRIVERS_ATA_DECT
LDFLAGS += -lpthread -ldl
endif

CFLAGS += $(ECOS_SHELL_CFLAGS)

CFLAGS += -Werror

#----------------------------------------------------------------------
all:
	$(CC) ring.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o ring
	$(CC) phone.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o phone
	$(CC) rtp.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o rtp
	$(CC) rtp2.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o rtp2
	$(CC) daa.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o daa
	$(CC) caller_id.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o caller_id
	$(CC) fxo.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o fxo
	$(CC) fxo_int.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o fxo_int
	$(CC) conference.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o conference
	$(CC) custom_tone.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o custom_tone	
	$(CC) fxs.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o fxs
	$(CC) fxs_int.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o fxs_int
	$(CC) pulse_dial.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o pulse_dial
	$(CC) pcm_dump.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o pcm_dump
	$(CC) gain_test.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o gain_test
	$(CC) sip_reg_led.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o sip_reg_led
	$(CC) fxo_callerid_det.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o fxo_callerid_det
	$(CC) dis_tone.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o dis_tone
	$(CC) dtmfdet_dump.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o dtmfdet_dump
	$(CC) send_2833.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o send_2833
	$(CC) proslic_set.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o proslic_set
	$(CC) fskgen.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o fskgen
	$(CC) vmwigen.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o vmwigen
	$(CC) led_ctrl.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o led_ctrl

ecos-shell:
	$(CC) rtp.c $(CFLAGS) -c -o rtp.o 

romfs:
	$(ROMFSINST) fxs /bin/fxs
	$(ROMFSINST) fxs_int /bin/fxs_int
	$(ROMFSINST) fxo /bin/fxo
	$(ROMFSINST) fxo_int /bin/fxo_int
	$(ROMFSINST) rtp /bin/rtp
	$(ROMFSINST) rtp2 /bin/rtp2
	$(ROMFSINST) pcm_dump /bin/pcm_dump
	$(ROMFSINST) caller_id /bin/caller_id
	$(ROMFSINST) dtmfdet_dump /bin/dtmfdet_dump
	$(ROMFSINST) send_2833 /bin/send_2833
	$(ROMFSINST) proslic_set /bin/proslic_set
	$(ROMFSINST) fskgen /bin/fskgen
	$(ROMFSINST) vmwigen /bin/vmwigen
	$(ROMFSINST) led_ctrl /bin/led_ctrl

clean:
	rm -f *.o ring phone rtp rtp2 daa caller_id vmwi fxo fxo_int conference fxs fxs_int custom_tone pulse_dial pcm_dump gain_test sip_reg_led fxo_callerid_det dis_tone dtmfdet_dump send_2833 proslic_set fskgen vmwigen led_ctrl
