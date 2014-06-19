/*
=============================================================================
Head Name:dal_ver.h

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
		zgan			2008/08/14		modified file name to dal_ver
		---------------	---------------	----------------------------------------------
*/
#ifndef	 VERSION_H
#define VERSION_H

#include "opl_driver.h"
#include "opconn_api.h"

#define OPCONN_VER_STR			"opconn:4.4"
#define OPCONN_VER_NUM			4.4

OPL_STATUS oplVerStrGet(UINT8 *pString);

#if 0/*implement in user space*/

OPL_STATUS oplVerNumGet(FLOAT32 *verNum);
#endif
OPL_STATUS oplVerShow(void);
#endif
