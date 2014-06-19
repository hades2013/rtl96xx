#ifdef _AIPC_CPU_
#include <linux/module.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/version.h>
#include <linux/spinlock.h>
#include <linux/delay.h>

#include "./include/aipc_mem.h"
#include "./include/aipc_reg.h"
#include "./include/dram_share.h"
#include "./include/aipc_ioctl.h"

static volatile unsigned int *dsp_boot_ins;

#ifndef CONFIG_RTL8686_ASIC_TEST_IP

static unsigned int jump_ins[] = {
                0x3c088000        /*lui     t0,0x8000*/,
                0x350804c4        /*ori     t0,t0,0x04c4*/,
                0x01000008        /*jr      t0*/,
                0x00000000        /*nop       */
              };

#else

static unsigned int jump_ins[] = {
//				0x3c0880c0		  /*lui 	t0,0x80c0*/,
				0x3c088000        /*lui     t0,0x8000*/,
				0x35080000        /*ori     t0,t0,0x0000*/,
				0x01000008        /*jr      t0*/,
				0x00000000        /*nop		  */
			  };
#endif

int
aipc_cpu_sram_map(
	u8_t  seg_no , 
	u32_t map_addr , 
	u32_t size , 
	u8_t  enable , 
	u32_t base_addr , 
	u8_t  lx_match)
{
	void *seg_addr=NULL;
	volatile u32_t tmp=0;
	if ((void*)map_addr==NULL || seg_no>=SRAM_SEG_MAX){
		printk( "wrong sram map setting\n" );
		return NOK;	
		}

	if (seg_no==SRAM_SEG_IDX_0){
		seg_addr = (void*)R_C0SRAMSAR0;
		}
	else if (seg_no==SRAM_SEG_IDX_1){
		seg_addr = (void*)R_C0SRAMSAR1;
		}
	else if (seg_no==SRAM_SEG_IDX_2){
		seg_addr = (void*)R_C0SRAMSAR2;
		}
	else if (seg_no==SRAM_SEG_IDX_3){
		seg_addr = (void*)R_C0SRAMSAR3;
		}
	else{
		return NOK;	
		}

	tmp	= Virtual2Physical(map_addr);
	SDEBUG("sram map_addr v2p=%x\n" , Virtual2Physical(map_addr));
	
	if (enable==SRAM_SEG_ENABLE){
		tmp	|= SRAM_SEG_ENABLE_BIT;		//enable
		}
	else{
		tmp	&= ~SRAM_SEG_ENABLE_BIT;
		}
	
	if (lx_match==SRAM_LX_MATCH_ENABLE){
		tmp	|= SRAM_LX_MATCH_BIT;
		}
	else{
		tmp	&= ~SRAM_LX_MATCH_BIT;
		}

	REG32(seg_addr)		=	tmp;	
	REG32(seg_addr+4)	=	size;
	REG32(seg_addr+8)	=	base_addr;	//base
	
	return OK;
}

int
aipc_cpu_dram_unmap(
	u8_t  seg_no , 
	u32_t unmap_addr , 
	u32_t size , 
	u8_t  enable , 
	u8_t  lx_match)
{
	void *seg_addr=NULL;
	volatile u32_t tmp=0;	
	if ((void*)unmap_addr==NULL || seg_no>=DRAM_SEG_MAX){
		printk( "wrong dram unmap setting\n" );
		return NOK;	
		}

	if (seg_no==DRAM_SEG_IDX_0){
		seg_addr = (void*)R_C0UMSAR0;
		}
	else if (seg_no==DRAM_SEG_IDX_1){
		seg_addr = (void*)R_C0UMSAR1;
		}
	else if (seg_no==DRAM_SEG_IDX_2){
		seg_addr = (void*)R_C0UMSAR2;
		}
	else if (seg_no==DRAM_SEG_IDX_3){
		seg_addr = (void*)R_C0UMSAR3;
		}
	else{
		return NOK;	
		}
	
	tmp	= Virtual2Physical(unmap_addr);
	SDEBUG("dram unmap_addr v2p=%x\n" , Virtual2Physical(unmap_addr));

	if (enable==DRAM_SEG_ENABLE){
		tmp	|= DRAM_SEG_ENABLE_BIT;		//enable
		}
	else{
		tmp	&= ~DRAM_SEG_ENABLE_BIT;
		}

	if (lx_match==DRAM_LX_MATCH_ENABLE){
		tmp	|= DRAM_LX_MATCH_BIT;
		}
	else{
		tmp	&= ~DRAM_LX_MATCH_BIT;
		}
	
	REG32(seg_addr)		=	tmp;
	REG32(seg_addr+4)	=	size;	

	return OK;
}

int
aipc_dsp_sram_map(
	u8_t  seg_no , 
	u32_t map_addr , 
	u32_t size , 
	u8_t  enable , 
	u32_t base_addr , 
	u8_t  lx_match)
{
	void *seg_addr=NULL;
	volatile u32_t tmp=0;	
	if ((void*)map_addr==NULL || seg_no>=SRAM_SEG_MAX){
		printk( "wrong sram map setting\n" );
		return NOK;	
		}

	if (seg_no==SRAM_SEG_IDX_0){
		seg_addr = (void*)R_C1SRAMSAR0;
		}
	else if (seg_no==SRAM_SEG_IDX_1){
		seg_addr = (void*)R_C1SRAMSAR1;
		}
	else if (seg_no==SRAM_SEG_IDX_2){
		seg_addr = (void*)R_C1SRAMSAR2;
		}
	else if (seg_no==SRAM_SEG_IDX_3){
		seg_addr = (void*)R_C1SRAMSAR3;
		}
	else{
		return NOK;	
		}

	tmp	= Virtual2Physical(map_addr);
	if (enable==SRAM_SEG_ENABLE){
		tmp	|= SRAM_SEG_ENABLE_BIT;		//enable
		}
	else{
		tmp	&= ~SRAM_SEG_ENABLE_BIT;
		}

	if (lx_match==SRAM_LX_MATCH_ENABLE){
		tmp	|= SRAM_LX_MATCH_BIT;
		}
	else{
		tmp	&= ~SRAM_LX_MATCH_BIT;
		}
	
	REG32(seg_addr)		=	tmp;
	REG32(seg_addr+4)	=	size;
	REG32(seg_addr+8)	=	base_addr;	//base
		
	return OK;
}

int
aipc_dsp_dram_unmap(
	u8_t  seg_no , 
	u32_t unmap_addr , 
	u32_t size , 
	u8_t  enable , 
	u8_t  lx_match)
{
	void *seg_addr=NULL;
	volatile u32_t tmp=0;		
	if ((void*)unmap_addr==NULL || seg_no>=DRAM_SEG_MAX){
		printk( "wrong dram unmap setting\n" );
		return NOK;	
		}
 
	if (seg_no==DRAM_SEG_IDX_0){
		seg_addr = (void*)R_C1UMSAR0;
		}
	else if (seg_no==DRAM_SEG_IDX_1){
		seg_addr = (void*)R_C1UMSAR1;
		}
	else if (seg_no==DRAM_SEG_IDX_2){
		seg_addr = (void*)R_C1UMSAR2;
		}
	else if (seg_no==DRAM_SEG_IDX_3){
		seg_addr = (void*)R_C1UMSAR3;
		}
	else{
		return NOK;
		}
	
	tmp	= Virtual2Physical(unmap_addr);
	if (enable==DRAM_SEG_ENABLE){
		tmp	|= DRAM_SEG_ENABLE_BIT;		//enable
		}
	else{
		tmp	&= ~DRAM_SEG_ENABLE_BIT;
		}
	
	if (lx_match==DRAM_LX_MATCH_ENABLE){
		tmp	|= DRAM_LX_MATCH_BIT;
		}
	else{
		tmp	&= ~DRAM_LX_MATCH_BIT;
		}

	REG32(seg_addr)		= 	tmp;
	REG32(seg_addr+4)	=	size;

	return OK;
}

int
aipc_cpu_set_mem(void)
{
#ifdef GPON_RESV
//CPU
	//set sram map
	aipc_cpu_sram_map(
		SRAM_SEG_IDX_1,			// CPU1 can only write SEG 1 
		AIPC_CPU_IPC_SRAM_ADDR , 
#ifndef CONFIG_DESC_IN_SRAM
		SRAM_SIZE_32KB, 
#else
		SRAM_SIZE_8KB,
#endif
		SRAM_SEG_ENABLE , 
		0 , 
		~SRAM_LX_MATCH_ENABLE);

	//set dram unmap
	aipc_cpu_dram_unmap(
		DRAM_SEG_IDX_1, 
		AIPC_CPU_IPC_SRAM_ADDR , 
#ifndef CONFIG_DESC_IN_SRAM
		DRAM_SIZE_32KB, 
#else
		DRAM_SIZE_8KB,
#endif
		DRAM_SEG_ENABLE , 
		~DRAM_LX_MATCH_ENABLE);
	
//DSP
	//set sram map
	aipc_dsp_sram_map(
		SRAM_SEG_IDX_1, 
		AIPC_DSP_IPC_SRAM_ADDR , 
#ifndef CONFIG_DESC_IN_SRAM
		SRAM_SIZE_32KB, 
#else
		SRAM_SIZE_8KB,
#endif
		SRAM_SEG_ENABLE , 
		0 , 
		SRAM_LX_MATCH_ENABLE);	

	//set dram unmap
	aipc_dsp_dram_unmap(
		DRAM_SEG_IDX_1, 
		AIPC_DSP_IPC_SRAM_ADDR , 
#ifndef CONFIG_DESC_IN_SRAM
		DRAM_SIZE_32KB, 
#else
		DRAM_SIZE_8KB,
#endif
		DRAM_SEG_ENABLE , 
		DRAM_LX_MATCH_ENABLE);
#else
//CPU
	//set sram map
	aipc_cpu_sram_map(
		SRAM_SEG_IDX_0 , 
		AIPC_CPU_IPC_SRAM_ADDR , 
		SRAM_SIZE_128KB , 
		SRAM_SEG_ENABLE , 
		0 , 
		~SRAM_LX_MATCH_ENABLE);

	//set dram unmap
	aipc_cpu_dram_unmap(
		DRAM_SEG_IDX_0 , 
		AIPC_CPU_IPC_SRAM_ADDR , 
		DRAM_SIZE_128KB , 
		DRAM_SEG_ENABLE , 
		~DRAM_LX_MATCH_ENABLE);
	
//DSP
	//set sram map
	aipc_dsp_sram_map(
		SRAM_SEG_IDX_0 , 
		AIPC_DSP_IPC_SRAM_ADDR , 
		SRAM_SIZE_128KB , 
		SRAM_SEG_ENABLE , 
		0 , 
		SRAM_LX_MATCH_ENABLE);	

	//set dram unmap
	aipc_dsp_dram_unmap(
		DRAM_SEG_IDX_0 , 
		AIPC_DSP_IPC_SRAM_ADDR , 
		DRAM_SIZE_128KB , 
		DRAM_SEG_ENABLE , 
		DRAM_LX_MATCH_ENABLE);
#endif

	return OK;
}

int
aipc_cpu_rom_map( 
	u32_t map_addr , 
	u32_t clk , 
	u32_t wait ,
	u32_t size , 
	u8_t  enable)
{
	volatile u32_t tmp=0;
	
	if ((void*)map_addr==NULL){
		printk( "wrong rom map setting\n" );
		return NOK;
	}

	//Setup segment address	register
#if 0
	tmp	= Virtual2Physical(map_addr) & ROM_SEG_MASK;
#else
	tmp	= Virtual2Physical(map_addr);
#endif
	
	if (enable==ROM_SEG_ENABLE){
		tmp	|= ROM_SEG_ENABLE_BIT;		//enable
		}
	else{
		tmp	&= ~ROM_SEG_ENABLE_BIT;
		}
	REG32(R_ROMSAR) = tmp;
	
	
	//Setup segment size register
	tmp = REG32(R_ROMSSR);
	
	if (clk==ROM_CLOCK_DIV_2){
		tmp &= ~ROM_CLOCK_DIV_BIT;
	}
	else{	//ROM_CLOCK_DIV_4
		tmp |= ROM_CLOCK_DIV_BIT;
	}
	
	if (wait>ROM_WAIT_TIME_NONE && wait<=ROM_WAIT_TIME_7T){
		tmp |= (wait<<7);
	}
	
#ifdef CONFIG_RTL8686_FPGA		//in FPGA phase
	if (clk==ROM_CLOCK_DIV_NONE){
		tmp &= ~ROM_CLOCK_DIV_BIT;
	}
	if (wait==ROM_WAIT_TIME_NONE){
		tmp &= (0<<7);
		tmp &= (0<<8);
		tmp &= (0<<9);
	}
#endif
		
	if (size>=ROM_SIZE_32K && size<=ROM_SIZE_1M){
		tmp |= size;
	}
	
	REG32(R_ROMSSR) = tmp;

	SDEBUG("ROM Address Reg=%x\n" , REG32(R_ROMSAR));
	SDEBUG("ROM Size Reg=%x\n"    , REG32(R_ROMSSR));
	
	return OK;	
}

int
aipc_dsp_entry(void)
{
	int i=0;

#ifdef GPON_RESV
	aipc_cpu_sram_map(
		SRAM_SEG_IDX_1, 
		DSP_BOOT_INS_SRAM_MAPPING_ADDR , 
#ifndef CONFIG_DESC_IN_SRAM
		SRAM_SIZE_32KB , 
#else
		SRAM_SIZE_8KB,
#endif
		SRAM_SEG_ENABLE , 
		GPON_SRAM_BASE , 		//Add GPON SRAM BASE
		~SRAM_LX_MATCH_ENABLE);

	aipc_cpu_dram_unmap(
		DRAM_SEG_IDX_1,
		DSP_BOOT_INS_SRAM_MAPPING_ADDR , 
#ifndef CONFIG_DESC_IN_SRAM
		DRAM_SIZE_32KB, 
#else
		DRAM_SIZE_8KB,
#endif
		DRAM_SEG_ENABLE , 
		~DRAM_LX_MATCH_ENABLE);
#else
	aipc_cpu_sram_map(
		SRAM_SEG_IDX_0 , 
		DSP_BOOT_INS_SRAM_MAPPING_ADDR , 
		SRAM_SIZE_128KB , 
		SRAM_SEG_ENABLE , 
		0 , 
		~SRAM_LX_MATCH_ENABLE);

	aipc_cpu_dram_unmap(
		DRAM_SEG_IDX_0 , 
		DSP_BOOT_INS_SRAM_MAPPING_ADDR , 
		DRAM_SIZE_128KB , 
		DRAM_SEG_ENABLE , 
		~DRAM_LX_MATCH_ENABLE);
#endif

	dsp_boot_ins = ((volatile unsigned int *)DSP_BOOT_INS_SRAM_MAPPING_ADDR);
	
	for(i=0 ; i<sizeof(jump_ins) ; i++){
		*dsp_boot_ins = jump_ins[i];
		dsp_boot_ins++;
	}

	return 0;
}

int
aipc_rom_set(unsigned int rom_addr)
{
#ifdef CONFIG_RTL8686_ASIC
	aipc_cpu_rom_map(
		rom_addr , //DSP_ROMCODE_ADDR
		ROM_CLOCK_DIV_NONE , 
		ROM_WAIT_TIME_NONE , 
		ROM_SIZE_256K , 
		ROM_SEG_ENABLE);

#else		//in FPGA phase

#ifdef GPON_RESV

	aipc_cpu_rom_map(
		rom_addr , //DSP_ROMCODE_ADDR
		ROM_CLOCK_DIV_NONE , 
		ROM_WAIT_TIME_NONE , 
		ROM_SIZE_256K , 
		ROM_SEG_ENABLE);

#else

#if 1
	aipc_cpu_sram_map(
		SRAM_SEG_IDX_0 ,
		rom_addr , 			//DSP_ROMCODE_ADDR
		SRAM_SIZE_128KB , 
		SRAM_SEG_ENABLE , 
		0 , 
		~SRAM_LX_MATCH_ENABLE);

	aipc_cpu_sram_map(
		SRAM_SEG_IDX_1 ,
		rom_addr+ROM_BASE_128K , 	//DSP_ROMCODE_ADDR+0x20000
		SRAM_SIZE_128KB , 
		SRAM_SEG_ENABLE , 
		ROM_BASE_128K , 			//base=0x20000
		~SRAM_LX_MATCH_ENABLE);
#else
#if 1
	aipc_cpu_sram_map(
		SRAM_SEG_IDX_0 ,
		rom_addr , //DSP_ROMCODE_ADDR
		SRAM_SIZE_128KB , 
		SRAM_SEG_ENABLE , 
		0 , 
		~SRAM_LX_MATCH_ENABLE);

#else		//it's also ok to use SRAM segment 1.
	aipc_cpu_sram_map(
		SRAM_SEG_IDX_1 , 
		rom_addr , //DSP_ROMCODE_ADDR
		SRAM_SIZE_128KB , 
		SRAM_SEG_ENABLE , 
		0 , 
		~SRAM_LX_MATCH_ENABLE);
#endif
#endif
#endif

#endif
	return OK;
}

int
aipc_zone_set(zone_plan_t zp)
{

#ifndef	CONFIG_RTL8686_ASIC_TEST_IP

	if (zp==zp_dsp_init){
		//CPU Zone 0
		REG32(C0DOR0)  = CONFIG_RTL8686_CPU_MEM_BASE;
		REG32(C0DMAR0) = CONFIG_RTL8686_CPU_MEM_SIZE-1;
		
		//CPU Zone 1
		REG32(C0DOR1)  = CONFIG_RTL8686_DSP_MEM_BASE;
		REG32(C0DMAR1) = CONFIG_RTL8686_DSP_MEM_SIZE-1;

		//DSP Zone 0
		REG32(C1DOR0)  = CONFIG_RTL8686_DSP_MEM_BASE;
		REG32(C1DMAR0) = CONFIG_RTL8686_DSP_MEM_SIZE-1;

		//DSP Zone 1
		REG32(C1DOR1)  = CONFIG_RTL8686_IPC_MEM_BASE;	
		REG32(C1DMAR1) = CONFIG_RTL8686_IPC_MEM_SIZE-1;
	}
	else {
		//CPU Zone 1
		REG32(C0DOR1)  = CONFIG_RTL8686_IPC_MEM_BASE;
		REG32(C0DMAR1) = CONFIG_RTL8686_IPC_MEM_SIZE-1;
	}

	SDEBUG("CPU zone config:\n");
	SDEBUG("  C0DOR0=%08x C0DMAR0=%08x\n" , REG32(C0DOR0) , REG32(C0DMAR0));
	SDEBUG("  C0DOR1=%08x C0DMAR1=%08x\n" , REG32(C0DOR1) , REG32(C0DMAR1));
	
	SDEBUG("DSP zone config:\n");
	SDEBUG("  C1DOR0=%08x C1DMAR0=%08x\n" , REG32(C1DOR0) , REG32(C1DMAR0));
	SDEBUG("  C1DOR1=%08x C1DMAR1=%08x\n" , REG32(C1DOR1) , REG32(C1DMAR1));
	return 0;

#else
	unsigned int dram_size = AIPC_DRAM_SIZE;	//64MB

	//CPU Zone 1
	REG32(C0DOR1)  = dram_size/2;
	REG32(C0DMAR1) = dram_size-1;

	//DSP Zone 0
	REG32(C1DOR0)  = dram_size/2;	
	REG32(C1DMAR0) = dram_size-1;

	//DSP Zone 1
	REG32(C1DOR1)  = dram_size/2;	
	REG32(C1DMAR1) = dram_size-1;

	return 0;

#endif
}

#ifdef CONFIG_RTL8686_ASIC

int
aipc_dsp_boot(void)
{
    volatile unsigned int *dsp_reg;
    volatile u32_t tmp=0;

	/*
	*	1. add clock to DSP and set DSP TAP on.
	*/
    dsp_reg 	= 	(volatile unsigned int *)(R_AIPC_ASIC_ENABLE_DSP_CLK);
    tmp			=   *dsp_reg;
    
    tmp			&=  ~BIT_ENABLE_DSP_TAP;	// Set BIT(6) as 0 to enable DSP TAP.
    tmp			|= 	BIT_ENABLE_DSP_CLOCK;
    
    *dsp_reg 	= 	tmp;

	/*
	*	2. delay 4 ms
	*/
	mdelay(KICK_DSP_DELAY_TIME);

	/*
	*	3. kick DSP
	*/ 
    dsp_reg 	= 	(volatile unsigned int *)(R_AIPC_ASIC_KICK_DSP);
    *dsp_reg 	|= 	BIT_KICK_DSP;


	/*
	*	4. kernel delay time
	*/
#ifdef KERNEL_BOOT_DELAY_TIME
	mdelay(KERNEL_BOOT_DELAY_TIME);
#endif
	
	return OK;
}

#else

int
aipc_dsp_boot(void)
{
    volatile unsigned int *dsp_kick_reg;

    dsp_kick_reg 	= (volatile unsigned int *)(R_AIPC_BOOT_DSP);
    *dsp_kick_reg 	|= BOOT_DSP_BIT;
	
	return OK;
}

#endif

#ifdef AIPC_BOOT
/*
*	Shared memory init function
*/
int aipc_boot_init(void)
{									//DSP booting address
//	ABOOT.cmd[0] = 0x3c08bfc0;		/*lui 	t0,0xbfc0*/
	ABOOT.cmd[0] = 0x3c088000;		/*lui 	t0,0x8000*/
	ABOOT.cmd[1] = 0x35080000;		/*ori 	t0,t0,0x0000*/
	ABOOT.cmd[2] = 0x01000008;		/*jr	t0*/
	ABOOT.cmd[3] = 0x00000000;		/*nop 	  */
	
	return OK;
}
#endif

#endif 

