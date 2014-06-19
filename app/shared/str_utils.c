/*
 * str_utils.c
 *
 *  Created on: Jan 15, 2011
 *      Author: root
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hex_utils.h"


#include "basetype.h"


int split_string(char *s, char tok, char **ret)
{
	char *cp;
	int n = 0;

	cp = s;
	if(!*cp) return n;
	n = 1;
	while(*cp) {
//		printf("CP: %s\n", cp);
		ret[n - 1] = cp;
		while(*cp && *cp != tok) cp ++;
		if(*cp == tok) n = n + 1;
		if(*cp == 0) return n;
		*cp = 0;
		cp ++;
	}
	ret[n - 1] = cp;
	return n;
}


char *safe_strncpy(char *dest, const char *src, size_t n)
{
	strncpy(dest, src, n - 1);
	dest[n - 1] = '\0';
	return dest;
}

char *mac2str(ethernet_addr_t *mac)
{
	static char macstr[64];
	hexdecode(mac->octet, sizeof(ethernet_addr_t), macstr, sizeof(macstr));
	return macstr;
}

static int valid_hfid_char(char c)
{
	if (isalpha(c) || isdigit(c) || (c == '-') || (c == '_') || (c == ' ')){
		return 1;
	}
	return 0;
}

char hex2ascii(unsigned char c)
{
	c = c & 0x0f;
	if (c >= 0x0a){
		return c - 10 + 'A';
	}else {
		return c + '0';
	}
}

char *trim_left(char *str)
{
	while(str && *str && (*str == ' ')) str ++;
	return str;
}

char *trim_right(char *str)
{
	while(str && *str && (str[strlen(str) - 1] == ' ')) str[strlen(str) - 1] = 0;
	return str;
}

char *hfid2str(const char *hfid)
{
	static char buffer[128];
	int i;
	int j = 0;
	for (i = 0; (i < 32) && (hfid[i]) ; i ++){// only display the first 32 codes
		if (valid_hfid_char(hfid[i])){
			buffer[j ++] = hfid[i];
		}else {
			buffer[j ++] = '%';
			buffer[j ++] = hex2ascii(hfid[i] >> 4);
			buffer[j ++] = hex2ascii(hfid[i] & 0x0f);
		}
	}
	buffer[j] = '\0';
	if (j == 0){// empty
		strcpy(buffer, "null");
	}
	
	return buffer;
}



#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>


#define __swap16(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff))
#define __swap32(l) (((l) >> 24) | \
  (((l) & 0x00ff0000) >> 8)  | \
  (((l) & 0x0000ff00) << 8)  | \
  ((l) << 24))
  /*
#define __swap64(ll) (((ll) >> 56) |\
  (((ll) & 0x00ff000000000000) >> 40) |\
  (((ll) & 0x0000ff0000000000) >> 24) |\
  (((ll) & 0x000000ff00000000) >> 8) |\
  (((ll) & 0x00000000ff000000) << 8) |\
  (((ll) & 0x0000000000ff0000) << 24) |\
  (((ll) & 0x000000000000ff00) << 40) |\
  (((ll) << 56)))
*/

uint64_t __swap64(uint64_t llong)
{
	uint32_t low = (uint32_t) (llong & 0x00000000FFFFFFFFLL);
	uint32_t high = (uint32_t) ((llong & 0xFFFFFFFF00000000LL) >> 32);
	low = __swap32(low);
	high = __swap32(high);
	llong = high + (((uint64_t)low) << 32);
	return llong;
}

uint64_t ntohl64(uint64_t llong)
{

#if __BYTE_ORDER==__LITTLE_ENDIAN
	return __swap64(llong);
#else
 	return llong;
#endif
}  


uint64_t htonl64(uint64_t llong)
{
#if __BYTE_ORDER==__LITTLE_ENDIAN
	return __swap64(llong);
#else
	return llong;
#endif

} 



uint64_t letohl64(uint64_t llong)
{
#if __BYTE_ORDER==__LITTLE_ENDIAN
	return llong;
#else
	return __swap64(llong);
#endif 
}

uint16_t letohs(uint16_t s)
{
#if __BYTE_ORDER==__LITTLE_ENDIAN
	return s;
#else
	return __swap16(s);
#endif 
}

/*****************************************************************
Function:str2index
Description: str2index
Author:huangmingjian
Date:2013/10/05
INPUT:    
OUTPUT:         
Return:
Others:	 find the index according  to the location in  *str_array[]    
*****************************************************************/
uint16_t str2index(char *str_array[], char *str) 		
{	
	uint16_t index = 0;	
	int found = 0;
	
	if(str_array==NULL || str==NULL)	
	{		
		fprintf(stderr ,"Str(%s) is null\n", str);		
		return 0;	
	}		
	while(str_array[index])	
	{			
		if(0==strcmp(str, str_array[index]))		
		{	
			found = 1;			
			break;		
		}		
		index++;	
	}
	
	if(!found)	
	{		
		fprintf(stderr ,"Can not get str(%s) index\n", str);		
	}
	
	return (index = found?index:0);
}


