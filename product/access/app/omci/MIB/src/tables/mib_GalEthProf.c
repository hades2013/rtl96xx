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

MIB_TABLE_INFO_T gMibGalEthProfTableInfo;
MIB_ATTR_INFO_T  gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_ATTR_NUM];
MIB_TABLE_GALETHPROF_T gMibGalEthProfDefRow;
MIB_TABLE_OPER_T gMibGalEthProfOper;


GOS_ERROR_CODE GalEthProfDumpMib(void *pData)
{
	MIB_TABLE_GALETHPROF_T *pGalEthProf = (MIB_TABLE_GALETHPROF_T*)pData;
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"%s", "GalEthProf");

	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"EntityId: 0x%02x", pGalEthProf->EntityId);
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"MaxGemPayloadSize: %d", pGalEthProf->MaxGemPayloadSize);

	return GOS_OK;
}

GOS_ERROR_CODE GalEthProfDrvCfg(MIB_TABLE_INDEX tableIdx, void* pOldRow, void* pNewRow, MIB_OPERA_TYPE operationType)
{
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: process end\n", __FUNCTION__);
	return GOS_OK;
}

GOS_ERROR_CODE GalEthProf_Init(void)
{
    gMibGalEthProfTableInfo.Name = "GalEthProf";
    gMibGalEthProfTableInfo.Desc = "GAL Ethernet profile";
    gMibGalEthProfTableInfo.MaxEntry = (UINT32)(128);
    gMibGalEthProfTableInfo.ClassId = (UINT32)(272);
    gMibGalEthProfTableInfo.InitType = (UINT32)(PON_ME_INIT_TYPE_BY_OLT);
    gMibGalEthProfTableInfo.StdType = (UINT32)(PON_ME_STD_TYPE_STD);
    gMibGalEthProfTableInfo.ActionType = (UINT32)(PON_ME_ACTION_CREATE | PON_ME_ACTION_DELETE | PON_ME_ACTION_SET | PON_ME_ACTION_GET);
    gMibGalEthProfTableInfo.pAttributes = &(gMibGalEthProfAttrInfo[0]);

	gMibGalEthProfTableInfo.attrNum = MIB_TABLE_GALETHPROF_ATTR_NUM;
	gMibGalEthProfTableInfo.entrySize = sizeof(MIB_TABLE_GALETHPROF_T);
	gMibGalEthProfTableInfo.pDefaultRow = &gMibGalEthProfDefRow;

    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Name = "EntityId";
    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_MAXGEMPAYLOADSIZE_INDEX - MIB_TABLE_FIRST_INDEX].Name = "MaxGemPayloadSize";

    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Entity ID";
    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_MAXGEMPAYLOADSIZE_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "maximun GEM payload size";

    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_MAXGEMPAYLOADSIZE_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT32;

    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_MAXGEMPAYLOADSIZE_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;

    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = TRUE;
    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_MAXGEMPAYLOADSIZE_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;

    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_MAXGEMPAYLOADSIZE_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_WRITE;

    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_MAXGEMPAYLOADSIZE_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;

    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_MAXGEMPAYLOADSIZE_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;

    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ;
    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_MAXGEMPAYLOADSIZE_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE;

    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_MAXGEMPAYLOADSIZE_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;

    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibGalEthProfAttrInfo[MIB_TABLE_GALETHPROF_MAXGEMPAYLOADSIZE_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;

    memset(&(gMibGalEthProfDefRow.EntityId), 0x00, sizeof(gMibGalEthProfDefRow.EntityId));
    memset(&(gMibGalEthProfDefRow.MaxGemPayloadSize), 0x00, sizeof(gMibGalEthProfDefRow.MaxGemPayloadSize));

    gMibGalEthProfOper.meOperDrvCfg = GalEthProfDrvCfg;
    gMibGalEthProfOper.meOperConnCheck = NULL;
    gMibGalEthProfOper.meOperDump = GalEthProfDumpMib;
	gMibGalEthProfOper.meOperConnCfg = NULL;

    MIB_Register(MIB_TABLE_GALETHPROF_INDEX, &gMibGalEthProfTableInfo, &gMibGalEthProfOper);

    return GOS_OK;
}

