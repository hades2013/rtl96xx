/*
 *	osdep.h
 *	Operating System and Hardware Abstraction Layer
 *	Release $Name: MATRIXSSL-3-1-3-OPEN $
 */
/*
 *	Copyright (c) PeerSec Networks, 2002-2010. All Rights Reserved.
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software 
 *	into proprietary programs.  If you are unable to comply with the GPL, a 
 *	commercial license for this software may be purchased from PeerSec Networks
 *	at http://www.peersec.com
 *	
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	See the GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#ifndef _h_PS_PLATFORM
#define _h_PS_PLATFORM

/******************************************************************************/
/*
	Standard types
	POSIX define is used for Linux and Mac OS X
*/
#include <stdio.h>

#ifndef POSIX
	#if defined(LINUX) || defined(OSX)
		#define POSIX
	#endif
#endif

/* Branch hints for GCC. */
#ifdef __GNUC__
#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)
#else
#define likely(x) x
#define unlikely(x) x
#endif

#ifdef POSIX
#include <stdint.h>
typedef int32_t int32;
typedef uint32_t uint32;
/* 64 bit native integers */
#ifdef HAVE_NATIVE_INT64 
	typedef int64_t int64;
	typedef uint64_t uint64;
#endif
#endif /* POSIX */

#ifdef WIN32
#include <windows.h>
typedef signed long int32;
typedef unsigned long uint32;
#ifdef HAVE_NATIVE_INT64 
	typedef unsigned long long	uint64;
	typedef signed long long	int64;
#endif
#endif /* WIN32 */




/******************************************************************************/
/*
	Internal os core APIs
	
	NOTE: If the compile is failing here the os is probably not set
		as a compile-time definition (POSIX, WIN32, ...)
*/
extern int32	osdepTraceOpen(void);
extern int32	osdepTraceClose(void);
extern int32	osdepTimeOpen(void);
extern int32	osdepTimeClose(void);
extern int32	osdepEntropyOpen(void);
extern int32	osdepEntropyClose(void);
#ifdef HALT_ON_PS_ERROR
extern void		osdepBreak(void);
#endif

#define halAlert()

/******************************************************************************/
#ifdef USE_MULTITHREADING
/******************************************************************************/
/*
	Defines to make library multithreading safe
*/
/******************************************************************************/
/*
	Mutex
*/
extern int32 osdepMutexOpen(void);
extern int32 osdepMutexClose(void);

/*
	OS-specific psMutex_t types
*/
#ifdef WIN32
/******************************************************************************/
typedef CRITICAL_SECTION psMutex_t;

/******************************************************************************/
#elif POSIX
/******************************************************************************/
#include <pthread.h>
#include <string.h>

typedef pthread_mutex_t psMutex_t;

/******************************************************************************/
#elif VXWORKS
/******************************************************************************/
#include "semLib.h"

typedef SEM_ID	psMutex_t; 

/******************************************************************************/
#endif /* OS specific mutex */
/******************************************************************************/
#endif /* USE_MULTITHREADING */
/******************************************************************************/

/******************************************************************************/
#ifdef USE_FILE_SYSTEM
/******************************************************************************/
#ifdef POSIX
#include <sys/stat.h>
#endif /* POSIX */

#endif /* USE_FILE_SYSTEM */
/******************************************************************************/

#ifndef min
#define min(a,b)	(((a) < (b)) ? (a) : (b))
#endif /* min */

/******************************************************************************/
/*
	OS-specific psTime_t types
	
	Make psTime_t an opaque time value.
*/
#ifdef WIN32
	typedef LARGE_INTEGER psTime_t;
#endif

#ifdef VXWORKS
	typedef struct {
		long sec;
		long usec;
	} psTime_t;
#endif



#ifdef POSIX
	#include <sys/time.h>
	#include <time.h>
	typedef struct timeval psTime_t;
#endif

/******************************************************************************/
/*
	PSPUBLIC magic for Win DLLs	
*/
#ifndef _USRDLL
	#ifdef WIN32
		#define PSPUBLIC extern __declspec(dllimport)
	#endif /* WIN32 */
#else /* h_EXPORT_SYMOBOLS */
	#ifdef WIN32
		#define PSPUBLIC extern __declspec(dllexport)
	#endif /* WIN */
#endif /* h_EXPORT_SYMOBOLS */

#ifndef WIN32
#define PSPUBLIC extern
#endif /* !WIN */

/******************************************************************************/
/*
	Raw trace and error
*/
PSPUBLIC void _psTrace(char *msg);
PSPUBLIC void _psTraceInt(char *msg, int32 val);
PSPUBLIC void _psTraceStr(char *msg, char *val);
PSPUBLIC void _psTracePtr(char *message, void *value);

PSPUBLIC void _psError(char *msg);
PSPUBLIC void _psErrorInt(char *msg, int32 val);
PSPUBLIC void _psErrorStr(char *msg, char *val);

/******************************************************************************/
/*
	Core trace
*/
#ifndef USE_CORE_TRACE
#define psTraceCore(x) 
#define psTraceStrCore(x, y) 
#define psTraceIntCore(x, y)
#define psTracePtrCore(x, y)
#else
#define psTraceCore(x) _psTrace(x)
#define psTraceStrCore(x, y) _psTraceStr(x, y)
#define psTraceIntCore(x, y) _psTraceInt(x, y)
#define psTracePtrCore(x, y) _psTracePtr(x, y)
#endif /* USE_CORE_TRACE */

/******************************************************************************/
/*
	HALT_ON_PS_ERROR define at compile-time determines whether to halt on
	psAssert and psError calls
*/ 
#define psAssert(C)  if (likely(C)) ; else \
{halAlert();_psTraceStr("psAssert %s", __FILE__);_psTraceInt(":%d ", __LINE__);\
_psError(#C);} 

#define psError(a) \
 halAlert();_psTraceStr("psError %s", __FILE__);_psTraceInt(":%d ", __LINE__); \
 _psError(a);
 
#define psErrorStr(a,b) \
 halAlert();_psTraceStr("psError %s", __FILE__);_psTraceInt(":%d ", __LINE__); \
 _psErrorStr(a,b)
 
#define psErrorInt(a,b) \
 halAlert();_psTraceStr("psError %s", __FILE__);_psTraceInt(":%d ", __LINE__); \
 _psErrorInt(a,b)

#endif /* _h_PS_PLATFORM */

