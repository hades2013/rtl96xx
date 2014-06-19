/*
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 * *
 * $Revision: 39315 $
 * $Date: 2012-10-16
 *
 * Purpose : EPON MAC register access APIs
 *
 * Feature : Provide the APIs to access EPON MAC
 *
 */

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>
#include <rtk/epon.h>


/* Function Name:
 *      rtk_epon_init
 * Description:
 *      epon register level initial function
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_init(void)  
{
    rtdrv_eponCfg_t epon_cfg;

    /* function body */
    SETSOCKOPT(RTDRV_EPON_INIT, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_init */




/* Function Name:
 *      rtk_epon_intrMask_get
 * Description:
 *      Get EPON interrupt mask
 * Input:
 *	  intrType: type of interrupt
 * Output:
 *      pState: point of get interrupt state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_intrMask_get(rtk_epon_intrType_t intrType, rtk_enable_t *pState)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&epon_cfg.intrType, &intrType, sizeof(rtk_epon_intrType_t));
    GETSOCKOPT(RTDRV_EPON_INTRMASK_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pState, &epon_cfg.state, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_epon_intrMask_get */


/* Function Name:
 *      rtk_epon_intrMask_set
 * Description:
 *      Set EPON interrupt mask
 * Input:
 * 	    intrType: type of top interrupt
 *      state: set interrupt mask state
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_intrMask_set(rtk_epon_intrType_t intrType, rtk_enable_t state)
{
    rtdrv_eponCfg_t epon_cfg;

    /* function body */
    osal_memcpy(&epon_cfg.intrType, &intrType, sizeof(rtk_epon_intrType_t));
    osal_memcpy(&epon_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_EPON_INTRMASK_SET, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_intrMask_set */




/* Function Name:
 *      rtk_epon_intr_get
 * Description:
 *      Set EPON interrupt state
 * Input:
 * 	  intrType: type of interrupt
 * Output:
 *      pState: point for get  interrupt state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_intr_get(rtk_epon_intrType_t intrType, rtk_enable_t *pState)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&epon_cfg.intrType, &intrType, sizeof(rtk_epon_intrType_t));
    GETSOCKOPT(RTDRV_EPON_INTR_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pState, &epon_cfg.state, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_gpon_intr_get */


/* Function Name:
 *      rtk_epon_intr_disableAll
 * Description:
 *      Disable all interrupt for EPON
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_intr_disableAll(void)  
{
    rtdrv_eponCfg_t epon_cfg;

    /* function body */
    SETSOCKOPT(RTDRV_EPON_INTR_DISABLEALL, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_intr_disableAll */


/* Function Name:
 *      rtk_epon_llid_entry_set
 * Description:
 *      Set llid entry
 * Input:
 *      llidIdx: LLID table index
 * Output:
 *      pLlidEntry: LLID entry 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_llid_entry_set(rtk_epon_llid_entry_t *pLlidEntry)  
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pLlidEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&epon_cfg.llidEntry, pLlidEntry, sizeof(rtk_epon_llid_entry_t));
    SETSOCKOPT(RTDRV_EPON_LLID_ENTRY_SET, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_llid_entry_set */

/* Function Name:
 *      rtk_epon_llid_entry_get
 * Description:
 *      Get llid entry
 * Input:
 *      llidIdx: LLID table index
 * Output:
 *      pLlidEntry: LLID entry 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_llid_entry_get(rtk_epon_llid_entry_t *pLlidEntry)  
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pLlidEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&epon_cfg.llidEntry, pLlidEntry, sizeof(rtk_epon_llid_entry_t));
    GETSOCKOPT(RTDRV_EPON_LLID_ENTRY_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pLlidEntry, &epon_cfg.llidEntry, sizeof(rtk_epon_llid_entry_t));

    return RT_ERR_OK;
}   /* end of rtk_epon_llid_entry_get */


/* Function Name:
 *      rtk_epon_forceLaserState_set
 * Description:
 *      Set Force Laser status
 * Input:
 *      laserStatus: Force laser status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_epon_forceLaserState_set(rtk_epon_laser_status_t laserStatus)
{
    rtdrv_eponCfg_t epon_cfg;

    /* function body */
    osal_memcpy(&epon_cfg.laserStatus, &laserStatus, sizeof(rtk_epon_laser_status_t));
    SETSOCKOPT(RTDRV_EPON_FORCELASERSTATE_SET, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_forceLaserState_set */

/* Function Name:
 *      rtk_epon_forceLaserState_get
 * Description:
 *      Get Force Laser status
 * Input:
 *      None
 * Output:
 *      pLaserStatus: Force laser status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_forceLaserState_get(rtk_epon_laser_status_t *pLaserStatus)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pLaserStatus), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memcpy(&epon_cfg.laserStatus, pLaserStatus, sizeof(rtk_epon_laser_status_t));
    GETSOCKOPT(RTDRV_EPON_FORCELASERSTATE_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pLaserStatus, &epon_cfg.laserStatus, sizeof(rtk_epon_laser_status_t));

    return RT_ERR_OK;
}   /* end of rtk_epon_forceLaserState_get */


/* Function Name:
 *      rtk_epon_laserTime_set
 * Description:
 *      Set laserTime value
 * Input:
 *      lasetOnTime:  OE module laser on time
 *      lasetOffTime: OE module laser off time
 * Output:
 *      plaserStatus: 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_epon_laserTime_set(uint8 lasetOnTime, uint8 lasetOffTime)
{
    rtdrv_eponCfg_t epon_cfg;

    /* function body */
    osal_memcpy(&epon_cfg.lasetOnTime, &lasetOnTime, sizeof(uint8));
    osal_memcpy(&epon_cfg.lasetOffTime, &lasetOffTime, sizeof(uint8));
    SETSOCKOPT(RTDRV_EPON_LASERTIME_SET, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_laserTime_set */

/* Function Name:
 *      rtk_epon_laserTime_get
 * Description:
 *      Get laser Time value
 * Input:
 *      None
 * Output:
 *      pLasetOnTime:  OE module laser on time
 *      pLasetOffTime: OE module laser off time
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_laserTime_get(uint8 *pLasetOnTime, uint8 *pLasetOffTime)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pLasetOnTime), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pLasetOffTime), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_EPON_LASERTIME_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pLasetOnTime, &epon_cfg.lasetOnTime, sizeof(uint8));
    osal_memcpy(pLasetOffTime, &epon_cfg.lasetOffTime, sizeof(uint8));

    return RT_ERR_OK;
}   /* end of rtk_epon_laserTime_get */

/* Function Name:
 *      rtk_epon_syncTime_get
 * Description:
 *      Get sync Time value
 * Input:
 *      None
 * Output:
 *      pSyncTime  : olt assigned sync time
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_syncTime_get(uint8 *pSyncTime)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pSyncTime), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_EPON_SYNCTIME_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pSyncTime, &epon_cfg.syncTime, sizeof(uint8));

    return RT_ERR_OK;
}   /* end of rtk_epon_syncTime_get */


/* Function Name:
 *      rtk_epon_registerReq_get
 * Description:
 *      Get register request relative parameter
 * Input:
 *      pRegEntry : register request relative parament
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_registerReq_get(rtk_epon_regReq_t *pRegEntry)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegEntry), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memcpy(&epon_cfg.regEntry, pRegEntry, sizeof(rtk_epon_regReq_t));
    GETSOCKOPT(RTDRV_EPON_REGISTERREQ_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pRegEntry, &epon_cfg.regEntry, sizeof(rtk_epon_regReq_t));

    return RT_ERR_OK;
}   /* end of rtk_epon_registerReq_get */

/* Function Name:
 *      rtk_epon_registerReq_set
 * Description:
 *      Set register request relative parameter
 * Input:
 *       None
 * Output:
 *       pRegEntry : register request relative parament 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_registerReq_set(rtk_epon_regReq_t *pRegEntry)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&epon_cfg.regEntry, pRegEntry, sizeof(rtk_epon_regReq_t));
    SETSOCKOPT(RTDRV_EPON_REGISTERREQ_SET, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_registerReq_set */



/* Function Name:
 *      rtk_epon_churningKey_set
 * Description:
 *      Set churning key entry
 * Input:
 *       pEntry : churning key relative parameter 
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_churningKey_set(rtk_epon_churningKeyEntry_t *pEntry)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&epon_cfg.entry, pEntry, sizeof(rtk_epon_churningKeyEntry_t));
    SETSOCKOPT(RTDRV_EPON_CHURNINGKEY_SET, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_churningKey_set */


/* Function Name:
 *      rtk_epon_churningKey_get
 * Description:
 *      Get churning key entry
 * Input:
 *       None
 * Output:
 *       pEntry : churning key relative parameter 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_churningKey_get(rtk_epon_churningKeyEntry_t *pEntry)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memcpy(&epon_cfg.entry, pEntry, sizeof(rtk_epon_churningKeyEntry_t));
    GETSOCKOPT(RTDRV_EPON_CHURNINGKEY_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pEntry, &epon_cfg.entry, sizeof(rtk_epon_churningKeyEntry_t));

    return RT_ERR_OK;
}   /* end of rtk_epon_churningKey_get */


/* Function Name:
 *      rtk_epon_usFecState_get
 * Description:
 *      Get upstream fec state
 * Input:
 *       None
 * Output:
 *       *pState : upstream FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_usFecState_get(rtk_enable_t *pState)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_EPON_USFECSTATE_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pState, &epon_cfg.state, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_epon_usFecState_get */


/* Function Name:
 *      rtk_epon_usFecState_set
 * Description:
 *      Set upstream fec state
 * Input:
 *       state : upstream FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_usFecState_set(rtk_enable_t state)
{
    rtdrv_eponCfg_t epon_cfg;

    /* function body */
    osal_memcpy(&epon_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_EPON_USFECSTATE_SET, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_usFecState_set */


/* Function Name:
 *      rtk_epon_dsFecState_get
 * Description:
 *      Get down-stream fec state
 * Input:
 *       None
 * Output:
 *       *pState : down-stream FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_dsFecState_get(rtk_enable_t *pState)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_EPON_DSFECSTATE_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pState, &epon_cfg.state, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_epon_dsFecState_get */


/* Function Name:
 *      rtk_epon_dsFecState_set
 * Description:
 *      Set down-stream fec state
 * Input:
 *       state : down-stream FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_dsFecState_set(rtk_enable_t state)
{
    rtdrv_eponCfg_t epon_cfg;

    /* function body */
    osal_memcpy(&epon_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_EPON_DSFECSTATE_SET, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_dsFecState_set */


/* Function Name:
 *      rtk_epon_dsFecState_set
 * Description:
 *      Set down-stream fec state
 * Input:
 *       None
 * Output:
 *       pCounter : EPON mib counter 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_mibCounter_get(rtk_epon_counter_t *pCounter)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCounter), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memcpy(&epon_cfg.counter, pCounter, sizeof(rtk_epon_counter_t));
    GETSOCKOPT(RTDRV_EPON_MIBCOUNTER_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pCounter, &epon_cfg.counter, sizeof(rtk_epon_counter_t));

    return RT_ERR_OK;
}   /* end of rtk_epon_mibCounter_get */



/* Function Name:
 *      rtk_epon_mibGlobal_reset
 * Description:
 *      Reset EPON global counters.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not reset Global Counter
 * Note:
 *      None
 */
int32
rtk_epon_mibGlobal_reset(void)
{
    rtdrv_eponCfg_t epon_cfg;

    /* function body */
    SETSOCKOPT(RTDRV_EPON_MIBGLOBAL_RESET, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_mibGlobal_reset */


/* Function Name:
 *      rtk_epon_mibLlidIdx_reset
 * Description:
 *      Reset the specified LLID index counters.
 * Input:
 *      llidIdx - LLID table index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_epon_mibLlidIdx_reset(uint8 llidIdx)
{
    rtdrv_eponCfg_t epon_cfg;

    /* function body */
    osal_memcpy(&epon_cfg.llidIdx, &llidIdx, sizeof(uint8));
    SETSOCKOPT(RTDRV_EPON_MIBLLIDIDX_RESET, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_mibLlidIdx_reset */




/* Function Name:
 *      rtk_epon_losState_get
 * Description:
 *      Get laser lose of signal state.
 * Input:
 *      pState LOS state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_epon_losState_get(rtk_enable_t *pState)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */

    GETSOCKOPT(RTDRV_EPON_LOSSTATE_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pState, &epon_cfg.state, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_epon_losState_get */

/* Function Name:
 *      rtk_epon_mpcpTimeoutVal_get
 * Description:
 *      Get mpcp time out value.
 * Input:
 *      pTimeVal - pointer of mpcp tomeout value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_epon_mpcpTimeoutVal_get(uint32 *pTimeVal)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimeVal), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&epon_cfg.timeVal, pTimeVal, sizeof(uint32));
    GETSOCKOPT(RTDRV_EPON_MPCPTIMEOUTVAL_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pTimeVal, &epon_cfg.timeVal, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_epon_mpcpTimeoutVal_get */



/* Function Name:
 *      rtk_epon_mpcpTimeoutVal_set
 * Description:
 *      Set mpcp time out value.
 * Input:
 *      timeVal - mpcp tomeout value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_epon_mpcpTimeoutVal_set(uint32 timeVal)
{
    rtdrv_eponCfg_t epon_cfg;

    /* function body */
    osal_memcpy(&epon_cfg.timeVal, &timeVal, sizeof(uint32));
    SETSOCKOPT(RTDRV_EPON_MPCPTIMEOUTVAL_SET, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_mpcpTimeoutVal_set */



/* Function Name:
 *      rtk_epon_opticalPolarity_set
 * Description:
 *      Set OE module polarity.
 * Input:
 *      polarity - OE module polarity
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_epon_opticalPolarity_set(rtk_epon_polarity_t polarity)
{
    rtdrv_eponCfg_t epon_cfg;

    /* function body */
    osal_memcpy(&epon_cfg.polarity, &polarity, sizeof(rtk_epon_polarity_t));
    SETSOCKOPT(RTDRV_EPON_OPTICALPOLARITY_SET, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_opticalPolarity_set */

/* Function Name:
 *      rtk_epon_opticalPolarity_get
 * Description:
 *      Set OE module polarity.
 * Input:
 *      pPolarity - pointer of OE module polarity
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_epon_opticalPolarity_get(rtk_epon_polarity_t *pPolarity)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPolarity), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&epon_cfg.polarity, pPolarity, sizeof(rtk_epon_polarity_t));
    GETSOCKOPT(RTDRV_EPON_OPTICALPOLARITY_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pPolarity, &epon_cfg.polarity, sizeof(rtk_epon_polarity_t));

    return RT_ERR_OK;
}   /* end of rtk_epon_opticalPolarity_get */




/* Function Name:
 *      rtk_epon_fecState_get
 * Description:
 *      Get EPON global fec state
 * Input:
 *       None
 * Output:
 *       *pState : glogal FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_fecState_get(rtk_enable_t *pState)
{
    rtdrv_eponCfg_t epon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&epon_cfg.state, pState, sizeof(rtk_enable_t));
    GETSOCKOPT(RTDRV_EPON_FECSTATE_GET, &epon_cfg, rtdrv_eponCfg_t, 1);
    osal_memcpy(pState, &epon_cfg.state, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_epon_fecState_get */


/* Function Name:
 *      rtk_epon_fecState_set
 * Description:
 *      Set EPON global fec state
 * Input:
 *       state : global FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_fecState_set(rtk_enable_t state)
{
    rtdrv_eponCfg_t epon_cfg;

    /* function body */
    osal_memcpy(&epon_cfg.state, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_EPON_FECSTATE_SET, &epon_cfg, rtdrv_eponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_epon_fecState_set */

