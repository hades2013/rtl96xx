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
* FILENAME:  cli_cmd_system.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_cmd_system.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "cli.h"
#include "cli_cmd_list.h"
#include "defs.h"
#include "opl_driver.h"
#include "system.h"

extern int g_cliDividePage;

extern int cliTextConfigLoad(ENV_t *pstEnv, char *filename);

STATUS cliCmdSystemIpAddrSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    ODM_SYS_IF_CONFIG_t stIpConfig;

	if (NULL == pstEnv || NULL == pstPara) 
	{
		return ERROR;
	}

    vosMemSet(&stIpConfig, 0, sizeof(ODM_SYS_IF_CONFIG_t));

    /*
    interface {wan|lan} {share|{independent {admin|signal|media}}}  \
     {{static [ip-address ipaddr net-mask mask gateway gwaddr dns dnsaddr]}|\
     dhcp|{pppoe user username password psword auth {auto|pap|chap}}} \
     [qos {{dscp|tos} qos-value}] \
     [tag-mode {transparent|{tag cvlan cvlan1 priority pri1}|\
     {vlan-stacking cvlan cvlan2 priority pri2 svlan svlan2}}] 
    */

    stIpConfig.ucIf = (UINT8)(pstPara[0].u - 1);
    stIpConfig.ucIfShareMode = (UINT8)(pstPara[1].u - 1);
    if (stIpConfig.ucIfShareMode == INTERFACE_SHARE_MODE_IND)
    {
        stIpConfig.ucIfMode = (UINT8)(pstPara[3].u - 1);
    }
    stIpConfig.ucIpMode = (UINT8)(pstPara[4].u - 1);

    switch (stIpConfig.ucIpMode)
    {
        case IF_IP_MODE_STATIC:
            if (pstPara[6].u > 0)
            {
                stIpConfig.ulIpAddr = pstPara[7].u;
                stIpConfig.ulIpMask = pstPara[8].u;
                stIpConfig.ulGwAddr = pstPara[9].u;
                stIpConfig.ulDnsAddr = pstPara[10].u;
            }
            break;
        case IF_IP_MODE_PPPOE:
            vosStrCpy(stIpConfig.acPPPoEUser, pstPara[12].p);
            vosStrCpy(stIpConfig.acPPPoEPass, pstPara[13].p);
            stIpConfig.ucPPPoEAuthType = (UINT8)(pstPara[14].u-1);
            break;
        default:
            break;
    }

    if (pstPara[15].u > 0)
    {
        stIpConfig.ucQoSMode = (UINT8)(pstPara[17].u - 1);
        stIpConfig.ucQoSVal = (UINT8)(pstPara[18].u);
    }

    if (pstPara[19].u > 0)
    {
        stIpConfig.ucTagged = (UINT8)(pstPara[20].u - 1);
        switch (stIpConfig.ucTagged)
        {
            case IF_TAG_MODE_TAG:
                stIpConfig.usCVlan = pstPara[22].u;
                stIpConfig.ucPriority = pstPara[23].u;
                break;
            case IF_TAG_MODE_VLAN_STACKING:
                //vosPrintf(pstEnv->nWriteFd, "%VLAN stacking does not supported.\r\n");
                stIpConfig.usCVlan = pstPara[25].u;
                stIpConfig.ucPriority = pstPara[26].u;
				stIpConfig.usSVLAN = pstPara[27].u;
                return ERROR;
            default:
                break;
        }
    }

    if (NO_ERROR != odmSysIfConfigSet(&stIpConfig))
    {
        vosPrintf(pstEnv->nWriteFd, "Configure interface failed\r\n");
        return ERROR;
    }

    return OK;
}

STATUS cliCmdSystemTimeSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    char            acOldDate[20] = {0};
    char            *pcTmpDate;
    char            *pcTmpTime;

	if (NULL == pstEnv || NULL == pstPara) 
	{
		return ERROR;
	}

    pcTmpDate = pstPara[0].p;
    pcTmpTime = pstPara[1].p;

    odmSysCfgDateGet(acOldDate);

    if (NO_ERROR != odmSysCfgDateSet(pcTmpDate, NULL))
    {
        vosPrintf(pstEnv->nWriteFd, "Set system date failed\r\n");
        return ERROR;
    }

    if (NO_ERROR != odmSysCfgTimeSet(pcTmpTime, NULL))
    {
        vosPrintf(pstEnv->nWriteFd, "Set system time failed\r\n");
        /* restore the old date here */
        odmSysCfgDateSet(acOldDate, NULL);
        return ERROR;
    }

    return OK;
}

STATUS cliCmdSystemHostNameSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	if (NULL == pstEnv || NULL == pstPara) 
	{
		return ERROR;
	}

    if (NO_ERROR != odmSysCfgNameSet(pstPara->p, NULL))
    {
        vosPrintf(pstEnv->nWriteFd, "Set host name failed\r\n");
        return ERROR;
    }

    return OK;
}


STATUS cliCmdSystemLocationSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	if (NULL == pstEnv || NULL == pstPara) 
	{
		return ERROR;
	}

    if (NO_ERROR != odmSysCfgLocationSet(pstPara->p, NULL))
    {
        vosPrintf(pstEnv->nWriteFd, "Set host location failed\r\n");
        return ERROR;
    }
	
	return OK;
}

STATUS cliCmdSystemTimeout(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	if (NULL == pstEnv || NULL == pstPara) 
	{
		return ERROR;
	}

    if (NO_ERROR != odmSysCfgSessionTimeoutSet(pstPara->u, NULL))
    {
        vosPrintf(pstEnv->nWriteFd, "Set host location failed\r\n");
        return ERROR;
    }
	
	return OK;
}



STATUS cliCmdSystemCPUPathMtu(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	if (NULL == pstEnv || NULL == pstPara) 
	{
		return ERROR;
	}

	if (NO_ERROR != odmSysCPUPathMtuSet(pstPara->u))
	{
		vosPrintf(pstEnv->nWriteFd, "Set System Mtu failed\r\n");
		return ERROR;
	}

	return OK;
}

STATUS cliCmdSystemKillSession(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    UINT32 ret = NO_ERROR;
	if (NULL == pstEnv || NULL == pstPara) 
	{
		return ERROR;
	}
    
    switch (pstPara[0].i)
    {
    case 1:
        ret = cliSessionCloseByIndex(pstEnv, pstPara[2].u);
        switch (ret)
        {
            case NO_ERROR:
                /*vosPrintf(pstEnv->nWriteFd, "Session close seccussfully.\r\n", pstPara[2].u); */
                break;
            case SYS_SESSION_INVALID_ID:
                vosPrintf(pstEnv->nWriteFd, "%% Invalid session number (%d).\r\n", pstPara[2].u);
                break;
            case SYS_SESSION_CAN_NOT_KILL_YOURSELF:
                vosPrintf(pstEnv->nWriteFd, "%% Can not kill yourself.\r\n");
                break;
            default:
                break;
        }
        break;

    case 2:
        cliTelnetdShutdown();
        cliTelnetdInit();
        break;

    default:
        return (ERROR);
    }

	return OK;
}

STATUS cliCmdSystemShow(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
    cliShowStart(pstEnv);
    odmSysInfoShow(pstEnv);
    cliShowEnd(pstEnv);
	return OK;
}
/*****************************************************************************
 *cliCmdSystemDividepage - Divide Page.
 *DESCRIPTION
 *This routine mainly to determine whether divide page
 ****************************************************************************/
STATUS cliCmdSystemDividepage(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	if(NULL == pstEnv) 
	{
		return ERROR;
	}
	if (1 == pstPara[0].i)
    {
        g_cliDividePage= OPL_TRUE;
    }
    else
    {
        g_cliDividePage= OPL_FALSE;
    }
	return OK;
}

STATUS cliCmdSystemShowCfg(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
    cliShowStart(pstEnv);
    odmSysCfgShow(pstEnv);
    cliShowEnd(pstEnv);
	
	return OK;
}

STATUS cliCmdSystemSave(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    UINT32 configLen = 0;
	if (NULL == pstEnv) 
	{
		return ERROR;
	}

    vosPrintf(pstEnv->nWriteFd, "Save system configuration ... ");
    cliTextConfigSave();
    vosConfigSave(NULL);
    configLen = vosConfigFileLengthGet();
	vosPrintf(pstEnv->nWriteFd, "wrote %lu bytes to flash\r\n", configLen);
    vosPrintf(pstEnv->nWriteFd, "done\r\n\r\n");

	return OK;
}

STATUS cliCmdSystemShowCfgAll(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
    cliShowStart(pstEnv);
    vosConfigShow(pstEnv->nWriteFd);
    cliShowEnd(pstEnv);
	
	return OK;
}

STATUS cliCmdSystemErase(ENV_t *pstEnv,PARA_TABLE_t *pstPara)
{
    int erase = FALSE;
    char answer[5] = {0};
    int answerLength = 0;
    unsigned short usInputChar;

    if (NULL == pstEnv)
    {
        return ERROR;
    }

    vosPrintf(pstEnv->nWriteFd, "Erase system conifiguration? (yes or no)");    

    while (answerLength<sizeof(answer))
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
    if (answerLength >= sizeof(answer))
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
        erase = TRUE;
        
    vosPrintf(pstEnv->nWriteFd, "\r\n");

    if (TRUE == erase)
    {
        vosConfigErase();
        odmUserCfgInit();
    }
    
    return OK;
}

STATUS cliCmdSystemLoadScript(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
	if (NULL == pstEnv || NULL == pstPara) 
	{
		return ERROR;
	}

    while (cliTextConfigIsLoading(pstEnv))
    {
        /* waiting for loading */
    }

    if (OK != cliTextConfigLoad(pstEnv, pstPara->p))
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nLoad configuration script [%s] failed.\r\n\r\n", pstPara->p);
        return ERROR;
    }

    vosPrintf(pstEnv->nWriteFd, "\r\nLoad configuration script [%s] done.\r\n\r\n", pstPara->p);
	return OK;
}

#ifdef OPL_COM_UART1
STATUS cliCmdSystemUart1Com(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;
    UINT8 enable = FALSE;
    
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
    switch (psPara->i)
    {
        case 1:
            odmOnuUartCommEnableGet(&enable);
            if (FALSE == enable) return OK;
            odmOnuUartCommEnableSet(0);
            odmOnuUart1CommThreadShutdowm();
            break;
        case 2:
            odmOnuUartCommEnableGet(&enable);
            if (TRUE == enable) return OK;
            odmOnuUartCommEnableSet(1);
            odmOnuUart1CommThreadInit();
            break;
        default:
            break;
    }
 
	return OK;
}
#endif

STATUS cliCmdSystemProcess(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int fd, rlt = -1;
    UINT8 bootFlag;

    const char *pacFileTypeString[] = {
        "Firmware", "Startup-config"
    };
    const char *pacOptTypeString[] = {
        "downlown", "upload"
    };

    if (NULL == pstEnv)
    {
        return ERROR;
    }

    if (!pstEnv->ucIsConsole)
    {
        vosPrintf(pstEnv->nWriteFd, "%%XModem only works on console.\r\n");
        return ERROR;
    }

    if (pstPt[0].i > 2)
    {
        vosPrintf(pstEnv->nWriteFd, "%%Invalid file type input.\r\n");
        return ERROR;
    }

    if (pstPt[1].i > 2)
    {
        vosPrintf(pstEnv->nWriteFd, "%%Invalid operation type input.\r\n");
        return ERROR;
    }

    vosPrintf(pstEnv->nWriteFd, "%s %s starting ... ",
        pacFileTypeString[pstPt[0].i-1], pacOptTypeString[pstPt[1].i-1]);

    switch (pstPt[0].i)
    {
        case 1: /* firmware */

            switch (pstPt[1].i)
            {
                case 1: /* download */

                    fd = open("/dev/ufile", O_RDONLY);
                    if (fd < 0)
                    {
                        vosPrintf(pstEnv->nWriteFd, "Can not open device (/dev/ufile)!!!\r\n");
                        return ERROR;
                    }

                    ioctl(fd, 2, &rlt);

                    if (rlt != OK)
                    {
                        printf("failed.\r\n\r\n");
                        return ERROR;
                    }

					if (check_image("/tmp/opconn.img.bak") != OK)
					{
                        printf("image file is invalid, download failed.\r\n\r\n");
                        return ERROR;						
					}

                    bootFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_NORMAL);
                    if (FLASH_BOOT_OS2_FLAG == bootFlag)
                    {
                        write_flash("/tmp/opconn.img.bak", FLASH_DEV_NAME_OS1);
						
                        vosSystem("rm -f /tmp/opconn.img.bak ");
    					vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS1_FLAG);
                    }
                    else {
                        write_flash("/tmp/opconn.img.bak", FLASH_DEV_NAME_OS2);

                        vosSystem("rm -f /tmp/opconn.img.bak ");
    					vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS2_FLAG);
                    }

                    vosPrintf(pstEnv->nWriteFd, "done.\r\n\r\n");

                    close(fd);

                    break;

                case 2: /* upload */
					fd = open("/dev/ufile", O_RDONLY);
                    if (fd < 0)
                    {
                        vosPrintf(pstEnv->nWriteFd, "Can not open device (/dev/ufile)!!!\r\n");
                        return ERROR;
                    }

                    ioctl(fd, 3, &rlt);

                    if (rlt != OK)
                    {
                        printf("failed.\r\n\r\n");
                        return ERROR;
                    }

                    close(fd);

                    vosPrintf(pstEnv->nWriteFd, "done.\r\n\r\n");
                    break;

                default:
                    break;

            }
            break;

        case 2: /* startup-config */

            switch (pstPt[1].i)
            {
                case 1: /* download */

                    fd = open("/dev/ufile", O_RDONLY);
                    if (fd < 0)
                    {
                        vosPrintf(pstEnv->nWriteFd, "Can not open device (/dev/ufile)!!!\r\n");
                        return ERROR;
                    }

                    ioctl(fd, 4, &rlt);

                    if (rlt != OK)
                    {
                        printf("failed.\r\n\r\n");
                        return ERROR;
                    }

                    /* config download notify to voip */
                    vosConfigSave("/tmp/cfg.tar.gz");

                    vosPrintf(pstEnv->nWriteFd, "done.\r\n\r\n");

                    close(fd);

                    break;

                case 2: /* upload */

                    /* dump config to /tmp/cfg.tar.gz */
                    vosConfigReadFromFlash("/tmp/cfg.tar.gz");

					fd = open("/dev/ufile", O_RDONLY);
                    if (fd < 0)
                    {
                        vosPrintf(pstEnv->nWriteFd, "Can not open device (/dev/ufile)!!!\r\n");
                        return ERROR;
                    }

                    ioctl(fd, 5, &rlt);

                    if (rlt != OK)
                    {
                        printf("failed.\r\n\r\n");
                        return ERROR;
                    }

                    close(fd);

                    vosPrintf(pstEnv->nWriteFd, "done.\r\n\r\n");
                    break;

                default:
                    break;

            }
            break;

        default:
            break;
    }
    return OK;
}


STATUS cliCmdSystemFirmwareSwitch(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT8 bootFlag;

    if (NULL == pstEnv)
    {
        return ERROR;
    }

    bootFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_NORMAL);
    if (FLASH_BOOT_OS2_FLAG == bootFlag)
    {
		vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS1_FLAG);
    }
    else {
		vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS2_FLAG);
    }

    return OK;
}

STATUS cliCmdSystemMacAddrSet(ENV_t *pstEnv, PARA_TABLE_t *pstPara)
{
    char acMac[6];
    UINT8 ucIf, ucIfMode;

	if (NULL == pstEnv || NULL == pstPara) 
	{
		return ERROR;
	}

    vosMemSet(acMac, 0, sizeof(acMac));

    /*interface {wan|lan} {admin|signal|media} mac mac-addr*/

    ucIf = (UINT8)(pstPara[0].u - 1);
    ucIfMode = (UINT8)(pstPara[1].u - 1);

    if (utilStrToMacAddr(pstPara[2].p,acMac) != OK)
    {
        vosPrintf(pstEnv->nWriteFd, "%%Invalid input detected(xx:xx:xx:xx:xx)\r\n");
        return ERROR;
    }

    if (NO_ERROR != odmSysIfMacAddrSet(ucIf, ucIfMode, acMac))
    {
        vosPrintf(pstEnv->nWriteFd, "Configure interface failed\r\n");
        return ERROR;
    }

    return OK;
}

	
STATUS cliCmdTftp(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	int fd, rlt = -1;
	UINT8 bootFlag;
	UINT32 tftpAddress;
	char str_ip[20] = "";
	char acCommand[128] = {0};

	if (NULL == pstEnv)
	{
		return ERROR;
	}
	
	tftpAddress = pstPt[1].u;
	cliIpToStr(tftpAddress,str_ip);
	if (!cliIsValidIpAddress (str_ip) )
	{
		vosPrintf(pstEnv->nWriteFd,"%%Invalid tftp IP server address \n");
		return ERROR;
	}

    if (vosStrLen(pstPt[2].p) > 256)
    {
        vosPrintf(pstEnv->nWriteFd, "soure file path is too long[%d]!\r\n", vosStrLen(pstPt[0].p));
        return ERROR;
    }

    if (vosStrLen(pstPt[3].p) > 256)
    {
        vosPrintf(pstEnv->nWriteFd, "dest file path is too long[%d]!\r\n", vosStrLen(pstPt[0].p));
        return ERROR;
    }
	vosPrintf(pstEnv->nWriteFd, "tftp transfer starting ... ");

	switch (pstPt[0].i)
	{
		case 1: /* get */
            sprintf(acCommand, "tftp -g %s -l %s -r %s",str_ip, pstPt[2].p,pstPt[3].p);
            vosSystem(acCommand);
        
            //sprintf(acCommand, "'/%s\t/d'",pstPt[5].p);
            printf("check image file...\r\n");
            if (check_image(pstPt[2].p) != OK)
            {
                printf("image file is invalid, download failed.\r\n\r\n");
                return ERROR;                       
            }
            vosPrintf(pstEnv->nWriteFd, "done.\r\n\r\n");   
        
            printf("write to flash...\r\n");
            bootFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_NORMAL);
            if (FLASH_BOOT_OS2_FLAG == bootFlag)
            {
                write_flash(pstPt[2].p, FLASH_DEV_NAME_OS1);
        
                sprintf(acCommand, "rm -f %s",pstPt[2].p);                
                vosSystem(acCommand);
                vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS1_FLAG);
            }
            else {
                write_flash(pstPt[2].p, FLASH_DEV_NAME_OS2);
            
                sprintf(acCommand, "rm -f %s",pstPt[2].p);                
                vosSystem(acCommand);
                vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS2_FLAG);
            }
            
            vosPrintf(pstEnv->nWriteFd, "done.\r\n\r\n");
            vosPrintf(pstEnv->nWriteFd, "System reset.\r\n\r\n");
        
            odmSysReset();
        
            break;
        
        case 2: /* put  ftpput [OPTIONS] HOST REMOTE_FILE LOCAL_FILE*/
            sprintf(acCommand, "tftp -p %s -l %s -r %s", str_ip, pstPt[2].p,pstPt[3].p);
            vosSystem(acCommand);             
            break;


		default:
			break;
	}
	return OK;
}

	
STATUS cliCmdFtp(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	int fd, rlt = -1;
	UINT32 tftpAddress;
	char str_ip[20] = "";
	UINT8 bootFlag;
	char acCommand[128] = {0};

	tftpAddress = pstPt[1].u;
	cliIpToStr(tftpAddress,str_ip);
	if (!cliIsValidIpAddress (str_ip) )
	{
		vosPrintf(pstEnv->nWriteFd,"%%Invalid ftp IP server address \n");
		return ERROR;
	}

	if((pstPt[2].p == NULL) || (pstPt[3].p == NULL)){
		vosPrintf(pstEnv->nWriteFd,"user's name or password is NULL \r\n");
		return -1;
	}

    if (vosStrLen(pstPt[2].p) > 32)
    {
        vosPrintf(pstEnv->nWriteFd, "user name is too long[%d]!\r\n", vosStrLen(pstPt[2].p));
        return ERROR;
    }

    if (vosStrLen(pstPt[3].p) > 32)
    {
        vosPrintf(pstEnv->nWriteFd, "password is too long[%d]!\r\n", vosStrLen(pstPt[3].p));
        return ERROR;
    }

    if (vosStrLen(pstPt[4].p) > 256)
    {
        vosPrintf(pstEnv->nWriteFd, "file path is too long[%d]!\r\n", vosStrLen(pstPt[4].p));
        return ERROR;
    }

    if (vosStrLen(pstPt[5].p) > 256)
    {
        vosPrintf(pstEnv->nWriteFd, "file path is too long[%d]!\r\n", vosStrLen(pstPt[5].p));
        return ERROR;
    }


	vosPrintf(pstEnv->nWriteFd, "file transfer starting ... \r\n");

	switch (pstPt[0].i)
	{
		case 1: /* get  ftpget [OPTIONS] HOST LOCAL_FILE REMOTE_FILE */            
			sprintf(acCommand, "ftpget -u %s -p %s %s %s %s", pstPt[2].p,pstPt[3].p,str_ip,pstPt[4].p,pstPt[5].p);
            vosSystem(acCommand);

            //sprintf(acCommand, "'/%s\t/d'",pstPt[5].p);
            printf("check image file...\r\n");
            if (check_image(pstPt[4].p) != OK)
            {
                printf("image file is invalid, download failed.\r\n\r\n");
                return ERROR;                       
            }
            vosPrintf(pstEnv->nWriteFd, "done.\r\n\r\n");   

            printf("write to flash...\r\n");
            bootFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_NORMAL);
            if (FLASH_BOOT_OS2_FLAG == bootFlag)
            {
                write_flash(pstPt[4].p, FLASH_DEV_NAME_OS1);

                sprintf(acCommand, "rm -f %s",pstPt[4].p);                
                vosSystem(acCommand);
                vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS1_FLAG);
            }
            else {
                write_flash(pstPt[4].p, FLASH_DEV_NAME_OS2);
            
                sprintf(acCommand, "rm -f %s",pstPt[4].p);                
                vosSystem(acCommand);
                vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS2_FLAG);
            }
            
            vosPrintf(pstEnv->nWriteFd, "done.\r\n\r\n");
            vosPrintf(pstEnv->nWriteFd, "System reset.\r\n\r\n");

            odmSysReset();

			break;

        case 2: /* put  ftpput [OPTIONS] HOST REMOTE_FILE LOCAL_FILE*/
            sprintf(acCommand, "ftpput -u %s -p %s %s %s %s", pstPt[2].p,pstPt[3].p,str_ip,pstPt[5].p,pstPt[4].p);
			vosSystem(acCommand);            
            break;


		default:
			break;
	}
  
	return OK;
}


