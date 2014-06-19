#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rtk_api.h"

int main(int argc, char **argv)
{
	if(argc < 2) {
		printf("rtl8367 arguments...\n");
		exit(1);
	}
	if(!strcmp(argv[1], "init")) {
		printf("Init LED...%d\n", rtk_led_operation_set(LED_OP_SERIAL));
	} 
	else if(!strcmp(argv[1], "test")) {
		printf("Init LED...%d\n", rtk_led_operation_set(LED_OP_PARALLEL));
	}
	return 0;
}
