#ifndef _PDT_PRIV_CONFIG_H_
#define _PDT_PRIV_CONFIG_H_

#ifdef  __cplusplus
extern "C"{
#endif

#define MAX_UPLOAD_FILE_LEN 45*1024*1024


#define CHIPSET_RTL9607 1
#define AGGRE_GRP_NUM 0
#define GE_PORT_NO     2
#define CABLE_PORT_NO  1

#define PON_VCC_GPIO  23
#define PON_MON_GPIO  35
#define PON_ALARM_GPIO  46
#define PON_LINK_GPIO	21				
#define LAN0_GPIO		58
#define LAN1_GPIO		59

#define LAN0_LED		13
#define LAN1_LED		12
#define PON_LINK_LED    15		

#define SWITCH_STYLE 1
#define MAX_PORT_NUM 3
#define LOGIC_PORT_NO   MAX_PORT_NUM
#define LOGIC_CPU_PORT  (LOGIC_PORT_NO + 4)
#define LOGIC_PON_PORT 	  LOGIC_PORT_NO + 1

#ifdef  __cplusplus
}
#endif
#endif


