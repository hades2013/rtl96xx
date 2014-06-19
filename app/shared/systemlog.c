/*
 * syslog.c
 *
 *  Created on: 2013/09/06
 *      Author: root
 */
#include <stdio.h>
#include <stdlib.h>

#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

#include <shutils.h>

/* Linux specific headers */
#include <error.h>
#include <termios.h>
#include <sys/time.h>
#include <net/ethernet.h>
#include <dirent.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <ctype.h>
#include <net/if_arp.h>
#include <lw_config.h>
#include "build_time.h"
#include <time_zone.h>

/****һ��������Ӧһ�����ԣ�ÿ���ַ�����Ӧһ��ID���ɿ�����Ա�Լ�����*/
/*0-255 ��ʾģ��Ķ�������ʵ�֣�����256��ʼ���ɿ�����Ա�Լ���*/

struct log_messg log_module_msg[STR_END] = {{M_SYSLOG_STA,"syslogd starting" ,"syslogd 启动"},
									 {M_SYSLOG_EXIT,"syslogd exiting","syslogd 退出."},
									 {M_STP_STA,"the device started running STP now","设备开始运行STP"},
									 {M_STP_EXIT,"the device stopped running STP now","设备停止运行STP"},
									 {M_AAA_STA,"enter the AAA security Settings","进入AAA安全设置"},
									 {M_AAA_STA,"exit the AAA security Settings","退出AAA安全设置"},
									 {M_NTP_STA,"enter the NTP module Settings","进入系统时间模块设置"},
									 {M_NTP_EXIT,"exit the NTP module Settings","退出系统时间模块设置"},
									 {M_WEB_STA,"enter the WEB module","进入WEB设置模块"},
									 {M_WEB_EXIT,"exit the WEB module","退出WEB设置模块"},
									 {M_TELNET_STA,"telnet enable","telnet功能启动"},
									 {M_TELNET_EXIT,"telnet diabled","telnet功能关闭"},
									 {M_USER_STA,"enter the USER manage Settings","进入用户管理设置"},
									 {M_USER_EXIT,"exit the USER manage Settings","退出用户管理设置"},
									 {M_SYSTEM_STA,"system starting","系统启动"},
									 {M_SYSTEM_EXIT,"system exiting","退出系统"},
									 {M_MVLAN_STA,"the device started running MVLAN now","设备开始MLAN的运行"},
									 {M_MVLAN_EXIT,"the device stopped running MVLAN now","设备停止MLAN的运行"},									 
									 {M_LOOP_STA,"loop enable now","loop功能开启"},
									 {M_LOOP_EXIT,"loop diabled now","loop功能关闭"},
									 {M_QOS_STA,"enter the QOS module","进入QOS设置模块"},
									 {M_QOS_EXIT,"exit the QOS module","退出QOS设置模块"},
									 {M_SNMP_STA,"snmp enable now","snmp功能开启"},
									 {M_SNMP_EXIT,"snmp diabled now","snmp功能关闭"},
									 {M_LOGIN," user logged in from "," 用户 admin 登录,ip为"},
									 {M_LOGOUT," user logged out from "," 用户admin注销,ip为"},
									 {M_USERNAMER,"username or password is Invalid or error",""},
									 {M_REBOOT,"the device was restarted by user","该设备是由用户重新启动"},
									 {M_SAVECONF,"the device saved current configuration successfully","配设备保存当前配置成功"},
									 {M_RECONF,"the device restored factory configuration successfully","配设备恢复工厂配置成功"},
									 {M_IPCHANGE,"ip address has been changed into ",",IP地址已经更改为"},
									 {M_LINKUP," is UP","状态已经连接"},
									 {M_LINKDOWN," is DOWN","连接状态断开"},
									 {M_LONGLIGHT,"pon status detected always laser","pon状态检测长发光"},
									 {M_OPTICALLINK,"normal optical link negotiation","光链路协商正常"}
								   };

struct log_mod log_module[MODULE_MAX] = {{SYSLOG, "SYSLOG","系统日志"}, 
									 {STP, "STP" ,"STP"}, 
									 {AAA, "AAA" , "AAA" },
									 {NTP, "NTP" ,"NTP"  },
									 {WEB, "WEB" ,"WEB"  }, 
									 {TELNET, "TELNET" ,"TELNET"},
									 {USER, "USER" ,"USER"	}, 
									 {SYSTEM, "SYSTEM" ,"SYSTEM"},
									 {MVLAN, "MVLAN", "MVLAN"}, 
									 {LOOP, "LOOP" ,"LOOP"},
									 {QOS, "QOS" ,"QOS" }, 
									 {SNMP, "SNMP" ,"SNMP"}


};

int getStrById_EN(unsigned int module_id, unsigned int mesg_id,char *module_str, char *msg_str, unsigned int module_size,unsigned int msg_size)
{
	char *pStr = NULL;
	char *Str = NULL;
	int i = 0;
	int y = 0;

	if(module_id>MODULE_MAX)
	{
		return -1;
	}

	if(mesg_id>STR_END)
	{
		return -1;
	}
	if((!module_str)||(!msg_str))
	{
		return -1;
	} 
	//login_out(id,login_msg,sizeof(login_msg));
	for(i = SYSLOG;i<MODULE_MAX;i++)
	{
		if(log_module[i].id == module_id)
		{
			pStr = log_module[i].mod_en;
			safe_strncpy(module_str, pStr, module_size);
			break;
		}
	}
	for(y = M_SYSLOG_STA;y<STR_END;y++)
	{
		if(log_module_msg[y].id == mesg_id)
		{
			Str = log_module_msg[y].msg_en;
			safe_strncpy(msg_str, Str, msg_size);
			break;
		}
	}
}


int getStrById_CN(unsigned int module_id, unsigned int mesg_id,char *module_str, char *msg_str, unsigned int module_size,unsigned int msg_size)
{
	char *pStr = NULL;
	char *Str = NULL;
	int i = 0;
	int y = 0;

	if(module_id>MODULE_MAX)
	{
		return -1;
	}

	if(mesg_id>STR_END)
	{
		return -1;
	}
	if((!module_str)||(!msg_str))
	{
		return -1;
	} 
	//login_out(id,login_msg,sizeof(login_msg));
	for(i = SYSLOG;i<MODULE_MAX;i++)
	{
		if(log_module[i].id == module_id)
		{
			pStr = log_module[i].mod_cn;
			safe_strncpy(module_str, pStr, module_size);
			break;
		}
	}
	for(y = M_SYSLOG_STA;y<STR_END;y++)
	{
		if(log_module_msg[y].id == mesg_id)
		{
			Str = log_module_msg[y].msg_cn;
			safe_strncpy(msg_str, Str, msg_size);
			break;
		}
	}
}

#if 1
int (*MesFunc[LANG_MAX])(unsigned int,unsigned int, char *,char *,unsigned int,unsigned int);
void Syslog_Init(void)
{
	MesFunc[EN] = getStrById_EN;
	MesFunc[CN] = getStrById_CN;
	/*here add the Func_Init*/
}
#endif

int LangId = EN;

void USER_LOG(unsigned int lang_id,unsigned int module_id,unsigned int priority,unsigned int msg_id,unsigned char *left_fmt,unsigned char *right_fmt)
{
	char msg_str[1024] = {0};
	char module_str[64]={0};
	
	if(!right_fmt||!left_fmt)
	{
		return -1;
	} 
	if(NULL !=(*MesFunc[lang_id]))
	{
		(*MesFunc[lang_id])(module_id,msg_id, module_str,msg_str,sizeof(module_str),sizeof(msg_str));
		openlog(module_str,0,0);
		//(*MesFunc[lang_id])(msg_id, msg_str, sizeof(msg_str));
		syslog(priority, "%s:%s%s%s",module_str,left_fmt,msg_str,right_fmt);
		closelog();
	}
	else
	{
		printf("get module OR lang_id error!!\n");
		return -1;
	}
}
/*end by leijinbao 2013/9/2*/
