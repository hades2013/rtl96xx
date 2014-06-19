
#include <errors.h>

#define S_OK		(UINT32)0
#define S_ERROR		(UINT32)(SECURITY_BASE)
#define S_NULL_PTR_ERROR	(UINT32)(SECURITY_BASE + 2)

#define FILTER_RULE1 64
#define FILTER_RULE12 96
#define FILTER_RULE2 128
#define FILTER_RULE3 192

#define CMASK_NUM 32
#define FILTER_MASK_NUM	9/*7*18=126 suitable*/
#define FILTER_RULE_NUM	126

#define CMASK_VBAS 0
#define CMASK_PPPOE 1
#define CMASK_DHCP 2
#define CMASK_IGMP 3
#define CMASK_SA 4
#define CMASK_DA 5
/*#define CMASK_IP_PTYPE 6
#define CMASK_SIP 7
#define CMASK_SPORT 8
#define CMASK_DIP 9
#define CMASK_DPORT 10*/

#define USER_MAC_NUM	8

#define UUI_NONE	0
#define UUI_VBAS	1
#define UUI_PPPOE	2
#define UUI_DHCP	3
#define UUI_QVLAN	4

typedef struct OPTION_82_s{
	UINT8 type;
	UINT8 length;
	UINT8 subtype;
	UINT8 sublength;
	UINT8 value[64];
}OPTION_82_t;

typedef struct IP_FILTER_s{
	UINT32 sip;
	UINT32 dip;
	UINT16 sport;
	UINT16 dport;
	UINT8  ipptype;
}IP_FILTER_t;

typedef struct QVLAN_s
{
	UINT8	DA[MAC_LEN];
	UINT8	SA[MAC_LEN];
	UINT16	sTPID;
	UINT16  sCOS:3;
	UINT16  sCFI:1;
	UINT16  svlanID:12;

	UINT16	cTPID;
	UINT16  cCOS:3;
	UINT16  cCFI:1;
	UINT16  cvlanID:12;

	UINT16  EtherType;
}QVLAN_t;

extern UINT32 uui_type_set(UINT32 type);
extern UINT32 uui_type_validate();
extern UINT32 ge_trunk_set(UINT32 type);
extern UINT32 ge_trunk_get();

extern UINT32 vbas_enable();
extern UINT32 vbas_disable();
extern UINT32 prelay_enable();
extern UINT32 prelay_disable();
extern UINT32 drelay_enable();
extern UINT32 drelay_disable();
extern UINT32 qvlan_enable(UINT32 svlanID);
extern UINT32 qvlan_disable();
extern UINT32 uui_type_get();

extern UINT32 ipmuxFloodEnable(UINT32 portID,UINT32 bVal);
extern UINT32 ipmuxFloodSendEnable(UINT32 portID,UINT32 bVal);
extern UINT32 ipmuxFloodReceiveEnable(UINT32 portID,UINT32 bVal);
extern UINT32 ipmuxMulticatReceiveEnable(UINT32 portID,UINT32 bVal);
extern UINT32 ipmuxBroadcastReceiveEnable(UINT32 portID,UINT32 bVal);

extern UINT32 ipmuxFloodSendEnableGet(UINT32 portID);
extern UINT32 ipmuxMulticatReceiveEnableGet(UINT32 portID);
extern UINT32 ipmuxFloodReceiveEnableGet(UINT32 portID);
extern UINT32 ipmuxBroadcastReceiveEnableGet(UINT32 portID);

extern UINT32 ipmuxMacPortBind(UINT32 portID,UINT8 *mac,UINT32 bVal,UINT32 num);
extern UINT32 ipmuxMacPortBindGet(UINT32 portID,UINT8 *mac);

extern UINT32 optionAdd(UINT32 portID, char *data);
extern UINT32 dhcpHandle(UINT32 portID, char *data, UINT32 length);
extern UINT32 pppoeHandle(UINT32 portID, char *data, UINT32 length);
extern UINT32 vbasPadiHandle(UINT32 portID, char *data, UINT32 length);
extern UINT32 vbasQueryHandle(UINT32 portID, char *data, UINT32 length);

extern UINT32 IPFilterSet(UINT32 ulIndex,IP_FILTER_t *pstFilter,UINT32 type);
extern UINT32 SAFilterSet(UINT32 ulIndex,UINT8 *mac,UINT32 type);
extern UINT32 DAFilterSet(UINT32 ulIndex,UINT8 *mac,UINT32 type);

extern UINT32 IPFilterGet(UINT32 ulIndex,IP_FILTER_t *pstFilter);
extern UINT32 SAFilterGet(UINT32 ulIndex,UINT8 *mac);
extern UINT32 DAFilterGet(UINT32 ulIndex,UINT8 *mac);

extern UINT32 IPFilterFree(UINT32 ulIndex);
extern UINT32 SAFilterFree(UINT32 ulIndex);
extern UINT32 DAFilterFree(UINT32 ulIndex);

extern UINT32 SAFilterRuleCheck(UINT32 ulIndex);
extern UINT32 DAFilterRuleCheck(UINT32 ulIndex);
extern UINT32 IPFilterRuleCheck(UINT32 ulIndex);
