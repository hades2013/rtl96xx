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

MIB_TABLE_INFO_T gMibSchedulerTableInfo;
MIB_ATTR_INFO_T  gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_ATTR_NUM];
MIB_TABLE_SCHEDULER_T gMibSchedulerDefRow;
MIB_TABLE_OPER_T	gMibSchedulerOper;


GOS_ERROR_CODE SchedulerDumpMib(void* pData)
{
	MIB_TABLE_SCHEDULER_T *pScheduler = (MIB_TABLE_SCHEDULER_T*)pData;
	
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"%s","Scheduler");
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"EntityID: 0x%02x",pScheduler->EntityID);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"Policy: %d",pScheduler->Policy);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"PriWeight: %d",pScheduler->PriWeight);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"SchedulerPtr: 0x%02x",pScheduler->SchedulerPtr);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"TcontPtr: 0x%02x",pScheduler->TcontPtr);

	return GOS_OK;
}


GOS_ERROR_CODE SchedulerDrvCfg(void* pOldRow,void* pNewRow,MIB_OPERA_TYPE  operationType)
{
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: process end\n",__FUNCTION__);
	return GOS_OK;
}


GOS_ERROR_CODE Scheduler_Init(void)
{
    gMibSchedulerTableInfo.Name = "Scheduler";
    gMibSchedulerTableInfo.Desc = "Traffic Scheduler-G";
    gMibSchedulerTableInfo.MaxEntry = (UINT32)(8);
    gMibSchedulerTableInfo.ClassId = (UINT32)(278);
    gMibSchedulerTableInfo.InitType = (UINT32)(PON_ME_INIT_TYPE_AUTO);
    gMibSchedulerTableInfo.StdType = (UINT32)(PON_ME_STD_TYPE_STD);
    gMibSchedulerTableInfo.ActionType = (UINT32)(PON_ME_ACTION_SET | PON_ME_ACTION_GET);
    gMibSchedulerTableInfo.pAttributes = &(gMibSchedulerAttrInfo[0]);


	gMibSchedulerTableInfo.attrNum = MIB_TABLE_SCHEDULER_ATTR_NUM;
	gMibSchedulerTableInfo.entrySize = sizeof(MIB_TABLE_SCHEDULER_T);
	gMibSchedulerTableInfo.pDefaultRow = &gMibSchedulerDefRow;


    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Name = "EntityID";
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_TCONTPTR_INDEX - MIB_TABLE_FIRST_INDEX].Name = "TcontPtr";
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_SCHEDULERPTR_INDEX - MIB_TABLE_FIRST_INDEX].Name = "SchedulerPtr";
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].Name = "Policy";
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_PRIWEIGHT_INDEX - MIB_TABLE_FIRST_INDEX].Name = "PriWeight";

    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Entity ID";
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_TCONTPTR_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "T-CONT Pointer";
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_SCHEDULERPTR_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Traffic Scheduler Pointer";
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Policy";
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_PRIWEIGHT_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Priority_Weight";

    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_TCONTPTR_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_SCHEDULERPTR_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT8;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_PRIWEIGHT_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT8;

    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_TCONTPTR_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_SCHEDULERPTR_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].Len = 1;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_PRIWEIGHT_INDEX - MIB_TABLE_FIRST_INDEX].Len = 1;

    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = TRUE;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_TCONTPTR_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_SCHEDULERPTR_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_PRIWEIGHT_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;

    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_TCONTPTR_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_SCHEDULERPTR_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_PRIWEIGHT_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;

    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_TCONTPTR_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_SCHEDULERPTR_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_PRIWEIGHT_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;

    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_TCONTPTR_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_SCHEDULERPTR_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_PRIWEIGHT_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;

    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_TCONTPTR_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_SCHEDULERPTR_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_PRIWEIGHT_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE;

    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_TCONTPTR_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_SCHEDULERPTR_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_PRIWEIGHT_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;

    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_TCONTPTR_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_SCHEDULERPTR_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_POLICY_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibSchedulerAttrInfo[MIB_TABLE_SCHEDULER_PRIWEIGHT_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;


    memset(&(gMibSchedulerDefRow.EntityID), 0x00, sizeof(gMibSchedulerDefRow.EntityID));
    memset(&(gMibSchedulerDefRow.TcontPtr), 0x00, sizeof(gMibSchedulerDefRow.TcontPtr));
    memset(&(gMibSchedulerDefRow.SchedulerPtr), 0x00, sizeof(gMibSchedulerDefRow.SchedulerPtr));
    memset(&(gMibSchedulerDefRow.Policy), 0x00, sizeof(gMibSchedulerDefRow.Policy));
    memset(&(gMibSchedulerDefRow.PriWeight), 0x00, sizeof(gMibSchedulerDefRow.PriWeight));


	gMibSchedulerOper.meOperDrvCfg = SchedulerDrvCfg;
	gMibSchedulerOper.meOperConnCheck = NULL;
	gMibSchedulerOper.meOperDump = SchedulerDumpMib;
	gMibSchedulerOper.meOperConnCfg = NULL;

	MIB_Register(MIB_TABLE_SCHEDULER_INDEX,&gMibSchedulerTableInfo,&gMibSchedulerOper);
	
    return GOS_OK;
}


