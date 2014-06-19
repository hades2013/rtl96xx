/******************************************************************************/
/**  COPYRIGHT (C) 2003- Opulan, INC. ALL RIGHTS RESERVED.                   **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to Opulan, Inc.                                      		 **/
/**--------------------------------------------------------------------------**/
/**                                                                          **/
/**                             Revision History                             **/
/**--------------------------------------------------------------------------**/
/**	Rev #       Date		         Author		Description                  **/
/**	-----       ------		-------		-----------                          **/
/**	                    													 **/
/******************************************************************************/

#ifndef __OPL_WDT_H__
#define __OPL_WDT_H__
#include "opl_driver.h"


/* function forward declaration */
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
OPL_STATUS oplWdtCurtimeGet (UINT32* value);


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
OPL_STATUS oplWdtEnable (UINT32 enable);

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
OPL_STATUS oplWdtReset (UINT32 enable);


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
OPL_STATUS oplWdtTimeout(UINT32* timeout);

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
OPL_STATUS oplWdtDivSet(UINT32 divVal);


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
OPL_STATUS oplWdtDivGet(UINT32* divVal);

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
OPL_STATUS oplWdtFeed (void);

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
OPL_STATUS oplWdtTimeoutSet (UINT32 timeoutValue);

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
OPL_STATUS oplWdtTimeoutGet (UINT32* timeoutValue);

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
OPL_STATUS oplWdtInit (UINT32 divVal, UINT32 timeoutValue, UINT32 resetEnable);



/*---------------------------- End of ipmux_wdt.h -------------------------------*/
#endif
