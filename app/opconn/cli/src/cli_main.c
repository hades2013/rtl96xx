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
* FILENAME:  cli_main.c
*
* DESCRIPTION: 
*	cli main entry
*
* Date Created: Nov 05, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_main.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <vos.h>
#include "cli.h"

extern int cliCmdCountGet(void);
extern MODEL_INTF_INFO_t g_asCliCmdTable[];

ENV_t        *g_pstConsole = NULL;
CMD_KEY_PARA_LINK_t *g_psRootOfForest = NULL; /* command tree root */       
unsigned int *g_punArrayOfCliMode;  /* cli mode array */
CMD_KEY_PARA_LINK_t *g_asArrayOfCmdTreeTemp[NUM_OF_CLI_MODE];
CMD_KEY_PARA_LINK_t **g_asArrayOfCmdTree;

unsigned int g_aunArrayOfCliModeTemp[NUM_OF_CLI_MODE] = 
{
    CLI_MODE_USER,
    CLI_MODE_ENABLE,
    CLI_MODE_CONFIG_SYS,
    CLI_MODE_CONFIG_USER,
    CLI_MODE_CONFIG_QOS,
    CLI_MODE_CONFIG_FDB,
    CLI_MODE_CONFIG_MCAST,
    CLI_MODE_CONFIG_VLAN,
    CLI_MODE_CONFIG_PORT,
    CLI_MODE_CONFIG_RSTP,
    CLI_MODE_CONFIG_PON,
    CLI_MODE_CONFIG_STORM,
    CLI_MODE_CONFIG_QOS_CLASS,
    CLI_MODE_CONFIG_MIRROR,
    CLI_MODE_SUPER
};

/*******************************************************************************
*
* cliInit:	 Initialize CLI.
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	n/a.
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void cliInit(void)
{
    int    i;
    int    nCmdNums = 0;

    vosThreadMainSet(vosThreadSelf());

#if 0  /* deleted by Gan Zhiheng - 2011/05/27, this feature just for testing */
	/*fixed bug 3290, close icrnl */
	system("stty -icrnl -F /dev/ttyS0");
#endif /* #if 0 */

    g_pstConsole = cliSessionAdd(vosThreadSelf(), TRUE);
    if (!g_pstConsole) 
    {
        return;
    }

    g_pstConsole->nReadFd    = STDIN_FILENO;
    g_pstConsole->nWriteFd   = STDOUT_FILENO;
    g_pstConsole->nErrFd     = STDERR_FILENO;

    /* set default terminal size */
    g_pstConsole->ulTermWidth = 80;
    g_pstConsole->ulTermHeight = 25;
    g_pstConsole->tmode = TERMINAL_MODE_NORMAL;

    /* add debug info temporarily */
    //cliSessionDebugLevelSet(g_pstConsole->ucIndex, MOD_OAM, DEBUG_LEVEL_DEBUGGING);
    //cliSessionDebugLevelSet(g_pstConsole->ucIndex, MOD_PON, DEBUG_LEVEL_DEBUGGING);
    //cliSessionDebugLevelSet(g_pstConsole->ucIndex, MOD_PORT, DEBUG_LEVEL_DEBUGGING);

    /* To surport show more in vxWorks */
#ifndef LINUX
    if (ERROR == memDrv())
    {   
        vosPrintf(g_pstConsole->nWriteFd, "CLI: MultiPage display failed.\r\n"); 
    	return;
    }
#endif /* #ifndef LINUX */

    g_punArrayOfCliMode = g_aunArrayOfCliModeTemp;
    g_asArrayOfCmdTree = g_asArrayOfCmdTreeTemp;
    
    nCmdNums = cliCmdCountGet();
    if (cliForestCreate(g_asCliCmdTable, nCmdNums, g_asArrayOfCmdTree,&g_psRootOfForest) != OK)
    {   
        vosPrintf(g_pstConsole->nWriteFd, "CLI: Command tree initialization failed.\r\n");
    	return;
    }

}

/*******************************************************************************
*
* cliMainEntry:	 Main entry of CLI module
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	n/a.
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
void cliMainEntry(void)
{
    if (g_pstConsole) 
    {
     //   cliPrintVersion(g_pstConsole, TRUE);/*modified by huangmingjian 2012/09/07 for EPN104QID0018*/  
        cliCmdProcess(g_pstConsole);			/* which is the main loop */
    }
}

