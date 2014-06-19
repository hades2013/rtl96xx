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


#ifndef __MIB_TCONT_TABLE_H__
#define __MIB_TCONT_TABLE_H__


/* Table Tcont attribute index */
#define MIB_TABLE_TCONT_ATTR_NUM (4)
#define MIB_TABLE_TCONT_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_TCONT_ALLOCID_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_TCONT_MODEIND_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_TCONT_POLICY_INDEX ((MIB_ATTR_INDEX)4)


/* Table Tcont attribute len, only string attrubutes have length definition */

// Table Tcont entry stucture
typedef struct {
    UINT16 EntityID; // index 1
    UINT16 AllocID;
    UINT8  ModeInd;
    UINT8  Policy;
    UINT8  resv0[2]; // to align with 4 bytes
} MIB_TABLE_TCONT_T;


#endif
