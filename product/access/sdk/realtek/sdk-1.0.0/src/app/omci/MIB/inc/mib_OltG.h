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


#ifndef __MIB_OLTG_TABLE_H__
#define __MIB_OLTG_TABLE_H__

/* Table SWImage attribute len, only string attrubutes have length definition */
#define MIB_TABLE_EQUIPID_LEN (20)
#define MIB_TABLE_VERSION_LEN (14)
#define MIB_TABLE_TODINFO_LEN (14)

/* Table OltG attribute index */
#define MIB_TABLE_OLTG_ATTR_NUM (5)
#define MIB_TABLE_OLTG_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_OLTG_OLTVENDORID_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_OLTG_EQUIPID_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_OLTG_VERSION_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_OLTG_TODINFO_INDEX ((MIB_ATTR_INDEX)5)

/* Table OltG attribute len, only string attrubutes have length definition */
typedef struct {
	UINT16   EntityId;
	UINT8    resv0[2];
	UINT32   OltVendorId;
	CHAR     EquipId[MIB_TABLE_EQUIPID_LEN+1];
	CHAR     Version[MIB_TABLE_VERSION_LEN+1];
	CHAR     ToDInfo[MIB_TABLE_TODINFO_LEN+1];	
	UINT8    resv1[3];
} MIB_TABLE_OLTG_T;

#endif /* __MIB_OLTG_TABLE_H__ */
