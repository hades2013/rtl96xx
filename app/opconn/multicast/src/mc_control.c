/*
*
*  COPYRIGHT (C) 2003-2009 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*   used only with Opulan semi-conductor products.
*
* mc_control.c
*
* DESCRIPTION:
*
* Date Created:
*
* Authors(optional):
*
* Reviewed by (optional):
*
* Edit History:
*
*/

/**
 * @defgroup mc_control MC_CONTROL
 * @{
 */

/***** INCLUDE FILES *****/
#include "vos.h"
#include "hal.h"
#include "mc_control.h"
#include "opl_host.h"
#include "odm_vlan.h"
#include <errno.h>

/***** LOCAL DEFINES and ENUM *****/
/*#define MC_DEBUG(LEVEL, FMT, ...) \
  OP_DEBUG(LEVEL, "%s, %d ", __FUNCTION__, __LINE__); \
  OP_DEBUG(LEVEL, FMT, ##__VA_ARGS__)*/
/*#define MC_DEBUG(LEVEL, FMT, ...) \
  printf(FMT, ##__VA_ARGS__)*/

/***** LOCAL STRUCTURES and UNIONS *****/
struct oam_mcast_vlan_translation_entry{
 uint16   mcast_vlan;
 uint16   iptv_vlan;
}__attribute__((packed));
typedef struct oam_mcast_vlan_translation_entry oam_mcast_vlan_translation_entry_t;

/***** LOCAL TYPEDEFS ****/

/***** LOCAL (PRIVATE) PROTOTYPES *****/
static TIMER_OBJ_t *mcHostAgingTimer = NULL;

/* port based multicast vlan */
/* */
static uint32 mcVlanNum = 0;
/* 013987 */
static uint32 mcVlanMap[OP_MAX_VLAN_NUMBER]; /* portX: 1<<ODM_START_PORT_NUN+X-1 */

static uint32 mcMaxGroupPerPort[ODM_NUM_OF_PORTS+1];

static uint32 mcTagOperPerPort[ODM_NUM_OF_PORTS+1]; /* transparent, strip, translation */

static uint32 mcGroupTotalNum; /* the total number of groups */

/* use array, this is not the critical data structure */
MC_VLAN_TRANSLATION_ENTRY_t mcTagTranslationTable[MC_VLAN_TRANSLATION_ENTRY];

static MC_ROUTER_PORT_ENTRY_t mcRouterPort[MC_ROUTER_PORT_NUM];

static MC_CONTROL_STATS_t mcControlStats;

static clList *mcGroupList;

static MC_CONTROL_t mcControlBlock;

uint32 mcPktPrintDebug = 0;

static  VOS_SEM_t mcGroupSem;// = NULL;
static  uint32 mcGroupLock = 0;


#if defined(ONU_1PORT)
static uint32 mcDnSwitchType = MC_DN_SWITCH_TYPE_ONU_1PORT; /* the total number of groups */
#define SEND_SPECIFIC_QUERY_TO_UNI_PORT_DIRECTLY
#elif defined(ONU_4PORT_88E6045)
static uint32 mcDnSwitchType = MC_DN_SWITCH_TYPE_ONU_4PORT_88E6045; /* the total number of groups */
#elif defined(ONU_4PORT_88E6046)
static uint32 mcDnSwitchType = MC_DN_SWITCH_TYPE_ONU_4PORT_88E6046; /* the total number of groups */
#elif defined(ONU_4PORT_88E6097)
static uint32 mcDnSwitchType = MC_DN_SWITCH_TYPE_ONU_4PORT_88E6097; /* the total number of groups */
#elif defined(ONU_4PORT_AR8228)
static uint32 mcDnSwitchType = MC_DN_SWITCH_TYPE_ONU_4PORT_AR8228; /* the total number of groups */
#define SEND_SPECIFIC_QUERY_TO_UNI_PORT_DIRECTLY
#elif defined(ONU_4PORT_AR8327)
static uint32 mcDnSwitchType = MC_DN_SWITCH_TYPE_ONU_4PORT_AR8327; /* the total number of groups */
#define SEND_SPECIFIC_QUERY_TO_UNI_PORT_DIRECTLY
#elif defined(ONU_4PORT_AR8306)
static uint32 mcDnSwitchType = MC_DN_SWITCH_TYPE_ONU_4PORT_AR8306; /* the total number of groups */
#endif

#ifndef MAX_MC_MSG_QUEUE
#define MAX_MC_MSG_QUEUE 1024
#endif

#ifndef MC_IGMP_MSG_SIZE
#define MC_IGMP_MSG_SIZE (sizeof(MC_IGMP_MSG_t) - sizeof(long))
#endif

#ifndef MC_TIMER_MSG_SIZE
#define MC_TIMER_MSG_SIZE (sizeof(MC_TIMER_MSG_t) - sizeof(long))
#endif

static MC_IGMP_MSG_t mcIgmpMsg;
static MC_TIMER_MSG_t mcTimerMsg;

static volatile int mcIgmpMsgId = -1;
static volatile int mcTimerMsgId = -1;

static long mcIgmpSequenceNo = 0;
static long mcTimerSequenceNo = 0;

static VOS_SEM_t mcMsgSem;// = NULL;
static VOS_SEM_t mcTimerSem;// = NULL;
static uint32 mcMsgLock = 0;
static uint32 mcTimerLock = 0;

extern int errno;

/***** DATA ALLOCATION *****/

/***** PUBLIC FUNCTIONS ****/

/******************************************************************************
*
*
* chksum:
*
* DESCRIPTION:
*     this function calculate the packet checksum
*
* INPUTS:
         dataptr: packet ointer
         len: packet length
*
* OUTPUTS:

* RETURN:
*
* SEE ALSO:
*/
static uint32
chksum(void* dataptr, int len)
{
  uint32 acc;
  uint16 *temp;
  temp = (uint16 *)dataptr;

  for (acc = 0; len > 1; len -= 2) {
    //acc += *((uint16 *)dataptr)++;
    acc += *temp++;
  }

  /* add up any odd byte */
  if (len == 1) {
    acc += htons((uint16) ((*(uint8 *) dataptr) & 0xff) << 8);
    printf("inet: chksum: odd byte %d\n", *(uint8 *) dataptr);
  }

  return acc;
}
/******************************************************************************
*
* op_lib_inet_chksum:
*
* DESCRIPTION:
*     this function calculate the packet checksum of 16bits
*
* INPUTS:
         dataptr: packet ointer
         len: packet length
*
* OUTPUTS:

* RETURN:
*
* SEE ALSO:
*/
static uint16
op_lib_inet_chksum(void* dataptr, uint16 len)
{
  uint32 acc;

  acc = chksum(dataptr, len);

  while (acc >> 16) {
    acc = (acc & 0xffff) + (acc >> 16);
  }
  return ~(acc & 0xffff);
}

int
pktPrintDebug(void* pkt, int len)
{
  int i;
  if(mcPktPrintDebug) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "------ethernet packet print------:len=%d.\r\n", len);
    for (i = 0; i < (len + 3) / 4; i++) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%08x ", *((unsigned int *) pkt + i));
      if (i != 0 && (i + 1) % 8 == 0) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n");
      }
    }
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n");
  }

  return 0;
}

int descPrintDebug(MC_PACKET_DESC_t *desc)
{
  uint8 daMacStr[SMALL_BUF_LEN];
  uint8 saMacStr[SMALL_BUF_LEN];
  uint8 destIpStr[SMALL_BUF_LEN];
  uint8 srcIpStr[SMALL_BUF_LEN];
  uint8 groupAddressStr[SMALL_BUF_LEN];

  vosMemSet(daMacStr, 0, sizeof(daMacStr));
  vosMemSet(saMacStr, 0, sizeof(saMacStr));
  vosMemSet(destIpStr, 0, sizeof(destIpStr));
  vosMemSet(srcIpStr, 0, sizeof(srcIpStr));
  vosMemSet(groupAddressStr, 0, sizeof(groupAddressStr));

  clStringGetByMac(daMacStr, desc->daMac);
  clStringGetByMac(saMacStr, desc->saMac);
  clStringGetByIp(destIpStr, desc->destIp);
  clStringGetByIp(srcIpStr, desc->srcIp);
  clStringGetByIp(groupAddressStr, desc->igmpGroupAdress);

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Desc: daMac=%s, saMac=%s, tagType=%d, vlan=%d, destIp=%s, srcIp=%s, groupAddress=%s, igmpType=%d, portId=%d.\r\n",
    daMacStr, saMacStr, desc->ingressTagType, desc->cVlan, destIpStr, srcIpStr, groupAddressStr, desc->igmpType, desc->ingressPortId);

  return 0;
}

/**
* @par Description
* This function initialize xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
void mcGroupSemLock(void)
{
  if(mcGroupLock == 0)
  {
      if (VOS_SEM_OK != vosSemCreate(&mcGroupSem, 0, 1))
      {
          printf("Group lock initial failed.\n");
          return;
      }
      mcGroupLock = 1;
  }
  vosSemTake(&mcGroupSem);

  return;
}

/**
* @par Description
* This function initialize xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
void mcGroupSemUnLock(void)
{
  if (mcGroupLock == 0)
  	return;

  vosSemGive(&mcGroupSem);

  return;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
static long mcIgmpMsgSeqNoGet(void)
{
    mcIgmpSequenceNo++;
    /* range in 1 ~ MAX_MC_MSG_QUEUE */
    if (mcIgmpSequenceNo > MAX_MC_MSG_QUEUE)
        mcIgmpSequenceNo = 1;
    return mcIgmpSequenceNo;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
void mcMsgSemLock(void)
{
  if (mcMsgLock == 0)
  {
      if (VOS_SEM_OK != vosSemCreate(&mcMsgSem, 0, 1))
      {
          printf("mcMsg lock initial failed.\n");
          return;
      }
      mcMsgLock = 1;
  }
  vosSemTake(&mcMsgSem);

  return;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
void mcMsgSemUnLock(void)
{
  if (mcMsgLock == 0)
  	return;

  vosSemGive(&mcMsgSem);

  return;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
void mcInputMsgSend(uint8 *ingressRawPkt, uint32 ingressRawPktLen, uint32 portId)
{
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcInputMsgSend: portId=%d.\r\n", portId);

    mcMsgSemLock();

    mcIgmpMsg.lSeqNo = mcIgmpMsgSeqNoGet();
    mcIgmpMsg.pBuff = vosAlloc(BIG_BUF_LEN);
    if (NULL == mcIgmpMsg.pBuff)
    {
        mcMsgSemUnLock();
        return;
    }
    mcControlStats.memAllocCount++;
    mcIgmpMsg.len = ingressRawPktLen;
    mcIgmpMsg.portId = portId;
    vosMemCpy(mcIgmpMsg.pBuff, ingressRawPkt, ingressRawPktLen);

    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcInputMsgSend: pBuff=0x%x len=%d portId=%d.\r\n", mcIgmpMsg.pBuff, mcIgmpMsg.len, mcIgmpMsg.portId);

    vosMsqSend(mcIgmpMsgId, (char *)&mcIgmpMsg, MC_IGMP_MSG_SIZE);

    mcMsgSemUnLock();
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
static long mcTimerMsgSeqNoGet(void)
{
    mcTimerSequenceNo++;
    /* range in 1 ~ MAX_MC_MSG_QUEUE */
    if (mcTimerSequenceNo > MAX_MC_MSG_QUEUE)
        mcTimerSequenceNo = 1;
    return mcTimerSequenceNo;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
void mcTimerSemLock(void)
{
  if (mcTimerLock == 0)
  {
      if (VOS_SEM_OK != vosSemCreate(&mcTimerSem, 0, 1))
      {
          printf("mcTimer lock initial failed.\n");
          return;
      }
      mcTimerLock = 1;
  }
  vosSemTake(&mcTimerSem);

  return;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
void mcTimerSemUnLock(void)
{
  if (mcTimerLock == 0)
  	return;

  vosSemGive(&mcTimerSem);

  return;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
void mcTimerMsgSend(MC_TIMER_MSG_TYPE_e msgType, MC_PACKET_DESC_t *desc)
{
    mcTimerSemLock();

    mcTimerMsg.lSeqNo = mcIgmpMsgSeqNoGet();
    mcTimerMsg.msgType = msgType;
    if (desc)
    {
        mcTimerMsg.desc = desc;
    }
    vosMsqSend(mcTimerMsgId, (char *)&mcTimerMsg, MC_TIMER_MSG_SIZE);

    mcTimerSemUnLock();
}

/**
* @par Description
* This function initialize the multicast vlan mapping structure.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcVlanMapInit(void)
{
  vosMemSet(mcVlanMap,0,sizeof(mcVlanMap));

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcVlanMapAdd(uint32 portId, uint32 vlanId)
{
  /*  */
  if (0 == mcVlanMap[vlanId])
  {
    mcVlanNum++;
  }
  /* 013987 */
  mcVlanMap[vlanId] |= 1<<portId; /* from bit1 to bit4, bit0 is not used */

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcVlanMapDel(uint32 portId, uint32 vlanId)
{
  mcVlanMap[vlanId] &= ~(1<<portId); /* from bit1 to bit4, bit0 is not used */
  /* */
  if (0 == mcVlanMap[vlanId])
  {
    if (0 < mcVlanNum)
    {
        mcVlanNum--;
    }
  }
  /* 013987 */
  return OPL_OK;
}

/*  */
uint32 mcVlanNumGet(void)
{
    return mcVlanNum;
}

OPL_STATUS mcVlanExist(uint32 vlanId)
{
  if (0 == mcVlanMap[vlanId]) {
    return OPL_FALSE;
  }

  return OPL_TRUE;
}
/* 013987 */
/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcVlanMapExist(uint32 portId, uint32 vlanId)
{
  if((mcVlanMap[vlanId]&1<<portId)!=0) {
    return OPL_TRUE;
  }
  else {
    return OPL_FALSE;
  }
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcTagOperPerPortInit(void)
{
  vosMemSet(mcTagOperPerPort,0,sizeof(mcTagOperPerPort));

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcTagOperPerPortSet(uint32 portId, MC_TAG_OPER_MODE_t tagOper)
{
  mcTagOperPerPort[portId] = tagOper;

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
MC_TAG_OPER_MODE_t mcTagOperPerPortGet(uint32 portId)
{
  return mcTagOperPerPort[portId];
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcTagTranslationTableInit(void)
{
  vosMemSet(mcTagTranslationTable,0,sizeof(mcTagTranslationTable));

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcTagTranslationTableAdd(uint32 portId, uint32 mcVid, uint32 userVid)
{
  uint32 i;

  for(i=0; i<MC_VLAN_TRANSLATION_ENTRY; i++) {
    if(mcTagTranslationTable[i].enable==OPL_ENABLE) { /* not blank entry */
      if(mcTagTranslationTable[i].portId == portId
        &&mcTagTranslationTable[i].mcVid == mcVid
        &&mcTagTranslationTable[i].userVid == userVid) { /* match */
	//modified by tanglin return OK--2010-07-18;
	   printf("***caution ***  there is a same entry \r\n");
	//  return OPL_EXIST;
    return OPL_OK;
      }
    }
  }

  for(i=0; i<MC_VLAN_TRANSLATION_ENTRY; i++) {
    if(mcTagTranslationTable[i].enable!=OPL_ENABLE) { /* blank entry */
      mcTagTranslationTable[i].enable = OPL_ENABLE;
      mcTagTranslationTable[i].portId = portId;
      mcTagTranslationTable[i].mcVid = mcVid;
      mcTagTranslationTable[i].userVid = userVid;

      return OPL_OK;
    }
  }

  /* table full */
  return OPL_FULL;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcTagTranslationTableDel(uint32 portId, uint32 mcVid, uint32 userVid)
{
  uint32 i;

  for(i=0; i<MC_VLAN_TRANSLATION_ENTRY; i++) {
    if(mcTagTranslationTable[i].enable==OPL_ENABLE) {
      if(mcTagTranslationTable[i].portId == portId
        &&mcTagTranslationTable[i].mcVid == mcVid
        &&mcTagTranslationTable[i].userVid == userVid) { /* match */
        vosMemSet(&mcTagTranslationTable[i], 0, sizeof(MC_VLAN_TRANSLATION_ENTRY_t));

        return OPL_OK;
      }
    }
  }

  /* not match */
  return OPL_NOT_EXIST;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcTagTranslationTableGet(uint32 portId, uint32 mcVid, uint32 *userVid)
{
  uint32 i;

  for(i=0; i<MC_VLAN_TRANSLATION_ENTRY; i++) {
    if(mcTagTranslationTable[i].enable==OPL_ENABLE) {
      if(mcTagTranslationTable[i].portId == portId
        &&mcTagTranslationTable[i].mcVid == mcVid) { /* match */
        *userVid = mcTagTranslationTable[i].userVid;
        return OPL_OK;
      }
    }
  }

  /* not match */
  return OPL_NOT_EXIST;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcTagTranslationTableClear(uint32 portId)
{
  uint32 i;

  for(i=0; i<MC_VLAN_TRANSLATION_ENTRY; i++) {
    if(mcTagTranslationTable[i].enable==OPL_ENABLE) {
      if(mcTagTranslationTable[i].portId == portId) { /* match */
        vosMemSet(&mcTagTranslationTable[i], 0, sizeof(MC_VLAN_TRANSLATION_ENTRY_t));
      }
    }
  }

  /* not match */
  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcRouterPortInit(void)
{
  vosMemSet(mcRouterPort,0,sizeof(mcRouterPort));


  #ifndef CTC_MULTICAST_SURPORT
  mcRouterPortAdd(PORT_PON_LOGIC);
  #else
  {
  extern int32 DRV_GetSwitchUplinkLPort(uint32 *puiLPort);
  uint32 uiLUplinkPort;
  int32  iRet;

  iRet = DRV_GetSwitchUplinkLPort(&uiLUplinkPort);
  if (NO_ERROR != iRet)
  {
    return OPL_ERROR;
  }

  (void)mcRouterPortAdd(uiLUplinkPort);
  }
  #endif


  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcRouterPortAdd(uint32 portId)
{
  uint32 i;

  for(i=0; i<MC_ROUTER_PORT_NUM; i++) {
    if(mcRouterPort[i].enable==OPL_ENABLE) {
      if(mcRouterPort[i].routerPortId == portId) {
        return OPL_EXIST;
      }
    }
  }

  for(i=0; i<MC_ROUTER_PORT_NUM; i++) {
    if(mcRouterPort[i].enable!=OPL_ENABLE) {
      mcRouterPort[i].enable = OPL_ENABLE;
      mcRouterPort[i].routerPortId = portId;

      return OPL_OK;
    }
  }

  return OPL_FULL;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcRouterPortDel(uint32 portId)
{
  uint32 i;

  for(i=0; i<MC_ROUTER_PORT_NUM; i++) {
    if(mcRouterPort[i].enable==OPL_ENABLE) {
      if(mcRouterPort[i].routerPortId == portId) {
        vosMemSet(&mcRouterPort[i], 0, sizeof(MC_ROUTER_PORT_ENTRY_t));
        return OPL_OK;
      }
    }
  }

  return OPL_NOT_EXIST;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcRouterPortExist(uint32 portId)
{
  uint32 i;

  for(i=0; i<MC_ROUTER_PORT_NUM; i++) {
    if(mcRouterPort[i].enable==OPL_ENABLE) {
      if(mcRouterPort[i].routerPortId == portId) {
        return OPL_TRUE;
      }
    }
  }

  return OPL_FALSE;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcGroupListInit(void)
{
  mcGroupList = NULL;
  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
MC_GROUP_t *mcGroupListAdd(uint32 hostPortId, uint8 *groupMac, uint32 groupAddress,
  uint32 mcVlan, uint32 state, uint32 bitmap)
{
  uint32 i;
  clList *list;
  MC_GROUP_t *node;
  uint32 compareResult;

  list = NULL;
  node = NULL;
  compareResult = OPL_FALSE;

  /* check if the entry exists */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;
    if(node) {
      GROUP_COMPARE(node, hostPortId, groupMac, groupAddress, mcVlan, bitmap, compareResult);
    }
    if(bitmap==compareResult) { /* equal */
      break;
    }
    compareResult = OPL_FALSE;
    list = list->next;
  }

  if(list==NULL) {
    /* add new node */
    node = NULL;
    node = vosAlloc(sizeof(MC_GROUP_t));
    mcControlStats.memAllocCount++;
    RETURN_VAL_IF_FAIL(node!=NULL, NULL);
    vosMemSet(node, 0, sizeof(MC_GROUP_t));

    mcGroupList = (clList *)clListInsert(mcGroupList, (void *)node);
    mcGroupTotalNum++;
  }

  /* update data structure */
  if(bitmap&MC_BITMAP_GDA_MAC) {
    vosMemCpy(node->groupMac, groupMac, MAC_ADDRESS_LENGTH);
  }

  node->groupAddress = groupAddress;
  node->mcVlan = mcVlan;
  node->hostPortId = hostPortId;
  node->state = state; 
  node->querycnt = 0;/*014041 */

  return node;
}

/**
* @par Description
* This function xxx. Please call xxx before this function is called.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcGroupListDel(uint32 hostPortId, uint8 *groupMac, uint32 groupAddress,
  uint32 mcVlan, uint32 bitmap)
{
  uint32 i;
  clList *list;
  MC_GROUP_t *node;
  uint32 compareResult;

  list = NULL;
  node = NULL;
  compareResult = OPL_FALSE;

  /* check if the entry exists */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;
    if(node) {
      GROUP_COMPARE(node, hostPortId, groupMac, groupAddress, mcVlan, bitmap, compareResult);
    }
    if(bitmap==compareResult) { /* equal */
      break;
    }
    compareResult = OPL_FALSE;
    list = list->next;
  }

  if(list==NULL) { /* not exist */
    return OPL_NOT_EXIST;
  }

  /* remove the exist node */
  mcGroupList = clListRemove(mcGroupList, (void *)node);
  vosFree(node);
  mcControlStats.memfreeCount++;
  mcGroupTotalNum--;

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
MC_GROUP_t *mcGroupListGet(uint32 hostPortId, uint8 *groupMac, uint32 groupAddress,
  uint32 mcVlan, uint32 bitmap)
{
  uint32 i;
  clList *list;
  MC_GROUP_t *node;
  uint32 compareResult;

  list = NULL;
  node = NULL;
  compareResult = OPL_FALSE;

  /* check if the entry exists */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;
    if(node) {
      GROUP_COMPARE(node, hostPortId, groupMac, groupAddress, mcVlan, bitmap, compareResult);
    }
    if(bitmap==compareResult) { /* equal */
      break;
    }
    compareResult = OPL_FALSE;
    list = list->next;
  }

  if(list!=NULL) {
    return node;
  }
  else
    return NULL;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
uint32 mcGroupNumPerPort(uint32 hostPortId)
{
  uint32 i;
  clList *list;
  MC_GROUP_t *node;

  uint32 num;

  list = NULL;
  node = NULL;
  num = 0;

  /* check if the entry exists */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;

    if(node->hostPortId==hostPortId) {
      num++;
    }

    list = list->next;
  }

  return num;
}

/*  */
/*get the group number used by the same mc vlan in one port.*/
uint32 mcGroupNumPerPortPerMcvid(uint32 hostPortId, uint32 mcVlan)
{
  uint32 i;
  clList *list;
  MC_GROUP_t *node;

  uint32 num;

  list = NULL;
  node = NULL;
  num = 0;

  /* check if the entry exists */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;

    if ((node->hostPortId==hostPortId) &&
        (node->mcVlan == mcVlan)) {
      num++;
    }

    list = list->next;
  }

  return num;
}

/*Remove mc vlan member for ctc controlable mulcast.*/
OPL_STATUS mcCtcMcvlanMemRmv(uint32 hostPortId, uint32 mcVlan)
{
  uint32 num;
  OPL_STATUS ret;
  
  num = mcGroupNumPerPortPerMcvid(hostPortId, mcVlan);
  /*The port member can not be deleted, 
    if the mc vlan is used by other mc groups in this port.*/
  if (0 == num)
  {
    ret = odmMcVlanMapDel(hostPortId, mcVlan);
    if (OPL_OK != ret)
    {
        return OPL_ERROR;
    }
  }

  return OPL_OK;
}
/* 014040 */
    
/**
* @par Description
* This function xxx. Please call xxx before this function is called.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcGroupListHostListEmpty(uint32 hostPortId, uint8 *groupMac, uint32 groupAddress,
  uint32 mcVlan, uint32 bitmap)
{
  uint32 i;
  clList *list;
  MC_GROUP_t *node;
  uint32 compareResult;

  list = NULL;
  node = NULL;
  compareResult = OPL_FALSE;

  /* check if the entry exists */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;
    if(node) {
      GROUP_COMPARE(node, hostPortId, groupMac, groupAddress, mcVlan, bitmap, compareResult);
    }
    if(bitmap==compareResult) { /* equal */
      break;
    }
    compareResult = OPL_FALSE;
    list = list->next;
  }

  if(list==NULL) { /* not exist */
    return OPL_NOT_EXIST;
  }

  if(!node->hostList) {
    return OPL_TRUE;
  }
  else {
    return OPL_FALSE;
  }
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcGroupListDestory()
{
  uint32 i;
  clList *list;
  MC_GROUP_t *node;

  list = NULL;
  node = NULL;

  /* check if the entry exists */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;
    mcHostListDestory(node);
    vosFree(node);
    mcControlStats.memfreeCount++;
    list = list->next;
  }
  clListFree(mcGroupList);
  mcGroupList = NULL;
  mcGroupTotalNum = 0;

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcGroupListSetHostAgingTime(uint32 hostPortId, uint8 *groupMac, uint32 groupAddress,
  uint32 mcVlan, uint32 bitmap, uint32 hostAgingTime)
{
  uint32 i;
  clList *list;
  MC_GROUP_t *node;
  uint32 compareResult;

  list = NULL;
  node = NULL;
  compareResult = OPL_FALSE;

  /* check if the entry exists */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;
    GROUP_COMPARE(node, hostPortId, groupMac, groupAddress, mcVlan, bitmap, compareResult);
    if(bitmap==compareResult) { /* equal */
      break;
    }
    compareResult = OPL_FALSE;
    list = list->next;
  }

  if(list==NULL) {
    return OPL_NOT_EXIST;
  }

  /* update data structure */
  node->hostAgingTime = hostAgingTime;

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcGroupListSetisLastReportRecevied(uint32 hostPortId, uint8 *groupMac, uint32 groupAddress,
  uint32 mcVlan, uint32 bitmap, uint32 isLastReportRecevied)
{
  uint32 i;
  clList *list;
  MC_GROUP_t *node;
  uint32 compareResult;

  list = NULL;
  node = NULL;
  compareResult = OPL_FALSE;

  /* check if the entry exists */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;
    GROUP_COMPARE(node, hostPortId, groupMac, groupAddress, mcVlan, bitmap, compareResult);
    if(bitmap==compareResult) { /* equal */
      break;
    }
    compareResult = OPL_FALSE;
    list = list->next;
  }

  if(list==NULL) {
    return OPL_NOT_EXIST;
  }

  /* update data structure */
  node->isLastReportRecevied = isLastReportRecevied;

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcGroupListSetisNonFastLeaveTimerStart(uint32 hostPortId, uint8 *groupMac, uint32 groupAddress,
  uint32 mcVlan, uint32 bitmap, uint32 isNonFastLeaveTimerStart)
{
  uint32 i;
  clList *list;
  MC_GROUP_t *node;
  uint32 compareResult;

  list = NULL;
  node = NULL;
  compareResult = OPL_FALSE;

  /* check if the entry exists */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;
    GROUP_COMPARE(node, hostPortId, groupMac, groupAddress, mcVlan, bitmap, compareResult);
    if(bitmap==compareResult) { /* equal */
      break;
    }
    compareResult = OPL_FALSE;
    list = list->next;
  }

  if(list==NULL) {
    return OPL_NOT_EXIST;
  }

  /* update data structure */
  node->isNonFastLeaveTimerStart = isNonFastLeaveTimerStart;

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcHostListInit(MC_GROUP_t *group)
{
  RETURN_VAL_IF_FAIL(group!=NULL, OPL_BAD_PARA);

  group->hostList = NULL;
  group->hostTotalNum = 0;

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
MC_HOST_t *mcHostListAdd(MC_GROUP_t *group, uint8 *saMac, uint32 srcIp, uint32 state,
  uint32 hostBitmap)
{
  uint32 i;
  clList *list;
  MC_HOST_t *node;
  uint32 compareResult;

  RETURN_VAL_IF_FAIL(group!=NULL, NULL);

  list = NULL;
  node = NULL;
  compareResult = OPL_FALSE;

  /* check if the entry exists */
  list = group->hostList;
  while (list) {
    node = (MC_HOST_t *)list->data;
    if(node) {
      HOST_COMPARE(node, saMac, srcIp, hostBitmap, compareResult);
    }
    if(hostBitmap==compareResult) { /* equal */
      break;
    }
    compareResult = OPL_FALSE;
    list = list->next;
  }

  if(list==NULL) {
    /* add new node */
    node = NULL;
    node = vosAlloc(sizeof(MC_HOST_t));
    mcControlStats.memAllocCount++;
    RETURN_VAL_IF_FAIL(node!=NULL, NULL);
    vosMemSet(node, 0, sizeof(*node));

    group->hostList = (clList *)clListInsert(group->hostList, (void *)node);
    group->hostTotalNum++;
  }

  /* update data structure */
  if(hostBitmap&MC_BITMAP_HOST_SA_MAC) {
    vosMemCpy(node->saMac, saMac, MAC_ADDRESS_LENGTH);
  }
  node->srcIp = srcIp;
  node->state = state;

  return node;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcHostListDel(MC_GROUP_t *group, uint8 *saMac, uint32 srcIp,
  uint32 hostBitmap)
{
  uint32 i;
  clList *list;
  MC_HOST_t *node;
  uint32 compareResult;

  RETURN_VAL_IF_FAIL(group!=NULL, OPL_BAD_PARA);

  list = NULL;
  node = NULL;
  compareResult = OPL_FALSE;

  /* check if the entry exists */
  list = group->hostList;
  while (list) {
    node = (MC_HOST_t *)list->data;
    if(node) {
      HOST_COMPARE(node, saMac, srcIp, hostBitmap, compareResult);
    }
    if(hostBitmap==compareResult) { /* equal */
      break;
    }
    compareResult = OPL_FALSE;
    list = list->next;
  }

  if(list==NULL) { /* not exist */
    return OPL_NOT_EXIST;
  }

  /* remove the exist node */
  group->hostList = clListRemove(group->hostList, (void *)node);
  vosFree(node);
  mcControlStats.memfreeCount++;
  group->hostTotalNum--;

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
MC_HOST_t *mcHostListGet(MC_GROUP_t *group, uint8 *saMac, uint32 srcIp,
  uint32 hostBitmap)
{
  uint32 i;
  clList *list;
  MC_HOST_t *node;
  uint32 compareResult;

  RETURN_VAL_IF_FAIL(group!=NULL, NULL);

  list = NULL;
  node = NULL;
  compareResult = OPL_FALSE;

  /* check if the entry exists */
  list = group->hostList;
  while (list) {
    node = (MC_HOST_t *)list->data;
    if(node) {
      HOST_COMPARE(node, saMac, srcIp, hostBitmap, compareResult);
    }
    if(hostBitmap==compareResult) { /* equal */
      break;
    }
    compareResult = OPL_FALSE;
    list = list->next;
  }

  if(list==NULL) { /* not exist */
    return NULL;
  }
  else {
    return node;
  }
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcHostListDestory(MC_GROUP_t *group)
{
  uint32 i;
  clList *list;
  MC_HOST_t *node;

  RETURN_VAL_IF_FAIL(group!=NULL, OPL_BAD_PARA);

  list = NULL;
  node = NULL;

  /* check if the entry exists */
  list = group->hostList;
  while (list) {
    node = (MC_HOST_t *)list->data;
    vosFree(node);
    mcControlStats.memfreeCount++;
    list = list->next;
  }
  clListFree(group->hostList);
  group->hostList = NULL;
  group->hostTotalNum = 0;

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcHostListSetHostAgingTime(MC_GROUP_t *group, uint8 *saMac, uint32 srcIp,
  uint32 hostBitmap, uint32 hostAgingTime)
{
  uint32 i;
  clList *list;
  MC_HOST_t *node;
  uint32 compareResult;

  RETURN_VAL_IF_FAIL(group!=NULL, OPL_BAD_PARA);

  list = NULL;
  node = NULL;
  compareResult = OPL_FALSE;

  /* check if the entry exists */
  list = group->hostList;
  while (list) {
    node = (MC_HOST_t *)list->data;
    HOST_COMPARE(node, saMac, srcIp, hostBitmap, compareResult);
    if(hostBitmap==compareResult) { /* equal */
      break;
    }
    compareResult = OPL_FALSE;
    list = list->next;
  }

  if(list==NULL) { /* not exist */
    return OPL_NOT_EXIST;
  }

  /* remove the exist node */
  node->hostAgingTime = hostAgingTime;

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcHostListSetisLastReportRecevied(MC_GROUP_t *group, uint8 *saMac, uint32 srcIp,
  uint32 hostBitmap, uint32 isLastReportRecevied)
{
  uint32 i;
  clList *list;
  MC_HOST_t *node;
  uint32 compareResult;

  RETURN_VAL_IF_FAIL(group!=NULL, OPL_BAD_PARA);

  list = NULL;
  node = NULL;
  compareResult = OPL_FALSE;

  /* check if the entry exists */
  list = group->hostList;
  while (list) {
    node = (MC_HOST_t *)list->data;
    HOST_COMPARE(node, saMac, srcIp, hostBitmap, compareResult);
    if(hostBitmap==compareResult) { /* equal */
      break;
    }
    compareResult = OPL_FALSE;
    list = list->next;
  }

  if(list==NULL) { /* not exist */
    return OPL_NOT_EXIST;
  }

  node->isLastReportRecevied = isLastReportRecevied;

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcHostListSetisNonFastLeaveTimerStart(MC_GROUP_t *group, uint8 *saMac, uint32 srcIp,
  uint32 hostBitmap, uint32 isNonFastLeaveTimerStart)
{
  uint32 i;
  clList *list;
  MC_HOST_t *node;
  uint32 compareResult;

  RETURN_VAL_IF_FAIL(group!=NULL, OPL_BAD_PARA);

  list = NULL;
  node = NULL;
  compareResult = OPL_FALSE;

  /* check if the entry exists */
  list = group->hostList;
  while (list) {
    node = (MC_HOST_t *)list->data;
    HOST_COMPARE(node, saMac, srcIp, hostBitmap, compareResult);
    if(hostBitmap==compareResult) { /* equal */
      break;
    }
    compareResult = OPL_FALSE;
    list = list->next;
  }

  if(list==NULL) { /* not exist */
    return OPL_NOT_EXIST;
  }

  /* remove the exist node */
  node->isNonFastLeaveTimerStart = isNonFastLeaveTimerStart;

  return OPL_OK;
}

/**
* @par Description
* This function xxx. Please call xxx before this function is called.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcGroupShow()
{
  uint32 i, j;
  clList *list, *hlist;
  MC_GROUP_t *node;
  MC_HOST_t *hnode;
  uint8 macStr[SMALL_BUF_LEN];
  uint8 ipStr[SMALL_BUF_LEN];

  i = j = 0;
  list = hlist = NULL;
  node = NULL;
  hnode = NULL;
  vosMemSet(macStr, 0, sizeof(macStr));
  vosMemSet(ipStr, 0, sizeof(ipStr));

  /* check if the entry exists */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;
    if(node->groupMac!=NULL)
      clStringGetByMac(macStr, node->groupMac);

    clStringGetByIp(ipStr, node->groupAddress);
    printf("G%d: GMac:%s, GAddr:%s, McVlan:%d, HostPort:%d, HostNum:%d, state:%d, AgingTime:%d, isLastReportRecevied:%d, isNonFastLeaveTimerStart:%d.\n",
      i++, macStr, ipStr, node->mcVlan,
      node->hostPortId, node->hostTotalNum, node->state, node->hostAgingTime,
      node->isLastReportRecevied, node->isNonFastLeaveTimerStart);

    hlist = node->hostList;
    while (hlist) {
      hnode = (MC_HOST_t *)hlist->data;

      clStringGetByIp(ipStr, hnode->srcIp);
      printf("  H%d: SrcIp:%s, state:%d, AgingTime:%d, isLastReportRecevied:%d, isNonFastLeaveTimerStart:%d.\n",
        j++, ipStr, hnode->state, hnode->hostAgingTime, hnode->isLastReportRecevied,
        hnode->isNonFastLeaveTimerStart);

      hlist = hlist->next;
    }

    j = 0;
    list = list->next;
  }

  return OPL_OK;
}
#if 1
  /*  */
int mcGroupInfoShow(UINT32 uiIndex, UINT32 *puiReIndex, CHAR *pszIgspInfoStr)
{
    uint8 i, j;
    clList *list;
    MC_GROUP_t *stNode;
    uint32 groupAddress;
    uint8 auiIpStr[16];
    uint32 ulStrLen = 0;
    uint8 macStr[20];
    uint8 tempGroupMac[MAC_ADDRESS_LENGTH]; 
    static UINT32 uiIgspIndex = 0;
    static MC_GROUP_t astMcGroupEntry[MC_MAX_GROUP_NUM];
    MC_GROUP_t *pstTempNode = NULL;
    static UINT32 uiNodeNum = 0;
    UINT32 uiTemp = 0;
    static uint32 mcIgspMode; 

    if((NULL == puiReIndex) || (NULL == pszIgspInfoStr))
    {
        return 0;
    }

    if(0 == uiIndex)/* 获取标题 */
    {
        memset(&astMcGroupEntry, 0, sizeof(astMcGroupEntry));
        pstTempNode = astMcGroupEntry;
        uiNodeNum = 0;
        uiIgspIndex = 0;
        switch(mcControlBlock.mcMode) 
        {
        case MC_MODE_SNOOPING:
            mcIgspMode = MC_MODE_SNOOPING;
            ulStrLen += sprintf((CHAR *)(pszIgspInfoStr + ulStrLen), "\rMulticast-mode:IGMP-snooping\n");
            mcGroupSemLock();
            list = mcGroupList;
            if (NULL != list) 
            {
                ulStrLen += sprintf((CHAR *)(pszIgspInfoStr + ulStrLen),
                    "\r\n Groupid  Mac Group Address   Ip Group Address  Vlan  UNI Port  HostNums\n"); 
                *puiReIndex = 1;
             } 
            else 
            {
                ulStrLen += sprintf((CHAR *)(pszIgspInfoStr + ulStrLen), "\r\nMulticast Group num is 0.\n"); 
                *puiReIndex = 0;
             }
             
             while (NULL != list)
            {
                stNode = (MC_GROUP_t *)list->data;
                memcpy(pstTempNode, stNode, sizeof(MC_GROUP_t));
                pstTempNode++;     
                uiNodeNum++;
                list = list->next;
            }
             mcGroupSemUnLock();
            
            break;
         case MC_MODE_CTC:
            mcIgspMode = MC_MODE_CTC;
            ulStrLen += sprintf((CHAR *)(pszIgspInfoStr + ulStrLen), "\rMulticast-mode: Multicast-control \n");
            mcGroupSemLock();
            list = mcGroupList;
            if (NULL != list) 
            {
                ulStrLen += sprintf((CHAR *)(pszIgspInfoStr + ulStrLen),
                "\r\n Groupid  Mac Group Address   Vlan   UNI Port  HostNums\n");   
                *puiReIndex = 1;
            } 
            else 
            {
                ulStrLen += sprintf((CHAR *)(pszIgspInfoStr + ulStrLen), "\r\nMulticast Group num is 0.\n");
                *puiReIndex = 0;
            }
            while (NULL != list)
            {
                stNode = (MC_GROUP_t *)list->data;
                memcpy(pstTempNode, stNode, sizeof(MC_GROUP_t));
                pstTempNode++;   
                uiNodeNum++;
                list = list->next;
            }
            mcGroupSemUnLock();
            
            break;
        default:    
            *puiReIndex = 0;
          break;
        }
        
        return ulStrLen; 
        
    }
    else
    {
        #define IGSP_GETINFONUM_ONCE      10
        #define OPCONN_IGSPINFO_LEN       1024
        uiTemp = 0;
        for(;uiIgspIndex < uiNodeNum; uiIgspIndex++)
        {
            uiTemp++;
            if(IGSP_GETINFONUM_ONCE < uiTemp || OPCONN_IGSPINFO_LEN < ulStrLen)
            {
                break;
            }

            if(mcIgspMode == MC_MODE_SNOOPING)
            {
                stNode = &astMcGroupEntry[uiIgspIndex];
                clMcMacGetByMcIp(tempGroupMac, stNode->groupAddress);
                clStringGetByMac(macStr, tempGroupMac);
                clStringGetByIp(auiIpStr, stNode->groupAddress);
                if (0 == stNode->mcVlan)
                {
                    ulStrLen += sprintf((CHAR *)(pszIgspInfoStr + ulStrLen), 
                        " %-7d  %-19s %-16s  %-5s %-7d   %-4d\n",
                        uiIgspIndex+1, macStr, auiIpStr, "N/A", stNode->hostPortId, stNode->hostTotalNum);
                } else
                {
                    ulStrLen += sprintf((CHAR *)(pszIgspInfoStr + ulStrLen), 
                        " %-7d  %-19s %-16s  %-5d %-7d   %-4d\n",
                        uiIgspIndex+1, macStr, auiIpStr, stNode->mcVlan, stNode->hostPortId, stNode->hostTotalNum);

                }
            }
            else
            {
                stNode = &astMcGroupEntry[uiIgspIndex];
                clStringGetByMac(macStr, stNode->groupMac);
                if (0 == stNode->mcVlan)
                {
                    ulStrLen += sprintf((CHAR *)(pszIgspInfoStr + ulStrLen), 
                                      " %-7d  %-19s %-5s  %-7d   %-4s\n",
                                      uiIgspIndex+1, macStr, "N/A", stNode->hostPortId, "N/A");
                } else
                {
                    ulStrLen += sprintf((CHAR *)(pszIgspInfoStr + ulStrLen), 
                                      " %-7d  %-19s %-5d  %-7d   %-4s\n",
                                      uiIgspIndex+1, macStr, stNode->mcVlan, stNode->hostPortId, "N/A");

                }

            }
        }
        
        if (uiIgspIndex >= uiNodeNum)
        {
            *puiReIndex = 0;
        }
        else
        {
            *puiReIndex = uiIgspIndex;
        }
        return ulStrLen; 
        
    }

      return ulStrLen; 
    
}
  #else
int mcGroupInfoShow(UCHAR *pRetBuf)
{
    uint8 i, j;
    clList *list;
    MC_GROUP_t *stNode;
    uint32 groupAddress;
    uint8 auiIpStr[16];
    uint32 ulStrLen = 0;
    uint8 macStr[20];
    uint8 tempGroupMac[MAC_ADDRESS_LENGTH];
   
    i = j = 1;
    stNode = NULL;
    vosMemSet(auiIpStr, 0, sizeof(auiIpStr));
    vosMemSet(macStr, 0, sizeof(macStr));

    switch(mcControlBlock.mcMode) 
    {
        case MC_MODE_SNOOPING:
            ulStrLen += sprintf((CHAR *)(pRetBuf + ulStrLen), "Multicast-mode:IGMP-snooping\n\n");
            mcGroupSemLock();
            list = mcGroupList;
            if (NULL != list) 
            {
                ulStrLen += sprintf((CHAR *)(pRetBuf + ulStrLen),
                    "Groupid  Mac Group Address       Ip Group Address        Vlan  UNI Port  HostNums\n");   
             } else {
                ulStrLen += sprintf((CHAR *)(pRetBuf + ulStrLen), "Multicast Group num is 0.\n");  
             }

            while (NULL != list)
            {
                stNode = (MC_GROUP_t *)list->data;
                clMcMacGetByMcIp(tempGroupMac, stNode->groupAddress);
                clStringGetByMac(macStr, tempGroupMac);
                
                clStringGetByIp(auiIpStr, stNode->groupAddress);
                ulStrLen += sprintf((CHAR *)(pRetBuf + ulStrLen), 
                    " %2d      %s       %-15s\t %-4d  %-3d       %-3d\n",
                    i++, macStr, auiIpStr, stNode->mcVlan, stNode->hostPortId, stNode->hostTotalNum);
            
                list = list->next;
            }
            mcGroupSemUnLock();
          break;
        case MC_MODE_CTC:
          ulStrLen += sprintf((CHAR *)(pRetBuf + ulStrLen), "Multicast-mode: Multicast-control \n\n");
          mcGroupSemLock();
          list = mcGroupList;
          if (NULL != list) 
          {
              ulStrLen += sprintf((CHAR *)(pRetBuf + ulStrLen),
                  "Groupid  Mac Group Address       Vlan  UNI Port  HostNums\n");   
           } else {
              ulStrLen += sprintf((CHAR *)(pRetBuf + ulStrLen), "Multicast Group num is 0.\n");  
           }
        
          while (NULL != list)
          {
              stNode = (MC_GROUP_t *)list->data;
              
             clStringGetByMac(macStr, stNode->groupMac);
                          
             ulStrLen += sprintf((CHAR *)(pRetBuf + ulStrLen), 
                  " %2d      %-14s\t %-4d  %-3d       %-4s\n",
                  i++, macStr, stNode->mcVlan, stNode->hostPortId, "N/A");

          
              list = list->next;
          }
          mcGroupSemUnLock();

          break;
        default:    
          //return OPL_NOT_SUPPORTED;
          break;
      }
    
      return ulStrLen;
}
  #endif
  /* 013989 */
/**
* @par Description
* This function initialize the multicast vlan mapping structure.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcControlStatsInit(void)
{
  vosMemSet(&mcControlStats,0,sizeof(mcControlStats));

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcControlBlockInit()
{
  uint32 i;

  vosMemSet(&mcControlBlock,0,sizeof(mcControlBlock));

  /* set default values */
  mcControlBlock.mcRecvEn = OPL_ENABLE;
  mcControlBlock.mcMode = MC_MODE_SNOOPING;
 
  #ifndef CTC_MULTICAST_SURPORT
  mcControlBlock.ctcControlType = MC_CTL_GDA_GDA_IP_VID;
  MC_BITMAP = CTC_CONTROL_BITMAP_GDA_IP_VLAN_ID;
  mcControlBlock.hostBitmap = MC_BITMAP_HOST_SRC_IP;
  mcControlBlock.leaveMode = MC_LEAVE_MODE_FAST;
  #else
  mcControlBlock.ctcControlType = MC_CTL_GDA_MAC;
  MC_BITMAP = CTC_CONTROL_BITMAP_GDA_MAC_VLAN_ID;
  mcControlBlock.hostBitmap = MC_BITMAP_HOST_SA_MAC;
  mcControlBlock.leaveMode = MC_LEAVE_MODE_NON_FAST_LEAVE;
  #endif
  
  mcControlBlock.hostAgingTime = IGMP_HOST_AGING_TIME_DEFAULT;
  mcControlBlock.lastQueryInterval = LAST_MEMBER_QUERY_INTERVAL_DEFAULT;
  mcControlBlock.lastQueryCount = LAST_MEMBER_QUERY_COUNT_DEFAULT;
  mcControlBlock.stpid = VLAN_DEFAULT_STPID;
  mcControlBlock.ctpid = VLAN_DEFAULT_CTPID;
  mcControlBlock.dnSwitchType = mcDnSwitchType;

  for(i=ODM_START_PORT_NUN; i<ODM_NUM_OF_PORTS+1; i++) {
   
    #if 0
    mcControlBlock.maxGroupNum[i] = IGMP_MAX_GROUP_NUMBER_PER_PORT;
    #else
    mcControlBlock.maxGroupNum[i] = ODM_MAX_GRUP_NUM_PER_PORT;
    #endif
    
  }

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
void mcControlBlockIsInitializedSet(uint32 mode)
{
  mcControlBlock.isInitialized = mode;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
uint32 mcControlBlockIsInitializedGet()
{
  return mcControlBlock.isInitialized;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMcControlBlockRecvEnSet(uint32 mcRecvEn)
{
  int ret;

  ret = 0;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMcControlBlockRecvEnSet: mcRecvEn=%d.\r\n", mcRecvEn);

  /* dal */
  if(mcRecvEn!=OPL_DISABLE) {
   
    #ifndef CTC_MULTICAST_SURPORT
    ret = dalMulticastEnable();
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
    #else
    {
    extern INT32 DRV_EnableSwitchMc(INT32 iEnable);
    ret = DRV_EnableSwitchMc(TRUE);
    if (NO_ERROR != ret)
    {
        mc_printf("\nfunc:%s,line:%d. Failed to enable multicast.\n",__FUNCTION__,__LINE__);
        return OPL_ERROR;
    }
    }
    #endif
    
  }
  else {

    #ifndef CTC_MULTICAST_SURPORT
    ret = dalMulticastDisable();
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
    #else
    {
    extern INT32 DRV_EnableSwitchMc(INT32 iEnable);
    ret = DRV_EnableSwitchMc(FALSE);
    if (NO_ERROR != ret)
    {
        mc_printf("\nfunc:%s,line:%d. Failed to enable multicast.\n",__FUNCTION__,__LINE__);
        OPL_ERROR;
    }
    }
    #endif
    
  }

  /* fal */
  mcControlBlock.mcRecvEn = mcRecvEn;

  /* odm */
  vosConfigUInt32Set(CFGFILE_MULTICAST, CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
    CFGKEY_MULTICAST_IGMP_MANAGEMENT_IGMP_MODE, mcControlBlock.mcRecvEn);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
uint32 mcControlBlockRecvEnGet()
{
  return mcControlBlock.mcRecvEn;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMcControlBlockModeSet(uint32 mcMode)
{
  /* fal */
  mcControlBlock.mcMode = mcMode;

  /* odm */
  vosConfigUInt32Set(CFGFILE_MULTICAST, CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
    CFGKEY_MULTICAST_IGMP_MANAGEMENT_MODE, mcControlBlock.mcMode);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
uint32 mcControlBlockModeGet()
{
  return mcControlBlock.mcMode;
}

OPL_STATUS odmMcControlBlockCtcControlTypeSet(uint32 ctcControlType)
{
  if(ctcControlType==mcControlBlock.ctcControlType) {
    return OPL_OK;
  }

  /* clear group table */
  odmMulticastControlEntryClear();

  /* fal */
  mcControlBlock.ctcControlType = ctcControlType;

  if(mcControlBlock.ctcControlType==MC_CTL_GDA_MAC) {
    MC_BITMAP = CTC_CONTROL_BITMAP_GDA_MAC;
  }
  if(mcControlBlock.ctcControlType==MC_CTL_GDA_MAC_VID) {
    MC_BITMAP = CTC_CONTROL_BITMAP_GDA_MAC_VLAN_ID;
  }
  if(mcControlBlock.ctcControlType==MC_CTL_GDA_GDA_SA_MAC) {
    MC_BITMAP = CTC_CONTROL_BITMAP_GDA_MAC_SA_MAC;
  }
  if(mcControlBlock.ctcControlType==MC_CTL_GDA_GDA_IP_VID) {
    MC_BITMAP = CTC_CONTROL_BITMAP_GDA_IP_VLAN_ID;
  }

  /* odm */
  vosConfigUInt32Set(CFGFILE_MULTICAST, CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
    CFGKEY_MULTICAST_IGMP_MANAGEMENT_CTC_CONTROL_TYPE, mcControlBlock.ctcControlType);

  return OPL_OK;
}

uint32 mcControlBlockCtcControlTypeGet()
{
  return mcControlBlock.ctcControlType;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMcControlBlockLeaveModeSet(uint32 leaveMode)
{
  /* fal */
  mcControlBlock.leaveMode = leaveMode;

  /* odm */
  vosConfigUInt32Set(CFGFILE_MULTICAST, CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
    CFGKEY_MULTICAST_IGMP_MANAGEMENT_LEAVE_MODE, mcControlBlock.leaveMode);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
uint32 mcControlBlockLeaveModeGet()
{
  return mcControlBlock.leaveMode;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMcControlBlockHostAgingTimeSet(uint32 hostAgingTime)
{
  /* fal */
  mcControlBlock.hostAgingTime = hostAgingTime;

  /* odm */
  vosConfigUInt32Set(CFGFILE_MULTICAST, CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
    CFGKEY_MULTICAST_IGMP_MANAGEMENT_HOST_AGING_TIME, mcControlBlock.hostAgingTime);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
uint32 mcControlBlockHostAgingTimeGet()
{
  return mcControlBlock.hostAgingTime;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMcControlBlockLastQueryIntervalSet(uint32 lastQueryInterval)
{
  /* fal */
  mcControlBlock.lastQueryInterval = lastQueryInterval;

  /* odm */
  vosConfigUInt32Set(CFGFILE_MULTICAST, CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
    CFGKEY_MULTICAST_IGMP_MANAGEMENT_LAST_QUERY_INTERVAL, mcControlBlock.lastQueryInterval);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
uint32 mcControlBlockLastQueryIntervalGet()
{
  return mcControlBlock.lastQueryInterval;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMcControlBlockLastQueryCountSet(uint32 lastQueryCount)
{
  /* fal */
  mcControlBlock.lastQueryCount = lastQueryCount;

  /* odm */
  vosConfigUInt32Set(CFGFILE_MULTICAST, CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
    CFGKEY_MULTICAST_IGMP_MANAGEMENT_LAST_QUERY_COUNT, mcControlBlock.lastQueryCount);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
uint32 mcControlBlockLastQueryCountGet()
{
  return mcControlBlock.lastQueryCount;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMcVlanMapAdd(uint32 portId, uint32 vlanId)
{
  char portIdStr[SMALL_BUF_LEN];
  char newStr[BIG_BUF_LEN];
  char vlanIdStr[SMALL_BUF_LEN];
  char *configFileStr, *p;
  int ret;

  vosMemSet(portIdStr, 0, sizeof(portIdStr));
  vosMemSet(newStr, 0, sizeof(newStr));
  vosMemSet(vlanIdStr, 0, sizeof(vlanIdStr));
  configFileStr = p = NULL;
  ret = 0;

  ////20100327_XFAN_ADD_BEGIN
  /************************** check if it is the same tagOper **************************/
  if(mcVlanMapExist(portId, vlanId)==OPL_TRUE) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, vlan exists, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, vlanId);
    return OPL_OK;
  }
  ////20100327_XFAN_ADD_END

  /* dal */
  ret = dalMcVlanMapAdd(portId, vlanId);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  /* odm */
  vosSprintf(portIdStr, "%s%d", CFGSECTION_MULTICAST_IGMP_PORT, portId);
  configFileStr = (char *)vosConfigValueGet(CFGFILE_MULTICAST, portIdStr,
      CFGKEY_MULTICAST_IGMP_PORT_MULTICAST_VLAN, NULL);

  if(configFileStr == NULL) {
    vosSprintf(newStr, "%d", vlanId);
  }
  else {
    /* check if the vlan exists */
    vosSprintf(vlanIdStr, "%d", vlanId);
    p = strstr(configFileStr, vlanIdStr);
    if(p != NULL) { /* the vlan exists */
      return OPL_EXIST;
    }

    vosSprintf(newStr, "%s,%d", configFileStr, vlanId);
  }

  vosConfigValueSet(CFGFILE_MULTICAST, portIdStr, CFGKEY_MULTICAST_IGMP_PORT_MULTICAST_VLAN,
      newStr);

  /* fal */
  ret = mcVlanMapAdd(portId, vlanId);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  return OPL_OK;
}

/* 014531 */
OPL_STATUS mcGroupListDelbyPort(uint32 hostPortId)
{
 
  uint32 i;
  clList *list;
  MC_GROUP_t *node;
  uint32 compareResult;

  list = NULL;
  node = NULL;
  compareResult = OPL_FALSE;

  /* check if the entry exists */
  mcGroupSemLock();
  list = mcGroupList;
  while (list) 
  {
    node = (MC_GROUP_t *)list->data;
    if (hostPortId == node->hostPortId)
    {
        mcHostListDestory(node);
        mcGroupList = clListRemove(mcGroupList, (void *)node);
        vosFree(node);
        mcControlStats.memfreeCount++;
        mcGroupTotalNum--; 
        
    }
    list = list->next;
  }
  mcGroupSemUnLock();
  
  return OPL_OK;
}
/*  */

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMcVlanMapDel(uint32 portId, uint32 vlanId)
{
  char portIdStr[SMALL_BUF_LEN];
  char newStr[BIG_BUF_LEN];
  char vlanIdStr[SMALL_BUF_LEN];
  char *configFileStr, *p, *p_next;
  int ret;
  int if_delete;
  int isVlanPortExist;
  uint32 i;

  vosMemSet(portIdStr, 0, sizeof(portIdStr));
  vosMemSet(newStr, 0, sizeof(newStr));
  vosMemSet(vlanIdStr, 0, sizeof(vlanIdStr));
  configFileStr = p = p_next = NULL;
  ret = 0;
  if_delete = OPL_FALSE;
  isVlanPortExist = OPL_FALSE;

  /* fal */
  ret = mcVlanMapDel(portId, vlanId);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  /* dal */
  ret = dalMcVlanMapDel(portId, vlanId);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  /* odm */
  vosSprintf(portIdStr, "%s%d", CFGSECTION_MULTICAST_IGMP_PORT, portId);
  configFileStr = (char *)vosConfigValueGet(CFGFILE_MULTICAST, portIdStr,
      CFGKEY_MULTICAST_IGMP_PORT_MULTICAST_VLAN, NULL);

  if(configFileStr==NULL) {
    return OPL_ERROR;
  }

  vosStrCpy(newStr, configFileStr);

  vosSprintf(vlanIdStr, "%d", vlanId);

  p = strstr(newStr, vlanIdStr);
  if(p == NULL) {
    return ERROR;
  }
  p_next = strstr(p, ",");
  if(p_next == NULL) { /* the deleted vlan is the last vlan */
    if(p == newStr) { /* only one vlan in this string */
      if_delete = OPL_TRUE;
    }
    else { /* two or more vlans in this string */
      p--;
      *p = '\0';
    }
  }
  else {
    p_next++;
    while(*(char *)p_next != NULL) {
      *(p++) = *(p_next++);
    }
    *p = '\0';
  }

  if(if_delete == OPL_TRUE)
    vosConfigKeyDelete(CFGFILE_MULTICAST, portIdStr, CFGKEY_MULTICAST_IGMP_PORT_MULTICAST_VLAN);
  else
    vosConfigValueSet(CFGFILE_MULTICAST, portIdStr, CFGKEY_MULTICAST_IGMP_PORT_MULTICAST_VLAN, newStr);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMcControlBlockMaxGroupNumSet(uint32 portId, uint32 maxGroup)
{
  char portIdStr[SMALL_BUF_LEN];

  vosMemSet(portIdStr, 0, sizeof(portIdStr));

  /* fal */

  #if 0
  mcControlBlock.maxGroupNum[portId] = maxGroup;
  #else
  if (ODM_MAX_GRUP_NUM_PER_PORT < maxGroup)
  {
    return OPL_ERROR;
  }
  
  mcControlBlock.maxGroupNum[portId] = maxGroup;
  #endif
  

  /* odm */
  vosSprintf(portIdStr, "%s%d", CFGSECTION_MULTICAST_IGMP_PORT, portId);
  vosConfigUInt32Set(CFGFILE_MULTICAST, portIdStr,
    CFGKEY_MULTICAST_IGMP_PORT_MAX_GROUP_NUM,
    mcControlBlock.maxGroupNum[portId]);

  odmMulticastControlEntryNumSet(portId, maxGroup);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
uint32 mcControlBlockmaxGroupNumGet(uint32 portId)
{
  return mcControlBlock.maxGroupNum[portId];
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMcTagOperSet(uint32 portId, MC_TAG_OPER_MODE_t tagOper)
{
  int ret;
  uint32 vlanMode;
  char portIdStr[SMALL_BUF_LEN];

  ret = 0;
  vlanMode = 0;
  vosMemSet(portIdStr, 0, sizeof(portIdStr));

  ////20100327_XFAN_ADD_BEGIN
  /************************** check if it is the same tagOper **************************/
  if(tagOper==mcTagOperPerPortGet(portId)) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, tagOper does not change, tagOper=%d.\r\n", __FUNCTION__, __LINE__, tagOper);
    return OPL_OK;
  }
  ////20100327_XFAN_ADD_END

  /************************** vlan translation action **************************/
  if(tagOper!=MC_TAG_OPER_MODE_TRANSLATION) {
   
    #ifndef CTC_MULTICAST_SURPORT
    ret = odmMcTagTranslationClear(portId);
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
    #endif
   
  }

  /* dal */
  ret = dalMcTagOperSet(portId, tagOper);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  /* fal */
  ret = mcTagOperPerPortSet(portId, tagOper);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  /* odm */
  vosSprintf(portIdStr, "%s%d", CFGSECTION_MULTICAST_IGMP_PORT, portId);
  vosConfigUInt32Set(CFGFILE_MULTICAST, portIdStr, CFGKEY_MULTICAST_IGMP_PORT_TAGSTRIP,
      tagOper);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMcTagTranslationSet(uint32 portId, uint32 mcVid, uint32 userVid)
{
  int i,ret;
  char portIdStr[SMALL_BUF_LEN];
  char mcVidStr[SMALL_BUF_LEN];
  uint32 oldUserVid;

  ret = 0;
  vosMemSet(portIdStr, 0, sizeof(portIdStr));
  vosMemSet(mcVidStr, 0, sizeof(mcVidStr));

  if(mcTagOperPerPortGet(portId)!=MC_TAG_OPER_MODE_TRANSLATION) {
    /* exist */
    return OPL_ERROR;
  }

#if 0 /* Bug3024 do this after checking the exising mcvid. */
  if(mcTagTranslationTableCheck(portId,mcVid,userVid)!=OPL_OK){
      return OPL_ERROR;
  }
  #endif

	/* checking the existing mcvid or uservid. if the translation vlan list has existed, it returns ok. Others, if the mcvid or uservid has existed, it returns the error code. */
	for(i=0; i<MC_VLAN_TRANSLATION_ENTRY; i++) {
		if (mcTagTranslationTable[i].enable!=OPL_ENABLE ||
			mcTagTranslationTable[i].portId != portId)
			continue;
		if (mcTagTranslationTable[i].userVid == userVid){
		   if (mcTagTranslationTable[i].mcVid == mcVid){
		       return OPL_OK;
		   }
		   printf("userVid which equal to existed userVid in the translation table is not allowed \r\n");
	   	   return OPL_ERROR; 
		}
		else if (mcTagTranslationTable[i].userVid == mcVid) {
	      printf("mcVid which equal to existed userVid in the translation table is not allowed \r\n");
	      return OPL_ERROR;
		 }
		else if(mcTagTranslationTable[i].mcVid == userVid){
	      printf("userVid which equal to existed mcVid in the translation table is not allowed \r\n");
	   	  return OPL_ERROR;
	  	}
	}


  /* Seach translation entry with same mcVid */
  if(mcTagTranslationTableGet(portId, mcVid, &oldUserVid) == OPL_OK)
  {
    if(oldUserVid != userVid)
    {
        /* dal */
        ret = dalMcTagTranslationDel(portId, mcVid, oldUserVid);
        RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

        /* fal */
        ret = mcTagTranslationTableDel(portId, mcVid, oldUserVid);
        RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
    }
    else
    {
        return OPL_OK;
    }
  }

  /* dal */
  /* TBD */
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMcTagTranslationSet: portId=%d, mcVid=%d, userVid=%d.\r\n",
    portId, mcVid, userVid);
  ret = dalMcTagTranslationAdd(portId, mcVid, userVid);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  /* fal */
  ret = mcTagTranslationTableAdd(portId, mcVid, userVid);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  /* odm */
  vosSprintf(portIdStr, "%s%d", CFGSECTION_MULTICAST_IGMP_PORT, portId);
  vosSprintf(mcVidStr, "%s%d", CFGKEY_MULTICAST_IGMP_PORT_TAGSTRIP_TRANSLATION_VLAN, mcVid);

  vosConfigUInt32Set(CFGFILE_MULTICAST, portIdStr, mcVidStr, userVid);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMcTagTranslationClear(uint32 portId)
{
  int ret;
  char portIdStr[SMALL_BUF_LEN];
  char mcVidStr[SMALL_BUF_LEN];
  uint32 i;

  ret = 0;
  vosMemSet(portIdStr, 0, sizeof(portIdStr));
  vosMemSet(mcVidStr, 0, sizeof(mcVidStr));

  if(mcTagOperPerPortGet(portId)!=MC_TAG_OPER_MODE_TRANSLATION) {
    /* exist */
    return OPL_OK;
  }

  /* dal */
  /* TBD */
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMcTagTranslationClear: portId=%d.\r\n", portId);
  for(i=0; i<MC_VLAN_TRANSLATION_ENTRY; i++) {
    if(mcTagTranslationTable[i].enable==OPL_ENABLE) {
      if(mcTagTranslationTable[i].portId == portId) { /* match */
        MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMcTagTranslationSet: portId=%d, mcVid=%d, userVid=%d.\r\n",
          portId, mcTagTranslationTable[i].mcVid, mcTagTranslationTable[i].userVid);
        ret = dalMcTagTranslationDel(portId, mcTagTranslationTable[i].mcVid, mcTagTranslationTable[i].userVid);
        RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      }
    }
  }

  /* fal */
  ret = mcTagTranslationTableClear(portId);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  /* odm */
  vosSprintf(portIdStr, "%s%d", CFGSECTION_MULTICAST_IGMP_PORT, portId);
  for(i=1; i<OP_MAX_VLAN_NUMBER; i++) {
    vosSprintf(mcVidStr, "%s%d", CFGKEY_MULTICAST_IGMP_PORT_TAGSTRIP_TRANSLATION_VLAN, i);
    vosConfigKeyDelete(CFGFILE_MULTICAST, portIdStr, mcVidStr);
  }

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMcGroupAdd(uint32 hostPortId, uint8 *groupMac, uint32 groupAddress,
  uint32 mcVlan, uint32 state, uint32 bitmap)
{
  int ret;
  MC_GROUP_t *group;

  ret = 0;
  group = NULL;
//printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
  /* dal */
  ret = dalMcGroupAdd(hostPortId, groupMac, groupAddress, mcVlan, bitmap);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  /* fal */
  group = mcGroupListAdd(hostPortId, groupMac, groupAddress, mcVlan, state, bitmap);
  RETURN_VAL_IF_FAIL(group != NULL, OPL_ERROR);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMcGroupDel(uint32 hostPortId, uint8 *groupMac, uint32 groupAddress,
  uint32 mcVlan, uint32 bitmap)
{
  int ret;

  ret = 0;

  /* dal */
  ret = dalMcGroupDel(hostPortId, groupMac, groupAddress,
    mcVlan, bitmap);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  /* fal */
  ret = mcGroupListDel(hostPortId, groupMac, groupAddress,
    mcVlan, bitmap);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  return OPL_OK;
}

/**
* @par Description
* This function xxxs.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
void mcHostAgingTimerSet(uint32 hostAgingTime)
{
  int ret;
  clList *list, *listNext, *hlist, *hlistNext;
  MC_GROUP_t *node;
  MC_HOST_t *hnode;

  ret = 0;
  list = listNext = hlist = hlistNext = NULL;
  node = NULL;
  hnode = NULL;

  mcGroupSemLock();

  /* poll the group list */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;
    listNext = list->next;
    hlist = node->hostList;
    while (hlist) {
      hnode = (MC_HOST_t *)hlist->data;
      hlistNext = hlist->next;

      hnode->hostAgingTime = hostAgingTime;

      hlist = hlistNext;
    }

    list = listNext;
  }

  mcGroupSemUnLock();

  return;
}

/**
* @par Description
* This function xxxs.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
void mcHostAgingTimerSnoopingHandler()
{
  int ret;
  clList *list, *listNext, *hlist, *hlistNext;
  MC_GROUP_t *node;
  MC_HOST_t *hnode;

  if(!mcControlBlock.hostAgingTime) { /* need not aging */
    return;
  }

  ret = 0;
  list = listNext = hlist = hlistNext = NULL;
  node = NULL;
  hnode = NULL;

  mcGroupSemLock();

  /* poll the group list */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;
    listNext = list->next;
    hlist = node->hostList;
    while (hlist) {
      hnode = (MC_HOST_t *)hlist->data;
      hlistNext = hlist->next;
      if(!hnode->hostAgingTime) { /* timeout */
        ret = mcHostListDel(node, hnode->saMac, hnode->srcIp, MC_HOST_BITMAP);
        hnode = NULL;

        if(!node->hostList) { /* the group has no member */
          ret = odmMcGroupDel(node->hostPortId, node->groupMac, node->groupAddress,
            node->mcVlan, MC_BITMAP);

          node = NULL;
        }
      }
      else {
        hnode->hostAgingTime--;
      }
      hlist = hlistNext;
    }

    list = listNext;
  }

  mcGroupSemUnLock();

  return;
}

/**
* @par Description
* This function xxxs.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
void mcHostAgingTimerCtcHandler()
{
  if(!mcControlBlock.hostAgingTime) { /* need not aging */
    return;
  }

  return;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
void mcHostAgingTimerInit(uint32 mcMode, uint32 millisecond)
{

  mcHostAgingTimerHandler hander;
  MC_TIMER_MSG_TYPE_e msgType;

  if(mcHostAgingTimer) {
    vosTimerCancel(mcHostAgingTimer);
    mcHostAgingTimer = NULL;
  }

  if (!mcHostAgingTimer) {
      mcHostAgingTimer = vosTimerGetFree();
  }

  switch(mcMode) {
    case MC_MODE_SNOOPING:
      hander = mcTimerMsgSend;
      msgType = MC_HOST_AGING_TIMER_SNOOPING;
      break;
    case MC_MODE_CTC:
      hander = mcTimerMsgSend;
      msgType = MC_HOST_AGING_TIMER_CTC;
      break;
    default:
      hander = NULL;
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Not support, mcMode=%d.\r\n", mcMode);
      break;
  }

  if (mcHostAgingTimer&&hander!=NULL) {
      vosTimerStart(mcHostAgingTimer, REOCURRING_TIMER, millisecond,
        (FUNCPTR)hander,(uint32)msgType,0,0,0,0,0,0,0);
  }

  return;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcInitFromConfigFile()
{
  uint32 i, j, userVid;
  int ret;
  char portIdStr[SMALL_BUF_LEN];
  char vlanIdStr[SMALL_BUF_LEN];
  char tempStr[BIG_BUF_LEN];
  char transVlanStr[SMALL_BUF_LEN];
  char *vlanMapFromFile, *p, *outerBuf;
  uint32 vlanId;
  MC_TAG_OPER_MODE_t tagOperTmp;
  uint32 vlanMode;

  ret = 0;
  vosMemSet(portIdStr, 0, sizeof(portIdStr));
  vosMemSet(vlanIdStr, 0, sizeof(vlanIdStr));
  vosMemSet(tempStr, 0, sizeof(tempStr));
  vosMemSet(transVlanStr, 0, sizeof(transVlanStr));
  vlanMapFromFile = p = outerBuf = NULL;
  vlanId = 0;
  tagOperTmp = 0;
  vlanMode = 0;

  mcControlBlock.mcRecvEn = vosConfigUInt32Get(CFGFILE_MULTICAST,
    CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
    CFGKEY_MULTICAST_IGMP_MANAGEMENT_IGMP_MODE, mcControlBlock.mcRecvEn);

  if(mcControlBlock.mcRecvEn) {
    
    #ifndef CTC_MULTICAST_SURPORT
    dalMulticastEnable();
    #else
    {
    extern INT32 DRV_EnableSwitchMc(INT32 iEnable);
    ret = DRV_EnableSwitchMc(TRUE);
    if (NO_ERROR != ret)
    {
        mc_printf("\nfunc:%s,line:%d. Failed to enable multicast.\n",__FUNCTION__,__LINE__);
        return OPL_ERROR;
    }
    }
    #endif
 
    
  }
  else {
   
    #ifndef CTC_MULTICAST_SURPORT
    dalMulticastDisable();
    #else
    {
    extern INT32 DRV_EnableSwitchMc(INT32 iEnable);
    ret = DRV_EnableSwitchMc(FALSE);
    if (NO_ERROR != ret)
    {
        mc_printf("\nfunc:%s,line:%d. Failed to enable multicast.\n",__FUNCTION__,__LINE__);
        return OPL_ERROR;
    }
    }
    #endif

  }

  mcControlBlock.mcMode = vosConfigUInt32Get(CFGFILE_MULTICAST,
    CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
    CFGKEY_MULTICAST_IGMP_MANAGEMENT_MODE, mcControlBlock.mcMode);

  if(mcControlBlock.mcMode==MC_MODE_SNOOPING) {
    mcControlBlock.hostBitmap = MC_BITMAP_HOST_SRC_IP;
  }
  if(mcControlBlock.mcMode==MC_MODE_CTC) {
    mcControlBlock.hostBitmap = 0;
  }
  mcHostAgingTimerInit(mcControlBlock.mcMode, 1000); /* 1s */

  mcControlBlock.ctcControlType = vosConfigUInt32Get(CFGFILE_MULTICAST,
    CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
    CFGKEY_MULTICAST_IGMP_MANAGEMENT_CTC_CONTROL_TYPE, mcControlBlock.ctcControlType);

  if(mcControlBlock.mcMode==MC_MODE_SNOOPING) {
    MC_BITMAP = CTC_CONTROL_BITMAP_GDA_IP_VLAN_ID;
  }
  if(mcControlBlock.mcMode==MC_MODE_CTC) {
    if(mcControlBlock.ctcControlType==MC_CTL_GDA_MAC) {
      MC_BITMAP = CTC_CONTROL_BITMAP_GDA_MAC;
    }
    if(mcControlBlock.ctcControlType==MC_CTL_GDA_MAC_VID) {
      MC_BITMAP = CTC_CONTROL_BITMAP_GDA_MAC_VLAN_ID;
    }
    if(mcControlBlock.ctcControlType==MC_CTL_GDA_GDA_SA_MAC) {
     MC_BITMAP = CTC_CONTROL_BITMAP_GDA_MAC_SA_MAC;
    }
    if(mcControlBlock.ctcControlType==MC_CTL_GDA_GDA_IP_VID) {
      MC_BITMAP = CTC_CONTROL_BITMAP_GDA_IP_VLAN_ID;
    }
  }

  mcControlBlock.leaveMode = vosConfigUInt32Get(CFGFILE_MULTICAST,
    CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
    CFGKEY_MULTICAST_IGMP_MANAGEMENT_LEAVE_MODE, mcControlBlock.leaveMode);

  mcControlBlock.hostAgingTime = vosConfigUInt32Get(CFGFILE_MULTICAST,
    CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
    CFGKEY_MULTICAST_IGMP_MANAGEMENT_HOST_AGING_TIME, mcControlBlock.hostAgingTime);

  mcControlBlock.lastQueryInterval = vosConfigUInt32Get(CFGFILE_MULTICAST,
    CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
    CFGKEY_MULTICAST_IGMP_MANAGEMENT_LAST_QUERY_INTERVAL, mcControlBlock.lastQueryInterval);

  mcControlBlock.lastQueryCount = vosConfigUInt32Get(CFGFILE_MULTICAST,
    CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
    CFGKEY_MULTICAST_IGMP_MANAGEMENT_LAST_QUERY_COUNT, mcControlBlock.lastQueryCount);

  for(i = ODM_START_PORT_NUN; i < ODM_NUM_OF_PORTS+1; i++) {
    vosSprintf(portIdStr, "%s%d", CFGSECTION_MULTICAST_IGMP_PORT, i);

    /************************ multicast vlan ************************/
    vlanMapFromFile = (char *)vosConfigValueGet(CFGFILE_MULTICAST, portIdStr,
        CFGKEY_MULTICAST_IGMP_PORT_MULTICAST_VLAN, NULL);
    if(vlanMapFromFile != NULL) {
      vosStrCpy(tempStr, vlanMapFromFile);
      p = strtok_r(tempStr, ",", &outerBuf);
      if(p != NULL) {
        if((vlanId=atol(p)) > 0) {
          /* dal */
          ret = dalMcVlanMapAdd(i, vlanId);
          RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

          ret = mcVlanMapAdd(i, vlanId);
          RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
        }
      }

      while((p = strtok_r(NULL, ",", &outerBuf))) {
        if(p != NULL) {
          if((vlanId=atol(p)) > 0) {
            /* dal */
            ret = dalMcVlanMapAdd(i, vlanId);
            RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

            /* fal */
            ret = mcVlanMapAdd(i, vlanId);
            RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
          }
        }
      }
    }

    /************************ tag oper ************************/
    /* odm */
    tagOperTmp = vosConfigUInt32Get(CFGFILE_MULTICAST, portIdStr,
      CFGKEY_MULTICAST_IGMP_PORT_TAGSTRIP, MC_TAG_OPER_MODE_TRANSPARENT);

    /* dal */
    ret = dalMcTagOperSet(i, tagOperTmp);
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

    /* fal */
    ret = mcTagOperPerPortSet(i, tagOperTmp);
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

    /************************ vlan translation ************************/
    for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
      vosSprintf(transVlanStr, "%s%d", CFGKEY_MULTICAST_IGMP_PORT_TAGSTRIP_TRANSLATION_VLAN, j);
      userVid = vosConfigUInt32Get(CFGFILE_MULTICAST, portIdStr, transVlanStr, 0);
      if(userVid > 0) { /* match a line */
        /* dal */
        /* TBD */
        //ret = odmPortVttEntryAdd(i, 0, userVid, 0, j);
        ret = dalMcTagTranslationAdd(i, j, userVid);
        RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

        /* fal */
        ret = mcTagTranslationTableAdd(i, j, userVid);
        RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      }
    }

    /************************ max group ************************/
    mcControlBlock.maxGroupNum[i] = vosConfigUInt32Get(CFGFILE_MULTICAST, portIdStr,
      CFGKEY_MULTICAST_IGMP_PORT_MAX_GROUP_NUM,
      mcControlBlock.maxGroupNum[i]);
  }

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcCtcPacketSetVlan(uint8 *packet, uint32 *packetLength, uint32 *tagType, uint32 tpid, uint32 vlanId)
{
  uint32 copyLength;
  eth_header_with_sgl_tag_t *sglEthHeader;

  sglEthHeader = NULL;

  /************************** untag packet **************************/
  if(*tagType==TAG_TYPE_UNTAG) {
    copyLength = *packetLength;

    while(copyLength-->12) {
      *(packet+copyLength+3) = *(packet+copyLength-1);
    }

    *packetLength = *packetLength+4;
    *tagType = TAG_TYPE_SINGLE_TAG;
  }

  /************************** single tag packet **************************/
  sglEthHeader = (eth_header_with_sgl_tag_t *)packet;
  sglEthHeader->vlan_tag.type = tpid;
  sglEthHeader->vlan_tag.vid = vlanId;
  sglEthHeader->vlan_tag.pri =0;  /* fix bug3598 mcCtcPacketSetVlan only called by mcCtcLeaveAction and mcCtcJoinAction 
                                                           according to CTC,set the forwarding ctc control join and leave packet priority field to 0 */
  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcSnoopingQueryAction(MC_PACKET_DESC_t *desc)
{
  uint32 i;
  int ret;
  uint32 mcVlanExist;
  uint8 macStr[SMALL_BUF_LEN];

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcSnoopingQueryAction, enter.\r\n");

  mcVlanExist = OPL_FALSE;
  vosMemSet(macStr, 0, sizeof(macStr));

  /************************** check multicast vlan **************************/
  if(desc->ingressTagType==TAG_TYPE_UNTAG) {
    mcControlStats.otherRouterRxUnknownCount++;
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping query: no tag, dropped.\r\n");
    return OPL_ERROR;
  }
  else {
    for(i=ODM_START_PORT_NUN; i<ODM_NUM_OF_PORTS+1; i++) {
      if(OPL_TRUE==mcVlanMapExist(i, desc->cVlan)) {
        mcVlanExist = OPL_TRUE;
      }
    }
    if(mcVlanExist==OPL_FALSE) {
      mcControlStats.otherRouterRxUnknownCount++;
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping query: multicast vlan mismatch, dropped.\r\nportId=%d, vlan=%d.\r\n",
        desc->ingressPortId, desc->cVlan);
      return OPL_ERROR;
    }
  }

  /************************** check router ports **************************/
  if(OPL_FALSE==mcRouterPortExist(desc->ingressPortId)) {
    mcControlStats.otherHostRxUnknownCount++;
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping query: query packet not received from router ports.\r\nportId=%d.\r\n",
      desc->ingressPortId);
    return OPL_ERROR;
  }

  /************************** query version **************************/
  if(desc->igmpMaxRespTime) {
    desc->igmpVersion = IGMP_VERSION_2;
    if(desc->igmpGroupAdress) {
      mcControlStats.groupSpecificV2QueryRxCount++;
    }
    else {
      mcControlStats.generalQueryV2RxCount++;
    }
  }
  else {
    desc->igmpVersion = IGMP_VERSION_1;
    mcControlStats.generalQueryV1RxCount++;
  }

  /************************** general query action **************************/
  if(desc->igmpGroupAdress==0&&desc->destIp==MULTICAST_ALL_HOST_GROUP) {
    /* flooding to vlan */
    for(i=ODM_START_PORT_NUN; i<ODM_NUM_OF_PORTS+1; i++) {
      if(OPL_TRUE==mcVlanMapExist(i, desc->cVlan)) {
        desc->egressPortId = i;
        mcSnoopingOutput(desc->egressPortId, desc->ingressTagType, desc->ingressPacket,
          desc->ingressPacketLength, OPL_FALSE);
      }
    }

    return OPL_OK;
  }

  /************************** specific query action **************************/
  if(desc->igmpGroupAdress==desc->destIp&&desc->igmpGroupAdress!=MULTICAST_ALL_HOST_GROUP) {
    for(i=ODM_START_PORT_NUN; i<ODM_NUM_OF_PORTS+1; i++) {
      if(OPL_TRUE==mcVlanMapExist(i, desc->cVlan)) {
#ifndef SEND_SPECIFIC_QUERY_TO_UNI_PORT_DIRECTLY
        if(mcGroupListGet(i, desc->igmpGroupMac, desc->igmpGroupAdress, desc->cVlan, MC_BITMAP))
#endif
        {
          desc->egressPortId = i;
          mcSnoopingOutput(desc->egressPortId, desc->ingressTagType, desc->ingressPacket,
            desc->ingressPacketLength, OPL_FALSE);
        }
      }
    }

    return OPL_OK;
  }

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping query: bad query packet.\r\n");
  return OPL_ERROR;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcSnoopingQueryPacketGenerate(MC_PACKET_DESC_t *desc,
  uint8 *queryPkt)
{
  uint8 hostSaMac[MAC_ADDRESS_LENGTH];
  uint8 hostSrcIp[SMALL_BUF_LEN];

  eth_igmp_frm_with_sgl_t *pkt;
  uint32 vid;
  uint16 pvid;
  oam_mcast_vlan_translation_entry_t translation_entry[16];
  uint8 num, index;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcSnoopingQueryPacketGenerate, enter.\r\n");

  vosMemSet(hostSaMac, 0, sizeof(hostSaMac));
  vosMemSet(hostSrcIp, 0, sizeof(hostSrcIp));

  pkt = (eth_igmp_frm_with_sgl_t *)queryPkt;

  if(vosHWAddrGet("eth0", hostSaMac) != NO_ERROR) {
    MC_DEBUG(DEBUG_LEVEL_WARNING, "Query generate: Get host MAC error.\r\n");
    return OPL_ERROR;
  }
  if(vosIPAddrGet("br0", hostSrcIp) != NO_ERROR) {//modify by zhouguanhua EPN204QID0018
    MC_DEBUG(DEBUG_LEVEL_WARNING, "Query generate: Get host IP error.\r\n");
    return OPL_ERROR;
  }

  /************************ ethernet header ************************/
  vosMemCpy(pkt->eth_hdr.dest, desc->igmpGroupMac, MAC_ADDRESS_LENGTH);
  vosMemCpy(pkt->eth_hdr.src, hostSaMac, MAC_ADDRESS_LENGTH);
  pkt->eth_hdr.vlan_tag.type = desc->cTpid;
#if 0
  pkt->eth_hdr.vlan_tag.vid = desc->cVlan;
#else
  /* Set vid as mcVid that ingressPort configured, if several mcVlan configured, just select one */
  for(vid=1; vid<OP_MAX_VLAN_NUMBER; vid++) {
    if(OPL_TRUE==mcVlanMapExist(desc->ingressPortId, vid)) {
      break;
    }
  }
  if(vid!=OP_MAX_VLAN_NUMBER) {
    odmMulticastTagTranslationGet(desc->ingressPortId, translation_entry, &num);
    for(index=0; index<num; index++) {
      if(translation_entry[index].mcast_vlan==vid) {
        vid = translation_entry[index].iptv_vlan;
        break;
      }
    }
    pkt->eth_hdr.vlan_tag.vid = vid;
  }
  /* If no mcVlan configured, set vid as pvid */
  else {
    dalPortPvidGet(desc->ingressPortId, &pvid);
    pkt->eth_hdr.vlan_tag.vid = pvid;
  }
#endif

/*  */
/*the query frame will be untagged, if recieved leaving frame is untagged.*/
    if (0 == desc->cVlan)
    {
        pkt->eth_hdr.vlan_tag.type = 0x8100;
        /*this vlan will be stripped by mcCtcoutput/mcSnoopingoutput function.*/
        pkt->eth_hdr.vlan_tag.vid = 0xfff;
    }
/*  */

  pkt->eth_hdr.protocol = 0x0800;

  /************************ ip header ************************/
  pkt->ip_hdr.version = 0x4;
  pkt->ip_hdr.ihl = 6;
  pkt->ip_hdr.total_len = 32;

  /*  */
  /*this query packet can be recieved by all of hosts in the same ip subnet,
  but do not be transmitted to other ip subnet by igmp router.*/
  #if 0
  pkt->ip_hdr.ttl = 64;
  #else
  pkt->ip_hdr.ttl = 1;
  #endif
  /* 014541 */
  
  pkt->ip_hdr.protocol = 2;
  pkt->ip_hdr.checksum = 0;
  pkt->ip_hdr.src_addr = inet_addr(hostSrcIp);
  pkt->ip_hdr.dest_addr = desc->igmpGroupAdress;

  *(uint32 *)&pkt->ip_opt = 0x94040000;

  pkt->ip_hdr.checksum = op_lib_inet_chksum(&pkt->ip_hdr, pkt->ip_hdr.ihl*4);

  /************************ igmp ************************/
  pkt->igmp_msg.type = IGMP_TYPE_GENERAL_QUERY;
  /* */
  /*the host recieved this query packet must respond it in time interval 10*0.1=1s.*/
  #if 0
  pkt->igmp_msg.max_resp_time = 100;
  #else
  pkt->igmp_msg.max_resp_time = 10;
  #endif
  /* 014541 */
  
  pkt->igmp_msg.checksum = 0;
  pkt->igmp_msg.group = desc->igmpGroupAdress;

  pkt->igmp_msg.checksum = op_lib_inet_chksum(&pkt->igmp_msg, 8);

  /************************ desc ************************/
  desc->nonFastLeaveEgressIsHaveIpOptions = OPL_TRUE;
  desc->nonFastLeaveQueryPacketLength = 64; /* include single vlan id */
  desc->nonFastLeaveQueryPacket = queryPkt;
  desc->nonFastLeaveEgressTagType = TAG_TYPE_SINGLE_TAG;

  /************************ if the packet received is untag ************************/
  if(desc->ingressTagType==TAG_TYPE_UNTAG) {
    vosMemCpy(queryPkt+12, queryPkt+16, desc->nonFastLeaveQueryPacketLength-16);
    desc->nonFastLeaveQueryPacketLength -= 4;
    desc->nonFastLeaveEgressTagType = TAG_TYPE_UNTAG;
  }

  return OPL_OK;
}
/*014054 */

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcSnoopingNonFastLeaveAction(MC_PACKET_DESC_t *desc)
{
  int ret;
  uint8 queryPkt[BIG_BUF_LEN];
  /*static uint32 i = 0;*/
  uint32 j;
  TIMER_OBJ_t *tmpTimer;
  MC_GROUP_t *group = NULL;
  MC_HOST_t *host = NULL;
  /*Begin modify by zhouguanhua 2012/12/12 of EPN204QID0005*/  
  int uni_num=0;
  int last_num_flag=0;
  /*End modify by zhouguanhua 2012/12/12 of EPN204QID0005*/

  ret = 0;
  vosMemSet(queryPkt, 0, sizeof(queryPkt));
  j = 0;
  tmpTimer = NULL;
  group = NULL;
  host = NULL;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcSnoopingNonFastLeaveAction, enter.\r\n");
  descPrintDebug(desc);

  mcSnoopingQueryPacketGenerate(desc, queryPkt);

  /*i++;*/

  mcGroupSemLock();

  /************************** group and host check **************************/
  group = mcGroupListGet(desc->ingressPortId, desc->igmpGroupMac, desc->igmpGroupAdress, desc->cVlan, MC_BITMAP);
  if(!group) { /* gruop does not exist */
    goto kill_timer;
  }

  /* group exists */
  /* check host */
  host = mcHostListGet(group, desc->saMac, desc->srcIp, MC_HOST_BITMAP);
  if(!host) { /* host does not exist */
    goto kill_timer;
  }

  /************************ not reach counter ************************/
  /* 014041 */
  //if(i<mcControlBlockLastQueryCountGet()+1) {
  if(group->querycnt<mcControlBlockLastQueryCountGet()) {
    if(host->isLastReportRecevied==OPL_TRUE) { /* report received */
      host->isNonFastLeaveTimerStart = OPL_FALSE;
      goto kill_timer;
    }
    else { /* send specific query */
      mcSnoopingOutput(desc->ingressPortId, desc->nonFastLeaveEgressTagType,
        desc->nonFastLeaveQueryPacket, desc->nonFastLeaveQueryPacketLength,
        OPL_TRUE);
	  /* 014041 */
      group->querycnt++;
      mcGroupSemUnLock();

      return OPL_OK;
    }
  }

  /************************ reach counter ************************/
  /* 014041 */
  //if(i==mcControlBlockLastQueryCountGet()+1) {
  if(group->querycnt==mcControlBlockLastQueryCountGet()) {
    if(host->isLastReportRecevied==OPL_TRUE) { /* report received */
      host->isNonFastLeaveTimerStart = OPL_FALSE;
      goto kill_timer;
    }
    else { /* delete host and group */
    /*Begin modify by zhouguanhua 2012/12/12 of EPN204QID0005*/
     //check other uni have the same group 
    for(uni_num=ODM_START_PORT_NUN;uni_num<=ODM_NUM_OF_PORTS;uni_num++)
    {
      if(uni_num!=group->hostPortId)           
      { 
          if(mcGroupListGet(uni_num, desc->igmpGroupMac, desc->igmpGroupAdress, desc->cVlan, MC_BITMAP))
          {
             last_num_flag=1;
             break;
          }
          else
          {
             last_num_flag=0;
          }
      }
    }
   /*End modify by zhouguanhua 2012/12/12 of EPN204QID0005*/
      /* delete host node */
      ret = mcHostListDel(group, host->saMac, host->srcIp, MC_HOST_BITMAP);
      //RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      /* check if the host list is empty */
      if(OPL_TRUE==mcGroupListHostListEmpty(group->hostPortId, group->groupMac, group->groupAddress,
        group->mcVlan, MC_BITMAP)) { /* the host list is empty */
        /* delete group node */
        ret = odmMcGroupDel(group->hostPortId, group->groupMac, group->groupAddress,
          group->mcVlan, MC_BITMAP);
        //RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

        group = NULL;
      }

      /************************** send to router ports **************************/
      /*  */
      /*If recieved report frame sent by other host from the same port,
      do not transimit this leaving frame to olt.*/
      if ((0==last_num_flag)&&((NULL == group) || (group->isLastReportRecevied != OPL_TRUE))) { /* report received */
        for(j=0; j<MC_ROUTER_PORT_NUM; j++) {
            if(mcRouterPort[j].enable==OPL_ENABLE) {
            desc->egressPortId = mcRouterPort[j].routerPortId;
            ret = mcSnoopingOutput(desc->egressPortId, desc->ingressTagType, desc->ingressPacket,
                desc->ingressPacketLength, OPL_TRUE);
            //RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
            }
        }
      }
      /* 014541 */

      goto kill_timer;
    }
  }

  /************************** kill timer **************************/
kill_timer:
  mcGroupSemUnLock();

  /* 014041 */
  /*i = 0;*/
  if (group)
  {
    group->querycnt = 0;
  }
  /* 014041 */
  
  tmpTimer = desc->nonFastLeaveTimer;
  vosTimerCancel(tmpTimer);

  vosFree(desc->ingressPacket);
  desc->ingressPacket = NULL;
  mcControlStats.memfreeCount++;
  vosFree(desc);
  mcControlStats.memfreeCount++;

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcSnoopingLeaveAction(MC_PACKET_DESC_t *desc)
{
  int ret;
  uint32 i;
  MC_GROUP_t *group;
  MC_HOST_t *host;
  uint8 macStr[SMALL_BUF_LEN];
  MC_PACKET_DESC_t *descNonFastLeaveTimer;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcSnoopingLeaveAction, enter.\r\n");

  ret = 0;
  group = NULL;
  host = NULL;
  vosMemSet(macStr, 0, sizeof(macStr));
  descNonFastLeaveTimer = NULL;

  /************************** check host ports **************************/
  if(!(desc->ingressPortId>=ODM_START_PORT_NUN&&desc->ingressPortId<=ODM_NUM_OF_PORTS)) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping leave: leqve packet not received from host ports.\r\nportId=%d.\r\n",
      desc->ingressPortId);
    return OPL_ERROR;
  }

  /************************** check da MAC **************************/
  if(!(desc->daMac[0]==0x01 && desc->daMac[1]==0x00
    && desc->daMac[2]==0x5e && desc->daMac[3]==0x0
    && desc->daMac[4]==0x0 && desc->daMac[5]==0x02)) {
    clStringGetByMac(macStr, desc->daMac);
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping leave: Da MAC error.\r\ndaMac=%s.\r\n", macStr);
    return OPL_ERROR;
  }

  /************************** ip check **************************/
  /* the multicast ip: 224.0.0.0-239.255.255.255 */
  /* 1110xxxx.xxxxxxxx.xxxxxxxx.xxxxxxx */
  if((desc->igmpGroupAdress&0xe0000000) != 0xe0000000) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping leave: Group address is not multicast address.\r\ngroupAddr=%s\r\n",
      inet_ntoa(*(struct in_addr *)&desc->igmpGroupAdress));
    return OPL_ERROR;

  }
  if(MULTICAST_ALL_ROUTERS_GROUP!=desc->destIp) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping leave: Dest IP error.\r\ndestIp=%s.",
      inet_ntoa(*(struct in_addr *)&desc->destIp));
    return OPL_ERROR;
  }

  /************************** group and host check **************************/
  group = mcGroupListGet(desc->ingressPortId, desc->igmpGroupMac, desc->igmpGroupAdress, desc->cVlan, MC_BITMAP);
  if(!group) { /* gruop does not exist */
    clStringGetByMac(macStr, desc->igmpGroupMac);
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING,
      "Snooping leave: Group does not exist.\r\nportId=%d, groupMac=%s, groupAdress=%s, vlan=%d.\r\n",
      desc->ingressPortId, macStr, inet_ntoa(*(struct in_addr *)&desc->igmpGroupAdress), desc->cVlan);
    return OPL_ERROR;
  }

  /* group exists */
  /* check host */
  host = mcHostListGet(group, desc->saMac, desc->srcIp, MC_HOST_BITMAP);
  if(!host) { /* host does not exist */
    clStringGetByMac(macStr, desc->saMac);
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING,
      "Snooping leave: Host does not exist.\r\nsaMac=%s, srcIp=%s.\r\n",
      macStr, inet_ntoa(*(struct in_addr *)&desc->srcIp));
    clStringGetByMac(macStr, desc->igmpGroupMac);
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING,
      "Snooping leave: portId=%d, groupMac=%s, groupAdress=%s, vlan=%d.\r\n",
      desc->ingressPortId, macStr, inet_ntoa(*(struct in_addr *)&desc->igmpGroupAdress), desc->cVlan);
    return OPL_ERROR;
  }

  /* host exists */

  /************************** group and host delete **************************/
  /* fast leave mode */
  if(mcControlBlockLeaveModeGet()==MC_LEAVE_MODE_FAST) {
    printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
    /* delete host node */
    ret = mcHostListDel(group, host->saMac, host->srcIp, MC_HOST_BITMAP);
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
    /* check if the host list is empty */
    if(OPL_TRUE==mcGroupListHostListEmpty(group->hostPortId, group->groupMac, group->groupAddress,
      group->mcVlan, MC_BITMAP)) { /* the host list is empty */
      /* delete group node */
      printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
      ret = odmMcGroupDel(group->hostPortId, group->groupMac, group->groupAddress,
        group->mcVlan, MC_BITMAP);
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

      group = NULL;
    }

    /************************** send to router ports **************************/
    for(i=0; i<MC_ROUTER_PORT_NUM; i++) {
      if(mcRouterPort[i].enable==OPL_ENABLE) {
        desc->egressPortId = mcRouterPort[i].routerPortId;
        ret = mcSnoopingOutput(desc->egressPortId, desc->ingressTagType, desc->ingressPacket,
          desc->ingressPacketLength, OPL_FALSE);
        RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      }
    }

    return OPL_OK;
  }

  /* none fast leave mode */
  if(mcControlBlockLeaveModeGet()==MC_LEAVE_MODE_NON_FAST_LEAVE) {
    printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
    if(host->isNonFastLeaveTimerStart==OPL_TRUE) { /* timer exist */
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING,
        "Snooping leave: Non-fast-leave, timer exists.\r\n");
      return OPL_ERROR;
    }

    /* */
    ret = mcGroupListSetisLastReportRecevied(group->hostPortId, group->groupMac, group->groupAddress,
      group->mcVlan, (MC_BITMAP_HOST_PORT_ID|MC_BITMAP_GDA_MAC|MC_BITMAP_GDA_IP|MC_BITMAP_VLAN_ID), OPL_FALSE);
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
    /*014541 */
    
    host->isNonFastLeaveTimerStart=OPL_TRUE;

    /************************** create timer **************************/
    descNonFastLeaveTimer = (MC_PACKET_DESC_t *)vosAlloc(sizeof(MC_PACKET_DESC_t));
    mcControlStats.memAllocCount++;
    if(!descNonFastLeaveTimer) {
      return OPL_ERROR;
    }
    vosMemSet(descNonFastLeaveTimer, 0, sizeof(MC_PACKET_DESC_t));

    vosMemCpy(descNonFastLeaveTimer, desc, sizeof(MC_PACKET_DESC_t));

    desc->isNonFastLeaveTimerStart = OPL_TRUE;

    ret = mcHostListSetisLastReportRecevied(group, host->saMac, host->srcIp,
      MC_HOST_BITMAP, OPL_FALSE);
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

    /* create timer */
    descNonFastLeaveTimer->nonFastLeaveTimer = vosTimerGetFree();
    if(descNonFastLeaveTimer->nonFastLeaveTimer) {
      ret = vosTimerStart(descNonFastLeaveTimer->nonFastLeaveTimer, REOCURRING_TIMER,
        mcControlBlockLastQueryIntervalGet(),
       (FUNCPTR)mcTimerMsgSend,
       MC_SNOOP_NON_FAST_LEAVE, (uint32)descNonFastLeaveTimer, 0,0,0,0,0,0);
     
      if (0 != ret)
      {
        vosFree(descNonFastLeaveTimer);
        desc->isNonFastLeaveTimerStart = OPL_FALSE;
      }

    }
    
    else
    {
        vosFree(descNonFastLeaveTimer);
        desc->isNonFastLeaveTimerStart = OPL_FALSE;
    }
    

    return OPL_OK;
  }

  return OPL_ERROR;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcSnoopingJoinAction(MC_PACKET_DESC_t *desc)
{
  int ret;
  /* 014041 */
  uint32 i;
  uint32 j;
  uint32 num= 0;
  /* */
  MC_GROUP_t *group;
  MC_HOST_t *host;
  uint8 tempGroupMac[MAC_ADDRESS_LENGTH];
  uint8 macStr1[SMALL_BUF_LEN];
  uint8 macStr2[SMALL_BUF_LEN];

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcSnoopingJoinAction, enter.\r\n");
  descPrintDebug(desc);

  ret = 0;
  group = NULL;
  host = NULL;
  vosMemSet(tempGroupMac, 0, MAC_ADDRESS_LENGTH);
  vosMemSet(macStr1, 0, sizeof(macStr1));
  vosMemSet(macStr2, 0, sizeof(macStr2));

  /* 014041 */
  if (0 == desc->cVlan)
  {
    for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
      if(OPL_TRUE==mcVlanMapExist(desc->ingressPortId, j)) {
       num++;
      }
    }
    if (0 == num){
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcSnoopingJoin: no vlan configue in port %d.\r\n", desc->ingressPortId);
      return OPL_ERROR;
    }
  }
  else
  {
    if(OPL_TRUE!=mcVlanMapExist(desc->ingressPortId, desc->cVlan)) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcSnoopingJoin: vlan %d is not supported in port %d.\r\n", 
  	  	            desc->cVlan, desc->ingressPortId);
  	  return OPL_ERROR;
    }
  }  
  /*  */
  
  /************************** check host ports **************************/
  if(!(desc->ingressPortId>=ODM_START_PORT_NUN&&desc->ingressPortId<=ODM_NUM_OF_PORTS)) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping join: join packet not received from host ports.\r\nportId=%d.\r\n",
      desc->ingressPortId);
    return OPL_ERROR;
  }

  /************************** ip check **************************/
  /* the multicast ip: 224.0.0.0-239.255.255.255 */
  /* 1110xxxx.xxxxxxxx.xxxxxxxx.xxxxxxx */
  if((desc->igmpGroupAdress&0xe0000000) != 0xe0000000) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping join: Group address is not multicast address.\r\ngroupAddr=%s\r\n",
      inet_ntoa(*(struct in_addr *)&desc->igmpGroupAdress));
    return OPL_ERROR;

  }
  if(desc->igmpGroupAdress!=desc->destIp) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping join: group address not equal to dest ip.\r\ngroupAddr=%s, destIp=%s.\r\n",
      inet_ntoa(*(struct in_addr *)&desc->igmpGroupAdress), inet_ntoa(*(struct in_addr *)&desc->destIp));
    return OPL_ERROR;
  }

  clMcMacGetByMcIp(tempGroupMac, desc->igmpGroupAdress);
  if(FALSE==clMacCmp(tempGroupMac, desc->daMac)) {
    clStringGetByMac(macStr1, tempGroupMac);
    clStringGetByMac(macStr2, desc->daMac);
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping join: group mac not equal to dest mac.\r\ngroupMac=%s, destMac=%s.\r\n",
      macStr1, macStr2);
    return OPL_ERROR;
  }

  /************************** group add **************************/
  group = mcGroupListGet(desc->ingressPortId, desc->igmpGroupMac, desc->igmpGroupAdress, desc->cVlan, MC_BITMAP);
  if(!group) { /* gruop does not exist */
    /* check max group limit */
    if(mcGroupNumPerPort(desc->ingressPortId)==mcControlBlockmaxGroupNumGet(desc->ingressPortId)) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping join: reach max group number.\r\ngroupNum=%d.\r\n",
        mcControlBlockmaxGroupNumGet(desc->ingressPortId));
      return OPL_ERROR;
    }

    ret = odmMcGroupAdd(desc->ingressPortId, desc->igmpGroupMac, desc->igmpGroupAdress, desc->cVlan, GROUP_STATE_LEARNING, MC_BITMAP);
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

    group = mcGroupListGet(desc->ingressPortId, desc->igmpGroupMac, desc->igmpGroupAdress, desc->cVlan, MC_BITMAP);
    RETURN_VAL_IF_FAIL(group != NULL, OPL_NOT_EXIST);
  }

  /* group exists */

  /* check host */
  host = mcHostListGet(group, desc->saMac, desc->srcIp, MC_HOST_BITMAP);
  if(!host) { /* host does not exist */
    /* add host node */
    host = mcHostListAdd(group, desc->saMac, desc->srcIp, GROUP_STATE_LEARNING, MC_HOST_BITMAP);
    RETURN_VAL_IF_FAIL(host != NULL, OPL_ERROR);
  }

  /* host exists */
  /* reset host aging time */
  ret = mcHostListSetHostAgingTime(group, desc->saMac, desc->srcIp,
    MC_HOST_BITMAP, mcControlBlockHostAgingTimeGet());
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
  /* reset last report received flag */
  ret = mcHostListSetisLastReportRecevied(group, desc->saMac, desc->srcIp,
    MC_HOST_BITMAP, OPL_TRUE);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  /* */
  ret = mcGroupListSetisLastReportRecevied(group->hostPortId, group->groupMac,
      group->groupAddress, group->mcVlan, (MC_BITMAP_HOST_PORT_ID|MC_BITMAP_GDA_MAC|MC_BITMAP_GDA_IP|MC_BITMAP_VLAN_ID), OPL_TRUE);
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
  /* 014541 */

  /************************** send to router ports **************************/
  for(i=0; i<MC_ROUTER_PORT_NUM; i++) {
    if(mcRouterPort[i].enable==OPL_ENABLE) {
      desc->egressPortId = mcRouterPort[i].routerPortId;
      ret = mcSnoopingOutput(desc->egressPortId, desc->ingressTagType, desc->ingressPacket,
          desc->ingressPacketLength, OPL_FALSE);
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
    }
  }

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcSnoopingOutput(uint32 egressPortId, uint32 tagType, uint8 *packet, uint32 packetLen,
  uint32 isLastQueryPacket)
{
  uint8 aucPacket[BIG_BUF_LEN];
  uint8 *tmpPacketPointer;
  uint32 mcVid, userVid;
  uint32 tmpPacketLength;
  int32 ret;
  eth_header_with_sgl_tag_t *sglEthHeader;
  host_outbound_hdr_t *out_hdr;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcSnoopingOutput, enter.\r\n");

  vosMemSet(aucPacket, 0, sizeof(aucPacket));
  mcVid = userVid = 0;
  ret = 0;

  /*Reserve some bytes for opulan header.*/
  tmpPacketPointer = aucPacket + sizeof(host_outbound_hdr_t);
  tmpPacketLength = 0;

  sglEthHeader = (eth_header_with_sgl_tag_t *)packet;

  /************************ tag oper ************************/
  if((egressPortId!=PORT_PON_LOGIC
    && tagType==TAG_TYPE_SINGLE_TAG
    && mcTagOperPerPortGet(egressPortId)==MC_TAG_OPER_MODE_STRIP)
    || (tagType==TAG_TYPE_SINGLE_TAG&&sglEthHeader->vlan_tag.vid==0xfff)){ /* tag strip mode */
    /* remove tag */
    vosMemCpy(tmpPacketPointer, packet, 12);
    vosMemCpy(tmpPacketPointer+12, packet+16, packetLen-16);
    tmpPacketLength = packetLen-4;
  }
  else if(egressPortId!=PORT_PON_LOGIC
    && tagType==TAG_TYPE_SINGLE_TAG
    && mcTagOperPerPortGet(egressPortId)==MC_TAG_OPER_MODE_TRANSLATION
    && isLastQueryPacket==OPL_FALSE) { /* vlan translation */
    mcVid = sglEthHeader->vlan_tag.vid;
    ret = mcTagTranslationTableGet(egressPortId, mcVid, &userVid);
    vosMemCpy(tmpPacketPointer, packet, packetLen);
    sglEthHeader = (eth_header_with_sgl_tag_t *)tmpPacketPointer;
    if(ret == OPL_OK)
    {
        sglEthHeader->vlan_tag.vid = userVid;
    }
    else
    {
        sglEthHeader->vlan_tag.vid = mcVid;
    }

    tmpPacketLength = packetLen;
  }
  else {
    vosMemCpy(tmpPacketPointer, packet, packetLen);
    tmpPacketLength = packetLen;
  }

  /************************** set header **************************/
  out_hdr =  (host_outbound_hdr_t *)aucPacket;
  out_hdr->reserved = 0;
  out_hdr->icos = 0;
  out_hdr->eport = egressPortId;

  tmpPacketLength += sizeof(host_outbound_hdr_t);

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping output: send packet to %d, packetLength=%d.\r\n", egressPortId, tmpPacketLength-1);

  /************************** send to cpu **************************/
  eopl_host_send((void *)out_hdr, tmpPacketLength);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcSnoopingInput(MC_PACKET_DESC_t *desc)
{
  int ret = OPL_ERROR;

  //MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcSnoopingInput, enter.\r\n");


  switch(desc->igmpType) {
    case IGMP_TYPE_GENERAL_QUERY:
   
      #ifdef CTC_MULTICAST_SURPORT
      mc_printf("\nfunc:%s,line:%d. igmp query.\n",__FUNCTION__,__LINE__);
      #endif
      
      ret = mcSnoopingQueryAction(desc);
      break;
    case IGMP_TYPE_V1_REPORT:
    
      #ifdef CTC_MULTICAST_SURPORT
      mc_printf("\nfunc:%s,line:%d. igmp v1 report.\n",__FUNCTION__,__LINE__);
      #endif
 
      mcControlStats.joinReportV1RxCount++;
      ret = mcSnoopingJoinAction(desc);
      break;
    case IGMP_TYPE_V2_REPORT:
     
      #ifdef CTC_MULTICAST_SURPORT
      mc_printf("\nfunc:%s,line:%d. igmp v2 report.\n",__FUNCTION__,__LINE__);
      #endif
      
      mcControlStats.joinReportV2RxCount++;
      ret = mcSnoopingJoinAction(desc);
      break;
    case IGMP_TYPE_V2_LEAVE:
   
      #ifdef CTC_MULTICAST_SURPORT
      mc_printf("\nfunc:%s,line:%d. igmp v2 leave.\n",__FUNCTION__,__LINE__);
      #endif
   
      mcControlStats.leaveV2RxCount++;
      ret = mcSnoopingLeaveAction(desc);
      break;
    default:
      
      #ifdef CTC_MULTICAST_SURPORT
      mc_printf("\nfunc:%s,line:%d. default.\n",__FUNCTION__,__LINE__);
      #endif
     
      if(desc->ingressPortId>=ODM_START_PORT_NUN&&desc->ingressPortId<=ODM_NUM_OF_PORTS) {
        mcControlStats.otherHostRxUnknownCount++;
      }
      else {
        mcControlStats.otherRouterRxUnknownCount++;
      }

      /************************ flooding to vlan ************************/
      /* TBD */
      //mcSnoopingOutput(desc, HOST_GENERATED_PTYPE_BROADCAST);

      break;
  }

  return ret;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcCtcQueryAction(MC_PACKET_DESC_t *desc)
{
  uint32 i;
  int ret;
  uint32 mcVlanExist;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcCtcQueryAction, enter.\r\n");

  mcVlanExist = OPL_FALSE;

  /************************** check multicast vlan **************************/
  if(desc->ingressTagType==TAG_TYPE_UNTAG) {
    mcControlStats.otherRouterRxUnknownCount++;
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "CTC query: no tag, dropped.\r\n");
    return OPL_ERROR;
  }
  else {
    for(i=ODM_START_PORT_NUN; i<ODM_NUM_OF_PORTS+1; i++) {
      if(OPL_TRUE==mcVlanMapExist(i, desc->cVlan)) {
        mcVlanExist = OPL_TRUE;
      }
    }
    if(mcVlanExist==OPL_FALSE) {
      mcControlStats.otherRouterRxUnknownCount++;
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "CTC query: multicast vlan mismatch, dropped.\r\nportId=%d, vlan=%d.\r\n",
        desc->ingressPortId, desc->cVlan);
      return OPL_ERROR;
    }
  }

  /************************** check router ports **************************/
  if(OPL_FALSE==mcRouterPortExist(desc->ingressPortId)) {
    mcControlStats.otherHostRxUnknownCount++;
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "CTC query: query packet not received from router ports.\r\nportId=%d.\r\n",
      desc->ingressPortId);
    return OPL_ERROR;
  }

  /************************** query version **************************/
  if(desc->igmpMaxRespTime) {
    desc->igmpVersion = IGMP_VERSION_2;
    if(desc->igmpGroupAdress) {
      mcControlStats.groupSpecificV2QueryRxCount++;
    }
    else {
      mcControlStats.generalQueryV2RxCount++;
    }
  }
  else {
    desc->igmpVersion = IGMP_VERSION_1;
    mcControlStats.generalQueryV1RxCount++;
  }

  /************************** general query action **************************/
  if(desc->igmpGroupAdress==0&&desc->destIp==MULTICAST_ALL_HOST_GROUP) {
    /* flooding to vlan */
    for(i=ODM_START_PORT_NUN; i<ODM_NUM_OF_PORTS+1; i++) {
      if(OPL_TRUE==mcVlanMapExist(i, desc->cVlan)) {
        desc->egressPortId = i;
        mcCtcOutput(desc->egressPortId, desc->ingressTagType, desc->ingressPacket,
          desc->ingressPacketLength, OPL_FALSE);
      }
    }

    return OPL_OK;
  }

  /************************** specific query action **************************/
  if(desc->igmpGroupAdress==desc->destIp&&desc->igmpGroupAdress!=MULTICAST_ALL_HOST_GROUP) {
    for(i=ODM_START_PORT_NUN; i<ODM_NUM_OF_PORTS+1; i++) {
      if(OPL_TRUE==mcVlanMapExist(i, desc->cVlan)) {
        if(mcGroupListGet(i, desc->igmpGroupMac, desc->igmpGroupAdress, desc->cVlan, MC_BITMAP)) {
          desc->egressPortId = i;
          mcCtcOutput(desc->egressPortId, desc->ingressTagType, desc->ingressPacket,
            desc->ingressPacketLength, OPL_FALSE);
        }
      }
    }

    return OPL_OK;
  }

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "CTC query: bad query packet.\r\n");
  return OPL_ERROR;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcCtcNonFastLeaveAction(MC_PACKET_DESC_t *desc)
{
  int ret;
  uint8 queryPkt[BIG_BUF_LEN];
  /*static uint32 i = 0;*/
  uint32 j;
  TIMER_OBJ_t *tmpTimer;
  MC_GROUP_t *group = NULL;

  ret = 0;
  vosMemSet(queryPkt, 0, sizeof(queryPkt));
  j = 0;
  tmpTimer = NULL;
  group = NULL;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "CTC non-fast-leave:\r\n");
  //descPrintDebug(desc);

  mcSnoopingQueryPacketGenerate(desc, queryPkt);

  /*i++;*/

  mcGroupSemLock();

  group = mcGroupListGet(desc->ingressPortId, desc->igmpGroupMac, desc->igmpGroupAdress, desc->cVlan, MC_BITMAP);
  if(!group) { /* gruop does not exist */
    goto kill_timer;
  }

  /************************ not reach counter ************************/
  /* 014041 */
  //if(i<mcControlBlockLastQueryCountGet()+1) {
  if(group->querycnt<mcControlBlockLastQueryCountGet()) {
    if(group->isLastReportRecevied==OPL_TRUE) { /* report received */
      group->isNonFastLeaveTimerStart = OPL_FALSE;
      goto kill_timer;
    }
    else { /* send specific query */
      mcCtcOutput(desc->ingressPortId, desc->nonFastLeaveEgressTagType,
        desc->nonFastLeaveQueryPacket, desc->nonFastLeaveQueryPacketLength,
        OPL_TRUE);
	  /* 014041 */
      group->querycnt++;
      mcGroupSemUnLock();

      return OPL_OK;
    }
  }

  /************************ reach counter ************************/
  /* 014041 */
  //if(i==mcControlBlockLastQueryCountGet()+1) {
  if(group->querycnt==mcControlBlockLastQueryCountGet()) {
    if(group->isLastReportRecevied==OPL_TRUE) { /* report received */
      group->isNonFastLeaveTimerStart = OPL_FALSE;
      goto kill_timer;
    }
    else { /* delete group */
      /* check if the host list is empty */
      if(OPL_TRUE==mcGroupListHostListEmpty(group->hostPortId, group->groupMac, group->groupAddress,
        group->mcVlan, MC_BITMAP)) { /* the host list is empty */
        /* delete group node */
        ret = odmMcGroupDel(group->hostPortId, group->groupMac, group->groupAddress,
          group->mcVlan, MC_BITMAP);
        //RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

        /*  */
        /*Mc vlan must be deleted in ctc controlable mc mode.*/
        ret = mcCtcMcvlanMemRmv(group->hostPortId, group->mcVlan);
        if (OPL_OK != ret)
        {
          return OPL_ERROR;
        }
        /* 014040 */

        group = NULL;
      }

      /************************** send to router ports **************************/
      for(j=0; j<MC_ROUTER_PORT_NUM; j++) {
        if(mcRouterPort[j].enable==OPL_ENABLE) {
          desc->egressPortId = mcRouterPort[j].routerPortId;
          ret = mcCtcOutput(desc->egressPortId, desc->ingressTagType, desc->ingressPacket,
            desc->ingressPacketLength, OPL_TRUE);
          //RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
        }
      }

      goto kill_timer;
    }
  }

  /************************** kill timer **************************/
kill_timer:
  mcGroupSemUnLock();

  /* 014041 */  
  //i = 0;
  if (group)
  {
    group->querycnt = 0;
  }
  /* 014041 */
  
  tmpTimer = desc->nonFastLeaveTimer;
  vosTimerCancel(tmpTimer);

  vosFree(desc->ingressPacket);
  desc->ingressPacket = NULL;
  mcControlStats.memfreeCount++;
  vosFree(desc);
  mcControlStats.memfreeCount++;

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcCtcLeaveAction(MC_PACKET_DESC_t *desc)
{
  int ret;
  uint32 i;
  MC_GROUP_t *group;
  uint8 macStr[SMALL_BUF_LEN];
  MC_PACKET_DESC_t *descNonFastLeaveTimer;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcCtcLeaveAction, enter.\r\n");

  ret = 0;
  group = NULL;
  vosMemSet(macStr, 0, sizeof(macStr));
  descNonFastLeaveTimer = NULL;

  /************************** check host ports **************************/
  if(!(desc->ingressPortId>=ODM_START_PORT_NUN&&desc->ingressPortId<=ODM_NUM_OF_PORTS)) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "CTC leave: leqve packet not received from host ports.\r\nportId=%d.\r\n",
      desc->ingressPortId);
    return OPL_ERROR;
  }

  /************************** check da MAC **************************/
  if(!(desc->daMac[0]==0x01 && desc->daMac[1]==0x00
    && desc->daMac[2]==0x5e && desc->daMac[3]==0x0
    && desc->daMac[4]==0x0 && desc->daMac[5]==0x02)) {
    clStringGetByMac(macStr, desc->daMac);
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "CTC leave: Da MAC error.\r\ndaMac=%s.\r\n", macStr);
    return OPL_ERROR;
  }

  /************************** ip check **************************/
  /* the multicast ip: 224.0.0.0-239.255.255.255 */
  /* 1110xxxx.xxxxxxxx.xxxxxxxx.xxxxxxx */
  if((desc->igmpGroupAdress&0xe0000000) != 0xe0000000) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "CTC leave: Group address is not multicast address.\r\ngroupAddr=%s\r\n",
      inet_ntoa(*(struct in_addr *)&desc->igmpGroupAdress));
    return OPL_ERROR;

  }
  if(MULTICAST_ALL_ROUTERS_GROUP!=desc->destIp) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "CTC leave: Dest IP error.\r\ndestIp=%s.",
      inet_ntoa(*(struct in_addr *)&desc->destIp));
    return OPL_ERROR;
  }

  /************************** group and host check **************************/
  group = mcGroupListGet(desc->ingressPortId, desc->igmpGroupMac, desc->igmpGroupAdress, desc->cVlan, MC_BITMAP);
  if(!group) { /* gruop does not exist */
    clStringGetByMac(macStr, desc->igmpGroupMac);
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING,
      "CTC leave: Group does not exist.\r\nportId=%d, groupMac=%s, groupAdress=%s, vlan=%d.\r\n",
      desc->ingressPortId, macStr, inet_ntoa(*(struct in_addr *)&desc->igmpGroupAdress), desc->cVlan);
	
    /* even if the group not exist, still send the leave packet to olt transparently */
    if(mcControlBlockLeaveModeGet()==MC_LEAVE_MODE_FAST) {
     /************************** change vlan id to port id **************************/
        mcCtcPacketSetVlan(desc->ingressPacket, &desc->ingressPacketLength,
    &desc->ingressTagType, VLAN_DEFAULT_CTPID, desc->ingressPortId);

     /************************** send to router ports **************************/
    for(i=0; i<MC_ROUTER_PORT_NUM; i++) {
      if(mcRouterPort[i].enable==OPL_ENABLE) {
        desc->egressPortId = mcRouterPort[i].routerPortId;
        ret = mcCtcOutput(desc->egressPortId, desc->ingressTagType, desc->ingressPacket,
          desc->ingressPacketLength, OPL_FALSE);
        RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      }
    }
    }
    return OPL_ERROR;
  }

  /* group exists */

  /************************** change vlan id to port id **************************/
  mcCtcPacketSetVlan(desc->ingressPacket, &desc->ingressPacketLength,
    &desc->ingressTagType, VLAN_DEFAULT_CTPID, desc->ingressPortId);

#if 1 /* yxzhao added, igmp leave packets transmit to OLT transparently, 2009-12-31, 10:35:37 */
  /************************** group delete **************************/
  /* fast leave mode */
  if(mcControlBlockLeaveModeGet()==MC_LEAVE_MODE_FAST) {
    /* check if the host list is empty */
    if(OPL_TRUE==mcGroupListHostListEmpty(group->hostPortId, group->groupMac, group->groupAddress,
      group->mcVlan, MC_BITMAP)) { /* the host list is empty */
      /* delete group node */
      ret = odmMcGroupDel(group->hostPortId, group->groupMac, group->groupAddress,
        group->mcVlan, MC_BITMAP);
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

      /*  */
      /*Mc vlan must be deleted in ctc controlable mc mode.*/
      ret = mcCtcMcvlanMemRmv(group->hostPortId, group->mcVlan);
      if (OPL_OK != ret)
      {
        return OPL_ERROR;
      }
      /* 014040 */
      
      group = NULL;
    }

    /************************** send to router ports **************************/
    for(i=0; i<MC_ROUTER_PORT_NUM; i++) {
      if(mcRouterPort[i].enable==OPL_ENABLE) {
        desc->egressPortId = mcRouterPort[i].routerPortId;
        ret = mcCtcOutput(desc->egressPortId, desc->ingressTagType, desc->ingressPacket,
          desc->ingressPacketLength, OPL_FALSE);
        RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      }
    }

    return OPL_OK;
  }

  /* none fast leave mode */
  if(mcControlBlockLeaveModeGet()==MC_LEAVE_MODE_NON_FAST_LEAVE) {
    if(group->isNonFastLeaveTimerStart==OPL_TRUE) { /* timer exist */
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING,
        "CTC leave: Non-fast-leave, timer exists.\r\n");
      return OPL_ERROR;
    }
    group->isNonFastLeaveTimerStart=OPL_TRUE;

    /************************** create timer **************************/
    descNonFastLeaveTimer = (MC_PACKET_DESC_t *)vosAlloc(sizeof(MC_PACKET_DESC_t));
    mcControlStats.memAllocCount++;
    if(!descNonFastLeaveTimer) {
      return OPL_ERROR;
    }
    vosMemSet(descNonFastLeaveTimer, 0, sizeof(MC_PACKET_DESC_t));

    vosMemCpy(descNonFastLeaveTimer, desc, sizeof(MC_PACKET_DESC_t));

    desc->isNonFastLeaveTimerStart = OPL_TRUE;

    ret = mcGroupListSetisLastReportRecevied(group->hostPortId, group->groupMac, group->groupAddress,
      group->mcVlan, MC_BITMAP, OPL_FALSE);
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

    /* create timer */
    descNonFastLeaveTimer->nonFastLeaveTimer = vosTimerGetFree();
    if(descNonFastLeaveTimer->nonFastLeaveTimer) {
      vosTimerStart(descNonFastLeaveTimer->nonFastLeaveTimer, REOCURRING_TIMER,
        mcControlBlockLastQueryIntervalGet(),
       (FUNCPTR)mcTimerMsgSend,
       MC_CTC_LEAVE_ACTIVE, (uint32)descNonFastLeaveTimer, 0,0,0,0,0,0);
    }

    return OPL_OK;
  }

  return OPL_ERROR;
#else
  /************************** send to router ports **************************/
  for(i=0; i<MC_ROUTER_PORT_NUM; i++) {
    if(mcRouterPort[i].enable==OPL_ENABLE) {
      desc->egressPortId = mcRouterPort[i].routerPortId;
      ret = mcCtcOutput(desc->egressPortId, desc->ingressTagType, desc->ingressPacket,
        desc->ingressPacketLength, OPL_FALSE);
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
    }
  }

  return OPL_OK;
#endif
}


/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcCtcJoinAction(MC_PACKET_DESC_t *desc)
{
  int ret;
  uint32 i;
  MC_GROUP_t *group;
  uint8 tempGroupMac[MAC_ADDRESS_LENGTH];
  uint8 macStr1[SMALL_BUF_LEN];
  uint8 macStr2[SMALL_BUF_LEN];

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcCtcJoinAction, enter.\r\n");

  ret = 0;
  group = NULL;
  vosMemSet(tempGroupMac, 0, MAC_ADDRESS_LENGTH);
  vosMemSet(macStr1, 0, sizeof(macStr1));
  vosMemSet(macStr2, 0, sizeof(macStr2));

  /************************** check host ports **************************/
  if(!(desc->ingressPortId>=ODM_START_PORT_NUN&&desc->ingressPortId<=ODM_NUM_OF_PORTS)) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Snooping join: join packet not received from host ports.\r\nportId=%d.\r\n",
      desc->ingressPortId);
    return OPL_ERROR;
  }

  /************************** ip check **************************/
  /* the multicast ip: 224.0.0.0-239.255.255.255 */
  /* 1110xxxx.xxxxxxxx.xxxxxxxx.xxxxxxx */
  if((desc->igmpGroupAdress&0xe0000000) != 0xe0000000) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "CTC join: Group address is not multicast address.\r\ngroupAddr=%s\r\n",
      inet_ntoa(*(struct in_addr *)&desc->igmpGroupAdress));
    return OPL_ERROR;

  }
  if(desc->igmpGroupAdress!=desc->destIp) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "CTC join: group address not equal to dest ip.\r\ngroupAddr=%s, destIp=%s.\r\n",
      inet_ntoa(*(struct in_addr *)&desc->igmpGroupAdress), inet_ntoa(*(struct in_addr *)&desc->destIp));
    return OPL_ERROR;
  }

  clMcMacGetByMcIp(tempGroupMac, desc->igmpGroupAdress);
  if(FALSE==clMacCmp(tempGroupMac, desc->daMac)) {
    clStringGetByMac(macStr1, tempGroupMac);
    clStringGetByMac(macStr2, desc->daMac);
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "CTC join: group mac not equal to dest mac.\r\ngroupMac=%s, destMac=%s.\r\n",
      macStr1, macStr2);
    return OPL_ERROR;
  }

  /************************** change vlan id to port id **************************/
  mcCtcPacketSetVlan(desc->ingressPacket, &desc->ingressPacketLength,
    &desc->ingressTagType, VLAN_DEFAULT_CTPID, desc->ingressPortId);

  /************************** group check **************************/
  group = mcGroupListGet(desc->ingressPortId, desc->igmpGroupMac, desc->igmpGroupAdress, desc->cVlan, MC_BITMAP);
  if(group) { /* gruop exists */
    /************************** update flags **************************/
    /* reset group aging time */
    ret = mcGroupListSetHostAgingTime(group->hostPortId, group->groupMac,
      group->groupAddress, group->mcVlan, MC_BITMAP, mcControlBlockHostAgingTimeGet());
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
    /* reset last report received flag */
    ret = mcGroupListSetisLastReportRecevied(group->hostPortId, group->groupMac,
      group->groupAddress, group->mcVlan, MC_BITMAP, OPL_TRUE);
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
  }

  /************************** send to router ports **************************/
  for(i=0; i<MC_ROUTER_PORT_NUM; i++) {
    if(mcRouterPort[i].enable==OPL_ENABLE) {
      desc->egressPortId = mcRouterPort[i].routerPortId;
      #ifdef CTC_MULTICAST_SURPORT
      mc_printf("\nfunc:%s egressport:%d\n",__FUNCTION__,desc->egressPortId);
      #endif
      ret = mcCtcOutput(desc->egressPortId, desc->ingressTagType, desc->ingressPacket,
          desc->ingressPacketLength, OPL_FALSE);
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
    }
  }

  return OPL_OK;
}


/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcCtcOutput(uint32 egressPortId, uint32 tagType, uint8 *packet, uint32 packetLen,
  uint32 isLastQueryPacket)
{
  uint8 *tmpPacketPointer;
  uint8 aucPacket[BIG_BUF_LEN];
  uint32 tmpPacketLength;
  uint32 mcVid, userVid;
  int32 ret;
  host_outbound_hdr_t *out_hdr;
  eth_header_with_sgl_tag_t *sglEthHeader;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcCtcOutput, enter.\r\n");

  vosMemSet(aucPacket, 0, sizeof(aucPacket));
  
  /*Reserve some bytes for opulan header.*/
  tmpPacketPointer = aucPacket + sizeof(host_outbound_hdr_t);
  tmpPacketLength = 0;

  mcVid = userVid = 0;
  ret = 0;


  sglEthHeader = (eth_header_with_sgl_tag_t *)packet;

  /************************ tag oper ************************/
  if((egressPortId!=PORT_PON_LOGIC
    && tagType==TAG_TYPE_SINGLE_TAG
    && mcTagOperPerPortGet(egressPortId)==MC_TAG_OPER_MODE_STRIP)
    || (tagType==TAG_TYPE_SINGLE_TAG&&sglEthHeader->vlan_tag.vid==0xfff)){ /* tag strip mode */
    /* remove tag */
    vosMemCpy(tmpPacketPointer, packet, 12);
    vosMemCpy(tmpPacketPointer+12, packet+16, packetLen-16);
    tmpPacketLength = packetLen-4;
  }
  else if(egressPortId!=PORT_PON_LOGIC
    && tagType==TAG_TYPE_SINGLE_TAG
    && mcTagOperPerPortGet(egressPortId)==MC_TAG_OPER_MODE_TRANSLATION
    && isLastQueryPacket==OPL_FALSE) { /* vlan translation */
    mcVid = sglEthHeader->vlan_tag.vid;
    ret = mcTagTranslationTableGet(egressPortId, mcVid, &userVid);
    vosMemCpy(tmpPacketPointer, packet, packetLen);
    sglEthHeader = (eth_header_with_sgl_tag_t *)tmpPacketPointer;
    if(ret == OPL_OK)
    {
        sglEthHeader->vlan_tag.vid = userVid;
    }

    tmpPacketLength = packetLen;
  }
  else {
    vosMemCpy(tmpPacketPointer, packet, packetLen);
    tmpPacketLength = packetLen;
  }

  /************************** set header **************************/
  out_hdr =  (host_outbound_hdr_t *)aucPacket;
  out_hdr->reserved = 0;
  out_hdr->icos = 0;
  out_hdr->eport = egressPortId;

  tmpPacketLength += sizeof(host_outbound_hdr_t);

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "CTC output: send packet to %d, packetLength=%d.\r\n", egressPortId, tmpPacketLength-1);

  /************************** send to cpu **************************/
  eopl_host_send((void *)out_hdr, tmpPacketLength);
  return OPL_OK;
}


/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcCtcInput(MC_PACKET_DESC_t *desc)
{
  int ret = OPL_OK;

  //MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcCtcInput, enter.\r\n");


  switch(desc->igmpType) {
    case IGMP_TYPE_GENERAL_QUERY:
      #ifdef CTC_MULTICAST_SURPORT
      mc_printf("\nfunc:%s query\n",__FUNCTION__);
      #endif
      ret = mcCtcQueryAction(desc);
      break;
    case IGMP_TYPE_V1_REPORT:
      #ifdef CTC_MULTICAST_SURPORT
      mc_printf("\nfunc:%s v1 report\n",__FUNCTION__);
      #endif
      mcControlStats.joinReportV1RxCount++;
      ret = mcCtcJoinAction(desc);
      break;
    case IGMP_TYPE_V2_REPORT:
      #ifdef CTC_MULTICAST_SURPORT
      mc_printf("\nfunc:%s v2 report\n",__FUNCTION__);
      #endif
      mcControlStats.joinReportV2RxCount++;
      ret = mcCtcJoinAction(desc);
      break;
    case IGMP_TYPE_V2_LEAVE:
      #ifdef CTC_MULTICAST_SURPORT
      mc_printf("\nfunc:%s v2 leave\n",__FUNCTION__);
      #endif
      mcControlStats.leaveV2RxCount++;
      ret = mcCtcLeaveAction(desc);
      break;
    default:
      #ifdef CTC_MULTICAST_SURPORT
      mc_printf("\nfunc:%s default\n",__FUNCTION__);
      #endif
      if(desc->ingressPortId>=ODM_START_PORT_NUN&&desc->ingressPortId<=ODM_NUM_OF_PORTS) {
        mcControlStats.otherHostRxUnknownCount++;
      }
      else {
        mcControlStats.otherRouterRxUnknownCount++;
      }

      /************************ flooding to vlan ************************/
      /* TBD */
      //mcCtcOutput(desc, HOST_GENERATED_PTYPE_BROADCAST);

      break;
  }

  return ret;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcModeSwitch()
{
  uint32 i;
  MC_TAG_OPER_MODE_t tdMcTagOp;

  i = 0;

  mcControlBlockIsInitializedSet(OPL_FALSE);

  /* clear old configuration */
  odmMulticastControlEntryClear();
  for(i=ODM_START_PORT_NUN; i<ODM_NUM_OF_PORTS+1; i++) {
    odmMulticastVlanClear(i);
    
    /* 2012-9-21 */
    tdMcTagOp = mcTagOperPerPortGet(i);
    #if 0
    odmMulticastTagstripSet(i, MC_TAG_OPER_MODE_TRANSPARENT);
    #else
    odmMulticastTagstripSet(i, tdMcTagOp);
    #endif
    /*  014542 */
    
    odmMulticastVlanTranslateEntryClear(i);
#if 0 /* yxzhao added, bug 2391, 2010-01-13, 11:44:33 */
    odmMulticastGroupMaxNumSet(i, IGMP_MAX_GROUP_NUMBER_PER_PORT);
#endif
  }

  mcControlBlockIsInitializedSet(OPL_TRUE);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcInput(uint8 *ingressRawPkt, uint32 ingressRawPktLen, uint32 portId)
{
  int iRet;
  MC_PACKET_DESC_t desc;
  uint8 macStr[SMALL_BUF_LEN];

  eth_header_with_sgl_tag_t *sglEthHeader;
  ipv4_header_t *ipv4Header;
  igmp_msg_t *igmpMsg;
  uint8 *rawPkt;
  uint32 rawPktLen;

  RETURN_VAL_IF_FAIL(ingressRawPkt!=NULL, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(ingressRawPktLen<=1518, OPL_BAD_PARA);

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcInput, enter.\r\ningressRawPkt=0x%x,ingressRawPktLen=%d,portId=%d.\r\n", ingressRawPkt, ingressRawPktLen, portId);
  pktPrintDebug(ingressRawPkt, ingressRawPktLen);

  if(mcControlBlockRecvEnGet()==OPL_DISABLE) {
    return OPL_ERROR;
  }

  if(mcControlBlockIsInitializedGet()!=OPL_TRUE) {
    return OPL_ERROR;
  }

  vosMemSet(&desc, 0, sizeof(desc));
  sglEthHeader = NULL;
  ipv4Header = NULL;
  igmpMsg = NULL;
  rawPkt = NULL;
  rawPktLen = 0;

  /************************** debug **************************/
  //MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "portId=%d.\r\n", portId);
  //pktPrintDebug(ingressRawPkt, ingressRawPktLen);

  /************************** copy packet **************************/
  rawPkt = (uint8 *)vosAlloc(BIG_BUF_LEN);
  mcControlStats.memAllocCount++;
  if(rawPkt) {
    vosMemSet(rawPkt, 0, sizeof(BIG_BUF_LEN));
  }
  vosMemCpy(rawPkt, ingressRawPkt, ingressRawPktLen);
  rawPktLen = ingressRawPktLen;
  desc.ingressPacket = rawPkt;
  desc.ingressPacketLength = rawPktLen;
  desc.ingressPortId = portId;

  /* parse eth */
  sglEthHeader = (eth_header_with_sgl_tag_t *)rawPkt;

  vosMemCpy(desc.daMac, sglEthHeader->dest, ETH_MAC_ADDR_LEN);
  vosMemCpy(desc.saMac, sglEthHeader->src, ETH_MAC_ADDR_LEN);

  if(sglEthHeader->vlan_tag.type==VLAN_DEFAULT_STPID
    ||sglEthHeader->vlan_tag.type==VLAN_DEFAULT_CTPID) { /* single tag */
    desc.cTpid = sglEthHeader->vlan_tag.type;
    desc.cVlan = sglEthHeader->vlan_tag.vid;
    desc.ingressTagType = TAG_TYPE_SINGLE_TAG;
  }
  else { /* untag */
    desc.cTpid = 0;
    desc.cVlan = 0;
    desc.ingressTagType = TAG_TYPE_UNTAG;
  }

  /* parse ip */
  if(desc.ingressTagType == TAG_TYPE_SINGLE_TAG) {
    ipv4Header = (ipv4_header_t *)(rawPkt+18);
  }
  if(desc.ingressTagType == TAG_TYPE_UNTAG) {
    ipv4Header = (ipv4_header_t *)(rawPkt+14);
  }
  desc.destIp = ipv4Header->dest_addr;
  desc.srcIp = ipv4Header->src_addr;
   
if(ipv4Header->protocol==0x11)
  {
        vosFree(desc.ingressPacket);
        desc.ingressPacket = NULL;
        
        /* End   Added of porting */
        return OPL_ERROR;
  }


#ifdef CTC_MULTICAST_SURPORT
#if 0
  /* parse igmp */
  if(ipv4Header->ihl==5) { /* no options */
    desc.ingressIsHaveIpOptions = OPL_FALSE;
    igmpMsg = (igmp_msg_t *)((uint8 *)ipv4Header+20);
  }
  if(ipv4Header->ihl==6) { /* no options */
    desc.ingressIsHaveIpOptions = OPL_TRUE;
    igmpMsg = (igmp_msg_t *)((uint8 *)ipv4Header+24);
  }
#else
  
  if (ipv4Header->ihl<5){

    vosFree(desc.ingressPacket);
    desc.ingressPacket = NULL;
    /* End   Added of porting */
    return OPL_ERROR;
  }
  else if(ipv4Header->ihl==5) { /* no options */
    desc.ingressIsHaveIpOptions = OPL_FALSE;
  }
  else {
    desc.ingressIsHaveIpOptions = OPL_TRUE;
  }

  igmpMsg = (igmp_msg_t *)((uint8 *)ipv4Header + (4*(ipv4Header->ihl)));
#endif
#endif


#ifdef CTC_MULTICAST_SURPORT
mc_printf("\nigmp type:%d, rsptime:%d, groupadd:0x%08X.\n",
          igmpMsg->type,igmpMsg->max_resp_time,igmpMsg->group);
#endif

  desc.igmpType = igmpMsg->type;
  desc.igmpMaxRespTime = igmpMsg->max_resp_time;
  desc.igmpGroupAdress = igmpMsg->group;

  clMcMacGetByMcIp(desc.igmpGroupMac, desc.igmpGroupAdress);

  /* TBD */
  /* 可以根据报文解析igmp版本,v1,v2,v3 */

  /************************** igmp packet check **************************/
  /* check dest mac: 01 00 5e ... */
  if(!(desc.daMac[0]==0x01 && desc.daMac[1]==0x00 && desc.daMac[2]==0x5e)) {
    clStringGetByMac(macStr, desc.daMac);
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Error packet, da MAC error.\r\ndaMac=%s\r\n", macStr);
    vosFree(desc.ingressPacket);
    desc.ingressPacket = NULL;
    mcControlStats.memfreeCount++;
    return OPL_ERROR;
  }

  mcGroupSemLock();

  switch(mcControlBlock.mcMode) {
    case MC_MODE_SNOOPING:
      #ifdef CTC_MULTICAST_SURPORT
      mc_printf("\nfunc:%s igsp mode\n",__FUNCTION__);
      #endif
      iRet = mcSnoopingInput(&desc);
      break;
    case MC_MODE_CTC:
      #ifdef CTC_MULTICAST_SURPORT
      mc_printf("\nfunc:%s ctc mode\n",__FUNCTION__);
      #endif
      iRet = mcCtcInput(&desc);
      break;
    default:
      //return OPL_NOT_SUPPORTED;
      break;
  }

  mcGroupSemUnLock();

  if((desc.isNonFastLeaveTimerStart==OPL_FALSE) || (OPL_OK != iRet)) {
  //if(desc.isNonFastLeaveTimerStart==OPL_FALSE) {
    vosFree(desc.ingressPacket);
    desc.ingressPacket = NULL;
    mcControlStats.memfreeCount++;
  }
  /* else, enter non fast leave routine */

  return OPL_OK;

}

/**
* @par Description
* This function xxx.
* @param fd
* @param vlanId, if equal 0, show all.
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcSnoopingGroupShowByVlan(int fd, uint16 vlanId)
{
  uint32 i, j;
  clList *list, *hlist;
  MC_GROUP_t *node;
  MC_HOST_t *hnode;
  uint8 macStr[SMALL_BUF_LEN];
  uint8 ipStr[SMALL_BUF_LEN];
  uint8 string[SMALL_BUF_LEN];

  i = j = 0;
  list = hlist = NULL;
  node = NULL;
  hnode = NULL;
  vosMemSet(macStr, 0, sizeof(macStr));
  vosMemSet(ipStr, 0, sizeof(ipStr));

  /************************ router ports ************************/
  vosPrintf(fd, "    %-20s : ", "Router ports");
  for(i=0; i<MC_ROUTER_PORT_NUM; i++) {
    if(mcRouterPort[i].enable==OPL_ENABLE) {
      vosPrintf(fd, "%d ", mcRouterPort[i].routerPortId);
    }
  }
  vosPrintf(fd, "\r\n");

  mcGroupSemLock();

  /************************ groups ************************/
  /* check if the entry exists */
  list = mcGroupList;
  i = 0;
  while (list) {
    node = (MC_GROUP_t *)list->data;

    /* check vlan */
    if(vlanId!=node->mcVlan && vlanId!=0) {
      list = list->next;
      continue;
    }

    vosSprintf(string, "%s%d", "Multicast Group ", i);
    vosPrintf(fd, "  [%s]\r\n", string);

    if(node->groupMac!=NULL)
      clStringGetByMac(macStr, node->groupMac);
    if(MC_BITMAP&MC_BITMAP_GDA_MAC) {
      vosPrintf(fd, "GroupMAC: %s, ", macStr);
    }

    clStringGetByIp(ipStr, node->groupAddress);
    if(MC_BITMAP&MC_BITMAP_GDA_IP) {
      vosPrintf(fd, "GroupAddr: %s, ", ipStr);
    }
    vosPrintf(fd, "port: %d, vid: %d.\r\n", node->hostPortId, node->mcVlan);

    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "G%d: GMac:%s, GAddr:%s, McVlan:%d, HostPort:%d, HostNum:%d, state:%d, AgingTime:%d, isLastReportRecevied:%d, isNonFastLeaveTimerStart:%d.\n",
      i, macStr, ipStr, node->mcVlan,
      node->hostPortId, node->hostTotalNum, node->state, node->hostAgingTime,
      node->isLastReportRecevied, node->isNonFastLeaveTimerStart);

    hlist = node->hostList;
    j = 0;
    while (hlist) {
      hnode = (MC_HOST_t *)hlist->data;
      clStringGetByIp(ipStr, hnode->srcIp);
      vosPrintf(fd, "hostIP:%s;\r\n", ipStr);

      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "  H%d: SrcIp:%s, state:%d, AgingTime:%d, isLastReportRecevied:%d, isNonFastLeaveTimerStart:%d.\n",
        j, ipStr, hnode->state, hnode->hostAgingTime, hnode->isLastReportRecevied,
        hnode->isNonFastLeaveTimerStart);

      j++;
      hlist = hlist->next;
    }

    i++;
    list = list->next;
  }

  mcGroupSemUnLock();

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param fd
* @param vlanId, if equal 0, show all.
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcControlStatsShow(int fd)
{
  vosPrintf(fd, "  [%s]\r\n", "Multicast Statistics");
  vosPrintf(fd, "    %-20s : %d\r\n", "generalQueryV1RxCount", mcControlStats.generalQueryV1RxCount);
  vosPrintf(fd, "    %-20s : %d\r\n", "generalQueryV2RxCount", mcControlStats.generalQueryV2RxCount);
  vosPrintf(fd, "    %-20s : %d\r\n", "groupSpecificV2QueryRxCount", mcControlStats.groupSpecificV2QueryRxCount);
  vosPrintf(fd, "    %-20s : %d\r\n", "otherHostRxUnknownCount", mcControlStats.otherHostRxUnknownCount);
  vosPrintf(fd, "    %-20s : %d\r\n", "otherRouterRxUnknownCount", mcControlStats.otherRouterRxUnknownCount);
  vosPrintf(fd, "    %-20s : %d\r\n", "joinReportV1RxCount", mcControlStats.joinReportV1RxCount);
  vosPrintf(fd, "    %-20s : %d\r\n", "joinReportV2RxCount", mcControlStats.joinReportV2RxCount);
  vosPrintf(fd, "    %-20s : %d\r\n", "leaveV2RxCount", mcControlStats.leaveV2RxCount);
  vosPrintf(fd, "    %-20s : %d\r\n", "memAllocCount", mcControlStats.memAllocCount);
  vosPrintf(fd, "    %-20s : %d\r\n", "memfreeCount", mcControlStats.memfreeCount);

  return NO_ERROR;
}

/**
* @par Description
* This function xxx.
* @param fd
* @param vlanId, if equal 0, show all.
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcControlBlockShow(int fd)
{
  vosPrintf(fd, "Multicast configuration:\r\n");
  /* Section IGMP */
  vosPrintf(fd, "  [%s]\r\n", CFGSECTION_MULTICAST_IGMP_MANAGEMENT);
  vosPrintf(fd, "    %-20s : %d(0:disable,1:enable)\r\n", CFGKEY_MULTICAST_IGMP_MANAGEMENT_IGMP_MODE, mcControlBlock.mcRecvEn);
  vosPrintf(fd, "    %-20s : %d(0:SNOOPING,1:CTC)\r\n", CFGKEY_MULTICAST_IGMP_MANAGEMENT_MODE, mcControlBlock.mcMode);
  vosPrintf(fd, "    %-20s : %d(0:GDA_MAC,1:GDA_MAC_VID,2:GDA_SA_MAC,3:GDA_IP_VID)\r\n", CFGKEY_MULTICAST_IGMP_MANAGEMENT_CTC_CONTROL_TYPE, mcControlBlock.ctcControlType);
  vosPrintf(fd, "    %-20s : %d(1:NON_FAST_LEAVE,2:FAST_LEAVE)\r\n", "leave mode", mcControlBlock.leaveMode);
  vosPrintf(fd, "    %-20s : %ds\r\n", "host aging time", mcControlBlock.hostAgingTime);
  vosPrintf(fd, "    %-20s : %dms\r\n", "last query interval", mcControlBlock.lastQueryInterval);
  vosPrintf(fd, "    %-20s : %d\r\n", "last query count", mcControlBlock.lastQueryCount);

  return NO_ERROR;
}

/**
* @par Description
* This function xxx.
* @param fd
* @param vlanId, if equal 0, show all.
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS mcPortConfigShow(int fd, uint32 portId)
{
  int ret;
  uint32 i;
  char portStr[SMALL_BUF_LEN];

  vosMemSet(portStr, 0, sizeof(portStr));

  vosSprintf(portStr, "%s%d", "Port ", portId);
  vosPrintf(fd, "  [%s]\r\n", portStr);

  /* show mutlicast vlan */
  vosPrintf(fd, "    %-20s : ", "multicast vlan");

  for(i=1; i<OP_MAX_VLAN_NUMBER; i++) {
    if(mcVlanMapExist(portId, i)==OPL_TRUE) {
      vosPrintf(fd, "%d ", i);
    }
  }
  vosPrintf(fd, "\r\n");

  vosPrintf(fd, "    %-20s : %d(0:TRANSPARENT,1:STRIP,2:TRANSLATION)\r\n", "tagstrip mode", mcTagOperPerPortGet(portId));

  /************************** vlan translation **************************/
  vosPrintf(fd, "    %-20s : ","mcVlan->userVlan");
  for(i=0; i<MC_VLAN_TRANSLATION_ENTRY; i++) {
    if(mcTagTranslationTable[i].enable==OPL_ENABLE) {
      if(mcTagTranslationTable[i].portId == portId) { /* match */
        vosPrintf(fd, "%d->%d, ", mcTagTranslationTable[i].mcVid, mcTagTranslationTable[i].userVid);
      }
    }
  }
  vosPrintf(fd, "\r\n");

  vosPrintf(fd, "    %-20s : %d\r\n", "max group num", mcControlBlock.maxGroupNum[portId]);

  return NO_ERROR;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
static void * mcIgmpMsgThread(void *arg)
{
    MC_IGMP_MSG_t msg;
    uint32 length;

    while(mcIgmpMsgId >= 0)
    {
        length = vosMsqReceive(mcIgmpMsgId, (void *)&msg, MC_IGMP_MSG_SIZE);
        if (length == INVALID_32)
        {
            
            #ifdef CTC_MULTICAST_SURPORT
            mc_printf("\nRturn from mcIgmpMsgThread.\n");
            #endif
            
            break;
        }

        MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "vosMsqReceive for mcIgmpMsgThread:portId=%d.\r\n", msg.portId);

        
        #ifdef CTC_MULTICAST_SURPORT
        mc_printf("\nRecieve a igmp message.\n");
        #endif
       
        mcInput(msg.pBuff, msg.len, msg.portId);
        vosFree(msg.pBuff);
        mcControlStats.memfreeCount++;
        MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "mcInputMsgSend:vosFree.\r\n");
    }

    vosThreadExit(0);
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
static void * mcTimerMsgThread(void *arg)
{
    MC_TIMER_MSG_t msg;
    uint32 length;

    while(mcTimerMsgId >= 0)
    {
        length = vosMsqReceive(mcTimerMsgId, (void *)&msg, MC_TIMER_MSG_SIZE);
        if (length == INVALID_32)
        {
            break;
        }

        switch (msg.msgType)
        {
            case MC_HOST_AGING_TIMER_SNOOPING:
                mcHostAgingTimerSnoopingHandler();
                break;
            case MC_HOST_AGING_TIMER_CTC:
                mcHostAgingTimerCtcHandler();
                break;
            case MC_SNOOP_NON_FAST_LEAVE:
                mcSnoopingNonFastLeaveAction(msg.desc);
                break;
            case MC_CTC_LEAVE_ACTIVE:
                mcCtcNonFastLeaveAction(msg.desc);
                break;
            default:
                printf("%s[%d]: Invalid Mc Timer msgType.\n", __FUNCTION__, __LINE__);
                break;
        }
    }

    vosThreadExit(0);
}

/********************************************************************
 *
 * code for old implementation
 *
*********************************************************************/

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS odmMulticastInit()
{
  uint32 i;

  VOS_THREAD_t mcMsgThreadId = NULL;

  mcControlBlockIsInitializedSet(OPL_FALSE);

  /* function layer */
  mcControlBlockInit();

  mcVlanMapInit();
  mcTagOperPerPortInit();
  mcTagTranslationTableInit();
  mcRouterPortInit();
  odmMulticastControlEntryClear();
  mcControlStatsInit();

  /* driver layer */
  
  #ifndef CTC_MULTICAST_SURPORT
  dalMulticastInit();
  #else
  {
  extern INT32 DRV_SwitchMcInit(void);
  INT32 Ret;
  Ret = DRV_SwitchMcInit();
  if (NO_ERROR != Ret)
  {
    mc_printf("\nfunc:%s,line:%d. Failed to init multicast.\n",__FUNCTION__,__LINE__);
    return OPL_FALSE;
  }
  }
  #endif
  
  

  /* get from config file */
  mcInitFromConfigFile();

  mcIgmpMsgId = vosMsqCreate(sizeof(MC_IGMP_MSG_t));
  if (mcIgmpMsgId == -1)
  {
      printf("%s[%d], vosMsqCreate fail.\n", __FUNCTION__, __LINE__);
      return OPL_FALSE;
  }

  mcMsgThreadId = vosThreadCreate("tMcIgmp", OP_VOS_THREAD_STKSZ, 60,
                  (void *)mcIgmpMsgThread, (void *)NULL);
  if (mcMsgThreadId == NULL)
  {
      printf("%s[%d], vosThreadCreate fail.\n", __FUNCTION__, __LINE__);
      return OPL_FALSE;
  }

  mcTimerMsgId = vosMsqCreate(sizeof(MC_TIMER_MSG_t));
  if (mcTimerMsgId == -1)
  {
      printf("%s[%d], vosMsqCreate fail.\n", __FUNCTION__, __LINE__);
      return OPL_FALSE;
  }

  mcMsgThreadId = NULL;
  mcMsgThreadId = vosThreadCreate("tMcTimer", OP_VOS_THREAD_STKSZ, 100,
                  (void *)mcTimerMsgThread, (void *)NULL);
  if (mcMsgThreadId == NULL)
  {
      printf("%s[%d], vosThreadCreate fail.\n", __FUNCTION__, __LINE__);
      return OPL_FALSE;
  }

  mcControlBlockIsInitializedSet(OPL_TRUE);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
int odmMulticastVlanAdd(uint32 portId, uint32 vlanId)
{
  int ret = 0;
  uint32 i;
  uint32 port_array[ODM_NUM_OF_PORTS+1];

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastVlanAdd: portId=%d,vlanId=%d.\r\n",
    portId, vlanId);

  RETURN_VAL_IF_FAIL(IS_VALID_PORT_ID(portId)==OPL_TRUE||portId==255, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(IS_VALID_VLAN_ID(vlanId)==OPL_TRUE, OPL_BAD_PARA);

  /*  */
  /*Mc vlan can not be same with ctc vlan.*/
  #ifdef CTC_MULTICAST_SURPORT
  {
    extern BOOL CTC_CheckValidVidForMcvlan(uint32 uiMcVid);
    if (FALSE == CTC_CheckValidVidForMcvlan(vlanId))
    {
        return OPL_ERROR;
    }
  }
  #endif
  /* 014040 */
  
  vosMemSet(port_array, 0, sizeof(port_array));

  if(portId == 255) {
    for(i = ODM_START_PORT_NUN; i < ODM_NUM_OF_PORTS+1; i++) {
      port_array[i] = 1;
    }
  }
  else {
      port_array[portId] = 1;
  }

  for(i = ODM_START_PORT_NUN; i < ODM_NUM_OF_PORTS+1; i++) {
    if(port_array[i] == 1) {
      odmMcVlanMapAdd(i, vlanId);
    }
  }

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
int odmMulticastVlanDelete(uint32 portId, uint32 vlanId)
{
  int ret;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastVlanDelete: portId=%d,vlanId=%d.\r\n",
    portId, vlanId);

  RETURN_VAL_IF_FAIL(IS_VALID_PORT_ID(portId)==OPL_TRUE||portId==255, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(IS_VALID_VLAN_ID(vlanId)==OPL_TRUE, OPL_BAD_PARA);

  ret = 0;

  if(mcVlanMapExist(portId, vlanId)) {
    ret = odmMcVlanMapDel(portId, vlanId);
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
  }

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
int odmMulticastVlanClear(uint32 portId)
{
  uint32 i, j;
  int ret;
  uint32 port_array[ODM_NUM_OF_PORTS+1];

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastVlanClear: portId=%d.\r\n", portId);

  RETURN_VAL_IF_FAIL(IS_VALID_PORT_ID(portId)==OPL_TRUE||portId==255, OPL_BAD_PARA);

  ret = 0;
  vosMemSet(port_array, 0, sizeof(port_array));

  if(portId == 255) { /* equal to all */
    for(i = ODM_START_PORT_NUN; i < ODM_NUM_OF_PORTS+1; i++) {
      port_array[i] = 1;
    }
  }
  else {
    port_array[portId] = 1;
  }

  for(i = ODM_START_PORT_NUN; i < ODM_NUM_OF_PORTS+1; i++) {
    if(port_array[i] == 1) {
      for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
        if(mcVlanMapExist(i, j)) {
          ret = odmMcVlanMapDel(i, j);
        }
      }
      /* 014531 */
      mcGroupListDelbyPort(i);
      /*  */
    }
  }

  return NO_ERROR;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
int odmMulticastVlanGet(uint32 portId, uint16 *vlanId, uint8 *num)
{

  uint32 j;
  uint8 vlanIdNum = 0;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastVlanGet: portId=%d, vlanId=0x%X, num=0x%X.\r\n",
    portId, vlanId, num);

  RETURN_VAL_IF_FAIL(IS_VALID_PORT_ID(portId)==OPL_TRUE||portId==255, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(vlanId!=NULL, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(num!=NULL, OPL_BAD_PARA);

  for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
    if(OPL_TRUE==mcVlanMapExist(portId, j)) {
      *vlanId = j;
      vlanId++;
      vlanIdNum++;
    }
  }

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastVlanGet: portId=%d, num=%d.\r\n",
    portId, vlanIdNum);

  *num = vlanIdNum;

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
int odmMulticastTagstripSet(uint32 portId, uint32 tagOper)
{
  uint32 i;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastTagstripSet: portId=%d, tagOper=%d.\r\n",
    portId, tagOper);

  if (IS_ONU_RESPIN)
  {
    /*avoid silira olt reg problem*/
    if (0 == portId) return NO_ERROR;
  }
  RETURN_VAL_IF_FAIL(IS_VALID_PORT_ID(portId)==OPL_TRUE||portId==255, OPL_BAD_PARA);

  if(portId == 255) { /* equal to all */
    for(i = ODM_START_PORT_NUN; i < ODM_NUM_OF_PORTS+1; i++) {
      odmMcTagOperSet(i, tagOper);
    }
  }
  else {
    odmMcTagOperSet(portId, tagOper);
  }

  return NO_ERROR;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
int odmMulticastTagstripGet(uint32 portId, uint8 *tagOper)
{
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastTagstripGet: portId=%d, tagOper=0x%X.\r\n",
    portId, tagOper);

  RETURN_VAL_IF_FAIL(IS_VALID_PORT_ID(portId)==OPL_TRUE||portId==255, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(tagOper!=NULL, OPL_BAD_PARA);

  *tagOper = mcTagOperPerPortGet(portId);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
int odmMulticastTagTranslationAdd(uint32 portId, uint32 mcVid, uint32 userVid)
{
  uint32 i;
  int ret;

  ret = 0;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastTagTranslationSet: portId=%d, mcVid=%d, userVid=%d.\r\n",
    portId, mcVid, userVid);

  RETURN_VAL_IF_FAIL(IS_VALID_PORT_ID(portId)==OPL_TRUE||portId==255, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(odmVlanRangeCheck(mcVid)==OK, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(odmVlanRangeCheck(userVid)==OK, OPL_BAD_PARA);

  if(portId == 255) { /* equal to all */
    for(i = ODM_START_PORT_NUN; i < ODM_NUM_OF_PORTS+1; i++) {
      odmMcTagTranslationSet(i, mcVid, userVid);
    }
  }
  else {
    ret = odmMcTagTranslationSet(portId, mcVid, userVid);
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
  }

  return NO_ERROR;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
int odmMulticastTagTranslationGet(uint32 portId, oam_mcast_vlan_translation_entry_t *p_tranlation_entry, uint8 *num)
{
  uint32 i;
  uint32 entryNum;
  oam_mcast_vlan_translation_entry_t *tmp;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastTagTranslationGet: portId=%d, p_tranlation_entry=0x%08X, num=0x%08X.\r\n",
    portId, p_tranlation_entry, num);

  RETURN_VAL_IF_FAIL(IS_VALID_PORT_ID(portId)==OPL_TRUE, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(p_tranlation_entry!=NULL, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(num!=NULL, OPL_BAD_PARA);

  entryNum = 0;
  tmp = p_tranlation_entry;

  for(i=0; i<MC_VLAN_TRANSLATION_ENTRY; i++) {
    if(mcTagTranslationTable[i].enable==OPL_ENABLE) {
      if(mcTagTranslationTable[i].portId == portId) { /* match */
        tmp->mcast_vlan = mcTagTranslationTable[i].mcVid;
        tmp->iptv_vlan = mcTagTranslationTable[i].userVid;

        tmp++;
        entryNum++;
      }
    }
  }

  *num = entryNum;

  tmp = p_tranlation_entry;
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastTagTranslationGet: portId=%d, num=%d.\r\n",
    portId, *num);
  for(i=0; i<entryNum; i++) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastTagTranslationGet: portId=%d, index=%d, mvlan=%d, uvlan=%d.\r\n",
      portId, i, tmp->mcast_vlan, tmp->iptv_vlan);
    tmp++;
  }

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
int odmMulticastVlanTranslateEntryClear(uint32 portId)
{
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastVlanTranslateEntryClear: portId=%d.\r\n",
    portId);

  RETURN_VAL_IF_FAIL(IS_VALID_PORT_ID(portId)==OPL_TRUE||portId==255, OPL_BAD_PARA);

  odmMcTagTranslationClear(portId);

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
int odmMulticastSwitchSet(uint32 mode)
{
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastSwitchSet: mode=%d.\r\n", mode);

  if(mode==0) { /* MC_MODE_SNOOPING */
    if(mcControlBlockModeGet()!=MC_MODE_SNOOPING) {
      odmMcControlBlockModeSet(MC_MODE_SNOOPING);
      mcModeSwitch();
    }
#ifdef CTC_MULTICAST_SURPORT
mc_printf("\nfunc:%s sets igsp mode.\n",__FUNCTION__);
#endif
  }
  if(mode==1) { /* MC_MODE_CTC */
    if(mcControlBlockModeGet()!=MC_MODE_CTC) {
      odmMcControlBlockModeSet(MC_MODE_CTC);
      mcModeSwitch();
    }
#ifdef CTC_MULTICAST_SURPORT
mc_printf("\nfunc:%s sets ctc control mode.\n",__FUNCTION__);
#endif
  }

  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
int odmMulticastSwitchGet()
{
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastSwitchGet: mode=%d.\r\n",
    mcControlBlockModeGet());

  if(mcControlBlockModeGet()==MC_MODE_SNOOPING) {
    return 0;
  }
  if(mcControlBlockModeGet()==MC_MODE_CTC) {
    return 1;
  }

  return OPL_ERROR;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
int odmMulticastGroupMaxNumSet(uint32 portId, uint32 num)
{
  uint32 i;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastGroupMaxNumSet: portId=%d, num=%d.\r\n",
    portId, num);

  if (IS_ONU_RESPIN)
  {
    /*avoid silira tk kick off*/
    if (portId==0) {
  	  return OPL_OK;
    }
  }
  RETURN_VAL_IF_FAIL(IS_VALID_PORT_ID(portId)==OPL_TRUE||portId==255, OPL_BAD_PARA);

   /* 14055 */
  if(ODM_MAX_GRUP_NUM_PER_PORT < num)
  {
      MC_DEBUG(DEBUG_LEVEL_CRITICAL, "odmMulticastGroupMaxNumSet: num=%d.\r\n", num);  
      return ERR_INVALID_PARAMETERS;
  }
  /* */

  if(portId == 255) { /* equal to all */
    for(i = ODM_START_PORT_NUN; i < ODM_NUM_OF_PORTS+1; i++) {
      odmMcControlBlockMaxGroupNumSet(i, num);
    }
  }
  else {
    odmMcControlBlockMaxGroupNumSet(portId, num);
  }

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastGroupMaxNumSet: num=%d.\r\n", num);
  return OPL_OK;
}

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
uint8 odmMulticastGroupMaxNumGet(uint32 portId)
{
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastGroupMaxNumGet: portId=%d, num=%d.\r\n",
    portId, mcControlBlockmaxGroupNumGet(portId));

  RETURN_VAL_IF_FAIL(IS_VALID_PORT_ID(portId)==OPL_TRUE||portId==255, OPL_BAD_PARA);

  return mcControlBlockmaxGroupNumGet(portId);
}

int odmMulticastFastleaveAdminStateGet()
{
  uint32 leaveMode;

  leaveMode = mcControlBlockLeaveModeGet();

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastFastleaveAdminStateGet: leaveMode=%d.\r\n",
    leaveMode);

  if(leaveMode==MC_LEAVE_MODE_FAST) {
    return 0;
  }
  if(leaveMode==MC_LEAVE_MODE_NON_FAST_LEAVE) {
    return 1;
  }

  return OPL_ERROR;
}

int odmMulticastFastleaveModeSet(uint32 enable)
{
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastFastleaveModeSet: enable=%d.\r\n", enable);

  if(enable==1) {
    odmMcControlBlockLeaveModeSet(MC_LEAVE_MODE_FAST);
  }

  if(enable==0) {
    odmMcControlBlockLeaveModeSet(MC_LEAVE_MODE_NON_FAST_LEAVE);
  }

  return OPL_OK;
}

int odmMulticastControlTypeSet(multicast_control_e type)
{
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastControlTypeSet: type=%d.\r\n", type);

  odmMcControlBlockCtcControlTypeSet(type);

  return NO_ERROR;
}

multicast_control_e odmMulticastControlTypeGet(void)
{
#if 0
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastControlTypeGet: type=%d.\r\n", mcControlBlockCtcControlTypeGet());
#endif

  return mcControlBlockCtcControlTypeGet();
}

int odmMulticastControlEntryAdd(uint32 portId, uint32 vlanId, uint8 *mac)
{
  uint8 daMacStr[SMALL_BUF_LEN];
  MC_GROUP_t *group;
  int ret;
  uint32 groupAddress;

  RETURN_VAL_IF_FAIL(IS_VALID_PORT_ID(portId)==OPL_TRUE||portId==255, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(IS_VALID_VLAN_ID(vlanId)==OPL_TRUE, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(mac!=NULL, OPL_BAD_PARA);

  vosMemSet(daMacStr, 0, sizeof(daMacStr));
  group = NULL;
  ret = 0;
  groupAddress = 0;

  clStringGetByMac(daMacStr, mac);

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastControlEntryAdd: portId=%d, vlanId=%d, mac=%s.\r\n",
    portId, vlanId, daMacStr);

  if(mcControlBlockModeGet()!=MC_MODE_CTC) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Not in CTC mode.\r\n");
    return OPL_ERROR;
  }


#if defined(ONU_1PORT)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID
    || odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID) {
	ret = odmMulticastVlanAdd(portId, vlanId);
	RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
   }
#endif
  mcGroupSemLock();

#if 0 /* yxzhao added, don't check max group limit in CTC mode, 2009-12-31, 13:22:02 */
  /* check max group limit */
  if(mcGroupNumPerPort(portId)==mcControlBlockmaxGroupNumGet(portId)) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Reach max group number.\r\ngroupNum=%d.\r\n",
      mcControlBlockmaxGroupNumGet(portId));

    mcGroupSemUnLock();

    return OPL_ERROR;
  }
#endif

  /************************ group add ************************/
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC
    || odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID) {
    /************************ MC_CTL_GDA_MAC or MC_CTL_GDA_MAC_VID ************************/
    ret = odmMcGroupAdd(portId, mac, 0, vlanId, GROUP_STATE_CONFIG_BY_OAM, MC_BITMAP);

    if(ret!=OPL_OK) {mcGroupSemUnLock(); }

    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
  }
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID) {
    /************************ MC_CTL_GDA_GDA_IP_VID ************************/
    groupAddress = *(uint32 *)&mac[2];
    ret = odmMcGroupAdd(portId, 0, groupAddress, vlanId, GROUP_STATE_CONFIG_BY_OAM, MC_BITMAP);

    if(ret!=OPL_OK) {mcGroupSemUnLock(); }

    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
  }

  mcGroupSemUnLock();
#if 0
#if defined(ONU_1PORT)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID
    || odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID) {
	ret = odmMulticastVlanAdd(portId, vlanId);
	RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
   }
#endif
#endif
  return NO_ERROR;
}

int odmMulticastControlEntryDelete(uint32 portId, uint32 vlanId, uint8 *mac)
{
  uint8 daMacStr[SMALL_BUF_LEN];
  MC_GROUP_t *group;
  int ret;
  uint32 groupAddress;
  
  #if 0
  multicast_control_entry_t ctl_entry_list[256];
  #else
  multicast_control_entry_t ctl_entry_list[MC_MAX_GROUP_NUM];
  #endif
 
  
  UINT16 num, m;
  int del;

  RETURN_VAL_IF_FAIL(IS_VALID_PORT_ID(portId)==OPL_TRUE||portId==255, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(IS_VALID_VLAN_ID(vlanId)==OPL_TRUE, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(mac!=NULL, OPL_BAD_PARA);

  vosMemSet(daMacStr, 0, sizeof(daMacStr));
  group = NULL;
  ret = 0;
  vosMemSet(daMacStr, 0, sizeof(daMacStr));
  groupAddress = 0;
  num = 0;

  clStringGetByMac(daMacStr, mac);

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastControlEntryDelete: portId=%d, vlanId=%d, mac=%s.\r\n",
    portId, vlanId, daMacStr);

  if(mcControlBlockModeGet()!=MC_MODE_CTC) {
    return OPL_ERROR;
  }

  mcGroupSemLock();

  /************************ group get ************************/
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC
    || odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID) {
    /************************ MC_CTL_GDA_MAC or MC_CTL_GDA_MAC_VID ************************/
    group = mcGroupListGet(portId, mac, 0, vlanId, MC_BITMAP);
  }
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID) {
    /************************ MC_CTL_GDA_GDA_IP_VID ************************/
    groupAddress = *(uint32 *)&mac[2];
    group = mcGroupListGet(portId, 0, groupAddress, vlanId, MC_BITMAP);
  }

  if(!group) { /* gruop does not exist */
    clStringGetByMac(daMacStr, mac);
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING,
      "Group does not exist.\r\nportId=%d, groupMac=%s, groupAdress=%s, vlan=%d.\r\n",
      portId, daMacStr, inet_ntoa(*(struct in_addr *)&groupAddress), vlanId);

    mcGroupSemUnLock();
    //return OPL_ERROR;
    return OPL_OK;
  }

  /* check if the host list is empty */
  if(OPL_TRUE==mcGroupListHostListEmpty(group->hostPortId, group->groupMac, group->groupAddress,
    group->mcVlan, MC_BITMAP)) { /* the host list is empty */

    /* delete group node */
    printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
    ret = odmMcGroupDel(group->hostPortId, group->groupMac, group->groupAddress,
      group->mcVlan, MC_BITMAP);
    if(ret!=OPL_OK) { mcGroupSemUnLock(); }

    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

    group = NULL;
  }

  mcGroupSemUnLock();

#if defined(ONU_1PORT)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID
    || odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID) {
       odmMulticastControlEntryGet(ctl_entry_list, &num);

       del = OPL_TRUE;
       for (m = 0; m < num; m++)
       {
         if ((ctl_entry_list[m].port == portId) && (ctl_entry_list[m].vid == vlanId))
         {
            del = OPL_FALSE;
            break;
         }
       }

       if (del)
       {
         ret = odmMulticastVlanDelete(portId, vlanId);
         if (OPL_OK != ret)
         {
            #ifdef CTC_MULTICAST_SURPORT
            mc_printf("\nfunc:%s, line:%d\n",__FUNCTION__,__LINE__);
            #endif
            return OPL_ERROR;
         }
       }
   }
#endif

  return OPL_OK;
}

int odmMulticastControlEntryDeleteByVlan(uint32 vlanId)
{
  int ret;
  clList *list;
  MC_GROUP_t *group;
  uint32 groupMatched;
  uint32 i;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastControlEntryDeleteByVlan: vlanId=%d.\r\n",
    vlanId);

  RETURN_VAL_IF_FAIL(IS_VALID_VLAN_ID(vlanId)==OPL_TRUE, OPL_BAD_PARA);

  ret = 0;
  list = NULL;
  group = NULL;
  groupMatched = OPL_FALSE;

  mcGroupSemLock();

  list = mcGroupList;

  while(list) {
    while(list) {
      group = (MC_GROUP_t *)list->data;

      if(group->mcVlan==vlanId) {
        groupMatched = OPL_TRUE;

        ret = odmMcGroupDel(group->hostPortId, group->groupMac, group->groupAddress,
          group->mcVlan, MC_BITMAP);

        if(ret!=OPL_OK) { mcGroupSemUnLock(); }

        RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

        group = NULL;

        break;
      }

      list = list->next;
    }

    if(groupMatched==OPL_TRUE) {
      list = mcGroupList;
      groupMatched = OPL_FALSE;
    }
    else {
      break;
    }
  }

  mcGroupSemUnLock();

#if defined(ONU_1PORT)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID
    || odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID) {

    for(i = ODM_START_PORT_NUN; i < ODM_NUM_OF_PORTS+1; i++) {
	ret = odmMulticastVlanDelete(i, vlanId);
	//RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
    }
   }
#endif

  return NO_ERROR;
}

int odmMulticastControlEntryClear()
{
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastControlEntryClear: .\r\n");

  int ret;
  clList *list;
  MC_GROUP_t *group;
  uint32 i, j;

  ret = 0;
  list = NULL;
  group = NULL;

  mcGroupSemLock();

  dalMulticastRuleDelAll();

  list = mcGroupList;

  /* dal del */
  while(list) {
    group = (MC_GROUP_t *)list->data;

    ret = dalMcGroupDel(group->hostPortId, group->groupMac, group->groupAddress,
      group->mcVlan, MC_BITMAP);

    if(ret!=OPL_OK) { mcGroupSemUnLock(); }

    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

    list = list->next;
  }

  /* fal */
  mcGroupListDestory();

  mcGroupSemUnLock();

#if defined(ONU_1PORT)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID
    || odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID) {
    for(i = ODM_START_PORT_NUN; i < ODM_NUM_OF_PORTS+1; i++) {
	ret = odmMulticastVlanClear(i);
	if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
    }
   }
#endif

  return OPL_OK;
}

int odmMulticastControlEntryGet(multicast_control_entry_t *ctl_entry_list, UINT16 *num)
{
  multicast_control_entry_t *ptr_entry;
  UINT16 entry_number;
  clList *list;
  MC_GROUP_t *node;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastControlEntryGet: ctl_entry_list=0x%0X, num=0x%X.\r\n",
    ctl_entry_list, num);

  if((OPL_NULL == num)||(OPL_NULL == ctl_entry_list))
    return OPL_ERROR;

  entry_number = 0;
  list = NULL;
  node = NULL;

  mcGroupSemLock();

  ptr_entry = ctl_entry_list;

  /* check if the entry exists */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;

	/*014059 */
	#if 0
    /************************ group get ************************/
    if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC
      || odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID) {
      /************************ MC_CTL_GDA_MAC or MC_CTL_GDA_MAC_VID ************************/
      vosMemCpy(ptr_entry->mmac, node->groupMac, MAC_ADDRESS_LENGTH);
    }
    if(odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID) {
      /************************ MC_CTL_GDA_GDA_IP_VID ************************/
      vosMemCpy(&ptr_entry->mmac[2], &node->groupAddress, 4);
    }
    #else
	if (mcControlBlockModeGet()==MC_MODE_SNOOPING)
    {
      vosMemCpy(&ptr_entry->mmac[2], &node->groupAddress, 4);
    }
    else
    {
      vosMemCpy(ptr_entry->mmac, node->groupMac, MAC_ADDRESS_LENGTH);
    }
	#endif
	/* 014059 */
    ptr_entry->port = node->hostPortId;
    ptr_entry->vid = node->mcVlan;
    entry_number++;
    ptr_entry++;

    list = list->next;
  }

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastControlEntryGet: ctl_entry_list=0x%0X, num=%d.\r\n",
    ctl_entry_list, entry_number);

  *num = entry_number;

  mcGroupSemUnLock();

  return OPL_OK;
}

/*  */
#if defined(CONFIG_PRODUCT_EPN104)
int odmIgspMacGroupTableGet(multicast_control_entry_t *ctl_entry_list, UINT16 *num)
{
  multicast_control_entry_t *ptr_entry;
  UINT16 entry_number;
  clList *list;
  MC_GROUP_t *node;
  uint8 tempGroupMac[MAC_ADDRESS_LENGTH];

  if((OPL_NULL == num)||(OPL_NULL == ctl_entry_list))
    return OPL_ERROR;

  entry_number = 0;
  list = NULL;
  node = NULL;

  mcGroupSemLock();

  ptr_entry = ctl_entry_list;

  /* check if the entry exists */
  list = mcGroupList;
  while (list) {
    node = (MC_GROUP_t *)list->data;

    vosMemSet(tempGroupMac, 0, MAC_ADDRESS_LENGTH);
    clMcMacGetByMcIp(tempGroupMac, node->groupAddress);

    vosMemCpy(ptr_entry->mmac, tempGroupMac, MAC_ADDRESS_LENGTH);
   
    ptr_entry->port = node->hostPortId;
    ptr_entry->vid = node->mcVlan;
    entry_number++;
    ptr_entry++;

    list = list->next;
  }
  *num = entry_number;

  mcGroupSemUnLock();

  return OPL_OK;
}
#endif
/* END:  014257 */

int odmMulticastHostAgingTimeSet(uint32 hostAgingTime)
{
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastHostAgingTimeSet: hostAgingTime=%d.\r\n", hostAgingTime);

  if(mcControlBlockModeGet()!=MC_MODE_SNOOPING) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Not in Snooping mode.\r\n");
    return OPL_ERROR;
  }

  odmMcControlBlockHostAgingTimeSet(hostAgingTime);
  mcHostAgingTimerSet(hostAgingTime);

  return OPL_OK;
}

uint32 odmMulticastControlEntryNumGet(uint32 portId)
{
  uint32 ret = OPL_ZERO;
  clList *list = NULL;
  MC_GROUP_t *group;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastControlEntryNumGet: portId=%d.\r\n",
    portId);

  if(IS_VALID_PORT_ID(portId)==OPL_FALSE)
  {
    MC_DEBUG(DEBUG_LEVEL_WARNING, "odmMulticastControlEntryNumGet: invalid portId.\r\n");
    return ret;
  }

  mcGroupSemLock();

  list = mcGroupList;

  while(list) {
    group = (MC_GROUP_t *)list->data;
    if(group && group->hostPortId == portId) {
      ret++;
    }
    list = list->next;
  }

  mcGroupSemUnLock();

  return ret;
}

int odmMulticastControlEntryNumSet(uint32 portId, uint32 num)
{
  int ret;
  clList *list;
  MC_GROUP_t *group;
  uint32 groupMatched;
  uint32 delEntryNum, entryNum;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastControlEntryNumSet: portId=%d,num=%d.\r\n",
    portId, num);

  if(IS_VALID_PORT_ID(portId)==OPL_FALSE)
  {
    MC_DEBUG(DEBUG_LEVEL_WARNING, "odmMulticastControlEntryNumSet: invalid portId.\r\n");
    return OPL_BAD_PARA;
  }

  ret = 0;
  list = NULL;
  group = NULL;
  groupMatched = OPL_FALSE;
  entryNum = 0;
  delEntryNum = 0;

  mcGroupSemLock();

  list = mcGroupList;

  while(list) {
    group = (MC_GROUP_t *)list->data;
    if(group && group->hostPortId == portId) {
      entryNum++;
    }
    list = list->next;
  }

  if (entryNum <= num)
  {
    mcGroupSemUnLock();
    return NO_ERROR;
  }

  delEntryNum = entryNum - num;

  list = mcGroupList;

  while(list) {
    while(list) {
      group = (MC_GROUP_t *)list->data;

      if(group &&
        group->hostPortId == portId &&
        delEntryNum > 0) {
        groupMatched = OPL_TRUE;

        ret = odmMcGroupDel(group->hostPortId, group->groupMac, group->groupAddress,
          group->mcVlan, MC_BITMAP);

        if(ret!=OPL_OK) { mcGroupSemUnLock(); }

        RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

        delEntryNum--;

        group = NULL;

        break;
      }

      list = list->next;
    }

    if(groupMatched==OPL_TRUE) {
      list = mcGroupList;
      groupMatched = OPL_FALSE;
    }
    else {
      break;
    }
  }

  mcGroupSemUnLock();

  return NO_ERROR;
}


/* from onu */
/*Used for DMA1(host mii)*/
int odmMulticastIgmpMsgHandler(void *ptr_frm, uint32 len)
{
  host_inbound_hdr_t *ptr_inb_hdr;
  uint32 i_port_logic;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastIgmpMsgHandler:len=%d.\r\n", len);

  RETURN_VAL_IF_FAIL(ptr_frm!=NULL, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(len<=1518+4, OPL_BAD_PARA);

  
  #ifndef CTC_MULTICAST_SURPORT
  {
  host_inbound_hdr_t *ptr_inb_hdr;
  
  ptr_inb_hdr = (host_inbound_hdr_t *)ptr_frm;
  if(ptr_inb_hdr->iport == PORT_GE_PHYSICAL)
    i_port_logic = ODM_START_PORT_NUN;
  else if(ptr_inb_hdr->iport == PORT_PON_PHYSICAL)
    i_port_logic = PORT_PON_LOGIC;
  else {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastIgmpMsgHandler:Ingress port error.\r\nphysicalPort=%d.\r\n", ptr_inb_hdr->iport);
    return OPL_ERROR;
  }

  /************************** debug **************************/
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastIgmpMsgHandler:len=%d, port=%d.\r\n", len-4, i_port_logic);
  pktPrintDebug(ptr_frm, len);

  mcInputMsgSend(ptr_frm+4, len-4, i_port_logic);
  }
  #else
  {
  extern uint32 DRV_GetSwitchHeaderLen(void);
  //extern uint32 DRV_PortPhysical2Logical(uint32 uiPPort);
  extern int32 DRV_GetPortFromSwitchHeader(uint8 *pucData, uint32 *puiPPort);
  uint32 uiRltHeaderLen;
  uint32 ret;

  mc_printf("odmMulticastIgmpMsgHandler\r\n");

  /*In fact, this is physical port.*/
  ret = DRV_GetPortFromSwitchHeader((ptr_frm + 12), &i_port_logic);
  if (0 != ret)
  {
    mc_printf("\nFailed to get port id from header.\n");
    return OPL_ERROR;
  }
  //i_port_logic = DRV_PortPhysical2Logical(i_port_logic);
  if (0xFF == i_port_logic)
  {
    mc_printf("\nFailed to change phy port to logic port.\n");
    return OPL_ERROR;
  }
  
  /************************** debug **************************/
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastIgmpMsgHandler:len=%d, port=%d.\r\n", len-4, i_port_logic);
  pktPrintDebug(ptr_frm, len);

  /*There is not opulan header.*/
  mc_printf("\nSend msg to mcIgmpMsgThread.\n");

  uiRltHeaderLen = DRV_GetSwitchHeaderLen();
  if (len > (uiRltHeaderLen+12))
  {
    len -= uiRltHeaderLen;
  }
  else
  {
    mc_printf("\nError packet length.\n");
    return OPL_ERROR;
  }

  memcpy(ptr_frm+12, ptr_frm+12+uiRltHeaderLen, len-12);
  mcInputMsgSend(ptr_frm, len, i_port_logic);
  }
  #endif
  

  return OPL_OK;
}

/* from marvell or asseros */
/*Used for DMA0(pon and gmac)*/
int odmMulticastIgmpMsgHandler_MultiPort(
  UINT8 portNum,UINT16 vlan,UINT8 withTag,UINT8 *payLoad,UINT16 len, uint8 *raw_pkt)
{
  uint32 packetLength;

  RETURN_VAL_IF_FAIL(payLoad!=NULL, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(raw_pkt!=NULL, OPL_BAD_PARA);
  RETURN_VAL_IF_FAIL(len<=1518+4, OPL_BAD_PARA);

  if(!(portNum==PORT_PON_LOGIC
    || IS_VALID_PORT_ID(portNum))) {
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastIgmpMsgHandler_MultiPort:Ingress port error.\r\nportNum=%d.\r\n", portNum);
    return OPL_ERROR;
  }

  if(portNum==PORT_PON_LOGIC) {
    /************************** debug **************************/
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastIgmpMsgHandler_MultiPort:from pon, len=%d, port=%d.\r\n", len, portNum);
    pktPrintDebug(payLoad, len);
    /************************** from router **************************/
    /*  */
    #if 0
    mcInputMsgSend(payLoad, len, portNum);
    #else
    if(withTag==1) {
      packetLength = 12 + 4 + len;
    }
    else {
      packetLength = 12 + len;
    }
    mcInputMsgSend(raw_pkt+4, packetLength, portNum);
    #endif
    /* 014054 */
  }
  else {
    /************************** from hosts **************************/
    if(withTag==1) {
      packetLength = 12 + 4 + len;
    }
    else {
      packetLength = 12 + len;
    }

    /************************** debug **************************/
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmMulticastIgmpMsgHandler_MultiPort:from host, len=%d, port=%d, withtag=%d.\r\n", packetLength, portNum,withTag);
    pktPrintDebug(raw_pkt+4, packetLength);

    mcInputMsgSend(raw_pkt+4, packetLength, portNum);
  }

  return NO_ERROR;
}


OPL_STATUS mcTagTranslationTableCheck(uint32 portId, uint32 mcVid, uint32 userVid)
{
  uint32 i;
  if(mcVid==userVid)
  {
  	printf("mcVid should not equal to userVid \r\n");
  	return OPL_ERROR;
  }

  for(i=0; i<MC_VLAN_TRANSLATION_ENTRY; i++) {
    if(mcTagTranslationTable[i].enable==OPL_ENABLE) {
      if((mcTagTranslationTable[i].portId == portId)
        && ((mcTagTranslationTable[i].mcVid == userVid) || (mcTagTranslationTable[i].userVid == userVid)))
      {
          printf("userVid which equal to existed mcVid or userVid in the translation table is not allowed \r\n");
	   return OPL_ERROR;
      }
      if((mcTagTranslationTable[i].portId == portId)
        &&(mcTagTranslationTable[i].userVid == mcVid)) {
          printf("mcVid which equal to existed userVid in the translation table is not allowed \r\n");
          return OPL_ERROR;
      }
    }
  }

  return OPL_OK;
}



/********************************************************************/

#define MC_CONTROL_TEST
#ifdef MC_CONTROL_TEST

/* 0x11 membership query */
unsigned char igmpGeneralQuerySingleMsg[72] =
{
  0x00, 0x00, 0x00, 0x01,
  0x01, 0x00, 0x5e, 0x00, 0x00, 0x01,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x81, 0x00, 0x0f, 0xab,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xe0, 0x00, 0x00, 0x01,
  0x94, 0x04, 0x00, 0x00,
  0x11, 0x00, 0xea, 0x03,
  0x00, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned char igmpGeneralQuerySingle[68] =
{
  0x01, 0x00, 0x5e, 0x00, 0x00, 0x01,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x81, 0x00, 0x0f, 0xab,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xe0, 0x00, 0x00, 0x01,
  0x94, 0x04, 0x00, 0x00,
  0x11, 0x00, 0xea, 0x03,
  0x00, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned char igmpGeneralQueryUntag[64] =
{
  0x01, 0x00, 0x5e, 0x00, 0x00, 0x01,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xe0, 0x00, 0x00, 0x01,
  0x94, 0x04, 0x00, 0x00,
  0x11, 0x00, 0xea, 0x03,
  0x00, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned char igmpSpecificQuerySingle[68] =
{
  0x01, 0x00, 0x5e, 0x01, 0x01, 0x01,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x81, 0x00, 0x0f, 0xab,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xef, 0x01, 0x01, 0x01,
  0x94, 0x04, 0x00, 0x00,
  0x11, 0x00, 0xea, 0x03,
  0xef, 0x01, 0x01, 0x01,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned char igmpSpecificQueryUntag[64] =
{
  0x01, 0x00, 0x5e, 0x01, 0x01, 0x01,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xef, 0x01, 0x01, 0x01,
  0x94, 0x04, 0x00, 0x00,
  0x11, 0x00, 0xea, 0x03,
  0xef, 0x01, 0x01, 0x01,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char igmpReportUntag1_hostIp1[64] =
{
  0x01, 0x00, 0x5e, 0x01, 0x01, 0x02,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x41,
  0xef, 0x01, 0x01, 0x02,
  0x94, 0x04, 0x00, 0x00,
  0x16, 0x00, 0xea, 0x03,
  0xef, 0x01, 0x01, 0x02,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned char igmpReportUntag2[64] =
{
  0x01, 0x00, 0x5e, 0x01, 0x01, 0x02,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xef, 0x01, 0x01, 0x02,
  0x94, 0x04, 0x00, 0x00,
  0x16, 0x00, 0xea, 0x03,
  0xef, 0x01, 0x01, 0x02,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* group 239.1.1.1 */
unsigned char igmpReportSingleErr[72] =
{
  0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x5e, 0x01, 0x01, 0x01,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x81, 0x00, 0xff, 0xff,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x40, 0x02, 0x43, 0x1f, 0xc0, 0xa8, 0x01, 0x12,
  0xe0, 0x01, 0x01, 0x01,
  0x94, 0x00, 0x00, 0x00,
  0x16, 0x64, 0x08, 0x99,
  0xe0, 0x01, 0x01, 0x01,
  0x02, 0x03, 0x04, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned char igmpReportUntagErr[68] =
{
  0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x5e, 0x01, 0x01, 0x01,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x40, 0x02, 0x43, 0x1f, 0xc0, 0xa8, 0x01, 0x12,
  0xe0, 0x01, 0x01, 0x01,
  0x94, 0x00, 0x00, 0x00,
  0x16, 0x64, 0x08, 0x99,
  0xe0, 0x01, 0x01, 0x01,
  0x02, 0x03, 0x04, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char igmpReportSingleMsg[72] =
{
  0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x5e, 0x01, 0x01, 0x01,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x81, 0x00, 0x00, 0x01,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xef, 0x01, 0x01, 0x01,
  0x94, 0x04, 0x00, 0x00,
  0x16, 0x00, 0xea, 0x03,
  0xef, 0x01, 0x01, 0x01,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned char igmpReportSingle[68] =
{
  0x01, 0x00, 0x5e, 0x01, 0x01, 0x01,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x81, 0x00, 0x00, 0x01,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xef, 0x01, 0x01, 0x01,
  0x94, 0x04, 0x00, 0x00,
  0x16, 0x00, 0xea, 0x03,
  0xef, 0x01, 0x01, 0x01,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char igmpReportSingle1[68] =
{
  0x01, 0x00, 0x5e, 0x01, 0x01, 0x02,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x81, 0x00, 0x00, 0x01,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xef, 0x01, 0x01, 0x02,
  0x94, 0x04, 0x00, 0x00,
  0x16, 0x00, 0xea, 0x03,
  0xef, 0x01, 0x01, 0x02,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00
};
unsigned char igmpReportSingle2[68] =
{
  0x01, 0x00, 0x5e, 0x01, 0x01, 0x03,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x81, 0x00, 0x00, 0x01,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xef, 0x01, 0x01, 0x03,
  0x94, 0x04, 0x00, 0x00,
  0x16, 0x00, 0xea, 0x03,
  0xef, 0x01, 0x01, 0x03,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00
};

unsigned char igmpLeaveUntag[64] =
{
  0x01, 0x00, 0x5e, 0x00, 0x00, 0x02,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xe0, 0x00, 0x00, 0x02,
  0x94, 0x04, 0x00, 0x00,
  0x17, 0x00, 0xea, 0x03,
  0xef, 0x01, 0x01, 0x02,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* 0x17, leave group */
unsigned char igmpLeaveSingleMsg[72] =
{
  0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x5e, 0x00, 0x00, 0x02,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x81, 0x00, 0x00, 0x01,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xe0, 0x00, 0x00, 0x02,
  0x94, 0x04, 0x00, 0x00,
  0x17, 0x00, 0xea, 0x03,
  0xef, 0x01, 0x01, 0x01,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned char igmpLeaveSingle[68] =
{
  0x01, 0x00, 0x5e, 0x00, 0x00, 0x02,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x81, 0x00, 0x00, 0x01,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xe0, 0x00, 0x00, 0x02,
  0x94, 0x04, 0x00, 0x00,
  0x17, 0x00, 0xea, 0x03,
  0xef, 0x01, 0x01, 0x01,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char igmpLeaveSingle1[68] =
{
  0x01, 0x00, 0x5e, 0x00, 0x00, 0x02,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x81, 0x00, 0x00, 0x01,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xe0, 0x00, 0x00, 0x02,
  0x94, 0x04, 0x00, 0x00,
  0x17, 0x00, 0xea, 0x03,
  0xef, 0x01, 0x01, 0x02,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00
};
unsigned char igmpLeaveSingle2[68] =
{
  0x01, 0x00, 0x5e, 0x00, 0x00, 0x02,
  0x00, 0x1e, 0xc9, 0x2e, 0x3f, 0x6d,
  0x81, 0x00, 0x00, 0x01,
  0x08, 0x00,
  0x46, 0x00, 0x00, 0x2e, 0xda, 0xce, 0x00, 0x00,
  0x01, 0x02, 0x67, 0x9a, 0xac, 0x10, 0x56, 0x40,
  0xe0, 0x00, 0x00, 0x02,
  0x94, 0x04, 0x00, 0x00,
  0x17, 0x00, 0xea, 0x03,
  0xef, 0x01, 0x01, 0x03,
  0x04, 0x00, 0x00, 0x00,
  0xef, 0xff, 0xff, 0xfa, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00
};

void igmpTest()
{

  mcModeSwitch(MC_MODE_SNOOPING);

  return;
}

int mc_main()
{

#if 0
  MC_GROUP_t *node;
  uint8 macStr[6] = {0,0,0,0,0,0x55};
  uint8 srcMacStraa[6] = {0,0,0,0,0,0xaa};
  uint8 srcMacStrbb[6] = {0,0,0,0,0,0xbb};

  node = NULL;

  mcVlanMapInit();

  node = mcGroupListGet(1, macStr, 0x11223344, 100, 0x7);

  if(!node)
    return -1;

  mcHostListInit(node);
  mcHostListAdd(node, srcMacStraa, 0x000000aa, 1, 0x60);
  mcHostListAdd(node, srcMacStrbb, 0x000000bb, 1, 0x60);
  mcHostListSetisLastReportRecevied(node, srcMacStraa, 0x000000aa, 0x60, 1);
  mcHostListSetisNonFastLeaveTimerStart(node, srcMacStrbb, 0x000000bb, 0x60, 1);
  mcGroupShow();
  mcHostListDel(node, srcMacStraa, 0x000000aa, 0x60);
  mcHostListDel(node, srcMacStrbb, 0x000000bb, 0x60);
  mcGroupShow();
#endif
  return 0;
}
#endif

/**
 * @}
 */

/* END FILE */


