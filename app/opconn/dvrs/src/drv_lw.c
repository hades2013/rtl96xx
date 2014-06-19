/*****************************************************************************


*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif



#include "lw_config.h"

/*For lint*/
#include "../../vos/linux/h/vos_sync.h"
#include <math.h>
#include <string.h>
#include "vos_types.h"
#include "vos_libc.h"
//#include "common_vos.h"
#include "oam.h"
#include "../../qos/h/odm_qos.h"
#include "oam_lw.h"
#include "lw_type.h"
#include "lw_drv_pub.h"
#include "opconn_usr_ioctrl.h"
/*For lint*/
#include "lw_drv_req.h"
#include "drv_lw.h"
#include "multicast_control.h"

static UINT32  oam_monitor_port = MONITOR_PORT_DEFAULT_VALUE;
static logic_pmask_t  oam_in_mirror_lmask = {0x00};
static logic_pmask_t  oam_out_mirror_lmask = {0x00};

extern UINT8 g_bPonLos;
//extern UCHAR g_aucOnuMac[MAC_ADDR_LEN];

UCHAR g_aucOnuMac[MAC_ADDR_LEN];
UINT8 g_bLightAlways = FALSE;
ONU_BOARD_TYPE_E g_enOnuBoardType = ONU_BORAD_TYPE_EPN104;
#if defined(CONFIG_PRODUCT_EPN200)
BOOL_T m_abAutoNegEnable[CTC_MAX_SUBPORT_NUM + 1] = {BOOL_TRUE, BOOL_TRUE};
#else
BOOL_T m_abAutoNegEnable[CTC_MAX_SUBPORT_NUM + 1] = {BOOL_TRUE, BOOL_TRUE, BOOL_TRUE, BOOL_TRUE, BOOL_TRUE};
#endif
#if 0
#define diag_printf printf
#else
#define diag_printf(fmt, ...)
#endif

/* 当前的平台配置 */
PORT_CFG_S g_astPortCfg[MAX_PORT_NUM+1];

/*used for eoam filter*/
static VOS_MUTEX_t m_DrvCfgMutex;
#if !defined(CONFIG_PRODUCT_EPN200)
#define DRV_CFGLOCK   vosMutexTake(&m_DrvCfgMutex)
#define DRV_CFGUNLOCK vosMutexGive(&m_DrvCfgMutex)

#define MAX_ACL_FILTER_NUM 32

INT32 CTC_DRV_GetVlanIdAndPri(UINT32 uiVlanTag, UINT32 *puiVlanId, UINT32 *puiPri);
#endif

//static SLL_S stEoamFilterDataList;
static PORT_QINQ_S g_astUniQinqMode[MAX_PORT_NUM+1];


double g_adPower[100][2] = {
0.100000 , -1.000000 ,
0.200000 , -0.698970 ,
0.300000 , -0.522879 ,
0.400000 , -0.397940 ,
0.500000 , -0.301030 ,
0.600000 , -0.221849 ,
0.700000 , -0.154902 ,
0.800000 , -0.096910 ,
0.900000 , -0.045757 ,
1.000000 , -0.000000 ,
1.100000 , 0.041393 ,
1.200000 , 0.079181 ,
1.300000 , 0.113943 ,
1.400000 , 0.146128 ,
1.500000 , 0.176091 ,
1.600000 , 0.204120 ,
1.700000 , 0.230449 ,
1.800000 , 0.255273 ,
1.900000 , 0.278754 ,
2.000000 , 0.301030 ,
2.100000 , 0.322219 ,
2.200000 , 0.342423 ,
2.300000 , 0.361728 ,
2.400000 , 0.380211 ,
2.500000 , 0.397940 ,
2.600000 , 0.414973 ,
2.700000 , 0.431364 ,
2.800000 , 0.447158 ,
2.900000 , 0.462398 ,
3.000000 , 0.477121 ,
3.100000 , 0.491362 ,
3.200000 , 0.505150 ,
3.300000 , 0.518514 ,
3.400000 , 0.531479 ,
3.500000 , 0.544068 ,
3.600000 , 0.556303 ,
3.700000 , 0.568202 ,
3.800000 , 0.579784 ,
3.900000 , 0.591065 ,
4.000000 , 0.602060 ,
4.100000 , 0.612784 ,
4.200000 , 0.623249 ,
4.300000 , 0.633468 ,
4.400000 , 0.643453 ,
4.500000 , 0.653213 ,
4.600000 , 0.662758 ,
4.700000 , 0.672098 ,
4.800000 , 0.681241 ,
4.900000 , 0.690196 ,
5.000000 , 0.698970 ,
5.100000 , 0.707570 ,
5.200000 , 0.716003 ,
5.300000 , 0.724276 ,
5.400000 , 0.732394 ,
5.500000 , 0.740363 ,
5.600000 , 0.748188 ,
5.700000 , 0.755875 ,
5.800000 , 0.763428 ,
5.900000 , 0.770852 ,
6.000000 , 0.778151 ,
6.100000 , 0.785330 ,
6.200000 , 0.792392 ,
6.300000 , 0.799341 ,
6.400000 , 0.806180 ,
6.500000 , 0.812913 ,
6.600000 , 0.819544 ,
6.700000 , 0.826075 ,
6.800000 , 0.832509 ,
6.900000 , 0.838849 ,
7.000000 , 0.845098 ,
7.100000 , 0.851258 ,
7.200000 , 0.857332 ,
7.300000 , 0.863323 ,
7.400000 , 0.869232 ,
7.500000 , 0.875061 ,
7.600000 , 0.880814 ,
7.700000 , 0.886491 ,
7.800000 , 0.892095 ,
7.900000 , 0.897627 ,
8.000000 , 0.903090 ,
8.100000 , 0.908485 ,
8.200000 , 0.913814 ,
8.300000 , 0.919078 ,
8.400000 , 0.924279 ,
8.500000 , 0.929419 ,
8.600000 , 0.934498 ,
8.700000 , 0.939519 ,
8.800000 , 0.944483 ,
8.900000 , 0.949390 ,
9.000000 , 0.954243 ,
9.100000 , 0.959041 ,
9.200000 , 0.963788 ,
9.300000 , 0.968483 ,
9.400000 , 0.973128 ,
9.500000 , 0.977724 ,
9.600000 , 0.982271 ,
9.700000 , 0.986772 ,
9.800000 , 0.991226 ,
9.900000 , 0.995635 ,
10.000000 , 1.000000
};


/*****************************************************************************
 
*****************************************************************************/
VOID InitPortCfg(PORT_CFG_S *psPortCfg)
{
    USHORT i = 0;

    if(NULL == psPortCfg)
    {
        return;
    }

    MEM_SET_ZERO(psPortCfg, sizeof(PORT_CFG_S));
    /* port cfg */
    for (i = 1; i <= MAX_PORT_NUM; i++)
    {
        psPortCfg[i].ucPortEnable           = ENABLE;
        psPortCfg[i].ucSpeed                = PORT_SPEED_AUTO;
        psPortCfg[i].ucDuplex               = PORT_DUPLEX_AUTO;
        psPortCfg[i].ucMdi                  = PORT_MDI_AUTO;
        psPortCfg[i].ucFlowCtrl             = PORT_FLOWCTRL_DISABLE;
        psPortCfg[i].ulLineRateIn           = 0;
        psPortCfg[i].ulLineRateOut          = 0;
        psPortCfg[i].ucIsolateEnable        = DISABLE;
        psPortCfg[i].ucLinkType             = LINK_TYPE_ACCESS;
        psPortCfg[i].usPvid                 = DEFAULT_VID;
        psPortCfg[i].ucVlanPri              = 0;
        /*  */
        psPortCfg[i].ulQosTrustMode = QOS_COS;
        /* 014453 */
    }

    return;
}

/*****************************************************************************
   
*****************************************************************************/
INT32 DRV_LwInit(void)
{
    INT32 retVal;

    retVal = vosMutexCreate(&m_DrvCfgMutex);
    if (VOS_MUTEX_OK != retVal)
    {
        return ERROR;
    }

    //SLL_Init(&stEoamFilterDataList);
    InitPortCfg(g_astPortCfg);
    /*begin added by liaohongjun 2012/10/23 of EPN104QID0071*/
    (VOID)Drv_SwitchInit();
    /*end added by liaohongjun 2012/10/23 of EPN104QID0071*/
    return NO_ERROR;
}
#if !defined(CONFIG_PRODUCT_EPN200)
/*****************************************************************************

*****************************************************************************/
INT32 _CTC_GetUniConfiguration( UCHAR ucUniPortId, PORT_CFG_S *pstUniConf )
{
    /* 检查输入参数合法性 */
  

    DRV_CFGLOCK;
    /* 获取端口配置 */
    *pstUniConf = g_astPortCfg[ucUniPortId];
    DRV_CFGUNLOCK;

    return NO_ERROR;
}

/*****************************************************************************
    Func Name: _CTC_GetPortVlanModeCfg


*****************************************************************************/
static INT32 _CTC_GetPortVlanModeCfg(UINT32 uiLPortId, CTC_VLAN_CFG_S *pstCtcVlanMode)
{
    if ((!IsValidLgcPort(uiLPortId)) ||
        (NULL == pstCtcVlanMode))
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    if (DRV_OK != Ioctl_ctc_get_acl_rule(uiLPortId, pstCtcVlanMode))
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    return NO_ERROR;
}

/*****************************************************************************
    Func Name: _CTC_SetPortVlanModeCfg
 

*****************************************************************************/
static INT32 _CTC_SetPortVlanModeCfg(UINT32 uiLPortId, CTC_VLAN_CFG_S *pstCtcVlanMode)
{

    if ((!IsValidLgcPort(uiLPortId)) ||
        (NULL == pstCtcVlanMode))
    {
        return ERROR;
    }

    if (DRV_OK != Ioctl_ctc_set_acl_rule(uiLPortId, pstCtcVlanMode))
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    return NO_ERROR;
}

/*****************************************************************************
    Func Name: _CTC_AddVtuPortMem

  --------------------------------------------------------------------------

*****************************************************************************/
static INT32 _CTC_AddVtuPortMem(UINT32 uiLPort, UINT32 uiVlanId, CTC_VLAN_MEM_E enTagMode)
{
    INT32 iRet = NO_ERROR;
    logic_pmask_t stLMask, stUntagLMsk;

    if (!IsValidLgcPort(uiLPort)    ||
        (VLAN_MEM_END <= enTagMode) ||
        ((VLAN_ID_0 != uiVlanId) && !VALID_VLAN_ID(uiVlanId)))
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    ClrLgcMaskAll(&stLMask);
    ClrLgcMaskAll(&stUntagLMsk);

    switch (enTagMode)
    {
        case VLAN_MEM_UNTAG:
            SetLgcMaskBit(uiLPort, &stLMask);
            SetLgcMaskBit(uiLPort, &stUntagLMsk);
            break;
        case VLAN_MEM_TAG:
            SetLgcMaskBit(uiLPort, &stLMask);
            break;
        case VLAN_MEM_NO:
            ClrLgcMaskBit(uiLPort, &stLMask);
            ClrLgcMaskBit(uiLPort, &stUntagLMsk);
            break;
        default :
            break;
    }

    iRet = Ioctl_SetVlanMemberAdd(uiVlanId, stLMask, stUntagLMsk);
    if (DRV_OK != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    return NO_ERROR;
}

/*****************************************************************************


*****************************************************************************/
static INT32 _CTC_DelVtuPortMem(UINT32 uiLPort, UINT32 uiVlanId)
{
    INT32 iRet = NO_ERROR;
    logic_pmask_t stLMask;

    if (!IsValidLgcPort(uiLPort)    ||
        ((VLAN_ID_0 != uiVlanId) && !VALID_VLAN_ID(uiVlanId)))
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    ClrLgcMaskAll(&stLMask);
    SetLgcMaskBit(uiLPort, &stLMask);

    iRet = Ioctl_SetVlanMemberRemove(uiVlanId, stLMask);
    if (DRV_OK != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    return NO_ERROR;
}

/*****************************************************************************
  
*****************************************************************************/
static INT32 _CTC_AddVtuPortMemForCtcVlan(UINT32 uiLPortId, UINT32 uiVlanId, UINT32 uiPri, CTC_VLAN_MEM_E enMemship)
{
    UINT32 uiPortIndex = 0;
    INT32 iRet = NO_ERROR;
    logic_pmask_t stLMask, stUntagLMsk;
    CTC_VLAN_CFG_S stVlanMode;

    if (!IsValidLgcPort(uiLPortId) ||
        !VALID_VLAN_ID(uiVlanId)   ||
        (VLAN_MEM_END <= enMemship))
    {
        return ERROR;
    }

    /*For lint*/
    uiPri = uiPri;

    ClrLgcMaskAll(&stLMask);
    ClrLgcMaskAll(&stUntagLMsk);

    SetLgcMaskBit(LOGIC_PON_PORT, &stLMask);

    switch (enMemship)
    {
        case VLAN_MEM_UNTAG:
            SetLgcMaskBit(uiLPortId, &stLMask);
            SetLgcMaskBit(uiLPortId, &stUntagLMsk);
            break;
        case VLAN_MEM_TAG:
            SetLgcMaskBit(uiLPortId, &stLMask);
            break;
        case VLAN_MEM_NO:
            ClrLgcMaskBit(uiLPortId, &stLMask);
            ClrLgcMaskBit(uiLPortId, &stUntagLMsk);
            break;
        default :
            break;
    }

    LgcPortFor(uiPortIndex)
    {
        if (uiPortIndex == uiLPortId)
        {
            continue;
        }

        iRet = _CTC_GetPortVlanModeCfg(uiPortIndex, &stVlanMode);
        if (NO_ERROR != iRet)
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }

        if ((CTC_VLAN_MODE_TRANSPARENT == stVlanMode.mode) ||
            (CTC_VLAN_MODE_QINQ == stVlanMode.mode))
        {
            SetLgcMaskBit(uiPortIndex, &stLMask);
        }
    }

    iRet = Ioctl_SetVlanMemberAdd(uiVlanId, stLMask, stUntagLMsk);
    if (DRV_OK != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return DRV_ERR_UNKNOW;
    }

    return NO_ERROR;
}

/*****************************************************************************
   
*****************************************************************************/
static INT32 _CTC_DelVtuPortMemForCtcVlan(UINT32 uiLPortId, UINT32 uiVlanId)
{
    UINT32 uiPortIndex = 0;
    INT32 iRet = NO_ERROR;
    INT32 iFlag = FALSE;
    logic_pmask_t stLMask;
    CTC_VLAN_CFG_S stVlanMode;

    if (!IsValidLgcPort(uiLPortId) || !VALID_VLAN_ID(uiVlanId))
    {
        return ERROR;
    }


    LgcPortFor(uiPortIndex)
    {
        if (uiPortIndex == uiLPortId)
        {
            continue;
        }

        iRet = _CTC_GetPortVlanModeCfg(uiPortIndex, &stVlanMode);
        if (NO_ERROR != iRet)
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }

        #if 0
        if ((CTC_VLAN_MODE_TAG == stVlanMode.mode) ||
            (CTC_VLAN_MODE_QINQ == stVlanMode.mode))
        #else
        if (CTC_VLAN_MODE_TAG == stVlanMode.mode)
        #endif
        {

            if (uiVlanId == stVlanMode.default_vlan)
            {
                iFlag = TRUE;
                break;
            }
        }
        else if (CTC_VLAN_MODE_TRANSLATION == stVlanMode.mode)
        {
            INT32  iFlagTmp = FALSE;

            UINT32 i = 0;

            if (uiVlanId == stVlanMode.default_vlan)
            {
                iFlag = TRUE;
                break;
            }

            for (i = 0; i < stVlanMode.number_of_entries; i++)
            {
                if ((uiVlanId == stVlanMode.vlan_list[i*2]) ||
                    (uiVlanId == stVlanMode.vlan_list[(i*2) + 1]))
                {
                    iFlagTmp = TRUE;
                    break;
                }
            }

            if (TRUE == iFlagTmp)
            {
                iFlag = TRUE;
                break;
            }
        }
    }

    /*Remove this port from the vtu if it is used by other ports.*/
    if (TRUE == iFlag)
    {
        ClrLgcMaskAll(&stLMask);
        SetLgcMaskBit(uiLPortId, &stLMask);

        if (DRV_OK != Ioctl_SetVlanMemberRemove(uiVlanId, stLMask))
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }
    }
    /*Delete the vtu if it is not used by anyone.*/
    else
    {
        if (DRV_OK != Ioctl_SetVlanEntryDelete(uiVlanId))
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }
    }

    return NO_ERROR;
}

/*****************************************************************************


*****************************************************************************/
static INT32 _CTC_ResetTagVlan(UINT32 uiLPortId)
{
    INT32 iRet = NO_ERROR;
    UINT32 uiPvid = 0;
    logic_pmask_t stLMask;
    CTC_VLAN_CFG_S stVlanMode;

    if (!IsValidLgcPort(uiLPortId))
    {
        return ERROR;
    }

    iRet = _CTC_GetPortVlanModeCfg(uiLPortId, &stVlanMode);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    uiPvid = stVlanMode.default_vlan;
    ClrLgcMaskAll(&stLMask);
    SetLgcMaskBit(uiLPortId, &stLMask);

    iRet = Ioctl_ctc_del_port_base_vlan_mem(uiPvid, &stLMask);
    if (DRV_OK != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = Ioctl_ctc_del_acl_rule_for_tag(uiLPortId);
    if (DRV_OK != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = _CTC_DelVtuPortMemForCtcVlan(uiLPortId, uiPvid);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /*Clear port base vlan entry used for pvid.*/
    iRet = Ioctl_ctc_clr_port_base_vlan_entry_by_vid(uiPvid);
    if (DRV_OK != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;  
    }

    return NO_ERROR;
}

/*****************************************************************************
   

*****************************************************************************/
static INT32 _CTC_ResetTranslationVlan(UINT32 uiLPortId)
{
    INT32 iRet = NO_ERROR;
    UINT32 i = 0;
    UINT32 uiOldVid = 0;
    UINT32 uiNewVid = 0;
    UINT32 uiPvid = 0;
    logic_pmask_t stLMask;
    CTC_VLAN_CFG_S stVlanMode;

    if (!IsValidLgcPort(uiLPortId))
    {
        return ERROR;
    }

    iRet = _CTC_GetPortVlanModeCfg(uiLPortId, &stVlanMode);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    uiPvid = stVlanMode.default_vlan;

    ClrLgcMaskAll(&stLMask);
    SetLgcMaskBit(uiLPortId, &stLMask);

    iRet = Ioctl_ctc_del_port_base_vlan_mem(uiPvid, &stLMask);
    if (DRV_OK != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = Ioctl_ctc_del_acl_rule_for_transl(uiLPortId);
    if (DRV_OK != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /*Delete port from default vtu.*/
    iRet = _CTC_DelVtuPortMemForCtcVlan(uiLPortId, uiPvid);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /*Clear port base vlan entry used for pvid.*/
    iRet = Ioctl_ctc_clr_port_base_vlan_entry_by_vid(uiPvid);
    if (DRV_OK != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;  
    }
    
    /*Delete port from vlan list.*/
    for (i = 0; i < stVlanMode.number_of_entries; i++)
    {
        uiOldVid    = stVlanMode.vlan_list[i*2];
        uiNewVid    = stVlanMode.vlan_list[(i*2) + 1];

        iRet = _CTC_DelVtuPortMemForCtcVlan(uiLPortId, uiOldVid);
        if (NO_ERROR != iRet)
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }

        iRet = _CTC_DelVtuPortMemForCtcVlan(uiLPortId, uiNewVid);
        if (NO_ERROR != iRet)
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }

        /*Clear port base vlan entry used for old vlan id.*/
        iRet = Ioctl_ctc_clr_port_base_vlan_entry_by_vid(uiOldVid);
        if (DRV_OK != iRet)
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;  
        }
        
        /*Clear port base vlan entry used for new vlan id.*/
        iRet = Ioctl_ctc_clr_port_base_vlan_entry_by_vid(uiNewVid);
        if (DRV_OK != iRet)
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;  
        }
    }

    return NO_ERROR;
}

/*****************************************************************************
  
*****************************************************************************/
static INT32 _CTC_AddReservedVlanMem(UINT32 uiLPortId)
{
    INT32 iRet;
    logic_pmask_t stLMask;


    if (!IsValidLgcPort(uiLPortId))
    {
        return ERROR;
    }

    /* Set the default VLAN point to vlan index 31.
    Vlan index 31 is default vlan. Its vlan id is 0.*/
    ClrLgcMaskAll(&stLMask);
    SetLgcMaskBit(uiLPortId, &stLMask);
    iRet = Ioctl_ctc_add_port_base_vlan_mem_by_id(31, &stLMask);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    return NO_ERROR;
}


/*****************************************************************************
  
*****************************************************************************/
static INT32 _CTC_DelReservedVlanMem(UINT32 uiLPortId)
{
    INT32 iRet;
    logic_pmask_t stLMask;


    if (!IsValidLgcPort(uiLPortId))
    {
        return ERROR;
    }

    /* The default VLAN points to vlan index 31.
    Vlan index 31 is default vlan. Its vlan id is 0.*/
    ClrLgcMaskAll(&stLMask);
    SetLgcMaskBit(uiLPortId, &stLMask);
    iRet = Ioctl_ctc_del_port_base_vlan_mem_by_id(31, &stLMask);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    return NO_ERROR;
}

/*****************************************************************************
   
*****************************************************************************/
static INT32 _CTC_RenewVtuMemForTranspPort(UINT32 uiLPortId)
{
    INT32 iRet = NO_ERROR;
    UINT32 uiPortIndex = 0;
    CTC_VLAN_CFG_S stVlanMode;

    if (!IsValidLgcPort(uiLPortId))
    {
        return ERROR;
    }

    LgcPortFor(uiPortIndex)
    {
        if (uiPortIndex == uiLPortId)
        {
            continue;
        }

        iRet = _CTC_GetPortVlanModeCfg(uiPortIndex, &stVlanMode);
        if (NO_ERROR != iRet)
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }

        #if 0
        if ((CTC_VLAN_MODE_TAG == stVlanMode.mode) ||
            (CTC_VLAN_MODE_QINQ == stVlanMode.mode))
        #else
        if (CTC_VLAN_MODE_TAG == stVlanMode.mode)
        #endif
        {
            iRet = _CTC_AddVtuPortMem(uiLPortId, stVlanMode.default_vlan, VLAN_MEM_TAG);
            if (NO_ERROR != iRet)
            {
                mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
                return ERROR;
            }
        }
        else if (CTC_VLAN_MODE_TRANSLATION == stVlanMode.mode)
        {
            UINT32 i = 0;

            iRet = _CTC_AddVtuPortMem(uiLPortId, stVlanMode.default_vlan, VLAN_MEM_TAG);
            if (NO_ERROR != iRet)
            {
                mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
                return ERROR;
            }

            for(i = 0; i < stVlanMode.number_of_entries; i++)
            {
                iRet = _CTC_AddVtuPortMem(uiLPortId, stVlanMode.vlan_list[i*2], VLAN_MEM_TAG);
                if (NO_ERROR != iRet)
                {
                    mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
                    return ERROR;
                }

                iRet = _CTC_AddVtuPortMem(uiLPortId, stVlanMode.vlan_list[(i*2) + 1], VLAN_MEM_TAG);
                if (NO_ERROR != iRet)
                {
                    mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
                    return ERROR;
                }
            }
        }
    }

    return NO_ERROR;
}

/*****************************************************************************
    
*****************************************************************************/
static INT32 _CTC_DelVtuMemForTranspPort(UINT32 uiLPortId)
{
    INT32 iRet = NO_ERROR;
    UINT32 uiPortIndex = 0;
    CTC_VLAN_CFG_S stVlanMode;

    if (!IsValidLgcPort(uiLPortId))
    {
        return ERROR;
    }

    LgcPortFor(uiPortIndex)
    {
        if (uiPortIndex == uiLPortId)
        {
            continue;
        }

        iRet = _CTC_GetPortVlanModeCfg(uiPortIndex, &stVlanMode);
        if (NO_ERROR != iRet)
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }

        #if 0
        if ((CTC_VLAN_MODE_TAG == stVlanMode.mode) ||
            (CTC_VLAN_MODE_QINQ == stVlanMode.mode))
        #else
        if (CTC_VLAN_MODE_TAG == stVlanMode.mode)
        #endif
        {
            iRet = _CTC_DelVtuPortMem(uiLPortId, stVlanMode.default_vlan);
            if (NO_ERROR != iRet)
            {
                mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
                return ERROR;
            }
        }
        else if (CTC_VLAN_MODE_TRANSLATION == stVlanMode.mode)
        {
            UINT32 i = 0;

            iRet = _CTC_DelVtuPortMem(uiLPortId, stVlanMode.default_vlan);
            if (NO_ERROR != iRet)
            {
                mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
                return ERROR;
            }

            for(i = 0; i < stVlanMode.number_of_entries; i++)
            {
                iRet = _CTC_DelVtuPortMem(uiLPortId, stVlanMode.vlan_list[i*2]);
                if (NO_ERROR != iRet)
                {
                    mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
                    return ERROR;
                }

                iRet = _CTC_DelVtuPortMem(uiLPortId, stVlanMode.vlan_list[(i*2) + 1]);
                if (NO_ERROR != iRet)
                {
                    mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
                    return ERROR;
                }
            }
        }
    }

    return NO_ERROR;
}

/*****************************************************************************
  

*****************************************************************************/
static BOOL _CTC_CheckVidForTagMode(UINT32 uiLPortId, UINT32 uiVlanId)
{
    UINT32 uiLPortIndex;
    CTC_VLAN_CFG_S stVlanMode;

    if ((!IsValidLgcPort(uiLPortId)) || (!VALID_VLAN_ID(uiVlanId)))
    {
        return FALSE;
    }

    /*ctc vlan can not be same with mc vlan.*/
    #ifdef CTC_MULTICAST_SURPORT
    {
        extern OPL_STATUS mcVlanExist(UINT32 vlanId);
        if (OPL_TRUE == mcVlanExist(uiVlanId))
        {
            return FALSE;
        }
    }
    #endif
    
    LgcPortFor(uiLPortIndex)
    {
        if (uiLPortIndex == uiLPortId)
        {
            continue;
        }

        (VOID)_CTC_GetPortVlanModeCfg(uiLPortIndex, &stVlanMode);
        if (CTC_VLAN_MODE_TRANSLATION == stVlanMode.mode)
        {
            UINT32 uiVlanIndex = 0;

            for (uiVlanIndex = 0; uiVlanIndex < stVlanMode.number_of_entries; uiVlanIndex++)
            {
                /*The case that pvid of tag vlan mode equals old vlan of translation mode is not allowed.*/
                if ((stVlanMode.vlan_list[uiVlanIndex*2] != stVlanMode.vlan_list[uiVlanIndex*2 + 1]) &&
                    ((uiVlanId == stVlanMode.vlan_list[uiVlanIndex*2]) ||
                     (uiVlanId == stVlanMode.vlan_list[uiVlanIndex*2 + 1])))
                {
                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}

/*****************************************************************************
   
*****************************************************************************/
static BOOL _CTC_HaveEnoughEmptyRules(UINT32 uiIdNum)
{
    INT32 iRet;
    UINT32 uiAclEmptyNum = 0;

    iRet = Ioctl_ctc_get_empty_acl_num(&uiAclEmptyNum);
    if (DRV_OK != iRet)
    {
        return FALSE;
    }

    if (uiIdNum > uiAclEmptyNum)
    {
        mc_printf("\nWe need %d rules, and there are only %d empty rules leaved.\n", uiIdNum,uiAclEmptyNum);
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************************


*****************************************************************************/
static BOOL _CTC_CheckVlanForTranslationMode(UINT32 uiLPortId, UINT32 uiDefVlan, UINT32 uiVlanEntryNum, UINT32 *pVlanList)
{
    UINT32 i;
    UINT32 uiLPortIndex;
    UINT32 uiVlanIndex;
    UINT32 uiOldVlanIndex;
    UINT32 uiNewVlanIndex;
    CTC_VLAN_CFG_S stVlanMode;

    if ((!IsValidLgcPort(uiLPortId)) ||
        (!VALID_VLAN_ID(uiDefVlan))  ||
        (NULL == pVlanList))
    {
        return FALSE;
    }
    
    /*ctc vlan can not be same with mc vlan.*/
    #ifdef CTC_MULTICAST_SURPORT
    {
        extern OPL_STATUS mcVlanExist(UINT32 vlanId);
        if (OPL_TRUE == mcVlanExist(uiDefVlan))
        {
            return FALSE;
        }

        for (uiVlanIndex = 0; uiVlanIndex < uiVlanEntryNum; uiVlanIndex++)
        {
            uiOldVlanIndex = uiVlanIndex << 1;
            uiNewVlanIndex = (uiVlanIndex << 1) + 1;
            if ((OPL_TRUE == mcVlanExist(pVlanList[uiOldVlanIndex])) ||
                (OPL_TRUE == mcVlanExist(pVlanList[uiNewVlanIndex])))
            {
                return FALSE;
            }
        }
    }
    #endif
            
    LgcPortFor(uiLPortIndex)
    {
        if (uiLPortIndex == uiLPortId)
        {
            UINT32 uiOldIndex;
            UINT32 uiNewIndex;

            for (uiVlanIndex = 0; uiVlanIndex < uiVlanEntryNum; uiVlanIndex++)
            {
                uiOldVlanIndex = uiVlanIndex << 1;
                uiNewVlanIndex = (uiVlanIndex << 1) + 1;

                /* The case of pvid=oldvid=newvid is allowded.
                This case is used for access vlan. */
                if (pVlanList[uiOldVlanIndex] != pVlanList[uiNewVlanIndex])
                {
                    if ((uiDefVlan == pVlanList[uiOldVlanIndex]) ||
                        (uiDefVlan == pVlanList[uiNewVlanIndex]))
                    {
                        return FALSE;
                    }
                }

                /* Vids of different translation pairs can not be equal. */
                for (i = (uiVlanIndex + 1); i < uiVlanEntryNum; i++)
                {
                    uiOldIndex = i*2;
                    uiNewIndex = (i*2) + 1;

                    if ((pVlanList[uiOldVlanIndex] == pVlanList[uiOldIndex])     ||
                        (pVlanList[uiOldVlanIndex] == pVlanList[uiNewIndex]) ||
                        (pVlanList[uiNewVlanIndex] == pVlanList[uiOldIndex]) ||
                        (pVlanList[uiNewVlanIndex] == pVlanList[uiNewIndex]))
                    {
                        return FALSE;
                    }
                }
            }

            continue;
        }

        (VOID)_CTC_GetPortVlanModeCfg(uiLPortIndex, &stVlanMode);
        if ((CTC_VLAN_MODE_TAG == stVlanMode.mode) ||
            (CTC_VLAN_MODE_QINQ == stVlanMode.mode))
        {
            for (uiVlanIndex = 0; uiVlanIndex < uiVlanEntryNum; uiVlanIndex++)
            {
                uiOldVlanIndex = uiVlanIndex*2;
                uiNewVlanIndex = (uiVlanIndex*2) + 1;

                /*The case that pvid of tag vlan mode equals old vlan of translation mode is not allowed.*/
                if ((pVlanList[uiOldVlanIndex] != pVlanList[uiNewVlanIndex]) &&
                    ((pVlanList[uiOldVlanIndex] == stVlanMode.default_vlan) ||
                     (pVlanList[uiNewVlanIndex] == stVlanMode.default_vlan)))
                {
                    return FALSE;
                }
            }
        }
        else if (CTC_VLAN_MODE_TRANSLATION == stVlanMode.mode)
        {
            UINT32 uiOldIndex;
            UINT32 uiNewIndex;

            for (i = 0; i < stVlanMode.number_of_entries; i++)
            {
                uiOldIndex = i*2;
                uiNewIndex = (i*2) + 1;

                if ((stVlanMode.vlan_list[uiOldIndex] != stVlanMode.vlan_list[uiNewIndex]) &&
                    ((uiDefVlan == stVlanMode.vlan_list[uiOldIndex]) ||
                     (uiDefVlan == stVlanMode.vlan_list[uiNewIndex])))
                {
                    return FALSE;
                }
            }

            for (uiVlanIndex = 0; uiVlanIndex < uiVlanEntryNum; uiVlanIndex++)
            {
                uiOldVlanIndex = uiVlanIndex*2;
                uiNewVlanIndex = (uiVlanIndex*2) + 1;

                /*The case that pvid of tag vlan mode equals old vlan of translation mode is not allowed.*/
                if ((pVlanList[uiOldVlanIndex] != pVlanList[uiNewVlanIndex]) &&
                    ((pVlanList[uiOldVlanIndex] == stVlanMode.default_vlan) ||
                     (pVlanList[uiNewVlanIndex] == stVlanMode.default_vlan)))
                {
                    return FALSE;
                }

                for (i = 0; i < stVlanMode.number_of_entries; i++)
                {
                    uiOldIndex = i*2;
                    uiNewIndex = (i*2) + 1;

                    /*The case that old vids of different translation entries are equal but new vids are different
                    is not allowed.*/
                    if (((pVlanList[uiOldVlanIndex] == stVlanMode.vlan_list[uiOldIndex]) &&
                         (pVlanList[uiNewVlanIndex] != stVlanMode.vlan_list[uiNewIndex])) ||
                        ((pVlanList[uiOldVlanIndex] != stVlanMode.vlan_list[uiOldIndex]) &&
                         (pVlanList[uiNewVlanIndex] == stVlanMode.vlan_list[uiNewIndex])))
                    {
                        return FALSE;
                    }

                    if (pVlanList[uiOldVlanIndex] == stVlanMode.vlan_list[uiNewIndex])
                    {
                        /* If old vid of this entry is same with new vid of any other entriy,
                        only trunk mode is supported. */
                        if (!((pVlanList[uiOldVlanIndex] == pVlanList[uiNewVlanIndex]) &&
                             (stVlanMode.vlan_list[uiOldIndex] == stVlanMode.vlan_list[uiNewIndex])))
                        {
                            return FALSE;
                        }
                    }
                    else
                    {
                        /* If old vid of this entry is different from new vid of any other entry,
                        new vid of this entry should not be same with that entry's old vid. */
                        if (pVlanList[uiNewVlanIndex] == stVlanMode.vlan_list[uiOldIndex])
                        {
                            return FALSE;
                        }
                    }
                }
            }
        }
    }

    return TRUE;
}

/*****************************************************************************


*****************************************************************************/
static BOOL _CTC_CheckAclForCtcTranslVlan(UINT32 uiLPortId, UINT32 uiPvid, UINT32 *puiVlanList, UINT32 uiVlanListEntry)
{
    INT32  iRet;
    UINT32 uiAclNum;
    UINT32 uiPvidRuleNum = 0;
    UINT32 uiTransRuleNum = 0;
    UINT32 i;
    CTC_VLAN_CFG_S stVlanMode;

    if ((!IsValidLgcPort(uiLPortId)) ||
        (!VALID_VLAN_ID(uiPvid))  ||
        (NULL == puiVlanList))
    {
        return FALSE;
    }

    for (i = 0; i < uiVlanListEntry; i++)
    {
        /* For access vlan. */
        if ((puiVlanList[i*2] == puiVlanList[i*2 + 1]) &&
            (uiPvid == puiVlanList[i*2]))
        {
            continue;
        }

        /*Every vlan translation couple occupies 2 rules.*/
        uiTransRuleNum += 2;

        /*One rule is used for dropping packet with new vlan tag.*/
        if (puiVlanList[i*2] != puiVlanList[i*2 + 1])
        {
            uiTransRuleNum++;
        }
    }

    /*Pvid occupies 2 rules.*/
    uiPvidRuleNum = 2;
    uiAclNum = uiPvidRuleNum + uiTransRuleNum;

    (VOID)_CTC_GetPortVlanModeCfg(uiLPortId, &stVlanMode);

    /* If current vlan mode needs some extra acl rules,
    we have to check whether there are enough rules or not.
    */
    if (uiAclNum > stVlanMode.number_of_acl_rules)
    {
        uiAclNum -= stVlanMode.number_of_acl_rules;
        iRet = _CTC_HaveEnoughEmptyRules(uiAclNum);
        if (FALSE == iRet)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*****************************************************************************
    Func Name: _CTC_SetUniQosTrustModeCfg
 
  Description: Set port qos trust mode cfg.
        Input: ULONG ulLPort         
                ULONG ulQosTrustMode  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
INT32 _CTC_SetUniQosTrustModeCfg(ULONG ulLPort, ULONG ulQosTrustMode)
{
    

    DRV_CFGLOCK;
    g_astPortCfg[ulLPort].ulQosTrustMode = ulQosTrustMode;
     DRV_CFGUNLOCK;

    return NO_ERROR;
}

/*****************************************************************************
    Func Name: _CTC_GetUniQosTrustModeCfg
 
  Description: Get port qos trust mode cfg.
        Input: ULONG ulLPort           
                ULONG *pulQosTrustMode  
       Output: 
       Return: 
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
INT32 _CTC_GetUniQosTrustModeCfg(ULONG ulLPort, ULONG *pulQosTrustMode)
{
   
     DRV_CFGLOCK;
    *pulQosTrustMode = g_astPortCfg[ulLPort].ulQosTrustMode;
   DRV_CFGUNLOCK;

    return NO_ERROR;
}

/* 014453 */

/*****************************************************************************
    Func Name: CTC_DRV_SetTransparentVlan

  Description: Set ctc transparent vlan mode for port.
        Input: UINT32 uiLPortId
       Output:
       Return:
      Caution:
------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
  --------------------------------------------------------------------------

  
*****************************************************************************/
INT32 CTC_DRV_SetTransparentVlan(UINT32 uiLPortId)
{
    INT32 iRet = NO_ERROR;
    CTC_VLAN_CFG_S stVlanMode;

    if (!IsValidLgcPort(uiLPortId))
    {
        return ERROR;
    }

    iRet = _CTC_GetPortVlanModeCfg(uiLPortId, &stVlanMode);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
#if defined(CHIPSET_RTL8305)
    #if 0
    if (CTC_VLAN_MODE_TAG == stVlanMode.mode)
    {
        iRet = _CTC_ResetTagVlan(uiLPortId);
        if (NO_ERROR != iRet)
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }

        stVlanMode.mode = CTC_VLAN_MODE_TRANSPARENT;
    }
    else if (CTC_VLAN_MODE_TRANSLATION == stVlanMode.mode)
    {
        iRet = _CTC_ResetTranslationVlan(uiLPortId);
        if (NO_ERROR != iRet)
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }

        stVlanMode.mode = CTC_VLAN_MODE_TRANSPARENT;
        stVlanMode.number_of_entries = 0;
    }
    else if (CTC_VLAN_MODE_QINQ == stVlanMode.mode)
    {
        iRet = DRV_ResetPortQinq(uiLPortId);
        iRet += DRV_ResetQinqPvid(uiLPortId);
        if (NO_ERROR != iRet)
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }

        stVlanMode.mode = CTC_VLAN_MODE_TRANSPARENT;
    }

    /* Set the default VLAN point to vlan index 31.
    Vlan index 31 is default vlan. Its vlan id is 0.*/
    iRet = _CTC_AddReservedVlanMem(uiLPortId);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = Ioctl_ctc_port_ingress_vlan_filter(uiLPortId, FALSE);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = Ioctl_ctc_port_ingress_vlan_rcv_mod(uiLPortId, PORT_IN_FRAM_BOTH);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = Ioctl_ctc_port_egress_vlan_mode(uiLPortId, PORT_EG_TAG_MODE_REAL_KEEP);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /* Add acl rule. */
    iRet = Ioctl_ctc_add_acl_rule_for_transp(uiLPortId, stVlanMode);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = _CTC_AddVtuPortMem(uiLPortId, VLAN_ID_0, VLAN_MEM_UNTAG);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /*Add this transparent port to untransparent port's vtu entry.*/
    iRet = _CTC_RenewVtuMemForTranspPort(uiLPortId);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
    #else  
    if (CTC_VLAN_MODE_TAG == stVlanMode.mode)
    {
        iRet = Ioctl_ctc_del_acl_rule_for_tag(uiLPortId);
        if (DRV_OK != iRet)
        {
            diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }
    }
    else if (CTC_VLAN_MODE_TRANSLATION == stVlanMode.mode)
    {
        /*restore qos trust mode*/
        ULONG ulQosTrustMode;
        (void)_CTC_GetUniQosTrustModeCfg(uiLPortId, &ulQosTrustMode);
        /*when qos trust dscp, use dscp to queue frame and remap cos.*/
        if (QOS_DSCP != ulQosTrustMode)
        {
            if (DRV_OK != Ioctl_SetQosTrustMode(ulQosTrustMode))
            {
                diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
                return ERROR;
            }
        }
        
        iRet = Ioctl_ctc_del_acl_rule_for_transl(uiLPortId);
        if (DRV_OK != iRet)
        {
            diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }
        
        stVlanMode.number_of_entries = 0;
    }
    else if (CTC_VLAN_MODE_QINQ == stVlanMode.mode)
    {
        iRet = DRV_ResetPortQinq(uiLPortId);
        iRet += DRV_ResetQinqPvid(uiLPortId);
        if (NO_ERROR != iRet)
        {
            diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }
    }
    else if (CTC_VLAN_MODE_TRANSPARENT == stVlanMode.mode)
    {
        return NO_ERROR;
    }

    memset(&stVlanMode,0,sizeof(stVlanMode));
    stVlanMode.mode = CTC_VLAN_MODE_TRANSPARENT;
    iRet = Ioctl_ctc_add_acl_rule_for_transp(uiLPortId, stVlanMode);
    if (DRV_OK != iRet)
    {
        diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
    #endif
#endif
    return NO_ERROR;
}

/*****************************************************************************
   

*****************************************************************************/
INT32 CTC_DRV_ResetTransparentVlan(UINT32 uiLPortId)
{
    INT32 iRet = NO_ERROR;
    //CTC_VLAN_CFG_S stVlanMode;

    if (!IsValidLgcPort(uiLPortId))
    {
        return ERROR;
    }
#if 0
    iRet = _CTC_DelReservedVlanMem(uiLPortId);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /*Delete default vtu membership*/
    iRet = _CTC_DelVtuPortMem(uiLPortId, VLAN_ID_0);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = _CTC_DelVtuMemForTranspPort(uiLPortId);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = Ioctl_ctc_del_acl_rule_for_transp(uiLPortId);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
#else
    iRet = Ioctl_ctc_del_acl_rule_for_transp(uiLPortId);
    if (DRV_OK != iRet)
    {
        diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

#endif
    return NO_ERROR;
}
/*****************************************************************************
    
*****************************************************************************/
INT32 CTC_DRV_SetTagVlan(UINT32 uiLPortId, UINT32 uiPvid, UINT32 uiPriority)
{
    INT32 iRet = NO_ERROR;
    CTC_VLAN_CFG_S stVlanMode;
    #if 0
    UINT32 uiPortIndex = 0;
   
    logic_pmask_t stLMask, stUntagLMsk;
	UINT32 uiEnoughEntry = FALSE;
    #endif

    if ((!IsValidLgcPort(uiLPortId)) || (!VALID_VLAN_ID(uiPvid)))
    {
        return ERROR;
    }

    /*Check if other translation port's old vlan equals this pvid.*/
    iRet = _CTC_CheckVidForTagMode(uiLPortId, uiPvid);
    if (FALSE == iRet)
    {
        diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
    /* 014029 */
    /* 相同配置直接返回OK, ONU反复注册时会重复下发相同配置 */
    iRet = _CTC_GetPortVlanModeCfg(uiLPortId, &stVlanMode);
    if (NO_ERROR != iRet)
    {
        diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    if((CTC_VLAN_MODE_TAG == stVlanMode.mode) &&
        (stVlanMode.default_vlan == uiPvid) &&
        (stVlanMode.uiPriority == uiPriority))
    {
        return NO_ERROR;
    }
    /* 014029 */

#if 0
    /*Check wether there are enough acl rule for tag vlan or not.*/
    iRet = _CTC_GetPortVlanModeCfg(uiLPortId, &stVlanMode);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    if ((CTC_VLAN_MODE_TRANSPARENT == stVlanMode.mode) ||
        (CTC_VLAN_MODE_QINQ == stVlanMode.mode))
    {
        iRet = _CTC_HaveEnoughEmptyRules(1);
        if (FALSE == iRet)
        {
            mc_printf("\nThere are not enough acl rule for tag vlan.\n");
            return ERROR;
        }
    }

    /*Check if there is enough vlan index entry for tag mode.
    We do this because rtl8305mb uses vlan index entry to implement acl vlan 
    remarking action.
    In rtl8305mb there are 32 vlan index entries.*/
    stVlanMode.default_vlan = uiPvid;
    stVlanMode.mode = CTC_VLAN_MODE_TAG;
    stVlanMode.number_of_entries = 0;
    stVlanMode.uiPriority = uiPriority;
    iRet = Ioctl_ctc_enough_vlan_index_check(stVlanMode, &uiEnoughEntry);
    if ((NO_ERROR != iRet) || (FALSE == uiEnoughEntry))
    {
        mc_printf("func:%s,line:%d, bEnoughEntry = %d\n",__FUNCTION__,__LINE__,uiEnoughEntry);
        return ERROR;  
    }

    iRet = CTC_DRV_SetTransparentVlan(uiLPortId);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = CTC_DRV_ResetTransparentVlan(uiLPortId);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = _CTC_GetPortVlanModeCfg(uiLPortId, &stVlanMode);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /*Fill priority.*/
    uiPvid &= 0xFFFF0FFF;
    uiPvid |= (uiPriority << 12);
    iRet = Ioctl_SetVlanPvid(uiLPortId, uiPvid);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    ClrLgcMaskAll(&stLMask);
    SetLgcMaskBit(uiLPortId, &stLMask);
    SetLgcMaskBit(LOGIC_CPU_PORT, &stLMask);
    SetLgcMaskBit(LOGIC_PON_PORT, &stLMask);

    uiPvid &= 0x00000FFF;
    iRet = Ioctl_ctc_add_port_base_vlan_mem(uiPvid, &stLMask);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = Ioctl_ctc_port_ingress_vlan_filter(uiLPortId, TRUE);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = Ioctl_ctc_port_ingress_vlan_rcv_mod(uiLPortId, PORT_IN_FRAM_UNTAGGED_ONLY);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = Ioctl_ctc_port_egress_vlan_mode(uiLPortId, PORT_EG_TAG_MODE_ORI);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /* Add vtu entry. */
    ClrLgcMaskAll(&stLMask);
    ClrLgcMaskAll(&stUntagLMsk);

    SetLgcMaskBit(LOGIC_PON_PORT, &stLMask);

    LgcPortFor(uiPortIndex)
    {
        if (uiPortIndex == uiLPortId)
        {
            continue;
        }

        iRet = _CTC_GetPortVlanModeCfg(uiPortIndex, &stVlanMode);
        if (NO_ERROR != iRet)
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }

        if ((CTC_VLAN_MODE_TRANSPARENT == stVlanMode.mode) ||
            (CTC_VLAN_MODE_QINQ == stVlanMode.mode))
        {
            SetLgcMaskBit(uiPortIndex, &stLMask);
        }
    }

    SetLgcMaskBit(uiLPortId, &stLMask);
    SetLgcMaskBit(uiLPortId, &stUntagLMsk);
    /*We do not set priority to vtu entry.
      Maybe it is necessary to do that!?*/
    iRet = Ioctl_SetVlanMemberAdd(uiPvid, stLMask, stUntagLMsk);
    //iRet = VTU_AddMem(uiPvid, uiPriority, uiTagPortMask, uiUtagPortMask);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
#if 0
    /* Add acl cfg. */
    iRet = _CTC_GetPortVlanModeCfg(uiLPortId, &stVlanMode);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
#endif
    stVlanMode.default_vlan = uiPvid;
    stVlanMode.mode = CTC_VLAN_MODE_TAG;
    stVlanMode.number_of_entries = 0;
    stVlanMode.uiPriority = uiPriority;

    iRet = Ioctl_ctc_add_acl_rule_for_tag(uiLPortId, stVlanMode);
    //iRet = ACL_CreateRuleForCtcTagVlan(uiLPortId, uiPvid, &stVlanMode);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
#else
    iRet = CTC_DRV_SetTransparentVlan(uiLPortId);
    if (NO_ERROR != iRet)
    {
        diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = CTC_DRV_ResetTransparentVlan(uiLPortId);
    if (NO_ERROR != iRet)
    {
        diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    memset(&stVlanMode, 0, sizeof(stVlanMode));
    stVlanMode.default_vlan = uiPvid;
    stVlanMode.mode = CTC_VLAN_MODE_TAG;
    stVlanMode.number_of_entries = 0;
    stVlanMode.uiPriority = uiPriority;

    iRet = Ioctl_ctc_add_acl_rule_for_tag(uiLPortId, stVlanMode);
    if (DRV_OK != iRet)
    {
        diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

#endif

    return NO_ERROR;
}

/*****************************************************************************


*****************************************************************************/
INT32 CTC_DRV_SetTranslationVlan(UINT32 uiLPortId, UINT32 uiVlanTag,
                                 UINT32 *puiVlanList, UINT32 uiVlanListEntry)
{
    INT32 iRet = NO_ERROR;
    UINT32 i = 0;
    UINT32 uiPvid;
    UINT32 uiPriority;
    UINT32 uiOldVlan;
    UINT32 uiNewVlan;
    #if 0
    UINT32 uiEnoughEntry = FALSE;
    logic_pmask_t stLMask;
    #endif
    UINT32 uiQosTrustMode;
    CTC_VLAN_CFG_S stVlanMode;
    CTC_VLAN_CFG_S stOldVlanMode;

    if ((!IsValidLgcPort(uiLPortId)) || (NULL == puiVlanList))
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
#if 0
    iRet = CTC_DRV_GetVlanIdAndPri(uiVlanTag, &uiPvid, &uiPriority);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /*Check if other translation port's old vlan equals this port's but they dont have same new vlan
    or if other tag vlan port's pvid equals this port's old vlan.*/
    iRet = _CTC_CheckVlanForTranslationMode(uiLPortId, uiPvid, uiVlanListEntry, puiVlanList);
    if (FALSE == iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /*Check wether there are enough acl rules for translation vlan or not.*/
    iRet = _CTC_CheckAclForCtcTranslVlan(uiLPortId, uiPvid, puiVlanList, uiVlanListEntry);
    if (FALSE == iRet)
    {
        mc_printf("\nThere are not enough acl rule for translation vlan.\n");
        return ERROR;
    }

    /*Check if there is enough vlan index entry for translation mode.
    We do this because rtl8305mb uses vlan index entry to implement acl vlan 
    remarking action.
    In rtl8305mb there are 32 vlan index entries.*/
    stVlanMode.default_vlan = uiPvid;
    stVlanMode.mode = CTC_VLAN_MODE_TRANSLATION;
    stVlanMode.number_of_entries = 0;

    for (i = 0; i < uiVlanListEntry; i++)
    {
        uiOldVlan = puiVlanList[i*2];
        uiNewVlan = puiVlanList[(i*2) + 1];

        stVlanMode.number_of_entries++;
        stVlanMode.vlan_list[i*2] = uiOldVlan;
        stVlanMode.vlan_list[(i*2) + 1] = uiNewVlan;
    }
    iRet = Ioctl_ctc_enough_vlan_index_check(stVlanMode, &uiEnoughEntry);
    if ((NO_ERROR != iRet) || (FALSE == uiEnoughEntry))
    {
        mc_printf("func:%s,line:%d,ret:%d,eno:%d.\n",__FUNCTION__,__LINE__,
                   iRet,uiEnoughEntry);
        return ERROR;  
    }
    
    iRet = CTC_DRV_SetTransparentVlan(uiLPortId);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = CTC_DRV_ResetTransparentVlan(uiLPortId);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = _CTC_GetPortVlanModeCfg(uiLPortId, &stVlanMode);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /* Set port pvid */
    /*Fill priority.*/
    uiPvid &= 0xFFFF0FFF;
    uiPvid |= (uiPriority << 12);
    iRet = Ioctl_SetVlanPvid(uiLPortId, uiPvid);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    ClrLgcMaskAll(&stLMask);
    SetLgcMaskBit(uiLPortId, &stLMask);
    SetLgcMaskBit(LOGIC_CPU_PORT, &stLMask);
    SetLgcMaskBit(LOGIC_PON_PORT, &stLMask);

    uiPvid &= 0x00000FFF;
    iRet = Ioctl_ctc_add_port_base_vlan_mem(uiPvid, &stLMask);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = Ioctl_ctc_port_ingress_vlan_filter(uiLPortId, TRUE);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = Ioctl_ctc_port_ingress_vlan_rcv_mod(uiLPortId, PORT_IN_FRAM_BOTH);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = Ioctl_ctc_port_egress_vlan_mode(uiLPortId, PORT_EG_TAG_MODE_ORI);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /* Add vtu entry. */
    iRet = _CTC_AddVtuPortMemForCtcVlan(uiLPortId, uiPvid, uiPriority, VLAN_MEM_UNTAG);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /*save vlan mode configuration of this port*/
    stVlanMode.default_vlan = uiPvid;
    stVlanMode.mode = CTC_VLAN_MODE_TRANSLATION;
    stVlanMode.number_of_entries = 0;

    stVlanMode.uiPriority = uiPriority;

    for (i = 0; i < uiVlanListEntry; i++)
    {
        uiOldVlan = puiVlanList[i*2];
        uiNewVlan = puiVlanList[(i*2) + 1];

        stVlanMode.number_of_entries++;
        stVlanMode.vlan_list[i*2] = uiOldVlan;
        stVlanMode.vlan_list[(i*2) + 1] = uiNewVlan;

        /* For access vlan mode, downstream with pvid tag will be untagged egressing from uni port. */
        if ((uiOldVlan == uiNewVlan) &&
            (uiPvid == uiNewVlan))
        {
            continue;
        }

        iRet = _CTC_AddVtuPortMemForCtcVlan(uiLPortId, uiOldVlan, 0, VLAN_MEM_TAG);
        if (NO_ERROR != iRet)
        {
            mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }

        if (uiOldVlan != uiNewVlan)
        {
            iRet = _CTC_AddVtuPortMemForCtcVlan(uiLPortId, uiNewVlan, 0, VLAN_MEM_TAG);
            if (NO_ERROR != iRet)
            {
                mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
                return ERROR;
            }
        }
    }

    /* Add vlan changing acl rule for this port and uplink port. */
    iRet = Ioctl_ctc_add_acl_rule_for_transl(uiLPortId, stVlanMode);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
#if 0
    iRet = CFG_SetPortVlanMode(uiLPortId, &stVlanMode);
    if (NO_ERROR != iRet)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
#endif
#else
    iRet = CTC_DRV_GetVlanIdAndPri(uiVlanTag, &uiPvid, &uiPriority);
    if (NO_ERROR != iRet)
    {
        diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /*Check if other translation port's old vlan equals this port's but they dont have same new vlan
    or if other tag vlan port's pvid equals this port's old vlan.*/
    iRet = _CTC_CheckVlanForTranslationMode(uiLPortId, uiPvid, uiVlanListEntry, puiVlanList);
    if (FALSE == iRet)
    {
        diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = CTC_DRV_SetTransparentVlan(uiLPortId);
    if (NO_ERROR != iRet)
    {
        diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    iRet = CTC_DRV_ResetTransparentVlan(uiLPortId);
    if (NO_ERROR != iRet)
    {
        diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    (void)_CTC_GetUniQosTrustModeCfg((ULONG)uiLPortId, (ULONG *)&uiQosTrustMode);
    /*when qos trust dscp, use dscp to queue frame and remap cos.
    otherwise use priority in stag.*/
    if (QOS_DSCP != uiQosTrustMode)
    {
        if (DRV_OK != Ioctl_SetQosTrustMode(QOS_SVLAN))
        {
            diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }
    }
    
    /*save vlan mode configuration of this port*/
    memset(&stVlanMode, 0, sizeof(stVlanMode));
    stVlanMode.default_vlan = uiPvid;
    stVlanMode.uiPriority = uiPriority;
    stVlanMode.mode = CTC_VLAN_MODE_TRANSLATION;
    stVlanMode.number_of_entries = 0;

    for (i = 0; i < uiVlanListEntry; i++)
    {
        uiOldVlan = puiVlanList[i*2];
        uiNewVlan = puiVlanList[(i*2) + 1];

        stVlanMode.number_of_entries++;
        stVlanMode.vlan_list[i*2] = uiOldVlan;
        stVlanMode.vlan_list[(i*2) + 1] = uiNewVlan;
    }

    /* 014029 */
    /* 相同配置直接返回OK, ONU反复注册时会重复下发相同配置 */
    memset(&stOldVlanMode, 0, sizeof(stOldVlanMode));
    iRet = _CTC_GetPortVlanModeCfg(uiLPortId, &stOldVlanMode);
    if (NO_ERROR != iRet)
    {
        diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    if(0 == memcmp(&stOldVlanMode, &stVlanMode, sizeof(stVlanMode)))
    {
        return NO_ERROR;
    }
    /* 014029 */

    iRet = Ioctl_ctc_add_acl_rule_for_transl(uiLPortId, stVlanMode);
    if (DRV_OK != iRet)
    {
        diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

#endif
    return NO_ERROR;
}

/*****************************************************************************
   
*****************************************************************************/
INT32 CTC_DRV_GetVlanIdAndPri(UINT32 uiVlanTag, UINT32 *puiVlanId, UINT32 *puiPri)
{
    if ((NULL == puiVlanId) || (NULL == puiPri))
    {
        return ERROR;
    }

    *puiVlanId = uiVlanTag & 0x00000FFF;
    *puiPri    = (uiVlanTag >> 13) & 0x000007;

    return NO_ERROR;
}



/*****************************************************************************

*****************************************************************************/
INT32  _CTC_GetUniFlowCtrlCfg( ULONG ulLPort, BOOL *pbEnable )
{
  

    DRV_CFGLOCK;
    *pbEnable = (TRUE == g_astPortCfg[ulLPort].ucFlowCtrl)? TRUE : FALSE;
    DRV_CFGUNLOCK;

    return NO_ERROR;
}

/*****************************************************************************

*****************************************************************************/
INT32 _CTC_SetUniFlowCtrlCfg( ULONG ulLPort, BOOL bEnable )
{
    /* 检查输入参数合法性 */
  

    DRV_CFGLOCK;
    g_astPortCfg[ulLPort].ucFlowCtrl = (TRUE == bEnable)? TRUE : FALSE;
    DRV_CFGUNLOCK;

    return NO_ERROR;
}

/*****************************************************************************
 
*****************************************************************************/
INT32 _CTC_GetUniPortEnableCfg( ULONG ulLPort, BOOL *pbEnable )
{
  

    DRV_CFGLOCK;
    *pbEnable = (TRUE == g_astPortCfg[ulLPort].ucPortEnable)? TRUE : FALSE;
    DRV_CFGUNLOCK;

    return NO_ERROR;
}

/*****************************************************************************
 
*****************************************************************************/
INT32 _CTC_SetUniPortEnableCfg( ULONG ulLPort, BOOL bEnable )
{
    

    DRV_CFGLOCK;
    g_astPortCfg[ulLPort].ucPortEnable = (TRUE == bEnable)? TRUE : FALSE;
    DRV_CFGUNLOCK;

    return NO_ERROR;
}

/*****************************************************************************


*****************************************************************************/
INT32 DRV_GetPortVlanCfg(UINT32 uiLPortId, UINT8 *pucMode, UINT8 *pucVlanCfg, UINT16 *pusLength)
{
    CTC_VLAN_CFG_S stVlanCfg;
	UINT16 usLen = 0;
	UINT8 *pucTmp = NULL;
	UINT32 uiValue = 0;
	UINT32 i = 0;



	MEM_SET_ZERO(&stVlanCfg, sizeof(stVlanCfg));

    if(NO_ERROR != _CTC_GetPortVlanModeCfg(uiLPortId, &stVlanCfg))
    {
        return ERROR;
    }

	*pucMode = stVlanCfg.mode;
	pucTmp = pucVlanCfg;

	/* Length不包括模式 */
    switch ( stVlanCfg.mode )
	{
	    case CTC_VLAN_MODE_TRANSPARENT:
		{
	        *pusLength = usLen;
	        break;
	    }
	    case CTC_VLAN_MODE_TAG :
		{
	        uiValue = 0x81000000 | stVlanCfg.default_vlan;
	        HLTOBUF(pucTmp, uiValue);
			pucTmp += 4;
			usLen += 4;
			*pusLength = usLen;
	        break;
	    }
	    case CTC_VLAN_MODE_TRANSLATION:
		{
	        uiValue = 0x81000000 | stVlanCfg.default_vlan;
	        HLTOBUF(pucTmp, uiValue);
			pucTmp += 4;
			usLen += 4;
			for ( i = 0 ; (i < stVlanCfg.number_of_entries) && (i < CTC_VLAN_TRANSL_PAIR_NUM_MAX/2); i++ )
        	{
        	    uiValue = 0x81000000 | stVlanCfg.vlan_list[i];
				HLTOBUF(pucTmp, uiValue);
    			pucTmp += 4;
    			usLen += 4;
				uiValue = 0x81000000 | stVlanCfg.vlan_list[i+1];
				HLTOBUF(pucTmp, uiValue);
    			pucTmp += 4;
    			usLen += 4;
        	}
			*pusLength = usLen;
	        break;
	    }
	    default:
	        return ERROR;
	}

	return NO_ERROR;
}
/* 014043 */

INT32 _CTC_GetUniIsolateEnableCfg( ULONG ulLPort, BOOL *pbEnable )
{
   

     DRV_CFGLOCK;
    *pbEnable = (TRUE == g_astPortCfg[ulLPort].ucIsolateEnable)? TRUE : FALSE;
     DRV_CFGUNLOCK;

    return NO_ERROR;
}

INT32 _CTC_SetUniIsolateEnableCfg( ULONG ulLPort, BOOL bEnable )
{
  

    DRV_CFGLOCK;
    g_astPortCfg[ulLPort].ucIsolateEnable = (TRUE == bEnable)? TRUE : FALSE;
     DRV_CFGUNLOCK;

    return NO_ERROR;
}

INT32 _CTC_GetUniSpeedCfg( ULONG ulLPort, UCHAR *pucSpeed )
{


    DRV_CFGLOCK;
    *pucSpeed = g_astPortCfg[ulLPort].ucSpeed;
    DRV_CFGUNLOCK;

    return NO_ERROR;
}

INT32 _CTC_SetUniSpeedCfg( ULONG ulLPort, UCHAR ucSpeed )
{
    UCHAR ucTempSpeed;
  

    switch(ucSpeed)
    {
        case SPEED_AUTO:
            ucTempSpeed = PORT_SPEED_AUTO;
            break;
        case SPEED_10M:
            ucTempSpeed = PORT_SPEED_10;
            break;
        case SPEED_100M:
            ucTempSpeed = PORT_SPEED_100;
            break;
        case SPEED_1000M:
            ucTempSpeed = PORT_SPEED_1000;
            break;
        default:
            return ERROR;
    }
     DRV_CFGLOCK;
    g_astPortCfg[ulLPort].ucSpeed = ucTempSpeed;
    DRV_CFGUNLOCK;

    return NO_ERROR;
}

INT32 _CTC_GetUniDuplexCfg( ULONG ulLPort, UCHAR *pucDuplex )
{
  
    DRV_CFGLOCK;
    *pucDuplex = g_astPortCfg[ulLPort].ucDuplex;
     DRV_CFGUNLOCK;

    return NO_ERROR;
}

INT32 _CTC_SetUniDuplexCfg( ULONG ulLPort, UCHAR ucDuplex )
{
    UCHAR ucTempDuplex;


    switch(ucDuplex)
    {
        case DUPLEX_AUTO:
            ucTempDuplex = PORT_DUPLEX_AUTO;
            break;
        case DUPLEX_FULL:
            ucTempDuplex = PORT_DUPLEX_FULL;
            break;
        case DUPLEX_HALF:
            ucTempDuplex= PORT_DUPLEX_HALF;
            break;
        default:
            return ERROR;
    }
     DRV_CFGLOCK;
    g_astPortCfg[ulLPort].ucDuplex = ucTempDuplex;
    DRV_CFGUNLOCK;

    return NO_ERROR;
}

INT32 _CTC_GetUniMdiCfg( ULONG ulLPort, UCHAR *pucMdi )
{
   

  DRV_CFGLOCK;
    *pucMdi = g_astPortCfg[ulLPort].ucMdi;
    DRV_CFGUNLOCK;

    return NO_ERROR;
}

INT32 _CTC_SetUniMdiCfg( ULONG ulLPort, UCHAR ucMdi )
{
    
    DRV_CFGLOCK;
    g_astPortCfg[ulLPort].ucMdi = ucMdi;
    DRV_CFGUNLOCK;

    return NO_ERROR;
}

INT32 _CTC_GetUniAutoNegModeCfg( ULONG ulLPort, BOOL *pbAutoNegMode )
{
  

   DRV_CFGLOCK;
    *pbAutoNegMode = (TRUE == g_astPortCfg[ulLPort].bAutoNegMode)? TRUE : FALSE;
    DRV_CFGUNLOCK;

    return NO_ERROR;
}

INT32 _CTC_SetUniAutoNegModeCfg( ULONG ulLPort, BOOL bAutoNegMode )
{
    

     DRV_CFGLOCK;
    g_astPortCfg[ulLPort].bAutoNegMode = (TRUE == bAutoNegMode)? TRUE : FALSE;
      DRV_CFGUNLOCK;

    return NO_ERROR;
}

INT32 _CTC_GetUniLineRateInCfg( ULONG ulLPort, ULONG *pulLineRateIn )
{


      DRV_CFGLOCK;
    *pulLineRateIn = g_astPortCfg[ulLPort].ulLineRateIn;
     DRV_CFGUNLOCK;

    return NO_ERROR;
}

INT32 _CTC_SetUniLineRateInCfg( ULONG ulLPort, ULONG ulLineRateIn )
{
  

    DRV_CFGLOCK;
    g_astPortCfg[ulLPort].ulLineRateIn = ulLineRateIn;
  DRV_CFGUNLOCK;

    return NO_ERROR;
}

INT32 _CTC_GetUniLineRateOutCfg( ULONG ulLPort, ULONG *pulLineRateOut )
{
 

    DRV_CFGLOCK;
    *pulLineRateOut = g_astPortCfg[ulLPort].ulLineRateOut;
    DRV_CFGUNLOCK;

    return NO_ERROR;
}

INT32 _CTC_SetUniLineRateOutCfg( ULONG ulLPort, ULONG ulLineRateOut )
{
    

   DRV_CFGLOCK;
    g_astPortCfg[ulLPort].ulLineRateOut = ulLineRateOut;
      DRV_CFGUNLOCK;

    return NO_ERROR;
}

/* 014043 */
#endif
/*****************************************************************************
  

*****************************************************************************/
INT32 DRV_ResetAllPortsStatistic(VOID)
{
    #if !defined(CONFIG_PRODUCT_EPN200)
    UINT32 i;
    INT32  iRet = NO_ERROR;

    /*Reset statistic of switch ports.*/
    for (i = 1; i < MAX_PORT_NUM; i++)
    {
        iRet = DRV_ResetPortStatistic(i);
        if (NO_ERROR != iRet)
        {
            return ERROR;
        }
    }
    #endif

    /*Reset statistic of ge port in opulan chip.*/
    (VOID)CntClr(CNT_GE, 0, 0);

    /*Reset statistic of pon port in opulan chip.*/
    (VOID)CntClr(CNT_PON, 0, 0);

	OamClrPeirodStats();

    return NO_ERROR;
}


INT32 DRV_GetPortMibCounter(UINT32 lport, UNI_COUNTER_S*  pMibCtrs)
{
#if !defined(CONFIG_PRODUCT_EPN200)
    int ret;
	UINT64 ullValue = 0;

    if(NULL == pMibCtrs)
    {
        return ERROR;
    }

    if(FALSE == IsValidLgcPort(lport))
    {
        return ERROR;
    }

    ret = DRV_OK;

    ret+=Ioctl_GetPortStatisticsByPort(lport, TxOctets, &ullValue);
	pMibCtrs->uiTransOctets = ullValue;
    ret+=Ioctl_GetPortStatisticsByPort(lport, RxOctets, &ullValue);
	pMibCtrs->uiRecOctets = ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxGoodOctets, &ullValue);
    pMibCtrs->ulRevBadOctets = (ULONG)(pMibCtrs->uiRecOctets - ullValue);

    ret+=Ioctl_GetPortStatisticsByPort(lport, RxUnicastPkts, &ullValue);
	pMibCtrs->InUnicast = (ULONG)ullValue;
    ret+=Ioctl_GetPortStatisticsByPort(lport, TxUnicastPkts, &ullValue);
	pMibCtrs->OutUnicast = (ULONG)ullValue;
    ret+=Ioctl_GetPortStatisticsByPort(lport, RxMulticastPkts, &ullValue);
	pMibCtrs->InMulticast = (ULONG)ullValue;
    ret+=Ioctl_GetPortStatisticsByPort(lport, TxMulticastPkts, &ullValue);
	pMibCtrs->OutMulticast = (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxBroadcastPkts, &ullValue);
	pMibCtrs->InBroadcast = (ULONG)ullValue;
    ret+=Ioctl_GetPortStatisticsByPort(lport, TxBroadcastPkts, &ullValue);
    pMibCtrs->OutBroadcast = (ULONG)ullValue;

	ret+=Ioctl_GetPortStatisticsByPort(lport, RxPausePkts, &ullValue);
	pMibCtrs->InPause = (ULONG)ullValue;
    ret+=Ioctl_GetPortStatisticsByPort(lport, TxPausePkts, &ullValue);
	pMibCtrs->OutPause = (ULONG)ullValue;

    /* 014538 */
    #if 0
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxOversizePkts, &ullValue);
	pMibCtrs->InOverflow = (ULONG)ullValue;
    #else
    pMibCtrs->InOverflow = 0;
    #endif
    /* 014538 */
    ret+=Ioctl_GetPortStatisticsByPort(lport, RxUdersizePkts, &ullValue);

	pMibCtrs->InUnderflow = (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxFCSErrors, &ullValue);
	pMibCtrs->InCrcError = (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxFragment, &ullValue);
	pMibCtrs->InUnderflow += (ULONG)ullValue;

	ret+=Ioctl_GetPortStatisticsByPort(lport, TxSingleCollision, &ullValue);
	pMibCtrs->OutCollision = (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, TxLateCollision, &ullValue);
	pMibCtrs->OutLate = (ULONG)ullValue;
    ret+=Ioctl_GetPortStatisticsByPort(lport, RxDropPkts, &ullValue);
    pMibCtrs->ulInDropped = (ULONG)ullValue;
    if(DRV_OK == ret){
        return NO_ERROR;
    }
    else{
        return ERROR;
    }
#else
    int iRet;
    UINT64 ullValue = 0;
    UINT64 ullValueTmp = 0;
    OPL_CNT_t oplCounter[DAL_CNT_GE_NUM];

    if(NULL == pMibCtrs)
    {
        return ERROR;
    }

    if(FALSE == IsValidLgcPort(lport))
    {
        return ERROR;
    }

    iRet = dalCounterGet(DAL_CNT_POLL_GE_MASK, oplCounter, 0, DAL_CNT_GE_NUM);
    if (0 != iRet)
    {
        return ERROR;
    }
    /*Forlowing mibs refer to parameter g_astCntGe.*/
    /*total tx bytes*/
	pMibCtrs->uiTransOctets = ((UINT64)(oplCounter[11].stAdded.uiHigh) << 32) | (oplCounter[11].stAdded.uiLow);
    /*total rx bytes*/
	pMibCtrs->uiRecOctets = ((UINT64)(oplCounter[34].stAdded.uiHigh) << 32) | oplCounter[34].stAdded.uiLow;
	/*rx bad bytes*/
    pMibCtrs->ulRevBadOctets = 0;

    /*rx unicast*/        /*rx total - rx broadcast - rx multicast*/
    ullValue = ((UINT64)(oplCounter[20].stAdded.uiHigh) << 32) | oplCounter[20].stAdded.uiLow;
    ullValue += (((UINT64)(oplCounter[21].stAdded.uiHigh) << 32) | oplCounter[21].stAdded.uiLow);
    ullValueTmp = (((UINT64)(oplCounter[19].stAdded.uiHigh) << 32) | oplCounter[19].stAdded.uiLow);
    pMibCtrs->InUnicast = (UINT32)(ullValueTmp - ullValue);
	                      
    /*tx unicast*/        /*tx total - tx broadcast - tx multicast*/
    ullValue = (((UINT64)(oplCounter[1].stAdded.uiHigh) << 32) | oplCounter[1].stAdded.uiLow);
    ullValue += (((UINT64)(oplCounter[2].stAdded.uiHigh) << 32) | oplCounter[2].stAdded.uiLow);
    ullValueTmp = (((UINT64)(oplCounter[0].stAdded.uiHigh) << 32) | oplCounter[0].stAdded.uiLow);
	pMibCtrs->OutUnicast = (UINT32)(ullValueTmp - ullValue);
    
    /*rx multicast*/
	pMibCtrs->InMulticast = oplCounter[21].stAdded.uiLow;
    /*tx multicast*/
	pMibCtrs->OutMulticast = oplCounter[2].stAdded.uiLow;
	/*rx broadcast*/
	pMibCtrs->InBroadcast = oplCounter[20].stAdded.uiLow;
    /*tx broadcast*/
    pMibCtrs->OutBroadcast = oplCounter[1].stAdded.uiLow;

	/*rx pause*/
	pMibCtrs->InPause = oplCounter[22].stAdded.uiLow;
    /*tx pause*/
	pMibCtrs->OutPause = oplCounter[3].stAdded.uiLow;

	/*rx oversize*/
	pMibCtrs->InOverflow = oplCounter[26].stAdded.uiLow;
    /*rx undersize*/
	pMibCtrs->InUnderflow = oplCounter[25].stAdded.uiLow;
	/*rx FCS error*/
	pMibCtrs->InCrcError = oplCounter[15].stAdded.uiLow;
	/*rx fragment*/
	pMibCtrs->InUnderflow += oplCounter[17].stAdded.uiLow;

	/*tx single collision*/
	pMibCtrs->OutCollision = 0;
    /*tx late collision*/
	pMibCtrs->OutLate = 0;
    /*rx drop*/
    pMibCtrs->ulInDropped = oplCounter[14].stAdded.uiLow;

    return NO_ERROR;
#endif
}

/*****************************************************************************
 

*****************************************************************************/
INT32 DRV_GetPortLinkStatus(UINT32 lport, BOOL* pStatus)
{
    unsigned int linksts = 0;

    if(NULL == pStatus){
        return ERROR;
    }

    if(FALSE == IsValidLgcPort(lport)) {
        return ERROR;
    }
#if !defined(CONFIG_PRODUCT_EPN200)
    if(DRV_OK == Ioctl_GetPortCurrentLink(lport, &linksts)){
        switch (linksts)
        {
            case TRUE :
                *pStatus = TRUE;
                break;
            case FALSE :
                *pStatus = FALSE;
                break;
            default:
                return ERROR;
        }
    }
#else
    linksts = linksts;
    *pStatus = TRUE;
#endif
    return NO_ERROR;
}

/*****************************************************************************
 

*****************************************************************************/
INT32 DRV_GetPortDuplex(UINT32 lport, PORT_DUPLEX_E* pDuplexMode)
{
    unsigned int duplex = 0;

    if(NULL == pDuplexMode){
        return ERROR;
    }

    if(FALSE == IsValidLgcPort(lport)) {
        return ERROR;
    }
#if !defined(CONFIG_PRODUCT_EPN200)
    if(DRV_OK == Ioctl_GetPortDuplex(lport, &duplex)){
        switch (duplex)
        {
            case PORT_DUPLEX_FULL :
                *pDuplexMode = DUPLEX_FULL;
                break;
            case PORT_DUPLEX_HALF :
                *pDuplexMode = DUPLEX_HALF;
                break;
            default:
                return ERROR;
        }
    }
#else
    duplex = duplex;
    *pDuplexMode = DUPLEX_FULL;
#endif
    return NO_ERROR;
}

/*****************************************************************************


*****************************************************************************/
INT32 DRV_GetPortSpeed(UINT32 lport, PORT_SPEED_E* pSpeedMode)
{
    unsigned int speed = 0;

    if(NULL == pSpeedMode){
        return ERROR;
    }

    if(FALSE == IsValidLgcPort(lport)) {
        return ERROR;
    }
#if !defined(CONFIG_PRODUCT_EPN200)
    if(DRV_OK == Ioctl_GetPortSpeed(lport, &speed)){
        switch (speed)
        {
            case PORT_SPEED_10 :
                *pSpeedMode = SPEED_10M;
                break;
            case PORT_SPEED_100 :
                *pSpeedMode = SPEED_100M;
                break;
            case PORT_SPEED_1000 :
                *pSpeedMode = SPEED_1000M;
                break;
            default:
                return ERROR;
        }
    }
#else
    speed = speed;
    *pSpeedMode = SPEED_1000M;
#endif
    return NO_ERROR;
}

/*****************************************************************************
 

*****************************************************************************/
INT32 DRV_GetPortMdiStatus(UINT32 lport,  PORT_MDI_E* pMdiMode)
{
    unsigned int mdix = 0;

    if(NULL == pMdiMode){
        return ERROR;
    }

    if(FALSE == IsValidLgcPort(lport)) {
        return ERROR;
    }
#if !defined(CONFIG_PRODUCT_EPN200)
    if(DRV_OK == Ioctl_GetPortMdix(lport, &mdix)){
        switch (mdix)
        {
            case PORT_MDI_NORMAL :
                *pMdiMode = MDI_NORMAL;
                break;
            case PORT_MDI_ACROSS :
                *pMdiMode = MDI_ACROSS;
                break;
            default:
                return ERROR;
        }
    }
#else
    mdix = mdix;
    *pMdiMode = MDI_NORMAL;
#endif
    return NO_ERROR;
}

/*****************************************************************************


*****************************************************************************/
INT32 DRV_ResetPortStatistic(UINT32 lport)
{
    if(FALSE == IsValidLgcPort(lport)) {
        return ERROR;
    }
#if !defined(CONFIG_PRODUCT_EPN200)
    if(DRV_OK == Ioctl_SetMibCountReset(lport)){
        return NO_ERROR;
    }
    else{
        return ERROR;
    }
#else
    /*Reset statistic of ge port in opulan chip.*/
    (VOID)CntClr(CNT_GE, 0, 0);
    return NO_ERROR;
#endif
}

/*****************************************************************************
 

*****************************************************************************/
int get_spdlx_by_new_speed(PORT_SPEED_E enPortSpeed, int* spdlx_val)
{
    int duplex_tmp;

    duplex_tmp = (*spdlx_val % 3);

    switch (enPortSpeed)
    {
        case SPEED_AUTO :
            *spdlx_val = (3*3) + duplex_tmp;
            break;
        case SPEED_10M :
            *spdlx_val = (0*3) + duplex_tmp;
            break;
        case SPEED_100M :
            *spdlx_val = (1*3) + duplex_tmp;
            break;
        case SPEED_1000M :
            *spdlx_val = (2*3) + duplex_tmp;
            break;
        default:
            return -1;
    }

    return 0;
}

/*****************************************************************************


*****************************************************************************/
int get_spdlx_by_new_duplex(PORT_DUPLEX_E enPortDuplex, int* spdlx_val)
{
    int speed_tmp;

    speed_tmp = (*spdlx_val / 3);

    switch (enPortDuplex)
    {
        case DUPLEX_AUTO :
            *spdlx_val = (speed_tmp*3) + 2;
            break;
        case DUPLEX_FULL :
            *spdlx_val = (speed_tmp*3) + 1;
            break;
        case DUPLEX_HALF :
            *spdlx_val = (speed_tmp*3) + 0;
            break;
        default:
            return -1;
    }

    return 0;

}

/*****************************************************************************
 

*****************************************************************************/
INT32 DRV_SetPortSpeed(UINT32 lport, PORT_SPEED_E enPortSpeed)
{
#if !defined(CONFIG_PRODUCT_EPN200)
    int spdlx_val;

    if(FALSE == IsValidLgcPort(lport)) {
        return ERROR;
    }

    if(DRV_OK != Ioctl_GetSpdlxSet(lport, &spdlx_val)) {
        return ERROR;
    }

    if(-1 == get_spdlx_by_new_speed(enPortSpeed, &spdlx_val)) {
        return ERROR;
    }

    if(DRV_OK == Ioctl_SetPortSpdlx(lport, spdlx_val)){
        (VOID)_CTC_SetUniSpeedCfg(lport, enPortSpeed);/* 014043 */
        return NO_ERROR;
    }
    else{
        return ERROR;
    }
#else
    lport = lport;
    enPortSpeed = enPortSpeed;
    return NO_ERROR;
#endif
}

/*****************************************************************************
 

*****************************************************************************/
INT32 DRV_SetPortDuplex(UINT32 lport, PORT_DUPLEX_E enPortDuplex)
{
#if !defined(CONFIG_PRODUCT_EPN200)
    int spdlx_val;

    if(FALSE == IsValidLgcPort(lport)) {
        return ERROR;
    }

    if(DRV_OK != Ioctl_GetSpdlxSet(lport, &spdlx_val)) {
        return ERROR;
    }

    if(-1 == get_spdlx_by_new_duplex(enPortDuplex, &spdlx_val)) {
        return ERROR;
    }

    if(DRV_OK == Ioctl_SetPortSpdlx(lport, spdlx_val)){
        (VOID)_CTC_SetUniDuplexCfg(lport, enPortDuplex);/* 014043 */
        return NO_ERROR;
    }
    else{
        return ERROR;
    }
#else
    lport = lport;
    enPortDuplex = enPortDuplex;
    return NO_ERROR;
#endif
}


/*****************************************************************************


*****************************************************************************/
INT32 DRV_SetPortIsolation(UINT32 lport,  BOOL bEnable)
{
    logic_pmask_t lmask;

    if(FALSE == IsValidLgcPort(lport)) {
        return ERROR;
    }
#if !defined(CONFIG_PRODUCT_EPN200)
    if(DRV_OK != Ioctl_GetIsolateSet(&lmask)) {
        return ERROR;
    }

    if(TRUE == bEnable) {
        SetLgcMaskBit(lport, &lmask);
    }
    else if(FALSE == bEnable){
        ClrLgcMaskBit(lport, &lmask);
    }
    else{
        return ERROR;
    }

    if(DRV_OK != Ioctl_SetPortIsolate(lmask)) {
        return ERROR;
    }
    else{
        (VOID)_CTC_SetUniIsolateEnableCfg(lport, bEnable);/* 014043 */
        return NO_ERROR;
    }
#else
    memset(&lmask, 0, sizeof(lmask));   
    bEnable = bEnable;
    return ERROR;
#endif
}

/*****************************************************************************
  
*****************************************************************************/
INT32 DRV_SetPortFlowCtrl(UINT32 lport,  BOOL bEnable)
{
    BOOL bCfgEnable = FALSE;

    if(FALSE == IsValidLgcPort(lport)) {
        return ERROR;
    }

	if((FALSE != bEnable) && (TRUE != bEnable)) {
        return ERROR;
    }
#if !defined(CONFIG_PRODUCT_EPN200)
	(VOID)_CTC_GetUniFlowCtrlCfg(lport, &bCfgEnable);
	if ( bEnable == bCfgEnable)
	{
	    return NO_ERROR;
	}

	if(DRV_OK == Ioctl_SetPortPause(lport, bEnable, bEnable))
	{
		(VOID)_CTC_SetUniFlowCtrlCfg(lport, bEnable);
        return NO_ERROR;
    }
    else{
        return ERROR;
    }
#else
    bCfgEnable = bCfgEnable;
    return NO_ERROR;
#endif
}

/*****************************************************************************
 

*****************************************************************************/
INT32 DRV_SetPortLineRate(UINT32 lport,  PORT_DIRECT_E enDirect, UINT32 uiLineRate)
{
#if !defined(CONFIG_PRODUCT_EPN200)
    logic_pmask_t lmask;
	UINT32 uiDirection = DRV_RATELIMIT_NONE;

	
	if (0 != (uiLineRate % MIN_PORT_BANDWIDTH))
    {
        return ERROR;
    }

	switch ( enDirect )
	{
	    case DIRECT_IN:
	        uiDirection = DRV_RATELIMIT_RX;
	        break;
	    case DIRECT_OUT:
	        uiDirection = DRV_RATELIMIT_TX;
	        break;
	    default:
	        return ERROR;
	}

    if ((MAX_PORT_BANDWIDTH == uiLineRate) ||
        (0 == uiLineRate))
    {
        uiLineRate = 0;
    }

    /* 014085 */
    memset(&lmask, 0, sizeof(lmask));
	SetLgcMaskBit(lport, &lmask);

	if(DRV_OK == Ioctl_SetRateLimitPort(uiDirection, lmask, uiLineRate)){
        /* 014043 */
        if(DRV_RATELIMIT_RX == uiDirection)
        {
            _CTC_SetUniLineRateInCfg( lport, uiLineRate );
        }
        else
        {
            _CTC_SetUniLineRateOutCfg( lport, uiLineRate );
        }
        /* 014043 */
        return NO_ERROR;
    }
    else{
        return ERROR;
    }
#else
   
	if (0 != (uiLineRate % MIN_PORT_BANDWIDTH))
    {
        return ERROR;
    }
    return ERROR;
#endif
}

/*****************************************************************************
 
*****************************************************************************/
INT32 DRV_SetPortLinkState(UINT32 uiLPortId, BOOL bPortStatus)
{
#if !defined(CONFIG_PRODUCT_EPN200)
    BOOL bCfgEnable = FALSE;

    
	(VOID)_CTC_GetUniPortEnableCfg(uiLPortId, &bCfgEnable);
	if ( bPortStatus == bCfgEnable)
	{
	    return NO_ERROR;
	}

    if(DRV_OK == Ioctl_SetPortEnable(uiLPortId, bPortStatus))
	{
	    (VOID)_CTC_SetUniPortEnableCfg(uiLPortId, bPortStatus);
        return NO_ERROR;
    }
    else{
        return ERROR;
    }
#else
  
    return ERROR;
#endif
}

/*****************************************************************************
 
*****************************************************************************/
INT32 DRV_PortRestartNeg(UINT32 uiLPortId)
{

#if !defined(CONFIG_PRODUCT_EPN200)
    if(DRV_OK == Ioctl_ctc_restart_autoneg(uiLPortId)){
         (VOID)_CTC_SetUniAutoNegModeCfg(uiLPortId, TRUE);/* 014043 */
         return NO_ERROR;
    }
    else{
        return ERROR;
    }
#else
    return NO_ERROR;
#endif
}

/*****************************************************************************
  --------------------------------------------------------------------------

*****************************************************************************/
INT32 DRV_SetPortAutoNeg(UINT32 uiLPortId, UINT8 ucAction)
{
#if !defined(CONFIG_PRODUCT_EPN200)
    PORT_SPEED_E enSpeedMode = SPEED_AUTO;
    PORT_DUPLEX_E enDuplexMode = DUPLEX_AUTO;
	int spdlx_val;


    if (m_abAutoNegEnable[uiLPortId] == ucAction)
    {
        return NO_ERROR;
    }

    /* 使能/关闭端口自协商 */
    if (TRUE == ucAction)
    {
        enSpeedMode  = SPEED_AUTO;
        enDuplexMode = DUPLEX_AUTO;
    }
    else
    {
        /* 千兆应该工作在自协商模式下，因此强制时速率降到100M */
        enSpeedMode  = SPEED_100M;
        enDuplexMode = DUPLEX_FULL;
    }

	if(-1 == get_spdlx_by_new_speed(enSpeedMode, &spdlx_val)) {
        return ERROR;
    }

	if(-1 == get_spdlx_by_new_duplex(enDuplexMode, &spdlx_val)) {
        return ERROR;
    }

    if(DRV_OK != Ioctl_SetPortSpdlx(uiLPortId, spdlx_val))
    {
        return ERROR;
    }

    _CTC_SetUniDuplexCfg(uiLPortId, enDuplexMode);
    _CTC_SetUniSpeedCfg(uiLPortId, enSpeedMode);
    (VOID)_CTC_SetUniAutoNegModeCfg(uiLPortId, ucAction);/* 014043 */
#else
    
  
#endif
    m_abAutoNegEnable[uiLPortId] = ucAction;

    return NO_ERROR;
}

/*****************************************************************************

*****************************************************************************/
INT32 DRV_SetPriTrustMode(QOS_PRI_TRUST_MODE_E enTrustMode)
{
#if !defined(CONFIG_PRODUCT_EPN200)
	UINT32 i;
    UINT32 trustMode=QOS_COS;

    switch (enTrustMode)
    {
        case QOS_PRI_TRUST_MODE_IP_TOS :
            return ERROR;
        case QOS_PRI_TRUST_MODE_DOT1P :
            trustMode = QOS_COS;
            break;
        case QOS_PRI_TRUST_MODE_DSCP :
            trustMode = QOS_DSCP;
            break;
        case QOS_PRI_TRUST_MODE_PORT :
            trustMode = QOS_PORT;
            break;
        default:
            return ERROR;
    }

    /* */
    if(DRV_OK != Ioctl_SetQosTrustMode(trustMode)){
        return ERROR;
    }
    
    for (i = 1; i <= LOGIC_PORT_NO; i++)
    {
        (void)_CTC_SetUniQosTrustModeCfg(i, trustMode);
    }

    return NO_ERROR;
    /*014453 */
#else
    enTrustMode = enTrustMode;
    return ERROR;
#endif
}

/*****************************************************************************

*****************************************************************************/
INT32 DRV_SetPriSchedMode(QOS_QUEUE_SCHEDULER_MODE_E enPriSchedMode)
{
#if !defined(CONFIG_PRODUCT_EPN200)
    QueueMode_S stQueue;

    switch (enPriSchedMode)
    {
        case QOS_QUEUE_SCHEDULER_MODE_WFQ :
            stQueue.QueueMode = QUEUE_MODE_WFQ;
            /* 013997 */
            #if 0
			stQueue.Weight[0] = 8;
            stQueue.Weight[1] = 4;
            stQueue.Weight[2] = 2;
            stQueue.Weight[3] = 1;
            #else
            stQueue.Weight[0] = 1;
            stQueue.Weight[1] = 2;
            stQueue.Weight[2] = 4;
            stQueue.Weight[3] = 8;
            #endif
            /* 013997 */
            break;
        case QOS_QUEUE_SCHEDULER_MODE_SPQ :
            stQueue.QueueMode = QUEUE_MODE_HQ_WRR;
			stQueue.Weight[0] = 0;
            stQueue.Weight[1] = 0;
            stQueue.Weight[2] = 0;
            stQueue.Weight[3] = 0;
            break;
        default:
            return ERROR;
    }    

    if(DRV_OK != Ioctl_SetQosQueueSchedul(stQueue)){
        return ERROR;
    }
#else
    UINT8 ucQid;
    UINT8 ucCosIndex;
    UINT8 auCos2Qid[NUMBER_OF_DOT1P] = {0,0,1,1,2,2,3,3};
    OPL_STATUS Ret;
    extern OPL_STATUS tmQueuePriHwWrite(UINT8 stream, UINT8 Qid, UINT8 priority);
    extern OPL_STATUS tmQueueWeightHwWrite(UINT8 stream, UINT8 Qid, UINT32 weight);

    /*The queues whose priority are different are scheduled using sp mode.*/
    if (QOS_QUEUE_SCHEDULER_MODE_SPQ == enPriSchedMode)
    {
        for (ucQid = 0; ucQid < 4; ucQid++)
        {
            Ret = tmQueuePriHwWrite(UP_STREAM, ucQid, ucQid);
            Ret += tmQueueWeightHwWrite(UP_STREAM, ucQid, 0);
            Ret += tmQueuePriHwWrite(DOWN_STREAM, ucQid, ucQid);
            Ret += tmQueueWeightHwWrite(DOWN_STREAM, ucQid, 0);
            if (OPL_OK != Ret)
            {
                return ERROR;
            }
        }

        for (ucCosIndex = 0; ucCosIndex < NUMBER_OF_DOT1P; ucCosIndex++)
        {
            Ret = odmQosCos2QMapSet(ucCosIndex, auCos2Qid[ucCosIndex]);
            if (NO_ERROR != Ret)
            {
                return ERROR;
            }
        }
    }
    /*The queues whose priority are same are scheduled using wrr mode.*/
    else
    {
        UINT8 aucWeght[4] = {8,4,2,1};
        for (ucQid = 0; ucQid < 4; ucQid++)
        {
            Ret = tmQueuePriHwWrite(UP_STREAM, ucQid, 0);
            //Ret += tmQueueWeightHwWrite(UP_STREAM, ucQid, (1U) << ucQid);
            Ret += tmQueueWeightHwWrite(UP_STREAM, ucQid, aucWeght[ucQid]);
            Ret += tmQueuePriHwWrite(DOWN_STREAM, ucQid, 0);
            //Ret += tmQueueWeightHwWrite(DOWN_STREAM, ucQid, (1U) << ucQid);
            Ret += tmQueueWeightHwWrite(DOWN_STREAM, ucQid, aucWeght[ucQid]);
            if (OPL_OK != Ret)
            {
                return ERROR;
            }
        }

        //(void)tmQueueWeightHwWrite(UP_STREAM, 7, 8);
    }
#endif
    return NO_ERROR;
}

/*****************************************************************************

*****************************************************************************/
INT32 DRV_SetCos2QidMap(UINT32* auiCos2QidMap)
{
    qos_8021p_to_intpri_t st1pMap;
    int ii;

    for ( ii = 0 ; ii < NUMBER_OF_DOT1P ; ii++ ) {
        st1pMap.internalPri[ii] = (unsigned char)auiCos2QidMap[ii];
    }
#if !defined(CONFIG_PRODUCT_EPN200)
    if(DRV_OK == Ioctl_SetQos2Qid(st1pMap)){
        return NO_ERROR;        
    }
    else{
        return ERROR;
    }
#else
    extern int odmQosCos2QMapSet(UINT8 VlanCos,UINT8 QueId);
    int iRet = 0;
    UINT8 uiCosIndex;

    for (uiCosIndex = 0; uiCosIndex < NUMBER_OF_DOT1P; uiCosIndex++)
    {
        iRet += odmQosCos2QMapSet(uiCosIndex, st1pMap.internalPri[uiCosIndex]);
    }

    if (NO_ERROR != iRet)
    {
        return ERROR;
    }
    
    return NO_ERROR;
#endif
}

INT32 DRV_SetStpEnable(BOOL ulEnable)
{
    (void)ulEnable;
     return 0;
}

/*****************************************************************************
    Func Name: _DRV_IsUniCastIp
  --------------------------------------------------------------------------

*****************************************************************************/
BOOL_T _DRV_IsUniCastIp(ULONG ulIp)
{
    UCHAR aucIp[4] = { 0 };
    UCHAR aucZeroIp[4] = { 0, 0, 0, 0 };
    UCHAR aucBCastIp[4] = { 255, 255, 255, 255 };

    aucIp[0] = (UCHAR) ((ulIp & 0xFF000000) >> 24);
    aucIp[1] = (UCHAR) ((ulIp & 0x00FF0000) >> 16);
    aucIp[2] = (UCHAR) ((ulIp & 0x0000FF00) >> 8);
    aucIp[3] = (UCHAR) (ulIp & 0x000000FF);


    /* 组播IP和环回IP判断 */
    if (((aucIp[0] >= 224) && (aucIp[0] <= 239)) || (127 == aucIp[0]))
    {
        return BOOL_FALSE;
    }

    /* 全0，广播地址判断 */
    if ((COMPARE_EQUAL == MEM_COMPARE(aucIp, aucZeroIp, 4))
        || (COMPARE_EQUAL == MEM_COMPARE(aucIp, aucBCastIp, 4)))
    {
        return BOOL_FALSE;
    }

    return BOOL_TRUE;
}

/*****************************************************************************
    Func Name: _DRV_CheckFilterRule
  Description: Check if the filter rule is valid.
        Input: ACL_FILTER_RULE_S *pstRule
       Output:
       Return: static
      Caution:
------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
  --------------------------------------------------------------------------

*****************************************************************************/
static BOOL_T _Drv_CheckFilterRule(ACL_FILTER_RULE_S *pstRule)
{
    if (NULL == pstRule)
    {
        diag_printf("Wrong Filter Rule: null pointer.\r\n");
        return BOOL_FALSE;
    }

    switch ( pstRule->enType )
    {
        case FILTER_SRC_MAC :      /* 源MAC过滤 */
        {
            UCHAR aucZeroMac[MAC_ADDR_LEN]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
            #if 0
            if ((DIRECTION_UPLINK != pstRule->enDirect) ||
                (!memcmp(aucZeroMac,pstRule->unValue.aucMacAddr,MAC_ADDR_LEN)) ||
                (!memcmp(g_aucOnuMac,pstRule->unValue.aucMacAddr,MAC_ADDR_LEN)))
            #else
            /*Do not suport multicast/broadcast/zero/device mac.*/
            if ((pstRule->enDirect >= DIRECTION_BUTT) ||
                (0 != (0x01 & pstRule->unValue.aucMacAddr[0])) ||
                (!memcmp(aucZeroMac,pstRule->unValue.aucMacAddr,MAC_ADDR_LEN)) ||
                (!memcmp(g_aucOnuMac,pstRule->unValue.aucMacAddr,MAC_ADDR_LEN)))
            #endif
            {
                //UCHAR *pucMac = pstRule->unValue.aucMacAddr;
                diag_printf("Wrong Filter Rule: Type: SA MAC Direct:%d MAC:%02x:%02x:%02x:%02x:%02x:%02x.\r\n",
                           pstRule->enDirect,
                           pstRule->unValue.aucMacAddr[0],
                           pstRule->unValue.aucMacAddr[1],
                           pstRule->unValue.aucMacAddr[2],
                           pstRule->unValue.aucMacAddr[3],
                           pstRule->unValue.aucMacAddr[4],
                           pstRule->unValue.aucMacAddr[5]);

                return BOOL_FALSE;
            }
            break;
        }
        case FILTER_DEST_MAC :      /* 目的MAC过滤 */
        {
            UCHAR aucZeroMac[MAC_ADDR_LEN]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

            /*Do not suport multicast/broadcast/zero/device mac.*/
            if ((pstRule->enDirect >= DIRECTION_BUTT) ||
                (0 != (0x01 & pstRule->unValue.aucMacAddr[0])) ||
                (!memcmp(aucZeroMac,pstRule->unValue.aucMacAddr,MAC_ADDR_LEN)) ||
                (!memcmp(g_aucOnuMac,pstRule->unValue.aucMacAddr,MAC_ADDR_LEN)))
            {
                //UCHAR *pucMac = pstRule->unValue.aucMacAddr;
                diag_printf("Wrong Filter Rule: Type: DA MAC Direct:%d MAC:%02x:%02x:%02x:%02x:%02x:%02x.\r\n",
                           pstRule->enDirect,
                           pstRule->unValue.aucMacAddr[0],
                           pstRule->unValue.aucMacAddr[1],
                           pstRule->unValue.aucMacAddr[2],
                           pstRule->unValue.aucMacAddr[3],
                           pstRule->unValue.aucMacAddr[4],
                           pstRule->unValue.aucMacAddr[5]);
                return BOOL_FALSE;
            }
            break;
        }
        case FILTER_ETHER_TYPE :    /* 以太网类型过滤 */
        {
            if (pstRule->enDirect >= DIRECTION_BOTH)
            {
                diag_printf("Wrong Filter Rule: Type:EtherType Direct:%d Ether:0x%04X.\r\n",
                           pstRule->enDirect, pstRule->unValue.usEtherType);
                return BOOL_FALSE;
            }
            break;
        }
        case FILTER_VLAN_ID :       /* VLAN ID过滤 */
        {
            if ((pstRule->enDirect >= DIRECTION_BOTH) ||
                ((pstRule->unValue.usVlanId < MIN_VLAN_ID) ||
                (pstRule->unValue.usVlanId > MAX_VLAN_ID)))
            {
                diag_printf("Wrong Filter Rule: Type:VLAN Direct:%d VID:%d\r\n",
                           pstRule->enDirect, pstRule->unValue.usVlanId);
                return BOOL_FALSE;
            }
            break;
        }
        case FILTER_SRC_IP :        /* 源IP过滤 */
        case BIND_SRC_IP   :        /* 源IP绑定 */
        case FILTER_DEST_IP :       /* 目的IP过滤 */
        case BIND_DEST_IP   :       /* 目的IP绑定 */
        {
            if ((pstRule->enDirect >= DIRECTION_BOTH) ||
                (BOOL_FALSE == _DRV_IsUniCastIp(pstRule->unValue.ulIpAddr)))
            {
                //ULONG ulIp = pstRule->unValue.ulIpAddr;
                diag_printf("Wrong Filter Rule: Type:DIP Direct:%d IP:%d.%d.%d.%d\r\n",
                           pstRule->enDirect,
                           (pstRule->unValue.ulIpAddr & 0xFF000000) >> 24,
                           (pstRule->unValue.ulIpAddr & 0x00FF0000) >> 16,
                           (pstRule->unValue.ulIpAddr & 0x0000FF00) >> 8,
                           (pstRule->unValue.ulIpAddr & 0x000000FF));
                return BOOL_FALSE;
            }
            break;
        }
        case FILTER_SRC_TCP:
        case FILTER_SRC_UDP:
        case FILTER_DEST_TCP:
        case FILTER_DEST_UDP:
        {
            if (pstRule->enDirect >= DIRECTION_BOTH)
            {
                diag_printf("Wrong Filter Rule: Type:L4PORT Direct:%d \r\n",
                           pstRule->enDirect);
                return BOOL_FALSE;
            }
            break;
        }
        default :
            return BOOL_FALSE;
    }

    return BOOL_TRUE;
}

/*****************************************************************************
*****************************************************************************/
#if 0
INT32 DRV_AclAddFilterRule(ACL_FILTER_RULE_S* pRule)
{
#if defined(CONFIG_PRODUCT_EPN200)
    pRule = pRule;
    return ERROR;
#else
    UINT32 uiLPort;
    //logic_pmask_t uiLPortMask;
    UINT32 uiAclRuleType;
    UINT32 uiAclRuleNum;
    UINT32 uiRulePrecedence = 0;
    BOOL_T bSameVal = BOOL_FALSE;
    //BOOL_T bRuleUpExisted = BOOL_FALSE;
    //BOOL_T bRuleDwExisted = BOOL_FALSE;
    DRV_RET_E enRet;
    SLL_NODE_S *pstNode = NULL;
    ACL_FILTER_RULE_S *pstFilter = NULL;
    ACL_DIRECTION_E enFilterDir;

    if(NULL == pRule)
    {
        diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    if (BOOL_TRUE != _Drv_CheckFilterRule(pRule))
    {
        diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
//diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
#if 0
    ClrLgcMaskAll(&uiLPortMask);
    if (DIRECTION_UPLINK == pRule->enDirect){
        SetLgcMaskAll(&uiLPortMask);/*all l2 eth port*/
    }
    else if(DIRECTION_DOWNLINK  == pRule->enDirect){
        SetLgcMaskBitNoCheck(LOGIC_UPPON_PORT, &uiLPortMask);/*pon port*/
    }
    else if(DIRECTION_BOTH  == pRule->enDirect){
        /*lport + upon-port*/
        SetLgcMaskAll(&uiLPortMask);
        SetLgcMaskBitNoCheck(LOGIC_UPPON_PORT, &uiLPortMask);
    }
    else{
        return ERROR;
    }
#endif
    DRV_CFGLOCK;
    uiAclRuleNum = SLL_Count(&stEoamFilterDataList);
    DRV_CFGUNLOCK;
    if(MAX_ACL_FILTER_NUM <= uiAclRuleNum)
    {
        diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
//diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
    DRV_CFGLOCK;
    SLL_Scan(&stEoamFilterDataList, pstNode, SLL_NODE_S *)
    {
        /* 比较当前节点 */
        ACL_FILTER_RULE_S *pstRec = (ACL_FILTER_RULE_S *)SLL_GET_HANDLE(pstNode);

        /*014069 */
        uiRulePrecedence++;

        if ((pstRec->enType == pRule->enType) &&
            (pstRec->enDirect == pRule->enDirect))
        {
            switch (pRule->enType)
            {
                case FILTER_SRC_MAC:
                case FILTER_DEST_MAC:
                    //uiRulePrecedence++;
                    if (COMPARE_EQUAL == MEM_COMPARE(pstRec->unValue.aucMacAddr, pRule->unValue.aucMacAddr, MAC_ADDR_LEN))
                    {
                        bSameVal = BOOL_TRUE;
                    }
                    break;
                case FILTER_ETHER_TYPE:
                    //uiRulePrecedence++;
                    if (pstRec->unValue.usEtherType == pRule->unValue.usEtherType)
                    {
                        bSameVal = BOOL_TRUE;
                    }
                    break;
                case FILTER_VLAN_ID:
                    //uiRulePrecedence++;
                    if (pstRec->unValue.usVlanId == pRule->unValue.usVlanId)
                    {
                        bSameVal = BOOL_TRUE;
                    }
                    break;
                case FILTER_SRC_IP:
                case FILTER_DEST_IP:
                case BIND_SRC_IP:
                case BIND_DEST_IP:
                    //uiRulePrecedence++;
                    if (pstRec->unValue.ulIpAddr == pRule->unValue.ulIpAddr)
                    {
                        bSameVal = BOOL_TRUE;
                    }
                    break;
                case FILTER_SRC_TCP:
                case FILTER_DEST_TCP:
                case FILTER_SRC_UDP:
                case FILTER_DEST_UDP:
                    //uiRulePrecedence++;
                    if (pstRec->unValue.usPort == pRule->unValue.usPort)
                    {
                        bSameVal = BOOL_TRUE;
                    }
                    break;
                default :
                    DRV_CFGUNLOCK;
                    diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
                    return ERROR;
            }

            if (BOOL_TRUE == bSameVal)
            {
                DRV_CFGUNLOCK;
                diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
                return NO_ERROR;
            }
        }
    }
    DRV_CFGUNLOCK;

    if (EOAM_FILTER_RULE_NUM_MAX <= uiRulePrecedence)
    {
        diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    switch (pRule->enType)
    {
        case FILTER_SRC_MAC : /* 根据源MAC过滤  */
            uiAclRuleType = ACL_TRUST_SMAC;
            break;
        case FILTER_DEST_MAC :/* 根据目的MAC过滤  */
            uiAclRuleType = ACL_TRUST_DMAC;
            break;
        case FILTER_ETHER_TYPE : /* 根据以太网过滤   */
            uiAclRuleType = ACL_TRUST_ETHTYPE;
            break;
        case FILTER_VLAN_ID : /* 根据VLAN ID过滤  */
            uiAclRuleType = ACL_TRUST_CTAG_VID;
            break;
        case FILTER_SRC_IP :/* 根据源IP过滤     */
            uiAclRuleType = ACL_TRUST_IPV4_SIP;
            break;
        case FILTER_DEST_IP :/* 根据目的IP过滤   */
            uiAclRuleType = ACL_TRUST_IPV4_DIP;
            break;
        case BIND_SRC_IP : /* 绑定源IP         */
        case BIND_DEST_IP :  /* 绑定目的IP       */
            return ERROR;
        case FILTER_SRC_TCP : /* 根据源TCP过滤     */
            uiAclRuleType = ACL_TRUST_TCP_SPORT;
            break;
        case FILTER_DEST_TCP :/* 根据目的TCP过滤   */
            uiAclRuleType = ACL_TRUST_TCP_DPORT;
            break;
        case FILTER_SRC_UDP : /* 根据源UDP过滤     */
            uiAclRuleType = ACL_TRUST_UDP_SPORT;
            break;
        case FILTER_DEST_UDP : /* 根据目的UDP过滤   */
            uiAclRuleType = ACL_TRUST_UDP_DPORT;
            break;
        default:
            diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
    }

#if 0
    LgcPortForNoCheck(uiLPortIndex, &uiLPortMask){
        if(DRV_OK != Ioctl_AddAclFilter(uiLPortIndex, 0, uiAclRuleType, (void *)&(pRule->unValue))){
            return ERROR;
        }
    }
#else
    if (DIRECTION_UPLINK == pRule->enDirect)
    {
        uiLPort = INVALID_PORT;
        enFilterDir = ACL_DIRECTION_UPLINK;
    }
    else if(DIRECTION_DOWNLINK == pRule->enDirect)
    {
        uiLPort = LOGIC_UPPON_PORT;
        enFilterDir = ACL_DIRECTION_DOWNLINK;
    }
    else if(DIRECTION_BOTH == pRule->enDirect)
    {
        uiLPort = INVALID_PORT;
        enFilterDir = ACL_DIRECTION_BOTH;
    }
    else
    {
        diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /*Bind the rule to ports in switch chip.*/
    enRet = Ioctl_AddAclFilter(uiLPort, uiRulePrecedence,
                               uiAclRuleType,
                               enFilterDir,
                               (void *)&(pRule->unValue));
    if(DRV_OK != enRet)
    {
        diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    pstFilter = (ACL_FILTER_RULE_S *)malloc(sizeof(ACL_FILTER_RULE_S));
    if (NULL == pstFilter)
    {
        diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return VOS_ERR;
    }

    memcpy(pstFilter, pRule, sizeof(ACL_FILTER_RULE_S));
    pstFilter->uiRulePrecedence = uiRulePrecedence;

    /*lint --e{429}*/
    pstNode = (SLL_NODE_S *)malloc(sizeof(SLL_NODE_S));
    if (NULL == pstNode)
    {
        free(pstFilter);
        diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return VOS_ERR;
    }

    SLL_Init_Node(pstNode);
    SLL_SET_HANDLE(pstNode, pstFilter);
    DRV_CFGLOCK;
    SLL_Add(&stEoamFilterDataList, pstNode);
    DRV_CFGUNLOCK;
    /*lint -restore */
#endif

    return NO_ERROR;
#endif
}
#endif
/*****************************************************************************


*****************************************************************************/
#if 0
INT32 DRV_AclDelFilterRule(ACL_FILTER_RULE_S* pRule)
{
#if defined(CONFIG_PRODUCT_EPN200)
    pRule = pRule;
    return NO_ERROR;
#else
    BOOL_T bFound = BOOL_FALSE;
    UINT32 uiAclRuleNum;
    UINT32 uiRulePrecedence = 0;
    SLL_NODE_S *pstNode = NULL;
    ACL_DIRECTION_E enFilterDir;

    if(NULL == pRule)
    {
        diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    if (BOOL_TRUE != _Drv_CheckFilterRule(pRule))
    {
        diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    DRV_CFGLOCK;
    uiAclRuleNum = SLL_Count(&stEoamFilterDataList);
    DRV_CFGUNLOCK;
    if (uiAclRuleNum == 0)
    {
        diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return NO_ERROR;
    }

    DRV_CFGLOCK;
    SLL_Scan(&stEoamFilterDataList, pstNode, SLL_NODE_S *)
    {
        diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
        ACL_FILTER_RULE_S *pstRec = (ACL_FILTER_RULE_S *)SLL_GET_HANDLE(pstNode);
        if ((pstRec->enType == pRule->enType) &&
            (pstRec->enDirect == pRule->enDirect))
        {
            switch (pstRec->enType)
            {
                case FILTER_SRC_MAC:
                case FILTER_DEST_MAC:
                    if (COMPARE_EQUAL == MEM_COMPARE(pstRec->unValue.aucMacAddr, pRule->unValue.aucMacAddr, MAC_ADDR_LEN))
                    {
                        bFound = BOOL_TRUE;
                    }
                    break;
                case FILTER_ETHER_TYPE:
                    if (pstRec->unValue.usEtherType == pRule->unValue.usEtherType)
                    {
                        bFound = BOOL_TRUE;
                    }
                    break;
                case FILTER_VLAN_ID:
                    if (pstRec->unValue.usVlanId == pRule->unValue.usVlanId)
                    {
                        bFound = BOOL_TRUE;
                    }
                    break;
                case FILTER_SRC_IP:
                case FILTER_DEST_IP:
                case BIND_SRC_IP:
                case BIND_DEST_IP:
                    if (pstRec->unValue.ulIpAddr == pRule->unValue.ulIpAddr)
                    {
                        bFound = BOOL_TRUE;
                    }
                    break;
                case FILTER_SRC_TCP:
                case FILTER_DEST_TCP:
                case FILTER_SRC_UDP:
                case FILTER_DEST_UDP:
                    if (pstRec->unValue.usPort == pRule->unValue.usPort)
                    {
                        bFound = BOOL_TRUE;
                    }
                    break;
                default :
                    break;
            }
        }

        if (bFound == BOOL_TRUE)
        {
            uiRulePrecedence = pstRec->uiRulePrecedence;

            switch (pRule->enDirect)
            {
                case DIRECTION_UPLINK:
                    enFilterDir = ACL_DIRECTION_UPLINK;
                    break;
                case DIRECTION_DOWNLINK:
                    enFilterDir = ACL_DIRECTION_DOWNLINK;
                    break;
                case DIRECTION_BOTH:
                    enFilterDir = ACL_DIRECTION_BOTH;
                    break;
                default :
                    DRV_CFGUNLOCK;
                    diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
                    return ERROR;
            }
            diag_printf("\ndel dir:%d\n",enFilterDir);
            if(DRV_OK != Ioctl_DelAclFilterByPort(INVALID_PORT, uiRulePrecedence, enFilterDir))
            {
                DRV_CFGUNLOCK;
                diag_printf("\nfunc:%s,line:%d.\n",__FUNCTION__,__LINE__);
                return ERROR;
            }

            SLL_Delete(&stEoamFilterDataList, pstNode);
            free(pstRec);
            free(pstNode);
            break;
        }
    }
    DRV_CFGUNLOCK;

    return NO_ERROR;
#endif
}
#endif
#if !defined(CONFIG_PRODUCT_EPN200)
/*****************************************************************************
*****************************************************************************/
INT32 DRV_CtcAddClfRmkByPrec(UINT32 lport,
	                         UINT32  precedenceOfRule,
	                         oam_clasmark_rulebody_t *pstClsRule,
	                         oam_clasmark_fieldbody_t *pstClsField)
{
    UINT32 uiAclRuleType;
    UINT32 uiRemarkPri;
	UINT32 uiFieldNum;
    UINT32 uiFieldId;
    ACL_CLF_RMK_VAL_EN enRuleValue;

    /* 0180-C200-0000 ~ 0180-C200-003F */
    UINT8  aucSavedMacAdd1[MAC_ADDR_LEN] = {0x01, 0x80, 0xC2, 0x0, 0x0, 0x3F};
    /* 0100-5Exx-xxxx */
    UINT8  aucSavedMacAdd2[MAC_ADDR_LEN] = {0x01, 0x0, 0x5E, 0x0, 0x0, 0x0};

    if((FALSE == IsValidLgcPort((port_num_t)lport)) || (NULL == pstClsRule)  || (NULL == pstClsField))
    {
        printf("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    uiFieldNum = (UINT32)(pstClsRule->numOfField);
    uiRemarkPri = (UINT32)(pstClsRule->ethPriMark);

    /*only support one rule per precedence*/
    if (uiFieldNum > CLF_RMK_RULES_IN_1_PRECEDENCE_MAX)
    {
        return ERROR;
    }

    for(uiFieldId = 0; uiFieldId < uiFieldNum; uiFieldId++)
    {
        switch (pstClsField->fieldSelect)
        {
            case OAM_CTC_CLS_FIELD_TYPE_DA_MAC :
            {
                UINT8  aucZeroMacAdd[MAC_ADDR_LEN] = {0};

                memcpy(enRuleValue.aucMacAddr, pstClsField->matchValue, MAC_ADDR_LEN);

                /* 014022 */
                if ( ((0 == memcmp(enRuleValue.aucMacAddr, aucSavedMacAdd1, (MAC_ADDR_LEN - 1))) &&
                      (enRuleValue.aucMacAddr[5] <= aucSavedMacAdd1[5])) ||
                     (0 == memcmp(enRuleValue.aucMacAddr, aucSavedMacAdd2, (MAC_ADDR_LEN - 3))) )
                {
                    return ERROR;
                }
                /*  */

                if (0 == memcmp(enRuleValue.aucMacAddr, aucZeroMacAdd, MAC_ADDR_LEN))
                {
                    /*Always match*/
                    uiAclRuleType = ACL_TRUST_PORT;;
                }
                else
                {
                    /*Dmac*/
                    uiAclRuleType = ACL_TRUST_DMAC;
                }
                break;
            }
            case OAM_CTC_CLS_FIELD_TYPE_SA_MAC :
                uiAclRuleType = ACL_TRUST_SMAC;
                memcpy(enRuleValue.aucMacAddr, pstClsField->matchValue, MAC_ADDR_LEN); 

                /* 014022 */
                if ( ((0 == memcmp(enRuleValue.aucMacAddr, aucSavedMacAdd1, (MAC_ADDR_LEN - 1))) &&
                      (enRuleValue.aucMacAddr[5] <= aucSavedMacAdd1[5])) ||
                     (0 == memcmp(enRuleValue.aucMacAddr, aucSavedMacAdd2, (MAC_ADDR_LEN - 3))) )
                {
                    return ERROR;
                }
                /* */

                break;
            case OAM_CTC_CLS_FIELD_TYPE_ETH_PRI :
                uiAclRuleType = ACL_TRUST_CTAG_PRIO;
                enRuleValue.ulValue = pstClsField->matchValue[5];
                break;
            case OAM_CTC_CLS_FIELD_TYPE_VLAN_ID :
                uiAclRuleType = ACL_TRUST_CTAG_VID;
                enRuleValue.ulValue = (pstClsField->matchValue[4]<<8) | pstClsField->matchValue[5];
                break;
            case OAM_CTC_CLS_FIELD_TYPE_ETHER_TYPE :
                uiAclRuleType = ACL_TRUST_ETHTYPE;
                enRuleValue.ulValue = (pstClsField->matchValue[4]<<8) | pstClsField->matchValue[5];
                break;
            case OAM_CTC_CLS_FIELD_TYPE_DST_IP :
                uiAclRuleType = ACL_TRUST_IPV4_DIP;
                memcpy(&enRuleValue.ulValue, &(pstClsField->matchValue[2]), sizeof(enRuleValue.ulValue));
                break;
            case OAM_CTC_CLS_FIELD_TYPE_SRC_IP :
                uiAclRuleType = ACL_TRUST_IPV4_SIP;
                memcpy(&enRuleValue.ulValue, &(pstClsField->matchValue[2]), sizeof(enRuleValue.ulValue));
                break;
            case OAM_CTC_CLS_FIELD_TYPE_IP_TYPE :
                uiAclRuleType = ACL_TRUST_IPV4_PROTOCOL;
                enRuleValue.ulValue = pstClsField->matchValue[5];
                break;
            case OAM_CTC_CLS_FIELD_TYPE_IP_DSCP :
                uiAclRuleType = ACL_TRUST_IPV4_TOS;
                enRuleValue.ulValue = (pstClsField->matchValue[5] << 2 );
                break;
            /*Do not support this rule.*/
            #if 0
            case OAM_CTC_CLS_FIELD_TYPE_IP_PRECEDENCE :
                uiAclRuleType = DRV_ACL_TRUST_IPV4_TOS;
                enRuleValue.ulValue = pstClsField->matchValue[5];
                break;
            #endif
            case OAM_CTC_CLS_FIELD_TYPE_L4_SRC_PORT :
                /*
                L4 port rule need 2 rules: src udp port and src tcp port.
                */
                uiAclRuleType = ACL_TRUST_TCP_SPORT;
                enRuleValue.ulValue = (pstClsField->matchValue[4]<<8) | pstClsField->matchValue[5]; 
                if(DRV_OK != Ioctl_AddCtcClfByPort(lport, precedenceOfRule, 
                                                   uiAclRuleType, 
                                                   (void *)&enRuleValue, 
                                                   uiRemarkPri))
                {
                    return ERROR;
                }

                uiAclRuleType = ACL_TRUST_UDP_SPORT;
                break;
            case OAM_CTC_CLS_FIELD_TYPE_L4_DST_PORT :
                /*
                L4 port rule need 2 rules: dst udp port and dst tcp port.
                */
                uiAclRuleType = ACL_TRUST_TCP_DPORT;
                enRuleValue.ulValue = (pstClsField->matchValue[4]<<8) | pstClsField->matchValue[5];
                if(DRV_OK != Ioctl_AddCtcClfByPort(lport, precedenceOfRule, 
                                                   uiAclRuleType, 
                                                   (void *)&enRuleValue, 
                                                   uiRemarkPri))
                {
                    return ERROR;
                }

                uiAclRuleType = ACL_TRUST_UDP_DPORT;
                break;
            default:
                return ERROR;
        }

        if(DRV_OK != Ioctl_AddCtcClfByPort(lport, precedenceOfRule, 
                                           uiAclRuleType, 
                                           (void *)&enRuleValue, 
                                           uiRemarkPri))
        {
            return ERROR;
        }
    }

    return NO_ERROR;
}

/*****************************************************************************

*****************************************************************************/
INT32 DRV_CtcClearClfRmkByPort(UINT32 lport)
{
    if(FALSE == IsValidLgcPort((port_num_t)lport))
    {
        return ERROR;
    }

    if(DRV_OK == Ioctl_ClearCtcClfByPort(lport)){
        return NO_ERROR;
    }
    else{
        return ERROR;
    }
}

/*****************************************************************************
             ERROR
*****************************************************************************/
INT32 DRV_CtcDelClfRmkByPrec(UINT32 lport, UINT32 precedenceOfRule)
{
    if(FALSE == IsValidLgcPort((port_num_t)lport))
    {
        return ERROR;
    }

    if(DRV_OK == Ioctl_DelCtcClfByPrec(lport, precedenceOfRule)){
        return NO_ERROR;
    }
    else{
        return ERROR;
    }
}
#endif

/*****************************************************************************
 函 数 名  : DRV_CheckMonitorConflict

*****************************************************************************/
INT32 DRV_CheckMonitorConflict(UINT8 lport)
{
    unsigned int i = 0;
    logic_pmask_t  oam_monitor_lmask = {0x00};

    SetLgcMaskBit((port_num_t)lport, &oam_monitor_lmask);

    for (i = 0; i < sizeof(logic_pmask_t); i++)
    {
        if (( (((char*)&oam_monitor_lmask)[i]) & (((char*)&oam_in_mirror_lmask)[i]) ) ||
            ( (((char*)&oam_monitor_lmask)[i]) & (((char*)&oam_out_mirror_lmask)[i])) )
        {
            return ERROR;
        }
    }

    return NO_ERROR;
}

/*****************************************************************************
 函 数 名  : DRV_SetMonitor

*****************************************************************************/
INT32 DRV_SetMonitor(UINT8 lport, UINT8 lenable)
{
#if !defined(CONFIG_PRODUCT_EPN200)
    int ret = DRV_OK;

    lport = lport - 1;

    if(FALSE == IsValidLgcPort((port_num_t)lport))
    {
        return ERROR;
    }

    if (DISABLE == lenable)
    {
        if (oam_monitor_port == (UINT32)lport)
        {
            oam_monitor_port = MONITOR_PORT_DEFAULT_VALUE;
            /*in kernel, 0xff 表示 disable mirror功能*/
            Ioctl_SetMirrorGroup(0xff, oam_in_mirror_lmask, oam_out_mirror_lmask);
        }
        return NO_ERROR;
    }

    if (NO_ERROR != DRV_CheckMonitorConflict(lport))
    {
        return ERROR;
    }

    oam_monitor_port = (UINT32)lport;

    if(LgcMaskNotNull(&oam_in_mirror_lmask) || LgcMaskNotNull(&oam_out_mirror_lmask))
    {
        ret = Ioctl_SetMirrorGroup(oam_monitor_port, oam_in_mirror_lmask, oam_out_mirror_lmask);
    }

    if(DRV_OK == ret)
    {
        return NO_ERROR;
    }
    else
    {
        return ERROR;
    }
#else
    lport = lport;
    return ERROR;
#endif
}


/*****************************************************************************
 函 数 名  : DRV_AddMirror
 输入参数  : UINT8 lport
             UCHAR ucMirrorDirect
 输出参数  : 无
 返 回 值  : NO_ERROR
             ERROR


*****************************************************************************/
INT32 DRV_AddMirror(UINT8 lport, UCHAR ucMirrorDirect)
{
#if !defined(CONFIG_PRODUCT_EPN200)
    int ret = DRV_OK;
    logic_pmask_t  * pstoam_in_mirror_lmask_temp = NULL;
    logic_pmask_t  * pstoam_out_mirror_lmask_temp = NULL;
#if defined(CONFIG_PRODUCT_EPN104)

    logic_pmask_t  oam_in_mirror_lmask_temp = {0x00};
    logic_pmask_t  oam_out_mirror_lmask_temp = {0x00};

    memcpy(&oam_in_mirror_lmask_temp, &oam_in_mirror_lmask, sizeof(oam_in_mirror_lmask_temp));
    memcpy(&oam_out_mirror_lmask_temp, &oam_out_mirror_lmask, sizeof(oam_out_mirror_lmask_temp));

    pstoam_in_mirror_lmask_temp = &oam_in_mirror_lmask_temp;
    pstoam_out_mirror_lmask_temp = &oam_out_mirror_lmask_temp;
#else
    pstoam_in_mirror_lmask_temp = &oam_in_mirror_lmask;
    pstoam_out_mirror_lmask_temp = &oam_out_mirror_lmask;
#endif
    lport = lport - 1;
    
    if ((FALSE == IsValidLgcPort((port_num_t)lport)) || (lport == oam_monitor_port))
    {
        return ERROR;
    }

    switch (ucMirrorDirect)
    {
        case IN_MIRROR :
            SetLgcMaskBit((port_num_t)lport, pstoam_in_mirror_lmask_temp);
            ClrLgcMaskBit((port_num_t)lport, pstoam_out_mirror_lmask_temp);
            break;
        case OUT_MIRROR :
            SetLgcMaskBit((port_num_t)lport, pstoam_out_mirror_lmask_temp);
            ClrLgcMaskBit((port_num_t)lport, pstoam_in_mirror_lmask_temp);
            break;
        case BOTH_MIRROR :
            SetLgcMaskBit((port_num_t)lport, pstoam_in_mirror_lmask_temp);
            SetLgcMaskBit((port_num_t)lport, pstoam_out_mirror_lmask_temp);
            break;
        default:
            return ERROR;
    }

#if defined(CONFIG_PRODUCT_EPN104)
    /* realtek交换芯片限制多端口做镜像时必须配置相同 */
    if ((LgcMaskNotNull(pstoam_in_mirror_lmask_temp)) &&
        (LgcMaskNotNull(pstoam_out_mirror_lmask_temp)) &&
        (0 != memcmp(pstoam_in_mirror_lmask_temp, pstoam_out_mirror_lmask_temp, sizeof(oam_out_mirror_lmask_temp))))
    {
        return ERROR;
    }

    memcpy(&oam_in_mirror_lmask, &oam_in_mirror_lmask_temp, sizeof(oam_in_mirror_lmask_temp));
    memcpy(&oam_out_mirror_lmask, &oam_out_mirror_lmask_temp, sizeof(oam_out_mirror_lmask_temp));
#endif

    if(MONITOR_PORT_DEFAULT_VALUE == oam_monitor_port)
    {
        return NO_ERROR;
    }

    if(LgcMaskNotNull(&oam_in_mirror_lmask) || LgcMaskNotNull(&oam_out_mirror_lmask))
    {
        ret = Ioctl_SetMirrorGroup(oam_monitor_port, oam_in_mirror_lmask, oam_out_mirror_lmask);
    }

    if(DRV_OK == ret)
    {
        return NO_ERROR;
    }
    else
    {
        return ERROR;
    }
#else
    lport = lport;
    ucMirrorDirect = ucMirrorDirect;
    return ERROR;
#endif
}

/*****************************************************************************
 函 数 名  : DRV_DelMirror

*****************************************************************************/
INT32 DRV_DelMirror(UINT8 lport, UCHAR ucMirrorDirect)
{
#if !defined(CONFIG_PRODUCT_EPN200)
    int ret = DRV_OK;
    logic_pmask_t  * pstoam_in_mirror_lmask_temp = NULL;
    logic_pmask_t  * pstoam_out_mirror_lmask_temp = NULL;
#if defined(CONFIG_PRODUCT_EPN104)
    logic_pmask_t  oam_in_mirror_lmask_temp = {0x00};
    logic_pmask_t  oam_out_mirror_lmask_temp = {0x00};

    memcpy(&oam_in_mirror_lmask_temp, &oam_in_mirror_lmask, sizeof(oam_in_mirror_lmask_temp));
    memcpy(&oam_out_mirror_lmask_temp, &oam_out_mirror_lmask, sizeof(oam_out_mirror_lmask_temp));

    pstoam_in_mirror_lmask_temp = &oam_in_mirror_lmask_temp;
    pstoam_out_mirror_lmask_temp = &oam_out_mirror_lmask_temp;
#else
    pstoam_in_mirror_lmask_temp = &oam_in_mirror_lmask;
    pstoam_out_mirror_lmask_temp = &oam_out_mirror_lmask;
#endif
    lport = lport - 1;
    
    if(FALSE == IsValidLgcPort((port_num_t)lport))
    {
        return ERROR;
    }

    switch (ucMirrorDirect)
    {
        case IN_MIRROR :
            ClrLgcMaskBit((port_num_t)lport, pstoam_in_mirror_lmask_temp);
            break;
        case OUT_MIRROR :
            ClrLgcMaskBit((port_num_t)lport, pstoam_out_mirror_lmask_temp);
            break;
        case BOTH_MIRROR :
            ClrLgcMaskBit((port_num_t)lport, pstoam_in_mirror_lmask_temp);
            ClrLgcMaskBit((port_num_t)lport, pstoam_out_mirror_lmask_temp);
            break;
        default:
            return ERROR;
    }

#if defined(CONFIG_PRODUCT_EPN104)
    /* realtek交换芯片限制多端口做镜像时必须配置相同 */
    if ((LgcMaskNotNull(pstoam_in_mirror_lmask_temp)) &&
        (LgcMaskNotNull(pstoam_out_mirror_lmask_temp)) &&
        (0 != memcmp(pstoam_in_mirror_lmask_temp, pstoam_out_mirror_lmask_temp, sizeof(oam_out_mirror_lmask_temp))))
    {
        return ERROR;
    }

    memcpy(&oam_in_mirror_lmask, &oam_in_mirror_lmask_temp, sizeof(oam_in_mirror_lmask_temp));
    memcpy(&oam_out_mirror_lmask, &oam_out_mirror_lmask_temp, sizeof(oam_out_mirror_lmask_temp));
#endif

    if(MONITOR_PORT_DEFAULT_VALUE == oam_monitor_port)
    {
        return NO_ERROR;
    }

    ret = Ioctl_SetMirrorGroup(oam_monitor_port, oam_in_mirror_lmask, oam_out_mirror_lmask);

    if(DRV_OK == ret)
    {
        return NO_ERROR;
    }
    else
    {
        return ERROR;
    }
#else
    lport = lport;
    ucMirrorDirect = ucMirrorDirect;
    return NO_ERROR;
#endif
}


/*****************************************************************************
 函 数 名  : CanReadOpticsPower
 修改历史      :

*****************************************************************************/
static BOOL_T CanReadOpticsPower(void)
{
#if 0
    UCHAR aucData[OPTICS_TRANSCEIVER_VENDOR_PN_LEN+1] = {0};
    UCHAR aucZeroData[OPTICS_TRANSCEIVER_VENDOR_PN_LEN];
    int iRet = NO_ERROR;

    iRet = i2cReadRandom(IIC_OPTICS_TRANSCEIVER_ADDRESS1,
                         IIC_OPTICS_TRANSCEIVER_VENDOR_PN_FIRST_REGISTER,
                         OPTICS_TRANSCEIVER_VENDOR_PN_LEN,
                         &aucData[0]);

    if (NO_ERROR != iRet)
    {
        return BOOL_FALSE;
    }

    MEM_SET_ZERO(aucZeroData, OPTICS_TRANSCEIVER_VENDOR_PN_LEN);

    if (0 == MEM_COMPARE(aucData, aucZeroData, OPTICS_TRANSCEIVER_VENDOR_PN_LEN))
    {
        return BOOL_FALSE;
    }
#endif
    return BOOL_TRUE;
}

/*****************************************************************************
 函 数 名  : Drv_GetOpticsTransmitPower

*****************************************************************************/
ULONG Drv_GetOpticsTransmitPower(USHORT *pusTransPower)
{
    UINT8 aucTransPower[2] = {0};
    int iRet = NO_ERROR;

    if(NULL == pusTransPower)
    {
        return VOS_ERR;
    }

    if (BOOL_TRUE != CanReadOpticsPower())
    {
        return VOS_NOT_SUPPORT;
    }

    iRet = i2cReadRandom(IIC_OPTICS_TRANSCEIVER_ADDRESS2,
                         IIC_OPTICS_TRANSCEIVER_TXPOWER_MSB_REGISTER,
                         2,
                         &aucTransPower[0]);

    if (0 > iRet)
    {
        return VOS_ERR;
    }

    *pusTransPower = (aucTransPower[OFFSET_MSB] << 8U) | aucTransPower[OFFSET_LSB];

    return VOS_OK;
}

/*****************************************************************************
 输出参数  : 无
 被调函数  :

 修改历史      :

*****************************************************************************/
ULONG Drv_GetOpticsReceivePower(USHORT *pusReceivPower)
{
    UINT8 aucReceiPower[2] = {0};
    int iRet = NO_ERROR;

    if(NULL == pusReceivPower)
    {
        return VOS_ERR;
    }

    if (BOOL_TRUE != CanReadOpticsPower())
    {
        return VOS_ERR;
    }

    iRet = i2cReadRandom(IIC_OPTICS_TRANSCEIVER_ADDRESS2,
                         IIC_OPTICS_TRANSCEIVER_RXPOWER_MSB_REGISTER,
                         2,
                         aucReceiPower);
    if (0 > iRet)
    {
        return VOS_ERR;
    }
    *pusReceivPower = (aucReceiPower[OFFSET_MSB] << 8U) | aucReceiPower[OFFSET_LSB];

    return VOS_OK;
}

static int getDigit(unsigned short usNumber)
{
	int i = 0;
    unsigned short usTempNum = usNumber;
	while(usTempNum)
	{
		usTempNum = usTempNum/10;
		i++;
	}
	return i;
}
static double findPower(double dPowerData)
{
	int i;
	if(dPowerData <0.0 || dPowerData >10.0)
		return 0.0;

	for(i = 0; i<100; i++)
	{
		if(dPowerData - g_adPower[i][0] < 0.000001 && dPowerData - g_adPower[i][0] > -0.000001)
			return g_adPower[i][1];
	}
	return 0.0;
}
double GetOptics(unsigned short usPowerData)
{
	int iDigit;
	double dPowerData;
    double dPower;

	iDigit= getDigit(usPowerData);

	switch(iDigit)
	{
	case 1:
		dPowerData = (double)usPowerData/10.0;
		dPower = 10*(findPower(dPowerData) + 1.0 -4.0);
		break;
	case 2:
		dPowerData = (double)((unsigned short)((usPowerData % 10)/1))/10.0;
        /*lint --e{653}*/
		dPowerData = dPowerData+ (double)(usPowerData/10);
		dPower = 10*(findPower(dPowerData) + 1.0 -4.0);
		break;
	case 3:
		dPowerData = (double)((unsigned short)((usPowerData % 100)/10))/10.0;
		dPowerData = dPowerData+ (double)(usPowerData/100);
		dPower = 10*(findPower(dPowerData) + 2.0 -4.0);
		break;
	case 4:
		dPowerData = (double)((unsigned short)((usPowerData % 1000)/100))/10.0;
		dPowerData = dPowerData+ (double)(usPowerData/1000);
		dPower= 10*(findPower(dPowerData) + 3.0 -4.0);
		break;
	case 5:
		dPowerData= (double)((unsigned short)((usPowerData % 10000)/1000))/10.0;
		dPowerData= dPowerData+ (double)(usPowerData/10000);
		dPower= 10*(findPower(dPowerData) + 4.0 -4.0);
		break;
	default:
        dPower = 0.0;
		break;
	}
    /*lint -restore */
	return dPower;
}


/*****************************************************************************
 函 数 名  : Drv_GetOpticsPower
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :

*****************************************************************************/
ULONG Drv_GetOpticsPower(FLOAT *pReceivPower, FLOAT *pTransPower)
{
    ULONG ulRet = VOS_OK;
    USHORT usReceivPower = 0;
    USHORT usTransPower = 0;

    if((NULL == pReceivPower) || (NULL == pTransPower))
    {
        return VOS_ERR;
    }

    ulRet = Drv_GetOpticsReceivePower(&usReceivPower);
    ulRet += Drv_GetOpticsTransmitPower(&usTransPower);

    if (VOS_OK != ulRet)
    {
        return VOS_ERR;
    }

    else if((FALSE == g_bPonLos) && (BOOL_FALSE == g_bLightAlways))
    {
        *pReceivPower = MIN_RECIVE_OPTIC_POWER;
		*pTransPower = MIN_RECIVE_OPTIC_POWER;
    }
    else
    {
        //OPTICS_POWER_CALCULATE(fRecPower, pReceivPower);
        //OPTICS_POWER_CALCULATE(fTransPower, pTransPower);
        *pReceivPower = (FLOAT)GetOptics(usReceivPower);
        *pTransPower = (FLOAT)GetOptics(usTransPower);
        CHECK_OPTIC_POWER(*pReceivPower);
        CHECK_OPTIC_POWER(*pTransPower);
    }
    return VOS_OK;
}

/*****************************************************************************

*****************************************************************************/
ULONG Drv_GetOpticsTemperature(SHORT *psTempratrue, USHORT *psVoltage, USHORT *psBias)
{
    UINT8 aucData[6] = {0};
    signed char cData = 0;
    int iRet = NO_ERROR;

    if((NULL == psTempratrue) || (NULL == psVoltage) || (NULL == psBias))
    {
        return VOS_ERR;
    }

    if (BOOL_TRUE != CanReadOpticsPower())
    {
        return VOS_ERR;
    }

    iRet = i2cReadRandom(IIC_OPTICS_TRANSCEIVER_ADDRESS2,
                         IIC_OPTICS_TRANSCEIVER_TEMPERATURE_MSB_REGISTER,
                         6,
                         &aucData[0]);
    if (0 > iRet)
    {
        return VOS_ERR;
    }

    cData = (char)aucData[0];
    *psTempratrue = cData;

	*psVoltage = (aucData[OFFSET_MSB + 2] << 8U) | aucData[OFFSET_LSB + 2];
	*psBias    = (aucData[OFFSET_MSB + 4] << 8U) | aucData[OFFSET_LSB + 4];

    return VOS_OK;
}

/*****************************************************************************

*****************************************************************************/
INT32 DRV_GetPortQinqCfg(UINT32 uiLPortId, PORT_QINQ_S *pstQinqCfg)
{

    /*OPL_SOFT_CFG_ACCESS_LOCK;*/
    memcpy(pstQinqCfg, g_astUniQinqMode + uiLPortId, sizeof(PORT_QINQ_S));
    /*OPL_SOFT_CFG_ACCESS_UNLOCK;*/

    return NO_ERROR;
}

/*****************************************************************************
  --------------------------------------------------------------------------

*****************************************************************************/
INT32 DRV_SetPortQinqCfg(UINT32 uiLPortId, PORT_QINQ_S *pstQinqCfg)
{
    /*OPL_SOFT_CFG_ACCESS_LOCK;*/
    memcpy(g_astUniQinqMode + uiLPortId, pstQinqCfg, sizeof(PORT_QINQ_S));
    /*OPL_SOFT_CFG_ACCESS_UNLOCK;*/

    return NO_ERROR;
}

/*****************************************************************************

*****************************************************************************/
STATIC INT32 DRV_CheckVlanForQinqMode(UINT32 uiLPortId, UINT32 uiVlanId)
{
    UINT32 uiLPortIndex;
	CTC_VLAN_CFG_S stVlanMode;


    memset(&stVlanMode, 0, sizeof(stVlanMode));
        
    for (uiLPortIndex = 1; uiLPortIndex <= MAX_PORT_NUM; uiLPortIndex++)
    {
        if (uiLPortIndex == uiLPortId)
        {
            continue;
        }

/*To do*/
#if !defined(CONFIG_PRODUCT_EPN200)
        if(NO_ERROR != _CTC_GetPortVlanModeCfg(uiLPortId, &stVlanMode))
        {
            continue;
        }
#endif
        if (CTC_VLAN_MODE_TRANSLATION == stVlanMode.mode)
        {
            UINT32 uiVlanIndex;

            for (uiVlanIndex = 0; uiVlanIndex < stVlanMode.number_of_entries; uiVlanIndex++)
            {
                /*The case that pvid of tag vlan mode equals old vlan of translation mode is not allowed.*/
                if ((stVlanMode.vlan_list[uiVlanIndex*2] != stVlanMode.vlan_list[uiVlanIndex*2 + 1]) &&
                    ((uiVlanId == stVlanMode.vlan_list[uiVlanIndex*2]) ||
                     (uiVlanId == stVlanMode.vlan_list[uiVlanIndex*2 + 1])))
                {
                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}

/*****************************************************************************
  函 数 名: DRV_ResetQinqPvid
*****************************************************************************/
INT32 DRV_ResetQinqPvid(UINT32 uiLPortId)
{
    INT32 iRet = NO_ERROR;
    UINT32 uiPvid = 0;
    CTC_VLAN_CFG_S stVlanMode;
    PORT_QINQ_S stQinqCfg;


    memset(&stVlanMode, 0, sizeof(stVlanMode));
    /*to do*/
    #if !defined(CONFIG_PRODUCT_EPN200)
    iRet = _CTC_GetPortVlanModeCfg(uiLPortId, &stVlanMode);
    if (NO_ERROR != iRet)
    {
        DRV_DEBUG_PRINT("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;  
    }
    #endif

    
    iRet = DRV_GetPortQinqCfg(uiLPortId, &stQinqCfg);
    if (NO_ERROR != iRet)
    {
        return ERROR;
    }
    
    if (TRUE != stQinqCfg.bEnable)
    {
        return NO_ERROR;
    }

    //uiPvid = stVlanMode.default_vlan;
    uiPvid = stQinqCfg.uiPvid;

    iRet = dalQinqPvidReSet(uiPvid, 0);
    if (NO_ERROR != iRet)
    {
        DRV_DEBUG_PRINT("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;  
    }

    stQinqCfg.bEnable = FALSE;

    iRet = DRV_SetPortQinqCfg(uiLPortId, &stQinqCfg);
    if (NO_ERROR != iRet)
    {
        return ERROR;
    }
    
    return NO_ERROR;
}

/*****************************************************************************
------------------------------------------------------------------------------
*****************************************************************************/
INT32 DRV_EnablePortQinq(UINT32 uiLPortId, UINT32 uiTpid, UINT32 uiPvid, UINT32 uiPriority)
{
    INT32 iRet = NO_ERROR;
    CTC_VLAN_CFG_S stVlanMode;
    PORT_QINQ_S stQinqCfg;


    memset(&stVlanMode, 0, sizeof(stVlanMode));
    
    iRet = DRV_GetPortQinqCfg(uiLPortId, &stQinqCfg);
    if (NO_ERROR != iRet)
    {
        DRV_DEBUG_PRINT("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    if ((TRUE == stQinqCfg.bEnable)  &&
        (uiPvid == stQinqCfg.uiPvid) &&
        (uiTpid == stQinqCfg.uiTpid) &&
        (uiPriority == stQinqCfg.uiPriority))
    {
        return NO_ERROR;
    }

    /*Check if other tag/translation port's pvid/old vlan equals this pvid.*/
    iRet = DRV_CheckVlanForQinqMode(uiLPortId, uiPvid);
    if (FALSE == iRet)
    {
        DRV_DEBUG_PRINT("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
    
    /*to do*/
    #if !defined(CONFIG_PRODUCT_EPN200)
    iRet = CTC_DRV_SetTransparentVlan(uiLPortId);
    if (NO_ERROR != iRet)
    {
        DRV_DEBUG_PRINT("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
    #endif

    iRet = dalQinqPvidSet(uiPvid, uiPriority);
    if (NO_ERROR != iRet)
    {
        DRV_DEBUG_PRINT("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    stVlanMode.default_vlan = uiPvid;
    stVlanMode.mode = CTC_VLAN_MODE_QINQ;
    stVlanMode.number_of_entries = 0;
    /*to do*/
    #if !defined(CONFIG_PRODUCT_EPN200)
    iRet = _CTC_SetPortVlanModeCfg(uiLPortId, &stVlanMode);
    if (NO_ERROR != iRet)
    {
        DRV_DEBUG_PRINT("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
    #endif

    stQinqCfg.bEnable = TRUE;
    stQinqCfg.uiPvid = uiPvid;
    stQinqCfg.uiTpid = uiTpid;
    stQinqCfg.uiPriority = uiPriority;

    iRet = DRV_SetPortQinqCfg(uiLPortId, &stQinqCfg);
    if (NO_ERROR != iRet)
    {
        DRV_DEBUG_PRINT("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    return NO_ERROR;
}

/*****************************************************************************
  














*****************************************************************************/
STATIC BOOL DRV_CheckQinqCfg(UINT32 uiLPortId, QINQ_CTAG_STAG_ENTRY_S *pstQinqCfg)
{
    UINT32 i,j;
    PORT_QINQ_S stQinqCfg;




    for (i = 1; i <= MAX_PORT_NUM; i++)
    {
        (VOID)DRV_GetPortQinqCfg(i, &stQinqCfg);

        /*
        (1)Different qinq entries of this port should not have same ctag.
        (2)Stag in one qinq entries should not be same with any ctag in other qinq entries of this port. */
        if (i == uiLPortId)
        {
            for (j = 0; j < stQinqCfg.uiCtagStagEntryNum; j++)
            {
                if (stQinqCfg.astCtagStagEntry[j].uiStagVid != pstQinqCfg->uiStagVid)
                {
                    UINT32 uiOldCtagIndex;
                    UINT32 uiNewCtagIndex;

                    for (uiOldCtagIndex = 0; uiOldCtagIndex < stQinqCfg.astCtagStagEntry[j].uiCtagVidNum; uiOldCtagIndex++)
                    {
                        for (uiNewCtagIndex = 0; uiNewCtagIndex < pstQinqCfg->uiCtagVidNum; uiNewCtagIndex++)
                        {
                            if ((stQinqCfg.astCtagStagEntry[j].uiStagVid == pstQinqCfg->auiCtagVid[uiNewCtagIndex]) ||
                                (stQinqCfg.astCtagStagEntry[j].auiCtagVid[uiOldCtagIndex] == pstQinqCfg->auiCtagVid[uiNewCtagIndex]))
                            {
                                return FALSE;
                            }
                        }

                        if (stQinqCfg.astCtagStagEntry[j].auiCtagVid[uiOldCtagIndex] == pstQinqCfg->uiStagVid)
                        {
                            return FALSE;
                        }
                    }
                }
            }

            continue;
        }

        /* It is invalid to insert diferrent stag befor the same ctag for upstream. */
        for (j = 0; j < stQinqCfg.uiCtagStagEntryNum; j++)
        {
            /* If there aready has been a qinq entry inserting same stag befor all ctag,
                other qinq entries are not allowded. */
            if (TRUE == stQinqCfg.astCtagStagEntry[j].bAllCtagVid)
            {
                if ((MAX_VLAN_NUM - 1) != pstQinqCfg->uiCtagVidNum)
                {
                    return FALSE;
                }
                else if (stQinqCfg.astCtagStagEntry[j].uiStagVid != pstQinqCfg->uiStagVid)
                {
                    return FALSE;
                }
            }
            /* Following situations are not allowed:
            (1)There are qinq entry that inserts same stag befor all ctag and other kind of entries.
            (2)There are diffrent qinq entries that have same stag.
            (3)There are diffrent qinq entries that have same ctag.
            */
            else
            {
                if ((MAX_VLAN_NUM - 1) == pstQinqCfg->uiCtagVidNum)
                {
                    return FALSE;
                }
                else
                {
                    if (stQinqCfg.astCtagStagEntry[j].uiStagVid == pstQinqCfg->uiStagVid)
                    {
                        return FALSE;
                    }
                    else
                    {
                        UINT32 uiOldCtagIndex;
                        UINT32 uiNewCtagIndex;

                        for (uiOldCtagIndex = 0; uiOldCtagIndex < stQinqCfg.astCtagStagEntry[j].uiCtagVidNum; uiOldCtagIndex++)
                        {
                            for (uiNewCtagIndex = 0; uiNewCtagIndex < pstQinqCfg->uiCtagVidNum; uiNewCtagIndex++)
                            {
                                if (stQinqCfg.astCtagStagEntry[j].auiCtagVid[uiOldCtagIndex] == pstQinqCfg->auiCtagVid[uiNewCtagIndex])
                                {
                                    return FALSE;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return TRUE;
}

/*****************************************************************************
















*****************************************************************************/
STATIC INT32 DRV_CreateUpStreamClassForQinq(UINT32 uiLPortId, UINT32 uiStagVid, UINT32 uiCtagVid, char *pstrClassName)
{
    INT32 Ret;






    /* Create class. */
    Ret = odmCreateClass(pstrClassName);
    if (QOS_OK != Ret)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /* Create classifier rule for upstream. */
    Ret = odmCfgClassFVid(pstrClassName,(UINT16)uiCtagVid,CLASS_ADD);
    if (QOS_OK != Ret)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /* Create rule's action for upstream. */
    Ret = odmCfgClassRmkVid(pstrClassName, INSERTVID, (UINT16)uiStagVid, CLASS_ADD);
    if (QOS_OK != Ret)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /* Install class for upstream. */
    Ret = odmInstallClassEntry(pstrClassName, UP_STREAM_, NULL);
    if (QOS_OK != Ret)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    return NO_ERROR;
}

/*****************************************************************************















*****************************************************************************/
STATIC INT32 DRV_CreateDownStreamClassForQinq(UINT32 uiLPortId, UINT32 uiStagVid, char *pstrClassName)
{
    INT32 Ret;





    /* Create class. */
    Ret = odmCreateClass(pstrClassName);
    if (QOS_OK != Ret)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /* Create classifier rule for downstream. */
    Ret = odmCfgClassFVid(pstrClassName,(UINT16)uiStagVid,CLASS_ADD);
    if (QOS_OK != Ret)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /* Create rule's action for downstream. */
    /* Tag 0 will be delete via vtt when packets egress from GE port. */
    Ret = odmCfgClassRmkVid(pstrClassName, CHANGEVID, VLAN_ID_0, CLASS_ADD);
    if (QOS_OK != Ret)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /* Install class for downstream. */
    Ret = odmInstallClassEntry(pstrClassName, DOWN_STREAM_, NULL);
    if (NO_ERROR != Ret)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    return NO_ERROR;
}

/*****************************************************************************















*****************************************************************************/
INT32 DRV_SetPortQinq(UINT32 uiLPortId, QINQ_CTAG_STAG_ENTRY_S *pstQinqCfg)
{
    INT32 Ret;
    UINT32 i;
    UINT32 uiEnmptyEntyIndex = MAX_QINQ_CTAG_STAG_ENTRY;
    char strQinqClassName[QINQ_CLASS_NAME_LEN] = {0};
    PORT_QINQ_S stQinqCfg;




    memset(&stQinqCfg, 0, sizeof(stQinqCfg));
    Ret = DRV_GetPortQinqCfg(uiLPortId, &stQinqCfg);
    if (NO_ERROR != Ret)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    if (TRUE != stQinqCfg.bEnable)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    if (MAX_QINQ_CTAG_STAG_ENTRY <= stQinqCfg.uiCtagStagEntryNum)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    if (FALSE == DRV_CheckQinqCfg(uiLPortId, pstQinqCfg))
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /* Stag should not be same with qinq pvid. */
    if (pstQinqCfg->uiStagVid == stQinqCfg.uiPvid)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /* Ctag should not be same with qinq pvid. */
    if ((MAX_VLAN_NUM - 1) != pstQinqCfg->uiCtagVidNum)
    {
        for (i = 0; i < pstQinqCfg->uiCtagVidNum; i++)
        {
            if (pstQinqCfg->auiCtagVid[i] == stQinqCfg.uiPvid)
            {
                DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
                return ERROR;
            }
        }
    }

    for (i = 0; i < MAX_QINQ_CTAG_STAG_ENTRY; i++)
    {
        /* Get first enmpty entry. */
        if (DEFAULT_SVLAN_ID == stQinqCfg.astCtagStagEntry[i].uiStagVid)
        {
            if (MAX_QINQ_CTAG_STAG_ENTRY == uiEnmptyEntyIndex)
            {
                uiEnmptyEntyIndex = i;
                break;
            }
        }
    }

    if (MAX_QINQ_CTAG_STAG_ENTRY <= uiEnmptyEntyIndex)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /* Set upstream class rules for qinq. */
    /* Trust port. */
    if ((MAX_VLAN_NUM - 1) == pstQinqCfg->uiCtagVidNum)
    {
        Ret = HalQinqClfSet((UINT16)pstQinqCfg->uiStagVid, UP_STREAM);
        if (OPL_OK != Ret)
        {
            DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
            return ERROR;
        }

        /* Do not add extra stag for stream with qinq pvid. */
        Ret = dalVttEntryTagAdd(TRUE, TRUE, (UINT16)stQinqCfg.uiPvid, (UINT16)stQinqCfg.uiPvid);
        if (OPL_OK != Ret)
        {
            DRV_DEBUG_PRINT("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__);
            return ERROR;
        }

        stQinqCfg.astCtagStagEntry[0].bAllCtagVid = TRUE;
    }
    /* Trust vlan id. */
    else
    {
        if (MAX_QINQ_CTAG_NUM < pstQinqCfg->uiCtagVidNum)
        {
            DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
            return ERROR;
        }

        for (i = 0; i < pstQinqCfg->uiCtagVidNum; i++)
        {
            sprintf(strQinqClassName, "port%02dqinqupstag%04dctag%04d",uiLPortId,pstQinqCfg->uiStagVid,pstQinqCfg->auiCtagVid[i]);
            Ret = DRV_CreateUpStreamClassForQinq(uiLPortId, pstQinqCfg->uiStagVid, pstQinqCfg->auiCtagVid[i], strQinqClassName);
            if (NO_ERROR != Ret)
            {
                DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
                return ERROR;
            }

            stQinqCfg.astCtagStagEntry[uiEnmptyEntyIndex].uiCtagVidNum++;
            stQinqCfg.astCtagStagEntry[uiEnmptyEntyIndex].auiCtagVid[i] = pstQinqCfg->auiCtagVid[i];
            stQinqCfg.astCtagStagEntry[uiEnmptyEntyIndex].uiAclClassNum++;
            strcpy(stQinqCfg.astCtagStagEntry[uiEnmptyEntyIndex].astrAclClassName[i],strQinqClassName);
            DRV_DEBUG_PRINT("\nCreate Class %s, index %d num:%d.\n",stQinqCfg.astCtagStagEntry[uiEnmptyEntyIndex].astrAclClassName[i],uiEnmptyEntyIndex,i);
        }
    }

    /* Set downstream class rule for qinq. */
    sprintf(strQinqClassName, "port%02dqinqdownstag%04d",uiLPortId,pstQinqCfg->uiStagVid);
    Ret = DRV_CreateDownStreamClassForQinq(uiLPortId, pstQinqCfg->uiStagVid, strQinqClassName);
    if (NO_ERROR != Ret)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    strcpy(stQinqCfg.astCtagStagEntry[uiEnmptyEntyIndex].astrAclClassName[stQinqCfg.astCtagStagEntry[uiEnmptyEntyIndex].uiAclClassNum],strQinqClassName);
    DRV_DEBUG_PRINT("\nCreate Class %s.\n",stQinqCfg.astCtagStagEntry[uiEnmptyEntyIndex].astrAclClassName[stQinqCfg.astCtagStagEntry[uiEnmptyEntyIndex].uiAclClassNum]);
    stQinqCfg.astCtagStagEntry[uiEnmptyEntyIndex].uiAclClassNum++;
    stQinqCfg.uiCtagStagEntryNum++;
    stQinqCfg.astCtagStagEntry[uiEnmptyEntyIndex].uiStagVid = pstQinqCfg->uiStagVid;
    Ret = DRV_SetPortQinqCfg(uiLPortId, &stQinqCfg);
    if (NO_ERROR != Ret)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    return NO_ERROR;
}

/*****************************************************************************














*****************************************************************************/
INT32 DRV_ResetPortQinq(UINT32 uiLPortId)
{
    INT32 iRet;
    UINT32 i,j;
    PORT_QINQ_S stQinqCfg;

   

    iRet= DRV_GetPortQinqCfg(uiLPortId, &stQinqCfg);
    if (NO_ERROR != iRet)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    if (TRUE != stQinqCfg.bEnable)
    {
        return NO_ERROR;
    }

    for (i = 0; i < stQinqCfg.uiCtagStagEntryNum; i++)
    {
        /* Delete classifier for qinq rule that inserts stag for all upstreams */
        if (TRUE == stQinqCfg.astCtagStagEntry[i].bAllCtagVid)
        {
            iRet = HalQinqClfReset();
            if (NO_ERROR != iRet)
            {
                DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
                return ERROR;
            }

            /* Add qinq pvid for untagged stream. */
            iRet = dalVttEntryTagAdd(FALSE, TRUE, (UINT16)stQinqCfg.uiPvid, (UINT16)stQinqCfg.uiPvid);
            if (NO_ERROR != iRet)
            {
                DRV_DEBUG_PRINT("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__);
                return ERROR;
            }
        }

        for (j = 0; j < stQinqCfg.astCtagStagEntry[i].uiAclClassNum; j++)
        {
            DRV_DEBUG_PRINT("\nRemove class %s, entry:%d, num:%d.\n",stQinqCfg.astCtagStagEntry[i].astrAclClassName[j],i,j);
            /* Ubind acl rule. */
            iRet = odmRemoveClassEntry(stQinqCfg.astCtagStagEntry[i].astrAclClassName[j], 1);
            if ((NO_ERROR != iRet) &&
                (QOS_CLS_EXIST_CHECK_ERROR != iRet))
            {
                DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
                return ERROR;
            }

            /* Delete acl rule. */
            iRet = odmDestroyClass(stQinqCfg.astCtagStagEntry[i].astrAclClassName[j]);
            if (QOS_OK != iRet)
            {
                DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
                return ERROR;
            }

            stQinqCfg.astCtagStagEntry[i].astrAclClassName[j][0] = '\0';
        }

        stQinqCfg.astCtagStagEntry[i].bAllCtagVid = FALSE;
        stQinqCfg.astCtagStagEntry[i].uiAclClassNum = 0;
        stQinqCfg.astCtagStagEntry[i].uiCtagVidNum = 0;
        stQinqCfg.astCtagStagEntry[i].uiStagVid = DEFAULT_SVLAN_ID;
    }

    stQinqCfg.uiCtagStagEntryNum = 0;
    iRet= DRV_SetPortQinqCfg(uiLPortId, &stQinqCfg);
    if (NO_ERROR != iRet)
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    return NO_ERROR;
}


/* Support multicast feature */
#ifdef CTC_MULTICAST_SURPORT
//#if 0
/*****************************************************************************








------------------------------------------------------------------------------


  --------------------------------------------------------------------------

*****************************************************************************/
INT32 DRV_GetSwitchUplinkLPort(UINT32 *puiLPort)
{
    if(puiLPort == NULL)
	{
	    return ERROR;
	}

    *puiLPort = LOGIC_PON_PORT;

    return NO_ERROR;
}


/*****************************************************************************













  --------------------------------------------------------------------------

*****************************************************************************/
INT32 DRV_AddPort2McastAddr(UINT32 uiLPort, UINT32 uiVlan, UINT8 aucMac[MAC_ADDR_LEN])
{
    DRV_RET_E Ret;
	vlan_id_t tdVid;
	mac_address_t stMacAddress;
	mac_mcast_t stMacMcast;
	logic_pmask_t stLPortMask;

    if ((NULL == aucMac) || (FALSE == IsValidLgcPort(uiLPort))||(MAX_VLAN_ID < uiVlan))
    {
        return ERROR;
    }
	ClrLgcMaskAll(&stLPortMask);
    SetLgcMaskBit(uiLPort,&stLPortMask);

	memset(&stMacMcast, 0, sizeof(stMacMcast));
    memset(&stMacAddress, 0, sizeof(stMacAddress));
    memcpy(stMacAddress, aucMac, MAC_ADDR_LEN);

    tdVid = (vlan_id_t)uiVlan;
    #if 0
	Ret = Ioctl_GetMacMcastMac(tdVid, stMacAddress, &stMacMcast);
    #else
    Ret = Ioctl_ctc_mc_get_mac(tdVid, stMacAddress, &stMacMcast);
    #endif

    if (DRV_ERR_MAC_ADDR_NOT_FOUND == Ret)
    {
    	memset(&stMacMcast, 0, sizeof(stMacMcast));
        stMacMcast.tdVid = tdVid;
		LgcMaskCopy(&stMacMcast.port_mask,&stLPortMask);
    }
	else if (DRV_OK == Ret)
	{
	    LgcMaskOr(&(stMacMcast.port_mask), &stLPortMask);
	}
	else
	{
		return ERROR;
	}

    memcpy(stMacMcast.mac_addr, stMacAddress, sizeof(mac_address_t));

	mc_printf("\n%s portmsk:0x%04X mc-mac %02X%02X-%02X%02X-%02X%02X.\n",__FUNCTION__,
             stLPortMask.pbits[0],stMacMcast.mac_addr[0],stMacMcast.mac_addr[1],
             stMacMcast.mac_addr[2],stMacMcast.mac_addr[3],
             stMacMcast.mac_addr[4],stMacMcast.mac_addr[5]);
    #if 0
    Ret = Ioctl_SetMacSetMcastMac(stMacMcast);
    #else
    Ret = Ioctl_ctc_mc_set_mac(stMacMcast);
    #endif
    if (DRV_OK != Ret)
    {
        return ERROR;
    }

    return NO_ERROR;
}

/*****************************************************************************










------------------------------------------------------------------------------


  --------------------------------------------------------------------------

*****************************************************************************/
INT32 DRV_DelPortFromMcastAddr(UINT32 uiLPort, UINT32 uiVlan, UINT8 aucMac[MAC_ADDR_LEN])
{
    DRV_RET_E Ret;
	vlan_id_t tdVid;
	mac_address_t stMacAddress;
	mac_mcast_t stMacMcast;
	logic_pmask_t stLPortMask;

    if ((NULL == aucMac) || (FALSE == IsValidLgcPort(uiLPort))||(MAX_VLAN_ID < uiVlan))
    {
        mc_printf("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
	ClrLgcMaskAll(&stLPortMask);
    SetLgcMaskBit(uiLPort,&stLPortMask);

	memset(&stMacMcast, 0, sizeof(stMacMcast));
    memcpy(&stMacAddress[0], aucMac, sizeof(stMacAddress));

    tdVid = (vlan_id_t)uiVlan;
    #if 0
	Ret = Ioctl_GetMacMcastMac(tdVid, stMacAddress, &stMacMcast);
    #else
    Ret = Ioctl_ctc_mc_get_mac(tdVid, stMacAddress, &stMacMcast);
    #endif
	if(DRV_OK == Ret)
	{
        memcpy(stMacMcast.mac_addr, stMacAddress, sizeof(mac_address_t));

        mc_printf("\n%s portmsk:0x%04X mc-mac %02X%02X-%02X%02X-%02X%02X.\n",__FUNCTION__,
             stLPortMask.pbits[0],stMacMcast.mac_addr[0],stMacMcast.mac_addr[1],
             stMacMcast.mac_addr[2],stMacMcast.mac_addr[3],
             stMacMcast.mac_addr[4],stMacMcast.mac_addr[5]);

	    /*remove deleting mac portmask*/
        #if 0
	    //LgcMaskRemove(&(stMacMcast.port_mask), &stLPortMask);
	    if(FALSE == LgcMaskNotNull(&(stMacMcast.port_mask)))
	    {
	        /*delete*/
	        Ret = Ioctl_SetMacDelMcastMac(stMacMcast);
	        if(DRV_OK != Ret)
	        {
                mc_printf("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
	            return ERROR;
	        }
	    }
	    else
	    {
	        /*reset portmask for give mac*/
	        if (DRV_OK != Ioctl_SetMacSetMcastMac(stMacMcast))
        	{
                mc_printf("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
        	    return ERROR;
        	}
	    }
        #else
        LgcMaskAnd(&(stMacMcast.port_mask), &stLPortMask);
        #if 0
        Ret = Ioctl_SetMacDelMcastMac(stMacMcast);
        #else
        Ret = Ioctl_ctc_mc_del_mac(stMacMcast);
        #endif
        if(DRV_OK != Ret)
        {
            mc_printf("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
            return ERROR;
        }
        #endif
	}
	else
	{
	    /*not exist*/
        mc_printf("\nfunc:%s,line:%d\n",__FUNCTION__,__LINE__);
	    return ERROR;
	}

    return NO_ERROR;
}

/*****************************************************************************













*****************************************************************************/
STATIC INT32 DRV_CheckVlanForMcMode(UINT32 uiVlanId)
{
    INT32 iRet;
    UINT32 uiLPortIndex;
    CTC_VLAN_CFG_S stVlanMode;

   

    LgcPortFor(uiLPortIndex)
    {
		if (DRV_OK != Ioctl_ctc_get_acl_rule(uiLPortIndex, &stVlanMode))
	    {
	        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
	        return FALSE;
	    }

        if (CTC_VLAN_MODE_TRANSLATION == stVlanMode.mode)
        {
            UINT32 uiVlanIndex = 0;

            if (uiVlanId == stVlanMode.default_vlan)
            {
                return FALSE;
            }

            for (uiVlanIndex = 0; uiVlanIndex < stVlanMode.number_of_entries; uiVlanIndex++)
            {
                if ((uiVlanId == stVlanMode.vlan_list[uiVlanIndex*2]) ||
                    (uiVlanId == stVlanMode.vlan_list[uiVlanIndex*2 + 1]))
                {
                    return FALSE;
                }
            }
        }
        else if (CTC_VLAN_MODE_TAG == stVlanMode.mode)
        {
            if (uiVlanId == stVlanMode.default_vlan)
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/*****************************************************************************

*****************************************************************************/
INT32 DRV_AddPort2McastVlan(UINT32 uiLPort, UINT32 uiMcVid, MC_TAG_MODE_E enTagMode)
{
	DRV_RET_E ulRetVal = DRV_OK;
    logic_pmask_t stLMask, stUntagLMsk;

    if ((!IsValidLgcPort(uiLPort))   ||
        (!VALID_VLAN_ID(uiMcVid)) ||
        (MC_TAG_MODE_END < enTagMode))
    {
        return ERROR;
    }

    if (TRUE != DRV_CheckVlanForMcMode(uiMcVid))
    {
        return ERROR;
    }

	ClrLgcMaskAll(&stLMask);
	ClrLgcMaskAll(&stUntagLMsk);
    #if 0
	ulRetVal = Ioctl_GetVlanMember(uiMcVid, &stLMask, &stUntagLMsk);
    #else
    ulRetVal = Ioctl_ctc_mc_get_vlan_mem(uiMcVid, &stLMask, &stUntagLMsk);
    #endif
	if (DRV_OK != ulRetVal)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    switch (enTagMode)
    {
        case MC_TAG_MODE_TRANSPARENT:
            if (TstLgcMaskBit(uiLPort, &stLMask) && (!TstLgcMaskBit(uiLPort, &stUntagLMsk)))
            {
                return NO_ERROR;
            }
            else
            {
				SetLgcMaskBit(uiLPort, &stLMask);
                ClrLgcMaskBit(uiLPort, &stUntagLMsk);
                #if 0
				SetLgcMaskBit(LOGIC_CPU_PORT, &stLMask);
				SetLgcMaskBit(LOGIC_PON_PORT, &stLMask);
                #endif
            }

            break;
        case MC_TAG_MODE_STRIP:
            if (TstLgcMaskBit(uiLPort, &stLMask) && TstLgcMaskBit(uiLPort, &stUntagLMsk))
            {
                return NO_ERROR;
            }
            else
            {
                SetLgcMaskBit(uiLPort, &stLMask);
                #if 0
				SetLgcMaskBit(LOGIC_CPU_PORT, &stLMask);
				SetLgcMaskBit(LOGIC_UPPON_PORT, &stLMask);
                #endif
				SetLgcMaskBit(uiLPort,&stUntagLMsk);
            }

            break;
        default :
            /*Do not support mc vlan translation.*/
            return ERROR;
            break;
    }

	ulRetVal = Ioctl_SetMcVlanMemberAdd(uiMcVid, stLMask, stUntagLMsk);
    if (DRV_OK != ulRetVal)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
#if 0
    /* Begin  */
    {
    UINT32 bEnoughEntry = FALSE;
    CTC_VLAN_CFG_S stVlanMode;
    /*Check if there is enough vlan index entry for tag mode.
    We do this because rtl8305mb uses vlan index entry to implement acl vlan 
    remarking action.
    In rtl8305mb there are 32 vlan index entries.*/
    stVlanMode.default_vlan = uiMcVid;
    stVlanMode.mode = CTC_VLAN_MODE_TAG;
    stVlanMode.number_of_entries = 0;
    ulRetVal = Ioctl_ctc_enough_vlan_index_check(stVlanMode, (&bEnoughEntry));
    if ((NO_ERROR != ulRetVal) || (FALSE == bEnoughEntry))
    {
        mc_printf("func:%s,line:%d,ret:%d,en:%d.\n",
                   __FUNCTION__,__LINE__,ulRetVal,bEnoughEntry);
        return ERROR;  
    }

    ulRetVal = Ioctl_ctc_add_acl_rule_for_mvlan(uiLPort, uiMcVid);
    if (NO_ERROR != ulRetVal)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;  
    }
    }
    /*   */
#endif    
    return NO_ERROR;
}

/*****************************************************************************
  --------------------------------------------------------------------------

*****************************************************************************/
INT32 DRV_DelPortFromMcastVlan(UINT32 uiLPort, UINT32 uiMcVid)
{
	DRV_RET_E ulRetVal = DRV_OK;
    #if 0
    logic_pmask_t stLMask, stUntagLMsk, stTmpMsk;
    #else
    logic_pmask_t stLMask;
    #endif

    if ((!IsValidLgcPort(uiLPort))   ||
        (!VALID_VLAN_ID(uiMcVid)))
    {
        return ERROR;
    }

    /*Return error if this vid is not multicast vlan.*/
    if (TRUE != DRV_CheckVlanForMcMode(uiMcVid))
    {
        return ERROR;
    }

	ClrLgcMaskAll(&stLMask);
#if 0    
	ClrLgcMaskAll(&stUntagLMsk);

	ulRetVal = Ioctl_GetVlanMember(uiMcVid, &stLMask, &stUntagLMsk);
	if (DRV_OK != ulRetVal)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

	if(!TstLgcMaskBit(uiLPort, &stLMask))
	{
	    return NO_ERROR;
	}

	ClrLgcMaskBit(uiLPort, &stLMask);
	ClrLgcMaskBit(uiLPort, &stUntagLMsk);

	LgcMaskCopy(&stTmpMsk, &stLMask);
	ClrLgcMaskBit(LOGIC_CPU_PORT, &stTmpMsk);
	ClrLgcMaskBit(LOGIC_UPPON_PORT, &stTmpMsk);
	if(FALSE == LgcMaskNotNull(&stTmpMsk))
	{
		ClrLgcMaskAll(&stLMask);
		ClrLgcMaskAll(&stUntagLMsk);
	}

	ulRetVal = Ioctl_SetMcVlanMemberAdd(uiMcVid, stLMask, stUntagLMsk);
    if (DRV_OK != ulRetVal)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    {
    /*Delete acl rule used by mc vlan.*/
    ulRetVal = Ioctl_ctc_del_acl_rule_for_mvlan(uiLPort, uiMcVid);
    if (NO_ERROR != ulRetVal)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;  
    }

    /*Clear port base vlan entry used by mc vlan.*/
    ulRetVal = Ioctl_ctc_clr_port_base_vlan_entry_by_vid(uiLPort);
    if (DRV_OK != ulRetVal)
    {
        mc_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;  
    }
    }
   
#else
    SetLgcMaskBit(uiLPort, &stLMask);
    ulRetVal = Ioctl_ctc_mc_rmv_vlan_mem(uiMcVid, stLMask);
    if (DRV_OK != ulRetVal)
    {
        diag_printf("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;  
    }
#endif	
    return NO_ERROR;
}

/*****************************************************************************
  --------------------------------------------------------------------------

*****************************************************************************/
INT32 DRV_SwitchMcInit(void)
{
    INT32 Ret;
    UINT32 uiLPortId;

    /*Trap igmp protocol packet to cpu.*/
	Ret = Ioctl_SetIgmpPktAction(PASS_TYPE_CPU_ONLY);
    if (DRV_OK != Ret)
    {
        mc_printf("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
#if 0
    /*Drop unknown igmp data.*/
	Ret = Ioctl_SetMcastLookupMissDrop(ENABLE);
    if (DRV_OK != Ret)
    {
        mc_printf("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }
#endif
    return NO_ERROR;
}

/*****************************************************************************
  函 数 名: DRV_EnableSwitchMc
  --------------------------------------------------------------------------

*****************************************************************************/
INT32 DRV_EnableSwitchMc(INT32 iEnable)
{
    INT32 Ret;
    UINT32 uiLPortId;

    if ((TRUE != iEnable) && (FALSE != iEnable))
    {
        return ERROR;
    }

    if (TRUE == iEnable)
    {
        /*Trap igmp protocol packet to cpu.*/
        Ret = Ioctl_SetIgmpPktAction(PASS_TYPE_CPU_ONLY);
        if (DRV_OK != Ret)
        {
            return ERROR;
        }

        /*Drop unknown igmp data.*/
		//Ret = Ioctl_SetMcastLookupMissDrop(ENABLE);
		Ret = Ioctl_igmp_unknow_ip4_act(MCAST_ACT_DROP);
        if (DRV_OK != Ret)
        {
            return ERROR;
        }
    }
    else
    {
        /*Forward igmp protocol packet to any port.*/
        Ret = Ioctl_SetIgmpPktAction(PASS_TYPE_ALLPORTS);
        if (DRV_OK != Ret)
        {
            return ERROR;
        }

        /*Do not drop unknown igmp data.*/
        //Ret = Ioctl_SetMcastLookupMissDrop(DISABLE);
        Ret = Ioctl_igmp_unknow_ip4_act(MCAST_ACT_FORWARD);
        if (DRV_OK != Ret)
        {
            return ERROR;
        }
    }

    return NO_ERROR;
}

/*****************************************************************************
  --------------------------------------------------------------------------

*****************************************************************************/
INT32 DRV_SendFrame2SwitchPort(UINT32 uiLPort, INT8 *pPacket, UINT32 uiLength)
{
#if 0
    INT16 sRet = 0;
    UINT16 usVlanId = 0;
    UINT32 uiPortIdx = 0;
    BOOL_T bIsBcast = BOOL_FALSE;
    RTL_HEAD_S stRtlHead;

    if ((NULL == pPacket)            ||
        (MIN_PACKET_SIZE > uiLength) ||
        (!IsValidLgcPort(uiLPort)))
    {
        mc_printf("\nfunc:%s Error parameter.\n",__FUNCTION__);
        return ERROR;
    }

    bIsBcast = IS_MAC_BROADCAST(pPacket);

    if (( pPacket[12] == 0x81) && ( pPacket[13] == 0))
    {
        usVlanId = ((pPacket[14] & 0xF) << 8) + pPacket[15];
        if((0 == usVlanId) || (0xFFF == usVlanId))
        {
            memmove(pPacket + 12, pPacket + 16, uiLength - 16);
            pPacket[uiLength - 4] = 0;
            pPacket[uiLength - 3] = 0;
            pPacket[uiLength - 2] = 0;
            pPacket[uiLength - 1] = 0;
            uiLength = (uiLength >= MIN_PACKET_SIZE + 4)? uiLength - 4 : MIN_PACKET_SIZE;
        }
    }

	if (LOGIC_PON_PORT == uiLPort)
	{
        sRet = eopl_send_to_uplink(0, (VOID *)pPacket, uiLength);
        if (NO_ERROR != sRet)
        {
            mc_printf("\nfunc:%s failed to call eopl_send_to_uplink.\n",__FUNCTION__);
            return ERROR;
        }

		return NO_ERROR;
	}

    /*rtl head*/
    memset(&stRtlHead, 0, sizeof(stRtlHead));

    stRtlHead.usEthType = 0x8899;
    stRtlHead.ucProtocol = 0x4;
    stRtlHead.ucPrioritySelect = 1;
    stRtlHead.ucPriority = 7;

    if (BOOL_TRUE == bIsBcast)
	{
	    for (uiPortIdx = MIN_UNI_PORT_ID; uiPortIdx <= MAX_UNI_PORT_ID; uiPortIdx++)
    	{
            stRtlHead.usPortNo |= 1 << DRV_PortLogical2Physical(uiPortIdx);
		}
	}
	else
	{
        stRtlHead.usPortNo |= 1 << DRV_PortLogical2Physical(uiLPort);
	}

    memmove(pPacket + RTL_HEAD_OFF + RTL_HEAD_LEN, pPacket + RTL_HEAD_OFF, uiLength - RTL_HEAD_OFF);
    memcpy(pPacket + RTL_HEAD_OFF, &stRtlHead, RTL_HEAD_LEN);

    uiLength += RTL_HEAD_LEN;

    (VOID)syscall(NR_TEST_SETDATA, pPacket, uiLength);
#endif
	return NO_ERROR;
}


UINT32 DRV_GetSwitchHeaderLen(void)
{
    //return sizeof(RTL_HEAD_S);
    return 4;
}

INT32 DRV_GetPortFromSwitchHeader(UINT8 *pucData, UINT32 *puiPPort)
{
#if 0
    RTL_HEAD_S *pstInHeader = NULL;

    if ((NULL == pucData) || (NULL == puiPPort))
    {
        return ERROR;
    }

    pstInHeader = (RTL_HEAD_S *)pucData;
    *puiPPort = pstInHeader->usPortNo;
#endif
    return NO_ERROR;
}
#endif

/*****************************************************************************

*****************************************************************************/
VOID DRV_InitQinqCfg(VOID)
{
    UINT32 uiPorIndex;
    UINT32 uiAclIndex;

    for (uiPorIndex = 0; uiPorIndex <= MAX_PORT_NUM; uiPorIndex++)
    {
        for (uiAclIndex = 0; uiAclIndex < MAX_QINQ_CTAG_STAG_ENTRY; uiAclIndex ++)
        {
            g_astUniQinqMode[uiPorIndex].astCtagStagEntry[uiAclIndex].uiStagVid = DEFAULT_SVLAN_ID;
        }
    }
    return;
}

/*****************************************************************************
  --------------------------------------------------------------------------

*****************************************************************************/
static INT32 Drv_ParseRtctResult(VCT_CABLE_DIAG_S *pstCableDiag, cable_diag_t *pstDiag)
{
    UINT8 ucIdx = 0;

   
 

    for ( ucIdx = CABLE_PAIR_0; ucIdx < CABLE_PAIR_NUM; ucIdx++ )
    {
        switch ( pstDiag->pair_state[ucIdx] )
        {
            case CABLE_STATE_OK:
                pstCableDiag->aenCableStatus[ucIdx] = VCT_NORMAL_CABLE;
                break;
            case CABLE_STATE_OPEN:
                pstCableDiag->aenCableStatus[ucIdx] = VCT_OPEN_CABLE;
                break;
            case CABLE_STATE_SHORT:
                pstCableDiag->aenCableStatus[ucIdx] = VCT_SHORT_CABLE;
                break;
            default:
                pstCableDiag->aenCableStatus[ucIdx] = VCT_IMPEDANCE_MISMATCH;
				break;
        }
		
        /* 014091 */
        if (VCT_NORMAL_CABLE == pstCableDiag->aenCableStatus[ucIdx])
		{
		    pstCableDiag->ausCableLen[ucIdx] = 0xFFFF;
		}
		else
		{
		    pstCableDiag->ausCableLen[ucIdx] = pstDiag->pair_len[ucIdx];
		}
        /* 014091 */		       
    }

    return NO_ERROR;
}

/*****************************************************************************

*****************************************************************************/
INT32 DRV_GetPortVct(ULONG ulLPort, VCT_CABLE_DIAG_S *pstCableDiag )
{
#if defined(CONFIG_PRODUCT_EPN200)
    ulLPort = ulLPort;
    pstCableDiag = pstCableDiag;
    return ERROR;
#else
    cable_diag_t stDiag;
    INT32 iRet = 0;
    BOOL bPortState = FALSE;
    UINT8 ucPairIdx = 0;

    /* 参数检查 */
   
 

    memset(&stDiag, 0, sizeof(stDiag));

	/* 获取端口link状态 */
    iRet = DRV_GetPortLinkStatus(ulLPort, &bPortState);
    if ( NO_ERROR != iRet )
    {
        DRV_DEBUG_PRINT("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

	iRet = Ioctl_get_cable_diag(ulLPort, &stDiag);
    if (DRV_OK != iRet)
    {
        DRV_DEBUG_PRINT("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;
    }

    /*  端口LINK UP的情况下，不取cable实际测试结果，直接返回cable状态normal。 */
    if (TRUE == bPortState)
    {
        for ( ucPairIdx = CABLE_PAIR_0 ; ucPairIdx < CABLE_PAIR_NUM; ucPairIdx++ )
        {
            stDiag.pair_state[ucPairIdx] = CABLE_STATE_OK;
        }
    }

    (VOID)Drv_ParseRtctResult(pstCableDiag, &stDiag);

    return NO_ERROR;
#endif
}


/*****************************************************************************













*****************************************************************************/
INT32 DRV_BlockInnerUni(BOOL bUniEnable)
{
    UINT32 uiValue = 0;
   

	uiValue = (TRUE == bUniEnable)? 0 : 1;/* TRUE表示堵塞 */
	if(NO_ERROR != oplRegFieldWrite(REG_BRG_GE_PORT_TABLE, 0, 1, uiValue))
	{
	    DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
	}
    if(NO_ERROR != oplRegFieldWrite(REG_BRG_PON_PORT_TABLE, 0, 1, uiValue))
    {
        DRV_DEBUG_PRINT("\nfunc:%s line:%d\n",__FUNCTION__,__LINE__);
    }
	return NO_ERROR;
}

#if defined(CONFIG_PRODUCT_EPN200)
INT32 DRV_SingleMdioDbgWrite(UINT32 uiReg, UINT32 uiData)
{
    (void)Ioctl_SetExtPhyDbgReg(0, uiReg, uiData);
    return NO_ERROR;
}

INT32 DRV_SingleMdioDbgRead(UINT32 uiReg, UINT32 *puiData)
{
    (void)Ioctl_GetExtPhyDbgReg(0, uiReg, puiData);
    return NO_ERROR;
}

INT32 DRV_ExtPhyInit(void)
{
    UINT32 uiData;

    /*Set phy auto-negotiation register to advertise phy abilities:
    close pause/close 1000M-half/enable 1000M-full
    */
    (void)Ioctl_GetExtPhyMiiReg(0, 4, &uiData);
    /*close pause*/
    uiData &= ~((1U << 7) | (1U << 8));
    /*close 1000m/half*/
    uiData &= ~(1U << 6);
    /*enable 1000m/full*/
    uiData |= (1U << 5);
    (void)Ioctl_SetExtPhyMiiReg(0, 4, uiData);
    
    /*Set phy control register*/
    (void)Ioctl_GetExtPhyMiiReg(0, 0, &uiData);
    /*Enable auto-negotiation*/
    uiData |= (1U << 12);
    /*Set 1000M mode*/
    uiData &= ~(1U << 13);
    uiData |= (1U << 6);
    /*Set full mode*/
    uiData |= (1U << 8);
    /*Restart auto-negotiation*/
    uiData |= (1U << 9);
    (void)Ioctl_SetExtPhyMiiReg(0, 0, uiData);
    
    return NO_ERROR;
}
#endif
/* End   014334 */

/* Begin*/
#ifdef CTC_MULTICAST_SURPORT
/*****************************************************************************
    Func Name: CTC_CheckValidVidForMcvlan
 
  Description: Mc vlan id can not be same with ctc vlan id.
        Input: UINT32 uiMcVid  
       Output: 
       Return: BOOL
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/
BOOL CTC_CheckValidVidForMcvlan(UINT32 uiMcVid)
{
    UINT32 uiLPortIndex;
    CTC_VLAN_CFG_S stVlanMode;

    if (!VALID_VLAN_ID(uiMcVid))
    {
        return FALSE;
    }

    LgcPortFor(uiLPortIndex)
    {
        (VOID)_CTC_GetPortVlanModeCfg(uiLPortIndex, &stVlanMode);
        if (CTC_VLAN_MODE_TRANSLATION == stVlanMode.mode)
        {
            UINT32 uiVlanIndex = 0;

            if (uiMcVid == stVlanMode.default_vlan)
            {
                return FALSE;
            }
            
            for (uiVlanIndex = 0; uiVlanIndex < stVlanMode.number_of_entries; uiVlanIndex++)
            {
                if ((uiMcVid == stVlanMode.vlan_list[uiVlanIndex<<1]) ||
                    (uiMcVid == stVlanMode.vlan_list[(uiVlanIndex<<1) + 1]))
                {
                    return FALSE;
                }
            }
        }
        else if (CTC_VLAN_MODE_TAG == stVlanMode.mode)
        {
            if (uiMcVid == stVlanMode.default_vlan)
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}
#endif
/* D014040 */
#ifdef CONFIG_BOSA
INT32 DRV_25L90ImodSet(UINT32 uiModVal)
{
    OPL_STATUS iRet;
    iRet = dal25L90ImodSet(uiModVal);
    if (NO_ERROR != iRet)
    {
        DRV_DEBUG_PRINT("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;  
    }    
    return NO_ERROR;
}

INT32 DRV_25L90ApcSet(UINT32 uiApcVal)
{
    OPL_STATUS iRet;
    iRet = dal25L90ApcSet(uiApcVal);
    if (NO_ERROR != iRet)
    {
        DRV_DEBUG_PRINT("func:%s,line:%d.\n",__FUNCTION__,__LINE__);
        return ERROR;  
    }    
    return NO_ERROR;
}
#endif

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

