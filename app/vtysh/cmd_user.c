/*****************************************************************************

      File name:cmd_user.c
      Description:provide funcs of user interface
      Author:liaohongjun
      Date:
              2012/8/17
==========================================================
      Note:
*****************************************************************************/

#ifdef  __cplusplus
    extern "C"{
#endif

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include "eoc_utils.h"
#include "debug.h"
#include "cmd_eoc.h"
#include "user.h"
#include "vty.h"
#include <lw_type.h>
#include <lw_config_oid.h>
#include <lw_config_api.h>
#include <lw_if_pub.h>

VTY_USER_INFO_S vty_user[MAX_VTY_COUNT] = {0};

struct cmd_node vty_node = { VTY_NODE, NULL, NULL, 1, };

static const unsigned char itoa64[] =
    "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void to64(char *s, long v, int n) {
    while (--n >= 0) {
        *s++ = itoa64[v & 0x3f];
        v >>= 6;
    }
}

static char *zencrypt(const char *passwd)
{
    char salt[6];
    struct timeval tv;
    char *crypt(const char *, const char *);

    gettimeofday(&tv, 0);

    to64(&salt[0], random(), 3);
    to64(&salt[3], tv.tv_usec, 3);
    salt[5] = '\0';

    return crypt(passwd, salt);
}

/*****************************************************************
    Function:exec_timeout
    Description: set vty user time out value 
    Author:liaohongjun
    Date:2012/8/17
    Input:struct vty *vty, const char *min_str, const char *sec_str     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS
=================================================
    Note:
*****************************************************************/
static int exec_timeout(struct vty *vty, const char *min_str, const char *sec_str)
{
    struct vty *vty_tmp = NULL;
    unsigned long timeout = 0;
    char name[BUF_SIZE_64] = {0};
    char val[BUF_SIZE_64] = {0};
    /*begin modified by liaohongjun 2012/9/3 of QID0015*/
    int iRet = IPC_STATUS_OK;
    /* min_str and sec_str are already checked by parser.  So it must be all digit string. */
    if(NULL == min_str && NULL == sec_str)
    {
        timeout = strtoul(DEFAULT_VTY_TIMEOUT, NULL, 10);
    }   
    else
    {
        if (min_str) 
        {
            timeout = strtoul(min_str, NULL, 10);
            timeout *= 60;
        }
        if (sec_str)
        {
            timeout += strtoul(sec_str, NULL, 10);
        }
    }
    
    sprintf(name, PARTS_VTY_TIMEOUT_ITEM, vty->para.para_integer);
    sprintf(val,"%ld",timeout);
    iRet = ipc_cfg_set_user(name, val);//modify by zhouguanhua 2013/6/5
    if (IPC_STATUS_OK != iRet)
    {
        vty_out(vty, " ERROR: set exec_timeout failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }
    /*end modified by liaohongjun 2012/9/3 of QID0015*/
    vty_user[vty->para.para_integer].timeout = timeout;
    if(vty_user[vty->para.para_integer].active)
    {
        if(NULL != (vty_tmp = vector_slot (vtyvec, vty_user[vty->para.para_integer].vty_sockfd)))    
        {
            /*begined modified by liaohongjun 2012/9/25 of EPN104QID0061*/
            if((AUTH_NODE != vty_tmp->node) && (AUTH_PASSWD_NODE != vty_tmp->node))
            {
                vty_tmp->v_timeout = timeout;
                vty_event(VTY_TIMEOUT_RESET, 0, vty_tmp);
            }
            /*end modified by liaohongjun 2012/9/25 of EPN104QID0061*/
        }
    }
    return CMD_SUCCESS;
}
/*****************************************************************
    Function:vty_user_init
    Description: init vty user's configration 
    Author:liaohongjun
    Date:2012/8/17
    Input:     
    Output:         
    Return:ret
=================================================
    Note:
*****************************************************************/
int vty_user_init(void)
{
    int i = 0;
    
    int ret = IPC_STATUS_OK;
    ipc_all_vty_user_info_t vty_user_info;

    memset(&vty_user_info, 0x0, sizeof(ipc_all_vty_user_info_t));
    ret = ipc_get_vty_user_info_all(&vty_user_info);
    if (IPC_STATUS_OK != ret)
    {
        printf(" ERROR: vty user init failed!\n\r");
        return ret;        
    }

    for(i = 0; i < MAX_VTY_COUNT; i++)
    {
        memcpy(vty_user[i].username, vty_user_info.vty_user[i].username, sizeof(vty_user[i].username));
        memcpy(vty_user[i].passwd, vty_user_info.vty_user[i].passwd, sizeof(vty_user[i].passwd));
        memcpy(vty_user[i].authmode, vty_user_info.vty_user[i].authmode, sizeof(vty_user[i].authmode));
        memcpy(vty_user[i].passwd_type, vty_user_info.vty_user[i].passwd_type, sizeof(vty_user[i].passwd_type));
        vty_user[i].timeout = vty_user_info.vty_user[i].timeout;
        //DBG_PRINTF("vty_user[%d].username = %s",i,vty_user[i].username);
        //DBG_PRINTF("vty_user[%d].passwd = %s",i,vty_user[i].passwd);
        //DBG_PRINTF("vty_user[%d].authmode = %s",i,vty_user[i].authmode);
        //DBG_PRINTF("vty_user[%d].passwd_type = %s",i,vty_user[i].passwd_type);
        //DBG_PRINTF("vty_user[%d].timeout = %ld",i,vty_user[i].timeout);
    }
    
    return ret;
}

/*****************************************************************
    Function:access_vty
    Description: access vty mode 
    Author:liaohongjun
    Date:2012/8/17
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS
=================================================
    Note:
*****************************************************************/
DEFUN (access_vty,
        access_vty_cmd,
        "user-config vty <0-2>",
        "Enter user configure mode\n"
        "Virtual terminal\n"
        "Virtual terminal index\n")
{        
    vty->para.para_integer = strtoul(argv[0], NULL, 0);
    switch(vty->para.para_integer)
    {
        case 0:
        case 1:
        case 2:
            break;
        default:
            vty_out(vty, " ERROR: invalid paras%s", VTY_NEWLINE);
            return CMD_WARNING;
    }
 
    vty->node = VTY_NODE;
    return CMD_SUCCESS;
}
/*begin modified by liaohongjun 2012/9/3 of QID0015*/
/*****************************************************************
    Function:vty_username
    Description: set username of vty index 
    Author:liaohongjun
    Date:2012/8/17
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (vty_username,
        vty_username_cmd,
        "username WORD<1-16>",
        "Specify username of this virtual terminal\n"
        "Input username of this virtual terminal\n")
{
    char name[BUF_SIZE_64] = {0};
    int iRet = IPC_STATUS_OK;

    if(!strstr(self->string, "no"))
    {
        if((!strlen(argv[0])) || (strlen(argv[0]) > MAX_VTY_USERNAME_LEN))
        {
            vty_out(vty, " ERROR: invalid lengths of username!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        sprintf(name, PARTS_VTY_USERNAME_ITEM, vty->para.para_integer);
        iRet = ipc_cfg_set_user(name, (char *)argv[0]);//modify by zhouguanhua 2013/6/10
        if (IPC_STATUS_OK != iRet)
        {
            vty_out(vty, " ERROR: set username failed!%s", VTY_NEWLINE);
            return CMD_WARNING;        
        }

        memset(vty_user[vty->para.para_integer].username, 0x0, sizeof(vty_user[vty->para.para_integer].username));    
        strcpy(vty_user[vty->para.para_integer].username, (char *)argv[0]);
    }
    else
    {
        sprintf(name, PARTS_VTY_USERNAME_ITEM, vty->para.para_integer);
        iRet = ipc_cfg_set_user(name, DEFAULT_USERNAME);//modify by zhouguanhua 2013/6/10
        if (IPC_STATUS_OK != iRet)
        {
            vty_out(vty, " ERROR: set username failed!%s", VTY_NEWLINE);
            return CMD_WARNING;        
        }

        memset(vty_user[vty->para.para_integer].username, 0x0, sizeof(vty_user[vty->para.para_integer].username));    
        strcpy(vty_user[vty->para.para_integer].username, DEFAULT_USERNAME);
    }
    //DBG_PRINTF("vty_user[%d].username = %s",vty->para.para_integer, vty_user[vty->para.para_integer].username);
    return CMD_SUCCESS;
}

ALIAS (vty_username,
        no_vty_username_cmd,
        "no username",
        NO_DESC_STR
        "Specify username of this virtual terminal\n");

/*****************************************************************
    Function:vty_password
    Description: set password of vty index 
    Author:liaohongjun
    Date:2012/8/17
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (vty_password,
        vty_password_cmd,
        "password (cleartext|ciphertext) WORD<1-32>",
        "Assign the terminal connection password\n"
        "Specifies a cleartext password will follow\n"
        "Specifies a ciphertext password will follow\n"
        "Input password of this virtual terminal\n")
{
    char name[BUF_SIZE_64] = {0};
    char passwd_val[MAX_VTY_PASSWD_LEN+1] = {0};
    int iRet = IPC_STATUS_OK;
    
    if(!strstr(self->string, "no"))
    {
        memset(name, 0x0, sizeof(name));
        sprintf(name, PARTS_VTY_PASSWD_TYPE_ITEM, vty->para.para_integer);
        iRet = ipc_cfg_set_user(name, (char *)argv[0]); //modify by zhouguanhua 2013/6/10
        if (IPC_STATUS_OK != iRet)
        {
            vty_out(vty, " ERROR: set password type failed!%s", VTY_NEWLINE);
            return CMD_WARNING;        
        }        
        memset(vty_user[vty->para.para_integer].passwd_type, 0x0, sizeof(vty_user[vty->para.para_integer].passwd_type));
        strcpy(vty_user[vty->para.para_integer].passwd_type, (char *)argv[0]);    

        if (argc == 2) 
        {
            if((!strlen(argv[1])) || (strlen(argv[1]) > MAX_VTY_PASSWD_LEN))
            {
                vty_out(vty, " ERROR: invalid lengths of password!%s", VTY_NEWLINE);
                return CMD_WARNING;
            }   
            
            if (0 == strcmp(argv[0],PASSWD_TYPE_CLEARTEXT)) 
            {
                safe_strncpy(passwd_val, (char *)argv[1], sizeof(passwd_val));
            } 
            else
            {
                safe_strncpy(passwd_val, zencrypt((char *)argv[1]), sizeof(passwd_val));
            }
        }
        //DBG_PRINTF("set passwd_val = %s",passwd_val);  
        memset(name, 0x0, sizeof(name));
        sprintf(name, PARTS_VTY_AUTHPASSWD_ITEM, vty->para.para_integer);
        iRet = ipc_cfg_set_user(name, passwd_val);//modify by zhouguanhua 2013/6/5
        if (IPC_STATUS_OK != iRet)
        {
            vty_out(vty, " ERROR: set password failed!%s", VTY_NEWLINE);
            return CMD_WARNING;        
        }   
        memset(vty_user[vty->para.para_integer].passwd, 0x0, sizeof(vty_user[vty->para.para_integer].passwd));
        strcpy(vty_user[vty->para.para_integer].passwd, passwd_val);         
    }
    else
    {
        memset(name, 0x0, sizeof(name));
        sprintf(name, PARTS_VTY_PASSWD_TYPE_ITEM, vty->para.para_integer);
        iRet = ipc_cfg_set_user(name, DEFAULT_PASSWD_TYPE);//modify by zhouguanhua 2013/6/5
        if (IPC_STATUS_OK != iRet)
        {
            vty_out(vty, " ERROR: set password type failed!%s", VTY_NEWLINE);
            return CMD_WARNING;        
        }   
        memset(vty_user[vty->para.para_integer].passwd_type, 0x0, sizeof(vty_user[vty->para.para_integer].passwd_type));
        strcpy(vty_user[vty->para.para_integer].passwd_type, DEFAULT_PASSWD_TYPE);

        memset(name, 0x0, sizeof(name));
        sprintf(name, PARTS_VTY_AUTHPASSWD_ITEM, vty->para.para_integer); 
        iRet = ipc_cfg_set_user(name, DEFAULT_VTY_PASSWD);//modify by zhouguanhua 2013/6/5
        if (IPC_STATUS_OK != iRet)
        {
            vty_out(vty, " ERROR: set password failed!%s", VTY_NEWLINE);
            return CMD_WARNING;        
        }     
        memset(vty_user[vty->para.para_integer].passwd, 0x0, sizeof(vty_user[vty->para.para_integer].passwd));
        strcpy(vty_user[vty->para.para_integer].passwd, DEFAULT_VTY_PASSWD);          
    }

    //DBG_PRINTF("vty_user[%d].passwd_type = %s",vty->para.para_integer, vty_user[vty->para.para_integer].passwd_type);
    //DBG_PRINTF("vty_user[%d].passwd = %s",vty->para.para_integer, vty_user[vty->para.para_integer].passwd);
    return CMD_SUCCESS;
}

ALIAS (vty_password,
        no_vty_password_cmd,
        "no password",
        NO_DESC_STR
        "Assign the terminal connection password\n");

/*****************************************************************
    Function:enable_vty_auth_mode
    Description: set authentication mode of vty index 
    Author:liaohongjun
    Date:2012/8/17
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (enable_vty_auth_mode,
        enable_auth_mode_cmd,
        "enable auth-mode (none|password)",
        ENABLE_DESC_STR
        "Specifies the Terminal connection authentication mode\n"
        "Login without any authentication\n"
        "Access authentication by use terminal password\n")
{
    char name[BUF_SIZE_64] = {0};
    char auth_mode_val[MAX_AUTHMODE_TXT_LEN+1] = {0};
    int iRet = IPC_STATUS_OK; 

    if(strstr(AUTH_MODE_NONE, (char *)argv[0]))
    {
        sprintf(auth_mode_val, "%s", AUTH_MODE_NONE);
    }
    else if(strstr(AUTH_MODE_PASSWORD, (char *)argv[0])) 
    {
        sprintf(auth_mode_val, "%s", AUTH_MODE_PASSWORD);
    }
    else
    {
        vty_out(vty, " ERROR: set auth-mode paras error!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }     
    
    sprintf(name, PARTS_VTY_AUTHMODE_ITEM, vty->para.para_integer);
    iRet = ipc_cfg_set_user(name, auth_mode_val);
    if (IPC_STATUS_OK != iRet)
    {
        vty_out(vty, " ERROR: set auth-mode failed!%s", VTY_NEWLINE);
        return CMD_WARNING;        
    }

    memset(vty_user[vty->para.para_integer].authmode, 0x0, sizeof(vty_user[vty->para.para_integer].authmode));
    strcpy(vty_user[vty->para.para_integer].authmode, auth_mode_val);
    //DBG_PRINTF("vty_user[%d].authmode = %s",vty->para.para_integer, vty_user[vty->para.para_integer].authmode);
    return CMD_SUCCESS;
}
/*end modified by liaohongjun 2012/9/3 of QID0015*/

/*****************************************************************
    Function:exec_timeout_min
    Description: set exec-timeout mins of vty index 
    Author:liaohongjun
    Date:2012/8/17
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (exec_timeout_min,
        exec_timeout_min_cmd,
        "exec-timeout <0-35791>",
        "Set timeout value\n"
        "Timeout value in minutes\n")
{
    return exec_timeout(vty, argv[0], NULL);
}

/*****************************************************************
    Function:exec_timeout_sec
    Description: set exec-timeout secs of vty index 
    Author:liaohongjun
    Date:2012/8/17
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (exec_timeout_sec,
        exec_timeout_sec_cmd,
        "exec-timeout <0-35791> <0-59>",
        "Set the EXEC timeout\n"
        "Timeout in minutes\n"
        "Timeout in seconds\n")
{
    return exec_timeout(vty, argv[0], argv[1]);
}

/*****************************************************************
    Function:exec_timeout_sec
    Description:set exec-timeout to default setting of vty index 
    Author:liaohongjun
    Date:2012/8/17
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
DEFUN (exec_timeout_defaults,
        no_exec_timeout_cmd,
        "no exec-timeout",
        NO_DESC_STR
        "Set the EXEC timeout\n")
{
    return exec_timeout(vty, NULL, NULL);
}

/*****************************************************************
    Function:func_vty_node_show_current
    Description:show vty mode current config 
    Author:liaohongjun
    Date:2012/8/17
    Input:     
    Output:         
    Return:
        CMD_WARNING
        CMD_SUCCESS    
=================================================
    Note:
*****************************************************************/
int func_vty_node_show_current(struct vty *vty)
{
    int i = 0;
    int ret = IPC_STATUS_OK;
    unsigned long timeout_min = 0;
    unsigned long timeout_sec = 0;
	ifindex_t ifindex=IF_DOMAIN_IFINDEX(IF_SUB_ROOT,IF_DOMAIN_VIRTUAL,0);
	char val[MAX_CONFIG_VAL_SIZE] = {0};
	
    ipc_all_vty_user_info_t vty_user_info;

    if(NULL == vty)
    {
        return CMD_WARNING;
    }    

    memset(&vty_user_info, 0x0, sizeof(ipc_all_vty_user_info_t));
	if(vty->cfgdomain==0)
	{
	    ret = ipc_get_vty_user_info_all(&vty_user_info);
	    if (IPC_STATUS_OK != ret)
	    {
	        vty_out(vty, " ERROR: get vty user info failed! %s",VTY_NEWLINE);
	        return CMD_WARNING;        
	    } 
	}
	else
	{
		
		for(i = 0; i < MAX_VTY_COUNT; i++)
		{
		   if(0 != cfg_getval(ifindex,CONFIG_USER_VTY_NAME+i+1,vty_user_info.vty_user[i].username,"default",sizeof(vty_user_info.vty_user[i].username)))
		   {
			   vty_out(vty, " ERROR: get CONFIG_USER_VTY_NAME failed! %s",VTY_NEWLINE);
			   return CMD_WARNING;
		   }
		   if(0 != cfg_getval(ifindex,CONFIG_USER_VTY_PASSWORD+i+1,vty_user_info.vty_user[i].passwd,"default",sizeof(vty_user_info.vty_user[i].passwd)))
		   {
			   vty_out(vty, " ERROR: get CONFIG_USER_VTY_PASSWORD failed! %s",VTY_NEWLINE);
			   return CMD_WARNING;			  
		   }
		   if(0 != cfg_getval(ifindex,CONFIG_USER_VTY_AUTHMODE+i+1,vty_user_info.vty_user[i].authmode,"default",sizeof(vty_user_info.vty_user[i].authmode)))
		   {
			   vty_out(vty, " ERROR: get CONFIG_USER_VTY_AUTHMODE failed! %s",VTY_NEWLINE);
			   return CMD_WARNING;			   
		   }
		   if(0 != cfg_getval(ifindex,CONFIG_USER_VTY_PASSWORD_TYPE+i+1,vty_user_info.vty_user[i].passwd_type,"default",sizeof(vty_user_info.vty_user[i].passwd_type)))
		   {
			   vty_out(vty, " ERROR: get CONFIG_USER_VTY_PASSWORD_TYPE failed! %s",VTY_NEWLINE);
			   return CMD_WARNING;			   
		   }
		   if(0 != cfg_getval(ifindex,CONFIG_USER_VTY_TIMEOUT+i+1,(void *)val,"default",sizeof(val)))
		   {
			   vty_out(vty, " ERROR: get CONFIG_USER_VTY_TIMEOUT failed! %s",VTY_NEWLINE);
			   return CMD_WARNING;			   
		   }

		   if (val[0])
		   {
				   vty_user_info.vty_user[i].timeout = strtoul(val, NULL, 0);
		   }
		 
	   }
	}
    
    for(i = MIN_VTY_USER_NO; i < MAX_VTY_COUNT; i++)
    {
        if(VTY_USER_NO_OF_CONSOLE == i)
        {
            
            vty_out(vty, "user-config vty %d for console user%s", i, VTY_NEWLINE);

            if(strcmp(vty_user_info.vty_user[i].authmode, DEFAULT_CONSOL_AUTH_MODE))
            {
                vty_out(vty, " enable auth-mode %s%s", vty_user_info.vty_user[i].authmode, VTY_NEWLINE);
            }
     
        }
        else
        {
            vty_out(vty, "user-config vty %d for telnet user%s", i, VTY_NEWLINE);
            if(strcmp(vty_user_info.vty_user[i].authmode, DEFAULT_TELNET_AUTH_MODE))
            {
                vty_out(vty, " enable auth-mode %s %s", vty_user_info.vty_user[i].authmode, VTY_NEWLINE);
            }
        }
        if(strcmp(vty_user_info.vty_user[i].passwd_type, DEFAULT_PASSWD_TYPE) || \
            strcmp(vty_user_info.vty_user[i].passwd, DEFAULT_VTY_PASSWD))
        {
            vty_out(vty, " password %s %s %s", vty_user_info.vty_user[i].passwd_type, \
                vty_user_info.vty_user[i].passwd, VTY_NEWLINE);
        }               
        if((unsigned long)strtoul(DEFAULT_VTY_TIMEOUT, NULL, 10) != vty_user_info.vty_user[i].timeout)
        {
            timeout_min = vty_user_info.vty_user[i].timeout/60;
            timeout_sec = vty_user_info.vty_user[i].timeout%60;
            if(timeout_sec)
            {
                vty_out(vty, " exec-timeout %ld %ld %s", timeout_min, timeout_sec, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, " exec-timeout %ld %s", timeout_min, VTY_NEWLINE);
            }
        }
        if(strcmp(vty_user_info.vty_user[i].username, DEFAULT_USERNAME))
        {
            vty_out(vty, " username %s %s", vty_user_info.vty_user[i].username, VTY_NEWLINE);
        }        
    }
    
    vty_out(vty, "!%s", VTY_NEWLINE);
    return CMD_SUCCESS;
}
static char *cmd_vty_node_prompt(struct vty *vty)
{
	static char prompt[40];
	sprintf(prompt, "%%s(User-config-vty%d)# ", vty->para.para_integer);
	return prompt;
}

/*****************************************************************
    Function:cmd_vty_user_init
    Description:init cmd of vty mode 
    Author:liaohongjun
    Date:2012/8/17
    Input:     
    Output:         
    Return:
=================================================
    Note:
*****************************************************************/
void cmd_vty_user_init(void)
{
    vty_node.prompt_func = cmd_vty_node_prompt;
    install_node(&vty_node, NULL);
    install_element(ENABLE_NODE, &access_vty_cmd);
    install_default(VTY_NODE);
    install_element(VTY_NODE, &vty_username_cmd);
    install_element(VTY_NODE, &no_vty_username_cmd);
    install_element(VTY_NODE, &vty_password_cmd);
    install_element(VTY_NODE, &no_vty_password_cmd);
    install_element(VTY_NODE, &enable_auth_mode_cmd);
    install_element(VTY_NODE, &exec_timeout_min_cmd);
    install_element(VTY_NODE, &exec_timeout_sec_cmd);
    install_element(VTY_NODE, &no_exec_timeout_cmd);    
#ifdef HAVE_IPV6
    install_element (VTY_NODE, &vty_ipv6_access_class_cmd);
    install_element (VTY_NODE, &no_vty_ipv6_access_class_cmd);
#endif /* HAVE_IPV6 */    
}

#ifdef  __cplusplus
}
#endif


