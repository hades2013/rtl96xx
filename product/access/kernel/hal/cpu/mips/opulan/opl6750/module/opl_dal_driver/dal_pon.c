/*
=============================================================================
     Header Name: dal_pon.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/13		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#include "dal_lib.h"

extern UINT8 oam_src_mac[6];
extern OPCONN_TAB_INFO_t *oplTabAttrInfoGet(void);

UINT8 ctcTripleChurningDebug  = OPL_DISABLE;

/*Begin add by huangmingjian 2013/03/15 for EPN204QID0033*/ 
extern int (* pFunc_oplRegFieldWrite)(unsigned int  regAddr,unsigned short fieldOffset,unsigned short fieldWidth,unsigned int data0);
extern int (* pFunc_oplRegFieldRead)(unsigned int  regAddr,unsigned short fieldOffset,unsigned short fieldWidth,unsigned int *data0);
/*End add by huangmingjian 2013/03/15 for EPN204QID0033*/ 

OPCONN_TAB_INFO_t *dalTabAttrInfoGet(void)
{
    return oplTabAttrInfoGet();   
}

/*******************************************************************************
* dalPonInit
*
* DESCRIPTION:
*  		this function is used to init the default pon config ,such as serdes delay, laser on and of time count
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
OPL_STATUS dalPonInit(void)
{
	OPL_STATUS retVal = OPL_OK;

    ponSerdesReset();

	/*Begin add by huangmingjian 2013/03/15 for EPN204QID0033*/ 
	pFunc_oplRegFieldWrite = oplRegFieldWrite;
	pFunc_oplRegFieldRead = oplRegFieldRead;
	/*End add by huangmingjian 2013/03/15 for EPN204QID0033*/ 

    /* Disable PMAC */
    oplRegWrite(REG_PMAC_CFG_TRANSFER_ENA, 0x0);
    /* LED control by software */
    oplRegFieldWrite(REG_CP_LED_CTRL, 9, 1, 1);
    oplRegFieldWrite(REG_CP_LED_CTRL, 8, 1, 1);
    oplRegFieldWrite(REG_CP_LED_CTRL, 1, 1, 1);
    oplRegFieldWrite(REG_CP_LED_CTRL, 0, 1, 1);

	retVal = ponSerdesDelayHwWrite(8);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	retVal = ponLaserOnCntHwWrite(4);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	retVal = ponLaserOffCntHwWrite(4);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	/* Begin Added, 2011-5-6*/
    retVal = dalponMpcpRttCtrlWrite(OPL_ENABLE, OPL_DISABLE, 0x20, OPL_ENABLE, OPL_DISABLE, 0x20);
    if(OPL_OK != retVal)
	{
		return retVal;
	}
    /* End   Added, 2011-5-6*/
	return dalPonMacIdSet(&oam_src_mac[0]);
}
/*******************************************************************************
* dalPonMacIdSet
*
* DESCRIPTION:
*  		this function is used to set the onu macld, used for mpcp register and oam pkt.
*
*	INPUTS:
*		mac : 
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMacIdSet(UINT8 *mac)
{
	if(OPL_NULL == mac)
	{
		return OPL_ERR_NULL_POINTER;
	}

    OPL_MEMCPY(oam_src_mac,mac,MAC_LENGTH);
    
	return ponMacIdHwWrite(mac);
}

/*******************************************************************************
* dalPonMacIdGet
*
* DESCRIPTION:
*  		this function is used get the llid mac address used for mpcp register.
*
*	INPUTS:
*		mac:
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMacIdGet(UINT8 *mac)
{
	if(OPL_NULL == mac)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}
	return ponMacIdHwRead(mac);
}

/*******************************************************************************
* dalPonPhyFecModeSet
*
* DESCRIPTION:
*  		this function is used to set the fec mode. 0x01 for unknown,0x02 for eanble,0x03 for disable.
*
*	INPUTS:
*		portId: 		portId
*		mode:		0 disable, 1 enable
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonPhyFecModeSet(UINT32 mode)
{
	UINT8 fecEnable;
	OPL_STATUS retVal = OPL_OK;
    
	if(mode == 1)
	{
		fecEnable = OPL_ENABLE;
	}else if(mode == 0)
	{
		fecEnable = OPL_DISABLE;
	}else
	{
		OPL_DAL_PRINTF(("ctc enable is 2, disable is 3.\n"));
		return OPL_ERROR;
	}

    if(OPL_ENABLE == fecEnable)
    {
        //oplRegWrite(REG_MPCP_PKT_OVERHEAD,0x20000f);
		/* fec deregistered by 64bytes traffic */
		oplRegWrite(REG_MPCP_PKT_OVERHEAD, 0x1b000f);		        
    }else
    {
    	if (IS_ONU_RESPIN)
    	{
	        oplRegWrite(REG_MPCP_PKT_OVERHEAD,0xc000f);
		}else{
			oplRegWrite(REG_MPCP_PKT_OVERHEAD,0xf000f);
		}
    }

	retVal = ponFecRxEnHwWrite(fecEnable);
	if(OPL_OK != retVal)
	{
		OPL_DAL_PRINTF(("enable fec rx enable fialed\n"));
		return OPL_ERROR;
	}

	retVal = ponFecTxEnHwWrite(fecEnable);
	if(OPL_OK != retVal)
	{
		OPL_DAL_PRINTF(("enable fec tx enable fialed\n"));
		return OPL_ERROR;
	}

	
	return retVal;
}
/*******************************************************************************
* dalPonPhyFecModeGet
*
* DESCRIPTION:
*  		this function is used to get the fec mode info.0x01 for unknown,0x02 for eanble,0x03 for disable.
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		mode:		0x01:unknown,0x02:enable,0x03:disable
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonPhyFecModeGet(UINT32 *mode)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 fecEnable;
	
	if(OPL_NULL == mode)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}
	retVal = ponFecRxEnHwRead(&fecEnable);

	if(fecEnable)
	{
		*mode = 1;	
	}else 
	{
		*mode = 0;	
	}

	return retVal;
}
/*******************************************************************************
* dalPonPhyTripleChuningModeSet
*
* DESCRIPTION:
*  		this function is used to enable or disable ctc triple chuning.
*
*	INPUTS:
*		mode: 0 for disable while 1 for enable
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonPhyTripleChuningModeSet(UINT32 mode)
{
	UINT8 ctcEn;
	OPL_STATUS retVal;

	ctcEn = mode;
	retVal = ponTripleChuningEnHwWrite(ctcEn);

	return retVal;
}
/*******************************************************************************
* dalPonPhyTripleChuningModeGet
*
* DESCRIPTION:
*  		this function is used to get the ctc triple chuning mode status
*
*	INPUTS:
*		
*	OUTPUTS:
*		mode: 0 for disable while 1 for enable
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonPhyTripleChuningModeGet(UINT32 *mode)
{
	UINT8 ctcEn;
	OPL_STATUS retVal;

	if(OPL_NULL == mode)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	retVal = ponTripleChuningEnHwRead(&ctcEn);

	*mode = ctcEn;
	
	return retVal;
}
/*******************************************************************************
* dalPonPhyAesModeSet
*
* DESCRIPTION:
*  		this function is used to enable or disable aes
*
*	INPUTS:
*		mode: 0 for disable while 1 for enable
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonPhyAesModeSet(UINT32 mode)
{
	UINT8 aesEn;
	OPL_STATUS retVal = OPL_OK;

	aesEn = mode;

	retVal = ponAesEnHwWrite(aesEn);

	return retVal;
}
/*******************************************************************************
* dalPonPhyTripleChuningModeGet
*
* DESCRIPTION:
*  		this function is used to get the aes
*
*	INPUTS:
*		
*	OUTPUTS:
*		mode: 0 for disable while 1 for enable
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonPhyAesModeGet(UINT32 *mode)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 aesEn;

	if(OPL_NULL == mode)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}
	
	retVal = ponAesEnHwRead(&aesEn);

	*mode = aesEn;
	return retVal;
}

/*******************************************************************************
* dalPonLaserOnCntSet
*
* DESCRIPTION:
*  		this function is used to set the laser on count
*
*	INPUTS:
*		count: num of tq laser on used.
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonLaserOnCntSet(UINT32 count)
{
	if(MAX_LASER_ON_TIME < count)
	{
		OPL_DAL_PRINTF(("out of max laser on time range.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	return ponLaserOnCntHwWrite(count);
}

/*******************************************************************************
* dalPonLaserOnCntSet
*
* DESCRIPTION:
*  		this function is used to get the laser on count
*
*	INPUTS:
*		count: buff used to store the value of tq used for laser on
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonLaserOnCntGet(UINT32 *count)
{
	if(OPL_NULL == count)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}
	
	return ponLaserOnCntHwRead(count);
}
/*******************************************************************************
* dalPonLaserOffCntSet
*
* DESCRIPTION:
*  		this function is used to set the laser off count
*       
*	INPUTS:
*		this function is used to set the laser off count
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonLaserOffCntSet(UINT32 count)
{
	if(MAX_LASER_ON_TIME < count)
	{
		OPL_DAL_PRINTF(("out of max laser off time range.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	return ponLaserOffCntHwWrite(count);
}

/*******************************************************************************
* dalPonLaserOnCntSet
*
* DESCRIPTION:
*  		this function is used to set the laser off count
*
*	INPUTS:
*		count: buff used to store the value of tq used for laser off
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonLaserOffCntGet(UINT32 *count)
{
	if(OPL_NULL == count)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}
	
	return ponLaserOffCntHwRead(count);
}

/* Begin Added , 2011-5-6 */
/*****************************************************************************
  函 数 名: ponMpcpRttCtrlRead
  创建日期: 2011年5月5日 
  作    者: x03836
  函数描述: RTT时延寄存器获取
  输入参数: UINT32 *puiRttCompEn      
               UINT32 *puiRttCompSyncEn  
               UINT32 *puiRttCompOffset  
               UINT32 *puiRttCorEn       
               UINT32 *puiRttCorSyncEn   
               UINT32 *puiRttCorOffset   
  输出参数: 无
    返回值: 
    注意点: 
------------------------------------------------------------------------------
    修改历史                                                                  
    日期        姓名             描述                                         
  --------------------------------------------------------------------------
                                                                              
*****************************************************************************/
OPL_STATUS dalponMpcpRttCtrlRead(UINT32 *puiRttCompEn,
                              UINT32 *puiRttCompSyncEn,
                              UINT32 *puiRttCompOffset,
                              UINT32 *puiRttCorEn,
                              UINT32 *puiRttCorSyncEn,
                              UINT32 *puiRttCorOffset)
{
    OPL_STATUS retVal = OPL_OK;
    PON_MPCP_RTT_CTRL_t stPonRttCtl;

    if(NULL == puiRttCompEn
        ||NULL == puiRttCompSyncEn
        ||NULL == puiRttCompOffset
        ||NULL == puiRttCorEn
        ||NULL == puiRttCorSyncEn
        ||NULL == puiRttCorOffset)
    {
        return OPL_ERR_NULL_POINTER;
    }
        
    memset(&stPonRttCtl, 0, sizeof(stPonRttCtl));
    retVal = oplRegRead(REG_MPCP_RTT_CTL, (UINT32 *)&stPonRttCtl);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
    
    *puiRttCompEn = stPonRttCtl.RttCompEn;
    *puiRttCompSyncEn = stPonRttCtl.RttCompSyncEn;
    *puiRttCompOffset = stPonRttCtl.RttCompOffset;
    *puiRttCorEn = stPonRttCtl.RttCorEn;
    *puiRttCorSyncEn = stPonRttCtl.RttCorSyncEn;
    *puiRttCorOffset = stPonRttCtl.RttCorOffset;

	return retVal;
}

/*****************************************************************************
  函 数 名: ponMpcpRttCtrlWrite
  创建日期: 2011年5月5日 
  作    者: x03836
  函数描述: 设置RTT延时设置寄存器
  输入参数: UINT32 uiRttCompEn      
               UINT32 uiRttCompSyncEn  
               UINT32 uiRttCompOffset  
               UINT32 uiRttCorEn       
               UINT32 uiRttCorSyncEn   
               UINT32 uiRttCorOffset   
  输出参数: 无
    返回值: 
    注意点: 
------------------------------------------------------------------------------
    修改历史                                                                  
    日期        姓名             描述                                         
  --------------------------------------------------------------------------
                                                                              
*****************************************************************************/
OPL_STATUS dalponMpcpRttCtrlWrite(UINT32 uiRttCompEn,
                               UINT32 uiRttCompSyncEn,
                               UINT32 uiRttCompOffset,
                               UINT32 uiRttCorEn,
                               UINT32 uiRttCorSyncEn,
                               UINT32 uiRttCorOffset)
{
    OPL_STATUS retVal = OPL_OK;
    PON_MPCP_RTT_CTRL_t stPonRttCtl;

    memset(&stPonRttCtl, 0, sizeof(stPonRttCtl));
    
    stPonRttCtl.RttCompEn = uiRttCompEn;
    stPonRttCtl.RttCompSyncEn = uiRttCompSyncEn;
    stPonRttCtl.RttCompOffset = uiRttCompOffset;
    stPonRttCtl.RttCorEn = uiRttCorEn;
    stPonRttCtl.RttCorSyncEn = uiRttCorSyncEn;
    stPonRttCtl.RttCorOffset = uiRttCorOffset;

    retVal = oplRegWrite(REG_MPCP_RTT_CTL, *((UINT32 *)&stPonRttCtl));
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	return retVal;
}
/* End   Added, 2011-5-6*/


/*******************************************************************************
* dalPonSyncTimeGet
*
* DESCRIPTION:
*  		this function is used get the synctime value learned from olt.
*
*	INPUTS:
*		count: buff used to store the value of tq used for synctime
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonSyncTimeGet(UINT32 *count)
{
	if(OPL_NULL == count)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}
	
	return ponSyncTimeHwRead(count);
}
/*******************************************************************************
* dalPonLlidGet
*
* DESCRIPTION:
*  		this function is used get the llid assigned by olt.
*
*	INPUTS:
*		count: buff used to store the value of tq used for synctime
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonLlidGet(UINT32 *llid)
{
	if(OPL_NULL == llid)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}
	
	return ponLlidHwRead(llid);
}
/*******************************************************************************
* dalPonSerdesDelayCntSet
*
* DESCRIPTION:
*  		this function is used set the serdes dealy count
*
*	INPUTS:
*		count:
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonSerdesDelayCntSet(UINT32 count)
{
	if(MAX_SERDES_DELAY_TIME < count)
	{
		OPL_DAL_PRINTF(("serdesdelay should less than %d\n",count));
		return OPL_ERR_INVALID_PARAMETERS;
	}	

	return ponSerdesDelayHwWrite(count);
}
/*******************************************************************************
* dalPonSerdesDelayCntSet
*
* DESCRIPTION:
*  		this function is used get the serdes delay count
*	INPUTS:
*		count:
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonSerdesDelayCntGet(UINT32 *count)
{
	if(OPL_NULL == count)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	return ponSerdesDelayHwRead(count);
}
/*******************************************************************************
* changbits
*
* DESCRIPTION:
*  		this function is used change bit like other venders
*	INPUTS:
*		count:
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
void changbits(UINT8 *bitstring)
{
	UINT8 firstByte;
	UINT16 tmp1;
	UINT16 tmp2;

	firstByte = *bitstring;
	tmp1 = (*(UINT16 *)&bitstring[1]);
	*bitstring = ((firstByte&0x80)>>7)|((firstByte&0x40)>>5)|((firstByte&0x20)>>3)|((firstByte&0x10)>>1)|((firstByte&0x01)<<7)|((firstByte&0x02)<<5)|((firstByte&0x04)<<3)|((firstByte&0x08)<<1);
	tmp2 = ((tmp1&0xff) <<8)|((tmp1&0xff00)>>8);
	OPL_MEMCPY(&bitstring[1],&tmp2,2);
}

OPL_STATUS dalPonSecurityModeSet(UINT32 channel,UINT32 mode)
{
    UINT32 secChannel;
     
    switch(channel)
    {
        case PON_SECU_CHANNEL_SCB:
            secChannel = PON_SECU_CHANNEL_SCB;
            break;
        case PON_SECU_CHANNEL_UNICAST:
            secChannel = PON_SECU_CHANNEL_UNICAST;
            break;
        default:
            return OPL_ERROR;
            break;
    }   

    switch(mode)
    {
        case PON_SECU_MODE_AES:
            oplRegFieldWrite(REG_PMAC_SECU_CFG,secChannel?4:5,1,0);
            break;
        case PON_SECU_MODE_TRIPLE_CHURNING:
            oplRegFieldWrite(REG_PMAC_SECU_CFG,secChannel?4:5,1,1);
            break;
        default:
            return OPL_ERROR;
            break;
    }   

    return OPL_OK;
}

OPL_STATUS dalPonSecurityModeGet(UINT32 channel,UINT32 *mode)
{
    UINT32 secChannel;

    if(NULL == mode)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
    switch(channel)
    {
        case PON_SECU_CHANNEL_SCB:
            secChannel = PON_SECU_CHANNEL_SCB;
            break;
        case PON_SECU_CHANNEL_UNICAST:
            secChannel = PON_SECU_CHANNEL_UNICAST;
            break;
        default:
            return OPL_ERROR;
            break;
    }   

    oplRegFieldRead(REG_PMAC_SECU_CFG,secChannel?4:5,1,mode);

    if(*mode)
    {
        *mode = PON_SECU_MODE_TRIPLE_CHURNING;
    }else
    {
        *mode = PON_SECU_MODE_AES;
    }
   
    return OPL_OK;
}

OPL_STATUS dalPonSecurityAesModeSet(UINT32 channel,UINT32 mode)
{
    UINT32 secChannel;
     
    switch(channel)
    {
        case PON_SECU_CHANNEL_SCB:
            secChannel = PON_SECU_CHANNEL_SCB;
            break;
        case PON_SECU_CHANNEL_UNICAST:
            secChannel = PON_SECU_CHANNEL_UNICAST;
            break;
        default:
            return OPL_ERROR;
            break;
    }   

    switch(mode)
    {
        case PON_AES_MODE_OFB:
        case PON_AES_MODE_CTR:
        case PON_AES_MODE_CFB_128:
            oplRegFieldWrite(REG_PMAC_SECU_CFG,0,3,mode);
            break;
        default:
            return OPL_ERROR;
            break;
    }

    return OPL_OK;
}

OPL_STATUS dalPonSecurityAesModeGet(UINT32 channel,UINT32 *mode)
{
    UINT32 secChannel;

    if(NULL == mode)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
    switch(channel)
    {
        case PON_SECU_CHANNEL_SCB:
            secChannel = PON_SECU_CHANNEL_SCB;
            break;
        case PON_SECU_CHANNEL_UNICAST:
            secChannel = PON_SECU_CHANNEL_UNICAST;
            break;
        default:
            return OPL_ERROR;
            break;
    }   

    oplRegFieldRead(REG_PMAC_SECU_CFG,0,3,mode);

    switch(*mode)
    {
        case PON_AES_MODE_OFB:
        case PON_AES_MODE_CTR:
        case PON_AES_MODE_CFB_128:
            *mode = *mode;
            break;
        default:
            return OPL_ERROR;
            break;
    }
   
    return OPL_OK;
}


/*******************************************************************************
* dalPonUnicastTripleChurningKeyGet
*
* DESCRIPTION:
*  		this function is used get the Triple Churning Key
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonUnicastTripleChurningKeyGet(UINT8 keyIndex,UINT8 *keyBuff)
{
	OPL_STATUS retVal = OPL_OK;
    UINT32 secuMode;

	if(OPL_NULL == keyBuff)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	if(keyIndex > PON_SECU_KEY_1)
	{
		OPL_DAL_PRINTF(("no such index.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}

    retVal = oplRegFieldRead(REG_PMAC_SECU_CFG,4,1,&secuMode);
    if(OPL_OK != retVal)
    {
        return retVal;
    }

    if(!secuMode)
    {
        oplRegFieldWrite(REG_PMAC_SECU_CFG,4,1,1);
    }
    
	/*first generate a key*/
	retVal = ponSecuKeyGenerate(keyIndex);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return OPL_ERROR;
	}	

	retVal = ponSecuKeyHwRead(PON_SECU_CHANNEL_UNICAST, PON_SECU_MODE_TRIPLE_CHURNING,keyIndex, keyBuff);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return OPL_ERROR;
	}

	if(ctcTripleChurningDebug)
	{
		OPL_DAL_PRINTF(("key: %04x %04x %04x\n",keyBuff[0],keyBuff[1],keyBuff[2]));
	}
	return retVal;
}

/*******************************************************************************
* dalPonScbTripleChurningKeySet
*
* DESCRIPTION:
*  		this function is used get the Triple Churning Key
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonScbTripleChurningKeySet(UINT8 keyIndex,UINT8 *keyBuff)
{
	OPL_STATUS retVal = OPL_OK;
    UINT32 secuMode;

	if(OPL_NULL == keyBuff)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

    retVal = oplRegFieldRead(REG_PMAC_SECU_CFG,5,1,&secuMode);
    if(OPL_OK != retVal)
    {
        return retVal;
    }

    if(!secuMode)
    {
        oplRegFieldWrite(REG_PMAC_SECU_CFG,5,1,1);
    }

	if(keyIndex > PON_SECU_KEY_1)
	{
		OPL_DAL_PRINTF(("no such index.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	retVal = ponSecuKeyHwWrite(PON_SECU_CHANNEL_SCB, PON_SECU_MODE_TRIPLE_CHURNING,keyIndex, keyBuff);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return OPL_ERROR;
	}
	
	return retVal;
}

/*******************************************************************************
* dalPonUnicastAesKeyGet
*
* DESCRIPTION:
*  		this function is used get aes key
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonUnicastAesKeyGet(UINT8 keyIndex,UINT8 *keyBuff,UINT8 mode)
{
	OPL_STATUS retVal = OPL_OK;
    UINT32 secuMode;
    
	if(OPL_NULL == keyBuff)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	if(keyIndex > PON_SECU_KEY_1)
	{
		OPL_DAL_PRINTF(("no such index.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}

    retVal = oplRegFieldRead(REG_PMAC_SECU_CFG,4,1,&secuMode);
    if(OPL_OK != retVal)
    {
        return retVal;
    }

    if(secuMode)
    {
        oplRegFieldWrite(REG_PMAC_SECU_CFG,4,1,0);
    }

    oplRegFieldWrite(REG_PMAC_SECU_CFG,0,3,mode);

	/*first generate a key*/
	retVal = ponSecuKeyGenerate(keyIndex);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return OPL_ERROR;
	}

	retVal = ponSecuKeyHwRead(PON_SECU_CHANNEL_UNICAST, PON_SECU_MODE_AES,keyIndex, keyBuff);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return OPL_ERROR;
	}
	return retVal;
}


/*******************************************************************************
* dalPonScbAesKeySet
*
* DESCRIPTION:
*  		this function is used get aes key
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonScbAesKeySet(UINT8 keyIndex,UINT8 *keyBuff,UINT8 mode)
{
	OPL_STATUS retVal = OPL_OK;
    UINT32 secuMode;
	
	if(OPL_NULL == keyBuff)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	if(keyIndex > PON_SECU_KEY_1)
	{
		OPL_DAL_PRINTF(("no such index.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}

    retVal = oplRegFieldRead(REG_PMAC_SECU_CFG,5,1,&secuMode);
    if(OPL_OK != retVal)
    {
        return retVal;
    }

    if(secuMode)
    {
        oplRegFieldWrite(REG_PMAC_SECU_CFG,5,1,0);
    }

    oplRegFieldWrite(REG_PMAC_SECU_CFG,0,3,mode);
    
	retVal = ponSecuKeyHwWrite(PON_SECU_CHANNEL_SCB, PON_SECU_MODE_AES,keyIndex, keyBuff);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return OPL_ERROR;
	}
	return retVal;
}

/*******************************************************************************
* dalPonTripleChurningModeGet
*
* DESCRIPTION:
*  		this function is used get aes key
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonTripleChurningModeGet(UINT8 *mode)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	retVal = ponTripleChurningModeHwRead(&regVal);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	*mode = regVal?OPL_ENABLE:OPL_DISABLE;

	return retVal;
}

/*******************************************************************************
* dalPonTripleChurningModeSet
*
* DESCRIPTION:
*  		this function is used get aes key
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonTripleChurningModeSet(UINT8 mode)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = mode;
	
	retVal = ponTripleChurningModeHwWrite(regVal);
	
	return retVal;
}

/*******************************************************************************
* dalPonMpcpDbaAgentConfigAllSet
*
* DESCRIPTION:
*  		this function is used to set mpcp dba agent delay config
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMpcpDbaAgentConfigAllSet(
	UINT32 cpRptStaMode,
    UINT32 cpMergeRptSendMode,
    UINT32 cpRptSendMode,
    UINT32 cpPktInitTimerCfg,
	UINT32 cpDeltaTxNormalNoFecTime,
	UINT32 cpTxMpcpNoFecTime
    )
{
    return  ponMpcpDbaAgetConfigAllSet(
	 cpRptStaMode,
     cpMergeRptSendMode,
     cpRptSendMode,
     cpPktInitTimerCfg,
	 cpDeltaTxNormalNoFecTime,
	 cpTxMpcpNoFecTime
    );  
}

/*******************************************************************************
* dalPonMpcpDbaAgentConfigAllGet
*
* DESCRIPTION:
*  		this function is used to set mpcp dba agent delay config
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMpcpDbaAgentConfigAllGet(
	UINT32 *cpRptStaMode,
    UINT32 *cpMergeRptSendMode,
    UINT32 *cpRptSendMode,
    UINT32 *cpPktInitTimerCfg,
	UINT32 *cpDeltaTxNormalNoFecTime,
	UINT32 *cpTxMpcpNoFecTime
    )
{
    return  ponMpcpDbaAgetConfigAllGet(
	cpRptStaMode,
    cpMergeRptSendMode,
    cpRptSendMode,
    cpPktInitTimerCfg,
	cpDeltaTxNormalNoFecTime,
	cpTxMpcpNoFecTime
    );  
}

/*******************************************************************************
* dalPonDelayConfigAllSet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonDelayConfigAllSet(UINT8 cpTxDlyEn,UINT8 cpSyncEn,UINT16 cpRptsentDelt,UINT16 cpTxDly)
{
    return ponDelayConfigAllSet(cpTxDlyEn,cpSyncEn,cpRptsentDelt,cpTxDly);
}

/*******************************************************************************
* dalPonDelayConfigAllGet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonDelayConfigAllGet(UINT32 *cpTxDlyEn,UINT32 *cpSyncEn,UINT32 *cpRptsentDelt,UINT32 *cpTxDly)
{
    return ponDelayConfigAllGet(cpTxDlyEn,cpSyncEn,cpRptsentDelt,cpTxDly);
}

OPL_STATUS dalPonMpcpStatusGet(DAL_PON_STATUS_t *ponStatus)
{
	OPL_STATUS retVal;
	PON_MPCP_DISC_STATUS_t mpcpStatus;

    if(OPL_NULL == ponStatus)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
	retVal = oplRegRead(REG_MPCP_DISC_STATUS, (UINT32 *)&mpcpStatus);

	if(OPL_OK != retVal)
	{
		return retVal;
	}
    
	ponStatus->bfGatePendingNum = mpcpStatus.bfGatePendingNum;
	
	if(mpcpStatus.bfDiscLEvent <= EVENT_WATCHDOG_TIMEOUT)
	{
		ponStatus->bfDiscLEvent = mpcpStatus.bfDiscLEvent;
	}
	
	if(mpcpStatus.bfDiscCEvent <= EVENT_WATCHDOG_TIMEOUT)
	{
		ponStatus->bfDiscCEvent = mpcpStatus.bfDiscCEvent;
	}

	if(mpcpStatus.bfDiscLState <= STATE_WATCHDOG_TIMEOUT)
	{
		ponStatus->bfDiscLState = mpcpStatus.bfDiscLState;
	}
	
	if(mpcpStatus.bfDiscCState <= STATE_WATCHDOG_TIMEOUT)
	{
		ponStatus->bfDiscCState = mpcpStatus.bfDiscCState;
	}

	return OPL_OK;
}

/*******************************************************************************
* dalPonOamFinishedSet
*
* DESCRIPTION:
*  		this function is used to set pon delay config info 
*	INPUTS:
*		keyIndex: which key
*		
*	OUTPUTS:
*		key: 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonOamFinishedSet(UINT32 oamFinished)
{
    return ponOamFinishedHwWrite(oamFinished);
}


/*******************************************************************************
* dalPonMpcpFsmHoldOverCtrlSet
*
* DESCRIPTION:
*  		this function is used to enable or disable holdover feature 
*	INPUTS:
*		enable: 1-enable 0-disable
*		
*	OUTPUTS:
*		 
* 	RETURNS:
* 		OPL_OK if the access is success.
* 		OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMpcpFsmHoldOverCtrlSet(UINT32 enable)
{
	UINT8 driftHoldEn;
	UINT8 timerHoldEn;
	UINT8 grantFlushEn;
	UINT8 losPosEn;
	UINT8 losHoldEn;
	UINT8 linkHoldEn;
	UINT8 softHoldEn;

	if(1 == enable)
	{
		driftHoldEn = 1;
		timerHoldEn = 1;
		grantFlushEn = 1;
		losPosEn = 0;
		losHoldEn = 1;
		linkHoldEn = 1;
		softHoldEn = 0;
	}
	else
	{
		driftHoldEn = 0;
		timerHoldEn = 0;
		grantFlushEn = 0;
		losPosEn = 1;
		losHoldEn = 0;
		linkHoldEn = 0;
		softHoldEn = 1;
	}
	
	return ponMpcpFsmHoldControlHwWrite(driftHoldEn,
										timerHoldEn,
										grantFlushEn, 
										losPosEn, 
										losHoldEn, 
										linkHoldEn, 
										softHoldEn);
}

/*******************************************************************************
* dalPonMpcpFsmHoldOverCtrlGet
*
* DESCRIPTION:
*  		this function is used to get enable status of holdover feature 
*	INPUTS:
*		
*	OUTPUTS:
*		enable: 0-disable 1-enable
*		 
* 	RETURNS:
* 		OPL_OK if the access is success.
* 		OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMpcpFsmHoldOverCtrlGet(UINT32 *enable)
{
	UINT8 driftHoldEn;
	UINT8 timerHoldEn;
	UINT8 grantFlushEn;
	UINT8 losPosEn;
	UINT8 losHoldEn;
	UINT8 linkHoldEn;
	UINT8 softHoldEn;	
	OPL_STATUS ret;

	ret = OPL_OK;

	ret = ponMpcpFsmHoldControlHwRead(&driftHoldEn,
								&timerHoldEn,
								&grantFlushEn, 
								&losPosEn, 
								&losHoldEn, 
								&linkHoldEn, 
								&softHoldEn);	
	if(ret != OPL_OK)
		return ret;

	if((driftHoldEn == 1)
		&&(timerHoldEn == 1)
		&&(grantFlushEn == 1)
		&&(losPosEn == 0)
		&&(losHoldEn == 1)
		&&(linkHoldEn == 1)
		&&(softHoldEn == 0))
	{
		*enable = 1;
	}
	else
	{
		*enable = 0;
	}

	return ret;
}

/*******************************************************************************
* dalPonMpcpFsmHoldOverTimeSet
*
* DESCRIPTION:
*  		this function is used to set holdover time 
*	INPUTS:
*		holdTime: holdover time
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 		OPL_OK if the access is success.
* 		OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMpcpFsmHoldOverTimeSet(UINT32 holdTime)
{
	UINT32 tq;
	UINT64 tmpHoldTime;

	tmpHoldTime = (UINT64)holdTime;
	tmpHoldTime = tmpHoldTime * 1000000;
	/* transform holdtime unit from ms to TQ */
	tq = (UINT32)(tmpHoldTime / 16);

	return ponMpcpFsmHoldTimeHwWrite(tq);
}

/*******************************************************************************
* dalPonMpcpFsmHoldOverTimeGet
*
* DESCRIPTION:
*  		this function is used to get holdover time
*	INPUTS:
*		
*		
*	OUTPUTS:
*		holdTime: holdover time
*
* 	RETURNS:
* 		OPL_OK if the access is success.
* 		OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPonMpcpFsmHoldOverTimeGet(UINT32 *holdTime)
{
	UINT32 tq;
	OPL_STATUS ret;

	ret= ponMpcpFsmHoldTimeHwRead(&tq);
	if(ret != OPL_OK)
		return ret;

	/* transform holdtime unit from TQ to ms */
	*holdTime = (tq * 16) / 1000000;

	return ret;
}

/*******************************************************************************
* dalOnuMtuSet
*
* DESCRIPTION:
*  		this function is used to set onu mtu info 
*	INPUTS:
*		mtuSize: mtu size
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 		OPL_OK if the access is success.
* 		OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalOnuMtuSet(UINT32 mtuSize)
{
	OPL_STATUS ret;
    ret = oplRegWrite(REG_PMAC_CFG_MAXFRAMLEN,mtuSize);
    ret += oplRegWrite(REG_GMAC_CFG_MAXFRAMLEN,mtuSize);
    ret += oplRegFieldWrite(REG_GE_PARSER_PKTLEN_PARA,16,12,mtuSize+4);
	return ret;
}

/*******************************************************************************
* dalOnuMtuGet
*
* DESCRIPTION:
*  		this function is used to get onu mtu info 
*	INPUTS:
*		mtuSize: mtu size

*
* 	RETURNS:
* 		OPL_OK if the access is success.
* 		OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalOnuMtuGet(UINT32 *mtuSize)
{
	return oplRegRead(REG_GMAC_CFG_MAXFRAMLEN,&mtuSize);
}

OPL_STATUS dalOnuPonLoopbackConfig(UINT32 uniorscb, UINT32 enable)
{
	UINT32  retVal = OPL_OK;
	UINT32 tmpregVal;

	if (IS_ONU_RESPIN)
	{
		if(!enable)// select enable uni_llid or scb_llid
		{
			retVal = oplRegRead(0x3023*4,&tmpregVal);
			if(OPL_OK != retVal)
			{
				printk("read 0x3023 failed.\n");
				return OPL_ERR_REG_READ_FAIL;
			}
			if(!((tmpregVal&0x800000)>>23))
			{
				retVal += oplRegFieldWrite(0x3023*4, 23, 1, 1);
				if(OPL_OK != retVal)
				{
					printk("write 0x3023 failed.\n");
					return OPL_ERR_REG_WRITE_FAIL;
				}
			}

			if(0 == uniorscb)//uni llid
			{
				retVal += oplRegFieldWrite(0x3020*4, 17,1,1);	
				if(OPL_OK != retVal)
				{
					printk("write 0x3020 failed.\n");
					return OPL_ERR_REG_WRITE_FAIL;
				}
			}
			else if(1== uniorscb)//scb llid
			{
				retVal += oplRegFieldWrite(0x3020*4, 16,1,1);
				if(OPL_OK != retVal)
				{
					printk("write 0x3020 failed.\n");
					return OPL_ERR_REG_WRITE_FAIL;
				}
			}
			else
			{
				printk("Input error.\r\n");
			}
		}
		else if(1==enable)//disable uni_llid or scb_llid
		{	
			if(0 == uniorscb)//uni llid
			{
				retVal += oplRegFieldWrite(0x3020*4, 17,1,0);
				if(OPL_OK != retVal)
				{
					printk("write 0x3020 failed.\n");
					return OPL_ERR_REG_WRITE_FAIL;
				}
			}
			else if(1==uniorscb)//scb llid
			{
				retVal += oplRegFieldWrite(0x3020*4, 16,1,0);
				if(OPL_OK != retVal)
				{
					printk("write 0x3020 failed.\n");
					return OPL_ERR_REG_WRITE_FAIL;
				}
			}
			else
			{	
				printk("Input error.\r\n");
			}
		}
		else
		{	
			printk("Input error.\r\n");
		}
		return OPL_OK;
	}
	else
	{
		return OPL_ERROR;
	}
}


OPL_STATUS dalOnuPonTxPolarityConfig(UINT32 loworhigh)
{
	UINT32  retVal = OPL_OK;

	if (IS_ONU_RESPIN)
	{
		if((0 != loworhigh) && (1 != loworhigh))
		{
			return OPL_ERR_INVALID_PARAMETERS;
		}
		if(1 == loworhigh)
		{
			retVal += oplRegFieldWrite(0x0a19*4, 5, 1, 1);
			if(OPL_OK != retVal)
			{
				printk("write 0x0a19 failed.\n");
				return OPL_ERR_REG_WRITE_FAIL;
			}
		}
		else if(0 == loworhigh)
		{
			retVal += oplRegFieldWrite(0x0a19*4, 5, 1, 0);
			if(OPL_OK != retVal)
			{
				printk("write 0x0a19 failed.\n");
				return OPL_ERR_REG_WRITE_FAIL;
			}
		}
		else
		{
			printk("Input error.\r\n");
		}
	}

	return OPL_OK;
}

