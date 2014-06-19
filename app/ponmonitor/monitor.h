/*****************************************************************************
------------------------------------------------------------------------------

   Description: monitor header file 

------------------------------------------------------------------------------
                                                                             
*****************************************************************************/
#ifndef _BGMONITOR_H_
#define _BGMONITOR_H_

#ifdef  __cplusplus
extern "C"{
#endif

/*debug*/
#define MONITOR_SYS_DBG 0
#if (MONITOR_SYS_DBG)
#define MONITOR_SYS_ERR(fmt, ...) do { fprintf(stderr, "error-[%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\r\n"); } while(0)	
#define MONITOR_PARAM_ERR(fmt, ...) do { fprintf(stderr, "invalid input param-[%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\r\n"); } while(0)
#define MONITOR_DBG_ERR(fmt, ...) do { fprintf(stderr, "err-[%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\r\n"); } while(0)
#define MONITOR_DBG_INFO(fmt, ...) do { fprintf(stderr, "info-[%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\r\n"); } while(0)
#else
#define MONITOR_SYS_ERR(fmt, ...)
#define MONITOR_PARAM_ERR(fmt, ...)
#define MONITOR_DBG_ERR(fmt, ...)
#define MONITOR_DBG_INFO(fmt, ...)
#endif
//#define MONITOR_DEBUG
#ifdef MONITOR_DEBUG
#define MONITOR_PRINTF  do { fprintf(stderr, "[%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\r\n"); } while(0)
#define MONITOR_TRACE() MONITOR_PRINTF
#else
#define MONITOR_PRINTF(...) 
#define MONITOR_TRACE()
#endif

/** @def MONITOR_MALLOC_STRUCT(s)
    Mallocs memory of sizeof(struct s), zeros it and returns a pointer to it. */
#define MONITOR_MALLOC_STRUCT(s)   (struct s *) calloc(1, sizeof(struct s))

#define MONITOR_FREE(s)    do { if (s) { free((void *)s); s=NULL; } } while(0)



/*function prototype*/
extern void cfglogtime_handler(const char *token, char *cptr);
extern void cputhreshold_handler(const char *token, char *cptr);
extern void memthreshold_handler(const char *token, char *cptr);
extern UINT32 Monitor_SetTimer(UINT32 uiInteval, INT32 (*func)(struct thread *));

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif  /* end of _MONITOR_H_ */
