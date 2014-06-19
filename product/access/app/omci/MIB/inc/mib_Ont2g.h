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


#ifndef __MIB_ONTG2_TABLE_H__
#define __MIB_ONTG2_TABLE_H__


/* Table Ont2g attribute index */
#define MIB_TABLE_ONT2G_ATTR_NUM (11)
#define MIB_TABLE_ONT2G_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_ONT2G_EQTID_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_ONT2G_OMCCVER_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_ONT2G_VPCODE_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_ONT2G_SECCAPABILITY_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_ONT2G_SECMODE_INDEX ((MIB_ATTR_INDEX)6)
#define MIB_TABLE_ONT2G_NUMOFPRIQ_INDEX ((MIB_ATTR_INDEX)7)
#define MIB_TABLE_ONT2G_NUMOFSCHEDULER_INDEX ((MIB_ATTR_INDEX)8)
#define MIB_TABLE_ONT2G_MODE_INDEX ((MIB_ATTR_INDEX)9)
#define MIB_TABLE_ONT2G_NUMOFGEMPORT_INDEX ((MIB_ATTR_INDEX)10)
#define MIB_TABLE_ONT2G_SYSUPTIME_INDEX ((MIB_ATTR_INDEX)11)



/* Table Ont2g attribute len, only string attrubutes have length definition */
#define MIB_TABLE_ONT2G_EQTID_LEN (20)

// Table Ont2g entry stucture
typedef struct {
    UINT16 EntityID; // index 1
    CHAR   EqtID[MIB_TABLE_ONT2G_EQTID_LEN+1];
    UINT8  OMCCVer;
    UINT16 VPCode;
    UINT8  SecCapability;
    UINT8  SecMode;
    UINT16 NumOfPriQ;
    UINT8  NumOfScheduler;
    UINT8  Mode;
    UINT16 NumOfGemPort;
    UINT8  resv0[2]; // to align with 4 bytes
    UINT32 SysUpTime;
} MIB_TABLE_ONT2G_T;



#endif
