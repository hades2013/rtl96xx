/*****************************************************************************

*****************************************************************************/
#ifndef _PDT_CONFIG_H_
#define _PDT_CONFIG_H_

#ifdef  __cplusplus
extern "C"{
#endif

#include "lw_config.h"
#include "pdt_priv_config.h"

#ifdef CONFIG_ISOUSERVLAN
#define MODULE_ISOLATE_USER_VLAN 1
#else
#define MODULE_ISOLATE_USER_VLAN 0
#endif

#define BOOT_SECRET_KEY "LOSO"
#define PRODUCT_NAME CONFIG_PRODUCT_NAME
#define PROD_MODEL   PRODUCT_NAME
#define VTY_MOTD     "ONU Master Controller.Copyright(C) "
#define DEV_COPYRIGHT_STR CONFIG_COPYRIGHT_STRING
#define DEFAULT_VAL_HOSTNAME CONFIG_DEFAULT_HOSTNAME
#define IP_ZERO "0.0.0.0"

#define APP_UPGRADE_FILE "appupgrade.bin"
#ifndef CONFIG_HARD_VERSION
#define CONFIG_HARD_VERSION
#define HARDWARE_VERSION  CONFIG_DEVELOP_HARDWARE_VER
#endif

#define DEFAULT_SNMP_ENGINE_ID "80001f88803cd4279a386d4384"  
#define DEFAULT_SNMP_MAX_PACKET_LEN 1500 
#define DEFAULT_SNMP_LOCATION "China"  
#define DEFAULT_SNMP_CONTACT "R&D" 
#define DEFAULT_UDP_PORT 162 
#define DEFAULT_TRAP_IP "192.168.0.123"

#define CPU_PORT_TOTAL_RATE_LIMIT (2048)   
#define CPU_QUEUE_RATE_LIMIT      (2048)  

#define MIN_MAC_AGETIME 10       /*×?D?à??ˉê±??*/
#define MAX_MAC_AGETIME 1000000  /*×?′óà??ˉê±??*/

#define FE_PORT_NO     	    0
#define FIBER_PORT_NO       0
#define EXT_LGCPORT0 6


#define COMBO_PORT_NO 0
#define MAX_CHIP_NUM 1

#define CHIP1_PHY_PORT_NO  7
#define CHIP2_PHY_PORT_NO  0
#define PHY_PORT_NO    (CHIP1_PHY_PORT_NO+CHIP2_PHY_PORT_NO)

#define AGGRE_GRP_PORT_NUM  0
#define CHIP_STACK_PORT_NO 0
#define MAX_ARL_LEARN_LIMIT 2048
#define DIS_ARL_LEARN_LIMIT (MAX_ARL_LEARN_LIMIT+1)
#define MAX_ARL_TABLE (2*1024)
#define MAX_ARL_TABLE_INDEX (MAX_ARL_TABLE*MAX_CHIP_NUM)

#define MAX_MULTICAST_ENTRY 256
#define MAX_MAC_TAB_CONFIG  256
#define MAX_MMAC_TAB_CONFIG 256  /*组播最大配置数*/
#define DEFAULT_MAC_AGETIME 300  /*默认老化时间*/

#define DEFAULT_MANAGEMENT_VLAN 1
#define MAX_VLAN_INTERFACE 1   /*no more than 32*/
#define MAX_NETWORK_ROUTE 16    /*no more than 32*/
#define MAX_ACL_SUPPORT         64
#define MAX_ACL_ACTION_MIRROR    0    /*acl rule action mirror support three ports*/
#define CPU_TAG_ENABLE 0
//#define SUPPORT_CPU_PORT_ADD_TO_VLAN 0

#define QUEUE_NUM   4 /*only support 2 4 or 8*/
#define MAX_DEVICE_NUM      1

#define MAX_SLOT_NUM        3

#define DRV1_PER_COMBO_HAVE_PORT_NO  0
#define I2C_BUS_SUPPORT 1
#define MIN_COMBO_PORT_NUM  0
#define MAX_VLAN_NUMS 4096

#define LED_OFF 1
#define LED_ON  0

#define PORT_COUNTER_TYPE_LENGTH   2
#define PORT_TYPE_RX  0
#define PORT_TYPE_TX  1

#define EPON_OAM_STATE_WAIT_REMOTE          1
#define EPON_OAM_STATE_WAIT_REMOTE_OK       2
#define EPON_OAM_STATE_COMPLETE             3

#define GPIO_STATE_REGADDR_COM_H			0xBB0000F4
#define GPIO_STATE_REGADDR_COM_L			0xBB0000F0
#define BOOT_GPIO_BASE 						0xB8003300
#define BOOT_GPIO_CTRL_ABCDr (BOOT_GPIO_BASE+0x0) 	/*enable gpio function*/
#define BOOT_GPIO_CTRL_EFGHr (BOOT_GPIO_BASE+0x1c) 	/*enable gpio function*/
#define BOOT_GPIO_DIR_ABCDr	 (BOOT_GPIO_BASE+0x08) 	/*configure gpio pin to gpo or gpi*/
#define BOOT_GPIO_DIR_EFGHr  (BOOT_GPIO_BASE+0x24)	/*configure gpio pin to gpo or gpi*/
#define BOOT_GPIO_DATA_ABCDr (BOOT_GPIO_BASE+0x0c) 	/*datatit for input/output mode*/
#define BOOT_GPIO_DATA_EFGHr (BOOT_GPIO_BASE+0x28)	/*datatit for input/output mode*/
#define BOOT_GPIO_IMS_ABCDr  (BOOT_GPIO_BASE+0x10)	/*interrupt status */
#define BOOT_GPIO_IMS_EFGHr  (BOOT_GPIO_BASE+0x2c)	/*interrupt status */
#define BOOT_GPIO_IMR_ABr 	 (BOOT_GPIO_BASE+0x14)  /*interrupt mask register AB */
#define BOOT_GPIO_IMR_CDr 	 (BOOT_GPIO_BASE+0x18)  /*interrupt mask register CD*/
#define BOOT_GPIO_IMR_EFr	 (BOOT_GPIO_BASE+0x30)	/*interrupt mask register EF*/
#define BOOT_GPIO_IMR_GHr	 (BOOT_GPIO_BASE+0x34)	/*interrupt mask register GH*/
#define BOOT_GPIO_CPU0_ABCDr (BOOT_GPIO_BASE+0x38)	/*configure gpio imr to cpu 0*/
#define BOOT_GPIO_CPU1_ABCDr (BOOT_GPIO_BASE+0x3c)	/*configure gpio imr to cpu 1*/

#define GPIO_MODE_INPUT  		0
#define GPIO_MODE_OUTPUT 		1
#define GPIO_STATE_DISABLED  	0
#define GPIO_STATE_ENABLED 		1
#define GPIO_DATABIT_1			1
#define GPIO_DATABIT_0			0

/*
SN:PRODUCT_SERIES+A(factory)+YY(year)+MM(month)+D(batch)+XXXXXX...(water NO.)  total 22=5+6+11	
*/

#define SN_MAX_SIZE  22
#define SN_SUFFIX 	 ""       /*10^11*/
#define SN_STRING    CONFIG_PRODUCT_SERIES_AND_FACTORY_CODE""SN_SUFFIX

#define SNV_VERSION_MAC_PRODUCT_ENCRYPT 4230

#define CFGIMPORT_TIME 24
#if defined(HGU_STYLE)
#define UPGRADE_TIME 120
#else
#define UPGRADE_TIME 90
#endif

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif  /* end of _PDT_CONFIG_H_ */
