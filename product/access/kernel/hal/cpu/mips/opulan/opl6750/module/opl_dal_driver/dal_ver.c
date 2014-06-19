/*
=============================================================================
File Name:dal_ver.c

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
#include "dal_ver.h"

OPL_STATUS oplVerStrGet(UINT8 *pString)
{
	if(OPL_NULL != pString)
	{
		OPL_STRCPY(pString,OPCONN_VER_STR);
		return OPL_OK;
	}else 
	{
		return OPL_ERR_NULL_POINTER;
	}
}

#if 0/*implement in user space*/
OPL_STATUS oplVerNumGet(FLOAT32  *verNum)
{
	if(OPL_NULL != verNum)
	{
		*verNum = OPCONN_VER_NUM;
		return OPL_OK;
	}else 
	{
		return OPL_ERR_NULL_POINTER;
	}
}
#endif
OPL_STATUS oplVerShow(void)
{
	OPL_DAL_PRINTF(("%s\n",OPCONN_VER_STR));
	OPL_DAL_PRINTF(("opconn:%f\n",OPCONN_VER_NUM));
	return OPL_OK;
}

