/*
=============================================================================
     Header Name: odm_fdb.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 jiangmingli	   2008/08/28		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#ifndef  __ODM_FDB_H__
#define __ODM_FDB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <vos.h>
#include "opl_driver.h"

#define FDB_FILENAME_CFG		"/cfg/fdb.conf"

/* section for aging time */
#define FDB_SECTION_AGINGTIME	"AgingTime"
#define FDB_KEY_VALUE_AGINGTIME "AgingTime Value"
/*section for soft-learning */
#define FDB_SECTION_SOFT_LEARNING	"SoftLearning"
#define FDB_KEY_VALUE_SOFT_LEARNING "SoftLearning Value"

/* section for mac discard */
#define FDB_SECTION_MAC_DISCARD	"Mac Discard"

/* key for mac entry */
#define FDB_KEY_VALUE_MAC_ADDR "Mac Address"
#define FDB_KEY_U32_PORT_BITMAP "Port Bitmap"
#define FDB_KEY_U32_VLANID "vlanid"

/* definition for mac filter */
#define FDB_SEC_MAC_FILTER_INFO   "port_%d_filter_info"
#define FDB_KEY_MAC_FITLER_NUM    "num" 

#define FDB_SEC_MAC_FILTER_ENTRY  "port_%d_filter_%d"
#define FDB_KEY_MAC_FILTER_ADDR   "mac"
#define FDB_KEY_MAC_FILTER_VID    "vid"
#define FDB_KEY_MAC_FILTER_PORD   "port"

#define ODM_MAX_NUM_OF_MAC_FILTER	16

/* definition for static mac */
#define FDB_SEC_STATIC_MAC_INFO   "port_%d_static_mac_info"
#define FDB_KEY_STATIC_MAC_NUM    "num"

#define FDB_SEC_STATIC_MAC_ENTRY  "port_%d_static_mac_%d"
#define FDB_KEY_STATIC_MAC_ADDR   "mac"
#define FDB_KEY_STATIC_MAC_VID    "vid"

#define ODM_MAX_NUM_OF_STATIC_MAC   16

/* definition for bind mac */
#define FDB_SEC_BIND_MAC_INFO   "port_%d_bind_mac_info"
#define FDB_KEY_BIND_MAC_NUM    "num"

#define FDB_SEC_BIND_MAC_ENTRY  "port_%d_bind_mac_%d"
#define FDB_KEY_BIND_MAC_ADDR   "mac"
#define FDB_KEY_BIND_MAC_VID    "vid"

#if defined(ONU_1PORT)
#define ODM_MAX_NUM_OF_BIND_MAC   64
#elif defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
#define ODM_MAX_NUM_OF_BIND_MAC   16
#else
#define ODM_MAX_NUM_OF_BIND_MAC   0
#endif


#ifndef INOUT
#define IN      /* consider as comments near 'input' parameters */
#define OUT     /* consider as comments near 'output' parameters */
#define INOUT   /* consider as comments near 'input/output' parameters */
#endif

/*******************************************************************************
* odmFdbMacVlan2Sec
*
* DESCRIPTION:
*  		transform mac entry section for fdb configure file
*
*	INPUTS:
*		aucMacAddress	mac address
*		usVlanId			vlan id
*	OUTPUTS:
*		szMacVlanSec		mac entry section
*		     
* 	RETURNS:
* 		NO_ERROR									success
*		FDB_MAC_AGINGTIME_SET_ERROR				set aging time error
* 		FDB_MAC_CFG_FIEL_AGINGTIME_SET_ERROR		set config file error
* 	SEE ALSO: 
*/
VOID odmFdbMacVlan2Sec(IN UINT8 *aucMacAddress, IN UINT16 usVlanId, OUT UINT8 *szMacVlanSec);

/*******************************************************************************
* odmFdbAgingTimeSet
*
* DESCRIPTION:
*		CLI callback function for "agingtime <0-120>"
*  		set the automatic address aging time
*
*	INPUTS:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 			OPL_OK 		success
* 			OPL_ERROR	failed
* 	SEE ALSO: 
*/
OPL_STATUS odmFdbAgingTimeSet(IN UINT32 uiAgingTime);

/*******************************************************************************
* odmFdbAgingTimeGet
*
* DESCRIPTION:
*		get the automatic address aging time from hw
*
*	INPUTS:
*
*	OUTPUTS:
*		puiAgingTime		the aging time got from hw     
* 	RETURNS:
* 		NO_ERROR								success
* 		FDB_MAC_GET_AGINGTIME_ERROR			get aging time error
* 	SEE ALSO: 
*/
STATUS odmFdbAgingTimeGet(OUT UINT32 *puiAgingTime);

/*******************************************************************************
* odmFdbSetDefault
*
* DESCRIPTION:
*		set the default value for fdb, while initial or restore
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR									success
*		FDB_MAC_SET_AGINGTIME_ERROR				set aging time error
* 		FDB_MAC_SET_CONFIG_FIEL_AGINGTIME_ERROR	set config file error
* 	SEE ALSO: 
*/
STATUS odmFdbSetDefault(VOID);

/*******************************************************************************
* odmFdbMacAdd
*
* DESCRIPTION:
*		add an static mac entry with single port
*		if need to add an static mac entry with multiple ports, please use function odmFdbMacPortAdd
*	INPUTS:
*		usPortId			the single port associated with the mac
*		aucMacAddress	the mac address needed to add
*		usVlanId			the vlanid associated with the mac
*	OUTPUTS:
*		  
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_ADD_ERROR			add mac entry error
* 	SEE ALSO: 
*/
STATUS odmFdbMacAdd(IN UINT16 usPortId, IN UINT8 *aucMacAddress, IN UINT16 usVlanId);	

/*******************************************************************************
* odmFdbMacPortAdd
*
* DESCRIPTION:
*		if the mac entry not exists, this function will add an static mac entry with  desired multiple ports
*		else add the desired multiple ports to the existed mac entry
*	INPUTS:
*		uiPortNum		Port number for Port List 
*		auiPortlist		the Port List associated with the mac
*		aucMacAddress	the mac address needed to add
*		usVlanId			the vlanid associated with the mac
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_PORT_ADD_ERROR	add multiple ports error
* 	SEE ALSO: 
*/
STATUS odmFdbMacPortAdd
(
	IN UINT32 uiPortNum,
	IN UINT32 *auiPortlist,
	IN UINT8 *aucMacAddress,
	IN UINT16 usVlanId
);

/*******************************************************************************
* odmFdbMacDel
*
* DESCRIPTION:
*		delete the desired mac entry, if the mac entry not exists, this function will return ok
*	INPUTS:
*		aucMacAddress	the mac address needed to delete
*		usVlanId			the vlanid associated with the mac
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_ADD_ERROR			add mac entry error
* 	SEE ALSO: 
*/
STATUS odmFdbMacDel(IN UINT8 *aucMacAddress, IN UINT16 usVlanId);

/*******************************************************************************
* odmFdbMacClearAll
*
* DESCRIPTION:
*		delete all mac entry
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_DEL_ALL_ERROR		delete all mac entry error
* 	SEE ALSO: 
*/
STATUS odmFdbMacClearAll(VOID);

/*******************************************************************************
* odmFdbMacClearAll
*
* DESCRIPTION:
*		delete all mac entry
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_DEL_ALL_ERROR		delete all mac entry error
* 	SEE ALSO: 
*/
STATUS odmFdbMacClearAll(VOID);

/*******************************************************************************
* odmFdbMacPortDel
*
* DESCRIPTION:
*		del the desired multiple ports to the existed mac entry
*		if the mac entry not exists, this function will return ok
*	INPUTS:
*		uiPortNum		Port number for Port List 
*		auiPortlist		the Port List associated with the mac
*		aucMacAddress	the mac address needed to delete
*		usVlanId			the vlanid associated with the mac
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_PORT_DEL_ERROR	del multiple ports error
* 	SEE ALSO: 
*/
STATUS odmFdbMacPortDel
(
	IN UINT32 uiPortNum,
	IN UINT32 *auiPortlist,
	IN UINT8 *aucMacAddress,
	IN UINT16 usVlanId
);

/*******************************************************************************
* odmFdbMacDiscard
*
* DESCRIPTION:
*		set the portlist associated with the mac entry 0
*		if the mac entry not exists, this function will add an static mac entry with portlis 0
*	INPUTS:
*		aucMacAddress	the mac address needed to delete
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_DISCARD_ERROR		discard mac entry error
* 	SEE ALSO: 
*/
STATUS odmFdbMacDiscard(IN UINT8 *aucMacAddress);

/*******************************************************************************
* odmFdbMacDiscardClear
*
* DESCRIPTION:
*		don't discard the packets, who source mac address is aucMacAddress
*	INPUTS:
*		aucMacAddress	the mac address needed to delete
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR						success
* 		FDB_MAC_DISCARD_CLEAR_ERROR	clear discard mac entry error
* 	SEE ALSO: 
*/
STATUS odmFdbMacDiscardClear(IN UINT8 *aucMacAddress);

/*******************************************************************************
* odmFdbMacShowOne
*
* DESCRIPTION:
*		show the information of desired mac entry
*		if the mac entry not exists, this function will print "this mac entry exists"
*	INPUTS:
*		lFd				WriteFd of CLI Env
*		aucMacAddress	the mac address needed to delete
*		usType			FDB_MAC_STATIC	for static
*						FDB_MAC_DYNAMIC for dynamic
*						FDB_MAC_TYPE_MAX for not care
*		uiPortNum	Port number for Port List 
*					0 for not care
*		auiPortlist	the Port List associated with the mac
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_SHOW_ONE_ERROR	show one mac entry error
* 	SEE ALSO: 
*/
STATUS odmFdbMacShowOne
(
	IN INT32 lFd,
	IN UINT8 *aucMacAddress,
	IN UINT16 usType,
	IN UINT32 uiPortNum,
	IN UINT32 *auiPortlist
);

/*******************************************************************************
* odmFdbMacShowAll
*
* DESCRIPTION:
*		show the information of all mac entry
*	INPUTS:
*		lFd				WriteFd of CLI Env
*		usType			FDB_MAC_STATIC	for static
*						FDB_MAC_DYNAMIC for dynamic
*						FDB_MAC_TYPE_MAX for not care
*		uiPortNum	Port number for Port List 
*					0 for not care
*		auiPortlist	the Port List associated with the mac
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_SHOW_ONE_ERROR	show one mac entry error
* 	SEE ALSO: 
*/
STATUS odmFdbMacShowAll
(
	IN INT32 lFd,
	IN UINT16 usType,
	IN UINT32 uiPortNum,
	IN UINT32 *auiPortlist
);

/*******************************************************************************
* odmFdbCfgShow
*
* DESCRIPTION:
*		show the information of fdb configure file
*	INPUTS:
*		lFd				WriteFd of CLI Env
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success
* 	SEE ALSO: 
*/
STATUS odmFdbCfgShow(IN INT32 lFd);

/*******************************************************************************
* odmFdbSwLearnSet
*
* DESCRIPTION:
*		enable/disable fdb software learning
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					        success
* 		FDB_MAC_SOFT_LEARN_SET_ERROR	 enable/disable fdb software learning error
* 	SEE ALSO: 
*/
STATUS odmFdbSwLearnSet(BOOL_T bEnable);

/*******************************************************************************
* odmFdbSwLearnGet
*
* DESCRIPTION:
*		get fdb software learning state
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					        success
* 		FDB_MAC_SOFT_LEARN_GET_ERROR	 get fdb software learning state error
* 	SEE ALSO: 
*/
STATUS odmFdbSwLearnGet(BOOL_T *pbEnable);

UINT32 odmPortMacFilterNumGet(UINT32 portId, UINT8 *pEntryNum);
UINT32 odmPortMacFilterNumSet(UINT32 portId, UINT8 entryNum);
UINT32 odmPortMacFilterEntryAdd(UINT32 portId, UINT16 vlanId, UINT8 *pMacAddr);
UINT32 odmPortMacFilterEntryDel(UINT32 portId, UINT16 vlanId, UINT8 *pMacAddr);

UINT32 odmPortBindMacNumGet(UINT32 portId, UINT8 *pEntryNum);
UINT32 odmPortBindMacNumSet(UINT32 portId, UINT8 entryNum);
UINT32 odmPortBindMacEntryAdd(UINT32 portId, UINT16 vlanId, UINT8 *pMacAddr);
UINT32 odmPortBindMacEntryDel(UINT32 portId, UINT16 vlanId, UINT8 *pMacAddr);
UINT32 odmPortBindMacEntryGet(UINT32 portId, UINT32 entryIndex, UINT16 *pVlanId, UINT8 *pMacAddr);
UINT32 odmPortBindMacEntryClear(UINT32 portId);
UINT32 odmPortMacFilterEntryGet(UINT32 portId, UINT32 entryIndex, UINT16 *pVlanId, UINT8 *pMacAddr);
UINT32 odmPortMacFilterEntryClear(UINT32 portId);

UINT32 odmPortStaticMacNumGet(UINT32 portId, UINT8 *pEntryNum);
UINT32 odmPortStaticMacNumSet(UINT32 portId, UINT8 entryNum);
UINT32 odmPortStaticMacEntryAdd(UINT32 portId, UINT16 vlanId, UINT8 *pMacAddr);
UINT32 odmPortStaticMacEntryDel(UINT32 portId, UINT16 vlanId, UINT8 *pMacAddr);
UINT32 odmPortStaticMacEntryGet(UINT32 portId, UINT32 entryIndex, UINT16 *pVlanId, UINT8 *pMacAddr);
UINT32 odmPortStaticMacEntryClear(UINT32 portId);

#ifdef __cplusplus
}
#endif

#endif

