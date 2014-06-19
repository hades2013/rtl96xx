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
	



#include "master.h"
#include "timer.h"
#include <ipc_protocol.h>
#include "build_time.h"
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
#include <lw_drv_req.h>
#include <lw_drv_pub.h>

#include "wireless.h"
#define LW_DBG_PRINTF(fmt,arg...)  \
	do{	FILE *fp=NULL;\
		fp=fopen("/dev/ttyS0","r+");\
		if(fp)\
		{\
			fprintf(fp,"%s[%d]: "fmt"\n",__FUNCTION__,__LINE__,##arg);\
			fclose(fp);\
		}\
	}while(0)

#ifdef CONFIG_WLAN_AP	

int wlan_fd = -1;		/* generic raw socket desc. */
unsigned char pwrlevelCCK_A_value[34] = {0};
unsigned char pwrlevelHT40_1S_A_value[34] = {0};
unsigned char pwrdiffOFDM_value[34] = {0};
unsigned short LocalUp_Service_vlan = 1;

#define RTL8192CD_IOCTL_GET_MIB 0x89f2

int iwpriv_wlan0_set_mib(char *cmd_key, char *str_val);
int iwpriv_wlan0_vapn_set_mib(int id, char *cmd_key, char *str_val);
int ifconfig_wlan0_status(int action);
int ifconfig_wlan0_vapn_status(int id, int action);
int wlan_cmd_set_model(unsigned int index, WLAN_MODEL_E model);
int wlan_cmd_set_tx_rate(WLAN_TXRATE_E tx_rate);
int sockets_open(void);
int get_wlan_txpower(unsigned char *buffer, unsigned int len);

/*****************************************************************
  Function:        sys_wlan_ap_load
  Description:     sys_wlan_ap_load
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

void sys_wlan_ap_load(wlan_ap_t *sys) 
{
	
	wlan_security_t *sys_sec = &(sys->wlan_security);
    wlan_access_ctr_t *sys_aceess = &(sys->wlan_access_ctr);
    wlan_advance_t 	*sys_adv = &(sys->wlan_advance);
    wlan_client_t *sys_client = &(sys->wlan_client);
	int id = 0;
	int invalid = 0;
	char val[BUF_SIZE_64] = {0};
	char def_val[BUF_SIZE_64]= {0};
	
	if(sys == NULL)
	{
		DBG_ASSERT(0, "Sys is NULL!");
		return;
	}
	memset(sys, 0, sizeof(wlan_ap_t));

	/*basic*/
	memset(val, 0, sizeof(val)); 
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_ENABLED, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_WLAN_ENABLED faild!");
	}
	if (val[0])	
		sys->wlan_basic.enable = str2index(enable_str, val);
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}

	memset(val, 0, sizeof(val)); 
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_MODEL, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_WLAN_MODEL faild!");
	}
	if (val[0])
		sys->wlan_basic.model = str2index(model_str, val);
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}

	memset(val, 0, sizeof(val)); 
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_BANDWIDTH, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_WLAN_BANDWIDTH faild!");
	}
	if (val[0])
		sys->wlan_basic.bandwidth = str2index(bandwidth_str, val);
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}

	memset(val, 0, sizeof(val)); 
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_CHANNEL, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_WLAN_CHANNEL faild!");
	}
	
	if (val[0]){
		sys->wlan_basic.channel = str2index(channel_str, val);
		DBG_PRINTF("sval:%s, sys->wlan_basic.channel:%d", val, sys->wlan_basic.channel);
		}
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}

	memset(val, 0, sizeof(val)); 
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_TX_RATE, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_WLAN_TX_RATE faild!");
	}
	if (val[0])
	{
		sys->wlan_basic.tx_rate = str2index(tx_rate_str, val);
		DBG_PRINTF("sval:%s, sys->wlan_basic.tx_rate:%d", val, sys->wlan_basic.tx_rate);
	}
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}

	memset(val, 0, sizeof(val)); 
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_UP_SER_VLAN, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_WLAN_UP_SER_VLAN faild!");
	}
	if (val[0])
	{
		sys->wlan_basic.up_ser_vlan = strtoul(val, NULL, 0);
		DBG_PRINTF("sval:%s, sys->wlan_basic.up_ser_vlan:%d", val, sys->wlan_basic.up_ser_vlan);
	}
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}	

	/*security*/
	for(id=0; id<MAX_SSID; id++)
	{
		memset(val, 0, sizeof(val)); 
		if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_NAME+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
		{	
			DBG_ASSERT(0, "Get CONFIG_WLAN_SSID_NAME faild!");
			continue;
		}
		if (val[0])
			safe_strncpy(sys_sec->ssid[id].name, val, sizeof(sys_sec->ssid[id].name));
		else 
		{
			invalid++;
			DBG_PRINTF("invalid:%d", invalid);
		}


		if(strcmp(sys_sec->ssid[id].name, BLANK_STR))
			sys_sec->curr_count++;

		memset(val, 0, sizeof(val)); 
		if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_EN+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
		{
			DBG_ASSERT(0, "Get CONFIG_WLAN_SSID_EN faild!");
		}
		if (val[0])
			sys_sec->ssid[id].enable = str2index(enable_str, val);
		else 
		{
			invalid++;
			DBG_PRINTF("invalid:%d", invalid);
		}

		
		memset(val, 0, sizeof(val)); 
		if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ISOLATE_EN+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
		{
			DBG_ASSERT(0, "Get CONFIG_WLAN_SSID_ISOLATE_EN faild!");
		}
		if (val[0])
			sys_sec->ssid[id].isolate_en = str2index(enable_str, val);
		else 
		{
			invalid++;
			DBG_PRINTF("invalid:%d", invalid);
		}

		memset(val, 0, sizeof(val)); 
		if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_BCAST_EN+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
		{
			DBG_ASSERT(0, "Get CONFIG_WLAN_SSID_BCAST_EN faild!");
		}
		if (val[0])
			sys_sec->ssid[id].bcast_en = str2index(enable_str, val);
		else 
		{
			invalid++;
			DBG_PRINTF("invalid:%d", invalid);
		}

		memset(val, 0, sizeof(val)); 
		if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_TYPE+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
		{
			DBG_ASSERT(0, "Get CONFIG_WLAN_SSID_ENC_TYPE faild!");
		}
		if (val[0])
			sys_sec->ssid[id].enc_type = str2index(enc_str, val);
		else 
		{
			invalid++;
			DBG_PRINTF("invalid:%d", invalid);
		}

		if(sys_sec->ssid[id].enc_type == ENC_NONE)
		{
			/*Do something: clear all sec data ?*/
		}
		else if(sys_sec->ssid[id].enc_type == ENC_WEP)
		{
			memset(val, 0, sizeof(val)); 
			if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_AUTHTYPE+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
			{
				DBG_ASSERT(0, "Get CONFIG_WLAN_AUTHTYPE faild!");
			}
			if (val[0])
				sys_sec->ssid[id].enc.wep.authtype = str2index(authtype_str, val);
			else 
			{
				invalid++;
				DBG_PRINTF("invalid:%d", invalid);
			}
		
			memset(val, 0, sizeof(val)); 
			if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_WEP_GROUP+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
			{
				DBG_ASSERT(0, "Get CONFIG_WLAN_SSID_ENC_WEP_GROUP faild!");
			}
			if (val[0])
				sys_sec->ssid[id].enc.wep.group = strtoul(val, NULL, 0);
			else 
			{
				invalid++;
				DBG_PRINTF("invalid:%d", invalid);
			}

			memset(val, 0, sizeof(val)); 
			if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_WEP_KEY_LEN+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
			{
				DBG_ASSERT(0, "Get CONFIG_WLAN_SSID_ENC_WEP_KEY_LEN faild!");
			}
			if (val[0])
				sys_sec->ssid[id].enc.wep.key_len = str2index(enc_wep_key_len_str, val);
			else 
			{
				invalid++;
				DBG_PRINTF("invalid:%d", invalid);
			}

			memset(val, 0, sizeof(val)); 
			if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_WEP_KEY_TYPE+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
			{
				DBG_ASSERT(0, "Get CONFIG_WLAN_SSID_ENC_WEP_KEY_TYPE faild!");
			}
			if (val[0])
				sys_sec->ssid[id].enc.wep.key_type = str2index(enc_wep_key_type_str, val);
			else 
			{
				invalid++;
				DBG_PRINTF("invalid:%d", invalid);
			}

			memset(val, 0, sizeof(val)); 
			if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_WEP_KEY+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
			{
				DBG_ASSERT(0, "Get CONFIG_WLAN_SSID_ENC_WEP_KEY faild!");
			}
			if (val[0])
				safe_strncpy(sys_sec->ssid[id].enc.wep.key, val, sizeof(sys_sec->ssid[id].enc.wep.key));
			else 
			{
				invalid++;
				DBG_PRINTF("invalid:%d", invalid);
			}
		}
		else if(sys_sec->ssid[id].enc_type > ENC_WEP)
		{
			memset(val, 0, sizeof(val)); 
			if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_WPA_KEY+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
			{
				DBG_ASSERT(0, "Get CONFIG_WLAN_SSID_ENC_WPA_KEY faild!");
			}
			if (val[0])
				safe_strncpy(sys_sec->ssid[id].enc.wpa.key, val, sizeof(sys_sec->ssid[id].enc.wpa.key));
			else 
			{
				invalid++;
				DBG_PRINTF("invalid:%d", invalid);
			}

			memset(val, 0, sizeof(val)); 
			if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_WPA_KEY_TYPE+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
			{
				DBG_ASSERT(0, "Get CONFIG_WLAN_SSID_ENC_WPA_KEY_TYPE faild!");
			}
			if (val[0])
				sys_sec->ssid[id].enc.wpa.key_type = str2index(enc_wpa_enc_pro_str, val);
			else 
			{
				invalid++;
				DBG_PRINTF("invalid:%d", invalid);
			}


			memset(val, 0, sizeof(val)); 
			if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_WPA_KEY_PERIOD+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
			{
				DBG_ASSERT(0, "Get CONFIG_WLAN_SSID_ENC_WPA_KEY_PERIOD faild!");
			}
			if (val[0])
				sys_sec->ssid[id].enc.wpa.period = strtoul(val, NULL, 0);
			else 
			{
				invalid++;
				DBG_PRINTF("invalid:%d", invalid);
			}

		
		}
		
	}

	/*access ctr*/
	memset(val, 0, sizeof(val)); 
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_MAC_FILTER_EN, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_WLAN_MAC_FILTER_EN faild!");
	}
	if (val[0])
		sys_aceess->filter_en = str2index(enable_str, val);
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}

	memset(val, 0, sizeof(val)); 
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_MAC_FILTER_MODEL, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_WLAN_MAC_FILTER_MODEL faild!");
	}
	if (val[0])
		sys_aceess->filter_model = str2index(filter_model_str, val);
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}

	for(id=0; id<MAX_FILTER; id++)
	{
		memset(val, 0, sizeof(val)); 
		if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_FILTER_MAC_MAC+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
		{
			DBG_ASSERT(0, "Get CONFIG_WLAN_FILTER_MAC_MAC faild!");
			continue;
		}
		if (val[0])
			safe_strncpy(sys_aceess->filter[id].mac, val, sizeof(sys_aceess->filter[id].mac));
		else 
		{
			invalid++;
			DBG_PRINTF("invalid:%d", invalid);
		}


		if(strcmp(sys_aceess->filter[id].mac, BLANK_STR))
			sys_aceess->curr_count++;

		memset(val, 0, sizeof(val)); 
		if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_FILTER_MAC_MAC_DEC+id+1, (void *)val, BLANK_STR, sizeof(val))) < 0)
		{
			DBG_ASSERT(0, "Get CONFIG_WLAN_FILTER_MAC_MAC_DEC faild!");
		}
		if (val[0])
			safe_strncpy(sys_aceess->filter[id].dec, val, sizeof(sys_aceess->filter[id].dec));
		else 
		{
			invalid++;
			DBG_PRINTF("invalid:%d", invalid);
		}

	}

	
	/*advance*/
	memset(val, 0, sizeof(val)); 
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_BEACON_INTERVAL, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_WLAN_BEACON_INTERVAL faild!");
	}
	if (val[0])
		sys_adv->beacon_interval = strtoul(val, NULL, 0);
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}

	memset(val, 0, sizeof(val)); 
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_RST_THRESHOLD, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_WLAN_RST_THRESHOLD faild!");
	}
	if (val[0])
		sys_adv->rts_threshold = strtoul(val, NULL, 0);
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}

	memset(val, 0, sizeof(val)); 
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_FRAG, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_WLAN_FRAG faild!");
	}
	if (val[0])
		sys_adv->frag = strtoul(val, NULL, 0);
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}

	memset(val, 0, sizeof(val)); 
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_DTIM_INTERVAL, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_WLAN_DTIM_INTERVAL faild!");
	}
	if (val[0])
		sys_adv->dtim_interval = strtoul(val, NULL, 0);
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}

	memset(val, 0, sizeof(val)); 
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_TXPOWER, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_WLAN_TXPOWER faild!");
	}
	if(val[0])
		sys_adv->tx_power = strtoul(val, NULL, 0);
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}

	memset(val, 0, sizeof(val));  
	if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_WLAN_WMM_EN, (void *)val, BLANK_STR, sizeof(val))) < 0)
	{
		DBG_ASSERT(0, "Get CONFIG_WLAN_WMM_EN faild!");
	}
	if (val[0])
		sys_adv->wmm_en = str2index(enable_str, val);
	else 
	{
		invalid++;
		DBG_PRINTF("invalid:%d", invalid);
	}

	/*client*/	
	if(invalid)
	{
		DBG_ASSERT(0, "Get wlan para faild(%d)!", invalid);
	}
	
	return;
}


/*****************************************************************
  Function:        wlan_ap_init_apply
  Description:     wlan_ap_init_apply
  Author: 	         huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

void wlan_ap_init_apply(wlan_ap_t *sys)
{
	unsigned int id = 0;
	unsigned int i = 0;
	unsigned char str_val[BUF_SIZE256]= {0};
	IF_INFO_S ifinfo;
	ifindex_t ifindex=0;
	ifindex_t pifindex=0;
	IF_RET_E ret=0;
	unsigned int mtu=1496;

	if(sys == NULL)
	{
		DBG_ASSERT(0, "Sys is NULL!");
		return;
	}

	/* Create a channel to the NET kernel. */
	if((wlan_fd = sockets_open()) < 0)
	{
		DBG_PRINTF("socket open error\n");
	}

	/*get the values about txpower from eeprom*/
	memcpy(pwrlevelCCK_A_value, "pwrlevelCCK_A", sizeof("pwrlevelCCK_A"));
	get_wlan_txpower(pwrlevelCCK_A_value, sizeof("pwrlevelCCK_A"));
	memcpy(pwrlevelHT40_1S_A_value, "pwrlevelHT40_1S_A", sizeof("pwrlevelHT40_1S_A"));
	get_wlan_txpower(pwrlevelHT40_1S_A_value, sizeof("pwrlevelHT40_1S_A"));
	memcpy(pwrdiffOFDM_value, "pwrdiffOFDM", sizeof("pwrdiffOFDM"));
	get_wlan_txpower(pwrdiffOFDM_value, sizeof("pwrdiffOFDM"));

	sprintf(str_val,"ifconfig br0 %s %d up", "mtu", mtu);
	system(str_val);

	LocalUp_Service_vlan = sys->wlan_basic.up_ser_vlan;
	Ioctl_SetWirelessUpServiceVlan(LocalUp_Service_vlan);

	#if 1
	/*create new l3van*/
	ifinfo.enSubType=IF_SUB_L3VLAN;
	IF_GetByIFName(IF_L3VLAN_BASE,&pifindex);
	sprintf(ifinfo.szIfName,"%s%d",IF_L3VLAN_NAMEPREFIX,LocalUp_Service_vlan);
	sprintf(ifinfo.szPseudoName,"%s%d",IF_L3VLAN_NAMEPREFIX,LocalUp_Service_vlan);
    ret=IF_GetByIFName(ifinfo.szIfName,&ifindex);
    if(IF_OK!=ret)
    {
		ifinfo.uiSpecIndex=IF_INVALID_IFINDEX;		
		ifinfo.uiParentIfindex=pifindex;
		ret=IF_CreatInterface(&ifinfo,&ifindex);
	}

	sprintf(str_val,"ifconfig %s.%d %s %d",IF_L3VLAN_BASE,LocalUp_Service_vlan,"mtu",mtu);
	system(str_val);
	IF_SetAttr(ifindex,IF_MTU,&mtu,sizeof(mtu));

	sprintf(str_val, "brctl addif br0 %s.%d", IF_L3VLAN_BASE, LocalUp_Service_vlan);
	system(str_val);

	IF_Enable(ifindex);
	#endif

	/*apply the cfg value to mib*/
	sys->module_mask = WLAN_MM_BASIC | WLAN_MM_ADVANCE | WLAN_MM_ACCESS_CTR;
	/*basic*/
	sys->para_mask[0] = PM_ENABLE | PM_MODEL | PM_BANDWIDTH | PM_CHANNEL | PM_TX_RATE;
	/*advance*/
	sys->para_mask[0] |= PM_BEACON_INTERVAL | PM_RST_THRESHOLD | PM_FRAG | PM_DTIM_INTERVAL | PM_TX_POWER | PM_WMM_EN;	
	/*access ctr*/
	sys->para_mask[0] |= PM_FILTER_EN | PM_FILTER_MODEL | PM_FILTER_MAC;
	sys_wlan_ap_apply(IPC_APPLY_NONE);
	
	/*security*/
	sys->module_mask = WLAN_MM_SECURITY;
	sys->para_mask[0] = PM_SSID_NAME | PM_ISOLATE_EN | PM_BCAST_EN | PM_ENC_TYPE | PM_AUTHTYPE;
	sys->para_mask[0] |= PM_ENC_WEP_GROUP | PM_ENC_WEP_KEY_LEN	| PM_ENC_WEP_KEY_TYPE |PM_ENC_WEP_KEY | PM_ENC_WPA_KEY | PM_ENC_WPA_KEY_TYPE | PM_ENC_WPA_KEY_PERIOD;
	/*Not Set PM_SSID_ENABLE*/
	for(id = 0; id < MAX_SSID; id++)
	{
		if(strcmp(sys->wlan_security.ssid[id].name, BLANK_STR))		
		{
			sys->wlan_security.curr_index = id;
			sys_wlan_ap_apply(IPC_APPLY_NOW);
			if(sys->wlan_security.ssid[id].enable)
			{
				sprintf(str_val, "brctl addif br0 wlan0-vap%d", id);
				system(str_val);
			}
			else
			{
				sprintf(str_val, "brctl delif br0 wlan0-vap%d", id);
				system(str_val);
			}
		}
	}	

	sys->module_mask = 0;
	sys->para_mask[0] = 0;
	sys->para_mask[1] = 0;
	
	return;
}



/*****************************************************************
  Function:        sys_wlan_ap_update
  Description:     sys_wlan_ap_update
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/
int sys_wlan_ap_update(wlan_ap_t *cfg)
{
	wlan_ap_t *sys = &sys_wlan_ap;
	wlan_security_t *sys_sec = &(sys->wlan_security);
	wlan_security_t *cfg_sec = &(cfg->wlan_security);
    wlan_access_ctr_t *sys_aceess = &(sys->wlan_access_ctr);
    wlan_access_ctr_t *cfg_aceess = &(cfg->wlan_access_ctr);
    wlan_advance_t 	*sys_adv = &(sys->wlan_advance);   
    wlan_advance_t 	*cfg_adv = &(cfg->wlan_advance);
    wlan_client_t 	*sys_client = &(sys->wlan_client);
    wlan_client_t 	*cfg_client = &(cfg->wlan_client);
	int update = 0;
	int id = 0;
	int i = 0;
	unsigned int enable;

	sys->module_mask = cfg->module_mask; 
	sys->para_mask[0] = 0;
	sys->para_mask[1] = 0;
	/*basic*/
	if(sys->module_mask & WLAN_MM_BASIC)
	{
		if((sys->del_mask== MASK_NO_USE) && (sys->del_mask != cfg->del_mask)) /*del ssid*/																/*del mac*/	
		{	
			sys->del_mask = cfg->del_mask;
			for(i=0; i<MAX_SSID; i++)
			{
				if(sys->del_mask & (1<<i))
				{	
					if((sys_sec->curr_count-1)<0)
					{
						DBG_PRINTF("Have no mac del");
						return -1;
					}
					sys_sec->curr_count--;
					/*begin modify by shipeng*/
					enable = sys_sec->ssid[i].enable;
					memset(&(sys_sec->ssid[i]), 0 , sizeof(sys_sec->ssid[i]));
					sys_sec->ssid[i].enable = enable;
					/*end modify by shipeng*/
				}
			}
			sys->para_mask[0] |= PM_DEL;
			update++;
		}
		
		if (sys->wlan_basic.enable != cfg->wlan_basic.enable)
		{
			sys->wlan_basic.enable = cfg->wlan_basic.enable;
			sys->para_mask[0] |= PM_ENABLE;
			update++;
		}
		if (sys->wlan_basic.model != cfg->wlan_basic.model)
		{
			sys->wlan_basic.model = cfg->wlan_basic.model;
			sys->para_mask[0] |= PM_MODEL;
			update++;
		}
		if (sys->wlan_basic.bandwidth != cfg->wlan_basic.bandwidth)
		{
			sys->wlan_basic.bandwidth = cfg->wlan_basic.bandwidth;
			sys->para_mask[0] |= PM_BANDWIDTH;
			update++;
		}
		if (sys->wlan_basic.channel != cfg->wlan_basic.channel)
		{
			sys->wlan_basic.channel = cfg->wlan_basic.channel;
			sys->para_mask[0] |= PM_CHANNEL;
			update++;
		}
		if (sys->wlan_basic.tx_rate != cfg->wlan_basic.tx_rate)
		{
			sys->wlan_basic.tx_rate = cfg->wlan_basic.tx_rate;
			sys->para_mask[0] |= PM_TX_RATE;
			update++;
		}	
		if (sys->wlan_basic.up_ser_vlan != cfg->wlan_basic.up_ser_vlan)
		{
			sys->wlan_basic.up_ser_vlan = cfg->wlan_basic.up_ser_vlan;
			sys->para_mask[1] |= PM_UP_SER_VLAN;
			update++;
		}
	}

	/*security*/	
	if(sys->module_mask & WLAN_MM_SECURITY)
	{	
		id = sys_sec->curr_index = cfg_sec->curr_index;
		if(0==strcmp(sys_sec->ssid[id].name, BLANK_STR))   /*New ssid, is add, and set it to default value*/
		{	
			if((sys_sec->curr_count+1)>MAX_SSID)
			{
				DBG_PRINTF("Out of range");
				return -1;
			}
			sys_sec->curr_count++;
			strncpy(sys_sec->ssid[id].name, cfg_sec->ssid[id].name, sizeof(sys_sec->ssid[id].name));
			//sys_sec->ssid[id].enable = DEF_SSID_EN;/*begin del by shipeng*/
			sys_sec->ssid[id].isolate_en = DEF_SSID_ISOLATE_EN;
			sys_sec->ssid[id].bcast_en = DEF_SSID_BCAST_EN;
			sys_sec->ssid[id].enc_type = DEF_SSID_ENC_TYPE;
			sys->para_mask[0] |= (PM_SSID_NAME | PM_SSID_ENABLE | PM_ISOLATE_EN | PM_BCAST_EN| PM_AUTHTYPE | PM_ENC_TYPE);
			update++;
		}
		else											  /*exist, is edit*/
		{
			if (strncmp(sys_sec->ssid[id].name, cfg_sec->ssid[id].name, sizeof(sys_sec->ssid[id].name)))
			{
				strncpy(sys_sec->ssid[id].name, cfg_sec->ssid[id].name, sizeof(sys_sec->ssid[id].name));
				sys->para_mask[0] |= PM_SSID_NAME;
				update++;
			}
			if (sys_sec->ssid[id].enable != cfg_sec->ssid[id].enable)
			{
				//sys_sec->ssid[id].enable = cfg_sec->ssid[id].enable;/*begin del by shipeng*/
				sys->para_mask[0] |= PM_SSID_ENABLE;
				update++;
			}
			if (sys_sec->ssid[id].isolate_en != cfg_sec->ssid[id].isolate_en)
			{
				sys_sec->ssid[id].isolate_en = cfg_sec->ssid[id].isolate_en;
				sys->para_mask[0] |= PM_ISOLATE_EN;
				update++;
			}
			if (sys_sec->ssid[id].bcast_en != cfg_sec->ssid[id].bcast_en)
			{
				sys_sec->ssid[id].bcast_en = cfg_sec->ssid[id].bcast_en;
				sys->para_mask[0] |= PM_BCAST_EN;
				update++;
			}					
			if (sys_sec->ssid[id].enc_type != cfg_sec->ssid[id].enc_type)
			{
				sys_sec->ssid[id].enc_type = cfg_sec->ssid[id].enc_type;
				sys->para_mask[0] |= PM_ENC_TYPE;
				update++;
			}
			
			if(sys_sec->ssid[id].enc_type == ENC_NONE)
			{
				/*do nothing*/
				memset(&(sys_sec->ssid[id].enc), 0, sizeof(sys_sec->ssid[id].enc));
			}
			else if(sys_sec->ssid[id].enc_type == ENC_WEP)
			{
				if (sys_sec->ssid[id].enc.wep.authtype != cfg_sec->ssid[id].enc.wep.authtype)
				{
					sys_sec->ssid[id].enc.wep.authtype = cfg_sec->ssid[id].enc.wep.authtype;
					sys->para_mask[0] |= PM_AUTHTYPE;
					update++;
				}				
				if (sys_sec->ssid[id].enc.wep.group != cfg_sec->ssid[id].enc.wep.group)
				{
					sys_sec->ssid[id].enc.wep.group = cfg_sec->ssid[id].enc.wep.group;
					sys->para_mask[0] |= PM_ENC_WEP_GROUP;
					update++;
				}
				if (sys_sec->ssid[id].enc.wep.key_len != cfg_sec->ssid[id].enc.wep.key_len)
				{
					sys_sec->ssid[id].enc.wep.key_len = cfg_sec->ssid[id].enc.wep.key_len;
					sys->para_mask[0] |= PM_ENC_WEP_KEY_LEN;
					update++;
				}
				if (sys_sec->ssid[id].enc.wep.key_type != cfg_sec->ssid[id].enc.wep.key_type)
				{
					sys_sec->ssid[id].enc.wep.key_type = cfg_sec->ssid[id].enc.wep.key_type;
					sys->para_mask[0] |= PM_ENC_WEP_KEY_TYPE;
					update++;
				}	
				if (strncmp(sys_sec->ssid[id].enc.wep.key, cfg_sec->ssid[id].enc.wep.key, sizeof(sys_sec->ssid[id].enc.wep.key)))
				{
					strncpy(sys_sec->ssid[id].enc.wep.key, cfg_sec->ssid[id].enc.wep.key, sizeof(sys_sec->ssid[id].enc.wep.key));
					sys->para_mask[0] |= PM_ENC_WEP_KEY;
					update++;
				}
			}
			else if(sys_sec->ssid[id].enc_type > ENC_WEP)
			{
				if (strncmp(sys_sec->ssid[id].enc.wpa.key, cfg_sec->ssid[id].enc.wpa.key, sizeof(sys_sec->ssid[id].enc.wpa.key)))
				{
					strncpy(sys_sec->ssid[id].enc.wpa.key, cfg_sec->ssid[id].enc.wpa.key, sizeof(sys_sec->ssid[id].enc.wpa.key));
					sys->para_mask[0] |= PM_ENC_WPA_KEY;
					update++;
				}
				if (sys_sec->ssid[id].enc.wpa.key_type != cfg_sec->ssid[id].enc.wpa.key_type)
				{
					sys_sec->ssid[id].enc.wpa.key_type = cfg_sec->ssid[id].enc.wpa.key_type;
					sys->para_mask[0] |= PM_ENC_WPA_KEY_TYPE;
					update++;
				}	
				if (sys_sec->ssid[id].enc.wpa.period != cfg_sec->ssid[id].enc.wpa.period)
				{
					sys_sec->ssid[id].enc.wpa.period = cfg_sec->ssid[id].enc.wpa.period;
					sys->para_mask[0] |= PM_ENC_WPA_KEY_PERIOD;
					update++;
				}			
			}
			else
			{
				DBG_PRINTF("Type no found!");
			}
		}
	}

	/*access ctr*/
	if(sys->module_mask & WLAN_MM_ACCESS_CTR)
	{
		if(sys_aceess->filter_en != cfg_aceess->filter_en)
		{
			sys_aceess->filter_en = cfg_aceess->filter_en;
			sys->para_mask[0] |= PM_FILTER_EN;
			update++;
		}
		if(sys_aceess->filter_model != cfg_aceess->filter_model)
		{
			sys_aceess->filter_model = cfg_aceess->filter_model;
			sys->para_mask[0] |= PM_FILTER_MODEL;
			update++;
		}
		
		id = sys_aceess->curr_index = cfg_aceess->curr_index;
		if((sys->del_mask== MASK_NO_USE) && (sys->del_mask != cfg->del_mask)) /*del mac*/																/*del mac*/	
		{	
			sys->del_mask = cfg->del_mask;
			for(i=0; i<MAX_FILTER; i++)
			{
				if(sys->del_mask & (1<<i))
				{	
					if((sys_aceess->curr_count-1)<0)
					{
						DBG_PRINTF("Have no mac del");
						return -1;
					}
					sys_aceess->curr_count--;
					memset(&(sys_aceess->filter[i]), 0 , sizeof(sys_aceess->filter[i]));
				}
			}
			sys->para_mask[0] |= PM_DEL;
			update++;
		}
		else if(0==strcmp(sys_aceess->filter[id].mac, BLANK_STR))	 /*add mac*/
		{	
			if((sys_aceess->curr_count+1)>MAX_FILTER)
			{
				DBG_PRINTF("Out of range");
				return -1;
			}
			sys_aceess->curr_count++;
			safe_strncpy(sys_aceess->filter[id].mac, cfg_aceess->filter[id].mac, sizeof(sys_aceess->filter[id].mac));
			safe_strncpy(sys_aceess->filter[id].dec, cfg_aceess->filter[id].dec, sizeof(sys_aceess->filter[id].dec));
			sys->para_mask[0] |= PM_FILTER_MAC;
			update++;
		}
		else  										  /*edit mac*/									
		{
			if(strncmp(sys_aceess->filter[id].dec, cfg_aceess->filter[id].dec,  sizeof(sys_aceess->filter[id].dec)))
			{
				safe_strncpy(sys_aceess->filter[id].dec, cfg_aceess->filter[id].dec, sizeof(sys_aceess->filter[id].dec));
				update++;
				sys->para_mask[0] |= PM_FILTER_MAC_DEC;
			}			
		}
	}

	/*advance*/
	if(sys->module_mask & WLAN_MM_ADVANCE)
	{
		if(sys_adv->beacon_interval != cfg_adv->beacon_interval) 
		{
			sys_adv->beacon_interval = cfg_adv->beacon_interval;
			sys->para_mask[0] |= PM_BEACON_INTERVAL;
			update++;
		}
		if(sys_adv->rts_threshold != cfg_adv->rts_threshold) 
		{
			sys_adv->rts_threshold = cfg_adv->rts_threshold;
			sys->para_mask[0] |= PM_RST_THRESHOLD;
			update++;
		}
		if(sys_adv->frag != cfg_adv->frag) 
		{
			sys_adv->frag = cfg_adv->frag;
			sys->para_mask[0] |= PM_FRAG;
			update++;
		}
		if(sys_adv->dtim_interval != cfg_adv->dtim_interval) 
		{
			sys_adv->dtim_interval = cfg_adv->dtim_interval;
			sys->para_mask[0] |= PM_DTIM_INTERVAL;
			update++;
		}
		if(sys_adv->tx_power != cfg_adv->tx_power) 
		{
			sys_adv->tx_power = cfg_adv->tx_power;
			sys->para_mask[0] |= PM_TX_POWER;
			update++;
		}
		if(sys_adv->wmm_en != cfg_adv->wmm_en) 
		{
			sys_adv->wmm_en = cfg_adv->wmm_en;
			sys->para_mask[0] |= PM_WMM_EN;
			update++;
		}
	}

	/*client*/			
	if(sys->module_mask & WLAN_MM_CLIENT)
	{
		update++;
	}
	DBG_PRINTF("wlan_ap update:%d, para_mask[0] :0x%08x, module_mask:0x%02x", update, sys->para_mask[0], sys->module_mask);
	
	return update;
}



/*****************************************************************
  Function:        iwpriv_wlan0_set_mib
  Description:    iwpriv_wlan0_set_mib
  Author: 	         huangmingjian
  Date:   		  2013/10/19
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/
int iwpriv_wlan0_set_mib(char *cmd_key, char *str_val)
{
	char cmd[BUF_SIZE256] = {0};
	
	if(!cmd_key || !str_val)
	{
		DBG_PRINTF("cmd_key or str_val is NULL");
		return -1;
	}

	sprintf(cmd, "%s %s=%s", IWPRIV_WLAN0_SET_MIB, cmd_key, str_val); 
	DBG_PRINTF("cmd:%s", cmd);
	if(0 != system(cmd))
	{
		perror("Fail");
		return -1;
	}
	return 0;
}


/*****************************************************************
  Function:        iwpriv_wlan0_vapn_set_mib
  Description:    iwpriv_wlan0_vapn_set_mib
  Author: 	         huangmingjian
  Date:   		  2013/10/19
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/
int iwpriv_wlan0_vapn_set_mib(int id, char *cmd_key, char *str_val)
{
	char cmd[BUF_SIZE256] = {0};

	if((id<0 || id>=MAX_SSID) || !cmd_key || !str_val)
	{
		DBG_PRINTF("Paras is invalid");
		return -1;
	}
	
	sprintf(cmd, IWPRIV_WLAN0_VAPN_SET_MIB" %s=%s", id, cmd_key, str_val);
	DBG_PRINTF("cmd:%s", cmd);
	if(0 != system(cmd))
	{
		perror("Fail");
		return -1;
	}
	return 0;
}


/*****************************************************************
  Function:        ifconfig_wlan0_status
  Description:    ifconfig_wlan0_status
  Author: 	         huangmingjian
  Date:   		  2013/10/19
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/
int ifconfig_wlan0_status(int action)
{
	char *act_ptr = NULL;
	char cmd[BUF_SIZE256] = {0};
	
	if(action<WALAN_DOWN || action>WALAN_UP)
	{
		DBG_PRINTF("Paras is invalid");
		return -1;
	}

	act_ptr = (action?UP_STR:DOWN_STR);
	sprintf(cmd, "ifconfig wlan0 %s", act_ptr);
	DBG_PRINTF("cmd:%s", cmd);
	if(0 != system(cmd))
	{
		perror("Fail");
		return -1;
	}
	return 0;

}


/*****************************************************************
  Function:        ifconfig_wlan0_vapn_status
  Description:    ifconfig_wlan0_vapn_status
  Author: 	         huangmingjian
  Date:   		  2013/10/19
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/
int ifconfig_wlan0_vapn_status(int id, int action)
{
	char *act_ptr = NULL;
	char cmd[BUF_SIZE256] = {0};
	
	if((id<0 || id>=MAX_SSID) || (action<WALAN_DOWN || action>WALAN_UP))
	{
		DBG_PRINTF("Paras is invalid");
		return -1;
	}

	act_ptr = (action?UP_STR:DOWN_STR);
	sprintf(cmd, "ifconfig wlan0-vap%d %s", id, act_ptr);
	DBG_PRINTF("cmd:%s", cmd);
	if(0 != system(cmd))
	{
		perror("Fail");
		return -1;
	}
	return 0;
}

int wlan_cmd_set_model(unsigned int index, WLAN_MODEL_E model)
{
	unsigned int id = 0;
	unsigned int value = 0;
	char str_val[BUF_SIZE256]= {0};
	
	if(model<WLAN_MODEL_B_ONLY || model>=WLAN_MODEL_END)
	{
		DBG_PRINTF("Paras is invalid");
		return -1;
	}
	
	switch(model)
	{
		case WLAN_MODEL_B_ONLY: value = 1; break;
		case WLAN_MODEL_G_ONLY: value = 2; break;
		case WLAN_MODEL_B_G:	value = 3; break;
		case WLAN_MODEL_N_ONLY: value = 8; break;
		case WLAN_MODEL_B_G_N:	value = 11; break;
		default:				break;
	}
	if(index & SSID_MASK)
	{
		sprintf(str_val, "%d", value);
		iwpriv_wlan0_vapn_set_mib((index & (~SSID_MASK)), "band", str_val);
		if((model == WLAN_MODEL_N_ONLY) || (model == WLAN_MODEL_B_G_N))
		{
			iwpriv_wlan0_vapn_set_mib((index & (~SSID_MASK)), "ampdu", "1");
		}
	}
	else
	{
		sprintf(str_val, "%d", value);
		iwpriv_wlan0_set_mib("band", str_val);
		for(id = 0; id < MAX_SSID; id++)
		{
			iwpriv_wlan0_vapn_set_mib(id, "band", str_val);
		}

		if((model == WLAN_MODEL_N_ONLY) || (model == WLAN_MODEL_B_G_N))
		{
			iwpriv_wlan0_set_mib("ampdu", "1");
			for(id = 0; id < MAX_SSID; id++)
			{
				iwpriv_wlan0_vapn_set_mib(id, "ampdu", "1");
			}
		}
	}
	
	return 0;
}

int wlan_cmd_set_tx_rate(WLAN_TXRATE_E tx_rate)
{
	int id = 0;
	unsigned int value = 0;
	char str_val[BUF_SIZE256]= {0};
	
	if(tx_rate<WLAN_TXRAT_0_AUTO || tx_rate>=WLAN_TXRAT_END)
	{
		DBG_PRINTF("Paras is invalid");
		return -1;
	}
	
	if(WLAN_TXRAT_0_AUTO == tx_rate)
	{
		iwpriv_wlan0_set_mib("autorate", "1");
		iwpriv_wlan0_set_mib("fixrate", "0");
		for(id = 0; id < MAX_SSID; id++)
		{
			iwpriv_wlan0_vapn_set_mib(id, "autorate", "1");
			iwpriv_wlan0_vapn_set_mib(id, "fixrate", "0");
		}				
	}
	else
	{
		switch(tx_rate)
		{
			case WLAN_TXRAT_1_1MBPS:			value = (0x1 << 0); break;
			case WLAN_TXRAT_2_2MBPS:			value = (0x1 << 1); break;
			case WLAN_TXRAT_3_5_5MBPS:			value = (0x1 << 2); break;
			case WLAN_TXRAT_4_6MBPS:			value = (0x1 << 4); break;
			case WLAN_TXRAT_5_6_5MBPS:	 
			case WLAN_TXRAT_10_13_5MBPS:		value = (0x1 << 12); break;
			case WLAN_TXRAT_6_9MBPS:			value = (0x1 << 5); break;
			case WLAN_TXRAT_7_11MBPS:			value = (0x1 << 3); break;
			case WLAN_TXRAT_8_12MBPS:			value = (0x1 << 6); break;
			case WLAN_TXRAT_9_13MBPS:			
			case WLAN_TXRAT_15_27MBPS:			value = (0x1 << 13); break;
			case WLAN_TXRAT_11_18MBPS:			value = (0x1 << 7); break;
			case WLAN_TXRAT_12_19_5MBPS:		
			case WLAN_TXRAT_18_40_5MBPS:		value = (0x1 << 14); break;
			case WLAN_TXRAT_13_24MBPS:			value = (0x1 << 8); break;
			case WLAN_TXRAT_14_26MBPS:			
			//case WLAN_TXRAT_21_54MBPS:		value = (0x1 << 15); break; 
			case WLAN_TXRAT_16_36MBPS:			value = (0x1 << 9); break;
			case WLAN_TXRAT_17_39MBPS:			
			case WLAN_TXRAT_25_81MBPS:			value = (0x1 << 16); break;
			case WLAN_TXRAT_19_48MBPS:			value = (0x1 << 10); break;
			case WLAN_TXRAT_20_52MBPS:			
			case WLAN_TXRAT_26_108MBPS: 		value = (0x1 << 17); break;
			case WLAN_TXRAT_21_54MBPS:			value = (0x1 << 11); break;
			case WLAN_TXRAT_22_58_5MBPS:		
			case WLAN_TXRAT_27_121_5MBPS:		value = (0x1 << 18); break;
			case WLAN_TXRAT_24_65MBPS:			
			case WLAN_TXRAT_28_135MBPS: 		value = (0x1 << 19); break;
			default:							break;		
		}
		
		iwpriv_wlan0_set_mib("autorate", "0");
		sprintf(str_val, "%d", value);
		iwpriv_wlan0_set_mib("fixrate", str_val);
		
		for(id = 0; id < MAX_SSID; id++)
		{
			iwpriv_wlan0_vapn_set_mib(id, "autorate", "0");
			iwpriv_wlan0_vapn_set_mib(id, "fixrate", str_val);					
		}
	}
	return 0;
}

int getnext (char *src,int separator,char *dest)
{
    char *c;
    int len = 0;

    if ( (src == NULL) || (dest == NULL) ) {
        return -1;
    }

    c = strchr(src, separator);
    if (c == NULL) {
        strcpy(dest, src);
        return -1;
    }
    len = c - src;
    strncpy(dest, src, len);
    dest[len] = '\0';
    return len + 1;
}

/* Convert MAC address from string to unsigned char array */
unsigned long simple_strtoul(const char *cp,char **endp, unsigned int base)
{
    unsigned long result = 0,value;

    if (*cp == '0') {
        cp++;
        if ((*cp == 'x') && isxdigit(cp[1])) {
            base = 16;
            cp++;
        }
        if (!base) {
            base = 8;
        }
    }
    if (!base) {
        base = 10;
    }
    while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
        ? toupper(*cp) : *cp)-'A'+10) < base) {
        result = result*base + value;
        cp++;
    }
    if (endp)
        *endp = (char *)cp;
    return result;
}

int str_to_mac (unsigned char *mac,char *str)
{
    int     len;
    char    *ptr = str;
    char    buf[128], tmp[128];
    int     i;

    for (i = 0; i < 5; ++i)
    {
        if ((len = getnext(ptr, ':', buf)) == -1 &&
            (len = getnext(ptr, '-', buf)) == -1)
        {
            return -1; /* parse error */
        }

        if (len != 3) return -1;

        strcpy(tmp, "");
        strcpy(tmp, buf);

        if (NULL != strtok(tmp, "0123456789ABCDEFabcdef")) return -1;

        mac[i] = simple_strtoul(buf, NULL, 16);
        ptr += len;
    }

    if (strlen(ptr) != 2) return -1;
    strcpy(tmp, "");
    strcpy(tmp, ptr);

    if (NULL != strtok(tmp, "0123456789ABCDEFabcdef")) return -1;

    mac[5] = simple_strtoul(ptr, NULL, 16);

    return 0;
}

/************************ SOCKET SUBROUTINES *************************/

/*------------------------------------------------------------------*/
/*
 * Open a socket.
 * Depending on the protocol present, open the right socket. The socket
 * will allow us to talk to the driver.
 */
int sockets_open(void)
{
	int ipx_sock = -1;		/* IPX socket			*/
	int ax25_sock = -1;		/* AX.25 socket			*/
	int inet_sock = -1;		/* INET socket			*/
	int ddp_sock = -1;		/* Appletalk DDP socket		*/

	inet_sock=socket(AF_INET, SOCK_DGRAM, 0);
   	//10/15/05' hrchen, we did not support IPX/AX25/APPLETALK at this time
	//ipx_sock=socket(AF_IPX, SOCK_DGRAM, 0);
	//ax25_sock=socket(AF_AX25, SOCK_DGRAM, 0);
	//ddp_sock=socket(AF_APPLETALK, SOCK_DGRAM, 0);
	/*
	 * Now pick any (exisiting) useful socket family for generic queries
	 */
	if(inet_sock!=-1)
		return inet_sock;
	if(ipx_sock!=-1)
		return ipx_sock;
	if(ax25_sock!=-1)
		return ax25_sock;
	/*
	 * If this is -1 we have no known network layers and its time to jump.
	 */
	 
	return ddp_sock;
}

/*------------------------------------------------------------------*/
/*
 * Display an Ethernet address in readable format.
 */
char *
pr_ether(unsigned char *ptr)
{
  static char buff[64];

  sprintf(buff, "%02X:%02X:%02X:%02X:%02X:%02X",
	(ptr[0] & 0xFF), (ptr[1] & 0xFF), (ptr[2] & 0xFF),
	(ptr[3] & 0xFF), (ptr[4] & 0xFF), (ptr[5] & 0xFF)
  );
  return(buff);
}

int getTxPowerScale(unsigned char mode)
{
	switch (mode)
	{
		case TXPOWER_100_PER: //100%
			return 0;
		case TXPOWER_70_PER: //70%
			return 3;
		case TXPOWER_50_PER: //50%
			return 6;
		case TXPOWER_35_PER: //35%
			return 9;
		case TXPOWER_10_PER: //10%
			return 17;
	}
}

int get_wlan_txpower(unsigned char *buffer, unsigned int len)
{
	struct iwreq wrq;

	strncpy(wrq.ifr_name, "wlan0", IFNAMSIZ);
	wrq.u.data.pointer = (caddr_t) buffer;
	wrq.u.data.length = len;

	if(ioctl(wlan_fd, RTL8192CD_IOCTL_GET_MIB, &wrq) < 0)
    {
		DBG_PRINTF("Interface doesn't accept private ioctl...\n");
		return(-1);
    }

	DBG_PRINTF("%d %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n", 
		wrq.u.data.length, 
		buffer[0], buffer[1],buffer[2],buffer[3],buffer[4],
		buffer[5], buffer[6],buffer[7],buffer[8],buffer[9],
		buffer[10], buffer[11],buffer[12],buffer[13]);

	return 0;
}

/*****************************************************************
  Function:        sys_wlan_ap_apply
  Description:     sys_wlan_ap_apply
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

int sys_wlan_ap_apply(int apply_options)
{
	wlan_ap_t *sys = &sys_wlan_ap;
	wlan_basic_t *sys_basic = &(sys->wlan_basic);
	wlan_security_t *sys_sec = &(sys->wlan_security);
    wlan_access_ctr_t *sys_aceess = &(sys->wlan_access_ctr);
    wlan_advance_t 	*sys_adv = &(sys->wlan_advance);
    wlan_client_t 	*sys_client = &(sys->wlan_client);
	unsigned int id = 0;
	unsigned char value[34] = {0};
	unsigned char str_cmd[BUF_SIZE256]= {0};
	unsigned char str_val[BUF_SIZE256]= {0};
	unsigned char mac_addr[6] = {0};
	unsigned int i = 0;
	unsigned int count = 0;
	int intVal;
	IF_INFO_S ifinfo;
	ifindex_t ifindex=0,oldifindex;
	ifindex_t pifindex=0;
	IF_RET_E ret=0;
	unsigned int mtu=1496;
	
	if (sys->para_mask[0] > 0)
	{
		if(sys->module_mask & (WLAN_MM_BASIC | WLAN_MM_ADVANCE | WLAN_MM_ACCESS_CTR))
		{						
			if (sys->para_mask[0] & PM_DEL)
			{
	            for(id = 0; id < MAX_SSID; id++)
	            {
					if(sys->del_mask & (0x1 << id))
					{
						if(sys_sec->ssid[id].enable)
						{
							ifconfig_wlan0_vapn_status(id, WALAN_DOWN);
						}
					}
	            }			
			}
			else
			{
				ifconfig_wlan0_status(WALAN_DOWN);
			}
		}
		else
		{
			id = sys->wlan_security.curr_index;
			if(sys_sec->ssid[id].enable)
			{
				ifconfig_wlan0_vapn_status(id, WALAN_DOWN);
			}
		}
	}
	/*basic*/
	if(sys->module_mask & WLAN_MM_BASIC)
	{
		if (sys->para_mask[0] & PM_DEL)
		{
            for(id = 0; id < MAX_SSID; id++)
            {
				if(sys->del_mask & (0x1 << id))
				{
					sys_sec->ssid[id].enable = DISABLE;
					if(sys_sec->ssid[id].enable)
					{
						sprintf(str_val, "brctl addif br0 wlan0-vap%d", id);
						system(str_val);
					}
					else
					{
						sprintf(str_val, "brctl delif br0 wlan0-vap%d", id);
						system(str_val);
					}
				}
            }			
		}
		
		if (sys->para_mask[0] & PM_ENABLE)
		{
			sprintf(str_val, "%d", sys_basic->enable);
			iwpriv_wlan0_set_mib("vap_enable", str_val);
		}
		if (sys->para_mask[0] & PM_MODEL)
		{
			wlan_cmd_set_model(0, sys_basic->model);
		}
		if (sys->para_mask[0] & PM_BANDWIDTH)
		{
			sprintf(str_val, "%d", sys_basic->bandwidth-1);
			iwpriv_wlan0_set_mib("use40M", str_val);
		}
		if (sys->para_mask[0] & PM_CHANNEL)
		{
			sprintf(str_val, "%d", sys_basic->channel);
			iwpriv_wlan0_set_mib("channel", str_val);
			for(id = 0; id < MAX_SSID; id++)
			{
				iwpriv_wlan0_vapn_set_mib(id, "channel", str_val);
			}			
		}
		if (sys->para_mask[0] & PM_TX_RATE)
		{
			wlan_cmd_set_tx_rate(sys_basic->tx_rate);
		}
		if (sys->para_mask[1] & PM_UP_SER_VLAN)
		{
			DBG_PRINTF("sys->wlan_basic.up_ser_vlan:%d", sys_basic->up_ser_vlan);

			#if 1
			/*delete old l3van*/
			sprintf(str_val, "%s%d", IF_L3VLAN_NAMEPREFIX, LocalUp_Service_vlan);
			IF_GetByIFName(str_val,&oldifindex);

			//sprintf(str_val, "brctl delif br0 %s.%d", IF_L3VLAN_BASE, LocalUp_Service_vlan);
			//system(str_val);

			IF_DeleteInterface(oldifindex);
			
			LocalUp_Service_vlan = sys_basic->up_ser_vlan;
			Ioctl_SetWirelessUpServiceVlan(LocalUp_Service_vlan);

			/*create new l3van*/
			ifinfo.enSubType=IF_SUB_L3VLAN;
			IF_GetByIFName(IF_L3VLAN_BASE,&pifindex);
			sprintf(ifinfo.szIfName,"%s%d",IF_L3VLAN_NAMEPREFIX,LocalUp_Service_vlan);
			sprintf(ifinfo.szPseudoName,"%s%d",IF_L3VLAN_NAMEPREFIX,LocalUp_Service_vlan);
		    ret=IF_GetByIFName(ifinfo.szIfName,&ifindex);
		    if(IF_OK!=ret)
		    {
				ifinfo.uiSpecIndex=IF_INVALID_IFINDEX;		
				ifinfo.uiParentIfindex=pifindex;
				ret=IF_CreatInterface(&ifinfo,&ifindex);
			}
			sprintf(str_val,"ifconfig %s.%d %s %d",IF_L3VLAN_BASE,LocalUp_Service_vlan,"mtu",mtu);
			system(str_val);
			IF_SetAttr(ifindex,IF_MTU,&mtu,sizeof(mtu));

			sprintf(str_val, "brctl addif br0 %s.%d", IF_L3VLAN_BASE, LocalUp_Service_vlan);
			system(str_val);

			IF_Enable(ifindex);
			#else
			LocalUp_Service_vlan = sys_basic->up_ser_vlan;
			//Ioctl_SetWirelessUpServiceVlan(LocalUp_Service_vlan);
			#endif
		}
		
	}

	/*security*/	
	if(sys->module_mask & WLAN_MM_SECURITY)
	{
		id = sys->wlan_security.curr_index;
		if((sys->para_mask[0] & (PM_SSID_NAME | PM_SSID_ENABLE | PM_ISOLATE_EN | PM_BCAST_EN | PM_AUTHTYPE | PM_ENC_TYPE))
			== (PM_SSID_NAME | PM_SSID_ENABLE | PM_ISOLATE_EN | PM_BCAST_EN | PM_AUTHTYPE | PM_ENC_TYPE))
		{/*add ssid*/ 
			iwpriv_wlan0_vapn_set_mib(id, "ssid", sys_sec->ssid[id].name);

			sys_sec->ssid[id].enable = DEF_SSID_EN;
			
			sprintf(str_val, "%d", sys_sec->ssid[id].isolate_en);
			iwpriv_wlan0_vapn_set_mib(id, "block_relay", str_val);

			sprintf(str_val, "%d", (sys_sec->ssid[id].bcast_en == ENABLE)?DISABLE:ENABLE);
			iwpriv_wlan0_vapn_set_mib(id, "hiddenAP", str_val);

			sprintf(str_val, "%d", sys_sec->ssid[id].enc_type);
			iwpriv_wlan0_vapn_set_mib(id, "encmode", str_val);
			iwpriv_wlan0_vapn_set_mib(id, "psk_enable", str_val);

			if(sys_sec->ssid[id].enable)
			{
				sprintf(str_val, "brctl addif br0 wlan0-vap%d", id);
				system(str_val);
			}
			else
			{
				sprintf(str_val, "brctl delif br0 wlan0-vap%d", id);
				system(str_val);
			}
		}
		else/*edit ssid*/ 
		{
			if (sys->para_mask[0] & PM_SSID_INDEX)
			{
	
			}
			if (sys->para_mask[0] & PM_SSID_NAME)
			{
				iwpriv_wlan0_vapn_set_mib(id, "ssid", sys_sec->ssid[id].name);
			}
			if (sys->para_mask[0] & PM_SSID_ENABLE)
			{
				sys_sec->ssid[id].enable = (sys_sec->ssid[id].enable == ENABLE)?DISABLE:ENABLE;
				if(sys_sec->ssid[id].enable)
				{
					sprintf(str_val, "brctl addif br0 wlan0-vap%d", id);
					system(str_val);
				}
				else
				{
					sprintf(str_val, "brctl delif br0 wlan0-vap%d", id);
					system(str_val);
				}
			}
			if (sys->para_mask[0] & PM_ISOLATE_EN)
			{
				sprintf(str_val, "%d", sys_sec->ssid[id].isolate_en);
				iwpriv_wlan0_vapn_set_mib(id, "block_relay", str_val);
			}
			if (sys->para_mask[0] & PM_BCAST_EN)
			{
				sprintf(str_val, "%d", (sys_sec->ssid[id].bcast_en == ENABLE)?DISABLE:ENABLE);
				iwpriv_wlan0_vapn_set_mib(id, "hiddenAP", str_val);
			}			
			if (sys->para_mask[0] & PM_ENC_TYPE)
			{
				if(sys_sec->ssid[id].enc_type == ENC_WEP)
				{
					if(sys_sec->ssid[id].enc.wep.key_len == _64BIT_10HEX_5ASCII)
					{
						iwpriv_wlan0_vapn_set_mib(id, "encmode", "1");
					}
					else if(sys_sec->ssid[id].enc.wep.key_len == _128BIT_26HEX_13ASCII)
					{
						iwpriv_wlan0_vapn_set_mib(id, "encmode", "5");
					}
					iwpriv_wlan0_vapn_set_mib(id, "psk_enable", "0");
				}
				else if(sys_sec->ssid[id].enc_type == ENC_WPA_PSK)
				{
					iwpriv_wlan0_vapn_set_mib(id, "psk_enable", "1");
				}
				else if(sys_sec->ssid[id].enc_type == ENC_WPA2_PSK)
				{
					iwpriv_wlan0_vapn_set_mib(id, "psk_enable", "2");
				}
				else if(sys_sec->ssid[id].enc_type == ENC_WPA_PSK_WPA2_PSK)
				{
					iwpriv_wlan0_vapn_set_mib(id, "psk_enable", "3");
				}
				else
				{
					iwpriv_wlan0_vapn_set_mib(id, "encmode", "0");
					iwpriv_wlan0_vapn_set_mib(id, "psk_enable", "0");
					wlan_cmd_set_model((id | SSID_MASK), sys_basic->model);
					iwpriv_wlan0_vapn_set_mib(id, "authtype", "2");
				}
			}
	
			if(sys_sec->ssid[id].enc_type == ENC_WEP)
			{
				if (sys->para_mask[0] & PM_AUTHTYPE)
				{
					sprintf(str_val, "%d", sys_sec->ssid[id].enc.wep.authtype);
					iwpriv_wlan0_vapn_set_mib(id, "authtype", str_val);
				}

				if ((sys->para_mask[0] & PM_ENC_WEP_GROUP) 
					|| (sys->para_mask[0] & PM_ENC_WEP_KEY_TYPE)
					|| (sys->para_mask[0] & PM_ENC_WEP_KEY))
				{					
					sprintf(str_val, "%d", (sys_sec->ssid[id].enc.wep.group - 1));
					iwpriv_wlan0_vapn_set_mib(id, "wepdkeyid", str_val);

					if(sys_sec->ssid[id].enc.wep.key_type == ENC_WEP_KEY_TYPE_HEX)
					{
						sprintf(str_val, "%s", sys_sec->ssid[id].enc.wep.key);
					}
					else if(sys_sec->ssid[id].enc.wep.key_type == ENC_WEP_KEY_TYPE_ASCII)
					{
						if(sys_sec->ssid[id].enc.wep.key_len == _64BIT_10HEX_5ASCII)
						{
							sprintf(str_val, "%.2x%.2x%.2x%.2x%.2x", 
								sys_sec->ssid[id].enc.wep.key[0], sys_sec->ssid[id].enc.wep.key[1],
								sys_sec->ssid[id].enc.wep.key[2], sys_sec->ssid[id].enc.wep.key[3],
								sys_sec->ssid[id].enc.wep.key[4]);
						}
						else if(sys_sec->ssid[id].enc.wep.key_len == _128BIT_26HEX_13ASCII)											
						{
							sprintf(str_val, "%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x", 
								sys_sec->ssid[id].enc.wep.key[0], sys_sec->ssid[id].enc.wep.key[1],
								sys_sec->ssid[id].enc.wep.key[2], sys_sec->ssid[id].enc.wep.key[3],
								sys_sec->ssid[id].enc.wep.key[4], sys_sec->ssid[id].enc.wep.key[5],
								sys_sec->ssid[id].enc.wep.key[6], sys_sec->ssid[id].enc.wep.key[7],
								sys_sec->ssid[id].enc.wep.key[8], sys_sec->ssid[id].enc.wep.key[9],
								sys_sec->ssid[id].enc.wep.key[10], sys_sec->ssid[id].enc.wep.key[11],
								sys_sec->ssid[id].enc.wep.key[12]);
						}
					}
					for(i = 1; i < 5; i++)
					{
						sprintf(str_cmd, "wepkey%d", i);
						iwpriv_wlan0_vapn_set_mib(id, str_cmd, str_val);
					}
				}
			}
			else if((sys_sec->ssid[id].enc_type == ENC_WPA_PSK)
					|| (sys_sec->ssid[id].enc_type == ENC_WPA2_PSK)
					|| (sys_sec->ssid[id].enc_type == ENC_WPA_PSK_WPA2_PSK))
			{
				iwpriv_wlan0_vapn_set_mib(id, "authtype", "2");
				if (sys->para_mask[0] & PM_ENC_WPA_KEY)
				{
					sprintf(str_val, "%s", sys_sec->ssid[id].enc.wpa.key);
					iwpriv_wlan0_vapn_set_mib(id, "passphrase", str_val);
				}
				if (sys->para_mask[0] & PM_ENC_WPA_KEY_TYPE)
				{
					iwpriv_wlan0_vapn_set_mib(id, "encmode", "2");
					if(sys_sec->ssid[id].enc.wpa.key_type == ENC_WPA_ENCODE_PRO_TKIP)
					{						
						if(sys_sec->ssid[id].enc_type == ENC_WPA_PSK)
						{
							iwpriv_wlan0_vapn_set_mib(id, "wpa_cipher", "2");
						}
						else if(sys_sec->ssid[id].enc_type == ENC_WPA2_PSK)
						{
							iwpriv_wlan0_vapn_set_mib(id, "wpa2_cipher", "2");
						}
						else if(sys_sec->ssid[id].enc_type == ENC_WPA_PSK_WPA2_PSK)
						{
							iwpriv_wlan0_vapn_set_mib(id, "wpa_cipher", "2");
							iwpriv_wlan0_vapn_set_mib(id, "wpa2_cipher", "2");
						}
					}
					else if(sys_sec->ssid[id].enc.wpa.key_type == ENC_WPA_ENCODE_PRO_AES)
					{
						if(sys_sec->ssid[id].enc_type == ENC_WPA_PSK)
						{
							iwpriv_wlan0_vapn_set_mib(id, "wpa_cipher", "8");
						}
						else if(sys_sec->ssid[id].enc_type == ENC_WPA2_PSK)
						{
							iwpriv_wlan0_vapn_set_mib(id, "wpa2_cipher", "8");
						}
						else if(sys_sec->ssid[id].enc_type == ENC_WPA_PSK_WPA2_PSK)
						{
							iwpriv_wlan0_vapn_set_mib(id, "wpa_cipher", "8");
							iwpriv_wlan0_vapn_set_mib(id, "wpa2_cipher", "8");
						}
						wlan_cmd_set_model((id | SSID_MASK), sys_basic->model);
					}
					else if(sys_sec->ssid[id].enc.wpa.key_type == ENC_WPA_ENCODE_PRO_TKIP_AES)
					{
						if(sys_sec->ssid[id].enc_type == ENC_WPA_PSK)
						{
							iwpriv_wlan0_vapn_set_mib(id, "wpa_cipher", "10");
						}
						else if(sys_sec->ssid[id].enc_type == ENC_WPA2_PSK)
						{
							iwpriv_wlan0_vapn_set_mib(id, "wpa2_cipher", "10");
						}
						else if(sys_sec->ssid[id].enc_type == ENC_WPA_PSK_WPA2_PSK)
						{
							iwpriv_wlan0_vapn_set_mib(id, "wpa_cipher", "10");
							iwpriv_wlan0_vapn_set_mib(id, "wpa2_cipher", "10");
						}
						wlan_cmd_set_model((id | SSID_MASK), sys_basic->model);
					}
				}
				if (sys->para_mask[0] & PM_ENC_WPA_KEY_PERIOD)
				{
					sprintf(str_val, "%d", sys_sec->ssid[id].enc.wpa.period);
					iwpriv_wlan0_vapn_set_mib(id, "gk_rekey", str_val);
				}
			}
			else
			{
				DBG_PRINTF("Enc Type no found!");
			}
		}
	}

	/*access ctr*/
	if(sys->module_mask & WLAN_MM_ACCESS_CTR)
	{
		if((sys->para_mask[0] & PM_FILTER_EN) || (sys->para_mask[0] & PM_FILTER_MODEL))
		{			
			if(sys_aceess->filter_en)
			{				
				if(sys_aceess->filter_model == ONLY_ALLOW)
				{
					iwpriv_wlan0_set_mib("aclmode", "1");
					for(id = 0; id < MAX_SSID; id++)
					{
						iwpriv_wlan0_vapn_set_mib(id, "aclmode", "1");
					}
				}
				else if(sys_aceess->filter_model == ONLY_FORBIDEN)
				{
					iwpriv_wlan0_set_mib("aclmode", "2");
					for(id = 0; id < MAX_SSID; id++)
					{
						iwpriv_wlan0_vapn_set_mib(id, "aclmode", "2");
					}
				}
			}
			else
			{
				iwpriv_wlan0_set_mib("aclmode", "0");
				for(id = 0; id < MAX_SSID; id++)
				{
					iwpriv_wlan0_vapn_set_mib(id, "aclmode", "0");
				}
			}
		}

		if(sys->para_mask[0] & PM_DEL)/*del mac*/
		{
			iwpriv_wlan0_set_mib("aclnum", "0");
			for(id = 0; id < MAX_SSID; id++)
			{
				iwpriv_wlan0_vapn_set_mib(id, "aclnum", "0");
			}
			for(i = 0; i < MAX_FILTER; i++)
			{
				if(sys->del_mask & (1 << i))
				{
					continue;
				}
				else
				{
					if(0 == str_to_mac(mac_addr, sys_aceess->filter[i].mac))
	    			{
						sprintf(str_val, "%.2x%.2x%.2x%.2x%.2x%.2x", 
							mac_addr[0], mac_addr[1], mac_addr[2], 
							mac_addr[3], mac_addr[4], mac_addr[5]);
						iwpriv_wlan0_set_mib("acladdr", str_val);
						for(id = 0; id < MAX_SSID; id++)
						{
							iwpriv_wlan0_vapn_set_mib(id, "acladdr", str_val);
						}
					}
					count++;
				}
				if(sys_aceess->curr_count == count)
					break;
			}
		}
		else if(sys->para_mask[0] & PM_FILTER_MAC)/*add mac*/ 
		{
			iwpriv_wlan0_set_mib("aclnum", "0");
			for(id = 0; id < MAX_SSID; id++)
			{
				iwpriv_wlan0_vapn_set_mib(id, "aclnum", "0");
			}
			for(i = 0; i < MAX_FILTER; i++)
			{
				if(strcmp(sys_aceess->filter[i].mac, BLANK_STR))		
				{
					if(0 == str_to_mac(mac_addr, sys_aceess->filter[i].mac))
		    		{
						sprintf(str_val, "%.2x%.2x%.2x%.2x%.2x%.2x", 
							mac_addr[0], mac_addr[1], mac_addr[2], 
							mac_addr[3], mac_addr[4], mac_addr[5]);
						iwpriv_wlan0_set_mib("acladdr", str_val);
						for(id = 0; id < MAX_SSID; id++)
						{
							iwpriv_wlan0_vapn_set_mib(id, "acladdr", str_val);
						}
					}
				}
			}
		}
		else if(sys->para_mask[0] & PM_FILTER_MAC_DEC)/*edit mac*/ 
		{
			
		}
	}

	/*advance*/
	if(sys->module_mask & WLAN_MM_ADVANCE)
	{		
		if(sys->para_mask[0] & PM_BEACON_INTERVAL)
		{
			sprintf(str_val, "%d", sys_adv->beacon_interval);			
			iwpriv_wlan0_set_mib("bcnint", str_val);			
			for(id = 0; id < MAX_SSID; id++)
			{
				iwpriv_wlan0_vapn_set_mib(id, "bcnint", str_val);
			}			
		}
		if(sys->para_mask[0] & PM_RST_THRESHOLD)
		{
			sprintf(str_val, "%d", sys_adv->rts_threshold);			
			iwpriv_wlan0_set_mib("rtsthres", str_val);			
			for(id = 0; id < MAX_SSID; id++)
			{
				iwpriv_wlan0_vapn_set_mib(id, "rtsthres", str_val);
			}
		}
		if(sys->para_mask[0] & PM_FRAG)
		{
			sprintf(str_val, "%d", sys_adv->frag);			
			iwpriv_wlan0_set_mib("fragthres", str_val);			
			for(id = 0; id < MAX_SSID; id++)
			{
				iwpriv_wlan0_vapn_set_mib(id, "fragthres", str_val);				
			}
		}
		if(sys->para_mask[0] & PM_DTIM_INTERVAL)
		{
			sprintf(str_val, "%d", sys_adv->dtim_interval);			
			iwpriv_wlan0_set_mib("dtimperiod", str_val);			
			for(id = 0; id < MAX_SSID; id++)
			{
				iwpriv_wlan0_vapn_set_mib(id, "dtimperiod", str_val);								
			}
		}
		if(sys->para_mask[0] & PM_TX_POWER)
		{			
			if(pwrlevelCCK_A_value[0] != 0)
			{
				memset(str_val, 0, BUF_SIZE256);
				memcpy(value, pwrlevelCCK_A_value, sizeof(pwrlevelCCK_A_value));
				iwpriv_wlan0_set_mib("use_efuse", "0");
				intVal = getTxPowerScale(sys_adv->tx_power);
				for(i=0; i<=13; i++) {
					if((value[i] - intVal)>=1)
						value[i] -= intVal;
					else
						value[i] = 1;
					sprintf(str_val, "%s%02x", str_val, value[i]);
				}
				iwpriv_wlan0_set_mib("pwrlevelCCK_A", str_val);			
				/*for(id = 0; id < MAX_SSID; id++)
				{
					iwpriv_wlan0_vapn_set_mib(id, "pwrlevelCCK_A", str_val);								
				}*/
			}
			else
			{
				iwpriv_wlan0_set_mib("use_efuse", "1");
			}

			if(pwrlevelHT40_1S_A_value[0] != 0)
			{
				memset(str_val, 0, BUF_SIZE256);
				memcpy(value, pwrlevelHT40_1S_A_value, sizeof(pwrlevelHT40_1S_A_value));
				intVal = getTxPowerScale(sys_adv->tx_power);
				for(i=0; i<=13; i++) {
					if((value[i] - intVal)>=1)
						value[i] -= intVal;
					else
						value[i] = 1;
					sprintf(str_val, "%s%02x", str_val, value[i]);
				}
				iwpriv_wlan0_set_mib("pwrlevelHT40_1S_A", str_val);			
				/*for(id = 0; id < MAX_SSID; id++)
				{
					iwpriv_wlan0_vapn_set_mib(id, "pwrlevelHT40_1S_A", str_val);								
				}*/
			}

			memset(str_val, 0, BUF_SIZE256);
			for(i=0; i<=13; i++)
			{
				sprintf(str_val, "%s%02x", str_val, pwrdiffOFDM_value[i]);
			}
			iwpriv_wlan0_set_mib("pwrdiffOFDM", str_val);
		}
		if(sys->para_mask[0] & PM_WMM_EN)
		{
			sprintf(str_val, "%d", sys_adv->wmm_en);			
			iwpriv_wlan0_set_mib("qos_enable", str_val);			
			for(id = 0; id < MAX_SSID; id++)
			{
				iwpriv_wlan0_vapn_set_mib(id, "qos_enable", str_val);												
			}			
		}		
	}

	/*client*/			
	if(sys->module_mask & WLAN_MM_CLIENT)
	{
	}

	if (sys->para_mask[0] > 0)
	{
		if(sys->module_mask & (WLAN_MM_BASIC | WLAN_MM_ADVANCE | WLAN_MM_ACCESS_CTR))
		{			
			if (!(sys->para_mask[0] & PM_DEL))
			{
				ifconfig_wlan0_status(WALAN_UP);
			}			
		}
		else
		{
			id = sys->wlan_security.curr_index;
			if(sys_sec->ssid[id].enable)
			{
				ifconfig_wlan0_vapn_status(id, WALAN_UP);
			}
		}
	}	
	
	return 0;
}

/*****************************************************************
  Function:        sys_wlan_ap_commit
  Description:     sys_wlan_ap_commit
  Author: 	       huangmingjian
  Date:   		   2013/10/05
  INPUT:           
  OUTPUT:         
  Return:   	   
  Others:	       
*****************************************************************/

int sys_wlan_ap_commit(int apply_options)
{	
	wlan_ap_t *sys = &sys_wlan_ap;
	wlan_security_t *sys_sec = &(sys->wlan_security);
    wlan_access_ctr_t *sys_aceess = &(sys->wlan_access_ctr);
    wlan_advance_t 	*sys_adv = &(sys->wlan_advance);
    wlan_client_t *sys_client = &(sys->wlan_client);
	int id = 0;
	char value[BUF_SIZE_64] = {0};

	/*basic*/
	if(sys->module_mask & WLAN_MM_BASIC)
	{
		if (sys->para_mask[0] & PM_ENABLE)
		{		
            if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_ENABLED, enable_str[sys->wlan_basic.enable]))
            {	
				DBG_PRINTF("Set CONFIG_WLAN_ENABLED faild!");
                return -1;  
            }
		}
		
		if (sys->para_mask[0] & PM_MODEL)
		{
            if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_MODEL, model_str[sys->wlan_basic.model]))
            {	
				DBG_PRINTF("Set CONFIG_WLAN_MODEL faild!");
                return -1;  
            }
		}
		
		if (sys->para_mask[0] & PM_BANDWIDTH)
		{
            if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_BANDWIDTH, bandwidth_str[sys->wlan_basic.bandwidth]))
            {	
				DBG_PRINTF("Set CONFIG_WLAN_BANDWIDTH faild!");
                return -1;  
            }
		}
		
		if (sys->para_mask[0] & PM_CHANNEL)
		{
            if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_CHANNEL, channel_str[sys->wlan_basic.channel]))
            {	
				DBG_PRINTF("Set CONFIG_WLAN_CHANNEL faild!");
                return -1;  
            }
		}
		if (sys->para_mask[0] & PM_TX_RATE)
		{
            if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_TX_RATE, tx_rate_str[sys->wlan_basic.tx_rate]))
            {	
				DBG_PRINTF("Set CONFIG_WLAN_TX_RATE faild!");
                return -1;  
            }
		}
		if (sys->para_mask[1] & PM_UP_SER_VLAN)
		{
			sprintf(value, "%d", sys->wlan_basic.up_ser_vlan);
			if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_UP_SER_VLAN, value))
            {	
				DBG_PRINTF("Set CONFIG_WLAN_UP_SER_VLAN faild!");
                return -1;  
            }
		}		
	}

	/*security*/
	if(sys->module_mask & WLAN_MM_SECURITY)
	{
		id = sys->wlan_security.curr_index;
		if (sys->para_mask[0] & PM_SSID_NAME)
		{
            if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_NAME+id+1, sys_sec->ssid[id].name))
            {	
				DBG_PRINTF("Set CONFIG_WLAN_SSID_NAME faild!");
                return -1;  
            }
		}
		if (sys->para_mask[0] & PM_SSID_ENABLE)
		{
            if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_EN+id+1, enable_str[sys_sec->ssid[id].enable]))
            {	
				DBG_PRINTF("Set CONFIG_WLAN_SSID_EN faild!");
                return -1;  
            }
		}
		if (sys->para_mask[0] & PM_ISOLATE_EN)
		{
            if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ISOLATE_EN+id+1, enable_str[sys_sec->ssid[id].isolate_en]))
            {	
				DBG_PRINTF("Set CONFIG_WLAN_SSID_ISOLATE_EN faild!");
                return -1;  
            }
		}
		if (sys->para_mask[0] & PM_BCAST_EN)
		{
            if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_BCAST_EN+id+1, enable_str[sys_sec->ssid[id].bcast_en]))
            {	
				DBG_PRINTF("Set CONFIG_WLAN_SSID_BCAST_EN faild!");
                return -1;  
            }
		}	
		if (sys->para_mask[0] & PM_ENC_TYPE)
		{
            if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_TYPE+id+1, enc_str[sys_sec->ssid[id].enc_type]))
            {	
				DBG_PRINTF("Set CONFIG_WLAN_SSID_BCAST_EN faild!");
                return -1;  
            }
		}
		
		if(sys_sec->ssid[id].enc_type == ENC_NONE)
		{
			/*Do something: clear all sec data ?*/
		}
		else if(sys_sec->ssid[id].enc_type == ENC_WEP)
		{
			if (sys->para_mask[0] & PM_AUTHTYPE)
			{
				if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_AUTHTYPE+id+1, authtype_str[sys_sec->ssid[id].enc.wep.authtype]))
				{	
					DBG_PRINTF("Set CONFIG_WLAN_AUTHTYPE faild!");
					return -1;	
				}
			}
			if (sys->para_mask[0] & PM_ENC_WEP_GROUP)
			{
				sprintf(value, "%d", sys_sec->ssid[id].enc.wep.group);
				if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_WEP_GROUP+id+1, value))
				{	
					DBG_PRINTF("Set CONFIG_WLAN_SSID_ENC_WEP_GROUP faild!");
					return -1;	
				}
			}
			if (sys->para_mask[0] & PM_ENC_WEP_KEY_LEN)
			{
				if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_WEP_KEY_LEN+id+1, enc_wep_key_len_str[sys_sec->ssid[id].enc.wep.key_len]))
				{	
					DBG_PRINTF("Set CONFIG_WLAN_SSID_ENC_WEP_KEY_LEN faild!");
					return -1;	
				}
			}
			if (sys->para_mask[0] & PM_ENC_WEP_KEY_TYPE)
			{
				if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_WEP_KEY_TYPE+id+1, enc_wep_key_type_str[sys_sec->ssid[id].enc.wep.key_type]))
				{	
					DBG_PRINTF("Set CONFIG_WLAN_SSID_ENC_WEP_KEY_TYPE faild!");
					return -1;	
				}
			}
			if (sys->para_mask[0] & PM_ENC_WEP_KEY)
			{
				if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_WEP_KEY+id+1, sys_sec->ssid[id].enc.wep.key))
				{	
					DBG_PRINTF("Set CONFIG_WLAN_SSID_ENC_WEP_KEY faild!");
					return -1;	
				}
			}
		}
		else if(sys_sec->ssid[id].enc_type > ENC_WEP)
		{
			if (sys->para_mask[0] & PM_ENC_WPA_KEY)
			{
				if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_WPA_KEY+id+1, sys_sec->ssid[id].enc.wpa.key))
				{	
					DBG_PRINTF("Set CONFIG_WLAN_SSID_ENC_WPA_KEY faild!");
					return -1;	
				}
			}
			if (sys->para_mask[0] & PM_ENC_WPA_KEY_TYPE)
			{
					if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_WPA_KEY_TYPE+id+1, enc_wpa_enc_pro_str[sys_sec->ssid[id].enc.wpa.key_type]))
					{	
						DBG_PRINTF("Set CONFIG_WLAN_SSID_ENC_WPA_KEY_TYPE faild!");
						return -1;	
					}
			}
			if (sys->para_mask[0] & PM_ENC_WPA_KEY_PERIOD)
			{
				sprintf(value, "%d", sys_sec->ssid[id].enc.wpa.period);
				if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_SSID_ENC_WPA_KEY_PERIOD+id+1, value))
				{	
					DBG_PRINTF("Set CONFIG_WLAN_SSID_ENC_WPA_KEY_PERIOD faild!");
					return -1;	
				}
			}			
		}
		else
		{
			DBG_PRINTF("Type no found!");
		}
	}

	/*access ctr*/
	if(sys->module_mask & WLAN_MM_ACCESS_CTR)
	{
		if(sys->para_mask[0] & PM_FILTER_EN)
		{
            if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_MAC_FILTER_EN, enable_str[sys_aceess->filter_en]))
            {	
				DBG_PRINTF("Set CONFIG_WLAN_MAC_FILTER_EN faild!");
                return -1;  
            }
		}
		if(sys->para_mask[0] & PM_FILTER_MODEL)
		{
            if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_MAC_FILTER_MODEL, filter_model_str[sys_aceess->filter_model]))
            {	
				DBG_PRINTF("Set CONFIG_WLAN_MAC_FILTER_MODEL faild!");
                return -1;  
            }
		}
		if(sys->para_mask[0] & PM_DEL)   							/*del mac*/
		{	
			for(id=0; id<MAX_FILTER; id++)
			{
				if(sys->del_mask & (1<<id))
				{	
					if(0 != master_cfg_del(IF_ROOT_IFINDEX, CONFIG_WLAN_FILTER_MAC_SECTION+id+1))
					{	
						DBG_PRINTF("Del CONFIG_WLAN_FILTER_MAC_SECTION faild!");
						return -1;	
					}
				}
			}
		}
		else if(sys->para_mask[0] & (PM_FILTER_MAC | PM_FILTER_MAC_DEC))   /*add mac*/ 
		{	
			id = sys_aceess->curr_index;			
			if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_FILTER_MAC_MAC+id+1, sys_aceess->filter[id].mac))
			{	
				DBG_PRINTF("Set CONFIG_WLAN_FILTER_MAC_MAC faild!");
				return -1;	
			}
			if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_FILTER_MAC_MAC_DEC+id+1, sys_aceess->filter[id].dec))
			{	
				DBG_PRINTF("Set CONFIG_WLAN_FILTER_MAC_MAC_DEC faild!");
				return -1;	
			}
		}
		else if(sys->para_mask[0] & PM_FILTER_MAC_DEC)   					 /*edit mac*/ 
		{	
			id = sys_aceess->curr_index;
			if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_FILTER_MAC_MAC_DEC+id+1, sys_aceess->filter[id].dec))
			{	
				DBG_PRINTF("Set CONFIG_WLAN_FILTER_MAC_MAC_DEC faild!");
				return -1;	
			}
		}
	}

	/*advance*/
	if(sys->module_mask & WLAN_MM_ADVANCE)
	{
		if(sys->para_mask[0] & PM_BEACON_INTERVAL)
		{
			sprintf(value, "%d", sys_adv->beacon_interval);
			if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_BEACON_INTERVAL, value))
			{	
				DBG_PRINTF("Set CONFIG_WLAN_BEACON_INTERVAL faild!");
				return -1;	
			}
		}
		if(sys->para_mask[0] & PM_RST_THRESHOLD)
		{
			sprintf(value, "%d", sys_adv->rts_threshold);
			if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_RST_THRESHOLD, value))
			{	
				DBG_PRINTF("Set CONFIG_WLAN_RST_THRESHOLD faild!");
				return -1;	
			}
		}
		if(sys->para_mask[0] & PM_FRAG)
		{
			sprintf(value, "%d", sys_adv->frag);
			if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_FRAG, value))
			{	
				DBG_PRINTF("Set CONFIG_WLAN_FRAG faild!");
				return -1;	
			}
		}
		if(sys->para_mask[0] & PM_DTIM_INTERVAL)
		{
			sprintf(value, "%d", sys_adv->dtim_interval);
			if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_DTIM_INTERVAL, value))
			{	
				DBG_PRINTF("Set CONFIG_WLAN_DTIM_INTERVAL faild!");
				return -1;	
			}
		}
		if(sys->para_mask[0] & PM_TX_POWER)
		{
			sprintf(value, "%d", sys_adv->tx_power);
			if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_TXPOWER, value))
			{	
				DBG_PRINTF("Set CONFIG_WLAN_TXPOWER faild!");
				return -1;	
			}
		}
		if(sys->para_mask[0] & PM_WMM_EN)
		{
			if(0 != master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_WLAN_WMM_EN, enable_str[sys_adv->wmm_en]))
			{	
				DBG_PRINTF("Set CONFIG_WLAN_WMM_EN faild!");
				return -1;	
			}
		}		
	}

	/*client*/			
	if(sys->module_mask & WLAN_MM_CLIENT)
	{

	}
	
	sys->module_mask = MASK_NO_USE;
	sys->para_mask[0] = MASK_NO_USE;
	sys->para_mask[1] = MASK_NO_USE;
	sys->del_mask = MASK_NO_USE;
	
	return 0;
}

int get_wlan_mac_filter(wlan_ap_t *sys) 
{
	
	unsigned int i = 0;
	unsigned int id = 0;
	unsigned int count = 0;
	struct iwreq wrq;
	char buffer[(sizeof(struct iw_quality) + sizeof(struct sockaddr)) * MAX_AP_CLIENT];
	struct sockaddr *hwa = NULL;
	struct iw_quality *qual = NULL;
	int	has_qual = 0;

	if(!sys)
	{
 		DBG_PRINTF("sys is null");
		return -1;
	}
	
	wlan_client_t *client = &(sys->wlan_client);
	wlan_security_t *sys_sec = &(sys->wlan_security);
	
	memset(client, 0, sizeof(wlan_client_t));
	for(id = 0; id < MAX_SSID; id++)
	{
		if(sys_sec->ssid[id].enable)
		{
			/* Collect stats */
			sprintf(wrq.ifr_name, "wlan0-vap%d", id);
			wrq.u.data.pointer = (caddr_t) buffer;
			wrq.u.data.length = 0;
			wrq.u.data.flags = 0;
			if(ioctl(wlan_fd, SIOCGIWAPLIST, &wrq) < 0)
			{
				DBG_PRINTF("%s	Interface doesn't have a list of Access Points\n\n", wrq.ifr_name);
				continue;
			}
	
			/* Number of addresses */
			count = wrq.u.data.length;
			count = count > MAX_AP_CLIENT?MAX_AP_CLIENT:count;
			has_qual = wrq.u.data.flags;
	
			/* The two lists */
			hwa = (struct sockaddr *) buffer;
			qual = (struct iw_quality *) (buffer + (sizeof(struct sockaddr) * count));
	
			for(i = 0; i < count; i++)
			{
				client->block[i].ssid = id;
				safe_strncpy(&(client->block[i].mac), pr_ether(hwa[i].sa_data), MAC_SIZE);
				sprintf(&(client->block[i].tx), "%s", "0.000");
				sprintf(&(client->block[i].rx), "%s", "0.000");
			}
		}
	}

	return 0;
}

#endif

