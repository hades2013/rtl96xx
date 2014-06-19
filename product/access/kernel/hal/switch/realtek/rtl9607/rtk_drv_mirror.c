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

#include <common/error.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/mirror.h>


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
    phy_pmask_t stIngressPhyMask, stEgressPhyMask;
	
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
    
    return DRV_OK;
}

void Hal_InitMirrorAclGrp(void)
{
    return;
}

/*not support stack yet*/
DRV_RET_E Hal_SetMirrorAclGrp(port_num_t mirrorDstPort, UINT32 *puiMirrorGrpId)
{
    if(!IsValidLgcPort(mirrorDstPort))
    {
        return DRV_ERR_PARA;
    }

    if(puiMirrorGrpId == NULL)
    {
        return DRV_ERR_PARA;
    }
    
    *puiMirrorGrpId=1;
    
    return DRV_OK;
}

/*not support stack yet*/
DRV_RET_E Hal_DelMirrorAclGrp(port_num_t mirrorDstPort)
{   
    if(!IsValidLgcPort(mirrorDstPort))
    {
        return DRV_ERR_PARA;
    }

    return DRV_OK;
}
#ifdef  __cplusplus
}
#endif

