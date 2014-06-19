/*
=============================================================================
Header Name:opconn_hw_reg.h

  General Description:
  
	This file define all opcomm chip regisger and table address.
	===============================================================================
	Opulan Confidential Proprietary                     
	ID and version: xxxxxxxxxxxxxx  Version 1.00
	(c) Copyright Opulan XXXX - XXXX, All Rights Reserved
	
	  
		Revision History:
		Modification
		Author			Date				Description of Changes
		---------------	---------------	----------------------------------------------
		zzhu			2007/10/09		Initial Version
		---------------	---------------	----------------------------------------------
*/
#ifndef OPCONN_HW_REG_H
#define OPCONN_HW_REG_H

#define OPCONN_BASE_ADDR		0x00000000

/* gloable register */
#define GB_BA_B                  (OPCONN_BASE_ADDR + 0x0000)

#define REG_GB_SOFTRST_ADN_DEVID			((GB_BA_B + 0x0000)*4)
#define REG_FPGA_VERSION				    ((GB_BA_B + 0x0001)*4)
#define REG_GB_CHIP_INIT					((GB_BA_B + 0x0002)*4)
#define REG_GB_LL_CTRL						((GB_BA_B + 0x0003)*4)
#define REG_GB_DRAM_SIZE					((GB_BA_B + 0x0004)*4)
#define REG_GB_LL_ALMOST_PAGE_NUM			((GB_BA_B + 0x0005)*4)
#define REG_GB_LL_REAL_PAGE_NUM				((GB_BA_B + 0x0006)*4)
#define REG_GB_UP_RSV_SIZE					((GB_BA_B + 0x0007)*4)
#define REG_GB_DN_RSV_SIZE					((GB_BA_B + 0x0008)*4)
#define REG_GB_TEST_CFG						((GB_BA_B + 0x0009)*4)
#define REG_GB_TEST_DATA					((GB_BA_B + 0x000A)*4)
#define REG_GB_RESET_ENA					((GB_BA_B + 0x000B)*4)

#define REG_GB_PONTX_MODE					((GB_BA_B + 0x000E)*4)
#define REG_GB_QSET_NUM						((GB_BA_B + 0x000F)*4)
#define REG_GB_REPORT_TIMERVALUE			((GB_BA_B + 0x0010)*4)
#define REG_GB_SYNCTIME						((GB_BA_B + 0x0011)*4)
#define REG_GB_LLID							((GB_BA_B + 0x0012)*4)
#define REG_MPCP_SYNCTIME					((GB_BA_B + 0x0013)*4)
#define REG_MPCP_LLID						((GB_BA_B + 0x0014)*4)
#define REG_MASTER_CFG						((GB_BA_B + 0x0016)*4)
#define REG_TFA_EN							((GB_BA_B + 0x0017)*4)
#define REG_GB_ONU_LED						((GB_BA_B + 0x0018)*4)
#define REG_SERDES_DELAY					((GB_BA_B + 0x0020)*4)
#define REG_MPCP_ONU_MACID0					((GB_BA_B + 0x0021)*4)
#define REG_MPCP_ONU_MACID1					((GB_BA_B + 0x0022)*4)
#define REG_CP_CPTM_DN_RSV_SIZE				((GB_BA_B + 0x0023)*4)
#define REG_CP_FE_RSV_SIZE					((GB_BA_B + 0x0024)*4)
#define REG_CP_LED_CTRL						((GB_BA_B + 0x0025)*4)
#define REG_CP_OAM_REGI_CTRL				((GB_BA_B + 0x0026)*4)
#define REG_LED_REGI_BLINK_CFG				((GB_BA_B + 0x0027)*4)
#define REG_LED_TRAF_BLINK_CFG				((GB_BA_B + 0x0028)*4)
#define REG_GB_UART_MODE					((GB_BA_B + 0x0030)*4)
#define REG_GB_SUM_RSV_SIZE					((GB_BA_B + 0x0031)*4)
#define REG_GB_TM_MODE                      ((GB_BA_B + 0x0032)*4)
#define REG_US_Q_OFFSET                     ((GB_BA_B + 0x0033)*4)

#define REG_JTAGTestMuxMultiplexer          ((GB_BA_B + 0X0040)*4)
#define REG_ExternalCodecReset              ((GB_BA_B + 0X0050)*4)
#define REG_MDIO_SELECT                     ((GB_BA_B + 0X0052)*4)
#define REG_DMAControlEnable                ((GB_BA_B + 0X0060)*4)
#define REG_DMAControlReadCmd0              ((GB_BA_B + 0X0061)*4)
#define REG_DMAControlReadCmd1              ((GB_BA_B + 0X0062)*4)
#define REG_DMAControlWriteCmd0             ((GB_BA_B + 0X0063)*4)
#define REG_DMAControlWriteCmd1             ((GB_BA_B + 0X0064)*4)

/*spi register*/
#define SPI_BA_B                  (OPCONN_BASE_ADDR + 0X0300)

#define REG_SPI_TX_0			((SPI_BA_B + 0x0000)*4)
#define REG_SPI_TX_1			((SPI_BA_B + 0x0001)*4)
#define REG_SPI_TX_2			((SPI_BA_B + 0x0002)*4)
#define REG_SPI_TX_3			((SPI_BA_B + 0x0003)*4)
#define REG_SPI_RX_0			((SPI_BA_B + 0x0004)*4)
#define REG_SPI_RX_1			((SPI_BA_B + 0x0005)*4)
#define REG_SPI_RX_2			((SPI_BA_B + 0x0006)*4)
#define REG_SPI_RX_3			((SPI_BA_B + 0x0007)*4)
#define REG_SPI_DIVIDE		    ((SPI_BA_B + 0x0008)*4)
#define REG_SPI_CHAR_LEN	    ((SPI_BA_B + 0x0009)*4)
#define REG_SPI_CTRL			((SPI_BA_B + 0x000a)*4)
//#define REG_SPI_SS				((SPI_BA_B + 0x000b)*4)
#define REG_SPI_START			((SPI_BA_B + 0x000c)*4)

//#define REG_SPI_INT_ENABLE		((SPI_BA_B + 0x0018)*4)
#define REG_SPI_START			((SPI_BA_B + 0x000c)*4)

/* o2 driver compatible */
#define SPI_BASE_ADDR                                           (OPCONN_BASE_ADDR + 0x300)

#define REG_SPI_TX_(n)                                          (SPI_BASE_ADDR + 0X0000 + n)*4
#define REG_SPI_RX_(n)                                          (SPI_BASE_ADDR + 0X0008 + n)*4
#define REG_SPI_FRQ_DIV                                         (SPI_BASE_ADDR + 0X0010)*4
#define REG_SPI_LEN                                             (SPI_BASE_ADDR + 0X0011)*4
#define REG_SPI_CTL                                             (SPI_BASE_ADDR + 0X0012)*4
#define REG_SPI_SS                                              (SPI_BASE_ADDR + 0X0013)*4
#define REG_SPI_BUSY                                            (SPI_BASE_ADDR + 0X0014)*4
#define REG_SPI_INT                                             (SPI_BASE_ADDR + 0X0015)*4
#define REG_SPI_CS_DELAY                                        (SPI_BASE_ADDR + 0X0016)*4
#define REG_SPI_BUF_READY                                       (SPI_BASE_ADDR + 0X0017)*4
#define REG_SPI_INT_ENABLE                                      (SPI_BASE_ADDR + 0X0018)*4
#define REG_SPI_MASTER_BOOT_SCK_RATE                            (SPI_BASE_ADDR + 0X0020)*4
#define REG_SPI_MASTER_BOOT_RD_CMD                              (SPI_BASE_ADDR + 0X0021)*4
#define REG_SPI_MASTER_BOOT_SUB_MODE                            (SPI_BASE_ADDR + 0X0022)*4
#define REG_SPI_MASTER_BOOT_DUMMY_CYL                           (SPI_BASE_ADDR + 0X0023)*4
#define REG_SPI_MASTER_BOOT_OFFSET                              (SPI_BASE_ADDR + 0X0024)*4
#define REG_SPI_SLAVE_ADDR                                      (SPI_BASE_ADDR + 0x0050)*4
#define REG_SPI_SLAVE_CTRL                                      (SPI_BASE_ADDR + 0x0051)*4
#define REG_SPI_SLAVE_DATA                                      (SPI_BASE_ADDR + 0x0052)*4
#define REG_SPI_SLAVE_SRAM_BUF_START                            (SPI_BASE_ADDR + 0x0053)*4
#define REG_SPI_SLAVE_SRAM_PAGE_SIZE                            (SPI_BASE_ADDR + 0x0054)*4


/*mdio register*/
#define MDIO_BA_B				 (OPCONN_BASE_ADDR + 0x0500)
#define MDIO_BA_E				 (OPCONN_BASE_ADDR + 0x0506)

#define REG_MDIO_DIV_FACTOR     		((MDIO_BA_B + 0)*4)
#define REG_MDIO_OP_PULSE				((MDIO_BA_B + 1)*4)
#define REG_MDIO_PHYAD  				((MDIO_BA_B + 2)*4)
#define REG_MDIO_REGAD					((MDIO_BA_B + 3)*4)
#define REG_MDIO_WRITE_DATA   			((MDIO_BA_B + 4)*4)
#define REG_MDIO_READ_DATA     			((MDIO_BA_B + 5)*4)
#define REG_MDIO_BUSY     				((MDIO_BA_B + 6)*4)

/*uart0 register*/
#define UART0_BA_B 				 	(OPCONN_BASE_ADDR + 0x600)

#define REG_UART0_REG_RH		((UART0_BA_B + 0x0000)*4)
#define REG_UART0_REG_TH		((UART0_BA_B + 0x0000)*4)
#define REG_UART0_REG_IE		((UART0_BA_B + 0x0001)*4)
#define REG_UART0_REG_IS		((UART0_BA_B + 0x0002)*4)
#define REG_UART0_REG_FC		((UART0_BA_B + 0x0002)*4)
#define REG_UART0_REG_LC		((UART0_BA_B + 0x0003)*4)
#define REG_UART0_REG_MC		((UART0_BA_B + 0x0004)*4)
#define REG_UART0_REG_LS		((UART0_BA_B + 0x0005)*4)
#define REG_UART0_REG_MS		((UART0_BA_B + 0x0006)*4)
#define REG_UART0_REG_SP		((UART0_BA_B + 0x0007)*4)
                                                   
#define REG_UART0_REG_DL1	    ((UART0_BA_B + 0x0000)*4)
#define REG_UART0_REG_DL2	    ((UART0_BA_B + 0x0001)*4)
#define REG_UART0_REG_EF		((UART0_BA_B + 0x0002)*4)

/*uart1 register*/
#define UART1_BA_B					(OPCONN_BASE_ADDR + 0x700)

#define REG_UART1_REG_RH		((UART1_BA_B + 0x0000)*4)  
#define REG_UART1_REG_TH		((UART1_BA_B + 0x0000)*4)  
#define REG_UART1_REG_IE		((UART1_BA_B + 0x0001)*4)  
#define REG_UART1_REG_IS		((UART1_BA_B + 0x0002)*4)  
#define REG_UART1_REG_FC		((UART1_BA_B + 0x0002)*4)  
#define REG_UART1_REG_LC		((UART1_BA_B + 0x0003)*4)  
#define REG_UART1_REG_MC		((UART1_BA_B + 0x0004)*4)  
#define REG_UART1_REG_LS		((UART1_BA_B + 0x0005)*4)  
#define REG_UART1_REG_MS		((UART1_BA_B + 0x0006)*4)  
#define REG_UART1_REG_SP		((UART1_BA_B + 0x0007)*4)  
                                                 
#define REG_UART1_REG_DL1	    ((UART1_BA_B + 0x0000)*4)  
#define REG_UART1_REG_DL2	    ((UART1_BA_B + 0x0001)*4)  
#define REG_UART1_REG_EF		((UART1_BA_B + 0x0002)*4)

#define UART_BASE_ADDR(n)                                        UART0_BA_B + 0x100 * n
#define REG_RECV_HOLD(n)                                         ((UART_BASE_ADDR(n) + 0X0000)*4)
#define REG_TRANS_HOLD(n)                                        ((UART_BASE_ADDR(n) + 0X0000)*4)
#define REG_INT_ENABLE(n)                                        ((UART_BASE_ADDR(n) + 0X0001)*4)
#define REG_INT_STATUS(n)                                        ((UART_BASE_ADDR(n) + 0X0002)*4)
#define REG_FIFO_CTL(n)                                          ((UART_BASE_ADDR(n) + 0X0002)*4)
#define REG_LINE_CTL(n)                                          ((UART_BASE_ADDR(n) + 0X0003)*4)
#define REG_MODEM_CTL(n)                                         ((UART_BASE_ADDR(n) + 0X0004)*4)
#define REG_LINE_STATUS(n)                                       ((UART_BASE_ADDR(n) + 0X0005)*4)
#define REG_MODEM_STATUS(n)                                      ((UART_BASE_ADDR(n) + 0X0006)*4)
#define REG_SCRATCHPAD(n)                                        ((UART_BASE_ADDR(n) + 0X0007)*4)
#define REG_DIV_0(n)                                             ((UART_BASE_ADDR(n) + 0X0000)*4)
#define REG_DIV_1(n)                                             ((UART_BASE_ADDR(n) + 0X0001)*4)
#define REG_ENHANC_FEATURE(n)                                    ((UART_BASE_ADDR(n) + 0X0002)*4)

/*I2C register*/
#define I2C_BA_B   					(OPCONN_BASE_ADDR + 0X800)

#define REG_I2C_STA					((I2C_BA_B + 0x0000)*4)     
#define REG_I2C_TO					((I2C_BA_B + 0x0000)*4)     
#define REG_I2C_DAT					((I2C_BA_B + 0x0001)*4)     
#define REG_I2C_ADR					((I2C_BA_B + 0x0002)*4)     
#define REG_I2C_CON				    ((I2C_BA_B + 0x0003)*4)     
#define REG_I2C_EN				    ((I2C_BA_B + 0x0004)*4)     
#define REG_I2C_SR					((I2C_BA_B + 0x0005)*4)   

/* gmac core base addr  */ 
#define GMAC_BA_B              	(OPCONN_BASE_ADDR + 0x0900) 

#define REG_GMAC_CFG_TRANSFER_ENA		((GMAC_BA_B + 0X0000)*4)
#define REG_GMAC_CFG_PAUSE_ENA			((GMAC_BA_B + 0X0001)*4)
#define REG_GMAC_CFG_MAXFRAMLEN			((GMAC_BA_B + 0X0002)*4)
#define REG_GMAC_CFG_MINIFG					((GMAC_BA_B + 0X0004)*4)
#define REG_GMAC_CFG_LENMENA				((GMAC_BA_B + 0X0005)*4)
#define REG_GMAC_CFG_IPG							((GMAC_BA_B + 0X0006)*4)
#define REG_GMAC_CFG_PRENUM					((GMAC_BA_B + 0X0007)*4)
#define REG_GMAC_CFG_PADCRC					((GMAC_BA_B + 0X0008)*4)
#define REG_GMAC_CFG_SPEED						((GMAC_BA_B + 0X0009)*4)
#define REG_GMAC_MODE                               ((GMAC_BA_B + 0X000C)*4)
#define REG_GMAC_LP_BACK							((GMAC_BA_B + 0X0018)*4)
#define REG_GMAC_TxetherStatsPktsCount									((GMAC_BA_B + 0X0020)*4)
#define REG_GMAC_TxetherStatsBroadcastPktsCount					((GMAC_BA_B + 0X0021)*4)
#define REG_GMAC_TxetherStatsMulticastPktsCount					((GMAC_BA_B + 0X0022)*4)
#define REG_GMAC_TxaPAUSEMACCtrlFramesCount						((GMAC_BA_B + 0X0023)*4)
#define REG_GMAC_TxaMACControlFramesCount							((GMAC_BA_B + 0X0024)*4)
#define REG_GMAC_TxetherStatsPkts64OctetsCount					((GMAC_BA_B + 0X0025)*4)
#define REG_GMAC_TxetherStatsPkts65to127OctetsCount			((GMAC_BA_B + 0X0026)*4)
#define REG_GMAC_TxetherStatsPkts128to255OctetsCount		((GMAC_BA_B + 0X0027)*4)
#define REG_GMAC_TxtherStatsPkts256to511OctetsCount			((GMAC_BA_B + 0X0028)*4)
#define REG_GMAC_TxetherStatsPkts512to1023OctetsCount		((GMAC_BA_B + 0X0029)*4)
#define REG_GMAC_TxetherStatsPkts1024to1518OctetsCount 	((GMAC_BA_B + 0X002a)*4)
#define REG_GMAC_TxetherStatsOctetsCount								((GMAC_BA_B + 0X002b)*4)
#define REG_GMAC_TxetherStatsPkts1519toMAXOctetsCount		((GMAC_BA_B + 0X002C)*4)
#define REG_GMAC_TxL2MulticastPktsCount		                ((GMAC_BA_B + 0X002E)*4)
#define REG_GMAC_RxetherStatsGoodPktsCount							((GMAC_BA_B + 0X0040)*4)
#define REG_GMAC_RxetherStatsDropEventsCount						((GMAC_BA_B + 0X0041)*4)
#define REG_GMAC_RxetherStatsCRCAlignErrorsCount					((GMAC_BA_B + 0X0042)*4)
#define REG_GMAC_RxetherStatsLenmisCount								((GMAC_BA_B + 0X0043)*4)
#define REG_GMAC_RxetherStatsShortCount								((GMAC_BA_B + 0X0044)*4)
#define REG_GMAC_RxetherStatsJabbersCount							((GMAC_BA_B + 0X0045)*4)
#define REG_GMAC_RxetherStatsPktsCount									((GMAC_BA_B + 0X0046)*4)
#define REG_GMAC_RxetherStatsBroadcastPktsCount					((GMAC_BA_B + 0X0047)*4)
#define REG_GMAC_RxetherStatsMulticastPktsCount					((GMAC_BA_B + 0X0048)*4)
#define REG_GMAC_RxaPAUSEMACCtrlFramesCount					((GMAC_BA_B + 0X0049)*4)
#define REG_GMAC_RxaUnsupportedOpcodesCount						((GMAC_BA_B + 0X004a)*4)
#define REG_GMAC_RxaMACControlFramesCount							((GMAC_BA_B + 0X004b)*4)
#define REG_GMAC_RxetherStatsUndersizePktsCount					((GMAC_BA_B + 0X004c)*4)
#define REG_GMAC_RxetherStatsOversizePktsCount					((GMAC_BA_B + 0X004d)*4)
#define REG_GMAC_RxetherStatsPkts64OctetsCount					((GMAC_BA_B + 0X004e)*4)
#define REG_GMAC_RxtherStatsPkts65to127OctetsCount			((GMAC_BA_B + 0X004f)*4)
#define REG_GMAC_RxetherStatsPkts128to255OctetsCount		((GMAC_BA_B + 0X0050)*4)
#define REG_GMAC_RxtherStatsPkts256to511OctetsCount			((GMAC_BA_B + 0X0051)*4)
#define REG_GMAC_RxetherStatsPkts512to1023OctetsCount		((GMAC_BA_B + 0X0052)*4)
#define REG_GMAC_RxetherStatsPkts1024to1518OctetsCount	((GMAC_BA_B + 0X0053)*4)
#define REG_GMAC_RxetherStatsPkts1518toMAXOctetsCount		((GMAC_BA_B + 0X0054)*4)
#define REG_GMAC_RxetherStatsOctetsCount								((GMAC_BA_B + 0X0055)*4)
#define REG_GMAC_RX_ETHL2MULTICAST_PKT_CNT						((GMAC_BA_B + 0X0059)*4)
#define REG_GMAC_RxetherRX_ERdropCount								((GMAC_BA_B + 0x0060)*4)
#define REG_GMAC_RxetherIFGdropCount									((GMAC_BA_B + 0X0061)*4)
#define REG_GMAC_RGMII_RXDLL_CFG                                    	((GMAC_BA_B + 0X0093)*4)
#define REG_GMAC_RGMII_TXDLL_CFG                                        ((GMAC_BA_B + 0X0096)*4)
#define REG_GMAC_RGMII_SOFT_CFG                                         ((GMAC_BA_B + 0X0091)*4)

/*pon mac config register*/
#define PMAC_BA_B              (OPCONN_BASE_ADDR + 0X0A00) 
#define PMAC_BA_E              (OPCONN_BASE_ADDR + 0x0A22) 

#define REG_PMAC_CFG_TRANSFER_ENA	((PMAC_BA_B + 0X0000)*4)
#define REG_PMAC_CFG_PAUSE_ENA		((PMAC_BA_B + 0X0001)*4)
#define REG_PMAC_CFG_MAXFRAMLEN		((PMAC_BA_B + 0X0002)*4)
#define REG_PMAC_CFG_MINIFG				((PMAC_BA_B + 0X0004)*4)
#define REG_PMAC_CFG_LENMENA			((PMAC_BA_B + 0X0005)*4)
#define REG_PMAC_CFG_IPG						((PMAC_BA_B + 0X0006)*4)
#define REG_PMAC_CFG_PRENUM				((PMAC_BA_B + 0X0007)*4)
#define REG_PMAC_CFG_PADCRC				((PMAC_BA_B + 0X0008)*4)
#define REG_PMAC_TBI_CFG0					((PMAC_BA_B + 0X0010)*4)

#define REG_PMAC_TBI_CFG5					((PMAC_BA_B + 0X0015)*4)
#define REG_PMAC_TBI_INTF_CFG				((PMAC_BA_B + 0X0016)*4)
#define REG_PMAC_LP_BACK						((PMAC_BA_B + 0X0018)*4)
#define REG_PMAC_LASER_ON_CFG			((PMAC_BA_B + 0X0019)*4)
#define REG_PMAC_INT								((PMAC_BA_B + 0X001A)*4)
#define REG_PMAC_INT_EN						((PMAC_BA_B + 0X001B)*4)
#define REG_PMAC_EBR                        ((PMAC_BA_B + 0X001C)*4)
#define REG_PMAC_CBR                        ((PMAC_BA_B + 0X001D)*4)    

#define REG_PMAC_TxetherStatsPktsCount									((PMAC_BA_B + 0X0020)*4)
#define REG_PMAC_TxetherStatsBroadcastPktsCount					((PMAC_BA_B + 0X0021)*4)
#define REG_PMAC_TxetherStatsMulticastPktsCount					((PMAC_BA_B + 0X0022)*4)
#define REG_PMAC_TxaPAUSEMACCtrlFramesCount						((PMAC_BA_B + 0X0023)*4)
#define REG_PMAC_TxaMACControlFramesCount							((PMAC_BA_B + 0X0024)*4)
#define REG_PMAC_TxetherStatsPkts64OctetsCount					((PMAC_BA_B + 0X0025)*4)
#define REG_PMAC_TxetherStatsPkts65to127OctetsCount			((PMAC_BA_B + 0X0026)*4)
#define REG_PMAC_TxetherStatsPkts128to255OctetsCount		((PMAC_BA_B + 0X0027)*4)
#define REG_PMAC_TxtherStatsPkts256to511OctetsCount			((PMAC_BA_B + 0X0028)*4)
#define REG_PMAC_TxetherStatsPkts512to1023OctetsCount		((PMAC_BA_B + 0X0029)*4)
#define REG_PMAC_TxetherStatsPkts1024to1518OctetsCount	((PMAC_BA_B + 0X002a)*4)
#define REG_PMAC_TxetherStatsOctetsCount								((PMAC_BA_B + 0X002b)*4)
#define REG_PMAC_TxetherStatsPkts1519toMAXOctetsCount		((PMAC_BA_B + 0X002C)*4)
#define REG_PMAC_TX_OAM_CNT													((PMAC_BA_B + 0X002d)*4)

#define REG_PMAC_RxetherStatsGoodPktsCount							((PMAC_BA_B + 0X0040)*4)
#define REG_PMAC_RxetherStatsDropEventsCount						((PMAC_BA_B + 0X0041)*4)
#define REG_PMAC_RxetherStatsCRCAlignErrorsCount					((PMAC_BA_B + 0X0042)*4)
#define REG_PMAC_RxetherStatsLenmisCount								((PMAC_BA_B + 0X0043)*4)
#define REG_PMAC_RxetherStatsShortCount								((PMAC_BA_B + 0X0044)*4)
#define REG_PMAC_RxetherStatsJabbersCount							((PMAC_BA_B + 0X0045)*4)
#define REG_PMAC_RxetherStatsPktsCount									((PMAC_BA_B + 0X0046)*4)
#define REG_PMAC_RxetherStatsBroadcastPktsCount					((PMAC_BA_B + 0X0047)*4)
#define REG_PMAC_RxetherStatsMulticastPktsCount					((PMAC_BA_B + 0X0048)*4)
#define REG_PMAC_RxaPAUSEMACCtrlFramesCount						((PMAC_BA_B + 0X0049)*4)
#define REG_PMAC_RxaUnsupportedOpcodesCount						((PMAC_BA_B + 0X004a)*4)
#define REG_PMAC_RxaMACControlFramesCount							((PMAC_BA_B + 0X004b)*4)
#define REG_PMAC_RxetherStatsUndersizePktsCount					((PMAC_BA_B + 0X004c)*4)
#define REG_PMAC_RxetherStatsOversizePktsCount					((PMAC_BA_B + 0X004d)*4)
#define REG_PMAC_RxetherStatsPkts64OctetsCount					((PMAC_BA_B + 0X004e)*4)
#define REG_PMAC_RxtherStatsPkts65to127OctetsCount			((PMAC_BA_B + 0X004f)*4)
#define REG_PMAC_RxetherStatsPkts128to255OctetsCount		((PMAC_BA_B + 0X0050)*4)
#define REG_PMAC_RxtherStatsPkts256to511OctetsCount			((PMAC_BA_B + 0X0051)*4)
#define REG_PMAC_RxetherStatsPkts512to1023OctetsCount		((PMAC_BA_B + 0X0052)*4)
#define REG_PMAC_RxetherStatsPkts1024to1518OctetsCount	((PMAC_BA_B + 0X0053)*4)
#define REG_PMAC_RxetherStatsPkts1518toMAXOctetsCount		((PMAC_BA_B + 0X0054)*4)
#define REG_PMAC_RxetherStatsOctetsCount								((PMAC_BA_B + 0X0055)*4)
#define REG_PMAC_FEC_CORRECTED_PKTCNT								((PMAC_BA_B + 0X0056)*4)
#define REG_PMAC_FEC_UNCORRECTED_PKTCNT							((PMAC_BA_B + 0X0057)*4)
#define REG_PMAC_FEC_NOERR_PKTCNT										((PMAC_BA_B + 0X0058)*4)
#define REG_PMAC_RX_ETHL2MULTICAST_PKT_CNT						((PMAC_BA_B + 0X0059)*4)
#define REG_PMAC_RX_OAM_CNT													((PMAC_BA_B + 0X005a)*4)
#define REG_PMAC_RxetherRX_ERdropCount								((PMAC_BA_B + 0X0060)*4)
#define REG_RxetherIFGdropCount												((PMAC_BA_B + 0X0061)*4)		
#define REG_PMAC_SECU_CFG														((PMAC_BA_B + 0X0080)*4)
#define REG_PMAC_SECU_GEN														((PMAC_BA_B + 0X0081)*4)
#define REG_PMAC_SECU_KEY0_0													((PMAC_BA_B + 0X0082)*4)
#define REG_PMAC_SECU_KEY0_1													((PMAC_BA_B + 0X0083)*4)
#define REG_PMAC_SECU_KEY0_2													((PMAC_BA_B + 0X0084)*4)
#define REG_PMAC_SECU_KEY0_3													((PMAC_BA_B + 0X0085)*4)

#define REG_PMAC_SECU_KEY1_0													((PMAC_BA_B + 0X0086)*4)
#define REG_PMAC_SECU_KEY1_1													((PMAC_BA_B + 0X0087)*4)
#define REG_PMAC_SECU_KEY1_2													((PMAC_BA_B + 0X0088)*4)
#define REG_PMAC_SECU_KEY1_3													((PMAC_BA_B + 0X0089)*4)

#define REG_PMAC_SECU_SCB_KEY0_0											((PMAC_BA_B + 0X0090)*4)
#define REG_PMAC_SECU_SCB_KEY0_1											((PMAC_BA_B + 0X0091)*4)
#define REG_PMAC_SECU_SCB_KEY0_2											((PMAC_BA_B + 0X0092)*4)
#define REG_PMAC_SECU_SCB_KEY0_3											((PMAC_BA_B + 0X0093)*4)

#define REG_PMAC_SECU_SCB_KEY1_0											((PMAC_BA_B + 0X0094)*4)
#define REG_PMAC_SECU_SCB_KEY1_1											((PMAC_BA_B + 0X0095)*4)
#define REG_PMAC_SECU_SCB_KEY1_2											((PMAC_BA_B + 0X0096)*4)
#define REG_PMAC_SECU_SCB_KEY1_3											((PMAC_BA_B + 0X0097)*4)

#define REG_PMAC_SECU_LEN_DROP_CNT									((PMAC_BA_B + 0X00A0)*4)

#define REG_PMAC_SERDES_COMMON											((PMAC_BA_B + 0X00B0)*4)
#define REG_PMAC_SERDES_TX														((PMAC_BA_B + 0X00B1)*4)			
#define REG_PMAC_SERDES_RX														((PMAC_BA_B + 0X00B2)*4)	
#define REG_PMAC_SERDES_MATCH												((PMAC_BA_B + 0X00B3)*4)
#define REG_PMAC_SERDES_STATUS												((PMAC_BA_B + 0X00B4)*4)
#define REG_PMAC_PRBS_CFG														((PMAC_BA_B + 0X00B9)*4)
#define REG_PMAC_PRBS_STATUS													((PMAC_BA_B + 0X00Ba)*4)
#define REG_RScrc8errCount															((PMAC_BA_B + 0X00C0)*4)	
#define REG_RSlliderrCount															((PMAC_BA_B + 0X00C1)*4)
#define REG_D256_SFIFO_FULL														((PMAC_BA_B + 0X00C2)*4)
#define REG_D256_SFIFO_EMPTY															((PMAC_BA_B + 0X00C3)*4)		

/*watch dog register*/
#define WATCHDOG_BA_B  			(OPCONN_BASE_ADDR + 0x0b00)
#define REG_WATCHDOG_CFG		((WATCHDOG_BA_B + 0X0000)*4)      
#define REG_WATCHDOG_FEED		((WATCHDOG_BA_B + 0X0001)*4)      
#define REG_WATCHDOG_MAX		((WATCHDOG_BA_B + 0X0002)*4)      
#define REG_WATCHDOG_VALUE	((WATCHDOG_BA_B + 0X0003)*4)   

/*gpio register*/
#define GPIO_BA_B							(OPCONN_BASE_ADDR + 0XB00)
#define REG_GPIO_VALUE					((GPIO_BA_B + 0x0020)*4)
#define REG_GPIO_DIRECTION			((GPIO_BA_B + 0x0021)*4)
#define REG_GPIO_SET						((GPIO_BA_B + 0x0022)*4)
#define REG_GPIO_CLEAR					((GPIO_BA_B + 0x0023)*4)
#define REG_GPIO_TRIGGER_MODE	((GPIO_BA_B + 0x0026)*4)
#define REG_GPIO_LEVEL_MODE		((GPIO_BA_B + 0x0027)*4)
#define REG_GPIO_EDGE_MODE			((GPIO_BA_B + 0x0028)*4)
#define REG_GPIO_MUX						((GPIO_BA_B + 0X0029)*4)

/*host register*/
#define HOST_BA_B    (OPCONN_BASE_ADDR + 0x0c00)

#define REG_CS0_CFG	((IS_ONU_OPCONN)?((HOST_BA_B+0x0000)*4):((HOST_BA_B+0x0000)*4))
#define REG_CS1_CFG	((IS_ONU_OPCONN)?((HOST_BA_B+0x0001)*4):((HOST_BA_B+0x0002)*4))
#define REG_CS2_CFG	((IS_ONU_OPCONN)?((HOST_BA_B+0x0002)*4):((HOST_BA_B+0x0004)*4))

/* only work in RESPIN == */
#define REG_CS0_CFG1	((HOST_BA_B+0x0001)*4))
#define REG_CS1_CFG1	((HOST_BA_B+0x0003)*4))
#define REG_CS2_CFG1	((HOST_BA_B+0x0005)*4))
/* == */

#define REG_CS0_ACCESS_SPACE0	((IS_ONU_OPCONN)?((HOST_BA_B+0x0010)*4):((HOST_BA_B+0x0008)*4))
#define REG_CS1_ACCESS_SPACE0	((IS_ONU_OPCONN)?((HOST_BA_B+0x0011)*4):((HOST_BA_B+0x0009)*4))
#define REG_CS2_ACCESS_SPACE0	((IS_ONU_OPCONN)?((HOST_BA_B+0x0012)*4):((HOST_BA_B+0x000a)*4))

#define REG_DRAM_ACCESS_SPACE	((IS_ONU_OPCONN)?((HOST_BA_B+0x0010)*4):((HOST_BA_B+0x0024)*4))
#define REG_INTERNAL_REG_ACCESS_SPACE	((IS_ONU_OPCONN)?((HOST_BA_B+0x0011)*4):((HOST_BA_B+0x0023)*4))
#define REG_CS0_ACCESS_SPACE1	((IS_ONU_OPCONN)?((HOST_BA_B+0x0012)*4):((HOST_BA_B+0x0018)*4))
#define REG_CS1_ACCESS_SPACE1	((IS_ONU_OPCONN)?((HOST_BA_B+0x0013)*4):((HOST_BA_B+0x0019)*4))
#define REG_CS2_ACCESS_SPACE1	((IS_ONU_OPCONN)?((HOST_BA_B+0x0014)*4):((HOST_BA_B+0x001a)*4))

/*fe register*/
#define FE_BA_B 		(OPCONN_BASE_ADDR + 0x0d00)

#define REG_FE_TRANSFER_ENA	((FE_BA_B+0x0000)*4)        
#define REG_FE_PAUSE_ENA			((FE_BA_B+0x0001)*4)    
#define REG_FE_MAXFRAMLEN		((FE_BA_B+0x0002)*4)    
#define REG_FE_MINIFG					((FE_BA_B+0x0003)*4)            
#define REG_FE_LENMENA				((FE_BA_B+0x0004)*4)            
#define REG_FE_IPG						((FE_BA_B+0x0005)*4)    
#define REG_FE_PRENUM				((FE_BA_B+0x0006)*4)            
#define REG_FE_PADCRC				((FE_BA_B+0x0007)*4)            
#define REG_FE_LP_BACK				((FE_BA_B+0x0008)*4)            
#define REG_FE_TX_CNT					((FE_BA_B+0x0009)*4)            
#define REG_FE_RX_CNT				((FE_BA_B+0x000A)*4)            
#define REG_FE_DROP_CNT			((FE_BA_B+0x000B)*4)    
#define REG_FE_MACID_0				((FE_BA_B+0x000C)*4)            
#define REG_FE_MACID_1				((FE_BA_B+0x000D)*4)            
                                                       
/* interrupt */
#define INT_BA_B 					(OPCONN_BASE_ADDR + 0X0B80)	
#define INTC_PENDING			((INT_BA_B+0x00)*4)
#define INTC_PENDING_EN		((INT_BA_B+0x02)*4)
#define INTC_IRQ_CFG			((INT_BA_B+0x06)*4)
#define INTC_LEVEL0				((INT_BA_B+0x07)*4)
#define INTC_LEVEL1				((INT_BA_B+0x08)*4)
#define INTC_LEVEL2				((INT_BA_B+0x09)*4)
#define INTC_LEVEL3				((INT_BA_B+0x0a)*4)
#define INTC_LEVEL4				((INT_BA_B+0x0b)*4)
#define INTC_LEVEL5				((INT_BA_B+0x0c)*4)
#define INTC_LEVEL6				((INT_BA_B+0x0d)*4)
#define INTC_LEVEL7				((INT_BA_B+0x0e)*4)




/* report register offset*/
#define REPORT_BA_B			 (OPCONN_BASE_ADDR + 0X1000)


#define REG_RPT_MODE			((REPORT_BA_B + 0X0000)*4)               
#define REG_RPT_Q0_THR0	((REPORT_BA_B + 0X0008)*4)       
#define REG_RPT_Q1_THR0	((REPORT_BA_B + 0X0009)*4)       
#define REG_RPT_Q2_THR0	((REPORT_BA_B + 0X000a)*4)       
#define REG_RPT_Q3_THR0	((REPORT_BA_B + 0X000b)*4)       
#define REG_RPT_Q4_THR0	((REPORT_BA_B + 0X000c)*4)       
#define REG_RPT_Q5_THR0	((REPORT_BA_B + 0X000d)*4)       
#define REG_RPT_Q6_THR0	((REPORT_BA_B + 0X000e)*4)       
#define REG_RPT_Q7_THR0	((REPORT_BA_B + 0X000f)*4)       
#define REG_RPT_Q0_THR1	((REPORT_BA_B + 0X0010)*4)       
#define REG_RPT_Q1_THR1	((REPORT_BA_B + 0X0011)*4)       
#define REG_RPT_Q2_THR1	((REPORT_BA_B + 0X0012)*4)       
#define REG_RPT_Q3_THR1	((REPORT_BA_B + 0X0013)*4)       
#define REG_RPT_Q4_THR1	((REPORT_BA_B + 0X0014)*4)       
#define REG_RPT_Q5_THR1	((REPORT_BA_B + 0X0015)*4)       
#define REG_RPT_Q6_THR1	((REPORT_BA_B + 0X0016)*4)       
#define REG_RPT_Q7_THR1	((REPORT_BA_B + 0X0017)*4)    
#define REG_DDR_BASE_ADDRESS	((REPORT_BA_B + 0X0018)*4)
#define REG_RPT_ERR_DEFAULT0	((REPORT_BA_B + 0X0019)*4)
#define REG_RPT_ERR_DEFAULT1	((REPORT_BA_B + 0X001A)*4)



/* tm register */
#define TM_BA_B						(OPCONN_BASE_ADDR + 0X2000)

#define REG_TM_CTRL								((TM_BA_B + 0x0000)*4)                            
#define REG_TMUS_SHAPER_EN				((TM_BA_B + 0x0001)*4)                            
#define REG_TMDS_SHAPER_EN				((TM_BA_B + 0x0002)*4)                            
#define REG_TMUS_INT							((TM_BA_B + 0x0003)*4)                            
#define REG_TMDS_INT							((TM_BA_B + 0x0004)*4)                            
#define REG_TMUS_INT_EN						((TM_BA_B + 0x0005)*4)                    
#define REG_TMDS_INT_EN						((TM_BA_B + 0x0006)*4)   
#define REG_TMUS_SHAPER_OFFSET		((TM_BA_B + 0x0007)*4)
#define REG_TMDS_SHAPER_OFFSET		((TM_BA_B + 0x0008)*4)
#define  REG_TMUS_PRIORITY0	((TM_BA_B + 0x0010)*4)              	              
#define  REG_TMUS_PRIORITY1	((TM_BA_B + 0x0011)*4)                            
#define  REG_TMUS_PRIORITY2	((TM_BA_B + 0x0012)*4)                            
#define  REG_TMUS_PRIORITY3	((TM_BA_B + 0x0013)*4)                            
#define  REG_TMUS_PRIORITY4	((TM_BA_B + 0x0014)*4)                            
#define  REG_TMUS_PRIORITY5	((TM_BA_B + 0x0015)*4)                            
#define  REG_TMUS_PRIORITY6	((TM_BA_B + 0x0016)*4)                            
#define  REG_TMUS_PRIORITY7	((TM_BA_B + 0x0017)*4)                            
#define  REG_TMUS_WEIGHT0	((TM_BA_B + 0x0018)*4)                            
#define  REG_TMUS_WEIGHT1	((TM_BA_B + 0x0019)*4)                            
#define  REG_TMUS_WEIGHT2	((TM_BA_B + 0x001a)*4)                            
#define  REG_TMUS_WEIGHT3	((TM_BA_B + 0x001b)*4)                            
#define  REG_TMUS_WEIGHT4	((TM_BA_B + 0x001c)*4)                            
#define  REG_TMUS_WEIGHT5	((TM_BA_B + 0x001d)*4)                            
#define  REG_TMUS_WEIGHT6	((TM_BA_B + 0x001e)*4)                            
#define  REG_TMUS_WEIGHT7	((TM_BA_B + 0x001f)*4)                            
#define  REG_TMDS_PRIORITY0	((TM_BA_B + 0x0020)*4)                            
#define  REG_TMDS_PRIORITY1	((TM_BA_B + 0x0021)*4)                            
#define  REG_TMDS_PRIORITY2	((TM_BA_B + 0x0022)*4)                            
#define  REG_TMDS_PRIORITY3	((TM_BA_B + 0x0023)*4) 
#define  REG_TMDS_PRIORITY4	((TM_BA_B + 0x0024)*4)                            
#define  REG_TMDS_PRIORITY5	((TM_BA_B + 0x0025)*4)                            
#define  REG_TMDS_PRIORITY6	((TM_BA_B + 0x0026)*4)                            
#define  REG_TMDS_PRIORITY7	((TM_BA_B + 0x0027)*4)     
#define  REG_TMDS_WEIGHT0	((TM_BA_B + 0x0028)*4)                            
#define  REG_TMDS_WEIGHT1	((TM_BA_B + 0x0029)*4)                            
#define  REG_TMDS_WEIGHT2	((TM_BA_B + 0x002a)*4)                            
#define  REG_TMDS_WEIGHT3	((TM_BA_B + 0x002b)*4)    
#define  REG_TMDS_WEIGHT4	((TM_BA_B + 0x002c)*4)                            
#define  REG_TMDS_WEIGHT5	((TM_BA_B + 0x002d)*4)                            
#define  REG_TMDS_WEIGHT6	((TM_BA_B + 0x002e)*4)                            
#define  REG_TMDS_WEIGHT7	((TM_BA_B + 0x002f)*4)   

#define  REG_TMUS_Q0_MAX_CELL_NUM  ((TM_BA_B + 0x0050)*4) 
#define  REG_TMUS_Q1_MAX_CELL_NUM  ((TM_BA_B + 0x0051)*4) 
#define  REG_TMUS_Q2_MAX_CELL_NUM  ((TM_BA_B + 0x0052)*4) 
#define  REG_TMUS_Q3_MAX_CELL_NUM  ((TM_BA_B + 0x0053)*4)
#define  REG_TMUS_Q4_MAX_CELL_NUM  ((TM_BA_B + 0x0054)*4) 
#define  REG_TMUS_Q5_MAX_CELL_NUM  ((TM_BA_B + 0x0055)*4) 
#define  REG_TMUS_Q6_MAX_CELL_NUM  ((TM_BA_B + 0x0056)*4) 
#define  REG_TMUS_Q7_MAX_CELL_NUM  ((TM_BA_B + 0x0057)*4)
#define  REG_TMUS_Q0_MAX_PKT_NUM   ((TM_BA_B + 0x0058)*4) 
#define  REG_TMUS_Q1_MAX_PKT_NUM   ((TM_BA_B + 0x0059)*4) 
#define  REG_TMUS_Q2_MAX_PKT_NUM   ((TM_BA_B + 0x005A)*4) 
#define  REG_TMUS_Q3_MAX_PKT_NUM   ((TM_BA_B + 0x005B)*4)
#define  REG_TMUS_Q4_MAX_PKT_NUM   ((TM_BA_B + 0x005C)*4) 
#define  REG_TMUS_Q5_MAX_PKT_NUM   ((TM_BA_B + 0x005D)*4) 
#define  REG_TMUS_Q6_MAX_PKT_NUM   ((TM_BA_B + 0x005E)*4) 
#define  REG_TMUS_Q7_MAX_PKT_NUM   ((TM_BA_B + 0x005F)*4)

#define  REG_TMUS_CELL_INQ0_CNT    ((TM_BA_B + 0x0090)*4) 
#define  REG_TMUS_CELL_INQ1_CNT    ((TM_BA_B + 0x0091)*4) 
#define  REG_TMUS_CELL_INQ2_CNT    ((TM_BA_B + 0x0092)*4) 
#define  REG_TMUS_CELL_INQ3_CNT    ((TM_BA_B + 0x0093)*4)
#define  REG_TMUS_CELL_INQ4_CNT    ((TM_BA_B + 0x0094)*4) 
#define  REG_TMUS_CELL_INQ5_CNT    ((TM_BA_B + 0x0095)*4) 
#define  REG_TMUS_CELL_INQ6_CNT    ((TM_BA_B + 0x0096)*4) 
#define  REG_TMUS_CELL_INQ7_CNT    ((TM_BA_B + 0x0097)*4)
#define  REG_TMUS_PKT_INQ0_CNT     ((TM_BA_B + 0x0098)*4) 
#define  REG_TMUS_PKT_INQ1_CNT     ((TM_BA_B + 0x0099)*4)
#define  REG_TMUS_PKT_INQ2_CNT     ((TM_BA_B + 0x009A)*4)
#define  REG_TMUS_PKT_INQ3_CNT     ((TM_BA_B + 0x009B)*4)
#define  REG_TMUS_PKT_INQ4_CNT     ((TM_BA_B + 0x009C)*4)
#define  REG_TMUS_PKT_INQ5_CNT     ((TM_BA_B + 0x009D)*4)
#define  REG_TMUS_PKT_INQ6_CNT     ((TM_BA_B + 0x009E)*4)
#define  REG_TMUS_PKT_INQ7_CNT     ((TM_BA_B + 0x009F)*4)

#define REG_TMUS_QID_ICOS0	((TM_BA_B + 0x0070)*4)                            
#define REG_TMUS_QID_ICOS1	((TM_BA_B + 0x0071)*4)                            
#define REG_TMUS_QID_ICOS2	((TM_BA_B + 0x0072)*4)                            
#define REG_TMUS_QID_ICOS3	((TM_BA_B + 0x0073)*4)                            
#define REG_TMUS_QID_ICOS4	((TM_BA_B + 0x0074)*4)                            
#define REG_TMUS_QID_ICOS5	((TM_BA_B + 0x0075)*4)                            
#define REG_TMUS_QID_ICOS6	((TM_BA_B + 0x0076)*4)                            
#define REG_TMUS_QID_ICOS7	((TM_BA_B + 0x0077)*4)                            
#define REG_TMDS_QID_ICOS0	((TM_BA_B + 0x0078)*4)                            
#define REG_TMDS_QID_ICOS1	((TM_BA_B + 0x0079)*4)                            
#define REG_TMDS_QID_ICOS2	((TM_BA_B + 0x007a)*4)                            
#define REG_TMDS_QID_ICOS3	((TM_BA_B + 0x007b)*4)                            
#define REG_TMDS_QID_ICOS4	((TM_BA_B + 0x007c)*4)                            
#define REG_TMDS_QID_ICOS5	((TM_BA_B + 0x007d)*4)                            
#define REG_TMDS_QID_ICOS6	((TM_BA_B + 0x007e)*4)                            
#define REG_TMDS_QID_ICOS7	((TM_BA_B + 0x007f)*4)   

#define REG_TMUS_CLKNUM_PERIOD		((TM_BA_B + 0x0080)*4)                            
#define REG_TMDS_CLKNUM_PERIOD		((TM_BA_B + 0x0081)*4)                            

#define REG_TMUS_STEP_CNT				((TM_BA_B + 0x0083)*4)                    
#define REG_TMDS_STEP_CNT				((TM_BA_B + 0x0084)*4)                    

#define REG_TMUS_WRED_DROP  				((TM_BA_B + 0x00ac)*4)
#define REG_TMUS_ENQ_PKT_CNT 				((TM_BA_B + 0x00ad)*4)
#define REG_TMDS_WRED_DROP				((TM_BA_B + 0x00ae)*4)
#define REG_TMDS_ENQ_PKT_CNT 				((TM_BA_B + 0x00af)*4)

#define REG_TMUS_RXED_PKT_CNT			((TM_BA_B + 0x00b0)*4)                            
#define REG_TMUS_CMDDRP_PKT_CNT		((TM_BA_B + 0x00b1)*4)                    
#define REG_TMUS_QFULLDRP_PKT_CNT	((TM_BA_B + 0x00b2)*4)                    
#define REG_TMUS_TXED_PKT_CNT			((TM_BA_B + 0x00b3)*4)                            
#define REG_TMDS_RXED_PKT_CNT			((TM_BA_B + 0x00b4)*4)                            
#define REG_TMDS_CMDDRP_PKT_CNT 		((TM_BA_B + 0x00b5)*4)                    
#define REG_TMDS_QFULLDRP_PKT_CNT	((TM_BA_B + 0x00b6)*4)                    
#define REG_TMDS_TXED_PKT_CNT			((TM_BA_B + 0x00b7)*4)      

#define REG_TMUS_MEM_INDIR_ACC_CTRL_SHPBD 	((TM_BA_B + 0x00b8)*4)	
#define REG_TMUS_SHADOW_SET_SHPBD				    	((TM_BA_B + 0x00b9)*4)	
#define REG_TMDS_MEM_INDIR_ACC_CTRL_SHPBD  	((TM_BA_B + 0x00bA)*4)
#define REG_TMDS_SHADOW_SET_SHPBD					((TM_BA_B + 0x00bB)*4)	
#define REG_TM_MEM_INDIR_ACC_CTRL_QSTAB		((TM_BA_B + 0x00bC)*4)
#define REG_TM_SHADOW_SET_QSTAB						((TM_BA_B + 0x00bD)*4)
#define REG_TM_SHADOW_QUEUE_INFO					((TM_BA_B + 0x00bE)*4)

#define REG_TMUS_MEM_INDIR_ACC_CTRL_WRED 	((TM_BA_B + 0x00c0)*4)
#define REG_TMUS_SHADOW_SET_WRED_H				((TM_BA_B + 0x00c1)*4)
#define REG_TMUS_SHADOW_SET_WRED_L				((TM_BA_B + 0x00c2)*4)
#define REG_TMDS_MEM_INDIR_ACC_CTRL_WRED	((TM_BA_B + 0x00c3)*4)
#define REG_TMDS_SHADOW_SET_WRED_H				((TM_BA_B + 0x00c4)*4)
#define REG_TMDS_SHADOW_SET_WRED_L				((TM_BA_B + 0x00c5)*4)
#define REG_TMUS_WRED_CTRL									((TM_BA_B + 0x00c6)*4)	
#define REG_TMDS_WRED_CTRL									((TM_BA_B + 0x00c7)*4)

#define REG_TM_RPT_CLEAR            ((TM_BA_B + 0x00c8)*4)
#define REG_TM_RPT_DELTA            ((TM_BA_B + 0x00c9)*4)
#define REG_TM_PRE_SCHEDULE_OFFSET  ((TM_BA_B + 0x00cA)*4)
#define REG_TM_RPT_LENGTH_OF_Q0     ((TM_BA_B + 0x00E0)*4)
#define REG_TM_RPT_LENGTH_OF_Q1     ((TM_BA_B + 0x00E1)*4)
#define REG_TM_RPT_LENGTH_OF_Q2     ((TM_BA_B + 0x00E2)*4)
#define REG_TM_RPT_LENGTH_OF_Q3     ((TM_BA_B + 0x00E3)*4)
#define REG_TM_RPT_LENGTH_OF_Q4     ((TM_BA_B + 0x00E4)*4)
#define REG_TM_RPT_LENGTH_OF_Q5     ((TM_BA_B + 0x00E5)*4)
#define REG_TM_RPT_LENGTH_OF_Q6     ((TM_BA_B + 0x00E6)*4)
#define REG_TM_RPT_LENGTH_OF_Q7     ((TM_BA_B + 0x00E7)*4)

#define REG_TM_TOTAL_PKTS_INC_MPCP_IN_FIFO_Q0 ((TM_BA_B + 0x00F0)*4)
#define REG_TM_TOTAL_PKTS_INC_MPCP_IN_FIFO_Q1 ((TM_BA_B + 0x00F1)*4)
#define REG_TM_TOTAL_PKTS_INC_MPCP_IN_FIFO_Q2 ((TM_BA_B + 0x00F2)*4)
#define REG_TM_TOTAL_PKTS_INC_MPCP_IN_FIFO_Q3 ((TM_BA_B + 0x00F3)*4)
#define REG_TM_TOTAL_PKTS_INC_MPCP_IN_FIFO_Q4 ((TM_BA_B + 0x00F4)*4)
#define REG_TM_TOTAL_PKTS_INC_MPCP_IN_FIFO_Q5 ((TM_BA_B + 0x00F5)*4)
#define REG_TM_TOTAL_PKTS_INC_MPCP_IN_FIFO_Q6 ((TM_BA_B + 0x00F6)*4)
#define REG_TM_TOTAL_PKTS_INC_MPCP_IN_FIFO_Q7 ((TM_BA_B + 0x00F7)*4)

/* bridge register */
#define BRG_BA_B                 (OPCONN_BASE_ADDR + 0x3000)
	
#define REG_CLE_INIT								((BRG_BA_B + 0x0000)*4)                   
#define REG_CLE_CTRL0								((BRG_BA_B + 0x0001)*4)                   
#define REG_CLE_CTRL1								((BRG_BA_B + 0x0002)*4)                   
#define REG_CLE_CTRL2								((BRG_BA_B + 0x0003)*4)                   
#define REG_CLE_MEM_INDIR_ACC_CTRL	((BRG_BA_B + 0x0004)*4)           
#define REG_CLE_STATE_MONI					((BRG_BA_B + 0x0008)*4)           
#define REG_CLE_RULE_MISS_COUNT		((BRG_BA_B + 0x0009)*4)                   
#define REG_CLE_RULE_DROP_COUNT		((BRG_BA_B + 0x000a)*4)                   
#define REG_CLE_SHADOW_SET0				((BRG_BA_B + 0x0010)*4)           
#define REG_CLE_SHADOW_SET1				((BRG_BA_B + 0x0011)*4)           
#define REG_CLE_SHADOW_SET2				((BRG_BA_B + 0x0012)*4)           
#define REG_CLE_SHADOW_SET3				((BRG_BA_B + 0x0013)*4)           
#define REG_CLE_SHADOW_SET4				((BRG_BA_B + 0x0014)*4)           
#define REG_CLE_SHADOW_SET5				((BRG_BA_B + 0x0015)*4)           
#define REG_CLE_SHADOW_SET6				((BRG_BA_B + 0x0016)*4)           
#define REG_CLE_SHADOW_SET7				((BRG_BA_B + 0x0017)*4)           
#define REG_CLE_SHADOW_SET8				((BRG_BA_B + 0x0018)*4)           
#define REG_CLE_SHADOW_SET9				((BRG_BA_B + 0x0019)*4)           
#define REG_BRG_VLAN_CTRL					((BRG_BA_B + 0x0020)*4)           
#define REG_BRG_I_COS_UP_MAP				((BRG_BA_B + 0x0021)*4)                   
#define REG_BRG_I_COS_DN_MAP				((BRG_BA_B + 0x0022)*4)                   
#define REG_BRG_PON_PORT_TABLE			((BRG_BA_B + 0x0023)*4)                   
#define REG_BRG_GE_PORT_TABLE			((BRG_BA_B + 0x0024)*4)                   
#define REG_BRG_USER_RSV_MAC0_0		((BRG_BA_B + 0x0030)*4)                   
#define REG_BRG_USER_RSV_MAC0_1		((BRG_BA_B + 0x0031)*4)                   
#define REG_BRG_USER_RSV_MAC1_0		((BRG_BA_B + 0x0032)*4)                   
#define REG_BRG_USER_RSV_MAC1_1		((BRG_BA_B + 0x0033)*4)                   
#define REG_BRG_RSV_MAC_CTRL0			((BRG_BA_B + 0x0034)*4)                   
#define REG_BRG_RSV_MAC_CTRL1			((BRG_BA_B + 0x0035)*4)                   
#define REG_BRG_RSV_MAC_CTRL2			((BRG_BA_B + 0x0036)*4)                   
#define REG_BRG_RSV_MAC_CTRL3			((BRG_BA_B + 0x0037)*4)                   
#define REG_BRG_RSV_MAC_CTRL4			((BRG_BA_B + 0x0038)*4)                   
#define REG_BRG_RSV_MAC_CTRL5			((BRG_BA_B + 0x0039)*4)                   
#define REG_BRG_RSV_MAC_CTRL6			((BRG_BA_B + 0x003a)*4)                   
#define REG_BRG_RSV_MAC_CTRL7			((BRG_BA_B + 0x003b)*4)                   
#define REG_BRG_RSV_MAC_CTRL8			((BRG_BA_B + 0x003c)*4)                   
#define REG_BRG_USER_RSV_MAC_CTRL	((BRG_BA_B + 0x003d)*4)           
#define REG_BRG_INT_EN							((BRG_BA_B + 0x0040)*4)                   
#define REG_BRG_INT									((BRG_BA_B + 0x0041)*4)           
#define REG_BRG_AGE_CTRL						((BRG_BA_B + 0x0044)*4)           
#define REG_BRG_AGE_IN_SEC					((BRG_BA_B + 0x0045)*4)  

#define REG_BRG_GE_RX_DROP_COUNT		((BRG_BA_B + 0x0048)*4)           
#define REG_BRG_GE_TX_DROP_COUNT		((BRG_BA_B + 0x0049)*4)           
#define REG_BRG_GE_VLAN_DROP_COUNT		((BRG_BA_B + 0x004a)*4)           
#define REG_BRG_GE_TAG_DROP_COUNT		((BRG_BA_B + 0x004b)*4)           
#define REG_BRG_PON_RX_DROP_COUNT		((BRG_BA_B + 0x004c)*4)           
#define REG_BRG_PON_TX_DROP_COUNT		((BRG_BA_B + 0x004d)*4)           
#define REG_BRG_PON_VLAN_DROP_COUNT		((BRG_BA_B + 0x004e)*4)           
#define REG_BRG_PON_TAG_DROP_COUNT		((BRG_BA_B + 0x004f)*4)           
#define REG_BRG_RSV_DROP_COUNT			((BRG_BA_B + 0x0050)*4)  
#define REG_BRG_GE_UC_SA_DROP_COUNT		((BRG_BA_B + 0x0051)*4)           
#define REG_BRG_DMAC_DROP_COUNT			((BRG_BA_B + 0x0052)*4)  
#define REG_BRG_PON_LEARN_DROP_COUNT	((BRG_BA_B + 0x0053)*4)
#define REG_BRG_GE_LEARN_DROP_COUNT		((BRG_BA_B + 0x0054)*4)
#define REG_BRG_PON_LEARN_CFL_DROP_COUNT	((BRG_BA_B + 0x0055)*4)	
#define REG_BRG_GE_LEARN_CFL_DROP_COUNT		((BRG_BA_B + 0x0056)*4)	
#define REG_BRG_PON_UC_SA_DROP_COUNT		((BRG_BA_B + 0x0057)*4)     
#define REG_BRG_DS_0_MAC_DROP_COUNT         ((BRG_BA_B + 0x0058)*4)
#define REG_BRG_US_0_MAC_DROP_COUNT         ((BRG_BA_B + 0x0059)*4)

#define REG_BRG_ARL_CAP_REG0					((BRG_BA_B + 0x0060)*4)                   
#define REG_BRG_ARL_CAP_REG1					((BRG_BA_B + 0x0061)*4)                   

#define REG_BRG_DN_VLAN_CAP_REG			((BRG_BA_B + 0x0064)*4)                   
#define REG_BRG_UP_VLAN_CAP_REG			((BRG_BA_B + 0x0065)*4)                   
#define REG_BRG_UP_TAG_CAP_REG				((BRG_BA_B + 0x0066)*4)                   
#define REG_BRG_DN_TAG_CAP_REG				((BRG_BA_B + 0x0067)*4)   

#define REG_BRG_VTT_REC_0               ((BRG_BA_B + 0x0080)*4)
#define REG_BRG_VTT_REC_1               ((BRG_BA_B + 0x0081)*4)
#define REG_BRG_VTT_REC_2               ((BRG_BA_B + 0x0082)*4)
#define REG_BRG_VTT_REC_3               ((BRG_BA_B + 0x0083)*4)
#define REG_BRG_VTT_REC_4               ((BRG_BA_B + 0x0084)*4)
#define REG_BRG_VTT_REC_5               ((BRG_BA_B + 0x0085)*4)
#define REG_BRG_VTT_REC_6               ((BRG_BA_B + 0x0086)*4)
#define REG_BRG_VTT_REC_7               ((BRG_BA_B + 0x0087)*4)
#define REG_BRG_VTT_REC_8               ((BRG_BA_B + 0x0088)*4)
#define REG_BRG_VTT_REC_9               ((BRG_BA_B + 0x0089)*4)
#define REG_BRG_VTT_REC_10              ((BRG_BA_B + 0x008A)*4)
#define REG_BRG_VTT_REC_11              ((BRG_BA_B + 0x008B)*4)
#define REG_BRG_VTT_REC_12              ((BRG_BA_B + 0x008C)*4)
#define REG_BRG_VTT_REC_13              ((BRG_BA_B + 0x008D)*4)
#define REG_BRG_VTT_REC_14              ((BRG_BA_B + 0x008E)*4)
#define REG_BRG_VTT_REC_15              ((BRG_BA_B + 0x008F)*4)

/* mpcp and pon control register */
#define MPCP_BA_B                    (OPCONN_BASE_ADDR + 0x4000)

#define REG_MPCP_INT_EN						((MPCP_BA_B + 0x0000)*4) 
#define REG_MPCP_INT								((MPCP_BA_B + 0x0001)*4)     
#define REG_MPCP_DBA_AGENT_CFG		((MPCP_BA_B + 0x0002)*4)
#define REG_MPCP_TX_DLY						((MPCP_BA_B + 0x0003)*4)

#define REG_MPCP_PARSER_CTRL				((MPCP_BA_B + 0x0004)*4)         
#define REG_MPCP_PARSER_TIME_THR		((MPCP_BA_B + 0x0005)*4) 
#define REG_MPCP_DISC_CTRL					((MPCP_BA_B + 0x0006)*4) 
#define REG_MPCP_DISC_STATUS				((MPCP_BA_B + 0x0007)*4)         
#define REG_MPCP_GATE_LASER_ON			((MPCP_BA_B + 0x0008)*4)         
#define REG_MPCP_GATE_LASER_OFF		((MPCP_BA_B + 0x0009)*4) 
#define REG_MPCP_GATE_THR_H				((MPCP_BA_B + 0x000a)*4)         
#define REG_MPCP_GATE_THR_L				((MPCP_BA_B + 0x000b)*4)  
#define REG_MPCP_TIME_DRIFT_MAX		((MPCP_BA_B + 0x000C)*4)
#define REG_MPCP_TAIL_GUARD				((MPCP_BA_B + 0x000d)*4)
#define REG_MPCP_PKT_OVERHEAD			((MPCP_BA_B + 0x000e)*4)
#define REG_MPCP_LASERON_DLY				((MPCP_BA_B + 0x000F)*4)

#define REG_MPCP_FSM_HOLD_CTRL	    ((MPCP_BA_B + 0x0010)*4)
#define REG_MPCP_FSM_HOLD_TIME	    ((MPCP_BA_B + 0x0011)*4)
#define REG_MPCP_NORM_GATE_CNT		((MPCP_BA_B + 0x0012)*4)         
#define REG_MPCP_DISC_PKT_CNT			((MPCP_BA_B + 0x0013)*4)         
#define REG_MPCP_PKT_DROP_CNT			((MPCP_BA_B + 0x0014)*4)         
#define REG_MPCP_CLE_PKT_CNT				((MPCP_BA_B + 0x0015)*4)         
#define REG_MPCP_CLE_DROP_CNT			((MPCP_BA_B + 0x0016)*4)         
#define REG_MPCP_DISCV_GATE_CNT		((MPCP_BA_B + 0x0017)*4) 

#define REG_MPCP_GRANT_CNT					((MPCP_BA_B + 0x0018)*4)
#define REG_GATE_DROP_CNT					((MPCP_BA_B + 0x0019)*4) 
#define REG_GRANT_RTP_PKT_CNT				((MPCP_BA_B + 0x001A)*4)
#define REG_GRANT_RPT_DROP_CNT			((MPCP_BA_B + 0x001B)*4)
#define REG_GRANT_TOTAL_LEN				((MPCP_BA_B + 0x001C)*4)
#define REG_MPCP_DRIFT_DROP_CNT			((MPCP_BA_B + 0x001D)*4)
#define REG_GRANT_TOTAL_LEN_IN_SEC		((MPCP_BA_B + 0x001E)*4)
#define REG_MPCP_GATE_TIMEOUT			((MPCP_BA_B + 0x001F)*4)

#define REG_US_NORMPKT_TX_CNT			((MPCP_BA_B + 0x0020)*4)  
#define REG_US_REGREQ_TX_CNT				((MPCP_BA_B + 0x0021)*4)
#define REG_US_REGACK_TX_CNT				((MPCP_BA_B + 0x0022)*4)
#define REG_US_RPTPKT_TX_CNT				((MPCP_BA_B + 0x0023)*4)

#define REG_MPCP_SOP_DROP_CNT			((MPCP_BA_B + 0x0024)*4)    
#define REG_MPCP_CLE_FULL_DROP_CNT ((MPCP_BA_B + 0x0025)*4)

#define REG_MPCP_SCH_CFG ((MPCP_BA_B + 0x0028)*4)

#define REG_MPCP_DYGSP_CFG0				((MPCP_BA_B + 0x0030)*4)         
#define REG_MPCP_DYGSP_CFG1				((MPCP_BA_B + 0x0031)*4)         
#define REG_MPCP_DYGSP_CFG2				((MPCP_BA_B + 0x0032)*4)         
#define REG_MPCP_DYGSP_CFG3				((MPCP_BA_B + 0x0033)*4)         
#define REG_MPCP_DYGSP_CFG4				((MPCP_BA_B + 0x0034)*4)         
#define REG_MPCP_DYGSP_CFG5				((MPCP_BA_B + 0x0035)*4)         
#define REG_MPCP_DYGSP_CFG6				((MPCP_BA_B + 0x0036)*4)         
#define REG_MPCP_DYGSP_CFG7				((MPCP_BA_B + 0x0037)*4)         
#define REG_MPCP_DYGSP_CFG8				((MPCP_BA_B + 0x0038)*4)       
#define REG_OAM_RST_CFG					((MPCP_BA_B + 0x0039)*4)
#define REG_OAM_RST_PAYLOAD				((MPCP_BA_B + 0x003A)*4)

/*	ddr config register	*/
#define  MARB_BA_B	                 (OPCONN_BASE_ADDR + 0x5000)

#define REG_MARB_EN							((MARB_BA_B + 0x0020)*4)                         
#define REG_MARB_CFG							((MARB_BA_B + 0x0021)*4)                                 
#define REG_MARB_WEIGHT0					((MARB_BA_B + 0x0023)*4)                         
#define REG_MARB_WEIGHT1					((MARB_BA_B + 0x0024)*4)                         
#define REG_MARB_WEIGHT2					((MARB_BA_B + 0x0025)*4)                         
#define REG_UPMARB_UCAST_COUNT	((MARB_BA_B + 0x0030)*4)                                 
#define REG_DNMARB_UCAST_COUNT	((MARB_BA_B + 0x0031)*4)                                 
#define REG_MARB_CPTM_COUNT			((MARB_BA_B + 0x0032)*4)                                 

#define REG_MARB_FE_COUNT				((MARB_BA_B + 0x0034)*4)                         
#define REG_MIPS_WORD_CNT				((MARB_BA_B + 0x0035)*4)                         
#define REG_MARB_FREE_COUNT			((MARB_BA_B + 0x0040)*4)                         
#define REG_MARB_SDRAM_FULL			((MARB_BA_B + 0x0041)*4)                         
#define REG_DDR_INT								((MARB_BA_B + 0x0042)*4)                         
#define REG_DDR_INT_EN						((MARB_BA_B + 0x0043)*4)                                 
#define REG_SELF_TEST_DONE				((MARB_BA_B + 0x0048)*4)                         
#define REG_SELF_TEST_STATUS			((MARB_BA_B + 0x0049)*4)                                 
#define REG_SELF_ERR_STEP					((MARB_BA_B + 0x004a)*4)                         
#define REG_SELF_ERR_ADDR				((MARB_BA_B + 0x004b)*4)                         
#define REG_SELF_ERR_DATA					((MARB_BA_B + 0x004c)*4)                         
#define REG_LL_QCTRL_STATE				((MARB_BA_B + 0x0050)*4)                         
#define REG_LL_FREE_STATE					((MARB_BA_B + 0x0051)*4)                         
#define REG_DDR_FPGA_TEST				((MARB_BA_B + 0x0058)*4)                         
#define REG_MARB_DDR2_IO					((MARB_BA_B + 0x0059)*4)                         
#define REG_MARB_DDR2_STATUS			((MARB_BA_B + 0x005a)*4)                                 
#define REG_MARB_DDR2_CTRL				((MARB_BA_B + 0x005b)*4)                         



/*host dma register address */
#define DMA0_BA_B	                 (OPCONN_BASE_ADDR + 0x6000)
#define REG_DMA0_SHADOW_SET_RXBD					((DMA0_BA_B+0x0000)*4)                            
#define REG_DMA0_MEM_INDIR_ACC_CTRL_RXBD	((DMA0_BA_B+0x0001)*4)                            
#define REG_DMA0_SHADOW_SET_TXBD					((DMA0_BA_B+0x0002)*4)                            
#define REG_DMA0_MEM_INDIR_ACC_CTRL_TXBD	((DMA0_BA_B+0x0003)*4)                            
#define REG_DMA0_BASE_ADDR									((DMA0_BA_B+0x0004)*4)                            
#define REG_DMA0_EN					((DMA0_BA_B+0x0005)*4)                            
#define REG_DMA0_INT					((DMA0_BA_B+0x0006)*4)                                                    
#define REG_DMA0_INT_EN			((DMA0_BA_B+0x0007)*4)                                                    
#define REG_DMA0_QLEN0_CELL	((DMA0_BA_B+0x0010)*4)                                            
#define REG_DMA0_QLEN1_CELL	((DMA0_BA_B+0x0011)*4)                                            
#define REG_DMA0_QLEN2_CELL	((DMA0_BA_B+0x0012)*4)                                            
#define REG_DMA0_QLEN3_CELL	((DMA0_BA_B+0x0013)*4)                                            
#define REG_DMA0_QLEN4_CELL	((DMA0_BA_B+0x0014)*4)                                            
#define REG_DMA0_QLEN5_CELL	((DMA0_BA_B+0x0015)*4)                                            
#define REG_DMA0_QLEN6_CELL	((DMA0_BA_B+0x0016)*4)                                            
#define REG_DMA0_QLEN7_CELL	((DMA0_BA_B+0x0017)*4)                                            
#define REG_DMA0_QLEN0_PKT		((DMA0_BA_B+0x0018)*4)                                            
#define REG_DMA0_QLEN1_PKT		((DMA0_BA_B+0x0019)*4)                                            
#define REG_DMA0_QLEN2_PKT		((DMA0_BA_B+0x001a)*4)                                            
#define REG_DMA0_QLEN3_PKT		((DMA0_BA_B+0x001b)*4)                                            
#define REG_DMA0_QLEN4_PKT		((DMA0_BA_B+0x001c)*4)                                            
#define REG_DMA0_QLEN5_PKT		((DMA0_BA_B+0x001d)*4)                                            
#define REG_DMA0_QLEN6_PKT		((DMA0_BA_B+0x001e)*4)                                            
#define REG_DMA0_QLEN7_PKT		((DMA0_BA_B+0x001f)*4)                                            
                                        
#define REG_DMA0_QID_ICOS0		((DMA0_BA_B+0x0028)*4)                                            
#define REG_DMA0_QID_ICOS1		((DMA0_BA_B+0x0029)*4)                                            
#define REG_DMA0_QID_ICOS2		((DMA0_BA_B+0x002a)*4)                                            
#define REG_DMA0_QID_ICOS3		((DMA0_BA_B+0x002b)*4)                                            
#define REG_DMA0_QID_ICOS4		((DMA0_BA_B+0x002c)*4)                                            
#define REG_DMA0_QID_ICOS5		((DMA0_BA_B+0x002d)*4)                                            
#define REG_DMA0_QID_ICOS6		((DMA0_BA_B+0x002e)*4)                                            
#define REG_DMA0_QID_ICOS7		((DMA0_BA_B+0x002f)*4)                                            
#define REG_DMA0_POLIC_EN		((DMA0_BA_B+0x0030)*4)                                            
#define REG_DMA0_POLIC_STEP_CNT0		((DMA0_BA_B+0x0031)*4)                                            
#define REG_DMA0_POLIC_STEP_CNT1		((DMA0_BA_B+0x0032)*4)                                        
#define REG_DMA0_GE_POLIC_CIR					((DMA0_BA_B+0x0033)*4)                                            
#define REG_DMA0_GE_POLIC_CBS					((DMA0_BA_B+0x0034)*4)                                            
#define REG_DMA0_PON_POLIC_CIR					((DMA0_BA_B+0x0035)*4)                                            
#define REG_DMA0_PON_POLIC_CBS					((DMA0_BA_B+0x0036)*4)                                            
#define REG_DMA0_SHAPER_EN						((DMA0_BA_B+0x0037)*4)                                    
#define REG_DMA0_SHAPER_RATE				    ((DMA0_BA_B+0x0038)*4)                                    
#define REG_DMA0_SHAPER_COMPEN	                ((DMA0_BA_B+0x0039)*4)                                    
#define REG_DMA0_Q0_CELL_CNT		((DMA0_BA_B+0x0040)*4)                                            
#define REG_DMA0_Q1_CELL_CNT		((DMA0_BA_B+0x0041)*4)                                            
#define REG_DMA0_Q2_CELL_CNT		((DMA0_BA_B+0x0042)*4)                                            
#define REG_DMA0_Q3_CELL_CNT		((DMA0_BA_B+0x0043)*4)                                            
#define REG_DMA0_Q4_CELL_CNT		((DMA0_BA_B+0x0044)*4)                                            
#define REG_DMA0_Q5_CELL_CNT		((DMA0_BA_B+0x0045)*4)                                            
#define REG_DMA0_Q6_CELL_CNT		((DMA0_BA_B+0x0046)*4)                                            
#define REG_DMA0_Q7_CELL_CNT		((DMA0_BA_B+0x0047)*4)                                            
#define REG_DMA0_Q0_PKT_CNT		((DMA0_BA_B+0x0048)*4)                                            
#define REG_DMA0_Q1_PKT_CNT		((DMA0_BA_B+0x0049)*4)                                            
#define REG_DMA0_Q2_PKT_CNT		((DMA0_BA_B+0x004a)*4)                                            
#define REG_DMA0_Q3_PKT_CNT		((DMA0_BA_B+0x004b)*4)                                            
#define REG_DMA0_Q4_PKT_CNT		((DMA0_BA_B+0x004c)*4)                                            
#define REG_DMA0_Q5_PKT_CNT		((DMA0_BA_B+0x004d)*4)                                            
#define REG_DMA0_Q6_PKT_CNT		((DMA0_BA_B+0x004e)*4)                                            
#define REG_DMA0_Q7_PKT_CNT		((DMA0_BA_B+0x004f)*4)                                            
#define REG_DMA0_RX_CNT				((DMA0_BA_B+0x0050)*4)                                                    
#define REG_DMA0_TX_CNT				((DMA0_BA_B+0x0051)*4)                                                    
#define REG_DMA0_GE2CPU_ENQ_CNT				((DMA0_BA_B+0x0052)*4)                                    
#define REG_DMA0_GE2CPU_POLIC_DRPCNT		((DMA0_BA_B+0x0053)*4)                                    
#define REG_DMA0_GE2CPU_QFULL_DRPCNT	    ((DMA0_BA_B+0x0054)*4)                                    
#define REG_DMA0_PON2CPU_ENQ_CNT			((DMA0_BA_B+0x0055)*4)                            
#define REG_DMA0_PON2CPU_POLIC_DRPCNT	((DMA0_BA_B+0x0056)*4)                                    
#define REG_DMA0_PON2CPU_QFULL_DRPCNT	((DMA0_BA_B+0x0057)*4)  
#define REG_DMA0_DEQ_CNT                ((DMA0_BA_B+0x0058)*4)
#define REG_GE_FIFO_POST_PKT_ERR_CNT    ((DMA0_BA_B+0x0059)*4)
#define REG_PON_FIFO_POST_PKT_ERR_CNT   ((DMA0_BA_B+0x005A)*4)
#define REG_GE_FIFO_EARLING_PKT_DRPCNT  ((DMA0_BA_B+0x005B)*4)
#define REG_PON_FIFO_EARLING_PKT_DRPCNT ((DMA0_BA_B+0x005C)*4) 
                                         
#define REG_DMA0_RXBD_POINTER	((DMA0_BA_B+0x0080)*4)                                            
#define REG_DMA0_TXBD_POINTER	((DMA0_BA_B+0x0081)*4)  
#define REG_DMA0_CIR            ((DMA0_BA_B+0x0082)*4)
#define REG_DMA0_CBS            ((DMA0_BA_B+0x0083)*4)
#define REG_DMA0_EnqueueLengthErrCnt  ((DMA0_BA_B+0x0085)*4)
#define REG_DMA0_EnqueueCRCErrCnt	  ((DMA0_BA_B+0x0086)*4)


/*FE DMA REGISTER*/
#define DMA1_BA_B	                 (OPCONN_BASE_ADDR + 0x7000)

#define REG_DMA1_SHADOW_SET_RXBD					((DMA1_BA_B+0x0000)*4)
#define REG_DMA1_MEM_INDIR_ACC_CTRL_RXBD	((DMA1_BA_B+0x0001)*4)
#define REG_DMA1_SHADOW_SET_TXBD					((DMA1_BA_B+0x0002)*4)
#define REG_DMA1_MEM_INDIR_ACC_CTRL_TXBD	((DMA1_BA_B+0x0003)*4)
#define REG_DMA1_BASE_ADDR									((DMA1_BA_B+0x0004)*4)
#define REG_DMA1_EN												((DMA1_BA_B+0x0005)*4)
#define REG_DMA1_INT												((DMA1_BA_B+0x0006)*4)
#define REG_DMA1_INT_EN										((DMA1_BA_B+0x0007)*4)
#define REG_DMA1_TX_CNT										((DMA1_BA_B+0x0008)*4)
#define REG_DMA1_ENQ_CNT										((DMA1_BA_B+0x0009)*4)
#define REG_DMA1_FEMACERR_DRPCNT					((DMA1_BA_B+0x000a)*4)
#define REG_DMA1_SEF_FULL_DRPCNT						((DMA1_BA_B+0x000b)*4)
#define REG_DMA1_QFULL_DRPCNT							((DMA1_BA_B+0x000c)*4)
#define REG_DMA1_DEQ_CNT										((DMA1_BA_B+0x000d)*4)
#define REG_DMA1_RX_CNT										((DMA1_BA_B+0x000e)*4)

#define REG_DMA1_PKTIN_THRESH							((DMA1_BA_B+0x0010)*4)
#define REG_DMA1_SHAPER_EN									((DMA1_BA_B+0x0011)*4)
#define REG_DMA1_SHAPER_STEP_CNT						((DMA1_BA_B+0x0012)*4)
#define REG_DMA1_SHAPER_CLKNUM_PERIOD			((DMA1_BA_B+0x0013)*4)
#define REG_DMA1_SHAPER_CIR									((DMA1_BA_B+0x0014)*4)
#define REG_DMA1_SHAPER_CBS									((DMA1_BA_B+0x0015)*4)
#define REG_DMA1_QLEN_CELL									((DMA1_BA_B+0x0016)*4)
#define REG_DMA1_QLEN_PKT										((DMA1_BA_B+0x0017)*4)
#define REG_DMA1_QCELL_CNT									((DMA1_BA_B+0x0018)*4)
#define REG_DMA1_QPKT_CNT										((DMA1_BA_B+0x0019)*4)
#define REG_DMA1_GMAC_SPEED								((DMA1_BA_B+0x0020)*4)
#define REG_DMA1_PAUSE_MODE								((DMA1_BA_B+0x0021)*4)
#define REG_DMA1_PAUSE_PARA									((DMA1_BA_B+0x0022)*4)
#define REG_DMA1_HI_PTHRESH_PKT						((DMA1_BA_B+0x0023)*4)
#define REG_DMA1_LO_PTHRESH_PKT						((DMA1_BA_B+0x0024)*4)
#define REG_DMA1_HI_PTHRESH_CELL						((DMA1_BA_B+0x0025)*4)
#define REG_DMA1_LO_PTHRESH_CELL						((DMA1_BA_B+0x0026)*4)
#define REG_DMA1_HI_PTHRESH_SFIFO					((DMA1_BA_B+0x0027)*4)
#define REG_DMA1_LO_PTHRESH_SFIFO					((DMA1_BA_B+0x0028)*4)
#define REG_DMA1_RXBD_POINTER								((DMA1_BA_B+0x0030)*4)
#define REG_DMA1_TXBD_POINTER								((DMA1_BA_B+0x0031)*4)

/*GE PARSER REGISTER*/
#define GE_PARSER_BA_B    				(OPCONN_BASE_ADDR + 0x8000)
#define REG_GE_PARSER_SHAPER							((GE_PARSER_BA_B+0x0000)*4)                     
#define REG_GE_PARSER_PKTLEN_PARA				((GE_PARSER_BA_B+0x0001)*4)             
#define REG_GE_PARSER_RX_THRESHOLD				((GE_PARSER_BA_B+0x0002)*4)             
#define REG_GE_PARSER_EPD_THRESHOLD			((GE_PARSER_BA_B+0x0003)*4)             
#define REG_GE_PARSER_PPD_THRESHOLD			((GE_PARSER_BA_B+0x0004)*4)             
#define REG_GE_PARSER_RX_DROP_PKTCNT			((GE_PARSER_BA_B+0x0005)*4)             
#define REG_GE_PARSER_FULL_DROP_PKTCNT		((GE_PARSER_BA_B+0x0006)*4)             
#define REG_GE_PARSER_ERR_DROP_PKTCNT		((GE_PARSER_BA_B+0x0007)*4)             
#define REG_GE_PARSER_TOKEN_DROP_PKTCNT	((GE_PARSER_BA_B+0x0008)*4)             
#define REG_GMAC_SPEED											((GE_PARSER_BA_B+0x0009)*4)             
#define REG_GE_PAUSE_STARTSTOP						((GE_PARSER_BA_B+0x000A)*4)                     
#define REG_CFG_GE_PAUSE_PARAM						((GE_PARSER_BA_B+0x000B)*4)                     
#define REG_CFG_GE_PAUSE_PARAM_SYS				((GE_PARSER_BA_B+0x000C)*4)             
#define REG_GE_PARSER_STEP_INC						((GE_PARSER_BA_B+0x000D)*4)                     
#define REG_GE_PARSER_RX_PKTCNT						((GE_PARSER_BA_B+0x000E)*4)
#define REG_GE_MaxMinDropPktCnt						((GE_PARSER_BA_B+0x000f)*4)
#define REG_GE_FIFOFullInterruptStatus				((GE_PARSER_BA_B+0x0010)*4)
#define REG_GE_FIFOFullInterruptEnable				((GE_PARSER_BA_B+0x0011)*4)


#define OPCONN_SIZE   32
#define FIELD_BITS(n)				(0xffffffff >> (OPCONN_SIZE - (n)))
#define OPCONN_BIT(n)				(1<<(n))
#define DBG_LINE 


typedef struct OPCONN_REG_INFO_s
{
	unsigned int	regAddr;
	unsigned int 	rwMask;
	unsigned int 	rsvMask;
	unsigned int 	rMask;
	unsigned int 	wMask;
	unsigned int  	rcMask;
	unsigned int 	defaultVal;
	unsigned char 		*desc;
}OPCONN_REG_INFO_t;

typedef struct OPCONN_TAB_INFO_s
{
	unsigned int 	tabId;
	unsigned int  	numOfRec;
	unsigned int 	widthOfRec;
	unsigned char 		*desc;
}OPCONN_TAB_INFO_t;

typedef enum OPCONN_TAB_s
{
	OPCONN_TAB_CLS_MASK = 0,
	OPCONN_TAB_CLS_ADDR,
	OPCONN_TAB_CLS_RULE,
	OPCONN_TAB_CLS_COUNTER,
	OPCONN_TAB_PON_MAC,
	OPCONN_TAB_US_SHPBD,
	OPCONN_TAB_DS_SHPBD,
	OPCONN_TAB_US_WRED,
	OPCONN_TAB_DS_WRED,
	OPCONN_TAB_Q_CONF_STA,
	OPCONN_TAB_DMA0_RX,
	OPCONN_TAB_DMA0_TX,
	OPCONN_TAB_DMA1_RX,
	OPCONN_TAB_DMA1_TX,
	OPCONN_TAB_END
}OPCONN_TAB_e;

typedef struct OPCONN_COUNTER_s
{
	unsigned char *name;
	unsigned int regId;
	unsigned int regAddr;
	unsigned int regMask;
	unsigned char *desc;
}OPCONN_COUNTER_t;

typedef struct OPCONN_TAB_CTL_REG_MAP_s
{
	unsigned int tabId;
	unsigned int regCtlAddr;
	unsigned int dataAddr;
}OPCONN_TAB_CTL_REG_MAP_t;



int oplRegBitTest (unsigned int regId, unsigned int bits);

void oplRegBitSet(unsigned int regId, unsigned int bits);	

void oplRegBitClear(unsigned int regId, unsigned int bits);	

void oplRegBitToggle(unsigned int regId, unsigned int bits);	


#endif
