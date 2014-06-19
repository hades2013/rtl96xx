/*
=============================================================================
     Header Name: opconn_api.h

     General Description:
     this file include all api for uplayer
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/10/30		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifndef OPCONN_API_H
#define OPCONN_API_H
#include "bridgeport.h"
#include "bridgevlan.h"
#include "bridgemac.h"
#include "classify.h"
#include "pon.h"
#include "tm.h"

/*******************************************************************************
* brgVlanModeSet
*
* DESCRIPTION:
*  		this function is used to set the vlan mode :
*			0x0: transparent mode
*			0x1: tag mode
*			0x2: translation mode
*
*	INPUTS:
*			mode:0x00,0x01,0x02
*	OUTPUTS:
*		   
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgVlanModeSet(UINT8 mode) ;

/*******************************************************************************
* brgVlanModeGet
*
* DESCRIPTION:
*  		this function is used to set the vlan mode :
*			0x0: transparent mode
*			0x1: tag mode
*			0x2: translation mode
*
*	INPUTS:
*			
*	OUTPUTS:
*		   mode:0x01,0x02,0x00
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgVlanModeGet(UINT8 mode);


/*******************************************************************************
* brgPortLinkStatusGet
*
* DESCRIPTION:
*  		this function is used to get the ether port link status,define by CTC.
*
*	INPUTS:
*			portNum:
*	OUTPUTS:
*		   linkStatus:0X00:power down,0x01:power on
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
/*
#define brg_port_link_status_get(portNum,linkStatus) 
*/
OPL_STATUS brgPortLinkStatusGet(UINT8 portNum,UINT8 *linkStatus);

/*******************************************************************************
* brgPortFlowControlSet
*
* DESCRIPTION:
*  		this function is used to disabe/enable the ether port flowcontrol,0 for disable while 1 for enable
*
*	INPUTS:
*			portNum:
*			enable:0x0:disable,0x01:enable
*	OUTPUTS:
*		   
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgPortFlowControlSet(UINT8 portNum,UINT8 enable);

/*******************************************************************************
* brgPortFlowControlGet
*
* DESCRIPTION:
*  		this function is used to get port flow control status  .
*	INPUTS:
*			portNum:
*		
*	OUTPUTS:
*		   	enable:0x0:disable,0x01:enable
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgPortFlowControlGet(UINT8 portNum,UINT8 *enable);

/*******************************************************************************
* brgPortPolicingSet
*
* DESCRIPTION:
*  		this function is used set the ether port ingress policing,include PORT policing disable/enable,
*			port policing CIR,CBS,EBS.
*
*	INPUTS:
*			portNum:
*			enable:
*			portCir:
*			portCbs:
*			portEbs:
*	OUTPUTS:
*		   
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgPortPolicingSet(UINT8 portNum,UINT8 enable,UINT16 portCir,UINT16 portCbs,UINT16 portEbs);

/*******************************************************************************
* brgPortPolicingGet
*
* DESCRIPTION:
*  		this function is used get the ether port ingress policing,include PORT policing disable/enable,
*			port policing CIR,CBS,EBS.
*
*	INPUTS:
*			portNum:
*			
*	OUTPUTS:
*			enable:
*			portCir:
*			portCbs:
*			portEbs:	   
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgPortPolicingGet(UINT8 portNum,UINT8 *enable,UINT16 *portCir,UINT16 *portCbs,UINT16 *portEbs);


/*******************************************************************************
* brgPortAdminSet
*
* DESCRIPTION:
*  		this function is used to disable or enable the ethernet port
*
*	INPUTS:
*			portNum:
*	OUTPUTS:
*		   enable:0x02:disable,0x01
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgPortAdminSet(UINT8 portNum, UINT8 active);

/*******************************************************************************
* brgPortAdminGet
*
* DESCRIPTION:
*  		this function is used to get the port infor (enable or disable)
*
*	INPUTS:
*			portNum:
*	OUTPUTS:
*		   enable:0x02:disable,0x01
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgPortAdminGet(UINT8 portNum, UINT8 *active);

/*******************************************************************************
* brgPortPhyAdminSet
*
* DESCRIPTION:
*  		this function is used to deactivate the ethernet port or activate the ethernet port.
*			0x00000001:   deactivate
*			0x00000002:	activate
*
*	INPUTS:
*			portNum:
*			active:1:deactive,2:active
*	OUTPUTS:
*		   
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgPortPhyAdminSet(UINT8 portNum,UINT8 active);

/*******************************************************************************
* brgPortPhyAdminGet
*
* DESCRIPTION:
*  		this function is used to get the port phy status(active or not).
*			0x00000001:   deactivate
*			0x00000002:	activate
*
*	INPUTS:
*			portNum:
*			active:1:deactive,2:active
*	OUTPUTS:
*		   active:1:deactive,2:active
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgPortPhyAdminGet(UINT8 portNum,UINT8 *active);

/*******************************************************************************
* brgPortAutonegSet
*
* DESCRIPTION:
*  		this function is used to dsiable or enable the auto negocitation .
*			0x00000001:   disable
*			0x00000002:	 enable
*
*	INPUTS:
*			portNum:
*			enable:1:diable,2:enable
*	OUTPUTS:
*		  
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgPortAutonegSet(UINT8 portNum,UINT8 enable);

/*******************************************************************************
* brgPortAutonegGet
*
* DESCRIPTION:
*  		this function is used to get the auto negociation disable or enable
*			0x00000001:   disable
*			0x00000002:	 enable
*
*	INPUTS:
*			portNum:
*			
*	OUTPUTS:
*		  enable:1:diable,2:enable
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgPortAutonegGet(UINT8 portNum,UINT8 *enable);

/*******************************************************************************
* brgPortAutonegAbilityGet
*
* DESCRIPTION:
*  		this function is used to get the device autonegociation ability 
*			0x028:1000BASE-T UTP PHY
*			0x192:Full duplex 1000 BASE-T UTP PHY
*			0x142:Full duplex 100 BASE-T2 
*
*	INPUTS:
*			portNum:
*			
*	OUTPUTS:
*		  capability
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgPortAutonegAbilityGet(UINT8 portNum,UINT16 *capability);

/*******************************************************************************
* brgPortAutonegRestartSet
*
* DESCRIPTION:
*  		this function is used to restart the autonegociation of the specified 
*
*	INPUTS:
*			portNum:
*			
*	OUTPUTS:
*		  
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgPortAutonegRestartSet(UINT8 portNum);

/*******************************************************************************
* brgPortAutonegAdminSet
*
* DESCRIPTION:
*  		this function is used to enable the auto negociation 
*
*	INPUTS:
*			portNum:
*			enable
*	OUTPUTS:
*		  
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgPortAutonegAdminSet(UINT8 portNum,UINT8 enable);

/*******************************************************************************
* brgPortAutonegAdminGet
*
* DESCRIPTION:
*  		this function is used to get the autonegociation admin status
*
*	INPUTS:
*			portNum:
*			
*	OUTPUTS:
*		  enable
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS brgPortAutonegAdminGet(UINT8 portNum,UINT8 *enable);

#endif
