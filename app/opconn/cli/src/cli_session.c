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
* FILENAME:  cli_session.c
*
* DESCRIPTION: 
*	
*
* Date Created: May 07, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_session.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "cli_session.h"
#include "errors.h"
#include "cli_utils.h"

#define SESSION_KEY_STR "Session"
static HASH_TABLE_t *g_pstSessions = NULL;

UINT32 g_ulCliSessionMap = 0;
UINT8 g_ucModuleDebugLevel[MAX_SESSION_NUM][MAX_MODULE];

static void cliSessionDataDestroy(void *key, void *value)
{
    char *k = key;
    ENV_t *v = value;
    vosFree(k);
    if (1 != v->free)
    {
        vosFree(v->para);
        cliResourceFree(v);
    }
    if (v->pcUserName)
    {
        vosFree(v->pcUserName);
    }
    cliSessionIndexRelease(v->ucIndex);
    vosFree(v);
}

UINT32 cliSessionInit(void)
{
    g_pstSessions = vosHashCreate(vosStringHash, vosEqualString, cliSessionDataDestroy, 0);

    return NO_ERROR;
}

UINT32 cliSessionDestroy(void)
{
    vosHashFree(g_pstSessions);

    return NO_ERROR;
}

UINT32 cliSessionCount(void)
{
    /* should be decrease 1 for console */
    return (UINT32)vosHashCount(g_pstSessions) - 1;
}

ENV_t * cliSessionGet(VOS_THREAD_t tThreadId)
{
    char sessKeyStr[STR_SIZE_32_BYTES] = {0};
    vosSnprintf(sessKeyStr,STR_SIZE_32_BYTES,"%s%08X",SESSION_KEY_STR, (UINT32)tThreadId);
    return (ENV_t *)vosHashValue((void *)sessKeyStr, g_pstSessions);
}

ENV_t * cliSessionGetByIndex(UINT16 usIndex)
{
    char    *pcSessionKey = NULL;
    ENV_t   *pstResult = NULL;
    int     ret;
	UINT16  i;
    UINT16  usCount = cliSessionCount();

	for (i = 0; i <= usCount; i++)
	{
        ret = vosHashValueGetByIndex(i, g_pstSessions, &pcSessionKey, &pstResult);
        if (OK == ret)
        {
            if (NULL != pstResult && pstResult->ucIndex == usIndex)
            {
                return pstResult;
            }
        }
	}
    return NULL;
}


ENV_t * cliSessionAdd(VOS_THREAD_t tThreadId, UINT8 ucIsConsole)
{
    ENV_t *pstEnv = NULL;
    char sessKeyStr[STR_SIZE_32_BYTES] = {0};
    /* +1 for console */
    if (vosHashCount(g_pstSessions) >= odmSysCfgTelnetSessionNumGet() + 1)
        return pstEnv;
    
    pstEnv = (ENV_t *)vosAlloc(sizeof(ENV_t));

    if (pstEnv != NULL)
    {
        vosMemSet(pstEnv, 0, sizeof(ENV_t));

        pstEnv->pstMainTaskId = tThreadId;
        pstEnv->echo = ENABLED;
        vosSnprintf(sessKeyStr, STR_SIZE_32_BYTES,"%s%08X",SESSION_KEY_STR, (UINT32)tThreadId);
        pstEnv->ucIsConsole = ucIsConsole;
        pstEnv->ucIndex = cliSessionFreeIndexGet();
        pstEnv->page.nPageFd = ERROR;
        pstEnv->page.nOldStdOutFd = ERROR;
        if (INVALID_8 == pstEnv->ucIndex)
        {
            vosFree(pstEnv);
            return NULL;
        }
        vosHashInsert((void *)vosStrDup(sessKeyStr), (void *)pstEnv, g_pstSessions);
    }

    return pstEnv;
}

UINT32 cliSessionDelete(VOS_THREAD_t tThreadId)
{
    char sessKeyStr[STR_SIZE_32_BYTES] = {0};
    vosSnprintf(sessKeyStr,STR_SIZE_32_BYTES,"%s%08X",SESSION_KEY_STR, (UINT32)tThreadId);
    vosHashRemove((void *)sessKeyStr, g_pstSessions);
    
    return NO_ERROR;
}

UINT32 cliSessionCloseByIndex(ENV_t *pstCurrentEnv, UINT16 usIndex)
{
    ENV_t *pstEnv = cliSessionGetByIndex(usIndex);
    if (NULL == pstEnv)
    {
        return SYS_SESSION_INVALID_ID;
    }
    else if (pstCurrentEnv == pstEnv)
    {
        return SYS_SESSION_CAN_NOT_KILL_YOURSELF;
    }
    else
    {
        return cliSessionClose(pstEnv);
    }
}

HASH_TABLE_t * cliSessionTableGet(void)
{
    return g_pstSessions;
}

void cliSessionShow(int fd)
{
	UINT16  i;
    UINT16  usCount = odmSysCfgTelnetSessionNumGet() + 1;
    ENV_t   *pstEnv;
    char    ipAddr[20];
    char    *pacSessionType[2] = {"Telnet", "Console"};

	vosPrintf(fd, " #    Type     Login User         IP Address           Port \r\n");
    vosPrintf(fd, "---- -------- ------------------ -------------------- ------\r\n");

	for (i = 0; i <= usCount; i++)
	{
        pstEnv = cliSessionGetByIndex(i);
        if (NULL != pstEnv)
        {
            cliIpToStr(pstEnv->clientAddress.sin_addr.s_addr, ipAddr);
    		vosPrintf(fd, "%4d %-8s %-18s %-20s %-5d\r\n",
    			pstEnv->ucIndex,
    			pacSessionType[pstEnv->ucIsConsole],
    			(pstEnv->pcUserName == NULL) ? "" : pstEnv->pcUserName,
    			ipAddr,
    			pstEnv->clientAddress.sin_port);
		}
	}

    vosPrintf(fd, "\r\n");
}

UINT8 cliSessionFreeIndexGet()
{
    UINT16 BitPos;
    UINT8 i;
    BitPos = vosFirstClearBitPosGet(g_ulCliSessionMap, 0);
    if( BitPos >= MAX_SESSION_NUM)
    {
        return INVALID_8;
    }  

    g_ulCliSessionMap = vosBitSet(g_ulCliSessionMap, BitPos);
    for (i = 0; i < MAX_MODULE; i++)
    {
        g_ucModuleDebugLevel[BitPos][i] = DEBUG_LEVEL_OFF;
    }

    return (UINT8)BitPos;
}

STATUS cliSessionIndexRelease(UINT8 ucIndex)
{
    UINT8 i;
    if (!VOS_IS_BIT_SET(ucIndex, g_ulCliSessionMap))
        return ERROR;

    for (i = 0; i < MAX_MODULE; i++)
    {
        g_ucModuleDebugLevel[ucIndex][i] = DEBUG_LEVEL_OFF;
    }
    g_ulCliSessionMap = vosBitClear(g_ulCliSessionMap, ucIndex);
    return OK;
}

STATUS cliSessionDebugLevelSet(UINT8 ucIndex, UINT8 ucModule, UINT8 ucLevel)
{
    UINT8 i;
    if (!VOS_IS_BIT_SET(ucIndex, g_ulCliSessionMap))
        return ERROR;

    if (ucModule >= MAX_MODULE)
        return ERR_OUT_OF_RANGE;

    if (ucLevel >= DEBUG_LEVEL_MAX)
        return ERR_OUT_OF_RANGE;

    if (ucModule == ALL_MODULE)
    {
        for (i = 0; i < ALL_MODULE; i++)
        {
            g_ucModuleDebugLevel[ucIndex][i] = DEBUG_LEVEL_OFF;
        }
    }
    g_ucModuleDebugLevel[ucIndex][ucModule] = ucLevel;
    return OK;
}

UINT8 cliSessionDebugLevelGet(UINT8 ucIndex, UINT8 ucModule)
{
    if (!VOS_IS_BIT_SET(ucIndex, g_ulCliSessionMap))
        return DEBUG_LEVEL_OFF;

    return g_ucModuleDebugLevel[ucIndex][ucModule];
}

STATUS cliSessionDebugLevelCheck(UINT8 ucIndex, UINT8 ucModule, UINT8 ucLevel)
{
    if (!VOS_IS_BIT_SET(ucIndex, g_ulCliSessionMap))
        return ERROR;

    if (((g_ucModuleDebugLevel[ucIndex][ALL_MODULE] >= ucLevel) || 
        (g_ucModuleDebugLevel[ucIndex][ucModule] >= ucLevel)))
    {
        return OK;
    }

    return ERROR;
}

STATUS cliSessionGlobalDebugLevelCheck(UINT8 ucModule, UINT8 ucLevel)
{
    UINT8 i, enCnt = 0;

    for (i = 0; i < MAX_SESSION_NUM; i++)
    {
        if (!VOS_IS_BIT_SET(i, g_ulCliSessionMap))
            continue;

        if (((g_ucModuleDebugLevel[i][ALL_MODULE] >= ucLevel) || 
            (g_ucModuleDebugLevel[i][ucModule] >= ucLevel)))
        {
            return OK;
        }
    }

    return ERROR;    
}


