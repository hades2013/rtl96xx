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



#ifndef __MIB_ETHPMHISTORYDATA2_TABLE_H__
#define __MIB_ETHPMHISTORYDATA2_TABLE_H__

/* Table SWImage attribute len, only string attrubutes have length definition */

/* Table EthPmHistoryData2 attribute index */
#define MIB_TABLE_ETHPMHISTORYDATA2_ATTR_NUM (4)
#define MIB_TABLE_ETHPMHISTORYDATA2_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_ETHPMHISTORYDATA2_INTERVALENDTIME_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_ETHPMHISTORYDATA2_THRESHOLDDATA12ID_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_ETHPMHISTORYDATA2_PPPOEFILTERFRAMECOUNTER_INDEX ((MIB_ATTR_INDEX)4)

/* Table EthPmHistoryData2 attribute len, only string attrubutes have length definition */
typedef struct {
	UINT16   EntityId;
	UINT8    IntervalEndTime;
	UINT8    resv0[1];
	UINT32   ThresholdData12Id;
	UINT32   PppoeFilterFramecounter;
} MIB_TABLE_ETHPMHISTORYDATA2_T;

#endif /* __MIB_ETHPMHISTORYDATA2_TABLE_H__ */
