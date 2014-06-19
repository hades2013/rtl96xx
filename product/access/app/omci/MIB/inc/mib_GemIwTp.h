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


#ifndef __MIB_GEMIWTP_TABLE_H__
#define __MIB_GEMIWTP_TABLE_H__



/* Table GemIwTp attribute index */
#define MIB_TABLE_GEMIWTP_ATTR_NUM (9)
#define MIB_TABLE_GEMIWTP_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_GEMIWTP_GEMCTPPTR_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_GEMIWTP_IWOPT_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_GEMIWTP_SERVPROPTR_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_GEMIWTP_IWTPPTR_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_GEMIWTP_PPTPCOUNTER_INDEX ((MIB_ATTR_INDEX)6)
#define MIB_TABLE_GEMIWTP_OPSTATE_INDEX ((MIB_ATTR_INDEX)7)
#define MIB_TABLE_GEMIWTP_GALPROFPTR_INDEX ((MIB_ATTR_INDEX)8)
#define MIB_TABLE_GEMIWTP_GALLOOPBACKCFG_INDEX ((MIB_ATTR_INDEX)9)


/* Table GemIwTp attribute len, only string attrubutes have length definition */

// Table GemIwTp entry stucture
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
    UINT8  resv1[1]; // to align with 4 bytes
} MIB_TABLE_GEMIWTP_T;

enum{
	GEMIWTP_TP_TYPE_UN_TDM=0,
	GEMIWTP_TP_TYPE_MACBRIDGE_LAN,
	GEMIWTP_TP_TYPE_RESERVED,
	GEMIWTP_TP_TYPE_IPHOST,
	GEMIWTP_TP_TYPE_VIDEO,
	GEMIWTP_TP_TYPE_8021P
};


#endif
