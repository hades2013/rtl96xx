/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif

#include <linux/mutex.h>
#include "opl_drv.h"
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_config.h"

/*----------------------------------------------*
 * extern function                              *
 *----------------------------------------------*/
extern DRV_RET_E Hal_CpuRegRead(UINT32 regAddr,UINT32 *regVal);
extern DRV_RET_E Hal_CpuRegWrite(UINT32 regAddr,UINT32 regVal);

/*----------------------------------------------*
 * internal function                              *
 *----------------------------------------------*/
 
/*----------------------------------------------*
 * global parameter                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module parameter                             *
 *----------------------------------------------*/
struct mutex m_stGpioMutex;
/*----------------------------------------------*
 * macro                                        *
 *----------------------------------------------*/
#define REG_GPIO_BASEADDR        (0x0B20)
#define REG_GPIO_VAL             ((REG_GPIO_BASEADDR + 0)*4)
#define REG_GPIO_DIR             ((REG_GPIO_BASEADDR + 1)*4)
#define REG_GPIO_SET             ((REG_GPIO_BASEADDR + 2)*4)
#define REG_GPIO_CLR             ((REG_GPIO_BASEADDR + 3)*4)
#define REG_GPIO_TRG             ((REG_GPIO_BASEADDR + 6)*4)
#define REG_GPIO_LVL             ((REG_GPIO_BASEADDR + 7)*4)
#define REG_GPIO_ADG             ((REG_GPIO_BASEADDR + 8)*4)
#define REG_GPIO_MLT             ((REG_GPIO_BASEADDR + 9)*4)


//#define GPIO_MUTEX_CREATE()   DEFINE_MUTEX(m_stGpioMutex)
#define GPIO_MUTEX_INIT     mutex_init(&m_stGpioMutex)
#define GPIO_MUTEX_LOCK     mutex_lock(&m_stGpioMutex)
#define GPIO_MUTEX_UNLOCK   mutex_unlock(&m_stGpioMutex)

/*****************************************************************************
    Func Name: Hal_GpioInit
  Description: Initiate gpio
        Input: void  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
void Hal_GpioInit(void)
{
    GPIO_MUTEX_INIT; 


    return;
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
    UINT32 uiRegVal;
    
    GPIO_MUTEX_LOCK;
    (void)Hal_CpuRegRead(REG_GPIO_DIR, &uiRegVal);
    uiRegVal &= ~(1U << uiPin);
    (void)Hal_CpuRegWrite(REG_GPIO_DIR, uiRegVal);
    GPIO_MUTEX_UNLOCK;

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
    UINT32 uiRegVal;
    
    GPIO_MUTEX_LOCK;
    (void)Hal_CpuRegRead(REG_GPIO_DIR, &uiRegVal);
    uiRegVal |= 1U << uiPin;
    (void)Hal_CpuRegWrite(REG_GPIO_DIR, uiRegVal);
    GPIO_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GpioTrigModeLevelSet
  Description: Set level trig mode
        Input: UINT32 uiPin  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GpioTrigModeLevelSet(UINT32 uiPin)
{
    UINT32 uiRegVal;
    
    GPIO_MUTEX_LOCK;
    (void)Hal_CpuRegRead(REG_GPIO_TRG, &uiRegVal);
    uiRegVal |= 1U << uiPin;
    (void)Hal_CpuRegWrite(REG_GPIO_TRG, uiRegVal);
    GPIO_MUTEX_UNLOCK;
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GpioTrigModeEdgeSet
  Description: Set edge trig mode.
        Input: UINT32 uiPin  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GpioTrigModeEdgeSet(UINT32 uiPin)
{
    UINT32 uiRegVal;
    
    GPIO_MUTEX_LOCK;
    (void)Hal_CpuRegRead(REG_GPIO_TRG, &uiRegVal);
    uiRegVal &= ~(1U << uiPin);
    (void)Hal_CpuRegWrite(REG_GPIO_TRG, uiRegVal);
    GPIO_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GpioIntLevelHighSet
  Description: Set high level trig
        Input: UINT32 uiPin  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GpioIntLevelHighSet(UINT32 uiPin)
{
    UINT32 uiRegVal;
    
    GPIO_MUTEX_LOCK;
    (void)Hal_CpuRegRead(REG_GPIO_LVL, &uiRegVal);
    uiRegVal |= 1U << uiPin;
    (void)Hal_CpuRegWrite(REG_GPIO_LVL, uiRegVal);
    GPIO_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GpioIntLevelLowSet
  Description: Set low level trig
        Input: UINT32 uiPin  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GpioIntLevelLowSet(UINT32 uiPin)
{
    UINT32 uiRegVal;
    
    GPIO_MUTEX_LOCK;
    (void)Hal_CpuRegRead(REG_GPIO_LVL, &uiRegVal);
    uiRegVal &= ~(1U << uiPin);
    (void)Hal_CpuRegWrite(REG_GPIO_LVL, uiRegVal);
    GPIO_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GpioIntEdgeSet
  Description: Set edge trig mode.
        Input: UINT32 uiPin       
                UINT32 uiEdgeType :
                                   0x00: invalid
                                   0x01: triggered on rising edge
                                   0x10: triggered on falling edge
                                   0x11: triggered on both edge
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GpioIntEdgeSet(UINT32 uiPin, UINT32 uiEdgeType)
{
    UINT32 uiRegVal;

    if (0x03 < uiEdgeType)
    {
        return DRV_ERR_PARA;
    }
    
    GPIO_MUTEX_LOCK;
    (void)Hal_CpuRegRead(REG_GPIO_ADG, &uiRegVal);
    /*Clean edge mode first*/
    uiRegVal &= ~(0x11U << (uiPin*2));

    /*Then set edge mode*/
    uiRegVal |= uiEdgeType << (uiPin*2);
    (void)Hal_CpuRegWrite(REG_GPIO_ADG, uiRegVal);
    GPIO_MUTEX_UNLOCK;

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
	UINT32 uiRegValTmp;

    if (NULL == puiRegValue)
    {
        return DRV_ERR_PARA;
    }

    GPIO_MUTEX_LOCK;
    (void)Hal_CpuRegRead(REG_GPIO_VAL, &uiRegValTmp);
    *puiRegValue = (uiRegValTmp & (1U << uiPin)) >> uiPin;
    GPIO_MUTEX_UNLOCK;

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
    UINT32 uiRegValTmp = 0;

    GPIO_MUTEX_LOCK;
    
    (void)Hal_CpuRegRead(REG_GPIO_SET, &uiRegValTmp);
    
	if (uiRegValue) 
	{
		uiRegValTmp |= 1U << uiPin;
        (void)Hal_CpuRegWrite(REG_GPIO_SET, uiRegValTmp);
	}
	else
	{
		uiRegValTmp &= ~(1U << uiPin);
        (void)Hal_CpuRegWrite(REG_GPIO_SET, uiRegValTmp);
        
        (void)Hal_CpuRegRead(REG_GPIO_CLR, &uiRegValTmp);
        uiRegValTmp |= 1U << uiPin;
		(void)Hal_CpuRegWrite(REG_GPIO_CLR, uiRegValTmp);
	}

    GPIO_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GpioMultiplexSet
  Description: Set gpio multiplex mode.
        Input: UINT32 uiPin       
                UINT32 uiWorkMode :
                                   0:work as gpio mode
                                   1:work as other functionality
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GpioMultiplexSet(UINT32 uiPin, UINT32 uiWorkMode)
{
    UINT32 uiRegValTmp = 0;

    if (1 < uiWorkMode)
    {
        return DRV_ERR_PARA;
    }
    
    GPIO_MUTEX_LOCK;
    (void)Hal_CpuRegRead(REG_GPIO_MLT, &uiRegValTmp);

    switch (uiPin)
    {
        /*gpio 0/1/2 share with spi*/
        case 0:
        case 1:
        case 2:
            uiRegValTmp &= ~(1U << 2);
            uiRegValTmp |= uiWorkMode << 2;
            break;
        /*gpio 3 share with ss0*/
        case 3:
            uiRegValTmp &= ~(1U << 3);
            uiRegValTmp |= uiWorkMode << 3;
            break;
        /*gpio 4/5 share with UART1*/
        case 4:
        case 5:
            uiRegValTmp &= ~(1U << 1);
            uiRegValTmp |= uiWorkMode << 1;
            break;
        /*gpio 6/7 share with IIC*/
        case 6:
        case 7:
            uiRegValTmp &= ~(1U);
            uiRegValTmp |= uiWorkMode;
            break;
        /*gpio 8 share with ss1*/
        case 8:
            uiRegValTmp &= ~(1U << 4);
            uiRegValTmp |= uiWorkMode << 4;
            break;
        /*gpio 9 share with ss2*/
        case 9:
            uiRegValTmp &= ~(1U << 5);
            uiRegValTmp |= uiWorkMode << 5;
            break;
        default :
            GPIO_MUTEX_UNLOCK;
            return DRV_ERR_PARA;
    }
	
    (void)Hal_CpuRegWrite(REG_GPIO_MLT, uiRegValTmp);
    GPIO_MUTEX_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GpioMultiplexGet
  Description: Get gpio multiplex mode.
        Input: UINT32 uiPin       
       Output: 
               UINT32 * uiWorkMode :
                                   0:work as gpio mode
                                   1:work as other functionality
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GpioMultiplexGet(UINT32 uiPin, UINT32 * puiWorkMode)
{
    UINT32 uiRegValTmp = 0;

    if (NULL == puiWorkMode)
    {
        return DRV_ERR_PARA;
    }
    
    GPIO_MUTEX_LOCK;
    (void)Hal_CpuRegRead(REG_GPIO_MLT, &uiRegValTmp);
    GPIO_MUTEX_UNLOCK;

    switch (uiPin)
    {
        /*gpio 0/1/2 share with spi*/
        case 0:
        case 1:
        case 2:
            *puiWorkMode = (uiRegValTmp & (1U << 2)) >> 2;
            break;
        /*gpio 3 share with ss0*/
        case 3:
            *puiWorkMode = (uiRegValTmp & (1U << 3)) >> 3;
            break;
        /*gpio 4/5 share with UART1*/
        case 4:
        case 5:
            *puiWorkMode = (uiRegValTmp & (1U << 1)) >> 1;
            break;
        /*gpio 6/7 share with IIC*/
        case 6:
        case 7:
            *puiWorkMode = uiRegValTmp & 1U;
            break;
        /*gpio 8 share with ss1*/
        case 8:
            *puiWorkMode = (uiRegValTmp & (1U << 4)) >> 4;
            break;
        /*gpio 9 share with ss2*/
        case 9:
            *puiWorkMode = (uiRegValTmp & (1U << 5)) >> 5;
            break;
        default :
            return DRV_ERR_PARA;
    }

    return DRV_OK;
}

#ifdef  __cplusplus
}
#endif

