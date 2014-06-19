/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  cli_utils.h
*
* DESCRIPTION: 
*	CLI utility tools
*
* Date Created: Aug 12, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/h/cli_utils.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __CLI_UTILS_H_
#define __CLI_UTILS_H_

#include <sys/socket.h>
#include <vos.h>
#include "cli_engine.h"

/* Used to cliGetxx() return value */
#define    TOOSMALL        0x01     /* Input too small */
#define    TOOLARGE        0x02     /* Input too large */
#define    RANGEERR        0x04     /* Invalid range */
#define    NOTALLDIGITAL  0x08     /* Not all of digit */
#define    NULLPTRERR     0x10    /* null pointer */
#define    TERMINALERR     0x20   /* terminal error */

#define    CTRLCHITED      0x40    /* CTRL+C */
#define    ENTERHITED      0x80    /* Enter */   

extern STATUS cliByteGet(
    ENV_t *pstEnv, 
    unsigned char *pbyAddr, 
    unsigned char byMinVal, 
    unsigned char byMaxVal, 
    unsigned short *pusRetVal
    );
extern STATUS cliWordGet(
    ENV_t *pstEnv, 
    unsigned short *pusAddr, 
    unsigned short usMinVal,
    unsigned short usMaxVal, 
    unsigned short *pusRetVal
    );
extern STATUS cliLongGet(
    ENV_t *pstEnv, 
    unsigned long *pulAddr, 
    unsigned long ulMinVal,
    unsigned long ulMaxVal, 
    unsigned short *pusRetVal
    );
extern STATUS cliIntegerGet(
    ENV_t *pstEnv, 
    int *pnAddr, 
    int nMinVal, 
    int nMaxVal,
    unsigned short *pusRetVal
    );
extern STATUS cliStringGet(
    ENV_t *pstEnv, 
    char *pchAddr, 
    unsigned long ulMaxStrLen,
    unsigned short *pusRetVal
    );

extern BOOL cliIsValidIpAddress(char *pchStrIP);
extern BOOL cliIsValidNetMask (char str_ip[]);

extern BOOL cliIsDigits(char *pchNumSrc);
extern BOOL cliIsInteger(char str_num[]);

extern STATUS cliIpToStr(ULONG ip_addr, char *ip_string);
extern STATUS cliStrToIp(char *pchDotIPStr,ULONG *pulIP);
extern int cliDigitToHexChar(int digit);
/* begin modified by jiangmingli, 20080905 */
extern STATUS cliMacToStr(unsigned char *pMac, unsigned char *pMacStr);
/* end modified by jiangmingli, 20080905 */
extern STATUS cliStrToDate(char *sprDate, int *pnYear, int *pnMon, int *pnDay);
extern STATUS cliStrToTime(char *sprTime, int *pnHour, int *pnMin, int *pnSec);

extern STATUS cliStrLeftTrim(char **pchString);
extern STATUS cliStrRightTrim(char *pchString);
extern STATUS cliStrToLower(char *pchString);
extern BOOL cliIsInputYes(char *pchInput);
extern BOOL cliIsInputNo(char *pchInput);

/* begin added by jiangmingli, 2008-08-26 */
extern STATUS cliGetPortlist(INT8 *pcPortlist, UINT32 uiMaxPortNum, UINT32 *puiPortId, UINT32 *puiPortNum);
/* end added by jiangmingli, 2008-08-26 */

extern int cliIsLeapYear(unsigned int unYear);


#endif  /* #ifdef __CLI_UTILS_H_ */

