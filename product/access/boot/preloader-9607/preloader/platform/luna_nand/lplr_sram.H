sram_init:
  li      t0, 0xB8001300
  li      t1, 0x1FC00001
  sw      t1, 0(t0) //unmapping 1fc00001
  li      t1, 0x00000008
  sw      t1, 4(t0) //unmapping size

  li      t0, 0xB8004000
  li      t1, 0x1FC00001
  sw      t1, 0(t0) //sram for base address SRAM maps to
  li      t1, 0x00000008
  sw      t1, 4(t0) //sram for size of SRAM maps to (32K)
//segment 1
/* Switch PON SRAM to SoC */
  la      t0, 0xBB000178
  li      t1, 0x0
  sw      t1, 0(t0)
  la      t0, 0xB8001310
  li      t1, 0x1FC08001
  sw      t1, 0(t0)
  li      t2, 0x00000008
  sw      t2, 4(t0)
  la      t0, 0xB8004010
  sw      t1, 0(t0)
  sw      t2, 4(t0) //Map SRAM 0x0 to OCP0 0x1fc08000, size=32KB


//segment 2
#if (SRAM_SIZE >= 0x20000)
la 		t0,0xb8004028 //set sram base address for segment 2
li		t1,0x8000
sw 		t1,0(t0)
la              t0, 0xB8001320 //set dram unmapping address
li              t1, 0x1FC10001
sw              t1, 0(t0)
li              t2, 0x00000009	//64k
sw              t2, 4(t0)
la              t0, 0xB8004020
sw              t1, 0(t0)
sw              t2, 4(t0) //Map sram 0x8000 to 1fc10000 , size 64k
#endif
