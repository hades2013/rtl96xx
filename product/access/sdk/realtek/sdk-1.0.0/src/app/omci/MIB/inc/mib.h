/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 */



#ifndef __MIB_H__
#define __MIB_H__


#ifdef __cplusplus
extern "C" {
#endif



#define MIB_VERSION_NUM  (0x03)

// the max size of an entry in each table will never greater than MIB_TABLE_ENTRY_MAX_SIZE
#define MIB_TABLE_ENTRY_MAX_SIZE (1024)
// the max size of an attr in each table will never greater than MIB_TABLE_ATTR_MAX_SIZE
#define MIB_TABLE_ATTR_MAX_SIZE  (1024)
// the max attribute num of a table will never greater than MIB_TABLE_ATTR_MAX_NUM
#define MIB_TABLE_ATTR_MAX_NUM   (32)    

typedef enum 
{
    MIB_SET     = 0x0,    /* MIB set */
    MIB_GET     = 0x1,    /* MIB get */
    MIB_GETNEXT = 0x2,    /* MIB get next */
    MIB_DEL     = 0x3,    /* MIB delete */
    MIB_ADD     = 0x4,    /* MIB add */
} MIB_OPERA_TYPE;

typedef enum
{
    MIB_ATTR_USR_READ_ONLY = 0x0,   /* Read only on CLI or snmp */
    MIB_ATTR_USR_WRITE     = 0x1,   /* Read and write on CLI or snmp */
} MIB_ATTR_USR_ACC_TYPE;

typedef enum
{
    MIB_ATTR_OUT_CHAR,
    MIB_ATTR_OUT_DEC,
    MIB_ATTR_OUT_HEX,
    MIB_ATTR_OUT_UNKNOWN
} MIB_ATTR_OUT_STYLE;

typedef enum 
{
    MIB_ATTR_TYPE_UINT8,  /* UINT8 */
    MIB_ATTR_TYPE_UINT16, /* UINT16 */
    MIB_ATTR_TYPE_UINT32, /* UINT32 */
    MIB_ATTR_TYPE_UINT64, /* UINT64 */
    MIB_ATTR_TYPE_STR,    /* String */
    MIB_ATTR_TYPE_TABLE,  /* Table */
} MIB_ATTR_TYPE;

typedef enum
{
    MIB_ATTR_DEF_ZERO = 0,           /* For UINT8 UINT16 UINT32 UINT64 */
    MIB_ATTR_DEF_SPACE,              /* All spaces for string */
    MIB_ATTR_DEF_EMPTY,              /* Null for string */
    MIB_ATTR_DEF_FFFF = 0xFFFFFFFF,  /* All 'F' for For UINT8 UINT16 UINT32 UINT64 */
} MIB_ATTR_DEF_VALUE;


typedef UINT32 MIB_TABLE_INDEX;
#define MIB_TABLE_UNKNOWN_INDEX    ((MIB_TABLE_INDEX)0)
#define MIB_TABLE_FIRST_INDEX      ((MIB_TABLE_INDEX)0x1)
#define MIB_TABLE_NEXT_INDEX(idx)  ((MIB_TABLE_INDEX)(idx + 1))

typedef UINT32 MIB_ATTR_INDEX;
#define MIB_ATTR_UNKNOWN_INDEX    ((MIB_ATTR_INDEX)0)
#define MIB_ATTR_FIRST_INDEX      ((MIB_ATTR_INDEX)0x1)
#define MIB_ATTR_NEXT_INDEX(idx)  ((MIB_ATTR_INDEX)(idx + 1))

typedef UINT32 MIB_ATTRS_SET;

#define MIB_TABLE_CB_MAX_NUM     (8)
typedef GOS_ERROR_CODE (* MIB_CALLBACK_FUNC)(MIB_TABLE_INDEX tableIndex,
                                             void*           pOldRow, 
                                             void*           pNewRow, 
                                             MIB_ATTRS_SET*  pAttrsSet,
                                             MIB_OPERA_TYPE  operationType);


typedef GOS_ERROR_CODE (* MIB_CFG_FUNC)(  void*           pOldRow, 
                                             void*           pNewRow, 
                                             MIB_OPERA_TYPE  operationType);


GOS_ERROR_CODE  MIB_Init(BOOL bReadOnly, BOOL logEnable, MIB_TABLE_INDEX* pIncludeTbl, MIB_TABLE_INDEX* pExcludeTbl);
MIB_TABLE_INDEX MIB_GetTableIndexByName(const CHAR* name);
UINT32          MIB_GetTableAttrNum(MIB_TABLE_INDEX tableIndex);
UINT32          MIB_GetTableCurEntryCount(MIB_TABLE_INDEX tableIndex);
UINT32          MIB_GetTableEntrySize(MIB_TABLE_INDEX tableIndex);
UINT32          MIB_GetTableEntrySizeBeforePadded(MIB_TABLE_INDEX tableIndex);

MIB_ATTR_INDEX MIB_GetAttrIndexByName(MIB_TABLE_INDEX tableIndex, const CHAR* name);
UINT32         MIB_GetAttrSize(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);
UINT32         MIB_GetAttrDefaultValue(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);

GOS_ERROR_CODE MIB_RegisterCallback(MIB_TABLE_INDEX tableIndex, MIB_CALLBACK_FUNC pfnPreCheck, MIB_CALLBACK_FUNC pfnCallback);
GOS_ERROR_CODE MIB_RegisterCallbackToAll(MIB_CALLBACK_FUNC pfnPreCheck, MIB_CALLBACK_FUNC pfnCallback);
GOS_ERROR_CODE MIB_ClearAttrSet(MIB_ATTRS_SET* pAttrSet);
GOS_ERROR_CODE MIB_SetAttrSet(MIB_ATTRS_SET* pAttrSet, MIB_ATTR_INDEX attrIndex);    
BOOL           MIB_IsInAttrSet(MIB_ATTRS_SET* pAttrSet, MIB_ATTR_INDEX attrIndex);
GOS_ERROR_CODE MIB_FullAttrSet(MIB_ATTRS_SET* pAttrSet);
UINT32         MIB_GetAttrNumOfSet(MIB_ATTRS_SET* pAttrSet);   

GOS_ERROR_CODE MIB_Get(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen);                                
GOS_ERROR_CODE MIB_GetNext(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen); 
GOS_ERROR_CODE MIB_GetFirst(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen); 
 
GOS_ERROR_CODE MIB_Set(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen);                                
GOS_ERROR_CODE MIB_SetAttributes(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen, MIB_ATTRS_SET* pAttrSet);

GOS_ERROR_CODE MIB_Delete(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen);

GOS_ERROR_CODE MIB_Clear(MIB_TABLE_INDEX tableIndex);
GOS_ERROR_CODE MIB_ClearAll(void);
GOS_ERROR_CODE MIB_Default(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen); 

GOS_ERROR_CODE MIB_SetAttrToBuf(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex, void *pAttr, void *pBuf, UINT32 size);
GOS_ERROR_CODE MIB_GetAttrFromBuf(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex, void *pAttr, void *pBuf, UINT32 size);

GOS_ERROR_CODE MIB_CreateSnapshot(MIB_TABLE_INDEX tableIndex);
GOS_ERROR_CODE MIB_DeleteSnapshot(MIB_TABLE_INDEX tableIndex);
GOS_ERROR_CODE MIB_CreateAllTableSnapshots(void);
GOS_ERROR_CODE MIB_DeleteAllTableSnapshots(void);
UINT32         MIB_GetSnapshotEntryCount(MIB_TABLE_INDEX tableIndex);
GOS_ERROR_CODE MIB_GetSnapshot(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen);                                
GOS_ERROR_CODE MIB_GetSnapshotNext(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen); 
GOS_ERROR_CODE MIB_GetSnapshotFirst(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen);

BOOL           MIB_IsDirty(void);
GOS_ERROR_CODE MIB_SetDirty(BOOL dirty);

GOS_ERROR_CODE MIB_DumpText(UINT32 fd);
GOS_ERROR_CODE MIB_DumpAll(void);
GOS_ERROR_CODE MIB_DumpTable(MIB_TABLE_INDEX tableIndex, void* pData);


#include "mib_table.h"
#include "mib_ext.h"



#ifdef __cplusplus
}
#endif


#endif //__MIB_H__

