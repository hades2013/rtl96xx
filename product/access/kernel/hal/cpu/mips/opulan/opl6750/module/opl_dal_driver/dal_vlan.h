#ifndef DAL_VLAN_H
#define DAL_VLAN_H


#include "opl_driver.h"
#include "opconn_api.h"


typedef enum VLAN_EGRESS_TYPE_S
{
    UNMODIFY = 0,
    UNTAG    = 1,
    TAG      = 2 
}VLAN_EGRESS_TYPE_E;

typedef enum
{
    UNMODIFY_EGRESS = 0,
    UNTAGGED_EGRESS,
    TAGGED_EGRESS,
    ADD_TAG
} PORT_EGRESS_MODE;
typedef enum
{
	DISABLE = 0,
	FALLBACK,
	CHECK,
	SECURE
} SW_DOT1Q_MODE;

#pragma pack(1)		/* pragma pack definition */

typedef struct QINQ_FlEXIBLE_VLAN_US_s {
  UINT16 svlan;
  UINT16 cvlan;
  UINT16 sPriority;
  UINT32 ruleID;
  UINT8 valid;
} QINQ_FlEXIBLE_VLAN_US_t;

typedef struct QINQ_TRANSPARENT_VLAN_s {
  UINT16 vlan;
  UINT32 ruleID;
  UINT8 valid;
} QINQ_TRANSPARENT_VLAN_t;

typedef struct QINQ_BASE_VLAN_s {
  UINT16 vlan;
  UINT16 insertPri;
  UINT16 remarkPri;
  UINT32 ruleID;
  UINT32 ruleRemarkId;
  UINT8 valid;
} QINQ_BASE_VLAN_t;

#pragma pack()		/* end of pragma pack definition */


OPL_STATUS dalTrunkEntryAdd(UINT8 portId, UINT32 vlanId);

OPL_STATUS dalTrunkEntryDel(UINT8 portId, UINT32 vlanId);

OPL_STATUS dalTrunkEntryValueGet(UINT8 portId, UINT16 entryIndex, UINT16 *pVlanId);

OPL_STATUS dalTrunkEntryNumGet(UINT8 portId, UINT32 *pNum);

OPL_STATUS dalTrunkEntryClear(UINT8 portId);


OPL_STATUS dalCreateVlanID(UINT16 vid);

OPL_STATUS dalFlushVlanAll(void);

OPL_STATUS dalSetLocalSwitch( UINT8  lport,UINT8  enable);


OPL_STATUS dalVlanQinQModeSet(unsigned char mode);

OPL_STATUS dalVlanQinQBaseAdd(unsigned short vlan,unsigned short remarkPri,unsigned short insertPri);

OPL_STATUS dalVlanQinQBaseDel(unsigned short vlan,unsigned short remarkPri,unsigned short insertPri);

OPL_STATUS dalVlanTransparentAdd(unsigned short vlan);

OPL_STATUS dalVlanTransparentDel(unsigned short vlan);

OPL_STATUS dalVlanQinQFlexibleAdd(unsigned short svlan, unsigned short cvlan,unsigned short sPriority);

OPL_STATUS dalVlanQinQFlexibleDel(unsigned short svlan, unsigned short cvlan,unsigned short sPriority);

OPL_STATUS dalQinQInit(void);


#endif 
