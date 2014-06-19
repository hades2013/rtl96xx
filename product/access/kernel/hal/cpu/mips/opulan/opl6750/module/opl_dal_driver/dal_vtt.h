/*
=============================================================================
Header Name:dal_vtt.h

  General Description:
  
	This file define all opcomm chip regisger and table address.
	===============================================================================
	Opulan Confidential Proprietary                     
	ID and version: xxxxxxxxxxxxxx  Version 1.00
	(c) Copyright Opulan XXXX - XXXX, All Rights Reserved
	
	  
		Revision History:
		Modification
		Author			Date				Description of Changes
		---------------	---------------	----------------------------------------------
		zzhu			2007/11/09		Initial Version
		---------------	---------------	----------------------------------------------
*/
#ifndef DAL_VTT_H
#define DAL_VTT_H

#define DAL_VTT_REC_INVALID				        (0)
#define DAL_VTT_REC_VALID					    (1)
#define DAL_VTT_FULL							(-100)

#define DAL_VTT_PORT_DEFAULT				    (0)   
#define DAL_VTT_PON_PORT_STRIP			        (1)
#define DAL_VTT_GE_PORT_STRIP			        (1)

#define DAL_TAG_STRIP_EN						(1)
#define DAL_TAG_STRIP_DIS						(0)
#define DAL_VTT_LOOKUPFAIL_BY_PASS		        (1)
#define DAL_VTT_LOOKUPFAIL_DROP				    (0)

#define DAL_PORT_DEFAULT_PVID					(1)

#define DAL_PORT_PVID_VTT_ENTRY_START_NUM       (11)
#define DAL_PORT_PVID_VTT_ENTRY_NUM             (1)

#define DAL_PORT_VTT_ENTRY_START_NUM            (0)
#define DAL_PORT_VTT_ENTRY_NUM                  (11)

#define DAL_PORT_MCAST_ENTRY_START_NUM          (12)
#define DAL_PORT_MCAST_ENTRY_NUM                (4)

#pragma pack(1)		/* pragma pack definition */

typedef enum DAL_VTT_TYPE_s
{
	DAL_VTT_UC = 0, /* used for  mc */
	DAL_VTT_MC = 1,	/* used for  mc */
	DAL_VTT_END
}DAL_VTT_TYPE_e;

typedef struct VTT_RECORD_s
{
	UINT32 sVlanInfo;
	UINT32 cVlanInfo;
	UINT8	valid;
}VTT_RECORD_t;

typedef struct VLAN_CONFI_INFO_s
{
	UINT8 		vlanMode;
	UINT16 	    defaultVlanTag;
	UINT16 	    defaultTPID;
	UINT8   	defaultPRI;
	UINT8   	defaultCFI;
	UINT8		numOfVttRecords;
	VTT_RECORD_t vtt_record[DAL_PORT_VTT_ENTRY_NUM];
}VLAN_CONFI_INFO_t;

#pragma pack()

/*******************************************************************************
* dalVttModeSet
*
* DESCRIPTION:
*		opconn support three vlan mode,unmodified mode,tag mode,translation mode.
*	INPUT:
*		mode:0x00:transparent mode,0x01:tag mode,0x02:translation mode.
*
*		 TRANSPARENT mode
*      --------------------------------------------------------------------------------------
*		 direction				|   tag type       | action
*      --------------------------------------------------------------------------------------
*		 							|   with tag        |  forward the ethernet packet without an modification
*      upstream         -------------------------------------------------------------------------
*		 							|   without tag   |  forward the ethernet packet without an modification
*		---------------------------------------------------------------------------------------	
*		 							|   with tag        |  forward the ethernet packet without an modification
*      downstream     -------------------------------------------------------------------------
*		 							|   without tag   |  forward the ethernet packet without an modification
*     ---------------------------------------------------------------------------------------
*
*		 vlan tag mode
*      ---------------------------------------------------------------------------------------
*		 direction				|   tag type       | action
*      ---------------------------------------------------------------------------------------
*		 							|   with tag        |  DROP
*      upstream         -------------------------------------------------------------------------
*		 							|   without tag   |  tagged with default VID
*		---------------------------------------------------------------------------------------	
*		 							|   with tag        |  forward the ethernet packet and strip the tag
*      downstream     -------------------------------------------------------------------------
*		 							|   without tag   |  drop
*     ---------------------------------------------------------------------------------------
*
*		 vlan translation mode
*      ----------------------------------------------------------------------------------------
*		 direction				|   tag type       | action
*      ----------------------------------------------------------------------------------------
*		 							|   with tag        |  lookup the vlan translation table,if there is match entry then replace
*                           |                      |  the vid and forward,else drop the packet. 
*      upstream         ---------------------------------------------------------------------------
*		 							|   without tag   |  tagged with default VID
*		-----------------------------------------------------------------------------------------
*		 							|   with tag        |  lookup the vlan translation table, if there is match entry then replace
*									|                      |  the vid and forward,else drop the packet.		
*      downstream     ---------------------------------------------------------------------------
*		 							|   without tag   |  drop
*     -----------------------------------------------------------------------------------------
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttModeSet(UINT8 mode,UINT32 portId);

/*******************************************************************************
* dalVttModeGet
*
* DESCRIPTION:
*		opconn support three vlan mode,unmodified mode,tag mode,translation mode.
*
*	INPUT:
*		mode:0x00:transparent mode,0x01:tag mode,0x02:translation mode.
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttModeGet(UINT8 *mode,UINT32 portId);

/*******************************************************************************
* dalVttTpidSet
*
* DESCRIPTION:
*		this function is uset to set the tpid to check the vlan.
*	INPUT:
*		tpid:0x8100 ...
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttTpidSet(UINT16 tpid,UINT32 portId);

/*******************************************************************************
* dalVttTpidGet
*
* DESCRIPTION:
*		this function is uset to get the tpid to check the vlan.
*	INPUT:
*		tpid:0x8100 ...
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttTpidGet(UINT16 *tpid,UINT32 portId);

/*******************************************************************************
* dalVttVidSet
*
* DESCRIPTION:
*		this function is uset to set the default vlan id for untag packet.
*	INPUT:
*		pvid:1-4095
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttVidSet(UINT16 pvid,UINT32 portId);

/*******************************************************************************
* dalVttVidGet
*
* DESCRIPTION:
*		this function is uset to get the default vlan id for untag packet.
*	INPUT:
*
* OUTPUTS:
*   pvid:1-4095
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttVidGet(UINT16 *pvid,UINT32 portId);

/*******************************************************************************
* dalVttDefaultPriSet
*
* DESCRIPTION:
*		this function is uset to set the default ether priority  for untag packet.
*	INPUT:
*
* OUTPUTS:
*   etherPri:0-7
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttDefaultPriSet(UINT8  etherPri,UINT32 portId);

/*******************************************************************************
* dalVttDefaultPriGet
*
* DESCRIPTION:
*		this function is uset to get the default ether priority  for untag packet.
*	INPUT:
*
* OUTPUTS:
*		etherPri:0-7
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttDefaultPriGet(UINT8  *etherPri,UINT32 portId);

/*******************************************************************************
* dalVttTagEntryAdd
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag 
*  record ,the others is tag record.
*  this function is used to del a tag record for vlan translation table 
*
* INPUTS:
*        pStrip		: 	0:strip disable,1: strip enable   for pon port.
*        gStrip	 	:  0:strip disable,1: strip enable 	for ge port.
*			cValn   	:	0-4095:vid in the packet which received from the subscrible port
*        sVlan		:	0-4095:vid at system side.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttTagEntryAdd(UINT8 pStrip,UINT8 gStrip,UINT32 cVlan,UINT32 sVlan,UINT32 portId,UINT32 *pEntryIndex, UINT8 type);

/*******************************************************************************
* dalVttTagEntryGet
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag 
*  record ,the others is tag record.
*  this function is used to del a tag record for vlan translation table 
*
* INPUTS:
*        pStrip		: 	0:strip disable,1: strip enable   for pon port.
*        gStrip	 	:  0:strip disable,1: strip enable 	for ge port.
*			cValn   	:	0-4095:vid in the packet which received from the subscrible port
*        sVlan		:	0-4095:vid at system side.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttTagEntryGet(UINT8 ValidRecordIndex,UINT32 *cVlan,UINT32 *sVlan,UINT32 portId);

/*******************************************************************************
* dalVttTagEntryDel
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag 
*  record ,the others is tag record.
*  this function is used to del a tag record for vlan translation table 
*
* INPUTS:
*        pStrip		: 	0:strip disable,1: strip enable   for pon port.
*        gStrip	 	:  0:strip disable,1: strip enable 	for ge port.
*			cValn   	:	0-4095:vid in the packet which received from the subscrible port
*        sVlan		:	0-4095:vid at system side.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttTagEntryDel(UINT8 pStrip,UINT8 gStrip,UINT32 cVlan,UINT32 sVlan,UINT32 portId,UINT32 *pEntryIndex, UINT8 type);

/*******************************************************************************
* dalVttNumOfRecordGet
*
* DESCRIPTION:
*		this function is used to init the vlan translaion table. both the software database and hardware table.
*
* INPUTS:
*        pStrip		: 	0:strip disable,1: strip enable   for pon port.
*        gStrip	 	:  0:strip disable,1: strip enable 	for ge port.
*			cValn   	:	0-4095:vid in the packet which received from the subscrible port
*        sVlan		:	0-4095:vid at system side.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttNumOfRecordGet(UINT8 *numOfRecords,UINT32 portId);

/*******************************************************************************
* dalVttMulticastEntryAdd
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag 
*  record ,the others is tag record.
*  this function is used to del a tag record for vlan translation table 
*
* INPUTS:
*        pStrip		: 	0:strip disable,1: strip enable   for pon port.
*        gStrip	 	:  0:strip disable,1: strip enable 	for ge port.
*			cValn   	:	0-4095:vid in the packet which received from the subscrible port
*        sVlan		:	0-4095:vid at system side.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttMulticastEntryAdd(UINT8 pStrip,UINT8 gStrip,UINT32 cVlan,UINT32 sVlan,UINT32 portId);

/*******************************************************************************
* dalVttMulticastEntryDel
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag 
*  record ,the others is tag record.
*  this function is used to del a tag record for vlan translation table 
*
* INPUTS:
*        pStrip		: 	0:strip disable,1: strip enable   for pon port.
*        gStrip	 	:  0:strip disable,1: strip enable 	for ge port.
*			cValn   	:	0-4095:vid in the packet which received from the subscrible port
*        sVlan		:	0-4095:vid at system side.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttMulticastEntryDel(UINT8 pStrip,UINT8 gStrip,UINT32 cVlan,UINT32 sVlan,UINT32 portId);

/*******************************************************************************
* dalVttMulticastStripEnable
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag 
*  record ,the others is tag record.
*  this function is used to del a tag record for vlan translation table 
*
* INPUTS:
*        pStrip		: 	0:strip disable,1: strip enable   for pon port.
*        gStrip	 	:  0:strip disable,1: strip enable 	for ge port.
*			cValn   	:	0-4095:vid in the packet which received from the subscrible port
*        sVlan		:	0-4095:vid at system side.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttMulticastStripEnable(UINT8 portId,UINT32 enable);

/*******************************************************************************
* dalVttMulticastInit
*
* DESCRIPTION:
* 
* INPUTS:
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttMulticastInit(UINT8 portId);

/*******************************************************************************
* dalVttMulticastNumOfRecordGet
*
* DESCRIPTION:
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS dalVttMulticastNumOfRecordGet(UINT8 *numOfRecords);

/*******************************************************************************
* dalVttInit
*
* DESCRIPTION:
*		this function is used to init the vlan translaion table. both the software database and hardware table.
*
* INPUTS:
*        pStrip		: 	0:strip disable,1: strip enable   for pon port.
*        gStrip	 	:  0:strip disable,1: strip enable 	for ge port.
*			cValn   	:	0-4095:vid in the packet which received from the subscrible port
*        sVlan		:	0-4095:vid at system side.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS dalVttInit(void);
OPL_STATUS dalVttVlanAggDel(UINT16 egressPort, UINT16 targetVlan, UINT16 aggedVlan,UINT8 *aggedMac);
OPL_STATUS dalVttVlanAggAdd(
	UINT16 egressPort, 
	UINT16 targetVlan, 
	UINT16 aggedVlan,
    UINT8 *aggedMac, 
    UINT16* pRuleId);

#endif
