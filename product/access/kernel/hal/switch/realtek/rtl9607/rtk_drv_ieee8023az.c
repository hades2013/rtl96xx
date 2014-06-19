/*****************************************************************************
*****************************************************************************/
#ifdef  __cplusplus
    extern "C"{
#endif
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include "hal_common.h"
/*realtek sdk include*/
//#include <common/rt_error.h>
//#include "dal/esw/dal_esw_led.h"
//#include <osal/time.h>
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include "rtk_api.h"
#include "rtk_api_ext.h"
#include "rtl8367b_asicdrv_green.h"

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 宏定义                                       *
*----------------------------------------------*/


/*****************************************************************************
    Func Name:  Hal_SetEEEByPort
  Description:  set ieee802.3az disable or enable by port mask
        Input:  plportMask
                enable
       Output: 
       Return:  DRV_ERR_PARA
                DRV_ERR_UNKNOW
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetEEEByPort(logic_pmask_t *plportMask, BOOL enable)
{
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 unit;
#endif
    port_num_t lport;
    phyid_t phyid;
    INT32 retVal = RT_ERR_OK;
    UINT32 uiVal = 0;

    if(plportMask == NULL)
    {
        return DRV_ERR_PARA;
    }
    IfLgcMaskSet(plportMask, lport)
    {
        phyid = PortLogic2PhyID(lport);
        (VOID) Hal_GetPortLinkupType(phyid, &uiVal);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
        unit = CHIPID(phyid);
    #endif
        
        if(uiVal != HAL_PORT_LINKUP_TYPE_FIBER_UP)
        {
            if(enable == TRUE)
            {
                
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
                retVal += rtk_eee_portEnable_set(unit, PORTID(phyid), ENABLED);
#elif defined(CHIPSET_RTL8305)
                retVal += rtk_eee_portEnable_set(PORTID(phyid), ENABLED);     
#endif
            }
            else
            {
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
                retVal += rtk_eee_portEnable_set(unit, PORTID(phyid), DISABLED);
#elif defined(CHIPSET_RTL8305)
                retVal += rtk_eee_portEnable_set(PORTID(phyid), DISABLED);
#endif
            }
        }
    }
    if(retVal != RT_ERR_OK)
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetAutoPwrDwnByPort
  Description:  set green ethernet disable or enable
        Input:  plportMask
                enable
       Output: 
       Return:  DRV_ERR_PARA
                DRV_ERR_UNKNOW
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetAutoPwrDwnByPort(logic_pmask_t *plportMask, BOOL enable)
{
    UINT32 unit;
    phy_pmask_t stPhyMask;
    rtk_port_t pport;
    INT32 retVal;

    if(plportMask == NULL)
    {
        return DRV_ERR_PARA;
    }
    
    MaskLogic2Phy(plportMask, &stPhyMask);
    IfPhyMaskSet(&stPhyMask, unit, pport)
    {
        if(enable == TRUE)
        {
        #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
            retVal = rtk_port_greenEnable_set(unit, pport, ENABLED); 
            if(retVal != RT_ERR_OK)
            {
                return DRV_ERR_UNKNOW;
            }
        #elif defined(CHIPSET_RTL8305)
            if ((retVal = rtl8367b_setAsicGreenEthernet(ENABLED))!=RT_ERR_OK)
            return DRV_ERR_UNKNOW;
            if ((retVal = rtl8367b_setAsicPowerSaving(pport,ENABLED))!=RT_ERR_OK)
            return DRV_ERR_UNKNOW;
        #endif            
        }
        else
        {
        #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
            retVal = rtk_port_greenEnable_set(unit, pport, DISABLED);
            if(retVal != RT_ERR_OK)
            {
                return DRV_ERR_UNKNOW;
            }
        #elif defined(CHIPSET_RTL8305)
            retVal = rtl8367b_setAsicGreenEthernet(DISABLED);
            if (retVal!=RT_ERR_OK)
            return DRV_ERR_UNKNOW;
            if ((retVal = rtl8367b_setAsicPowerSaving(pport,DISABLED))!=RT_ERR_OK)
            return DRV_ERR_UNKNOW;
        #endif
        }
    }    

    return DRV_OK;
}



#ifdef  __cplusplus
}
#endif

