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


#ifndef __MIB_GEMTRAFFICDESCRIPTOR_TABLE_H__
#define __MIB_GEMTRAFFICDESCRIPTOR_TABLE_H__

/* Table SWImage attribute len, only string attrubutes have length definition */

/* Table GemTrafficDescriptor attribute index */
#define MIB_TABLE_GEMTRAFFICDESCRIPTOR_ATTR_NUM (3)
#define MIB_TABLE_GEMTRAFFICDESCRIPTOR_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_GEMTRAFFICDESCRIPTOR_CIR_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_GEMTRAFFICDESCRIPTOR_PIR_INDEX ((MIB_ATTR_INDEX)3)

/* Table GemTrafficDescriptor attribute len, only string attrubutes have length definition */
typedef struct {
	UINT16   EntityId;
	UINT8    resv0[2];
	UINT32   CIR;
	UINT32   PIR;
} MIB_TABLE_GEMTRAFFICDESCRIPTOR_T;

#endif /* __MIB_GEMTRAFFICDESCRIPTOR_TABLE_H__ */
