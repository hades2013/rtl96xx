/*
=============================================================================
     File Name: hal_rstp.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 jiangmingli	   2009/03/17		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#include <vos.h>
#include "hal.h"
//wfxu #include <dal_rstp.h>

#include "hal_rstp.h"
#include "opconn_usr_ioctrl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include "hal_multicast.h"
#include "rstp_def.h"
#include "oam.h"
#include "lw_config.h"

extern BOOL_T  g_bStpEnable;


/*******************************************************************************
* halRstpModeSet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halRstpModeSet(IN VOID *pPara)
{
    OPL_BOOL bEnable;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    bEnable = *(OPL_BOOL *)pPara;

    dalRstpModeSet(bEnable);

    return;
}

/*******************************************************************************
* halRstpPortStateSet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halRstpPortStateSet(IN VOID *pPara)
{
    HAL_API_RSTP_PORT_STATE_SET_S *pstPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_RSTP_PORT_STATE_SET_S *)pPara;

    dalRstpPortStateSet(pstPara->ulPortId, pstPara->ucPortState);

    return;
}

/*******************************************************************************
* halRstpPortStateGet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halRstpPortStateGet(IN VOID *pPara)
{
    HAL_API_RSTP_PORT_STATE_GET_S *pstPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_RSTP_PORT_STATE_GET_S *)pPara;

    dalRstpPortStateGet(pstPara->ulPortId, pstPara->pucPortState);

    return;
}

/*******************************************************************************
* halRstpPortMacFlush
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halRstpPortMacFlush(IN VOID *pPara)
{
    HAL_API_RSTP_PORT_MAC_FLUSH_S *pstPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_RSTP_PORT_MAC_FLUSH_S *)pPara;

    dalRstpPortMacFlush(pstPara->ulPortid, pstPara->bOnlyThisPort);

    return;
}


/*******************************************************************************
* halRstpInit
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
OPL_RESULT halRstpInit(VOID)
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet;

    /* register rstp api */
    stApiUnit.apiId = HAL_API_RSTP_MODE_SET;
    stApiUnit.apiFunc = halRstpModeSet;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_RSTP_PORT_STATE_SET;
    stApiUnit.apiFunc = halRstpPortStateSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_RSTP_PORT_STATE_GET;
    stApiUnit.apiFunc = halRstpPortStateGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_RSTP_PORT_MAC_FLUSH;
    stApiUnit.apiFunc = halRstpPortMacFlush;
    ulRet += halDrvApiRegister(&stApiUnit);

    return ulRet;
}



/*****************************************************************************
                                                
*****************************************************************************/
ULONG CFG_GetStpEnable(ULONG *pulEnable)
{
   

    *pulEnable = g_bStpEnable;

    return NO_ERROR;
}

/*****************************************************************************
                                                                      
*****************************************************************************/
ULONG CFG_SetStpEnable(ULONG ulEnable)
{
 

    g_bStpEnable = ulEnable;

    return NO_ERROR;
}

/*****************************************************************************
                                                                
*****************************************************************************/
INT32 DRV_SetBpduMac(UINT8 ucEnable)
{    
	UINT32 priority; 
	UINT8 ucMacCtrl = 0;
	rsv_mac_pass_action_t action;

	/*01-80-C2-00-00-00*/
	if (ENABLE == ucEnable)
	{
		action = PASS_TYPE_CPU_ONLY;
		priority = FRAME_TRAP_PRIORITY_7;

		ucMacCtrl = 1; /*Send to cpu*/
	}
	else
	{
		action = PASS_TYPE_ALLPORTS;
		priority = FRAME_TRAP_PRIORITY_0;		

		ucMacCtrl= 2; /*Forward*/
	}

	(VOID)brgRsvMacCtrlHwWrite(0, 1, ucMacCtrl);

	if (DRV_OK != Ioctl_SetReservedMacTrap(0, priority, action))
	{
	    return ERROR;
	}

    return NO_ERROR;   
}

/*****************************************************************************
                                                           
*****************************************************************************/
INT32 DRV_GetPortStpState(UINT32 uiLport, UINT32 *puiStpState)
{
    PORT_STP_STATE_E Stp_State = PORT_STATE_END;

  
    
    if (DRV_OK != Ioctl_GetStpPortState(uiLport, &Stp_State) )
    {
        return ERROR;
    }

    switch (Stp_State )
    {
        case PORT_STATE_DISABLED:
            *puiStpState = PORT_RSTP_DISABLE;
            break;
        case PORT_STATE_BLOCKING:
            *puiStpState = PORT_RSTP_DISCARDING;
            break;
        case PORT_STATE_LEARNING:
            *puiStpState = PORT_RSTP_LEARNING;
            break;
        case PORT_STATE_FORWARDING:
            *puiStpState = PORT_RSTP_FORWARDING;
            break;
        default:
            return ERROR;
    }   

    return NO_ERROR;
}

/*****************************************************************************
                                                              
*****************************************************************************/
UINT32 DRV_SetPortState(UINT32 uiPortIndex, UINT32 uiState)
{
	PORT_STP_STATE_E Stp_State = PORT_STATE_END;
	
  

	switch (uiState )
    {
        case PORT_RSTP_DISABLE:
            Stp_State = PORT_STATE_DISABLED;
            break;
        case PORT_RSTP_DISCARDING:
            Stp_State = PORT_STATE_BLOCKING;
            break;
        case PORT_RSTP_LEARNING:
            Stp_State = PORT_STATE_LEARNING;
            break;
        case PORT_RSTP_FORWARDING:
            Stp_State = PORT_STATE_FORWARDING;
            break;
        default:
            return ERROR;
    }   

    if (DRV_OK != Ioctl_SetStpPortState(uiPortIndex, Stp_State))
    {
        return ERROR;
    }
    
    return NO_ERROR;
}

/*****************************************************************************
                                                                    
*****************************************************************************/
UINT32 DRV_ConvertPortLgc2Stp(UINT32 uiLPort)
{
    if (!IS_VALID_UNI_PORT(uiLPort))
    {
        return IVALID_PORT_ID;
    }
    return uiLPort - 1;
}

/*****************************************************************************
                                                                  
*****************************************************************************/
ULONG Drv_ConvertPort(ULONG ulPortIndex)
{
    /* 
     * ulPortIndex从0开始计数。
    */
    return ulPortIndex + 1;
}


/*****************************************************************************
                                                                     
*****************************************************************************/
ULONG Drv_RSTPSetPortState(ULONG ulPortIndex, USHORT enState)
{
    UINT32 uiState = PORT_RSTP_BUTT;
    UINT32 uiRet = 0;
    
    if (ulPortIndex >= RSTP_MAX_PORT_NUM_LOCAL)
    {
        return VOS_ERR; 
    }

    switch(enState)
    {
        case PORT_RSTP_DISABLE:
            uiState = PORT_RSTP_DISABLE;
            break;
        case PORT_RSTP_DISCARDING:
            uiState = PORT_RSTP_DISCARDING;
            break;
        case PORT_RSTP_LEARNING:
            uiState = PORT_RSTP_LEARNING;
            break;
        case PORT_RSTP_FORWARDING:
            uiState = PORT_RSTP_FORWARDING;
            break;
        default:
            return VOS_ERR;            
    }
    
    uiRet = DRV_SetPortState((UINT32)Drv_ConvertPort(ulPortIndex), uiState);
    if (uiRet != NO_ERROR)
    {
        return VOS_ERR;
    }
        
    return VOS_OK;
}

/*****************************************************************************
                                                                    
*****************************************************************************/
ULONG DRV_MacDeleteEntry (ULONG ulPortIndex)
{
	mac_delete_t stMacDel;

	memset(&stMacDel, 0, sizeof(stMacDel));
	stMacDel.flushByPort = ENABLE;
	stMacDel.port = Drv_ConvertPort(ulPortIndex);

    if (DRV_OK != Ioctl_SetMacFlushUcastMac(stMacDel))
    {
        return VOS_ERR;
	}
	
    return VOS_OK;
}

/*****************************************************************************
                                                                
*****************************************************************************/
ULONG DRV_GetUniLinkStatus(ULONG ulLPort, PORT_STATUS_E *penStatus)
{
    UINT32 link_up = FALSE;
    INT32 iRet = 0;

  

	if (DRV_OK != Ioctl_GetPortCurrentLink(ulLPort,&link_up))
    {
        return VOS_ERR;
    }

    if (TRUE == link_up)
    {
        *penStatus = STATUS_UP;
    }
    else
    {
        *penStatus = STATUS_DOWN;
    }
    return VOS_OK;
}

/*****************************************************************************
                                                                    
*****************************************************************************/
ULONG DRV_GetUniSpeedMode(ULONG ulLPort, UNI_SPEED_MODE_E *penMode)
{
    UINT32 uiPortSpeed = 0;
    
  
    
    if (DRV_OK != Ioctl_GetPortSpeed(ulLPort,&uiPortSpeed))
    {
        return VOS_ERR;
    }

	switch(uiPortSpeed)
    {
        case PORT_SPEED_10:
            *penMode = SPEED_MODE_10M;
            break;
        case PORT_SPEED_100:
            *penMode = SPEED_MODE_100M;
            break;
        case PORT_SPEED_1000:
            *penMode = SPEED_MODE_1000M;
            break;
        default:
            return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
                                                                     
*****************************************************************************/
ULONG DRV_GetUniDuplexMode(ULONG ulLPort, UNI_DUPLEX_MODE_E *penMode)
{
    INT32 iPortDuplex = 0;
    


    if (DRV_OK != Ioctl_GetPortDuplex(ulLPort,&iPortDuplex))
    {
        return VOS_ERR;
    }

	switch(iPortDuplex)
    {
        case PORT_DUPLEX_FULL:
            *penMode = DUPLEX_MODE_FULL;
            break;
        case PORT_DUPLEX_HALF:
            *penMode = DUPLEX_MODE_HALF;
            break;
        default:
            return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
                                                                    
*****************************************************************************/
ULONG DRV_RSTPEnable(ULONG ulEnable)
{
    ULONG     ulSubPortNo = 0;
    ULONG     ulRet = VOS_ERR;
    
    


    ulEnable = (RSTP_ENABLE == ulEnable) ? ENABLE : DISABLE;

	ulRet = (UINT32)DRV_SetBpduMac(ulEnable);
    if (ulRet != NO_ERROR)
    {
        DRV_DEBUG_PRINT("DRV_RSTPEnable: Failed to call DRV_SetStpMode. Ret:%d\r\n", ulRet);
        return VOS_ERR;
    }
    
    /* 利用DISABLE状态，将所有端口的队列buffer清除一次 */
    for (ulSubPortNo = 0; ulSubPortNo < RSTP_MAX_PORT_NUM_LOCAL; ulSubPortNo++)
    {
        ulRet = (ULONG)DRV_SetPortState(Drv_ConvertPort(ulSubPortNo), PORT_STATE_DISABLED);        
        if (ulRet != NO_ERROR)
        {
            return VOS_ERR;
        }
    }

    return VOS_OK;
}

/*****************************************************************************
                                                                     
*****************************************************************************/
ULONG Drv_GetBridgeMacAddress( UCHAR ucEth_Addr[MAC_ADDR_LEN])
{
    vosHWAddrGet("eth0", ucEth_Addr);
    
    return VOS_OK;    
}

/*****************************************************************************
                                                                    
*****************************************************************************/
ULONG DRV_GetSubPortStpHwStatus(PORT_RSTP_STATE_E aenStpStatus[RSTP_MAX_PORT_NUM] ) 
{
    INT32 iRet = 0;
    UINT32 uiLport = 0;
    UINT32 uiStpState = 0;
    UINT32 uiTmpPort = 0;

    for ( uiLport = MIN_LOGIC_UNI_ID; uiLport <= MAX_LOGIC_PORT_NUM; uiLport++ )
    {
        uiTmpPort = DRV_ConvertPortLgc2Stp(uiLport);
        if (uiTmpPort >= RSTP_MAX_PORT_NUM)
        {
            return VOS_OK;
        }
        
        iRet = DRV_GetPortStpState(uiLport, &uiStpState);
        if (iRet != NO_ERROR)
        {
            return VOS_ERR;
        }
        aenStpStatus[uiTmpPort] = (PORT_RSTP_STATE_E)uiStpState;
    }
    
    return VOS_OK;    
}

/*****************************************************************************

*****************************************************************************/
ULONG DRV_GetSubPortStpProStatus ( PORT_RSTP_STATE_E aenStpStatus[RSTP_MAX_PORT_NUM] )
{    
    ULONG  ulPortIndex = 0;
    ULONG  ulPortRole = 0; 
    
    /*因为编译问题，临时申明*/
    #define RSTP_ROLE_DISABLEDPORT      0 /* 端口不可用 */
    

    
    for (ulPortIndex = 0; ulPortIndex < RSTP_MAX_PORT_NUM ; ulPortIndex++)
    {
        /* 获取端口状态 */
        aenStpStatus[ulPortIndex] = (PORT_RSTP_STATE_E)RSTP_Comm_GetPortStatus(ulPortIndex);
        
        /* 获取端口角色 */
        ulPortRole = RSTP_Comm_GetPortRole( ulPortIndex);

        /* 为了规避显示问题，DISABLE端口统一显示为DISABLE状态 */
        if (RSTP_ROLE_DISABLEDPORT == ulPortRole)
        {
            aenStpStatus[ulPortIndex] = PORT_RSTP_DISABLE;
        }
    }
    
    return VOS_OK;
}

/*****************************************************************************
                                                               
*****************************************************************************/
UINT32 DRV_GetVlanIdAndPri(UINT32 uiVlanTag, UINT32 *puiVlanId, UINT32 *puiPri)
{
    H3C_ASSERT(puiVlanId != NULL);
    H3C_ASSERT(puiPri != NULL);

    *puiVlanId = uiVlanTag & 0x00000FFF;
    *puiPri    = (uiVlanTag >> 13) & 0x000007;
    
    return NO_ERROR;
}

/*****************************************************************************
 
*****************************************************************************/
ULONG Drv_SetStpDebugEnable(ULONG ulEnable)
{
 
    
    if ( ENABLE ==  ulEnable)
    {
        /* on */
        RSTP_Comm_SetDebugInfo(RSTP_DEBUG_ERROR,VRP_YES);
        RSTP_Comm_SetDebugInfo(RSTP_DEBUG_EVENT,VRP_YES);
    }
    else
    {
        /* off */
        RSTP_Comm_SetDebugInfo(RSTP_DEBUG_ERROR,VRP_NO);
        RSTP_Comm_SetDebugInfo(RSTP_DEBUG_EVENT,VRP_NO);
    }
    return VOS_OK;
}


/*****************************************************************************
                                                                 
*****************************************************************************/
ULONG DRV_PortTransmitFrame(UCHAR *pPacket, USHORT usLength, USHORT usPort)
{
	INT16 sRet = 0;
	BOOL_T bIsBpdu	= FALSE;
	PORT_RSTP_STATE_E aenStpStatus[RSTP_MAX_PORT_NUM] = {PORT_RSTP_BUTT};
	UINT8 aucBpduMac[MAC_ADDR_LEN]     = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x00}; /* BDPU MAC地址 */
	
	/* 参数合法性检查 */
	
	/* 对于非BPDU报文，判断端口的STP状态 */
	if ((ENABLE == g_bStpEnable) && (FALSE == bIsBpdu))
	{
#if 0 
		(VOID)DRV_GetSubPortStpHwStatus(aenStpStatus);
#else
		(VOID)DRV_GetSubPortStpProStatus(aenStpStatus);
#endif
	}

	if (LOGIC_PON_PORT == usPort)
	{
		UINT8 oamStatus = 0;
		OamStatusGet(&oamStatus);
		
		/* OAM is not established  */
		if (EX_DISCOVERY_SEND_ANY == oamStatus) 		
		{
        #if 0
			if ((BOOL_FALSE == g_bStpEnable) 
				|| (BOOL_TRUE == bIsBpdu) 
				|| (PORT_RSTP_FORWARDING == aenStpStatus[DRV_ConvertPortPhy2Stp((UINT32)usPort)]))
        #endif 
			{				
				sRet = eopl_send_to_uplink(0, (VOID *)pPacket, usLength);
				if (NO_ERROR != sRet)
				{
					DRV_DEBUG_PRINT("DRV_PortTransmitFrame: Failed to call eopl_send_to_uplink. Ret: %d\r\n", sRet);
					return VOS_ERR;
				}
			}
		}

		return VOS_OK;
	}

	if(!memcmp(&aucBpduMac[0], pPacket ,MAC_ADDR_LEN))
	{
	    bIsBpdu = TRUE;
	}


	if ((LOGIC_PON_PORT+1 == usPort) || (DISABLE == g_bStpEnable) || (TRUE == bIsBpdu) || (PORT_RSTP_FORWARDING == aenStpStatus[usPort-1]))
	{
		sRet = eopl_send_to_down_link(usPort, 0, 0, (VOID *)pPacket, usLength);
		if (NO_ERROR != sRet)
		{
			DRV_DEBUG_PRINT("DRV_PortTransmitFrame: Failed to call eopl_send_to_down_link. Ret: %d\r\n", sRet);
			return VOS_ERR;
		}
	}	

	return VOS_OK;
}



/*****************************************************************************
 -----------------------------------------------------------------
                                                                              
*****************************************************************************/
ULONG DRV_Transmit(ULONG ulPortIndex, VOID *pPacket, ULONG ulLength)
{
    ULONG ulRet = VOS_OK;   
    UINT32 uiPPort = 0;
    /*UCHAR aucTxBuf[PACKET_BUFFER_SIZE];*/
    UCHAR *pucTxData = NULL;
    ULONG ulTmpLen = 0;
    UCHAR aucStpLLCHead[STP_LLC_LEN]   = {STP_LLC_DSCP, STP_LLC_SSCP, STP_LLC_CONTROL};
    UCHAR aucBpduMac[MAC_ADDR_LEN]     = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x00}; /* BDPU MAC地址 */

    UCHAR aucSrcMac[MAC_ADDR_LEN] = {0x00, 0x0f, 0xe2, 0x12, 0x34, 0x56}; /* BDPU报文的源mac地址 */
  
    
    /* 参数合法性检查 */
   
	if (ulLength > PACKET_BUFFER_SIZE - DRV_BPDU_HEADER_LEN)
    {
        return VOS_ERR;
    }

    
    /* 如果STP没有使能，或者shutdown的时候往PON口发，就不发送STP消息报文,注意只有C模式会往PON口发 */
    if (DISABLE == g_bStpEnable)
    {
        return VOS_OK;
    }

    /* 计算报文总长度 */
    ulTmpLen = (ulLength + DRV_BPDU_HEADER_LEN > MIN_PACKET_SIZE)? ulLength + DRV_BPDU_HEADER_LEN : MIN_PACKET_SIZE;
    
    /* 预留RTL首部的空间 */
    pucTxData = (UCHAR *)vosAlloc(ulTmpLen);
    if (NULL == pucTxData)
    {
        DRV_DEBUG_PRINT("DRV_Transmit : failed to alloc buffer!\n");
        return VOS_ERR;
    }
    memset(pucTxData, 0, sizeof(ulTmpLen));

    /*逻辑端口号*/
    uiPPort = Drv_ConvertPort(ulPortIndex); 

    /* 填充以太网头部 */
    memcpy(pucTxData, aucBpduMac, MAC_ADDR_LEN);
    memcpy(pucTxData + MAC_ADDR_LEN, aucSrcMac, MAC_ADDR_LEN);

    /* 填充LLC头 */
    memcpy(pucTxData + STP_LLC_OFF, aucStpLLCHead, STP_LLC_LEN);
    
    /* 填充BPDU数据 */
    memcpy(pucTxData + BPDU_DATA_OFF, pPacket, ulLength);

    /* 填充报文长度 */
   
    *(USHORT *)(pucTxData + E802_PACKETLEN_OFF) = (ulLength + STP_LLC_LEN);
 
     
    /* 打印报文,用于调试 */
    ulRet = DRV_PortTransmitFrame(pucTxData, (USHORT)ulTmpLen, (USHORT)uiPPort);

    vosFree(pucTxData);

    return ulRet;

}


/*****************************************************************************
                                                                   
*****************************************************************************/
ULONG Drv_SetStpEnable(ULONG ulEnable)
{
#if defined(CONFIG_PRODUCT_EPN200)
    if (ENABLE == ulEnable)
    {
        return VOS_ERR;
    }
    else
    {
        return VOS_OK;
    }
#else
    ULONG  ulStpEnableCfg = 0;
    ULONG  ulRet = VOS_ERR;

    /* 输入参数合法性检查*/
    H3C_ASSERT((ENABLE == ulEnable) || (DISABLE == ulEnable));

    (VOID)CFG_GetStpEnable(&ulStpEnableCfg);

    /* Check current status */
    if (ulEnable == ulStpEnableCfg)
    {
        return VOS_OK;
    }
    if (ENABLE == ulEnable)
    { 
        /* 启动STP */
        ulRet = RSTP_Enable(RSTP_ENABLE);
        if (VOS_OK != ulRet)
        {
            DRV_DEBUG_PRINT("Drv_SetStpEnable: Call RSTP_Enable failed. Ret:%d\r\n", ulRet);
            return VOS_ERR;
        }
    }
    else
    {
        /* 关闭STP */
        ulRet = RSTP_Enable(RSTP_DISABLE);
        if (VOS_OK != ulRet)
        {
            DRV_DEBUG_PRINT("Drv_SetStpEnable: Call RSTP_Enable failed. Ret:%d\r\n", ulRet);
            return VOS_ERR;
        } 
    } 
    
    (VOID)CFG_SetStpEnable(ulEnable);

    return VOS_OK;
#endif
}

