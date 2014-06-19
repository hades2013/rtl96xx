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

MIB_TABLE_INFO_T gMibVlanTagOpCfgDataTableInfo;
MIB_ATTR_INFO_T  gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_ATTR_NUM];
MIB_TABLE_VLANTAGOPCFGDATA_T gMibVlanTagOpCfgDataDefRow;
MIB_TABLE_OPER_T gMibVlanTagOpCfgDataOper;


static MIB_TREE_T* VlanTagOpCfgDataGetTree(MIB_TABLE_VLANTAGOPCFGDATA_T *pVlanTagOpCfgData,MIB_AVL_KEY_T *pKey)
{	
	BOOL isUni = FALSE;
	MIB_TREE_T *pTree;
	
	switch(pVlanTagOpCfgData->Type)
	{
	case VLANTAGOP_TP_TYPE_ETHUNI:
		isUni = TRUE;
		pTree = MIB_AvlTreeSearchByKey(pVlanTagOpCfgData->Pointer,AVL_KEY_PPTPUNI);
	break;
	case VLANTAGOP_TP_TYPE_8021P:				
		isUni = FALSE;
		pTree = MIB_AvlTreeSearchByKey(pVlanTagOpCfgData->Pointer,AVL_KEY_MAP8021PSERVPROF);
	break;
	case VLANTAGOP_TP_TYPE_MACBRIDGE:				
		isUni = TRUE;
		pTree = MIB_AvlTreeSearchByKey(pVlanTagOpCfgData->Pointer,AVL_KEY_MACBRIPORT_UNI);
		if(!pTree)
		{
			isUni = FALSE;
			pTree = MIB_AvlTreeSearchByKey(pVlanTagOpCfgData->Pointer,AVL_KEY_MACBRIPORT_ANI);
		}
	break;
	case VLANTAGOP_TP_TYPE_GEMIWTP:
		isUni = FALSE;
		pTree = MIB_AvlTreeSearchByKey(pVlanTagOpCfgData->Pointer,AVL_KEY_GEMIWTP);
	break;
	case VLANTAGOP_TP_TYPE_MGEMIWTP:
		isUni = FALSE;
		pTree = MIB_AvlTreeSearchByKey(pVlanTagOpCfgData->Pointer,AVL_KEY_MULTIGEMIWTP);
	break;
	case VLANTAGOP_TP_TYPE_PPTP_ETHUNI:
		isUni = TRUE;
		pTree = MIB_AvlTreeSearchByKey(pVlanTagOpCfgData->Pointer,AVL_KEY_PPTPUNI);
	break;
	default:
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Not support association type!");
		return NULL;
	break;
	}
	*pKey = isUni ? AVL_KEY_VLANTAGOPCFG_UNI : AVL_KEY_VLANTAGOPCFG_ANI;	
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"key is %d",*pKey);

	return pTree;
}

GOS_ERROR_CODE VlanTagOpCfgDataDumpMib(void *pData)
{
	
	MIB_TABLE_VLANTAGOPCFGDATA_T *pVlanTagOpCfgData = (MIB_TABLE_VLANTAGOPCFGDATA_T *)pData;
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"%s","VlanTagOpCfgData");	
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"EntityID: 0x%02x",pVlanTagOpCfgData->EntityID);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"DsTagOpMode: %d",pVlanTagOpCfgData->DsTagOpMode);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"Pointer: 0x%02x",pVlanTagOpCfgData->Pointer);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"Type: %d",pVlanTagOpCfgData->Type);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"UsTagOpMode: %d",pVlanTagOpCfgData->UsTagOpMode);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"UsTagTci: 0x%04x",pVlanTagOpCfgData->UsTagTci);

	
	return GOS_OK;
}


GOS_ERROR_CODE VlanTagOpCfgDataConnCheck(MIB_TREE_T *pTree,MIB_TREE_CONN_T *pConn,PON_ME_ENTITY_ID entityId, void *opt)
{
	MIB_ENTRY_T *pEntry;
	MIB_NODE_T  *pNode;
	MIB_TABLE_VLANTAGOPCFGDATA_T *pVlanTagOpCfgData;

	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Start %s...",__FUNCTION__);
	pNode = MIB_AvlTreeSearch(pTree->root,AVL_KEY_VLANTAGOPCFG_UNI);

	if(pNode)
	{

		LIST_FOREACH(pEntry,&pNode->data.treeNodeHead,entries)
		{
			pVlanTagOpCfgData = (MIB_TABLE_VLANTAGOPCFGDATA_T*)pEntry->pData;

			if(pVlanTagOpCfgData->Pointer == entityId)
			{
				pConn->pVlanTagOpCfg = pVlanTagOpCfgData;
				return GOS_OK;
			}
		}
	}

	pNode = MIB_AvlTreeSearch(pTree->root,AVL_KEY_VLANTAGOPCFG_ANI);
	
	if(pNode)
	{
		LIST_FOREACH(pEntry,&pNode->data.treeNodeHead,entries)
		{
			pVlanTagOpCfgData = (MIB_TABLE_VLANTAGOPCFGDATA_T*)pEntry->pData;

			if(pVlanTagOpCfgData->Pointer == entityId)
			{
				pConn->pVlanTagOpCfg = pVlanTagOpCfgData;
				return GOS_OK;
			}
		}
	}
	return GOS_FAIL;
}



GOS_ERROR_CODE VlanTagOpCfgDataDrvCfg(void* pOldRow,void* pNewRow,MIB_OPERA_TYPE  operationType)
{
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: process end\n", __FUNCTION__);
	return GOS_OK;
}


GOS_ERROR_CODE VlanTagOpCfgDataConnCfg(void* pOldRow,void* pNewRow,MIB_OPERA_TYPE  operationType)
{
	MIB_AVL_KEY_T key;
	MIB_TABLE_VLANTAGOPCFGDATA_T *pVlanTagOpCfgData;
	MIB_TREE_T *pTree = NULL;
	MIB_ENTRY_T *pEntry;


    switch (operationType)
    {
        case MIB_ADD:
        {
            OMCI_LOG(OMCI_LOG_LEVEL_LOW,"VLAN Tag Operation ---- > ADD");
           
            /*check if new connection is complete*/
            pVlanTagOpCfgData = (MIB_TABLE_VLANTAGOPCFGDATA_T *)pNewRow;
            /*find avl tree*/
			pTree =VlanTagOpCfgDataGetTree(pVlanTagOpCfgData,&key);
			if(pTree==NULL)
			{
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Can't find Root Tree");
				return GOS_FAIL;
			}
			/*add new node to tree*/
			if(MIB_AvlTreeNodeAdd(&pTree->root,key,MIB_TABLE_VLANTAGOPCFGDATA_INDEX,pVlanTagOpCfgData)==NULL)
			{				
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Add VlanTagCfgData Mapper Node Fail");
				return GOS_FAIL;
			}
            break;
        }

        case MIB_SET:
        {
            OMCI_LOG(OMCI_LOG_LEVEL_LOW,"VLAN Tag Operation ---- > SET");
			pVlanTagOpCfgData = (MIB_TABLE_VLANTAGOPCFGDATA_T *)pNewRow;
            /*find avl tree*/
			pTree =VlanTagOpCfgDataGetTree(pVlanTagOpCfgData,&key);
			if(pTree==NULL)
			{
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Can't find Root Tree");
				return GOS_FAIL;
			}
            break;
        }
        
        case MIB_DEL:
        {
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"VLAN Tag Operation ---- > DEL");
			pVlanTagOpCfgData = (MIB_TABLE_VLANTAGOPCFGDATA_T *)pNewRow;
			/*find avl tree*/
			pTree =VlanTagOpCfgDataGetTree(pVlanTagOpCfgData,&key);
			if(pTree==NULL)
			{
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Can't find Root Tree");
				return GOS_FAIL;
			}
			pEntry = MIB_AvlTreeEntrySearch(pTree->root,key,pVlanTagOpCfgData->EntityID);
			LIST_REMOVE(pEntry,treeEntry);
			break;
        }
        
        default:
            return GOS_OK;
    }
    
	MIB_TreeConnUpdate(pTree);
    return GOS_OK;
}


GOS_ERROR_CODE VlanTagOpCfgData_Init(void)
{
    gMibVlanTagOpCfgDataTableInfo.Name = "VlanTagOpCfgData";
    gMibVlanTagOpCfgDataTableInfo.Desc = "VLAN Tag Operate Configuration Data";
    gMibVlanTagOpCfgDataTableInfo.MaxEntry = (UINT32)(12);
    gMibVlanTagOpCfgDataTableInfo.ClassId = (UINT32)(78);
    gMibVlanTagOpCfgDataTableInfo.InitType = (UINT32)(PON_ME_INIT_TYPE_BY_OLT);
    gMibVlanTagOpCfgDataTableInfo.StdType = (UINT32)(PON_ME_STD_TYPE_STD);
    gMibVlanTagOpCfgDataTableInfo.ActionType = (UINT32)(PON_ME_ACTION_CREATE | PON_ME_ACTION_DELETE | PON_ME_ACTION_SET | PON_ME_ACTION_GET);
    gMibVlanTagOpCfgDataTableInfo.pAttributes = &(gMibVlanTagOpCfgDataAttrInfo[0]);



	gMibVlanTagOpCfgDataTableInfo.attrNum = MIB_TABLE_VLANTAGOPCFGDATA_ATTR_NUM;
	gMibVlanTagOpCfgDataTableInfo.entrySize = sizeof(MIB_TABLE_VLANTAGOPCFGDATA_T);
	gMibVlanTagOpCfgDataTableInfo.pDefaultRow = &gMibVlanTagOpCfgDataDefRow;


    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Name = "EntityID";
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].Name = "UsTagOpMode";
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGTCI_INDEX - MIB_TABLE_FIRST_INDEX].Name = "UsTagTci";
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_DSTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].Name = "DsTagOpMode";
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_TYPE_INDEX - MIB_TABLE_FIRST_INDEX].Name = "Type";
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_POINTER_INDEX - MIB_TABLE_FIRST_INDEX].Name = "Pointer";

    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Entity ID";
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Upstream VLAN Tagging Operation Mode";
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGTCI_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Upstream VLAN Tag TCI Value";
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_DSTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Downstream VLAN Tagging Operation Mode";
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_TYPE_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Association Type";
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_POINTER_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Association ME Pointer";

    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT8;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGTCI_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_DSTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT8;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_TYPE_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT8;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_POINTER_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;

    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].Len = 1;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGTCI_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_DSTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].Len = 1;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_TYPE_INDEX - MIB_TABLE_FIRST_INDEX].Len = 1;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_POINTER_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;

    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = TRUE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGTCI_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_DSTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_TYPE_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_POINTER_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;

    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGTCI_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_DSTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_TYPE_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_POINTER_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;

    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGTCI_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_DSTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_TYPE_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_POINTER_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;

    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGTCI_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_DSTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_TYPE_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_POINTER_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;

    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_SET_BY_CREATE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE | PON_ME_OLT_SET_BY_CREATE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGTCI_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE | PON_ME_OLT_SET_BY_CREATE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_DSTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE | PON_ME_OLT_SET_BY_CREATE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_TYPE_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE | PON_ME_OLT_SET_BY_CREATE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_POINTER_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE | PON_ME_OLT_SET_BY_CREATE;

    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGTCI_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_DSTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_TYPE_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_POINTER_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;

    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_USTAGTCI_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_DSTAGOPMODE_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_TYPE_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_OPT_SUPPORT;
    gMibVlanTagOpCfgDataAttrInfo[MIB_TABLE_VLANTAGOPCFGDATA_POINTER_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_OPT_SUPPORT;


    memset(&(gMibVlanTagOpCfgDataDefRow.EntityID), 0x00, sizeof(gMibVlanTagOpCfgDataDefRow.EntityID));
    memset(&(gMibVlanTagOpCfgDataDefRow.UsTagOpMode), 0x00, sizeof(gMibVlanTagOpCfgDataDefRow.UsTagOpMode));
    memset(&(gMibVlanTagOpCfgDataDefRow.UsTagTci), 0x00, sizeof(gMibVlanTagOpCfgDataDefRow.UsTagTci));
    memset(&(gMibVlanTagOpCfgDataDefRow.DsTagOpMode), 0x00, sizeof(gMibVlanTagOpCfgDataDefRow.DsTagOpMode));
    memset(&(gMibVlanTagOpCfgDataDefRow.Type), 0x00, sizeof(gMibVlanTagOpCfgDataDefRow.Type));
    memset(&(gMibVlanTagOpCfgDataDefRow.Pointer), 0x00, sizeof(gMibVlanTagOpCfgDataDefRow.Pointer));


	gMibVlanTagOpCfgDataOper.meOperDrvCfg = VlanTagOpCfgDataDrvCfg;
	gMibVlanTagOpCfgDataOper.meOperConnCheck = VlanTagOpCfgDataConnCheck;
	gMibVlanTagOpCfgDataOper.meOperDump = VlanTagOpCfgDataDumpMib;
	gMibVlanTagOpCfgDataOper.meOperConnCfg = VlanTagOpCfgDataConnCfg;

	MIB_Register(MIB_TABLE_VLANTAGOPCFGDATA_INDEX,&gMibVlanTagOpCfgDataTableInfo,&gMibVlanTagOpCfgDataOper);

    return GOS_OK;
}


