#ifndef _PDT_PRIV_CONFIG_H_
#define _PDT_PRIV_CONFIG_H_

#ifdef  __cplusplus
extern "C"{
#endif


#define MAX_UPLOAD_FILE_LEN 80*1024*1024 

#define CHIPSET_RTL9607 1
#define AGGRE_GRP_NUM 1
#define GE_PORT_NO			4
#define CABLE_PORT_NO		0

#define PON_VCC_GPIO  43
#define PON_MON_GPIO  35
#define PON_ALARM_GPIO	44

#define LAN0_LED	13
#define LAN1_LED	12
#define LAN2_LED	4
#define LAN3_LED	11

#define PON_LINK_GPIO 42  /*the gpio of EPN105 has no led function as hardware design*/
#define PON_LOS_GPIO  23
#define ENABLE_UART_GPIO 41

#define LAN0_GPIO	58
#define LAN1_GPIO	59
#define LAN2_GPIO	46
#define LAN3_GPIO	60
#define ONU_STYLE 1

#define MAX_PORT_NUM 4
#define LOGIC_PORT_NO   MAX_PORT_NUM
#define LOGIC_CPU_PORT  (LOGIC_PORT_NO + 3)
#define LOGIC_PON_PORT 	  LOGIC_PORT_NO + 1

#ifdef  __cplusplus
}
#endif
#endif

