/*************************************************************************
*
*  COPYRIGHT (C) 2003-2009 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  hal_global.h
*
* DESCRIPTION: 
*	
*
* Date Created: Mar 18, 2009
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/hal/h/hal_global.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __HAL_GLOBAL_H_
#define __HAL_GLOBAL_H_
 
#ifdef __cplusplus
extern "C" {
#endif

#include <hal.h>


typedef struct HAL_API_GBL_REG_s
{
    UINT32 ulAddr;
    UINT32 ulValue;
    UINT32 ulResult; /* 0 is ok, otherwise false */
} HAL_API_GBL_REG_t;

#define HAL_API_GBL_REG_SET( RegAddr, Value, Result ) \
{ \
    OPL_API_DATA_t stApiData; \
    HAL_API_GBL_REG_t stRegParam; \
    stRegParam.ulAddr = (RegAddr); \
    stRegParam.ulValue = (Value); \
    stRegParam.ulResult = OPL_ZERO; \
    stApiData.apiId = HAL_API_GBL_REG_WRITE; \
    stApiData.length = sizeof(HAL_API_GBL_REG_t); \
    stApiData.param = &stRegParam; \
    stApiData.eventFunc = NULL; \
    (Result) = halAppApiCallSync(&stApiData); \
    if (NO_ERROR != stRegParam.ulResult) \
    { \
        (Result) = stRegParam.ulResult; \
    } \
    (Value) = stRegParam.ulValue; \
}

#define HAL_API_GBL_REG_GET( RegAddr, Value, Result ) \
{ \
    OPL_API_DATA_t stApiData; \
    HAL_API_GBL_REG_t stRegParam; \
    stRegParam.ulAddr = (RegAddr); \
    stRegParam.ulValue = OPL_ZERO; \
    stApiData.apiId = HAL_API_GBL_REG_READ; \
    stApiData.length = sizeof(HAL_API_GBL_REG_t); \
    stApiData.param = &stRegParam; \
    stApiData.eventFunc = NULL; \
    (Result) = halAppApiCallSync(&stApiData); \
    if (NO_ERROR != stRegParam.ulResult) \
    { \
        (Result) = stRegParam.ulResult; \
    } \
    (Value) = stRegParam.ulValue; \
}


typedef struct HAL_API_GBL_REG_FIELDS_s
{
    UINT32 ulAddr;
    UINT16 usFieldOffset;
    UINT16 usFieldWidth;
    UINT32 ulFieldData;  /* not the register value, ulFieldData = {regVal} >> usFieldOffset */
    UINT32 ulResult;
} HAL_API_GBL_REG_FIELDS_t;

#define HAL_API_GBL_REG_FIELD_SET( RegAddr, FieldOffset, FieldWidth, FieldData, Result ) \
{ \
    OPL_API_DATA_t stApiData; \
    HAL_API_GBL_REG_FIELDS_t stRegParam; \
    stRegParam.ulAddr = (RegAddr); \
    stRegParam.usFieldOffset = (FieldOffset); \
    stRegParam.usFieldWidth = (FieldWidth); \
    stRegParam.ulFieldData = (FieldData); \
    stApiData.apiId = HAL_API_GBL_REG_FIELD_WRITE; \
    stApiData.length = sizeof(HAL_API_GBL_REG_FIELDS_t); \
    stApiData.param = &stRegParam; \
    stApiData.eventFunc = NULL; \
    (Result) = halAppApiCallSync(&stApiData); \
    if (NO_ERROR != stRegParam.ulResult) \
    { \
        (Result) = stRegParam.ulResult; \
    } \
}

#define HAL_API_GBL_REG_FIELD_GET( RegAddr, FieldOffset, FieldWidth, FieldData, Result ) \
{ \
    OPL_API_DATA_t stApiData; \
    HAL_API_GBL_REG_FIELDS_t stRegParam; \
    stRegParam.ulAddr = (RegAddr); \
    stRegParam.usFieldOffset = (FieldOffset); \
    stRegParam.usFieldWidth = (FieldWidth); \
    stRegParam.ulFieldData = (FieldData); \
    stApiData.apiId = HAL_API_GBL_REG_FIELD_READ; \
    stApiData.length = sizeof(HAL_API_GBL_REG_FIELDS_t); \
    stApiData.param = &stRegParam; \
    stApiData.eventFunc = NULL; \
    (Result) = halAppApiCallSync(&stApiData); \
    if (NO_ERROR != stRegParam.ulResult) \
    { \
        (Result) = stRegParam.ulResult; \
    } \
    (FieldData) = stRegParam.ulFieldData; \
}


typedef struct HAL_API_GBL_TAB_s
{
    UINT32 ulRegion;
    UINT16 usRecordStart;
    UINT16 usRecordNum;
    UINT32 *pulRecordData;
    UINT32 ulResult;
} HAL_API_GBL_TAB_t;


typedef struct HAL_API_GBL_REG_BITS_s
{
    UINT32 ulRegAddr;
    UINT32 ulBits;
    UINT32 ulResult;
} HAL_API_GBL_REG_BITS_t;


typedef struct HAL_API_GBL_ORR_s
{
    UINT32 ulRegAddr;
    UINT32 ulFd;        /* for debugging, can be STD_OUT or specified fd, 
                           if it is 0, system changes to STD_OUT */
} HAL_API_GBL_ORR_t;

typedef struct HAL_API_GBL_OWR_s
{
    UINT32 ulRegAddr;
    UINT32 ulRegVal;
    UINT32 ulFd;
} HAL_API_GBL_OWR_t;


typedef struct HAL_API_GBL_MRR_s
{
    UINT32 ulRegAddr;
    UINT32 ulRegNum;
    UINT32 ulFd;
} HAL_API_GBL_MRR_t;


typedef struct HAL_API_GBL_MWR_s
{
    UINT32 ulRegAddr;
    UINT32 ulRegNum;
    UINT32 ulNewVal;
    UINT32 ulMode; /* 0 indicate increasing value and fixed register, 
                      1 indicate increasing register and fixed value */
    UINT32 ulFd;
} HAL_API_GBL_MWR_t;


typedef struct HAL_API_GBL_OST_s
{
    UINT32 ulFd;
} HAL_API_GBL_OST_t;


typedef struct HAL_API_GBL_ORT_s
{
    UINT32 ulRegion;
    UINT32 ulRecordStart;
    UINT32 ulRecordNum;
    UINT32 ulFd;
} HAL_API_GBL_ORT_t;


typedef struct HAL_API_GBL_OWT_s
{
    UINT32 ulRegion;
    UINT32 ulRecordStart;
    UINT32 ulRecordNum;
    UINT32 ulOffset;
    UINT32 ulValue;
} HAL_API_GBL_OWT_t;


typedef struct HAL_API_GBL_MDIO_REG_s
{
    UINT8 ucDevAddr;
    UINT8 ucRegAddr;
    UINT16 usData;
    UINT32 ulResult;
} HAL_API_GBL_MDIO_REG_t;


typedef struct HAL_API_GBL_MDIO_REG_FIELDS_s
{
    UINT8 ucDevAddr;
    UINT8 ucRegAddr;
    UINT8 ucOffset;
    UINT8 ucWidth;
    UINT16 usData;
    UINT32 ulResult;
} HAL_API_GBL_MDIO_REG_FIELDS_t;




#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __HAL_GLOBAL_H_ */
 

