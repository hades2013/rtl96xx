/*
=============================================================================
     Header Name: interrupt.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2008/2/3		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifndef INTERRUPT_H
#define INTERRUPT_H
#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"
#include "opl_utils.h"
#include "hw_interface.h"

//#include "hal_interrupt.h"

#define OPL_MAX_NUM_OF_INTR_LEVEL2  32

typedef enum UPTM_INT_TYPE_s
{
	TMUS_QUEUE_FULL_INT_TYPE,
	TMUS_RD_ERR_INT_TYPE,
	TMUS_PKT_POS_ERR_INT_TYPE,
	TMUS_PKT_LEN_ERR_INT_TYPE,
	TMUS_LL_FULL_INT_TYPE,
	TMUS_RPT_ERR_INT_TYPE,
	TM_HEAD_DISORDER_INT_TYPE,
	TM_HEAD_OVFLW_INT_TYPE,
	TM_MPCP_SEND_FAIL_ERR_INT_TYPE,
	TM_MPCP_SEND_OK_ERR_INT_TYPE,
	TMUS_INT_TYPE_END
}UPTM_INT_TYPE_e;

typedef enum DNTM_INT_TYPE_s
{
	TMDS_QUEUE_FULL_INT_TYPE,
	TMDS_RD_ERR_INT_TYPE,
	TMDS_PKT_POS_ERR_INT_TYPE,
	TMDS_PKT_LEN_ERR_INT_TYPE,
	TMDS_LL_FULL_INT_TYPE,
	TMDS_INT_TYPE_END
}DNTM_INT_TYPE_e;

typedef enum BRG_INT_TYPE_s
{
	RX_DIS_DROP_INT_TYPE,	
	TX_DIS_DROP_INT_TYPE,
	RSV_MAC_DROP_INT_TYPE,
	USR_MAC_DROP_INT_TYPE,
	US_VTT_ERR_INT_TYPE,
	DS_VTT_ERR_INT_TYPE,
	US_TAG_ERR_INT_TYPE,
	DS_TAG_ERR_INT_TYPE,
	US_ARL_SMAC_CFL_INT_TYPE,
	DS_ARL_SMAC_CFL_INT_TYPE,
	US_ZERO_DA_DROP_INT_TYPE,
	DS_ZERO_DA_DROP_INT_TYPE,
	ARL_FULL_ARL_NEW_REQ_INT_TYPE,
	US_NON_UCST_DROP_INT_TYPE,
	DS_NON_UCST_DROP_INT_TYPE,
	BRG_INT_TYPE_END
}BRG_INT_TYPE_e;

typedef enum MPCP_INT_TYPE_s
{
	MPCP_STATE_INT_TYPE,
	MPCP_GATE_DISORDER_INT_TYPE,
	MPCP_ECHO_PENDING_ERR_INT_TYPE,
	MPCP_STATE_CHANGE_INT_TYPE,	
	MPCP_GRANT_LIST_FULL_INT_TYPE,
	MPCP_GATE_TIMEOUT_INT_TYPE,
	MPCP_LINK_UP_INT_TYPE,
	MPCP_LINK_DOWN_INT_TYPE,
	MPCP_INT_TYPE_END
}MPCP_INT_TYPE_e;

typedef enum MISC_INT_TYPE_s
{
	MISC_DATA_SPACE_FULL_INT_TYPE,
	MISC_TRAF_OVFLW_INT_TYPE,
	MISC_MPIS_UFLOW_INT_TYPE,
	MISC_LL_EMPTY_INT_TYPE,
	MISC_INT_TYPE_END
}MISC_INT_TYPE_e;

typedef enum PMAC_INT_TYPE_s
{
	SIGNAL_VAL_INT_TYPE,
    SYNC_ERR_RX_INT_TYPE,
    SYNC_ERR_TX_INT_TYPE,
    RXPLL_LCK_CHANGE_INT_TYPE,
    TXPLL_LCK_CHANGE_INT_TYPE,
    LINK_CHANGE_INT_TYPE,
	CRC8_ERR_INT_TYPE,
	LOS_IND_INT_TYPE,
	PMAC_INT_TYPE_END
}PMAC_INT_TYPE_e;

typedef enum PCM_INT_TYPE_s
{
    TX_FIFO_FULL_ERR_INT_TYPE,
    TX_FIFO_EMPTY_ERR_INT_TYPE,
    RX_FIFO_FULL_ERR_INT_TYPE,
    RX_FIFO_EMPTY_ERR_INT_TYPE,
	TX_FIFO_ALMOST_EMPTY_INT_TYPE,
	RX_FIFO_ALMOST_FULL_INT_TYPE,
	PCM_INT_TYPE_END
}PCM_INT_TYPE_e;

typedef enum OPL_INTR_L1_ID_s 
{
	OPL_IRQ0_INTR,
	OPL_IRQ1_INTR,
	OPL_IRQ2_INTR,
	OPL_IRQ3_INTR,
	OPL_IRQ4_INTR,
	OPL_IRQ5_INTR,
	OPL_IRQ6_INTR,
	OPL_IRQ7_INTR,
	OPL_IRQ8_INTR,
	OPL_IRQ9_INTR,
	OPL_IRQ10_INTR,
	OPL_IRQ11_INTR,
	OPL_MISC_INTR,
	OPL_BRG_INTR,
	OPL_UPTM_INTR,
	OPL_DNTM_INTR,
	OPL_MPCP_INTR,
	OPL_PMAC_INTR,
	OPL_GE_INTR,
	OPL_PCM_INTR,
	OPL_DMA0_INTR,
	OPL_DMA1_INTR,
	OPL_SPI_INTR,
	OPL_WDOG_INTR,
	OPL_RTC_INTR,
	OPL_TIMER0_INTR,
	OPL_TIMER1_INTR,
	OPL_TIMER2_INTR,
	OPL_TIMER3_INTR,
	OPL_I2C_INTR,
	OPL_UART0_INTR,
	OPL_UART1_INTR,
	OPL_END_INTR
}OPL_INTR_L1_ID_e;

#define HAL_MAX_NUM_OF_INTR_LEVEL2 32

typedef struct HAL_EVENT_INT_HANDLING_s
{
    UINT32 ulModInt;        /* level 1 interrupt: 0 ~ 0xFFFFFFFF */
    UINT32 ulSubModInt[HAL_MAX_NUM_OF_INTR_LEVEL2];     /* level 2 interrupt: 0 ~ 0xFFFFFFFF */
} HAL_EVENT_INT_HANDLING_t;

typedef enum HAL_INTR_L1_ID_s 
{
	HAL_INTR_IRQ0,
	HAL_INTR_IRQ1,
	HAL_INTR_IRQ2,
	HAL_INTR_IRQ3,
	HAL_INTR_IRQ4,
	HAL_INTR_IRQ5,
	HAL_INTR_IRQ6,
	HAL_INTR_IRQ7,
	HAL_INTR_IRQ8,
	HAL_INTR_IRQ9,
	HAL_INTR_IRQ10,
	HAL_INTR_IRQ11,
	HAL_INTR_MISC,
	HAL_INTR_BRG,
	HAL_INTR_UPTM,
	HAL_INTR_DNTM,
	HAL_INTR_MPCP,
	HAL_INTR_PMAC,
	HAL_INTR_GE,
	HAL_INTR_PCM,
	HAL_INTR_DMA0,
	HAL_INTR_DMA1,
	HAL_INTR_SPI,
	HAL_INTR_WDOG,
	HAL_INTR_RTC,
	HAL_INTR_TIMER0,
	HAL_INTR_TIMER1,
	HAL_INTR_TIMER2,
	HAL_INTR_TIMER3,
	HAL_INTR_I2C,
	HAL_INTR_UART0,
	HAL_INTR_UART1,
	HAL_INTR_END
}HAL_INTR_L1_ID_e;


#pragma pack(1)		/* pragma pack definition */

typedef struct OPL_INT_LEVEL2_s
{
	INT16 	id;
	UINT8	*name;
	UINT32 	penAddr;
	UINT32 	penBit;
	INT32 		enAddr;
	UINT32 	enBit;
	UINT32 	counter;
	UINT32    	ulErrCnt;
}OPL_INT_LEVEL2_t;

typedef struct OPL_INT_s
{
	OPL_INTR_L1_ID_e 	 id;
	UINT8 						* name;
	UINT32 					counter;
	UINT32 					errCounter;
	OPL_INT_LEVEL2_t 	level2Int[OPL_MAX_NUM_OF_INTR_LEVEL2];
}OPL_INT_t;

#pragma pack()

/*******************************************************************************
* intMpcpLinkDowHandler
*
* DESCRIPTION:
* 
*	this function handler the mpcp link down
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intMpcpLinkDowHandler(void);
/*******************************************************************************
* intSerEnHwWrite
*
* DESCRIPTION:
* 
*	this function is used to enable the opconn interrupt.
* 
* INPUTS: 
*		enable:1 for enable while 0 for disable 
* OUTPUTS: 
*		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intSerEnHwWrite(UINT8 enable);

/*******************************************************************************
* intSerEnHwRead
*
* DESCRIPTION:
* 
*	this function is used to get wether the interrupt is enable
* 
* INPUTS: 
*
* OUTPUTS: 
*	eanbe: 0 for disable while 1 for enable		
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intSerEnHwRead(UINT8 *enable);

/*******************************************************************************
* intUPTmEnHwWrite
*
* DESCRIPTION:
* 
*	this function is used enable or disable the tm interrupt
* 
* INPUTS: 
*	type: 
*		0	TMUS_INT_TYPE
*		1	TMUS_MARB_READERR_INT_TYPE
*		2  TMDS_INT_TYPE
*		3 	TMDS_MARB_READERR_INT_TYPE
* OUTPUT:
*	enable:
*		0 	for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intUPTmEnHwWrite(UINT8 type,UINT8 enable);

/*******************************************************************************
* intUPTmEnHwRead
*
* DESCRIPTION:
* 
*	this function is used get the  enable or disable status of tm interrupt
* 
* INPUTS: 
*		type: 
*			0	TMUS_INT_TYPE
*			1	TMUS_MARB_READERR_INT_TYPE
*			2	TMDS_INT_TYPE
*			3 	TMDS_MARB_READERR_INT_TYPE
*	OUTPUT
*		enable:
*			0 	for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intUPTmEnHwRead(UINT8 type,UINT8 *enable);

/*******************************************************************************
* intDNTmEnHwWrite
*
* DESCRIPTION:
* 
*	this function is used enable or disable the tm interrupt
* 
* INPUTS: 
*	type: 
*		0	TMUS_INT_TYPE
*		1	TMUS_MARB_READERR_INT_TYPE
*		2   TMDS_INT_TYPE
*		3 	TMDS_MARB_READERR_INT_TYPE
* OUTPUT:
*	enable:
*		0 	for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intDNTmEnHwWrite(UINT8 type,UINT8 enable);

/*******************************************************************************
* intDNTmEnHwRead
*
* DESCRIPTION:
* 
*	this function is used get the  enable or disable status of tm interrupt
* 
* INPUTS: 
*		type: 
*			0	TMUS_INT_TYPE
*			1	TMUS_MARB_READERR_INT_TYPE
*			2	TMDS_INT_TYPE
*			3 	TMDS_MARB_READERR_INT_TYPE
*	OUTPUT
*		enable:
*			0 	for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intDNTmEnHwRead(UINT8 type,UINT8 *enable);

/*******************************************************************************
* intBrgEnHwWrite
*
* DESCRIPTION:
* 
*	this function is used enable or disable the bridge interrupt
* 
* INPUTS: 
*	type: 
*			0 		NON_UC_SA_DRP_INT_TYPE,
*			1		ARL_FULL_ARL_NEW_REQ_INT_TYPE,
*			2		UPLINK_VTG_ERR_INT_TYPE,
*			3		ACC_VTG_ERR_INT_TYPE,
*			4		DN_VP_ERR_INT_TYPE,
*			5		UP_VT_ERR_INT_TYPE,
*			6		USR_MAC_DROP_INT_TYPE,
*			7		RSV_MAC_DRP_INT_TYPE,
*			8		TX_DIS_DRP_INT_TYPE,
*			9		RX_DIS_DRP_INT_TYPE,
*
*	enable:
*			0 for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intBrgEnHwWrite(UINT8 type,UINT8 enable);

/*******************************************************************************
* intBrgEnHwRead
*
* DESCRIPTION:
* 
*	this function is used get wether the interrupt is enable .
* 
* INPUTS: 
*	type: 
*			0 		NON_UC_SA_DRP_INT_TYPE,
*			1		ARL_FULL_ARL_NEW_REQ_INT_TYPE,
*			2		UPLINK_VTG_ERR_INT_TYPE,
*			3		ACC_VTG_ERR_INT_TYPE,
*			4		DN_VP_ERR_INT_TYPE,
*			5		UP_VT_ERR_INT_TYPE,
*			6		USR_MAC_DROP_INT_TYPE,
*			7		RSV_MAC_DRP_INT_TYPE,
*			8		TX_DIS_DRP_INT_TYPE,
*			9		RX_DIS_DRP_INT_TYPE
*	OUTPUT:
*	enable:
*			0 for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intBrgEnHwRead(UINT8 type,UINT8 *enable);

/*******************************************************************************
* intMpcpEnHwWrite
*
* DESCRIPTION:
* 
*	this function is used to config the interrupt enable or disable
* 
* INPUTS: 
*	type: 
*		0	MPCP_INT_TYPE,
*		1	MPCP_STATE_CHANGE_INT_TYPE
*	enable:
*		0 disable
*		1 enable
*	OUTPUT:
*	enable:
*			0 for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intMpcpEnHwWrite(UINT8 type,UINT8 enable);

/*******************************************************************************
* intMpcpEnHwRead
*
* DESCRIPTION:
* 
*	this function is used to get the config value of the interrrupt
* 
* INPUTS: 
*	type: 
*		0	MPCP_INT_TYPE,
*		1	MPCP_STATE_CHANGE_INT_TYPE
*	enable:
*		0 disable
*		1 enable
*	OUTPUT:
*	enable:
*			0 for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intMpcpEnHwRead(UINT8 type,UINT8 *enable);

/*******************************************************************************
* intDdrEnHwWrite
*
* DESCRIPTION:
* 
*	this function is used enable or disable the ddr interrupt
* 
* INPUTS: 
*	type: 
*		0 	MPIS_ADDR_UNDERFLOW_INT_TYPE,
*		1	TRAFFIC_ADDR_OVERFLOW_INT_TYPE,
*		2	SDRAM_FULL_INT_TYPE,
* OUTPUT:
*	enable:
*		0 	for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intDdrEnHwWrite(UINT8 type,UINT8 enable);

/*******************************************************************************
* intDdrEnHwRead
*
* DESCRIPTION:
* 
*	this function is used get the  enable or disable status of tm interrupt
* 
* INPUTS: 
*		type: 
*		0 	MPIS_ADDR_UNDERFLOW_INT_TYPE,
*		1	TRAFFIC_ADDR_OVERFLOW_INT_TYPE,
*		2	SDRAM_FULL_INT_TYPE,
*	OUTPUT
*		enable:
*			0 	for disable while 1 for enable
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intDdrEnHwRead(UINT8 type,UINT8 *enable);


/*******************************************************************************
* intMiscHandler
*
* DESCRIPTION:
* 
*	this function handler the misc interrupt,such as mpcp interrupt.
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intMiscHandler(UINT32 intIndex, UINT32 level2Int);

/*******************************************************************************
* intBrgHandler
*
* DESCRIPTION:
* 
*	this function handler the brg interrupt.
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intBrgHandler(UINT32 intIndex, UINT32 level2Int);

/*******************************************************************************
* intUPTMHandler
*
* DESCRIPTION:
* 
*	this function handler the UPTM interrupt.
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intUPTMHandler(UINT32 intIndex, UINT32 level2Int);

/*******************************************************************************
* intDNTMHandler
*
* DESCRIPTION:
* 
*	this function handler the UPTM interrupt.
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intDNTMHandler(UINT32 intIndex, UINT32 level2Int);


/*******************************************************************************
* intMpcpHandler
*
* DESCRIPTION:
* 
*	this function handler the mpcp interrupt.
* 
* INPUTS: 
*	intIndex:
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intMpcpHandler(UINT32 intIndex, UINT32 level2Int);

/*******************************************************************************
* intProcesstHandler
*
* DESCRIPTION:
* 
*	this function process the interrrupt 
* 
* INPUTS: 
*	ulIrqPending: 
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intProcesstHandler(HAL_EVENT_INT_HANDLING_t *pstHandleInt);

/*******************************************************************************
* intCounterShow
*
* DESCRIPTION:
* 
*	this function used to show the int counter  
* 
* INPUTS: 
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intCounterShow(void);


/*******************************************************************************
* intCountEnable
*
* DESCRIPTION:
* 
*	this function used to disable or enable interrupt counter
* 
* INPUTS: 
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intCountEnable(UINT8 enable);


/*******************************************************************************
* intCountEnableState
*
* DESCRIPTION:
* 
*	this function used to get the state of interrupt counter
* 
* INPUTS: 
*
* OUTPUTS:
*
* RETURNS:
* OPL_OK if the access is success.
* OPL_ERROR if the access is failed.
* SEE ALSO: 
*/
OPL_STATUS intCountEnableState(UINT32 *enable);

/*******************************************************************************
*
* interruptLevel1Enable - enable or disable level 1 interrupts
*
* DESCRIPTION:
* 	enable or disable level 1 interrupts.
*
* INPUT:
*	ulModule    - level 1 interrupt: 0 ~ 32, 32 enable or disable all level 1 interrupts
*	ulEnable    - 0 or 1, 0 to disable; 1 to enbale:
*
* OUTPUT:
*	none
*
* RETURNS:
* 	n/a.
*
* SEE ALSO: 
*/

OPL_STATUS interruptLevel1Enable(UINT32 ulModule, UINT32 ulEnable);

/*******************************************************************************
*
* interruptLevel2Enable - enable or disable level 2 interrupts
*
* DESCRIPTION:
* 	enable or disable level 2 interrupts.
*
* INPUT:
*	ulModule	- level 1 interrupt: 0 ~ 32, 32 enable or disable all level1 interrupts
*   ulSubModule - level 2 interrupt: 0 ~ 32, 32 enable or disable all level2 interrupts in the module
*	ulEnable	- 0 or 1, 0 to disable; 1 to enbale:
*
* OUTPUT:
*	none
*
* RETURNS:
* 	n/a.
*
* SEE ALSO: 
*/
OPL_STATUS interruptLevel2Enable(UINT32 ulModule, UINT32 ulSubModule, UINT32 ulEnable);

/*******************************************************************************
*
* interruptLevel1EnableState - get level 1 interrupt state
*
* DESCRIPTION:
* 	get level 1 interrupt state
*
* INPUT:
*	ulModule	- level 1 interrupt: 0 ~ 31
*	pulEnable	- pointer to destination value, 0 or 1, 0 to disable; 1 to enbale
*
* OUTPUT:
*	none
*
* RETURNS:
* 	n/a.
*
* SEE ALSO: 
*/
OPL_STATUS interruptLevel1EnableState(UINT32 ulModule, UINT32 *pulEnable);

/*******************************************************************************
*
* interruptLevel2EnableState - get level 2 interrupt state
*
* DESCRIPTION:
* 	get level 2 interrupt state
*
* INPUT:
*	ulModule	- level 1 interrupt: 0 ~ 31
*   ulSubModule	- level 2 interrupt: 0 ~ 31
*	pulEnable	- pointer to destination value, 0 or 1, 0 to disable; 1 to enbale
*
* OUTPUT:
*	none
*
* RETURNS:
* 	n/a.
*
* SEE ALSO: 
*/
OPL_STATUS interruptLevel2EnableState(UINT32 ulModule, UINT32 ulSubModule, UINT32 *pulEnable);


#endif

