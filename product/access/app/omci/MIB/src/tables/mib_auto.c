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





const CHAR* MIB_GetTableName(MIB_TABLE_INDEX tableIndex)
{
    MIB_TABLE_INFO_T* pTableInfo = mib_GetTableInfoPtr(tableIndex);
    GOS_ASSERT(pTableInfo != NULL);
    return pTableInfo->Name;
}


const CHAR* MIB_GetTableDesc(MIB_TABLE_INDEX tableIndex)
{
    MIB_TABLE_INFO_T* pTableInfo = mib_GetTableInfoPtr(tableIndex);
    GOS_ASSERT(pTableInfo != NULL);
    return pTableInfo->Desc;
}


UINT32 MIB_GetTableMaxEntry(MIB_TABLE_INDEX tableIndex)
{
    MIB_TABLE_INFO_T* pTableInfo = mib_GetTableInfoPtr(tableIndex);
    GOS_ASSERT(pTableInfo != NULL);
    return pTableInfo->MaxEntry;
}


UINT32 MIB_GetTableClassId(MIB_TABLE_INDEX tableIndex)
{
    MIB_TABLE_INFO_T* pTableInfo = mib_GetTableInfoPtr(tableIndex);
    GOS_ASSERT(pTableInfo != NULL);
    return pTableInfo->ClassId;
}


UINT32 MIB_GetTableInitType(MIB_TABLE_INDEX tableIndex)
{
    MIB_TABLE_INFO_T* pTableInfo = mib_GetTableInfoPtr(tableIndex);
    GOS_ASSERT(pTableInfo != NULL);
    return pTableInfo->InitType;
}


UINT32 MIB_GetTableStdType(MIB_TABLE_INDEX tableIndex)
{
    MIB_TABLE_INFO_T* pTableInfo = mib_GetTableInfoPtr(tableIndex);
    GOS_ASSERT(pTableInfo != NULL);
    return pTableInfo->StdType;
}


UINT32 MIB_GetTableActionType(MIB_TABLE_INDEX tableIndex)
{
    MIB_TABLE_INFO_T* pTableInfo = mib_GetTableInfoPtr(tableIndex);
    GOS_ASSERT(pTableInfo != NULL);
    return pTableInfo->ActionType;
}



const CHAR* MIB_GetAttrName(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex)
{
    MIB_ATTR_INFO_T* pAttrInfo = mib_GetAttrInfo(tableIndex, attrIndex);
    GOS_ASSERT(pAttrInfo != NULL);
    return pAttrInfo->Name;
}


const CHAR* MIB_GetAttrDesc(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex)
{
    MIB_ATTR_INFO_T* pAttrInfo = mib_GetAttrInfo(tableIndex, attrIndex);
    GOS_ASSERT(pAttrInfo != NULL);
    return pAttrInfo->Desc;
}


MIB_ATTR_TYPE MIB_GetAttrDataType(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex)
{
    MIB_ATTR_INFO_T* pAttrInfo = mib_GetAttrInfo(tableIndex, attrIndex);
    GOS_ASSERT(pAttrInfo != NULL);
    return pAttrInfo->DataType;
}


UINT32 MIB_GetAttrLen(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex)
{
    MIB_ATTR_INFO_T* pAttrInfo = mib_GetAttrInfo(tableIndex, attrIndex);
    GOS_ASSERT(pAttrInfo != NULL);
    return pAttrInfo->Len;
}


BOOL MIB_GetAttrIsIndex(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex)
{
    MIB_ATTR_INFO_T* pAttrInfo = mib_GetAttrInfo(tableIndex, attrIndex);
    GOS_ASSERT(pAttrInfo != NULL);
    return pAttrInfo->IsIndex;
}


MIB_ATTR_USR_ACC_TYPE MIB_GetAttrUsrAcc(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex)
{
    MIB_ATTR_INFO_T* pAttrInfo = mib_GetAttrInfo(tableIndex, attrIndex);
    GOS_ASSERT(pAttrInfo != NULL);
    return pAttrInfo->UsrAcc;
}


BOOL MIB_GetAttrMibSave(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex)
{
    MIB_ATTR_INFO_T* pAttrInfo = mib_GetAttrInfo(tableIndex, attrIndex);
    GOS_ASSERT(pAttrInfo != NULL);
    return pAttrInfo->MibSave;
}


MIB_ATTR_OUT_STYLE MIB_GetAttrOutStyle(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex)
{
    MIB_ATTR_INFO_T* pAttrInfo = mib_GetAttrInfo(tableIndex, attrIndex);
    GOS_ASSERT(pAttrInfo != NULL);
    return pAttrInfo->OutStyle;
}


UINT32 MIB_GetAttrOltAcc(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex)
{
    MIB_ATTR_INFO_T* pAttrInfo = mib_GetAttrInfo(tableIndex, attrIndex);
    GOS_ASSERT(pAttrInfo != NULL);
    return pAttrInfo->OltAcc;
}


BOOL MIB_GetAttrAvcFlag(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex)
{
    MIB_ATTR_INFO_T* pAttrInfo = mib_GetAttrInfo(tableIndex, attrIndex);
    GOS_ASSERT(pAttrInfo != NULL);
    return pAttrInfo->AvcFlag;
}


UINT32 MIB_GetAttrOptionType(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex)
{
    MIB_ATTR_INFO_T* pAttrInfo = mib_GetAttrInfo(tableIndex, attrIndex);
    GOS_ASSERT(pAttrInfo != NULL);
    return pAttrInfo->OptionType;
}



UINT32 MIB_GetTableAttrNum(MIB_TABLE_INDEX tableIndex)
{

	MIB_TABLE_INFO_T *pTableInfo;

	pTableInfo = mib_GetTableInfoPtr(tableIndex);
	if(pTableInfo)
	{
    	return pTableInfo->attrNum;
	}
	return 0;
}


UINT32 MIB_GetTableEntrySize(MIB_TABLE_INDEX tableIndex)
{

	MIB_TABLE_INFO_T *pTableInfo;

	pTableInfo = mib_GetTableInfoPtr(tableIndex);

	if(pTableInfo)
	{
    	return pTableInfo->entrySize;
	}
    return 0;
}

void MIB_Foreset_Init(void)
{
	LIST_INIT(&forest.treeHead);
	forest.treeCount  = 0;
}




GOS_ERROR_CODE MIB_Init(BOOL bReadOnly, BOOL logEnable, MIB_TABLE_INDEX* pIncludeTbl, MIB_TABLE_INDEX* pExcludeTbl)
{

	/*create tables*/

	LIST_INIT(&head);

	/*init tree*/
	MIB_Foreset_Init();

	/*init to default ME data*/
	Ontg_Init();
	Ont2g_Init();		
	OntData_Init();
	SWImage_Init();
	Cardholder_Init();
	CircuitPack_Init();	
	Anig_Init();
	EthUni_Init();
	GemIwTp_Init();
	GemPortCtp_Init();
	MultiGemIwTp_Init();
	Tcont_Init();
	Scheduler_Init();
	PriQ_Init();
	MacBriPortCfgData_Init();
	MacBriServProf_Init();
	Map8021pServProf_Init();
	VlanTagFilterData_Init();
	VlanTagOpCfgData_Init();

	MacBridgePortPmMonitorHistoryData_Init();
	ExtVlanTagOperCfgData_Init();
	EthPmHistoryData_Init();
	GemTrafficDescriptor_Init();
	ThresholdData1_Init();
	GemTrafficDescriptor_Init();
	GalEthProf_Init();
	OltG_Init();
	GemTrafficDescriptor_Init();
	ThresholdData2_Init();
	EthPmHistoryData2_Init();
	GemTrafficDescriptor_Init();
	EthPmHistoryData3_Init();
	McastOperProf_Init();
	McastSubMonitor_Init();
	McastSubConfInfo_Init();
	VEIP_Init();
	LoIdAuth_Init();
	
    gMibReadOnly = bReadOnly;

    return GOS_OK;
}


MIB_TABLE_INFO_T* mib_GetTableInfoPtr(MIB_TABLE_INDEX tableIndex)
{
	MIB_TABLE_T *pTable;
	pTable = mib_GetTablePtr(tableIndex);

	if(pTable)
	{
 		return pTable->pTableInfo;
    }
    return NULL;
}


void* mib_GetDefaultRow(MIB_TABLE_INDEX tableIndex)
{
   
	MIB_TABLE_INFO_T *pTableInfo;

	pTableInfo = mib_GetTableInfoPtr(tableIndex);

	if(pTableInfo)
	{
 		return pTableInfo->pDefaultRow;
    }
    return NULL;
}


