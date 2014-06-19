/*****************************************************************************
  --------------------------------------------------------------------------
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include <linux/string.h>
#include <linux/slab.h>
#ifdef CHIPSET_RTL8328
#include <rtk/vlan.h>
#include <common/rt_error.h>
#include <hal/mac/reg.h>
#include <hal/chipdef/esw/rtk_esw_reg_struct.h>
#elif defined(CHIPSET_RTL8305)
#include "rtk_api.h"
#include "rtk_api_ext.h"
#endif
#include "hal_common.h"


/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/
//extern DRV_RET_E Hal_AclRuleForTrapCpuCreate(ACL_TRUST_MODE_E uiAclRuleType,void *pRuleValue);
//extern DRV_RET_E Hal_AclRuleForTrapCpuDelete(ACL_TRUST_MODE_E uiAclRuleType,void *pRuleValue);
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

DRV_RET_E Hal_SdkInitSpecial(UINT32 unit)
{
#if 0
    INT32 iRet = 0;
    
    /*设置报文进入CPU口后保持tag*/
    /*当前sdk没有提供API，这里直接写寄存器*/
    iRet = reg_field_write(unit,INT_ESW_PACKET_TO_CPU_PORT_FORMAT_CONTROL_RTL8328,ESW_TOCPUPKTFMTf,1);
    if(RT_ERR_OK != iRet)
    {
        printk("%s,%d error!\n",__FUNCTION__,__LINE__);
    }

    iRet = rtk_vlan_portEgrInnerTagEnable_set(unit, PHY_CPU_PORTID, ENABLED);
    if(RT_ERR_OK != iRet)
    {
        printk("%s,%d error!\n",__FUNCTION__,__LINE__);
    }
    
    return ErrorSdk2Drv(iRet);
#else
    /*For lint*/
    unit = unit;
    return DRV_OK;
#endif
}

DRV_RET_E Hal_SetCpuHeader(BOOL bEnable)
{
#if defined(CHIPSET_RTL8305)
    rtk_api_ret_t tdRet;
    rtk_enable_t tdEnable;
#endif

    if ((FALSE != bEnable) && (TRUE != bEnable))
    {
        return DRV_ERR_PARA;
    }
#if defined(CHIPSET_RTL8305)
    if (FALSE == bEnable)
    {
        tdEnable = DISABLED;
    }
    else
    {
        tdEnable = ENABLED;
    }

    //tdRet = rtk_cpu_tagPort_set(PHY_CPU_PORTID, CPU_INSERT_TO_TRAPPING);
    tdRet = rtk_cpu_tagPort_set(6, CPU_INSERT_TO_TRAPPING);
    if (RT_ERR_OK != tdRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    tdRet = rtk_cpu_enable_set(tdEnable);
    if (RT_ERR_OK != tdRet)
    {
        return DRV_SDK_GEN_ERROR;
    }
#endif

    return DRV_OK;
}



#ifdef  __cplusplus
}
#endif

