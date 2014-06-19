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
* FILENAME: opl_debug.h
*
* DESCRIPTION: Opulan privated defined debug control and trace tools.
*	the debug and trace tools mainly including:
*		Controlled output definitions for debug and trace information
*		Switch variables to switch on/off the output
*		Functions to operation the Switch Varialbe.
*		Functions to operation error codes
*		Error Codes for driver
*
* Date Created: 05/19/2007
*
* Authors(optional): yzhang
*
* Reviewed by (optional):
*
**************************************************************************/
#ifndef OPL_DEBUG_H
#define OPL_DEBUG_H

/***** INCLUDE FILES *****/
#include "opl_driver.h"
//#include <stdio.h>
//#include <stdlib.h>

extern OPL_BOOL gbPrtDbg;
extern OPL_BOOL gbLogDbg;
extern OPL_BOOL gbClsDbg;
extern OPL_BOOL gbVttDbg;

extern OPL_BOOL gbClsFileDbg;
extern OPL_BOOL gbVttFileDbg;

extern OPL_BOOL gbMcCtlDbg;
extern OPL_BOOL gbHostDbg;
extern OPL_BOOL gbDmaDbg;

/* begin added by jiangmingli for host debug */
extern OPL_BOOL g_bHostDsDbgEn;
extern OPL_BOOL g_bHostUsDbgEn;
/* end added by jiangmingli for host debug */

#ifdef OPL_DRV_DEBUG

/* printf function controled by gbPrtDbg */
#define OPL_PRINTF(format, ...) \
  do {\
	if(gbPrtDbg) printk(format, ##__VA_ARGS__ ); }while(0)

/* Three level controled log by gbLogDbg, module level pointer, file level pointer */

#define OPL_LOG(mod_level, file_level, format, ...) \
 do {\
  	if(gbLogDbg){\
 	 	  if(OPL_NULL == (file_level)){\
 			  if(OPL_NULL == (mod_level)){\
 			     printk( format, ##__VA_ARGS__ );}\
         else if (*(INT32 *)(mod_level)){\
 		          printk(format, ##__VA_ARGS__ );\
         }\
       }else if (*(INT32 *)(file_level)){\
       	  if(OPL_NULL == (mod_level)){\
 			       printk(format, ##__VA_ARGS__ );}\
           else if(*(INT32 *)(mod_level)){\
             printk(format, ##__VA_ARGS__ );}\
       }\
    }\
 }while(0)

#define OPL_LOG_TRACE()    			    printk("\n%s,%s,%d!\n",__FILE__,__FUNCTION__,__LINE__)
#define OPL_TRACE()						printk("\n%s,%s,%d!\n",__FILE__,__FUNCTION__,__LINE__)
#else /*#ifdef _OPL_DRV_DEBUG_*/

/* Do not compile debug info in objective file */
#define OPL_PRINTF  	printk
#define OPL_LOG(mod_level, file_level, format, ...)
#define OPL_LOG_TRACE()  
#define OPL_TRACE()
#endif /*#ifdef _OPL_DRV_DEBUG_*/

void OPL_DUMP(UINT32 *pModId, const void *pFromAddr, UINT32 uLen, const UINT8 *pCaption);
void ohelp(INT32 nHow);
OPL_STATUS oplLogSet(UINT32 nModId, OPL_BOOL logEn);
OPL_STATUS oplLogGet(UINT32 nModId, OPL_BOOL *logEn);
void oplLogShow(void);
void oplLogTest(void);
#endif 
