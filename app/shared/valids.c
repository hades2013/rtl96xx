
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time_zone.h>
#include "valids.h"


int valid_unicast_mac(ethernet_addr_t *addr)
{
	const unsigned char zero_addr[ETHER_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	const unsigned char bcast_addr[ETHER_ADDR_LEN] = {0xff, 0xff, 0xff, 0xff,0xff, 0xff};

	if (!memcmp(addr->octet, zero_addr, sizeof(ethernet_addr_t))){
		return 0;
	}
		
	if (!memcmp(addr->octet, bcast_addr, sizeof(ethernet_addr_t))){
		return 0;
	}

	if (addr->octet[0] & 0x01){
		return 0;
	}
	return 1;
}


int valid_time_zone(int tz)
{
	return in_range(tz, 0, time_zone_num() - 1);
}



int valid_ip(struct in_addr ip)
{
	int fa;
	
	fa = (int)((ip.s_addr >> 24) & 0xff);

	if ((fa > 223) || (fa == 0) || (fa == 127)){
		return 0;
	}

	return 1;
}

int valid_subnet_mask(struct in_addr ip, struct in_addr mask)
{/*
	unsigned long addr = mask.s_addr;
	int i;
	for (i = 0; i < 32; i ++){
		if (!(addr & (1 << i))){ 
			break;
		}
	}
	printf("\nmask:0x%x, i:%d\n",addr, i);

	// 0000FFFF &  ~(0000FFFF)  
	if ((addr & ~((1 << i) - 1))){
		
		printf("shift:0x%x\n",(~((1 << i) - 1)));
		return 0;
	}

	if (i == 0 || i == 32){
		
		printf("0 | 30$$i:%d\n",i);
		return 0;
	}
	printf("OK\n");
	return 1;
	*/

//modify by zhouguanhua 2012-12-3 of EPN204QID00011
   unsigned long addr = mask.s_addr;
    if((0==addr)||(addr==0xffffffff)) //ии?0oикии?1
    {
        return 0;   
    }
//modify by zhouguanhua 2012-12-3 of EPN204QID00011

   /*Begin add by huangmingjian 2013-08-30*/
   unsigned long umask = ~mask.s_addr;
   unsigned long result = (ip.s_addr) & umask;
   if(0 == result || umask == result)
   {
	 return 0;
   }
   /*Begin add by huangmingjian 2013-08-30*/

   addr = ~addr +1;
   if((addr & (addr-1)) ==0)
   	{ 
   	  return 1;
   	}
   return 0;
}


int valid_gateway(struct in_addr ip, struct in_addr mask, struct in_addr gw)
{
	if (!valid_ip(gw)){
		return 0;
	}
	
	if ((ip.s_addr & mask.s_addr) != (gw.s_addr & mask.s_addr)){
		return 0;
	}
	
	return 1;
}


int valid_hostname(char *v)
{
	int phase = 0;
/*
	phase 0, hostname
	phase 1, domain
*/
	if (v == NULL){
		return 0;
	}

	do{
		//the first [a-zA-Z0-9] @ phase 0
		if ((phase == 0) && !isalnum(*v)){
			return 0;
		}
		//the first [a-zA-Z] @ phase 1
		if ((phase == 1) && !isalpha(*v)){
			return 0;
		}

		v ++;
		while(*v && (*v != '.')){
			if (!isalnum(*v) && (*v != '-') ){
				return 0;
			}
			v ++;
		}

		// the last one can not be '-'	
		v --;
		if (*v == '-'){
			return 0;
		}
		v ++;

		if (*v == '.'){
			if (phase == 0){
				phase ++;
				v ++;
			}else {
				return 0;
			}
		}else {
			break;
		}
		
	}while(phase < 2);
	
	return 1;
}



