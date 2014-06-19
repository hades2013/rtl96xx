/*****************************************************************************
------------------------------------------------------------------------------
  --------------------------------------------------------------------------
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
    extern "C"{
#endif

#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include "hal_common.h"
/*realtek sdk include*/
#include <common/error.h>
#include <common/rt_error.h>
#include <common/rt_type.h>

#include <hal/chipdef/apollomp/apollomp_def.h>
#include <hal/common/halctrl.h>

#include <rtk/l2.h>
#include <rtk/trap.h>

#include <linux/spinlock_types.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>

/*lint -save -e550 -e522 */

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static UINT32 _Hal_MacCompare(mac_address_t address_1, mac_address_t address_2);
extern void msleep(unsigned int msecs);

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
static mac_dump_ucast_tab_t _mac_ucast_entry_dump[CHIP_NO][MAX_ARL_TABLE];
static mac_dump_ucast_link_t _mac_ucast_dump_link[CHIP_NO][MAX_ARL_TABLE];
static mac_dump_ucast_tab_t _mac_ucast_entry_dump_sorted[MAX_ARL_TABLE];
static mac_dump_ucast_link_t *pstMacUcastDumpLinkHead = NULL;
static mac_mcast_t _mac_mcast_entry_dump[MAX_MULTICAST_ENTRY];
static UINT32 ulMacMcastCount = 0, ulMacMacstInited = 0;
//static mac_mcast_t  
static UINT32 ulMacDumpInited = 0;
/*lint -save -e86 -e133 -e43 -e651 -e155 -e69 -e110 -e24 -e40 -e63 -e35 -e785 -e10 -esym(552,if_lock) -e830 -e745 -e533*/
static spinlock_t _spin_hal_mac_lock = SPIN_LOCK_UNLOCKED;
/*lint -restore*/
static UINT32 ulMacCurrentIndex = 0;

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 宏定义                                       *
*----------------------------------------------*/
#define DRV_MAC_DUMP_NUMBER_PER_ROLL 2000
#define SPIN_HAL_MAC_LOCK       spin_lock(&_spin_hal_mac_lock)
#define SPIN_HAL_MAC_UNLOCK     spin_unlock(&_spin_hal_mac_lock)

#if 0
#define diag_printk printk
#else
#define diag_printk(fmt, ...) 
#endif

/*inser _dump_link_insert before _dump_link_cur*/
void DUMP_LINK_INSERT_PRE(mac_dump_ucast_link_t *_dump_link_cur, mac_dump_ucast_link_t *_dump_link_insert)
{    
    if(pstMacUcastDumpLinkHead == _dump_link_cur)
    {
        /*insert before head*/
        pstMacUcastDumpLinkHead = _dump_link_insert;
        _dump_link_insert->pstMacDumpLinkPre = NULL;
        _dump_link_insert->pstMacDumpLinkNext = _dump_link_cur;
        _dump_link_cur->pstMacDumpLinkPre = _dump_link_insert;
    }
    else
    {
        /*insert before current*/
        _dump_link_insert->pstMacDumpLinkPre = _dump_link_cur->pstMacDumpLinkPre;
        _dump_link_insert->pstMacDumpLinkNext = _dump_link_cur;
        if(NULL != _dump_link_cur->pstMacDumpLinkPre)
        {
            _dump_link_cur->pstMacDumpLinkPre->pstMacDumpLinkNext = _dump_link_insert;
        }
        _dump_link_cur->pstMacDumpLinkPre = _dump_link_insert;
    }
}

void DUMP_LINK_REMOVE(UINT32 _unit, UINT32 _index)
{
    if(_mac_ucast_entry_dump[_unit][_index].flag & DRV_MAC_DUMP_FLAG_VALID)
    {
        if(pstMacUcastDumpLinkHead != &(_mac_ucast_dump_link[_unit][_index]))
        {
            /*not head*/
            _mac_ucast_dump_link[_unit][_index].pstMacDumpLinkPre->pstMacDumpLinkNext = 
                _mac_ucast_dump_link[_unit][_index].pstMacDumpLinkNext;
            _mac_ucast_dump_link[_unit][_index].pstMacDumpLinkNext->pstMacDumpLinkPre = 
                _mac_ucast_dump_link[_unit][_index].pstMacDumpLinkPre;            
        }
        else
        {
            /*remove head*/
            pstMacUcastDumpLinkHead = _mac_ucast_dump_link[_unit][_index].pstMacDumpLinkNext;
            if(NULL != pstMacUcastDumpLinkHead)
            {
                pstMacUcastDumpLinkHead->pstMacDumpLinkPre = NULL;
            }            
        }
        _mac_ucast_dump_link[_unit][_index].pstMacDumpLinkPre = NULL;
        _mac_ucast_dump_link[_unit][_index].pstMacDumpLinkNext = NULL;
        _mac_ucast_entry_dump[_unit][_index].flag = 0;
    }
}

/*****************************************************************************
    Func Name:  Hal_SetCpuPortMacLearnEnable
  Description:  enable or disable mac learning by cpu port
        Input:  ulEnable
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------


*****************************************************************************/
DRV_RET_E Hal_SetCpuPortMacLearnEnable(UINT32 ulEnable)
{        
    rtk_api_ret_t rtkApiRet;
    
    if(MAC_LEARN_FWD_ENABLED == ulEnable)
    {
        /*Enable mac learning*/
        rtkApiRet = rtk_l2_portLimitLearningCnt_set(PHY_CPU_PORTID, HAL_L2_LEARN_LIMIT_CNT_MAX());
        if (RT_ERR_OK != rtkApiRet)
        {
            return DRV_SDK_GEN_ERROR;
        }
        /*Forward packet when learning mac num exceeds the limited num.*/
        rtkApiRet = rtk_l2_portLimitLearningCntAction_set(PHY_CPU_PORTID, LIMIT_LEARN_CNT_ACTION_FORWARD);
        if (RT_ERR_OK != rtkApiRet)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }
    else if(MAC_LEARN_FWD_DISABLED == ulEnable)
    {
        /*Disable mac learning*/
        rtkApiRet = rtk_l2_portLimitLearningCnt_set(PHY_CPU_PORTID, 0);
        if (RT_ERR_OK != rtkApiRet)
        {
            return DRV_SDK_GEN_ERROR;
        }

        /*Forward packet when learning mac num exceeds the limited num.*/
        rtkApiRet = rtk_l2_portLimitLearningCntAction_set(PHY_CPU_PORTID, LIMIT_LEARN_CNT_ACTION_FORWARD);
        if (RT_ERR_OK != rtkApiRet)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }
    else
    {
        return DRV_ERR_PARA;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetMacLearnEnable
  Description:  enable or disable mac learning by port
        Input:  ulLgcPort
                ulEnable
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetMacLearnEnable(UINT32 ulEnable, logic_pmask_t stPortMask)
{        
    port_num_t lport;
    rtk_api_ret_t rtkApiRet;

    LgcPortFor(lport)
    {
        if(TstLgcMaskBit(lport, &stPortMask))
        {
            if(MAC_LEARN_FWD_ENABLED == ulEnable)
            {
                /*Enable mac learning*/
                rtkApiRet = rtk_l2_portLimitLearningCnt_set(PortLogic2PhyPortId(lport), HAL_L2_LEARN_LIMIT_CNT_MAX());
                if (RT_ERR_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
                /*Forward packet when learning mac num exceeds the limited num.*/
                rtkApiRet = rtk_l2_portLimitLearningCntAction_set(PortLogic2PhyPortId(lport), LIMIT_LEARN_CNT_ACTION_FORWARD);
                if (RT_ERR_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
            }
            else if(MAC_LEARN_FWD_DISABLED == ulEnable)
            {
                /*Disable mac learning*/
                rtkApiRet = rtk_l2_portLimitLearningCnt_set(PortLogic2PhyPortId(lport), 0);
                if (RT_ERR_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
                /*Drop packet when learning mac num exceeds the limited num.*/
                rtkApiRet = rtk_l2_portLimitLearningCntAction_set(PortLogic2PhyPortId(lport), LIMIT_LEARN_CNT_ACTION_DROP);
                if (RT_ERR_OK != rtkApiRet)
                {
                    return DRV_SDK_GEN_ERROR;
                }
            }
            else
            {
                return DRV_ERR_PARA;
            }
        }
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_GetMacLearnEnable
  Description:  get mac learning status by port
        Input:  ulLgcPort
                pulEnable
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
DRV_RET_E Hal_GetMacLearnEnable(UINT32 ulLgcPort, UINT32 *pulEnable)
{    
    rtk_api_ret_t rtkApiRet;
    UINT32 rtkMacCnt;
    
    if(!VALID_PORT(ulLgcPort))
    {
        return DRV_ERR_PARA;
    }
    if(NULL == pulEnable)
    {
        return DRV_ERR_PARA;
    }

    rtkApiRet = rtk_l2_portLimitLearningCnt_get(PortLogci2ChipId(ulLgcPort), &rtkMacCnt);
    if (RT_ERR_OK != rtkApiRet)
    {
        return DRV_SDK_GEN_ERROR;
    }

    if(0 == rtkMacCnt)
    {
        *pulEnable = MAC_LEARN_FWD_DISABLED;
    }
    else
    {
        *pulEnable = MAC_LEARN_FWD_ENABLED;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetMacCpuMacAdd
  Description:  add cpu mac address
        Input:  ulVid
                mac_address
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
DRV_RET_E Hal_SetMacCpuMacAdd(vlan_id_t tdVid, mac_address_t mac_address)
{
    rtk_vlan_t tdVlanId;
    rtk_l2_ucastAddr_t stUcastMacAdd;

    if(!VALID_VLAN_ID(tdVid))
    {
        return DRV_ERR_PARA;
    }

    tdVlanId = tdVid;
	memset(&stUcastMacAdd, 0, sizeof(stUcastMacAdd));
    memcpy(stUcastMacAdd.mac.octet, mac_address, sizeof(mac_address_t));    
	//stUcastMacAdd.flags |= RTK_L2_UCAST_FLAG_IVL;
	stUcastMacAdd.flags |= RTK_L2_UCAST_FLAG_STATIC;
	stUcastMacAdd.vid = tdVlanId;
    stUcastMacAdd.port = PHY_CPU_PORTID;

    /*The following opration will renew the existed LUT entry.*/
    if(RT_ERR_OK != rtk_l2_addr_add(&stUcastMacAdd))
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetMacFlushUcastMac
  Description:  flush unicast mac by port(include trunk group port), vlan id, static mac,
                all mac or specified MAC
        Input:  
       Output: 
       Return:  DRV_ERR_PARA
                DRV_ERR_UNKNOW
                DRV_ERR_MAC_ADDR_NOT_FOUND
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetMacFlushUcastMac(mac_delete_t stMacDelete)
{
    rtk_l2_flushCfg_t stMacFlush;
    port_num_t lport;

    /*Do not support this functionality*/
    if(ENABLE == stMacDelete.flushByTunk)
    {
        return DRV_ERR_PARA;
    }
    
    memset(&stMacFlush, 0, sizeof(rtk_l2_flushCfg_t));

    /*flush all*/
    if(ENABLE == stMacDelete.flushAll)
    {
		/*Begin modified by sunmingliang for bug 260*/
	    stMacFlush.flushDynamicAddr = ENABLED;
		/*End modified by sunmingliang for bug 260*/
        if(ENABLE == stMacDelete.flushStaticAddr)
        {            
            stMacFlush.flushStaticAddr = ENABLED;
        }

        /*flush mac by port*/
        LgcPortFor(lport)
        {
            stMacFlush.flushByPort = ENABLED;
            stMacFlush.port = PortLogic2PhyPortId(lport);
            if(RT_ERR_OK != rtk_l2_ucastAddr_flush(&stMacFlush))
            {
                return DRV_ERR_UNKNOW;
            }
        }

        stMacFlush.flushByPort = ENABLED;
        stMacFlush.port = PHY_CPU_PORTID;
        if(RT_ERR_OK != rtk_l2_ucastAddr_flush(&stMacFlush))
        {
            return DRV_ERR_UNKNOW;
        }

        return DRV_OK;
    }    

    /*specified vlan id*/
    if(ENABLE == stMacDelete.flushByVid)
    {
        if(!VALID_VLAN_ID(stMacDelete.vid))
        {
            return DRV_ERR_PARA;
        }
        stMacFlush.flushByVid = ENABLED;
        stMacFlush.vid = stMacDelete.vid;
    }

    /*specified port*/
    if(ENABLE == stMacDelete.flushByPort)
    {
	
        if(!VALID_PORT(stMacDelete.port) && LOGIC_CPU_PORT != stMacDelete.port)
        {
            return DRV_ERR_PARA;
        }
        stMacFlush.port = PortLogic2PhyPortId(stMacDelete.port);
        stMacFlush.flushByPort = ENABLED;
    }

    /*specified MAC address*/
	#if 0
	if(ENABLE == stMacDelete.flushByMac)
    {
        stMacFlush.flushByMac = ENABLED;
        memcpy(stMacFlush.ucastAddr.octet, stMacDelete.macAddr, sizeof(mac_address_t));
    }
	#endif

    /*static*/
    if(ENABLE == stMacDelete.flushStaticAddr)
    {
        stMacFlush.flushStaticAddr = ENABLED;
    }
	/*Begin modified by sunmingliang for bug 260*/
	else
	{
		stMacFlush.flushDynamicAddr = ENABLED;
	}
	/*End modified by sunmingliang for bug 260*/
    if(RT_ERR_OK != rtk_l2_ucastAddr_flush(&stMacFlush))
    {
        if(ENABLE == stMacDelete.flushByMac)
        {
            return DRV_ERR_MAC_ADDR_NOT_FOUND;
        }
        else
        {
            return DRV_ERR_UNKNOW;
        }
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetMacAddUcastMac
  Description:  add unicast MAC
        Input:  stMacUcast
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
DRV_RET_E Hal_SetMacAddUcastMac(mac_ucast_t stMacUcast)
{
    rtk_l2_ucastAddr_t ucast_addr;
	/*Begin modified by sunmingliang for bug 260*/
    rtk_l2_ucastAddr_t l2Addr;

    if(!VALID_VLAN_ID(stMacUcast.tdVid) && (0 != stMacUcast.tdVid))
    {
        return DRV_ERR_PARA;
    }

    if(IS_MULTICAST(stMacUcast.mac_addr))
    {
        /*multicast*/
        return DRV_ERR_PARA;
    }

    /*8305 does not support this functionality.*/
    if(stMacUcast.ucIsAggr)
    {
        return DRV_ERR_PARA;
    }
    
    /*set struct*/
    memset(&ucast_addr, 0, sizeof(rtk_l2_ucastAddr_t));
    memset(&l2Addr, 0, sizeof(rtk_l2_ucastAddr_t));
	memcpy(ucast_addr.mac.octet, stMacUcast.mac_addr, sizeof(mac_address_t));
	//ucast_addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    ucast_addr.vid = (rtk_uint32)stMacUcast.tdVid;
	/*only use fid 0*/
	ucast_addr.fid= 0;
	ucast_addr.efid = 0;

    /*add to port*/
    if (VALID_PORT(stMacUcast.lgcPort))
    {
		if(stMacUcast.ucStatic)
		{
			ucast_addr.flags |= RTK_L2_UCAST_FLAG_STATIC;
		}
		else
		{
			ucast_addr.port = PortLogic2PhyPortId(stMacUcast.lgcPort);

			
			memcpy(&l2Addr, &ucast_addr, sizeof(rtk_l2_ucastAddr_t));
			
			ucast_addr.flags |= RTK_L2_UCAST_FLAG_STATIC;
			if(RT_ERR_OK != rtk_l2_addr_add(&ucast_addr))
			{
				return DRV_ERR_UNKNOW;
			}
			
			memcpy(&ucast_addr, &l2Addr, sizeof(rtk_l2_ucastAddr_t));
			if(RT_ERR_OK != rtk_l2_addr_add(&ucast_addr))
			{
				return DRV_ERR_UNKNOW;
			}
			
			memcpy(&ucast_addr, &l2Addr, sizeof(rtk_l2_ucastAddr_t));
			/*same with diag*/
			ucast_addr.age = 7;

		}
        ucast_addr.port = PortLogic2PhyPortId(stMacUcast.lgcPort);
    }
    /*balckhole address*/
    else if(0 == stMacUcast.lgcPort)
    {      
        ucast_addr.flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;
		ucast_addr.flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;
		ucast_addr.flags |= RTK_L2_UCAST_FLAG_STATIC;
    }
    else
    {
        return DRV_ERR_PARA;
    }  
	
    /*add mac address to chip*/
    /*The following opration will renew the existed LUT entry.*/
    if(RT_ERR_OK != rtk_l2_addr_add(&ucast_addr))
    {
        return DRV_ERR_UNKNOW;
    }
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetMacDelUcastMac
  Description:  delete unicast MAC
        Input:  stMacUcast
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetMacDelUcastMac(mac_ucast_t stMacUcast)
{
    rtk_l2_ucastAddr_t stUniLutEntry;
    
    if(!VALID_VLAN_ID(stMacUcast.tdVid) && (0 != stMacUcast.tdVid))
    {
        return DRV_ERR_PARA;
    }

    if(IS_MULTICAST(stMacUcast.mac_addr))
    {
        /*multicast*/
        return DRV_ERR_PARA;
    }
    
    memset(&stUniLutEntry, 0, sizeof(stUniLutEntry));	
	memcpy(stUniLutEntry.mac.octet, stMacUcast.mac_addr, sizeof(mac_address_t));
//    stUniLutEntry.vid = stMacUcast.tdVid;
	stUniLutEntry.fid = 0;
	stUniLutEntry.efid = 0;

	/*End modified by sunmingliang for bug 260*/
    if(RT_ERR_OK != rtk_l2_addr_del(&stUniLutEntry))
    {
        return DRV_SDK_GEN_ERROR;
    }
    return DRV_OK;
}

static DRV_RET_E _Hal_MacMcastAdd(mac_mcast_t *pstMacMcast)
{
    UINT32 i;

    mac_address_t pMacAddr;
    /*设置一个全0的MAC地址*/
    memset(pMacAddr, 0, sizeof(mac_address_t));


    if(ulMacMcastCount >= MAX_MULTICAST_ENTRY)
    {
        return DRV_ERR_UNKNOW;
    }
    for(i = 0; i < MAX_MULTICAST_ENTRY; i++)
    {
        if(0 == _Hal_MacCompare(_mac_mcast_entry_dump[i].mac_addr, pMacAddr))
        {
            _mac_mcast_entry_dump[i].tdVid = pstMacMcast->tdVid;
            memcpy(_mac_mcast_entry_dump[i].mac_addr, pstMacMcast->mac_addr, sizeof(mac_address_t));
            memcpy(&(_mac_mcast_entry_dump[i].port_mask), &(pstMacMcast->port_mask), sizeof(logic_pmask_t));
            ulMacMcastCount++;
            return DRV_OK;
        }
    }
    return DRV_ERR_UNKNOW;
}

static DRV_RET_E _Hal_MacMcastDel(mac_mcast_t *pstMacMcast)
{
    UINT32 i;
    
    for(i = 0; i < MAX_MULTICAST_ENTRY; i++)
    {
        if(pstMacMcast->tdVid == _mac_mcast_entry_dump[i].tdVid)
        {
            if(!_Hal_MacCompare(pstMacMcast->mac_addr, _mac_mcast_entry_dump[i].mac_addr))
            {
                memset(&_mac_mcast_entry_dump[i], 0, sizeof(_mac_mcast_entry_dump[i]));
				if(ulMacMcastCount>0)
                	ulMacMcastCount--;
                
                return DRV_OK;
            }            
        }
    }
    
    diag_printk("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
    return DRV_ERR_UNKNOW;
}

static DRV_RET_E _Hal_MacMcastSet(mac_mcast_t *pstMacMcast)
{
    UINT32 i;
    
    for(i = 0; i < MAX_MULTICAST_ENTRY; i++)
    {
        if(pstMacMcast->tdVid == _mac_mcast_entry_dump[i].tdVid)
        {
            if(!_Hal_MacCompare(pstMacMcast->mac_addr, _mac_mcast_entry_dump[i].mac_addr))
            {
                memcpy(&(_mac_mcast_entry_dump[i].port_mask), &(pstMacMcast->port_mask), sizeof(logic_pmask_t));
                return DRV_OK;
            }            
        }
    }
    return DRV_ERR_UNKNOW;
}

/*****************************************************************************
    Func Name: _Hal_MacMcastGetByVid
  Description: Get multicast mac entry by vlan id.
        Input: vlan_id_t tdVid           
                mac_mcast_t *pstMacMcast  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_MacMcastGetByVid(vlan_id_t tdVid, mac_mcast_t *pstMacMcast)
{
    UINT32 i;

    if (NULL == pstMacMcast)
    {
        return DRV_ERR_UNKNOW;
    }
    
    for(i = 0; i < MAX_MULTICAST_ENTRY; i++)
    {
        if(tdVid == _mac_mcast_entry_dump[i].tdVid)
        {
            memcpy(pstMacMcast, &_mac_mcast_entry_dump[i], sizeof(mac_mcast_t));
            return DRV_OK;         
        }
    }
    return DRV_NULL_POINTER;
}

/*****************************************************************************
    Func Name: _Hal_MacMcastGetByMac
  Description: Get mcentry by mac.
        Input: mac_address_t tdMac
               mac_mcast_t *pstMacMcast  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_MacMcastGetByMac(mac_address_t tdMac, mac_mcast_t *pstMacMcast)
{
    UINT32 i;

    if ((NULL == tdMac) || (NULL == pstMacMcast))
    {
        return DRV_ERR_UNKNOW;
    }
    
    for(i = 0; i < MAX_MULTICAST_ENTRY; i++)
    {
        if(!_Hal_MacCompare(tdMac, _mac_mcast_entry_dump[i].mac_addr))
        {
            memcpy(pstMacMcast, &_mac_mcast_entry_dump[i], sizeof(mac_mcast_t));
            return DRV_OK;
        }            
    }
    return DRV_ERR_UNKNOW;
}

/*****************************************************************************
    Func Name:  Hal_SetMacAddMcastMac
  Description:  add a new multicast MAC
        Input:  stMacMcast
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
DRV_RET_E Hal_SetMacAddMcastMac(mac_mcast_t stMacMcast)
{
    UINT32 unit;
    phy_pmask_t phy_mask;
    rtk_api_ret_t rtkApiRet;
    rtk_l2_mcastAddr_t stMcastAddr;

    MaskLogic2Phy(&(stMacMcast.port_mask), &phy_mask);

    memset(&stMcastAddr, 0, sizeof(stMcastAddr));
    memcpy(stMcastAddr.mac.octet, stMacMcast.mac_addr, sizeof(mac_address_t));
	//stMcastAddr.flags |= RTK_L2_UCAST_FLAG_IVL;

    UnitFor(unit)
    {
        memcpy(&stMcastAddr.portmask, &phy_mask.pmask[unit], sizeof(rtk_portmask_t));
		/*use svl fid 0 */
        rtkApiRet = rtk_l2_mcastAddr_add(&stMcastAddr);
        if (RT_ERR_OK != rtkApiRet)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    SPIN_HAL_MAC_LOCK;
    if(!ulMacMacstInited)
    {        
        memset(_mac_mcast_entry_dump, 0, sizeof(_mac_mcast_entry_dump));
        ulMacMacstInited = 1;        
    }

    (void)_Hal_MacMcastAdd(&stMacMcast);
    SPIN_HAL_MAC_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetMacDelMcastMac
  Description:  delete multicast MAC
        Input:  stMacMcast
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
DRV_RET_E Hal_SetMacDelMcastMac(mac_mcast_t stMacMcast)
{
   
    DRV_RET_E enRet;
	rtk_l2_mcastAddr_t stMcastAddr;    
    mac_mcast_t stMcastMacEntryTmp;
    
    if((0 != stMacMcast.tdVid) && (!VALID_VLAN_ID(stMacMcast.tdVid)))
    {
        return DRV_ERR_PARA;
    }

    if(!IS_MULTICAST(stMacMcast.mac_addr))
    {
        /*unicast*/
        return DRV_ERR_PARA;
    }
    
	memset(&stMcastAddr, 0, sizeof(stMcastAddr));
    memcpy(stMcastAddr.mac.octet, stMacMcast.mac_addr, sizeof(mac_address_t));
	//stMcastAddr.flags |= RTK_L2_UCAST_FLAG_IVL;
    
    SPIN_HAL_MAC_LOCK;
    if(!ulMacMacstInited)
    {        
        memset(_mac_mcast_entry_dump, 0, sizeof(_mac_mcast_entry_dump));
        ulMacMacstInited = 1;        
    }
    
    //enRet = _Hal_MacMcastGetByVid(stMacMcast.tdVid, &stMcastMacEntryTmp);
    enRet = _Hal_MacMcastGetByMac(stMacMcast.mac_addr, &stMcastMacEntryTmp);

    SPIN_HAL_MAC_UNLOCK;
    /*Find the entry.*/
    if (DRV_OK == enRet)
    {

        diag_printk("\nfunc:%s,line:%d,msk1:0x%04X,msk2:0x%04X\n",__FUNCTION__,__LINE__,
                  stMacMcast.port_mask.pbits[0],stMcastMacEntryTmp.port_mask.pbits[0]);
        
        /*Delete it*/
        /*use svl fid 0 */
        if(RT_ERR_OK != rtk_l2_mcastAddr_del(&stMcastAddr))
        {
            diag_printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return DRV_SDK_GEN_ERROR;
        }

        diag_printk("\nfunc:%s,line:%d,vid:%d,mac:%02X%02X-%02X%02X-%02X%02X\n",
                  __FUNCTION__,__LINE__,stMcastMacEntryTmp.tdVid,
                  stMcastMacEntryTmp.mac_addr[0],stMcastMacEntryTmp.mac_addr[1],
                  stMcastMacEntryTmp.mac_addr[2],stMcastMacEntryTmp.mac_addr[3],
                  stMcastMacEntryTmp.mac_addr[4],stMcastMacEntryTmp.mac_addr[5]);

        SPIN_HAL_MAC_LOCK;
        (void)_Hal_MacMcastDel(&stMcastMacEntryTmp);
        SPIN_HAL_MAC_UNLOCK;

        return DRV_OK;

    }

    return DRV_OK;
}


/*****************************************************************************
	Func Name:	Hal_SetMacAgeTime
  Description:	set age time in seconds, 0 means not age
		Input:	ulSecond
	   Output: 
	   Return:	DRV_ERR_PARA
				DRV_OK
	  Caution: 
------------------------------------------------------------------------------
  Modification History														
  DATE		  NAME			   DESCRIPTION									
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_GetMacAgeTime(UINT32 *pulSecond)
{
	if(RT_ERR_OK != rtk_l2_aging_get(pulSecond))
	{
		return DRV_ERR_PARA;
	}
	
	return DRV_OK;
}


/*****************************************************************************
    Func Name:  Hal_SetMacAgeTime
  Description:  set age time in seconds, 0 means not age
        Input:  ulSecond
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetMacAgeTime(UINT32 ulSecond)
{
    if(RT_ERR_OK != rtk_l2_aging_set(ulSecond))
    {
        return DRV_ERR_PARA;
    }
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  
  Description:  
        Input:  portmask and limit
       Output:  state
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetMacLearnLimit(UINT32 ulLgcPort, UINT32 uLlimit, UINT32 ulDisForward)
{
    rtk_port_t port;
    rtk_l2_limitLearnCntAction_t action;
    UINT32 uiMacCnt = 0;

    if(!VALID_PORT(ulLgcPort))
    {
        return DRV_ERR_PARA;
    }

    port = PortLogic2PhyPortId(ulLgcPort);

    uiMacCnt = uLlimit;
    /*未设置数量时，则设置为最大值*/
    if(DIS_ARL_LEARN_LIMIT == uiMacCnt)
    {
        //uiMacCnt = MAX_ARL_LEARN_LIMIT;
        uiMacCnt = HAL_L2_LEARN_LIMIT_CNT_MAX();
    }
    
    if(RT_ERR_OK != rtk_l2_portLimitLearningCnt_set(port, uiMacCnt))
    {
        return DRV_ERR_UNKNOW;
    }

    if(DISABLE == ulDisForward)
    {
        /*forward*/
        action = LIMIT_LEARN_CNT_ACTION_FORWARD;
    }
    else
    {
        /*drop*/
        action = LIMIT_LEARN_CNT_ACTION_DROP;
    }

    if(RT_ERR_OK != rtk_l2_portLimitLearningCntAction_set(port, action))
    {
        return DRV_ERR_UNKNOW;
    }
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_GetMacUcastMac
  Description:  get unicast mac entry by given vid and mac address
        Input:  ulVid
                mac_addr
       Output:  pstMacMcast
       Return:  DRV_ERR_PARA
                DRV_ERR_MAC_ADDR_NOT_FOUND
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_GetMacUcastMac(vlan_id_t tdVid, mac_address_t mac_addr, mac_ucast_t *pstMacUcast)
{
    UINT32 unit;
    port_num_t lport;
    rtk_l2_ucastAddr_t stUcastAddr;

    if(!VALID_VLAN_ID(tdVid) && (0 != tdVid))
    {
        return DRV_ERR_PARA;
    }
    if(NULL == pstMacUcast)
    {
        return DRV_ERR_PARA;
    }

    memset(&stUcastAddr, 0, sizeof(stUcastAddr));
    memcpy(stUcastAddr.mac.octet, mac_addr, sizeof(mac_address_t));
	//stUcastAddr.flags |= RTK_L2_UCAST_FLAG_IVL;
//    stUcastAddr.vid = tdVid;
	stUcastAddr.fid = 0;
    
    memset(pstMacUcast, 0, sizeof(mac_ucast_t));
	/*Begin modified by sunmingliang for bug 260*/
//    stUcastAddr.flags |= RTK_L2_UCAST_FLAG_IVL;
	/*End modified by sunmingliang for bug 260*/
    memcpy(pstMacUcast->mac_addr, mac_addr, sizeof(mac_address_t));
    pstMacUcast->tdVid = (vlan_id_t)tdVid;
        
    UnitFor(unit)
    {
        if(RT_ERR_OK == rtk_l2_addr_get(&stUcastAddr))
        {
            
            //if(ENABLED == stUcastAddr.da_block)
            if(stUcastAddr.flags & RTK_L2_UCAST_FLAG_DA_BLOCK)
            {
                /*black hole mac address*/
                pstMacUcast->lgcPort = 0;
            }
            else
            {
                /*normal ucast*/
                lport = PortPhyID2Logic(TOPHYID(unit, stUcastAddr.port));
                if(INVALID_PORT == lport)
                {
                    continue;
                }
                else
                {
                    pstMacUcast->lgcPort = lport;
                }
            }

            //if(ENABLED == stUcastAddr.is_static)
            if(stUcastAddr.flags & RTK_L2_UCAST_FLAG_STATIC)
            {
                pstMacUcast->ucStatic = TRUE;
            }
            return DRV_OK;
        }
    }
    return DRV_ERR_MAC_ADDR_NOT_FOUND;
}

/*****************************************************************************
    Func Name:  Hal_GetMacMcastMac
  Description:  get multicast mac address entry
        Input:  ulVid
                mac_addr
       Output:  pstMacMcast
       Return:  DRV_ERR_PARA
                DRV_ERR_MAC_ADDR_NOT_FOUND
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_GetMacMcastMac(vlan_id_t tdVid, mac_address_t mac_addr, mac_mcast_t *pstMacMcast)
{
    UINT32 unit;
    phy_pmask_t stPhyMask;
    UINT8 ucGetMac = 0;
	rtk_l2_mcastAddr_t stMcastAddr;

    if(!VALID_VLAN_ID(tdVid) && (0 != tdVid))
    {
        return DRV_ERR_PARA;
    }
    if(NULL == pstMacMcast)
    {
        return DRV_ERR_PARA;
    }

    pstMacMcast->tdVid = (vlan_id_t)tdVid;    
    memcpy(pstMacMcast->mac_addr,mac_addr,sizeof(mac_address_t));

	memset(&stMcastAddr, 0, sizeof(stMcastAddr));
	memcpy(stMcastAddr.mac.octet, mac_addr, sizeof(mac_address_t));
	
    UnitFor(unit)
    {
        if(RT_ERR_OK == rtk_l2_mcastAddr_get(&stMcastAddr))
        {
            memcpy(&(stPhyMask.pmask[unit]), &(stMcastAddr.portmask), sizeof(rtk_portmask_t));
            ucGetMac = 1;
        }
    }

    if(ucGetMac)
    {
        MaskPhy2Logic(&stPhyMask, &(pstMacMcast->port_mask));
        return DRV_OK;
    }
    else
    {
        return DRV_ERR_MAC_ADDR_NOT_FOUND;
    }
}

/*****************************************************************************
    Func Name:  Hal_SetMacSetMcastMac
  Description:  set exist multicast mac address entry.
                If mac entry does not exist, add new one.
        Input:  stMacMcast
       Output:  
       Return:  DRV_ERR_PARA
                DRV_ERR_MAC_ADDR_NOT_FOUND
                DRV_OK
      Caution:  igmp join and leave must use this API
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetMacSetMcastMac(mac_mcast_t stMacMcast)
{
    BOOL bSamePort;
    UINT32 unit;
    DRV_RET_E enRet;
    phy_pmask_t phy_mask;
    mac_mcast_t stMcastMacEntryTmp;
    rtk_api_ret_t rtkApiRet;
	rtk_l2_mcastAddr_t stMcastAddr;

    MaskLogic2Phy(&(stMacMcast.port_mask), &phy_mask);
    
    /*Check same multicast mac entry.*/
    SPIN_HAL_MAC_LOCK;
    //enRet = _Hal_MacMcastGetByVid(stMacMcast.tdVid, &stMcastMacEntryTmp);
    enRet = _Hal_MacMcastGetByMac(stMacMcast.mac_addr, &stMcastMacEntryTmp);
    SPIN_HAL_MAC_UNLOCK;
    if (DRV_OK == enRet)
    {
        LOGIC_PORT_MASK_CPARE(stMacMcast.port_mask, stMcastMacEntryTmp.port_mask, bSamePort);
        /*Try add the same one.*/
        if (TRUE == bSamePort)
        {
            return DRV_OK;
        }       
    }
    
    memset(&stMcastAddr, 0, sizeof(stMcastAddr));
    memcpy(stMcastAddr.mac.octet, stMacMcast.mac_addr, sizeof(mac_address_t));

    UnitFor(unit)
    {
        memcpy(&stMcastAddr.portmask, &phy_mask.pmask[unit], sizeof(rtk_portmask_t));
        rtkApiRet = rtk_l2_mcastAddr_add(&stMcastAddr);
        if (RT_ERR_OK != rtkApiRet)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }

    SPIN_HAL_MAC_LOCK;
    if(!ulMacMacstInited)
    {        
        memset(_mac_mcast_entry_dump, 0, sizeof(_mac_mcast_entry_dump));
        ulMacMacstInited = 1;        
    }
    SPIN_HAL_MAC_UNLOCK;
    
    /*Renew the existed multicast mac entry.*/
    if (DRV_OK == enRet)
    {
        SPIN_HAL_MAC_LOCK;
        (void)_Hal_MacMcastSet(&stMacMcast);
        SPIN_HAL_MAC_UNLOCK;
    }
    /*Add a new multicast mac entry.*/
    else
    {
        SPIN_HAL_MAC_LOCK;
        (void)_Hal_MacMcastAdd(&stMacMcast);
        SPIN_HAL_MAC_UNLOCK;
    }
    
    return DRV_OK;
}

/* */
VLAN_FID_S m_astVlanFid[APOLLOMP_VLAN_FID_MAX+1];
/*****************************************************************************
    Func Name: _Hal_GetFidByVid
 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_GetFidByVid(UINT32 uiVid, UINT32 *puiFid)
{
    UINT32 i;
    
    if (!VALID_VLAN_ID(uiVid) || (NULL == puiFid)) 
    {
        return DRV_ERR_PARA;
    }

    *puiFid = FID_INVALID_ID;
        
    SPIN_HAL_MAC_LOCK;
    for (i = 0; i <= HAL_VLAN_FID_MAX(); i++)
    {
        if (TRUE == m_astVlanFid[i].bValid)
        {
            if (uiVid == m_astVlanFid[i].uiVid)
            {
                *puiFid = m_astVlanFid[i].uiFid;
                break;
            }
        }
    }
    SPIN_HAL_MAC_UNLOCK;

    return DRV_OK;
}
void Hal_flush_mcvlan_macbyvid(int vid)
{
	mac_mcast_t pstMacMcast;
	rtk_l2_mcastAddr_t stMcastAddr;
	UINT32 uiFid=0;
	while(DRV_OK==_Hal_MacMcastGetByVid(vid, &pstMacMcast))
	{
		(void)_Hal_GetFidByVid(vid, &uiFid);
       
		
		SPIN_HAL_MAC_LOCK;
		memset(&stMcastAddr, 0, sizeof(stMcastAddr));
   	    memcpy(stMcastAddr.mac.octet, pstMacMcast.mac_addr, sizeof(rtk_mac_t));
		stMcastAddr.fid = uiFid;
		rtk_l2_mcastAddr_del(&stMcastAddr);
    	(void)_Hal_MacMcastDel(&pstMacMcast);
		SPIN_HAL_MAC_UNLOCK;
	}
	return;
}

/*****************************************************************************
    Func Name: Hal_SetFidByVid
 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetFidByVid(UINT32 uiVid, UINT32 uiFid, logic_pmask_t stLPortMsk, logic_pmask_t stUntagLPortMsk)
{
    UINT32 i;
    UINT32 uiIndex = FID_INVALID_ID;
    
    if (!VALID_VLAN_ID(uiVid) || (HAL_VLAN_FID_MAX() < uiFid)) 
    {
        return DRV_ERR_PARA;
    }
 
    SPIN_HAL_MAC_LOCK;
    for (i = 0; i <= HAL_VLAN_FID_MAX(); i++)
    {
        if (FALSE == m_astVlanFid[i].bValid)
        {
            if (FID_INVALID_ID == uiIndex)
            {
                uiIndex = i;
            }
        }
        else
        {
            if (uiVid == m_astVlanFid[i].uiVid)
            {
                uiIndex = i;
                break;
            }
        }
    }

    if (FID_INVALID_ID != uiIndex)
    {
        if (TRUE == LgcMaskNotNull(&stLPortMsk))
        {
            m_astVlanFid[uiIndex].bValid = TRUE;
            m_astVlanFid[uiIndex].uiVid  = uiVid;
            m_astVlanFid[uiIndex].uiFid  = uiFid;
            memcpy(&(m_astVlanFid[uiIndex].stLPortMsk), &stLPortMsk, sizeof(logic_pmask_t));
            memcpy(&(m_astVlanFid[uiIndex].stUntagLPortMsk), &stUntagLPortMsk, sizeof(logic_pmask_t));
        }
        else
        {
            memset(&m_astVlanFid[uiIndex], 0, sizeof(m_astVlanFid[uiIndex]));
        }
    }
    SPIN_HAL_MAC_UNLOCK;

    if (FID_INVALID_ID == uiIndex)
    {
        return DRV_ERR_UNKNOW;
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GetMcVlanMem
 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetMcVlanMem(UINT32 uiVid, logic_pmask_t *pstLPortMsk, logic_pmask_t *pstUntagLPortMsk)
{
    UINT32 i;
    UINT32 uiIndex = FID_INVALID_ID;
    
    if (!VALID_VLAN_ID(uiVid) || (NULL == pstLPortMsk) || (NULL == pstUntagLPortMsk)) 
    {
        return DRV_ERR_PARA;
    }

    memset(pstLPortMsk, 0, sizeof(logic_pmask_t));
    memset(pstUntagLPortMsk, 0, sizeof(logic_pmask_t));
    SPIN_HAL_MAC_LOCK;
    for (i = 0; i <= HAL_VLAN_FID_MAX(); i++)
    {
        if (TRUE == m_astVlanFid[i].bValid)
        {
            if (uiVid == m_astVlanFid[i].uiVid)
            {
                uiIndex = i;
                break;
            }
        }
    }

    if (FID_INVALID_ID != uiIndex)
    {
        memcpy(pstLPortMsk, &(m_astVlanFid[uiIndex].stLPortMsk), sizeof(logic_pmask_t));
        memcpy(pstUntagLPortMsk, &(m_astVlanFid[uiIndex].stUntagLPortMsk), sizeof(logic_pmask_t));
    }
    SPIN_HAL_MAC_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GetValideFid
 
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetValideFid(UINT32 uiVid, UINT32 *puiFid)
{
    UINT32 i;
    UINT32 uiFidIdx;
    
    if (!VALID_VLAN_ID(uiVid) || (NULL == puiFid))  
    {
        return DRV_ERR_PARA;
    }

    (void)_Hal_GetFidByVid(uiVid, puiFid);
    if (HAL_VLAN_FID_MAX() >= *puiFid)
    {
        return DRV_OK;
    }
        
    SPIN_HAL_MAC_LOCK;
    for (uiFidIdx = 1; uiFidIdx <= HAL_VLAN_FID_MAX(); uiFidIdx++)
    {
        for (i = 0; i <= HAL_VLAN_FID_MAX(); i++)
        {
            if (TRUE == m_astVlanFid[i].bValid)
            {
                if (uiFidIdx == m_astVlanFid[i].uiFid)
                {
                    break;
                }
            }
        }

        if (HAL_VLAN_FID_MAX() < i)
        {
            *puiFid = uiFidIdx;
            break;
        }
    }
    SPIN_HAL_MAC_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_GetMcMacByMacAndFid
                                            
*****************************************************************************/
DRV_RET_E Hal_GetMcMacByMacAndFid(UINT32 uiVid, mac_address_t mac_addr, mac_mcast_t *pstMacMcast)
{
    UINT32 unit;
    UINT32 uiFid=0;
    phy_pmask_t stPhyMask; 
	rtk_l2_mcastAddr_t stMcastAddr;

    if(!VALID_VLAN_ID(uiVid) && (0 != uiVid))
    {
        return DRV_ERR_PARA;
    }
    if(NULL == pstMacMcast)
    {
        return DRV_ERR_PARA;
    }

    (void)_Hal_GetFidByVid(uiVid, &uiFid);
    if (HAL_VLAN_FID_MAX() < uiFid)
    {
        //printk("\nf:%s,l:%d,fid:%d\n",__func__,__LINE__,uiFid);
        return DRV_ERR_MAC_ADDR_NOT_FOUND;
    }

	memset(&stMcastAddr, 0, sizeof(stMcastAddr));
    memcpy(stMcastAddr.mac.octet, mac_addr, sizeof(mac_address_t));
	stMcastAddr.fid = uiFid;
	
    UnitFor(unit)
    {
        if(RT_ERR_OK != rtk_l2_mcastAddr_get(&stMcastAddr))
        {
            //printk("\nf:%s,l:%d,fid:%d\n",__func__,__LINE__,uiFid);
            return DRV_ERR_MAC_ADDR_NOT_FOUND;
        }
        memcpy(&(stPhyMask.pmask[unit]), &(stMcastAddr.portmask), sizeof(rtk_portmask_t)); 
        #if 0
        printk("\nf:%s,mac:%02X%02X-%02X%02X-%02X%02X, mem mask:0x%04X,fid:%d.\n",__func__,
                 pstMacMcast->mac_addr[0],pstMacMcast->mac_addr[1],pstMacMcast->mac_addr[2],
                 pstMacMcast->mac_addr[3],pstMacMcast->mac_addr[4],pstMacMcast->mac_addr[5],
                 stPhyMask.pmask[unit].bits[0],uiFid);
        #endif

    }

    pstMacMcast->tdVid = (vlan_id_t)uiVid; 
    MaskPhy2Logic(&stPhyMask, &(pstMacMcast->port_mask));
    return DRV_OK;
}

/*****************************************************************************
    Func Name: _Hal_MacMcastGetByMac

  Description: Get mcentry by mac.
        Input: mac_address_t tdMac
               mac_mcast_t *pstMacMcast  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _Hal_MacMcastGetByMacAndVid(mac_address_t tdMac,vlan_id_t tdVid, mac_mcast_t *pstMacMcast)
{
    UINT32 i;

    if ((NULL == tdMac) || (NULL == pstMacMcast))
    {
        return DRV_ERR_UNKNOW;
    }
    
    for(i = 0; i < MAX_MULTICAST_ENTRY; i++)
    {
        
        if(!_Hal_MacCompare(tdMac, _mac_mcast_entry_dump[i].mac_addr) && tdVid == _mac_mcast_entry_dump[i].tdVid)
        {
           // printk("\n_mac_mcast_entry_dump[i].tdVid=%d mac:%02x\n",_mac_mcast_entry_dump[i].tdVid,_mac_mcast_entry_dump[i].mac_addr[5]);
            memcpy(pstMacMcast, &_mac_mcast_entry_dump[i], sizeof(mac_mcast_t));
            return DRV_OK;
        }            
    }
    return DRV_ERR_UNKNOW;
}
/*****************************************************************************
    Func Name: Hal_SetMcMacByMacAndFid
 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetMcMacByMacAndFid(mac_mcast_t stMacMcast)
{
    BOOL bSamePort;
    UINT32 unit;
    UINT32 uiFid;
    DRV_RET_E enRet;
    phy_pmask_t phy_mask,phy_mask_tmp;
    mac_mcast_t stMcastMacEntryTmp;
    rtk_api_ret_t rtkApiRet;
	rtk_l2_mcastAddr_t stMcastAddr;

    MaskLogic2Phy(&(stMacMcast.port_mask), &phy_mask);
    
    /*Check same multicast mac entry.*/
    SPIN_HAL_MAC_LOCK;
    /*014531 */
   //enRet = _Hal_MacMcastGetByMac(stMacMcast.mac_addr, &stMcastMacEntryTmp);
    enRet = _Hal_MacMcastGetByMacAndVid(stMacMcast.mac_addr,stMacMcast.tdVid, &stMcastMacEntryTmp);
   // printk("\n*****enRet=%d\n",enRet);
    SPIN_HAL_MAC_UNLOCK;
    if (DRV_OK == enRet)
    {
        LOGIC_PORT_MASK_CPARE(stMacMcast.port_mask, stMcastMacEntryTmp.port_mask, bSamePort);
        /*Try add the same one.*/
        if (TRUE == bSamePort)
        {
            return DRV_OK;
        }

        MaskLogic2Phy(&(stMcastMacEntryTmp.port_mask), &phy_mask_tmp);
        /*Add new ports to existed port mask.*/
        PhyMaskOr(&phy_mask, &phy_mask_tmp);
    }

    (void)Hal_GetValideFid(stMacMcast.tdVid, &uiFid);
    if (HAL_VLAN_FID_MAX() < uiFid)
    {
        return DRV_SDK_GEN_ERROR;
    }
    
    memset(&stMcastAddr, 0, sizeof(stMcastAddr));
    memcpy(stMcastAddr.mac.octet, stMacMcast.mac_addr, sizeof(mac_address_t));
	stMcastAddr.fid = uiFid;

    UnitFor(unit)
    {
        memcpy(&stMcastAddr.portmask, &phy_mask.pmask[unit], sizeof(rtk_portmask_t));
        rtkApiRet = rtk_l2_mcastAddr_add(&stMcastAddr);
        if (RT_ERR_OK != rtkApiRet)
        {
            return DRV_SDK_GEN_ERROR;
        }
        #if 0
        printk("\nf:%s,mac:%02X%02X-%02X%02X-%02X%02X, mem mask:0x%04X,fid:%d.\n",__func__,
                 stMacAddr.octet[0],stMacAddr.octet[1],stMacAddr.octet[2],
                 stMacAddr.octet[3],stMacAddr.octet[4],stMacAddr.octet[5],
                 stPortMask.bits[0],uiFid);
        #endif

    }

    SPIN_HAL_MAC_LOCK;
    if(!ulMacMacstInited)
    {        
        memset(_mac_mcast_entry_dump, 0, sizeof(_mac_mcast_entry_dump));
        ulMacMacstInited = 1;        
    }
    SPIN_HAL_MAC_UNLOCK;
    
    /*Renew the existed multicast mac entry.*/
    if (DRV_OK == enRet)
    {
        SPIN_HAL_MAC_LOCK;
        (void)_Hal_MacMcastSet(&stMacMcast);
        SPIN_HAL_MAC_UNLOCK;
    }
    /*Add a new multicast mac entry.*/
    else
    {
        SPIN_HAL_MAC_LOCK;
        (void)_Hal_MacMcastAdd(&stMacMcast);
        SPIN_HAL_MAC_UNLOCK;
    }
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_DelMcMacByMacAndFid
 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_DelMcMacByMacAndFid(mac_mcast_t stMacMcast)
{
    BOOL bSamePort;
    UINT32 unit;
    DRV_RET_E enRet;
    rtk_api_ret_t rtkApiRet;
    phy_pmask_t phy_mask,phy_mask_tmp;
    mac_mcast_t stMcastMacEntryTmp;
	rtk_l2_mcastAddr_t stMcastAddr;

    if(!VALID_VLAN_ID(stMacMcast.tdVid))
    {
        return DRV_ERR_PARA;
    }

    if(!IS_MULTICAST(stMacMcast.mac_addr))
    {
        /*unicast*/
        return DRV_ERR_PARA;
    }

	memset(&stMcastAddr, 0, sizeof(stMcastAddr));
    memcpy(stMcastAddr.mac.octet, stMacMcast.mac_addr, sizeof(rtk_mac_t));
    
    SPIN_HAL_MAC_LOCK;
    if(!ulMacMacstInited)
    {        
        memset(_mac_mcast_entry_dump, 0, sizeof(_mac_mcast_entry_dump));
        ulMacMacstInited = 1;        
    }
    /* 014041 */
    
     enRet = _Hal_MacMcastGetByMacAndVid(stMacMcast.mac_addr,stMacMcast.tdVid, &stMcastMacEntryTmp);
    /* */
    SPIN_HAL_MAC_UNLOCK;
    /*Find the entry.*/
    if (DRV_OK == enRet)
    {
        UINT32 uiFid;
        
        LOGIC_PORT_MASK_CPARE(stMacMcast.port_mask, stMcastMacEntryTmp.port_mask, bSamePort);

        (void)_Hal_GetFidByVid(stMacMcast.tdVid, &uiFid);
        if (HAL_VLAN_FID_MAX() < uiFid)
        {
            return DRV_ERR_MAC_ADDR_NOT_FOUND;
        }

		stMcastAddr.fid = uiFid;
		
        /*Delete it*/
        if (TRUE == bSamePort)
        {        					
            if(RT_ERR_OK != rtk_l2_mcastAddr_del(&stMcastAddr))
            {
                diag_printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
                return DRV_SDK_GEN_ERROR;
            }
        #if 0
        printk("\nf:%s,mac:%02X%02X-%02X%02X-%02X%02X, fid:%d.\n",__func__,
                 stMacAddr.octet[0],stMacAddr.octet[1],stMacAddr.octet[2],
                 stMacAddr.octet[3],stMacAddr.octet[4],stMacAddr.octet[5],
                 uiFid);
        #endif

            SPIN_HAL_MAC_LOCK;
            (void)_Hal_MacMcastDel(&stMcastMacEntryTmp);
            SPIN_HAL_MAC_UNLOCK;
            
            return DRV_OK;
        }
        /*Renew the existed entry.*/
        else
        {
            MaskLogic2Phy(&(stMacMcast.port_mask), &phy_mask);
            MaskLogic2Phy(&(stMcastMacEntryTmp.port_mask), &phy_mask_tmp);
            /*Delete ports from existed port mask.*/
            PhyMaskRemove(&phy_mask_tmp, &phy_mask);

            UnitFor(unit)
            {
                memcpy(&stMcastAddr.portmask, &phy_mask_tmp.pmask[unit], sizeof(rtk_portmask_t));
                rtkApiRet = rtk_l2_mcastAddr_add(&stMcastAddr);
                if (RT_ERR_OK != rtkApiRet)
                {
                    diag_printk("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
                    return DRV_SDK_GEN_ERROR;
                }
        #if 0
        printk("\nf:%s,mac:%02X%02X-%02X%02X-%02X%02X, mem mask:0x%04X,fid:%d.\n",__func__,
                 stMacAddr.octet[0],stMacAddr.octet[1],stMacAddr.octet[2],
                 stMacAddr.octet[3],stMacAddr.octet[4],stMacAddr.octet[5],
                 stPortMask.bits[0],uiFid);
        #endif
            }

            LgcMaskRemove(&(stMcastMacEntryTmp.port_mask), &(stMacMcast.port_mask));
            SPIN_HAL_MAC_LOCK;
            (void)_Hal_MacMcastSet(&stMcastMacEntryTmp);
            SPIN_HAL_MAC_UNLOCK;
        }
    }
    
    return DRV_OK;
}
/* 013987 */

/*****************************************************************************
    Func Name:  Hal_SetMacLookFailFrd
  Description:  set lookup failed forward port mask
        Input:  stPortMask
       Output:  
       Return:  DRV_ERR_UNKNOW
      Caution:  
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetMacLookFailFrd(logic_pmask_t stPortMask)
{
    #if 0
    UINT32 unit;
    phy_pmask_t stPhyMask;

    MaskLogic2Phy(&stPortMask, &stPhyMask);
    UnitFor(unit)
    {        
        if(RT_ERR_OK != rtk_l2_lookupMissFloodPortMask_set(unit, &CHIPNMASK(unit, &stPhyMask)))
        {
            /*RTL8328L/S/M not support*/
            return DRV_ERR_UNKNOW;
        }
    }

    /*RTL8389 support*/
    return DRV_OK;
    #else
    /*For lint*/
    memcpy(&stPortMask, &stPortMask, sizeof(stPortMask));
    return DRV_ERR_PARA;
    #endif
}

/*****************************************************************************
    Func Name:  Hal_GetMacLearnCount
  Description:  get dynamic learning count by port
        Input:  ulLgcPort
       Output:  pulCount
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_GetMacLearnCount(UINT32 ulLgcPort, UINT32 *pulCount)
{    
    if(!VALID_PORT(ulLgcPort))
    {
        return DRV_ERR_PARA;
    }

    if(NULL == pulCount)
    {
        return DRV_ERR_PARA;
    }

    (void)rtk_l2_portLearningCnt_get(PortLogic2PhyPortId(ulLgcPort), pulCount);

    return DRV_OK;
}

/*****************************************************************************
    Func Name:  _Hal_MacCompare
  Description:  compare mac address
        Input:  address_1
                address_2                
       Output:  
       Return:  0: means the same
                1: means different
      Caution:  
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static UINT32 _Hal_MacCompare(mac_address_t address_1, mac_address_t address_2)
{
    UINT32 i;

    for(i = 0; i< MAC_ADDR_LEN; i++)
    {
        if(address_1[i] != address_2[i])
        {
            break;
        }
    }

    if(MAC_ADDR_LEN == i)
    {
        return 0;
    }

    return 1;
}

/*****************************************************************************
    Func Name:  _Hal_InsertUcastMacEntry
  Description:  sort unicast dump link
        Input:  ulUnit
                ulIndex
                pstMacDump
       Output: 
       Return:  
      Caution:  should check argument first
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
static void _Hal_InsertUcastMacEntry(UINT32 ulUnit, UINT32 ulIndex)
{
    mac_dump_ucast_link_t *pstDumpLinkCur;
    mac_dump_ucast_tab_t *pstDumpTabCur;
    mac_dump_ucast_tab_t *pstMacDump;
    mac_dump_ucast_link_t *pstDumpLinkInsert;
    UINT32 i;
    
    /*do not need to check para, optimized*/
    pstDumpLinkInsert = &_mac_ucast_dump_link[ulUnit][ulIndex];
    pstMacDump = &_mac_ucast_entry_dump[ulUnit][ulIndex];
    pstDumpLinkInsert->pstMacDump = pstMacDump;

    /*table sort*/
    if(NULL != pstMacUcastDumpLinkHead)
    {
        pstDumpLinkCur = pstMacUcastDumpLinkHead;
        
        while(NULL != pstDumpLinkCur)
        {
            pstDumpTabCur = pstDumpLinkCur->pstMacDump;

            /*skip invalid entry*/
            if(!(pstDumpTabCur->flag & DRV_MAC_DUMP_FLAG_VALID))
            {
                /*get next*/
                pstDumpLinkCur = pstDumpLinkCur->pstMacDumpLinkNext;
                continue;
            }

            /*find first different byte*/
            for(i = 0; i< MAC_ADDR_LEN; i++)
            {
                if(pstDumpTabCur->mac_addr[i] != pstMacDump->mac_addr[i])
                {
                    break;
                }
            }

            if(MAC_ADDR_LEN == i)
            {
                /*found same mac, insert pre*/
                DUMP_LINK_INSERT_PRE(pstDumpLinkCur, pstDumpLinkInsert);
                return;
            }
            else if(pstDumpTabCur->mac_addr[i] > pstMacDump->mac_addr[i])
            {
                /*insert pre*/
                DUMP_LINK_INSERT_PRE(pstDumpLinkCur, pstDumpLinkInsert);
                return;
            }
            else
            {
                /*check if we got end*/
                if(NULL == pstDumpLinkCur->pstMacDumpLinkNext)
                {
                    /*end, insert tail*/
                    pstDumpLinkCur->pstMacDumpLinkNext = pstDumpLinkInsert;
                    pstDumpLinkInsert->pstMacDumpLinkPre = pstDumpLinkCur;
                    pstDumpLinkInsert->pstMacDumpLinkNext = NULL;
                    return;
                }
                /*get next*/
                pstDumpLinkCur = pstDumpLinkCur->pstMacDumpLinkNext;
            }
        }
    }
    else
    {
        /*first link*/
        pstMacUcastDumpLinkHead = pstDumpLinkInsert;
        pstMacUcastDumpLinkHead->pstMacDump = pstMacDump;
        pstMacUcastDumpLinkHead->pstMacDumpLinkPre = NULL;
        pstMacUcastDumpLinkHead->pstMacDumpLinkNext = NULL;
    }
}

/*****************************************************************************
    Func Name:  _Hal_UcastCompare
  Description:  unicast entry compare (pstRtkUcast pstDumpTab)
        Input:  ulUnit
                pstRtkUcast
                pstDumpTab                
       Output:  pulRefreshOnly, 0 means need remove and add, 1 means need refresh only
       Return:  0: means the same
                1: means different
      Caution:  
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
static UINT32 _Hal_UcastCompare(UINT32 ulUnit, rtk_l2_ucastAddr_t *pstRtkUcast, mac_dump_ucast_tab_t *pstDumpTab, UINT32 *pulRefreshOnly)
{
    UINT8 ucIsAggr = 0, ucIsAggrPre = 0;
    vlan_id_t tdVid;
    UINT8 ucIsStatic = 0, ucIsStaticPre = 0;
    
    /*do not need to check para, optimized*/

    *pulRefreshOnly = 1;
    /*compare vid*/
    tdVid = (vlan_id_t)pstRtkUcast->vid;
    if(tdVid != pstDumpTab->vid)
    {
        return 1;
    }

    /*compare mac address*/
    if(_Hal_MacCompare(pstRtkUcast->mac.octet, pstDumpTab->mac_addr))
    {
        *pulRefreshOnly = 0;
        return 1;
    }    

    /*compare static*/
    //if(ENABLED == pstRtkUcast->is_static)
    if(pstRtkUcast->flags & RTK_L2_UCAST_FLAG_STATIC)
    {
        ucIsStatic = 1;
    }
    if(pstDumpTab->flag & DRV_MAC_DUMP_FLAG_STATIC)
    {
        ucIsStaticPre = 1;
    }
    if(ucIsStatic != ucIsStaticPre)
    {
        return 1;
    }
    
    if(pstDumpTab->flag & DRV_MAC_DUMP_FLAG_AGGR)
    {
        ucIsAggrPre = 1;
    }
    if(ucIsAggr != ucIsAggrPre)
    {
        return 1;
    }

    /*compare blackhole*/
    if(0 != pstDumpTab->lport)
    {
        //if(ENABLED == pstRtkUcast->da_block)
        if(pstRtkUcast->flags & RTK_L2_UCAST_FLAG_DA_BLOCK)
        {
            return 1;
        }
    }

    /*compare port number*/
    if(!ucIsAggr)
    {
        if(pstDumpTab->lport != PortPhyID2Logic(TOPHYID(ulUnit, pstRtkUcast->port)))
        {
            return 1;
        }
    }
    
    return 0;
}

/*****************************************************************************
    Func Name: _HAL_GetBitNum
  Description: Get bit id from mask
        Input: UINT32 uiMask    
                UINT32 *pBitNum  
       Output: 
       Return: static
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
static DRV_RET_E _HAL_GetBitNum(UINT32 uiMask, UINT32 *pBitNum)
{
    UINT32 i;
    UINT32 uiBitNum = 8 * sizeof(uiMask);

    if (NULL == pBitNum)
    {
        return DRV_ERR_PARA;
    }

    *pBitNum = 0xFFFFFFFF;
    for (i = 0; i < uiBitNum; i++)
    {
        if (uiMask & (1U << i))
        {
            *pBitNum = i;
            break;
        }
    }

    return DRV_OK;
}

#if 0
/*****************************************************************************
    Func Name:  _Hal_DumpUCastMacEntry
  Description:  dump all ucast mac entry
        Input:  ulUnit
                ulStartIndex
                ulEndIndex
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution:  UNICAST: system will auto learn all unicast mac address on stack
                port, so we only need to dump all mac address with all logic port;
                MULTICAST: all multicast mac address should be added by software,
                so we do not need to dump multicast mac address from register, but
                we need to insert multicast operation in Hal_SetMacAddMcastMac()
                Hal_SetMacDelMcastMac() and Hal_SetMacSetMcastMac()
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
static DRV_RET_E _Hal_DumpUCastMacEntry(UINT32 ulUnit, UINT32 ulStartIndex, UINT32 ulEndIndex)
{
    UINT32 ulIndex;
    mac_dump_ucast_tab_t *pstMacDump;
    UINT32 ulRefreshOnly = 0;
    rtk_l2_addr_table_t stRtkMacEntry;
    rtk_l2_ucastAddr_t stRtkMac;

    if(ulEndIndex <= ulStartIndex)
    {
        return DRV_ERR_PARA;
    }

    if(ulStartIndex >= MAX_ARL_TABLE)
    {
        return DRV_ERR_PARA;
    }

    if(ulEndIndex > MAX_ARL_TABLE)
    {
        ulEndIndex = MAX_ARL_TABLE;
    }

    if(ulUnit >= CHIP_NO)
    {
        return DRV_ERR_PARA;
    }
    
    for(ulIndex = ulStartIndex; ulIndex < ulEndIndex; ulIndex++)
    {        
        if(RT_ERR_OK == rtk_l2_nextValidEntry_get(&ulIndex, &stRtkMacEntry))
        {                            
            /*unicast*/
            if (0 == (stRtkMacEntry.entry.l2UcEntry.mac.octet[0] & 0x01))
            {
                SPIN_HAL_MAC_LOCK;
                ulRefreshOnly = 0;
                pstMacDump = &(_mac_ucast_entry_dump[ulUnit][ulIndex]);

                /*compare legacy if it is valid*/
                if(pstMacDump->flag & DRV_MAC_DUMP_FLAG_VALID)
                {
                    memset(&stRtkMac, 0, sizeof(stRtkMac));
                    memcpy(&stRtkMac.mac, &stRtkMacEntry.entry.l2UcEntry.mac, sizeof(stRtkMac.mac));
                    stRtkMac.vid      = stRtkMacEntry.entry.l2UcEntry.vid;
                    stRtkMac.fid       = stRtkMacEntry.entry.l2UcEntry.fid;

                    //if (0 == stRtkMacEntry.portmask)
                    if(stRtkMacEntry.entry.l2UcEntry.flags & RTK_L2_UCAST_FLAG_DA_BLOCK)
                    {
                        stRtkMac.da_block  = ENABLED;
                    }
                    else
                    {
                        //(void)_HAL_GetBitNum(stRtkMacEntry.portmask, &stRtkMac.port);
                        stRtkMac.port = stRtkMacEntry.entry.l2UcEntry.port;
                        stRtkMac.da_block  = DISABLED;
                    }
                    
                    /*compare, remove current if different*/
                    if(_Hal_UcastCompare(ulUnit, &stRtkMac, pstMacDump, &ulRefreshOnly))
                    {
                        if(!ulRefreshOnly)
                        {
                            /*remove current*/
                            DUMP_LINK_REMOVE(ulUnit, ulIndex);
                        }
                    }
                    else
                    {
                        SPIN_HAL_MAC_UNLOCK;
                        break;
                    }
                }
                else
                {
                    /*clear invalid entry*/
                    pstMacDump->flag = 0;
                }

                /*start to fill in _mac_ucast_entry_dump[ulUnit][ulIndex]*/
                //if(0 == stRtkMacEntry.portmask)
                if(stRtkMacEntry.entry.l2UcEntry.flags & RTK_L2_UCAST_FLAG_DA_BLOCK)
                {
                    /*blackhole*/
                    pstMacDump->lport = 0;
                    pstMacDump->flag = DRV_MAC_DUMP_FLAG_VALID;
                }
                else
                {
                    UINT32 uiPhyId;
                    //(void)_HAL_GetBitNum(stRtkMacEntry.portmask, &uiPhyId);
                    pstMacDump->lport = PortPhyID2Logic(uiPhyId);                            
                    if((INVALID_PORT != pstMacDump->lport) && (LOGIC_CPU_PORT != pstMacDump->lport))
                    {
                        pstMacDump->flag = DRV_MAC_DUMP_FLAG_VALID;
                    }
                    else
                    {
                        if(pstMacDump->flag & DRV_MAC_DUMP_FLAG_VALID)
                        {
                            DUMP_LINK_REMOVE(ulUnit, ulIndex);
                            pstMacDump->flag = 0;
                        }
                        /*skip cpu port and stack port*/
                        SPIN_HAL_MAC_UNLOCK;

                        break;
                    }
                }

                //if(ENABLED == stRtkMacEntry.is_static)
                if(stRtkMacEntry.entry.l2UcEntry.flags & RTK_L2_UCAST_FLAG_STATIC)
                {
                    /*static*/
                    pstMacDump->flag |= DRV_MAC_DUMP_FLAG_STATIC;
                }

                memcpy(pstMacDump->mac_addr, stRtkMacEntry.entry.l2UcEntry.mac.octet, sizeof(mac_address_t));
                pstMacDump->vid = (vlan_id_t)stRtkMacEntry.entry.l2UcEntry.vid;
                if(!ulRefreshOnly)
                {
                    /*insert if need*/
                    _Hal_InsertUcastMacEntry(ulUnit, ulIndex);
                }
                
                SPIN_HAL_MAC_UNLOCK;
                
            }
            else
            {
                /*remove current*/
                SPIN_HAL_MAC_LOCK;
                DUMP_LINK_REMOVE(ulUnit, ulIndex);
                SPIN_HAL_MAC_UNLOCK;
                break;
            }
        }
        else
        {
            /*remove current*/
            SPIN_HAL_MAC_LOCK;
            DUMP_LINK_REMOVE(ulUnit, ulIndex);
            SPIN_HAL_MAC_UNLOCK;
        }
    }

    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_RefreshUCast
  Description:  dump unicast, sugget to call this function every 200ms
        Input:  ulInterval               
       Output:  
       Return:  DRV_OK
      Caution:  this function is not locked, should be called from only one thread
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_RefreshUCast(UINT32 ulInterval)
{
    UINT32 ulUnit;


    if(ulInterval > MAX_ARL_TABLE)
    {
        ulInterval = MAX_ARL_TABLE;
    }

    if(!ulInterval)
    {
        ulInterval = DRV_MAC_DUMP_NUMBER_PER_ROLL;
    }
    if(!ulMacDumpInited)
    {
        memset(_mac_ucast_dump_link, 0, sizeof(_mac_ucast_dump_link));
        memset(_mac_ucast_entry_dump, 0, sizeof(_mac_ucast_dump_link));
        pstMacUcastDumpLinkHead = NULL;        
        ulMacDumpInited = 1;
    }

    UnitFor(ulUnit)
    {
        (void)_Hal_DumpUCastMacEntry(ulUnit, ulMacCurrentIndex, ulMacCurrentIndex+ulInterval);
        ulMacCurrentIndex += ulInterval;
        if(ulMacCurrentIndex >= MAX_ARL_TABLE)
        {
            /*return back*/
            ulMacCurrentIndex = 0;
        }
    }

    return DRV_OK;
}
#endif
/*userPointer must be implemented by unsigned long type, long is 8 bytes in 64bit-system*/
/*****************************************************************************
    Func Name:  Hal_GetMacUcastDump
  Description:  dump unicast from kernel to user
        Input:  userPointer, user memory pointer   
                UINT32 ulPDataType -- 地址空间(ADDRESS_IN_USER_SPACE／ADDRESS_IN_KERNEL_SPACE)
       Output:  pulCountOut
       Return:  DRV_OK
                DRV_ERR_PARA
      Caution:  
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_GetMacUcastDump(ULONG userPointer, UINT32 *pulCountOut, UINT32 ulPDataType)
{
    mac_dump_ucast_tab_t *pstUserUcastDump;
    UINT32 ulIndex, count = 0, unit;
    mac_dump_ucast_tab_t stUcastDumpTmp;
    UINT32 uiPhyId;
    rtk_l2_addr_table_t stMacEntry;
    UINT32 addrNum = 0;
    UINT32 pre_addrNum = 0;
    
    if(NULL == pulCountOut)
    {
        return DRV_ERR_PARA;
    }

    memset(&stUcastDumpTmp, 0, sizeof(stUcastDumpTmp));
    memset(&stMacEntry, 0, sizeof(stMacEntry));

    /*user memory pointer*/
    pstUserUcastDump = (mac_dump_ucast_tab_t *)userPointer;
    
    *pulCountOut = 0;

    UnitFor(unit)
    {
        //for(ulIndex = 0; ulIndex < MAX_ARL_TABLE; ulIndex++)
        for(ulIndex = 0; ulIndex < 2048; ulIndex++)  //test 
        {
            /*14010 */
            extern void msleep(unsigned int msecs);
            if(ulIndex % 50 == 0)
            {
                msleep(10);
            }
             /* 14010 */
            //stMacEntry.index = (rtk_int32)ulIndex;
            //if (RT_ERR_OK == rtk_l2_entry_get(&stMacEntry))
            stMacEntry.entry.l2UcEntry.index = (rtk_int32)addrNum;
            pre_addrNum = addrNum;
            if(RT_ERR_OK == rtk_l2_nextValidEntry_get(&addrNum, &stMacEntry))
            {       
                //if(0 == (stMacEntry.mac.octet[0] & 0x01)) 
                if(0 == (stMacEntry.entry.l2UcEntry.mac.octet[0] & 0x01))
                {
                    /*blackhole mac*/
                    /* 014049 */
                    //if( ENABLED == stMacEntry.da_block)
                    if(stMacEntry.entry.l2UcEntry.flags & RTK_L2_UCAST_FLAG_DA_BLOCK)
                    {
                        stUcastDumpTmp.lport = 0;
                        stUcastDumpTmp.flag = DRV_MAC_DUMP_FLAG_VALID;
                    }
                    else
                    {
                        //(void)_HAL_GetBitNum(stMacEntry.portmask, &uiPhyId);
                        uiPhyId = stMacEntry.entry.l2UcEntry.port;
                        if (!IsValidPhyID(uiPhyId))
                        {
                            return DRV_ERR_UNKNOW;
                        }
                        stUcastDumpTmp.lport = PortPhyID2Logic(uiPhyId);                            
                        if (INVALID_PORT != stUcastDumpTmp.lport)
                        {
                            stUcastDumpTmp.flag = DRV_MAC_DUMP_FLAG_VALID;
                        }
                        else
                        {
                            /*skip stack and cpu port*/
                            continue;
                        }
                    }
                
                    /*static mac*/
                    //if(ENABLED == stMacEntry.is_static)
                    if(stMacEntry.entry.l2UcEntry.flags & RTK_L2_UCAST_FLAG_STATIC)
                    {
                        stUcastDumpTmp.flag |= DRV_MAC_DUMP_FLAG_STATIC;
                    }

                    memcpy(stUcastDumpTmp.mac_addr, stMacEntry.entry.l2UcEntry.mac.octet, sizeof(mac_address_t));
                    stUcastDumpTmp.vid = (UINT16)stMacEntry.entry.l2UcEntry.vid;
                
                    /*Copy mac to user space*/
                    if(ADDRESS_IN_USER_SPACE == ulPDataType)
                    {
                        copy_to_user(pstUserUcastDump, &stUcastDumpTmp, sizeof(mac_dump_ucast_tab_t));
                    }
                    /*Copy mac to kernel space*/
                    else
                    {
                        (void)memcpy(pstUserUcastDump, &stUcastDumpTmp, sizeof(mac_dump_ucast_tab_t));
                    }
                
                    pstUserUcastDump++;
                    count++;
                }

                if(addrNum < pre_addrNum)
                {
                    break;
                }
                addrNum++;
            }
            else
            {
                break;
            }
        }
    }
    *pulCountOut = count;
    
    return DRV_OK;
}
#if 0
/*****************************************************************************
    Func Name:  Hal_GetMacNextValidUcast
  Description:  get valid unicast entry
        Input:  uiStartIndex: 0 ~ (MAX_ARL_TABLE_INDEX-1)
       Output:  pstUcastEntry
                puiCurrentIndex: current valid entry index
       Return:  DRV_OK
                DRV_ERR_PARA
                DRV_ERR_MAC_ADDR_NOT_FOUND
      Caution:  
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_GetMacNextValidUcast(UINT32 uiStartIndex, mac_dump_ucast_tab_t* pstUcastEntry, UINT32 *puiCurrentIndex)
{
    UINT32 ulIndex, unit;
    UINT32 uiPhyId;
    rtk_l2_addr_table_t stMacEntry;

    if(uiStartIndex >= MAX_ARL_TABLE_INDEX)
    {
        return DRV_ERR_PARA;
    }

    memset(&stMacEntry, 0, sizeof(stMacEntry));

    for(ulIndex = uiStartIndex; ulIndex < MAX_ARL_TABLE_INDEX; ulIndex++)
    {            
        unit = ulIndex/MAX_ARL_TABLE;
        stMacEntry.index = (rtk_int32)ulIndex;
        if (RT_ERR_OK == rtk_l2_entry_get(&stMacEntry))
        {
            if(0 == (stMacEntry.mac.octet[0] & 0x01))
            {
                /*blackhole*/
                if(ENABLED == stMacEntry.sa_block)
                {
                    /*blackhole*/
                    pstUcastEntry->lport = 0;
                    pstUcastEntry->flag = DRV_MAC_DUMP_FLAG_VALID;
                }
                else
                {
                    (void)_HAL_GetBitNum(stMacEntry.portmask, &uiPhyId);
                    if (!IsValidPhyID(uiPhyId))
                    {
                        return DRV_ERR_UNKNOW;
                    }
                
                    pstUcastEntry->lport = PortPhyID2Logic(uiPhyId);                            
                    if((INVALID_PORT != pstUcastEntry->lport) && (LOGIC_CPU_PORT != pstUcastEntry->lport))
                    {
                        pstUcastEntry->flag = DRV_MAC_DUMP_FLAG_VALID;
                    }
                    else
                    {
                        /*skip stack and cpu port*/
                        continue;
                    }
                }

                if(ENABLED == stMacEntry.is_static)
                {
                    /*static*/
                    pstUcastEntry->flag |= DRV_MAC_DUMP_FLAG_STATIC;
                }

                memcpy(pstUcastEntry->mac_addr, stMacEntry.mac.octet, sizeof(mac_address_t));
                pstUcastEntry->vid = (UINT16)stMacEntry.cvid;
                *puiCurrentIndex = ulIndex;

                return DRV_OK;
            }
        }
    }
    
    return DRV_ERR_MAC_ADDR_NOT_FOUND;
}
#endif

/*****************************************************************************
    Func Name:  Hal_GetMacMcastDump
  Description:  dump multicast from kernel to user
        Input:  userPointer, user memory pointer    
                UINT32 ulPDataType -- 地址空间(ADDRESS_IN_USER_SPACE／ADDRESS_IN_KERNEL_SPACE)
       Output:  pulCountOut
       Return:  DRV_OK
                DRV_ERR_PARA
      Caution:  
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetMacMcastDump(ULONG userPointer, UINT32 *pulCountOut, UINT32 ulPDataType)
{
    mac_mcast_t *pstUserMacMcast;
    
    if(NULL == pulCountOut)
    {
        return DRV_ERR_PARA;
    }
    pstUserMacMcast = (mac_mcast_t *)userPointer;

    SPIN_HAL_MAC_LOCK;
    if(ulMacMcastCount)
    {
        if(ADDRESS_IN_USER_SPACE == ulPDataType)
        {
            /*copy to user memory space*/
            copy_to_user(pstUserMacMcast, _mac_mcast_entry_dump, sizeof(mac_mcast_t)*MAX_MULTICAST_ENTRY);
        }
        else
        {
            (void)memcpy(pstUserMacMcast, _mac_mcast_entry_dump, sizeof(mac_mcast_t)*MAX_MULTICAST_ENTRY);
        }
    }
    *pulCountOut = ulMacMcastCount;
    SPIN_HAL_MAC_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_GetMacNextValidMcast
  Description:  get valid multicast entry
        Input:  uiStartIndex: 0 ~ (MAX_MULTICAST_ENTRY-1)
       Output:  pstMcastEntry
                puiCurrentIndex: current valid entry index
       Return:  DRV_OK
                DRV_ERR_PARA
                DRV_ERR_MAC_ADDR_NOT_FOUND
      Caution:  
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetMacNextValidMcast(UINT32 uiStartIndex, mac_mcast_t* pstMcastEntry, UINT32 *puiCurrentIndex)
{
    UINT32 uiIndex;
    
    if(NULL == pstMcastEntry)
    {
        return DRV_ERR_PARA;
    }

    if(NULL == puiCurrentIndex)
    {
        return DRV_ERR_PARA;
    }

    if(uiStartIndex >= MAX_MULTICAST_ENTRY)
    {
        return DRV_ERR_PARA;
    }

    if(0 == ulMacMcastCount)
    {
        return DRV_ERR_MAC_ADDR_NOT_FOUND;
    }

    for(uiIndex = uiStartIndex; uiStartIndex < MAX_MULTICAST_ENTRY; uiStartIndex++)
    {
        SPIN_HAL_MAC_LOCK;
        if(0 != _mac_mcast_entry_dump[uiIndex].tdVid)
        {
            memcpy(pstMcastEntry, &_mac_mcast_entry_dump[uiIndex], sizeof(mac_mcast_t));
            *puiCurrentIndex = uiIndex;
            SPIN_HAL_MAC_UNLOCK;
            return DRV_OK;
        }
        SPIN_HAL_MAC_UNLOCK;
    }

    return DRV_ERR_MAC_ADDR_NOT_FOUND;
}


/*****************************************************************************
    Func Name:  Hal_GetMacUcastDumpSorted
  Description:  dump sorted unicast from kernel to user
        Input:  userPointer, user memory pointer      
                UINT32 ulPDataType -- 地址空间(ADDRESS_IN_USER_SPACE／ADDRESS_IN_KERNEL_SPACE)
       Output:  pulCountOut
       Return:  DRV_OK
                DRV_ERR_PARA
      Caution:  
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetMacUcastDumpSorted(ULONG userPointer, UINT32 *pulCountOut, UINT32 ulPDataType)
{
    mac_dump_ucast_tab_t *pstUserUcastDump;
    mac_dump_ucast_tab_t *pstUcastDumpSort;
    mac_dump_ucast_link_t *pstMacDumpLint;
    UINT32 i, count = 0;
    
    if(NULL == pulCountOut)
    {
        return DRV_ERR_PARA;
    }

    /*user memory pointer*/
    pstUserUcastDump = (mac_dump_ucast_tab_t *)userPointer;
    
    pstUcastDumpSort = &(_mac_ucast_entry_dump_sorted[0]);
    *pulCountOut = 0;
    
    SPIN_HAL_MAC_LOCK;
    pstMacDumpLint = pstMacUcastDumpLinkHead;
    for(i = 0; i < MAX_ARL_TABLE; i++)
    {
        if(NULL != pstMacDumpLint)
        {
            if(NULL != pstMacDumpLint->pstMacDump)
            {
                memcpy(pstUcastDumpSort, pstMacDumpLint->pstMacDump, sizeof(mac_dump_ucast_tab_t));
                count++;
                /*next memory space*/
                pstUcastDumpSort++;
            }
            pstMacDumpLint = pstMacDumpLint->pstMacDumpLinkNext; 
        }
        else
        {
            *pulCountOut = count;
            break;
        }
    }

    if(ADDRESS_IN_USER_SPACE == ulPDataType)
    {
        /*copy to user memory space*/
        (void)copy_to_user(pstUserUcastDump, &(_mac_ucast_entry_dump_sorted[0]), sizeof(mac_dump_ucast_tab_t)*count);
    }
    else
    {
        (void)memcpy(pstUserUcastDump, &(_mac_ucast_entry_dump_sorted[0]), sizeof(mac_dump_ucast_tab_t)*count);
    }

    SPIN_HAL_MAC_UNLOCK;
    *pulCountOut = count;
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetMacTrapToCpu
  Description:  set packet trap to cpu
        Input:  lport, 0 for all port
                frameType
                priority, 0~7
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetMacTrapToCpu(port_num_t lport, mac_trap_set_t stTrapSet, UINT32 priority)
{
    port_num_t lgcPort;
    rtk_api_ret_t tdRtkRet;
    rtk_trap_igmpMld_type_t enRtkFrameType;
    rtk_action_t enRtkAction;

    if(priority >= QOS_MAX_INTERNAL_PRIORITY)
    {
        return DRV_ERR_PARA;
    }

    switch(stTrapSet.frameType)
    {
        case FRAME_TRAP_TYPE_RIP:
        case FRAME_TRAP_TYPE_ICMP:
        case FRAME_TRAP_TYPE_ICMPV6:
        case FRAME_TRAP_TYPE_ARP:
            return DRV_ERR_PARA;
        case FRAME_TRAP_TYPE_MLD:
            enRtkFrameType = IGMPMLD_TYPE_MLDV1;
            break;
        case FRAME_TRAP_TYPE_IGMP:
            enRtkFrameType = IGMPMLD_TYPE_IGMPV1;
            break;
        case FRAME_TRAP_TYPE_BGP:
        case FRAME_TRAP_TYPE_OSPFV2:
        case FRAME_TRAP_TYPE_OSPFV3:
        case FRAME_TRAP_TYPE_SNMP:
        case FRAME_TRAP_TYPE_SSH:
        case FRAME_TRAP_TYPE_FTP:
        case FRAME_TRAP_TYPE_TFTP:
        case FRAME_TRAP_TYPE_TELNET:
        case FRAME_TRAP_TYPE_HTTP:
        case FRAME_TRAP_TYPE_HTTPS:
        case FRAME_TRAP_TYPE_DHCPV6:
        case FRAME_TRAP_TYPE_DHCP:
        case FRAME_TRAP_TYPE_DOT1X:
        case FRAME_TRAP_TYPE_BPDU:
        default:
            return DRV_ERR_PARA;
    }

    switch(stTrapSet.trapAction)
    {
        case FRAME_TRAP_ACTION_FORWARD:
            enRtkAction = ACTION_FORWARD;
            break;
        case FRAME_TRAP_ACTION_DROP:
            enRtkAction = ACTION_DROP;
            break;
        case FRAME_TRAP_ACTION_TRAP2CPU:
            enRtkAction = ACTION_TRAP2CPU;
            break;
        case FRAME_TRAP_ACTION_COPY2CPU:
        case FRAME_TRAP_ACTION_TO_GUESTVLAN:
        case FRAME_TRAP_ACTION_FLOOD_IN_VLAN:
        case FRAME_TRAP_ACTION_FLOOD_IN_ALL_PORT:
        case FRAME_TRAP_ACTION_FLOOD_IN_ROUTER_PORTS:
        default:
            return DRV_ERR_PARA;
    }

    
    if(0 == lport)
    {
        LgcPortFor(lgcPort)
        {
            tdRtkRet = rtk_trap_portIgmpMldCtrlPktAction_set(lgcPort, enRtkFrameType, enRtkAction);
            if (RT_ERR_OK != tdRtkRet)
            {
                return DRV_SDK_GEN_ERROR;
            }
        }
    }
    else if(VALID_PORT(lport))
    {
        tdRtkRet = rtk_trap_portIgmpMldCtrlPktAction_set(lport, enRtkFrameType, enRtkAction);
        if (RT_ERR_OK != tdRtkRet)
        {
            return DRV_SDK_GEN_ERROR;
        }
    }
    else
    {
        return DRV_ERR_PARA;
    }
    
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_GetMacFindConflictMac
  Description:  find conflict mac with given mac entry
        Input:  stMacOp
       Output:  pstMacOp
       Return:  DRV_ERR_PARA
                DRV_OK
                DRV_ERR_UNKNOW
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_GetMacFindConflictMac(Mac_op_t stMacOp, Mac_op_t *pstMacOp)
{
    /*For lint*/
    memcpy(&stMacOp, &stMacOp, sizeof(stMacOp));
    pstMacOp = pstMacOp;

    /*rtl8305 does not support this functionality.*/
    return DRV_ERR_UNKNOW;
}

/*****************************************************************************
    Func Name: Hal_SetReservedMacTrap
  Description: 设置保留组播地址的转发行为 (0180-c200-0000 --- 0180-c200-002f)
        Input: UINT32 uiIndex            
                UINT32 priority           
                mac_trap_action_t action  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetReservedMacTrap(UINT32 uiIndex, UINT32 priority, rsv_mac_pass_action_t action)
{
    rtk_mac_t rtkMac;
    rtk_trap_rma_action_t rtkRamAction;

    if(uiIndex > 0x2f)
    {
        return DRV_ERR_PARA;
    }
    
    if(priority >= QOS_MAX_INTERNAL_PRIORITY)
    {
        return DRV_ERR_PARA;
    }

    switch (action)
    {
        case PASS_TYPE_CPU_ONLY:
            rtkRamAction = RMA_ACTION_TRAP2CPU;
            break;
        case PASS_TYPE_ALLPORTS:
            rtkRamAction = RMA_ACTION_FORWARD;
            break;
        case PASS_TYPE_CPU_EXCLUDE:
            rtkRamAction = RMA_ACTION_FORWARD_EXCLUDE_CPU;
            break;
        case PASS_TYPE_DROP:
            rtkRamAction = RMA_ACTION_DROP;
            break;
        default:
            return DRV_ERR_PARA;
    }
    
    rtkMac.octet[0] = 0x01;
    rtkMac.octet[1] = 0x80;
    rtkMac.octet[2] = 0xc2;
    rtkMac.octet[3] = 0x00;
    rtkMac.octet[4] = 0x00;
    rtkMac.octet[5] = (UINT8)uiIndex;

    if (RT_ERR_OK != rtk_trap_rmaAction_set(&rtkMac, rtkRamAction))
    {
        return DRV_SDK_GEN_ERROR;
    }

    return DRV_OK;
}

#if 0
/*lint -restore*/

/*****************************************************************************
    Func Name: Hal_GetMacByIndex
  Description: 通过ARL Index获取MAC地址
        Input: UINT32 ulStartIdx              
       Output: mac_common_t * pstTmpMacEntry
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_GetMacByIndex(UINT32 ulStartIdx, mac_common_t * pstTmpMacEntry)
{
    UINT32 unit;
    UINT32 uiPhyId;
    phy_pmask_t stPhyMask;
    rtk_mac_t stRtkBrodMac;
    rtk_l2_addr_table_t stMacEntry;
    
    if((ulStartIdx > MAX_ARL_TABLE)
        || (NULL == pstTmpMacEntry))
    {
        return DRV_ERR_PARA;
    }

    memset(&stMacEntry, 0, sizeof(stMacEntry));
    memset(&stRtkBrodMac, 0xFF, sizeof(stRtkBrodMac));
    
    UnitFor(unit)
    {
        stMacEntry.index = (rtk_int32)ulStartIdx;
        if (RT_ERR_OK == rtk_l2_entry_get(&stMacEntry))
        {
            memset(pstTmpMacEntry, 0, sizeof(mac_common_t));

            /*unicast mac*/
            if(stMacEntry.mac.octet[0] & 0x01)
            {
                pstTmpMacEntry->ucMacType = MAC_UCAST;
                /*blackhole*/
                if(ENABLED == stMacEntry.sa_block)
                {
                    pstTmpMacEntry->port = 0;
                }
                else
                {
                    (void)_HAL_GetBitNum(stMacEntry.portmask, &uiPhyId);
                    if (!IsValidPhyID(uiPhyId))
                    {
                        return DRV_ERR_UNKNOW;
                    }
                    pstTmpMacEntry->port = PortPhyID2Logic(uiPhyId);                            
                    if((INVALID_PORT == pstTmpMacEntry->port) || 
                       (LOGIC_CPU_PORT == pstTmpMacEntry->port))
                    {
                        /*skip stack and cpu port*/
                        continue;
                    }
                }

                if(ENABLED == stMacEntry.is_static)
                {
                    /*static*/
                    pstTmpMacEntry->ucStatic = TRUE;
                }

                memcpy(pstTmpMacEntry->mac, stMacEntry.mac.octet, sizeof(mac_address_t));
                pstTmpMacEntry->vid = (UINT16)stMacEntry.cvid;

                return DRV_OK;
            }
            /*broadcast mac*/
            else if (memcmp(&stRtkBrodMac, &stMacEntry.mac, sizeof(stRtkBrodMac)))
            {
                pstTmpMacEntry->ucMacType = MAC_BROAD;

                if(ENABLED == stMacEntry.is_static)
                {
                    pstTmpMacEntry->ucStatic = TRUE;
                }

                memcpy(pstTmpMacEntry->mac, stMacEntry.mac.octet, sizeof(mac_address_t));
                pstTmpMacEntry->vid = (UINT16)stMacEntry.cvid;

                /*copy phymask to logicmask*/
                memcpy(&(stPhyMask.pmask[unit]), &(stMacEntry.portmask), sizeof(rtk_portmask_t));
                MaskPhy2Logic(&stPhyMask, &(pstTmpMacEntry->port_mask));
                
                return DRV_OK;
            }
            /*multicast mac*/
            else 
            {
                pstTmpMacEntry->ucMacType = MAC_MCAST;

                /*multicast always's static*/
                pstTmpMacEntry->ucStatic = TRUE;

                memcpy(pstTmpMacEntry->mac, stMacEntry.mac.octet, sizeof(mac_address_t));
                pstTmpMacEntry->vid = (UINT16)stMacEntry.cvid;

                /*copy phymask to logicmask*/
                memcpy(&(stPhyMask.pmask[unit]), &(stMacEntry.portmask), sizeof(rtk_portmask_t));
                MaskPhy2Logic(&stPhyMask, &(pstTmpMacEntry->port_mask));
                
                return DRV_OK;
            }
        }
    }
    
    return DRV_ERR_UNKNOW;
}

/*****************************************************************************
    Func Name: Hal_GetMacMoreByIndex
  Description: 从指定Index起一次获取指定数量的MAC地址，返回获取实际数量及下一个mac的index
        Input:  UINT32 ulStartIdx    
                UINT32 ulGetCount    
                ULONG userPointer   
                UINT32 ulPDataType -- 地址空间(ADDRESS_IN_USER_SPACE／ADDRESS_IN_KERNEL_SPACE)
       Output:  UINT32* pulCountOut  
                UINT32* pulNextIdx   
       Return:  DRV_ERR_PARA / DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_GetMacMoreByIndex(UINT32 ulStartIdx, UINT32 ulGetCount, ULONG userPointer, UINT32* pulCountOut, UINT32* pulNextIdx, UINT32 ulPDataType)
{
    /*lint -e550 */
    mac_common_t * pstUserMacDump;
    UINT32 unit;
    UINT32 uiPhyId;
    UINT32 ulIndex, ulLoopCnt = 0, ulFindCount = 0;
    phy_pmask_t stPhyMask;
    mac_common_t stTmpMacEntry;
    rtk_mac_t stRtkBrodMac;
    rtk_l2_addr_table_t stMacEntry;
   
    if((ulStartIdx > MAX_ARL_TABLE) || (ulGetCount < 1)
        || (NULL == pulCountOut) || (NULL == pulNextIdx))
    {
        return DRV_ERR_PARA;
    }

    memset(&stMacEntry, 0, sizeof(stMacEntry));
    memset(&stRtkBrodMac, 0xFF, sizeof(stRtkBrodMac));
    
    ulLoopCnt = ulGetCount;
    /*user memory pointer*/
    pstUserMacDump = (mac_common_t *)userPointer;
    *pulCountOut = 0;
    *pulNextIdx = 0;
    
    UnitFor(unit)
    {
        for(ulIndex = ulStartIdx; 
            (ulFindCount < ulLoopCnt) && (ulIndex < MAX_ARL_TABLE); 
            ulIndex++)
        {
            stMacEntry.index = (rtk_int32)ulStartIdx;
            if (RT_ERR_OK == rtk_l2_entry_get(&stMacEntry))
            {
                memset(&stTmpMacEntry, 0, sizeof(mac_common_t));

                /*unicast mac*/
                if(stMacEntry.mac.octet[0] & 0x01)
                {
                    stTmpMacEntry.ucMacType = MAC_UCAST;
                    /*blackhole*/
                    if(ENABLED == stMacEntry.sa_block)
                    {
                        stTmpMacEntry.port = 0;
                    }
                    else
                    {
                        (void)_HAL_GetBitNum(stMacEntry.portmask, &uiPhyId);
                        if (!IsValidPhyID(uiPhyId))
                        {
                            return DRV_ERR_UNKNOW;
                        }
                        stTmpMacEntry.port = PortPhyID2Logic(uiPhyId);                            
                        if((INVALID_PORT == stTmpMacEntry.port)
                            || (LOGIC_CPU_PORT == stTmpMacEntry.port))
                        {
                            /*skip stack and cpu port*/
                            continue;
                        }
                    }

                    if(ENABLED == stMacEntry.is_static)
                    {
                        /*static*/
                        stTmpMacEntry.ucStatic = TRUE;
                    }

                    memcpy(stTmpMacEntry.mac, stMacEntry.mac.octet, sizeof(mac_address_t));
                    stTmpMacEntry.vid = (UINT16)stMacEntry.cvid;

                    /*arl index*/
                    stTmpMacEntry.ulL2Idx = ulIndex;
                }
                /*broadcast mac*/
                else if (!memcmp(&stRtkBrodMac, &stMacEntry.mac, sizeof(stRtkBrodMac)))
                {
                    stTmpMacEntry.ucMacType = MAC_BROAD;

                    if (ENABLED == stMacEntry.is_static)
                    {
                        stTmpMacEntry.ucStatic = TRUE;
                    }

                    memcpy(stTmpMacEntry.mac, stMacEntry.mac.octet, sizeof(mac_address_t));
                    stTmpMacEntry.vid = (UINT16)stMacEntry.cvid;

                    /*copy phymask to logicmask*/
                    memcpy(&(stPhyMask.pmask[unit]), &(stMacEntry.portmask), sizeof(rtk_portmask_t));
                    MaskPhy2Logic(&stPhyMask, &(stTmpMacEntry.port_mask));

                    /*arl index*/
                    stTmpMacEntry.ulL2Idx = ulIndex;
                }
                /*multicast mac*/
                else 
                {
                    stTmpMacEntry.ucMacType = MAC_MCAST;

                    /*multicast always's static*/
                    stTmpMacEntry.ucStatic = TRUE;

                    memcpy(stTmpMacEntry.mac, stMacEntry.mac.octet, sizeof(mac_address_t));
                    stTmpMacEntry.vid = (UINT16)stMacEntry.cvid;

                    /*copy phymask to logicmask*/
                    memcpy(&(stPhyMask.pmask[unit]), &(stMacEntry.portmask), sizeof(rtk_portmask_t));
                    MaskPhy2Logic(&stPhyMask, &(stTmpMacEntry.port_mask));

                    /*arl index*/
                    stTmpMacEntry.ulL2Idx = ulIndex;
                }

                /*copy to user memory space*/
                if(ADDRESS_IN_USER_SPACE == ulPDataType)
                {
                    (void)copy_to_user(pstUserMacDump, &stTmpMacEntry, sizeof(mac_common_t));
                }
                /*copy to kernel memory space*/
                else
                {
                    (void)memcpy(pstUserMacDump, &stTmpMacEntry, sizeof(mac_common_t));
                }

                pstUserMacDump++;
                ulFindCount++;
            }
        }
    }
    
    *pulCountOut = ulFindCount;
    *pulNextIdx = ulIndex;
    
    return DRV_OK;
}
#endif

/*****************************************************************************
    Func Name: Hal_GetMacExact
  Description: 查找指定vlan的mac地址是否存在，存在则返回之
        Input: vlan_id_t tdVid          
               mac_address_t mac_addr   
       Output: mac_common_t *pstTmpMac  
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_GetMacExact(vlan_id_t tdVid, mac_address_t mac_addr, mac_common_t *pstTmpMac)
{
    UINT32 unit;
    port_num_t lport;
    phy_pmask_t stPhyMask;
    rtk_l2_ucastAddr_t stUcastAddr;
	rtk_l2_mcastAddr_t stMcastAddr;

    if(!VALID_VLAN_ID(tdVid))
    {
        return DRV_ERR_PARA;
    }
    if(NULL == pstTmpMac)
    {
        return DRV_ERR_PARA;
    }

    /*init return value*/
    memset(pstTmpMac, 0, sizeof(mac_common_t));
    memcpy(pstTmpMac->mac, mac_addr, sizeof(mac_address_t));
    pstTmpMac->vid = (vlan_id_t)tdVid;
    
    /*find unicast first*/
    memset(&stUcastAddr, 0, sizeof(stUcastAddr));
    memcpy(stUcastAddr.mac.octet, mac_addr, sizeof(mac_address_t));
	//stUcastAddr.flags |= RTK_L2_UCAST_FLAG_IVL;
    stUcastAddr.vid = tdVid;

    UnitFor(unit)
    {
        if(RT_ERR_OK == rtk_l2_addr_get(&stUcastAddr))
        {
            /*black hole mac address*/
            //if((ENABLED == stUcastAddr.sa_block) || 
            //   (ENABLED == stUcastAddr.da_block))
            if((stUcastAddr.flags & RTK_L2_UCAST_FLAG_SA_BLOCK) ||
				(stUcastAddr.flags & RTK_L2_UCAST_FLAG_DA_BLOCK))
            {
                pstTmpMac->port = 0;
            }
            else
            {
                /*normal ucast*/
                lport = PortPhyID2Logic(TOPHYID(unit, stUcastAddr.port));
                if(INVALID_PORT == lport)
                {
                    continue;
                }
                else
                {
                    pstTmpMac->port = lport;
                }
            }

            //if(ENABLED == stUcastAddr.is_static)
            if(stUcastAddr.flags & RTK_L2_UCAST_FLAG_STATIC)
            {
                pstTmpMac->ucStatic = TRUE;
            }

            return DRV_OK;
        }
    }

    /*find multicast*/
    memset(&stMcastAddr, 0, sizeof(stMcastAddr));
	memcpy(stMcastAddr.mac.octet, mac_addr, sizeof(mac_address_t));
	stMcastAddr.flags |= RTK_L2_UCAST_FLAG_IVL;
    stMcastAddr.vid = tdVid;

    UnitFor(unit)
    {
        
        if(RT_ERR_OK == rtk_l2_mcastAddr_get(&stMcastAddr))
        {
            memcpy(&(stPhyMask.pmask[unit]), &stMcastAddr.portmask, sizeof(rtk_portmask_t));
            MaskPhy2Logic(&stPhyMask, &(pstTmpMac->port_mask));
            pstTmpMac->ucStatic = TRUE;
            return DRV_OK;
        }
    }
 
    return DRV_ERR_MAC_ADDR_NOT_FOUND;
}

/*****************************************************************************
    Func Name: Hal_GetMcastCount
  Description: 获取当前组播地址数
        Input: UINT32 * pulMcastCount  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_GetMcastCount(UINT32 * pulMcastCount)
{
    SPIN_HAL_MAC_LOCK;
    *pulMcastCount = ulMacMcastCount;
    SPIN_HAL_MAC_UNLOCK;

    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetMcastLookupMissDrop
  Description:  设置未知组播丢弃开启关闭
        Input:  ulEnable
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/
DRV_RET_E Hal_SetMcastLookupMissDrop(UINT32 ulEnable)
{        
    UINT32 unit;
    UINT32 uiLPort;
    UINT32 uiPPort;
    rtk_action_t stAction;

    if((DISABLE != ulEnable) && (ENABLE != ulEnable))
    {
        return DRV_ERR_PARA;
    }

    if(DISABLE == ulEnable)
    {
        /*Forward unknow multicast-MAC packet*/
        stAction = ACTION_FORWARD;

    }
    else
    {
        /*Drop unknow multicast-MAC packet*/
        stAction = ACTION_DROP;
    }

    UnitFor(unit)
    {
        LgcPortFor(uiLPort)
        {
            uiPPort = PortLogic2PhyID(uiLPort);
            if (INVALID_PORT == uiPPort)
            {
                continue;
            }
        
            if(RT_ERR_OK != rtk_l2_portLookupMissAction_set(uiPPort, DLF_TYPE_MCAST, stAction))
            {
                return DRV_SDK_GEN_ERROR;
            }
        }
    }

    /*Drop unknow mcpkt ingress from uplink port.*/
    if(RT_ERR_OK != rtk_l2_portLookupMissAction_set(SWITCH_UPLINK_PHY_PORT, DLF_TYPE_MCAST, stAction))
    {
        return DRV_SDK_GEN_ERROR;
    }
    return DRV_OK;
}

/*****************************************************************************
    Func Name: Hal_SetUnKnowIp4McastAct
  Description: Set unknow ipv4 multicast(excluding ip address 224.0.0.x) action.
        Input: MCAST_UNKNOW_ACT_E enActMode  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Hal_SetUnKnowIp4McastAct(MCAST_UNKNOW_ACT_E enActMode)
{        
    UINT32 unit;
    UINT32 uiLPort;
    UINT32 uiPPort;
    rtk_action_t stAction;

    if(MCAST_ACT_END <= enActMode)
    {
        return DRV_ERR_PARA;
    }

    switch (enActMode)
    {
        case MCAST_ACT_FORWARD:
            stAction = ACTION_FORWARD;
            break;
        case MCAST_ACT_DROP:
            stAction = ACTION_DROP;
            break;
        case MCAST_ACT_TRAP2CPU:
            stAction = ACTION_TRAP2CPU;
            break;
        case MCAST_ACT_ROUTER_PORT:
            stAction = ACTION_FLOOD_IN_ROUTER_PORTS;
            break;
        default :
            return DRV_ERR_PARA;
    }

    UnitFor(unit)
    {
        LgcPortFor(uiLPort)
        {
            uiPPort = PortLogic2PhyID(uiLPort);
            if (INVALID_PORT == uiPPort)
            {
                continue;
            }
        
            if(RT_ERR_OK != rtk_l2_portLookupMissAction_set(uiPPort, DLF_TYPE_IPMC, stAction))
            {
                return DRV_SDK_GEN_ERROR;
            }
        }
    }

    /*Drop unknow mcpkt ingress from uplink port.*/
    if(RT_ERR_OK != rtk_l2_portLookupMissAction_set(SWITCH_UPLINK_PHY_PORT, DLF_TYPE_IPMC, stAction))
    {
        return DRV_SDK_GEN_ERROR;
    }
    return DRV_OK;
}

/*****************************************************************************
    Func Name:  Hal_SetIgmpPktAction
  Description:  设置IGMP 报文是否上CPU
        Input:  
       Output: 
       Return:  DRV_ERR_PARA
                DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------

*****************************************************************************/

DRV_RET_E Hal_SetIgmpPktAction(UINT32 ulAction)
{
	UINT32 unit;
    UINT32 uiLPort;
    UINT32 uiPPort;
    rtk_action_t stAction;

    switch (ulAction)
	{
	    case PASS_TYPE_CPU_ONLY:
			 stAction = ACTION_TRAP2CPU;
			 break;
		case PASS_TYPE_ALLPORTS:
			 stAction = ACTION_FORWARD;
			 break;
		case PASS_TYPE_DROP:
			 stAction = ACTION_DROP;
			 break;
		case PASS_TYPE_CPU_EXCLUDE:
			 stAction = ACTION_FORWARD_EXCLUDE_CPU;
			 break;
		default:
			 return DRV_ERR_PARA;
	}
	
	UnitFor(unit)
    {
        LgcPortFor(uiLPort)
        {
            uiPPort = PortLogic2PhyID(uiLPort);
            if (INVALID_PORT == uiPPort)
            {
                continue;
            }
        
            if(RT_ERR_OK != rtk_trap_portIgmpMldCtrlPktAction_set(uiPPort, IGMPMLD_TYPE_IGMPV1, stAction) ||
  			   RT_ERR_OK != rtk_trap_portIgmpMldCtrlPktAction_set(uiPPort, IGMPMLD_TYPE_IGMPV2, stAction))
            {
                return DRV_SDK_GEN_ERROR;
            }
			diag_printk("Hal_SetIgmpPktAction port %d action %d\n", uiPPort, stAction);
        }
    }
    return DRV_OK;
}

#if 0
/*  */
DRV_RET_E Hal_GetFdbEntryByIndex(UINT32 uiIndex, UINT32 *puiFid, mac_common_t *pstCommMac)
{
    rtk_l2_addr_table_t stMacEntry;

    if ((NULL == puiFid) ||
        (NULL == pstCommMac))
    {
        return DRV_ERR_PARA;
    }

    *puiFid = 0;
    memset(pstCommMac, 0, sizeof(mac_common_t));
    
    stMacEntry.index = (INT32)uiIndex;
    if (RT_ERR_OK == rtk_l2_entry_get(&stMacEntry))
    {
        *puiFid = stMacEntry.fid;
        pstCommMac->vid = (vlan_id_t)(stMacEntry.cvid);
        /*temporarily use ulL2Idx for phy port mask. */
        pstCommMac->ulL2Idx = stMacEntry.portmask;
        memcpy(pstCommMac->mac, stMacEntry.mac.octet, 6);
    }

    return DRV_OK;
}
/* 014528 */
#endif

#ifdef  __cplusplus
}
#endif


