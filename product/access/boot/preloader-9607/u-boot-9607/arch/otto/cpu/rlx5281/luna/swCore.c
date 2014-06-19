/*
 * ----------------------------------------------------------------
 * Copyright c                  Realtek Semiconductor Corporation, 2002  
 * All rights reserved.
 * 
 *
 * Abstract: Switch core driver source code.
 *
 * $Author: yachang $
 *
 * ---------------------------------------------------------------
 */
#include <common.h>
#include "swCore.h"
#include <asm/arch/bspchip.h>
#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))

#define MACReg(offset, val)		(WRITE_MEM32(SWITCH_BASE + offset, val))

int swCore_init(void)
{
/* Move init to Lan_RXENABLE() at re8670poll.c */
#if 1
	//IO_CMD |= (RE << 5);
	int flag;
	//if(0 == analog_patch_flag) {
		unsigned int SOC_SUB_TYPE;

		//printf("%s %d\n", __func__, __LINE__);
		//printf("6266 swcore_init\r\n");
		SOC_SUB_TYPE=(REG32(BOND_CHIP_MODE)& 0xff);
		
		do
		{
			flag = *((volatile unsigned int *)0xb8000044) & 0x2;
		} while (flag == 0);
		
		//phy patch: port set phy-reg port 4 page 0 register 0 data 0x1340
		MACReg(0x0c, 0x00001340);
		MACReg(0x10, 0x0064a400);
		  //port 0: auto neg capability 10h/10f/100h/100f flow contrl enable
		MACReg(0x0c, 0x00000de1);
		MACReg(0x10, 0x0060a408);
		MACReg(0x0c, 0x00000c00);
		MACReg(0x10, 0x0060a412);
		MACReg(0x0c, 0x00001340);
		MACReg(0x10, 0x0060a400);
		  //port 1: auto neg capability 10h/10f/100h/100f flow contrl enable
		MACReg(0x0c, 0x00000de1);
		MACReg(0x10, 0x0061a408);
		MACReg(0x0c, 0x00000c00);
		MACReg(0x10, 0x0061a412);
		MACReg(0x0c, 0x00001340);
		MACReg(0x10, 0x0061a400);
		  //port 2: auto neg capability 10h/10f/100h/100f flow contrl enable
		MACReg(0x0c, 0x00000de1);
		MACReg(0x10, 0x0062a408);
		MACReg(0x0c, 0x00000c00);
		MACReg(0x10, 0x0062a412);
		MACReg(0x0c, 0x00001340);
		MACReg(0x10, 0x0062a400);
		//port 3: auto neg capability 10h/10f/100h/100f flow contrl enable
		MACReg(0x0c, 0x00000de1);
		MACReg(0x10, 0x0063a408);
		MACReg(0x0c, 0x00000c00);
		MACReg(0x10, 0x0063a412);
		MACReg(0x0c, 0x00001340);
		MACReg(0x10, 0x0063a400);
	
		  //patch phy done
		MACReg(0x88, 1);
		  //SVLAN uplink port
		MACReg(0x23110, 0);
		  //port isolation
		MACReg(0x27000, 0x3ffffff);
		MACReg(0x27004, 0x3ffffff);
		MACReg(0x27008, 0x3ffffff);
		MACReg(0x2700c, 0x1fff);
		//cpu port force mode
		MACReg(0x120, 0x40);
		//cpu port ability
		MACReg(0xa8, 0x196);
		//meter set tick-token tick-period 53 token 58
		MACReg(0x25000, 0x1353a);
		  //meter set pon-tick-token tick-period 53 token 58
		MACReg(0x25108, 0x1353a);
		  //vlan set state disable
		MACReg(0x13108, 0x00000000);
		  //vlan set tag-mode port 0-6 keep-format
		MACReg(0x20030, 0x00000001);
		MACReg(0x20430, 0x00000001);
		MACReg(0x20830, 0x00000001);
		MACReg(0x20c30, 0x00000001);
		MACReg(0x21030, 0x00000001);
		MACReg(0x21430, 0x00000001);
		MACReg(0x21830, 0x00000001);
		  //cpu set trap-insert-tag state disable
		MACReg(0x23064, 0x00000000);
		  //MOCIR_FRC_MD, 0711
		MACReg(0x2D8F8, 0xffffffff);
		  //#MOCIR_FRC_VAL, 0711
		MACReg(0x2D8FC, 0xffffffff);

		//phy4 select
		MACReg(0x170, 0x2);
		//tcont 0 scheduling queue 0-7
		MACReg(0x23280, 0xb28);
		MACReg(0x2de44, 0x1);
		MACReg(0x2dd08, 0xff);
		MACReg(0x2de34, 0x0);
		MACReg(0x2d908, 0x0);
		MACReg(0x2d90c, 0x0);
		MACReg(0x2d910, 0x0);
		MACReg(0x2d914, 0x0);
		MACReg(0x2d918, 0x0);
		MACReg(0x2d91c, 0x0);
		MACReg(0x2d920, 0x0);
		MACReg(0x2d924, 0x0);
		MACReg(0x2db08, 0x1ffff);
		MACReg(0x2db0c, 0x1ffff);
		MACReg(0x2db10, 0x1ffff);
		MACReg(0x2db14, 0x1ffff);
		MACReg(0x2db18, 0x1ffff);
		MACReg(0x2db20, 0x1ffff);
		MACReg(0x2db24, 0x1ffff);
		MACReg(0x1c3b8, 0xffffffff);
		
		*((volatile unsigned int *)0xb8000044) = *((volatile unsigned int *)0xb8000044) | 1;

		//analog_patch_flag = 1;
	//}
#endif

	return 0;
}



