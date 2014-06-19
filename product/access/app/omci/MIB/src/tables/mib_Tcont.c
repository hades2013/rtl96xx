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

MIB_TABLE_INFO_T gMibTcontTableInfo;
MIB_ATTR_INFO_T  gMibTcontAttrInfo[MIB_TABLE_TCONT_ATTR_NUM];
MIB_TABLE_TCONT_T gMibTcontDefRow;
MIB_TABLE_OPER_T  gMibTcontTableOper;



GOS_ERROR_CODE TcontDumpMib(void *pData)
{
	MIB_TABLE_TCONT_T *pTcont = (MIB_TABLE_TCONT_T*)pData;
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"%s","Tcont");
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"EntityID: 0x%02x",pTcont->EntityID);	
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"AllocID: %d",pTcont->AllocID);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"ModeInd: %d",pTcont->ModeInd);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"Policy: %d",pTcont->Policy);
	return GOS_OK;
}

GOS_ERROR_CODE TcontDrvCfg(void* pOldRow,void* pNewRow,MIB_OPERA_TYPE  operationType)
{
	
	GOS_ERROR_CODE     ret;
    MIB_TABLE_ONTG_T   ontg;
    UINT16             tcontId;
    MIB_TABLE_TCONT_T* pNewTcont;
    MIB_TABLE_TCONT_T* pOldTcont;


    pNewTcont = (MIB_TABLE_TCONT_T *)pNewRow;
    pOldTcont = (MIB_TABLE_TCONT_T *)pOldRow;

    ontg.EntityID = 0;
    ret = MIB_Get(MIB_TABLE_ONTG_INDEX, &ontg, sizeof(MIB_TABLE_ONTG_T));
	
    if (ret == OK){
		
        switch (operationType){
        case MIB_ADD:
        {
            OMCI_LOG(OMCI_LOG_LEVEL_LOW,"T-Cont ---- > ADD: new allocId[0x%x]", pNewTcont->AllocID);
    
            if (0xFF != pNewTcont->AllocID)
            {
                ret = OMCI_wrapper_CreateTcont(pNewTcont->EntityID,pNewTcont->AllocID, (PON_US_TRAFF_MG_OPTION)ontg.TraffMgtOpt, &tcontId);
            }
            
            break;
        }
        case MIB_SET:
        {
            OMCI_LOG(OMCI_LOG_LEVEL_LOW,"T-Cont ---- > SET: old allocId[0x%x] new allocId[0x%x]", pOldTcont->AllocID, pNewTcont->AllocID);
    
            // When set an invalid allocId, it will remove the old allocId.
            if (0xFF == pNewTcont->AllocID && 0xFF != pOldTcont->AllocID)
            {
                ret = OMCI_wrapper_DeleteTcont(pNewTcont->EntityID,pOldTcont->AllocID);
            }
    
            // When set an valid allocId, it will add the old allocId.
            if (0xFF != pNewTcont->AllocID)
            {
                ret = OMCI_wrapper_CreateTcont(pNewTcont->EntityID,pNewTcont->AllocID, (PON_US_TRAFF_MG_OPTION)ontg.TraffMgtOpt, &tcontId);
	    
            }
    
            break;
        }
        default:
            return GOS_OK;
        }
    }
    return ret;
}



GOS_ERROR_CODE Tcont_Init(void)
{
    gMibTcontTableInfo.Name = "Tcont";
    gMibTcontTableInfo.Desc = "T-CONT";
    gMibTcontTableInfo.MaxEntry = (UINT32)(32);
    gMibTcontTableInfo.ClassId = (UINT32)(262);
    gMibTcontTableInfo.InitType = (UINT32)(PON_ME_INIT_TYPE_AUTO);
    gMibTcontTableInfo.StdType = (UINT32)(PON_ME_STD_TYPE_STD);
    gMibTcontTableInfo.ActionType = (UINT32)(PON_ME_ACTION_SET | PON_ME_ACTION_GET);
    gMibTcontTableInfo.pAttributes = &(gMibTcontAttrInfo[0]);


	gMibTcontTableInfo.attrNum = MIB_TABLE_TCONT_ATTR_NUM;
	gMibTcontTableInfo.entrySize = sizeof(MIB_TABLE_TCONT_T);
	gMibTcontTableInfo.pDefaultRow = &gMibTcontDefRow;


    gMibTcontAttrInfo[MIB_TABLE_TCONT_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Name = "EntityID";
    gMibTcontAttrInfo[MIB_TABLE_TCONT_ALLOCID_INDEX - MIB_TABLE_FIRST_INDEX].Name = "AllocID";
    gMibTcontAttrInfo[MIB_TABLE_TCONT_MODEIND_INDEX - MIB_TABLE_FIRST_INDEX].Name = "ModeInd";
    gMibTcontAttrInfo[MIB_TABLE_TCONT_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].Name = "Policy";

    gMibTcontAttrInfo[MIB_TABLE_TCONT_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Entity ID";
    gMibTcontAttrInfo[MIB_TABLE_TCONT_ALLOCID_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Alloc ID";
    gMibTcontAttrInfo[MIB_TABLE_TCONT_MODEIND_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Mode Indicator";
    gMibTcontAttrInfo[MIB_TABLE_TCONT_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Policy";

    gMibTcontAttrInfo[MIB_TABLE_TCONT_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_ALLOCID_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_MODEIND_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT8;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT8;

    gMibTcontAttrInfo[MIB_TABLE_TCONT_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_ALLOCID_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_MODEIND_INDEX - MIB_TABLE_FIRST_INDEX].Len = 1;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].Len = 1;

    gMibTcontAttrInfo[MIB_TABLE_TCONT_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = TRUE;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_ALLOCID_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_MODEIND_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;

    gMibTcontAttrInfo[MIB_TABLE_TCONT_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_ALLOCID_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_MODEIND_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;

    gMibTcontAttrInfo[MIB_TABLE_TCONT_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_ALLOCID_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_MODEIND_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;

    gMibTcontAttrInfo[MIB_TABLE_TCONT_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_ALLOCID_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_MODEIND_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;

    gMibTcontAttrInfo[MIB_TABLE_TCONT_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_ALLOCID_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_MODEIND_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ;

    gMibTcontAttrInfo[MIB_TABLE_TCONT_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_ALLOCID_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_MODEIND_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = TRUE;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;

    gMibTcontAttrInfo[MIB_TABLE_TCONT_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_ALLOCID_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_MODEIND_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibTcontAttrInfo[MIB_TABLE_TCONT_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;


    memset(&(gMibTcontDefRow.EntityID), 0x00, sizeof(gMibTcontDefRow.EntityID));
    gMibTcontDefRow.AllocID = 255;
    gMibTcontDefRow.ModeInd = 0;
    gMibTcontDefRow.Policy = 0;


	gMibTcontTableOper.meOperDrvCfg = TcontDrvCfg;
	gMibTcontTableOper.meOperConnCheck = NULL;
	gMibTcontTableOper.meOperDump = TcontDumpMib;
	gMibTcontTableOper.meOperConnCfg = NULL;

	MIB_Register(MIB_TABLE_TCONT_INDEX,&gMibTcontTableInfo,&gMibTcontTableOper);

    return GOS_OK;
}


