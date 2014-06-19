/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  cli_cmd_fdb.c
*
* DESCRIPTION: 
*	
*
* Date Created: Aug 19, 2008
*
* Authors(optional): 
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_cmd_fdb.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "cli.h"
#include "cli_cmd_list.h"

/* begin added by jiangmingli, 2008-08-27 */
//wfxu #include "dal_mac.h"
#include "odm_port.h"
#include "odm_vlan.h"
#include "opl_debug.h"

#include "opconn_usr_ioctrl.h"

/* end added by jiangmingli, 2008-08-27 */


/* begin added by jiangmingli, 2008-08-25 */
/*******************************************************************************
* cliCmdFdbSetAgingTime
*
* DESCRIPTION:
*		CLI callback function for "agingtime <0-1800>"
*  		set the automatic address aging time
*
*	INPUTS:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 		NO_ERROR									success
*		ERR_NULL_POINTER							NULL point
*		ERR_INVALID_PARAMETERS						aging time invalid
*		FDB_MAC_SET_AGINGTIME_ERROR				set aging time error
* 		FDB_MAC_SET_CONFIG_FIEL_AGINGTIME_ERROR	set config file error
* 	SEE ALSO: 
*/
STATUS cliCmdFdbSetAgingTime(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	UINT32 uiAgingTime;
	STATUS iRet;
	
	/* parameter check */
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		OPL_LOG_TRACE();
		return ERR_NULL_POINTER;
	}

	/* get aging time from pstEnv */
	uiAgingTime = pstPt[0].u;

#ifdef ONU_1PORT
    if (1 == uiAgingTime)
    {
        vosPrintf(pstEnv->nWriteFd, "Not support Aging Time %d seconds.\r\n", uiAgingTime);
        return OPL_OK;
    }
#endif

	/* set aging time */
	iRet = odmFdbAgingTimeSet(uiAgingTime);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}

	/* print set register OK */
    vosPrintf(pstEnv->nWriteFd, "Set Aging Time %d seconds OK.\r\n", uiAgingTime);

	return NO_ERROR;
}

/*******************************************************************************
* cliCmdFdbAddFdbEntry
*
* DESCRIPTION:
*		CLI callback function for "Create fdbentry <macaddr> <portlist> [<vlanname>]"
*  		Add Fdb Entry
*
*	INPUTS:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 		NO_ERROR					success
* 		ERR_NULL_POINTER			NULL point
*		FDB_MAC_PORT_ADD_ERROR	add multiple ports error
* 	SEE ALSO: 
*/
STATUS cliCmdFdbAddFdbEntry(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	UINT8 aucMacAddress[MAC_LENGTH];
	UINT8  macAddrStr[32];
	UINT32 uiPortNum;
	UINT32 auiPortlist[ODM_NUM_OF_PORTS];
	UINT16 usVlanId;	
	STATUS iRet;

	/* parameter check */
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		OPL_LOG_TRACE();
		return ERR_NULL_POINTER;
	}
	vosStrCpy(macAddrStr, pstPt[0].p);
	/*-----cann't configure multicast mac -------*/
	if(((vosMacCharToDigit(macAddrStr[1]))%2) == 1)
	{
		vosPrintf(pstEnv->nWriteFd, "\nerror:cann't configure multicast mac\n");
		return 	ERROR;
	}
	/* get macaddr */
	iRet = vosStrToMac(pstPt[0].p, aucMacAddress);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}

	/* get portlist */
	iRet = cliGetPortlist(pstPt[1].p, ODM_NUM_OF_PORTS, auiPortlist, &uiPortNum);
	if (0 != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}

	/* get Vlanid */
	#if defined(ONU_1PORT) || defined(ONU_4PORT_AR8327)
	usVlanId = pstPt[2].u;
	#else
	usVlanId = 0;
	#endif

	/* add one mac Entry */
	iRet = odmFdbMacPortAdd(uiPortNum, auiPortlist, aucMacAddress, usVlanId);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}

	return NO_ERROR;
}

/*******************************************************************************
* cliCmdFdbDelFdbEntry
*
* DESCRIPTION:
*		CLI callback function for "Delete fdbentry {<macaddr>|all} [<vlanname>]"
*  		Delete Fdb Entry
*
*	INPUTS:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 		NO_ERROR							success
*		ERR_NULL_POINTER					NULL point
*		FDB_MAC_ADD_ERROR					add mac entry error
*		FDB_MAC_DEL_ALL_ERROR				delete all mac entry error
* 	SEE ALSO: 
*/
STATUS cliCmdFdbDelFdbEntry(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	UINT8 aucMacAddress[MAC_LENGTH];
	BOOL_T bDelAll = OPL_TRUE;
	UINT16 usVlanId;	
	STATUS iRet;
	
	/* parameter check */
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		OPL_LOG_TRACE();
		return ERR_NULL_POINTER;
	}

	/* get mac address */
	switch (pstPt[0].i)
	{
		case 1:
		{
			iRet = vosStrToMac(pstPt[2].p, aucMacAddress);
			if (NO_ERROR != iRet)
			{
				OPL_LOG_TRACE();
				return iRet;
			}

			bDelAll = OPL_FALSE;
			break;
		}
		case 2:
		{
			bDelAll = OPL_TRUE;
			break;
		}
		default:
		{
			break;
		}
	}

	/* get Vlanid */
	#if defined(ONU_1PORT) || defined(ONU_4PORT_AR8327)
	usVlanId = pstPt[3].u;
	#else
	usVlanId = 0;
	#endif

	/* delete one Fdb entry */
	if (OPL_FALSE == bDelAll)
	{
		iRet = odmFdbMacDel(aucMacAddress, usVlanId);
		if (NO_ERROR != iRet)
		{
			OPL_LOG_TRACE();
			return iRet;
		}
	}
	/* delete all Fdb entry */
	else
	{
		iRet = odmFdbMacClearAll();
		if (NO_ERROR != iRet)
		{
			OPL_LOG_TRACE();
			return iRet;
		}
	}

	return NO_ERROR;
}

/*******************************************************************************
* cliCmdFdbDiscardFdbEntry
*
* DESCRIPTION:
*		CLI callback function for "Discard fdbentry <macaddr>"
*  		drop all data packets from this mac address 
*
*	INPUTS:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 		NO_ERROR							success
*		ERR_NULL_POINTER					NULL point
* 	       FDB_MAC_DISCARD_ERROR				discard mac entry error
*		DB_MAC_DISCARD_CLEAR_ERROR		clear discard mac entry error
* 	SEE ALSO: 
*/
STATUS cliCmdFdbDiscardFdbEntry(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{	
	UINT8 aucMacAddress[MAC_LENGTH];
	UINT32 uiPortNum;
	UINT32 auiPortlist[ODM_NUM_OF_PORTS];
	BOOL_T bSet;
	STATUS iRet;

	/* parameter check */
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		OPL_LOG_TRACE();
		return ERR_NULL_POINTER;
	}

	/* get macaddr */
	iRet = vosStrToMac(pstPt[0].p, aucMacAddress);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}

	if (0 == pstPt[1].i)
	{
		bSet = OPL_TRUE;
	}
	else
	{
		bSet = OPL_FALSE;
	}
	

	/* discard mac entry */
	if (OPL_TRUE == bSet)
	{
		iRet = odmFdbMacDiscard(aucMacAddress);
	}
	/* don't discard mac entry */
	else
	{
		iRet = odmFdbMacDiscardClear(aucMacAddress);
	}
	
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
	
	return NO_ERROR;
}


/*******************************************************************************
* cliCmdFdbMoveFdbEntry
*
* DESCRIPTION:
*		CLI callback function for "Move fdbentry <portId> <portId>"
*  		move fdb from one port to another 
*
*	INPUTS:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 		NO_ERROR							success
*		ERR_NULL_POINTER					NULL point
* 	SEE ALSO: 
*/
STATUS cliCmdFdbMoveFdbEntry(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{	
	UINT32 uiPortNum;
	UINT32 uiToPortNum;
	BOOL_T bSet;
	STATUS iRet;

	/* parameter check */
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		OPL_LOG_TRACE();
		return ERR_NULL_POINTER;
	}

	/* move mac Entry */
	iRet = odmFdbMove(pstPt[0].p, pstPt[1].p);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
	
	return NO_ERROR;
}

/*******************************************************************************
* cliCmdFdbSetSwLearn
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
STATUS cliCmdFdbSetSwLearn(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    BOOL_T bEnable;
	STATUS iRet;
    
    /* parameter check */
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		OPL_LOG_TRACE();
		return ERR_NULL_POINTER;
	}

	if (1 == pstPt[0].i)
	{
		bEnable = OPL_FALSE;
	}
	else
	{
		bEnable = OPL_TRUE;
	}

    iRet = odmFdbSwLearnSet(bEnable);
    if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
	
	return NO_ERROR;
}

/*******************************************************************************
* cliCmdFdbShowSwLearn
*
* DESCRIPTION:
*		show fdb software learning state
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					        success
* 		FDB_MAC_SOFT_LEARN_GET_ERROR	 get fdb software learning state error
* 	SEE ALSO: 
*/
STATUS cliCmdFdbShowSwLearn(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    BOOL_T bEnable;
	STATUS iRet;
    
    /* parameter check */
	if (NULL == pstEnv)
	{
		OPL_LOG_TRACE();
		return ERR_NULL_POINTER;
	}

    iRet = odmFdbSwLearnGet(&bEnable);
    if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}

    if (OPL_TRUE == bEnable)
    {
        vosPrintf(pstEnv->nWriteFd, "Software learning is enabled.\r\n");
    }
    else
    {
        vosPrintf(pstEnv->nWriteFd, "Software learning is disabled.\r\n");
    }
	
	return NO_ERROR;
}



/*******************************************************************************
* cliCmdShowAgingTime
*
* DESCRIPTION:
*		CLI callback function for "Show fdb aging time"
*  		show the automatic address aging time
*
*	INPUTS:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 		NO_ERROR								success
* 		ERR_NULL_POINTER						NULL point
*		FDB_MAC_GET_AGINGTIME_ERROR			get aging time error
* 	SEE ALSO: 
*/
STATUS cliCmdShowAgingTime(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{	
	UINT32 uiAgingTime;
	STATUS iRet;
	
	/* parameter check */
	if (NULL == pstEnv)
	{
		OPL_LOG_TRACE();
		return ERR_NULL_POINTER;
	}

	/* read ATU control register */
	iRet = odmFdbAgingTimeGet(&uiAgingTime);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
	
	/* print the automatic address aging time */
    vosPrintf(pstEnv->nWriteFd, "Aging Time is %d seconds.\r\n", uiAgingTime);

	return NO_ERROR;

}

/*******************************************************************************
* cliCmdShowFdbAll
*
* DESCRIPTION:
*		CLI callback function for "Show fdb all"
*  		show all Fdb Entry 
*
*	INPUTS:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 		NO_ERROR								success
* 		ERR_NULL_POINTER						NULL point
* 	SEE ALSO: 
*/
STATUS cliCmdShowFdbAll(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	STATUS iRet;

	/* parameter check */
	if (NULL == pstEnv)
	{
		OPL_LOG_TRACE();
		return ERR_NULL_POINTER;
	}

	/* show all mac entry */
    cliShowStart(pstEnv);
	iRet = odmFdbMacShowAll(pstEnv->nWriteFd, FDB_MAC_TYPE_MAX, 0, NULL);
    cliShowEnd(pstEnv);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}

	return NO_ERROR;
}

/*******************************************************************************
* cliCmdShowFdbStatic
*
* DESCRIPTION:
*		CLI callback function for "Show fdb static {[mac] <macaddr>}"
*  		show Static Fdb Entry
*
*	INPUTS:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 		NO_ERROR								success
* 		ERR_NULL_POINTER						NULL point
* 	SEE ALSO: 
*/
STATUS cliCmdShowFdbStatic(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	UINT8 aucMacAddress[MAC_LENGTH];
	STATUS iRet;
	
	/* parameter check */
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		OPL_LOG_TRACE();
		return ERR_NULL_POINTER;
	}

	/* show all Static Atu Entry */
	if (0 == pstPt[0].i)
	{
        cliShowStart(pstEnv);
		iRet = odmFdbMacShowAll(pstEnv->nWriteFd, FDB_MAC_STATIC, 0, NULL);
        cliShowEnd(pstEnv);
	}
	/* show one desired Static Atu Entry */
	else
	{
		iRet = vosStrToMac(pstPt[1].p, aucMacAddress);
		if (NO_ERROR != iRet)
		{
			OPL_LOG_TRACE();
			return iRet;
		}

		iRet = odmFdbMacShowOne(pstEnv->nWriteFd, aucMacAddress, FDB_MAC_STATIC, 0, NULL);
	}

	return iRet;
}

STATUS cliCmdShowFdbDiscard(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	UINT8 aucMacAddress[MAC_LENGTH];
	STATUS iRet;
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		OPL_LOG_TRACE();
		return ERR_NULL_POINTER;
	}
#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	if (0 == pstPt[0].i)
	{
        cliShowStart(pstEnv);
		iRet = odmFdbMacShowAll(pstEnv->nWriteFd, FDB_MAC_DISCARD, 0, NULL);
        cliShowEnd(pstEnv);
	}
	else
	{
		iRet = vosStrToMac(pstPt[1].p, aucMacAddress);
		if (NO_ERROR != iRet)
		{
			OPL_LOG_TRACE();
			return iRet;
		}
		iRet = odmFdbMacShowOne(pstEnv->nWriteFd, aucMacAddress, FDB_MAC_DISCARD, 0, NULL);
	}
#endif
	return iRet;
}
/*******************************************************************************
* cliCmdShowFdbDynamic
*
* DESCRIPTION:
*		CLI callback function for "Show fdb dynamic {[mac] <macaddr>}"
*  		show dynamic Fdb Entry
*
*	INPUTS:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 		NO_ERROR								success
* 		ERR_NULL_POINTER						NULL point
* 	SEE ALSO: 
*/
STATUS cliCmdShowFdbDynamic(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	UINT8 aucMacAddress[MAC_LENGTH];
	UINT16 usPhyPortBitmap;
	UINT32 uiPortNum;
	UINT32 auiPortlist[ODM_NUM_OF_PORTS];
	UINT16 i;
	STATUS iRet;
	
	/* parameter check */
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		OPL_LOG_TRACE();
		return ERR_NULL_POINTER;
	}

	if (0 == pstPt[0].i)
	{
		uiPortNum = 0;
	}
	else
	{
		/* get portlist */
		iRet = cliGetPortlist(pstPt[1].p, ODM_NUM_OF_PORTS, auiPortlist, &uiPortNum);
		if (0 != iRet)
		{
			OPL_LOG_TRACE();
			return iRet;
		}
	}

	/* show all Dynamic Atu Entry */
	if (0 == pstPt[2].i)
	{
        cliShowStart(pstEnv);
		iRet = odmFdbMacShowAll(pstEnv->nWriteFd, FDB_MAC_DYNAMIC, uiPortNum, auiPortlist);
        cliShowEnd(pstEnv);
	}
	/* show one desired Dynamic Atu Entry */
	else
	{
		iRet = vosStrToMac(pstPt[3].p, aucMacAddress);
		if (NO_ERROR != iRet)
		{
			OPL_LOG_TRACE();
			return iRet;
		}

		iRet = odmFdbMacShowOne(pstEnv->nWriteFd, aucMacAddress, FDB_MAC_DYNAMIC, uiPortNum, auiPortlist);
	}

	return iRet;
}

/*******************************************************************************
* cliCmdShowFdbCfg
*
* DESCRIPTION:
*		CLI callback function for "Show fdb cfg"
*  		show Fdb configure file
*
*	INPUTS:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 		NO_ERROR								success
* 		ERR_NULL_POINTER						NULL point
* 	SEE ALSO: 
*/
STATUS cliCmdShowFdbCfg(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	STATUS iRet;
		
	/* parameter check */
	if (NULL == pstEnv)
	{
		OPL_LOG_TRACE();
		return ERR_NULL_POINTER;
	}

    cliShowStart(pstEnv);
	iRet = odmFdbCfgShow(pstEnv->nWriteFd);
    cliShowEnd(pstEnv);

	return iRet;
}
/* end added by jiangmingli, 2008-08-25 */

STATUS cliCmdPortMacFilterAdd(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId = 1;
	UINT8  macAddrStr[32];
	UINT16 vlanId = 0;

	if (NULL == pstEnv || NULL == pstPara)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
	if(NULL == pstEnv->para)
	{
		return ERROR;
	}
	retVal = cliConvertPortListAndCheck(pstEnv,portArry,ODM_START_PORT_NUN,ODM_NUM_OF_PORTS+1);
	if(OK != retVal)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
		return ERROR;
	}
	vosStrCpy(macAddrStr, pstPara[0].p);
	/*-----cann't configure multicast mac -------*/
	if(((vosMacCharToDigit(macAddrStr[1]))%2) == 1)
	{
		vosPrintf(pstEnv->nWriteFd, "\nerror:cann't configure multicast mac\n");
		return 	ERROR;
	}
#if defined(ONU_1PORT)
	retVal = odmPortMacFilterEntryAdd(portId, vlanId, macAddrStr);
	if(retVal != NO_ERROR)
	{
        if(retVal == ERR_DUPLICATE_ITEM)
            vosPrintf(pstEnv->nWriteFd, "\nerror:this mac exist in bind\n");
		return retVal;	
	}		
#elif defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
		for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
		{
			if(portArry[portId] == 1)
			{
			       #if defined(ONU_4PORT_AR8327)
				    vlanId = pstPara[1].u;
				#endif
				/* add one mac filter */
				retVal = odmPortMacFilterEntryAdd(portId, vlanId, macAddrStr);
				if(retVal != NO_ERROR)
				{
                    if(retVal == ERR_DUPLICATE_ITEM)
                        vosPrintf(pstEnv->nWriteFd, "\nerror:this mac exist in bind\n");
					return retVal;	
				}
			}
		}
#endif

	return OK;
}
STATUS cliCmdPortMacFilterDel(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId = 1;
	UINT8  macAddrStr[32];
	UINT16 vlanId = 0;
	if (NULL == pstEnv || NULL == pstPara)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
	if(NULL == pstEnv->para)
	{
		return ERROR;
	}
	retVal = cliConvertPortListAndCheck(pstEnv,portArry,ODM_START_PORT_NUN,ODM_NUM_OF_PORTS+1);
	if(OK != retVal)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
		return ERROR;
	}
	vosStrCpy(macAddrStr, pstPara[0].p);
#if defined(ONU_1PORT)
	retVal = odmPortMacFilterEntryDel(portId, vlanId, macAddrStr);
	if(retVal != NO_ERROR)
	{
		return retVal;
	}		
#elif defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
		for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
		{
			if(portArry[portId] == 1)
			{
			       #if defined(ONU_4PORT_AR8327)
				vlanId = pstPara[1].u;
				#endif
				/* del one mac filter */
				retVal = odmPortMacFilterEntryDel(portId, vlanId, macAddrStr);
				if(retVal != NO_ERROR)
				{
					return retVal;
				}
			}
		}	
#endif

	return OK;
}



STATUS cliCmdPortMacFilterClear(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;
	UINT8  macAddrStr[32];
	UINT16 vlanId = 0;
	UINT8  macFilterNum;
	UINT8  i;
	if (NULL == pstEnv)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
	retVal = cliConvertPortListAndCheck(pstEnv,portArry,ODM_START_PORT_NUN,ODM_NUM_OF_PORTS+1);
	if(OK != retVal)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
		return ERROR;
	}
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
		   	retVal = odmPortMacFilterEntryClear(portId);
			if(retVal != NO_ERROR)
				return retVal;
		}
	}
	return OK;
}
STATUS cliCmdPortMacFilterShow(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;
	UINT8  macAddrStr[32];
	UINT8 portListBuff[255];
	UINT16 vlanId;
	UINT8  macFilterNum;
	UINT8  i;
	UINT8  j;
	if (NULL == pstEnv)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
	
	if (pstPara)
	{
		switch(pstPara[0].i)
	    {
	        case 1:
	           vosStrCpy(portListBuff,pstPara[2].p);
	            retVal = convertPortListString2PortArry(portListBuff, portArry, ODM_NUM_OF_PORTS+1);
	            if(-1 == retVal)
	            {
	                vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",portListBuff);
	    	        return ERROR;
	        	}
	        	if(portArry[0] == 1)
	        	{
	        		vosPrintf(pstEnv->nWriteFd, "\r\nport num shuld start at 1.\r\n");
	        		return ERROR;
	        	}
	            break;
	        case 2:
	            for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
	            {
	                portArry[portId] = 1;
	            }
	            break;
	        default:
	            return ERROR;
	            break;
	    
		}
	}
    else 
	{
		retVal = cliConvertPortListAndCheck(pstEnv,portArry,ODM_START_PORT_NUN,ODM_NUM_OF_PORTS+1);
		if(OK != retVal)
		{
			vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
			return ERROR;
		}
    }
	
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortMacFilterNumGet(portId, &macFilterNum);
		   	if(retVal != NO_ERROR)
		   	{
				return retVal;
		   	}
			vosPrintf(pstEnv->nWriteFd, "-------------------------------\n");
			vosPrintf(pstEnv->nWriteFd, "Port %d MAC filter list        \n", portId);
			vosPrintf(pstEnv->nWriteFd, "-------------------------------\n");
			for(i = 0; i < macFilterNum; i++)
			{
				retVal = odmPortMacFilterEntryGet(portId, i, &vlanId, macAddrStr);
				if(retVal != NO_ERROR)
				{
					return retVal;
				}
				vosPrintf(pstEnv->nWriteFd, "[ entry %d ]: vid = %d, mac = %s\n", i, vlanId, macAddrStr);
			}
			vosPrintf(pstEnv->nWriteFd, "-------------------------------\n");
			vosPrintf(pstEnv->nWriteFd, "Total %d entries\n", macFilterNum);
			vosPrintf(pstEnv->nWriteFd, "-------------------------------\n");
		}
	}
	return OK;
}


STATUS cliCmdPortStaticMacAdd(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;
	UINT8  macAddrStr[32];
	UINT16 vlanId = 4095; /* modify by tony  add 4095*/
	if (NULL == pstEnv || NULL == pstPara)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
	if(NULL == pstEnv->para)
	{
		return ERROR;
	}
	retVal = cliConvertPortListAndCheck(pstEnv,portArry,ODM_START_PORT_NUN,ODM_NUM_OF_PORTS+1);
	if(OK != retVal)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
		return ERROR;
	}
	vosStrCpy(macAddrStr, pstPara[0].p);
	/*modify by tony   add the macro 2010-05-18*/
	#if defined(ONU_1PORT) || defined(ONU_4PORT_AR8327)
	if(pstPara[1].p!=NULL  && 0 < pstPara[1].i )
	{
    		vlanId = pstPara[2].u;
	}
	#endif
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
		   	retVal = odmPortStaticMacEntryAdd(portId, vlanId, macAddrStr);
		   	if(retVal != NO_ERROR)
		   	{
		   		return retVal;	
		   	}
		}
	}
	return OK;
}
STATUS cliCmdPortStaticMacDel(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;
	UINT8  macAddrStr[32];
	UINT16 vlanId = 4095;
	if (NULL == pstEnv || NULL == pstPara)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
	if(NULL == pstEnv->para)
	{
		return ERROR;
	}
	retVal = cliConvertPortListAndCheck(pstEnv,portArry,ODM_START_PORT_NUN,ODM_NUM_OF_PORTS+1);
	if(OK != retVal)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
		return ERROR;
	}
	vosStrCpy(macAddrStr, pstPara[0].p);
	
	/*modify by tony   add the macro 2010-05-18*/
	
	#if defined(ONU_1PORT)
	if(pstPara[1].p!=NULL  && 0 < pstPara[1].i )
	{
    		vlanId = pstPara[2].u;
	}
	#endif
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
		   	retVal = odmPortStaticMacEntryDel(portId, vlanId, macAddrStr);
			if(retVal != NO_ERROR)
		   	{
		   		return retVal;
		   	}
		}
	}
	return OK;
}
STATUS cliCmdPortStaticMacClear(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;
	UINT8  macAddrStr[32];
	UINT16 vlanId=4095; /* modify by tony  add 4095*/
	UINT8  macFilterNum;
	UINT8  i;
	if (NULL == pstEnv)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
	retVal = cliConvertPortListAndCheck(pstEnv,portArry,ODM_START_PORT_NUN,ODM_NUM_OF_PORTS+1);
	if(OK != retVal)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
		return ERROR;
	}
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
		   	retVal = odmPortStaticMacEntryClear(portId);
			if(retVal != NO_ERROR)
				return retVal;
		}
	}
	return OK;
}
STATUS cliCmdPortStaticMacShow(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;
	UINT8  macAddrStr[32];
	UINT16 vlanId=4095;
	UINT8  entryNum;
	UINT8  i;
	UINT8  j;
	if (NULL == pstEnv)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
	retVal = cliConvertPortListAndCheck(pstEnv,portArry,ODM_START_PORT_NUN,ODM_NUM_OF_PORTS+1);
	if(OK != retVal)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
		return ERROR;
	}
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortStaticMacNumGet(portId, &entryNum);
		   	if(retVal != NO_ERROR)
		   	{
				return retVal;
		   	}
			vosPrintf(pstEnv->nWriteFd, "-------------------------------\n");
			vosPrintf(pstEnv->nWriteFd, "Port %d static MAC list        \n", portId);
			vosPrintf(pstEnv->nWriteFd, "-------------------------------\n");
			for(i = 0; i < entryNum; i++)
			{
				retVal = odmPortStaticMacEntryGet(portId, i, &vlanId, macAddrStr);
				if(retVal != NO_ERROR)
				{
					return retVal;
				}
				vosPrintf(pstEnv->nWriteFd, "[ entry %d ]: vid = %d, mac = %s\n", i, vlanId, macAddrStr);
			}
			vosPrintf(pstEnv->nWriteFd, "-------------------------------\n");
			vosPrintf(pstEnv->nWriteFd, "Total %d entries\n", entryNum);
			vosPrintf(pstEnv->nWriteFd, "-------------------------------\n");
		}
	}
	return OK;
}


#if defined(ONU_4PORT_AR8228) || defined(ONU_1PORT) || defined(ONU_4PORT_AR8327)

STATUS cliCmdPortBindMacAdd(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;
	UINT8  macAddrStr[32];
	UINT16 vlanId = 4095;
	if (NULL == pstEnv || NULL == pstPara)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
	if(NULL == pstEnv->para)
	{
		return ERROR;
	}
	retVal = cliConvertPortListAndCheck(pstEnv,portArry,ODM_START_PORT_NUN,ODM_NUM_OF_PORTS+1);
	if(OK != retVal)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
		return ERROR;
	}
	vosStrCpy(macAddrStr, pstPara[0].p);
	/*-----cann't configure multicast mac -------*/
	if(((vosMacCharToDigit(macAddrStr[1]))%2) == 1)
	{
		vosPrintf(pstEnv->nWriteFd, "\nerror:cann't configure multicast mac\n");
		return 	ERROR;
	}
#if defined(ONU_1PORT) || defined(ONU_4PORT_AR8327)
    vlanId = pstPara[1].u;
#endif
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
		   	retVal = odmPortBindMacEntryAdd(portId, vlanId, macAddrStr);
		   	if(retVal != NO_ERROR)
		   	{
                if(retVal == ERR_DUPLICATE_ITEM)
                        vosPrintf(pstEnv->nWriteFd, "\nerror:this mac exist in filter\n");
		   		return retVal;	
		   	}
		}
	}
	return OK;
}
STATUS cliCmdPortBindMacDel(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;
	UINT8  macAddrStr[32];
	UINT16 vlanId = 4095;
	if (NULL == pstEnv || NULL == pstPara)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
	if(NULL == pstEnv->para)
	{
		return ERROR;
	}
	retVal = cliConvertPortListAndCheck(pstEnv,portArry,ODM_START_PORT_NUN,ODM_NUM_OF_PORTS+1);
	if(OK != retVal)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
		return ERROR;
	}
	vosStrCpy(macAddrStr, pstPara[0].p);
#if defined(ONU_1PORT)|| defined(ONU_4PORT_AR8327)
    vlanId = pstPara[1].u;
#endif
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
		   	retVal = odmPortBindMacEntryDel(portId, vlanId, macAddrStr);
			if(retVal != NO_ERROR)
		   	{
		   		return retVal;
		   	}
		}
	}
	return OK;
}
STATUS cliCmdPortBindMacClear(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;
	UINT8  macAddrStr[32];
	UINT16 vlanId=4095;   /* modify by tony  add 4095*/
	UINT8  macFilterNum;
	UINT8  i;
	if (NULL == pstEnv)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
	retVal = cliConvertPortListAndCheck(pstEnv,portArry,ODM_START_PORT_NUN,ODM_NUM_OF_PORTS+1);
	if(OK != retVal)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
		return ERROR;
	}
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
		   	retVal = odmPortBindMacEntryClear(portId);
			if(retVal != NO_ERROR)
				return retVal;
		}
	}
	return OK;
}
STATUS cliCmdPortBindMacShow(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT8 portListBuff[255];
	UINT32 portId;
	UINT8  macAddrStr[32];
	UINT16 vlanId=4095;   /* modify by tony  add 4095*/
	UINT8  entryNum;
	UINT8  i;
	UINT8  j;
	if (NULL == pstEnv)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));

	if (pstPara)
	{
		switch(pstPara[0].i)
	    {
	        case 1:
	            vosStrCpy(portListBuff,pstPara[2].p);
	            retVal = convertPortListString2PortArry(portListBuff, portArry, ODM_NUM_OF_PORTS+1);
	            if(-1 == retVal)
	            {
	                vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",portListBuff);
	    	        return ERROR;
	        	}
	        	if(portArry[0] == 1)
	        	{
	        		vosPrintf(pstEnv->nWriteFd, "\r\nport num shuld start at 1.\r\n");
	        		return ERROR;
	        	}
	            break;
	        case 2:
	            for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
	            {
	                portArry[portId] = 1;
	            }
	            break;
	        default:
	            return ERROR;
	            break;
	    
		}
	}
    else 
	{
		retVal = cliConvertPortListAndCheck(pstEnv,portArry,ODM_START_PORT_NUN,ODM_NUM_OF_PORTS+1);
		if(OK != retVal)
		{
			vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
			return ERROR;
		}
    }
	
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortBindMacNumGet(portId, &entryNum);
		   	if(retVal != NO_ERROR)
		   	{
				return retVal;
		   	}
			vosPrintf(pstEnv->nWriteFd, "-------------------------------\n");
			vosPrintf(pstEnv->nWriteFd, "Port %d bind MAC list        \n", portId);
			vosPrintf(pstEnv->nWriteFd, "-------------------------------\n");
			for(i = 0; i < entryNum; i++)
			{
				retVal = odmPortBindMacEntryGet(portId, i, &vlanId, macAddrStr);
				if(retVal != NO_ERROR)
				{
					return retVal;
				}
				vosPrintf(pstEnv->nWriteFd, "[ entry %d ]: vid = %d, mac = %s\n", i, vlanId, macAddrStr);
			}
			vosPrintf(pstEnv->nWriteFd, "-------------------------------\n");
			vosPrintf(pstEnv->nWriteFd, "Total %d entries\n", entryNum);
			vosPrintf(pstEnv->nWriteFd, "-------------------------------\n");
		}
	}
	return OK;
}

#endif


