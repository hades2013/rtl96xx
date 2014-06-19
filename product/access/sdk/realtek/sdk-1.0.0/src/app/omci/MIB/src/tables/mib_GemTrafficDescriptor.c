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

MIB_TABLE_INFO_T gMibGemTrafficDescriptorTableInfo;
MIB_ATTR_INFO_T  gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_ATTR_NUM];
MIB_TABLE_GEMTRAFFICDESCRIPTOR_T gMibGemTrafficDescriptorDefRow;
MIB_TABLE_OPER_T gMibGemTrafficDescriptorOper;


GOS_ERROR_CODE GemTrafficDescriptorDumpMib(void *pData)
{
	MIB_TABLE_GEMTRAFFICDESCRIPTOR_T *pGemTrafficDescriptor = (MIB_TABLE_GEMTRAFFICDESCRIPTOR_T*)pData;
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"%s", "GemTrafficDescriptor");

	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"EntityId: 0x%02x", pGemTrafficDescriptor->EntityId);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"CIR: %d", pGemTrafficDescriptor->CIR);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"PIR: %d", pGemTrafficDescriptor->PIR);

	return GOS_OK;
}

GOS_ERROR_CODE GemTrafficDescriptorDrvCfg(MIB_TABLE_INDEX tableIdx, void* pOldRow, void* pNewRow, MIB_OPERA_TYPE operationType)
{
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: process end\n", __FUNCTION__);
	return GOS_OK;
}

GOS_ERROR_CODE GemTrafficDescriptor_Init(void)
{
    gMibGemTrafficDescriptorTableInfo.Name = "GemTrafficDescriptor";
    gMibGemTrafficDescriptorTableInfo.Desc = "GEM Traffic Descriptor";
    gMibGemTrafficDescriptorTableInfo.MaxEntry = (UINT32)(32);
    gMibGemTrafficDescriptorTableInfo.ClassId = (UINT32)(280);
    gMibGemTrafficDescriptorTableInfo.InitType = (UINT32)(PON_ME_INIT_TYPE_BY_OLT);
    gMibGemTrafficDescriptorTableInfo.StdType = (UINT32)(PON_ME_STD_TYPE_STD);
    gMibGemTrafficDescriptorTableInfo.ActionType = (UINT32)(PON_ME_ACTION_CREATE | PON_ME_ACTION_DELETE | PON_ME_ACTION_SET | PON_ME_ACTION_GET);
    gMibGemTrafficDescriptorTableInfo.pAttributes = &(gMibGemTrafficDescriptorAttrInfo[0]);

	gMibGemTrafficDescriptorTableInfo.attrNum = MIB_TABLE_GEMTRAFFICDESCRIPTOR_ATTR_NUM;
	gMibGemTrafficDescriptorTableInfo.entrySize = sizeof(MIB_TABLE_GEMTRAFFICDESCRIPTOR_T);
	gMibGemTrafficDescriptorTableInfo.pDefaultRow = &gMibGemTrafficDescriptorDefRow;

    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Name = "EntityId";
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_CIR_INDEX - MIB_TABLE_FIRST_INDEX].Name = "CIR";
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_PIR_INDEX - MIB_TABLE_FIRST_INDEX].Name = "PIR";

    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Entity ID";
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_CIR_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "committed information rate";
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_PIR_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "specifies peak information rate";

    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_CIR_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_PIR_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;

    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_CIR_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_PIR_INDEX - MIB_TABLE_FIRST_INDEX].Len = 4;

    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = TRUE;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_CIR_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_PIR_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;

    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_CIR_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_PIR_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;

    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_CIR_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_PIR_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;

    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_CIR_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_PIR_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;

    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_CIR_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_PIR_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE;

    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_CIR_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_PIR_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;

    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_CIR_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_OPT_SUPPORT;
    gMibGemTrafficDescriptorAttrInfo[MIB_TABLE_GEMTRAFFICDESCRIPTOR_PIR_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_OPT_SUPPORT;

    memset(&(gMibGemTrafficDescriptorDefRow.EntityId), 0x00, sizeof(gMibGemTrafficDescriptorDefRow.EntityId));
    memset(&(gMibGemTrafficDescriptorDefRow.CIR), 0x00, sizeof(gMibGemTrafficDescriptorDefRow.CIR));
    memset(&(gMibGemTrafficDescriptorDefRow.PIR), 0x00, sizeof(gMibGemTrafficDescriptorDefRow.PIR));

    gMibGemTrafficDescriptorOper.meOperDrvCfg = GemTrafficDescriptorDrvCfg;
    gMibGemTrafficDescriptorOper.meOperConnCheck = NULL;
    gMibGemTrafficDescriptorOper.meOperDump= GemTrafficDescriptorDumpMib;
	gMibGemTrafficDescriptorOper.meOperConnCfg   = NULL;

    MIB_Register(MIB_TABLE_GEMTRAFFICDESCRIPTOR_INDEX, &gMibGemTrafficDescriptorTableInfo, &gMibGemTrafficDescriptorOper);

    return GOS_OK;
}

