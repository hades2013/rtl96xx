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

#ifndef __MIB_ETHUNI_TABLE_H__
#define __MIB_ETHUNI_TABLE_H__



/* Table EthUni attribute index */
#define MIB_TABLE_ETHUNI_ATTR_NUM (16)
#define MIB_TABLE_ETHUNI_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_ETHUNI_EXPECTEDTYPE_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_ETHUNI_SENSEDTYPE_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_ETHUNI_AUTODECTECTCFG_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_ETHUNI_ETHLOOPBACKCFG_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_ETHUNI_ADMINSTATE_INDEX ((MIB_ATTR_INDEX)6)
#define MIB_TABLE_ETHUNI_OPSTATE_INDEX ((MIB_ATTR_INDEX)7)
#define MIB_TABLE_ETHUNI_DUPLEXIND_INDEX ((MIB_ATTR_INDEX)8)
#define MIB_TABLE_ETHUNI_MAXFRAMESIZE_INDEX ((MIB_ATTR_INDEX)9)
#define MIB_TABLE_ETHUNI_DTEORDCEIND_INDEX ((MIB_ATTR_INDEX)10)
#define MIB_TABLE_ETHUNI_PAUSETIME_INDEX ((MIB_ATTR_INDEX)11)
#define MIB_TABLE_ETHUNI_BRIDGEDORIPIND_INDEX ((MIB_ATTR_INDEX)12)
#define MIB_TABLE_ETHUNI_ARC_INDEX ((MIB_ATTR_INDEX)13)
#define MIB_TABLE_ETHUNI_ARCINTERVAL_INDEX ((MIB_ATTR_INDEX)14)
#define MIB_TABLE_ETHUNI_PPPOEFILTER_INDEX ((MIB_ATTR_INDEX)15)
#define MIB_TABLE_ETHUNI_POWERCONTROL_INDEX ((MIB_ATTR_INDEX)16)


/* Table EthUni attribute len, only string attrubutes have length definition */

// Table EthUni entry stucture
typedef struct {
    UINT16 EntityID; // index 1
    UINT8  ExpectedType;
    UINT8  SensedType;
    UINT8  AutoDectectCfg;
    UINT8  EthLoopbackCfg;
    UINT8  AdminState;
    UINT8  OpState;
    UINT8  DuplexInd;
    UINT8  resv0[1]; // to align with 2 bytes
    UINT16 MaxFrameSize;
    UINT8  DTEorDCEInd;
    UINT8  resv1[1]; // to align with 2 bytes
    UINT16 PauseTime;
    UINT8  BridgedorIPInd;
    UINT8  ARC;
    UINT8  ARCInterval;
    UINT8  PppoeFilter;
    UINT8  PowerControl;
    UINT8  resv2[3]; // to align with 4 bytes
} MIB_TABLE_ETHUNI_T;




#endif
