sram_init:
  la      t0, 0xB8001300
  li      t1, 0x1F000001
  sw      t1, 0(t0)
  li      t2, 0x00000008
  sw      t2, 4(t0)

  la      t0, 0xB8004000
  sw      t1, 0(t0) //SRAMSAR0 for base address SRAM maps to
  sw      t2, 4(t0) //SRAMSSR0 for size of SRAM maps to (32K)

/* Reset SWCORE for its SRAM */
  la      t0, 0xBB000074
  li      t1, 0x00000010
  sw      t1, 0(t0)

/* check switch for ready patch, because we will setting switch register for SRAM usage */
  li      t0, 0xB8000044
1:
  lw      t2, 0(t0)
  andi    t2, t2, 0x2
  beq     zero, t2, 1b
  nop

/* Allocate Switch SRAM for SoC */
  la      t0, 0xBB000178
  li      t1, 0x0
  sw      t1, 0(t0)

  la      t0, 0xB8001310
  li      t1, 0x1F008001
  sw      t1, 0(t0)
  li      t2, 0x00000008
  sw      t2, 4(t0)

  la      t0, 0xB8004010
  sw      t1, 0(t0)
  sw      t2, 4(t0) //Map SRAM 0x0 to OCP0 0x1f008000, size=32KB

#if 0
/* reset the whole SRAM to zero */
  li      t0, SRAM_SIZE
  li      t1, UCSRAM_BASE
  addu    t0, t0, t1
1:
  sw      zero, 0x00(t1)
  sw      zero, 0x04(t1)
  sw      zero, 0x08(t1)
  sw      zero, 0x0c(t1)
  sw      zero, 0x10(t1)
  sw      zero, 0x14(t1)
  sw      zero, 0x18(t1)
  sw      zero, 0x1c(t1)
  sw      zero, 0x20(t1)
  sw      zero, 0x24(t1)
  sw      zero, 0x28(t1)
  sw      zero, 0x2c(t1)
  sw      zero, 0x30(t1)
  sw      zero, 0x34(t1)
  sw      zero, 0x38(t1)
  addi    t1, 0x40
  bne     t0, t1, 1b
  sw      zero, 0x3c(t1)
#endif

