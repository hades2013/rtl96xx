/*
=============================================================================
     Header Name: phy.c

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
#include "phy.h"
#include "opconn_lib.h"

UINT16 phyAdminStatus = 1;

#define PHY_POWER_DOWN \
	if(phyAdminStatus){ \
		 phyPowerDown(1); \
	}
	
#define PHY_POWER_UP \
	if(phyAdminStatus){ \
		 phyPowerDown(0); \
	}
/* remove the phy define, should just be in H file */
#ifdef PHY_TYPE_REALTEK

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

/* ******************************************************************************
 * speed,duplex,and auto-negotiation eanble take on the values set by hardware reset only
 * a write to the register has no effect unless any one of the following also ocuures:
 * 1.	software reset 0.15
 * 2.	restart auto-negotiation 0.9
 * 3.	transition from power up 0.11
*  4.	link goes down
********************************************************************************/
OPL_STATUS phySpeedHwWrite(UINT32 phySpeedMode)
{	
	OPL_STATUS retVal = OPL_OK;

	if(PHY_SPEED_1000M < phySpeedMode)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
    PHY_POWER_DOWN
	switch(phySpeedMode)
	{
		case PHY_SPEED_10M:
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_BMCR,  6,1, 0);
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_BMCR, 13,1, 0);
			break;
		case PHY_SPEED_100M:
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_BMCR,  6,1, 0);
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_BMCR, 13,1, 1);
			break;
		case PHY_SPEED_1000M:
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_BMCR,  6,1, 1);
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_BMCR, 13,1, 0);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
    PHY_POWER_UP

	return retVal;
}

OPL_STATUS phySpeedHwRead(PHY_SPEED_MODE_e *phySpeedMode)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 highBit;
	UINT16 lowBit;
	UINT16 regVal;

	if(OPL_NULL == phySpeedMode)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, PHY_BMCR,  6,1, &highBit);
	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, PHY_BMCR,  13,1, &lowBit);

	regVal = ((highBit<<1)|(lowBit))&0x3;

	*phySpeedMode = regVal;

	return retVal;
}

OPL_STATUS phyDuplexHwWrite(PHY_DUPLEX_MODE_e phyDuplexMode)
{	
	OPL_STATUS retVal = OPL_OK;

	if(PHY_HALF_DUPLEX > phyDuplexMode ||PHY_FULL_DUPLEX < phyDuplexMode)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

    PHY_POWER_DOWN
	switch(phyDuplexMode)
	{
		case PHY_HALF_DUPLEX:
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_BMCR,  8,1, 0);
			break;
		case PHY_FULL_DUPLEX:
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_BMCR,  8,1, 1);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	PHY_POWER_UP
	return retVal;
}

OPL_STATUS phyDuplexHwRead(PHY_DUPLEX_MODE_e *phyDuplexMode)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;

	if(OPL_NULL == phyDuplexMode)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, PHY_BMCR,  8,1, &regVal);

	*phyDuplexMode = regVal;

	return retVal;
}

OPL_STATUS phyAutoNegHwWrite(UINT8 enable)
{
	UINT16 regVal;
	OPL_STATUS retVal = OPL_OK;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;
    PHY_POWER_DOWN
	retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_BMCR,  12,1, regVal);
    PHY_POWER_UP
	return retVal;
}
OPL_STATUS phyAutoNegHwRead(UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, PHY_BMCR,  12,1, &regVal);

	*enable = regVal;

	return retVal;
}

OPL_STATUS phyRestartAutoNeg(void)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_BMCR,  9,1, 1);

	return retVal;
}

OPL_STATUS phyLoopbackEnableHwWrite(UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	
	retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_BMCR,  14,1, regVal);

	return retVal;
}

OPL_STATUS phyLoopbackEnableHwRead(UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, PHY_BMCR,  14,1, &regVal);

	*enable = regVal;

	return retVal;
}

OPL_STATUS phyReset(void)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_BMCR,  15,1, 1);

	return retVal;
}

OPL_STATUS phyPowerDown(UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_BMCR,  11,1, enable);

	return retVal;
}

OPL_STATUS phyLinkStatusHwRead(UINT8 *linkStatus)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;

	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, PHY_PHYSR,  10,1, &regVal);

	*linkStatus = regVal;

	return retVal;
}

OPL_STATUS phyFlowControlEnHwWrite(UINT8 enable)
{
    OPL_STATUS retVal = OPL_OK;

    retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, PHY_ANAR,  10,1, enable);

    return retVal;
}

OPL_STATUS phyRegShow(UINT16 phyAddr)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regAddr;
	UINT16 devAddr;
	UINT16 regVal;
	
	devAddr = phyAddr;
	for(regAddr = PHY_BMCR; regAddr < 31;regAddr++)
	{
		if(regAddr >= 11 && regAddr <= 14)
		{
			continue;
		}
		if(regAddr >= 22 && regAddr <=23)
		{
			continue;
		}
		if(regAddr >= 25 && regAddr <=31)
		{
			continue;
		}
		retVal = mdioRegisterRead(devAddr, regAddr, &regVal);
		if(OPL_OK != retVal)
		{
			break;
		}
		OPL_DRV_PRINTF(("regAddr:%02d regVal:%04x\n",regAddr,regVal));
	}
	return retVal;
}

#endif

#ifdef PHY_TYPE_MARVELL

/* Basic Mode Control Register */
#define PHY_BMCR                0

/* Basic Mode Status Register */
#define PHY_BMSR                1

/* PHY Specific Status Register */
#define PHY_PHYSR               17


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

/* ******************************************************************************
 * speed,duplex,and auto-negotiation eanble take on the values set by hardware reset only
 * a write to the register has no effect unless any one of the following also ocuures:
 * 1.	software reset 0.15
 * 2.	restart auto-negotiation 0.9
 * 3.	transition from power up 0.11
*  4.	link goes down
********************************************************************************/
OPL_STATUS phySpeedHwWrite(UINT32 phySpeedMode)
{	
	OPL_STATUS retVal = OPL_OK;

	if(PHY_SPEED_1000M < phySpeedMode)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
    PHY_POWER_DOWN
	switch(phySpeedMode)
	{
		case PHY_SPEED_10M:
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, CONTROL_REG,  6,1, 0);
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, CONTROL_REG, 13,1, 0);
			break;
		case PHY_SPEED_100M:
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, CONTROL_REG,  6,1, 0);
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, CONTROL_REG, 13,1, 1);
			break;
		case PHY_SPEED_1000M:
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, CONTROL_REG,  6,1, 1);
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, CONTROL_REG, 13,1, 0);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
    PHY_POWER_UP

	return retVal;
}

OPL_STATUS phySpeedHwRead(PHY_SPEED_MODE_e *phySpeedMode)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 highBit;
	UINT16 lowBit;
	UINT16 regVal;

	if(OPL_NULL == phySpeedMode)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, CONTROL_REG,  6,1, &highBit);
	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, CONTROL_REG,  13,1, &lowBit);

	regVal = ((highBit<<1)|(lowBit))&0x3;

	*phySpeedMode = regVal;

	return retVal;
}

OPL_STATUS phyDuplexHwWrite(PHY_DUPLEX_MODE_e phyDuplexMode)
{	
	OPL_STATUS retVal = OPL_OK;

	if(PHY_HALF_DUPLEX > phyDuplexMode ||PHY_FULL_DUPLEX < phyDuplexMode)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

    PHY_POWER_DOWN
	switch(phyDuplexMode)
	{
		case PHY_HALF_DUPLEX:
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, CONTROL_REG,  8,1, 0);
			break;
		case PHY_FULL_DUPLEX:
			retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, CONTROL_REG,  8,1, 1);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	PHY_POWER_UP
	return retVal;
}

OPL_STATUS phyDuplexHwRead(PHY_DUPLEX_MODE_e *phyDuplexMode)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;

	if(OPL_NULL == phyDuplexMode)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, CONTROL_REG,  8,1, &regVal);

	*phyDuplexMode = regVal;

	return retVal;
}

OPL_STATUS phyAutoNegHwWrite(UINT8 enable)
{
	UINT16 regVal;
	OPL_STATUS retVal = OPL_OK;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;
    PHY_POWER_DOWN
	retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, CONTROL_REG,  12,1, regVal);
    PHY_POWER_UP
	return retVal;
}
OPL_STATUS phyAutoNegHwRead(UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, CONTROL_REG,  12,1, &regVal);

	*enable = regVal;

	return retVal;
}

OPL_STATUS phyRestartAutoNeg(void)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, CONTROL_REG,  9,1, 1);

	return retVal;
}

OPL_STATUS phyLoopbackEnableHwWrite(UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	
	retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, 20,  14,1, regVal);

	return retVal;
}

OPL_STATUS phyLoopbackEnableHwRead(UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, 20,  14,1, &regVal);

	*enable = regVal;

	return retVal;
}

OPL_STATUS phyReset(void)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, CONTROL_REG,  15,1, 1);

	return retVal;
}

OPL_STATUS phyPowerDown(UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, CONTROL_REG,  11,1, enable);

	return retVal;
}

OPL_STATUS phyLinkStatusHwRead(UINT8 *linkStatus)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;

	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, PHY_SPEC_STATUS,  10,1, &regVal);

	*linkStatus = regVal;

	return retVal;
}

OPL_STATUS phyFlowControlEnHwWrite(UINT8 enable)
{
    OPL_STATUS retVal = OPL_OK;

    retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR, AUTO_NEG_AD,  10,1, enable);

    return retVal;
}

OPL_STATUS phyRegShow(UINT16 phyAddr)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regAddr;
	UINT16 devAddr;
	UINT16 regVal;
	
	devAddr = phyAddr;
	for(regAddr = CONTROL_REG; regAddr < 31;regAddr++)
	{
		if(regAddr >= 11 && regAddr <= 14)
		{
			continue;
		}
		if(regAddr >= 22 && regAddr <=23)
		{
			continue;
		}
		if(regAddr >= 25 && regAddr <=31)
		{
			continue;
		}
		retVal = mdioRegisterRead(devAddr, regAddr, &regVal);
		if(OPL_OK != retVal)
		{
			break;
		}
		OPL_DRV_PRINTF(("regAddr:%02d regVal:%04x\n",regAddr,regVal));
	}
	return retVal;
}


#endif



/* begin: add by xukang for bug 3265 */
OPL_STATUS phySpecificAutoRead(PHY_AUTONEG_DONE_e *phyAutonegDone)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;

	if(OPL_NULL == phyAutonegDone)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, PHY_BMSR,  5,1, &regVal);
	
	*phyAutonegDone = regVal;

	return retVal;
}

OPL_STATUS phySpecificSpeedRead(PHY_SPEED_MODE_e *phySpeed)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 highBit;
	UINT16 lowBit;
	UINT16 regVal;

	if(OPL_NULL == phySpeed)
	{
		return OPL_ERR_NULL_POINTER;
	}

    retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, PHY_PHYSR,  15, 1, &highBit);
	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, PHY_PHYSR,  14, 1, &lowBit);

    regVal = ((highBit<<1)|(lowBit))&0x3;
	
	*phySpeed = regVal;
	
	return retVal;
}

OPL_STATUS phySpecificDuplexRead(PHY_DUPLEX_MODE_e *phyDuplex)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 regVal;

	if(OPL_NULL == phyDuplex)
	{
		return OPL_ERR_NULL_POINTER;
	}

	retVal = mdioRegisterFieldRead(PHY_DEVICE_ADDR, PHY_PHYSR, 13, 1, &regVal);
	
	*phyDuplex = regVal;
	
	return retVal;
}
/* end: add by xukang for bug 3265 */
