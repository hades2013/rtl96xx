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



/* Get larger attribute state machine */
#define OMCI_MULGET_STATE_IDLE (0)
#define OMCI_MULGET_STATE_RUN  (1)
#define OMCI_MULGET_STATE_NUM  (2)

#define OMCI_MULGET_EVT_START  (0)
#define OMCI_MULGET_EVT_GET    (1)
#define OMCI_MULGET_EVT_NUM    (2)

/* Upload Mib state machine */

#define OMCI_MIB_UPLOAD_STATE_IDLE (0)
#define OMCI_MIB_UPLOAD_STATE_RUN  (1)
#define OMCI_MIB_UPLOAD_STATE_NUM  (2)

#define OMCI_MIB_UPLOAD_EVT_START  (0)
#define OMCI_MIB_UPLOAD_EVT_GET    (1)
#define OMCI_MIB_UPLOAD_EVT_NUM    (2)



/* omci msg to mib format buffer */
UINT8 gOmciMibRowBuff[MIB_TABLE_ENTRY_MAX_SIZE];


#define OMCI_MAX_FSM_ID (3)

typedef UINT32 omci_fsm_id;
typedef UINT32 (*OMCI_FSMEVTHDL)(void* pEvtArg);

typedef struct 
{
    UINT32           curState;
    UINT32           maxState;
    UINT32           maxEvent;
    OMCI_FSMEVTHDL*  pHandlers;
    BOOL             valid;
} omci_fsm_info_ts;

omci_fsm_info_ts  gOmciFsmInfo[OMCI_MAX_FSM_ID];


typedef struct 
{
    MIB_ATTR_INDEX   attrIndex;
    UINT16           maxSeqNum;
    UINT8            attrValue[MIB_TABLE_ATTR_MAX_SIZE];
    UINT32           attrSize;
    UINT32           doneSeqNum;
}omci_mulget_attr_ts;

typedef struct 
{
    PON_ME_CLASS_ID     classID;
    PON_ME_ENTITY_ID    entityID;
    omci_mulget_attr_ts attribute[MIB_TABLE_ATTR_MAX_NUM];
} omci_mulget_info_ts;

typedef struct
{
    UINT32 curChannelPri;
    UINT16 doneSeqNum;
    UINT16 maxSeqNum;
} omci_mib_upload_info_ts;


/* multi get data */
omci_mulget_info_ts gOmciMulGetData[PON_OMCI_MSG_PRI_NUM];
omci_fsm_id         gOmciMulGetFsm[PON_OMCI_MSG_PRI_NUM];

/* mib upload data*/
omci_mib_upload_info_ts gOmciMibUploadData;
omci_fsm_id             gOmciMibUploadFsm;



GOS_ERROR_CODE omci_InitFsm(void);
omci_fsm_id omci_FsmCreate(UINT32 defaultState, UINT32 maxState, UINT32 maxEvent);
GOS_ERROR_CODE omci_AddFsmEventHandler(omci_fsm_id fsmId, UINT32 state, UINT32 event, OMCI_FSMEVTHDL handler);
GOS_ERROR_CODE omci_FsmRun(omci_fsm_id fsmId, UINT32 event, void* pEvtArg);
void* omci_GetMibRowBuff(void);
PON_OMCI_ATTRS_SET omci_GetOltAccAttrSet(MIB_TABLE_INDEX tableIndex, PON_ME_OLT_ACC_TYPE accType);
GOS_ERROR_CODE omci_OmciDataToMibRow(MIB_TABLE_INDEX tableIndex, UINT8* pOmciData, void* pMibRow, PON_OMCI_ATTRS_SET omciAttrSet);
GOS_ERROR_CODE omci_AttrsOmciSetToMibSet(PON_OMCI_ATTRS_SET* pOmciAttrSet, MIB_ATTRS_SET* pMibAttrSet);
GOS_ERROR_CODE omci_SyncMibData(void);
GOS_ERROR_CODE omci_CreateRelatedMe(MIB_TABLE_INDEX tableIndex, void* pMibRow);
GOS_ERROR_CODE omci_SingleSet(PON_OMCI_MSG_H_T* pNorOmciMsg);
PON_OMCI_ATTRS_SET omci_GetOptionAttrSet(MIB_TABLE_INDEX tableIndex, PON_ME_ATTR_OPT_TYPE optType);
GOS_ERROR_CODE omci_SingleGet(PON_OMCI_MSG_H_T* pNorOmciMsg);
MIB_ATTRS_SET omci_MibRowToOmciData(MIB_TABLE_INDEX tableIndex, void* pMibRow, UINT8* pOmciData, MIB_ATTRS_SET* pMibAttrSet);
GOS_ERROR_CODE omci_MultiGet(PON_OMCI_MSG_H_T* pNorOmciMsg);
UINT32 omci_MulGetStartOnIdle(PON_OMCI_MSG_H_T* pNorOmciMsg);
UINT32 omci_GetTableAttrLen(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex);
UINT32 omci_MulGetGetOnIdle(PON_OMCI_MSG_H_T* pNorOmciMsg);
UINT32 omci_MulGetStartOnRun(PON_OMCI_MSG_H_T* pNorOmciMsg);
UINT32 omci_MulGetGetOnRun(PON_OMCI_MSG_H_T* pNorOmciMsg);
GOS_ERROR_CODE omci_ReportAvcs(void);
UINT32 omci_MibUploadStartOnIdle(PON_OMCI_MSG_H_T* pNorOmciMsg);
UINT16 omci_GetMibUploadMaxSeqNum(void);
UINT16 omci_GetSeqNumPerRow(MIB_TABLE_INDEX tableIndex);
MIB_ATTRS_SET omci_GetMibUploadData(UINT16 seqNum, CHAR* pDataBuf, UINT32 size, MIB_TABLE_INDEX* pTableIndex, PON_ME_ENTITY_ID* pEntityId);
UINT32 omci_MibUploadGetOnIdle(PON_OMCI_MSG_H_T* pNorOmciMsg);
UINT32 omci_MibUploadStartOnRun(PON_OMCI_MSG_H_T* pNorOmciMsg);
MIB_ATTR_INDEX omci_GetFirstAttrIndex(PON_OMCI_ATTRS_SET attrsSet);
UINT32 omci_MibUploadGetOnRun(PON_OMCI_MSG_H_T* pNorOmciMsg);



GOS_ERROR_CODE OMCI_InitStateMachines(void)
{
    omci_InitFsm();

    /* Multi Get state machine */
    memset(gOmciMulGetData, 0x00, PON_OMCI_MSG_PRI_NUM * sizeof(omci_mulget_info_ts));

    gOmciMulGetFsm[PON_OMCI_MSG_PRI_LOW] = omci_FsmCreate(OMCI_MULGET_STATE_IDLE, OMCI_MULGET_STATE_NUM, OMCI_MULGET_EVT_NUM);
    omci_AddFsmEventHandler(gOmciMulGetFsm[PON_OMCI_MSG_PRI_LOW], OMCI_MULGET_STATE_IDLE, \
                        OMCI_MULGET_EVT_START,  (OMCI_FSMEVTHDL)omci_MulGetStartOnIdle);
    omci_AddFsmEventHandler(gOmciMulGetFsm[PON_OMCI_MSG_PRI_LOW], OMCI_MULGET_STATE_IDLE, \
                        OMCI_MULGET_EVT_GET,    (OMCI_FSMEVTHDL)omci_MulGetGetOnIdle);
    omci_AddFsmEventHandler(gOmciMulGetFsm[PON_OMCI_MSG_PRI_LOW], OMCI_MULGET_STATE_RUN,  \
                        OMCI_MULGET_EVT_START,  (OMCI_FSMEVTHDL)omci_MulGetStartOnRun);
    omci_AddFsmEventHandler(gOmciMulGetFsm[PON_OMCI_MSG_PRI_LOW], OMCI_MULGET_STATE_RUN,  \
                        OMCI_MULGET_EVT_GET,    (OMCI_FSMEVTHDL)omci_MulGetGetOnRun);

    gOmciMulGetFsm[PON_OMCI_MSG_PRI_HIGH] = omci_FsmCreate(OMCI_MULGET_STATE_IDLE, OMCI_MULGET_STATE_NUM, OMCI_MULGET_EVT_NUM);
    omci_AddFsmEventHandler(gOmciMulGetFsm[PON_OMCI_MSG_PRI_HIGH], OMCI_MULGET_STATE_IDLE, \
                        OMCI_MULGET_EVT_START,  (OMCI_FSMEVTHDL)omci_MulGetStartOnIdle);
    omci_AddFsmEventHandler(gOmciMulGetFsm[PON_OMCI_MSG_PRI_HIGH], OMCI_MULGET_STATE_IDLE, \
                        OMCI_MULGET_EVT_GET,    (OMCI_FSMEVTHDL)omci_MulGetGetOnIdle);
    omci_AddFsmEventHandler(gOmciMulGetFsm[PON_OMCI_MSG_PRI_HIGH], OMCI_MULGET_STATE_RUN,  \
                        OMCI_MULGET_EVT_START,  (OMCI_FSMEVTHDL)omci_MulGetStartOnRun);
    omci_AddFsmEventHandler(gOmciMulGetFsm[PON_OMCI_MSG_PRI_HIGH], OMCI_MULGET_STATE_RUN,  \
                        OMCI_MULGET_EVT_GET,    (OMCI_FSMEVTHDL)omci_MulGetGetOnRun);

    /* Mib Upload state machine */
    memset(&gOmciMibUploadData, 0x00, sizeof(omci_mib_upload_info_ts));
    
    gOmciMibUploadFsm = omci_FsmCreate(OMCI_MIB_UPLOAD_STATE_IDLE, OMCI_MIB_UPLOAD_STATE_NUM, OMCI_MIB_UPLOAD_EVT_NUM);
    omci_AddFsmEventHandler(gOmciMibUploadFsm, OMCI_MIB_UPLOAD_STATE_IDLE, \
                        OMCI_MIB_UPLOAD_EVT_START,  (OMCI_FSMEVTHDL)omci_MibUploadStartOnIdle);
    omci_AddFsmEventHandler(gOmciMibUploadFsm, OMCI_MIB_UPLOAD_STATE_IDLE, \
                        OMCI_MIB_UPLOAD_EVT_GET,    (OMCI_FSMEVTHDL)omci_MibUploadGetOnIdle);
    omci_AddFsmEventHandler(gOmciMibUploadFsm, OMCI_MIB_UPLOAD_STATE_RUN,  \
                        OMCI_MIB_UPLOAD_EVT_START,  (OMCI_FSMEVTHDL)omci_MibUploadStartOnRun);
    omci_AddFsmEventHandler(gOmciMibUploadFsm, OMCI_MIB_UPLOAD_STATE_RUN,  \
                        OMCI_MIB_UPLOAD_EVT_GET,    (OMCI_FSMEVTHDL)omci_MibUploadGetOnRun);

    return GOS_OK;
}


GOS_ERROR_CODE omci_InitFsm(void)
{
    static BOOL inited = FALSE;
    UINT32 i;

    if (inited)
    {
        return GOS_OK;
    }

    memset(gOmciFsmInfo, 0x00, OMCI_MAX_FSM_ID * sizeof(omci_fsm_info_ts));

    for (i = 0; i < OMCI_MAX_FSM_ID; i++)
    {
        gOmciFsmInfo[i].valid = FALSE;
    }

    inited = TRUE;
    
    return GOS_OK;
}


omci_fsm_id omci_FsmCreate(UINT32 defaultState, UINT32 maxState, UINT32 maxEvent)
{
    omci_fsm_info_ts fsmInfo;
    UINT32     fsmId;

    memset(&fsmInfo, 0x00, sizeof(omci_fsm_info_ts));
    fsmInfo.curState = defaultState;
    fsmInfo.maxEvent = maxEvent;
    fsmInfo.maxState = maxState;
    fsmInfo.pHandlers = (OMCI_FSMEVTHDL*)malloc(sizeof(OMCI_FSMEVTHDL) * maxState * maxEvent);
    GOS_ASSERT(fsmInfo.pHandlers != NULL);
    memset(fsmInfo.pHandlers, 0x00, sizeof(OMCI_FSMEVTHDL));

    for (fsmId = 0; fsmId < OMCI_MAX_FSM_ID; fsmId++)
    {
        if (!gOmciFsmInfo[fsmId].valid)
        {
            break;
        }
    }
    
    if (fsmId < OMCI_MAX_FSM_ID)
    {
        memcpy(&(gOmciFsmInfo[fsmId]), &fsmInfo, sizeof(omci_fsm_info_ts));
        gOmciFsmInfo[fsmId].valid = TRUE;
    }

    return fsmId;
}

GOS_ERROR_CODE omci_AddFsmEventHandler(omci_fsm_id fsmId, UINT32 state, UINT32 event, OMCI_FSMEVTHDL handler)
{
    omci_fsm_info_ts* pFsmInfo;


    GOS_ASSERT(fsmId < OMCI_MAX_FSM_ID);
    
    if (gOmciFsmInfo[fsmId].valid == TRUE)
    {
        pFsmInfo = &(gOmciFsmInfo[fsmId]);
        
        GOS_ASSERT(state < pFsmInfo->maxState);
        GOS_ASSERT(event < pFsmInfo->maxEvent);
        GOS_ASSERT(pFsmInfo->pHandlers);
        
        *(pFsmInfo->pHandlers + state * pFsmInfo->maxEvent + event) = handler;
        
        return GOS_OK;
    }

    return GOS_FAIL;
}

GOS_ERROR_CODE omci_FsmRun(omci_fsm_id fsmId, UINT32 event, void* pEvtArg)
{
    omci_fsm_info_ts*    pFsmInfo;
    UINT32         newState;
    OMCI_FSMEVTHDL handler;


    GOS_ASSERT(fsmId < OMCI_MAX_FSM_ID);
    
    if (gOmciFsmInfo[fsmId].valid == TRUE)
    {
        pFsmInfo = &(gOmciFsmInfo[fsmId]);
        
        GOS_ASSERT(event < pFsmInfo->maxEvent);
        GOS_ASSERT(pFsmInfo->curState < pFsmInfo->maxState);
        newState = pFsmInfo->curState;
        
        
        handler = *(pFsmInfo->pHandlers + pFsmInfo->curState * pFsmInfo->maxEvent + event);
        if (handler)
        {
            newState = (*handler)(pEvtArg);
        }
        
        if (newState != pFsmInfo->curState)
        {
            pFsmInfo->curState = newState;
        }
        
        return GOS_OK;
    }

    return GOS_FAIL;
}


GOS_ERROR_CODE OMCI_OnCreateMsg(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    void*              pMibRowBuff = omci_GetMibRowBuff();
    PON_ME_CLASS_ID    classID     = (PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass;
    PON_ME_ENTITY_ID   entityID    = (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance;
    MIB_TABLE_INDEX    tableIndex  = MIB_GetTableIndexByClassId(classID);
    UINT32             entrySize   = MIB_GetTableEntrySize(tableIndex);
    PON_OMCI_ATTRS_SET attrsSet    = omci_GetOltAccAttrSet(tableIndex, PON_ME_OLT_SET_BY_CREATE);
    BOOL               meValid;
    BOOL               meExist;


    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling CREATE msg: class=%d, entity=0x%x",
                (PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass, (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance);

    MIB_Default(tableIndex, pMibRowBuff, entrySize);
    
    if (GOS_OK != OMCI_CheckIsMeValid(pNorOmciMsg->msgId.meClass, &pNorOmciMsg->msgId.meInstance, &meValid, &meExist))
    {
        return GOS_FAIL;
    }

    if (meValid == FALSE || meExist == TRUE)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"OnCreateMsg: unknown entity or entity exists in MIB, class=%d, entity=0x%x", 
                    (PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass, (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance);
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_INST_EXIST, TRUE);
        
        return GOS_FAIL;
    }

    omci_OmciDataToMibRow(tableIndex, &(pNorOmciMsg->content[0]), pMibRowBuff, attrsSet);

    // Create ME in MIB
    if (GOS_OK != MIB_Set(tableIndex, pMibRowBuff, entrySize))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"OnCreateMsg: MIB_Set error, class=%d, entity=0x%x", classID, entityID);
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_PROC_ERROR, TRUE);

        return GOS_FAIL;
    }
	
    if (GOS_OK != OMCI_MeOperCfg(tableIndex, pMibRowBuff, pMibRowBuff, MIB_ADD))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"OnCreateMsg: OMCI_MeOperCfg error, class=%d, entity=0x%x", classID, entityID);
        MIB_Delete(tableIndex, pMibRowBuff, entrySize);
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_PROC_ERROR, TRUE);

        return GOS_FAIL;
    }
	
    omci_SyncMibData();
    OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_SUCCESS, TRUE);   


    // Create Related ME
    if (GOS_OK != omci_CreateRelatedMe(tableIndex, pMibRowBuff))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"OnCreateMsg: CreateRelatedMe error, class=%d, entity=0x%x", classID, entityID);
    }
    
    return GOS_OK;
}


void* omci_GetMibRowBuff(void)
{
    memset(&gOmciMibRowBuff[0], 0x00, MIB_TABLE_ENTRY_MAX_SIZE);
    return &gOmciMibRowBuff[0];
}


GOS_ERROR_CODE OMCI_CheckIsMeValid(UINT16 MeClassID, UINT16* pMeInstance, BOOL* pMeValid, BOOL* pMeExist)
{
    CHAR* pMibRowBuff = omci_GetMibRowBuff();
    MIB_TABLE_INDEX tableIndex;


    GOS_ASSERT(pMeInstance != NULL);
    GOS_ASSERT(pMeValid != NULL);
    GOS_ASSERT(pMeExist != NULL);

    *pMeValid = FALSE;
    *pMeExist = FALSE;

    tableIndex = MIB_GetTableIndexByClassId((PON_ME_CLASS_ID)MeClassID);

    if (tableIndex != MIB_TABLE_UNKNOWN_INDEX)
    {
        *pMeValid = TRUE;

        MIB_SetAttrToBuf(tableIndex, MIB_ATTR_FIRST_INDEX, pMeInstance, pMibRowBuff, sizeof(PON_ME_ENTITY_ID));
        
        if (GOS_OK == MIB_Get(tableIndex, pMibRowBuff, MIB_GetTableEntrySize(tableIndex)))
        {
            *pMeExist = TRUE;
        }
    }
    
    return GOS_OK;    
}


PON_OMCI_ATTRS_SET omci_GetOltAccAttrSet(MIB_TABLE_INDEX tableIndex, PON_ME_OLT_ACC_TYPE accType)
{
    PON_OMCI_ATTRS_SET attrsSet;
    MIB_ATTRS_SET      mibAttrsSet;
    MIB_ATTR_INDEX     attrIndex;
    UINT32             i;

    MIB_ClearAttrSet(&mibAttrsSet);
    
    // skip the first attribute, EntityID
    for (attrIndex = MIB_ATTR_NEXT_INDEX(MIB_ATTR_FIRST_INDEX), i = 1; i < MIB_GetTableAttrNum(tableIndex);
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (accType & MIB_GetAttrOltAcc(tableIndex, attrIndex))
        {
            MIB_SetAttrSet(&mibAttrsSet, attrIndex);
        }
    }

    OMCI_AttrsMibSetToOmciSet(&mibAttrsSet, &attrsSet);

    return attrsSet;
}


GOS_ERROR_CODE OMCI_AttrsMibSetToOmciSet(MIB_ATTRS_SET* pMibAttrSet, PON_OMCI_ATTRS_SET* pOmciAttrSet)
{
    MIB_ATTR_INDEX attrIndex;
    UINT32 i;

    *pOmciAttrSet = 0;
    
    for (attrIndex = MIB_ATTR_NEXT_INDEX(MIB_ATTR_FIRST_INDEX), i = 0; i < sizeof(PON_OMCI_ATTRS_SET) * 8;
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (MIB_IsInAttrSet(pMibAttrSet, attrIndex))
        {
            *pOmciAttrSet = (*pOmciAttrSet) | (0x8000 >> i);
        }
    }

    return GOS_OK;
}


GOS_ERROR_CODE omci_OmciDataToMibRow(MIB_TABLE_INDEX tableIndex, UINT8* pOmciData, void* pMibRow, PON_OMCI_ATTRS_SET omciAttrSet)
{
    MIB_ATTRS_SET  mibAttrsSet;
    MIB_ATTR_INDEX attrIndex;
    UINT32         attrLen;
    MIB_ATTR_TYPE  attrType;
    UINT32         i;
 
    GOS_ASSERT(pOmciData);
    GOS_ASSERT(pMibRow);

    omci_AttrsOmciSetToMibSet(&omciAttrSet, &mibAttrsSet);
    
    for (attrIndex = MIB_ATTR_FIRST_INDEX, i = 0; i < MIB_GetTableAttrNum(tableIndex);
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (MIB_IsInAttrSet(&mibAttrsSet, attrIndex))
        {
            attrLen  = MIB_GetAttrLen(tableIndex, attrIndex);
            attrType = MIB_GetAttrDataType(tableIndex, attrIndex);
            switch (attrType)
            {
                case MIB_ATTR_TYPE_UINT8:
                {
                    UINT8 value;
                    value = *pOmciData;
                    MIB_SetAttrToBuf(tableIndex, attrIndex, &value, pMibRow, attrLen);
                    break;
                }
                
                case MIB_ATTR_TYPE_UINT16:
                {
                    UINT16 value;
                    value = GOS_Ntohs(GOS_GetUINT16((UINT16*)pOmciData));
                    MIB_SetAttrToBuf(tableIndex, attrIndex, &value, pMibRow, attrLen);
                    break;
                }
                
                case MIB_ATTR_TYPE_UINT32:
                {
                    UINT32 value;
                    value = GOS_Ntohl(GOS_GetUINT32((UINT32*)pOmciData));
                    MIB_SetAttrToBuf(tableIndex, attrIndex, &value, pMibRow, attrLen);
                    break;
                }                

                case MIB_ATTR_TYPE_UINT64:
                {
                    UINT64 value;
                    value = GOS_BuffToUINT64((CHAR*)pOmciData, attrLen);
                    MIB_SetAttrToBuf(tableIndex, attrIndex, &value, pMibRow, sizeof(UINT64));
                    break;
                }
                
                case MIB_ATTR_TYPE_STR:
                {
			MIB_SetAttrToBuf(tableIndex, attrIndex, pOmciData, pMibRow, attrLen+1);
                    	break;
                }

                case MIB_ATTR_TYPE_TABLE:
                {
			MIB_SetAttrToBuf(tableIndex, attrIndex, pOmciData, pMibRow, attrLen);
                    	break;
                }
                
                default:
                    break;
            }
            pOmciData  += attrLen;
        }
    }
	

    return GOS_OK;
}


GOS_ERROR_CODE omci_AttrsOmciSetToMibSet(PON_OMCI_ATTRS_SET* pOmciAttrSet, MIB_ATTRS_SET* pMibAttrSet)
{
    MIB_ATTR_INDEX attrIndex;
    UINT32 i;

    MIB_ClearAttrSet(pMibAttrSet);
    
    for (attrIndex = MIB_ATTR_NEXT_INDEX(MIB_ATTR_FIRST_INDEX), i = 0; i < sizeof(PON_OMCI_ATTRS_SET) * 8;
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if ((*pOmciAttrSet) & (0x8000 >> i))
        {
            MIB_SetAttrSet(pMibAttrSet, attrIndex);
        }
    }

    return GOS_OK;
}


GOS_ERROR_CODE omci_SyncMibData(void)
{
    MIB_TABLE_ONTDATA_T mibRow;
    
    memset(&mibRow, 0x00, sizeof(MIB_TABLE_ONTDATA_T));
    
    GOS_ASSERT(GOS_OK == MIB_GetFirst(MIB_TABLE_ONTDATA_INDEX, &mibRow, sizeof(MIB_TABLE_ONTDATA_T)));

    mibRow.MIBDataSync++;
    
    if (0 == mibRow.MIBDataSync)
    {
        mibRow.MIBDataSync = 1;
    }
    
    GOS_ASSERT(GOS_OK == MIB_Set(MIB_TABLE_ONTDATA_INDEX, &mibRow, sizeof(MIB_TABLE_ONTDATA_T)));

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"omci_SyncMibData: dataSync =0x%x", mibRow.MIBDataSync);

    return GOS_OK;
}


GOS_ERROR_CODE omci_CreateRelatedMe(MIB_TABLE_INDEX tableIndex, void* pMibRow)
{
    if (MIB_TABLE_GEMIWTP_INDEX == tableIndex)
    {
        MIB_TABLE_GEMIWTP_T* pIwTp = (MIB_TABLE_GEMIWTP_T*)pMibRow;
        pIwTp->OpState = PON_ENABLE;
        MIB_Set(MIB_TABLE_GEMIWTP_INDEX, pIwTp, sizeof(MIB_TABLE_GEMIWTP_T));
    }

    if (MIB_TABLE_MULTIGEMIWTP_INDEX == tableIndex)
    {
        MIB_TABLE_MULTIGEMIWTP_T* pIwTp = (MIB_TABLE_MULTIGEMIWTP_T*)pMibRow;
        pIwTp->OpState = PON_ENABLE;
        MIB_Set(MIB_TABLE_MULTIGEMIWTP_INDEX, pIwTp, sizeof(MIB_TABLE_MULTIGEMIWTP_T));
    }

    return GOS_OK;
}


GOS_ERROR_CODE OMCI_OnDeleteMsg(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    MIB_TABLE_INDEX tableIndex;
    UINT32          entrySize;
    void*           pMibRowBuff;
    BOOL            meValid;
    BOOL            meExist;
    PON_OMCI_ATTRS_SET attrsSet    = omci_GetOltAccAttrSet(tableIndex, PON_ME_OLT_SET_BY_CREATE);


    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling DELETE msg: class=%d, entity=0x%x", 
                (PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass, (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance);

    if (GOS_OK != OMCI_CheckIsMeValid(pNorOmciMsg->msgId.meClass, &pNorOmciMsg->msgId.meInstance, &meValid, &meExist))
    {
        return GOS_FAIL;
    }

    if (meValid == FALSE || meExist == FALSE)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"OnDeleteMsg: entity not exist in MIB, class=%d, entity=0x%x", 
                    (PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass, (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance);
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_UNKNOWN_ME_INST, TRUE);
        
        return GOS_FAIL;
    }

    tableIndex  = MIB_GetTableIndexByClassId((PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass);
    entrySize   = MIB_GetTableEntrySize(tableIndex);
    pMibRowBuff = omci_GetMibRowBuff();

	MIB_SetAttrToBuf(tableIndex, MIB_ATTR_FIRST_INDEX, &pNorOmciMsg->msgId.meInstance, pMibRowBuff, sizeof(PON_ME_ENTITY_ID));
    
	if(MIB_Get(tableIndex, pMibRowBuff, entrySize)!=GOS_OK)
	{
		OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"OnDeleteMsg: can't find old value");
		return GOS_FAIL;	
	}
	
    OMCI_MeOperCfg(tableIndex, pMibRowBuff, pMibRowBuff, MIB_DEL);
    // Delete ME in MIB
    if (GOS_OK != MIB_Delete(tableIndex, pMibRowBuff, entrySize))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"OnDeleteMsg: MIB_Delete error, class=%d, entity=0x%x", 
                    (PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass, (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance);
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_PROC_ERROR, TRUE);

        return GOS_FAIL;
    }

    omci_SyncMibData();
    OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_SUCCESS, TRUE); 

    return GOS_OK;   
}


GOS_ERROR_CODE OMCI_OnSetMsg(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    MIB_TABLE_INDEX     tableIndex;
    PON_OMCI_ATTRS_SET  attrOmciSet;
    MIB_ATTRS_SET       validMibSet;
    UINT32              attrSize;
    MIB_ATTR_INDEX      attrIndex;
    BOOL                meValid;
    BOOL                meExist;
    UINT32              i;
    
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling SET msg: class=%d, entity=0x%x",
                (PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass, (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance);

    if (GOS_OK != OMCI_CheckIsMeValid(pNorOmciMsg->msgId.meClass, &pNorOmciMsg->msgId.meInstance, &meValid, &meExist))
    {
        return GOS_FAIL;
    }

    if (meValid == FALSE || meExist == FALSE)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"OnSetMsg: entity not exist in MIB, class=%d, entity=0x%x", 
                    (PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass, (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance);
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_UNKNOWN_ME_INST, TRUE);
        
        return GOS_FAIL;
    }

    tableIndex  = MIB_GetTableIndexByClassId((PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass);
    attrOmciSet = (PON_OMCI_ATTRS_SET)GOS_Ntohs(GOS_GetUINT16((UINT16*)&pNorOmciMsg->content[0]));
    
    omci_AttrsOmciSetToMibSet(&attrOmciSet, &validMibSet);

    // Get total attribute size in set
    for (attrIndex = MIB_ATTR_FIRST_INDEX, i = 0, attrSize = 0; i < MIB_GetTableAttrNum(tableIndex);
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (MIB_IsInAttrSet(&validMibSet, attrIndex))
        {
            attrSize += MIB_GetAttrLen(tableIndex, attrIndex);
        }
    }

    return omci_SingleSet(pNorOmciMsg);
}


GOS_ERROR_CODE omci_SingleSet(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    void*               pMibRowBuff     = omci_GetMibRowBuff();
    PON_ME_CLASS_ID     classID         = (PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass;
    PON_ME_ENTITY_ID    entityID        = (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance;
    MIB_TABLE_INDEX     tableIndex      = MIB_GetTableIndexByClassId(classID);
    UINT32              entrySize       = MIB_GetTableEntrySize(tableIndex);
    PON_OMCI_ATTRS_SET  supportOmciSet  = omci_GetOptionAttrSet(tableIndex, PON_ME_ATTR_OPT_SUPPORT) | omci_GetOptionAttrSet(tableIndex, PON_ME_ATTR_MANDATORY);
    PON_OMCI_ATTRS_SET  writableOmciSet = omci_GetOltAccAttrSet(tableIndex, PON_ME_OLT_WRITE);
    PON_OMCI_ATTRS_SET  attrOmciSet;
    PON_OMCI_ATTRS_SET  execOmciSet, validOmciSet;
    MIB_ATTRS_SET       validMibSet, unsupportOmciSet;
    CHAR                oldMibRow[MIB_TABLE_ENTRY_MAX_SIZE];

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling SingleSet msg: class=%d, entity=0x%x", classID, entityID);
    
    // Entity ID is the first attribute
    MIB_SetAttrToBuf(tableIndex, MIB_ATTR_FIRST_INDEX, &entityID, pMibRowBuff, sizeof(entityID));

    // Check if entity exists or not
    if (GOS_OK != MIB_Get(tableIndex, pMibRowBuff, entrySize))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"SingleSet: entity not exist in MIB, class=%d, entity=0x%x", classID, entityID);
        return OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_UNKNOWN_ME_INST, TRUE);
    }

    memcpy(oldMibRow, pMibRowBuff, entrySize);

    attrOmciSet = (PON_OMCI_ATTRS_SET)GOS_Ntohs(GOS_GetUINT16((UINT16*)&pNorOmciMsg->content[0]));

    omci_OmciDataToMibRow(tableIndex, &(pNorOmciMsg->content[2]), pMibRowBuff, attrOmciSet);

    // validSet indicats the valid attributes that can be set
    validOmciSet = attrOmciSet & supportOmciSet & writableOmciSet;
    omci_AttrsOmciSetToMibSet(&validOmciSet, &validMibSet);

    // Save in MIB
    if (GOS_OK != MIB_SetAttributes(tableIndex, pMibRowBuff, entrySize, &validMibSet))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"SingleSet: MIB_Set error, class=%d, entity=0x%x, attrSet=0x%x", 
                  classID, entityID, validMibSet);
        return OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_PROC_ERROR, TRUE);
    }
    
    if (GOS_OK != MIB_Get(tableIndex, pMibRowBuff, entrySize))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"SingleSet: entity not exist in MIB, class=%d, entity=0x%x", classID, entityID);
        return OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_PROC_ERROR, TRUE);
    }
	
    if (GOS_OK != OMCI_MeOperCfg(tableIndex, oldMibRow, pMibRowBuff, MIB_SET))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"SingleSet: SingleSet error, class=%d, entity=0x%x, attrSet=0x%x", 
                  classID, entityID, validMibSet);
        // Rollback MIB
        MIB_Set(tableIndex, oldMibRow, entrySize);
        // Process error
        return OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_PROC_ERROR, TRUE);
    }


    omci_SyncMibData();

    // execSet indicats the attributes that can not be set but suppose to supporting set
    execOmciSet = validOmciSet;
    unsupportOmciSet = (~supportOmciSet) & attrOmciSet;
    
    // part of attributes executed, refer 983.2 P.335
    if (unsupportOmciSet)
    {
        memset(&pNorOmciMsg->content[0], 0x00, PON_OMCI_MSG_CONTENT_LEN);
        GOS_SetUINT16((UINT16*)&pNorOmciMsg->content[1], GOS_Htons((UINT16)unsupportOmciSet));
        GOS_SetUINT16((UINT16*)&pNorOmciMsg->content[3], GOS_Htons((UINT16)execOmciSet));
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_ATTR_FAILED, FALSE);
    }
    else
    {
        // Success
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_SUCCESS, TRUE);
    }

    return GOS_OK;
}


PON_OMCI_ATTRS_SET omci_GetOptionAttrSet(MIB_TABLE_INDEX tableIndex, PON_ME_ATTR_OPT_TYPE optType)
{
    PON_OMCI_ATTRS_SET attrsSet;
    MIB_ATTRS_SET      mibAttrsSet;
    MIB_ATTR_INDEX     attrIndex;
    UINT32             i;

    MIB_ClearAttrSet(&mibAttrsSet);
    
    // skip the first attribute, EntityID
    for (attrIndex = MIB_ATTR_NEXT_INDEX(MIB_ATTR_FIRST_INDEX), i = 1; i < MIB_GetTableAttrNum(tableIndex);
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (optType == MIB_GetAttrOptionType(tableIndex, attrIndex))
        {
            MIB_SetAttrSet(&mibAttrsSet, attrIndex);
        }
    }

    OMCI_AttrsMibSetToOmciSet(&mibAttrsSet, &attrsSet);

    return attrsSet;
}


GOS_ERROR_CODE OMCI_OnGetMsg(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    MIB_TABLE_INDEX tableIndex;
    MIB_ATTRS_SET   validMibSet;
    UINT32          attrSize;
	UINT32 			entrySize;
    MIB_ATTR_INDEX  attrIndex;
    BOOL            meValid;
    BOOL            meExist;
    UINT32          i;
    
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling GET msg: class=%d, entity=0x%x",
                (PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass, (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance);

    if (GOS_OK != OMCI_CheckIsMeValid(pNorOmciMsg->msgId.meClass, &pNorOmciMsg->msgId.meInstance, &meValid, &meExist))
    {
        return GOS_FAIL;
    }

    if (meValid == FALSE || meExist == FALSE)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"OnGetMsg: entity not exist in MIB, class=%d, entity=0x%x", 
                    (PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass, (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance);
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_UNKNOWN_ME_INST, TRUE);
        
        return GOS_FAIL;
    }

    tableIndex = MIB_GetTableIndexByClassId((PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass);
    entrySize  = MIB_GetTableEntrySize(tableIndex);


    // To check if it is a multi-get
    for (attrIndex = MIB_ATTR_FIRST_INDEX, i = 0, attrSize = 0; i < MIB_GetTableAttrNum(tableIndex);
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (MIB_IsInAttrSet(&validMibSet, attrIndex))
        {
            if (MIB_ATTR_TYPE_TABLE == MIB_GetAttrDataType(tableIndex, attrIndex))
            {
                return omci_MultiGet(pNorOmciMsg);
            }
            else
            {
                attrSize = MIB_GetAttrLen(tableIndex, attrIndex);
                if (attrSize > PON_OMCI_MSG_GET_LIMIT)
                {
                    return omci_MultiGet(pNorOmciMsg);
                }
            }
        }
    }

    return omci_SingleGet(pNorOmciMsg);
}


GOS_ERROR_CODE omci_SingleGet(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    void*               pMibRowBuff    = omci_GetMibRowBuff();
    PON_ME_CLASS_ID     classID        = (PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass;
    PON_ME_ENTITY_ID    entityID       = (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance;
    MIB_TABLE_INDEX     tableIndex     = MIB_GetTableIndexByClassId(classID);
    UINT32              entrySize      = MIB_GetTableEntrySize(tableIndex);
    PON_OMCI_ATTRS_SET  supportOmciSet = omci_GetOptionAttrSet(tableIndex, PON_ME_ATTR_OPT_SUPPORT) | omci_GetOptionAttrSet(tableIndex, PON_ME_ATTR_MANDATORY);
    PON_OMCI_ATTRS_SET  attrOmciSet, validOmciSet, execOmciSet, succOmciSet;
    MIB_ATTRS_SET       validMibSet, succMibSet, unsupportOmciSet;

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Handling SingleGet msg: class=%d, entity=0x%x", classID, entityID);
    
    // Entity ID is the first attribute
    MIB_SetAttrToBuf(tableIndex, MIB_ATTR_FIRST_INDEX, &entityID, pMibRowBuff, sizeof(entityID));

    // To inform driver to update MIB
    
    OMCI_MeOperCfg(tableIndex, pMibRowBuff, pMibRowBuff, MIB_GET);
	
    // Check if entity exists or not
    if (GOS_OK != MIB_Get(tableIndex, pMibRowBuff, entrySize))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"SingleGet: entity not exist in MIB, class=%d, entity=0x%x", classID, entityID);
        return OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_UNKNOWN_ME_INST, TRUE);
    }

    attrOmciSet = (PON_OMCI_ATTRS_SET)GOS_Ntohs(GOS_GetUINT16((UINT16*)&pNorOmciMsg->content[0]));
    memset(&pNorOmciMsg->content[0], 0x00, PON_OMCI_MSG_CONTENT_LEN);

    // validSet indicats the valid attributes that can be get
    validOmciSet = attrOmciSet & supportOmciSet;
    omci_AttrsOmciSetToMibSet(&validOmciSet, &validMibSet);

    succMibSet = omci_MibRowToOmciData(tableIndex, pMibRowBuff, &pNorOmciMsg->content[3], &validMibSet);
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"SingleGet: MibRowToOmciData, class=%d, entity=0x%x, validSet=0x%x, succSet=0x%x", 
              classID, entityID, validMibSet, succMibSet);
    
    OMCI_AttrsMibSetToOmciSet(&succMibSet, &succOmciSet);
    
    // execSet indicats the attributes that can not be get but suppose to supporting get
    execOmciSet = validOmciSet & (~succOmciSet);
    unsupportOmciSet = (~supportOmciSet) & attrOmciSet;

    GOS_SetUINT16((UINT16*)&pNorOmciMsg->content[1], GOS_Htons((UINT16)succOmciSet));

    // part of attributes executed
    if (unsupportOmciSet)
    {
        GOS_SetUINT16((UINT16*)&pNorOmciMsg->content[PON_OMCI_MSG_CONTENT_LEN - 4], GOS_Htons((UINT16)unsupportOmciSet));
        GOS_SetUINT16((UINT16*)&pNorOmciMsg->content[PON_OMCI_MSG_CONTENT_LEN - 2], GOS_Htons((UINT16)execOmciSet));
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_ATTR_FAILED, FALSE);
    }
    else
    {
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_SUCCESS, FALSE);
    }

    return GOS_OK;
}


MIB_ATTRS_SET omci_MibRowToOmciData(MIB_TABLE_INDEX tableIndex, void* pMibRow, UINT8* pOmciData, MIB_ATTRS_SET* pMibAttrSet)
{
    MIB_ATTR_INDEX attrIndex;
    UINT32         attrLen;
    MIB_ATTR_TYPE  attrType;
    MIB_ATTRS_SET  resultSet;
    UINT32         doneSize;
    UINT32         i;

    GOS_ASSERT(pOmciData);
    GOS_ASSERT(pMibRow);

    MIB_ClearAttrSet(&resultSet);

    for (attrIndex = MIB_ATTR_FIRST_INDEX, i = 0, doneSize = 0; i < MIB_GetTableAttrNum(tableIndex);
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (MIB_IsInAttrSet(pMibAttrSet, attrIndex))
        {
            attrLen = MIB_GetAttrLen(tableIndex, attrIndex);
            attrType = MIB_GetAttrDataType(tableIndex, attrIndex);

            // exceed omci message size, just break;
            if ((attrLen + doneSize) > PON_OMCI_MSG_GET_LIMIT)
            {
                break;
            }

            MIB_SetAttrSet(&resultSet, attrIndex);
            
            switch (attrType)
            {
                case MIB_ATTR_TYPE_UINT8:
                {
                    UINT8 value;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pMibRow, attrLen);
                    *pOmciData = value;
                    break;
                }
                
                case MIB_ATTR_TYPE_UINT16:
                {
                    UINT16 value;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pMibRow, attrLen);
                    GOS_SetUINT16((UINT16*)pOmciData, GOS_Htons(value));
                    break;
                }
                
                case MIB_ATTR_TYPE_UINT32:
                {
                    UINT32 value;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pMibRow, attrLen);
                    GOS_SetUINT32((UINT32*)pOmciData, GOS_Htonl(value));
                    break;
                }                

                case MIB_ATTR_TYPE_UINT64:
                {
                    UINT64 value;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pMibRow, sizeof(UINT64));
                    value = GOS_Htonll(value);
                    GOS_UINT64ToBuff(value, (CHAR*)pOmciData, attrLen);
                    break;
                }                
                
                case MIB_ATTR_TYPE_STR:
                {
                    CHAR temp[MIB_TABLE_ATTR_MAX_SIZE];
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, temp, pMibRow, attrLen + 1);
                    memcpy(pOmciData, temp, attrLen);
                    break;
                }

                case MIB_ATTR_TYPE_TABLE:
                {
                    CHAR temp[MIB_TABLE_ATTR_MAX_SIZE];
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, temp, pMibRow, attrLen);
                    memcpy(pOmciData, temp, attrLen);
                    break;
                }
                
                default:
                    break;
            }
            pOmciData  += attrLen;
            doneSize   += attrLen;
        }
    }
    
    return resultSet;
}


GOS_ERROR_CODE omci_MultiGet(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    UINT32 pri = pNorOmciMsg->priority;
    
    GOS_ASSERT(pri < PON_OMCI_MSG_PRI_NUM);

    omci_FsmRun(gOmciMulGetFsm[pri], OMCI_MULGET_EVT_START, pNorOmciMsg);
    
    return GOS_OK;
}


GOS_ERROR_CODE OMCI_OnGetNextMsg(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    UINT32 pri = pNorOmciMsg->priority;

    GOS_ASSERT(pri < PON_OMCI_MSG_PRI_NUM);

    omci_FsmRun(gOmciMulGetFsm[pri], OMCI_MULGET_EVT_GET, pNorOmciMsg);
    
    return GOS_OK;
}


UINT32 omci_MulGetStartOnIdle(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    void*              pMibRowBuff    = omci_GetMibRowBuff();
    UINT32             pri            = pNorOmciMsg->priority;
    PON_ME_CLASS_ID    classID        = (PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass;
    PON_ME_ENTITY_ID   entityID       = (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance;
    MIB_TABLE_INDEX    tableIndex     = MIB_GetTableIndexByClassId(classID);
    PON_OMCI_ATTRS_SET supportOmciSet = omci_GetOptionAttrSet(tableIndex, PON_ME_ATTR_OPT_SUPPORT) | omci_GetOptionAttrSet(tableIndex, PON_ME_ATTR_MANDATORY);
    PON_OMCI_ATTRS_SET attrOmciSet    = GOS_Ntohs(GOS_GetUINT16((UINT16*)&pNorOmciMsg->content[0]));
    UINT8*             pOmciData      = &pNorOmciMsg->content[3];
    PON_OMCI_ATTRS_SET validOmciSet, execOmciSet, succOmciSet;
    MIB_ATTRS_SET      validMibSet, unsupportOmciSet, succMibSet = 0;
    MIB_ATTR_INDEX     attrIndex; 
    UINT32             i;

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Start Getting Large Attributes: class=%d, entity=0x%x", classID, entityID);

    // Entity ID is the first attribute
    MIB_SetAttrToBuf(tableIndex, MIB_ATTR_FIRST_INDEX, &entityID, pMibRowBuff, sizeof(entityID));

    // To inform driver to update MIB
    
    OMCI_MeOperCfg(tableIndex, pMibRowBuff, pMibRowBuff, MIB_GET);
	
    // Check if entity exists or not
    if (GOS_OK != MIB_Get(tableIndex, pMibRowBuff, MIB_GetTableEntrySize(tableIndex)))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"MulGet: entity not exist in MIB, class=%d, entity=0x%x", classID, entityID);
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_UNKNOWN_ME_INST, TRUE);
        return OMCI_MULGET_STATE_IDLE;
    }

    validOmciSet = attrOmciSet & supportOmciSet;
    omci_AttrsOmciSetToMibSet(&validOmciSet, &validMibSet);

    memset(&pNorOmciMsg->content[0], 0x00, PON_OMCI_MSG_CONTENT_LEN);

    for (attrIndex = MIB_ATTR_FIRST_INDEX, i = 0; i < MIB_GetTableAttrNum(tableIndex);
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (MIB_IsInAttrSet(&validMibSet, attrIndex))
        {
            MIB_ATTR_TYPE attrType;
            UINT32 doneSize = 0;
            UINT32 attrLen;    
            
            attrLen = MIB_GetAttrLen(tableIndex, attrIndex);
            attrType = MIB_GetAttrDataType(tableIndex, attrIndex);

            // table or large attribute
            if ((attrType == MIB_ATTR_TYPE_TABLE)||(attrLen > PON_OMCI_MSG_GET_LIMIT))
            {
                attrLen = 4;
            }

            // exceed omci message size, just break;
            if ((attrLen + doneSize) > PON_OMCI_MSG_GET_LIMIT)
            {
                break;
            }

            MIB_SetAttrSet(&succMibSet, attrIndex);
            
            switch (attrType)
            {
                case MIB_ATTR_TYPE_UINT8:
                {
                    UINT8 value;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pMibRowBuff, attrLen);
                    *pOmciData = value;
                    break;
                }
                
                case MIB_ATTR_TYPE_UINT16:
                {
                    UINT16 value;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pMibRowBuff, attrLen);
                    GOS_SetUINT16((UINT16*)pOmciData, GOS_Htons(value));
                    break;
                }
                
                case MIB_ATTR_TYPE_UINT32:
                {
                    UINT32 value;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pMibRowBuff, attrLen);
                    GOS_SetUINT32((UINT32*)pOmciData, GOS_Htonl(value));
                    break;
                }                

                case MIB_ATTR_TYPE_UINT64:
                {
                    UINT64 value;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pMibRowBuff, sizeof(UINT64));
                    value = GOS_Htonll(value);
                    GOS_UINT64ToBuff(value, (CHAR*)pOmciData, attrLen);
                    break;
                }                
                
                case MIB_ATTR_TYPE_STR:
                {
                    if (MIB_GetAttrLen(tableIndex, attrIndex) > PON_OMCI_MSG_GET_LIMIT)
                    {
                        MIB_GetAttrFromBuf(tableIndex, attrIndex, gOmciMulGetData[pri].attribute[attrIndex].attrValue, pMibRowBuff, MIB_GetAttrLen(tableIndex, attrIndex) + 1);
                        gOmciMulGetData[pri].attribute[attrIndex].attrSize = MIB_GetAttrLen(tableIndex, attrIndex);
                        GOS_SetUINT32((UINT32*)pOmciData, GOS_Htonl(MIB_GetAttrLen(tableIndex, attrIndex)));
                        
                        gOmciMulGetData[pri].attribute[attrIndex].attrIndex = attrIndex;
                        gOmciMulGetData[pri].attribute[attrIndex].doneSeqNum = 0;
                        gOmciMulGetData[pri].attribute[attrIndex].maxSeqNum = (gOmciMulGetData[pri].attribute[attrIndex].attrSize + PON_OMCI_MSG_GET_NEXT_LIMIT - 1) / PON_OMCI_MSG_GET_NEXT_LIMIT;
                    }
                    else
                    {
                        CHAR temp[MIB_TABLE_ATTR_MAX_SIZE];
                        MIB_GetAttrFromBuf(tableIndex, attrIndex, temp, pMibRowBuff, attrLen + 1);
                        memcpy(pOmciData, temp, attrLen);
                    }
                    break;
                }

                case MIB_ATTR_TYPE_TABLE:
                {
                    MIB_TABLE_INDEX  subTableIndex;
                    GOS_ERROR_CODE   ret;
                    PON_ME_ENTITY_ID entryEntityId;
                    UINT32           offset = 0;
            
                    subTableIndex = MIB_GetTableIndexByName(MIB_GetAttrName(tableIndex, attrIndex));
                    if (MIB_TABLE_INDEX_VALID(subTableIndex))
                    {
                        ret = MIB_GetFirst(subTableIndex, pMibRowBuff, MIB_GetTableEntrySize(subTableIndex));
                        while (GOS_OK == ret)
                        {
                            MIB_GetAttrFromBuf(subTableIndex, MIB_ATTR_FIRST_INDEX, &entryEntityId, pMibRowBuff, sizeof(UINT16));
                            if (entryEntityId == entityID)
                            {
                                offset += omci_GetTableAttrLen(tableIndex, attrIndex);
                            }
            
                            ret = MIB_GetNext(subTableIndex, pMibRowBuff, MIB_GetTableEntrySize(subTableIndex));
                        }
                    }
                    gOmciMulGetData[pri].attribute[attrIndex].attrSize = offset;
                    GOS_SetUINT32((UINT32*)pOmciData, GOS_Htonl(offset));
                    
                    gOmciMulGetData[pri].attribute[attrIndex].attrIndex = attrIndex;
                    gOmciMulGetData[pri].attribute[attrIndex].doneSeqNum = 0;
                    gOmciMulGetData[pri].attribute[attrIndex].maxSeqNum = (gOmciMulGetData[pri].attribute[attrIndex].attrSize + PON_OMCI_MSG_GET_NEXT_LIMIT - 1) / PON_OMCI_MSG_GET_NEXT_LIMIT;
                    break;
                }
                
                default:
                    break;
            }
            
            pOmciData  += attrLen;
            doneSize   += attrLen;
        }
    }
              
    gOmciMulGetData[pri].classID = classID;
    gOmciMulGetData[pri].entityID = entityID;

    OMCI_AttrsMibSetToOmciSet(&succMibSet, &succOmciSet);
    
    // execSet indicats the attributes that can not be get but suppose to supporting get
    execOmciSet = validOmciSet & (~succOmciSet);
    unsupportOmciSet = (~supportOmciSet) & attrOmciSet;

    GOS_SetUINT16((UINT16*)&pNorOmciMsg->content[1], GOS_Htons((UINT16)succOmciSet));

    // part of attributes executed
    if (unsupportOmciSet)
    {
        GOS_SetUINT16((UINT16*)&pNorOmciMsg->content[PON_OMCI_MSG_CONTENT_LEN - 4], GOS_Htons((UINT16)unsupportOmciSet));
        GOS_SetUINT16((UINT16*)&pNorOmciMsg->content[PON_OMCI_MSG_CONTENT_LEN - 2], GOS_Htons((UINT16)execOmciSet));
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_ATTR_FAILED, FALSE);
    }
    else
    {
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_SUCCESS, FALSE);
    }
    
    return OMCI_MULGET_STATE_RUN;
}


UINT32 omci_GetTableAttrLen(MIB_TABLE_INDEX tableIndex, MIB_ATTR_INDEX attrIndex)
{
    // here MIB_GetAttrLen(tableIndex, attrIndex) is 12 * 2, :(
    if ((MIB_TABLE_MULTIGEMIWTP_INDEX == tableIndex) && \
        (MIB_TABLE_MULTIGEMIWTP_MCASTADDRTABLE_INDEX == attrIndex))
    {
        return 12;
    }

    return MIB_GetAttrLen(tableIndex, attrIndex);
}


UINT32 omci_MulGetGetOnIdle(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_PARAM_ERROR, TRUE);
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Get on IDLE state of Getting Large Attribute");
    return OMCI_MULGET_STATE_IDLE;
}


UINT32 omci_MulGetStartOnRun(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    UINT32 pri = pNorOmciMsg->priority;
    PON_OMCI_ATTRS_SET attrOmciSet = GOS_Ntohs(GOS_GetUINT16((UINT16*)&pNorOmciMsg->content[0]));
    MIB_ATTR_INDEX     attrIndex = omci_GetFirstAttrIndex(attrOmciSet);

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Terminate(interrupt) Getting Large Attribute: class=%d, entity=0x%x, attr=%d", 
              gOmciMulGetData[pri].classID, gOmciMulGetData[pri].entityID, attrIndex);

    // Do same thing as startOnIdle
    return omci_MulGetStartOnIdle(pNorOmciMsg);
}


UINT32 omci_MulGetGetOnRun(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    UINT32             pri = pNorOmciMsg->priority;
    PON_ME_CLASS_ID    classID = (PON_ME_CLASS_ID)pNorOmciMsg->msgId.meClass;
    PON_ME_ENTITY_ID   entityID = (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance;
    PON_OMCI_ATTRS_SET attrOmciSet = GOS_Ntohs(GOS_GetUINT16((UINT16*)&pNorOmciMsg->content[0]));
    MIB_ATTR_INDEX     attrIndex = omci_GetFirstAttrIndex(attrOmciSet);
    UINT16             seqNum = GOS_Ntohs(GOS_GetUINT16((UINT16*)&pNorOmciMsg->content[2]));
    UINT32             curSize;
    UINT32             offset;

    if ((gOmciMulGetData[pri].classID != classID) ||
        (gOmciMulGetData[pri].entityID != entityID) ||
        (gOmciMulGetData[pri].attribute[attrIndex].attrIndex != attrIndex))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Received para is wrong: pri=%d, class=%d, entity=0x%x, attr=%d",
                              pri, classID, entityID, attrIndex);
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_PARAM_ERROR, TRUE);
        return OMCI_MULGET_STATE_RUN;
    }

    // Sequence number is out of range
    if (seqNum >= gOmciMulGetData[pri].attribute[attrIndex].maxSeqNum)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Received sequence is out of range: seq=%d,max=%d",
                  seqNum, gOmciMulGetData[pri].attribute[attrIndex].maxSeqNum);
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_PARAM_ERROR, TRUE);
        return OMCI_MULGET_STATE_RUN;
    }

    curSize = PON_OMCI_MSG_GET_NEXT_LIMIT;
    // if it is the last sequence
    if (seqNum == gOmciMulGetData[pri].attribute[attrIndex].maxSeqNum - 1)
    {
        curSize = gOmciMulGetData[pri].attribute[attrIndex].attrSize - PON_OMCI_MSG_GET_NEXT_LIMIT * seqNum;
    }

    offset = PON_OMCI_MSG_GET_NEXT_LIMIT * seqNum;

    GOS_SetUINT16((UINT16*)&pNorOmciMsg->content[1], GOS_Htons(attrOmciSet));
    memcpy(&pNorOmciMsg->content[3], gOmciMulGetData[pri].attribute[attrIndex].attrValue + offset, curSize);
    OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_SUCCESS, FALSE);

    
    gOmciMulGetData[pri].attribute[attrIndex].doneSeqNum++;

    // Done
    if (gOmciMulGetData[pri].attribute[attrIndex].doneSeqNum >= gOmciMulGetData[pri].attribute[attrIndex].maxSeqNum)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Terminate(done) Getting Large Attribute: class=%d, entity=0x%x, attr=%d", 
                 gOmciMulGetData[pri].classID, gOmciMulGetData[pri].entityID, attrIndex);
        return OMCI_MULGET_STATE_IDLE;
    }
    
    return OMCI_MULGET_STATE_RUN;
}


GOS_ERROR_CODE OMCI_OnMibReset(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"OLT reset MIB");

	OMCI_wrapper_ResetMib();    
    if (GOS_OK == OMCI_ResetMib())
    {
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_SUCCESS, TRUE);
    }
    else
    {
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_PROC_ERROR, TRUE);
    }

    omci_ReportAvcs();

    return GOS_OK;
}


GOS_ERROR_CODE omci_ReportAvcs(void)
{
    MIB_ATTR_INDEX     attrIndex;
    UINT32             i, attrSize,entrySize;
    MIB_ATTRS_SET      avcAttrSet;
    PON_ME_ENTITY_ID   entityID;
    PON_OMCI_MSG_H_T   omciMsg;
    PON_OMCI_ATTRS_SET avcOmciSet;
    GOS_ERROR_CODE     ret;
    CHAR*              pData;
    MIB_TABLE_INDEX    tableIndex;
    CHAR*              pRow = omci_GetMibRowBuff();

    for (i = 0, tableIndex = MIB_TABLE_FIRST_INDEX; i <  MIB_TABLE_TOTAL_NUMBER; i++, tableIndex = MIB_TABLE_NEXT_INDEX(tableIndex))
    {
        // only report these MEs' AVC
        if (MIB_TABLE_ETHUNI_INDEX != tableIndex)
        {
            continue;
        }

		
		entrySize =  MIB_GetTableEntrySize(tableIndex);
        ret = MIB_GetFirst(tableIndex, pRow,entrySize);
        while (GOS_OK == ret)
        {
            MIB_ClearAttrSet(&avcAttrSet);

            MIB_GetAttrFromBuf(tableIndex, MIB_ATTR_FIRST_INDEX, &entityID, pRow, sizeof(PON_ME_ENTITY_ID));

			omciMsg.msgId.meInstance = entityID;
            omciMsg.msgId.meClass = MIB_GetTableClassId(tableIndex);
			omciMsg.type = PON_OMCI_MSG_AVC;
            memset(&omciMsg.content[0], 0x00, PON_OMCI_MSG_CONTENT_LEN);

            pData = (CHAR*)(&omciMsg.content[2]);

            for (attrIndex = MIB_ATTR_FIRST_INDEX, i = 0; i < MIB_GetTableAttrNum(tableIndex);
                 i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
            {
                // Not a AVC attribute
                if (!MIB_GetAttrAvcFlag(tableIndex, attrIndex))
                {
                    continue;
                }
                
                attrSize = MIB_GetAttrSize(tableIndex, attrIndex);
                
                switch (MIB_GetAttrDataType(tableIndex, attrIndex))
                {
                    case MIB_ATTR_TYPE_UINT8:
                    {
                        UINT8 value;
                        MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pRow, attrSize);
                        memcpy(pData, &value, attrSize);
                        break;
                    }
                    case MIB_ATTR_TYPE_STR:
                    {
                        CHAR value[MIB_TABLE_ATTR_MAX_SIZE];
                        MIB_GetAttrFromBuf(tableIndex, attrIndex, value, pRow, attrSize);
                        memcpy(pData, value, attrSize);
                        break;
                    }
                    case MIB_ATTR_TYPE_UINT16:
                    {
                        UINT16 value;
                        MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pRow, attrSize);
                        GOS_SetUINT16((UINT16*)pData, GOS_Htons(value));
                        break;
                    }
                    case MIB_ATTR_TYPE_UINT32:
                    {
                        UINT32 value;
                        MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pRow, attrSize);
                        GOS_SetUINT32((UINT32*)pData, GOS_Htonl(value));
                        break;
                    }
                    case MIB_ATTR_TYPE_UINT64:
                    {
                        UINT64 value;
                        MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, pRow, attrSize);
                        value = GOS_Htonll(value);
                        GOS_UINT64ToBuff(value, pData, MIB_GetAttrLen(tableIndex, attrIndex));
                        break;
                    }

                    case MIB_ATTR_TYPE_TABLE:
                        break;
                    default:
                        break;
                }

                pData += attrSize;
                MIB_SetAttrSet(&avcAttrSet, attrIndex);
            }

            OMCI_AttrsMibSetToOmciSet(&avcAttrSet, &avcOmciSet);
            GOS_SetUINT16((UINT16*)&omciMsg.content[0], GOS_Htons((UINT16)avcOmciSet));

            OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"%s: Class %d, Entity 0x%x, attr %d value changed indication",__FUNCTION__, 
                 omciMsg.msgId.meClass, omciMsg.msgId.meInstance, avcOmciSet);
            OMCI_SendIndicationToOlt(&omciMsg, 0);
            // Delay 10ms, just to help OLT easier
            OMCI_TaskDelay(10);

            ret = MIB_GetNext(tableIndex, pRow, MIB_GetTableEntrySize(tableIndex));
        }
    }
    
    return GOS_OK;
}


GOS_ERROR_CODE OMCI_OnActivateSw(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    return GOS_OK;
}


GOS_ERROR_CODE OMCI_OnCommitSw(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    PON_ME_ENTITY_ID    entityId = (PON_ME_ENTITY_ID)pNorOmciMsg->msgId.meInstance;
    MIB_TABLE_SWIMAGE_T mibImg;
    GOS_ERROR_CODE      ret;

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Receive commit SW Image msg, entity=0x%x", entityId);

    mibImg.EntityID = entityId;
    if (GOS_OK != MIB_Get(MIB_TABLE_SWIMAGE_INDEX, &mibImg, sizeof(MIB_TABLE_SWIMAGE_T)))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Commit SW Image: MIB_Get return failure, entity=0x%x", entityId);
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_UNKNOWN_ME_INST, TRUE);
        
        return GOS_FAIL;
    }

    if (!mibImg.Valid)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Commit SW Image: image is not valid, entity=0x%x", entityId);
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_PROC_ERROR, TRUE);
        
        return GOS_FAIL;
    }
    
    mibImg.Committed = TRUE;
    if (GOS_OK != MIB_Set(MIB_TABLE_SWIMAGE_INDEX, &mibImg, sizeof(MIB_TABLE_SWIMAGE_T)))
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Commit SW Image: MIB_Set return failure, entity=0x%x", entityId);
        OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_PROC_ERROR, TRUE);
        
        return GOS_FAIL;
    }

    ret = MIB_GetFirst(MIB_TABLE_SWIMAGE_INDEX, &mibImg, sizeof(MIB_TABLE_SWIMAGE_T));
    while(GOS_OK == ret)
    {
        if (entityId != mibImg.EntityID)
        {
            mibImg.Committed = FALSE;
            if (GOS_OK != MIB_Set(MIB_TABLE_SWIMAGE_INDEX, &mibImg, sizeof(MIB_TABLE_SWIMAGE_T)))
            {
                OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Commit SW Image: MIB_Set return failure, entity=0x%x", entityId);
                OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_PROC_ERROR, TRUE);
                return GOS_OK;
            }
        }
        ret = MIB_GetNext(MIB_TABLE_SWIMAGE_INDEX, &mibImg, sizeof(MIB_TABLE_SWIMAGE_T));
    }

    omci_SyncMibData();
    OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_SUCCESS, TRUE);
    
    return GOS_OK;
}


GOS_ERROR_CODE OMCI_OnReboot(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    OMCI_ResponseMsg(pNorOmciMsg, PON_OMCI_RSP_SUCCESS, TRUE);

    OMCI_TaskDelay(100);
    
    // Reboot system
    // system("reboot");

    return GOS_OK;
}

GOS_ERROR_CODE OMCI_OnMibUploadMsg(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    omci_FsmRun(gOmciMibUploadFsm, OMCI_MIB_UPLOAD_EVT_START, pNorOmciMsg);
    return GOS_OK;
}

GOS_ERROR_CODE OMCI_OnMibUploadNextMsg(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    omci_FsmRun(gOmciMibUploadFsm, OMCI_MIB_UPLOAD_EVT_GET, pNorOmciMsg);
    return GOS_OK;
}


UINT32 omci_MibUploadStartOnIdle(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    UINT16 maxSeq;

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Start MIB Upload");
    
    if (GOS_OK != MIB_CreatePublicTblSnapshot())
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"MIB_CreatePublicTblSnapshot returned failure");
        OMCI_ResponseMsg(pNorOmciMsg, (PON_OMCI_MSG_RESULT)0, TRUE);
        return OMCI_MIB_UPLOAD_STATE_IDLE;    
    }
    
    maxSeq = omci_GetMibUploadMaxSeqNum();

    // MIB is empty
    if (0 == maxSeq)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"MIB is empty");
        MIB_DeletePublicTblSnapshot();
        OMCI_ResponseMsg(pNorOmciMsg, (PON_OMCI_MSG_RESULT)0, TRUE);
        return OMCI_MIB_UPLOAD_STATE_IDLE;    
    }
    
    memset(&pNorOmciMsg->content[0], 0x00, PON_OMCI_MSG_CONTENT_LEN);
    GOS_SetUINT16((UINT16*)&pNorOmciMsg->content[0], GOS_Htons(maxSeq));
    OMCI_ResponseMsg(pNorOmciMsg, (PON_OMCI_MSG_RESULT)pNorOmciMsg->content[0], FALSE);

    gOmciMibUploadData.curChannelPri = pNorOmciMsg->priority;
    gOmciMibUploadData.doneSeqNum    = 0;
    gOmciMibUploadData.maxSeqNum     = maxSeq - 1;

    return OMCI_MIB_UPLOAD_STATE_RUN;
}


UINT16 omci_GetMibUploadMaxSeqNum(void)
{
    UINT16 numOfSubseqCom = 0;
    UINT16 i = 0;
    MIB_TABLE_INDEX tableIndex;

    for(i = 0, tableIndex = MIB_TABLE_FIRST_INDEX; i < MIB_TABLE_TOTAL_NUMBER; 
        i++, tableIndex = MIB_TABLE_NEXT_INDEX(tableIndex))
    {
        if (MIB_GetTableStdType(tableIndex) == PON_ME_STD_TYPE_PRI)
        {
            continue;
        }
        numOfSubseqCom += MIB_GetSnapshotEntryCount(tableIndex) * omci_GetSeqNumPerRow(tableIndex);
    }
    return numOfSubseqCom;
}

UINT16 omci_GetSeqNumPerRow(MIB_TABLE_INDEX tableIndex)
{
    UINT16 numOfAttr      = MIB_GetTableAttrNum(tableIndex);
    UINT16 aggreAttrLen   = 0;
    UINT16 numOfSubseqCom = 0;
    UINT16 attrLen;
    UINT16 i;
    
    // skip the first attribute (EntityID)
    MIB_ATTR_INDEX attrIndex = MIB_ATTR_NEXT_INDEX(MIB_ATTR_FIRST_INDEX);
    for (i = 1; i < numOfAttr; i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        PON_ME_ATTR_OPT_TYPE attrOption = MIB_GetAttrOptionType(tableIndex, attrIndex);

        if ((PON_ME_ATTR_OPT_NOT_SUPPORT == attrOption) || (PON_ME_ATTR_PRIVATE == attrOption))
        {
            continue;
        }
            
        attrLen = MIB_GetAttrLen(tableIndex, attrIndex);
        
        // if the size of this attribute is larger than the OMCI message content size, just skip it
        if (PON_OMCI_MIB_UPLOAD_NEXT_LIMIT < attrLen)
        {
            continue;
        }

        // this attribute will be in a new message
        if ((aggreAttrLen + attrLen) > PON_OMCI_MIB_UPLOAD_NEXT_LIMIT)
        {
            aggreAttrLen = attrLen;
            numOfSubseqCom++;
        }
        else
        {
            aggreAttrLen += attrLen;
        }
    }


    if (aggreAttrLen > 0)
    {
        numOfSubseqCom++;
    }

    // the ME has no other attributes exept entityID
    if (0 == numOfSubseqCom)
    {
        numOfSubseqCom = 1;
    }

    return numOfSubseqCom;
}


MIB_ATTRS_SET omci_GetMibUploadData(UINT16 seqNum, CHAR* pDataBuf, UINT32 size, MIB_TABLE_INDEX* pTableIndex, PON_ME_ENTITY_ID* pEntityId)
{
    UINT16 curTableSeqNum;
    
    UINT8*  rowBuff = omci_GetMibRowBuff();
    UINT8   attrBuff[MIB_TABLE_ATTR_MAX_SIZE];
    UINT16  i, j;
    UINT16  entityID;

    MIB_TABLE_INDEX tableIndex;
    MIB_ATTR_INDEX  attrIndex;
    UINT16          attrLen;
    MIB_ATTR_TYPE   attrType;
    MIB_ATTRS_SET   attrSet;
    UINT32          totalAttrSize;
    UINT32          maxSeqNumPerRow;
    

    GOS_ASSERT(pDataBuf && pTableIndex && pEntityId);
    for (i = 0, tableIndex = MIB_TABLE_FIRST_INDEX; i < MIB_TABLE_TOTAL_NUMBER; 
        i++, tableIndex = MIB_TABLE_NEXT_INDEX(tableIndex))
    {
        if (MIB_GetTableStdType(tableIndex) == PON_ME_STD_TYPE_PRI)
        {
            continue;
        }

        maxSeqNumPerRow = omci_GetSeqNumPerRow(tableIndex);

        curTableSeqNum = MIB_GetSnapshotEntryCount(tableIndex) * maxSeqNumPerRow;

        if (seqNum >= curTableSeqNum)
        {
            seqNum -= curTableSeqNum;
            continue;
        }

        // this table is selected
        if (GOS_OK == MIB_GetSnapshotFirst(tableIndex, rowBuff, MIB_GetTableEntrySize(tableIndex)))
        {
            do 
            {
                // this entity is selected
                if (seqNum < maxSeqNumPerRow)
                {
                    MIB_GetAttrFromBuf(tableIndex, MIB_ATTR_FIRST_INDEX, &entityID, rowBuff, sizeof(UINT16));
                    break;
                }
                seqNum -= maxSeqNumPerRow;
            }
            while (GOS_OK == MIB_GetSnapshotNext(tableIndex, rowBuff, MIB_GetTableEntrySize(tableIndex)));
        }

        // Table and entity are targeted
        *pTableIndex = tableIndex;
        *pEntityId = entityID;

        // Search the beginning attribute
        attrIndex = MIB_ATTR_NEXT_INDEX(MIB_ATTR_FIRST_INDEX);
        totalAttrSize = 0;
        for (j = 1; j < MIB_GetTableAttrNum(tableIndex); j++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
        {
            if (0 == seqNum)
            {
                break;
            }
            
            attrLen = MIB_GetAttrLen(tableIndex, attrIndex);
            if ((totalAttrSize + attrLen) > PON_OMCI_MIB_UPLOAD_NEXT_LIMIT)
            {
                seqNum--;
                totalAttrSize = 0;
                if (0 == seqNum)
                {
                    break;
                }
            }
            totalAttrSize += attrLen;
        }
        MIB_ClearAttrSet(&attrSet);
        // Attribute is selected
        for (; j < MIB_GetTableAttrNum(tableIndex); j++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
        {
            PON_ME_ATTR_OPT_TYPE attrOption = MIB_GetAttrOptionType(tableIndex, attrIndex);

            if ((PON_ME_ATTR_OPT_NOT_SUPPORT == attrOption) || (PON_ME_ATTR_PRIVATE == attrOption))
            {
                continue;
            }

            attrLen = MIB_GetAttrLen(tableIndex, attrIndex);
            // if the size of this attribute is larger than the OMCI message content size, just skip it
            if (PON_OMCI_MIB_UPLOAD_NEXT_LIMIT < attrLen)
            {
                continue;
            }

            // the left space in not enough to adopt this attribute, then stop
            if (attrLen > size)
            {
                break;
            }

            // OK, copy the attribute value into the buffer
            attrType = MIB_GetAttrDataType(tableIndex, attrIndex);
            switch(attrType)
            {
                case MIB_ATTR_TYPE_UINT16:
                {
                    UINT16 value;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, rowBuff, attrLen);
                    value = GOS_Htons(value);
                    memcpy(pDataBuf, &value, attrLen);
                    break;
                }
                
                case MIB_ATTR_TYPE_UINT32:
                {
                    UINT32 value;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, rowBuff, attrLen);
                    value = GOS_Htonl(value);
                    memcpy(pDataBuf, &value, attrLen);
                    break;
                }
                case MIB_ATTR_TYPE_UINT64:
                {
                    UINT64 value;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, rowBuff, sizeof(UINT64));
                    GOS_UINT64ToBuff(value, pDataBuf, attrLen);
                    break;
                }
                case MIB_ATTR_TYPE_UINT8:
                {
                    UINT8 value;
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, &value, rowBuff, attrLen);
                    memcpy(pDataBuf, &value, attrLen);
                    break;
                }
                case MIB_ATTR_TYPE_STR:
                {
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, attrBuff, rowBuff, attrLen + 1);
                    memcpy(pDataBuf, attrBuff, attrLen);
                    break;
                }
                case MIB_ATTR_TYPE_TABLE:
                {
                    MIB_GetAttrFromBuf(tableIndex, attrIndex, attrBuff, rowBuff, attrLen);
                    memcpy(pDataBuf, attrBuff, attrLen);
                    break;
                }
                default:
                    break;
            }
            MIB_SetAttrSet(&attrSet, attrIndex);
            pDataBuf += attrLen;
            size -= attrLen;
        }

        // Return, mission accomplished!
        break;
    }
    return attrSet;
}


UINT32 omci_MibUploadGetOnIdle(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    OMCI_ResponseMsg(pNorOmciMsg, (PON_OMCI_MSG_RESULT)0, TRUE);
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Get on IDLE state of MIB Upload");
    return OMCI_MIB_UPLOAD_STATE_IDLE;
}


UINT32 omci_MibUploadStartOnRun(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    //MIB_DeletePublicTblSnapshot();
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Terminate(interrupt) MIB Upload");
    
    return omci_MibUploadStartOnIdle(pNorOmciMsg);
}


MIB_ATTR_INDEX omci_GetFirstAttrIndex(PON_OMCI_ATTRS_SET attrsSet)
{
    MIB_ATTRS_SET  attrMibSet;
    MIB_ATTR_INDEX attrIndex;
    UINT32         i;
    
    omci_AttrsOmciSetToMibSet(&attrsSet, &attrMibSet);


    for (attrIndex = MIB_ATTR_NEXT_INDEX(MIB_ATTR_FIRST_INDEX), i = 0; i < sizeof(PON_OMCI_ATTRS_SET) * 8;
         i++, attrIndex = MIB_ATTR_NEXT_INDEX(attrIndex))
    {
        if (MIB_IsInAttrSet(&attrMibSet, attrIndex))
        {
            return attrIndex;
        }
    }

    return MIB_ATTR_UNKNOWN_INDEX;
}


UINT32 omci_MibUploadGetOnRun(PON_OMCI_MSG_H_T* pNorOmciMsg)
{
    UINT32             seqNum = GOS_Ntohs(GOS_GetUINT16((UINT16*)&pNorOmciMsg->content[0]));
    MIB_TABLE_INDEX    tableIndex;
    PON_ME_ENTITY_ID   entityID;
    MIB_ATTRS_SET      attrSet;
    PON_OMCI_ATTRS_SET omciSet;

    // Check channel
    if (gOmciMibUploadData.curChannelPri != pNorOmciMsg->priority)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Receive MibUploadNext on wrong channel: expected=%d, received=%d",
                  gOmciMibUploadData.curChannelPri, pNorOmciMsg->priority);
        OMCI_ResponseMsg(pNorOmciMsg, (PON_OMCI_MSG_RESULT)0, TRUE);
        return OMCI_MIB_UPLOAD_STATE_RUN;
    }

    // Check sequence
    if (seqNum > gOmciMibUploadData.maxSeqNum)
    {
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Received seqnum is out of range: seq=%d, max=%d",
                  seqNum, gOmciMibUploadData.maxSeqNum);
        OMCI_ResponseMsg(pNorOmciMsg, (PON_OMCI_MSG_RESULT)0, TRUE);
        return OMCI_MIB_UPLOAD_STATE_RUN;
    }

    // Get MIB Data
    attrSet = omci_GetMibUploadData(seqNum, (CHAR*)&(pNorOmciMsg->content[6]), PON_OMCI_MIB_UPLOAD_NEXT_LIMIT, &tableIndex, &entityID);
    
    OMCI_AttrsMibSetToOmciSet(&attrSet, &omciSet);
    GOS_SetUINT16((UINT16*)&pNorOmciMsg->content[0], GOS_Htons((UINT16)MIB_GetTableClassId(tableIndex)));
    GOS_SetUINT16((UINT16*)&pNorOmciMsg->content[2], GOS_Htons((UINT16)entityID));
    GOS_SetUINT16((UINT16*)&pNorOmciMsg->content[4], GOS_Htons((UINT16)omciSet));
    
    OMCI_ResponseMsg(pNorOmciMsg, (PON_OMCI_MSG_RESULT)pNorOmciMsg->content[0], FALSE);

    gOmciMibUploadData.doneSeqNum++;

    // done
    if (gOmciMibUploadData.doneSeqNum >= gOmciMibUploadData.maxSeqNum + 1)
    {
        MIB_DeletePublicTblSnapshot();
        OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Terminate(done) MIB Upload");

        return OMCI_MIB_UPLOAD_STATE_IDLE;
    }

    return OMCI_MIB_UPLOAD_STATE_RUN;
}


BOOL OMCI_CheckIsActionSupported(UINT16 meClassID, UINT8 type)
{
    MIB_TABLE_INDEX tableIndex;
    PON_ME_ACTION_TYPE actionType = 0;


    tableIndex = MIB_GetTableIndexByClassId(meClassID);
    
    switch(type)
    {
        case PON_OMCI_MSG_CREATE:
        {
            actionType = PON_ME_ACTION_CREATE;
            break;
        }
        case PON_OMCI_MSG_DELETE:
        {
            actionType = PON_ME_ACTION_DELETE;
            break;
        }
        case PON_OMCI_MSG_SET:
        {
            actionType = PON_ME_ACTION_SET;
            break;
        }
        case PON_OMCI_MSG_GET:
        {
            actionType = PON_ME_ACTION_GET;
            break;
        }
        case PON_OMCI_MSG_MIB_UPLOAD:
        {
            actionType = PON_ME_ACTION_MIB_UPLOAD;
            break;
        }
        case PON_OMCI_MSG_MIB_UPLOAD_NEXT:
        {
            actionType = PON_ME_ACTION_MIB_UPLOAD_NEXT;
            break;
        }
        case PON_OMCI_MSG_MIB_RESET:
        {
            actionType = PON_ME_ACTION_MIB_RESET;
            break;
        }
        case PON_OMCI_MSG_ACTIVATE_SW:
        {
            actionType = PON_ME_ACTION_ACT_SW;
            break;
        }
        case PON_OMCI_MSG_COMMIT_SW:
        {
            actionType = PON_ME_ACTION_COMMIT_SW;
            break;
        }
        case PON_OMCI_MSG_REBOOT:
        {
            actionType = PON_ME_ACTION_REBOOT;
            break;
        }
        case PON_OMCI_MSG_GET_NEXT:
        {
            actionType = PON_ME_ACTION_GET_NEXT;
            break;
        }
        default:
            return FALSE;
    }
    
    return MIB_TableSupportAction(tableIndex, actionType);
}

