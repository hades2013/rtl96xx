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
* FILENAME:  cli_page.c
*
* DESCRIPTION: 
*	To surport showing in more screen.
*
* Date Created: Aug 12, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_page.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "cli.h"
#include <vos_alloc.h>

#ifdef LINUX
#include <unistd.h>
#endif
#define DISABLE_PAGING 1

int g_cliDividePage;

void cliPrintInMorePage(ENV_t *pstEnv, char *pMessage);

#ifndef LINUX
#define MEM_FILE_NAME "/mem/pool%0.8x"
#else
#define MEM_FILE_NAME "/pool%0.8x"
#endif

static STATUS memFileNameGet(ENV_t *pstEnv, char *pcDest)
{
    int r = 0;
    if (NULL == pcDest)
        return ERROR;

#ifndef LINUX
    r = vosSprintf(pcDest, MEM_FILE_NAME, pstEnv->pstMainTaskId);
    if (0 <= r) 
    {
        return OK;
    }
    else 
    {
        return ERROR;
    }

#else
    r = odmSysCfgTempDirGet(pcDest);
    if (ERROR == r)
        return ERROR;

    r = vosSprintf(pcDest+strlen(pcDest), MEM_FILE_NAME, pstEnv->pstMainTaskId);
    if (0 <= r) 
    {
        return OK;
    }
    else 
    {
        return ERROR;
    }


#endif
}

/*****************************************************************************
 *cliShowStart - start paging display.
 *DESCRIPTION
 *  This function is used to paging display, which is called before 
 *  you call printf
 ****************************************************************************/
void cliShowStart(ENV_t *pstEnv)
{
    char aucMemPathName[STR_SIZE_64_BYTES] = {0};
    if (NULL == pstEnv)
    {
        return;
    }
	/*Whether close divide page*/
    if (DISABLE_PAGING == g_cliDividePage)
    {
        return;
    }

    memset(&pstEnv->page, 0, sizeof(PAGING_t));
    memFileNameGet(pstEnv, aucMemPathName);

    if (NULL == pstEnv->page.pPrivMem)
    {
        pstEnv->page.pPrivMem = (char *)vosAlloc(PAGE_MEMORY+1);   
    }

#ifndef LINUX

    memset(pstEnv->page.pPrivMem, 0, PAGE_MEMORY+1);
    
    /* create pseudo memory device */
    if (ERROR == memDevCreate(aucMemPathName,
                              pstEnv->page.pPrivMem, PAGE_MEMORY))  
    {
        return;
    }

    pstEnv->page.nPageFd = open(aucMemPathName, O_RDWR, 0);
    if (ERROR == pstEnv->page.nPageFd)
    {   
        memDevDelete(aucMemPathName);      
        return;
    }
    pstEnv->page.nOldStdOutFd = ioTaskStdGet(0, STD_OUT);   
    ioTaskStdSet(0, STD_OUT, pstEnv->page.nPageFd);         

#else

    pstEnv->page.nPageFd = open(aucMemPathName, O_CREAT | O_RDWR | O_TRUNC, 0);
    if (ERROR == pstEnv->page.nPageFd)
    {   
        return;
    }
    /* create a duplicate handle for standard output */
    pstEnv->page.nOldStdOutFd = dup(pstEnv->nWriteFd);
    /*
　　redirect old output to paging fd
　　by duplicating the file handle onto the
　　file handle for standard output.
　　*/
    dup2(pstEnv->page.nPageFd, pstEnv->nWriteFd);
    close(pstEnv->page.nPageFd);
#endif
}


/*****************************************************************************
 *cliShowEnd - Print one page and free resource after it.
 *DESCRIPTION
 *This routine mainly to print one page and free resource after it.
 ****************************************************************************/
void cliShowEnd(ENV_t *pstEnv)
{
    long bytes_read = 0;
    char aucMemPathName[STR_SIZE_64_BYTES] = {0};
    if (NULL == pstEnv)
    {
        return;
    }
	/*Whether close divide page*/
    if(DISABLE_PAGING == g_cliDividePage)
    {
    	 return;  
    }
    if (NULL == pstEnv->page.pPrivMem)
    {
        return;
    }

    memFileNameGet(pstEnv, aucMemPathName);

#ifndef LINUX

    if (ERROR != pstEnv->page.nOldStdOutFd)
    {
        ioTaskStdSet(0, STD_OUT, pstEnv->page.nOldStdOutFd);
    }
    cliPrintInMorePage(pstEnv, pstEnv->page.pPrivMem);
    if (ERROR != pstEnv->page.nPageFd)
    {
        close(pstEnv->page.nPageFd);                        
    }
    memDevDelete(aucMemPathName);

#else

    if (ERROR != pstEnv->page.nOldStdOutFd)
    {
        /* restore original standard output handle */
        dup2(pstEnv->page.nOldStdOutFd, pstEnv->nWriteFd);
        /* close duplicate handle for STDOUT */
        close(pstEnv->page.nOldStdOutFd);
    }
    pstEnv->page.nPageFd = open(aucMemPathName, O_RDONLY, 0);
    if (ERROR != pstEnv->page.nPageFd)
    {
        bytes_read = vosSafeRead(pstEnv->page.nPageFd, pstEnv->page.pPrivMem, PAGE_MEMORY);
        if (0 < bytes_read)
        {
            pstEnv->page.pPrivMem[bytes_read] = 0;
            cliPrintInMorePage(pstEnv, pstEnv->page.pPrivMem);
        }
        close(pstEnv->page.nPageFd);
    }
    vosRemove(aucMemPathName);
    
#endif 
    if (pstEnv->page.pPrivMem)
    {
        vosFree(pstEnv->page.pPrivMem);
        pstEnv->page.pPrivMem = NULL;
    }
    pstEnv->page.nPageFd = ERROR;
    pstEnv->page.nOldStdOutFd = ERROR;
}

/*****************************************************************************
 *cliPrintInMorePage - Print one Page.
 *DESCRIPTION
 *This routine mainly to print one page from memory buffer.
 ****************************************************************************/
void cliPrintInMorePage(ENV_t *pstEnv, char *pMessage)
{
    int   k = 0;
    int   nLineNum = 0;
    char  *pCurrMsg=NULL;
    char  *pCharFinded = NULL;
    char  *pCurrInPage;
    UINT16 usTemp;
    char  chTmp;
    #define LINECHARNUM   255 
    char  *pchPage;  
    int   LINENUMS = 23; 

    if (NULL == pstEnv)
        return;

    if (pstEnv->ulTermHeight > 0)
    {
        LINENUMS = pstEnv->ulTermHeight - 1;
    }

#if 0 /* test code */
    vosPrintf(pstEnv->nWriteFd, "LINENUMS = %d, window size: width = %d, height = %d\r\n",
                    LINENUMS, pstEnv->ulTermWidth, pstEnv->ulTermHeight);
#endif /* 0 */
    
    if ((pchPage = (char *)vosAlloc(LINENUMS*LINECHARNUM)) == NULL)
    {
        vosPrintf(pstEnv->nWriteFd, "CLIModule: Fail to Malloc memory\r\n");
        return;
    }
    
    memset(pchPage, 0x0, LINENUMS*LINECHARNUM);  

    pCurrMsg = pMessage;
    pCurrInPage = pchPage;
    while (*pCurrMsg != '\0')
    {
        if (NULL != (pCharFinded = index(pCurrMsg, '\n')))
        {
            k = pCharFinded-pCurrMsg+1;
            memcpy(pCurrInPage, pCurrMsg, k);
            pCurrMsg += k;
            pCurrInPage += k;
            nLineNum++;
        }
        else
        {
            k = strlen(pCurrMsg);
            memcpy(pCurrInPage, pCurrMsg, k);
            pCurrMsg += k;
            pCurrInPage += k;
            break;
        }

        if (nLineNum == (LINENUMS-1))
        {
            *pCurrInPage = '\0';
            vosPrintf(pstEnv->nWriteFd, "%s",pchPage);
            vosPrintf(pstEnv->nWriteFd, "--More--");
            memset(pchPage, 0x0, LINENUMS*LINECHARNUM);                  
      
            nLineNum = 0;
            while (*pCurrMsg != '\0')
            {
                /*wait user input*/
                chTmp = cliCharGet(pstEnv);
                
                switch (chTmp)
                {
                case CR:
                    usTemp = cliCharGet(pstEnv);
                    if (usTemp == NL || usTemp == 0xffff)
                    {
                        vosPrintf(pstEnv->nWriteFd, "\b\b\b\b\b\b\b\b"
                               "        "
                               "\b\b\b\b\b\b\b\b");
                        while (*pCurrMsg != '\0')
                        {
                            if (NULL != (pCharFinded = index(pCurrMsg, '\n')))
                            {
                                k = pCharFinded-pCurrMsg+1;
                                memcpy(pchPage, pCurrMsg, k);
                                pCurrMsg += k;                          
                                vosPrintf(pstEnv->nWriteFd, "%s",pchPage);
                                memset(pchPage, 0x0, LINENUMS*LINECHARNUM);  
                                vosPrintf(pstEnv->nWriteFd, "--More--");                                                 
                                break;
                            }
                            else
                            {
                                k = strlen(pCurrMsg);
                                memcpy(pchPage, pCurrMsg, k);
                                vosPrintf(pstEnv->nWriteFd, "%s",pchPage);
                                memset (pchPage, 0x0, (LINENUMS*LINECHARNUM));
                                /*vosPrintf(pstEnv->nWriteFd, "\r\r                        ---More---");*/
                                pCurrMsg += k;
                                pCurrInPage += k;
                                break;
                            }
                        }
                    }
                    break;
                case NL:  
                    vosPrintf(pstEnv->nWriteFd, "\b\b\b\b\b\b\b\b"
                           "        "
                           "\b\b\b\b\b\b\b\b");
                    while (*pCurrMsg != '\0')
                    {
                        if (NULL != (pCharFinded = index(pCurrMsg, '\n')))
                        {
                            k = pCharFinded-pCurrMsg+1;
                            memcpy(pchPage, pCurrMsg, k);
                            pCurrMsg += k;                          
                            vosPrintf(pstEnv->nWriteFd, "%s",pchPage);
                            memset(pchPage, 0x0, LINENUMS*LINECHARNUM);  
                            vosPrintf(pstEnv->nWriteFd, "--More--");                                                 
                            break;
                        }
                        else
                        {
                            k = strlen(pCurrMsg);
                            memcpy(pchPage, pCurrMsg, k);
                            vosPrintf(pstEnv->nWriteFd, "%s",pchPage);
                            memset (pchPage, 0x0, (LINENUMS*LINECHARNUM));
                            /*vosPrintf(pstEnv->nWriteFd, "\r\r                        ---More---");*/
                            pCurrMsg += k;
                            pCurrInPage += k;
                            break;
                        }
                    }
                    break;

                case BLANKSPACE:                    
                    vosPrintf(pstEnv->nWriteFd, "\b\b\b\b\b\b\b\b"
                           "        "
                           "\b\b\b\b\b\b\b\b");
                    nLineNum = 0;
                    pCurrInPage = pchPage;
                    while (*pCurrMsg != '\0')
                    {
                        if (NULL != (pCharFinded = index(pCurrMsg, '\n')))
                        {
                            k = pCharFinded-pCurrMsg+1;
                            memcpy(pCurrInPage, pCurrMsg, k);
                            pCurrMsg += k;
                            pCurrInPage += k;
                            nLineNum++;
                        }
                        else
                        {
                            k = strlen(pCurrMsg);
                            memcpy(pCurrInPage, pCurrMsg, k);
                            pCurrMsg += k;
                            pCurrInPage += k;
                            break;
                        }

                        if (nLineNum == (LINENUMS-1))
                        {
                            *pCurrInPage = '\0';
                            vosPrintf(pstEnv->nWriteFd, "%s",pchPage);
                            vosPrintf(pstEnv->nWriteFd, "--More--");
                            memset(pchPage, 0x0, LINENUMS*LINECHARNUM);
                            break;
                        }

                    }
                    break;
               
                case 'Q':
                case 'q':
                    {
                        vosFree (pchPage);   
                        vosPrintf(pstEnv->nWriteFd, "\b\b\b\b\b\b\b\b"
                               "        "
                               "\b\b\b\b\b\b\b\b"); 
                        return;                
                    }
                    break;
                }
            }
        }
         
    }
    vosPrintf(pstEnv->nWriteFd, "\b\b\b\b\b\b\b\b"
           "        "
           "\b\b\b\b\b\b\b\b");
    *pCurrInPage = '\0';
    vosPrintf(pstEnv->nWriteFd, "%s",pchPage);
    vosFree(pchPage);	
    return;
}



/*****************************************************************************
 *cliShowFree - free paging resource.
 *DESCRIPTION
 *This routine mainly to free paging resource.
 ****************************************************************************/
void cliShowFree(ENV_t *pstEnv)
{
    long bytes_read = 0;
    char aucMemPathName[STR_SIZE_64_BYTES] = {0};
    if (NULL == pstEnv)
    {
        return;
    }
	/*Whether close divide page*/
    if(DISABLE_PAGING == g_cliDividePage)
    {
    	 return;  
    }
    memFileNameGet(pstEnv, aucMemPathName);
#ifndef LINUX

    if (ERROR != pstEnv->page.nOldStdOutFd)
    {
        ioTaskStdSet(0, STD_OUT, pstEnv->page.nOldStdOutFd);
    }
    if (ERROR != pstEnv->page.nPageFd) 
    {
        close(pstEnv->page.nPageFd);                        
    }

    memDevDelete(aucMemPathName);

#else
    /* restore original standard output handle */
    if (ERROR != pstEnv->page.nOldStdOutFd)
    {
        dup2(pstEnv->page.nOldStdOutFd, pstEnv->nWriteFd);
        close(pstEnv->page.nOldStdOutFd);
    }
    /* close duplicate handle for STDOUT */
    if (ERROR != pstEnv->page.nPageFd) 
    {
        close(pstEnv->page.nPageFd);
    }
    vosRemove(aucMemPathName);
    
#endif 

    if (pstEnv->page.pPrivMem)
    {
        vosFree(pstEnv->page.pPrivMem);
        pstEnv->page.pPrivMem = NULL;
    }
    pstEnv->page.nPageFd = ERROR;
    pstEnv->page.nOldStdOutFd = ERROR;
}

