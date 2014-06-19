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


#ifndef __MIB_MCASTSUBMONITOR_TABLE_H__
#define __MIB_MCASTSUBMONITOR_TABLE_H__

/* Table SWImage attribute len, only string attrubutes have length definition */
#define MIB_TABLE_ACTIVEGROUPLISTTABLE_LEN (24)

/* Table McastSubMonitor attribute index */
#define MIB_TABLE_MCASTSUBMONITOR_ATTR_NUM (6)
#define MIB_TABLE_MCASTSUBMONITOR_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_MCASTSUBMONITOR_METYPE_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_MCASTSUBMONITOR_CURRMCASTBANDWIDTH_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_MCASTSUBMONITOR_JOINMSGCOUNTER_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_MCASTSUBMONITOR_BANDWIDTHEXCCOUNTER_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_MCASTSUBMONITOR_ACTIVEGROUPLISTTABLE_INDEX ((MIB_ATTR_INDEX)6)

/* Table McastSubMonitor attribute len, only string attrubutes have length definition */
typedef struct {
	UINT16   EntityId;
	UINT8    MeType;
	UINT8    resv0[1];
	UINT32   CurrMcastBandwidth;
	UINT32   JoinMsgCounter;
	UINT32   BandwidthExcCounter;
	CHAR     ActiveGroupListTable[MIB_TABLE_ACTIVEGROUPLISTTABLE_LEN+1];
	UINT8    resv1[3];
} MIB_TABLE_MCASTSUBMONITOR_T;

#endif /* __MIB_MCASTSUBMONITOR_TABLE_H__ */
