/*****************************************************************************
 *cli_command.c      Define Some common command interface function.                     
 ****************************************************************************/

#include <time.h>
#include "version.h"
#include "cli.h"
#include "cli_cmd_list.h"
#include "log.h"

#include "odm_port.h"
#include "lw_config.h"

extern const char * product_rev;
extern const char * build_date;
extern const char * build_time;
extern const char * product_info;
extern UINT8	oam_src_mac[6];

extern UINT32 odmUserRelogin(ENV_t *pstEnv);


STATUS cliCmdHelp(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    vosPrintf(pstEnv->nWriteFd, "\r\nHelp may be requested at any point in a command by entering");
    vosPrintf(pstEnv->nWriteFd, "\r\na question mark '?'. If nothing matches, the help list will");
    vosPrintf(pstEnv->nWriteFd, "\r\nbe empty and you must backup until entering a '?' shows the");
    vosPrintf(pstEnv->nWriteFd, "\r\navailable options.");
    vosPrintf(pstEnv->nWriteFd, "\r\nTwo styles of help are provided:");
    vosPrintf(pstEnv->nWriteFd, "\r\n1.Full help is available when you are ready to enter a");
    vosPrintf(pstEnv->nWriteFd, "\r\n  command argument (e.g. 'show ?') and describes each possible");
    vosPrintf(pstEnv->nWriteFd, "\r\n  arguments.");
    vosPrintf(pstEnv->nWriteFd, "\r\n2.Partial help is provided when an abbreviated argument is entered");
    vosPrintf(pstEnv->nWriteFd, "\r\n  and you want to know what arguments match the input");
    vosPrintf(pstEnv->nWriteFd, "\r\n  (e.g. 'show po?'.)\r\n");
    vosPrintf(pstEnv->nWriteFd, "\r\n");
    return OK;
}


STATUS  cliCmdDisable(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	if (NULL == pstEnv) 
	{
	    return ERROR;
	}

	pstEnv->pmode = CLI_MODE_USER;
	return OK;
}

STATUS  cliCmdEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    int  i = 0, accessLevel;
    int passwordValid = FALSE;
    enum { PASSWORD_LEN = 33 };
    char passWord[PASSWORD_LEN];
    int passwordLength = 0;
    unsigned short usInputChar;
	char *srcPassword = NULL;

    if (pstEnv == NULL) 
        return ERROR;

    if (CLI_MODE_ENABLE == pstEnv->pmode)
    	return OK;

	accessLevel = odmUserAccessLevelGet(pstEnv->pcUserName);
	if (accessLevel < ACCESS_LEVEL_ADMINISTRATION &&
		accessLevel > ACCESS_LEVEL_SUPER)
	{
		return ERROR;
	}

#if 0 /* dont need to check password again */
    srcPassword = odmUserPasswordGet(pstEnv->pcUserName);

    while(i++ < 3 && !passwordValid)
    {
        memset(passWord, 0, sizeof(passWord));
        usInputChar = 0;
        passwordLength = 0;
        
        vosPrintf(pstEnv->nWriteFd, "Password: ");
        
        while (passwordLength<PASSWORD_LEN)
        {
            usInputChar = cliCharGet(pstEnv);
            
            if (usInputChar==CR || usInputChar==NL )
                break;
                
            if (usInputChar == (0xff&usInputChar))
            {
                if (usInputChar==BSP)
                {
                    if (passwordLength>0)
                    {
                        /*cliEcho(pstEnv, BSP);*/
                        passwordLength--;
                    }
                }
                else
                {
                    /*cliEcho(pstEnv, '*');*/
                    passWord[passwordLength] = usInputChar;
                    passwordLength++;
                }
            }
        }
        
        vosPrintf(pstEnv->nWriteFd, "\r\n");

        passWord[passwordLength] = 0;
        if (passwordLength >= PASSWORD_LEN)
        {
            vosPrintf(pstEnv->nWriteFd, "password too long\r\n");
            continue;
        }

        if (0 == strcmp(passWord, srcPassword))
            passwordValid = TRUE;
        
    }

    if (TRUE != passwordValid) 
    {
        vosPrintf(pstEnv->nWriteFd, "%% Bad password\r\n");
        return ERROR;
    }
#endif

    vosPrintf(pstEnv->nWriteFd, "\r\n");
    pstEnv->pmode = CLI_MODE_ENABLE;

	return OK;

}

STATUS  cliCmdQuit(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    if (NULL == pstEnv) 
        return ERROR;

    pstEnv->pmode = 0;
    pstEnv->used = 0;
    pstEnv->close = CLI_CLOSE_ACTIVE;
    pstEnv->err = TERMINAL_IO_FAULT;
        
    return OK;
}


STATUS  cliCmdExiter(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    if (NULL == pstEnv) 
        return ERROR;

    if (TERMINAL_MODE_NORMAL == pstEnv->tmode && pstEnv->pmode == CLI_MODE_USER)
    {
        pstEnv->phase = CLI_CMD_PHASE_EXIT;
        return OK;
    }

    if ((pstEnv->pmode == CLI_MODE_CONFIG_SYS) 
        || (pstEnv->pmode == CLI_MODE_CONFIG_USER) 
        || (pstEnv->pmode == CLI_MODE_CONFIG_QOS) 
        || (pstEnv->pmode == CLI_MODE_CONFIG_FDB) 
        || (pstEnv->pmode == CLI_MODE_CONFIG_MCAST)
        || (pstEnv->pmode == CLI_MODE_CONFIG_VLAN)
        || (pstEnv->pmode == CLI_MODE_CONFIG_PORT)
        || (pstEnv->pmode == CLI_MODE_CONFIG_RSTP) 
        || (pstEnv->pmode == CLI_MODE_CONFIG_PON) 
        || (pstEnv->pmode == CLI_MODE_CONFIG_STORM) 
        || (pstEnv->pmode == CLI_MODE_CONFIG_MIRROR) 
        || (pstEnv->pmode == CLI_MODE_SUPER)
        )
    {
        pstEnv->pmode = CLI_MODE_ENABLE;
    }
    else if (pstEnv->pmode == CLI_MODE_CONFIG_QOS_CLASS)
    {
        pstEnv->pmode = CLI_MODE_CONFIG_QOS;
    }
    else if (pstEnv->pmode == CLI_MODE_ENABLE)
    {
        pstEnv->pmode = CLI_MODE_USER;
    }
    else if (pstEnv->pmode == CLI_MODE_USER)
    {
        pstEnv->pmode = 0;
        pstEnv->used = 0;
        pstEnv->close = CLI_CLOSE_ACTIVE;
        pstEnv->err = TERMINAL_IO_FAULT;
    }
    else
    {
        pstEnv->pmode = CLI_MODE_USER;
    }
        
    return OK;
}

STATUS cliCmdShowHistory(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    int nFirstIndex = 0;
    int nLastIndex = 0;
    unsigned int  unMode = 0;
    CMD_HISTORY_t *psCmdHistory = NULL;
	
    if (NULL == pstEnv)
        return ERROR;

    psCmdHistory = pstEnv->configureHistoryCommand;	        
    if (0 == psCmdHistory->tail && 0 == psCmdHistory->head && 0 == psCmdHistory->pos) 
        return OK;

    nFirstIndex = psCmdHistory->head;
    nLastIndex = psCmdHistory->tail;
    while (nFirstIndex != nLastIndex)
    {
        vosPrintf(pstEnv->nWriteFd, "  %s\r\n",psCmdHistory->cmd[nLastIndex]);
        nLastIndex--;
        if (nLastIndex<0)
            nLastIndex = MAX_HISTORY_CMD_NUMBER-1;
    }
    /* History is Empty? */
    if (!(0 == psCmdHistory->head && 0 == psCmdHistory->tail && 0 == psCmdHistory->pos)) 
        vosPrintf(pstEnv->nWriteFd, "  %s\r\n",psCmdHistory->cmd[nFirstIndex]);

    return OK;
}

void cliPrintVersion(ENV_t *pstEnv, int nVerbose)
{
  int nYear,nDay;
  char acMonth[5] = {0};
  time_t timep;
  struct tm *pNow;
  if (nVerbose) {
    time(&timep);
    pNow = localtime(&timep);
    sscanf(build_date, "%s %d %d", acMonth, &nDay, &nYear);
    if (pNow->tm_year + 1900 > nYear)
        nYear = pNow->tm_year + 1900;
    vosPrintf(pstEnv->nWriteFd,
      " \r\n"
      " \r\n"
      " \r\n"
      "                        ___ \r\n"
      "                  ____      ___\r\n"
      "                ____         ___\r\n"
      "              ____            ___                ]]\\\r\n"
      "             ____             ____                ]\\\r\n"
      "            ____              ]]]]]]\\  ]]\\ ]]\\    ]\\    ]]]]\\   ]]\\]]\\\r\n"
      "            ____              _]\\_  ]\\  ]\\  ]\\    ]\\        ]\\   ]]\\ ]\\\r\n"
      "            ____              _]\\_  ]\\  ]\\  ]\\    ]\\    ]]]]]\\   ]\\ ]\\\r\n"
      "            ____             __]\\   ]\\  ]\\  ]\\    ]\\   ]\\   ]\\   ]\\ ]\\\r\n"
      "             ___            ___]\\   ]\\  ]\\ ]]\\    ]\\   ]\\   ]\\   ]\\ ]\\\r\n"
      "              ___          ___ ]]]]]\\    ]]\\]]\\ ]]]]]\\  ]]]]\\]\\ ]]]\\]]]\\\r\n"
      "               ____     ___    ]\\\r\n"
      "                   ___        ]]]]\\\r\n"
      " \r\n"
      " \r\n"
      " \r\n"
      "        Opulan Technologies Corp.\r\n"
      "        Semiconductor for Broadband Connectivity\r\n"
      "        Copyright (C) 2003 - %d\r\n"
      " \r\n"
      "        Product  : %s \r\n"
      "        Release  : %d.%d.%d (%s)\r\n"
      "        Revision : %s\r\n"
      "        Build    : %s, %s \r\n"
      " \r\n"
      "        MAC     : %02x:%02x:%02x:%02x:%02x:%02x\r\n"
      " \r\n",
      nYear,
      product_info,
      MAJOR_VERSION,
      MINOR_VERSION,
      BUILD_NUMBER,
      BUILD_FLAG,
      product_rev,
      build_time,
      build_date,
      oam_src_mac[0],
      oam_src_mac[1],
      oam_src_mac[2],
      oam_src_mac[3],
      oam_src_mac[4],
      oam_src_mac[5]);
  }
  else {
    vosPrintf(pstEnv->nWriteFd,
      "Software version: %d.%d.%d (%s)\r\n"
      "Revision: %s\r\n"
      "Build time: %s, %s\r\n",
      MAJOR_VERSION,
      MINOR_VERSION,
      BUILD_NUMBER,
      BUILD_FLAG,
      product_rev,
      build_time,
      build_date);
  }
}

STATUS cliCmdShowVersion(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    if (NULL == pstEnv)
    {
        return ERROR;
    }
    
    cliPrintVersion(pstEnv, FALSE);

    return OK;
}

STATUS cliCmdReset(ENV_t *pstEnv,PARA_TABLE_t *pt)
{
    int reset = FALSE;
    enum { BUF_LEN = 5 };
    char answer[BUF_LEN] = {0};
    int answerLength = 0;
    unsigned short usInputChar;

    if (NULL == pstEnv)
    {
        return ERROR;
    }

    vosPrintf(pstEnv->nWriteFd, "Reboot system now? (yes or no) ");    

    while (answerLength<BUF_LEN)
    {
        usInputChar = cliCharGet(pstEnv);
        
        if (usInputChar==CR || usInputChar==NL )
            break;
            
        if (usInputChar == (0xff&usInputChar))
        {
            if (usInputChar==BSP)
            {
                if (answerLength>0)
                {
                    cliEcho(pstEnv, BSP);
                    answerLength--;
                }
            }
            else
            {
                cliEcho(pstEnv, usInputChar);
                answer[answerLength] = usInputChar;
                answerLength++;
            }
        }
    }
    
    vosPrintf(pstEnv->nWriteFd, "\r\n");

    answer[answerLength] = 0;
    if (answerLength >= BUF_LEN)
    {
        vosPrintf(pstEnv->nWriteFd, "Invalid input\r\n");
        return ERROR;
    }

    if (TRUE != cliIsInputYes(answer) && TRUE != cliIsInputNo(answer))
    {
        vosPrintf(pstEnv->nWriteFd, "Invalid input\r\n");
        return ERROR;
    }

    if (TRUE == cliIsInputYes(answer)) 
        reset = TRUE;
        
    vosPrintf(pstEnv->nWriteFd, "\r\n");

    if (TRUE == reset)
    {
        odmSysReset();
    }
    
    return OK;
}

STATUS cliCmdSystem(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
	
	pstEnv->pmode = CLI_MODE_CONFIG_SYS;
    vosPrintf(pstEnv->nWriteFd, "Enter system configuration mode. \r\n");
	
	return OK;
}


STATUS cliCmdUser(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
	
	pstEnv->pmode = CLI_MODE_CONFIG_USER;
    vosPrintf(pstEnv->nWriteFd, "Enter user configuration mode. \r\n");
	
	return OK;
}

STATUS cliCmdQoS(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
	
	pstEnv->pmode = CLI_MODE_CONFIG_QOS;
    vosPrintf(pstEnv->nWriteFd, "Enter QoS configuration mode. \r\n");
	
	return OK;
}
STATUS cliCmdClearAll(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{

	int flag=0;
	if (NULL == pstEnv)
	{
	return ERROR;
	}
	if(psPara[0].i)
	flag=1;
	
	if(!flag)	
  		odmOamClassRemoveAll(0);
	 else 	
	 	odmOamClassRemoveAll(0);
  	return OK;
}
STATUS cliCmdFDB(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
	
	pstEnv->pmode = CLI_MODE_CONFIG_FDB;
    vosPrintf(pstEnv->nWriteFd, "Enter FDB configuration mode. \r\n");
	
	return OK;
}

STATUS cliCmdMcast(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
	
	pstEnv->pmode = CLI_MODE_CONFIG_MCAST;
    vosPrintf(pstEnv->nWriteFd, "Enter multicast configuration mode. \r\n");
	
	return OK;
}

STATUS cliCmdVLAN(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
	
	pstEnv->pmode = CLI_MODE_CONFIG_VLAN;
    vosPrintf(pstEnv->nWriteFd, "Enter VLAN configuration mode. \r\n");
	
	return OK;
}

STATUS cliCmdPort(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal;
	UINT8 portListBuff[255];
	UINT32 portArry[ODM_NUM_OF_PORTS+ 1];

	vosMemSet(&portListBuff[0],0x00,255);
	vosMemSet(&portArry[0],0X00,(ODM_NUM_OF_PORTS+1)*4);
	
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
    if (NULL == psPara->p)
    {
        return ERROR;
    }

	vosStrCpy(portListBuff,psPara->p);

	retVal = convertPortListString2PortArry(portListBuff,portArry,ODM_NUM_OF_PORTS+1);	
	if(ERROR == retVal)
	{
#if defined(ONU_1PORT)
  		vosPrintf(pstEnv->nWriteFd,"port list input error,port list value range should be <1>\r\n");
#else
  		vosPrintf(pstEnv->nWriteFd,"port list input error,port list value range should be <1-4>\r\n");
#endif
		return retVal;
	}else
	{
		if(portArry[0] == 1)  
		{
#if defined(ONU_1PORT)
			vosPrintf(pstEnv->nWriteFd,"portId start at 1.\r\n");
#else
			vosPrintf(pstEnv->nWriteFd,"portId start at 1-4.\r\n");
#endif
			return ERROR;
		}
	}
	
    /* you must check if the pstEnv->para is freed */
    if (NULL != pstEnv->para)
    {
        vosFree(pstEnv->para);
        pstEnv->para = NULL;
    }
		
    pstEnv->para = (void *)vosStrDup(psPara->p);
	
	pstEnv->pmode = CLI_MODE_CONFIG_PORT;
    vosPrintf(pstEnv->nWriteFd, "Enter port configuration mode. \r\n");
	
	return OK;
}

STATUS cliCmdRSTP(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
	
	pstEnv->pmode = CLI_MODE_CONFIG_RSTP;
    vosPrintf(pstEnv->nWriteFd, "Enter RSTP configuration mode. \r\n");
	
	return OK;
}

STATUS cliCmdPon(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
	
	pstEnv->pmode = CLI_MODE_CONFIG_PON;
    vosPrintf(pstEnv->nWriteFd, "Enter PON configuration mode. \r\n");
	
	return OK;
}

STATUS cliCmdStorm(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
	
	pstEnv->pmode = CLI_MODE_CONFIG_STORM;
    vosPrintf(pstEnv->nWriteFd, "Enter storm configuration mode. \r\n");
	
	return OK;
}


STATUS  cliCmdSuper(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    int  i = 0, accessLevel;
    int passwordValid = FALSE;
    enum { PASSWORD_LEN = 33 };
    char passWord[PASSWORD_LEN];
    int passwordLength = 0;
    unsigned short usInputChar;
    char* srcPassword = NULL;

    if (pstEnv == NULL) 
        return ERROR;

    if (CLI_MODE_SUPER == pstEnv->pmode)
    	return OK;	


	accessLevel = odmUserAccessLevelGet(pstEnv->pcUserName);
	if (accessLevel != ACCESS_LEVEL_SUPER)
	{
		return ERROR;
	}

#if 0 /* dont need to check password again */
    srcPassword = odmUserPasswordGet(pstEnv->pcUserName);

    while(i++ < 3 && !passwordValid)
    {
        memset(passWord, 0, sizeof(passWord));
        usInputChar = 0;
        passwordLength = 0;
        
        vosPrintf(pstEnv->nWriteFd, "Password: ");
        
        while (passwordLength<PASSWORD_LEN)
        {
            usInputChar = cliCharGet(pstEnv);
            
            if (usInputChar==CR || usInputChar==NL )
                break;
                
            if (usInputChar == (0xff&usInputChar))
            {
                if (usInputChar==BSP)
                {
                    if (passwordLength>0)
                    {
                        /*cliEcho(pstEnv, BSP);*/
                        passwordLength--;
                    }
                }
                else
                {
                    /*cliEcho(pstEnv, '*');*/
                    passWord[passwordLength] = usInputChar;
                    passwordLength++;
                }
            }
        }
        
        vosPrintf(pstEnv->nWriteFd, "\r\n");

        passWord[passwordLength] = 0;
        if (passwordLength >= PASSWORD_LEN)
        {
            vosPrintf(pstEnv->nWriteFd, "password too long\r\n");
            continue;
        }

        if (0 == strcmp(passWord, srcPassword))   
            passwordValid = TRUE;
        
    }

    if (TRUE != passwordValid) 
    {
        vosPrintf(pstEnv->nWriteFd, "%% Bad password\r\n");
        return ERROR;
    }
#endif

    vosPrintf(pstEnv->nWriteFd, "\r\n");
    pstEnv->pmode = CLI_MODE_SUPER;

	return OK;
}

STATUS  cliCmdLogin(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	if(0)
	{
		printf("pstpara: %p\n", psPara);
	}

    if (pstEnv == NULL)
        return ERROR;

	return odmUserRelogin(pstEnv);
}

STATUS cliCmdQinQ(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
	
	//pstEnv->pmode = CLI_MODE_CONFIG_QINQ;
    vosPrintf(pstEnv->nWriteFd, "Enter QinQ configuration mode. \r\n");
	
	return OK;
}
#ifdef CONFIG_BOSA
STATUS cliCmd25L90iModSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 uiModVal = 0;
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
	uiModVal = psPara[0].u;
	if(NO_ERROR != DRV_25L90ImodSet(uiModVal))
	{
        vosPrintf(pstEnv->nWriteFd, "Set 25L90 register IMODSET faild. \r\n");
    }
    
	return OK;
}
STATUS cliCmd25L90ApcSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 uiApcVal = 0;
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
	uiApcVal = psPara[0].u;
	if(NO_ERROR != DRV_25L90ApcSet(uiApcVal))
	{
        vosPrintf(pstEnv->nWriteFd, "Set 25L90 5L90 register APCSET faild. \r\n");
    }
    
	return OK;
}
#endif

