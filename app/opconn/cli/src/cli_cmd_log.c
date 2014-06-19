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
* FILENAME:  cli_cmd_log.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_cmd_log.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "cli.h"
#include "cli_cmd_list.h"
#include "log.h"
#include "opl_driver.h"

/* begin added by jiangmingli for host debug */
/* end added by jiangmingli for host debug */

extern const char *g_pcModule[MAX_MODULE];
extern const char *g_pcDebugLevel[DEBUG_LEVEL_MAX];

extern OPL_BOOL g_bHostDsDbgEn;
extern OPL_BOOL g_bHostUsDbgEn;

#define MODULE MOD_LOG

STATUS cliCmdLogDebugLevel(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int i;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    if (pstPt[0].i > 0 && 
        pstPt[0].i <= MAX_MODULE && 
        pstPt[1].i > 0 && 
        pstPt[1].i <= DEBUG_LEVEL_MAX)
    {
        cliSessionDebugLevelSet(pstEnv->ucIndex, pstPt[0].i - 1, pstPt[1].i - 1);
        vosPrintf(pstEnv->nWriteFd, 
            "Set debug module %s level %s\r\n", 
            g_pcModule[pstPt[0].i - 1], 
            g_pcDebugLevel[pstPt[1].i - 1]);
        return OK;
    }

    vosPrintf(pstEnv->nWriteFd, 
            "Invalid debug module %d level %d\r\n", 
            pstPt[0].i - 1, 
            pstPt[1].i - 1);

    return ERROR;    
}


STATUS cliCmdShowLog(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int i;
    UINT16 uiIdx;

    if (NULL == pstEnv)
    {
        return ERROR;
    }

    if (NULL != pstPt &&
        0 < pstPt->i)
    {
        cliShowStart(pstEnv);
        sysLogCfgShow(pstEnv);
        cliShowEnd(pstEnv);
    }
    else {
#if 0  /* test code */
        for (i = 0; i < 10; i++)
        {
            OP_DEBUG(DEBUG_LEVEL_CRITICAL, "test critical level debug");
            OP_DEBUG(DEBUG_LEVEL_WARNING, "test warning level debug");
            OP_DEBUG(DEBUG_LEVEL_INFO, "test information level debug");
            OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "test debugging level debug");
            
            OP_DEBUG_RAW(DEBUG_LEVEL_DEBUGGING, "==========receive payload length: %d===========", 120);
            for(uiIdx = 0; uiIdx < 120; uiIdx++)
            {
                if(0 == uiIdx%16)
                {
                    OP_DEBUG_RAW(DEBUG_LEVEL_DEBUGGING, "\r\n0x%08x\t", uiIdx);
                }
                OP_DEBUG_RAW(DEBUG_LEVEL_DEBUGGING, " 0x%02x", (vosRandom() % 255));
            }
            OP_DEBUG_RAW(DEBUG_LEVEL_DEBUGGING, "\r\n--------------------------------------\r\n");
        } 
#else
        cliShowStart(pstEnv);
        sysLogShow(pstEnv);
        cliShowEnd(pstEnv);
#endif /* #if 0 */
    }

    return OK;
}


STATUS cliCmdLogEnable(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    sysLogSetLogEnable(pstPt->i-1);

    return OK;
}

STATUS cliCmdSyslogEnable(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    sysLogSetServerLogEnable(pstPt->i-1);

    return OK;
}

STATUS cliCmdSyslogTest(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	int i;
	int testnum=1;
	
    if (NULL == pstEnv)
    {
        return ERROR;
    }
	if(pstPt[0].p!=NULL && pstPt[0].u>0) testnum=pstPt[1].u;

	for(i=0;i<testnum;i++)
    	sysLogTestServerLog();
    return OK;
}



STATUS cliCmdShowSyslogServer(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);
    sysLogServerShow(pstEnv);
    cliShowEnd(pstEnv);

    return OK;
}

STATUS cliCmdSetSyslogServer(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	UINT32 ucSyslogAddress;
	UINT16 ucLogPort;
    char str_ip[20] = "";
    int ret;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
	ucSyslogAddress = pstPt[0].u;
    cliIpToStr(ucSyslogAddress,str_ip);
    if (!cliIsValidIpAddress (str_ip) )
    {
        vosPrintf(pstEnv->nWriteFd,"%%Invalid IP address \n");
        return ERROR;
    }
	if(pstPt[1].p!=NULL  && 0 < pstPt[1].i )
	{
		ucLogPort=(UINT16)(pstPt[2].u);
	}
	else
	{
		ucLogPort=SYSLOG_DEFAULT_PORT;
	}
    ret=sysLogSetServerLogAddress(ucSyslogAddress,ucLogPort);
	if(ret== SYS_LOG_SERVER_ALREADY_EXIST)
	 {
		vosPrintf(pstEnv->nWriteFd,"Error when create syslog server ,already exist...\n");
		return ERROR;
	 }
	if(ret== SYS_LOG_SERVER_CFG_ERROR)
	 {
		vosPrintf(pstEnv->nWriteFd,"General config file error when create syslog server...\n");
		return ERROR;
	 }
    return OK;
}

STATUS cliCmdDelSyslogServer(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	UINT32 ucSyslogAddress;
    int ret;
	
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
	ucSyslogAddress = pstPt[0].u;
    ret=sysLogDelServerLogAddress(ucSyslogAddress);
	if(ret== SYS_LOG_SERVER_NOT_EXIST)
	 {
		vosPrintf(pstEnv->nWriteFd,"Syslog Server Address Not Exist... \n");
		return ERROR;
	 }

    return OK;
}
STATUS cliCmdLogClear(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv)
    {
        return ERROR;
    }

    sysLogFileClear();
	sysLogServerFileClear();/*clear syslog info*/
    return OK;
}

STATUS cliCmdShowAlarm(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int i;

    if (NULL == pstEnv)
    {
        return ERROR;
    }

#if 0  /* test code */
    for (i = 0; i < 100; i++)
    {
        OP_ALARM("test alram message (%d) ", i);
    }
#else
    cliShowStart(pstEnv);
    sysAlarmShow(pstEnv);
    cliShowEnd(pstEnv);
#endif /* #if 0 */

    return OK;
}

STATUS cliCmdAlarmClear(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv)
    {
        return ERROR;
    }

    sysAlarmFileClear();

    return OK;
}

/* begin added by jiangmingli for host debug */
STATUS cliCmdHostDbgEn(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_BOOL bEnable;
    
    if (NULL == pstEnv)
    {
        return ERROR;
    }

    if (1 == pstPt[1].i)
    {
        bEnable = OPL_TRUE;
    }
    else
    {
        bEnable = OPL_FALSE;
    }

    if (1 == pstPt[0].i)
    {
        g_bHostUsDbgEn = bEnable;
    }
    else
    {
        g_bHostDsDbgEn = bEnable;
    }

    return OK;
}
/* end added by jiangmingli for host debug */



