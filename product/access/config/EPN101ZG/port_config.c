/*****************************************************************************
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif

#include <lw_type.h>
#include <lw_drv_pub.h>

/*NOTE:
rtl9607 has 1 ports and 3 phys.
cpu port id is 6.
So only port ids 0, 4, 6 are valid.
*/

PORT_TYPE_S g_szPortType[LOGIC_PORT_NO+7] = {
/* 0 */    {INVALID_PORT, INVALID_PORT, 0},
/* 1 */    {TO_USERPORT(1, 0, 1),  TOPHYID(0,0),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_1000)},
/* 2 */    {TO_USERPORT(1, 0, 2),  TOPHYID(0,4),	 	PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_1000)},
/* 3 */    {INVALID_PORT,  INVALID_PORT,     0},
/* 4 */    {INVALID_PORT,  INVALID_PORT,     0},
/* 5 */    {INVALID_PORT,  INVALID_PORT,	    0},
/* 6 */    {INVALID_PORT,  INVALID_PORT,     0},
/* rtl9607 CPU port is mac 6 and it's port id is port 6. */
/*LGC_CPU*/{INVALID_PORT,          TOPHYID(0,6),     PORT_TYPE_DEF(0, TP_CPU, 0, 0)}
};

/*只有端口为光电复用口才会添加到以下结构中*/
PORT_TYPE_S g_szComboPortSecType[LOGIC_PORT_NO+7] = {
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
/*1*/ 5,
/*2*/ 3,
/*3*/ 4,
/*4*/ LOGIC_PON_PORT,
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
