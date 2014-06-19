obj-y = script/ coprocessor_dsp_tx.o coprocessor_dsp_rx.o 

EXTRA_CFLAGS += -I$(DIR_VOIP)/include
EXTRA_CFLAGS += -I$(DIR_VOIP)/voip_ipc/
EXTRA_CFLAGS += -I$(DIR_VOIP)/voip_ipc/coprocessor_dsp/include/
EXTRA_CFLAGS += -D_AIPC_DSP_

ifeq ($(CONFIG_RTK_VOIP),m)
EXTRA_CFLAGS += -mlong-calls
EXTRA_CFLAGS += -DMODULE
endif

EXTRA_CFLAGS += -DMODULE_NAME=\"IPC\"

include $(DIR_VOIP)/rules.mak

