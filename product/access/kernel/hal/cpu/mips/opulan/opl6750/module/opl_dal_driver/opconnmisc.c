#include "opconnmisc.h"
#include "phy.h"
#include "opconn_lib.h"


OPL_STATUS chipReset(void)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = oplRegFieldWrite(REG_GB_SOFTRST_ADN_DEVID, 31, 1, 1);

	return retVal;
}

OPL_STATUS chipAttrGet(UINT8 type,UINT32 *value)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;
	
	if(type > CHIP_FPGA_VER)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	if(OPL_NULL == value)
	{
		return OPL_ERR_NULL_POINTER;
	}

	switch(type)
	{
		case CHIP_PRO_REV:
			retVal = oplRegFieldRead(REG_GB_SOFTRST_ADN_DEVID, 0, 4, &regVal);
			break;
		case CHIP_PRO_ID:
			retVal = oplRegFieldRead(REG_GB_SOFTRST_ADN_DEVID, 4, 16, &regVal);
			break;
		case CHIP_FPGA_VER:
			retVal = oplRegFieldRead(REG_FPGA_VERSION, 0, 8, &regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	*value = regVal;
	return retVal;
}

OPL_STATUS chipInit(void)
{
	chipAttrGet(CHIP_PRO_REV, &gChipRevision);	
	return oplRegFieldWrite(REG_GB_CHIP_INIT, 0, 1, 1);
}

OPL_STATUS linkListEnable(void)
{
	return oplRegFieldWrite(REG_GB_LL_CTRL, 0, 1, 1);
}

OPL_STATUS respinInit(void)
{
	OPL_STATUS retVal = OPL_OK;

	if (IS_ONU_RESPIN)
	{
		//printk("\n--------- RESPIN FPGA Version ---------\n");
		/* fpga bug : FPGA will drop packets on PON port if IPG > 1 */
		retVal = oplRegWrite(REG_PMAC_CFG_MINIFG, 1);
		if(OPL_OK != retVal)
		{
			return retVal;
		}

		/* 
			MDIO Select : 1 : select GMII PHY
						0 : select SGMII PHY
		*/
		retVal = oplRegWrite(REG_MDIO_SELECT, 1);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
#ifdef ONU_RESPIN_A
#define PHY_ADD 0x1D
#define PHY_DAT 0x1E
#define PHY_DBG_REG5 0x05
#define PHY_RGMII_TXCLK_BIT 0x8
#define GPIO9_MUX_OFF 0x5
#define GPIO9_OFFSET 0x9
#define GPIO_OUTPUT_EN 0x1
	    UINT16 regVal;
	    retVal = oplRegWrite(REG_GMAC_MODE, 0x1);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
	    /*    set the RGMII RXDLL, TXDLL   */	
	   // retVal = oplRegWrite(REG_GMAC_RGMII_RXDLL_CFG, 0x00800002);    /*no need RX DLL */
	    retVal = oplRegWrite(REG_GMAC_RGMII_TXDLL_CFG, 0x00800002);
		
		/******** Respin connect with AR8035 through RGMII, set Phy delay ********/
		retVal = mdioRegisterWrite(PHY_DEVICE_ADDR, PHY_ADD,PHY_DBG_REG5);
		retVal = mdioRegisterFieldWrite(PHY_DEVICE_ADDR,PHY_DAT,PHY_RGMII_TXCLK_BIT,1,1);

		/********240 pin single port Set GPIO9 to high for optical module TX power control ******/
		oplRegFieldWrite(REG_GPIO_MUX,GPIO9_MUX_OFF,1,0);
		oplRegFieldWrite(REG_GPIO_DIRECTION,GPIO9_OFFSET,1,GPIO_OUTPUT_EN);
		oplRegFieldWrite(REG_GPIO_SET,GPIO9_OFFSET,1,1);
#endif 

#ifdef ONU_RESPIN_A_4PORT
#define GPIO9_MUX_OFF 0x5
#define GPIO9_OFFSET 0x9
#define GPIO_OUTPUT_EN 0x1

	       /********240 pin single port Set GPIO9 to high for optical module TX power control ******/
		oplRegFieldWrite(REG_GPIO_MUX,GPIO9_MUX_OFF,1,0);
		oplRegFieldWrite(REG_GPIO_DIRECTION,GPIO9_OFFSET,1,GPIO_OUTPUT_EN);
		oplRegFieldWrite(REG_GPIO_SET,GPIO9_OFFSET,1,1);  
#endif
		/* cpdma mac enable : */
		retVal = oplRegWrite(REG_DMA0_EN, 0x3);
		if(OPL_OK != retVal)
		{
			return retVal;
		}


		/* FEC IPG : default(0x20) is too large for nofec mode */
		retVal = oplRegWrite(REG_MPCP_PKT_OVERHEAD, 0xc000f);
		if(OPL_OK != retVal)
		{
			return retVal;
		}

		/*   bit 31 : 
				tm module caclulated       : not accurate, report threshold when larger then threshold
				repore mudole callculated : accurate, removed in onu 1e.
			bit 0-6   : non fec time : 42TQ = 84B = 64B PayLoad + 12 IPG + 8 Preamble
			bit 7-13 : fec time4 : 58TQ = 116B = NON_FEC_TIME + 32 (4 S_FEC + [16 FEC Parity + 6 T_FEC_E]  + 6 T_FEC_E)
		*/
		oplRegWrite(REG_MPCP_DBA_AGENT_CFG,0x074a9d2a);

		/* 4: ,  bug 3255
		    3: delay of <laser on> and <synctime> when tx, 
		    2:

		    0x20 : tx delay, same with lasyer on (cause asic err, should be laser on -8 = 32 - 8 = 24).
		    will cause pmc oam can't register.
		    oplRegWrite(0x4003*4,0x40003018);
		*/

		/* bug 3255 : laser off decreased to resovle ping delay */
		//oplRegWrite(REG_MPCP_GATE_LASER_OFF, 0x18);	
		oplRegWrite(REG_MPCP_GATE_LASER_ON, 0x1c);		
		oplRegWrite(REG_MPCP_GATE_LASER_OFF, 0x20);	/*0x1c-->0x20*/

		/* 
		   8: asic internerl pre time indicator for windows.
		   A: mpcp start time before grant, shouldn't confict with last grant time, will cause traffic down 
		*/
		oplRegWrite(REG_MPCP_SCH_CFG, 0x8000000A);

		/* mpcp control laser on delay, asic internel delay between mpcp and laser */
		oplRegWrite(REG_MPCP_LASERON_DLY, 0x80080008);

		/* laser on control : output enable, window control by mpcp or data*/
		oplRegWrite(REG_PMAC_LASER_ON_CFG, 0x15);	

		/* us queue shaper enable : us queue cir pir shaper enable*/
		oplRegWrite(REG_TMUS_SHAPER_EN, 0x000000ff);
	}

	//spiInit(0);
	return retVal;
}	

OPL_STATUS gmacInit(void)
{
	OPL_STATUS retVal = OPL_OK;

#if 0  /* deleted by Gan Zhiheng - 2010/08/29 */
	/* gmac rx/tx enable*/

	retVal = oplRegFieldWrite(REG_GMAC_CFG_TRANSFER_ENA,0, 2, 0X03);

	if(OPL_OK != retVal)
	{
		return retVal;
	}
#endif /* #if 0 */

	/* gmac rx/tx pause enable*/

	/*bwei modify default tx DISABLE,10.05.17*/
	//retVal = oplRegFieldWrite(REG_GMAC_CFG_PAUSE_ENA,0, 2, 0X03);
	retVal = oplRegFieldWrite(REG_GMAC_CFG_PAUSE_ENA,0, 2, 0X01);

	if(OPL_OK != retVal)
	{
		return retVal;
	}

	return retVal;
}

OPL_STATUS pmacInit(void)
{
	OPL_STATUS retVal = OPL_OK;

#if 0  /* deleted by Gan Zhiheng - 2010/08/29 */
	/* gmac rx/tx enable*/

	retVal = oplRegFieldWrite(REG_PMAC_CFG_TRANSFER_ENA,0, 2, 0X03);

	if(OPL_OK != retVal)
	{
		return retVal;
	}
#endif /* #if 0 */

	/* gmac rx/tx pause enable*/

	retVal = oplRegFieldWrite(REG_PMAC_CFG_PAUSE_ENA,0, 2, 0X03);

	if(OPL_OK != retVal)
	{
		return retVal;
	}

	return retVal;
}


