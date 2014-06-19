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

MIB_TABLE_INFO_T gMibExtVlanTagOperCfgDataTableInfo;
MIB_ATTR_INFO_T  gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ATTR_NUM];
MIB_TABLE_EXTVLANTAGOPERCFGDATA_T gMibExtVlanTagOperCfgDataDefRow;
MIB_TABLE_OPER_T gMibExtVlanTagOperCfgDataOper;

static void ExtVlanTagTableDump(MIB_TABLE_EXTVLANTAGOPERCFGDATA_T* pExtVlan)
{
	extvlanTableEntry_t *pEntry;
	omci_extvlan_row_entry_t *pRowEntry;
	int count=0;

	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"ReceivedFrameVlanTagOperTable");
	LIST_FOREACH(pEntry,&pExtVlan->head,entries){
		pRowEntry = &pEntry->tableEntry;
		OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"INDEX %d",count);
		OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"Filter Outer   : PRI %d,VID %d, TPID %d",
		pRowEntry->outerFilterWord.bit.filterOuterPri,
		pRowEntry->outerFilterWord.bit.filterOuterVid,
		pRowEntry->outerFilterWord.bit.filterOuterTpId);
		OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"Filter Inner   : PRI %d,VID %d, TPID %d, EthType 0x%02x",
		pRowEntry->innerFilterWord.bit.filterInnerPri,
		pRowEntry->innerFilterWord.bit.filterInnerVid,
		pRowEntry->innerFilterWord.bit.filterInnerTpId,
		pRowEntry->innerFilterWord.bit.filterEthType);
		OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"Treatment Outer   : PRI %d,VID %d, TPID %d, RemoveTags %d",
		pRowEntry->outerTreatmentWord.bit.treatmentOuterPri,
		pRowEntry->outerTreatmentWord.bit.treatmentOuterVid,
		pRowEntry->outerTreatmentWord.bit.treatmentOuterTpId,
		pRowEntry->outerTreatmentWord.bit.treatment);
		OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"Treatment Inner   : PRI %d,VID %d, TPID %d",
		pRowEntry->innerTreatmentWord.bit.treatmentInnerPri,
		pRowEntry->innerTreatmentWord.bit.treatmentInnerVid,
		pRowEntry->innerTreatmentWord.bit.treatmentInnerTpId);
		count ++;
	}
}


GOS_ERROR_CODE ExtVlanTagOperCfgDataDumpMib(void *pData)
{
	MIB_TABLE_EXTVLANTAGOPERCFGDATA_T *pExtVlanTagOperCfgData = (MIB_TABLE_EXTVLANTAGOPERCFGDATA_T*)pData;
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"%s", "ExtVlanTagOperCfgData");

	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"EntityId: 0x%02x", pExtVlanTagOperCfgData->EntityId);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"AssociationType: %d", pExtVlanTagOperCfgData->AssociationType);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"ReceivedFrameVlanTagOperTableMaxSize: %d", pExtVlanTagOperCfgData->ReceivedFrameVlanTagOperTableMaxSize);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"InputTPID: 0x%02x", pExtVlanTagOperCfgData->InputTPID);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"OutputTPID: 0x%02x", pExtVlanTagOperCfgData->OutputTPID);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"DsMode: %d", pExtVlanTagOperCfgData->DsMode);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"AssociatedMePoint: 0x%02x", pExtVlanTagOperCfgData->AssociatedMePoint);
	//OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"DscpToPbitMapping: %s", pExtVlanTagOperCfgData->DscpToPbitMapping);
	ExtVlanTagTableDump(pExtVlanTagOperCfgData);
	return GOS_OK;
}

static BOOL ExtVlanEntryCheck(char *extVlanEntry)
{	
	if(strcmp(extVlanEntry,""))
		return TRUE;
	else
		return FALSE;	
}

static BOOL ExtVlanEntryCompare(omci_extvlan_row_entry_t *pEntryA,omci_extvlan_row_entry_t *pEntryB)
{
	if(pEntryA->outerFilterWord.val == pEntryB->outerFilterWord.val &&
	pEntryB->innerFilterWord.val == pEntryB->innerFilterWord.val)
	{
		return 0;
	}

	return 1;
}

static GOS_ERROR_CODE ExtVlanEntryAdd(UINT8 *pReceiveEntry,MIB_TABLE_EXTVLANTAGOPERCFGDATA_T *pExtVlan)
{
	extvlanTableEntry_t *pEntry,*pTmpEntry;
	omci_extvlan_row_entry_t *pRowEntry;
	
	pEntry = (extvlanTableEntry_t*)malloc(sizeof(extvlanTableEntry_t));

	memcpy(&(pEntry->tableEntry),(omci_extvlan_row_entry_t*)pReceiveEntry,sizeof(omci_extvlan_row_entry_t));

	pRowEntry = &pEntry->tableEntry;

	pRowEntry->outerFilterWord.val 		= GOS_Htonl(pRowEntry->outerFilterWord.val);
	pRowEntry->innerFilterWord.val 		= GOS_Htonl(pRowEntry->innerFilterWord.val);	
	pRowEntry->outerTreatmentWord.val 	= GOS_Htonl(pRowEntry->outerTreatmentWord.val);	
	pRowEntry->innerTreatmentWord.val 	= GOS_Htonl(pRowEntry->innerTreatmentWord.val);

	/*compare this entry is exist or not*/
	LIST_FOREACH(pTmpEntry,&pExtVlan->head,entries)
	{
		if(! ExtVlanEntryCompare(pRowEntry,&pTmpEntry->tableEntry))
		{
			pTmpEntry->tableEntry.outerTreatmentWord.val = pRowEntry->outerTreatmentWord.val;
			pTmpEntry->tableEntry.innerTreatmentWord.val = pRowEntry->innerTreatmentWord.val;
			return GOS_OK;
		}
	}
	/*not found, create new entry*/
	LIST_INSERT_HEAD(&pExtVlan->head,pEntry,entries);
	return GOS_OK;
}


GOS_ERROR_CODE ExtVlanTagOperCfgDataDrvCfg(void* pOldRow, void* pNewRow, MIB_OPERA_TYPE operationType)
{
	MIB_TABLE_EXTVLANTAGOPERCFGDATA_T *pExtVlan,*pMibExtVlan,tmpExtVlan;

	pExtVlan = (MIB_TABLE_EXTVLANTAGOPERCFGDATA_T*)pNewRow;
	tmpExtVlan.EntityId = pExtVlan->EntityId;

	if(!mib_FindEntry(MIB_TABLE_EXTVLANTAGOPERCFGDATA_INDEX,&tmpExtVlan,&pMibExtVlan))
	{
		MIB_LOG(OMCI_LOG_LEVEL_LOW,"can't find extvlan 0x%x",pExtVlan->EntityId);
		return GOS_OK;	
	}
	
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"find extvlan %x",pMibExtVlan);

	switch(operationType){
	case MIB_ADD:
	{
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"ExtVlanTagOperCfg --> ADD");		
		LIST_INIT(&pMibExtVlan->head);
	}
	break;
	case MIB_SET:
	{
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"ExtVlanTagOperCfg --> SET");		
		if(ExtVlanEntryCheck(pExtVlan->ReceivedFrameVlanTaggingOperTable))
		{
			ExtVlanEntryAdd(pExtVlan->ReceivedFrameVlanTaggingOperTable,pMibExtVlan);
		}
	}
	break;
	case MIB_DEL:
	{	
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"ExtVlanTagOperCfg --> DEL");		
	}
	break;
	default:	
		return GOS_FAIL;		
	break;
	}
	
	return GOS_OK;
}

static MIB_TREE_T* ExtVlanTagOperCfgDataGetTree(MIB_TABLE_EXTVLANTAGOPERCFGDATA_T *pExtVlan,MIB_AVL_KEY_T *key)
{
	MIB_TREE_T *pTree=NULL;

	switch(pExtVlan->AssociationType){
	case EXTVLAN_TP_TYPE_MACBRIDGEPORT:
	{
		pTree = MIB_AvlTreeSearchByKey(pExtVlan->AssociatedMePoint,AVL_KEY_MACBRIPORT_UNI);
		if(pTree) 
		{
			*key = AVL_KEY_EXTVLAN_ANI;
		}else
		{
			pTree = MIB_AvlTreeSearchByKey(pExtVlan->AssociatedMePoint,AVL_KEY_MACBRIPORT_ANI);
			*key = AVL_KEY_EXTVLAN_UNI;
		}
	}
	break;
	case EXTVLAN_TP_TYPE_8021P:
		pTree = MIB_AvlTreeSearchByKey(pExtVlan->AssociatedMePoint,AVL_KEY_MAP8021PSERVPROF);
		*key = AVL_KEY_EXTVLAN_ANI;
	break;
	case EXTVLAN_TP_TYPE_PPTPETHUNI:
		pTree = MIB_AvlTreeSearchByKey(pExtVlan->AssociatedMePoint,AVL_KEY_PPTPUNI);
		*key = AVL_KEY_EXTVLAN_UNI;
	break;
	case EXTVLAN_TP_TYPE_IPHOST:
		*key = AVL_KEY_EXTVLAN_UNI;
	break;
	case EXTVLAN_TP_TYPE_PPTPXDSLUNI:
		*key = AVL_KEY_EXTVLAN_UNI;
	break;
	case EXTVLAN_TP_TYPE_GEMIWTP:
		pTree = MIB_AvlTreeSearchByKey(pExtVlan->AssociatedMePoint,AVL_KEY_GEMIWTP);
		*key = AVL_KEY_EXTVLAN_ANI;
	break;
	case EXTVLAN_TP_TYPE_MCASTGEMIWTP:
		pTree = MIB_AvlTreeSearchByKey(pExtVlan->AssociatedMePoint,AVL_KEY_MULTIGEMIWTP);
		*key = AVL_KEY_EXTVLAN_ANI;
	break;
	case EXTVLAN_TP_TYPE_PPTPMOCAUNI:
		*key = AVL_KEY_EXTVLAN_UNI;
	break;
	case EXTVLAN_TP_TYPE_PPTP80211UNI:
		*key = AVL_KEY_EXTVLAN_UNI;
	break;
	case EXTVLAN_TP_TYPE_ETHFLOWUNI:
		*key = AVL_KEY_EXTVLAN_UNI;
	break;
	case EXTVLAN_TP_TYPE_VEIP:
		pTree = MIB_AvlTreeSearchByKey(pExtVlan->AssociatedMePoint,AVL_KEY_VEIP);
		*key = AVL_KEY_EXTVLAN_UNI;
	break;
	default:
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Can't support this associationType %d",pExtVlan->AssociationType);
	break; 
	}
	return pTree;
}

GOS_ERROR_CODE ExtVlanTagOperCfgDataConnCheck(MIB_TREE_T *pTree,MIB_TREE_CONN_T *pConn,PON_ME_ENTITY_ID entityId,
void *opt)
{
	MIB_ENTRY_T *pEntry;
	MIB_NODE_T  *pNode;
	MIB_TABLE_EXTVLANTAGOPERCFGDATA_T *pExtVlan;

	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Start %s...",__FUNCTION__);
	pNode = MIB_AvlTreeSearch(pTree->root,AVL_KEY_EXTVLAN_UNI);

	if(pNode)
	{

		LIST_FOREACH(pEntry,&pNode->data.treeNodeHead,entries)
		{
			pExtVlan = (MIB_TABLE_EXTVLANTAGOPERCFGDATA_T*)pEntry->pData;

			if(pExtVlan->AssociatedMePoint == entityId)
			{
				pConn->pExtVlanCfg = pExtVlan;
				return GOS_OK;
			}
		}
	}

	pNode = MIB_AvlTreeSearch(pTree->root,AVL_KEY_EXTVLAN_ANI);
	
	if(pNode)
	{
		LIST_FOREACH(pEntry,&pNode->data.treeNodeHead,entries)
		{
			pExtVlan = (MIB_TABLE_EXTVLANTAGOPERCFGDATA_T*)pEntry->pData;

			if(pExtVlan->AssociatedMePoint == entityId)
			{
				pConn->pExtVlanCfg = pExtVlan;
				return GOS_OK;
			}
		}
	}
	return GOS_FAIL;
}


GOS_ERROR_CODE ExtVlanTagOperCfgDataConnCfg(void* pOldRow, void* pNewRow, MIB_OPERA_TYPE operationType)
{
	MIB_TREE_T *pTree;
	MIB_TABLE_EXTVLANTAGOPERCFGDATA_T *pExtVlan;
	MIB_AVL_KEY_T key;
	MIB_ENTRY_T *pEntry;

	switch(operationType){
	case MIB_ADD:
	{
		pExtVlan = (MIB_TABLE_EXTVLANTAGOPERCFGDATA_T*)pNewRow;
		pTree = ExtVlanTagOperCfgDataGetTree(pExtVlan,&key);
		
		if(!pTree) return GOS_FAIL;

		if(MIB_AvlTreeNodeAdd(&pTree->root,key,MIB_TABLE_EXTVLANTAGOPERCFGDATA_INDEX,pExtVlan)==NULL)
		{				
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Add ExtVlanTagCfgData Node Fail");
			return GOS_FAIL;
		}
	}
	break;
	case MIB_SET:
		pExtVlan = (MIB_TABLE_EXTVLANTAGOPERCFGDATA_T*)pNewRow;
		pTree = ExtVlanTagOperCfgDataGetTree(pExtVlan,&key);
		if(!pTree) return GOS_FAIL;
	break;
	case MIB_DEL:
		pExtVlan = (MIB_TABLE_EXTVLANTAGOPERCFGDATA_T*)pOldRow;
		pTree = ExtVlanTagOperCfgDataGetTree(pExtVlan,&key);
		if(!pTree)
		{
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"can't find root tree");
			return GOS_FAIL;
		}
		pEntry = MIB_AvlTreeEntrySearch(pTree->root,key,pExtVlan->EntityId);
		LIST_REMOVE(pEntry,treeEntry);
	break;
	default:
		return GOS_FAIL;
	break;
	}
	MIB_TreeConnUpdate(pTree);
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: process end\n", __FUNCTION__);
	return GOS_OK;
}


GOS_ERROR_CODE ExtVlanTagOperCfgData_Init(void)
{
    gMibExtVlanTagOperCfgDataTableInfo.Name = "ExtVlanTagOperCfgData";
    gMibExtVlanTagOperCfgDataTableInfo.Desc = "Extended VLAN tagging operation configure data";
    gMibExtVlanTagOperCfgDataTableInfo.MaxEntry = (UINT32)(128);
    gMibExtVlanTagOperCfgDataTableInfo.ClassId = (UINT32)(171);
    gMibExtVlanTagOperCfgDataTableInfo.InitType = (UINT32)(PON_ME_INIT_TYPE_BY_OLT);
    gMibExtVlanTagOperCfgDataTableInfo.StdType = (UINT32)(PON_ME_STD_TYPE_STD);
    gMibExtVlanTagOperCfgDataTableInfo.ActionType = (UINT32)(PON_ME_ACTION_CREATE | PON_ME_ACTION_DELETE | PON_ME_ACTION_SET | PON_ME_ACTION_GET | PON_ME_ACTION_GET_NEXT);
    gMibExtVlanTagOperCfgDataTableInfo.pAttributes = &(gMibExtVlanTagOperCfgDataAttrInfo[0]);

	gMibExtVlanTagOperCfgDataTableInfo.attrNum = MIB_TABLE_EXTVLANTAGOPERCFGDATA_ATTR_NUM;
	gMibExtVlanTagOperCfgDataTableInfo.entrySize = sizeof(MIB_TABLE_EXTVLANTAGOPERCFGDATA_T);
	gMibExtVlanTagOperCfgDataTableInfo.pDefaultRow = &gMibExtVlanTagOperCfgDataDefRow;

    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Name = "EntityId";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATIONTYPE_INDEX - MIB_TABLE_FIRST_INDEX].Name = "AssociationType";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGOPERTABLEMAXSIZE_INDEX - MIB_TABLE_FIRST_INDEX].Name = "ReceivedFrameVlanTagOperTableMaxSize";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_INPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].Name = "InputTPID";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_OUTPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].Name = "OutputTPID";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSMODE_INDEX - MIB_TABLE_FIRST_INDEX].Name = "DsMode";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGGINGOPERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].Name = "ReceivedFrameVlanTaggingOperTable";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATEDMEPOINT_INDEX - MIB_TABLE_FIRST_INDEX].Name = "AssociatedMePoint";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSCPTOPBITMAPPING_INDEX - MIB_TABLE_FIRST_INDEX].Name = "DscpToPbitMapping";

    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Entity ID";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATIONTYPE_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Type of the ME associated";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGOPERTABLEMAXSIZE_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "specifies peak information rate";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_INPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "special TPID value for operations.";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_OUTPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "special TPID value for operations.";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSMODE_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "downstream mode";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGGINGOPERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "table for filters tags upstream frames";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATEDMEPOINT_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "associated ME pointer";
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSCPTOPBITMAPPING_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "DSCP to Pbit mapping";

    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATIONTYPE_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT8;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGOPERTABLEMAXSIZE_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_INPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_OUTPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSMODE_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT8;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGGINGOPERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_TABLE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATEDMEPOINT_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSCPTOPBITMAPPING_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_STR;

    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATIONTYPE_INDEX - MIB_TABLE_FIRST_INDEX].Len = 1;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGOPERTABLEMAXSIZE_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_INPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_OUTPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSMODE_INDEX - MIB_TABLE_FIRST_INDEX].Len = 1;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGGINGOPERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].Len = 16;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATEDMEPOINT_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSCPTOPBITMAPPING_INDEX - MIB_TABLE_FIRST_INDEX].Len = 24;

    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = TRUE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATIONTYPE_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGOPERTABLEMAXSIZE_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_INPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_OUTPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSMODE_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGGINGOPERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATEDMEPOINT_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSCPTOPBITMAPPING_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;

    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATIONTYPE_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGOPERTABLEMAXSIZE_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_INPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_OUTPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSMODE_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGGINGOPERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATEDMEPOINT_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSCPTOPBITMAPPING_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;

    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATIONTYPE_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGOPERTABLEMAXSIZE_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_INPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_OUTPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSMODE_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGGINGOPERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATEDMEPOINT_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSCPTOPBITMAPPING_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;

    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATIONTYPE_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGOPERTABLEMAXSIZE_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_INPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_OUTPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSMODE_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGGINGOPERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATEDMEPOINT_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSCPTOPBITMAPPING_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;

    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ| PON_ME_OLT_SET_BY_CREATE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATIONTYPE_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE | PON_ME_OLT_SET_BY_CREATE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGOPERTABLEMAXSIZE_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_INPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_OUTPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSMODE_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGGINGOPERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATEDMEPOINT_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE | PON_ME_OLT_SET_BY_CREATE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSCPTOPBITMAPPING_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE;

    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATIONTYPE_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGOPERTABLEMAXSIZE_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_INPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_OUTPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSMODE_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGGINGOPERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATEDMEPOINT_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSCPTOPBITMAPPING_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;

    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATIONTYPE_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGOPERTABLEMAXSIZE_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_INPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_OUTPUTTPID_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSMODE_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGGINGOPERTABLE_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATEDMEPOINT_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibExtVlanTagOperCfgDataAttrInfo[MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSCPTOPBITMAPPING_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;

    memset(&(gMibExtVlanTagOperCfgDataDefRow.EntityId), 0x00, sizeof(gMibExtVlanTagOperCfgDataDefRow.EntityId));
    memset(&(gMibExtVlanTagOperCfgDataDefRow.AssociationType), 0x00, sizeof(gMibExtVlanTagOperCfgDataDefRow.AssociationType));
    memset(&(gMibExtVlanTagOperCfgDataDefRow.ReceivedFrameVlanTagOperTableMaxSize), 0x00, sizeof(gMibExtVlanTagOperCfgDataDefRow.ReceivedFrameVlanTagOperTableMaxSize));
    memset(&(gMibExtVlanTagOperCfgDataDefRow.InputTPID), 0x00, sizeof(gMibExtVlanTagOperCfgDataDefRow.InputTPID));
    memset(&(gMibExtVlanTagOperCfgDataDefRow.OutputTPID), 0x00, sizeof(gMibExtVlanTagOperCfgDataDefRow.OutputTPID));
    memset(&(gMibExtVlanTagOperCfgDataDefRow.DsMode), 0x00, sizeof(gMibExtVlanTagOperCfgDataDefRow.DsMode));
    strncpy(&(gMibExtVlanTagOperCfgDataDefRow.ReceivedFrameVlanTaggingOperTable), "0", sizeof(gMibExtVlanTagOperCfgDataDefRow.ReceivedFrameVlanTaggingOperTable));
    memset(&(gMibExtVlanTagOperCfgDataDefRow.AssociatedMePoint), 0x00, sizeof(gMibExtVlanTagOperCfgDataDefRow.AssociatedMePoint));
    strncpy(&(gMibExtVlanTagOperCfgDataDefRow.DscpToPbitMapping), "0", sizeof(gMibExtVlanTagOperCfgDataDefRow.DscpToPbitMapping));

	/*add for table type attribute*/
	LIST_INIT(&gMibExtVlanTagOperCfgDataDefRow.head);

    gMibExtVlanTagOperCfgDataOper.meOperDrvCfg = ExtVlanTagOperCfgDataDrvCfg;
    gMibExtVlanTagOperCfgDataOper.meOperConnCheck = ExtVlanTagOperCfgDataConnCheck;
    gMibExtVlanTagOperCfgDataOper.meOperDump = ExtVlanTagOperCfgDataDumpMib;
	gMibExtVlanTagOperCfgDataOper.meOperConnCfg = ExtVlanTagOperCfgDataConnCfg;

    MIB_Register(MIB_TABLE_EXTVLANTAGOPERCFGDATA_INDEX, &gMibExtVlanTagOperCfgDataTableInfo, &gMibExtVlanTagOperCfgDataOper);

    return GOS_OK;
}

