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
 * $Revision: 39787 $
 * $Date: 2013-05-28 17:16:03 +0800 (Tue, 28 May 2013) $
 *
 * Purpose : Definition of GPIO API
 *
 * Feature : Provide the APIs to enable and configure GPIO
 *
 */



/*
 * Include Files
 */
#include <dal/rtl9601b/dal_rtl9601b.h>
#include <rtk/gpio.h>
#include <dal/rtl9601b/dal_rtl9601b_gpio.h>


static uint32    gpio_init = INIT_NOT_COMPLETED;

/* Function Name:
 *      dal_rtl9601b_gpio_init
 * Description:
 *      gpio init function
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
 extern int32 
dal_rtl9601b_gpio_init(void )
{
	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO),"%s",__FUNCTION__);
	gpio_init = INIT_COMPLETED;		
	return RT_ERR_OK;
}



/* Function Name:
 *      dal_rtl9601b_gpio_state_set
 * Description:
 *      enable or disable gpio function
 * Input:
 *      gpioId		- gpio id from 0~71
 *      enable		- enable or disable 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32 
dal_rtl9601b_gpio_state_set(uint32 gpioId, rtk_enable_t enable)
{
    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d,enable=%d,ret=%d",gpioId, enable,ret);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_GPIO_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
	//Star mark it, since register TBD
#if 0

    /* function body */
   if ((ret=reg_array_field_write(RTL9601B_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, gpioId, RTL9601B_EN_GPIOf,&enable)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }
#endif
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_gpio_state_set */



/* Function Name:
 *      dal_rtl9601b_gpio_state_get
 * Description:
 *      enable or disable gpio function
 * Input:
 *      gpioId		- gpio id from 0~71
 *      enable		- point for get gpio enable or disable 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32 
dal_rtl9601b_gpio_state_get(uint32 gpioId, rtk_enable_t *enable)
{
    int32 ret;
    uint32 value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d,enable=%d,ret=%d",gpioId, enable,ret);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_GPIO_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == enable), RT_ERR_INPUT);
	//Star mark it, since register TBD
#if 0

    /* function body */
   if ((ret=reg_array_field_read(RTL9601B_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, gpioId, RTL9601B_EN_GPIOf,&value)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }
#endif
    *enable = value;
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_gpio_state_get */


/* Function Name:
 *      dal_rtl9601b_gpio_mode_set
 * Description:
 *     set gpio to input or output mode
 * Input:
 *      gpioId 		-gpio id from 0 to 71
 *	  mode		-gpio mode, input or output mode	
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32 
dal_rtl9601b_gpio_mode_set(uint32 gpioId, rtk_gpio_mode_t mode)
{

    int32 ret;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d,mode=%d,ret=%d",gpioId, mode,ret);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_GPIO_NUM <=gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((GPIO_MODE_END <=mode), RT_ERR_INPUT);
	//Star mark it, since register TBD
#if 0

    /* function body */
   if ((ret=reg_array_field_write(RTL9601B_GPIO_CTRL_4r, REG_ARRAY_INDEX_NONE, gpioId, RTL9601B_SEL_GPIOf,&mode)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }
#endif
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_gpio_mode_set */


/* Function Name:
 *      dal_rtl9601b_gpio_mode_get
 * Description:
 *     get gpio to input or output mode
 * Input:
 *      gpioId 		-gpio id from 0 to 71
 *	  mode		-point for get gpio mode	
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32 
dal_rtl9601b_gpio_mode_get(uint32 gpioId, rtk_gpio_mode_t *mode)
{

    int32 ret;
    uint32 value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d,mode=%d,ret=%d",gpioId, mode,ret);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_GPIO_NUM <=gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == mode), RT_ERR_NULL_POINTER);
	//Star mark it, since register TBD
#if 0

    /* function body */
   if ((ret=reg_array_field_read(RTL9601B_GPIO_CTRL_4r, REG_ARRAY_INDEX_NONE, gpioId, RTL9601B_SEL_GPIOf,&value)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }
#endif
   *mode = value;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_gpio_mode_get */




/* Function Name:
 *      dal_rtl9601b_gpio_databit_get
 * Description:
 *     read gpio data
 * Input:
 *      gpioId 		-gpio id from 0 to 71
 *	  data		-point for read data from gpio	
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32 
dal_rtl9601b_gpio_databit_get(uint32 gpioId, uint32 *data)
{
    int32 ret;
    uint32 value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d",gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_GPIO_NUM <=gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);


    /*check gpio is enable*/
    if((ret=dal_rtl9601b_gpio_state_get(gpioId,&value)) != RT_ERR_OK)
    {
 	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }
    if(value != ENABLED)
    {
      ret  = RT_ERR_NOT_ALLOWED;
    	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }
    /*check gpio is input mode*/
    if((ret=dal_rtl9601b_gpio_mode_get(gpioId,&value)) != RT_ERR_OK)
    {
 	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }
	
    if(value!=GPIO_INPUT)
    {
      ret = RT_ERR_NOT_ALLOWED;
    	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }	
	//Star mark it, since register TBD
#if 0

    /* function body */
   if ((ret=reg_array_field_read(RTL9601B_GPIO_CTRL_1r, REG_ARRAY_INDEX_NONE, gpioId, RTL9601B_STS_GPIOf,&value)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }
#endif
   *data = value;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_gpio_databit_get */


/* Function Name:
 *      dal_rtl9601b_gpio_databit_set
 * Description:
 *     write data to gpio
 * Input:
 *      gpioId 		-gpio id from 0 to 71
 *	  data		-write data to gpio	
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32 
dal_rtl9601b_gpio_databit_set(uint32 gpioId, uint32 data)
{

    int32 ret;
    uint32 value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d",gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_GPIO_NUM <=gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((2 <=data), RT_ERR_INPUT);
	
    /*check gpio is enable*/
    if((ret=dal_rtl9601b_gpio_state_get(gpioId,&value)) != RT_ERR_OK)
    {
 	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }
    if(value != ENABLED)
    {
      ret  = RT_ERR_NOT_ALLOWED;
    	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }
     /*check gpio is input mode*/
    if((ret=dal_rtl9601b_gpio_mode_get(gpioId,&value)) != RT_ERR_OK)
    {
 	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }
	
    if(value!=GPIO_OUTPUT)
    {
      ret = RT_ERR_NOT_ALLOWED;
    	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }	
//Star mark it, since register TBD
#if 0	
    /* function body */
   if ((ret=reg_array_field_write(RTL9601B_GPIO_CTRL_0r, REG_ARRAY_INDEX_NONE, gpioId, RTL9601B_CTRL_GPIOf,&data)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }
#endif

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_gpio_databit_set */

/* Function Name:
 *      dal_rtl9601b_gpio_intr_set
 * Description:
 *     write data to gpio
 * Input:
 *      intrId: id 0 for gpio pin 63, id 1 for gpio pin 65
 *      state: point of state, enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
dal_rtl9601b_gpio_intr_set(uint32 intrId,rtk_enable_t state)
{
    int32 ret;
    uint32 value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "state=%d,ret=%d,value=%d",state,ret,value);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END<=state), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_GPIO_INTR_NUM<=intrId), RT_ERR_INPUT);
	//Star mark it, since register TBD
#if 0

    /* get current status first*/
   if ((ret=reg_field_read(RTL9601B_IO_MODE_ENr, RTL9601B_INTRPT_ENf,&value)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }

    if(intrId == 0 ){
   	value = state | (value & 0x2); 
   }else{
	value = (state << 1) | (value & 0x1); 
   }

    /* function body */
   if ((ret=reg_field_write(RTL9601B_IO_MODE_ENr, RTL9601B_INTRPT_ENf,&value)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }
  #endif
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9601b_gpio_intr_get
 * Description:
 *     write data to gpio
 * Input:
 *      intrId: id 0 for gpio pin 63, id 1 for gpio pin 65
 *      state: point of state, enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
dal_rtl9601b_gpio_intr_get(uint32 intrId,rtk_enable_t *state)
{
    int32 ret;
    uint32 value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO),"ret=%d,value=%d",ret,value);

    /* check Init status */
    RT_INIT_CHK(gpio_init);
	
    /* parameter check */
    RT_PARAM_CHK((NULL == state), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_GPIO_INTR_NUM<=intrId), RT_ERR_INPUT);
	//Star mark it, since register TBD
#if 0

    
    /* function body */
   if ((ret=reg_field_read(RTL9601B_IO_MODE_ENr, RTL9601B_INTRPT_ENf,&value)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }
   
    if(intrId == 0 ){
   	*state = (value & 0x1); 
   }else{
	*state = ((value & 0x2) >> 1 );
   }
  #endif
    return RT_ERR_OK;
}

