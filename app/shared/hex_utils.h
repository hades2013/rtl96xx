/*
 * hex_utils.h
 *
 *  Created on: Dec 29, 2010
 *      Author: root
 */

#ifndef HEX_UTILS_H_
#define HEX_UTILS_H_

unsigned char todigit (unsigned char c);
void hexdump (const void * memory, size_t offset, size_t extent);
size_t hexdecode (const unsigned char memory [], size_t extent, char buffer [], size_t length);
char * hexstring (char buffer [], size_t length, unsigned char memory [], size_t extent);
size_t hexencode (unsigned char memory [], size_t extent, const char * string);

#endif /* HEX_UTILS_H_ */
