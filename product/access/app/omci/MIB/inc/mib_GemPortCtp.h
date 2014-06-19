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


#ifndef __MIB_GEMPORTCTP_TABLE_H__
#define __MIB_GEMPORTCTP_TABLE_H__

/* Table SWImage attribute len, only string attrubutes have length definition */

/* Table GemPortCtp attribute index */
#define MIB_TABLE_GEMPORTCTP_ATTR_NUM (11)
#define MIB_TABLE_GEMPORTCTP_ENTITYID_INDEX ((MIB_ATTR_INDEX)1)
#define MIB_TABLE_GEMPORTCTP_PORTID_INDEX ((MIB_ATTR_INDEX)2)
#define MIB_TABLE_GEMPORTCTP_TCADAPTERPTR_INDEX ((MIB_ATTR_INDEX)3)
#define MIB_TABLE_GEMPORTCTP_DIRECTION_INDEX ((MIB_ATTR_INDEX)4)
#define MIB_TABLE_GEMPORTCTP_USTRAFFMGMTPTR_INDEX ((MIB_ATTR_INDEX)5)
#define MIB_TABLE_GEMPORTCTP_USTRAFFDESCPTR_INDEX ((MIB_ATTR_INDEX)6)
#define MIB_TABLE_GEMPORTCTP_UNICOUNTER_INDEX ((MIB_ATTR_INDEX)7)
#define MIB_TABLE_GEMPORTCTP_DSPRIQPTR_INDEX ((MIB_ATTR_INDEX)8)
#define MIB_TABLE_GEMPORTCTP_ENCRYPTIONSTATE_INDEX ((MIB_ATTR_INDEX)9)
#define MIB_TABLE_GEMPORTCTP_DSTRAFFDESCPTR_INDEX ((MIB_ATTR_INDEX)10)
#define MIB_TABLE_GEMPORTCTP_ENCRYPTIONKEYRING_INDEX ((MIB_ATTR_INDEX)11)

/* Table GemPortCtp attribute len, only string attrubutes have length definition */
typedef struct {
	UINT16   EntityID;
	UINT16   PortID;
	UINT16   TcAdapterPtr;
	UINT8    Direction;
	UINT8    resv0[1];
	UINT16   UsTraffMgmtPtr;
	UINT16   UsTraffDescPtr;
	UINT8    UniCounter;
	UINT8    resv1[1];
	UINT16   DsPriQPtr;
	UINT8    EncryptionState;
	UINT8    resv2[1];
	UINT16   DsTraffDescPtr;
	UINT8    EncryptionKeyRing;	
	UINT8    resv3[3];
} MIB_TABLE_GEMPORTCTP_T;

#endif /* __MIB_GEMPORTCTP_TABLE_H__ */
