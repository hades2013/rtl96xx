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


#ifndef __MIB_GALETHPROF_TABLE_H__
#define __MIB_GALETHPROF_TABLE_H__

/* Table SWImage attribute len, only string attrubutes have length definition */

/* Table GalEthProf attribute index */
#define MIB_TABLE_GALETHPROF_ATTR_NUM (2)
#define MIB_TABLE_GALETHPROF_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_GALETHPROF_MAXGEMPAYLOADSIZE_INDEX ((MIB_ATTR_INDEX)2)

/* Table GalEthProf attribute len, only string attrubutes have length definition */
typedef struct {
	UINT16   EntityId;
	UINT8    resv0[2];
	UINT32   MaxGemPayloadSize;
} MIB_TABLE_GALETHPROF_T;

#endif /* __MIB_GALETHPROF_TABLE_H__ */
