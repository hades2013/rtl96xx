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


#ifndef __MIB_VLANTAGFILTERDATA_TABLE_H__
#define __MIB_VLANTAGFILTERDATA_TABLE_H__

/* Table VlanTagFilterData attribute index */
#define MIB_TABLE_VLANTAGFILTERDATA_ATTR_NUM (4)
#define MIB_TABLE_VLANTAGFILTERDATA_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_VLANTAGFILTERDATA_FWDOP_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_VLANTAGFILTERDATA_NUMOFENTRIES_INDEX ((MIB_ATTR_INDEX)4)


/* Table VlanTagFilterData attribute len, only string attrubutes have length definition */
#define MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_LEN (24)

// Table VlanTagFilterData entry stucture
typedef struct {
    UINT16 EntityID; // index 1
    CHAR   FilterTbl[MIB_TABLE_VLANTAGFILTERDATA_FILTERTBL_LEN+1];
    UINT8  FwdOp;
    UINT8  NumOfEntries;
    UINT8  resv0[3]; // to align with 4 bytes
} MIB_TABLE_VLANTAGFILTERDATA_T;

#endif
