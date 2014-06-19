/*
 * mctrl.c
 *
 */
#include <stdio.h>

#include <rtk_api.h>
#include <rtk_api_ext.h>

 
void switch_init(void)
{
	rtk_led_operation_set(LED_OP_SERIAL);
	rtk_vlan_init();
	rtk_filter_igrAcl_init();
}


/*
  mctrl disable all
  mctrl enable all
  mctrl enable 0
  mctrl enable 1
*/


void port_isolate(int en)
{
	rtk_portmask_t mask0, mask1;	

	switch_init();
	
	mask0.bits[0] = RTK_MAX_PORT_MASK;
	mask1.bits[0] = RTK_MAX_PORT_MASK;

	if (en == 1){
		mask1.bits[0] = RTK_MAX_PORT_MASK & (~((1 << 3) | (1 << 4)));
	}else if (en == 2){
		mask0.bits[0] = RTK_MAX_PORT_MASK & (~((1 << 3) | (1 << 4)));
	}
	
	rtk_port_isolation_set(RTK_EXT_0_MAC, mask0);
	rtk_port_isolation_set(RTK_EXT_1_MAC, mask1);	
}

int main(int argc, char **argv)
{

	char c;
/*
IMPORTANT: 
	port_isolate(1);
*/
	port_isolate(1);

	do {
		fprintf(stderr, "Please make a choice:\n");
		fprintf(stderr, " 1 : Burning MAC for Cable 1 \n");
		fprintf(stderr, " 2 : Burning MAC for Cable 2 \n");
		fprintf(stderr, " q : Exit this program \n");
		fprintf(stderr, "Input : ");
		
		do{
		c = getchar();		
		}while(c !='1' && c != '2' && c != 'q');

		if (c == '1'){
			port_isolate(1);
			fprintf(stderr, "\n MAC burning for Cable 1 ready.\n\n");
		}else if (c == '2'){
			port_isolate(2);
			fprintf(stderr, "\n MAC burning for Cable 2 ready.\n\n");			
		}
		
		
	}while(c != 'q');

	port_isolate(0);// enable all

	return 0;
}




