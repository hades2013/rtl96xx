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


#ifndef __MIB_MCASTSUBCONFINFO_TABLE_H__
#define __MIB_MCASTSUBCONFINFO_TABLE_H__

/* Table SWImage attribute len, only string attrubutes have length definition */

/* Table McastSubConfInfo attribute index */
#define MIB_TABLE_MCASTSUBCONFINFO_ATTR_NUM (6)
#define MIB_TABLE_MCASTSUBCONFINFO_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_MCASTSUBCONFINFO_METYPE_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_MCASTSUBCONFINFO_MCASTOPERPROFPTR_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_MCASTSUBCONFINFO_MAXSIMGROUPS_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_MCASTSUBCONFINFO_MAXMCASTBANDWIDTH_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_MCASTSUBCONFINFO_BANDWIDTHENFORCEMENT_INDEX ((MIB_ATTR_INDEX)6)

/* Table McastSubConfInfo attribute len, only string attrubutes have length definition */
typedef struct {
	UINT16   EntityId;
	UINT8    MeType;
	UINT8    resv0[1];
	UINT16   McastOperProfPtr;
	UINT8    resv1[2];
	UINT16   MaxSimGroups;
	UINT8    resv2[2];
	UINT32   MaxMcastBandwidth;
	UINT8    BandwidthEnforcement;
	UINT8 	 resv3[3];
} MIB_TABLE_MCASTSUBCONFINFO_T;

#endif /* __MIB_MCASTSUBCONFINFO_TABLE_H__ */
