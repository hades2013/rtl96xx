#define __ASM__
#include "mipsregs.h"
#include <regdef.h>
#include "rlx5281.h"
#include "dram_init.h"

.text
       .globl apollo_dram_init
.ent apollo_dram_init
apollo_dram_init:
/* hard copy for dram setting */
/* 1. DRAM Clock 100MHz, CPU 500MHz, we asum LX bus 200MHz */
	li	t0, 0xb8000200
	li	t1, 0x4  
	sw	t1, 0(t0)
	
	li	t0, 0xb8000224
	li	t1, 0x2  
	sw	t1, 0(t0)

	li	t0, 0xb8000220
	li	t1, 0x2  
	sw	t1, 0(t0)

/* 2. Enable DRAM clock  */
	li	t0, 0xB800022c
	li	t1, 0x1  
	sw	t1, 0(t0)

/* 3. Clock reverse setting: LX clock > DRAM clock */
	li	t0, 0xB8000308
	li	t1, 0x03ffff00  
	sw	t1, 0(t0)

/* 4. DCR: DRAM size configuration */

	li	t0, 0xb8001004
	li	t1, 0x10320000  
	sw	t1, 0(t0)

/* 5. DTR: DRAM timing configuration */

	li	t0, 0xb8001008
	li	t1, 0x53422b10  
	sw	t1, 0(t0)
	
	li	t0, 0xb800100c
	li	t1, 0x03211000  
	sw	t1, 0(t0)

	li	t0, 0xb8001010
	li	t1, 0x04040311  
	sw	t1, 0(t0)


/* 6. DQS delay configuraion */

	li	t0, 0xb8001510
	li	t1, 0x0f000f00  
	sw	t1, 0(t0)

	li	t0, 0xb8001514
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb8001518
	nop
	sw	t1, 0(t0)

	li	t0, 0xb800151c
	nop
	sw	t1, 0(t0)

	li	t0, 0xb8001520
	nop
	sw	t1, 0(t0)
		
	li	t0, 0xb8001524
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb800152c
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb8001530
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb8001540
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb8001544
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb8001548
	nop
	sw	t1, 0(t0)
	li	t0, 0xb800154c
	nop
	sw	t1, 0(t0)

	li	t0, 0xb8001550
	li	t1, 0x00000f00  
	sw	t1, 0(t0)
		
	li	t0, 0xb8001554
	nop
	sw	t1, 0(t0)							
	li	t0, 0xb8001558
	nop
	sw	t1, 0(t0)
	li	t0, 0xb800155c
	nop
	sw	t1, 0(t0)
	li	t0, 0xb8001560
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb8001564
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb8001568
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb800156c
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb8001570
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb8001574
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb8001578
	nop
	sw	t1, 0(t0)	
							
	li	t0, 0xb800157c
	nop
	sw	t1, 0(t0)	
	
	li	t0, 0xb8001570
	nop
	sw	t1, 0(t0)

	li	t0, 0xb8001574
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb8001578
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb800157c
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb8001580
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb8001584
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb8001588
	nop
	sw	t1, 0(t0)
	
	li	t0, 0xb800158c
	nop
	sw	t1, 0(t0)

	li	t0, 0xb8001590
	li	t1, 0x0
	sw	t1, 0(t0)


/* 7. Configure DDR2 DRAM Mode registers */
		
   li	t0, 0xb800101c
   li t1, 0x00110001 
	 sw t1, 0(t0)
	 

        /* waiting for MRS busy */
        li      t1, 0x80000000
1:
        lw      t4, 0(t0)
        nop
        and     t4, t4, t1
        bne     zero, t4, 1b
        nop




//ew 0xb800101c = 0x00110004;
//w 10;


     /* MR2 */
        li      t0, 0xb800101c
        li      t1, 0x00110004 
        sw      t1, 0(t0)

        /* waiting for MRS busy */
        li      t1, 0x80000000
1:
        lw      t4, 0(t0)
        nop
        and     t4, t4, t1
        bne     zero, t4, 1b
        nop



//ew 0xb800101c = 0x00100762
//w 10;

     /* MR2 */
        li      t0, 0xb800101c
        li      t1, 0x00100762 
        sw      t1, 0(t0)

        /* waiting for MRS busy */
        li      t1, 0x80000000
1:
        lw      t4, 0(t0)
        nop
        and     t4, t4, t1
        bne     zero, t4, 1b
        nop



//ew 0xb800101c = 0x00120000
//w 10;

     /* MR2 */
        li      t0, 0xb800101c
        li      t1, 0x00120000 
        sw      t1, 0(t0)

        /* waiting for MRS busy */
        li      t1, 0x80000000
1:
        lw      t4, 0(t0)
        nop
        and     t4, t4, t1
        bne     zero, t4, 1b
        nop
        

//ew 0xb800101c = 0x00100662
//w 10;


     /* MR2 */
        li      t0, 0xb800101c
        li      t1, 0x00100662 
        sw      t1, 0(t0)

        /* waiting for MRS busy */
        li      t1, 0x80000000
1:
        lw      t4, 0(t0)
        nop
        and     t4, t4, t1
        bne     zero, t4, 1b
        nop
        

/* 8. Reset phy fifo pointer */
//ew 0xb8001500 = 0xc0000000;ew 0xb8001500 = 0xc0000010;

	li	t0, 0xb8001500
	li	t1, 0xc0000000
	sw	t1, 0(t0)
	
	li	t0, 0xb8001500
	li	t1, 0xc0000010
	sw	t1, 0(t0)
	
	
	jr	ra
	nop
.end apollo_dram_init
