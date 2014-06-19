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
* FILENAME:  vos.h
*
* DESCRIPTION: 
*	
*
* Date Created: Oct 31, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_H_
#define __VOS_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
#include <vos_types.h>
#include <vos_alloc.h>
#include <vos_bitmap.h>
#include <vos_compiler.h>
#include <vos_config.h>
#include <vos_hash.h>
#include <vos_io.h>
#include <vos_libc.h>
#include <vos_macros.h>
#include <vos_match.h>
#include <vos_msq.h>
#include <vos_socket.h>
#include <vos_sync.h>
#include <vos_terminal.h>
#include <vos_thread.h>
#include <vos_timer.h>
#include <vos_time.h>

/* begin modifieded by liaohongjun of QID0017*/
#define VOS_DBG_ON 0
#if VOS_DBG_ON
#define VOS_DBG(args...) printf(args)
#else
#define VOS_DBG(args...)
#endif
/* end modifieded by liaohongjun of QID0017*/

/*--------------------------Macro definition------------------------- */
#ifdef RTOS_STRINGS
#define vosStrNCaseCmp strncasecmp
#define vosStrCaseCmp strcasecmp
#else
int vosStrNCaseCmp(const char *s1, const char *s2, size_t n);
int vosStrCaseCmp(const char *s1, const char *s2);
#endif

//#define DBG_LINE printf("%10s %20s %d\n", __FILE__, __FUNCTION__, __LINE__);vosUDelay(300);
#define DBG_LINE 

#define OP_VOS_RAND_MIN	0
#define OP_VOS_RAND_MAX	0x7fffffff


int vosAppInit(void);
int vosAppDestroy(void);

int vosMacCharToDigit(char);
int vosCoreInit(void);
int vosCoreDestroy(void);


void vosShell(void);
int vosSystem(const char *cmd);
int vosRandom(void);
void vosReboot(void);

STATUS vosHWAddrGet(UINT8 *ifName, UINT8 *macAddr);
STATUS vosHWAddrSet(UINT8 *ifName, UINT8 *macAddr);
int vosDigitToHexChar(int digit);
STATUS vosMacToStr(unsigned char *pMac, unsigned char *pMacStr);
STATUS vosIPAddrGet(UINT8 *ifName, char *ipAddr);
STATUS vosIPAddrSet(UINT8 *ifName, char *ipAddr);
STATUS vosNetMaskGet(UINT8 *ifName, char *netMask);
STATUS vosNetMaskSet(UINT8 *ifName, char *netMask);
STATUS vosCpuMtuGet(UINT8 *ifName, UINT32 *ifMtu);
STATUS vosCpuMtuSet(UINT8 *ifName, UINT32 ifMtu);

extern VOS_MUTEX_t g_pstAppAccessLock;

#if 0  /* modified by Gan Zhiheng - 2010/03/20 */

#define VOS_APP_ACCESS_LOCK \
{ \
    printf("%s,%s,%d: take app access lock\r\n", __FILE__, __FUNCTION__, __LINE__); \
    vosMutexTake(&g_pstAppAccessLock, VOS_MUTEX_FOREVER); \
    printf("%s,%s,%d: take app access lock ok\r\n", __FILE__, __FUNCTION__, __LINE__); \
}

#define VOS_APP_ACCESS_UNLOCK \
{ \
    printf("%s,%s,%d: give app access lock\r\n", __FILE__, __FUNCTION__, __LINE__); \
    vosMutexGive(&g_pstAppAccessLock); \
    printf("%s,%s,%d: give app access lock ok\r\n", __FILE__, __FUNCTION__, __LINE__); \
}

#else

#define VOS_APP_ACCESS_LOCK vosMutexTake(&g_pstAppAccessLock)
#define VOS_APP_ACCESS_UNLOCK vosMutexGive(&g_pstAppAccessLock)

#endif /* #if 0 */


#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_H_ */
 
