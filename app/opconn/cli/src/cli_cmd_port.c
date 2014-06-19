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
* FILENAME:  cli_cmd_port.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_cmd_port.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "cli.h"
#include "cli_cmd_list.h"
#include "rstp_in.h"
#include "odm_port.h"
#include "opconn_hw_reg.h"
//wfxu #include "opconn_lib.h"
#include "opconn_usr_ioctrl.h"

#define CLI_CMD_PORT_DEBUG 			1

STATUS cliConvertPortListAndCheck(ENV_t *pstEnv,UINT32 *portArry,UINT32 startPortId,UINT32 numOfPorts)
{
	STATUS retVal = OK;
	UINT8 portListBuff[256];

	vosMemSet(portListBuff,0x00,256);

	if(NULL == pstEnv || NULL == portArry || NULL == pstEnv->para)
	{
		return ERROR;
	}

	if(startPortId >= numOfPorts)
	{
		return ERROR;
	}

	vosStrCpy(portListBuff,(UINT8 *)pstEnv->para);
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

	return OK;
}

/*****************************************************************************
 *cliCmdPortAdminEnable  - config
 *DESCRIPTION
 *.
 *Input:N/A.
 *Output:N/A.
 *Return:int.
 ****************************************************************************/
STATUS cliCmdPortAdminEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal;
	UINT32 enable;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

	if(psPara->i == 1)
	{
		enable = TRUE;
	}else if(psPara->i == 2)
	{
		enable = FALSE;
	}else
	{
		vosPrintf(pstEnv->nWriteFd, "\r\ninput error.\r\n");
		return ERROR;
	}

	for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortAdminSet(portId,enable);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d admin failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortAutoEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal;
	UINT32 enable;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;
	UINT32 portSpeed;
	UINT32 duplex;

	if (NULL == pstEnv || NULL == psPara)
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

	if(psPara->i == 1)
	{
		enable = TRUE;
	}else if(psPara->i == 2)
	{
		enable = FALSE;
	}else
	{
		vosPrintf(pstEnv->nWriteFd, "\r\ninput error.\r\n");
		return ERROR;
	}

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			if (TRUE == enable)
			{
				retVal = odmPortAutoEnableSet(portId,enable);
				if(retVal != OK)
				{
					vosPrintf(pstEnv->nWriteFd, "\r\nset port %d auto neg enable failed.\r\n",portId);
					return retVal;
				}				
			}
			else
			{
				/* get config info */
				odmPortSpeedGet(portId, &portSpeed);
				odmPortDuplexGet(portId, &duplex);
				
				retVal = odmPortAutoEnableSet(portId,enable);
		    	retVal += odmPortSpeedSet(portId, portSpeed);
		    	retVal += odmPortDuplexSet(portId, duplex);								
				if(retVal != OK)
				{
					vosPrintf(pstEnv->nWriteFd, "\r\nset port %d auto neg enable failed.\r\n",portId);
					return retVal;
				}				
			}				
		}
	}
	return OK;
}

STATUS cliCmdPortSpeedSelect(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal;
	UINT32 portSpeed;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;
	UINT32 autoenable;

	if (NULL == pstEnv || NULL == psPara)
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

	if(psPara->i == 1)
	{
		portSpeed = ODM_PORT_SPEED_10M;
	}
    else if(psPara->i == 2)
	{
		portSpeed = ODM_PORT_SPEED_100M;
	}
    else
	{
		vosPrintf(pstEnv->nWriteFd, "\r\ninput error.\r\n");
		return ERROR;
	}

#if defined(ONU_1PORT)
#elif defined(MULTI_PORT)
    if(ODM_PORT_SPEED_1000M ==portSpeed)
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nthese ports(list:%s) not support 1000M.\r\n",(UINT8 *)pstEnv->para);
        return ERROR;
    }
#endif

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			odmPortAutoEnableGet(portId, &autoenable);
			if (TRUE == autoenable)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d speed failed, because of current port is auto neg mode.\r\n",portId);
				return ERROR;
			}	
			
			retVal = odmPortSpeedSet(portId,portSpeed);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d speed failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortDuplexSelect(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal;
	UINT32 portDuplex;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;
	UINT32 autoenable;	

	if (NULL == pstEnv || NULL == psPara)
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

	if(psPara->i == 1)
	{
		portDuplex = ODM_PORT_DUPLEX_FULL;
	}else if(psPara->i == 2)
	{
		portDuplex = ODM_PORT_DUPLEX_HALF;
	}else
	{
		vosPrintf(pstEnv->nWriteFd, "\r\ninput error.\r\n");
		return ERROR;
	}

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			odmPortAutoEnableGet(portId, &autoenable);
			if (TRUE == autoenable)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d duplex failed, because of current port is auto neg mode.\r\n",portId);
				return ERROR;
			}
			
			retVal = odmPortDuplexSet(portId,portDuplex);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d duplex failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortFlowControlEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal;
	UINT32 enable;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

	if(psPara->i == 1)
	{
		enable = TRUE;
	}else if(psPara->i == 2)
	{
		enable = FALSE;
	}else
	{
		vosPrintf(pstEnv->nWriteFd, "\r\ninput error.\r\n");
		return ERROR;
	}

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortFlowCtrolEnableSet(portId,enable);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d pause failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortLoopbackEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal;
	UINT32 enable;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

	if(psPara->i == 1)
	{
		enable = TRUE;
	}else if(psPara->i == 2)
	{
		enable = FALSE;
	}else
	{
		vosPrintf(pstEnv->nWriteFd, "\r\ninput error.\r\n");
		return ERROR;
	}

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortLoopbackEnableSet(portId,enable);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d pause failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortLearningEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal;
	UINT32 enable;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

	if(psPara->i == 1)
	{
		enable = FALSE;
	}else if(psPara->i == 2)
	{
		enable = TRUE;
	}else
	{
		vosPrintf(pstEnv->nWriteFd, "\r\ninput error.\r\n");
		return ERROR;
	}

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortLearningEnableSet(portId,enable);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d learn failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortMacLimitNumEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;
	UINT32 numOfMac;
	UINT8  entryNum = 0;

	if (NULL == pstEnv || NULL == psPara)
	{
		return ERROR;
	}

	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));

	if(NULL == pstEnv->para)
	{
		return ERROR;
	}
	
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		odmPortBindMacNumGet(portId, &entryNum);
		if ( entryNum > 0 ){
			vosPrintf(pstEnv->nWriteFd, "Please delete mac bind entry first!\n");
			return ERROR;
		}
	}
	

	retVal = cliConvertPortListAndCheck(pstEnv,portArry,ODM_START_PORT_NUN,ODM_NUM_OF_PORTS+1);
	if(OK != retVal)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",(UINT8 *)pstEnv->para);
		return ERROR;
	}

	numOfMac = psPara[0].u;

#ifdef CLI_CMD_PORT_DEBUG
	vosPrintf(pstEnv->nWriteFd, "\r\nmacnum =  %u\r\n",numOfMac);
#endif

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortMacLimitNumEnableSet(portId,TRUE,numOfMac);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d mac limit  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortMacLimitNumDisable(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 portId;
	UINT8  entryNum = 0;

	if (NULL == pstEnv)
	{
		return ERROR;
	}

	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));

	if(NULL == pstEnv->para)
	{
		return ERROR;
	}

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		odmPortBindMacNumGet(portId, &entryNum);
		if ( entryNum > 0 ){
			vosPrintf(pstEnv->nWriteFd, "Please delete mac bind entry first!\n");
			return ERROR;
		}
	}
	

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
			retVal = odmPortMacLimitNumEnableSet(portId,FALSE,0);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d mac limit  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortUspolicingEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 enable;
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

	if(psPara[0].i == 1)
	{
		enable = TRUE;
	}else if(psPara[0].i == 2)
	{
		enable = FALSE;
	}else
	{
		vosPrintf(pstEnv->nWriteFd, "\r\ninput error.\r\n");
		return ERROR;
	}

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortUsPolicingEnableSet(portId,enable);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d upstream policing  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortUscirConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 usCir;
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

	usCir = psPara[0].u;

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortUsCirSet(portId,usCir);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d upstream policing cir failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}
STATUS cliCmdPortUscbsConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 usCbs;
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

	usCbs = psPara[0].u;

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortUsCbsSet(portId,usCbs);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d upstream policing cbs  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortUsebsConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 usEbs;
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

	usEbs = psPara[0].u;

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortUsEbsSet(portId,usEbs);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d upstream policing ebs  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortDsRateLimitEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 enable;
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

	if(psPara[0].i == 1)
	{
		enable = TRUE;
	}else if(psPara[0].i == 2)
	{
		enable = FALSE;
	}else
	{
		vosPrintf(pstEnv->nWriteFd, "\r\ninput error.\r\n");
		return ERROR;
	}

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortDsRateLimitEnableSet(portId,enable);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d downstream policing  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortDscirConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 dsCir;
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

	dsCir = psPara[0].u;

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortDsCirSet(portId,dsCir);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d upstream policing cir  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortDsCbsConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 dsCbs;
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

	dsCbs = psPara[0].u;

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortDsCbsSet(portId,dsCbs);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d upstream policing PIR  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortMulticastTagStripEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 enable;
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

	if(1 == psPara[0].i)
	{
		enable = TRUE;
	}else if(2 == psPara[0].i)
	{
		enable = FALSE;
	}else
	{
		vosPrintf(pstEnv->nWriteFd, "\r\ninput error.\r\n");
		return ERROR;
	}

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortMultcastTagStripEnableSet(portId,enable);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d multicast strip failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortMaxMulticastGroupNumConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 num;
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

	num = psPara[0].u;

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortMaxMulticastGroupNumSet(portId,num);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d multicast max group num  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}


STATUS cliCmdPortVlanModeConfigTransparent(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 vlanMode;
	UINT32 portId;

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

	vlanMode = ODM_VLAN_TRANSPARENT;

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortVlanModeSet(portId,vlanMode);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d vlan mode  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortVlanModeConfigTag(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 vlanMode = ODM_VLAN_TAG;
    UINT32 pvid = 0x0;
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

    pvid = psPara[0].i;

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
            retVal = odmPortDefaultVlanSet(portId, pvid);
            if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d port vlan failed.\r\n",portId);
				return retVal;
			}
			retVal = odmPortVlanModeSet(portId,vlanMode);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d vlan mode  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortVlanModeConfigTrunk(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 vlanMode = ODM_VLAN_TRUNK;
    UINT32 pvid = 0x0;
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

    pvid = psPara[0].i;

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
            retVal = odmPortDefaultVlanSet(portId, pvid);
            if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d port vlan failed.\r\n",portId);
				return retVal;
			}
			retVal = odmPortVlanModeSet(portId,vlanMode);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d vlan mode  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortVlanModeConfigTranslation(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 vlanMode;
	UINT32 portId;
    UINT32 defaultVlan;

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

	vlanMode = ODM_VLAN_TRANSLATION;
    defaultVlan = psPara[0].i;

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
            retVal = odmPortDefaultVlanSet(portId, defaultVlan);
            if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d port vlan failed.\r\n",portId);
				return retVal;
			}
			retVal = odmPortVlanModeSet(portId,vlanMode);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d vlan mode  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortVlanModeConfigAggregation(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
  STATUS retVal = OK;
  UINT32 portArry[ODM_NUM_OF_PORTS + 1];
  UINT32 vlanMode;
  UINT32 portId;
    UINT32 defaultVlan;

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

  vlanMode = ODM_VLAN_AGGREGATION;
    defaultVlan = psPara[0].i;

  for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
  {
    if(portArry[portId] == 1)
    {
      retVal = odmPortDefaultVlanSet(portId, defaultVlan);
      if(retVal != OK)
      {
        vosPrintf(pstEnv->nWriteFd, "\r\nset port %d port vlan failed.\r\n",portId);
        return retVal;
      }
      retVal = odmPortVlanModeSet(portId,vlanMode);
      if(retVal != OK)
      {
        vosPrintf(pstEnv->nWriteFd, "\r\nset port %d vlan mode  failed.\r\n",portId);
        return retVal;
      }
    }
  }
  return OK;
}

STATUS cliCmdPortDefaultVlanConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 defaultVlan;
	UINT32 portId;

	if (NULL == pstEnv || NULL == psPara)
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

	defaultVlan = psPara[0].u;

	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
			retVal = odmPortDefaultVlanSet(portId,defaultVlan);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d default valn  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortVttAdd(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 oldVlan,newVlan;
	UINT32 portId;
    UINT32 vlanMode;
	UINT32 defaultVlan;

	if (NULL == pstEnv || NULL == psPara)
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

	oldVlan = psPara[0].u;
	newVlan = psPara[1].u;

	for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
		    retVal = odmPortVlanModeGet(portId,&vlanMode);
            if(OK != retVal)
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
                return retVal;
            }
            if(ODM_VLAN_TRANSLATION != vlanMode)
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nport%d is not in translation mode.please set to translation mode first.\r\n",portId);
                return VTT_VLAN_NOT_AT_TRANSLATION_MODE;
            }

			/* judge if default vlan is equal oldvlan */
		    retVal = odmPortDefaultVlanGet(portId,&defaultVlan);
            if(OK != retVal)
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nget default vlan error[%u]\r\n",retVal);
                return retVal;
            }

			if (defaultVlan == oldVlan)
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nvlan to be translate can't equal default vlan\r\n");
                return ERROR;
            }
			
			retVal = odmPortVttEntryAdd(portId,0,oldVlan,0,newVlan);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d vtt  failed,error code %u.\r\n",portId,retVal);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortVttDel(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 oldVlan,newVlan;
	UINT32 portId;
    UINT32 vlanMode;

	if (NULL == pstEnv || NULL == psPara)
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

	oldVlan = psPara[0].u;
	newVlan = psPara[1].u;
#ifdef CLI_CMD_PORT_DEBUG
//	vosPrintf(pstEnv->nWriteFd, "\r\noldvlan = %d,newvlan = %d.\r\n",oldVlan,newVlan);
#endif
	for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
		    retVal = odmPortVlanModeGet(portId,&vlanMode);
            if(OK != retVal)
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
                return retVal;
            }
            if(ODM_VLAN_TRANSLATION != vlanMode)
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nport%d is not in translation mode.please set to translation mode first.\r\n",portId);
                return VTT_VLAN_NOT_AT_TRANSLATION_MODE;
            }
			retVal = odmPortVttEntryDel(portId,0,oldVlan,0,newVlan);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nremove port %d vtt  failed.\r\n",portId);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortVlanTrunkAdd(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 trunkVlanId;
	UINT32 portId;
    UINT32 vlanMode;
	UINT32 defaultVlan;

	if (NULL == pstEnv || NULL == psPara)
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

	trunkVlanId = psPara[0].u;

	for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
		    retVal = odmPortVlanModeGet(portId,&vlanMode);
            if(OK != retVal)
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
                return retVal;
            }
            if(ODM_VLAN_TRUNK != vlanMode)
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nport%d is not in vlan trunk mode.please set to trunk mode first.\r\n",portId);
                return PORT_NOT_AT_VLAN_TRUNK_MODE;
            }
			
			/* judge if default vlan is equal oldvlan */
		    retVal = odmPortDefaultVlanGet(portId,&defaultVlan);
            if(OK != retVal)
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nget default vlan error[%u]\r\n",retVal);
                return retVal;
            }

			if (defaultVlan == trunkVlanId)
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nvlan can't equal default vlan\r\n");
                return ERROR;
            }
			
			retVal = odmPortVlanTrunkEntryAdd(portId, trunkVlanId);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d vlan trunk entry failed,error code %u.\r\n",portId,retVal);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortVlanTrunkDel(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
	UINT32 trunkVlanId;
	UINT32 portId;
    UINT32 vlanMode;

	if (NULL == pstEnv || NULL == psPara)
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

	trunkVlanId = psPara[0].u;

	for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
	{
		if(portArry[portId] == 1)
		{
		    retVal = odmPortVlanModeGet(portId,&vlanMode);
            if(OK != retVal)
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
                return retVal;
            }
            if(ODM_VLAN_TRUNK!= vlanMode)
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nport%d is not in vlan trunk mode.please set to trunk mode first.\r\n",portId);
                return PORT_NOT_AT_VLAN_TRUNK_MODE;
            }
			retVal = odmPortVlanTrunkEntryDel(portId, trunkVlanId);
			if(retVal != OK)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nset port %d vlan trunk entry failed,error code %u.\r\n",portId,retVal);
				return retVal;
			}
		}
	}
	return OK;
}

STATUS cliCmdPortListFdbMiscShow(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	
	UINT32 retVal = OK;

    if (NULL == pstEnv || NULL == psPara)
	{
		return ERROR;
	}

    switch(psPara[4].i)
    {
		case 1:
			cliCmdPortBindMacShow(pstEnv, psPara);
			break;
		case 2:
			cliCmdPortMacFilterShow(pstEnv, psPara);
			break;
		default:
            break;
    }

	return OK;
}

STATUS cliCmdPortListMiscShow(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;

    if (NULL == pstEnv || NULL == psPara)
	{
		return ERROR;
	}

    switch(psPara[3].i)
    {
      /* begin: add by xukang for bug 3265 */
        case 1:
            retVal = cliCmdPortAutonegStateShow(pstEnv,psPara);
	    break;
	  /* end: add by xukang for bug 3265 */
	    case 2:
	        cliShowStart(pstEnv);
            retVal = cliCmdPortParameterShow(pstEnv,psPara);
            cliShowEnd(pstEnv);
            break;
        case 3:
            cliShowStart(pstEnv);
            retVal = cliCmdPortMacLimitShow(pstEnv,psPara);
            cliShowEnd(pstEnv);
            break;
        case 4:
            cliShowStart(pstEnv);
            retVal = cliCmdPortRateLimitShow(pstEnv,psPara);
            cliShowEnd(pstEnv);
            break;
        case 5:
            cliShowStart(pstEnv);
            retVal = cliCmdPortVlanShow(pstEnv,psPara);
            cliShowEnd(pstEnv);
            break;
        case 6:
            cliShowStart(pstEnv);
            retVal = cliCmdPortVttShow(pstEnv,psPara);
            cliShowEnd(pstEnv);
            break;
		case 7:
			cliShowStart(pstEnv);
			retVal = cliCmdPortVlanTrunkShow(pstEnv, psPara);
			cliShowEnd(pstEnv);
			break;
		case 8:
			cliShowStart(pstEnv);
			retVal = cliCmdPortListFdbMiscShow(pstEnv, psPara);
			cliShowEnd(pstEnv);
			break;
        default:
            break;
    }
    return OK;
}
/* begin: add by xukang for bug 3265 */
STATUS cliCmdPortAutonegStateShow(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    STATUS retVal = OK;
    UINT32 portArry[ODM_NUM_OF_PORTS + 1];
    UINT8  portListBuff[255];
    UINT32 portId;
	UINT32  linkState;
	UINT32 autoCompleted;
	UINT32 speedSelect;
	UINT32 duplex;
	UINT32 lbEnable;
	UINT8  *cliAutoCompleted[] = {"Failed","Completed","Disabled"};
	UINT8  *cliLinkStateString[] = {"Down","Up",NULL};
	UINT8  *cliDuplexString[] = {"Half","Full",NULL};
	UINT8  *cliLbEnable[] = {"Enable","Disable",NULL};
	UINT32 adminState;
	UINT32 autoenable;
	UINT32 state;
	UINT32 idx_AutoCompleted = 1;
	UINT32 idx_LinkStateString;
	UINT32 idx_DuplexString;
	UINT32 idx_LbEnable;
		
	if (NULL == pstEnv || NULL == psPara)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));


	switch(psPara[0].i)
    {
        case 1:
            vosStrCpy(portListBuff,psPara[2].p);
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
            break;
    }

    
    vosPrintf(pstEnv->nWriteFd, "%-9s %-9s %-9s %-9s %-9s %-9s\r\n",
        "PortId",
        "LinkState",
        "Auto",
        "Speed",
        "Duplex",
        "Loopback");
	
    vosPrintf(pstEnv->nWriteFd, "--------- --------- --------- --------- --------- ---------\r\n");
	
	for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
	{

          if(1 != portArry[portId])
          {
               continue;
          } 


          retVal = odmPortLinkStateGet(portId, &linkState);
          if(OK != retVal)
          {
		  vosPrintf(pstEnv->nWriteFd, "\r\nget port%d admin status failde,errocode %u.\r\n",portId,retVal);
		  return ERROR;
	      }
          

          retVal = dalPhyAutonegAdminStateGet(portId, &adminState);
	      if(OK != retVal)
          {
		  vosPrintf(pstEnv->nWriteFd, "\r\nget port%d admin status failde,errocode %u.\r\n",portId,retVal);
		  return ERROR;
	      }
		  odmPortAutoEnableGet(portId, &autoenable);
	      if((autoenable) && (2 == adminState))
	      {		    
		    if (linkState)
		    {
		 	      dalPhySpecificAutonegRead(portId, &state);
			      switch(state)
			      {
			      	  case 0:
					      idx_AutoCompleted=0;
					      break;
				      case 1:
					      idx_AutoCompleted=1;
					      break;
                      default:
					      return ERROR;
                          break;			 	
			       }
		     }
			 else
			 {
			     idx_AutoCompleted = 0;   
			 }
	      }
	      else 
	      {
	   	           idx_AutoCompleted=2;
	      }
            
		  if((autoenable)&&((!linkState) || (1 == adminState)))
          {
              speedSelect = ODM_PORT_SPEED_10M;
			  idx_DuplexString=0;
		  }
		  else
		  {
		      retVal = dalPortSpecificSpeedRead(portId,&speedSelect);
              if(OK != retVal)
              {
		         vosPrintf(pstEnv->nWriteFd, "\r\nget port%d speed status failed,errocode %u.\r\n",portId,retVal);
		         return ERROR;
	          }
		
		      retVal = dalPortSpecificDuplexRead(portId,&duplex);
		      if(OK != retVal)
              {
		         vosPrintf(pstEnv->nWriteFd, "\r\nget port%d duplex status failed,errocode %u.\r\n",portId,retVal);
		         return ERROR;
	          }
              else
	          {
                  switch(duplex)
			      {
			      	 case ODM_PORT_DUPLEX_HALF:
					     idx_DuplexString=0;
					     break;
				     case ODM_PORT_DUPLEX_FULL:
					     idx_DuplexString=1;
					     break;
                     default:
					     return ERROR;
                         break;			 	
			      }
			  }
		  }

		 #ifndef ONU_1PORT	
		 if(1==rstpwarningstate[portId-1])
		     idx_LbEnable=0;
		 else 
		     idx_LbEnable=1;
		 #endif
		     
		 vosPrintf(pstEnv->nWriteFd, "%-9d %-9s %-9s %-9d %-9s %-9s\r\n",
        	portId,
        	cliLinkStateString[linkState],
        	cliAutoCompleted[idx_AutoCompleted],
        	speedSelect,
        	cliDuplexString[idx_DuplexString],
        	cliLbEnable[idx_LbEnable]);
	}

	return OK;
	 
}
	
/* end: add by xukang for bug 3265 */

STATUS cliCmdPortParameterShow(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
    UINT8 portListBuff[255];
    UINT32 portId;
    UINT32 adminEnable;
    UINT32 autoEnable;
    UINT32 speedSelect;
    UINT32 duplexSelect;
    UINT32 flowCtrolEnable;
    UINT32 tagStripEn;
    UINT32 maxGroupNum;
    UINT32 learnEnable;
	UINT32 lbEnable;
    UINT8 *cliStatusString[] = {"Disable","Enable",NULL};
    UINT8 *cliOperStateString[] = {"Down","Up",NULL};
    UINT8 *cliDuplexString[] = {NULL,"Half","Full"};
	UINT8 *ucondition = {"N/A"};
    UINT32 operateState;

	if (NULL == pstEnv || NULL == psPara)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));

    switch(psPara[0].i)
    {
        case 1:
           vosStrCpy(portListBuff,psPara[2].p);
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

    vosPrintf(pstEnv->nWriteFd, "%-7s %-7s %-7s %-7s %-7s %-7s %-9s %8s %8s\r\n",
        "PortId",
        "Admin",
        "Operate",
        "Auto",
        "Speed",
        "Duplex",
        "FlowCtrl",
        "Learning",
        "Loopback");
    vosPrintf(pstEnv->nWriteFd, "------- ------- ------- ------- ------- ------- --------- -------- --------\r\n");

	for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
	{
	    if(1 != portArry[portId])
        {
            continue;
        }
        retVal = odmPortAdminGet(portId, &adminEnable);
        if(OK != retVal)
        {
		    vosPrintf(pstEnv->nWriteFd, "\r\nget port%d admin status failde,errocode %u.\r\n",portId,retVal);
		    return ERROR;
	    }
        retVal = odmPortLinkStateGet(portId, &operateState);
        if(OK != retVal)
        {
		    vosPrintf(pstEnv->nWriteFd, "\r\nget port%d admin status failde,errocode %u.\r\n",portId,retVal);
		    return ERROR;
	    }
        retVal = odmPortAutoEnableGet(portId, &autoEnable);
        if(OK != retVal)
        {
		    vosPrintf(pstEnv->nWriteFd, "\r\nget port%d autonegotiation mode failde,errocode %u.\r\n",portId,retVal);
		    return ERROR;
	    }
        retVal = odmPortSpeedGet(portId, &speedSelect);
        if(OK != retVal)
        {
		    vosPrintf(pstEnv->nWriteFd, "\r\nget port%d speed mode failde,errocode %u.\r\n",portId,retVal);
		    return ERROR;
	    }
        retVal = odmPortDuplexGet(portId, &duplexSelect);
        if(OK != retVal)
        {
		    vosPrintf(pstEnv->nWriteFd, "\r\nget port%d duplex mode failde,errocode %u.\r\n",portId,retVal);
		    return ERROR;
	    }
        retVal = odmPortFlowCtrolEnableGet(portId, &flowCtrolEnable);
        if(OK != retVal)
        {
		    vosPrintf(pstEnv->nWriteFd, "\r\nget port%d flowctrol status failde,errocode %u.\r\n",portId,retVal);
		    return ERROR;
	    }

        retVal = odmPortLearningEnableGet(portId,&learnEnable);
        if(OK != retVal)
        {
		    vosPrintf(pstEnv->nWriteFd, "\r\nget port%d flowctrol status failde,errocode %u.\r\n",portId,retVal);
		    return ERROR;
	    }

		retVal = odmPortLoopbackEnableGet(portId,&lbEnable);
        if(OK != retVal)
        {
		    vosPrintf(pstEnv->nWriteFd, "\r\nget port%d loop back failde,errocode %u.\r\n",portId,retVal);
		    return ERROR;
	    }

	 	if(autoEnable)
		{
			vosPrintf(pstEnv->nWriteFd, "%-7d %-7s %-7s %-7s %-7s %-7s %-9s %-8s %-8s\r\n",
        	portId,
        	cliStatusString[adminEnable],
        	cliOperStateString[operateState],
        	cliStatusString[autoEnable],
        	"N/A",
        	"N/A",
        	cliStatusString[flowCtrolEnable],
        	cliStatusString[learnEnable],
        	cliStatusString[lbEnable]);
		}else
		{
			vosPrintf(pstEnv->nWriteFd, "%-7d %-7s %-7s %-7s %-7d %-7s %-9s %-8s %-8s\r\n",
        	portId,
        	cliStatusString[adminEnable],
        	cliOperStateString[operateState],
        	cliStatusString[autoEnable],
        	speedSelect,
        	cliDuplexString[duplexSelect],
        	cliStatusString[flowCtrolEnable],
        	cliStatusString[learnEnable],
        	cliStatusString[lbEnable]);
		}

	}
	return OK;
}

STATUS cliCmdPortMacLimitShow(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
    UINT8 portListBuff[255];
    UINT32 portId;
    UINT32 limitEnable;
    UINT32 limitNum;
    UINT8  *statusString[] = {"Disable","Enable",NULL};

	if (NULL == pstEnv || NULL == psPara)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));

    switch(psPara[0].i)
    {
        case 1:
           vosStrCpy(portListBuff,psPara[2].p);
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

    vosPrintf(pstEnv->nWriteFd, "%-7s %-7s %-7s\r\n","PortId","State","MacNum");
    vosPrintf(pstEnv->nWriteFd, "------- ------- -------\r\n");

	for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
	{
	    if(1 != portArry[portId])
        {
            continue;
        }
	    retVal = odmPortMacLimitNumEnableGet(portId,&limitEnable,&limitNum);
        if(OK != retVal)
        {
		    vosPrintf(pstEnv->nWriteFd, "\r\nget port%d mac limit on off failed %u.\r\n",portId,retVal);
		    return ERROR;
	    }

        vosPrintf(pstEnv->nWriteFd, "%-7d %-7s %-7d\r\n",portId,statusString[limitEnable],limitNum);
	}
	return OK;
}

STATUS cliCmdPortVlanShow(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
    UINT8  portListBuff[255];
    UINT32 defaultVlan;
    UINT32 vlanMode;
    UINT32 portId;
    UINT8 *cliVlanModeString[] = {"Transparent","Tag","Translation","Aggregation", "Trunk"};
	
	if (NULL == pstEnv || NULL == psPara)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));

    switch(psPara[0].i)
    {
        case 1:
           vosStrCpy(portListBuff,psPara[2].p);
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

    vosPrintf(pstEnv->nWriteFd, "%-7s %-20s %-20s\r\n",
        "PortId",
        "VLANMode",
        "DefaultVLAN");
    vosPrintf(pstEnv->nWriteFd, "------- -------------------- --------------------\r\n");

	for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
	{
	    if(1 != portArry[portId])
        {
            continue;
        }
	    retVal = odmPortVlanModeGet(portId, &vlanMode);
        if(OK != retVal)
        {
		    vosPrintf(pstEnv->nWriteFd, "\r\nget port%d vlan mode failde,errocode %u.\r\n",portId,retVal);
		    return ERROR;
	    }
        retVal = odmPortDefaultVlanGet(portId, &defaultVlan);
        if(OK != retVal)
        {
		    vosPrintf(pstEnv->nWriteFd, "\r\nget port%d default vlan failde,errocode %u.\r\n",portId,retVal);
		    return ERROR;
	    }
        vosPrintf(pstEnv->nWriteFd, "%-7d %-20s %-20d\r\n",
        portId,
        cliVlanModeString[vlanMode],
        defaultVlan);
	}
	return OK;
}
STATUS cliCmdPortRateLimitShow(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
    UINT8 portListBuff[255];
    UINT32 portId;
    UINT32 usPolicingEnable;
    UINT32 usCir;
    UINT32 usCbs;
    UINT32 usEbs;
    UINT32 dsRateLimitEnable;
    UINT32 dsCir;
    UINT32 dsCbs;
    UINT8  *cliStatusString[] = {"Disable","Enable"};
	if (NULL == pstEnv || NULL == psPara)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));

    switch(psPara[0].i)
    {
        case 1:
           vosStrCpy(portListBuff,psPara[2].p);
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

    vosPrintf(pstEnv->nWriteFd, "\r\n[Upstream]\r\n%-7s %-12s %-12s %-12s %-12s\r\n",
        "PortId",
        "Policing",
        "CIR",
        "CBS",
        "EBS");
    vosPrintf(pstEnv->nWriteFd, "------- ------------ ------------ ------------ ------------\r\n");

	for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
	{
	    if(1 != portArry[portId])
        {
            continue;
        }
	    retVal = odmPortUsPolicingEnableGet(portId, &usPolicingEnable);
        if(OK != retVal)
        {
		    vosPrintf(pstEnv->nWriteFd, "\r\nget port%d ingress rate limit failde,errocode %u.\r\n",portId,retVal);
		    return ERROR;
	    }
        if(TRUE == usPolicingEnable)
        {
            retVal = odmPortUsCirGet(portId, &usCir);
            if(OK != retVal)
            {
		        vosPrintf(pstEnv->nWriteFd, "\r\nget port%d ingress cir failde,errocode %u.\r\n",portId,retVal);
		        return ERROR;
	        }
            retVal = odmPortUsCbsGet(portId, &usCbs);
            if(OK != retVal)
            {
		        vosPrintf(pstEnv->nWriteFd, "\r\nget port%d ingress cbs failde,errocode %u.\r\n",portId,retVal);
		        return ERROR;
	        }
            retVal = odmPortUsEbsGet(portId, &usEbs);
            if(OK != retVal)
            {
		        vosPrintf(pstEnv->nWriteFd, "\r\nget port%d ingress ebs failde,errocode %u.\r\n",portId,retVal);
		        return ERROR;
	        }
        }else
        {
            usCir = 0;
            usCbs = 0;
            usEbs = 0;
        }
        vosPrintf(pstEnv->nWriteFd, "%-7d %-12s %-12d %-12d %-12d\r\n",
        portId,
        cliStatusString[usPolicingEnable],
        usCir,
        usCbs,
        usEbs);
    }
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    vosPrintf(pstEnv->nWriteFd, "\r\n[Downstream]\r\n%-7s %-14s %-12s\r\n",
        "PortId",
        "RateLimit",
        "CIR");
    vosPrintf(pstEnv->nWriteFd, "------- -------------- ------------\r\n");
#else
    vosPrintf(pstEnv->nWriteFd, "\r\n[Downstream]\r\n%-7s %-14s %-12s %-12s\r\n",
        "PortId",
        "RateLimit",
        "CIR",
        "CBS");
    vosPrintf(pstEnv->nWriteFd, "------- -------------- ------------ ------------\r\n");
#endif
	for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
	{
	    if(1 != portArry[portId])
        {
            continue;
        }
	    retVal = odmPortDsRateLimitEnableGet(portId, &dsRateLimitEnable);
        if(OK != retVal)
        {
		    vosPrintf(pstEnv->nWriteFd, "\r\nget port%d ingress rate limit failde,errocode %u.\r\n",portId,retVal);
		    return ERROR;
	    }
        if(TRUE == dsRateLimitEnable)
        {
            retVal = odmPortDsCirGet(portId, &dsCir);
            if(OK != retVal)
            {
		        vosPrintf(pstEnv->nWriteFd, "\r\nget port%d ingress cir failde,errocode %u.\r\n",portId,retVal);
		        return ERROR;
	        }
            retVal = odmPortDsCbsGet(portId, &dsCbs);
            if(OK != retVal)
            {
		        vosPrintf(pstEnv->nWriteFd, "\r\nget port%d ingress cbs failde,errocode %u.\r\n",portId,retVal);
		        return ERROR;
	        }
        }else
        {
            dsCir = 0;
            dsCbs = 0;
        }
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
        vosPrintf(pstEnv->nWriteFd, "%-7d %-14s %-12d\r\n",
        portId,
        cliStatusString[dsRateLimitEnable],
        dsCir);
#else
		vosPrintf(pstEnv->nWriteFd, "%-7d %-14s %-12d %-12d\r\n",
        portId,
        cliStatusString[dsRateLimitEnable],
        dsCir,
        dsCbs);
#endif
    }
	return OK;
}
STATUS cliCmdPortVttShow(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
    UINT8 portListBuff[255];
	UINT32 enable;
	UINT32 portId;
    UINT32 vttRecId;
    UINT32 oldTpid,oldVlan;
    UINT32 newTpid,newVlan;
    UINT32 valid;
    UINT32 vlanMode;
	if (NULL == pstEnv || NULL == psPara)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));

    switch(psPara[0].i)
    {
        case 1:
           vosStrCpy(portListBuff,psPara[2].p);
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

	for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
	{
	    if(1 != portArry[portId])
        {
            continue;
        }
	    vosPrintf(pstEnv->nWriteFd, "Port%02d %-10s %-10s %-10s\r\n",portId,"ID","C-VLAN","S-VLAN");
        vosPrintf(pstEnv->nWriteFd, "------ ---------- ---------- ----------\r\n");
	    for(vttRecId = 0; vttRecId < ODM_NUM_OF_VTT; vttRecId++)
        {
            retVal = odmPortVlanModeGet(portId,&vlanMode);
            if(OK != retVal)
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
                return retVal;
            }
            if(ODM_VLAN_TRANSLATION != vlanMode)
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nport%d is not in translation mode.please set to translation mode first.\r\n",portId);
                break;
            }
            retVal = odmPortVttEntryGet(portId,vttRecId,&valid,&oldTpid,&oldVlan,&newTpid,&newVlan);
            if(OK != retVal)
            {
                return retVal;
            }
            if(TRUE == valid)
            {
                vosPrintf(pstEnv->nWriteFd, "%-6s %-10d %-10d %-10d\r\n"," ",vttRecId,oldVlan,newVlan);
            }
        }
	}
	return OK;
}

STATUS cliCmdPortVlanTrunkShow(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 portArry[ODM_NUM_OF_PORTS + 1];
    UINT8 portListBuff[255];
	UINT32 portId;
    UINT32 vlanMode;
	UINT32 trunkEntryNum;
	UINT32 trunkVlanId;
	UINT32 i;
	
	if (NULL == pstEnv || NULL == psPara)
	{
		return ERROR;
	}
	vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));

    switch(psPara[0].i)
    {
        case 1:
           vosStrCpy(portListBuff,psPara[2].p);
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

	for(portId = ODM_START_PORT_NUN; portId <= ODM_NUM_OF_PORTS; portId++)
	{
	    if(1 != portArry[portId])
        {
            continue;
        }
	    vosPrintf(pstEnv->nWriteFd, "Port%02d %-10s\r\n",portId,"Allowded VLAN");
        vosPrintf(pstEnv->nWriteFd, "------ --------------\r\n");

		retVal = odmPortVlanModeGet(portId,&vlanMode);
		if(OK != retVal)
		{
			vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
			return retVal;
		}
		
		if(ODM_VLAN_TRUNK != vlanMode)
		{
			vosPrintf(pstEnv->nWriteFd, "\r\nport%d is not in trunk mode.please set to trunk mode first.\r\n",portId);
			continue;
		}

		retVal = odmPortVlanTrunkEntryNumGet(portId, &trunkEntryNum);
		if(retVal != OK)
		{
			vosPrintf(pstEnv->nWriteFd,"fail to get vlan entry num\n");
			return retVal;
		}

		for(i = 0; i < ODM_MAX_NUM_OF_TRUNK_VLAN; i++)
		{
			retVal = odmPortVlanTrunkEntryValueGet(portId, i, &trunkVlanId);
			if(retVal != OK)
			{
				continue;	
			}
			vosPrintf(pstEnv->nWriteFd, "%-6s %-10d\r\n"," ",trunkVlanId);
		}
	}
	return OK;
}

STATUS cliCmdMirrorGroupMode(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    if (NULL == pstPt->p)
    {
        return ERROR;
    }

    /* you must check if the pstEnv->para is freed */
    if (NULL != pstEnv->para)
    {
        vosFree(pstEnv->para);
        pstEnv->para = NULL;
    }
    ret = odmMirrorGroupNameSet(pstPt->p);
    if (ret != OK) 
    {
        vosPrintf(pstEnv->nWriteFd, "%%error: support only one mirror group, there is one mirror group that has been existed!\r\n");
        return ERROR;
    }
    pstEnv->para = (void *)vosStrDup(pstPt->p);
    pstEnv->pmode = CLI_MODE_CONFIG_MIRROR;  
    vosPrintf(pstEnv->nWriteFd, "Enter Mirror configuration mode. \r\n");
    return OK;
}

STATUS cliCmdMirrorPortApply(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    UINT8  portnum;
    UINT8  type;

    char   portbuf[255];
    UINT32 portArry[ODM_NUM_OF_PORTS + 1];
    UINT32 monitorPortArry[ODM_NUM_OF_PORTS + 1];

    if ((NULL == pstEnv) || (NULL == pstPt))
    {
        return ERROR;
    }

    vosMemSet(portArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
    vosStrCpy(portbuf,pstPt[0].p);
    ret = convertPortListString2PortArry(portbuf, portArry, ODM_NUM_OF_PORTS+1);
    if(-1 == ret)
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",portbuf);
        return ERROR;
    }
    if(portArry[0] == 1)
    {
	 vosPrintf(pstEnv->nWriteFd, "\r\nport num shuld start at 1.\r\n");
	 return ERROR;
    }

    vosMemSet(monitorPortArry,0x00,4*(ODM_NUM_OF_PORTS + 1));
    vosStrCpy(portbuf,pstPt[2].p);
    ret = convertPortListString2PortArry(portbuf, monitorPortArry, ODM_NUM_OF_PORTS+1);
    if(-1 == ret)
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",portbuf);
        return ERROR;
    }
    if(portArry[0] == 1)
    {
	 vosPrintf(pstEnv->nWriteFd, "\r\nport num shuld start at 1.\r\n");
	 return ERROR;
    }

    ret = odmMirrorGroupCfgDel(pstEnv->para);
    if (OK != ret)
    {
    	vosPrintf(pstEnv->nWriteFd,"%%delete port mirror cfg failed:! \r\n");
    	return ERROR;
    }    
	
    //set mirror port
    for(portnum = ODM_START_PORT_NUN; portnum <= ODM_NUM_OF_PORTS; portnum++)
    {
        if(1 != portArry[portnum])
        {
            continue;
        }
        if (pstPt[1].i == 1) type = ODM_MIRROR_BOTH;
        else if (pstPt[1].i == 2) type = ODM_MIRROR_INGRESS;
        else if (pstPt[1].i == 3) type = ODM_MIRROR_EGRESS;
        else return ERROR;
        if (odmMirrorPortSet(pstEnv->para,portnum,type,1) != NO_ERROR)
        {
            vosPrintf(pstEnv->nWriteFd,"%%Add mirroring port failed:! \r\n");
            return ERROR;
        }
    }

    //set monitor port
    for(portnum = ODM_START_PORT_NUN; portnum <= ODM_NUM_OF_PORTS; portnum++)
    {
        if(1 != monitorPortArry[portnum])
        {
            continue;
        }
        if (odmMonitorPortSet(pstEnv->para, portnum, 1) != NO_ERROR)
        {
            vosPrintf(pstEnv->nWriteFd,"%%Add monitor port failed:! \r\n");
            return ERROR;
        }
    }
    return OK;
}


STATUS cliCmdMirrorClear(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int    iRet;
    if (NULL == pstEnv)
    {
        return ERROR;
    }
	
    iRet = odmMirrorGroupClear(pstEnv->para);
    if (OK != iRet)
    {
    	vosPrintf(pstEnv->nWriteFd,"%%delete monitor port failed:! \r\n");
    	return ERROR;
    }    
    pstEnv->pmode = CLI_MODE_ENABLE;  
    vosPrintf(pstEnv->nWriteFd, "Exit Mirror configuration mode. \r\n");
    return OK;
}


STATUS cliCmdShowPortMirrorAll(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    
    if (NULL == pstEnv)
    {
        return ERROR;
    }
  
    odmShowMirrorList(pstEnv->nWriteFd);
    return NO_ERROR;
}

