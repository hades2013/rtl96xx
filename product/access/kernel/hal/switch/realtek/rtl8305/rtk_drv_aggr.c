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
    Func Name:  Hal_SetAggrMode
  Description:  set global aggr mode
        Input:  UINT32 ulMode     
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
DRV_RET_E Hal_SetAggrMode(UINT32 ulMode)
{
    #if defined(CHIPSET_RTL8305)
    (void)ulMode;     
    #endif

    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    UINT32 ulUnit, ulGroupNo, ulMaxGroup, ulAlgoBitmask=0;

    /*
    SA_TRUNK=0,             
    DA_TRUNK,
    SADA_TRUNK,         
    SAIPDAIP_TRUNK,    
    SADAIP_TRUNK,
    */
    switch(ulMode)
    {
        case SA_TRUNK:
            ulAlgoBitmask = TRUNK_DISTRIBUTION_ALGO_SMAC_BIT;
            break;
        case DA_TRUNK:
            ulAlgoBitmask = TRUNK_DISTRIBUTION_ALGO_DMAC_BIT;
            break;
        case SADA_TRUNK:
            ulAlgoBitmask = TRUNK_DISTRIBUTION_ALGO_SMAC_BIT|TRUNK_DISTRIBUTION_ALGO_DMAC_BIT;
            break;
        case SAIPDAIP_TRUNK:
            ulAlgoBitmask = TRUNK_DISTRIBUTION_ALGO_SIP_BIT|TRUNK_DISTRIBUTION_ALGO_DIP_BIT;
            break;
        case SADAIP_TRUNK:
            ulAlgoBitmask = TRUNK_DISTRIBUTION_ALGO_SMAC_BIT|TRUNK_DISTRIBUTION_ALGO_DMAC_BIT|
                TRUNK_DISTRIBUTION_ALGO_SIP_BIT|TRUNK_DISTRIBUTION_ALGO_DIP_BIT;
            break;
        default:
            return DRV_ERR_PARA;
    }

    UnitFor(ulUnit)
    {
        ulMaxGroup = HAL_MAX_NUM_OF_TRUNK(ulUnit);
        for(ulGroupNo = 0; ulGroupNo < ulMaxGroup; ulGroupNo++)
        {
            /*
            #define TRUNK_DISTRIBUTION_ALGO_SPA_BIT         0x01
            #define TRUNK_DISTRIBUTION_ALGO_SMAC_BIT        0x02
            #define TRUNK_DISTRIBUTION_ALGO_DMAC_BIT        0x04
            #define TRUNK_DISTRIBUTION_ALGO_SIP_BIT         0x08
            #define TRUNK_DISTRIBUTION_ALGO_DIP_BIT         0x10
            #define TRUNK_DISTRIBUTION_ALGO_SRC_L4PORT_BIT  0x20
            #define TRUNK_DISTRIBUTION_ALGO_DST_L4PORT_BIT  0x40
            #define TRUNK_DISTRIBUTION_ALGO_MASKALL         0x7F
            */
            if(RT_ERR_OK != rtk_trunk_distributionAlgorithm_set(ulUnit, ulGroupNo, ulAlgoBitmask))
            {
                return DRV_ERR_UNKNOW;
            }
        }
    }
    #endif
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetAggrGroup
  Description:  set aggr group
        Input:  grpNum
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
DRV_RET_E Hal_SetAggrGroup(agg_grp_num_t grpNum, logic_pmask_t stLgcMask)
{
    #if defined(CHIPSET_RTL8305)
    (void)grpNum;
    (void)stLgcMask;  
    #endif
    #if defined(CHIPSET_RTL8328) || defined(CHIPSET_RTL8328L)
    phy_pmask_t stPhyMask;
    UINT32 ulUnit;
    INT32 lRtkRetVal = 0;
    
    if(grpNum >= AGGRE_GRP_NUM)
    {
        return DRV_ERR_PARA;
    }
    
    MaskLogic2Phy(&stLgcMask, &stPhyMask);
    UnitFor(ulUnit)
    {
        /*stop link-scan*/
        lRtkRetVal = rtk_trunk_port_set(ulUnit, grpNum, &CHIPNMASK(ulUnit, &stPhyMask));
        /*recover link-scan*/
        if(RT_ERR_OK != lRtkRetVal)
        {
            return DRV_ERR_UNKNOW;
        }
    }    
    #endif
    return DRV_OK;
}

#ifdef  __cplusplus
}
#endif

