/*
=============================================================================
Header Name:bridgevlan.h

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
		zzhu			2007/10/09		Initial Version
		---------------	---------------	----------------------------------------------
*/

#ifndef BRIDGEVLAN_H
#define BRIDGEVLAN_H

#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"
#include "hw_interface.h"

#ifdef OPL_DEBUG
#if 0
#define BRIDGEVLAN_DEBUG
#endif
#endif

#define BRG_VTT_TAB_NUM    TAB_BRG_VTT_LEN
#define BRG_VLAN_MAX_NUM  4095

typedef enum OPCONN_VLAN_MODE_s
{
	VLAN_TRANSPARENT,
	VLAN_TAG,
	VLAN_TRANSLATION,
	VLAN_AGGREGATION,
	VLAN_TRUNK,
	VLAN_END
}OPCONN_VLAN_MODE_e;

typedef enum OPCONN_VTT_PON_STRIP_s
{
	VTT_PON_STRIP_DIS,
	VTT_PON_STRIP_EN
}OPCONN_VTT_PON_STRIP_e;

typedef enum OPCONN_VTT_GE_STRIP_s
{
	VTT_GE_STRIP_DIS,
	VTT_GE_STRIP_EN
}OPCONN_VTT_GE_STRIP_e;

typedef enum OPCONN_VTT_ACTIVE_s
{
	VTT_UN_ACTIVE,
	VTT_ACTIVE
}OPCONN_VTT_ACTIVE_e;

typedef struct BRG_T_VTT_ENTRY_s
{
#ifdef OPCONN_BIG_ENDIAN
	UINT32 reserved:5;
	UINT32 active:1;
	UINT32 p_strip:1;
	UINT32 g_strip:1;
	UINT32 c_vid:12;
	UINT32 s_vid:12;
#else
	UINT32 s_vid:12;
	UINT32 c_vid:12;
	UINT32 g_strip:1;
	UINT32 p_strip:1;
	UINT32 active:1;
	UINT32 reserved:5;
#endif
}BRG_T_VTT_ENTRY_t;

typedef struct BRG_VTT_ENTRY_s
{
	union
	{
		BRG_T_VTT_ENTRY_t t_vtt_entry;
		UINT32 temp[1];
	}common;
}BRG_VTT_ENTRY_t;


/*******************************************************************************
* brgVttEntryHwWrite
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag
*  record ,the others is tag record.
*  this function is used to set a vlan translation entry in vlan entry table.
*
* INPUTS:
*	    entryId:0---BRG_VTT_TAB_NUM - 1
*     pBrg_vtt_entry:pointer of vlan value to be configed
*
* OUTPUTS:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS brgVttEntryHwWrite(UINT32 entryId,BRG_VTT_ENTRY_t *pBrg_vtt_entry);

/*******************************************************************************
* brgVttEntryHwRead
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag
*  record ,the others is tag record.
*  this function is used to get a vlan translation entry in vlan entry table.
*
* INPUTS:
*	    entryId:0---BRG_VTT_TAB_NUM - 1
*     pBrg_vtt_entry:pointer of value to be stored
*
* OUTPUTS:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS brgVttEntryHwRead(UINT32 entryId,BRG_VTT_ENTRY_t *pBrg_vtt_entry);

/*******************************************************************************
* brgVttInit
*
* DESCRIPTION:
*	  		bridge support 16 vlan translation table, this function is used to init the vlan translation table to defalut
*			value.
*
* INPUTS:
*			void
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS brgVttInit(void);


/*******************************************************************************
* brgVttEntryExist
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag
*  record ,the others is tag record.
*  this function is used search if the entry is exist
*
* INPUTS:
*				brg_vtt_entry
*
* OUTPUTS:
*   entryId: the match entryid .
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS brgVttEntryExist(BRG_VTT_ENTRY_t *pstEntry, UINT32 *entryId,UINT8 *found);


/*******************************************************************************
* brgVttFreeEntryFind
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag
*  record ,the others is tag record.
*  this function is used to get a empty table
*
* INPUTS:
*
* OUTPUTS:
*   entryId: the first empty entryid .
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS brgVttFreeEntryFind(UINT32 *entryId,UINT8 *found);

/*******************************************************************************
* brgVttEntryTagAdd
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag
*  record ,the others is tag record.
*  this function is used to set a tag record for vlan translation table
*
* INPUTS:
*        pon_strip_en: 0:strip disable,1: strip enable   for pon port.
*        ge_strip_en	 :  	0:strip disable,1: strip enable 	for ge port.
*			vid            	 :0-4095:vid in the packet which received from the subscrible port
*        vid_s			 :0-4095:vid at system side.
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS brgVttEntryTagAdd(UINT8 pon_strip_en,UINT8 ge_strip_en, UINT16 c_vid,UINT16 s_vid);

/*******************************************************************************
* brgVttEntryTagDel
*
* DESCRIPTION:
*  there are 16 vlan translation table in opconn,and have two types of vlan translation table  one is untag
*  record ,the others is tag record.
*  this function is used to del a tag record for vlan translation table
*
* INPUTS:
*        pon_strip_en: 0:strip disable,1: strip enable   for pon port.
*        ge_strip_en	 :  	0:strip disable,1: strip enable 	for ge port.
*			vid            	 :0-4095:vid in the packet which received from the subscrible port
*        vid_s			 :0-4095:vid at system side.
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS brgVttEntryTagDel(UINT8 pon_strip_en,UINT8 ge_strip_en, UINT16 c_vid,UINT16 s_vid);


/*******************************************************************************
* brgTpidHwWrite
*
* DESCRIPTION:
* this function is used to set TPID valuewhich is used to check the received packet both from the
* 	GE and PON port.
* OUTPUTS:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS brgTpidHwWrite(UINT16 tpid);


/*******************************************************************************
* brgTpidHwRead
*
* DESCRIPTION:
* this function is used to get TPID valuewhich is used to check the received packet both from the
* 	GE and PON port.
* OUTPUTS:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS brgTpidHwRead(UINT16 *tpid);


/*******************************************************************************
* brgVttLookupFailActionHwWrite
*
* DESCRIPTION:
* This bit used to control the VLAN translation action when there is no match for a specific VID in the
*	received packet.
*
* OUTPUTS:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS brgVttLookupFailActionHwWrite(UINT8 action);


/*******************************************************************************
* brgVttLookupFailActionHwRead
*
* DESCRIPTION:
*	This bit used to control the VLAN translation action when there is no match for a specific VID in the
*	received packet.
*
* OUTPUTS:
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS brgVttLookupFailActionHwRead(UINT8 *action);


/*******************************************************************************
* brgVttEntryShow
*
* DESCRIPTION:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO:
*/
OPL_STATUS brgVttEntryShow(UINT32 startEntryId,UINT32 endEntryId);

OPL_STATUS brgVttDirectHwWrite(INT32 entryId,INT32 active,INT32 pStrip,INT32 gStrip,INT32 cVlan,INT32 sVlan);
#endif
