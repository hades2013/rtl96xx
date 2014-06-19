/*
=============================================================================
     Header Name: dal_igmp.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/9		Initial Version
----------------   ------------  ----------------------------------------------
*/

#include "dal_lib.h"

GROUP_ADDR_CLS_INDEX_MAP_t  groupAddrClsIndexMapTab[MC_MAX_GROUP_NUM];
UINT16 mcIgmpControlRuleId;
static UINT16 mcDataFilterRuleId = 0xFFFF;

/*******************************************************************************
* dalMulticastInit
*
* DESCRIPTION:
*  		this function drop all multicast packets from olt except the destma equeal the table.
*		send all oam packet to cpu.
*
*	INPUTS:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastInit(void)
{
	OPL_STATUS retVal = OPL_OK;
       
	OPL_MEMSET(&groupAddrClsIndexMapTab[0],0x00,MC_MAX_GROUP_NUM*sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
	/*drop all multicast packets*/
#if 0
	retVal = dalClsIngressPortCfg(MULTCAST_CLS_STARTID -2,CLASSIFY_EII_IP,1,1,1);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
#endif
	/*send all igmp packets to cpu*/
	retVal = dalClsTypeLenCfg(MULTCAST_CLS_STARTID -1, CLASSIFY_EII_IP,0x0800, 0xffff, OPL_ENABLE);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	retVal = dalClsIpTypeCfg(MULTCAST_CLS_STARTID -1, CLASSIFY_EII_IP, 2, 0xff, OPL_ENABLE);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	retVal = dalClsDestMacAddrCfg(MULTCAST_CLS_STARTID -1, CLASSIFY_EII_IP, "01:00:5e:00:00:00", "ff:ff:ff:00:00:00", OPL_ENABLE);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	retVal = dalClsTranActCfg(MULTCAST_CLS_STARTID -1,CLASSIFY_EII_IP,COPY_PKTS_TO_CPU|DROP_PKTS);
	if(OPL_OK != retVal)
	{
		return retVal;
	}


  retVal = dalClsDestMacAddrCfg(MULTCAST_CLS_STARTID -2, CLASSIFY_EII_IP, "01:00:5e:00:00:00", "ff:ff:ff:00:00:00", OPL_ENABLE);
  if(OPL_OK != retVal)
  {
    return retVal;
  }

  retVal = dalClsTranActCfg(MULTCAST_CLS_STARTID -2,CLASSIFY_EII_IP,DROP_PKTS);
  if(OPL_OK != retVal)
  {
    return retVal;
  }

#if 0
  dalMulticastEnable();
#endif
  
	return retVal;
}
OPL_STATUS dalMulticastEnable()
{
    OPL_STATUS retVal = OPL_OK;
        
    OPL_MEMSET(&groupAddrClsIndexMapTab[0],0x00,MC_MAX_GROUP_NUM*sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));

    retVal = dalClsTranActCfg(MULTCAST_CLS_STARTID -2,CLASSIFY_EII_IP,DROP_PKTS);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	retVal = dalClsTranActCfg(MULTCAST_CLS_STARTID -1,CLASSIFY_EII_IP,COPY_PKTS_TO_CPU|DROP_PKTS);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

    retVal = dalMulticastDataFilterAdd();
    if(OPL_OK != retVal)
    {
    	return retVal;
    }
    
    return retVal;
    
}

OPL_STATUS dalMulticastDisable()
{
    OPL_STATUS retVal = OPL_OK;
        
    OPL_MEMSET(&groupAddrClsIndexMapTab[0],0x00,MC_MAX_GROUP_NUM*sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));

    retVal = dalClsTranActCfg(MULTCAST_CLS_STARTID -2,CLASSIFY_EII_IP,0);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	retVal = dalClsTranActCfg(MULTCAST_CLS_STARTID -1,CLASSIFY_EII_IP,0);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

    retVal = dalMulticastDataFilterDel();
    if(OPL_OK != retVal)
    {
    	return retVal;
    }
    
    return retVal;
}
/*******************************************************************************
* dalMUlticastVlanCreate
*
* DESCRIPTION:
*  		this function create a multicast vlan.
*
*	INPUTS:
           vlanId:VLAN ID.
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMUlticastVlanCreate(UINT16 vlanId)
{
    return OPL_OK;
}
/*******************************************************************************
* dalMUlticastVlanDelete
*
* DESCRIPTION:
*  		this function delete a multicast vlan.
*
*	INPUTS:
           vlanId:VLAN ID.
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMUlticastVlanDelete(UINT16 vlanId)
{
    return OPL_OK;
}
/*******************************************************************************
* dalMulticastVlanAddPort
*
* DESCRIPTION:
*  		this function used to add a port to vlan,default is MEMBER_EGRESS_UNTAGGED
*
*	INPUTS:
*			portNum:
*			vlanId:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastVlanAddPort(UINT8 portNum,UINT16 vlanId)
{
	UINT32 cVlan,sVlan;

	cVlan = vlanId;
	sVlan = vlanId;

	return dalVttMulticastEntryAdd( 0,1,cVlan,  sVlan, portNum);
}
/*******************************************************************************
* dalMulticastVlanDelPort
*
* DESCRIPTION:
*  		this function used to remove a port from a vlan.
*
*	INPUTS:
*			portNum:
*			vlanId:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastVlanDelPort(UINT8 portNum,UINT16 vlanId)
{
	UINT32 cVlan,sVlan;

	cVlan = vlanId;
	sVlan = vlanId;

	return dalVttMulticastEntryDel(0,1, cVlan,  sVlan, portNum);
}
/*******************************************************************************
* dalMulticastVlanTagStripe
*
* DESCRIPTION:
*  		this function used to disable or enable the egress with or without tag information
*
*	INPUTS:
*			portNum:
*			enable:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastVlanTagStripe(UINT8 portNum,UINT8 enable)
{
	return dalVttMulticastStripEnable(portNum,enable);
}
/*******************************************************************************
* dalMulticastArlAddPort
*
* DESCRIPTION:
*  		this function used to add a arl records(unicast),or add a port to a multicast records.
*
*	INPUTS:
*			portNum:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastArlAddPort(UINT8 portNum,UINT8 *macAddr)
{
	INT32 index;
	OPL_STATUS retVal = OPL_OK;
	UINT8 macString[MAC_STRING_LENG];

	if(OPL_NULL == macAddr)
	{
		return OPL_ERR_NULL_POINTER;
	}

	sprintf(macString, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);


	for(index = 0; index < MC_MAX_GROUP_NUM; index++)
	{
		if((!OPL_MEMCMP(groupAddrClsIndexMapTab[index].mac,macAddr,MAC_LENGTH))&&groupAddrClsIndexMapTab[index].valid == OPL_TRUE)
		{
			return OPL_OK;
		}
	}

	for(index = 0; index < MC_MAX_GROUP_NUM; index++)
	{
		if(!OPL_MEMCMP(groupAddrClsIndexMapTab[index].mac,macAddr,MAC_LENGTH))
		{
			groupAddrClsIndexMapTab[index].valid = OPL_TRUE;
			return OPL_OK;
		}
	}

	for(index = 0; index < MC_MAX_GROUP_NUM; index++)
	{
		if(groupAddrClsIndexMapTab[index].valid != OPL_TRUE)
		{
			retVal = dalClsTranActCfg(MULTCAST_CLS_STARTID + index, CLASSIFY_EII_IP, DONOT_DROP);
			if(OPL_OK != retVal)
			{
				OPL_MEMSET(&groupAddrClsIndexMapTab[index],0X0,sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
				return retVal;
			}
			retVal = dalClsIngressPortCfg(MULTCAST_CLS_STARTID + index, CLASSIFY_EII_IP,1, 1, 1);
			if(OPL_OK != retVal)
			{
				OPL_MEMSET(&groupAddrClsIndexMapTab[index],0X0,sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
				return retVal;
			}
			retVal = dalClsDestMacAddrCfg(MULTCAST_CLS_STARTID + index, CLASSIFY_EII_IP, macString, "FF:FF:FF:FF:FF:FF",OPL_ENABLE);
			if(OPL_OK != retVal)
			{
				OPL_MEMSET(&groupAddrClsIndexMapTab[index],0X0,sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
				return retVal;
			}
      /* 0800 */
			retVal = dalClsTypeLenCfg(MULTCAST_CLS_STARTID + index, CLASSIFY_EII_IP, 0x0800, 0xffff,OPL_ENABLE);
			if(OPL_OK != retVal)
			{
				OPL_MEMSET(&groupAddrClsIndexMapTab[index],0X0,sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
				return retVal;
			}
      /* udp */
			retVal = dalClsIpTypeCfg(MULTCAST_CLS_STARTID + index, CLASSIFY_EII_IP, 17, 0xff,OPL_ENABLE);
			if(OPL_OK != retVal)
			{
				OPL_MEMSET(&groupAddrClsIndexMapTab[index],0X0,sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
				return retVal;
			}
			groupAddrClsIndexMapTab[index].valid = OPL_TRUE;
			groupAddrClsIndexMapTab[index].clsIndex = MULTCAST_CLS_STARTID + index;
			OPL_MEMCPY(groupAddrClsIndexMapTab[index].mac,macAddr,MAC_LENGTH);
			return OPL_OK;
		}
	}
	if(index == MC_MAX_GROUP_NUM)
	{
		OPL_DAL_PRINTF(("multicast address table is full.\n"));
	}
	return OPL_OK;
}

OPL_STATUS dalMulticastArlAddPortTest(UINT8 portNum,UINT8 *macString)
{
	UINT8 macAddr[6];

	string2mac(macString,macAddr);

	return dalMulticastArlAddPort(portNum,&macAddr[0]);
}

/*******************************************************************************
* dalMulticastArlPortDel
*
* DESCRIPTION:
*  		this function used to remove a arl records(unicast),or remove a port from a multicast group.
*
*	INPUTS:
*			portNum:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastArlPortDel(UINT8 portNum,UINT8 *macAddr)
{
	INT32 index;
	OPL_STATUS retVal = OPL_OK;
	UINT8 macString[MAC_STRING_LENG];

	if(OPL_NULL == macAddr)
	{
		return OPL_ERR_NULL_POINTER;
	}

	sprintf(macString, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);

	for(index = 0; index < MC_MAX_GROUP_NUM; index++)
	{
		if((!OPL_MEMCMP(groupAddrClsIndexMapTab[index].mac,macAddr,MAC_LENGTH))&&groupAddrClsIndexMapTab[index].valid == OPL_TRUE)
		{
			retVal = dalClsRuleDel(groupAddrClsIndexMapTab[index].clsIndex);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			OPL_MEMSET(&groupAddrClsIndexMapTab[index],0X0,sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
			return OPL_OK;
		}
	}

	for(index = 0; index < MC_MAX_GROUP_NUM; index++)
	{
		if(!OPL_MEMCMP(groupAddrClsIndexMapTab[index].mac,macAddr,MAC_LENGTH))
		{
			OPL_MEMSET(&groupAddrClsIndexMapTab[index],0X0,sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
			return OPL_OK;
		}
	}

	return OPL_OK;
}
/*******************************************************************************
* dalMulticastArlVlanAdd
*
* DESCRIPTION:
*  		this function used add a multicast group:port+vlan+mac
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastArlVlanAdd(UINT8 portNum,UINT16 vlanId,UINT8 *macAddr)
{
    OPL_STATUS retVal = OPL_OK;

	UINT32 cVlan,sVlan;

	cVlan = vlanId;
	sVlan = vlanId;
	retVal = dalVttMulticastEntryAdd( 0,1,cVlan,  sVlan, portNum);
    if(OPL_OK != retVal)
    {
        return retVal;
    }

    return dalMulticastArlAddPort(portNum,macAddr);
}
/*******************************************************************************
* dalMulticastArlVlanDel
*
* DESCRIPTION:
*  		this function used remove a multicast group:port+vlan+mac
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastArlVlanDel(UINT8 portNum,UINT16 vlanId,UINT8 *macAddr)
{
    OPL_STATUS retVal = OPL_OK;

	UINT32 cVlan,sVlan;

	cVlan = vlanId;
	sVlan = vlanId;
	retVal = dalVttMulticastEntryDel( 0,1,cVlan,  sVlan, portNum);
    if(OPL_OK != retVal)
    {
        return retVal;
    }
    return dalMulticastArlPortDel(portNum,macAddr);
}
/*******************************************************************************
* dalMulticastArlTabFlush
*
* DESCRIPTION:
*  		this function used flush all arl table
*
*	INPUTS:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastArlTabFlush(void)
{
    OPL_STATUS retVal = OPL_OK;

    retVal = dalVttMulticastInit(1);

    return retVal;
}

/*******************************************************************************
* dalMulticastPortVlanMacAdd
*
* DESCRIPTION:
*  		this function used to add a multicast control rule: port+vlan+mac
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastPortVlanMacAdd(UINT8 portNum, UINT16 vlanId, UINT8 *macAddr, OPL_BOOL fuzzy)
{
/* Begin Modified 2011-12-8 */
//#ifndef CTC_MULTICAST_SURPORT
#if 1
    INT32 index;
    OPL_STATUS retVal = OPL_OK;
    CLS_CONFIG_INFO_t stClassCfg;
    UINT16 ruleId;
#endif            
/* End   Modified 2011-12-8*/

    if(OPL_NULL == macAddr)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
/* Begin Modified 2011-12-8 */
//#ifndef CTC_MULTICAST_SURPORT   
#if 1
    OPL_DAL_PRINTF(("dalMulticastPortVlanMacAdd Input Parameters: portNum=%d vlanId=%d macAddr=%02x%02x%02x%02x%02x%02x.\n", 
        portNum, vlanId, macAddr[0], macAddr[1], 
        macAddr[2], macAddr[3], macAddr[4], macAddr[5]));

    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
        /* 01:00:5e:xx:xx:xx */
        if(((fuzzy && (!OPL_MEMCMP(groupAddrClsIndexMapTab[index].mac, macAddr, MAC_LENGTH/2))) 
            || (!fuzzy && (!OPL_MEMCMP(groupAddrClsIndexMapTab[index].mac, macAddr, MAC_LENGTH))))
            && (groupAddrClsIndexMapTab[index].vid == vlanId) 
            && (groupAddrClsIndexMapTab[index].valid == OPL_TRUE))
        {
            OPL_DAL_PRINTF(("%s[%d]: Multicast Classify Rule Exist.\n", __FUNCTION__, __LINE__));
            return OPL_OK;
        }
    }

    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
        if (groupAddrClsIndexMapTab[index].valid != OPL_TRUE)
        {
            OPL_MEMSET((char *)&stClassCfg, 0, sizeof(stClassCfg));

            /* Valid */
            stClassCfg.valid = 1;

            /* Action */
            stClassCfg.t_act = DONOT_DROP;

            /* SRC Port, not used for single port mode */
#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
            stClassCfg.ports[0] = portNum;
#endif

            /* GDA MAC, 01:00:5e:xx:xx:xx */
            stClassCfg.dstMacFlag = 1;
            stClassCfg.dstMac.lowRange[0] = macAddr[0];
            stClassCfg.dstMac.lowRange[1] = macAddr[1];
            stClassCfg.dstMac.lowRange[2] = macAddr[2];
            stClassCfg.dstMac.lowRange[3] = macAddr[3];
            stClassCfg.dstMac.lowRange[4] = macAddr[4];
            stClassCfg.dstMac.lowRange[5] = macAddr[5];
            stClassCfg.dstMac.highRange[0] = 0xff;
            stClassCfg.dstMac.highRange[1] = 0xff;
            stClassCfg.dstMac.highRange[2] = 0xff;
            if (fuzzy)
            {
                stClassCfg.dstMac.highRange[3] = 0x0;
                stClassCfg.dstMac.highRange[4] = 0x0;
                stClassCfg.dstMac.highRange[5] = 0x0;
            }
            else
            {
                stClassCfg.dstMac.highRange[3] = 0xff;
                stClassCfg.dstMac.highRange[4] = 0xff;
                stClassCfg.dstMac.highRange[5] = 0xff;
            }
            
            /* VID */
            stClassCfg.vlanIdFlag = 1;
            stClassCfg.vlanId.lowRange = vlanId;
            stClassCfg.vlanId.highRange = 0xffff;

            /* Type or Length, 0x0800 */
            stClassCfg.lenOrTypeFlag = 1;
            stClassCfg.lenOrType.lowRange = 0x0800;
            stClassCfg.lenOrType.highRange = 0xffff;

            /* IP Type, UDP */
            stClassCfg.ipTypeFlag = 1;
            stClassCfg.ipType.lowRange = 17;
            stClassCfg.ipType.highRange = 0xff;

            /* Do not change COS value */
            stClassCfg.cosMapVal = CTC_ETHER_PRI_END;
            stClassCfg.queueMapId = CTC_QUEUE_MAP_END;
            
#if 0
            stClassCfg.bingress=0x01;
            stClassCfg.ingressmask=0x01;
#endif
      
            retVal = dalVoipClsRuleCtcAdd(&stClassCfg, &ruleId);
            if (retVal != OPL_OK)
            {
                OPL_MEMSET(&groupAddrClsIndexMapTab[index], 0x0, sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
                OPL_DAL_PRINTF(("%s[%d]: dalVoipClsRuleCtcAdd Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
                return retVal;
            }
            
            groupAddrClsIndexMapTab[index].valid = OPL_TRUE;
            groupAddrClsIndexMapTab[index].clsIndex = ruleId;
            OPL_MEMCPY(groupAddrClsIndexMapTab[index].mac,macAddr,MAC_LENGTH);
            groupAddrClsIndexMapTab[index].vid = vlanId;
            
            OPL_DAL_PRINTF(("Multicast Classify Rule Added: valid=%d index=%d macAddr=%02x%02x%02x%02x%02x%02x vlanId=%d\n", 
                groupAddrClsIndexMapTab[index].valid, groupAddrClsIndexMapTab[index].clsIndex, 
                groupAddrClsIndexMapTab[index].mac[0], groupAddrClsIndexMapTab[index].mac[1], 
                groupAddrClsIndexMapTab[index].mac[2], groupAddrClsIndexMapTab[index].mac[3], 
                groupAddrClsIndexMapTab[index].mac[4], groupAddrClsIndexMapTab[index].mac[5], 
                groupAddrClsIndexMapTab[index].vid));

            return OPL_OK;
        }
    }
    
    if (index == MC_MAX_GROUP_NUM)
    {
        OPL_DAL_PRINTF(("multicast address table is full.\n"));
    }
#else
{
    extern INT32 DRV_AddPort2McastAddr(UINT32 uiLPort, UINT32 uiVlan, UINT8 aucMac [6]);
    INT32 ret;
    printk("\n%s adds port %d to mc-mac atu %02X%02X-%02X%02X-%02X%02X.\n",__FUNCTION__,
             portNum,macAddr[0],macAddr[1],macAddr[2],macAddr[3],
             macAddr[4],macAddr[5]);
    ret = DRV_AddPort2McastAddr(portNum, vlanId, macAddr);
    if (0 != ret)
    {
        return OPL_ERROR;
    }
}
#endif
/* End   Modified 2011-12-8*/    
    return OPL_OK;
}

/*******************************************************************************
* dalMulticastPortVlanMacDel
*
* DESCRIPTION:
*  		this function used to delete a multicast control rule: port+vlan+mac
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastPortVlanMacDel(UINT8 portNum, UINT16 vlanId, UINT8 *macAddr, OPL_BOOL fuzzy)
{
/* Begin Modified 2011-12-8 */
//#ifndef CTC_MULTICAST_SURPORT 
#if 1
    INT32 index;
    OPL_STATUS retVal = OPL_OK;
#endif            
/* End   Modified 2011-12-8 */

    if (OPL_NULL == macAddr)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
/* Begin Modified 2011-12-8 */
//#ifndef CTC_MULTICAST_SURPORT	
#if 1
    OPL_DAL_PRINTF(("dalMulticastPortVlanMacDel Input Parameters: portNum=%d vlanId=%d macAddr=%02x%02x%02x%02x%02x%02x.\n", 
        portNum, vlanId, macAddr[0], macAddr[1], 
        macAddr[2], macAddr[3], macAddr[4], macAddr[5]));

    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
    	if (((fuzzy && (!OPL_MEMCMP(groupAddrClsIndexMapTab[index].mac, macAddr, MAC_LENGTH/2))) 
            || (!fuzzy && (!OPL_MEMCMP(groupAddrClsIndexMapTab[index].mac, macAddr, MAC_LENGTH))))
            && (groupAddrClsIndexMapTab[index].vid == vlanId) 
            && (groupAddrClsIndexMapTab[index].valid == OPL_TRUE))
        {
            retVal = dalVoipClsDelEntry(groupAddrClsIndexMapTab[index].clsIndex);
            if (retVal != OPL_OK)
            {
                OPL_DAL_PRINTF(("%s[%d]: dalVoipClsDelEntry Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
                return retVal;
            }        

            OPL_MEMSET(&groupAddrClsIndexMapTab[index], 0x0, sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
            return OPL_OK;
        }
    }

    OPL_DAL_PRINTF(("%s[%d]: Multicast Classify Rule Not Exist.\n", __FUNCTION__, __LINE__));
#else
{
    extern INT32 DRV_DelPortFromMcastAddr(UINT32 uiLPort, UINT32 uiVlan, UINT8 aucMac[6]);
    INT32 ret;
    
    ret = DRV_DelPortFromMcastAddr(portNum, vlanId, macAddr);
    if (0 != ret)
    {
        return OPL_ERROR;
    }
}
#endif
/* End   Modified 2011-12-8 */
    return OPL_OK;
}

/*******************************************************************************
* dalMulticastPortVlanIpAdd
*
* DESCRIPTION:
*  		this function used to add a multicast control rule: port+vlan+ip
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastPortVlanIpAdd(UINT8 portNum, UINT16 vlanId, UINT32 ip)
{
    INT32 index;
    OPL_STATUS retVal = OPL_OK;
    CLS_CONFIG_INFO_t stClassCfg;
    UINT16 ruleId;
    
    OPL_DAL_PRINTF(("dalMulticastPortVlanIpAdd Input Parameters: portNum=%d vlanId=%d ip=%08x.\n", 
        portNum, vlanId, ip));

    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
        if((groupAddrClsIndexMapTab[index].ip == ip) 
            && (groupAddrClsIndexMapTab[index].vid == vlanId) 
            && (groupAddrClsIndexMapTab[index].valid == OPL_TRUE))
        {
            OPL_DAL_PRINTF(("%s[%d]: Multicast Classify Rule Exist.\n", __FUNCTION__, __LINE__));
            return OPL_OK;
        }
    }

    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
        if (groupAddrClsIndexMapTab[index].valid != OPL_TRUE)
        {
            OPL_MEMSET((char *)&stClassCfg, 0, sizeof(stClassCfg));

            /* Valid */
            stClassCfg.valid = 1;

            /* Action */
            stClassCfg.t_act = DONOT_DROP;

            /* SRC Port, not used for single port mode */
#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
            stClassCfg.ports[0] = portNum;
#endif

            /* GDA IP */
            stClassCfg.dstIpFlag = 1;
            stClassCfg.dstIp.lowRange = ip;
            stClassCfg.dstIp.highRange = 0xffffffff;
            
            /* VID */
            stClassCfg.vlanIdFlag = 1;
            stClassCfg.vlanId.lowRange = vlanId;
            stClassCfg.vlanId.highRange = 0xffff;

            /* Type or Length, 0x0800 */
            stClassCfg.lenOrTypeFlag = 1;
            stClassCfg.lenOrType.lowRange = 0x0800;
            stClassCfg.lenOrType.highRange = 0xffff;

            /* IP Type, UDP */
            stClassCfg.ipTypeFlag = 1;
            stClassCfg.ipType.lowRange = 17;
            stClassCfg.ipType.highRange = 0xff;
      
            /* Do not change COS value */
            stClassCfg.cosMapVal = CTC_ETHER_PRI_END;
            stClassCfg.queueMapId = CTC_QUEUE_MAP_END;
            
            retVal = dalVoipClsRuleCtcAdd(&stClassCfg, &ruleId);
            if (retVal != OPL_OK)
            {
                OPL_MEMSET(&groupAddrClsIndexMapTab[index], 0x0, sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
                OPL_DAL_PRINTF(("%s[%d]: dalVoipClsRuleCtcAdd Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
                return retVal;
            }
            
            groupAddrClsIndexMapTab[index].valid = OPL_TRUE;
            groupAddrClsIndexMapTab[index].clsIndex = ruleId;
            groupAddrClsIndexMapTab[index].ip = ip;
            groupAddrClsIndexMapTab[index].vid = vlanId;

            OPL_DAL_PRINTF(("Multicast Classify Rule Added: valid=%d index=%d ip=%08x vlanId=%d\n", 
                groupAddrClsIndexMapTab[index].valid, groupAddrClsIndexMapTab[index].clsIndex, 
                groupAddrClsIndexMapTab[index].ip, groupAddrClsIndexMapTab[index].vid));
            
            return OPL_OK;
        }
    }
    
    if (index == MC_MAX_GROUP_NUM)
    {
        OPL_DAL_PRINTF(("multicast address table is full.\n"));
    }
    
    return OPL_OK;
}

/*******************************************************************************
* dalMulticastPortVlanMacDel
*
* DESCRIPTION:
*  		this function used to delete a multicast control rule: port+vlan+ip
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastPortVlanIpDel(UINT8 portNum, UINT16 vlanId, UINT32 ip)
{
    INT32 index;
    OPL_STATUS retVal = OPL_OK;
	
    OPL_DAL_PRINTF(("dalMulticastPortVlanIpDel Input Parameters: portNum=%d vlanId=%d ip=%08x.\n", 
        portNum, vlanId, ip));

    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
    	if ((groupAddrClsIndexMapTab[index].ip == ip) 
            && (groupAddrClsIndexMapTab[index].vid == vlanId) 
            && (groupAddrClsIndexMapTab[index].valid == OPL_TRUE))
        {
            retVal = dalVoipClsDelEntry(groupAddrClsIndexMapTab[index].clsIndex);
            if (retVal != OPL_OK)
            {
                OPL_DAL_PRINTF(("%s[%d]: dalVoipClsDelEntry Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
                return retVal;
            }  

            OPL_MEMSET(&groupAddrClsIndexMapTab[index], 0x0, sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
            return OPL_OK;
        }
    }

    OPL_DAL_PRINTF(("%s[%d]: Multicast Classify Rule Not Exist.\n", __FUNCTION__, __LINE__));

    return OPL_OK;
}

/*******************************************************************************
* dalMulticastRuleDelAll
*
* DESCRIPTION:
*  		this function used to delete all multicast control rule
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastRuleDelAll(void)
{
    INT32 index;
    OPL_STATUS retVal = OPL_OK;
    
    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
    	if (groupAddrClsIndexMapTab[index].valid == OPL_TRUE)
        {
            retVal = dalVoipClsDelEntry(groupAddrClsIndexMapTab[index].clsIndex);
            if (retVal != OPL_OK)
            {
                OPL_DAL_PRINTF(("%s[%d]: dalVoipClsDelEntry Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
                return retVal;
            }  

            OPL_MEMSET(&groupAddrClsIndexMapTab[index], 0x0, sizeof(GROUP_ADDR_CLS_INDEX_MAP_t));
        }
    }

    OPL_DAL_PRINTF(("%s[%d]: Multicast Classify Rule Del All.\n", __FUNCTION__, __LINE__));

    return OPL_OK;
}

#if 0 /* yxzhao added, 2010-01-06, 16:17:35 */
/*******************************************************************************
* dalMulticastIgmpControlAdd
*
* DESCRIPTION:
*  		this function used to enable sending igmp packets to cpu.
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastIgmpControlAdd(void)
{
    INT32 index;
    OPL_STATUS retVal = OPL_OK;
    
    
    CLS_CONFIG_INFO_t stClassCfg;
    OPL_MEMSET((char *)&stClassCfg, 0, sizeof(stClassCfg));

    /* Valid */
    stClassCfg.valid = 1;

    /* Action */
    stClassCfg.t_act = COPY_PKTS_TO_CPU | DROP_PKTS;

    /* GDA MAC, 01:00:5e:xx:xx:xx */
    stClassCfg.dstMacFlag = 1;
    stClassCfg.dstMac.lowRange[0] = 0x01;
    stClassCfg.dstMac.lowRange[1] = 0x00;
    stClassCfg.dstMac.lowRange[2] = 0x5e;
    stClassCfg.dstMac.lowRange[3] = 0x00;
    stClassCfg.dstMac.lowRange[4] = 0x00;
    stClassCfg.dstMac.lowRange[5] = 0x00;
    stClassCfg.dstMac.highRange[0] = 0xff;
    stClassCfg.dstMac.highRange[1] = 0xff;
    stClassCfg.dstMac.highRange[2] = 0xff;
    stClassCfg.dstMac.highRange[3] = 0x0;
    stClassCfg.dstMac.highRange[4] = 0x0;
    stClassCfg.dstMac.highRange[5] = 0x0;

    /* Type or Length, 0x0800 */
    stClassCfg.lenOrTypeFlag = 1;
    stClassCfg.lenOrType.lowRange = 0x0800;
    stClassCfg.lenOrType.highRange = 0xffff;

    /* IP Type, IGMP */
    stClassCfg.ipTypeFlag = 1;
    stClassCfg.ipType.lowRange = 2;
    stClassCfg.ipType.highRange = 0xff;

    retVal = dalVoipClsRuleCtcAdd(&stClassCfg, &mcIgmpControlRuleId);
    if (retVal != OPL_OK)
    {
        OPL_DAL_PRINTF(("%s[%d]: dalVoipClsRuleCtcAdd Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
        return retVal;
    }
    
    OPL_DAL_PRINTF(("dalMulticastIgmpControlAdd succeed, mcDataFilterRuleId=%d.\r\n", mcIgmpControlRuleId)); 
    
    return OPL_OK;
}

/*******************************************************************************
* dalMulticastIgmpControlDel
*
* DESCRIPTION:
*  		this function used to disable sending igmp packets to cpu.
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastIgmpControlDel(void)
{
    INT32 index;
    OPL_STATUS retVal = OPL_OK;

    retVal = dalVoipClsDelEntry(mcIgmpControlRuleId);
    if (retVal != OPL_OK)
    {
        OPL_DAL_PRINTF(("%s[%d]: dalVoipClsDelEntry Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
        return retVal;
    }

    OPL_DAL_PRINTF(("dalMulticastIgmpControlDel succeed, mcDataFilterRuleId=%d.\r\n", mcIgmpControlRuleId)); 
    
    return OPL_OK;
}
#endif

/*******************************************************************************
* dalMulticastDataFilterAdd
*
* DESCRIPTION:
*  		this function used add a multicast control rule: port+vlan+mac
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastDataFilterAdd(void)
{

    OPL_STATUS retVal = OPL_OK;
    CLS_CONFIG_INFO_t stClassCfg;

    if (mcDataFilterRuleId != 0xFFFF)
    {
        OPL_DAL_PRINTF(("dalMulticastDataFilterAdd, already exist.\r\n"));
        return OPL_OK;
    }
    
    OPL_MEMSET((char *)&stClassCfg, 0, sizeof(stClassCfg));
    
    /* Valid */
    stClassCfg.valid = 1;

    /* Action */
    stClassCfg.t_act = DROP_PKTS;

    /* GDA MAC, 01:00:5e:xx:xx:xx */
    stClassCfg.dstMacFlag = 1;
    stClassCfg.dstMac.lowRange[0] = 0x01;
    stClassCfg.dstMac.lowRange[1] = 0x00;
    stClassCfg.dstMac.lowRange[2] = 0x5e;
    stClassCfg.dstMac.lowRange[3] = 0x00;
    stClassCfg.dstMac.lowRange[4] = 0x00;
    stClassCfg.dstMac.lowRange[5] = 0x00;
    stClassCfg.dstMac.highRange[0] = 0xff;
    stClassCfg.dstMac.highRange[1] = 0xff;
    stClassCfg.dstMac.highRange[2] = 0xff;
    stClassCfg.dstMac.highRange[3] = 0x0;
    stClassCfg.dstMac.highRange[4] = 0x0;
    stClassCfg.dstMac.highRange[5] = 0x0;

    /* Type or Length, 0x0800 */
    stClassCfg.lenOrTypeFlag = 1;
    stClassCfg.lenOrType.lowRange = 0x0800;
    stClassCfg.lenOrType.highRange = 0xffff;

    /* IP Type, UDP */
    stClassCfg.ipTypeFlag = 1;
    stClassCfg.ipType.lowRange = 17;
    stClassCfg.ipType.highRange = 0xff;
	
    /* Do not change COS value */
    stClassCfg.cosMapVal = CTC_ETHER_PRI_END;
    stClassCfg.queueMapId = CTC_QUEUE_MAP_END;

    retVal = dalVoipClsRuleCtcAdd(&stClassCfg, &mcDataFilterRuleId);
    if (retVal != OPL_OK)
    {
        OPL_DAL_PRINTF(("%s[%d]: dalVoipClsRuleCtcAdd Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
        return retVal;
    }            
    
    OPL_DAL_PRINTF(("dalMulticastDataFilterAdd succeed, mcDataFilterRuleId=0x%x.\r\n", mcDataFilterRuleId)); 
    
    return OPL_OK;
}

/*******************************************************************************
* dalMulticastDataFilterDel
*
* DESCRIPTION:
*  		this function used add a multicast control rule: port+vlan+mac
*
*	INPUTS:
*			portNum:
*			vlanId:
*			macAddr:
*
*	OUTPUTS:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
OPL_STATUS dalMulticastDataFilterDel(void)
{

    OPL_STATUS retVal = OPL_OK;

    if (mcDataFilterRuleId == 0xFFFF)
    {
        OPL_DAL_PRINTF(("dalMulticastDataFilterDel, not exist.\r\n"));
        return OPL_OK;
    }

    retVal = dalVoipClsDelEntry(mcDataFilterRuleId);
    if (retVal != OPL_OK)
    {
        OPL_DAL_PRINTF(("%s[%d]: dalVoipClsDelEntry Fail, retVal=%d.\n", __FUNCTION__, __LINE__, retVal));
        return retVal;
    }

    OPL_DAL_PRINTF(("dalMulticastDataFilterDel succeed, mcDataFilterRuleId=0x%x.\r\n", mcDataFilterRuleId));

    mcDataFilterRuleId = 0xFFFF;
  
    return OPL_OK;
}

/*******************************************************************************
* dalMulticastRuleNumGet
*
* DESCRIPTION:
*  		this function gets the number of used multicast control rule
*
*	INPUTS:
*
*	OUTPUTS:
*                   numOfRule:
*
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*/
/* 014547 */
OPL_STATUS dalMulticastNumofRuleGet(UINT16 *numOfRule)
{
    INT32 index;
   
    UINT16 number = 0;
/* 014547 */
    for (index = 0; index < MC_MAX_GROUP_NUM; index++)
    {
        if (groupAddrClsIndexMapTab[index].valid == OPL_TRUE)
        {
            number++;
        }
    }

    *numOfRule = number;
    
    return OPL_OK;
}
