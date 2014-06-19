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


#ifndef __MIB_EXTVLANTAGOPERCFGDATA_TABLE_H__
#define __MIB_EXTVLANTAGOPERCFGDATA_TABLE_H__

/* Table SWImage attribute len, only string attrubutes have length definition */
#define MIB_TABLE_RECEIVEDFRAMEVLANTAGGINGOPERTABLE_LEN (16)
#define MIB_TABLE_DSCPTOPBITMAPPING_LEN (24)

/* Table ExtVlanTagOperCfgData attribute index */
#define MIB_TABLE_EXTVLANTAGOPERCFGDATA_ATTR_NUM (9)
#define MIB_TABLE_EXTVLANTAGOPERCFGDATA_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATIONTYPE_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGOPERTABLEMAXSIZE_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_EXTVLANTAGOPERCFGDATA_INPUTTPID_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_EXTVLANTAGOPERCFGDATA_OUTPUTTPID_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSMODE_INDEX ((MIB_ATTR_INDEX)6)
#define MIB_TABLE_EXTVLANTAGOPERCFGDATA_RECEIVEDFRAMEVLANTAGGINGOPERTABLE_INDEX ((MIB_ATTR_INDEX)7)
#define MIB_TABLE_EXTVLANTAGOPERCFGDATA_ASSOCIATEDMEPOINT_INDEX ((MIB_ATTR_INDEX)8)
#define MIB_TABLE_EXTVLANTAGOPERCFGDATA_DSCPTOPBITMAPPING_INDEX ((MIB_ATTR_INDEX)9)

typedef struct
{
#ifdef OMCI_X86
    union
    {
        struct
        {
            UINT32 word1Pad : 12;
            UINT32 filterOuterTpId : 3;
            UINT32 filterOuterVid : 13;
            UINT32 filterOuterPri : 4;
        } bit;
        UINT32 val;
    } outerFilterWord;

    union
    {
        struct
        {
            UINT32 filterEthType : 4;
            UINT32 word2Pad : 8;
            UINT32 filterInnerTpId : 3;
            UINT32 filterInnerVid : 13;
            UINT32 filterInnerPri : 4;
        } bit;
        UINT32 val;
    } innerFilterWord;

    union
    {
        struct
        {
            UINT32 treatmentOuterTpId : 3;
            UINT32 treatmentOuterVid : 13;
            UINT32 treatmentOuterPri: 4;
            UINT32 word3Pad : 10;
            UINT32 treatment : 2;
        } bit;
        UINT32 val;
    } outerTreatmentWord;

    union
    {
        struct
        {
            UINT32 treatmentInnerTpId : 3;
            UINT32 treatmentInnerVid : 13;
            UINT32 treatmentInnerPri : 4;
            UINT32 word4Pad : 12;
        } bit;
        UINT32 val;
    } innerTreatmentWord;
#else
    union
    {
        struct
        {
            UINT32 filterOuterPri : 4;
            UINT32 filterOuterVid : 13;
            UINT32 filterOuterTpId : 3;
            UINT32 word1Pad : 12;
        } bit;
        UINT32 val;
    } outerFilterWord;

    union
    {
        struct
        {
            UINT32 filterInnerPri : 4;
            UINT32 filterInnerVid : 13;
            UINT32 filterInnerTpId : 3;
            UINT32 word2Pad : 8;
            UINT32 filterEthType : 4;
        } bit;
        UINT32 val;
    } innerFilterWord;

    union
    {
        struct
        {
            UINT32 treatment : 2;
            UINT32 word3Pad : 10;
            UINT32 treatmentOuterPri: 4;
            UINT32 treatmentOuterVid : 13;
            UINT32 treatmentOuterTpId : 3;
        } bit;
        UINT32 val;
    } outerTreatmentWord;

    union
    {
        struct
        {
            UINT32 word4Pad : 12;
            UINT32 treatmentInnerPri : 4;
            UINT32 treatmentInnerVid : 13;
            UINT32 treatmentInnerTpId : 3;
        } bit;
        UINT32 val;
    } innerTreatmentWord;
#endif /* Big endian. */
} omci_extvlan_row_entry_t;


typedef struct extvlanTableEntry_s{
	
	omci_extvlan_row_entry_t tableEntry;	
	LIST_ENTRY(extvlanTableEntry_s) entries;

}extvlanTableEntry_t;



/* Table ExtVlanTagOperCfgData attribute len, only string attrubutes have length definition */
typedef struct {
	UINT16   EntityId;
	UINT8    AssociationType;
	UINT8    resv0[1];
	UINT16   ReceivedFrameVlanTagOperTableMaxSize;
	UINT16   InputTPID;
	UINT16   OutputTPID;
	UINT8    DsMode;
	UINT8    ReceivedFrameVlanTaggingOperTable[MIB_TABLE_RECEIVEDFRAMEVLANTAGGINGOPERTABLE_LEN];
	UINT16   AssociatedMePoint;
	CHAR     DscpToPbitMapping[MIB_TABLE_DSCPTOPBITMAPPING_LEN+1];	
	UINT8    resv2[1];
	LIST_HEAD(extvlanHead,extvlanTableEntry_s) head;
} MIB_TABLE_EXTVLANTAGOPERCFGDATA_T;


enum {
	EXTVLAN_TP_TYPE_MACBRIDGEPORT = 0,
	EXTVLAN_TP_TYPE_8021P=1,
	EXTVLAN_TP_TYPE_PPTPETHUNI=2,
	EXTVLAN_TP_TYPE_IPHOST=3,
	EXTVLAN_TP_TYPE_PPTPXDSLUNI=4,
	EXTVLAN_TP_TYPE_GEMIWTP=5,
	EXTVLAN_TP_TYPE_MCASTGEMIWTP=6,
	EXTVLAN_TP_TYPE_PPTPMOCAUNI=7,
	EXTVLAN_TP_TYPE_PPTP80211UNI=8,
	EXTVLAN_TP_TYPE_ETHFLOWUNI=9,
	EXTVLAN_TP_TYPE_VEIP=10,
};




#endif /* __MIB_EXTVLANTAGOPERCFGDATA_TABLE_H__ */
