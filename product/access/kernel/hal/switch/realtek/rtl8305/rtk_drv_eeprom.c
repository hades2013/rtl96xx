/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
    extern "C"{
#endif
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include <linux/string.h>
#include <rtk/port.h>
#include <rtk/switch.h>
#include "hal_common.h"
#include <common/rt_error.h>
#include "drv/gpio/gpio.h"
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <if_pub.h>
#include "if_ethport.h"

#include <common/debug/rt_log.h>

typedef BOOL ACK_STATUS;

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/
extern int32 DrvPin_To_RtkPin(GPIO_PIN_E gpio_pin, gpioID *pGpioId);
BOOL ComboPortCfgChange(uint32 unit, uint32 phyPort, rtk_port_media_t enMedia);

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static void i2c_delay_us(uint32 us);
static void i2c_reset(gpioID GpioSclId, gpioID GpioSdaId);
static void i2c_init(gpioID GpioSclId, gpioID GpioSdaId);
static void i2c_start(gpioID GpioSclId, gpioID GpioSdaId);
static void i2c_repstart(gpioID GpioSclId, gpioID GpioSdaId);
static void i2c_stop(gpioID GpioSclId, gpioID GpioSdaId);
static void i2c_SendByte(uint8 data, gpioID GpioSclId, gpioID GpioSdaId);
static ACK_STATUS i2c_check_ACK(gpioID GpioSclId, gpioID GpioSdaId);
static uint8 i2c_RevByte(gpioID GpioSclId, gpioID GpioSdaId);
static void i2c_SendAck(gpioID GpioSclId, gpioID GpioSdaId);
static void i2c_SendNoAck(gpioID GpioSclId, gpioID GpioSdaId);

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/
BOOL g_bI2cFiberInitDone = FALSE;

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
/*lint -save -e86 -e133 -e43 -e651 -e155 -e69 -e110 -e24 -e40 -e63 -e35 -e785 -e10 -esym(552,if_lock) -e830 -e745 -e533*/
static spinlock_t _spin_hal_eeprom_lock = SPIN_LOCK_UNLOCKED;
/*lint -restore*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

static GPIO_PIN_E g_szI2cFiberGpioPin[LOGIC_PORT_NO+1][EEPROM_I2C_PIN_END] = {
/* 0 */    {GPIO_PIN_END, GPIO_PIN_END},
/* 1 */    {GPIO_PIN_END, GPIO_PIN_END},
/* 2 */    {GPIO_PIN_END, GPIO_PIN_END},
/* 3 */    {GPIO_PIN_END, GPIO_PIN_END},
/* 4 */    {GPIO_PIN_END, GPIO_PIN_END},
/* 5 */    {GPIO_PIN_END, GPIO_PIN_END},
/* 6 */    {GPIO_PIN_END, GPIO_PIN_END},
/* 7 */    {GPIO_PIN_END, GPIO_PIN_END},
/* 8 */    {GPIO_PIN_END, GPIO_PIN_END},
/* 9 */    {GPIO_PIN_END, GPIO_PIN_END},
/*10 */    {GPIO_PIN_END, GPIO_PIN_END},
/*11 */    {GPIO_PIN_END, GPIO_PIN_END},
/*12 */    {GPIO_PIN_END, GPIO_PIN_END},
/*13 */    {GPIO_PIN_END, GPIO_PIN_END},
/*14 */    {GPIO_PIN_END, GPIO_PIN_END},
/*15 */    {GPIO_PIN_END, GPIO_PIN_END},
/*16 */    {GPIO_PIN_END, GPIO_PIN_END},
/*17 */    {GPIO_PIN_END, GPIO_PIN_END},
/*18 */    {GPIO_PIN_END, GPIO_PIN_END},
/*19 */    {GPIO_PIN_END, GPIO_PIN_END},
/*20 */    {GPIO_PIN_END, GPIO_PIN_END},
/*21 */    {GPIO_PIN_END, GPIO_PIN_END},
/*22 */    {GPIO_PIN_END, GPIO_PIN_END},
/*23 */    {GPIO_PIN_END, GPIO_PIN_END},
/*24 */    {GPIO_PIN_END, GPIO_PIN_END},
/*25 */    {GPIO_PIN_SFP_SCL, GPIO_PIN_SFP0_SDA},
/*26 */    {GPIO_PIN_SFP_SCL, GPIO_PIN_SFP1_SDA}
};

/*----------------------------------------------*
 * 宏定义                                       *
*----------------------------------------------*/
#define SPIN_HAL_EEPROM_LOCK       spin_lock(&_spin_hal_eeprom_lock)
#define SPIN_HAL_EEPROM_UNLOCK     spin_unlock(&_spin_hal_eeprom_lock)


#define EEPROM_WRITE_ADDR 0xa0
#define EEPROM_READ_ADDR  0xa1
#define EEPROM_BYTE_SIZE  (2048) /*2K*/
//#define EEPROM_ADDR_MAX   (EEPROM_BYTE_SIZE-1) /*2K - 1*/
static uint8 g_auEepromBuf[EEPROM_BYTE_SIZE];

#define I2C_SPEED_SLOW_IO  100000  /* 100Khz */
#define EEPROM_I2C_SPEED   I2C_SPEED_SLOW_IO /*与原来产品一致*/

/* 这里的delay时间与实际的频率有偏差*/
#define I2C_DELAY_TIME_US         (1000000/EEPROM_I2C_SPEED)
#define I2C_DELAY_HALF_TIME_US    (I2C_DELAY_TIME_US/2)
#define I2C_DELAY_QUARTER_TIME_US (I2C_DELAY_TIME_US/4)
#define I2c_DELAY_SDA_INPUT_OUPUT_CHANGE_US (2)

#define EEPROM_RESET_CLOCK_NUM 9

#define I2C_GPIO_LOW  0
#define I2C_GPIO_HIGH 1

#define I2C_SCL_HIGH(__PIN__)   (drv_gpio_dataBit_set(__PIN__, I2C_GPIO_HIGH))
#define I2C_SCL_LOW(__PIN__)    (drv_gpio_dataBit_set(__PIN__, I2C_GPIO_LOW))

#define I2C_SDA_HIGH(__PIN__)   (drv_gpio_dataBit_set(__PIN__, I2C_GPIO_HIGH))
#define I2C_SDA_LOW(__PIN__)    (drv_gpio_dataBit_set(__PIN__, I2C_GPIO_LOW))
#define I2C_SDA_GET(__PIN__,__PDATA__)     (drv_gpio_dataBit_get(__PIN__, (__PDATA__)))
#define I2C_SDA_INPUT(__PIN__)  (drv_gpio_directionBit_set(__PIN__, GPIO_DIR_IN))
#define I2C_SDA_OUTPUT(__PIN__) (drv_gpio_directionBit_set(__PIN__, GPIO_DIR_OUT))

//#define EEPROM_DEBUG_SUPPORT
#ifdef EEPROM_DEBUG_SUPPORT
#define EEPROM_DEBUG rt_log_printf
#else
#define EEPROM_DEBUG(fmt,arg...)
#endif


/*******************************************************************************
* 函 数 名 : i2c_delay                                                                                                  
* 函数功能 : 用于I2C高低电平保持和建立等需要的延时（一般需要5uS）
* 入口参数 : us 微秒个数，例如果延时5uS 则传递参数 us = 5
* 返回参数 : NONE
*******************************************************************************/
static void i2c_delay_us(uint32 us)
{
    udelay(us);
    return;
} 


/*******************************************************************************
* 函 数 名 : i2c_reset                                                                                                  
* 函数功能 : I2C总线复位
* 入口参数 : NONE
* 返回参数 : NONE
*******************************************************************************/
static void i2c_reset(gpioID GpioSclId, gpioID GpioSdaId)
{
    uint8 tmp;
    for(tmp=0;tmp<EEPROM_RESET_CLOCK_NUM;tmp++)
    {
        I2C_SCL_LOW(GpioSclId);
        i2c_delay_us(I2C_DELAY_HALF_TIME_US);
        I2C_SCL_HIGH(GpioSclId);
        i2c_delay_us(I2C_DELAY_HALF_TIME_US);
    }
    
    /* scl, sda are high */
    I2C_SDA_HIGH(GpioSdaId);
    /*24cxx需要再start才能恢复*/
    i2c_start(GpioSclId, GpioSdaId);
    i2c_stop(GpioSclId, GpioSdaId);
    
    return;
}


/*******************************************************************************
* 函 数 名 : i2c_init                                                                                                  
* 函数功能 : I2C总线初始化
* 入口参数 : GpioSclId-I2C的clock, GpioSdaId--I2C的data
* 返回参数 : NONE
*******************************************************************************/
static void i2c_init(gpioID GpioSclId, gpioID GpioSdaId)
{
    
    (void)drv_gpio_init(GpioSclId, GPIO_CTRLFUNC_NORMAL, GPIO_DIR_OUT, GPIO_INT_DISABLE);
    (void)drv_gpio_init(GpioSdaId, GPIO_CTRLFUNC_NORMAL, GPIO_DIR_OUT, GPIO_INT_DISABLE);

    i2c_reset(GpioSclId, GpioSdaId);
    
    /* scl, sda are high */
    I2C_SCL_HIGH(GpioSclId);
    I2C_SDA_HIGH(GpioSdaId);

    return;
}

/*******************************************************************************
* 函 数 名 : i2c_start                                                                                                  
* 函数功能 : I2C总线启动时序
* 入口参数 : GpioSclId-I2C的clock, GpioSdaId--I2C的data
* 返回参数 : NONE
*******************************************************************************/
static void i2c_start(gpioID GpioSclId, gpioID GpioSdaId)
{
	/* assert: scl, sda are high */
    I2C_SDA_LOW(GpioSdaId);
	i2c_delay_us(I2C_DELAY_HALF_TIME_US);
    I2C_SCL_LOW(GpioSclId);
    i2c_delay_us(I2C_DELAY_HALF_TIME_US);

    return;
}


/*******************************************************************************
* 函 数 名 : i2c_repstart                                                                                                  
* 函数功能 : I2C总线重启动时序
* 入口参数 : GpioSclId-I2C的clock, GpioSdaId--I2C的data
* 返回参数 : NONE
*******************************************************************************/
static void i2c_repstart(gpioID GpioSclId, gpioID GpioSdaId)
{
	/* scl, sda may not be high */
	I2C_SDA_HIGH(GpioSdaId);
	I2C_SCL_HIGH(GpioSclId);
	i2c_delay_us(I2C_DELAY_HALF_TIME_US);
	I2C_SDA_LOW(GpioSdaId);
	i2c_delay_us(I2C_DELAY_HALF_TIME_US);
	I2C_SCL_LOW(GpioSclId);
	i2c_delay_us(I2C_DELAY_HALF_TIME_US);

	return;
}

/*******************************************************************************
* 函 数 名 : i2c_stop                                                                                                  
* 函数功能 : I2C总线停止时序
* 入口参数 : GpioSclId-I2C的clock, GpioSdaId--I2C的data
* 返回参数 : NONE
*******************************************************************************/
static void i2c_stop(gpioID GpioSclId, gpioID GpioSdaId)
{
	/* assert: scl is low */
	I2C_SDA_LOW(GpioSdaId);
	i2c_delay_us(I2C_DELAY_HALF_TIME_US);
	I2C_SCL_HIGH(GpioSclId); 
	i2c_delay_us(I2C_DELAY_HALF_TIME_US);
	I2C_SDA_HIGH(GpioSdaId);
	i2c_delay_us(I2C_DELAY_HALF_TIME_US);

	return;
}

/*******************************************************************************
* 函 数 名 : i2c_SendByte                                                                                                  
* 函数功能 : I2C发送一个字节
* 入口参数 : data 一字节数据, GpioSclId-I2C的clock, GpioSdaId--I2C的data
* 返回参数 : NONE
*******************************************************************************/
static void i2c_SendByte(uint8 data, gpioID GpioSclId, gpioID GpioSdaId)
{ 
    uint8 tmp;
    for(tmp=0;tmp<8;tmp++)
    {
        if(data & 0x80)
        {
          I2C_SDA_HIGH(GpioSdaId);
        }
        else
        {
          I2C_SDA_LOW(GpioSdaId);
        }
        i2c_delay_us(I2C_DELAY_QUARTER_TIME_US);
        
        I2C_SCL_HIGH(GpioSclId);
        i2c_delay_us(I2C_DELAY_HALF_TIME_US);
        I2C_SCL_LOW(GpioSclId);
        i2c_delay_us(I2C_DELAY_QUARTER_TIME_US);
        data <<= 1;
    }

    return;
}

/*******************************************************************************
* 函 数 名 : check_ACK                                                                                                  
* 函数功能 : 检测应答
* 入口参数 : GpioSclId-I2C的clock, GpioSdaId--I2C的data
* 返回参数 : NONE
*******************************************************************************/
static ACK_STATUS i2c_check_ACK(gpioID GpioSclId, gpioID GpioSdaId)
{ 
    ACK_STATUS AckStatus;
    uint32 PinData = 0;

    i2c_delay_us(I2c_DELAY_SDA_INPUT_OUPUT_CHANGE_US);
    I2C_SDA_INPUT(GpioSdaId);
    I2C_SCL_LOW(GpioSclId);
    i2c_delay_us(I2C_DELAY_QUARTER_TIME_US);
    I2C_SCL_HIGH(GpioSclId);
    i2c_delay_us(I2C_DELAY_QUARTER_TIME_US);
    /*读取数据在clock为高时的中间*/
    I2C_SDA_GET(GpioSdaId, &PinData);
    if(PinData)
    {
        AckStatus = FALSE;
    }
    else
    {
        AckStatus = TRUE;
    }
    I2C_SCL_HIGH(GpioSclId);
    i2c_delay_us(I2C_DELAY_QUARTER_TIME_US);
    
    I2C_SCL_LOW(GpioSclId);
    i2c_delay_us(I2C_DELAY_QUARTER_TIME_US);
    i2c_delay_us(I2c_DELAY_SDA_INPUT_OUPUT_CHANGE_US);
    I2C_SDA_OUTPUT(GpioSdaId); 
    
    return AckStatus;
}

/*******************************************************************************
* 函 数 名 : i2c_RevByte                                                                                                  
* 函数功能 : I2C接收一个字节
* 入口参数 : GpioSclId-I2C的clock, GpioSdaId--I2C的data
* 返回参数 : uint8 接收到的数据
*******************************************************************************/
static uint8 i2c_RevByte(gpioID GpioSclId, gpioID GpioSdaId)
{
    uint8 tmp = 0;
    uint8 data = 0;
    uint32 PinData = 0;

    i2c_delay_us(I2c_DELAY_SDA_INPUT_OUPUT_CHANGE_US);
    I2C_SDA_INPUT(GpioSdaId);
    for(tmp=0;tmp<8;tmp++)
    {
        I2C_SCL_LOW(GpioSclId);
        i2c_delay_us(I2C_DELAY_QUARTER_TIME_US);
        I2C_SCL_HIGH(GpioSclId);
        i2c_delay_us(I2C_DELAY_QUARTER_TIME_US);        
        data <<= 1;

        /*读取数据在clock为高时的中间*/
        I2C_SDA_GET(GpioSdaId, &PinData);
        if(PinData)
        {
          data |= 0x01; 
        }
        else
        {
          data &= 0xfe;
        }
        I2C_SCL_HIGH(GpioSclId);
        i2c_delay_us(I2C_DELAY_QUARTER_TIME_US);        
        I2C_SCL_LOW(GpioSclId);
        i2c_delay_us(I2C_DELAY_QUARTER_TIME_US);
    }
    i2c_delay_us(I2c_DELAY_SDA_INPUT_OUPUT_CHANGE_US);
    I2C_SDA_OUTPUT(GpioSdaId);
    
    return data;
}

/*******************************************************************************
* 函 数 名 : i2c_SendAck                                                                                                  
* 函数功能 : 主设备发送应答位
* 入口参数 : GpioSclId-I2C的clock, GpioSdaId--I2C的data
* 返回参数 : NONE
*******************************************************************************/
static void i2c_SendAck(gpioID GpioSclId, gpioID GpioSdaId)
{
    I2C_SDA_LOW(GpioSdaId);
    i2c_delay_us(I2C_DELAY_QUARTER_TIME_US);
    I2C_SCL_HIGH(GpioSclId);
    i2c_delay_us(I2C_DELAY_HALF_TIME_US);
    I2C_SCL_LOW(GpioSclId);
    i2c_delay_us(I2C_DELAY_QUARTER_TIME_US);
    
    return;
}

/*******************************************************************************
* 函 数 名 : i2c_SendNoAck                                                                                                  
* 函数功能 : 主设备发送非应答位
* 入口参数 : GpioSclId-I2C的clock, GpioSdaId--I2C的data
* 返回参数 : NONE
*******************************************************************************/
static void i2c_SendNoAck(gpioID GpioSclId, gpioID GpioSdaId)
{
    I2C_SDA_HIGH(GpioSdaId);
    i2c_delay_us(I2C_DELAY_QUARTER_TIME_US);
    I2C_SCL_HIGH(GpioSclId);
    i2c_delay_us(I2C_DELAY_HALF_TIME_US);
    I2C_SCL_LOW(GpioSclId);
    i2c_delay_us(I2C_DELAY_QUARTER_TIME_US);

    return;
}

/*目前只需要实现顺序读，其它的不支持*/
#if 0// EEPROM_SUPPORT_WRITE
/*******************************************************************************
* 函 数 名 : i2c_delay_ms                                                                                                  
* 函数功能 : 毫秒延时程序，完成一次写操作之后需要延时5MS用以内部写操作完成
* 入口参数 : ms 毫秒个数，例如果5ms 则传递参数 ms = 5
* 返回参数 : NONE
*******************************************************************************/
static void i2c_delay_ms(uint32 ms)
{
    udelay(ms*1000);
    return;
}

/*******************************************************************************
* 函 数 名 : EEPROM_ByteWrite                                                                                                  
* 函数功能 : EEPROM任意地址写一个字节数据
* 入口参数 : addr-AT24CXX内部地址，data-预写入的数据
* 返回参数 : 0-操作成功，非0-操作失败
*******************************************************************************/
static uint16 EEPROM_ByteWrite(uint16 addr,uint8 data, GPIO_PIN_E gpio_scl_pin, GPIO_PIN_E gpio_sda_pin)
{
    uint8 Dev_addr;               //设备地址
    uint8 AddrLow;
    uint8 AddrHigh;
    gpioID GpioSclId  = 0; 
    gpioID GpioSdaId  = 0; 
    
    if(addr > EEPROM_ADDR_MAX)            //检查预写入地址是否有效                      
    {
        return OutOfAddr;
    } 

    if(RT_ERR_OK != DrvPin_To_RtkPin(gpio_scl_pin, &GpioSclId))
    {
        return GpioPinError;
    }

    if(RT_ERR_OK != DrvPin_To_RtkPin(gpio_sda_pin, &GpioSdaId))
    {
        return GpioPinError;
    }
    
    AddrLow = (uint8)addr;
    AddrHigh = (uint8)(addr>>8);

    Dev_addr = EEPROM_WRITE_ADDR|(AddrHigh<<1);
    i2c_start(GpioSclId, GpioSdaId);
    i2c_SendByte(Dev_addr, GpioSclId, GpioSdaId);    
    if(i2c_check_ACK(GpioSclId, GpioSdaId) == FALSE)
    {
        return AckError;
    } 
    i2c_SendByte(AddrLow, GpioSclId, GpioSdaId); 
    if(i2c_check_ACK(GpioSclId, GpioSdaId) == FALSE)
    {
        return AckError;
    } 
    i2c_SendByte(data, GpioSclId, GpioSdaId);    
    if(i2c_check_ACK(GpioSclId, GpioSdaId) == FALSE)
    {
        return AckError;
    } 
    i2c_stop(GpioSclId, GpioSdaId);    
    i2c_delay_ms(10); 
    return EepromOk;            
}

/*******************************************************************************
* 函 数 名 : EEPROM_PageWrite                                                                                                  
* 函数功能 : 从任意页开始连写N字节，但不可以跨页
* 入口参数 : page-页号，n-写n字节数据
* 返回参数 : 0-操作成功，非0-操作失败
*******************************************************************************/
static uint16 EEPROM_PageWrite(uint8 page,uint8* p,uint8 n, GPIO_PIN_E gpio_scl_pin, GPIO_PIN_E gpio_sda_pin)
{
    uint8 Dev_addr;                 //设备地址
    uint8 AddrLow;
    uint8 AddrHigh;
    uint16 tmp;
    gpioID GpioSclId  = 0; 
    gpioID GpioSdaId  = 0; 
    
    if(NULL == p)
    {
        return PointError;
    }

    if(RT_ERR_OK != DrvPin_To_RtkPin(gpio_scl_pin, &GpioSclId))
    {
        return GpioPinError;
    }

    if(RT_ERR_OK != DrvPin_To_RtkPin(gpio_sda_pin, &GpioSdaId))
    {
        return GpioPinError;
    }
    
    if((n > 16)|(page > 128))       //根据读写的设备而变更为适合的页数和每页字节数
    {
        return OutOfRang;
    }
    tmp = ((uint16)page) << 4;      //得出页首地址
    AddrLow = (uint8)tmp;
    AddrHigh = (uint8)(tmp>>8);
    Dev_addr = EEPROM_WRITE_ADDR | (AddrHigh << 1);
    i2c_start(GpioSclId, GpioSdaId);
    i2c_SendByte(Dev_addr, GpioSclId, GpioSdaId);    
    if(i2c_check_ACK(GpioSclId, GpioSdaId) == FALSE)
    {
        return AckError;
    } 
    i2c_delay_ms(5);
    i2c_SendByte(AddrLow, GpioSclId, GpioSdaId); 
    if(i2c_check_ACK(GpioSclId, GpioSdaId) == FALSE)
    {
        return AckError;
    } 
    while(n--)
    {
        i2c_SendByte(*p++, GpioSclId, GpioSdaId);
        if(i2c_check_ACK(GpioSclId, GpioSdaId) == FALSE)
        {
          return AckError;
        } 
    }
    i2c_stop(GpioSclId, GpioSdaId); 
    i2c_delay_ms(10);
    return EepromOk;
}

/*******************************************************************************
* 函 数 名 : EEPROM_RandomRead                                                                                                  
* 函数功能 : EEPROM任意地址读取一个字节
* 入口参数 : addr预读取的地址
* 返回参数 : 读取的数据
*******************************************************************************/
static uint8 EEPROM_RandomRead(uint16 addr, uint8 *p, GPIO_PIN_E gpio_scl_pin, GPIO_PIN_E gpio_sda_pin)
{
    uint8 Dev_addr;               //设备地址
    uint8 AddrLow;
    uint8 AddrHigh;
    uint8 tmp;
    gpioID GpioSclId  = 0; 
    gpioID GpioSdaId  = 0; 
    
    if(NULL == p)
    {
        return PointError;
    }
    
    if(addr > EEPROM_ADDR_MAX)            //检查预写入地址是否有效                      
    {
        return OutOfAddr;
    } 

    if(RT_ERR_OK != DrvPin_To_RtkPin(gpio_scl_pin, &GpioSclId))
    {
        return GpioPinError;
    }

    if(RT_ERR_OK != DrvPin_To_RtkPin(gpio_sda_pin, &GpioSdaId))
    {
        return GpioPinError;
    }
    
    AddrLow  = (uint8)addr;
    AddrHigh = (uint8)(addr>>8);

    Dev_addr = EEPROM_WRITE_ADDR|(AddrHigh<<1);
    i2c_start(GpioSclId, GpioSdaId);
    i2c_SendByte(Dev_addr, GpioSclId, GpioSdaId);    
    if(i2c_check_ACK(GpioSclId, GpioSdaId) == FALSE)
    {
        return AckError;
    } 
    i2c_delay_ms(5);
    i2c_SendByte(AddrLow, GpioSclId, GpioSdaId); 
    if(i2c_check_ACK(GpioSclId, GpioSdaId) == FALSE)
    {
        return AckError;
    } 
    i2c_delay_ms(5);
    i2c_start(GpioSclId, GpioSdaId);
    Dev_addr = EEPROM_READ_ADDR|(AddrHigh<<1);
    i2c_SendByte(Dev_addr, GpioSclId, GpioSdaId);    
    if(i2c_check_ACK(GpioSclId, GpioSdaId) == FALSE)
    {
        return AckError;
    } 
    i2c_delay_ms(5);
    *p = i2c_RevByte(GpioSclId, GpioSdaId);
    i2c_SendNoAck(GpioSclId, GpioSdaId);
    i2c_stop(GpioSclId, GpioSdaId);
    i2c_delay_us(100); 
    
    return EepromOk;
}
#endif

/*******************************************************************************
* 函 数 名 : EEPROM_SequentialRead                                                                                                  
* 函数功能 : EEPROM从任意地址连续读取N个字节
* 入口参数 : addr-预读取的地址，n-预读取的字节数，p-读出数据存放指针,
             gpio_scl_pin-I2C的clock, gpio_sda_pin--I2C的data
* 返回参数 : n 预读取的字节个数
*******************************************************************************/
static uint16 EEPROM_SequentialRead(uint16 addr,uint16 n,uint8* p, GPIO_PIN_E gpio_scl_pin, GPIO_PIN_E gpio_sda_pin)
{
    uint8 Dev_addr = 0;   //设备地址
    uint8 AddrLow  = 0;
    uint8 AddrHigh = 0;
    gpioID GpioSclId  = 0; 
    gpioID GpioSdaId  = 0; 
    EEPROM_RET ret = EepromOk;

    if(NULL == p)
    {
        return (uint16)PointError;
    }

     /*检查预写入地址是否有效*/
    if(n > (EEPROM_BYTE_SIZE - addr))             
    {
        return (uint16)OutOfAddr;
    }

    if(RT_ERR_OK != DrvPin_To_RtkPin(gpio_scl_pin, &GpioSclId))
    {
        return (uint16)GpioPinError;
    }

    if(RT_ERR_OK != DrvPin_To_RtkPin(gpio_sda_pin, &GpioSdaId))
    {
        return (uint16)GpioPinError;
    }

    EEPROM_DEBUG("%s, %d,GpioSclId=0x%x, GpioSdaId=0x%x\n",__FUNCTION__,__LINE__,GpioSclId,GpioSdaId);

    AddrLow  = (uint8)addr;
    AddrHigh = (uint8)(addr>>8);

    Dev_addr = EEPROM_WRITE_ADDR|((uint8)(AddrHigh<<1));
    i2c_start(GpioSclId, GpioSdaId);
    i2c_SendByte(Dev_addr, GpioSclId, GpioSdaId);    
    if(i2c_check_ACK(GpioSclId, GpioSdaId) == FALSE)
    {
        EEPROM_DEBUG("i2c_check_ACK fail, %s, %d\n",__FUNCTION__,__LINE__);
        ret = AckError;
        goto EEPROM_SReadEnd;
    } 
    
    i2c_SendByte(AddrLow, GpioSclId, GpioSdaId); 
    if(i2c_check_ACK(GpioSclId, GpioSdaId) == FALSE)
    {
        EEPROM_DEBUG("i2c_check_ACK fail, %s, %d\n",__FUNCTION__,__LINE__);
        ret = AckError;
        goto EEPROM_SReadEnd;
    } 
    
    i2c_repstart(GpioSclId, GpioSdaId);
    Dev_addr = EEPROM_READ_ADDR|((uint8)(AddrHigh<<1));
    i2c_SendByte(Dev_addr, GpioSclId, GpioSdaId);    
    if(i2c_check_ACK(GpioSclId, GpioSdaId) == FALSE)
    {
        EEPROM_DEBUG("i2c_check_ACK fail, %s, %d\n",__FUNCTION__,__LINE__);
        ret = AckError;
        goto EEPROM_SReadEnd;
    } 

    while(n--)
    {
        *p = i2c_RevByte(GpioSclId, GpioSdaId);
        p++;
        if(n)
          i2c_SendAck(GpioSclId, GpioSdaId);
        else
          i2c_SendNoAck(GpioSclId, GpioSdaId);
    }

    ret = EepromOk;
    
EEPROM_SReadEnd:
    
    i2c_stop(GpioSclId, GpioSdaId); 

    return ret; 
}

/*******************************************************************************
* 函 数 名 : EEPROM_Reset                                                                                                  
* 函数功能 : EEPROM I2C异常复位处理函数
* 入口参数 : gpio_scl_pin-I2C的clock, gpio_sda_pin--I2C的data
* 返回参数 : 
*******************************************************************************/
static uint16 EEPROM_Reset(GPIO_PIN_E gpio_scl_pin, GPIO_PIN_E gpio_sda_pin)
{
    gpioID GpioSclId  = 0; 
    gpioID GpioSdaId  = 0; 

    if(RT_ERR_OK != DrvPin_To_RtkPin(gpio_scl_pin, &GpioSclId))
    {
        return GpioPinError;
    }

    if(RT_ERR_OK != DrvPin_To_RtkPin(gpio_sda_pin, &GpioSdaId))
    {
        return GpioPinError;
    }

    i2c_reset(GpioSclId, GpioSdaId);

    return EepromOk;
}

/*****************************************************************************
    Func Name: LogicPort_To_I2cDrvPin
  Description: 从输入的端口号获取对应端口的I2C管脚
        Input: port_num_t lgcPort
       Output: GPIO_PIN_E *p_gpio_scl_pin
               GPIO_PIN_E *p_gpio_sda_pin
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static int32 LogicPort_To_I2cDrvPin(port_num_t lgcPort, GPIO_PIN_E *p_gpio_scl_pin, GPIO_PIN_E *p_gpio_sda_pin)
{
    if((NULL == p_gpio_scl_pin) || (NULL == p_gpio_sda_pin))
    {
        return RT_ERR_INPUT;
    }
    
    if(!IsValidLgcPort(lgcPort))
    {
        return RT_ERR_INPUT;
    }
    
    *p_gpio_scl_pin = g_szI2cFiberGpioPin[lgcPort][EEPROM_I2C_SCL_PIN];
    *p_gpio_sda_pin = g_szI2cFiberGpioPin[lgcPort][EEPROM_I2C_SDA_PIN];
    
    return RT_ERR_OK;
}

/*****************************************************************************
    Func Name: LogicPort_To_I2cDrvPin
  Description: 读取光模块的信息
        Input: port_num_t lgcPort -- 端口号
               uint16 ucAddr      -- 读取信息的起始地址
               uint32 ulLen       -- 读取信息的长度
               uint32 ulPDataType -- 地址空间(ADDRESS_IN_USER_SPACE／ADDRESS_IN_KERNEL_SPACE)
       Output: uint8* pucData     -- 存放读取到的信息
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_I2cFiberPortRead(port_num_t lgcPort, uint16 usAddr, uint8* pucData, uint32 ulLen, uint32 ulPDataType)
{
    GPIO_PIN_E gpio_scl_pin = GPIO_PIN_END; 
    GPIO_PIN_E gpio_sda_pin = GPIO_PIN_END;
    
    EEPROM_DEBUG("%s, %d, lgcPort=%d\n",__FUNCTION__,__LINE__,lgcPort);
    if(NULL == pucData)
    {
        return DRV_ERR_PARA;
    }

    if(!IS_COMBO_PORT(lgcPort) && !IS_FIBER_PORT(lgcPort))
    {
        return DRV_ERR_PARA;
    }

    /*I2C初始化*/
    if(FALSE == g_bI2cFiberInitDone)
    {
        EEPROM_DEBUG("i2c not init, %s, %d\n",__FUNCTION__,__LINE__);
        return DRV_SDK_ERR_EEPROM_I2C_NOT_INIT;
    }

    
    if(RT_ERR_OK != LogicPort_To_I2cDrvPin(lgcPort, &gpio_scl_pin, &gpio_sda_pin))
    {
        return DRV_ERR_PARA;
    }

    SPIN_HAL_EEPROM_LOCK;
    memset(g_auEepromBuf, 0, EEPROM_BYTE_SIZE);
    
    if(EepromOk != EEPROM_SequentialRead(usAddr, (uint16)ulLen, g_auEepromBuf, gpio_scl_pin, gpio_sda_pin))
    {
        EEPROM_DEBUG("EEPROM_SequentialRead fail, %s, %d\n",__FUNCTION__,__LINE__);
        /*读取失败，则认为是I2C异常，需要复位*/
        if(EepromOk != EEPROM_Reset(gpio_scl_pin, gpio_sda_pin))
        {
            EEPROM_DEBUG("EEPROM_Reset fail, %s, %d\n",__FUNCTION__,__LINE__);
            SPIN_HAL_EEPROM_UNLOCK;
            return DRV_SDK_ERR_EEPROM_I2C_READ;
        }
        else
        {
            EEPROM_DEBUG("EEPROM_Reset fail, %s, %d\n",__FUNCTION__,__LINE__);
            /*重新读取*/
            if(EepromOk != EEPROM_SequentialRead(usAddr, (uint16)ulLen, g_auEepromBuf, gpio_scl_pin, gpio_sda_pin))
            {
                EEPROM_DEBUG("EEPROM_SequentialRead fail, %s, %d\n",__FUNCTION__,__LINE__);
                SPIN_HAL_EEPROM_UNLOCK;
                return DRV_SDK_ERR_EEPROM_I2C_READ;
            }
        }
    }

    if(ADDRESS_IN_USER_SPACE == ulPDataType)
    {
        /*copy to user memory space*/
        (void)copy_to_user(pucData, g_auEepromBuf, ulLen);
    }
    else
    {
        (void)memcpy(pucData, g_auEepromBuf, ulLen);
    }
    SPIN_HAL_EEPROM_UNLOCK;
    
    #ifdef EEPROM_DEBUG_SUPPORT
    int i = 0;
    EEPROM_DEBUG("###################################%s, %d\n",__FUNCTION__,__LINE__);
    for(i=0; i < ulLen; i++)
    {
        EEPROM_DEBUG("%2x ",*(pucData+i));
    }
    EEPROM_DEBUG("###################################%s, %d\n",__FUNCTION__,__LINE__);
    #endif

    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_I2cFiberInit
  Description: I2C管脚初始化函数
        Input: 
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_I2cFiberInit(VOID)
{
    port_num_t uiPort;
    GPIO_PIN_E gpio_scl_pin = GPIO_PIN_END; 
    GPIO_PIN_E gpio_sda_pin = GPIO_PIN_END;
    gpioID GpioSclId  = 0; 
    gpioID GpioSdaId  = 0; 

    LgcPortFor(uiPort)
    {
        if(IS_COMBO_PORT(uiPort) || IS_FIBER_PORT(uiPort))
        {
            if(RT_ERR_OK == LogicPort_To_I2cDrvPin(uiPort, &gpio_scl_pin, &gpio_sda_pin))
            {
                (VOID)DrvPin_To_RtkPin(gpio_scl_pin, &GpioSclId);
                (VOID)DrvPin_To_RtkPin(gpio_sda_pin, &GpioSdaId);
                i2c_init(GpioSclId, GpioSdaId);
            }
        }
    }
    
    g_bI2cFiberInitDone = TRUE;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: FiberInfoWriteToDram
  Description: 光模块的信息写入到内存中
        Input: port_num_t lgcPort -- 端口号
               uint8* pucData     -- 存放写入的信息
               uint32 ulLen       -- 写入信息的长度
               BOOL bValidFlag    -- 用于标志写入的信息是否有效
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
BOOL FiberInfoWriteToDram(port_num_t lgcPort, uint8* pucData, uint32 ulLen, BOOL bValidFlag)
{
    ifindex_t uiIfindex=IF_INVALID_IFINDEX;
    FIBERINFOFLAG_E stFlag = FIBER_INFO_INVALID;

    if(!IsValidLgcPort(lgcPort))
    {
        return FALSE;
    }

    uiIfindex = IF_IFINDEX(IF_SUB_ETHPORT, lgcPort);
    
    /*先将flag设置成invalid*/
    stFlag = FIBER_INFO_INVALID;
    if(IF_OK != K_IF_SetAttr(uiIfindex, IF_FIBER_INFO_FLAG, &stFlag, sizeof(FIBERINFOFLAG_E)))
    {
        return FALSE;
    }

    /*错误信息只要设置flag为invalid，不需要清除信息*/
    if(FALSE == bValidFlag)
    {
        return TRUE;
    }

    if(NULL == pucData)
    {
        return FALSE;
    }
    
    /*更新信息*/
    if(IF_OK != K_IF_SetAttr(uiIfindex, IF_FIBER_INFO, pucData, ulLen))
    {
        return FALSE;
    }

    /*将flag设置成valid*/
    stFlag = FIBER_INFO_VALID;
    if(IF_OK != K_IF_SetAttr(uiIfindex, IF_FIBER_INFO_FLAG, &stFlag, sizeof(FIBERINFOFLAG_E)))
    {
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************************
    Func Name: FiberInfoUpdate
  Description: 更新保存在内存中光模块的信息
        Input: uint32 unit        -- 设备号
               uint32 phyPort     -- 物理端口号
               BOOL bValidFlag    -- 用于标志写入的信息是否有效
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
BOOL FiberInfoUpdate(uint32 unit, uint32 phyPort, BOOL bValidFlag)
{
    port_num_t lgcPort;
    UCHAR * pucTemp = NULL;

    
    lgcPort = PortPhyID2Logic(phyPort);

    if(!IsValidLgcPort(lgcPort))
    {
        return FALSE;
    }
    
    if(FALSE == bValidFlag)
    {
        if(TRUE != FiberInfoWriteToDram(lgcPort, NULL, 0, bValidFlag))
        {
            return FALSE;
        }
    }
    else
    {
        pucTemp = (UCHAR *)kmalloc(MAX_TRANSCEIVER_CODE_LEN,GFP_KERNEL);
        if(NULL == pucTemp)
        {
            return FALSE;
        }
        memset(pucTemp, 0, MAX_TRANSCEIVER_CODE_LEN);
        
        /*读取信息*/
        if(DRV_OK != Hal_I2cFiberPortRead(lgcPort, I2C_START, pucTemp, MAX_TRANSCEIVER_CODE_LEN, ADDRESS_IN_KERNEL_SPACE))
        {
            kfree(pucTemp);
            return FALSE;
        }

        /*更新信息*/
        if(TRUE != FiberInfoWriteToDram(lgcPort, pucTemp, MAX_TRANSCEIVER_CODE_LEN, bValidFlag))
        {
            kfree(pucTemp);
            return FALSE;
        }

        kfree(pucTemp);
    }

    /*更新光口的速率*/
    (void)ComboPortCfgChange(unit, phyPort, PORT_MEDIA_FIBER);
    return TRUE;
}

#ifdef  __cplusplus
}
#endif

