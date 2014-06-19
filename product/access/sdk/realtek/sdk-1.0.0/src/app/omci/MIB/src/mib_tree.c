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






/*for AVL tree*/
MIB_NODE_T* MIB_AvlTreeRotateLL(MIB_NODE_T *parent) 
{ 
    MIB_NODE_T *child = parent->lChild; 
    parent->lChild = child->rChild; 
    child->rChild = parent; 
    return child; 
} 


MIB_NODE_T* MIB_AvlTreeRotateRR(MIB_NODE_T *parent) 
{ 
    MIB_NODE_T *child = parent->rChild; 
    parent->rChild= child->lChild; 
    child->lChild = parent; 
    return child; 
} 


MIB_NODE_T* MIB_AvlTreeRotateRL(MIB_NODE_T *parent) 
{ 
    MIB_NODE_T *child = parent->rChild; 
    parent->rChild= MIB_AvlTreeRotateLL(child); 
    return MIB_AvlTreeRotateRR(parent); 
} 


MIB_NODE_T* MIB_AvlTreeRotateLR(MIB_NODE_T *parent) 
{ 
    MIB_NODE_T *child = parent->lChild; 
    parent->lChild= MIB_AvlTreeRotateRR(child); 
    return MIB_AvlTreeRotateLL(parent); 
} 

int MIB_GetAvlTreeHeight(MIB_NODE_T *node) 
{ 
    int height=0; 
    if(node != NULL) 
        height = 1+avlTreeGetMax(MIB_GetAvlTreeHeight(node->lChild),MIB_GetAvlTreeHeight(node->rChild)); 
    return height; 
} 

 
int MIB_GetAvlTreeBalance(MIB_NODE_T *node) 
{ 
    if(node == NULL) return 0; 
    return MIB_GetAvlTreeHeight(node->lChild) - MIB_GetAvlTreeHeight(node->rChild); 
} 

 
MIB_NODE_T* MIB_BalanceAvlTree(MIB_NODE_T **node) 
{ 
    int height_diff= MIB_GetAvlTreeBalance(*node); 
     
    if(height_diff > 1) 
{ 
        if(MIB_GetAvlTreeBalance((*node)->lChild) > 0) 
            *node = MIB_AvlTreeRotateLL(*node); 
        else  
            *node = MIB_AvlTreeRotateLR(*node); 
    } 
    else if(height_diff < -1)
{ 
        if(MIB_GetAvlTreeBalance((*node)->rChild) < 0) 
            *node = MIB_AvlTreeRotateRR(*node); 
        else 
            *node = MIB_AvlTreeRotateRL(*node); 
    } 
    return *node; 
} 


MIB_NODE_T* MIB_AvlTreeNodeAdd(MIB_NODE_T **root,MIB_AVL_KEY_T key, MIB_TABLE_INDEX tableId,void *pData) 
{ 
	MIB_ENTRY_T 	*pEntry = NULL;
	MIB_TREE_DATA_T *treeData = NULL;
	UINT16 			id;
		
    if(*root == NULL) 
    { 
    	MIB_LOG("%s: find NULL node",__FUNCTION__);
        *root = (MIB_NODE_T*)malloc(sizeof(MIB_NODE_T)); 
        if(*root == NULL) 
        { 
            MIB_LOG("fail: memory allocation"); 
            return NULL;
        } 
		treeData = &((*root)->data);
		treeData->key = key;
		treeData->tableIndex = tableId;
		LIST_INIT(&treeData->treeNodeHead);
		/*Search Entry from MIB*/
		if((pEntry = MIB_GetTableEntry(tableId,pData))==NULL)
		{
			 MIB_LOG("fail for GetTable Entry!"); 
             return NULL;
		}
		
		MIB_GetAttrFromBuf(tableId,MIB_ATTR_FIRST_INDEX,&id,pData,sizeof(id));
		MIB_LOG("%s(): Add Table Id: %d, Entity Id 0x%2x to entry(%x)",__FUNCTION__,tableId,id,pEntry);
		LIST_INSERT_HEAD(&treeData->treeNodeHead,pEntry,treeEntry);
        (*root)->lChild = (*root)->rChild = NULL; 

    } 
    else if(key < (*root)->data.key) 
    { 
    	MIB_LOG("%s: Search left child, node key %d, insert key %d",__FUNCTION__,(*root)->data.key,key);
        (*root)->lChild = MIB_AvlTreeNodeAdd(&((*root)->lChild),key,tableId,pData); 
        (*root) = MIB_BalanceAvlTree(root);  
    } 
    else if(key > (*root)->data.key) 
    { 
    
		MIB_LOG("%s: Search right child, node key %d, insert key %d",__FUNCTION__,(*root)->data.key,key);
        (*root)->rChild = MIB_AvlTreeNodeAdd(&((*root)->rChild),key,tableId,pData); 
        (*root) = MIB_BalanceAvlTree(root); 
    } 
	else
	{		
	
		MIB_LOG("%s: key is exist, add entry to list",__FUNCTION__);
        treeData = &(*root)->data;
		LIST_FOREACH(pEntry,&treeData->treeNodeHead,treeEntry)
		{
			if(mib_CompareEntry(tableId,pData,pEntry->pData)==0)
			{
				MIB_LOG("%s: find a duplicate entity Id.",__FUNCTION__);
				return *root;
			}
		}
		/*Search Entry from MIB*/
		if((pEntry=MIB_GetTableEntry(tableId,pData))==NULL)
		{
			 MIB_LOG("fail for GetTable Entry!"); 
             return NULL;
		}
		LIST_INSERT_HEAD(&treeData->treeNodeHead,pEntry,treeEntry);
		
	}
   
    return *root; 
} 


MIB_NODE_T* MIB_AvlTreeSearch(MIB_NODE_T *node, MIB_AVL_KEY_T key) 
{ 
    if(node == NULL) return NULL; 
     
    MIB_LOG("%s(): %d->",__FUNCTION__,node->data.key); 

    if(key == node->data.key)  
        return node; 
    else if(key < node->data.key) 
        return MIB_AvlTreeSearch(node->lChild,key); 
    else  
        return MIB_AvlTreeSearch(node->rChild,key); 
} 


MIB_ENTRY_T* MIB_AvlTreeEntrySearch(MIB_NODE_T *node, MIB_AVL_KEY_T key,PON_ME_ENTITY_ID entityId) 
{ 
	MIB_ENTRY_T 	*pEntry;	
	MIB_TREE_DATA_T *pData;
	PON_ME_ENTITY_ID  id ;
	int ismatch = 0;
	
    if(node == NULL) return NULL;
	
    pData = &node->data;
	
    MIB_LOG("%s(): Current key %d, Search key %d",__FUNCTION__,pData->key,key); 

    if(key == pData->key) 
    {
		LIST_FOREACH(pEntry,&pData->treeNodeHead,treeEntry)
		{
			MIB_GetAttrFromBuf(pData->tableIndex,MIB_ATTR_FIRST_INDEX,&id,pEntry->pData,sizeof(PON_ME_ENTITY_ID));
			MIB_LOG("Entry (%x) entity id =0x%02x, Search Id 0x%02x,\n",pEntry,id,entityId);
			if(id == entityId)
			{
				ismatch = 1;
				break;
			}
		}
		if(ismatch){
			return pEntry; 
		}else
		{
			return NULL;
		}
    }
    else if(key < pData->key) 
        return MIB_AvlTreeEntrySearch(node->lChild,key,entityId); 
    else  
        return MIB_AvlTreeEntrySearch(node->rChild,key,entityId); 
} 


MIB_TREE_T* MIB_AvlTreeSearchByKey(PON_ME_ENTITY_ID entityId,MIB_AVL_KEY_T key) 
{ 
	MIB_TREE_T* pTree;
	MIB_ENTRY_T *pEntry;
	
   	if(forest.treeCount > 0)
	{
		LIST_FOREACH(pTree,&forest.treeHead,entries)
		{
			pEntry = MIB_AvlTreeEntrySearch(pTree->root,key,entityId);
			if(pEntry!=NULL){
				MIB_LOG("%s: pEntry %x\n",__FUNCTION__,pEntry);
				return pTree;
			}
		}
	}
	return NULL;
}



GOS_ERROR_CODE MIB_AvlTreeEntryDump(MIB_NODE_T *pNode)
{
	MIB_ENTRY_T *pEntry;
	PON_ME_ENTITY_ID id;

	/*preorder dump*/
	
	if(pNode->lChild!=NULL)
	{
		MIB_AvlTreeEntryDump(pNode->lChild);
	}

	
	if(pNode!=NULL){
		MIB_LOG("AVL Key:%d, TableName: %s",pNode->data.key,MIB_GetTableName(pNode->data.tableIndex));
		LIST_FOREACH(pEntry,&pNode->data.treeNodeHead,treeEntry)
		{
			MIB_GetAttrFromBuf(pNode->data.tableIndex,MIB_ATTR_FIRST_INDEX,&id,pEntry->pData,sizeof(id));
			MIB_LOG("Entity (%x) Id: 0x%02x",pEntry,id);	
		}
	}

	if(pNode->rChild!=NULL)
	{
		MIB_AvlTreeEntryDump(pNode->rChild);
	}
	return GOS_OK;
}


GOS_ERROR_CODE MIB_AvlTreeDump(void)
{
	MIB_TREE_T* pTree;
	int i=0;
	
   	if(forest.treeCount > 0)
	{
		LIST_FOREACH(pTree,&forest.treeHead,entries)
		{
			
			MIB_LOG("AVL Tree ID: %d",i);
			MIB_AvlTreeEntryDump(pTree->root);
			i++;
		}
	}

	return GOS_OK;
}



GOS_ERROR_CODE MIB_TreeConnRuleDump(OMCI_TRAFF_RULE_ts rule)
{	
	int i;
	int vlan;
	
	if(rule.ingress == 0 && rule.outgress == 0) return GOS_OK;

	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"==================================================");
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"direction: %s",rule.dir==PON_GEMPORT_DIRECTION_US ? "Upstream" : "Downstream");
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"ingress entity: %X",rule.ingress);
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"engress entity: %X",rule.outgress);

	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"==================================================");
	return GOS_OK;
}


static GOS_ERROR_CODE mib_treeConnDump(MIB_TREE_CONN_T *pConn)
{
	int i,max;

	if(!pConn)
		return GOS_FAIL;
	
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Traffic Mode: %d",pConn->traffMode);
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"ANI Ptr: 0x%02x",pConn->pAniPort);
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"UNI Ptr: 0x%02x",pConn->pUniPort);
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"ETH Ptr: 0x%02x",pConn->pEthUni);
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"802 Ptr: 0x%02x",pConn->p8021Map);
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"VlanFilter Ptr: 0x%02x",pConn->pVlanTagFilter);
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"VlanTagOper Ptr: 0x%02x",pConn->pVlanTagOpCfg);
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"McastGem Ptr: 0x%02x",pConn->pMcastGemIwTp);
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"GemIwTp Ptr: 0x%02x",pConn->pGemIwTp);
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"GemPortCtp Ptr: 0x%02x",pConn->pGemPortCtp);
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"ExtVlan Ptr: 0x%02x",pConn->pExtVlanCfg);
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Rule Mode: %s",pConn->traffMode==OMCI_TRAF_MODE_FLOW_BASE ? "FLOW BASE" : "802.1P BASE");
	//MIB_TreeConnRuleDump(pConn->usRule);
	//MIB_TreeConnRuleDump(pConn->dsRule);
   
	return GOS_OK;
}


GOS_ERROR_CODE MIB_TreeConnDump(void)
{
	MIB_TREE_CONN_T *pEntry;
	MIB_TREE_T *pTree;

	LIST_FOREACH(pTree,&forest.treeHead,entries)
	{
		
		for (pEntry = LIST_FIRST(&pTree->conns); pEntry != NULL; pEntry=LIST_NEXT(pEntry,entries) )
		{
			mib_treeConnDump(pEntry);

		}
	}

	return GOS_OK;
}



GOS_ERROR_CODE MIB_TreeConnDefault(MIB_TREE_CONN_T *pConn)
{
	int i;
	memset(pConn,0,sizeof(MIB_TREE_CONN_T));
	pConn->pAniPort 	= NULL;
	pConn->pUniPort 	= NULL;
	pConn->pEthUni 		= NULL;	
	pConn->p8021Map 	= NULL;	
	pConn->pVlanTagFilter 	= NULL;
	pConn->pVlanTagOpCfg 	= NULL;
	pConn->pMcastGemIwTp 	= NULL;
	pConn->pExtVlanCfg 	= NULL;
	pConn->state = OMCI_CONN_STATE_COMMON;

	for(i=0;i<8;i++)
	{
		pConn->pGemIwTp[i] = NULL;
		pConn->pGemPortCtp[i] = NULL;
		LIST_INIT(&pConn->usRuleHead[i]);
		LIST_INIT(&pConn->dsRuleHead[i]);
	}

	return GOS_OK;
}



static GOS_ERROR_CODE mib_treeConnSet(MIB_TREE_CONN_T *pConn)
{
	GOS_ERROR_CODE ret;
	OMCI_TRAFF_RULE_ts *pRule;
	int i,max;

	if(!pConn)
		return GOS_FAIL;

	if(pConn->traffMode == OMCI_TRAF_MODE_FLOW_BASE)
	{
		max = 1;
	}else
	{
		max = 8;
	}
		
	for(i=0;i<max;i++)
	{
		ret = omci_GenTraffRule(pConn,i);
	
		if(ret!=GOS_OK)
		{	
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Fail: GenTraffRule, ret=%d",ret);
			return GOS_FAIL;
		}
		
	}


	return GOS_OK;
}



static GOS_ERROR_CODE mib_treeConnDel(MIB_TREE_CONN_T *pConn)
{
	GOS_ERROR_CODE ret;		
	int i,max;
#if 0
	ret = OMCI_wrapper_DeactiveBdgConn(pConn->usRule);
	ret = OMCI_wrapper_DeactiveBdgConn(pConn->dsRule);		
#endif
	return ret;
}


static GOS_ERROR_CODE mib_TreeConnSet(MIB_TREE_T *pTree)
{
	MIB_TREE_CONN_T *pEntry,*pTmpEntry;

	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: Start ...",__FUNCTION__);
	for (pEntry = LIST_FIRST(&pTree->conns); pEntry != NULL; )
	{
		
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: state %d",__FUNCTION__,pEntry->state);
		switch(pEntry->state)
		{
		case OMCI_CONN_STATE_NEW:				
			mib_treeConnSet(pEntry);			
			pEntry = LIST_NEXT(pEntry,entries);
		break;
		case OMCI_CONN_STATE_COMMON:			
			pEntry = LIST_NEXT(pEntry,entries);
		break;
		case OMCI_CONN_STATE_DEL:
			mib_treeConnDel(pEntry);
			pTmpEntry = pEntry;
			pEntry = LIST_NEXT(pEntry,entries);
			LIST_REMOVE(pTmpEntry,entries);
		break;
		case OMCI_CONN_STATE_UPDATE:
			mib_treeConnDel(pEntry);
			mib_treeConnSet(pEntry);			
			pEntry = LIST_NEXT(pEntry,entries);
		break;
		default:			
			pEntry = LIST_NEXT(pEntry,entries);
		break;
		}

	}

	return GOS_OK;
}


MIB_TREE_T* MIB_AvlTreeCreate(MIB_TABLE_MACBRISERVPROF_T *pBridge)
{
	MIB_TREE_T* pTree;
	
	/*check is tree exist or not*/
	pTree = MIB_AvlTreeSearchByKey(pBridge->EntityID,AVL_KEY_MACBRISERVPROF);

	if(pTree!=NULL)
	{
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: tree is exist!",__FUNCTION__);
		return pTree;
	}
	
	/*tree not exist, create a new one*/
	forest.treeCount ++;
	/*create a new one avl tree*/
	pTree = (MIB_TREE_T*)malloc(sizeof(MIB_TREE_T));
	pTree->root = NULL;
	LIST_INIT(&pTree->conns);
	
	/*insert a new data to tree, return new node*/
	if(MIB_AvlTreeNodeAdd(&pTree->root,AVL_KEY_MACBRISERVPROF,MIB_TABLE_MACBRISERVPROF_INDEX,pBridge)==NULL)
	{
		return NULL;
	}	
	LIST_INSERT_HEAD(&forest.treeHead,pTree,entries);
	
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: Add new Tree and root node, [Tree] %x, [Root] %x\n",__FUNCTION__,pTree,pTree->root);
	return pTree;
}



GOS_ERROR_CODE MIB_AvlTreeNodeRemove(MIB_NODE_T *pNode)
{
	MIB_ENTRY_T *pEntry;

	if(!pNode)
	{
		return GOS_OK;
	}
	
	MIB_AvlTreeNodeRemove(pNode->lChild);
	MIB_AvlTreeNodeRemove(pNode->rChild);

	for(pEntry = LIST_FIRST(&pNode->data.treeNodeHead); pEntry!=NULL ; pEntry = LIST_NEXT(pEntry,treeEntry))
	{
		LIST_REMOVE(pEntry,treeEntry);
	}
	return GOS_OK;
}


GOS_ERROR_CODE MIB_AvlTreeRemove(MIB_TREE_T *pTree)
{
	MIB_TREE_CONN_T *pConn;
	
	/*remove frome forest*/
	LIST_REMOVE(pTree,entries);
	forest.treeCount --;

	/*remove all tree connection*/
	for(pConn = LIST_FIRST(&pTree->conns); pConn!=NULL; pConn = LIST_NEXT(pConn,entries))
	{
		mib_treeConnDel(pConn);
		LIST_REMOVE(pConn,entries);
	}
	/*remove all node*/
	MIB_AvlTreeNodeRemove(pTree->root);
	
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: remove Tree and nodes",__FUNCTION__);
	return GOS_OK;
}


GOS_ERROR_CODE MIB_AvlTreeRemoveAll(void)
{
	MIB_TREE_T *pTree;

	for(pTree = LIST_FIRST(&forest.treeHead); pTree!=NULL; pTree = LIST_NEXT(pTree,entries))
	{
		MIB_AvlTreeRemove(pTree);
	}
	LIST_INIT(&forest.treeHead);
	forest.treeCount = 0;
	return GOS_OK;
}


static GOS_ERROR_CODE MIB_TreeRootConnUpdate(MIB_TREE_T *pTree,MIB_TREE_CONN_T *pConn)
{
	BOOL isExist = FALSE;	
	MIB_TREE_CONN_T *pConnEntry;
	int i;
	
	/*insert connection to list, check exist first*/
	LIST_FOREACH(pConnEntry,&pTree->conns,entries)
	{
		if(pConnEntry->pAniPort == pConn->pAniPort && pConnEntry->pUniPort == pConn->pUniPort)
		{
			isExist = TRUE;
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: is connection exist, update it only",__FUNCTION__);
			break;
		}
	}
	/*if exist, update it, else create a new one*/
	if(isExist)
	{
		pConnEntry->state = OMCI_CONN_STATE_UPDATE;		
		pConnEntry->pEthUni = pConn->pEthUni;
		for(i=0;i<8;i++)
		{
			pConnEntry->pGemPortCtp[i] = pConn->pGemPortCtp[i];			
			pConnEntry->pGemIwTp[i] = pConn->pGemIwTp[i];
		}
		pConnEntry->pMcastGemIwTp = pConn->pMcastGemIwTp;
		pConnEntry->pVlanTagFilter = pConn->pVlanTagFilter;
		pConnEntry->pVlanTagOpCfg = pConn->pVlanTagOpCfg;
		pConnEntry->pExtVlanCfg = pConn->pExtVlanCfg;
		pConnEntry->traffMode = pConnEntry->traffMode;
	}else
	{
		pConnEntry = (MIB_TREE_CONN_T*)malloc(sizeof(MIB_TREE_CONN_T));
		memset(pConnEntry,0,sizeof(MIB_TREE_CONN_T));
		memcpy(pConnEntry,pConn,sizeof(MIB_TREE_CONN_T));		
		pConnEntry->state = OMCI_CONN_STATE_NEW;			
		LIST_INSERT_HEAD(&pTree->conns,pConnEntry,entries);
	}
	
	return GOS_OK;	
}


GOS_ERROR_CODE MIB_TreeRootConnCheck(MIB_TREE_T *pTree)
{
	int i;
	MIB_NODE_T  *pUniNode,*pAniNode;
	MIB_ENTRY_T *pUniEntry,*pAniEntry;
	MIB_TREE_DATA_T *pUniData,*pAniData;
	int aniOk = GOS_FAIL, uniOk = GOS_FAIL;
	MIB_TABLE_MACBRIPORTCFGDATA_T *pUniPort,*pAniPort;
	MIB_AVL_KEY_T key;
	MIB_TREE_CONN_T conn;

	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Start %s...",__FUNCTION__);

	/*get uni side MacBridgePort*/
	pUniNode = MIB_AvlTreeSearch(pTree->root,AVL_KEY_MACBRIPORT_UNI);

	if(!pUniNode)
	{ 	
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: can't find MacBridgePortData UNI on side",__FUNCTION__);
		return GOS_FAIL;
	}

	pUniData = &pUniNode->data;

	/*get ani side MacBridgePort*/
	pAniNode = MIB_AvlTreeSearch(pTree->root,AVL_KEY_MACBRIPORT_ANI);

	if(!pAniNode)
	{			
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: can't find MacBridgePortData on ANI side",__FUNCTION__);
		return GOS_FAIL;
	}
	pAniData = &pAniNode->data;

	/*search each connections included uni+ani*/
	LIST_FOREACH(pUniEntry,&pUniData->treeNodeHead,entries){
			
		
		MIB_TreeConnDefault(&conn);

		pUniPort = (MIB_TABLE_MACBRIPORTCFGDATA_T*) pUniEntry->pData;
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"UNI Entity: 0x%02x",pUniPort->EntityID); 
		key = AVL_KEY_MACBRIPORT_UNI;
		if(OMCI_MeOperConnCheck(MIB_TABLE_MACBRIPORTCFGDATA_INDEX,pTree,conn,pUniPort->EntityID,key)!=GOS_OK)
		{
			continue;
		}
		uniOk = GOS_OK;
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: connect to 0x%04x,UNI OK",__FUNCTION__,pUniPort->EntityID);
		
		LIST_FOREACH(pAniEntry,&pAniData->treeNodeHead,entries){
			
			pAniPort = (MIB_TABLE_MACBRIPORTCFGDATA_T*) pAniEntry->pData;
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"ANI Entity: 0x%02x",pAniPort->EntityID); 
			key = AVL_KEY_MACBRIPORT_ANI;
			if(OMCI_MeOperConnCheck(MIB_TABLE_MACBRIPORTCFGDATA_INDEX,pTree,conn,pAniPort->EntityID,key)!=GOS_OK){
				continue;
			}
			
			aniOk = GOS_OK;
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: connect to 0x%04x,ANI OK",__FUNCTION__,pAniPort->EntityID);
			
			/*update connections*/
			MIB_TreeRootConnUpdate(pTree,&conn);
		}

	}
	
	if(uniOk!=GOS_OK || aniOk!=GOS_OK ){
		
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: UNI & ANI check faile!",__FUNCTION__);
		return GOS_FAIL;
	}

	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: check connection ok!",__FUNCTION__);
	
	return GOS_OK;
	
}


GOS_ERROR_CODE MIB_TreeConnUpdate(MIB_TREE_T *pTree)
{
	BOOL isNewCmplt = TRUE;

	if(pTree!=NULL){
		/*check connection*/
		if (MIB_TreeRootConnCheck(pTree) == GOS_FAIL)
		{
		    isNewCmplt = FALSE;
		}

		/*start to handler new traffic*/
		OMCI_LOG(OMCI_LOG_LEVEL_LOW,"New Connection is %s", isNewCmplt == TRUE ? "complete" : "not complete");

		/*start to handle connections*/
		if (isNewCmplt == TRUE)
		{
			mib_TreeConnSet(pTree);
		}
	}

	return GOS_OK;
}


