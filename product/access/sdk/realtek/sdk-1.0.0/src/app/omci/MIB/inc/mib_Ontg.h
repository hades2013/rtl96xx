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


#ifndef __MIB_ONTG_TABLE_H__
#define __MIB_ONTG_TABLE_H__


/* Table Ontg attribute index */
#define MIB_TABLE_ONTG_ATTR_NUM (10)

/*ONT-G attribute index*/
enum {
	MIB_TABLE_ONTG_ENTITYID_INDEX=1,
	MIB_TABLE_ONTG_VID_INDEX,
	MIB_TABLE_ONTG_VERSION_INDEX,
	MIB_TABLE_ONTG_SERIALNUM_INDEX,
	MIB_TABLE_ONTG_TRAFFMGTOPT_INDEX,
	MIB_TABLE_ONTG_ATMCCOPT_INDEX,
	MIB_TABLE_ONTG_BATTERYBACK_INDEX,
	MIB_TABLE_ONTG_ADMINSTATE_INDEX,
	MIB_TABLE_ONTG_OPSTATE_INDEX,
	MIB_TABLE_ONTG_ONTSTATE_INDEX
};


/* Table Ontg attribute len, only string attrubutes have length definition */
#define MIB_TABLE_ONTG_VID_LEN (4)
#define MIB_TABLE_ONTG_VERSION_LEN (14)
#define MIB_TABLE_ONTG_SERIALNUM_LEN (8)

// Table Ontg entry stucture
typedef struct {
    UINT16 EntityID; // index 1
    CHAR   VID[MIB_TABLE_ONTG_VID_LEN+1];
    CHAR   Version[MIB_TABLE_ONTG_VERSION_LEN+1];
    CHAR   SerialNum[MIB_TABLE_ONTG_SERIALNUM_LEN+1];
    UINT8  TraffMgtOpt;
    UINT8  AtmCCOpt;
    UINT8  BatteryBack;
    UINT8  AdminState;
    UINT8  OpState;
    UINT8  OntState;
    UINT8  resv0[3]; // to align with 4 bytes
} MIB_TABLE_ONTG_T;


#endif
