/*
=============================================================================
     Header Name: phy.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/14		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifndef PHY_H
#define PHY_H

#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"
#include "hw_interface.h"

typedef enum PHY_SPEED_MODE_s
{
	PHY_SPEED_10M 	= 0X00,
	PHY_SPEED_100M 	= 0X01,
	PHY_SPEED_1000M	= 0X02,
	PHY_SPEED_RSV
}PHY_SPEED_MODE_e;

typedef enum PHY_DUPLEX_MODE_s
{
	PHY_HALF_DUPLEX = 0X00,
	PHY_FULL_DUPLEX  = 0X01	
}PHY_DUPLEX_MODE_e;

typedef enum PHY_AUTONEG_DONE_s
{
	PHY_AUTONEG_FAILED = 0X00,
	PHY_AUTONEG_COMPLETED = 0X01	
}PHY_AUTONEG_DONE_e;


#ifdef ONU_RESPIN_NA
	#define PHY_TYPE_MARVELL 1
	#ifdef PHY_TYPE_REALTEK
		#undef PHY_TYPE_REALTEK
	#endif 
#else
#define PHY_TYPE_REALTEK 1

#endif 


#ifdef PHY_TYPE_REALTEK


#ifdef ONU_RESPIN_A
	#define PHY_DEVICE_ADDR     	0x7
#else
	#define PHY_DEVICE_ADDR     	0x2
#endif

/* Basic Mode Control Register */
#define PHY_BMCR                0

/* Basic Mode Status Register */
#define PHY_BMSR                1

/* PHY Identifier Register 1 */
#define PHY_PHYID1              2
/* PHY Identifier Register 2 */
#define PHY_PHYID2              3

/* Auto-Negotiation Advertising Register */
#define PHY_ANAR                4

/* Auto-Negotiation Link Partner Ability Register */
#define PHY_ANLPAR              5

/* Auto-Negotiation Expansion Register */
#define PHY_ANER                6

/* Auto-Negotiation Next Page Transmit Register */
#define PHY_ANNPTR              7

/* Auto-Negotiation Next Page Receive Register */
#define PHY_ANNPRR              8

/* 1000Base-T Control Register */
#define PHY_GBCR                9

/* 1000Base-T Status Register */
#define PHY_GBSR                10

/* 1000Base-T Extended Status Register */
#define PHY_GBESR               15

/* PHY Specific Control Register */
#define PHY_PHYCR               16

/* PHY Specific Status Register */
#define PHY_PHYSR               17

/* Interrupt Enable Register */
#define PHY_INER                18

/* Interrupt Status Register */
#define PHY_INSR                19

/* Receive Error Counter */
#define PHY_RXERC               21

/* LED Control Register */
#define PHY_LEDCR               24

#endif

#ifdef PHY_TYPE_MARVELL

#ifdef ONU_RESPIN
#define PHY_DEVICE_ADDR     	0x2
#else
#define PHY_DEVICE_ADDR     	0x18
#endif

/* seclect copper banks of register 0,1,4,5,6,7,8,17,18,19*/
#define PHY_COPPER_PAGE0		0X00  
/* seclect fiber banks of register 0,1,4,5,6,7,8,17,18,19*/
#define PHY_FIBER_PAGE1		0X01

#define PAGE00_OF_REG30		0X00
#define PAGE07_OF_REG30		0X07
#define PAGE12_OF_REG30		0X0C
#define PAGE16_OF_REG30		0X10
#define PAGE18_OF_REG30		0X12

#define CONTROL_REG								0
#define STATUS_REG									1
#define PHY_ID_0										2
#define PHY_ID_1										3
#define AUTO_NEG_AD             					4
#define LINK_PARTNER_AB       					5
#define AUTO_NEG_EXPEN							6
#define NEXT_PAGE_TRAN							7
#define LINK_PARTNER_NEXT_PAGE			8
#define BASE_T_CONTROL_1000				9
#define BASE_T_STATUS_1000					10

#define EXTEND_STATUS								15
#define PHY_SPEC_CONTROL						16
#define PHY_SPEC_STATUS						17

#define INTERRUPT_ENABLE						18
#define INTERRUPT_STATUS						19
#define EXTEND_PHY_SPEC_CONTROL		20
#define RECEIVE_ERROR_COUNTER			21

#define EXTEND_ADDRESS_22					22
#define GLOBAL_STATUS							23
#define LED_CONTROL 								24
#define MANUAL_LED_OVERRIDE				25
#define EXTEND_PHY_SPEC_CONTROL_2 	26
#define EXTEND_PHY_SPEC_STATUS			27
#define VCT_STATUS									28
#define EXTEND_ADDRESS_29					29

#define CALIBRATION_OVERRIDE				30
#define FORCE_GIGABIT_MODE					30
#define TRANSMITTER_TYPE						30
#define CRC_CHECKER_RESULT					30
#define TEST_ENABLE_CONTROL				30
#define MISCELLANEOUS_CONTROL			30

/* LED Control Register */
#define PHY_LEDCR               24
#endif

/* ******************************************************************************
 * speed,duplex,and auto-negotiation eanble take on the values set by hardware reset only
 * a write to the register has no effect unless any one of the following also ocuures:
 * 1.	software reset 0.15
 * 2.	restart auto-negotiation 0.9
 * 3.	transition from power up 0.11
*  4.	link goes down
********************************************************************************/
OPL_STATUS phySpeedHwWrite(UINT32 phySpeedMode);

OPL_STATUS phySpeedHwRead(PHY_SPEED_MODE_e *phySpeedMode);

OPL_STATUS phyDuplexHwWrite(PHY_DUPLEX_MODE_e phyDuplexMode);

OPL_STATUS phyDuplexHwRead(PHY_DUPLEX_MODE_e *phyDuplexMode);

OPL_STATUS phyAutoNegHwWrite(UINT8 enable);

OPL_STATUS phyAutoNegHwRead(UINT8 *enable);

OPL_STATUS phyRestartAutoNeg(void);

OPL_STATUS phyLoopbackEnableHwWrite(UINT8 enable);

OPL_STATUS phyLoopbackEnableHwRead(UINT8 *enable);

OPL_STATUS phyReset(void);

OPL_STATUS phyPowerDown(UINT8 enable);

OPL_STATUS phyLinkStatusHwRead(UINT8 *linkStatus);

OPL_STATUS phyFlowControlEnHwWrite(UINT8 enable);

OPL_STATUS phyRegShow(UINT16 phyAddr);

/*add by xukang ---start */
OPL_STATUS phySpecificAutoRead(UINT32 *phyAutonegDone);
OPL_STATUS phySpecificSpeedRead(PHY_SPEED_MODE_e *phySpeed);
OPL_STATUS phySpecificDuplexRead(PHY_DUPLEX_MODE_e *phyDuplex);

/*add by xukang ---end */

#endif
