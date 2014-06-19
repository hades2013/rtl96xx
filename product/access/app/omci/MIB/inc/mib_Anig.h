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



#ifndef __MIB_ANIG_TABLE_H__
#define __MIB_ANIG_TABLE_H__


/* Table Anig attribute index */
#define MIB_TABLE_ANIG_ATTR_NUM (17)
#define MIB_TABLE_ANIG_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_ANIG_SRIND_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_ANIG_NUMOFTCONT_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_ANIG_GEMBLKLEN_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_ANIG_PBDBARPT_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_ANIG_ONUDBARPT_INDEX ((MIB_ATTR_INDEX)6)
#define MIB_TABLE_ANIG_SFTHRESHOLD_INDEX ((MIB_ATTR_INDEX)7)
#define MIB_TABLE_ANIG_SDTHRESHOLD_INDEX ((MIB_ATTR_INDEX)8)
#define MIB_TABLE_ANIG_ARC_INDEX ((MIB_ATTR_INDEX)9)
#define MIB_TABLE_ANIG_ARCINTERVAL_INDEX ((MIB_ATTR_INDEX)10)
#define MIB_TABLE_ANIG_OPTICALSIGNALLEVEL_INDEX ((MIB_ATTR_INDEX)11)
#define MIB_TABLE_ANIG_LOWOPTHRESHOLD_INDEX ((MIB_ATTR_INDEX)12)
#define MIB_TABLE_ANIG_UPPOPTHRESHOLD_INDEX ((MIB_ATTR_INDEX)13)
#define MIB_TABLE_ANIG_ONTRSPTIME_INDEX ((MIB_ATTR_INDEX)14)
#define MIB_TABLE_ANIG_TRANOPTICLEVEL_INDEX ((MIB_ATTR_INDEX)15)
#define MIB_TABLE_ANIG_LOWTRANPOWTHRESHOLD_INDEX ((MIB_ATTR_INDEX)16)
#define MIB_TABLE_ANIG_UPPTRANPOWTHRESHOLD_INDEX ((MIB_ATTR_INDEX)17)



/* Table Anig attribute len, only string attrubutes have length definition */

// Table Anig entry stucture
typedef struct {
    UINT16	EntityID; // index 1
    UINT8	SRInd;
    UINT8 	resv0[1]; // to align with 2 bytes
    UINT16	NumOfTcont;
    UINT16	GemBlkLen;
    UINT8	PbDbaRpt;
    UINT8	OnuDbaRpt;
    UINT8	SFThreshold;
    UINT8	SDThreshold;
    UINT8	ARC;
    UINT8	ARCInterval;
    UINT16	OpticalSignalLevel;
    UINT8	LowOpThreshold;
    UINT8	UppOpThreshold;
    UINT16	OntRspTime;
	UINT16	TranOpticLevel;
	UINT8	LowTranPowThreshold;
	UINT8	UppTranPowThreshold;
	
} MIB_TABLE_ANIG_T;

#endif
