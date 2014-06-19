/*
=============================================================================
     Header Name: bridgeport.c

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
#include "bridgeport.h"

/*******************************************************************************
* brgPonPortLinkControlSet
*
* DESCRIPTION:
*  there are two port table in the asic.one is ge port while the other is pon port.
*	
*  this function is used to config the port link control.
*	INPUT:
*		type:
*			PON_PORT_TRSMT_EN
*			PON_PORT_TAG
*			PON_PORT_UNTAG
*			PON_PORT_PVID
*			PON_PORT_COS
*			PON_PORT_I_COS
*			PON_PORT_I_ACTION
*			PON_PORT_LP
*			PON_PORT_RSV
*			PON_PORT_RCV_EN
*		value:
*			
* 
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgPonPortLinkControlSet(BRG_PON_PORT_LINK_CONTROL_e type,UINT32 value)
{	
	OPL_STATUS retVal = OPL_ZERO;
	UINT32 regVal = OPL_ZERO;
	
	switch(type)
	{
		case PON_PORT_TRSMT_EN:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 0, 1, regVal);
			break;
		case PON_PORT_TAG:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 1, 1, regVal);
			break;
		case PON_PORT_UNTAG:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 2, 1, regVal);
			break;
		case PON_PORT_I_COS:
			regVal = value&0x07;
			retVal = oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 3, 3, regVal);
			break;	
		case PON_PORT_I_ACTION:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 6, 1, regVal);
			break;	
		case PON_PORT_PVID:
			regVal = value&0xffff;
			retVal = oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 7, 12, regVal);
			break;
		case PON_PORT_COS:
			regVal = value&0x07;
			retVal = oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 19, 3, regVal);
			break;
		case PON_PORT_RSV:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 22, 1, regVal);
			break;
		case PON_PORT_LP:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 23, 1, regVal);
			break;
		case PON_PORT_RCV_EN:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 24, 1, regVal);
			break;
		default:
			OPL_TRACE();
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
/*******************************************************************************
* brgPonPortLinkControlGet
*
* DESCRIPTION:
*  there are two port entry in opconn, one for ge and other for pon port,the two port entry is two registers
*	
*  this function is get the port link control parameter.
*
*	INPUT:
*		type:
*			PON_PORT_TRSMT_EN
*			PON_PORT_TAG
*			PON_PORT_UNTAG
*			PON_PORT_PVID
*			PON_PORT_COS
*			PON_PORT_I_COS
*			PON_PORT_I_ACTION
*			PON_PORT_LP
*			PON_PORT_RSV
*			PON_PORT_RCV_EN			
* OUTPUTS:
*			value   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgPonPortLinkControlGet(BRG_GE_PORT_LINK_CONTROL_e type,UINT32 *value)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal = OPL_ZERO;

	if(OPL_NULL == value)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	switch(type)
	{
		case PON_PORT_TRSMT_EN:
			retVal = oplRegFieldRead(REG_BRG_PON_PORT_TABLE, 0, 1, &regVal);
			break;
		case PON_PORT_TAG:
			retVal = oplRegFieldRead(REG_BRG_PON_PORT_TABLE, 1, 1, &regVal);
			break;
		case PON_PORT_UNTAG:
			retVal = oplRegFieldRead(REG_BRG_PON_PORT_TABLE, 2, 1, &regVal);
			break;
		case PON_PORT_I_COS:
			retVal = oplRegFieldRead(REG_BRG_PON_PORT_TABLE, 3, 3, &regVal);
			break;	
		case PON_PORT_I_ACTION:
			retVal = oplRegFieldRead(REG_BRG_PON_PORT_TABLE, 6, 1, &regVal);
			break;	
		case PON_PORT_PVID:
			retVal = oplRegFieldRead(REG_BRG_PON_PORT_TABLE, 7, 12, &regVal);
			break;
		case PON_PORT_COS:
			retVal = oplRegFieldRead(REG_BRG_PON_PORT_TABLE, 19, 3, &regVal);
			break;
		case PON_PORT_RSV:
			retVal = oplRegFieldRead(REG_BRG_PON_PORT_TABLE, 22, 1, &regVal);
			break;
		case PON_PORT_LP:
			retVal = oplRegFieldRead(REG_BRG_PON_PORT_TABLE, 23, 1, &regVal);
			break;
		case PON_PORT_RCV_EN:
			retVal = oplRegFieldRead(REG_BRG_PON_PORT_TABLE, 24, 1, &regVal);
			break;
		default:
			OPL_TRACE();
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	if(OPL_OK == retVal)
	{
		*value = regVal;
	}
	return retVal;
}
/*******************************************************************************
* brgGePortLinkControlSet
*
* DESCRIPTION:
*  there are two port entry in opconn, one for ge and other for pon port,the two port entry is two registers
*	
*  this function is config the ge port table
*
*	INPUTS:
*		type:
*			GE_PORT_TRSMT_EN
*			GE_PORT_TAG
*			GE_PORT_UNTAG
*			GE_PORT_I_COS
*			GE_PORT_I_ACTION
*			GE_PORT_PVID
*			GE_PORT_COS
*			GE_PORT_RSV
*			GE_PORT_LP
*			GE_PORT_LERN_LIMIT
*			GE_PORT_LERN_DROP
*			GE_PORT_LERN
*			GE_PORT_RCV_EN
*		vale:
*
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgGePortLinkControlSet(BRG_GE_PORT_LINK_CONTROL_e type,UINT32 value)
{	
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal = OPL_ZERO;
	
	switch(type)
	{
		case GE_PORT_TRSMT_EN:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 0, 1, regVal);	
			break;
		case GE_PORT_TAG:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 1, 1, regVal);	
			break;
		case GE_PORT_UNTAG:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 2, 1, regVal);	
			break;
		case GE_PORT_I_COS:
			regVal = value&0x07;
			retVal = oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 3, 3, regVal);	
			break;
		case GE_PORT_I_ACTION:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 6, 1, regVal);	
			break;
		case GE_PORT_PVID:
			regVal = value&0xfff;
			retVal = oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 7, 12, regVal);	
			break;
		case GE_PORT_COS:
			regVal = value&0x07;
			retVal = oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 19, 3, regVal);	
			break;
		case GE_PORT_RSV:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 22, 1, regVal);
			break;
		case GE_PORT_LP:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 23, 1, regVal);			
			break;
		case GE_PORT_LERN_LIMIT:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 24, 1, regVal);	
			break;
		case GE_PORT_LERN_DROP:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 25, 1, regVal);		
			break;
		case GE_PORT_LERN:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 26, 1, regVal);		
			break;
		case GE_PORT_RCV_EN:
			regVal = value?OPL_ENABLE:OPL_DISABLE;
			retVal = oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 27, 1, regVal);		
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			OPL_TRACE();
			break;
	}
	return retVal;
}
/*******************************************************************************
* brgGePortLinkControlGet
*
* DESCRIPTION:
*  there are two port entry in opconn, one for ge and other for pon port,the two port entry is two registers
*	
*  this function is get the ge port table
*
*	INPUTS:
*		type:
*			GE_PORT_TRSMT_EN
*			GE_PORT_TAG
*			GE_PORT_UNTAG
*			GE_PORT_I_COS
*			GE_PORT_I_ACTION
*			GE_PORT_PVID
*			GE_PORT_COS
*			GE_PORT_RSV
*			GE_PORT_LP
*			GE_PORT_LERN_LIMIT
*			GE_PORT_LERN_DROP
*			GE_PORT_LERN
*			GE_PORT_RCV_EN
*
* OUTPUTS:
*		value
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgGePortLinkControlGet(BRG_GE_PORT_LINK_CONTROL_e type,UINT32 *value)
{	
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal = OPL_ZERO;

	if(OPL_NULL == value)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	switch(type)
	{
		case GE_PORT_TRSMT_EN:
			retVal = oplRegFieldRead(REG_BRG_GE_PORT_TABLE, 0, 1, &regVal);	
			break;
		case GE_PORT_TAG:
			retVal = oplRegFieldRead(REG_BRG_GE_PORT_TABLE, 1, 1, &regVal);	
			break;
		case GE_PORT_UNTAG:
			retVal = oplRegFieldRead(REG_BRG_GE_PORT_TABLE, 2, 1, &regVal);	
			break;
		case GE_PORT_I_COS:
			retVal = oplRegFieldRead(REG_BRG_GE_PORT_TABLE, 3, 3, &regVal);	
			break;
		case GE_PORT_I_ACTION:
			retVal = oplRegFieldRead(REG_BRG_GE_PORT_TABLE, 6, 1, &regVal);	
			break;
		case GE_PORT_PVID:
			retVal = oplRegFieldRead(REG_BRG_GE_PORT_TABLE, 7, 12, &regVal);	
			break;
		case GE_PORT_COS:
			retVal = oplRegFieldRead(REG_BRG_GE_PORT_TABLE, 19, 3, &regVal);	
			break;
		case GE_PORT_RSV:
			retVal = oplRegFieldRead(REG_BRG_GE_PORT_TABLE, 22, 1, &regVal);
			break;
		case GE_PORT_LP:
			retVal = oplRegFieldRead(REG_BRG_GE_PORT_TABLE, 23, 1, &regVal);			
			break;
		case GE_PORT_LERN_LIMIT:
			retVal = oplRegFieldRead(REG_BRG_GE_PORT_TABLE, 24, 1, &regVal);	
			break;
		case GE_PORT_LERN_DROP:
			retVal = oplRegFieldRead(REG_BRG_GE_PORT_TABLE, 25, 1, &regVal);		
			break;
		case GE_PORT_LERN:
			retVal = oplRegFieldRead(REG_BRG_GE_PORT_TABLE, 26, 1, &regVal);		
			break;
		case GE_PORT_RCV_EN:
			retVal = oplRegFieldRead(REG_BRG_GE_PORT_TABLE, 27, 1, &regVal);		
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			OPL_TRACE();
			break;
	}
	if(OPL_OK == retVal)
	{
		*value = regVal;
	}
	return retVal;
}

/*******************************************************************************
* brgPortEntryHwWrite
*
* DESCRIPTION:
*  there are two port entry in opconn, one for ge and other for pon port,the two port entry is two registers
*	
*  this function is used to config the the port entry
* INPUTS:
*       portNum: 0:ge port;1 : pon port,2: fe port
*       pstEntry: entry value to be configed.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgPortEntryHwWrite(UINT32 portNum,UINT32 *pstEntry) 
{
	OPL_STATUS retVal = OPL_OK;

	if(OPL_NULL == pstEntry)
	{
		OPL_DRV_PRINTF(("input NULL pointer.\n"));
		OPL_TRACE();
		return OPL_ERR_NULL_POINTER;
	}
	
	switch(portNum)
	{
		case BRG_PORT_GE:
			retVal = oplRegWrite(REG_BRG_GE_PORT_TABLE,*(UINT32 *)pstEntry);
			break;
		case BRG_PORT_PON:
			retVal = oplRegWrite(REG_BRG_PON_PORT_TABLE,*(UINT32 *)pstEntry);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
	
/*******************************************************************************
* brgPortEntryHwRead
*
* DESCRIPTION:
*  there are two port entry in opconn, one for ge and other for pon port,the two port entry is two registers
*	
*  this function is used to read out the port entry value
* INPUTS:
*       portNum: 0:ge port;1 : pon port,2: fe port
*       pstEntry: entry value to be configed.
*	    
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgPortEntryHwRead(UINT32 portNum,UINT32 *pstEntry) 
{
	OPL_STATUS retVal;

	if(OPL_NULL == pstEntry)
	{
		OPL_DRV_PRINTF(("input NULL pointer.\n"));
		OPL_TRACE();
		return OPL_ERR_NULL_POINTER;
	}
	
	switch(portNum)
	{
		case BRG_PORT_GE:
			retVal = oplRegRead(REG_BRG_GE_PORT_TABLE,(UINT32 *)pstEntry);
			break;
		case BRG_PORT_PON:
			retVal = oplRegRead(REG_BRG_PON_PORT_TABLE,(UINT32 *)pstEntry);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
/*******************************************************************************
* brgPortInit
*
* DESCRIPTION:
*  there are two port entry in opconn, one for ge and other for pon port,the two port entry is two registers
*	
*  this function is init the port entry to default value
* 
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgPortInit(void)
{
	BRG_PORT_TAB_ENTRY_t portEntry;
	UINT32 portNum;
	OPL_STATUS retVal;
	/* drop packets when Classify module finishes processing the packet.*/
	brgGePortLinkControlSet(GE_PORT_TRSMT_EN, 0);
	brgPonPortLinkControlSet(PON_PORT_TRSMT_EN, 0);
	
	OPL_MEMSET(&portEntry,0x00,sizeof(BRG_PORT_TAB_ENTRY_t));

	for(portNum = BRG_PORT_GE; portNum <= BRG_PORT_PON; portNum++)
	{
		retVal = brgPortEntryHwWrite(portNum, (UINT32 *)&portEntry);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
	}
	return retVal;
}
/*******************************************************************************
* brgPortEntryShow
*
* DESCRIPTION:
*  there are two port entry in opconn, one for ge and other for pon port,the two port entry is two registers
*	
*  this function is init the port entry to default value
*
*	INPUT:
*		startPortNum: 	0 - 1
*		endPortNum:		0 - 1	
* OUTPUTS:
*   
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS brgPortEntryShow(UINT32 startPortNum,UINT32 endPortNum)
{
	UINT32 portNum;
	OPL_STATUS retVal;
	BRG_PORT_TAB_ENTRY_t portEntry;
	
	UINT8 *statusString[] = {"Disable","Enable"};

	if(startPortNum < BRG_PORT_GE ||startPortNum > BRG_PORT_PON ||endPortNum < BRG_PORT_GE ||endPortNum > BRG_PORT_PON )
	{
		OPL_DRV_PRINTF(("out of port num range.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(startPortNum > endPortNum)
	{
		OPL_DRV_PRINTF(("start port num should be less than end portNum.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	for(portNum = startPortNum; portNum <= endPortNum;portNum++)
	{
		retVal = brgPortEntryHwRead(portNum, (UINT32 *)(&portEntry));
		if(OPL_OK != retVal)
		{
			return retVal;
		}

		switch(portNum)
		{
			case BRG_PORT_GE:
				OPL_DRV_PRINTF(("GE PORT:\n"));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","trsmtEn",			statusString[portEntry.common.port_entry_ge.trsmt_en]));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","rcvTagEn",		statusString[portEntry.common.port_entry_ge.tag]));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","rcvUntagEn",		statusString[portEntry.common.port_entry_ge.untag]));
				OPL_DRV_PRINTF(("%-13s:%-10x\n","icos",					portEntry.common.port_entry_ge.i_cos));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","icosEn",				statusString[portEntry.common.port_entry_ge.i_action]));
				OPL_DRV_PRINTF(("%-13s:%-10x\n","pvid",					portEntry.common.port_entry_ge.pvid));
				OPL_DRV_PRINTF(("%-13s:%-10x\n","cos",					portEntry.common.port_entry_ge.cos));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","rcvRsvEn",			statusString[portEntry.common.port_entry_ge.rsv_en]));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","lpEn",					statusString[portEntry.common.port_entry_ge.lp]));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","lernLimitEn",		statusString[portEntry.common.port_entry_ge.lern_limit]));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","lernLermDrop",	statusString[portEntry.common.port_entry_ge.lern_drop]));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","lernEn",				statusString[portEntry.common.port_entry_ge.lern]));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","rcvEn",				statusString[portEntry.common.port_entry_ge.rcv_en]));
				break;
			case BRG_PORT_PON:
				OPL_DRV_PRINTF(("PON PORT:\n"));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","trsmtEn",			statusString[portEntry.common.port_entry_pon.trsmt_en]));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","rcvTagEn",		statusString[portEntry.common.port_entry_pon.tag]));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","rcvUntagEn",		statusString[portEntry.common.port_entry_pon.untag]));
				OPL_DRV_PRINTF(("%-13s:%-10x\n","icos",					portEntry.common.port_entry_pon.i_cos));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","icosEn",				statusString[portEntry.common.port_entry_pon.i_action]));
				OPL_DRV_PRINTF(("%-13s:%-10x\n","pvid",					portEntry.common.port_entry_pon.pvid));
				OPL_DRV_PRINTF(("%-13s:%-10x\n","cos",					portEntry.common.port_entry_pon.cos));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","rcvRsvEn",			statusString[portEntry.common.port_entry_pon.rsv_en]));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","lpEn",					statusString[portEntry.common.port_entry_pon.lp]));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","lernLimitEn",		statusString[portEntry.common.port_entry_pon.lern_limit]));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","lernLermDrop",	statusString[portEntry.common.port_entry_pon.lern_drop]));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","lernEn",				statusString[portEntry.common.port_entry_pon.lern]));
				OPL_DRV_PRINTF(("%-13s:%-10s\n","rcvEn",				statusString[portEntry.common.port_entry_pon.rcv_en]));
				break;
			default:
				OPL_DRV_PRINTF(("no such portnum.\n"));
				retVal = OPL_ERROR;
				break;
		}
	}
	return retVal;
}
