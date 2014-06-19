/******************************************************************************/
/**  COPYRIGHT (C) 2003-2103 Opulan Technologies Corp. ALL RIGHTS RESERVED.  **/
/**--------------------------------------------------------------------------**/
/**                       Proprietary and Confidential                       **/
/**                                                                          **/
/** This software is made available only to customers and prospective        **/
/** customers of Opulan Technologies Corporation under license and may be    **/
/** used only with Opulan semi-conductor products.                           **/
/**--------------------------------------------------------------------------**/
/*******************************************************************************
**    MODULE     :  WDT.
**
**    FILE       :  wdt.c
**
**    DESCRIPTION:  this file includes the routines that read and accumulate,clear,show WDT register counter values.
**
**
**    NOTES      :
**
*******************************************************************************/
/*
**    MODIFICATION HISTORY:
**
**    Rev #    Date         Author         Description
**    -----    ----------   ------------   ------------------------------------
**	                    
**
**
*/
/* includes */
#include "wdt.h"
#include "opconn_hw_reg.h"
#include "opconn_usr_ioctrl.h"

/*******************************************************************************
*
* oplWdtCurtimeGet
*
* DESCRIPTION:
* 	This function get the current value of Watchdog timer value.
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
*	The current value of Watchdog timer value.
*
* SEE ALSO:
*
*/
OPL_STATUS oplWdtCurtimeGet (UINT32* value)
{
	OPL_STATUS retVal;

	retVal = oplRegRead(REG_WATCHDOG_VALUE, value);

	return retVal;
}

/*******************************************************************************
*
* oplWdtEnable
*
* DESCRIPTION:
* 	This function enable watch dog timer.
*
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
*	n/a.
*
* SEE ALSO:
*
*/
OPL_STATUS oplWdtEnable (UINT32 enable)
{
	OPL_STATUS ret;
	
	ret = oplRegFieldWrite(REG_WATCHDOG_CFG, 0, 1, enable&0x1);

	return ret;
}


/*******************************************************************************
*
* oplWdtReset
*
* DESCRIPTION:
* 	This function enable/disable reset whole system when time out.
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
*	n/a.
*
* SEE ALSO:
*	oplWdtReset()
*
*/
OPL_STATUS oplWdtReset (UINT32 enable)
{
	OPL_STATUS ret;
	
	ret = oplRegFieldWrite(REG_WATCHDOG_CFG, 1, 1, enable&0x1);

	return ret;
}


/*******************************************************************************
*
* ipmuxWdtTimeout
*
* DESCRIPTION:
*	This function check if the watch dog timer is timeout.
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
*	1		- the watch dog timer timeout.
*	0		- the watch dog timer doesn't timeout.
*
* SEE ALSO:
*/
OPL_STATUS oplWdtTimeout(UINT32* timeout)
{
	OPL_STATUS ret;
	
	ret = oplRegFieldRead(REG_WATCHDOG_CFG, 2, 1, timeout);

	return ret;
}


/*******************************************************************************
*
* oplWdtDivSet
*
* DESCRIPTION:
* 	This function set the system clock divide scaler of watch dog timer.
*
* INPUTS:
*	divVal	- the system clock divide scaler of watch dog timer.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
*	n/a.
*
* SEE ALSO:
*	oplWdtDivSet()
*
*/
OPL_STATUS oplWdtDivSet(UINT32 divVal)
{
	OPL_STATUS ret;
	UINT32 value;
	
	ret = oplRegFieldWrite(REG_WATCHDOG_CFG, 16, 16, divVal&0xFFFF);

	return ret;
}

/*******************************************************************************
*
* oplWdtDivGet
*
* DESCRIPTION:
*	This function get the system clock divide scaler of watch dog timer.
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
*	the current clock divide scaler.
*
* SEE ALSO:
*	oplWdtDivGet()
*
*/
OPL_STATUS oplWdtDivGet(UINT32* divVal)
{
	OPL_STATUS ret;
	
	ret = oplRegFieldRead(REG_WATCHDOG_CFG, 16, 16, divVal);

	return ret;
}

/*******************************************************************************
*
* oplWdtFeed
*
* DESCRIPTION:
* 	This function generate the service sequence to reload the timer.
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
*	n/a.
*
* SEE ALSO:
*
*/
OPL_STATUS oplWdtFeed (void)
{
	OPL_STATUS ret;
	
	ret = oplRegFieldWrite(REG_WATCHDOG_FEED, 0, 8, 0xaa);
	ret += oplRegFieldWrite(REG_WATCHDOG_FEED, 0, 8, 0x55);

	return ret;
}

/*******************************************************************************
*
* oplWdtTimeoutSet
*
* DESCRIPTION:
* 	This function set the timerout value for watch dog timer.
*
* INPUTS:
*	timeoutValue	-	the watch dog timer time out value.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
*	n/a.
*
* SEE ALSO:
*/
OPL_STATUS oplWdtTimeoutSet (UINT32 timeoutValue)
{
	OPL_STATUS ret;
	
	ret = oplRegWrite(REG_WATCHDOG_MAX, timeoutValue);

	return ret;
}

/*******************************************************************************
*
* oplWdtTimeoutGet
*
* DESCRIPTION:
*	This function get the timerout value for watch dog timer.
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
*	the current timerout value.
*
* SEE ALSO:
*
*/
OPL_STATUS oplWdtTimeoutGet (UINT32* timeoutValue)
{
	OPL_STATUS ret;
	
	ret = oplRegRead(REG_WATCHDOG_MAX, timeoutValue);

	return ret;
}

/*******************************************************************************
*
* oplWdtInit
*
* DESCRIPTION:
* 	This function config the watch dog timer and start it.
*
* INPUTS:
*	divVal			- the system clock divide scaler of watch dog timer.
*	timeoutValue	- the watch dog timer time out value.
*	resetEnable		- reset the board if time out.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
*	n/a.
*
* SEE ALSO:
*/
OPL_STATUS oplWdtInit (UINT32 divVal, UINT32 timeoutValue, UINT32 resetEnable)
{
	oplWdtEnable (0);

	oplWdtDivSet (divVal);

	oplWdtTimeoutSet (timeoutValue);

	oplWdtReset (resetEnable);

	oplWdtEnable (1);

    /* start the wdt  */
	oplWdtFeed ();

	return OPL_OK;
}





