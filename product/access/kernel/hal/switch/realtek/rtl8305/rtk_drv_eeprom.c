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
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/
extern int32 DrvPin_To_RtkPin(GPIO_PIN_E gpio_pin, gpioID *pGpioId);
BOOL ComboPortCfgChange(uint32 unit, uint32 phyPort, rtk_port_media_t enMedia);

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
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
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/
BOOL g_bI2cFiberInitDone = FALSE;

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
/*lint -save -e86 -e133 -e43 -e651 -e155 -e69 -e110 -e24 -e40 -e63 -e35 -e785 -e10 -esym(552,if_lock) -e830 -e745 -e533*/
static spinlock_t _spin_hal_eeprom_lock = SPIN_LOCK_UNLOCKED;
/*lint -restore*/

/*----------------------------------------------*
 * ��������                                     *
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
 * �궨��                                       *
*----------------------------------------------*/
#define SPIN_HAL_EEPROM_LOCK       spin_lock(&_spin_hal_eeprom_lock)
#define SPIN_HAL_EEPROM_UNLOCK     spin_unlock(&_spin_hal_eeprom_lock)


#define EEPROM_WRITE_ADDR 0xa0
#define EEPROM_READ_ADDR  0xa1
#define EEPROM_BYTE_SIZE  (2048) /*2K*/
//#define EEPROM_ADDR_MAX   (EEPROM_BYTE_SIZE-1) /*2K - 1*/
static uint8 g_auEepromBuf[EEPROM_BYTE_SIZE];

#define I2C_SPEED_SLOW_IO  100000  /* 100Khz */
#define EEPROM_I2C_SPEED   I2C_SPEED_SLOW_IO /*��ԭ����Ʒһ��*/

/* �����delayʱ����ʵ�ʵ�Ƶ����ƫ��*/
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
* �� �� �� : i2c_delay                                                                                                  
* �������� : ����I2C�ߵ͵�ƽ���ֺͽ�������Ҫ����ʱ��һ����Ҫ5uS��
* ��ڲ��� : us ΢��������������ʱ5uS �򴫵ݲ��� us = 5
* ���ز��� : NONE
*******************************************************************************/
static void i2c_delay_us(uint32 us)
{
    udelay(us);
    return;
} 


/*******************************************************************************
* �� �� �� : i2c_reset                                                                                                  
* �������� : I2C���߸�λ
* ��ڲ��� : NONE
* ���ز��� : NONE
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
    /*24cxx��Ҫ��start���ָܻ�*/
    i2c_start(GpioSclId, GpioSdaId);
    i2c_stop(GpioSclId, GpioSdaId);
    
    return;
}


/*******************************************************************************
* �� �� �� : i2c_init                                                                                                  
* �������� : I2C���߳�ʼ��
* ��ڲ��� : GpioSclId-I2C��clock, GpioSdaId--I2C��data
* ���ز��� : NONE
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
* �� �� �� : i2c_start                                                                                                  
* �������� : I2C��������ʱ��
* ��ڲ��� : GpioSclId-I2C��clock, GpioSdaId--I2C��data
* ���ز��� : NONE
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
* �� �� �� : i2c_repstart                                                                                                  
* �������� : I2C����������ʱ��
* ��ڲ��� : GpioSclId-I2C��clock, GpioSdaId--I2C��data
* ���ز��� : NONE
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
* �� �� �� : i2c_stop                                                                                                  
* �������� : I2C����ֹͣʱ��
* ��ڲ��� : GpioSclId-I2C��clock, GpioSdaId--I2C��data
* ���ز��� : NONE
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
* �� �� �� : i2c_SendByte                                                                                                  
* �������� : I2C����һ���ֽ�
* ��ڲ��� : data һ�ֽ�����, GpioSclId-I2C��clock, GpioSdaId--I2C��data
* ���ز��� : NONE
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
* �� �� �� : check_ACK                                                                                                  
* �������� : ���Ӧ��
* ��ڲ��� : GpioSclId-I2C��clock, GpioSdaId--I2C��data
* ���ز��� : NONE
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
    /*��ȡ������clockΪ��ʱ���м�*/
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
* �� �� �� : i2c_RevByte                                                                                                  
* �������� : I2C����һ���ֽ�
* ��ڲ��� : GpioSclId-I2C��clock, GpioSdaId--I2C��data
* ���ز��� : uint8 ���յ�������
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

        /*��ȡ������clockΪ��ʱ���м�*/
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
* �� �� �� : i2c_SendAck                                                                                                  
* �������� : ���豸����Ӧ��λ
* ��ڲ��� : GpioSclId-I2C��clock, GpioSdaId--I2C��data
* ���ز��� : NONE
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
* �� �� �� : i2c_SendNoAck                                                                                                  
* �������� : ���豸���ͷ�Ӧ��λ
* ��ڲ��� : GpioSclId-I2C��clock, GpioSdaId--I2C��data
* ���ز��� : NONE
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

/*Ŀǰֻ��Ҫʵ��˳����������Ĳ�֧��*/
#if 0// EEPROM_SUPPORT_WRITE
/*******************************************************************************
* �� �� �� : i2c_delay_ms                                                                                                  
* �������� : ������ʱ�������һ��д����֮����Ҫ��ʱ5MS�����ڲ�д�������
* ��ڲ��� : ms ��������������5ms �򴫵ݲ��� ms = 5
* ���ز��� : NONE
*******************************************************************************/
static void i2c_delay_ms(uint32 ms)
{
    udelay(ms*1000);
    return;
}

/*******************************************************************************
* �� �� �� : EEPROM_ByteWrite                                                                                                  
* �������� : EEPROM�����ַдһ���ֽ�����
* ��ڲ��� : addr-AT24CXX�ڲ���ַ��data-Ԥд�������
* ���ز��� : 0-�����ɹ�����0-����ʧ��
*******************************************************************************/
static uint16 EEPROM_ByteWrite(uint16 addr,uint8 data, GPIO_PIN_E gpio_scl_pin, GPIO_PIN_E gpio_sda_pin)
{
    uint8 Dev_addr;               //�豸��ַ
    uint8 AddrLow;
    uint8 AddrHigh;
    gpioID GpioSclId  = 0; 
    gpioID GpioSdaId  = 0; 
    
    if(addr > EEPROM_ADDR_MAX)            //���Ԥд���ַ�Ƿ���Ч                      
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
* �� �� �� : EEPROM_PageWrite                                                                                                  
* �������� : ������ҳ��ʼ��дN�ֽڣ��������Կ�ҳ
* ��ڲ��� : page-ҳ�ţ�n-дn�ֽ�����
* ���ز��� : 0-�����ɹ�����0-����ʧ��
*******************************************************************************/
static uint16 EEPROM_PageWrite(uint8 page,uint8* p,uint8 n, GPIO_PIN_E gpio_scl_pin, GPIO_PIN_E gpio_sda_pin)
{
    uint8 Dev_addr;                 //�豸��ַ
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
    
    if((n > 16)|(page > 128))       //���ݶ�д���豸�����Ϊ�ʺϵ�ҳ����ÿҳ�ֽ���
    {
        return OutOfRang;
    }
    tmp = ((uint16)page) << 4;      //�ó�ҳ�׵�ַ
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
* �� �� �� : EEPROM_RandomRead                                                                                                  
* �������� : EEPROM�����ַ��ȡһ���ֽ�
* ��ڲ��� : addrԤ��ȡ�ĵ�ַ
* ���ز��� : ��ȡ������
*******************************************************************************/
static uint8 EEPROM_RandomRead(uint16 addr, uint8 *p, GPIO_PIN_E gpio_scl_pin, GPIO_PIN_E gpio_sda_pin)
{
    uint8 Dev_addr;               //�豸��ַ
    uint8 AddrLow;
    uint8 AddrHigh;
    uint8 tmp;
    gpioID GpioSclId  = 0; 
    gpioID GpioSdaId  = 0; 
    
    if(NULL == p)
    {
        return PointError;
    }
    
    if(addr > EEPROM_ADDR_MAX)            //���Ԥд���ַ�Ƿ���Ч                      
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
* �� �� �� : EEPROM_SequentialRead                                                                                                  
* �������� : EEPROM�������ַ������ȡN���ֽ�
* ��ڲ��� : addr-Ԥ��ȡ�ĵ�ַ��n-Ԥ��ȡ���ֽ�����p-�������ݴ��ָ��,
             gpio_scl_pin-I2C��clock, gpio_sda_pin--I2C��data
* ���ز��� : n Ԥ��ȡ���ֽڸ���
*******************************************************************************/
static uint16 EEPROM_SequentialRead(uint16 addr,uint16 n,uint8* p, GPIO_PIN_E gpio_scl_pin, GPIO_PIN_E gpio_sda_pin)
{
    uint8 Dev_addr = 0;   //�豸��ַ
    uint8 AddrLow  = 0;
    uint8 AddrHigh = 0;
    gpioID GpioSclId  = 0; 
    gpioID GpioSdaId  = 0; 
    EEPROM_RET ret = EepromOk;

    if(NULL == p)
    {
        return (uint16)PointError;
    }

     /*���Ԥд���ַ�Ƿ���Ч*/
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
* �� �� �� : EEPROM_Reset                                                                                                  
* �������� : EEPROM I2C�쳣��λ������
* ��ڲ��� : gpio_scl_pin-I2C��clock, gpio_sda_pin--I2C��data
* ���ز��� : 
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
  Description: ������Ķ˿ںŻ�ȡ��Ӧ�˿ڵ�I2C�ܽ�
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
  Description: ��ȡ��ģ�����Ϣ
        Input: port_num_t lgcPort -- �˿ں�
               uint16 ucAddr      -- ��ȡ��Ϣ����ʼ��ַ
               uint32 ulLen       -- ��ȡ��Ϣ�ĳ���
               uint32 ulPDataType -- ��ַ�ռ�(ADDRESS_IN_USER_SPACE��ADDRESS_IN_KERNEL_SPACE)
       Output: uint8* pucData     -- ��Ŷ�ȡ������Ϣ
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

    /*I2C��ʼ��*/
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
        /*��ȡʧ�ܣ�����Ϊ��I2C�쳣����Ҫ��λ*/
        if(EepromOk != EEPROM_Reset(gpio_scl_pin, gpio_sda_pin))
        {
            EEPROM_DEBUG("EEPROM_Reset fail, %s, %d\n",__FUNCTION__,__LINE__);
            SPIN_HAL_EEPROM_UNLOCK;
            return DRV_SDK_ERR_EEPROM_I2C_READ;
        }
        else
        {
            EEPROM_DEBUG("EEPROM_Reset fail, %s, %d\n",__FUNCTION__,__LINE__);
            /*���¶�ȡ*/
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
  Description: I2C�ܽų�ʼ������
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
  Description: ��ģ�����Ϣд�뵽�ڴ���
        Input: port_num_t lgcPort -- �˿ں�
               uint8* pucData     -- ���д�����Ϣ
               uint32 ulLen       -- д����Ϣ�ĳ���
               BOOL bValidFlag    -- ���ڱ�־д�����Ϣ�Ƿ���Ч
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
    
    /*�Ƚ�flag���ó�invalid*/
    stFlag = FIBER_INFO_INVALID;
    if(IF_OK != K_IF_SetAttr(uiIfindex, IF_FIBER_INFO_FLAG, &stFlag, sizeof(FIBERINFOFLAG_E)))
    {
        return FALSE;
    }

    /*������ϢֻҪ����flagΪinvalid������Ҫ�����Ϣ*/
    if(FALSE == bValidFlag)
    {
        return TRUE;
    }

    if(NULL == pucData)
    {
        return FALSE;
    }
    
    /*������Ϣ*/
    if(IF_OK != K_IF_SetAttr(uiIfindex, IF_FIBER_INFO, pucData, ulLen))
    {
        return FALSE;
    }

    /*��flag���ó�valid*/
    stFlag = FIBER_INFO_VALID;
    if(IF_OK != K_IF_SetAttr(uiIfindex, IF_FIBER_INFO_FLAG, &stFlag, sizeof(FIBERINFOFLAG_E)))
    {
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************************
    Func Name: FiberInfoUpdate
  Description: ���±������ڴ��й�ģ�����Ϣ
        Input: uint32 unit        -- �豸��
               uint32 phyPort     -- ����˿ں�
               BOOL bValidFlag    -- ���ڱ�־д�����Ϣ�Ƿ���Ч
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
        
        /*��ȡ��Ϣ*/
        if(DRV_OK != Hal_I2cFiberPortRead(lgcPort, I2C_START, pucTemp, MAX_TRANSCEIVER_CODE_LEN, ADDRESS_IN_KERNEL_SPACE))
        {
            kfree(pucTemp);
            return FALSE;
        }

        /*������Ϣ*/
        if(TRUE != FiberInfoWriteToDram(lgcPort, pucTemp, MAX_TRANSCEIVER_CODE_LEN, bValidFlag))
        {
            kfree(pucTemp);
            return FALSE;
        }

        kfree(pucTemp);
    }

    /*���¹�ڵ�����*/
    (void)ComboPortCfgChange(unit, phyPort, PORT_MEDIA_FIBER);
    return TRUE;
}

#ifdef  __cplusplus
}
#endif

