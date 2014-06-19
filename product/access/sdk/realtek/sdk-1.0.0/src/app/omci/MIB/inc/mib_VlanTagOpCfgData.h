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


#ifndef __MIB_VLANTAGOPCFGDATA_TABLE_H__
#define __MIB_VLANTAGOPCFGDATA_TABLE_H__


/* Table VlanTagOpCfgData attribute index */
#define MIB_TABLE_VLANTAGOPCFGDATA_ATTR_NUM (6)
#define MIB_TABLE_VLANTAGOPCFGDATA_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_VLANTAGOPCFGDATA_USTAGOPMODE_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_VLANTAGOPCFGDATA_USTAGTCI_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_VLANTAGOPCFGDATA_DSTAGOPMODE_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_VLANTAGOPCFGDATA_TYPE_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_VLANTAGOPCFGDATA_POINTER_INDEX ((MIB_ATTR_INDEX)6)


/* Table VlanTagOpCfgData attribute len, only string attrubutes have length definition */

// Table VlanTagOpCfgData entry stucture
typedef struct {
    UINT16 EntityID; // index 1
    UINT8  UsTagOpMode;
    UINT8  resv0[1]; // to align with 2 bytes
    UINT16 UsTagTci;
    UINT8  DsTagOpMode;
    UINT8  Type;
    UINT16 Pointer;
    UINT8  resv1[2]; // to align with 4 bytes
} MIB_TABLE_VLANTAGOPCFGDATA_T;



enum {
	VLANTAGOP_TP_TYPE_ETHUNI = 0,
	VLANTAGOP_TP_TYPE_IPHOST,
	VLANTAGOP_TP_TYPE_8021P,
	VLANTAGOP_TP_TYPE_MACBRIDGE,
	VLANTAGOP_TP_TYPE_PPTP_XDSL,
	VLANTAGOP_TP_TYPE_GEMIWTP,
	VLANTAGOP_TP_TYPE_MGEMIWTP,
	VLANTAGOP_TP_TYPE_PPTP_MOCA,
	VLANTAGOP_TP_TYPE_PPTP_80211,
	VLANTAGOP_TP_TYPE_ETHFLOW,
	VLANTAGOP_TP_TYPE_PPTP_ETHUNI
};


#endif
