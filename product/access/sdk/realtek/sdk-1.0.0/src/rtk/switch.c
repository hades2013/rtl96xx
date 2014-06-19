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
 * $Revision: 41502 $
 * $Date: 2013-07-25 11:04:59 +0800 (Thu, 25 Jul 2013) $
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
#include <common/rt_type.h>
#include <hal/chipdef/chip.h>
#include <rtk/switch.h>
#include <rtk/init.h> 
#include <rtk/default.h> 
#include <dal/dal_mgmt.h> 
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
/*
 * Symbol Definition
 */



/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      rtk_switch_init
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
rtk_switch_init(void)
{
    int32   ret;

    if (NULL == RT_MAPPER->switch_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->switch_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_switch_init */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      rtk_switch_deviceInfo_get
 * Description:
 *      Get device information of the specific unit
 * Input:
 *      none
 * Output:
 *      pDevInfo - pointer to the device information
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer 
 * Note:
 *      None
 */
int32
rtk_switch_deviceInfo_get(rtk_switch_devInfo_t *pDevInfo)
{
    hal_control_t *pHal_control;
    
    RT_PARAM_CHK((NULL == pDevInfo), RT_ERR_NULL_POINTER);

    RTK_API_LOCK();
    if ((pHal_control = hal_ctrlInfo_get()) == NULL)
    {
        return RT_ERR_FAILED;
    }
    RTK_API_UNLOCK();
    
    pDevInfo->chipId    = pHal_control->chip_id;
    pDevInfo->revision  = pHal_control->chip_rev_id;
    pDevInfo->port_number = pHal_control->pDev_info->pPortinfo->port_number;
    pDevInfo->fe        = pHal_control->pDev_info->pPortinfo->fe;
    pDevInfo->ge        = pHal_control->pDev_info->pPortinfo->ge;
    pDevInfo->ge_combo  = pHal_control->pDev_info->pPortinfo->ge_combo;
    pDevInfo->serdes    = pHal_control->pDev_info->pPortinfo->serdes;
    pDevInfo->ether     = pHal_control->pDev_info->pPortinfo->ether;
    pDevInfo->all       = pHal_control->pDev_info->pPortinfo->all;
    pDevInfo->cpuPort   = pHal_control->pDev_info->pPortinfo->cpuPort;
    pDevInfo->dsl        = pHal_control->pDev_info->pPortinfo->dsl;
    pDevInfo->ext        = pHal_control->pDev_info->pPortinfo->ext;
    memcpy(&pDevInfo->capacityInfo, pHal_control->pDev_info->pCapacityInfo, sizeof(rt_register_capacity_t));

    return RT_ERR_OK;
} /* end of rtk_switch_deviceInfo_get */


/* Function Name:
 *      rtk_switch_phyPortId_get
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
rtk_switch_phyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    int32   ret;

    if (NULL == RT_MAPPER->switch_phyPortId_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->switch_phyPortId_get( portName, pPortId);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_switch_phyPortId_get */




/* Function Name:
 *      rtk_switch_logicalPort_get
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
rtk_switch_logicalPort_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    int32   ret;

    if (NULL == RT_MAPPER->switch_logicalPort_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->switch_logicalPort_get( portId, pPortName);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_switch_logicalPort_get */

/* Function Name:
 *      rtk_switch_port2PortMask_set
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
rtk_switch_port2PortMask_set(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;

    if (NULL == RT_MAPPER->switch_port2PortMask_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->switch_port2PortMask_set( pPortMask, portName);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_switch_port2PortMask_set */



/* Function Name:
 *      rtk_switch_port2PortMask_set
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
rtk_switch_port2PortMask_clear(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;

    if (NULL == RT_MAPPER->switch_port2PortMask_clear)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->switch_port2PortMask_clear( pPortMask, portName);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_switch_port2PortMask_clear */



/* Function Name:
 *      rtk_switch_portIdInMask_check
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
rtk_switch_portIdInMask_check(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;

    if (NULL == RT_MAPPER->switch_portIdInMask_check)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->switch_portIdInMask_check( pPortMask, portName);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_switch_portIdInMask_check */





/* Function Name:
 *      rtk_switch_portMask_Clear
 * Description:
 *      Clear all port mask
 * Input:
 *      pPortMask    - port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer 
 * Note:
 */
int32
rtk_switch_portMask_Clear(rtk_portmask_t *pPortMask)
{
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    RTK_PORTMASK_RESET(*pPortMask);
    return RT_ERR_OK;     
} /*rtk_switch_portMask_Clear*/   


/* Function Name:
 *      rtk_switch_allPortMask_set
 * Description:
 *      Set all switch port to mask
 * Input:
 *      pPortMask    - port mask
 * Output:
 *      pPortMask    - port mask
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer 
 * Note:
 */
int32
rtk_switch_allPortMask_set(rtk_portmask_t *pPortMask)
{
	rtk_port_t    port;

    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    RTK_PORTMASK_RESET(*pPortMask);
    HAL_SCAN_ALL_PORT(port)
    { 
        RTK_PORTMASK_PORT_SET((*pPortMask), port);
    }

    return RT_ERR_OK;    
} /*rtk_switch_allPortMask_set*/    



/* Function Name:
 *      rtk_switch_allExtPortMask_set
 * Description:
 *      Set all extention port to mask
 * Input:
 *      pPortMask    - port mask
 * Output:
 *      pPortMask    - port mask
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer 
 * Note:
 */
int32
rtk_switch_allExtPortMask_set(rtk_portmask_t *pPortMask)
{
	rtk_port_t    port;

    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    RTK_PORTMASK_RESET(*pPortMask);
    HAL_SCAN_ALL_EXT_PORT(port)
    { 
        RTK_PORTMASK_PORT_SET((*pPortMask), port);
    }

    return RT_ERR_OK;     
    
}/*rtk_switch_allExtPortMask_set*/     



/* Function Name:
 *      rtk_switch_nextPortInMask_get
 * Description:
 *      Get next port id in the port mask
 * Input:
 *      pPortMask    - port mask
 *      pPortId      - given port id to get the next port in the port mask
 * Output:
 *      pPortId      - the next port id 
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer 
 * Note:
 *      -RT_ERR_OK means the pPortId record the next port, 
 *           others do not have any port for given port id next port
 *      -Set *pPortId to RTK_SWITCH_FIRST_PORT to get the first port 
 */
int32
rtk_switch_nextPortInMask_get(rtk_portmask_t *pPortMask, int32 *pPortId)
{
    rtk_portmask_t tempPortlist;
    int32 port;
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);

    
    RTK_PORTMASK_ASSIGN(tempPortlist,*pPortMask);
    
    
    do{
        if((port = RTK_PORTMASK_GET_FIRST_PORT(tempPortlist))== -1)
            return RT_ERR_FAILED;    
        if(port > *pPortId)
        {
            *pPortId = port;
            break;    
        }
        /*remove port from port mask*/
        RTK_PORTMASK_PORT_CLEAR(tempPortlist,port)    
    }while(1);

    return RT_ERR_OK;    
}



/* Function Name:
  *      rtk_switch_maxPktLenLinkSpeed_get
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
rtk_switch_maxPktLenLinkSpeed_get(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen)
{
    int32   ret;
 
    if (NULL == RT_MAPPER->switch_maxPktLenLinkSpeed_get)
        return RT_ERR_DRIVER_NOT_FOUND;

   RTK_API_LOCK();
    ret = RT_MAPPER->switch_maxPktLenLinkSpeed_get( speed, pLen);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_switch_maxPktLenLinkSpeed_get */

/* Function Name:
  *      rtk_switch_maxPktLenLinkSpeed_set
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
rtk_switch_maxPktLenLinkSpeed_set(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len)
{
    int32   ret;

    if (NULL == RT_MAPPER->switch_maxPktLenLinkSpeed_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->switch_maxPktLenLinkSpeed_set( speed, len);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_switch_maxPktLenLinkSpeed_set */


/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */


/* Function Name:
 *      rtk_switch_mgmtMacAddr_get
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
rtk_switch_mgmtMacAddr_get(rtk_mac_t *pMac)
{
    int32   ret;

    if (NULL == RT_MAPPER->switch_mgmtMacAddr_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->switch_mgmtMacAddr_get( pMac);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_switch_mgmtMacAddr_get */

/* Function Name:
 *      rtk_switch_mgmtMacAddr_set
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
rtk_switch_mgmtMacAddr_set(rtk_mac_t *pMac)
{
    int32   ret;

    if (NULL == RT_MAPPER->switch_mgmtMacAddr_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->switch_mgmtMacAddr_set( pMac);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_switch_mgmtMacAddr_set */


/* Function Name:
 *      rtk_switch_chip_reset
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
rtk_switch_chip_reset(void){
    int32   ret;

    if (NULL == RT_MAPPER->switch_chip_reset)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->switch_chip_reset();
    RTK_API_UNLOCK();
    return ret;
}/* end of rtk_switch_mgmtMacAddr_set */


/* Function Name:
 *      rtk_switch_version_get
 * Description:
 *      Get chip version
 * Input:
 *      pChipId    - chip id
 *      pRev       - revision id
 *      pSubtype   - sub type
 * Output:
 *      None 
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer 
 */
int32
rtk_switch_version_get(uint32 *pChipId, uint32 *pRev, uint32 *pSubtype)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->switch_version_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->switch_version_get(pChipId, pRev, pSubtype);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_switch_version_get */
