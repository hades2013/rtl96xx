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

#include <common/error.h>
#include <rtk/cpu.h>

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
    rtk_api_ret_t tdRet;
    rtk_enable_t tdEnable;	
	UINT32 lport, pport;

    if ((FALSE != bEnable) && (TRUE != bEnable))
    {
        return DRV_ERR_PARA;
    }

    if (FALSE == bEnable)
    {
        tdEnable = DISABLED;
    }
    else
    {
        tdEnable = ENABLED;
    }

	tdRet = rtk_cpu_trapInsertTag_set(ENABLE);
    if (RT_ERR_OK != tdRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    return DRV_OK;
}

#ifdef  __cplusplus
}
#endif

