/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*   used only with Opulan semi-conductor products. 
*
* FILENAME:  cli_cmd_enable.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_cmd_enable.c#1 $
* $Log:$
*
*
**************************************************************************/
#include <stdlib.h>
#include "cli.h"
#include "cli_cmd_list.h"
//wfxu #include <hw_interface.h>
#include <hal.h>
//wfxu #include "tm.h"

#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <stats.h>
#include "opconn_usr_ioctrl.h"
#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "sd.h"
#if defined(ONU_4PORT_AR8327)
#include "isis_reg_access.h"
#else
#include "shiva_reg_access.h"
#endif
#endif
#include "lw_config.h"

UINT8 cliToAtherosPortMap[] = {0,1,2,3,4,5,6,0,0,0,0};

STATUS cliCmdOrr(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_ORR_t stRegParam;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    stRegParam.ulRegAddr = pstPt[0].u;
    stRegParam.ulFd = pstEnv->nWriteFd;

    stApiData.apiId = HAL_API_GBL_ORR;
    stApiData.length = sizeof(HAL_API_GBL_ORR_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    return halAppApiCallSync(&stApiData);
}

STATUS cliCmdOwr(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_OWR_t stRegParam;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    stRegParam.ulRegAddr = pstPt[0].u;
    stRegParam.ulRegVal = pstPt[1].u;
    stRegParam.ulFd = pstEnv->nWriteFd;

    stApiData.apiId = HAL_API_GBL_OWR;
    stApiData.length = sizeof(HAL_API_GBL_OWR_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    return halAppApiCallSync(&stApiData);
}

STATUS cliCmdMrr(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal;
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_MRR_t stRegParam;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);

    stRegParam.ulRegAddr = pstPt[0].u;
    stRegParam.ulRegNum = pstPt[1].u;
    stRegParam.ulFd = pstEnv->nWriteFd;

    stApiData.apiId = HAL_API_GBL_MRR;
    stApiData.length = sizeof(HAL_API_GBL_MRR_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    retVal = halAppApiCallSync(&stApiData);

    cliShowEnd(pstEnv);

    return retVal;
}

STATUS cliCmdMwr(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal;
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_MWR_t stRegParam;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);

    stRegParam.ulRegAddr = pstPt[0].u;
    stRegParam.ulRegNum = pstPt[1].u;
    stRegParam.ulNewVal = pstPt[2].u;
    stRegParam.ulMode = pstPt[3].u;
    stRegParam.ulFd = pstEnv->nWriteFd;

    stApiData.apiId = HAL_API_GBL_MWR;
    stApiData.length = sizeof(HAL_API_GBL_MWR_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    retVal = halAppApiCallSync(&stApiData);

    cliShowEnd(pstEnv);
    return retVal;
}

STATUS cliCmdOrt(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal;
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_ORT_t stRegParam;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);

    stRegParam.ulRegion = pstPt[0].u;
    stRegParam.ulRecordStart = pstPt[1].u;
    stRegParam.ulRecordNum = pstPt[2].u;
    stRegParam.ulFd = pstEnv->nWriteFd;

    stApiData.apiId = HAL_API_GBL_ORT;
    stApiData.length = sizeof(HAL_API_GBL_ORT_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    retVal = halAppApiCallSync(&stApiData);

    cliShowEnd(pstEnv);
    return retVal;
}

STATUS cliCmdOwt(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_OWT_t stRegParam;
	
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

	if (FALSE == utilIsInt(pstPt[4].p))
	{
		vosPrintf(pstEnv->nWriteFd, "%%Invalid input detected. Hex or deciamal format is expected.\r\n"); 
		return ERROR;
	}

    stRegParam.ulRegion = pstPt[0].u;
    stRegParam.ulRecordStart = pstPt[1].u;
    stRegParam.ulRecordNum = pstPt[2].u;
    stRegParam.ulOffset = pstPt[3].u;
    stRegParam.ulValue = vosStrToUInt(pstPt[4].p, NULL);

    stApiData.apiId = HAL_API_GBL_OWT;
    stApiData.length = sizeof(HAL_API_GBL_OWT_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    return halAppApiCallSync(&stApiData);
}

STATUS cliCmdOst(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal;
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_OST_t stRegParam;
	
    if (NULL == pstEnv)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);

    stRegParam.ulFd = pstEnv->nWriteFd;

    stApiData.apiId = HAL_API_GBL_OST;
    stApiData.length = sizeof(HAL_API_GBL_OST_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    retVal = halAppApiCallSync(&stApiData);

    cliShowEnd(pstEnv);

    return retVal;
}

STATUS cliCmdBrgVttAdd(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
  
    if (pstPt[0].i >= 16)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }
    if (pstPt[1].i >= 2)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }
    if (pstPt[2].i >= 2)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }
    if (pstPt[3].i >= 4096)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }
    if (pstPt[4].i >= 4096)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }
   // return brgVttDirectHwWrite(pstPt[0].i,1,pstPt[1].i, pstPt[2].i, pstPt[3].i, pstPt[4].i);
}
STATUS cliCmdBrgVttDel(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (pstPt[0].i >= 16)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }
   // return brgVttDirectHwWrite(pstPt[0].i,0,0, 0,0, 0);
}
STATUS cliCmdBrgVttShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    //return brgVttEntryShow(pstPt[0].i,pstPt[1].i);
}
#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)

STATUS cliCmdVttShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    return dalArVttShow(pstPt[0].i);
}

STATUS cliCmdVlanShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    return dalVlanShow(pstPt[0].i);
}
#endif

STATUS cliCmdTmUsShaperSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	int ret = OK;
  
	if (pstPt[0].i >= 8)
	{
		vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
		return ERROR;
	}
	if(pstPt[1].i == 1)//enable cir
	{
		ret += tmQueueShaperCirEnableHwWrite(UP_STREAM,pstPt[0].i,OPL_TRUE);
	}
	if(pstPt[1].i == 2)//disable cir
	{
		ret += tmQueueShaperCirEnableHwWrite(UP_STREAM,pstPt[0].i,OPL_FALSE);
	}
	if(pstPt[5].i == 1)//enable pir
	{
		ret += tmQueueShaperPirEnableHwWrite(UP_STREAM,pstPt[0].i,OPL_TRUE);
	}
	if(pstPt[5].i == 2)//disable pir
	{
		ret += tmQueueShaperPirEnableHwWrite(UP_STREAM,pstPt[0].i,OPL_FALSE);
	}
	if (pstPt[2].i == 1)//config cir
	{
		ret += tmUsShaperTabCirDirectHwWrite(pstPt[0].i,pstPt[3].i,pstPt[4].i);
	}
	if (pstPt[6].i == 1)// config pir
	{
		ret += tmUsShaperTabPirDirectHwWrite(pstPt[0].i,pstPt[7].i,pstPt[8].i);
	}

	return ret;    


    
}
STATUS cliCmdTmDsShaperSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	int ret = OK;

	if (pstPt[0].i > 8)
	{
		vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
		return ERROR;
	}
	if(pstPt[1].i == 1)
	{
		ret += tmQueueShaperEnableHwWrite(DOWN_STREAM,pstPt[0].i,OPL_TRUE);
	}
	if(pstPt[1].i == 2)
	{
		ret += tmQueueShaperEnableHwWrite(DOWN_STREAM,pstPt[0].i,OPL_FALSE);
	}
	if (pstPt[2].i == 1)
	{
		ret += tmDsShaperTabDirectHwWrite(pstPt[0].i,pstPt[3].i,pstPt[4].i);
	}
        

    return ret;
}

STATUS cliCmdPortQueuePriWeightSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret = OK;
	UINT8 stream;

	if (IS_ONU_RESPIN)
	{
		if(pstPt[0].i == 1)//upstream
			{
				stream=0;
			}
		else if(pstPt[0].i == 2)//downstream
			{
				stream=1;
			}else
			{
				return OPL_ERROR;
			}	
		/*configure queue priority*/
	    ret += tmQueuePriHwWrite(stream,pstPt[1].i,pstPt[2].i);
		/*configure queue weight*/
	 	ret += tmQueueWeightHwWrite(stream,pstPt[1].i,pstPt[3].i);
	}
	else
	{
		vosPrintf(pstEnv->nWriteFd, "%% Not support.\r\n");
	}
    return ret;
}

STATUS cliCmdTmQueueInfoSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	UINT8 stream = 0;

    if (pstPt[0].i >= 16)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }

	if(pstPt[0].i < 8)
	{
		stream = 0;	
	}else
	{
		stream = 1;
	}

	tmQueueMaxCellNumHwWrite(stream,(pstPt[0].i%8),pstPt[1].i);
	
    tmQueueMaxPktNumHwWrite(stream,(pstPt[0].i%8),pstPt[2].i);
}
STATUS cliCmdWredSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (pstPt[0].i >= 2)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }
    if(pstPt[0].i == 0)
    {
        return tmUsWredTabDirectHwWrite(pstPt[1].i,pstPt[2].i,pstPt[3].i,pstPt[4].i);  
    }else
    {
        return tmDsWredTabDirectHwWrite(pstPt[1].i,pstPt[2].i,pstPt[3].i,pstPt[4].i);   
    }
    return OPL_ERROR;   
}


STATUS cliCmdTmTabShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
#if 0 /*wfxu 0403 will be done*/
    switch (pstPt[0].i)
    {
        case 1:
			cliTmCosIcosMapShow(pstEnv->nWriteFd);
			break;
		case 2:
			cliTmQidIcosMapShow(pstEnv->nWriteFd);
			break;
		case 3:	
			cliTmInQueueCounterShow(pstEnv->nWriteFd);
			break;
		case 4:	
			cliTmShaperConfigShow(pstEnv->nWriteFd);
			break;
		case 5:	
			cliTmInQueueConfigShow(pstEnv->nWriteFd);
			break;	
		case 6:
			cliTmWredConfigShow(pstEnv->nWriteFd);
			break;
		case 7:	
			cliTmQueueWeightShow(pstEnv->nWriteFd);
			break;
		case 8:	
			cliTmQueuePriorityShow(pstEnv->nWriteFd);
			break;
		case 9:	
			cliTmRptValueOfQueueShow(pstEnv->nWriteFd);
			break;
		case 10:	
			cliTmTotalPktsInQueueIncMpcpShow(pstEnv->nWriteFd);
			break;
        default:
            return ERROR;
    }
#endif	
    return OK;
}

STATUS cliCmdCounterShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int portId;
#if 0 /*wfxu 0403 will be done*/
    switch (pstPt[0].i)
    {
    	case 1:
			cliBrgGeParserCounterShow(pstEnv->nWriteFd);
			break;
		case 2:
			cliBrgPortCounterShow(pstEnv->nWriteFd);
			break;
		case 3:
			cliBrgCounterShow(pstEnv->nWriteFd);
			break;
		case 4:
			cliTmCounterShow(pstEnv->nWriteFd);
			break;
		case 5:
			cliPonMpcpCounterShow(pstEnv->nWriteFd);
			break;
		case 6:
			cliHostDmaCounterShow(pstEnv->nWriteFd);
			break;
		case 7: 	
			cliAllCounterShow(pstEnv->nWriteFd);
            vosPrintf(pstEnv->nWriteFd, "\r\n");
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)
            sampleGetRMONCounter3(dev);
#elif defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
            extern sw_error_t atheros_show_cnt(fal_port_t port_id);
            for (portId = 0; portId < 7; portId++)
            {
                atheros_show_cnt(portId);
            }
#endif


			break;
        case 8:
			cliIntStatusShow(pstEnv->nWriteFd);
			break;
		default:
			break;
    }
#endif
return OK;
}

STATUS cliCmdInterruptShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal;
    OPL_API_DATA_t stApiData;
    HAL_API_INT_COUNTER_SHOW_t stIntParam;

    if (NULL == pstEnv)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);

    stIntParam.ulFd = pstEnv->nWriteFd;

    stApiData.apiId = HAL_API_INT_COUNTER_SHOW;
    stApiData.length = sizeof(HAL_API_INT_COUNTER_SHOW_t);
    stApiData.param = &stIntParam;
    stApiData.eventFunc = NULL;
    
    retVal = halAppApiCallSync(&stApiData);

    cliShowEnd(pstEnv);

    return retVal;
}


STATUS cliCmdInterruptSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal = ERROR;
    OPL_API_DATA_t stApiData;
    HAL_API_INT_LEVEL1_ENABLE_t stIntLevel1Param;
    HAL_API_INT_LEVEL2_ENABLE_t stIntLevel2Param;


    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    stIntLevel1Param.ulModule = pstPt[0].u;
    stIntLevel1Param.ulEnable = pstPt[2].u - 1;

    stApiData.apiId = HAL_API_INT_LEVEL1_ENABLE;
    stApiData.length = sizeof(HAL_API_INT_LEVEL1_ENABLE_t);
    stApiData.param = &stIntLevel1Param;
    stApiData.eventFunc = NULL;

    retVal = halAppApiCallSync(&stApiData);
    retVal += stIntLevel1Param.ulResult;

    stIntLevel2Param.ulModule = pstPt[0].u;
    stIntLevel2Param.ulSubModule = pstPt[1].u;
    stIntLevel2Param.ulEnable = pstPt[2].u - 1;

    stApiData.apiId = HAL_API_INT_LEVEL2_ENABLE;
    stApiData.length = sizeof(HAL_API_INT_LEVEL2_ENABLE_t);
    stApiData.param = &stIntLevel2Param;
    stApiData.eventFunc = NULL;

    retVal += halAppApiCallSync(&stApiData);
    retVal += stIntLevel2Param.ulResult;

    return retVal;
}


STATUS cliCmdOnuLogicalIdSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	STATUS retVal;

	retVal = NO_ERROR;

	if (NULL == pstEnv || NULL == pstPt) 
	{
		retVal = ERROR;
		return retVal;
	}
	
    if (vosStrLen(pstPt[0].p) > 32)
    {
        vosPrintf(pstEnv->nWriteFd, "Logical onu id too long[%d]!\r\n", vosStrLen(pstPt[0].p));
        return ERROR;
    }

    if (pstPt[1].i == 1)
    {
        if (vosStrLen(pstPt[2].p) > 32)
        {
            vosPrintf(pstEnv->nWriteFd, "Password too long[%d]!\r\n", vosStrLen(pstPt[2].p));
            return ERROR;
        }
        odmPonLoidSet(pstPt[0].p);
        odmPonPasswordSet(pstPt[2].p);
    }
    else {
        odmPonLoidSet(pstPt[0].p);
	    odmPonPasswordSet(NULL);
    }	
	return retVal;
}


STATUS cliCmdMdioRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal = ERROR;
	UINT32 regId;
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_MDIO_REG_t stRegParam;
    HAL_API_GBL_MDIO_REG_FIELDS_t stRegFieldsParam;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);

    if (pstPt[2].u == 1)
    {
    	for(regId = pstPt[1].u; regId < pstPt[1].u+pstPt[4].u;regId++)
    	{
            stRegParam.ucDevAddr = pstPt[0].u;
            stRegParam.ucRegAddr = regId;
            stRegParam.usData = 0;
            stRegParam.ulResult = 0;

            stApiData.apiId = HAL_API_GBL_MDIO_REG_READ;
            stApiData.length = sizeof(HAL_API_GBL_MDIO_REG_t);
            stApiData.param = &stRegParam;
            stApiData.eventFunc = NULL;
            halAppApiCallSync(&stApiData);
    		vosPrintf(pstEnv->nWriteFd, "devAddr = %02u, regId = %02u val = 0x%04x\n", pstPt[0].u,regId,stRegParam.usData);
    	}
    }
    else if (pstPt[2].u == 2)
    {
        stRegFieldsParam.ucDevAddr = pstPt[0].u;
        stRegFieldsParam.ucRegAddr = pstPt[1].u;
        stRegFieldsParam.ucOffset = pstPt[6].u;
        stRegFieldsParam.ucWidth = pstPt[7].u;
        stRegFieldsParam.usData = 0;
        stRegFieldsParam.ulResult = 0;

        stApiData.apiId = HAL_API_GBL_MDIO_REG_FIELD_READ;
        stApiData.length = sizeof(HAL_API_GBL_MDIO_REG_FIELDS_t);
        stApiData.param = &stRegFieldsParam;
        stApiData.eventFunc = NULL;
        retVal = halAppApiCallSync(&stApiData);
        vosPrintf(pstEnv->nWriteFd, "devAddr = %d, regId = %d, offset = %d, width = %d, val = 0x%04x\n", pstPt[0].u,pstPt[1].u,pstPt[6].u,pstPt[7].u,stRegFieldsParam.usData);

    }
    else
    {
        stRegParam.ucDevAddr = pstPt[0].u;
        stRegParam.ucRegAddr = pstPt[1].u;
        stRegParam.usData = 0;
        stRegParam.ulResult = 0;

        stApiData.apiId = HAL_API_GBL_MDIO_REG_READ;
        stApiData.length = sizeof(HAL_API_GBL_MDIO_REG_t);
        stApiData.param = &stRegParam;
        stApiData.eventFunc = NULL;
        halAppApiCallSync(&stApiData);
		vosPrintf(pstEnv->nWriteFd, "devAddr = %d, regId = %d val = 0x%04x\n", pstPt[0].u, pstPt[1].u, stRegParam.usData);
    }

    cliShowEnd(pstEnv);

    return retVal;
}

STATUS cliCmdMdioWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal = ERROR;
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_MDIO_REG_t stRegParam;
    HAL_API_GBL_MDIO_REG_FIELDS_t stRegFieldsParam;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    if (pstPt[2].u == 1)
    {
        stRegParam.ucDevAddr = pstPt[0].u;
        stRegParam.ucRegAddr = pstPt[1].u;
        stRegParam.usData = pstPt[4].u;
        stRegParam.ulResult = 0;

        stApiData.apiId = HAL_API_GBL_MDIO_REG_WRITE;
        stApiData.length = sizeof(HAL_API_GBL_MDIO_REG_t);
        stApiData.param = &stRegParam;
        stApiData.eventFunc = NULL;
        retVal =halAppApiCallSync(&stApiData);
		vosPrintf(pstEnv->nWriteFd, "devAddr = %d, regId = %d val = 0x%04x\n", pstPt[0].u,pstPt[1].u,stRegParam.usData);
    }
    else if (pstPt[2].u == 2)
    {
        stRegFieldsParam.ucDevAddr = pstPt[0].u;
        stRegFieldsParam.ucRegAddr = pstPt[1].u;
        stRegFieldsParam.ucOffset = pstPt[6].u;
        stRegFieldsParam.ucWidth = pstPt[7].u;
        stRegFieldsParam.usData = pstPt[8].u;
        stRegFieldsParam.ulResult = 0;

        stApiData.apiId = HAL_API_GBL_MDIO_REG_FIELD_WRITE;
        stApiData.length = sizeof(HAL_API_GBL_MDIO_REG_FIELDS_t);
        stApiData.param = &stRegFieldsParam;
        stApiData.eventFunc = NULL;
        retVal = halAppApiCallSync(&stApiData);
        if (retVal != OK || stRegFieldsParam.ulResult != OK)
        {
            vosPrintf(pstEnv->nWriteFd, 
                "MDIO write (devAddr = %d, regId = %d, offset = %d, width = %d, val = 0x%04x) failed.\r\n", 
                pstPt[0].u,pstPt[1].u,pstPt[6].u,pstPt[7].u,pstPt[8].u);
        }
        else 
        {
            vosPrintf(pstEnv->nWriteFd, 
                "MDIO write (devAddr = %d, regId = %d, offset = %d, width = %d, val = 0x%04x) successfully.\r\n", 
                pstPt[0].u,pstPt[1].u,pstPt[6].u,pstPt[7].u,pstPt[8].u);
        }

    }

    return retVal;
}


extern OPL_STATUS MdioSwGlobalRegRead( UINT32 regAddr, UINT32 * regVal );
extern OPL_STATUS MdioSwGlobalRegWrite( UINT32 regAddr, UINT32 regVal );
extern OPL_STATUS MdioSwPhyRegRead(UINT32 phyId, UINT32 regAddr, UINT32 * regVal );
extern OPL_STATUS MdioSwPhyRegWrite(UINT32 phyId, UINT32 regAddr, UINT32 regVal);
    
STATUS cliCmdSwMdioRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT32 uiDevAddr;
    UINT32 uiRegAddr;
	UINT32 uiVal = 0x66;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    //cliShowStart(pstEnv);

    uiDevAddr = pstPt[0].u;
    uiRegAddr = pstPt[1].u;

    (void)MdioSwGlobalRegRead(uiRegAddr, &uiVal);
    printf("\nreg:0x%04X val:0x%04X\n",uiRegAddr,uiVal);
    //vosPrintf(pstEnv->nWriteFd, "devAddr = %d, regId = %d, offset = %d, width = %d, val = 0x%04x\n", pstPt[0].u,pstPt[1].u,pstPt[6].u,pstPt[7].u,uiVal);
    
    //cliShowEnd(pstEnv);

    return NO_ERROR;
}

STATUS cliCmdSwMdioWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT32 uiDevAddr;
    UINT32 uiRegAddr;
    UINT32 uiVal;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    uiDevAddr = pstPt[0].u;
    uiRegAddr = pstPt[1].u;
    uiVal     = pstPt[2].u;

    (void)MdioSwGlobalRegWrite(uiRegAddr, uiVal);
    printf("\nreg:0x%04X val:0x%04X\n",uiRegAddr,uiVal);
	//vosPrintf(pstEnv->nWriteFd, "devAddr = %d, regId = %d val = 0x%04x\n", pstPt[0].u,pstPt[1].u,uiVal);

    return NO_ERROR;
}

STATUS cliCmdSwMdioPhyRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT32 uiDevAddr;
    UINT32 uiPhyID;
    UINT32 uiRegAddr;
	UINT32 uiVal = 0x66;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    //cliShowStart(pstEnv);

    uiDevAddr = pstPt[0].u;
    uiPhyID   = pstPt[1].u;
    uiRegAddr = pstPt[2].u;

    (void)MdioSwPhyRegRead(uiPhyID, uiRegAddr, &uiVal);
    printf("\nphy:%d reg:0x%04X val:0x%04X\n",uiPhyID,uiRegAddr,uiVal);
    //vosPrintf(pstEnv->nWriteFd, "devAddr = %d, regId = %d, offset = %d, width = %d, val = 0x%04x\n", pstPt[0].u,pstPt[1].u,pstPt[6].u,pstPt[7].u,uiVal);
    
    //cliShowEnd(pstEnv);

    return NO_ERROR;
}

STATUS cliCmdSwMdioPhyWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT32 uiDevAddr;
    UINT32 uiPhyID;
    UINT32 uiRegAddr;
    UINT32 uiVal;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    uiDevAddr = pstPt[0].u;
    uiPhyID   = pstPt[1].u;
    uiRegAddr = pstPt[2].u;
    uiVal     = pstPt[3].u;

    (void)MdioSwPhyRegWrite(uiPhyID, uiRegAddr, uiVal);
    printf("\nphy:%d reg:0x%04X val:0x%04X\n",uiPhyID,uiRegAddr,uiVal);
	//vosPrintf(pstEnv->nWriteFd, "devAddr = %d, regId = %d val = 0x%04x\n", pstPt[0].u,pstPt[1].u,uiVal);

    return NO_ERROR;
}

#if defined(CONFIG_PRODUCT_EM200)
extern INT32 DRV_SingleMdioDbgWrite(UINT32 uiReg, UINT32 uiData);
extern INT32 DRV_SingleMdioDbgRead(UINT32 uiReg, UINT32 *puiData);

STATUS cliCmdSingleMdioDbgWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT32 uiRegAddr;
    UINT32 uiVal;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    uiRegAddr = pstPt[0].u;
    uiVal   = pstPt[1].u;
    (void)DRV_SingleMdioDbgWrite(uiRegAddr, uiVal);
    printf("\nreg:0x%04X val:0x%04X\n",uiRegAddr,uiVal);
	//vosPrintf(pstEnv->nWriteFd, "devAddr = %d, regId = %d val = 0x%04x\n", pstPt[0].u,pstPt[1].u,uiVal);

    return NO_ERROR;
}

STATUS cliCmdSingleMdioDbgRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT32 uiRegAddr;
	UINT32 uiVal = 0x66;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    //cliShowStart(pstEnv);

    uiRegAddr = pstPt[0].u;

    (void)DRV_SingleMdioDbgRead(uiRegAddr, &uiVal);
    printf("\nreg:0x%04X val:0x%04X\n",uiRegAddr,uiVal);
    //vosPrintf(pstEnv->nWriteFd, "devAddr = %d, regId = %d, offset = %d, width = %d, val = 0x%04x\n", pstPt[0].u,pstPt[1].u,pstPt[6].u,pstPt[7].u,uiVal);
    
    //cliShowEnd(pstEnv);

    return NO_ERROR;
}
#endif


STATUS cliCmdI2cInit(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT32 uiMode;
    UINT32 uiIntMode;
    UINT32 uiSpeed;
    UINT32 uiVal;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    uiMode    = pstPt[0].u;
    uiIntMode = pstPt[1].u;
    uiSpeed   = pstPt[2].u;

    (void)i2cInit(uiMode, uiIntMode, 0, uiSpeed);

    return NO_ERROR;
}

STATUS cliCmdI2cRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT32 Phyaddr;
    UINT32 Regaddr;
    UINT32 uiBytes;
    UINT8 aucVal[32] = {0};
    int i =0;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    Phyaddr = pstPt[0].u;
    Regaddr   = pstPt[1].u;
    uiBytes = pstPt[2].u;
    if (uiBytes > 32)
    {
        printf("\nInput error!\n");
    }
    (void)i2cReadRandom((UINT8)Phyaddr, (UINT8)Regaddr, uiBytes, aucVal);
	printf("\nread i2c addr:0x%x reg:0x%x byte:0x%x val:\n",Phyaddr,Regaddr,uiBytes);
	for ( i = 0 ; i < uiBytes; i++ )
	{
	    printf(" %x", aucVal[i]);
	}
	printf("\n");

    return NO_ERROR;
}


#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
STATUS cliCmdAtherosPhyRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_STATUS retVal = OPL_ERROR;
	UINT32 regId;
    UINT16 regVal;
	UINT32 index;
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);
	regId = pstPt[2].u;

	for(index=0; index < pstPt[3].u;regId+=2,index++)
	{
	 #if defined(ONU_4PORT_AR8327)
        retVal = isis_phy_get(pstPt[0].u, pstPt[1].u, regId, &regVal);
	 #else
        retVal = shiva_phy_get(pstPt[0].u, pstPt[1].u, regId, &regVal);
	 #endif
        if (retVal != OPL_OK)
        {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, PHY = %02u, regId = %02u, read failed.\n", 
                pstPt[0].u,pstPt[1].u,regId);
        }
        else {
		    vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, PHY = %02u, regId = %02u, val = 0x%04x\n", 
                pstPt[0].u,pstPt[1].u,regId,regVal);
        }
	}

    cliShowEnd(pstEnv);

    return retVal;
}

STATUS cliCmdAtherosPhyWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_STATUS retVal = OPL_ERROR;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    #if defined(ONU_4PORT_AR8327)
    retVal = isis_phy_set(pstPt[0].u, pstPt[1].u, pstPt[2].u, (UINT16)pstPt[3].u);
    #else
    retVal = shiva_phy_set(pstPt[0].u, pstPt[1].u, pstPt[2].u, (UINT16)pstPt[3].u);
    #endif
    if (retVal != OPL_OK)
    {
        vosPrintf(pstEnv->nWriteFd, 
            "Write devAddr = %02u, PHY = %02u, regId = %02u, val = 0x%04x failed.\n", 
            pstPt[0].u, pstPt[1].u, pstPt[2].u, (UINT16)pstPt[3].u);
    }
    else {
        vosPrintf(pstEnv->nWriteFd, 
            "Write devAddr = %02u, PHY = %02u, regId = %02u, val = 0x%04x succeeded.\n", 
            pstPt[0].u, pstPt[1].u, pstPt[2].u, (UINT16)pstPt[3].u);
    }

    return retVal;
}


STATUS cliCmdAtherosRegRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_STATUS retVal = OPL_ERROR;
	UINT32 regId;
    UINT32 regVal;
	UINT32 i=0;
	
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);
	
	regId=pstPt[1].u & 0xfffffffc;

    if (pstPt[2].u == 1)
    {
    	for(i=0; i < pstPt[4].u;regId+=4,i++)
    	{
    	     #if defined(ONU_4PORT_AR8327)
	     retVal = isis_reg_get(pstPt[0].u, regId, (UINT8 *) &regVal, sizeof(UINT32));
	     #else
            retVal = shiva_reg_get(pstPt[0].u, regId, (UINT8 *) &regVal, sizeof(UINT32));
	     #endif
            if (retVal != OPL_OK)
            {
                vosPrintf(pstEnv->nWriteFd, 
                    "devAddr = %02u, regId = 0x%04x, read failed.\n", 
                    pstPt[0].u, regId);
            }
            else {
    		    vosPrintf(pstEnv->nWriteFd, 
                    "devAddr = %02u, regId = 0x%04x, val = 0x%08x\n", 
                    pstPt[0].u, regId, regVal);
            }
    	}
    }
    else if (pstPt[2].u == 2)
    {
        #if defined(ONU_4PORT_AR8327)
	 retVal = isis_reg_field_get(pstPt[0].u, regId, pstPt[6].u, pstPt[7].u, (UINT8 *) &regVal, sizeof(UINT32));
        #else
	 retVal = shiva_reg_field_get(pstPt[0].u, regId, pstPt[6].u, pstPt[7].u, (UINT8 *) &regVal, sizeof(UINT32));
	 #endif
        if (retVal != OPL_OK)
        {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, offset = %d, width = %d, read failed.\n", 
                pstPt[0].u,pstPt[1].u,pstPt[6].u,pstPt[7].u);
        }
        else {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, offset = %d, width = %d, val = 0x%08x\n", 
                pstPt[0].u,pstPt[1].u,pstPt[6].u,pstPt[7].u, regVal);
        }
    }
    else if (pstPt[2].u == 0)
    {
        #if defined(ONU_4PORT_AR8327)
	 retVal = isis_reg_get(pstPt[0].u, regId, (UINT8 *) &regVal, sizeof(UINT32));
	 #else
        retVal = shiva_reg_get(pstPt[0].u, regId, (UINT8 *) &regVal, sizeof(UINT32));
	 #endif
        if (retVal != OPL_OK)
        {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, read failed.\n", 
                pstPt[0].u, regId);
        }
        else {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, val = 0x%08x\n", 
                pstPt[0].u, regId, regVal);
        }
    }

    cliShowEnd(pstEnv);

    return retVal;
}

STATUS cliCmdAtherosRegWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_STATUS retVal = OPL_ERROR;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    if (pstPt[2].u == 1)
    {
        #if defined(ONU_4PORT_AR8327)
	 	retVal = isis_reg_set(pstPt[0].u, pstPt[1].u, (UINT8 *) &pstPt[4].u, sizeof(UINT32));
	 #else
        retVal = shiva_reg_set(pstPt[0].u, pstPt[1].u, (UINT8 *) &pstPt[4].u, sizeof(UINT32));
	 #endif
        if (retVal != OPL_OK)
        {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, write failed.\n", 
                pstPt[0].u, pstPt[1].u);
        }
        else {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, val = 0x%04x\n", 
                pstPt[0].u, pstPt[1].u, pstPt[4].u);
        }
    }
    else if (pstPt[2].u == 2)
    {
        #if defined(ONU_4PORT_AR8327)
	    retVal = isis_reg_field_set(pstPt[0].u, pstPt[1].u, pstPt[6].u, pstPt[7].u, (UINT8 *) &pstPt[8].u, sizeof(UINT32));
	 #else
        retVal = shiva_reg_field_set(pstPt[0].u, pstPt[1].u, pstPt[6].u, pstPt[7].u, (UINT8 *) &pstPt[8].u, sizeof(UINT32));
	 #endif
        if (retVal != OPL_OK)
        {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, offset = %d, width = %d, write failed.\n", 
                pstPt[0].u, pstPt[1].u,pstPt[6].u,pstPt[7].u);
        }
        else {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, offset = %d, width = %d, val = 0x%04x\n", 
                pstPt[0].u, pstPt[1].u,pstPt[6].u,pstPt[7].u,pstPt[8].u);
        }
    }

    return retVal;
}
STATUS cliCmdAtherosClassifyWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_STATUS retVal = OPL_ERROR;

    if (NULL == pstEnv)
    {
        return ERROR;
    }
    cliShowStart(pstEnv);
	#if defined(ONU_4PORT_AR8327)
	retVal=isis_acl_rule_dump(0);
	retVal+=isis_acl_list_dump(0);
	#else
	retVal=shiva_acl_rule_dump(0);
	retVal+=shiva_acl_list_dump(0);
	#endif
	cliShowEnd(pstEnv);

    return retVal;
}

STATUS cliCmdAtherosMibShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_STATUS retVal = OPL_ERROR;
	UINT8 port=pstPt[0].u ;
    UINT32  atherosPort = cliToAtherosPortMap[port];

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);
	retVal=atheros_show_cnt(atherosPort);
	cliShowEnd(pstEnv);

    return retVal;
}

#endif

#if 0 /*wfxu 0403 will be done*/

static VOS_THREAD_t g_pstI2cTestThreadId = NULL;
struct I2C_TEST_ARG_s
{
    int i2cMode;
    int intMode;
    int slaveAddr;
    int frequency;
    int fd;
};

struct I2C_TEST_ARG_s g_stI2CTestArg;

UINT32 g_ulI2CTestTotal = 0;
UINT32 g_ulI2CTestReadCnt = 0;
UINT32 g_ulI2CTestErrCnt = 0;

static void i2cTestingThread(void *pArg)
{
    int i; 
    STATUS retVal = ERROR;
    enum { BUF_SIZE = 256 };
    UINT8 aucOriginBuf[BUF_SIZE+1] = {0};
    UINT8 aucBuf[BUF_SIZE+1] = {0};
    char acTime[TIME_STR_LEN];
    time_t now;
    struct tm *pstLogTime;

    struct I2C_TEST_ARG_s *pstI2CTestArg = (struct I2C_TEST_ARG_s *)pArg;

    if (NULL == pstI2CTestArg) 
    {
        vosThreadExit(0);
        return;
    }

    g_ulI2CTestTotal = 0;
    g_ulI2CTestReadCnt = 0;
    g_ulI2CTestErrCnt = 0;

    time(&now);
    pstLogTime = gmtime(&now);
    strftime(acTime, TIME_STR_LEN, "%Y-%m-%d %H:%M:%S", pstLogTime);
    vosPrintf(pstI2CTestArg->fd, "\r\n\r\nI2C testing task started, at %s.\r\n", acTime);

    i2cInit(pstI2CTestArg->i2cMode, pstI2CTestArg->intMode, pstI2CTestArg->slaveAddr, pstI2CTestArg->frequency);

    while(1)
    {
        time(&now);
        pstLogTime = gmtime(&now);
        strftime(acTime, TIME_STR_LEN, "%Y-%m-%d %H:%M:%S", pstLogTime);
        retVal = i2cReadRandom(OT_EPROM_I2C_ADDR, 0, BUF_SIZE, aucBuf);
        g_ulI2CTestTotal++;
        if (retVal < 0) /* check status */
        {
            vosPrintf(pstI2CTestArg->fd, "%s: i2cRead got error(%ld). Reset I2C.\r\n", acTime, retVal);
            i2cInit(pstI2CTestArg->i2cMode, pstI2CTestArg->intMode, pstI2CTestArg->slaveAddr, pstI2CTestArg->frequency);
            vosSleep(1);
        }
        else {
            g_ulI2CTestReadCnt++;
            if (BUF_SIZE == retVal && g_ulI2CTestReadCnt == 3)
            {
                memcpy(aucOriginBuf, aucBuf, BUF_SIZE);
            }

            if (g_ulI2CTestReadCnt < 4)
            {
                vosPrintf(pstI2CTestArg->fd, "%s: i2cRead got data[%d]: \r\n", acTime, retVal);
                for (i = 0; i < retVal; i++)
                {
                    vosPrintf(pstI2CTestArg->fd, "%02X ", aucBuf[i]);
                }
                vosPrintf(pstI2CTestArg->fd, "\r\n\r\n");
            }
            else {
                if (0 != memcmp(aucOriginBuf, aucBuf, BUF_SIZE))
                {
                    g_ulI2CTestErrCnt++;
                    vosPrintf(pstI2CTestArg->fd, "%s: i2cRead got error data[%d]: \r\n", acTime, retVal);
                    for (i = 0; i < retVal; i++)
                    {
                        vosPrintf(pstI2CTestArg->fd, "%02X ", aucBuf[i]);
                    }
                    vosPrintf(pstI2CTestArg->fd, "\r\n\r\n");
                }
            }
            
            vosPrintf(pstI2CTestArg->fd, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            vosPrintf(pstI2CTestArg->fd, "%lu/%lu/%lu",g_ulI2CTestTotal,g_ulI2CTestReadCnt,g_ulI2CTestErrCnt);
        }
        /*vosUSleep(10);*/
    }
    vosThreadExit(0);
}

STATUS i2cTestingThreadInit(int fd, int intMode, int frequency)
{
    int ret;

    if (g_pstI2cTestThreadId) {
        return ERROR;
    }

    g_stI2CTestArg.i2cMode = 1;
    g_stI2CTestArg.intMode = intMode;
    g_stI2CTestArg.slaveAddr = 0;
    g_stI2CTestArg.frequency = frequency;
    g_stI2CTestArg.fd = fd;

    g_pstI2cTestThreadId = vosThreadCreate("tI2CTest", OP_VOS_THREAD_STKSZ, 90,
                        (void *)i2cTestingThread, (void *)&g_stI2CTestArg);
    if (g_pstI2cTestThreadId == NULL) {
        return ERROR;
    }

    return OK;
}

STATUS i2cTestingThreadShutdown(int fd)
{
    char acTime[TIME_STR_LEN];
    time_t now;
    struct tm *pstLogTime;

    if (!g_pstI2cTestThreadId) {
        return ERROR;
    }

    if (g_pstI2cTestThreadId != NULL) {
        vosThreadDestroy(g_pstI2cTestThreadId);
        g_pstI2cTestThreadId = NULL;
    }

    time(&now);
    pstLogTime = gmtime(&now);
    strftime(acTime, TIME_STR_LEN, "%Y-%m-%d %H:%M:%S", pstLogTime);
    vosPrintf(fd, "\r\n\r\nI2C testing task stoped, at %s.\r\n", acTime);
    vosPrintf(fd, "total: %lu, read: %lu, err: %lu.\r\n\r\n",g_ulI2CTestTotal,g_ulI2CTestReadCnt,g_ulI2CTestErrCnt);
    
    return OK;
}


STATUS cliCmdI2CTestStart(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv || NULL == pstPt) 
	{
		return ERROR;
	}
	
    if (NULL != g_pstI2cTestThreadId)
    {
        vosPrintf(pstEnv->nWriteFd, "I2C testing task has already been started.\r\n");
        return OK;
    }
    
    return i2cTestingThreadInit(pstEnv->nWriteFd, pstPt[0].u-1, pstPt[1].u-1);
}

STATUS cliCmdI2CTestStop(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{

    if (NULL == pstEnv) 
	{
		return ERROR;
	}
	
    if (NULL == g_pstI2cTestThreadId)
    {
        vosPrintf(pstEnv->nWriteFd, "I2C testing task is not started.\r\n");
        return OK;
    }
    
    return i2cTestingThreadShutdown(pstEnv->nWriteFd);
}

#endif
STATUS cliCmdLinuxShell(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int oldPhase, pid, stat;

    if (NULL == pstEnv)
    {
        return ERROR;
    }

    oldPhase = pstEnv->phase;
    pstEnv->phase = CLI_CMD_PHASE_SYS_SHELL;
    pstEnv->used = TRUE;
    if (pstEnv->ucIsConsole)
    {
        vosTermRestore(pstEnv->nReadFd);
        setenv("PS1", "OPULAN / #", 1);
        vosSystem("/bin/sh");
	    vosTermConfig(pstEnv->nReadFd);
    }
    else {
	    vosPrintf(pstEnv->nWriteFd, "%% Does not support, please telnet to 23 port.\r\n");
    }

    pstEnv->phase = oldPhase;

    return OK;
}


STATUS cliCmdShowThreadInfo(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv)
    {
        return ERROR;
    }

	vosThreadShow(pstEnv->nWriteFd);

    return OK;
}


STATUS cliCmdOpticalModuleOnOff(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT32 retVal;
    int i;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    if (1 == pstPt->u)
    {
        vosPrintf(pstEnv->nWriteFd, "Turn on optical module ... ");
        odmPonTxPowerDown(FALSE);
        vosPrintf(pstEnv->nWriteFd, "done\r\n");
    }
    else if (2 == pstPt->u)
    {
        vosPrintf(pstEnv->nWriteFd, "Turn off optical module ... ");
        odmPonTxPowerDown(TRUE);
        vosPrintf(pstEnv->nWriteFd, "done\r\n");
    }

    return OK;
}


extern unsigned long g_ulVosAllocBytes;
extern unsigned long g_ulVosAllocCalls;
extern unsigned long g_ulVosFreeBytes;
extern unsigned long g_ulVosFreeCalls;

STATUS cliCmdShowMemInfo(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    char *header  = "  status    calls       bytes\r\n";
    char *divider = " --------- ----------- ----------- \r\n";
    char *format = " %-9s %11u %11u\r\n";
    
    vosPrintf(pstEnv->nWriteFd, header);
    vosPrintf(pstEnv->nWriteFd, divider);
    vosPrintf(pstEnv->nWriteFd, format, "alloc", g_ulVosAllocCalls, g_ulVosAllocBytes);
    vosPrintf(pstEnv->nWriteFd, format, "free", g_ulVosFreeCalls, g_ulVosFreeBytes);

}


