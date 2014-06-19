/*****************************************************************************
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif

#include <lw_type.h>
#include <lw_drv_pub.h>

/*NOTE:
rtl8305 has 5 ports and 4 phys.
External interface 1's port id is 6.
So we define port number 7.
But only port ids 0/1/2/3/6 are valid.
*/

PORT_TYPE_S g_szPortType[LOGIC_PORT_NO+2] = {
/* 0 */    {INVALID_PORT, INVALID_PORT, 0},
/* 1 */    {TO_USERPORT(1, 0, 1),  TOPHYID(0,0),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_100)},
/* 2 */    {TO_USERPORT(1, 0, 2),  TOPHYID(0,1),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_100)},
/* 3 */    {TO_USERPORT(1, 0, 3),  TOPHYID(0,2),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_100)},
/* 4 */    {TO_USERPORT(1, 0, 4),  TOPHYID(0,3),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_100)},
/* rtl8305 uplink port is external mac 1 and it's port id is port 6. */
/*LGC_CPU*/{INVALID_PORT,          TOPHYID(0,6),     PORT_TYPE_DEF(0, TP_CPU, 0, 0)}
};

/*ֻ�ж˿�Ϊ��縴�ÿڲŻ���ӵ����½ṹ��*/
PORT_TYPE_S g_szComboPortSecType[LOGIC_PORT_NO+2] = {
/* 0 */    {INVALID_PORT, INVALID_PORT, 0},
/* 1 */    {INVALID_PORT, INVALID_PORT, 0},
/* 2 */    {INVALID_PORT, INVALID_PORT, 0},
/* 3 */    {INVALID_PORT, INVALID_PORT, 0},
/* 4 */    {INVALID_PORT, INVALID_PORT, 0},
/*LGC_CPU*/{INVALID_PORT, INVALID_PORT, 0}
};

port_num_t g_szPhyToLogicMap[PHY_PORT_NO] = {
/*chip1*/    
/*0*/ 1,
/*1*/ 2,
/*2*/ 3,
/*3*/ 4,
/*4*/ INVALID_PORT,
/*5*/ INVALID_PORT,
/*6*/ LOGIC_CPU_PORT
};

UINT32 g_szChipMaxPort[MAX_CHIP_NUM]={
    CHIP1_PHY_PORT_NO
};

UINT32 g_szUserMaxPortNum[DEVICE_NO+1][MAX_SLOT_NO]={
    /*device 1*/{/*slot 0*/DEVICE1_SLOT0_PORT_NO, /*slot 1*/DEVICE1_SLOT1_PORT_NO, /*slot 2*/DEVICE1_SLOT2_PORT_NO},
    /*   end  */{/*slot 0*/0, /*slot 1*/0, /*slot 2*/0}
};

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */
