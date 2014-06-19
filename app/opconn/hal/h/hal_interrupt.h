/*************************************************************************
*
*  COPYRIGHT (C) 2003-2009 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  hal_interrupt.h
*
* DESCRIPTION: 
*	
*
* Date Created: Mar 18, 2009
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/hal/h/hal_interrupt.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __HAL_INTERRUPT_H_
#define __HAL_INTERRUPT_H_
 
#ifdef __cplusplus
extern "C" {
#endif

/* API */

typedef struct HAL_API_INT_LEVEL1_EANBLE_s
{
    UINT32 ulModule;    /* level 1 interrupt id: 0 ~ 31, 
                         * 32 enable or disable all level 1 interrupts 
                         */
    UINT32 ulEnable;    /* 0 disable, 1 enable */
    UINT32 ulResult;
} HAL_API_INT_LEVEL1_ENABLE_t;


typedef struct HAL_API_INT_LEVEL1_ENABLE_STATE_s
{
    UINT32 ulModule;    /* level 1 interrupt id: 0 ~ 31 */
    UINT32 ulEnable;    /* 0 disable, 1 enable */
    UINT32 ulResult;
} HAL_API_INT_LEVEL1_ENABLE_STATE_t;


typedef struct HAL_API_INT_LEVEL2_ENABLE_s
{
    UINT32 ulModule;    /* level 1 interrupt id: 0 ~ 31, 
                         * 32 enable or disable all level 1 interrupts 
                         */
    UINT32 ulSubModule; /* level 2 interrupt id: 0 ~ 31, 
                         * 32 enable or disable all level2 interrupts in the module 
                         */
    UINT32 ulEnable;    /* 0 disable, 1 enable */
    UINT32 ulResult;
} HAL_API_INT_LEVEL2_ENABLE_t;


typedef struct HAL_API_INT_COUNTER_EN_s
{
    UINT32 ulEnable;
    UINT32 ulResult;
} HAL_API_INT_COUNTER_EN_t;


typedef struct HAL_API_INT_COUNTER_SHOW_s
{
    UINT32 ulFd;
} HAL_API_INT_COUNTER_SHOW_t;


/* EVENT */
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


#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __HAL_INTERRUPT_H_ */
 

