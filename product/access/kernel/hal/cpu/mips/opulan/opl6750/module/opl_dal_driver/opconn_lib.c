/*
=============================================================================
     Header Name: opconn.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/10/20		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"
#include "opconn_lib.h"
#include "uart.h"

UINT32 gChipRevision = ONU_OPCONN;

OPL_STATUS oplDriverInit(void)
{
	OPL_STATUS retVal = OPL_OK;

    retVal = oplHwAcessInit();
    if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init hardware access failed.\n"));
		return retVal;
	}
	OPL_DRV_PRINTF(("init hardware access done.\n"));

	retVal = chipInit();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("chipInit.\n"));
		return retVal;
	}
	OPL_DRV_PRINTF(("chip init done.\n"));

	retVal = linkListEnable();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("linkListEnable.\n"));
		return retVal;
	}
	OPL_DRV_PRINTF(("linkListEnable done.\n"));

	retVal = clsInit();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init cls table failed.\n"));
		return retVal;
	}
	OPL_DRV_PRINTF(("init cls table done.\n"));

	retVal = brgArlInit();
	if(retVal != OPL_OK)
	{
		OPL_DRV_PRINTF(("init arl table failed.\n"));
		return retVal;
	}
	OPL_DRV_PRINTF(("init arl table done.\n"));

	retVal = brgVttInit();
	if(retVal != OPL_OK)
	{
		OPL_DRV_PRINTF(("init vtt table failed.\n"));
		return retVal;
	}
	OPL_DRV_PRINTF(("init vtt table done.\n"));

	retVal = brgPortInit();
	if(retVal != OPL_OK)
	{
		OPL_DRV_PRINTF(("init port table failed.\n"));
		return retVal;
	}
	OPL_DRV_PRINTF(("init port table done.\n"));

	retVal = tmInit();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init tm failed.\n"));
		return retVal;
	}
	OPL_DRV_PRINTF(("init tm done.\n"));
	
	/*retVal = eopl_dma_init();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init dma failed.\n"));
		return retVal;
	}*/

	retVal = gmacInit();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init gmac failed.\n"));
		return retVal;
	}
	OPL_DRV_PRINTF(("init gmac done.\n"));
	
	retVal = pmacInit();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init pmac failed.\n"));
		return retVal;
	}
	OPL_DRV_PRINTF(("init pmac done.\n"));

	/*init reserved mac control*/
	retVal  = brgRsvMacCtrlInit();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("rsvmac control init failed.\n"));
		return retVal;
	}
	OPL_DRV_PRINTF(("init rsvmac control done.\n"));
	
    retVal  = brgZeroMacDropInit();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init zero mac packets dropping failed.\n"));
		return retVal;
	}
	OPL_DRV_PRINTF(("init zero mac packets dropping done.\n"));
    /*begin added by liaohongjun 2012/12/25 of EPN204QID0019*/
    retVal  = Uart1Init();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init uart1 failed.\n"));
		return retVal;
	}
	OPL_DRV_PRINTF(("init uart1 done.\n"));
    /*end added by liaohongjun 2012/12/25 of EPN204QID0019*/
	return retVal;
}

