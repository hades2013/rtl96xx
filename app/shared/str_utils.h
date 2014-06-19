/*
 * str_utils.h
 *
 *  Created on: Jan 15, 2011
 *      Author: root
 */

#ifndef STR_UTILS_H_
#define STR_UTILS_H_

#include <basetype.h>

int split_string(char *s, char tok, char **ret);
char *safe_strncpy(char *dest, const char *src, size_t n);
char *mac2str(ethernet_addr_t *mac);
char *hfid2str(const char *hfid);
char *trim_left(char *str);
char *trim_right(char *str);


uint64_t ntohl64(uint64_t llong);
uint64_t htonl64(uint64_t llong);
uint64_t letohl64(uint64_t llong);

uint16_t letohs(uint16_t s);




#endif /* STR_UTILS_H_ */
