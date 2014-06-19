/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 39101 $
 * $Date: 2013-05-03 04:35:27 -0500 (Fri, 03 May 2013) $
 *
 * Purpose : Main function of OMCI Application
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) OMCI G.984.4
 *
 */



#include "app_basic.h"
#include "omci_defs.h"



BOOL gOmciProcOn    = TRUE;
BOOL gOmciLogEnable = FALSE;
int  gOmciLogLevel  = OMCI_LOG_LEVEL_LOW;


void omci_EnableLog(void);
void omci_DisableLog(void);





int main(int argc, char* argv[])
{
    GOS_ERROR_CODE ret;

    if(argc >= 2)
    {
        if(strcmp(argv[1], "-d" ) == 0)
        {
            omci_EnableLog();
        }
        else
        {
            omci_DisableLog();
        }
    }
    else
    {
        omci_DisableLog();
    }

    ret = OMCI_AppInit(OMCI_APPL, "OMCI");
    GOS_ASSERT(ret == GOS_OK);

    OMCI_AppStart((OMCI_APPL_INIT_PTR)OMCI_Init, (OMCI_APPL_MSG_HANDLER_PTR)OMCI_HandleMsg, (OMCI_APPL_DEINIT_PTR)OMCI_DeInit);

    return 0;
}


GOS_ERROR_CODE OMCI_Init(void)
{
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Start OMCI application initializing ... ");

    /* initialize MIB */
    OMCI_InitMib();

    /* initialize state machine for ME Multi-Actions */
    OMCI_InitStateMachines();


    /* initialize Gmac device */
    OMCI_createGmacDev();


    /* initialize message facility */
    OMCI_ResetHistoryRspMsg();
    OMCI_ResetLastRspMsgIndex();

    gOmciProcOn = TRUE;

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Start Gmac Agent ... \n");

    OMCI_SpawnTask("Gmac Agent", (OMCI_TASK_ENTRY)OMCI_waitGmacMsg,OMCI_TASK_PRI_MAIN);
	/*initial me operation work queue*/
	OMCI_MeOperCfgTaskInit();

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Finish OMCI application initializing ... \n");

    return GOS_OK;
}


GOS_ERROR_CODE OMCI_DeInit(void)
{
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Start OMCI application deinitializing ... \n");

    gOmciProcOn = FALSE;

    OMCI_deleteGmacDev();

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Finish OMCI application deinitializing ... \n");

    return GOS_OK;
}


void omci_EnableLog(void)
{
    gOmciLogEnable = TRUE;
}


void omci_DisableLog(void)
{
    gOmciLogEnable = FALSE;
}



