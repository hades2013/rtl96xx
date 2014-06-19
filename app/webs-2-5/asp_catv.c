/*******************************************************************************
 Copyright (c) 2013, Hangzhou LEPEI Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                             
  Project Code: lware
   Module Name: 
  Date Created: 2013-10-05
        Author: huangmingjian 
   Description: 

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD  

*******************************************************************************/

#include	"uemf.h"
#include	"wsIntrn.h"
#include	<signal.h>
#include	<unistd.h>
#include	<time.h>
#include	<sys/types.h>
#include	<debug.h>
#include 	<config.h>
#include	<syslog.h>
#include	<cable.h>
#include	<ipc_client.h>
#include	<hex_utils.h>
#include	<str_utils.h>
#include 	"webs_nvram.h"
#include	"asp_variable.h"
#include    "pdt_config.h"
#include 	<ipc_protocol.h>
#include 	"memshare.h"
#include    <lw_config_oid.h>
#include    <sys/ioctl.h>
#include    <net/if.h>
#include    "shutils.h"
#include    <time_zone.h>
#include    <lw_config_api.h>
#include    <lw_drv_pub.h>
#include    <lw_drv_req.h>
#include    <vlan_utils.h>
#include	"asp_variable.h"  


#ifdef CONFIG_CATVCOM	
extern int ipc_fd;


/*****************************************************************
  Function:        asp_get_catv
  Description:    asp_get_catv
  Author: 	         huangmingjian
  Date:   		  2013/10/15
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

 int asp_get_catv(int eid, webs_t wp, int argc, char_t **argv)
{
	sys_admin_t *info_admin = NULL;
	ipc_sys_admin_ack_t *pack_admin = NULL;
    ipc_catvinfo_ack_t *pack = NULL;
	catvinfo_t *info = NULL;
	int ret = 0;
	
	pack_admin = ipc_sys_admin(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if(pack_admin && pack_admin->hdr.status == IPC_STATUS_OK) 
	{
		info_admin = &pack_admin->admin;
	}
	else
	{
		DBG_PRINTF("ipc_sys_admin error\n");
		if(pack_admin) 
		{
			free(pack_admin);
		}
		return -1;
	}

    pack = ipc_catv_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW , NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) 
	{		
		info = &pack->catvinfo;
	#if defined(CONFIG_ZBL_SNMP)
		ret = websWrite(wp, T("'%u;%lu;%s"), 
				info->dev_status_update,
				info_admin->idle_time,
				info->input_laser_power);
	
		if(VAL_INT_ERROR == info->output_level)
			ret = websWrite(wp, T(";%s"), "N/A");
		else
			ret = websWrite(wp, T(";%d.%d"), info->output_level/10, info->output_level%10);
		
		ret = websWrite(wp, T(";%s"), info->temper);
		
		if(VAL_INT_ERROR == info->power_12v)
			ret = websWrite(wp, T(";%s"), "N/A");
		else
			ret = websWrite(wp, T(";%d.%d"), info->power_12v/10, info->power_12v%10);
		
		if(VAL_INT_ERROR == info->input_laser_power_mw)
			ret = websWrite(wp, T(";%s'"), "N/A");
		else
			ret = websWrite(wp, T(";%d.%d'"), info->input_laser_power_mw/10, info->input_laser_power_mw%10);

	#else
		ret = websWrite(wp, T("'%u;%lu;%s;%s;%s;%s;%s;%u'"), 
				info->dev_status_update,
				info_admin->idle_time,
				info->soft_version,
				info->input_laser_power,
				info->output_level,
				info->temper,
				info->power_12v,
				info->output_level_atten);
		DBG_PRINTF("'%u;%lu;%s;%s;%s;%s;%s;%u'",
				info->dev_status_update,
				info_admin->idle_time,
				info->soft_version,
				info->input_laser_power,
				info->output_level,
				info->temper,
				info->power_12v,
				info->output_level_atten);
	#endif
	}
	else
	{	
		DBG_PRINTF("asp_get_catv get error!");
		if(pack) 
		{
			free(pack);
		}
		if(pack_admin) 
		{
			free(pack_admin);
		}
		return -1;
	}
	
	if(pack) 
	{
		free(pack);
	}	
	if(pack_admin) 
	{
		free(pack_admin);
	}
	
	return ret;	
	
}



/*****************************************************************
  Function:        catv_basic_getconfig
  Description:    catv_basic_getconfig
  Author: 	         huangmingjian
  Date:   		  2013/10/15
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/


static int catv_basic_getconfig(uint32 *valid, char *sval, char *cp, catvinfo_t *req)
{
	if(!req)
	{	
		DBG_PRINTF("req is null!");
		return -1;
	}
	
	if (!(*valid & PM_UPDATE) && !strcmp("update", sval))
    {
        *valid |= PM_UPDATE;
		req->dev_status_update = strtoul(cp, NULL, 0);
    }
	else if (!(*valid & PM_OUTPUT_LEVEL_ATTEN) && !strcmp("output_level_atten", sval))
    {
        *valid |= PM_OUTPUT_LEVEL_ATTEN;
		req->output_level_atten = strtoul(cp, NULL, 0);
    }
    return valid;
}



/*****************************************************************
  Function:        asp_set_catv
  Description:    asp_set_catv
  Author: 	         huangmingjian
  Date:   		  2013/10/15
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

 int asp_set_catv(webs_t wp, cgi_result_t *result)
{
    ipc_catvinfo_ack_t *pack = NULL;
    ipc_catvinfo_ack_t *pack_set = NULL;
	catvinfo_t info;
    int ret = 0;
    int i = 0;
    uint32 valid = 0;
    int count = 0;
    char *cp = NULL;
    char *sval = NULL;
    char svar[BUF_SIZE_64] = {0};  

	memset(&info, 0, sizeof(catvinfo_t));
    pack = ipc_catv_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW , NULL);
 	if(pack && (pack->hdr.status == IPC_STATUS_OK)) 
	{	
		memcpy(&info, &(pack->catvinfo), sizeof(catvinfo_t));
	}
	else
	{
		DBG_PRINTF("Get catv fail!");
	    if(pack) free(pack);
		return 0;
	}
	if(pack) free(pack);

    while(1) 
    {
        sprintf(svar, "SET%d", count);
        sval = websGetVar(wp, svar, NULL);
        if(!sval)
        {      
            break;
        }
		DBG_PRINTF("SET%d = %s", count, sval);			
        cp = strchr(sval, '=');
        if (!cp)
        {   
            break;
        }
        *cp = 0;
        cp ++;
        if (!(ret = catv_basic_getconfig(&valid, sval, cp, &info)))
        {
            break;
        }
		
        count++;
    }   
	
#if defined(CONFIG_ZBL_SNMP)
	if (valid != PM_UPDATE)
    {
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
    }
#else
	if (valid != (PM_UPDATE | PM_OUTPUT_LEVEL_ATTEN))
    {
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
    }
#endif
	info.module_mask |= DEV_STATUS;
    pack_set = ipc_catv_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, &info);
	IPC_RESULT(pack_set, result);
    if(pack_set) free(pack_set);

	return 1;	
}


#endif

