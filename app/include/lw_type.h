/*******************************************************************************
*                 File Comments                                                
*******************************************************************************/
#ifndef   _INCLUDE_LW_TYPE_DEF_H_
#define   _INCLUDE_LW_TYPE_DEF_H_

#ifdef  __cplusplus
extern "C"{
#endif

#include "lw_config.h"

#if	!defined(NULL)
#if defined __GNUG__
#define NULL (__null)
#else
#if !defined(__cplusplus) && 0
#define NULL ((void*)0)
#else
#define NULL (0)
#endif
#endif
#endif

#if	!defined(EOF) || (EOF!=(-1))
#define EOF		(-1)
#endif

#if	!defined(FALSE) || (FALSE!=0)
#define FALSE		0
#endif

#if	!defined(TRUE) || (TRUE!=1)
#define TRUE		1
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
#if defined(CONFIG_PRODUCT_EPN104) || defined(CONFIG_PRODUCT_EPN204)
typedef	int                 STATUS;
#endif
typedef int                 ARGINT;

#define BUF_SIZE_16  16
#define BUF_SIZE_32  32
#define BUF_SIZE_64  64
#define BUF_SIZE_128 128
#define BUF_SIZE_256 256

#ifndef OK
#define OK 0
#endif

#ifndef ERROR
#define ERROR -1
#endif

#ifndef NO_ERROR
#define NO_ERROR 0
#endif

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

#define BOOL_TRUE  ((BOOL_T)1)
#define BOOL_FALSE ((BOOL_T)0)

#define ENUMTOSTR(VAL)  VAL##_STR
#define ENABLE_STR       "enable"
#define DISABLE_STR       "disable"
#define TRUE_STR           "true"
#define FALSE_STR          "flase"

/* typedef void will raise g++ compile error, zhouxiaochen */
/* typedef void		VOID; */
#define VOID        void

typedef int         PART_ID;

#define FAST        register
#define IMPORT      extern
#define LOCAL       static
#define CONST      const
#if defined(UT_TEST) || defined(UT_TEST_X86)
#define STATIC       
#else
#define STATIC   static
#endif
/* timeout defines */

#define NO_WAIT             (0)
#define WAIT_FOREVER        (-1)

//#define OK          (0)
//#define ERROR		(-1)
#define IN
#define INOUT
#define OUT
typedef UINT32 port_num_t;

typedef UINT16 vlan_id_t;


/* user port num define : | device num(8bit) | slot num(8bit) | reserve(8bit) | port num(8bit) |*/
typedef UINT32 user_port_num_t;
typedef UINT32 agg_grp_num_t;

#define BUF_SIZE128  128
#define BUF_SIZE64   64
#define BUF_SIZE32   32
#define BUF_SIZE1024 1024
#define BUF_SIZE256 256
#ifdef  __cplusplus
}
#endif
#endif /* _INCLUDE_LW_TYPE_DEF_H_  */



