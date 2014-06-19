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
* FILENAME:  opl_type.h
*
* DESCRIPTION: 
*	data type definination
*
* Date Created: Apr 30, 2008
*
* Authors(optional): zzhu
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/exchange/h/opl_type.h#1 $
* $Log:$
*
*
**************************************************************************/
#ifndef __OPL_TYPE_H__
#define __OPL_TYPE_H__
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h> 



#ifdef _VXWORKS_
#include "vxWorks.h"
#include "taskLib.h"
#include "semLib.h"
#else
#include <pthread.h>
#include "stdarg.h"
#endif


#define  OPL_API_PACK_HEADER_LENGTH     3
#define  OPL_API_PACK_BUFF_LENGTH       1500
#define  OPL_API_RET_BUFF_LENGTH        255

#define USECOND_BASE         1/*microsecond*/
#define MSECOND_BASE(us)     (1000*us)
#define SECOND_BASE(ms)      (1000*ms)
#define OPL_TIMER_GRANULARITY    500
#define TIME_OUT             10
typedef char            opint8;
typedef short           opint16;
typedef int             opint32;
typedef unsigned char   opuint8;
typedef unsigned short  opuint16;
typedef unsigned int    opuint32;

typedef unsigned long long   opuint64;
typedef	unsigned char        OPBOOL;

#define TRUE            1
#define FALSE           0

#define     OPL_SDK_BIG_ENDIAN 1

#define swap16(n)                       ((UINT16)((((n)<<8)&0xFF00)|(((n)>>8)&0x00FF)))
#define swap32(n)                       (((swap16((n)&0xFFFF)<<16)&0xFFFF0000)| \
                                        (swap16(((n)>>16)&0xFFFF)&0x0000FFFF))
#ifdef OPL_SDK_BIG_ENDIAN
#define OPL_PACK_UINT8(_buf, _var) \
    *_buf++ = (_var)
#define OPL_UNPACK_UINT8(_buf, _var) \
    _var = *_buf++

#define OPL_PACK_UINT16(_buf, _var) \
    *_buf++ = ((_var) >> 8) & 0xff; \
    *_buf++ = (_var) & 0xff;
#define OPL_UNPACK_UINT16(_buf, _var) \
    _var  = *_buf++ << 8; \
    _var |= *_buf++; 

#define OPL_PACK_UINT32(_buf, _var) \
    *_buf++ = ((_var) >> 24) & 0xff; \
    *_buf++ = ((_var) >> 16) & 0xff; \
    *_buf++ = ((_var) >> 8) & 0xff; \
    *_buf++ = (_var) & 0xff;
#define OPL_UNPACK_UINT32(_buf, _var) \
    _var  = *_buf++ << 24; \
    _var |= *_buf++ << 16; \
    _var |= *_buf++ << 8; \
    _var |= *_buf++; 

#define OPL_PACK_UINT64(_buf,_var) \
    *_buf++ = ((_var) >> 56) & 0xff; \
    *_buf++ = ((_var) >> 48) & 0xff; \
    *_buf++ = ((_var) >> 40) & 0xff; \
    *_buf++ = ((_var) >> 32) & 0xff; \
    *_buf++ = ((_var) >> 24) & 0xff; \
    *_buf++ = ((_var) >> 16) & 0xff; \
    *_buf++ = ((_var) >> 8) & 0xff; \
    *_buf++ = (_var) & 0xff;

#define OPL_UNPACK_UINT64(_buf, _var) \
    _var  = *_buf++ << 56; \
    _var  = *_buf++ << 48; \
    _var  = *_buf++ << 40; \
    _var  = *_buf++ << 32; \
    _var  = *_buf++ << 24; \
    _var |= *_buf++ << 16; \
    _var |= *_buf++ << 8; \
    _var |= *_buf++; 

#define OPL_PACK_BUFF(_buff,_pvar,_len) \
    opl_sdk_memcpy(_buff,_pvar,_len); \
    _buff = _buff+_len;

#define OPL_UNPACK_BUFF(_buff,_pvar,_len) \
    opl_sdk_memcpy(_pvar,_buff,_len); \
    _buff = _buff+_len;
#define htoles(n)                       (n)                        
#define htolel(n)                       (n)
#define letohs(n)                       (n)
#define letohl(n)                       (n)
#else
#define OPL_PACK_UINT8(_buf, _var) \
    *_buf++ = (_var)
#define OPL_UNPACK_UINT8(_buf, _var) \
    _var = *_buf++

#define OPL_PACK_UINT16(_buf, _var) \
    *_buf++ = ((_var) >> 8) & 0xff; \
    *_buf++ = (_var) & 0xff;

#define OPL_UNPACK_UINT16(_buf, _var) \
    _var  = *_buf++ \
    _var |= *_buf++ << 8; 

#define OPL_PACK_UINT32(_buf, _var) \
    *_buf++ = ((_var) >> 24) & 0xff; \
    *_buf++ = ((_var) >> 16) & 0xff; \
    *_buf++ = ((_var) >> 8) & 0xff; \
    *_buf++ = (_var) & 0xff;

#define OPL_UNPACK_UINT32(_buf, _var) \
    *_buf++ = ((_var) >> 24) & 0xff; \
    *_buf++ = ((_var) >> 16) & 0xff; \
    *_buf++ = ((_var) >> 8) & 0xff; \
    *_buf++ = (_var) & 0xff;

#define OPL_PACK_UINT64(_buf, _var) \
    *_buf++ = ((_var) >> 56) & 0xff; \
    *_buf++ = ((_var) >> 48) & 0xff; \
    *_buf++ = ((_var) >> 40) & 0xff; \
    *_buf++ = ((_var) >> 32) & 0xff; \
    *_buf++ = ((_var) >> 24) & 0xff; \
    *_buf++ = ((_var) >> 16) & 0xff; \
    *_buf++ = ((_var) >> 8) & 0xff; \
    *_buf++ = (_var) & 0xff;

#define OPL_UNPACK_UINT64(_buf, _var) \
    _var  = *_buf++ << 56; \
    _var  = *_buf++ << 48; \
    _var  = *_buf++ << 40; \
    _var  = *_buf++ << 32; \
    _var  = *_buf++ << 24; \
    _var |= *_buf++ << 16; \
    _var |= *_buf++ << 8; \
    _var |= *_buf++; 

#define OPL_PACK_BUFF(_buff,_pvar,_len) \
    opl_sdk_memcpy(_buff,_pvar,_len);   \
    _buff = _buff+_len;
#define OPL_UNPACK_BUFF(_buff,_pvar,_len) \
    opl_sdk_memcpy(_pvar,_buff,_len);   \
    _buff = _buff+_len;

#define htoles(n)                       swap16(n)   /* convert host to big endian short */
#define htolel(n)                       swap32(n)   /* convert host to big endian long */
#define letohs(n)                       swap16(n)   /* convert big endian to host short */
#define letohl(n)                       swap32(n)   /* convert big endian to host long */
#endif

typedef enum
{
  S_OK,
  S_ERROR,
  S_INVALID,
  S_FAIL,
  S_NOT_SUPPORTED,
  S_NOT_INITIALIZED,
  S_OUT_OF_RANGE,
  S_BAD_PARAM,
  S_BAD_CONFIGURATION,
  S_NO_RESOURCES,
  S_NOT_FOUND,
  S_ALREADY_EXISTS
} EXCHANGE_STATUS_t;

/*upload ack status*/
typedef enum
{
    /*wait upload ack*/
    UPLOAD_WAIT,
    /*upload success*/
    UPLOAD_OK,
    /*error code*/
    UPLOAD_ERROR,
    UPLOAD_CRC_ERR
} UPLOAD_ACK_STATUS_t;

#define opl_return_parse(ret) {if (opl_ok ==ret) return S_OK;\
                               else return S_ERROR;}

#define opl_sdk_memset     memset
#define opl_sdk_memcpy     memcpy
#define opl_sdk_memcmp     memcmp 

#define opl_ok              0 
#define opl_error           -1
#define opl_get_timeout     -2
#define opl_true            1
#define opl_false           0


#define UART1_COMMAND_CLEAR_RXBUFFER 0x10000
#define UART1_COMMAND_CLEAR_TXBUFFER 0x10001
#define ENABLE_UART1_INTERRUPT       0x10002
#define DISABLE_UART1_INTERRUPT      0x10003
#define DUMP_UART1_TXBUFFER          0x10004
#define DUMP_UART1_RXBUFFER          0x10005

/*#define __ONU_SDK__             1*/
#define __FPGA_ONU_SDK__          1
/*#define __IPMUX_SDK__             1 */
/*#define _OPL_DBG_*/
#ifdef _OPL_DBG_
#define opl_sdk_printf(x)  oplDbgPrintf x
#define opl_sdk_trace()    oplDbgPrintf("%s,%d\n",__FUNCTION__,__LINE__)
#define opl_dump_data(p,len,width) \
{ \
    opuint32 index;opuint8 *x = p;for(index = 0; index < len; index++) \
    { \
        if((index != 0) && (index%width == 0)) \
        { \
            opl_sdk_printf(("\n%02x ",x[index])); \
        }else \
        { \
            opl_sdk_printf(("%02x ",x[index])); \
        } \
    } \
    opl_sdk_printf(("\n")); \
    opl_sdk_printf(("%s,%d\n",__FUNCTION__,__LINE__)); \
}

#define opl_dbg(x) printf x
#else 
#define opl_sdk_trace()
#define opl_sdk_printf(x)
#define opl_dump_data(p,len,width)
#define opl_dbg(x)
#endif
#define using_sem_for_send          1

#endif
