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
 * $Revision: 41959 $
 * $Date: 2013-08-09 18:25:45 +0800 (星期五, 09 八月 2013) $
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
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/i2c.h>
#include <dal/apollomp/dal_apollomp_i2c.h>

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
 *      dal_apollomp_i2c_init
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
dal_apollomp_i2c_init(rtk_i2c_port_t i2cPort)
{
    uint32 data;
    uint32 value;
    uint32 array_idx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "");

    /* function body */
    if(I2C_PORT_0 == i2cPort)
        array_idx = 0;
    else
        array_idx = 1;

    reg_array_read(APOLLOMP_I2C_CLOCK_DIVr, REG_ARRAY_INDEX_NONE, array_idx, &data);

    /* set address width to 16 bit */
    value = 1;
    reg_field_set(APOLLOMP_I2C_CLOCK_DIVr, APOLLOMP_I2C_AWf, &value, &data);

    /* set data width to 16 bit */
    value = 1;
    reg_field_set(APOLLOMP_I2C_CLOCK_DIVr, APOLLOMP_I2C_DWf, &value, &data);

    /* set clock to 125KHz bit */
    /* real clock rate = ( 125MHz /(((2*I2C_CLOCK_DIV)+2)*2) )*/
    value = 0x000000f9;
    data = 0x0;
    reg_field_set(APOLLOMP_I2C_CLOCK_DIVr, APOLLOMP_I2C_CLOCK_DIVf, &value, &data);

    reg_array_write(APOLLOMP_I2C_CLOCK_DIVr, REG_ARRAY_INDEX_NONE, array_idx, &data);

    /* set enable */
    reg_field_read(APOLLOMP_IO_MODE_ENr, APOLLOMP_I2C_ENf, &value);
    if(I2C_PORT_0 == i2cPort)
        value |= 0x1;
    else
        value |= 0x2;

    reg_field_write(APOLLOMP_IO_MODE_ENr, APOLLOMP_I2C_ENf, &value);

    /* Init status */
    i2c_init = INIT_COMPLETED;


    return RT_ERR_OK;
}   /* end of dal_apollomp_i2c_init */

/* Function Name:
 *      dal_apollomp_i2c_enable_set
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
dal_apollomp_i2c_enable_set(rtk_i2c_port_t i2cPort, rtk_enable_t enable)
{
    uint32 val;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "enable=%d",enable);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if(ENABLED == enable)
        data = 1;
    else
        data = 0;

    reg_field_read(APOLLOMP_IO_MODE_ENr, APOLLOMP_I2C_ENf, &val);

    if(I2C_PORT_0 == i2cPort)
    {
        val &= ~(0x1<<0);
        val |= (data<<0);
    }
    else
    {
        val &= ~(0x1<<1);
        val |= (data<<1);
    }

    reg_field_write(APOLLOMP_IO_MODE_ENr, APOLLOMP_I2C_ENf, &val);

    return RT_ERR_OK;
}   /* end of dal_apollomp_i2c_enable_set */

/* Function Name:
 *      dal_apollomp_i2c_enable_get
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
dal_apollomp_i2c_enable_get(rtk_i2c_port_t i2cPort, rtk_enable_t *pEnable)
{
    uint32 val;
    uint32 mask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "");

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    if(I2C_PORT_0 == i2cPort)
        mask = 0x1<<0;
    else
        mask = 0x1<<1;

    reg_field_read(APOLLOMP_IO_MODE_ENr, APOLLOMP_I2C_ENf, &val);
    if(val & mask)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_apollomp_i2c_enable_get */

/* Function Name:
 *      dal_apollomp_i2c_width_set
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
dal_apollomp_i2c_width_set(rtk_i2c_port_t i2cPort, rtk_i2c_width_t width)
{
    uint32 val;
    uint32 array_idx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "width=%d",width);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((I2C_WIDTH_END <=width), RT_ERR_INPUT);

    /* function body */
    if(I2C_PORT_0 == i2cPort)
        array_idx = 0;
    else
        array_idx = 1;

    if(I2C_WIDTH_16bit == width)
        val = 1;
    else if(I2C_WIDTH_8bit == width)
        val = 0;
    else
        return RT_ERR_INPUT;

    /* set address width */
    reg_array_field_write(APOLLOMP_I2C_CLOCK_DIVr, APOLLOMP_I2C_AWf, REG_ARRAY_INDEX_NONE, array_idx, &val);

    /* set data width */
    reg_array_field_write(APOLLOMP_I2C_CLOCK_DIVr, APOLLOMP_I2C_DWf, REG_ARRAY_INDEX_NONE, array_idx, &val);

    return RT_ERR_OK;
}   /* end of dal_apollomp_i2c_width_set */

/* Function Name:
 *      dal_apollomp_i2c_width_get
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
dal_apollomp_i2c_width_get(rtk_i2c_port_t i2cPort, rtk_i2c_width_t *pWidth)
{
    uint32 val;
    uint32 array_idx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "");

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pWidth), RT_ERR_NULL_POINTER);

    /* function body */
    if(I2C_PORT_0 == i2cPort)
        array_idx = 0;
    else
        array_idx = 1;

    reg_array_field_read(APOLLOMP_I2C_CLOCK_DIVr, APOLLOMP_I2C_AWf, REG_ARRAY_INDEX_NONE, array_idx, &val);

    if(val == 1)
        *pWidth = I2C_WIDTH_16bit;
    else if(val == 0)
        *pWidth = I2C_WIDTH_8bit;
    else if(val == 2)
        *pWidth = I2C_WIDTH_24bit;
    else if(val == 3)
        *pWidth = I2C_WIDTH_32bit;

    return RT_ERR_OK;
}   /* end of dal_apollomp_i2c_width_get */

/* Function Name:
 *      dal_apollomp_i2c_write
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
dal_apollomp_i2c_write(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 data)
{
    uint32 value;
    uint32 en;
    uint32 array_idx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "devID=%d,regAddr=%d,data=%d",devID, regAddr, data);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* function body */
    if(I2C_PORT_0 == i2cPort)
        array_idx = 0;
    else
        array_idx = 1;

    /* set device ID */
    reg_array_field_write(APOLLOMP_I2C_CLOCK_DIVr, REG_ARRAY_INDEX_NONE, array_idx, APOLLOMP_I2C_DEV_IDf, &devID);
    /* set write data */
    reg_array_field_write(APOLLOMP_I2C_IND_WDr, REG_ARRAY_INDEX_NONE, array_idx, APOLLOMP_WR_DATf, &data);

    /* set regAddr */
    value=0;
    reg_field_set(APOLLOMP_I2C_IND_CMDr, APOLLOMP_ADRf, &regAddr, &value);
    /* set write */
    en = 1;
    reg_field_set(APOLLOMP_I2C_IND_CMDr, APOLLOMP_WRENf, &en, &value);
    /* set cmd en */
    en = 1;
    reg_field_set(APOLLOMP_I2C_IND_CMDr, APOLLOMP_CMD_ENf, &en, &value);

    reg_array_write(APOLLOMP_I2C_IND_CMDr, REG_ARRAY_INDEX_NONE, array_idx, &value);

    /* check busy */
    do{
        reg_array_field_read(APOLLOMP_I2C_IND_RDr, REG_ARRAY_INDEX_NONE, array_idx, APOLLOMP_BUSYf, &value);
    }while(value == 1);

    return RT_ERR_OK;
}   /* end of dal_apollomp_i2c_write */

/* Function Name:
 *      dal_apollomp_i2c_read
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
dal_apollomp_i2c_read(rtk_i2c_port_t i2cPort, uint32 devID, uint32 regAddr, uint32 *pData)
{
    uint32 value;
    uint32 en;
    uint32 busy;
    uint32 array_idx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "devID=%d,regAddr=%d",devID, regAddr);

    /* check Init status */
    RT_INIT_CHK(i2c_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    if(I2C_PORT_0 == i2cPort)
        array_idx = 0;
    else
        array_idx = 1;

    /* set device ID */
    reg_array_field_write(APOLLOMP_I2C_CLOCK_DIVr, REG_ARRAY_INDEX_NONE, array_idx, APOLLOMP_I2C_DEV_IDf, &devID);

    /* set regAddr */
    value=0;
    reg_field_set(APOLLOMP_I2C_IND_CMDr, APOLLOMP_ADRf, &regAddr, &value);
    /* set read */
    en = 0;
    reg_field_set(APOLLOMP_I2C_IND_CMDr, APOLLOMP_WRENf, &en, &value);
    /* set cmd en */
    en = 1;
    reg_field_set(APOLLOMP_I2C_IND_CMDr, APOLLOMP_CMD_ENf, &en, &value);

    reg_array_write(APOLLOMP_I2C_IND_CMDr, REG_ARRAY_INDEX_NONE, array_idx, &value);

    do
    {
        reg_array_read(APOLLOMP_I2C_IND_RDr, REG_ARRAY_INDEX_NONE, array_idx, &value);
        reg_field_get(APOLLOMP_I2C_IND_RDr, APOLLOMP_BUSYf, &busy, &value);
    } while (busy == 1);

    reg_field_get(APOLLOMP_I2C_IND_RDr, APOLLOMP_RD_DATf, pData, &value);

    return RT_ERR_OK;
}   /* end of dal_apollomp_i2c_read */

/* Function Name:
 *      dal_apollomp_i2c_clock_set
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
dal_apollomp_i2c_clock_set(rtk_i2c_port_t i2cPort, uint32 clock)
{
    uint32 array_idx;
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "clock=%d",clock);

    /* parameter check */
    RT_PARAM_CHK((0x3ff < clock), RT_ERR_INPUT);

    /* function body */
    if(I2C_PORT_0 == i2cPort)
        array_idx = 0;
    else
        array_idx = 1;

    data = (31250/clock) - 1;

    /* set data width */
    if((ret = reg_array_field_write(APOLLOMP_I2C_CLOCK_DIVr, REG_ARRAY_INDEX_NONE, array_idx, APOLLOMP_I2C_CLOCK_DIVf, &data)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}   /* end of dal_apollomp_i2c_clock_set */

/* Function Name:
 *      dal_apollomp_i2c_clock_get
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
dal_apollomp_i2c_clock_get(rtk_i2c_port_t i2cPort, uint32 *pClock)
{
    uint32 array_idx;
    uint32 data;
    uint32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_HWMISC), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == pClock), RT_ERR_NULL_POINTER);

    /* function body */
    if(I2C_PORT_0 == i2cPort)
        array_idx = 0;
    else
        array_idx = 1;

    if((ret = reg_array_field_read(APOLLOMP_I2C_CLOCK_DIVr, REG_ARRAY_INDEX_NONE, array_idx, APOLLOMP_I2C_CLOCK_DIVf, &data)) != RT_ERR_OK)
        return ret;

    *pClock = 125000/((data*2+2)*2);

    return RT_ERR_OK;
}   /* end of dal_apollomp_i2c_clock_get */

