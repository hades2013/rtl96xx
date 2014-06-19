/*
 * basetype.h
 *
 *  Created on: Dec 29, 2010
 *      Author: root
 */

#ifndef BASETYPE_H_
#define BASETYPE_H_

#include <stdint.h>
#include <net/ethernet.h>

typedef struct ethernet_addr_s {
	uint8_t octet[ETHER_ADDR_LEN];
} ethernet_addr_t;

#endif /* BASETYPE_H_ */
