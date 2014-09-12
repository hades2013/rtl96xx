
#include <stdio.h>
#include <string.h>

#include <lw_type.h>
#include <lw_config.h>
#include <lw_drv_pub.h>
#include <lw_drv_req.h>
#include "../../kernel/linux-2.6.x/drivers/net/rtl86900/sdk/include/rtk/port.h"

int main(int argc, char *argv[])
{
	int ret , i , tx = 0, rx = 0;
	logic_pmask_t mask;

	memset(&mask, 0, sizeof(mask));

	printf("I am in mytest\n");
	
	ret = Ioctl_GetPortLinkMask(&mask);

	printf("call Ioctl_GetPortLinkMask() return %d, mask = %08X\n", ret, mask.pbits[0]);
	
//	printf("== Config EOC RGMII Delay! By Alan Lee.\n");
//	printf("tx =  ( 0 - 1 ), rx = ( 0 - 7 )\n");
//	scanf("%d %d",&tx,&rx);
//	printf("OK ,tx = %d ( 0 - 1 ), rx = %d ( 0 - 7 )\n",tx,rx);
	(void)rtk_port_macExtRgmiiDelay_set(5, 0, 3);/*rxdelayÐèÒªµ÷ÊÔ*/
	

	
		
	return 0;	
}
