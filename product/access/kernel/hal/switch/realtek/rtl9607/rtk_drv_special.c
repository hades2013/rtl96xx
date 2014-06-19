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
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/
//extern DRV_RET_E Hal_AclRuleForTrapCpuCreate(ACL_TRUST_MODE_E uiAclRuleType,void *pRuleValue);
//extern DRV_RET_E Hal_AclRuleForTrapCpuDelete(ACL_TRUST_MODE_E uiAclRuleType,void *pRuleValue);
/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/


/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * �궨��                                       *
*----------------------------------------------*/

DRV_RET_E Hal_SdkInitSpecial(UINT32 unit)
{
#if 0
    INT32 iRet = 0;
    
    /*���ñ��Ľ���CPU�ں󱣳�tag*/
    /*��ǰsdkû���ṩAPI������ֱ��д�Ĵ���*/
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

