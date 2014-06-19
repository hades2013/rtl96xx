/*create by lizhe for alarm report*/
#include <stdio.h>
#include <string.h>

#include "oam_alarm.h"
#include "log.h"

#define NO_ERROR 0 
#define ERROR -1 
#define MODULE MOD_OAM

/* alarm.obj_instance.instance_id= (lport + 1) | (0x01 << 24);
                alarm.obj_instance.obj_type_leaf = 0x0001;*/
char g_report_enable = 1;
char objPort;
	
/*�ݶ�ÿ����ĸ澯�Ų�����32��*/
unsigned int g_alarm_eth_state[NUMBER_OF_PORTS] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
unsigned int g_alarm_onu_state = 0xffffffff;
unsigned int g_alarm_pon_state = 0xffffffff;
unsigned int g_alarm_card_state = 0xffffffff;
unsigned int g_alarm_pots_state = 0xffffffff;
unsigned int g_alarm_e1_state = 0xffffffff;

/*���ÿ��alarm�ĸ澯����*/
#if 1
static unsigned int g_alarm_eth_enable[NUMBER_OF_PORTS] = {0xffff, 0xffff, 0xffff, 0xffff};
static unsigned int g_alarm_onu_enable = 0;
static unsigned int g_alarm_pon_enable = 0;
static unsigned int g_alarm_card_enable = 0;
static unsigned int g_alarm_pots_enable = 0;
static unsigned int g_alarm_e1_enable= 0;
#endif

alarm_thr_t g_alarm_optical_thr[WARNING_TEMP_LOW]={{0xffffffff, 0x0},
													{0x0, 0xffffffff},
													{0xffffffff, 0x0},
													{0x0, 0xffffffff},
													{0xffffffff, 0x0},
													{0x0, 0xffffffff},
													{0xffffffff, 0x0},
													{0x0, 0xffffffff},
													{0xffffffff, 0x0},
													{0x0, 0xffffffff},
													{0xffffffff, 0x0},
													{0x0, 0xffffffff},
													{0xffffffff, 0x0},
													{0x0, 0xffffffff},
													{0xffffffff, 0x0},
													{0x0, 0xffffffff},
													{0xffffffff, 0x0},
													{0x0, 0xffffffff},
													{0xffffffff, 0x0},
													{0x0, 0xffffffff}};

/*�ⲿ��ȡ�澯״̬����*/
char g_alarm_ups_state = 0;

/*pippo add for dry alarm report at 2009-11-18 */
/*    �߼�"1"�������ɸ澯��ƽѡ��ȷ����Ain1��   */
#define GPIO9      9   /*  ����������1         */
#define GPIO11     11  /*  ����������2         */
#define DRY1_ALARMID 0x8001
#define DRY2_ALARMID 0x8002
char old_dry1_state=0;
char old_dry2_state=0;
/*end add*/

#if 0
#define THRESHOLD_HIGH_DEFAULT_VALUE 0xffffffff
#define THRESHOLD_LOW_DEFAULT_VALUE 0x0

optical_param_t optical_value;
/*��ֵ*/
optical_param_t opticalAlarmHighThreshold={
											.tmp = 0xffff,
											.vol = 0xffff,
											.cur = 0xffff,
											.input = 0xffff,
											.output = 0xffff
										};
optical_param_t opticalALarmLowThreshold = {0};

optical_param_t opticalWarningHighThreshold ={
											.tmp = 0xffff,
											.vol = 0xffff,
											.cur = 0xffff,
											.input = 0xffff,
											.output = 0xffff
											};

optical_param_t opticalWarningLowThreshold = {0};
#endif

#if 0
#define WAIT_CTC_TIME_DELAY 3
#define COUTER_NO_USE 0

static init_after_ctc_start_up()
{

}
#endif

static void dump_alarm_packet( unsigned char *pkt, int len )
{
	#if 1
	fprintf(stderr, "\n--------------------------------\r\n");

	int i = 0;
	fprintf(stderr, "pkt_len = %d\n", len);
	for ( i = 0; i < len; i++ )
	{
		if (i % 16 == 0 && i != 0)
		{
			fprintf(stderr, "\n");
		}
		
		fprintf(stderr, "%02X ", pkt[i]&0x000000ff);
	}

	fprintf(stderr, "\n--------------------------------\r\n");

	#endif
}



/*********************get alarm NO.**********************/
#if 0
unsigned int get_port_state_alarm_num(char port_state)
{
	if(port_state == 1)
		return ALARM_PORT_UP;
	else
		return ALARM_PORT_DOWN;
}

unsigned int get_ldp_alarm_num(char ldp_state)
{
	if(ldp_state == 1)
		return ALARM_LOOPBACK;
	else
		return ALARM_LOOPBACK_COMEBACK;
}

unsigned int get_ups_alarm_num(char ups_state)
{
	switch(ups_state)
	{
		case ALARM_UPS_STATE_ON:
			return ALARM_UPS_ON;
			break;
		case ALARM_UPS_STATE_POWER_LOSING:
			return ALARM_UPS_POWER;
			break;
		case ALARM_UPS_STATE_VOLT_LOW:
			return ALARM_UPS_VOLT_LOW;
			break;
		case ALARM_UPS_STATE_OFF:
			return ALARM_UPS_MISSING;
			break;
		default:
			return NO_ALARM_NUM;
	}

	
}

static alarm_num_t get_alarm_num_table[] =
{
	get_port_state_alarm_num,
	get_ldp_alarm_num,
};

static alarm_num_t get_onu_alarm_num_table[] =
{
	get_ups_alarm_num,
	NULL,/*ups ռ����bit*/
};

unsigned int get_alarm_num(char type, char state)
{
	get_alarm_num_table[type](state);
}

unsigned int get_onu_alarm_num(char type,char state)
{
	get_onu_alarm_num_table[type](state);
}
#endif

/*********************************************************************************
 ��������: oam_alarm
 ����˵��: �澯���ش�ʱ���ϱ�����
 �������:  alarm_num: �ϱ�alarm ID
 			port: CTC port��
 �������: ��
 			
 �� �� ֵ: 0 �ɹ�
 ��ʷ��¼:          
          .����: ����
         	 ����: 2009-5-15
          	����: 
          	����:2009-7-9
 *********************************************************************************/ 
int oam_alarm(alarm_report_t *info)
{	
	int i;
	int nRet = 0;
	//CTC_STACK_ALARM_INFO_t alarm;

	//TRACE_ALARM_ARG(info->alarmState);
	//TRACE_ALARM_ARG(info->objectType);
	//TRACE_ALARM_ARG(info->alarmID);

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"report alarm info (state %d type %d id %d).\n", info->alarmState, info->objectType, info->alarmID);
	#if 0
	/*��CTC2.1����*/
	char tx_frame[ALARM_INFO_HEAD + ALARM_INFO_MAX_WIDTH + 2] = {0};

	SET_BYTE_INIT();
	/*0x111111֮ǰ����lengthδ���*/
	SET_OUI(tx_frame,0x111111);
	
	
	SET_OBJ_TYPE(tx_frame, info->objectType);
	SET_INSTANCE_NUM(tx_frame, info->instanceNum);
	
	SET_ALARM_ID(tx_frame, info->alarmID);
	SET_TIMESTAMP(tx_frame, 0x00);
	SET_ALARM_STATE(tx_frame, info->alarmState);
	SET_ALARM_INFO(tx_frame, info->alarmInfo, info->length);

	dump_alarm_packet(tx_frame, ALARM_INFO_HEAD + info->length);
	
	nRet = PASONU_OAM_transmit_vend_ext((INT32U *)tx_frame, (INT16U)(ALARM_INFO_HEAD + info->length));
	
	if (nRet != NO_ERROR)
	{
		#ifdef ALARM_DEG
		fprintf(stderr, "%s %d %d\r\n", __FUNCTION__, __LINE__, nRet);
		#endif
	}
	#endif

	#if 0
	alarm.alarm_id = info->alarmID;
	alarm.alarm_state =! info->alarmState;
	alarm.obj_instance.obj_type_leaf = info->objectType;
	alarm.obj_instance.instance_id = info->instanceNum;
	memcpy((char *)&alarm.value, info->alarmInfo, sizeof(alarm.value));
	alarm.value=alarm.value;
	alarm.time_stamp = 0;
	
	nRet = CTC_STACK_alarm_generate(&alarm);
	#endif

	//info->alarmState = !info->alarmState;
	nRet = eopl_ctc_alarm_report(info);
	if(nRet != NO_ERROR)
	{
		printf("Generate alarm failed.\n");
	}
	return nRet;
	
}

#if 0
int alarm_loop_handle(INT8U lport, RSTP_loop_state_t loop_state)
{
	int nRet = 0;
	alarm_report_t info;

	info.alarmID = ALARM_ETHPORT_LDP;
	info.length = 4;
	memset(info.alarmInfo, 0, ALARM_INFO_MAX_WIDTH);
	//info.instanceNum= lport + 1;
	info.objectType = CTC_OBJ_PORT;	
	info.instanceNum = (lport +1)|ETH_TYPE << 24;
		
	switch(loop_state)
	{
		case LDP_LOOP_DETECTED:
			//printf("LDP_LOOP_DETECTED %d\n",lport);
			alarm_ldp[lport] = 1;
			break;
			
		case RSTP_LOOP_DETECTED:
			//printf("RSTP_LOOP_DETECTED %d\n",lport);
			if(get_eth_alarm_enable(lport, ALARM_ETHPORT_LDP & 0xff))
			{
				info.alarmState = 0;
				oam_alarm(&info);
			}
			break;
			
		case LDP_LOOP_REPAIRED:
			//printf("LDP_LOOP_REPAIRED %d\n",lport);
			alarm_ldp[lport] = 0;
			break;
			
		case RSTP_LOOP_REPAIRED:
			//printf("RSTP_LOOP_REPAIRED %d\n",lport);
			if(get_eth_alarm_enable(lport, ALARM_ETHPORT_LDP & 0xff))
			{
				info.alarmState = 1;
				oam_alarm(&info);
			}
			break;
		default:
			;
	}

	return nRet;
}
#endif

#if 0
void init_alarm_state()
{
	int i;
	
	for(i=0; i<NUMBER_OF_PORTS; i++)
	{
		clear_alarm_state(i, ALARM_TYPE_LINK,ALARM_TYPE_LINK_MASK);
		clear_alarm_state(i, ALARM_TYPE_LDP,ALARM_TYPE_LDP_MASK);
	}

	config_onu_alarm_state(ALARM_TYPE_UPS, ALARM_UPS_ON, ALARM_TYPE_UPS_MASK);

}
#endif

void eth_judge_and_alarm(alarm_report_t *info)
{
	char old_state;
	char type = 0xff & info->alarmID;
	int port = (info->instanceNum & 0xff) - 1;
	
	old_state = get_eth_alarm_state(port, type-1);
	if( info->alarmState != old_state)
	{
		oam_alarm(info);
		/*����ϱ�����λ�ϱ�״̬*/
		config_eth_alarm_state(port, type-1, info->alarmState); 
	}

}

void onu_judge_and_alarm(alarm_report_t *info)
{
	char type = 0xff & info->alarmID;
	char old_state;
	
	old_state = get_onu_alarm_state(type);

	if( info->alarmState != old_state)
	{
		oam_alarm(info);
		/*����ϱ�����λ�ϱ�״̬*/
		config_onu_alarm_state(type, info->alarmState); 

	}	
}

void pon_judge_and_alarm(alarm_report_t *info)
{
	char type = 0xff & info->alarmID;
	char old_state;

	old_state = get_pon_alarm_state(type);
	//printf("after get_pon_alarm_state:");
	//printf("0x%X\n", g_alarm_pon_state);

	if( info->alarmState != old_state)
	{

		oam_alarm(info);
		/*����ϱ�����λ�ϱ�״̬*/
		config_pon_alarm_state(type, info->alarmState); 
		//printf("after config_pon_alarm_state:");
		//printf("0x%X\n", g_alarm_pon_state);
	}	
}

char get_alarm_switch_state()
{
	#if 0
	FILE *fp;
	
	fp = fopen(ALARM_SWITCH_PATCH, "r");
	if(fp != NULL)
	{
		fscanf(fp, "enable=%hhd\n", &g_report_enable);
		fclose(fp);
	}
	#endif
	
	return g_report_enable;
}

void link_state_and_loop_alarm(int port, alarm_report_t *alarm_info)
{
	static char count = 0;
	unsigned int link_status = 0;
	int i, nRet,autonegState, adminState = 0;

   
	for(i = ALARM_ETHPORT_AUTONEG; i <= ALARM_ETHPORT_LDP; i++){
       // printf("g_alarm_eth_enable[0]=%x\n",g_alarm_eth_enable[0]);
		if (!get_eth_alarm_enable(port, i -ALARM_ETHPORT_AUTONEG))
			continue;
		alarm_info->alarmID = i;
		alarm_info->length = 0;
		alarm_info->alarmState = 1;

      
		switch(i){
			case ALARM_ETHPORT_AUTONEG:

               odmPortLinkStateGet(port + 1 , &link_status);

               if(1==link_status)
               {
				dalPhyAutonegAdminStateGet(port+1, &adminState);
				if (2 == adminState){
					nRet = dalPhySpecificAutonegRead(port+1, &autonegState);
					if (0 == nRet){
						alarm_info->alarmState = autonegState;
						eth_judge_and_alarm(alarm_info);
					}
				  }
                }
				break;
			case ALARM_ETHPORT_FAILURE:
				break;
			case ALARM_ETHPORT_LOS:
				/*�Ի�ʱ�رն˿�״̬�澯������resolve��������link�᲻ͣup,down�澯*/
				nRet = odmPortLinkStateGet(port + 1 , &link_status);
				//printf("link_state_and_loop_alarm link %d rstp %d.\n", link_status, rstpwarningstate[port]);
				if(rstpwarningstate[port] == 0 && NO_ERROR == nRet){	
					alarm_info->alarmState = link_status;
					eth_judge_and_alarm(alarm_info);
				}
				break;
			case ALARM_ETHPORT_LDP:
				/*��Ϊ�Ի��ظ��澯�������´�resolve֮ǰ���ж�*/
				if(count == LDP_RESOLVE_TIME -1){
					alarm_info->alarmState = (char)(!rstpwarningstate[port]);
					eth_judge_and_alarm(alarm_info);
				}
				break;
			default:
				break;
		}
	}

	/*pmc�ṩ�Ի����ܻ�Ѷ˿�down�������ֶ��ͷ�*/
	/*��Ϊ�Ի�״̬����resolve*/
//	if(get_eth_alarm_state(port, ALARM_ETHPORT_LDP) == 0)
	{
		count ++;
		count %= LDP_RESOLVE_TIME;
		//if(count == 0)
		//{
		//	RSTP_LDP_resolve_loop(port);
			//printf("port%d\n",port);
		//}
	}

}

void ups_alarm(alarm_report_t *alarm_info)
{
	char cur_state = get_alarm_ups_cur_state();
	char cur_volt_low_state = 0;
	char cur_power_losing_state = 0;
	char cur_off_state = 0;

	alarm_info->length = 4;
	//memset(alarm_info->alarmInfo, 0, ALARM_INFO_MAX_WIDTH);
	switch(cur_state)
	{
		case ALARM_UPS_STATE_ON:
			cur_off_state = 0;
			cur_power_losing_state = 0;
			cur_volt_low_state = 0;
			break;
		case ALARM_UPS_STATE_POWER_LOSING:
			cur_off_state = 0;
			cur_power_losing_state = 1;
			cur_volt_low_state = 0;
			break;
		case ALARM_UPS_STATE_VOLT_LOW:
			cur_off_state = 0;
			cur_power_losing_state = 1;
			cur_volt_low_state = 1;
			break;
		case ALARM_UPS_STATE_OFF:
			cur_off_state = 1;
			cur_power_losing_state = 0;
			cur_volt_low_state = 0;
			break;
		default:
			return;
		
	}

	if(get_onu_alarm_enable(ALARM_UPS_MISSING & 0xff))
	{
		alarm_info->alarmID = ALARM_UPS_MISSING;
		alarm_info->alarmState = !cur_off_state;
		onu_judge_and_alarm(alarm_info);
	}

	if(get_onu_alarm_enable(ALARM_UPS_POWER & 0xff))
	{
		alarm_info->alarmID = ALARM_UPS_POWER;
		alarm_info->alarmState = !cur_power_losing_state;
		onu_judge_and_alarm(alarm_info);
	}

	if(get_onu_alarm_enable(ALARM_UPS_VOLT_LOW & 0xff))
	{
		alarm_info->alarmID = ALARM_UPS_VOLT_LOW;
		alarm_info->alarmState = !cur_volt_low_state;
		onu_judge_and_alarm(alarm_info);
	}
	
}

/*pippo add for dry alarm report at 2009-11-18 */
void dry_judge_and_alarm(alarm_report_t *info)
{
	int i;
	int nRet = 0;
	/*��CTC2.1����*/
	char tx_frame[ALARM_INFO_HEAD + ALARM_INFO_MAX_WIDTH + 2] = {0};

	SET_BYTE_INIT();
	/*0x111111֮ǰ����lengthδ���*/
	SET_OUI(tx_frame,0x111111);
	
	
	SET_OBJ_TYPE(tx_frame, info->objectType);
	SET_INSTANCE_NUM(tx_frame, info->instanceNum);
	
	SET_ALARM_ID(tx_frame, info->alarmID);
	SET_TIMESTAMP(tx_frame, 0x00);
	SET_ALARM_STATE(tx_frame, info->alarmState);
	SET_ALARM_INFO(tx_frame, info->alarmInfo, ALARM_INFO_MAX_WIDTH);
	
	//printf("alarm the id is 0x%x\n",info->alarmID);
	//dump_alarm_packet(tx_frame, sizeof(tx_frame));
	
	//nRet = PASONU_OAM_transmit_vend_ext((INT32U *)tx_frame, sizeof(tx_frame));
	
	if (nRet != NO_ERROR)
	{
		#ifdef ALARM_DEG
		fprintf(stderr, "%s %d %d\r\n", __FUNCTION__, __LINE__, nRet);
		#endif
	}		
	
	
}

/*pippo add for dry alarm report at 2009-11-18 */
#ifdef DRY_ALARM
void	dry1_alarm(alarm_report_t *alarm_info)
{	
	INT8U 	nLevel = 0;
	INT32U level=0;
	INT32U monitor=0;
	int i=0;
	level=alarm_report_dry_alarm.level1;
	monitor = alarm_report_dry_alarm.monitor1 & 0xffffffff;
	alarm_info->alarmID =DRY1_ALARMID;
	memcpy(alarm_info->alarmInfo, (char *)&monitor, ALARM_INFO_MAX_WIDTH);	
	if (PASONU_GPIO_get(GPIO9, &nLevel)!=NO_ERROR)
	{	
		printf("get the gpio9 value failure!!!\n");
		return -1;
	}
	
#if 0
        nLevel = level; /*pippo for debug add at 2009-11-18*/
#endif	

	if(nLevel!=old_dry1_state)
	{
		if(level)
		{
			alarm_info->alarmState =nLevel;
		}
		else
		{
			alarm_info->alarmState =!nLevel;
		}

		old_dry1_state = nLevel;
		
		dry_judge_and_alarm(alarm_info);
	}
	
}

/*pippo add for dry alarm report at 2009-11-18 */
void	dry2_alarm(alarm_report_t *alarm_info)
{
	INT8U 	nLevel = 0;
	INT32U level=0;
	INT32U monitor=0;
	int i=0;
	level=alarm_report_dry_alarm.level2;
	monitor = alarm_report_dry_alarm.monitor2 & 0xffffffff;
	alarm_info->alarmID =DRY2_ALARMID;
	memcpy(alarm_info->alarmInfo, (char *)&monitor, ALARM_INFO_MAX_WIDTH);

	if (PASONU_GPIO_get(GPIO11, &nLevel)!=NO_ERROR)
	{
		printf("get the gpio11 value failure!!!\n");
		return -1;
	}

	if(nLevel!=old_dry2_state)
	{
		if(level)
		{
			alarm_info->alarmState =nLevel;
		}
		else
		{
			alarm_info->alarmState =!nLevel;
		}
		old_dry2_state = nLevel;
		dry_judge_and_alarm(alarm_info);
	}
}
#endif

void optical_alarm(alarm_report_t *alarm_info)
{
	
	int alarm_id;
	//optical_param_t optical_param;
	int nRet = 0;
	char cur_state = 1;
	unsigned short int optical_param_value;
	unsigned int local_value = 0;
	
	for(alarm_id = ALARM_RX_POWER_HIGH; alarm_id<WARNING_TEMP_LOW + 1; alarm_id++)
	{
		if(!get_pon_alarm_enable(alarm_id & 0xff))
			continue;

		switch(alarm_id)
		{
			case ALARM_TX_POWER_HIGH:
			case ALARM_TX_POWER_LOW:
			case WARNING_TX_POWER_HIGH:
			case WARNING_TX_POWER_LOW:
			//	optical_param_value = optical_param.output;
				stub_get_onu_optical_transiver_tx_power(&optical_param_value);
				break;
			case ALARM_RX_POWER_HIGH:
			case ALARM_RX_POWER_LOW:
			case WARNING_RX_POWER_HIGH:
			case WARNING_RX_POWER_LOW:
				//optical_param_value = optical_param.input;
				stub_get_onu_optical_transiver_rx_power(&optical_param_value);
				break;
			case ALARM_TX_BIAS_HIGH:
			case ALARM_TX_BIAS_LOW:
			case WARNING_TX_BIAS_HIGH:
			case WARNING_TX_BIAS_LOW:
				//optical_param_value = optical_param.cur;				
				stub_get_onu_optical_transiver_tx_bias_current(&optical_param_value);
				break;
			case ALARM_VCC_HIGH:
			case ALARM_VCC_LOW:
			case WARNING_VCC_HIGH:
			case WARNING_VCC_LOW:
				//optical_param_value = optical_param.vol;
				stub_get_onu_optical_transiver_supply_vcc(&optical_param_value);
				break;
			case ALARM_TEMP_HIGH:
			case ALARM_TEMP_LOW:
			case WARNING_TEMP_HIGH:
			case WARNING_TEMP_LOW:
				stub_get_onu_optical_transiver_temperature(&optical_param_value);
				//optical_param_value = optical_param.tmp;
				break;	
			default:
				return;
		}

		local_value = optical_param_value;
		
		//printf("alarm id: %d, value: %d, setAlarmThreshold: %lu, clearAlarmThreshold: %lu\n", 
			//alarm_id, local_value,g_alarm_optical_thr[alarm_id - ALARM_RX_POWER_HIGH].setAlarmThreshold
			//, g_alarm_optical_thr[alarm_id - ALARM_RX_POWER_HIGH].clearAlarmThreshold);

		/*�������߸澯*/
		if(alarm_id%2 == 1)
		{
			if(local_value >= g_alarm_optical_thr[alarm_id - ALARM_RX_POWER_HIGH].setAlarmThreshold)
			{
				cur_state = 0;
			}
			else if(local_value < g_alarm_optical_thr[alarm_id - ALARM_RX_POWER_HIGH].clearAlarmThreshold)
			{
				cur_state = 1;
			}
			else
			{
				continue;
			}
		}
		/*���͸澯*/
		else
		{
			if(local_value <= g_alarm_optical_thr[alarm_id - ALARM_RX_POWER_HIGH].setAlarmThreshold)
			{
				cur_state = 0;
			}
			else if(local_value > g_alarm_optical_thr[alarm_id - ALARM_RX_POWER_HIGH].clearAlarmThreshold)
			{
				cur_state = 1;
			}
			else
			{
				continue;
			}
	
		}
		//printf("g_alarm_onu_state 0x%04X\n", g_alarm_onu_state);
		//printf("0x%X\n", g_alarm_pon_state);
		//printf("g_alarm_card_state 0x%04X\n", g_alarm_card_state);
		//local_value = ntohs(optical_param_value);
		//local_value = htonl(local_value);
		
		alarm_info->alarmID = alarm_id;
		alarm_info->alarmState = cur_state;
		memcpy(alarm_info->alarmInfo, (char *)&local_value, ALARM_INFO_MAX_WIDTH);
		
		pon_judge_and_alarm(alarm_info);
			
	}
}

/*�˿ڸ澯*/
void port_alarm_report()
{
	int i;
	alarm_report_t alarm_info;
	alarm_info.objectType = CTC_OBJ_PORT;		
	for(i = 0; i < NUMBER_OF_PORTS; i++)
	{
		alarm_info.instanceNum = (i+1)|ETH_TYPE << 24;
		link_state_and_loop_alarm(i, &alarm_info);	
	}
}

/*ȫ�ָ澯*/
void onu_alarm_report()
{
	alarm_report_t alarm_info;
	alarm_info.objectType = CTC_OBJ_ONU;
	alarm_info.instanceNum = 0xffffffff;
	ups_alarm(&alarm_info);
}
/*pippo add for dry alarm report at 2009-11-18 */
#ifdef DRY_ALARM
void dry_alarm_report()
{
	alarm_report_t alarm_info_dry1;
	alarm_report_t alarm_info_dry2;
	memset(&alarm_info_dry1,0,sizeof(alarm_report_t));
	memset(&alarm_info_dry2,0,sizeof(alarm_report_t));	
	alarm_info_dry1.objectType = htons(CTC_OBJ_ONU);
	alarm_info_dry1.instanceNum = htonl(0xffffffff);
	alarm_info_dry2.objectType = htons(CTC_OBJ_ONU);
	alarm_info_dry2.instanceNum = htonl(0xffffffff);
	
	dry1_alarm(&alarm_info_dry1);
	dry2_alarm(&alarm_info_dry2);
	
}
#endif

void pon_alarm_report()
{
	alarm_report_t alarm_info;
	alarm_info.objectType = CTC_OBJ_PON_IF;
	alarm_info.instanceNum = 0x0000000;
	optical_alarm(&alarm_info);
}

//PAS_STATUS CTC_STACK_alarm_get_state(CTC_STACK_object_instance_t *obj_instance, INT16U alarm_id, CTC_STACK_alarm_admin_config_t *enabled)
int CTC_STACK_alarm_get_state(INT16U alarm_id, int port_num, char *enabled)
{
	unsigned short obj_type = alarm_id & ALARM_OBJ_TYPE_MASK;
	unsigned short instance_type = alarm_id & ALARM_INSTANCE_TYPE_MASK;
	//unsigned int port_num = obj_instance->instance_id & 0xffff;
	char enable;

	

	TRACE_ALARM("FUNCTION:%s, LINE:%d\n",__FUNCTION__,__LINE__);

	switch(obj_type)
	{
		case ALARM_ONU_BASE:
			enable = get_onu_alarm_enable(instance_type);
			break;
		case ALARM_PON_IF_BASE:
			enable = get_pon_alarm_enable(instance_type);
			break;
		case ALARM_CARD_BASE:
			return ERROR;
		case ALARM_ETH_PORT_BASE:
			port_num--;
			enable = get_eth_alarm_enable(port_num, instance_type);
			break;
		case ALARM_POTS_BASE:
		case ALARM_E1_BASE:
		default:
			return ERROR;
	}

	//printf("instance_type:%d enable:%d  port_num:%d\n",instance_type, enable, port_num);
	TRACE_ALARM_ARG(g_alarm_pon_enable);
	*enabled = enable == 0 ? CTC_STACK_ALARM_CONFIG_DISABLED : CTC_STACK_ALARM_CONFIG_ENABLED;
	return NO_ERROR;
}

//PAS_STATUS CTC_STACK_alarm_set_state(CTC_STACK_object_instance_t *obj_instance, INT16U alarm_id, CTC_STACK_alarm_admin_config_t enabled)
int CTC_STACK_alarm_set_state(INT16U alarm_id, int port_num, char enabled)
{
	unsigned short obj_type = alarm_id & ALARM_OBJ_TYPE_MASK;
	unsigned short instance_type = alarm_id & ALARM_INSTANCE_TYPE_MASK;
	//unsigned int port_num = obj_instance->instance_id & 0xffff;
	char enable = (enabled == CTC_STACK_ALARM_CONFIG_DISABLED ? 0 : 1);
	
	//printf("instance_type:%d enable:%d  port_num:%d\n",instance_type, enable, port_num);
	
	TRACE_ALARM("FUNCTION:%s, LINE:%d\n",__FUNCTION__,__LINE__);

	switch(obj_type)
	{
		case ALARM_ONU_BASE:
			config_onu_alarm_enable(instance_type, enable);
			break;
		case ALARM_PON_IF_BASE:
			config_pon_alarm_enable(instance_type, enable);
			break;
		case ALARM_CARD_BASE:
			return ERROR;
		case ALARM_ETH_PORT_BASE:
			port_num--;
			config_eth_alarm_enable(port_num, instance_type-1, enable);
			break;
		case ALARM_POTS_BASE:
		case ALARM_E1_BASE:
		default:
			return ERROR;
	}
	TRACE_ALARM_ARG(g_alarm_pon_enable);

	return NO_ERROR;
}

int set_optical_thr(INT16U alarm_id, alarm_thr_t *thr)
{
	TRACE_ALARM("FUNCTION:%s, LINE:%d\n",__FUNCTION__,__LINE__);
	
	switch(alarm_id)
	{
		case ALARM_RX_POWER_HIGH:
		case ALARM_RX_POWER_LOW:
		case ALARM_TX_POWER_HIGH:
		case ALARM_TX_POWER_LOW:
		case ALARM_TX_BIAS_HIGH:
		case ALARM_TX_BIAS_LOW:
		case ALARM_VCC_HIGH:
		case ALARM_VCC_LOW:
		case ALARM_TEMP_HIGH:
		case ALARM_TEMP_LOW:
		case WARNING_RX_POWER_HIGH:
		case WARNING_RX_POWER_LOW:
		case WARNING_TX_POWER_HIGH:
		case WARNING_TX_POWER_LOW:
		case WARNING_TX_BIAS_HIGH:
		case WARNING_TX_BIAS_LOW:
		case WARNING_VCC_HIGH:
		case WARNING_VCC_LOW:
		case WARNING_TEMP_HIGH:
		case WARNING_TEMP_LOW:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "set optical thr: alarm_id %d thr(%d->%d).\n", alarm_id, thr->clearAlarmThreshold, thr->setAlarmThreshold);
			memcpy(&g_alarm_optical_thr[alarm_id-ALARM_RX_POWER_HIGH], thr, sizeof(alarm_thr_t));
			break;			
		default:
			return ERROR;
	}

	return NO_ERROR;
}

int get_optical_thr(INT16U alarm_id, alarm_thr_t *thr)
{

	TRACE_ALARM_ARG(alarm_id);

	switch(alarm_id)
	{
		case ALARM_RX_POWER_HIGH:
		case ALARM_RX_POWER_LOW:
		case ALARM_TX_POWER_HIGH:
		case ALARM_TX_POWER_LOW:
		case ALARM_TX_BIAS_HIGH:
		case ALARM_TX_BIAS_LOW:
		case ALARM_VCC_HIGH:
		case ALARM_VCC_LOW:
		case ALARM_TEMP_HIGH:
		case ALARM_TEMP_LOW:
		case WARNING_RX_POWER_HIGH:
		case WARNING_RX_POWER_LOW:
		case WARNING_TX_POWER_HIGH:
		case WARNING_TX_POWER_LOW:
		case WARNING_TX_BIAS_HIGH:
		case WARNING_TX_BIAS_LOW:
		case WARNING_VCC_HIGH:
		case WARNING_VCC_LOW:
		case WARNING_TEMP_HIGH:
		case WARNING_TEMP_LOW:
			memcpy(thr, &g_alarm_optical_thr[alarm_id-ALARM_RX_POWER_HIGH], sizeof(alarm_thr_t));
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "get optical thr: alarm_id %d thr(%d->%d).\n", alarm_id, thr->clearAlarmThreshold, thr->setAlarmThreshold);
			break;			
		default:
			return ERROR;
	}

	return NO_ERROR;
}

//PAS_STATUS CTC_STACK_alarm_get_thr(CTC_STACK_object_instance_t *obj_instance, INT16U alarm_id, INT32U *rising_thr, INT32U *falling_thr)
int CTC_STACK_alarm_get_thr(INT16U alarm_id, int port_num, INT32U *rising_thr, INT32U *falling_thr)
{
	unsigned short obj_type = alarm_id & ALARM_OBJ_TYPE_MASK;
	unsigned short instance_type = alarm_id & ALARM_INSTANCE_TYPE_MASK;
	alarm_thr_t thr;
	int nRet = 0;

	
	TRACE_ALARM_ARG(obj_type);
	
	switch(obj_type)
	{
		case ALARM_ONU_BASE:
			return ERROR;
		case ALARM_PON_IF_BASE:
			nRet = get_optical_thr(alarm_id, &thr);
			break;
		case ALARM_CARD_BASE:
		case ALARM_ETH_PORT_BASE:
		case ALARM_POTS_BASE:
		case ALARM_E1_BASE:
		default:
			return ERROR;
	}

	*rising_thr = thr.setAlarmThreshold;
	*falling_thr = thr.clearAlarmThreshold;
	
	//printf("instance_type:%d thr.setAlarmThreshold:%04x  thr.clearAlarmThreshold:%04x port_num:%d\n",instance_type,thr.setAlarmThreshold, thr.clearAlarmThreshold,port_num);

	TRACE_ALARM("FUNCTION:%s, LINE:%d\n",__FUNCTION__,__LINE__);
	TRACE_ALARM_ARG(nRet);
	
	return nRet;

}

//PAS_STATUS CTC_STACK_alarm_set_thr(CTC_STACK_object_instance_t *obj_instance, INT16U alarm_id, INT32U rising_thr, INT32U falling_thr)
int CTC_STACK_alarm_set_thr(INT16U alarm_id, int port_num, INT32U rising_thr, INT32U falling_thr)
{
	unsigned short obj_type = alarm_id & ALARM_OBJ_TYPE_MASK;
	unsigned short instance_type = alarm_id & ALARM_INSTANCE_TYPE_MASK;
	alarm_thr_t thr;
	thr.setAlarmThreshold = rising_thr;
	thr.clearAlarmThreshold = falling_thr;
	int nRet = -1;

	//printf("instance_type:%d thr.setAlarmThreshold:%04x  thr.clearAlarmThreshold:%04x port_num:%d\n",instance_type,thr.setAlarmThreshold, thr.clearAlarmThreshold,port_num);
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "alarm_id %d: (%d->%d).\n", alarm_id, falling_thr, rising_thr);

	TRACE_ALARM("FUNCTION:%s, LINE:%d\n",__FUNCTION__,__LINE__);
	
	switch(obj_type)
	{
		case ALARM_ONU_BASE:
			return ERROR;
		case ALARM_PON_IF_BASE:
			nRet = set_optical_thr(alarm_id, &thr);
			break;
		case ALARM_CARD_BASE:
		case ALARM_ETH_PORT_BASE:
		case ALARM_POTS_BASE:
		case ALARM_E1_BASE:
		default:
			return ERROR;
	}

	TRACE_ALARM_ARG(nRet);
	return nRet;
}

void init_alarm_hooks()
{
   // CTC_STACK_g_hooks_alarm.alarm_admin_state_get_hook= CTC_STACK_alarm_get_state;
//    CTC_STACK_g_hooks_alarm.alarm_admin_state_set_hook = CTC_STACK_alarm_set_state;
//    CTC_STACK_g_hooks_alarm.alarm_threshold_get_hook = CTC_STACK_alarm_get_thr;
//    CTC_STACK_g_hooks_alarm.alarm_threshold_set_hook = CTC_STACK_alarm_set_thr;
}

/*�澯����߳�*/
void alarm_pthread()
{
	//char timeCounter = 1;
		
//	gv_rstp_hooks.send_loop_event = alarm_loop_handle;
	
//	RSTP_LDP_set_mode((BOOLEAN)1);

	//init_alarm_state();

	//g_alarm_pon_state = 0xffffffff;

	while(1)
	{
		sleep(QUERY_TIME);
		
		if(!get_alarm_switch_state())
			continue;

		port_alarm_report();
		
		/*onu_alarm_report();*/

		pon_alarm_report();

		#ifdef DRY_ALARM
		dry_alarm_report();
		#endif
		/*��澯�޹أ����Ҽ������*/
		#if 0
		if(timeCounter == COUTER_NO_USE)
			continue;

		if(timeCounter * QUERY_TIME == WAIT_CTC_TIME_DELAY)
		{
			timeCounter = COUTER_NO_USE;
			//init_after_ctc_start_up();
			break;
		}

		timeCounter++;
		#endif
	}
}

