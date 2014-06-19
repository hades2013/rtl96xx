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
 * $Revision: 39101 $
 * $Date: 2013-05-03 04:35:27 -0500 (Fri, 03 May 2013) $
 *
 * Purpose : Definition of SVLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) OAM (802.3ah) configuration
 *
 */


#include "app_basic.h"
#include "omci_defs.h"
#include <omci_task.h>

static OMCI_TASK_INFO_T *pOperTask;


typedef struct WORK_MSG_HDR_T{
	MIB_TABLE_INDEX tableId;
	void *OldRow;
	void *NewRow;
	MIB_OPERA_TYPE operType;
} WORK_MSG_HDR_T;


UINT8 gOmciMibCfgNewBuff[MIB_TABLE_ENTRY_MAX_SIZE];
UINT8 gOmciMibCfgOldBuff[MIB_TABLE_ENTRY_MAX_SIZE];

static void* omci_GetMibCfgOldBuff(void)
{
    memset(&gOmciMibCfgOldBuff[0], 0x00, MIB_TABLE_ENTRY_MAX_SIZE);
    return &gOmciMibCfgOldBuff[0];
}

static void* omci_GetMibCfgNewBuff(void)
{
    memset(&gOmciMibCfgNewBuff[0], 0x00, MIB_TABLE_ENTRY_MAX_SIZE);
    return &gOmciMibCfgNewBuff[0];
}



static GOS_ERROR_CODE omci_meOperCfg(MIB_TABLE_INDEX tableId, void* pOldRow, void* pNewRow, MIB_OPERA_TYPE operType)
{
	GOS_ERROR_CODE ret;
	MIB_TABLE_T *pTable = mib_GetTablePtr(tableId);
	
	if(!pTable || !pTable->meOper)
		return GOS_OK;

	if(pTable->meOper->meOperConnCfg && operType !=MIB_GET)
		ret = pTable->meOper->meOperConnCfg(pOldRow,pNewRow,operType);

	if(pTable->meOper->meOperDrvCfg)
		ret = pTable->meOper->meOperDrvCfg(pOldRow,pNewRow,operType);
	
    return ret;

}


static GOS_ERROR_CODE omci_MeOperCfgTask(void)
{
	
	WORK_MSG_HDR_T*   pMsg; 
	CHAR*			  pUsrData = NULL;
	CHAR			  msgBuff[16];

	pMsg = (WORK_MSG_HDR_T*)msgBuff;

	while (1)
	{
		if (msgrcv(pOperTask->msgQId, (WORK_MSG_HDR_T*)pMsg, sizeof(WORK_MSG_HDR_T), 0, 0) == -1) 
		{
			OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"Receive MIB configure Task failed!");
			return GOS_FAIL;
		}
		omci_meOperCfg(pMsg->tableId,pMsg->OldRow,pMsg->NewRow,pMsg->operType);
	}
	return GOS_OK;
}


GOS_ERROR_CODE OMCI_MeOperCfgTaskInit(void)
{
	int taskId ;
	int i;
	taskId = OMCI_SpawnTask("MIB Configure Agent", (OMCI_TASK_ENTRY)omci_MeOperCfgTask,OMCI_TASK_PRI_MIB);

	if(taskId == OMCI_TASK_ID_INVALID)
	{
		return GOS_FAIL;
	}
	pOperTask = OMCI_GetTaskInfo(taskId);

	if(pOperTask==NULL)
	{
		return GOS_FAIL;
	}

	return GOS_OK;
}


GOS_ERROR_CODE OMCI_MeOperCfg(MIB_TABLE_INDEX tableId, void* pOldRow, void* pNewRow, MIB_OPERA_TYPE operType)
{
	GOS_ERROR_CODE	  ret;
	CHAR			  sendBuf[16];
	WORK_MSG_HDR_T*   pHdr = NULL;
	void *pNew = NULL,*pOld = NULL;
#ifdef CONFIG_NO_DELAY
	/*directly do update action*/
	if(operType == MIB_GET)
	{
		omci_meOperCfg(tableId,pOldRow,pNewRow,operType);
	}else
	{
		pNew = omci_GetMibCfgNewBuff();
		pOld = omci_GetMibCfgOldBuff();
		memset(sendBuf, 0, sizeof(sendBuf));

		pHdr = (WORK_MSG_HDR_T*)sendBuf;

		GOS_ASSERT(pHdr != NULL);

		if(pNewRow)
			memcpy(pNew,pNewRow,MIB_TABLE_ENTRY_MAX_SIZE);
		if(pOldRow)
			memcpy(pOld,pOldRow,MIB_TABLE_ENTRY_MAX_SIZE);

		pHdr->tableId = tableId;
		pHdr->OldRow = pOld;
		pHdr->NewRow = pNew;
		pHdr->operType = operType;
		
		if ((ret= msgsnd(pOperTask->msgQId, (WORK_MSG_HDR_T *)pHdr, sizeof(WORK_MSG_HDR_T), 0)) == -1)
		{
		     OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"Send MIB configure Task failed!");
		}
	}
#else
	omci_meOperCfg(tableId,pOldRow,pNewRow,operType);
#endif
	return GOS_OK;

}

GOS_ERROR_CODE OMCI_MeOperDumpMib(MIB_TABLE_INDEX tableId,void *pData)
{
    GOS_ERROR_CODE ret;

	MIB_TABLE_T *pTable = mib_GetTablePtr(tableId);

	if(pTable && pTable->meOper->meOperDump)
	{
		ret = pTable->meOper->meOperDump(pData);
	}
    return ret;
}

GOS_ERROR_CODE OMCI_MeOperConnCheck(MIB_TABLE_INDEX tableId,MIB_TREE_T *pTree,MIB_TREE_CONN_T *pConn,
PON_ME_ENTITY_ID entityId,int parm)
{
	GOS_ERROR_CODE ret;
	MIB_TABLE_T *pTable = mib_GetTablePtr(tableId);
	if(pTable && pTable->meOper->meOperConnCheck)
	{
		ret = pTable->meOper->meOperConnCheck(pTree,pConn,entityId,parm);
	}
	return ret;
}


/*
*
* CLI Handler
*
*/

static void omci_mib_show(MIB_TABLE_T*   pTable)
{
	MIB_ENTRY_T*   pEntry;
	MIB_TABLE_INDEX tableIndex = pTable->tableIndex;

	if(!pTable)
		return ;

	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"%s",MIB_GetTableName(tableIndex	));
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
	
	LIST_FOREACH(pEntry,&pTable->entryHead,entries){
		
		OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"=================================");
		OMCI_MeOperDumpMib(tableIndex,pEntry->pData);			
		OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"=================================");
	}

}

GOS_ERROR_CODE omci_tree_dump(void)
{
	MIB_AvlTreeDump();
	return GOS_OK;
}

GOS_ERROR_CODE omci_log_set(int state)
{
	if(state)
		omci_EnableLog();
	else 
		omci_DisableLog();

	return GOS_OK;
}

GOS_ERROR_CODE omci_tqTable_dump(void)
{
	omci_queueMapDump();
	return GOS_OK;

}

GOS_ERROR_CODE omci_conn_dump(void)
{
	MIB_TreeConnDump();
	return GOS_OK;
}

GOS_ERROR_CODE omci_mib_dump(int tableId,char *tableName)
{
	MIB_TABLE_T*   pTable;
	MIB_TABLE_INDEX tableIndex;

	if(tableId < 0){
		for (tableIndex = MIB_TABLE_FIRST_INDEX; tableIndex <= MIB_TABLE_LAST_INDEX; tableIndex = MIB_TABLE_NEXT_INDEX(tableIndex))
		{
				
			pTable = mib_GetTablePtr(tableIndex);
			if (!pTable)
				return GOS_ERR_PARAM;
			
			omci_mib_show(pTable);
		}
	}else
	{
		tableId = MIB_GetTableIndexByName(tableName);
		pTable = mib_GetTablePtr(tableId);
		if (!pTable)
			return GOS_ERR_PARAM;
		
		omci_mib_show(pTable);
	}

	return GOS_OK;
}
	

GOS_ERROR_CODE omci_sn_set(char *serial)
{


	MIB_TABLE_ONTG_T ontg;

	if(MIB_GetFirst(MIB_TABLE_ONTG_INDEX,&ontg,sizeof(MIB_TABLE_ONTG_T))!=GOS_OK)
	{
		return GOS_FAIL;
	}

	strncpy(ontg.SerialNum,serial,sizeof(ontg.SerialNum));
	
	if(MIB_Set(MIB_TABLE_ONTG_INDEX,&ontg,sizeof(MIB_TABLE_ONTG_T))!=GOS_OK)
	{
		return GOS_FAIL;
	}

	OMCI_wrapper_SetSerialNum(serial);
	return GOS_OK;
}


GOS_ERROR_CODE omci_sn_get(void)
{
	MIB_TABLE_ONTG_T ontg;
	char serial[16]="";

	if(MIB_GetFirst(MIB_TABLE_ONTG_INDEX,&ontg,sizeof(MIB_TABLE_ONTG_T))!=GOS_OK)
	{
		return GOS_FAIL;
	}

	printf("SerialNumber: %c%c%c%c%x%x%x%x\n",
	ontg.SerialNum[0],ontg.SerialNum[1],
	ontg.SerialNum[2],ontg.SerialNum[3],	
	ontg.SerialNum[4],ontg.SerialNum[5],
	ontg.SerialNum[6],ontg.SerialNum[7]);

	OMCI_wrapper_GetSerialNum(serial);
		
	return GOS_OK;
}

#if 0

/*Traffic related*/
static UINT8 omci_GetVlanFilterMode(UINT8 fwdOp)
{
	UINT8 mode;
	
	switch(fwdOp){
	case PON_VLANFILTER_FWDOP_A_A:
	case PON_VLANFILTER_FWDOP_FV_A:	
	case PON_VLANFILTER_FWDOP_GV_A:	
	case PON_VLANFILTER_FWDOP_FP_A:	
	case PON_VLANFILTER_FWDOP_GP_A:	
	case PON_VLANFILTER_FWDOP_FT_A:	
	case PON_VLANFILTER_FWDOP_GT_A:
	{
		
		MIB_LOG("%s: forward all",__FUNCTION__);	
		mode = OMCI_VLANFILTER_MODE_FORWARDALL;
	}	
	break;
	case PON_VLANFILTER_FWDOP_C_A:
	{		
		MIB_LOG("%s: filter > drop tag, forward untag",__FUNCTION__);
		mode = OMCI_VLANFILTER_MODE_DROPTAG_FOWARDUNTAG;
	}
	break;
	case PON_VLANFILTER_FWDOP_A_E:
	case PON_VLANFILTER_FWDOP_FV_E:	
	case PON_VLANFILTER_FWDOP_GV_E:
	case PON_VLANFILTER_FWDOP_FP_E:	
	case PON_VLANFILTER_FWDOP_GP_E:	
	case PON_VLANFILTER_FWDOP_FT_E:	
	case PON_VLANFILTER_FWDOP_GT_E:	
	{			
		MIB_LOG("%s: filter > forward tag, drop untag",__FUNCTION__);
		mode = OMCI_VLANFILTER_MODE_FORWARDTAG_DROPUNTAG;

	}
	break;
	case PON_VLANFILTER_FWDOP_HV_A:	
	case PON_VLANFILTER_FWDOP_HP_A:	
	case PON_VLANFILTER_FWDOP_HT_A:
	{
		MIB_LOG("%s: filter > forward list VID,others drop, forward untag",__FUNCTION__);
		mode = OMCI_VLANFILTER_MODE_FORWARDLIST_FORWARDUNTAG;
	}
	break;
	case PON_VLANFILTER_FWDOP_HV_E:
	case PON_VLANFILTER_FWDOP_HP_E:
	case PON_VLANFILTER_FWDOP_HT_E:
	{
		MIB_LOG("%s: filter > forward list VID,others drop, drop untag",__FUNCTION__);
		mode = OMCI_VLANFILTER_MODE_FORWARDLIST_DROPUNTAG;
	}
	break;
	default:
		mode = OMCI_VLANFILTER_MODE_NOTSUPPORT;
	break;
	}

	return mode;
}



static GOS_ERROR_CODE omci_GenUsNoVlanFilterRule(MIB_TREE_CONN_T *pConn, int index)
{
	GOS_ERROR_CODE ret;
	OMCI_TRAFF_RULE_ts *pRule;
	OMCI_VLAN_OPER_ts *pVlanRule;
	
	pRule = (OMCI_TRAFF_RULE_ts*)malloc(sizeof(OMCI_TRAFF_RULE_ts));
	pRule->dir 	= PON_GEMPORT_DIRECTION_US;
	pRule->ingress 	= pConn->pEthUni->EntityID;
	pRule->outgress = pConn->pGemPortCtp[index]->PortID;
	memset(&pRule->vlanRule,0,sizeof(OMCI_VLAN_OPER_ts));

	pVlanRule = &pRule->vlanRule;
	/*All PASS*/
	if(pConn->traffMode == OMCI_TRAF_MODE_FLOW_BASE)
	{
		pVlanRule->filterMode = VLAN_OPER_MODE_FORWARD_ALL;

	}else	
	/*1:M*/
	{
		pVlanRule->filterMode = VLAN_OPER_MODE_FORWARD_INNER_PRI;
		pVlanRule->filterCTag.pri = index;			
		pVlanRule->filterCTag.vid = 0;	
	}
	if((ret = OMCI_wrapper_ActiveBdgConn(pRule))==GOS_OK)
	{
		LIST_INSERT_HEAD(&pConn->usRuleHead[index],pRule,entries);
	}
	return ret;
}


static GOS_ERROR_CODE omci_GenUsForwardUnTagOnly(MIB_TREE_CONN_T *pConn, int index)
{
	GOS_ERROR_CODE ret;
	OMCI_TRAFF_RULE_ts *pRule;
	OMCI_VLAN_OPER_ts *pVlanRule;
	
	pRule = (OMCI_TRAFF_RULE_ts*)malloc(sizeof(OMCI_TRAFF_RULE_ts));
	pRule->dir 	= PON_GEMPORT_DIRECTION_US;
	pRule->ingress 	= pConn->pEthUni->EntityID;
	pRule->outgress = pConn->pGemPortCtp[index]->PortID;
	memset(&pRule->vlanRule,0,sizeof(OMCI_VLAN_OPER_ts));

	pVlanRule = &pRule->vlanRule;

	pVlanRule->filterMode = VLAN_OPER_MODE_FORWARD_UNTAG;

	if((ret = OMCI_wrapper_ActiveBdgConn(pRule))==GOS_OK)
	{
		LIST_INSERT_HEAD(&pConn->usRuleHead[index],pRule,entries);
	}
	return ret;
}


static GOS_ERROR_CODE omci_GenUsForwardSingleTagOnly(MIB_TREE_CONN_T *pConn, int index)
{
	GOS_ERROR_CODE ret;
	OMCI_TRAFF_RULE_ts *pRule;
	OMCI_VLAN_OPER_ts *pVlanRule;
	
	pRule = (OMCI_TRAFF_RULE_ts*)malloc(sizeof(OMCI_TRAFF_RULE_ts));
	pRule->dir 	= PON_GEMPORT_DIRECTION_US;
	pRule->ingress 	= pConn->pEthUni->EntityID;
	pRule->outgress = pConn->pGemPortCtp[index]->PortID;
	memset(&pRule->vlanRule,0,sizeof(OMCI_VLAN_OPER_ts));

	pVlanRule = &pRule->vlanRule;

	pVlanRule->filterMode = VLAN_OPER_MODE_FORWARD_SINGLETAG;

	if((ret = OMCI_wrapper_ActiveBdgConn(pRule))==GOS_OK)
	{
		LIST_INSERT_HEAD(&pConn->usRuleHead[index],pRule,entries);
	}
	return ret;
}

static GOS_ERROR_CODE omci_GenVlanFilterRule(MIB_TREE_CONN_T *pConn,int index)
{
	int i,arraySize;
	UINT16 tci;
	MIB_TABLE_VLANTAGFILTERDATA_T *pVlanFilter = pConn->pVlanTagFilter;
	GOS_ERROR_CODE ret;
	OMCI_TRAFF_RULE_ts *pRule;
	OMCI_VLAN_OPER_ts *pVlanRule;
	
	arraySize = pVlanFilter->NumOfEntries*2;
	for(i=0;i<arraySize ;i+=2)
	{
		tci = pVlanFilter->FilterTbl[i] << 8 | pVlanFilter->FilterTbl[i+1];
		pRule = (OMCI_TRAFF_RULE_ts*)malloc(sizeof(OMCI_TRAFF_RULE_ts));
		memset(pRule,0,sizeof(OMCI_TRAFF_RULE_ts));
		pRule->dir	= PON_GEMPORT_DIRECTION_US;
		pRule->ingress	= pConn->pEthUni->EntityID;
		pRule->outgress = pConn->pGemPortCtp[index]->PortID;
		pVlanRule = &pRule->vlanRule;
		pVlanRule->filterMode = VLAN_OPER_MODE_FILTER_SINGLETAG;
		pVlanRule->filterCTag.vid = tci & 0xfff;
		pVlanRule->filterCTag.pri = (tci >>13) & 0x7;

		if((ret = OMCI_wrapper_ActiveBdgConn(pRule))==GOS_OK)
		{
			LIST_INSERT_HEAD(&pConn->usRuleHead[index],pRule,entries);
		}
	}

	return GOS_OK;
}


static GOS_ERROR_CODE omci_GenUsVlanFilterOnlyRule(MIB_TREE_CONN_T *pConn,int index)
{
	GOS_ERROR_CODE ret;
	OMCI_VLANFILTER_MODE_T mode ;
	MIB_TABLE_VLANTAGFILTERDATA_T *pVlanFilter;
	pVlanFilter = pConn->pVlanTagFilter;	

	mode = omci_GetVlanFilterMode(pVlanFilter->FwdOp);

	switch(mode){
	case OMCI_VLANFILTER_MODE_FORWARDALL:
		ret = omci_GenUsNoVlanFilterRule(pConn,index);
	break;
	case OMCI_VLANFILTER_MODE_DROPTAG_FOWARDUNTAG:
		ret = omci_GenUsForwardUnTagOnly(pConn,index);
	break;
	case OMCI_VLANFILTER_MODE_FORWARDTAG_DROPUNTAG:
		ret = omci_GenUsForwardSingleTagOnly(pConn,index);
	break;
	case OMCI_VLANFILTER_MODE_FORWARDLIST_FORWARDUNTAG:
		ret = omci_GenVlanFilterRule(pConn,index);
		ret = omci_GenUsForwardUnTagOnly(pConn,index);
	break;
	case OMCI_VLANFILTER_MODE_FORWARDLIST_DROPUNTAG:
		ret = omci_GenVlanFilterRule(pConn,index);
	break;
	case OMCI_VLANFILTER_MODE_NOTSUPPORT:
	break;
	}
	
	return ret;
}


static GOS_ERROR_CODE omci_StripExtVlanFilterRule(
OMCI_VLAN_ts tVlan,
OMCI_VLAN_OPER_ts *pVlan,
OMCI_VLAN_FILTER_MODE_e *pFilterMode,
int *pTagNum)
{
	/*outer filter*/
	if(	tVlan.pri == OMCI_PRI_FILTER_IGNORE_FIELD)
	{
		*pFilterMode = VLAN_FILTER_NO_TAG;
	}else
	if(	tVlan.pri == OMCI_PRI_FILTER_DEFAULT_FIELD)
	{
		*pFilterMode = VLAN_FILTER_NO_CARE;
		pTagNum = 2;
		
	}else
	if(	tVlan.pri == OMCI_PRI_FILTER_IGNORE)
	{
		
		*pFilterMode = VLAN_FILTER_VID;
		pVlan->vid = tVlan.vid;
		pVlanRule->filterTagNum = 2;
	}else
	{
		*pFilterMode = VLAN_FILTER_TCI;
		pVlan->vid = tVlan.vid;
		pVlan->pri = tVlan.pri;
		pVlan->tpid= tVlan.tpid;		
		pVlanRule->filterTagNum = 2;
	}

	return GOS_OK;
}

static GOS_ERROR_CODE omci_GenUsExtVlanSingleRule(MIB_TREE_CONN_T *pConn, int index,extvlanTableEntry_t *pEntry)
{
	OMCI_TRAFF_RULE_ts *pRule;
	OMCI_VLAN_OPER_ts *pVlanRule;
	omci_extvlan_row_entry_t *pExtVlan = &pEntry->tableEntry;	
	OMCI_VLAN_ts *pVlan,tVlan;
	int tagNum = 0;
	
	pRule = (OMCI_TRAFF_RULE_ts*)malloc(sizeof(OMCI_TRAFF_RULE_ts));
	memset(pRule,0,sizeof(OMCI_TRAFF_RULE_ts));
	pRule->dir	= PON_GEMPORT_DIRECTION_US;
	pRule->ingress	= pConn->pEthUni->EntityID;
	pRule->outgress = pConn->pGemPortCtp[index]->PortID;
	pVlanRule = &pRule->vlanRule;
	pVlanRule->filterMode = VLAN_OPER_MODE_EXTVLAN;
	/*assign VlanRule*/
	tVlan.pri = pExtVlan->outerFilterWord.bit.filterOuterPri;
	tVlan.vid = pExtVlan->innerFilterWord.bit.filterInnerVid;
	tVlan.tpid = pExtVlan->innerFilterWord.bit.filterInnerTpId;
	omci_StripExtVlanFilterRule(tVlan,&pVlanRule->filterSTag,&pVlanRule->filterStagMode,&tagNum);
	/*outer act*/
	pVlanRule->stagAct = 
	pVlanRule->treatmentOutPri  = pExtVlan->outerTreatmentWord.bit.treatmentOuterPri;	
	pVlanRule->treatmentOutVid  = pExtVlan->outerTreatmentWord.bit.treatmentOuterVid;	
	pVlanRule->treatmentOutTPID = pExtVlan->outerTreatmentWord.bit.treatmentOuterTpId;	
	pVlanRule->treatmentOutTPID = pExtVlan->outerTreatmentWord.bit.treatmentOuterTpId;	
	/*inner act*/
	pVlanRule->treatmentInnerPri  = pExtVlan->outerFilterWord.bit.filterOuterPri;	
	pVlanRule->treatmentInnerVid  = pExtVlan->outerFilterWord.bit.filterOuterVid;		
	pVlanRule->treatmentInnerTPID = pExtVlan->outerFilterWord.bit.filterOuterTpId;
	
}


static GOS_ERROR_CODE omci_GenUsExtVlanRule(MIB_TREE_CONN_T *pConn, int index)
{
	MIB_TABLE_EXTVLANTAGOPERCFGDATA_T *pExtVlan;
	extvlanTableEntry_t *pEntry;

	pExtVlan = pConn->pExtVlanCfg;
	
	LIST_FOREACH(pEntry,&pExtVlan->head,entries){
		omci_GenUsExtVlanSingleRule(pConn,index,pEntry);
	}
	
	return GOS_OK;
}


static GOS_ERROR_CODE omci_GenUsVlanRule(MIB_TREE_CONN_T *pConn, int index)
{
	GOS_ERROR_CODE ret;

	/*1:M or All PASS*/
	if(!pConn->pExtVlanCfg && !pConn->pVlanTagFilter)
	{
		ret = omci_GenUsNoVlanFilterRule(pConn,index);
	}else
	/*1:P or 1:MP, no VLAN operation*/
	if(!pConn->pExtVlanCfg)
	{	
		ret = omci_GenUsVlanFilterOnlyRule(pConn,index);
	}else
	/*1:P or 1:MP, and VLAN operation*/
	{
		ret = omci_GenUsExtVlanRule(pConn,index);
	}

	return ret;
}




static GOS_ERROR_CODE omci_GenDsVlanRule(MIB_TREE_CONN_T *pConn,int index)
{
#if 0
	extvlanTableEntry_t *pExtVlanEntry;

	memset(pVlanOper,0,sizeof(OMCI_VLAN_OPER_ts));
	
	if(pExtVlan)
	{
		LIST_FOREACH(
	}
	

	

	if(pVlanTagFilter)
	{
		fltRule.ena = TRUE;		
		fltRule.fwdOper = pVlanFilter->FwdOp;
		for(i=0;i<pVlanFilter->NumOfEntries;i++)
		{
			fltRule.cTci[i] = pVlanFilter->FilterTbl[i*2] << 8 | pVlanFilter->FilterTbl[i*2+1] ;
		}
	}
#endif
	return GOS_OK;

}


#endif


GOS_ERROR_CODE omci_GenTraffRule(MIB_TREE_CONN_T *pConn,int index)
{
	OMCI_VLAN_OPER_ts 	*pVlanRule;
	
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: Start ...!",__FUNCTION__);
	
	GOS_ASSERT(pConn && pConn->pAniPort && pConn->pUniPort && pConn->pGemPortCtp && pConn->pEthUni);


	/*for upstream rule*/
	if(pConn->pGemPortCtp[index]->Direction != PON_GEMPORT_DIRECTION_DS)
	{
		//omci_GenUsVlanRule(pConn,index);
	}
		/*for downstream rule*/
	if(pConn->pGemPortCtp[index]->Direction != PON_GEMPORT_DIRECTION_US)
	{
		//omci_GenDsVlanRule(pConn,index);
	}
	 	
	return GOS_OK;
}


