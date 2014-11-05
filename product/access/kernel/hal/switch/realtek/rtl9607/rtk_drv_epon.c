/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
    extern "C"{
#endif
#include <linux/kernel.h>
#include <linux/delay.h>
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include <linux/string.h>
#include "hal_common.h"
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <linux/slab.h>

#include <common/error.h>
#include <common/rt_error.h>
#include <rtk/epon.h>
#include <rtk/ponmac.h>
#include <rtk/l2.h>


DRV_RET_E Hal_SetFecMode(UINT32 BothState)
{
	int32 iRv = RT_ERR_OK;
	rtk_enable_t enable = DISABLED;
	
	if(!((BothState == DISABLED) || (BothState == ENABLED)))
	{
		return DRV_INVALID_VALUE;
	}

	switch (BothState)
    {
        case DISABLED:
            enable = DISABLED;
            break;
        case ENABLED:
            enable = ENABLED;
            break;
        default:
            return DRV_INVALID_VALUE;
    }
	
	if((iRv = rtk_epon_usFecState_set(enable)) != RT_ERR_OK)
    {      
       return ErrorSdk2Drv(iRv);
    }
	
/*begin modified by wanghuanyu for 190 */
//	iRv = rtk_epon_dsFecState_set(enable);
	
/*end modified by wanghuanyu for 190 */
	return ErrorSdk2Drv(iRv);
}

DRV_RET_E Hal_GetFecMode(UINT32 *pBothState)
{
	int32 iRv = RT_ERR_OK;
	rtk_enable_t usState;
	rtk_enable_t dsState;
	
	if(pBothState == NULL)
    {
        return DRV_ERR_PARA;
    }
	
	if((iRv = rtk_epon_usFecState_get(&usState)) != RT_ERR_OK)
    {      
       return ErrorSdk2Drv(iRv);
    }
/*begin modified by wanghuanyu for 190 */
	#if 0	
	if((iRv = rtk_epon_dsFecState_get(&dsState)) != RT_ERR_OK)
    {      
       return ErrorSdk2Drv(iRv);
    }
	#endif
	#if 0
	if(usState != dsState)
    {
        *pBothState = RTK_ENABLE_END;
    }
	else
	{
	#endif
		if(ENABLED == usState)
        {
            *pBothState = ENABLED;
        }
        else
        {
            *pBothState = DISABLED;
        }
	#if 0
	}
	#endif
	
  /*end modified by wanghuanyu for 190 */
	return ErrorSdk2Drv(iRv);
}


/*Begin add by huangmingjian 2013-08-27*/
DRV_RET_E Hal_GetEponMibCounter(rtk_epon_counter_app_t *pCounter)
{	
	return rtk_epon_mibCounter_get((rtk_epon_counter_t *)pCounter);	
}

DRV_RET_E Hal_EponMibGlobal_Reset()
{
	return rtk_epon_mibGlobal_reset();	
}

DRV_RET_E Hal_GetPonmacTransceiver(rtk_transceiver_parameter_type_app_t type, rtk_transceiver_data_app_t *pData)
{	
	return rtk_ponmac_transceiver_get((rtk_transceiver_parameter_type_t)type, (rtk_transceiver_data_t *)pData);	
}

/*End add by huangmingjian 2013-08-27*/

/*Begin add by huangmingjian 2013-09-24*/
DRV_RET_E Hal_LookupMissFloodPortMaskSet(UINT32 type, rtk_portmask_t *pFlood_portmask)
{
	return rtk_l2_lookupMissFloodPortMask_set(type, pFlood_portmask);
}
/*End add by huangmingjian 2013-09-24*/

/*add by an for set epon laser state*/
DRV_RET_E Hal_SetPonLaserState(UINT32 state)
{
    return rtk_epon_forceLaserState_set(state);
}


