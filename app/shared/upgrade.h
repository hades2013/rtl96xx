
#ifndef __UPGRADE_H__
#define __UPGRADE_H__

#include "ufile.h"


typedef enum{
	UP_OK = 0,
	UP_FAILED, 
	UP_ERR_LENGTH,
	UP_ERR_IDENTIFY,
	UP_ERR_CHKSUM,
	UP_ERR_IO,
	UP_ERR_NOMEM,
	UP_ERR_NONAME,
	UP_ERR_IPC,
	UP_ERR_UNKNOWN
}
upgrade_error_t;


int ufile_validate(const uint8_t *buffer, uint32_t length);
int ufile_process(const uint8_t *buffer, uint32_t length);


#endif /*#ifndef __UPGRADE_H__*/

