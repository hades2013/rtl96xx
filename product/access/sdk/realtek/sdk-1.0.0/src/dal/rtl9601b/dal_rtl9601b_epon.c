/*
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 * *
 * $Revision:  $
 * $Date: 2013-10-16 $
 *
 * Purpose : EPON MAC register access APIs
 *
 * Feature : Provide the APIs to access EPON MAC
 *
 */

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <dal/rtl9601b/dal_rtl9601b.h>
#include <dal/rtl9601b/dal_rtl9601b_ponmac.h>
#include <dal/rtl9601b/dal_rtl9601b_epon.h>
#include <ioal/mem32.h>
#include <rtk/ponmac.h>
#include <rtk/oam.h>


static uint32 epon_init = {INIT_NOT_COMPLETED};

static rtk_epon_laser_status_t forceLaserState = RTK_EPON_LASER_STATUS_NORMAL;



typedef struct rtl9601b_raw_epon_llid_table_s
{
    uint16  llid;
    uint8   valid;
    uint8   report_timer;
    uint8   is_report_timeout; /*read only*/

}rtl9601b_raw_epon_llid_table_t;




/* Function Name:
 *      rtl9601b_raw_epon_regLlidIdx_set
 * Description:
 *      Set EPON registeration LLID index
 * Input:
 *      idx 		- registeration LLID index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 * Note:
 *      None
 */
static int32 rtl9601b_raw_epon_regLlidIdx_set(uint32 idx)
{
 	int32   ret;
	uint32 tmp_val;   

    RT_PARAM_CHK((idx >= 8), RT_ERR_INPUT);

	
	tmp_val = idx;	
    if ((ret = reg_field_write(RTL9601B_EPON_RGSTR1r,RTL9601B_REG_LLID_IDXf,&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
	 
    return RT_ERR_OK;   
}



/* Function Name:
 *      rtl9601b_raw_epon_regLlidIdx_get
 * Description:
 *      Get EPON registeration LLID index
 * Input:
 *      None
 * Output:
 *      idx 		- registeration LLID index
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 * Note:
 *      None
 */
static int32 rtl9601b_raw_epon_regLlidIdx_get(uint32 *idx)
{
 	int32   ret;
	uint32 tmp_val;   
	
    if ((ret = reg_field_read(RTL9601B_EPON_RGSTR1r,RTL9601B_REG_LLID_IDXf,&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
	*idx = tmp_val;	

    return RT_ERR_OK;   
}



/* Function Name:
 *      rtl9601b_raw_epon_llidIdxMac_set
 * Description:
 *      Set EPON registeration MAC address
 * Input:
 *      idx 		- registeration LLID index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 * Note:
 *      None
 */
static int32 rtl9601b_raw_epon_llidIdxMac_set(uint32 llidIdx, rtk_mac_t *mac)
{
 	int32   ret;
 	uint32 tmp_val[3],fieldVal;   
 	
    if ((ret = reg_array_read(RTL9601B_LLID_TABLEr,REG_ARRAY_INDEX_NONE,llidIdx,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

	fieldVal = mac->octet[5];
    if ((ret = reg_field_set(RTL9601B_LLID_TABLEr,RTL9601B_ONU_MAC5f,&fieldVal,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  
	 
	fieldVal = mac->octet[5];
    if ((ret = reg_field_set(RTL9601B_LLID_TABLEr,RTL9601B_ONU_MAC5f,&fieldVal,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  

	fieldVal = mac->octet[4];
    if ((ret = reg_field_set(RTL9601B_LLID_TABLEr,RTL9601B_ONU_MAC4f,&fieldVal,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  

	fieldVal = mac->octet[3];
    if ((ret = reg_field_set(RTL9601B_LLID_TABLEr,RTL9601B_ONU_MAC3f,&fieldVal,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  

	fieldVal = mac->octet[2];
    if ((ret = reg_field_set(RTL9601B_LLID_TABLEr,RTL9601B_ONU_MAC2f,&fieldVal,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  

	fieldVal = mac->octet[1];
    if ((ret = reg_field_set(RTL9601B_LLID_TABLEr,RTL9601B_ONU_MAC1f,&fieldVal,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  

	fieldVal = mac->octet[0];
    if ((ret = reg_field_set(RTL9601B_LLID_TABLEr,RTL9601B_ONU_MAC0f,&fieldVal,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  

    if ((ret = reg_array_write(RTL9601B_LLID_TABLEr,REG_ARRAY_INDEX_NONE,llidIdx,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;   
}



/* Function Name:
 *      rtl9601b_raw_epon_llidIdxMac_get
 * Description:
 *      Get EPON registeration MAC address
 * Input:
 *      rtk_mac_t *mac 		- registeration MAC address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 * Note:
 *      None
 */
static int32 rtl9601b_raw_epon_llidIdxMac_get(uint32 llidIdx,rtk_mac_t *mac)
{
 	int32   ret;
 	uint32 tmp_val[3],fieldVal;   
 	
    if ((ret = reg_array_read(RTL9601B_LLID_TABLEr,REG_ARRAY_INDEX_NONE,llidIdx,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_get(RTL9601B_LLID_TABLEr,RTL9601B_ONU_MAC5f,&fieldVal,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  
	mac->octet[5] = fieldVal;
		
    if ((ret = reg_field_get(RTL9601B_LLID_TABLEr,RTL9601B_ONU_MAC4f,&fieldVal,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  
	mac->octet[4] = fieldVal;


    if ((ret = reg_field_get(RTL9601B_LLID_TABLEr,RTL9601B_ONU_MAC3f,&fieldVal,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  
	mac->octet[3] = fieldVal;


    if ((ret = reg_field_get(RTL9601B_LLID_TABLEr,RTL9601B_ONU_MAC2f,&fieldVal,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  
	mac->octet[2] = fieldVal;



    if ((ret = reg_field_get(RTL9601B_LLID_TABLEr,RTL9601B_ONU_MAC1f,&fieldVal,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  
	mac->octet[1] = fieldVal;



    if ((ret = reg_field_get(RTL9601B_LLID_TABLEr,RTL9601B_ONU_MAC0f,&fieldVal,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  
	mac->octet[0] = fieldVal;

    return RT_ERR_OK;   
}



/* Function Name:
 *      rtl9601b_raw_epon_regReguest_set
 * Description:
 *      Set EPON registeration request mode
 * Input:
 *      mode 		- registeration request enable/disable 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 * Note:
 *      None
 */
static int32 rtl9601b_raw_epon_regReguest_set(rtk_enable_t mode)
{
 	int32   ret;
	uint32 tmp_val;   
	
    if(mode==DISABLED)
        tmp_val = 0;    
    else
        tmp_val = 1;
        
    if ((ret = reg_field_write(RTL9601B_EPON_RGSTR2r,RTL9601B_REGISTER_REQUESTf,&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
	 
    return RT_ERR_OK;   
}



/* Function Name:
 *      rtl9601b_raw_epon_regReguest_get
 * Description:
 *      Get EPON registeration request mode
 * Input:
 *      mode 		- registeration request enable/disable 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 * Note:
 *      None
 */
static int32 rtl9601b_raw_epon_regReguest_get(rtk_enable_t *mode)
{
 	int32   ret;
	uint32 tmp_val;   
	
    if ((ret = reg_field_read(RTL9601B_EPON_RGSTR2r,RTL9601B_REGISTER_REQUESTf,&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(tmp_val==0)
        *mode = DISABLED;    
    else
        *mode = ENABLED;

    return RT_ERR_OK;   
}




/* Function Name:
 *      rtl9601b_raw_epon_regPendingGrantNum_set
 * Description:
 *      Set EPON registeration pendding grant number
 * Input:
 *      num		- pendding grant number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 * Note:
 *      None
 */
static int32 rtl9601b_raw_epon_regPendingGrantNum_set(uint8 num)
{
 	int32   ret;
	uint32 tmp_val;   

    RT_PARAM_CHK((num > 8), RT_ERR_INPUT);

	
	tmp_val = num;	
    if ((ret = reg_field_write(RTL9601B_EPON_RGSTR2r,RTL9601B_REG_PENDDING_GRANTf,&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
	 
    return RT_ERR_OK;   
}



/* Function Name:
 *      rtl9601b_raw_epon_regPendingGrantNum_get
 * Description:
 *      Get EPON registeration pendding grant number
 * Input:
 *      None 
 * Output:
 *      num		- pendding grant number * Return:
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 * Note:
 *      None
 */
static int32 rtl9601b_raw_epon_regPendingGrantNum_get(uint32 *num)
{
 	int32   ret;
	uint32 tmp_val;   
	
    if ((ret = reg_field_read(RTL9601B_EPON_RGSTR2r,RTL9601B_REG_PENDDING_GRANTf,&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
	*num = tmp_val;	

    return RT_ERR_OK;   
}



static int32 rtl9601b_raw_epon_llidTable_set(uint32 llid_idx, rtl9601b_raw_epon_llid_table_t *entry)
{
 	int32   ret;
	uint32  tmp_val[3],tmp_field_val;   

    
    if ((ret = reg_array_read(RTL9601B_LLID_TABLEr,REG_ARRAY_INDEX_NONE,llid_idx,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    
    tmp_field_val = entry->llid;
    
    if ((ret = reg_field_set(RTL9601B_LLID_TABLEr,RTL9601B_LLIDf,&tmp_field_val,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
	
    tmp_field_val = entry->valid;
    
    if ((ret = reg_field_set(RTL9601B_LLID_TABLEr,RTL9601B_VALIDf,&tmp_field_val,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  

    tmp_field_val = entry->report_timer;
    
    if ((ret = reg_field_set(RTL9601B_LLID_TABLEr,RTL9601B_REPORT_TIMERf,&tmp_field_val,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  

    if ((ret = reg_array_write(RTL9601B_LLID_TABLEr,REG_ARRAY_INDEX_NONE,llid_idx,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }        
    return RT_ERR_OK;   
}


static int32 rtl9601b_raw_epon_llidTable_get(uint32 llid_idx,rtl9601b_raw_epon_llid_table_t *entry)
{
 	int32   ret;
	uint32  tmp_val[3],tmp_field_val;   

    
    if ((ret = reg_array_read(RTL9601B_LLID_TABLEr,REG_ARRAY_INDEX_NONE,llid_idx,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
  
    
    if ((ret = reg_field_get(RTL9601B_LLID_TABLEr,RTL9601B_LLIDf,&tmp_field_val,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    entry->llid = tmp_field_val;


    if ((ret = reg_field_get(RTL9601B_LLID_TABLEr,RTL9601B_VALIDf,&tmp_field_val,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  
    entry->valid = tmp_field_val;

	
    if ((ret = reg_field_get(RTL9601B_LLID_TABLEr,RTL9601B_REPORT_TIMERf,&tmp_field_val,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  
    entry->report_timer = tmp_field_val;

   if ((ret = reg_field_get(RTL9601B_LLID_TABLEr,RTL9601B_REPORT_TIMEOUTf,&tmp_field_val,tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }  
    entry->is_report_timeout = tmp_field_val;
   
    return RT_ERR_OK;   
}



static int32 rtl9601b_raw_epon_forceLaserOn_set(uint32 force)
{
 	int32   ret;
	
    if ((ret = reg_field_write(RTL9601B_EPON_TX_CTRLr,RTL9601B_FORCE_LASER_ONf,&force)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
       
    return RT_ERR_OK;   
}


static int32 rtl9601b_raw_epon_forceLaserOn_get(uint32 *pForce)
{
 	int32   ret;
	uint32  tmp_val;   
	
    if ((ret = reg_field_read(RTL9601B_EPON_TX_CTRLr,RTL9601B_FORCE_LASER_ONf,&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    
    *pForce = tmp_val;    
        
    return RT_ERR_OK;   
}





/* Function Name:
 *      dal_rtl9601b_epon_init
 * Description:
 *      epon register level initial function
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_init(void)  
{
    uint32 data;
    int32  ret;
#if 0
    rtl9601b_raw_port_ability_t mac_ability;
#endif
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");


#if 1 /* it should be determined by strap pin */
    /* set the EPON mode enable */
    data = 2;
    if ((ret = reg_write(RTL9601B_PON_MODE_CFGr, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
#endif

    /* set the serdes mode to EPON mode */
    data = 0xC;
    if ((ret = reg_field_write(RTL9601B_SDS1_CFGr, RTL9601B_CFG_SDS1_MODEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

#if 0 /* MODIFICATION needed */
    data = 0x0;
    if ((ret = reg_write(RTL9601B_WSDS_ANA_1Cr, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    /* Serdes amplitude 500 mv */
    data = 0x1164;
    if ((ret = reg_write(RTL9601B_WSDS_ANA_0Er, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
#endif

    /* reset serdes including digital and analog */
    data = 0x0;
    if ((ret = reg_field_write(RTL9601B_WSDS_DIG_00r, RTL9601B_CFG_SFT_RSTBf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    data = 0x1;
    if ((ret = reg_field_write(RTL9601B_WSDS_DIG_00r, RTL9601B_CFG_SFT_RSTBf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    /* BEN on */
    data = 0x1;
    if ((ret = reg_field_write(RTL9601B_WSDS_DIG_18r, RTL9601B_BEN_OEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    
    /*do not accept special packet on pon port*/
    data = 0;
    if ((ret = reg_array_field_write(RTL9601B_P_MISCr, HAL_GET_PON_PORT(), REG_ARRAY_INDEX_NONE, RTL9601B_RX_SPCf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
#if 0
    /* PON port set as force mode */
    {
        osal_memset(&mac_ability, 0x00, sizeof(rtl9601b_raw_port_ability_t));
        mac_ability.speed           = PORT_SPEED_1000M;
        mac_ability.duplex          = PORT_FULL_DUPLEX;
        mac_ability.linkFib1g       = DISABLED;
        mac_ability.linkStatus      = PORT_LINKDOWN;
        mac_ability.txFc            = DISABLED;
        mac_ability.rxFc            = DISABLED;
        mac_ability.nwayAbility     = DISABLED;
        mac_ability.masterMod       = DISABLED;
        mac_ability.nwayFault       = DISABLED;
        mac_ability.lpi_100m        = DISABLED;
        mac_ability.lpi_giga        = DISABLED;
        if((ret = rtl9601b_raw_port_ForceAbility_set(HAL_GET_PON_PORT(), &mac_ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }

        mac_ability.linkStatus      = PORT_LINKUP;
        if((ret = rtl9601b_raw_port_ForceAbility_set(HAL_GET_PON_PORT(), &mac_ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }

        if((ret = reg_field_read(RTL9601B_ABLTY_FORCE_MODEr, RTL9601B_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }

        data |= (0x01 << HAL_GET_PON_PORT());

        if((ret = reg_field_write(RTL9601B_ABLTY_FORCE_MODEr, RTL9601B_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
    }
#endif    
    /*when EPON not registered to OLT only OAM packet can send out from pon port*/
    if((ret = rtk_oam_multiplexerAction_set(HAL_GET_PON_PORT(),OAM_MULTIPLEXER_ACTION_DISCARD)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    
    
    if ((ret = dal_rtl9601b_epon_fecOverhead_set(0xc)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    if ((ret = dal_rtl9601b_epon_churningKeyMode_set(RTK_EPON_CHURNING_BL)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
 
    if ((ret = dal_rtl9601b_epon_reportMode_set(RTK_EPON_REPORT_NORMAL)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    /*init EPON register*/
    
    /*reset EPON*/
    data = 0;
    if ((ret = reg_field_write(RTL9601B_WSDS_DIG_00r,RTL9601B_CFG_SFT_RSTB_EPONf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
        
    /*set laser parameter*/
    data = 34;
    if ((ret = reg_field_write(RTL9601B_LASER_ON_OFF_TIMEr,RTL9601B_LASER_ON_TIMEf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    data = 30;
    if ((ret = reg_field_write(RTL9601B_LASER_ON_OFF_TIMEr,RTL9601B_LASER_OFF_TIMEf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    data = 10;
    if ((ret = reg_field_write(RTL9601B_EPON_ASIC_TIMING_ADJUST2r,RTL9601B_LSR_ON_SHIFTf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    data = 11;
    if ((ret = reg_field_write(RTL9601B_EPON_ASIC_TIMING_ADJUST2r,RTL9601B_LSR_OFF_SHIFTf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    data = 0x28; /*ADJ_BC*/
    if ((ret = reg_field_write(RTL9601B_EPON_ASIC_TIMING_ADJUST2r,RTL9601B_ADJ_BCf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    data = 0x16; /*ADJ RPT_TMG*/
    if ((ret = reg_field_write(RTL9601B_EPON_ASIC_TIMING_ADJUST1r,RTL9601B_RPT_TMGf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    data = 0x1; /*bypass DS FEC*/
    if ((ret = reg_field_write(RTL9601B_EPON_FEC_CONFIGr,RTL9601B_BYPASS_FECf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

#if 0
    /*
    RSVD_PER_PORT_MAC [4][3].RSVD_MEM[7:0] 0x21088 0x55555508
    signal name:  cfg_adj_fec_ovhd
    description  :  adjusting switch upstream FEC overhead
    */ 
    ioal_mem32_write(0x21088, 0x55555508);
#endif
    /*
    RSVD_EGR_SCH[2:0]
    signal name:      gate_len_cmps
    description        adjusting gate length from gate packet:
                        3'd1 : gate length - 1*8
                        3'd2 : gate length - 2*8
                        3'd3 : gate length - 3*8
                        3'd4 : gate length - 4*8
                        3'd5 : gate length - 5*8
                        3'd6 : gate length - 6*8
                        3'd7 : gate length - 7*8
    */
    /*ioal_mem32_write(0x2de54, 0x0);*/
    data = 0;
    if((ret = reg_field_write(RTL9601B_EPON_GATE_CTRLr, RTL9601B_GATE_LENGTHf, &data))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    data = 0x1;
    if ((ret = reg_field_write(RTL9601B_EP_MISCr,RTL9601B_SRT_GNf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    data = 0x1;
    if ((ret = reg_field_write(RTL9601B_EP_MISCr,RTL9601B_ALWAYS_SVYf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
     

    data = 1; /*release EPON reset*/
    if ((ret = reg_field_write(RTL9601B_WSDS_DIG_00r,RTL9601B_CFG_SFT_RSTB_EPONf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }


    /*register mode setting*/
 
        /*register pennding grant set to 4*/
    data = 4;
    if ((ret = reg_field_write(RTL9601B_EPON_RGSTR2r,RTL9601B_REG_PENDDING_GRANTf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
  
        /*register enable random delay*/
    data = 1;
    if ((ret = reg_field_write(RTL9601B_EPON_DEBUG1r,RTL9601B_DIS_RANDOM_DELAY_ENf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

        /*register ack flag fields set to 1*/
    data = 0x1;
    if ((ret = reg_field_write(RTL9601B_EPON_REG_ACKr,RTL9601B_ACK_FLAGf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

  

    /*invalid frame handle set to drop*/
    data = 0;
    if ((ret = reg_field_write(RTL9601B_EPON_DEBUG1r,RTL9601B_MODE0_INVALID_HDLf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    data = 0;
    if ((ret = reg_field_write(RTL9601B_EPON_DEBUG1r,RTL9601B_MODE1_INVALID_HDLf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
  
    /*MPCP Gate frame handle set to asic handle*/
    data = 0x0;
    if ((ret = reg_field_write(RTL9601B_EPON_MPCP_CTRr,RTL9601B_GATE_TRAP_TYPEf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    data = 0x0;
    if ((ret = reg_field_write(RTL9601B_EPON_MPCP_CTRr,RTL9601B_GATE_HANDLEf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    
    /*invalid mpcp packet handle set to drop*/
    data = 0x0;
    if ((ret = reg_field_write(RTL9601B_EPON_MPCP_CTRr,RTL9601B_OTHER_HANDLEf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON| MOD_DAL), "");
        return ret;
    }

    data = 0x0;
    if ((ret = reg_field_write(RTL9601B_EPON_MPCP_CTRr,RTL9601B_INVALID_LEN_HANDLEf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    
    /*decryption mode set to churnning key*/
    data = 0x1;
    if ((ret = reg_field_write(RTL9601B_EPON_DECRYP_CFGr,RTL9601B_EPON_DECRYPf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    /*serdes patch*/
    /*WSDS_ANA_18*/
    ioal_mem32_write(0x22060, 0xa8b2);
    /*WSDS_ANA_1A*/
    ioal_mem32_write(0x22068, 0xdde4);
    /*SDS_REG1*/
    ioal_mem32_write(0x22804, 0x0);

#if 0 /* MODIFICATION needed */
    /*PCS derdes patch*/    
    data = 0x3;
    if ((ret = reg_write(RTL9601B_SDS_EXT_REG12r, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    /*data = 0x4e6a;*/
    data = 0x4eff;
    if ((ret = reg_write(RTL9601B_SDS_EXT_REG13r, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    /*data = 0x1562;*/
    data = 0x1400;
    if ((ret = reg_write(RTL9601B_SDS_EXT_REG15r, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    data = 0xbd2a;
    if ((ret = reg_write(RTL9601B_SDS_EXT_REG16r, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    data = 0x1059;
    if ((ret = reg_write(RTL9601B_SDS_REG7r, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }

    data = 0x3F;
    if ((ret = reg_field_write(RTL9601B_SDS_REG24r,RTL9601B_CFG_FIB2G_TXCFG_NP_P0f,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
#endif

    /*pon mac init*/
    if ((ret = rtk_ponmac_init()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    /*mapping queue 0~7 to schedule id 0*/
    {
        rtk_ponmac_queueCfg_t   queueCfg;
        rtk_ponmac_queue_t logicalQueue;
        uint32 queueId;

        memset(&queueCfg, 0, sizeof(rtk_ponmac_queueCfg_t));

        logicalQueue.schedulerId = 0 ;

        queueCfg.cir       = 0x0;
        queueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX();
        queueCfg.type      = STRICT_PRIORITY;
        queueCfg.egrssDrop = DISABLED;
        for(queueId = 0 ; queueId <= RTK_DOT1P_PRIORITY_MAX ; queueId++ )
        {
            logicalQueue.queueId = queueId;

            if((ret= dal_rtl9601b_ponmac_queue_add(&logicalQueue, &queueCfg)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
        }
    }



    /* function body */
    epon_init = INIT_COMPLETED;
    
    
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_init */




/* Function Name:
 *      dal_rtl9601b_epon_intrMask_get
 * Description:
 *      Get EPON interrupt mask
 * Input:
 *	  intrType: type of interrupt
 * Output:
 *      pState: point of get interrupt state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_intrMask_get(rtk_epon_intrType_t intrType, rtk_enable_t *pState)
{
	uint32  tmpVal;   
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "intrType=%d",intrType);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((EPON_INTR_END <=intrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    switch(intrType)
    {
        case EPON_INTR_TIMEDRIFT:
            if ((ret = reg_field_read(RTL9601B_EPON_INTRr,RTL9601B_TIME_DRIFT_IMRf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            break;    
        case EPON_INTR_MPCPTIMEOUT:
            if ((ret = reg_field_read(RTL9601B_EPON_INTRr,RTL9601B_MPCP_TIMEOUT_IMRf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            break;    
        case EPON_INTR_REG_FIN:
            if ((ret = reg_field_read(RTL9601B_EPON_INTRr,RTL9601B_REG_COMPLETE_IMSf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            break; 
        case EPON_INTR_LOS:
            if ((ret = reg_field_read(RTL9601B_EPON_INTRr,RTL9601B_LOS_IMSf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            break; 

        default:
            return RT_ERR_INPUT;       
        
    }
    
    if(tmpVal==0)
        *pState = DISABLED;    
    else
        *pState = ENABLED;

       
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_intrMask_get */


/* Function Name:
 *      dal_rtl9601b_epon_intrMask_set
 * Description:
 *      Set EPON interrupt mask
 * Input:
 * 	    intrType: type of top interrupt
 *      state: set interrupt mask state
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_intrMask_set(rtk_epon_intrType_t intrType, rtk_enable_t state)
{
	uint32  tmpVal;   
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "intrType=%d,state=%d",intrType, state);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((EPON_INTR_END <=intrType), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if(state==ENABLED)
        tmpVal = 1;    
    else
        tmpVal = 0;
        
    switch(intrType)
    {
        case EPON_INTR_TIMEDRIFT:
            if ((ret = reg_field_write(RTL9601B_EPON_INTRr,RTL9601B_TIME_DRIFT_IMRf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            break;    
        case EPON_INTR_MPCPTIMEOUT:
            if ((ret = reg_field_write(RTL9601B_EPON_INTRr,RTL9601B_MPCP_TIMEOUT_IMRf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            break;    
        case EPON_INTR_REG_FIN:
            if ((ret = reg_field_write(RTL9601B_EPON_INTRr,RTL9601B_REG_COMPLETE_IMRf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            break; 
        case EPON_INTR_LOS:
            if ((ret = reg_field_write(RTL9601B_EPON_INTRr,RTL9601B_LOS_IMSf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
            break; 
        default:
            return RT_ERR_INPUT;       
        
    }
     
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_intrMask_set */




/* Function Name:
 *      dal_rtl9601b_epon_intr_get
 * Description:
 *      Set EPON interrupt state
 * Input:
 * 	  intrType: type of interrupt
 * Output:
 *      pState: point for get  interrupt state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_intr_get(rtk_epon_intrType_t intrType, rtk_enable_t *pState)
{
 	int32   ret;
    uint32  field,tmp_val;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "intrType=%d",intrType);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((EPON_INTR_END <=intrType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    switch(intrType)
    {
        case EPON_INTR_TIMEDRIFT:
            field = RTL9601B_TIME_DRIFT_IMSf; 
            break;    
        case EPON_INTR_MPCPTIMEOUT:
            field = RTL9601B_MPCP_TIMEOUT_IMSf; 
            break;    
        case EPON_INTR_REG_FIN:
            field = RTL9601B_REG_COMPLETE_IMSf; 
            break; 
        case EPON_INTR_LOS:
            field = RTL9601B_LOS_IMSf; 
            break; 
        default:
            return RT_ERR_INPUT;       
        
    }


    if ((ret = reg_field_read(RTL9601B_EPON_INTRr,field,&tmp_val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(tmp_val == 0)
    {
        tmp_val = 1;
        if ((ret = reg_field_write(RTL9601B_EPON_INTRr,field,&tmp_val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
            return ret;
        }
    }


    
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_gpon_intr_get */


/* Function Name:
 *      dal_rtl9601b_epon_intr_disableAll
 * Description:
 *      Disable all of top interrupt for EPON
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_intr_disableAll(void)  
{
	uint32  tmpVal;   
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    tmpVal = 0;
    
    /* function body */
    if ((ret = reg_field_write(RTL9601B_EPON_INTRr,RTL9601B_TIME_DRIFT_IMRf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9601B_EPON_INTRr,RTL9601B_MPCP_TIMEOUT_IMRf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9601B_EPON_INTRr,RTL9601B_REG_COMPLETE_IMSf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9601B_EPON_INTRr,RTL9601B_LOS_IMSf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_intr_disableAll */



/* Function Name:
 *      dal_rtl9601b_epon_llid_entry_set
 * Description:
 *      Set llid entry
 * Input:
 *      llidIdx: LLID table index
 * Output:
 *      pLlidEntry: LLID entry 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_llid_entry_set(rtk_epon_llid_entry_t *pLlidEntry)  
{
    rtl9601b_raw_epon_llid_table_t  rawLlidEntry;
 	int32   ret;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLlidEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pLlidEntry->llidIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pLlidEntry->valid), RT_ERR_INPUT);

    /* function body */
    rawLlidEntry.llid                = pLlidEntry->llid;
    rawLlidEntry.report_timer        = pLlidEntry->reportTimer;
    if(ENABLED == pLlidEntry->valid )
        rawLlidEntry.valid = 1;
    else
        rawLlidEntry.valid = 0;

    if ((ret = rtl9601b_raw_epon_llidTable_set(pLlidEntry->llidIdx,&rawLlidEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_llid_entry_set */

/* Function Name:
 *      dal_rtl9601b_epon_llid_entry_get
 * Description:
 *      Get llid entry
 * Input:
 *      llidIdx: LLID table index
 * Output:
 *      pLlidEntry: LLID entry 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_llid_entry_get(rtk_epon_llid_entry_t *pLlidEntry)  
{
    rtl9601b_raw_epon_llid_table_t  rawLlidEntry;
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLlidEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pLlidEntry->llidIdx), RT_ERR_INPUT);
    
    
    /* function body */
    if ((ret = rtl9601b_raw_epon_llidTable_get(pLlidEntry->llidIdx,&rawLlidEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    pLlidEntry->llid        = rawLlidEntry.llid;
    pLlidEntry->reportTimer = rawLlidEntry.report_timer;

    if(1 == rawLlidEntry.valid)
        pLlidEntry->valid = ENABLED;
    else
        pLlidEntry->valid = DISABLED;
    
    if(1 == rawLlidEntry.is_report_timeout)
        pLlidEntry->isReportTimeout = ENABLED;
    else
        pLlidEntry->isReportTimeout = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_llid_entry_get */


/* Function Name:
 *      dal_rtl9601b_epon_forceLaserState_set
 * Description:
 *      Set Force Laser status
 * Input:
 *      laserStatus: Force laser status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  dal_rtl9601b_epon_forceLaserState_set(rtk_epon_laser_status_t laserStatus)
{
 	int32   ret;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "laserStatus=%d",laserStatus);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EPON_LASER_STATUS_END <=laserStatus), RT_ERR_INPUT);

    /* function body */
    if(RTK_EPON_LASER_STATUS_NORMAL==laserStatus)
    {
        /*set force laser on disable*/
        if ((ret = rtl9601b_raw_epon_forceLaserOn_set(DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
    }
    if(RTK_EPON_LASER_STATUS_FORCE_ON==laserStatus)
    {
        /*set force laser on enable*/
        if ((ret = rtl9601b_raw_epon_forceLaserOn_set(ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
    }
    if(RTK_EPON_LASER_STATUS_FORCE_OFF==laserStatus)
    {
        /*set force laser on enable*/
        if ((ret = rtl9601b_raw_epon_forceLaserOn_set(ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
    }

    forceLaserState = laserStatus;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_forceLaserState_set */

/* Function Name:
 *      dal_rtl9601b_epon_forceLaserState_get
 * Description:
 *      Get Force Laser status
 * Input:
 *      None
 * Output:
 *      pLaserStatus: Force laser status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_forceLaserState_get(rtk_epon_laser_status_t *pLaserStatus)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLaserStatus), RT_ERR_NULL_POINTER);

    /* function body */
    *pLaserStatus=forceLaserState;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_forceLaserState_get */


/* Function Name:
 *      dal_rtl9601b_epon_laserTime_set
 * Description:
 *      Set laserTime value
 * Input:
 *      laserOnTime:  OE module laser on time
 *      laserOffTime: OE module laser off time
 * Output:
 *      None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  dal_rtl9601b_epon_laserTime_set(uint8 laserOnTime, uint8 laserOffTime)
{
 	int32   ret;
    uint32  val;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "laserOnTime=%d,laserOffTime=%d",laserOnTime, laserOffTime);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_EPON_MAX_LASER_ON_TIME <=laserOnTime), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EPON_MAX_LASER_OFF_TIME <=laserOffTime), RT_ERR_INPUT);
    val = laserOnTime;
    /* function body */
    if ((ret = reg_field_write(RTL9601B_LASER_ON_OFF_TIMEr,RTL9601B_LASER_ON_TIMEf,&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    val = laserOffTime;
    if ((ret = reg_field_write(RTL9601B_LASER_ON_OFF_TIMEr,RTL9601B_LASER_OFF_TIMEf,&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }


    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_laserTime_set */

/* Function Name:
 *      dal_rtl9601b_epon_laserTime_get
 * Description:
 *      Get laser Time value
 * Input:
 *      None
 * Output:
 *      pLasetOnTime:  OE module laser on time
 *      pLasetOffTime: OE module laser off time
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_laserTime_get(uint8 *pLaserOnTime, uint8 *pLaserOffTime)
{
	uint32  tmpVal;   
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON),"");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLaserOnTime), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pLaserOffTime), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_write(RTL9601B_LASER_ON_OFF_TIMEr,RTL9601B_LASER_ON_TIMEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    *pLaserOnTime = tmpVal;
    
    if ((ret = reg_field_write(RTL9601B_LASER_ON_OFF_TIMEr,RTL9601B_LASER_OFF_TIMEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    *pLaserOffTime = tmpVal;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_laserTime_get */

/* Function Name:
 *      dal_rtl9601b_epon_syncTime_get
 * Description:
 *      Get sync Time value
 * Input:
 *      None
 * Output:
 *      pSyncTime  : olt assigned sync time
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_syncTime_get(uint8 *pSyncTime)
{
	uint32  tmpVal;   
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSyncTime), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(RTL9601B_SYNC_TIMEr,RTL9601B_NORMAL_SYNC_TIMEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_syncTime_get */


/* Function Name:
 *      dal_rtl9601b_epon_registerReq_get
 * Description:
 *      Get register request relative parameter
 * Input:
 *      pRegEntry : register request relative parament
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_registerReq_get(rtk_epon_regReq_t *pRegEntry)
{
	uint32  tmpVal;   
 	int32   ret;
    rtk_enable_t enable;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegEntry), RT_ERR_NULL_POINTER);

    /* function body */

    if ((ret = rtl9601b_raw_epon_regLlidIdx_get(&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    pRegEntry->llidIdx = tmpVal;

    
    if ((ret = rtl9601b_raw_epon_llidIdxMac_get(pRegEntry->llidIdx,&(pRegEntry->mac))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    
    if ((ret = rtl9601b_raw_epon_regPendingGrantNum_get(&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    pRegEntry->pendGrantNum = tmpVal;

    if ((ret = rtl9601b_raw_epon_regReguest_get(&enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    pRegEntry->doRequest = enable;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_registerReq_get */

/* Function Name:
 *      dal_rtl9601b_epon_registerReq_set
 * Description:
 *      Set register request relative parameter
 * Input:
 *       None
 * Output:
 *       pRegEntry : register request relative parament 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_registerReq_set(rtk_epon_regReq_t *pRegEntry)
{
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pRegEntry->llidIdx), RT_ERR_INPUT);

    /* function body */
    if ((ret = rtl9601b_raw_epon_regLlidIdx_set(pRegEntry->llidIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }
    
    if ((ret = rtl9601b_raw_epon_llidIdxMac_set(pRegEntry->llidIdx, &(pRegEntry->mac))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    if ((ret = rtl9601b_raw_epon_regPendingGrantNum_set(pRegEntry->pendGrantNum)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    if ((ret = rtl9601b_raw_epon_regReguest_set(pRegEntry->doRequest)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_registerReq_set */



/* Function Name:
 *      dal_rtl9601b_epon_churningKey_set
 * Description:
 *      Set churning key entry
 * Input:
 *       pEntry : churning key relative parameter 
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_churningKey_set(rtk_epon_churningKeyEntry_t *pEntry)
{
 	int32   ret;
    uint8   pKeyTmp[4];

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((2 <= pEntry->keyIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pEntry->llidIdx), RT_ERR_INPUT);

    pKeyTmp[0] = 0;
    pKeyTmp[1] = pEntry->churningKey[0];
    pKeyTmp[2] = pEntry->churningKey[1];
    pKeyTmp[3] = pEntry->churningKey[2];
    
    if(0==pEntry->keyIdx)
    {
        if ((ret = reg_array_field_write(RTL9601B_EPON_DECRYP_KEY0r, REG_ARRAY_INDEX_NONE, pEntry->llidIdx, RTL9601B_EPON_DECRYP_KEY0f, (uint32 *)pKeyTmp)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_array_field_write(RTL9601B_EPON_DECRYP_KEY1r, REG_ARRAY_INDEX_NONE, pEntry->llidIdx, RTL9601B_EPON_DECRYP_KEY1f, (uint32 *)pKeyTmp)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_churningKey_set */


/* Function Name:
 *      dal_rtl9601b_epon_churningKey_get
 * Description:
 *      Get churning key entry
 * Input:
 *       None
 * Output:
 *       pEntry : churning key relative parameter 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_churningKey_get(rtk_epon_churningKeyEntry_t *pEntry)
{
	uint32  tmpVal;   
    uint8   *pTmpPtr;
 	int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((2 <= pEntry->keyIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pEntry->llidIdx), RT_ERR_INPUT);

    if(0==pEntry->keyIdx)
    {
        if ((ret = reg_array_field_read(RTL9601B_EPON_DECRYP_KEY0r, pEntry->llidIdx, REG_ARRAY_INDEX_NONE, RTL9601B_EPON_DECRYP_KEY0f, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_array_field_read(RTL9601B_EPON_DECRYP_KEY1r, pEntry->llidIdx, REG_ARRAY_INDEX_NONE, RTL9601B_EPON_DECRYP_KEY1f, (uint32 *)&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }
    }
    pTmpPtr = (uint8 *)&tmpVal;
    pEntry->churningKey[0] = pTmpPtr[0];
    pEntry->churningKey[1] = pTmpPtr[1];
    pEntry->churningKey[2] = pTmpPtr[2];
    
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_churningKey_set */


/* Function Name:
 *      dal_rtl9601b_epon_usFecState_get
 * Description:
 *      Get upstream fec state
 * Input:
 *       None
 * Output:
 *       *pState : upstream FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_usFecState_get(rtk_enable_t *pState)
{
 	int32   ret;
	uint32  tmpVal;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(RTL9601B_EPON_FEC_CONFIGr,RTL9601B_FEC_US_ENf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(1==tmpVal)
        *pState = ENABLED;
    else
        *pState = DISABLED;
   
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_usFecState_get */


/* Function Name:
 *      dal_rtl9601b_epon_usFecState_set
 * Description:
 *      Set upstream fec state
 * Input:
 *       state : upstream FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_usFecState_set(rtk_enable_t state)
{
 	int32   ret;
	uint32  tmpVal,gateLenAdj;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "state=%d",state);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if(ENABLED==state)
    {
        gateLenAdj = 6;
        tmpVal =1;
    }
    else
    {
        gateLenAdj = 0;
        tmpVal =0;
    }    
    if ((ret = reg_field_write(RTL9601B_EPON_FEC_CONFIGr,RTL9601B_FEC_US_ENf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

#if 0 /* MODIFICATION needed */
    if ((ret = reg_field_write(RTL9601B_EP_MISCr,RTL9601B_FEC_ENABLEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
#endif

    if ((ret = reg_field_write(RTL9601B_EPON_GATE_CTRLr,RTL9601B_GATE_LENGTHf,&gateLenAdj)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
 
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_usFecState_set */


/* Function Name:
 *      dal_rtl9601b_epon_dsFecState_get
 * Description:
 *      Get down-stream fec state
 * Input:
 *       None
 * Output:
 *       *pState : down-stream FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_dsFecState_get(rtk_enable_t *pState)
{
 	int32   ret;
	uint32  tmpVal;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(RTL9601B_EPON_FEC_CONFIGr,RTL9601B_BYPASS_FECf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    if(0==tmpVal)
        *pState = ENABLED;
    else
        *pState = DISABLED;
    
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_dsFecState_get */


/* Function Name:
 *      dal_rtl9601b_epon_dsFecState_set
 * Description:
 *      Set down-stream fec state
 * Input:
 *       state : down-stream FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_dsFecState_set(rtk_enable_t state)
{
 	int32   ret;
	uint32  tmpVal,recoverFec,data;
	
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "state=%d",state);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    if(ENABLED==state)
    {
        tmpVal =0;
        recoverFec = 1;
    }
    else
    {
        tmpVal =1;
        recoverFec = 0;
    }
    
    /*reset EPON*/
    data = 0;
    if ((ret = reg_field_write(RTL9601B_WSDS_DIG_00r,RTL9601B_CFG_SFT_RSTB_EPONf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    
    if ((ret = reg_field_write(RTL9601B_EPON_FEC_CONFIGr,RTL9601B_BYPASS_FECf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_write(RTL9601B_EPON_FEC_CONFIGr,RTL9601B_FEC_RECOVERf,&recoverFec)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    
    
    data = 1;
    if ((ret = reg_field_write(RTL9601B_WSDS_DIG_00r,RTL9601B_CFG_SFT_RSTB_EPONf,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
        return ret;
    }
    
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_dsFecState_set */


/* Function Name:
 *      dal_rtl9601b_epon_dsFecState_set
 * Description:
 *      Set down-stream fec state
 * Input:
 *       None
 * Output:
 *       pCounter : EPON mib counter 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_mibCounter_get(rtk_epon_counter_t *pCounter)
{
 	int32   ret;
	uint32  tmpVal,queueId;   

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");


    /* parameter check */
    RT_PARAM_CHK((NULL == pCounter), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pCounter->llidIdx), RT_ERR_INPUT);

    /* function body */
    /*get global counter*/
    if ((ret = reg_field_read(RTL9601B_DOT3_MPCP_RX_DISCr,RTL9601B_DOT3MPCPRXDISCOVERYGATEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    
    pCounter->mpcpRxDiscGate=tmpVal;

    if ((ret = reg_field_read(RTL9601B_DOT3_EPON_FEC_UNCORRECTED_BLOCKSr,RTL9601B_DOT3EPONFECUNCORRECTABLEBLOCKSf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    
    pCounter->fecUncorrectedBlocks=tmpVal;

    if ((ret = reg_field_read(RTL9601B_DOT3_EPON_FEC_CORRECTED_BLOCKSr,RTL9601B_DOT3EPONFECCORRECTEDBLOCKSf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    
    pCounter->fecCorrectedBlocks=tmpVal;    

    if ((ret = reg_field_read(RTL9601B_DOT3_EPON_FEC_CODING_VIOr,RTL9601B_FECPCSCODINGVIOLATIONf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    
    pCounter->fecCodingVio=tmpVal;

    if ((ret = reg_field_read(RTL9601B_DOT3_NOT_BROADCAST_BIT_NOT_ONU_LLIDr,RTL9601B_NOTBROADCASTBITNOTONULLIDf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    
    pCounter->notBcstBitNotOnuLlid=tmpVal;

    if ((ret = reg_field_read(RTL9601B_DOT3_BROADCAST_BIT_PLUS_ONU_LLIDr,RTL9601B_BROADCASTBITPLUSONULLIDf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    
    pCounter->bcstBitPlusOnuLLid=tmpVal;

    if ((ret = reg_field_read(RTL9601B_DOT3_BROADCAST_NOT_ONUIDr,RTL9601B_BROADCASTBITNOTONULLIDf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    
    pCounter->bcstNotOnuLLid=tmpVal;

    if ((ret = reg_field_read(RTL9601B_DOT3_CRC8_ERRORSr,RTL9601B_CRC8ERRORSf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    
    pCounter->crc8Err=tmpVal;

    if ((ret = reg_field_read(RTL9601B_DOT3_MPCP_TX_REG_REQr,RTL9601B_DOT3MPCPTXREGREQUESTf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    
    pCounter->mpcpTxRegRequest=tmpVal;
    
    if ((ret = reg_field_read(RTL9601B_DOT3_MPCP_TX_REG_ACKr,RTL9601B_DOT3MPCPTXREGACKf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    
    pCounter->mpcpTxRegAck=tmpVal;
        
    /*get llid counter*/
    if ((ret = reg_array_field_read(RTL9601B_DOT3_MPCP_TX_REPORTr, REG_ARRAY_INDEX_NONE, pCounter->llidIdx, RTL9601B_DOT3MPCPTXREPORTf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    pCounter->llidIdxCnt.mpcpTxReport=tmpVal;

    if ((ret = reg_array_field_read(RTL9601B_DOT3_MPCP_EX_GATEr, REG_ARRAY_INDEX_NONE, pCounter->llidIdx, RTL9601B_DOT3MPCPRXGATEf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    pCounter->llidIdxCnt.mpcpRxGate=tmpVal;

    if ((ret = reg_array_field_read(RTL9601B_DOT3_ONUID_NOT_BROADCASTr, REG_ARRAY_INDEX_NONE, pCounter->llidIdx, RTL9601B_ONULLIDNOTBROADCASTf, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    
    pCounter->llidIdxCnt.onuLlidNotBcst=tmpVal;
    /*LLID Queue basic counter*/
    
    for(queueId=0;queueId<RTK_EPON_MAX_QUEUE_PER_LLID;queueId++)
    {
        if ((ret = reg_array_field_read(RTL9601B_DOT3_Q_TX_FRAMESr, REG_ARRAY_INDEX_NONE, (pCounter->llidIdx)*8+queueId, RTL9601B_DOT3EXTPKGSTATTXFRAMESQUEUEf, &tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
            return ret;
        }    
        pCounter->llidIdxCnt.queueTxFrames[queueId]=tmpVal;        
    }    
    
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_mibCounter_get */



/* Function Name:
 *      dal_rtl9601b_epon_mibGlobal_reset
 * Description:
 *      Reset EPON global counters.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not reset Global Counter
 * Note:
 *      None
 */
int32
dal_rtl9601b_epon_mibGlobal_reset(void)
{
 	int32   ret;
	uint32  tmpVal;   

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* parameter check */
    
    /* function body */
	tmpVal = 1;
    if ((ret = reg_field_write(RTL9601B_EPON_STAT_RSTr,RTL9601B_RST_EPON_MIBf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    /*trigger mib reset*/
	tmpVal = 1;
    if ((ret = reg_field_write(RTL9601B_EPON_STAT_RSTr,RTL9601B_RST_CMDf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    /*check busy flag*/
    do{
        if ((ret = reg_field_write(RTL9601B_STAT_RSTr, RTL9601B_RST_STATf, &tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
            return ret;
        }
    }while(tmpVal == 0);
    
    /*set to default*/
	tmpVal = 0;
    if ((ret = reg_field_write(RTL9601B_EPON_STAT_RSTr,RTL9601B_RST_EPON_MIBf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_mibGlobal_reset */


/* Function Name:
 *      dal_rtl9601b_epon_mibLlidIdx_reset
 * Description:
 *      Reset the specified LLID index counters.
 * Input:
 *      llidIdx - LLID table index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9601b_epon_mibLlidIdx_reset(uint8 llidIdx)
{
 	int32   ret;
	uint32  tmpVal;   

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "llidIdx=%d",llidIdx);

    /* parameter check */
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <=llidIdx), RT_ERR_INPUT);

    /* function body */
	tmpVal = 1;
    if ((ret = reg_field_write(RTL9601B_EPON_STAT_RSTr,RTL9601B_RST_LLIDf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

	tmpVal = llidIdx;
    if ((ret = reg_field_write(RTL9601B_EPON_STAT_RSTr,RTL9601B_RST_LLID_IDXf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    
    /*trigger mib reset*/
	tmpVal = 1;
    if ((ret = reg_field_write(RTL9601B_EPON_STAT_RSTr,RTL9601B_RST_CMDf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    /*check busy flag*/
    do{
        if ((ret = reg_field_write(RTL9601B_STAT_RSTr, RTL9601B_RST_STATf, &tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
            return ret;
        }
    }while(tmpVal == 0);
    
    /*set to default*/
	tmpVal = 0;
    if ((ret = reg_field_write(RTL9601B_EPON_STAT_RSTr,RTL9601B_RST_EPON_MIBf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_mibLlidIdx_reset */


int32
dal_rtl9601b_epon_churningKeyMode_set(rtk_epon_churning_mode_t mode)
{
 	int32   ret;
	uint32  tmpVal;   

    /* parameter check */
    RT_PARAM_CHK((RTK_EPON_CHURNING_END <=mode), RT_ERR_INPUT);

    /* function body */
	if(RTK_EPON_CHURNING_BL == mode)
	    tmpVal = 1;
    else
	    tmpVal = 0;
    
    if ((ret = reg_field_write(RTL9601B_EPON_DEBUG1r,RTL9601B_CHURN_MODEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
    
}    



int32
dal_rtl9601b_epon_fecOverhead_set(uint32 val)
{
 	int32   ret;
	uint32  tmpVal;   

    /* parameter check */
    RT_PARAM_CHK((64 <= val), RT_ERR_INPUT);

    /* function body */
    tmpVal = val;
    if ((ret = reg_field_write(RTL9601B_EPON_FEC_CONFIGr,RTL9601B_FEC_OVER_TXf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
}    




int32
dal_rtl9601b_epon_reportMode_set(rtk_epon_report_mode_t mode)
{
 	//int32   ret; /* MODIFICATION needed */
	//uint32  tmpVal; /* MODIFICATION needed */

    /* parameter check */
    RT_PARAM_CHK((RTK_EPON_REPORT_END <= mode), RT_ERR_INPUT);

#if 0 /* MODIFICATION needed */
    /* function body */
    switch(mode)
    {
        case RTK_EPON_REPORT_NORMAL:
            tmpVal = 1;
            if ((ret = reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_REPORT_LENGTH_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 0;
            if ((ret = reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_STATIC_REPORT_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 0;
            if ((ret = reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_FORCE_REPORT_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            break;
        case RTK_EPON_REPORT_0_F:
            tmpVal = 0;
            if ((ret = reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_REPORT_LENGTH_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 1;
            if ((ret = reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_STATIC_REPORT_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 0;
            if ((ret = reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_FORCE_REPORT_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            break;
        case RTK_EPON_REPORT_FORCE_0:
            tmpVal = 0;
            if ((ret = reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_REPORT_LENGTH_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 0;
            if ((ret = reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_STATIC_REPORT_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 1;
            if ((ret = reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_FORCE_REPORT_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 0;
            if ((ret = reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_FORCE_REPORT_CONTENTf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            break;
        case RTK_EPON_REPORT_FORCE_F:
            tmpVal = 0;
            if ((ret = reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_REPORT_LENGTH_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 0;
            if ((ret = reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_STATIC_REPORT_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 1;
            if ((ret = reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_FORCE_REPORT_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            tmpVal = 1;
            if ((ret = reg_field_write(RTL9601B_EPON_REPORT_MODEr,RTL9601B_FORCE_REPORT_CONTENTf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;         
    }
#endif

    return RT_ERR_OK;
}    





int32
dal_rtl9601b_epon_reportMode_get(rtk_epon_report_mode_t *pMode)
{
 	//int32   ret; /* MODIFICATION needed */
	//uint32  tmpVal; /* MODIFICATION needed */

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);
    *pMode = RTK_EPON_REPORT_END;

#if 0 /* MODIFICATION needed */
    /* function body */
    if ((ret = reg_field_read(RTL9601B_EPON_REPORT_MODEr,RTL9601B_FORCE_REPORT_MODEf,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }

    if(1==tmpVal)
    {/*force mode*/
        if ((ret = reg_field_read(RTL9601B_EPON_REPORT_MODEr,RTL9601B_FORCE_REPORT_CONTENTf,&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
            return ret;
        }
        if(1==tmpVal)/*forcee to 0xFFFF*/
        {
            *pMode = RTK_EPON_REPORT_FORCE_F;
        }    
        else
        {
            *pMode = RTK_EPON_REPORT_FORCE_0;
        }
    }
    else
    {
        if ((ret = reg_field_read(RTL9601B_EPON_REPORT_MODEr,RTL9601B_REPORT_LENGTH_MODEf,&tmpVal)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
            return ret;
        }        
        if(1==tmpVal)
        {
            *pMode = RTK_EPON_REPORT_NORMAL;    
        }
        else
        {
            if ((ret = reg_field_read(RTL9601B_EPON_REPORT_MODEr,RTL9601B_STATIC_REPORT_MODEf,&tmpVal)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
                return ret;
            }
            if(1==tmpVal)
            {
                *pMode = RTK_EPON_REPORT_0_F;    
            }                              
        }
    }
#endif
    return RT_ERR_OK;
}    


/* Function Name:
 *      dal_rtl9601b_epon_mpcpTimeoutVal_get
 * Description:
 *      Get mpcp time out value.
 * Input:
 *      pTimeVal - pointer of mpcp tomeout value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9601b_epon_mpcpTimeoutVal_get(uint32 *pTimeVal)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimeVal), RT_ERR_NULL_POINTER);

    /* function body */

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_mpcpTimeoutVal_get */



/* Function Name:
 *      dal_rtl9601b_epon_mpcpTimeoutVal_set
 * Description:
 *      Set mpcp time out value.
 * Input:
 *      timeVal - mpcp tomeout value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9601b_epon_mpcpTimeoutVal_set(uint32 timeVal)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "timeVal=%d",timeVal);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((0x80 <=timeVal), RT_ERR_INPUT);

    /* function body */

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_mpcpTimeoutVal_set */


#if 0
/* Function Name:
 *      dal_rtl9601b_epon_fecState_get
 * Description:
 *      Get EPON global fec state
 * Input:
 *       None
 * Output:
 *       *pState : glogal FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_fecState_get(rtk_enable_t *pState)
{
 	//int32   ret; /* MODIFICATION needed */
	uint32  tmpVal;   

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);
    

#if 0 /* MODIFICATION needed */
    /* function body */
    tmpVal=0;
    if ((ret = reg_field_read(RTL9601B_SDS_REG25r,RTL9601B_CFG_FIB2G_TXCFG_NP_P1f,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
#endif
    
    /*bit 1 is PCS FEC mode*/
    if(tmpVal & 0x2)
    {
        *pState=ENABLED;    
    }
    else
    {
        *pState=DISABLED;    
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_fecState_get */


/* Function Name:
 *      dal_rtl9601b_epon_fecState_set
 * Description:
 *      Set EPON global fec state
 * Input:
 *       state : global FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rtl9601b_epon_fecState_set(rtk_enable_t state)
{
 	//int32   ret; /* MODIFICATION needed */
	uint32  tmpVal;   

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

#if 0 /* MODIFICATION needed */
    /* function body */
    tmpVal=0;
    if ((ret = reg_field_read(RTL9601B_SDS_REG25r,RTL9601B_CFG_FIB2G_TXCFG_NP_P1f,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
#endif

    if(ENABLED == state)
    {
        tmpVal = tmpVal | 0x2;        
    }
    else
    {
        tmpVal = tmpVal & (uint32)(~0x00000002);        
    }
    
#if 0 /* MODIFICATION needed */
    if ((ret = reg_field_write(RTL9601B_SDS_REG25r,RTL9601B_CFG_FIB2G_TXCFG_NP_P1f,&tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
#endif

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_epon_fecState_set */
#endif

