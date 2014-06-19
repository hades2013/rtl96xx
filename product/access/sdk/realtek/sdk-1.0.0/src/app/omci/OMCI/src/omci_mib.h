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
 * $Revision: 39101 $
 * $Date: 2013-05-03 04:35:27 -0500 (Fri, 03 May 2013) $
 *
 * Purpose : Definition of MIB related header
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) OMCI
 *
 */


#ifndef __OMCI_MIB_H__
#define __OMCI_MIB_H__


/* ONT-G  */

// traffic scheduler and priority queue mechanism are used for upstream traffic.
#define TXC_ONTG_TRAF_OPTION (0) 


/* ONT2-G */

#define TXC_ONT2G_PRIO_Q_NUM   (TXC_PRIO_Q_DS_Q_NUM + TXC_PRIO_Q_US_Q_NUM) 

// 128 Gem flow
#define TXC_ONT2G_GEM_PORT_NUM (128)

#define TXC_HW_CF_RULE_NUM (512)

/* CardHolder */

#define SIEMENS_OLT_PON_SLOT  (0x80)
#define SIEMENS_OLT_FE_SLOT   (0x4)
#define SIEMENS_OLT_GE_SLOT   (0x5)
#define SIEMENS_OLT_VEIP_SLOT (0x6)

// ethernet mode: ( 4 X FE + 1 X GE ) or ( 2 X GE ), port id from 0 to 4


#define TXC_CARDHLD_PON_SLOT     (0)
#define TXC_CARDHLD_ETH_FE_SLOT  (1)
#define TXC_CARDHLD_ETH_GE_SLOT  (2)
#define TXC_CARDHLD_VEIP_SLOT    (3)

#define TXC_CARDHLD_SLOT_NUM     (4)

#define TXC_GET_CARDTYPE_BY_SLOT_ID(slotId) (( slotId  == TXC_CARDHLD_PON_SLOT)    ? (248) :   \
( slotId  == TXC_CARDHLD_ETH_FE_SLOT) ? (24)  :   \
( slotId  == TXC_CARDHLD_ETH_GE_SLOT) ? (47)  :   \
48)
                                                
#define TXC_CARDHLD_PON_SLOT_PORT_COUNT  (1)
#define TXC_CARDHLD_FE_SLOT_PORT_COUNT   (0)
#define TXC_CARDHLD_GE_SLOT_PORT_COUNT   (4)
#define TXC_CARDHLD_VEIP_SLOT_PORT_COUNT (1)
    
#define TXC_GET_PORT_COUNT_BY_SLOT_ID(slotId) ((slotId) == TXC_CARDHLD_PON_SLOT ? TXC_CARDHLD_PON_SLOT_PORT_COUNT : \
                                               (slotId) == TXC_CARDHLD_ETH_FE_SLOT ? TXC_CARDHLD_FE_SLOT_PORT_COUNT : \
                                               (slotId) == TXC_CARDHLD_ETH_GE_SLOT ? TXC_CARDHLD_GE_SLOT_PORT_COUNT : \
					       TXC_CARDHLD_VEIP_SLOT_PORT_COUNT)
                                                    
#define TXC_GET_SLOT_NUM_BY_SLOT_ID(slotId) ((slotId) == TXC_CARDHLD_PON_SLOT ? SIEMENS_OLT_PON_SLOT : \
                                             (slotId) == TXC_CARDHLD_ETH_FE_SLOT ? SIEMENS_OLT_FE_SLOT : \
                                             (slotId) == TXC_CARDHLD_ETH_GE_SLOT ? SIEMENS_OLT_GE_SLOT : \
					     					 SIEMENS_OLT_VEIP_SLOT)


/* T-cont */

// up to 52 T-Conts, port id from 5 to 56. 
#ifdef __MDU_GMII__

#define TXC_TCONT_TCONT_NUM        (8)
#define TXC_TCONT_PRIO_Q_PER_TCONT (2)

#else  /*__SFU_SSSMII__*/

#define TXC_TCONT_TCONT_NUM        (8)
#define TXC_TCONT_PRIO_Q_PER_TCONT (8)

#endif /*__MDU_GMII__*/
#define TXC_MAX_QUEUE_ID (128)
#define TXC_MAX_TCONT_ID (32)


/* Traffic Scheduler */

// not support HOL or WRR
#define TXC_TRAFF_SKED_POLICY      (0) 


/* Priority Queue */

// highest priority
#define TXC_PRIO_Q_PRIOR           (0)

// not sure
#define TXC_PRIO_Q_MAX_SIZE        (0xCCC)

// 8 Queues per Ethernet port each representing different class of service.
#define TXC_PRIO_Q_Q_NUM_PER_PORT  (8)  

#define TXC_PRIO_Q_DS_Q_BASE       (0)
#define TXC_PRIO_Q_DS_Q_NUM        (40)

#define TXC_PRIO_Q_US_Q_BASE       (43)
#define TXC_PRIO_Q_US_Q_NUM        (120)


/*SWImage*/
#define TXC_DEFAULT_SW_VERSION    "RTL9600V1.01"

#endif /*__OMCI_MIB_H__*/

