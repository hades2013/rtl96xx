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
 * $Revision: 39157 $
 * $Date: 2013-05-06 17:36:30 +0800 (Mon, 06 May 2013) $
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
 *
 */

#ifndef __DAL_RTL9601B_SWITCH_H__
#define __DAL_RTL9601B_SWITCH_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <hal/chipdef/chip.h>
#include <rtk/switch.h>

/*
 * Symbol Definition
 */


/* information of device */
typedef enum dal_rtl9601b_switch_asic_type_e
{
    RTL9601B_ASIC_PON,
    RTL9601B_ASIC_FIBER,
    RTL9601B_ASIC_END
} dal_rtl9601b_switch_asic_type_t;



/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

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
extern int32
dal_rtl9601b_switch_init(void);

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
extern int32
dal_rtl9601b_switch_phyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId);

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
extern int32
dal_rtl9601b_switch_logicalPort_get(int32 portId, rtk_switch_port_name_t *pPortName);

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
extern int32
dal_rtl9601b_switch_port2PortMask_set(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName);



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
extern int32
dal_rtl9601b_switch_port2PortMask_clear(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName);



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
extern int32
dal_rtl9601b_switch_portIdInMask_check(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName);


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
extern int32
dal_rtl9601b_switch_maxPktLenLinkSpeed_get(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen);

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
extern int32
dal_rtl9601b_switch_maxPktLenLinkSpeed_set(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len);



/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */


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
extern int32
dal_rtl9601b_switch_mgmtMacAddr_get(rtk_mac_t *pMac);

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
extern int32
dal_rtl9601b_switch_mgmtMacAddr_set(rtk_mac_t *pMac);


/* Function Name:
 *      dal_apollo_switch_chip_reset
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
extern int32
dal_rtl9601b_switch_chip_reset(void);

#endif /* __DAL_RTL9601B_SWITCH_H__ */

