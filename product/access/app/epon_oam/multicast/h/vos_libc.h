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
* FILENAME:  vos_libc.h
*
* DESCRIPTION: 
*	
*
* Date Created: Nov 03, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_libc.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_LIBC_H_
#define __VOS_LIBC_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
#include <vos_types.h>

#include <stdarg.h>
#include <string.h>

/*--------------------------Macro definition------------------------- */

#define RTOS_STRINGS



#ifdef RTOS_STRINGS
#define vosStrLen		strlen
#define vosStrCpy		strcpy
#define vosStrCmp		strcmp
#define vosMemCpy		memcpy
#define vosMemSet		memset
#else /* !RTOS_STRINGS */
int vosStrLen(const char *s);
char *vosStrCpy(char *, const char *);
char *vosStrCat(char *dst, const char *src);
int vosStrCmp(const char *, const char *);
void *vosMemCpy(void *, const void *, size_t);
void *vosMemSet(void *, int, size_t);

#endif /* !RTOS_STRINGS */

int vosMemCmp(const void *, const void *, size_t);

char *vosStrDup(const char *s);
int vosStrToInt(const char *s, char **end);	/* C constant to integer */
unsigned int vosStrToUInt(const char *s, char **end);	/* C constant to integer */
void vosIntToStr(char *buf, uint32 num,
		     int base, int caps, int prec);
char *vosStrStr(const char *s1, const char *s2);

#ifdef COMPILER_HAS_DOUBLE
void vosDoubleToStr(char *buf, double f, int decimals);
#endif
int vosVSnprintf(char *buf, size_t bufsize,
			    const char *fmt, va_list ap);
int vosVSprintf(char *buf, const char *fmt, va_list ap);
int vosSnprintf(char *buf, size_t bufsize, const char *fmt, ...);
int vosSprintf(char *buf, const char *fmt, ...);
char *vosSafeStrNCpy(char *dst, const char *src, size_t size);
char *vosLastCharIs(const char *s, int c);
char *vosVAsprintf(const char *fmt, va_list *args);
char *vosAsprintf(const char *fmt, ...);

uint32 *crc32FillTable(int endian);
uint32 vosCRC32(const char *s,int size);
int vosCheckCRC(const char *s,int size,unsigned int value);
int vosSystem(const char *cmd);

 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_LIBC_H_ */
 
