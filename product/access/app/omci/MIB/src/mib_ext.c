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



MIB_TABLE_INDEX MIB_GetTableIndexByClassId(PON_ME_CLASS_ID classId)
{
    MIB_TABLE_INDEX   tableIndex;
    
    for (tableIndex = MIB_TABLE_FIRST_INDEX; tableIndex <= MIB_TABLE_LAST_INDEX; tableIndex = MIB_TABLE_NEXT_INDEX(tableIndex))
    {
        if (classId == MIB_GetTableClassId(tableIndex))
        {
            return tableIndex;
        }
    }

    return MIB_TABLE_UNKNOWN_INDEX;
}


BOOL MIB_TableSupportAction(MIB_TABLE_INDEX tableIndex, PON_ME_ACTION_TYPE action)
{
    UINT32 actionSet = MIB_GetTableActionType(tableIndex);

    return (actionSet & action);
}


GOS_ERROR_CODE MIB_CreatePublicTblSnapshot(void)
{
   MIB_TABLE_INDEX tableIndex;
   
    for (tableIndex = MIB_TABLE_FIRST_INDEX; tableIndex <= MIB_TABLE_LAST_INDEX; tableIndex = MIB_TABLE_NEXT_INDEX(tableIndex))
    {
        if (PON_ME_STD_TYPE_PRI != MIB_GetTableStdType(tableIndex))
        {
            MIB_CreateSnapshot(tableIndex);
		}
    }
    
    return GOS_OK;
}


GOS_ERROR_CODE MIB_DeletePublicTblSnapshot(void)
{
    MIB_TABLE_INDEX tableIndex;
   
    for (tableIndex = MIB_TABLE_FIRST_INDEX; tableIndex <= MIB_TABLE_LAST_INDEX; tableIndex = MIB_TABLE_NEXT_INDEX(tableIndex))
    {
        if (PON_ME_STD_TYPE_PRI != MIB_GetTableStdType(tableIndex))
        {
            MIB_DeleteSnapshot(tableIndex);
        }
    }
    
    return GOS_OK;
}


GOS_ERROR_CODE MIB_ClearPublic(void)
{
    MIB_TABLE_INDEX tableIndex;
    for (tableIndex = MIB_TABLE_FIRST_INDEX; tableIndex <= MIB_TABLE_LAST_INDEX; tableIndex = MIB_TABLE_NEXT_INDEX(tableIndex))
    {
        if (PON_ME_STD_TYPE_PRI != MIB_GetTableStdType(tableIndex))
        {
            MIB_Clear(tableIndex);
        }
    }

    return GOS_OK;
}

