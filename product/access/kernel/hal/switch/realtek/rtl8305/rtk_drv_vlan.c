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
#ifdef CHIPSET_RTL8328
#include <common/error.h>
#include <common/rt_error.h>
#include <rtk/vlan.h>
#include <hal/common/halctrl.h>
#elif defined(CHIPSET_RTL8305)
#include "rtl8367b_asicdrv_vlan.h"
#include "rtl8367b_asicdrv_svlan.h"
#include "rtk_api.h"
#include "rtk_api_ext.h"
#endif

#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>


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
/*lint -save -e550 -e522 */
static spinlock_t _spin_hal_vlan_lock = SPIN_LOCK_UNLOCKED;
/*lint -restore*/
static UINT32 _hal_vlan_num = 1;
/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define SPIN_HAL_MAC_LOCK       spin_lock(&_spin_hal_vlan_lock)
#define SPIN_HAL_MAC_UNLOCK     spin_unlock(&_spin_hal_vlan_lock)

/*****************************************************************************
    Func Name:  Hal_GetVlanExistNum
  Description:  get total exist vlan entry number
        Input:  puiNum
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetVlanExistNum(UINT32 *puiNum)
{
    if(NULL == puiNum)
    {
        return DRV_ERR_PARA;
    }    
    
    SPIN_HAL_MAC_LOCK;
    *puiNum = _hal_vlan_num;
    SPIN_HAL_MAC_UNLOCK;
    
    return DRV_OK;
}



/*****************************************************************************
    Func Name:  Hal_SetVlanMode
  Description:  set global vlan mode
        Input:  ulMode      DISABLE for port-based vlan
                            ENABLE for 802.1q vlan
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetVlanMode(UINT32 ulMode)
{
    UINT32 ulUnit, ulPPort;
    ret_t ret;

    switch (ulMode)
    {
        case PVLAN:
            /*port-based VLAN*/
            /*Disable 1q vlan*/
            ret = rtl8367b_setAsicVlanFilter(DISABLED);
            if (RT_ERR_OK != ret)
            {
                return DRV_SDK_GEN_ERROR;
            }
            
            /*打开per-port设定VLAN tag ignore，这样不会参考Tag中的VLAN 
            ID（所有封包仅参考port based vlan，default vlan）*/
            PhyPortFor(ulUnit, ulPPort)
            {
                /*Disable vlan ingress filter.*/
                ret = rtl8367b_setAsicVlanIngressFilter(ulPPort, DISABLED);
                if (RT_ERR_OK != ret)
                {
                    return DRV_SDK_GEN_ERROR;
                }

                /*Vlan egress original.*/
                ret = rtl8367b_setAsicVlanEgressTagMode(ulPPort, EG_TAG_MODE_KEEP);
                if (RT_ERR_OK != ret)
                {
                    return DRV_SDK_GEN_ERROR;
                }
            }
            break;
        case QVLAN:
            /*802.1q VLAN*/
            /*Enable 1q vlan*/
            ret = rtl8367b_setAsicVlanFilter(ENABLED);
            if (RT_ERR_OK != ret)
            {
                return DRV_SDK_GEN_ERROR;
            }
            
            PhyPortFor(ulUnit, ulPPort)
            {
                /*Enable vlan ingress filter.*/
                ret = rtl8367b_setAsicVlanIngressFilter(ulPPort, ENABLED);
                if (RT_ERR_OK != ret)
                {
                    return DRV_SDK_GEN_ERROR;
                }

                /*Vlan egress original.*/
                ret = rtl8367b_setAsicVlanEgressTagMode(ulPPort, EG_TAG_MODE_KEEP);
                if (RT_ERR_OK != ret)
                {
                    return DRV_SDK_GEN_ERROR;
                }
            }
            break;
        default:
            return DRV_ERR_PARA;
    }
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetVlanEntryCreate
  Description:  set vlan entry
        Input:  ulVlanId     
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
DRV_RET_E Hal_SetVlanEntryCreate(UINT32 ulVlanId)
{
    UINT32 ulUnit;
    UINT32 ulPPortid;
    phy_pmask_t stPhyMask, stPhyMaskUntag;
    rtk_fid_t fid = 0;
    rtk_api_ret_t ret;

    if(!VALID_VLAN_ID(ulVlanId))
    {
        return DRV_ERR_PARA;
    }

    ClrPhyMaskAll(&stPhyMask);
    ClrPhyMaskAll(&stPhyMaskUntag);

    UnitFor(ulUnit)
    {
        ret = rtk_vlan_get((rtk_vlan_t)ulVlanId, &(stPhyMask.pmask[ulUnit]), &(stPhyMaskUntag.pmask[ulUnit]), &fid);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    /*eixisted*/
    PhyPortFor(ulUnit, ulPPortid)
    {
        if (RTK_PORTMASK_IS_PORT_SET((stPhyMask.pmask[ulUnit]), ulPPortid))
        {
            return DRV_OK;
        }
    }

    fid = 0;
    UnitFor(ulUnit)
    {
        ret = rtk_vlan_set(ulVlanId, (stPhyMask.pmask[ulUnit]), (stPhyMaskUntag.pmask[ulUnit]), fid);
        if(RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    SPIN_HAL_MAC_LOCK;
    _hal_vlan_num++;
    SPIN_HAL_MAC_UNLOCK;
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetVlanEntryDelete
  Description:  vlan entry delete
        Input:  ulVlanId     
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetVlanEntryDelete(UINT32 ulVlanId)
{
    BOOL bExisted = FALSE;
    UINT32 ulUnit;
    UINT32 ulPPortid;
    phy_pmask_t stPhyMask, stPhyMaskUntag, stPhyMaskTmp;
    rtk_fid_t fid = 0;
    rtk_api_ret_t ret;

    if(!VALID_VLAN_ID(ulVlanId))
    {
        return DRV_ERR_PARA;
    }

    ClrPhyMaskAll(&stPhyMask);
    ClrPhyMaskAll(&stPhyMaskUntag);
    SetPhyMaskAll(&stPhyMaskTmp);

    UnitFor(ulUnit)
    {
        ret = rtk_vlan_get((rtk_vlan_t)ulVlanId, &(stPhyMask.pmask[ulUnit]), &(stPhyMaskUntag.pmask[ulUnit]), &fid);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    /*deleted*/
    PhyPortFor(ulUnit, ulPPortid)
    {
        if (RTK_PORTMASK_IS_PORT_SET((stPhyMask.pmask[ulUnit]), ulPPortid))
        {
            bExisted = TRUE;
            break;
        }
    }

    if (TRUE != bExisted)
    {
        return DRV_OK;
    }
    
    PhyMaskRemove(&stPhyMask, &stPhyMaskTmp);
    PhyMaskRemove(&stPhyMaskUntag, &stPhyMaskTmp);

    UnitFor(ulUnit)
    {
        ret = rtk_vlan_set(ulVlanId, (stPhyMask.pmask[ulUnit]), (stPhyMaskUntag.pmask[ulUnit]), fid);
        if(RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    SPIN_HAL_MAC_LOCK;
    if(_hal_vlan_num > 0)
    {
        _hal_vlan_num--;
    }
    SPIN_HAL_MAC_UNLOCK;
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetVlanMemberRemove
  Description:  VlanMemberRemove
        Input:  ulVlanId
                stLgcMask
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetVlanMemberRemove(UINT32 ulVlanId, logic_pmask_t stLgcMask)
{
    UINT32 ulUnit;
    phy_pmask_t stPhyMask, stPhyMaskTmp, stPhyMaskUntagTmp;
    rtk_fid_t fid = 0;
    rtk_api_ret_t ret;

#if defined(CONFIG_PRODUCT_EPN104)
    if ((0 != ulVlanId) && (!VALID_VLAN_ID(ulVlanId)))
#else
    if (!VALID_VLAN_ID(ulVlanId))
#endif
    {
        return DRV_ERR_PARA;
    }

    ClrPhyMaskAll(&stPhyMask);
    ClrPhyMaskAll(&stPhyMaskTmp);
    ClrPhyMaskAll(&stPhyMaskUntagTmp);
    MaskLogic2Phy(&stLgcMask, &stPhyMask);

    UnitFor(ulUnit)
    {
        ret = rtk_vlan_get((rtk_vlan_t)ulVlanId, &(stPhyMaskTmp.pmask[ulUnit]), &(stPhyMaskUntagTmp.pmask[ulUnit]), &fid);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    //PhyMaskNegate(phy_pmask_t * mska, phy_pmask_t * mskb);
    PhyMaskRemove(&stPhyMaskTmp, &stPhyMask);
    PhyMaskRemove(&stPhyMaskUntagTmp, &stPhyMask);

    fid = 0;
    UnitFor(ulUnit)
    {
        ret = rtk_vlan_set(ulVlanId, (stPhyMaskTmp.pmask[ulUnit]), (stPhyMaskUntagTmp.pmask[ulUnit]), fid);
        if(RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }  
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetVlanMemberAdd
  Description:  VlanMemberAdd
        Input:  ulVlanId
                stLgcMask
                stLgcMaskUntag
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetVlanMemberAdd(UINT32 ulVlanId, logic_pmask_t stLgcMask, logic_pmask_t stLgcMaskUntag)
{
    UINT32 ulUnit;
    phy_pmask_t stPhyMask, stPhyMaskUntag, stPhyMaskTmp, stPhyMaskUntagTmp;
    rtk_fid_t fid = 0;
    rtk_api_ret_t ret;

#if defined(CONFIG_PRODUCT_EPN104)
    if ((0 != ulVlanId) && (!VALID_VLAN_ID(ulVlanId)))
#else
    if (!VALID_VLAN_ID(ulVlanId))
#endif
    {
        return DRV_ERR_PARA;
    }

    ClrPhyMaskAll(&stPhyMask);
    ClrPhyMaskAll(&stPhyMaskUntag);
    ClrPhyMaskAll(&stPhyMaskTmp);
    ClrPhyMaskAll(&stPhyMaskUntagTmp);
  
    MaskLogic2Phy(&stLgcMask, &stPhyMask);
    MaskLogic2Phy(&stLgcMaskUntag, &stPhyMaskUntag);
    
    UnitFor(ulUnit)
    {
        ret = rtk_vlan_get((rtk_vlan_t)ulVlanId, &(stPhyMaskTmp.pmask[ulUnit]), &(stPhyMaskUntagTmp.pmask[ulUnit]), &fid);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
   
    
        PhyMaskOr(&stPhyMask, &stPhyMaskTmp);
        PhyMaskOr(&stPhyMaskUntag, &stPhyMaskUntagTmp);

        fid = 0;

    
        #if 0
        printk("\nvid:%d, mem mask:0x%04X, untagged:0x%04X.\n",ulVlanId,
                 stPhyMask.pmask[ulUnit].bits[0],
                 stPhyMaskUntag.pmask[ulUnit].bits[0]);
        #endif
        ret = rtk_vlan_set(ulVlanId, stPhyMask.pmask[ulUnit], stPhyMaskUntag.pmask[ulUnit], fid);
        if(RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_SetMcVlanMemberAdd
  Description: Add mc vlan member.
        Input: UINT32 ulVlanId               
                logic_pmask_t stLgcMask       
                logic_pmask_t stLgcMaskUntag  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
extern DRV_RET_E Hal_GetValideFid(UINT32 uiVid, UINT32 *puiFid);
extern DRV_RET_E Hal_SetFidByVid(UINT32 uiVid, UINT32 uiFid, logic_pmask_t stLPortMsk, logic_pmask_t stUntagLPortMsk);
DRV_RET_E Hal_SetMcVlanMemberAdd(UINT32 ulVlanId, logic_pmask_t stLgcMask, logic_pmask_t stLgcMaskUntag)
{
    UINT32 ulUnit;
    UINT32 uiFid;
    UINT32 uiLPortTmp;
    DRV_RET_E enRet;
    logic_pmask_t stLPortMskTmp;
    logic_pmask_t stUntagLPortMskTmp;
    logic_pmask_t stTranspLgcMask;
    CTC_VLAN_CFG_S stCtcVlanMode;
    phy_pmask_t stPhyMask, stPhyMaskUntag, stPhyMaskTmp, stPhyMaskUntagTmp;
    rtk_fid_t fidTmp = 0;
    rtk_api_ret_t ret;

#if defined(CONFIG_PRODUCT_EPN104)
    if ((0 != ulVlanId) && (!VALID_VLAN_ID(ulVlanId)))
#else
    if (!VALID_VLAN_ID(ulVlanId))
#endif
    {
        return DRV_ERR_PARA;
    }

    (void)Hal_GetValideFid(ulVlanId, &uiFid);
    if (RTL8367B_FIDMAX < uiFid)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /* */
    memset(&stLPortMskTmp, 0, sizeof(stLPortMskTmp));
    memset(&stUntagLPortMskTmp, 0, sizeof(stUntagLPortMskTmp));
    memset(&stTranspLgcMask, 0, sizeof(stTranspLgcMask));
    /* 014545 */
    
    /*get transparent port mask*/
    for (uiLPortTmp = 1; uiLPortTmp <= LOGIC_PORT_NO; uiLPortTmp++)
    {
        enRet = Hal_CfgPortCtcVlanGet(uiLPortTmp, &stCtcVlanMode);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }

        if ((CTC_VLAN_MODE_TRANSPARENT == stCtcVlanMode.mode) ||
            (CTC_VLAN_MODE_QINQ == stCtcVlanMode.mode))
        {
            SetLgcMaskBit((port_num_t)uiLPortTmp, &stTranspLgcMask);
        }
    }

    (void)Hal_GetMcVlanMem(ulVlanId, &stLPortMskTmp, &stUntagLPortMskTmp);
    LgcMaskOr(&stLPortMskTmp, &stLgcMask);
    LgcMaskOr(&stUntagLPortMskTmp, &stLgcMaskUntag);

    ClrPhyMaskAll(&stPhyMask);
    ClrPhyMaskAll(&stPhyMaskUntag);
    ClrPhyMaskAll(&stPhyMaskTmp);
    ClrPhyMaskAll(&stPhyMaskUntagTmp);

    LgcMaskOr(&stLgcMask, &stTranspLgcMask);
    LgcMaskOr(&stLgcMask, &stLPortMskTmp);
    LgcMaskOr(&stLgcMaskUntag, &stUntagLPortMskTmp);
    SetLgcMaskBit(LOGIC_CPU_PORT, &stLgcMask);
	SetLgcMaskBit(LOGIC_UPPON_PORT, &stLgcMask);
    
    MaskLogic2Phy(&stLgcMask, &stPhyMask);
    MaskLogic2Phy(&stLgcMaskUntag, &stPhyMaskUntag);
    
    UnitFor(ulUnit)
    {
        #if 0
        fidTmp = uiFid;
        ret = rtk_vlan_get((rtk_vlan_t)ulVlanId, &(stPhyMaskTmp.pmask[ulUnit]), &(stPhyMaskUntagTmp.pmask[ulUnit]), &fidTmp);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }

        PhyMaskOr(&stPhyMask, &stPhyMaskTmp);
        PhyMaskOr(&stPhyMaskUntag, &stPhyMaskUntagTmp);
        #endif
        fidTmp = uiFid;
        #if 0
        printk("\nf:%s, cvid:%d, mem mask:0x%04X, untagged:0x%04X,fid:%d.\n",__func__,ulVlanId,
                 stPhyMask.pmask[ulUnit].bits[0],
                 stPhyMaskUntag.pmask[ulUnit].bits[0],fidTmp);
        #endif
        ret = rtk_vlan_set(ulVlanId, stPhyMask.pmask[ulUnit], stPhyMaskUntag.pmask[ulUnit], fidTmp);
        if(RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    /*set svlan entry*/
    UnitFor(ulUnit)
    {
        rtk_svlan_memberCfg_t stSvlan_cfg;
        memset(&stSvlan_cfg, 0, sizeof(stSvlan_cfg));
        #if 0
        ret = rtk_svlan_memberPortEntry_get((rtk_vlan_t)ulVlanId, &stSvlan_cfg);
        if ((RT_ERR_OK != ret) && (RT_ERR_SVLAN_ENTRY_NOT_FOUND != ret))
        {
            return DRV_SDK_GEN_ERROR;
        }
        #endif
        stSvlan_cfg.svid = ulVlanId;
        stSvlan_cfg.priority = 0;
        stSvlan_cfg.fid = uiFid;
        stSvlan_cfg.memberport |= stPhyMask.pmask[ulUnit].bits[0];
        stSvlan_cfg.untagport  |= stPhyMaskUntag.pmask[ulUnit].bits[0];
        #if 0
        printk("\nf:%s, svid:%d, mem mask:0x%04X, untagged:0x%04X,fid:%d.\n",__func__,ulVlanId,
                 stPhyMask.pmask[ulUnit].bits[0],
                 stPhyMaskUntag.pmask[ulUnit].bits[0],fidTmp);
        #endif

        ret = rtk_svlan_memberPortEntry_set((rtk_vlan_t)ulVlanId, &stSvlan_cfg);
        if(RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    /*port members exclude ctc transparent ports ,uplink and cpu ports*/
    (void)Hal_SetFidByVid(ulVlanId, uiFid, stLPortMskTmp, stUntagLPortMskTmp);
    return DRV_OK;
}

/* */
/*****************************************************************************
    Func Name: Hal_SetMcVlanMemberRmv
 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetMcVlanMemberRmv(UINT32 ulVlanId, logic_pmask_t stLgcMask)
{
    UINT32 ulUnit;
    UINT32 uiFid;
    UINT32 uiLPortTmp;
    DRV_RET_E enRet;
    logic_pmask_t stLPortMskTmp;
    logic_pmask_t stUntagLPortMskTmp;
    logic_pmask_t stTranspLgcMask;
    CTC_VLAN_CFG_S stCtcVlanMode;
    phy_pmask_t stPhyMask, stPhyMaskUntag;
    rtk_api_ret_t ret;

#if defined(CONFIG_PRODUCT_EPN104)
    if ((0 != ulVlanId) && (!VALID_VLAN_ID(ulVlanId)))
#else
    if (!VALID_VLAN_ID(ulVlanId))
#endif
    {
        return DRV_ERR_PARA;
    }

    (void)Hal_GetValideFid(ulVlanId, &uiFid);
    if (RTL8367B_FIDMAX < uiFid)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /*get transparent port mask*/
    ClrLgcMaskAll(&stTranspLgcMask);
    for (uiLPortTmp = 1; uiLPortTmp <= LOGIC_PORT_NO; uiLPortTmp++)
    {
        enRet = Hal_CfgPortCtcVlanGet(uiLPortTmp, &stCtcVlanMode);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }

        if ((CTC_VLAN_MODE_TRANSPARENT == stCtcVlanMode.mode) ||
            (CTC_VLAN_MODE_QINQ == stCtcVlanMode.mode))
        {
            SetLgcMaskBit((port_num_t)uiLPortTmp, &stTranspLgcMask);
        }
    }

    (void)Hal_GetMcVlanMem(ulVlanId, &stLPortMskTmp, &stUntagLPortMskTmp);
    LgcMaskRemove(&stLPortMskTmp, &stLgcMask);
    LgcMaskRemove(&stUntagLPortMskTmp, &stLgcMask);
    if (TRUE == LgcMaskNotNull(&stLPortMskTmp))
    {
        SetLgcMaskBit(LOGIC_CPU_PORT, &stLPortMskTmp);
	    SetLgcMaskBit(LOGIC_UPPON_PORT, &stLPortMskTmp);
        /*  */
        /*the ports should be removed from mc vlan member,
        but should not be removed from ctc transparent member.*/
        #if 0
        LgcMaskRemove(&stTranspLgcMask, &stLgcMask);
        #endif
        /* 014545 */
        
        LgcMaskOr(&stLPortMskTmp, &stTranspLgcMask);
    }

    ClrPhyMaskAll(&stPhyMask);
    ClrPhyMaskAll(&stPhyMaskUntag);

    MaskLogic2Phy(&stLPortMskTmp, &stPhyMask);
    MaskLogic2Phy(&stUntagLPortMskTmp, &stPhyMaskUntag);
    /*set cvlan entry*/
    UnitFor(ulUnit)
    {
        #if 0
        printk("\nf:%s, cvid:%d, mem mask:0x%04X, untagged:0x%04X,fid:%d.\n",__func__,ulVlanId,
                 stPhyMask.pmask[ulUnit].bits[0],
                 stPhyMaskUntag.pmask[ulUnit].bits[0],uiFid);
        #endif
        ret = rtk_vlan_set(ulVlanId, stPhyMask.pmask[ulUnit], stPhyMaskUntag.pmask[ulUnit], uiFid);
        if(RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    /*set svlan entry*/
    UnitFor(ulUnit)
    {
        rtk_svlan_memberCfg_t stSvlan_cfg;
        memset(&stSvlan_cfg, 0, sizeof(stSvlan_cfg));
        stSvlan_cfg.svid = ulVlanId;
        stSvlan_cfg.priority = 0;
        stSvlan_cfg.fid = uiFid;
        stSvlan_cfg.memberport |= stPhyMask.pmask[ulUnit].bits[0];
        stSvlan_cfg.untagport  |= stPhyMaskUntag.pmask[ulUnit].bits[0];
        #if 0
        printk("\nf:%s, svid:%d, mem mask:0x%04X, untagged:0x%04X,fid:%d.\n",__func__,ulVlanId,
                 stPhyMask.pmask[ulUnit].bits[0],
                 stPhyMaskUntag.pmask[ulUnit].bits[0],uiFid);
        #endif

        ret = rtk_svlan_memberPortEntry_set((rtk_vlan_t)ulVlanId, &stSvlan_cfg);
        if(RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    /*port members exclude ctc transparent ports ,uplink and cpu ports*/
    (void)Hal_GetMcVlanMem(ulVlanId, &stLPortMskTmp, &stUntagLPortMskTmp);
    LgcMaskRemove(&stLPortMskTmp, &stLgcMask);
    LgcMaskRemove(&stUntagLPortMskTmp, &stLgcMask);
    (void)Hal_SetFidByVid(ulVlanId, uiFid, stLPortMskTmp, stUntagLPortMskTmp);
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GetMcVlanMember
 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetMcVlanMember(UINT32 ulVlanId, logic_pmask_t *pstLgcMask, logic_pmask_t *pstLgcMaskUntag)
{
    UINT32 uiUnit;
    UINT32 uiFid = 0;
    phy_pmask_t stPhyMaskTmp, stPhyMaskUntagTmp;
    rtk_api_ret_t ret;

#if defined(CONFIG_PRODUCT_EPN104)
    if ((0 != ulVlanId) && (!VALID_VLAN_ID(ulVlanId)))
#else
    if (!VALID_VLAN_ID(ulVlanId))
#endif
    {
        return DRV_ERR_PARA;
    }
    if ((NULL == pstLgcMask) || (NULL == pstLgcMaskUntag))
    {
        return DRV_ERR_PARA;
    }

    (void)Hal_GetValideFid(ulVlanId, &uiFid);
    if (RTL8367B_FIDMAX < uiFid)
    {
        return DRV_SDK_GEN_ERROR;
    }
    
    UnitFor(uiUnit)
    {
        ret = rtk_vlan_get((rtk_vlan_t)ulVlanId, &(stPhyMaskTmp.pmask[uiUnit]), &(stPhyMaskUntagTmp.pmask[uiUnit]), (rtk_fid_t *)&uiFid);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
        #if 0
        printk("\nf:%s,cvid:%d, mem mask:0x%04X, untagged:0x%04X,fid:%d.\n",__func__,ulVlanId,
                 stPhyMaskTmp.pmask[uiUnit].bits[0],
                 stPhyMaskUntagTmp.pmask[uiUnit].bits[0],uiFid);
        #endif

    }

    MaskPhy2Logic(&stPhyMaskTmp, pstLgcMask);
    MaskPhy2Logic(&stPhyMaskUntagTmp, pstLgcMaskUntag);
    
    return DRV_OK;
}
/* 013987 */

/*****************************************************************************
    Func Name:  Hal_SetValnPvid
  Description:  set port pvid
        Input:  ulLgcPortNumber
                ulPvid
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
DRV_RET_E Hal_SetVlanPvid(UINT32 ulLgcPortNumber, UINT32 ulPvid)
{
    rtk_pri_t priority = 0;
    rtk_api_ret_t ret;

#if defined(CONFIG_PRODUCT_EPN104)
    priority = (ulPvid & 0x0000F000) >> 12;
    ulPvid &= 0x0FFF;

    if (7 < priority)
    {
        return DRV_ERR_PARA;
    }

    if ((0 != ulPvid) && (!VALID_VLAN_ID(ulPvid)))
#else
    if(!VALID_VLAN_ID(ulPvid))
#endif
    {
        return DRV_ERR_PARA;
    }
    if(!IsValidLgcPort(ulLgcPortNumber))
    {
        return DRV_ERR_PARA;
    }

    ret = rtk_vlan_portPvid_set(PortLogic2PhyPortId(ulLgcPortNumber), ulPvid, priority);
    if (RT_ERR_OK != ret)
    {
        return DRV_SDK_GEN_ERROR;
    }
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetVlanPortVlanMember
  Description:  set port-based VLAN member
        Input:  ulLgcPortNumber
                stLgcMask
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
DRV_RET_E Hal_SetVlanPortVlanMember(UINT32 ulLgcPortNumber, logic_pmask_t stLgcMask)
{
    UINT32 ulUnit;
    phy_pmask_t stPhyMask;
    rtk_api_ret_t ret;

    if(!IsValidLgcPort(ulLgcPortNumber))
    {
        return DRV_ERR_PARA;
    }
    
    /*must add CPU port to port mask*/
    MaskLogic2Phy(&stLgcMask, &stPhyMask);
    PhyMaskAddCpuPort(&stPhyMask);

    ulUnit = PortLogci2ChipId(ulLgcPortNumber);
    ret = rtk_port_isolation_set(PortLogic2PhyPortId(ulLgcPortNumber), CHIPNMASK(ulUnit, &stPhyMask));
    if (RT_ERR_OK != ret)
    {
        return DRV_SDK_GEN_ERROR;
    }
    
    return DRV_OK;
}

DRV_RET_E Hal_GetVlanMember(UINT32 ulVlanId, logic_pmask_t *pstLgcMask, logic_pmask_t *pstLgcMaskUntag)
{
    UINT32 ulUnit;
    phy_pmask_t stPhyMaskTmp, stPhyMaskUntagTmp;
    rtk_fid_t fid = 0;
    rtk_api_ret_t ret;

#if defined(CONFIG_PRODUCT_EPN104)
    if ((0 != ulVlanId) && (!VALID_VLAN_ID(ulVlanId)))
#else
    if (!VALID_VLAN_ID(ulVlanId))
#endif
    {
        return DRV_ERR_PARA;
    }
    if ((NULL == pstLgcMask) || (NULL == pstLgcMaskUntag))
    {
        return DRV_ERR_PARA;
    }

    UnitFor(ulUnit)
    {
        ret = rtk_vlan_get((rtk_vlan_t)ulVlanId, &(stPhyMaskTmp.pmask[ulUnit]), &(stPhyMaskUntagTmp.pmask[ulUnit]), &fid);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    MaskPhy2Logic(&stPhyMaskTmp, pstLgcMask);
    MaskPhy2Logic(&stPhyMaskUntagTmp, pstLgcMaskUntag);
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AddCpuToVlanMember
  Description: 向vlan中添加CPU端口
        Input: UINT32 ulVlanId  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_AddCpuToVlanMember(UINT32 ulVlanId)
{
    UINT32 ulUnit = 0;
    phy_pmask_t stPhyMask;
    phy_pmask_t stPhyMaskUntag;
    rtk_fid_t fid = 0;
    rtk_api_ret_t ret;

    if(!VALID_VLAN_ID(ulVlanId))
    {
        return DRV_ERR_PARA;
    }
    
    ClrPhyMaskAll(&stPhyMask);
    ClrPhyMaskAll(&stPhyMaskUntag);
    
    UnitFor(ulUnit)
    {
        ret = rtk_vlan_get((rtk_vlan_t)ulVlanId, &(stPhyMask.pmask[ulUnit]), &(stPhyMaskUntag.pmask[ulUnit]), &fid);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }
    
    PhyMaskAddCpuPort(&stPhyMask);
    PhyMaskRemoveCpuPort(&stPhyMaskUntag);/*realtek cpu 端口需要按TAG方式添加*/

    fid = 0;
    UnitFor(ulUnit)
    {
        ret = rtk_vlan_set(ulVlanId, (stPhyMask.pmask[ulUnit]), (stPhyMaskUntag.pmask[ulUnit]), fid);
        if(RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_RemoveCpuFromVlanMember
  Description: 将cpu端口从vlan中删除
        Input: UINT32 ulVlanId  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_RemoveCpuFromVlanMember(UINT32 ulVlanId)
{
    UINT32 ulUnit = 0;
    phy_pmask_t stPhyMask;
    phy_pmask_t stPhyMaskUntag;
    rtk_fid_t fid = 0;
    rtk_api_ret_t ret;

    if(!VALID_VLAN_ID(ulVlanId))
    {
        return DRV_ERR_PARA;
    }
    
    ClrPhyMaskAll(&stPhyMask);
    ClrPhyMaskAll(&stPhyMaskUntag);
    
    UnitFor(ulUnit)
    {
        ret = rtk_vlan_get((rtk_vlan_t)ulVlanId, &(stPhyMask.pmask[ulUnit]), &(stPhyMaskUntag.pmask[ulUnit]), &fid);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }
    PhyMaskRemoveCpuPort(&stPhyMask);
    PhyMaskRemoveCpuPort(&stPhyMaskUntag);

    fid = 0;
    UnitFor(ulUnit)
    {
        ret = rtk_vlan_set(ulVlanId, (stPhyMask.pmask[ulUnit]), (stPhyMaskUntag.pmask[ulUnit]), fid);
        if(RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    return DRV_OK;
}

#if defined(CONFIG_PRODUCT_EPN104)
/*****************************************************************************
    Func Name: Hal_GetPortBaseVlanEntry
  Description: Get specific vlan id's port base vlan entry.
        Input: UINT32 *puiEntryId,rtk_vlan_mbrcfg_t *pstVlanEntry  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortBaseVlanEntry(UINT32 *puiEntryId, rtk_vlan_mbrcfg_t *pstVlanEntry)
{
    UINT32 i;
    UINT32 uiVid;
	#ifdef CHIPSET_RTL8305   
    UINT32 uiEntryIdTmp = RTL8367B_CVIDXNO;
    rtk_api_ret_t Ret;
    #endif

    if ((NULL == puiEntryId) || (NULL == pstVlanEntry))
    {
        return DRV_ERR_PARA;
    }
    
#ifdef CHIPSET_RTL8305 
    uiVid = pstVlanEntry->evid;
    for (i = 0; i < RTL8367B_CVIDXMAX; i++)
    {   
        Ret = rtk_vlan_mbrCfg_get(i, pstVlanEntry);
        if (RT_ERR_OK != Ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
            
        if (uiVid == pstVlanEntry->evid)
        {       
            uiEntryIdTmp = i;
            break;
        }
        else if (0 == (pstVlanEntry->mbr & 0xFF))
        {
            if (RTL8367B_CVIDXNO == uiEntryIdTmp)
            {
                uiEntryIdTmp = i;
            }
        }
    }

    *puiEntryId = uiEntryIdTmp;
#endif

	return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_SetPortBaseVlanEntry
  Description: Set port base vlan entry to specific index.
        Input: UINT32 *puiEntryId                 
                rtk_vlan_mbrcfg_t *pstVlanEntry  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortBaseVlanEntry(UINT32 *puiEntryId, rtk_vlan_mbrcfg_t *pstVlanEntry)
{
#ifdef CHIPSET_RTL8305   
    rtk_api_ret_t Ret;
#endif

    if ((NULL == puiEntryId) || (NULL == pstVlanEntry))
    {
        return DRV_ERR_PARA;
    }
    
#ifdef CHIPSET_RTL8305 
    Ret = rtk_vlan_mbrCfg_set(*puiEntryId, pstVlanEntry);
    if (RT_ERR_OK != Ret)
    {
        return DRV_SDK_GEN_ERROR;
    }
#endif

	return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_DelPortBaseVlanMemb
  Description: Delete port base vlan member.
        Input: UINT32 uiVlanId          
                logic_pmask_t stLgcMask  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_DelPortBaseVlanMemb(UINT32 uiVlanId, logic_pmask_t stLgcMask)
{
    UINT32 uiEntryId;
    UINT32 uiUnit;
    phy_pmask_t stPhyMask;
    rtk_vlan_mbrcfg_t stPortBaseVlan;

    if(!VALID_VLAN_ID(uiVlanId))
    {
        return DRV_ERR_PARA;
    }

    ClrPhyMaskAll(&stPhyMask);
    MaskLogic2Phy(&stLgcMask, &stPhyMask);

    UnitFor(uiUnit)
    {
        /* Also set the default VLAN to 32 member configuration index 0 */
        memset(&stPortBaseVlan,0,sizeof(stPortBaseVlan));

        /*Vlan index 31 is used for default vlan.*/
        stPortBaseVlan.evid = (UINT16)uiVlanId;
 
        if (DRV_OK != Hal_GetPortBaseVlanEntry(&uiEntryId, &stPortBaseVlan))
        {
            return DRV_ERR_UNKNOW;
        }

        stPortBaseVlan.evid = (UINT16)uiVlanId;
        stPortBaseVlan.mbr &= ((UINT16)(~(stPhyMask.pmask[uiUnit].bits[0])));                  

        if (DRV_OK != Hal_SetPortBaseVlanEntry(&uiEntryId, &stPortBaseVlan))
        {
            return DRV_ERR_UNKNOW;  
        }
    }
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_ClrPortBaseVlanEntryByVid
  Description: Clear port base vlan entry by vlan id.
        Input: UINT32 uiVlanId  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_ClrPortBaseVlanEntryByVid(UINT32 uiVlanId)
{
    UINT32 uiEntryId;
    UINT32 uiUnit;
    DRV_RET_E enRet;
    //phy_pmask_t stPhyMask;
    rtk_vlan_mbrcfg_t stPortBaseVlan;
    rtl8367b_user_vlan4kentry st4KVlanEntry;

    if(!VALID_VLAN_ID(uiVlanId))
    {
        return DRV_ERR_PARA;
    }

    //ClrPhyMaskAll(&stPhyMask);
    //MaskLogic2Phy(&stLgcMask, &stPhyMask);

    UnitFor(uiUnit)
    {
        /* Also set the default VLAN to 32 member configuration index 0 */
        memset(&stPortBaseVlan,0,sizeof(stPortBaseVlan));

        /*Vlan index 31 is used for default vlan.*/
        stPortBaseVlan.evid = (UINT16)uiVlanId;
        enRet = Hal_GetPortBaseVlanEntry(&uiEntryId, &stPortBaseVlan);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }

        if (RTL8367B_CVIDXNO == uiEntryId)
        {
            printk("\nfunc:%s: Do not find th vlan index entry.\n",__FUNCTION__);
            return DRV_OK;
        }
        //stPortBaseVlan.mbr &= ((UINT16)(~(stPhyMask.pmask[uiUnit].bits[0])));                  

        memset(&st4KVlanEntry, 0, sizeof(st4KVlanEntry));
        st4KVlanEntry.vid = (UINT16)uiVlanId;
        if (RT_ERR_OK != rtl8367b_getAsicVlan4kEntry(&st4KVlanEntry))
        {
            return DRV_ERR_UNKNOW;
        }
        //printk("\nfunc:%s: vid:%d, index memb:0x%04X, 4k memb:0x%04X.\n",__FUNCTION__,
        //          uiVlanId,stPortBaseVlan.mbr,st4KVlanEntry.mbr);
        stPortBaseVlan.mbr &= st4KVlanEntry.mbr;
        if (0 != stPortBaseVlan.evid)
        {
            if (0 == (stPortBaseVlan.mbr & ~((1U << SWITCH_UPLINK_PHY_PORT) | (1U << SWITCH_CPU_PHY_PORT))))
            {
                stPortBaseVlan.evid = 0;
                stPortBaseVlan.mbr = 0;
            }
        }

        if (DRV_OK != Hal_SetPortBaseVlanEntry(&uiEntryId, &stPortBaseVlan))
        {
            return DRV_ERR_UNKNOW;  
        }
    }
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AddPortBaseVlanMemb
  Description: Add port base vlan membership.
        Input: UINT32 uiVlanId          
                logic_pmask_t stLgcMask  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AddPortBaseVlanMemb(UINT32 uiVlanId, logic_pmask_t stLgcMask)
{
    UINT32 uiEntryId;
    UINT32 uiUnit;
    phy_pmask_t stPhyMask;
    rtk_vlan_mbrcfg_t stPortBaseVlan;

    if(!VALID_VLAN_ID(uiVlanId))
    {
        printk("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_PARA;
    }

    ClrPhyMaskAll(&stPhyMask);
    MaskLogic2Phy(&stLgcMask, &stPhyMask);

    UnitFor(uiUnit)
    {
        /* Also set the default VLAN to 32 member configuration index 0 */
        memset(&stPortBaseVlan,0,sizeof(stPortBaseVlan));

        /*Vlan index 31 is used for default vlan.*/
        stPortBaseVlan.evid = (UINT16)uiVlanId;
 
        if (DRV_OK != Hal_GetPortBaseVlanEntry(&uiEntryId, &stPortBaseVlan))
        {
            printk("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        stPortBaseVlan.evid = (UINT16)uiVlanId;
        stPortBaseVlan.mbr |= ((UINT16)stPhyMask.pmask[uiUnit].bits[0]); 

        if (DRV_OK != Hal_SetPortBaseVlanEntry(&uiEntryId, &stPortBaseVlan))
        {
            printk("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__);
            return DRV_ERR_UNKNOW;  
        }
    }
    
    return DRV_OK;
}
/*****************************************************************************
    Func Name: Hal_AddPortBaseVlanMemByIndx
  Description: Set port base vlan member by entry id.
        Input: UINT32 uiEntryId         
                logic_pmask_t stLgcMask  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AddPortBaseVlanMemByIndx(UINT32 uiEntryId, logic_pmask_t stLgcMask)
{
    UINT32 uiUnit;
    phy_pmask_t stPhyMask;
    rtk_vlan_mbrcfg_t stPortBaseVlan;

    if(RTL8367B_CVIDXMAX < uiEntryId)
    {
        return DRV_ERR_PARA;
    }

    ClrPhyMaskAll(&stPhyMask);
    MaskLogic2Phy(&stLgcMask, &stPhyMask);

    UnitFor(uiUnit)
    {
        memset(&stPortBaseVlan,0,sizeof(stPortBaseVlan));
        /*Vlan index 31 is used for default vlan.*/
        if (RT_ERR_OK != rtk_vlan_mbrCfg_get(uiEntryId, &stPortBaseVlan))
        {
            return DRV_SDK_GEN_ERROR;
        }

        stPortBaseVlan.mbr |= (UINT16)(stPhyMask.pmask[uiUnit].bits[0]);                  

        if (RT_ERR_OK != rtk_vlan_mbrCfg_set(uiEntryId, &stPortBaseVlan))
        {
            return DRV_SDK_GEN_ERROR;  
        }
    }
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_DelPortBaseVlanMemByIndx
  Description: Delete port base vlan membership by entry id.
        Input: UINT32 uiEntryId         
                logic_pmask_t stLgcMask  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_DelPortBaseVlanMemByIndx(UINT32 uiEntryId, logic_pmask_t stLgcMask)
{
    UINT32 uiUnit;
    phy_pmask_t stPhyMask;
    rtk_vlan_mbrcfg_t stPortBaseVlan;

    if(RTL8367B_CVIDXMAX < uiEntryId)
    {
        return DRV_ERR_PARA;
    }

    ClrPhyMaskAll(&stPhyMask);
    MaskLogic2Phy(&stLgcMask, &stPhyMask);

    UnitFor(uiUnit)
    {
        memset(&stPortBaseVlan,0,sizeof(stPortBaseVlan));
        /*Vlan index 31 is used for default vlan.*/
        if (RT_ERR_OK != rtk_vlan_mbrCfg_get(uiEntryId, &stPortBaseVlan))
        {
            return DRV_SDK_GEN_ERROR;
        }

        stPortBaseVlan.mbr &= (UINT16)(~(stPhyMask.pmask[uiUnit].bits[0]));                  

        if (RT_ERR_OK != rtk_vlan_mbrCfg_set(uiEntryId, &stPortBaseVlan))
        {
            return DRV_SDK_GEN_ERROR;  
        }
    }
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_SetPortVlanIngressFilter
  Description: Enable or disable port vlan ingress filter.
        Input: UINT32 uiLPort  
                BOOL bEnable    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortVlanIngressFilter(UINT32 uiLPort, BOOL bEnable)
{
    UINT32 uiPPort;
    rtk_enable_t tdEnable;
    
    if((!IsValidLgcPort(uiLPort)) ||
        ((TRUE != bEnable) && (FALSE != bEnable)))
    {
        return DRV_ERR_PARA;
    }

    uiPPort = PortLogic2PhyID(uiLPort);

    if (TRUE == bEnable)
    {
        tdEnable = ENABLED;
    }
    else
    {
        tdEnable = DISABLED;
    }
    
    if (RT_ERR_OK != rtk_vlan_portIgrFilterEnable_set(uiPPort, tdEnable))
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_SetPortVlanIngressMode
  Description: Set port recieve vlan mode.
        Input: UINT32 uiLPort                     
                PORT_INGRESS_MODE_E enIngressMode  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortVlanIngressMode( UINT32 uiLPort, PORT_INGRESS_MODE_E enIngressMode)
{
    UINT32 uiPPort;
    ret_t  RetVal = RT_ERR_OK;
    rtl8367b_accframetype enAccFrameType;
    
    if((!IsValidLgcPort(uiLPort)) ||
        (PORT_IN_FRAM_END <= enIngressMode))
    {
        return DRV_ERR_PARA;
    }


    uiPPort = PortLogic2PhyID(uiLPort);
    if (INVALID_PORT == uiPPort)
    {
        return DRV_ERR_UNKNOW;
    }

    switch (enIngressMode)
    {
        case PORT_IN_FRAM_BOTH:
            enAccFrameType = FRAME_TYPE_BOTH;
            break;
        case PORT_IN_FRAM_TAGGED_ONLY:
            enAccFrameType = FRAME_TYPE_TAGGED_ONLY;
            break;
        case PORT_IN_FRAM_UNTAGGED_ONLY:
            enAccFrameType = FRAME_TYPE_UNTAGGED_ONLY;
            break;
        default :
            return DRV_ERR_PARA;
    }
    
    RetVal = rtl8367b_setAsicVlanAccpetFrameType(uiPPort, enAccFrameType);
    if (RT_ERR_OK != RetVal)
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_SetPortVlanEgressMode
  Description: Set port egress vlan mode. 
        Input: UINT32 uiLPort                    
                PORT_EGRESS_MODE_E enEgressMode  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortVlanEgressMode( UINT32 uiLPort, PORT_EGRESS_MODE_E enEgressMode)
{
    UINT32 uiPPort;
    ret_t  RetVal = RT_ERR_OK;
    rtl8367b_egtagmode enEgTagMode;
    
    if((!IsValidLgcPort(uiLPort)) ||
        (PORT_EG_TAG_MODE_END <= enEgressMode))
    {
        return DRV_ERR_PARA;
    }

    uiPPort = PortLogic2PhyID(uiLPort);
    if (INVALID_PORT == uiPPort)
    {
        return DRV_ERR_UNKNOW;
    }

    switch (enEgressMode)
    {
        case PORT_EG_TAG_MODE_ORI:
            enEgTagMode = EG_TAG_MODE_ORI;
            break;
        case PORT_EG_TAG_MODE_KEEP:
            enEgTagMode = EG_TAG_MODE_KEEP;
            break;
        case PORT_EG_TAG_MODE_PRI_TAG:
            enEgTagMode = EG_TAG_MODE_PRI_TAG;
            break;
        case PORT_EG_TAG_MODE_REAL_KEEP:
            enEgTagMode = EG_TAG_MODE_REAL_KEEP;
            break;
        default :
            return DRV_ERR_PARA;
    }
    
    RetVal = rtl8367b_setAsicVlanEgressTagMode(uiPPort, enEgTagMode);
    if (RT_ERR_OK != RetVal)
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

#endif

/*  */
/*****************************************************************************
    Func Name: _Hal_CheckVidUsed
 
                                                                            
*****************************************************************************/
DRV_RET_E _Hal_CheckVidUsed(UINT32 uiLPort, UINT32 uiVid, BOOL *pFlag)
{
    UINT32 i;
    DRV_RET_E enRet;
    UINT32 uiLPortTmp;
    UINT32 uiOldVlanId;
    UINT32 uiNewVlanId;
    CTC_VLAN_CFG_S stCtcVlanMode;

    if ((!IsValidLgcPort(uiLPort)) || 
        (NULL == pFlag)            || 
        ((0 != uiVid) && (!VALID_VLAN_ID(uiVid))))
    {
        return DRV_ERR_PARA;
    }

    *pFlag = FALSE;

    /*check if the vid is used by other port*/
    for (uiLPortTmp = 1; uiLPortTmp <= LOGIC_PORT_NO; uiLPortTmp++)
    {
        if (uiLPortTmp == uiLPort)
        {
            continue;
        }

        enRet = Hal_CfgPortCtcVlanGet(uiLPortTmp, &stCtcVlanMode);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }

        if (CTC_VLAN_MODE_TAG == stCtcVlanMode.mode)
        {
            if (uiVid == stCtcVlanMode.default_vlan)
            {
                *pFlag = TRUE;
                break;
            }
        }
        else if (CTC_VLAN_MODE_TRANSLATION == stCtcVlanMode.mode)
        {
            if (uiVid == stCtcVlanMode.default_vlan)
            {
                *pFlag = TRUE;
                break;
            }

            for (i = 0; i < stCtcVlanMode.number_of_entries; i++)
            {
                uiOldVlanId = stCtcVlanMode.vlan_list[i*2];
                uiNewVlanId = stCtcVlanMode.vlan_list[(i*2)+1];

                if ((uiVid == uiOldVlanId) || (uiVid == uiNewVlanId))
                {
                    *pFlag = TRUE;
                    break;
                }
            }

            if (TRUE == *pFlag)
            {
                break;
            }
        }
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_CheckTranslPairUsed

  Description: Check if the translation pair is used by other port.
        Input: UINT32 uiLPort  
                UINT32 uiCvid   
                UINT32 uiSvid   
                BOOL *pFlag     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E _Hal_CheckTranslPairUsed(UINT32 uiLPort, UINT32 uiCvid, UINT32 uiSvid, BOOL *pFlag)
{
    UINT32 i;
    DRV_RET_E enRet;
    UINT32 uiLPortTmp;
    UINT32 uiOldVlanId;
    UINT32 uiNewVlanId;
    CTC_VLAN_CFG_S stCtcVlanMode;

    if ((!IsValidLgcPort(uiLPort)) || 
        (NULL == pFlag)            || 
        (!VALID_VLAN_ID(uiCvid))   ||
        (!VALID_VLAN_ID(uiSvid)))
    {
        return DRV_ERR_PARA;
    }

    *pFlag = FALSE;

    /*check if the vid is used by other port*/
    for (uiLPortTmp = 1; uiLPortTmp <= LOGIC_PORT_NO; uiLPortTmp++)
    {
        if (uiLPortTmp == uiLPort)
        {
            continue;
        }

        enRet = Hal_CfgPortCtcVlanGet(uiLPortTmp, &stCtcVlanMode);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }

        if (CTC_VLAN_MODE_TRANSLATION == stCtcVlanMode.mode)
        {
            for (i = 0; i < stCtcVlanMode.number_of_entries; i++)
            {
                uiOldVlanId = stCtcVlanMode.vlan_list[i*2];
                uiNewVlanId = stCtcVlanMode.vlan_list[(i*2)+1];

                if ((uiCvid == uiOldVlanId) && (uiSvid == uiNewVlanId))
                {
                    *pFlag = TRUE;
                    break;
                }
            }

            if (TRUE == *pFlag)
            {
                break;
            }
        }
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_SetPortTransparent
 
  Description: Set port transparent.
        Input: UINT32 uiEgLPort   
                UINT32 uiIngLPort  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E _Hal_SetPortTransparent(UINT32 uiEgLPort, UINT32 uiIngLPort)
{
    UINT32 uiEgPPort;
    UINT32 uiIngPPort;
    rtk_api_ret_t tdRet;
    rtk_enable_t bFalg = DISABLED;
    rtk_portmask_t stPPortMask;
    
    if ((!IsValidLgcPort(uiEgLPort)) || (!IsValidLgcPort(uiIngLPort)))
    {
        return DRV_ERR_PARA;
    }
    
    uiEgPPort = PortLogic2PhyID(uiEgLPort);
    uiIngPPort = PortLogic2PhyID(uiIngLPort);
    tdRet = rtk_vlan_transparent_get(uiEgPPort, &stPPortMask, &bFalg);
    if (RT_ERR_OK != tdRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    if (ENABLED == bFalg)
    {
        stPPortMask.bits[0] |= 1U << uiIngPPort;
    }
    else
    {
        stPPortMask.bits[0] = 1U << uiIngPPort;
    }
    
    tdRet = rtk_vlan_transparent_set(uiEgPPort, stPPortMask, ENABLED);
    if (RT_ERR_OK != tdRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_ResetPortTransparent
 
  Description: Reset port transparent.
        Input: UINT32 uiEgLPort   
                UINT32 uiIngLPort  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E _Hal_ResetPortTransparent(UINT32 uiEgLPort, UINT32 uiIngLPort)
{
    UINT32 uiEgPPort;
    UINT32 uiIngPPort;
    rtk_api_ret_t tdRet;
    rtk_enable_t bFalg = DISABLED;
    rtk_portmask_t stPPortMask;
    
    if ((!IsValidLgcPort(uiEgLPort)) || (!IsValidLgcPort(uiIngLPort)))
    {
        return DRV_ERR_PARA;
    }
    
    uiEgPPort = PortLogic2PhyID(uiEgLPort);
    uiIngPPort = PortLogic2PhyID(uiIngLPort);
    tdRet = rtk_vlan_transparent_get(uiEgPPort, &stPPortMask, &bFalg);
    if (RT_ERR_OK != tdRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    stPPortMask.bits[0] &= ~(1U << uiIngPPort);
    tdRet = rtk_vlan_transparent_set(uiEgPPort, stPPortMask, ENABLED);
    if (RT_ERR_OK != tdRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AddSvlanMember
 
  Description: Add svlan entry.
        Input: UINT32 ulSvlanId              
                UINT32 ulSvlanPri             
                logic_pmask_t stLgcMask       
                logic_pmask_t stLgcMaskUntag  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AddSvlanMember(UINT32 ulSvlanId, UINT32 ulSvlanPri, logic_pmask_t stLgcMask, logic_pmask_t stLgcMaskUntag)
{
    UINT32 ulUnit;
    rtk_api_ret_t ret;
    rtk_svlan_memberCfg_t stSvlan_cfg;
    phy_pmask_t stPhyMask, stPhyMaskUntag;

    if ((0 != ulSvlanId) && (!VALID_VLAN_ID(ulSvlanId)))
    {
        return DRV_ERR_PARA;
    }

    ClrPhyMaskAll(&stPhyMask);
    ClrPhyMaskAll(&stPhyMaskUntag);

    MaskLogic2Phy(&stLgcMask, &stPhyMask);
    MaskLogic2Phy(&stLgcMaskUntag, &stPhyMaskUntag);
 
    UnitFor(ulUnit)
    {
        memset(&stSvlan_cfg, 0, sizeof(stSvlan_cfg));
        ret = rtk_svlan_memberPortEntry_get((rtk_vlan_t)ulSvlanId, &stSvlan_cfg);
        if ((RT_ERR_OK != ret) && (RT_ERR_SVLAN_ENTRY_NOT_FOUND != ret))
        {
            return DRV_SDK_GEN_ERROR;
        }

        stSvlan_cfg.svid = ulSvlanId;
        stSvlan_cfg.priority = ulSvlanPri;
        stSvlan_cfg.memberport |= stPhyMask.pmask[ulUnit].bits[0];
        stSvlan_cfg.untagport  |= stPhyMaskUntag.pmask[ulUnit].bits[0];
        ret = rtk_svlan_memberPortEntry_set((rtk_vlan_t)ulSvlanId, &stSvlan_cfg);
        if(RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_RemoveSvlanMember

  Description: Remove port member from svlan entry.
        Input: UINT32 ulSvlanId         
                logic_pmask_t stLgcMask  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_RemoveSvlanMember(UINT32 ulSvlanId, logic_pmask_t stLgcMask)
{
    UINT32 ulUnit;
    rtk_api_ret_t ret;
    phy_pmask_t stPhyMask;
    rtk_svlan_memberCfg_t stSvlan_cfg;

    if ((0 != ulSvlanId) && (!VALID_VLAN_ID(ulSvlanId)))
    {
        return DRV_ERR_PARA;
    }

    ClrPhyMaskAll(&stPhyMask);
    MaskLogic2Phy(&stLgcMask, &stPhyMask);

    UnitFor(ulUnit)
    {
        memset(&stSvlan_cfg, 0, sizeof(stSvlan_cfg));
        ret = rtk_svlan_memberPortEntry_get((rtk_vlan_t)ulSvlanId, &stSvlan_cfg);
        if (RT_ERR_SVLAN_ENTRY_NOT_FOUND == ret)
        {
            return DRV_OK;
        }
        else if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }

        stSvlan_cfg.svid = ulSvlanId;
        stSvlan_cfg.memberport &= ~(stPhyMask.pmask[ulUnit].bits[0]);
        stSvlan_cfg.untagport  &= ~(stPhyMask.pmask[ulUnit].bits[0]);
        if ((0 == (stSvlan_cfg.memberport & ~(1U << PHY_UPLINK_PORT))) &&
            (0 != ulSvlanId))
        {
            /*delete svlan entry*/
            stSvlan_cfg.memberport = 0;
        }

        ret = rtk_svlan_memberPortEntry_set((rtk_vlan_t)ulSvlanId, &stSvlan_cfg);
        if(RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }  
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_SetPortDefaultSvid

  Description: Set port's default svlan id.
        Input: UINT32 uiLPort    
                UINT32 ulSvlanId  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortDefaultSvid(UINT32 uiLPort, UINT32 ulSvlanId)
{
    UINT32 uiPPort;
    rtk_api_ret_t ret;

    if (((0 != ulSvlanId) && (!VALID_VLAN_ID(ulSvlanId))) ||
        (!IsValidLgcPort(uiLPort)))
    {
        return DRV_ERR_PARA;
    }

    uiPPort = PortLogic2PhyID(uiLPort);
    ret = rtk_svlan_defaultSvlan_set(uiPPort,ulSvlanId);
    if(RT_ERR_OK != ret)
    {
        return DRV_SDK_GEN_ERROR;
    } 

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AddC2sEntry2Port

  Description: Add c2s entry to a port. 
               Packet with specified cvid tag will be assigned to defined svid tag.
               When this kind of packet egress from svlan service port, the action 
               of cvlan tag(tagged/untagged) decided by cvlan entry and the action
               of svlan tag(tagged/untagged) decided by svlan entry.
        Input: UINT32 uiLPort  
                UINT32 ulCvid   
                UINT32 ulSvid   
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AddC2sEntry2Port(UINT32 uiLPort, UINT32 ulCvid, UINT32 ulSvid)
{
    UINT32 uiPPort;
    rtk_api_ret_t ret;

    if (((0 != ulSvid) && (!VALID_VLAN_ID(ulSvid))) ||
        (!VALID_VLAN_ID(ulCvid))                    ||
        (!IsValidLgcPort(uiLPort)))
    {
        return DRV_ERR_PARA;
    }

    uiPPort = PortLogic2PhyID(uiLPort);
    ret = rtk_svlan_c2s_add(ulCvid, uiPPort, ulSvid);
    if(RT_ERR_OK != ret)
    {
        return DRV_SDK_GEN_ERROR;
    } 

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_DelC2sEntryFromPort
 
  Description: Delete a c2s entry from port.
        Input: UINT32 uiLPort  
                UINT32 ulCvid   
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_DelC2sEntryFromPort(UINT32 uiLPort, UINT32 ulCvid)
{
    UINT32 uiPPort;
    rtk_api_ret_t ret;

    if ((!VALID_VLAN_ID(ulCvid)) || (!IsValidLgcPort(uiLPort)))
    {
        return DRV_ERR_PARA;
    }

    uiPPort = PortLogic2PhyID(uiLPort);
    ret = rtk_svlan_c2s_del(ulCvid, uiPPort);
    if(RT_ERR_OK != ret)
    {
        return DRV_SDK_GEN_ERROR;
    } 

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_AddSp2cEntry2Port

  Description: Add sp2c entry to a port.
               Packet with specified svid tag will be assigned to defined cvid tag,
               when it ingresses in svlan service port.
               When this kind of packet egress from ports excluding svlan service port, 
               the action of cvlan tag(tagged/untagged) decided by cvlan entry and 
               the action of svlan tag(tagged/untagged) decided by svlan entry.
        Input: UINT32 uiLPort  
                UINT32 ulSvid   
                UINT32 ulCvid   
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_AddSp2cEntry2Port(UINT32 uiLPort, UINT32 ulSvid, UINT32 ulCvid)
{
    UINT32 uiPPort;
    rtk_api_ret_t ret;

    if (((0 != ulSvid) && (!VALID_VLAN_ID(ulSvid))) ||
        (!VALID_VLAN_ID(ulCvid))                    ||
        (!IsValidLgcPort(uiLPort)))
    {
        return DRV_ERR_PARA;
    }

    uiPPort = PortLogic2PhyID(uiLPort);
    ret = rtk_svlan_sp2c_add(ulSvid, uiPPort, ulCvid);
    if(RT_ERR_OK != ret)
    {
        return DRV_SDK_GEN_ERROR;
    } 

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_DelSp2cEntryFromPort

  Description: Delete sp2c entry from a port.
        Input: UINT32 uiLPort  
                UINT32 ulSvid   
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_DelSp2cEntryFromPort(UINT32 uiLPort, UINT32 ulSvid)
{
    UINT32 uiPPort;
    rtk_api_ret_t ret;

    if ((0 != ulSvid) && (!VALID_VLAN_ID(ulSvid)))
    {
        return DRV_ERR_PARA;
    }

    uiPPort = PortLogic2PhyID(uiLPort);
    ret = rtk_svlan_sp2c_del(ulSvid, uiPPort);
    if(RT_ERR_OK != ret)
    {
        return DRV_SDK_GEN_ERROR;
    } 

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_SetPortTransparentMode
 
  Description: set port ctc transparent vlan mode.
        Input: UINT32 uiLPort               
                CTC_VLAN_CFG_S *pstVlanMode  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortTransparentMode(UINT32 uiLPort, CTC_VLAN_CFG_S *pstVlanMode)
{
    UINT32 uiLPortTmp;
    DRV_RET_E enRet;
    CTC_VLAN_CFG_S stCtcVlanMode;
    logic_pmask_t stLgcMask;
    logic_pmask_t stLgcMaskUntag;

    if ((!IsValidLgcPort(uiLPort)) || (NULL == pstVlanMode))
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_ERR_PARA;
    }
    
    ClrLgcMaskAll(&stLgcMask);
    ClrLgcMaskAll(&stLgcMaskUntag);
    /*set svlan 0 entry*/ 
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
    SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMask);
    SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMaskUntag);
    enRet = Hal_AddSvlanMember(0, 0, stLgcMask, stLgcMaskUntag);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }

    /*set default svlan id*/
    enRet = Hal_SetPortDefaultSvid(uiLPort, 0);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }
    
    /*set default cvlan entry*/ 
    ClrLgcMaskAll(&stLgcMask);
    ClrLgcMaskAll(&stLgcMaskUntag);
    
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMaskUntag);
    enRet = Hal_SetVlanMemberAdd(0, stLgcMask, stLgcMaskUntag);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }
    
    /*set default cvlan id*/
    enRet = Hal_SetVlanPvid(uiLPort, 0);
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /*Keep transparent Port upstream packet format, packet from transparent Port 
      will not add S-tag in uplink port. */
    enRet = _Hal_SetPortTransparent(LOGIC_UPPON_PORT, uiLPort);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }
    
	/*Keep uplink Port downstream to transparent Port packet format*/	
    enRet = _Hal_SetPortTransparent(uiLPort, LOGIC_UPPON_PORT);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }

    /*disable ingress filter*/
    enRet = Hal_SetPortVlanIngressFilter(uiLPort, FALSE);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }
    
    /*renew non-transparent port vlan member*/
    for (uiLPortTmp = 1; uiLPortTmp <= LOGIC_PORT_NO; uiLPortTmp++)
    {
        if (uiLPortTmp == uiLPort)
        {
            continue;
        }

        enRet = Hal_CfgPortCtcVlanGet(uiLPortTmp, &stCtcVlanMode);
        if (DRV_OK != enRet)
        {
            printk("\nf:%s,l:%d\n",__func__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        if (CTC_VLAN_MODE_TAG == stCtcVlanMode.mode)
        {
            /*add svlan entry member for tag port's pvid*/
            ClrLgcMaskAll(&stLgcMask);
            ClrLgcMaskAll(&stLgcMaskUntag);
            SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
            enRet = Hal_AddSvlanMember(stCtcVlanMode.default_vlan, stCtcVlanMode.uiPriority, stLgcMask, stLgcMaskUntag);
            if (DRV_OK != enRet)
            {
                printk("\nf:%s,l:%d\n",__func__,__LINE__);
                return DRV_SDK_GEN_ERROR;
            }

            /*add cvlan entry member for tag port's pvid*/
            ClrLgcMaskAll(&stLgcMask);
            ClrLgcMaskAll(&stLgcMaskUntag);
            SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
            enRet = Hal_SetVlanMemberAdd(stCtcVlanMode.default_vlan, stLgcMask, stLgcMaskUntag);
            if (DRV_OK != enRet)
            {
                printk("\nf:%s,l:%d\n",__func__,__LINE__);
                return DRV_SDK_GEN_ERROR;
            }
        }
        else if (CTC_VLAN_MODE_TRANSLATION == stCtcVlanMode.mode)
        {
            UINT32 i;
            UINT32 uiOldVlanId;
            UINT32 uiNewVlanId;

            /*add svlan entry member for port's pvid*/
            ClrLgcMaskAll(&stLgcMask);
            ClrLgcMaskAll(&stLgcMaskUntag);
            SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
            enRet = Hal_AddSvlanMember(stCtcVlanMode.default_vlan, stCtcVlanMode.uiPriority, stLgcMask, stLgcMaskUntag);
            if (DRV_OK != enRet)
            {
                printk("\nf:%s,l:%d\n",__func__,__LINE__);
                return DRV_SDK_GEN_ERROR;
            }

            /*add cvlan entry member for port's pvid*/
            ClrLgcMaskAll(&stLgcMask);
            ClrLgcMaskAll(&stLgcMaskUntag);
            SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
            enRet = Hal_SetVlanMemberAdd(stCtcVlanMode.default_vlan, stLgcMask, stLgcMaskUntag);
            if (DRV_OK != enRet)
            {
                printk("\nf:%s,l:%d\n",__func__,__LINE__);
                return DRV_SDK_GEN_ERROR;
            }
            
            for (i = 0; i < stCtcVlanMode.number_of_entries; i++)
            {
                uiOldVlanId = stCtcVlanMode.vlan_list[i*2];
                uiNewVlanId = stCtcVlanMode.vlan_list[(i*2)+1];

                /*add cvlan entry member for tag port's old vid*/
                ClrLgcMaskAll(&stLgcMask);
                ClrLgcMaskAll(&stLgcMaskUntag);
                SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
                enRet = Hal_SetVlanMemberAdd(uiOldVlanId, stLgcMask, stLgcMaskUntag);
                if (DRV_OK != enRet)
                {
                    printk("\nf:%s,l:%d\n",__func__,__LINE__);
                    return DRV_SDK_GEN_ERROR;
                }

                /*add svlan entry member for tag port's new vid*/
                ClrLgcMaskAll(&stLgcMask);
                ClrLgcMaskAll(&stLgcMaskUntag);
                SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
                enRet = Hal_AddSvlanMember(uiNewVlanId, 0, stLgcMask, stLgcMaskUntag);
                if (DRV_OK != enRet)
                {
                    printk("\nf:%s,l:%d\n",__func__,__LINE__);
                    return DRV_SDK_GEN_ERROR;
                }
            }
        }
        else 
        {
            /*Keep packet format egress from this port*/	
            enRet = _Hal_SetPortTransparent(uiLPort, uiLPortTmp);
            if (DRV_OK != enRet)
            {
                printk("\nf:%s,l:%d\n",__func__,__LINE__);
                return DRV_SDK_GEN_ERROR;
            }

            /*Keep packet format ingress into this port and egress from other transparent port*/	
            enRet = _Hal_SetPortTransparent(uiLPortTmp, uiLPort);
            if (DRV_OK != enRet)
            {
                printk("\nf:%s,l:%d\n",__func__,__LINE__);
                return DRV_SDK_GEN_ERROR;
            }
        }
    }

    (void)Hal_CfgPortCtcVlanSet(uiLPort, pstVlanMode);

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_ResetPortTransparentMode
 Date Created: 2012/8/24 
       Author: x06333
  Description: Reset port ctc transparent vlan mode.
        Input: UINT32 uiLPort               
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_ResetPortTransparentMode(UINT32 uiLPort)
{
    UINT32 uiLPortTmp;
    DRV_RET_E enRet;
    CTC_VLAN_CFG_S stCtcVlanMode;
    logic_pmask_t stLgcMask;

    if (!IsValidLgcPort(uiLPort))
    {
        return DRV_ERR_PARA;
    }
    
    ClrLgcMaskAll(&stLgcMask);
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
    /*resset svlan 0 entry*/ 
    enRet = Hal_RemoveSvlanMember(0, stLgcMask);
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /*retset default cvlan entry*/ 
    enRet = Hal_SetVlanMemberRemove(0, stLgcMask);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }

    /*undo upstream packet transparent. */
    enRet = _Hal_ResetPortTransparent(LOGIC_UPPON_PORT, uiLPort);
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

	/*undo downstream packet transparent*/	
    enRet = _Hal_ResetPortTransparent(uiLPort, LOGIC_UPPON_PORT);
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /*enable ingress filter*/
    enRet = Hal_SetPortVlanIngressFilter(uiLPort, TRUE);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }
    
    /*renew non-transparent port vlan member*/
    ClrLgcMaskAll(&stLgcMask);
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
    for (uiLPortTmp = 1; uiLPortTmp <= LOGIC_PORT_NO; uiLPortTmp++)
    {
        if (uiLPortTmp == uiLPort)
        {
            continue;
        }

        enRet = Hal_CfgPortCtcVlanGet(uiLPortTmp, &stCtcVlanMode);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }

        if (CTC_VLAN_MODE_TAG == stCtcVlanMode.mode)
        {
            /*remove svlan entry member for tag port's pvid*/
            #if 0
            ClrLgcMaskAll(&stLgcMask);
            SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
            #endif
            enRet = Hal_RemoveSvlanMember(stCtcVlanMode.default_vlan, stLgcMask);
            if (DRV_OK != enRet)
            {
                return DRV_SDK_GEN_ERROR;
            }

            /*remove cvlan entry member for tag port's pvid*/
            #if 0
            ClrLgcMaskAll(&stLgcMask);
            SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
            #endif
            enRet = Hal_SetVlanMemberRemove(stCtcVlanMode.default_vlan, stLgcMask);
            if (DRV_OK != enRet)
            {
                return DRV_SDK_GEN_ERROR;
            }
        }
        else if (CTC_VLAN_MODE_TRANSLATION == stCtcVlanMode.mode)
        {
            UINT32 i;
            UINT32 uiOldVlanId;
            UINT32 uiNewVlanId;

            /*remove svlan entry member for tag port's pvid*/
            #if 0
            ClrLgcMaskAll(&stLgcMask);
            SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
            #endif
            enRet = Hal_RemoveSvlanMember(stCtcVlanMode.default_vlan, stLgcMask);
            if (DRV_OK != enRet)
            {
                return DRV_SDK_GEN_ERROR;
            }

            /*remove cvlan entry member for tag port's pvid*/
            #if 0
            ClrLgcMaskAll(&stLgcMask);
            SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
            #endif
            enRet = Hal_SetVlanMemberRemove(stCtcVlanMode.default_vlan, stLgcMask);
            if (DRV_OK != enRet)
            {
                return DRV_SDK_GEN_ERROR;
            }
            
            for (i = 0; i < stCtcVlanMode.number_of_entries; i++)
            {
                uiOldVlanId = stCtcVlanMode.vlan_list[i*2];
                uiNewVlanId = stCtcVlanMode.vlan_list[(i*2)+1];

                /*remove cvlan entry member for tag port's old vid*/
                #if 0
                ClrLgcMaskAll(&stLgcMask);
                #endif
                enRet = Hal_SetVlanMemberRemove(uiOldVlanId, stLgcMask);
                if (DRV_OK != enRet)
                {
                    printk("\nf:%s,l:%d\n",__func__,__LINE__);
                    return DRV_SDK_GEN_ERROR;
                }

                /*remove svlan entry member for tag port's new vid*/
                #if 0
                ClrLgcMaskAll(&stLgcMask);
                SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
                #endif
                enRet = Hal_RemoveSvlanMember(uiNewVlanId, stLgcMask);
                if (DRV_OK != enRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
            }
        }
        #if 0
        else 
        {
            /*undo packet transparent between transparent ports*/	
            enRet = _Hal_ResetPortTransparent(uiLPort, uiLPortTmp);
            if (DRV_OK != enRet)
            {
                return DRV_SDK_GEN_ERROR;
            }
        }
        #else
        /*undo packet transparent egress from this port*/	
        enRet = _Hal_ResetPortTransparent(uiLPort, uiLPortTmp);
        if (DRV_OK != enRet)
        {
            return DRV_SDK_GEN_ERROR;
        }

        /*undo packet transparent ingress into this port*/	
        enRet = _Hal_ResetPortTransparent(uiLPortTmp, uiLPort);
        if (DRV_OK != enRet)
        {
            return DRV_SDK_GEN_ERROR;
        }
        #endif
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_SetPortTranslationMode
 Date Created: 2012/8/24 
       Author: x06333
  Description: set port ctc translation vlan mode.
        Input: UINT32 uiLPort               
                CTC_VLAN_CFG_S *pstVlanMode  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
extern DRV_RET_E ACL_CreateRuleForDropVid(UINT32 uiLPortId, UINT32 uiVid, UINT32 *puiAclId);
DRV_RET_E Hal_SetPortTranslationMode(UINT32 uiLPort, CTC_VLAN_CFG_S *pstVlanMode)
{
    UINT32 i;
    UINT32 uiOldVlanId;
    UINT32 uiNewVlanId;
    UINT32 uiPPort;
    UINT32 uiLPortTmp;
    UINT32 uiAclId;
    DRV_RET_E enRet;
    CTC_VLAN_CFG_S stCtcVlanMode;
    logic_pmask_t stLgcMask;
    logic_pmask_t stLgcMaskUntag;
    logic_pmask_t stTranspLgcMask;
    rtk_api_ret_t tdRet;
    BOOL bAccessMode = FALSE;

    if ((!IsValidLgcPort(uiLPort)) || (NULL == pstVlanMode))
    {
        return DRV_ERR_PARA;
    }
#if 0
    /*use svlan priority for down stream.*/
    /*here force the qos trusting mode.
    the original trusting mode has to be restored when reset translation vlan.*/
    enRet = Hal_SetQosTrustMode(QOS_SVLAN);
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }
#endif
    /*enable cos remarking. 
      it associating befor action for down stream's cvlan priority.*/
    uiPPort = PortLogic2PhyID(uiLPort);
    tdRet = rtk_qos_1pRemarkEnable_set(uiPPort, ENABLED);
    if (RT_ERR_OK != tdRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    uiPPort = PortLogic2PhyID(LOGIC_UPPON_PORT);
    tdRet = rtk_qos_1pRemarkEnable_set(uiPPort, ENABLED);
    if (RT_ERR_OK != tdRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /*get transparent port mask*/
    ClrLgcMaskAll(&stTranspLgcMask);
    for (uiLPortTmp = 1; uiLPortTmp <= LOGIC_PORT_NO; uiLPortTmp++)
    {
        if (uiLPortTmp == uiLPort)
        {
            continue;
        }

        (void)Hal_CfgPortCtcVlanGet(uiLPortTmp, &stCtcVlanMode);
        if ((CTC_VLAN_MODE_TRANSPARENT == stCtcVlanMode.mode) ||
            (CTC_VLAN_MODE_QINQ == stCtcVlanMode.mode))
        {
            SetLgcMaskBit((port_num_t)uiLPortTmp, &stTranspLgcMask);
        }
    }

    /*set default svlan entry*/
    ClrLgcMaskAll(&stLgcMask);
    ClrLgcMaskAll(&stLgcMaskUntag);
    
    memcpy(&stLgcMask, &stTranspLgcMask, sizeof(stLgcMask));
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMaskUntag);
    SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMask); 
    enRet = Hal_AddSvlanMember(pstVlanMode->default_vlan, pstVlanMode->uiPriority, stLgcMask, stLgcMaskUntag);
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /*set default svlan id*/
    enRet = Hal_SetPortDefaultSvid(uiLPort, pstVlanMode->default_vlan);
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /*set default cvlan entry*/ 
    ClrLgcMaskAll(&stLgcMask);
    ClrLgcMaskAll(&stLgcMaskUntag);

    memcpy(&stLgcMask, &stTranspLgcMask, sizeof(stLgcMask));
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMaskUntag);
    SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMask);
    SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMaskUntag);
    enRet = Hal_SetVlanMemberAdd(pstVlanMode->default_vlan, stLgcMask, stLgcMaskUntag);
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /*set default cvlan id*/
    enRet = Hal_SetVlanPvid(uiLPort, (pstVlanMode->default_vlan | (pstVlanMode->uiPriority << 12)));
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

#if 0
    /*enable ingress filter*/
    enRet = Hal_SetPortVlanIngressFilter(uiLPort, TRUE);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }
#endif    
    /*Drop frames with pvid tag*/
    /* For access vlan mode, packets with pvid tag should not be dropped. */
    for (i = 0; i < pstVlanMode->number_of_entries; i++)
    {
        uiOldVlanId = pstVlanMode->vlan_list[i*2];
        uiNewVlanId = pstVlanMode->vlan_list[(i*2)+1];

        if ((uiOldVlanId == uiNewVlanId) &&
            (pstVlanMode->default_vlan == uiNewVlanId))
        {
            bAccessMode = TRUE;
            break;
        }
    }

    if (FALSE == bAccessMode)
    {
        enRet = ACL_CreateRuleForDropVid(uiLPort, pstVlanMode->default_vlan, &uiAclId);
        if (DRV_OK != enRet)
        {
            return DRV_SDK_GEN_ERROR;
        }

        pstVlanMode->number_of_acl_rules = 0;
        pstVlanMode->acl_list[0] = uiAclId;
        pstVlanMode->number_of_acl_rules++;
    }
    else
    {
        pstVlanMode->acl_list[0] = ACL_RULE_NUM_MAX;
        pstVlanMode->number_of_acl_rules = 0;
    }
  
    /*renew transparent port vlan member*/
    for (i = 0; i < pstVlanMode->number_of_entries; i++)
    {
        uiOldVlanId = pstVlanMode->vlan_list[i<<1];
        uiNewVlanId = pstVlanMode->vlan_list[(i<<1)+1];
        /*for access mode*/
        if ((uiOldVlanId == uiNewVlanId) &&
            (pstVlanMode->default_vlan == uiNewVlanId))
        {
            continue;
        }
        
        /*add cvlan entry member for port's old vid*/
        ClrLgcMaskAll(&stLgcMask);
        ClrLgcMaskAll(&stLgcMaskUntag);

        memcpy(&stLgcMask, &stTranspLgcMask, sizeof(stLgcMask));
        SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
        SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMask);
        SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMaskUntag);
        enRet = Hal_SetVlanMemberAdd(uiOldVlanId, stLgcMask, stLgcMaskUntag);
        if (DRV_OK != enRet)
        {
            return DRV_SDK_GEN_ERROR;
        }

        /*add svlan entry member for port's new vid*/
        ClrLgcMaskAll(&stLgcMask);
        ClrLgcMaskAll(&stLgcMaskUntag);

        memcpy(&stLgcMask, &stTranspLgcMask, sizeof(stLgcMask));
        SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
        SetLgcMaskBit((port_num_t)uiLPort, &stLgcMaskUntag);
        SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMask);
        enRet = Hal_AddSvlanMember(uiNewVlanId, 0, stLgcMask, stLgcMaskUntag);
        if (DRV_OK != enRet)
        {
            return DRV_SDK_GEN_ERROR;
        }

        /*add c2s entry*/
        enRet = Hal_AddC2sEntry2Port(uiLPort, uiOldVlanId, uiNewVlanId);
        if (DRV_OK != enRet)
        {
            return DRV_SDK_GEN_ERROR;
        }

        /*add sp2c entry*/
        enRet = Hal_AddSp2cEntry2Port(uiLPort, uiNewVlanId, uiOldVlanId);
        if (DRV_OK != enRet)
        {
            return DRV_SDK_GEN_ERROR;
        }

    }

    (void)Hal_CfgPortCtcVlanSet(uiLPort, pstVlanMode);

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_ResetPortTranslationMode

  Description: Reset the translation vlan mode.
        Input: UINT32 uiLPort  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
extern DRV_RET_E ACL_DeleteRuleByAclid(UINT32 uiAclRuleId);
DRV_RET_E Hal_ResetPortTranslationMode(UINT32 uiLPort)
{
    BOOL bFind = FALSE;
    UINT32 i;
    UINT32 uiPvid;
    UINT32 uiOldVlanId;
    UINT32 uiNewVlanId;
    UINT32 uiPPort;
    UINT32 uiLPortTmp;
    DRV_RET_E enRet;
    CTC_VLAN_CFG_S stCtcVlanMode;
    logic_pmask_t stLgcMask;
    logic_pmask_t stTranspLgcMask;
    rtk_api_ret_t tdRet;

    if (!IsValidLgcPort(uiLPort))
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_ERR_PARA;
    }

    /*restore the original trusting mode.*/
    #if 0
    enRet = Hal_SetQosTrustMode(QOS_SVLAN);
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }
    #endif

    /*disable cos remarking.*/
    uiPPort = PortLogic2PhyID(uiLPort);
    tdRet = rtk_qos_1pRemarkEnable_set(uiPPort, DISABLED);
    if (RT_ERR_OK != tdRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }
    uiPPort = PortLogic2PhyID(LOGIC_UPPON_PORT);
    tdRet = rtk_qos_1pRemarkEnable_set(uiPPort, DISABLED);
    if (RT_ERR_OK != tdRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /*get transparent port mask*/
    ClrLgcMaskAll(&stTranspLgcMask);
    for (uiLPortTmp = 1; uiLPortTmp <= LOGIC_PORT_NO; uiLPortTmp++)
    {
        if (uiLPortTmp == uiLPort)
        {
            continue;
        }

        enRet = Hal_CfgPortCtcVlanGet(uiLPortTmp, &stCtcVlanMode);
        if (DRV_OK != enRet)
        {
            printk("\nf:%s,l:%d\n",__func__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        if ((CTC_VLAN_MODE_TRANSPARENT == stCtcVlanMode.mode) ||
            (CTC_VLAN_MODE_QINQ == stCtcVlanMode.mode))
        {
            SetLgcMaskBit((port_num_t)uiLPortTmp, &stTranspLgcMask);
        }
    }

    enRet = Hal_CfgPortCtcVlanGet(uiLPort, &stCtcVlanMode);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
    uiPvid = stCtcVlanMode.default_vlan;
    /*check if the pvid is used by other port*/
    enRet = _Hal_CheckVidUsed(uiLPort, uiPvid, &bFind);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_ERR_UNKNOW;
    }
    
    /*renew default svlan entry*/
    ClrLgcMaskAll(&stLgcMask);

    /*pvid svlan entry will be deleted if it is not used by other port.*/
    if (FALSE == bFind)
    {
        memcpy(&stLgcMask, &stTranspLgcMask, sizeof(stLgcMask));
        SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMask);
    }
    
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
    enRet = Hal_RemoveSvlanMember(uiPvid, stLgcMask);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }

    enRet = Hal_SetVlanMemberRemove(uiPvid, stLgcMask);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    } 

    /*delete acl rule used by dropping pvid*/
    if (0 != stCtcVlanMode.number_of_acl_rules)
    {
        enRet = ACL_DeleteRuleByAclid(stCtcVlanMode.acl_list[0]);
        if (DRV_OK != enRet)
        {
            printk("\nf:%s,l:%d\n",__func__,__LINE__);
            return DRV_SDK_GEN_ERROR;
        }

        stCtcVlanMode.number_of_acl_rules = 0;
        stCtcVlanMode.acl_list[0] = ACL_RULE_NUM_MAX;
    }
    
    /*renew translation port vlan member*/
    for (i = 0; i < stCtcVlanMode.number_of_entries; i++)
    {
        uiOldVlanId = stCtcVlanMode.vlan_list[i*2];
        uiNewVlanId = stCtcVlanMode.vlan_list[(i*2)+1];

        /*for access mode*/
        if ((uiOldVlanId == uiNewVlanId) &&
            (stCtcVlanMode.default_vlan == uiNewVlanId))
        {
            continue;
        }

        /*delete c2s and sp2c and sp2c entry*/
        /**/
        #if 0
        enRet = _Hal_CheckTranslPairUsed(uiLPort, uiOldVlanId, uiNewVlanId,&bFind);
        if (DRV_OK != enRet)
        {
            printk("\nf:%s,l:%d\n",__func__,__LINE__);
            return DRV_SDK_GEN_ERROR;
        }

        if (FALSE == bFind)
        {
            enRet = Hal_DelC2sEntryFromPort(uiLPort, uiOldVlanId);
            if (DRV_OK != enRet)
            {
                printk("\nf:%s,l:%d\n",__func__,__LINE__);
                return DRV_SDK_GEN_ERROR;
            }

            enRet = Hal_DelSp2cEntryFromPort(uiLPort, uiNewVlanId);
            if (DRV_OK != enRet)
            {
                printk("\nf:%s,l:%d\n",__func__,__LINE__);
                return DRV_SDK_GEN_ERROR;
            }
        }
        #else
        /*delete port from c2s and s2c entry*/
        enRet = Hal_DelC2sEntryFromPort(uiLPort, uiOldVlanId);
        if (DRV_OK != enRet)
        {
            printk("\nf:%s,l:%d\n",__func__,__LINE__);
            return DRV_SDK_GEN_ERROR;
        }

        enRet = Hal_DelSp2cEntryFromPort(uiLPort, uiNewVlanId);
        if (DRV_OK != enRet)
        {
            printk("\nf:%s,l:%d\n",__func__,__LINE__);
            return DRV_SDK_GEN_ERROR;
        }
        #endif
        /* 014528 */

        /*renew cvlan entry member for port's old vid*/
        ClrLgcMaskAll(&stLgcMask);

        /*check if the old vid is used by other port*/
        enRet = _Hal_CheckVidUsed(uiLPort, uiOldVlanId, &bFind);
        if (DRV_OK != enRet)
        {
            printk("\nf:%s,l:%d\n",__func__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        /*delete the cvlan entry if it's no not used by any other port*/
        if (FALSE == bFind)
        {
            memcpy(&stLgcMask, &stTranspLgcMask, sizeof(stLgcMask));
            SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMask);
        }
        
        SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
        enRet = Hal_SetVlanMemberRemove(uiOldVlanId, stLgcMask);
        if (DRV_OK != enRet)
        {
            printk("\nf:%s,l:%d\n",__func__,__LINE__);
            return DRV_SDK_GEN_ERROR;
        }

        /*renew svlan entry member for port's new vid*/
        ClrLgcMaskAll(&stLgcMask);

        /*check if the new vid is used by other port*/
        enRet = _Hal_CheckVidUsed(uiLPort, uiNewVlanId, &bFind);
        if (DRV_OK != enRet)
        {
            printk("\nf:%s,l:%d\n",__func__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        /*delete the new svlan entry if it is not used by any other port*/
        if (FALSE == bFind)
        {
            memcpy(&stLgcMask, &stTranspLgcMask, sizeof(stLgcMask));
            SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMask);
        }

        SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
        enRet = Hal_RemoveSvlanMember(uiNewVlanId, stLgcMask);
        if (DRV_OK != enRet)
        {
            printk("\nf:%s,l:%d\n",__func__,__LINE__);
            return DRV_SDK_GEN_ERROR;
        }
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_SetPortTagMode
 
  Description: set port ctc tag  vlan mode.
        Input: UINT32 uiLPort               
                CTC_VLAN_CFG_S *pstVlanMode  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortTagMode(UINT32 uiLPort, CTC_VLAN_CFG_S *pstVlanMode)
{
    //UINT32 uiPPort;
    UINT32 uiLPortTmp;
    DRV_RET_E enRet;
    CTC_VLAN_CFG_S stCtcVlanMode;
    logic_pmask_t stLgcMask;
    logic_pmask_t stLgcMaskUntag;
    logic_pmask_t stTranspLgcMask;
    //rtk_api_ret_t tdRet;

    if ((!IsValidLgcPort(uiLPort)) || (NULL == pstVlanMode))
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_ERR_PARA;
    }

    /*Keep transparent Port upstream packet format, packet from transparent Port 
      will not add S-tag in uplink port. */
    enRet = _Hal_SetPortTransparent(LOGIC_UPPON_PORT, uiLPort);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }
    
	/*Keep uplink Port downstream to transparent Port packet format*/	
    enRet = _Hal_SetPortTransparent(uiLPort, LOGIC_UPPON_PORT);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }
#if 0
    /*enable ingress filter*/
    enRet = Hal_SetPortVlanIngressFilter(uiLPort, TRUE);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }
#endif    
    /*get transparent port mask*/
    ClrLgcMaskAll(&stTranspLgcMask);
    for (uiLPortTmp = 1; uiLPortTmp <= LOGIC_PORT_NO; uiLPortTmp++)
    {
        if (uiLPortTmp == uiLPort)
        {
            continue;
        }

        enRet = Hal_CfgPortCtcVlanGet(uiLPortTmp, &stCtcVlanMode);
        if (DRV_OK != enRet)
        {
            printk("\nf:%s,l:%d\n",__func__,__LINE__);
            return DRV_ERR_UNKNOW;
        }

        if ((CTC_VLAN_MODE_TRANSPARENT == stCtcVlanMode.mode) ||
            (CTC_VLAN_MODE_QINQ == stCtcVlanMode.mode))
        {
            SetLgcMaskBit((port_num_t)uiLPortTmp, &stTranspLgcMask);
        }
    }

    /*set default svlan entry*/
    ClrLgcMaskAll(&stLgcMask);
    ClrLgcMaskAll(&stLgcMaskUntag);
    
    memcpy(&stLgcMask, &stTranspLgcMask, sizeof(stLgcMask));
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMaskUntag);
    SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMask); 
    enRet = Hal_AddSvlanMember(pstVlanMode->default_vlan, pstVlanMode->uiPriority, stLgcMask, stLgcMaskUntag);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }

    /*set default svlan id*/
    enRet = Hal_SetPortDefaultSvid(uiLPort, pstVlanMode->default_vlan);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d,port:%d,vid:%d\n",__func__,__LINE__,uiLPort,pstVlanMode->default_vlan);
        return DRV_SDK_GEN_ERROR;
    }

    /*set default cvlan entry*/ 
    ClrLgcMaskAll(&stLgcMask);
    ClrLgcMaskAll(&stLgcMaskUntag);

    memcpy(&stLgcMask, &stTranspLgcMask, sizeof(stLgcMask));
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMaskUntag);
    SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMask);
    SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMaskUntag);
    enRet = Hal_SetVlanMemberAdd(pstVlanMode->default_vlan, stLgcMask, stLgcMaskUntag);
    if (DRV_OK != enRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }

    /*set default cvlan id*/
    enRet = Hal_SetVlanPvid(uiLPort, (pstVlanMode->default_vlan | (pstVlanMode->uiPriority << 12)));
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /* */
    /*The frame tagged with mc-vlan must be transmitted by ctc tag port.
    So we have to use acl rule to drop the frame tagged with pvid.*/
    #if 0
    uiPPort = PortLogic2PhyID(uiLPort);
    tdRet = rtk_vlan_portAcceptFrameType_set(uiPPort, ACCEPT_FRAME_TYPE_UNTAG_ONLY);
    if (RT_ERR_OK != tdRet)
    {
        printk("\nf:%s,l:%d\n",__func__,__LINE__);
        return DRV_SDK_GEN_ERROR;
    }
    #else
    {
        UINT32 uiAclId;
        enRet = ACL_CreateRuleForDropVid(uiLPort, pstVlanMode->default_vlan, &uiAclId);
        if (DRV_OK != enRet)
        {
            return DRV_SDK_GEN_ERROR;
        }

        pstVlanMode->number_of_acl_rules = 0;
        pstVlanMode->acl_list[0] = uiAclId;
        pstVlanMode->number_of_acl_rules++;
    }
    #endif
    /*013987 */

    (void)Hal_CfgPortCtcVlanSet(uiLPort, pstVlanMode);

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_ResetPortTagMode
 
  Description: Reset ctc tag vlan mode.
        Input: UINT32 uiLPort  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_ResetPortTagMode(UINT32 uiLPort)
{
    BOOL bFind;
    //UINT32 uiPPort;
    UINT32 uiLPortTmp;
    DRV_RET_E enRet;
    CTC_VLAN_CFG_S stCtcVlanMode;
    logic_pmask_t stLgcMask;
    //logic_pmask_t stLgcMaskUntag;
    logic_pmask_t stTranspLgcMask;
    //rtk_api_ret_t tdRet;

    if (!IsValidLgcPort(uiLPort))
    {
        return DRV_ERR_PARA;
    }

    /*undo upstream packet transparent. */
    enRet = _Hal_ResetPortTransparent(LOGIC_UPPON_PORT, uiLPort);
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

	/*undo downstream packet transparent*/	
    enRet = _Hal_ResetPortTransparent(uiLPort, LOGIC_UPPON_PORT);
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /*get transparent port mask*/
    ClrLgcMaskAll(&stTranspLgcMask);
    for (uiLPortTmp = 1; uiLPortTmp <= LOGIC_PORT_NO; uiLPortTmp++)
    {
        if (uiLPortTmp == uiLPort)
        {
            continue;
        }

        enRet = Hal_CfgPortCtcVlanGet(uiLPortTmp, &stCtcVlanMode);
        if (DRV_OK != enRet)
        {
            return DRV_ERR_UNKNOW;
        }

        if ((CTC_VLAN_MODE_TRANSPARENT == stCtcVlanMode.mode) ||
            (CTC_VLAN_MODE_QINQ == stCtcVlanMode.mode))
        {
            SetLgcMaskBit((port_num_t)uiLPortTmp, &stTranspLgcMask);
        }
    }

    enRet = Hal_CfgPortCtcVlanGet(uiLPort, &stCtcVlanMode);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }
    
    /*check if the pvid is used by other port*/
    enRet = _Hal_CheckVidUsed(uiLPort, stCtcVlanMode.default_vlan, &bFind);
    if (DRV_OK != enRet)
    {
        return DRV_ERR_UNKNOW;
    }
    
    /*renew default svlan entry*/
    ClrLgcMaskAll(&stLgcMask);
    //ClrLgcMaskAll(&stLgcMaskUntag);

    if (FALSE == bFind)
    {
        memcpy(&stLgcMask, &stTranspLgcMask, sizeof(stLgcMask));
        SetLgcMaskBit((port_num_t)(LOGIC_UPPON_PORT), &stLgcMask); 
    }
    
    SetLgcMaskBit((port_num_t)uiLPort, &stLgcMask);
    enRet = Hal_RemoveSvlanMember(stCtcVlanMode.default_vlan, stLgcMask);
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /*renew default cvlan entry*/ 
    enRet = Hal_SetVlanMemberRemove(stCtcVlanMode.default_vlan, stLgcMask);
    if (DRV_OK != enRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    /* */
    #if 0
    uiPPort = PortLogic2PhyID(uiLPort);
    tdRet = rtk_vlan_portAcceptFrameType_set(uiPPort, ACCEPT_FRAME_TYPE_ALL);
    if (RT_ERR_OK != tdRet)
    {
        return DRV_SDK_GEN_ERROR;
    }
    #else
    /*delete acl rule used by dropping pvid*/
    if (0 != stCtcVlanMode.number_of_acl_rules)
    {
        enRet = ACL_DeleteRuleByAclid(stCtcVlanMode.acl_list[0]);
        if (DRV_OK != enRet)
        {
            printk("\nf:%s,l:%d\n",__func__,__LINE__);
            return DRV_SDK_GEN_ERROR;
        }

        stCtcVlanMode.number_of_acl_rules = 0;
        stCtcVlanMode.acl_list[0] = ACL_RULE_NUM_MAX;
    }
    #endif
    /*  */

    return DRV_OK;
}
/* 014453 */

/*  */
/*****************************************************************************
    Func Name: Hal_GetCvlanEntry
 Date Created: 2012/9/18 
       Author: x06333
  Description: Get cvlan entry.
        Input: UINT32 uiCvlanId                
                UINT32 *puiFid                  
                UINT32 *puiPhyMsk      
                UINT32 *puiPhyMskUntag
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetCvlanEntry(UINT32 uiCvlanId, UINT32 *puiPhyMsk, UINT32 *puiPhyMskUntag, UINT32 *puiFid)
{
    UINT32 ulUnit;
    phy_pmask_t stPhyMaskTmp, stPhyMaskUntagTmp;
    rtk_fid_t fid = 0;
    rtk_api_ret_t ret;

    if ((NULL == puiFid) ||
        (NULL == puiPhyMsk) || 
        (NULL == puiPhyMskUntag) ||
        ((0 != uiCvlanId) && (!VALID_VLAN_ID(uiCvlanId))))
    {
        return DRV_ERR_PARA;
    }

    ClrPhyMaskAll(&stPhyMaskTmp);
    ClrPhyMaskAll(&stPhyMaskUntagTmp);

    *puiPhyMsk = 0;
    *puiPhyMskUntag = 0;

    UnitFor(ulUnit)
    {
        ret = rtk_vlan_get((rtk_vlan_t)uiCvlanId, &(stPhyMaskTmp.pmask[ulUnit]), &(stPhyMaskUntagTmp.pmask[ulUnit]), &fid);
        if (RT_ERR_OK != ret)
        {
            return DRV_SDK_GEN_ERROR;
        }

        *puiFid = fid;
        *puiPhyMsk      = stPhyMaskTmp.pmask[ulUnit].bits[0];
        *puiPhyMskUntag = stPhyMaskUntagTmp.pmask[ulUnit].bits[0];
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GetSvlanMember
 Date Created: 2012/9/18 
       Author: x06333
  Description: Get svlan entry.
        Input: UINT32 uiSvlanId                
                UINT32 *puiFid
                logic_pmask_t *pstLgcMask       
                logic_pmask_t *pstLgcMaskUntag  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetSvlanEntry(UINT32 uiSvlanId, UINT32 *puiPhyMsk, UINT32 *puiPhyMskUntag, UINT32 *puiFid)
{
    UINT32 uiUnit;
    rtk_api_ret_t ret;
    rtk_svlan_memberCfg_t stSvlan_cfg;

    if ((NULL == puiFid) ||
        (NULL == puiPhyMsk) || 
        (NULL == puiPhyMskUntag) ||
        ((0 != uiSvlanId) && (!VALID_VLAN_ID(uiSvlanId))))
    {
        return DRV_ERR_PARA;
    }

    *puiPhyMsk = 0;
    *puiPhyMskUntag = 0;

    UnitFor(uiUnit)
    {
        memset(&stSvlan_cfg, 0, sizeof(stSvlan_cfg));
        ret = rtk_svlan_memberPortEntry_get((rtk_vlan_t)uiSvlanId, &stSvlan_cfg);
        if ((RT_ERR_OK != ret) && (RT_ERR_SVLAN_ENTRY_NOT_FOUND != ret))
        {
            return DRV_SDK_GEN_ERROR;
        }

        *puiFid = stSvlan_cfg.fid;
        *puiPhyMsk = stSvlan_cfg.memberport;
        *puiPhyMskUntag = stSvlan_cfg.untagport;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GetC2sEntryByIndex
 Date Created: 2012/9/18 
       Author: x06333
  Description: Get c2s entry by index.
        Input: UINT32 uiIndex       
                UINT32 *puiCvid      
                UINT32 *puiSvid      
                UINT32 *puiPPortMsk  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetC2sEntryByIndex(UINT32 uiIndex, UINT32 *puiCvid, UINT32 *puiSvid, UINT32 *puiPPortMsk)
{
    ret_t ret;
    UINT32 uiSvidIndex;
    rtl8367b_svlan_memconf_t stSvlanEntry;

    if ((RTL8367B_C2SIDXMAX < uiIndex) ||
        (NULL == puiCvid) ||
        (NULL == puiSvid) ||
        (NULL == puiPPortMsk))
    {
        return DRV_ERR_PARA;
    }

    ret = rtl8367b_getAsicSvlanC2SConf(uiIndex, puiCvid, puiPPortMsk, &uiSvidIndex);
    if (RT_ERR_OK != ret)
    {
        return DRV_SDK_GEN_ERROR;
    }

    memset(&stSvlanEntry, 0, sizeof(stSvlanEntry));
    ret = rtl8367b_getAsicSvlanMemberConfiguration(uiSvidIndex, &stSvlanEntry);
    if (RT_ERR_OK != ret)
    {
        return DRV_SDK_GEN_ERROR;
    }

    *puiSvid = stSvlanEntry.vs_svid;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GetSp2cEntryByIndex
 Date Created: 2012/9/18 
       Author: x06333
  Description: Get sp2c entry by index.
        Input: UINT32 uiIndex       
                UINT32 *puiCvid      
                UINT32 *puiSvid      
                UINT32 *puiDstPPort  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetSp2cEntryByIndex(UINT32 uiIndex, UINT32 *puiCvid, UINT32 *puiSvid, UINT32 *puiDstPPort)
{
    ret_t ret;
    rtl8367b_svlan_s2c_t stSp2cEntry;
    rtl8367b_svlan_memconf_t stSvlanEntry;

    if ((RTL8367B_SP2CMAX < uiIndex) ||
        (NULL == puiCvid) ||
        (NULL == puiSvid) ||
        (NULL == puiDstPPort))
    {
        return DRV_ERR_PARA;
    }

    *puiCvid = 0;
    *puiSvid = 0;
    *puiDstPPort = 0xFF;
    
    ret = rtl8367b_getAsicSvlanSP2CConf(uiIndex, &stSp2cEntry);
    if (RT_ERR_OK != ret)
    {
        return DRV_SDK_GEN_ERROR;
    }

    if (0 == stSp2cEntry.valid)
    {
        return DRV_OK;
    }

    memset(&stSvlanEntry, 0, sizeof(stSvlanEntry));
    ret = rtl8367b_getAsicSvlanMemberConfiguration(stSp2cEntry.svidx, &stSvlanEntry);
    if (RT_ERR_OK != ret)
    {
        return DRV_SDK_GEN_ERROR;
    }
    
    *puiCvid = stSp2cEntry.vid;
    *puiSvid = stSvlanEntry.vs_svid;
    *puiDstPPort = stSp2cEntry.dstport;

    return DRV_OK;
}

#ifdef  __cplusplus
}
#endif

