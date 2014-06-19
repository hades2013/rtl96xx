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
* FILENAME:  cli_engine.c
*
* DESCRIPTION:
*   cli module engine
*
* Date Created: Aug 05, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_engine.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "cli.h"
#include "cli_cmd_list.h"

typedef struct CLI_CHILD_TASK_ARG_s
{
    FUNCPTR fnProcess;
    ENV_t *pstEnv;
    PARA_TABLE_t *psPara;
}CLI_CHILD_TASK_ARG_t;

extern unsigned int *g_punArrayOfCliMode;
extern CMD_KEY_PARA_LINK_t *g_psRootOfForest;

static unsigned short g_usSeqNo = 0; /* task sequence number */
static char g_acChildTaskName[STR_SIZE_32_BYTES];
static char g_acSpecialChar[] = {'@', 0xFF};
static char g_cTerminalChar = 0x03;  /* Ctrl+C */

static void cliChildTask(CLI_CHILD_TASK_ARG_t *pstArg);

/*****************************************************************************
 *cliWordExtract -  based on direction, extract a word from
                    source string and return it by destination string
 *DESCRIPTION
 *
 *Input:ppSrcStr,ppRetWord,nDirection.
            ppSrcStr   : address of the pointer that points to
                           the source string
            pDestStr  : address of the pointer that points to
                           the return string
            nDirection : if direction==FORWARD, extract a word forward
                         if direction==BACKWARD,extract a word backward

 *Output:ppSrcStr,pDestStr.
 *Return:STATUS.
         OK     ---If success.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliWordExtract(char **ppSrcStr, char *pDestStr, int nDirection)
{
    int  nLen = 0;
    char achChar[MAX_POOL_LENGTH+2];
    char *pchChar = NULL;
    char *pchSrcStringHead = NULL;

    if (NULL == ppSrcStr || NULL == pDestStr || NULL == *ppSrcStr)
    {
        return ERROR;
    }
    if (!(FORWARD == nDirection || BACKWARD == nDirection))
    {
        return ERROR;
    }
    pchChar = achChar;
    pchSrcStringHead = *ppSrcStr ;

    while ((' ' == **ppSrcStr) && (*ppSrcStr  >=  pchSrcStringHead))
    {
        (*ppSrcStr)+= nDirection;
    }
    if (('|' == **ppSrcStr)||('[' == **ppSrcStr)
            ||(']' == **ppSrcStr)||('{' == **ppSrcStr)||('}' == **ppSrcStr))
    {
        pchChar[0] = **ppSrcStr;
        pchChar[1] = '\0';
        (*ppSrcStr)++;
    }
    else
    {
        /* Maybe *s>=head will leads to something wrong in some system,
        its purpose is to be away from out of boundary */

        while ((**ppSrcStr != ' ')&&(**ppSrcStr != '|')&&(**ppSrcStr != '[')
                &&(**ppSrcStr != ']')&&(**ppSrcStr != '{')&&(**ppSrcStr != '}')
                &&(**ppSrcStr != '\0')&&(*ppSrcStr>=pchSrcStringHead))
        {
            if ('"' == **ppSrcStr)
            {
                (*ppSrcStr)++;

                while (**ppSrcStr != '"' && **ppSrcStr != '\0')
                {
                    *pchChar++ = **ppSrcStr;
                    (*ppSrcStr) += nDirection;
                }

                if ('\0' == **ppSrcStr)
                {
                    return ERROR;
                }
                (*ppSrcStr)++;
                break;
            }

            *pchChar++ = **ppSrcStr;
            (*ppSrcStr) += nDirection;
        }

        *pchChar = '\0';

        if (*ppSrcStr < pchSrcStringHead)
        {
            *ppSrcStr = pchSrcStringHead;
        }
    }

    nLen = strlen(achChar);

    if (0 == nLen)
    {
        return ERROR;
    }
    else
    {
        strcpy(pDestStr,achChar);
    }
    return OK;
}

/*****************************************************************************
 *cliTreeNodeMatch -  To find the match node.
 *DESCRIPTION
 *
 *Input:psTreeRoot,pchWord,ppsMatchNode.
 *Output:ppsMatchNode.
 *Return:STATUS.
              OK   --if successfully.
              ERROR--failly.
 ****************************************************************************/
static BOOL bFinded = FALSE;

STATUS cliTreeNodeMatch(CMD_KEY_PARA_LINK_t *psTreeRoot,
                         char *pchWord, CMD_KEY_PARA_LINK_t **ppsMatchNode)
{
    int   nWordLen = 0;
    int   nIsWordMatch = ERROR;
    int   nIsRightMatch = ERROR;
    int   nIsNextMatch = ERROR;

    if (NULL == psTreeRoot || NULL == ppsMatchNode)
    {
        return ERROR;
    }
    switch(psTreeRoot->type)
    {
    case NT_KEY:
        nWordLen = strlen(pchWord);
        if (0 == nWordLen)
        {
            return ERROR;
        }
        if (strcmp(psTreeRoot->key,pchWord) == 0)
        {
            nIsWordMatch = 0;
        }

        if (0 == nIsWordMatch && FALSE == bFinded)
        {
            *ppsMatchNode = psTreeRoot;
             return OK;
        }

        if (psTreeRoot->right)
        {
            nIsRightMatch = cliTreeNodeMatch(psTreeRoot->right,pchWord,ppsMatchNode);
        }
        if (0 == nIsRightMatch && FALSE == bFinded)
        {
            *ppsMatchNode = psTreeRoot->right;
            return OK;
        }

        if (psTreeRoot->next)
        {
            nIsNextMatch = cliTreeNodeMatch (psTreeRoot->next,pchWord,ppsMatchNode);
        }
        if (0 == nIsNextMatch && FALSE == bFinded)
        {
            *ppsMatchNode = psTreeRoot->next;
            return OK;
        }
        break;
    case NT_PARAMETER:
        if (strcmp(psTreeRoot->key,pchWord) == 0)
        {
            nIsWordMatch = 0;
        }
        if (0 == nIsWordMatch && FALSE == bFinded)
        {
            *ppsMatchNode = psTreeRoot;
            bFinded = TRUE;
            return OK;
        }
        if (psTreeRoot->right)
        {
            nIsRightMatch = cliTreeNodeMatch(psTreeRoot->right, pchWord, ppsMatchNode);
        }
        if (0 == nIsRightMatch && FALSE == bFinded)
        {
            *ppsMatchNode = psTreeRoot;
            bFinded = TRUE;
            return OK;
        }
        break;
    case NT_VIRTUAL_REQUIRED:
        if (psTreeRoot->explain)
        {
            nIsWordMatch = cliTreeNodeMatch(psTreeRoot->explain,pchWord,ppsMatchNode);
        }
        if (0 == nIsWordMatch && FALSE == bFinded)
        {
            *ppsMatchNode = psTreeRoot->explain;
            return OK;
        }
        if (psTreeRoot->right)
        {
            nIsRightMatch = cliTreeNodeMatch(psTreeRoot->right,pchWord,ppsMatchNode);
        }
        if (0 == nIsRightMatch && FALSE == bFinded)
        {
            return OK;
        }
        if (psTreeRoot->next)
        {
            nIsNextMatch = cliTreeNodeMatch (psTreeRoot->next,pchWord,ppsMatchNode);
        }
        if (0 == nIsNextMatch && FALSE == bFinded)
        {
            *ppsMatchNode = psTreeRoot->next;
            return OK;
        }
        break;
    case NT_VIRTUAL_OPTIONAL:
        if (psTreeRoot->explain)
        {
            nIsWordMatch = cliTreeNodeMatch(psTreeRoot->explain,pchWord,ppsMatchNode);
        }
        if (0 == nIsWordMatch && FALSE == bFinded)
        {
            *ppsMatchNode = psTreeRoot->explain;
            return OK;
        }

        if (psTreeRoot->right)
        {
            nIsRightMatch = cliTreeNodeMatch(psTreeRoot->right,pchWord,ppsMatchNode);
        }
        if (0 == nIsRightMatch && FALSE == bFinded)
        {
            return OK;
        }

        if (psTreeRoot->next)
        {
            nIsNextMatch = cliTreeNodeMatch (psTreeRoot->next,pchWord,ppsMatchNode);
        }
        if (0 == nIsNextMatch && FALSE == bFinded)
        {
            *ppsMatchNode = psTreeRoot->next;
            return OK;
        }
        break;
    case NT_VIRTUAL_LOAD:
        nIsWordMatch = cliTreeNodeMatch (psTreeRoot->explain,pchWord,ppsMatchNode);
        if (0 == nIsWordMatch && FALSE == bFinded)
        {
            *ppsMatchNode = psTreeRoot->explain;
            return OK;
        }
        break;
    case NT_VIRTUAL_FUNCTION:
        nIsWordMatch = cliTreeNodeMatch (psTreeRoot->explain,pchWord,ppsMatchNode);
        if (0 == nIsRightMatch && FALSE == bFinded)
        {
            *ppsMatchNode = psTreeRoot->right;
            return OK;
        }

        if (psTreeRoot->right)
        {
            nIsRightMatch=cliTreeNodeMatch (psTreeRoot->right,pchWord,ppsMatchNode);
        }
        if (0 == nIsRightMatch && FALSE == bFinded)
        {
            *ppsMatchNode = psTreeRoot->right;
            return OK;
        }
        break;
    case NT_VIRTUAL_COMMAND:
        break;
    default:
        break;
    }
    return ERROR;
 }

/*****************************************************************************
 *cliExtraConditionAdd -  Add extra condiction to the node.
 *DESCRIPTION
 *
 *Input:psTreeRoot,ppchCommand,psKeyParaLnk.
 *Output:ppchCommand.
 *Return:N/A.
 ****************************************************************************/
void cliExtraConditionAdd(CMD_KEY_PARA_LINK_t *psTreeRoot, char **ppchCommand,
                         CMD_KEY_PARA_LINK_t *psKeyParaLnk)
{
    char    achWord[MAX_POOL_LENGTH+2];
    char    *pchWord = achWord;
    BOOL    bIsEnd = FALSE;
    EXTRA_CONDITION_t     *psExtraConditionTmp = NULL;
    EXTRA_CONDITION_t     *psExtraCondition = NULL;
    CMD_KEY_PARA_LINK_t *psMatchNode = NULL;


    cliWordExtract (ppchCommand, pchWord, FORWARD);
    psExtraCondition = (EXTRA_CONDITION_t *)vosAlloc(sizeof (EXTRA_CONDITION_t));
    memset(psExtraCondition,0,sizeof(EXTRA_CONDITION_t));
    cliWordExtract (ppchCommand, pchWord, FORWARD);
    cliWordExtract (ppchCommand, pchWord, FORWARD);
    cliTreeNodeMatch(psTreeRoot,pchWord,&psMatchNode);  /* Search the node */
    psExtraCondition->keyorparanode = psKeyParaLnk;
    psMatchNode->extra_condiction = psExtraCondition;
    cliWordExtract (ppchCommand, pchWord, FORWARD);
    cliWordExtract (ppchCommand, pchWord, FORWARD);
    while (FALSE == bIsEnd)
    {
        cliWordExtract (ppchCommand, pchWord, FORWARD);
        if (strcmp(pchWord,"EXTRA_TYPE_D") == 0)
        {
            psExtraCondition->para_type = EXTRA_TYPE_D;
            cliWordExtract (ppchCommand, pchWord, FORWARD);
            if (strcmp(pchWord,"EQ") == 0)
            {
               psExtraCondition->para.d_para.operate = EQ;
            }
            if (strcmp(pchWord,"NEQ") == 0)
            {
               psExtraCondition->para.d_para.operate = NEQ;
            }
            if (strcmp(pchWord,"GT") == 0)
            {
               psExtraCondition->para.d_para.operate = GT;
            }
            if (strcmp(pchWord,"LT") == 0)
            {
               psExtraCondition->para.d_para.operate = LT;
            }

            switch(psExtraCondition->para.d_para.operate)
            {
            case EQ:
                cliWordExtract (ppchCommand, pchWord, FORWARD);
                psExtraCondition->para.d_para.value = atoi(pchWord);
                cliWordExtract (ppchCommand, pchWord, FORWARD);
                break;
            case NEQ:
                cliWordExtract (ppchCommand, pchWord, FORWARD);
                psExtraCondition->para.d_para.value = atoi(pchWord);
                cliWordExtract (ppchCommand, pchWord, FORWARD);
                break;
            case GT:
                cliWordExtract (ppchCommand, pchWord, FORWARD);
                psExtraCondition->para.d_para.value = atoi(pchWord);
                cliWordExtract (ppchCommand, pchWord, FORWARD);
                break;
            case LT:
                cliWordExtract (ppchCommand, pchWord, FORWARD);
                psExtraCondition->para.d_para.value = atoi(pchWord);
                cliWordExtract (ppchCommand, pchWord, FORWARD);
                break;
            default:
                break;
            }
        }

        if (strcmp(pchWord,"EXTRA_TYPE_A") == 0)
        {
            psExtraCondition->para_type = EXTRA_TYPE_A;
            cliWordExtract (ppchCommand, pchWord, FORWARD);
            if (strcmp(pchWord,"RANGE") == 0)
            {
                psExtraCondition->para.a_para.operate = RANGE;
            }
            if (strcmp(pchWord,"EQ") == 0)
            {
                psExtraCondition->para.a_para.operate = EQ;
            }
            switch(psExtraCondition->para.a_para.operate)
            {
            case RANGE:
                cliWordExtract (ppchCommand, pchWord, FORWARD);
                psExtraCondition->para.a_para.min = atoi(pchWord);
                cliWordExtract (ppchCommand, pchWord, FORWARD);
                psExtraCondition->para.a_para.max = atoi(pchWord);
                break;
            default:
                break;
            }
        }

        if (strcmp(pchWord,"EXTRA_TYPE_STRING") == 0)
        {
            psExtraCondition->para_type = EXTRA_TYPE_STRING;
            cliWordExtract (ppchCommand, pchWord, FORWARD);
            if (strcmp(pchWord,"NEQ") == 0)
            {
                psExtraCondition->para.p_string.operate = NEQ;
            }
            if (strcmp(pchWord,"EQ") == 0)
            {
                psExtraCondition->para.p_string.operate = EQ;
            }
            switch(psExtraCondition->para.p_string.operate)
            {
            case NEQ:
                cliWordExtract (ppchCommand, pchWord, FORWARD);
                strcpy(pchWord, psExtraCondition->para.p_string.pString);
                cliWordExtract (ppchCommand, pchWord, FORWARD);
                break;
            case EQ:
                cliWordExtract (ppchCommand, pchWord, FORWARD);
                strcpy(pchWord, psExtraCondition->para.p_string.pString);
                cliWordExtract (ppchCommand, pchWord, FORWARD);
                break;
            default:
                break;
            }
        }

        cliWordExtract (ppchCommand, pchWord, FORWARD);
        if (strcmp(pchWord,"#") == 0)
        {
            bIsEnd = TRUE;
        }
        else
        {
            psExtraConditionTmp = (EXTRA_CONDITION_t *)vosAlloc(sizeof(EXTRA_CONDITION_t));
            memset(psExtraConditionTmp,0,sizeof(EXTRA_CONDITION_t));
            psExtraConditionTmp->keyorparanode = psKeyParaLnk;
            bIsEnd = FALSE;
            psExtraCondition->next = psExtraConditionTmp;
            psExtraCondition = psExtraConditionTmp;
        }
    }
    cliWordExtract (ppchCommand, pchWord, FORWARD);
}

/*****************************************************************************
 *cliTreeCreate -  Create a command link tree based on MODEL_INTF_INFO_t
                 return result by **root_of_tree.
 *DESCRIPTION
 *
 *Input:ppsTreeRoot,psCmdInfo.

            ppsTreeRoot: address of return pointer of tree

            psCmdInfo  : pointer of data struct which we used to
                         create the new tree
 *Output:ppsTreeRoot.
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliTreeCreate(CMD_KEY_PARA_LINK_t **ppsTreeRoot, MODEL_INTF_INFO_t *psCmdInfo)
{
    int  nFlag = ERROR;
    int  nParaNum = 1;     /* init to '1' */
    BOOL bIsNeedAdd = FALSE;
    BOOL bIsGetWordOk = FALSE;
    BOOL bIsBrotherNode = FALSE;
    char achWord[MAX_POOL_LENGTH+2];
    char *pchWord = achWord;
    char *pchCommand = NULL;
    PARA_LINK_t *psLink = NULL;
    PARA_LINK_t **ppsLast = NULL;
    CMD_KEY_PARA_LINK_t *psCurrent = NULL;
    CMD_KEY_PARA_LINK_t *psNewCmdKeyParaNode = NULL;
    CMD_KEY_PARA_LINK_t *psCmdKeyParaNodeLink = NULL;


    if (NULL == ppsTreeRoot || NULL == psCmdInfo)
    {
        return ERROR;
    }
    *ppsTreeRoot = NULL;
    psCurrent = *ppsTreeRoot;
    pchCommand = psCmdInfo->command;
    nFlag = cliParameterInfoGet(psCmdInfo,&psCmdKeyParaNodeLink);
    if (ERROR == nFlag)
    {
        return ERROR;
    }
    ppsLast = (PARA_LINK_t **)vosAlloc(sizeof(PARA_LINK_t *));
    /*Now create a basic Command tree without adjust parameter */
    nFlag = cliWordExtract (&pchCommand, pchWord, FORWARD);
    if (OK == nFlag)
    {
        bIsGetWordOk = TRUE;
    }
    else
    {
        return ERROR;
    }

    while (TRUE == bIsGetWordOk)
    {
        switch(pchWord[0])
        {
        case ']':
        case '}':
            if (psCurrent != NULL)
            {
                psCurrent = psCurrent->parent;
            }
            bIsBrotherNode = FALSE;
            break;
        case '|':
            bIsBrotherNode = TRUE;
            break;
        case '[':
            nFlag = cliTreeNodeCreate(NULL, &psNewCmdKeyParaNode);
            if (OK == nFlag)
            {
                bIsNeedAdd = TRUE;
            }
            else
            {
                return ERROR;
            }
            psNewCmdKeyParaNode->type = NT_VIRTUAL_OPTIONAL;
            break;
        case '{':
            nFlag = cliTreeNodeCreate(NULL, &psNewCmdKeyParaNode);
            if (OK == nFlag)
            {
                bIsNeedAdd = TRUE;
            }
            else
            {
                return ERROR;
            }
            psNewCmdKeyParaNode->type = NT_VIRTUAL_REQUIRED;

            while (' ' == *pchCommand)
            {
                pchCommand++;
            }
            if ('@' == *pchCommand)
            {
                cliWordExtract (&pchCommand,pchWord,FORWARD);
                cliExtraConditionAdd(*ppsTreeRoot,&pchCommand,psNewCmdKeyParaNode);
            }
            break;
        default:
            psNewCmdKeyParaNode = psCmdKeyParaNodeLink;
            if (psCmdKeyParaNodeLink != NULL)
            {
                psCmdKeyParaNodeLink = psCmdKeyParaNodeLink->next;
            }
            if (psNewCmdKeyParaNode != NULL)
            {
                psNewCmdKeyParaNode->next = NULL;
            }
            bIsNeedAdd = TRUE;
            break;
        }

        if (TRUE == bIsNeedAdd)
        {
            if (psCurrent != NULL)
            {
                switch (psCurrent->type)
                {
                case NT_VIRTUAL_OPTIONAL:
                case NT_VIRTUAL_REQUIRED:
                    if (TRUE == bIsBrotherNode)
                    {
                        while (psCurrent->pre)
                        {
                            psCurrent = psCurrent->pre;
                        }
                        psCurrent->right=psNewCmdKeyParaNode;
                        psNewCmdKeyParaNode->left = psCurrent;
                        psNewCmdKeyParaNode->parent = psCurrent->parent;
                    }
                    else
                    {
                        if (NULL == psCurrent->explain)
                        {
                            psCurrent->explain = psNewCmdKeyParaNode;
                            psNewCmdKeyParaNode->parent = psCurrent;
                        }
                        else
                        {
                            psCurrent->next = psNewCmdKeyParaNode;
                            psNewCmdKeyParaNode->pre = psCurrent;
                            psNewCmdKeyParaNode->parent = psCurrent->parent;
                        }
                    }
                    break;
                case NT_KEY:
                case NT_PARAMETER:
                    if (TRUE == bIsBrotherNode)
                    {
                        while (psCurrent->pre)
                        {
                            psCurrent = psCurrent->pre;
                        }
                        psCurrent->right = psNewCmdKeyParaNode;
                        psNewCmdKeyParaNode->left = psCurrent;
                        psNewCmdKeyParaNode->parent = psCurrent->parent;
                    }
                    else
                    {
                        psCurrent->next = psNewCmdKeyParaNode;
                        psNewCmdKeyParaNode->pre = psCurrent;
                        psNewCmdKeyParaNode->parent = psCurrent->parent;
                    }
                    break;
                default:
                    break;
                }/*switch*/
            }/*if !NULL*/
            else
            {
                *ppsTreeRoot = psNewCmdKeyParaNode;
            }
            psCurrent = psNewCmdKeyParaNode;
            psNewCmdKeyParaNode->exec_mode = psCmdInfo->exec_mode;   /*  do more than once !!!! */
            psNewCmdKeyParaNode = NULL;
            bIsBrotherNode = FALSE;
        }/*if bIsNeedAdd==0*/

        nFlag = cliWordExtract(&pchCommand, pchWord, FORWARD);
        bIsGetWordOk = (OK == nFlag)?TRUE:FALSE;
        bIsNeedAdd = FALSE;
    }/*while bIsGetWordOk*/

    nFlag = cliTreeNodeCreate(NULL, &psNewCmdKeyParaNode);
    if (ERROR == nFlag)
    {
        return ERROR;
    }
    psCurrent->next = psNewCmdKeyParaNode;
    psNewCmdKeyParaNode->pre = psCurrent;
    psNewCmdKeyParaNode->type = NT_VIRTUAL_COMMAND;
    psNewCmdKeyParaNode->func_info = (FUNC_INFO_t *)vosAlloc(sizeof(FUNC_INFO_t));
    if (NULL == psNewCmdKeyParaNode->func_info)
    {
        return ERROR;
    }
    psNewCmdKeyParaNode->func_info->function_point = psCmdInfo->p;
    psNewCmdKeyParaNode->func_info->is_task = psCmdInfo->is_task;
    psNewCmdKeyParaNode->exec_mode = psCmdInfo->exec_mode;
    psNewCmdKeyParaNode->exec_sub_mode = psCmdInfo->exec_sub_mode;

	psNewCmdKeyParaNode->access_level = psCmdInfo->access_level;
    /*Adjust basic tree, modify next pointer and number parameter*/
    nFlag = cliTreeParameterAdjust(*ppsTreeRoot, psNewCmdKeyParaNode , &nParaNum,&psLink,&ppsLast);
    if (ERROR == nFlag)
    {
        return ERROR;
    }
    psNewCmdKeyParaNode->func_info->parameter = psLink;
    return OK;
}


/*****************************************************************************
 *cliTreeNodeCreate -  Create a command node with key fill with word
                 return result by rt.
 *DESCRIPTION
 *
 *Input:pchWord,ppsNodeToCreat.
            pchWord : string to fill node's key
 *Output:ppsNodeToCreat.
            ppsNodeToCreat  : address of pointer that points to return node
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliTreeNodeCreate(char *pchWord, CMD_KEY_PARA_LINK_t **ppsNodeToCreat)
{
    CMD_KEY_PARA_LINK_t *psNode = NULL; /* Template node */


    if (NULL == ppsNodeToCreat)
    {
        return ERROR;
    }
    psNode = (CMD_KEY_PARA_LINK_t *)vosAlloc(sizeof(CMD_KEY_PARA_LINK_t));
    if (NULL == psNode)
    {
        return ERROR;
    }
    psNode->description = NULL;
    psNode->exec_mode = 0;
    psNode->exec_sub_mode = 0;
	psNode->access_level = ACCESS_LEVEL_GUEST;
    psNode->visualable = TRUE;
    psNode->explain = NULL;
    psNode->func_info = NULL;
    if (pchWord != NULL)
    {
        if ((pchWord[0] != '[') && (pchWord[0] != '{'))
        {
            psNode->key = pchWord;
        }
        else
        {
            psNode->key = NULL;
        }
    }
    else
    {
        psNode->key = NULL;
    }

    psNode->keyword_position = 0;
    psNode->left = NULL;
    psNode->next = NULL;
    psNode->parameter = NULL;
    psNode->extra_condiction = NULL;
    psNode->parameter_postion = 0;
    psNode->parent = NULL;
    psNode->pre = NULL;
    psNode->right = NULL;
    psNode->type = 0;
    *ppsNodeToCreat = psNode;  /* *ppsNodeToCreat point to the newer node */
    psNode = NULL;
    return OK;
}

/*****************************************************************************
 *cliDescriptionParse - Parse the command description.

 *DESCRIPTION
 *
 *Input:ppchDescription.
 *Output:ppchDescription.
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS cliDescriptionParse(char **ppchDescription, char *pchKey, char *pchDesc,
                         int *pnParaType)
{
    int  nIndex = 0;

    if ((NULL == ppchDescription) || (NULL == *ppchDescription))
    {
        return ERROR;
    }
    if ('{' == **ppchDescription)
    {
        (*ppchDescription)++;
    }
    cliStrLeftTrim(ppchDescription);
    cliStrRightTrim(*ppchDescription);
    nIndex = 0;
    while (**ppchDescription != ',')
    {
        pchKey[nIndex++] = **ppchDescription;
        (*ppchDescription)++;
    }
    while (nIndex && ' ' == pchKey[nIndex-1])
    {
        nIndex--;
    }
    pchKey[nIndex] = '\0';
    (*ppchDescription)++;
    cliStrLeftTrim(ppchDescription);
    cliStrRightTrim(*ppchDescription);
    if ('1' == **ppchDescription)
    {
        *pnParaType = NT_PARAMETER;
    }
    else
    {
        *pnParaType = NT_KEY;
    }
    (*ppchDescription) += 2;
    nIndex = 0;
    cliStrLeftTrim(ppchDescription);
    cliStrRightTrim(*ppchDescription);
    while (**ppchDescription != '}')
    {
        pchDesc[nIndex++] = **ppchDescription;
        (*ppchDescription)++;
    }
    while (**ppchDescription != '{' && **ppchDescription != '\0')
    {
        (*ppchDescription)++;
    }
    while (nIndex && pchDesc[nIndex-1] == ' ')
    {
        nIndex--;
    }
    pchDesc[nIndex] = '\0';

	return OK;
}

/*****************************************************************************
 *cliParameterTypeGet - Return the type of parameter .

 *DESCRIPTION
 *
 *Input:pchParameter,pnParaType.
 *Output:pnParaType.
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS cliParameterTypeGet(char *pchParameter, int *pnParaType)
{
    while (TRUE)
    {
        if ((strcmp(pchParameter, "CLI_WORD")) == 0)
        {
            *pnParaType = CLI_WORD;
            break;
        }
        if ((strcmp(pchParameter, "CLI_UINT")) == 0)
        {
            *pnParaType = CLI_UINT;
            break;
        }
        if ((strcmp(pchParameter, "CLI_IPDOTADDR")) == 0)
        {
            *pnParaType = CLI_IPDOTADDR;
            break;
        }
        if ((strcmp(pchParameter, "CLI_IPDOTMASK")) == 0)
        {
            *pnParaType = CLI_IPDOTMASK;
            break;
        }

        if ((strcmp(pchParameter, "CLI_MACADDR")) == 0)
        {
            *pnParaType = CLI_MACADDR;
            break;
        }

        if ((strcmp(pchParameter, "CLI_IPINTMASK")) == 0)
        {
            *pnParaType = CLI_IPINTMASK;
            break;
        }
        if ((strcmp(pchParameter, "CLI_INTEGER")) == 0)
        {
            *pnParaType = CLI_INTEGER;
            break;
        }
        if ((strcmp(pchParameter, "CLI_STRING")) == 0)
        {
            *pnParaType = CLI_STRING;
            break;
        }
        if ((strcmp(pchParameter, "CLI_DATE")) == 0)
        {
            *pnParaType = CLI_DATE;
            break;
        }
        if ((strcmp(pchParameter, "CLI_TIME")) == 0)
        {
            *pnParaType = CLI_TIME;
            break;
        }
        if ((strcmp(pchParameter, "CLI_IPINTADDR")) == 0)
        {
            *pnParaType = CLI_IPINTADDR;
            break;
        }
        break;
    }
    return OK;
 }

/*****************************************************************************
 *cliParameterParse - Parse the command parameter.

 *DESCRIPTION
 *
 *Input:ppchParameter,pnValueMin,pnValueMax,pnValueDefault,pnParaType.
 *Output:ppchParameter,pnValueMin,pnValueMax,pnValueDefault,pnParaType.
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS cliParameterParse(char **ppchParameter, int *pnValueMin, int *pnValueMax,
                       int *pnValueDefault, int *pnParaType)
{
    int  nIndex = 0;
    char achPara[128];
    char achValueMin[128];
    char achValueMax[128];
    char achValueDef[128];


    /*ignore ppchParameter name and get ppchParameter type*/
    while (**ppchParameter != ',')
    {
        (*ppchParameter)++;
    }
    (*ppchParameter)++;
    while (' ' == **ppchParameter)
    {
        (*ppchParameter)++;
    }
    nIndex = 0;
    while (**ppchParameter != ',')
    {
        achPara[nIndex++] = **ppchParameter;
        (*ppchParameter)++;
    }
    while (nIndex && ' ' == achPara[nIndex-1])
    {
        nIndex--;
    }
    achPara[nIndex] = '\0';
    if (ERROR == cliParameterTypeGet(achPara, pnParaType))
    {
        return ERROR;
    }

    nIndex = 0;
    (*ppchParameter)++;
    while (' ' == **ppchParameter)
    {
        (*ppchParameter)++;
    }
    while (**ppchParameter != ',')
    {
        achValueMin[nIndex++] = **ppchParameter;
        (*ppchParameter)++;
    }
    while (nIndex && achValueMin[nIndex-1] == ' ')
    {
        nIndex--;
    }
    achValueMin[nIndex] = '\0';
   /* *pnValueMin = atoi(achValueMin);
    if (ERROR == *pnValueDefault)
    {
        return ERROR;
    }
    */
    nIndex = 0;
    (*ppchParameter)++;
    while (' ' == **ppchParameter)
    {
        (*ppchParameter)++;
    }
    while (**ppchParameter != ',')
    {
        achValueMax[nIndex++] = **ppchParameter;
        (*ppchParameter)++;
    }
    while (nIndex && achValueMax[nIndex-1] == ' ')
    {
        nIndex--;
    }
    achValueMax[nIndex] = '\0';
  /*  *pnValueMax = atoi(achValueMax);
    if (ERROR == *pnValueDefault)
    {
        return ERROR;
    }
   */
    nIndex = 0;
    (*ppchParameter)++;
    while (' ' == **ppchParameter)
    {
        (*ppchParameter)++;
    }
    while (**ppchParameter != '}')
    {
        achValueDef[nIndex++] = **ppchParameter;
        (*ppchParameter)++;
    }
    while (nIndex && achValueDef[nIndex-1] == ' ')
    {
        nIndex--;
    }
    achValueDef[nIndex] = '\0';
    while (**ppchParameter != '{' && **ppchParameter != '\0')
    {
        (*ppchParameter)++;
    }
 /*   *pnValueDefault = atoi(achValueDef);
    if (ERROR == *pnValueDefault)
    {
        return ERROR;
    }
  */
    switch(*pnParaType)
    {
    case CLI_WORD:
    case CLI_MACADDR:
    case CLI_DATE:
    case CLI_TIME:
        *pnValueDefault = atoi(achValueDef);  /* String's Length */
        break;
    case CLI_STRING:
        *pnValueDefault = atoi(achValueDef); /* String's Length */
        break;
    case CLI_INTEGER:
        *pnValueMin = vosStrToInt(achValueMin, NULL);
        *pnValueMax = vosStrToInt(achValueMax, NULL);
        *pnValueDefault = vosStrToInt(achValueDef, NULL);
        break;
    case CLI_UINT:
        *pnValueMin = vosStrToUInt(achValueMin, NULL);
        *pnValueMax = vosStrToUInt(achValueMax, NULL);
        *pnValueDefault = vosStrToUInt(achValueDef, NULL);
        break;
    case CLI_IPDOTADDR:
    case CLI_IPDOTMASK:
        *pnValueMin = ntohl(inet_addr(achValueMin));
        *pnValueMax = ntohl(inet_addr(achValueMax));
        *pnValueDefault = ntohl(inet_addr(achValueDef));
        break;

    default:
        *pnValueMin = vosStrToUInt(achValueMin, NULL);
        *pnValueMax = vosStrToUInt(achValueMax, NULL);
        *pnValueDefault = vosStrToUInt(achValueDef, NULL);
        break;
    }

    return OK;
}

/*****************************************************************************
 *cliParameterInfoGet -  Get infomation from MODEL_INTF_INFO_t data struct
                     and store them into CMD_KEY_PARA_LINK_t vessels and
                     PARA_LINK_t vessels return result by
                     node_link_info_of_cmd
 *DESCRIPTION
 *
 *Input:psCmdInfo.
            psCmdInfo : information from MODEL_INTF_INFO_t
 *Output:ppsCmdNodeLnk.
            ppsCmdNodeLnk : address of pointer that returns parameter
                                information
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliParameterInfoGet(MODEL_INTF_INFO_t *psCmdInfo,CMD_KEY_PARA_LINK_t **ppsCmdNodeLnk)
{
    int   nValueMin = 0;
    int   nValueMax = 0;
    int   nValueDefault = 0;
    unsigned int nKeyType = 0;
    unsigned int nParaType = 0;
    char  achKey[128];
    char  achDesc[128];
    char  *pchKey = NULL;
    char  *pchDesc = NULL;
    char  *pchParameter = NULL;
    char  *pchDescription = NULL;
    CMD_KEY_PARA_LINK_t *psLast = NULL;
    CMD_KEY_PARA_LINK_t *psNewCmdKeyParaNode = NULL;


    if ((NULL == psCmdInfo) || (NULL == ppsCmdNodeLnk))
    {
        return ERROR;
    }

    *ppsCmdNodeLnk = NULL;
    pchDescription = psCmdInfo->description;
    pchParameter = psCmdInfo->parameter;
    cliStrLeftTrim(&pchDescription);
    cliStrLeftTrim(&pchParameter);
    cliStrRightTrim(pchDescription);
    cliStrRightTrim(pchParameter);
    if ('\0' == *pchDescription)
    {
        return ERROR;
    }

    while (*pchDescription)
    {
        cliDescriptionParse(&pchDescription, achKey, achDesc, &nKeyType);
        if (ERROR == cliTreeNodeCreate(NULL,&psNewCmdKeyParaNode)) /*create a node*/
        {
            return ERROR;
        }
        pchKey = (char *)vosAlloc(strlen(achKey)+sizeof(char));
        if (NULL == pchKey)
        {
            vosFree(psNewCmdKeyParaNode);
            psNewCmdKeyParaNode = NULL;
            return ERROR;
        }
        pchDesc = (char *)vosAlloc(strlen(achDesc)+sizeof(char));
        if (NULL == pchDesc)
        {
            vosFree(pchKey);
            vosFree(psNewCmdKeyParaNode);
            pchKey=NULL;
            psNewCmdKeyParaNode = NULL;
            return ERROR;
        }
        strcpy(pchKey,achKey);
        strcpy(pchDesc,achDesc);
        psNewCmdKeyParaNode->key = pchKey;
        psNewCmdKeyParaNode->description = pchDesc;
        psNewCmdKeyParaNode->type = nKeyType;

        /*get psCmdInfo from pchParameter*/
        if (NT_PARAMETER == nKeyType)
        {
            cliParameterParse(&pchParameter, &nValueMin, &nValueMax,
                            &nValueDefault, &nParaType);
            psNewCmdKeyParaNode->parameter
            = (PARA_INFO_t *)vosAlloc(sizeof(PARA_INFO_t));
            if (NULL == psNewCmdKeyParaNode->parameter)
            {
                return ERROR;
            }
            psNewCmdKeyParaNode->parameter->type = nParaType;
            switch(nParaType)
            {
            case CLI_WORD:
                  case CLI_MACADDR:
                  case CLI_DATE:
                  case CLI_TIME:
                psNewCmdKeyParaNode->parameter->fail = nValueDefault;
                psNewCmdKeyParaNode->parameter->max = NULL;
                psNewCmdKeyParaNode->parameter->min = NULL;
                break;
            case CLI_STRING:
                psNewCmdKeyParaNode->parameter->fail = nValueDefault;
                psNewCmdKeyParaNode->parameter->max = NULL;
                psNewCmdKeyParaNode->parameter->min = NULL;
                break;
            case CLI_INTEGER:
            case CLI_UINT:
                psNewCmdKeyParaNode->parameter->min = nValueMin;
                psNewCmdKeyParaNode->parameter->max = nValueMax;
                psNewCmdKeyParaNode->parameter->fail = nValueDefault;
                break;
            default:
                psNewCmdKeyParaNode->parameter->min = nValueMin;
                psNewCmdKeyParaNode->parameter->max = nValueMax;
                psNewCmdKeyParaNode->parameter->fail = nValueDefault;
                break;
            }
        }
        /*add node to return link table*/
        psNewCmdKeyParaNode->exec_mode = psCmdInfo->exec_mode;
        psNewCmdKeyParaNode->exec_sub_mode = psCmdInfo->exec_sub_mode;

		/* Add access control */
		psNewCmdKeyParaNode->access_level = psCmdInfo->access_level;
        if (NULL == *ppsCmdNodeLnk)
        {
            *ppsCmdNodeLnk = psNewCmdKeyParaNode;
        }
        if (psLast != NULL)
        {
            psLast->next = psNewCmdKeyParaNode;
        }
        psLast = psNewCmdKeyParaNode;
    }/*while*/
    return OK;
}

/*****************************************************************************
 *cliTreeParameterAdjust -  Adjust the parameter of command-tree.
 *DESCRIPTION
 *
 *Input:psTreeRoot,psCmdNode,pnParaPos,ppsParaLnkRoot,pppsLast.

 *Output:
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliTreeParameterAdjust(CMD_KEY_PARA_LINK_t *psTreeRoot,
                        CMD_KEY_PARA_LINK_t *psCmdNode,
                        int *pnParaPos, PARA_LINK_t **ppsParaLnkRoot,
                        PARA_LINK_t ***pppsLast)
{
    int  nCount = 0;
    PARA_LINK_t *psParaLnkTmp = NULL; /* create a template PARA_LINK_t to it */
    CMD_KEY_PARA_LINK_t   *psTreeRootTemp = NULL;


    if (*pnParaPos < 1 || NULL == ppsParaLnkRoot || NULL == pppsLast)
    {
        return ERROR;
    }
    if (NULL == psTreeRoot || psTreeRoot == psCmdNode)
    {
        return OK;
    }
    psTreeRootTemp = psTreeRoot;
    switch (psTreeRoot->type)
    {
    case NT_KEY:
        psTreeRoot->parameter_postion = *pnParaPos;/* ??  need parameter ?? */
        if (psTreeRoot->next)
        {
            cliTreeParameterAdjust(psTreeRoot->next, psCmdNode, pnParaPos,
                            ppsParaLnkRoot, pppsLast);
        }
        if (psTreeRoot->right)
        {
            cliTreeParameterAdjust(psTreeRoot->right, psCmdNode, pnParaPos,
                            ppsParaLnkRoot, pppsLast);
        }
        break;
    case NT_PARAMETER:
        psTreeRoot->parameter_postion = *pnParaPos;
        psParaLnkTmp = (PARA_LINK_t *)vosAlloc(sizeof(PARA_LINK_t));
        psParaLnkTmp->p = psTreeRoot->parameter;
        psParaLnkTmp->position = *pnParaPos;
        *pnParaPos = *pnParaPos+1;
        psParaLnkTmp->next = NULL;
        if (NULL == *ppsParaLnkRoot)
        {
            *ppsParaLnkRoot = psParaLnkTmp;
        }
        (**pppsLast) = psParaLnkTmp;
        (*pppsLast) = &(psParaLnkTmp->next);
        if (psTreeRoot->next)
        {
            cliTreeParameterAdjust(psTreeRoot->next, psCmdNode, pnParaPos,
                            ppsParaLnkRoot, pppsLast);
        }
        if (psTreeRoot->right)
        {
            cliTreeParameterAdjust(psTreeRoot->right, psCmdNode, pnParaPos,
                            ppsParaLnkRoot, pppsLast);
        }
        break;
    case NT_VIRTUAL_REQUIRED:
    case NT_VIRTUAL_OPTIONAL:
        psTreeRoot->parameter_postion = *pnParaPos;
        psTreeRootTemp = psTreeRootTemp->explain;
        nCount = 0;
        /*  sum up the number of options ( '|' ) in this node */
        while ( psTreeRootTemp )
        {
            nCount++;
            psTreeRootTemp->keyword_position = nCount;
            psTreeRootTemp = psTreeRootTemp->right;
        }
        psTreeRoot->parameter = (PARA_INFO_t *)vosAlloc(sizeof(PARA_INFO_t));
        if (NULL == psTreeRoot->parameter)
        {
            return ERROR;
        }
        psTreeRoot->parameter->fail = 0;
        psTreeRoot->parameter->max = nCount;
        psTreeRoot->parameter->min = 0;
        psTreeRoot->parameter->type = CLI_INTEGER;
        psParaLnkTmp = (PARA_LINK_t *)vosAlloc(sizeof(PARA_LINK_t));
        if (NULL == psParaLnkTmp)
        {
            return ERROR;
        }
        psParaLnkTmp->p = psTreeRoot->parameter;
        psParaLnkTmp->next = NULL;
        psParaLnkTmp->position = *pnParaPos;
        *pnParaPos = *pnParaPos+1;
        if (NULL == *ppsParaLnkRoot)
        {
            *ppsParaLnkRoot = psParaLnkTmp;
        }
        (**pppsLast) = psParaLnkTmp;
        (*pppsLast) = &(psParaLnkTmp->next);

        cliTreeParameterAdjust (psTreeRoot->explain, psCmdNode, pnParaPos,
                         ppsParaLnkRoot, pppsLast);
        if (psTreeRoot->right)
        {
            cliTreeParameterAdjust(psTreeRoot->right, psCmdNode, pnParaPos,
                            ppsParaLnkRoot, pppsLast);
        }
        if (psTreeRoot->next)
        {
            cliTreeParameterAdjust(psTreeRoot->next, psCmdNode, pnParaPos,
                            ppsParaLnkRoot, pppsLast);
        }
        break;
    default:
        break;
    }
    return OK;
}


/*****************************************************************************
 *cliTreePointerAdjust -  Adjust pointers, each node point to its next node
                    and the latest node'type is NT_VIRTUAL_COMMAND.
 *DESCRIPTION
 *
 *Input:psTreeRoot.

 *Output:psTreeRoot.
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliTreePointerAdjust(CMD_KEY_PARA_LINK_t *psTreeRoot)
{
    int  nRelsult = OK;


    if (NULL == psTreeRoot)
    {
        return ERROR;
    }

    switch (psTreeRoot->type)
    {
    case NT_KEY:
    case NT_PARAMETER:
        if (NULL == psTreeRoot->next && psTreeRoot->parent)
        {
            if (psTreeRoot->parent->type != NT_VIRTUAL_FUNCTION)
            {
                psTreeRoot->next = psTreeRoot->parent->next;
            }
        }
        if (psTreeRoot->next)
        {
            cliTreePointerAdjust(psTreeRoot->next);
        }
        if (psTreeRoot->right)
        {
            cliTreePointerAdjust(psTreeRoot->right);
        }
        break;
    case NT_VIRTUAL_OPTIONAL:
    case NT_VIRTUAL_REQUIRED:
        if (NULL == psTreeRoot->next && psTreeRoot->parent)
        {
            if (psTreeRoot->parent->type != NT_VIRTUAL_FUNCTION)
            {
                psTreeRoot->next = psTreeRoot->parent->next;
            }
        }
        cliTreePointerAdjust(psTreeRoot->explain);
        if (psTreeRoot->right)
        {
            cliTreePointerAdjust(psTreeRoot->right);
        }
        if (psTreeRoot->next)
        {
            cliTreePointerAdjust(psTreeRoot->next);
        }
        break;
    case NT_VIRTUAL_LOAD:
        cliTreePointerAdjust(psTreeRoot->explain);
        break;
    case NT_VIRTUAL_FUNCTION:
        cliTreePointerAdjust(psTreeRoot->explain);
        if (psTreeRoot->right)
        {
            cliTreePointerAdjust(psTreeRoot->right);
        }
        break;
    case NT_VIRTUAL_COMMAND:
        break;
    default:
        nRelsult = ERROR;
        break;
    }
    return(nRelsult);
}

/*****************************************************************************
 *cliTreeNodeAdd -  Add a created tree to root , and also unite the tree
             if there is the same branch.
 *DESCRIPTION
 *
 *Input:ppsUnitTreeRoot,psCmdInfo.

 *Output:
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliTreeNodeAdd(CMD_KEY_PARA_LINK_t **ppsUnitTreeRoot, MODEL_INTF_INFO_t *psCmdInfo)
{
    int  nFlag = ERROR;
    char achWord[MAX_POOL_LENGTH+2];
    char *pchWord = achWord;
    char *pchCommand = NULL;
    BOOL  bIsMatch = FALSE;
    BOOL  bIsGetWordOk = FALSE;
    BOOL  bIsFindNodeToInsert = FALSE;
    BOOL  bIsNeedCreateVTFuncNode = FALSE;
    CMD_KEY_PARA_LINK_t *psTreeRoot = NULL;
    CMD_KEY_PARA_LINK_t *psFindedNode = NULL;
    CMD_KEY_PARA_LINK_t *psCurrentNode = NULL;
    CMD_KEY_PARA_LINK_t *psNewVirtualFuncNode = NULL;
    CMD_KEY_PARA_LINK_t *psNewVirtualLoadNode = NULL;


    /*check parameter and init temp parameters*/
    if ((NULL == ppsUnitTreeRoot) || (NULL == *ppsUnitTreeRoot)
        || (NULL == psCmdInfo))
    {
        return ERROR;
    }
    psCurrentNode = *ppsUnitTreeRoot;
    pchCommand = psCmdInfo->command;

    nFlag = cliTreeCreate (&psTreeRoot,psCmdInfo);
    if (ERROR == nFlag)
    {
        return ERROR;
    }
    nFlag = cliWordExtract (&pchCommand, pchWord, FORWARD);
    bIsGetWordOk = (OK == nFlag)?TRUE:FALSE;
    while (TRUE == bIsGetWordOk) /* if get pchCommand NOT end ,continue */
    {
        switch (pchWord[0])
        {
        case '[':
        case '{':
            pchCommand--;
            // *pchCommand = pchWord[0];/*complete [ ... ] and { ... } pair  */

            nFlag = cliTreeVirtualNodeMatch(psCurrentNode, &pchCommand,
                                          &psFindedNode, psCmdInfo->exec_mode);
            bIsMatch = (nFlag == OK) ? TRUE : FALSE;
            if (TRUE == bIsMatch)
            {
                psNewVirtualLoadNode = psTreeRoot;
                psTreeRoot = psTreeRoot->next;
                psNewVirtualLoadNode->next = NULL;
                cliTreeNodeFree(psNewVirtualLoadNode);
                psCurrentNode = psFindedNode->next;
            }
            else
            {
                nFlag = cliTreeNodeCreate(NULL,&psNewVirtualFuncNode);
                if (ERROR == nFlag)
                {
                    return ERROR;
                }
                switch (psCurrentNode->type)
                {
                case NT_VIRTUAL_LOAD:
                    psNewVirtualLoadNode = psCurrentNode->explain;
                    psCurrentNode->explain = psNewVirtualFuncNode;
                    psNewVirtualFuncNode->parent = psCurrentNode;
                    psNewVirtualFuncNode->right = psNewVirtualLoadNode;
                    psNewVirtualLoadNode->left = psNewVirtualFuncNode;
                    psNewVirtualFuncNode->type = NT_VIRTUAL_FUNCTION;
                    psNewVirtualFuncNode->explain = psTreeRoot;
                    psTreeRoot->parent = psNewVirtualFuncNode;
                    psTreeRoot->pre = NULL;
                    break;
                default:
                    /*
                      add virtual load node and a virtual function node
                      to place the psCurrentNode tree
                    */
                    nFlag = cliTreeNodeCreate(NULL,&psNewVirtualLoadNode);
                    if (ERROR == nFlag)
                    {
                        return ERROR;
                    }
                    psNewVirtualFuncNode->type = NT_VIRTUAL_LOAD;
                    if (NULL == psCurrentNode->pre)
                    {
                        *ppsUnitTreeRoot = psNewVirtualFuncNode;
                    }
                    else
                    {
                        psCurrentNode->pre->next = psNewVirtualFuncNode;
                        psNewVirtualFuncNode->pre = psCurrentNode->pre;
                    }
                    psNewVirtualLoadNode->type = NT_VIRTUAL_FUNCTION;
                    psNewVirtualFuncNode->explain = psNewVirtualLoadNode;
                    psNewVirtualLoadNode->parent = psNewVirtualFuncNode;
                   /* psNewVirtualFuncNode->explain = psCurrentNode;*/
                    psNewVirtualLoadNode->explain = psCurrentNode;
                   /* psCurrentNode->parent = psNewVirtualFuncNode;*/
                    psCurrentNode->parent = psNewVirtualLoadNode;
                    psCurrentNode->pre = NULL;

                    /*   add new node   */
                    nFlag = cliTreeNodeCreate(NULL,&psNewVirtualFuncNode);
                    if (ERROR == nFlag)
                    {
                        return ERROR;
                    }
                    psNewVirtualFuncNode->type = NT_VIRTUAL_FUNCTION;
                    psNewVirtualFuncNode->left = psNewVirtualLoadNode;
                    psNewVirtualLoadNode->right = psNewVirtualFuncNode;
                    psNewVirtualFuncNode->parent = psNewVirtualLoadNode->parent;
                    psNewVirtualFuncNode->explain = psTreeRoot;
                    psTreeRoot->pre = NULL;
                    psTreeRoot->parent = psNewVirtualFuncNode;
                    break;
                }/*switch type*/
            return OK;     /*  link to the end of tree and return */
            }
            break;
        /*   we get a normal pchWord   */
        default:
            switch (psCurrentNode->type)
            {
            case NT_KEY:
                if (0 == strcmp(pchWord,psCurrentNode->key))
                {
                    bIsMatch = TRUE;
                }
                else
                {
                    bIsMatch = FALSE;
                }
                if (TRUE == bIsMatch)
                {
                    psCurrentNode->exec_mode = psCurrentNode->exec_mode
                                               | psCmdInfo->exec_mode;
                    psCurrentNode->exec_sub_mode = psCurrentNode->exec_sub_mode
                                                   | psCmdInfo->exec_sub_mode;
					/* Add access control */
					psCurrentNode->access_level = psCmdInfo->access_level;
                    psNewVirtualLoadNode = psTreeRoot;
                    psTreeRoot = psTreeRoot->next;
                    psNewVirtualLoadNode->next = NULL;
                    cliTreeNodeFree (psNewVirtualLoadNode);
                    psCurrentNode = psCurrentNode->next;
                }
                else
                {
                    bIsNeedCreateVTFuncNode = TRUE;
                }
                break;
            case NT_PARAMETER:
                if (0 == strcmp(pchWord,psCurrentNode->key))
                {
                    bIsMatch = TRUE;
                }
                else
                {
                    bIsMatch = FALSE;
                }
                if (TRUE == bIsMatch)
                {
                    if (psCurrentNode->parameter->type == psTreeRoot->parameter->type)
                    {
                        if ((psCurrentNode->parameter->type & 0x03)
                            || (psCurrentNode->parameter->type & 0xf0))
                        {
                            if ((psCurrentNode->parameter->fail == psTreeRoot->parameter->fail)
                                && (psCurrentNode->parameter->max == psTreeRoot->parameter->max)
                                && (psCurrentNode->parameter->min == psTreeRoot->parameter->min))
                            {
                                psCurrentNode->exec_mode = psCurrentNode->exec_mode
                                                           | psCmdInfo->exec_mode;
                                psCurrentNode->exec_sub_mode = psCurrentNode->exec_sub_mode
                                                               | psCmdInfo->exec_sub_mode;
								/* Add access control */
								psCurrentNode->access_level = psCmdInfo->access_level;

                                psNewVirtualLoadNode = psTreeRoot;
                                psTreeRoot = psTreeRoot->next;
                                psNewVirtualLoadNode->next = NULL;
                                cliTreeNodeFree(psNewVirtualLoadNode);
                                psCurrentNode = psCurrentNode->next;
                            }
                        }
                        else
                        {
                            psCurrentNode->exec_mode = psCurrentNode->exec_mode
                                                       | psCmdInfo->exec_mode;
                            psCurrentNode->exec_sub_mode = psCurrentNode->exec_sub_mode
                                                           | psCmdInfo->exec_sub_mode;  /* need ? */

							/* Add access control */
							psCurrentNode->access_level = psCmdInfo->access_level;

                            psNewVirtualLoadNode = psTreeRoot;
                            psTreeRoot = psTreeRoot->next;
                            psNewVirtualLoadNode->next = NULL;
                            cliTreeNodeFree(psNewVirtualLoadNode);
                            psCurrentNode = psCurrentNode->next;
                        }
                    }
                    else
                    {
                        bIsNeedCreateVTFuncNode = TRUE;
                    }
                 }
                else
                {
                    bIsNeedCreateVTFuncNode = TRUE;
                }
                break;
            case NT_VIRTUAL_LOAD:
               /*psCurrentNode now is FUNC NODE*/
                psCurrentNode = psCurrentNode->explain;
                bIsFindNodeToInsert = FALSE;
                bIsMatch = FALSE;
                while (FALSE == bIsFindNodeToInsert)
                {
                    /* psNewVirtualLoadNode is point to the template NODE*/
                    psNewVirtualLoadNode = psCurrentNode->explain;
                    if (psNewVirtualLoadNode->type == psTreeRoot->type)
                    {
                        switch (psNewVirtualLoadNode->type)
                        {
                        case NT_KEY:
                            if (0 == strcmp(psTreeRoot->key,psNewVirtualLoadNode->key))
                            {
                                bIsMatch = TRUE;
                            }
                            else
                            {
                                bIsMatch = FALSE;
                            }
                            break;
                        case NT_PARAMETER:
                            nFlag = strcmp(psTreeRoot->key,psNewVirtualLoadNode->key);
                            bIsMatch = (OK == nFlag)?TRUE:FALSE;
                            if (TRUE == bIsMatch)
                            {
                                if ((psNewVirtualLoadNode->parameter->type & 0x03)
                                    || (psNewVirtualLoadNode->parameter->type & 0xf0))
                                {
                                    if ((psNewVirtualLoadNode->parameter->fail != psTreeRoot->parameter->fail)
                                        || (psNewVirtualLoadNode->parameter->max != psTreeRoot->parameter->max)
                                        || ( psNewVirtualLoadNode->parameter->min != psTreeRoot->parameter->min))
                                    {
                                        bIsMatch = FALSE;
                                    }
                                }
                            }
                            break;
                        default:
                            break;
                        }
                        if (FALSE == bIsMatch)
                        {
                            if ((psCurrentNode != NULL) && (psCurrentNode->right != NULL))
                            {
                                psCurrentNode = psCurrentNode->right;
                            }
                            else
                            {
                                bIsFindNodeToInsert = TRUE;
                            }
                        }
                    }
                    else
                    {
                        if ((psCurrentNode != NULL) && (psCurrentNode->right != NULL))
                        {
                            psCurrentNode = psCurrentNode->right;
                        }
                        else
                        {
                            bIsFindNodeToInsert = TRUE;
                        }
                    }

                    if (TRUE == bIsMatch)
                    {
                        bIsFindNodeToInsert = TRUE;
                    }
                }

                if (TRUE == bIsMatch)
                {
                    psNewVirtualLoadNode->exec_mode = psNewVirtualLoadNode->exec_mode | psCmdInfo->exec_mode;
                    psCurrentNode->exec_sub_mode = psCurrentNode->exec_sub_mode
                                                   | psCmdInfo->exec_sub_mode;  /* need ? */

					/* Add access control */
					psCurrentNode->access_level = psCmdInfo->access_level;
                    psCurrentNode = psNewVirtualLoadNode->next;
                    psNewVirtualLoadNode = psTreeRoot;
                    psTreeRoot = psTreeRoot->next;
                    psNewVirtualLoadNode->next = NULL;
                    cliTreeNodeFree(psNewVirtualLoadNode);
                }
                else
                {
                    nFlag = cliTreeNodeCreate(NULL,&psNewVirtualFuncNode);
                    if (ERROR == nFlag)
                    {
                        return ERROR;
                    }
                    psNewVirtualFuncNode->type = NT_VIRTUAL_FUNCTION;
                    psNewVirtualFuncNode->left = psCurrentNode;
                    psCurrentNode->right = psNewVirtualFuncNode;
                    psNewVirtualFuncNode->parent = psCurrentNode->parent;
                    psNewVirtualFuncNode->explain = psTreeRoot;
                    psTreeRoot->parent = psNewVirtualFuncNode;
                    psTreeRoot->pre = NULL;
                    return OK;
                }
                break;
            /*
              node is virtual optional or virtual required or virtual pchCommand
              add new virtual load and virtual function
             */
            default:
                bIsNeedCreateVTFuncNode = TRUE;
                break;
            }/*switch psCurrentNode->type*/

            if (TRUE == bIsNeedCreateVTFuncNode)
            {
                nFlag = cliTreeNodeCreate(NULL,&psNewVirtualFuncNode);
                if (ERROR == nFlag)
                {
                    return ERROR;
                }
                nFlag = cliTreeNodeCreate(NULL,&psNewVirtualLoadNode);
                if (ERROR == nFlag)
                {
                    return ERROR;
                }
                psNewVirtualFuncNode->type = NT_VIRTUAL_LOAD;
                if (NULL == psCurrentNode->pre)
                {
                    *ppsUnitTreeRoot = psNewVirtualFuncNode;
                }
                else
                {
                    psCurrentNode->pre->next = psNewVirtualFuncNode;
                    psNewVirtualFuncNode->pre = psCurrentNode->pre;
                }
                psNewVirtualLoadNode->type = NT_VIRTUAL_FUNCTION;
                psNewVirtualFuncNode->explain = psNewVirtualLoadNode;
                psNewVirtualLoadNode->parent = psNewVirtualFuncNode;
                psNewVirtualLoadNode->explain = psCurrentNode;
                psCurrentNode->parent = psNewVirtualLoadNode;
                psCurrentNode->pre = NULL;

                /*   add new node   */
                nFlag = cliTreeNodeCreate(NULL,&psNewVirtualFuncNode);
                if (ERROR == nFlag)
                {
                    return ERROR;
                }
                psNewVirtualFuncNode->type = NT_VIRTUAL_FUNCTION;
                psNewVirtualFuncNode->left = psNewVirtualLoadNode;
                psNewVirtualLoadNode->right = psNewVirtualFuncNode;
                psNewVirtualFuncNode->parent = psNewVirtualLoadNode->parent;
                psNewVirtualFuncNode->explain = psTreeRoot;
                psTreeRoot->pre = NULL;
                psTreeRoot->parent = psNewVirtualFuncNode;
                return OK;
            }
            break;
        }/*switch*/
        nFlag = cliWordExtract(&pchCommand, pchWord, FORWARD);
        bIsGetWordOk = (OK == nFlag)?TRUE:FALSE;
    }/*while*/
    return OK;
}


/*****************************************************************************
 *cliTreeVirtualNodeMatch -  To find the same virtual node.
 *DESCRIPTION
 *
 *Input:psTreeRoot,ppchCommand,ppsMatchNode,unMode .

 *Output:ppchCommand,ppsMatchNode.
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliTreeVirtualNodeMatch(CMD_KEY_PARA_LINK_t *psTreeRoot,
                            char **ppchCommand,
                            CMD_KEY_PARA_LINK_t **ppsMatchNode,
                            unsigned int unMode)
{
    int nFlag = ERROR;


    if (NULL == psTreeRoot || NULL == ppchCommand || NULL == *ppchCommand
        || NULL == ppsMatchNode)
    {
        return ERROR;
    }

    *ppsMatchNode = NULL;
    switch(psTreeRoot->type)
    {
    case NT_VIRTUAL_LOAD:
        psTreeRoot = psTreeRoot->explain;
        while (psTreeRoot && ERROR == nFlag)
        {
            nFlag = cliTreeBranchNodeMatch(psTreeRoot->explain,ppchCommand,unMode);
        /*  psTreeRoot =>psTreeRoot->explain  */
            if (ERROR == nFlag)
            {
                psTreeRoot = psTreeRoot->right;
            }
        }
        if (OK == nFlag)
        {
            *ppsMatchNode = psTreeRoot;
        }
        break;
    case NT_VIRTUAL_OPTIONAL:
    case NT_VIRTUAL_REQUIRED:
        nFlag = cliTreeBranchNodeMatch (psTreeRoot,ppchCommand,unMode);
        if (OK == nFlag)
        {
            *ppsMatchNode = psTreeRoot;
        }
        break;
    default:
        nFlag = ERROR;
        break;
    }
    return(nFlag);
}


/*****************************************************************************
 *cliTreeBranchNodeMatch -  To find the match branch .
 *DESCRIPTION
 *
 *Input:psTreeRoot,ppchCommand,unMode.

 *Output:
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliTreeBranchNodeMatch(CMD_KEY_PARA_LINK_t *psTreeRoot, char **ppchCommand,
                     unsigned int unMode)
{
    int  nFlag = OK;
    char achWord[MAX_POOL_LENGTH+2];
    char *pchWord = achWord;


    switch(psTreeRoot->type)
    {
    case NT_VIRTUAL_OPTIONAL:
        nFlag = cliWordExtract(ppchCommand, pchWord,FORWARD);
        if (ERROR == nFlag)
        {
            return ERROR;
        }
        if (pchWord[0] != '[')
        {
            return ERROR;
        }
        nFlag = cliTreeBranchNodeMatch(psTreeRoot->explain,ppchCommand,unMode);
        if (ERROR == nFlag)
        {
            return ERROR;
        }
        if (psTreeRoot->next)
        {
            nFlag = cliTreeBranchNodeMatch(psTreeRoot->next,ppchCommand,unMode);
            if (ERROR == nFlag)
            {
                return ERROR;
            }
        }
        if (psTreeRoot->right)
        {
            nFlag = cliTreeBranchNodeMatch(psTreeRoot->right,ppchCommand,unMode);
            if (ERROR == nFlag)
            {
                return ERROR;
            }
        }
        nFlag = cliWordExtract(ppchCommand, pchWord,FORWARD);
        if (ERROR == nFlag)
        {
            return ERROR;
        }
        if (pchWord[0] != ']')
        {
            return ERROR;
        }
        break;
    case NT_VIRTUAL_REQUIRED:
        nFlag = cliWordExtract(ppchCommand, pchWord,FORWARD);
        if (ERROR == nFlag)
        {
            return ERROR;
        }
        if (pchWord[0] != '{')
        {
            return ERROR;
        }
        nFlag = cliTreeBranchNodeMatch(psTreeRoot->explain,ppchCommand,unMode);
        if (ERROR == nFlag)
        {
            return ERROR;
        }
        nFlag = cliWordExtract(ppchCommand, pchWord,FORWARD);
        if (ERROR == nFlag)
        {
            return ERROR;
        }
        if (pchWord[0] != '}' )
        {
            return ERROR;
        }
        break;
    case NT_KEY:
    case NT_PARAMETER:
        nFlag = cliWordExtract(ppchCommand, pchWord,FORWARD);
        if ('|' == pchWord[0])
        {
            nFlag = cliWordExtract(ppchCommand, pchWord,FORWARD);
        }
        if (ERROR == nFlag)
        {
            return ERROR;
        }
        nFlag = strcmp(psTreeRoot->key,pchWord);
        if (nFlag != 0)
        {
            return ERROR;
        }
        psTreeRoot->exec_mode = psTreeRoot->exec_mode | unMode;
        if (psTreeRoot->next)
        {
            nFlag = cliTreeBranchNodeMatch(psTreeRoot->next,ppchCommand,unMode);
        }
        if (nFlag != 0)
        {
            return ERROR;
        }
        if (psTreeRoot->right)
        {
            nFlag = cliTreeBranchNodeMatch(psTreeRoot->right,ppchCommand,unMode);
        }
        if (nFlag != 0)
        {
            return ERROR;
        }
        break;
    default:
        nFlag = ERROR;
        break;
    }
    return(nFlag);
}


/*****************************************************************************
 *cliTreeNodeFree -  free node  CMD_KEY_PARA_LINK_t .
 *DESCRIPTION
 *
 *Input:psTreeRoot.

 *Output:
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliTreeNodeFree(CMD_KEY_PARA_LINK_t *psTreeRoot)
{
    PARA_LINK_t *psParaLnk = NULL;
    PARA_LINK_t *psParaLnkNext = NULL;


    if (NULL == psTreeRoot)
    {
        return OK;
    }
    if (psTreeRoot->next)
    {
        cliTreeNodeFree(psTreeRoot->next);
    }
    if (psTreeRoot->explain)
    {
        cliTreeNodeFree(psTreeRoot->explain);
    }
    if (psTreeRoot->right)
    {
        cliTreeNodeFree(psTreeRoot->right);
    }
    if (psTreeRoot->description)
    {
        vosFree(psTreeRoot->description);
        psTreeRoot->description = NULL;
    }
    if (psTreeRoot->func_info)
    {
        psParaLnk = psTreeRoot->func_info->parameter;
        while (psParaLnk)
        {
            psParaLnkNext = psParaLnk->next;
            vosFree(psParaLnk);
            psParaLnk = psParaLnkNext;
        }

        vosFree(psTreeRoot->func_info);
        psTreeRoot->func_info = NULL;
    }
    if (NULL != psTreeRoot->parameter)
    {
        vosFree(psTreeRoot->parameter);
        psTreeRoot->parameter = NULL;
    }
    vosFree(psTreeRoot);
    psTreeRoot = NULL;
    return OK;
}


/*****************************************************************************
 *cliForestCreate -  Create command forest.
 *DESCRIPTION
 *
 *Input:psCmdInfo,nMiiNum,ppsCmdTreeArray,ppsForestRoot.

 *Output:ppsCmdTreeArray,ppsForestRoot.
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS cliForestCreate(MODEL_INTF_INFO_t *psCmdInfo,int nMiiNum,
                     CMD_KEY_PARA_LINK_t **ppsCmdTreeArray,
                     CMD_KEY_PARA_LINK_t **ppsForestRoot)
{
    int  i = 0;
    int  j = 0;
    int  k = 0;
    int nFlag = ERROR;
    unsigned int unCliMode = 0;
    BOOL bIsCreated = FALSE;
    CMD_KEY_PARA_LINK_t *psNewNode = NULL;
    CMD_KEY_PARA_LINK_t *psTempNode = NULL;


    if (NULL == psCmdInfo || nMiiNum < 1 || NULL == ppsCmdTreeArray)
    {
        return ERROR;
    }
    *ppsForestRoot = NULL;
    for (i = 0; i < NUM_OF_CLI_MODE; i++)
    {
        ppsCmdTreeArray[i] = NULL;
    }

    for ( i = 0; i < nMiiNum; i++)
    {
        unCliMode = psCmdInfo[i].exec_mode;
        for (j = 0; j < NUM_OF_CLI_MODE; j++)
        {
#if 0
            k = 0;
            if (g_punArrayOfCliMode[j] & unCliMode)
            {
                while (k < j)
                {
                    if (g_punArrayOfCliMode[k] & unCliMode)
                    {
                       bIsCreated = TRUE;
                    }
                    k++;
                }
                if (TRUE == bIsCreated)
                {
                    k = 0;
                    bIsCreated = FALSE;
                    break;
                }
                if (NULL == ppsCmdTreeArray[j])
                {
                    cliTreeCreate(&(ppsCmdTreeArray[j]),&psCmdInfo[i]);
                }
                else
                {
                    nFlag = cliTreeNodeAdd (&(ppsCmdTreeArray[j]),&psCmdInfo[i]);
                    if (ERROR == nFlag)
                    {
                       return ERROR;
                    }
                }
            }
#else
            if (g_punArrayOfCliMode[j] & unCliMode)
            {
                if (NULL == ppsCmdTreeArray[j])
                    cliTreeCreate(&(ppsCmdTreeArray[j]),&psCmdInfo[i]);
                else
                {
                    nFlag = cliTreeNodeAdd (&(ppsCmdTreeArray[j]),&psCmdInfo[i]);
                    if (ERROR == nFlag)
                       return ERROR;
                }
                break;
            }
#endif
        }
    }

/* Adjust the next to point to Node NT_VIRTUAL_COMMAND */
    for (j = 0; j < NUM_OF_CLI_MODE; j++)
    {
        if (ppsCmdTreeArray[j] != NULL)
        {
            cliTreePointerAdjust(ppsCmdTreeArray[j]);
        }
    }

/*  Link the single tree to forest */
    nFlag = cliTreeNodeCreate(NULL, ppsForestRoot);
    if (nFlag == ERROR)
    {
        return ERROR;
    }

    (*ppsForestRoot)->type = NT_VIRTUAL_LOAD;  /*  Create the Node,type of
                                                   NT_VIRTUAL_LOAD,as the
                                                   root of forest */
    (*ppsForestRoot)->explain = NULL;
    nFlag = cliTreeNodeCreate(NULL, &psNewNode);
    if (ERROR == nFlag)
    {
        vosFree(ppsForestRoot); /* Bug ??*/
        ppsForestRoot = NULL;
        return ERROR;
    }

    if (NULL == (*ppsForestRoot)->explain)
    {
        (*ppsForestRoot)->explain = psNewNode;
        psNewNode->type = NT_VIRTUAL_FUNCTION;
        psNewNode->left = NULL;

        psNewNode->parent = *ppsForestRoot;
        psNewNode->explain = ppsCmdTreeArray[0];   /* if NULL then Wrong !!!  we should search the tree */
        ppsCmdTreeArray[0]->parent = psNewNode;
        ppsCmdTreeArray[0]->pre = NULL;
    }

    for (i = 1 ; i < NUM_OF_CLI_MODE ; i++ )
    {
        psTempNode = psNewNode;
        if (ppsCmdTreeArray[i] != NULL)
        {
            nFlag = cliTreeNodeCreate(NULL, &psNewNode);
            if (ERROR == nFlag)
            {
                /* need to release some resource that have been alloced */
                return ERROR;
            }

            psNewNode->type = NT_VIRTUAL_FUNCTION;
            psNewNode->left = psTempNode;
            psTempNode->right = psNewNode;
            psNewNode->parent = psTempNode->parent;
            psNewNode->explain = ppsCmdTreeArray[i];
            ppsCmdTreeArray[i]->parent = psNewNode;
            ppsCmdTreeArray[i]->pre = NULL;
        }
    }
    return OK ;
}


/*****************************************************************************
 *cliTreeNodeIsAllEqual -  To check if the tree node is all things being equal
 *DESCRIPTION
 *
 *Input:psNodeCollection.

 *Output:
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
BOOL cliTreeNodeIsAllEqual(NODE_COLLECTION_t *psNodeCollection)
{

    NODE_COLLECTION_t      *psNodeCollectionTmp = NULL;
    CMD_KEY_PARA_LINK_t *psFirstNode = NULL;


    if (NULL == psNodeCollection)
    {
        return FALSE;
    }
    if (psNodeCollection->total < 2)
    {
        return FALSE;
    }
    psNodeCollectionTmp = psNodeCollection->next;
    psFirstNode = psNodeCollectionTmp->p;
    psNodeCollectionTmp = psNodeCollectionTmp->next;
    while (psNodeCollectionTmp)
    {
        if (0 != strcmp(psFirstNode->key, psNodeCollectionTmp->p->key))
        {
            return FALSE;
        }
        psNodeCollectionTmp = psNodeCollectionTmp->next;
    }
    return TRUE;
}

/*****************************************************************************
 *cliWordTypeGet - Get the type of word which you given.
 *DESCRIPTION
 *
 *Input:pchWord,pulWordType.

 *Output:
 *Return:STATUS:
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliWordTypeGet(char *pchWord, unsigned int *pulWordType)
{
    int nWordLen = 0;

    if (NULL == pchWord)
    {
        return ERROR;
    }
    nWordLen = strlen(pchWord);
    if (1 == nWordLen)
    {
        switch (pchWord[0])
        {
        case 9:
            *pulWordType = WORD_TAB;
            break;
        case '?':
            *pulWordType = WORD_QUESTION;
            break;
        default:
            *pulWordType = WORD_NORMAL;
            break;
        }
    }
    else
    {
        switch(pchWord[nWordLen-1])
        {
        case 9:
            *pulWordType = WORD_END_TAB;
            break;
        case '?':
            *pulWordType = WORD_END_QUESTION;
            break;
        default:
            *pulWordType = WORD_NORMAL;
            break;
        }
    }
    return OK;
}

/*****************************************************************************
 *cliCollectionFree -  free the type of PARA_COLLECTION_t node link.
 *DESCRIPTION
 *
 *Input:psParaCollection,psNodeCollection,psCmdPool.

 *Output:
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliCollectionFree(PARA_COLLECTION_t *psParaCollection,
                        NODE_COLLECTION_t *psNodeCollection,
                        CMD_POOL_t *psCmdPool)
{
    int  i;
    PARA_COLLECTION_t   *psParaCollectionNext = NULL;
    NODE_COLLECTION_t   *psNodeCollectionNext = NULL;


    while (psParaCollection)
    {
        psParaCollectionNext = psParaCollection->next;
        if (psParaCollection->p)
        {
            if (CLI_WORD == psParaCollection->type
                || CLI_STRING == psParaCollection->type
                || CLI_MACADDR == psParaCollection->type
                || CLI_DATE == psParaCollection->type
                || CLI_TIME == psParaCollection->type
                )
            {
                if (psParaCollection->p->p)
                {
                    vosFree(psParaCollection->p->p);
                    psParaCollection->p->p = NULL;
                }
            }
            if (psParaCollection->p)
            {
                vosFree(psParaCollection->p);
                psParaCollection->p = NULL;
            }
        }
        if (psParaCollection)
        {
            vosFree(psParaCollection);
            psParaCollection = NULL;
        }
        psParaCollection = psParaCollectionNext;
    }

    while (psNodeCollection)
    {
        psNodeCollectionNext = psNodeCollection->next;
        if (psNodeCollection)
        {
            vosFree(psNodeCollection);
            psNodeCollection = NULL;
        }
        psNodeCollection = psNodeCollectionNext;
    }

    if (psCmdPool)
    {   /* 1.01,
        for (i = 0; i < MAX_POOL_LENGTH; i++)
        {
            psCmdPool->achCmdPool[i] = '\0';
        }
        psCmdPool->nInputTailPos = 0;
         */
        memset(psCmdPool, 0, sizeof(CMD_POOL_t));
    }
    return OK;
}


BOOL cliCharIsValid(char chToCheckChar)
{
    int i = 0;
    BOOL bCharIsValid = FALSE;


    if ('_' == chToCheckChar || '-' == chToCheckChar || '.' == chToCheckChar
        || '/' == chToCheckChar || ':' == chToCheckChar || '\\' == chToCheckChar
        || (chToCheckChar >= 'a' && chToCheckChar <= 'z')
        || (chToCheckChar >= 'A' && chToCheckChar <= 'Z')
        || (chToCheckChar >= '0' && chToCheckChar <= '9')
        || '*' == chToCheckChar || ',' == chToCheckChar
        )
    {
        bCharIsValid = TRUE;
        return(bCharIsValid);
    }
    while (0xFF != (uint8)g_acSpecialChar[i])
    {
        if (g_acSpecialChar[i] == chToCheckChar)
        {
            bCharIsValid = TRUE;
        }
        i++;
    }

    return(bCharIsValid);
}

/*****************************************************************************
 *cliParameterCheck -  check the parameter's valid and collect it.
 *DESCRIPTION
 *
 *Input:psParaCollection,psForestRoot,pchWord,ppchCommand.

 *Output:
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS cliParameterCheck(PARA_COLLECTION_t *psParaCollection,
                  CMD_KEY_PARA_LINK_t *psForestRoot,
                  char *pchWord, char **ppchCommand)
{
    int  i = 0;
    int  nMax = 0;
    int  nMin = 0;
    int  nLen = 0;
    int  nFlag = -1;
    int  nValue = 0;
    char *pString;
    unsigned long ulValue = 0;
    unsigned long ulMax = 0;
    unsigned long ulMin = 0;
    unsigned long ulSubnetMask = 0;
    unsigned long ulLeftShift = 0x80000000;
    PARA_COLLECTION_t *psParaCollectionTemp = NULL;
    PARA_COLLECTION_t *psParaCollectionHead = NULL;
    PARA_COLLECTION_t *psParaCollectionTail = NULL;
    CMD_KEY_PARA_LINK_t *psCmdKeyParaNode = NULL;


    if (NULL == psParaCollection || NULL == psForestRoot || NULL == ppchCommand)
    {
        return ERROR;
    }

    psCmdKeyParaNode = psForestRoot;
    while (psCmdKeyParaNode->parent
           && (NT_VIRTUAL_REQUIRED == psCmdKeyParaNode->parent->type
           || NT_VIRTUAL_OPTIONAL == psCmdKeyParaNode->parent->type))
    {
        if (psCmdKeyParaNode->pre)
        {
            break;
        }
        psParaCollectionTemp = (PARA_COLLECTION_t *)vosAlloc(sizeof(PARA_COLLECTION_t));
        if (NULL == psParaCollectionTemp)
        {
            return ERROR;
        }

        psParaCollectionTemp->p = (PARA_TABLE_t *)vosAlloc(sizeof (PARA_TABLE_t));
        if (NULL == psParaCollectionTemp->p)
        {
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL;
            return ERROR;
        }

        psParaCollectionTemp->last = NULL;
        psParaCollectionTemp->next = NULL;
        psParaCollectionTemp->p->i = psCmdKeyParaNode->keyword_position;
        psParaCollectionTemp->type = CLI_INTEGER;
        psParaCollectionTemp->pos = psCmdKeyParaNode->parent->parameter_postion;

        if (NULL == psParaCollectionTail)
        {
            psParaCollectionTail = psParaCollectionTemp;
        }

        if (NULL == psParaCollectionHead)
        {
            psParaCollectionHead = psParaCollectionTemp;
        }
        else
        {
            psParaCollectionTemp->next = psParaCollectionHead;
            psParaCollectionHead = psParaCollectionTemp;
            psParaCollectionTemp = NULL;
        }
        psCmdKeyParaNode = psCmdKeyParaNode->parent;
    }

    psParaCollectionTemp = NULL;

    switch (psForestRoot->parameter->type)
    {
    case CLI_INTEGER:
        nMin = psForestRoot->parameter->min;
        nMax = psForestRoot->parameter->max;
        if (FALSE == utilIsInt(pchWord))
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        else
        {
            nValue = vosStrToInt(pchWord, NULL);
        }

        if ((nValue < nMin) ||( nValue > nMax) )
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp = (PARA_COLLECTION_t *)vosAlloc(sizeof(PARA_COLLECTION_t));
        if (NULL == psParaCollectionTemp)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp->p = (PARA_TABLE_t *)vosAlloc(sizeof(PARA_TABLE_t));
        if (NULL == psParaCollectionTemp->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL;
            return ERROR;
        }
        psParaCollectionTemp->last = NULL;
        psParaCollectionTemp->next = NULL;
        psParaCollectionTemp->pos = psForestRoot->parameter_postion;
        psParaCollectionTemp->p->i = nValue;
        psParaCollectionTemp->type = CLI_INTEGER;

        /*add father's first and then his*/
        if (psParaCollectionHead)
        {
            psParaCollection->last->next = psParaCollectionHead;
            psParaCollection->last = psParaCollectionTail;
        }
        psParaCollection->last->next = psParaCollectionTemp;
        psParaCollection->last = psParaCollectionTemp;
        break;

    case CLI_UINT:
        ulMin = (UINT32)psForestRoot->parameter->min;
        ulMax = (UINT32)psForestRoot->parameter->max;
        if (FALSE == utilIsInt(pchWord))
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        else
        {
            ulValue = vosStrToUInt(pchWord, NULL);
        }

        if (ulValue < ulMin || ulValue > ulMax)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp = (PARA_COLLECTION_t *)vosAlloc(sizeof(PARA_COLLECTION_t));
        if (NULL == psParaCollectionTemp)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp->p = (PARA_TABLE_t *)vosAlloc(sizeof(PARA_TABLE_t));
        if (NULL == psParaCollectionTemp->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL;
            return ERROR;
        }
        psParaCollectionTemp->last = NULL;
        psParaCollectionTemp->next = NULL;
        psParaCollectionTemp->pos = psForestRoot->parameter_postion;
        psParaCollectionTemp->p->u = ulValue;
        psParaCollectionTemp->type = CLI_UINT;

        /*add father's first and then his*/
        if (psParaCollectionHead)
        {
            psParaCollection->last->next = psParaCollectionHead;
            psParaCollection->last = psParaCollectionTail;
        }
        psParaCollection->last->next = psParaCollectionTemp;
        psParaCollection->last = psParaCollectionTemp;
        break;
    case CLI_WORD:
        nLen = strlen(pchWord);
        if (nLen > psForestRoot->parameter->fail)
        {
            //vosPrintf(pstEnv->nWriteFd, "parameter is too large\r\n");
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        if (0 == nLen)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        for (i = 0; i < nLen; i++ )
        {
            #if 0 /*to make configuration flexibility */
            if ('_' == pchWord[i] || '-' == pchWord[i] || '.' == pchWord[i]
                || '/' == pchWord[i] || ':' == pchWord[i] || '\\' == pchWord[i]
                || (pchWord[i] >= 'a' && pchWord[i] <= 'z') || (pchWord[i] >= 'A'
                && pchWord[i] <= 'Z')||(pchWord[i] >= '0' && pchWord[i] <= '9'))
            #endif
            if (TRUE == cliCharIsValid(pchWord[i]))
            {
                continue;
            }
            else
            {
                break;
            }
        }
        if (i < nLen)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp = (PARA_COLLECTION_t *)vosAlloc(sizeof(PARA_COLLECTION_t));
        if (NULL == psParaCollectionTemp)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp->p = (PARA_TABLE_t *)vosAlloc(sizeof(PARA_TABLE_t));
        if (NULL == psParaCollectionTemp->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL;
            return ERROR;
        }
        psParaCollectionTemp->last = NULL;
        psParaCollectionTemp->next = NULL;
        psParaCollectionTemp->pos = psForestRoot->parameter_postion;
        psParaCollectionTemp->p->p = (char *)vosAlloc((nLen+1)*sizeof(char));
        if (NULL == psParaCollectionTemp->p->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(psParaCollectionTemp->p);
            psParaCollectionTemp->p = NULL;
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL;
            return ERROR;
        }
        strcpy(psParaCollectionTemp->p->p,pchWord);
        psParaCollectionTemp->type = CLI_WORD;
        /*add father's first and then his*/
        if (psParaCollectionHead)
        {
            psParaCollection->last->next = psParaCollectionHead;
            psParaCollection->last = psParaCollectionTail;
        }
        psParaCollection->last->next = psParaCollectionTemp;
        psParaCollection->last = psParaCollectionTemp;
        break;
       case CLI_MACADDR:
        nLen = strlen(pchWord);
        if (nLen > psForestRoot->parameter->fail)
        {
            //vosPrintf(pstEnv->nWriteFd, "parameter is too large\r\n");
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        if (0 == nLen)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        for (i = 0; i < nLen; i++ )
        {
            #if 0 /*  to make configuration flexibility */
            if ('_' == pchWord[i] || '-' == pchWord[i] || '.' == pchWord[i]
                || '/' == pchWord[i] || ':' == pchWord[i] || '\\' == pchWord[i]
                || (pchWord[i] >= 'a' && pchWord[i] <= 'z') || (pchWord[i] >= 'A'
                && pchWord[i] <= 'Z')||(pchWord[i] >= '0' && pchWord[i] <= '9'))
            #endif
            if (TRUE == cliCharIsValid(pchWord[i]))
            {
                continue;
            }
            else
            {
                break;
            }
        }
        if (i < nLen)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp = (PARA_COLLECTION_t *)vosAlloc(sizeof(PARA_COLLECTION_t));
        if (NULL == psParaCollectionTemp)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp->p = (PARA_TABLE_t *)vosAlloc(sizeof(PARA_TABLE_t));
        if (NULL == psParaCollectionTemp->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL;
            return ERROR;
        }
        psParaCollectionTemp->last = NULL;
        psParaCollectionTemp->next = NULL;
        psParaCollectionTemp->pos = psForestRoot->parameter_postion;
        psParaCollectionTemp->p->p = (char *)vosAlloc((nLen+1)*sizeof(char));
        if (NULL == psParaCollectionTemp->p->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(psParaCollectionTemp->p);
            psParaCollectionTemp->p = NULL;
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL;
            return ERROR;
        }
        strcpy(psParaCollectionTemp->p->p,pchWord);
        psParaCollectionTemp->type = CLI_MACADDR;
        /*add father's first and then his*/
        if (psParaCollectionHead)
        {
            psParaCollection->last->next = psParaCollectionHead;
            psParaCollection->last = psParaCollectionTail;
        }
        psParaCollection->last->next = psParaCollectionTemp;
        psParaCollection->last = psParaCollectionTemp;
        break;
    case CLI_DATE:
        nLen = strlen(pchWord);
        if (nLen > psForestRoot->parameter->fail)
        {
            //vosPrintf(pstEnv->nWriteFd, "parameter is too large\r\n");
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        if (0 == nLen)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        for (i = 0; i < nLen; i++ )
        {
            #if 0 /*to make configuration flexibility */
            if ('_' == pchWord[i] || '-' == pchWord[i] || '.' == pchWord[i]
                || '/' == pchWord[i] || ':' == pchWord[i] || '\\' == pchWord[i]
                || (pchWord[i] >= 'a' && pchWord[i] <= 'z') || (pchWord[i] >= 'A'
                && pchWord[i] <= 'Z')||(pchWord[i] >= '0' && pchWord[i] <= '9'))
            #endif
            if (TRUE == cliCharIsValid(pchWord[i]))
            {
                continue;
            }
            else
            {
                break;
            }
        }
        if (i < nLen)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp = (PARA_COLLECTION_t *)vosAlloc(sizeof(PARA_COLLECTION_t));
        if (NULL == psParaCollectionTemp)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp->p = (PARA_TABLE_t *)vosAlloc(sizeof(PARA_TABLE_t));
        if (NULL == psParaCollectionTemp->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL;
            return ERROR;
        }
        psParaCollectionTemp->last = NULL;
        psParaCollectionTemp->next = NULL;
        psParaCollectionTemp->pos = psForestRoot->parameter_postion;
        psParaCollectionTemp->p->p = (char *)vosAlloc((nLen+1)*sizeof(char));
        if (NULL == psParaCollectionTemp->p->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(psParaCollectionTemp->p);
            psParaCollectionTemp->p = NULL;
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL;
            return ERROR;
        }
        strcpy(psParaCollectionTemp->p->p,pchWord);
        psParaCollectionTemp->type = CLI_DATE;
        /*add father's first and then his*/
        if (psParaCollectionHead)
        {
            psParaCollection->last->next = psParaCollectionHead;
            psParaCollection->last = psParaCollectionTail;
        }
        psParaCollection->last->next = psParaCollectionTemp;
        psParaCollection->last = psParaCollectionTemp;
        break;
       case CLI_TIME:
        nLen = strlen(pchWord);
        if (nLen > psForestRoot->parameter->fail)
        {
            //vosPrintf(pstEnv->nWriteFd, "parameter is too large\r\n");
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        if (0 == nLen)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        for (i = 0; i < nLen; i++ )
        {
            #if 0 /* to make configuration flexibility */
            if ('_' == pchWord[i] || '-' == pchWord[i] || '.' == pchWord[i]
                || '/' == pchWord[i] || ':' == pchWord[i] || '\\' == pchWord[i]
                || (pchWord[i] >= 'a' && pchWord[i] <= 'z') || (pchWord[i] >= 'A'
                && pchWord[i] <= 'Z')||(pchWord[i] >= '0' && pchWord[i] <= '9'))
            #endif
            if (TRUE == cliCharIsValid(pchWord[i]))
            {
                continue;
            }
            else
            {
                break;
            }
        }
        if (i < nLen)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp = (PARA_COLLECTION_t *)vosAlloc(sizeof(PARA_COLLECTION_t));
        if (NULL == psParaCollectionTemp)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp->p = (PARA_TABLE_t *)vosAlloc(sizeof(PARA_TABLE_t));
        if (NULL == psParaCollectionTemp->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL;
            return ERROR;
        }
        psParaCollectionTemp->last = NULL;
        psParaCollectionTemp->next = NULL;
        psParaCollectionTemp->pos = psForestRoot->parameter_postion;
        psParaCollectionTemp->p->p = (char *)vosAlloc((nLen+1)*sizeof(char));
        if (NULL == psParaCollectionTemp->p->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(psParaCollectionTemp->p);
            psParaCollectionTemp->p = NULL ;
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL ;
            return ERROR;
        }
        strcpy(psParaCollectionTemp->p->p,pchWord);
        psParaCollectionTemp->type = CLI_TIME;
        /*add father's first and then his*/
        if (psParaCollectionHead)
        {
            psParaCollection->last->next = psParaCollectionHead;
            psParaCollection->last = psParaCollectionTail;
        }
        psParaCollection->last->next = psParaCollectionTemp;
        psParaCollection->last = psParaCollectionTemp;
        break;

    case CLI_STRING:
        pString = (char *)vosAlloc(256*sizeof(char));
        if (NULL == pString)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }

        nLen = strlen(pchWord);
        if ((0 == nLen) && (nLen >255) )   /*   nLen > 255*/
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }

        for (i = 0; i < nLen; i++)
        {
            pString[i] = pchWord[i];
        }

        while (**ppchCommand && **ppchCommand != '?' && **ppchCommand != '\t')
        {
            pString[i++] = **ppchCommand;
            (*ppchCommand)++;
        }
        pString[i] = '\0';
        ulValue = strlen(pString);
        psParaCollectionTemp = (PARA_COLLECTION_t *)vosAlloc(sizeof(PARA_COLLECTION_t));
        if (psParaCollectionTemp == NULL)
        {
            cliCollectionFree(psParaCollectionHead,NULL,NULL);
            return ERROR;
        }
        psParaCollectionTemp->p=(PARA_TABLE_t *)vosAlloc(sizeof(PARA_TABLE_t));
        if (NULL == psParaCollectionTemp->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL ;
            return ERROR;
        }
        psParaCollectionTemp->last = NULL;
        psParaCollectionTemp->next = NULL;
        psParaCollectionTemp->pos = psForestRoot->parameter_postion;
        psParaCollectionTemp->p->p = (char *)vosAlloc((ulValue+1)*sizeof(char));
        if (NULL == psParaCollectionTemp->p->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(pString);
            pString = NULL ;
            vosFree(psParaCollectionTemp->p);
            psParaCollectionTemp->p = NULL ;
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL ;
            return ERROR;
        }
        strcpy(psParaCollectionTemp->p->p,pString);
        vosFree(pString);
        pString = NULL ;
        psParaCollectionTemp->type = CLI_STRING;

        /*add father's first and then his*/
        if (psParaCollectionHead)
        {
            psParaCollection->last->next = psParaCollectionHead;
            psParaCollection->last = psParaCollectionTail;
        }
        psParaCollection->last->next = psParaCollectionTemp;
        psParaCollection->last = psParaCollectionTemp;
        return OK;
        break;

    case CLI_IPDOTADDR:
        ulMin = psForestRoot->parameter->min;
        ulMax = psForestRoot->parameter->max;
        ulValue = ntohl(inet_addr(pchWord));
        if (ERROR == ulValue)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }

        if (ulValue < ulMin || ulValue > ulMax)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp = (PARA_COLLECTION_t *)vosAlloc(sizeof(PARA_COLLECTION_t));
        if (NULL == psParaCollectionTemp)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp->p = (PARA_TABLE_t *)vosAlloc(sizeof(PARA_TABLE_t));
        if (NULL == psParaCollectionTemp->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL ;
            return ERROR;
        }
        psParaCollectionTemp->last = NULL;
        psParaCollectionTemp->next = NULL;
        psParaCollectionTemp->pos = psForestRoot->parameter_postion;
        psParaCollectionTemp->p->u = ulValue;
        psParaCollectionTemp->type = psForestRoot->parameter->type;

        /*add father's first and then his*/
        if (psParaCollectionHead)
        {
            psParaCollection->last->next = psParaCollectionHead;
            psParaCollection->last = psParaCollectionTail;
        }
        psParaCollection->last->next = psParaCollectionTemp;
        psParaCollection->last = psParaCollectionTemp;
        break;
    case CLI_IPDOTMASK:
        ulMin = psForestRoot->parameter->min;
        ulMax = psForestRoot->parameter->max;
        ulValue = ntohl(inet_addr(pchWord));
        ulSubnetMask = ulValue;

        if (ERROR == ulValue) /* To handle 255.255.255.255 */
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }

        if (ulValue < ulMin || ulValue > ulMax)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }

        while (ulSubnetMask & ulLeftShift)
        {
            ulSubnetMask = ulSubnetMask << 1;

        }
        if (ulSubnetMask != 0)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp = (PARA_COLLECTION_t *)vosAlloc(sizeof(PARA_COLLECTION_t));
        if (NULL == psParaCollectionTemp)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp->p = (PARA_TABLE_t *)vosAlloc(sizeof(PARA_TABLE_t));
        if (NULL == psParaCollectionTemp->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp = NULL ;
            return ERROR;
        }
        psParaCollectionTemp->last = NULL;
        psParaCollectionTemp->next = NULL;
        psParaCollectionTemp->pos = psForestRoot->parameter_postion;
        psParaCollectionTemp->p->u = ulValue;
        psParaCollectionTemp->type = psForestRoot->parameter->type;

        /*add father's first and then his*/
        if (psParaCollectionHead)
        {
            psParaCollection->last->next = psParaCollectionHead;
            psParaCollection->last = psParaCollectionTail;
        }
        psParaCollection->last->next = psParaCollectionTemp;
        psParaCollection->last = psParaCollectionTemp;
        break;
    case CLI_IPINTADDR:
    case CLI_IPINTMASK:
        ulMin=psForestRoot->parameter->min;
        ulMax=psForestRoot->parameter->max;
        ulValue = ntohl(inet_addr(pchWord));
        if (ulValue < ulMin || ulValue > ulMax)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp=(PARA_COLLECTION_t *)vosAlloc(sizeof(PARA_COLLECTION_t));
        if (NULL == psParaCollectionTemp)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            return ERROR;
        }
        psParaCollectionTemp->p=(PARA_TABLE_t *)vosAlloc(sizeof(PARA_TABLE_t));
        if (NULL == psParaCollectionTemp->p)
        {
            cliCollectionFree(psParaCollectionHead, NULL, NULL);
            vosFree(psParaCollectionTemp);
            psParaCollectionTemp  = NULL ;
            return ERROR;
        }
        psParaCollectionTemp->last = NULL;
        psParaCollectionTemp->next = NULL;
        psParaCollectionTemp->pos = psForestRoot->parameter_postion;
        psParaCollectionTemp->p->u = ulValue;
        psParaCollectionTemp->type = psForestRoot->type;

        /*add father's first and then his*/
        if (psParaCollectionHead)
        {
            psParaCollection->last->next = psParaCollectionHead;
            psParaCollection->last = psParaCollectionTail;
        }
        psParaCollection->last->next = psParaCollectionTemp;
        psParaCollection->last = psParaCollectionTemp;
        break;
    default:
        break;
    }
    return OK;
}

/*****************************************************************************
 *cliChildTask -  Start child task.
 *DESCRIPTION
 *
 *Input:pstArg.
 *Output:
 *Return:
 ****************************************************************************/

static void cliChildTask(CLI_CHILD_TASK_ARG_t *pstArg)
{
    FUNCPTR fnProcess = NULL;
    ENV_t *pstEnv = NULL;
    PARA_TABLE_t *psPara = NULL;

    if (pstArg)
    {
        fnProcess = pstArg->fnProcess;
        pstEnv = pstArg->pstEnv;
        psPara = pstArg->psPara;
    }

    if (fnProcess)
    {
        fnProcess(pstEnv, psPara);
    }

    /* we must call it to release the thread info */
    vosThreadExit(0);
}

void cliCmdPoolPrint(ENV_t *pstEnv)
{
    CMD_POOL_t  *psCmdPool;
    char achBuf[MAX_POOL_LENGTH+2];

    psCmdPool = (CMD_POOL_t *)(pstEnv->command);
    if (NULL == psCmdPool)
        return;

    if ((psCmdPool->bIsLeftFlag != TRUE))
    {
        vosPrintf(pstEnv->nWriteFd, "%s", psCmdPool->achCmdPool);
    }
    else
    {
        achBuf[0] = LEFT_FLAG_CHARACTOR;
        memcpy(&achBuf[1], &psCmdPool->achCmdPool[psCmdPool->nDispBegin], psCmdPool->nDispLen);
        achBuf[psCmdPool->nDispLen+1] = '\0';
        vosPrintf(pstEnv->nWriteFd, "%s", achBuf);
    }
}

/*****************************************************************************
 *cliPromptPrint - Print the prompt according mode.
 *DESCRIPTION
 *
 *Input:pstEnv.
 *Output:N/A.
 *Return:N/A.
 ****************************************************************************/
void cliPromptPrint(ENV_t *pstEnv)
{
    vosPrintf(pstEnv->nWriteFd, "%s",odmSysCfgNameGet());

    switch(pstEnv->pmode)
    {
    case CLI_MODE_USER:
        vosPrintf(pstEnv->nWriteFd, ">");
        break;

    case CLI_MODE_ENABLE:
        vosPrintf(pstEnv->nWriteFd, "#");
        break;

    case CLI_MODE_CONFIG_SYS:
        vosPrintf(pstEnv->nWriteFd, "(sys)#");
        break;

    case CLI_MODE_CONFIG_USER:
        vosPrintf(pstEnv->nWriteFd, "(user)#");
        break;

    case CLI_MODE_CONFIG_QOS:
        vosPrintf(pstEnv->nWriteFd, "(qos)#");
        break;

    case CLI_MODE_CONFIG_FDB:
        vosPrintf(pstEnv->nWriteFd, "(fdb)#");
        break;

    case CLI_MODE_CONFIG_MCAST:
        vosPrintf(pstEnv->nWriteFd, "(mcast)#");
        break;

    case CLI_MODE_CONFIG_VLAN:
        vosPrintf(pstEnv->nWriteFd, "(vlan)#");
        break;

    case CLI_MODE_CONFIG_PORT:
        if (NULL != pstEnv->para)
        {
            vosPrintf(pstEnv->nWriteFd, "(port<%s>)#",(char *)pstEnv->para);
        }
        else
        {
            pstEnv->pmode = CLI_MODE_USER;
            vosPrintf(pstEnv->nWriteFd, ">");
        }
        break;

    case CLI_MODE_CONFIG_RSTP:
        vosPrintf(pstEnv->nWriteFd, "(rstp)#");
        break;

    case CLI_MODE_CONFIG_PON:
        vosPrintf(pstEnv->nWriteFd, "(pon)#");
        break;

    case CLI_MODE_CONFIG_STORM:
        vosPrintf(pstEnv->nWriteFd, "(storm)#");
        break;

    case CLI_MODE_CONFIG_QOS_CLASS:
        if (NULL != pstEnv->para)
        {
            vosPrintf(pstEnv->nWriteFd, "(qos-class<%s>)#",(char *)pstEnv->para);
        }
        else
        {
            pstEnv->pmode = CLI_MODE_USER;
            vosPrintf(pstEnv->nWriteFd, ">");
        }
        break;
    case CLI_MODE_CONFIG_MIRROR:
        if (NULL != pstEnv->para)
        {
            vosPrintf(pstEnv->nWriteFd, "(mirror-group<%s>)#",(char *)pstEnv->para);
        }
        else
        {
            pstEnv->pmode = CLI_MODE_USER;
            vosPrintf(pstEnv->nWriteFd, ">");
        }
        break;
    case CLI_MODE_SUPER:
        vosPrintf(pstEnv->nWriteFd, "(super)#");
        break;

    default:
        vosPrintf(pstEnv->nWriteFd, ">");
        break;
    }
}

/*****************************************************************************
 *cliPromptLenGet - Get the prompt string's length.
 *DESCRIPTION
 *
 *Input:pstEnv.
 *Output:N/A.
 *Return:int.
 ****************************************************************************/
int cliPromptLenGet(ENV_t *pstEnv)
{
    int len = strlen(odmSysCfgNameGet()) + 1;

    switch(pstEnv->pmode)
    {
    case CLI_MODE_USER:
        break;

    case CLI_MODE_ENABLE:
        break;

    case CLI_MODE_CONFIG_SYS:
        len += strlen("(sys)#");
        break;

    case CLI_MODE_CONFIG_USER:
        len += strlen("(user)#");
        break;

    case CLI_MODE_CONFIG_QOS:
        len += strlen("(qos)#");
        break;

    case CLI_MODE_CONFIG_FDB:
        len += strlen("(fdb)#");
        break;

    case CLI_MODE_CONFIG_MCAST:
        len += strlen("(mcast)#");
        break;

    case CLI_MODE_CONFIG_VLAN:
        len += strlen("(vlan)#");
        break;

    case CLI_MODE_CONFIG_PORT:
        if (NULL != pstEnv->para)
        {
            len += strlen("(port<>)#")+strlen((char *)pstEnv->para);
        }
        break;

    case CLI_MODE_CONFIG_RSTP:
        len += strlen("(rstp)#");
        break;

    case CLI_MODE_CONFIG_PON:
        len += strlen("(pon)#");
        break;

    case CLI_MODE_CONFIG_STORM:
        len += strlen("(storm)#");
        break;

    case CLI_MODE_CONFIG_QOS_CLASS:
        if (NULL != pstEnv->para)
        {
            len += strlen("(qos-class<>)#")+strlen((char *)pstEnv->para);
        }
        break;
    case CLI_MODE_CONFIG_MIRROR:
        if (NULL != pstEnv->para)
        {
            len += strlen("(mirror-group<>)#")+strlen((char *)pstEnv->para);
        }
        break;
    case CLI_MODE_SUPER:
        len += strlen("(super)#");
        break;

    default:
        break;
    }

    return len;
}

/*****************************************************************************
 *cliCharGet - Getting a character from INPUT terminal.
 *DESCRIPTION
 *Type of terminal is VT-100, or the other.
 *All ASCII characters from 1 to 127 are sent by terminal as is.
 *The following keystrokes are well established:
        Keystroke             Sent
        up arrow key          ESC [ A
        down arrow key        ESC [ B
 *Input:pstEnv.
 *Output:N/A.
 *Return: unsigned short .
 ****************************************************************************/
unsigned short cliCharGet(ENV_t *pstEnv)
{
    int    i;
    int    nCharToGetNum;
    unsigned short usChar = 0xffff;
    char    achTemp[3] = {0, 0, 0};


    if (NULL == pstEnv)
    {
        return(0);
    }

    switch (pstEnv->tmode)
    {
    case TERMINAL_MODE_NORMAL:
        nCharToGetNum = 3;
        break;
    case TERMINAL_MODE_VT100_C:
        nCharToGetNum = 3;
        break;
    case TERMINAL_MODE_VT100_N:
        nCharToGetNum = 3;
        break;
    default:
        break;
    }

    for (i=1; i<=nCharToGetNum; i++)
    {
        /* failed read from device.*/
        if (vosSafeRead(pstEnv->nReadFd, &achTemp[i-1], 1) < 1)
        {
            pstEnv->err = TERMINAL_IO_FAULT;
            return(0xffff);
        }
        if (cliCharTranslate(pstEnv, achTemp, i, &usChar))
        {
            break;
        }
    }
    return(usChar);
}


/*****************************************************************************
 *cliCharFilter - Filter the input char.
 *DESCRIPTION
 *
 *Input:pusCharToFilter.
 *Output:N/A.
 *Return: N/A .
 ****************************************************************************/
void cliCharFilter(unsigned short *pusCharToFilter)
{

    if (*pusCharToFilter>=32 && *pusCharToFilter<=126)
    {
        return;
    }

    if (!(BSP == *pusCharToFilter || CR == *pusCharToFilter
        || NL == *pusCharToFilter || TAB == *pusCharToFilter
        || CTRLZ == *pusCharToFilter || CTRLC == *pusCharToFilter
        || CTRLP == *pusCharToFilter || CTRLN == *pusCharToFilter
        || IAC == *pusCharToFilter))
    {
        *pusCharToFilter = 0xffff;
    }
}


/*****************************************************************************
 *cliCharTranslate - Translate char.
 *DESCRIPTION
 *VT-100 to user-defined.
 *Input:pstEnv,pchTemp,nSrcNum,pusChar.
 *Output:N/A.
 *Return: int .
 ****************************************************************************/
int cliCharTranslate(ENV_t *pstEnv, char *pchTemp, int nSrcNum, unsigned short *pusChar)
{
    if (nSrcNum > 3)
    {
        *pusChar = 0xffff;
        return 1;
    }

    switch (pstEnv->tmode)
    {
    case TERMINAL_MODE_NORMAL:
    case TERMINAL_MODE_VT100_N:   /* ONLY support ESC+'['+'A'(or 'B') */
        if (pchTemp[0] != 27)
        {
            *pusChar = (unsigned char)pchTemp[0];
            cliCharFilter(pusChar);
            return(1);
        }
        if (nSrcNum < 2)
        {
            return(0);
        }
        if (!('[' == pchTemp[1] || 'O' == pchTemp[1]))
        {
            *pusChar = 0xffff;
            return(1);
        }
        if (nSrcNum < 3)
        {
            return(0);
        }
        if ('A' == pchTemp[2])
        {
            *pusChar = CURSOR_UP;
            return(1);
        }
        if ('B' == pchTemp[2])
        {
            *pusChar = CURSOR_DOWN;
            return(1);
        }
        if ('C' == pchTemp[2])
        {
            *pusChar = CURSOR_RIGHT;
            return(1);
        }
        if ('D' == pchTemp[2])
        {
            *pusChar = CURSOR_LEFT;
            return(1);
        }
        break;
     default:
        break;
    }
    *pusChar = 0xffff;
    return(1);
}


/*****************************************************************************
 *cliForwardIndentNoLeftFlag - Forward indent Left Flag.
 *DESCRIPTION
 *
 *Input:pstEnv.
 *Output:N/A.
 *Return: N/A.
 ****************************************************************************/
void cliForwardIndentNoLeftFlag(ENV_t *pstEnv)
{
    int   i;
    int   nBufIndex;
    int   nLeftCnt = 0;
    int   nDispCnt = 0;
    int   nBackCnt = 0;
    int   nAdjustCnt = 0;
    char  achBuf[2*MAX_POOL_LENGTH+1];
    CMD_POOL_t *psCmdPool = NULL;


    psCmdPool = pstEnv->command;
    if (TRUE == psCmdPool->bIsRightFlag)
    {
        /* abcd^$ ->cusor is at here */
        if (psCmdPool->nCursorPos >= psCmdPool->nDispLen-1)
        {
            nDispCnt = psCmdPool->nDispLen-1;
            psCmdPool->nDispBegin += INDENTWIDTH;
            psCmdPool->nDispEnd += INDENTWIDTH;
            if (psCmdPool->nInputTailPos > psCmdPool->nDispEnd)
            {
                nLeftCnt = psCmdPool->nDispLen;
                nAdjustCnt = 0;
                nBackCnt = INDENTWIDTH+1;
            }
            else
            {
                psCmdPool->nDispEnd = psCmdPool->nInputTailPos;
                psCmdPool->nDispLen = psCmdPool->nDispEnd-psCmdPool->nDispBegin+1;
                nLeftCnt = psCmdPool->nDispLen;
                psCmdPool->bIsRightFlag = FALSE;
                nAdjustCnt = (DISPLAYWIDTH-psCmdPool->nDispLen);
                nBackCnt = INDENTWIDTH-nAdjustCnt;
            }
            /*  Echo characters need to show */
            nBufIndex = 0;
            for (i = 0; i < nDispCnt; i++)
            {
                achBuf[nBufIndex++] = BSP;
            }
            achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
            for (i = 0; i < nLeftCnt; i++)
            {
                achBuf[nBufIndex++] = psCmdPool->achCmdPool[psCmdPool->nDispBegin+i];
            }
            if (TRUE == psCmdPool->bIsRightFlag)
            {
                achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
            }
            /*  Adjust display */
            for (i = 0; i < nAdjustCnt; i++)
            {
                achBuf[nBufIndex++] = BLANKSPACE;
            }
            for (i = 0; i < nAdjustCnt; i++)
            {
               achBuf[nBufIndex++] = BSP;
            }
            if (TRUE == psCmdPool->bIsRightFlag)
            {
                nBackCnt++;
            }
            for (i = 0; i < nBackCnt; i++)
            {
               achBuf[nBufIndex++] = BSP;
            }
            write(pstEnv->nWriteFd, achBuf, nBufIndex);
            psCmdPool->nCursorPos -= INDENTWIDTH;
            psCmdPool->bIsLeftFlag = TRUE;
        }
        else  /*  ^ab^cd$   ->cursor is at here */
        {
            psCmdPool->nDispEnd = DISPLAYWIDTH-psCmdPool->nDispBegin-1;
            psCmdPool->nDispLen = psCmdPool->nDispEnd-psCmdPool->nDispBegin+1;
            nLeftCnt = psCmdPool->nDispEnd - psCmdPool->nCursorPos+1;
            nBufIndex = 0;
            for (i = 0; i < nLeftCnt; i++)
            {
                achBuf[nBufIndex++] = psCmdPool->achCmdPool[psCmdPool->nCursorPos+i];
            }
            achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
            nLeftCnt++;
            for (i = 0; i < nLeftCnt; i++)
            {
                achBuf[nBufIndex++] = BSP;
            }
            write(pstEnv->nWriteFd,achBuf,nBufIndex);
        }
    }
    else
    {
        /*  ^ab^cd   ->cursor is at here */
        if (psCmdPool->nDispLen >= DISPLAYWIDTH)
        {
            psCmdPool->bIsRightFlag = TRUE;
        }

        /* 回显剩下的字符 */
        nLeftCnt = psCmdPool->nDispLen-psCmdPool->nCursorPos;
        if (0 == nLeftCnt)
        {
            achBuf[0] = LEFT_FLAG_CHARACTOR;
            achBuf[1] = BSP;
            achBuf[2] = BSP;
            write (pstEnv->nWriteFd,achBuf,3);
            return;
        }
        nBufIndex = 0;
        for (i = 0; i < nLeftCnt; i++)
        {
            achBuf[nBufIndex++] = psCmdPool->achCmdPool[psCmdPool->nCursorPos+i];
        }
        if (TRUE == psCmdPool->bIsRightFlag)
        {
            achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
            nLeftCnt++;
        }
        for (i = 0; i < nLeftCnt; i++)
        {
            achBuf[nBufIndex++] = BSP;
        }
        write(pstEnv->nWriteFd,achBuf,nBufIndex);
    }
    return;
 }

/*****************************************************************************
 *cliForwardIndentLeftFlag - Forward indent Left Flag.
 *DESCRIPTION
 *
 *Input:pstEnv.
 *Output:N/A.
 *Return: N/A.
 ****************************************************************************/
void cliForwardIndentLeftFlag(ENV_t *pstEnv)
{
    int   i;
    int   nBufIndex;
    int   nLeftCnt = 0;
    int   nDispCnt = 0;
    int   nBackCnt = 0;
    int   nAdjustCnt = 0;
    char  achBuf[2*MAX_POOL_LENGTH+1];
    CMD_POOL_t *psCmdPool = NULL;


    psCmdPool = pstEnv->command;
    if (TRUE == psCmdPool->bIsRightFlag)
    {
        /*  $^abc^d$ ->cursor is at here */
        if (psCmdPool->nCursorPos < DISPLAYWIDTH)
        {
            psCmdPool->nDispEnd--;
            psCmdPool->nDispLen--;
            nLeftCnt = psCmdPool->nDispLen-psCmdPool->nCursorPos;

            nBufIndex = 0;
            for (i = 0; i < nLeftCnt; i++)
            {
                achBuf[nBufIndex++] = psCmdPool->achCmdPool[psCmdPool->nInputPos+i];
            }
            achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
            nLeftCnt++;
            for (i = 0; i < nLeftCnt; i++)
            {
                achBuf[nBufIndex++] = BSP;
            }
            write(pstEnv->nWriteFd, achBuf, nBufIndex);
        }
        else   /* $abcd^$ */
        {
            psCmdPool->nDispBegin += INDENTWIDTH;
            psCmdPool->nDispEnd += INDENTWIDTH;
            psCmdPool->nDispEnd--;
            psCmdPool->nDispLen--;
            psCmdPool->nCursorPos -=INDENTWIDTH;
            nLeftCnt = psCmdPool->nDispLen;
            if (psCmdPool->nDispEnd > psCmdPool->nInputTailPos-1)
            {
                psCmdPool->nDispEnd = psCmdPool->nInputTailPos;
                psCmdPool->nDispLen
                = psCmdPool->nDispEnd-psCmdPool->nDispBegin+1;
                psCmdPool->bIsRightFlag = FALSE;
                nAdjustCnt = DISPLAYWIDTH-psCmdPool->nDispLen+1;
                nBackCnt = INDENTWIDTH-nAdjustCnt+1;
            }
            else
            {
                nAdjustCnt = 0;
                nBackCnt = INDENTWIDTH;
            }
            nDispCnt = psCmdPool->nDispLen;
            nBufIndex = 0;
            for (i = 0; i < nLeftCnt; i++)
            {
                achBuf[nBufIndex++] = BSP;
            }
            for (i = 0; i < nDispCnt; i++)
            {
                achBuf[nBufIndex++]
                = psCmdPool->achCmdPool[psCmdPool->nDispBegin+i];
            }
            if (TRUE == psCmdPool->bIsRightFlag)
            {
                achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
                nAdjustCnt++;
                nBackCnt++;
            }
            /* Adjust display */
            for (i = 0; i < nAdjustCnt; i++)
            {
                achBuf[nBufIndex++] = BLANKSPACE;
            }
            for (i = 0; i < nAdjustCnt; i++)
            {
                achBuf[nBufIndex++] = BSP;
            }
            for (i = 0; i < nBackCnt; i++)
            {
                achBuf[nBufIndex++] = BSP;
            }
            write(pstEnv->nWriteFd,achBuf,nBufIndex);
        }
    }
    else    /*  $^ab^cd   ->cursor is at here */
    {
        if (psCmdPool->nDispLen > DISPLAYWIDTH)
        {
            psCmdPool->bIsRightFlag = TRUE;
            psCmdPool->nDispEnd = DISPLAYWIDTH+psCmdPool->nDispBegin-1;
            psCmdPool->nDispLen = psCmdPool->nDispEnd-psCmdPool->nDispBegin+1;
        }
        nLeftCnt = psCmdPool->nDispLen-psCmdPool->nCursorPos;
        nBufIndex = 0;
        for (i = 0; i < nLeftCnt; i++)
        {
            achBuf[nBufIndex++] = psCmdPool->achCmdPool[psCmdPool->nInputPos+i];
        }
        if (TRUE == psCmdPool->bIsRightFlag)
        {
            achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
            nLeftCnt++;
        }
        for (i = 0; i < nLeftCnt; i++)
        {
            achBuf[nBufIndex++] = BSP;
        }
        write(pstEnv->nWriteFd, achBuf, nBufIndex);
    }
    return;
}

/*****************************************************************************
 *cliForwardIndent - Forward indent.
 *DESCRIPTION
 *
 *Input:pstEnv.
 *Output:N/A.
 *Return: N/A.
 ****************************************************************************/
void cliForwardIndent(ENV_t *pstEnv)
{
    int   i;
    int   nBufIndex;
    int   nLeftCnt = 0;
    int   nDispCnt = 0;
    int   nBackCnt = 0;
    int   nAdjustCnt = 0;
    char  achBuf[2*MAX_POOL_LENGTH+1];
    CMD_POOL_t *psCmdPool = NULL;


    psCmdPool = pstEnv->command;
    if (TRUE == psCmdPool->bIsInsert)  /* Insert mode */
    {
        if (FALSE == psCmdPool->bIsLeftFlag)
        {
            /*  ^ab^cd^$   ->cursor is at here */
            cliForwardIndentNoLeftFlag(pstEnv);
        }
        else
        {
            /*  $^ab^cd^$   ->cursor is at here */
            cliForwardIndentLeftFlag(pstEnv);
        }
    }/* End of inserting mode*/

    nDispCnt = psCmdPool->nDispLen;
    if (nDispCnt <= DISPLAYWIDTH)
    {
        return;
    }
    psCmdPool->nDispBegin += INDENTWIDTH;
    nBufIndex = 0;
    for (i = 0; i < nDispCnt; i++)
    {
        achBuf[nBufIndex++] = BSP;
    }
    if (TRUE == psCmdPool->bIsLeftFlag)
    {
        achBuf[nBufIndex++] = BSP;
    }
    achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
    nDispCnt = psCmdPool->nDispLen;
    for (i = 0;i < nDispCnt; i++)
    {
        achBuf[nBufIndex++] = psCmdPool->achCmdPool[psCmdPool->nDispBegin+i];
    }
    for (i = 0; i < INDENTWIDTH; i++)
    {
        achBuf[nBufIndex++] = BLANKSPACE;
    }
    for (i = 0; i < INDENTWIDTH; i++)
    {
        achBuf[nBufIndex++] = BSP;
    }
    write(pstEnv->nWriteFd, achBuf, nBufIndex);
    psCmdPool->nCursorPos -= INDENTWIDTH;
    psCmdPool->bIsLeftFlag = TRUE;
    psCmdPool->nDispLen -= INDENTWIDTH;
}

/*****************************************************************************
 *cliBackwardIndentNoLeftFlag - Backward indent No Left Flag.
 *DESCRIPTION
 *
 *Input:pstEnv.
 *Output:N/A.
 *Return: N/A .
 ****************************************************************************/
void cliBackwardIndentNoLeftFlag(ENV_t *pstEnv)
{
    int  i;
    int  nBufIndex = 0;
    int  nLeftCnt = 0;
    int  nBackCnt = 0;
    int  nDispCnt = 0;
    char achBuf[2*MAX_POOL_LENGTH+1];
    CMD_POOL_t *psCmdPool = NULL;


    psCmdPool = pstEnv->command;
    if (TRUE == psCmdPool->bIsRightFlag)
    {
        psCmdPool->nDispEnd++;
        psCmdPool->nDispLen++;
        nLeftCnt = psCmdPool->nDispLen-psCmdPool->nCursorPos;
        nBufIndex = 0;
        for (i = 0; i<nLeftCnt; i++)
        {
            achBuf[nBufIndex++]
            = psCmdPool->achCmdPool[psCmdPool->nInputPos+i];
        }
        if (psCmdPool->nInputTailPos == psCmdPool->nDispEnd)
        {
            psCmdPool->bIsRightFlag = FALSE;
            achBuf[nBufIndex++] = BLANKSPACE;
            achBuf[nBufIndex++] = BSP;
        }
        for (i = 0; i < nLeftCnt; i++)
        {
            achBuf[nBufIndex++] = BSP;
        }
        write(pstEnv->nWriteFd,achBuf,nBufIndex);
        return;
    }
    else      /*   123456789.......^123456789^   del at here!!   */
    {
        /* The cursor in the end of command */
        if (psCmdPool->nDispBegin+psCmdPool->nCursorPos
            == psCmdPool->nInputTailPos+1)
        {
            return;
        }
        else    /*  The cursor is in the middle of command. */
        {
            nLeftCnt = psCmdPool->nDispLen-psCmdPool->nCursorPos;
            nBufIndex = 0;
            for (i = 0; i < nLeftCnt; i++)
            {
                achBuf[nBufIndex++]
                = psCmdPool->achCmdPool[psCmdPool->nInputPos+i];
            }
            achBuf[nBufIndex++] = BLANKSPACE;
            nLeftCnt++;
            for (i = 0; i < nLeftCnt; i++)
            {
                achBuf[nBufIndex++] = BSP;
            }
            write(pstEnv->nWriteFd,achBuf,nBufIndex);
            return;
        }
    }
}

/*****************************************************************************
 *cliBackwardIndentLeftRightFlag - Backward indent Left-Right Flag.
 *DESCRIPTION
 *
 *Input:pstEnv.
 *Output:N/A.
 *Return: N/A.
 ****************************************************************************/
void cliBackwardIndentLeftRightFlag(ENV_t *pstEnv)
{
    int  i;
    int  nBufIndex = 0;
    int  nLeftCnt = 0;
    int  nBackCnt = 0;
    int  nDispCnt = 0;
    char achBuf[2*MAX_POOL_LENGTH+1];
    CMD_POOL_t *psCmdPool = NULL;


    psCmdPool = pstEnv->command;
    if (0 == psCmdPool->nCursorPos)
    {
        nBufIndex = 0;
        nBackCnt = 0;
       /*if move to head of command,Not delete to head of command*/
        if (psCmdPool->nDispBegin > psCmdPool->nInputPos)
        {
            if (0 == (psCmdPool->nDispBegin-INDENTWIDTH))
            {
                psCmdPool->bIsLeftFlag = FALSE;
            }
            else
            {
                achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
            }
            nBackCnt++;
            psCmdPool->nCursorPos--;
        }
        else
        {
            if (0 == psCmdPool->nDispBegin-INDENTWIDTH)
            {
                psCmdPool->bIsLeftFlag = FALSE;
                achBuf[nBufIndex++] = BSP;
            }
        }
        psCmdPool->nCursorPos += INDENTWIDTH;
        psCmdPool->nDispBegin -= INDENTWIDTH;
        psCmdPool->nDispLen += INDENTWIDTH;
        nDispCnt = psCmdPool->nDispLen;
        if (nDispCnt > DISPLAYWIDTH)
        {
            nDispCnt = DISPLAYWIDTH;
            psCmdPool->bIsRightFlag = TRUE;
            psCmdPool->nDispEnd = psCmdPool->nDispBegin+DISPLAYWIDTH-1;
            psCmdPool->nDispLen = DISPLAYWIDTH;
        }
        nBackCnt += nDispCnt-INDENTWIDTH;
        for (i = 0; i < nDispCnt; i++)
        {
            achBuf[nBufIndex++] = psCmdPool->achCmdPool[psCmdPool->nDispBegin+i];
        }
        if (TRUE == psCmdPool->bIsRightFlag)
        {
            achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
            nBackCnt++;
        }
        achBuf[nBufIndex++] = BLANKSPACE;
        achBuf[nBufIndex++] = BSP;
        for (i = 0; i < nBackCnt; i++)
        {
            achBuf[nBufIndex++] = BSP;
        }
        write(pstEnv->nWriteFd, achBuf, nBufIndex);
        return;
    }
    else  /* $123456789.......^123456789^$   del at here!! */
    {
        psCmdPool->nDispEnd++;
        psCmdPool->nDispLen++;
        nDispCnt = psCmdPool->nDispLen-psCmdPool->nCursorPos;
        nBackCnt = nDispCnt;
        nBufIndex = 0;
        for (i = 0; i < nDispCnt; i++)
        {
            achBuf[nBufIndex++] = psCmdPool->achCmdPool[psCmdPool->nInputPos+i];
        }
        if (psCmdPool->nInputTailPos == psCmdPool->nDispEnd)
        {
            psCmdPool->bIsRightFlag = FALSE;
            achBuf[nBufIndex++] = BLANKSPACE;
            nBackCnt++;
        }
        for (i = 0; i < nBackCnt; i++)
        {
            achBuf[nBufIndex++] = BSP;
        }
        write(pstEnv->nWriteFd,achBuf,nBufIndex);
        return;
    }
}

/*****************************************************************************
 *cliBackwardIndentLeftNoRightFlag - Backward indent Left-NoRight Flag.
 *DESCRIPTION
 *
 *Input:pstEnv.
 *Output:
 *Return: N/A .
 ****************************************************************************/
void cliBackwardIndentLeftNoRightFlag(ENV_t *pstEnv)
{
    int  i;
    int  nBufIndex = 0;
    int  nLeftCnt = 0;
    int  nBackCnt = 0;
    int  nDispCnt = 0;
    char achBuf[2*MAX_POOL_LENGTH+1];
    CMD_POOL_t *psCmdPool = NULL;


    psCmdPool = pstEnv->command;
    /* The cursor in the end of command */
    if (psCmdPool->nInputPos-1 == psCmdPool->nInputTailPos)
    {
        if (0 == psCmdPool->nCursorPos)
        {
            nBufIndex = 0;
            psCmdPool->nDispBegin -= INDENTWIDTH;
            if (0 == psCmdPool->nDispBegin)
            {
                achBuf[nBufIndex++] = BSP;
                psCmdPool->bIsLeftFlag = FALSE;
            }

            for (i = 0; i < INDENTWIDTH; i++)
            {
                achBuf[nBufIndex++] = psCmdPool->achCmdPool[psCmdPool->nDispBegin+i];
            }
            psCmdPool->nCursorPos +=INDENTWIDTH;
            write(pstEnv->nWriteFd, achBuf, nBufIndex);
            return;
        }
        return;
    }
    else  /*  The cursor is in the middle of command. */
    {
        /*  cursor now is at the head of command  */
        nBufIndex = 0;
        nBackCnt = 0;
        if (0 == psCmdPool->nCursorPos)
        {   /*if move to head of command,Not delete to head of command*/
            if (psCmdPool->nDispBegin > psCmdPool->nInputPos)
            {
                if (0 == (psCmdPool->nDispBegin-INDENTWIDTH))
                {
                    psCmdPool->bIsLeftFlag = FALSE;
                }
                else
                {
                    achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
                }
                nBackCnt++;
                psCmdPool->nCursorPos--;
            }
            else
            {
                if (0 == psCmdPool->nDispBegin-INDENTWIDTH)
                {
                    psCmdPool->bIsLeftFlag = FALSE;
                    achBuf[nBufIndex++] = BSP;
                }
            }
            psCmdPool->nCursorPos += INDENTWIDTH;
            psCmdPool->nDispBegin -= INDENTWIDTH;
            psCmdPool->nDispLen += INDENTWIDTH;
            nDispCnt = psCmdPool->nDispLen;
            if (nDispCnt > DISPLAYWIDTH)
            {
                nDispCnt = DISPLAYWIDTH;
                psCmdPool->bIsRightFlag = TRUE;
                psCmdPool->nDispEnd = psCmdPool->nDispBegin+DISPLAYWIDTH-1;
                psCmdPool->nDispLen = DISPLAYWIDTH;
            }
            for (i = 0; i < nDispCnt; i++)
            {
                achBuf[nBufIndex++] = psCmdPool->achCmdPool[psCmdPool->nDispBegin+i];
            }
            nBackCnt += nDispCnt-INDENTWIDTH;
            if (TRUE == psCmdPool->bIsRightFlag)
            {
                achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
                nBackCnt++;
            }
            achBuf[nBufIndex++] = BLANKSPACE;
            achBuf[nBufIndex++] = BSP;
            for (i = 0; i < nBackCnt; i++)
            {
                achBuf[nBufIndex++] = BSP;
            }
            write(pstEnv->nWriteFd,achBuf,nBufIndex);
            return;
        }
        else
        {
            nLeftCnt = psCmdPool->nDispLen-psCmdPool->nCursorPos;
            nBufIndex = 0;
            for (i = 0; i < nLeftCnt; i++)
            {
                achBuf[nBufIndex++] = psCmdPool->achCmdPool[psCmdPool->nInputPos+i];

            }
            achBuf[nBufIndex++] = BLANKSPACE;
            achBuf[nBufIndex++] = BSP;
            for (i = 0; i < nLeftCnt; i++)
            {
                achBuf[nBufIndex++] = BSP;
            }
            write(pstEnv->nWriteFd, achBuf, nBufIndex);
            return ;
        }
    }
}

void cliBackwardIndent(ENV_t *pstEnv)
{
    int  i;
    int  nLeftCnt = 0;
    int  nBackCnt = 0;
    int  nDispCnt = 0;
    int  nBufIndex = 0;
    char achBuf[2*MAX_POOL_LENGTH+1];
    CMD_POOL_t *psCmdPool = NULL;


    psCmdPool = pstEnv->command;
    if (cliCmdPoolIsEmpty(psCmdPool))  /*  No char to del */
        return;

    if (TRUE == psCmdPool->bIsLeftFlag)
    {
        /*  $^123456789.......^123456789^$   del at here!!  */
        if (TRUE == psCmdPool->bIsRightFlag)
        {
            /*   $^123456789.......123456789$   del at here!! */
            cliBackwardIndentLeftRightFlag(pstEnv);
        }
        else
        {
            /*  $^123456789.......^123456789^   del at here!!  */
            cliBackwardIndentLeftNoRightFlag(pstEnv);
        }
    }
    else
    {
        /*   123456789.......^123456789^$   del at here!!   */
        cliBackwardIndentNoLeftFlag(pstEnv);
    }
    return;
}


/*****************************************************************************
 *cliCursorMoveToRight - To handle cursor move to right.
 ****************************************************************************/
void cliCursorMoveToRight (ENV_t *pstEnv)
{
    int  i;
    int  nBufIndex = 0;
    int  nDispCnt = 0;
    int  nBackCnt = 0;
    char achBuf[2*MAX_POOL_LENGTH+1];
    int  nAdjustCnt = 0;
    CMD_POOL_t *psCmdPool = NULL;


    psCmdPool = pstEnv->command;
    if (TRUE == cliCmdPoolIsEmpty(psCmdPool))
    {
        return;
    }
    psCmdPool->bIsPosAtHead = FALSE;
    if (psCmdPool->nDispLen == psCmdPool->nCursorPos)
    {
        psCmdPool->bIsInsert = FALSE;
        psCmdPool->bIsPosAtHead = FALSE;
        psCmdPool->bIsPosAtTail = TRUE;
        return;
    }
    if (psCmdPool->nCursorPos < (DISPLAYWIDTH-1)
        ||(((DISPLAYWIDTH-1) == psCmdPool->nCursorPos)
        &&(FALSE == psCmdPool->bIsRightFlag)))
    {
        achBuf[0] = psCmdPool->achCmdPool[psCmdPool->nInputPos];
        psCmdPool->nCursorPos++;
        psCmdPool->nInputPos++;
        write (pstEnv->nWriteFd, achBuf, 1);
        if (psCmdPool->nDispLen == psCmdPool->nCursorPos)
        {
            psCmdPool->bIsInsert = FALSE;
            psCmdPool->bIsPosAtHead = FALSE;
        }
    }
    else
    {
        nBufIndex = 0;
        if (TRUE == psCmdPool->bIsLeftFlag)
        {
            achBuf[nBufIndex++] = BSP;
        }
        nBackCnt = psCmdPool->nDispLen-1;  /* 需要回退 nBackCnt */
        for (i = 0; i < nBackCnt; i++)
        {
             achBuf[nBufIndex++] = BSP;
        }

        psCmdPool->nInputPos++;
        psCmdPool->nCursorPos -= (INDENTWIDTH-1);
        psCmdPool->nDispBegin += INDENTWIDTH;
        psCmdPool->bIsLeftFlag = TRUE;
        /* 判断是否右边还有是溢出模式?? */
        if (psCmdPool->nInputTailPos-INDENTWIDTH > psCmdPool->nInputPos-1)
        {
            psCmdPool->nDispEnd += INDENTWIDTH;
            nAdjustCnt = INDENTWIDTH+1;
        }
        else  /* 不是右边溢出模式 */
        {
            psCmdPool->bIsRightFlag = FALSE;
            psCmdPool->nDispEnd = psCmdPool->nInputTailPos;
            psCmdPool->nDispLen = psCmdPool->nDispEnd-psCmdPool->nDispBegin+1;
            nAdjustCnt = INDENTWIDTH-(DISPLAYWIDTH-psCmdPool->nDispLen);
        }
        achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
        nDispCnt = psCmdPool->nDispLen;
        for (i = 0; i < nDispCnt; i++)
        {
            achBuf[nBufIndex++] = psCmdPool->achCmdPool[psCmdPool->nDispBegin+i];
        }
        if (TRUE == psCmdPool->bIsRightFlag)
        {
            achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
        }
        else
        {
            for (i = 0; i < DISPLAYWIDTH-nDispCnt+1; i++)
            {
                achBuf[nBufIndex++] = BLANKSPACE;
            }
            for (i = 0; i < DISPLAYWIDTH-nDispCnt+1; i++)
            {
                achBuf[nBufIndex++] = BSP;
            }
        }
        for (i = 0; i < nAdjustCnt; i++)
        {
            achBuf[nBufIndex++] = BSP;
        }
        write(pstEnv->nWriteFd,achBuf,nBufIndex);
    }
}


/*****************************************************************************
 *cliCursorMoveToLeft - To handle cursor move to left.
 ****************************************************************************/
void cliCursorMoveToLeft (ENV_t *pstEnv)
{
  int  i;
  int  nBufIndex;
  char achBuf[2*MAX_POOL_LENGTH+1];
  int  nLeftCnt = 0;
  int  nDispCnt = 0;
  int  nBackCnt = 0;
  CMD_POOL_t *psCmdPool = NULL;


  psCmdPool = pstEnv->command;
  if (TRUE == cliCmdPoolIsEmpty(psCmdPool))
  {
      return;
  }
  psCmdPool->bIsInsert = TRUE;
  psCmdPool->bIsPosAtTail = FALSE;
  if (TRUE == psCmdPool->bIsPosAtHead)
  {
      return;
  }
  if (psCmdPool->nCursorPos > 0)
  {
      achBuf[0] = BSP;
      write(pstEnv->nWriteFd, achBuf, 1);
      psCmdPool->nCursorPos--;
      psCmdPool->nInputPos--;
      if (0 == psCmdPool->nDispBegin && 0 == psCmdPool->nCursorPos)
      {
          psCmdPool->bIsPosAtHead = TRUE;
      }
      return;
  }
  else  /* $^abcd...ef  or ^abc...ef --cursor at here */
  {
      if (0 == psCmdPool->nDispBegin) /* ^abc...ef ,So do nothing */
      {
          psCmdPool->bIsPosAtHead = TRUE;
          return;
      }
      if (psCmdPool->nDispBegin >= INDENTWIDTH)
      {
          psCmdPool->nInputPos--;
          psCmdPool->nCursorPos--;
          psCmdPool->nDispBegin -= INDENTWIDTH;
          psCmdPool->nDispLen += INDENTWIDTH;
          psCmdPool->nCursorPos +=INDENTWIDTH;
          if (0 == psCmdPool->nDispBegin)
          {
              psCmdPool->bIsLeftFlag = FALSE;
          }
          if (psCmdPool->nDispLen >= DISPLAYWIDTH)
          {
              psCmdPool->bIsRightFlag = TRUE;
              psCmdPool->nDispLen = DISPLAYWIDTH;
              psCmdPool->nDispEnd = psCmdPool->nDispBegin+DISPLAYWIDTH-1;
          }
          nDispCnt = psCmdPool->nDispLen;
          nBufIndex = 0;
          if (FALSE == psCmdPool->bIsLeftFlag)
          {
              achBuf[nBufIndex++] = BSP;
          }
          for (i = 0; i < nDispCnt; i++)
          {
              achBuf[nBufIndex++]
              = psCmdPool->achCmdPool[psCmdPool->nDispBegin+i];
          }
          nBackCnt = nDispCnt-INDENTWIDTH+1;
          if (TRUE == psCmdPool->bIsRightFlag)
          {
              achBuf[nBufIndex++] = LEFT_FLAG_CHARACTOR;
              if (FALSE == psCmdPool->bIsLeftFlag)
              {
                  achBuf[nBufIndex++] = BLANKSPACE;
                  nBackCnt++;
              }
              achBuf[nBufIndex++] = BSP;
          }
          for (i = 0; i < nBackCnt; i++)
          {
              achBuf[nBufIndex++] = BSP;
          }
          write(pstEnv->nWriteFd, achBuf, nBufIndex);
      }
   }
}


/*****************************************************************************
 *cliEcho - Echo characters on the OUTPUT terminal
 ****************************************************************************/
void cliEcho(ENV_t *pstEnv, unsigned short usChar)
{
    int  nCharNum;
    char achTemp[3];


    achTemp[0] = usChar & 0x00ff;
    if (pstEnv->echo == TRUE)
    {
        switch(usChar)
        {
        case CURSOR_UP:
            achTemp[0] = 0x27;
            achTemp[1] = '[';
            achTemp[2] = 'A';
            nCharNum = 3;
            break;
        case CURSOR_DOWN:
            achTemp[0] = 0x27;
            achTemp[1] = '[';
            achTemp[2] = 'B';
            nCharNum = 3;
            break;
        case CURSOR_RIGHT:
            break;
        case CURSOR_LEFT:
            break;
        case BSP:
            achTemp[0] = BSP;
            achTemp[1] = ' ';
            achTemp[2] = BSP;
            nCharNum = 3;
            break;
        default:
            nCharNum = 1;
            break;
        }
        write(pstEnv->nWriteFd, &achTemp[0], nCharNum);
        if ('\n' == usChar)
        {
            return;
        }
        if (usChar != BSP)
        {
            cliForwardIndent(pstEnv);
        }
        else
        {
            cliBackwardIndent(pstEnv);
        }
    }
}


/*****************************************************************************
 *cliEchoEnable - Echo enable.
 ****************************************************************************/
void cliEchoEnable(ENV_t *pstEnv)
{
    pstEnv->echo = TRUE;
}

/*****************************************************************************
 *cliEchoDisable - Echo disable.
 ****************************************************************************/
void cliEchoDisable(ENV_t *pstEnv)
{
    pstEnv->echo = FALSE;
}

void cliNewLineOutput(ENV_t *pstEnv)
{
    if (!pstEnv->ucIsConsole)
    {
        vosPutChar(pstEnv->nWriteFd, '\r');
    }
    vosPutChar(pstEnv->nWriteFd, '\n');
}

/*****************************************************************************
 *cliCmdPush - Push character into the command pool.
 *DESCRIPTION
 *
 *Input:usChar,psCmdPool.
 *Output:N/A.
 *Return: STATUS.
 ****************************************************************************/
STATUS  cliCmdPush(unsigned short usChar, CMD_POOL_t *psCmdPool)
{
    int i;
    int nLeftCnt = 0;


    if (NULL == psCmdPool)
        return OK;

    if ((psCmdPool->nInputTailPos >= MAX_POOL_LENGTH-1) && (!(('?' == usChar)
            ||(CR == usChar)||(NL == usChar)||(TAB == usChar))))
        return ERROR;

    if (TRUE == psCmdPool->bIsInsert) /* Insert mode */
    {
        nLeftCnt = psCmdPool->nInputTailPos - psCmdPool->nInputPos+1;
        for (i = 0; i < nLeftCnt; i++)
        {
            psCmdPool->achCmdPool[psCmdPool->nInputTailPos-i+1]
            = psCmdPool->achCmdPool[psCmdPool->nInputTailPos-i];
        }
        psCmdPool->achCmdPool[psCmdPool->nInputPos] = (char)usChar & 0xff;

        psCmdPool->nInputPos++;
        psCmdPool->nInputTailPos++;
        psCmdPool->nCursorPos++;
        psCmdPool->nDispEnd++;
        psCmdPool->nInputLen++;
        psCmdPool->nDispLen++;
    }
    else  /* Not insert mode */
    {
        if (TRUE == psCmdPool->bIsEmpty)
        {
            psCmdPool->bIsEmpty = FALSE;
            psCmdPool->achCmdPool[0] = (char)usChar & 0xff;

            psCmdPool->nDispBegin = 0;
            psCmdPool->nDispEnd = 0;
            psCmdPool->nInputTailPos = 0;
            psCmdPool->nCursorPos = 1;
            psCmdPool->nInputPos = 1;
            psCmdPool->nInputLen = 1;
            psCmdPool->nDispLen = 1;
        }
        else
        {
            psCmdPool->achCmdPool[psCmdPool->nInputPos] = (char)usChar & 0xff;

            psCmdPool->nCursorPos++;
            psCmdPool->nDispEnd++;
            psCmdPool->nInputPos++;
            psCmdPool->nInputTailPos++;
            psCmdPool->nInputLen++;
            psCmdPool->nDispLen++;
        }
    }
    psCmdPool->bIsPosAtHead = FALSE;
    return OK;
}

/*****************************************************************************
 *cliCmdPop - remove the last character from pool.
 ****************************************************************************/
char cliCmdPop(CMD_POOL_t *psCmdPool)
{
    int  i;
    char chRetChar = 0x00;
    int  nLeftCnt = 0;


    if (NULL == psCmdPool)
    {
        return(chRetChar);
    }
    /* If pool is empty,then return 0x00 */
    if (TRUE == psCmdPool->bIsEmpty || TRUE == psCmdPool->bIsPosAtHead)
    {
        return(chRetChar);
    }

    if (TRUE == psCmdPool->bIsInsert) /* Insert mode */
    {
        if (psCmdPool->nCursorPos > 1)
        {
            nLeftCnt = psCmdPool->nInputTailPos-psCmdPool->nInputPos+1;
            chRetChar = psCmdPool->achCmdPool[psCmdPool->nInputPos-1];

            for (i = 0; i < nLeftCnt; i++)
            {
                psCmdPool->achCmdPool[psCmdPool->nInputPos-1+i]
                = psCmdPool->achCmdPool[psCmdPool->nInputPos+i];
            }
            psCmdPool->achCmdPool[psCmdPool->nInputTailPos] = 0x00;
            psCmdPool->nInputPos--;
            psCmdPool->nDispEnd--;
            psCmdPool->nInputTailPos--;
            psCmdPool->nCursorPos--;
            psCmdPool->nInputLen--;
            psCmdPool->nDispLen--;
        }
        else
        {
            if (TRUE == psCmdPool->bIsLeftFlag)
            {
                nLeftCnt = psCmdPool->nInputTailPos-psCmdPool->nInputPos+1;
                chRetChar = psCmdPool->achCmdPool[psCmdPool->nInputPos-1];

                for (i = 0; i < nLeftCnt; i++)
                {
                    psCmdPool->achCmdPool[psCmdPool->nInputPos-1+i]
                    = psCmdPool->achCmdPool[psCmdPool->nInputPos+i];
                }
                psCmdPool->achCmdPool[psCmdPool->nInputTailPos] = 0x00;
                psCmdPool->nInputPos--;
                psCmdPool->nDispEnd--;
                psCmdPool->nInputTailPos--;
                if (psCmdPool->nCursorPos > 0)
                {
                    psCmdPool->nCursorPos--;
                }
                psCmdPool->nInputLen--;
                psCmdPool->nDispLen--;
            }
            else
            {
                nLeftCnt = psCmdPool->nInputTailPos-psCmdPool->nInputPos+1;
                chRetChar = psCmdPool->achCmdPool[psCmdPool->nInputPos-1];

                for (i = 0; i < nLeftCnt; i++)
                {
                    psCmdPool->achCmdPool[psCmdPool->nInputPos-1+i]
                    = psCmdPool->achCmdPool[psCmdPool->nInputPos+i];
                }
                psCmdPool->achCmdPool[psCmdPool->nInputTailPos] = 0x00;
                psCmdPool->nInputPos = 0;
                psCmdPool->nCursorPos = 0;
                psCmdPool->nInputTailPos--;
                psCmdPool->nDispEnd--;
                psCmdPool->nInputLen--;
                psCmdPool->nDispLen--;
                psCmdPool->bIsPosAtHead = TRUE;
            }
        }
    }
    else     /* Not insert mode */
    {
        chRetChar = psCmdPool->achCmdPool[psCmdPool->nInputTailPos];
        psCmdPool->achCmdPool[psCmdPool->nInputTailPos] = 0x00;

        if (psCmdPool->nInputTailPos > 0)
        {
            psCmdPool->nInputTailPos--;
        }
        else
        {
            psCmdPool->bIsEmpty = TRUE;
        }
        if (psCmdPool->nInputPos > 0)
        {
            psCmdPool->nInputPos--;
        }
        if (psCmdPool->nDispEnd > 0)
        {
            psCmdPool->nDispEnd--;
        }
        if (psCmdPool->nCursorPos > 0)
        {
            psCmdPool->nCursorPos--;
        }
        psCmdPool->nInputLen--;
        psCmdPool->nDispLen--;
    }
    return(chRetChar);
}

/*****************************************************************************
 *cliCmdPoolIsEmpty - To judge pool is empty or not.
 ****************************************************************************/
BOOL cliCmdPoolIsEmpty(CMD_POOL_t *psCmdPool)
{
    if (NULL == psCmdPool)
    {
        return TRUE;
    }

    return(psCmdPool->bIsEmpty);
}

/*****************************************************************************
 *cliCmdPoolTailCharGet - Get the last char.
 ****************************************************************************/
char cliCmdPoolTailCharGet(CMD_POOL_t *psCmdPool)
{
    if (NULL == psCmdPool)
    {
        return 0x00;
    }

    return((char)psCmdPool->achCmdPool[psCmdPool->nInputTailPos]);
}

/*****************************************************************************
 *cliCmdPoolClear - Clear pool.
 ****************************************************************************/
void cliCmdPoolClear(CMD_POOL_t *psCmdPool)
{
    if (NULL == psCmdPool)
    {
        return;
    }

    memset(psCmdPool, 0, sizeof(CMD_POOL_t));
    psCmdPool->bIsEmpty = TRUE;
    psCmdPool->bIsFull = FALSE;
}

/*****************************************************************************
 *cliCmdPoolRefresh - Replace the content of command pool with string "cmd".
 ****************************************************************************/
void cliCmdPoolRefresh(ENV_t *pstEnv, char *pchCmd)
{
    char  *pchCmdTemp;
    CMD_POOL_t *psCmdPool = NULL;


    if (NULL == pstEnv || NULL == pchCmd)
    {
        return;
    }
    psCmdPool = pstEnv->command;
    pchCmdTemp = pchCmd;
    while(*pchCmdTemp)
    {
        if (TRUE == psCmdPool->bIsEmpty)
        {
            psCmdPool->bIsEmpty = FALSE;
            psCmdPool->nInputPos = 1;
            psCmdPool->nInputTailPos = 0;
            psCmdPool->nCursorPos = 1;
            psCmdPool->nDispBegin = 0;
            psCmdPool->nDispEnd = 0;
            psCmdPool->nInputLen = 1;
            psCmdPool->nDispLen = 1;
            psCmdPool->achCmdPool[0] = *pchCmdTemp++;
        }
        else
        {
            psCmdPool->achCmdPool[psCmdPool->nInputPos++] = *pchCmdTemp++;
            psCmdPool->nInputTailPos++;
            psCmdPool->nCursorPos++;
            psCmdPool->nDispEnd++;
            psCmdPool->nInputLen++;
            psCmdPool->nDispLen++;
        }
    }

    psCmdPool->achCmdPool[psCmdPool->nInputPos] = '\0';
    if (LINEWIDTH- psCmdPool->nCursorPos-cliPromptLenGet(pstEnv) > 3)
    {
        return;
    }
    psCmdPool->bIsLeftFlag = TRUE;
    while (LINEWIDTH - psCmdPool->nCursorPos-cliPromptLenGet(pstEnv) <= 3)
    {
        psCmdPool->nDispBegin += INDENTWIDTH;
        psCmdPool->nCursorPos -= INDENTWIDTH;
        psCmdPool->nDispLen -= INDENTWIDTH;
    }
    return;
}

/*****************************************************************************
 *cliLineClear - Clear one line in the pool.
 ****************************************************************************/
void cliLineClear(ENV_t *pstEnv)
{
    int    i;
    int    nBufIndex;
    int    nBackCnt = 0;
    int    nDispCnt = 0;
    char   *pchBuf = NULL;
    CMD_POOL_t  *psCmdPool = NULL;


    psCmdPool = pstEnv->command;
    if (psCmdPool->bIsEmpty)    /* no command in pool */
    {
        return;
    }
    nDispCnt = psCmdPool->nDispLen;
    nBackCnt = psCmdPool->nCursorPos;

    if (TRUE == psCmdPool->bIsLeftFlag)
    {
        nBackCnt++;
        nDispCnt++;
    }

    if (TRUE == psCmdPool->bIsRightFlag)
    {
        nDispCnt++;
    }

    pchBuf = (char *)vosAlloc(nDispCnt*3+1);
    memset(pchBuf, 0, nDispCnt*3+1);
    nBufIndex = 0;
    for (i = 0; i < nBackCnt; i++)
    {
        pchBuf[nBufIndex++] = 0x08;    /*  ASCII Code '\b'  */
    }
    for (i = 0; i < nDispCnt; i++)
    {
         pchBuf[nBufIndex++] = 0x20;   /*  ASCII Code ' '  */
    }
    for (i = 0; i < nDispCnt; i++)
    {
        pchBuf[nBufIndex++] = 0x08;    /*  ASCII Code '\b'  */
    }
    write(pstEnv->nWriteFd, pchBuf, nBufIndex);
    vosFree(pchBuf);
    pchBuf = NULL ;
}

/*****************************************************************************
 *cliCmdMatch -  These functions below are mainly for match command
               and collect the matched node.
 *DESCRIPTION
 *
 *Input:pstEnv,psForestRoot,pchWord,ppsNodeCollection .

 *Output:ppsNodeCollection
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliCmdMatch(ENV_t *pstEnv, CMD_KEY_PARA_LINK_t *psForestRoot,
                  char *pchWord, NODE_COLLECTION_t **ppsNodeCollection)
{
    int  i = 0;
    int  nWordLen = 0;
    int  nFlag = ERROR;
    int  nFlagRight = ERROR;
    int  nFlagNext = ERROR;
    BOOL bIsVisual = FALSE;
    NODE_COLLECTION_t   *psNodeLinkHead = NULL;
    NODE_COLLECTION_t   *psNewNode = NULL;
    EXTRA_CONDITION_t  *psExtraNode = NULL;
    CMD_KEY_PARA_LINK_t  *psKeyOrParaTmp = NULL;


    if (NULL == psForestRoot || NULL == ppsNodeCollection)
    {
        return ERROR;
    }
    /*psNodeLinkHead point to the head of return link table*/
    if (NULL == *ppsNodeCollection)
    {
        *ppsNodeCollection = (NODE_COLLECTION_t *)vosAlloc(sizeof(NODE_COLLECTION_t));
        psNodeLinkHead = *ppsNodeCollection;
        if (NULL == psNodeLinkHead)
        {
            return ERROR;
        }
        psNodeLinkHead->last = psNodeLinkHead;
        psNodeLinkHead->next = NULL;
        psNodeLinkHead->p = NULL;
        psNodeLinkHead->total = 0;
    }
    else
    {
        psNodeLinkHead = *ppsNodeCollection;
    }

    switch(psForestRoot->type)
    {
    case NT_KEY:
        if (((pstEnv->pmode & psForestRoot->exec_mode)
            && (pstEnv->pmode != CLI_SPACIFY_SUB_MODE))
            || ((CLI_SPACIFY_SUB_MODE == pstEnv->pmode)
            && (pstEnv->sub_pmode == psForestRoot->exec_sub_mode)))
        {
			/* Add access control */
			if(pstEnv->accessLevel < psForestRoot->access_level)
			{
				return ERROR;
			}
            if (NULL == pchWord)
            {
                nFlag = 0;
            }
            else
            {
                nWordLen = strlen(pchWord);
                if (0 == nWordLen)
                {
                    return ERROR;
                }
                for (i = 0; i < nWordLen; i++)
                {
                    if (pchWord[i] >= 'A' && pchWord[i] <= 'Z')
                    {
                        pchWord[i] = pchWord[i]-'A'+'a';
                    }
                }
                for (i = 0; i < nWordLen && psForestRoot->key[i]; i++)
                {   if (pchWord[i] == psForestRoot->key[i])
                    {
                        continue;
                    }
                    else if(pchWord[i] >= 'a' && pchWord[i] <= 'z' && ((pchWord[i]-'a'+'A') == psForestRoot->key[i]))
                    {
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                if (i == nWordLen)
                {
                    nFlag = 0;
                }
            }

            if (0 == nFlag)
            {
                psNewNode = (NODE_COLLECTION_t *)vosAlloc(sizeof(NODE_COLLECTION_t));
                if (NULL == psNewNode)
                {
                    return ERROR;
                }
                psNewNode->p = psForestRoot;
                psNewNode->last = NULL;
                psNewNode->next = NULL;
                psNewNode->total = 0;
                psNodeLinkHead->last->next=psNewNode;
                psNodeLinkHead->last = psNewNode;
                psNodeLinkHead->total++;
            }
        }

        if (psForestRoot->right)
        {
            nFlagRight = cliCmdMatch(pstEnv,psForestRoot->right,pchWord,ppsNodeCollection);
        }
        if (OK == nFlag || OK == nFlagRight)
        {
            return OK;
        }
        else
        {
            return ERROR;
        }
        break;
    case NT_PARAMETER:
        if (((pstEnv->pmode & psForestRoot->exec_mode)
            && (pstEnv->pmode != CLI_SPACIFY_SUB_MODE))
            || ((CLI_SPACIFY_SUB_MODE == pstEnv->pmode)
            && (pstEnv->sub_pmode == psForestRoot->exec_sub_mode)))
        {
        	/* Add access control */
			if(pstEnv->accessLevel < psForestRoot->access_level)
			{
				return ERROR;
			}

            psNewNode = (NODE_COLLECTION_t *)vosAlloc(sizeof(NODE_COLLECTION_t));
            if (NULL == psNewNode)
            {
                return ERROR;
            }

            psNewNode->p = psForestRoot;
            psNewNode->last = NULL;
            psNewNode->next = NULL;
            psNewNode->total = 0;
            psNodeLinkHead->last->next = psNewNode;
            psNodeLinkHead->last = psNewNode;
            psNodeLinkHead->total++;
        }

        if (psForestRoot->extra_condiction != NULL)
        {
            psExtraNode = psForestRoot->extra_condiction;
            while(psExtraNode != NULL)
            {
                psKeyOrParaTmp = (CMD_KEY_PARA_LINK_t *)psExtraNode->keyorparanode;
                switch(psExtraNode->para_type)
                {
                case EXTRA_TYPE_D:
                    switch(psExtraNode->para.d_para.operate)
                    {
                    case NEQ:
                        break;
                    case EQ:
                        if (psExtraNode->para.d_para.value == atoi(pchWord))
                        {
                            bIsVisual = TRUE;
                        }
                        break;
                    case GT:
                        break;
                    case LT:
                        break;
                    default:
                        break;
                    }
                    break;
                case EXTRA_TYPE_A:
                    break;
                case EXTRA_TYPE_STRING:
                    break;
                default:
                    break;
                }
                psExtraNode = psExtraNode->next;
            }
            if (TRUE == bIsVisual)
            {
                psKeyOrParaTmp->visualable = TRUE;
            }
            else
            {
                psKeyOrParaTmp->visualable = FALSE;
            }
        }

        if (psForestRoot->right)
        {
            nFlag = cliCmdMatch(pstEnv, psForestRoot->right, pchWord, ppsNodeCollection);
        }
        return OK;
        break;
    case NT_VIRTUAL_REQUIRED:
        if (FALSE == psForestRoot->visualable)
        {
            psForestRoot->visualable = TRUE;
            psForestRoot = psForestRoot->next;
            if (psForestRoot != NULL)
            {
                nFlag = cliCmdMatch(pstEnv,psForestRoot,pchWord,ppsNodeCollection);
            }
            if (psForestRoot->explain != NULL)
            {
                nFlag = cliCmdMatch(pstEnv,psForestRoot->explain,pchWord,ppsNodeCollection);
            }
        }
        else
        {
            nFlag = cliCmdMatch(pstEnv,psForestRoot->explain,pchWord,ppsNodeCollection);
        }

        if (psForestRoot->right)
        {
            nFlagRight = cliCmdMatch(pstEnv,psForestRoot->right,pchWord,ppsNodeCollection);
        }
        if (OK == nFlag || OK == nFlagRight)
        {
            return OK;
        }
        else
        {
            return ERROR;
        }
        break;
    case NT_VIRTUAL_OPTIONAL:
        nFlag = cliCmdMatch(pstEnv,psForestRoot->explain,pchWord,ppsNodeCollection);
        if (psForestRoot->right)
        {
            nFlagRight = cliCmdMatch(pstEnv,psForestRoot->right,pchWord,ppsNodeCollection);
        }
        if (psForestRoot->next)
        {
            nFlagNext = cliCmdMatch(pstEnv,psForestRoot->next,pchWord,ppsNodeCollection);
        }
        if (OK == nFlag || OK == nFlagRight || OK == nFlagNext)
        {
            return OK;
        }
        else
        {
            return ERROR;
        }
        break;

    case NT_VIRTUAL_LOAD:
        nFlag = cliCmdMatch(pstEnv,psForestRoot->explain,pchWord,ppsNodeCollection);
        return nFlag;
        break;

    case NT_VIRTUAL_FUNCTION:
        nFlag = cliCmdMatch(pstEnv,psForestRoot->explain,pchWord,ppsNodeCollection);
        if (psForestRoot->right)
        {
            nFlagRight = cliCmdMatch(pstEnv,psForestRoot->right,pchWord,ppsNodeCollection);
        }
        if (OK == nFlag || OK == nFlagRight)
        {
            return OK;
        }
        else
        {
            return ERROR;
        }
        break;

    case NT_VIRTUAL_COMMAND:
        if (((pstEnv->pmode & psForestRoot->exec_mode)
            && (pstEnv->pmode != CLI_SPACIFY_SUB_MODE))
            || ((CLI_SPACIFY_SUB_MODE == pstEnv->pmode)
            && (pstEnv->sub_pmode == psForestRoot->exec_sub_mode)))
        {
        	/* Add access control */
			if(pstEnv->accessLevel < psForestRoot->access_level)
			{
				return ERROR;
			}

            if (NULL == pchWord)
            {
                psNewNode = (NODE_COLLECTION_t *)vosAlloc(sizeof(NODE_COLLECTION_t));
                if (NULL == psNewNode)
                {
                    return ERROR;
                }
                psNewNode->p = psForestRoot;
                psNewNode->last = NULL;
                psNewNode->next = NULL;
                psNewNode->total = 0;
                psNodeLinkHead->last->next = psNewNode;
                psNodeLinkHead->last = psNewNode;
                psNodeLinkHead->total++;
                return OK;
            }
            else
            {
                return ERROR;
            }
        }
        return ERROR;
        break;
    default:
        return ERROR;
        break;
    }
    return OK;
}

/*****************************************************************************
 *cliCmdExecute -  Execute the corresponding interface function.
 *DESCRIPTION
 *
 *Input:pEnv,psParaCollection,psNodeCollection .

 *Output:
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliCmdExecute(ENV_t *pstEnv, PARA_COLLECTION_t *psParaCollection,
                  NODE_COLLECTION_t *psNodeCollection)
{
    int  i = 0;
    int  nParaNum = 0;
    int  nFlag = -1;
    int  *pnVisited = NULL;
    int  bKill = FALSE;
    char chEsc;
    VOS_THREAD_t     pstTaskID = NULL;
    PARA_TABLE_t            *psParaTbl;
    PARA_LINK_t             *psParaLink;
    CMD_KEY_PARA_LINK_t  *psCmdKeyParaLnkTbl;
    CLI_CHILD_TASK_ARG_t   stArg;

    if (NULL == pstEnv || NULL == psNodeCollection)
    {
        return ERROR;
    }
    if (NULL == psNodeCollection->p)
    {
        return ERROR;
    }
    psCmdKeyParaLnkTbl = psNodeCollection->p;
    psParaLink = psCmdKeyParaLnkTbl->func_info->parameter;
    while (psParaLink)
    {
        nParaNum++;
        psParaLink = psParaLink->next;
    }

    if (0 == nParaNum)
    {
        if (*(psCmdKeyParaLnkTbl->func_info->function_point) != NULL)
        {
            if (psCmdKeyParaLnkTbl->func_info->is_task)
            {
               /******************************************/
               /* Add your own codes here                */
               /******************************************/
                pstEnv->closeTask = 0;
                sprintf( g_acChildTaskName, "tCli%d", g_usSeqNo++);
                stArg.fnProcess = psCmdKeyParaLnkTbl->func_info->function_point;
                stArg.pstEnv = pstEnv;
                stArg.psPara = NULL;
                pstTaskID = vosThreadCreate (
                    g_acChildTaskName,
                    CLI_CHILD_TASK_STACK_SIZE,
                    CLI_CHILD_TASK_PRIORITY,
                    (FUNCPTR)cliChildTask,
                    (void *)(&stArg));
                if (NULL == pstTaskID)
                {
                    return ERROR;
                }
                pstEnv->pstCommandTaskId = pstTaskID;
                while (pstEnv->pstCommandTaskId)
                {
                    if ((TERMINAL_IO_FAULT == pstEnv->err) || (pstEnv->close)
                        || (bKill == TRUE))
                    {
                        if (vosThreadVerify(pstEnv->pstCommandTaskId) == OK)
                        {
                           /*   Kill Task  */
                            pstEnv->closeTask = 1;
                            vosThreadDestroy(pstEnv->pstCommandTaskId);
                            pstEnv->pstCommandTaskId = NULL;
                            vosUSleep(200000);
                            break;
                        }
                    }
                    if (vosThreadVerify(pstEnv->pstCommandTaskId) == ERROR)
                    {
                        pstEnv->pstCommandTaskId = NULL;
                        break;
                    }
                    if (pstEnv->startESC == TRUE)
                    {

                        chEsc = cliCharGet(pstEnv);
                        if (chEsc == g_cTerminalChar)
                        {
                           bKill = TRUE;
                        }
                    }
                    vosUSleep(10000);
                }
                return OK;
            }
            else
            {
                nFlag = (*(psCmdKeyParaLnkTbl->func_info->function_point))(pstEnv,NULL);
            }
        }
        return nFlag;
    }

    /*assemble parameters*/
    psParaTbl = (PARA_TABLE_t *)vosAlloc(nParaNum*sizeof(PARA_TABLE_t));
    pnVisited = (int *)vosAlloc(nParaNum*sizeof(int));
    for (i = 0; i < nParaNum; i++)
    {
        pnVisited[i] = 0;
        psParaTbl[i].i = 0;
    }
    if (psParaCollection)
    {
        psParaCollection = psParaCollection->next;
        while (psParaCollection)
        {
            switch (psParaCollection->type)
            {
            case CLI_INTEGER:
                psParaTbl[psParaCollection->pos-1].i = psParaCollection->p->i;
                break;
            case CLI_UINT:
            case CLI_IPINTADDR:
            case CLI_IPINTMASK:
            case CLI_IPDOTADDR:
            case CLI_IPDOTMASK:
                psParaTbl[psParaCollection->pos-1].u = psParaCollection->p->u;
                break;
            case CLI_WORD:
            case CLI_STRING:
                     case CLI_MACADDR:
                     case CLI_DATE:
                     case CLI_TIME:
                psParaTbl[psParaCollection->pos-1].p = psParaCollection->p->p;
                break;
            default:
                break;
            }
            pnVisited[psParaCollection->pos-1] = 1;
            psParaCollection = psParaCollection->next;
        }
    }

    psParaLink = psCmdKeyParaLnkTbl->func_info->parameter;
    for (i = 0; i < nParaNum; i++)
    {
        if (0 == pnVisited[i])
        {
            switch(psParaLink->p->type)
            {
            case CLI_WORD:
            case CLI_STRING:
                  case CLI_MACADDR:
                  case CLI_DATE:
                  case CLI_TIME:
                psParaTbl[i].p = NULL;
                break;
            case CLI_INTEGER:
                psParaTbl[i].i = psParaLink->p->fail;
                break;
            default:
                psParaTbl[i].u = psParaLink->p->fail;
                break;
            }
        }
        psParaLink = psParaLink->next;
    }
    if (*(psCmdKeyParaLnkTbl->func_info->function_point) != NULL)
    {
        if (psCmdKeyParaLnkTbl->func_info->is_task)
        {
             /******************************************/
             /* Add your own codes here                */
             /******************************************/
            pstEnv->closeTask = 0;
            sprintf( g_acChildTaskName, "tCli%d", g_usSeqNo++);
            stArg.fnProcess = psCmdKeyParaLnkTbl->func_info->function_point;
            stArg.pstEnv = pstEnv;
            stArg.psPara = psParaTbl;
            pstTaskID = vosThreadCreate (
                g_acChildTaskName,
                CLI_CHILD_TASK_STACK_SIZE,
                CLI_CHILD_TASK_PRIORITY+5,
                (FUNCPTR)cliChildTask,
                (void *)(&stArg));
            if (NULL == pstTaskID)
            {
                return ERROR;
            }
            pstEnv->pstCommandTaskId = pstTaskID;
            while (pstEnv->pstCommandTaskId)
            {
                if ((TERMINAL_IO_FAULT == pstEnv->err) || (pstEnv->close)
                    || (bKill == TRUE))
                {
                    if (vosThreadVerify(pstEnv->pstCommandTaskId) == OK)
                    {
                       /*   Kill Task  */
                        pstEnv->closeTask = 1;
                        vosThreadDestroy(pstEnv->pstCommandTaskId);
                        pstEnv->pstCommandTaskId = NULL;
                        vosUSleep(200000);
                        break;
                    }
                }
                if (vosThreadVerify(pstEnv->pstCommandTaskId) == ERROR)
                {
                    pstEnv->pstCommandTaskId = NULL;
                    break;
                }
                if (pstEnv->startESC == TRUE)
                {

                    chEsc = cliCharGet(pstEnv);
                    if (chEsc == g_cTerminalChar)
                    {
                       bKill = TRUE;
                    }
                }
                vosUSleep(10000);
            }
            nFlag = OK;
        }
        else
        {
            nFlag = (*psCmdKeyParaLnkTbl->func_info->function_point)(pstEnv,psParaTbl);
        }
    }

    /*Free pnVisited and psParaTbl 's memory here,others will free by other model*/
    vosFree(psParaTbl);
    psParaTbl = NULL ;
    vosFree(pnVisited);
    pnVisited = NULL ;
    return nFlag;
}

/*****************************************************************************
 *cliTabComplete - Complete 'TAB' key.
 *DESCRIPTION
 *
 *Input:pstEnv,pchCmd.
 *Output:N/A.
 *Return: int.
 ****************************************************************************/
STATUS cliTabComplete(ENV_t *pstEnv, char *pchCmd)
{
    CMD_POOL_t *psCmdPool = NULL;

    if (NULL == pstEnv || NULL == pchCmd)
        return ERROR;

    psCmdPool = pstEnv->command;
    if (TAB == psCmdPool->achCmdPool[psCmdPool->nInputTailPos])
    {
        if (psCmdPool->nInputTailPos > 0)
        {
            psCmdPool->nInputTailPos--;
        }
        else
        {
            psCmdPool->bIsEmpty = TRUE;
            psCmdPool->bIsFull = FALSE;
        }
        psCmdPool->nDispEnd--;
        psCmdPool->nInputPos--;
        psCmdPool->nCursorPos--;
        psCmdPool->nInputLen--;
        psCmdPool->nDispLen--;
    }
    while (FALSE == psCmdPool->bIsEmpty
           && ' ' != psCmdPool->achCmdPool[psCmdPool->nInputTailPos])
    {
        psCmdPool->nCursorPos--;
        if (psCmdPool->nDispEnd > 0)
            psCmdPool->nDispEnd--;

        psCmdPool->nInputPos--;

        if (psCmdPool->nInputTailPos > 0)
        {
            psCmdPool->nInputTailPos--;
        }
        else
        {
            psCmdPool->bIsEmpty = TRUE;
            psCmdPool->bIsFull = FALSE;
        }
        psCmdPool->nInputLen--;
        psCmdPool->nDispLen--;
        vosPrintf(pstEnv->nWriteFd, "%c", BSP);
    }
    while (*pchCmd)
    {
        vosPrintf(pstEnv->nWriteFd, "%c", *pchCmd);
        if (TRUE == psCmdPool->bIsEmpty)
        {
            psCmdPool->bIsEmpty = FALSE;
            psCmdPool->achCmdPool[0] = *pchCmd++;
            psCmdPool->nCursorPos = 1;
            psCmdPool->nInputTailPos = 0;
            psCmdPool->nDispEnd = 0;
            psCmdPool->nInputPos = 1;
            psCmdPool->nInputLen = 1;
            psCmdPool->nDispLen = 1;
        }
        else
        {
            psCmdPool->achCmdPool[psCmdPool->nInputPos] = *pchCmd++;
            psCmdPool->nCursorPos++;
            psCmdPool->nInputTailPos++;
            psCmdPool->nDispEnd++;
            psCmdPool->nInputPos++;
            psCmdPool->nInputLen++;
            psCmdPool->nDispLen++;
        }
    }
    vosPrintf(pstEnv->nWriteFd, " ");
    psCmdPool->achCmdPool[psCmdPool->nInputPos] = ' ';
    psCmdPool->nCursorPos++;
    psCmdPool->nDispEnd++;
    psCmdPool->nInputPos++;
    psCmdPool->nInputTailPos++;
    psCmdPool->achCmdPool[psCmdPool->nInputPos] = '\0';
    psCmdPool->nInputLen++;
    psCmdPool->nDispLen++;

    if (LINEWIDTH-psCmdPool->nCursorPos-cliPromptLenGet(pstEnv) > 3)
        return OK;

    while (LINEWIDTH-psCmdPool->nCursorPos-cliPromptLenGet(pstEnv) <= 3)
    {
        psCmdPool->nDispBegin += INDENTWIDTH;
        psCmdPool->nCursorPos -= INDENTWIDTH;
        psCmdPool->nDispLen -= INDENTWIDTH;
    }
    return OK;
}


/*****************************************************************************
 *cliDescriptionProcess - Pre Handle the output description of KEY.
 *DESCRIPTION
 *
 *Input:pstEnv,ucKeyMaxLen,pResult,pDesr.
 *Output:pResult.
 *Return: N/A.
 ****************************************************************************/
void cliDescriptionProcess(ENV_t *pstEnv, int ucKeyMaxLen, char *pResult, char *pDesr)
{
    int i;
    int nPreLen = 0;
    int nDesrLen = 0;
    int nDisplayLen = 0;
    char *pchDesrTemp;

    if (NULL == pstEnv || NULL == pDesr || NULL == pResult)
        return;

    nPreLen = 5+ucKeyMaxLen;
    nDesrLen = strlen(pDesr);
    pchDesrTemp = pDesr;
    if ((pstEnv->ulTermWidth - nPreLen) > 0)
    {
        nDisplayLen = pstEnv->ulTermWidth - nPreLen;
    }
    else
    {
        nDisplayLen = 80 - nPreLen;
    }

    while (nDesrLen > nDisplayLen)
    {
        while (' ' == *pchDesrTemp)
            pchDesrTemp++;

        memcpy(pResult,pchDesrTemp,nDisplayLen);
        pResult += nDisplayLen;
        pchDesrTemp += nDisplayLen;
        *pResult++ = '\r';
        *pResult++ = '\n';

        for (i = 0; i < nPreLen; i++)
            *pResult++ = 0x20;

        nDesrLen -= nDisplayLen;
    }

    while (' ' == *pchDesrTemp)
        pchDesrTemp++;

    memcpy(pResult,pchDesrTemp,nDesrLen);
    pResult += nDesrLen;
    *pResult = '\0';
}

/*****************************************************************************
 *cliHelpProcess - Process the help info and get the length of the help string.
 *DESCRIPTION
 *
 *Input:psNodeCollectionSrc,pnStrMaxLen.
 *Output:pnStrMaxLen.
 *Return: STATUS.
 ****************************************************************************/
STATUS cliHelpProcess(NODE_COLLECTION_t *psNodeCollectionSrc, int *pnStrMaxLen)
{
    int i = 0;
    int j = 0;
    char achTempBuffer[64];
    int nNodeTotalNums = 0;
    int nParaLen = 0;
    int nFlag = 0;
    int nCompResult = 0;
    char achDescription[2*MAX_POOL_LENGTH+1];
    NODE_COLLECTION_t *psNodeCollection = NULL;
    NODE_COLLECTION_t *psNodeCollectionTmp = NULL;
    NODE_COLLECTION_t *psNodeCollectionKeep = NULL;
    CMD_KEY_PARA_LINK_t *psCmdNode = NULL;


    if (NULL == psNodeCollectionSrc)
    {
        return ERROR;
    }
    /*del the node whose type is NT_VIRTUAL_COMMAND */
    nNodeTotalNums = psNodeCollectionSrc->total;
    psNodeCollection = psNodeCollectionSrc->next;
    while (psNodeCollection && psNodeCollection->next)
    {
        if (NT_VIRTUAL_COMMAND == psNodeCollection->p->type)
        {
            nFlag = 1;
            psNodeCollectionKeep = psNodeCollection;
        }
        psNodeCollectionTmp = psNodeCollection->next;
        if (NT_VIRTUAL_COMMAND == psNodeCollectionTmp->p->type && 1 == nFlag)
        {
            psNodeCollection->next = psNodeCollection->next->next;
            psNodeCollectionSrc->total--;
            vosFree(psNodeCollectionTmp);
            psNodeCollectionTmp = NULL ;
        }
        else
        {
            psNodeCollection = psNodeCollection->next;
        }
    }
    if (psNodeCollection->next)
    {
        psNodeCollection = psNodeCollection->next;
    }
    if (psNodeCollectionKeep)
    {
        psCmdNode = psNodeCollectionKeep->p;
        psNodeCollectionKeep->p = psNodeCollection->p;
        psNodeCollection->p = psCmdNode;
    }
    /* The following is typically a bubble sort algorithm*/
    psNodeCollection = psNodeCollectionSrc->next;
    psNodeCollectionKeep = psNodeCollection;
    if (psNodeCollection &&
        psNodeCollection->p &&
        psNodeCollection->p->type != NT_VIRTUAL_COMMAND)
    {
        if (NT_PARAMETER == psNodeCollection->p->type &&
            psNodeCollection->p->parameter)
        {
            switch(psNodeCollection->p->parameter->type)
            {
            case CLI_UINT:
                nParaLen = 3;
                nParaLen += vosSnprintf(achTempBuffer, 64, "%lu", (UINT32)psNodeCollection->p->parameter->min);
                nParaLen += vosSnprintf(achTempBuffer, 64, "%lu", (UINT32)psNodeCollection->p->parameter->max);
                *pnStrMaxLen = nParaLen;
                break;
            case CLI_INTEGER:
                j = 1;
                nParaLen = 3;
                while ((psNodeCollection->p->parameter->min/j) > 0)
                {
                    nParaLen++;
                    j*=10;
                }
                if (0 == psNodeCollection->p->parameter->min)
                {
                    nParaLen++;
                }
                j = 1;
                while ((psNodeCollection->p->parameter->max/j) > 0)
                {
                    nParaLen++;
                    j*=10;
                }
                if (0 == psNodeCollection->p->parameter->max)
                {
                    nParaLen++;
                }
                *pnStrMaxLen = nParaLen;
                break;
            case CLI_WORD:
            case CLI_STRING:
                *pnStrMaxLen = 5;  /*INPUT*/
                break;

            case CLI_DATE:
                *pnStrMaxLen = 10;  /*yyyy-mm-dd*/
                break;

            case CLI_TIME:
                *pnStrMaxLen = 8;  /*hh:mm:ss*/
                break;

            case CLI_IPDOTADDR:
            case CLI_IPDOTMASK:
                *pnStrMaxLen = 7; /*A.B.C.D*/
                break;

            case CLI_MACADDR:
                *pnStrMaxLen = 17;
                break;

            case CLI_IPINTADDR:
            case CLI_IPINTMASK:
                *pnStrMaxLen = 14;
                break;
            default:
                break;
            }
        }
        else
        {
            *pnStrMaxLen = strlen(psNodeCollection->p->key);
        }
    }

    while (psNodeCollection && psNodeCollection->next)
    {
        psNodeCollectionTmp = psNodeCollection->next;
        while (psNodeCollectionTmp &&
            psNodeCollectionTmp->p &&
            psNodeCollectionTmp->p->type != NT_VIRTUAL_COMMAND)
        {
            if (NT_PARAMETER == psNodeCollectionTmp->p->type &&
                psNodeCollectionTmp->p->parameter)
            {
                switch(psNodeCollectionTmp->p->parameter->type)
                {
                case CLI_UINT:
                    nParaLen = 3;
                    nParaLen += vosSnprintf(achTempBuffer, 64, "%lu", (UINT32)psNodeCollectionTmp->p->parameter->min);
                    nParaLen += vosSnprintf(achTempBuffer, 64, "%lu", (UINT32)psNodeCollectionTmp->p->parameter->max);
                    break;
                case CLI_INTEGER:
                    j = 1;
                    nParaLen = 3;
                    while ((psNodeCollectionTmp->p->parameter->min/j) > 0)
                    {
                        nParaLen++;
                        j*=10;
                    }
                    if (0 == psNodeCollectionTmp->p->parameter->min)
                    {
                        nParaLen++;
                    }
                    j = 1;
                    while ((psNodeCollectionTmp->p->parameter->max/j) > 0)
                    {
                        nParaLen++;
                        j*=10;
                    }
                    if (0 == psNodeCollectionTmp->p->parameter->max)
                    {
                        nParaLen++;
                    }
                    break;
                case CLI_WORD:
                case CLI_STRING:
                    nParaLen = 5;
                    break;
                case CLI_DATE:
                    nParaLen = 10;
                    break;
                case CLI_TIME:
                    nParaLen = 8;
                    break;

                case CLI_IPDOTADDR:
                case CLI_IPDOTMASK:
                    nParaLen = 7;
                    break;

                   case CLI_MACADDR:
                       *pnStrMaxLen = 17;
                       break;

                case CLI_IPINTADDR:
                case CLI_IPINTMASK:
                    nParaLen = 14;
                    break;
                default:
                    nParaLen = 7;
                    break;
                }
                if (nParaLen > *pnStrMaxLen)
                {
                    *pnStrMaxLen = nParaLen;
                }
            }
            else
            {
                if (psNodeCollectionTmp->p->key &&
                    (int)strlen(psNodeCollectionTmp->p->key) > *pnStrMaxLen)
                {
                    *pnStrMaxLen = strlen(psNodeCollectionTmp->p->key);
                }
            }
            if (psNodeCollectionTmp->p->key &&
                psNodeCollection->p &&
                psNodeCollection->p->key)
            {
                nCompResult = strcmp(psNodeCollection->p->key,
                                 psNodeCollectionTmp->p->key);
            }
            /* if there is the same item , then we'll delete one of two! */
            if (0 == nCompResult)
            {
                psNodeCollectionKeep->next = psNodeCollectionTmp->next;
                psNodeCollectionTmp->next = NULL;
                cliCollectionFree(NULL, (NODE_COLLECTION_t *)psNodeCollectionTmp, NULL);
                psNodeCollectionTmp = psNodeCollectionKeep->next;
                psNodeCollectionSrc->total--;
                continue;
            }
            else
            {
                if (nCompResult > 0)
                {
                    psCmdNode = psNodeCollection->p;
                    psNodeCollection->p = psNodeCollectionTmp->p;
                    psNodeCollectionTmp->p = psCmdNode;
                }
            }
            psNodeCollectionKeep = psNodeCollectionTmp;
            psNodeCollectionTmp = psNodeCollectionTmp->next;
        }
        psNodeCollection = psNodeCollection->next;
        psNodeCollectionKeep = psNodeCollection;
    }

    return OK;
}

STATUS cliHelpShow(ENV_t *pstEnv, NODE_COLLECTION_t *psNodeCollectionSrc)
{
    int i = 0;
    int k = 0;
    int nStrMaxLen = 0;
    char achDescription[2*MAX_POOL_LENGTH+1] = {0};
    NODE_COLLECTION_t *psNodeCollection = NULL;
    char temStr[30];

    if (NULL == psNodeCollectionSrc)
        return OK;

    if (ERROR == cliHelpProcess(psNodeCollectionSrc,&nStrMaxLen))
        return ERROR;

    if (pstEnv->command->nInputPos==1)
    {
        switch (pstEnv->pmode)
        {
        case CLI_MODE_USER:
            vosPrintf(pstEnv->nWriteFd, "User EXEC commands:\r\n");
            break;
        case CLI_MODE_ENABLE:
            vosPrintf(pstEnv->nWriteFd, "Enabled EXEC commands:\r\n");
            break;
        case CLI_MODE_CONFIG_SYS:
            vosPrintf(pstEnv->nWriteFd, "System configuration commands:\r\n");
            break;
        case CLI_MODE_CONFIG_USER:
            vosPrintf(pstEnv->nWriteFd, "User configuration commands:\r\n");
            break;
        case CLI_MODE_CONFIG_QOS:
            vosPrintf(pstEnv->nWriteFd, "QoS configuration commands:\r\n");
            break;
        case CLI_MODE_CONFIG_FDB:
            vosPrintf(pstEnv->nWriteFd, "FDB configuration commands:\r\n");
            break;
        case CLI_MODE_CONFIG_MCAST:
            vosPrintf(pstEnv->nWriteFd, "Multicast configuration commands:\r\n");
            break;
        case CLI_MODE_CONFIG_VLAN:
            vosPrintf(pstEnv->nWriteFd, "VLAN configuration commands:\r\n");
            break;
        case CLI_MODE_CONFIG_PORT:
            vosPrintf(pstEnv->nWriteFd, "Port configuration commands:\r\n");
            break;
        case CLI_MODE_CONFIG_RSTP:
            vosPrintf(pstEnv->nWriteFd, "RSTP configuration commands:\r\n");
            break;
        case CLI_MODE_CONFIG_PON:
            vosPrintf(pstEnv->nWriteFd, "PON configuration commands:\r\n");
            break;
        case CLI_MODE_CONFIG_STORM:
            vosPrintf(pstEnv->nWriteFd, "Storm configuration commands:\r\n");
            break;
        case CLI_MODE_CONFIG_QOS_CLASS:
            vosPrintf(pstEnv->nWriteFd, "QoS class configuration commands:\r\n");
            break;
	 case CLI_MODE_CONFIG_MIRROR:
            vosPrintf(pstEnv->nWriteFd, "Mirror configuration commands:\r\n");
            break;
        default:
            break;
        }
    }

    nStrMaxLen += 2;
    nStrMaxLen = (nStrMaxLen>22)?nStrMaxLen:22;

    psNodeCollection = psNodeCollectionSrc;
    while (psNodeCollectionSrc->total > 0)
    {
        psNodeCollectionSrc->total--;
        psNodeCollection = psNodeCollection->next;
        if (NULL == psNodeCollection)
            break;

        if (NULL == psNodeCollection->p)
            continue;

        cliDescriptionProcess(pstEnv, nStrMaxLen, achDescription,
            psNodeCollection->p->description);

        if (NULL != psNodeCollection->p->key && !strcmp(psNodeCollection->p->key, "exit"))
        {
            switch (pstEnv->pmode)
            {
            case CLI_MODE_USER:
                sprintf(achDescription, "Exit from the EXEC");
                break;
            case CLI_MODE_ENABLE:
                sprintf(achDescription, "Exit from enabled mode");
                break;
            case CLI_MODE_CONFIG_SYS:
                sprintf(achDescription, "Exit from system configuration mode");
                break;
            case CLI_MODE_CONFIG_USER:
                sprintf(achDescription, "Exit from user configuration mode");
                break;
            case CLI_MODE_CONFIG_QOS:
                sprintf(achDescription, "Exit from QoS configuration mode");
                break;
            case CLI_MODE_CONFIG_FDB:
                sprintf(achDescription, "Exit from FDB configuration mode");
                break;
            case CLI_MODE_CONFIG_MCAST:
                sprintf(achDescription, "Exit from multicast configuration mode");
                break;
            case CLI_MODE_CONFIG_VLAN:
                sprintf(achDescription, "Exit from VLAN configuration mode");
                break;
            case CLI_MODE_CONFIG_PORT:
                sprintf(achDescription, "Exit from port configuration mode");
                break;
            case CLI_MODE_CONFIG_RSTP:
                sprintf(achDescription, "Exit from RSTP configuration mode");
                break;
            case CLI_MODE_CONFIG_PON:
                sprintf(achDescription, "Exit from PON configuration mode");
                break;
            case CLI_MODE_CONFIG_STORM:
                sprintf(achDescription, "Exit from storm configuration mode");
                break;
            case CLI_MODE_CONFIG_QOS_CLASS:
                sprintf(achDescription, "Exit from QoS class configuration mode");
                break;
	     case CLI_MODE_CONFIG_MIRROR:
                sprintf(achDescription, "Exit from Mirror configuration mode");
                break;
            default:
                break;
            }
        }

        switch(psNodeCollection->p->type)
        {
        case NT_KEY:
            sprintf( temStr, "  %s", psNodeCollection->p->key);
            k = strlen(temStr);
            vosPrintf(pstEnv->nWriteFd, "%s", temStr);
            for (i = 0; i < nStrMaxLen-k; i++)
                vosPrintf(pstEnv->nWriteFd, " ");

            vosPrintf(pstEnv->nWriteFd, "%s\r\n",achDescription);
            break;

        case NT_PARAMETER:
            switch(psNodeCollection->p->parameter->type)
            {
            case CLI_WORD:
            case CLI_STRING:
                sprintf( temStr, "  %s","INPUT");
                break;
            case CLI_DATE:
                sprintf( temStr, "  %s","yyyy-mm-dd");
                break;
            case CLI_TIME:
                sprintf( temStr, "  %s","hh:mm:ss");
                break;
            case CLI_INTEGER:
                sprintf( temStr, "  <%ld-%ld>", psNodeCollection->p->parameter->min, psNodeCollection->p->parameter->max);
                break;
            case CLI_UINT:
                sprintf( temStr, "  <%lu-%lu>", (UINT32)psNodeCollection->p->parameter->min, (UINT32)psNodeCollection->p->parameter->max);
                break;
            case CLI_IPDOTADDR:
            case CLI_IPDOTMASK:
                sprintf( temStr, "  %s","A.B.C.D");
                break;
             case CLI_MACADDR:
          sprintf( temStr, "  %s","XX:XX:XX:XX:XX:XX");
          break;
            case CLI_IPINTADDR:
            case CLI_IPINTMASK:
                sprintf( temStr, "  %s","<0-4294967295>");
                break;
            default:
                return ERROR;
            }
            k=strlen(temStr);
            vosPrintf(pstEnv->nWriteFd, "%s", temStr);
            for (i = 0; i < nStrMaxLen-k; i++)
                vosPrintf(pstEnv->nWriteFd, " ");

            vosPrintf(pstEnv->nWriteFd, "%s\r\n", achDescription);
            break;

        case NT_VIRTUAL_COMMAND:
            vosPrintf(pstEnv->nWriteFd, "  <CR>\r\n");
            break;

        default:
            break;
        }
    }

    return OK;
}

/*****************************************************************************
 *cliResourceFree - Free telnet resource.
 *DESCRIPTION
 *
 *Input:pstEnv.
 *Output:N/A.
 *Return: STATUS.
 ****************************************************************************/
STATUS cliResourceFree(ENV_t *pstEnv)
{
    int i;

    if (pstEnv->command)
    {
        vosFree(pstEnv->command);
        pstEnv->command = NULL;
    }
    /* comment out debug info.
    else
        vosPrintf(pstEnv->nWriteFd, "command pool error!\r\n");*/

    if (pstEnv->configureHistoryCommand)
    {
        if (1 == pstEnv->configureHistoryCommand->full)
        {
            for (i = 0; i<MAX_HISTORY_CMD_NUMBER; i++)
            {
                if (NULL != pstEnv->configureHistoryCommand->cmd[i])
                {
                    vosFree(pstEnv->configureHistoryCommand->cmd[i]);
                    pstEnv->configureHistoryCommand->cmd[i] = NULL;
                }
            }
        }
        else
        {
            for ( i=0; i<= pstEnv->configureHistoryCommand->tail; i++)
            {
                if (NULL != pstEnv->configureHistoryCommand->cmd[i])
                {
                    vosFree(pstEnv->configureHistoryCommand->cmd[i]);
                    pstEnv->configureHistoryCommand->cmd[i] = NULL;
                }
            }
        }

        if (pstEnv->configureHistoryCommand)
        {
            vosFree(pstEnv->configureHistoryCommand);
            pstEnv->configureHistoryCommand = NULL;
        }
    }

    cliShowFree(pstEnv);    /*if unnormal exit ,then free some resource. */

    pstEnv->free = 1;
    return OK;
}


/*****************************************************************************
 *cliLineRead - Get one line which is inputed from cli session.
 *DESCRIPTION
 *This routine to get one line which is inputed from cli session.
 *Input:pstEnv,pchBuf,pusIsCtrlC.
 *Output:buffer,pusIsCtrlC.
 *Return:unsigned long.
 ****************************************************************************/
ULONG cliLineRead(ENV_t *pstEnv, char *pchBuf,unsigned short *pusIsCtrlC)
{
    int nLineLen = 0;
    unsigned short  usChar;
    unsigned short  usTemp;


    if (NULL == pstEnv || NULL == pchBuf)
    {
        return (ULONG)ERROR;
    }
    *pusIsCtrlC = FALSE;
    cliCmdPoolClear(pstEnv->command);
    do
    {
        if (TERMINAL_IO_FAULT == pstEnv->err)
        {
            return (ULONG)ERROR;
        }
        usChar = cliCharGet(pstEnv);
        switch(usChar)
        {
        case 0xffff: /* invalid character. Do nothing*/
            break;
        /* <Backspace>. If the command pool isn't empty,
           remove the last character input from pool and screen
         */
        case BSP:
        case RUBOUT:
            if (!cliCmdPoolIsEmpty((CMD_POOL_t *)(pstEnv->command)))
            {
                cliCmdPop((CMD_POOL_t *)(pstEnv->command));
                cliEcho(pstEnv, usChar);
            }
            break;
        case NL:   /* <CR>. Insert  ' ' at the end of the pool. */
            cliNewLineOutput(pstEnv);
            break;
        case CR:
            usTemp = cliCharGet(pstEnv);
            if (usTemp == NL || usTemp == 0xffff)
            {
                cliNewLineOutput(pstEnv);
            }
            break;
        case TAB:
        case '?':
        case CURSOR_UP:
        case CURSOR_DOWN:
        case CURSOR_LEFT:
        case CURSOR_RIGHT:
        case CTRLP:
        case CTRLN:
            break;
        case CTRLC:
            *pusIsCtrlC = TRUE;
            cliNewLineOutput(pstEnv);
            cliCmdPoolClear (pstEnv->command);
            return 0;
        default:   /* normal key(A~Z, a~z,0~9). Insert it at the end of pool */
            if(cliCmdPush(usChar, pstEnv->command) == OK)
            cliEcho(pstEnv, usChar);
            break;
        }
    }while(!(NL == usChar || CR == usChar));

    nLineLen = strlen(pstEnv->command->achCmdPool);

    if (!strcpy(pchBuf,pstEnv->command->achCmdPool))
    {
        return (ULONG)ERROR;
    }
    return (ULONG)nLineLen;
}


/*****************************************************************************
 *cliCmdHistoryProcess - To handle history command, according to type, return
                or save history command.
 *DESCRIPTION
 *This routine mainly to handle doskey function.
 *Input:pe,type,PC.
       uchProcType:  required function type
             0(SAVE)    : save a command to history command pool
             1(GETPRE)  : get pre history command in pool
             2(GETNEXT) : get next history command in pool
       psCmdPool  :  struct for carry in or carry out command
 *Output:psCmdPool.
       psCmdPool  :
             type=SAVE   : not changed
             type=GETPRE : filled with a command from history command pool,
                           if pool is empty, "\0"
             type=GETNEXT: filled with a command from history command pool,
                           if reach last, "\0"
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS cliCmdHistoryProcess(ENV_t *pstEnv,unsigned char uchProcType,CMD_POOL_t *psCmdPool)
{
    int    i = 0;
    char  *pchCmd = NULL;
    CMD_HISTORY_t   *psCmdHistoryTmp = NULL;

    if (NULL == pstEnv || NULL == pstEnv->configureHistoryCommand)
        return ERROR;

    /*Get the pointer of history pool that We wil process*/
    psCmdHistoryTmp = pstEnv->configureHistoryCommand;
    /*Process the function that user asked for*/
    switch(uchProcType)
    {
    case SAVE:
        if (!(0 == psCmdHistoryTmp->tail && 0 == psCmdHistoryTmp->head
            && 0 == psCmdHistoryTmp->pos))   /* is buffer Empty ? */
        {
            if ((strcmp(psCmdHistoryTmp->cmd[psCmdHistoryTmp->tail],
                 pstEnv->command->achCmdPool)) == 0) /* is command exist? */
            {
                psCmdHistoryTmp->pos = (psCmdHistoryTmp->tail+1)
                                       % MAX_HISTORY_CMD_NUMBER;
                return OK;
            }
        }
        /* is buffer Full ? */
        if (((psCmdHistoryTmp->tail+1) % MAX_HISTORY_CMD_NUMBER)
              == psCmdHistoryTmp->head)
        {
            psCmdHistoryTmp->full = TRUE;
        }
        if (!(0 == psCmdHistoryTmp->tail && 0 == psCmdHistoryTmp->head
            && 0 == psCmdHistoryTmp->pos))
        {
            psCmdHistoryTmp->tail = (psCmdHistoryTmp->tail+1)
                                    % MAX_HISTORY_CMD_NUMBER;
        }
        if (TRUE == psCmdHistoryTmp->full)
        {
            vosFree(psCmdHistoryTmp->cmd[psCmdHistoryTmp->tail]);
            psCmdHistoryTmp->cmd[psCmdHistoryTmp->tail] = NULL ;
            psCmdHistoryTmp->head = (psCmdHistoryTmp->head+1)
                                    % MAX_HISTORY_CMD_NUMBER;
        }
        psCmdHistoryTmp->pos = (psCmdHistoryTmp->tail+1)
                               % MAX_HISTORY_CMD_NUMBER;
        psCmdHistoryTmp->flag = FALSE;
        /* Reserve '\0' memory */
        pchCmd = (char *)vosAlloc(pstEnv->command->nInputLen + 1);
        if (NULL == pchCmd)
        {
            return ERROR;
        }
        if ('?' == pstEnv->command->achCmdPool[pstEnv->command->nInputTailPos] ||
            TAB == pstEnv->command->achCmdPool[pstEnv->command->nInputTailPos])
        {
            strncpy(pchCmd,pstEnv->command->achCmdPool,pstEnv->command->nInputLen-1);
            pchCmd[pstEnv->command->nInputLen-1] = '\0';
        }
        else {
            strcpy(pchCmd,pstEnv->command->achCmdPool);
        }
        psCmdHistoryTmp->cmd[psCmdHistoryTmp->tail] = pchCmd;
        break;
    case GETNEXT:
        psCmdHistoryTmp->flag = 0;
        if (0 == psCmdHistoryTmp->tail && 0 == psCmdHistoryTmp->head
            && 0 == psCmdHistoryTmp->pos)
        {
            cliCmdPoolRefresh(pstEnv,"\0");
            return OK;
        }
        if (psCmdHistoryTmp->pos == psCmdHistoryTmp->tail)
        {
            cliCmdPoolRefresh (pstEnv,"\0");
            psCmdHistoryTmp->flag = 1;
            return OK;
        }
        /* add :to handle one cmd error!!16:37 */
        if (TRUE == psCmdHistoryTmp->full)
        {
            psCmdHistoryTmp->pos = (psCmdHistoryTmp->pos+1)
                                   % MAX_HISTORY_CMD_NUMBER;
        }
        else
        {
            if (psCmdHistoryTmp->pos != psCmdHistoryTmp->tail+1)
            {
                psCmdHistoryTmp->pos = (psCmdHistoryTmp->pos+1)
                                       % MAX_HISTORY_CMD_NUMBER;
            }
            else
            {
                cliCmdPoolRefresh(pstEnv,"\0");
                return OK;
            }
        }
        cliCmdPoolRefresh (pstEnv,psCmdHistoryTmp->cmd[psCmdHistoryTmp->pos]);
        break;
    case GETPRE:
        if (0 == psCmdHistoryTmp->tail && 0 == psCmdHistoryTmp->head
            && 0 == psCmdHistoryTmp->pos)
        {
            cliCmdPoolRefresh (pstEnv,"\0");
            return OK;
        }
        if (psCmdHistoryTmp->pos == psCmdHistoryTmp->head
            && 1 == psCmdHistoryTmp->flag)
        {
            pchCmd = psCmdHistoryTmp->cmd[psCmdHistoryTmp->pos];
        }
        else
        {
            if (0 == psCmdHistoryTmp->flag
                && psCmdHistoryTmp->head
                   == ((psCmdHistoryTmp->pos-1+MAX_HISTORY_CMD_NUMBER)
                       % MAX_HISTORY_CMD_NUMBER))
            {
                pchCmd = psCmdHistoryTmp->cmd[psCmdHistoryTmp->head];
                if (!(0 == psCmdHistoryTmp->tail && 0 == psCmdHistoryTmp->head))
                {
                    psCmdHistoryTmp->pos = psCmdHistoryTmp->head;
                    psCmdHistoryTmp->flag = 1;
                }
            }
            else
            {
                if (1 == psCmdHistoryTmp->flag)
                {
                    psCmdHistoryTmp->pos = psCmdHistoryTmp->tail;
                    psCmdHistoryTmp->flag = 0;
                }
                else
                {
                    psCmdHistoryTmp->pos = (psCmdHistoryTmp->pos-1+MAX_HISTORY_CMD_NUMBER)%MAX_HISTORY_CMD_NUMBER;
                }

                pchCmd = psCmdHistoryTmp->cmd[psCmdHistoryTmp->pos];
            }
        }
        cliCmdPoolRefresh (pstEnv,pchCmd);
        break;
    default:     /*user required unknown functions*/
        break;
    }
    return OK;
}

/**********************************************************
* cliSessionTimeoutTask -
* Input:pstEnv.
* Output:N/A.
* RETURNS: N/A
 *********************************************************/
void cliSessionTimeoutTask(ENV_t *pstEnv)
{
    UINT32 start, current, timeout;

    start = vosTimeGet();
    while (pstEnv)
    {
        if (!pstEnv->used &&
            CLI_CMD_PHASE_SYS_SHELL != pstEnv->phase)
        {
            current = vosTimeGet();
            timeout = odmSysCfgSessionTimeoutGet();
            /*vosPrintf(pstEnv->nWriteFd, "session[0x%x] start=%u, current=%u, timeout=%u\r\n",
                pstEnv->pstMainTaskId,start,current,timeout);*/
            if ((0 != timeout) &&
                ((current - start) >= timeout))
            {
                vosPrintf(pstEnv->nWriteFd, "\r\nSession timeout\r\n");
                pstEnv->phase = CLI_CMD_PHASE_EXIT;
                /* here we only close telnet session */
                cliSessionClose(pstEnv);
                break;
            }
        }
        else
        {
            start = vosTimeGet();
            pstEnv->used = FALSE;
        }
        vosSleep(1);
    }

    vosThreadExit(0);
}

static void cliKeyParaLinkFree(CMD_KEY_PARA_LINK_t  **ppsKeyParaLink)
{
    CMD_KEY_PARA_LINK_t  *psCmdKeyParaTmp = NULL;

    if (ppsKeyParaLink != NULL && *ppsKeyParaLink != NULL)
    {
        psCmdKeyParaTmp = (*ppsKeyParaLink)->explain;
        if (psCmdKeyParaTmp != NULL )
        {
            while (psCmdKeyParaTmp->right)
                psCmdKeyParaTmp = psCmdKeyParaTmp->right;

            while (psCmdKeyParaTmp->left)
            {
                psCmdKeyParaTmp->explain = NULL;
                psCmdKeyParaTmp = psCmdKeyParaTmp->left ;
                vosFree(psCmdKeyParaTmp->right);
                psCmdKeyParaTmp->right = NULL;
            }
            psCmdKeyParaTmp->explain = NULL;
            vosFree(psCmdKeyParaTmp);
            psCmdKeyParaTmp = NULL;
        }
        (*ppsKeyParaLink)->explain = NULL;
        vosFree(*ppsKeyParaLink);
        *ppsKeyParaLink = NULL;
    }
}


/*****************************************************************************
 *cliCmdInterpreter -  These functions below are mainly for match command
                     and execute special function.
 *DESCRIPTION
 *This function is used to interprete the input comand.
 *
 *Input:pstEnv,psForestRoot.

 *Output:
 *Return:STATUS.
         OK     ---If no error.
         ERROR  ---If error.
 ****************************************************************************/
STATUS  cliCmdInterpreter(ENV_t *pstEnv, CMD_KEY_PARA_LINK_t *psForestRoot, CMD_VALIDITY_t *pCmdValidity)
{
    int   nLen = 0;
    int   nFlag = -1;
    int   nCompResult = -1;
    char  achWord[MAX_POOL_LENGTH+2];
    char  *pchWord = achWord;
    char  *pchCommand = NULL;
    unsigned int  unWordType = 0;
    BOOL  bIsNeed = FALSE;
    BOOL  bIsError = FALSE;
    BOOL  bIsParaMore = FALSE;
    BOOL  bIsKeyMatch = FALSE;
    BOOL  bIsParaMatch = FALSE;
    BOOL  bIsKeyMatchMore = FALSE;
    BOOL  bIsParaMatchMore = FALSE;
    BOOL  bIsKeyForceMatch = FALSE;
    BOOL  bIsKeyForceMatchMore = FALSE;
    NODE_COLLECTION_t   *psNode = NULL;
    NODE_COLLECTION_t   *psNodeNext = NULL;
    PARA_COLLECTION_t   *psParaNode = NULL;
    PARA_COLLECTION_t   *psNewParaNode = NULL;
    PARA_COLLECTION_t   *psParaNext = NULL;
    PARA_COLLECTION_t   *psParaLast = NULL;
    CMD_KEY_PARA_LINK_t  *psKeepNode = NULL ;
    CMD_KEY_PARA_LINK_t  *psRootTemp = NULL;
    CMD_KEY_PARA_LINK_t  *psCmdKeyParaTmp = NULL;
    CMD_KEY_PARA_LINK_t  *psNewCmdKeyParaNode = NULL;
    CMD_KEY_PARA_LINK_t  *psCmdKeyParaCurrent = NULL;
    NODE_COLLECTION_t       *psPreNode = NULL;
    NODE_COLLECTION_t       *psCurrentNode = NULL;

    if (pCmdValidity)
        *pCmdValidity = CMD_VALID;

    if (NULL == pstEnv || NULL == psForestRoot)
    {
        return ERROR;
    }

    pchCommand = pstEnv->command->achCmdPool;

    while (' ' == *pchCommand)
        pchCommand++;

    if ((strlen(pchCommand)) == 0)
    {
        return OK;
    }

    while (pstEnv->phase != CLI_CMD_PHASE_EXIT)
    {
        pstEnv->phase = CLI_CMD_PHASE_WHOLE;
        if (psNode)
        {
            cliCollectionFree(NULL,psNode,NULL);
            psNode = NULL;
        }
        memset(pchWord, 0, (MAX_POOL_LENGTH+2));
        nFlag = cliWordExtract(&pchCommand, pchWord, FORWARD);
        if (nFlag == ERROR)
        {
            nFlag = cliCmdMatch(pstEnv,psForestRoot,NULL,&psNode);
            cliKeyParaLinkFree(&psRootTemp);
            if (ERROR == nFlag)
            {
                cliCollectionFree(psParaNode, psNode, pstEnv->command);
                return ERROR;
            }

            if (0 == psNode->total)
            {
                cliCollectionFree(psParaNode, psNode, pstEnv->command);
                return ERROR;
            }

            psNodeNext = psNode->next;
            while (psNodeNext && 0 == nFlag)
            {
                if (NT_VIRTUAL_COMMAND == psNodeNext->p->type)
                    nFlag = 1;
                else
                    psNodeNext = psNodeNext->next;
            }

            if ((1 == nFlag) && ((psNodeNext->p->exec_mode & pstEnv->pmode)
                    ||((psNodeNext->p->exec_mode & CLI_SPACIFY_SUB_MODE)
                    &&(psNodeNext->p->exec_sub_mode & pstEnv->sub_pmode))))
            {
                cliCmdHistoryProcess(pstEnv, SAVE, pstEnv->command);
                nFlag = cliCmdExecute(pstEnv, psParaNode, psNodeNext);
                if (OK == nFlag)
                    nFlag = 1;
                /*bwei add it 10.05.12, print error info*/
                else
               	    vosPrintf(pstEnv->nWriteFd, "command execute unsuccessfully.\r\n");
		/*----------------------------------------*/
            }
            else
            {
                vosPrintf(pstEnv->nWriteFd, "%% Incomplete command.\r\n");
                if (pCmdValidity)
                    *pCmdValidity = CMD_INCOMPLETE;

                cliCmdHistoryProcess(pstEnv, SAVE, pstEnv->command);
            }

            cliCollectionFree(psParaNode, psNode, pstEnv->command);

            if (1 == nFlag)
            {
                return OK;
            }
            else
            {
                return ERROR;
            }
        }
        else
        {
            nFlag = cliWordTypeGet(pchWord, &unWordType);
            if (ERROR == nFlag)
            {
                cliKeyParaLinkFree(&psRootTemp);
                cliCollectionFree(psParaNode, psNode, pstEnv->command);
                return ERROR;
            }

            if (NT_VIRTUAL_COMMAND == psForestRoot->type
                    && unWordType != WORD_TAB && unWordType != WORD_QUESTION)
            {
                vosPrintf(pstEnv->nWriteFd, "%% Invalid input detected.\r\n");
				cliCmdHistoryProcess(pstEnv,SAVE,pstEnv->command);

                if (pCmdValidity)
                    *pCmdValidity = CMD_INVALID_INPUT;

                cliKeyParaLinkFree(&psRootTemp);
                cliCollectionFree(psParaNode,psNode,pstEnv->command);
                return ERROR;
            }

            switch (unWordType)
            {
            case WORD_TAB:
                pstEnv->phase = CLI_CMD_PHASE_SUPPLY;
                cliKeyParaLinkFree(&psRootTemp);
                return OK;

            case WORD_QUESTION:
                nFlag = cliCmdMatch(pstEnv,psForestRoot,NULL,&psNode);
                if (ERROR == nFlag)
                {
                    cliKeyParaLinkFree(&psRootTemp);
                    cliCollectionFree(psParaNode,psNode,NULL);
                    return OK;
                }
                pstEnv->phase = CLI_CMD_PHASE_HELP;

                cliShowStart(pstEnv);
                cliHelpShow(pstEnv, psNode);
                cliShowEnd(pstEnv);

                cliKeyParaLinkFree(&psRootTemp);
                cliCollectionFree(psParaNode,psNode,NULL);
                return OK;

            case WORD_END_TAB:
                pchWord[strlen(pchWord)-1] = '\0';
                nFlag = cliCmdMatch(pstEnv,psForestRoot,pchWord,&psNode);
                if (ERROR == nFlag)
                {
                    cliKeyParaLinkFree(&psRootTemp);
                    cliCollectionFree(psParaNode,psNode,NULL);
                    cliNewLineOutput(pstEnv);
                    vosPrintf(pstEnv->nWriteFd, "%% Unrecognized command.\r\n");
					cliCmdHistoryProcess(pstEnv, SAVE, pstEnv->command);

                    if (pCmdValidity)
                        *pCmdValidity = CMD_UNRECOGNIZED;

                   return OK;
                }

                psNodeNext = psNode->next;
                psPreNode = psNodeNext;

                while (psNodeNext && psNodeNext->next)
                {
                    psCurrentNode = psNodeNext->next;
                    while (psCurrentNode)
                    {
                        nCompResult = strcmp(psNodeNext->p->key,psCurrentNode->p->key);
                        if (0 == nCompResult)
                        {
                            psPreNode->next = psCurrentNode->next;
                            psNode->total--;
                            vosFree (psCurrentNode);
                            psCurrentNode = psPreNode->next;
                            continue;
                        }
                        psPreNode = psCurrentNode;
                        psCurrentNode = psCurrentNode->next;
                    }
                    psNodeNext = psNodeNext->next;
                }

                if (0 == nFlag && 1 == psNode->total && NT_KEY == psNode->next->p->type)
                {
                    cliTabComplete(pstEnv, psNode->next->p->key);
                    pstEnv->phase = CLI_CMD_PHASE_SUPPLY;
                    cliKeyParaLinkFree(&psRootTemp);
                    cliCollectionFree(psParaNode,psNode,NULL);
                }
                else if (0 == nFlag)
                {
                    cliNewLineOutput(pstEnv);
                    nLen = 0;

                    while (psNode->total>0)
                    {
                        nLen++;
                        psNode->total--;
                        psNodeNext = psNode->next;
                        psNode->next = psNode->next->next;

                        switch(psNodeNext->p->type)
                        {
                        case NT_KEY:
                            vosPrintf(pstEnv->nWriteFd, "  %s",psNodeNext->p->key);
                            break;
                        case NT_PARAMETER:
                            switch(psNodeNext->p->parameter->type)
                            {
                            case CLI_WORD:
                                vosPrintf(pstEnv->nWriteFd, "  %s","INPUT");
                                break;
                            case CLI_STRING:
                                vosPrintf(pstEnv->nWriteFd, "  %s","INPUT");
                                break;
                            case CLI_DATE:
                                vosPrintf(pstEnv->nWriteFd, "  %s","yyyy-mm-dd");
                                break;
                            case CLI_TIME:
                                vosPrintf(pstEnv->nWriteFd, "  %s","hh:mm:ss");
                                break;
                            case CLI_INTEGER:
                                vosPrintf(pstEnv->nWriteFd, "  <%ld-%ld>",psNodeNext->p->parameter->min,psNodeNext->p->parameter->max);
                                break;
                            case CLI_UINT:
                                vosPrintf(pstEnv->nWriteFd, "  <%lu-%lu>",(UINT32)psNodeNext->p->parameter->min,(UINT32)psNodeNext->p->parameter->max);
                                break;
                            case CLI_IPDOTADDR:
                            case CLI_IPDOTMASK:
                            case CLI_IPINTADDR:
                            case CLI_IPINTMASK:
                                vosPrintf(pstEnv->nWriteFd, "  A.B.C.D");
                                break;

                            case CLI_MACADDR:
                                vosPrintf(pstEnv->nWriteFd, "  XX:XX:XX:XX:XX:XX");
                                break;
                            default:
                                break;
                            }

                            break;

                        default:
                            break;
                        }

                        vosFree(psNodeNext);
                        psNodeNext = NULL ;
                        if (!(nLen % 5))
                            vosPrintf(pstEnv->nWriteFd, "\r\n");
                    }

                    if (nLen % 5)
                        vosPrintf(pstEnv->nWriteFd, "\r\n");

                    pstEnv->phase = CLI_CMD_PHASE_HELP;

                    cliKeyParaLinkFree(&psRootTemp);
                    cliCollectionFree(psParaNode,psNode,NULL);
                }

                return OK;

            case WORD_END_QUESTION:
                pchWord[strlen(pchWord)-1] = '\0';
                nFlag = cliCmdMatch(pstEnv,psForestRoot,pchWord,&psNode);
                if (ERROR == nFlag)
                {
                    cliKeyParaLinkFree(&psRootTemp);
                    cliCollectionFree(psParaNode,psNode,NULL);

                    vosPrintf(pstEnv->nWriteFd, "%% Unrecognized command.\r\n");
					cliCmdHistoryProcess(pstEnv, SAVE, pstEnv->command);

                    if (pCmdValidity)
                        *pCmdValidity = CMD_UNRECOGNIZED;

                    return OK;
                }
                psNodeNext = psNode->next;
                psPreNode = psNodeNext;

                while (psNodeNext && psNodeNext->next)
                {
                    psCurrentNode = psNodeNext->next;
                    while (psCurrentNode )
                    {
                        nCompResult = strcmp(psNodeNext->p->key,psCurrentNode->p->key);
                        if (0 == nCompResult)
                        {
                            psPreNode->next = psCurrentNode->next;
                            psNode->total--;
                            vosFree(psCurrentNode);
                            psCurrentNode = psPreNode->next;
                            continue;
                        }
                        psPreNode = psCurrentNode;
                        psCurrentNode = psCurrentNode->next;
                    }
                    psNodeNext = psNodeNext->next;
                }

                pstEnv->phase = CLI_CMD_PHASE_HELP;
                nLen = 0;

                while (psNode->total>0)
                {
                    nLen++;
                    psNode->total--;
                    psNodeNext = psNode->next;
                    psNode->next = psNode->next->next;

                    switch(psNodeNext->p->type)
                    {
                    case NT_KEY:
                        vosPrintf(pstEnv->nWriteFd, "  %s",psNodeNext->p->key);
                        break;
                    case NT_PARAMETER:
                        switch(psNodeNext->p->parameter->type)
                        {
                        case CLI_WORD:
                            vosPrintf(pstEnv->nWriteFd, "  %s","INPUT");
                            break;
                        case CLI_STRING:
                            vosPrintf(pstEnv->nWriteFd, "  %s","INPUT");
                            break;
                        case CLI_DATE:
                            vosPrintf(pstEnv->nWriteFd, "  %s","yyyy-mm-dd");
                            break;
                        case CLI_TIME:
                            vosPrintf(pstEnv->nWriteFd, "  %s","hh:mm:ss");
                            break;
                        case CLI_INTEGER:
                            vosPrintf(pstEnv->nWriteFd, "  <%ld-%ld>",psNodeNext->p->parameter->min,psNodeNext->p->parameter->max);
                            break;
                        case CLI_UINT:
                            vosPrintf(pstEnv->nWriteFd, "  <%lu-%lu>",(UINT32)psNodeNext->p->parameter->min,(UINT32)psNodeNext->p->parameter->max);
                            break;
                        case CLI_IPDOTADDR:
                        case CLI_IPDOTMASK:
                        case CLI_IPINTADDR:
                        case CLI_IPINTMASK:
                            vosPrintf(pstEnv->nWriteFd, "  A.B.C.D");
                            break;

                        case CLI_MACADDR:
                            vosPrintf(pstEnv->nWriteFd, "  XX:XX:XX:XX:XX:XX");
                            break;
                        default:
                            break;
                        }

                        break;

                    default:
                        break;
                    }

                    vosFree(psNodeNext);
                    psNodeNext = NULL ;
                    if (!(nLen % 5))
                        vosPrintf(pstEnv->nWriteFd, "\r\n");
                }

                if (nLen % 5)
                    vosPrintf(pstEnv->nWriteFd, "\r\n");

                cliKeyParaLinkFree(&psRootTemp);
                cliCollectionFree(psParaNode,psNode,NULL);
                return OK;

            default:
                if ((pstEnv->command->achCmdPool[pstEnv->command->nInputPos-1] != TAB)
                        && (pstEnv->command->achCmdPool[pstEnv->command->nInputPos-1] != '?'))
                {
                    cliCmdHistoryProcess(pstEnv,SAVE,pstEnv->command);
                }

                nFlag = cliCmdMatch(pstEnv, psForestRoot, pchWord, &psNode);
                bIsNeed = cliTreeNodeIsAllEqual(psNode);
                cliKeyParaLinkFree(&psRootTemp);
                if (ERROR == nFlag || 0 == psNode->total)
                {
                    vosPrintf(pstEnv->nWriteFd, "%% Unrecognized command.\r\n");
					cliCmdHistoryProcess(pstEnv, SAVE, pstEnv->command);

                    if (pCmdValidity)
                        *pCmdValidity = CMD_UNRECOGNIZED;

                    cliCollectionFree(psParaNode,psNode,pstEnv->command);
                    return OK;
                }

                psNodeNext=psNode->next;
                bIsKeyForceMatch = FALSE;
                bIsKeyMatch = FALSE;
                bIsKeyMatchMore = FALSE;
                bIsParaMatch = FALSE;
                bIsParaMore = FALSE;

                while (psNodeNext)
                {
                    switch (psNodeNext->p->type)
                    {
                    case NT_KEY:
                        bIsError = FALSE;
                        psCmdKeyParaCurrent = psNodeNext->p;
                        cliStrToLower (pchWord);

                        if (TRUE == bIsKeyMatch)
                            bIsKeyMatchMore = TRUE;
                        else
                            bIsKeyMatch = TRUE;

                        if (strcmp(pchWord,psNodeNext->p->key) == 0 || TRUE == bIsNeed)
                        {
                            if (NULL == psRootTemp)
                            {
                                if (cliTreeNodeCreate(NULL, &psRootTemp))
                                {
                                    return ERROR;
                                }

                                psRootTemp->type = NT_VIRTUAL_LOAD;
                            }

                            if (cliTreeNodeCreate(NULL, &psNewCmdKeyParaNode))
                            {
                                cliKeyParaLinkFree(&psRootTemp);
                                return ERROR;
                            }

                            psNewCmdKeyParaNode->type = NT_VIRTUAL_FUNCTION;
                            psNewCmdKeyParaNode->parent = psRootTemp;
                            psNewCmdKeyParaNode->explain = psNodeNext->p->next;
                            psNodeNext->p->next->parent = psNewCmdKeyParaNode;

                            if (NULL == psRootTemp->explain)
                            {
                                psRootTemp->explain = psNewCmdKeyParaNode;
                            }
                            else
                            {
                                psCmdKeyParaTmp = psRootTemp->explain;
                                while ((psCmdKeyParaTmp != NULL) && (psCmdKeyParaTmp->right != NULL))
                                    psCmdKeyParaTmp = psCmdKeyParaTmp->right;

                                psCmdKeyParaTmp->right = psNewCmdKeyParaNode;
                                psNewCmdKeyParaNode->left = psCmdKeyParaTmp;
                            }

                            bIsKeyForceMatch = TRUE;
                            bIsKeyMatch = FALSE;
                            psKeepNode = psNodeNext->p->next;
                        }

                        if (NULL == psCmdKeyParaCurrent->parent
                                || (psCmdKeyParaCurrent->parent->type != NT_VIRTUAL_REQUIRED
                                        && psCmdKeyParaCurrent->parent->type != NT_VIRTUAL_OPTIONAL)
                                || psCmdKeyParaCurrent->pre)
                        {
                            if (FALSE == bIsKeyForceMatch)
                            {
                                psKeepNode = psNodeNext->p->next;
                                bIsParaMatchMore = FALSE;
                            }
                            break;
                        }

                        if (FALSE == bIsKeyForceMatch && FALSE == bIsKeyMatch)
                        {
                            break;
                        }

                        if (TRUE == bIsKeyForceMatch && TRUE == bIsKeyMatch)
                        {
                            break;
                        }

                        psNewParaNode = NULL;
                        psParaNext = NULL;
                        psParaLast = NULL;

                        while (psCmdKeyParaCurrent->parent
                                && (NT_VIRTUAL_REQUIRED == psCmdKeyParaCurrent->parent->type
                                        || NT_VIRTUAL_OPTIONAL == psCmdKeyParaCurrent->parent->type))
                        {
                            if (psCmdKeyParaCurrent->pre)
                            {
                                break;
                            }

                            if (NULL == psParaNode)
                            {
                                psParaNode = (PARA_COLLECTION_t *)vosAlloc(sizeof(PARA_COLLECTION_t));
                                if (NULL == psParaNode)
                                {
                                    bIsError = TRUE;
                                    break;
                                }
                                psParaNode->last = psParaNode;
                                psParaNode->next = NULL;
                                psParaNode->p = NULL;
                                psParaNode->type = 0;
                                psParaNode->pos = 0;
                            }
                            psNewParaNode = (PARA_COLLECTION_t *)vosAlloc(sizeof(PARA_COLLECTION_t));
                            if (NULL == psNewParaNode)
                            {
                                bIsError = TRUE;
                                break;
                            }
                            psNewParaNode->p = (PARA_TABLE_t *)vosAlloc(sizeof(PARA_TABLE_t));
                            if (NULL == psNewParaNode->p)
                            {
                                vosFree(psNewParaNode);
                                psNewParaNode = NULL ;
                                bIsError = TRUE;
                                break;
                            }
                            psNewParaNode->last = NULL;
                            psNewParaNode->next = NULL;
                            psNewParaNode->p->i = psCmdKeyParaCurrent->keyword_position;
                            psNewParaNode->type = CLI_INTEGER;
                            psNewParaNode->pos = psCmdKeyParaCurrent->parent->parameter_postion;

                            if (NULL == psParaLast)
                                psParaLast = psNewParaNode;

                            if (NULL == psParaNext)
                            {
                                psParaNext = psNewParaNode;
                            }
                            else
                            {
                                psNewParaNode->next = psParaNext;
                                psParaNext = psNewParaNode;
                                psNewParaNode = NULL;
                            }

                            psCmdKeyParaCurrent = psCmdKeyParaCurrent->parent;
                        }

                        if (TRUE == bIsError)
                        {
                            cliCollectionFree(psParaNext, NULL, NULL);
                        }
                        else
                        {
                            psParaNode->last->next = psParaNext;
                            psParaNode->last = psParaLast;
                            psKeepNode = psNodeNext->p->next;
                        }
                        break;

                    case NT_PARAMETER:
                        bIsError = FALSE;
                        psCmdKeyParaCurrent = psNodeNext->p;
                        if (TRUE == bIsKeyForceMatch)
                        {
                            break;
                        }

                        if (TRUE == bIsParaMatch && TRUE == bIsParaMore)
                        {
                            break;
                        }

                        if (NULL == psParaNode)
                        {
                            psParaNode = (PARA_COLLECTION_t*)vosAlloc(sizeof(PARA_COLLECTION_t));
                            if (NULL == psParaNode)
                            {
                                bIsError = TRUE;
                                break;
                            }
                            psParaNode->last = psParaNode;
                            psParaNode->next = NULL;
                            psParaNode->p = NULL;
                            psParaNode->type = 0;
                            psParaNode->pos = 0;
                        }
                        psNewParaNode = (PARA_COLLECTION_t *)vosAlloc(sizeof(PARA_COLLECTION_t));

                        if (NULL == psNewParaNode)
                        {
                            break;
                        }

                        psNewParaNode->last = psNewParaNode;
                        psNewParaNode->next = NULL;
                        psNewParaNode->p = NULL;
                        psNewParaNode->pos = 0;
                        psNewParaNode->type = 0;
                        nFlag = cliParameterCheck(psNewParaNode, psNodeNext->p,
                        pchWord, &pchCommand);
                        if (ERROR == nFlag)
                        {
                            vosFree(psNewParaNode);
                            psNewParaNode = NULL;
                            bIsError = TRUE;
                        }
                        else
                        {
                            if (TRUE == bIsParaMatch)
                                bIsParaMore = TRUE;
                            else
                                bIsParaMatch = TRUE;

                            if (FALSE == bIsParaMore)
                            {
                                psParaNode->last->next = psNewParaNode->next;
                                psParaNode->last = psNewParaNode->last;
                                vosFree(psNewParaNode);
                                psNewParaNode = NULL;
                                psKeepNode = psNodeNext->p;
                                bIsKeyForceMatchMore = FALSE;

                                psForestRoot = psKeepNode->next;
                            }

                            if (TRUE == bIsParaMore)
                                bIsParaMatchMore = TRUE;

                            if (TRUE == bIsParaMatchMore)
                            {
                                if (cliTreeNodeCreate(NULL, &psNewCmdKeyParaNode))
                                {
                                    cliKeyParaLinkFree(&psRootTemp);
                                    return ERROR;
                                }

                                psNewCmdKeyParaNode->type = NT_VIRTUAL_FUNCTION;
                                psCmdKeyParaTmp = psRootTemp->explain;
                                while (psCmdKeyParaTmp->right != NULL)
                                    psCmdKeyParaTmp = psCmdKeyParaTmp->right;

                                psCmdKeyParaTmp->right = psNewCmdKeyParaNode;
                                psNewCmdKeyParaNode->left = psCmdKeyParaTmp;
                                psNewCmdKeyParaNode->parent = psRootTemp;
                                psNewCmdKeyParaNode->explain = psKeepNode->next;
                                psKeepNode = psRootTemp ;
                            }
                        }
                        break;
                    default:
                        bIsError = TRUE;
                        break;
                    }

                    psNodeNext = psNodeNext->next;
                }

                bIsNeed = FALSE;

                if (TRUE == bIsKeyForceMatch)
                    psForestRoot = psRootTemp;

                if (TRUE == bIsKeyMatchMore && FALSE == bIsKeyForceMatch)
                {
                    vosPrintf(pstEnv->nWriteFd, "%% Unrecognized command.\r\n");
                    cliCmdHistoryProcess(pstEnv,SAVE,pstEnv->command);

                    if (pCmdValidity)
                        *pCmdValidity = CMD_UNRECOGNIZED;

                    cliKeyParaLinkFree(&psRootTemp);
                    cliCollectionFree(psParaNode,psNode,pstEnv->command);
                    return OK;
                }

                if (TRUE == bIsKeyMatch && FALSE == bIsKeyForceMatch)
                    psForestRoot = psKeepNode;

                if (TRUE == bIsParaMore)
                {
                    if (pstEnv->command->achCmdPool[pstEnv->command->nInputPos] == TAB)
                        vosPrintf(pstEnv->nWriteFd, "\r\n");

                    vosPrintf(pstEnv->nWriteFd, "%% Invalid input detected.\r\n");
					cliCmdHistoryProcess(pstEnv,SAVE,pstEnv->command);

                    if (pCmdValidity)
                        *pCmdValidity = CMD_INVALID_INPUT;

                    cliKeyParaLinkFree(&psRootTemp);
                    cliCollectionFree(psParaNode,psNode,pstEnv->command);
                    return OK;
                }
                if (TRUE == bIsParaMatch)
                    psForestRoot = psKeepNode->next;

                if (FALSE == bIsKeyMatch && FALSE == bIsKeyMatchMore
                        && FALSE == bIsParaMatch && FALSE == bIsParaMore
                        && FALSE == bIsKeyForceMatch)
                {
                    vosPrintf(pstEnv->nWriteFd, "%% Invalid input detected.\r\n");
					cliCmdHistoryProcess(pstEnv,SAVE,pstEnv->command);

                    if (pCmdValidity)
                        *pCmdValidity = CMD_INVALID_INPUT;

                    cliKeyParaLinkFree(&psRootTemp);
                    cliCollectionFree(psParaNode,psNode,pstEnv->command);
                    return ERROR;
                }
            break;
            }
        }
    }
    return OK;
}


/*****************************************************************************
 *cliCmdInput - get command from INPUT terminal.
 *DESCRIPTION
 *      1. get one character from terminal
 *      2. save it
 *      3. echo it while succeeding in saving it
 *      4. if END_OF_LINE character received, exit.
 *         END_OF_LINE characters include: <TAB>,<?>,<CR>,<Up>,<Down>.
 *      We support to remove the last character by <Backspace>
 *      While inputing more than one <Blankspace> continuously,
 *      only one <Blankspace> key is saved.
 *Input:pstEnv,pusIsCtrlC.
 *Output:N/A.
 *Return: STATUS .
 ****************************************************************************/
STATUS cliCmdInput(ENV_t *pstEnv, unsigned short  *pusIsCtrlC)
{
    unsigned short  usInputChar = 0xffff;
    unsigned short  usTemp;

    unsigned char telOpt[3] = {0};
    unsigned char telSubOpt[10] = {0};
    int telOptLen = 0;
    VOS_TIME_t start;

    *pusIsCtrlC = FALSE;
    do
    {
        if (TERMINAL_IO_FAULT == pstEnv->err)
            return ERROR;

        usInputChar = cliCharGet(pstEnv);
        pstEnv->used = TRUE;
        switch(usInputChar)
        {
        case 0xffff: /* invalid character. Do nothing*/
            break;

        case BSP:  /* <Backspace>. If the command pool isn't empty, remove the last character input from pool and screen */
            if (!cliCmdPoolIsEmpty((CMD_POOL_t *)(pstEnv->command)))
            {
                if (cliCmdPop((CMD_POOL_t *)(pstEnv->command)) != 0x0)
                    cliEcho(pstEnv, usInputChar);
            }
            break;

        case NL:/* <CR>. Insert  ' ' at the end of the pool. */
            cliNewLineOutput(pstEnv);
            break;
        case CR:
            usTemp = cliCharGet(pstEnv);
            if (usTemp == NL || usTemp == 0xffff)
            {
                cliNewLineOutput(pstEnv);
            }
            break;

        case TAB:/* <TAB>. Insert it and  ' ' at the end of the pool, change the PHASE to CLI_CMD_PHASE_SUPPLY then exit. */
            cliCmdPush(usInputChar, pstEnv->command);
            /*cliEcho(pstEnv, '\n');*/
            return OK;

        case '?':/* <?>. Insert it and ' ' at the end of the pool, change the PHASE to CLI_CMD_PHASE_HELP the exit. */
            cliCmdPush(usInputChar,pstEnv->command);
            //cliEcho(pstEnv,usInputChar);
            cliNewLineOutput(pstEnv);
            return OK;

        case CURSOR_UP:/* <Up>/<Down>. Remove the command just input */
        case CTRLP:
            cliLineClear(pstEnv);
            cliCmdPoolClear(pstEnv->command);
            cliCmdHistoryProcess(pstEnv, GETPRE, pstEnv->command);
            cliCmdPoolPrint(pstEnv);
            break;
        case CURSOR_DOWN:
        case CTRLN:
            cliLineClear(pstEnv);
            cliCmdPoolClear(pstEnv->command);
            cliCmdHistoryProcess(pstEnv, GETNEXT, pstEnv->command);
            cliCmdPoolPrint(pstEnv);
            break;
        case CURSOR_RIGHT:
            cliCursorMoveToRight (pstEnv);
            break;
        case CURSOR_LEFT:
            cliCursorMoveToLeft (pstEnv);
            break;
        case CTRLC:
            *pusIsCtrlC = TRUE;
            cliNewLineOutput(pstEnv);
            cliCmdPoolClear(pstEnv->command);
            return OK;
        case CTRLZ:
            cliNewLineOutput(pstEnv);
            cliCmdPoolClear(pstEnv->command);
            if (pstEnv->pmode!=CLI_MODE_USER)
                pstEnv->pmode = CLI_MODE_USER;
            return OK;

        /* process telnet message */
        case IAC:
            telOptLen = 0;
            start = vosTimeGet();
            do {
                telOptLen += vosSafeRead(pstEnv->nReadFd, telOpt, 2);
                if ((vosTimeGet() - start) >= 3)
                {
                    break;
                }
            } while (telOptLen < 2);

            if (telOpt[0] == SB && telOpt[1] == TELOPT_NAWS)
            {
                telOptLen = 0;
                start = vosTimeGet();
                do {
                    telOptLen += vosSafeRead(pstEnv->nReadFd, telSubOpt, 6);
                    if ((vosTimeGet() - start) >= 3)
                    {
                        break;
                    }
                } while (telOptLen < 6);
                /*
                 * IAC -> SB -> TELOPT_NAWS -> 4-byte -> IAC -> SE
                 */
                /*
                struct winsize ws;
                ws.ws_col = (telSubOpt[0] << 8) | telSubOpt[1];
                ws.ws_row = (telSubOpt[2] << 8) | telSubOpt[3];
                */
                pstEnv->ulTermWidth = (telSubOpt[0] << 8) | telSubOpt[1];
                pstEnv->ulTermHeight = (telSubOpt[2] << 8) | telSubOpt[3];
                /*
                vosPrintk("Receive NAWS width = %d, height = %d\r\n",
                    pstEnv->ulTermWidth, pstEnv->ulTermHeight);
                */
                /* ioctl(ts->nWriteFd, TIOCSWINSZ, (char *)&ws); */
            } else {
                /* skip other IAC */
                /*
                vosPrintk("Receive IAC action = %d, option = %d\r\n",
                    telOpt[0], telOpt[1]);
                */
            }
            break;


        default:/* normal key(A~Z, a~z,0~9). Insert it at the end of pool */
            if (cliCmdPush(usInputChar, pstEnv->command) == OK)
                cliEcho(pstEnv, usInputChar);

            break;
        }
    }while(!(NL == usInputChar || CR == usInputChar));
    return OK;
}


/*****************************************************************************
 *cliCmdProcess - To handle command.

 *DESCRIPTION
 *Input: pstEnv.
 *Output:n/a.
 *Return:n/a.
 ****************************************************************************/
void cliCmdProcess(ENV_t *pstEnv)
{
    unsigned short usIsCtrlC = FALSE;
    CMD_POOL_t *psCmdPool = NULL;
    int i;
    unsigned char telOpt[3] = {0};
    unsigned char telSubOpt[10] = {0};
    int telOptLen = 0;
    VOS_THREAD_t pstSessionTimoutTaskID = NULL;

    if (!pstEnv)
    {
        return;
    }

restart:

    pstEnv->used = FALSE;

	#if 0
    pstSessionTimoutTaskID = vosThreadCreate ("tSessionTmt",OP_VOS_THREAD_STKSZ,110,
                         (FUNCPTR)cliSessionTimeoutTask,
                         (void *)pstEnv);
	#endif

    if (NO_ERROR == odmUserLogin(pstEnv))
    {
       /* pstEnv->pmode = DEF_CLI_MODE;*/
        pstEnv->prompt |= DEF_CLI_MODE;
        pstEnv->para = NULL;
        pstEnv->command = (CMD_POOL_t *)vosAlloc(sizeof(CMD_POOL_t));
        memset(pstEnv->command, 0, sizeof(CMD_POOL_t));
        psCmdPool = pstEnv->command;
        psCmdPool->bIsEmpty = TRUE;
        psCmdPool->bIsFull = FALSE;

        pstEnv->configureHistoryCommand = (CMD_HISTORY_t *)vosAlloc(sizeof(CMD_HISTORY_t));
        memset(pstEnv->configureHistoryCommand, 0, sizeof(CMD_HISTORY_t));

        pstEnv->close = CLI_CLOSE_NULL;
        pstEnv->err = OK;
        pstEnv->free = 0;
        pstEnv->phase = CLI_CMD_PHASE_INIT;
        /*pstEnv->pmode = CLI_MODE_USER;*/

        cliEchoEnable(pstEnv);
        if (pstEnv->ucIsConsole)
        {
			vosTermBackup(pstEnv->nReadFd);
            vosTermConfig(pstEnv->nReadFd);
        }
        else
        {
#ifndef LINUX
            ioTaskStdSet (0, STD_OUT, pstEnv->nWriteFd);
            ioTaskStdSet (0, STD_IN, pstEnv->nReadFd);
            ioTaskStdSet (0, STD_ERR, pstEnv->nErrFd);
#endif
        }

        while(1)
        {
            if (TERMINAL_IO_FAULT == pstEnv->err)
            {
                break;
            }
            if (pstEnv->phase != CLI_CMD_PHASE_SUPPLY)
            {
                cliPromptPrint(pstEnv);
                vosPrintf(pstEnv->nWriteFd, " ");
                cliCmdPoolPrint(pstEnv);
            }

            pstEnv->phase = CLI_CMD_PHASE_INIT;

            if (cliCmdInput(pstEnv,&usIsCtrlC) == ERROR)
                break;

            if (TRUE == usIsCtrlC)
                break;

            cliCmdInterpreter(pstEnv,g_psRootOfForest,NULL);

            if (pstEnv->phase == CLI_CMD_PHASE_EXIT)
                break;

            if ((CLI_CMD_PHASE_HELP == pstEnv->phase) &&
                (psCmdPool->nInputTailPos >= 0) &&
                (('?' == psCmdPool->achCmdPool[psCmdPool->nInputTailPos]) ||
                (TAB == psCmdPool->achCmdPool[psCmdPool->nInputTailPos]))) /* not single '?' or TAB */
            {
                psCmdPool->achCmdPool[psCmdPool->nInputTailPos] = 0x00;
                if (psCmdPool->nInputTailPos > 0)
                {
                    psCmdPool->nInputTailPos--;
                }
                else
                {
                    psCmdPool->bIsEmpty = TRUE;
                    psCmdPool->bIsFull = FALSE;
                }
                psCmdPool->nCursorPos--;
                if (psCmdPool->nDispEnd > 0)
                {
                    psCmdPool->nDispEnd--;
                }
                psCmdPool->nInputPos--;
                psCmdPool->nInputLen--;
                psCmdPool->nDispLen--;

            }
            else
            {
                if (CLI_CMD_PHASE_SUPPLY == pstEnv->phase)
                {
                    if (TAB == psCmdPool->achCmdPool[psCmdPool->nInputTailPos])
                    {
                        psCmdPool->achCmdPool[psCmdPool->nInputTailPos] = 0x00;
                        if (psCmdPool->nInputTailPos > 0)
                        {
                            psCmdPool->nInputTailPos--;
                        }
                        else
                        {
                            psCmdPool->bIsEmpty = TRUE;
                            psCmdPool->bIsFull = FALSE;
                        }
                        psCmdPool->nCursorPos--;
                        if (psCmdPool->nDispEnd > 0)
                        {
                            psCmdPool->nDispEnd--;
                        }
                        psCmdPool->nInputPos--;
                        psCmdPool->nInputLen--;
                        psCmdPool->nDispLen--;
                    }
                }
                else
                {
                    cliCmdPoolClear(psCmdPool);
                }
            }
        }
    }
    if (!pstEnv->free)
    {
        //vosPrintk("free session\r\n");
        cliResourceFree (pstEnv);
    }

	#if 0
    if (vosThreadVerify(pstSessionTimoutTaskID) == OK)
    {
        /*   Kill Task  */
        //vosPrintk("Kill session timeout task\r\n");
        vosThreadDestroy(pstSessionTimoutTaskID);
    }
	#endif

    if (pstEnv->ucIsConsole)
    {
        vosTermRestore(pstEnv->nReadFd);
#if 1  /* If you need to exit normally, please set to zero */
        vosUSleep(10);
        goto restart;
#else
        if (pstEnv->phase == CLI_CMD_PHASE_EXIT)
            goto restart;
#endif /* #if 0 */
    }
}


