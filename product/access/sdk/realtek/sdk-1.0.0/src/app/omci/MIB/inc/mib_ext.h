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

#ifndef __MIB_EXT_H__
#define __MIB_EXT_H__

#ifdef  __cplusplus
extern "C" {
#endif

GOS_ERROR_CODE  MIB_LoadTable(const CHAR* fileName, MIB_TABLE_INDEX tableIndex);
GOS_ERROR_CODE  MIB_Load(const CHAR* fileName);
GOS_ERROR_CODE  MIB_Save(const CHAR* fileName);
MIB_TABLE_INDEX MIB_GetTableIndexByClassId(PON_ME_CLASS_ID classId);
BOOL            MIB_TableSupportAction(MIB_TABLE_INDEX tableIndex, PON_ME_ACTION_TYPE action);
GOS_ERROR_CODE  MIB_CreatePublicTblSnapshot(void);
GOS_ERROR_CODE  MIB_DeletePublicTblSnapshot(void);
GOS_ERROR_CODE  MIB_ClearPublic(void);
GOS_ERROR_CODE  MIB_ListTables(void* pData);
GOS_ERROR_CODE  MIB_ShowMibInfo(MIB_TABLE_INDEX tableIndex, void* pData);


#ifdef  __cplusplus
}
#endif

#endif //__MIB_EXT_H__
 

