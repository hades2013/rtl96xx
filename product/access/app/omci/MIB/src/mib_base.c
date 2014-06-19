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


#include "app_basic.h"
#include "mib_defs.h"
#include "mib_table.h"
#include "mib_tree.h"

BOOL             gMibDirty = FALSE;
BOOL             gMibReadOnly = FALSE;

MIB_TABLE_T 	 *gCacheTable = NULL;


GOS_ERROR_CODE MIB_Register(MIB_TABLE_INDEX tableIdx,MIB_TABLE_INFO_T *pTableInfo, MIB_TABLE_OPER_T *oper)
{
	MIB_TABLE_T *pTable;

	pTable = (MIB_TABLE_T*)malloc(sizeof(MIB_TABLE_T));

	memset(pTable, 0x00, sizeof(MIB_TABLE_T));


	if(pTable)
	{
		pTable->tableIndex = tableIdx;
		pTable->pTableInfo = mib_GetTableInfoPtr(tableIdx);
		pTable->curEntryCount = 0;
		pTable->snapshotEntryCount = 0;
		pTable->pSnapshotData = NULL;
		pTable->pTableInfo = pTableInfo;
		pTable->meOper = oper;	
		LIST_INIT(&(pTable->entryHead));
		LIST_INSERT_HEAD(&head, pTable, entries);
	}
	
	return GOS_OK;
}


MIB_TABLE_T* mib_GetTablePtr(MIB_TABLE_INDEX tableIndex)
{
	MIB_TABLE_T *np;

    if (!MIB_TABLE_INDEX_VALID(tableIndex))
    {
        return NULL;
    }
	if(gCacheTable!=NULL && gCacheTable->tableIndex == tableIndex)
	{
		return gCacheTable;
	}

	LIST_FOREACH(np,&head,entries)
	{
		
		if(np->tableIndex == tableIndex)
		{
			gCacheTable = np;
			return np;
		}
	}

    return NULL;
}


MIB_ATTR_INFO_T* mib_GetAttrInfo(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex)
{
    MIB_TABLE_INFO_T* pTableInfo = mib_GetTableInfoPtr(tableIndex);

    if (pTableInfo == NULL)
    {
        return NULL;
    }

    if ((attrIndex >= MIB_ATTR_FIRST_INDEX + MIB_GetTableAttrNum(tableIndex)) ||
         (attrIndex < MIB_ATTR_FIRST_INDEX))
    {
        return NULL;
    }

    return &(pTableInfo->pAttributes[attrIndex - MIB_ATTR_FIRST_INDEX]);
}


GOS_ERROR_CODE mib_InvokeCallbacks(MIB_TABLE_INDEX tableIndex, void* pOldRow, void* pNewRow, MIB_ATTRS_SET* pAttrsSet, MIB_OPERA_TYPE  operationType)
{
    MIB_TABLE_T*   pTable;
    UINT32         i;
    GOS_ERROR_CODE cbRet;
    
    pTable = mib_GetTablePtr(tableIndex);

    if (!pTable)
    {
        return GOS_ERR_PARAM;
    }

    for (i = 0; i < MIB_TABLE_CB_MAX_NUM; i++)
    {
        if (pTable->preCheckCB[i])
        {
            cbRet = (*pTable->preCheckCB[i])(tableIndex, pOldRow, pNewRow, pAttrsSet, operationType);
            if (GOS_OK != cbRet)
            {
                return cbRet;
            }
        }
    }

    for (i = 0; i < MIB_TABLE_CB_MAX_NUM; i++)
    {
        if (pTable->operationCB[i])
        {
            cbRet = (*pTable->operationCB[i])(tableIndex, pOldRow, pNewRow, pAttrsSet, operationType);
            if (GOS_OK != cbRet)
            {
                return cbRet;
            }
        }
    }

    return GOS_OK;
}


INT32 mib_CompareEntry(MIB_TABLE_INDEX tableIndex, void * pRow1, void * pRow2)
{
    MIB_ATTR_INDEX attrIndex;
    UINT32 i;
    INT32  ret = 0;
    CHAR   strAttr1[MIB_TABLE_ATTR_MAX_SIZE];
    CHAR   strAttr2[MIB_TABLE_ATTR_MAX_SIZE];
    
    for (attrIndex = MIB_ATTR_FIRST_INDEX, i = 0; i < MIB_GetTableAttrNum(tableIndex);
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (MIB_GetAttrIsIndex(tableIndex, attrIndex))
        {
            ret = 0;
            switch(MIB_GetAttrDataType(tableIndex, attrIndex))
            {
                case MIB_ATTR_TYPE_UINT8:
                {
                    UINT8 value1, value2;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value1, pRow1, 1);
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value2, pRow2, 1);
                    ret = value2 - value1;
                    break;
                }
                case MIB_ATTR_TYPE_UINT16:
                {
                    UINT16 value1, value2;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value1, pRow1, 2);
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value2, pRow2, 2);
                    ret = value2 - value1;
                    break;
                }
                case MIB_ATTR_TYPE_UINT32:
                {
                    UINT32 value1, value2;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value1, pRow1, 4);
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value2, pRow2, 4);
                    ret = value2 - value1;
                    break;
                }
                // Not supported
                case MIB_ATTR_TYPE_UINT64:
                {
                    break;
                }
                case MIB_ATTR_TYPE_STR:
                {
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, strAttr1, pRow1, MIB_GetAttrSize(tableIndex, attrIndex));
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, strAttr2, pRow2, MIB_GetAttrSize(tableIndex, attrIndex));
                    ret = memcmp(strAttr2, strAttr1, MIB_GetAttrSize(tableIndex, attrIndex));
                    break;
                }
                // Not supported
                case MIB_ATTR_TYPE_TABLE:
                {
                    break;
                }
                default:
                    break;
            }

            if (0 != ret)
            {
                return ret;
            }
        }
    }

    return ret;
}


BOOL mib_FindEntry(MIB_TABLE_INDEX tableIndex, void* pObjectRow, void* ppRetRow)
{
    MIB_TABLE_T*   pTable;
    void*          pRowData;
	MIB_ENTRY_T *np;
	
    pTable = mib_GetTablePtr(tableIndex);
	
    if (!pTable)
    {
        return FALSE;
    }

    if (0 == pTable->curEntryCount)
    {
        return FALSE;
    }

	LIST_FOREACH(np,&pTable->entryHead,entries)
	{
		pRowData = np->pData;
		if (0 == mib_CompareEntry(tableIndex, pRowData, pObjectRow))
        {
            GOS_SetUINT32((UINT32*)ppRetRow, (UINT32)pRowData);
            return TRUE;
        }
	}

    GOS_SetUINT32((UINT32*)ppRetRow, (UINT32)pRowData);

    return FALSE;
}


BOOL mib_FindNextEntry(MIB_TABLE_INDEX tableIndex, void* pObjectRow, void* ppRetRow)
{
     MIB_TABLE_T*   pTable;
	INT32 			result;
    void*          pRowData;	
	MIB_ENTRY_T *np,*next;
    
    pTable = mib_GetTablePtr(tableIndex);
    if (!pTable)
    {
        return FALSE;
    }

    if (0 == pTable->curEntryCount)
    {
        return FALSE;
    }

	LIST_FOREACH(np,&pTable->entryHead,entries)
	{
		pRowData = np->pData;
		result  = mib_CompareEntry(tableIndex, pRowData, pObjectRow);

		if (0 == result)
		{
		  next=LIST_NEXT(np, entries);
		  
		  if (next==NULL)
		  {
			  return FALSE;
		  }
		  else
		  {
			  pRowData = next->pData;
			  GOS_SetUINT32((UINT32*)ppRetRow, (UINT32)pRowData);
			  return TRUE;
		  }
		}
	}

    return FALSE;
}





BOOL mib_FindSnapshotEntry(MIB_TABLE_INDEX tableIndex, void* pObjectRow, void* ppRetRow)
{
    MIB_TABLE_T*   pTable;
    UINT32         i;
    void*          pRowData;
    
    pTable = mib_GetTablePtr(tableIndex);
    if (!pTable)
    {
        return FALSE;
    }

    if ((NULL == pTable->pSnapshotData) || (0 == pTable->snapshotEntryCount))
    {
        return FALSE;
    }

    pRowData = pTable->pSnapshotData;
    for (i = 1; i <= pTable->snapshotEntryCount; i++)
    {
        if (0 == mib_CompareEntry(tableIndex, pRowData, pObjectRow))
        {
            GOS_SetUINT32((UINT32*)ppRetRow, (UINT32)pRowData);
            return TRUE;
        }

        if (0 > mib_CompareEntry(tableIndex, pRowData, pObjectRow))
        {
            GOS_SetUINT32((UINT32*)ppRetRow, (UINT32)pRowData);
            return FALSE;
        }

        pRowData = (void*)((UINT32)pRowData + MIB_GetTableEntrySize(tableIndex));
    }

    GOS_SetUINT32((UINT32*)ppRetRow, (UINT32)pRowData);

    return FALSE;
}



BOOL mib_FindSnapshotNextEntry(MIB_TABLE_INDEX tableIndex, void* pObjectRow, void* ppRetRow)
{
    MIB_TABLE_T*   pTable;
    UINT32         i;
    void*          pRowData;
    
    pTable = mib_GetTablePtr(tableIndex);
    if (!pTable)
    {
        return FALSE;
    }

    if ((NULL == pTable->pSnapshotData) || (0 == pTable->snapshotEntryCount))
    {
        return FALSE;
    }

    pRowData = pTable->pSnapshotData;
    for (i = 1; i <= pTable->snapshotEntryCount; i++)
    {
        if (0 == mib_CompareEntry(tableIndex, pRowData, pObjectRow))
        {
            if (i == pTable->snapshotEntryCount)
            {
                return FALSE;
            }
            else
            {
                GOS_SetUINT32((UINT32*)ppRetRow, (UINT32)pRowData + MIB_GetTableEntrySize(tableIndex));
                return TRUE;
            }
        }

        pRowData = (void*)((UINT32)pRowData + MIB_GetTableEntrySize(tableIndex));
    }

    return FALSE;
}


GOS_ERROR_CODE mib_AddEntry(MIB_TABLE_INDEX tableIndex, void* pWhere, void* pObjectRow)
{
	MIB_TABLE_T*   pTable;
	MIB_ENTRY_T*   pEntry;
	int tableSize = 0;
    
    	pTable = mib_GetTablePtr(tableIndex);
    	if (!pTable)
    	{
        	return FALSE;
   	}
    	if (pTable->curEntryCount >= MIB_GetTableMaxEntry(tableIndex))
    	{
        	return GOS_FAIL;
    	}

	pEntry = (MIB_ENTRY_T*) malloc(sizeof(MIB_ENTRY_T));

	GOS_ASSERT(pEntry);
	
	tableSize = MIB_GetTableEntrySize(tableIndex);
	pEntry->pData = malloc(tableSize);

	GOS_ASSERT(pEntry->pData);
	
	memcpy(pEntry->pData, pObjectRow,tableSize);
	LIST_INSERT_HEAD(&pTable->entryHead,pEntry,entries);
    	pTable->curEntryCount++;

    	return GOS_OK;
}


GOS_ERROR_CODE mib_DeleteEntry(MIB_TABLE_INDEX tableIndex, void* pObjectRow)
{
    MIB_TABLE_T*   pTable;
	MIB_ENTRY_T*   pEntry;
    
    pTable = mib_GetTablePtr(tableIndex);
    if (!pTable)
    {
        return FALSE;
    }

    if (0 == pTable->curEntryCount)
    {
        return GOS_FAIL;
    }

	LIST_FOREACH(pEntry,&pTable->entryHead,entries)
	{
		if(!memcmp(pObjectRow,pEntry->pData,MIB_GetTableEntrySize(tableIndex)))
		{
			LIST_REMOVE(pEntry,entries);
			LIST_REMOVE(pEntry,treeEntry);
			free(pEntry->pData);
			free(pEntry);
			pTable->curEntryCount--;
			return GOS_OK;
		}
	}

	return GOS_FAIL;
}




GOS_ERROR_CODE mib_SetAttributes(MIB_TABLE_INDEX tableIndex, void* pObjRow, void* pRefRow, MIB_ATTRS_SET* pAttrsSet)
{
    MIB_ATTR_INDEX attrIndex;
    UINT32         i;
    CHAR           attrBuf[MIB_TABLE_ATTR_MAX_SIZE];

    for (attrIndex = MIB_ATTR_FIRST_INDEX, i = 0; i < MIB_GetTableAttrNum(tableIndex);
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (MIB_IsInAttrSet(pAttrsSet, attrIndex))
        {
            if (GOS_OK != MIB_GetAttrFromBuf(tableIndex, attrIndex, attrBuf, pRefRow, MIB_GetAttrSize(tableIndex, attrIndex)))
            {
                return GOS_FAIL;
            }
            if (GOS_OK != mib_SetGetAttribute(tableIndex, attrBuf, pObjRow, attrIndex, TRUE))
            {
                return GOS_FAIL;
            }
        }
    }

    return GOS_OK;
}


BOOL mib_TableSave(MIB_TABLE_INDEX tableIndex)
{
    MIB_ATTR_INDEX attrIndex;
    UINT32 i;
    
    for (attrIndex = MIB_ATTR_FIRST_INDEX, i = 0; i < MIB_GetTableAttrNum(tableIndex);
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (MIB_GetAttrMibSave(tableIndex, attrIndex))
        {
            return TRUE;
        }
    }

    return FALSE;
}


void* mib_GetAttrPtr(MIB_TABLE_INDEX tableIndex, void* pRow, MIB_ATTR_INDEX attrIndex)
{
    MIB_ATTR_INDEX tempAttrIndex;
    UINT32 i;
    UINT32 offset = (UINT32)pRow;


    for (tempAttrIndex = MIB_ATTR_FIRST_INDEX, i = 0; i < MIB_GetTableAttrNum(tableIndex);
         i++, tempAttrIndex = MIB_ATTR_NEXT_INDEX(tempAttrIndex))
    {
        switch(MIB_GetAttrDataType(tableIndex, tempAttrIndex))
        {
            case MIB_ATTR_TYPE_UINT16:
            {
                offset = (offset + 1) / 2 * 2;
                break;
            }
            case MIB_ATTR_TYPE_UINT32:
            case MIB_ATTR_TYPE_UINT64:
            {
                offset = (offset + 3) / 4 * 4;
                break;
            }
            default:
                break;
        }
        
        if (tempAttrIndex == attrIndex)
        {
            return (void*)offset;
        }

        offset += MIB_GetAttrSize(tableIndex, tempAttrIndex);
    }

    return NULL;
}


GOS_ERROR_CODE mib_SetGetAttribute(MIB_TABLE_INDEX tableIndex, void* pAttr, void* pRow, MIB_ATTR_INDEX attrIndex, BOOL set)
{
    void*  attrPtr = mib_GetAttrPtr(tableIndex, pRow, attrIndex);
    UINT32 attrSize = MIB_GetAttrSize(tableIndex, attrIndex);
    
    if (attrPtr)
    {
        if (set)
        {
            memcpy(attrPtr, pAttr, attrSize);
        }
        else
        {
            memcpy(pAttr, attrPtr, attrSize);
        }
        
        return GOS_OK;
    }
    
    return GOS_FAIL;
}

GOS_ERROR_CODE mib_DeleteSubTableEntry(MIB_TABLE_INDEX tableIndex, PON_ME_ENTITY_ID entityId)
{
    MIB_ATTR_INDEX   attrIndex;
    UINT32           i;
    CHAR             rowBuff[MIB_TABLE_ENTRY_MAX_SIZE];
    MIB_TABLE_INDEX  subTableIndex;
    GOS_ERROR_CODE   ret = GOS_OK;
    PON_ME_ENTITY_ID entryEntityId;

    for (attrIndex = MIB_ATTR_FIRST_INDEX, i = 0; i < MIB_GetTableAttrNum(tableIndex);
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (MIB_ATTR_TYPE_TABLE == MIB_GetAttrDataType(tableIndex, attrIndex))
        {
            subTableIndex = MIB_GetTableIndexByName(MIB_GetAttrName(tableIndex, attrIndex));
            if (MIB_TABLE_INDEX_VALID(subTableIndex))
            {
                ret = MIB_GetFirst(subTableIndex, rowBuff, MIB_GetTableEntrySize(subTableIndex));
                while (GOS_OK == ret)
                {                    
                    MIB_GetAttrFromBuf(subTableIndex, MIB_ATTR_FIRST_INDEX, &entryEntityId, rowBuff, sizeof(UINT16));
                    if (entryEntityId == entityId)
                    {
                        MIB_Delete(subTableIndex, rowBuff, MIB_GetTableEntrySize(subTableIndex));
                    }
                    ret = MIB_GetNext(subTableIndex, rowBuff, MIB_GetTableEntrySize(subTableIndex));    
                }
            }
        }
    }

    return GOS_OK;
}



GOS_ERROR_CODE mib_SetDirty(BOOL dirty)
{
    gMibDirty = dirty;
    return GOS_OK;
}


GOS_ERROR_CODE MIB_Clear(MIB_TABLE_INDEX tableIndex)
{
    MIB_TABLE_T*     pTable;
    pTable = mib_GetTablePtr(tableIndex);
    if (!pTable)
    {
        return GOS_ERR_PARAM;
    }
    
    MIB_LOG("MIB_Clear: table %s", MIB_GetTableName(tableIndex));
	pTable->curEntryCount = 0;
    mib_SetDirty(TRUE);
	LIST_INIT(&pTable->entryHead);	
	
    return GOS_OK;
}

BOOL MIB_IsDirty(void)
{
    return gMibDirty;
}


MIB_TABLE_INDEX MIB_GetTableIndexByName(const CHAR* name)
{
    MIB_TABLE_INDEX   tableIndex;
    
    for (tableIndex = MIB_TABLE_FIRST_INDEX; tableIndex <= MIB_TABLE_LAST_INDEX; tableIndex = MIB_TABLE_NEXT_INDEX(tableIndex))
    {
        if (0 == strcmp(name, MIB_GetTableName(tableIndex)))
        {
            return tableIndex;
        }
    }

    return MIB_TABLE_UNKNOWN_INDEX;
}


UINT32 MIB_GetTableCurEntryCount(MIB_TABLE_INDEX tableIndex)
{
    MIB_TABLE_T* pTable;
    
    pTable = mib_GetTablePtr(tableIndex);

    if (!pTable)
    {
        return 0;
    }

    return pTable->curEntryCount;
}


UINT32 MIB_GetTableEntrySizeBeforePadded(MIB_TABLE_INDEX tableIndex)
{
    MIB_ATTR_INDEX    attrIndex;
    UINT32            i;
    UINT32            size = 0;
    
    for (attrIndex = MIB_ATTR_FIRST_INDEX, i = 0; i < MIB_GetTableAttrNum(tableIndex);
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        size += MIB_GetAttrLen(tableIndex, attrIndex);
    }

    return size;
}


MIB_ATTR_INDEX MIB_GetAttrIndexByName(MIB_TABLE_INDEX tableIndex, const CHAR* name)
{
    MIB_ATTR_INDEX    attrIndex;
    UINT32            i;

    for (attrIndex = MIB_ATTR_FIRST_INDEX, i = 0; i < MIB_GetTableAttrNum(tableIndex);
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (0 == strcmp(MIB_GetAttrName(tableIndex, attrIndex), name))
        {
            return attrIndex;
        }
    }

    return MIB_ATTR_UNKNOWN_INDEX;
}


UINT32 MIB_GetAttrSize(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex)
{
    switch(MIB_GetAttrDataType(tableIndex, attrIndex))
    {
        case MIB_ATTR_TYPE_UINT8:
            return 1;
        case MIB_ATTR_TYPE_UINT16:
            return 2;
        case MIB_ATTR_TYPE_UINT32:
            return 4;
        case MIB_ATTR_TYPE_UINT64:
            return 8;
        case MIB_ATTR_TYPE_STR:
            return MIB_GetAttrLen(tableIndex, attrIndex) + 1;
        case MIB_ATTR_TYPE_TABLE:
            return MIB_GetAttrLen(tableIndex, attrIndex);
    }

    return 0;
}


GOS_ERROR_CODE MIB_RegisterCallback(MIB_TABLE_INDEX tableIndex, MIB_CALLBACK_FUNC pfnPreCheck, MIB_CALLBACK_FUNC pfnCallback)
{
    UINT32         i;
    MIB_TABLE_T*   pTable;
    

    if ((NULL == pfnPreCheck) && (NULL == pfnCallback))
    {
        return GOS_ERR_PARAM;
    }

    pTable = mib_GetTablePtr(tableIndex);

    if (!pTable)
    {
        return GOS_ERR_PARAM;
    }
    
    if (pfnPreCheck)
    {
        for (i = 0; i < MIB_TABLE_CB_MAX_NUM; i++)
        {
            if (pTable->preCheckCB[i] == pfnPreCheck)
            {
                return GOS_ERR_DUPLICATED;
            }

            if (0 == pTable->preCheckCB[i])
            {
                pTable->preCheckCB[i] = pfnPreCheck;
                break;
            }
        }

        GOS_ASSERT(i != MIB_TABLE_CB_MAX_NUM);
    }

    if (pfnCallback)
    {
        for (i = 0; i < MIB_TABLE_CB_MAX_NUM; i++)
        {
            if (pTable->operationCB[i] == pfnCallback)
            {
                return GOS_ERR_DUPLICATED;
            }

            if (0 == pTable->operationCB[i])
            {
                pTable->operationCB[i] = pfnCallback;
                break;
            }
        }

        GOS_ASSERT(i != MIB_TABLE_CB_MAX_NUM);
    }

    return GOS_OK;
}


GOS_ERROR_CODE MIB_RegisterCallbackToAll(MIB_CALLBACK_FUNC pfnPreCheck, MIB_CALLBACK_FUNC pfnCallback)
{
    MIB_TABLE_INDEX tableIndex;
    
    if ((NULL == pfnPreCheck) && (NULL == pfnCallback))
    {
        return GOS_ERR_PARAM;
    }
    
    for (tableIndex = MIB_TABLE_FIRST_INDEX; tableIndex <= MIB_TABLE_LAST_INDEX; tableIndex = MIB_TABLE_NEXT_INDEX(tableIndex))
    {
        MIB_RegisterCallback(tableIndex, pfnPreCheck, pfnCallback);
    }

    return GOS_OK;
}


GOS_ERROR_CODE MIB_ClearAttrSet(MIB_ATTRS_SET* pAttrSet)
{
    memset(pAttrSet, 0x00, sizeof(MIB_ATTRS_SET));

    return GOS_OK;
}


GOS_ERROR_CODE MIB_SetAttrSet(MIB_ATTRS_SET* pAttrSet, MIB_ATTR_INDEX attrIndex)
{
    (*pAttrSet) = (*pAttrSet) | (1 << (attrIndex - MIB_ATTR_FIRST_INDEX));
    return GOS_OK;
}


BOOL MIB_IsInAttrSet(MIB_ATTRS_SET* pAttrSet, MIB_ATTR_INDEX attrIndex)
{
    return ((*pAttrSet) & (1 << (attrIndex - MIB_ATTR_FIRST_INDEX)));
}


GOS_ERROR_CODE MIB_FullAttrSet(MIB_ATTRS_SET* pAttrSet)
{
    *pAttrSet = (MIB_ATTRS_SET)0xFFFFFFFF;
    return GOS_OK;
}


UINT32 MIB_GetAttrNumOfSet(MIB_ATTRS_SET* pAttrSet)
{
    MIB_ATTR_INDEX    attrIndex;
    UINT32            i;
    UINT32            count = 0;

    for (attrIndex = MIB_ATTR_FIRST_INDEX, i = 0; i < MIB_TABLE_ATTR_MAX_NUM;
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (MIB_IsInAttrSet(pAttrSet, attrIndex))
        {
            count++;
        }
    }

    return count;
}


MIB_ENTRY_T* MIB_GetTableEntry(MIB_TABLE_INDEX tableIndex, void* pObjectRow)
{
    MIB_TABLE_T*   pTable;
	MIB_ENTRY_T *np;
	void *pRowData;
	
    pTable = mib_GetTablePtr(tableIndex);
	
    if (!pTable)
    {
        return NULL;
    }

    if (0 == pTable->curEntryCount)
    {
        return NULL;
    }

	LIST_FOREACH(np,&pTable->entryHead,entries)
	{
		pRowData = np->pData;
		if (0 == mib_CompareEntry(tableIndex, pRowData, pObjectRow))
        {
            return np;
        }
	}


    return NULL;
}




GOS_ERROR_CODE MIB_Get(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen)
{
    MIB_ATTRS_SET    attrSet;
    void*            pMibRow;
    GOS_ERROR_CODE   ret = GOS_FAIL;
    PON_ME_ENTITY_ID entityID;

    MIB_FullAttrSet(&attrSet);
    GOS_ASSERT(rowLen == MIB_GetTableEntrySize(tableIndex));

    MIB_GetAttrFromBuf(tableIndex, MIB_ATTR_FIRST_INDEX, &entityID, pRow, sizeof(PON_ME_ENTITY_ID));
    

    if (mib_FindEntry(tableIndex, pRow, &pMibRow))
    {
        CHAR tempEntry[MIB_TABLE_ENTRY_MAX_SIZE];
        memcpy(tempEntry, pMibRow, rowLen);

        if (GOS_OK == mib_InvokeCallbacks(tableIndex, tempEntry, tempEntry, &attrSet, MIB_GET))
        {
            memcpy(pRow, tempEntry, rowLen);
            ret = GOS_OK;
        }
        else
        {
            MIB_LOG("Callbacks return failure in MIB_Get: %s, 0x%x", 
                      MIB_GetTableName(tableIndex), entityID);
        }
    }
    else
    {
        MIB_LOG("Entry not found in MIB_Get: %s, 0x%x", 
                  MIB_GetTableName(tableIndex), entityID);
    }

    return ret;
}


GOS_ERROR_CODE MIB_GetNext(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen)
{
    MIB_ATTRS_SET    attrSet;
    void*            pMibRow;
    GOS_ERROR_CODE   ret = GOS_FAIL;
    PON_ME_ENTITY_ID entityID;

    MIB_FullAttrSet(&attrSet);

    GOS_ASSERT(rowLen == MIB_GetTableEntrySize(tableIndex));

    MIB_GetAttrFromBuf(tableIndex, MIB_ATTR_FIRST_INDEX, &entityID, pRow, sizeof(PON_ME_ENTITY_ID));
    
    if (mib_FindNextEntry(tableIndex, pRow, &pMibRow))
    {
        CHAR tempEntry[MIB_TABLE_ENTRY_MAX_SIZE];
        memcpy(tempEntry, pMibRow, rowLen);

        if (GOS_OK == mib_InvokeCallbacks(tableIndex, tempEntry, tempEntry, &attrSet, MIB_GETNEXT))
        {
            memcpy(pRow, tempEntry, rowLen);
            ret = GOS_OK;
        }
        else
        {
            MIB_LOG("Callbacks return failure in MIB_GetNext: %s, 0x%x", 
                      MIB_GetTableName(tableIndex), entityID);
        }
    }

    return ret;
}


GOS_ERROR_CODE MIB_GetFirst(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen)
{
    MIB_TABLE_T*     pTable;
    MIB_ATTRS_SET    attrSet;
    GOS_ERROR_CODE   ret = GOS_FAIL;
    PON_ME_ENTITY_ID entityID;
	MIB_ENTRY_T*	 n1;

    pTable = mib_GetTablePtr(tableIndex);
    if (!pTable)
    {
        return GOS_ERR_PARAM;
    }

    MIB_FullAttrSet(&attrSet);
	
    GOS_ASSERT(rowLen == MIB_GetTableEntrySize(tableIndex));
    
    if (pTable->curEntryCount)
    {
        CHAR tempEntry[MIB_TABLE_ENTRY_MAX_SIZE];
		n1 = LIST_FIRST(&pTable->entryHead);  
		memcpy(tempEntry,n1->pData,rowLen);

		MIB_GetAttrFromBuf(tableIndex, MIB_ATTR_FIRST_INDEX, &entityID, pRow, sizeof(PON_ME_ENTITY_ID));

        if (GOS_OK == mib_InvokeCallbacks(tableIndex, tempEntry, tempEntry, &attrSet, MIB_GET))
        {
            memcpy(pRow, tempEntry, rowLen);
            ret = GOS_OK;
        }
        else
        {
            MIB_LOG("Callbacks return failure in MIB_GetFirst: %s, 0x%x", 
                      MIB_GetTableName(tableIndex), entityID);
        }
    }

    return ret;
}


GOS_ERROR_CODE MIB_Set(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen)
{
    MIB_ATTRS_SET    attrSet;
    void*            pMibRow;
    GOS_ERROR_CODE   ret = GOS_FAIL;
    PON_ME_ENTITY_ID entityID;

    MIB_FullAttrSet(&attrSet);

    GOS_ASSERT(rowLen == MIB_GetTableEntrySize(tableIndex));

    MIB_GetAttrFromBuf(tableIndex, MIB_ATTR_FIRST_INDEX, &entityID, pRow, sizeof(PON_ME_ENTITY_ID));
    
    MIB_LOG("MIB_Set: table %s, entity 0x%x", MIB_GetTableName(tableIndex), entityID);
                              
    if (mib_FindEntry(tableIndex, pRow, &pMibRow))
    {
    
        CHAR tempEntry[MIB_TABLE_ENTRY_MAX_SIZE];
        memcpy(tempEntry, pMibRow, rowLen);

        if (GOS_OK == mib_InvokeCallbacks(tableIndex, tempEntry, pRow, &attrSet, MIB_SET))
        {
        
            memcpy(pMibRow, pRow, rowLen);
            ret = GOS_OK;
            mib_SetDirty(TRUE);
        }
        else
        {
            MIB_LOG("Callbacks return failure in MIB_Set: %s, 0x%x", 
                      MIB_GetTableName(tableIndex), entityID);
        }
    }
    else
    {
    
        if (MIB_GetTableCurEntryCount(tableIndex) < MIB_GetTableMaxEntry(tableIndex))
        {
        
            if (GOS_OK == mib_InvokeCallbacks(tableIndex, NULL, pRow, &attrSet, MIB_ADD))
            {
            
                mib_AddEntry(tableIndex, pMibRow, pRow);
                ret = GOS_OK;
                mib_SetDirty(TRUE);
            }
            else
            {
                MIB_LOG("Callbacks return failure in MIB_Set: %s, 0x%x", 
                          MIB_GetTableName(tableIndex), entityID);
            }
        }
    }

    return ret;
}


GOS_ERROR_CODE MIB_SetAttributes(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen, MIB_ATTRS_SET* pAttrSet)
{
    void*            pMibRow;
    GOS_ERROR_CODE   ret = GOS_FAIL;
    PON_ME_ENTITY_ID entityID;

    GOS_ASSERT(rowLen == MIB_GetTableEntrySize(tableIndex));

    MIB_GetAttrFromBuf(tableIndex, MIB_ATTR_FIRST_INDEX, &entityID, pRow, sizeof(PON_ME_ENTITY_ID));
    
    MIB_LOG("MIB_SetAttributes: table %s, entity 0x%x, set 0x%x", 
                              MIB_GetTableName(tableIndex), entityID, *pAttrSet);

    if (mib_FindEntry(tableIndex, pRow, &pMibRow))
    {
        CHAR tempEntry[MIB_TABLE_ENTRY_MAX_SIZE];
        memcpy(tempEntry, pMibRow, rowLen);

        if (GOS_OK == mib_InvokeCallbacks(tableIndex, tempEntry, pRow, pAttrSet, MIB_SET))
        {
            ret = mib_SetAttributes(tableIndex, pMibRow, pRow, pAttrSet);
            mib_SetDirty(TRUE);
        }
        else
        {
            MIB_LOG("Callbacks return failure in MIB_SetAttributes: %s, 0x%x", 
                      MIB_GetTableName(tableIndex), entityID);
        }
    }
    else
    {
        MIB_LOG("Entry not found in MIB_SetAttributes: %s, 0x%x", 
                  MIB_GetTableName(tableIndex), entityID);

    }

    return ret;
}


GOS_ERROR_CODE MIB_Delete(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen)
{
    MIB_ATTRS_SET    attrSet;
    void*            pMibRow;
    GOS_ERROR_CODE   ret = GOS_FAIL;
    PON_ME_ENTITY_ID entityID;

    MIB_FullAttrSet(&attrSet);

    GOS_ASSERT(rowLen == MIB_GetTableEntrySize(tableIndex));

    MIB_GetAttrFromBuf(tableIndex, MIB_ATTR_FIRST_INDEX, &entityID, pRow, sizeof(PON_ME_ENTITY_ID));
    
    MIB_LOG("MIB_Delete: table %s, entity 0x%x", 
                              MIB_GetTableName(tableIndex), entityID);

    if (mib_FindEntry(tableIndex, pRow, &pMibRow))
    {
        CHAR tempEntry[MIB_TABLE_ENTRY_MAX_SIZE];
        memcpy(tempEntry, pMibRow, rowLen);

        if (GOS_OK == mib_InvokeCallbacks(tableIndex, tempEntry, tempEntry, &attrSet, MIB_DEL))
        {
            mib_DeleteEntry(tableIndex, pMibRow);
            mib_SetDirty(TRUE);
            mib_DeleteSubTableEntry(tableIndex, entityID);
            ret = GOS_OK;
        }
        else
        {
            MIB_LOG("Callbacks return failure in MIB_Delete: %s, 0x%x", 
                      MIB_GetTableName(tableIndex), entityID);
        }
    }
    else
    {
        MIB_LOG("Entry not found in MIB_Delete: %s, 0x%x", 
                  MIB_GetTableName(tableIndex), entityID);
    }

    return ret;
}


GOS_ERROR_CODE MIB_ClearAll(void)
{
   MIB_TABLE_INDEX tableIndex;
   
    for (tableIndex = MIB_TABLE_FIRST_INDEX; tableIndex <= MIB_TABLE_LAST_INDEX; tableIndex = MIB_TABLE_NEXT_INDEX(tableIndex))
    {
        MIB_Clear(tableIndex);
    }

    return GOS_OK;
}


GOS_ERROR_CODE MIB_Default(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen)
{
    void* pDefaultRow = mib_GetDefaultRow(tableIndex);
    GOS_ASSERT(rowLen == MIB_GetTableEntrySize(tableIndex));
    
    if (pRow)
    {
        memcpy(pRow, pDefaultRow, rowLen);
        return GOS_OK;
    }

    return GOS_ERR_PARAM;
}


GOS_ERROR_CODE MIB_SetAttrToBuf(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex, void *pAttr, void *pBuf, UINT32 size)
{
    GOS_ASSERT(size == MIB_GetAttrSize(tableIndex, attrIndex));

    return mib_SetGetAttribute(tableIndex, pAttr, pBuf, attrIndex, TRUE);
}


GOS_ERROR_CODE MIB_GetAttrFromBuf(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex, void *pAttr, void *pBuf, UINT32 size)
{
    GOS_ASSERT(size == MIB_GetAttrSize(tableIndex, attrIndex));

    return mib_SetGetAttribute(tableIndex, pAttr, pBuf, attrIndex, FALSE);
}


GOS_ERROR_CODE MIB_CreateSnapshot(MIB_TABLE_INDEX tableIndex)
{
	int i=0;
    MIB_TABLE_T*   pTable;
	MIB_ENTRY_T*   pEntry;
	INT32 entrySize;
	void *curPtr;
	
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"%s: TableId %d",__FUNCTION__,tableIndex);
    pTable = mib_GetTablePtr(tableIndex);
    if (!pTable)
    {    	
		OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"%s: err parem",__FUNCTION__);
        return GOS_ERR_PARAM;
    }

    if (pTable->pSnapshotData)
    {    	
		OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"%s: err duplicated",__FUNCTION__);
        return GOS_ERR_DUPLICATED;
    }
	entrySize= MIB_GetTableEntrySize(tableIndex);

    pTable->pSnapshotData = malloc((pTable->curEntryCount) * entrySize);
    GOS_ASSERT(pTable->pSnapshotData);
    memset(pTable->pSnapshotData, 0x00, (pTable->curEntryCount)*entrySize);
    pTable->snapshotEntryCount = pTable->curEntryCount;
	
	curPtr = pTable->pSnapshotData;
	LIST_FOREACH(pEntry,&pTable->entryHead,entries)
	{
		memcpy(curPtr,pEntry->pData, entrySize);

		curPtr+=entrySize;
		i++;
	}
    return GOS_OK;
}



GOS_ERROR_CODE MIB_DeleteSnapshot(MIB_TABLE_INDEX tableIndex)
{
    MIB_TABLE_T*   pTable;
    pTable = mib_GetTablePtr(tableIndex);
    if (!pTable)
    {
        return GOS_ERR_PARAM;
    }

    if (!pTable->pSnapshotData)
    {
        return GOS_ERR_DUPLICATED;
    }

    free(pTable->pSnapshotData);
    pTable->pSnapshotData = NULL;
    pTable->snapshotEntryCount = 0;
    
    return GOS_OK;
}


UINT32 MIB_GetSnapshotEntryCount(MIB_TABLE_INDEX tableIndex)
{
    MIB_TABLE_T*   pTable;
    pTable = mib_GetTablePtr(tableIndex);
    if (!pTable)
    {
        return GOS_ERR_PARAM;
    }

    return pTable->snapshotEntryCount;
}


GOS_ERROR_CODE MIB_GetSnapshot(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen)
{
    void*          pMibRow;
    GOS_ERROR_CODE ret = GOS_FAIL;

    GOS_ASSERT(rowLen == MIB_GetTableEntrySize(tableIndex));
    
    if (mib_FindSnapshotEntry(tableIndex, pRow, &pMibRow))
    {
        memcpy(pRow, pMibRow, rowLen);
        ret = GOS_OK;
    }
    else
    {
        MIB_LOG("Entry not found in MIB_GetSnapshot: %s", 
                  MIB_GetTableName(tableIndex));
    }

    return ret;
}


GOS_ERROR_CODE MIB_GetSnapshotNext(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen)
{
    void*          pMibRow;
    GOS_ERROR_CODE ret = GOS_FAIL;

    GOS_ASSERT(rowLen == MIB_GetTableEntrySize(tableIndex));
    
    if (mib_FindSnapshotNextEntry(tableIndex, pRow, &pMibRow))
    {
        memcpy(pRow, pMibRow, rowLen);
        ret = GOS_OK;
    }
    else
    {
        MIB_LOG("Entry not found in MIB_GetSnapshotNext: %s", 
                  MIB_GetTableName(tableIndex));
    }

    return ret;
}


GOS_ERROR_CODE MIB_GetSnapshotFirst(MIB_TABLE_INDEX tableIndex, void* pRow, UINT32 rowLen)
{
    MIB_TABLE_T*   pTable;
    GOS_ERROR_CODE ret = GOS_FAIL;

    pTable = mib_GetTablePtr(tableIndex);
    if (!pTable)
    {
        return GOS_ERR_PARAM;
    }

    GOS_ASSERT(rowLen == MIB_GetTableEntrySize(tableIndex));
    
    if (pTable->snapshotEntryCount)
    {
        memcpy(pRow, pTable->pSnapshotData, rowLen);
        ret = GOS_OK;
    }
    else
    {
        MIB_LOG("Entry not found in MIB_GetSnapshotFirst: %s", 
                  MIB_GetTableName(tableIndex));
    }

    return ret;
}




