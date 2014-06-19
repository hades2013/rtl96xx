/*
=============================================================================
     File Name: dal_lib.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/20		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#include "dal_lib.h"

extern UINT8 oam_src_mac[6];

extern void dalCpuClsInit(void);

#define POLICING_LINE_RATE_ENABLE 1
#define SYSTEM_CLOCK_125M 1

OPL_STATUS dalChipReset()
{
    return chipReset();
}

OPL_STATUS dalDemonInit(void)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = oplDriverInit();
	if(OPL_OK != retVal)
	{
		OPL_DAL_PRINTF(("init oplDriverInit failed.\n"));
		return retVal;
	}
	OPL_DAL_PRINTF(("init oplDriverInit done.\n"));

	retVal = dalClsInit();
	if(OPL_OK != retVal)
	{
		OPL_DAL_PRINTF(("init dalClsInit failed.\n"));
		return retVal;
	}
	OPL_DAL_PRINTF(("init dalClsInit done.\n"));

	retVal = dalPortInit();
	if(OPL_OK != retVal)
	{
		OPL_DAL_PRINTF(("init dalPort failed.\n"));
		return retVal;
	}
	OPL_DAL_PRINTF(("init dalPort done.\n"));
	
	retVal = dalVttInit();
	if(OPL_OK != retVal)
	{
		OPL_DAL_PRINTF(("init dalVttInit failed.\n"));
		return retVal;
	}
	OPL_DAL_PRINTF(("init dalVttInit done.\n"));

	retVal = dalArlFlushAll();
	if(OPL_OK != retVal)
	{
		OPL_DAL_PRINTF(("init dalArlFlushAll failed.\n"));
		return retVal;
	}
	OPL_DAL_PRINTF(("init dalArlFlushAll done.\n"));

	retVal = dalPonInit();
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	OPL_DAL_PRINTF(("init dalPonInit done.\n"));

#if POLICING_LINE_RATE_ENABLE
	/* Modified shaper offset:20=8premble+12IPG */
    retVal = oplRegWrite(REG_TMDS_SHAPER_OFFSET, 0x14);
    if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMUS_SHAPER_OFFSET, 0x14);
    if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegFieldWrite(REG_GE_PARSER_STEP_INC, 8, 8, 0x14);
	if(OPL_OK != retVal){
		return retVal;
	}
#endif

#if SYSTEM_CLOCK_125M	/*for 125MHz system clock */
	retVal = oplRegFieldWrite(REG_GE_PARSER_STEP_INC, 0, 8, 0x32);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMUS_CLKNUM_PERIOD, 0x0480);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMDS_CLKNUM_PERIOD, 0x0480);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMUS_STEP_CNT, 0x3C);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMDS_STEP_CNT, 0x3C);
	if(OPL_OK != retVal){
		return retVal;
	}
#else
	retVal = oplRegFieldWrite(REG_GE_PARSER_STEP_INC, 0, 8, 0x30);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMUS_CLKNUM_PERIOD, 0x7880);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMDS_CLKNUM_PERIOD, 0x7880);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMUS_STEP_CNT, 0x3F);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMDS_STEP_CNT, 0x3F);
	if(OPL_OK != retVal){
		return retVal;
	}
#endif

    /* Copy broadcast arp/dhcp req and ack to cpu. */
    dalCpuClsInit();

    /*Added of I2C init */
    i2cInit(I2C_MODE_MASTER, DISABLE, 0, I2C_DEFAULT_RATE);
    #if (CONFIG_BOSA)
    dal_I2cPonMonitorInit();
    #endif
	/* Begin Added */
	oplRegFieldWrite(REG_MARB_PRIO_CFG, 0, 18, 0x818);
	
	OPL_DAL_PRINTF(("dal demon init done.\n"));

	return OPL_OK;
}

void *
vosMemCpy(void *dst_void, const void *src_void, size_t len)
{
    unsigned char *dst = dst_void;
    const unsigned char *src = src_void;

    while (len--) {
    *dst++ = *src++;
    }

    return dst_void;
}

void *
vosMemSet(void *dst_void, int val, size_t len)
{
    unsigned char *dst = dst_void;

    while (len--) {
    *dst++ = (unsigned char) val;
    }

    return dst_void;
}


