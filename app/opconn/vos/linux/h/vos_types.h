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
* FILENAME:  vos_types.h
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_types.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_TYPES_H_
#define __VOS_TYPES_H_
 
#ifdef __cplusplus
extern "C" {
#endif
 
#include <vos_compiler.h>
#include <lw_type.h>

/*--------------------------Macro definition------------------------- */
/* for platform-independent types */
#ifndef TRUE
#define TRUE			1
#endif

#ifndef FALSE
#define FALSE			0
#endif

#ifndef DONT_CARE
#define DONT_CARE		0
#endif

#define VOL			volatile

#define BITS2BYTES(x)		(((x) + 7) / 8)
#define BITS2WORDS(x)		(((x) + 31) / 32)
#define BYTES2BITS(x)		((x) * 8)
#define BYTES2WORDS(x)		(((x) + 3) / 4)
#define WORDS2BITS(x)		((x) * 32)
#define WORDS2BYTES(x)		((x) * 4)

#define COUNTOF(ary)		((int) (sizeof (ary) / sizeof ((ary)[0])))

#define PTR_TO_INT(x)		((uint32)(x))
#define INT_TO_PTR(x)		((void *)(uint32)(x))

/*--------------------------type definition------------------------- */
typedef unsigned char   uint8;		/* 8-bit quantity  */
typedef unsigned short  uint16;		/* 16-bit quantity */
typedef unsigned int    uint32;		/* 32-bit quantity */
typedef signed char     int8;		/* 8-bit quantity  */
typedef signed short    int16;		/* 16-bit quantity */
typedef signed int      int32;		/* 32-bit quantity */

#ifndef INVALID_8
#define INVALID_8       ((uint8)0xff)
#endif

#ifndef INVALID_16
#define INVALID_16      ((uint16)0xffff)
#endif

#ifndef INVALID_32
#define INVALID_32      ((uint32)0xffffffff)
#endif

#ifndef OK
#define OK 0
#endif

#ifndef ERROR
#define ERROR -1
#endif

#ifndef NO_ERROR
#define NO_ERROR 0
#endif

#ifndef NULL
#define NULL            ((void *) 0)   /* a null pointer */
#endif

#ifndef _MWTYPE_DEF
#define _MWTYPE_DEF

typedef char                CHAR;
typedef int                 INT;
typedef int                 LONG;
typedef short               SHORT;
typedef float               FLOAT;

typedef	char		        INT8;
typedef	short		        INT16;
typedef	int                 INT32;
typedef	long long	        INT64;

typedef	unsigned char       UINT8;
typedef	unsigned short      UINT16;
typedef	unsigned int        UINT32;
typedef	unsigned long long  UINT64;

typedef	unsigned char	    UCHAR;
typedef unsigned short	    USHORT;
typedef	unsigned int	    UINT;
typedef unsigned long	    ULONG;
typedef unsigned long long  ULONG64;
typedef ULONG               IP_ADDRESS;
typedef unsigned char       BYTE;

typedef	int                 BOOL;
typedef unsigned short      BOOL_T;
typedef	int                 STATUS;
typedef int                 ARGINT;
typedef void                VOID;
#define OK          (0)
#define ERROR		(-1)

#if defined(__KERNEL__)
#define ASSERT(_x)  do{         \
    if (!(_x)) {                \
        printk("Assert[%s][%d] %s\n", __FUNCTION__, __LINE__, #_x);       \
    }   \
}while(0)
#else
#define ASSERT(_x)  do{         \
    if (!(_x)) {                \
        printf("Assert[%s][%d] %s\n", __FUNCTION__, __LINE__, #_x);       \
    }   \
}while(0)

#endif
#endif

#ifdef __cplusplus
typedef int             (*FUNCPTR) (...);     /* ptr to function returning int */
typedef void            (*VOIDFUNCPTR) (...); /* ptr to function returning void */
typedef double          (*DBLFUNCPTR) (...);  /* ptr to function returning double*/
typedef float           (*FLTFUNCPTR) (...);  /* ptr to function returning float */
#else
typedef int             (*FUNCPTR) ();	   /* ptr to function returning int */
typedef void            (*VOIDFUNCPTR) (); /* ptr to function returning void */
typedef double          (*DBLFUNCPTR) ();  /* ptr to function returning double*/
typedef float           (*FLTFUNCPTR) ();  /* ptr to function returning float */
#endif			/* _cplusplus */

 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_TYPES_H_ */

