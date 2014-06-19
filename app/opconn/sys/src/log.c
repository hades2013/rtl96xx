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
* FILENAME:  log.c
*
* DESCRIPTION: 
*	
*
* Date Created: May 08, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/src/log.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <time.h>

#include "log.h"
#include "errors.h"
#include "cli_session.h"
#include "vos_socket.h"
#include "odm_service.h"

#ifndef MAX_LOG_MSG_QUEUE
#define MAX_LOG_MSG_QUEUE 1024
#endif

#ifndef MAX_LOG_FILE_SIZE
#define MAX_LOG_FILE_SIZE (256*1204)
#endif

#ifndef MAX_ALARM_FILE_SIZE
#define MAX_ALARM_FILE_SIZE (64*1204)
#endif

#ifndef MAX_LOG_TEXT_SIZE
#define MAX_LOG_TEXT_SIZE 4086
#endif

#ifndef MAX_SYSLOG_SIZE
#define MAX_SYSLOG_SIZE 1024
#endif

#ifndef MAX_SYSLOG_FILE_SIZE
#define MAX_SYSLOG_FILE_SIZE (16*1024)
#endif


#pragma pack(1)		/* pragma pack definition */

enum LOG_TYPE_e
{
    LOG_TYPE_INVALID,
    LOG_TYPE_ALARM,
    LOG_TYPE_BCAST,
    LOG_TYPE_DEBUG,
    LOG_TYPE_DEBUG_RAW,
	LOG_TYPE_LOGSERVER,    
};

struct SYS_LOG_BUF_s
{
    UINT32  lSeqNo; /* message queue mtype, if vxworks we should skip this */
    UINT16  usLogType; /* log type */
    UINT8   ucModule;
    UINT8   ucLevel;
    UINT8   ucFac;
	UINT8 	Reserved1;		
	UINT16 	Reserved2;	
    time_t  stTime;
    char    acText[MAX_LOG_TEXT_SIZE];
};

#ifndef SYS_LOG_BUF_SIZE
#define SYS_LOG_BUF_SIZE (sizeof(struct SYS_LOG_BUF_s) - sizeof(long))
#endif

#pragma pack()

const char *g_pcCRNF = "\r\n";

static UINT8 g_ucInitialized = 0;
static VOS_THREAD_t g_pstSysLogTaskId = NULL;
static volatile int g_nSysLogMsgId = -1;
struct SYS_LOG_BUF_s g_stSysLogBuf;
static UINT8 g_ucLogFileEnable = FALSE;
static UINT8 g_ucNeedToLogFile = FALSE;

static VOS_MUTEX_t g_pstSysLogMutex;

/* log file related variables */
static VOS_MUTEX_t g_pstSysLogFileMutex;
static char g_acSysLogFileName[STR_SIZE_64_BYTES] = {0};
static char g_acSysLogBakFileName[STR_SIZE_64_BYTES] = {0};
static FILE *g_pstSysLogFile = NULL;
static UINT32 g_ulSysLogFileSize = 0;

/* alarm file related variables */
static VOS_MUTEX_t g_pstSysAlarmFileMutex;
static char g_acSysAlarmFileName[STR_SIZE_64_BYTES] = {0};
static char g_acSysAlarmBakFileName[STR_SIZE_64_BYTES] = {0};
static FILE *g_pstSysAlarmFile = NULL;
static UINT32 g_ulSysAlarmFileSize = 0;
static int g_nSysAlarmCount = 0;

static long g_lSequenceNo = 0; 

/*added for syslog server */
tSyslogServer g_tSyslogServer;
static UINT8 g_ucSysLogServerEnable=ENABLED;
static VOS_MUTEX_t g_pstSysLogMsgMutex;
static VOS_MUTEX_t g_pstSysLogServerMutex;
static int  g_tSysLogServerFd;
static volatile int g_nSysLogServerMsgId=-1;
static FILE *g_pstSyslogServerFile = NULL;
static char g_acSyslogServerFileName[STR_SIZE_64_BYTES] = {0};
static char g_acSyslogServerBakFileName[STR_SIZE_64_BYTES] = {0};

static UINT32 g_ulSysLogServerFileSize = 0;
static UINT32 sysLogServerFileClose(void);
static UINT32 sysLogServerFileOpen(int nAppend);
static UINT32 syslogServerFileAppend(const char *buf);

/* extern */
extern const char *g_pcModule[MAX_MODULE];
extern const char *g_pcDebugLevel[DEBUG_LEVEL_MAX];

/* local functions declaration */
static void * sysLogThread(void *arg);
static UINT32 sysLogPrint(
    VOS_THREAD_t tThreadId,
    ENV_t *pstEnv, 
    char *pcMsgBuf
    );
static UINT32 sysLogSessionDebug(
    VOS_THREAD_t tThreadId,
    ENV_t *pstEnv, 
    UINT8 ucModule, 
    UINT8 ucLevel, 
    char *pcMsgBuf
    );

static UINT32 sysLogFileOpen(int nAppend);
static UINT32 sysLogFileClose(void);
static UINT32 sysLogFileAppend(const char *buf);

static UINT32 sysAlarmFileOpen(int nAppend);
static UINT32 sysAlarmFileClose(void);
static UINT32 sysAlarmFileAppend(const char *buf);

/* function definition start */

static long sysLogSeqNoGet()
{
    g_lSequenceNo++;
    /* range in 1 ~ MAX_LOG_MSG_QUEUE */
    if (g_lSequenceNo > MAX_LOG_MSG_QUEUE)
        g_lSequenceNo = 1;
    return g_lSequenceNo;
}

UINT32 sysLogInit()
{

    if (g_ucInitialized) {
        return SYS_LOG_THREAD_ALREADY_INITIALIZED;
    }

    odmSysCfgTempDirGet(g_acSysLogFileName);
    strcat(g_acSysLogFileName, SLASH_STR); 
    strcat(g_acSysLogFileName, DEFAULT_LOG_FILE_NAME); 

    odmSysCfgTempDirGet(g_acSysLogBakFileName);
    strcat(g_acSysLogBakFileName, SLASH_STR); 
    strcat(g_acSysLogBakFileName, DEFAULT_BAK_LOG_FILE_NAME); 

    odmSysCfgTempDirGet(g_acSysAlarmFileName);
    strcat(g_acSysAlarmFileName, SLASH_STR); 
    strcat(g_acSysAlarmFileName, DEFAULT_ALARM_FILE_NAME); 

    odmSysCfgTempDirGet(g_acSysAlarmBakFileName);
    strcat(g_acSysAlarmBakFileName, SLASH_STR); 
    strcat(g_acSysAlarmBakFileName, DEFAULT_BAK_ALARM_FILE_NAME); 

    if (vosMutexCreate(&g_pstSysLogMutex) != VOS_MUTEX_OK) {
        return SYS_LOG_THREAD_CREATE_FAILED;
    }
    if (vosMutexCreate(&g_pstSysLogFileMutex) != VOS_MUTEX_OK) {
        vosMutexDestroy(&g_pstSysLogMutex);
       // g_pstSysLogMutex = NULL;
        return SYS_LOG_THREAD_CREATE_FAILED;
    }

    sysLogFileOpen(1);

    if (vosMutexCreate(&g_pstSysAlarmFileMutex) != VOS_MUTEX_OK) {
        vosMutexDestroy(&g_pstSysLogMutex);
        sysLogFileClose();
        vosMutexDestroy(&g_pstSysLogFileMutex);
        return SYS_LOG_THREAD_CREATE_FAILED;
    }

    sysAlarmFileOpen(1);

    g_nSysLogMsgId = vosMsqCreate(sizeof(struct SYS_LOG_BUF_s));
    if (-1 == g_nSysLogMsgId) {
        vosMutexDestroy(&g_pstSysLogMutex);
        sysLogFileClose();
        vosMutexDestroy(&g_pstSysLogFileMutex);
        sysAlarmFileClose();
        vosMutexDestroy(&g_pstSysAlarmFileMutex);
        return SYS_LOG_THREAD_CREATE_FAILED;
    }
	if(sysLogServerInit()!=NO_ERROR)
	{
	    vosMutexDestroy(&g_pstSysLogMutex);
        sysLogFileClose();
        vosMutexDestroy(&g_pstSysLogFileMutex);
        sysAlarmFileClose();
        vosMutexDestroy(&g_pstSysAlarmFileMutex);
        vosMsqDelete(g_nSysLogMsgId);
        g_nSysLogMsgId = -1;
        return SYS_LOG_THREAD_CREATE_FAILED;	
	}
    g_pstSysLogTaskId = vosThreadCreate("tSysLog", OP_VOS_THREAD_STKSZ, 110,
                        (void *)sysLogThread, (void *)NULL);
    if (g_pstSysLogTaskId == NULL) {
		sysLogServerDestroy();
        vosMsqDelete(g_nSysLogMsgId);
        g_nSysLogMsgId = -1;
        vosMutexDestroy(&g_pstSysLogMutex);
        sysLogFileClose();
        vosMutexDestroy(&g_pstSysLogFileMutex);
        sysAlarmFileClose();
        vosMutexDestroy(&g_pstSysAlarmFileMutex);
        return SYS_LOG_THREAD_CREATE_FAILED;
    }

    g_ucInitialized = 1;

    return NO_ERROR;
    
}

UINT32 sysLogShutdown(void)
{
    if (!g_ucInitialized) {
        return SYS_LOG_THREAD_NOT_INITIALIZED;
    }

    g_ucInitialized = 0;

    if (g_nSysLogMsgId >= 0)
    {
        vosMsqDelete(g_nSysLogMsgId);
        g_nSysLogMsgId = -1;
    }

    if (g_pstSysLogTaskId != NULL) {
        vosThreadDestroy(g_pstSysLogTaskId);
        g_pstSysLogTaskId = NULL;
    }

    sysLogFileClose();
    sysAlarmFileClose();

    vosMutexDestroy(&g_pstSysLogMutex);

    vosMutexDestroy(&g_pstSysLogFileMutex);

    vosMutexDestroy(&g_pstSysAlarmFileMutex);

    return NO_ERROR;
}


static UINT32 sysLogPrint(
    VOS_THREAD_t tThreadId,
    ENV_t *pstEnv, 
    char *pcMsgBuf
    )
{
    if (pstEnv == NULL)
        return INVALID_32;

    vosPrintf(pstEnv->nWriteFd, pcMsgBuf);

    return NO_ERROR;
}

static UINT32 sysLogSessionDebug(
    VOS_THREAD_t tThreadId,
    ENV_t *pstEnv, 
    UINT8 ucModule, 
    UINT8 ucLevel, 
    char *pcMsgBuf
    )
{
    UINT32 res = INVALID_32;

    if (pstEnv == NULL)
        return res;

    if (OK == cliSessionDebugLevelCheck(pstEnv->ucIndex, ucModule, ucLevel))
    {
        vosPrintf(pstEnv->nWriteFd, pcMsgBuf);
        if (FALSE == g_ucNeedToLogFile)
        {
            g_ucNeedToLogFile = TRUE;
        }
        res = NO_ERROR;
    }

    return res;
}

static void * sysLogThread(void *arg)
{
    struct SYS_LOG_BUF_s msgBuf;
    char acTime[TIME_STR_LEN];
    char acTypeStr[STR_SIZE_128_BYTES];
    uint32 length;
    struct tm *pstLogTime;
	UDP_PARAMS_t pParams;
	tSyslogServer *SyslogServer;
	struct list_head *pos, *n;
    char Logstr[4096];

    while(g_nSysLogMsgId >= 0)
    {
        length = vosMsqReceive(g_nSysLogMsgId, (void *)&msgBuf, SYS_LOG_BUF_SIZE);
        if (length == INVALID_32)
        {
            break;
        }
		
		vosMemSet(acTypeStr,0,sizeof(acTypeStr));//added
        pstLogTime = gmtime(&msgBuf.stTime);
        strftime(acTime, TIME_STR_LEN, "%Y-%m-%dT%H:%M:%S", pstLogTime);
        switch (msgBuf.usLogType)
        {
        case LOG_TYPE_ALARM:
            vosSnprintf(acTypeStr, STR_SIZE_128_BYTES, "%s: ", acTime);
            vosHashForEachDoCb(
                cliSessionTableGet(),
                (FUNCPTR)sysLogPrint,
                (uint32)&acTypeStr[0], 4, 5, 6, 7, 8);
            vosHashForEachDoCb(
                cliSessionTableGet(),
                (FUNCPTR)sysLogPrint,
                (uint32)&msgBuf.acText[0], 4, 5, 6, 7, 8);
            vosHashForEachDoCb(
                cliSessionTableGet(),
                (FUNCPTR)sysLogPrint,
                (uint32)g_pcCRNF, 4, 5, 6, 7, 8);
            sysAlarmFileAppend(acTypeStr);
            sysAlarmFileAppend(&msgBuf.acText[0]);
            sysAlarmFileAppend(g_pcCRNF);
            break;

        case LOG_TYPE_BCAST:
            vosSnprintf(acTypeStr, STR_SIZE_128_BYTES, "%s: ", acTime);
            vosHashForEachDoCb(
                cliSessionTableGet(),
                (FUNCPTR)sysLogPrint,
                (uint32)&acTypeStr[0], 4, 5, 6, 7, 8);
            vosHashForEachDoCb(
                cliSessionTableGet(),
                (FUNCPTR)sysLogPrint,
                (uint32)&msgBuf.acText[0], 4, 5, 6, 7, 8);
            vosHashForEachDoCb(
                cliSessionTableGet(),
                (FUNCPTR)sysLogPrint,
                (uint32)g_pcCRNF, 4, 5, 6, 7, 8);
            break;

        case LOG_TYPE_DEBUG:
            /* for cut down space usage, we dont use the header and format output
            vosSnprintf(acTypeStr, STR_SIZE_128_BYTES, "%s %-9s %-11s ", 
                acTime, g_pcModule[msgBuf.ucModule], g_pcDebugLevel[msgBuf.ucLevel]); */
            vosSnprintf(acTypeStr, STR_SIZE_128_BYTES, "%s %s %s: ", 
                acTime, g_pcModule[msgBuf.ucModule], g_pcDebugLevel[msgBuf.ucLevel]);
            vosHashForEachDoCb(
                cliSessionTableGet(),
                (FUNCPTR)sysLogSessionDebug,
                (uint32)msgBuf.ucModule, 
                (uint32)msgBuf.ucLevel, 
                (uint32)&acTypeStr[0], 6, 7, 8);
            vosHashForEachDoCb(
                cliSessionTableGet(),
                (FUNCPTR)sysLogSessionDebug,
                (uint32)msgBuf.ucModule, 
                (uint32)msgBuf.ucLevel, 
                (uint32)&msgBuf.acText[0], 6, 7, 8);
            vosHashForEachDoCb(
                cliSessionTableGet(),
                (FUNCPTR)sysLogSessionDebug,
                (uint32)msgBuf.ucModule, 
                (uint32)msgBuf.ucLevel, 
                (uint32)g_pcCRNF, 6, 7, 8);
            if (TRUE == sysLogGetLogEnable() && TRUE == g_ucNeedToLogFile)
            {
                sysLogFileAppend(acTypeStr);
                sysLogFileAppend(&msgBuf.acText[0]);
                sysLogFileAppend(g_pcCRNF);
                g_ucNeedToLogFile = FALSE;
            }
            break;

        case LOG_TYPE_DEBUG_RAW:
            vosHashForEachDoCb(
                cliSessionTableGet(),
                (FUNCPTR)sysLogSessionDebug,
                (uint32)msgBuf.ucModule, 
                (uint32)msgBuf.ucLevel, 
                (uint32)&msgBuf.acText[0], 6, 7, 8);
            if (TRUE == sysLogGetLogEnable() && TRUE == g_ucNeedToLogFile)
            {
                sysLogFileAppend(&msgBuf.acText[0]);
                g_ucNeedToLogFile = FALSE;
            }
            break;
		case LOG_TYPE_LOGSERVER:
			if(g_ucSysLogServerEnable==TRUE)
			{
			
				vosMemSet(Logstr,0,sizeof(Logstr));//added
				vosSnprintf(acTypeStr, STR_SIZE_128_BYTES, "<%ld> %s %s %s:", 
					msgBuf.ucFac|msgBuf.ucLevel,acTime,odmSysCfgNameGet(),g_pcModule[msgBuf.ucModule]);
				vosSnprintf(Logstr,4096,"%s%s",acTypeStr,&msgBuf.acText[0]);	
				syslogServerFileAppend(Logstr);
				syslogServerFileAppend(g_pcCRNF);
				vosMemSet(&pParams,0,sizeof(pParams));	
				list_for_each_safe(pos, n, &g_tSyslogServer.list) {
					SyslogServer=list_entry(pos,tSyslogServer,list);	
					pParams.clientPort=SyslogServer->logServerPort;
					pParams.clientAddr=SyslogServer->logserverIp;
					pParams.pSendPacket=Logstr;
					pParams.sendPacketLength=vosStrLen(Logstr);
					
					if(vosSocketUdpSendTo(g_tSysLogServerFd,&pParams)!=0)
					{
						//printf("sendto syslog server %x error \n",pParams.clientAddr);
					}			
					//printf("sendto syslog server %x successful \n",pParams.clientAddr);
				}		
			}
			break;
        default:
            break;
        }
    }
    vosThreadExit(0);
}

UINT32 sysLogAlarm(char *fmt, ...)
{
    if (cliSessionTableGet() == NULL)
        return INVALID_32;

    vosMutexTake(&g_pstSysLogMutex);

    g_nSysAlarmCount++;
    va_list	argp;
    va_start(argp, fmt);
    vosVSnprintf(g_stSysLogBuf.acText, MAX_LOG_TEXT_SIZE, fmt, argp);
    g_stSysLogBuf.lSeqNo = sysLogSeqNoGet();
    g_stSysLogBuf.usLogType = LOG_TYPE_ALARM;
    time(&g_stSysLogBuf.stTime);
    g_stSysLogBuf.ucModule = ALL_MODULE;
    g_stSysLogBuf.ucLevel = DEBUG_LEVEL_OFF;
    vosMsqSend(g_nSysLogMsgId, (char *)&g_stSysLogBuf, SYS_LOG_BUF_SIZE);
    va_end(argp);

    /* TODO: set alarm led here */
    odmAlarmLedControl(TRUE);

    vosMutexGive(&g_pstSysLogMutex);

    return NO_ERROR;
}

UINT32 sysLogAlarmDecrease(int nNum)
{

    vosMutexTake(&g_pstSysLogMutex);

    g_nSysAlarmCount -= nNum;
    if (0 >= g_nSysAlarmCount)
    {
        g_nSysAlarmCount = 0;
        odmAlarmLedControl(FALSE);
    }

    vosMutexGive(&g_pstSysLogMutex);

    return NO_ERROR;
}

UINT32 sysLogBroadcast(char *fmt, ...)
{
    if (cliSessionTableGet() == NULL)
        return INVALID_32;

    vosMutexTake(&g_pstSysLogMutex);

    va_list	argp;
    va_start(argp, fmt);
    vosVSnprintf(g_stSysLogBuf.acText, MAX_LOG_TEXT_SIZE, fmt, argp);
    g_stSysLogBuf.lSeqNo = sysLogSeqNoGet();
    g_stSysLogBuf.usLogType = LOG_TYPE_BCAST;
    time(&g_stSysLogBuf.stTime);
    g_stSysLogBuf.ucModule = ALL_MODULE;
    g_stSysLogBuf.ucLevel = DEBUG_LEVEL_OFF;
    vosMsqSend(g_nSysLogMsgId, (char *)&g_stSysLogBuf, SYS_LOG_BUF_SIZE);
    va_end(argp);

    vosMutexGive(&g_pstSysLogMutex);

    return NO_ERROR;
}


UINT32 sysLogDebug(UINT8 ucModule, UINT8 ucLevel, char *fmt, ...)
{
	UINT32 log = 1;
    if (cliSessionTableGet() == NULL)
        log = 0;

    if (OK != cliSessionGlobalDebugLevelCheck(ucModule, ucLevel))
        log = 0;

    vosMutexTake(&g_pstSysLogMutex);

    va_list	argp;
    va_start(argp, fmt);
    vosVSnprintf(g_stSysLogBuf.acText, MAX_LOG_TEXT_SIZE, fmt, argp);
	
	if (1 == log)
	{
		
	    g_stSysLogBuf.lSeqNo = sysLogSeqNoGet();
	    g_stSysLogBuf.usLogType = LOG_TYPE_DEBUG;
	    time(&g_stSysLogBuf.stTime);
	    g_stSysLogBuf.ucModule = ucModule;
	    g_stSysLogBuf.ucLevel = ucLevel;
	    vosMsqSend(g_nSysLogMsgId, (char *)&g_stSysLogBuf, SYS_LOG_BUF_SIZE);
	}
	
    va_end(argp);

    vosMutexGive(&g_pstSysLogMutex);

    return NO_ERROR;
}


UINT32 sysLogDebugRawData(UINT8 ucModule, UINT8 ucLevel, char *fmt, ...)
{
    if (cliSessionTableGet() == NULL)
        return INVALID_32;

    if (OK != cliSessionGlobalDebugLevelCheck(ucModule, ucLevel))
        return INVALID_32;

    vosMutexTake(&g_pstSysLogMutex);

    va_list	argp;
    va_start(argp, fmt);
    vosVSnprintf(g_stSysLogBuf.acText, MAX_LOG_TEXT_SIZE, fmt, argp);
    g_stSysLogBuf.lSeqNo = sysLogSeqNoGet();
    g_stSysLogBuf.usLogType = LOG_TYPE_DEBUG_RAW;
    time(&g_stSysLogBuf.stTime);
    g_stSysLogBuf.ucModule = ucModule;
    g_stSysLogBuf.ucLevel = ucLevel;
    vosMsqSend(g_nSysLogMsgId, (char *)&g_stSysLogBuf, SYS_LOG_BUF_SIZE);
    va_end(argp);

    vosMutexGive(&g_pstSysLogMutex);

    return NO_ERROR;
}

UINT32 sysLogMsg(UINT8 ucModule, UINT8 ucLevel,UINT8 ucFac, char *fmt, ...)
{


    vosMutexTake(&g_pstSysLogServerMutex);

    va_list	argp;
    va_start(argp, fmt);
    vosVSnprintf(g_stSysLogBuf.acText, MAX_SYSLOG_SIZE, fmt, argp);
    g_stSysLogBuf.usLogType = LOG_TYPE_LOGSERVER;
    time(&g_stSysLogBuf.stTime);
    g_stSysLogBuf.ucModule = ucModule;
    g_stSysLogBuf.ucLevel = ucLevel&0x07;
	if((ucFac>SYSLOG_FAC_LOCAL7)||(ucFac%8!=0))	
		g_stSysLogBuf.ucFac = SYSLOG_FAC_LOCAL7;
	else		
		g_stSysLogBuf.ucFac = ucFac;
    vosMsqSend(g_nSysLogMsgId, (char *)&g_stSysLogBuf, SYS_LOG_BUF_SIZE);
    va_end(argp);

    vosMutexGive(&g_pstSysLogServerMutex);

    return NO_ERROR;
}
UINT32 sysLogSetLogEnable(UINT8 ucEnableLog)
{
	
    g_ucLogFileEnable = ucEnableLog;

    return NO_ERROR;
}
UINT32 sysLogSetServerLogEnable(UINT8 ucEnable)
{
	char section[16] = {0};

	g_ucSysLogServerEnable = ucEnable;
	vosSprintf(section, SYSLOG_CONFIG_GLB_STATUS);
	if (vosConfigValueSet(SYSLOG_CONFIG_NAME,SYSLOG_CONFIG_GLOBAL, 
						section,STATE_STR_GET(g_ucSysLogServerEnable)) != NO_ERROR)
	{
	    return SYS_LOG_SERVER_CFG_ERROR;
	}
	return NO_ERROR;
}

UINT32 sysLogTestServerLog(UINT8 ucEnable)
{
	int i,j;
	char *aaa="hello world,syslog";
	for(i=0;i<SYSLOG_LEVEL_DEBUG;i++)
			sysLogMsg(i,i,SYSLOG_FAC_LOCAL7,"only for test %s",aaa);	
	return NO_ERROR;

}

tSyslogServer * sysLogGetServerLogEntry(UINT32 ucSyslogAddress)
{
	UINT32 logip;
	struct list_head *pos, *n;
	tSyslogServer *SyslogServer;

	list_for_each_safe(pos, n, &g_tSyslogServer.list) {
		SyslogServer=list_entry(pos,tSyslogServer,list);
		logip=SyslogServer->logserverIp;
		if(logip==ucSyslogAddress)
		{
			return SyslogServer;
		}
	}	
	return NULL;
}

static UINT32 odmSysLogSetServerLogAddress(UINT32 ucSyslogAddress,UINT16 ucLogPort)
{
	tSyslogServer *SyslogServer;

	if(sysLogGetServerLogEntry(ucSyslogAddress)!=NULL)
	{
		return 0;
	}
	SyslogServer=vosAlloc(sizeof(tSyslogServer));
	if(SyslogServer==NULL)
	{
		return 0;
	}
	vosMemSet(SyslogServer,0,sizeof(tSyslogServer));
	SyslogServer->logserverIp=ucSyslogAddress;
	SyslogServer->logServerPort=ucLogPort;	
	SyslogServer->ServerNum=(++g_tSyslogServer.ServerNum);
	list_add(&SyslogServer->list, &g_tSyslogServer.list);
	return  SyslogServer->ServerNum;

}
UINT32 sysLogSetServerLogAddress(UINT32 ucSyslogAddress,UINT16 ucLogPort)
{
	char section[16] = {0};
    char str_ip[20] = "";
	int index;
	index=odmSysLogSetServerLogAddress(ucSyslogAddress, ucLogPort);
	if(index)
	{
		vosSprintf(section, SYSLOG_SERVER_SECTION, g_tSyslogServer.ServerNum);
		if (vosConfigSectionCreate(SYSLOG_CONFIG_NAME,section) != NO_ERROR)
	    {
	        return SYS_LOG_SERVER_CFG_ERROR;
	    }	
	    cliIpToStr(ucSyslogAddress,str_ip);
		if (vosConfigValueSet(SYSLOG_CONFIG_NAME,section, SYSLOG_CONFIG_SERVER,str_ip) != NO_ERROR)
	    {
	        return SYS_LOG_SERVER_CFG_ERROR;
	    }
		if (vosConfigUInt32Set(SYSLOG_CONFIG_NAME,section, SYSLOG_CONFIG_PORT,ucLogPort) != NO_ERROR)
	    {
	        return SYS_LOG_SERVER_CFG_ERROR;
	    }		
		return NO_ERROR;
	}

	return SYS_LOG_SERVER_ALREADY_EXIST;
}
UINT32 sysLogDelServerLogAddress(UINT32 ucSyslogAddress)
{
	tSyslogServer *SyslogServer;
	char section[16] = {0};
	
	SyslogServer=sysLogGetServerLogEntry(ucSyslogAddress);
	if(SyslogServer!=NULL)
	{
		vosSprintf(section, SYSLOG_SERVER_SECTION, SyslogServer->ServerNum);
		vosConfigSectionDelete(SYSLOG_CONFIG_NAME,section);
		list_del(&SyslogServer->list);
		vosFree(SyslogServer);		
		g_tSyslogServer.ServerNum--;			
		return NO_ERROR;
	}
	return  SYS_LOG_SERVER_NOT_EXIST;
}

UINT8 sysLogGetServerLogEnable()
{
    return g_ucSysLogServerEnable;
}

UINT32 sysLogServerShow(ENV_t *pstEnv)
{
	char *pcLine;
	UINT32 logip;
    char strIp[20] = {0};
	tSyslogServer *SyslogServer;
	struct list_head *pos, *n;
    FILE *pstLogFile = NULL;
	
    vosPrintf(pstEnv->nWriteFd, "Syslog Server Status : %s\r\n",STATE_STR_GET(sysLogGetServerLogEnable()));
    vosPrintf(pstEnv->nWriteFd, "Number of Syslog Server(s): %d\r\n",g_tSyslogServer.ServerNum);
	vosPrintf(pstEnv->nWriteFd, "-------------------------------------------------------\r\n");
	list_for_each_safe(pos, n, &g_tSyslogServer.list) {
		SyslogServer=list_entry(pos,tSyslogServer,list);
		logip=SyslogServer->logserverIp;
		  	cliIpToStr(logip,strIp);
	    	vosPrintf(pstEnv->nWriteFd, "Syslog Server IP : %15s\t",strIp);
			vosPrintf(pstEnv->nWriteFd, "Port : %5d\r\n",SyslogServer->logServerPort);

	}
	vosPrintf(pstEnv->nWriteFd, "\r\nLogging Messages:\r\n");
	vosMutexTake(&g_pstSysLogMsgMutex);
	if ((pstLogFile = vosFOpen(g_acSyslogServerBakFileName, "rb")) != NULL) 
    {
        while (!feof(pstLogFile)) {
            pcLine = vosFGetWholeLine(pstLogFile);
            if (pcLine == NULL)
                continue;
            vosPrintf(pstEnv->nWriteFd,pcLine);
            vosPrintf(pstEnv->nWriteFd, "\n");
            vosFree(pcLine);
        }
    }
    if ((pstLogFile = vosFOpen(g_acSyslogServerFileName, "rb")) != NULL) 
    {   
        while (!feof(pstLogFile)) {
            pcLine = vosFGetWholeLine(pstLogFile);
            if (pcLine == NULL)
                continue;
            vosPrintf(pstEnv->nWriteFd,pcLine);
            vosPrintf(pstEnv->nWriteFd, "\n");
            vosFree(pcLine);
        }
    }
    vosMutexGive(&g_pstSysLogMsgMutex);
	return NO_ERROR;
}

UINT32 sysLogServerInit()
{
	int status;
    UINT8  count;
    UINT8  roop;
	char section[16] = {0};
	char *session=NULL;
	UINT16 LogPort;
	UINT32 LogIp;
	char *pLogIp=NULL;
	char *pLogStatus;


	
	vosMemSet(&g_tSyslogServer, 0, sizeof(tSyslogServer));
	INIT_LIST_HEAD(&g_tSyslogServer.list);

    if (VOS_MUTEX_OK!= vosMutexCreate(&g_pstSysLogMsgMutex)) 
	{
		return SYS_LOG_THREAD_CREATE_FAILED;
    }

    if (VOS_MUTEX_OK!=vosMutexCreate(&g_pstSysLogServerMutex)) 
	{
		return SYS_LOG_THREAD_CREATE_FAILED;
    }
	
	 g_tSysLogServerFd = vosSocketUdpCreate();
    if (-1 ==g_tSysLogServerFd) 
	{
		return SYS_LOG_THREAD_CREATE_FAILED;
    }
	status=vosSocketBind(g_tSysLogServerFd,0,SYSLOG_DEFAULT_PORT);
	if (-1==status) 
	{
		return SYS_LOG_THREAD_CREATE_FAILED;
    }
	odmSysCfgTempDirGet(g_acSyslogServerFileName);
    strcat(g_acSyslogServerFileName, SLASH_STR); 
    strcat(g_acSyslogServerFileName, DEFAULT_SYSLOG_SERVER_FILE_NAME); 

    odmSysCfgTempDirGet(g_acSyslogServerBakFileName);
    strcat(g_acSyslogServerBakFileName, SLASH_STR); 
    strcat(g_acSyslogServerBakFileName, DEFAULT_SYSLOG_SERVER_BAK_FILE_NAME); 
    sysLogServerFileClear();
	
	vosSprintf(section, SYSLOG_CONFIG_GLOBAL);
	if(vosConfigSectionIsExisted(SYSLOG_CONFIG_NAME,section))
	{
		vosConfigSectionCreate(SYSLOG_CONFIG_NAME,section);
		if (vosConfigValueSet(SYSLOG_CONFIG_NAME,section, 
							SYSLOG_CONFIG_GLB_STATUS,STATE_STR_GET(g_ucSysLogServerEnable)) != NO_ERROR)
		{
		    return SYS_LOG_SERVER_CFG_ERROR;
		}
	}
	else
	{
		pLogStatus = vosConfigValueGet(SYSLOG_CONFIG_NAME,section,SYSLOG_CONFIG_GLB_STATUS,0);
		if(pLogStatus!=NULL && !vosStrCmp(pLogStatus,STATE_ENABLE_STR))	
			g_ucSysLogServerEnable=ENABLED;
		else
			g_ucSysLogServerEnable=DISABLED;		
			
	}
    count = vosConfigSectionCount(SYSLOG_CONFIG_NAME);
    for (roop = 0;roop < count;roop++)
    {
	    if (vosConfigSectionGetByIndex(SYSLOG_CONFIG_NAME,roop,&session) == 0)
	    {
	        LogPort = vosConfigUInt32Get(SYSLOG_CONFIG_NAME,session,SYSLOG_CONFIG_PORT,0);
			if(LogPort!=0)
			{
				pLogIp = vosConfigValueGet(SYSLOG_CONFIG_NAME,session,SYSLOG_CONFIG_SERVER,0);
				if(pLogIp!=NULL)
				{
					cliStrToIp(pLogIp,(ULONG *)&LogIp);
					odmSysLogSetServerLogAddress(LogIp,LogPort);
				}
				
			}
	     }
    }

	return NO_ERROR;
}

UINT32 sysLogServerDestroy()
{
	sysLogServerFileClose();
	vosSocketClose(g_tSysLogServerFd);	
    vosMutexDestroy(&g_pstSysLogMsgMutex);
    vosMutexDestroy(&g_pstSysLogServerMutex);
	return NO_ERROR;
}

UINT8 sysLogGetLogEnable()
{
    return g_ucLogFileEnable;
}

UINT32 sysLogFileOpen(int nAppend)
{
    char title[STR_SIZE_128_BYTES] = {0};
    char divider[STR_SIZE_128_BYTES] = {0};
    int ch, writeLen;
#ifndef NO_FILEIO
    sysLogFileClose();
    vosMutexTake(&g_pstSysLogFileMutex);
    if ((g_pstSysLogFile = vosFOpen(g_acSysLogFileName, (nAppend > 0) ? "ab" : "wb")) == 0) 
    {
        vosMutexGive(&g_pstSysLogFileMutex);
        return INVALID_32;
    }
    
    g_ulSysLogFileSize = 0;    
    if (nAppend > 0)
    {
        while ((ch = fgetc(g_pstSysLogFile)) != EOF)
        {
            g_ulSysLogFileSize++;
        }
    }

    /* for cut down space usage, we dont use the header and format output
    if (0 >= nAppend || 0 == g_ulSysLogFileSize)
    {
        vosSnprintf(title, STR_SIZE_128_BYTES, "%-19s %-9s %-11s %s\r\n", "Time", "Module", "Level", "Message");
        writeLen = fwrite(title, 1, strlen(title), g_pstSysLogFile);
        g_ulSysLogFileSize += writeLen;
        fflush(g_pstSysLogFile);
        vosSnprintf(divider, STR_SIZE_128_BYTES, "------------------- --------- ----------- -------------------------------------\r\n");
        writeLen = fwrite(divider, 1, strlen(divider), g_pstSysLogFile);
        g_ulSysLogFileSize += writeLen;
        fflush(g_pstSysLogFile);
    }
    */
    vosMutexGive(&g_pstSysLogFileMutex);
#endif /* NO_FILEIO */
    return NO_ERROR;
}

UINT32 sysLogFileClose(void)
{
#ifndef NO_FILEIO
    vosMutexTake(&g_pstSysLogFileMutex);
    if (!g_acSysLogFileName) {
        vosMutexGive(&g_pstSysLogFileMutex);
        return INVALID_32;
    }
    if (g_pstSysLogFile) {
        vosFClose(g_pstSysLogFile);
        g_pstSysLogFile = NULL;
    }
    vosMutexGive(&g_pstSysLogFileMutex);
#endif
    return NO_ERROR;
}

UINT32 sysLogServerFileOpen(int nAppend)
{
    char title[STR_SIZE_128_BYTES] = {0};
    char divider[STR_SIZE_128_BYTES] = {0};
    int ch, writeLen;
#ifndef NO_FILEIO
    sysLogServerFileClose();
    vosMutexTake(&g_pstSysLogMsgMutex);
    if ((g_pstSyslogServerFile = vosFOpen(g_acSyslogServerFileName, (nAppend > 0) ? "ab" : "wb")) == 0) 
    {
        vosMutexGive(&g_pstSysLogMsgMutex);
        return INVALID_32;
    }
    
    g_ulSysLogServerFileSize = 0;    
    if (nAppend > 0)
    {
        while ((ch = fgetc(g_pstSyslogServerFile)) != EOF)
        {
            g_ulSysLogServerFileSize++;
        }
    }

    vosMutexGive(&g_pstSysLogMsgMutex);
#endif /* NO_FILEIO */
    return NO_ERROR;
}

UINT32 sysLogServerFileClose(void)
{
#ifndef NO_FILEIO
    vosMutexTake(&g_pstSysLogMsgMutex);
    if (!g_acSyslogServerFileName) {
        vosMutexGive(&g_pstSysLogMsgMutex);
        return INVALID_32;
    }
    if (g_pstSyslogServerFile) {
        vosFClose(g_pstSyslogServerFile);
        g_pstSyslogServerFile = NULL;
    }
    vosMutexGive(&g_pstSysLogMsgMutex);
#endif
    return NO_ERROR;
}

UINT32 sysLogServerFileClear()
{
    UINT32	retv = 0;
#ifndef NO_FILEIO
    /* Truncate file to zero length or create text file for writing. 
    The stream is positioned at the beginning of the file. */
    retv = sysLogServerFileOpen(0);
    if (retv == INVALID_32)
        return retv;



    vosMutexTake(&g_pstSysLogMsgMutex);
    vosRemove(g_acSyslogServerBakFileName);
    vosMutexGive(&g_pstSysLogMsgMutex);
    /* reopen the log file */
    retv = sysLogServerFileOpen(1);
    if (retv == INVALID_32)
        return retv;

#endif /* !NO_FILEIO */
    return(retv);
}


UINT32 syslogServerFileAppend(const char *buf)
{
    int	retv = 0;
    int nbytes = vosStrLen(buf);
#ifndef NO_FILEIO

    if (nbytes + g_ulSysLogServerFileSize > MAX_SYSLOG_FILE_SIZE)
    {
        sysLogServerFileClose();
        vosMutexTake(&g_pstSysLogMsgMutex);
        retv = vosRename(g_acSyslogServerFileName, g_acSyslogServerBakFileName);
        vosMutexGive(&g_pstSysLogMsgMutex);
        if (retv == -1)
            return (UINT32)retv;
        
        retv = sysLogServerFileClear();
        if (retv == -1)
            return (UINT32)retv;
    }
    vosMutexTake(&g_pstSysLogMsgMutex);
    if (g_pstSyslogServerFile) {
        retv = fwrite(buf, 1, nbytes, g_pstSyslogServerFile);
        g_ulSysLogServerFileSize += retv;
        fflush(g_pstSyslogServerFile);
    } else
        retv = 0;

    vosMutexGive(&g_pstSysLogMsgMutex);
#endif /* !NO_FILEIO */
    return(retv);
}

UINT32 sysLogFileAppend(const char *buf)
{
    int	retv = 0;
    int nbytes = vosStrLen(buf);
#ifndef NO_FILEIO
    if (nbytes + g_ulSysLogFileSize > MAX_LOG_FILE_SIZE)
    {
        sysLogFileClose();
        vosMutexTake(&g_pstSysLogFileMutex);
        retv = vosRename(g_acSysLogFileName, g_acSysLogBakFileName);
        vosMutexGive(&g_pstSysLogFileMutex);
        if (retv == -1)
            return (UINT32)retv;
        
        retv = sysLogFileClear();
        if (retv == -1)
            return (UINT32)retv;
    }
    vosMutexTake(&g_pstSysLogFileMutex);
    if (g_pstSysLogFile) {
        retv = fwrite(buf, 1, nbytes, g_pstSysLogFile);
        g_ulSysLogFileSize += retv;
        fflush(g_pstSysLogFile);
    } else
        retv = 0;

    vosMutexGive(&g_pstSysLogFileMutex);
#endif /* !NO_FILEIO */
    return(retv);
}


UINT32 sysLogFileClear()
{
    UINT32	retv = 0;
#ifndef NO_FILEIO
    /* Truncate file to zero length or create text file for writing. 
    The stream is positioned at the beginning of the file. */
    retv = sysLogFileOpen(0);
    if (retv == INVALID_32)
        return retv;

    /* reopen the log file */
    retv = sysLogFileOpen(1);
    if (retv == INVALID_32)
        return retv;

    vosMutexTake(&g_pstSysLogFileMutex);
    vosRemove(g_acSysLogBakFileName);
    vosMutexGive(&g_pstSysLogFileMutex);

#endif /* !NO_FILEIO */
    return(retv);
}


UINT32 sysAlarmFileOpen(int nAppend)
{
    char title[STR_SIZE_128_BYTES] = {0};
    char divider[STR_SIZE_128_BYTES] = {0};
    int ch, writeLen;
#ifndef NO_FILEIO
    sysAlarmFileClose();
    vosMutexTake(&g_pstSysAlarmFileMutex);
    if ((g_pstSysAlarmFile = vosFOpen(g_acSysAlarmFileName, (nAppend > 0) ? "ab" : "wb")) == 0) 
    {
        vosMutexGive(&g_pstSysAlarmFileMutex);
        return INVALID_32;
    }
    g_ulSysAlarmFileSize = 0;    

    while ((ch = fgetc(g_pstSysAlarmFile)) != EOF)
    {
        g_ulSysAlarmFileSize++;
    }

    /* for cut down space usage, we dont use the header and format output
    if (0 >= nAppend || 0 == g_ulSysAlarmFileSize)
    {
        vosSnprintf(title, STR_SIZE_128_BYTES, "%-19s %s\r\n", "Time", "Message");
        writeLen = fwrite(title, 1, strlen(title), g_pstSysAlarmFile);
        g_ulSysAlarmFileSize += writeLen;
        fflush(g_pstSysAlarmFile);
        vosSnprintf(divider, STR_SIZE_128_BYTES, "------------------- -------------------------------------\r\n");
        writeLen = fwrite(divider, 1, strlen(divider), g_pstSysAlarmFile);
        g_ulSysAlarmFileSize += writeLen;
        fflush(g_pstSysAlarmFile);
    } */
    vosMutexGive(&g_pstSysAlarmFileMutex);
#endif /* NO_FILEIO */
    return NO_ERROR;
}

UINT32 sysAlarmFileClose(void)
{
#ifndef NO_FILEIO
    vosMutexTake(&g_pstSysAlarmFileMutex);
    if (!g_acSysAlarmFileName) {
        vosMutexGive(&g_pstSysAlarmFileMutex);
        return INVALID_32;
    }
    if (g_pstSysAlarmFile) {
        vosFClose(g_pstSysAlarmFile);
        g_pstSysAlarmFile = NULL;
    }
    vosMutexGive(&g_pstSysAlarmFileMutex);
#endif
    return NO_ERROR;
}

UINT32 sysAlarmFileAppend(const char *buf)
{
    int	retv = 0;
    int nbytes = vosStrLen(buf);
#ifndef NO_FILEIO
    if (nbytes + g_ulSysAlarmFileSize > MAX_ALARM_FILE_SIZE)
    {
        sysAlarmFileClose();
        vosMutexTake(&g_pstSysAlarmFileMutex);
        retv = vosRename(g_acSysAlarmFileName, g_acSysAlarmBakFileName);
        vosMutexGive(&g_pstSysAlarmFileMutex);
        if (retv == -1)
            return (UINT32)retv;
        
        retv = sysAlarmFileClear();
        if (retv == -1)
            return (UINT32)retv;
    }
    vosMutexTake(&g_pstSysAlarmFileMutex);
    if (g_pstSysAlarmFile) {
        retv = fwrite(buf, 1, nbytes, g_pstSysAlarmFile);
        g_ulSysAlarmFileSize += retv;
        fflush(g_pstSysAlarmFile);
    } else
        retv = 0;

    vosMutexGive(&g_pstSysAlarmFileMutex);
#endif /* !NO_FILEIO */
    return(retv);
}


UINT32 sysAlarmFileClear()
{
    UINT32	retv = 0;
#ifndef NO_FILEIO
    /* Truncate file to zero length or create text file for writing. 
    The stream is positioned at the beginning of the file. */
    retv = sysAlarmFileOpen(0);
    if (retv == INVALID_32)
        return retv;

    /* reopen the log file */
    retv = sysAlarmFileOpen(1);
    if (retv == INVALID_32)
        return retv;

    vosMutexTake(&g_pstSysAlarmFileMutex);
    g_nSysAlarmCount = 0;
    odmAlarmLedControl(FALSE);
    vosRemove(g_acSysAlarmBakFileName);
    vosMutexGive(&g_pstSysAlarmFileMutex);

#endif /* !NO_FILEIO */
    return(retv);
}


UINT32 sysLogShow(ENV_t *pstEnv)
{
    int i;
    FILE *pstLogFile = NULL;
    char *pcLine;

    vosMutexTake(&g_pstSysLogFileMutex);

    if ((pstLogFile = vosFOpen(g_acSysLogBakFileName, "rb")) != NULL) 
    {
        while (!feof(pstLogFile)) {
            pcLine = vosFGetWholeLine(pstLogFile);
            if (pcLine == NULL)
                continue;
            vosPrintf(pstEnv->nWriteFd, pcLine);
            vosPrintf(pstEnv->nWriteFd, "\n");
            vosFree(pcLine);
        }
    }

    if ((pstLogFile = vosFOpen(g_acSysLogFileName, "rb")) != NULL) 
    {
        while (!feof(pstLogFile)) {
            pcLine = vosFGetWholeLine(pstLogFile);
            if (pcLine == NULL)
                continue;
            vosPrintf(pstEnv->nWriteFd, pcLine);
            vosPrintf(pstEnv->nWriteFd, "\n");
            vosFree(pcLine);
        }
    }

    vosMutexGive(&g_pstSysLogFileMutex);

    vosPrintf(pstEnv->nWriteFd, "\r\n");
}


UINT32 sysAlarmShow(ENV_t *pstEnv)
{
    int i;
    FILE *pstAlarmFile = NULL;
    char *pcLine;

    vosMutexTake(&g_pstSysAlarmFileMutex);

    if ((pstAlarmFile = vosFOpen(g_acSysAlarmBakFileName, "rb")) != NULL) 
    {
        while (!feof(pstAlarmFile)) {
            pcLine = vosFGetWholeLine(pstAlarmFile);
            if (pcLine == NULL)
                continue;
            vosPrintf(pstEnv->nWriteFd, pcLine);
            vosPrintf(pstEnv->nWriteFd, "\n");
            vosFree(pcLine);
        }
    }

    if ((pstAlarmFile = vosFOpen(g_acSysAlarmFileName, "rb")) != NULL) 
    {
        while (!feof(pstAlarmFile)) {
            pcLine = vosFGetWholeLine(pstAlarmFile);
            if (pcLine == NULL)
                continue;
            vosPrintf(pstEnv->nWriteFd, pcLine);
            vosPrintf(pstEnv->nWriteFd, "\n");
            vosFree(pcLine);
        }
    }

    vosMutexGive(&g_pstSysAlarmFileMutex);

    vosPrintf(pstEnv->nWriteFd, "\r\n");
}



UINT32 sysLogCfgShow(ENV_t *pstEnv)
{
    int i;
    
    vosPrintf(pstEnv->nWriteFd, "Log Configuration:\r\n");
    vosPrintf(pstEnv->nWriteFd, 
        "    %-20s : %s\r\n", 
        "Log File", 
        STATE_STR_GET(sysLogGetLogEnable()));
    
    for (i = 0; i < MAX_MODULE; i++) 
    {
        vosPrintf(pstEnv->nWriteFd, 
            "    %-20s : %-10s - %s\r\n", 
            "Debug Level", 
            g_pcModule[i], 
            g_pcDebugLevel[cliSessionDebugLevelGet(pstEnv->ucIndex, i)]);
    }

    vosPrintf(pstEnv->nWriteFd, "\r\n");
}


