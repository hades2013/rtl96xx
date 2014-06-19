#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>

/*Begin modified by huangmingjian 2012/09/21 for EPN104QID0046*/  

//#define	DEBUG_SOURCE	1

#ifdef	DEBUG_SOURCE
#define	DBG_PRINTF(fmt, ...) do { fprintf(stderr, "DBG [%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\r\n"); } while(0)
#define	DBG_EXIT(fmt, ...) do { fprintf(stderr, "EXIT [%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\r\n"); while(1);} while(0)

#define DBG_ASSERT(bool, fmt, ...) do { if(!(bool)) { fprintf(stderr, "\nFAIL [%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\r\n"); } } while(0)
#else
#define	DBG_PRINTF
#define DBG_ASSERT
#endif

#define	ERR_PRINTF(fmt, ...) while(0) {\
			fprintf(stderr, "ERR[%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); \
			fprintf(stderr, "\r\n"); \
	}

#define	ERR_NO_MEM()	ERR_PRINTF("No enough memory!")

//#define DBG_ASSERT(bool, fmt, ...) do { if(!(bool)) { fprintf(stderr, "\nFAIL [%s,%s,%d]: " fmt, __FILE__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\r\n"); } } while(0)
/*End modified by huangmingjian 2012/09/21 for EPN104QID0046 */

#endif /* DEBUG_H_ */
