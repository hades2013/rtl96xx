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


#ifndef __MIB_TABLE_DEFS_H__
#define __MIB_TABLE_DEFS_H__


#ifdef __cplusplus
extern "C" {
#endif



GOS_ERROR_CODE    Ontg_Init(void);
MIB_TABLE_INFO_T* Ontg_GetTableInfoPtr(void);
void*             Ontg_GetDefaultRow(void);


GOS_ERROR_CODE    Ont2g_Init(void);
MIB_TABLE_INFO_T* Ont2g_GetTableInfoPtr(void);
void*             Ont2g_GetDefaultRow(void);


GOS_ERROR_CODE    OntData_Init(void);
MIB_TABLE_INFO_T* OntData_GetTableInfoPtr(void);
void*             OntData_GetDefaultRow(void);


GOS_ERROR_CODE    SWImage_Init(void);
MIB_TABLE_INFO_T* SWImage_GetTableInfoPtr(void);
void*             SWImage_GetDefaultRow(void);


GOS_ERROR_CODE    Cardholder_Init(void);
MIB_TABLE_INFO_T* Cardholder_GetTableInfoPtr(void);
void*             Cardholder_GetDefaultRow(void);


GOS_ERROR_CODE    CircuitPack_Init(void);
MIB_TABLE_INFO_T* CircuitPack_GetTableInfoPtr(void);
void*             CircuitPack_GetDefaultRow(void);


GOS_ERROR_CODE    Anig_Init(void);
MIB_TABLE_INFO_T* Anig_GetTableInfoPtr(void);
void*             Anig_GetDefaultRow(void);


GOS_ERROR_CODE    Tcont_Init(void);
MIB_TABLE_INFO_T* Tcont_GetTableInfoPtr(void);
void*             Tcont_GetDefaultRow(void);


GOS_ERROR_CODE    Scheduler_Init(void);
MIB_TABLE_INFO_T* Scheduler_GetTableInfoPtr(void);
void*             Scheduler_GetDefaultRow(void);


GOS_ERROR_CODE    EthUni_Init(void);
MIB_TABLE_INFO_T* EthUni_GetTableInfoPtr(void);
void*             EthUni_GetDefaultRow(void);


GOS_ERROR_CODE    PriQ_Init(void);
MIB_TABLE_INFO_T* PriQ_GetTableInfoPtr(void);
void*             PriQ_GetDefaultRow(void);


GOS_ERROR_CODE    GemPortCtp_Init(void);
MIB_TABLE_INFO_T* GemPortCtp_GetTableInfoPtr(void);
void*             GemPortCtp_GetDefaultRow(void);


GOS_ERROR_CODE    GemIwTp_Init(void);
MIB_TABLE_INFO_T* GemIwTp_GetTableInfoPtr(void);
void*             GemIwTp_GetDefaultRow(void);


GOS_ERROR_CODE    MultiGemIwTp_Init(void);
MIB_TABLE_INFO_T* MultiGemIwTp_GetTableInfoPtr(void);
void*             MultiGemIwTp_GetDefaultRow(void);


GOS_ERROR_CODE    MacBriServProf_Init(void);
MIB_TABLE_INFO_T* MacBriServProf_GetTableInfoPtr(void);
void*             MacBriServProf_GetDefaultRow(void);


GOS_ERROR_CODE    MacBriPortCfgData_Init(void);
MIB_TABLE_INFO_T* MacBriPortCfgData_GetTableInfoPtr(void);
void*             MacBriPortCfgData_GetDefaultRow(void);


GOS_ERROR_CODE    VlanTagOpCfgData_Init(void);
MIB_TABLE_INFO_T* VlanTagOpCfgData_GetTableInfoPtr(void);
void*             VlanTagOpCfgData_GetDefaultRow(void);


GOS_ERROR_CODE    VlanTagFilterData_Init(void);
MIB_TABLE_INFO_T* VlanTagFilterData_GetTableInfoPtr(void);
void*             VlanTagFilterData_GetDefaultRow(void);


GOS_ERROR_CODE    Map8021pServProf_Init(void);
MIB_TABLE_INFO_T* Map8021pServProf_GetTableInfoPtr(void);
void*             Map8021pServProf_GetDefaultRow(void);




#ifdef __cplusplus
}
#endif


#endif //__MIB_TABLE_DEFS_H__

