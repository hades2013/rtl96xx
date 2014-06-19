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
* FILENAME:  cli_cmd_user.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_cmd_user.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "cli.h"
#include "cli_cmd_list.h"


STATUS cliCmdUserAdd(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    UINT32 ret = NO_ERROR;

	if (NULL == pstEnv || NULL == pstPara) 
	{
		return ERROR;
	}
	
    if (strlen(pstPara[0].p) > 32)
    {
        vosPrintf(pstEnv->nWriteFd, "User name too long[%d]!\r\n", strlen(pstPara[0].p));
        return ERROR;
    }

    if (strlen(pstPara[1].p) > 32)
    {
        vosPrintf(pstEnv->nWriteFd, "Password too long[%d]!\r\n", strlen(pstPara[1].p));
        return ERROR;
    }
    
    ret = odmUserAddWithAllParams(
        pstPara[0].p, 
        pstPara[1].p,
        ENABLED,
        ACCESS_LEVEL_ADMINISTRATION,
        ENGLISH,
        NULL);

    switch (ret)
    {
    case NO_ERROR:
        return OK;
    case ERR_DUPLICATE_ITEM:
        vosPrintf(pstEnv->nWriteFd, "Input duplicate user name!\r\n");
        break;
    case ERR_EXCEED_MAXIMUM:
        vosPrintf(pstEnv->nWriteFd, "Exceed maximum user number!\r\n");
        break;
    case ERR_DATABASE_FAILED:
        vosPrintf(pstEnv->nWriteFd, "Configuration database error!\r\n");
        break;
    default:
        vosPrintf(pstEnv->nWriteFd, "Invalid input!\r\n");
        break;
    }
    return ERROR;
}

STATUS cliCmdUserSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    UINT32 ret = NO_ERROR;

	if (NULL == pstEnv || NULL == pstPara) 
	{
		return ERROR;
	}
	
    if (strlen(pstPara[0].p) > 32)
    {
        vosPrintf(pstEnv->nWriteFd, "User name too long[%d]!\r\n", strlen(pstPara[0].p));
        return ERROR;
    }

    if (NO_ERROR != odmUserExistCheck(pstPara[0].p))
    {
        vosPrintf(pstEnv->nWriteFd, "User name [%s] does not exist!\r\n", pstPara[0].p);
        return ERROR;
    }

    if (strlen(pstPara[1].p) > 32)
    {
        vosPrintf(pstEnv->nWriteFd, "Password too long[%d]!\r\n", strlen(pstPara[1].p));
        return ERROR;
    }
    
    ret = odmUserPasswordSet(
        pstPara[0].p, 
        pstPara[1].p,
        NULL);

    if (NO_ERROR != ret)
    {
        vosPrintf(pstEnv->nWriteFd, "Set user name [%s] password [%s] failed!\r\n", pstPara[0].p, pstPara[1].p);
        return ERROR;
    }

    return OK;
}

STATUS cliCmdUserDelete(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    UINT32 ret = NO_ERROR;

	if (NULL == pstEnv || NULL == pstPara) 
	{
		return ERROR;
	}
	
    if (strlen(pstPara[0].p) > 32)
    {
        vosPrintf(pstEnv->nWriteFd, "User name too long[%d]!\r\n", strlen(pstPara[0].p));
        return ERROR;
    }

    if (NO_ERROR != odmUserExistCheck(pstPara[0].p))
    {
        vosPrintf(pstEnv->nWriteFd, "User name [%s] does not exist!\r\n", pstPara[0].p);
        return ERROR;
    }

    ret = odmUserDelete(pstPara[0].p, NULL);

    if (NO_ERROR != ret)
    {
        vosPrintf(pstEnv->nWriteFd, "Delete user name [%s] failed!\r\n", pstPara[0].p);
        return ERROR;
    }

    return OK;
}

STATUS cliCmdUserShow(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
	cliShowStart(pstEnv);
    odmUserShow(pstEnv->nWriteFd);
    cliShowEnd(pstEnv);
	
	return OK;
}

