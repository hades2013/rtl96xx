/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 41000 $
 * $Date: 2013-07-10 13:14:11 +0800 (Wed, 10 Jul 2013) $
 *
 * Purpose : Definition of PON MAC API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) PON mac
 */

/*
 * Include Files
 */
#include <dal/rtl9601b/dal_rtl9601b.h>
#include <dal/rtl9601b/dal_rtl9601b_ponmac.h>
#include <osal/time.h>

/*
 * Symbol Definition
 */
/* Define Serdes index */
#define RTL9601B_SDS_IDX_LAN                0x00
#define RTL9601B_SDS_IDX_PON                0x01

/* Define Serdes pages */
/* Digital Serdes */
#define RTL9601B_SDS_PAGE_SDS               0x00
#define RTL9601B_SDS_PAGE_SDS_EXT           0x01
#define RTL9601B_SDS_PAGE_FIB               0x02
#define RTL9601B_SDS_PAGE_FIB_EXT           0x03
#define RTL9601B_SDS_PAGE_TGR_STD_0         0x04
#define RTL9601B_SDS_PAGE_TGR_STD_1         0x05
#define RTL9601B_SDS_PAGE_TGR_PRO_0         0x06
#define RTL9601B_SDS_PAGE_TGR_PRO_1         0x07
#define RTL9601B_SDS_PAGE_TGX_STD_0         0x08
#define RTL9601B_SDS_PAGE_TGX_STD_1         0x09
#define RTL9601B_SDS_PAGE_TGX_PRO_0         0x0A
#define RTL9601B_SDS_PAGE_TGX_PRO_1         0x0B
#define RTL9601B_SDS_PAGE_WDIG              0x1F
/* Analog Serdes */
#define RTL9601B_SDS_PAGE_MISC              0x20
#define RTL9601B_SDS_PAGE_COMMON            0x21
#define RTL9601B_SDS_PAGE_SPEED             0x22
#define RTL9601B_SDS_PAGE_SPEED_EXT         0x23
#define RTL9601B_SDS_PAGE_125G_SPEED        0x24    /* 1.25G Speed */
#define RTL9601B_SDS_PAGE_125G_SPEED_EXT    0x25    /* 1.25G Speed Ext */
#define RTL9601B_SDS_PAGE_25G_SPEED         0x26    /* 2.5G Speed */
#define RTL9601B_SDS_PAGE_25G_SPEED_EXT     0x27    /* 2.5G Speed Ext */
#define RTL9601B_SDS_PAGE_3125G_SPEED       0x28    /* 3.125G Speed */
#define RTL9601B_SDS_PAGE_3125G_SPEED_EXT   0x29    /* 3.125G Speed Ext */
#define RTL9601B_SDS_PAGE_5G_SPEED          0x2A    /* 5G Speed */
#define RTL9601B_SDS_PAGE_5G_SPEED_EXT      0x2B    /* 5G Speed Ext */
#define RTL9601B_SDS_PAGE_625G_SPEED        0x2C    /* 6.25G Speed */
#define RTL9601B_SDS_PAGE_625G_SPEED_EXT    0x2D    /* 6.25G Speed Ext */
#define RTL9601B_SDS_PAGE_10G_SPEED         0x2E    /* 10G Speed */
#define RTL9601B_SDS_PAGE_10G_SPEED_EXT     0x2F    /* 10G Speed Ext */
#define RTL9601B_SDS_PAGE_GPON_SPEED        0x30    /* GPON Speed */
#define RTL9601B_SDS_PAGE_GPON_SPEED_EXT    0x31    /* GPON Speed Ext */
#define RTL9601B_SDS_PAGE_EPON_GSPEED       0x32    /* EPON Speed */
#define RTL9601B_SDS_PAGE_EPON_SPEED_EXT    0x33    /* EPON Speed Ext */


#define RTL9601B_SDS_ACCESS_TIMEOUT     0x0010
struct rtl9601b_sds_acc_s {
    uint32 reserved:16;
    uint32 index:5;
    uint32 page:6;
    uint32 regaddr:5;
};

static uint32    ponmac_init = INIT_NOT_COMPLETED;
static rtk_ponmac_mode_t ponMode = PONMAC_MODE_GPON;

/*
 * Macro Declaration
 */


static int32 _ponmac_physicalQueueId_get(uint32 schedulerId, uint32 logicalQId, uint32 *pPhysicalQid)
{

#if(GPON_MAC_MODE)
    *pPhysicalQid = RTL9601B_TCONT_QUEUE_MAX *(schedulerId/8) + logicalQId;
#else
    *pPhysicalQid = RTL9601B_TCONT_QUEUE_MAX *(schedulerId/2) + logicalQId;
#endif

    return RT_ERR_OK;
}


/* Function Name:
 *      _rtl9601b_ponMacQueueDrainOut_set
 * Description:
 *      Set PON mac per queue draint out state
 * Input:
 *      qid 		- Specify the draint out Queue id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMacQueueDrainOutState_set(uint32 qid)
{
	//int32 ret; /* MODIFICATION needed */
    //uint32  data; /* MODIFICATION needed */
    //uint32  tmpData; /* MODIFICATION needed */
    //uint32  busy; /* MODIFICATION needed */
    //uint32  i; /* MODIFICATION needed */
    
   
	/* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

#if 0 /* MODIFICATION needed */
    reg_read(RTL9601B_PONMAC_DRN_CTRLr, &data);


    /* queue drain out */
    tmpData = 0;
    if ((ret = reg_field_set(RTL9601B_PONMAC_DRN_CTRLr, RTL9601B_PON_DRN_SELf, &tmpData,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_set(RTL9601B_PONMAC_DRN_CTRLr, RTL9601B_PON_DRN_IDXf, &qid,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    tmpData = 1;
    if ((ret = reg_field_set(RTL9601B_PONMAC_DRN_CTRLr, RTL9601B_PON_DRN_ENf, &tmpData,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /* drain out */
    if ((ret = reg_write(RTL9601B_PONMAC_DRN_CTRLr,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /* check busy bit */
    i = 0;
    do
    {
        reg_field_read(RTL9601B_PONMAC_DRN_CTRLr, RTL9601B_PON_DRN_BUSYf, &busy);
        i++;
    } while (busy && (i < 10000));

    /* stop drain out */
    data = 0;
    if ((ret = reg_write(RTL9601B_PONMAC_DRN_CTRLr,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#endif

    return RT_ERR_OK;
}



/* Function Name:
 *      _rtl9601b_ponMacQueueDrainOutState_get
 * Description:
 *      Get PON mac queue draint out state
 * Input:
 *      None
 * Output:
 *      pState - pon queue draint out status
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMacQueueDrainOutState_get(rtl9601b_ponmac_draintOutState_t *pState)
{
	//int32 ret; /* MODIFICATION needed */

	/* Error Checking */
    RT_PARAM_CHK((pState  == NULL), RT_ERR_NULL_POINTER);

#if 0 /* MODIFICATION needed */
    /* set to register */
    if ((ret = reg_field_read(RTL9601B_PONMAC_DRN_CTRLr, RTL9601B_PON_DRN_ENf, pState)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#endif

    return RT_ERR_OK;
}


/* Function Name:
 *      _rtl9601b_ponMacTcontDrainOutState_set
 * Description:
 *      Set PON mac per T-cont draint out state
 * Input:
 *      tcont 		- Specify the draint out T-cont id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_GPON_EXCEED_MAX_TCONT
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMacTcontDrainOutState_set(uint32 tcont)
{
	//int32 ret; /* MODIFICATION needed */
    //uint32 data; /* MODIFICATION needed */
    //uint32 tmpData; /* MODIFICATION needed */

	/* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT() <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);


#if 0 /* MODIFICATION needed */
    /* set to register */
    tmpData = 0;
    if ((ret = reg_field_set(RTL9601B_PONMAC_DRN_CTRLr, RTL9601B_PON_DRN_SELf, &tmpData,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_set(RTL9601B_PONMAC_DRN_CTRLr, RTL9601B_PON_DRN_IDXf, &tcont,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    tmpData = 1;
    if ((ret = reg_field_set(RTL9601B_PONMAC_DRN_CTRLr, RTL9601B_PON_DRN_ENf, &tmpData,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }


    /* set to register */
    if ((ret = reg_write(RTL9601B_PON_PORT_CTRLr,&data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#endif

    return RT_ERR_OK;
}



/* Function Name:
 *      _rtl9601b_ponMacTcontDrainOutState_get
 * Description:
 *      Get PON mac T-cont draint out state
 * Input:
 *      None
 * Output:
 *      pState - T-cont draint out status
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMacTcontDrainOutState_get(rtl9601b_ponmac_draintOutState_t *pState)
{
	//int32 ret; /* MODIFICATION needed */

	/* Error Checking */
    RT_PARAM_CHK((pState  == NULL), RT_ERR_NULL_POINTER);

#if 0 /* MODIFICATION needed */
    /* set to register */
    if ((ret = reg_field_write(RTL9601B_PONMAC_DRN_CTRLr, RTL9601B_PON_DRN_ENf, pState)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#endif

    return RT_ERR_OK;
}



/* Function Name:
 *      _rtl9601b_ponMacSidToQueueMap_set
 * Description:
 *      Set PON mac session id to queue maping
 * Input:
 *      sid 		- Specify the session id
 *      qid 		- Specify the mapping queue id id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMacSidToQueueMap_set(uint32 sid, uint32 qid)
{
	int32 ret;

	/* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((128 <= sid),RT_ERR_INPUT);

    /* set to register */
    if ((ret = reg_array_field_write(RTL9601B_PON_SID_TO_QIDr,REG_ARRAY_INDEX_NONE, sid, RTL9601B_QIDf, &qid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _rtl9601b_ponMacSidToQueueMap_get
 * Description:
 *      Get PON mac session id to queue maping
 * Input:
 *      tcont 		- Specify the draint out T-cont id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMacSidToQueueMap_get(uint32 sid, uint32 *pQid)
{
	int32 ret;

	/* Error Checking */
    RT_PARAM_CHK((128 <= sid),RT_ERR_INPUT);
    RT_PARAM_CHK((pQid  == NULL), RT_ERR_NULL_POINTER);

    /* set to register */
    if ((ret = reg_array_field_read(RTL9601B_PON_SID_TO_QIDr,REG_ARRAY_INDEX_NONE, sid, RTL9601B_QIDf, pQid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      _rtl9601b_ponMacCirRate_set
 * Description:
 *      Per queue set PON mac CIR rate
 * Input:
 *      qid 		- - Specify the queue id
 *      rate 		- Specify the CIR rate unit 64kbps
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMacCirRate_set(uint32 qid, uint32 rate)
{
	int32 ret;

	/* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((HAL_PONMAC_PIR_CIR_RATE_MAX() < rate),RT_ERR_INPUT);

    /* set to register */
    if ((ret = reg_array_field_write(RTL9601B_PON_QID_CIR_RATEr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_RATEf, &rate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      _rtl9601b_ponMacCirRate_get
 * Description:
 *      Per queue set PON mac CIR rate
 * Input:
 *      qid 		- - Specify the queue id
 * Output:
 *      pRate       - Specify the CIR rate unit 64kbps
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMacCirRate_get(uint32 qid, uint32 *pRate)
{
	int32 ret;

	/* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pRate  == NULL), RT_ERR_NULL_POINTER);

    /* set to register */
    if ((ret = reg_array_field_read(RTL9601B_PON_QID_CIR_RATEr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_RATEf, pRate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      _rtl9601b_ponMacPirRate_set
 * Description:
 *      Per queue set PON mac PIR rate
 * Input:
 *      qid 		- Specify the queue id
 *      rate 		- Specify the CIR rate unit 64kbps
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMacPirRate_set(uint32 qid, uint32 rate)
{
	int32 ret;

	/* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((HAL_PONMAC_PIR_CIR_RATE_MAX() < rate),RT_ERR_INPUT);

    /* set to register */
    if ((ret = reg_array_field_write(RTL9601B_PON_QID_PIR_RATEr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_RATEf, &rate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      _rtl9601b_ponMacPirRate_get
 * Description:
 *      Per queue set PON mac PIR rate
 * Input:
 *      qid 		- - Specify the queue id
 * Output:
 *      pRate       - Specify the PIR rate unit 64kbps
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMacPirRate_get(uint32 qid, uint32 *pRate)
{
	int32 ret;

	/* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((pRate  == NULL), RT_ERR_NULL_POINTER);

    /* set to register */
    if ((ret = reg_array_field_read(RTL9601B_PON_QID_PIR_RATEr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_RATEf, pRate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}





/* Function Name:
 *      _rtl9601b_ponMacScheQmap_set
 * Description:
 *      Per T-CONT/LLID set schedule queue map
 * Input:
 *      tcont 		- Specify T-CONT or LLID
 *      map 		- Specify schedule queue mapping mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMacScheQmap_set(uint32 tcont, uint32 map)
{
	int32 ret;

	/* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT() <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);

    /* set to register */
    if ((ret = reg_array_field_write(RTL9601B_PON_SCH_QMAPr,REG_ARRAY_INDEX_NONE, tcont, RTL9601B_MAPPING_TBLf, &map)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      _rtl9601b_ponMacScheQmap_get
 * Description:
 *      Per T-CONT/LLID get schedule queue map
 * Input:
 *      tcont 		- Specify T-CONT or LLID
 * Output:
 *      pMap 		- Specify schedule queue mapping mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMacScheQmap_get(uint32 tcont, uint32 *pMap)
{
	int32 ret;

	/* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT() <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);

    /* set to register */
    if ((ret = reg_array_field_read(RTL9601B_PON_SCH_QMAPr,REG_ARRAY_INDEX_NONE, tcont, RTL9601B_MAPPING_TBLf, pMap)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}




/* Function Name:
 *      _rtl9601b_ponMac_queueType_set
 * Description:
 *      Set queue set queue type for PON MAC
 * Input:
 *      qid 	- The queue ID wanted to set (0~127)
 *      type 	- queue type  0: strict 1:WFQ
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMac_queueType_set(uint32 qid, rtk_qos_queue_type_t type)
{
	int32 ret;
    uint32 wData;
    
	/* Error Checking */
    RT_PARAM_CHK((QUEUE_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

    if(STRICT_PRIORITY==type)
        wData = 0;    
    else
        wData = 1;    

    /* set to register */
    if ((ret = reg_array_field_write(RTL9601B_PON_WFQ_TYPEr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_QUEUE_TYPEf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      _rtl9601b_ponMac_queueType_get
 * Description:
 *      Set queue get queue type for PON MAC
 * Input:
 *      qid 	- The queue ID wanted to set (0~127)
 *      type 	- queue type  0: strict 1:WFQ
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMac_queueType_get(uint32 qid, rtk_qos_queue_type_t *pType)
{
	int32 ret;
    uint32 rData;
	/* Error Checking */
    RT_PARAM_CHK((pType  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

    /* set to register */
    if ((ret = reg_array_field_read(RTL9601B_PON_WFQ_TYPEr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_QUEUE_TYPEf, &rData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if(0==rData)
        *pType= STRICT_PRIORITY;    
    else
        *pType= WFQ_WRR_PRIORITY;    
    
    return RT_ERR_OK;
}






/* Function Name:
 *      _rtl9601b_ponMac_wfqWeight_set
 * Description:
 *      Set queue set WFQ queue weight for PON MAC
 * Input:
 *      qid 	- The queue ID wanted to set (0~127)
 *      weight 	- WFQ weight
  * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMac_wfqWeight_set(uint32 qid, uint32 weight)
{
	//int32 ret; /* MODIFICATION needed */
    //uint32 value; /* MODIFICATION needed */
    
	/* Error Checking */
	if(0 == qid)
        RT_PARAM_CHK((65535 <= weight), RT_ERR_INPUT);
    else
        RT_PARAM_CHK((1024 <= weight), RT_ERR_INPUT);

    RT_PARAM_CHK((0 == weight), RT_ERR_INPUT);

    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

#if 0 /* MODIFICATION needed */
    /* set to register */
    if ((ret = reg_array_field_write(RTL9601B_PON_WFQ_WEIGHTr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_WEIGHTf, &weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(0 == qid)
    {
        /* Write [15:10] to PON_QUEUE0_WEIGHT */
        value = ((weight >> 10) & 0x0000003F);
        if ((ret = reg_field_write(RTL9601B_PON_QUEUE0_WEIGHTr, RTL9601B_PON_Q0_WEIGHTf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
#endif

    return RT_ERR_OK;
}


/* Function Name:
 *      _rtl9601b_ponMac_wfqWeight_get
 * Description:
 *      Set queue set WFQ queue weight for PON MAC
 * Input:
 *      qid 	- The queue ID wanted to set (0~127)
  * Output:
 *      pWeight 	- WFQ weight
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMac_wfqWeight_get(uint32 qid, uint32 *pWeight)
{
	//int32 ret; /* MODIFICATION needed */
    //uint32 value; /* MODIFICATION needed */
 
	/* Error Checking */
    RT_PARAM_CHK((pWeight  == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_PON_QUEUE() <= qid), RT_ERR_QUEUE_ID);

#if 0 /* MODIFICATION needed */
    /* set to register */
    if ((ret = reg_array_field_read(RTL9601B_PON_WFQ_WEIGHTr,REG_ARRAY_INDEX_NONE, qid, RTL9601B_WEIGHTf, pWeight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    
    if(0==qid)
    {
        /* Read [15:10] from PON_QUEUE0_WEIGHT */
        if ((ret = reg_field_read(RTL9601B_PON_QUEUE0_WEIGHTr, RTL9601B_PON_Q0_WEIGHTf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }        
        *pWeight = *pWeight| (value<<10);
    }
#endif

    return RT_ERR_OK;
}






/* Function Name:
 *      _rtl9601b_ponMacTcontEnable_set
 * Description:
 *      Per T-CONT set schedule enable state
 * Input:
 *      tcont 		- Specify T-CONT
 *      enable 		- Specify T-CONT schedule enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMacTcontEnable_set(uint32 tcont, rtk_enable_t enable)
{
	int32 ret;

	/* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT() <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable ), RT_ERR_INPUT);

    /* set to register */
    if ((ret = reg_array_field_write(RTL9601B_PON_TCONT_ENr,REG_ARRAY_INDEX_NONE, tcont, RTL9601B_TCONT_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      _rtl9601b_ponMacTcontEnable_get
 * Description:
 *      Per T-CONT get schedule enable state
 * Input:
 *      tcont 		- Specify T-CONT
 * Output:
 *      pEnable 		- T-CONT schedule enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID
 * Note:
 *      None
 */
static int32 _rtl9601b_ponMacTcontEnable_get(uint32 tcont, rtk_enable_t *pEnable)
{
	int32 ret;

	/* Error Checking */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_GPON_TCONT() <= tcont), RT_ERR_GPON_EXCEED_MAX_TCONT);
    RT_PARAM_CHK((pEnable  == NULL), RT_ERR_NULL_POINTER);

    /* set to register */
    if ((ret = reg_array_field_read(RTL9601B_PON_TCONT_ENr,REG_ARRAY_INDEX_NONE, tcont, RTL9601B_TCONT_ENf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



static int32 _ponmac_schedulerQueue_get(uint32 physicalQid, uint32 *pSchedulerId, uint32 *pLogicalQId)
{
    int32   ret;
    uint32  qMask;
    uint32  maxSchedhlerId;
    uint32  schedhlerBase;
    uint32  i;

    *pLogicalQId = physicalQid%32;


#if(GPON_MAC_MODE)
    maxSchedhlerId = 8;
    schedhlerBase = (physicalQid/32) * 8;
#else
    maxSchedhlerId = 2;
    schedhlerBase = (physicalQid/32) * 2;
#endif


    for(i = schedhlerBase; i<schedhlerBase+maxSchedhlerId ; i++ )
    {
        if ((ret = _rtl9601b_ponMacScheQmap_get(i, &qMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        if(qMask & 1<<(*pLogicalQId))
        {
            *pSchedulerId = i;
            return RT_ERR_OK;
        }
    }
    *pSchedulerId = schedhlerBase;
    return RT_ERR_OK;
}

static int _rtl9601b_serdes_wait_complete(void)
{
    int32 ret;
    uint16 timeout = RTL9601B_SDS_ACCESS_TIMEOUT;
    uint32 value;

    while(timeout != 0)
    {
        timeout --;
        if ((ret = reg_field_read(RTL9601B_SERDES_IND_RDr, RTL9601B_BUSYf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        if(value == 0)
        {
            return RT_ERR_OK;
        }
    }
    return RT_ERR_TIMEOUT;
}

static int _rtl9601b_serdes_ind_read(struct rtl9601b_sds_acc_s sds_acc, uint16 *data)
{
	int32 ret;
    uint32 value;
    uint32 addr = (sds_acc.index << 11) | (sds_acc.page << 5) | (sds_acc.regaddr);

    RT_PARAM_CHK((data  == NULL), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_write(RTL9601B_SERDES_IND_CMDr, RTL9601B_ADRf, &addr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    value = 0;
    if ((ret = reg_field_write(RTL9601B_SERDES_IND_CMDr, RTL9601B_WRENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_field_write(RTL9601B_SERDES_IND_CMDr, RTL9601B_CMD_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = _rtl9601b_serdes_wait_complete()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(RTL9601B_SERDES_IND_RDr, RTL9601B_RD_DATf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    *data = value;

    return RT_ERR_OK;
}

static int _rtl9601b_serdes_ind_write(struct rtl9601b_sds_acc_s sds_acc, uint16 data)
{
	int32 ret;
    uint32 value;
    uint32 addr = (sds_acc.index << 11) | (sds_acc.page << 5) | (sds_acc.regaddr);

    if ((ret = reg_field_write(RTL9601B_SERDES_IND_CMDr, RTL9601B_ADRf, &addr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_field_write(RTL9601B_SERDES_IND_CMDr, RTL9601B_WRENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    value = data;
    if ((ret = reg_field_write(RTL9601B_SERDES_IND_WDr, RTL9601B_WR_DATf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    value = 1;
    if ((ret = reg_field_write(RTL9601B_SERDES_IND_CMDr, RTL9601B_CMD_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = _rtl9601b_serdes_wait_complete()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Module Name    : PON Mac                                  */
/* Sub-module Name: flow/t-cont/queue mapping */


/* Function Name:
 *      dal_rtl9601b_ponmac_init
 * Description:
 *      Configure PON MAC initial settings
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_NUM - Invalid queue number
 * Note:
 */
int32
dal_rtl9601b_ponmac_init(void)
{
    int32   ret;
    uint32 tcont,wData;
    uint32 physicalQid;
    uint32 flowId;
    uint32 wfqTbValue;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

#ifdef FPGA_DEFINED
    /*fpga do not have RTL9601B_WSDS_ANA_24*/
#else /*FPGA_DEFINED*/

//Star mark it, since register TBD
#if 0

#ifdef CONFIG_BEN_DIFFERENTIAL
    wData = 0;
    if ((ret = reg_field_write(RTL9601B_WSDS_ANA_24r, RTL9601B_REG_BEN_TTL_OUTf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    wData = 1;
    if ((ret = reg_field_write(RTL9601B_WSDS_ANA_18r, RTL9601B_REG_BEN_SEL_CMLf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#else /*CONFIG_BEN_DIFFERENTIAL*/
    wData = 1;
    if ((ret = reg_field_write(RTL9601B_WSDS_ANA_24r, RTL9601B_REG_BEN_TTL_OUTf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#endif /*CONFIG_BEN_DIFFERENTIAL*/
#endif /*FPGA_DEFINED*/
#endif
    /*init PON BW_THRESHOLD*/
    wData = 13;
    if ((ret = reg_field_write(RTL9601B_PON_PORT_CTRLr, RTL9601B_BW_THRESHOLDf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*init PON REQ_BW_THRESHOLD*/
    wData = 14;
    if ((ret = reg_field_write(RTL9601B_PON_PORT_CTRLr, RTL9601B_REQ_BW_THRESHOLDf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

#if 0 /* MODIFICATION needed */
    wData = 1;
    if ((ret = reg_field_write(RTL9601B_PON_CFGr, RTL9601B_PIR_EXCEED_DROPf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
#endif

    /*drant out all queue when pon mac init*/
    
    if(INIT_COMPLETED == ponmac_init)
    {
        rtk_ponmac_queueCfg_t   queueCfg;
        rtk_ponmac_queue_t logicalQueue;
        uint32 queueId;
        uint32 schedulerId;
 
        for(schedulerId = 0 ; schedulerId < HAL_MAX_NUM_OF_GPON_TCONT() ; schedulerId++ )
        {
            memset(&queueCfg, 0, sizeof(rtk_ponmac_queueCfg_t));
            logicalQueue.schedulerId = schedulerId;
    
            for(queueId = 0 ; queueId <= RTK_DOT1P_PRIORITY_MAX ; queueId++ )
            {
                logicalQueue.queueId = queueId;
    
                if((ret= dal_rtl9601b_ponmac_queue_get(&logicalQueue, &queueCfg)) == RT_ERR_OK)
                {
                    /*delete queue*/
                    if((ret= dal_rtl9601b_ponmac_queue_del(&logicalQueue)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
                        return ret;                    
                    }                
                }
            }
        }
    }


    for(tcont = 0; tcont <HAL_MAX_NUM_OF_GPON_TCONT(); tcont++ )
    {
        /*disable T-cont schedule */
        if ((ret = _rtl9601b_ponMacTcontEnable_set(tcont, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        /*clear T-cont schedule mask*/
        if ((ret = _rtl9601b_ponMacScheQmap_set(tcont, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }



    /*set all queue to strict, disable CIR/PIR and disable egress drop*/

    for(physicalQid=0 ; physicalQid<HAL_MAX_NUM_OF_PON_QUEUE() ; physicalQid++)
    {
        /*set queue schedule type*/
        if ((ret = _rtl9601b_ponMac_queueType_set(physicalQid, STRICT_PRIORITY)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /*set PIR CIR*/
        if ((ret = _rtl9601b_ponMacCirRate_set(physicalQid, 0x0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        if ((ret = _rtl9601b_ponMacPirRate_set(physicalQid, HAL_PONMAC_PIR_CIR_RATE_MAX())) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }

        /*set default weight to 1*/
        if ((ret = _rtl9601b_ponMac_wfqWeight_set(physicalQid, 0x1)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
#if defined(CONFIG_GPON_FEATURE)
    /* do not add queue 0~7 to scheduler id 0*/
#else
    /*mapping queue 0~7 to schedule id 0*/
    {
        rtk_ponmac_queueCfg_t   queueCfg;
        rtk_ponmac_queue_t logicalQueue;
        uint32 queueId;

        memset(&queueCfg, 0, sizeof(rtk_ponmac_queueCfg_t));

        logicalQueue.schedulerId = 0 ;

        queueCfg.cir       = 0x0;
        queueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX();
        queueCfg.type      = STRICT_PRIORITY;
        queueCfg.egrssDrop = DISABLED;
        ponmac_init = INIT_COMPLETED;
        for(queueId = 0 ; queueId <= RTK_DOT1P_PRIORITY_MAX ; queueId++ )
        {
            logicalQueue.queueId = queueId;

            if((ret= dal_rtl9601b_ponmac_queue_add(&logicalQueue, &queueCfg)) != RT_ERR_OK)
            {
                ponmac_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }
#endif
    /*mapping all flow to t-cont 0 queue 0*/
    {
        rtk_ponmac_queue_t logicalQueue;
#if defined(CONFIG_GPON_FEATURE)
        /* mapping all flow to t-cont 31 queue 31 */
        logicalQueue.schedulerId = 31;
        logicalQueue.queueId     = 31;
#else
        logicalQueue.schedulerId = 0;
        logicalQueue.queueId     = 0;
#endif
        ponmac_init = INIT_COMPLETED;
        for(flowId=0 ; flowId<33 ; flowId++)
        {
            if((ret= dal_rtl9601b_ponmac_flow2Queue_set(flowId, &logicalQueue)) != RT_ERR_OK)
            {
                ponmac_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }

    /*configure WFQ specific tb value on RSVD_EGR_SCH to 0xff*/
   wfqTbValue = 0xff;
   if((ret = reg_field_write(RTL9601B_PON_WFQ_LEAKY_BUCKETr, RTL9601B_PON_WFQ_LEAKY_BUCKETf, &wfqTbValue))!=RT_ERR_OK)
   {
   	   ponmac_init = INIT_NOT_COMPLETED;
       return ret;
   }

    ponmac_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_rtl9601b_ponmac_init */


/* Function Name:
 *      dal_rtl9601b_ponmac_queue_add
 * Description:
 *      Add queue to given scheduler id and apply queue setting
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 *      pQueueCfg     - queue configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueueList/pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9601b_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;
    uint32  qMask;
    uint32  physicalQid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= RTL9601B_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    RT_PARAM_CHK((pQueueCfg->type >= QUEUE_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueueCfg->egrssDrop >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* add queue to t-cont schedule mask*/
    if ((ret = _rtl9601b_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(qMask == 0)
    {/*first queue add to this t-cont enable t-cont schedule*/
        if ((ret = _rtl9601b_ponMacTcontEnable_set(pQueue->schedulerId, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }

    /*add queue to qMask*/
    qMask = qMask | (1<<pQueue->queueId);
    if ((ret = _rtl9601b_ponMacScheQmap_set(pQueue->schedulerId, qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*set Queue id and flow id mapping */
    _ponmac_physicalQueueId_get(pQueue->schedulerId, pQueue->queueId, &physicalQid);

    /*set queue schedule type*/
    if(pQueueCfg->type == STRICT_PRIORITY)
    {
        if ((ret = _rtl9601b_ponMac_queueType_set(physicalQid, STRICT_PRIORITY)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = _rtl9601b_ponMac_queueType_set(physicalQid, WFQ_WRR_PRIORITY)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        /*set wfq weight*/
        if ((ret = _rtl9601b_ponMac_wfqWeight_set(physicalQid, pQueueCfg->weight)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }


    /*set PIR CIR*/
    if ((ret = _rtl9601b_ponMacCirRate_set(physicalQid, pQueueCfg->cir)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = _rtl9601b_ponMacPirRate_set(physicalQid, pQueueCfg->pir)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }



    return RT_ERR_OK;
} /* end of dal_rtl9601b_ponmac_queue_add */



/* Function Name:
 *      dal_rtl9601b_ponmac_queue_get
 * Description:
 *      get queue setting
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      pQueueCfg     - queue configuration
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueueList/pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9601b_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;
    rtk_enable_t   enable;
    uint32  qMask;
    uint32  physicalQid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= RTL9601B_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    if ((ret = _rtl9601b_ponMacTcontEnable_get(pQueue->schedulerId, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(DISABLED==enable)
        return RT_ERR_ENTRY_NOTFOUND;

    if ((ret = _rtl9601b_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(!(qMask & 1<<(pQueue->queueId)))
        return RT_ERR_ENTRY_NOTFOUND;

    _ponmac_physicalQueueId_get(pQueue->schedulerId, pQueue->queueId, &physicalQid);

    if ((ret = _rtl9601b_ponMac_queueType_get(physicalQid, &(pQueueCfg->type))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*set wfq weight*/
    if ((ret = _rtl9601b_ponMac_wfqWeight_get(physicalQid, &(pQueueCfg->weight))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    /*set PIR CIR*/
    if ((ret = _rtl9601b_ponMacCirRate_get(physicalQid, &(pQueueCfg->cir))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = _rtl9601b_ponMacPirRate_get(physicalQid, &(pQueueCfg->pir))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }


    /*get egress drop*/
    pQueueCfg->egrssDrop = DISABLED;


    return RT_ERR_OK;
} /* end of dal_rtl9601b_ponmac_queue_get */


/* Function Name:
 *      dal_rtl9601b_ponmac_queue_del
 * Description:
 *      delete queue from given scheduler id
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9601b_ponmac_queue_del(rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    uint32  qMask;
    rtk_enable_t   enable;
    uint32  queueId;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= RTL9601B_TCONT_QUEUE_MAX), RT_ERR_INPUT);


    if ((ret = _rtl9601b_ponMacTcontEnable_get(pQueue->schedulerId, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(DISABLED==enable)
        return RT_ERR_ENTRY_NOTFOUND;

    if ((ret = _rtl9601b_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if(!(qMask & 1<<(pQueue->queueId)))
        return RT_ERR_ENTRY_NOTFOUND;


    /*remove queue from qMask*/
    qMask = qMask & (~(1<<pQueue->queueId));
    if ((ret = _rtl9601b_ponMacScheQmap_set(pQueue->schedulerId, qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

        /*drant out queue*/
    queueId = ((pQueue->schedulerId)/8) * 32 + pQueue->queueId;
    if ((ret = _rtl9601b_ponMacQueueDrainOutState_set(queueId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    
    if(0 == qMask)
    {/*no queue map to this t-cont disable schedule for this t-cont*/
        if ((ret = _rtl9601b_ponMacTcontEnable_set(pQueue->schedulerId, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_ponmac_queue_del */



/* Function Name:
 *      dal_rtl9601b_ponmac_flow2Queue_set
 * Description:
 *      mapping flow to given queue
 * Input:
 *      flow          - flow id.
 *      pQueue        - queue id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9601b_ponmac_flow2Queue_set(uint32  flow, rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    uint32  physicalQid;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() <= flow), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= RTL9601B_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    _ponmac_physicalQueueId_get(pQueue->schedulerId, pQueue->queueId, &physicalQid);

    if ((ret = _rtl9601b_ponMacSidToQueueMap_set(flow, physicalQid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_ponmac_flow2Queue_set */


/* Function Name:
 *      dal_rtl9601b_ponmac_flow2Queue_get
 * Description:
 *      get queue id for this flow
 * Input:
 *      flow         - flow id.
 * Output:
 *      pQueue       - queue id.
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9601b_ponmac_flow2Queue_get(uint32  flow, rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    uint32  physicalQid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() <= flow), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);

    if ((ret = _rtl9601b_ponMacSidToQueueMap_get(flow, &physicalQid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = _ponmac_schedulerQueue_get(physicalQid, &(pQueue->schedulerId), &(pQueue->queueId))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_ponmac_flow2Queue_get */



/* Function Name:
 *      dal_rtl9601b_ponmac_mode_set
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 *	  state 	     - enable or disable
 * Output:
 *      pQueue       - queue id.
 * Return:
 *      RT_ERR_OK              						- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9601b_ponmac_mode_set(rtk_ponmac_mode_t mode)
{
	int32 ret;
	uint32 data;
 	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    	/* check Init status */
   	 RT_INIT_CHK(ponmac_init);

	 RT_PARAM_CHK((PONMAC_MODE_END <= mode), RT_ERR_INPUT);

	if(mode == PONMAC_MODE_GPON)
	{
	    /* set the GPON mode enable */
	    data = 1;
	    if ((ret = reg_write(RTL9601B_PON_MODE_CFGr, &data)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	        return ret;
	    }
	    //Star mark it, since register TBD
#if 0
	    /* set the serdes mode to GPON mode */
	    data = 0x8;
	    if ((ret = reg_field_write(RTL9601B_SDS_CFGr, RTL9601B_CFG_SDS_MODEf, &data)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	        return ret;
	    }

	    data = 0x0;
	    /* PON MAC select phy 4 enable/disable */
	    if ((ret = reg_field_write(RTL9601B_CHIP_INF_SELr, RTL9601B_PHY4_ENf, &data)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_FLOWCTRL | MOD_DAL), "");
	        return ret;
	    }
#endif
#if 1
	    /* reset serdes including digital and analog */
	    /* no force sds */
	    data = 0x0;
	    if ((ret = reg_write(RTL9601B_WSDS_DIG_01r, &data)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	        return ret;
	    }

	    /* reset serdes including digital and analog, and GPON MAC */
	    data = 0x1;
	    if ((ret = reg_field_write(RTL9601B_SOFTWARE_RSTr, RTL9601B_CMD_SDS_RST_PSf, &data)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	        return ret;
	    }
	    osal_time_mdelay(10);

	    /* force sds for not reset GPON MAC when SD down */
	    data = 0xc;
	    if ((ret = reg_write(RTL9601B_WSDS_DIG_01r, &data)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	        return ret;
	    }
#endif
//Star mark it, since register TBD
#if 0
	    data = 0x0;
	    if ((ret = reg_write(RTL9601B_WSDS_ANA_1Cr, &data)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	        return ret;
	    }

	    /* Serdes amplitude 500 mv */
	    data = 0x1164;
	    if ((ret = reg_write(RTL9601B_WSDS_ANA_0Er, &data)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	        return ret;
	    }
#endif
	    /* BEN on */
	    data = 0x1;
	    if ((ret = reg_field_write(RTL9601B_WSDS_DIG_18r, RTL9601B_BEN_OEf, &data)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	        return ret;
	    }
	    /* pon port allow undersize packet */
	    data = 1;
	    if ((ret = reg_array_field_write(RTL9601B_P_MISCr, HAL_GET_PON_PORT(), REG_ARRAY_INDEX_NONE, RTL9601B_RX_SPCf, &data)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	        return ret;
	    }
	 	 /* turn off 'serdes_tx_dis when ben=0' */
	 	data = 0;
	 	if ((ret = reg_field_write(RTL9601B_WSDS_DIG_06r, RTL9601B_CFG_DMY1f, &data)) != RT_ERR_OK)
	 	{
	 	    RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	 	    return ret;
	 	}
	}
	
	ponMode = mode;
	return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9601b_ponmac_mode_get
 * Description:
 *      set pon mac mode
 * Input:
 *      mode         - pon mode, epon or gpon
 * Output:
 *      *pState       - mode state
 * Return:
 *      RT_ERR_OK              						- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_rtl9601b_ponmac_mode_get(rtk_ponmac_mode_t *pMode)
{
 	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
   	RT_INIT_CHK(ponmac_init);

    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    *pMode=ponMode;

	return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9601b_ponmac_schedulingType_get
 * Description:
 *      Get the scheduling types and weights of queues on specific port in egress scheduling.
 * Input:
 *      None
 * Output:
 *      pQueueType  - the WFQ schedule type (DAL_RTL9601B_QOS_WFQ or DAL_RTL9601B_QOS_WRR)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
dal_rtl9601b_ponmac_schedulingType_get(rtk_qos_scheduling_type_t *pQueueType)
{
	int32  ret;
	uint32 val;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueueType), RT_ERR_NULL_POINTER);

    /* function body */
	if ((ret = reg_field_read(RTL9601B_PON_CFGr,RTL9601B_PON_WFQ_MODEf, &val)) != RT_ERR_OK)
	{
	    RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	    return ret;
	}
	
	if(0 == val)
        *pQueueType=RTK_QOS_WFQ;
	else
        *pQueueType=RTK_QOS_WRR;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_schedulingType_get */

/* Function Name:
 *      dal_rtl9601b_pon_schedulingType_set
 * Description:
 *      Set the scheduling type.
 * Input:
 *      queueType - the WFQ schedule type (DAL_RTL9601B_QOS_WFQ or DAL_RTL9601B_QOS_WRR)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_rtl9601b_ponmac_schedulingType_set(rtk_qos_scheduling_type_t queueType)
{
	int32  ret;
	uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "queueType=%d",queueType);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((SCHEDULING_TYPE_END <=queueType), RT_ERR_INPUT);

    /* function body */
	if(RTK_QOS_WFQ == queueType)
	    val = 0;
	else    
	    val = 1;

	if ((ret = reg_field_write(RTL9601B_PON_CFGr,RTL9601B_PON_WFQ_MODEf, &val)) != RT_ERR_OK)
	{
	    RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
	    return ret;
	}

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_schedulingType_set */


/* Function Name:
 *      dal_rtl9601b_ponmac_queueDrainOut_set
 * Description:
 *      Set pon queue drain out.
 * Input:
 *      pQueue - Specified which PON queue will be drain out
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_rtl9601b_ponmac_queueDrainOut_set(rtk_ponmac_queue_t *pQueue)
{
    uint32 queueId;
	int32  ret;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= RTL9601B_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    /* function body */

    queueId = ((pQueue->schedulerId)/8) * 32 + pQueue->queueId;
    if ((ret = _rtl9601b_ponMacQueueDrainOutState_set(queueId)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_queueDrainOut_set */

/* Function Name:
 *      dal_rtl9601b_ponmac_opticalPolarity_get
 * Description:
 *      Get the current optical output polarity
 * Input:
 *      None
 * Output:
 *      pPolarity  - the current output polarity
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
dal_rtl9601b_ponmac_opticalPolarity_get(rtk_ponmac_polarity_t *pPolarity)
{
	int32  ret;
	uint32 val;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPolarity), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_field_read(RTL9601B_WSDS_DIG_18r,RTL9601B_CFG_FRC_BEN_INVf,&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    

    if(0 == val)
    {
        *pPolarity = PONMAC_POLARITY_HIGH;
    }
    else
    {
        *pPolarity = PONMAC_POLARITY_LOW;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_opticalPolarity_get */

/* Function Name:
 *      dal_rtl9601b_ponmac_opticalPolarity_set
 * Description:
 *      Set optical output polarity
 * Input:
 *      polarity - the optical output polarity
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_rtl9601b_ponmac_opticalPolarity_set(rtk_ponmac_polarity_t polarity)
{
	int32  ret;
	uint32 val;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((PONMAC_POLARITY_END <= polarity), RT_ERR_INPUT);

    /* function body */
    if(PONMAC_POLARITY_HIGH == polarity)
    {
        val = 0;
    }
    else
    {
        val = 1;
    }

    if ((ret = reg_field_write(RTL9601B_WSDS_DIG_18r,RTL9601B_CFG_FRC_BEN_INVf,&val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }    

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_opticalPolarity_set */

/* Function Name:
 *      dal_rtl9601b_ponmac_losState_get
 * Description:
 *      Get the current optical lost of signal (LOS) state
 * Input:
 *      None
 * Output:
 *      pEnable  - the current optical lost of signal state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
dal_rtl9601b_ponmac_losState_get(rtk_enable_t *pState)
{
	uint16  tmpVal;   
 	int32   ret;
    struct rtl9601b_sds_acc_s sds_acc;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    sds_acc.index = RTL9601B_SDS_IDX_PON;
    sds_acc.page = RTL9601B_SDS_PAGE_SDS_EXT;
    sds_acc.regaddr = 29;

    if ((ret = _rtl9601b_serdes_ind_read(sds_acc, &tmpVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
        return ret;
    }
    
    if(1==(tmpVal & (1<<8)))
        *pState = DISABLED;
    else
        *pState = ENABLED;
    
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_ponmac_losState_get */

