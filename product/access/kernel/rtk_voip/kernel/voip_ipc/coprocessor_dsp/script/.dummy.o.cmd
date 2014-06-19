cmd_rtk_voip/voip_ipc/coprocessor_dsp/script/dummy.o := rsdk-linux-gcc -Wp,-MD,rtk_voip/voip_ipc/coprocessor_dsp/script/.dummy.o.d  -nostdinc -isystem /home/shipeng/rtl86907/aaa/trunk/toolchains/rsdk-1.5.6-5281-EB-2.6.30-0.9.30.3-110915/bin/../lib/gcc/mips-linux/4.4.6/include -Iinclude  -I/home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include -include include/linux/autoconf.h -Iinclude/soc -D__KERNEL__ -D"VMLINUX_LOAD_ADDRESS=0x80041000" -D"LOADADDR=0x80041000" -D"DATAOFFSET=0" -D__ASSEMBLY__ -ffunction-sections  -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -EB -UMIPSEB -U_MIPSEB -U__MIPSEB -U__MIPSEB__ -UMIPSEL -U_MIPSEL -U__MIPSEL -U__MIPSEL__ -DMIPSEB -D_MIPSEB -D__MIPSEB -D__MIPSEB__ -Iinclude/asm-rlx -Iarch/rlx/bsp_rtl8686/ -Idrivers/net/rtl86900/sdk/include/ -Idrivers/net/rtl86900/sdk/system/include  -I/home/shipeng/rtl86907/aaa/trunk/product/access/kernel/linux-2.6.x/arch/rlx/include/asm/mach-generic          -c -o rtk_voip/voip_ipc/coprocessor_dsp/script/dummy.o rtk_voip/voip_ipc/coprocessor_dsp/script/dummy.S

deps_rtk_voip/voip_ipc/coprocessor_dsp/script/dummy.o := \
  rtk_voip/voip_ipc/coprocessor_dsp/script/dummy.S \

rtk_voip/voip_ipc/coprocessor_dsp/script/dummy.o: $(deps_rtk_voip/voip_ipc/coprocessor_dsp/script/dummy.o)

$(deps_rtk_voip/voip_ipc/coprocessor_dsp/script/dummy.o):
