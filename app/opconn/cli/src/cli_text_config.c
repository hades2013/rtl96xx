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
* FILENAME:  cli_text_config.c
*
* DESCRIPTION: 
*	
*
* Date Created: Sep 15, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_text_config.c#1 $
* $Log:$
*
*
**************************************************************************/


#include "cli.h"
#include "vos.h"

ENV_t  g_textConfigEnv;         /* enviroment parameter for text configuration*/
CMD_POOL_t g_textConfigCmdPool; /* command pool for text configuration */
int g_ucIsLoadingConfig = FALSE;
VOS_MUTEX_t g_pstTextConfigLock;

char textConfig[1024*128];

#define ERROR_INDICATION_LENGTH 4096
char errorIndication[ERROR_INDICATION_LENGTH + 256];

extern CMD_KEY_PARA_LINK_t *g_psRootOfForest;

void cliTextConfigInit()
{
    ENV_t *pstEnv = NULL;
    CMD_POOL_t *psCmdPool = NULL;

    g_ucIsLoadingConfig = FALSE;
    memset(&g_textConfigEnv, 0, sizeof(ENV_t));
    
    if (VOS_MUTEX_OK != vosMutexCreate(&g_pstTextConfigLock))
    { 
        printf("cliTextConfigInit: create config mutex failed.\r\n");
        return;
    } 

    pstEnv = &g_textConfigEnv;
    pstEnv->used = FALSE;

    pstEnv->pmode = CLI_MODE_ENABLE;
    pstEnv->prompt |= DEF_CLI_MODE;
    pstEnv->para = NULL;
    pstEnv->tmode = TERMINAL_MODE_NORMAL;
    pstEnv->accessLevel = ACCESS_LEVEL_SUPER;

    pstEnv->command = (CMD_POOL_t *)vosAlloc(sizeof(CMD_POOL_t));
    memset(pstEnv->command, 0, sizeof(CMD_POOL_t));
    psCmdPool = pstEnv->command;
    psCmdPool->bIsEmpty = TRUE;
    psCmdPool->bIsFull = FALSE;
    
#if 0  /* modified by Gan Zhiheng - 2009/12/24 */
    pstEnv->configureHistoryCommand = (CMD_HISTORY_t *)vosAlloc(sizeof(CMD_HISTORY_t));
    memset(pstEnv->configureHistoryCommand, 0, sizeof(CMD_HISTORY_t));
#else
    pstEnv->configureHistoryCommand = NULL;
#endif /* #if 0 */
  
    pstEnv->ucIndex = INVALID_8;
    pstEnv->close = CLI_CLOSE_NULL;         /* not closed */
    pstEnv->err = OK;
    pstEnv->free = 0;
    pstEnv->phase = CLI_CMD_PHASE_INIT;

    pstEnv->nReadFd = -1;
    pstEnv->nWriteFd = -1;
    pstEnv->nErrFd = -1;
      
    cliEchoDisable(pstEnv);
}

void cliTextConfigDestroy()
{
    ENV_t *pstEnv = NULL;

    pstEnv = &g_textConfigEnv;
    cliResourceFree(pstEnv);
    vosMutexDestroy(&g_pstTextConfigLock);
}

int cliTextConfigExec(char *text, int length, int *errorCmdNum)
{
    ENV_t *pstEnv = NULL;
    CMD_POOL_t *psCmdPool = NULL;
    int rc = OK;
    char *cmdText;
    char *lineStart;
    char *lineEnd;
    int cmdLen;
    CMD_VALIDITY_t cmdValidity = CMD_VALID;
    int indicationLen = 0;
    int lineCount = 1;
    char currentCmd[MAX_POOL_LENGTH+1];
    char *cmdErrorName[] = {"command valid", "command incomplete", "command unrecognized", "invalid input"};

    *errorCmdNum = 0;
    
    pstEnv = &g_textConfigEnv;
    psCmdPool = pstEnv->command;
    cmdText = psCmdPool->achCmdPool;

    memset(errorIndication, 0, sizeof(errorIndication));

    if (!text)
    {
        indicationLen += sprintf(&errorIndication[indicationLen], "No configuration data!\r\n");
        (*errorCmdNum)++;
        rc = ERROR;
        goto exit_label;
    }

    lineStart = text;
    lineEnd = text;

    while (1)
    {
        lineStart = lineEnd;
        while ((*lineStart=='\r')||(*lineStart=='\n')||(*lineStart==' '))
        {
            if (*lineStart=='\n')
            {
                lineCount++;
                /*printf("line (%d)\r\n", lineCount);*/
            }
            lineStart++;
        }
            
        if (!(*lineStart))
        {
            /*printf("read EOS, we are done\r\n");*/
            rc = OK;
            goto exit_label;
        }

        if ((lineStart[0]=='e' || lineStart[0]=='E')
            &&(lineStart[1]=='n' || lineStart[1]=='N')
            &&(lineStart[2]=='d' || lineStart[2]=='D')
            &&(lineStart[3]==0 || lineStart[3]=='\r' || lineStart[3]=='\n' || lineStart[3]==' '))
        {
            /*printf("read end, we are done\n");*/
            rc = OK;
            goto exit_label;
        }

        /* '\n' is more robust than '\r\n' */
        lineEnd = strstr(lineStart, "\n");
        if (!lineEnd)
            lineEnd = strchr(lineStart, 0);

        if (!lineEnd)
        {
            indicationLen += sprintf(&errorIndication[indicationLen], "EOS not found!\r\n");
            (*errorCmdNum)++;
            rc = ERROR;
            goto exit_label;
        }
        
        /* in case of "\r\n" */
        if (*(lineEnd-1) == '\r')
            lineEnd --;

        /* return from specific config mode, like sys, enable ..., to user mode */
        if (*lineStart == '!')
        {
            /*printf("read a \'!\'\r\n");*/
            pstEnv->pmode = CLI_MODE_ENABLE;
            continue;
        }
        
        /* remark line */
        if (*lineStart == '#')
            continue;

        cmdLen = (int)lineEnd - (int)lineStart;
        if (cmdLen>=sizeof(psCmdPool->achCmdPool))
        {
            if (indicationLen < ERROR_INDICATION_LENGTH)
                indicationLen += sprintf(&errorIndication[indicationLen], "line %-4d: command too long (%d bytes)\r\n", lineCount, cmdLen);
            (*errorCmdNum)++;
            continue;
        }

        memcpy(psCmdPool->achCmdPool, lineStart, cmdLen);

        memcpy(currentCmd, lineStart, cmdLen);
        currentCmd[cmdLen] = 0;

        /*printf("cmd: %s\r\n", currentCmd);*/
        rc = cliCmdInterpreter(pstEnv, g_psRootOfForest, &cmdValidity);

        if (cmdValidity != CMD_VALID)
        {
            if (indicationLen < ERROR_INDICATION_LENGTH)
                indicationLen += sprintf(&errorIndication[indicationLen],
                    "line %-4d: %s (%s)\r\n", lineCount, cmdErrorName[cmdValidity], currentCmd);
            (*errorCmdNum)++;
        }

        if (cmdValidity == CMD_VALID && rc != OK)
        {
            if (indicationLen < ERROR_INDICATION_LENGTH)
                indicationLen += sprintf(&errorIndication[indicationLen],
                    "line %-4d: %s (%s)\r\n", lineCount, "command process failure", currentCmd);
            (*errorCmdNum)++;
        }

        cliCmdPoolClear(psCmdPool);
    }
    rc = OK;

exit_label:
    return rc;
}

/* text must be zero-ended */
int cliTextConfigLoad(ENV_t *pstEnv, char *filename)
{
    int errorCmdNum = 0;
    int rc;
    char *text = NULL;
    long length;

    vosMutexTake(&g_pstTextConfigLock);

    g_ucIsLoadingConfig = TRUE;
    length = vosFileRead(filename, &text);
    rc = cliTextConfigExec(text, length, &errorCmdNum);

    if (errorCmdNum)
    {/*modified by huangmingjian 2012/09/07 for EPN104QID0018*/  
     //   vosPrintf(pstEnv->nWriteFd, "\r\n%d errors found in configuration file:\r\n", errorCmdNum);
     //   vosPrintf(pstEnv->nWriteFd, "%s", errorIndication);
    }

    vosFree(text);
    g_ucIsLoadingConfig = FALSE;

    vosMutexGive(&g_pstTextConfigLock);

    return rc;
}

int cliTextConfigIsLoading(ENV_t *pstEnv)
{
    return g_ucIsLoadingConfig;
}

void cliTextConfigSave()
{
    int nFd;

    vosMutexTake(&g_pstTextConfigLock);

	nFd = open(DEFAULT_CONFIG_FILE, O_CREAT | O_RDWR | O_TRUNC, 0);
    if (ERROR == nFd)
    {
        goto exit_label;
    }

	odmQosConfigDump(nFd,0,1);
	vosPrintf(nFd, "!\r\n");
	vosPrintf(nFd, "end\r\n");

	close(nFd);

exit_label:
    vosMutexGive(&g_pstTextConfigLock);
}

