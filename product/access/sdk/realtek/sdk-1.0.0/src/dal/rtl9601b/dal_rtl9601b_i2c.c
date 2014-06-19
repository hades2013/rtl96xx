/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 40329 $
 * $Date: 2013-06-19 19:22:57 +0800 (Wed, 19 Jun 2013) $
 *
 * Purpose : Definition of I2C API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) I2C control
 *           (2) I2C read/write
 */



/*
 * Include Files
 */

#include <common/rt_type.h>
#include <dal/rtl9601b/dal_rtl9601b.h>
#include <dal/rtl9601b/dal_rtl9601b_i2c.h>
#include <rtk/i2c.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32    i2c_init = INIT_NOT_COMPLETED;

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      dal_rtl9601b_i2c_init
 * Description:
 *      Initialize i2c interface.
 * Input:
 *      i2cPort    - I2C port interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize before calling any other APIs.
 */
int32
dal_rtl9601b_i2c_init(rtk_i2c_port_t i2cPort)
{
    uint32 data;
    uint32 value;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "");

    /* function body */
    if(I2C_PORT_0 != i2cPort)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if((ret = reg_read(RTL9601B_I2C_IND_WIDTHr, &data)) != RT_ERR_OK)
        return ret;

    /* set address width to 16 bit */
    value = 1;
    if((ret = reg_field_set(RTL9601B_I2C_IND_WIDTHr, RTL9601B_I2C_DEVICE_AWf, &value, &data)) != RT_ERR_OK)
        return ret;

    /* set data width to 16 bit */
    value = 1;
    if((ret = reg_field_set(RTL9601B_I2C_IND_WIDTHr, RTL9601B_I2C_DEVICE_DWf, &value, &data)) != RT_ERR_OK)
        return ret;

    if((ret = reg_write(RTL9601B_I2C_IND_WIDTHr, &data)) != RT_ERR_OK)
        return ret;

    /* set clock to 125KHz bit */
    value = 0xF9;
    if((ret = reg_field_write(RTL9601B_I2C_MASTER_CLOCK_DIVr, RTL9601B_I2C_CLOCK_DIVf, &value)) != RT_ERR_OK)
        return ret;

    /* set enable */
    value = 1;
    if((ret = reg_field_write(RTL9601B_IO_MODE_ENr, RTL9601B_I2C_MASTER_ENf, &value)) != RT_ERR_OK)
        return ret;

    /* Init status */
    i2c_init = INIT_COMPLETED;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_i2c_init */

/* Function Name:
 *      dal_rtl9601b_i2c_enable_set
 * Description:
 *      Enable/Disable I2C interface.
 * Input:
 *      i2cPort    - I2C port interface
 *      enable     - enable/disable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
int32
dal_rtl9601b_i2c_enable_set(rtk_i2c_port_t i2cPort, rtk_enable_t enable)
{
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "enable=%d",enable);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    if(I2C_PORT_0 != i2cPort)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if(ENABLED == enable)
        data = 1;
    else
        data = 0;

    if((ret = reg_field_write(RTL9601B_IO_MODE_ENr, RTL9601B_I2C_MASTER_ENf, &data)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_i2c_enable_set */

/* Function Name:
 *      dal_rtl9601b_i2c_enable_get
 * Description:
 *      Get I2C interface state.
 * Input:
 *      i2cPort   - I2C port interface
 *      pEnable   - the pointer of enable/disable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
int32
dal_rtl9601b_i2c_enable_get(rtk_i2c_port_t i2cPort, rtk_enable_t *pEnable)
{
    uint32 val;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "");

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    if(I2C_PORT_0 != i2cPort)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if((ret = reg_field_read(RTL9601B_IO_MODE_ENr, RTL9601B_I2C_MASTER_ENf, &val)) != RT_ERR_OK)
        return ret;

    if(1 == val)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_i2c_enable_get */

/* Function Name:
 *      dal_rtl9601b_i2c_width_set
 * Description:
 *      Set the data and address width of I2C interface.
 * Input:
 *      i2cPort   - I2C port interface
 *      width     - 8-bit or 16-bit
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
int32
dal_rtl9601b_i2c_width_set(rtk_i2c_port_t i2cPort, rtk_i2c_width_t width)
{
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "width=%d",width);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((I2C_WIDTH_END <=width), RT_ERR_INPUT);

    /* function body */
    if(I2C_PORT_0 != i2cPort)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if(I2C_WIDTH_24bit == width)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if((ret = reg_read(RTL9601B_I2C_IND_WIDTHr, &data)) != RT_ERR_OK)
        return ret;

    /* set address width */
    if((ret = reg_field_set(RTL9601B_I2C_IND_WIDTHr, RTL9601B_I2C_DEVICE_AWf, &width, &data)) != RT_ERR_OK)
        return ret;

    /* set data width */
    if((ret = reg_field_set(RTL9601B_I2C_IND_WIDTHr, RTL9601B_I2C_DEVICE_DWf, &width, &data)) != RT_ERR_OK)
        return ret;

    if((ret = reg_write(RTL9601B_I2C_IND_WIDTHr, &data)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_i2c_width_set */

/* Function Name:
 *      dal_rtl9601b_i2c_width_get
 * Description:
 *      Get the data and address width of I2C interface.
 * Input:
 *      i2cPort    - I2C port interface
 *      pWidth     - the pointer of width
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
int32
dal_rtl9601b_i2c_width_get(rtk_i2c_port_t i2cPort, rtk_i2c_width_t *pWidth)
{
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "");

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pWidth), RT_ERR_NULL_POINTER);

    /* function body */
    if(I2C_PORT_0 != i2cPort)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if((ret = reg_field_read(RTL9601B_I2C_IND_WIDTHr, RTL9601B_I2C_DEVICE_AWf, pWidth)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_i2c_width_get */

/* Function Name:
 *      dal_rtl9601b_i2c_write
 * Description:
 *      I2c write data.
 * Input:
 *      i2cPort   - I2C port interface
 *      devID     - the device ID
 *      regAddr   - register address
 *      data      - data value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
int32
dal_rtl9601b_i2c_write(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 data)
{
    uint32 value;
    uint32 en;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "devID=%d,regAddr=%d,data=%d",devID, regAddr, data);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* function body */
    if(I2C_PORT_0 != i2cPort)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    /* set device ID */
    if((ret = reg_field_write(RTL9601B_I2C_IND_WIDTHr, RTL9601B_I2C_DEVICE_IDf, &devID)) != RT_ERR_OK)
        return ret;
    /* set write data */
    if((ret = reg_field_write(RTL9601B_I2C_IND_WDr, RTL9601B_I2C_WR_DATf, &data)) != RT_ERR_OK)
        return ret;

    /* set regAddr */
    if((ret = reg_field_write(RTL9601B_I2C_IND_ADRr, RTL9601B_I2C_ADRf, &regAddr)) != RT_ERR_OK)
        return ret;

    /* set write */
    value=0;
    en = 1;
    if((ret = reg_field_set(RTL9601B_I2C_IND_CMDr, RTL9601B_I2C_WRENf, &en, &value)) != RT_ERR_OK)
        return ret;
    /* set cmd en */
    en = 1;
    if((ret = reg_field_set(RTL9601B_I2C_IND_CMDr, RTL9601B_I2C_CMD_ENf, &en, &value)) != RT_ERR_OK)
        return ret;

    if((ret = reg_write(RTL9601B_I2C_IND_CMDr, &value)) != RT_ERR_OK)
        return ret;

    /* check busy */
    do{
        if((ret = reg_field_read(RTL9601B_I2C_IND_CMDr, RTL9601B_I2C_BUSYf, &value)) != RT_ERR_OK)
            return ret;
    }while(value == 1);

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_i2c_write */

/* Function Name:
 *      dal_rtl9601b_i2c_read
 * Description:
 *      I2c read data.
 * Input:
 *      i2cPort   - I2C port interface
 *      devID     - the device ID
 *      regAddr   - register address
 *      pData     - the pointer of returned data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
int32
dal_rtl9601b_i2c_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData)
{
    uint32 value;
    uint32 en;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "devID=%d,regAddr=%d",devID, regAddr);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    if(I2C_PORT_0 != i2cPort)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    /* set device ID */
    if((ret = reg_field_write(RTL9601B_I2C_IND_WIDTHr, RTL9601B_I2C_DEVICE_IDf, &devID)) != RT_ERR_OK)
        return ret;

    /* set regAddr */
    if((ret = reg_field_write(RTL9601B_I2C_IND_ADRr, RTL9601B_I2C_ADRf, &regAddr)) != RT_ERR_OK)
        return ret;

    /* set read */
    value=0;
    en = 0;
    if((ret = reg_field_set(RTL9601B_I2C_IND_CMDr, RTL9601B_I2C_WRENf, &en, &value)) != RT_ERR_OK)
        return ret;
    /* set cmd en */
    en = 1;
    if((ret = reg_field_set(RTL9601B_I2C_IND_CMDr, RTL9601B_I2C_CMD_ENf, &en, &value)) != RT_ERR_OK)
        return ret;

    if((ret = reg_write(RTL9601B_I2C_IND_CMDr, &value)) != RT_ERR_OK)
        return ret;

    do
    {
        if((ret = reg_field_read(RTL9601B_I2C_IND_CMDr, RTL9601B_I2C_BUSYf, &value)) != RT_ERR_OK)
            return ret;
    } while (value == 1);

    if((ret = reg_field_read(RTL9601B_I2C_IND_RDr, RTL9601B_I2C_RD_DATf, pData)) != RT_ERR_OK)
            return ret;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_i2c_read */

/* Function Name:
 *      dal_rtl9601b_i2c_clock_set
 * Description:
 *      Set the clock of I2C interface.
 * Input:
 *      i2cPort   - I2C port interface
 *      clock     - KMHz, 10-bit value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
int32
dal_rtl9601b_i2c_clock_set(rtk_i2c_port_t i2cPort, uint32 clock)
{
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "clock=%d",clock);

    /* parameter check */
    RT_PARAM_CHK((0x3ff < clock), RT_ERR_INPUT);

    /* function body */
    if(I2C_PORT_0 != i2cPort)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    data = (31250/clock) - 1;

    /* set data width */
    if((ret = reg_field_write(RTL9601B_I2C_MASTER_CLOCK_DIVr, RTL9601B_I2C_CLOCK_DIVf, &data)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_i2c_clock_set */

/* Function Name:
 *      dal_rtl9601b_i2c_clock_get
 * Description:
 *      Get the clock of I2C interface.
 * Input:
 *      i2cPort    - I2C port interface
 *      pClock     - KMHz, 10-bit value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
int32
dal_rtl9601b_i2c_clock_get(rtk_i2c_port_t i2cPort, uint32 *pClock)
{
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == pClock), RT_ERR_NULL_POINTER);

    /* function body */
    if(I2C_PORT_0 != i2cPort)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if((ret = reg_field_read(RTL9601B_I2C_MASTER_CLOCK_DIVr, RTL9601B_I2C_CLOCK_DIVf, &data)) != RT_ERR_OK)
        return ret;

    *pClock = 125000/((data*2+2)*2);

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_i2c_width_get */

/* Function Name:
 *      dal_rtl9601b_i2c_eepMirror_set
 * Description:
 *      Configure EEPROM mirror setting.
 * Input:
 *      devId           - device id of eeprom
 *      addrWidth       - address width
 *      addrExtMode     - EEPROM device ID A2 A1 A0 used as addr extension bit
 *                          0b00:A2A1A0,  A2A1A0 not used as addr extension bit
 *                          0b01:A2A1P0,  A0 used as addr extension bit
 *                          0b10:A2P1P0,  A1A0 used as addr extension bit
 *                          0b11:P2P1P0�A A2A1A0 used as addr extension bit
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize before calling any other APIs.
 */
int32
dal_rtl9601b_i2c_eepMirror_set(rtk_i2c_eeprom_mirror_t eepMirrorCfg)
{
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "");

    /* parameter check */
    RT_PARAM_CHK((0x7f < eepMirrorCfg.devId), RT_ERR_INPUT);
    RT_PARAM_CHK((I2C_WIDTH_END <= eepMirrorCfg.addrWidth), RT_ERR_INPUT);
    RT_PARAM_CHK((I2C_ADDR_EXT_MODE_END <= eepMirrorCfg.addrExtMode), RT_ERR_INPUT);


    /* function body */
    if((ret = reg_read(RTL9601B_I2C_IND_WIDTHr, &data)) != RT_ERR_OK)
        return ret;

    /* set device id */
    if((ret = reg_field_set(RTL9601B_I2C_IND_WIDTHr, RTL9601B_EEPROM_DEV_IDf, &eepMirrorCfg.devId, &data)) != RT_ERR_OK)
        return ret;

    /* set address width */
    if((ret = reg_field_set(RTL9601B_I2C_IND_WIDTHr, RTL9601B_EEPROM_AWf, &eepMirrorCfg.addrWidth, &data)) != RT_ERR_OK)
        return ret;

    /* set address ext mode */
    if((ret = reg_field_set(RTL9601B_I2C_IND_WIDTHr, RTL9601B_EEPROM_AW_EXTf, &eepMirrorCfg.addrExtMode, &data)) != RT_ERR_OK)
        return ret;

    if((ret = reg_write(RTL9601B_I2C_IND_WIDTHr, &data)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_i2c_eepMirror_set */

/* Function Name:
 *      dal_rtl9601b_i2c_eepMirror_get
 * Description:
 *      Get EEPROM mirror setting.
 * Input:
 *      devId           - device id of eeprom
 *      addrWidth       - address width
 *      addrExtMode     - EEPROM device ID A2 A1 A0 used as addr extension bit
 *                          0b00:A2A1A0,  A2A1A0 not used as addr extension bit
 *                          0b01:A2A1P0,  A0 used as addr extension bit
 *                          0b10:A2P1P0,  A1A0 used as addr extension bit
 *                          0b11:P2P1P0�A A2A1A0 used as addr extension bit
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize before calling any other APIs.
 */
int32
dal_rtl9601b_i2c_eepMirror_get(rtk_i2c_eeprom_mirror_t *pEepMirrorCfg)
{
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == pEepMirrorCfg), RT_ERR_NULL_POINTER);


    /* function body */
    if((ret = reg_read(RTL9601B_I2C_IND_WIDTHr, &data)) != RT_ERR_OK)
        return ret;

    /* device id */
    if((ret = reg_field_get(RTL9601B_I2C_IND_WIDTHr, RTL9601B_EEPROM_DEV_IDf, &pEepMirrorCfg->devId, &data)) != RT_ERR_OK)
        return ret;

    /* address width */
    if((ret = reg_field_get(RTL9601B_I2C_IND_WIDTHr, RTL9601B_EEPROM_AWf, &pEepMirrorCfg->addrWidth, &data)) != RT_ERR_OK)
        return ret;

    /* address ext mode */
    if((ret = reg_field_get(RTL9601B_I2C_IND_WIDTHr, RTL9601B_EEPROM_AW_EXTf, &pEepMirrorCfg->addrExtMode, &data)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_i2c_eepMirror_get */

/* Function Name:
 *      dal_rtl9601b_i2c_eepMirror_write
 * Description:
 *      I2c EEPROM mirror write data.
 * Input:
 *      regAddr   - register address
 *      data      - data value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *
 */
int32
dal_rtl9601b_i2c_eepMirror_write(uint32 regAddr, uint32 data)
{
    uint32 value;
    uint32 en;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "regAddr=%d,data=%d", regAddr, data);

    /* function body */

    /* set write data */
    if((ret = reg_field_write(RTL9601B_I2C_IND_WDr, RTL9601B_I2C_WR_DATf, &data)) != RT_ERR_OK)
        return ret;

    /* set regAddr */
    if((ret = reg_field_write(RTL9601B_I2C_IND_ADRr, RTL9601B_I2C_ADRf, &regAddr)) != RT_ERR_OK)
        return ret;

    /* set write */
    value=0;
    en = 3;
    if((ret = reg_field_set(RTL9601B_I2C_IND_CMDr, RTL9601B_I2C_WRENf, &en, &value)) != RT_ERR_OK)
        return ret;
    /* set cmd en */
    en = 1;
    if((ret = reg_field_set(RTL9601B_I2C_IND_CMDr, RTL9601B_I2C_CMD_ENf, &en, &value)) != RT_ERR_OK)
        return ret;

    if((ret = reg_write(RTL9601B_I2C_IND_CMDr, &value)) != RT_ERR_OK)
        return ret;

    /* check busy */
    do{
        if((ret = reg_field_read(RTL9601B_I2C_IND_CMDr, RTL9601B_I2C_BUSYf, &value)) != RT_ERR_OK)
            return ret;
    }while(value == 1);

    /* check NACK */
    if((ret = reg_field_read(RTL9601B_I2C_IND_CMDr, RTL9601B_NACKf, &value)) != RT_ERR_OK)
        return ret;

    if(1 == value)
        return RT_ERR_EEPROM_NACK;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_i2c_eepMirror_write */

/* Function Name:
 *      dal_rtl9601b_i2c_eepMirror_read
 * Description:
 *      I2c read data from EEPROM mirror.
 * Input:
 *      regAddr   - register address
 *      pData     - the pointer of returned data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *
 */
int32
dal_rtl9601b_i2c_eepMirror_read(uint32 regAddr, uint32 *pData)
{
    uint32 value;
    uint32 en;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "regAddr=%d", regAddr);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */

    /* set regAddr */
    if((ret = reg_field_write(RTL9601B_I2C_IND_ADRr, RTL9601B_I2C_ADRf, &regAddr)) != RT_ERR_OK)
        return ret;

    /* set read */
    value=0;
    en = 2;
    if((ret = reg_field_set(RTL9601B_I2C_IND_CMDr, RTL9601B_I2C_WRENf, &en, &value)) != RT_ERR_OK)
        return ret;
    /* set cmd en */
    en = 1;
    if((ret = reg_field_set(RTL9601B_I2C_IND_CMDr, RTL9601B_I2C_CMD_ENf, &en, &value)) != RT_ERR_OK)
        return ret;

    if((ret = reg_write(RTL9601B_I2C_IND_CMDr, &value)) != RT_ERR_OK)
        return ret;

    do
    {
        if((ret = reg_field_read(RTL9601B_I2C_IND_CMDr, RTL9601B_I2C_BUSYf, &value)) != RT_ERR_OK)
            return ret;
    } while (value == 1);

    if((ret = reg_field_read(RTL9601B_I2C_IND_RDr, RTL9601B_I2C_RD_DATf, pData)) != RT_ERR_OK)
            return ret;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_i2c_eepMirror_read */

