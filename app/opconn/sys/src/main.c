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
* FILENAME:  main.c
*
* DESCRIPTION: 
*	
*
* Date Created: Aug 18, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/src/main.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <time.h>
#include <vos.h>
#include <errors.h>
#include <cli.h>
#include <system.h>
#include <log.h>
#include <userLib.h>

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "opl_driver.h"
#include "opconn_hw_reg.h"


#ifdef LINUX
#include <signal.h>
#else
#include <sigLib.h>
#endif
#include "lw_config.h"


/*-----------------global varible declaration--------------- */

const char * product_info = "OpConn";
const char * build_date = __DATE__;
const char * build_time = __TIME__;

#ifdef __REVISION__
const char * product_rev = (const char *)__REVISION__;
#else
const char * product_rev = "0";
#endif

const char *g_pcModule[MAX_MODULE] =
{
    "drivers", 
    "system", 
    "user", 
    "qos", 
    "fdb", 
    "multicast", 
    "vlan", 
    "port", 
    "rstp", 
    "pon", 
    "log", 
    "storm",
    "oam",
    "stats",
    "all"
};

const char *g_pcDebugLevel[DEBUG_LEVEL_MAX] =
{
    "off", 
    "critical", 
    "warning", 
    "information", 
    "debugging"
};

extern UINT8	oam_src_mac[6];

extern int g_nDisableMDIO;

extern ENV_t *g_pstConsole;

UINT32 gChipRevision = ONU_OPCONN;

extern INT32 DRV_LwInit(void);

#ifdef LINUX

/** description: this is the entry for the CLI.and it is the platform for operating on systems.
 *  @param 
 *  @param 
 *  @param 

 *  @see 
 */

int main(int argc,char *argv[])
{
	UINT32 retVal, regVal;;
	UINT8  hwAddr[6];
	UINT8  enable = FALSE;
    char   acTempDir[STR_SIZE_64_BYTES] = {0};

    /* begin modifieded by liaohongjun of QID0017*/
    DBG_LINE
    VOS_DBG("%s starting ... \n", product_info);
    DBG_LINE
    if(vosCoreInit() < 0 || vosAppInit() <0)
    {
        VOS_DBG("SAL initialization failed\r\n");
        return SYS_INIT_ERROR;
    }

    DBG_LINE
	
    Drv_Ports_Init();
    /* configuration database initialize */
    VOS_DBG("Init configuration database ... ");
    if(vosConfigInit() < 0)
    {
        VOS_DBG("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        VOS_DBG("done\r\n");
    }

    DBG_LINE
    /* create the local temp dir */
    VOS_DBG("Init message queue ... ");

    odmSysCfgMsqDirGet(acTempDir);
    if(vosMsqInit(acTempDir) < 0)
    {
        VOS_DBG("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        VOS_DBG("done\r\n");
    }

    DBG_LINE
    /* Ignore SIGPIPE signal */
    signal(SIGPIPE, SIG_IGN);

    /* optimer initialize */
    VOS_DBG("Init vos timer ... ");
    if(vosTimerInit() < 0)
    {
        VOS_DBG("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        VOS_DBG("done\r\n");
    }

    DBG_LINE
    /* session initialize */
    VOS_DBG("Init command line sessions ... ");
    if (NO_ERROR != cliSessionInit())
    {
        VOS_DBG("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        VOS_DBG("done\r\n");
    }

    DBG_LINE
#if 1            //open by huangmingjian 2012/09/07 for EPN104QID0018        
    /* cli initialize */
    VOS_DBG("Init command line engine ... ");
    cliInit();
    VOS_DBG("done\r\n");
#endif
    VOS_DBG("Init text config file ... ");
    cliTextConfigInit();
    VOS_DBG("done\r\n");


    /* sys log initialize */
    VOS_DBG("Init sysLog task ... ");
    if(NO_ERROR != sysLogInit())
    {
        VOS_DBG("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        VOS_DBG("done\r\n");
    }

    
    VOS_DBG("Init LW drv ... ");
    if(NO_ERROR != DRV_LwInit())
    {
        VOS_DBG("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        VOS_DBG("done\r\n");
    }
    
    
	VOS_DBG("open opconn device ... ");
    if(NO_ERROR != open_opconn_drv())
    {
        VOS_DBG("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        VOS_DBG("done\r\n");
    }
	
	oplRegFieldRead(REG_GB_SOFTRST_ADN_DEVID, 0, 4, &gChipRevision);

    /* Init PON MAC ID */
    odmPonMacIdInit();

    VOS_DBG("Init HAL ... ");
    if(NO_ERROR != halInit())
    {
        VOS_DBG("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        VOS_DBG("done\r\n");
    }

    /* Init system default configuration */
    VOS_DBG("Init ODM system configuration ... ");
    odmSysCfgInit();
    VOS_DBG("done\r\n");

    VOS_DBG("Init ODM user configuration ... ");
    if(NO_ERROR != odmUserCfgInit())
    {
        VOS_DBG("failed\r\n");
    }
    else
    {
        VOS_DBG("done\r\n");
    }

    /* Init other module here */
    VOS_DBG("Init ODM pon configuration ... ");
#if 1	
    if(NO_ERROR != odmPonCfgInit())
    {
        VOS_DBG("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        VOS_DBG("done\r\n");
    }
#endif

    VOS_DBG("Init ODM port configuration ... ");
    if(NO_ERROR != odmPortCfgInit())
    {
        VOS_DBG("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else {
        VOS_DBG("done\r\n");
    }

#if defined(ONU_4PORT_AR8228)
    VOS_DBG("Init ODM port mirror configuration ... ");
    if(NO_ERROR != odmPortMirrorCfgInit())
    {
        VOS_DBG("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else {
        VOS_DBG("done\r\n");
    }
#endif

    VOS_DBG("Init ODM FDB configuration ... ");
    if(NO_ERROR != odmFdbInit())
    {
        VOS_DBG("failed\r\n");
    }
    else
    {
        VOS_DBG("done\r\n");
    }

#ifdef ONU_1PORT_V1E_RF100_GNA_ATE
    /*add by xukang for test--begin*/
    VOS_DBG("Init test_module configuration ... ");
    if(NO_ERROR != odmTestInit())
    {
        VOS_DBG("failed\r\n");
    }
    else
    {
        VOS_DBG("done\r\n");
    }
    /*add by xukang for test--end*/
#endif
	
    VOS_DBG("Init ODM qos configuration ... ");
    if(NO_ERROR != odmClassInit())
    {
        VOS_DBG("failed\r\n");
    }
    else
    {
        VOS_DBG("done\r\n");
    }

    VOS_DBG("Init ODM VLAN configuration ... ");
    if(NO_ERROR != odmVlanInit())
    {
        VOS_DBG("failed\r\n");
    }
    else
    {
        VOS_DBG("done\r\n");
    }

    VOS_DBG("Init ODM multicast control ... ");
    if(OK != odmMulticastInit())
    {
        VOS_DBG("failed\r\n");
    }
    else
    {
        VOS_DBG("done\r\n");
    }
    #if 0
    printf("Init ODM RSTP task ... ");
	if(OK != RSTP_START())
    {
        printf("failed\r\n");
    }
    else
    {
        printf("done\r\n");
    }
#endif
    VOS_DBG("Init ODM port storm configuration ... ");
    if(NO_ERROR != odmPortStormCfgInit())
    {
        VOS_DBG("failed\r\n");
    }
    else
    {
        VOS_DBG("done\r\n");
    }

    VOS_DBG("Init ODM service ... ");
    if(OK != odmServiceInit())
    {
        VOS_DBG("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        VOS_DBG("done\r\n");
    }

#ifdef OPL_COM_UART1
    odmOnuUartCommEnableGet(&enable);
    if (TRUE == enable)
        odmOnuUart1CommThreadInit();
#endif
    #if 0
    /* Start telnetd */
    if (ENABLED == odmSysCfgTelnetServiceGet())
    {
        retVal = cliTelnetdInit();
        printf("Init telnetd task ... ");
        if (NO_ERROR != retVal &&
            SYS_TELNETD_ALREADY_INITIALIZED != retVal)
        {
            printf("failed\r\n");
        }
        else
        {
            printf("done\r\n");
        }
    }
    #endif

/* 014539 */
#if 0
    retVal = cliCmdDebugInit();
    VOS_DBG("Init cliCmdDebugInit task ... ");
    if (NO_ERROR != retVal &&
        SYS_TELNETD_ALREADY_INITIALIZED != retVal)
    {
        VOS_DBG("failed\r\n");
    }
    else
    {
        VOS_DBG("done\r\n");
    }
#endif
/*  */
#if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)|| defined(ONU_4PORT_AR8327)
    VOS_DBG("Init DMA1 task ... ");
    if(OK != halDma1ThreadInit())
    {
        VOS_DBG("failed\r\n");
    }
    else
    {
        VOS_DBG("done\r\n");
    }
#endif
    #ifdef OPL_DMA0_ETH

/* These classifiers are moved to kernel. 
   These classifiers include:
     Copy broadcast arp to cpu.
     Copy broadcast dhcp req/ack to cpu.
     Copy frames with device's mac to cpu.
     Add reserve mac of 01-5e.....
*/    
#if 0
    printf("Init mirror task ... ");
    halMirrorClsInit();
    printf("done\r\n");
 #endif
    


/* The following classifier is not needed for us. */
#if 0
    halEocClsInit();
#endif

    #endif
	
    /* LOS LED init */
    VOS_DBG("Init LOS LED ... ");
    if(NO_ERROR != odmLOSLedInit())
    {
        VOS_DBG("failed\r\n");
    }
    else
    {
        VOS_DBG("done\r\n");
    }

	
	#if 0
    if(NO_ERROR != halBurnImageThreadInit())
    {
        printf("Failed to init burn task.\r\n");
    }
   #endif
	
	/* respin version config */
	if (IS_ONU_RESPIN)
	{
		int retVal;
		#if 1
			/*FPGA verify*/
			VOS_DBG("\n ------------- FPGA verify ---------- \n");
			retVal	= respinInit();
			if(OPL_OK != retVal)
			{
				OPL_DRV_PRINTF(("respin init failed.\n"));
			}
		#else
			/*serdis verify*/
			printf("\n------------- serdis verify version ----------");
			owr(0xe,1);
		#endif
	}

#if defined(ONU_4PORT_AR8327)

	/* Added RGMII configuration */
	
	retVal = oplRegWrite(REG_GMAC_MODE, 0x5);
    if(OPL_OK != retVal)
    {
                  return retVal;
    }          

    retVal = oplRegWrite(REG_GMAC_RGMII_SOFT_CFG, 0x6);
    if(OPL_OK != retVal)
    {
                  return retVal;
    }             

    /* mac0 RGMII PHY mode */
    regVal = 0x40000000;
    isis_reg_set(0, 0x10, (UINT8 *)&regVal, sizeof(UINT32));
    regVal = 0x007f7f7f;
    isis_reg_set(0, 0x624, (UINT8 *)&regVal, sizeof(UINT32));
    regVal = 0x07600000;
    isis_reg_set(0, 0x4, (UINT8 *)&regVal, sizeof(UINT32));
    regVal = 0x01000000;
    isis_reg_set(0, 0xc, (UINT8 *)&regVal, sizeof(UINT32));
    regVal = 0x4e;
    isis_reg_set(0, 0x7c, (UINT8 *)&regVal, sizeof(UINT32));

	/* end of: Added RGMII configuration */
#endif


#if defined(SWITCH_TEST)
    /* add by zgan - 2009/10/22, test AR8228 reset issue */
    g_nDisableMDIO = 1;
#endif

	if (g_pstConsole)
	{
		cliTextConfigLoad(g_pstConsole, DEFAULT_CONFIG_FILE);
	}

    oplRegWrite(REG_FE_TRANSFER_ENA, 0x3);
    
    /* Start console CLI */
    cliMainEntry();  //modified  by huangmingjian 2012/09/07 for EPN104QID0018 
//    while(1)
//   {
//       sleep(1);
//    }
    /* end modifieded by liaohongjun of QID0017*/

	/* destory */
	//odmRstpDestroy();
    halDestroy();
    cliTextConfigDestroy();
    odmServiceShutdown();
    vosTimerShutdown();
    sysLogShutdown();
    cliTelnetdShutdown();
    cliSessionDestroy();
    vosMsqDestroy();
    vosConfigDestroy();
    vosAppDestroy();
    vosCoreDestroy();
    //odmUart1TestThreadShutdown();

#ifdef OPL_COM_UART1
    if (TRUE == enable)
        odmOnuUart1CommThreadShutdowm();
#endif

#ifdef DEBUG
    vosMemShow();
#endif
  
    return 0;
}
#else  /* for VXWORKS */
void vxspawn()
{
}
#endif
/** @}*/
/** @}*/
