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
#include <memshare.h>
#include <systemlog.h>



snmp_data_t *g_snmp_data = NULL;
char *snmp_security_str[] = {"noauthnopriv", "authnopriv", "authpriv" ,""};
//char *snmp_version_select[] ={"NONE", "V1", "", "V2", "V1,V2c", "V3", "V1,V3", "ALL", "V2c,V3", "V1,V2c,V3"};
void sys_snmp_load(sys_snmp_t *sys)
{
	int invalid = 0;
	uint8_t i = 0;
	char val[1024];  
	/*char *snmp_version_select[] ={"NONE", "V1", "", "V2", "V1,V2c", "V3", "V1,V3", "ALL", "V2c,V3", "V1,V2c,V3"};
					comein_version	0	    1      2    3       4       5      6       7        8     9*/


	/*Begin add by huangmingjian 2012/12/20 for EPN104QID0089:snmp*/
	/*agent */
    /*snmp_enabled*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_ENABLED, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_en  faild");
    }
	if(!strcmp(val, "enable"))
	{
		sys->snmp_en = 1;
	}
	else if(!strcmp(val, "disable"))
	{
		sys->snmp_en = 0;
	}
	else
	{
		invalid++;
	}
    
	
    /*snmp_engineID*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_ENGINEID, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_engineID  faild");
    }
    if(val[0])
    {       
        safe_strncpy(sys->snmp_engineID, val, sizeof(sys->snmp_engineID));
    }
    else
    {
        invalid++;
    }
    /*snmp_max_packet_len*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_MAX_PACKET_LEN, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_max_packet_len  faild");
    }
	if (val[0])
	{
		sys->snmp_max_packet_len = strtoul(val, NULL, 0);
	}
	else 
	{
		invalid++;
	}

	/*snmp_location*/
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_LOCATION, (void *)sys->snmp_location, "", sizeof(sys->snmp_location))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_location  faild");
    }

    /*snmp_contact*/
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_CONTACT, (void *)sys->snmp_contact, "", sizeof(sys->snmp_contact))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_contact  faild");
    }

    /*snmp_version*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_VERSION, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_version  faild");
    }
	if (val[0])
	{
		sys->snmp_version = strtoul(val, NULL, 0);
        if(NULL != strstr(val, "V1"))
        {
            sys->snmp_version= SNMP_V1_VAL;
        }
        else if(NULL != strstr(val, "V2c"))
        {
            sys->snmp_version= SNMP_V2c_VAL;
        }
        else if(NULL != strstr(val, "V3"))
        {
            sys->snmp_version= SNMP_V3_VAL;
        }		
	}
	else 
	{
		invalid++;
	}
    
	/*snmp_trust_host*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRUST_HOST, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trust_host  faild");
    }
    if(val[0])
    {       
        safe_strncpy(sys->snmp_trust_host, val, sizeof(sys->snmp_trust_host));
    }
    else
    {
        invalid++;
    }
	
	/*add community*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_COMM_CURRENT_COUNT, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_comm_current_count  faild");
    }
	if (val[0])
	{
		sys->snmp_comm_current_count = strtoul(val, NULL, 0);
	}
	else 
	{
		invalid++;
	}
	
	for(i=0; i< MAX_SNMP_COMM_COUNT; i++)
	{	
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_COMM_NAME+i+1, (void *)sys->snmp_comm[i].snmp_comm_name, "", sizeof(sys->snmp_comm[i].snmp_comm_name))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_comm_name  faild");
            return;
        }

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_ACCESS_MODE+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_access_mode  faild");
            return;
        }

		if(!strcmp(val, "read_write"))
		{
			sys->snmp_comm[i].snmp_access_mode = SNMP_ACESS_MODE_RW;
		}
		else if(!strcmp(val, "read_only"))
		{
			sys->snmp_comm[i].snmp_access_mode = SNMP_ACESS_MODE_RO;
		}
		else 
		{
		    sys->snmp_comm[i].snmp_access_mode=SNMP_ACESS_MODE_NULL;//NULL
		}
	}

	
	/*add*/
	sys->snmp_add_which_num = NO_ADD_NUM; 
	/*del */
	sys->snmp_del_which_num = NO_DEL_NUM; 

	sys->snmp_set_flag = 0;
	/*End add by huangmingjian 2012/12/20 for EPN104QID0089:snmp*/


	/*trap*/
    //TRAP add by zhouguanhua 2012/12/20  
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_EN, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_en  faild");
    }

	if (val[0]){
		sys->snmp_trap_en = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}
	
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_IP, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_ip  faild");
    }

	if (!val[0] || !inet_aton(val, &sys->snmp_trap_ip)){
		invalid ++;		
	}

    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_COM, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_com  faild");
    }
    /*snmp_trap_com*/

	if (val[0]){
        sys->snmp_trap_com = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}

    /*snmp_trap_community*/
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_COMMUNITY, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_community  faild");
    }

	if (val[0]){
		strncpy(sys->snmp_trap_community, val, sizeof(sys->snmp_trap_community));
	}else {
		invalid ++;
	}

    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_VERSION, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_version  faild");
    }

	if (val[0]){
		sys->snmp_trap_version = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}

    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_SAFE, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_trap_safe  faild");
    }

	if (val[0]){
        sys->snmp_trap_safe = strtoul(val, NULL, 0);
	}else {
		invalid ++;
	}

    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_LAST_SAVE_SAFE_NUM, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_last_save_safe_num  faild");
    }

	if (val[0]){
        sys->snmp_last_save_safe_num = strtoul(val, NULL, 0);
	}else {
		invalid ++;
    }
     //end add by zhouguanhua 2012/12/20
	/*Begin add by huangmingjian 2012/12/22 for EPN104QID0089:snmp*/
	/*add group*/

     memset(val, 0, sizeof(val));  
     if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_CURRENT_COUNT, (void *)val, "", sizeof(val))) < 0)
     {
         DBG_ASSERT(0, "Get snmp_group_current_count  faild");
     }

	if (val[0])
	{
		sys->snmp_group_current_count = strtoul(val, NULL, 0);
	}
	else 
	{
		invalid++;
	}
	
	for(i=0; i< MAX_SNMP_GROUP_COUNT; i++)
	{	
         if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_NAME+i+1, (void *)sys->snmp_group[i].snmp_group_name, "", sizeof(sys->snmp_group[i].snmp_group_name))) < 0)
         {
             DBG_ASSERT(0, "Get snmp_group_current_count  faild");
             return;
         }		

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_SECURITY+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_group_current_count  faild");
            return;
        }
		if(!strcmp(val, "noauthnopriv"))
		{
			sys->snmp_group[i].snmp_group_security = SNMP_GROUP_SECU_NOAUTHNOENCRYPT;
		}
		else if(!strcmp(val, "authnopriv"))
		{
			sys->snmp_group[i].snmp_group_security = SNMP_GROUP_SECU_AUTHNOENCRYPT;
		}
		else if(!strcmp(val, "authpriv"))
		{
			sys->snmp_group[i].snmp_group_security = SNMP_GROUP_SECU_AUTHENCRYPT;
		}
		else
		{
			sys->snmp_group[i].snmp_group_security = SNMP_GROUP_SECU_NULL;
		}

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_MODE+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_group_mode  faild");
            return;
        }

		if(!strcmp(val, "read_write"))
		{
			sys->snmp_group[i].snmp_group_mode = SNMP_ACESS_MODE_RW;
		}
		else if(!strcmp(val, "read_only"))
		{
			sys->snmp_group[i].snmp_group_mode = SNMP_ACESS_MODE_RO;
		}
		else
		{   
		    sys->snmp_group[i].snmp_group_mode = SNMP_ACESS_MODE_NULL;
		}
	}	
	
	/*End add by huangmingjian 2012/12/22 for EPN104QID0089:snmp*/
	/*add user*/
	//add by zhouguanhua 2012/12/24
    memset(val, 0, sizeof(val));  
    if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_USER_CURRENT_COUNT, (void *)val, "", sizeof(val))) < 0)
    {
        DBG_ASSERT(0, "Get snmp_user_current_count  faild");
    }

	if (val[0])
	{
		sys->snmp_user_current_count = strtoul(val, NULL, 0);
	}
	else 
	{
		invalid++;
	}

	for(i=0; i< MAX_SNMP_USER_COUNT; i++)
	{	
        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_USER_NMAE+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_name  faild");
            return;
        }	
		safe_strncpy(sys->snmp_user[i].snmp_user_name, val, sizeof(sys->snmp_user[i].snmp_user_name));

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_USER_GROUP+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_group  faild");
            return;
        }	
		safe_strncpy(sys->snmp_user[i].snmp_user_group, val, sizeof(sys->snmp_user[i].snmp_user_group));

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_PASSWORD+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_password  faild");
            return;
        }	
		safe_strncpy(sys->snmp_user[i].snmp_user_password, val, sizeof(sys->snmp_user[i].snmp_user_password));

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_ENPASSWORD+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_enpassword  faild");
            return;
        }	
		safe_strncpy(sys->snmp_user[i].snmp_user_enpassword, val, sizeof(sys->snmp_user[i].snmp_user_enpassword));

        memset(val, 0, sizeof(val));  
        if((cfg_getval(IF_ROOT_IFINDEX, CONFIG_SNMP_SECURITY+i+1, (void *)val, "", sizeof(val))) < 0)
        {
            DBG_ASSERT(0, "Get snmp_user_name  faild");
            return;
        }	
        
		if(!strcmp(val, "noauthnopriv"))
		{
			sys->snmp_user[i].snmp_user_security = SNMP_USER_SECU_NOAUTHNOENCRYPT;
		}
		else if(!strcmp(val, "authnopriv"))
		{
			sys->snmp_user[i].snmp_user_security = SNMP_USER_SECU_AUTHNOENCRYPT;
		}
		else if(!strcmp(val, "authpriv"))
		{
			sys->snmp_user[i].snmp_user_security = SNMP_USER_SECU_AUTHENCRYPT;
		}
		else 
		{
			sys->snmp_user[i].snmp_user_security = SNMP_USER_SECU_NULL;
		}


	}	
	//end add by zhouguanhua 2012/12/24	
	


	if (invalid > 0)
	{
		DBG_ASSERT(0, "Invalid SYS SNMP INFO");
	}

	if(snmpdconf_access() < 0)
	{
		DBG_PRINTF(" ERROR: snmpdconf access failed in snmp load!");
	    return; 	
	}
	
	return;
}

static int sys_start_snmpd(timer_element_t *timer, void *data)
{
    startup_process(SNMP_STARTUP_CMD); 
    return TIMER_RUN_ONCE;
}

//static int sys_start_snmpd(timer_element_t *timer, void *data);//modify by zhouguanhua 2013/2/28 for EPN204QID0021
/*****************************************************************
    Function:snmpd_init
    Description: startup snmp if necessary
    Author:huangmingjian
    Date:2012/12/19
    Input:     
    Output:         
    Return:
    Other:
*****************************************************************/
void snmpd_init()
{
	g_snmp_data = snmp_data_shmInit();
	if(PROCESS_ENABLE == sys_snmp.snmp_en)
	{
	    timer_register(11000, 0, (timer_func_t)sys_start_snmpd, NULL, NULL, "snmpdstart");//modify by zhouguanhua 2013/2/28 for EPN204QID0021
	}
	return;
}

int sys_snmp_apply(int apply_options, int access_flag)
{

	sys_snmp_t *sys = &sys_snmp; 
	pid_t pid = 0;
	
	#define SNMPD_ON (1)
	#define SNMPTRAP_ON (1)
	#define SNMPTRAP_OFF (0)
	
	/*Begin add by huangmingjian 2012/12/20 for EPN104QID0089:snmp*/ 
	if(access_flag == NEED_REREAD_CONF)
	{
		if(snmpdconf_access() < 0)
		{
			DBG_PRINTF(" ERROR: snmpdconf access failed in snmp apply!");
			sys->snmp_set_flag = 0;
			clr_update(snmp_en);
		    return -1; 
		}

		if((!test_update(snmp_en)) && (PROCESS_ENABLE == sys->snmp_en) && 
			!(sys->snmp_set_flag & DONT_TRIGER_SIGNAL))
		{	
    		if((pid = get_pid_by_name(SNMPD_NAME)) > 0)
    		{   
    			g_snmp_data->access = MASTER_TO_SNMP_SET;
				g_snmp_data->type = REREAD_CONFIG;
                if(-1 == kill(pid, SIGUSR2))
                {   
                    DBG_PRINTF(" Tell snmp reread config error, pid:%d\n", pid);
					sys->snmp_set_flag = 0;
					clr_update(snmp_en);
                    return -1;
                }          
    		}			
		}
	}
	
	/*End add by huangmingjian 2012/12/20 for EPN104QID0089:snmp*/ 
	/*agent */
	if(sys->snmp_set_flag & SNMP_AGENT_FLAG)
	{	
		/* snmp_en */	
		if (test_update(snmp_en))
		{	
			if(set_process_status(SNMPD_NAME, SNMP_STARTUP_CMD, sys->snmp_en) < 0)
	      	{
	       		DBG_PRINTF(" ERROR: set snmp status failed!");
				sys->snmp_set_flag = 0;
				clr_update(snmp_en);
	       		return -1; 
	      	}

		}
	}

	return 0;
}


int sys_snmp_update(sys_snmp_t *cfg, int *access_flag)
{
	sys_snmp_t *sys = &sys_snmp;    
    uint8_t which_num = 0;
    int invalid = 0;
    int snmp_en_change = 0;

	if(!cfg || !access_flag)
	{
		return 0;
	}
	
	/*Begin add by huangmingjian 2012/12/20 for EPN104QID0089:snmp*/
	sys->snmp_set_flag = cfg->snmp_set_flag;
	/*agent update*/
	if(sys->snmp_set_flag & SNMP_AGENT_FLAG)
	{	
		DBG_PRINTF("agent flag:%04X", sys->snmp_set_flag & SNMP_AGENT_FLAG);  
		if(sys->snmp_en != cfg->snmp_en)
		{	
			  sys->snmp_en = cfg->snmp_en;
			  snmp_en_change = 1;
			  set_update(snmp_en);	
		}
		if (strcmp(sys->snmp_engineID, cfg->snmp_engineID))
		{
			safe_strncpy(sys->snmp_engineID, cfg->snmp_engineID, sizeof(sys->snmp_engineID));
			invalid++;
		}
		if(sys->snmp_max_packet_len != cfg->snmp_max_packet_len)
		{	
			sys->snmp_max_packet_len = cfg->snmp_max_packet_len;
			invalid++;
		}
		if (strcmp(sys->snmp_location, cfg->snmp_location))
		{
			safe_strncpy(sys->snmp_location, cfg->snmp_location, sizeof(sys->snmp_location));
			invalid++;
		}
		if (strcmp(sys->snmp_contact, cfg->snmp_contact))
		{	
			safe_strncpy(sys->snmp_contact, cfg->snmp_contact, sizeof(sys->snmp_contact));
			invalid++;
		}
		if(sys->snmp_version != cfg->snmp_version)
		{	
			sys->snmp_version = cfg->snmp_version;
			invalid++;
		}
		if (strcmp(sys->snmp_trust_host, cfg->snmp_trust_host))
		{
			safe_strncpy(sys->snmp_trust_host, cfg->snmp_trust_host, sizeof(sys->snmp_trust_host));
			invalid++;
		}
	}
	/*add community update*/
	if(sys->snmp_set_flag & SNMP_ADD_COMM_FLAG)
	{		
		DBG_PRINTF("add community flag:%04X", sys->snmp_set_flag & SNMP_ADD_COMM_FLAG);  
		if(sys->snmp_comm_current_count != cfg->snmp_comm_current_count)
		{	
			sys->snmp_comm_current_count = cfg->snmp_comm_current_count;
			if((sys->snmp_add_which_num == NO_ADD_NUM) && (sys->snmp_add_which_num != cfg->snmp_add_which_num))
			{
				sys->snmp_add_which_num = cfg->snmp_add_which_num;
			}
			else
			{	
				sys->snmp_set_flag &= ~SNMP_ADD_COMM_FLAG;
				DBG_PRINTF("snmp add comm num error:%d", sys->snmp_add_which_num); 
		        return 0;
			}
			which_num = sys->snmp_add_which_num;
			safe_strncpy(sys->snmp_comm[which_num].snmp_comm_name, cfg->snmp_comm[which_num].snmp_comm_name, sizeof(sys->snmp_comm[which_num].snmp_comm_name));
			sys->snmp_comm[which_num].snmp_access_mode = cfg->snmp_comm[which_num].snmp_access_mode;
			invalid++;
		}
	}
	/*del community update*/
	if(sys->snmp_set_flag & SNMP_DEL_COMM_FLAG)
	{		
		DBG_PRINTF("del community flag:%04X", sys->snmp_set_flag & SNMP_DEL_COMM_FLAG);  
		if((sys->snmp_del_which_num == NO_DEL_NUM) && (sys->snmp_del_which_num != cfg->snmp_del_which_num))
		{	
			sys->snmp_del_which_num = cfg->snmp_del_which_num;
			which_num = sys->snmp_del_which_num;
			safe_strncpy(sys->snmp_comm[which_num].snmp_comm_name, "", sizeof(sys->snmp_comm[which_num].snmp_comm_name));
			sys->snmp_comm[which_num].snmp_access_mode = SNMP_ACESS_MODE_RO;
			if(sys->snmp_comm_current_count> 0)
			{
				sys->snmp_comm_current_count--;					
			}
			invalid++;
		}
		else
		{	
			sys->snmp_set_flag &= ~SNMP_DEL_COMM_FLAG;
			DBG_PRINTF("snmp del comm num error:%d", sys->snmp_del_which_num); 
			return 0;
		}
	}
	/*End add by huangmingjian 2012/12/20 for EPN104QID0089:snmp*/
	/*trap update*/
	if(sys->snmp_set_flag & SNMP_TRAP_FLAG)
	{
		if(sys->snmp_trap_en != cfg->snmp_trap_en)
		{	
			  sys->snmp_trap_en = cfg->snmp_trap_en;
			  invalid++;
		}
		if (!ip_equal(sys->snmp_trap_ip, cfg->snmp_trap_ip)) 
		{	
		      ip_set(sys->snmp_trap_ip, cfg->snmp_trap_ip);
			  invalid++;
		}
		if(sys->snmp_trap_com != cfg->snmp_trap_com)
		{	
			sys->snmp_trap_com = cfg->snmp_trap_com;
			invalid++;
		}
		if (strcmp(sys->snmp_trap_community, cfg->snmp_trap_community))
		{
			safe_strncpy(sys->snmp_trap_community, cfg->snmp_trap_community, sizeof(sys->snmp_trap_community));
			invalid++;
		}
		if(sys->snmp_trap_version != cfg->snmp_trap_version)
		{	
			sys->snmp_trap_version = cfg->snmp_trap_version;
			invalid++;
		}	
		if(sys->snmp_trap_safe != cfg->snmp_trap_safe)
		{	
			sys->snmp_trap_safe = cfg->snmp_trap_safe;
			invalid++;
		}		
		if(sys->snmp_last_save_safe_num != cfg->snmp_last_save_safe_num)
		{	
			sys->snmp_last_save_safe_num = cfg->snmp_last_save_safe_num;
			invalid++;	
		}	
	}
	/*Begin add by huangmingjian 2012/12/22 for EPN104QID0089:snmp*/ 
	/*add group update*/
	if(sys->snmp_set_flag & SNMP_ADD_GROUP_FLAG)
	{
		DBG_PRINTF("add community flag:%04X", sys->snmp_set_flag & SNMP_ADD_GROUP_FLAG);  
		if(sys->snmp_group_current_count != cfg->snmp_group_current_count)
		{	
			sys->snmp_group_current_count = cfg->snmp_group_current_count;
			if((sys->snmp_add_which_num == NO_ADD_NUM) && (sys->snmp_add_which_num != cfg->snmp_add_which_num))
			{
				sys->snmp_add_which_num = cfg->snmp_add_which_num;
			}
			else
			{	
				sys->snmp_set_flag &= ~SNMP_ADD_GROUP_FLAG;
				DBG_PRINTF("snmp add gruop num error:%d", sys->snmp_add_which_num); 
				return 0;
			}
			which_num = sys->snmp_add_which_num;
			safe_strncpy(sys->snmp_group[which_num].snmp_group_name, cfg->snmp_group[which_num].snmp_group_name, sizeof(sys->snmp_group[which_num].snmp_group_name));
			sys->snmp_group[which_num].snmp_group_security = cfg->snmp_group[which_num].snmp_group_security;
			sys->snmp_group[which_num].snmp_group_mode = cfg->snmp_group[which_num].snmp_group_mode;	
			invalid++;
		}			
	}
	/*del group update*/
	if(sys->snmp_set_flag & SNMP_DEL_GROUP_FLAG)
	{
		DBG_PRINTF("del community flag:%04X", sys->snmp_set_flag & SNMP_DEL_GROUP_FLAG);  
		if((sys->snmp_del_which_num == NO_DEL_NUM) && (sys->snmp_del_which_num != cfg->snmp_del_which_num))
		{	
			sys->snmp_del_which_num = cfg->snmp_del_which_num;
			which_num = sys->snmp_del_which_num;
			safe_strncpy(sys->snmp_group[which_num].snmp_group_name, "", sizeof(sys->snmp_group[which_num].snmp_group_name));
			sys->snmp_group[which_num].snmp_group_security = SNMP_GROUP_SECU_NULL;
			sys->snmp_group[which_num].snmp_group_mode = SNMP_ACESS_MODE_NULL;
			if(sys->snmp_group_current_count> 0)
			{
				sys->snmp_group_current_count--;					
			}	
			invalid++;
		}
		else
		{	
			sys->snmp_set_flag &= ~SNMP_DEL_GROUP_FLAG;
			DBG_PRINTF("snmp del gruop num error:%d", sys->snmp_del_which_num); 
			return 0;
		}
	}
	/*End add by huangmingjian 2012/12/22 for EPN104QID0089:snmp*/ 
	/*add user update*/
	if(sys->snmp_set_flag & SNMP_ADD_USER_FLAG)
    {
		DBG_PRINTF("add user flag:%04X", cfg->snmp_set_flag & SNMP_ADD_USER_FLAG);  
		if(sys->snmp_user_current_count != cfg->snmp_user_current_count)
		{	
			sys->snmp_user_current_count = cfg->snmp_user_current_count;
			if((sys->snmp_add_which_num == NO_ADD_NUM) &&(sys->snmp_add_which_num != cfg->snmp_add_which_num))
			{
				sys->snmp_add_which_num = cfg->snmp_add_which_num;
			}
			else
			{	
				sys->snmp_set_flag &= ~SNMP_ADD_USER_FLAG;
				DBG_PRINTF("snmp add user which num error:%d", sys->snmp_add_which_num); 
				return 0;
			}
			which_num = sys->snmp_add_which_num;
			safe_strncpy(sys->snmp_user[which_num].snmp_user_name, cfg->snmp_user[which_num].snmp_user_name, sizeof(sys->snmp_user[which_num].snmp_user_name));
            safe_strncpy(sys->snmp_user[which_num].snmp_user_group, cfg->snmp_user[which_num].snmp_user_group, sizeof(sys->snmp_user[which_num].snmp_user_group));
            safe_strncpy(sys->snmp_user[which_num].snmp_user_password, cfg->snmp_user[which_num].snmp_user_password, sizeof(sys->snmp_user[which_num].snmp_user_password));
            safe_strncpy(sys->snmp_user[which_num].snmp_user_enpassword, cfg->snmp_user[which_num].snmp_user_enpassword, sizeof(sys->snmp_user[which_num].snmp_user_enpassword));
			sys->snmp_user[which_num].snmp_user_security = cfg->snmp_user[which_num].snmp_user_security;
			invalid++;
		}
	}
	/*del user update*/
	if(sys->snmp_set_flag & SNMP_DEL_USER_FLAG)
	{
		DBG_PRINTF("del user flag:%04X", sys->snmp_set_flag & SNMP_DEL_USER_FLAG);  
		if((sys->snmp_del_which_num == NO_DEL_NUM) && (sys->snmp_del_which_num != cfg->snmp_del_which_num))
		{	
			sys->snmp_del_which_num = cfg->snmp_del_which_num;
			which_num = sys->snmp_del_which_num;			
			safe_strncpy(sys->snmp_user[which_num].snmp_user_name, "", sizeof(sys->snmp_user[which_num].snmp_user_name));
			safe_strncpy(sys->snmp_user[which_num].snmp_user_group, "", sizeof(sys->snmp_user[which_num].snmp_user_group));
			safe_strncpy(sys->snmp_user[which_num].snmp_user_password, "", sizeof(sys->snmp_user[which_num].snmp_user_password));
			safe_strncpy(sys->snmp_user[which_num].snmp_user_enpassword, "", sizeof(sys->snmp_user[which_num].snmp_user_enpassword));
			sys->snmp_user[which_num].snmp_user_security = SNMP_USER_SECU_NULL;
			if(sys->snmp_user_current_count> 0)
			{
				sys->snmp_user_current_count--; 				
			}
			invalid++;
		}
		else
		{	
			sys->snmp_set_flag &= ~SNMP_DEL_USER_FLAG;
			DBG_PRINTF("snmp del user which num error:%d", sys->snmp_del_which_num); 
			return 0;
		}			
	}

	if(0==snmp_en_change && PROCESS_ENABLE==sys->snmp_en && invalid>0)  /*alway keep enable, and other have change , need reread config*/
	{
		*access_flag = NEED_REREAD_CONF;
	}
	else
	{
		*access_flag = DONT_REREAD_CONF;
	}

	if(1==snmp_en_change && 0==invalid) 
	{
		invalid++;
	}
	
	return invalid;
}



int sys_snmp_commit(int apply_options)
{

	/*Begin add by huangmingjian 2012/12/20 for EPN104QID0089:snmp*/ 
	sys_snmp_t *sys = &sys_snmp; 
	char *snmp_en_str[] = {"disable", "enable"};
	char val[254] = {0};
	char *snmp_mode_str[] = {"read_only", "read_write",""};
	uint8_t type  = 0;
	/*agent */
	if(sys->snmp_set_flag & SNMP_AGENT_FLAG)
	{	
		if (test_update(snmp_en))
		{
            if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_ENABLED, snmp_en_str[sys_snmp.snmp_en]))
            {
                return -1;  
            }
			//add by leijinbao for 281 
			if(1 == sys_snmp.snmp_en)
			{
				 USER_LOG(LangId,SNMP,LOG_INFO,M_SNMP_STA,"",""); //add by leijinbao 2013/9/17
			}
			else
			{
				 USER_LOG(LangId,SNMP,LOG_INFO,M_SNMP_EXIT,"",""); //add by leijinbao 2013/9/17
			}
			//end by leijinbao 2013/11/21 
			clr_update(snmp_en);
		}
				
		/* snmp_engineID */		
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_ENGINEID, sys_snmp.snmp_engineID))
        {
            return -1;  
        }
		
		/* snmp_max_packet_len */	
		memset(val, 0, sizeof(val));
		sprintf(val, "%d", sys_snmp.snmp_max_packet_len);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_MAX_PACKET_LEN, val))
        {
            return -1;  
        }

		/* snmp_location */
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_LOCATION, sys_snmp.snmp_location))
        {
            return -1;  
        }
		
		/* snmp_contact */
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_CONTACT, sys_snmp.snmp_contact))
        {
            return -1;  
        }

		/* snmp_version */
		memset(val, 0, sizeof(val));
        if(sys_snmp.snmp_version ==SNMP_V1_VAL)
        {           
            safe_strncpy(val,"V1" , sizeof(val));           
        }
        else if(sys_snmp.snmp_version ==SNMP_V2c_VAL)
        {           
            safe_strncpy(val,"V2c" , sizeof(val));           
        }
        else if(sys_snmp.snmp_version ==SNMP_V3_VAL)
        {           
            safe_strncpy(val,"V3" , sizeof(val));           
        } 
        
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_VERSION, val))
        {
            return -1;  
        }


		/* snmp_trust_host */
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRUST_HOST, sys_snmp.snmp_trust_host))
        {
            return -1;  
        }		
		/*clean up the flag*/
		sys->snmp_set_flag &= ~SNMP_AGENT_FLAG; 
	}

	/*add community*/
	if(sys->snmp_set_flag & SNMP_ADD_COMM_FLAG)
	{		
		/* snmp_comm_current_count */	
		memset(val, 0, sizeof(val));
		sprintf(val, "%d", sys->snmp_comm_current_count);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_COMM_CURRENT_COUNT, val))
        {
            return -1;  
        }

		/* snmp_comm_name */
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_COMM_NAME+sys->snmp_add_which_num+1, sys->snmp_comm[sys->snmp_add_which_num].snmp_comm_name))
        {
            return -1;  
        }

		/* snmp_access_mode */
		type = sys->snmp_comm[sys->snmp_add_which_num].snmp_access_mode;
			
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_ACCESS_MODE+sys->snmp_add_which_num+1, snmp_mode_str[type]))
        {
            return -1;  
        }
		
		sys->snmp_add_which_num = NO_ADD_NUM;
		/*clean up the flag*/
		sys->snmp_set_flag &= ~SNMP_ADD_COMM_FLAG; 
	}

	/*del community*/
	if(sys->snmp_set_flag & SNMP_DEL_COMM_FLAG)
	{	
		/* snmp_del_comm_which_num */
			/* snmp_comm_current_count */	
			memset(val, 0, sizeof(val));
			sprintf(val, "%d", sys->snmp_comm_current_count);
            if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_COMM_CURRENT_COUNT, val)) 
            {
                return -1;  
            }


			/* snmp_comm_name */
            if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_COMM_NAME+sys->snmp_del_which_num+1, sys->snmp_comm[sys->snmp_del_which_num].snmp_comm_name))
            {
                return -1;  
            }

			/* snmp_access_mode */
			type = sys->snmp_comm[sys->snmp_del_which_num].snmp_access_mode;
            if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_ACCESS_MODE+sys->snmp_del_which_num+1, snmp_mode_str[type]))
            {
                return -1;  
            }

			sys->snmp_del_which_num = NO_DEL_NUM; 
		
		/*clean up the flag*/
		sys->snmp_set_flag &= ~SNMP_DEL_COMM_FLAG; 	
	}
	/*End add by huangmingjian 2012/12/20 for EPN104QID0089:snmp*/

	/*trap*/
	if(sys->snmp_set_flag & SNMP_TRAP_FLAG)
	{	
		DBG_PRINTF("sys->snmp_set_flag:%04X", (sys->snmp_set_flag & SNMP_TRAP_FLAG));	
		/* snmp_trap_en */	
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_EN, sys_snmp.snmp_trap_en ? "1" : "0"))
        {
            return -1;  
        }

		/* snmp_trap_ip */
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_IP, inet_ntoa(sys_snmp.snmp_trap_ip)))
        {
            return -1;  
        }


		/* snmp_trap_com */
		memset(val, 0, sizeof(val));
		sprintf(val, "%d", sys_snmp.snmp_trap_com);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_COM, val))
        {
            return -1;  
        }

		/* snmp_trap_community */
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_COMMUNITY, sys_snmp.snmp_trap_community))
        {
            return -1;  
        }

		/* snmp_trap_version */
        memset(val, 0, sizeof(val));
        sprintf(val, "%d", sys_snmp.snmp_trap_version);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_VERSION, val))
        {
            return -1;  
        }

		/* snmp_trap_safe */
        memset(val, 0, sizeof(val));
        sprintf(val, "%d", sys_snmp.snmp_trap_safe);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_TRAP_SAFE, val))
        {
            return -1;  
        }

		/* snmp_trap_safe */
        memset(val, 0, sizeof(val));
        sprintf(val, "%d", sys_snmp.snmp_last_save_safe_num);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_LAST_SAVE_SAFE_NUM, val))
        {
            return -1;  
        }

		/*clean up the flag*/
		sys->snmp_set_flag &= ~SNMP_TRAP_FLAG; 	
	}

	
	/*Begin add by huangmingjian 2012/12/22 for EPN104QID0089:snmp*/ 
	/*add group*/
	if(sys->snmp_set_flag & SNMP_ADD_GROUP_FLAG)
	{
		/* snmp_group_current_count */	
		memset(val, 0, sizeof(val));
		sprintf(val, "%d", sys->snmp_group_current_count);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_CURRENT_COUNT, val))
        {
            return -1;  
        }
		
		/* snmp_group_name */
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_NAME+1+sys->snmp_add_which_num, sys->snmp_group[sys->snmp_add_which_num].snmp_group_name))
        {
            return -1;  
        }

		/* snmp_group_security */
		type = sys->snmp_group[sys->snmp_add_which_num].snmp_group_security;
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_SECURITY+1+sys->snmp_add_which_num, snmp_security_str[type]))
        {
            return -1;  
        }

		/* snmp_group_mode */
		type = sys->snmp_group[sys->snmp_add_which_num].snmp_group_mode;
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_MODE+1+sys->snmp_add_which_num, snmp_mode_str[type]))
        {
            return -1;  
        }

		sys->snmp_add_which_num = NO_ADD_NUM;
		
		/*clean up the flag*/
		sys->snmp_set_flag &= ~SNMP_ADD_GROUP_FLAG; 	
	}

	/*del group*/
	if(sys->snmp_set_flag & SNMP_DEL_GROUP_FLAG)
	{	
		/* snmp_group_current_count */	
		memset(val, 0, sizeof(val));
		sprintf(val, "%d", sys->snmp_group_current_count);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_CURRENT_COUNT, val))
        {
            return -1;  
        }


		/* snmp_group_name */
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_NAME+1+sys->snmp_del_which_num, sys->snmp_group[sys->snmp_del_which_num].snmp_group_name))
        {
            return -1;  
        }

		/* snmp_group_security */
		type = sys->snmp_group[sys->snmp_del_which_num].snmp_group_security;
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_SECURITY+1+sys->snmp_del_which_num, snmp_security_str[type]))
        {
            return -1;  
        }

		/* snmp_group_mode */
		type = sys->snmp_group[sys->snmp_del_which_num].snmp_group_mode;
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_GROUP_MODE+1+sys->snmp_del_which_num, snmp_mode_str[type]))
        {
            return -1;  
        }

		sys->snmp_del_which_num = NO_DEL_NUM; 

		/*clean up the flag*/
		sys->snmp_set_flag &= ~SNMP_DEL_GROUP_FLAG; 	
	}

	/*End add by huangmingjian 2012/12/22 for EPN104QID0089:snmp*/ 

	/*add user*/
	if(sys->snmp_set_flag & SNMP_ADD_USER_FLAG)
	{
		/* snmp_user_current_count */
		memset(val, 0, sizeof(val));
		sprintf(val, "%d", sys->snmp_user_current_count);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_USER_CURRENT_COUNT, val))
        {
            return -1;  
        }

		/* snmp_user_name */
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_USER_NMAE+1+sys->snmp_add_which_num, sys->snmp_user[sys->snmp_add_which_num].snmp_user_name))
        {
            return -1;  
        }

		/* snmp_user_group */
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_USER_GROUP+1+sys->snmp_add_which_num, sys->snmp_user[sys->snmp_add_which_num].snmp_user_group))
        {
            return -1;  
        }

		/* snmp_user_password */
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_PASSWORD+1+sys->snmp_add_which_num, sys->snmp_user[sys->snmp_add_which_num].snmp_user_password))
        {
            return -1;  
        }

		/* snmp_user_enpassword */
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_ENPASSWORD+1+sys->snmp_add_which_num, sys->snmp_user[sys->snmp_add_which_num].snmp_user_enpassword))
        {
            return -1;  
        }

		/* snmp_user_security */
		type = sys->snmp_user[sys->snmp_add_which_num].snmp_user_security;
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_SECURITY+1+sys->snmp_add_which_num, snmp_security_str[type]))
        {
            return -1;  
        }

		sys->snmp_add_which_num = NO_ADD_NUM;

		/*clean up the flag*/
		sys->snmp_set_flag &= ~SNMP_ADD_USER_FLAG; 	
	}

	/*del user */
	if(sys->snmp_set_flag & SNMP_DEL_USER_FLAG)
	{
		/* snmp_user_current_count */	
        memset(val, 0, sizeof(val));
        sprintf(val, "%d", sys->snmp_user_current_count);
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_USER_CURRENT_COUNT, val))
        {
            return -1;  
        }


		/* snmp_user_name */
        if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_USER_NMAE+1+sys->snmp_del_which_num, sys->snmp_user[sys->snmp_del_which_num].snmp_user_name))
        {
            return -1;  
        }

		/* snmp_user_group */
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_USER_GROUP+1+sys->snmp_del_which_num, sys->snmp_user[sys->snmp_del_which_num].snmp_user_group))
        {
            return -1;  
        }

		/* snmp_user_password */
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_PASSWORD+1+sys->snmp_del_which_num, sys->snmp_user[sys->snmp_del_which_num].snmp_user_password))
        {
            return -1;  
        }

		/* snmp_user_enpassword */
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_ENPASSWORD+1+sys->snmp_del_which_num, sys->snmp_user[sys->snmp_del_which_num].snmp_user_enpassword))
        {
            return -1;  
        }

		/* snmp_user_security */
		type = sys->snmp_user[sys->snmp_del_which_num].snmp_user_security;
		if(0 !=master_cfg_setval(IF_ROOT_IFINDEX, CONFIG_SNMP_SECURITY+1+sys->snmp_del_which_num, snmp_security_str[type]))
        {
            return -1;  
        }

		sys->snmp_del_which_num = NO_DEL_NUM; 

		/*clean up the flag*/
		sys->snmp_set_flag &= ~SNMP_DEL_USER_FLAG; 	
	}
	sys->snmp_set_flag = 0;

	return 1;
}


/*****************************************************************
    Function:add_enter
    Description:add '\n' for a sting.
    Author:huangmingjian
    Date:2012/12/27
    Input:   string  
    Output:         
    Return:
   
=================================================
    Note:
*****************************************************************/  
void add_enter(char *string)
{	
	int len = strlen(string);
	if(string[len] != '\n' )	
	{		
		string[len] = '\n';	
	}	
	return;
}


/*****************************************************************
    Function:add_line_to_snmpdconf
    Description:add line to snmpdconf
    Author:huangmingjian
    Date:2013/01/01
    Input:   stream: the stream we open
    		  arg: the strings we want to cat.
    		 
    Output:         
    Return: error -1
    		  ok    :0
=================================================
    Note:
*****************************************************************/ 

int add_line_to_snmpdconf(FILE *stream, char *arg, ... ) 
{  	
	va_list argp;				   	
	uint8_t argno = 0;			    	
	char *para = NULL;			    	
	char line[MAX_LINE_STRING_SIZE] = {0};	

	if(NULL == stream)
	{	
		DBG_PRINTF("error:stream is NULL in keyword:%s", arg);			
		return -1;
	}
	va_start(argp, arg); 
	
	while(1)
	{  			
		para = va_arg( argp, char *);     		
		if (NULL == para)               
		{			
			break;  		
		}				
		if(0 != argno)		
		{	
			strcat(line , " ");		
		}		
		strcat(line, para);		
		argno++; 	
	}  	
	va_end(argp);                               
	DBG_PRINTF("line:%d   %s", argno, line);	
	add_enter(line);	
	fputs(line, stream);	
	return 0;
}


/*****************************************************************
    Function:get_snmp_group_mode
    Description:get snmp group mode from the group
    Author:huangmingjian
    Date:2013/01/22
    Input:   group
    Output:  group mode:        
    Return: error :-1
    		  ok    :SNMP_ACESS_MODE_RO SNMP_ACESS_MODE_RW
=================================================
    Note:
*****************************************************************/  

int get_snmp_group_mode(char *group_name)
{
	uint8_t i = 0;
	char ret = -1;
	sys_snmp_t *sys = &sys_snmp;

	if(NULL == group_name)
	{	
		DBG_PRINTF("error: group name is NULL");
		return ret;
	}
	for(i=0; i<MAX_SNMP_GROUP_COUNT; i++)
	{	
		if(!strcmp(sys->snmp_group[i].snmp_group_name, group_name))
		{	
			return 	sys->snmp_group[i].snmp_group_mode;
		}
	}
	
	return ret;
}



/*****************************************************************
    Function:snmpdconf_access
    Description:access snmpd.conf file.
    Author:huangmingjian
    Date:2012/12/27
    Input:  
    Output:         
    Return: error :-1
    		  ok    :0
=================================================
    Note:
*****************************************************************/  

int snmpdconf_access(void)
{	
	sys_snmp_t *sys = &sys_snmp;
	FILE *stream = NULL;	
	uint8_t i = 0;
	char buf[64] = {0};
	
#define V1  (1)
#define V2C (2)
#define V3  (3)
#define TRAP_EN (1)

	if(remove(SNMPDCONFI_PATH) < 0)
	{
		DBG_PRINTF("remove %s error:%s", SNMPDCONFI_PATH, strerror(errno));		
	}
	
	if ((stream = fopen(SNMPDCONFI_PATH, "w+")) == NULL) 	
	{			
		DBG_PRINTF("open %s error:%s", SNMPDCONFI_PATH, strerror(errno));
		return -1;	
	}

	add_line_to_snmpdconf(stream, NULL, "# Access Control", NULL);	
	add_line_to_snmpdconf(stream, NULL, "", NULL);
	
	add_line_to_snmpdconf(stream, NULL, "#community", NULL);				
	for(i=0; i<MAX_SNMP_COMM_COUNT; i++)
	{
		if(strcmp(sys->snmp_comm[i].snmp_comm_name, ""))
		{
			if(SNMP_ACESS_MODE_RO == sys->snmp_comm[i].snmp_access_mode)
			{
				add_line_to_snmpdconf(stream, NULL, "rocommunity", sys->snmp_comm[i].snmp_comm_name, sys->snmp_trust_host, NULL);				
			}
			else if(SNMP_ACESS_MODE_RW == sys->snmp_comm[i].snmp_access_mode)
			{	
				add_line_to_snmpdconf(stream, NULL, "rwcommunity", sys->snmp_comm[i].snmp_comm_name, sys->snmp_trust_host, NULL);				
			}	
		}
	}
	add_line_to_snmpdconf(stream, NULL, "", NULL);
	
    add_line_to_snmpdconf(stream, NULL, "exactEngineID", sys->snmp_engineID, NULL);
	
	memset(buf, '\0', sizeof(buf));
	sprintf(buf,"%d", sys->snmp_version);
	add_line_to_snmpdconf(stream, NULL, "snmpVersion", buf, NULL);
	memset(buf, '\0', sizeof(buf));
	sprintf(buf,"%d", sys->snmp_max_packet_len);
	add_line_to_snmpdconf(stream, NULL, "engineMaxMessageSize", buf, NULL);

	add_line_to_snmpdconf(stream, NULL, "", NULL);
	
	add_line_to_snmpdconf(stream, NULL, "#group and user", NULL);
	for(i=0; i<MAX_SNMP_USER_COUNT; i++)
	{
		if(strcmp(sys->snmp_user[i].snmp_user_name, ""))
		{	
			memset(buf, '\0', sizeof(buf));
			strncpy(buf, "comm_", sizeof(buf));
			strcat(buf, sys->snmp_user[i].snmp_user_name);
			
			add_line_to_snmpdconf(stream, NULL, "com2sec", sys->snmp_user[i].snmp_user_name, sys->snmp_trust_host, buf, NULL);
			add_line_to_snmpdconf(stream, NULL, "group", sys->snmp_user[i].snmp_user_group, "v1", sys->snmp_user[i].snmp_user_name, NULL);
			add_line_to_snmpdconf(stream, NULL, "group", sys->snmp_user[i].snmp_user_group, "v2c", sys->snmp_user[i].snmp_user_name, NULL);
			add_line_to_snmpdconf(stream, NULL, "group", sys->snmp_user[i].snmp_user_group, "usm", sys->snmp_user[i].snmp_user_name, NULL);
			
			add_line_to_snmpdconf(stream, NULL, "view", "all", "included", ".1", "80", NULL);
			add_line_to_snmpdconf(stream, NULL, "view", "all", "included", ".1", "80", NULL);
			
			add_line_to_snmpdconf(stream, NULL, "access", 
												sys->snmp_user[i].snmp_user_group, 
												"\"\"",
												"usm",
								  				snmp_security_str[sys->snmp_user[i].snmp_user_security],
								  				"exact", 
								  				"all", 
								 				((get_snmp_group_mode(sys->snmp_user[i].snmp_user_group))==SNMP_ACESS_MODE_RW?"all":"none"),
								 				"none", 
								 				NULL);
		    if(SNMP_USER_SECU_NOAUTHNOENCRYPT == sys->snmp_user[i].snmp_user_security)
			{
				add_line_to_snmpdconf(stream, NULL, "createUser", 
													sys->snmp_user[i].snmp_user_name,										
													NULL);					
			}
			else if(SNMP_USER_SECU_AUTHNOENCRYPT == sys->snmp_user[i].snmp_user_security)
			{
				add_line_to_snmpdconf(stream, NULL, "createUser", 
													sys->snmp_user[i].snmp_user_name,
													"MD5", 
													sys->snmp_user[i].snmp_user_password,
													NULL);					
			}
			else if(SNMP_USER_SECU_AUTHENCRYPT == sys->snmp_user[i].snmp_user_security)
			{
				add_line_to_snmpdconf(stream, NULL, "createUser", 
													sys->snmp_user[i].snmp_user_name,
													"MD5", 
													sys->snmp_user[i].snmp_user_password,
													"DES",
													sys->snmp_user[i].snmp_user_enpassword,
													NULL);				
			}

				

			add_line_to_snmpdconf(stream, NULL, "", NULL);
		}
	}




	add_line_to_snmpdconf(stream, NULL, "# System information", NULL);	
	add_line_to_snmpdconf(stream, NULL, "", NULL);
	add_line_to_snmpdconf(stream, NULL, "syslocation", sys->snmp_location, NULL);	
	add_line_to_snmpdconf(stream, NULL, "syscontact", sys->snmp_contact, NULL);	
	add_line_to_snmpdconf(stream, NULL, "", NULL); 

	//add by zhouguanhua 2013/1/11
	memset(buf, '\0', sizeof(buf));
    sprintf(buf, "%s:%d",inet_ntoa(sys->snmp_trap_ip), sys->snmp_trap_com);

    if(sys->snmp_trap_en==TRAP_EN)
    {
    	add_line_to_snmpdconf(stream, NULL, "# trap", NULL);
    	if(sys->snmp_trap_version==V1)
    	{
    	    add_line_to_snmpdconf(stream, NULL, "trapsink",buf,sys->snmp_trap_community, NULL);	
        }
        else if(sys->snmp_trap_version==V2C)
        {
    	    add_line_to_snmpdconf(stream, NULL, "trap2sink",buf,sys->snmp_trap_community, NULL);	
        }
        else if(sys->snmp_trap_version==V3)
        {
          //trapsess -r 0 -Ci -v 3 -u myuser -n "" -l authPriv -a MD5 -A myauthpass -x DES -X myencrpass 192.168.1.77:1177
          if(sys->snmp_trap_safe ==SNMP_USER_SECU_NOAUTHNOENCRYPT)
          {
        	  add_line_to_snmpdconf(stream, NULL, 
        	                    "trapsess -v 3 -u",sys->snmp_trap_community,
        	                    "-l",snmp_security_str[sys->snmp_trap_safe],
                                buf, NULL);
          }
          else if(sys->snmp_trap_safe ==SNMP_USER_SECU_AUTHNOENCRYPT)
          {
        	  add_line_to_snmpdconf(stream, NULL, 
        	                    "trapsess -v 3 -u",sys->snmp_trap_community,
        	                    "-a MD5 -A ",sys->snmp_user[sys->snmp_last_save_safe_num].snmp_user_password,
        	                    "-l",snmp_security_str[sys->snmp_trap_safe],
                                buf, NULL);
          }
          else if(sys->snmp_trap_safe ==SNMP_USER_SECU_AUTHENCRYPT)
          {
        	  add_line_to_snmpdconf(stream, NULL, 
        	                    "trapsess -v 3 -u",sys->snmp_trap_community,
        	                    "-a MD5 -A ",sys->snmp_user[sys->snmp_last_save_safe_num].snmp_user_password,
        	                    "-x DES -X",sys->snmp_user[sys->snmp_last_save_safe_num].snmp_user_enpassword,
        	                    "-l",snmp_security_str[sys->snmp_trap_safe],
                                buf, NULL);
           }
        }

        add_line_to_snmpdconf(stream, NULL, "authtrapenable", "1",NULL);
    }
	
	if(fclose(stream) <0 )	
	{			
		DBG_PRINTF("fclose %s error:%s", SNMPDCONFI_PATH, strerror(errno));		
		return -1;	
	}	

	return 0;	

}

