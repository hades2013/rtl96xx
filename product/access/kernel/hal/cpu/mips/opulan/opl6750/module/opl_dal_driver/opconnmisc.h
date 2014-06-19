/*
=============================================================================
Header Name:opconnmisc.h

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

#ifndef OPCONNMISC_H
#define OPCONNMISC_H
#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"
#include "hw_interface.h"

typedef enum OPCONN_CHIP_ATTR_s
{
	CHIP_PRO_REV,
	CHIP_PRO_ID,
	CHIP_FPGA_VER
}OPCONN_CHIP_ATTR_e;

OPL_STATUS chipReset(void);

OPL_STATUS chipAttrGet(UINT8 type,UINT32 *value);

OPL_STATUS chipInit(void);

OPL_STATUS respinInit(void);

OPL_STATUS linkListEnable(void);

OPL_STATUS gmacInit(void);

OPL_STATUS pmacInit(void);
#endif


