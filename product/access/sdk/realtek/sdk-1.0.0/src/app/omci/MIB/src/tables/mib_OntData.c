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

MIB_TABLE_INFO_T gMibOntDataTableInfo;
MIB_ATTR_INFO_T  gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_ATTR_NUM];
MIB_TABLE_ONTDATA_T gMibOntDataDefRow;
MIB_TABLE_OPER_T    gMibOntDataOper;

GOS_ERROR_CODE OntDataDumpMib(void *pData)
{
	MIB_TABLE_ONTDATA_T *pOntData = (MIB_TABLE_ONTDATA_T*)pData;
	
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"%s","OntData");
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"EntityID: 0x%02x",pOntData->EntityID);		
	OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"MIBDataSync: %d",pOntData->MIBDataSync);	

	
	return GOS_OK;
}

GOS_ERROR_CODE OntDataDrvCfg(void* pOldRow,void* pNewRow,MIB_OPERA_TYPE  operationType)
{
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s: process end\n",__FUNCTION__);
	return GOS_OK;
}


GOS_ERROR_CODE OntData_Init(void)
{
    gMibOntDataTableInfo.Name = "OntData";
    gMibOntDataTableInfo.Desc = "Ont Data";
    gMibOntDataTableInfo.MaxEntry = (UINT32)(1);
    gMibOntDataTableInfo.ClassId = (UINT32)(2);
    gMibOntDataTableInfo.InitType = (UINT32)(PON_ME_INIT_TYPE_AUTO);
    gMibOntDataTableInfo.StdType = (UINT32)(PON_ME_STD_TYPE_STD);
    gMibOntDataTableInfo.ActionType = (UINT32)(PON_ME_ACTION_SET | PON_ME_ACTION_GET | PON_ME_ACTION_MIB_UPLOAD | PON_ME_ACTION_MIB_UPLOAD_NEXT | PON_ME_ACTION_MIB_RESET);
    gMibOntDataTableInfo.pAttributes = &(gMibOntDataAttrInfo[0]);

	gMibOntDataTableInfo.attrNum = MIB_TABLE_ONTDATA_ATTR_NUM;
	gMibOntDataTableInfo.entrySize = sizeof(MIB_TABLE_ONTDATA_T);
	gMibOntDataTableInfo.pDefaultRow = &gMibOntDataDefRow;


    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Name = "EntityID";
    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_MIBDATASYNC_INDEX - MIB_TABLE_FIRST_INDEX].Name = "MIBDataSync";

    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "Entity ID";
    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_MIBDATASYNC_INDEX - MIB_TABLE_FIRST_INDEX].Desc = "MIB data sync";

    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT16;
    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_MIBDATASYNC_INDEX - MIB_TABLE_FIRST_INDEX].DataType = MIB_ATTR_TYPE_UINT8;

    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].Len = 2;
    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_MIBDATASYNC_INDEX - MIB_TABLE_FIRST_INDEX].Len = 1;

    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = TRUE;
    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_MIBDATASYNC_INDEX - MIB_TABLE_FIRST_INDEX].IsIndex = FALSE;

    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;
    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_MIBDATASYNC_INDEX - MIB_TABLE_FIRST_INDEX].UsrAcc = MIB_ATTR_USR_READ_ONLY;

    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;
    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_MIBDATASYNC_INDEX - MIB_TABLE_FIRST_INDEX].MibSave = TRUE;

    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_HEX;
    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_MIBDATASYNC_INDEX - MIB_TABLE_FIRST_INDEX].OutStyle = MIB_ATTR_OUT_DEC;

    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ;
    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_MIBDATASYNC_INDEX - MIB_TABLE_FIRST_INDEX].OltAcc = PON_ME_OLT_READ | PON_ME_OLT_WRITE;

    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;
    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_MIBDATASYNC_INDEX - MIB_TABLE_FIRST_INDEX].AvcFlag = FALSE;

    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_ENTITYID_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;
    gMibOntDataAttrInfo[MIB_TABLE_ONTDATA_MIBDATASYNC_INDEX - MIB_TABLE_FIRST_INDEX].OptionType = PON_ME_ATTR_MANDATORY;


    memset(&(gMibOntDataDefRow.EntityID), 0x00, sizeof(gMibOntDataDefRow.EntityID));
    memset(&(gMibOntDataDefRow.MIBDataSync), 0x00, sizeof(gMibOntDataDefRow.MIBDataSync));

	gMibOntDataOper.meOperDrvCfg = OntDataDrvCfg;
	gMibOntDataOper.meOperConnCheck = NULL;
	gMibOntDataOper.meOperDump = OntDataDumpMib;
	gMibOntDataOper.meOperConnCfg = NULL;


	MIB_Register(MIB_TABLE_ONTDATA_INDEX,&gMibOntDataTableInfo,&gMibOntDataOper);

    return GOS_OK;
}


