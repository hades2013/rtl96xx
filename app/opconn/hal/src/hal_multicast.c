/*
=============================================================================
     File Name: hal_multicast.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 xfan    2009/03/17   Initial Version
----------------   ------------  ----------------------------------------------
*/

//#include "opl_driver.h"
//#include <vos.h>
//#include "errors.h"
//#include "hal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include "hal_multicast.h"

#if 0
OPL_RESULT halMulticastInit(void *pInput)
{
    HAL_MC_CFG_INFO_t *p = (HAL_MC_CFG_INFO_t *)pInput;

    if(NULL == p)
    {
        HAL_MC_PRINTF(("input null pointer.\n"));
        return OPL_ERROR;
    }

    return dalMulticastInit();
}

OPL_RESULT halMulticastEnable(void *pInput)
{
    HAL_MC_CFG_INFO_t *p = (HAL_MC_CFG_INFO_t *)pInput;

    if(NULL == p)
    {
        HAL_MC_PRINTF(("input null pointer.\n"));
        return OPL_ERROR;
    }

    return dalMulticastEnable();
}

OPL_RESULT halMulticastDisable(void *pInput)
{
    HAL_MC_CFG_INFO_t *p = (HAL_MC_CFG_INFO_t *)pInput;

    if(NULL == p)
    {
        HAL_MC_PRINTF(("input null pointer.\n"));
        return OPL_ERROR;
    }

    return dalMulticastDisable();
}

OPL_RESULT halMulticastArlAddPort(void *pInput)
{
    HAL_MC_CFG_INFO_t *p = (HAL_MC_CFG_INFO_t *)pInput;

    if(NULL == p)
    {
        HAL_MC_PRINTF(("input null pointer.\n"));
        return OPL_ERROR;
    }

    return dalMulticastArlAddPort(p->port, p->mac);
}

OPL_RESULT halMulticastArlPortDel(void *pInput)
{
    HAL_MC_CFG_INFO_t *p = (HAL_MC_CFG_INFO_t *)pInput;

    if(NULL == p)
    {
        HAL_MC_PRINTF(("input null pointer.\n"));
        return OPL_ERROR;
    }

    return dalMulticastArlPortDel(p->port, p->mac);
}
#endif

UINT halMcRegister(void)
{
#if 0
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet = 0;

    stApiUnit.apiId = HAL_API_MCAST_INIT ;
    stApiUnit.apiFunc = (OPL_API_FUNC)halMulticastInit;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_MCAST_ENABLE;
    stApiUnit.apiFunc = (OPL_API_FUNC)halMulticastEnable;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_MCAST_DISABLE;
    stApiUnit.apiFunc = (OPL_API_FUNC)halMulticastDisable;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_MCAST_ARL_ADD;
    stApiUnit.apiFunc = (OPL_API_FUNC)halMulticastArlAddPort;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_MCAST_ARL_DEL;
    stApiUnit.apiFunc = (OPL_API_FUNC)halMulticastArlPortDel;
    ulRet += halDrvApiRegister(&stApiUnit);

    return ulRet;
#endif
    return OK;
}



/* Support multicast feature */
#ifdef CTC_MULTICAST_SURPORT
//#if 0
#if 0

INT32 DRV_GetSwitchUplinkLPort(UINT32 *puiLPort)
{
    if(puiLPort == NULL)
	{
	    return ERROR;
	}
     
    *puiLPort = LOGIC_PON_PORT;

    return NO_ERROR;
}

/*****************************************************************************
  
                                                                              
*****************************************************************************/
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
	Ret = Ioctl_GetMacMcastMac(tdVid, stMacAddress, &stMacMcast);
	
    if (DRV_ERR_MAC_ADDR_NOT_FOUND == Ret)
    {
    	memset(&stMacMcast, 0, sizeof(stMacMcast));
        memcpy(stMacMcast.mac_addr, stMacAddress, sizeof(mac_address_t));
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
		
    Ret = Ioctl_SetMacSetMcastMac(stMacMcast);

    if (DRV_OK != Ret)
    {
        return ERROR;
    }

    return NO_ERROR;
}

/*****************************************************************************
  
*****************************************************************************/
INT32 DRV_DelPortFromMcastAddr(UINT32 uiLPort, UINT32 uiVlan, UINT8 aucMac[MAC_ADDR_LEN])
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
	Ret = Ioctl_GetMacMcastMac(tdVid, stMacAddress, &stMacMcast);
	if(DRV_OK == Ret)
	{
	    /*remove deleting mac portmask*/
	    LgcMaskRemove(&(stMacMcast.port_mask), &stLPortMask);
	    if(FALSE == LgcMaskNotNull(&(stMacMcast.port_mask)))
	    {
	        /*delete*/
	        Ret = Ioctl_SetMacDelMcastMac(stMacMcast);
	        if(DRV_OK == Ret)
	        {
	            return NO_ERROR;
	        }
	    }
	    else
	    {
	        /*reset portmask for give mac*/
	        if (DRV_OK != Ioctl_SetMacSetMcastMac(stMacMcast))
        	{
        	    return ERROR;
        	}
	    }
	}
	else
	{
	    /*not exist*/
	    return ERROR;           
	}

    return NO_ERROR;
}

/*****************************************************************************
 
                                                                              
*****************************************************************************/
STATIC INT32 DRV_CheckVlanForMcMode(UINT32 uiVlanId)
{
    INT32 iRet;
    UINT32 uiLPortIndex;
    CTC_VLAN_CFG_S stVlanMode;

    H3C_ASSERT(VALID_VLAN_ID(uiVlanId));
    
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

/*****************************************************************************
 
                                                                              
*****************************************************************************/
INT32 DRV_AddPort2McastVlan(UINT32 uiLPort, UINT32 uiMcVid, MC_TAG_MODE_E enTagMode)
{
	DRV_RET_E ulRetVal = DRV_OK;
    logic_pmask_t stLMask, stUntagLMsk;

    if ((!VALID_PORT_ID(uiLPort))   ||
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
	ulRetVal = Ioctl_GetVlanMember(uiMcVid, &stLMask, &stUntagLMsk);
	if (DRV_OK != ulRetVal)
    {
        printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
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
				SetLgcMaskBit(LOGIC_CPU_PORT, &stLMask);
				SetLgcMaskBit(LOGIC_PON_PORT, &stLMask);
				ClrLgcMaskBit(uiLPort,&stUntagLMsk);
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
				SetLgcMaskBit(LOGIC_CPU_PORT, &stLMask);
				SetLgcMaskBit(LOGIC_UPPON_PORT, &stLMask);
				SetLgcMaskBit(uiLPort,&stUntagLMsk);
            }
            
            break;
        default :
            /*Do not support mc vlan translation.*/
            return ERROR;
            break;
    }
    
	ulRetVal = Ioctl_SetVlanMemberAdd(uiMcVid, stLMask, stUntagLMsk);
    if (DRV_OK != ulRetVal)
    {
        printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;  
    }
    return NO_ERROR;
}

/*****************************************************************************
  
                                                                              
*****************************************************************************/
INT32 DRV_DelPort2McastVlan(UINT32 uiLPort, UINT32 uiMcVid)
{
	DRV_RET_E ulRetVal = DRV_OK;
    logic_pmask_t stLMask, stUntagLMsk, stTmpMsk;

    if ((!VALID_PORT_ID(uiLPort))   ||
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
	ClrLgcMaskAll(&stUntagLMsk);

	ulRetVal = Ioctl_GetVlanMember(uiMcVid, &stLMask, &stUntagLMsk);
	if (DRV_OK != ulRetVal)
    {
        printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;  
    }

	if(!TstLgcMaskBit(uiLPort, &stLMask))
	{
	    return NO_ERROR;
	}

	ClrLgcMaskBit(uiLPort, &stLMask);
	ClrLgcMaskBit(uiLPort, &stUntagLMsk);

	LgcMaskCopy(&stTmpMsk, &stLMask);
	ClrLgcMaskBit(LOGIC_CPU_PORT, &stTmpMsk);
	ClrLgcMaskBit(LOGIC_UPPON_PORT, &stTmpMsk);
	if(FALSE == LgcMaskNotNull(&stTmpMsk))
	{
		ClrLgcMaskAll(&stLMask);
		ClrLgcMaskAll(&stUntagLMsk);
	}
	
	ulRetVal = Ioctl_SetVlanMemberAdd(uiMcVid, stLMask, stUntagLMsk);
    if (DRV_OK != ulRetVal)
    {
        printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;  
    }
	
    return NO_ERROR;
}

/*****************************************************************************
 
*****************************************************************************/
INT32 DRV_SwitchMcInit(void)
{
    INT32 Ret;
    UINT32 uiLPortId;

    /*Trap igmp protocol packet to cpu.*/
	Ret = Ioctl_SetIgmpPktAction(PASS_TYPE_CPU_ONLY);
    if (DRV_OK != Ret)
    {
        printf("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /*Drop unknown igmp data.*/
	Ret = Ioctl_SetMcastLookupMissDrop(ENABLE);
    if (DRV_OK != Ret)
    {
        printf("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    return NO_ERROR;
}

/*****************************************************************************
  
*****************************************************************************/
INT32 DRV_EnableSwitchMc(INT32 iEnable)
{
    INT32 Ret;
    UINT32 uiLPortId;

    if ((TRUE != iEnable) && (FALSE != iEnable))
    {
        return ERROR;
    }
	printf("DRV_EnableSwitchMc %d\r\n", iEnable);

    if (TRUE == iEnable)
    {
        /*Trap igmp protocol packet to cpu.*/
        Ret = Ioctl_SetIgmpPktAction(PASS_TYPE_CPU_ONLY);
        if (DRV_OK != Ret)
        {
            return ERROR;
        }

        /*Drop unknown igmp data.*/
		Ret = Ioctl_SetMcastLookupMissDrop(ENABLE);
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
        Ret = Ioctl_SetMcastLookupMissDrop(DISABLE);
        if (DRV_OK != Ret)
        {
            return ERROR;
        }
    }

    return NO_ERROR;
}

/*****************************************************************************
  
*****************************************************************************/
INT32 DRV_SendFrame2SwitchPort(UINT32 uiLPort, INT8 *pPacket, UINT32 uiLength)
{
#if 0
    INT16 sRet = 0;
    UINT16 usVlanId = 0;
    UINT32 uiPortIdx = 0;
    BOOL_T bIsBcast = BOOL_FALSE;
    RTL_HEAD_S stRtlHead;
    
    if ((NULL == pPacket)            ||
        (MIN_PACKET_SIZE > uiLength) ||
        (!VALID_PORT_ID(uiLPort)))
    {
        printf("\nfunc:%s Error parameter.\n",__FUNCTION__);
        return ERROR;
    }

    bIsBcast = IS_MAC_BROADCAST(pPacket);

    /* 检查报文，如果VLAN ID为0或者0xFFF，那么表示untag发送 */
    if (( pPacket[12] == 0x81) && ( pPacket[13] == 0))
    {
        usVlanId = ((pPacket[14] & 0xF) << 8) + pPacket[15];
        if((0 == usVlanId) || (0xFFF == usVlanId))
        {
            memmove(pPacket + 12, pPacket + 16, uiLength - 16);
            pPacket[uiLength - 4] = 0;
            pPacket[uiLength - 3] = 0;
            pPacket[uiLength - 2] = 0;
            pPacket[uiLength - 1] = 0;
            uiLength = (uiLength >= MIN_PACKET_SIZE + 4)? uiLength - 4 : MIN_PACKET_SIZE;
        }
    }

	if (LOGIC_PON_PORT == uiLPort)
	{
        sRet = eopl_send_to_uplink(0, (VOID *)pPacket, uiLength);
        if (NO_ERROR != sRet)
        {
            printf("\nfunc:%s failed to call eopl_send_to_uplink.\n",__FUNCTION__);
            return ERROR;
        }

		return NO_ERROR;
	}

    /*rtl head*/
    memset(&stRtlHead, 0, sizeof(stRtlHead));
    
    stRtlHead.usEthType = 0x8899;
    stRtlHead.ucProtocol = 0x4;
    stRtlHead.ucPrioritySelect = 1;
    stRtlHead.ucPriority = 7;

    if (BOOL_TRUE == bIsBcast)  
	{
	    for (uiPortIdx = MIN_UNI_PORT_ID; uiPortIdx <= MAX_UNI_PORT_ID; uiPortIdx++)
    	{
            stRtlHead.usPortNo |= 1 << DRV_PortLogical2Physical(uiPortIdx);
		}
	}
	else
	{
        stRtlHead.usPortNo |= 1 << DRV_PortLogical2Physical(uiLPort); 
	}
    
    memmove(pPacket + RTL_HEAD_OFF + RTL_HEAD_LEN, pPacket + RTL_HEAD_OFF, uiLength - RTL_HEAD_OFF);
    memcpy(pPacket + RTL_HEAD_OFF, &stRtlHead, RTL_HEAD_LEN);

    uiLength += RTL_HEAD_LEN;
    
    //PrintH3cPacket(RXTX_ACT_TRANS_STP, pPacket, uiLength);
    (VOID)syscall(NR_TEST_SETDATA, pPacket, uiLength);
#endif
	return NO_ERROR;
}

UINT32 DRV_GetSwitchHeaderLen(void)
{
    //return sizeof(RTL_HEAD_S);
    return 4;
}

INT32 DRV_GetPortFromSwitchHeader(UINT8 *pucData, UINT32 *puiPPort)
{
#if 0
    RTL_HEAD_S *pstInHeader = NULL;
        
    if ((NULL == pucData) || (NULL == puiPPort))
    {
        return ERROR;
    }
    
    pstInHeader = (RTL_HEAD_S *)pucData;
    *puiPPort = pstInHeader->usPortNo;
#endif
    return NO_ERROR;
}
#endif
#endif




