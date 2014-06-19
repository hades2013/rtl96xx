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


#ifndef __MIB_DEFS_H__
#define __MIB_DEFS_H__


#ifdef __cplusplus
extern "C" {
#endif

extern BOOL gOmciLogEnable;

#define MIB_TABLE_NAME_MAX_LEN (21)


typedef struct
{
    CHAR            tableName[MIB_TABLE_NAME_MAX_LEN + 1];
    UINT16          tableAttrCount;
    UINT16          tableEntryCount;
    UINT16          tableEntrySize;
    UINT32          tableDataOffset;
} MIB_TABLE_SAVE_T;


#define MIB_ATTR_NAME_MAX_LEN (23)
typedef struct
{
    CHAR            attrName[MIB_ATTR_NAME_MAX_LEN + 1];
    UINT16          attrSize;
    UINT16          attrOffset;
} MIB_ATTR_SAVE_T;




GOS_ERROR_CODE    mib_CreateTables(MIB_TABLE_INDEX* pIncludeTbl, MIB_TABLE_INDEX* pExcludeTbl);
MIB_TABLE_INFO_T* mib_GetTableInfoPtr(MIB_TABLE_INDEX tableIndex);
MIB_ATTR_INFO_T*  mib_GetAttrInfo(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);
INT32             mib_CompareEntry(MIB_TABLE_INDEX tableIndex, void* pRow1, void* pRow2);
GOS_ERROR_CODE    mib_SetGetAttribute(MIB_TABLE_INDEX tableIndex, void* pAttr, void* pRow, MIB_ATTR_INDEX attrIndex, BOOL set);
void*             mib_GetDefaultRow(MIB_TABLE_INDEX tableIndex);
MIB_TABLE_T*      mib_GetTablePtr(MIB_TABLE_INDEX tableIndex);
GOS_ERROR_CODE    mib_InvokeCallbacks(MIB_TABLE_INDEX tableIndex, void* pOldRow, void* pNewRow, MIB_ATTRS_SET* pAttrsSet, MIB_OPERA_TYPE  operationType);
BOOL              mib_FindEntry(MIB_TABLE_INDEX tableIndex, void* pObjectRow, void* ppRetRow);
BOOL              mib_FindNextEntry(MIB_TABLE_INDEX tableIndex, void* pObjectRow, void* ppRetRow);
BOOL              mib_FindSnapshotEntry(MIB_TABLE_INDEX tableIndex, void* pObjectRow, void* ppRetRow);
BOOL              mib_FindSnapshotNextEntry(MIB_TABLE_INDEX tableIndex, void* pObjectRow, void* ppRetRow);
GOS_ERROR_CODE    mib_AddEntry(MIB_TABLE_INDEX tableIndex, void* pWhere, void* pObjectRow);
GOS_ERROR_CODE    mib_DeleteEntry(MIB_TABLE_INDEX tableIndex, void* pObjectRow);
GOS_ERROR_CODE    mib_SetAttributes(MIB_TABLE_INDEX tableIndex, void* pObjRow, void* pRefRow, MIB_ATTRS_SET* pAttrsSet);
GOS_ERROR_CODE    mib_Save(UINT32 media, RW_FUNC writeFunc, RW_FUNC readFunc);
GOS_ERROR_CODE    mib_LoadTable(UINT32 media, RW_FUNC readFunc, MIB_TABLE_INDEX targetTable);
GOS_ERROR_CODE    mib_Load(UINT32 media, RW_FUNC readFunc);
GOS_ERROR_CODE    mib_SetDirty(BOOL dirty);
GOS_ERROR_CODE MIB_Register(MIB_TABLE_INDEX tableIdx,MIB_TABLE_INFO_T *pTableInfo, MIB_TABLE_OPER_T *oper);



extern UINT32 gMibLogId;
extern BOOL   gMibReadOnly;

#define MIB_LOG(fmt, arg...)  \
    do { if (gOmciLogEnable==TRUE) { printf(fmt, ##arg); printf("\n"); } } while (0);


#ifdef __cplusplus
}
#endif


#endif //__MIB_DEFS_H__

