#ifdef  __cplusplus
extern "C"{
#endif
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"

#include "lw_if_pub.h"
#include <lw_config_api.h>
#include <lw_config_oid.h>
#include <sys/types.h>			/* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <sys/ioctl.h>



static int get_sys_mac(char *ifname, mac_address_t *mac)
{
        int sock, ret;
        struct ifreq ifr;

        sock = socket(AF_INET, SOCK_STREAM, 0);
       // DBG_ASSERT(sock >= 0, "create socket: %s", strerror(errno));
        if(sock < 0) return -1;

        memset(&ifr, 0, sizeof(struct ifreq));
        strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name) - 1);
        ret = ioctl(sock, SIOCGIFHWADDR, &ifr);
       // DBG_ASSERT(ret >= 0, "ioctl: %s", strerror(errno));
        if(ret < 0) return -1;

        memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof(mac_address_t));
        close(sock);
        return 0;
}


DRV_RET_E Drv_VlanIfDel(vlan_id_t tdVid)
{
    DRV_RET_E iRet = DRV_OK;
    mac_address_t mac_addr;                                 
    vlan_id_t tdMVID = 0;
	Ioctl_DelVlanIntf(tdVid);
    return DRV_OK; /*ÔÝÊ±²»·µ»Ø´íÎó*/
}

DRV_RET_E Drv_VlanIfCreate(vlan_id_t tdVid)
{       
    mac_address_t mac_addr; 
	char mac[64]={0};
    vlan_id_t tdMngVlan = 0;
    DRV_RET_E iRet = DRV_OK;
//	cfg_getval(IF_TEMP_IFINDEX,CONFIG_LAN_MAC_ADDR,mac,"",sizeof(mac));
	
	get_sys_mac("eth0", mac_addr);
	//printf("mac=%s",mac);
    Ioctl_AddVlanIntf(tdVid,mac_addr);
    return iRet;
}

DRV_RET_E Drv_GetVlanStatus(vlan_id_t uiVid, UINT32 * puiStatus)
{
   
	logic_pmask_t  lportList;    

	port_num_t uiPortNum =0;
//	printf("%s %d\n",__FUNCTION__,__LINE__);
    if(uiVid!=0)
    {
    
    	ClrLgcMaskAll(&lportList);
    	(VOID)Drv_GetVlanMember(uiVid, &lportList);    
	//	printf("%02x\n",lportList.pbits[0]);
    	if(FALSE == LgcMaskNotNull(&lportList))
    	{
    		*puiStatus = DOWN;
    		return DRV_OK;
    	}
		//printf("%02x @\n",lportList.pbits[0]);
    	LgcPortFor(uiPortNum)
    	{    	   
    	    if(!TstLgcMaskBit(uiPortNum, &lportList))
    		{
    	        continue;
    		}
    		
    		if(TRUE == Drv_IsLinkUpPort(uiPortNum))
    	    {
    		     break;
    		}
    	}    	
    	if(LOGIC_PORT_NO >= uiPortNum)
    	{
    		*puiStatus = UP;
    		return DRV_OK;
    	}   
		*puiStatus = DOWN;
		if(LOGIC_PON_PORT!=INVALID_PORT)
		{
			if(TRUE == Drv_IsLinkUpPort(LOGIC_PON_PORT))
    	    {
    		     *puiStatus = UP;
    		}
		}
		
    }
    else  //vid ==0 for EPN
    {      
      
    	for(uiPortNum = 1; uiPortNum <= LOGIC_PORT_NO; uiPortNum++)
    	{
    		
    		if(TRUE == Drv_IsLinkUpPort(uiPortNum))
    	    {
    		     break;
    		}
    	}
    	if(LOGIC_PORT_NO >= uiPortNum)
    	{
    		*puiStatus = UP;
    		return DRV_OK;
    	}
        *puiStatus = DOWN;
    	if(LOGIC_PON_PORT!=INVALID_PORT)
		{
			if(TRUE == Drv_IsLinkUpPort(LOGIC_PON_PORT))
    	    {
    		     *puiStatus = UP;
    		}
		}
    }
	return DRV_OK;
}


#ifdef  __cplusplus
}
#endif

