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


#ifndef __MIB_THRESHOLDDATA1_TABLE_H__
#define __MIB_THRESHOLDDATA1_TABLE_H__

/* Table SWImage attribute len, only string attrubutes have length definition */

/* Table ThresholdData1 attribute index */
#define MIB_TABLE_THRESHOLDDATA1_ATTR_NUM (9)
#define MIB_TABLE_THRESHOLDDATA1_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_THRESHOLDDATA1_THRESHOLDVALUE1_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_THRESHOLDDATA1_THRESHOLDVALUE2_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_THRESHOLDDATA1_THRESHOLDVALUE3_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_THRESHOLDDATA1_THRESHOLDVALUE4_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_THRESHOLDDATA1_THRESHOLDVALUE5_INDEX ((MIB_ATTR_INDEX)6)
#define MIB_TABLE_THRESHOLDDATA1_THRESHOLDVALUE6_INDEX ((MIB_ATTR_INDEX)7)
#define MIB_TABLE_THRESHOLDDATA1_THRESHOLDVALUE7_INDEX ((MIB_ATTR_INDEX)8)

/* Table ThresholdData1 attribute len, only string attrubutes have length definition */
typedef struct {
	UINT16   EntityId;
	UINT8    resv0[2];
	UINT32   ThresholdValue1;
	UINT32   ThresholdValue2;
	UINT32   ThresholdValue3;
	UINT32   ThresholdValue4;
	UINT32   ThresholdValue5;
	UINT32   ThresholdValue6;
	UINT32   ThresholdValue7;
} MIB_TABLE_THRESHOLDDATA1_T;

#endif /* __MIB_THRESHOLDDATA1_TABLE_H__ */
