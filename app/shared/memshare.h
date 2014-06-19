/*****************************************************************************

      File name:memshare.h
      Description:header file of memshare.c
      Author:liaohongjun
      Date:
              2012/11/29
==========================================================
      Note:file added by liaohongjun 2012/11/30 of EPN104QID0084
*****************************************************************************/

#ifndef _MEMSHARE_H_
#define _MEMSHARE_H_

#ifdef  __cplusplus
extern "C"{
#endif

#include "lw_type.h"
#include <lw_drv_pub.h>

#define MEMSH_DBG_EN    (0)
#if MEMSH_DEG_EN
  #define MEMSH_DBG(fmt, ...) do { fprintf(stderr, "[%s,%d]: " fmt, __FUNCTION__, __LINE__ , ##__VA_ARGS__); fprintf(stderr, "\n"); } while(0)
#else /* else of MEMSH_DBG_EN */
  #define MEMSH_DBG(fmt, ...)
#endif /* end of else of MEMSH_DBG_EN */

#define LOOPD_CUR_PORTSTATE_SHMID 0x123456
#define CATV_WORK_INFO_SHMID 0x123457

typedef struct tagPort_LoopdCurState
{
    UINT8 curstate;
}LOOPD_CURSTATE_S;

typedef struct tagPort_LoopdState
{
    LOOPD_CURSTATE_S port[LOGIC_PORT_NO + 1];//logic port start from 1,so 0 is not use
}PORT_LOOPD_STATE_S;

#define SNMP_SHM_LABEL 0x12 
#define MAX_DATA_SIZE 			64
#define MASTER_TO_SNMP_GET		0
#define MASTER_TO_SNMP_SET		1
/*MASTER TO SNMP SET type*/
#define LINKDOWN_TRAP   0
#define LINKUP_TRAP     1
#define REREAD_CONFIG  			2
#define SEND_IP_CHANGE_TRAP 	3
#define FLAG_MASK				0x8000
#define VALUE_MASK 				0x7FFF


typedef struct{
	UINT8 access;
	UINT8 type;
	char data[MAX_DATA_SIZE];
	}snmp_data_t;
snmp_data_t *snmp_data_shmInit(void);
/*Begin add by huangmingjian 2013/01/23 */ 
#pragma pack (1)
typedef struct tag_OptFloats{
	INT16 val_int;
	UINT8 val_dec;
}CATV_MEMSHFLOAT_S;
typedef struct tag_Zg_OptFloats{
	INT8 val_int;
	UINT8 val_dec;
}CATV_ZG_MEMSHFLOAT_S;
#pragma pack ()
typedef struct tag_CatvInfo 
{
#if defined(CONFIG_ZBL_SNMP)
	UINT8 catv_connet;
	UINT8 rf_contrl;
	INT8 rf_offset;
    INT16 power_12v;            //电源电压    
    INT16 input_laser_power;    //输入光功率
    INT16 input_laser_power_mw;    //输入光功率
    INT16 output_level;         //输出电平
    CATV_ZG_MEMSHFLOAT_S temper;               //设备温度
    UINT8 catv_laser_test;
    UINT8 catv_rev_first; 
#else
    UINT8 vendor_id;          //CATV 厂商ID
    UINT8 dev_id;             //CATV设备ID
    UINT8 soft_ver;           //CATV软件版本
    UINT8 hardw_ver;          //CATV硬件版本
    UINT8 com_ver;        //串口协议版本
    CATV_MEMSHFLOAT_S power_12v;            //电源电压    
    CATV_MEMSHFLOAT_S input_laser_power;    //输入光功率
    CATV_MEMSHFLOAT_S output_level;         //输出电平
    CATV_MEMSHFLOAT_S temper;               //设备温度
    UINT32 output_atten_set;     //输出衰减设置
    UINT8 catv_laser_test; /*catv serial test flag :0 don't test ,1 test, default to 0*/ 
    UINT8 catv_rev_first;   /*when this para set to 1,current board will be set as receive board,otherwise it will default to send board*/
#endif
}CATV_WORK_INFO_S;

#define VAL_INT_ERROR		(0x7FFF-1)
#define VAL_UINT8_ERROR		0xFF
#define VAL_UINT32_ERROR		0xFFFFFFFF
#define ERROR_DISPLAY		"N/A"

#define LASER_NAOMAL 		0
#define LASER_FORCE_ON  	1
#define LASER_FORCE_OFF   	2


#ifdef CONFIG_PRODUCT_5500
#define PONSTATUS_SHM_LABEL 	0x24 
typedef struct tag_ponStatus_webDis
{
    UINT8 isOnline; 
	char ctcVer[BUF_SIZE_16];
	UINT8 remoteInfovalid;
	char oamVer[BUF_SIZE_16]; 
	char oui[BUF_SIZE_16];
	char venderSpecInfo[BUF_SIZE_16];
}ponStatus_webDis_t;
extern ponStatus_webDis_t *ponStatus_webDis_shmInit(void);
#endif

PORT_LOOPD_STATE_S *LoopdPortCurStateShmInit(void);
CATV_WORK_INFO_S *CATV_WorkInfoShmInit(void);


#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif  /* end of _MEMSHARE_H_ */


