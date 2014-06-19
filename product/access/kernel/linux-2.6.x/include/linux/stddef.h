#ifndef _LINUX_STDDEF_H
#define _LINUX_STDDEF_H

#include <linux/compiler.h>

#undef NULL
#if defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif

#ifdef __KERNEL__

//Patch for DSP , fixme
#if !(defined(__cplusplus)&&defined(RTOS_VXWORKS)&&defined(RTOS_uClinux)&&defined(UCLINUX_MAKE))
enum {
	false	= 0,
	true	= 1
};
#else
#define false 0
#define true 1
#endif

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif
#endif /* __KERNEL__ */

#endif
