/*creat by lizhe for alarm report*/
#ifndef OAM_ALARM_H
#define OAM_ALARM_H

#define OPL_ALARM_ENABLE 0

#define ALARM_SWITCH_PATCH "/var/alarm.conf"

#define CTC_2_1
/*little endian*/
#define SET_BYTE_INIT() static char byte_counter;\
						static unsigned long long temp_value
#define SET_BYTE(tx_val, value, byte)  byte_counter = byte;\
									temp_value = value;\
									while(byte_counter){\
									(tx_val)[byte_counter -1] = (temp_value) & 0xff;\
									(temp_value) >>= 8;\
									byte_counter --;\
									}

#define SET_BYTE_2(tx_val, pValue, byte)  byte_counter = byte;\
									while(byte_counter){\
									(tx_val)[byte-byte_counter] = (pValue)[byte_counter-1];\
									byte_counter --;\
									}

/*按CTC2.1更改*/									
#ifdef CTC_2_1		
#define SET_OUI(tx_val,oui) SET_BYTE(tx_val,oui,3)
#define SET_OBJ_TYPE(tx_val,ObjectType) SET_BYTE((tx_val)+3,ObjectType,2)
#define SET_INSTANCE_NUM(tx_val,InstanceNum) SET_BYTE((tx_val)+5,InstanceNum,4)
#define SET_ALARM_ID(tx_val, AlarmID) SET_BYTE((tx_val)+9,AlarmID,2)
#define SET_TIMESTAMP(tx_val, TimeStamp) SET_BYTE((tx_val)+11,TimeStamp,2)
#define SET_ALARM_STATE(tx_val, AlarmState) SET_BYTE((tx_val)+13,AlarmState,1)
#define SET_ALARM_INFO(tx_val, AlarmInfo,x) SET_BYTE_2((tx_val)+14, AlarmInfo,x)
#else
#define SET_OUI(tx_val,oui) tx_val[0] = oui>>16 & 0xff;\
							tx_val[1] = oui>>8 & 0xff;\ 
							tx_val[2] =  oui & 0xff
#define SET_OPCODE(tx_val,opcode) tx_val[3]=opcode
#define SET_TLV(tx_val,branch,leaf)  tx_val[4]=branch;\
									tx_val[5]=leaf>>8 & 0xff;\
									tx_val[6]=leaf & 0xff
#define SET_PORT(tx_val,port) tx_val[7] = 0x01;\
							tx_val[8] = port
#define SET_BRANCH_LEAF(tx_val,branch,leaf) tx_val[9]=branch;\
											tx_val[10]=leaf>>8 & 0xff;\
											tx_val[11]=leaf & 0xff
#define SET_VALUE(tx_val,width,value) tx_val[12] = width;\
									tx_val[13] = value>>8 & 0xff;\ 
									tx_val [14] = value & 0xff
#endif
#define ALARM_INFO_HEAD 14
#define ALARM_INFO_MAX_WIDTH 4

typedef struct{
    //char port;
    unsigned short objectType;
    unsigned int instanceNum;
    unsigned int alarmID;
    char alarmState;
    char length;
    char alarmInfo[ALARM_INFO_MAX_WIDTH];
}alarm_report_t;


typedef struct
{
    unsigned int setAlarmThreshold;
    unsigned int clearAlarmThreshold;	
}alarm_thr_t;

#ifdef ONU_1PORT
#define NUMBER_OF_PORTS  1
#else
#define NUMBER_OF_PORTS	 4
#endif

typedef struct
{
	unsigned int  monitor1;/*监控对象*/
	unsigned int  level1;/*有效电平*/
	unsigned int  monitor2;/*监控对象*/
	unsigned int  level2;/*有效电平*/	
}alarm_report_dry_alarm_t;

extern alarm_report_dry_alarm_t alarm_report_dry_alarm;

//#define NO_ALARM_NUM 0

/*按CTC2.1更改*/
#define ALARM_ONU_BASE 0x0000
#define ALARM_PON_IF_BASE 0x0100
#define ALARM_CARD_BASE 0x0200
#define ALARM_ETH_PORT_BASE 0x0300
#define ALARM_POTS_BASE 0x0400
#define ALARM_E1_BASE 0x0500

#define ALARM_OBJ_TYPE_MASK 0xff00
#define ALARM_INSTANCE_TYPE_MASK 0x00ff
//#define ALARM_LOOPBACK 0x0304
//#define ALARM_LOOPBACK_COMEBACK 0x0304
//#define ALARM_PORT_UP NO_ALARM_NUM
//#define ALARM_PORT_DOWN NO_ALARM_NUM
//#define ALARM_UPS_POWER 0x0002
//#define ALARM_UPS_VOLT_LOW 0x0005 
//#define ALARM_UPS_MISSING 0x0003
//#define ALARM_UPS_ON NO_ALARM_NUM

#define ALARM_SIGNAL 0x00
#define ALARM_SIGNAL_RECOVER 0x01


typedef enum
{
    ALARM_PON_BEGIN = ALARM_PON_IF_BASE,
    ALARM_RX_POWER_HIGH, // 1
    ALARM_RX_POWER_LOW, 
    ALARM_TX_POWER_HIGH,
    ALARM_TX_POWER_LOW,
    ALARM_TX_BIAS_HIGH, // 5
    ALARM_TX_BIAS_LOW,
    ALARM_VCC_HIGH,
    ALARM_VCC_LOW,
    ALARM_TEMP_HIGH,
    ALARM_TEMP_LOW, // 11
    WARNING_RX_POWER_HIGH,
    WARNING_RX_POWER_LOW,
    WARNING_TX_POWER_HIGH,
    WARNING_TX_POWER_LOW,
    WARNING_TX_BIAS_HIGH,
    WARNING_TX_BIAS_LOW,
    WARNING_VCC_HIGH,
    WARNING_VCC_LOW, 
    WARNING_TEMP_HIGH, // 19
    WARNING_TEMP_LOW, 
}pon_alarm_t;

typedef enum
{
    ALARM_POTS_BEGIN = ALARM_POTS_BASE,
}pots_alarm_t;

typedef enum
{
    ALARM_ETHPORT_BEGIN = ALARM_ETH_PORT_BASE,
    ALARM_ETHPORT_AUTONEG,// 0301
    ALARM_ETHPORT_LOS,// 0302
    ALARM_ETHPORT_FAILURE, // 0303
    ALARM_ETHPORT_LDP, // 0304
}port_alarm_t;

typedef enum
{
    ALARM_CARD_BEGIN	= ALARM_CARD_BASE,
}card_alarm_t;

typedef enum
{
    ALARM_ONU_BEGIN = ALARM_ONU_BASE,
    ALARM_EQUIPMENT,
    ALARM_UPS_POWER,
    ALARM_UPS_MISSING,
    ALARM_BATTERY_FAILURE,
    ALARM_UPS_VOLT_LOW,
}onu_alarm_t;

typedef enum
{
    PON_TYPE,
    ETH_TYPE,
    VOIP_TYPE,
    ADSL2_TYPE,
    VDSL2_TYPE,
    E1_TYPE,
}port_type_t;

typedef enum {
    CTC_OBJ_ONU = 0xffff,
    CTC_OBJ_PORT = 0x0001,
    CTC_OBJ_CARD = 0x0002,
    CTC_OBJ_LLID = 0x0003,
    CTC_OBJ_PON_IF = 0x0004,
    CTC_OBJ_OTHER = 0xfffe,
} CTC_OBJ_TYPE_t;

typedef enum {
    CTC_STACK_ALARM_CONFIG_DISABLED = 0x1,
    CTC_STACK_ALARM_CONFIG_ENABLED = 0x2,
} CTC_STACK_alarm_admin_config_t;

#define INT8U char
#define INT16U unsigned short
#define INT32U unsigned int

#define Get_instanceNum(port_num, type) (type)<<24|(port_num+1)   
#define Get_objType()

#define MASK_1_BIT 0x1
#define MASK_2_BIT 0x3
#define MASK_3_BIT 0x7

#define ALARM_TYPE_LINK_MASK MASK_1_BIT
#define ALARM_TYPE_LDP_MASK MASK_1_BIT
#define ALARM_TYPE_UPS_MASK MASK_2_BIT

#define QUERY_TIME 3
#define OAM_ALARM_TIME 1
#define LDP_RESOLVE_TIME (9/QUERY_TIME)

#if 1
extern unsigned int g_alarm_eth_state[NUMBER_OF_PORTS];
extern unsigned int g_alarm_onu_state;
extern unsigned int g_alarm_pon_state;
extern unsigned int g_alarm_card_state;
extern unsigned int g_alarm_pots_state;
extern unsigned int g_alarm_e1_state;
#endif

/*针对端口的告警操作*/
#define clear_port_alarm_state(port, type,mask,g_value)	g_alarm_##g_value[port] &= ~((mask) << (type))
#define set_port_alarm_state(port, type,mask, g_value) 	g_alarm_##g_value[port] |= (mask) << (type) 
#define config_port_alarm_state(port, type, state, mask,g_value) do{clear_port_alarm_state(port, type,mask, g_value);\
											g_alarm_##g_value[port] |= state << (type);\
											}while(0)											
#define get_port_alarm_state(port,type,mask,g_value) (g_alarm_##g_value[port] >> (type) & (mask))

/*针对全局的告警操作*/
#define clear_globle_alarm_state(type,mask, g_value)	g_alarm_##g_value &= ~((mask) << (type))
#define set_globle_alarm_state(type,mask, g_value)	g_alarm_##g_value |= (mask) << (type) 
#define config_globle_alarm_state(type, state,mask, g_value) do{clear_globle_alarm_state(type,mask, g_value);\
											g_alarm_##g_value |= state << (type);\
											}while(0)											
#define get_globle_alarm_state(type,mask, g_value) (g_alarm_##g_value >> (type) & (mask))


/*告警状态位设置*/
#define get_eth_alarm_state(port, type) get_port_alarm_state(port, type,MASK_1_BIT,eth_state)	
#define config_eth_alarm_state(port, type, state) config_port_alarm_state(port, type, state,MASK_1_BIT, eth_state)	

#define get_onu_alarm_state(type) get_globle_alarm_state(type,MASK_1_BIT, onu_state)	
#define config_onu_alarm_state(type, state) config_globle_alarm_state(type, state,MASK_1_BIT, onu_state)	

#define get_pon_alarm_state(type) get_globle_alarm_state(type,MASK_1_BIT,pon_state)	
#define config_pon_alarm_state(type, state) config_globle_alarm_state(type, state,MASK_1_BIT, pon_state)	

/*告警开关*/
#if 1
#define get_eth_alarm_enable(port, type) get_port_alarm_state(port, type,MASK_1_BIT,eth_enable)	
#define config_eth_alarm_enable(port, type, state) config_port_alarm_state(port, type, state,MASK_1_BIT, eth_enable)	

#define get_onu_alarm_enable(type) get_globle_alarm_state(type,MASK_1_BIT, onu_enable)	
#define config_onu_alarm_enable(type, state) config_globle_alarm_state(type, state,MASK_1_BIT, onu_enable)	

#define get_pon_alarm_enable(type) get_globle_alarm_state(type,MASK_1_BIT,pon_enable)	
#define config_pon_alarm_enable(type, state) config_globle_alarm_state(type, state,MASK_1_BIT, pon_enable)	
#endif
/*end*/

typedef unsigned int (* alarm_num_t)(char cur_state);

#ifdef CTC_2_1
static int oam_alarm(alarm_report_t *info);
#else
static int oam_alarm(unsigned short alarm_num, char port);
#endif

extern void alarm_pthread();

/*外部获取告警状态变量*/
extern char g_alarm_ups_state;
extern unsigned char rstpwarningstate[NUMBER_OF_PORTS] ;

typedef enum
{
    ALARM_UPS_STATE_ON,
    ALARM_UPS_STATE_POWER_LOSING,
    ALARM_UPS_STATE_VOLT_LOW,
    ALARM_UPS_STATE_OFF,
}ups_alarm_t;

#define set_alarm_ups_cur_state(cur_state)	g_alarm_ups_state = cur_state
#define get_alarm_ups_cur_state() g_alarm_ups_state

//extern int oam_alarm_hook(INT8U opcode,  const INT8U* a_in_message,  const INT16U a_in_length, 
//                                              INT8U* a_out_buffer, INT16U *a_out_length);

//#define ALARM_DEG
//#define ALARM_DEG_TRACE
//#define ALARM_DEG_ARG
int CTC_STACK_alarm_set_thr(INT16U alarm_id, int port_num, INT32U rising_thr, INT32U falling_thr);
int CTC_STACK_alarm_get_thr(INT16U alarm_id, int port_num, INT32U *rising_thr, INT32U *falling_thr);
int CTC_STACK_alarm_get_state(INT16U alarm_id, int port_num, char *enabled);
int CTC_STACK_alarm_set_state(INT16U alarm_id, int port_num, char enabled);
extern void *epon_oam_ctc_alarm_thread(void *argu);

#ifdef ALARM_DEG_TRACE
#define TRACE_ALARM(fmt, arg...) printf(fmt,##arg);\
								fflush(stdout)
#else
#define TRACE_ALARM(fmt, arg...)
#endif

#ifdef ALARM_DEG_ARG
#define TRACE_ALARM_ARG(arg) TRACE_ALARM("%s:%lu\n", #arg, arg);\
								fflush(stdout)
#else
#define TRACE_ALARM_ARG(arg) 
#endif

#endif
