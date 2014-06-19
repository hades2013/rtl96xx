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
//#include <rtk/port.h>
//#include <rtk/switch.h>
//#include <rtk/qos.h>
//#include <rtk/flowctrl.h>
#include "hal_common.h"
//#include <common/rt_error.h>
//#include "dal/esw/dal_esw_led.h"
//#include <osal/time.h>
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include "rtk_api.h"
#include "rtk_api_ext.h"
#include "rtl8367b_asicdrv_led.h"
#include "rtl8367b_asicdrv_phy.h"



//#include <common/debug/rt_log.h>
/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
 
typedef unsigned int   uint32;
typedef int             int32;
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
BOOL ComboPortCfgChange(uint32 unit, uint32 phyPort, rtk_port_media_t enMedia);
#endif
/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

uint32 spdlx_save[LOGIC_PORT_NO+1];
logic_pmask_t isolate_save;

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
COMBO_PORT_CONFIG_S g_stComboPortCfg[LOGIC_PORT_NO+1][PORT_TYPE_ALL];
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)

/*lint -save -e86 -e133 -e43 -e651 -e155 -e69 -e110 -e24 -e40 -e63 -e35 -e785 -e10 -esym(552,if_lock) -e830 -e745 -e533*/
static spinlock_t _spin_hal_port_lock = SPIN_LOCK_UNLOCKED;
/*lint -restore*/
#endif

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 宏定义                                       *
*----------------------------------------------*/
#define RTK_MAX_PACKET_LEN      16000
#define RTK_NORMAL_PACKET_LEN   1522
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)

#define SPIN_HAL_PORT_LOCK       spin_lock(&_spin_hal_port_lock)
#define SPIN_HAL_PORT_UNLOCK     spin_unlock(&_spin_hal_port_lock)

//#define RTK_PORT_DEBUG_SUPPORT
#ifdef RTK_PORT_DEBUG_SUPPORT
#define RTK_PORT_DEBUG rt_log_printf
#else
#define RTK_PORT_DEBUG(fmt,arg...)
#endif
#endif

/*****************************************************************************
    Func Name: Hal_GetPortLinkupType
  Description: 获取当前端口linkup的类型
        Input: phyid_t phyid  
                UINT32 *pulLinkupType      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
*****************************************************************************/
DRV_RET_E Hal_GetPortLinkupType(phyid_t phyid, UINT32 *pulLinkupType)
{
    int32 iRv = RT_ERR_OK;  
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    rtk_port_linkStatus_t enStatus;    
    rtk_port_media_t enMedia;
    #endif
    #ifdef CHIPSET_RTL8305
    rtk_port_mac_ability_t enStatus;
    #endif
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 uiChipId = 0;    
    UINT32 uiPortId = 0;
    #endif
    
    if(NULL == pulLinkupType)
    {
        return DRV_ERR_PARA;
    }    
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    uiChipId = CHIPID(phyid);
    uiPortId = PORTID(phyid);
    iRv=rtk_port_link_get(uiChipId, uiPortId, &enStatus);   
    if((RT_ERR_OK != iRv) || (PORT_LINKUP!=enStatus))
    {
        *pulLinkupType = HAL_PORT_LINKUP_TYPE_DOWN;
        return DRV_OK;
    }
    
    iRv=rtk_port_phyComboPortMedia_get(uiChipId, uiPortId, &enMedia);
    if(RT_ERR_OK == iRv)
    {
        switch(enMedia)
        {
            case PORT_MEDIA_COPPER_AUTO:
            case PORT_MEDIA_COPPER:
            {
                *pulLinkupType = HAL_PORT_LINKUP_TYPE_COPPER_UP;
                break;
            }
            case PORT_MEDIA_FIBER_AUTO:
            case PORT_MEDIA_FIBER:
            {                
                *pulLinkupType = HAL_PORT_LINKUP_TYPE_FIBER_UP; 
                break;
            }  
            default:
                *pulLinkupType = HAL_PORT_LINKUP_TYPE_DOWN;
                break; 
        }
    }
    else
    {
        *pulLinkupType = HAL_PORT_LINKUP_TYPE_DOWN;
    }
    #endif
    #ifdef CHIPSET_RTL8305   
    iRv=rtk_port_macStatus_get(PORTID(phyid),&enStatus);
   
    if((RT_ERR_OK != iRv) || (PORT_LINKUP!=enStatus.link))
    {
        *pulLinkupType = HAL_PORT_LINKUP_TYPE_DOWN;
        return DRV_OK;
    }
    *pulLinkupType = HAL_PORT_LINKUP_TYPE_COPPER_UP;
    #endif
    
    return DRV_OK;
}
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)

/*****************************************************************************
    Func Name: rtk_ComboPortSkipSet
  Description: 检查combo设置是否需要设置寄存器
        Input: phyid_t phyid  
               UINT32 uiPortType   PORT_TYPE_IS_COPPER/PORT_TYPE_IS_FIBER    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static bool rtk_ComboPortSkipSet(phyid_t phyid, UINT32 uiPortType)
{ 
    
    int32 iRv = RT_ERR_OK;
    UINT32 ulMediumType = 0;
    rtk_port_media_t enMedia;    
    iRv=rtk_port_phyComboPortMedia_get(CHIPID(phyid), PORTID(phyid), &enMedia);    
    
    if(RT_ERR_OK == iRv)
    {
        switch(enMedia)
        {           
            case PORT_MEDIA_COPPER_AUTO:           
            case PORT_MEDIA_COPPER:
            {
                ulMediumType=PORT_TYPE_NOT_FIBER;
                break;
            }            
            case PORT_MEDIA_FIBER_AUTO:            
            case PORT_MEDIA_FIBER:
            {                
                ulMediumType=PORT_TYPE_FIBER_1000; 
                break;
            }  
            default:
                return false; 
        }

        /*设置fiber时，如果为copper则不设置寄存器。　设置copper时，如果为fiber则不设置寄存器*/
        if(((PORT_TYPE_IS_FIBER == uiPortType) && (PORT_TYPE_NOT_FIBER == ulMediumType))
          || ((PORT_TYPE_IS_COPPER == uiPortType) && (PORT_TYPE_NOT_FIBER != ulMediumType)))
        {
            return true;
        }
    }   
    
    return false;
}
#endif
/*****************************************************************************
    Func Name: Hal_SetPortPriority
  Description: 设置端口优先级,范围1-7
        Input: port_num_t lgcPort  
                UINT32 uiPrio       
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortPriority(port_num_t lgcPort, UINT32 uiPrio)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if (!ISVALID_PORT_PRIORITY(uiPrio))
    {
        return DRV_INVALID_PRIORITY_VALUE;
    }

    phyid = PortLogic2PhyID(lgcPort);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_qos_portInnerPri_set(CHIPID(phyid), PORTID(phyid), uiPrio);
    #endif
    #ifdef CHIPSET_RTL8305
    iRv = rtk_qos_portPri_set(PORTID(phyid), uiPrio);
    #endif
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_SetPortMdix
  Description: 设置端口的MDI
        Input: port_num_t lgcPort  
                UINT32 uiMdix       
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortMdix(port_num_t lgcPort, UINT32 uiMdix)
{
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    phyid_t phyid = 0;    
    rtk_port_crossOver_mode_t rtkMdix = PORT_CROSSOVER_MODE_AUTO;
    #endif
    int32 iRv = RT_ERR_OK;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    (void)uiMdix;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    switch (uiMdix)
    {
        case PORT_MDI_AUTO:
            rtkMdix = PORT_CROSSOVER_MODE_AUTO;
            break;
        case PORT_MDI_ACROSS:
            rtkMdix = PORT_CROSSOVER_MODE_MDIX;
            break;
        case PORT_MDI_NORMAL:
            rtkMdix = PORT_CROSSOVER_MODE_MDI;
            break;
        default:
            return DRV_INVALID_MDI_VALUE;
    }

    phyid = PortLogic2PhyID(lgcPort);
    
    iRv = rtk_port_phyCrossOverMode_set(CHIPID(phyid), PORTID(phyid), rtkMdix);
    #endif    
    return ErrorSdk2Drv(iRv);

}

/*****************************************************************************
    Func Name: Hal_SetPortEnable
  Description: 设置端口的admin,
        Input: port_num_t lgcPort  
                UINT32 uiEnable     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortEnable(port_num_t lgcPort, UINT32 uiEnable)
{
    phyid_t phyid = 0;
    rtk_enable_t enable = DISABLED;
    int32 iRv = RT_ERR_OK;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    
    if(!ISVALID_ADMIN_VALUE(uiEnable))
    {
        return DRV_INVALID_ADMIN_VALUE;
    }

    switch (uiEnable)
    {
        case PORT_ADMIN_DISABLE:
            enable = DISABLED;
            break;
        case PORT_ADMIN_ENABLE:
            enable = ENABLED;
            break;
        default:
            return DRV_INVALID_ADMIN_VALUE;
    }

    phyid = PortLogic2PhyID(lgcPort);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_port_adminEnable_set(CHIPID(phyid), PORTID(phyid), enable);
    #endif
    #ifdef CHIPSET_RTL8305
    iRv = rtk_port_adminEnable_set(PORTID(phyid), enable);
    #endif
    return ErrorSdk2Drv(iRv);

}
/*****************************************************************************
    Func Name: Hal_GetPortEnable
  Description: 获取端口的admin
        Author:liaohongjun
        Input: port_num_t lgcPort  
                UINT32 *uiEnable     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
   added by liaohongjun 2012/12/10 of EPN104QID0086                                                                         
*****************************************************************************/
DRV_RET_E Hal_GetPortEnable(port_num_t lgcPort, UINT32 *uiEnable)
{
    phyid_t phyid = 0;
    rtk_enable_t enable = DISABLED;
    int32 iRv = RT_ERR_OK;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
   
   if(NULL == uiEnable)
   {
       return DRV_ERR_PARA;
   }


    phyid = PortLogic2PhyID(lgcPort);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_port_adminEnable_get(CHIPID(phyid), PORTID(phyid), &enable);
    #endif
    #ifdef CHIPSET_RTL8305
    iRv = rtk_port_adminEnable_get(PORTID(phyid), &enable);
    #endif
    *uiEnable = enable;
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_SetPortJumbo
  Description: 设置端口通过的最大包长
        Input: port_num_t lgcPort  
                UINT32 uiJumbo      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortJumbo(port_num_t lgcPort, UINT32 uiJumbo)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
    uint32 uiLen = 0;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    
    if(!ISVALID_JUMBO_VALUE(uiJumbo))
    {
        return DRV_INVALID_JUMBO_VALUE;
    }
   //w03828
    uiLen = (uiJumbo == PORT_JUMBO_ENABLE)? RTK_MAX_PACKET_LEN : RTK_NORMAL_PACKET_LEN;

    phyid = PortLogic2PhyID(lgcPort);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_switch_portMaxPktLen_set(CHIPID(phyid), PORTID(phyid), uiLen);
    #endif
    #ifdef CHIPSET_RTL8305
    iRv = rtk_switch_portMaxPktLen_set(PORTID(phyid), uiLen);
    #endif
    return ErrorSdk2Drv(iRv);
}
/*****************************************************************************
    Func Name: Hal_SetCPUPortJumbo
  Description: 
        Input:                 UINT32 uiLen      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetCPUPortJumbo(UINT32 uiLen)
{ 
    int32 iRv = RT_ERR_OK;
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_switch_portMaxPktLen_set(CHIPID(PORT_PHYID(LOGIC_CPU_PORT)), PORTID(PORT_PHYID(LOGIC_CPU_PORT)), uiLen);
#endif
#ifdef CHIPSET_RTL8305
    iRv = rtk_switch_portMaxPktLen_set(PORTID(PORT_PHYID(LOGIC_CPU_PORT)), uiLen);
#endif
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_SetPortTxRx
  Description: 设置打开/关闭端口的RX/TX
        Input: port_num_t lgcPort  
                UINT32 uiState      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortTxRx(port_num_t lgcPort, UINT32 uiState)
{
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    phyid_t phyid = 0;
    UINT32 uiChipId = 0;
    UINT32 uiPortId = 0;
    rtk_enable_t enable = DISABLED;
    #endif
    int32 iRv = RT_ERR_OK;    
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    #ifdef CHIPSET_RTL8305
       (void)uiState;
    #endif
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    switch (uiState)
    {
        case TRUE:
            enable = ENABLED;
            break;
        case FALSE:
            enable = DISABLED;
            break;
        default:
            return DRV_INVALID_STATE;
    }

    phyid = PortLogic2PhyID(lgcPort);
    uiChipId = CHIPID(phyid);
    uiPortId = PORTID(phyid);
   // rtk_port_macForceLinkExt_set
    iRv = rtk_port_rxEnable_set(uiChipId, uiPortId, enable);
    if(RT_ERR_OK != iRv)
    {
        return ErrorSdk2Drv(iRv);
    }
    iRv = rtk_port_txEnable_set(uiChipId, uiPortId, enable);
    #endif
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_SetPortAbility
  Description: 设置协商时端口的能力位
        Input: port_num_t lgcPort  
                UINT32 uiPortType   PORT_TYPE_IS_COPPER/PORT_TYPE_IS_FIBER
                UINT32 uiAbility    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortAbility(port_num_t lgcPort, UINT32 uiAbility, UINT32 uiPortType)
{
    phyid_t phyid = 0;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 uiChipId = 0;
    #endif
    UINT32 uiPortId = 0;
    INT32 iRv = RT_ERR_OK;    
    rtk_port_phy_ability_t rtkAbility;    
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    PortType_E uiFiberSpeed = PORT_TYPE_FIBER_UNKNOWN;
    #endif
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }   
    if(uiPortType > PORT_TYPE_IS_FIBER)
    {
        return DRV_ERR_PARA;
    }
   
    memset(&rtkAbility, 0x00, sizeof(rtk_port_phy_ability_t));  
   
    phyid = PortLogic2PhyID(lgcPort);    
    uiPortId = PORTID(phyid);   
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    uiChipId = CHIPID(phyid);
    if((IS_COMBO_PORT(lgcPort)) && (true == rtk_ComboPortSkipSet(phyid, uiPortType)))
    {
        RTK_PORT_DEBUG("Hal_SetPortAbility skip set, port=%d, uiPortType=%d, uiAbility=%d\n",lgcPort,uiPortType, uiAbility);
        SPIN_HAL_PORT_LOCK;
        (g_stComboPortCfg[lgcPort][uiPortType]).autoneg_advert = uiAbility;
        SPIN_HAL_PORT_UNLOCK;
        
        return DRV_OK;
    }
    #endif
    if(uiAbility & PORT_ABIL_10MB_HD){
        rtkAbility.Half_10 = 1;
    }
    else{
        rtkAbility.Half_10 = 0;
    }
    
    if(uiAbility & PORT_ABIL_10MB_FD){
        rtkAbility.Full_10 = 1;
    }
    else{
        rtkAbility.Full_10 = 0; 
    }
    
    if(uiAbility & PORT_ABIL_100MB_HD){
        rtkAbility.Half_100 = 1;
    }
    else{
        rtkAbility.Half_100 = 0; 
    }
    
    if(uiAbility & PORT_ABIL_100MB_FD){
        rtkAbility.Full_100 = 1; 
    }
    else{
        rtkAbility.Full_100 = 0;  
    }
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    if(uiAbility & PORT_ABIL_1000MB_HD){
        rtkAbility.Half_1000 = 1;
    }
    else{
        rtkAbility.Half_1000 = 0;
    }
    #endif
    if(uiAbility & PORT_ABIL_1000MB_FD){
        rtkAbility.Full_1000 = 1;
    }
    else{
        rtkAbility.Full_1000 = 0;
    }
#if 0 /*2.0.5 rtk sdk not support*/    
    if(uiAbility & PORT_ABIL_2500MB_HD)
    if(uiAbility & PORT_ABIL_2500MB_FD)
    if(uiAbility & PORT_ABIL_10GB_HD)
    if(uiAbility & PORT_ABIL_10GB_FD)
#endif /* #if 0 */

    /*PHY Reg 4 bit 10*/
    if(uiAbility & PORT_ABIL_PAUSE_TX){
        rtkAbility.FC = 1;
    }
    else{
        rtkAbility.FC = 0;
    }

    /*PHY Reg 4 bit 11*/
    if(uiAbility & PORT_ABIL_PAUSE_RX){
        rtkAbility.AsyFC = 1;
    }
    else{
        rtkAbility.AsyFC = 0; 
    }
#if 0 /*2.0.5 rtk sdk not support*/    
    if(uiAbility & PORT_ABIL_PAUSE_ASYMM)
    if(uiAbility & PORT_ABIL_TBI)
    if(uiAbility & PORT_ABIL_MII)
    if(uiAbility & PORT_ABIL_GMII)
    if(uiAbility & PORT_ABIL_SGMII)
    if(uiAbility & PORT_ABIL_XGMII)
    if(uiAbility & PORT_ABIL_LB_MAC)
    if(uiAbility & PORT_ABIL_LB_MAC_RMT)
    if(uiAbility & PORT_ABIL_LB_PHY)
    if(uiAbility & PORT_ABIL_AN)
    if(uiAbility & PORT_ABIL_3000MB_HD)
    if(uiAbility & PORT_ABIL_3000MB_FD)
    if(uiAbility & PORT_ABIL_12GB_HD)
    if(uiAbility & PORT_ABIL_12GB_FD)
    if(uiAbility & PORT_ABIL_13GB_HD)
    if(uiAbility & PORT_ABIL_13GB_FD)
#endif /* #if 0 */
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_port_phyAutoNegoAbility_set(uiChipId, uiPortId, &rtkAbility);
    #endif
    #ifdef CHIPSET_RTL8305
    iRv = rtk_port_phyForceModeAbility_set(uiPortId, &rtkAbility);
    #endif
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    if((IS_COMBO_PORT(lgcPort)) && (RT_ERR_OK == iRv))
    {
        RTK_PORT_DEBUG("Hal_SetPortAbility , port=%d, uiPortType=%d, uiAbility=%d\n",lgcPort,uiPortType, uiAbility);
        SPIN_HAL_PORT_LOCK;
        (g_stComboPortCfg[lgcPort][uiPortType]).autoneg_advert = uiAbility;
        SPIN_HAL_PORT_UNLOCK;
        if(PORT_TYPE_IS_FIBER == uiPortType)
        {
            (void)Hal_GetFiberSpeed(lgcPort, (UINT32 *)&uiFiberSpeed);
            RTK_PORT_DEBUG("Hal_GetFiberSpeed, port=%d, uiFiberSpeed=%d\n",lgcPort,uiFiberSpeed);
            if(PORT_TYPE_FIBER_100 == uiFiberSpeed)
            {
                /*先将能力位修改了，再设置fiber速率*/
                (void)rtk_port_phyComboPortFiberMedia_set( uiChipId, uiPortId, PORT_FIBER_MEDIA_100);
                return DRV_OK;
            }
            else
            {
                (void)rtk_port_phyComboPortFiberMedia_set( uiChipId, uiPortId, PORT_FIBER_MEDIA_1000);
                return DRV_OK;
            }
        }
    }   
    #endif
    
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_SetPortAutoneg
  Description: 设置开启/关闭端口的自协商
        Input: port_num_t lgcPort  
                UINT32 uiPortType   PORT_TYPE_IS_COPPER/PORT_TYPE_IS_FIBER
                UINT32 uiAuton      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortAutoneg(port_num_t lgcPort, UINT32 uiAuton, UINT32 uiPortType)
{
    phyid_t phyid = 0;
    INT32 iRv = RT_ERR_OK;
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    rtk_enable_t enabled = ENABLED;
    PortType_E uiFiberSpeed = PORT_TYPE_FIBER_UNKNOWN;
#endif
    
    #ifdef CHIPSET_RTL8305
    rtk_port_phy_ability_t ability;
    #endif
    #ifdef CHIPSET_RTL8305
    (void)uiAuton;
    #endif
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    if(uiPortType > PORT_TYPE_IS_FIBER)
    {
        return DRV_ERR_PARA;
    }
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    switch (uiAuton)
    {
        case TRUE:
            enabled = ENABLED;
            break;
        case FALSE:
            enabled = DISABLED;
            break;
        default:
            return DRV_INVALID_STATE;
    }
    #endif
    
    phyid = PortLogic2PhyID(lgcPort);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    if((IS_COMBO_PORT(lgcPort)) && (true == rtk_ComboPortSkipSet(phyid, uiPortType)))
    {
        SPIN_HAL_PORT_LOCK;
        (g_stComboPortCfg[lgcPort][uiPortType]).autoneg_enable = uiAuton;
        SPIN_HAL_PORT_UNLOCK;
        return DRV_OK;
    }
    #endif
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_port_phyAutoNegoEnable_set(CHIPID(phyid), PORTID(phyid), enabled);
    #endif
    #ifdef CHIPSET_RTL8305
    iRv = rtk_port_phyAutoNegoAbility_get(PORTID(phyid),&ability);
    if(RT_ERR_OK != iRv)
    {
        return ErrorSdk2Drv(iRv);
    }
    if(ability.AutoNegotiation==1)
    {
        return DRV_OK;
    }
    ability.AutoNegotiation=1;
    iRv = rtk_port_phyAutoNegoAbility_set(PORTID(phyid),&ability);
    #endif
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    if((IS_COMBO_PORT(lgcPort)) && (RT_ERR_OK == iRv))
    {
        SPIN_HAL_PORT_LOCK;
        (g_stComboPortCfg[lgcPort][uiPortType]).autoneg_enable = uiAuton;
        SPIN_HAL_PORT_UNLOCK;
        if(PORT_TYPE_IS_FIBER == uiPortType)
        {
            (void)Hal_GetFiberSpeed(lgcPort, (UINT32 *)&uiFiberSpeed);
            RTK_PORT_DEBUG("Hal_GetFiberSpeed, port=%d, uiFiberSpeed=%d\n",lgcPort,uiFiberSpeed);
            if(PORT_TYPE_FIBER_100 == uiFiberSpeed)
            {
                /*先将能力位修改了，再设置fiber速率*/
                (void)rtk_port_phyComboPortFiberMedia_set(CHIPID(phyid), PORTID(phyid), PORT_FIBER_MEDIA_100);
                return DRV_OK;
            }
            else
            {
                (void)rtk_port_phyComboPortFiberMedia_set(CHIPID(phyid), PORTID(phyid), PORT_FIBER_MEDIA_1000);
                return DRV_OK;
            }
        }
    }
    #endif
    
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_SetPortSpeed
  Description: 设置端口的速率
        Input: port_num_t lgcPort  
                UINT32 uiPortType   PORT_TYPE_IS_COPPER/PORT_TYPE_IS_FIBER
                UINT32 uiSpeed      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortSpeed(port_num_t lgcPort, UINT32 uiSpeed, UINT32 uiPortType)
{
    phyid_t phyid = 0;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 uiChipId = 0;
    #endif
    UINT32 uiPortId = 0;
    INT32 iRv = RT_ERR_OK;
    
    rtk_port_speed_t rtkSpeed = PORT_SPEED_10M;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    rtk_port_speed_t rtkTmpSpeed = PORT_SPEED_10M;
    rtk_enable_t rtkFlowctrl = DISABLED;
    rtk_port_duplex_t rtkDuplex = PORT_HALF_DUPLEX;
    
    PortType_E uiFiberSpeed = PORT_TYPE_FIBER_UNKNOWN;
    #endif

    #ifdef CHIPSET_RTL8305
    rtk_port_phy_ability_t Ability;
    #endif
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    if(uiPortType > PORT_TYPE_IS_FIBER)
    {
        return DRV_ERR_PARA;
    }
    
    switch (uiSpeed)
    {
        case PORT_SPEED_10:
            rtkSpeed = PORT_SPEED_10M;
            break;
        case PORT_SPEED_100:
            rtkSpeed = PORT_SPEED_100M;
            break;
        case PORT_SPEED_1000:
            rtkSpeed = PORT_SPEED_1000M;
            break;
        default:
            return DRV_INVALID_SPEED_VALUE;
    }
    
    phyid = PortLogic2PhyID(lgcPort);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    uiChipId = CHIPID(phyid);
    #endif
    uiPortId = PORTID(phyid);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    if((IS_COMBO_PORT(lgcPort)) && (true == rtk_ComboPortSkipSet(phyid, uiPortType)))
    {
        SPIN_HAL_PORT_LOCK;
        (g_stComboPortCfg[lgcPort][uiPortType]).force_speed = (PortSpeedValue_E)uiSpeed;
        SPIN_HAL_PORT_UNLOCK;
        return DRV_OK;
    }
    #endif
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_port_phyForceModeAbility_get(uiChipId, uiPortId, &rtkTmpSpeed, &rtkDuplex, &rtkFlowctrl);
    #endif
    #ifdef CHIPSET_RTL8305
    memset(&Ability,0,sizeof(rtk_port_phy_ability_t));
    iRv = rtk_port_phyForceModeAbility_get(uiPortId, &Ability);
    if(Ability.Full_10==1 || Ability.Full_100==1 || Ability.Full_1000==1)
    {
        Ability.Full_10=0;
        Ability.Full_100=0;
        Ability.Full_1000=0;
        switch(rtkSpeed)
        {
            case PORT_SPEED_10M:
                Ability.Full_10=1;
                break;
            case PORT_SPEED_100M:
                Ability.Full_100=1;
                break;
            case PORT_SPEED_1000M:
                Ability.Full_1000=1;
                break;
            default:
            return DRV_INVALID_SPEED_VALUE;
        }
    }
    if(Ability.Half_10==1 || Ability.Half_100==1)
    {
        Ability.Half_10=0;
        Ability.Half_100=0;       
        switch(rtkSpeed)
        {
            case PORT_SPEED_10M:
                Ability.Half_10=1;
                break;
            case PORT_SPEED_100M:
                Ability.Half_100=1;
                break;           
            default:
            return DRV_INVALID_SPEED_VALUE;
        }
    }
    #endif
    if(RT_ERR_OK != iRv)
    {
        return ErrorSdk2Drv(iRv);
    }
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_port_phyForceModeAbility_set(uiChipId, uiPortId, rtkSpeed, rtkDuplex, rtkFlowctrl);
    #endif
    
    #ifdef CHIPSET_RTL8305    
    iRv = rtk_port_phyForceModeAbility_set(uiPortId, &Ability);
    #endif
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    if((IS_COMBO_PORT(lgcPort)) && (RT_ERR_OK == iRv))
    {
        SPIN_HAL_PORT_LOCK;
        (g_stComboPortCfg[lgcPort][uiPortType]).force_speed = (PortSpeedValue_E)uiSpeed;
        SPIN_HAL_PORT_UNLOCK;
        if(PORT_TYPE_IS_FIBER == uiPortType)
        {
            (void)Hal_GetFiberSpeed(lgcPort, (UINT32 *)&uiFiberSpeed);
            RTK_PORT_DEBUG("Hal_GetFiberSpeed, port=%d, uiFiberSpeed=%d\n",lgcPort,uiFiberSpeed);
            if(PORT_TYPE_FIBER_100 == uiFiberSpeed)
            {
                /*先将能力位修改了，再设置fiber速率*/
                (void)rtk_port_phyComboPortFiberMedia_set( uiChipId, uiPortId, PORT_FIBER_MEDIA_100);
                return DRV_OK;
            }
            else
            {
                (void)rtk_port_phyComboPortFiberMedia_set( uiChipId, uiPortId, PORT_FIBER_MEDIA_1000);
                return DRV_OK;
            }
        }
    }
    #endif
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
 函 数 名  : Hal_SetPortSpdlx
 功能描述  : 设置端口的速率双工
 输入参数  : port_num_t lgcPort  
             UINT32 uiDuplex     
             UINT32 uiPortType   
 输出参数  : 无
 返 回 值  : 

 Author: 
*****************************************************************************/
DRV_RET_E Hal_SetPortSpdlx(port_num_t lgcPort, UINT32 uiSpdlx)
{
    phyid_t phyid = 0;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 uiChipId = 0;
    #endif
    UINT32 uiPortId = 0;
    INT32 iRv = RT_ERR_OK;
    rtk_port_phy_ability_t Ability;
    int powerdown = 0;
    uint32_t val32=0;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    spdlx_save[lgcPort] = uiSpdlx; /*for Hal_GetSpdlxSet*/

    phyid = PortLogic2PhyID(lgcPort);
    uiPortId = PORTID(phyid);

    rtk_port_phyReg_get(uiPortId, (rtk_port_phy_reg_t)PHY_CONTROL_REG, &val32);

    if(val32 & (0x01 << 11))
    {
        powerdown = 1;
    }

    /*set auto type*/
    if((PORT_10AUTO == uiSpdlx) || (PORT_100AUTO == uiSpdlx) || (PORT_1000FULL == uiSpdlx) \
       || (PORT_1000AUTO == uiSpdlx) || (PORT_AUTOHALF == uiSpdlx) || (PORT_AUTOFULL == uiSpdlx) \
       || (PORT_AUTOAUTO == uiSpdlx)){
       
        memset(&Ability,0,sizeof(rtk_port_phy_ability_t));
        rtk_port_phyAutoNegoAbility_get(uiPortId, &Ability); 
        Ability.AutoNegotiation = (uint32)ENABLED;
        Ability.Full_100 = (uint32)DISABLED;
        Ability.Half_100 = (uint32)DISABLED;
        Ability.Full_10 = (uint32)DISABLED;
        Ability.Half_10 = (uint32)DISABLED;
        Ability.Full_100 = (uint32)DISABLED;
        Ability.Full_1000 = (uint32)DISABLED;

        switch (uiSpdlx)
        {
            case PORT_10AUTO :
                Ability.Half_10 = (uint32)ENABLED;
                Ability.Full_10 = (uint32)ENABLED;
                break;
            case PORT_100AUTO :
                Ability.Half_100 = (uint32)ENABLED;
                Ability.Full_100 = (uint32)ENABLED;
                break;
            case PORT_1000FULL :    
            case PORT_1000AUTO :
                return DRV_ERR_PARA; /*not support*/
            case PORT_AUTOHALF :
                Ability.Half_10 = (uint32)ENABLED;
                Ability.Half_100 = (uint32)ENABLED;
                break;
            case PORT_AUTOFULL :
                Ability.Full_10 = (uint32)ENABLED;
                Ability.Full_100 = (uint32)ENABLED;            
                break;
            case PORT_AUTOAUTO:
                Ability.Full_100 = (uint32)ENABLED;            
                Ability.Half_100 = (uint32)ENABLED;
                Ability.Full_10 = (uint32)ENABLED;
                Ability.Half_10 = (uint32)ENABLED;
                break;
            default:
                break;
        }

        iRv = rtk_port_phyAutoNegoAbility_set(uiPortId,&Ability);
        return ErrorSdk2Drv(iRv); 
    }
    
    /*set force type*/
    memset(&Ability,0,sizeof(rtk_port_phy_ability_t));
    iRv = rtk_port_phyForceModeAbility_get(uiPortId, &Ability);            
    Ability.Half_10 = (uint32)DISABLED;
    Ability.Full_10 = (uint32)DISABLED;
    Ability.Half_100 = (uint32)DISABLED;
    Ability.Full_100 = (uint32)DISABLED;

    switch (uiSpdlx)
    {
        case PORT_10HALF :
            Ability.Half_10 = (uint32)ENABLED;
            break;
        case PORT_10FULL :
            Ability.Full_10 = (uint32)ENABLED;
            break;
        case PORT_100HALF :
            Ability.Half_100 = (uint32)ENABLED;
            break;
        case PORT_100FULL :
            Ability.Full_100 = (uint32)ENABLED;
            break;
        default:
            return DRV_ERR_PARA;
    }

    iRv = rtk_port_phyForceModeAbility_set(uiPortId, &Ability);

    if(powerdown)
    {
        rtk_port_adminEnable_set(uiPortId, DISABLED);
    }
    else{
        /*lint --e{155,62}  z03340 add*/
        /*mdelay(500);*/
        rtk_port_adminEnable_set(uiPortId, DISABLED);
        mdelay(100);
        rtk_port_adminEnable_set(uiPortId, ENABLED);
    }

    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
 函 数 名  : Hal_SetPhyReg
 功能描述  : 设置phy 寄存器
 输入参数  : port_num_t lgcPort  
             UINT32 reg_no       
             UINT32 reg_val      
 输出参数  : 无
 返 回 值  : 
 
 *****************************************************************************/
DRV_RET_E Hal_SetPhyReg(port_num_t lgcPort, UINT32 reg_no, UINT32 reg_val)
{
    phyid_t phyid = 0;
    UINT32 uiPortId = 0;
    INT32 iRv = RT_ERR_OK;

    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    phyid = PortLogic2PhyID(lgcPort);
    uiPortId = PORTID(phyid);

    iRv = rtk_port_phyReg_set(uiPortId, (rtk_port_phy_reg_t)reg_no, reg_val);


    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
 函 数 名  : Hal_GetPhyReg
 功能描述  : 获取phy 寄存器
 输入参数  : port_num_t lgcPort  
             UINT32* pRegVal     
             UINT32 reg_no       
 输出参数  : 无
 返 回 值  : 

 *****************************************************************************/
DRV_RET_E Hal_GetPhyReg(port_num_t lgcPort, UINT32* pRegVal, UINT32 reg_no)
{
    phyid_t phyid = 0;
    UINT32 uiPortId = 0;
    INT32 iRv = RT_ERR_OK;
    UINT32 reg_val=0;

    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    phyid = PortLogic2PhyID(lgcPort);
    uiPortId = PORTID(phyid);

    iRv = rtk_port_phyReg_get(uiPortId, (rtk_port_phy_reg_t)reg_no, &reg_val);

    *pRegVal = reg_val;

    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
 函 数 名  : Hal_GetSpdlxSet
 功能描述  : 获取前期对spdlx的配置
 输入参数  : port_num_t lgcPort  
             UINT32* uiSpdlx     
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :

*****************************************************************************/
DRV_RET_E Hal_GetSpdlxSet(port_num_t lgcPort, UINT32* uiSpdlx)
{
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    
    *uiSpdlx = spdlx_save[lgcPort];

    return DRV_OK;
}

/*****************************************************************************
 函 数 名  : Hal_GetIsolateSet
 功能描述  : 获取前期对isolate的配置
 输入参数  : logic_pmask_t *pstPortMask  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :

*****************************************************************************/
DRV_RET_E Hal_GetIsolateSet(logic_pmask_t *pstPortMask)
{
    LgcMaskCopy(pstPortMask, &isolate_save);
    
    return DRV_OK;
}



/*****************************************************************************
    Func Name: Hal_SetPortDuplex
  Description: 设置端口的双工状态
        Input: port_num_t lgcPort  
                UINT32 uiPortType   PORT_TYPE_IS_COPPER/PORT_TYPE_IS_FIBER
                UINT32 uiDuplex     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortDuplex(port_num_t lgcPort, UINT32 uiDuplex, UINT32 uiPortType)
{
    phyid_t phyid = 0;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 uiChipId = 0;
    #endif
    UINT32 uiPortId = 0;
    INT32 iRv = RT_ERR_OK;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    rtk_port_speed_t rtkSpeed = PORT_SPEED_10M;
    rtk_enable_t rtkFlowctrl = DISABLED;
    rtk_port_duplex_t rtkTmpDuplex = PORT_HALF_DUPLEX;
    /*Add ,2012-1-12*/
    PortType_E uiFiberSpeed = PORT_TYPE_FIBER_UNKNOWN;
    #endif
    rtk_port_duplex_t rtkDuplex = PORT_HALF_DUPLEX;    

    #ifdef CHIPSET_RTL8305
    rtk_port_phy_ability_t Ability;
    #endif
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    if(uiPortType > PORT_TYPE_IS_FIBER)
    {
        return DRV_ERR_PARA;
    }

    switch (uiDuplex)
    {
        case PORT_DUPLEX_FULL:
            rtkDuplex = PORT_FULL_DUPLEX;
            break;
        case PORT_DUPLEX_HALF:
            rtkDuplex = PORT_HALF_DUPLEX;
            break;
        default:
            return DRV_INVALID_DUPLEX_VALUE;
    }
    
    phyid = PortLogic2PhyID(lgcPort);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    uiChipId = CHIPID(phyid);
    #endif
    uiPortId = PORTID(phyid);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    if((IS_COMBO_PORT(lgcPort)) && (true == rtk_ComboPortSkipSet(phyid, uiPortType)))
    {
/*Begin modify ,2012-1-12*/
        SPIN_HAL_PORT_LOCK;
        (g_stComboPortCfg[lgcPort][uiPortType]).force_duplex = (PortDuplexValue_E)uiDuplex;
        SPIN_HAL_PORT_UNLOCK;
/*End modify,2012-1-12*/
        return DRV_OK;
    }
    #endif
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_port_phyForceModeAbility_get(uiChipId, uiPortId, &rtkSpeed, &rtkTmpDuplex, &rtkFlowctrl);    
    #endif
    #ifdef CHIPSET_RTL8305    
     memset(&Ability,0,sizeof(rtk_port_phy_ability_t));
    iRv = rtk_port_phyForceModeAbility_get(uiPortId, &Ability); 
    if(rtkDuplex==PORT_HALF_DUPLEX)
    {   
        Ability.Full_10=0;
        Ability.Full_100=0;
        Ability.Full_1000=0;
        Ability.Half_10=1;
        Ability.Half_100=1;  
           
    }
    if(rtkDuplex==PORT_FULL_DUPLEX)
    {           
        Ability.Full_10=1;
        Ability.Full_100=1;
        Ability.Full_1000=1;
        Ability.Half_10=0;
        Ability.Half_100=0;          
    }
    #endif
    if(RT_ERR_OK != iRv)
    {
        return ErrorSdk2Drv(iRv);
    }
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_port_phyForceModeAbility_set(uiChipId, uiPortId, rtkSpeed, rtkDuplex, rtkFlowctrl);
    #endif
    #ifdef CHIPSET_RTL8305   
    iRv = rtk_port_phyForceModeAbility_set(uiPortId, &Ability);
    #endif
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    if((IS_COMBO_PORT(lgcPort)) && (RT_ERR_OK == iRv))
    {
/*Begin modify,2012-1-12*/
        SPIN_HAL_PORT_LOCK;
        (g_stComboPortCfg[lgcPort][uiPortType]).force_duplex = (PortDuplexValue_E)uiDuplex;
        SPIN_HAL_PORT_UNLOCK;
/*End modify,2012-1-12 */
/*Begin Added,2012-1-12 */
        if(PORT_TYPE_IS_FIBER == uiPortType)
        {
            (void)Hal_GetFiberSpeed(lgcPort, (UINT32 *)&uiFiberSpeed);
            RTK_PORT_DEBUG("Hal_GetFiberSpeed, port=%d, uiFiberSpeed=%d\n",lgcPort,uiFiberSpeed);
            if(PORT_TYPE_FIBER_100 == uiFiberSpeed)
            {
                /*先将能力位修改了，再设置fiber速率*/
                (void)rtk_port_phyComboPortFiberMedia_set( uiChipId, uiPortId, PORT_FIBER_MEDIA_100);
                return DRV_OK;
            }
            else
            {
                (void)rtk_port_phyComboPortFiberMedia_set( uiChipId, uiPortId, PORT_FIBER_MEDIA_1000);
                return DRV_OK;
            }
        }
/*End Added ,2012-1-12*/
    }
    #endif
    return ErrorSdk2Drv(iRv);

}

/*****************************************************************************
    Func Name: Hal_SetPortPause
  Description: 设置端口的流控
        Input: port_num_t lgcPort  
                UINT32 uiPauseTx    
                UINT32 uiPauseRx    
       Output: 
       Return: 
      Caution: 流控设置时两个方向需要设置为相同值
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortFlowctrl(port_num_t lgcPort, UINT32 uiPauseTx, UINT32 uiPauseRx)
{
    phyid_t phyid = 0;
    rtk_enable_t enable = DISABLED;
    int32 iRv = RT_ERR_OK;
    #ifdef CHIPSET_RTL8305
    rtk_port_phy_ability_t Ability;
    #endif
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    if((TRUE == uiPauseTx) && (TRUE == uiPauseRx))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    phyid = PortLogic2PhyID(lgcPort);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_port_flowCtrlEnable_set(CHIPID(phyid), PORTID(phyid), enable);
    #endif
    #ifdef CHIPSET_RTL8305    
    if ((iRv = rtk_port_phyAutoNegoAbility_get(PORTID(phyid), &Ability)) != RT_ERR_OK)
    {
      
       return ErrorSdk2Drv(iRv);
    }
    if (ENABLED == enable)
    {
       Ability.FC = 1;
       Ability.AsyFC = 1;
    }
    else
    {
       Ability.FC = 0;
       Ability.AsyFC = 0;
    }
    if(Ability.AutoNegotiation==1)
    {
        if ((iRv = rtk_port_phyAutoNegoAbility_set(PORTID(phyid), &Ability)) != RT_ERR_OK)
        {
           //RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
           return ErrorSdk2Drv(iRv);
        }
    }
    else
    {
        if ((iRv = rtk_port_phyForceModeAbility_set(PORTID(phyid), &Ability)) != RT_ERR_OK)
        {
           //RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "Error Code: 0x%X", ret);
           return ErrorSdk2Drv(iRv);
        }
    }
    #endif
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    if(RT_ERR_OK == iRv)
    #endif
    {
        #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
        iRv = rtk_port_backpressureEnable_set(CHIPID(phyid), PORTID(phyid), enable);
        #endif
        
        #ifdef CHIPSET_RTL8305
		UINT32 uiRegData = 0;
         /* MAC force flow-control */
        if (ENABLED == enable)
        {
            uiRegData = 3;
        }
        else
        {
            uiRegData = 0;
        }
    
        iRv = rtl8367b_setAsicRegBits(RTL8367B_REG_MAC0_FORCE_SELECT+PORTID(phyid), RTL8367B_MAC0_FORCE_SELECT_FC_MASK, uiRegData);
        if (RT_ERR_OK != iRv)
        {
            return ErrorSdk2Drv(iRv);
        }        
        #endif
    }
    
    return ErrorSdk2Drv(iRv);
    
}

/*****************************************************************************
    Func Name: Hal_SetPortIsolateMask
  Description: 按端口掩码设置开启/关闭端口隔离
        Input: logic_pmask_t *pstPortMask  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortIsolateMask(logic_pmask_t *pstPortMask)
{ 
    port_num_t lgcPort = 0;
    phyid_t phyid = 0;
    logic_pmask_t stTmpLogicMask;
    phy_pmask_t stPhyMask;
    phy_pmask_t stAllPhyMask;
    int32 iRv = RT_ERR_OK;
    
    if(NULL == pstPortMask)
    {
        return DRV_ERR_PARA;
    }

    ClrLgcMaskAll(&stTmpLogicMask);
    ClrPhyMaskAll(&stPhyMask);
    SetPhyMaskAll(&stAllPhyMask);

    LgcMaskCopy(&isolate_save, pstPortMask);

    /*set port mask*/
    LgcPortFor(lgcPort)
    {
        if(!TstLgcMaskBit(lgcPort, pstPortMask))
        {
            SetLgcMaskBit(lgcPort, &stTmpLogicMask);
        }
    }
    /*must add CPU port for port mask*/
    MaskLogic2Phy(&stTmpLogicMask, &stPhyMask);
    PhyMaskAddCpuPort(&stPhyMask);
    PhyMaskAddStackPort(&stPhyMask);
    
    LgcPortFor(lgcPort)
    {
        phyid = PortLogic2PhyID(lgcPort); 
        /*isolate port*/
        if(TstLgcMaskBit(lgcPort, pstPortMask))
        {
            #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
            iRv = rtk_port_isolation_set(CHIPID(phyid), PORTID(phyid),CHIPNMASK(CHIPID(phyid), &stPhyMask));
            #endif
            #ifdef CHIPSET_RTL8305
            iRv = rtk_port_isolation_set(PORTID(phyid),CHIPNMASK(CHIPID(phyid), &stPhyMask));  
            #endif
            if(iRv != RT_ERR_OK)
            {
                return ErrorSdk2Drv(iRv);
            }
        }
        else
        {
            #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
            iRv = rtk_port_isolation_set(CHIPID(phyid), PORTID(phyid),CHIPNMASK(CHIPID(phyid), &stAllPhyMask));
            #endif
            #ifdef CHIPSET_RTL8305
            iRv = rtk_port_isolation_set( PORTID(phyid),CHIPNMASK(CHIPID(phyid), &stAllPhyMask));
            #endif
            if(iRv != RT_ERR_OK)
            {
                return ErrorSdk2Drv(iRv);
            }
        }
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_SetPortMedium
        Input: port_num_t lgcPort  
                UINT32 uiMedium   
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortMedium(port_num_t lgcPort, UINT32 uiMedium)
{
    phyid_t phyid = 0;  
    int32 iRv = RT_ERR_OK;
    rtk_port_media_t enMedia;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    uint32 ulPortType = PORT_TYPE_IS_COPPER;
    #endif
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    if(!IS_COMBO_PORT(lgcPort))
    {
        return DRV_ERR_PARA;
    }
    
    if(uiMedium == PORT_TYPE_NOT_FIBER)
    {
        enMedia = PORT_MEDIA_COPPER;
        #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
        ulPortType = PORT_TYPE_IS_COPPER;
        #endif
    }
    else
    {
        enMedia = PORT_MEDIA_FIBER;
        #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
        ulPortType = PORT_TYPE_IS_FIBER;
        #endif
    }
    
    phyid = PortLogic2PhyID(lgcPort);  
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv=rtk_port_phyComboPortMedia_set(CHIPID(phyid), PORTID(phyid), enMedia);
    #endif
    #ifdef CHIPSET_RTL8305
    iRv=rtk_port_phyComboPortMedia_set(PORTID(phyid), enMedia);
    #endif
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    if(RT_ERR_OK == iRv)
    {
        (void)ComboPortCfgChange(CHIPID(phyid), phyid, enMedia);
        SPIN_HAL_PORT_LOCK;
        (g_stComboPortCfg[lgcPort][PORT_TYPE_IS_COPPER]).port_type = ulPortType;
        SPIN_HAL_PORT_UNLOCK;
    }
    #endif
    return ErrorSdk2Drv(iRv);
}
/*****************************************************************************
    Func Name: Hal_GetPortType
  Description: 获取COMBO口当前使能的媒介
        Input: port_num_t lgcPort  
       Output: 
       Return: 当前使能的媒介
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
UINT32 Hal_GetPortType(port_num_t lgcPort)
{
    UINT32 ulPortType = PORT_TYPE_IS_COPPER;
    if(!IsValidLgcPort(lgcPort))
    {
        return PORT_TYPE_IS_COPPER;
    }
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    if(!IS_COMBO_PORT(lgcPort))
    {
        return PORT_TYPE_IS_COPPER;
    }

    SPIN_HAL_PORT_LOCK;
    ulPortType = (g_stComboPortCfg[lgcPort][PORT_TYPE_IS_COPPER]).port_type;
    SPIN_HAL_PORT_UNLOCK;
    #endif
    return ulPortType;
}

/*****************************************************************************
    Func Name: Hal_GetPortMedium
  Description: 获取COMBO口当前使能的媒介
        Input: port_num_t lgcPort  
                UINT32 *puiMedium   
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortMedium(port_num_t lgcPort, UINT32 *puiMedium)
{
    phyid_t phyid = 0;  
    int32 iRv = RT_ERR_OK;
    rtk_port_media_t enMedia;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiMedium == NULL)
    {
        return DRV_ERR_PARA;
    }
    /*default value*/
    *puiMedium = PORT_TYPE_NOT_FIBER;
    
    phyid = PortLogic2PhyID(lgcPort);  
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv=rtk_port_phyComboPortMedia_get(CHIPID(phyid), PORTID(phyid), &enMedia);
    #endif
    #ifdef CHIPSET_RTL8305
    iRv=rtk_port_phyComboPortMedia_get(PORTID(phyid), &enMedia);
    #endif
    if(RT_ERR_OK == iRv)
    {
        switch(enMedia)
        {
            #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
            case PORT_MEDIA_COPPER_AUTO:
            #endif
            case PORT_MEDIA_COPPER:
            {
                *puiMedium=PORT_TYPE_NOT_FIBER;
                break;
            }
            #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
            case PORT_MEDIA_FIBER_AUTO:
            #endif
            case PORT_MEDIA_FIBER:
            {                
                *puiMedium=PORT_TYPE_FIBER_1000; 
                break;
            }  
            default:
                return DRV_SDK_GEN_ERROR; 
        }
    }
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_GetPortCurrentLink
  Description: 获取当前端口的连接状态
        Input: port_num_t lgcPort  
                UINT32 *puiLink     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortCurrentLink(port_num_t lgcPort, UINT32 *puiLink)
{
    phyid_t phyid = 0;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    rtk_port_linkStatus_t enStatus;
    #endif
    #ifdef CHIPSET_RTL8305
    rtk_port_mac_ability_t enStatus;
    #endif 
    int32 iRv = RT_ERR_OK;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiLink == NULL)
    {
        return DRV_ERR_PARA;
    }
    phyid = PortLogic2PhyID(lgcPort);  
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv=rtk_port_link_get(CHIPID(phyid), PORTID(phyid),&enStatus);
    #endif
    #ifdef CHIPSET_RTL8305
    iRv=rtk_port_macStatus_get(PORTID(phyid),&enStatus);
    #endif
    if(iRv == RT_ERR_OK)
    {
        #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
        if(PORT_LINKUP==enStatus)
        {
            *puiLink=PORT_LINK_UP;
        }
        else
        {
            *puiLink=PORT_LINK_DOWN;
        }
        #endif
        #ifdef CHIPSET_RTL8305
        if(enStatus.link==1)
        {
            *puiLink=PORT_LINK_UP;
        }
        else
        {
            *puiLink=PORT_LINK_DOWN;
        }
        #endif
    }
    return ErrorSdk2Drv(iRv);

}

/*****************************************************************************
    Func Name: Hal_GetPortLinkMask
  Description: 获取已连接端口的端口掩码
        Input: logic_pmask_t *pstPortMask  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortLinkMask(logic_pmask_t *pstPortMask)
{
    port_num_t uiPort;  
    phyid_t phyid = 0;
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    rtk_port_linkStatus_t enStatus;
#endif
#ifdef CHIPSET_RTL8305
    rtk_port_mac_ability_t enStatus;
#endif 

    int32 iRv = RT_ERR_OK;
    
    if(NULL == pstPortMask)
    {
        return DRV_ERR_PARA;
    }
    LgcPortFor(uiPort)
    {
        phyid = PortLogic2PhyID(uiPort);  
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
        iRv = rtk_port_link_get(CHIPID(phyid), PORTID(phyid),&enStatus);
#endif
#ifdef CHIPSET_RTL8305
        iRv=rtk_port_macStatus_get(PORTID(phyid),&enStatus);
#endif
        if(iRv == RT_ERR_OK)
        {
            
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
            if(PORT_LINKUP==enStatus)
            {
                SetLgcMaskBit(uiPort,pstPortMask);
            }
            else
            {
                ClrLgcMaskBit(uiPort,pstPortMask);
            }
#endif
#ifdef CHIPSET_RTL8305
            if(1==enStatus.link)
            {
                SetLgcMaskBit(uiPort,pstPortMask);
            }
            else
            {
                ClrLgcMaskBit(uiPort,pstPortMask);
            }
#endif
        }
        else
        {
            return ErrorSdk2Drv(iRv);
        }
    }
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GetPortMediaFiberMask
  Description: 获取当前media为fiber的端口mask
        Input: logic_pmask_t *pstPortMask  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortMediaFiberMask(logic_pmask_t *pstPortMask)
{
    port_num_t uiPort;  
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
    rtk_port_media_t enMedia;
    
    if(NULL == pstPortMask)
    {
        return DRV_ERR_PARA;
    }
    LgcPortFor(uiPort)
    {
        phyid = PortLogic2PhyID(uiPort); 
        #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
        iRv = rtk_port_phyComboPortMedia_get(CHIPID(phyid), PORTID(phyid), &enMedia);
        #endif
        #ifdef CHIPSET_RTL8305
        iRv=rtk_port_phyComboPortMedia_get(PORTID(phyid), &enMedia);
        #endif
        if(iRv == RT_ERR_OK)
        {
            switch(enMedia)
            {
                #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
                case PORT_MEDIA_COPPER_AUTO:
                #endif
                case PORT_MEDIA_COPPER:
                {
                    ClrLgcMaskBit(uiPort,pstPortMask);
                    break;
                }
                #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
                case PORT_MEDIA_FIBER_AUTO:
                #endif
                case PORT_MEDIA_FIBER:
                {                
                    SetLgcMaskBit(uiPort,pstPortMask); 
                    break;
                }  
                default:
                    return DRV_SDK_GEN_ERROR; 
            }
        }
        else
        {
            return ErrorSdk2Drv(iRv);
        }
    }
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GetPortAbility
  Description: 获取端口的自协商能力位
        Input: port_num_t lgcPort  
               UINT32 *puiAbility  
               UINT32 uiPortType   PORT_TYPE_IS_COPPER/PORT_TYPE_IS_FIBER    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortAbility(port_num_t lgcPort, UINT32 *puiAbility, UINT32 uiPortType)
{
    phyid_t phyid = 0;
    INT32 iRv = RT_ERR_OK;
    rtk_port_phy_ability_t rtkAbility;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiAbility == NULL)
    {
        return DRV_ERR_PARA;
    }
    if(uiPortType > PORT_TYPE_IS_FIBER)
    {
        return DRV_ERR_PARA;
    }
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    if(IS_COMBO_PORT(lgcPort))
    {
        SPIN_HAL_PORT_LOCK;
        *puiAbility = g_stComboPortCfg[lgcPort][uiPortType].autoneg_advert;
        SPIN_HAL_PORT_UNLOCK;
    }
    else
    {
    #endif
        phyid = PortLogic2PhyID(lgcPort); 
        #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
        iRv = rtk_port_phyAutoNegoAbility_get(CHIPID(phyid), PORTID(phyid), &rtkAbility);
        #endif
        #ifdef CHIPSET_RTL8305
        iRv=rtk_port_phyAutoNegoAbility_get(PORTID(phyid), &rtkAbility);
        #endif
        if(RT_ERR_OK != iRv)
        {
            return ErrorSdk2Drv(iRv);
        }

        *puiAbility = 0;

        if(1 == rtkAbility.Half_10){
            *puiAbility |= PORT_ABIL_10MB_HD;
        }
        else{
            *puiAbility &= ~((UINT32)PORT_ABIL_10MB_HD);
        }
        
        if(1 == rtkAbility.Full_10){
            *puiAbility |= PORT_ABIL_10MB_FD;
        }
        else{
            *puiAbility &= ~((UINT32)PORT_ABIL_10MB_FD);
        }
        
        if(1 == rtkAbility.Half_100){
            *puiAbility |= PORT_ABIL_100MB_HD;
        }
        else{
            *puiAbility &= ~((UINT32)PORT_ABIL_100MB_HD);
        }
        
        if(1 == rtkAbility.Full_100){
            *puiAbility |= PORT_ABIL_100MB_FD;
        }
        else{
            *puiAbility &= ~((UINT32)PORT_ABIL_100MB_FD);
        }
        #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
        if(1 == rtkAbility.Half_1000){
            *puiAbility |= PORT_ABIL_1000MB_HD;
        }
        else{
            *puiAbility &= ~((UINT32)PORT_ABIL_1000MB_HD);
        }
        #endif
        if(1 == rtkAbility.Full_1000){
            *puiAbility |= PORT_ABIL_1000MB_FD;
        }
        else{
            *puiAbility &= ~((UINT32)PORT_ABIL_1000MB_FD);
        }
        
        if(1 == rtkAbility.FC){
            *puiAbility |= PORT_ABIL_PAUSE_TX;
        }
        else{
            *puiAbility &= ~((UINT32)PORT_ABIL_PAUSE_TX);
        }
        
        if(1 == rtkAbility.AsyFC){
            *puiAbility |= PORT_ABIL_PAUSE_RX;
        }
        else{
            *puiAbility &= ~((UINT32)PORT_ABIL_PAUSE_RX);
        }
   #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    }
   #endif
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GetPortSpeed
  Description: 获取端口的速率
        Input: port_num_t lgcPort  
                UINT32 *puiSpeed    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortSpeed(port_num_t lgcPort, UINT32 *puiSpeed)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    rtk_port_speed_t enSpeed;
    rtk_port_duplex_t enDuplex;
    #endif
    #ifdef CHIPSET_RTL8305
    rtk_port_mac_ability_t enStatus;
    #endif 
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(NULL == puiSpeed)
    {
        return DRV_ERR_PARA;
    }
    phyid = PortLogic2PhyID(lgcPort);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_port_speedDuplex_get(CHIPID(phyid), PORTID(phyid), &enSpeed, &enDuplex);
    if(RT_ERR_OK == iRv)
    {
        switch(enSpeed)
        {
            case PORT_SPEED_10M:
            {
                *puiSpeed=PORT_SPEED_10;
                break;
            }
            case PORT_SPEED_100M:
            {
                *puiSpeed=PORT_SPEED_100;
                break;
            }
            case PORT_SPEED_1000M:
            {
                *puiSpeed=PORT_SPEED_1000;
                break;
            }
            default:
                return DRV_SDK_GEN_ERROR;                           
        }
    }
    #endif
    
#ifdef CHIPSET_RTL8305
    iRv=rtk_port_macStatus_get(PORTID(phyid),&enStatus);

    if(RT_ERR_OK == iRv)
    {
        switch(enStatus.speed)
        {
            case PORT_SPEED_10M:
            {
                *puiSpeed=PORT_SPEED_10;
                break;
            }
            case PORT_SPEED_100M:
            {
                *puiSpeed=PORT_SPEED_100;
                break;
            }
            case PORT_SPEED_1000M:
            {
                *puiSpeed=PORT_SPEED_1000;
                break;
            }
            default:
                return DRV_SDK_GEN_ERROR;           
        }
    }
#endif
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_GetPortDuplex
  Description: 获取端口的双工状态
        Input: port_num_t lgcPort  
                UINT32 *puiDuplex   
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortDuplex(port_num_t lgcPort, UINT32 *puiDuplex)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    rtk_port_speed_t enSpeed;
    rtk_port_duplex_t enDuplex;
    #endif
    #ifdef CHIPSET_RTL8305
    rtk_port_mac_ability_t enStatus;
    #endif 
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiDuplex == NULL)
    {
        return DRV_ERR_PARA;
    }
    phyid = PortLogic2PhyID(lgcPort);      
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_port_speedDuplex_get(CHIPID(phyid), PORTID(phyid), &enSpeed, &enDuplex);
    
    if(RT_ERR_OK == iRv)
    {
        switch(enDuplex)
        {
            case PORT_HALF_DUPLEX:
            {
                *puiDuplex=PORT_DUPLEX_HALF;
                break;
            }
            case PORT_FULL_DUPLEX:
            {
                *puiDuplex=PORT_DUPLEX_FULL;
                break;
            }
            default:
                 return DRV_SDK_GEN_ERROR;                           
        }
    }
    #endif
    #ifdef CHIPSET_RTL8305
    iRv=rtk_port_macStatus_get(PORTID(phyid),&enStatus);
    
    if(RT_ERR_OK == iRv)
    {
        switch(enStatus.duplex)
        {
            case PORT_HALF_DUPLEX:
            {
                *puiDuplex=PORT_DUPLEX_HALF;
                break;
            }
            case PORT_FULL_DUPLEX:
            {
                *puiDuplex=PORT_DUPLEX_FULL;
                break;
            }
            default:
                 return DRV_SDK_GEN_ERROR;                           
        }
    }
    #endif
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_GetPortMdix
  Description: 获取端口的MDI
        Input: port_num_t lgcPort  
                UINT32 *puiMdix     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortMdix(port_num_t lgcPort, UINT32 *puiMdix)
{
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    phyid_t phyid = 0;    
    rtk_port_crossOver_mode_t rtkMdix = PORT_CROSSOVER_MODE_AUTO;
    #endif  
    int32 iRv = RT_ERR_OK;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    
    if(puiMdix == NULL)
    {
        return DRV_ERR_PARA;
    }
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    phyid = PortLogic2PhyID(lgcPort);    
    iRv = rtk_port_phyCrossOverMode_get(CHIPID(phyid), PORTID(phyid), &rtkMdix);

    if(RT_ERR_OK == iRv)
    {
        switch (rtkMdix)
        {
            case PORT_CROSSOVER_MODE_AUTO:
                *puiMdix = PORT_MDI_AUTO;
                break;
            case PORT_CROSSOVER_MODE_MDIX:
                *puiMdix = PORT_MDI_ACROSS;
                break;
            case PORT_CROSSOVER_MODE_MDI:
                *puiMdix = PORT_MDI_NORMAL;
                break;
            default:
                return DRV_INVALID_MDI_VALUE;
        }
    }
    #endif
    #ifdef CHIPSET_RTL8305
    *puiMdix = PORT_MDI_NORMAL;
    #endif
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_GetPortPriority
  Description: 获取端口优先级
        Input: port_num_t lgcPort  
                UINT32 *puiPrio     
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortPriority(port_num_t lgcPort, UINT32 *puiPrio)
{
    phyid_t phyid = 0;
    rtk_pri_t rtkPrio = 0;
    int32 iRv = RT_ERR_OK;

    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiPrio == NULL)
    {
        return DRV_ERR_PARA;
    }

    phyid = PortLogic2PhyID(lgcPort);
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)      
    iRv = rtk_qos_portPri_get(CHIPID(phyid), PORTID(phyid), &rtkPrio);
#endif
#ifdef CHIPSET_RTL8305
    iRv = rtk_qos_portPri_get(PORTID(phyid),&rtkPrio);
#endif
    if(iRv == RT_ERR_OK)
    {
        *puiPrio = rtkPrio;
    }
    
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_GetPortFlowctrl
  Description: 获取端口的流控状态
        Input: port_num_t lgcPort   
                UINT32 *puiFlowctrl  
       Output: 
       Return: 
      Caution: 发送和接收方向设置时都是统一设置,所以这里判断时只用tx判断
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortFlowctrl(port_num_t lgcPort, UINT32 *puiFlowctrl)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L) 
    rtk_enable_t enable = DISABLED;
    #endif
    #ifdef CHIPSET_RTL8305
    rtk_port_phy_ability_t ability;
    #endif
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiFlowctrl == NULL)
    {
        return DRV_ERR_PARA;
    }

    phyid = PortLogic2PhyID(lgcPort);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L) 
    iRv = rtk_port_flowCtrlEnable_get(CHIPID(phyid), PORTID(phyid),&enable);
    if(iRv == RT_ERR_OK)
    {
        if(enable==DISABLED)
        {
            *puiFlowctrl = PORT_FLOWCTRL_DISABLE;
        }
        else if(enable==ENABLED)
        {
            *puiFlowctrl = PORT_FLOWCTRL_ENABLE;
        }
        else
        {
            iRv = DRV_SDK_GEN_ERROR;
        }            
    }
    #endif
    #ifdef CHIPSET_RTL8305
    iRv = rtk_port_phyAutoNegoAbility_get(PORTID(phyid),&ability);
    if(iRv == RT_ERR_OK)
    {
        
        if (ability.FC && ability.AsyFC)
        *puiFlowctrl = PORT_FLOWCTRL_ENABLE;
        else
        *puiFlowctrl = PORT_FLOWCTRL_DISABLE;
    }
    #endif
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_GetPortJumbo
  Description: 获取端口是否使能JUMBO功能
        Input: port_num_t lgcPort  
                UINT32 *puiJumbo    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortJumbo(port_num_t lgcPort, UINT32 *puiJumbo)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
    uint32 ulLen = 0;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiJumbo == NULL)
    {
        return DRV_ERR_PARA;
    }
    
    phyid = PortLogic2PhyID(lgcPort);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_switch_portMaxPktLen_get(CHIPID(phyid),PORTID(phyid), &ulLen);
    #endif
    #ifdef CHIPSET_RTL8305
    iRv = rtk_switch_portMaxPktLen_get(PORTID(phyid), &ulLen);
    #endif
    if(RT_ERR_OK == iRv)
    {      
        if(RTK_NORMAL_PACKET_LEN != ulLen)
        {
            *puiJumbo = PORT_JUMBO_ENABLE;
        }
        else
        {
            *puiJumbo = PORT_JUMBO_DISABLE;
        }
    }
    return ErrorSdk2Drv(iRv);
}

/*****************************************************************************
    Func Name: Hal_GetPortJumbo
  Description: 获取端口支持的最大报文长度
        Input: port_num_t lgcPort  
                UINT32 *puiLen    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetPortMaxFrameLen(port_num_t lgcPort, UINT32 *puiLen)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;
    uint32 ulLen = 0;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(puiLen == NULL)
    {
        return DRV_ERR_PARA;
    }
    
    phyid = PortLogic2PhyID(lgcPort);
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    iRv = rtk_switch_portMaxPktLen_get(CHIPID(phyid),PORTID(phyid), &ulLen);
    #endif
    #ifdef CHIPSET_RTL8305
    iRv = rtk_switch_portMaxPktLen_get(PORTID(phyid), &ulLen);
    #endif
    if(RT_ERR_OK == iRv)
    {      
        *puiLen = ulLen;
    }
    else
    {
        *puiLen = 0;
    }
    
    return ErrorSdk2Drv(iRv);
}
/* 014538 */
/*****************************************************************************
  函 数 名: Hal_SetPortMaxFrameLen

  函数描述: 设置端口支持的最大报文长度
  输入参数: UINT32 uiLen
  输出参数: 无
    返回值:
    注意点:
------------------------------------------------------------------------------
    修改历史
    日期        姓名             描述
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetPortMaxFrameLen(port_num_t lgcPort, UINT32 uiLen)
{
    phyid_t phyid = 0;
    int32 iRv = RT_ERR_OK;

    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    phyid = PortLogic2PhyID(lgcPort);
    #ifdef CHIPSET_RTL8305
    iRv = rtk_switch_portMaxPktLen_set(PORTID(phyid), uiLen);
    #endif
    if(RT_ERR_OK == iRv)
    {
        return DRV_OK;
    }
    else
    {
        return ErrorSdk2Drv(iRv);
    }

}
/* 014538 */
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)

DRV_RET_E Hal_FiberPortCfgInit(port_num_t lgcPort, COMBO_PORT_CONFIG_S *pstCfg)
{
    if(NULL == pstCfg)
    {
        return DRV_ERR_PARA;
    }
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
   
    RTK_PORT_DEBUG("Hal_FiberPortCfgInit, port=%d\n",lgcPort);
/*Begin modify ,2012-1-12 */
    SPIN_HAL_PORT_LOCK;
    memcpy(&(g_stComboPortCfg[lgcPort][PORT_TYPE_IS_FIBER]), pstCfg, sizeof(COMBO_PORT_CONFIG_S));
    SPIN_HAL_PORT_UNLOCK;
/*End modify ,2012-1-12*/

    (void)ComboPortCfgChange(CHIPID(PortLogic2PhyID(lgcPort)), PortLogic2PhyID(lgcPort), PORT_MEDIA_FIBER);
   
    return DRV_OK;
}


DRV_RET_E Hal_CopperPortCfgInit(port_num_t lgcPort, COMBO_PORT_CONFIG_S *pstCfg)
{
    
    if(NULL == pstCfg)
    {
        return DRV_ERR_PARA;
    }
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    
    RTK_PORT_DEBUG("Hal_CopperPortCfgInit, port=%d\n",lgcPort);
/*Begin modify ,2012-1-12 */
    SPIN_HAL_PORT_LOCK;
    memcpy(&(g_stComboPortCfg[lgcPort][PORT_TYPE_IS_COPPER]), pstCfg, sizeof(COMBO_PORT_CONFIG_S));
    SPIN_HAL_PORT_UNLOCK;
/*End modify,2012-1-12 */

    (void)ComboPortCfgChange(CHIPID(PortLogic2PhyID(lgcPort)), PortLogic2PhyID(lgcPort), PORT_MEDIA_COPPER);
    
    return DRV_OK;
}
/*****************************************************************************
  函 数 名: SFP_GetBxTypeIndex
  函数描述: 获取SFP BX类型的物理模块字符串信息
  输入参数: UINT8 *pucData    
               UINT32 ulCodeVal  
                  
  输出参数: UINT8 *pucIndex
    返回值: STATIC
    注意点: 
------------------------------------------------------------------------------
    修改历史                                                                  
    日期        姓名             描述                                         
  --------------------------------------------------------------------------
                                                                              
*****************************************************************************/
VOID SFP_GetBxTypeIndex(UINT8 *pucData, UINT32 ulCodeVal, UINT8 *pucIndex)
{
    UINT8 ucIndexBX = 0;
    UINT8 ucRbNominal = 0;
    UINT16 usWaveLen = 0;

    /*100x_BASE_BX_y_SFP读BR. Nominal (offset=12)其值>10 则x=0；
    否则去掉x ,读取波长值ww，1260<ww<1360 则y=U；否则y=D */
    ucRbNominal = pucData[I2C_RB_NOMINAL_ADDR];
    usWaveLen =  (UINT16)pucData[I2C_WAVELENGTH_ADDR] << 8;
    usWaveLen |= (UINT16)pucData[I2C_WAVELENGTH_ADDR + 1];

    /*这里是根据g_zTransceiverTypeBX里的值的位置来设定的*/
    if ((usWaveLen > SFP_WARELENGTH_SHORT) && (usWaveLen < SFP_WARELENGTH_LONG))
    {
        ucIndexBX += 1;                        
    }
    else
    {
        ucIndexBX += 0;
    }
    
    if (SFP_100X_BASE_BX_Y == ulCodeVal)  /*0X040*/
    {
        if (ucRbNominal > 10)
        {
            ucIndexBX += 2;
        }
        else
        {
            ucIndexBX += 0;
        }
    }
    else if (SFP_100_BASE_BX_Y == ulCodeVal)  /*0X240*/
    {
        /*上面已做处理*/
    }
    
    *pucIndex = ucIndexBX;
    return;
}


/*****************************************************************************
    Func Name: Hal_GetFiberSpeed
  Description: 获取光模块的速率
        Input: port_num_t lgcPort      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetFiberSpeed(port_num_t lgcPort, UINT32 *puiSpeed)
{
    UINT8  aucData[MAX_TRANSCEIVER_CODE_LEN] = {0};
    UINT8  aucTransceiverCodes[I2C_TRANSCEIVER_CODES_LEN];
    UINT8  ucBXIndex = 0;
    UINT32 ulReadLen = 0;
    UINT32 ulTransceiverCode = 0;

    if(NULL == puiSpeed)
    {
        return DRV_ERR_PARA;
    }

    memset(aucTransceiverCodes, 0, sizeof(aucTransceiverCodes));

    ulReadLen = I2C_TRANSCEIVER_CODES_LEN;
    RTK_PORT_DEBUG("lgcPort= %d, I2C_TRANSCEIVER_CODES_ADDR=%x, ulReadLen=%d,\n",lgcPort, I2C_TRANSCEIVER_CODES_ADDR,ulReadLen);
    if(DRV_OK != Hal_I2cFiberPortRead(lgcPort, I2C_TRANSCEIVER_CODES_ADDR, aucTransceiverCodes, ulReadLen, ADDRESS_IN_KERNEL_SPACE))
    {
        return DRV_I2C_READ_ERROR;
    }
    ulTransceiverCode =  (UINT32)aucTransceiverCodes[0] << 24;
    ulTransceiverCode |= (UINT32)aucTransceiverCodes[1] << 16;
    ulTransceiverCode |= (UINT32)aucTransceiverCodes[2] << 8;
    ulTransceiverCode |= (UINT32)aucTransceiverCodes[3];
    ulTransceiverCode &= I2C_RANSCEIVER_CODES_MASK;
    
    switch(ulTransceiverCode)
    {
        case SFP_100_BASE_SX1:
        case SFP_100_BASE_SX2:
        case SFP_100_BASE_SX3:
        case SFP_100_BASE_LX1:
        case SFP_100_BASE_LX2:
        case SFP_100_BASE_LX3:
        case SFP_100_BASE_BX_Y:
        case SFP_100_BASE_ZX:
            *puiSpeed = PORT_TYPE_FIBER_100;
            return DRV_OK;
            
        case SFP_1000_BASE_SX:
        case SFP_1000_BASE_LX:
        case SFP_1000_BASE_T_AN:
        case SFP_1000_BASE_LX_AN:
        case SFP_1000_BASE_PX:
            *puiSpeed = PORT_TYPE_FIBER_1000;
            return DRV_OK;

        case SFP_100X_BASE_BX_Y:    /*BX模块类型*/
            ulReadLen = MAX_TRANSCEIVER_CODE_LEN;
            if(DRV_OK !=  Hal_I2cFiberPortRead(lgcPort, I2C_START, aucData, ulReadLen, ADDRESS_IN_KERNEL_SPACE))
            {
                return DRV_I2C_READ_ERROR;
            }
            
            SFP_GetBxTypeIndex(aucData, ulTransceiverCode, &ucBXIndex);
            switch(ucBXIndex)
            {
                case SFP_100_BASE_BX_D:
                case SFP_100_BASE_BX_U:
                    *puiSpeed = PORT_TYPE_FIBER_100;
                    return DRV_OK;
                case SFP_1000_BASE_BX_D:
                case SFP_1000_BASE_BX_U:
                    *puiSpeed = PORT_TYPE_FIBER_1000;
                    return DRV_OK;
                default:
                    return DRV_ERR_UNKNOW;
            }
        case SFP_STACK_PLUS:  /*SFP 堆叠模块*/
        case SFP_STACK:
            return DRV_ERR_UNKNOW;
        default:
            break;
    }
    return DRV_ERR_UNKNOW;
}

/*****************************************************************************
    Func Name: ComboPortCfgChange
  Description: 当端口media类型改变时，更新端口配置
        Input: uint32 unit
               uint32 phyPort  
               rtk_port_media_t enMedia       
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
BOOL ComboPortCfgChange(uint32 unit, uint32 phyPort, rtk_port_media_t enMedia)
{
    UINT32 uiPortType = 0;
    port_num_t lgcPort;
    COMBO_PORT_CONFIG_S stComboPortCfg;
    rtk_enable_t enEnable = DISABLED;
    PortType_E uiSpeed = PORT_TYPE_FIBER_UNKNOWN;
    rtk_port_media_t enOldMedia = PORT_MEDIA_COPPER;
    
    lgcPort = PortPhyID2Logic(phyPort);
    
    RTK_PORT_DEBUG("ComboPortCfgChange, port=%d\n",lgcPort);
    
    if(!IS_COMBO_PORT(lgcPort))
    {
        return FALSE;
    }
    (void)rtk_port_phyComboPortMedia_get(CHIPID(phyPort), PORTID(phyPort), &enOldMedia);
    
    switch(enMedia)
    {
        case PORT_MEDIA_COPPER:
        {
            if((PORT_MEDIA_COPPER != enOldMedia) && (PORT_MEDIA_COPPER_AUTO != enOldMedia))
            {
                RTK_PORT_DEBUG("%s, %d\n",__FUNCTION__,__LINE__);
                return TRUE;
            }
            uiPortType = PORT_TYPE_IS_COPPER;
            break;
        }
        case PORT_MEDIA_FIBER:
        {           
            if((PORT_MEDIA_FIBER!= enOldMedia) && (PORT_MEDIA_FIBER_AUTO != enOldMedia))
            {
                RTK_PORT_DEBUG("%s, %d\n",__FUNCTION__,__LINE__);
                return TRUE;
            }
            uiPortType = PORT_TYPE_IS_FIBER;
            break;
        }  
        default:
            return FALSE; 
    }
    
    RTK_PORT_DEBUG("ComboPortCfgChange, port=%d, uiPortType=%d\n",lgcPort,uiPortType);
    
    SPIN_HAL_PORT_LOCK;
    memcpy(&stComboPortCfg, &(g_stComboPortCfg[lgcPort][uiPortType]), sizeof(COMBO_PORT_CONFIG_S));
    SPIN_HAL_PORT_UNLOCK;
    
    /*--------------------------------如果为光口，还要获取速率，并设置autoneg_advert, force_speed-----------------------------------*/
    if(PORT_TYPE_IS_FIBER == uiPortType)
    {
        (void)Hal_GetFiberSpeed(lgcPort, (UINT32 *)&uiSpeed);
        RTK_PORT_DEBUG("Hal_GetFiberSpeed, port=%d, uiSpeed=%d\n",lgcPort,uiSpeed);
        if(PORT_TYPE_FIBER_100 == uiSpeed)
        {
            stComboPortCfg.autoneg_advert &= ~(PORT_ABIL_1000MB_FD);
            stComboPortCfg.autoneg_advert &= ~(PORT_ABIL_100MB_HD);
            stComboPortCfg.autoneg_advert |= (PORT_ABIL_100MB_FD);
            stComboPortCfg.force_speed     = PORT_SPEED_100;
        }
        else
        {
            stComboPortCfg.autoneg_advert &= ~(PORT_ABIL_100MB_FD);
            stComboPortCfg.autoneg_advert |= (PORT_ABIL_1000MB_FD);
            stComboPortCfg.force_speed     = PORT_SPEED_1000;
        }
    }

    RTK_PORT_DEBUG("ComboPortCfgChange, port=%d, autoneg_enable=%d,\nautoneg_advert=%d,\nforce_speed=%d,\nforce_duplex=%d\n",lgcPort,stComboPortCfg.autoneg_enable,stComboPortCfg.autoneg_advert,stComboPortCfg.force_speed,stComboPortCfg.force_duplex);
    (void)Hal_SetPortAbility(lgcPort, stComboPortCfg.autoneg_advert, uiPortType);

    if (stComboPortCfg.autoneg_enable)
    {
        /* set auto enable and start auto */
        (void)Hal_SetPortAutoneg(lgcPort, TRUE, uiPortType);
    }
    else
    {
        (void)Hal_SetPortSpeed(lgcPort, stComboPortCfg.force_speed, uiPortType);
        (void)Hal_SetPortDuplex(lgcPort, stComboPortCfg.force_duplex, uiPortType);
        (void)Hal_SetPortAutoneg(lgcPort, FALSE, uiPortType);

        (void)rtk_port_adminEnable_get(unit, PORTID(phyPort), &enEnable);
        if(ENABLED == enEnable)
        {
            /* Turn off and then turn on the power of port so the partner could detect the speed/duplex change */
            Hal_SetPortEnable(lgcPort, FALSE);
            /*sleep 0.2sec*/
            osal_time_usleep(200000);
            Hal_SetPortEnable(lgcPort, TRUE);
        }

    }

    if(PORT_TYPE_IS_FIBER == uiPortType)
    {
        RTK_PORT_DEBUG("Hal_GetFiberSpeed, port=%d, uiSpeed=%d\n",lgcPort,uiSpeed);
        if(PORT_TYPE_FIBER_100 == uiSpeed)
        {
            /*先将能力位修改了，再设置fiber速率*/
            (void)rtk_port_phyComboPortFiberMedia_set( unit, PORTID(phyPort), PORT_FIBER_MEDIA_100);
            return TRUE;
        }
        else
        {
            (void)rtk_port_phyComboPortFiberMedia_set( unit, PORTID(phyPort), PORT_FIBER_MEDIA_1000);
            return TRUE;
        }
    }

    return TRUE;
}
#endif
/*****************************************************************************
    Func Name: Hal_PortLedInit
  Description: 设置端口的led灯
        Input:      
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_PortLedInit(void)
{
    /*全部的灯亮灭一下*/
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    (void)dal_esw_all_led_turn_on_and_off(CHIPID(PortLogic2PhyID(1)));
    #endif
    #if defined(CHIPSET_RTL8305)
    rtk_led_modeForce_set(LED_GROUP_1, LED_FORCE_ON);
    msleep(500);
    rtk_led_modeForce_set(LED_GROUP_1, LED_FORCE_NORMAL);
    #endif
    return DRV_OK;
}
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
#define FIBER_PORT_1_NO 25
#define FIBER_PORT_2_NO 26
#endif
/*****************************************************************************
    Func Name: Hal_SetPortLed
  Description: 设置端口的led灯
        Input: port_num_t lgcPort   
                UINT32 uiPortType   PORT_TYPE_IS_COPPER/PORT_TYPE_IS_FIBER
                UINT32 uiLinkState  PORT_LINK_UP/PORT_LINK_DOWN    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetPortLed(port_num_t lgcPort, UINT32 uiLinkState, UINT32 uiPortType)
{      
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    phyid_t phyid = 0;  
    UINT32 uiChipId = 0;
    UINT32 ulVal = 0;
    #endif
    INT32 iRv = RT_ERR_OK;

    #if defined(CHIPSET_RTL8305)
    (void)uiLinkState;
    #endif
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if((PORT_TYPE_IS_FIBER == uiPortType) && (IS_COPPER_PORT(lgcPort)))
    {
        return DRV_INVALID_VALUE;
    }
    
    /*当前只需要处理fiber的link active灯*/
    if((lgcPort <= DEVICE1_SLOT0_PORT_NO) || (PORT_TYPE_IS_COPPER == uiPortType))
    {
        return DRV_OK;
    }

      
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L) 
    phyid = PortLogic2PhyID(lgcPort); 
     uiChipId = CHIPID(phyid);
    if((iRv = dal_esw_led_software_control0_get(uiChipId, &ulVal)) != RT_ERR_OK)
    {
        return ErrorSdk2Drv(iRv);
    }
   
    /*端口25*/
    if(FIBER_PORT_1_NO == lgcPort)
    {
        if(PORT_LINK_UP == uiLinkState)
        {
            /*enable fiber 1 led*/
            ulVal |= 0x00000001;
            
        }
        else
        {
            /*disable fiber 1 led*/
            ulVal &= ~(0x00000001);
        }
    }
    /*端口26*/
    else if(FIBER_PORT_2_NO == lgcPort)
    {
        if(PORT_LINK_UP == uiLinkState)
        {
            /*enable fiber 2 led*/
            ulVal |= 0x00000100;
            
        }
        else
        {
            /*disable fiber 2 led*/
            ulVal &= ~(0x00000100);
        }
    }
    
    iRv = dal_esw_led_software_control0_set(uiChipId, ulVal);
    #endif
    return ErrorSdk2Drv(iRv);
}
/*****************************************************************************
    Func Name: Hal_SetInternalLoopback
  Description: 设置内环测试
        Input: port_num_t lgcPort   
                UINT32 uiEnable 开启/关闭
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

DRV_RET_E Hal_SetInternalLoopback(port_num_t lgcPort, UINT32 uiEnable)
{
    INT32 iRv = RT_ERR_OK;
    phyid_t phyid = 0;
    //rtk_enable_t enable;    
    UINT32 data;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }
    if(!ISVALID_LOOPBACK_VALUE(uiEnable))
    {
        return DRV_INVALID_LOOPBACK_VALUE;
    }
    
    switch (uiEnable)
    {
        case PORT_LOOPBACK_DISABLE:
            //enable = DISABLED;
            break;
        case PORT_LOOPBACK_ENABLE:
            //enable = ENABLED;
            break;
        default:
            return DRV_INVALID_LOOPBACK_VALUE;
    }
    if(PORT_LOOPBACK_ENABLE==uiEnable)
    {
        phyid = PortLogic2PhyID(lgcPort); 
        #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L) 
        iRv=rtk_port_phyReg_get(CHIPID(phyid), PORTID(phyid),0,PHY_REG_CONTROL,&data);
        #endif
        #if defined(CHIPSET_RTL8305)
        iRv=rtk_port_phyReg_get(PORTID(phyid),PHY_REG_CONTROL,&data);
        #endif
        if(DRV_OK!=ErrorSdk2Drv(iRv))
        {
            return ErrorSdk2Drv(iRv);
        }    
        if(data&(1<<14))
        {
            return DRV_OK;
        }
        data|=(1<<14);
        #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L) 
        iRv=rtk_port_phyReg_set(CHIPID(phyid), PORTID(phyid),0,PHY_REG_CONTROL,data);  
        #endif
        #if defined(CHIPSET_RTL8305)
        iRv=rtk_port_phyReg_set(PORTID(phyid),PHY_REG_CONTROL,data);
        #endif
        return ErrorSdk2Drv(iRv);
    }
    else
    {
        phyid = PortLogic2PhyID(lgcPort); 
        #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L) 
        iRv=rtk_port_phyReg_get(CHIPID(phyid), PORTID(phyid),0,PHY_REG_CONTROL,&data);
        #endif
        #if defined(CHIPSET_RTL8305)
        iRv=rtk_port_phyReg_get(PORTID(phyid),PHY_REG_CONTROL,&data);
        #endif
        if(DRV_OK!=ErrorSdk2Drv(iRv))
        {
            return ErrorSdk2Drv(iRv);
        }
        if(!(data&(1<<14)))
        {
            return DRV_OK;
        }
        data&=~(1<<14);  
        #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L) 
        iRv=rtk_port_phyReg_set(CHIPID(phyid), PORTID(phyid),0,PHY_REG_CONTROL,data); 
        #endif
        #if defined(CHIPSET_RTL8305)
        iRv=rtk_port_phyReg_set(PORTID(phyid),PHY_REG_CONTROL,data);
        #endif
        return ErrorSdk2Drv(iRv);
    }
}


void  rtk_port_parm_init(void)
{    
    int ii;
    
    printk("##### Hi call Hal_CopperPortCfgInit\n");
    ClrLgcMaskAll(&isolate_save);
    for ( ii = 0 ; ii <  (LOGIC_PORT_NO+1); ii++ )
    {
        spdlx_save[ii] = (uint32)PORT_AUTOAUTO;
    }
}

#if defined(CHIPSET_RTL8305)
/*****************************************************************************
    Func Name: Hal_SetPortAbility
  Description: 设置协商时端口的能力位
        Input: port_num_t lgcPort  
                UINT32 uiPortType   PORT_TYPE_IS_COPPER/PORT_TYPE_IS_FIBER
                UINT32 uiAbility    
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_RestartPortNeg(port_num_t lgcPort)
{
    phyid_t phyid = 0;
    INT32 iRv = RT_ERR_OK;      
    uint32 phyData = 0;
    
    if(!IsValidLgcPort(lgcPort))
    {
        return DRV_INVALID_LPORT;
    }

    phyid = PortLogic2PhyID(lgcPort);

    iRv = rtl8367b_setAsicPHYReg(phyid,PHY_PAGE_ADDRESS,0);
    if(DRV_OK!=ErrorSdk2Drv(iRv))
    {
        return ErrorSdk2Drv(iRv);
    }

    iRv = rtl8367b_getAsicPHYReg(phyid,PHY_CONTROL_REG,&phyData);
    if(DRV_OK!=ErrorSdk2Drv(iRv))
    {
        return ErrorSdk2Drv(iRv);
    }       

    /* 不支持自协商，返回失败 */
    if (!(phyData & (1U << PHY_CONTROL_REG_AUTO_NEGO_OFFSET)))
    {
        return DRV_INVALID_VALUE;
    }
	
    phyData |= (1U << PHY_CONTROL_REG_RESTART_AUTO_NEGO_OFFSET); /* Restart neg bit */
    iRv = rtl8367b_setAsicPHYReg(phyid,PHY_CONTROL_REG,phyData);
    if(DRV_OK!=ErrorSdk2Drv(iRv))
    {
        return ErrorSdk2Drv(iRv);
    }           
    
    iRv = rtl8367b_setAsicPHYReg(phyid,PHY_PAGE_ADDRESS,0);
	if(DRV_OK!=ErrorSdk2Drv(iRv))
    {
        return ErrorSdk2Drv(iRv);
    }
    
    return ErrorSdk2Drv(iRv);
}
#endif

/*  */
/*****************************************************************************
    Func Name: Hal_PortInit
 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_PortInit(void)
{
    rtk_port_t tdPhyId;
    rtk_api_ret_t retVal;
    rtk_port_phy_data_t tdPhyData = 0;
    
    /*Enable all of the ports.*/
    for(tdPhyId = 0; tdPhyId <= RTK_PHY_ID_MAX; tdPhyId++)
    {
        retVal = rtk_port_phyReg_get(tdPhyId, PHY_REG_CONTROL, &tdPhyData);
        if(RT_ERR_OK != retVal)
        {
            return DRV_SDK_GEN_ERROR;
        }
    
        tdPhyData &= 0xF7FF;
        tdPhyData |= 0x0200;
        retVal = rtk_port_phyReg_set(tdPhyId, PHY_REG_CONTROL, tdPhyData);
        if(RT_ERR_OK != retVal)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    return DRV_OK;
}
/* 014288 */

#ifdef  __cplusplus
}
#endif

