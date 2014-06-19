/*
=============================================================================
File Name:peripheral.c

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
		zzhu			2007/11/1		Initial Version
		---------------	---------------	----------------------------------------------
*/

#include <linux/mutex.h>

#include "peripheral.h"


#define MDIO_REG_FIELD_READ(regAddr,fieldOffset,fieldWidth,data0) { \
	oplRegRead(regAddr, &(data0)); \
	data0 = ((data0)&((~(0XFFFFFFFF<<(fieldWidth)))<<(fieldOffset)))>>(fieldOffset); \
}

#define MDIO_REG_FIELD_WRITE(regAddr,fieldOffset,fieldWidth,data0) { \
	UINT32 oldVal,fieldMask; \
	fieldMask = (~(0XFFFFFFFF<<(fieldWidth)))<<(fieldOffset); \
	oplRegRead(regAddr,&oldVal); \
	oplRegWrite(regAddr, (((data0)<<(fieldOffset))&fieldMask)|(oldVal&(~fieldMask))); \
}

struct mutex		g_pstMidoAccessLock;

#define OPL_MDIO_ACCESS_LOCK mutex_lock(&g_pstMidoAccessLock)
#define OPL_MDIO_ACCESS_UNLOCK mutex_unlock(&g_pstMidoAccessLock)

//int g_nDisableMDIO = 0;


OPL_STATUS mdioRegisterInit()
{
	mutex_init(&g_pstMidoAccessLock);
	return OPL_OK;
}


/*******************************************************************************
* mdioRegisterRead
*
* DESCRIPTION:
*  		this function is usedt to read mdio data from a register.
*
*	INPUTS:
*			deviceAddr	: phy address (0-31)
*			regAddr		: reg address  (0-31)
*			
*	OUTPUTS:
*		   data0			: data buffer for read out data
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/


OPL_STATUS mdioRegisterRead(UINT8 deviceAddr,UINT8 regAddr,UINT16 *data0)
{
	UINT32 mdioBusy;
	UINT32 timeOut = MDIO_TIMEOUT;
	UINT32 regVal = 0;
	OPL_STATUS retVal = OPL_OK;

    OPL_MDIO_ACCESS_LOCK;

#if defined(SWITCH_TEST)
    /* added by zgan - 2009/10/22, test AR8228 reset issue  */
    if (g_nDisableMDIO)
    {
        goto exit_label;
    }
#endif

    /* first check that it is not busy */
	MDIO_REG_FIELD_READ(REG_MDIO_BUSY, 0, 1, mdioBusy);
	while(mdioBusy&MDIO_BUSY){
		if(!timeOut--){
			OPL_TRACE();
			retVal = OPL_ERROR;
            goto exit_label;
		}
		MDIO_REG_FIELD_READ(REG_MDIO_BUSY, 0, 1, mdioBusy);
	}
	
	MDIO_REG_FIELD_WRITE(REG_MDIO_PHYAD,0,5,deviceAddr&0x1f);
	MDIO_REG_FIELD_WRITE(REG_MDIO_REGAD, 0, 5, regAddr&0x1f);
	MDIO_REG_FIELD_WRITE(REG_MDIO_OP_PULSE, 0, 2, MDIO_READ);
	MDIO_REG_FIELD_READ(REG_MDIO_BUSY, 0, 1, mdioBusy);
	timeOut = MDIO_TIMEOUT;
	while(mdioBusy&MDIO_BUSY){
		if(!timeOut--){
			OPL_TRACE();
			retVal = OPL_ERROR;
            goto exit_label;
		}
		MDIO_REG_FIELD_READ(REG_MDIO_BUSY, 0, 1, mdioBusy);
	}

	MDIO_REG_FIELD_READ(REG_MDIO_READ_DATA, 0, 16, regVal);
	
exit_label:
	*data0 = regVal;
    OPL_MDIO_ACCESS_UNLOCK;
	return retVal;
}
/*******************************************************************************
* mdioRegisterWrite
*
* DESCRIPTION:
*  		this function is usedt to write mdio data to a register.
*
*	INPUTS:
*			deviceAddr	: phy address (0-31)
*			regAddr		: reg address  (0-31)
*			data0			: value to be write
*	OUTPUTS:
*		   
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS mdioRegisterWrite(UINT8 deviceAddr,UINT8 regAddr,UINT16 data0)
{
	UINT32 mdioBusy;
	UINT32 timeOut = MDIO_TIMEOUT;
	OPL_STATUS retVal = OPL_OK;

    OPL_MDIO_ACCESS_LOCK;

#if defined(SWITCH_TEST)
    /* added by zgan - 2009/10/22, test AR8228 reset issue  */
    if (g_nDisableMDIO)
    {
        goto exit_label;
    }
#endif

	MDIO_REG_FIELD_READ(REG_MDIO_BUSY,0, 1, mdioBusy);
	while(MDIO_BUSY&mdioBusy){
		if(!timeOut--){
			OPL_TRACE();
			retVal = OPL_ERROR;
            goto exit_label;
		}
		MDIO_REG_FIELD_READ(REG_MDIO_BUSY,0, 1, mdioBusy);
	}

	MDIO_REG_FIELD_WRITE(REG_MDIO_PHYAD, 0, 5, deviceAddr&0x1f);
	MDIO_REG_FIELD_WRITE(REG_MDIO_REGAD, 0, 5, regAddr&0x1f);
	MDIO_REG_FIELD_WRITE(REG_MDIO_WRITE_DATA, 0, 16, data0);
	MDIO_REG_FIELD_WRITE(REG_MDIO_OP_PULSE, 0, 2, MDIO_WRITE);

exit_label:	
    OPL_MDIO_ACCESS_UNLOCK;
	return retVal;	
}
/*******************************************************************************
* mdioRegisterFieldRead
*
* DESCRIPTION:
*  		this function is usedt to read a bitfied from specified offset of a register.
*
*	INPUTS:
*			deviceAddr	: phy address (0-31)
*			regAddr		: reg address  (0-31)
*			offset			: the offset of the start bit of the bitfield (0-31).
*			width			: the width of data (0-31)
*	OUTPUTS:
*		   data0			: the data read out.
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS mdioRegisterFieldRead(UINT8 deviceAddr,UINT8 regAddr,UINT8 offset,UINT8 width,UINT16 *data0)
{
	UINT16 fieldMask;
	UINT16 regVal;
	OPL_STATUS retVal;

#if defined(SWITCH_TEST)
    /* added by zgan - 2009/10/22, test AR8228 reset issue  */
    if (g_nDisableMDIO)
    {
        return OPL_ERROR;
    }
#endif

	if(offset > 15 || width < 1 || width > 16||((offset + width)>16))
	{
		OPL_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(OPL_NULL == data0)
	{
		OPL_TRACE();
		return OPL_ERR_NULL_POINTER;
	}

	if(width == 16)
	{
		fieldMask = 0xffff;
	}else
	{
		fieldMask = (~(0xffff<<width))<<offset;		
	}

	retVal = mdioRegisterRead( deviceAddr,  regAddr, &regVal);
	#ifdef OPL_DEBUG
	OPL_PERIPHERAL_LOG("mask = %04x\n",fieldMask);
	OPL_PERIPHERAL_LOG("regVal = %04x\n",regVal);
	#endif
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	regVal = regVal&fieldMask;
	*data0 = regVal>>offset;
	return retVal;
}
/*******************************************************************************
* mdioRegisterFieldWrite
*
* DESCRIPTION:
*  		this function is usedt to write a bitfied to specified offset of a register.
*
*	INPUTS:
*			deviceAddr	: phy address (0-31)
*			regAddr		: reg address  (0-31)
*			offset			: the offset of the start bit of the bitfield (0-31).
*			width			: the width of data (0-31)
*			 data0			: the data to be writed.
*	OUTPUTS:
*		  
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS mdioRegisterFieldWrite(UINT8 deviceAddr,UINT8 regAddr,UINT8 offset,UINT8 width,UINT16 data0)
{
	UINT16 fieldMask;
	UINT16 regVal;
	OPL_STATUS retVal;

#if defined(SWITCH_TEST)
    /* added by zgan - 2009/10/22, test AR8228 reset issue  */
    if (g_nDisableMDIO)
    {
        return OPL_ERROR;
    }
#endif

	if(offset > 15 || width < 1 || width > 16||((offset + width)>16))
	{
		OPL_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(width == 16)
	{
		fieldMask = 0xffff;
	}else
	{
		fieldMask = (~(0xffff<<width))<<offset;		
	}

	retVal = mdioRegisterRead( deviceAddr,  regAddr, &regVal);
	#ifdef OPL_DEBUG
	OPL_PERIPHERAL_LOG("mask = %04x\n",fieldMask);
	OPL_PERIPHERAL_LOG("regVal = %04x\n",regVal);
	#endif

	if(OPL_OK != retVal)
	{
		return retVal;
	}
	regVal = regVal&~fieldMask;
	regVal = regVal|((data0<<offset)&fieldMask);
	#ifdef OPL_DEBUG
	OPL_PERIPHERAL_LOG("regVal = %04x\n",regVal);
	#endif
	retVal = mdioRegisterWrite( deviceAddr,  regAddr,  regVal);
	return retVal;
}

