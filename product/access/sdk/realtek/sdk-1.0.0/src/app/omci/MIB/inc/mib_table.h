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


#ifndef __MIB_TABLE_H__
#define __MIB_TABLE_H__


#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include "mib_tree.h"

/*mib_table top head*/
LIST_HEAD(listhead,mib_table_s) head;


#ifdef __cplusplus
extern "C" {
#endif


/*mib attribute information structure*/
typedef struct {
    const CHAR* Name;
    const CHAR* Desc;
    MIB_ATTR_TYPE DataType;
    UINT32 Len;
    BOOL IsIndex;
    MIB_ATTR_USR_ACC_TYPE UsrAcc;
    BOOL MibSave;
    MIB_ATTR_OUT_STYLE OutStyle;
    UINT32 OltAcc;
    BOOL AvcFlag;
    UINT32 OptionType;
} MIB_ATTR_INFO_T;


/*mib table information structure*/
typedef struct {
    const CHAR* Name;
    const CHAR* Desc;
    UINT32 MaxEntry;
    UINT32 ClassId;
    UINT32 InitType;
    UINT32 StdType;
    UINT32 ActionType;
	UINT32 attrNum;
	UINT32 entrySize;	
	void*  pDefaultRow;
    MIB_ATTR_INFO_T* pAttributes;
} MIB_TABLE_INFO_T;


/*mib table entry structure for save low data and linking to tree and table*/
typedef struct mib_entry_s{
	void *pData;
	LIST_ENTRY(mib_entry_s) entries;
	LIST_ENTRY(mib_entry_s) treeEntry;
}MIB_ENTRY_T;

typedef struct mib_unlink_s{
	void *pNewData;
	void *pOldData;
	MIB_OPERA_TYPE operType;
	MIB_TABLE_INDEX tableIndex;
	LIST_ENTRY(mib_unlink_s) entries;
}MIB_UNLINK_T;


/*mib table callback function prototype*/
typedef GOS_ERROR_CODE (*MIB_CONN_CHECK_FUNC)(MIB_TREE_T *pTree,MIB_TREE_CONN_T *pConn,PON_ME_ENTITY_ID entity,int parm);
typedef GOS_ERROR_CODE (*MIB_DUMP_FUNC)(void *pData);

typedef struct mib_table_oper{	
	MIB_CFG_FUNC	  	meOperDrvCfg;
	MIB_CFG_FUNC		meOperConnCfg;
	MIB_CONN_CHECK_FUNC meOperConnCheck;
	MIB_DUMP_FUNC		meOperDump;
}MIB_TABLE_OPER_T;

/*mib table structure*/
typedef struct mib_table_s {	
	
	MIB_TABLE_INDEX	  tableIndex;
    MIB_TABLE_INFO_T* pTableInfo;
    MIB_CALLBACK_FUNC preCheckCB[MIB_TABLE_CB_MAX_NUM];
    MIB_CALLBACK_FUNC operationCB[MIB_TABLE_CB_MAX_NUM];
    UINT32            curEntryCount;
    UINT32            snapshotEntryCount;
	MIB_TABLE_OPER_T  *meOper;
    void*             pSnapshotData;	
	LIST_ENTRY(mib_table_s) entries;
    LIST_HEAD(entryhead,mib_entry_s) entryHead;
} MIB_TABLE_T;


const CHAR* MIB_GetTableName(MIB_TABLE_INDEX tableIndex);
const CHAR* MIB_GetTableDesc(MIB_TABLE_INDEX tableIndex);
UINT32 MIB_GetTableMaxEntry(MIB_TABLE_INDEX tableIndex);
UINT32 MIB_GetTableClassId(MIB_TABLE_INDEX tableIndex);
UINT32 MIB_GetTableInitType(MIB_TABLE_INDEX tableIndex);
UINT32 MIB_GetTableStdType(MIB_TABLE_INDEX tableIndex);
UINT32 MIB_GetTableActionType(MIB_TABLE_INDEX tableIndex);
const CHAR* MIB_GetAttrName(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);
const CHAR* MIB_GetAttrDesc(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);
MIB_ATTR_TYPE MIB_GetAttrDataType(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);
UINT32 MIB_GetAttrLen(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);
BOOL MIB_GetAttrIsIndex(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);
MIB_ATTR_USR_ACC_TYPE MIB_GetAttrUsrAcc(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);
BOOL MIB_GetAttrMibSave(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);
MIB_ATTR_OUT_STYLE MIB_GetAttrOutStyle(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);
UINT32 MIB_GetAttrOltAcc(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);
BOOL MIB_GetAttrAvcFlag(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);
UINT32 MIB_GetAttrOptionType(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);
MIB_ENTRY_T* MIB_GetTableEntry(MIB_TABLE_INDEX tableIndex, void* pObjectRow);



/*MIB Tree related export API*/
MIB_NODE_T* 	MIB_BalanceAvlTree(MIB_NODE_T **node) ;
MIB_NODE_T* 	MIB_AvlTreeNodeAdd(MIB_NODE_T **root,MIB_AVL_KEY_T key, MIB_TABLE_INDEX tableId,void *pData) ;
MIB_ENTRY_T*	MIB_AvlTreeEntrySearch(MIB_NODE_T *node, MIB_AVL_KEY_T key,PON_ME_ENTITY_ID entityId) ;
MIB_NODE_T* 	MIB_AvlTreeSearch(MIB_NODE_T *node, MIB_AVL_KEY_T key) ;
MIB_TREE_T* 	MIB_AvlTreeSearchByKey(PON_ME_ENTITY_ID entityId,MIB_AVL_KEY_T key) ;
GOS_ERROR_CODE 	MIB_AvlTreeEntryDump(MIB_NODE_T *pNode);
GOS_ERROR_CODE 	MIB_AvlTreeDump(void);
MIB_TREE_T* 	MIB_AvlTreeCreate(MIB_TABLE_MACBRISERVPROF_T *pBridge);
GOS_ERROR_CODE  MIB_AvlTreeNodeRemove(MIB_NODE_T *pNode);
GOS_ERROR_CODE 	MIB_AvlTreeRemove(MIB_TREE_T *pTree);



GOS_ERROR_CODE omci_GenTraffRule(MIB_TREE_CONN_T *pConn,int index);
GOS_ERROR_CODE OMCI_MeConnCheck(MIB_TABLE_INDEX tableId,MIB_TREE_T *pTree,MIB_TREE_CONN_T *pConn,PON_ME_ENTITY_ID entityId, void *opt);
GOS_ERROR_CODE MIB_TreeConnUpdate(MIB_TREE_T *pTree);



/* Table definitions */

enum {
	MIB_TABLE_ONTG_INDEX=1,
	MIB_TABLE_ONT2G_INDEX=2,
	MIB_TABLE_ONTDATA_INDEX,
	MIB_TABLE_SWIMAGE_INDEX,
	MIB_TABLE_CARDHOLDER_INDEX,
	MIB_TABLE_CIRCUITPACK_INDEX,
	MIB_TABLE_ANIG_INDEX,
	MIB_TABLE_TCONT_INDEX,
	MIB_TABLE_SCHEDULER_INDEX,
	MIB_TABLE_ETHUNI_INDEX,
	MIB_TABLE_PRIQ_INDEX,
	MIB_TABLE_GEMPORTCTP_INDEX,
	MIB_TABLE_GEMIWTP_INDEX,
	MIB_TABLE_MULTIGEMIWTP_INDEX,
	MIB_TABLE_MACBRISERVPROF_INDEX,
	MIB_TABLE_MACBRIPORTCFGDATA_INDEX,
	MIB_TABLE_VLANTAGOPCFGDATA_INDEX,
	MIB_TABLE_VLANTAGFILTERDATA_INDEX,
	MIB_TABLE_MAP8021PSERVPROF_INDEX,
	MIB_TABLE_GEMTRAFFICDESCRIPTOR_INDEX,
	MIB_TABLE_ETHPMHISTORYDATA_INDEX,
	MIB_TABLE_ETHPMHISTORYDATA2_INDEX,
	MIB_TABLE_ETHPMHISTORYDATA3_INDEX,
	MIB_TABLE_EXTVLANTAGOPERCFGDATA_INDEX,
	MIB_TABLE_GALETHPROF_INDEX,
	MIB_TABLE_MACBRIDGEPORTPMMONITORHISTORYDATA_INDEX,
	MIB_TABLE_THRESHOLDDATA1_INDEX,
	MIB_TABLE_THRESHOLDDATA2_INDEX,
	MIB_TABLE_OLTG_INDEX,
	MIB_TABLE_MCASTOPERPROF_INDEX,
	MIB_TABLE_MCASTSUBMONITOR_INDEX,
	MIB_TABLE_MCASTSUBCONFINFO_INDEX,
	MIB_TABLE_VEIP_INDEX,
	MIB_TABLE_LOIDAUTH_INDEX,
	MIB_TABLE_LAST_FAKE_INDEX,
	MIB_TABLE_LAST_INDEX = MIB_TABLE_LAST_FAKE_INDEX-1
};


#define MIB_TABLE_TOTAL_NUMBER     (MIB_TABLE_LAST_INDEX)
#define MIB_TABLE_INDEX_VALID(idx) ((idx >= MIB_TABLE_FIRST_INDEX) && (idx <= MIB_TABLE_LAST_INDEX))


#ifdef __cplusplus
}
#endif


#endif //__MIB_TABLE_H__

