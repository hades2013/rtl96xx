#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>
#include <errno.h>

//#define DEBUG 0	
/*Begin modified by huangmingjian 2012/09/21 for EPN104QID0046*/  

#ifdef DEBUG
#define DBG_PRINTF(fmt, ...) do { fprintf(stderr, "[%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\n"); } while(0)
#define DBG_ASSERT(bool, fmt, ...) do { if(!(bool)) { fprintf(stderr, "\nFAIL [%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\r\n"); } } while(0)

#else
#define DBG_PRINTF(fmt, ...)
#define DBG_ASSERT(fmt, ...)
#endif

#define ERR_PRINTF(fmt, ...) do { fprintf(stderr, "[%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\n"); syslog(LOG_ERR, fmt, ##__VA_ARGS__); } while(0)

//#define DBG_ASSERT(bool, fmt, ...) do { if(!(bool)) { fprintf(stderr, "\nFAIL [%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\r\n"); } } while(0)

#define	ERR_LOG(fmt, ...) do { syslog(LOG_ERR, fmt, ##__VA_ARGS__); fprintf(stderr, "[INFO]: " fmt "\n", ##__VA_ARGS__); } while(0)
#define	INFO_LOG(fmt, ...) do { syslog(LOG_INFO, fmt, ##__VA_ARGS__); fprintf(stderr, "[INFO]: " fmt "\n", ##__VA_ARGS__); } while(0)
/*End modified by huangmingjian 2012/09/21 for EPN104QID0046 */

/*Begin add by zhouguanhua 2013/04/7 */
/*
module_name--模块名称
priority--消息的级别，取值如下:
            LOG_EMERG          
            LOG_ALERT
            LOG_CRIT  关键状态的警告。例如，硬件故障；
            LOG_ERR
            LOG_WARNING
            LOG_NOTICE
            LOG_INFO   通报信息
            LOG_DEBUG
fmt, ... -----消息内容

*/
//#define	USER_LOG(module_name,priority,fmt, ...) do {openlog(module_name,LOG_CONS ,LOG_USER); syslog(priority, fmt";%s",module_name);closelog();fprintf(stderr, "[USER]: " fmt "\n", ##__VA_ARGS__); } while(0)
												
/*End begin add  by zhouguanhua 2013/04/7    last edit by leijinbao 2013/08/27/*/
#endif /*DEBUG_H_*/
