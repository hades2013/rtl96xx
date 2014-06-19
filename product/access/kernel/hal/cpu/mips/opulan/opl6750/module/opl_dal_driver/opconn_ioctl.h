#include <linux/ioctl.h>
#include "opl_driver.h"
#include "dal_lib.h"

#define __export_ioctl
#define __export_ioctl_internal
#define __argin
#define __argout
#define __arginout
#define __arginx(x,y)
#define __argoutx(x,y)
#define __arginoutx(x,y)

#pragma pack(1)		/* pragma pack definition */

typedef struct
{
	UINT8 mac[MAC_LENGTH];
} dalponmacidset_params_t;

typedef struct
{
	UINT8 mac[MAC_LENGTH];
} dalponmacidget_params_t;


typedef struct
{
	UINT32 channel;
	UINT32 mode;
} dalponsecuritymodeset_params_t;


typedef struct
{
	UINT32 channel;
	UINT32 mode;
} dalponsecuritymodeget_params_t;


typedef struct
{
	UINT32 channel;
	UINT32 mode;
} dalponsecurityaesmodeset_params_t;


typedef struct
{
	UINT32 channel;
	UINT32 mode;
} dalponsecurityaesmodeget_params_t;


typedef struct
{
	UINT8 keyIndex;
	UINT8 *keyBuff;
} dalponunicasttriplechurningkeyget_params_t;


typedef struct
{
	UINT8 keyIndex;
	UINT8 *keyBuff;
} dalponscbtriplechurningkeyset_params_t;


typedef struct
{
	UINT8 keyIndex;
	UINT8 *keyBuff;
	UINT8 mode;
} dalponunicastaeskeyget_params_t;


typedef struct
{
	UINT8 keyIndex;
	UINT8 *keyBuff;
	UINT8 mode;
} dalponscbaeskeyset_params_t;


typedef struct
{
	UINT32 cpRptStaMode;
	UINT32 cpMergeRptSendMode;
	UINT32 cpRptSendMode;
	UINT32 cpPktInitTimerCfg;
	UINT32 cpDeltaTxNormalNoFecTime;
	UINT32 cpTxMpcpNoFecTime;
} dalponmpcpdbaagentconfigallset_params_t;


typedef struct
{
	UINT32 cpRptStaMode;
	UINT32 cpMergeRptSendMode;
	UINT32 cpRptSendMode;
	UINT32 cpPktInitTimerCfg;
	UINT32 cpDeltaTxNormalNoFecTime;
	UINT32 cpTxMpcpNoFecTime;
} dalponmpcpdbaagentconfigallget_params_t;


typedef struct
{
	UINT8 cpTxDlyEn;
	UINT8 cpSyncEn;
	UINT16 cpRptsentDelt;
	UINT16 cpTxDly;
} dalpondelayconfigallset_params_t;


typedef struct
{
	UINT32 cpTxDlyEn;
	UINT32 cpSyncEn;
	UINT32 cpRptsentDelt;
	UINT32 cpTxDly;
} dalpondelayconfigallget_params_t;


typedef struct
{
	CLASSIFY_MASK_ENTRY_t *pMaskEntry;
	CLASSIFY_RULE_ENTRY_t *pRuleEntry;
	UINT16 pCruleID;
	opl_cls_hwtbl_t *pClshwtbl;
} dalclstblentryadd_params_t;


typedef struct
{
	CLASSIFY_MASK_ENTRY_t *pMaskEntry;
	CLASSIFY_RULE_ENTRY_t *pRuleEntry;
	UINT16 pCruleID;
} dalclstblentrydel_params_t;


typedef struct
{
	CLASSIFY_MASK_ENTRY_t *pMask;
	UINT8 maskID;
} dalclsmasktblwr_params_t;


typedef struct
{
	CLASSIFY_RULE_ENTRY_t *pRule;
	UINT16 ruleID;
} dalclsruletblwr_params_t;


typedef struct
{
	UINT16 address;
	UINT16 offset;
	UINT16 ruleId;
} dalclsaddrtblwr_params_t;


typedef struct
{
	CLASSIFY_MASK_ENTRY_t *pMask;
	UINT8 maskID;
} dalclsmaskhwwr_params_t;


typedef struct
{
	CLASSIFY_RULE_ENTRY_t *pRule;
	UINT16 ruleID;
} dalclsrulehwwr_params_t;


typedef struct
{
	UINT16 address;
	UINT16 offset;
	UINT16 ruleId;
} dalclsaddrhwwr_params_t;


typedef struct
{
	CLASSIFY_RULE_ENTRY_t *pRule;
	UINT16 ruleID;
	UINT8 tblType;
} dalclsruletblrd_params_t;


typedef struct
{
	UINT16 address;
	UINT16 offset;
	UINT16 pRuleId;
	UINT8 tblType;
} dalclsaddrtblrd_params_t;


typedef struct
{
	CLASSIFY_PARAMETER_t *pMaskRulePar;
	UINT16 pCruleID;
	opl_cls_hwtbl_t pCmaskID;
} dalclsentryset_params_t;


typedef struct
{
	CLASSIFY_PARAMETER_t *pRulePar;
	UINT16 pCruleID;
} dalclsentrymove_params_t;


typedef struct
{
	UINT16 classRuleID;
	UINT8 actValue;
} dalclstranactset_params_t;


typedef struct
{
	UINT16 classRuleID;
	UINT8 rmkactVal;
} dalclsrmkactset_params_t;


typedef struct
{
	UINT16 classRuleID;
	UINT8 cosValue;
} dalclsrmkcosset_params_t;


typedef struct
{
	UINT16 classRuleID;
	UINT8 iCosVal;
} dalclsrmkicosset_params_t;


typedef struct
{
	UINT16 classRuleID;
	UINT16 rmkVid;
} dalclsrmkvidset_params_t;

typedef struct
{
	UINT16 classRuleID;
	UINT8  rmkVidAct;
	UINT16 rmkVid;
} dalClsRmkVidActSet_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT8 portID;
	UINT8 eMask;
	UINT8 bEnable;
} dalclsegressportcfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT8 portID;
	UINT8 iMask;
	UINT8 bEnable;
} dalclsingressportcfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT8 actValue;
} dalclstranactcfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT8 rmkActVal;
} dalclsrmkactcfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT8 cosValue;
} dalclsrmkcoscfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT8 iCosValue;
} dalclsrmkicoscfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT16 rmkVid;
} dalclsrmkvidcfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT8 maskPrio;
} dalclsmaskpriocfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	INT8 pDestMac[MAC_LENGTH];
	INT8 pUsrMask[MAC_LENGTH];
	UINT8 bEnable;
} dalclsdestmacaddrcfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	INT8 pSrcMac[MAC_LENGTH];
	INT8 pUsrMask[MAC_LENGTH];
	UINT8 bEnable;
} dalclssrcmacaddrcfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT8 cosVal;
	UINT8 usrMask;
	UINT8 bEnable;
} dalclsfirstcoscfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT16 vlanID;
	UINT16 usrMask;
	UINT8 bEnable;
} dalclsfirstvidcfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT16 typeLen;
	UINT16 usrMask;
	UINT8 bEnable;
} dalclstypelencfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	INT8 pDestIp[IPVERSION4_LENGTH];
	INT8 pUsrMask[IPVERSION4_LENGTH];
	UINT8 bEnable;
} dalclsdestipcfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	INT8 pSrcIp[IPVERSION4_LENGTH];
	INT8 pUsrMask[IPVERSION4_LENGTH];
	UINT8 bEnable;
} dalclssrcipcfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT8 ipType;
	UINT8 usrMask;
	UINT8 bEnable;
} dalclsiptypecfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT8 ipTos;
	UINT8 usrMask;
	UINT8 bEnable;
} dalclsiptoscfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT16 L4DestPortID;
	UINT16 usrMask;
	UINT8 bEnable;
} dalclsl4destportcfg_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT16 L4SrcPortID;
	UINT16 usrMask;
	UINT8 bEnable;
} dalclsl4srcportcfg_params_t;


typedef struct
{
	UINT16 startId;
	UINT16 endId;
	UINT8 mode;
} dalclsruleentryshow_params_t;


typedef struct
{
	UINT16 startId;
	UINT16 endId;
	UINT8 mode;
} dalclsmaskentryshow_params_t;


typedef struct
{
	UINT16 startId;
	UINT16 endId;
	UINT8 mode;
} dalclsaddrentryshow_params_t;

typedef struct
{
	UINT16 usFirstItem;
	UINT16 usLastItem;
} dalclscfgshow_params_t;


typedef struct
{
	CLASSIFY_ITEM_t *pClassItemInfo;
	CLS_CONFIG_INFO_t *stClassCfg;
	UINT8 pktType;
} dalclsconvertpara_params_t;


typedef struct
{
	CLS_CONFIG_INFO_t *stClassCfg;
	UINT16 ruleId;
} dalclsrulectcadd_params_t;

typedef struct
{
	CLS_CONFIG_INFO_t *stClassCfg;
	UINT16 ruleId;
} dalvoipclsrulectcadd_params_t;


typedef struct
{
	CLS_CONFIG_INFO_t *stClassCfg;
	UINT16 ruleId;
} dalclsrulectcdel_params_t;

typedef struct
{
	UINT16 classItemID;
    UINT8 pktType;
    UINT8 ipVersion;
    UINT8 usrMask;
    UINT8 bEnable;
} dalClsIpVersionCfg_params_t;

typedef struct
{
	UINT16 classItemID;
    UINT8 pktType;
    UINT16 firsttpid;
    UINT16 usrMask;
    UINT8 bEnable;
} dalClsFirstTpidCfg_params_t;

typedef struct
{
	UINT8 portNum;
	UINT8 macAddress[MAC_LENGTH];
	UINT16 vlanIndex;
} dalarlmacadd_params_t;


typedef struct
{
	UINT8 portNum;
	UINT8 macAddress[MAC_LENGTH];
	UINT16 vlanIndex;
} dalarlmacdel_params_t;


typedef struct
{
	UINT8 macAddress[MAC_LENGTH];
	UINT16 vlanIndex;
} dalarlmacremove_params_t;


typedef struct
{
	UINT32 portID;
	UINT8 enable;
	UINT32 num;
} dalarlmacnumlimitset_params_t;


typedef struct
{
	UINT32 uiPortNum;
	UINT32 auiPortlist;
	UINT8 aucMacAddress[MAC_LENGTH];
	UINT16 usVlanId;
} dalarlmultiportmacadd_params_t;


typedef struct
{
	UINT32 uiPortNum;
	UINT32 auiPortlist;
	UINT8 aucMacAddress[MAC_LENGTH];
	UINT16 usVlanId;
} dalarlmultiportmacdel_params_t;


typedef struct
{
	UINT8 aucMacAddress[MAC_LENGTH];
	UINT16 usVlanId;
	FDB_MAC_ENTRY_INFO_S *pstMacEntryInfo;
	OPL_BOOL pbFind;
} dalarlmacentryfind_params_t;


typedef struct
{
	UINT32 puiStartMacEntryId;
	FDB_MAC_ENTRY_INFO_S *pstMacEntryInfo;
	OPL_BOOL pbFind;
} dalarlmacentrytraverse_params_t;


typedef struct
{
	INT32 lFd;
	UINT8 aucMacAddress[MAC_LENGTH];
	UINT16 usType;
	UINT32 uiPortNum;
	UINT32 auiPortlist;
} dalarlmacentryshowone_params_t;


typedef struct
{
	INT32 lFd;
	UINT16 usType;
	UINT32 uiPortNum;
	UINT32 auiPortlist;
} dalarlmacentryshowall_params_t;


typedef struct
{
	UINT8 portNum;
	UINT16 vlanId;
	UINT32 tag;
} dalmulticastvlanaddport_params_t;


typedef struct
{
	UINT8 portNum;
	UINT16 vlanId;
} dalmulticastvlandelport_params_t;


typedef struct
{
	UINT8 portNum;
	UINT8 enable;
} dalmulticastvlantagstripe_params_t;


typedef struct
{
	UINT8 portNum;
	UINT8 macAddr[MAC_LENGTH];
} dalmulticastarladdport_params_t;


typedef struct
{
	UINT8 portNum;
	UINT8 macAddr[MAC_LENGTH];
} dalmulticastarlportdel_params_t;


typedef struct
{
	UINT8 portNum;
	UINT16 vlanId;
	UINT8 macAddr[MAC_LENGTH];
} dalmulticastarlvlanadd_params_t;


typedef struct
{
	UINT8 portNum;
	UINT16 vlanId;
	UINT8 macAddr[MAC_LENGTH];
} dalmulticastarlvlandel_params_t;


typedef struct
{
	UINT8 portNum;
	UINT16 vlanId;
	UINT8 macAddr[MAC_LENGTH];
	OPL_BOOL fuzzy;
} dalmulticastportvlanmacadd_params_t;


typedef struct
{
	UINT8 portNum;
	UINT16 vlanId;
	UINT8 macAddr[MAC_LENGTH];
	OPL_BOOL fuzzy;
} dalmulticastportvlanmacdel_params_t;


typedef struct
{
	UINT8 portNum;
	UINT16 vlanId;
	UINT32 ip;
} dalmulticastportvlanipadd_params_t;


typedef struct
{
	UINT8 portNum;
	UINT16 vlanId;
	UINT32 ip;
} dalmulticastportvlanipdel_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 duplex;
} dalportspecificduplexread_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 state;
} dalphyspecificautonegread_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 portSpeed;
} dalportspecificspeedread_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 status;
} dalportstateget_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 portSpeed;
} dalportspeedset_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 portSpeed;
} dalportspeedget_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 duplex;
} dalportduplexset_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 duplex;
} dalportduplexget_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 enable;
} dalportflowcontrolset_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 enable;
} dalportflowcontrolget_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 enable;
} dalportloopbackset_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 enable;
} dalportuspolicingenableset_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 cir;
} dalportuspolicingcirset_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 cir;
} dalportuspolicingcirget_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 cbs;
} dalportuspolicingcbsset_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 cbs;
} dalportuspolicingcbsget_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 ebs;
} dalportuspolicingebsset_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 ebs;
} dalportuspolicingebsget_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 enable;
	UINT32 cir;
	UINT32 cbs;
	UINT32 ebs;
} dalportuspolicingset_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 enable;
	UINT32 cir;
	UINT32 cbs;
	UINT32 ebs;
} dalportuspolicingget_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 enable;
} dalportdspolicingenableset_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 enable;
} dalportdspolicingenableget_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 cir;
} dalportdspolicingcirset_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 cir;
} dalportdspolicingcirget_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 pir;
} dalportdspolicingpirset_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 pir;
} dalportdspolicingpirget_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 enable;
	UINT32 cir;
	UINT32 pir;
} dalportdspolicingset_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 enable;
	UINT32 cir;
	UINT32 pir;
} dalportdspolicingget_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 state;
} dalphyadminstateget_params_t;

typedef struct
{
	UINT8 portId;
	UINT32 state;
} dalphyadmincontrolset_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 state;
} dalphyautonegadminstateget_params_t;

typedef struct
{
	UINT8 portId;
	UINT32 state;
} dalphyautonegenableset_params_t;


typedef struct
{
	UINT32 ulPortId;
	UINT8 pucPortState;
} dalrstpportstateget_params_t;


typedef struct
{
	UINT32 ulPortId;
	UINT8 ucPortState;
} dalrstpportstateset_params_t;


typedef struct
{
	UINT32 ulPortid;
	BOOL_T bOnlyThisPort;
} dalrstpportmacflush_params_t;


typedef struct
{
	UINT32 stormLimitType;
	UINT32 streamType;
} dalstormclsruleadd_params_t;


typedef struct
{
	UINT32 stormLimitType;
	UINT32 streamType;
} dalstormclsruledel_params_t;


typedef struct
{
	UINT32 portId;
	UINT32 stormLimitType;
	UINT32 limitEnable;
} dalstormctrlframeset_params_t;


typedef struct
{
	UINT32 portId;
	UINT32 stormLimitType;
	UINT32 limitEnable;
} dalstormctrlframeget_params_t;


typedef struct
{
	UINT32 portId;
	UINT32 rateLimit;
} dalstormctrlrateset_params_t;


typedef struct
{
	UINT32 portId;
	UINT32 rateLimit;
} dalstormctrlrateget_params_t;


typedef struct
{
	UINT8 mode;
	UINT32 portId;
} dalvttmodeset_params_t;


typedef struct
{
	UINT8 mode;
	UINT32 portId;
} dalvttmodeget_params_t;


typedef struct
{
	UINT16 tpid;
	UINT32 portId;
} dalvtttpidset_params_t;


typedef struct
{
	UINT16 tpid;
	UINT32 portId;
} dalvtttpidget_params_t;


typedef struct
{
	UINT16 pvid;
	UINT32 portId;
} dalvttvidset_params_t;


typedef struct
{
	UINT16 pvid;
	UINT32 portId;
} dalvttvidget_params_t;


typedef struct
{
	UINT8 etherPri;
	UINT32 portId;
} dalvttdefaultpriset_params_t;


typedef struct
{
	UINT8 etherPri;
	UINT32 portId;
} dalvttdefaultpriget_params_t;


typedef struct
{
	UINT8 pStrip;
	UINT8 gStrip;
	UINT32 cVlan;
	UINT32 sVlan;
	UINT32 portId;
	UINT32 pEntryIndex;
	UINT8 type;
} dalvtttagentryadd_params_t;


typedef struct
{
	UINT8 ValidRecordIndex;
	UINT32 cVlan;
	UINT32 sVlan;
	UINT32 portId;
} dalvtttagentryget_params_t;


typedef struct
{
	UINT8 pStrip;
	UINT8 gStrip;
	UINT32 cVlan;
	UINT32 sVlan;
	UINT32 portId;
	UINT32 pEntryIndex;
	UINT8 type;
} dalvtttagentrydel_params_t;


typedef struct
{
	UINT8 numOfRecords;
	UINT32 portId;
} dalvttnumofrecordget_params_t;


typedef struct
{
	UINT8 pStrip;
	UINT8 gStrip;
	UINT32 cVlan;
	UINT32 sVlan;
	UINT32 portId;
} dalvttmulticastentryadd_params_t;


typedef struct
{
	UINT8 pStrip;
	UINT8 gStrip;
	UINT32 cVlan;
	UINT32 sVlan;
	UINT32 portId;
} dalvttmulticastentrydel_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 enable;
} dalvttmulticaststripenable_params_t;

typedef struct
{
	UINT8 stream;
	UINT8 enable;
} tmgeshaperenablehwwrite_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 enable;
} tmgeshaperenablehwread_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT8 enable;
} tmqueueshaperenablehwwrite_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT8 enable;
} tmqueueshaperenablehwread_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT8 priority;
} tmqueueprihwwrite_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT8 priority;
} tmqueueprihwread_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT32 weight;
} tmqueueweighthwwrite_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT32 weight;
} tmqueueweighthwread_params_t;


typedef struct
{
	UINT8 stream;
	UINT32 geCir;
} tmgecirhwwrite_params_t;


typedef struct
{
	UINT8 stream;
	UINT32 geCir;
} tmgecirhwread_params_t;


typedef struct
{
	UINT8 stream;
	UINT32 geCbs;
} tmgecbshwwrite_params_t;


typedef struct
{
	UINT8 stream;
	UINT32 geCbs;
} tmgecbshwread_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 cos;
	UINT8 icos;
} tmcosicosmaphwwrite_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 cos;
	UINT8 icos;
} tmcosicosmaphwread_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT32 cbs;
} tmqueuecbshwwrite_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT32 cbs;
} tmqueuecbshwread_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT32 cir;
} tmqueuecirhwwrite_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT32 cir;
} tmqueuecirhwread_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 queueId;
	UINT8 type;
	UINT32 value;
} tmwredcfgset_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 queueId;
	UINT32 enable;
} tmwredenablehwwrite_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 queueId;
	UINT32 enable;
} tmwredenablehwread_params_t;


typedef struct
{
	UINT32 entryId;
	UINT32 cir;
	UINT32 cbs;
} tmusshapertabdirecthwwrite_params_t;


typedef struct
{
	UINT32 entryId;
	UINT32 weight;
	UINT32 minth;
	UINT32 maxth;
} tmuswredtabdirecthwwrite_params_t;


typedef struct
{
	UINT32 entryId;
	UINT8  c1;
	UINT32 weight;
	UINT32 minth;
	UINT32 maxth;
} tmdswredtabdirecthwwrite_params_t;


typedef struct
{
	UINT32 entryId;
	UINT32 cir;
	UINT32 cbs;
} tmdsshapertabdirecthwwrite_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT16 maxCellNum;
} tmqueuemaxcellnumhwwrite_params_t;


typedef struct
{
	UINT8 stream;
	UINT8 queueId;
	UINT8 type;
	UINT32 value;
} tmwredcfgget_params_t;

typedef struct
{
	UINT32 entryId;
	UINT8 c1;
	UINT32 weight;
	UINT32 minth;
	UINT32 maxth;
} tmMaxpUsWredTabDirectHwWrite_params_t;

typedef struct
{
	UINT32 entryId;
	UINT8 c1;
	UINT32 weight;
	UINT32 minth;
	UINT32 maxth;
} tmMaxpDsWredTabDirectHwWrite_params_t;

typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT32 enable;
} tmqueueshapercirenablehwwrite_params_t;

typedef struct
{
	UINT32 entryId;
	UINT32 pir;
	UINT32 pbs;
} tmusshapertabpirdirecthwwrite_params_t;

typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT32 enable;
} tmqueueshaperpirenablehwwrite_params_t;

typedef struct
{
	UINT32 entryId;
	UINT32 cir;
	UINT32 cbs;
} tmusshapertabcirdirecthwwrite_params_t;

typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT16 maxPktNum;
} tmqueuemaxpktNumhwwrite_params_t;

typedef struct
{
	UINT8 stream;
	UINT8 Qid;
	UINT32 numOfCell;
} tmcellnuminqueuehwread_params_t;


typedef struct
{
	UINT32 regAddr;
	UINT32 regVal;
} oplregread_params_t;


typedef struct
{
	UINT32 regAddr;
	UINT32 regVal;
} oplregwrite_params_t;

typedef struct
{
	UINT32 regAddr;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	UINT32 data0;
} oplregfieldread_params_t;


typedef struct
{
	UINT32 regAddr;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	UINT32 data0;
} oplregfieldwrite_params_t;

typedef struct
{
	UINT32 region;
	UINT32 startId;
	UINT32 num;
	UINT32 *pVal;
} opltabread_params_t;


typedef struct
{
	UINT32 region;
	UINT32 startId;
	UINT32 num;
	UINT32 *pVal;
} opltabwrite_params_t;


typedef struct
{
	UINT8 deviceAddr;
	UINT8 regAddr;
	UINT16 data0;
} mdioregisterread_params_t;


typedef struct
{
	UINT8 deviceAddr;
	UINT8 regAddr;
	UINT16 data0;
} mdioregisterwrite_params_t;

/* Begin Added*/
typedef struct
{
	UINT32 deviceAddr;
	UINT32 regAddr;
	UINT32 data0;
} swmdioregisterread_params_t;


typedef struct
{
	UINT32 deviceAddr;
	UINT32 regAddr;
	UINT32 data0;
} swmdioregisterwrite_params_t;

typedef struct
{
	UINT32 deviceAddr;
    UINT32 phyid;
	UINT32 regAddr;
	UINT32 data0;
} swmdiophyregread_params_t;


typedef struct
{
	UINT32 deviceAddr;
    UINT32 phyid;
	UINT32 regAddr;
	UINT32 data0;
} swmdiophyregwrite_params_t;
/* End   Added of porting */

/* Begin Added of qinq */
typedef struct
{
    UINT8 pon_strip_en;
	UINT8 ge_strip_en;
	UINT16 c_vid;
	UINT16 s_vid;
} vttentrytag_params_t;

typedef struct
{
    UINT16 usVid;
    UP_DOWN_STREAM_e enDirect;
} qinqclfset_params_t;

typedef struct
{
    UINT32 uiPvid;
	UINT32 uiPriority;
} qinqpvidset_params_t;
/* End   Added of qinq */

typedef struct
{
	UINT8 deviceAddr;
	UINT8 regAddr;
	UINT8 offset;
	UINT8 width;
	UINT16 data0;
} mdioregisterfieldread_params_t;


typedef struct
{
	UINT8 deviceAddr;
	UINT8 regAddr;
	UINT8 offset;
	UINT8 width;
	UINT16 data0;
} mdioregisterfieldwrite_params_t;

typedef struct
{	
	UINT8 type;
	UINT32 value;
} chipattrget_params_t;

typedef struct
{
	UINT16 egressPort;
	UINT16 targetVlan;
	UINT16 aggedVlan;
	UINT8 aggedMac[MAC_LENGTH];
} dalvttvlanaggdel_params_t;


typedef struct
{
	UINT16 portId;
	UINT16 sVlan;
	UINT16 cVlan;
	UINT8 aggedMac[MAC_LENGTH];
	UINT16 ruleId;
} dalvttvlanaggadd_params_t;



typedef struct
{
	int i2cMode;
	int intMode;
	UINT8 slvAddr;
	int nRate;
} i2cinit_params_t;


typedef struct
{
	UINT8 slvAddr;
	UINT8 staRegAddr;
	UINT32 nByte;
	UINT8 pDataBuf[32];
} i2creadrandom_params_t;


typedef struct
{
	opl_acl_list_t *oamcls;
	UINT16 newmask;
} dalonuclsrulectcchange_params_t;


typedef struct
{
	CLS_CONFIG_INFO_t *stClassCfg;
	UINT16 ruleId;
} dalclacfgcheck_params_t;


typedef struct
{
	UINT8 portId;
	UINT16 pvid;
} dalportpvidset_params_t;


typedef struct
{
	UINT8 portId;
	UINT16 pvid;
} dalportpvidget_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 mode;
} dalportvlanmodeset_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 mode;
} dalportvlanmodeget_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 defaultPri;
} dalportdefaultpriset_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 vlanId;
} daltrunkentryadd_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 vlanId;
} daltrunkentrydel_params_t;


typedef struct
{
	UINT8 portId;
	UINT16 entryIndex;
	UINT16 pVlanId;
} daltrunkentryvalueget_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 pNum;
} daltrunkentrynumget_params_t;


typedef struct
{
	UINT8 lport;
	UINT8 enable;
} dalsetlocalswitch_params_t;


typedef struct
{
	UINT16 classItemID;
	UINT8 pktType;
	UINT8 rmkActVal;
} dalclsrmkactget_params_t;


typedef struct
{
	UINT8 qSetNum;
	UINT8 qNum;
	UINT16 thresholdVal;
} daldbaqueuethresholdset_params_t;


typedef struct
{
	UINT8 qSetNum;
	UINT8 qNum;
	UINT16 thresholdVal;
} daldbaqueuethresholdget_params_t;


typedef struct
{
	UINT32 uniorscb;
	UINT32 enable;
} dalonuponloopbackconfig_params_t;


typedef struct
{
	UINT8 macNum;
	UINT8 icos;
	UINT8 macCtl;
} brgusrdefinersvmacctrlhwread_params_t;

typedef struct
{
	UINT8 enable;
	UINT32 numOfArlNum;
} brgarlmacnumlimitset_params_t;


typedef struct
{
	UINT8 num;
	UINT8 mac[MAC_LENGTH];
} brgusrdefinersvmachwwrite_params_t;


typedef struct
{
	UINT8 num;
	UINT8 mac[MAC_LENGTH];
} brgusrdefinersvmachwread_params_t;


typedef struct
{
	UINT8 macNum;
	UINT8 icos;
	UINT8 macCtl;
} brgusrdefinersvmacctrlhwwrite_params_t;


typedef struct
{
	UINT8 macNum;
	UINT8 icos;
	UINT8 macCtl;
} brgrsvmacctrlhwread_params_t;


typedef struct
{
	UINT8 macNum;
	UINT8 icos;
	UINT8 macCtl;
} brgrsvmacctrlhwwrite_params_t;

typedef struct
{
	unsigned int moduleBit;
	OPL_CNT_t *oplCounter;
	unsigned int start;
	unsigned int num;
} dalcounterget_params_t;

#if defined(ONU_4PORT_AR8327)
typedef struct
{
	UINT32 dev_id;
	UINT32 phy_addr;
	UINT32 reg;
	UINT16 value;
}isisphyget_params_t;
#endif

typedef struct
{
	unsigned short vlan;
	unsigned short remarkPri;
	unsigned short insertPri;
} dalvlanqinqbaseadd_params_t;

typedef struct
{
	unsigned short vlan;
	unsigned short remarkPri;
	unsigned short insertPri;
} dalvlanqinqbasedel_params_t;

typedef struct
{
	unsigned short cvlan;
	unsigned short svlan;
	unsigned short sPriority;
} dalvlanqinqflexibleadd_params_t;

typedef struct
{
	unsigned short cvlan;
	unsigned short svlan;
	unsigned short sPriority;
} dalvlanqinqflexibledel_params_t;

typedef struct
{
	UINT8 mac[MAC_LENGTH];
} dalarldiscardmacadd_params_t;


typedef struct
{
	UINT8 mac[MAC_LENGTH];
} dalarldiscardmacdel_params_t;


typedef struct
{
	CLS_CONFIG_INFO_t *stClassCfg;
	UINT8 name[64];
} dalaclclsfindname_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 enable;
} dalportlearnenableset_params_t;


typedef struct
{
	UINT32 port;
	UINT32 toPort;
} dalarlmacmove_params_t;


typedef struct
{
	UINT8 portId;
	UINT16 vlanId;
	UINT8 macAddress[MAC_LENGTH];
} dalarlmacfilteradd_params_t;


typedef struct
{
	UINT8 portId;
	UINT16 vlanId;
	UINT8 macAddress[MAC_LENGTH];
} dalarlmacfilterdel_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 enable;
} dalarlmacdlffilteren_params_t;


typedef struct
{
	UINT8 portId;
	UINT8 enable;
} dalarlmacportlockdropen_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 enable;
} dalmiringportset_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 enable;
} dalmiringportget_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 enable;
} dalmiregportset_params_t;


typedef struct
{
	UINT8 portId;
	UINT32 enable;
} dalmiregportget_params_t;


typedef struct
{
	UINT8 portId;
	UINT16 vid;
	VLAN_EGRESS_TYPE_E type;
} daladdvlanport_params_t;


typedef struct
{
	UINT8 portId;
	UINT16 vid;
} dalmovevlanport_params_t;


typedef struct
{
	UINT8 portId;
	void *mib;
} dalmibget_params_t;


typedef struct
{
	UINT32 dev_id;
	UINT32 phy_addr;
	UINT32 reg;
	UINT16 value;
} isis_phy_get_params_t;


typedef struct
{
	UINT32 dev_id;
	UINT32 phy_addr;
	UINT32 reg;
	UINT16 value;
} isis_phy_set_params_t;


typedef struct
{
	UINT32 dev_id;
	UINT32 reg_addr;
	UINT8 *value;
	UINT32 value_len;
} isis_reg_get_params_t;


typedef struct
{
	UINT32 dev_id;
	UINT32 reg_addr;
	UINT8 *value;
	UINT32 value_len;
} isis_reg_set_params_t;


typedef struct
{
	UINT32 dev_id;
	UINT32 reg_addr;
	UINT32 bit_offset;
	UINT32 field_len;
	UINT8 *value;
	UINT32 value_len;
} isis_reg_field_get_params_t;


typedef struct
{
	UINT32 dev_id;
	UINT32 reg_addr;
	UINT32 bit_offset;
	UINT32 field_len;
	UINT8 *value;
	UINT32 value_len;
} isis_reg_field_set_params_t;


#pragma pack()		/* end of pragma pack definition */


#define OPCONN_IOC_MAGIC_BASE 200
#define NUM_MAGIC_IDS_PER_H_FILE 1


#define OPCONN_IOC_MAGIC_H_FILE0_BASE (OPCONN_IOC_MAGIC_BASE+0*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_DALPONMACIDSET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 0, dalponmacidset_params_t )
#define IOC_DALPONMACIDGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 1, dalponmacidget_params_t )
#define IOC_DALPONPHYFECMODESET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 2, UINT32 )
#define IOC_DALPONPHYFECMODEGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 3, UINT32 )
#define IOC_DALPONPHYTRIPLECHUNINGMODESET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 4, UINT32 )
#define IOC_DALPONPHYTRIPLECHUNINGMODEGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 5, UINT32 )
#define IOC_DALPONPHYAESMODESET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 6, UINT32 )
#define IOC_DALPONPHYAESMODEGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 7, UINT32 )
#define IOC_DALPONLASERONCNTSET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 8, UINT32 )
#define IOC_DALPONLASERONCNTGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 9, UINT32 )
#define IOC_DALPONLASEROFFCNTSET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 10, UINT32 )
#define IOC_DALPONLASEROFFCNTGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 11, UINT32 )
#define IOC_DALPONSYNCTIMEGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 12, UINT32 )
#define IOC_DALPONLLIDGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 13, UINT32 )
#define IOC_DALPONSERDESDELAYCNTSET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 14, UINT32 )
#define IOC_DALPONSERDESDELAYCNTGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 15, UINT32 )
#define IOC_DALPONSECURITYMODESET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 16, dalponsecuritymodeset_params_t )
#define IOC_DALPONSECURITYMODEGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 17, dalponsecuritymodeget_params_t )
#define IOC_DALPONSECURITYAESMODESET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 18, dalponsecurityaesmodeset_params_t )
#define IOC_DALPONSECURITYAESMODEGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 19, dalponsecurityaesmodeget_params_t )
#define IOC_DALPONUNICASTTRIPLECHURNINGKEYGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 20, dalponunicasttriplechurningkeyget_params_t )
#define IOC_DALPONSCBTRIPLECHURNINGKEYSET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 21, dalponscbtriplechurningkeyset_params_t )
#define IOC_DALPONUNICASTAESKEYGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 22, dalponunicastaeskeyget_params_t )
#define IOC_DALPONSCBAESKEYSET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 23, dalponscbaeskeyset_params_t )
#define IOC_DALPONTRIPLECHURNINGMODEGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 24, UINT8 )
#define IOC_DALPONTRIPLECHURNINGMODESET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 25, UINT8 )
#define IOC_DALPONMPCPDBAAGENTCONFIGALLSET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 26, dalponmpcpdbaagentconfigallset_params_t )
#define IOC_DALPONMPCPDBAAGENTCONFIGALLGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 27, dalponmpcpdbaagentconfigallget_params_t )
#define IOC_DALPONDELAYCONFIGALLSET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 28, dalpondelayconfigallset_params_t )
#define IOC_DALPONDELAYCONFIGALLGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 29, dalpondelayconfigallget_params_t )
#define IOC_DALPONMPCPSTATUSGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 30, DAL_PON_STATUS_t )
#define IOC_DALPONOAMFINISHEDSET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 31, UINT32 )
#define IOC_DALPONMPCPFSMHOLDOVERCTRLSET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 32, UINT32 )
#define IOC_DALPONMPCPFSMHOLDOVERCTRLGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 33, UINT32 )
#define IOC_DALPONMPCPFSMHOLDOVERTIMESET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 34, UINT32 )
#define IOC_DALPONMPCPFSMHOLDOVERTIMEGET _IOR( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 35, UINT32 )
#ifdef CONFIG_BOSA
#define IOC_DALPON25L90IMODSET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 36, UINT32 )
#define IOC_DALPON25L90APCSET _IOW( (OPCONN_IOC_MAGIC_H_FILE0_BASE+0), 37, UINT32 )
#endif


#define OPCONN_IOC_MAGIC_H_FILE1_BASE (OPCONN_IOC_MAGIC_BASE+1*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_DALCLSTBLENTRYADD _IOWR( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 0, dalclstblentryadd_params_t )
#define IOC_DALCLSTBLENTRYDEL _IOWR( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 1, dalclstblentrydel_params_t )
#define IOC_DALCLSMASKTBLWR _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 2, dalclsmasktblwr_params_t )
#define IOC_DALCLSRULETBLWR _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 3, dalclsruletblwr_params_t )
#define IOC_DALCLSADDRTBLWR _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 4, dalclsaddrtblwr_params_t )
#define IOC_DALCLSMASKHWWR _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 5, dalclsmaskhwwr_params_t )
#define IOC_DALCLSRULEHWWR _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 6, dalclsrulehwwr_params_t )
#define IOC_DALCLSADDRHWWR _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 7, dalclsaddrhwwr_params_t )
#define IOC_DALCLSRULETBLRD _IOWR( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 8, dalclsruletblrd_params_t )
#define IOC_DALCLSADDRTBLRD _IOWR( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 9, dalclsaddrtblrd_params_t )
#define IOC_DALCLSENTRYSET _IOWR( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 10, dalclsentryset_params_t )
#define IOC_DALCLSENTRYMOVE _IOWR( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 11, dalclsentrymove_params_t )
#define IOC_DALCLSINIT _IO( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 12 )
#define IOC_DALCLSTRANACTSET _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 13, dalclstranactset_params_t )
#define IOC_DALCLSRMKACTSET _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 14, dalclsrmkactset_params_t )
#define IOC_DALCLSRMKCOSSET _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 15, dalclsrmkcosset_params_t )
#define IOC_DALCLSRMKICOSSET _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 16, dalclsrmkicosset_params_t )
#define IOC_DALCLSRMKVIDSET _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 17, dalclsrmkvidset_params_t )
#define IOC_DALCLSEGRESSPORTCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 18, dalclsegressportcfg_params_t )
#define IOC_DALCLSINGRESSPORTCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 19, dalclsingressportcfg_params_t )
#define IOC_DALCLSTRANACTCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 20, dalclstranactcfg_params_t )
#define IOC_DALCLSRMKACTCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 21, dalclsrmkactcfg_params_t )
#define IOC_DALCLSRMKCOSCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 22, dalclsrmkcoscfg_params_t )
#define IOC_DALCLSRMKICOSCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 23, dalclsrmkicoscfg_params_t )
#define IOC_DALCLSRMKVIDCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 24, dalclsrmkvidcfg_params_t )
#define IOC_DALCLSMASKPRIOCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 25, dalclsmaskpriocfg_params_t )
#define IOC_DALCLSDESTMACADDRCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 26, dalclsdestmacaddrcfg_params_t )
#define IOC_DALCLSSRCMACADDRCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 27, dalclssrcmacaddrcfg_params_t )
#define IOC_DALCLSFIRSTCOSCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 28, dalclsfirstcoscfg_params_t )
#define IOC_DALCLSFIRSTVIDCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 29, dalclsfirstvidcfg_params_t )
#define IOC_DALCLSTYPELENCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 30, dalclstypelencfg_params_t )
#define IOC_DALCLSDESTIPCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 31, dalclsdestipcfg_params_t )
#define IOC_DALCLSSRCIPCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 32, dalclssrcipcfg_params_t )
#define IOC_DALCLSIPTYPECFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 33, dalclsiptypecfg_params_t )
#define IOC_DALCLSIPTOSCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 34, dalclsiptoscfg_params_t )
#define IOC_DALCLSL4DESTPORTCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 35, dalclsl4destportcfg_params_t )
#define IOC_DALCLSL4SRCPORTCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 36, dalclsl4srcportcfg_params_t )
#define IOC_DALCLSRULEDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 37, UINT16 )
#define IOC_DALCLSCONVERTPARA _IOWR( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 38, dalclsconvertpara_params_t )
#define IOC_DALCLSCTCINIT _IO( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 39 )
#define IOC_DALCLSRULECTCADD _IOWR( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 40, dalclsrulectcadd_params_t )
#define IOC_DALCLSRULECTCDEL _IOWR( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 41, dalclsrulectcdel_params_t )
#define IOC_DALCLSRULECTCCLEAR _IO( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 42 )
#define IOC_DALEXCLSDELENTRY _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 43, UINT32 )
#define IOC_DALCLSRMKVIDACTSET _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 44, dalClsRmkVidActSet_params_t )
#define IOC_DALCLSIPVERSIONCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 45, dalClsIpVersionCfg_params_t )
#define IOC_DALCLSFIRSTTPIDCFG _IOW( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 46, dalClsFirstTpidCfg_params_t )
#define IOC_DALVOIPCLSRULECTCADD _IOWR( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 47, dalvoipclsrulectcadd_params_t )
#define IOC_DALCLSVALIDENTRYGET  _IOR( (OPCONN_IOC_MAGIC_H_FILE1_BASE+0), 48, UINT16 ) 

#define OPCONN_IOC_MAGIC_H_FILE2_BASE (OPCONN_IOC_MAGIC_BASE+2*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_DALCHIPRESET _IO( (OPCONN_IOC_MAGIC_H_FILE2_BASE+0), 0 )
#define IOC_DALDEMONINIT _IO( (OPCONN_IOC_MAGIC_H_FILE2_BASE+0), 1 )


#define OPCONN_IOC_MAGIC_H_FILE3_BASE (OPCONN_IOC_MAGIC_BASE+3*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_DALARLAGETIMESET _IOW( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 0, UINT32 )
#define IOC_DALARLAGETIMEGET _IOR( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 1, UINT32 )
#define IOC_DALARLMACADD _IOW( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 2, dalarlmacadd_params_t )
#define IOC_DALARLMACDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 3, dalarlmacdel_params_t )
#define IOC_DALARLMACREMOVE _IOW( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 4, dalarlmacremove_params_t )
#define IOC_DALARLFLUSHDYNAMIC _IO( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 5 )
#define IOC_DALARLFLUSHALL _IO( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 6 )
#define IOC_DALARLLEARNENSET _IOW( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 7, UINT8 )
#define IOC_DALARLLEARNENGET _IOR( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 8, UINT8 )
#define IOC_DALARLSOFTLEARNENSET _IOW( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 9, UINT8 )
#define IOC_DALARLSOFTLEARNENGET _IOR( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 10, UINT8 )
#define IOC_DALARLAGEENABLESET _IOW( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 11, UINT8 )
#define IOC_DALARLAGEENABLEGET _IOR( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 12, UINT8 )
#define IOC_DALARLMACNUMLIMITSET _IOW( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 13, dalarlmacnumlimitset_params_t )
#define IOC_DALARLMULTIPORTMACADD _IOW( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 14, dalarlmultiportmacadd_params_t )
#define IOC_DALARLMULTIPORTMACDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 15, dalarlmultiportmacdel_params_t )
#define IOC_DALARLMACENTRYFIND _IOWR( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 16, dalarlmacentryfind_params_t )
#define IOC_DALARLMACENTRYTRAVERSE _IOWR( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 17, dalarlmacentrytraverse_params_t )
#define IOC_DALARLMACENTRYSHOWONE _IOW( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 18, dalarlmacentryshowone_params_t )
#define IOC_DALARLMACENTRYSHOWALL _IOW( (OPCONN_IOC_MAGIC_H_FILE3_BASE+0), 19, dalarlmacentryshowall_params_t )


#define OPCONN_IOC_MAGIC_H_FILE4_BASE (OPCONN_IOC_MAGIC_BASE+4*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_DALMULTICASTINIT _IO( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 0 )
#define IOC_DALMULTICASTVLANADDPORT _IOW( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 1, dalmulticastvlanaddport_params_t )
#define IOC_DALMULTICASTVLANDELPORT _IOW( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 2, dalmulticastvlandelport_params_t )
#define IOC_DALMULTICASTVLANTAGSTRIPE _IOW( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 3, dalmulticastvlantagstripe_params_t )
#define IOC_DALMULTICASTARLADDPORT _IOW( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 4, dalmulticastarladdport_params_t )
#define IOC_DALMULTICASTARLPORTDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 5, dalmulticastarlportdel_params_t )
#define IOC_DALMULTICASTARLVLANADD _IOW( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 6, dalmulticastarlvlanadd_params_t )
#define IOC_DALMULTICASTARLVLANDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 7, dalmulticastarlvlandel_params_t )
#define IOC_DALMULTICASTARLTABFLUSH _IO( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 8 )
#define IOC_DALMULTICASTPORTVLANMACADD _IOW( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 9, dalmulticastportvlanmacadd_params_t )
#define IOC_DALMULTICASTPORTVLANMACDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 10, dalmulticastportvlanmacdel_params_t )
#define IOC_DALMULTICASTPORTVLANIPADD _IOW( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 11, dalmulticastportvlanipadd_params_t )
#define IOC_DALMULTICASTPORTVLANIPDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 12, dalmulticastportvlanipdel_params_t )
#define IOC_DALMULTICASTRULEDELALL _IO( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 13 )
#define IOC_DALMULTICASTDATAFILTERADD _IO( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 14 )
#define IOC_DALMULTICASTDATAFILTERDEL _IO( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 15 )
#define IOC_DALMULTICASTNUMOFRULEGET _IOR( (OPCONN_IOC_MAGIC_H_FILE4_BASE+0), 16, UINT8 )


#define OPCONN_IOC_MAGIC_H_FILE5_BASE (OPCONN_IOC_MAGIC_BASE+5*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_DALPORTNUMISINVALID _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 0, UINT8 )
#define IOC_DALPORTSPECIFICDUPLEXREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 1, dalportspecificduplexread_params_t )
#define IOC_DALPHYSPECIFICAUTONEGREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 2, dalphyspecificautonegread_params_t )
#define IOC_DALPORTSPECIFICSPEEDREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 3, dalportspecificspeedread_params_t )
#define IOC_DALPORTSTATEGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 4, dalportstateget_params_t )
#define IOC_DALPORTSPEEDSET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 5, dalportspeedset_params_t )
#define IOC_DALPORTSPEEDGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 6, dalportspeedget_params_t )
#define IOC_DALPORTDUPLEXSET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 7, dalportduplexset_params_t )
#define IOC_DALPORTDUPLEXGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 8, dalportduplexget_params_t )
#define IOC_DALPORTFLOWCONTROLSET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 9, dalportflowcontrolset_params_t )
#define IOC_DALPORTFLOWCONTROLGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 10, dalportflowcontrolget_params_t )
#define IOC_DALPORTLOOPBACKSET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 11, dalportloopbackset_params_t )
#define IOC_DALPORTUSPOLICINGENABLESET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 12, dalportuspolicingenableset_params_t )
#define IOC_DALPORTUSPOLICINGCIRSET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 13, dalportuspolicingcirset_params_t )
#define IOC_DALPORTUSPOLICINGCIRGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 14, dalportuspolicingcirget_params_t )
#define IOC_DALPORTUSPOLICINGCBSSET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 15, dalportuspolicingcbsset_params_t )
#define IOC_DALPORTUSPOLICINGCBSGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 16, dalportuspolicingcbsget_params_t )
#define IOC_DALPORTUSPOLICINGEBSSET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 17, dalportuspolicingebsset_params_t )
#define IOC_DALPORTUSPOLICINGEBSGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 18, dalportuspolicingebsget_params_t )
#define IOC_DALPORTUSPOLICINGSET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 19, dalportuspolicingset_params_t )
#define IOC_DALPORTUSPOLICINGGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 20, dalportuspolicingget_params_t )
#define IOC_DALPORTDSPOLICINGENABLESET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 21, dalportdspolicingenableset_params_t )
#define IOC_DALPORTDSPOLICINGENABLEGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 22, dalportdspolicingenableget_params_t )
#define IOC_DALPORTDSPOLICINGCIRSET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 23, dalportdspolicingcirset_params_t )
#define IOC_DALPORTDSPOLICINGCIRGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 24, dalportdspolicingcirget_params_t )
#define IOC_DALPORTDSPOLICINGPIRSET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 25, dalportdspolicingpirset_params_t )
#define IOC_DALPORTDSPOLICINGPIRGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 26, dalportdspolicingpirget_params_t )
#define IOC_DALPORTDSPOLICINGSET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 27, dalportdspolicingset_params_t )
#define IOC_DALPORTDSPOLICINGGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 28, dalportdspolicingget_params_t )
#define IOC_DALPHYADMINSTATEGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 29, dalphyadminstateget_params_t )
#define IOC_DALPHYADMINCONTROLSET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 30, dalphyadmincontrolset_params_t )
#define IOC_DALPHYAUTONEGADMINSTATEGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 31, dalphyautonegadminstateget_params_t )
#define IOC_DALPHYAUTONEGENABLESET _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 32, dalphyautonegenableset_params_t )
#define IOC_DALPHYAUTONEGRESTART _IOW( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 33, UINT8 )
#define IOC_DALPORTINIT _IO( (OPCONN_IOC_MAGIC_H_FILE5_BASE+0), 34 )


#define OPCONN_IOC_MAGIC_H_FILE6_BASE (OPCONN_IOC_MAGIC_BASE+6*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_DALRSTPMODESET _IOW( (OPCONN_IOC_MAGIC_H_FILE6_BASE+0), 0, IN OPL_BOOL )
#define IOC_DALRSTPPORTSTATEGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE6_BASE+0), 1, dalrstpportstateget_params_t )
#define IOC_DALRSTPPORTSTATESET _IOW( (OPCONN_IOC_MAGIC_H_FILE6_BASE+0), 2, dalrstpportstateset_params_t )
#define IOC_DALRSTPPORTMACFLUSH _IOW( (OPCONN_IOC_MAGIC_H_FILE6_BASE+0), 3, dalrstpportmacflush_params_t )


#define OPCONN_IOC_MAGIC_H_FILE7_BASE (OPCONN_IOC_MAGIC_BASE+7*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_DALSTORMCLSRULEADD _IOW( (OPCONN_IOC_MAGIC_H_FILE7_BASE+0), 0, dalstormclsruleadd_params_t )
#define IOC_DALSTORMCLSRULEDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE7_BASE+0), 1, dalstormclsruledel_params_t )
#define IOC_DALSTORMCTRLFRAMESET _IOW( (OPCONN_IOC_MAGIC_H_FILE7_BASE+0), 2, dalstormctrlframeset_params_t )
#define IOC_DALSTORMCTRLFRAMEGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE7_BASE+0), 3, dalstormctrlframeget_params_t )
#define IOC_DALSTORMCTRLRATESET _IOW( (OPCONN_IOC_MAGIC_H_FILE7_BASE+0), 4, dalstormctrlrateset_params_t )
#define IOC_DALSTORMCTRLRATEGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE7_BASE+0), 5, dalstormctrlrateget_params_t )


#define OPCONN_IOC_MAGIC_H_FILE8_BASE (OPCONN_IOC_MAGIC_BASE+8*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_OPLVERSTRGET _IOR( (OPCONN_IOC_MAGIC_H_FILE8_BASE+0), 0, UINT8 )
//#define IOC_OPLVERSHOW _IO( (OPCONN_IOC_MAGIC_H_FILE8_BASE+0), 2 )


#define OPCONN_IOC_MAGIC_H_FILE9_BASE (OPCONN_IOC_MAGIC_BASE+9*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_DALVTTMODESET _IOW( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 0, dalvttmodeset_params_t )
#define IOC_DALVTTMODEGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 1, dalvttmodeget_params_t )
#define IOC_DALVTTTPIDSET _IOW( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 2, dalvtttpidset_params_t )
#define IOC_DALVTTTPIDGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 3, dalvtttpidget_params_t )
#define IOC_DALVTTVIDSET _IOW( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 4, dalvttvidset_params_t )
#define IOC_DALVTTVIDGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 5, dalvttvidget_params_t )
#define IOC_DALVTTDEFAULTPRISET _IOW( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 6, dalvttdefaultpriset_params_t )
#define IOC_DALVTTDEFAULTPRIGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 7, dalvttdefaultpriget_params_t )
#define IOC_DALVTTTAGENTRYADD _IOWR( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 8, dalvtttagentryadd_params_t )
#define IOC_DALVTTTAGENTRYGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 9, dalvtttagentryget_params_t )
#define IOC_DALVTTTAGENTRYDEL _IOWR( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 10, dalvtttagentrydel_params_t )
#define IOC_DALVTTNUMOFRECORDGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 11, dalvttnumofrecordget_params_t )
#define IOC_DALVTTMULTICASTENTRYADD _IOW( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 12, dalvttmulticastentryadd_params_t )
#define IOC_DALVTTMULTICASTENTRYDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 13, dalvttmulticastentrydel_params_t )
#define IOC_DALVTTMULTICASTSTRIPENABLE _IOW( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 14, dalvttmulticaststripenable_params_t )
#define IOC_DALVTTMULTICASTINIT _IOW( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 15, UINT8 )
#define IOC_DALVTTMULTICASTNUMOFRECORDGET _IOR( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 16, UINT8 )
#define IOC_DALVTTINIT _IO( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 17 )
/* Begin Added of qinq */
#define IOC_DALVTTQINQPVIDSET _IOW( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 18, qinqpvidset_params_t )
#define IOC_DALVTTQINQPVIDRESET _IOW( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 19, qinqpvidset_params_t )
#define IOC_DALVTTENTRYTAGADD   _IOW( (OPCONN_IOC_MAGIC_H_FILE9_BASE+0), 20, vttentrytag_params_t )
/* End   Added of qinq */



#define OPCONN_IOC_MAGIC_H_FILE10_BASE (OPCONN_IOC_MAGIC_BASE+10*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_TMGESHAPERENABLEHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 0, tmgeshaperenablehwwrite_params_t )
#define IOC_TMGESHAPERENABLEHWREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 1, tmgeshaperenablehwread_params_t )
#define IOC_TMQUEUESHAPERENABLEHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 2, tmqueueshaperenablehwwrite_params_t )
#define IOC_TMQUEUESHAPERENABLEHWREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 3, tmqueueshaperenablehwread_params_t )
#define IOC_TMQUEUEPRIHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 4, tmqueueprihwwrite_params_t )
#define IOC_TMQUEUEPRIHWREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 5, tmqueueprihwread_params_t )
#define IOC_TMQUEUEWEIGHTHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 6, tmqueueweighthwwrite_params_t )
#define IOC_TMQUEUEWEIGHTHWREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 7, tmqueueweighthwread_params_t )
#define IOC_TMGECIRHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 8, tmgecirhwwrite_params_t )
#define IOC_TMGECIRHWREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 9, tmgecirhwread_params_t )
#define IOC_TMGECBSHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 10, tmgecbshwwrite_params_t )
#define IOC_TMGECBSHWREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 11, tmgecbshwread_params_t )
#define IOC_TMCOSICOSMAPHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 12, tmcosicosmaphwwrite_params_t )
#define IOC_TMCOSICOSMAPHWREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 13, tmcosicosmaphwread_params_t )
#define IOC_TMQUEUECBSHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 14, tmqueuecbshwwrite_params_t )
#define IOC_TMQUEUECBSHWREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 15, tmqueuecbshwread_params_t )
#define IOC_TMQUEUECIRHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 16, tmqueuecirhwwrite_params_t )
#define IOC_TMQUEUECIRHWREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 17, tmqueuecirhwread_params_t )
#define IOC_TMWREDCFGSET _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 18, tmwredcfgset_params_t )
#define IOC_TMWREDENABLEHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 19, tmwredenablehwwrite_params_t )
#define IOC_TMWREDENABLEHWREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 20, tmwredenablehwread_params_t )
#define IOC_TMUSSHAPERTABDIRECTHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 21, tmusshapertabdirecthwwrite_params_t )
#define IOC_TMUSWREDTABDIRECTHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 22, tmuswredtabdirecthwwrite_params_t )
#define IOC_TMDSWREDTABDIRECTHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 23, tmdswredtabdirecthwwrite_params_t )
#define IOC_TMDSSHAPERTABDIRECTHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 24, tmdsshapertabdirecthwwrite_params_t )
#define IOC_TMQUEUEMAXCELLNUMHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 25, tmqueuemaxcellnumhwwrite_params_t )
#define IOC_TMWREDCFGGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 26, tmwredcfgget_params_t )
#define IOC_TMMAXPUSWREDTABDIRECTHWWRITE _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 27, tmMaxpUsWredTabDirectHwWrite_params_t )
#define IOC_TMMAXPDSWREDTABDIRECTHWWRITE _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 28, tmMaxpDsWredTabDirectHwWrite_params_t )
#define IOC_TMQUEUESHAPERCIRENABLEHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 29, tmqueueshapercirenablehwwrite_params_t )
#define IOC_TMUSSHAPERTABPIRDIRECTHWWRITE _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 30, tmusshapertabpirdirecthwwrite_params_t )
#define IOC_TMQUEUEMAXPKTNUMHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 31, tmqueuemaxpktNumhwwrite_params_t)
#define IOC_TMUSSHAPERTABCIRDIRECTHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 32, tmusshapertabcirdirecthwwrite_params_t  )
#define IOC_TMQUEUESHAPERPIRENABLEHWWRITE _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 33, tmqueueshaperpirenablehwwrite_params_t )
#define IOC_TMCELLNUMINQUEUEHWREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE10_BASE+0), 34, tmcellnuminqueuehwread_params_t )

#define OPCONN_IOC_MAGIC_H_FILE11_BASE (OPCONN_IOC_MAGIC_BASE+11*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_PHYSPEEDHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 0, UINT32 )
#define IOC_PHYSPEEDHWREAD _IOR( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 1, PHY_SPEED_MODE_e )
#define IOC_PHYDUPLEXHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 2, PHY_DUPLEX_MODE_e )
#define IOC_PHYDUPLEXHWREAD _IOR( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 3, PHY_DUPLEX_MODE_e )
#define IOC_PHYAUTONEGHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 4, UINT8 )
#define IOC_PHYAUTONEGHWREAD _IOW( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 5, UINT8 )
#define IOC_PHYRESTARTAUTONEG _IO( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 6 )
#define IOC_PHYLOOPBACKENABLEHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 7, UINT8 )
#define IOC_PHYLOOPBACKENABLEHWREAD _IOR( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 8, UINT8 )
#define IOC_PHYRESET _IO( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 9 )
#define IOC_PHYPOWERDOWN _IOW( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 10, UINT8 )
#define IOC_PHYLINKSTATUSHWREAD _IOR( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 11, UINT8 )
#define IOC_PHYFLOWCONTROLENHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 12, UINT8 )
#define IOC_PHYSPECIFICAUTOREAD _IOR( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 13, UINT32 )
#define IOC_PHYSPECIFICSPEEDREAD _IOR( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 14, PHY_SPEED_MODE_e )
#define IOC_PHYSPECIFICDUPLEXREAD _IOR( (OPCONN_IOC_MAGIC_H_FILE11_BASE+0), 15, PHY_DUPLEX_MODE_e )

#define OPCONN_IOC_MAGIC_H_FILE12_BASE (OPCONN_IOC_MAGIC_BASE+12*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_OPLREGREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE12_BASE+0), 0, oplregread_params_t )
#define IOC_OPLREGWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE12_BASE+0), 1, oplregwrite_params_t )
#define IOC_OPLREGFIELDREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE12_BASE+0), 2, oplregfieldread_params_t )
#define IOC_OPLREGFIELDWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE12_BASE+0), 3, oplregfieldwrite_params_t )
#define IOC_OPLTABREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE12_BASE+0), 4, opltabread_params_t )
#define IOC_OPLTABWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE12_BASE+0), 5, opltabwrite_params_t )

#define OPCONN_IOC_MAGIC_H_FILE13_BASE (OPCONN_IOC_MAGIC_BASE+13*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_MDIOREGISTERREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE13_BASE+0), 0, mdioregisterread_params_t )
#define IOC_MDIOREGISTERWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE13_BASE+0), 1, mdioregisterwrite_params_t )
#define IOC_MDIOREGISTERFIELDREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE13_BASE+0), 2, mdioregisterfieldread_params_t )
#define IOC_MDIOREGISTERFIELDWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE13_BASE+0), 3, mdioregisterfieldwrite_params_t )
#define IOC_MDIOREGISTERINIT _IO( (OPCONN_IOC_MAGIC_H_FILE13_BASE+0), 4 )
/* Begin Added*/
#define IOC_SWMDIORGLBREGREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE13_BASE+0), 5, swmdioregisterread_params_t )
#define IOC_SWMDIORGLBREGWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE13_BASE+0), 6, swmdioregisterwrite_params_t )
#define IOC_SWMDIOPHYREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE13_BASE+0), 7, swmdiophyregread_params_t )
#define IOC_SWMDIOPHYWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE13_BASE+0), 8, swmdiophyregwrite_params_t )
/* End   Added of porting */

#define OPCONN_IOC_MAGIC_H_FILE14_BASE (OPCONN_IOC_MAGIC_BASE+14*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_CHIPRESET _IO( (OPCONN_IOC_MAGIC_H_FILE14_BASE+0), 0 )
#define IOC_CHIPATTRGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE14_BASE+0), 1, chipattrget_params_t )
#define IOC_CHIPINIT _IO( (OPCONN_IOC_MAGIC_H_FILE14_BASE+0), 2 )
#define IOC_LINKLISTENABLE _IO( (OPCONN_IOC_MAGIC_H_FILE14_BASE+0), 3 )
#define IOC_RESPININIT _IO( (OPCONN_IOC_MAGIC_H_FILE14_BASE+0), 4 )
#define IOC_GMACINIT _IO( (OPCONN_IOC_MAGIC_H_FILE14_BASE+0), 5 )
#define IOC_PMACINIT _IO( (OPCONN_IOC_MAGIC_H_FILE14_BASE+0), 6 )

#define OPCONN_IOC_MAGIC_H_FILE15_BASE (OPCONN_IOC_MAGIC_BASE+15*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_DALCLSDELENTRY _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 0, UINT32 )
#define IOC_DALVTTVLANAGGDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 1, dalvttvlanaggdel_params_t )
#define IOC_DALVTTVLANAGGADD _IOWR( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 2, dalvttvlanaggadd_params_t )
#define IOC_BRGRSVMACCTRLHWREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 3, brgrsvmacctrlhwread_params_t )
#define IOC_BRGRSVMACCTRLHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 4, brgrsvmacctrlhwwrite_params_t )
#define IOC_DALMULTICASTENABLE _IO( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 5 )
#define IOC_DALMULTICASTDISABLE _IO( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 6 )
#define IOC_I2CINIT _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 7, i2cinit_params_t )
#define IOC_I2CREADRANDOM _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 8, i2creadrandom_params_t )
#define IOC_DALONUCLSRULECTCADD _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 9, opl_acl_list_t )
#define IOC_DALONUCLSRULECTCCHANGE _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 10, dalonuclsrulectcchange_params_t )
#define IOC_DALONUCLSRULECTCDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 11, opl_acl_list_t )
#define IOC_DALVOIPCLSDELENTRY _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 12, UINT32 )
#define IOC_DALCLACFGCHECK _IOWR( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 13, dalclacfgcheck_params_t )
#define IOC_DALPORTPVIDSET _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 14, dalportpvidset_params_t )
#define IOC_DALPORTPVIDGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 15, dalportpvidget_params_t )
#define IOC_DALPORTVLANMODESET _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 16, dalportvlanmodeset_params_t )
#define IOC_DALPORTVLANMODEGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 17, dalportvlanmodeget_params_t )
#define IOC_DALPORTDEFAULTPRISET _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 18, dalportdefaultpriset_params_t )
#define IOC_DALTRUNKENTRYADD _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 19, daltrunkentryadd_params_t )
#define IOC_DALTRUNKENTRYDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 20, daltrunkentrydel_params_t )
#define IOC_DALTRUNKENTRYVALUEGET _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 21, daltrunkentryvalueget_params_t )
#define IOC_DALTRUNKENTRYNUMGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 22, daltrunkentrynumget_params_t )
#define IOC_DALTRUNKENTRYCLEAR _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 23, UINT8 )
#define IOC_DALCREATEVLANID _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 24, UINT16 )
#define IOC_DALFLUSHVLANALL _IO( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 25 )
#define IOC_DALSETLOCALSWITCH _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 26, dalsetlocalswitch_params_t )
#define IOC_DALCLSRMKACTGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 27, dalclsrmkactget_params_t )
#define IOC_DALPONINIT _IO( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 28)
#define IOC_DALDBAQUEUETHRESHOLDSET _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 29, daldbaqueuethresholdset_params_t )
#define IOC_DALDBAQUEUETHRESHOLDGET _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 30, daldbaqueuethresholdget_params_t )
#define IOC_DALDBAREPORTMODESET _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 31, UINT32 )
#define IOC_DALDBAREPORTMODEGET _IOR( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 32, UINT32 )
#define IOC_DALONUMTUSET _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 33, UINT32 )
#define IOC_DALONUMTUGET _IOR( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 34, UINT32 )
#define IOC_DALONUPONLOOPBACKCONFIG _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 35, dalonuponloopbackconfig_params_t )
#define IOC_DALONUPONTXPOLARITYCONFIG _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 36, UINT32 )
#define IOC_BRGUSRDEFINERSVMACHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 37, brgusrdefinersvmachwwrite_params_t )
#define IOC_BRGUSRDEFINERSVMACHWREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 38, brgusrdefinersvmachwread_params_t )
#define IOC_BRGUSRDEFINERSVMACCTRLHWWRITE _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 39, brgusrdefinersvmacctrlhwwrite_params_t )
#define IOC_BRGUSRDEFINERSVMACCTRLHWREAD _IOWR( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 40, brgusrdefinersvmacctrlhwread_params_t )
#define IOC_DALCLSRULEENTRYSHOW _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 41, dalclsruleentryshow_params_t )
#define IOC_DALCLSMASKENTRYSHOW _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 42, dalclsmaskentryshow_params_t )
#define IOC_DALCLSADDRENTRYSHOW _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 43, dalclsaddrentryshow_params_t )
#define IOC_DALCLSCFGSHOW _IOW( (OPCONN_IOC_MAGIC_H_FILE15_BASE+0), 44, dalclscfgshow_params_t )

#define OPCONN_IOC_MAGIC_H_FILE16_BASE (OPCONN_IOC_MAGIC_BASE+16*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_DALPONPORTSTATISTICSGET1 _IOR( (OPCONN_IOC_MAGIC_H_FILE16_BASE+0), 0, ONU_PON_Port_Statistics_Get_1_t )
#define IOC_DALPONPORTSTATISTICSGET2 _IOR( (OPCONN_IOC_MAGIC_H_FILE16_BASE+0), 1, ONU_PON_Port_Statistics_Get_2_t )
#define IOC_DALPORTFLUXSTATISTICSCOUNTERGET _IOR( (OPCONN_IOC_MAGIC_H_FILE16_BASE+0), 2, ONU_Port_Flux_Statistics_Counter_t )
#define IOC_DALCOUNTERGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE16_BASE+0), 3, dalcounterget_params_t )
#define IOC_DALCOUNTERENABLE _IOW( (OPCONN_IOC_MAGIC_H_FILE16_BASE+0), 4, unsigned int )
#define IOC_DALCOUNTERENABLEGET _IOR( (OPCONN_IOC_MAGIC_H_FILE16_BASE+0), 5, unsigned int )
#define IOC_DALCOUNTERCLEAR _IOW( (OPCONN_IOC_MAGIC_H_FILE16_BASE+0), 6, unsigned int )


#define OPCONN_IOC_MAGIC_H_FILE17_BASE (OPCONN_IOC_MAGIC_BASE+17*NUM_MAGIC_IDS_PER_H_FILE)
#define IOC_DALVLANQINQMODESET _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 0, unsigned char )
#define IOC_DALVLANQINQBASEADD _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 1, dalvlanqinqbaseadd_params_t )
#define IOC_DALVLANQINQBASEDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 2, dalvlanqinqbasedel_params_t )
#define IOC_DALVLANTRANSPARENTADD _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 3, unsigned short )
#define IOC_DALVLANTRANSPARENTDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 4, unsigned short )
#define IOC_DALVLANQINQFLEXIBLEADD _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 5, dalvlanqinqflexibleadd_params_t )
#define IOC_DALVLANQINQFLEXIBLEDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 6, dalvlanqinqflexibledel_params_t )
#define IOC_DALARLDISCARDMACADD _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 7, UINT8 )
#define IOC_DALARLDISCARDMACDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 8, UINT8 )
#define IOC_DALACLRSTPADD _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 9, UINT8 )
#define IOC_DALMULTICASTVLANCREATE _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 10, UINT16 )
#define IOC_DALMULTICASTVLANDELETE _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 11, UINT16 )
#define IOC_DALACLCLSFINDNAME _IOWR( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 12, dalaclclsfindname_params_t )
#define IOC_DALARLFLUSHALLBYPORT _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 13, UINT8 )
#define IOC_DALPORTLEARNENABLESET _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 14, dalportlearnenableset_params_t )
#define IOC_DALARLMACMOVE _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 15, dalarlmacmove_params_t )
#define IOC_DALARLMACFILTERADD _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 16, dalarlmacfilteradd_params_t )
#define IOC_DALARLMACFILTERDEL _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 17, dalarlmacfilterdel_params_t )
#define IOC_DALARLMACDLFFILTEREN _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 18, dalarlmacdlffilteren_params_t )
#define IOC_DALARLMACPORTLOCKDROPEN _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 19, dalarlmacportlockdropen_params_t )
#define IOC_DALMIRPORTSET _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 20, UINT8 )
#define IOC_DALMIRPORTGET _IOR( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 21, UINT8 )
#define IOC_DALMIRINGPORTSET _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 22, dalmiringportset_params_t )
#define IOC_DALMIRINGPORTGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 23, dalmiringportget_params_t )
#define IOC_DALMIREGPORTSET _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 24, dalmiregportset_params_t )
#define IOC_DALMIREGPORTGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 25, dalmiregportget_params_t )
#define IOC_DALDELETEVLANID _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 26, UINT16 )
#define IOC_DALADDVLANPORT _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 27, daladdvlanport_params_t )
#define IOC_DALMOVEVLANPORT _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 28, dalmovevlanport_params_t )
#define IOC_DALMIBGET _IOWR( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 29, dalmibget_params_t )
#define IOC_ISIS_PHY_GET _IOWR( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 30, isis_phy_get_params_t )
#define IOC_ISIS_PHY_SET _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 31, isis_phy_set_params_t )
#define IOC_ISIS_REG_GET _IOWR( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 32, isis_reg_get_params_t )
#define IOC_ISIS_REG_SET _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 33, isis_reg_set_params_t )
#define IOC_ISIS_REG_FIELD_GET _IOWR( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 34, isis_reg_field_get_params_t )
#define IOC_ISIS_REG_FIELD_SET _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 35, isis_reg_field_set_params_t )
#define IOC_ISIS_ACL_RULE_DUMP _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 36, UINT32 )
#define IOC_ISIS_ACL_LIST_DUMP _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 37, UINT32 )
#define IOC_ATHEROS_SHOW_CNT _IOW( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 38, UINT32 )
#define IOC_DALVTTNUMEXISTGET _IOR( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 39, unsigned int )
#define IOC_DALARVTTSHOW _IOR( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 40, unsigned int )
#define IOC_DALVLANSHOW _IOR( (OPCONN_IOC_MAGIC_H_FILE17_BASE+0), 41, unsigned int )

OPL_STATUS get_ioctl_cmd_indexes( int cmd, unsigned int *h_file_index, unsigned int *fn_index);

int opconn_dal_ioctl( unsigned int cmd, unsigned int arg );



