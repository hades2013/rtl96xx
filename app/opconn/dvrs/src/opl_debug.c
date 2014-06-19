/*************************************************************************
*
*  COPYRIGHT (C) 2003-2007 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential                       
*
* 	This software is made available only to customers and prospective        
* 	customers of Opulan Technologies Corporation under license and may be
*	 used only with Opulan semi-conductor products.                           
*
* FILENAME: opl_debug.c
*
* DESCRIPTION: Provided implementation of functions, global variables
*	to operate debug control and trace tools.
*
* Date Created: 24/05/2007
*
* Authors(optional): yzhang
*
* Reviewed by (optional):
**************************************************************************/
/***** INCLUDE FILES *****/
#include "opl_driver.h"
#include "opl_debug.h"
#include "opl_errno.h"

OPL_BOOL gbPrtDbg 		= OPL_FALSE;
OPL_BOOL gbLogDbg		= OPL_TRUE;
OPL_BOOL gbClsDbg		= OPL_FALSE;
OPL_BOOL gbVttDbg		= OPL_FALSE;


OPL_BOOL gbClsFileDbg = OPL_FALSE;

OPL_BOOL gbVttFileDbg = OPL_FALSE;

OPL_BOOL gbMcCtlDbg   = OPL_FALSE;
OPL_BOOL gbHostDbg   = OPL_FALSE;
/* begin modifieded by liaohongjun for DMA debug of QID0017*/
OPL_BOOL gbDmaDbg   = OPL_FALSE;
/* end modifieded by jiangmingli for DMA debug of QID0017*/

/* begin added by jiangmingli for host debug */
OPL_BOOL g_bHostDsDbgEn = OPL_FALSE;
OPL_BOOL g_bHostUsDbgEn = OPL_FALSE;
/* end added by jiangmingli for host debug */
 
#ifdef OPL_DRV_DEBUG

/*******************************************************************************
*
* OPL_DUMP - Controlled memory dump function.
*
* DESCRIPTION:
* 	This function display memory in both hexadecimal and char format byte by byte 
* followd the caption specified, it is controlled by the Driver Level switch variable.
*
* INPUTS:
* nModId          - the ID for Driver Level, Module Level variables, see definitions
*                   followed. 0 is for help using, display the usage of this function and the current 
*                   status of output switch variables.logEn, the value returned.
* pFromAddr       - Start address to dump. 
* nLen            - Length to dump(in byte). 
* pCaption        - caption outpu.
*
* OUTPUT:
* n/a.
*
* RETURNS:
* n/a.
*
* SIDE AFFECTION: 
*
* SEE ALSO: 
*/
void OPL_DUMP(UINT32 *pModId, const void *pFromAddr, UINT32 uLen, const UINT8 *pCaption)
{
	return ;
}

/*******************************************************************************
*
* oplLogGet - Set the value of output switch variables in D&T Module. 
*
* DESCRIPTION:
* 	This function sets the value of output switch variables in D&T Module.
*
* INPUTS:
* nModId          - the ID for Driver Level, Module Level variables, see definitions
*                   followed. 0 is for help using, display the usage of this function
*                   and the current status of output switch variables.logEn, the value 
*                   returned.
* 
* OUTPUT:
* logEn           - OPL_TRUE, enable log.
*                   OPL_FALSE, disable log. 
*
* RETURNS:
* OPL_OK, Get succeed.
* OPL_ERROR, Get failed.
*
* SIDE AFFECTION: 
*
* SEE ALSO: 
*/
OPL_STATUS oplLogSet(UINT32 nModId, OPL_BOOL logEn)
{
	return OPL_OK;
}

/*******************************************************************************
*
* oplLogGet - Get the value of output switch variables in D&T Module. 
*
* DESCRIPTION:
* 	This function gets the value of output switch variables in D&T Module.
*
* INPUTS:
* nModId          - the ID for Driver Level, Module Level variables, see definitions
*                   followed. 
* 
* OUTPUT:
* logEn           - OPL_TRUE, enable log.
*                   OPL_FALSE, disable log. 
*
* RETURNS:
* OPL_OK, Get succeed.
* OPL_ERROR, Get failed.
*
* SIDE AFFECTION: 
*
* SEE ALSO: 
*/
OPL_STATUS oplLogGet(UINT32 nModId, OPL_BOOL *logEn)
{
	return OPL_OK;
}

/*******************************************************************************
*
* oplLogShow - Show the value of output switch variables in D&T Module. 
*
* DESCRIPTION:
* 	This function shows the value of output switch variables in D&T Module.
*
* INPUTS:
* n/a.
*
* OUTPUT:
* n/a.
*
* RETURNS:
* n/a.
*
* SIDE AFFECTION: 
*
* SEE ALSO: 
*/
void oplLogShow(void)
{
	return ;
}

/*******************************************************************************
*
* ohelp - Overall help. 
*
* DESCRIPTION:
* 	This function show the debug and trace tools provided by the driver system. 
* The tools including function available, module show routine. 
*
* INPUTS:
* nHow, provide a parameter reserved for future using. Any way, input 0 as 
* parameter is stick to print help information. 
*
* OUTPUT:
* n/a.
*
* RETURNS:
* n/a.
*
* SIDE AFFECTION: 
*
* SEE ALSO: 
*/
void ohelp(INT32 nHow)
{
	OPL_DRV_PRINTF(("======module help======\n"));
}

void oplLogTest(void)
{
	UINT32 mod = 10;
	UINT32 filePointer = 10;
	
	OPL_LOG(&mod, &filePointer, "file %s, function %s, line %d!\n",__FILE__,__FUNCTION__,__LINE__);

	return;
}
#else /*_OPL_DRV_DEBUG_*/

/*******************************************************************************
*
* OPL_DUMP - NULL dump function . 
*
* DESCRIPTION:
* 	This function is a NULL function, does nothing. 
*
* INPUTS:
*
*
* OUTPUT:
* n/a.
*
* RETURNS:
* n/a.
*
* SIDE AFFECTION: 
*
* SEE ALSO: 
*/
void OPL_DUMP(UINT32 *pModId, const void *pFromAddr, UINT32 uLen, const UINT8 *pCaption)
{
	return ;
}

/*******************************************************************************
*
* ohelp - NULL help function . 
*
* DESCRIPTION:
* 	This function is a NULL function, does nothing. 
*
* INPUTS:
*
*
* OUTPUT:
* n/a.
*
* RETURNS:
* n/a.
*
* SIDE AFFECTION: 
*
* SEE ALSO: 
*/
void ohelp(int nHow)
{
	/* Do thing */
}

void oplLogTest(void)
{
	UINT32 mod = 10;
	UINT32 filePointer = 10;
	
	OPL_LOG(&mod, &filePointer, "file %s, function %s, line %d!\n",__FILE__,__FUNCTION__,__LINE__);

	return OPL_OK;
}
#endif /* _OPL_DRV_DEBUG_ */

/* END FILE */
