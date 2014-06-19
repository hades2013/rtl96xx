/*
=============================================================================
     Header Name: bridgevlan.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/10/10		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#include "bridgevlan.h"
#include "bridgeport.h"

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
OPL_STATUS brgVttEntryHwWrite(UINT32 entryId,BRG_VTT_ENTRY_t *pBrg_vtt_entry)
{
	if(BRG_VTT_TAB_NUM <= entryId || OPL_ZERO > entryId || OPL_NULL == pBrg_vtt_entry)
	{
		OPL_TRACE();
		return OPL_ERROR;
	}
    return oplRegWrite((REG_BRG_VTT_REC_0 + (entryId*4)),(UINT32)(*(UINT32 *)pBrg_vtt_entry));
}
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
OPL_STATUS brgVttEntryHwRead(UINT32 entryId,BRG_VTT_ENTRY_t *pBrg_vtt_entry)
{
	if(BRG_VTT_TAB_NUM <= entryId || OPL_ZERO > entryId || OPL_NULL == pBrg_vtt_entry)
	{
		OPL_TRACE();
		return OPL_ERROR;
	}
	return oplRegRead((REG_BRG_VTT_REC_0 + (entryId*4)),(UINT32 *)pBrg_vtt_entry);
}
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
OPL_STATUS brgVttInit(void)
{
	OPL_STATUS retVal;
	UINT32 entryId;
	BRG_VTT_ENTRY_t brg_vtt_entry;

	OPL_MEMSET((void*)&brg_vtt_entry,0x0,sizeof(BRG_VTT_ENTRY_t));
	for(entryId = OPL_ZERO; entryId < BRG_VTT_TAB_NUM; entryId++)
	{
		retVal = brgVttEntryHwWrite(entryId, &brg_vtt_entry);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
	}
	return retVal;
}

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
OPL_STATUS brgVttEntryExist(BRG_VTT_ENTRY_t *pstEntry, UINT32 *entryId,UINT8 *found)
{
	OPL_STATUS retVal;
	UINT32 index;
	BRG_VTT_ENTRY_t tmpEntry;

	*found = OPL_NOT_FIND;
	for(index = OPL_ZERO; index < BRG_VTT_TAB_NUM; index++)
	{
		retVal = brgVttEntryHwRead(index, &tmpEntry);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
		if(pstEntry->common.t_vtt_entry.s_vid == tmpEntry.common.t_vtt_entry.s_vid &&
			pstEntry->common.t_vtt_entry.c_vid == tmpEntry.common.t_vtt_entry.c_vid)
		{
			*found = OPL_FIND;
			*entryId = index;
			/*
			OPL_MEMCPY(pstEntry,&tmpEntry,sizeof(BRG_VTT_ENTRY_t));	
			*/
			break;
		}
	}
	return retVal;
}

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
OPL_STATUS brgVttFreeEntryFind(UINT32 *entryId,UINT8 *found)
{
	OPL_STATUS retVal;
	BRG_VTT_ENTRY_t tmpEntry;
	UINT32 index;
	UINT8 firstUnActive = OPL_ZERO;

	*found = OPL_NOT_FIND;
	for(index = OPL_ZERO; index < BRG_VTT_TAB_NUM;index++)
	{
		retVal = brgVttEntryHwRead(index, &tmpEntry);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
		if(tmpEntry.common.t_vtt_entry.active == VTT_UN_ACTIVE)
		{
			if(!firstUnActive)
			{
				*entryId = index;
				*found = OPL_FIND;	
			}
		}
		if(0 == tmpEntry.common.t_vtt_entry.s_vid && 0 == tmpEntry.common.t_vtt_entry.s_vid)
		{
			*entryId = index;
			*found = OPL_FIND;
			break;
		}
	}
	return retVal;
}
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
OPL_STATUS brgVttEntryTagAdd(UINT8 pon_strip_en,UINT8 ge_strip_en, UINT16 c_vid,UINT16 s_vid)
{
	BRG_VTT_ENTRY_t brg_vtt_entry;
	OPL_STATUS retVal;
	UINT32 entryId;
	UINT8   find = OPL_NOT_FIND;
	
	if(c_vid > BRG_VLAN_MAX_NUM || s_vid > BRG_VLAN_MAX_NUM)
	{
		OPL_DRV_PRINTK(("VLAN out of range, cvid = %x,svid = %x\n",c_vid,s_vid));
		return OPL_ERROR;
	}

	OPL_MEMSET(&brg_vtt_entry,0X00,sizeof(BRG_VTT_ENTRY_t));
	if(pon_strip_en)
	{
		brg_vtt_entry.common.t_vtt_entry.p_strip = VTT_PON_STRIP_EN;
	}else
	{
		brg_vtt_entry.common.t_vtt_entry.p_strip = VTT_PON_STRIP_DIS;
	}

	if(ge_strip_en)
	{
		brg_vtt_entry.common.t_vtt_entry.g_strip = VTT_GE_STRIP_EN;
	}else
	{
		brg_vtt_entry.common.t_vtt_entry.g_strip = VTT_GE_STRIP_DIS;
	}
	brg_vtt_entry.common.t_vtt_entry.active = 	VTT_ACTIVE;
	brg_vtt_entry.common.t_vtt_entry.s_vid = s_vid;
	brg_vtt_entry.common.t_vtt_entry.c_vid = c_vid;
		
	retVal = brgVttEntryExist(&brg_vtt_entry,&entryId,&find);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	if(OPL_NOT_FIND == find)
	{
		retVal =  brgVttFreeEntryFind(&entryId, &find);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
		if(OPL_NOT_FIND == find)
		{
			OPL_DRV_PRINTK(("vtt table is full.\n"));
			return OPL_BRIDGE_VTT_TABLE_FULL;
		}
		
		retVal = brgVttEntryHwWrite(entryId, &brg_vtt_entry);
	}else
	{
		retVal = brgVttEntryHwWrite(entryId, &brg_vtt_entry);
	}
	
	return retVal;
}

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
OPL_STATUS brgVttEntryTagDel(UINT8 pon_strip_en,UINT8 ge_strip_en, UINT16 c_vid,UINT16 s_vid)
{
	BRG_VTT_ENTRY_t brg_vtt_entry;
	OPL_STATUS retVal;
	UINT32 entryId;
	UINT8 find = OPL_NOT_FIND;
	
	if(c_vid > BRG_VLAN_MAX_NUM || s_vid > BRG_VLAN_MAX_NUM)
	{
		OPL_DRV_PRINTK(("VLAN out of range, cvid = %x,svid = %x\n",c_vid,s_vid));
		return OPL_ERROR;
	}
	
	if(pon_strip_en)
	{
		brg_vtt_entry.common.t_vtt_entry.p_strip = VTT_PON_STRIP_EN;
	}else
	{
		brg_vtt_entry.common.t_vtt_entry.p_strip = VTT_PON_STRIP_DIS;
	}

	if(ge_strip_en)
	{
		brg_vtt_entry.common.t_vtt_entry.g_strip = VTT_GE_STRIP_EN;
	}else
	{
		brg_vtt_entry.common.t_vtt_entry.g_strip = VTT_GE_STRIP_DIS;
	}
	
	brg_vtt_entry.common.t_vtt_entry.s_vid = s_vid;
	brg_vtt_entry.common.t_vtt_entry.c_vid = c_vid;
		
	retVal = brgVttEntryExist(&brg_vtt_entry, &entryId, &find);

	if(OPL_OK != retVal)
	{
		return retVal;
	}

	OPL_MEMSET(&brg_vtt_entry,0X00,sizeof(BRG_VTT_ENTRY_t));

	if(OPL_FIND == find)
	{
		retVal = brgVttEntryHwWrite(entryId, &brg_vtt_entry);
	}
	
	return retVal;
}

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
OPL_STATUS brgTpidHwWrite(UINT16 tpid)
{
	return oplRegFieldWrite(REG_BRG_VLAN_CTRL, 0, 16, tpid);
}

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
OPL_STATUS brgTpidHwRead(UINT16 *tpid)
{
	UINT32 regVal;
	OPL_STATUS retVal = OPL_OK;

	if(OPL_NULL == tpid)
	{
		OPL_LOG_TRACE();
		return OPL_ERR_NULL_POINTER;
	}
	
	retVal =  oplRegFieldRead(REG_BRG_VLAN_CTRL, 0, 16, &regVal);

	if(OPL_OK != retVal)
	{
		return retVal;
	}
	*tpid = regVal;
	return retVal;
}

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
OPL_STATUS brgVttLookupFailActionHwWrite(UINT8 action)
{
	return oplRegFieldWrite(REG_BRG_VLAN_CTRL, 31, 1, action);
}

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
OPL_STATUS brgVttLookupFailActionHwRead(UINT8 *action)
{
	UINT32 regVal;
	OPL_STATUS retVal = OPL_OK;

	if(OPL_NULL == action)
	{
		OPL_LOG_TRACE();
		return OPL_ERR_NULL_POINTER;
	}
	
	retVal =  oplRegFieldRead(REG_BRG_VLAN_CTRL, 31, 1, &regVal);

	if(OPL_OK != retVal)
	{
		return retVal;
	}
	*action = regVal;
	return retVal;
}

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
OPL_STATUS brgVttEntryShow(UINT32 startEntryId,UINT32 endEntryId)
{
	UINT32 entryId;
	OPL_STATUS retVal = OPL_OK;
	BRG_VTT_ENTRY_t brg_vtt_entry;
	UINT8 *statusString[] = {"Disable","Enable"};
	UINT8 *statusActive[] = {"unActive","active"};

	if(startEntryId >= BRG_VTT_TAB_NUM || startEntryId < 0||endEntryId >= BRG_VTT_TAB_NUM || endEntryId < 0)
	{
		OPL_DRV_PRINTK(("out of vtt table range.\n"));
		return OPL_ERROR;
	}

	if(startEntryId > endEntryId)
	{
		OPL_DRV_PRINTK(("startId should be less than endId.\n"));
		return OPL_ERROR;
	}

	OPL_DRV_PRINTK(("%-8s %-8s %-8s %-8s %-8s %-10s\n","entryId", "sVlan", "cVlan", "ponStrip", "geStrip", "active"));
	for(entryId = startEntryId; entryId <= endEntryId;entryId++)
	{
		retVal = brgVttEntryHwRead( entryId, &brg_vtt_entry);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
		OPL_DRV_PRINTK(("%-8d %-8d %-8d %-8s %-8s %-8s\n",entryId,
																			brg_vtt_entry.common.t_vtt_entry.s_vid,
																			brg_vtt_entry.common.t_vtt_entry.c_vid,
																			statusString[brg_vtt_entry.common.t_vtt_entry.p_strip],
																			statusString[brg_vtt_entry.common.t_vtt_entry.g_strip],
																			statusActive[brg_vtt_entry.common.t_vtt_entry.active]));
	}
	return OPL_OK;
}


OPL_STATUS brgVttDirectHwWrite(INT32 entryId,INT32 active,INT32 pStrip,INT32 gStrip,INT32 cVlan,INT32 sVlan)
{
    BRG_T_VTT_ENTRY_t brg_vtt_entry;
    OPL_MEMSET(&brg_vtt_entry,0x00,sizeof(BRG_T_VTT_ENTRY_t));
    brg_vtt_entry.active = active;
    brg_vtt_entry.reserved = 0;
    brg_vtt_entry.p_strip = pStrip;
    brg_vtt_entry.g_strip = gStrip;
    brg_vtt_entry.c_vid = cVlan;
    brg_vtt_entry.s_vid = sVlan;
    return brgVttEntryHwWrite(entryId,(UINT32 *)(&brg_vtt_entry));
}


/* Begin Modified :Add default priority for qinq pvid */
/*******************************************************************************
* brgQinqPvidSet
*
* DESCRIPTION:
*  		This function is used to set qinq pvid function that
*       inserts a stag(pvid) for all untagged upstreams.
*       To accomplish this function, we set qinq pvid as GE's pvid, and use vtt 
*       entry to strip the pvid tag of packets egressing from GE port. 
*
*	INPUTS:
*			UINT32 uiPvid
*			UINT32 uiPriority
*	OUTPUTS:
*		    void
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
/* Begin Modified 2011-5-17*/
OPL_STATUS brgQinqPvidSet(UINT32 uiPvid, UINT32 uiPriority)
{
    UINT32 uiEntryIndex;
    OPL_STATUS OplRet;
    
    if ((4095 <= uiPvid) ||
        (7 < uiPriority))
    {
        return OPL_ERROR;
    }

    /* Set GE's pvid. */
    OplRet = brgGePortLinkControlSet(GE_PORT_PVID, uiPvid);
    if (OPL_OK != OplRet)
    {
        OPL_DRV_PRINTK(("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__));
        return OPL_ERROR;
    }

    /* Set GE's default priority. */
    OplRet = brgGePortLinkControlSet(GE_PORT_COS, uiPriority);
    if (OPL_OK != OplRet)
    {
        OPL_DRV_PRINTK(("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__));
        return OPL_ERROR;
    }

    /* Set vtt entry for pvid. */
	OplRet = brgVttEntryTagAdd(false, true, uiPvid, uiPvid);
    if (OPL_OK != OplRet)
    {
        OPL_DRV_PRINTK(("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__));
        return OPL_ERROR;
    }

    return OPL_OK;
}

/*******************************************************************************
* brgQinqPvidReset
*
* DESCRIPTION:
*  		This function is used to reset qinq pvid function that
*       inserts a stag(pvid) for all untagged upstreams.
*
*	INPUTS:
*			UINT32 uiPvid
*			UINT32 uiPriority
*	OUTPUTS:
*		    void
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
/* Begin Modified 2011-5-17 of*/
OPL_STATUS brgQinqPvidReset(UINT32 uiPvid, UINT32 uiPriority)
{
    UINT32 uiEntryIndex;
    OPL_STATUS OplRet;

    if ((4095 <= uiPvid) ||
        (7 < uiPriority))
    {
        return OPL_ERROR;
    }

    /* Set GE's pvid. */
    OplRet = brgGePortLinkControlSet(GE_PORT_PVID, 0);
    if (OPL_OK != OplRet)
    {
        OPL_DRV_PRINTK(("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__));
        return OPL_ERROR;
    }

    /* Set GE's default priority. */
    OplRet = brgGePortLinkControlSet(GE_PORT_COS, uiPriority);
    if (OPL_OK != OplRet)
    {
        OPL_DRV_PRINTK(("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__));
        return OPL_ERROR;
    }

    /* Set vtt entry for pvid. */
	OplRet = brgVttEntryTagDel(false, true, uiPvid, uiPvid);
    if (OPL_OK != OplRet)
    {
        OPL_DRV_PRINTK(("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__));
        return OPL_ERROR;
    }

    return OPL_OK;
}
/* End   Modified 2011-5-16*/

