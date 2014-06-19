/*
 * utils.h
 *
 *  Created on: Dec 29, 2010
 *      Author: root
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <basetype.h>

#define MAX_USER_NUM 		4
#define ENABLE_LOGIN_AUTH  1

typedef struct {
	char name[16];
	char password[16];
	uint32_t ip_addr;
	uint32_t timeout;
	uint32_t logout_time;
//	unsigned char listenif;
}userInfo_t;



int set_if_mac(char *ifname, ethernet_addr_t *mac);
int get_if_mac(char *ifname, ethernet_addr_t *mac);
int create_raw_socket(char *ifname, int *ifindex);
uint32_t checksum_32 (register const void * memory, register size_t extent, register uint32_t checksum);

#endif /* UTILS_H_ */
