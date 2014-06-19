/*
=============================================================================
     Header Name: hal_multicast.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 xfan    2009/03/17   Initial Version
----------------   ------------  ----------------------------------------------
*/
#ifndef _HAL_MULTICAST_H_
#define _HAL_MULTICAST_H_

typedef struct HAL_MC_CFG_INFO_s{
  UINT8 port;
  UINT8 mac[6];
} HAL_MC_CFG_INFO_t;

#define HAL_MC_PRINTF(x)  printf x
#define HAL_MC_TRACE() HAL_MC_PRINTF(("%s,%s,%d\n",__FILE__,__FUNCTION__,__LINE__))

#ifndef ERROR
#define ERROR -1
#endif

#ifndef NO_ERROR
#define NO_ERROR 0
#endif

#ifndef VOS_OK
#define VOS_OK 0
#endif
#ifndef VOS_ERR
#define VOS_ERR  1
#endif


#define MAX_LOGIC_PORT_NUM  6
#define MIN_LOGIC_UNI_ID 1
#define MAX_LOGIC_UNI_ID 4

#define IVALID_PORT_ID   0xFF

#define IS_VALID_UNI_PORT(_port) ((_port >= MIN_LOGIC_UNI_ID) && (_port <= MAX_LOGIC_UNI_ID))
#define VALID_PORT_ID(_port) ((_port >= MIN_LOGIC_UNI_ID) && (_port <= MAX_LOGIC_PORT_NUM))


#ifdef CTC_MULTICAST_SURPORT
typedef enum tagMULTICAST_Tag_Oper_Mode 
{
  MC_TAG_MODE_TRANSPARENT=0,
  MC_TAG_MODE_STRIP,
  MC_TAG_MODE_TRANSLATION,
  MC_TAG_MODE_END
} MC_TAG_MODE_E;
#endif


#ifndef MIN_PACKET_SIZE
#define MIN_PACKET_SIZE         60
#endif

#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE         1514
#endif

#if 1
#define H3C_ASSERT(x) \
do \
{\
   if(!(x))\
   {\
       printf("\nH3c assert fail!!! func:%s line:%d.\n",__FUNCTION__,__LINE__);\
       return ERROR;\
   }\
}while(0)
#else
#define H3C_ASSERT(x) 
#endif

#if 0
#define DRV_DEBUG_PRINT(format,args...) \
do                                      \
{                                       \
    printf(format, ## args);            \
}while(0)
#else
#define DRV_DEBUG_PRINT(format,args...) 
#endif

#endif /* _HAL_MULTICAST_H_ */