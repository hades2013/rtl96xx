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
* FILENAME:  log.h
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/h/log.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __OP_SYS_LOG_H_
#define __OP_SYS_LOG_H_

#include "defs.h"
#include "cli_engine.h"
#include "list_opconn.h"

#ifdef __cplusplus
extern "C" {
#endif

UINT32 sysLogInit(void);
UINT32 sysLogShutdown(void);

/* alarm message will be broadcasted to each session and write to file */
UINT32 sysLogAlarm(char *fmt, ...);
UINT32 sysLogAlarmDecrease(int nNum);

/* broadcast message will be broadcasted to each session */
UINT32 sysLogBroadcast(char *fmt, ...);

/* debug message will be broadcasted to the session which was opened debug level */
UINT32 sysLogDebug(UINT8 ucModule, UINT8 ucLevel, char *fmt, ...);
UINT32 sysLogDebugRawData(UINT8 ucModule, UINT8 ucLevel, char *fmt, ...);

/* enable log */
UINT32 sysLogSetLogEnable(UINT8 ucEnableLog);
UINT8 sysLogGetLogEnable();

/*syslog server related fucntions */
UINT32 sysLogSetServerLogEnable(UINT8 ucEnable);
UINT8 sysLogGetServerLogEnable();
UINT32 sysLogServerInit();
UINT32 sysLogServerDestroy();
UINT32 sysLogSetServerLogAddress(UINT32 ucSyslogAddress,UINT16 ucLogPort);
UINT32 sysLogDelServerLogAddress(UINT32 ucSyslogAddress);
UINT32 sysLogServerFileClear();

/* clear file content */
UINT32 sysLogFileClear();
UINT32 sysAlarmFileClear();

/* show */
UINT32 sysLogShow(ENV_t *pstEnv);
UINT32 sysLogCfgShow(ENV_t *pstEnv);
UINT32 sysAlarmShow(ENV_t *pstEnv);

#ifdef __cplusplus
}
#endif
typedef struct SYSLOG_SERVER_t
{
	struct list_head list;
	UINT32 logserverIp;
	UINT16 logServerPort;
	//UINT16 logFacility;
	//UINT16 logSeverity;
	UINT16 logServerStatus;
	UINT32 ServerNum;
}tSyslogServer;

#define   	SYSLOG_LEVEL_EMERGENCY 0
#define     SYSLOG_LEVEL_ALARM 1
#define     SYSLOG_LEVEL_CRITICAL 2
#define     SYSLOG_LEVEL_ERROR 3
#define     SYSLOG_LEVEL_DEBUG_WARNING 4
#define     SYSLOG_LEVEL_NOTICE 5
#define     SYSLOG_LEVEL_INFORMATIONAL 6
#define     SYSLOG_LEVEL_DEBUG  7 
 
#define	SYSLOG_FAC_KERN	(0<<3)	/* kernel messages */
#define	SYSLOG_FAC_USER	(1<<3)	/* random user-level messages */
#define	SYSLOG_FAC_MAIL	(2<<3)	/* mail system */
#define	SYSLOG_FAC_DAEMON	(3<<3)	/* system daemons */
#define	SYSLOG_FAC_AUTH	(4<<3)	/* security/authorization messages */
#define	SYSLOG_FAC_SYSSYSLOG_FAC	(5<<3)	/* messages generated internally by syslogd */
#define	SYSLOG_FAC_LPR		(6<<3)	/* line printer subsystem */
#define	SYSLOG_FAC_NEWS	(7<<3)	/* network news subsystem */
#define	SYSLOG_FAC_UUCP	(8<<3)	/* UUCP subsystem */
#define	SYSLOG_FAC_CRON	(9<<3)	/* clock daemon */
#define	SYSLOG_FAC_AUTHPRIV	(10<<3)	/* security/authorization messages (private) */
#define	SYSLOG_FAC_FTP		(11<<3)	/* ftp daemon */
/* other codes through 15 reserved for system use */
#define	SYSLOG_FAC_LOCAL0	(16<<3)	/* reserved for local use */
#define	SYSLOG_FAC_LOCAL1	(17<<3)	/* reserved for local use */
#define	SYSLOG_FAC_LOCAL2	(18<<3)	/* reserved for local use */
#define	SYSLOG_FAC_LOCAL3	(19<<3)	/* reserved for local use */
#define	SYSLOG_FAC_LOCAL4	(20<<3)	/* reserved for local use */
#define	SYSLOG_FAC_LOCAL5	(21<<3)	/* reserved for local use */
#define	SYSLOG_FAC_LOCAL6	(22<<3)	/* reserved for local use */
#define	SYSLOG_FAC_LOCAL7	(23<<3)	/* reserved for local use */

#define SYSLOG_DEFAULT_PORT 514
#define SYSLOG_CONFIG_NAME  "/cfg/syslog.conf"
#define SYSLOG_CONFIG_SERVER "IPAddress"
#define SYSLOG_CONFIG_PORT "Port"
#define SYSLOG_CONFIG_GLOBAL "Global"
#define SYSLOG_CONFIG_GLB_STATUS "Status"
#define SYSLOG_SERVER_SECTION "Syslog%d"

/* to use OP_DEBUG please #define MODULE at the beginning of 
 * your c file, for isolating each module debug info 
 */
#define OP_DEBUG(LEVEL, FMT, ...) \
    do { \
        sysLogDebug(MODULE, LEVEL, FMT, ##__VA_ARGS__); \
    } while (0)

#define OP_DEBUG_RAW(LEVEL, FMT, ...) \
    do { \
        sysLogDebugRawData(MODULE, LEVEL, FMT, ##__VA_ARGS__); \
    } while (0)

#define OP_ALARM(FMT, ...) \
    do { \
        sysLogAlarm(FMT, ##__VA_ARGS__); \
    } while (0)

#define OP_BCAST(FMT, ...) \
    do { \
        sysLogBroadcast(FMT, ##__VA_ARGS__); \
    } while (0)


#endif /* #ifndef __OP_SYS_LOG_H_ */

