/*
=============================================================================
     Header Name: dal_port.c

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

extern UINT16 phyAdminStatus;
OPL_STATUS dalPortNumIsInvalid(UINT8 portId)
{
    if(DAL_NUM_OF_PORTS != portId)
    {
        return OPL_ERR_INVALID_PARAMETERS;
    }else
    {
        return OPL_OK;
    }
}

OPL_STATUS dalPortPvidSet(UINT8 portId,UINT16 pvid)
{
    return dalVttVidSet(pvid,portId);
}

OPL_STATUS dalPortPvidGet(UINT8 portId,UINT16 *pvid)
{
    return dalVttVidGet(pvid,portId);
}

OPL_STATUS dalPortVlanModeSet(UINT8 portId,UINT8 mode)
{
    return dalVttModeSet(mode,portId);
}

OPL_STATUS dalPortVlanModeGet(UINT8 portId,UINT8 *mode)
{
    return dalVttModeGet(mode,portId);
}

OPL_STATUS dalPortDefaultPriSet(UINT8 portId,UINT8 defaultPri)
{
    return dalVttDefaultPriSet(defaultPri,portId);
}

OPL_STATUS dalPortDefaultPriGet(UINT8 portId,UINT8 *defaultPri)
{
    return dalVttDefaultPriGet(defaultPri,portId);
}
 
/*******************************************************************************
* dalPortStateGet
*
* DESCRIPTION:
*  		this function is used to get the ethernet port status down or up.
*
*	INPUTS:
*			portId:0x0
*
*	OUTPUTS:
*			status:0x01:up,0x00:down		     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortStateGet(UINT8 portId,UINT8 *status)
{
    OPL_STATUS retVal = OPL_OK;

    if(OPL_NULL == status)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }
    retVal = phyLinkStatusHwRead(status);
    return retVal;
}
/*******************************************************************************
* dalPortSpeedSet
*
* DESCRIPTION:
*  		this function is used to set port speed 
*
*	INPUTS:
*			portId:0x01-0x04
*			speed:10 100 1000.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortSpeedSet(UINT8 portId,UINT32 portSpeed)
{
    PHY_SPEED_MODE_e phySpeedMode;
    UINT8 phyAutoNegEnable;
    UINT32 regCfgIPG, regSpeedMode, regPreamble, sysClkNum,rgmiiRegPreamble;
	UINT32 rgmiispeedMode;
    UINT32 pauseTime = 0xff;
    OPL_STATUS retVal = OPL_OK;

    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }
	
    switch(portSpeed)
    {
        case DAL_1000_MBPS:
			regCfgIPG = 12;
			regSpeedMode = 1;
			regPreamble = 7;
            phySpeedMode = PHY_SPEED_1000M;
       	    rgmiispeedMode=0x2;			
			rgmiiRegPreamble= 7;
            break;
        case DAL_100_MBPS:
			regCfgIPG = 22;
			regSpeedMode = 0;
			regPreamble = 15;
            phySpeedMode = PHY_SPEED_100M;
			rgmiispeedMode=0x1;
			rgmiiRegPreamble= 0x7B;
            break;
        case DAL_10_MBPS:
			regCfgIPG = 22;
			regSpeedMode = 0;
			regPreamble = 15;
            phySpeedMode = PHY_SPEED_10M;
		    rgmiispeedMode=0x0;
			rgmiiRegPreamble= 0x7B;
            break;
        default:
            OPL_LOG_TRACE();    
            return OPL_ERR_INVALID_PARAMETERS;
            break;
    }
#ifdef ONU_RESPIN_A
    retVal = oplRegFieldWrite(REG_GMAC_RGMII_SOFT_CFG, 0, 2,rgmiispeedMode);
    if(OPL_OK != retVal)
    {
        OPL_LOG_TRACE();
        return retVal;
    }
#endif
    retVal = oplRegFieldWrite(REG_GMAC_CFG_SPEED, 0, 1, regSpeedMode);
    if(OPL_OK != retVal)
    {
        OPL_LOG_TRACE();
        return retVal;
    }    
    retVal = oplRegWrite(REG_GMAC_CFG_IPG, regCfgIPG);
    if(OPL_OK != retVal)
    {
        OPL_LOG_TRACE();
        return retVal;
    }
    #ifdef ONU_RESPIN_A
	/* RGMII mode connecting with Atheros AR8035 */
    retVal = oplRegFieldWrite(REG_GMAC_CFG_PRENUM, 0, 8, rgmiiRegPreamble);
	#else
    retVal = oplRegFieldWrite(REG_GMAC_CFG_PRENUM, 0, 8, regPreamble);
    #endif
    if(OPL_OK != retVal)
    {
        OPL_LOG_TRACE();
        return retVal;
    }

    phyAutoNegEnable = 0;
    phyAutoNegHwRead(&phyAutoNegEnable);
    if (0 == phyAutoNegEnable){ /* not need to configure PHY for the auto negotiation.*/
        retVal = phySpeedHwWrite(phySpeedMode);
    }
    
	if (OPL_OK == retVal){ /* Bug 2963, need to change the pause time register accordingly. */	
		if (OPL_OK == oplRegRead(REG_CFG_GE_PAUSE_PARAM_SYS, &sysClkNum)){
			pauseTime = (sysClkNum*portSpeed)/64000; /* 64000==(125*512); */ 
			if (OPL_OK != oplRegFieldWrite(REG_CFG_GE_PAUSE_PARAM, 0, 16, (pauseTime&0xFFFF))){
				OPL_LOG_TRACE(); 
			}
		} 
	}

    return retVal;
}

/*******************************************************************************
* dalPortSpeedGet
* DESCRIPTION:
*  		this function is used to get the port speed
*
*	INPUTS:
*			portId:0x01-0x04
*			speed:10 100 1000.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortSpeedGet(UINT8 portId,UINT32 *portSpeed)
{
    OPL_STATUS retVal = OPL_OK;
    UINT32 regVal;

    if(OPL_NULL == portSpeed)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

    retVal = oplRegFieldRead(REG_GMAC_CFG_SPEED, 0, 1, &regVal);
    if(OPL_OK != retVal)
    {
        return retVal;
    }

    if(regVal)
    {
        *portSpeed = DAL_1000_MBPS;
    }else
    {
        /*get phy speed*/
        *portSpeed = DAL_100_MBPS;
    }

    return OPL_OK;
}
/* begin: add by xukang for bug 3265 */
/*******************************************************************************
* dalPortSpecificSpeedRead
* DESCRIPTION:
*  		this function is used to get the phy specific speed
*
*	INPUTS:
*			portId:0x01-0x04
*			speed:10 100 1000.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortSpecificSpeedRead(UINT8 portId, UINT32 *portSpeed)
{
	PHY_SPEED_MODE_e phySpeed=0;
		
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if(OPL_NULL == portSpeed)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
    phySpecificSpeedRead(&phySpeed);

	switch(phySpeed)
	{
		case PHY_SPEED_10M:
			*portSpeed = DAL_10_MBPS;
			break;
		case PHY_SPEED_100M:
			*portSpeed = DAL_100_MBPS;
			break;
		case PHY_SPEED_1000M:
			*portSpeed = DAL_1000_MBPS;
			break;
		default:
			OPL_LOG_TRACE();
			return OPL_ERR_INVALID_PARAMETERS;
	}

	return OPL_OK;	
}

/*******************************************************************************
* dalPhySpecificAutonegRead
*
* DESCRIPTION:
*  		this function is used to get autonegocitation status info for the port.0x01 for completed,0x02 for failed
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		autonegState:		completed:0x01,failed:0x00
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPhySpecificAutonegRead(UINT8 portId, UINT32 *autonegState)
{
     PHY_AUTONEG_DONE_e phyAutonegDone=0;

    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

       if(OPL_NULL == autonegState)
       {
          return OPL_ERR_NULL_POINTER;
       }

	phySpecificAutoRead(&phyAutonegDone);
		
	switch(phyAutonegDone)
	{
		case  PHY_AUTONEG_FAILED:
			*autonegState = DAL_AUTONEG_FAILED;
			break;
		case PHY_AUTONEG_COMPLETED:
			*autonegState = DAL_AUTONEG_COMPLETED;
			break;
		default:
			OPL_LOG_TRACE();
			return OPL_ERR_INVALID_PARAMETERS;
	}

	return OPL_OK;	
	
}

/*******************************************************************************
* dalPortSpecificDuplexRead
*
* DESCRIPTION:
*  		this function is used to get the port specific duplex
*
*	INPUTS:
*			portId:0x01-0x04
*			duplex: Full-duplex or half-duplex.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortSpecificDuplexRead(UINT8 portId,UINT32 *duplex)
{
     PHY_DUPLEX_MODE_e  phyDuplex=0;
       
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

    if(OPL_NULL == duplex)
    {
        return OPL_ERR_NULL_POINTER;
    }

    phySpecificDuplexRead(&phyDuplex);

	switch(phyDuplex)
	{
		case PHY_HALF_DUPLEX:
			*duplex = DAL_HALF_DUPLEX;
			break;
		case PHY_FULL_DUPLEX:
			*duplex = DAL_FULL_DUPLEX;
			break;
		default:
			OPL_LOG_TRACE();
			return OPL_ERR_INVALID_PARAMETERS;
	}

	return OPL_OK;	
}
/* end: add by xukang for bug 3265 */	
/*******************************************************************************
* dalPortDuplexSet
*
* DESCRIPTION:
*  		this function is used to set port speed 
*
*	INPUTS:
*			portId:0x01-0x04
*			speed:10 100 1000.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDuplexSet(UINT8 portId,UINT32 duplex)
{
    UINT8 phyAutoNegEnable;
    PHY_DUPLEX_MODE_e phyDuplexMode;
    
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

    switch(duplex)
    {
        case DAL_HALF_DUPLEX:
            phyDuplexMode = PHY_HALF_DUPLEX;
			if (IS_ONU_RESPIN)
			{
				oplRegFieldWrite(0x90a*4, 0, 1, 0);
#ifdef ONU_RESPIN_A
				oplRegFieldWrite(REG_GMAC_RGMII_SOFT_CFG,2,1,0);
#endif 
			}
            break;
        case DAL_FULL_DUPLEX:
            phyDuplexMode = PHY_FULL_DUPLEX;
			if (IS_ONU_RESPIN)
			{
				oplRegFieldWrite(0x90a*4, 0, 1, 1);
#ifdef ONU_RESPIN_A
				oplRegFieldWrite(REG_GMAC_RGMII_SOFT_CFG,2,1,1);
#endif
			}
            break;
        default:
            OPL_LOG_TRACE();
		    return OPL_ERR_INVALID_PARAMETERS;
            break;
    }

    phyAutoNegEnable = 0;
    phyAutoNegHwRead(&phyAutoNegEnable);
    if (0 == phyAutoNegEnable){ /* not need to configure PHY for the auto negotiation.*/
        return phyDuplexHwWrite(phyDuplexMode);
    }
    return OPL_OK;
}

/*******************************************************************************
* dalPortDuplexGet
*
* DESCRIPTION:
*  		this function is used to get the port speed
*
*	INPUTS:
*			portId:0x01-0x04
*			speed:10 100 1000.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDuplexGet(UINT8 portId,UINT32 *duplex)
{
    OPL_STATUS retVal = OPL_OK;
    
    if(OPL_NULL == duplex)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

    retVal = phyDuplexHwRead((PHY_DUPLEX_MODE_e *)duplex);
    return retVal;
}
/*******************************************************************************
* dalPortFlowcontrolSet
*
* DESCRIPTION:
*  		this function is used to dsiable or enable the flow control
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortFlowcontrolSet(UINT8 portId,UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}
	regVal = enable;
/* Begin Modified */
#if 0
	retVal = oplRegFieldWrite(REG_GMAC_CFG_PAUSE_ENA, 1, 1, regVal);
#else
	retVal = oplRegFieldWrite(REG_GMAC_CFG_PAUSE_ENA, 0, 1, regVal);
    retVal = oplRegFieldWrite(REG_GMAC_CFG_PAUSE_ENA, 1, 1, regVal);
#endif
/* End   Modified of EM200 */
    if(OPL_OK != retVal)
    {
        return retVal;
    }

    retVal = phyFlowControlEnHwWrite(enable);
	
	return retVal;
}
/*******************************************************************************
* dalPortFlowcontrolGet
*
* DESCRIPTION:
*  			this function is used to get flowcontrol config info.
*
*	INPUTS:
*			portId:0x0
*	OUTPUTS:
*			enable:0x00:disable,0x01:enable. 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortFlowcontrolGet(UINT8 portId,UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

	if(OPL_NULL == enable)
	{
		OPL_DAL_PRINTF(("in put null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}
    
	retVal = oplRegFieldRead(REG_GMAC_CFG_PAUSE_ENA, 1, 1, &regVal);
	*enable = regVal;
	return OPL_OK;
}
/*******************************************************************************
* dalPortLoopbackSet
*
* DESCRIPTION:
*  		this function is used to dsiable or enable the externel loop back
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable.
*	OUTPUTS:
*			 
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortLoopbackSet(UINT8 portId,UINT8 enable)
{
    OPL_STATUS retVal = OPL_OK;

    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

	/*xwang : Not config the phy, but config our chip*/
	#if 0
		retVal = phyLoopbackEnableHwWrite(enable?1:0);
	#else
		retVal = oplRegWrite(REG_GMAC_LP_BACK, enable?1:0);
	#endif
    
    return retVal;
}
/*******************************************************************************
* dalPortUsPolicingEnableSet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingEnableSet(UINT8 portId,UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	retVal = tmGeShaperEnableHwWrite(UP_STREAM, enable);
	
	return retVal;
}

/*******************************************************************************
* dalPortUsPolicingEnableGet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingEnableGet(UINT8 portId,UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

    if(OPL_NULL == enable)
    {
        return OPL_ERR_NULL_POINTER;
    }
	
	retVal = tmGeShaperEnableHwRead(UP_STREAM, enable);
	
	return retVal;
}

/*******************************************************************************
* dalPortUsPolicingCirSet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			cir:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingCirSet(UINT8 portId,UINT32 cir)
{
	OPL_STATUS retVal = OPL_OK;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	retVal = tmGeCirHwWrite(UP_STREAM, (cir%32)?(cir>>5+1):(cir>>5));

    return retVal;
}

/*******************************************************************************
* dalPortUsPolicingCirGet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			cir:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingCirGet(UINT8 portId,UINT32 *cir)
{
	OPL_STATUS retVal = OPL_OK;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

    if(OPL_NULL == cir)
    {   
        return OPL_ERR_NULL_POINTER;
    }
	
	retVal = tmGeCirHwRead(UP_STREAM, cir);

    *cir = *cir*32;
    return retVal;
}

/*******************************************************************************
* dalPortUsPolicingCbsSet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			ebs:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingCbsSet(UINT8 portId,UINT32 cbs)
{
	OPL_STATUS retVal = OPL_OK;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	retVal = tmGeCbsHwWrite(UP_STREAM, (cbs%256)?(cbs/256+1):(cbs/256));

    return retVal;
}

/*******************************************************************************
* dalPortUsPolicingCbsGet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			cbs:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingCbsGet(UINT8 portId,UINT32 *cbs)
{
	OPL_STATUS retVal = OPL_OK;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

    if(OPL_NULL == cbs)
    {
        return OPL_ERR_NULL_POINTER;
    }
	
	retVal = tmGeCbsHwRead(UP_STREAM, cbs);
    *cbs = *cbs*256;

    return retVal;
}

/*******************************************************************************
* dalPortUsPolicingEbsSet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			ebs:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingEbsSet(UINT8 portId,UINT32 ebs)
{
	OPL_STATUS retVal = OPL_OK;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
    return retVal;
}

/*******************************************************************************
* dalPortUsPolicingEbsGet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			ebs:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingEbsGet(UINT8 portId,UINT32 *ebs)
{
	OPL_STATUS retVal = OPL_OK;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}

    if(OPL_NULL == ebs)
    {
        return OPL_ERR_NULL_POINTER;
    }
	
    return retVal;
}
/*******************************************************************************
* dalPortUsPolicingSet
*
* DESCRIPTION:
*  			this function is used to set the upstream policing parameter.
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable
*			cir:  0x000000-0xffffff
*			cbs:
*			ebs:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingSet(UINT8 portId,UINT8 enable,UINT32 cir,UINT32 cbs,UINT32 ebs)
{
	OPL_STATUS retVal = OPL_OK;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	retVal = tmGeShaperEnableHwWrite(UP_STREAM, enable);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	if(!enable)
	{
		return retVal;
	}

	retVal = tmGeCirHwWrite(UP_STREAM, (cir%32)?(cir/32+1) - 1:(cir/32) - 1);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

    if(((cbs+ebs)/256)>1023)
    {
        cbs=1023*256;
        ebs=0;
    }
    
	retVal = tmGeCbsHwWrite(UP_STREAM, ((cbs+ebs)%256)?((cbs+ebs)/256+1):((cbs+ebs)/256));
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	/* opconn no support ebs */
	return retVal;
}
/*******************************************************************************
* dalPortUsPolicingGet
*
* DESCRIPTION:
*  			this function is used to get the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			
*	OUTPUTS:
*			enable:0x00:disable,0x01:enable
*			cir:
*			cbs:
*			ebs:	
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortUsPolicingGet(UINT8 portId,UINT8 *enable,UINT32 *cir,UINT32 *cbs,UINT32 *ebs)
{
	OPL_STATUS retVal = OPL_OK;

	if(OPL_NULL == enable || OPL_NULL  == cir || OPL_NULL == cbs || OPL_NULL == ebs)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

	retVal = tmGeShaperEnableHwRead(UP_STREAM, enable);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	
	if(*enable != OPL_ENABLE)
	{
		return retVal;
	}
	retVal = tmGeCirHwRead(UP_STREAM, cir);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
    *cir = *cir*32;
    
	retVal = tmGeCbsHwRead(UP_STREAM, cbs);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
    *cbs = *cbs*256;
    
	*ebs = OPL_ZERO; 
	return retVal;
}

/*******************************************************************************
* dalPortDsPolicingEnableSet
*
* DESCRIPTION:
*  			this function is used to set the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingEnableSet(UINT8 portId,UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

	retVal = tmGeShaperEnableHwWrite(DOWN_STREAM,enable);
	
	return retVal;
}
/*******************************************************************************
* dalPortDsPolicingEnableGet
*
* DESCRIPTION:
*  			this function is used to get the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			
*	OUTPUTS:
*			enable:0x00:disable,0x01:enable
*			cir:
*			cbs:
*			ebs:	
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingEnableGet(UINT8 portId,UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;

	if(OPL_NULL == enable)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}
    
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }
	retVal = tmGeShaperEnableHwRead(DOWN_STREAM, enable);
	
	return retVal;
}
/*******************************************************************************
* dalPortDsPolicingCirSet
*
* DESCRIPTION:
*  			this function is used to set the shaper parameter
*
*	INPUTS:
*			portId:0x0
*           cir:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingCirSet(UINT8 portId,UINT32 cir)
{
	OPL_STATUS retVal = OPL_OK;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

	retVal = tmGeCirHwWrite(DOWN_STREAM, (cir%32)?(cir/32+1):(cir/32));

	return retVal;
}
/*******************************************************************************
* dalPortDsPolicingCirGet
*
* DESCRIPTION:
*  			this function is used to get the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			cir
*	OUTPUTS:
*			enable:0x00:disable,0x01:enable
*			cir:
*			cbs:
*			ebs:	
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingCirGet(UINT8 portId,UINT32 *cir)
{
	OPL_STATUS retVal = OPL_OK;

	if(cir == OPL_NULL)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}
    
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }
	
	retVal = tmGeCirHwRead(DOWN_STREAM, cir);
    *cir = *cir*32;
	
	return retVal;
}
/*******************************************************************************
* dalPortDsPolicingPirSet
*
* DESCRIPTION:
*  			this function is used to set the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable
*           pir
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingPirSet(UINT8 portId,UINT32 cbs)
{
	OPL_STATUS retVal = OPL_OK;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

	retVal = tmGeCbsHwWrite(DOWN_STREAM, (cbs%256)?(cbs/256+1):(cbs/256));

	return retVal;
}

/*******************************************************************************
* dalPortDsPolicingPirGet
*
* DESCRIPTION:
*  			this function is used to set the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable
*           pir
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingPirGet(UINT8 portId,UINT32 *cbs)
{
	OPL_STATUS retVal = OPL_OK;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

	retVal = tmGeCbsHwRead(DOWN_STREAM, cbs);
    
    *cbs = *cbs*256;
    
	return retVal;
}

/*******************************************************************************
* dalPortDsPolicingSet
*
* DESCRIPTION:
*  			this function is used to set the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			enable:0x00:disable,0x01:enable
*			cir:
*			cbs:
*			ebs:
*	OUTPUTS:
*			
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingSet(UINT8 portId,UINT8 enable,UINT32 cir,UINT32 cbs)
{
	OPL_STATUS retVal = OPL_OK;
	
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

	retVal = tmGeShaperEnableHwWrite(DOWN_STREAM,enable);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	if(!enable)
	{
		return retVal;
	}

	retVal = tmGeCirHwWrite(DOWN_STREAM, (cir%32)?(cir/32+1) -1 :(cir/32) - 1);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	retVal = tmGeCbsHwWrite(DOWN_STREAM, (cbs%256)?(cbs/256+1):(cbs/256));
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	return retVal;
}
/*******************************************************************************
* dalPortDsPolicingGet
*
* DESCRIPTION:
*  			this function is used to get the shaper parameter
*
*	INPUTS:
*			portId:0x0
*			
*	OUTPUTS:
*			enable:0x00:disable,0x01:enable
*			cir:
*			cbs:
*			ebs:	
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPortDsPolicingGet(UINT8 portId,UINT8 *enable,UINT32 *cir,UINT32 *cbs)
{
	OPL_STATUS retVal = OPL_OK;

	if(OPL_NULL == enable ||  cir == OPL_NULL || cbs == OPL_NULL)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}
    
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }
	retVal = tmGeShaperEnableHwRead(DOWN_STREAM, enable);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	
	if(*enable != OPL_ENABLE)
	{
		return retVal;
	}
	retVal = tmGeCirHwRead(DOWN_STREAM, cir);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	retVal = tmGeCbsHwRead(DOWN_STREAM, cbs);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
    *cir = *cir*32;
    *cbs = *cbs*256;
	return retVal;
}
/*******************************************************************************
* dalPhyAdminStateGet
*
* DESCRIPTION:
*  		this function is used to get the port's phy admin status.0x01 for disable,0x02 for enable.
*
*	INPUTS:
*		portId: 		portId
*		state:		enable:0x02,disable:0x01
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPhyAdminStateGet(UINT8 portId,UINT32 *state)
{
    OPL_STATUS retVal;
    UINT32 regVal;
    if(NULL == state)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }
    
    retVal = oplRegFieldRead(REG_GMAC_CFG_TRANSFER_ENA, 1, 1, &regVal);
	*state = regVal;
    return retVal;
}
/*******************************************************************************
* dalPhyAdminControlSet
*
* DESCRIPTION:
*  		this function is used to set the port'phy admin status,0x01 for disable,0x02 for enable
*
*	INPUTS:
*		portId: 		portId
*		state:		disable:0x01,enable:0x02
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPhyAdminControlSet(UINT8 portId,UINT32 state)
{
    UINT32 regVal;
    OPL_STATUS retVal;

    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

	regVal = state;

	retVal = oplRegFieldWrite(REG_GMAC_CFG_TRANSFER_ENA, 0, 1, regVal);
    if(OPL_OK != retVal)
    {
        return retVal;
    }

	retVal = oplRegFieldWrite(REG_GMAC_CFG_TRANSFER_ENA, 1, 1, regVal);
    if(OPL_OK != retVal)
    {
        return retVal;
    }

	if (!state){ 
		//printk("%s, flush the dynamic fdb\n", __FUNCTION__);
		dalArlFlushDynamic();
	}

	phyAdminStatus = state;
    return phyPowerDown(!state);
}
/*******************************************************************************
* dalPhyAutonegAdminStateGet
*
* DESCRIPTION:
*  		this function is used to get autonegocitation config info for the port.0x01 for disable,0x02 for enable
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		state:		disable:0x01,enable:0x02
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPhyAutonegAdminStateGet(UINT8 portId,UINT32 *state)
{
    UINT8 enable;
    OPL_STATUS retVal;
    if(NULL == state)
    {
        return OPL_ERR_NULL_POINTER;
    }
    
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

    retVal = phyAutoNegHwRead(&enable);
	if (enable)
	{
		*state = 0x2;
	}
	else
	{
		*state = 0x1;
	}
    return retVal;
}
/*******************************************************************************
* dalPhyAutonegEnableSet
*
* DESCRIPTION:
*  		this function is used to set autonegocitation enable. 0x01 for dsiable,0x02 for enable
*
*	INPUTS:
*		portId: 		portId
*		enable:		disable:0x01;eanble:0x02
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPhyAutonegEnableSet(UINT8 portId,UINT32 enable)
{
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }
 #ifdef ONU_RESPIN_A   /* for Respin RGMII AR8035 Single port only*/
    OPL_STATUS retVal = OPL_OK;
 
    if(enable)
		retVal = oplRegWrite(REG_GMAC_MODE, 0x1);
	else
		retVal = oplRegWrite(REG_GMAC_MODE, 0x5); 
 #endif
    return phyAutoNegHwWrite(enable); 
}
/*******************************************************************************
* dalPhyAutonegRestart
*
* DESCRIPTION:
*  		this function is used to force the port restart autonegocitation.
*
*	INPUTS:
*		portId: 		portId
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalPhyAutonegRestart(UINT8 portId)
{
    if(dalPortNumIsInvalid(portId))
    {
        OPL_LOG_TRACE();
        return OPL_ERR_INVALID_PARAMETERS;
    }

    return phyRestartAutoNeg();
}

/*******************************************************************************
* dalPortInit
*
* DESCRIPTION:
*  		this function is used to init the port parameter.
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
OPL_STATUS dalPortInit(void)
{
	OPL_STATUS retVal = OPL_OK;
	BRG_PORT_TAB_GE_t gePortEntry;
	BRG_PORT_TAB_PON_t ponPortEntry;
	
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	gePortEntry.rcv_en = OPL_ENABLE;
	gePortEntry.lern = OPL_DISABLE;
    /* begin added by jiangmingli for bug 1557 */
	gePortEntry.lern_drop = OPL_DISABLE;
	gePortEntry.lern_limit = OPL_DISABLE;
    /* end added by jiangmingli for bug 1557 */
	gePortEntry.lp = OPL_DISABLE;	
	gePortEntry.rsv_en = OPL_ENABLE;
	gePortEntry.cos = 0;
	gePortEntry.pvid = 1;
	gePortEntry.i_action = OPL_DISABLE;
	gePortEntry.i_cos = 0;
	gePortEntry.untag = OPL_ENABLE;
	gePortEntry.tag = OPL_ENABLE;
	gePortEntry.trsmt_en = OPL_ENABLE;

	retVal = brgPortEntryHwWrite(BRG_PORT_GE,(UINT32 *)&gePortEntry);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	ponPortEntry.rcv_en = OPL_ENABLE;
	ponPortEntry.lern = OPL_DISABLE;
    /* begin added by jiangmingli for bug 1557 */
	ponPortEntry.lern_drop = OPL_DISABLE;
	ponPortEntry.lern_limit = OPL_DISABLE;
    /* end added by jiangmingli for bug 1557 */
	ponPortEntry.lp = OPL_DISABLE;	
	ponPortEntry.rsv_en = OPL_ENABLE;
	ponPortEntry.cos = 0;
	ponPortEntry.pvid = 1;
	ponPortEntry.i_action = OPL_DISABLE;
	ponPortEntry.i_cos = 0;
	ponPortEntry.untag = OPL_ENABLE;
	ponPortEntry.tag = OPL_ENABLE;
	ponPortEntry.trsmt_en = OPL_ENABLE;
	retVal = brgPortEntryHwWrite(BRG_PORT_PON,(UINT32 *)&ponPortEntry);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

    retVal = oplRegWrite(REG_GMAC_CFG_LENMENA,0);
	if(OPL_OK != retVal)
	{
		return retVal;
	}


	retVal = oplRegWrite(REG_PMAC_CFG_LENMENA,0);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	return OPL_OK;
}


