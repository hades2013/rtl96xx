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


#ifndef __MIB_MULTIGEMIWTP_TABLE_H__
#define __MIB_MULTIGEMIWTP_TABLE_H__


/* Table MultiGemIwTp attribute index */
#define MIB_TABLE_MULTIGEMIWTP_ATTR_NUM (10)
#define MIB_TABLE_MULTIGEMIWTP_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_MULTIGEMIWTP_GEMCTPPTR_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_MULTIGEMIWTP_IWOPT_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_MULTIGEMIWTP_SERVPROPTR_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_MULTIGEMIWTP_IWTPPTR_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_MULTIGEMIWTP_PPTPCOUNTER_INDEX ((MIB_ATTR_INDEX)6)
#define MIB_TABLE_MULTIGEMIWTP_OPSTATE_INDEX ((MIB_ATTR_INDEX)7)
#define MIB_TABLE_MULTIGEMIWTP_GALPROFPTR_INDEX ((MIB_ATTR_INDEX)8)
#define MIB_TABLE_MULTIGEMIWTP_GALLOOPBACKCFG_INDEX ((MIB_ATTR_INDEX)9)
#define MIB_TABLE_MULTIGEMIWTP_MCASTADDRTABLE_INDEX ((MIB_ATTR_INDEX)10)


/* Table MultiGemIwTp attribute len, only string attrubutes have length definition */
#define MIB_TABLE_MULTIGEMIWTP_MCASTADDRTABLE_LEN (24)

// Table MultiGemIwTp entry stucture
typedef struct {
    UINT16 EntityID; // index 1
    UINT16 GemCtpPtr;
    UINT8  IwOpt;
    UINT8  resv0[1]; // to align with 2 bytes
    UINT16 ServProPtr;
    UINT16 IwTpPtr;
    UINT8  PptpCounter;
    UINT8  OpState;
    UINT16 GalProfPtr;
    UINT8  GalLoopbackCfg;
    CHAR   MCastAddrTable[MIB_TABLE_MULTIGEMIWTP_MCASTADDRTABLE_LEN];
    UINT8  resv1[1]; // to align with 4 bytes
} MIB_TABLE_MULTIGEMIWTP_T;


#endif
