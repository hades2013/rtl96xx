#ifndef _PDT_PRIV_CONFIG_H_
#define _PDT_PRIV_CONFIG_H_

#ifdef  __cplusplus
extern "C"{
#endif

#define MAX_UPLOAD_FILE_LEN 155*1024*1024


#define CHIPSET_RTL9601 1
#define AGGRE_GRP_NUM 1
#define GE_PORT_NO			4
#define CABLE_PORT_NO		0

#define PON_VCC_GPIO  43 /*can not contrl by software, it is always high by hardware*/
#define PON_MON_GPIO  21	
#define PON_ALARM_GPIO	46
#define PON_LINK_GPIO	12
#define LAN2_GPIO		48		
#define LAN2_LED		2		
#define PON_LINK_LED	9

#define ONU_STYLE 1
#define MAX_PORT_NUM 1
#define LOGIC_PORT_NO   MAX_PORT_NUM
#define LOGIC_CPU_PORT  (LOGIC_PORT_NO + 6)
#define LOGIC_PON_PORT 	  LOGIC_PORT_NO + 1

#ifdef  __cplusplus
}
#endif
#endif

