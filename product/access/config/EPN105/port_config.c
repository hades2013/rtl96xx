/*****************************************************************************
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif

#include <lw_type.h>
#include <lw_drv_pub.h>

/*NOTE:
rtl8353 has 52 ports and 52 phys.
cpu port id is 52.
So only port ids 0-52 are valid.
*/

PORT_TYPE_S g_szPortType[LOGIC_PORT_NO+3] = {
/* 0 */    {INVALID_PORT, INVALID_PORT, 0},
/* 1 */    {TO_USERPORT(1, 0, 1),  TOPHYID(0,0),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_1000)},
/* 2 */    {TO_USERPORT(1, 0, 2),  TOPHYID(0,1),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_1000)},
/* 3 */    {TO_USERPORT(1, 0, 3),  TOPHYID(0,2),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_1000)},
/* 4 */    {TO_USERPORT(1, 0, 4),  TOPHYID(0,3),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_1000)},
/* 5 */    {TO_USERPORT(1, 0, 5),  TOPHYID(0,5),     PORT_TYPE_DEF(0,TP_CABLE, UD_DOWNLINK, SP_1000)},
/* 6 */    {TO_USERPORT(1, 0, 6),  TOPHYID(0,4),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_1000)},
/* rtl9607 CPU port is mac 6 and it's port id is port 6. */
/*LGC_CPU*/{INVALID_PORT,          TOPHYID(0,6),     PORT_TYPE_DEF(0, TP_CPU, 0, 0)}
};

/*只有端口为光电复用口才会添加到以下结构中*/
PORT_TYPE_S g_szComboPortSecType[LOGIC_PORT_NO+3] = {
/* 0 */    {INVALID_PORT, INVALID_PORT, 0},
/* 1 */    {INVALID_PORT, INVALID_PORT, 0},
/* 2 */    {INVALID_PORT, INVALID_PORT, 0},
/* 3 */    {INVALID_PORT, INVALID_PORT, 0},
/* 4 */    {INVALID_PORT, INVALID_PORT, 0},
/* 5 */    {INVALID_PORT, INVALID_PORT, 0},
/* 6 */    {INVALID_PORT, INVALID_PORT, 0},
/*LGC_CPU*/{INVALID_PORT, INVALID_PORT, 0}
};

port_num_t g_szPhyToLogicMap[PHY_PORT_NO] = {
/*chip1*/    
/*0*/ 1,
/*1*/ 2,
/*2*/ 3,
/*3*/ 4,
/*4*/ 5,
/*5*/ 6,
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
