

/**
 * @defgroup xxx XXX
 * @{
 */

/***** INCLUDE FILES *****/
#include "vos_types.h"
#include <lw_type.h>

#include <lw_drv_pub.h>

#include <lw_drv_req.h>

#include "mc_control.h"

extern uint8 MC_MODE_SWITCH_OCCUR; /*Add by huangmingjian 2013-12-06 for Bug 294*/ 

extern OPL_STATUS dalMulticastPortVlanMacDel( UINT8 portNum, UINT16 vlanId, UINT8 * macAddr, OPL_BOOL fuzzy );

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
INT32 DRV_AddPort2McastAddr(UINT32 uiLPort, UINT32 uiVlan, UINT8 aucMac[MAC_ADDR_LEN])
{
	DRV_RET_E Ret;
		vlan_id_t tdVid;
		mac_address_t stMacAddress;
		mac_mcast_t stMacMcast;
		logic_pmask_t stLPortMask;

	if ((NULL == aucMac) || (FALSE == IsValidLgcPort(uiLPort))||(MAX_VLAN_ID < uiVlan))
	{
		return ERROR;
	}
		ClrLgcMaskAll(&stLPortMask);
	SetLgcMaskBit(uiLPort,&stLPortMask);

		memset(&stMacMcast, 0, sizeof(stMacMcast));
	memset(&stMacAddress, 0, sizeof(stMacAddress));
	memcpy(stMacAddress, aucMac, MAC_ADDR_LEN);

	tdVid = (vlan_id_t)uiVlan;
#if 0
		Ret = Ioctl_GetMacMcastMac(tdVid, stMacAddress, &stMacMcast);
#else
	Ret = Ioctl_ctc_mc_get_mac(tdVid, stMacAddress, &stMacMcast);
#endif
	
	if (DRV_ERR_MAC_ADDR_NOT_FOUND == Ret)
	{
		memset(&stMacMcast, 0, sizeof(stMacMcast));
		stMacMcast.tdVid = tdVid;
				LgcMaskCopy(&stMacMcast.port_mask,&stLPortMask);
	}
	else if (DRV_OK == Ret)
	{
		LgcMaskOr(&(stMacMcast.port_mask), &stLPortMask);
	}
	else
	{
			return ERROR;
	}
  	memcpy(stMacMcast.mac_addr, stMacAddress, sizeof(mac_address_t));

		mc_printf("\n%s portmsk:0x%04X mc-mac %02X%02X-%02X%02X-%02X%02X.\n",__FUNCTION__,
			 stLPortMask.pbits[0],stMacMcast.mac_addr[0],stMacMcast.mac_addr[1],
			 stMacMcast.mac_addr[2],stMacMcast.mac_addr[3],
			 stMacMcast.mac_addr[4],stMacMcast.mac_addr[5]);
#if 0
	Ret = Ioctl_SetMacSetMcastMac(stMacMcast);
#else
	Ret = Ioctl_ctc_mc_set_mac(stMacMcast);
#endif
	if (DRV_OK != Ret)
	{
		return ERROR;
	}

	return NO_ERROR;
}
STATIC INT32 DRV_CheckVlanForMcMode(UINT32 uiVlanId)
{
	INT32 iRet;
	UINT32 uiLPortIndex;
	CTC_VLAN_CFG_S stVlanMode;
	

	for (uiLPortIndex = MIN_LOGIC_UNI_ID; uiLPortIndex <= MAX_LOGIC_UNI_ID; uiLPortIndex++)
	{
			if (DRV_OK != Ioctl_ctc_get_acl_rule(uiLPortIndex, &stVlanMode))
			{
				printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
				return FALSE;
			}

		if (CTC_VLAN_MODE_TRANSLATION == stVlanMode.mode)
		{
			UINT32 uiVlanIndex = 0;

			if (uiVlanId == stVlanMode.default_vlan)
			{
				return FALSE;
			}

			for (uiVlanIndex = 0; uiVlanIndex < stVlanMode.number_of_entries; uiVlanIndex++)
			{
				/*The case that pvid of tag vlan mode equals old vlan of translation mode is not allowed.*/
				if ((stVlanMode.vlan_list[uiVlanIndex*2] != stVlanMode.vlan_list[uiVlanIndex*2 + 1]) &&
					((uiVlanId == stVlanMode.vlan_list[uiVlanIndex*2]) ||
					 (uiVlanId == stVlanMode.vlan_list[uiVlanIndex*2 + 1])))
				{
					return FALSE;
				}
			}
		}
		else if (CTC_VLAN_MODE_TAG == stVlanMode.mode)
		{
			if (uiVlanId == stVlanMode.default_vlan)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

INT32 DRV_AddPort2McastVlan(UINT32 uiLPort, UINT32 uiMcVid, MC_TAG_MODE_E enTagMode)
{
        DRV_RET_E ulRetVal = DRV_OK;
    logic_pmask_t stLMask, stUntagLMsk;

    if ((!IsValidLgcPort(uiLPort))   ||
        (!VALID_VLAN_ID(uiMcVid)) ||
        (MC_TAG_MODE_END < enTagMode))
    {
        return ERROR;
    }

    if (TRUE != DRV_CheckVlanForMcMode(uiMcVid))
    {
        return ERROR;
    }

        ClrLgcMaskAll(&stLMask);
        ClrLgcMaskAll(&stUntagLMsk);
    #if 0
        ulRetVal = Ioctl_GetVlanMember(uiMcVid, &stLMask, &stUntagLMsk);
    #else
    ulRetVal = Ioctl_ctc_mc_get_vlan_mem(uiMcVid, &stLMask, &stUntagLMsk);
    #endif
        if (DRV_OK != ulRetVal)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    switch (enTagMode)
    {
        case MC_TAG_MODE_TRANSPARENT:
            if (TstLgcMaskBit(uiLPort, &stLMask) && (!TstLgcMaskBit(uiLPort, &stUntagLMsk)))
            {
                return NO_ERROR;
            }
            else
            {
                                SetLgcMaskBit(uiLPort, &stLMask);
                ClrLgcMaskBit(uiLPort, &stUntagLMsk);
                #if 0
                                SetLgcMaskBit(LOGIC_CPU_PORT, &stLMask);
                                SetLgcMaskBit(LOGIC_PON_PORT, &stLMask);
                #endif
            }
			
			break;
		case MC_TAG_MODE_STRIP:
			if (TstLgcMaskBit(uiLPort, &stLMask) && TstLgcMaskBit(uiLPort, &stUntagLMsk))
			{
				return NO_ERROR;
			}
			else
			{
				SetLgcMaskBit(uiLPort, &stLMask);
	#if 0
								SetLgcMaskBit(LOGIC_CPU_PORT, &stLMask);
								SetLgcMaskBit(LOGIC_UPPON_PORT, &stLMask);
	#endif
								SetLgcMaskBit(uiLPort,&stUntagLMsk);
			}
	
			break;
		default :
			/*Do not support mc vlan translation.*/
			return ERROR;
			break;
	}
	
	ulRetVal = Ioctl_SetMcVlanMemberAdd(uiMcVid, stLMask, stUntagLMsk);
	if (DRV_OK != ulRetVal)
	{
		mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return ERROR;
	}
	return NO_ERROR;
}


INT32 DRV_GetSwitchUplinkLPort(UINT32 *puiLPort)
{
	if(puiLPort == NULL)
		{
			return ERROR;
		}

	*puiLPort = LOGIC_PON_PORT;

	return NO_ERROR;
}

INT32 DRV_EnableSwitchMc(INT32 iEnable)
{
    INT32 Ret;
    UINT32 uiLPortId;

    if ((TRUE != iEnable) && (FALSE != iEnable))
    {
        return ERROR;
    }

    if (TRUE == iEnable)
    {
        /*Trap igmp protocol packet to cpu.*/
        Ret = Ioctl_SetIgmpPktAction(PASS_TYPE_CPU_ONLY);
        if (DRV_OK != Ret)
        {
            return ERROR;
        }

        /*Drop unknown igmp data.*/
                //Ret = Ioctl_SetMcastLookupMissDrop(ENABLE);
                Ret = Ioctl_igmp_unknow_ip4_act(MCAST_ACT_DROP);
        if (DRV_OK != Ret)
        {
            return ERROR;
        }
    }
    else
    {
        /*Forward igmp protocol packet to any port.*/
        Ret = Ioctl_SetIgmpPktAction(PASS_TYPE_ALLPORTS);
        if (DRV_OK != Ret)
        {
            return ERROR;
        }

        /*Do not drop unknown igmp data.*/
        //Ret = Ioctl_SetMcastLookupMissDrop(DISABLE);
        Ret = Ioctl_igmp_unknow_ip4_act(MCAST_ACT_FORWARD);
        if (DRV_OK != Ret)
        {
            return ERROR;
        }
    }

    return NO_ERROR;
}


INT32 DRV_DelPortFromMcastAddr(UINT32 uiLPort, UINT32 uiVlan, UINT8 aucMac[MAC_ADDR_LEN])
{
    DRV_RET_E Ret;
	vlan_id_t tdVid;
	mac_address_t stMacAddress;
	mac_mcast_t stMacMcast;
	logic_pmask_t stLPortMask;

    if ((NULL == aucMac) || (FALSE == IsValidLgcPort(uiLPort))||(MAX_VLAN_ID < uiVlan))
    {
        mc_printf("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
	ClrLgcMaskAll(&stLPortMask);
	SetLgcMaskBit(uiLPort,&stLPortMask);

	memset(&stMacMcast, 0, sizeof(stMacMcast));
	memcpy(&stMacAddress[0], aucMac, sizeof(stMacAddress));

    tdVid = (vlan_id_t)uiVlan;
   
    Ret = Ioctl_ctc_mc_get_mac(tdVid, stMacAddress, &stMacMcast);
    
    if(DRV_OK == Ret)
    {
    	memcpy(stMacMcast.mac_addr, stMacAddress, sizeof(mac_address_t));
#if 0
        mc_printf("\n%s portmsk:0x%04X mc-mac %02X%02X-%02X%02X-%02X%02X.\n",__FUNCTION__,
             stLPortMask.pbits[0],stMacMcast.mac_addr[0],stMacMcast.mac_addr[1],
             stMacMcast.mac_addr[2],stMacMcast.mac_addr[3],
             stMacMcast.mac_addr[4],stMacMcast.mac_addr[5]);
#endif	
		 LgcMaskAnd(&(stMacMcast.port_mask), &stLPortMask);
#if 0
		Ret = Ioctl_SetMacDelMcastMac(stMacMcast);
#else
		Ret = Ioctl_ctc_mc_del_mac(stMacMcast);
#endif
		if(DRV_OK != Ret)
		{
			mc_printf("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
			return ERROR;
		}
	}
	else
	{
		/*not exist*/
	    mc_printf("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
		return ERROR;
	}

	return NO_ERROR;
}
INT32 DRV_DelPortFromMcastVlan(UINT32 uiLPort, UINT32 uiMcVid)
{
        DRV_RET_E ulRetVal = DRV_OK;
    #if 0
    logic_pmask_t stLMask, stUntagLMsk, stTmpMsk;
    #else
    logic_pmask_t stLMask;
    #endif

    if ((!IsValidLgcPort(uiLPort))   ||
        (!VALID_VLAN_ID(uiMcVid)))
    {
        return ERROR;
    }

    /*Return error if this vid is not multicast vlan.*/
    if (TRUE != DRV_CheckVlanForMcMode(uiMcVid))
    {
        return ERROR;
    }

	ClrLgcMaskAll(&stLMask);
	SetLgcMaskBit(uiLPort, &stLMask);
	ulRetVal = Ioctl_ctc_mc_rmv_vlan_mem(uiMcVid, stLMask);
	if (DRV_OK != ulRetVal)
	{
		mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return ERROR;
	}
	return NO_ERROR;
}
INT32 DRV_DelPortFromMcastTransltVlan(UINT32 uiLPort, UINT32 uiMcVid,UINT32 uiUsrVid)
{
        DRV_RET_E ulRetVal = DRV_OK;
    #if 0
    logic_pmask_t stLMask, stUntagLMsk, stTmpMsk;
    #else
    logic_pmask_t stLMask;
    #endif

    if ((!IsValidLgcPort(uiLPort))   ||
        (!VALID_VLAN_ID(uiUsrVid)))
    {
        return ERROR;
    }

    /*Return error if this vid is not multicast vlan.*/
    if (TRUE != DRV_CheckVlanForMcMode(uiUsrVid))
    {
        return ERROR;
    }

	ClrLgcMaskAll(&stLMask);
	SetLgcMaskBit(uiLPort, &stLMask);
	ulRetVal = ioctl_SetMctransltVlanMemberRmv(uiUsrVid,uiMcVid, stLMask);
	if (DRV_OK != ulRetVal)
	{
		mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return ERROR;
	}
	return NO_ERROR;
}

OPL_STATUS dalMcVlanMapAdd(uint32 portId, uint32 vlanId)
{
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

  //vosSprintf(vlanIdStr, "igmp%d", vlanId);
  //odmCreateVlanName(vlanIdStr);
 
{
  //extern int odmSetVlanNameTagCfg(char  *name, UINT16 vid);
//  extern int32 DRV_AddPort2McastVlan(uint32 uiLPort, uint32 uiMcVid, int enTagMode);
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
OPL_STATUS dalMcVlanMapDel(uint32 portId, uint32 vlanId)
{

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
      ret = DRV_DelPortFromMcastVlan(portId, vlanId);
      if (NO_ERROR != ret)
      {
        MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
      }    
      
      break;
    case MC_TAG_OPER_MODE_STRIP:
      ret = DRV_DelPortFromMcastVlan(portId, vlanId);
      if (NO_ERROR != ret)
      {
        MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
      }
      
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




}

OPL_STATUS dalMcTagTranslationAdd(uint32 portId, uint32 mcVid, uint32 userVid)
{
	DRV_RET_E ulRetVal = DRV_OK;
	logic_pmask_t stLMask, stUntagLMsk,stSvlanUntagLMsk;

    if ((!IsValidLgcPort(portId))   ||
        (!VALID_VLAN_ID(mcVid)) ||
        (!VALID_VLAN_ID(userVid)))
    {
        return ERROR;
    }

    if (TRUE != DRV_CheckVlanForMcMode(mcVid))
    {
        return ERROR;
    }
	if (TRUE != DRV_CheckVlanForMcMode(userVid))
    {
        return ERROR;
    }
    ClrLgcMaskAll(&stLMask);
    ClrLgcMaskAll(&stUntagLMsk);
   
    ulRetVal = Ioctl_ctc_mc_get_vlan_mem(mcVid, &stLMask, &stUntagLMsk);
    
    if (DRV_OK != ulRetVal)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
      //  return ERROR;
    }

 
    if (TstLgcMaskBit(portId, &stLMask))
    {
        return NO_ERROR;
    }
    else
    {
		SetLgcMaskBit(portId, &stLMask);
		SetLgcMaskBit(portId, &stSvlanUntagLMsk);
        ClrLgcMaskBit(portId, &stUntagLMsk);
    }
	

	
	//ulRetVal = Ioctl_SetMcVlanMemberAdd(uiMcVid, stLMask, stUntagLMsk);
	ulRetVal=ioctl_SetMcTransltVlanMemberAdd(userVid,mcVid,stLMask,stUntagLMsk,stSvlanUntagLMsk);
	if (DRV_OK != ulRetVal)
	{
		mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return ERROR;
	}
	ulRetVal=ioctl_ctcadds2centry(portId,mcVid,userVid);
	if (DRV_OK != ulRetVal)
	{
		mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return ERROR;
	}
	ulRetVal=ioctl_ctcaddc2sentry(portId,userVid,mcVid);
	if (DRV_OK != ulRetVal)
	{
		mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return ERROR;
	}	
  return OPL_OK;

}


OPL_STATUS dalMcTagTranslationDel(uint32 portId, uint32 mcVid, uint32 userVid)
{

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


	//MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, vlanId);
	//printf("dalMcTagTranslationDel port %d mcVid %d userVid %d\n",portId,mcVid,userVid);
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
	    ret = dalMulticastPortVlanMacDel((UINT8)portId, userVid, groupMac, OPL_FALSE);
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
	                                            groupAddress, userVid, 
	                                            uiBitMap))
	  {
	    MC_GROUP_t *pgroup = NULL;
	    pgroup = mcGroupListGet(portId, groupMac, groupAddress, userVid, uiBitMap);
	    if (NULL != pgroup)
	    {
	      (void)mcHostListDestory(pgroup);
	    }
	  }

	  /*delete group list*/
	  (void)mcGroupListDel(portId, groupMac, groupAddress, userVid, uiBitMap);
	  /* 014549 */
	  m++;
	}
	}


	ret=ioctl_ctcdels2centry(portId,mcVid);
	if (DRV_OK != ret)
	{
		//printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return ERROR;
	}
	ret=ioctl_ctcdelc2sentry(portId,userVid,mcVid);
	if (DRV_OK != ret)
	{
		//printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
		return ERROR;
	}	
	ret = DRV_DelPortFromMcastTransltVlan(portId,mcVid,userVid);
	if (NO_ERROR != ret)
	{
		MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret);
		//printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
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
OPL_STATUS dalMcVlanInTransparentModeSetAction(uint32 portId)
{

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
     
      {
        //extern int32 DRV_AddPort2McastVlan(uint32 uiLPort, uint32 uiMcVid, int enTagMode);
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
OPL_STATUS dalMcVlanInTransparentModeClear(uint32 portId)
{


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

    
     
      {
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
OPL_STATUS dalMcVlanInStripModeSetAction(uint32 portId)
{
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
   
      {
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
OPL_STATUS dalMcVlanInStripModeClear(uint32 portId)
{


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

      
    
      {
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
     
     

    }
  }
  return OPL_OK;
}


OPL_STATUS dalMcVlanInTranslationModeSetAction(uint32 portId)
{
  int ret;
  UINT32 j;
  UINT32 vttIndex;

  /*Begin add by huangmingjian 2013-10-26:Bug 203,257*/
  if(MC_MODE_CTC == mcControlBlockModeGet())
  {	
	return OPL_OK;
  }
  /*End add by huangmingjian 2013-10-26:Bug 203,257*/
  ret = 0;
  vttIndex = 0;

  for(j=1; j<OP_MAX_VLAN_NUMBER; j++) {
    if(mcVlanMapExist(portId, j)) {
      MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, portId=%d, vlanId=%d.\r\n", __FUNCTION__, __LINE__, portId, j);

      
      ret = dalMcTagTranslationAdd(portId,j, j);
      if(ret!=OPL_OK) { MC_DEBUG(DEBUG_LEVEL_DEBUGGING, "%s,%d, ret=0x%X,%d.\r\n", __FUNCTION__, __LINE__, ret, ret); }
      RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
	  /*Begin add by huangmingjian 2013-11-15:Bug 273*/
	  ret = mcTagTranslationTableAdd(portId,j, j);
  	  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
	  /*End add by huangmingjian 2013-11-15:Bug 273*/
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
OPL_STATUS dalMcVlanInXxxModeSetAction(uint32 portId, MC_TAG_OPER_MODE_t tagOper)
{

  int ret;

  ret = 0;
  uint16  vlanId[OP_MAX_VLAN_NUMBER] = {0};
  uint8 num = 0;

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
     	
	  #if 0	 /*Modeifed by huangmingjian 2013-11-22*/
	  ret = dalMcVlanInTranslationModeSetAction(portId);
		 RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);
      mc_printf("\nfunc %s:Do not support mcvlan translation.\n",__FUNCTION__);
      #endif
	  /* do nothing */
      break;
    default:
      /* do nothing */
      break;
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
OPL_STATUS dalMcVlanInXxxModeClear(uint32 portId, MC_TAG_OPER_MODE_t tagOper)
{

  int ret;

  ret = 0;
  uint16  vlanId[OP_MAX_VLAN_NUMBER] = {0};
  uint8 num = 0;

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
	  /*Begin add by huangmingjian 2013-12-06 for Bug 294*/
	  if(0==MC_MODE_SWITCH_OCCUR) /*it is not mode switch, so we clear all translation*/
	  {
		  ret = odmMcTagTranslationClear(portId);
		  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret); 
	  }
	  /*End add by huangmingjian 2013-12-06 for Bug 294*/ 

      break;
    default:
      /* do nothing */
      break;
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
OPL_STATUS dalMcTagOperSet(uint32 portId, MC_TAG_OPER_MODE_t tagOper)
{
  int ret;

  /* clear old mode info */
  ret = dalMcVlanInXxxModeClear(portId, mcTagOperPerPortGet(portId));

  ret = dalMcVlanInXxxModeSetAction(portId, tagOper);
  RETURN_VAL_IF_FAIL(ret == OPL_OK, ret);

  return OPL_OK;
}
OPL_STATUS dalMulticastPortVlanMacAdd( UINT8 portNum, UINT16 vlanId, UINT8 * macAddr, OPL_BOOL fuzzy )
{
    //extern INT32 DRV_AddPort2McastAddr(UINT32 uiLPort, UINT32 uiVlan, UINT8 aucMac [6]);
    INT32 ret;
   
    ret = DRV_AddPort2McastAddr(portNum, vlanId, macAddr);
    if (0 != ret)
    {
        mc_printf("\nfunc:%s failed to set mc mac.\n",__FUNCTION__);
        return OPL_ERROR;
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
OPL_STATUS dalMcGroupAdd(uint32 hostPortId, uint8 *groupMac, uint32 groupAddress, uint32 mcVlan, uint32 bitmap)
{
  uint8 tempGroupMac[MAC_ADDRESS_LENGTH];
  OPL_STATUS ret;
  UINT16 vlanId[OP_MAX_VLAN_NUMBER] = {0};
  UINT8 num= 0;
  UINT8 m;
  MC_GROUP_t *group;
  UINT32 state;
  
  vosMemSet(tempGroupMac, 0, MAC_ADDRESS_LENGTH);

  /************************ group add ************************/
  /* Snooping mode */
  if(mcControlBlockModeGet()==MC_MODE_SNOOPING)
  {
    clMcMacGetByMcIp(tempGroupMac, groupAddress);
    
	/*Begin modified by huangmingjian 2013-12-06 for Bug 297:switch mode faild*/
    if(mcTagOperPerPortGet(hostPortId)==MC_TAG_OPER_MODE_TRANSLATION)
    {
		odmTranslationVlanGet(hostPortId, vlanId, &num);
    }
    else
	{
		odmMulticastVlanGet(hostPortId, vlanId, &num);
	}
    /*End modified by huangmingjian 2013-12-06 for Bug 297*/
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
          
	  /*Begin modified by huangmingjian 2013-12-06 for Bug 297:switch mode faild*/
	  if(mcTagOperPerPortGet(hostPortId)==MC_TAG_OPER_MODE_TRANSLATION)
	  {
		  odmTranslationVlanGet(hostPortId, vlanId, &num);
	  }
	  else
	  {
		  odmMulticastVlanGet(hostPortId, vlanId, &num);
	  }
	  /*End modified by huangmingjian 2013-12-06 for Bug 297*/
	  
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
    
   
  
    
  }



  return OPL_OK;
}
OPL_STATUS dalMulticastPortVlanMacDel( UINT8 portNum, UINT16 vlanId, UINT8 * macAddr, OPL_BOOL fuzzy )
{
    INT32 ret;
    //printf("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
    ret = DRV_DelPortFromMcastAddr(portNum, vlanId, macAddr);
    if (0 != ret)
    {
        mc_printf("\nfunc:%s failed to delete port from mc mac.\n",__FUNCTION__);
        return OPL_ERROR;
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
OPL_STATUS dalMcGroupDel(uint32 hostPortId, uint8 *groupMac, uint32 groupAddress, uint32 mcVlan, uint32 bitmap)
{
  uint8 tempGroupMac[MAC_ADDRESS_LENGTH];
  OPL_STATUS ret;
  UINT16 vlanId[OP_MAX_VLAN_NUMBER];
  UINT8 num= 0;
  UINT8 m;
  
  vosMemSet(tempGroupMac, 0, MAC_ADDRESS_LENGTH);

  /************************ group del ************************/

  /* For all mc vlan */
  if(mcControlBlockModeGet()==MC_MODE_SNOOPING)
  {
      clMcMacGetByMcIp(tempGroupMac, groupAddress);
      
	  /*Begin modified by huangmingjian 2013-12-06 for Bug 297:switch mode faild*/
 	  if(mcTagOperPerPortGet(hostPortId)==MC_TAG_OPER_MODE_TRANSLATION)
 	  {
 		odmTranslationVlanGet(hostPortId, vlanId, &num);
 	  }
 	  else
 	  {
 		odmMulticastVlanGet(hostPortId, vlanId, &num);
 	  }
	  /*End modified by huangmingjian 2013-12-06 for Bug 297*/
	  
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
      
	  /*Begin modified by huangmingjian 2013-12-06 for Bug 297:switch mode faild*/
 	  if(mcTagOperPerPortGet(hostPortId)==MC_TAG_OPER_MODE_TRANSLATION)
 	  {
 		odmTranslationVlanGet(hostPortId, vlanId, &num);
 	  }
 	  else
 	  {
 		odmMulticastVlanGet(hostPortId, vlanId, &num);
 	  }
	  /*End modified by huangmingjian 2013-12-06 for Bug 297*/

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
    
   
    
  }

 
  return OPL_OK;
}


/**
 * @}
 */

/* END FILE */

