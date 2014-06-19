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


#ifndef __MIB_MACBRISERVPROF_TABLE_H__
#define __MIB_MACBRISERVPROF_TABLE_H__


/* Table MacBriServProf attribute index */
#define MIB_TABLE_MACBRISERVPROF_ATTR_NUM (10)
#define MIB_TABLE_MACBRISERVPROF_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_MACBRISERVPROF_SPANTREEIND_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_MACBRISERVPROF_LEARNINGIND_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_MACBRISERVPROF_ATMBRIIND_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_MACBRISERVPROF_PRIORITY_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_MACBRISERVPROF_MAXAGE_INDEX ((MIB_ATTR_INDEX)6)
#define MIB_TABLE_MACBRISERVPROF_HELLOTIME_INDEX ((MIB_ATTR_INDEX)7)
#define MIB_TABLE_MACBRISERVPROF_FORWARDDELAY_INDEX ((MIB_ATTR_INDEX)8)
#define MIB_TABLE_MACBRISERVPROF_DISCARDUNKNOW_INDEX ((MIB_ATTR_INDEX)9)
#define MIB_TABLE_MACBRISERVPROF_MACLEARNINGDEPTH_INDEX ((MIB_ATTR_INDEX)10)


/* Table MacBriServProf attribute len, only string attrubutes have length definition */

// Table MacBriServProf entry stucture
typedef struct {
    UINT16 EntityID; // index 1
    UINT8  SpanTreeInd;
    UINT8  LearningInd;
    UINT8  AtmBriInd;
    UINT8  resv0[1]; // to align with 2 bytes
    UINT16 Priority;
    UINT16 MaxAge;
    UINT16 HelloTime;
    UINT16 ForwardDelay;
    UINT8  DiscardUnknow;
    UINT8  MacLearningDepth;
} MIB_TABLE_MACBRISERVPROF_T;




enum {
	TP_TYPE_PPTP_ETHUNI=1,
	TP_TYPE_IWVCC=2,
	TP_TYPE_8021P=3,
	TP_TYPE_IPHOST=4,
	TP_TYPE_GEMIWTP=5,
	TP_TYPE_MULTIGEMIWTP=6,
	TP_TYPE_PPTP_XDSLUNI=7,
	TP_TYPE_PPTP_VDSLUNI=8,
	TP_TYPE_ETH_FLOWTP=9,
	TP_TYPE_PPTP_80211UNI=10
};


#endif
