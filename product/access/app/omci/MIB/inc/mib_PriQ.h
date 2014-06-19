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


#ifndef __MIB_PRIQ_TABLE_H__
#define __MIB_PRIQ_TABLE_H__

/* Table PriQ attribute index */
#define MIB_TABLE_PRIQ_ATTR_NUM (13)
#define MIB_TABLE_PRIQ_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_PRIQ_QCFGOPT_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_PRIQ_MAXQSIZE_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_PRIQ_ALLOCQSIZE_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_PRIQ_RESETINTERVAL_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_PRIQ_THRESHOLD_INDEX ((MIB_ATTR_INDEX)6)
#define MIB_TABLE_PRIQ_RELATEDPORT_INDEX ((MIB_ATTR_INDEX)7)
#define MIB_TABLE_PRIQ_SCHEDULERPTR_INDEX ((MIB_ATTR_INDEX)8)
#define MIB_TABLE_PRIQ_WEIGHT_INDEX ((MIB_ATTR_INDEX)9)
#define MIB_TABLE_PRIQ_BPOPERATION_INDEX ((MIB_ATTR_INDEX)10)
#define MIB_TABLE_PRIQ_BPTIME_INDEX ((MIB_ATTR_INDEX)11)
#define MIB_TABLE_PRIQ_BPOCCTHRESHOLD_INDEX ((MIB_ATTR_INDEX)12)
#define MIB_TABLE_PRIQ_BPCLRTHRESHOLD_INDEX ((MIB_ATTR_INDEX)13)


/* Table PriQ attribute len, only string attrubutes have length definition */

// Table PriQ entry stucture
typedef struct {
    UINT16 EntityID; // index 1
    UINT8  QCfgOpt;
    UINT8  resv0[1]; // to align with 2 bytes
    UINT16 MaxQSize;
    UINT16 AllocQSize;
    UINT16 ResetInterval;
    UINT16 Threshold;
    UINT32 RelatedPort;
    UINT16 SchedulerPtr;
    UINT8  Weight;
    UINT8  resv1[1]; // to align with 2 bytes
    UINT16 BPOperation;
    UINT8  resv2[2]; // to align with 4 bytes
    UINT32 BPTime;
    UINT16 BPOccThreshold;
    UINT16 BPClrThreshold;
} MIB_TABLE_PRIQ_T;


#endif
