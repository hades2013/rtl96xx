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
* FILENAME:  errorStrings.c
*
* DESCRIPTION: 
*   
*
* Date Created: Apr 23, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/src/errorStrings.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "errorStrings.h"

char g_acUnknownErrorStr[] = "<Unknown Error>";
static HASH_TABLE_t *g_pstErrString = NULL;

typedef struct ERR_CODE_s {
    char *pcErrString;
    char *pcErrCodeString;
} ERR_CODE_t;

static void odmErrorStringDataDestroy(void *key, void *value)
{
    char *k = key;
    ERR_CODE_t *v = value;
    vosFree(k);
    if (NULL != v->pcErrString)
    {
        vosFree(v->pcErrString);
    }
    if (NULL != v->pcErrCodeString)
    {
        vosFree(v->pcErrCodeString);
    }
    vosFree(v);
}

STATUS odmErrorStringInit()
{
    g_pstErrString = vosHashCreate(vosStringHash, vosEqualString, odmErrorStringDataDestroy, 0);
    if (NULL == g_pstErrString)
    {
        return ERROR;
    }

    INSERT_ERROR(NO_ERROR, "No Error");

    // DRIVERS
    INSERT_ERROR(DVRS_INIT_ERROR,
                 "Dirvers init error");

    // MULTICAST
    INSERT_ERROR(MULTICAST_INIT_ERROR,
                 "Multicast init error");

    // PORTS
    INSERT_ERROR(PORTS_INIT_ERROR,
                 "Ports init error");

    // QOS
    INSERT_ERROR(QOS_INIT_ERROR,
                 "QoS init error");

    // SECURITY

    // SNMP
    INSERT_ERROR(SNMP_INIT_ERROR,
                 "Snmp init error");

    // SYSTEM
    INSERT_ERROR(SYS_INIT_ERROR,
                 "System init error");
    INSERT_ERROR(SYS_USER_INVALID,
                 "System user invalid");

    // WEB
    INSERT_ERROR(WEB_INIT_ERROR,
                 "Web init error");

    return OK;
}

STATUS odmErrorStringDestroy()
{
    vosHashFree(g_pstErrString);

    return OK;
}

STATUS odmErrorStringInsert(
    UINT32 ulErrCode, 
    const char *pcErrString, 
    const char *pcErrCodeString)
{
    char *pcKey = NULL;
    ERR_CODE_t *pstErrCode = NULL;
    char aKeyStr[STR_SIZE_32_BYTES] = {0};

    pstErrCode = (ERR_CODE_t *)vosAlloc(sizeof(ERR_CODE_t));

    if (pstErrCode != NULL)
    {
        vosMemSet(pstErrCode, 0, sizeof(ERR_CODE_t));

        vosSnprintf(aKeyStr, STR_SIZE_32_BYTES, "%08X", ulErrCode);
        pcKey = vosStrDup(aKeyStr);
        if (NULL != pcErrString)
        {
            pstErrCode->pcErrString = vosStrDup(pcErrString);
        }
        if (NULL != pcErrCodeString)
        {
            pstErrCode->pcErrCodeString = vosStrDup(pcErrCodeString);
        }
        if (NULL != pcKey && 
            NULL != pstErrCode->pcErrString)
        {
            vosHashInsert((void *)pcKey, (void *)pstErrCode, g_pstErrString);
            return OK;
        }
    }
    return ERROR;
}

UINT32 odmErrorStringDelete(UINT32 ulErrCode)
{
    char aKeyStr[STR_SIZE_32_BYTES] = {0};
    vosSnprintf(aKeyStr, STR_SIZE_32_BYTES, "%08X", ulErrCode);
    vosHashRemove((void *)aKeyStr, g_pstErrString);
    
    return OK;
}

const char * odmErrorStringGet(UINT32 ulErrCode)
{
    ERR_CODE_t *pstErrCode = NULL;
    char aKeyStr[STR_SIZE_32_BYTES] = {0};

    vosSnprintf(aKeyStr, STR_SIZE_32_BYTES, "%08X", ulErrCode);
    pstErrCode = (ERR_CODE_t *)vosHashValue((void *)aKeyStr, g_pstErrString);
    if (NULL != pstErrCode)
    {
        return pstErrCode->pcErrString;
    }
    else
    {
        return g_acUnknownErrorStr;
    }
}

const char * odmErrorCodeStringGet(UINT32 ulErrCode)
{
    ERR_CODE_t *pstErrCode = NULL;
    char aKeyStr[STR_SIZE_32_BYTES] = {0};

    vosSnprintf(aKeyStr, STR_SIZE_32_BYTES, "%08X", ulErrCode);
    pstErrCode = (ERR_CODE_t *)vosHashValue((void *)aKeyStr, g_pstErrString);
    if (NULL != pstErrCode)
    {
        return (pstErrCode->pcErrCodeString != NULL) ? pstErrCode->pcErrCodeString : g_acUnknownErrorStr;
    }
    else
    {
        return g_acUnknownErrorStr;
    }
}

