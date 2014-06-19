#include "master.h"
#include "timer.h"
#include <ipc_protocol.h>
#include "build_time.h"
//#include "switch.h"
#include <shutils.h>
#include <str_utils.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time_zone.h>
#include "utils.h"
#include "sys_upgrade.h"
#include <drv_wtd_user.h>
#include <lw_type.h>
#include <lw_config_oid.h>
#include <lw_config_api.h>
#include <lw_if_pub.h>
#include <if_notifier.h>
#include "ufile_process.h"






version_t sys_version = {version_string, version_count, version_date};

void sys_info_load(sys_info_t *info)
{
	int invalid = 0;
	char val[64]={0};
	//char nvname[CFG_NAME_SIZE];
    char kernel_version[MAX_BUILDTIME_SIZE];
    char kernel_build_time[MAX_BUILDTIME_SIZE];
	char tmp_var[MAX_BUILDTIME_SIZE];

	
	info->uptime = 0;

	#if 0
	for (i = 0; i < MAX_CLT_CHANNEL; i ++){
		sprintf(nvname, "port_mac_addr_cab%d", i);
		val = nvram_safe_get(nvram_handle, nvname);
		if (val[0]){
			ret = hexencode(info->cabmac[i].octet, sizeof(ethernet_addr_t), val);
			if (ret != sizeof(ethernet_addr_t)) {
				invalid ++;
				DBG_ASSERT(1, "Invalid MAC address");
				continue;
			}
		}
	}
	val = nvram_safe_get(nvram_handle, "vlan_man_vid");
	if (val[0]){
		info->mvlan = strtoul(val, NULL, 0);
	}
	#endif
	
    /*begin modified by liaohongjun 2012/9/18 of QID0032*/
    #if 0
	val = nvram_safe_get(nvram_handle, "lan_mac_addr");
	if (!val[0] || (hexencode(info->sysmac.octet, sizeof(ethernet_addr_t), val) != sizeof(ether_addr_t)) ){
		//invalid ++;
	}	
	#endif

	
	if ((get_if_mac(SYS_NETIF_NAME, &(info->sysmac))) < 0){
		invalid ++;
	} 
    /*end modified by liaohongjun 2012/9/18 of QID0032*/
	safe_strncpy(info->sw_version, sys_version.toString(), sizeof(info->sw_version));
	safe_strncpy(info->buildtime, sys_version.toDate(), sizeof(info->buildtime));


    /*Add by zhouguanhua 2013/5/3 for bug id 36*/
    safe_strncpy(info->sw_inter_version,CONFIG_DEVELOP_VER, sizeof(info->sw_inter_version));
    memset(tmp_var,0,sizeof(tmp_var));
    if(GetHardVersion(tmp_var,sizeof(tmp_var)) !=-1)
    {      
        safe_strncpy(info->hw_version,tmp_var,sizeof(info->hw_version));     
    }
    else
    {
        invalid++;
    }
    memset(tmp_var,0,sizeof(tmp_var));
    if(GetBootVersion(tmp_var,sizeof(tmp_var)) !=-1)
    {
        safe_strncpy(info->bootload_version,tmp_var,sizeof(info->bootload_version));     
    }
    else
    {
        invalid++;
    }
    memset(kernel_version,0,sizeof(kernel_version));
    memset(kernel_build_time,0,sizeof(kernel_build_time));
    
    if(read_kernel_version(kernel_version,kernel_build_time) !=-1)
    {   
    safe_strncpy(info->kernel_version,kernel_version, sizeof(info->kernel_version));
    safe_strncpy(info->kernel_buildtime,kernel_build_time, sizeof(info->kernel_buildtime));
    }
    else
    {
        invalid++;
    }
    /*END add by zhouguanhua 2013/5/3 for bug id 36*/

    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_MVLAN_VID, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get mvlan id  faild");
    }

	if (val[0])
    {
		info->mvlan = strtoul(val, NULL, 0);       
	}
    else 
    {
            invalid ++;
    }

	if (invalid > 0){
		DBG_ASSERT(1, "Invalid SYS INFO");
	}	
}