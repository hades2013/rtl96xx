/*
=============================================================================
     Header Name: dal_port.h

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
#ifndef DAL_PORT_H
#define DAL_PORT_H

#include "opl_driver.h"
#include "opconn_api.h"

#define DAL_PORT_ADMIN_ENABLE   (1)
#define DAL_PORT_ADMIN_DISABLE  (0)

#define PHY_PORT_ADMIN_ENABLE	  (1)
#define PHY_PORT_ADMIN_DISABLE (0)

#define DAL_MAX_NUM_OF_PORT         (1)

typedef enum DAL_PORT_SPEED_s{
    DAL_10_MBPS     = 10,
    DAL_100_MBPS    = 100,
    DAL_1000_MBPS   = 1000,
    DAL_END_MBSP
}DAL_PORT_SPEED_e;

typedef enum DAL_DUPLEX_MODE_s
{
	DAL_HALF_DUPLEX = 0X01,
	DAL_FULL_DUPLEX  = 0X02	
}DAL_DUPLEX_MODE_e;

typedef enum DAL_AUTONEG_DONE_s
{
	DAL_AUTONEG_FAILED = 0X00,
	DAL_AUTONEG_COMPLETED  = 0X01	
}DAL_AUTONEG_DONE_e;

OPL_STATUS dalPortNumIsInvalid(UINT8 portId);
OPL_STATUS dalPortSpecificDuplexRead(UINT8 portId,UINT32 *duplex);
OPL_STATUS dalPhySpecificAutonegRead(UINT8 portId, UINT32 *state);
OPL_STATUS dalPortSpecificSpeedRead(UINT8 portId,UINT32 *portSpeed);

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
OPL_STATUS dalPortStateGet(UINT8 portId,UINT8 *status);

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
OPL_STATUS dalPortSpeedSet(UINT8 portId,UINT32 portSpeed);


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
OPL_STATUS dalPortSpeedGet(UINT8 portId,UINT32 *portSpeed);

	
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
OPL_STATUS dalPortDuplexSet(UINT8 portId,UINT32 duplex);


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
OPL_STATUS dalPortDuplexGet(UINT8 portId,UINT32 *duplex);

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
OPL_STATUS dalPortFlowcontrolSet(UINT8 portId,UINT8 enable);

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
OPL_STATUS dalPortFlowcontrolGet(UINT8 portId,UINT8 *enable);

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
OPL_STATUS dalPortLoopbackSet(UINT8 portId,UINT8 enable);
/*********************************************************************************
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
OPL_STATUS dalPortUsPolicingEnableSet(UINT8 portId,UINT8 enable);


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
OPL_STATUS dalPortUsPolicingCirSet(UINT8 portId,UINT32 cir);

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
OPL_STATUS dalPortUsPolicingCirGet(UINT8 portId,UINT32 *cir);


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
OPL_STATUS dalPortUsPolicingCbsSet(UINT8 portId,UINT32 cbs);


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
OPL_STATUS dalPortUsPolicingCbsGet(UINT8 portId,UINT32 *cbs);

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
OPL_STATUS dalPortUsPolicingEbsSet(UINT8 portId,UINT32 ebs);


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
OPL_STATUS dalPortUsPolicingEbsGet(UINT8 portId,UINT32 *ebs);

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
OPL_STATUS dalPortUsPolicingSet(UINT8 portId,UINT8 enable,UINT32 cir,UINT32 cbs,UINT32 ebs);

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
OPL_STATUS dalPortUsPolicingGet(UINT8 portId,UINT8 *enable,UINT32 *cir,UINT32 *cbs,UINT32 *ebs);

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
OPL_STATUS dalPortDsPolicingEnableSet(UINT8 portId,UINT8 enable);

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
OPL_STATUS dalPortDsPolicingEnableGet(UINT8 portId,UINT8 *enable);

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
OPL_STATUS dalPortDsPolicingCirSet(UINT8 portId,UINT32 cir);

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
OPL_STATUS dalPortDsPolicingCirGet(UINT8 portId,UINT32 *cir);

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
OPL_STATUS dalPortDsPolicingPirSet(UINT8 portId,UINT32 pir);


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
OPL_STATUS dalPortDsPolicingPirGet(UINT8 portId,UINT32 *pir);


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
OPL_STATUS dalPortDsPolicingSet(UINT8 portId,UINT8 enable,UINT32 cir,UINT32 pir);

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
OPL_STATUS dalPortDsPolicingGet(UINT8 portId,UINT8 *enable,UINT32 *cir,UINT32 *pir);

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
OPL_STATUS dalPhyAdminStateGet(UINT8 portId,UINT32 *state);

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
OPL_STATUS dalPhyAdminControlSet(UINT8 portId,UINT32 state);

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
OPL_STATUS dalPhyAutonegAdminStateGet(UINT8 portId,UINT32 *state);

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
OPL_STATUS dalPhyAutonegEnableSet(UINT8 portId,UINT32 enable);

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
OPL_STATUS dalPhyAutonegRestart(UINT8 portId);

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
OPL_STATUS dalPortInit(void);

OPL_STATUS dalPortPvidSet(UINT8 portId,UINT16 pvid);

OPL_STATUS dalPortPvidGet(UINT8 portId,UINT16 *pvid);

OPL_STATUS dalPortVlanModeSet(UINT8 portId,UINT8 mode);

OPL_STATUS dalPortVlanModeGet(UINT8 portId,UINT8 *mode);

OPL_STATUS dalPortDefaultPriSet(UINT8 portId,UINT8 defaultPri);

OPL_STATUS dalPortDefaultPriGet(UINT8 portId,UINT8 *defaultPri);



#endif
