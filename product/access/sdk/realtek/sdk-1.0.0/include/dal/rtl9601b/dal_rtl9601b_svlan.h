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
 * Purpose : Definition of SVLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) 802.1ad, SVLAN [VLAN Stacking] 
 *
 */


#ifndef __DAL_RTL9601B_SVLAN_H__
#define __DAL_RTL9601B_SVLAN_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/svlan.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
typedef enum dal_rtl9601b_svlan_priSel_e
{
    DAL_RTL9601B_SVLAN_PRISEL_INTERNAL_PRI  = 0,
    DAL_RTL9601B_SVLAN_PRISEL_1QTAG_PRI,
    DAL_RTL9601B_SVLAN_PRISEL_RESERVED,
    DAL_RTL9601B_SVLAN_PRISEL_PBPRI,
    DAL_RTL9601B_SVLAN_PRISEL_END,    

} dal_rtl9601b_svlan_priSel_t;



typedef enum dal_rtl9601b_svlan_untag_action_e
{
    DAL_RTL9601B_SVLAN_UNTAG_DROP = 0,
    DAL_RTL9601B_SVLAN_UNTAG_TRAP,
    DAL_RTL9601B_SVLAN_UNTAG_ASSIGN,
    DAL_RTL9601B_SVLAN_UNTAG_END
} dal_rtl9601b_svlan_untag_action_t;


/*
 * Macro Declaration
 */
#define DAL_RTL9601B_SVLAN_VID_NOT_ACCEPT(vid) \
    ((RTK_VLAN_ID_MAX < vid || 0 > vid)? 1: 0)


/*
 * Function Declaration
 */
 
/* Function Name:
 *      dal_rtl9601b_svlan_init
 * Description:
 *      Initialize svlan module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize svlan module before calling any svlan APIs.
 */
extern int32
dal_rtl9601b_svlan_init(void);

/* Function Name:
 *      dal_rtl9601b_svlan_create
 * Description:
 *      Create the svlan.
 * Input:
 *      svid - svlan id to be created
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_EXIST - SVLAN entry is exist
 * Note:
 *      None
 */
extern int32
dal_rtl9601b_svlan_create(rtk_vlan_t svid);


/* Function Name:
 *      dal_rtl9601b_svlan_destroy
 * Description:
 *      Destroy the svlan.
 * Input:
 *      svid - svlan id to be destroyed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND - specified svlan entry not found
 * Note:
 *      None
 */
extern int32
dal_rtl9601b_svlan_destroy(rtk_vlan_t svid);

/* Function Name:
 *      dal_rtl9601b_svlan_portSvid_get
 * Description:
 *      Get port default svlan id.
 * Input:
 *      port  - port id
 * Output:
 *      pSvid - pointer buffer of port default svlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9601b_svlan_portSvid_get(rtk_port_t port, rtk_vlan_t *pSvid);


/* Function Name:
 *      dal_rtl9601b_svlan_portSvid_set
 * Description:
 *      Set port default svlan id.
 * Input:
 *      port - port id
 *      svid - port default svlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
extern int32
dal_rtl9601b_svlan_portSvid_set(rtk_port_t port, rtk_vlan_t svid);

/* Function Name:
 *      dal_rtl9601b_svlan_servicePort_get
 * Description:
 *      Get service ports from the specified device.
 * Input:
 *      port        - port id
 * Output:
 *      pEnable     - status of service port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9601b_svlan_servicePort_get(rtk_port_t port, rtk_enable_t *pEnable);


/* Function Name:
 *      dal_rtl9601b_svlan_servicePort_set
 * Description:
 *      Set service ports to the specified device.
 * Input:
 *      port       - port id
 *      enable     - status of service port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_rtl9601b_svlan_servicePort_set(rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_rtl9601b_svlan_memberPort_set
 * Description:
 *      Replace the svlan members.
 * Input:
 *      svid            - svlan id
 *      pSvlanPortmask - svlan member ports
 *      pSvlanUntagPortmask - svlan untag member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_ENTRY_INDEX     - invalid svid entry no
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND - specified svlan entry not found
 * Note:
 *      (1) Don't care the original svlan members and replace with new configure
 *          directly.
 *      (2) svlan portmask only for svlan ingress filter checking
 */
extern int32
dal_rtl9601b_svlan_memberPort_set(rtk_vlan_t svid, rtk_portmask_t *pSvlanPortmask, rtk_portmask_t *pSvlanUntagPortmask);



/* Function Name:
 *      dal_rtl9601b_svlan_memberPort_get
 * Description:
 *      Get the svlan members.
 * Input:
 *      svid            - svlan id
 *      pSvlanPortmask - svlan member ports
 *      pSvlanUntagPortmask - svlan untag member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SVLAN_ENTRY_INDEX     - invalid svid entry no
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND - specified svlan entry not found
 * Note:
 *      (1) Don't care the original svlan members and replace with new configure
 *          directly.
 *      (2) svlan portmask only for svlan ingress filter checking
 */
extern int32
dal_rtl9601b_svlan_memberPort_get(rtk_vlan_t svid, rtk_portmask_t *pSvlanPortmask, rtk_portmask_t *pSvlanUntagPortmask);


/* Function Name:
 *      dal_rtl9601b_svlan_tpidEntry_get
 * Description:
 *      Get the svlan TPID.
 * Input:
 *      svlanIndex   - index of svlan table
 * Output:
 *      pSvlanTagId - pointer buffer of svlan TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Only support pSvlanTagId 0 in Apollo.
 */
extern int32
dal_rtl9601b_svlan_tpidEntry_get(uint32 svlanIndex, uint32 *pSvlanTagId);


/* Function Name:
 *      dal_rtl9601b_svlan_tpidEntry_set
 * Description:
 *      Set the svlan TPID.
 * Input:
 *      svlan_index  - index of svlan table
 *      svlan_tag_id - svlan TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Only support pSvlan_tag_id 0 in Apollo.
 */
extern int32
dal_rtl9601b_svlan_tpidEntry_set(uint32 svlan_index, uint32 svlan_tag_id);


/* Function Name:
 *      dal_rtl9601b_svlan_priorityRef_set
 * Description:
 *      Set S-VLAN upstream priority reference setting.
 * Input:
 *      ref - reference selection parameter.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      The API can set the upstream SVLAN tag priority reference source. The related priority
 *      sources are as following:
 *      - REF_INTERNAL_PRI,
 *      - REF_CTAG_PRI,
 *      - REF_SVLAN_PRI.
 */
extern int32 
dal_rtl9601b_svlan_priorityRef_set(rtk_svlan_pri_ref_t ref);

/* Function Name:
 *      dal_rtl9601b_svlan_priorityRef_get
 * Description:
 *      Get S-VLAN upstream priority reference setting.
 * Input:
 *      None
 * Output:
 *      pRef - reference selection parameter.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can get the upstream SVLAN tag priority reference source. The related priority
 *      sources are as following:
 *      - REF_INTERNAL_PRI,
 *      - REF_CTAG_PRI,
 *      - REF_SVLAN_PRI.
 */
extern int32 
dal_rtl9601b_svlan_priorityRef_get(rtk_svlan_pri_ref_t *pRef);

/* Function Name:
 *      dal_rtl9601b_svlan_memberPortEntry_set
 * Description:
 *      Configure system SVLAN member content
 * Input:
 *      svid - SVLAN id
 *      psvlan_cfg - SVLAN member configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Invalid input parameter.
 *      RT_ERR_SVLAN_VID        - Invalid SVLAN VID parameter.
 *      RT_ERR_PORT_MASK        - Invalid portmask.
 *      RT_ERR_SVLAN_TABLE_FULL - SVLAN configuration is full.
 * Note:
 *      The API can set system 64 accepted s-tag frame format. Only 64 SVID S-tag frame will be accpeted
 *      to receiving from uplink ports. Other SVID S-tag frame or S-untagged frame will be droped by default setup.
 *      - rtk_svlan_memberCfg_t->svid is SVID of SVLAN member configuration.
 *      - rtk_svlan_memberCfg_t->memberport is member port mask of SVLAN member configuration.
 *      - rtk_svlan_memberCfg_t->fid is filtering database of SVLAN member configuration.
 *      - rtk_svlan_memberCfg_t->priority is priority of SVLAN member configuration.
 */
extern int32 
dal_rtl9601b_svlan_memberPortEntry_set(rtk_svlan_memberCfg_t *pSvlan_cfg);


/* Function Name:
 *      dal_rtl9601b_svlan_memberPortEntry_get
 * Description:
 *      Get SVLAN member Configure.
 * Input:
 *      svid - SVLAN id
 * Output:
 *      pSvlan_cfg - SVLAN member configuration
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can get system 64 accepted s-tag frame format. Only 64 SVID S-tag frame will be accpeted
 *      to receiving from uplink ports. Other SVID S-tag frame or S-untagged frame will be droped.
 */
extern int32 
dal_rtl9601b_svlan_memberPortEntry_get(rtk_svlan_memberCfg_t *pSvlan_cfg);



/* Function Name:
 *      dal_rtl9601b_svlan_untagAction_set
 * Description:
 *      Configure Action of downstream UnStag packet
 * Input:
 *      action  - Action for UnStag
 *      svid    - The SVID assigned to UnStag packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can configure action of downstream Un-Stag packet. A SVID assigned
 *      to the un-stag is also supported by this API. The parameter of svid is
 *      only referenced when the action is set to UNTAG_ASSIGN
 */
extern int32 
dal_rtl9601b_svlan_untagAction_set(rtk_svlan_action_t action, rtk_vlan_t svid);


/* Function Name:
 *      dal_rtl9601b_svlan_untagAction_get
 * Description:
 *      Get Action of downstream UnStag packet
 * Input:
 *      None
 * Output:
 *      pAction  - Action for UnStag
 *      pSvid    - The SVID assigned to UnStag packet
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_SVLAN_VID                - Invalid SVLAN VID parameter.
 *      RT_ERR_SVLAN_ENTRY_NOT_FOUND    - specified svlan entry not found.
 *      RT_ERR_OUT_OF_RANGE             - input out of range.
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      The API can Get action of downstream Un-Stag packet. A SVID assigned
 *      to the un-stag is also retrieved by this API. The parameter pSvid is
 *      only refernced when the action is UNTAG_ASSIGN
 */
extern int32 
dal_rtl9601b_svlan_untagAction_get(rtk_svlan_action_t *pAction, rtk_vlan_t *pSvid);


/* Function Name:
 *      dal_rtl9601b_svlan_trapPri_get
 * Description:
 *      Get svlan trap priority
 * Input:
 *      None
 * Output:
 *      pPriority - priority for trap packets
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9601b_svlan_trapPri_get(rtk_pri_t *pPriority);


/* Function Name:
 *      dal_rtl9601b_svlan_trapPri_set
 * Description:
 *      Set svlan trap priority
 * Input:
 *      priority - priority for trap packets
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QOS_INT_PRIORITY 
 * Note:
 *      None
 */
extern int32
dal_rtl9601b_svlan_trapPri_set(rtk_pri_t priority);

/* Function Name:
 *      dal_rtl9601b_svlan_deiKeepState_get
 * Description:
 *      Get svlan dei keep state
 * Input:
 *      None
 * Output:
 *      pEnable - state of keep dei 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9601b_svlan_deiKeepState_get(rtk_enable_t *pEnable);

/* Function Name:
 *      dal_rtl9601b_svlan_deiKeepState_set
 * Description:
 *      Set svlan dei keep state
 * Input:
 *      enable  - state of DMAC CVID Selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                    - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_rtl9601b_svlan_deiKeepState_set(rtk_enable_t enable);


#endif /* __DAL_RTL9601B_SVLAN_H__ */
