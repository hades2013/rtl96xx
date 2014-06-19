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



#ifndef __MIB_CARDHOLDER_TABLE_H__
#define __MIB_CARDHOLDER_TABLE_H__


/* Table Cardholder attribute index */
#define MIB_TABLE_CARDHOLDER_ATTR_NUM (8)
#define MIB_TABLE_CARDHOLDER_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_CARDHOLDER_ACTUALTYPE_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_CARDHOLDER_EXPECTEDTYPE_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_CARDHOLDER_EXPECTEDPORTCOUNT_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_CARDHOLDER_EXPECTEDEQTID_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_CARDHOLDER_ACTUALEQTID_INDEX ((MIB_ATTR_INDEX)6)
#define MIB_TABLE_CARDHOLDER_PROTPRFPTR_INDEX ((MIB_ATTR_INDEX)7)
#define MIB_TABLE_CARDHOLDER_INVOKEPROTSWITCH_INDEX ((MIB_ATTR_INDEX)8)


/* Table Cardholder attribute len, only string attrubutes have length definition */
#define MIB_TABLE_CARDHOLDER_EXPECTEDEQTID_LEN (20)
#define MIB_TABLE_CARDHOLDER_ACTUALEQTID_LEN (20)

// Table Cardholder entry stucture
typedef struct {
    UINT16 EntityID; // index 1
    UINT8  ActualType;
    UINT8  ExpectedType;
    UINT8  ExpectedPortCount;
    CHAR   ExpectedEqtID[MIB_TABLE_CARDHOLDER_EXPECTEDEQTID_LEN+1];
    CHAR   ActualEqtID[MIB_TABLE_CARDHOLDER_ACTUALEQTID_LEN+1];
    UINT8  ProtPrfPtr;
    UINT8  InvokeProtSwitch;
    UINT8  resv0[3]; // to align with 4 bytes
} MIB_TABLE_CARDHOLDER_T;


#endif
