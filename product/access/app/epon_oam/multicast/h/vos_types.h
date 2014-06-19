#ifndef _VOS_TYPE_H_
#define _VOS_TYPE_H_
typedef	char		        int8;
typedef	short		        int16;
typedef	int                 int32;
typedef	long long	        int64;
typedef	unsigned char       uint8;
typedef	unsigned short      uint16;
typedef	unsigned int        uint32;
typedef	unsigned long long  uint64;
typedef int             (*FUNCPTR) ();     /* ptr to function returning int */
typedef void            (*VOIDFUNCPTR) (); /* ptr to function returning void */
typedef double          (*DBLFUNCPTR) ();  /* ptr to function returning double*/
typedef float           (*FLTFUNCPTR) ();  /* ptr to function returning float */
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


#endif

