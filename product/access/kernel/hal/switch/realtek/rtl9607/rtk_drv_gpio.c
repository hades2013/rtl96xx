#ifdef  __cplusplus
    extern "C"{
#endif
#include <lw_type.h>
#include <linux/mutex.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include "hal_common.h"
/*realtek sdk include*/
#include <common/error.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/gpio.h>
#include <rtk/irq.h>

/*----------------------------------------------*
 * module parameter                             *
 *----------------------------------------------*/
struct mutex m_stRegMutex;

/*----------------------------------------------*
 * macro                                        *
 *----------------------------------------------*/
#define REG_MUTEX_INIT     mutex_init(&m_stRegMutex)
#define REG_MUTEX_LOCK     mutex_lock(&m_stRegMutex)
#define REG_MUTEX_UNLOCK   mutex_unlock(&m_stRegMutex)

/*****************************************************************************
    Func Name: Hal_CpuRegAccessInit
  Description: Init mutext for register accessing.
        Input: void  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void Hal_CpuRegAccessInit(void)
{
    REG_MUTEX_INIT; 

    return ;
}

/*****************************************************************************
    Func Name: Hal_CpuRegRead
  Description: Read value of register
        Input: UINT32 regAddr  
                UINT32 *regVal  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_CpuRegRead(UINT32 regAddr,UINT32 *regVal)
{
	if(NULL == regVal)
	{
		return DRV_ERR_PARA;
	}
	if(regAddr%4)
	{
		return DRV_ERR_PARA;
	}

    REG_MUTEX_LOCK;
	*regVal = *(volatile UINT32 *)(regAddr);	
    REG_MUTEX_UNLOCK;

	return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_CpuRegWrite
  Description: Write value to register
        Input: UINT32 regAddr  
                UINT32 regVal   
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_CpuRegWrite(UINT32 regAddr,UINT32 regVal)
{
	if(regAddr%4)
	{
		return DRV_ERR_PARA;
	}

    REG_MUTEX_LOCK;
	*(volatile UINT32 *)(regAddr) = regVal;
    REG_MUTEX_UNLOCK;
	
	return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GpioDirInSet
  Description: Set gpio direction input.
        Input: UINT32 uiPin  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GpioDirInSet(UINT32 uiPin)
{
	if(RT_ERR_OK != rtk_gpio_mode_set(uiPin, GPIO_INPUT))
    {
        return DRV_ERR_UNKNOW;
    }
	if(RT_ERR_OK != rtk_gpio_state_set(uiPin, ENABLED))
    {
        return DRV_ERR_UNKNOW;
    }

	return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GpioDirOutSet
  Description: Set gpio direction out.
        Input: UINT32 uiPin  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GpioDirOutSet(UINT32 uiPin)
{
	if(RT_ERR_OK != rtk_gpio_mode_set(uiPin, GPIO_OUTPUT))
    {
        return DRV_ERR_UNKNOW;
    }
	if(RT_ERR_OK != rtk_gpio_state_set(uiPin, ENABLED))
    {
        return DRV_ERR_UNKNOW;
    }

	return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GpioValGet
  Description: Get gpio value
        Input: UINT32 uiPin         
                UINT32 *puiRegValue  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GpioValGet(UINT32 uiPin, UINT32 *puiRegValue)
{
	if(RT_ERR_OK != rtk_gpio_databit_get(uiPin, puiRegValue))
    {
        return DRV_ERR_UNKNOW;
    }

	return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GpioOutputSet
  Description: Set gpio value
        Input: UINT32 uiPin       
                UINT32 uiRegValue  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GpioOutputSet(UINT32 uiPin, UINT32 uiRegValue)
{
	if(RT_ERR_OK != rtk_gpio_databit_set(uiPin, uiRegValue))
    {
        return DRV_ERR_UNKNOW;
    }

	return DRV_OK;
}
extern unsigned int g_uiIfPonIntChangeErr ;

//
DRV_RET_E Hal_EnablePonGpio(void)
{
	
	//rtk_gpio_state_set(35, ENABLED);
	rtk_gpio_intr_set(35,GPIO_INTR_ENABLE_FALLING_EDGE);
	
	return DRV_OK;
}

void __PonLaserIntr_Handler(void)
{
	g_uiIfPonIntChangeErr=TRUE;
//	printk("low init@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	//rtk_gpio_state_set(35, DISABLED);
	rtk_gpio_intr_set(35,GPIO_INTR_DISABLE);
}

DRV_RET_E __Hal_InitPonLaserMonior(VOID)
{
	
	Hal_GpioDirInSet(35);
	rtk_irq_gpioISR_register(35, __PonLaserIntr_Handler);
	rtk_gpio_intr_set(35,GPIO_INTR_ENABLE_FALLING_EDGE);
	
}



#ifdef  __cplusplus
}
#endif

