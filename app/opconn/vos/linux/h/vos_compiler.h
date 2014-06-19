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
* FILENAME:  vos_compiler.h
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_compiler.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_COMPILER_H_
#define __VOS_COMPILER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------Macro definition------------------------- */
#define COMPILER_HAS_CONST
#define COMPILER_HAS_STATIC
#define COMPILER_HAS_DOUBLE

#if defined(__GNUC__) && !defined(__PEDANTIC__)
#define COMPILER_HAS_LONGLONG
#define COMPILER_HAS_LONGLONG_SHIFT

#define COMPILER_HAS_LONGLONG_ADDSUB
#define COMPILER_HAS_LONGLONG_ADDOR
#define COMPILER_HAS_LONGLONG_COMPARE
#define COMPILER_HAS_INLINE

#define COMPILER_ATTRIBUTE(_a) __attribute__ (_a)
#define COMPILER_REFERENCE(_a) ((void)(_a))
#else  /* __GNUC__ */
#define COMPILER_ATTRIBUTE(_a)
#define COMPILER_REFERENCE(_a)  ((void)(_a)
#endif /* __GNUC__ */

/*override the config from Makefile  */

#ifdef  COMPILER_OVERRIDE_NO_LONGLONG
#undef	COMPILER_HAS_LONGLONG
#undef	COMPILER_HAS_LONGLONG_SHIFT
#undef	COMPILER_HAS_LONGLONG_ADDSUB
#undef	COMPILER_HAS_LONGLONG_ANDOR
#undef	COMPILER_HAS_LONGLONG_COMPARE
#endif /* COMPILER_OVERRIDE_NO_LONGLONG */

#ifdef COMPILER_OVERRIDE_NO_DOUBLE
#undef COMPILER_HAS_DOUBLE
#endif /*COMPILER_OVERRIDE_NO_DOUBLE  */

#ifdef COMPILER_OVERRIDE_NO_INLINE
#undef COMPILER_HAS_INLINE
#endif /* COMPILER_OVERRIDE_NO_INLINE */

#ifdef COMPILER_OVERRIDE_NO_CONST
#undef COMPILER_HAS_CONST
#endif /*COMPILER_OVERRIDE_NO_CONST*/

#ifdef COMPILER_OVERRIDE_NO_STATIC
#undef COMPILER_HAS_STATIC
#endif /* COMPILER_OVERRIDE_NO_STATIC */

 /* for INLINE */
#ifndef INLIEN
#ifdef COMPILER_HAS_INLINE
#define INLINE inline
#else
#define INLINE
#endif
#endif /* !INLINE */
 /* for const  */
#ifndef CONST
#ifdef COMPILER_HAS_CONST
#define CONST		const
#else
#define CONST
#endif
#endif /* !CONST */

 /* for static  */
#ifndef STATIC
#if defined(COMPILER_HAS_STATIC)
#define STATIC	static
#else
#define STATIC
#endif
#endif /* !STATIC */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __VOS_COMPILER_H_ */

