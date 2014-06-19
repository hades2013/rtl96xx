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
* mc_control_dal.c
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
 * @defgroup xxx XXX
 * @{
 */

/***** INCLUDE FILES *****/
#include "hal.h"
#include "mc_control.h"

/***** LOCAL DEFINES and ENUM *****/

/***** LOCAL STRUCTURES and UNIONS *****/

/***** LOCAL TYPEDEFS ****/

/***** LOCAL (PRIVATE) PROTOTYPES *****/

/***** DATA ALLOCATION *****/

/***** PUBLIC FUNCTIONS ****/

/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS dalMcVlanMapAdd(uint32 portId, uint32 vlanId)
{
#ifdef ONU_1PORT

  int ret;
  UINT32 vttIndex;
  char vlanIdStr[SMALL_BUF_LEN];
  multicast_control_entry_t ctl_entry_list[MC_MAX_GROUP_NUM];
  UINT16 num = 0;
  /* 014547 */
  UINT16 m;
  /* */
  UINT32 groupAddress;
  UINT8  groupMac[ETH_MAC_ADDR_LEN];
  UINT32 userVid;
  
  ret = 0;
  vttIndex = 0;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, vlanId);

  vosSprintf(vlanIdStr, "igmp%d", vlanId);
  odmCreateVlanName(vlanIdStr);
 
#ifndef CTC_MULTICAST_SURPORT
  odmSetVlanNameTag(vlanIdStr, vlanId);
#if 0
  ret =  odmAddVlanPort(portId, vlanId, MC_TAG_OPER_MODE_TRANSPARENT);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
#endif

  /* add vlan check in cls */
  //TTBD//ret = odmMcMacVlanCheckAdd(j, ponPort, "01:00:5e");

  switch(mcTagOperPerPortGet(portId)) {
    case MC_TAG_OPER_MODE_TRANSPARENT:
      /* dal vtt entry */
      ret = dalVttMulticastEntryAdd(0, 0, vlanId, vlanId, portId);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      break;
    case MC_TAG_OPER_MODE_STRIP:
      ret = dalVttMulticastEntryAdd(0, 1, vlanId, vlanId, portId);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      break;
    case MC_TAG_OPER_MODE_TRANSLATION:
      /* add default translation entry for new mvlan if no entry configured */
      if(mcTagTranslationTableGet(portId, vlanId, &userVid)!=OPL_OK)
      {
        ret = dalVttMulticastEntryAdd(0, 0, vlanId, vlanId, portId);
        if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      }
      break;
    default:
      /* do nothing */
      break;
  }

  if((mcControlBlockModeGet()==MC_MODE_SNOOPING) 
    || ((mcControlBlockModeGet()==MC_MODE_CTC) 
      && (odmMulticastControlTypeGet()==MC_CTL_GDA_MAC)))
  {
    odmMulticastControlEntryGet(ctl_entry_list, &num);
    
    /* For all group entries */
    for (m = 0; m < num;)
    {
      /* if (vlanId != ctl_entry_list[m].vid) */
      {
        if (mcControlBlockModeGet()==MC_MODE_SNOOPING)
        {
          groupAddress = *(uint32 *)&(ctl_entry_list[m].mmac[2]);
          clMcMacGetByMcIp(groupMac, groupAddress);
        }
        else
        {
          vosMemCpy(groupMac, ctl_entry_list[m].mmac, MAC_ADDRESS_LENGTH);
        }
        
        ret = dalMulticastPortVlanMacAdd((UINT8)portId, (UINT16)vlanId, groupMac, OPL_FALSE);
        if(ret!=OPL_OK) 
        { 
          MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
        }
      }
      m++;
    }
  }
#else
{
  //extern int odmSetVlanNameTagCfg(char  *name, UINT16 vid);
  extern int32 DRV_AddPort2McastVlan(uint32 uiLPort, uint32 uiMcVid, int enTagMode);
  switch(mcTagOperPerPortGet(portId)) {
    case MC_TAG_OPER_MODE_TRANSPARENT:
      /* add multicast vlan entry */
      ret = DRV_AddPort2McastVlan(portId, vlanId, 0);
      if (0 != ret) 
      { 
        mc_printf("func %s failed to set port %d mcvtu %d trasp mode in switch chip.\r\n", __FUNCTION__,portId,vlanId); 
        return OPL_ERROR;
      }
      else
      { 
        mc_printf("func %s Succeeded to set port %d mcvtu %d trasp mode in switch chip.\r\n", __FUNCTION__,portId,vlanId); 
      }
      break;
    case MC_TAG_OPER_MODE_STRIP:
      /* add multicast vlan entry */
      ret = DRV_AddPort2McastVlan(portId, vlanId, 1);
      if (0 != ret) 
      { 
        mc_printf("func %s failed to set port %d mcvtu %d untagged mode in switch chip.\r\n", __FUNCTION__,portId,vlanId); 
        return OPL_ERROR;
      }
      else
      { 
        mc_printf("func %s Succeeded to set port %d mcvtu %d untagged mode in switch chip.\r\n", __FUNCTION__,portId,vlanId); 
      }
      break;
    case MC_TAG_OPER_MODE_TRANSLATION:
      /* do nothing */
      break;
    default:
      /* do nothing */
      break;
  }

  //odmSetVlanNameTagCfg(vlanIdStr, vlanId);
}
#endif
  
  return OPL_OK;

#endif

#if defined(ONU_4PORT_AR8228)

  int ret;
  UINT32 vttIndex;
  char vlanIdStr[SMALL_BUF_LEN];
  UINT8   macAddr[MAC_LENGTH];

  ret = 0;
  vttIndex = 0;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, vlanId);

#if 0
  vosSprintf(vlanIdStr, "igmp%d", vlanId);
  odmCreateVlanName(vlanIdStr);
  odmSetVlanNameTag(vlanIdStr, vlanId);
  ret =  odmAddVlanPort(portId, vlanId, MC_TAG_OPER_MODE_TRANSPARENT);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
#endif

  ret = dalMulticastVlanCreate(vlanId);
  if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }

  ret = dalMulticastVlanAddPort(portId, vlanId);

  if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }

  /* add vlan check in cls */
  //TTBD//ret = odmMcMacVlanCheckAdd(j, ponPort, "01:00:5e");

  switch(mcTagOperPerPortGet(portId)) {
    case MC_TAG_OPER_MODE_TRANSPARENT:
      /* dal vtt entry */
      ret = dalVttTagEntryAdd(0, 0, vlanId, vlanId, portId, &vttIndex, DAL_VTT_MC);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      break;
    case MC_TAG_OPER_MODE_STRIP:
      ret = dalVttTagEntryAdd(0, 0, 0, vlanId, portId, &vttIndex, DAL_VTT_MC);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      break;
    case MC_TAG_OPER_MODE_TRANSLATION:
      /* do nothing */
      break;
    default:
      /* do nothing */
      break;
  }

  /* Fuzzy matching filter add */
  macAddr[0] = 0x01;
  macAddr[1] = 0x00;
  macAddr[2] = 0x5e;
  macAddr[3] = 0x00;
  macAddr[4] = 0x00;
  macAddr[5] = 0x00;
  ret = dalMulticastPortVlanMacAdd((UINT8)portId, (UINT16)vlanId, macAddr, OPL_TRUE);
  if(ret != OPL_OK) 
  { 
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
  }

  return OPL_OK;

#endif

#if defined(ONU_4PORT_AR8327)
  int ret;
  UINT32 vttIndex;
  char vlanIdStr[SMALL_BUF_LEN];
  multicast_control_entry_t ctl_entry_list[MC_MAX_GROUP_NUM];
  UINT16 num = 0;
  UINT16 m;
  UINT32 groupAddress;
  UINT8  groupMac[ETH_MAC_ADDR_LEN];
  UINT32 userVid;
  UINT8   macAddr[MAC_LENGTH];

  ret = 0;
  vttIndex = 0;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, vlanId);

  ret = dalMulticastVlanCreate(vlanId);
  if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
  ret = dalMulticastVlanAddPort(portId, vlanId, 0);
  if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }

   ret = dalMulticastVlanAddPort(0, vlanId, 0);

  switch(mcTagOperPerPortGet(portId)) {
    case MC_TAG_OPER_MODE_TRANSPARENT:
      break;
    case MC_TAG_OPER_MODE_STRIP:
	  ret = dalMulticastVlanAddPort(portId, vlanId, 1);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      break;
    case MC_TAG_OPER_MODE_TRANSLATION:
      /* do nothing */
      break;
    default:
      /* do nothing */
      break;
  }

   if (1)
  {
    odmMulticastControlEntryGet(ctl_entry_list, &num);

    /* For all group entries */
    for (m = 0; m < num;m++)
    {
      if (portId == ctl_entry_list[m].port) 
      {
	  	
        if (odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID)
        {
          groupAddress = *(uint32 *)&(ctl_entry_list[m].mmac[2]);

		  clMcMacGetByMcIp(groupMac, groupAddress);
        }
        else if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC || odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID)
        {
          vosMemCpy(groupMac, ctl_entry_list[m].mmac, MAC_ADDRESS_LENGTH);
        }
		else {
			continue;
		}
        ret = dalMulticastPortVlanMacAdd((UINT8)portId, (UINT16)vlanId, groupMac, OPL_FALSE);
        if(ret!=OPL_OK) 
        { 
          MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
        }
      }
    }
  }

    /* Fuzzy matching filter add */
  macAddr[0] = 0x01;
  macAddr[1] = 0x00;
  macAddr[2] = 0x5e;
  macAddr[3] = 0x00;
  macAddr[4] = 0x00;
  macAddr[5] = 0x00;
  ret = dalMulticastPortVlanMacAdd((UINT8)portId, (UINT16)vlanId, macAddr, OPL_TRUE);
  if(ret != OPL_OK) 
  { 
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
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
OPL_STATUS dalMcVlanMapDel(uint32 portId, uint32 vlanId)
{
#ifdef ONU_1PORT

  int ret;
  UINT32 vttIndex;
  int isVlanPortExist;
  char vlanIdStr[SMALL_BUF_LEN];
  UINT32 i;
  multicast_control_entry_t ctl_entry_list[MC_MAX_GROUP_NUM];
  UINT16 num = 0;
  /* 014547 */
  UINT16 m;
  /*  */
  UINT32 groupAddress;
  UINT32 uiBitMap = 0;
  UINT8  groupMac[ETH_MAC_ADDR_LEN];
  
  ret = 0;
  vttIndex = 0;
  isVlanPortExist = 0;
  vosMemSet(vlanIdStr, 0, sizeof(vlanIdStr));

#if 0
  ret =  odmMoveVlanPort(portId, vlanId);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
#endif


  #ifndef CTC_MULTICAST_SURPORT
  /* check vlan */
  for(i = ODM_START_PORT_NUN; i < ODM_NUM_OF_PORTS+1; i++) {
    if(mcVlanMapExist(i, vlanId)==OPL_TRUE) {
      isVlanPortExist = OPL_TRUE;
    }
  }
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "xxx: isVlanPortExist=%d.\r\n",
    isVlanPortExist );

  if(isVlanPortExist==OPL_FALSE) {
    /* all ports is moved */
    ret = odmGetVlanIDName(vlanId, vlanIdStr);
    if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "xxx: vlanId=%d,vlanIdStr=%s.\r\n",
      vlanId, vlanIdStr);
    ret = odmDeleteVlanName(vlanIdStr);
    if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
  }
  #endif
  

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, vlanId);

  /* del vlan check in cls */
  //TTBD//ret = odmMcMacVlanCheckDel(j, ponPort, "01:00:5e");

  /* 014059 */
  /* delete mac first */
  if((mcControlBlockModeGet()==MC_MODE_SNOOPING) 
    || ((mcControlBlockModeGet()==MC_MODE_CTC) 
      && (odmMulticastControlTypeGet()==MC_CTL_GDA_MAC)))
  {
    odmMulticastControlEntryGet(ctl_entry_list, &num);

    /* For all group entries */
    for (m = 0; m < num;)
    {
      if (mcControlBlockModeGet()==MC_MODE_SNOOPING)
      {        
        groupAddress = *(uint32 *)&(ctl_entry_list[m].mmac[2]);
        clMcMacGetByMcIp(groupMac, groupAddress);
      }
      else
      {
        vosMemCpy(groupMac, ctl_entry_list[m].mmac, MAC_ADDRESS_LENGTH);
      }

      /* if (vlanId != ctl_entry_list[m].vid) */
      {
        ret = dalMulticastPortVlanMacDel((UINT8)portId, vlanId, groupMac, OPL_FALSE);
        if(ret!=OPL_OK) 
        { 
          MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
        }
      }

      /*  */
      /*delete soft-multicast entry*/
      uiBitMap = 0;
      if (mcControlBlockModeGet()==MC_MODE_SNOOPING)
      {
        uiBitMap = MC_BITMAP_HOST_PORT_ID | MC_BITMAP_GDA_IP | MC_BITMAP_VLAN_ID;
      }
      else
      {
        uiBitMap = MC_BITMAP_HOST_PORT_ID | MC_BITMAP_GDA_MAC | MC_BITMAP_VLAN_ID;
      }

      /*delete host list, if it is not emprt*/
      if (OPL_FALSE == mcGroupListHostListEmpty(portId, groupMac, \
                                                groupAddress, vlanId, 
                                                uiBitMap))
      {
        MC_GROUP_t *pgroup = NULL;
        pgroup = mcGroupListGet(portId, groupMac, groupAddress, vlanId, uiBitMap);
        if (NULL != pgroup)
        {
          (void)mcHostListDestory(pgroup);
        }
      }

      /*delete group list*/
      (void)mcGroupListDel(portId, groupMac, groupAddress, vlanId, uiBitMap);
      /* 014549 */
      m++;
    }
  }
  
  switch(mcTagOperPerPortGet(portId)) {
    case MC_TAG_OPER_MODE_TRANSPARENT:
    
      #ifndef CTC_MULTICAST_SURPORT
      /* dal vtt entry */
      ret = dalVttMulticastEntryDel(0, 0, vlanId, vlanId, portId);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      #else
      {
      extern INT32 DRV_DelPortFromMcastVlan(UINT32 uiLPort, UINT32 uiMcVid);

      ret = DRV_DelPortFromMcastVlan(portId, vlanId);
      if (NO_ERROR != ret)
      {
        MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
      }
      }
      #endif
      
      break;
    case MC_TAG_OPER_MODE_STRIP:
      
      #ifndef CTC_MULTICAST_SURPORT
      ret = dalVttMulticastEntryDel(0, 1, vlanId, vlanId, portId);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      #else
      {
      extern INT32 DRV_DelPortFromMcastVlan(UINT32 uiLPort, UINT32 uiMcVid);

      ret = DRV_DelPortFromMcastVlan(portId, vlanId);
      if (NO_ERROR != ret)
      {
        MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
      }
      }
      #endif
      
      break;
    case MC_TAG_OPER_MODE_TRANSLATION:
      /* do nothing */
      break;
    default:
      /* do nothing */
      break;
  }
  /* End  014059 */
  return OPL_OK;

#endif

#if defined(ONU_4PORT_AR8228)

  int ret;
  UINT32 vttIndex;
  int isVlanPortExist;
  char vlanIdStr[SMALL_BUF_LEN];
  UINT32 i;
  UINT8 macAddr[MAC_LENGTH];

  ret = 0;
  vttIndex = 0;
  isVlanPortExist = 0;
  vosMemSet(vlanIdStr, 0, sizeof(vlanIdStr));

#if 0
  ret =  odmMoveVlanPort(portId, vlanId);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  /* check vlan */
  for(i = ODM_START_PORT_NUN; i < ODM_NUM_OF_PORTS+1; i++) {
    if(mcVlanMapExist(i, vlanId)==OPL_TRUE) {
      isVlanPortExist = OPL_TRUE;
    }
  }
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "xxx: isVlanPortExist=%d.\r\n",
    isVlanPortExist );

  if(isVlanPortExist==OPL_FALSE) {
    /* all ports is moved */
    ret = odmGetVlanIDName(vlanId, vlanIdStr);
    if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "xxx: vlanId=%d,vlanIdStr=%s.\r\n",
      vlanId, vlanIdStr);
    ret = odmDeleteVlanName(vlanIdStr);
    if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
  }

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, vlanId);
#endif

  switch(mcTagOperPerPortGet(portId)) {
    case MC_TAG_OPER_MODE_TRANSPARENT:
      /* dal vtt entry */
      ret = dalVttTagEntryDel(0, 0, vlanId, vlanId, portId, &vttIndex, DAL_VTT_MC);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      break;
    case MC_TAG_OPER_MODE_STRIP:
      ret = dalVttTagEntryDel(0, 0, 0, vlanId, portId, &vttIndex, DAL_VTT_MC);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      break;
    case MC_TAG_OPER_MODE_TRANSLATION:
      /* do nothing */
      break;
    default:
      /* do nothing */
      break;
  }

  ret = dalMulticastVlanDelPort(portId, vlanId);
  if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }

  /* check vlan */
  for(i = ODM_START_PORT_NUN; i < ODM_NUM_OF_PORTS+1; i++) {
    if(mcVlanMapExist(i, vlanId)==OPL_TRUE) {
      isVlanPortExist = OPL_TRUE;
    }
  }
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "xxx: isVlanPortExist=%d.\r\n",
    isVlanPortExist );

  if(isVlanPortExist==OPL_FALSE) {
    /* all ports is moved */
    ret = dalMulticastVlanDelete(vlanId);
    if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }

    /* Fuzzy matching filter del */
    macAddr[0] = 0x01;
    macAddr[1] = 0x00;
    macAddr[2] = 0x5e;
    macAddr[3] = 0x00;
    macAddr[4] = 0x00;
    macAddr[5] = 0x00;
    ret = dalMulticastPortVlanMacDel((UINT8)portId, (UINT16)vlanId, macAddr, OPL_TRUE);
    if(ret != OPL_OK) 
    { 
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
    }
  }

  return OPL_OK;

#endif

#if defined(ONU_4PORT_AR8327)
   int ret;
  UINT32 vttIndex;
  int isVlanPortExist;
  char vlanIdStr[SMALL_BUF_LEN];
  UINT32 i;
  UINT8 macAddr[MAC_LENGTH];

  ret = 0;
  vttIndex = 0;
  isVlanPortExist = 0;
  vosMemSet(vlanIdStr, 0, sizeof(vlanIdStr));

  switch(mcTagOperPerPortGet(portId)) {
    case MC_TAG_OPER_MODE_TRANSPARENT:
      //ret = dalMulticastVlanDelPort(portId, vlanId);
      //if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      break;
    case MC_TAG_OPER_MODE_STRIP:
      //ret = dalMulticastVlanDelPort(portId, vlanId);
      //if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      break;
    case MC_TAG_OPER_MODE_TRANSLATION:
      /* do nothing */
      break;
    default:
      /* do nothing */
      break;
  }

  ret = dalMulticastVlanDelPort(portId, vlanId);
  if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }

  /* check vlan */
  for(i = ODM_START_PORT_NUN; i < ODM_NUM_OF_PORTS+1; i++) {
    if(mcVlanMapExist(i, vlanId)==OPL_TRUE) {
      isVlanPortExist = OPL_TRUE;
    }
  }
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "xxx: isVlanPortExist=%d.\r\n",
    isVlanPortExist );

  if(isVlanPortExist==OPL_FALSE) {
    /* all ports is moved */
    ret = dalMulticastVlanDelete(vlanId);
    if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }

    /* Fuzzy matching filter del */
    macAddr[0] = 0x01;
    macAddr[1] = 0x00;
    macAddr[2] = 0x5e;
    macAddr[3] = 0x00;
    macAddr[4] = 0x00;
    macAddr[5] = 0x00;
    ret = dalMulticastPortVlanMacDel((UINT8)portId, (UINT16)vlanId, macAddr, OPL_TRUE);
    if(ret != OPL_OK) 
    { 
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
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
OPL_STATUS dalMcTagTranslationAdd(uint32 portId, uint32 mcVid, uint32 userVid)
{
#ifdef ONU_1PORT

  int ret;
  UINT32 vttIndex;

  ret = 0;
  vttIndex = 0;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, mcVid=%d, userVid=%d.\r\n", __FUNCTION__, __LINE__, portId, mcVid, userVid);

  /* delete default entry if exist */
  ret = dalVttMulticastEntryDel(0, 0, mcVid, mcVid, portId);
  if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }

  /* dal vtt entry */
  ret = dalVttMulticastEntryAdd(0, 0, userVid, mcVid, portId);
  if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }

  return OPL_OK;

#endif

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)

  int ret;
  UINT32 vttIndex;

  ret = 0;
  vttIndex = 0;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, mcVid=%d, userVid=%d.\r\n", __FUNCTION__, __LINE__, portId, mcVid, userVid);

  /* dal vtt entry */
  ret = dalVttTagEntryAdd(0, 0, userVid, mcVid, portId, &vttIndex, DAL_VTT_MC);
  if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }

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
OPL_STATUS dalMcTagTranslationDel(uint32 portId, uint32 mcVid, uint32 userVid)
{
#ifdef ONU_1PORT

    int ret;
    UINT32 vttIndex;

    ret = 0;
    vttIndex = 0;

    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, mcVid=%d, userVid=%d.\r\n", __FUNCTION__, __LINE__, portId, mcVid, userVid);

    /* dal vtt entry */
    ret = dalVttMulticastEntryDel(0, 0, userVid, mcVid, portId);
    if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }

    return OPL_OK;

#endif

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)

    int ret;
    UINT32 vttIndex;

    ret = 0;
    vttIndex = 0;

    MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, mcVid=%d, userVid=%d.\r\n", __FUNCTION__, __LINE__, portId, mcVid, userVid);

    /* dal vtt entry */
    ret = dalVttTagEntryDel(0, 0, userVid, mcVid, portId, &vttIndex, DAL_VTT_MC);
    if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }

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
OPL_STATUS dalMcVlanInTransparentModeSetAction(uint32 portId)
{
#ifdef ONU_1PORT

  int ret;
  UINT32 j;
  UINT32 vttIndex;

  ret = 0;
  vttIndex = 0;

  for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
    if(mcVlanMapExist(portId, j)) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, j);

      /* add vlan check in cls */
      //TTBD//ret = odmMcMacVlanCheckAdd(j, ponPort, "01:00:5e");

     
      #ifndef CTC_MULTICAST_SURPORT
      /* dal vtt entry */
      ret = dalVttMulticastEntryAdd(0, 0, j, j, portId);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      #else
      {
        extern int32 DRV_AddPort2McastVlan(uint32 uiLPort, uint32 uiMcVid, int enTagMode);
        ret = DRV_AddPort2McastVlan(portId,j,0);
        if (0 != ret)
        {
            mc_printf("\nfunc %s:Failed to set port %d mcvlan %d\n",__FUNCTION__,portId,j);
            return OPL_ERROR;
        }
        else
        {
            mc_printf("\nfunc %s:Succeeded to set port %d mcvlan %d\n",__FUNCTION__,portId,j);
        }
      }
      #endif
     
    }
  }

  return OPL_OK;

#endif

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)

  int ret;
  UINT32 j;
  UINT32 vttIndex;

  ret = 0;
  vttIndex = 0;

  for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
    if(mcVlanMapExist(portId, j)) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, j);

      /* add vlan check in cls */
      //TTBD//ret = odmMcMacVlanCheckAdd(j, ponPort, "01:00:5e");

      /* dal vtt entry */
      #if defined(ONU_4PORT_AR8327)
      ret = dalMulticastVlanAddPort(portId, j, 0);
      #else
      ret = dalVttTagEntryAdd(0, 0, j, j, portId, &vttIndex, DAL_VTT_MC);
      #endif
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
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
OPL_STATUS dalMcVlanInTransparentModeClear(uint32 portId)
{
#ifdef ONU_1PORT

  int ret;
  UINT32 j;
  UINT32 vttIndex;

  ret = 0;
  vttIndex = 0;

  for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
    if(mcVlanMapExist(portId, j)) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, j);
      /* del vlan check in cls */
      //TTBD//ret = odmMcMacVlanCheckDel(j, ponPort, "01:00:5e");

    
      #ifndef CTC_MULTICAST_SURPORT
      /* dal vtt entry */
      ret = dalVttMulticastEntryDel(0, 0, j, j, portId);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      #else
      {
      extern int32 DRV_DelPortFromMcastVlan(uint32 uiLPort, uint32 uiMcVid);
      ret = DRV_DelPortFromMcastVlan(portId, j);
      if (0 != ret)
      {
        mc_printf("\nfunc %s:Failed to del port %d mcvlan %d.\n",__FUNCTION__,portId,j);
        return OPL_ERROR;
      }
      else
      {
        mc_printf("\nfunc %s:Succeeded to del port %d mcvlan %d.\n",__FUNCTION__,portId,j);
      }
      }
      #endif
      

    }
  }

  return OPL_OK;

#endif

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)

  int ret;
  UINT32 j;
  UINT32 vttIndex;

  ret = 0;
  vttIndex = 0;

  for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
    if(mcVlanMapExist(portId, j)) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, j);
      /* del vlan check in cls */
      //TTBD//ret = odmMcMacVlanCheckDel(j, ponPort, "01:00:5e");

      /* dal vtt entry */
      #if defined(ONU_4PORT_AR8327)
      ret = dalMulticastVlanDelPort(portId, j);
      #else
      ret = dalVttTagEntryDel(0, 0, j, j, portId, &vttIndex, DAL_VTT_MC);
      #endif
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }

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
OPL_STATUS dalMcVlanInStripModeSetAction(uint32 portId)
{
#ifdef ONU_1PORT

  int ret;
  UINT32 j;
  UINT32 vttIndex;

  ret = 0;
  vttIndex = 0;

  for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
    if(mcVlanMapExist(portId, j)) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, j);

      /* add vlan check in cls */
      //TTBD//ret = odmMcMacVlanCheckAdd(j, ponPort, "01:00:5e");

      
      #ifndef CTC_MULTICAST_SURPORT
      /* dal vtt entry */
      ret = dalVttMulticastEntryAdd(0, 1, j, j, portId);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      #else
      {
        extern int32 DRV_AddPort2McastVlan(uint32 uiLPort, uint32 uiMcVid, int enTagMode);
        ret = DRV_AddPort2McastVlan(portId,j,1);
        if (0 != ret)
        {
            mc_printf("\nfunc %s:Failed to set port %d mcvlan %d\n",__FUNCTION__,portId,j);
            return OPL_ERROR;
        }
        else
        {
            mc_printf("\nfunc %s:Succeeded to set port %d mcvlan %d\n",__FUNCTION__,portId,j);
        }
      }
      #endif
      
    }
  }

  return OPL_OK;

#endif

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)

  int ret;
  UINT32 j;
  UINT32 vttIndex;

  ret = 0;
  vttIndex = 0;

  for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
    if(mcVlanMapExist(portId, j)) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, j);

      /* add vlan check in cls */
      //TTBD//ret = odmMcMacVlanCheckAdd(j, ponPort, "01:00:5e");

      /* dal vtt entry */
      #if defined(ONU_4PORT_AR8327)
      ret = dalMulticastVlanAddPort(portId, j, 1);
      #else
      ret = dalVttTagEntryAdd(0, 0, 0, j, portId, &vttIndex, DAL_VTT_MC);
      #endif
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
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
OPL_STATUS dalMcVlanInStripModeClear(uint32 portId)
{
#ifdef ONU_1PORT

  int ret;
  UINT32 j;
  UINT32 vttIndex;

  ret = 0;
  vttIndex = 0;

  for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
    if(mcVlanMapExist(portId, j)) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, j);
      /* dal vlan check in cls */
      //TTBD//ret = odmMcMacVlanCheckDel(j, ponPort, "01:00:5e");

      
      #ifndef CTC_MULTICAST_SURPORT
      /* dal vtt entry */
      ret = dalVttMulticastEntryDel(0, 1, j, j, portId);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X, %d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      #else
      {
      extern int32 DRV_DelPortFromMcastVlan(uint32 uiLPort, uint32 uiMcVid);
      ret = DRV_DelPortFromMcastVlan(portId, j);
      if (0 != ret)
      {
        mc_printf("\nfunc %s:Failed to del port %d mcvlan %d.\n",__FUNCTION__,portId,j);
        return OPL_ERROR;
      }
      else
      {
        mc_printf("\nfunc %s:Succeeded to del port %d mcvlan %d.\n",__FUNCTION__,portId,j);
      }
      }
      #endif
     

    }
  }

  return OPL_OK;

#endif

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)

  int ret;
  UINT32 j;
  UINT32 vttIndex;

  ret = 0;
  vttIndex = 0;

  for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
    if(mcVlanMapExist(portId, j)) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, j);
      /* dal vlan check in cls */
      //TTBD//ret = odmMcMacVlanCheckDel(j, ponPort, "01:00:5e");

      /* dal vtt entry */
      #if defined(ONU_4PORT_AR8327)
      ret = dalMulticastVlanDelPort(portId, j);
      #else
      ret = dalVttTagEntryDel(0, 0, 0, j, portId, &vttIndex, DAL_VTT_MC);
      #endif
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X, %d.\r\n", __FUNCTION__, __LINE__, ret, ret); }

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
OPL_STATUS dalMcVlanInTranslationModeSetAction(uint32 portId)
{
#ifdef ONU_1PORT
  int ret;
  UINT32 j;
  UINT32 vttIndex;

  ret = 0;
  vttIndex = 0;

  for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
    if(mcVlanMapExist(portId, j)) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, j);

      /* add vlan check in cls */
      //TTBD//ret = odmMcMacVlanCheckAdd(j, ponPort, "01:00:5e");

      /* add default translation entry for configured mvlan */
      ret = dalVttMulticastEntryAdd(0, 0, j, j, portId);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
    }
  }

  return OPL_OK;
#endif

#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
  int ret;
  UINT32 j;
  UINT32 vttIndex;

  ret = 0;
  vttIndex = 0;

  for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
    if(mcVlanMapExist(portId, j)) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, j);

      /* add vlan check in cls */
      //TTBD//ret = odmMcMacVlanCheckAdd(j, ponPort, "01:00:5e");
      #if defined(ONU_4PORT_AR8327)
      printf("multicast vlan=%d\n", j);
      ret = dalMulticastVlanAddPort(portId, j, 0);
      #endif
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
OPL_STATUS dalMcVlanInTranslationModeClear(uint32 portId)
{
#if defined(ONU_1PORT)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)

  int ret;
  UINT32 j;
  UINT32 vttIndex;

  ret = 0;
  vttIndex = 0;

  for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
    if(mcVlanMapExist(portId, j)) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, j);
      /* dal vlan check in cls */
      //TTBD//ret = odmMcMacVlanCheckDel(j, ponPort, "01:00:5e");

#if defined(ONU_1PORT)
      /* delete default translation entry for configured mvlan */
      ret = dalVttMulticastEntryDel(0, 0, j, j, portId);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
#endif
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
OPL_STATUS dalMcVlanInXxxModeSetAction(uint32 portId, MC_TAG_OPER_MODE_t tagOper)
{
#if defined(ONU_1PORT)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)

  int ret;

  ret = 0;

  /* dal */
  switch(tagOper) {
    case MC_TAG_OPER_MODE_TRANSPARENT:
      ret = dalMcVlanInTransparentModeSetAction(portId);
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      break;
    case MC_TAG_OPER_MODE_STRIP:
      ret = dalMcVlanInStripModeSetAction(portId);
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      break;
    case MC_TAG_OPER_MODE_TRANSLATION:
     
      #ifndef CTC_MULTICAST_SURPORT
      /* clear vtt in transparent or strip mode */
      ret = dalMcVlanInTranslationModeSetAction(portId);
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      #else
      mc_printf("\nfunc %s:Do not support mcvlan translation.\n",__FUNCTION__);
      return OPL_ERROR;
      #endif
      
      /* do nothing */
      break;
    default:
      /* do nothing */
      break;
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
OPL_STATUS dalMcVlanInXxxModeClear(uint32 portId, MC_TAG_OPER_MODE_t tagOper)
{
#if defined(ONU_1PORT)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)

  int ret;

  ret = 0;

  /* dal */
  switch(tagOper) {
    case MC_TAG_OPER_MODE_TRANSPARENT:
      ret = dalMcVlanInTransparentModeClear(portId);
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      break;
    case MC_TAG_OPER_MODE_STRIP:
      ret = dalMcVlanInStripModeClear(portId);
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      break;
    case MC_TAG_OPER_MODE_TRANSLATION:
      
      #ifndef CTC_MULTICAST_SURPORT
      /* clear vtt in transparent or strip mode */
      ret = dalMcVlanInTranslationModeClear(portId);
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      #else
      mc_printf("\nfunc %s:Do not support mcvlan translation.\n",__FUNCTION__);
      return OPL_ERROR;
      #endif
      
      /* do nothing */
      break;
    default:
      /* do nothing */
      break;
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
OPL_STATUS dalMcTagOperSet(uint32 portId, MC_TAG_OPER_MODE_t tagOper)
{
  int ret;

  /* clear old mode info */
  ret = dalMcVlanInXxxModeClear(portId, mcTagOperPerPortGet(portId));

  ret = dalMcVlanInXxxModeSetAction(portId, tagOper);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

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
OPL_STATUS dalMcGroupAdd(uint32 hostPortId, uint8 *groupMac, uint32 groupAddress, uint32 mcVlan, uint32 bitmap)
{
  uint8 tempGroupMac[MAC_ADDRESS_LENGTH];
  OPL_STATUS ret;
  UINT16 vlanId[OP_MAX_VLAN_NUMBER];
  UINT8 num= 0;
  UINT8 m;
  MC_GROUP_t *group;
  UINT32 state;
  
  vosMemSet(tempGroupMac, 0, MAC_ADDRESS_LENGTH);

  /************************ group add ************************/
#ifdef ONU_1PORT
  /* Snooping mode */
  if(mcControlBlockModeGet()==MC_MODE_SNOOPING)
  {
    clMcMacGetByMcIp(tempGroupMac, groupAddress);
    
#if 0
    ret = dalMulticastPortVlanMacAdd((UINT8)hostPortId, mcVlan, tempGroupMac, OPL_FALSE);
    if(ret!=OPL_OK) 
    { 
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
    }
#endif
    
    odmMulticastVlanGet(hostPortId, vlanId, &num);
    
    /* For all mc vlan */
    for (m = 0; m < num;)
    {
      /* 014041 */
      if (0 == mcVlan )
      {
          ret = dalMulticastPortVlanMacAdd((UINT8)hostPortId, vlanId[m], tempGroupMac, OPL_FALSE);
          if(ret!=OPL_OK) 
          { 
              MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
          }
      } 
      else if (vlanId[m] == mcVlan) 
      {
        ret = dalMulticastPortVlanMacAdd((UINT8)hostPortId, vlanId[m], tempGroupMac, OPL_FALSE);
        if(ret!=OPL_OK) 
        { 
          MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
        }
      }
      m++;
    }
  }
  else if(mcControlBlockModeGet()==MC_MODE_CTC) /* CTC mode */
  {
    if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC)
    {
      /* For all mc vlan */
      ret = dalMulticastPortVlanMacAdd((UINT8)hostPortId, mcVlan, groupMac, OPL_FALSE);
      if(ret!=OPL_OK) 
      { 
        MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
      }
          
      odmMulticastVlanGet(hostPortId, vlanId, &num);
        
      for (m = 0; m < num;)
      {
        if (mcVlan != vlanId[m])
        {
          ret = dalMulticastPortVlanMacAdd((UINT8)hostPortId, vlanId[m], groupMac, OPL_FALSE);
          if(ret!=OPL_OK) 
          { 
            MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
          }
        }
        m++;
      }
    }
    
    if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID)
    {
      /* Exact matching filter, GDA MAC + VID */
      vosMemCpy(tempGroupMac, groupMac, MAC_ADDRESS_LENGTH);
      dalMulticastPortVlanMacAdd((UINT8)hostPortId, (UINT16)mcVlan, tempGroupMac, OPL_FALSE);
    }
    
   
    #ifndef CTC_MULTICAST_SURPORT
    if(odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID)
    {
      /* Exact matching filter, GDA IP + VID */
      dalMulticastPortVlanIpAdd((UINT8)hostPortId, (UINT16)mcVlan, groupAddress);
    }
    #endif
    
  }
#elif defined(ONU_4PORT_AR8327)
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC
    || odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID) {
    /************************ MC_CTL_GDA_MAC or MC_CTL_GDA_MAC_VID ************************/
    vosMemCpy(tempGroupMac, groupMac, MAC_ADDRESS_LENGTH);
  }
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID) {
    /************************ MC_CTL_GDA_GDA_IP_VID ************************/
    clMcMacGetByMcIp(tempGroupMac, groupAddress);
  }

    odmMulticastVlanGet(hostPortId, vlanId, &num);

    /* For all mc vlan */
    for (m = 0; m < num;)
    {
         MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "dalArlMultiPortMacAdd: hostPortId=%d, mcVlan=%d, tempGroupMac=%02x%02x%02x%02x%02x%02x.\r\n", 
         hostPortId, vlanId[m], tempGroupMac[0], tempGroupMac[1], tempGroupMac[2], 
         tempGroupMac[3], tempGroupMac[4], tempGroupMac[5]);
		 
	  ret = dalArlMultiPortMacAdd(1, &hostPortId, tempGroupMac, vlanId[m]);
	  if (ret  != OPL_OK)
	  {
	      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
	  }
	   m++;
    }
#else
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC
    || odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID) {
    /************************ MC_CTL_GDA_MAC or MC_CTL_GDA_MAC_VID ************************/
    vosMemCpy(tempGroupMac, groupMac, MAC_ADDRESS_LENGTH);
  }
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID) {
    /************************ MC_CTL_GDA_GDA_IP_VID ************************/
    clMcMacGetByMcIp(tempGroupMac, groupAddress);
  }
  
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "dalArlMultiPortMacAdd: hostPortId=%d, mcVlan=%d, tempGroupMac=%02x%02x%02x%02x%02x%02x.\r\n", 
    hostPortId, mcVlan, tempGroupMac[0], tempGroupMac[1], tempGroupMac[2], 
    tempGroupMac[3], tempGroupMac[4], tempGroupMac[5]);

  ret = dalArlMultiPortMacAdd(1, &hostPortId, tempGroupMac, mcVlan);
  if (ret  != OPL_OK)
  {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
  }
  
#if 0
  if (odmMulticastControlTypeGet()==MC_CTL_GDA_MAC)
  {
    ret = dalMulticastPortMacAdd((UINT8)hostPortId, tempGroupMac);
    {
        MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
    }
  }
#endif
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
OPL_STATUS dalMcGroupDel(uint32 hostPortId, uint8 *groupMac, uint32 groupAddress, uint32 mcVlan, uint32 bitmap)
{
  uint8 tempGroupMac[MAC_ADDRESS_LENGTH];
  OPL_STATUS ret;
  UINT16 vlanId[OP_MAX_VLAN_NUMBER];
  UINT8 num= 0;
  UINT8 m;
  
  vosMemSet(tempGroupMac, 0, MAC_ADDRESS_LENGTH);

  /************************ group del ************************/
#ifdef ONU_1PORT
  /* For all mc vlan */
  if(mcControlBlockModeGet()==MC_MODE_SNOOPING)
  {
      clMcMacGetByMcIp(tempGroupMac, groupAddress);
    
#if 0
      ret = dalMulticastPortVlanMacDel((UINT8)hostPortId, mcVlan, tempGroupMac, OPL_FALSE);
      if(ret!=OPL_OK) 
      { 
        MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
      }
#endif
      
      odmMulticastVlanGet(hostPortId, vlanId, &num);
      
      /* For all mc vlan */
      for (m = 0; m < num;)
      {
        /* 014041 */
        if (0 == mcVlan )
        {
            ret = dalMulticastPortVlanMacDel((UINT8)hostPortId, vlanId[m], tempGroupMac, OPL_FALSE);

            if(ret!=OPL_OK) 
            { 
                MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
            }
        } 
        else if (mcVlan == vlanId[m]) 
       {
        /* end */
          ret = dalMulticastPortVlanMacDel((UINT8)hostPortId, vlanId[m], tempGroupMac, OPL_FALSE);
          if(ret!=OPL_OK) 
          { 
            MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
          }
        }
        m++;
      }
  }
  else if(mcControlBlockModeGet()==MC_MODE_CTC)
  {
    if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC)
    {
      /* For all mc vlan */
      ret = dalMulticastPortVlanMacDel((UINT8)hostPortId, mcVlan, groupMac, OPL_FALSE);
      if(ret!=OPL_OK) 
      { 
        MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
      }
      
      odmMulticastVlanGet(hostPortId, vlanId, &num);
        
      for (m = 0; m < num;)
      {
        if (mcVlan != vlanId[m])
        {
          ret = dalMulticastPortVlanMacDel((UINT8)hostPortId, vlanId[m], groupMac, OPL_FALSE);
          if(ret!=OPL_OK) 
          { 
            MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
          }
        }
        m++;
      }
    }
    
    if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID)
    {
      /* Exact matching filter, GDA MAC + VID */
      vosMemCpy(tempGroupMac, groupMac, MAC_ADDRESS_LENGTH);
      dalMulticastPortVlanMacDel((UINT8)hostPortId, (UINT16)mcVlan, tempGroupMac, OPL_FALSE);
    }
    
   
    #ifndef CTC_MULTICAST_SURPORT
    if(odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID)
    {
      /* Exact matching filter, GDA IP + VID */
      dalMulticastPortVlanIpDel((UINT8)hostPortId, (UINT16)mcVlan, groupAddress);
    }
    #endif
    
  }
#elif defined(ONU_4PORT_AR8327)
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC
    || odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID) {
    /************************ MC_CTL_GDA_MAC or MC_CTL_GDA_MAC_VID ************************/
    vosMemCpy(tempGroupMac, groupMac, MAC_ADDRESS_LENGTH);
  }
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID) {
    /************************ MC_CTL_GDA_GDA_IP_VID ************************/
    clMcMacGetByMcIp(tempGroupMac, groupAddress);
  }
  
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "dalArlMultiPortMacDel: hostPortId=%d, mcVlan=%d, tempGroupMac=%02x%02x%02x%02x%02x%02x.\r\n", 
    hostPortId, mcVlan, tempGroupMac[0], tempGroupMac[1], tempGroupMac[2], 
    tempGroupMac[3], tempGroupMac[4], tempGroupMac[5]);

    odmMulticastVlanGet(hostPortId, vlanId, &num);

    /* For all mc vlan */
    for (m = 0; m < num;)
    {
	  ret = dalArlMultiPortMacDel(1, &hostPortId, tempGroupMac, (UINT16)vlanId[m]);
	  if (ret  != OPL_OK)
	  {
	      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
	  }
      m++;
    }
#else
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_MAC
    || odmMulticastControlTypeGet()==MC_CTL_GDA_MAC_VID) {
    /************************ MC_CTL_GDA_MAC or MC_CTL_GDA_MAC_VID ************************/
    vosMemCpy(tempGroupMac, groupMac, MAC_ADDRESS_LENGTH);
  }
  if(odmMulticastControlTypeGet()==MC_CTL_GDA_GDA_IP_VID) {
    /************************ MC_CTL_GDA_GDA_IP_VID ************************/
    clMcMacGetByMcIp(tempGroupMac, groupAddress);
  }
  
  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "dalArlMultiPortMacDel: hostPortId=%d, mcVlan=%d, tempGroupMac=%02x%02x%02x%02x%02x%02x.\r\n", 
    hostPortId, mcVlan, tempGroupMac[0], tempGroupMac[1], tempGroupMac[2], 
    tempGroupMac[3], tempGroupMac[4], tempGroupMac[5]);

  ret = dalArlMultiPortMacDel(1, &hostPortId, tempGroupMac, (UINT16)mcVlan);
  if (ret  != OPL_OK)
  {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
  }
  
#if 0
  if (odmMulticastControlTypeGet()==MC_CTL_GDA_MAC)
  {
    ret = dalMulticastPortMacDel((UINT8)hostPortId, tempGroupMac);
    {
        MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
    }
  }
#endif
#endif

  return OPL_OK;
}
#if 0
/**
* @par Description
* This function xxx.
* @param
*
* @return OPL_OK - on success
* @return OPL_ERROR - on error
* @see
*/
OPL_STATUS dalMcTagTranslationModeSet(uint32 portId)
{
  int ret;

  ret = 0;

#ifdef ONU_1PORT
  odmPortVlanModeSet(portId, ODM_VLAN_TRANSLATION);
#endif

#ifdef ONU_4PORT_AR8228
    /* using acl to do vtt */
    ret = odmPortMultcastTagStripEnableSet(portId, MC_TAG_OPER_MODE_TRANSPARENT);
    RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
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
OPL_STATUS dalMcTagTranslationAddInVlanTransparentMode(uint32 portId, uint32 mcVid, uint32 userVid)
{
  int ret;

  ret = 0;

#ifdef ONU_1PORT
#endif

#ifdef ONU_4PORT_AR8228
    ret = dalAclCtcVTAdd(mcVid, userVid, 0);
    if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
    //RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
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
OPL_STATUS dalMcTagTranslationDelInVlanTransparentMode(uint32 portId, uint32 mcVid, uint32 userVid)
{
  int ret;

  ret = 0;

#ifdef ONU_1PORT
#endif

#ifdef ONU_4PORT_AR8228
    ret = dalAclCtcVTDel(mcVid, userVid, 0);
    if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
    //RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
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
OPL_STATUS dalMcTagTranslationAddInVlanTranslationMode(uint32 portId, uint32 mcVid, uint32 userVid)
{
  int ret;

  ret = 0;

#ifdef ONU_1PORT
#endif

#ifdef ONU_4PORT_AR8228
    ret = odmPortVttEntryAdd(portId, 0, userVid, 0, mcVid);
    if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
    //RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
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
OPL_STATUS dalMcTagTranslationDelInVlanTranslationMode(uint32 portId, uint32 mcVid, uint32 userVid)
{
  int ret;

  ret = 0;

#ifdef ONU_1PORT
#endif

#ifdef ONU_4PORT_AR8228
    ret = odmPortVttEntryDel(portId, 0, userVid, 0, mcVid);
    if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
    //RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
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
OPL_STATUS dalMcTagTranslationAddAll(uint32 portId, uint32 vlanMode)
{
  int ret;
  uint32 i;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "dalMcTagTranslationAddAll: portId=%d, vlanMode=%d.\r\n",
    portId, vlanMode);

  for(i=0; i<MC_VLAN_TRANSLATION_ENTRY; i++) {
    if(mcTagTranslationTable[i].enable==OPL_ENABLE) {
      if(mcTagTranslationTable[i].portId == portId) { /* match */
        MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "dalMcTagTranslationAddAll: portId=%d, mcVid=%d, userVid=%d.\r\n",
          portId, mcTagTranslationTable[i].mcVid, mcTagTranslationTable[i].userVid);
        if(vlanMode==ODM_VLAN_TRANSPARENT) {
          ret = dalMcTagTranslationAddInVlanTransparentMode(portId, mcTagTranslationTable[i].mcVid, mcTagTranslationTable[i].userVid);
          //RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
        }
        else {
          ret = dalMcTagTranslationAddInVlanTranslationMode(portId, mcTagTranslationTable[i].mcVid, mcTagTranslationTable[i].userVid);
          //RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
        }
      }
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
OPL_STATUS dalMcTagTranslationDelAll(uint32 portId, uint32 vlanMode)
{
  int ret;
  uint32 i;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "dalMcTagTranslationDelAll: portId=%d, vlanMode=%d.\r\n",
    portId, vlanMode);

  for(i=0; i<MC_VLAN_TRANSLATION_ENTRY; i++) {
    if(mcTagTranslationTable[i].enable==OPL_ENABLE) {
      if(mcTagTranslationTable[i].portId == portId) { /* match */
        MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "dalMcTagTranslationDelAll: portId=%d, mcVid=%d, userVid=%d.\r\n",
          portId, mcTagTranslationTable[i].mcVid, mcTagTranslationTable[i].userVid);
        if(vlanMode==ODM_VLAN_TRANSPARENT) {
          ret = dalMcTagTranslationDelInVlanTransparentMode(portId, mcTagTranslationTable[i].mcVid, mcTagTranslationTable[i].userVid);
          //RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
        }
        else {
          ret = dalMcTagTranslationDelInVlanTranslationMode(portId, mcTagTranslationTable[i].mcVid, mcTagTranslationTable[i].userVid);
          //RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
        }
      }
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
OPL_STATUS dalMcTagNotTranslationAddAll(uint32 portId)
{
  int ret;
  uint32 i;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "dalMcTagNotTranslationAddAll: portId=%d.\r\n", portId);

  for(i=1; i<OP_MAX_VLAN_NUMBER; i++) {
    if(mcVlanMapExist(portId, i)) {
      ret = odmPortVttEntryAdd(portId, 0, i, 0, i);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
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
OPL_STATUS dalMcTagNotTranslationDelAll(uint32 portId)
{
  int ret;
  uint32 i;

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "dalMcTagNotTranslationDelAll: portId=%d.\r\n", portId);

  for(i=1; i<OP_MAX_VLAN_NUMBER; i++) {
    if(mcVlanMapExist(portId, i)) {
      ret = odmPortVttEntryDel(portId, 0, i, 0, i);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
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
OPL_STATUS Atheros8228_dalMcTagPortVlanModeChangeAction(uint32 portId)
{
  int ret;
  UINT32 i;
  uint32 vlanMode;

  ret = 0;
  i = 0;
  vlanMode = 0;

  ret = odmPortVlanModeGet(portId, &vlanMode);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "Atheros8228_dalMcTagPortVlanModeChangeAction: portId=%d,vlanMode=%d.\r\n", portId, vlanMode);

  if(mcTagOperPerPortGet(portId)==MC_TAG_OPER_MODE_TRANSLATION) {
    switch(vlanMode) {
    case ODM_VLAN_TRANSLATION:
      /* clear acl entries */
      dalMcTagNotTranslationDelAll(portId);
      dalMcTagTranslationDelAll(portId, ODM_VLAN_TRANSPARENT);
      dalMcTagTranslationAddAll(portId, ODM_VLAN_TRANSLATION);
      break;
    case ODM_VLAN_TAG:
    case ODM_VLAN_TRANSPARENT:
    case ODM_VLAN_AGGREGATION:
      /* clear acl entries */
      dalMcTagNotTranslationDelAll(portId);
      dalMcTagTranslationDelAll(portId, ODM_VLAN_TRANSLATION);
      dalMcTagTranslationAddAll(portId, ODM_VLAN_TRANSPARENT);
      break;
    default:
      break;
    }
  }
  else { /* MC_TAG_OPER_MODE_TRANSPARENT or MC_TAG_OPER_MODE_STRIP */
    switch(vlanMode) {
    case ODM_VLAN_TRANSLATION:
      dalMcTagTranslationDelAll(portId, ODM_VLAN_TRANSLATION);
      dalMcTagTranslationDelAll(portId, ODM_VLAN_TRANSPARENT);
      dalMcTagNotTranslationAddAll(portId);

      break;
    case ODM_VLAN_TAG:
    case ODM_VLAN_TRANSPARENT:
    case ODM_VLAN_AGGREGATION:
      dalMcTagTranslationDelAll(portId, ODM_VLAN_TRANSLATION);
      dalMcTagTranslationDelAll(portId, ODM_VLAN_TRANSPARENT);
      dalMcTagNotTranslationDelAll(portId);
      break;
    default:
      break;
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
OPL_STATUS Switch_dalMcTagPortVlanModeChangeAction(uint32 portId)
{

#ifdef ONU_4PORT_AR8228
    Atheros8228_dalMcTagPortVlanModeChangeAction(portId);
#endif

  return OPL_OK;
}
#endif

/**
 * @}
 */

/* END FILE */

