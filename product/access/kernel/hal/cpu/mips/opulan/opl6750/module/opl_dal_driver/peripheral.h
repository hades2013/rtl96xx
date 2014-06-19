/*
=============================================================================
Header file Name:peripheral.c

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
#ifndef PERIPHERAL_H
#define PERIPHERAL_H
#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"
#include "hw_interface.h"

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
OPL_STATUS mdioRegisterRead(UINT8 deviceAddr,UINT8 regAddr,UINT16 *data0);

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
OPL_STATUS mdioRegisterWrite(UINT8 deviceAddr,UINT8 regAddr,UINT16 data0);

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
OPL_STATUS mdioRegisterFieldRead(UINT8 deviceAddr,UINT8 regAddr,UINT8 offset,UINT8 width,UINT16 *data0);

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
OPL_STATUS mdioRegisterFieldWrite(UINT8 deviceAddr,UINT8 regAddr,UINT8 offset,UINT8 width,UINT16 data0);


OPL_STATUS mdioRegisterInit(void);

/* Begin Added*/
OPL_STATUS MdioSwGlobalRegRead( UINT32 regAddr, UINT32 * regVal );
OPL_STATUS MdioSwGlobalRegWrite( UINT32 regAddr, UINT32 regVal );
/* End   Added of porting */

#endif
