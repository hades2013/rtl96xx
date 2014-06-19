/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
    extern "C"{
#endif
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include "hal_common.h"
//#include <common/rt_error.h>
//#include "dal/esw/dal_esw_led.h"
//#include <osal/time.h>
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include "rtk_api.h"
#include "rtk_api_ext.h"


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
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
typedef struct {
    port_num_t destPort;    /*0 if invalid*/
    UINT32 uiCount;         /*share count, remove group when 0*/
}mirrorAclGrp_t;

static mirrorAclGrp_t mirrorAclGrp[MAX_ACL_ACTION_MIRROR];
#endif
/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 宏定义                                       *
*----------------------------------------------*/


/*****************************************************************************
    Func Name:  Hal_SetMirrorGroup
  Description:  set ingress and egress mirror
        Input:  mirrorDestPort
                ingressMask
                egressMask
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
DRV_RET_E Hal_SetMirrorGroup(port_num_t mirrorDestPort, logic_pmask_t ingressMask, logic_pmask_t egressMask)
{
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    rtk_mirror_entry_t stMirrorEntry;
    #endif
    phy_pmask_t stIngressPhyMask, stEgressPhyMask;
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 unit;   
    
    /*init*/
    stMirrorEntry.cross_vlan = ENABLED;
    stMirrorEntry.flowBasedOnly = DISABLED;   
    stMirrorEntry.mirror_badPkt = ENABLED;
    stMirrorEntry.mirror_bcast = ENABLED;
    stMirrorEntry.mirror_goodPkt = ENABLED;
    stMirrorEntry.mirror_mcast = ENABLED;
    stMirrorEntry.mirror_orginalPkt = ENABLED;
    stMirrorEntry.mirror_ucast = ENABLED;
    stMirrorEntry.oper_of_igr_and_egr_ports = 0;
    #ifdef CHIPSET_RTL8328
    stMirrorEntry.mirroring_port = 0x1F;/*RTL8328 invalid entry*/
    #else
    stMirrorEntry.mirroring_port = 0xFF;
    #endif
    memset(&(stMirrorEntry.mirrored_igrPorts), 0, sizeof(rtk_portmask_t));
    memset(&(stMirrorEntry.mirrored_egrPorts), 0, sizeof(rtk_portmask_t));

    if(0xff == mirrorDestPort)
    {
        /*disable mirror*/
        UnitFor(unit)
        {
            
            if(RT_ERR_OK != rtk_mirror_group_set(unit, 0, &stMirrorEntry))
            {
                return DRV_ERR_UNKNOW;
            }
        }

        return DRV_OK;
    }

    /*check mirror dest port*/
    if(!VALID_PORT(mirrorDestPort))
    {
        return DRV_ERR_PARA;
    }

    /*translate port and port mask*/
    stMirrorEntry.mirroring_port = PortLogic2PhyPortId(mirrorDestPort);
    MaskLogic2Phy(&ingressMask, &stIngressPhyMask);
    MaskLogic2Phy(&egressMask, &stEgressPhyMask);    
    
    UnitFor(unit)
    {
        /*set mirror*/
        memcpy(&(stMirrorEntry.mirrored_igrPorts), &CHIPNMASK(unit, &stIngressPhyMask), sizeof(rtk_portmask_t));
        memcpy(&(stMirrorEntry.mirrored_egrPorts), &CHIPNMASK(unit, &stEgressPhyMask), sizeof(rtk_portmask_t));
        if(RT_ERR_OK != rtk_mirror_group_set(unit, 0, &stMirrorEntry))
        {
            return DRV_ERR_UNKNOW;
        }
    }
    #endif
    #ifdef CHIPSET_RTL8305
    memset(&stIngressPhyMask, 0, sizeof(stIngressPhyMask));
    memset(&stEgressPhyMask, 0, sizeof(stEgressPhyMask));
    if(0xff == mirrorDestPort)
    {
        port_num_t lPortNo = 0;
       /*disable mirror*/       
        LgcPortFor(lPortNo)
        {    
           if(RT_ERR_OK != rtk_mirror_portBased_set(PortLogic2PhyPortId(lPortNo), &CHIPNMASK(0, &stIngressPhyMask),&CHIPNMASK(0, &stEgressPhyMask)))
           {
               return DRV_ERR_UNKNOW;
           }    
        }

       return DRV_OK;
    }

    /*check mirror dest port*/
    if(!VALID_PORT(mirrorDestPort))
    {
       return DRV_ERR_PARA;
    }
    MaskLogic2Phy(&ingressMask, &stIngressPhyMask);
    MaskLogic2Phy(&egressMask, &stEgressPhyMask);  
    if(RT_ERR_OK!=rtk_mirror_portBased_set(PortLogic2PhyPortId(mirrorDestPort),&CHIPNMASK(0, &stIngressPhyMask),&CHIPNMASK(0, &stEgressPhyMask)))
    {
        return DRV_ERR_UNKNOW;
    }
    #endif
    return DRV_OK;
}

void Hal_InitMirrorAclGrp(void)
{
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    memset(mirrorAclGrp, 0, sizeof(mirrorAclGrp));
    #endif
    return;
}

/*not support stack yet*/
DRV_RET_E Hal_SetMirrorAclGrp(port_num_t mirrorDstPort, UINT32 *puiMirrorGrpId)
{
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 i, index = MAX_ACL_ACTION_MIRROR;    
    rtk_mirror_entry_t stMirrorEntry;
    #endif
    if(!IsValidLgcPort(mirrorDstPort))
    {
        return DRV_ERR_PARA;
    }

    if(puiMirrorGrpId == NULL)
    {
        return DRV_ERR_PARA;
    }
#if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    for(i = 0; i < MAX_ACL_ACTION_MIRROR; i++)
    {
        if(mirrorAclGrp[i].destPort == 0)
        {
            index = i;
        }
        else if(mirrorAclGrp[i].destPort == mirrorDstPort)
        {
            /*share with other acl entry*/
            *puiMirrorGrpId = i+1;
            mirrorAclGrp[i].uiCount++;
            return DRV_OK;
        }
    }

    if(index == MAX_ACL_ACTION_MIRROR)
    {
        return DRV_ERR_MIR_FULL;
    }
    else
    {
        mirrorAclGrp[index].destPort = mirrorDstPort;
    }
   
    /*init*/
    stMirrorEntry.cross_vlan = ENABLED;
    stMirrorEntry.flowBasedOnly = ENABLED;  
    stMirrorEntry.mirror_badPkt = ENABLED;
    stMirrorEntry.mirror_bcast = ENABLED;
    stMirrorEntry.mirror_goodPkt = ENABLED;
    stMirrorEntry.mirror_mcast = ENABLED;
    stMirrorEntry.mirror_orginalPkt = ENABLED;
    stMirrorEntry.mirror_ucast = ENABLED;
    stMirrorEntry.oper_of_igr_and_egr_ports = 0;
    memset(&(stMirrorEntry.mirrored_igrPorts), 0xff, sizeof(rtk_portmask_t));
    memset(&(stMirrorEntry.mirrored_egrPorts), 0xff, sizeof(rtk_portmask_t));

    /*translate port and port mask*/
    stMirrorEntry.mirroring_port = PortLogic2PhyPortId(mirrorDstPort);
    
    if(RT_ERR_OK != rtk_mirror_group_set(0, index+1, &stMirrorEntry))
    {
        mirrorAclGrp[index].destPort = 0;
        return DRV_ERR_UNKNOW;
    }
   
    mirrorAclGrp[index].uiCount = 1;

    *puiMirrorGrpId = index+1;
    #endif
    #ifdef CHIPSET_RTL8305
    *puiMirrorGrpId=1;
    #endif
    return DRV_OK;
}

/*not support stack yet*/
DRV_RET_E Hal_DelMirrorAclGrp(port_num_t mirrorDstPort)
{   
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L) 
    UINT32 i;    
    rtk_mirror_entry_t stMirrorEntry;
    #endif
    if(!IsValidLgcPort(mirrorDstPort))
    {
        return DRV_ERR_PARA;
    }
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L) 
    for(i = 0; i < MAX_ACL_ACTION_MIRROR; i++)
    {
        if(mirrorAclGrp[i].destPort == mirrorDstPort)
        {
            /*check if shared with other acl entry*/
            if(mirrorAclGrp[i].uiCount)
            {
                mirrorAclGrp[i].uiCount--;
                if(mirrorAclGrp[i].uiCount == 0)
                {
                    break;
                }
                else
                {
                    return DRV_OK;
                }
            }
        }
    }

    if(i == MAX_ACL_ACTION_MIRROR)
    {
        return DRV_ERR_MIR_NO_MONITOR;
    }
    
    /*init*/
    stMirrorEntry.cross_vlan = ENABLED;
    stMirrorEntry.flowBasedOnly = ENABLED;    
    stMirrorEntry.mirror_badPkt = ENABLED;
    stMirrorEntry.mirror_bcast = ENABLED;
    stMirrorEntry.mirror_goodPkt = ENABLED;
    stMirrorEntry.mirror_mcast = ENABLED;
    stMirrorEntry.mirror_orginalPkt = ENABLED;
    stMirrorEntry.mirror_ucast = ENABLED;
    stMirrorEntry.oper_of_igr_and_egr_ports = 0;
    #ifdef CHIPSET_RTL8328
    stMirrorEntry.mirroring_port = 0x1F;/*RTL8328 invalid entry*/
    #else
    stMirrorEntry.mirroring_port = 0xFF;
    #endif
    memset(&(stMirrorEntry.mirrored_igrPorts), 0, sizeof(rtk_portmask_t));
    memset(&(stMirrorEntry.mirrored_egrPorts), 0, sizeof(rtk_portmask_t));
    
    /*disable mirror*/ 
    if(RT_ERR_OK != rtk_mirror_group_set(0, i+1, &stMirrorEntry))
    {
        return DRV_ERR_UNKNOW;
    }
    
    mirrorAclGrp[i].destPort = 0;
    #endif
    return DRV_OK;
}
#ifdef  __cplusplus
}
#endif

