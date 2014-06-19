/*
=============================================================================
     Header Name: dal_vtt.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/9		Initial Version
----------------   ------------  ----------------------------------------------
*/

#include "dal_lib.h"

static VLAN_CONFI_INFO_t dal_vtt_info[DAL_NUM_OF_PORTS +1];

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
extern 	OPL_STATUS dalVttVlanAggIni(UINT32 portId, UINT8 flg);
OPL_STATUS dalVttModeSet(UINT8 mode,UINT32 portId)
{

    BRG_T_VTT_ENTRY_t   brgPvidVttEntry;
    BRG_T_VTT_ENTRY_t   brgVttEntry;
    UINT32 vttRecIndex;
	OPL_STATUS retVal   = OPL_OK;
	UINT16 pvid;

  //printk("dalVttModeSet:mode=%d,portId=%d\n", mode, portId);

	if(DAL_NUM_OF_PORTS< portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(VLAN_END <= mode)
	{
		OPL_DAL_PRINTF(("no such vtt mode.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}

    OPL_MEMSET((UINT8 *)&brgPvidVttEntry,0,sizeof(BRG_T_VTT_ENTRY_t));

    /*clear all brgidge vlan translation entry*/
    OPL_MEMSET((UINT8 *)&brgVttEntry,0,sizeof(BRG_T_VTT_ENTRY_t));

	/* need to reset default vlan */
	dalVttVidGet(&pvid, portId);
    OPL_MEMSET((UINT8 *)&dal_vtt_info[portId],0x00,sizeof(VLAN_CONFI_INFO_t));
	dal_vtt_info[portId].defaultVlanTag = pvid;
	
    for(vttRecIndex = OPL_ZERO; vttRecIndex < DAL_PORT_VTT_ENTRY_NUM; vttRecIndex++)
	{
		retVal = brgVttEntryHwWrite(DAL_PORT_VTT_ENTRY_START_NUM + vttRecIndex, &brgVttEntry);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
	}

	dal_vtt_info[portId].vlanMode = mode;

  	/* hardware learning */
  	dalArlSoftLearnEnSet(0);
  dalVttVlanAggIni(portId, 0);
       brgSoftArlMacFlush();
  

	switch(mode)
	{
		case VLAN_TRANSPARENT:
			/* if vtt lookup fail,then forward the packet.*/
			brgVttLookupFailActionHwWrite(1);
			brgPonPortLinkControlSet(PON_PORT_UNTAG, 1);
			brgPonPortLinkControlSet(PON_PORT_TAG, 1);
			brgGePortLinkControlSet(GE_PORT_UNTAG, 1);
			brgGePortLinkControlSet(GE_PORT_TAG, 1);

            /* Begin Modified 2011-2-16*/
            #if 0
            brgPonPortLinkControlSet(PON_PORT_PVID, 0XFFF);
			brgGePortLinkControlSet(GE_PORT_PVID, 0XFFF);
            brgPvidVttEntry.p_strip = 1;
	        brgPvidVttEntry.g_strip = 1;
	        brgPvidVttEntry.c_vid = 0xFFF;
	        brgPvidVttEntry.s_vid = 0XFFF;
	        brgPvidVttEntry.active = DAL_VTT_REC_VALID;
			retVal = brgVttEntryHwWrite(DAL_PORT_PVID_VTT_ENTRY_START_NUM,&brgPvidVttEntry);
            #else
            brgPonPortLinkControlSet(PON_PORT_PVID, 0);
			brgGePortLinkControlSet(GE_PORT_PVID, 0);
            brgPvidVttEntry.p_strip = 1;
	        brgPvidVttEntry.g_strip = 1;
	        brgPvidVttEntry.c_vid = 0;
	        brgPvidVttEntry.s_vid = 0;
	        brgPvidVttEntry.active = DAL_VTT_REC_VALID;
			retVal = brgVttEntryHwWrite(DAL_PORT_PVID_VTT_ENTRY_START_NUM,&brgPvidVttEntry);
            #endif
            /* End   Modified, 2011-2-16 */

			break;
		case VLAN_TAG:
			/* if vtt lookup fail,then drop the packet.*/
			brgVttLookupFailActionHwWrite(0);
			/* in downstream, onu cannot receive untag packet.*/
			brgPonPortLinkControlSet(PON_PORT_UNTAG, 0);
			brgPonPortLinkControlSet(PON_PORT_TAG, 1);
			brgGePortLinkControlSet(GE_PORT_UNTAG, 1);
			brgGePortLinkControlSet(GE_PORT_TAG, 0);
#if 1	
	  		dalVttVidGet(&pvid, portId);

			brgGePortLinkControlSet(GE_PORT_PVID, pvid);

	  		brgPvidVttEntry.p_strip = 0;
	  		brgPvidVttEntry.g_strip = 1;
		 	brgPvidVttEntry.c_vid = pvid;
	  		brgPvidVttEntry.s_vid = pvid;
	  		brgPvidVttEntry.active = DAL_VTT_REC_VALID;
	  
	  		retVal = brgVttEntryHwWrite(DAL_PORT_PVID_VTT_ENTRY_START_NUM,&brgPvidVttEntry);
#endif
			break;
		case VLAN_TRANSLATION:
			brgVttLookupFailActionHwWrite(0);
			/* in downstream, onu cannot receive untag packet.*/
			brgPonPortLinkControlSet(PON_PORT_UNTAG, 0);
			brgPonPortLinkControlSet(PON_PORT_TAG, 1);
			brgGePortLinkControlSet(GE_PORT_UNTAG, 1);
			brgGePortLinkControlSet(GE_PORT_TAG, 1);
			brgGePortLinkControlSet(GE_PORT_PVID, 0XFFF);
#if 1
			dalVttVidGet(&pvid, portId);

			brgPvidVttEntry.p_strip = 0;
			brgPvidVttEntry.g_strip = 1;
			brgPvidVttEntry.c_vid = 0XFFF;
			brgPvidVttEntry.s_vid = pvid;
			brgPvidVttEntry.active = DAL_VTT_REC_VALID;

			retVal = brgVttEntryHwWrite(DAL_PORT_PVID_VTT_ENTRY_START_NUM,&brgPvidVttEntry);
#endif
			break;
    	case VLAN_AGGREGATION:
   		   /* software learning */
			dalArlSoftLearnEnSet(1);
      		       dalArlFlushAll();
			//dalArlFlushDynamic();/*2010-12-10, only flush the dynamic mac entry.*/

      		brgVttLookupFailActionHwWrite(1);
      		/* in downstream, onu cannot receive untag packet.*/
      		brgPonPortLinkControlSet(PON_PORT_UNTAG, 0);
      		brgPonPortLinkControlSet(PON_PORT_TAG, 1);
      		brgGePortLinkControlSet(GE_PORT_UNTAG, 1);
      		brgGePortLinkControlSet(GE_PORT_TAG, 1);
			brgGePortLinkControlSet(GE_PORT_PVID, 0XFFF);
#if 1
			dalVttVidGet(&pvid, portId);

			brgPvidVttEntry.p_strip = 0;
			brgPvidVttEntry.g_strip = 1;
			brgPvidVttEntry.c_vid = 0XFFF;
			brgPvidVttEntry.s_vid = pvid;
			brgPvidVttEntry.active = DAL_VTT_REC_VALID;

			retVal = brgVttEntryHwWrite(DAL_PORT_PVID_VTT_ENTRY_START_NUM,&brgPvidVttEntry);

			brgPvidVttEntry.p_strip = 0;
			brgPvidVttEntry.g_strip = 1;
			brgPvidVttEntry.c_vid = pvid;
			brgPvidVttEntry.s_vid = 0XFFF;
			brgPvidVttEntry.active = DAL_VTT_REC_VALID;

			retVal = brgVttEntryHwWrite(DAL_PORT_PVID_VTT_ENTRY_START_NUM - 1,&brgPvidVttEntry);			
#endif
      		dalVttVlanAggIni(portId, 1);
            retVal = OPL_OK;
	  		break;

		case VLAN_TRUNK:
			brgVttLookupFailActionHwWrite(0);
			brgPonPortLinkControlSet(PON_PORT_UNTAG, 0);
			brgPonPortLinkControlSet(PON_PORT_TAG, 1);
			brgGePortLinkControlSet(GE_PORT_UNTAG, 1);
			brgGePortLinkControlSet(GE_PORT_TAG, 1);
			brgGePortLinkControlSet(GE_PORT_PVID, 0XFFF);
			
#if 1
			dalVttVidGet(&pvid, portId);

			brgPvidVttEntry.p_strip = 0;
			brgPvidVttEntry.g_strip = 1;
			brgPvidVttEntry.c_vid = 0XFFF;
			brgPvidVttEntry.s_vid = pvid;
			brgPvidVttEntry.active = DAL_VTT_REC_VALID;

			retVal = brgVttEntryHwWrite(DAL_PORT_PVID_VTT_ENTRY_START_NUM,&brgPvidVttEntry);
#endif
			break;
		
		default:
			OPL_DAL_PRINTF(("no such vtt mode.\n"));
			return OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
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
OPL_STATUS dalVttModeGet(UINT8 *mode,UINT32 portId)
{
	if(OPL_NULL == mode)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	if(DAL_NUM_OF_PORTS < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}
	*mode = dal_vtt_info[portId].vlanMode;
	return OPL_OK;
}
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
OPL_STATUS dalVttTpidSet(UINT16 tpid,UINT32 portId)
{
	OPL_STATUS retVal = OPL_OK;

	if(DAL_NUM_OF_PORTS  < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}
	retVal = brgTpidHwWrite(tpid);

	if(OPL_OK != retVal)
	{
		return retVal;
	}

	dal_vtt_info[portId].defaultTPID = tpid;
	return OPL_OK;
}
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
OPL_STATUS dalVttTpidGet(UINT16 *tpid,UINT32 portId)
{
	if(OPL_NULL == tpid)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	if(DAL_NUM_OF_PORTS < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	*tpid = dal_vtt_info[portId].defaultTPID;
	return OPL_OK;
}
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
OPL_STATUS dalVttVidSet(UINT16 pvid,UINT32 portId)
{
	UINT8 vttMode;
    //BRG_T_VTT_ENTRY_t brgPvidVttEntry;
	OPL_STATUS retVal = OPL_OK;

	if(pvid > MAX_NUM_OF_VLAN || pvid == 0)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(DAL_NUM_OF_PORTS < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	vttMode = dal_vtt_info[portId].vlanMode;

	switch(vttMode)
	{
		case VLAN_TRANSPARENT:
			brgPonPortLinkControlSet(PON_PORT_PVID, pvid);
			brgGePortLinkControlSet(GE_PORT_PVID, pvid );
#if 0
            brgPvidVttEntry.p_strip = 1;
	        brgPvidVttEntry.g_strip = 1;
	        brgPvidVttEntry.c_vid = pvid;
	        brgPvidVttEntry.s_vid = pvid;
	        brgPvidVttEntry.active = DAL_VTT_REC_VALID;

			retVal = brgVttEntryHwWrite(DAL_PORT_PVID_VTT_ENTRY_START_NUM,&brgPvidVttEntry);
#endif			
			break;
		case VLAN_TAG:
			retVal = brgGePortLinkControlSet(GE_PORT_PVID, pvid);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
#if 0
            brgPvidVttEntry.p_strip = 0;
	        brgPvidVttEntry.g_strip = 1;
	        brgPvidVttEntry.c_vid = pvid;
	        brgPvidVttEntry.s_vid = pvid;
	        brgPvidVttEntry.active = DAL_VTT_REC_VALID;

			retVal = brgVttEntryHwWrite(DAL_PORT_PVID_VTT_ENTRY_START_NUM,&brgPvidVttEntry);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
#endif			
			break;
			
		case VLAN_TRANSLATION:
			brgGePortLinkControlSet(GE_PORT_PVID, pvid);
#if 0
            brgPvidVttEntry.p_strip = 0;
	        brgPvidVttEntry.g_strip = 1;
	        brgPvidVttEntry.c_vid = pvid;
	        brgPvidVttEntry.s_vid = pvid;
	        brgPvidVttEntry.active = DAL_VTT_REC_VALID;

			retVal = brgVttEntryHwWrite(DAL_PORT_PVID_VTT_ENTRY_START_NUM,&brgPvidVttEntry);

			printk("p_strip = %d, g_strip = %d\n", brgPvidVttEntry.p_strip, brgPvidVttEntry.g_strip);
			
			if(OPL_OK != retVal)
			{
				return retVal;
			}
#endif			
			break;
			case VLAN_AGGREGATION:
				brgGePortLinkControlSet(GE_PORT_PVID, pvid);
			 break;
			case VLAN_TRUNK:
				brgGePortLinkControlSet(GE_PORT_PVID, pvid);
			 break;
		default:
			break;
	}
	dal_vtt_info[portId].defaultVlanTag = pvid;

	return OPL_OK;
}
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
OPL_STATUS dalVttVidGet(UINT16 *pvid,UINT32 portId)
{

	if(OPL_NULL == pvid)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	if(DAL_NUM_OF_PORTS < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	
	 //brgGePortLinkControlGet(GE_PORT_PVID, &defaultVid);

	/* because of hardware default tag is diff with user configure value, so here get user configure default vlan */
	*pvid = dal_vtt_info[portId].defaultVlanTag;
	return OPL_OK;
}
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
OPL_STATUS dalVttDefaultPriSet(UINT8  etherPri,UINT32 portId)
{
	if(DAL_NUM_OF_PORTS < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	brgGePortLinkControlSet(GE_PORT_COS, etherPri);
	dal_vtt_info[portId].defaultPRI = etherPri;
	return OPL_OK;
}
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
OPL_STATUS dalVttDefaultPriGet(UINT8  *etherPri,UINT32 portId)
{
	if(OPL_NULL == etherPri)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	if(DAL_NUM_OF_PORTS < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	*etherPri = dal_vtt_info[portId].defaultPRI;
	return OPL_OK;
}
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
OPL_STATUS dalVttTagEntryAdd(UINT8 pStrip,UINT8 gStrip,UINT32 cVlan,UINT32 sVlan,UINT32 portId,UINT32 *pEntryIndex, UINT8 type)
{
	UINT8 recordIndex;
	BRG_VTT_ENTRY_t brg_vtt_entry;
    UINT32 firstEmptyEntryId = 0xffffffff;
    OPL_STATUS iStatus = OPL_OK;

	if(DAL_NUM_OF_PORTS < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}

    if(NULL == pEntryIndex)
    {
		OPL_DAL_PRINTF(("Input Null poiner.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	if(cVlan&&0xFFF > MAX_NUM_OF_VLAN || sVlan&0XFFF > MAX_NUM_OF_VLAN)
	{
		OPL_DAL_PRINTF(("vlanID is out of range.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	for(recordIndex = OPL_ZERO;recordIndex < DAL_PORT_VTT_ENTRY_NUM; recordIndex++)
	{
		if(dal_vtt_info[portId].vtt_record[recordIndex].valid)
		{
			if(dal_vtt_info[portId].vtt_record[recordIndex].sVlanInfo == sVlan &&
				dal_vtt_info[portId].vtt_record[recordIndex].cVlanInfo == cVlan)
			{
				return OPL_OK;
			}
		}else
		{
		    if(0xffffffff == firstEmptyEntryId)
            {
                 firstEmptyEntryId = recordIndex;
                 break;
            }
		}
	}

	if(0xffffffff == firstEmptyEntryId)
	{
		return DAL_VTT_FULL;
	}

	brg_vtt_entry.common.t_vtt_entry.p_strip = pStrip;
	brg_vtt_entry.common.t_vtt_entry.g_strip = gStrip;
	brg_vtt_entry.common.t_vtt_entry.c_vid = cVlan&0XFFF;
	brg_vtt_entry.common.t_vtt_entry.s_vid = sVlan&0XFFF;
	brg_vtt_entry.common.t_vtt_entry.active = DAL_VTT_REC_VALID;

	iStatus = brgVttEntryHwWrite(DAL_PORT_VTT_ENTRY_START_NUM + firstEmptyEntryId,  &brg_vtt_entry);

    if(OPL_OK == iStatus)
    {
        *pEntryIndex = firstEmptyEntryId;

		dal_vtt_info[portId].numOfVttRecords++;
		dal_vtt_info[portId].vtt_record[recordIndex].sVlanInfo= sVlan;
		dal_vtt_info[portId].vtt_record[recordIndex].cVlanInfo = cVlan;
		dal_vtt_info[portId].vtt_record[recordIndex].valid = DAL_VTT_REC_VALID;
    }

    return iStatus;
}
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
OPL_STATUS dalVttTagEntryGet(UINT8 ValidRecordIndex,UINT32 *cVlan,UINT32 *sVlan,UINT32 portId)
{
	UINT8 index;
	UINT8 validIndex;

	if(OPL_NULL == cVlan || OPL_NULL == sVlan )
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	if(DAL_NUM_OF_PORTS < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	for(index = OPL_ZERO,validIndex = OPL_ZERO; index < DAL_PORT_VTT_ENTRY_NUM;index++)
	{
		if(dal_vtt_info[portId].vtt_record[index].valid)
		{
			if(validIndex  == ValidRecordIndex)
			{
				break;
			}
			validIndex++;
		}
	}
	if(validIndex  != ValidRecordIndex)
		return OPL_ERROR;

	*cVlan = dal_vtt_info[portId].vtt_record[index].cVlanInfo;
	*sVlan = dal_vtt_info[portId].vtt_record[index].sVlanInfo;
	return OPL_OK;
}
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
OPL_STATUS dalVttTagEntryDel(UINT8 pStrip,UINT8 gStrip,UINT32 cVlan,UINT32 sVlan,UINT32 portId,UINT32 *pEntryIndex, UINT8 type)
{
	UINT8 recordIndex = OPL_ZERO;
	BRG_VTT_ENTRY_t brg_vtt_entry;
    OPL_STATUS iStatus = OPL_OK;

	if(DAL_NUM_OF_PORTS < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}

    if(NULL == pEntryIndex)
    {
		OPL_DAL_PRINTF(("Input Null poiner.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	if(cVlan > MAX_NUM_OF_VLAN || sVlan > MAX_NUM_OF_VLAN)
	{
		OPL_DAL_PRINTF(("vlanID is out of range.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	OPL_MEMSET(&brg_vtt_entry,0X00,sizeof(BRG_VTT_ENTRY_t));
	for(recordIndex = OPL_ZERO;recordIndex < DAL_PORT_VTT_ENTRY_NUM; recordIndex++)
	{
		if(dal_vtt_info[portId].vtt_record[recordIndex].valid &&
			dal_vtt_info[portId].vtt_record[recordIndex].cVlanInfo == cVlan &&
			dal_vtt_info[portId].vtt_record[recordIndex].sVlanInfo == sVlan)
			break;
	}

	if(recordIndex ==  DAL_PORT_VTT_ENTRY_NUM)
	{
		return OPL_BRIDGE_VTT_RECORD_NOT_FOUND;
	}

    OPL_MEMSET(&brg_vtt_entry,0X00,sizeof(BRG_VTT_ENTRY_t));

	iStatus = brgVttEntryHwWrite(DAL_PORT_VTT_ENTRY_START_NUM + recordIndex,  &brg_vtt_entry);

    if(OPL_OK == iStatus)
    {
        *pEntryIndex = recordIndex;

		OPL_MEMSET(&dal_vtt_info[portId].vtt_record[recordIndex],0X00,sizeof(VTT_RECORD_t));
	    dal_vtt_info[portId].numOfVttRecords = dal_vtt_info[portId].numOfVttRecords - 1;		
    }

    return iStatus;
}
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
OPL_STATUS dalVttNumOfRecordGet(UINT8 *numOfRecords,UINT32 portId)
{
	if(OPL_NULL == numOfRecords)
	{
		OPL_DAL_PRINTF(("input null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	if(DAL_NUM_OF_PORTS < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	*numOfRecords = dal_vtt_info[portId].numOfVttRecords;
	return OPL_OK;
}

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
OPL_STATUS dalVttMulticastEntryAdd(UINT8 pStrip,UINT8 gStrip,UINT32 cVlan,UINT32 sVlan,UINT32 portId)
{
	UINT8 recordIndex;
	BRG_T_VTT_ENTRY_t multicastVlanEntry;
    BRG_T_VTT_ENTRY_t oldEntry;
    OPL_STATUS retVal = OPL_OK;
    UINT32 firstEmptyEntryId = 0xffffffff;

	if(DAL_NUM_OF_PORTS < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(cVlan&&0xFFF > MAX_NUM_OF_VLAN || sVlan&0XFFF > MAX_NUM_OF_VLAN)
	{
		OPL_DAL_PRINTF(("vlanID is out of range.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}

    multicastVlanEntry.p_strip  = pStrip;
	multicastVlanEntry.g_strip  = gStrip;
	multicastVlanEntry.c_vid    = cVlan&0XFFF;
	multicastVlanEntry.s_vid    = sVlan&0XFFF;
	multicastVlanEntry.active   = DAL_VTT_REC_VALID;

    for(recordIndex = OPL_ZERO;recordIndex < DAL_PORT_MCAST_ENTRY_NUM;recordIndex++)
    {
        retVal = brgVttEntryHwRead(DAL_PORT_MCAST_ENTRY_START_NUM + recordIndex, (BRG_VTT_ENTRY_t *)&oldEntry);
        if(OPL_OK != retVal)
        {
            return retVal;
        }

        if(oldEntry.active == DAL_VTT_REC_INVALID)
        {
            if(0xffffffff == firstEmptyEntryId)
            {
                firstEmptyEntryId = recordIndex;
            }
        }
        else
        {
          if(oldEntry.c_vid == multicastVlanEntry.c_vid &&
              oldEntry.s_vid == multicastVlanEntry.s_vid)
          {
          //    return OPL_OK;
          	firstEmptyEntryId = recordIndex;
          	break;
          }
        }
    }

	if(firstEmptyEntryId == 0xffffffff)
	{
		return DAL_VTT_FULL;
	}

	return brgVttEntryHwWrite(DAL_PORT_MCAST_ENTRY_START_NUM + firstEmptyEntryId,  &multicastVlanEntry);
}

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
OPL_STATUS dalVttMulticastEntryDel(UINT8 pStrip,UINT8 gStrip,UINT32 cVlan,UINT32 sVlan,UINT32 portId)
{
	UINT8 recordIndex;
	BRG_T_VTT_ENTRY_t oldEntry;
    OPL_STATUS retVal = OPL_OK;
    UINT32 matchEntryId = 0xffffffff;

	if(DAL_NUM_OF_PORTS < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(cVlan&&0xFFF > MAX_NUM_OF_VLAN || sVlan&0XFFF > MAX_NUM_OF_VLAN)
	{
		OPL_DAL_PRINTF(("vlanID is out of range.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}

    OPL_MEMSET((UINT8 *)&oldEntry,0X00,sizeof(BRG_T_VTT_ENTRY_t));

    for(recordIndex = OPL_ZERO;recordIndex < DAL_PORT_MCAST_ENTRY_NUM;recordIndex++)
    {
        retVal = brgVttEntryHwRead(DAL_PORT_MCAST_ENTRY_START_NUM + recordIndex, (BRG_VTT_ENTRY_t *)&oldEntry);
        if(OPL_OK != retVal)
        {
            return retVal;
        }

        if(oldEntry.c_vid == (cVlan&0xfff) &&
            oldEntry.s_vid == (sVlan&0xfff))
        {
            matchEntryId = recordIndex;
            break;
        }
    }

	if(DAL_PORT_MCAST_ENTRY_NUM == recordIndex ||
     matchEntryId == 0xffffffff)
	{
		return OPL_OK;
	}

    OPL_MEMSET(&oldEntry,0X00,sizeof(BRG_T_VTT_ENTRY_t));

	return brgVttEntryHwWrite(DAL_PORT_MCAST_ENTRY_START_NUM + matchEntryId,  &oldEntry);
}

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
OPL_STATUS dalVttMulticastStripEnable(UINT8 portId,UINT32 enable)
{
	UINT8 recordIndex;
    BRG_T_VTT_ENTRY_t oldEntry;
    OPL_STATUS retVal = OPL_OK;

	if(DAL_NUM_OF_PORTS < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(OPL_TRUE != enable && OPL_FALSE != enable)
    {
		OPL_DAL_PRINTF(("parameter error.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}


    for(recordIndex = OPL_ZERO;recordIndex < DAL_PORT_MCAST_ENTRY_NUM;recordIndex++)
    {
        retVal = brgVttEntryHwRead(DAL_PORT_MCAST_ENTRY_START_NUM + recordIndex, (BRG_VTT_ENTRY_t *)&oldEntry);
        if(OPL_OK != retVal)
        {
            return retVal;
        }

        if(oldEntry.active == DAL_VTT_REC_VALID)
        {
            oldEntry.g_strip = enable;
            retVal = brgVttEntryHwWrite(DAL_PORT_MCAST_ENTRY_START_NUM + recordIndex,&oldEntry);
            if(OPL_OK != retVal)
            {
                return retVal;
            }
        }
    }
    return OPL_OK;
}

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
OPL_STATUS dalVttMulticastInit(UINT8 portId)
{
	UINT8 recordIndex;
    BRG_T_VTT_ENTRY_t vttMcastEntry;
    OPL_STATUS retVal = OPL_OK;

	if(DAL_NUM_OF_PORTS < portId)
	{
		OPL_DAL_PRINTF(("portId %d is invalid.\n",portId));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	OPL_MEMSET(&vttMcastEntry,0X00,sizeof(BRG_T_VTT_ENTRY_t));


    for(recordIndex = OPL_ZERO;recordIndex < DAL_PORT_MCAST_ENTRY_NUM;recordIndex++)
    {
       retVal = brgVttEntryHwWrite(DAL_PORT_MCAST_ENTRY_START_NUM + recordIndex,&vttMcastEntry);
       if(OPL_OK != retVal)
       {
           return retVal;
       }
    }
    return OPL_OK;
}

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
OPL_STATUS dalVttMulticastNumOfRecordGet(UINT8 *numOfRecords)
{
    UINT8 recordIndex, number;
    BRG_T_VTT_ENTRY_t vttMcastEntry;
    OPL_STATUS retVal = OPL_OK;

    if (OPL_NULL == numOfRecords)
    {
        OPL_DAL_PRINTF(("input null pointer.\n"));
        return OPL_ERR_NULL_POINTER;
    }

    number = 0;

    for (recordIndex = OPL_ZERO; recordIndex < DAL_PORT_MCAST_ENTRY_NUM; recordIndex++)
    {
        retVal = brgVttEntryHwRead(DAL_PORT_MCAST_ENTRY_START_NUM + recordIndex, (BRG_VTT_ENTRY_t *)&vttMcastEntry);
        if(OPL_OK != retVal)
        {
            OPL_DAL_PRINTF(("Read vtt multicast entry fail.\n"));
            return retVal;
        }

        if (vttMcastEntry.active == DAL_VTT_REC_VALID)
        {
            number++;
        }
    }

    *numOfRecords = number;
    
    return OPL_OK;
}


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
OPL_STATUS dalVttInit(void)
{
	UINT8 portId;

	/* Begin Modified, 2011-2-16 */
    #if 0
	for(portId = 1; portId <= DAL_NUM_OF_PORTS; portId++)
	{
		OPL_MEMSET(&dal_vtt_info[portId],0X00,sizeof(VLAN_CONFI_INFO_t));
		dalVttModeSet(VLAN_TRANSPARENT,portId);
	}
    #else
    for(portId = 0; portId <= DAL_NUM_OF_PORTS; portId++)
	{
		OPL_MEMSET(&dal_vtt_info[portId],0X00,sizeof(VLAN_CONFI_INFO_t));
		dalVttModeSet(VLAN_TRANSPARENT,portId);
	}
    #endif
	/* End   Modified, 2011-2-16*/
	
	return OPL_OK;
}

#if 0/*wfxu*/
OPL_STATUS dalVttShow(void)
{
  int i;
  BRG_T_VTT_ENTRY_t *e;
  UINT32 val;

  memset(&e, 0, sizeof(e));

  printk("Show Vtt Table:\n");
  for(i = 0; i < 16; i++) {
    oplRegRead((0x3080+i)*4, &val);
    if(val!=0) {
      e = (BRG_T_VTT_ENTRY_t *)&val;
      printk("%d: ACTIVE=%d,P_STRIP=%x,G_STRIP=%x,CVID=%d,SVID=%d\n",
        i, e->active, e->p_strip, e->g_strip, e->c_vid, e->s_vid);
    }
  }

  return 0;
}
#endif
OPL_STATUS dalVttVlanAggIni(UINT32 portId, UINT8 flg)
{
  CLASSIFY_ITEM_t entry;
  CLASSIFY_PARAMETER_t *pPara;
  UINT16 ruleId;
  int ret;
  opl_cls_hwtbl_t pClshwID;
  UINT16 pvid;
  
  #if 1
  memset(&entry, 0, sizeof(entry));
  entry.valid = 1;
  pPara = &entry.Parameter;
  pPara->pktType = 0x2;
  pPara->RuleEntry.bfTranActL = 0x2;
  pPara->PktRuleValue.EIIValue.bfFirstTpid = 0x8100;
  pPara->PktUsrMask.EIIValue.bfFirstTpid = 0xffff;
    
  pPara->FieldMap.bitMap = 0x28000000;
  if (1 == flg)
  {
    ret = dalClsEntrySet(pPara, &ruleId,&pClshwID);
  }
  else
  {
    ret = dalClsEntryMove(pPara, &ruleId);
  }

  dalVttVidGet(&pvid, portId);
  memset(&entry, 0, sizeof(entry));
  entry.valid = 1;
  pPara = &entry.Parameter;
  pPara->pktType = 0x2;
  pPara->RuleEntry.bfTranActL = 0x4;
  pPara->RuleEntry.bfTranActH = 0x1;
  pPara->PktRuleValue.EIIValue.bfFirstTpid = 0x8100;
  pPara->PktUsrMask.EIIValue.bfFirstTpid = 0xffff;
  pPara->PktRuleValue.EIIValue.bfFirstVlan = pvid;
  pPara->PktUsrMask.EIIValue.bfFirstVlan = 0xfff;
  
#if 0 /* avoid classify action collision with protocol packets that should be copied to cpu */
  pPara->FieldMap.bitMap = 0x28000000;
#else
  pPara->PktRuleValue.EIIValue.DestMac[0] = 0x00;
  pPara->PktUsrMask.EIIValue.DestMac[0] = 0x01;
  pPara->FieldMap.bitMap = 0xa8000000;
#endif
  
  if (1 == flg)
  {
    ret = dalClsEntrySet(pPara, &ruleId,&pClshwID);
  }
  else
  {
    ret = dalClsEntryMove(pPara, &ruleId);
  }
    
  #endif
  
  return ret;
}

/*
 * N:1 VLAN Aggregation mode support
 */
OPL_STATUS dalVttVlanAggAdd(UINT16 egressPort, UINT16 targetVlan, UINT16 aggedVlan,
  UINT8 *aggedMac, UINT16* pRuleId)
{
  int ret;
  CLASSIFY_ITEM_t entry;
  CLASSIFY_PARAMETER_t *pPara;
  UINT16 ruleId;
  UINT32 vttIndex;
  opl_cls_hwtbl_t pClshwID;

  /* parameters check */

  ret = 0;
  memset(&entry, 0, sizeof(entry));
  ruleId = 0;
  vttIndex = 0;

#if 0
  /* upstream, high priority */
  entry.valid = 1;
  pPara = &entry.Parameter;
  pPara->pktType = 0x2;
  pPara->RuleEntry.bfTranActL = 0x4;
  pPara->RuleEntry.bfTranActH = 0x1;
  pPara->FieldMap.EIIField.bfEnaSrcMAC = 0x1;
  pPara->FieldMap.EIIField.bfEnaFirstTag = 0x1;
#if 0 /* avoid classify action collision with protocol packets that should be copied to cpu */
  pPara->FieldMap.bitMap = 0x48000000;
#else
  pPara->PktRuleValue.EIIValue.DestMac[0] = 0x00;
  pPara->PktUsrMask.EIIValue.DestMac[0] = 0x01;
  pPara->FieldMap.bitMap = 0xc8000000;
#endif
  pPara->RuleEntry.bfRmkAct = 0xc;
  pPara->RuleEntry.bfRmkVid = targetVlan;

  pPara->PktRuleValue.EIIValue.SrcMac[0] = aggedMac[0];
  pPara->PktRuleValue.EIIValue.SrcMac[1] = aggedMac[1];
  pPara->PktRuleValue.EIIValue.SrcMac[2] = aggedMac[2];
  pPara->PktRuleValue.EIIValue.SrcMac[3] = aggedMac[3];
  pPara->PktRuleValue.EIIValue.SrcMac[4] = aggedMac[4];
  pPara->PktRuleValue.EIIValue.SrcMac[5] = aggedMac[5];
  pPara->PktRuleValue.EIIValue.bfFirstVlan = aggedVlan;

  pPara->PktUsrMask.EIIValue.SrcMac[0] = 0xff;
  pPara->PktUsrMask.EIIValue.SrcMac[1] = 0xff;
  pPara->PktUsrMask.EIIValue.SrcMac[2] = 0xff;
  pPara->PktUsrMask.EIIValue.SrcMac[3] = 0xff;
  pPara->PktUsrMask.EIIValue.SrcMac[4] = 0xff;
  pPara->PktUsrMask.EIIValue.SrcMac[5] = 0xff;
  pPara->PktUsrMask.EIIValue.bfFirstVlan = 0xfff;

  ret = dalClsEntrySet(pPara, &ruleId,&pClshwID);
  
  if(ret!=OPL_OK) {
    //printk("Cls return error, ret=%x!\n", ret);
    return ret;
  }
  //printk("ruleId=%d\n", ruleId);

  memset(&entry, 0, sizeof(entry));
#endif
  if ((aggedMac[0] == 0)
  	&& (aggedMac[1] == 0)
  	&& (aggedMac[2] == 0)
  	&& (aggedMac[3] == 0)
  	&& (aggedMac[4] == 0)
  	&& (aggedMac[5] == 0))
  {
	  /* upstream, aggregated vlan */
	  entry.valid = 1;
	  pPara = &entry.Parameter;
	  pPara->pktType = 0x2;
	  pPara->RuleEntry.bfTranActL = 0x4;
	  pPara->RuleEntry.bfTranActH = 0x1;
	  pPara->FieldMap.EIIField.bfEnaSrcMAC = 0x1;
	  pPara->FieldMap.EIIField.bfEnaFirstTag = 0x1;
	        #if 0 /* avoid classify action collision with protocol packets that should be copied to cpu */
	  pPara->FieldMap.bitMap = 0x48000000;
	        #else
	  pPara->PktRuleValue.EIIValue.DestMac[0] = 0x00;
	  pPara->PktUsrMask.EIIValue.DestMac[0] = 0x01;
	  pPara->FieldMap.bitMap = 0x88000000;
	        #endif
	  pPara->RuleEntry.bfRmkAct = 0xc;
	  pPara->RuleEntry.bfRmkVid = targetVlan;
	  pPara->PktRuleValue.EIIValue.bfFirstVlan = aggedVlan;
	  pPara->PktUsrMask.EIIValue.bfFirstVlan = 0xfff;

	  ret = dalClsEntrySet(pPara, &ruleId,&pClshwID);
  
	  if(ret!=OPL_OK) {
	    return ret;
	  }
  }
  else
  {
  /* downstream, low priority */
  entry.valid = 1;
  pPara = &entry.Parameter;
  pPara->pktType = 0x2;
  pPara->RuleEntry.bfTranActL = 0x4;
  pPara->RuleEntry.bfTranActH = 0x1;
  pPara->FieldMap.EIIField.bfEnaDestMAC = 0x1;
  pPara->FieldMap.EIIField.bfEnaFirstTag = 0x1;
  pPara->FieldMap.bitMap = 0x88000000;
  pPara->RuleEntry.bfRmkAct = 0xc;
  pPara->RuleEntry.bfRmkVid = aggedVlan;

  pPara->PktRuleValue.EIIValue.DestMac[0] = aggedMac[0];
  pPara->PktRuleValue.EIIValue.DestMac[1] = aggedMac[1];
  pPara->PktRuleValue.EIIValue.DestMac[2] = aggedMac[2];
  pPara->PktRuleValue.EIIValue.DestMac[3] = aggedMac[3];
  pPara->PktRuleValue.EIIValue.DestMac[4] = aggedMac[4];
  pPara->PktRuleValue.EIIValue.DestMac[5] = aggedMac[5];
  pPara->PktRuleValue.EIIValue.bfFirstVlan = targetVlan;

  pPara->PktUsrMask.EIIValue.DestMac[0] = 0xff;
  pPara->PktUsrMask.EIIValue.DestMac[1] = 0xff;
  pPara->PktUsrMask.EIIValue.DestMac[2] = 0xff;
  pPara->PktUsrMask.EIIValue.DestMac[3] = 0xff;
  pPara->PktUsrMask.EIIValue.DestMac[4] = 0xff;
  pPara->PktUsrMask.EIIValue.DestMac[5] = 0xff;
  pPara->PktUsrMask.EIIValue.bfFirstVlan = 0xfff;

  ret = dalClsEntrySet(pPara, &ruleId,&pClshwID);
  if(ret!=OPL_OK) {
    //printk("Cls return error, ret=%x!\n", ret);
    return ret;
  }
  //printk("ruleId=%d\n", ruleId);
  }

  if (pRuleId){
  	*pRuleId = ruleId;
  }
  
  return OPL_OK;
}

OPL_STATUS dalVttVlanAggDel(UINT16 egressPort, UINT16 targetVlan, UINT16 aggedVlan,
  UINT8 *aggedMac)
{
  int ret;
  CLASSIFY_ITEM_t entry;
  CLASSIFY_PARAMETER_t *pPara;
  UINT16 ruleId;
  UINT32 vttIndex;

  /* parameters check */

  ret = 0;
  memset(&entry, 0, sizeof(entry));
  ruleId = 0;
  vttIndex = 0;

#if 0
  /* upstream, high priority */
  entry.valid = 1;
  pPara = &entry.Parameter;
  pPara->pktType = 0x2;
  pPara->FieldMap.EIIField.bfEnaSrcMAC = 0x1;
  pPara->FieldMap.EIIField.bfEnaFirstTag = 0x1;
#if 0 /* avoid classify action collision with protocol packets that should be copied to cpu */
  pPara->FieldMap.bitMap = 0x48000000;
#else
  pPara->PktRuleValue.EIIValue.DestMac[0] = 0x00;
  pPara->PktUsrMask.EIIValue.DestMac[0] = 0x01;
  pPara->FieldMap.bitMap = 0xc8000000;
#endif
  pPara->RuleEntry.bfRmkAct = 0xc;
  pPara->RuleEntry.bfRmkVid = targetVlan;

  pPara->PktRuleValue.EIIValue.SrcMac[0] = aggedMac[0];
  pPara->PktRuleValue.EIIValue.SrcMac[1] = aggedMac[1];
  pPara->PktRuleValue.EIIValue.SrcMac[2] = aggedMac[2];
  pPara->PktRuleValue.EIIValue.SrcMac[3] = aggedMac[3];
  pPara->PktRuleValue.EIIValue.SrcMac[4] = aggedMac[4];
  pPara->PktRuleValue.EIIValue.SrcMac[5] = aggedMac[5];
  pPara->PktRuleValue.EIIValue.bfFirstVlan = aggedVlan;

  pPara->PktUsrMask.EIIValue.SrcMac[0] = 0xff;
  pPara->PktUsrMask.EIIValue.SrcMac[1] = 0xff;
  pPara->PktUsrMask.EIIValue.SrcMac[2] = 0xff;
  pPara->PktUsrMask.EIIValue.SrcMac[3] = 0xff;
  pPara->PktUsrMask.EIIValue.SrcMac[4] = 0xff;
  pPara->PktUsrMask.EIIValue.SrcMac[5] = 0xff;
  pPara->PktUsrMask.EIIValue.bfFirstVlan = 0xfff;

  ret = dalClsEntryMove(pPara, &ruleId);
  if(ret!=OPL_OK) {
    //printk("Cls return error, ret=%x!\n", ret);
    return ret;
  }
  //printk("ruleId=%d\n", ruleId);

  memset(&entry, 0, sizeof(entry));
#endif

   if ((aggedMac[0] == 0)
  	&& (aggedMac[1] == 0)
  	&& (aggedMac[2] == 0)
  	&& (aggedMac[3] == 0)
  	&& (aggedMac[4] == 0)
  	&& (aggedMac[5] == 0))
  {
	  /* upstream, aggregated vlan */
	  entry.valid = 1;
	  pPara = &entry.Parameter;
	  pPara->pktType = 0x2;
	  pPara->FieldMap.EIIField.bfEnaSrcMAC = 0x1;
	  pPara->FieldMap.EIIField.bfEnaFirstTag = 0x1;
	        #if 0 /* avoid classify action collision with protocol packets that should be copied to cpu */
	  pPara->FieldMap.bitMap = 0x48000000;
	        #else
	  pPara->PktRuleValue.EIIValue.DestMac[0] = 0x00;
	  pPara->PktUsrMask.EIIValue.DestMac[0] = 0x01;
	  pPara->FieldMap.bitMap = 0x88000000;
	        #endif
	  pPara->RuleEntry.bfRmkAct = 0xc;
	  pPara->RuleEntry.bfRmkVid = targetVlan;
	  pPara->PktRuleValue.EIIValue.bfFirstVlan = aggedVlan;
	  pPara->PktUsrMask.EIIValue.bfFirstVlan = 0xfff;

	  ret = dalClsEntryMove(pPara, &ruleId);
         if(ret!=OPL_OK) {
              return ret;
         }
  }
   else
   {
  /* downstream, low priority */
  entry.valid = 1;
  pPara = &entry.Parameter;
  pPara->pktType = 0x2;
  pPara->FieldMap.EIIField.bfEnaDestMAC = 0x1;
  pPara->FieldMap.EIIField.bfEnaFirstTag = 0x1;
  pPara->FieldMap.bitMap = 0x88000000;
  pPara->RuleEntry.bfRmkAct = 0xc;
  pPara->RuleEntry.bfRmkVid = aggedVlan;

  pPara->PktRuleValue.EIIValue.DestMac[0] = aggedMac[0];
  pPara->PktRuleValue.EIIValue.DestMac[1] = aggedMac[1];
  pPara->PktRuleValue.EIIValue.DestMac[2] = aggedMac[2];
  pPara->PktRuleValue.EIIValue.DestMac[3] = aggedMac[3];
  pPara->PktRuleValue.EIIValue.DestMac[4] = aggedMac[4];
  pPara->PktRuleValue.EIIValue.DestMac[5] = aggedMac[5];
  pPara->PktRuleValue.EIIValue.bfFirstVlan = targetVlan;

  pPara->PktUsrMask.EIIValue.DestMac[0] = 0xff;
  pPara->PktUsrMask.EIIValue.DestMac[1] = 0xff;
  pPara->PktUsrMask.EIIValue.DestMac[2] = 0xff;
  pPara->PktUsrMask.EIIValue.DestMac[3] = 0xff;
  pPara->PktUsrMask.EIIValue.DestMac[4] = 0xff;
  pPara->PktUsrMask.EIIValue.DestMac[5] = 0xff;
  pPara->PktUsrMask.EIIValue.bfFirstVlan = 0xfff;

  ret = dalClsEntryMove(pPara, &ruleId);
  if(ret!=OPL_OK) {
    //printk("Cls return error, ret=%x!\n", ret);
    return ret;
  }
  //printk("ruleId=%d\n", ruleId);
   }

  return OPL_OK;
}

#if 0 /*wfxu*/
OPL_STATUS dalVttVlanAggShow(void)
{
  printk("Cls mask table:\n");
  dalClsMaskEntryShow(0,0,23,2);
  printk("Cls addr table:\n");
  dalClsAddrEntryShow(0,0,127,2);
  printk("Cls rule table:\n");
  dalClsRuleEntryShow(0,0,127,2);

  return 0;
}

#endif

/* END: N:1 VLAN Aggregation mode support */

