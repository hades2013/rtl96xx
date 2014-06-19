
int _rtk_rg_arpAddByHwCallBack(rtk_rg_arpInfo_t* arpInfo);
int _rtk_rg_arpDelByHwCallBack(rtk_rg_arpInfo_t* arpInfo);
int _rtk_rg_macAddByHwCallBack(rtk_rg_macEntry_t* macInfo);
int _rtk_rg_macDelByHwCallBack(rtk_rg_macEntry_t* macInfo);
int _rtk_rg_naptAddByHwCallBack(rtk_rg_naptInfo_t* naptInfo);
int _rtk_rg_naptDelByHwCallBack(rtk_rg_naptInfo_t* naptInfo);
int _rtk_rg_routingAddByHwCallBack(rtk_rg_ipv4RoutingEntry_t* routingInfo);
int _rtk_rg_routingDelByHwCallBack(rtk_rg_ipv4RoutingEntry_t* routingInfo);
int _rtk_rg_bindingAddByHwCallBack(rtk_rg_bindingEntry_t* bindingInfo);
int _rtk_rg_bindingDelByHwCallBack(rtk_rg_bindingEntry_t* bindingInfo);

int _rtk_rg_initParameterSetByHwCallBack(void);
int _rtk_rg_interfaceAddByHwCallBack(rtk_rg_intfInfo_t* intfInfo,int* intfIdx);
int _rtk_rg_interfaceDelByHwCallBack(rtk_rg_intfInfo_t* intfInfo, int* intfIdx);
int _rtk_rg_pppoeBeforeDiagByHwCallBack(rtk_rg_pppoeClientInfoBeforeDial_t* before_diag, int* intfIdx);
int _rtk_rg_neighborAddByHwCallBack(rtk_rg_neighborInfo_t* neighborInfo);
int _rtk_rg_neighborDelByHwCallBack(rtk_rg_neighborInfo_t* neighborInfo);
int _rtk_rg_v6RoutingAddByHwCallBack(rtk_rg_ipv6RoutingEntry_t* cb_routv6Et);
int _rtk_rg_v6RoutingDelByHwCallBack(rtk_rg_ipv6RoutingEntry_t* cb_routv6Et);
int _rtk_rg_dhcpRequestByHwCallBack(int* intfIdx);




#define MAX_FILE_SIZE 1024
#define MAX_FILE_LINE 10
#define MAX_CMD_ARGV_SIZE 20
#define CMD_BUFF_SIZE 256


#define user_cmd( comment ,arg...) \
do {\
		char *cmd; \
		char *token; \
		char cmd_buff[CMD_BUFF_SIZE]; \
		char *argv[MAX_CMD_ARGV_SIZE], *envp[3];\
		int l, idx, retval; \
		idx = 0; \
		envp[idx++] = "HOME=/"; \
		envp[idx++] = "PATH=/sbin:/usr/sbin:/bin:/usr/bin"; \
		envp[idx] = NULL; \
		idx = 0; \
		sprintf( cmd_buff, comment,## arg); \
		cmd = cmd_buff; \
		l = strlen(cmd); \
		while (strsep(&cmd, " ")); \
		for (idx=0, token=cmd_buff; token<(cmd_buff+l);idx++) { argv[idx] = token; token=token+strlen(token)+1;} \
		argv[idx] = NULL; \
		if(rg_kernel.debug_level&RTK_RG_DEBUG_LEVEL_CALLBACK){\
			rtlglue_printf("[rg callback]CMD:");\
			for(idx=0;argv[idx]!=NULL;idx++){rtlglue_printf("\033[1;33m%s \033[0m",argv[idx]);}\
			if((retval=call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC))==0){rtlglue_printf("\n");} \
			else {rtlglue_printf("\033[1;35m [Exec Fialed]\033[0m,ret=%d @%s,line:%d\n",retval,__func__,__LINE__);}\
		}else{\
			call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);\
		}\
} while(0);


/* used for command which need to pipe to file, ex: >  >>*/
#define user_pipe_cmd( comment ,arg...) \
do{ \
	char cmd_buff[CMD_BUFF_SIZE]; \
	char *argv[MAX_CMD_ARGV_SIZE], *envp[3];\
	int idx, retval; \
	idx = 0; \
	envp[idx++] = "HOME=/"; \
	envp[idx++] = "PATH=/sbin:/usr/sbin:/bin:/usr/bin"; \
	envp[idx] = NULL; \
	idx = 0;\
	sprintf( cmd_buff, comment,## arg); \
	argv[idx++] = "/bin/ash";\
	argv[idx++] = "-c";\
	argv[idx++] = cmd_buff;	\
	argv[idx] = NULL; \
	if(rg_kernel.debug_level&RTK_RG_DEBUG_LEVEL_CALLBACK){\
		rtlglue_printf("[rg callback]CMD:");\
		for(idx=0;argv[idx]!=NULL;idx++){rtlglue_printf("\033[1;33m%s \033[0m",argv[idx]);}\
		if((retval=call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC))==0){rtlglue_printf("\n");} \
		else {rtlglue_printf("\033[1;35m [Exec Fialed]\033[0m, ret=%d @%s,line:%d\n",retval,__func__,__LINE__);}\
	}else{\
		call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);\
	}\
}while(0);

