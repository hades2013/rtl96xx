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
* FILENAME:  cli_cmd_rstp.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_cmd_rstp.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "cli.h"
#include "cli_cmd_list.h"
#include "odm_port.h"
#include "uid_stp.h"
#include "rstp_in.h"
//wfxu #include "dal_rstp.h"
#include "odm_rstp.h"
#include "opconn_usr_ioctrl.h"
//#include "opl_debug.h"

STATUS cliCmdRstpPort(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    cliShowStart(pstEnv);
	vosPrintf(pstEnv->nWriteFd, "rstp bridge info:\r\n");
	cli_br_get_cfg(pstEnv->nWriteFd);
	vosPrintf(pstEnv->nWriteFd, "\r\nrstp port info:\r\n");
	show_rstp_port(pstEnv->nWriteFd, 1);
	//rstp_conf_show(pstEnv->nWriteFd);
    cliShowEnd(pstEnv);
	return 0;
}
/* begin: add by jiangmingli for RSTP process */
STATUS cliCmdRstpInfoShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	if (NULL == pstEnv)
	{
		return ERR_NULL_POINTER;
	}

	cliShowStart(pstEnv);
	odmRstpInfoShow(pstEnv->nWriteFd);
	cliShowEnd(pstEnv);
	
	return 0;
}

STATUS cliCmdRstpCfgShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int i;

	if (NULL == pstEnv)
	{
		return ERR_NULL_POINTER;
	}

	cliShowStart(pstEnv);
	odmRstpInfoShow(pstEnv->nWriteFd);
	odmRstpConfShow(pstEnv->nWriteFd);

    for (i = 1; i <= ODM_NUM_OF_PORTS; i++)
	{
		vosPrintf(pstEnv->nWriteFd, "\r\nLoop detection config (Port %d)\r\n", i);
		vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", "Enable", bOnePortLoopDetectEn[i-1]?"Enabled":"Disabled");
		vosPrintf(pstEnv->nWriteFd, "    %-20s : %d\r\n", "Hold down time", bOnePortLoopWhile[i-1]);
        vosPrintf(pstEnv->nWriteFd, "    %-20s : %s\r\n", "Status", rstpwarningstate[i-1]?"Looped":"Normal");
        vosPrintf(pstEnv->nWriteFd, "    %-20s : %d\r\n", "Duration", bOnePortLoopWhile[i-1] - bOnePortLooptimer[i-1]);
	}
    
	cliShowEnd(pstEnv);
	
	return 0;
}
/* end: add by jiangmingli for RSTP process */

/* begin added by jiangmingli for tw_bug 2066 */
STATUS cliCmdRstpLoopDetect(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
  STATUS iRet;
  int ret;
  int i;
  UINT32 port_list_buff[128];
  UINT32 port_array[ODM_NUM_OF_PORTS+1];
	
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}
    
  for (i=0; i<(ODM_NUM_OF_PORTS+1); i++)
  {
    port_array[i] = 0;
  }
  if (pstPt[0].i == 2)
  {
    vosStrCpy(port_list_buff, pstPt[2].p);
    if((ret = convertPortListString2PortArry(port_list_buff, port_array, ODM_NUM_OF_PORTS+1)) == -1)
    {
      vosPrintf(pstEnv->nWriteFd, "\r\nPort list:%s, error.\r\n", port_list_buff);
      return ERROR;
    }
  }
  
  if (pstPt[0].i == 1)
  {
    for (i = 1; i < ODM_NUM_OF_PORTS+1; i++)
    {
      port_array[i] = 1;
    }
  }

  if (1 == pstPt[3].i)
  {
    for (i=1; i < ODM_NUM_OF_PORTS+1; i++)
    {
      if (port_array[i] == 1)
      {
        odmLoopDetectionSet(i, FALSE);
        odmLoopDetectionHoldDownTimeSet(i, 0);
      }
    }
  }
  else if (2 == pstPt[3].i)
  {
    for (i=1; i < ODM_NUM_OF_PORTS+1; i++)
    {
      if (port_array[i] == 1)
      {
        odmLoopDetectionSet(i, TRUE);
        odmLoopDetectionHoldDownTimeSet(i, pstPt[4].i);
      }
    }
  }
  else
  {
    return ERR_INVALID_PARAMETERS;
  }
  
	return NO_ERROR;
}
/* end added by jiangmingli for tw_bug 2066 */



STATUS cliCmdRstpEnable(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	STATUS iRet;
	
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	switch (pstPt[0].i)
	{
		case 1:
		{
			/* all ports disable rstp */
			odmRstpPortEnable(0x0000000F, OPL_FALSE);

			/* bridge disable rstp */
			iRet = odmRstpEnable(OPL_FALSE);
			if (NO_ERROR != iRet)
			{
				OPL_LOG_TRACE();
				return iRet;
			}
			break;
		}
		case 2:
		{
			/* bridge enable rstp */
			iRet = odmRstpEnable(OPL_TRUE);
			if (NO_ERROR != iRet)
			{
				OPL_LOG_TRACE();
				return iRet;
			}
			break;
		}
		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}
	
	return NO_ERROR;
}

#if 0
STATUS cliCmdRstpPortEnable(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	unsigned int iPortId;
	int iEnable;
	STATUS iRet;
	
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	switch (pstPt[0].i)
	{
		case 1:
		{
			iPortId = pstPt[1].u;
			if (iPortId > ODM_NUM_OF_PORTS)
			{
				return ERR_INVALID_PARAMETERS;
			}
			break;
		}
		case 2:
		{
			iPortId = 0;
			break;
		}
		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}

	switch (pstPt[2].i)
	{
		case 1:
		{
			iEnable = OPL_FALSE;
			break;
		}
		case 2:
		{
			iEnable = OPL_TRUE;
			break;
		}
		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}
	
	iRet = rstp_port_enable(iPortId, iEnable);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
		
	return NO_ERROR;
}
STATUS cliCmdRstpPortEdge(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	unsigned int iPortId;
	int iEnable;
	STATUS iRet;
	
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	switch (pstPt[0].i)
	{
		case 1:
		{
			iPortId = pstPt[1].u;
			if (iPortId > ODM_NUM_OF_PORTS)
			{
				return ERR_INVALID_PARAMETERS;
			}
			break;
		}
		case 2:
		{
			iPortId = 0;
			break;
		}
		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}

	switch (pstPt[2].i)
	{
		case 1:
		{
			iEnable = OPL_FALSE;
			break;
		}
		case 2:
		{
			iEnable = OPL_TRUE;
			break;
		}
		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}
	
	iRet = rstp_edgeport_enable(iPortId, iEnable);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
	
	return NO_ERROR;
}
STATUS cliCmdRstpPortP2plink(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	unsigned int iPortId;
	int iP2pMode;
	STATUS iRet;
	
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	switch (pstPt[0].i)
	{
		case 1:
		{
			iPortId = pstPt[1].u;
			if (iPortId > ODM_NUM_OF_PORTS)
			{
				return ERR_INVALID_PARAMETERS;
			}
			break;
		}
		case 2:
		{
			iPortId = 0;
			break;
		}
		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}

	switch (pstPt[2].i)
	{
		case 1:
		{
			iP2pMode = P2P_FORCE_TRUE;
			break;
		}
		case 2:
		{
			iP2pMode = P2P_FORCE_FALSE;
			break;
		}
		case 3:
		{
			iP2pMode = P2P_AUTO;
			break;
		}
		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}
	
	iRet = rstp_p2plink_mode(iPortId, iP2pMode);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
	
	return NO_ERROR;
}
STATUS cliCmdRstpPortPriority(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	unsigned int iPortId;
	unsigned int iPri;
	STATUS iRet;
	
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	switch (pstPt[0].i)
	{
		case 1:
		{
			iPortId = pstPt[1].u;
			if (iPortId > ODM_NUM_OF_PORTS)
			{
				return ERR_INVALID_PARAMETERS;
			}
			break;
		}
		case 2:
		{
			iPortId = 0;
			break;
		}
		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}
	
	iPri = pstPt[2].u;

	/* check port priority */
	if (iPri > 240)
	{
		return ERR_INVALID_PARAMETERS;
	}

	/* step is 16 */
	iPri = (iPri / 16) * 16;
	
	iRet = rstp_set_portpriority(iPortId, iPri);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
	
	return NO_ERROR;
}
STATUS cliCmdRstpPortPathcost(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	unsigned int iPortId;
	unsigned int iPcost;
	STATUS iRet;
	
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	switch (pstPt[0].i)
	{
		case 1:
		{
			iPortId = pstPt[1].u;
			if (iPortId > ODM_NUM_OF_PORTS)
			{
				return ERR_INVALID_PARAMETERS;
			}
			break;
		}
		case 2:
		{
			iPortId = 0;
			break;
		}
		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}
	
	iPcost = pstPt[2].u;

	/* check port priority */
	if ((iPcost < 1) || (iPcost > 20000000))
	{
		return ERR_INVALID_PARAMETERS;
	}

	iRet = rstp_set_pathcost(iPortId, iPcost);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
	
	return NO_ERROR;
}
#endif

STATUS cliCmdRstpSetHello(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	unsigned int iHelloTime;
	unsigned int ulCfgMaxAge;
	STATUS iRet;
	
    if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	iHelloTime = pstPt[0].u;

	/* check hellotime */
	if ((iHelloTime < 1) || (iHelloTime > 10))
	{
		return ERR_INVALID_PARAMETERS;
	}

	/* check if MaxAge >=  2 * (HelloTime + 1) */
	ulCfgMaxAge = vosConfigUInt32Get(CFGFILE_RSTP,
											    RSTP_SECTION_BRD,
											    RSTP_BRD_KEY_U32_MAXAGE,
											    RSTP_BRD_DEFAULT_MAXAGE);
	if (ulCfgMaxAge < 2 * (iHelloTime + 1))
	{
		vosPrintf(pstEnv->nWriteFd,
					  "MaxAge<2*(HelloTime+1.0 seconds)\r\nPlease check the input HelloTime value\r\n");
		return ERR_INVALID_PARAMETERS;
	}

	/* set hello time */	
	iRet = odmRstpSetHello(iHelloTime);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
	
	return NO_ERROR;
}

STATUS cliCmdRstpSetForward(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	unsigned int iFwdTime;
	unsigned int ulCfgMaxAge;
	STATUS iRet;
	
    if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	iFwdTime = pstPt[0].u;

	/* check forward delay time */
	if ((iFwdTime < 4) || (iFwdTime > 30))
	{
		return ERR_INVALID_PARAMETERS;
	}

	/* check if 2 * (ForwardDelay - 1) >= MaxAge */
	ulCfgMaxAge = vosConfigUInt32Get(CFGFILE_RSTP,
											    RSTP_SECTION_BRD,
											    RSTP_BRD_KEY_U32_MAXAGE,
											    RSTP_BRD_DEFAULT_MAXAGE);
	if (2 * (iFwdTime - 1) < ulCfgMaxAge)
	{
		vosPrintf(pstEnv->nWriteFd,
					  "2*(ForwardDelay-1.0 seconds)<MaxAge)\r\nPlease check the input ForwardDelay value\r\n");
		return ERR_INVALID_PARAMETERS;
	}

	/* set forward delay time */
	iRet = odmRstpSetForward(iFwdTime);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
	
	return NO_ERROR;
}

STATUS cliCmdRstpSetMaxage(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	unsigned int iMaxAge;
	unsigned int ulCfgHelloTime;
	unsigned int ulCfgFwdDelay;
	STATUS iRet;
	
    if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	iMaxAge = pstPt[0].u;

	/* check max age */
	if ((iMaxAge < 6) || (iMaxAge > 40))
	{
		return ERR_INVALID_PARAMETERS;
	}

	/* check if MaxAge >=  2 * (HelloTime + 1) */
	ulCfgHelloTime = vosConfigUInt32Get(CFGFILE_RSTP,
											       RSTP_SECTION_BRD,
											       RSTP_BRD_KEY_U32_HELLOTIME,
											       RSTP_BRD_DEFAULT_HELLOTIME);
	if (iMaxAge < 2 * (ulCfgHelloTime + 1))
	{
		vosPrintf(pstEnv->nWriteFd,
					  "MaxAge<2*(HelloTime+1.0 seconds)\r\nPlease check the input iMaxAge value\r\n");
		return ERR_INVALID_PARAMETERS;
	}

	/* check if 2 * (ForwardDelay - 1) >= MaxAge */
	ulCfgFwdDelay = vosConfigUInt32Get(CFGFILE_RSTP,
											      RSTP_SECTION_BRD,
											      RSTP_BRD_KEY_U32_FWDDELAY,
											      RSTP_BRD_DEFAULT_FWDDELAYTIME);
	if (2 * (ulCfgFwdDelay - 1) < iMaxAge)
	{
		vosPrintf(pstEnv->nWriteFd,
					  "2*(ForwardDelay-1.0 seconds)<MaxAge)\r\nPlease check the input iMaxAge value\r\n");
		return ERR_INVALID_PARAMETERS;
	}

	/* set max age */	
	iRet = odmRstpSetMaxage(iMaxAge);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
	
	return NO_ERROR;
}

STATUS cliCmdRstpSetForver(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    unsigned int iForceVersion;
	STATUS iRet;
	
    if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	switch (pstPt[0].i)
	{
		case 1:
		{
			iForceVersion = 0;
			break;
		}
		case 2:
		{
			iForceVersion = 2;
			break;
		}
		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}
	
	iRet = odmRstpSetForver(iForceVersion);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
	
	return NO_ERROR;
}

STATUS cliCmdRstpSetPriority(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    unsigned int iBrdPri;
	STATUS iRet;
	
    if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	iBrdPri = pstPt[0].u;

	/* check bridge priority */
	if (iBrdPri > 15)
	{
		return ERR_INVALID_PARAMETERS;
	}
	
	iRet = odmRstpSetPriority(iBrdPri);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
	
	return NO_ERROR;
}

STATUS cliCmdRstpSetDiameter(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    unsigned int iDiameter;
	STATUS iRet;
	
    if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	iDiameter = pstPt[0].u;

	/* check bridge priority */
	if ((iDiameter < 2) || (iDiameter > 7))
	{
		return ERR_INVALID_PARAMETERS;
	}
	
	iRet = odmRstpSetDiameter(iDiameter);
	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}
	
	return NO_ERROR;
}

STATUS cliCmdRstpRecoverBrdSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	unsigned int uiRecoverPara;
	unsigned int ulCfgMaxAge;
	unsigned int ulCfgHelloTime;
	unsigned int ulCfgFwdDelay;
	STATUS iRet;
	
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	uiRecoverPara = pstPt[0].u;

	switch (uiRecoverPara)
	{
		case 1:
		{
			/* check if MaxAge >=  2 * (HelloTime + 1) */
			ulCfgMaxAge = vosConfigUInt32Get(CFGFILE_RSTP,
													    RSTP_SECTION_BRD,
													    RSTP_BRD_KEY_U32_MAXAGE,
													    RSTP_BRD_DEFAULT_MAXAGE);
			if (ulCfgMaxAge < 2 * (RSTP_BRD_DEFAULT_HELLOTIME + 1))
			{
				vosPrintf(pstEnv->nWriteFd,
							  "MaxAge<2*(HelloTime+1.0 seconds)\r\nPlease set MaxAge value first\r\n");
				return ERR_INVALID_PARAMETERS;
			}
			
			iRet = odmRstpSetHello(RSTP_BRD_DEFAULT_HELLOTIME);
			break;
		}

		case 2:
		{
			/* check if 2 * (ForwardDelay - 1) >= MaxAge */
			ulCfgMaxAge = vosConfigUInt32Get(CFGFILE_RSTP,
													    RSTP_SECTION_BRD,
													    RSTP_BRD_KEY_U32_MAXAGE,
													    RSTP_BRD_DEFAULT_MAXAGE);
			if (2 * (RSTP_BRD_DEFAULT_FWDDELAYTIME - 1) < ulCfgMaxAge)
			{
				vosPrintf(pstEnv->nWriteFd,
							  "2*(ForwardDelay-1.0 seconds)<MaxAge)\r\nPlease set MaxAge value first\r\n");
				return ERR_INVALID_PARAMETERS;
			}
	
			iRet = odmRstpSetForward(RSTP_BRD_DEFAULT_FWDDELAYTIME);
			break;
		}

		case 3:
		{
			
			/* check if MaxAge >=  2 * (HelloTime + 1) */
			ulCfgHelloTime = vosConfigUInt32Get(CFGFILE_RSTP,
														   RSTP_SECTION_BRD,
														   RSTP_BRD_KEY_U32_HELLOTIME,
														   RSTP_BRD_DEFAULT_HELLOTIME);
			if (RSTP_BRD_DEFAULT_MAXAGE < 2 * (ulCfgHelloTime + 1))
			{
				vosPrintf(pstEnv->nWriteFd,
							  "MaxAge<2*(HelloTime+1.0 seconds)\r\nPlease set HelloTime value first\r\n");
				return ERR_INVALID_PARAMETERS;
			}
		
			/* check if 2 * (ForwardDelay - 1) >= MaxAge */
			ulCfgFwdDelay = vosConfigUInt32Get(CFGFILE_RSTP,
														  RSTP_SECTION_BRD,
														  RSTP_BRD_KEY_U32_FWDDELAY,
														  RSTP_BRD_DEFAULT_FWDDELAYTIME);
			if (2 * (ulCfgFwdDelay - 1) < RSTP_BRD_DEFAULT_MAXAGE)
			{
				vosPrintf(pstEnv->nWriteFd,
							  "2*(ForwardDelay-1.0 seconds)<MaxAge)\r\nPlease set Forward-delay value first\r\n");
				return ERR_INVALID_PARAMETERS;
			}

			iRet = odmRstpSetMaxage(RSTP_BRD_DEFAULT_MAXAGE);
			break;
		}

		case 4:
		{
			iRet = odmRstpSetDiameter(RSTP_BRD_DEFAULT_DIAMETER);
			break;
		}

		case 5:
		{
			iRet = odmRstpSetPriority(RSTP_BRD_DEFAULT_PRI);
			break;
		}

		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}

	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}

	return NO_ERROR;
}

STATUS cliCmdRstpRecoverPortSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	unsigned int iPortId;
	unsigned int aulPortArry[ODM_NUM_OF_PORTS + 1];
	unsigned long ulPortBitmap = 0;
	unsigned int uiRecoverPara;
	STATUS iRet;
	
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	switch (pstPt[0].i)
	{
		case 1:
		{
			iRet = convertPortListString2PortArry(pstPt[2].p, aulPortArry, ODM_NUM_OF_PORTS+1);
			if(0 != iRet)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",pstPt[2].p);
				return iRet;
			}

			for (iPortId = ODM_START_PORT_NUN; iPortId <= ODM_NUM_OF_PORTS; iPortId++)
			{
				if (1 == aulPortArry[iPortId])
				{
					ulPortBitmap |= (1 << (iPortId - 1));
				}
			}
			break;
		}
		case 2:
		{
			ulPortBitmap = 0x0000000F;
			break;
		}
		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}

	uiRecoverPara = pstPt[3].u;

	switch (uiRecoverPara)
	{
		case 1:
		{
			iRet = odmRstpSetPathCost(ulPortBitmap, RSTP_PORT_DEFAULT_PATH_COST);
			break;
		}

		case 2:
		{
			iRet = odmRstpSetPortPriority(ulPortBitmap, RSTP_PORT_DEFAULT_PRI);
			break;
		}

		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}

	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}

	return NO_ERROR;
}

STATUS cliCmdRstpPortSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	unsigned int iPortId;
	unsigned int aulPortArry[ODM_NUM_OF_PORTS + 1];
	unsigned long ulPortBitmap = 0;
	unsigned int iPri;
	unsigned int iPcost;
	STATUS iRet;
	
	if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

	/* set port id */
	switch (pstPt[0].i)
	{
		case 1:
		{
			iRet = convertPortListString2PortArry(pstPt[2].p, aulPortArry, ODM_NUM_OF_PORTS+1);
			if(0 != iRet)
			{
				vosPrintf(pstEnv->nWriteFd, "\r\nport list:%s, error.\r\n",pstPt[2].p);
				return iRet;
			}

			for (iPortId = ODM_START_PORT_NUN; iPortId <= ODM_NUM_OF_PORTS; iPortId++)
			{
				if (1 == aulPortArry[iPortId])
				{
					ulPortBitmap |= (1 << (iPortId - 1));
				}
			}
			break;
		}
		case 2:
		{
			ulPortBitmap = 0x0000000F;
			break;
		}
		default:
		{
			return ERR_INVALID_PARAMETERS;
		}
	}

	switch (pstPt[3].i)
	{
		/* rstp port disable */
		case 1:
		{
			iRet = odmRstpPortEnable(ulPortBitmap, OPL_FALSE);
			break;
		}
		/* rstp port enable */
		case 2:
		{
			iRet = odmRstpPortEnable(ulPortBitmap, OPL_TRUE);
			if (RSTP_BRD_NOT_ENABLE_WARNING == iRet)
			{
				vosPrintf(pstEnv->nWriteFd, "rstp hasn't enabled yet, please enable rstp first!\r\n");
				return NO_ERROR;
			}
			break;
		}
		/* rstp port edge disable or enable */
		case 3:
		{
			/* auto edge port */
			if (1 == pstPt[5].i)
			{
				iRet= odmRstpEdgePortEnable(ulPortBitmap, OPL_FALSE);
			}
			/* edge port */
			else
			{
				iRet= odmRstpEdgePortEnable(ulPortBitmap, OPL_TRUE);
			}
			break;
		}
		/* rstp port point-to-point mode */
		case 4:
		{
			/* p2p force-true */
			if (1 == pstPt[7].i)
			{
				iRet = odmRstpP2plinkMode(ulPortBitmap, P2P_FORCE_TRUE);
			}
			/* p2p force-false */
			else if (2 == pstPt[7].i)
			{
				iRet = odmRstpP2plinkMode(ulPortBitmap, P2P_FORCE_FALSE);
			}
			/* p2p auto */
			else
			{
				iRet = odmRstpP2plinkMode(ulPortBitmap, P2P_AUTO);
			}
			break;
		}
		/* rstp port priority */
		case 5:
		{
			iPri = pstPt[9].u;

			/* check port priority */
			if (iPri > 15)
			{
				return ERR_INVALID_PARAMETERS;
			}
	
			iRet = odmRstpSetPortPriority(ulPortBitmap, iPri);
			break;
		}
		/* rstp port pathcost */
		case 6:
		{
			iPcost = pstPt[11].u;
			
			/* check port priority */
			if ((iPcost < 1) || (iPcost > 20000000))
			{
				return ERR_INVALID_PARAMETERS;
			}

			iRet = odmRstpSetPathCost(ulPortBitmap, iPcost);
			break;
		}
	}

	if (NO_ERROR != iRet)
	{
		OPL_LOG_TRACE();
		return iRet;
	}

	return NO_ERROR;
}

/* begin added by jiangmingli for zte */
STATUS cliCmdRstpHandleModeSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT32 ulMode;
    STATUS iRet;
    
    if ((NULL == pstEnv) || (NULL == pstPt))
	{
		return ERR_NULL_POINTER;
	}

    if (1 == pstPt[0].i)    /* to cpu */
    {
        ulMode = 0;
    }
    else if (2 == pstPt[0].i)   /* forward */
    {
        ulMode = 1;
    }
    else
    {
        return ERR_INVALID_PARAMETERS;
    }

    iRet = odmRstpPktHandleModeSet(ulMode);

    return iRet;    
}

STATUS cliCmdRstpHandleModeShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT32 ulMode;
    STATUS iRet;
    
    if (NULL == pstEnv)
	{
		return ERR_NULL_POINTER;
	}

     iRet = odmRstpPktHandleModeGet(&ulMode);

    if (0 == ulMode)
    {
        vosPrintf(pstEnv->nWriteFd, "Rstp handle mode is trap to cpu.\r\n");
    }
    else if (1 == ulMode)
    {
        vosPrintf(pstEnv->nWriteFd, "Rstp handle mode is transparent.\r\n");
    }
    else
    {
        vosPrintf(pstEnv->nWriteFd, "Unknown Rstp handle mode.\r\n");
    }

    return NO_ERROR;    
}
/* end added by jiangmingli for zte */
