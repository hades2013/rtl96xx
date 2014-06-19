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
 * $Revision: 41127 $
 * $Date: 2013-07-12 15:21:35 +0800 (Fri, 12 Jul 2013) $
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
 *
 */


/*
 * Include Files
 */
#include <rtk/switch.h>

#include <dal/rtl9601b/dal_rtl9601b.h>
#include <dal/rtl9601b/dal_rtl9601b_switch.h>
#include <hal/common/miim.h>
#include <osal/time.h>

/*
 * Symbol Definition
 */
#define DAL_RTL9601B_BROADCAST_PHYID  (0x0010)


typedef struct dal_rtl9601b_phy_data_s
{
    uint16  phy;
    uint16  page;
    uint16  reg;
    uint16  data;
} dal_rtl9601b_phy_data_t;


/*
 * Data Declaration
 */
static uint32    switch_init = INIT_NOT_COMPLETED;

/*
 * Function Declaration
 */

int32 dal_rtl9601b_switch_phyPatch(void);

int32 _rtl9601b_switch_macAddr_set(rtk_mac_t *pMacAddr);
int32 _rtl9601b_switch_macAddr_get(rtk_mac_t *pMacAddr);

static int32
_dal_rtl9601b_switch_ponAsicPhyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    switch(portName)
    {
        case RTK_PORT_UTP0:
            *pPortId = 0;
            break;
#if defined(CONFIG_GPON_FEATURE) || defined(CONFIG_EPON_FEATURE)
        case RTK_PORT_PON:
            *pPortId = 1;
            break;
#else
        case RTK_PORT_UTP1:
            *pPortId = 1;
            break;
#endif
        case RTK_PORT_CPU:
            *pPortId = 2;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}




static int32
_dal_rtl9601b_switch_ponAsicLogicalPortName_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    switch(portId)
    {
        case 0:
            *pPortName = RTK_PORT_UTP0;
            break;
        case 1:
#if defined(CONFIG_GPON_FEATURE) || defined(CONFIG_EPON_FEATURE)
            *pPortName = RTK_PORT_PON;
#else
            *pPortName = RTK_PORT_UTP1;
#endif
            break;
        case 2:
            *pPortName = RTK_PORT_CPU;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}


static int32
_dal_rtl9601b_switch_tickToken_init(void)
{
    int ret;
    uint32 wData;

    /*meter pon-tick-token configuration*/
    wData = 53;
    if ((ret = reg_field_write(RTL9601B_PON_TB_CTRLr, RTL9601B_TICK_PERIODf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    wData = 58;
    if ((ret = reg_field_write(RTL9601B_PON_TB_CTRLr, RTL9601B_TKNf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    /*meter switch-tick-token configuration*/
    wData = 53;
    if ((ret = reg_field_write(RTL9601B_METER_TB_CTRLr, RTL9601B_TICK_PERIODf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    wData = 58;
    if ((ret = reg_field_write(RTL9601B_METER_TB_CTRLr, RTL9601B_TKNf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



static int32
_dal_rtl9601b_switch_green_enable(void)
{
    return RT_ERR_OK;
}

static int32
_dal_rtl9601b_switch_eee_enable(void)
{
    return RT_ERR_OK;
}

static int32
_dal_rtl9601b_switch_powerSaving_init(void)
{
    int ret;

    /* === Green enable === */
    if ((ret = _dal_rtl9601b_switch_green_enable()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /* === EEE enable === */
    if ((ret = _dal_rtl9601b_switch_eee_enable()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}



int32
dal_rtl9601b_switch_phyPatch(void)
{
    return RT_ERR_OK;
}


/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      dal_rtl9601b_switch_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
int32
dal_rtl9601b_switch_init(void)
{
    int32   ret;
    uint32 wData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    switch_init = INIT_COMPLETED;

    /*flow control threshold and set buffer mode*/
#if defined(CONFIG_GPON_FEATURE) || defined(CONFIG_EPON_FEATURE)
    /*set packet buffer size*/
#else
    /*set packet buffer size*/
#endif


    /*meter tick-token configuration*/
    if((ret = _dal_rtl9601b_switch_tickToken_init())!=RT_ERR_OK)
    {
 	    RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

#if defined(FPGA_DEFINED)

#else
    /*CFG_PHY_POLL_CMD set to 0x1100*/
    wData = 0x1100;
    if ((ret = reg_write(RTL9601B_CFG_PHY_POLL_CMDr,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#if defined(RTL_CYGWIN_EMULATE)

#else
    /*analog patch*/
    /*phy patch*/
    if((ret=dal_rtl9601b_switch_phyPatch())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "phy patch fail\n");
        return ret;
    }

    /* power saving feature */
    if((ret=_dal_rtl9601b_switch_powerSaving_init())!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif
    /*set switch ready, phy patch done*/
    wData = 1;
    if ((ret = reg_field_write(RTL9601B_WRAP_GPHY_MISCr,RTL9601B_PATCH_PHY_DONEf,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    /*CFG_PHY_POLL_CMD set to 0x1110*/
    wData = 0x1110;
    if ((ret = reg_write(RTL9601B_CFG_PHY_POLL_CMDr,&wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
#endif


    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_init */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */


/* Function Name:
 *      dal_rtl9601b_switch_phyPortId_get
 * Description:
 *      Get physical port id from logical port name
 * Input:
 *      portName - logical port name
 * Output:
 *      pPortId  - pointer to the physical port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port ID must get from this API
 */
int32
dal_rtl9601b_switch_phyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    dal_rtl9601b_switch_asic_type_t asicType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPortId), RT_ERR_NULL_POINTER);

    /*get chip info to check port name mapping*/
    asicType = RTL9601B_ASIC_PON;

    switch(asicType)
    {
        case RTL9601B_ASIC_PON:
        default:
            return _dal_rtl9601b_switch_ponAsicPhyPortId_get(portName,pPortId);
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_phyPortId_get */



/* Function Name:
 *      dal_rtl9601b_switch_logicalPort_get
 * Description:
 *      Get logical port name from physical port id
 * Input:
 *      portId  - physical port id
 * Output:
 *      pPortName - pointer to logical port name
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rtl9601b_switch_logicalPort_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    dal_rtl9601b_switch_asic_type_t asicType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortName), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(portId), RT_ERR_PORT_ID);

    /*get chip info to check port name mapping*/
    asicType = RTL9601B_ASIC_PON;

    switch(asicType)
    {
        case RTL9601B_ASIC_PON:
        default:
            return _dal_rtl9601b_switch_ponAsicLogicalPortName_get(portId,pPortName);
            break;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_logicalPort_get */



/* Function Name:
 *      dal_rtl9601b_switch_port2PortMask_set
 * Description:
 *      Set port id to the portlist
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port mask must set by this API
 */
int32
dal_rtl9601b_switch_port2PortMask_set(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32 portId;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9601b_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    RTK_PORTMASK_PORT_SET(*pPortMask,portId);

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_port2PortMask_set */



/* Function Name:
 *      dal_rtl9601b_switch_port2PortMask_clear
 * Description:
 *      Set port id to the portlist
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Call RTK API the port mask must set by this API
 */
int32
dal_rtl9601b_switch_port2PortMask_clear(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;
    int32 portId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9601b_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    RTK_PORTMASK_PORT_CLEAR(*pPortMask, portId);

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_port2PortMask_clear */



/* Function Name:
 *      dal_rtl9601b_switch_portIdInMask_check
 * Description:
 *      Check if given port is in port list
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rtl9601b_switch_portIdInMask_check(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;
    int32 portId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_rtl9601b_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    if(RTK_PORTMASK_IS_PORT_SET(*pPortMask,portId))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;

} /* end of dal_rtl9601b_switch_portIdInMask_check */


/* Function Name:
  *      dal_rtl9601b_switch_maxPktLenLinkSpeed_get
  * Description:
  *      Get the max packet length setting of the specific speed type
  * Input:
  *      speed - speed type
  * Output:
  *      pLen  - pointer to the max packet length
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  *      RT_ERR_INPUT        - invalid enum speed type
  * Note:
  *      Max packet length setting speed type
  *      - MAXPKTLEN_LINK_SPEED_FE
  *      - MAXPKTLEN_LINK_SPEED_GE
  */
int32
dal_rtl9601b_switch_maxPktLenLinkSpeed_get(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen)
{
    int32   ret;
	uint32 regAddr;
    uint32 fieldIdx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((MAXPKTLEN_LINK_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);

#if 0 /* MODIFICATION needed */
    if(speed == MAXPKTLEN_LINK_SPEED_FE)
    {
	    regAddr =  RTL9601B_MAX_LENGTH_CFG0r;
        fieldIdx = RTL9601B_ACCEPT_MAX_LENGTH_CFG0f;
    }
    else
    {
	    regAddr =  RTL9601B_MAX_LENGTH_CFG1r;
        fieldIdx = RTL9601B_ACCEPT_MAX_LENGTH_CFG1f;
    }
#endif

    if ((ret = reg_field_read(regAddr, fieldIdx, pLen)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_maxPktLenLinkSpeed_get */

/* Function Name:
  *      dal_rtl9601b_switch_maxPktLenLinkSpeed_set
  * Description:
  *      Set the max packet length of the specific speed type
  * Input:
  *      speed - speed type
  *      len   - max packet length
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  *      Max packet length setting speed type
  *      - MAXPKTLEN_LINK_SPEED_FE
  *      - MAXPKTLEN_LINK_SPEED_GE
  */
int32
dal_rtl9601b_switch_maxPktLenLinkSpeed_set(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len)
{
    rtk_port_t  port, max_port;
    int32   ret;
	uint32 regAddr;
    uint32 fieldIdx;
    uint32 portField;
    uint32 index;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((MAXPKTLEN_LINK_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9601B_PACEKT_LENGTH_MAX < len), RT_ERR_INPUT);

#if 0 /* MODIFICATION needed */
    if(speed == MAXPKTLEN_LINK_SPEED_FE)
    {
	    regAddr =  RTL9601B_MAX_LENGTH_CFG0r;
        fieldIdx = RTL9601B_ACCEPT_MAX_LENGTH_CFG0f;
        portField = RTL9601B_MAX_LENGTH_10_100f;
        index = 0;
    }
    else
    {
	    regAddr   = RTL9601B_MAX_LENGTH_CFG1r;
        fieldIdx  = RTL9601B_ACCEPT_MAX_LENGTH_CFG1f;
        portField = RTL9601B_MAX_LENGTH_GIGAf;
        index = 1;
    }
#endif 

    if ((ret = reg_field_write(regAddr, fieldIdx, &len)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    /*set all port index to currect length index*/
    max_port = HAL_GET_MAX_PORT();
    for (port = 0; port <= max_port; port++)
    {
        if (!HAL_IS_PORT_EXIST(port))
        {
            continue;
        }

        if ((ret = reg_array_field_write(RTL9601B_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, portField, &index)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }
    }
    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_maxPktLenLinkSpeed_set */


/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */
/* Function Name:
 *      _rtl9601b_switch_macAddr_set
 * Description:
 *      Set switch mac address configurations
 * Input:
 *      pMacAddr - Switch mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 _rtl9601b_switch_macAddr_set(rtk_mac_t *pMacAddr)
{
    int32 ret;
    uint32 i, tmp[6];
	
    if(pMacAddr == NULL)
        return RT_ERR_NULL_POINTER;

    for (i=0;i<ETHER_ADDR_LEN;i++)
        tmp[i] = pMacAddr->octet[i];


    if ((ret = reg_field_write(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC5f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC4f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC3f, &tmp[2])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_write(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC2f, &tmp[3])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }	
     if ((ret = reg_field_write(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC1f, &tmp[4])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_write(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC0f, &tmp[5])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
	 
    return RT_ERR_OK;
}/*end of _rtl9601b_switch_macAddr_set*/

/* Function Name:
 *      _rtl9601b_switch_macAddr_get
 * Description:
 *      Get switch mac address configurations
 * Input:
 *      pMacAddr - Switch mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 					- Success
 *      RT_ERR_SMI  				- SMI access error
 *      RT_ERR_INPUT  				- Invalid input parameter
 * Note:
 *      None
 */
int32 _rtl9601b_switch_macAddr_get(rtk_mac_t *pMacAddr)
{
    int32 ret;
    uint32 i, tmp[6];

    if ((ret = reg_field_read(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC5f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC4f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC3f, &tmp[2])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_field_read(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC2f, &tmp[3])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }	
     if ((ret = reg_field_read(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC1f, &tmp[4])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_field_read(RTL9601B_SWITCH_MACr, RTL9601B_SWITCH_MAC0f, &tmp[5])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    for (i=0;i<ETHER_ADDR_LEN;i++)
        pMacAddr->octet[i] = tmp[i];
	 
    return RT_ERR_OK;
}/*end of _rtl9601b_raw_switch_macAddr_get*/



/* Function Name:
 *      dal_rtl9601b_switch_mgmtMacAddr_get
 * Description:
 *      Get MAC address of switch.
 * Input:
 *      None
 * Output:
 *      pMac - pointer to MAC address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9601b_switch_mgmtMacAddr_get(rtk_mac_t *pMac)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if ((ret = _rtl9601b_switch_macAddr_get(pMac)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_mgmtMacAddr_get */

/* Function Name:
 *      dal_rtl9601b_switch_mgmtMacAddr_set
 * Description:
 *      Set MAC address of switch.
 * Input:
 *      pMac - MAC address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9601b_switch_mgmtMacAddr_set(rtk_mac_t *pMac)
{

    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if((pMac->octet[0] & BITMASK_1B) == 1)
        return RT_ERR_INPUT;

    if ((ret = _rtl9601b_switch_macAddr_set(pMac)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9601b_switch_mgmtMacAddr_set */


/* Function Name:
 *      dal_rtl9601b_switch_chip_reset
 * Description:
 *      Reset switch chip
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9601b_switch_chip_reset(void){

    int32   ret;
    uint32 resetValue = 1;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    if ((ret = reg_field_write(RTL9601B_SOFTWARE_RSTr,RTL9601B_CMD_CHIP_RST_PSf,&resetValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;

}
