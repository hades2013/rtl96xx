/*
=============================================================================
     File Name: dal_mac.c

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

UINT32 ARLMACNUMLIMIT = PON_BRG_MAC_ENTRY_NUM;


/*******************************************************************************
* dalArlAgeTimeSet
*
* DESCRIPTION:
*  		this function is used to set the arl age time interval
*
*	INPUTS:
*			ageingTime:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlAgeTimeSet(UINT32 ageingTime)
{
	OPL_STATUS iRet = OPL_OK ;
	
	/* 0 for ARL Entry age disable */
	if (0 == ageingTime)
	{
		iRet = brgArlAgeEnHwWrite(OPL_DISABLE);
	}
	else
	{		
		/* N for N+1 seconds in ARL Entry Age Time */
		iRet += brgArlAgeTimeHwWrite(ageingTime - 1);
		
		/* disable and enable ARL Entry age to validate the configuration */
		iRet += brgArlAgeEnHwWrite(OPL_DISABLE);
		iRet = brgArlAgeEnHwWrite(OPL_ENABLE);
	}
	return iRet;
}
/*******************************************************************************
* dalArlAgeTimeGet
*
* DESCRIPTION:
*  		this function is used to get the arl age time interval
*
*	INPUTS:
*
*	OUTPUTS:
*		ageingTime	     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlAgeTimeGet(UINT32 *ageingTime)
{
	return  brgArlAgeTimeHwRead(ageingTime);
}
/*******************************************************************************
* dalArlMacAdd
*
* DESCRIPTION:
*  		this function is used to add a port-mac map to arl table.the mac address may be unicast or 
*	multicast.
*
*	INPUTS:
*		portId: 		portId
*		macAddress:	mac
*		vlanIndex:	vlanIndex
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacAdd(UINT8 portNum,UINT8 *macAddress,UINT16 vlanIndex)
{
	return brgArlMacAdd(0,vlanIndex,macAddress);	
}
/*******************************************************************************
* dalArlMacDel
*
* DESCRIPTION:
*  		this function is used to del a port-mac map from arl table.the mac address may be unicast or 
*	multicast.if the macAddress is a unicast mac address,then remove the mac from arl,or only remove 
*	the portVec from the arl records.
*
*	INPUTS:
*		portId: 		portId
*		macAddress:	mac
*		vlanIndex:	vlanIndex
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacDel(UINT8 portNum,UINT8 *macAddress,UINT16 vlanIndex)
{
	return brgArlMacDel(0,vlanIndex,macAddress);
}
/*******************************************************************************
* dalArlMacRemove
*
* DESCRIPTION:
*  		this function is used to remove a mac address from the arl table of the vlanIndex.
*
*	INPUTS:
*		macAddress:	mac
*		vlanIndex:	vlanIndex
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacRemove(UINT8 *macAddress,UINT16 vlanIndex)
{
	return brgArlMacDel(0,vlanIndex,macAddress);
}
/*******************************************************************************
* dalArlFlushDynamic
*
* DESCRIPTION:
*  		this function is used to flush all unicast mac address learn by the arl.
*
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO:
*		modified by jiangmingli, 2008-09-09
*/
OPL_STATUS dalArlFlushDynamic(void)
{
	UINT32 uiLearnEnable;
	PON_BRG_MAC_TAB_t stMacEntry;
	PON_BRG_MAC_TAB_t stTempMacEntry;
	UINT32 uiEntryId;
	OPL_STATUS iRet;

	/* disable mac aging first to avoid learning mac entry in flush process */
	oplRegFieldRead(REG_BRG_GE_PORT_TABLE,26, 1, &uiLearnEnable);
	oplRegFieldWrite(REG_BRG_GE_PORT_TABLE,26, 1, OPL_DISABLE);
	
	OPL_MEMSET(&stMacEntry, 0X00, sizeof(PON_BRG_MAC_TAB_t));
	for (uiEntryId = OPL_ZERO; uiEntryId < ARLMACNUMLIMIT; uiEntryId++)
	{
		iRet = brgArlEntryHwRead(uiEntryId, &stTempMacEntry);
		if (OPL_OK != iRet)
		{
			return iRet;
		}

		if (VALID_STATIC == stTempMacEntry.state)
		{
			continue;
		}

		iRet = brgArlEntryHwWrite(uiEntryId,&stMacEntry);	
		if (OPL_OK != iRet)
		{
			return iRet;
		}
	}


	/*stMacEntry.state = NOT_VALID_STATIC;*/
	/*modified by ltang ,change NOT_VALID_STATIC to VALID_STATIC for bug3211   ---start*/
	stMacEntry.state = VALID_STATIC;
	/*modified by ltang ,change NOT_VALID_STATIC to VALID_STATIC for bug3211   ---end*/
	
	for (uiEntryId = ARLMACNUMLIMIT; uiEntryId < PON_BRG_MAC_ENTRY_NUM; uiEntryId++)
	{
		iRet = brgArlEntryHwWrite(uiEntryId, &stMacEntry);	
		if (OPL_OK != iRet)
		{
			return iRet;
		}
	}

	/* recover the LearnEnable state */
	oplRegFieldWrite(REG_BRG_GE_PORT_TABLE,26, 1, uiLearnEnable);
	
	return OPL_OK;
}
/*******************************************************************************
* dalArlFlushAll
*
* DESCRIPTION:
*  		this function is used to flush all mac address from arl table.
*
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*		modified by jiangmingli, 2008-09-09
*/
OPL_STATUS dalArlFlushAll(void)
{
	UINT32 uiLearnEnable;
	PON_BRG_MAC_TAB_t stMacEntry;
	UINT32 uiEntryId;
	OPL_STATUS iRet;

	/* disable mac aging first to avoid learning mac entry in flush process */
	oplRegFieldRead(REG_BRG_GE_PORT_TABLE,26, 1, &uiLearnEnable);
	oplRegFieldWrite(REG_BRG_GE_PORT_TABLE,26, 1, OPL_DISABLE);
	
	OPL_MEMSET(&stMacEntry, 0X00, sizeof(PON_BRG_MAC_TAB_t));
	for (uiEntryId = OPL_ZERO; uiEntryId < ARLMACNUMLIMIT; uiEntryId++)
	{
		iRet = brgArlEntryHwWrite(uiEntryId,&stMacEntry);	
		if (OPL_OK != iRet)
		{
			return iRet;
		}
	}

	/*stMacEntry.state = NOT_VALID_STATIC;*/
	/*modified by ltang ,change NOT_VALID_STATIC to VALID_STATIC for bug3211   ---start*/
	stMacEntry.state = VALID_STATIC;
	/*modified by ltang ,change NOT_VALID_STATIC to VALID_STATIC for bug3211   ---end*/
	for (uiEntryId = ARLMACNUMLIMIT; uiEntryId < PON_BRG_MAC_ENTRY_NUM; uiEntryId++)
	{
		iRet = brgArlEntryHwWrite(uiEntryId, &stMacEntry);	
		if (OPL_OK != iRet)
		{
			return iRet;
		}
	}

	/* recover the LearnEnable state */
	oplRegFieldWrite(REG_BRG_GE_PORT_TABLE,26, 1, uiLearnEnable);
	
	return OPL_OK;
}

/*******************************************************************************
* dalArlLearnEnSet
*
* DESCRIPTION:
*  		this function is used to enable or disable arl leanning function
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
OPL_STATUS dalArlLearnEnSet(UINT8 enable)
{
	return brgArlLearnEnHwWrite(enable);
}

/*******************************************************************************
* dalArlLearnEnGet
*
* DESCRIPTION:
*  		this function is used to get the arl learning function status
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
OPL_STATUS dalArlLearnEnGet(UINT8 *enable)
{
	return brgArlLearnEnHwRead(enable);
}

/*******************************************************************************
* dalArlSoftLearnEnSet
*
* DESCRIPTION:
*  		this function is used to enable or disable arl leanning function
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
OPL_STATUS dalArlSoftLearnEnSet(UINT8 enable)
{
    OPL_STATUS ulRet;

    ulRet = brgArlSoftLearnEnHwWrite(enable);
    ulRet += intBrgEnHwWrite(ARL_FULL_ARL_NEW_REQ_INT_TYPE, enable);
	return ulRet;
}

/*******************************************************************************
* dalArlSoftLearnEnGet
*
* DESCRIPTION:
*  		this function is used to get the arl learning function status
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
OPL_STATUS dalArlSoftLearnEnGet(UINT8 *enable)
{
	return brgArlSoftLearnEnHwRead(enable);
}

/*******************************************************************************
* dalArlAgeEnableSet
*
* DESCRIPTION:
*  		this function is used to enable or disable arl age function
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
OPL_STATUS dalArlAgeEnableSet(UINT8 enable)
{
	return brgArlAgeEnHwWrite(enable);
}

/*******************************************************************************
* dalArlSoftLearnEnGet
*
* DESCRIPTION:
*  		this function is used to get the arl age function status
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
OPL_STATUS dalArlAgeEnableGet(UINT8 *enable)
{
	return brgArlAgeEnHwRead(enable);
}

/*******************************************************************************
* dalArlMacNumLimitSet
*
* DESCRIPTION:
*  		this function is used to set num of arl mac limit
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
OPL_STATUS dalArlMacNumLimitSet(UINT32 portID, UINT8 enable, UINT32 num)
{
	ARLMACNUMLIMIT = num;
	return brgArlMacNumLimitSet(enable, num);
}

/* begin added by jiangmingli, 2008-08-26 */
/*******************************************************************************
* dalArlMultiPortMacAdd
*
* DESCRIPTION:
*  		this function is used to add an atu entry with multiple ports
*	if the atu entry exists already, add the portlist to this atu entry
*	if the atu entry not exists, add atu entry with the portlist
*
*	INPUTS:
*		uiPortNum	Port number for Port List
*		auiPortlist	the Port List associated with the mac
*		macAddress	the mac address of atu entry
*		usVlanId		Vlanid associated with the mac
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK 		success
* 			OPL_ERROR 	failed
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMultiPortMacAdd
(
	UINT32 uiPortNum,
	UINT32 *auiPortlist,
	UINT8 *aucMacAddress,
	UINT16 usVlanId
)
{
	OPL_STATUS iRet;
	
	iRet = brgArlMacAdd(0,usVlanId, aucMacAddress);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return OPL_ERR_REG_WRITE_FAIL;
	}
	
	return OPL_OK;
}

/*******************************************************************************
* dalArlMultiPortMacDel
*
* DESCRIPTION:
*  		this function is used to delete multiple ports from the desired atu entry
*	if the atu entry exists, delete the portlist of this atu entry
*	if the mac not exists, just return OK
*
*	INPUTS:
*		uiPortNum	Port num for Port List
*		auiPortlist		the Port List associated with the mac
*		macAddress	the mac address needed to be added
*		usVlanId		Vlanid associated with the mac
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK 		success
* 			OPL_ERROR 	failed
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMultiPortMacDel
(
	UINT32 uiPortNum,
	UINT32 *auiPortlist,
	UINT8 *aucMacAddress,
	UINT16 usVlanId
)
{
	OPL_STATUS iRet;
	
	iRet = brgArlMacDel(0,usVlanId, aucMacAddress);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return OPL_ERR_REG_WRITE_FAIL;
	}
	
	return OPL_OK;

}


/*******************************************************************************
* dalArlMacEntryFind
*
* DESCRIPTION:
*  		find the desired mac entry info in the desired FID associated with usVlanId
*
*	INPUTS:
*		aucMacAddress	the start mac address of atu entry, NULL for first search
*		usVlanId			Vlanid associated with the mac
*		
*	OUTPUTS:
*		pstMacEntryInfo	the mac entry info
*		pbFind			OPL_TRUE	find an atu entry
*						OPL_FALSE	no atu entry found
* 	RETURNS:
* 			OPL_OK 		success for searching
* 			OPL_ERROR	no desired mac address found 
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacEntryFind
(
	UINT8 *aucMacAddress,
	UINT16 usVlanId,
	FDB_MAC_ENTRY_INFO_S *pstMacEntryInfo,
	OPL_BOOL *pbFind
)
{
	PON_BRG_MAC_TAB_t stMacEntry;
	PON_BRG_MAC_TAB_COMMON_t *pstMacEntryCommon = NULL;
	UINT32 uiMacEntryId;
	UINT8 ucFind;
	OPL_STATUS iRet;

	pstMacEntryCommon = &stMacEntry;
	OPL_MEMSET(pstMacEntryCommon, 0, sizeof(PON_BRG_MAC_TAB_t));
	OPL_MEMCPY(&(pstMacEntryCommon->common.common[2]), aucMacAddress, MAC_LENGTH);
	stMacEntry.vlanId = usVlanId;

	/* mac entry not found, return ok */	
	iRet = brgArlMacExist(&stMacEntry, &uiMacEntryId, &ucFind);
	if ((OPL_OK != iRet) || (OPL_FIND != ucFind))
	{
		*pbFind = OPL_FALSE;
		return OPL_OK;
	}

	iRet = brgArlEntryHwRead(uiMacEntryId, &stMacEntry);
	if (OPL_OK != iRet)
	{
		return OPL_ERR_REG_READ_FAIL;
	}

	/* set mac address */
	OPL_MEMCPY(&(pstMacEntryInfo->aucMacAddress), &(pstMacEntryCommon->common.common[2]), MAC_LENGTH);
	
	/* set port bitmap */
	pstMacEntryInfo->usPortBitmap = DAL_MAC_GE_PORT_BITMAP;

	/* set port state */
	if (VALID_HARDWARE_LEARN == stMacEntry.state)
	{
		pstMacEntryInfo->usType = FDB_MAC_DYNAMIC;
	}
	else if (VALID_STATIC == stMacEntry.state)
	{
		pstMacEntryInfo->usType = FDB_MAC_STATIC;
	}
	else
	{
		pstMacEntryInfo->usType = FDB_MAC_INVALID;
	}

	/* set vlan id */
	pstMacEntryInfo->usVlanId = (UINT16)(stMacEntry.vlanId);

	*pbFind = OPL_TRUE;
	
	return OPL_OK;
}

/*******************************************************************************
* dalArlMacEntryTraverse
*
* DESCRIPTION:
*  		traverse the whole atu entry in the desired FID associated with usVlanId
*	if aucMacAddress is NULL, return the first mac entry info, else return the next mac entry info
*	whose mac address is aucMacAddress
*
*	INPUTS:
*		aucMacAddress	the start mac address of atu entry, NULL for first search
*		usVlanId			Vlanid associated with the mac
*		
*	OUTPUTS:
*		pstMacEntryInfo	the next mac entry
*		pbFind			OPL_TRUE	find an atu entry
*						OPL_FALSE	no atu entry found
* 	RETURNS:
* 			OPL_OK 		success for searching
* 			OPL_ERROR	no desired mac address found 
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacEntryTraverse
(
	UINT32 *puiStartMacEntryId,
	FDB_MAC_ENTRY_INFO_S *pstMacEntryInfo,
	OPL_BOOL *pbFind
)
{
	UINT32 uiIndex;
	PON_BRG_MAC_TAB_t stMacEntry;
	PON_BRG_MAC_TAB_COMMON_t *pstMacEntryCommon = NULL;
	OPL_STATUS iRet;

	pstMacEntryCommon = &stMacEntry;

	uiIndex = *puiStartMacEntryId;
	while (uiIndex < PON_BRG_MAC_ENTRY_NUM)
	{
		iRet = brgArlEntryHwRead(uiIndex, &stMacEntry);
		if(OPL_OK != iRet)
		{
			return iRet;
		}

		if ((VALID_HARDWARE_LEARN == stMacEntry.state) || (VALID_STATIC == stMacEntry.state))
		{
			/* set mac entry type */
			if (VALID_HARDWARE_LEARN == stMacEntry.state)
			{
				pstMacEntryInfo->usType = FDB_MAC_DYNAMIC;
			}
			else if (VALID_STATIC == stMacEntry.state)
			{
				pstMacEntryInfo->usType = FDB_MAC_STATIC;
			}

            /* set mac entry port type */
			if (FDB_PORT_TYPE_PON == stMacEntry.portId)
			{
				pstMacEntryInfo->usPortType = FDB_PORT_TYPE_PON;
			}
			else
			{
				pstMacEntryInfo->usPortType = FDB_PORT_TYPE_GE;
			}

			/* set mac entry address */
			OPL_MEMCPY(pstMacEntryInfo->aucMacAddress, &(pstMacEntryCommon->common.common[2]), MAC_LENGTH);

			/* set vlan id */
			pstMacEntryInfo->usVlanId = (UINT16)(stMacEntry.vlanId);
			
			*puiStartMacEntryId = uiIndex;
			*pbFind = OPL_TRUE;
			return OPL_OK;
		}

		uiIndex++;
	}

	*puiStartMacEntryId = uiIndex;
	*pbFind = OPL_FALSE;
	return OPL_OK;
}

#if 0
/*******************************************************************************
* dalArlMacEntryShowOne
*
* DESCRIPTION:
*		show the information of desired mac entry
*		if the mac entry not exists, this function will print "no such mac entry"
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
OPL_STATUS dalArlMacEntryShowOne
(
	INT32 lFd,
	UINT8 *aucMacAddress,
	UINT16 usType,
	UINT32 uiPortNum,
	UINT32 *auiPortlist
)
{
	UINT32 uiMacEntryId;
	FDB_MAC_ENTRY_INFO_S stMacEntryInfo;
	OPL_BOOL bFind;
	UINT32 uiMacEntryNum = 0;
	UINT8 szType[10];
	UINT8 szMacaddress[20];
	OPL_STATUS iRet;
	UINT32 cnt = 0;
	uiMacEntryId = 0;
	while (uiMacEntryId < PON_BRG_MAC_ENTRY_NUM)
	{
		iRet = dalArlMacEntryTraverse(&uiMacEntryId, &stMacEntryInfo, &bFind);
		if ((OPL_OK != iRet) || (OPL_TRUE != bFind))
		{
			/* no atu entry exists in the FID */
			if (0 == uiMacEntryNum)
			{
				vosPrintf(lFd, "no mac entry exists.\r\n");
			}

			break;
		}

		uiMacEntryNum++;
		uiMacEntryId++;

		/* first mac entry, printf the format */
		if (1 == uiMacEntryNum)
		{
			vosPrintf(lFd, "Mac-Address          Vlan     Port Number  Type\r\n");
			vosPrintf(lFd, "-------------------- -------- ------------ ------------\r\n");
		}

		/* if the mac address not concerned, continue to get the next mac entry  */
		if (0 != OPL_MEMCMP(aucMacAddress, stMacEntryInfo.aucMacAddress, MAC_LENGTH))
		{
			continue;
		}

		/* if the mac entry type not concerned, continue to get the next mac entry */
		if ((FDB_MAC_TYPE_MAX != usType) && (stMacEntryInfo.usType != usType))
		{
			continue;
		}
		/* transform mac entry type to string */
		switch(stMacEntryInfo.usType)
		{
			case FDB_MAC_INVALID:
			{
				OPL_MEMCPY(szType, "INVALID", 10);
				break;
			}
			case FDB_MAC_DYNAMIC:
			{
				OPL_MEMCPY(szType, "Dynamic", 10);
				break;
			}
			case FDB_MAC_STATIC:
			{
				OPL_MEMCPY(szType, "Static", 10);
				break;
			}
			default:
			{
				OPL_MEMCPY(szType, "Unknown", 10);
				break;
			}
		}

		oplMacToStr(stMacEntryInfo.aucMacAddress, szMacaddress);

		/* print mac entry information */
        if (stMacEntryInfo.usPortType == FDB_PORT_TYPE_PON)
        {
		    vosPrintf(lFd, "%-20s %-8d %-12s %-12s\r\n", szMacaddress, stMacEntryInfo.usVlanId, "0", szType);
        }
        else
        {
            vosPrintf(lFd, "%-20s %-8d %-12s %-12s\r\n", szMacaddress, stMacEntryInfo.usVlanId, "1", szType);
        }
	cnt++;
	}
	vosPrintf(lFd, "The number of FDB: %d\r\n", cnt);
	return OPL_OK;
}

/*******************************************************************************
* dalArlMacEntryShowAll
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
OPL_STATUS dalArlMacEntryShowAll
(
	INT32 lFd,
	UINT16 usType,
	UINT32 uiPortNum,
	UINT32 *auiPortlist
)
{
	UINT32 uiMacEntryId;
	FDB_MAC_ENTRY_INFO_S stMacEntryInfo;
	OPL_BOOL bFind;
	UINT32 uiMacEntryNum = 0;
	UINT8 szType[10];
	UINT8 szMacaddress[20];
        UINT8 invalidMacAddress[6] = {0,0,0,0,0,0};
	OPL_STATUS iRet;
	UINT32 cnt = 0;
	uiMacEntryId = 0;

	while (uiMacEntryId < PON_BRG_MAC_ENTRY_NUM)
	{
		iRet = dalArlMacEntryTraverse(&uiMacEntryId, &stMacEntryInfo, &bFind);
		if ((OPL_OK != iRet) || (OPL_TRUE != bFind))
		{
			/* no mac entry exists in the FID */
			if (0 == uiMacEntryNum)
			{
				vosPrintf(lFd, "\r\nno mac entry exists.\r\n");
			}

			break;
		}

		uiMacEntryNum++;
		uiMacEntryId++;

		/* first mac entry, printf the format */
		if (1 == uiMacEntryNum)
		{
			vosPrintf(lFd, "Mac Address          Vlan     Port Number  Type\r\n");
			vosPrintf(lFd, "-------------------- -------- ------------ ------------\r\n");
		}

		/* if the mac entry type not concerned, continue to get the next mac entry */
		if ((FDB_MAC_TYPE_MAX != usType) && (stMacEntryInfo.usType != usType))
		{
			continue;
		}

        /* if the mac address is invalid, continue to get the next mac entry  */
		if ((0 == OPL_MEMCMP(invalidMacAddress, stMacEntryInfo.aucMacAddress, MAC_LENGTH)) && 
            (FDB_MAC_STATIC == stMacEntryInfo.usType))
		{
			continue;
		}
		cnt++;

		/* transform mac entry type to string */
		switch(stMacEntryInfo.usType)
		{
			case FDB_MAC_INVALID:
			{
				OPL_MEMCPY(szType, "INVALID", 10);
				break;
			}
			case FDB_MAC_DYNAMIC:
			{
				OPL_MEMCPY(szType, "Dynamic", 10);
				break;
			}
			case FDB_MAC_STATIC:
			{
				OPL_MEMCPY(szType, "Static", 10);
				break;
			}
			default:
			{
				OPL_MEMCPY(szType, "Unknown", 10);
				break;
			}
		}

		oplMacToStr(stMacEntryInfo.aucMacAddress, szMacaddress);

		/* print mac entry information */
        if (stMacEntryInfo.usPortType == FDB_PORT_TYPE_PON)
        {
		    vosPrintf(lFd, "%-20s %-8d %-12s %-12s\r\n", szMacaddress, stMacEntryInfo.usVlanId, "0", szType);
        }
        else
        {
            vosPrintf(lFd, "%-20s %-8d %-12s %-12s\r\n", szMacaddress, stMacEntryInfo.usVlanId, "1", szType);
        }
	}

	/*add by xhpan,for add total entry ---begin */
	vosPrintf(lFd, "The number of FDB: %d\r\n", cnt);
	/*add by xhpan,for add total entry ---end */
	return OPL_OK;
}
#endif
/* begin added by jiangmingli for N:1 aggregation */
OPL_STATUS dalArlSoftLearnFuncRegister(BRG_SOFT_LEARN_FUNC pfFunc)
{
    if (NULL == pfFunc)
    {
        return OPL_ERROR;
    }

    brgArlSoftLearnFuncRegister(pfFunc);

    return OPL_OK;
}
/* end added by jiangmingli for N:1 aggregation */

/* end added by jiangmingli, 2008-08-26 */

OPL_STATUS dalArlMgmtMacRdt2Cpu(UINT8 * oldMac, UINT8 * newMac)
{
    return OPL_OK;
}


