/*
 * syslog.h
 *
 *  Created on: 2013/09/06
 *      Author: root
 */
//#include <lw_type.h>

#ifndef SYSTEMLOG_H_
#define SYSTEMLOG_H_

typedef enum log_module_id{
	SYSLOG = 0,//"SYSLOG"
	STP ,	//STP
	AAA ,
	NTP ,
	WEB ,
	TELNET ,
	USER ,
	SYSTEM ,
	MVLAN ,
	LOOP ,
	QOS ,
	SNMP,
	MODULE_MAX
}log_module_t;


typedef enum log_msg_id{
	M_SYSLOG_STA =0,  //syslog
	M_SYSLOG_EXIT,
	M_STP_STA,		//stp
	M_STP_EXIT,
	M_AAA_STA,		//aaa
	M_AAA_EXIT,
	M_NTP_STA,		//ntp	
	M_NTP_EXIT,
	M_WEB_STA,		//web
	M_WEB_EXIT,
	M_TELNET_STA,	//telnet	
	M_TELNET_EXIT,
	M_USER_STA,		//user
	M_USER_EXIT,
	M_SYSTEM_STA,	//system
	M_SYSTEM_EXIT,
	M_MVLAN_STA,	//mvlan
	M_MVLAN_EXIT,
	M_LOOP_STA,		//loop
	M_LOOP_EXIT,
	M_QOS_STA,		//qos
	M_QOS_EXIT,
	M_SNMP_STA,		//snmp
	M_SNMP_EXIT,
	M_LOGIN,		//login
	M_LOGOUT,
	M_USERNAMER,     //add by leijinbao for 281
	M_REBOOT,		//reboot
	M_SAVECONF,		//save config
	M_RECONF,		// recover config
	M_IPCHANGE,		//change ip 
	M_LINKUP,		//states of GigabitEthernet  up or down
	M_LINKDOWN,
	M_LONGLIGHT,
	M_OPTICALLINK,
	STR_END
}log_msg_t;

//#define SRT_NMU (STR_END-M_SYSLOG_STA +MODULE_MAX) 

typedef enum log_lang_id{
	CN = 0,//china
	EN,	  //english
	LANG_MAX
}log_lang_t;

struct log_messg{
unsigned int id;
char msg_en[1024];
char msg_cn[1024];
}messg_t;

struct log_mod{
unsigned int id;
char mod_en[1024];
char mod_cn[1024];
}mod_t;

#define LOG_EMERG       0       /* system is unusable */
#define LOG_ALERT       1       /* action must be taken immediately */
#define LOG_CRIT        2       /* critical conditions */
#define LOG_ERR         3       /* error conditions */
#define LOG_WARNING     4       /* warning conditions */
#define LOG_NOTICE      5       /* normal but significant condition */
#define LOG_INFO        6       /* informational */
#define LOG_DEBUG       7       /* debug-level messages */

extern void USER_LOG(unsigned int lang_id,unsigned int module_id,unsigned int priority,unsigned int msg_id,unsigned char *left_fmt,unsigned char *right_fmt);
extern int getStrById_EN(unsigned int module_id, unsigned int mesg_id,char *module_str, char *msg_str, unsigned int module_size,unsigned int msg_size);
extern void Syslog_Init(void);
extern int LangId;

#endif /* SYSLOG_H_ */
