/*
=============================================================================
     Header Name: odm_rstp.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 jiangmingli	   2008/10/23		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#ifndef  __ODM_RSTP_H__
#define __ODM_RSTP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <vos.h>
#include "opl_debug.h"


/**************** rstp configure file elements define ***************************/

/* module for rstp */
#define CFGFILE_RSTP "/cfg/rstp.conf"

/* section for rstp */
#define RSTP_SECTION_BRD		"RSTP"

/* value for enable and disble */
#define RSTP_VALUE_ENABLE	"enable"
#define RSTP_VALUE_DISABLE	"disable"

/* default value for rstp */
#define RSTP_BRD_DEFAULT_PRI			8		/* default bridge priority */
#define RSTP_PORT_DEFAULT_PRI			8		/* default port priority */
#define RSTP_BRD_DEFAULT_HELLOTIME		2		/* default bridge hellotime */
#define RSTP_BRD_DEFAULT_FWDDELAYTIME	15		/* default bridge forward delay time */
#define RSTP_BRD_DEFAULT_MAXAGE			20		/* default bridge max age */
#define RSTP_BRD_DEFAULT_FORCEVERSION	2		/* default bridge force version */
#define RSTP_BRD_DEFAULT_DIAMETER		7		/* default bridge diameter */
#define RSTP_PORT_DEFAULT_PATH_COST		200000	/* default port path cost */
#define RSTP_PORT_DEFAULT_P2P_MODE		2		/* default port p2p mode auto */

#define RSTP_BRD_DEFAULT_HANDLE_MODE	0		/* default rstp packet handle mode: to cpu */

/* key for bridge mode */
#define RSTP_BRD_KEY_VALUE_MODE		"mode"

/* key for bridge hellotime */
#define RSTP_BRD_KEY_U32_HELLOTIME	"hello_time"

/* key for bridge maxage */
#define RSTP_BRD_KEY_U32_MAXAGE		"max_age"

/* key for bridge forwarddelay */
#define RSTP_BRD_KEY_U32_FWDDELAY	"forward_delay"

/* key for bridge force version */
#define RSTP_BRD_KEY_U32_VERSION	"force_version"

/* key for bridge priority */
#define RSTP_BRD_KEY_U32_PRI		"bridge_priority"

/* key for bridge diameter */
#define RSTP_BRD_KEY_U32_DIAMETER	"diameter"

/* key for bridge handle mode */
#define RSTP_BRD_KEY_U32_HANDLE_MODE	"handle-mode"

/* key for port p2plink */
#define RSTP_PORT_KEY_U32_P2P		"p2plink"

/* key for port mode */
#define RSTP_PORT_KEY_VALUE_MODE	"mode"

/* key for port edge */
#define RSTP_PORT_KEY_VALUE_EDGE	"edge_port"

/* key for port priority */
#define RSTP_PORT_KEY_U32_PRI		"priority"

/* key for port path cost */
#define RSTP_PORT_KEY_U32_PCOST		"path_cost"

/****************************************************************************/


/***************************** type define **********************************/

/* rstp bridge info type */
typedef enum tagRSTP_BRD_ELEMENT
{
	RSTP_BRD_MODE,
	RSTP_BRD_PRI,
	RSTP_BRD_MAXAGE,
	RSTP_BRD_HELLO,
	RSTP_BRD_FWDDELAY,
	RSTP_BRD_VER,
	RSTP_BRD_DIAMETER
}RSTP_INFO_ELEMENT_E;

/* rstp port info type */
typedef enum tagRSTP_PORT_ELEMENT
{
	RSTP_PORT_MODE,
	RSTP_PORT_PCOST,
	RSTP_PORT_PRI,
	RSTP_PORT_EDGE,
	RSTP_PORT_P2P
}RSTP_PORT_ELEMENT_E;

/****************************************************************************/


/************************ function declare *************************************/

void odmShowRstpPort (int lFd, int detail);
STATUS odmRstpEnable (int enable);
void odmRstpInfoShow(int lFd);
void odmRstpConfShow (int lFd);
STATUS odmRstpSetHello(unsigned int data);
STATUS odmRstpSetForward(unsigned int data);
STATUS odmRstpSetMaxage(unsigned int data);
STATUS odmRstpSetForver(unsigned int data);
STATUS odmRstpSetPriority(unsigned int data);
STATUS odmRstpSetDiameter(unsigned int data);
STATUS odmRstpPortEnable(unsigned long ulPortBitmap, int enable);
STATUS odmRstpEdgePortEnable(unsigned long ulPortBitmap, int enable);
STATUS odmRstpP2plinkMode(unsigned long ulPortBitmap, int data);
STATUS odmRstpSetPathCost(unsigned long ulPortBitmap, unsigned int data);
STATUS odmRstpSetPortPriority(unsigned long ulPortBitmap, unsigned int data);
STATUS odmRstpBrdInfoGet(RSTP_INFO_ELEMENT_E enType, INT32 *pulBrdInfo);
STATUS odmRstpPortInfoGet(UINT32 ulPortId, RSTP_INFO_ELEMENT_E enType, INT32 *pulPortInfo);
STATUS odmRstpPortTransStatGet(UINT32 ulPortId, UINT8 *pucPortState);

/* begin added by jiangmingli for zte */
STATUS odmRstpPktHandleModeSet(UINT32 ulMode);
STATUS odmRstpPktHandleModeGet(UINT32 *pulMode);
/* end added by jiangmingli for zte */

/****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif


