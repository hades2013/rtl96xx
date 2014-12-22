/*
 * Copyright 2006, Realtek Semiconductor Corp.
 *
 * rtl8196b/bsp/bspchip.h:
 *   RTL8196B chip-level header file
 *
 * Tony Wu (tonywu@realtek.com.tw)
 * Nov. 07, 2006
 */
#ifndef _BSPCHIP_H_
#define _BSPCHIP_H_

#define CONFIG_RTL8686

/* System registers */
#define BONDING_REG             (0xBB0001E8)
#define STRAPING_REG                (0xBB0001EC)
#define BONDING_REG_DRAMTYPE_FREQ43_FD_S    (5) 
#define BONDING_REG_DRAMTYPE_FREQ43_MASK    (3 << BONDING_REG_DRAMTYPE_FREQ43_FD_S)
#define STRAPING_REG_DRAMTYPE_FREQ210_FD_S  (22)
#define STRAPING_REG_DRAMTYPE_FREQ210_MASK  (7 << STRAPING_REG_DRAMTYPE_FREQ210_FD_S)

#define SYSREG_SYSTEM_STATUS_REG        (0xB8000044)
#define SYSREG_PIN_STATUS_REG           (0xB8000100)
#define SYSREG_DDR123_IO_MODE_CONTROL_REG           (0xB8000144)
#define SYSREG_DDR123_IO_MODE_CONTROL_DDR3_MD_FD_S           (31)
#define SYSREG_DDR123_IO_MODE_CONTROL_DDR3_MD_MASK           (1 << SYSREG_DDR123_IO_MODE_CONTROL_DDR3_MD_FD_S)
#define SYSREG_LX_PLL_SEL_REG           (0xB8000228)
#define SYSREG_DRAM_CLK_EN_REG          (0xB800022c)
#define SYSREG_SYSCLK_CONTROL_REG       (0xB8000200)
#define SYSREG_MCKG_PHS_SEL_REG         (0xB8000220)
#define SYSREG_MCKG_FREQ_DIV_REG        (0xB8000224)
#define SYSREG_IP_EN_CTL_REG                (0xB8000600)
#define SYSREG_ANA1_CTL_REG             (0xB8000218)
#define SYS_DDRCLK_IO_DRI_CTL_REG       (0xB800021C)

#define SYSREG_PIN_STATUS_SPI_4B_EN_FD_S    (6)
#define SYSREG_PIN_STATUS_SPI_4B_EN_MASK    (1 << SYSREG_PIN_STATUS_SPI_4B_EN_FD_S)
#define SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_FD_S  (2)
#define SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_MASK  (1 << SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_FD_S)
#define SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_OCP0PLL SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_MASK
#define SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_LXPLL (0 << SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_FD_S)

#define SYSREG_PIN_STATUS_CKSEL_FD_S        (5)
#define SYSREG_PIN_STATUS_CKSEL_25MHZ       (0)
#define SYSREG_PIN_STATUS_CKSEL_40MHZ       (1)
#define SYSREG_PIN_STATUS_CKSEL_MASK        (1 << SYSREG_PIN_STATUS_CKSEL_FD_S)

#define SYSREG_DRAM_CLK_EN_FD_S         (0)
#define SYSREG_DRAM_CLK_EN_MASK         (1 << SYSREG_DRAM_CLK_EN_FD_S)
#define SYSREG_PIN_STATUS_CLSEL__FD_S       (5)
#define SYSREG_PIN_STATUS_CLSEL_MASK        (1 << SYSREG_PIN_STATUS_CLSEL__FD_S)
#define SYSREG_LX_DEFAULT_MHZ           (200)
#define SYSREG_MCKG_FREQ_DIV_FD_S           (0)
#define SYSREG_MCKG_FREQ_DIV_MASK           (0x3 << SYSREG_MCKG_FREQ_DIV_FD_S)

#define SYSREG_MCKG_PHS_SEL_PHS_FD_S        (0)
#define SYSREG_MCKG_PHS_SEL_PHS_MASK        (0x7 << SYSREG_MCKG_PHS_SEL_PHS_FD_S)
#define SYSREG_SYSCLK_CONTROL_SDPLL_FD_S    (0)
#define SYSREG_SYSCLK_CONTROL_SDPLL_MASK    (0x1f << SYSREG_SYSCLK_CONTROL_SDPLL_FD_S)
#define SYSREG_SYSCLK_CONTROL_OCP1PLL_FD_S  (8)
#define SYSREG_SYSCLK_CONTROL_OCP1PLL_MASK  (0x1f << SYSREG_SYSCLK_CONTROL_OCP1PLL_FD_S)
#define SYSREG_SYSCLK_CONTROL_OCP0PLL_FD_S  (16)
#define SYSREG_SYSCLK_CONTROL_OCP0PLL_MASK  (0x1f << SYSREG_SYSCLK_CONTROL_OCP0PLL_FD_S)
#define SYSREG_DDRCKODL_DDRCLM_TAP_FD_S     (8)
#define SYSREG_DDRCKODL_DDRCLM_TAP_MASK     (0x1f << SYSREG_DDRCKODL_DDRCLM_TAP_FD_S)
#define SYSREG_DDRCKODL_PH90_TAP_FD_S       (16)
#define SYSREG_DDRCKODL_PH90_TAP_MASK       (0x1f << SYSREG_DDRCKODL_PH90_TAP_FD_S)
#define SYSREG_DDRCKODL_DDRCK_PHS_FD_S (0)
#define SYSREG_DDRCKODL_DDRCK_PHS_MASK (0x1f << SYSREG_DDRCKODL_DDRCK_PHS_FD_S)
#define SYSREG_DDRCKODL_DDRCLM90_TAP_FD_S (16)
#define SYSREG_DDRCKODL_DDRCLM90_TAP_MASK (0x1f << SYSREG_DDRCKODL_DDRCLM90_TAP_FD_S)
#define SYSREG_OCP0_DEFAULT_MHZ         (500)
#define SYSREG_DDRCKODL_REG         (0xB800021c)

#define SYSREG_CMUCTLR_REG          (0xB8000308)
#define SYSREG_OCP0_SMALLER_MASK        (16)
#define SYSREG_OCP1_SMALLER_MASK        (8)
#define SYSREG_LX0_SMALLER_MASK         (4)
#define SYSREG_LX1_SMALLER_MASK         (2)
#define SYSREG_LX2_SMALLER_MASK         (1)


/*
 * IRQ Controller
 */

/*
 *  ====================================
 *  Platform Configurable Common Options
 *  ====================================
 */

            
/*                        
 * IRQ Mapping
 */


/*
 * Interrupt Routing Selection
 */

/*
 *  ==========================
 *  Platform Register Settings
 *  ==========================
 */

/*
 * CPU
 */

/*
 * SoC
 */
#define CHIP_ID_REG		0xBB010004

#define BOND_CHIP_MODE 0xBB010008	/* bond chip information */
		#define PAD_BOND_DMY_0 (1 << 0) //reserved 
		#define PAD_BOND_DMY_1 (1 << 1)
		#define PAD_BOND_DMY_2 (1 << 2)
		#define PAD_BOND_CHIP_MODE_0	(1 << 3)
		#define PAD_BOND_CHIP_MODE_1	(1 << 4)
        #define CHIP_901      (PAD_BOND_DMY_0|PAD_BOND_DMY_1)
        #define CHIP_906_1    (PAD_BOND_DMY_0|PAD_BOND_DMY_1|PAD_BOND_DMY_2)
        #define CHIP_906_2    (PAD_BOND_DMY_0|PAD_BOND_CHIP_MODE_0)
        #define CHIP_907      (PAD_BOND_CHIP_MODE_0|PAD_BOND_DMY_0|PAD_BOND_DMY_1)
        #define CHIP_902      (PAD_BOND_CHIP_MODE_0|PAD_BOND_DMY_0|PAD_BOND_DMY_2)
        #define CHIP_903      (PAD_BOND_CHIP_MODE_0|PAD_BOND_DMY_0|PAD_BOND_DMY_1|PAD_BOND_DMY_2)
        #define CHIP_96       (PAD_BOND_CHIP_MODE_1|PAD_BOND_DMY_0|PAD_BOND_DMY_1)
        #define CHIP_98B      (PAD_BOND_CHIP_MODE_1|PAD_BOND_DMY_0|PAD_BOND_DMY_1|PAD_BOND_DMY_2)
		#define CHIP_2510	(PAD_BOND_CHIP_MODE_0 | PAD_BOND_CHIP_MODE_1 | PAD_BOND_DMY_0)
        #define CHIP_96P      (PAD_BOND_CHIP_MODE_0 | PAD_BOND_CHIP_MODE_1 |PAD_BOND_DMY_1 | PAD_BOND_DMY_0)
        #define CHIP_DBG      (PAD_BOND_CHIP_MODE_0|PAD_BOND_CHIP_MODE_1|PAD_BOND_DMY_0|PAD_BOND_DMY_1|PAD_BOND_DMY_2)


/*
 * Memory Controller
 */

/*
 * NAND flash controller address
 */
//czyao , nand flash address

/*
 * UART
 */

/*
 * Interrupt Controller
 */

/*
 * System Clock
 */


/*
 * WATCHDOG
 */
#define WDTCNTRR_A           (0xB8003260)
    #define WDTKICK              (0x1 <<31)
#define WDTINTRR_A           (0xB8003264)
    #define PH1_IP               (0x1 <<31)
    #define PH2_IP               (0x1 <<30)
#define WDTCTRLR_A           (0xB8003268)


#endif   /* _BSPCHIP_H */
