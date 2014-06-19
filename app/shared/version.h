
#ifndef __VERSION_H__
#define __VERSION_H__



typedef struct {
	char *(*toString)(void);
	uint32_t (*toInt)(void);
	char *(*toDate)(void);
}
version_t;




#endif /*#ifndef __VERSION_H__*/

