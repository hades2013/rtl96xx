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
 * $Revision: 39162 $
 * $Date: 2013-05-06 18:52:13 +0800 (Mon, 06 May 2013) $
 *
 * Purpose : EPON MAC register access APIs
 *
 * Feature : Provide the APIs to access EPON MAC
 *
 */
#ifndef _EPON_H_
#define _EPON_H_

#include <common/rt_error.h>
#include <common/rt_type.h>


#define RTK_EPON_KEY_SIZE 3
#define RTK_EPON_MAX_QUEUE_PER_LLID 8
#define RTK_EPON_MAX_LLID 8

#define RTK_EPON_MAX_LASER_ON_TIME  64
#define RTK_EPON_MAX_LASER_OFF_TIME 64

/*
 * EPON MAC Laser Status
 */
typedef enum rtk_epon_laser_status_e{
    RTK_EPON_LASER_STATUS_NORMAL = 0,
    RTK_EPON_LASER_STATUS_FORCE_ON = 1,
    RTK_EPON_LASER_STATUS_FORCE_OFF = 2,
    RTK_EPON_LASER_STATUS_END
}rtk_epon_laser_status_t;


typedef enum rtk_epon_polarity_e{
    EPON_POLARITY_LOW=0,
    EPON_POLARITY_HIGH,
    EPON_POLARITY_END
}rtk_epon_polarity_t;


typedef enum rtk_epon_intrType_e{
    EPON_INTR_TIMEDRIFT=0,
    EPON_INTR_MPCPTIMEOUT,
    EPON_INTR_REG_FIN,
    EPON_INTR_LOS,
    EPON_INTR_END
}rtk_epon_intrType_t;

typedef struct rtk_epon_regReq_s{
    uint8           llidIdx;
    rtk_mac_t       mac;
    uint8           pendGrantNum;
    rtk_enable_t    doRequest;
}rtk_epon_regReq_t;



typedef struct rtk_epon_churningKeyEntry_s
{
    uint8   llidIdx;
    uint8   keyIdx;
    uint8   churningKey[RTK_EPON_KEY_SIZE];
}rtk_epon_churningKeyEntry_t;

typedef struct rtk_epon_llidCounter_s
{
    uint32  queueTxFrames[RTK_EPON_MAX_QUEUE_PER_LLID];
    uint32  mpcpTxReport;
    uint32  mpcpRxGate;
    uint32  onuLlidNotBcst;
}rtk_epon_llidCounter_t;


typedef struct rtk_epon_counter_s
{
    rtk_epon_llidCounter_t  llidIdxCnt;
    uint8   llidIdx; /*indicate LLID relative counter is get from which LLID index*/
    uint32  mpcpRxDiscGate;
    uint32  fecCorrectedBlocks;    
    uint32  fecUncorrectedBlocks;
    uint32  fecCodingVio;
    uint32  notBcstBitNotOnuLlid;
    uint32  bcstBitPlusOnuLLid;
    uint32  bcstNotOnuLLid;
    uint32  crc8Err;
    uint32  mpcpTxRegRequest;
    uint32  mpcpTxRegAck;
}rtk_epon_counter_t;




typedef struct rtk_epon_llid_entry_s
{
    uint8          llidIdx;
    uint16         llid;
    rtk_enable_t   valid;
    uint8          reportTimer;
    rtk_enable_t   isReportTimeout; /*read only*/
	rtk_mac_t 	   mac;
}rtk_epon_llid_entry_t;



typedef enum rtk_epon_churning_mode_e{
    RTK_EPON_CHURNING_NORMAL = 0,
    RTK_EPON_CHURNING_BL = 1,
    RTK_EPON_CHURNING_END
}rtk_epon_churning_mode_t;



typedef enum rtk_epon_report_mode_e{
    RTK_EPON_REPORT_NORMAL   = 0,
    RTK_EPON_REPORT_0_F      = 1,
    RTK_EPON_REPORT_FORCE_0  = 2,
    RTK_EPON_REPORT_FORCE_F  = 3,
    RTK_EPON_REPORT_END
}rtk_epon_report_mode_t;



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
extern int32 rtk_epon_init(void);  




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
extern int32 rtk_epon_intrMask_get(rtk_epon_intrType_t intrType, rtk_enable_t *pState);


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
extern int32 rtk_epon_intrMask_set(rtk_epon_intrType_t intrType, rtk_enable_t state);




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
extern int32 rtk_epon_intr_get(rtk_epon_intrType_t intrType, rtk_enable_t *pState);


/* Function Name:
 *      rtk_epon_intr_disableAll
 * Description:
 *      Disable all of top interrupt for EPON
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 rtk_epon_intr_disableAll(void);  


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
extern int32 rtk_epon_llid_entry_set(rtk_epon_llid_entry_t *pLlidEntry);  

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
extern int32 rtk_epon_llid_entry_get(rtk_epon_llid_entry_t *pLlidEntry);  


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
extern int32  rtk_epon_forceLaserState_set(rtk_epon_laser_status_t laserStatus);

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
extern int32 rtk_epon_forceLaserState_get(rtk_epon_laser_status_t *pLaserStatus);


/* Function Name:
 *      rtk_epon_laserTime_set
 * Description:
 *      Set laserTime value
 * Input:
 *      laserOnTime:  OE module laser on time
 *      laserOffTime: OE module laser off time
 * Output:
 *      None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32  rtk_epon_laserTime_set(uint8 laserOnTime, uint8 laserOffTime);

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
extern int32 rtk_epon_laserTime_get(uint8 *pLaserOnTime, uint8 *pLaserOffTime);

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
extern int32 rtk_epon_syncTime_get(uint8 *pSyncTime);


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
extern int32 rtk_epon_registerReq_get(rtk_epon_regReq_t *pRegEntry);

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
extern int32 rtk_epon_registerReq_set(rtk_epon_regReq_t *pRegEntry);



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
extern int32 rtk_epon_churningKey_set(rtk_epon_churningKeyEntry_t *pEntry);


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
extern int32 rtk_epon_churningKey_get(rtk_epon_churningKeyEntry_t *pEntry);


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
extern int32 rtk_epon_usFecState_get(rtk_enable_t *pState);


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
extern int32 rtk_epon_usFecState_set(rtk_enable_t state);


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
extern int32 rtk_epon_dsFecState_get(rtk_enable_t *pState);


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
extern int32 rtk_epon_dsFecState_set(rtk_enable_t state);


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
extern int32 rtk_epon_mibCounter_get(rtk_epon_counter_t *pCounter);



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
extern int32
rtk_epon_mibGlobal_reset(void);


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
extern int32
rtk_epon_mibLlidIdx_reset(uint8 llidIdx);




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
extern int32
rtk_epon_losState_get(rtk_enable_t *pState);



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
extern int32
rtk_epon_mpcpTimeoutVal_get(uint32 *pTimeVal);



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
extern int32
rtk_epon_mpcpTimeoutVal_set(uint32 timeVal);



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
extern int32
rtk_epon_opticalPolarity_set(rtk_epon_polarity_t polarity);

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
extern int32
rtk_epon_opticalPolarity_get(rtk_epon_polarity_t *pPolarity);




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
extern int32 rtk_epon_fecState_get(rtk_enable_t *pState);


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
extern int32 rtk_epon_fecState_set(rtk_enable_t state);



#endif /*#ifndef _EPON_H_*/

