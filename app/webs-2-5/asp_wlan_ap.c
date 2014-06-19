/*******************************************************************************
 Copyright (c) 2013, Hangzhou LEPEI Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                             
  Project Code: lware
   Module Name: 
  Date Created: 2013-10-01
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


#ifdef CONFIG_WLAN_AP	
extern int ipc_fd;


/*****************************************************************
  Function:        asp_get_wlan_basic
  Description:     asp_get_wlan_basic
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

 int asp_get_wlan_basic(int eid, webs_t wp, int argc, char_t **argv)
{
    ipc_wlan_ap_ack_t *pack = NULL;
	wlan_basic_t *info = NULL;
	
    pack = ipc_wlan_ap_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW , NULL);
 	if(!(pack && (pack->hdr.status == IPC_STATUS_OK))) 
	{	
		DBG_PRINTF("Get wlan_basic fail!");
	    if(pack)
	    {
	    	free(pack);
	    }
	    	
		return 0;
	}
	info =  &(pack->wlan_ap.wlan_basic);
    websWrite(wp, T("'%d;%d;%d;%d;%d;%d'"), 
						info->enable, 
						info->model,
						info->bandwidth,
						info->channel,
						info->tx_rate,
						info->up_ser_vlan);
	if(pack)
	{
		free(pack);
	}

    return 1;
}


/*****************************************************************
  Function:        wlan_basic_getconfig
  Description:     wlan_basic_getconfig
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

static int wlan_basic_getconfig(uint32 *valid, char *sval, char *cp, wlan_ap_t *req)
{
	if(!req)
	{	
		DBG_PRINTF("req is null!");
		return -1;
	}
	
	wlan_basic_t *info = &(req->wlan_basic);

	if (!(*valid & PM_FILTER_EN) && !strcmp("del_list", sval))
	{
		*valid |= PM_DEL;
		req->del_mask = strtoul(cp, NULL, 0);
	}
	else if (!(*valid & PM_ENABLE) && !strcmp("wlanEn", sval))
    {
        *valid |= PM_ENABLE;
		info->enable = strtoul(cp, NULL, 0);
    }
	else if (!(*valid & PM_MODEL) && !strcmp("wlanMode", sval))
    {
        *valid |= PM_MODEL;
		info->model = strtoul(cp, NULL, 0);
    }
	else if (!(*valid & PM_BANDWIDTH) && !strcmp("wlanWidth", sval))
    {
        *valid |= PM_BANDWIDTH;
		info->bandwidth = strtoul(cp, NULL, 0);		
    }
	else if (!(*valid & PM_CHANNEL) && !strcmp("wlanChannel", sval))
    {
        *valid |= PM_CHANNEL;
		info->channel = strtoul(cp, NULL, 0);		
    }
	else if (!(*valid & PM_TX_RATE) && !strcmp("wlanTxRate", sval))
    {
        *valid |= PM_TX_RATE;
		info->tx_rate = strtoul(cp, NULL, 0);			
    }
	else if (!(*valid & PM_UP_SER_VLAN) && !strcmp("up_ser_vlan", sval))
    {
        *valid |= PM_UP_SER_VLAN;
		info->up_ser_vlan = strtoul(cp, NULL, 0);			
    }
	
    return valid;
}


/*****************************************************************
  Function:        asp_set_wlan_basic
  Description:     asp_set_wlan_basic
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

 int asp_set_wlan_basic(webs_t wp, cgi_result_t *result)
{
    ipc_wlan_ap_ack_t *pack = NULL;
    ipc_wlan_ap_ack_t *pack_set = NULL;
	wlan_ap_t info;
    int ret = 0;
    int i = 0;
    uint32 valid = 0;
    int count = 0;
    char *cp = NULL;
    char *sval = NULL;
    char svar[BUF_SIZE_64] = {0};  

	memset(&info, 0, sizeof(wlan_ap_t));
    pack = ipc_wlan_ap_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW , NULL);
 	if(pack && (pack->hdr.status == IPC_STATUS_OK)) 
	{	
		memcpy(&info, &(pack->wlan_ap), sizeof(wlan_ap_t));
	}
	else
	{
		DBG_PRINTF("Get wlan_basic fail!");
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
        if (!(ret = wlan_basic_getconfig(&valid, sval, cp, &info)))
        {
            break;
        }
		
        count++;
    }   

    if ((valid != PM_DEL) && (valid!=(PM_ENABLE | PM_MODEL | PM_BANDWIDTH | PM_CHANNEL | PM_TX_RATE | PM_UP_SER_VLAN)))
    {
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
    }

	info.module_mask |= WLAN_MM_BASIC;
    pack_set = ipc_wlan_ap_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, &info);
	IPC_RESULT(pack_set, result);
    if(pack_set) free(pack_set);

	return 1;	
}


/*****************************************************************
  Function:        asp_get_wlan_security
  Description:     asp_get_wlan_security
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

 int asp_get_wlan_security(int eid, webs_t wp, int argc, char_t **argv)
{
    ipc_wlan_ap_ack_t *pack = NULL;
    unsigned int aging_time = 0;
	wlan_security_t *info = NULL;
	int end = MAX_SSID-1;
	int i = 0;
	char buf[1024] = {0};
	
    pack = ipc_wlan_ap_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW , NULL);
 	if(!(pack && (pack->hdr.status == IPC_STATUS_OK))) 
	{	
		DBG_PRINTF("Get wlan_basic fail!");
	    if(pack)
	    {
	    	free(pack);
	    }
	    	
		return 0;
	}

	info = &(pack->wlan_ap.wlan_security);
	for(i=0; i<MAX_SSID; i++)
	{ 
		if(strcmp(info->ssid[i].name, BLANK_STR))	
		{
			sprintf(buf+strlen(buf),"'%d;SSID-%d;%d;%s;%d;%d;%d;",
							info->curr_count, 
							i+1,
							info->ssid[i].enable,
							info->ssid[i].name,
							info->ssid[i].isolate_en,
							info->ssid[i].bcast_en,
							info->ssid[i].enc_type);
			
			if(info->ssid[i].enc_type == ENC_NONE)
			{
				sprintf(buf+strlen(buf),";;;;',");
			}
			else if(info->ssid[i].enc_type == ENC_WEP)
			{
				sprintf(buf+strlen(buf),"%d;%d;%d;%s;%d',",
								info->ssid[i].enc.wep.group,
								info->ssid[i].enc.wep.key_len,
								info->ssid[i].enc.wep.key_type,
								info->ssid[i].enc.wep.key,
								info->ssid[i].enc.wep.authtype);
			}
			else
			{	
				sprintf(buf+strlen(buf),"%s;%d;%d;;',",
								info->ssid[i].enc.wpa.key,
								info->ssid[i].enc.wpa.key_type,
								info->ssid[i].enc.wpa.period);					
			}
		}
	}

	if(strlen(buf)>0)
		buf[strlen(buf)-1] = '\0';
	websWrite(wp, T("%s"), buf);


	if(pack)
	{
		free(pack);
	}

    return 1;
}


/*****************************************************************
  Function:        wlan_security_getconfig
  Description:     wlan_security_getconfig
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

static int wlan_security_getconfig(uint32 *valid, char *sval, char *cp, wlan_ap_t *req)
{
	if(!req)
	{	
		DBG_PRINTF("Req is null!");
		return -1;
	}
	
	wlan_security_t *info = &(req->wlan_security);
	int index = info->curr_index;
	
	if (!(*valid & PM_SSID_INDEX) && !strcmp("ssid_num", sval))
    {
        *valid |= PM_SSID_INDEX; 
		info->curr_index = strtoul(cp, NULL, 0);
    }
	else if (!(*valid & PM_SSID_NAME) && !strcmp("ssid_name", sval))
    {
        *valid |= PM_SSID_NAME;
		safe_strncpy(info->ssid[index].name, cp, sizeof(info->ssid[index].name));
    }
	else if (!(*valid & PM_SSID_ENABLE) && !strcmp("ssid_en", sval))
    {
        *valid |= PM_SSID_ENABLE;
		info->ssid[index].enable = strtoul(cp, NULL, 0);		
    }
	else if (!(*valid & PM_ISOLATE_EN) && !strcmp("ssid_isolate_en", sval))
    {
        *valid |= PM_ISOLATE_EN;
		info->ssid[index].isolate_en = strtoul(cp, NULL, 0);		
    }
	else if (!(*valid & PM_BCAST_EN) && !strcmp("ssid_broad_en", sval))  
    {
        *valid |= PM_BCAST_EN;
		info->ssid[index].bcast_en = strtoul(cp, NULL, 0);			
    }	
	else if (!(*valid & PM_ENC_TYPE) && !strcmp("ssid_enc", sval))
    {
        *valid |= PM_ENC_TYPE;
		info->ssid[index].enc_type = strtoul(cp, NULL, 0);			
    }
	else if(info->ssid[index].enc_type == ENC_WEP)
	{
		if(!(*valid & PM_AUTHTYPE) && !strcmp("authtype", sval))  
	    {
	        *valid |= PM_AUTHTYPE;
			info->ssid[index].enc.wep.authtype = strtoul(cp, NULL, 0);			
	    }
		else if (!(*valid & PM_ENC_WEP_GROUP) && !strcmp("wep_group", sval))
		{
        	*valid |= PM_ENC_WEP_GROUP;
			info->ssid[index].enc.wep.group = strtoul(cp, NULL, 0);		
		}
		else if (!(*valid & PM_ENC_WEP_KEY_LEN) && !strcmp("wep_key_len", sval))
		{
        	*valid |= PM_ENC_WEP_KEY_LEN;
			info->ssid[index].enc.wep.key_len = strtoul(cp, NULL, 0);		
		}
		else if (!(*valid & PM_ENC_WEP_KEY_TYPE) && !strcmp("wep_key_type", sval))
		{
        	*valid |= PM_ENC_WEP_KEY_TYPE;
			info->ssid[index].enc.wep.key_type = strtoul(cp, NULL, 0);		
		}
		else if (!(*valid & PM_ENC_WEP_KEY) && !strcmp("wep_key", sval))
		{
        	*valid |= PM_ENC_WEP_KEY;
			safe_strncpy(info->ssid[index].enc.wep.key, cp, sizeof(info->ssid[index].enc.wep.key));
		}		
	}
	else if(info->ssid[index].enc_type > ENC_WEP)
	{
		if (!(*valid & PM_ENC_WPA_KEY) && !strcmp("wpa_key", sval))
		{
        	*valid |= PM_ENC_WPA_KEY;
			safe_strncpy(info->ssid[index].enc.wpa.key, cp, sizeof(info->ssid[index].enc.wpa.key));
		}
		else if (!(*valid & PM_ENC_WPA_KEY_TYPE) && !strcmp("wpa_key_type", sval))
		{
        	*valid |= PM_ENC_WPA_KEY_TYPE;
			info->ssid[index].enc.wpa.key_type = strtoul(cp, NULL, 0);		
		}
		else if (!(*valid & PM_ENC_WPA_KEY_PERIOD) && !strcmp("wpa_key_period", sval))
		{
        	*valid |= PM_ENC_WPA_KEY_PERIOD;
			info->ssid[index].enc.wpa.period = strtoul(cp, NULL, 0);		
		}
	}
	
	return *valid;
}


/*****************************************************************
  Function:        asp_set_wlan_security
  Description:     asp_set_wlan_security
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

 int asp_set_wlan_security(webs_t wp, cgi_result_t *result)
{
	#define   SSID_ADD (PM_SSID_INDEX | PM_SSID_NAME)
	#define   ENC_NONE (PM_SSID_INDEX | PM_SSID_NAME | PM_SSID_ENABLE | PM_ISOLATE_EN | PM_BCAST_EN | PM_ENC_TYPE)
	#define   ENC_WEP  (PM_SSID_INDEX | PM_SSID_NAME | PM_SSID_ENABLE | PM_ISOLATE_EN | PM_BCAST_EN | PM_AUTHTYPE | PM_ENC_TYPE | \
						PM_ENC_WEP_GROUP | PM_ENC_WEP_KEY_LEN | PM_ENC_WEP_KEY_TYPE | PM_ENC_WEP_KEY)
	#define   ENC_WPA  (PM_SSID_INDEX | PM_SSID_NAME | PM_SSID_ENABLE | PM_ISOLATE_EN | PM_BCAST_EN | PM_ENC_TYPE | \
						PM_ENC_WPA_KEY | PM_ENC_WPA_KEY_TYPE | PM_ENC_WPA_KEY_PERIOD)


    ipc_wlan_ap_ack_t *pack = NULL;
    ipc_wlan_ap_ack_t *pack_set = NULL;
	wlan_ap_t info;
	wlan_security_t *secu = &(info.wlan_security);
    int ret = 0;
    int i = 0;
    uint32 valid = 0;
    int count = 0;
    char *cp = NULL;
    char *sval = NULL;
    char svar[BUF_SIZE_64] = {0};  
	
	memset(&info, 0, sizeof(wlan_ap_t));
    pack = ipc_wlan_ap_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW , NULL);
 	if(pack && (pack->hdr.status == IPC_STATUS_OK)) 
	{	
		memcpy(&info, &(pack->wlan_ap), sizeof(wlan_ap_t));
	}
	else
	{
		DBG_PRINTF("Get wlan_basic fail!");
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
        if (!(ret = wlan_security_getconfig(&valid, sval, cp, &info)))
        {
            break;
        }
      
        count++;
    }   

    if((valid!=SSID_ADD) && (valid!=ENC_NONE) && (valid!=ENC_WEP) && (valid!=ENC_WPA)) 
    {	
		DBG_PRINTF("info.module_mask:%02x", info.module_mask);
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
    }

	info.module_mask |= WLAN_MM_SECURITY;
    pack_set = ipc_wlan_ap_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, &info);
	IPC_RESULT(pack_set, result);
    if(pack_set) free(pack_set);

	return 1;	
}


/*****************************************************************
  Function:        asp_get_wlan_mac_list
  Description:     asp_get_wlan_mac_list
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

 int asp_get_wlan_mac_list(int eid, webs_t wp, int argc, char_t **argv)
{
    ipc_wlan_ap_ack_t *pack = NULL;
	wlan_access_ctr_t *info = NULL;
	int i = 0;
	int end = MAX_FILTER-1;
	char buf[2048] = {0};
	int len = 0;
	char *ptr = NULL;
	
    pack = ipc_wlan_ap_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW , NULL);
 	if(!(pack && (pack->hdr.status == IPC_STATUS_OK))) 
	{	
		DBG_PRINTF("Get wlan_basic fail!");
	    if(pack)
	    {
	    	free(pack);
	    }
	    	
		return 0;
	}
	
	info = &(pack->wlan_ap.wlan_access_ctr);
	for(i=0; i<MAX_FILTER; i++)
	{ 
		if(strcmp(info->filter[i].mac, BLANK_STR))		
		{
			sprintf(buf+strlen(buf), "'%d;%s;%s',", 
							i,
							info->filter[i].mac, 
							info->filter[i].dec);
		}
	}
	len = strlen(buf);
	if(strlen(buf)>0)
		buf[len-1] = '\0';
	if(websWriteSafe(wp, buf, len-1)<0)
	{
		DBG_PRINTF("Send data fail");
	}
	
	if(pack)
	{
		free(pack);
	}

    return 1;
}



/*****************************************************************
  Function:        asp_get_wlan_access_ctr
  Description:     asp_get_wlan_access_ctr
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

 int asp_get_wlan_access_ctr(int eid, webs_t wp, int argc, char_t **argv)
{
    ipc_wlan_ap_ack_t *pack = NULL;
	wlan_access_ctr_t *info = NULL;

    pack = ipc_wlan_ap_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW, NULL);
 	if(!(pack && (pack->hdr.status == IPC_STATUS_OK))) 
	{	
		DBG_PRINTF("Get wlan_basic fail!");
	    if(pack)
	    {
	    	free(pack);
	    }
	    	
		return 0;
	}

	info = &(pack->wlan_ap.wlan_access_ctr);
    websWrite(wp, T("'%d;%d;%d;%d;%d'"), 
						info->filter_en, 
						info->filter_model,
						MAX_FILTER,
						info->curr_count,
						pack->wlan_ap.wlan_basic.enable);

	if(pack)
	{
		free(pack);
	}

    return 1;
}


/*****************************************************************
  Function:        mac_is_exist
  Description:     mac_is_exist
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

int mac_is_exist(wlan_filter_t *filter, int len, char *mac_str, int *index)
{
	int i = 0;
	if(!filter || !mac_str)
	{
		DBG_PRINTF("Filter or mac_str is NULL");
		return -1;
	}
	for(i=0; i<len; i++)
	{
		if(0==strncmp(filter[i].mac, mac_str, sizeof(filter[i].mac)))
		{
			*index = i; 
			return TRUE; 
		}
	}
	return FALSE;
}



/*****************************************************************
  Function:        find_filter_unuse_index
  Description:     find_filter_unuse_index
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

int find_filter_unuse_index(wlan_filter_t *filter, int len)
{
	int i = 0;
	if(!filter)
	{
		DBG_PRINTF("Filter is NULL");
		return -1;
	}
	for(i=0; i<len; i++)
	{
		if(0==strcmp(filter[i].mac, BLANK_STR))
		{	
			return i;
		}
	}
	return -1;
}


/*****************************************************************
  Function:        wlan_access_ctr_getconfig
  Description:     wlan_access_ctr_getconfig
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

static int wlan_access_ctr_getconfig(uint32 *valid, char *sval, char *cp, wlan_ap_t *req)
{
	wlan_access_ctr_t *info = NULL;
	int index = 0;
	
	if(!req)
	{	
		DBG_PRINTF("req is null!");
		return -1;
	}
	info = &(req->wlan_access_ctr);

	if (!(*valid & PM_FILTER_EN) && !strcmp("del_list", sval))
	{
		*valid |= PM_DEL;
		req->del_mask = strtoul(cp, NULL, 0);
	}
	else if (!(*valid & PM_FILTER_EN) && !strcmp("upload_mac_file", sval))
	{
		*valid |= PM_FILTER_UPLOAD;
		safe_strncpy(info->filename, cp, sizeof(info->filename));
	}
	else if (!(*valid & PM_FILTER_EN) && !strcmp("download_file", sval))
	{
		*valid |= PM_FILTER_DOWNLOAD;
		safe_strncpy(info->filename, cp, sizeof(info->filename));
	}	
	else if (!(*valid & PM_FILTER_EN) && !strcmp("mac_filter_en", sval))
    {
        *valid |= PM_FILTER_EN;
		info->filter_en = strtoul(cp, NULL, 0);
    }
	else if (!(*valid & PM_FILTER_MODEL) && !strcmp("mac_filter_mode", sval))
    {
        *valid |= PM_FILTER_MODEL;
		info->filter_model = strtoul(cp, NULL, 0);
    }
	else if (!(*valid & PM_FILTER_MAC) && !strcmp("mac_filter_mac", sval))
    {
    	if(FALSE==mac_is_exist(info->filter, MAX_FILTER, cp, &index))   /*FALSE:add mac, TRUE:edit index mac:*/
    	{	DBG_PRINTF("FALSE");
			if((index = find_filter_unuse_index(info->filter, MAX_FILTER))<0)
			{
				return -1;
			}
    	}
		
		DBG_PRINTF("index:%d", index);
		*valid |= PM_FILTER_MAC;
		info->curr_index = index;
		safe_strncpy(info->filter[index].mac, cp, sizeof(info->filter[index].mac));
    }
	else if (!(*valid & PM_FILTER_MAC_DEC) && !strcmp("mac_filter_mac_dec", sval))
    {
        *valid |= PM_FILTER_MAC_DEC;
		safe_strncpy(info->filter[info->curr_index].dec, cp, sizeof(info->filter[info->curr_index].dec));
    }
    return *valid;
}




/*****************************************************************
  Function:        asp_set_wlan_access_ctr
  Description:     asp_set_wlan_access_ctr
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

 int asp_set_wlan_access_ctr(webs_t wp, cgi_result_t *result)
{
	#define DEL_ACTON         	   PM_DEL
	#define UPLOAD_ACTON		   PM_FILTER_UPLOAD
	#define DOWNLOAD_ACTON		   PM_FILTER_DOWNLOAD
	#define ONLY_FILTER		   	   (PM_FILTER_EN | PM_FILTER_MODEL)
	#define FILTER_AND_MAC	   	   (PM_FILTER_EN | PM_FILTER_MODEL | PM_FILTER_MAC | PM_FILTER_MAC_DEC)

    ipc_wlan_ap_ack_t *pack = NULL;
    ipc_wlan_ap_ack_t *pack_set = NULL;
	wlan_ap_t info;
    int ret = 0;
    int i = 0;
    uint32 valid = 0;
    int count = 0;
    char *cp = NULL;
    char *sval = NULL;
    char svar[BUF_SIZE_256] = {0};  

	memset(&info, 0, sizeof(wlan_ap_t));
    pack = ipc_wlan_ap_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW , NULL);
 	if(pack && (pack->hdr.status == IPC_STATUS_OK)) 
	{	
		memcpy(&info, &(pack->wlan_ap), sizeof(wlan_ap_t));
	}
	else
	{
		DBG_PRINTF("Get wlan_basic fail!");
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
        if (!(ret = wlan_access_ctr_getconfig(&valid, sval, cp, &info)))
        {
            break;
        }

        count++;
    }   

	if((valid!=DEL_ACTON) && (valid != UPLOAD_ACTON) && (valid!=DOWNLOAD_ACTON) && 
		(valid != ONLY_FILTER)  && (valid != FILTER_AND_MAC))
	{
		DBG_PRINTF("valid invalid:0x%08x'", valid);
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
	}

	if(valid == UPLOAD_ACTON)
	{
		
		return 1;
	}
	else if(valid == DOWNLOAD_ACTON)
	{
		
		return 1;
	}
	
	info.module_mask |= WLAN_MM_ACCESS_CTR;
    pack_set = ipc_wlan_ap_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, &info);
	IPC_RESULT(pack_set, result);
    if(pack_set) free(pack_set);

	return 1;	
}


/*****************************************************************
  Function:        asp_get_wlan_advance
  Description:     asp_get_wlan_advance
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

 int asp_get_wlan_advance(int eid, webs_t wp, int argc, char_t **argv)
{
    ipc_wlan_ap_ack_t *pack = NULL;
	wlan_basic_t *wlan_basic = NULL;
	wlan_advance_t *info = NULL;
	
    pack = ipc_wlan_ap_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW , NULL);
 	if(!(pack && (pack->hdr.status == IPC_STATUS_OK))) 
	{	
		DBG_PRINTF("Get wlan_basic fail!");
	    if(pack)
	    {
	    	free(pack);
	    }	
		return 0;
	}

	wlan_basic = &(pack->wlan_ap.wlan_basic);
	info = &(pack->wlan_ap.wlan_advance);
    websWrite(wp, T("'%d;%d;%d;%d;%d;%d;%d;%d'"), 
						wlan_basic->enable, 
						wlan_basic->model,
						info->beacon_interval,
						info->rts_threshold,
						info->frag,
						info->dtim_interval,
						info->tx_power,
						info->wmm_en);  	
	if(pack)
	{
		free(pack);
	}

    return 1;
}


/*****************************************************************
  Function:        wlan_advance_getconfig
  Description:     wlan_advance_getconfig
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

static int wlan_advance_getconfig(uint32 *valid, char *sval, char *cp, wlan_ap_t *req)
{
	wlan_advance_t *info = &(req->wlan_advance);
	if(!req)
	{	
		DBG_PRINTF("req is null!");
		return -1;
	}
	
	if (!(*valid & PM_BEACON_INTERVAL) && !strcmp("wl_beaconinterval", sval))
    {
        *valid |= PM_BEACON_INTERVAL;
		info->beacon_interval = strtoul(cp, NULL, 0);
    }
	else if (!(*valid & PM_RST_THRESHOLD) && !strcmp("wl_rtsthreshold", sval))
    {
        *valid |= PM_RST_THRESHOLD;
		info->rts_threshold = strtoul(cp, NULL, 0);
    }
	else if (!(*valid & PM_FRAG) && !strcmp("wl_fragthreshold", sval))
    {
        *valid |= PM_FRAG;
		info->frag = strtoul(cp, NULL, 0);		
    }
	else if (!(*valid & PM_DTIM_INTERVAL) && !strcmp("wl_dtiminterval", sval))
    {
        *valid |= PM_DTIM_INTERVAL;
		info->dtim_interval = strtoul(cp, NULL, 0);		
    }
	else if (!(*valid & PM_TX_POWER) && !strcmp("wl_txpower", sval))
    {
        *valid |= PM_TX_POWER;
		info->tx_power = strtoul(cp, NULL, 0);			
    }
	else if (!(*valid & PM_WMM_EN) && !strcmp("wl_wmm", sval))
    {
        *valid |= PM_WMM_EN;
		info->wmm_en = strtoul(cp, NULL, 0);			
    }

    return (PM_BEACON_INTERVAL | PM_RST_THRESHOLD | PM_FRAG | PM_DTIM_INTERVAL | PM_TX_POWER | PM_WMM_EN);
}


/*****************************************************************
  Function:        asp_set_wlan_advance
  Description:     asp_set_wlan_advance
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

 int asp_set_wlan_advance(webs_t wp, cgi_result_t *result)
{
    ipc_wlan_ap_ack_t *pack = NULL;
    ipc_wlan_ap_ack_t *pack_set = NULL;
	wlan_ap_t info;
    int ret = 0;
    int i = 0;
    uint32 valid = 0;
    int count = 0;
    char *cp = NULL;
    char *sval = NULL;
    char svar[BUF_SIZE_64] = {0};  

	memset(&info, 0, sizeof(wlan_ap_t));
    pack = ipc_wlan_ap_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW , NULL);
 	if(pack && (pack->hdr.status == IPC_STATUS_OK)) 
	{	
		memcpy(&info, &(pack->wlan_ap), sizeof(wlan_ap_t));
	}
	else
	{
		DBG_PRINTF("Get wlan_basic fail!");
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
        if (!(ret = wlan_advance_getconfig(&valid, sval, cp, &info)))
        {
            break;
        }
        if ((valid & ret) == ret)
        {
            break;
        }       
        count++;
    }   

    if ((valid & ret) != ret)
    {
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
    }

	info.module_mask |= WLAN_MM_ADVANCE;
    pack_set = ipc_wlan_ap_config(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW, &info);
	IPC_RESULT(pack_set, result);
    if(pack_set) free(pack_set);

	return 1;	
}


/*****************************************************************
  Function:        asp_get_wlan_client
  Description:     asp_get_wlan_client
  Author: 	      	   huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

 int asp_get_wlan_client(int eid, webs_t wp, int argc, char_t **argv)
{
    ipc_wlan_ap_ack_t *pack = NULL;
	wlan_client_t *client = NULL;
	int i = 0;
	char buf[4096] = {0};
	int len = 0;

    pack = ipc_wlan_ap_config(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NOW , NULL);
 	if(!(pack && (pack->hdr.status == IPC_STATUS_OK))) 
	{	
		DBG_PRINTF("Get wlan_basic fail!");
	    if(pack)
	    {
	    	free(pack);
	    }	
		return 0;
	}

	client = &(pack->wlan_ap.wlan_client);
	for(i=0; i<MAX_AP_CLIENT; i++)
	{
		if(client->block[i].mac[0])
		{
			sprintf(buf+strlen(buf),"'SSID-%d;%s;%s;%s',", 
							client->block[i].ssid+1, 
							client->block[i].mac,
							client->block[i].tx,
							client->block[i].rx);	
		}
	}
	
	len = strlen(buf);
	if(strlen(buf)>0)
		buf[len-1] = '\0';
	if(websWriteSafe(wp, buf, len-1)<0)
	{
		DBG_PRINTF("Send data fail");
	}
	
	DBG_PRINTF("BUF:%s", buf);
	if(pack)
	{
		free(pack);
	}

    return 1;
}


#endif

