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
#include "omci_defs.h"

MIB_TABLE_INFO_T gMibVlanTagFilterDataTableInfo;
MIB_ATTR_INFO_T  gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_ATTR_NUM];
MIB_TABLE_VLANTAGFILTERDATA_T gMibVlanTagFilterDataDefRow;
MIB_TABLE_OPER_T gMibVlanTagFilterDataOper;


static MIB_TREE_T* VlanTagFilterDataGetTree(MIB_TABLE_VLANTAGFILTERDATA_T *pVlanTagFilterData,MIB_AVL_KEY_T* pKey)
{
	MIB_TREE_T *pTree;
	BOOL isUni = FALSE;
	isUni = TRUE;
	pTree = MIB_AvlTreeSearchByKey(pVlanTagFilterData->EntityID,AVL_KEY_MACBRIPORT_UNI);
	if(!pTree)
	{
		isUni = FALSE;
		pTree = MIB_AvlTreeSearchByKey(pVlanTagFilterData->EntityID,AVL_KEY_MACBRIPORT_ANI);
	}
	*pKey = isUni ? AVL_KEY_VLANTAGFILTER_UNI : AVL_KEY_VLANTAGFILTER_ANI;
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"key is %d",*pKey);

	return pTree;
}

GOS_ERROR_CODE VlanTagFilterDataDumpMib(void *pData)
{
	int i,arraySize = 0;
	UINT16 tci;
	MIB_TABLE_VLANTAGFILTERDATA_T *pVlanTagFilter = (MIB_TABLE_VLANTAGFILTERDATA_T*)pData;
		
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"%s","VlanTagFilterData");
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"EntityID: 0x%02x",pVlanTagFilter->EntityID);	
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"FwdOp: %d",pVlanTagFilter->FwdOp);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"NumOfEntries: %d",pVlanTagFilter->NumOfEntries);

	arraySize = pVlanTagFilter->NumOfEntries*2;
	for(i=0;i< arraySize ;i+=2)
	{
		tci = pVlanTagFilter->FilterTbl[i] << 8 | pVlanTagFilter->FilterTbl[i+1];
		OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"FilterTbl[%d]: PRI %d,CFI %d, VID %d",i,(tci >> 13) & 0x7,(tci >> 12) & 0x1,tci & 0xfff);
	}
	return GOS_OK;
}

GOS_ERROR_CODE VlanTagFilterConnCheck(MIB_TREE_T *pTree,MIB_TREE_CONN_T *pConn,PON_ME_ENTITY_ID entityId, void *opt)
{
	MIB_ENTRY_T *pEntry;
	MIB_TABLE_VLANTAGFILTERDATA_T *pVlanTagFilter;

	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Start %s...",__FUNCTION__);
	/*implict relationship*/
	pEntry = MIB_AvlTreeEntrySearch(pTree->root,AVL_KEY_VLANTAGFILTER_UNI,entityId);

	if(!pEntry){		
		pEntry = MIB_AvlTreeEntrySearch(pTree->root,AVL_KEY_VLANTAGFILTER_ANI,entityId);
	}

	if(pEntry){
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: found entity Id 0x02%x",__FUNCTION__,entityId);
		pVlanTagFilter = (MIB_TABLE_VLANTAGFILTERDATA_T*)pEntry->pData;
		pConn->pVlanTagFilter = pVlanTagFilter;
	}else
	{
		pConn->pVlanTagFilter = NULL;
	}
	
	return GOS_OK;
}


GOS_ERROR_CODE VlanTagFilterDataDrvCfg(void* pOldRow,void* pNewRow,MIB_OPERA_TYPE  operationType)
{
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: process end\n", __FUNCTION__);
	return GOS_OK;
}

GOS_ERROR_CODE VlanTagFilterDataConnCfg(void* pOldRow,void* pNewRow,MIB_OPERA_TYPE  operationType)
{
	MIB_TABLE_VLANTAGFILTERDATA_T *pVlanTagFilter;
	MIB_TREE_T *pTree;
	MIB_AVL_KEY_T key;
	MIB_ENTRY_T *pEntry;

	switch (operationType){
        case MIB_ADD:
	{
        	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"VLAN Tag Filter Data ---- > ADD");
		pVlanTagFilter = (MIB_TABLE_VLANTAGFILTERDATA_T*) pNewRow;
		pTree = VlanTagFilterDataGetTree(pVlanTagFilter,&key);
		if(pTree==NULL)
		{
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Can't find Root Tree");
			return GOS_FAIL;
		}
		/*add new node to tree*/
		if(MIB_AvlTreeNodeAdd(&pTree->root,key,MIB_TABLE_VLANTAGFILTERDATA_INDEX,pVlanTagFilter)==NULL)
		{				
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Add VlanTagCfgData Mapper Node Fail");
			return GOS_FAIL;
		}
	}
	break;
	case MIB_SET:
	{
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"VLAN Tag Filter Data ---- > SET");
		pVlanTagFilter = (MIB_TABLE_VLANTAGFILTERDATA_T*) pNewRow;
		pTree = VlanTagFilterDataGetTree(pVlanTagFilter,&key);			
		if(pTree==NULL)
		{
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Can't find Root Tree");
			return GOS_FAIL;
		}
        }
        break;
	case MIB_DEL:
	{
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"VLAN Tag Filter Data ---- > DEL");
		pVlanTagFilter = (MIB_TABLE_VLANTAGFILTERDATA_T*) pNewRow;	
		pTree = VlanTagFilterDataGetTree(pVlanTagFilter,&key);	
		if(pTree==NULL)
		{
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Can't find Root Tree");
			return GOS_FAIL;
		}
		/*add new node to tree*/
		pEntry = MIB_AvlTreeEntrySearch(pTree->root,key,pVlanTagFilter->EntityID);
		LIST_REMOVE(pEntry,treeEntry);
	}
        break;
	default:
		return GOS_OK;
	}
    
	MIB_TreeConnUpdate(pTree);
    	return GOS_OK;
}

GOS_ERROR_CODE VlanTagFilterData_Init(void)
{
    gMibVlanTagFilterDataTableInfo.Name = "VlanTagFilterData";
    gMibVlanTagFilterDataTableInfo.Desc = "VLAN Tag Filter Data";
    gMibVlanTagFilterDataTableInfo.MaxEntry = (UINT32)(144);
    gMibVlanTagFilterDataTableInfo.ClassId = (UINT32)(84);
    gMibVlanTagFilterDataTableInfo.InitType = (UINT32)(PON_ME_INIT_TYPE_BY_OLT);
    gMibVlanTagFilterDataTableInfo.StdType = (UINT32)(PON_ME_STD_TYPE_STD);
    gMibVlanTagFilterDataTableInfo.ActionType = (UINT32)(PON_ME_ACTION_CREATE | PON_ME_ACTION_DELETE | PON_ME_ACTION_SET | PON_ME_ACTION_GET);
    gMibVlanTagFilterDataTableInfo.pAttributes = &(gMibVlanTagFilterDataAttrInfo[0]);


	gMibVlanTagFilterDataTableInfo.attrNum = MIB_TABLE_VLANTAGFILTERDATA_ATTR_NUM;
	gMibVlanTagFilterDataTableInfo.entrySize = sizeof(MIB_TABLE_VLANTAGFILTERDATA_T);
	gMibVlanTagFilterDataTableInfo.pDefaultRow = &gMibVlanTagFilterDataDefRow;


    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Name = "EntityID";
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_INDEX - MIB_TABLE_FIRST_INDEX].Name = "FilterTbl";
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FWDOP_INDEX - MIB_TABLE_FIRST_INDEX].Name = "FwdOp";
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_NUMOFENTRIES_INDEX - MIB_TABLE_FIRST_INDEX].Name = "NumOfEntries";

    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Entity ID";
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "VLAN Filter Table";
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FWDOP_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Forward Operation";
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_NUMOFENTRIES_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Number of Entries";

    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_STR;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FWDOP_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT8;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_NUMOFENTRIES_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT8;

    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_INDEX - MIB_TABLE_FIRST_INDEX].Len = 24;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FWDOP_INDEX - MIB_TABLE_FIRST_INDEX].Len = 1;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_NUMOFENTRIES_INDEX - MIB_TABLE_FIRST_INDEX].Len = 1;

    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = TRUE;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FWDOP_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_NUMOFENTRIES_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;

    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FWDOP_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_NUMOFENTRIES_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;

    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FWDOP_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_NUMOFENTRIES_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;

    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FWDOP_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_NUMOFENTRIES_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;

    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_SET_BY_CREATE;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE | PON_ME_OLT_SET_BY_CREATE;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FWDOP_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE | PON_ME_OLT_SET_BY_CREATE;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_NUMOFENTRIES_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE | PON_ME_OLT_SET_BY_CREATE;

    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FWDOP_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_NUMOFENTRIES_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;

    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_FWDOP_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibVlanTagFilterDataAttrInfo[MIB_TABLE_VLANTAGFILTERDATA_NUMOFENTRIES_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;


    memset(&(gMibVlanTagFilterDataDefRow.EntityID), 0x00, sizeof(gMibVlanTagFilterDataDefRow.EntityID));
    memset(gMibVlanTagFilterDataDefRow.FilterTbl, '\0', MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_LEN + 1);
    gMibVlanTagFilterDataDefRow.FwdOp = MIB_ATTR_DEF_EMPTY;
    gMibVlanTagFilterDataDefRow.NumOfEntries = MIB_ATTR_DEF_EMPTY;


	gMibVlanTagFilterDataOper.meOperDrvCfg = VlanTagFilterDataDrvCfg;
	gMibVlanTagFilterDataOper.meOperConnCheck = VlanTagFilterConnCheck;
	gMibVlanTagFilterDataOper.meOperDump = VlanTagFilterDataDumpMib;
	gMibVlanTagFilterDataOper.meOperConnCfg = VlanTagFilterDataConnCfg;

	MIB_Register(MIB_TABLE_VLANTAGFILTERDATA_INDEX,&gMibVlanTagFilterDataTableInfo,&gMibVlanTagFilterDataOper);


    return GOS_OK;
}


