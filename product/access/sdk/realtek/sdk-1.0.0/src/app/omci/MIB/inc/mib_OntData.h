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


#ifndef __MIB_ONTDATA_TABLE_H__
#define __MIB_ONTDATA_TABLE_H__


/* Table OntData attribute index */
#define MIB_TABLE_ONTDATA_ATTR_NUM (2)
#define MIB_TABLE_ONTDATA_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_ONTDATA_MIBDATASYNC_INDEX ((MIB_ATTR_INDEX)2)


/* Table OntData attribute len, only string attrubutes have length definition */

// Table OntData entry stucture
typedef struct {
    UINT16 EntityID; // index 1
    UINT8  MIBDataSync;
    UINT8  resv0[1]; // to align with 4 bytes
} MIB_TABLE_ONTDATA_T;



#endif
