/*
=============================================================================
     File Name: odm_fdb.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 jiangmingli	2008/08/28		Initial Version
----------------   ------------  ----------------------------------------------
*/

#include "defs.h"
#include "hal.h"
#include "hal_fdb.h"
#include "odm_port.h"
#include "odm_vlan.h"
#include "odm_fdb.h"
#include "log.h"
#include "opl_debug.h"

#define MODULE MOD_FDB
extern OPL_STATUS odmVlanAggLearnAction(UINT16 egressPort, UINT16 aggedVlan, UINT8 *aggedMac);

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
* 		FDB_MAC_CFG_FILE_AGINGTIME_SET_ERROR		set config file error
* 	SEE ALSO:
*/
VOID odmFdbMacVlan2Sec(IN UINT8 *aucMacAddress, IN UINT16 usVlanId, OUT UINT8 *szMacVlanSec)
{
	(VOID)vosSprintf(szMacVlanSec,
						 "%x:%x:%x:%x:%x:%x_%u",
						 aucMacAddress[0],
						 aucMacAddress[1],
						 aucMacAddress[2],
						 aucMacAddress[3],
						 aucMacAddress[4],
						 aucMacAddress[5],
						 usVlanId);

	return;
}

/*******************************************************************************
* odmFdbAgingTimeSet
*
* DESCRIPTION:
*  		set the mac entry aging time
*
*	INPUTS:
*		uiAgingTime		aging time
*	OUTPUTS:
*
* 	RETURNS:
* 		NO_ERROR									success
*		FDB_MAC_AGINGTIME_SET_ERROR				set aging time error
* 		FDB_MAC_CFG_FILE_AGINGTIME_SET_ERROR		set config file error
* 	SEE ALSO:
*/
STATUS odmFdbAgingTimeSet(IN UINT32 uiAgingTime)
{
	UINT32 uiOldAgingTime;
	OPL_STATUS iRet;
	INT32 iExist;
    UINT32 ulAgingTime;
    OPL_API_DATA_t stData;

	iExist = vosConfigKeyIsExisted(FDB_FILENAME_CFG,
											   FDB_SECTION_AGINGTIME,
											   FDB_KEY_VALUE_AGINGTIME);

	uiOldAgingTime = vosConfigUInt32Get(FDB_FILENAME_CFG,
												   FDB_SECTION_AGINGTIME,
												   FDB_KEY_VALUE_AGINGTIME,
												   DAL_MAC_DEFAULT_AGING_TIME_IN_SEC);

	/* same aging time has been set in fdb configure file, nothing should be done */
	if ((0 == iExist) && (uiOldAgingTime == uiAgingTime))
	{
		return NO_ERROR;
	}

	/* set aging time to hw */
    #if 0
	iRet = dalArlAgeTimeSet(uiAgingTime);
    #endif
    ulAgingTime = uiAgingTime;
    stData.apiId = HAL_API_FDB_AGETIME_SET;
    stData.param = &ulAgingTime;
    stData.length = sizeof(ulAgingTime);
    iRet = halAppApiCallSync(&stData);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_AGINGTIME_SET_ERROR;
	}

	/* set aging time into fdb configure file */
	iRet = vosConfigUInt32Set(FDB_FILENAME_CFG,
									     FDB_SECTION_AGINGTIME,
									     FDB_KEY_VALUE_AGINGTIME,
									     uiAgingTime);
	if (0 != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_CFG_FILE_AGINGTIME_SET_ERROR;
	}

	return NO_ERROR;
}


/*******************************************************************************
* odmFdbAgingTimeGet
*
* DESCRIPTION:
*		get the mac entry aging time
*
*	INPUTS:
*
*	OUTPUTS:
*		puiAgingTime		the aging time got from hw
* 	RETURNS:
* 		NO_ERROR								success
* 		FDB_MAC_AGINGTIME_GET_ERROR			get aging time error
* 	SEE ALSO:
*/
STATUS odmFdbAgingTimeGet(OUT UINT32 *puiAgingTime)
{
	OPL_STATUS iRet;
	INT32 iExist;

	/* if aging time in fdb configure file not exists, get AgingTime from hw, else AgingTime from configure file */
	iExist = vosConfigKeyIsExisted(FDB_FILENAME_CFG,
											   FDB_SECTION_AGINGTIME,
											   FDB_KEY_VALUE_AGINGTIME);
	if (0 != iExist)
	{
		iRet = dalArlAgeTimeGet(puiAgingTime);
		if (OPL_OK != iRet)
		{
			OPL_LOG_TRACE();
			return FDB_MAC_AGINGTIME_GET_ERROR;
		}
	}
	else
	{
		*puiAgingTime = vosConfigUInt32Get(FDB_FILENAME_CFG,
													  FDB_SECTION_AGINGTIME,
													  FDB_KEY_VALUE_AGINGTIME,
													  DAL_MAC_DEFAULT_AGING_TIME_IN_SEC);
	}

	return NO_ERROR;
}

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
STATUS odmFdbSetDefault(VOID)
{
    UINT32 ulAgingTime;
    OPL_API_DATA_t stData;
	OPL_STATUS iRet;

	/* set default aging time to hw */
    #if 0
	iRet = dalArlAgeTimeSet(DAL_MAC_DEFAULT_AGING_TIME_IN_SEC);
    #endif
    ulAgingTime = DAL_MAC_DEFAULT_AGING_TIME_IN_SEC;
    stData.apiId = HAL_API_FDB_AGETIME_SET;
    stData.param = &ulAgingTime;
    stData.length = sizeof(ulAgingTime);
    iRet = halAppApiCallSync(&stData);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_AGINGTIME_SET_ERROR;
	}

	/* set default aging time into fdb configure file */
	iRet = vosConfigUInt32Set(FDB_FILENAME_CFG,
									     FDB_SECTION_AGINGTIME,
									     FDB_KEY_VALUE_AGINGTIME,
									     DAL_MAC_DEFAULT_AGING_TIME_IN_SEC);
	if (0 != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_CFG_FILE_AGINGTIME_SET_ERROR;
	}

	return NO_ERROR;
}

/*******************************************************************************
* odmFdbInit
*
* DESCRIPTION:
*		fdb module initial
*
*	INPUTS:
*
*	OUTPUTS:
*
* 	RETURNS:
* 		NO_ERROR									success
*		FDB_MAC_CFG_FILE_SECTION_READ_ERROR		read config file error
* 		FDB_MAC_AGINGTIME_SET_ERROR				set config file error
* 	SEE ALSO:
*/
STATUS odmFdbInit(VOID)
{
	INT32 iSecCnt;
	INT32 iKeyCnt;
	UINT16 i,j,k;
	char *szSecName;
	char *szKeyName;
	char *szKeyValue;
	UINT32 uiAgingTime;
	UINT8 szMacAddr[20];
	UINT8 aucMacAddress[MAC_LENGTH];
	CLS_CONFIG_INFO_t stClassCfgInfo;
	UINT16 usAclRuleId = 0;
	UINT8 *pucMacAddr = NULL;
	UINT32 uiPortBitmap;
	UINT32 uiVlanId;
	UINT32 uiPortNum;
	UINT32 auiPortlist[ODM_NUM_OF_PORTS];
	OPL_BOOL bSetAgingTime = OPL_FALSE;
    HAL_API_FDB_MAC_LEARN_S stMacLearn;
    UINT8 ucEnable;
    HAL_API_FDB_MAC_MULTIPORT_S stMacMultiPort;
    OPL_API_DATA_t stData;
	OPL_STATUS iRet;

    UINT32 retVal;
    UINT32 portId = 1;
	UINT8  macAddrStr[32];
	UINT16 vlanId;
	UINT8  entryNum;
	HAL_PORT_MAC_FILTER_ENTRY_t halMacFilterEntry;
	HAL_PORT_STATIC_MAC_ENTRY_t halStaticMacEntry;
	HAL_PORT_BIND_MAC_ENTRY_t halBindMacEntry;


    /* begin added by jiangmingli for N:1 aggregation */
    /* register arl soft learnning callback function  */
    #ifdef ONU_1PORT
    //dalArlSoftLearnFuncRegister(odmVlanAggLearnAction);
    #endif
    /* end added by jiangmingli for N:1 aggregation */

#if 0 /* it has been initialized in odmPortCfgInit*/
	/* enable mac learning */
    #if 0
	iRet = dalArlLearnEnSet(ENABLED);
    #endif
    stMacLearn.ulPortId = INVALID_32;
    stMacLearn.ulLearnEnable = ENABLED;
    stData.apiId = HAL_API_FDB_LEARN_EN_SET;
    stData.param = &stMacLearn;
    stData.length = sizeof(stMacLearn);
    iRet = halAppApiCallSync(&stData);
	if (0 != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_MAC_LEARN_SET_ERROR;
	}
#endif

	/* enable mac aging */
    #if 0
	iRet = dalArlAgeEnableSet(ENABLED);
    #endif
    ucEnable = ENABLED;
    stData.apiId = HAL_API_FDB_AGE_EN_SET;
    stData.param = &ucEnable;
    stData.length = sizeof(ucEnable);
    iRet = halAppApiCallSync(&stData);
	if (0 != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_MAC_AGING_SET_ERROR;
	}

	/* if fdb configure file not exists, nothing should be done */
	iRet = vosConfigModuleIsExisted(FDB_FILENAME_CFG);
	if (0 != iRet)
	{
		odmFdbAgingTimeSet(DAL_MAC_DEFAULT_AGING_TIME_IN_SEC);
		return NO_ERROR;
	}

	/* get section number */
	iSecCnt = vosConfigSectionCount(FDB_FILENAME_CFG);

	/* add mac entry, set aging time, set discarded mac entry */
	for (i = 0; i < iSecCnt; i++)
	{
		iRet = vosConfigSectionGetByIndex(FDB_FILENAME_CFG, i, &szSecName);
		if (OPL_OK != iRet)
		{
			OPL_LOG_TRACE();
			return FDB_MAC_CFG_FILE_SECTION_READ_ERROR;
		}

		/* the section is aging time */
		if (0 == OPL_MEMCMP(FDB_SECTION_AGINGTIME, szSecName, OPL_STRLEN(FDB_SECTION_AGINGTIME)))
		{
			bSetAgingTime = OPL_TRUE;

			/* get aging time in fdb configure file */
			uiAgingTime = vosConfigUInt32Get(FDB_FILENAME_CFG,
													    FDB_SECTION_AGINGTIME,
													    FDB_KEY_VALUE_AGINGTIME,
													    DAL_MAC_DEFAULT_AGING_TIME_IN_SEC);

			/* set aging time to hw */
            #if 0
			iRet = dalArlAgeTimeSet(uiAgingTime);
            #endif
            stData.apiId = HAL_API_FDB_AGETIME_SET;
            stData.param = &uiAgingTime;
            stData.length = sizeof(uiAgingTime);
            iRet = halAppApiCallSync(&stData);
			if (OPL_OK != iRet)
			{
				OPL_LOG_TRACE();
				return FDB_MAC_AGINGTIME_SET_ERROR;
			}
		}
		/* the section is mac discard */
		else if (0 == OPL_MEMCMP(FDB_SECTION_MAC_DISCARD, szSecName, OPL_STRLEN(FDB_SECTION_MAC_DISCARD)))
		{
			/* get mac discard count in section of mac discard */
			iKeyCnt = vosConfigKeyCount(FDB_FILENAME_CFG, FDB_SECTION_MAC_DISCARD);
			for (j = 0; j < iKeyCnt; j++)
			{

				/* get the discarded mac address, acl ruleid is omitted */
				iRet = vosConfigKeyGetByIndex(FDB_FILENAME_CFG,
												      FDB_SECTION_MAC_DISCARD,
												      j,
												      &szKeyName,
												      &szKeyValue);
				if (0 != iRet)
				{
					OPL_LOG_TRACE();
					return FDB_MAC_CFG_FILE_KEY_READ_ERROR;
				}


				/* get mac address */
				iRet = vosStrToMac(szKeyName, aucMacAddress);
				if (NO_ERROR != iRet)
				{
					OPL_LOG_TRACE();
					return ERR_INVALID_MAC;
				}

				/* set rule */
				OPL_MEMSET(&stClassCfgInfo, 0, sizeof(CLS_CONFIG_INFO_t));
				stClassCfgInfo.srcMacFlag = 1;
				OPL_MEMCPY(stClassCfgInfo.srcMac.lowRange, aucMacAddress, MAC_LENGTH);
				OPL_MEMSET(stClassCfgInfo.srcMac.highRange, 0xFF, MAC_LENGTH);

				/* set action */
				stClassCfgInfo.t_act = DONOT_COPY_TO_CPU | DROP_PKTS;

				/* write acl */
#if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
				iRet = dalArlDiscardMacAdd(aucMacAddress);
#else
				//iRet = dalClsRuleCtcAdd(&stClassCfgInfo, &usAclRuleId);
				iRet = dalVoipClsRuleCtcAdd(&stClassCfgInfo, &usAclRuleId);
#endif
				if (NO_ERROR != iRet)
				{
					OPL_LOG_TRACE();
					return FDB_MAC_DISCARD_ERROR;
				}

				/* reset the acl ruleid */
				iRet = vosConfigUInt32Set(FDB_FILENAME_CFG,
												     FDB_SECTION_MAC_DISCARD,
												     szKeyName,
												     (UINT32)usAclRuleId);
				if (OPL_OK != iRet)
				{
					OPL_LOG_TRACE();
					return FDB_MAC_CFG_FILE_MAC_DISCARD_ADD_ERROR;
				}
			}
		}
		/* the section is mac entry */
		else
		{
			/* get mac address */
			pucMacAddr = vosConfigValueGet(FDB_FILENAME_CFG,
											          szSecName,
											          FDB_KEY_VALUE_MAC_ADDR,
											          "00:00:00:00:00:00");
			/* get port bitmap */
			uiPortBitmap = vosConfigUInt32Get(FDB_FILENAME_CFG,
											             szSecName,
											             FDB_KEY_U32_PORT_BITMAP,
											             0);
			/* get vlanid */
			uiVlanId = vosConfigUInt32Get(FDB_FILENAME_CFG,
											         szSecName,
											         FDB_KEY_U32_VLANID,
											         0);

			/* get mac address */

			iRet = vosStrToMac(pucMacAddr, aucMacAddress);
			if (NO_ERROR != iRet)
			{
				OPL_LOG_TRACE();
				return ERR_INVALID_MAC;
			}

			/* get port number and port list */
			uiPortNum = 0;
			for (k = 0; k < DAL_MAC_MAX_LPORT_NUM;  k++)
			{
				if (0 != (uiPortBitmap & (1 << k)))
				{
					auiPortlist[uiPortNum] = k;
					uiPortNum++;
				}
			}

			/* set mac entry to hw */
            #if 0
			iRet = dalArlMultiPortMacAdd(uiPortNum, auiPortlist, aucMacAddress, (UINT16)uiVlanId);
            #endif
            stMacMultiPort.ulPortNum = uiPortNum;
            stMacMultiPort.aulPortlist = auiPortlist;
            stMacMultiPort.aucMacAddress = aucMacAddress;
            stMacMultiPort.usVlanId = (UINT16)uiVlanId;
            stData.apiId = HAL_API_FDB_MAC_MULTIPORT_ADD;
            stData.param = &stMacMultiPort;
            stData.length = sizeof(stMacMultiPort);
            iRet = halAppApiCallSync(&stData);
			if (OPL_OK != iRet)
			{
				OPL_LOG_TRACE();
				return FDB_MAC_PORT_ADD_ERROR;
			}
		}

	}

	/* set default aging time */
	if (OPL_FALSE == bSetAgingTime)
	{
		odmFdbAgingTimeSet(DAL_MAC_DEFAULT_AGING_TIME_IN_SEC);
	}

#if 0 //defined (ONU_1PORT)
	stData.apiId = HAL_API_PORT_MAC_FILTER_ENTRY_ADD;
	stData.length = sizeof(HAL_PORT_MAC_FILTER_ENTRY_t);
	halMacFilterEntry.vlanId = vlanId;
	halMacFilterEntry.portId = portId;
	vosStrToMac(macAddrStr, &halMacFilterEntry.mac[0]);
	stData.param = (void *)&halMacFilterEntry;
	halAppApiCallSync(&stData);
#elif defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    /* init port base filter mac */
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		retVal = odmPortMacFilterNumGet(portId, &entryNum);
	   	if(retVal != NO_ERROR)
	   	{
			continue;
	   	}

		for(i = 0; i < entryNum; i++)
		{
			retVal = odmPortMacFilterEntryGet(portId, i, &vlanId, macAddrStr);
			if(retVal != NO_ERROR)
			{
			    continue;
			}

            stData.apiId = HAL_API_PORT_MAC_FILTER_ENTRY_ADD;
            stData.length = sizeof(HAL_PORT_MAC_FILTER_ENTRY_t);
            halMacFilterEntry.vlanId = vlanId;
            halMacFilterEntry.portId = portId;
			/*@ tanglin add it 10.05.21,judge the function return*/
			if(OK!=vosStrToMac(macAddrStr, &halMacFilterEntry.mac[0]))
			{
				return ERROR;
			}
        	//vosStrToMac(macAddrStr, &halMacFilterEntry.mac[0]);
        	/*---------------------------------------------------*/

            stData.param = (void *)&halMacFilterEntry;
        	halAppApiCallSync(&stData);
		}
	}

    /* init port base static mac */
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		retVal = odmPortStaticMacNumGet(portId, &entryNum);
	   	if(retVal != NO_ERROR)
	   	{
			continue;
	   	}

		for(i = 0; i < entryNum; i++)
		{
			retVal = odmPortStaticMacEntryGet(portId, i, &vlanId, macAddrStr);
			if(retVal != NO_ERROR)
			{
			    continue;
			}

            stData.apiId = HAL_API_PORT_STATIC_MAC_ENTRY_ADD;
            stData.length = sizeof(HAL_PORT_STATIC_MAC_ENTRY_t);
            halStaticMacEntry.vlanId = vlanId;
            halStaticMacEntry.portId = portId;
		/*@ tanglin add it 10.05.21,judge the function return*/
		if(OK!=vosStrToMac(macAddrStr, &halStaticMacEntry.mac[0]))
			{
			return ERROR;
		}
        	//vosStrToMac(macAddrStr, &halStaticMacEntry.mac[0]);
		/*---------------------------------------------------*/
            stData.param = (void *)&halStaticMacEntry;
        	halAppApiCallSync(&stData);
		}
	}
#endif

    /* init port base bind mac */
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		retVal = odmPortBindMacNumGet(portId, &entryNum);
	   	if(retVal != NO_ERROR)
	   	{
			continue;
	   	}

		for(i = 0; i < entryNum; i++)
		{
			retVal = odmPortBindMacEntryGet(portId, i, &vlanId, macAddrStr);
			if(retVal != NO_ERROR)
			{
			    continue;
			}

            stData.apiId = HAL_API_PORT_BIND_MAC_ENTRY_ADD;
            stData.length = sizeof(HAL_PORT_BIND_MAC_ENTRY_t);
            halBindMacEntry.vlanId = vlanId;
            halBindMacEntry.portId = portId;
		/*@ tanglin add it 10.05.21,judge the function return*/
		if(OK!=vosStrToMac(macAddrStr, &halBindMacEntry.mac[0]))
		{
			return ERROR;
		}
        	//vosStrToMac(macAddrStr, &halBindMacEntry.mac[0]);
		/*---------------------------------------------------*/

            stData.param = (void *)&halBindMacEntry;
        	retVal = halAppApiCallSync(&stData);
			if(retVal != NO_ERROR)
			{
				printf("error: HAL_API_PORT_BIND_MAC_ENTRY_ADD\n");
			}
		}
	}

	return NO_ERROR;
}

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
*		FDB_MAC_CFG_FILE_MAC_ADD_ERROR	set config file error
* 	SEE ALSO:
*/
STATUS odmFdbMacAdd(IN UINT16 usPortId, IN UINT8 *aucMacAddress, IN UINT16 usVlanId)
{
	UINT8 szMacEntrySec[30];
	UINT8 szMacAddr[20];
	INT32 iExist;
	UINT32 uiPortBitmap;
	UINT32 uiVlanId;
	UINT32 uiCurPortBitMap;
    HAL_API_FDB_MAC_S stPara;
    OPL_API_DATA_t stData;
	OPL_STATUS iRet;

	OPL_MEMSET(szMacEntrySec, 0, 30);
	odmFdbMacVlan2Sec(aucMacAddress, usVlanId, szMacEntrySec);

	/* transfer the portid to port bitmap */
	uiCurPortBitMap = 0;
	uiCurPortBitMap |= (1 << usPortId);

	iExist = vosConfigSectionIsExisted(FDB_FILENAME_CFG,
												   szMacEntrySec);

	/* the same mac entry in fdb configure file already exists, nothing should be done */
	if (0 == iExist)
	{
		/* get port bitmap */
		uiPortBitmap = vosConfigUInt32Get(FDB_FILENAME_CFG,
										             szMacEntrySec,
										             FDB_KEY_U32_PORT_BITMAP,
										             0);

		/* mac entry already exists */
		if (uiPortBitmap == uiCurPortBitMap)
		{
			return NO_ERROR;
		}
	}

	/* set mac entry to hw */
    #if 0
	iRet = dalArlMacAdd((UINT8)usPortId, aucMacAddress, usVlanId);
    #endif
    stPara.ucPortId = (UINT8)usPortId;
    stPara.aucMacAddress = aucMacAddress;
    stPara.usVlanId = usVlanId;
    stData.apiId = HAL_API_FDB_MAC_ADD;
    stData.param = &stPara;
    stData.length = sizeof(stPara);
    iRet = halAppApiCallSync(&stData);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_ADD_ERROR;
	}

	/* set the mac entry into fdb configure file */
	OPL_MEMSET(szMacAddr, 0, 20);
	cliMacToStr(aucMacAddress, szMacAddr);
	iRet = vosConfigValueSet(FDB_FILENAME_CFG,
							            szMacEntrySec,
							            FDB_KEY_VALUE_MAC_ADDR,
							            szMacAddr);

	iRet += vosConfigUInt32Set(FDB_FILENAME_CFG,
							              szMacEntrySec,
							              FDB_KEY_U32_PORT_BITMAP,
							              uiCurPortBitMap);

	iRet += vosConfigUInt32Set(FDB_FILENAME_CFG,
							              szMacEntrySec,
							              FDB_KEY_U32_VLANID,
							              usVlanId);

	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_CFG_FILE_MAC_ADD_ERROR;
	}

	return NO_ERROR;
}

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
*		FDB_MAC_CFG_FILE_MAC_PORT_ADD_ERROR	set config file error
* 	SEE ALSO:
*/
STATUS odmFdbMacPortAdd
(
	IN UINT32 uiPortNum,
	IN UINT32 *auiPortlist,
	IN UINT8 *aucMacAddress,
	IN UINT16 usVlanId
)
{
	UINT8 szMacEntrySec[30];
	UINT8 szMacAddr[20];
	INT32 iExist;
	UINT32 uiPortBitmap;
	UINT32 uiVlanId;
	UINT32 uiCurPortBitMap;
	UINT16 i;
    HAL_API_FDB_MAC_MULTIPORT_S stPara;
    OPL_API_DATA_t stData;
	OPL_STATUS iRet;

	OPL_STATUS retVal;
	UINT32 entryNum=0;
	UINT32 portId=0;

	/*add by ltang, add for  mac bind procedure --start*/
	
	#if 0
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		retVal=odmPortBindMacNumGet(portId, &entryNum);
		if (0 != retVal ) 
		{
			printf("odmPortBindMacNumGet execute Error !\n");
			return ERROR;
		}
		if ( entryNum > 0 ){
			printf("Please delete mac bind entry first!\n");
			return ERROR;
		}
	}
	#endif



	/*add by ltang, add for  mac bind procedure --start*/

	

	OPL_MEMSET(szMacEntrySec, 0, 30);
	odmFdbMacVlan2Sec(aucMacAddress, usVlanId, szMacEntrySec);

	/* transfer the portid to port bitmap */
	uiPortBitmap = 0;
	uiCurPortBitMap = 0;
	for (i =0; i < uiPortNum; i++)
	{
		if (0 != auiPortlist[i])
		{
			uiCurPortBitMap |= (1 << auiPortlist[i]);
			//printf("port %d.\n", auiPortlist[i]);
			odmPortBindMacNumGet(auiPortlist[i], &entryNum);
			if ( entryNum > 0 ){
				printf("Please delete mac bind entry first!\n");
				return ERROR;
			}
		}
	}

	iExist = vosConfigSectionIsExisted(FDB_FILENAME_CFG,
												   szMacEntrySec);

	/* the same mac entry in fdb configure file already exists, nothing should be done */
	if (0 == iExist)
	{
		/* get port bitmap */
		uiPortBitmap = vosConfigUInt32Get(FDB_FILENAME_CFG,
										             szMacEntrySec,
										             FDB_KEY_U32_PORT_BITMAP,
										             0);
		/* mac entry already exists */
		if (uiPortBitmap == (uiPortBitmap | uiCurPortBitMap))
		{
			return NO_ERROR;
		}
	}

	/* set mac entry to hw */
    #if 0
	iRet = dalArlMultiPortMacAdd(uiPortNum, auiPortlist, aucMacAddress, usVlanId);
    #endif
    stPara.ulPortNum = uiPortNum;
    stPara.aulPortlist = auiPortlist;
    stPara.aucMacAddress = aucMacAddress;
    stPara.usVlanId = usVlanId;
    stData.apiId = HAL_API_FDB_MAC_MULTIPORT_ADD;
    stData.param = &stPara;
    stData.length = sizeof(stPara);
    iRet = halAppApiCallSync(&stData);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_PORT_ADD_ERROR;
	}

	/* calculate the new portlist */
	uiCurPortBitMap = uiCurPortBitMap | uiPortBitmap;

	/* set the mac entry into fdb configure file */
	OPL_MEMSET(szMacAddr, 0, 20);
	cliMacToStr(aucMacAddress, szMacAddr);
	iRet = vosConfigValueSet(FDB_FILENAME_CFG,
							            szMacEntrySec,
							            FDB_KEY_VALUE_MAC_ADDR,
							            szMacAddr);

	iRet += vosConfigUInt32Set(FDB_FILENAME_CFG,
							              szMacEntrySec,
							              FDB_KEY_U32_PORT_BITMAP,
							              uiCurPortBitMap);

	iRet += vosConfigUInt32Set(FDB_FILENAME_CFG,
							              szMacEntrySec,
							              FDB_KEY_U32_VLANID,
							              usVlanId);

	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_CFG_FILE_MAC_PORT_ADD_ERROR;
	}

	return NO_ERROR;
}

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
* 		FDB_MAC_DEL_ERROR			del mac entry error
*		FDB_MAC_CFG_FILE_MAC_DEL_ERROR	set config file error

* 	SEE ALSO:
*/
STATUS odmFdbMacDel(IN UINT8 *aucMacAddress, IN UINT16 usVlanId)
{
	UINT8 szMacEntrySec[30];
	INT32 iExist;
	UINT32 uiVlanId;
    HAL_API_FDB_MAC_S stPara;
    OPL_API_DATA_t stData;
	OPL_STATUS iRet;

	OPL_MEMSET(szMacEntrySec, 0, 30);
	odmFdbMacVlan2Sec(aucMacAddress, usVlanId, szMacEntrySec);

	/* delete mac entry */
    #if 0
	iRet = dalArlMacRemove(aucMacAddress, usVlanId);
    #endif
    stPara.ucPortId = INVALID_8;
    stPara.aucMacAddress = aucMacAddress;
    stPara.usVlanId = usVlanId;
    stData.apiId = HAL_API_FDB_MAC_REMOVE;
    stData.param = &stPara;
    stData.length = sizeof(stPara);
    iRet = halAppApiCallSync(&stData);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_DEL_ERROR;
	}

	iExist = vosConfigSectionIsExisted(FDB_FILENAME_CFG,
												   szMacEntrySec);

	/* the mac entry in fdb configure file not exists, nothing should be done */
	if (0 != iExist)
	{
		return NO_ERROR;
	}

	/* delete the mac entry  from fdb configure file */
	iRet = vosConfigSectionDelete(FDB_FILENAME_CFG, szMacEntrySec);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_CFG_FILE_MAC_DEL_ERROR;
	}

	return NO_ERROR;
}

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
*		FDB_MAC_CFG_FILE_SECTION_READ_ERROR	get section error
* 	SEE ALSO:
*/
STATUS odmFdbMacClearAll(VOID)
{
	INT32 iSecCnt;
	UINT16 i;
	char *szSecName;
    OPL_API_DATA_t stData;
	OPL_STATUS iRet;
	OPL_STATUS retVal;
	INT32 entryNum=0;
	INT32 portId=1;
#ifndef ONU_1PORT
	UINT8* pOldMacAddr;
	UINT8 macAddr[32];
	UINT32 vid, port;
	UINT32 vlanMode=0;
#endif

	/* delete all mac entry */
    #if 0
	iRet = dalArlFlushAll();
    #endif

	/*add by ltang, add for  mac bind procedure --start*/

#if 1
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		retVal=odmPortBindMacNumGet(portId, &entryNum);
		if (0 != retVal ) 
		{
			printf("odmPortBindMacNumGet execute Error !\n");
			return ERROR;
		}
		if ( entryNum > 0 ){
			printf("Please delete mac bind entry first!\n");
			return ERROR;
		}
	}
#endif

	/*add by ltang, add for  mac bind procedure --start*/
	
    stData.apiId = HAL_API_FDB_ALL_FLUSH;
    stData.param = NULL;
    stData.length = 0;
    iRet = halAppApiCallSync(&stData);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_DEL_ALL_ERROR;
	}

#ifdef ONU_4PORT_AR8228
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++){
		odmPortVlanModeGet(portId, &vlanMode);
		if (ODM_VLAN_AGGREGATION == vlanMode){
			//printf("Port %d vlan mode aggregation, can not clear fdb!\n", portId);
			//return ERROR;
			odmVlanAggMacDelByPort(portId);
		}
	}
#endif

	/* get section number */
	iSecCnt = vosConfigSectionCount(FDB_FILENAME_CFG);

	/* delete all mac entry */
	for (i = iSecCnt - 1; i < iSecCnt; i--)
	{
		iRet = vosConfigSectionGetByIndex(FDB_FILENAME_CFG, i, &szSecName);
		if (OPL_OK != iRet)
		{
			OPL_LOG_TRACE();
			return FDB_MAC_CFG_FILE_SECTION_READ_ERROR;
		}

		/* if the section is Agingtime, continue */
		if (0 == OPL_MEMCMP(FDB_SECTION_AGINGTIME, szSecName, OPL_STRLEN(FDB_SECTION_AGINGTIME)))
		{
			continue;
		}

#ifdef ONU_1PORT
		if (0xFFFFFFFF == vosConfigUInt32Get(FDB_FILENAME_CFG, szSecName, FDB_KEY_U32_PORT_BITMAP, 0xFFFFFFFF) &&
			OPL_NULL == vosStrStr(szSecName, "_static_mac_")){
			continue;
		}
#else		
		if (OPL_NULL != vosStrStr(szSecName, "_filter_"))
		{
			//printf("filter section name %s.\n", szSecName);
			pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG,szSecName,FDB_KEY_MAC_FILTER_ADDR,0);
			if(0 != pOldMacAddr){
				vosStrCpy(macAddr, pOldMacAddr);
				vid = vosConfigUInt32Get(FDB_FILENAME_CFG, szSecName, FDB_KEY_MAC_FILTER_VID, 0);
				port = vosConfigUInt32Get(FDB_FILENAME_CFG, szSecName, FDB_KEY_MAC_FILTER_PORD, 0);
				vosConfigSectionDelete(FDB_FILENAME_CFG, szSecName);
				odmPortMacFilterEntryAdd(port, vid, macAddr);
				continue;
			}
		}
#endif

		/* delete mac entry from fdb configure file */
		(VOID)vosConfigSectionDelete(FDB_FILENAME_CFG, szSecName);
	}



	return NO_ERROR;
}


STATUS odmFdbMacClearAllByPort(UINT32 portId)
{
	INT32 iSecCnt;
	UINT16 i;
	char *szSecName;
    OPL_API_DATA_t stData;
	OPL_STATUS iRet;
	OPL_STATUS retVal;
	INT32 entryNum=0;
#ifndef ONU_1PORT
	UINT8* pOldMacAddr;
	UINT8 macAddr[32];
	char portName[64];
	UINT32 vid, port;
	UINT32 vlanMode=0;
	
	vosSprintf(portName,"port_%d" ,portId);

#endif

	retVal=odmPortBindMacNumGet(portId, &entryNum);
	if (0 != retVal ) 
	{
		printf("odmPortBindMacNumGet execute Error !\n");
		return ERROR;
	}
	if ( entryNum > 0 ){
		printf("Please delete mac bind entry first!\n");
		return ERROR;
	}
	
    stData.apiId = HAL_API_FDB_ALL_FLUSH_BY_PORT;
    stData.param = &portId;
    stData.length = sizeof(portId);
    iRet = halAppApiCallSync(&stData);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_DEL_ALL_ERROR;
	}

#ifdef ONU_4PORT_AR8228
		odmPortVlanModeGet(portId, &vlanMode);
		if (ODM_VLAN_AGGREGATION == vlanMode){
			//printf("Port %d vlan mode aggregation, can not clear fdb!\n", portId);
			//return ERROR;
			odmVlanAggMacDelByPort(portId);
		}
#endif

	/* get section number */
	iSecCnt = vosConfigSectionCount(FDB_FILENAME_CFG);

	/* delete all mac entry */
	for (i = iSecCnt - 1; i < iSecCnt; i--)
	{
		iRet = vosConfigSectionGetByIndex(FDB_FILENAME_CFG, i, &szSecName);
		if (OPL_OK != iRet)
		{
			OPL_LOG_TRACE();
			return FDB_MAC_CFG_FILE_SECTION_READ_ERROR;
		}

		/* if the section is Agingtime, continue */
		if (0 == OPL_MEMCMP(FDB_SECTION_AGINGTIME, szSecName, OPL_STRLEN(FDB_SECTION_AGINGTIME)))
		{
			continue;
		}

#ifdef ONU_1PORT
		if (0xFFFFFFFF == vosConfigUInt32Get(FDB_FILENAME_CFG, szSecName, FDB_KEY_U32_PORT_BITMAP, 0xFFFFFFFF) &&
			OPL_NULL == vosStrStr(szSecName, "_static_mac_")){
			continue;
		}
#else	
		if (0 != strncmp(portName, szSecName, OPL_STRLEN(portName)))
		{
			continue;
		}
		
		if (OPL_NULL != vosStrStr(szSecName, "_filter_"))
		{
			//printf("filter section name %s.\n", szSecName);
			pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG,szSecName,FDB_KEY_MAC_FILTER_ADDR,0);
			if(0 != pOldMacAddr){
				vosStrCpy(macAddr, pOldMacAddr);
				vid = vosConfigUInt32Get(FDB_FILENAME_CFG, szSecName, FDB_KEY_MAC_FILTER_VID, 0);
				port = vosConfigUInt32Get(FDB_FILENAME_CFG, szSecName, FDB_KEY_MAC_FILTER_PORD, 0);
				vosConfigSectionDelete(FDB_FILENAME_CFG, szSecName);
				odmPortMacFilterEntryAdd(port, vid, macAddr);
				continue;
			}
		}

		port = vosConfigUInt32Get(FDB_FILENAME_CFG,szSecName,FDB_KEY_U32_PORT_BITMAP,0);
		if (0 != port)
		{
			//printf("bitmap: %x-%x\n", port, (1<<portId));
			if (port == (1<<portId))
			{
				//printf("1\n");
				vosConfigSectionDelete(FDB_FILENAME_CFG, szSecName);
			}
			else if (port&(1<<portId))
			{
				//printf("2\n");
				vosConfigUInt32Set(FDB_FILENAME_CFG,
						             szSecName,
						             FDB_KEY_U32_PORT_BITMAP,
						             port&(~(1<<portId)));
			}

			continue;
		}
#endif

		/* delete mac entry from fdb configure file */
		(VOID)vosConfigSectionDelete(FDB_FILENAME_CFG, szSecName);
	}

	return NO_ERROR;
}



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
*		FDB_MAC_CFG_FILE_MAC_PORT_DEL_ERROR	set config file error
* 	SEE ALSO:
*/
STATUS odmFdbMacPortDel
(
	IN UINT32 uiPortNum,
	IN UINT32 *auiPortlist,
	IN UINT8 *aucMacAddress,
	IN UINT16 usVlanId
)
{
	UINT8 szMacEntrySec[30];
	INT32 iExist;
	UINT32 uiPortBitmap;
	UINT32 uiVlanId;
	UINT32 uiDelPortBitMap;
	UINT16 i;
    HAL_API_FDB_MAC_MULTIPORT_S stPara;
    OPL_API_DATA_t stData;
	OPL_STATUS iRet;

	OPL_MEMSET(szMacEntrySec, 0, 30);
	odmFdbMacVlan2Sec(aucMacAddress, usVlanId, szMacEntrySec);

	iExist = vosConfigSectionIsExisted(FDB_FILENAME_CFG,
												   szMacEntrySec);

	/* the mac entry in fdb configure file not exists, nothing should be done */
	if (0 != iExist)
	{
		return NO_ERROR;
	}

	/* get port bitmap */
	uiPortBitmap = vosConfigUInt32Get(FDB_FILENAME_CFG,
									             szMacEntrySec,
									             FDB_KEY_U32_PORT_BITMAP,
									             0);
	/* transfer the portid to port bitmap */
	uiDelPortBitMap = 0;
	for (i =0; i < uiPortNum; i++)
	{
		uiDelPortBitMap |= (1 << auiPortlist[i]);
	}

	/* no desired port already exists in fdb configure file */
	if (0 == (uiDelPortBitMap & uiPortBitmap))
	{
		return NO_ERROR;
	}

	/* remove ports from the mac entry in hw */
    #if 0
	iRet = dalArlMultiPortMacDel(uiPortNum, auiPortlist, aucMacAddress, usVlanId);
    #endif
    stPara.ulPortNum = uiPortNum;
    stPara.aulPortlist = auiPortlist;
    stPara.aucMacAddress = aucMacAddress;
    stPara.usVlanId = usVlanId;
    stData.apiId = HAL_API_FDB_MAC_MULTIPORT_DEL;
    stData.param = &stPara;
    stData.length = sizeof(stPara);
    iRet = halAppApiCallSync(&stData);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_PORT_DEL_ERROR;
	}

	/* set port bitmap */
	uiPortBitmap = uiPortBitmap & (~uiDelPortBitMap);
	iRet = vosConfigUInt32Set(FDB_FILENAME_CFG,
							             szMacEntrySec,
							             FDB_KEY_U32_PORT_BITMAP,
							             uiPortBitmap);
	if (0 != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_CFG_FILE_MAC_PORT_DEL_ERROR;
	}

	return NO_ERROR;
}

/*******************************************************************************
* odmFdbMacDiscard
*
* DESCRIPTION:
*		discard the packets, who source mac address is aucMacAddress
*	INPUTS:
*		aucMacAddress	the mac address needed to delete
*	OUTPUTS:
*
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_DISCARD_ERROR		discard mac entry error
*		FDB_MAC_CFG_FILE_MAC_DISCARD_ADD_ERROR	set config file error
* 	SEE ALSO:
*/
STATUS odmFdbMacDiscard(IN UINT8 *aucMacAddress)
{
	UINT8 szAclRuleName[20];
	INT32 iExist;
	CLS_CONFIG_INFO_t stClassCfgInfo;
	UINT16 usAclRuleId = 0;
	OPL_STATUS iRet;

	OPL_MEMSET(szAclRuleName, 0, 20);
	(VOID)cliMacToStr(aucMacAddress, szAclRuleName);

	iExist = vosConfigKeyIsExisted(FDB_FILENAME_CFG,
											   FDB_SECTION_MAC_DISCARD,
											   szAclRuleName);

	/* same key has been set in fdb configure file, nothing should be done */
	if (0 == iExist)
	{
		return NO_ERROR;
	}

	/* set rule */
	OPL_MEMSET(&stClassCfgInfo, 0, sizeof(CLS_CONFIG_INFO_t));
	stClassCfgInfo.srcMacFlag = 1;
	OPL_MEMCPY(stClassCfgInfo.srcMac.lowRange, aucMacAddress, MAC_LENGTH);
	OPL_MEMSET(stClassCfgInfo.srcMac.highRange, 0xFF, MAC_LENGTH);

	/* set action */
	stClassCfgInfo.t_act = DROP_PKTS;

	/* write acl to hw */
#if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	iRet = dalArlDiscardMacAdd(aucMacAddress);
#else
	iRet = dalVoipClsRuleCtcAdd(&stClassCfgInfo, &usAclRuleId);
       if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_DISCARD_ERROR;
	}
       /* set acl ruleid into fdb configure file */
	iRet = vosConfigUInt32Set(FDB_FILENAME_CFG,
									     FDB_SECTION_MAC_DISCARD,
									     szAclRuleName,
									     (UINT32)usAclRuleId);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_CFG_FILE_MAC_DISCARD_ADD_ERROR;
	}

	/* add the discard entry for DMAC */
	/* set rule */
	OPL_MEMSET(&stClassCfgInfo, 0, sizeof(CLS_CONFIG_INFO_t));
	stClassCfgInfo.dstMacFlag = 1;
	OPL_MEMCPY(stClassCfgInfo.dstMac.lowRange, aucMacAddress, MAC_LENGTH);
	OPL_MEMSET(stClassCfgInfo.dstMac.highRange, 0xFF, MAC_LENGTH);

	/* set action */
	stClassCfgInfo.t_act = DROP_PKTS;

       szAclRuleName[17] = 'D';
	szAclRuleName[18] = '\0';
	iRet = dalVoipClsRuleCtcAdd(&stClassCfgInfo, &usAclRuleId);
#endif
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_DISCARD_ERROR;
	}

	/* set acl ruleid into fdb configure file */
	iRet = vosConfigUInt32Set(FDB_FILENAME_CFG,
									     FDB_SECTION_MAC_DISCARD,
									     szAclRuleName,
									     (UINT32)usAclRuleId);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_CFG_FILE_MAC_DISCARD_ADD_ERROR;
	}

	return NO_ERROR;
}

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
*		FDB_MAC_CFG_FILE_KEY_READ_ERROR	get key error
*		FDB_MAC_CFG_FILE_MAC_DISCARD_DEL_ERROR	set config file error
* 	SEE ALSO:
*/
STATUS odmFdbMacDiscardClear(IN UINT8 *aucMacAddress)
{
	UINT8 szAclRuleName[20];
	INT32 iExist;
	CLS_CONFIG_INFO_t stClassCfgInfo;
	UINT32 uiAclRuleId = 0;
	OPL_STATUS iRet;

	OPL_MEMSET(szAclRuleName, 0, 20);
	(VOID)cliMacToStr(aucMacAddress, szAclRuleName);

	iExist = vosConfigKeyIsExisted(FDB_FILENAME_CFG,
											   FDB_SECTION_MAC_DISCARD,
											   szAclRuleName);

	/* the key in fdb configure file not exists, nothing should be done */
	if (0 != iExist)
	{
		return NO_ERROR;
	}

	/* get acl ruleid */
	uiAclRuleId = vosConfigUInt32Get(FDB_FILENAME_CFG,
									     		FDB_SECTION_MAC_DISCARD,
									     		szAclRuleName,
									     		0xffffffff);
	if (0xffffffff == uiAclRuleId)
	{
		return FDB_MAC_CFG_FILE_KEY_READ_ERROR;
	}

	/* delete the acl */
#if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	iRet = dalArlDiscardMacDel(aucMacAddress);
#else
	iRet = dalClsDelEntry(uiAclRuleId);
       if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_DISCARD_CLEAR_ERROR;
	}

	/* delete mac discoard key from fdb configure file */
	iRet = vosConfigKeyDelete(FDB_FILENAME_CFG,
						     		FDB_SECTION_MAC_DISCARD,
						     		szAclRuleName);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_CFG_FILE_MAC_DISCARD_DEL_ERROR;
	}

       /* add the DMAC discard for bug3160 */
	szAclRuleName[17] = 'D';
	szAclRuleName[18] = '\0';
       iExist = vosConfigKeyIsExisted(FDB_FILENAME_CFG,
											   FDB_SECTION_MAC_DISCARD,
											   szAclRuleName);

	/* the key in fdb configure file not exists, nothing should be done */
	if (0 != iExist)
	{
		return NO_ERROR;
	}

	/* get acl ruleid */
	uiAclRuleId = vosConfigUInt32Get(FDB_FILENAME_CFG,
									     		FDB_SECTION_MAC_DISCARD,
									     		szAclRuleName,
									     		0xffffffff);
	if (0xffffffff == uiAclRuleId)
	{
		return FDB_MAC_CFG_FILE_KEY_READ_ERROR;
	}
	iRet = dalClsDelEntry(uiAclRuleId);
#endif
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_DISCARD_CLEAR_ERROR;
	}

	/* delete mac discoard key from fdb configure file */
	iRet = vosConfigKeyDelete(FDB_FILENAME_CFG,
						     		FDB_SECTION_MAC_DISCARD,
						     		szAclRuleName);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_CFG_FILE_MAC_DISCARD_DEL_ERROR;
	}

	return NO_ERROR;
}


/*******************************************************************************
* odmFdbMove
*
* DESCRIPTION:
*	move fdb from one port to another.
*
*	INPUTS:
*		uiPortNum		Port number for Port List
*		auiPortlist		the Port List associated with the mac
*		uiToPortNum		Port number for Port List
*		auiToPortlist		the Port List associated with the mac
*	OUTPUTS:
*
* 	RETURNS:
* 		NO_ERROR					success
* 	SEE ALSO:
*/
STATUS odmFdbMove
(
	IN UINT32 uiPort,
	IN UINT32 uiToPort
)
{
    HAL_API_FDB_MAC_MOVE_S stPara;
    OPL_API_DATA_t stData;
	OPL_STATUS iRet;

    stPara.ulPort = uiPort;
    stPara.ulToPort = uiToPort;
    stData.apiId = HAL_API_FDB_MAC_MOVE;
    stData.param = &stPara;
    stData.length = sizeof(stPara);
    iRet = halAppApiCallSync(&stData);
	if (OPL_OK != iRet)
	{
		OPL_LOG_TRACE();
		return FDB_MAC_MOVE_ERROR;
	}

	return NO_ERROR;
}

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
STATUS odmFdbSwLearnSet(BOOL_T bEnable)
{
	OPL_STATUS iRet;

    #ifdef ONU_1PORT
    iRet = dalArlSoftLearnEnSet(bEnable);
	vosConfigUInt32Set(FDB_FILENAME_CFG, FDB_SECTION_SOFT_LEARNING,
    FDB_KEY_VALUE_SOFT_LEARNING, bEnable);
    if (OPL_OK != iRet)
    {
        OPL_LOG_TRACE();
		return FDB_MAC_SOFT_LEARN_SET_ERROR;
    }
    #endif

    return OPL_OK;
}

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
STATUS odmFdbSwLearnGet(BOOL_T *pbEnable)
{
    OPL_STATUS iRet;

    #ifdef ONU_1PORT
    iRet = dalArlSoftLearnEnGet(pbEnable);
	*pbEnable = vosConfigUInt32Get(FDB_FILENAME_CFG, FDB_SECTION_SOFT_LEARNING,
    FDB_KEY_VALUE_SOFT_LEARNING, 0);
    if (OPL_OK != iRet)
    {
        OPL_LOG_TRACE();
		return FDB_MAC_SOFT_LEARN_GET_ERROR;
    }
    #else
    *pbEnable = OPL_FALSE;
    #endif

    return OPL_OK;
}

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
)
{
    HAL_API_FDB_MAC_SHOW_S stPara;
    OPL_API_DATA_t stData;
	OPL_STATUS iRet;

    #if 0
	iRet = dalArlMacEntryShowOne(lFd,
								 aucMacAddress,
								 usType,
								 uiPortNum,
								 auiPortlist);
    #endif
    stPara.lFd = lFd;
    stPara.aucMacAddress = aucMacAddress;
    stPara.usType = usType;
    stPara.ulPortNum = uiPortNum;
    stPara.aulPortlist = auiPortlist;
    stData.apiId = HAL_API_FDB_ONE_SHOW;
    stData.param = &stPara;
    stData.length = sizeof(stPara);
    iRet = halAppApiCallSync(&stData);
 	if (OPL_OK != iRet)
	{
		return FDB_MAC_SHOW_ONE_ERROR;
	}

	return NO_ERROR;
}

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
)
{
    HAL_API_FDB_MAC_SHOW_S stPara;
    OPL_API_DATA_t stData;
	OPL_STATUS iRet;

    #if 0
	iRet = dalArlMacEntryShowAll(lFd,
								 usType,
								 uiPortNum,
								 auiPortlist);
    #endif
    stPara.lFd = lFd;
    stPara.aucMacAddress = NULL;
    stPara.usType = usType;
    stPara.ulPortNum = uiPortNum;
    stPara.aulPortlist = auiPortlist;
    stData.apiId = HAL_API_FDB_ALL_SHOW;
    stData.param = &stPara;
    stData.length = sizeof(stPara);
    iRet = halAppApiCallSync(&stData);
	if (OPL_OK != iRet)
	{
		return FDB_MAC_SHOW_ALL_ERROR;
	}

	return NO_ERROR;
}

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
STATUS odmFdbCfgShow(IN INT32 lFd)
{
	vosConfigShowByModule(FDB_FILENAME_CFG, lFd);

	return NO_ERROR;
}

UINT32 odmPortMacFilterEntryAdd(UINT32 portId, UINT16 vlanId, UINT8 *pMacAddr)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};
	UINT8  macFilterNum;
	UINT16 oldVlanId;
	UINT8  *pOldMacAddr;
	UINT32 macFilterEntryId;
	UINT32 entryId;
    	UINT32 firstEmptyEntryId = 0xffff;
    	OPL_API_DATA_t stData;
	HAL_PORT_MAC_FILTER_ENTRY_t halMacFilterEntry;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: portId = %d, vid = %d, mac = %s\n", __FUNCTION__, portId, vlanId, pMacAddr);

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	retVal = odmPortMacFilterNumGet(portId, &macFilterNum);
	if(retVal != OK)
	{
		return retVal;
	}

	if(macFilterNum == ODM_MAX_NUM_OF_MAC_FILTER)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: No enough mac filter resource!\n", __FUNCTION__);
		return -1;
	}
	/* decide mac whether exist in bind*/
	for(entryId = 0; entryId < ODM_MAX_NUM_OF_BIND_MAC; entryId++)
	{
		vosSprintf(sectionBuff, FDB_SEC_BIND_MAC_ENTRY, portId, entryId);

	    retVal = vosConfigSectionIsExisted(FDB_FILENAME_CFG, sectionBuff);
		if(OK != retVal)
		{
    		continue;
		}
		pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_BIND_MAC_ADDR, 0);

		if((vosStrCmp(pOldMacAddr, pMacAddr)== 0))
		{
			/* entry already exist */
		    return ERR_DUPLICATE_ITEM;
		}
	}

	for(macFilterEntryId = 0; macFilterEntryId < ODM_MAX_NUM_OF_MAC_FILTER; macFilterEntryId++)
	{
		vosSprintf(sectionBuff, FDB_SEC_MAC_FILTER_ENTRY, portId, macFilterEntryId);

	    retVal = vosConfigSectionIsExisted(FDB_FILENAME_CFG, sectionBuff);
		if(OK != retVal)
		{
    		if(0xffff == firstEmptyEntryId)
    		{
        		firstEmptyEntryId = macFilterEntryId;
    		}
    		continue;
		}

		pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_MAC_FILTER_ADDR, 0);

		oldVlanId = vosConfigUInt32Get(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_MAC_FILTER_VID, 0);

		if((vosStrCmp(pOldMacAddr, pMacAddr)== 0) && (oldVlanId == vlanId))
		{
			/* entry already exist */
		    return NO_ERROR;
		}
    }

    if(0xffff == firstEmptyEntryId)
    {
        return -1;
    }
	
	/* call hal */
    stData.apiId = HAL_API_PORT_MAC_FILTER_ENTRY_ADD;
    stData.length = sizeof(HAL_PORT_MAC_FILTER_ENTRY_t);
    halMacFilterEntry.vlanId = vlanId;
    halMacFilterEntry.portId = portId;
	/*@ tanglin add it 10.05.21,judge the function return*/
	if(OK!=vosStrToMac(pMacAddr, &halMacFilterEntry.mac[0]))
		{
		return ERROR;
	}
	//vosStrToMac(pMacAddr, &halMacFilterEntry.mac[0]);
	/*---------------------------------------------------*/
    stData.param = (void *)&halMacFilterEntry;

	retVal = halAppApiCallSync(&stData);

	if(retVal != NO_ERROR)
	{
		return retVal;
	}

    if(NO_ERROR != halMacFilterEntry.retVal)
    {
        return halMacFilterEntry.retVal;
    }

	/* add mac filter entry to config file */
	vosSprintf(sectionBuff, FDB_SEC_MAC_FILTER_ENTRY, portId, firstEmptyEntryId);

    vosConfigValueSet(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_MAC_FILTER_ADDR, pMacAddr);

	vosConfigUInt32Set(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_MAC_FILTER_VID, vlanId);

	vosConfigUInt32Set(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_MAC_FILTER_PORD, portId);

	/* modify mac filter num in config file */
	odmPortMacFilterNumGet(portId, &macFilterNum);

	macFilterNum += 1;

	odmPortMacFilterNumSet(portId, macFilterNum);

	return OK;
};

UINT32 odmPortMacFilterEntryDel(UINT32 portId, UINT16 vlanId, UINT8 *pMacAddr)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};
    OPL_API_DATA_t stData;
	HAL_PORT_MAC_FILTER_ENTRY_t halMacFilterEntry;
	UINT32 macFilterEntryId;
	UINT8  *pOldMacAddr;
	UINT16 oldVlanId;
	UINT8  macFilterNum;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: portId = %d, vid = %d, mac = %s\n", __FUNCTION__, portId, vlanId, pMacAddr);

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	/* call hal */
    stData.apiId = HAL_API_PORT_MAC_FILTER_ENTRY_DEL;
    stData.length = sizeof(HAL_PORT_MAC_FILTER_ENTRY_t);
	vosMemSet((UINT8 *)&halMacFilterEntry, 0, sizeof(HAL_PORT_MAC_FILTER_ENTRY_t));
    halMacFilterEntry.vlanId = vlanId;
    halMacFilterEntry.portId = portId;
	/*@ tanglin add it 10.05.21,judge the function return*/
	if(OK!=vosStrToMac(pMacAddr, &halMacFilterEntry.mac[0]))
	{
		return ERROR;
	}
	//vosStrToMac(pMacAddr, &halMacFilterEntry.mac[0]);
	/*---------------------------------------------------*/
	stData.param = (void *)&halMacFilterEntry;

    retVal = halAppApiCallSync(&stData);

	if(retVal != NO_ERROR)
	{
		return retVal;
	}

    if(NO_ERROR != halMacFilterEntry.retVal)
    {
    	OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"[ %s ]: retVal = %d\n", __FUNCTION__, halMacFilterEntry.retVal);
        return halMacFilterEntry.retVal;
    }


	/* delete mac filter entry from config file */
	for(macFilterEntryId = 0; macFilterEntryId < ODM_MAX_NUM_OF_MAC_FILTER; macFilterEntryId++)
	{

		vosSprintf(sectionBuff,FDB_SEC_MAC_FILTER_ENTRY, portId, macFilterEntryId);

		retVal = vosConfigSectionIsExisted(FDB_FILENAME_CFG, sectionBuff);
		if(OK != retVal)
		{
			continue;
		}

		pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_MAC_FILTER_ADDR,0);

		oldVlanId = (UINT16)vosConfigUInt32Get(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_MAC_FILTER_VID,0);

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: [ entry %d ], vid = %d, mac = %s\n", __FUNCTION__, macFilterEntryId, oldVlanId, pOldMacAddr);

		if((vosStrCmp(pOldMacAddr, pMacAddr) == 0) && (oldVlanId == vlanId))
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: match found, entry id = %d\n", __FUNCTION__, macFilterEntryId);
			/* remove match mac filter entry */
			vosConfigSectionDelete(FDB_FILENAME_CFG,sectionBuff);

			/* modify mac filter num */
			odmPortMacFilterNumGet(portId, &macFilterNum);

			macFilterNum -= 1;

			odmPortMacFilterNumSet(portId, macFilterNum);

			return NO_ERROR;
		}
	}

	return NO_ERROR;
};




UINT32 odmPortMacFilterNumGet(UINT32 portId, UINT8 *pEntryNum)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};
	UINT8  macFilterEntryBuff[255] = {0};

	vosSprintf(sectionBuff,FDB_SEC_MAC_FILTER_INFO ,portId);

	*pEntryNum = (UINT8)vosConfigUInt32Get(FDB_FILENAME_CFG ,sectionBuff,FDB_KEY_MAC_FITLER_NUM ,0);

	return OK;
};

UINT32 odmPortMacFilterNumSet(UINT32 portId, UINT8 entryNum)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};
	UINT8  macFilterEntryBuff[255] = {0};

	vosSprintf(sectionBuff,FDB_SEC_MAC_FILTER_INFO ,portId);

	vosConfigUInt32Set(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_MAC_FITLER_NUM ,entryNum);

	return OK;
};




UINT32 odmPortMacFilterEntryGet(UINT32 portId, UINT32 entryIndex, UINT16 *pVlanId, UINT8 *pMacAddr)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};
	UINT8  *pOldMacAddr;
	UINT16 oldVlanId;
	UINT32 i;
	UINT32 validEntryCounter;

	validEntryCounter = 0;

	for(i = 0; i < ODM_MAX_NUM_OF_MAC_FILTER; i++)
	{
		vosSprintf(sectionBuff,FDB_SEC_MAC_FILTER_ENTRY, portId, i);

		retVal = vosConfigSectionIsExisted(FDB_FILENAME_CFG, sectionBuff);
		if(OK != retVal)
		{
			continue;
		}
		else
		{
			validEntryCounter++;
		}

		if(validEntryCounter == (entryIndex + 1))
			break;
	}

	vosSprintf(sectionBuff,FDB_SEC_MAC_FILTER_ENTRY, portId,  i);

	pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_MAC_FILTER_ADDR,0);

	if(pOldMacAddr == 0)
	{
		return -1;
	}

	vosStrCpy(pMacAddr, pOldMacAddr);

	oldVlanId = (UINT16)vosConfigUInt32Get(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_MAC_FILTER_VID, 0);

	*pVlanId = oldVlanId;

	return NO_ERROR;
};

UINT32 odmPortMacFilterEntryClear(UINT32 portId)
{
	UINT32 retVal = OK;
	UINT8  i;
	UINT16 vlanId;
	UINT8  sectionBuff[255] = {0};
	UINT8  macFilterNum;
	UINT8  *pOldMacAddr;
    OPL_API_DATA_t stData;
	HAL_PORT_MAC_FILTER_ENTRY_t halMacFilterEntry;

	for(i = 0; i < ODM_MAX_NUM_OF_MAC_FILTER; i++)
	{

		vosSprintf(sectionBuff,FDB_SEC_MAC_FILTER_ENTRY, portId, i);

		retVal = vosConfigSectionIsExisted(FDB_FILENAME_CFG, sectionBuff);
		if(OK != retVal)
		{
			continue;
		}
		else
		{
			vlanId = (UINT16)vosConfigUInt32Get(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_MAC_FILTER_VID, 0);

			pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_MAC_FILTER_ADDR,0);

			/* call hal */
			stData.apiId = HAL_API_PORT_MAC_FILTER_ENTRY_DEL;
			stData.length = sizeof(HAL_PORT_MAC_FILTER_ENTRY_t);
			vosMemSet((UINT8 *)&halMacFilterEntry, 0, sizeof(HAL_PORT_MAC_FILTER_ENTRY_t));
			halMacFilterEntry.vlanId = vlanId;
			halMacFilterEntry.portId = portId;
			/*@ tanglin add it 10.05.21,judge the function return*/
			if(OK!=vosStrToMac(pOldMacAddr, &halMacFilterEntry.mac[0]))
			{
				return ERROR;
			}
			//vosStrToMac(pOldMacAddr, &halMacFilterEntry.mac[0]);
			/*---------------------------------------------------*/
			stData.param = (void *)&halMacFilterEntry;

			retVal = halAppApiCallSync(&stData);

			if(retVal != NO_ERROR)
			{
				return retVal;
			}

			if(NO_ERROR != halMacFilterEntry.retVal)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"[ %s ]: retVal = %d\n", __FUNCTION__, halMacFilterEntry.retVal);
				return halMacFilterEntry.retVal;
			}


			/* remove mac filter entry */
			vosConfigSectionDelete(FDB_FILENAME_CFG, sectionBuff);

		}
	}

	macFilterNum = 0;

	retVal = odmPortMacFilterNumSet(portId, macFilterNum);

	return retVal;
}

UINT32 odmPortStaticMacNumGet(UINT32 portId, UINT8 *pEntryNum)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};

	vosSprintf(sectionBuff,FDB_SEC_STATIC_MAC_INFO ,portId);

	*pEntryNum = (UINT8)vosConfigUInt32Get(FDB_FILENAME_CFG ,sectionBuff,FDB_KEY_STATIC_MAC_NUM ,0);

	return OK;
};

UINT32 odmPortStaticMacNumSet(UINT32 portId, UINT8 entryNum)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};

	vosSprintf(sectionBuff,FDB_SEC_STATIC_MAC_INFO ,portId);

	vosConfigUInt32Set(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_STATIC_MAC_NUM ,entryNum);

	return OK;
};


UINT32 odmPortStaticMacEntryAdd(UINT32 portId, UINT16 vlanId, UINT8 *pMacAddr)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};
	UINT8  staticMacNum;
	UINT16 oldVlanId;
	UINT8  *pOldMacAddr;
	UINT32 entryId;
    UINT32 firstEmptyEntryId = 0xffff;
    OPL_API_DATA_t stData;
	HAL_PORT_STATIC_MAC_ENTRY_t halStaticMacEntry;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: portId = %d, vid = %d, mac = %s\n", __FUNCTION__, portId, vlanId, pMacAddr);

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	retVal = odmPortStaticMacNumGet(portId, &staticMacNum);
	if(retVal != OK)
	{
		return retVal;
	}

	if(staticMacNum == ODM_MAX_NUM_OF_STATIC_MAC)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: No enough mac filter resource!\n", __FUNCTION__);
		return -1;
	}

	/* call hal */
    stData.apiId = HAL_API_PORT_STATIC_MAC_ENTRY_ADD;
    stData.length = sizeof(HAL_PORT_STATIC_MAC_ENTRY_t);
    halStaticMacEntry.vlanId = vlanId;
    halStaticMacEntry.portId = portId;
     /*@ tanglin add it 10.05.21,judge the function return*/
    if(OK!=vosStrToMac(pMacAddr, &halStaticMacEntry.mac[0]))
	{
		return ERROR;
	}
    /*-----------------------------------*/
	//vosStrToMac(pMacAddr, &halStaticMacEntry.mac[0]);
    stData.param = (void *)&halStaticMacEntry;

	retVal = halAppApiCallSync(&stData);

	if(retVal != NO_ERROR)
	{
		return retVal;
	}

    if(NO_ERROR != halStaticMacEntry.retVal)
    {
        return halStaticMacEntry.retVal;
    }

	/* add static mac entry to config file */

	for(entryId = 0; entryId < ODM_MAX_NUM_OF_STATIC_MAC; entryId++)
	{
		vosSprintf(sectionBuff, FDB_SEC_STATIC_MAC_ENTRY, portId, entryId);

	    retVal = vosConfigSectionIsExisted(FDB_FILENAME_CFG, sectionBuff);
		if(OK != retVal)
		{
    		if(0xffff == firstEmptyEntryId)
    		{
        		firstEmptyEntryId = entryId;
    		}
    		continue;
		}

		pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_STATIC_MAC_ADDR, 0);

		oldVlanId = vosConfigUInt32Get(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_STATIC_MAC_VID, 0);

		if((vosStrCmp(pOldMacAddr, pMacAddr)== 0) && (oldVlanId == vlanId))
		{
			/* entry already exist */
		    return NO_ERROR;
		}
    }

    if(0xffff == firstEmptyEntryId)
    {
        return -1;
    }

	/* add static mac entry to config file */
	vosSprintf(sectionBuff, FDB_SEC_STATIC_MAC_ENTRY, portId, firstEmptyEntryId);

    vosConfigValueSet(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_STATIC_MAC_ADDR, pMacAddr);

	vosConfigUInt32Set(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_STATIC_MAC_VID, vlanId);

	/* modify static mac entry num in config file */
	odmPortStaticMacNumGet(portId, &staticMacNum);

	staticMacNum += 1;

	odmPortStaticMacNumSet(portId, staticMacNum);

	return OK;
};

UINT32 odmPortStaticMacEntryDel(UINT32 portId, UINT16 vlanId, UINT8 *pMacAddr)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};
    OPL_API_DATA_t stData;
	HAL_PORT_STATIC_MAC_ENTRY_t halStaticMacEntry;
	UINT32 entryId;
	UINT8  *pOldMacAddr;
	UINT16 oldVlanId;
	UINT8  staticMacNum;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: portId = %d, vid = %d, mac = %s\n", __FUNCTION__, portId, vlanId, pMacAddr);

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	/* call hal */
    stData.apiId = HAL_API_PORT_STATIC_MAC_ENTRY_DEL;
    stData.length = sizeof(HAL_PORT_STATIC_MAC_ENTRY_t);
	vosMemSet((UINT8 *)&halStaticMacEntry, 0, sizeof(HAL_PORT_MAC_FILTER_ENTRY_t));
    halStaticMacEntry.vlanId = vlanId;
    halStaticMacEntry.portId = portId;
	/*@ tanglin add it 10.05.21,judge the function return*/
	if(OK!=vosStrToMac(pMacAddr, &halStaticMacEntry.mac[0]))
	{
		return ERROR;
	}
	/*-----------------------------------*/
	//vosStrToMac(pMacAddr, &halStaticMacEntry.mac[0]);
	stData.param = (void *)&halStaticMacEntry;

    retVal = halAppApiCallSync(&stData);

	if(retVal != NO_ERROR)
	{
		return retVal;
	}

    if(NO_ERROR != halStaticMacEntry.retVal)
    {
    	OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"[ %s ]: retVal = %d\n", __FUNCTION__, halStaticMacEntry.retVal);
        return halStaticMacEntry.retVal;
    }

	/* delete static mac entry from config file */
	for(entryId = 0; entryId < ODM_MAX_NUM_OF_STATIC_MAC; entryId++)
	{

		vosSprintf(sectionBuff,FDB_SEC_STATIC_MAC_ENTRY, portId, entryId);

		retVal = vosConfigSectionIsExisted(FDB_FILENAME_CFG, sectionBuff);
		if(OK != retVal)
		{
			continue;
		}

		pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_STATIC_MAC_ADDR,0);

		oldVlanId = (UINT16)vosConfigUInt32Get(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_STATIC_MAC_VID,0);

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: [ entry %d ], vid = %d, mac = %s\n", __FUNCTION__, entryId, oldVlanId, pOldMacAddr);

		if((vosStrCmp(pOldMacAddr, pMacAddr) == 0) && (oldVlanId == vlanId))
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: match found, entry id = %d\n", __FUNCTION__, entryId);
			/* remove match static mac entry */
			vosConfigSectionDelete(FDB_FILENAME_CFG,sectionBuff);

			/* modify static mac num */
			odmPortStaticMacNumGet(portId, &staticMacNum);

			staticMacNum -= 1;

			odmPortStaticMacNumSet(portId, staticMacNum);

			return NO_ERROR;
		}
	}

	return -1;
};

UINT32 odmPortStaticMacEntryGet(UINT32 portId, UINT32 entryIndex, UINT16 *pVlanId, UINT8 *pMacAddr)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};
	UINT8  *pOldMacAddr;
	UINT16 oldVlanId;
	UINT32 i;
	UINT32 validEntryCounter;

	validEntryCounter = 0;

	for(i = 0; i < ODM_MAX_NUM_OF_STATIC_MAC; i++)
	{
		vosSprintf(sectionBuff,FDB_SEC_STATIC_MAC_ENTRY, portId, i);

		retVal = vosConfigSectionIsExisted(FDB_FILENAME_CFG, sectionBuff);
		if(OK != retVal)
		{
			continue;
		}
		else
		{
			validEntryCounter++;
		}

		if(validEntryCounter == (entryIndex + 1))
			break;
	}

	vosSprintf(sectionBuff,FDB_SEC_STATIC_MAC_ENTRY, portId,  i);

	pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_STATIC_MAC_ADDR,0);

	if(pOldMacAddr == 0)
	{
		return -1;
	}

	vosStrCpy(pMacAddr, pOldMacAddr);

	oldVlanId = (UINT16)vosConfigUInt32Get(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_STATIC_MAC_VID, 0);

	*pVlanId = oldVlanId;

	return NO_ERROR;
};

UINT32 odmPortStaticMacEntryClear(UINT32 portId)
{
	UINT32 retVal = OK;
	UINT8  i;
	UINT16 vlanId;
	UINT8  sectionBuff[255] = {0};
	UINT8  staticMacNum;
	UINT8  *pOldMacAddr;
    OPL_API_DATA_t stData;
	HAL_PORT_STATIC_MAC_ENTRY_t halStaticMacEntry;

	for(i = 0; i < ODM_MAX_NUM_OF_STATIC_MAC; i++)
	{

		vosSprintf(sectionBuff,FDB_SEC_STATIC_MAC_ENTRY, portId, i);

		retVal = vosConfigSectionIsExisted(FDB_FILENAME_CFG, sectionBuff);
		if(OK != retVal)
		{
			continue;
		}
		else
		{
			vlanId = (UINT16)vosConfigUInt32Get(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_STATIC_MAC_VID, 0);

			pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_STATIC_MAC_ADDR,0);

			/* call hal */
			stData.apiId = HAL_API_PORT_STATIC_MAC_ENTRY_DEL;
			stData.length = sizeof(HAL_PORT_STATIC_MAC_ENTRY_t);
			vosMemSet((UINT8 *)&halStaticMacEntry, 0, sizeof(HAL_PORT_STATIC_MAC_ENTRY_t));
			halStaticMacEntry.vlanId = vlanId;
			halStaticMacEntry.portId = portId;
			/*@ tanglin add it 10.05.21,judge the function return*/
			if(OK!=vosStrToMac(pOldMacAddr, &halStaticMacEntry.mac[0]))
			{
				return ERROR;
			}
			//vosStrToMac(pOldMacAddr, &halStaticMacEntry.mac[0]);
			/*-----------------------------------*/

			stData.param = (void *)&halStaticMacEntry;

			retVal = halAppApiCallSync(&stData);

			if(retVal != NO_ERROR)
			{
				return retVal;
			}

			if(NO_ERROR != halStaticMacEntry.retVal)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"[ %s ]: retVal = %d\n", __FUNCTION__, halStaticMacEntry.retVal);
				return halStaticMacEntry.retVal;
			}


			/* remove static mac entry */
			vosConfigSectionDelete(FDB_FILENAME_CFG, sectionBuff);

		}
	}

	staticMacNum = 0;

	retVal = odmPortStaticMacNumSet(portId, staticMacNum);

	return retVal;
}

UINT32 odmPortDlfFilterEnable(UINT32 portId, UINT32 enable)
{
  UINT32 retVal = OK;
  OPL_API_DATA_t stData;
  HAL_PORT_DLF_FILTER_EN_t portDlfFilterenable;
  UINT32 dalStatus = OK;

  stData.apiId = HAL_API_PORT_DLF_FILTER_ENABLE;
	stData.length = sizeof(HAL_PORT_DLF_FILTER_EN_t);
  portDlfFilterenable.portId = portId;
	portDlfFilterenable.enable = enable;
	stData.param = (void *)&portDlfFilterenable;

  dalStatus = halAppApiCallSync(&stData);
  if(OK != dalStatus)
	{
		ODM_PORT_LOG_TRACE();
		return dalStatus;
	}
	return retVal;
}

/*added by lancunyi at 2010-06-08*/
UINT32 odmPortLockDropEnable(UINT32 portId, UINT32 enable)
{
    UINT32 retVal = OK;
    OPL_API_DATA_t stData;
    HAL_PORT_CTL_LOCK_DROP_EN_t portCntlLockenable;
    UINT32 dalStatus = OK;

    stData.apiId = HAL_API_PORT_CTL_LOCK_DROP_ENABLE;
    stData.length = sizeof(HAL_PORT_CTL_LOCK_DROP_EN_t);
    portCntlLockenable.portId = portId;
    portCntlLockenable.enable = enable;
    stData.param = (void *)&portCntlLockenable;

    dalStatus = halAppApiCallSync(&stData);
    if(OK != dalStatus)
	{
		ODM_PORT_LOG_TRACE();
		return dalStatus;
	}
	return retVal;
}
/*end added*/

UINT32 odmPortBindMacNumGet(UINT32 portId, UINT8 *pEntryNum)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};

	vosSprintf(sectionBuff,FDB_SEC_BIND_MAC_INFO ,portId);

	*pEntryNum = (UINT8)vosConfigUInt32Get(FDB_FILENAME_CFG ,sectionBuff,FDB_KEY_BIND_MAC_NUM ,0);

	return OK;
};

UINT32 odmPortBindMacNumSet(UINT32 portId, UINT8 entryNum)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};

	vosSprintf(sectionBuff,FDB_SEC_BIND_MAC_INFO ,portId);

	vosConfigUInt32Set(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_BIND_MAC_NUM ,entryNum);

	return OK;
};


UINT32 odmPortBindMacEntryAdd(UINT32 portId, UINT16 vlanId, UINT8 *pMacAddr)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};
	UINT8  entryNum;
	UINT16 oldVlanId;
	UINT8  *pOldMacAddr;
	UINT32 entryId;
	UINT32 macFilterEntryId;
   	UINT32 firstEmptyEntryId = 0xffff;
    	OPL_API_DATA_t stData;
	HAL_PORT_BIND_MAC_ENTRY_t halBindMacEntry;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: portId = %d, vid = %d, mac = %s\n", __FUNCTION__, portId, vlanId, pMacAddr);

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	retVal = odmPortBindMacNumGet(portId, &entryNum);
	if(retVal != OK)
	{
		return retVal;
	}
	

	if(entryNum == ODM_MAX_NUM_OF_BIND_MAC)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: No enough mac filter resource!\n", __FUNCTION__);
		return -1;
	}
	/* decide mac whether exist in filter */
	for(macFilterEntryId = 0; macFilterEntryId < ODM_MAX_NUM_OF_MAC_FILTER; macFilterEntryId++)
	{
		vosSprintf(sectionBuff, FDB_SEC_MAC_FILTER_ENTRY, portId, macFilterEntryId);

	    retVal = vosConfigSectionIsExisted(FDB_FILENAME_CFG, sectionBuff);
		if(OK != retVal)
		{
    		continue;
		}

	    pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_MAC_FILTER_ADDR, 0);

		if((vosStrCmp(pOldMacAddr, pMacAddr)== 0) )
		{
			/* entry already exist */
		    return ERR_DUPLICATE_ITEM;
		}
	}

    /*add by ltang for bug3282 --start */
    if(entryNum <0 || entryNum > ODM_MAX_NUM_OF_BIND_MAC )
    {
        printf("Caution: Error entryNum !\n");
        return -1;
    }
    else if (entryNum > 0)
    {
        // 0 < entryNum < ODM_MAX_NUM_OF_BIND_MAC
        // just add the bind entry
        // printf("add anthoner bind entry");
    }
    else if (0 == entryNum)
    {
        //entryNum = 0  need clear fdb dynamic and static fdb entry

        odmPortLearningEnableSet(portId, FALSE);
        retVal=odmFdbMacClearAllByPort(portId);
        if(OPL_OK != retVal )
        {
            printf("\n[ %s ]: dalArlFlushAll return Error \n", __FUNCTION__);
        }
    }
    /*add by ltang for bug3282 --end */

    /* call hal */
    stData.apiId = HAL_API_PORT_BIND_MAC_ENTRY_ADD;
    stData.length = sizeof(HAL_PORT_BIND_MAC_ENTRY_t);
    halBindMacEntry.vlanId = vlanId;
    halBindMacEntry.portId = portId;

	/*bwei add it 10.05.12, check mac validity*/
 	retVal = vosStrToMac(pMacAddr, &halBindMacEntry.mac[0]);

 	if(retVal != NO_ERROR)
	{
		return retVal;
	}
	/*------------------------------------------*/

    stData.param = (void *)&halBindMacEntry;

	retVal = halAppApiCallSync(&stData);

	if(retVal != NO_ERROR)
	{
		return retVal;
	}

    if(NO_ERROR != halBindMacEntry.retVal)
    {
        return halBindMacEntry.retVal;
    }

    /* disable learning */
    #ifndef ONU_4PORT_AR8327
    odmPortLearningEnableSet(portId, FALSE);
	#if defined(ONU_4PORT_AR8228)
    //odmPortDlfFilterEnable(portId, TRUE);
    odmPortLockDropEnable(portId, TRUE);
	#endif
	/* add bind mac entry to config file */
    #else
    odmPortLearningEnableSet(portId, TRUE);
    odmPortLockDropEnable(portId, TRUE);
    #endif

	for(entryId = 0; entryId < ODM_MAX_NUM_OF_BIND_MAC; entryId++)
	{
		vosSprintf(sectionBuff, FDB_SEC_BIND_MAC_ENTRY, portId, entryId);

	    retVal = vosConfigSectionIsExisted(FDB_FILENAME_CFG, sectionBuff);
		if(OK != retVal)
		{
    		if(0xffff == firstEmptyEntryId)
    		{
        		firstEmptyEntryId = entryId;
    		}
    		continue;
		}

		pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_BIND_MAC_ADDR, 0);

		oldVlanId = vosConfigUInt32Get(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_BIND_MAC_VID, 0);

		if((vosStrCmp(pOldMacAddr, pMacAddr)== 0) && (oldVlanId == vlanId))
		{
			/* entry already exist */
		    return NO_ERROR;
		}
    }

    if(0xffff == firstEmptyEntryId)
    {
        return -1;
    }

	/* add bind mac entry to config file */
	vosSprintf(sectionBuff, FDB_SEC_BIND_MAC_ENTRY, portId, firstEmptyEntryId);

    vosConfigValueSet(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_BIND_MAC_ADDR, pMacAddr);

	vosConfigUInt32Set(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_BIND_MAC_VID, vlanId);

	/* modify bind mac entry num in config file */
	odmPortBindMacNumGet(portId, &entryNum);

	entryNum += 1;

	odmPortBindMacNumSet(portId, entryNum);

	return OK;
};

UINT32 odmPortBindMacEntryDel(UINT32 portId, UINT16 vlanId, UINT8 *pMacAddr)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};
    OPL_API_DATA_t stData;
	HAL_PORT_BIND_MAC_ENTRY_t halBindMacEntry;
	UINT32 entryId;
	UINT8  *pOldMacAddr;
	UINT16 oldVlanId;
	UINT8  entryNum;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: portId = %d, vid = %d, mac = %s\n", __FUNCTION__, portId, vlanId, pMacAddr);

	if(odmPortRangeCheck(portId))
	{
		return PORT_INVALID;
	}

	/* call hal */
    stData.apiId = HAL_API_PORT_BIND_MAC_ENTRY_DEL;
    stData.length = sizeof(HAL_PORT_BIND_MAC_ENTRY_t);
	vosMemSet((UINT8 *)&halBindMacEntry, 0, sizeof(HAL_PORT_BIND_MAC_ENTRY_t));
    halBindMacEntry.vlanId = vlanId;
    halBindMacEntry.portId = portId;
	/*@ tanglin add it 10.05.21,judge the function return*/
	if(OK!=vosStrToMac(pMacAddr, &halBindMacEntry.mac[0]))
	{
		return ERROR;
	}
	//vosStrToMac(pMacAddr, &halBindMacEntry.mac[0]);
	/*---------------------------------------------------*/
	stData.param = (void *)&halBindMacEntry;

    retVal = halAppApiCallSync(&stData);

	if(retVal != NO_ERROR)
	{
		return retVal;
	}

    if(NO_ERROR != halBindMacEntry.retVal)
    {
    	OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"[ %s ]: retVal = %d\n", __FUNCTION__, halBindMacEntry.retVal);
        return halBindMacEntry.retVal;
    }

	/* delete bind mac entry from config file */
	for(entryId = 0; entryId < ODM_MAX_NUM_OF_BIND_MAC; entryId++)
	{

		vosSprintf(sectionBuff,FDB_SEC_BIND_MAC_ENTRY, portId, entryId);

		retVal = vosConfigSectionIsExisted(FDB_FILENAME_CFG, sectionBuff);
		if(OK != retVal)
		{
			continue;
		}

		pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_BIND_MAC_ADDR,0);

		oldVlanId = (UINT16)vosConfigUInt32Get(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_BIND_MAC_VID,0);

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: [ entry %d ], vid = %d, mac = %s\n", __FUNCTION__, entryId, oldVlanId, pOldMacAddr);

		if((vosStrCmp(pOldMacAddr, pMacAddr) == 0) && (oldVlanId == vlanId))
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\n[ %s ]: match found, entry id = %d\n", __FUNCTION__, entryId);
			/* remove match bind mac entry */
			vosConfigSectionDelete(FDB_FILENAME_CFG,sectionBuff);

			/* modify bind mac num */
			odmPortBindMacNumGet(portId, &entryNum);

			entryNum -= 1;

			odmPortBindMacNumSet(portId, entryNum);

            if (0 == entryNum)
            {
                /* enable learning */
                odmPortLearningEnableSet(portId, TRUE);
				#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
                //odmPortDlfFilterEnable(portId, FALSE);
                odmPortLockDropEnable(portId, FALSE);
				#endif
            }

			return NO_ERROR;
		}
	}

	return -1;
};

UINT32 odmPortBindMacEntryGet(UINT32 portId, UINT32 entryIndex, UINT16 *pVlanId, UINT8 *pMacAddr)
{
	UINT32 retVal = OK;
	UINT8  sectionBuff[255] = {0};
	UINT8  *pOldMacAddr;
	UINT16 oldVlanId;
	UINT32 i;
	UINT32 validEntryCounter;

	validEntryCounter = 0;

	for(i = 0; i < ODM_MAX_NUM_OF_BIND_MAC; i++)
	{
		vosSprintf(sectionBuff,FDB_SEC_BIND_MAC_ENTRY, portId, i);

		retVal = vosConfigSectionIsExisted(FDB_FILENAME_CFG, sectionBuff);
		if(OK != retVal)
		{
			continue;
		}
		else
		{
			validEntryCounter++;
		}

		if(validEntryCounter == (entryIndex + 1))
			break;
	}

	vosSprintf(sectionBuff,FDB_SEC_BIND_MAC_ENTRY, portId,  i);

	pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_BIND_MAC_ADDR,0);

	if(pOldMacAddr == 0)
	{
		return -1;
	}

	vosStrCpy(pMacAddr, pOldMacAddr);

	oldVlanId = (UINT16)vosConfigUInt32Get(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_BIND_MAC_VID, 0);

	*pVlanId = oldVlanId;

	return NO_ERROR;
};

UINT32 odmPortBindMacEntryClear(UINT32 portId)
{
	UINT32 retVal = OK;
	UINT8  i;
	UINT16 vlanId;
	UINT8  sectionBuff[255] = {0};
	UINT8  entryNum;
	UINT8  *pOldMacAddr;
    OPL_API_DATA_t stData;
	HAL_PORT_BIND_MAC_ENTRY_t halBindMacEntry;

	for(i = 0; i < ODM_MAX_NUM_OF_BIND_MAC; i++)
	{

		vosSprintf(sectionBuff,FDB_SEC_BIND_MAC_ENTRY, portId, i);

		retVal = vosConfigSectionIsExisted(FDB_FILENAME_CFG, sectionBuff);
		if(OK != retVal)
		{
			continue;
		}
		else
		{
			vlanId = (UINT16)vosConfigUInt32Get(FDB_FILENAME_CFG, sectionBuff, FDB_KEY_BIND_MAC_VID, 0);
			pOldMacAddr = vosConfigValueGet(FDB_FILENAME_CFG,sectionBuff,FDB_KEY_BIND_MAC_ADDR,0);

			/* call hal */
			stData.apiId = HAL_API_PORT_BIND_MAC_ENTRY_DEL;
			stData.length = sizeof(HAL_PORT_BIND_MAC_ENTRY_t);
			vosMemSet((UINT8 *)&halBindMacEntry, 0, sizeof(HAL_PORT_BIND_MAC_ENTRY_t));
			halBindMacEntry.vlanId = vlanId;
			halBindMacEntry.portId = portId;
			/*@ tanglin add it 10.05.21,judge the function return*/
			if(OK!=vosStrToMac(pOldMacAddr, &halBindMacEntry.mac[0]))
			{
				return ERROR;
			}
			//vosStrToMac(pOldMacAddr, &halBindMacEntry.mac[0]);
			/*---------------------------------------------------*/
			stData.param = (void *)&halBindMacEntry;

			retVal = halAppApiCallSync(&stData);

			if(retVal != NO_ERROR)
			{
				return retVal;
			}

			if(NO_ERROR != halBindMacEntry.retVal)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"[ %s ]: retVal = %d\n", __FUNCTION__, halBindMacEntry.retVal);
				return halBindMacEntry.retVal;
			}


			/* remove bind mac entry */
			vosConfigSectionDelete(FDB_FILENAME_CFG, sectionBuff);

		}
	}

	entryNum = 0;
	odmPortLearningEnableSet(portId, TRUE);
	#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
  	//odmPortDlfFilterEnable(portId, FALSE); /*fix bug 3503*/
  	odmPortLockDropEnable(portId, FALSE);
	#endif
	retVal = odmPortBindMacNumSet(portId, entryNum);

	return retVal;
}


