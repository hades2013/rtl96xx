/*
 * ===============================================================================
 * Header Name: multicast_control.h
 *
 * General Description:
 *     This file defines Opconn Multicast Control related structures, constants and 
 * function defines and declaration, incl. CTC SCB and traditional IGMP Snooping 
 * with proxy reporting.    
 *
 * ===============================================================================
 * $Copyright: (c) 2007 OPulan Corp.
 * All Rights Reserved.$
 * ===============================================================================
 *  
 * Revision History:
 *                 
 * Author                Date              Description of Changes
 * ----------------   ------------  ----------------------------------------------
 * rzhou              12/10/2007    initial
 * ----------------   ------------  ----------------------------------------------
 *
 * ===============================================================================
 */
 
#ifndef MULTICAST_CONTROL_H
#define MULTICAST_CONTROL_H

#include "opl_ethernet.h" 

/****************************************************  
   Common Macro and Structure Defines 
****************************************************/ 
#define EOPL_PORT_GE				0     /* GE port No. */
#define EOPL_PORT_PON				1     /* PON port No. */
#define EOPL_PORT_CPU				2     /* CPU port No. */

/****************************************************  
   Multicast Constant Macro and Structure Defines 
****************************************************/ 
#define MULTICAST_PORT_MAX_NUM                1     /* Maximum 256 port support */

#define MULTICAST_VLAN_MAX_NUM                4     /* Maximum number of multicast vlan */

#define MULTICAST_CONTROL_ENTRY_MAX_NUM			16    /* Maximum number of multicast control entries support, minimum 16 */

#define MULTICAST_PORT_GROUP_NUM_MAX_DEFAULT	4     /* Default port maximum number of multicast group support */

/* IGMP Snooping Macro and Structure Defines  */

/* IGMP versions */
#define IGMP_VERSION1                         1
#define IGMP_VERSION2                         2

#define DEFAULT_ROBUSTNESS_VARIABLE		2        /* Default 2, but should consider the new querier comes up, so add 1 to 3 */

#define DEFAULT_PROXY_REPORT_INTERVAL	1250   /* 10: 1 second, Default 125 second */

#define DEFAULT_MAX_RESPONSE_TIME		100    /* 10: 1 second, Default 125 second */

/* This is in 0.1 seconds. The value inserted into the group specific query is 10.*/
#define DEFAULT_LAST_MEMBER_QUERY_INTERVAL	  10

/* Last member query counter, default is 2 */
#define DEFAULT_LAST_MEMBER_QUERY_COUNT			2  

#define DEFAULT_UNSOLICITED_REPORT_INTERVAL		10   /* Unsolicited Report Interval */

/* Message Types, Including version Number */
#define IGMP_V1_MEMBERSHIP_REPORT		0x12  /* IGMP membership report for backword-compatibility with IGMP v1 */

#define IGMP_MEMBERSHIP_QUERY				0x11  /* IGMP Ver. 1 and 2 membership query */

#define IGMP_V2_MEMBERSHIP_REPORT		0x16  /* IGMP Ver. 2 membership report */

#define IGMP_V2_LEAVE_GROUP				0x17  /* IGMP Ver. 2 Leave group */

#ifndef INADDR_ALLHOSTS_GROUP
#define INADDR_ALLHOSTS_GROUP				0xe0000001  /* 224.0.0.1 */  
#endif

#ifndef INADDR_ALLROUTERS_GROUP
#define INADDR_ALLROUTERS_GROUP			0xe0000002  /* 224.0.0.2 */  
#endif

#ifndef INADDR_MAX_LOCAL_GROUP
#define INADDR_MAX_LOCAL_GROUP			0xe00000ff  /* 224.0.0.255 */ 
#endif

/* Begin Added*/
#if 0
#define mc_printf printf
#else
#define mc_printf(fmt, ...) 
#endif
/* End   Added of porting */

/* Multicast fast leave admin control define */
typedef enum multicast_fl_admin_ctl_s
{
	MULTICAST_FL_ADMIN_CTL_DEACT  = 0x00000001,	/* Multicast control fast leave admin control deactivate */
	MULTICAST_FL_ADMIN_CTL_ACT,                  		/* Multicast control fast leave admin control activate */
	MULTICAST_FL_ADMIN_CTL_END                   		/* Multicast control fast leave admin control activate end */

}multicast_fl_admin_ctl_e;

/* Multicast switch define */
typedef enum multicast_switch_s
{
	MC_SWITCH_IGMP_SNOOPING	= 0x00,   /* IGMP Snooping with report proxy */
	MC_SWITCH_CTC_DYN_CTL     	= 0x01,   /* CTC Dynamic Controlled */
	MC_SWITCH_END                     			/* End of multicast switches */
    
} multicast_switch_e;

/* Multicast control define */
typedef enum multicast_control_s
{
	MC_CTL_GDA_MAC           		= 0x00,   /* GDA MAC (DA MAC Only) Based */
	MC_CTL_GDA_MAC_VID	     	= 0x01,   /* GDA MAC + multicast Valn ID based */ 
	MC_CTL_GDA_GDA_SA_MAC    = 0x02,   /* GDA MAC + SA MAC (IGMP v3 optional) based */
  MC_CTL_GDA_GDA_IP_VID    = 0x03,
	MC_CTL_END                        			/* End of multicast controls */
	
} multicast_control_e;
 
/* Multicast control define */
typedef enum multicast_control_entry_state_s
{
	MC_CTL_ENTRY_STATE_QUERY_RECEIVED         = 0x01,   /* Multicast control entry state query received */
	MC_CTL_ENTRY_STATE_REPORT_UNSOLICITED,               /* Unsolicited Report received */
	MC_CTL_ENTRY_STATE_REPORT_RECEIVED,                	/* Membership Report received */
	MC_CTL_ENTRY_STATE_REPORT_SENT,                    		/* Multicast control entry state report sent */
	MC_CTL_ENTRY_STATE_GROUP_SEPC_QUERY_SENT_FIRST,		/* Multicast control entry state first group specific query sent */
	MC_CTL_ENTRY_STATE_GROUP_SEPC_QUERY_SENT_SECOND,	/* Multicast control entry state second group specific query sent */
	MC_CTL_ENTRY_STATE_END                             					/* End of multicast control entry states */

} multicast_control_entry_state_e;
 
/* multicast control entry define */
#pragma pack (1) 
typedef struct multicast_control_entry_s
{
	UINT16 port;                    /* Multicast Port or user ID*/
	UINT16 vid;                     /* Multicast vlan ID*/
	UINT8  mmac[ETH_MAC_ADDR_LEN];   /* Group Destionation Address */   

} multicast_control_entry_t;
#pragma pack () 

typedef struct multicast_config_s
{
	UINT8   robustness_variable;
	UINT8   last_mem_query_count;
	UINT16 last_mem_query_interval;
	
}multicast_config_t;

/* multicast control define */
typedef struct multicast_control_tbl_s
{
    /* Multicast control enable or disable */
    INT32 mc_ctl_enable;
    
    /* Multicast control initilized flag */
    INT32 mc_initilized;
    /* IGMP version, default is IGMP Version 2 */
    UINT8 mc_igmp_version;  

	/* Multicast control config */
	 multicast_config_t config;
		
    /* 0x00: GDA MAC; 0x01: GDA MAC + multicast Valn ID; 0x02: GDA MAC + SA MAC (IGMP v3 optional) 
       used in both CTC DYN CTL and traditional IGMP Snooping */
    multicast_control_e mc_type;  
    
    /* Number of multicast control entries, used in both CTC DYN CTL and traditional IGMP Snooping */
    UINT8 mc_num;   
    
    /* Multicast switch,  in CTC DYN CTL or traditional IGMP Snooping */
    multicast_switch_e mc_switch;  

    /* Multicast fast leave admin control, used both in CTC DYN CTL and traditional IGMP Snooping */
    INT32 mc_fl_admin_ctl;

    /* Multicast fast leave enable, used both in CTC DYN CTL and traditional IGMP Snooping */
    INT32 mc_port_fast_leave[MULTICAST_PORT_MAX_NUM];
    
    /* Multicast port maximum group number support, only used in traditional IGMP Snooping */
    UINT8 mc_port_group_num_max[MULTICAST_PORT_MAX_NUM];
    
    /* Multicast port tag stripe */
    INT32 mc_port_tag_stripe[MULTICAST_PORT_MAX_NUM];
    
    /* Multicast control entry port state ,only used in traditional IGMP Snooping */
    multicast_control_entry_state_e mc_ctl_entry_state[MULTICAST_CONTROL_ENTRY_MAX_NUM];
    
    /*  Time to expire of the entry, filled when report received, decreased by IGMP timer */
    UINT16 mc_ctl_entry_expire[MULTICAST_CONTROL_ENTRY_MAX_NUM];
    
    /* Multicast control entry Group IP address */
    UINT32 mc_ctl_entry_group[MULTICAST_CONTROL_ENTRY_MAX_NUM];
    
    /* Multicast control entry list, used in both CTC DYN CTL and traditional IGMP Snooping */
    multicast_control_entry_t mc_ctl_entry[MULTICAST_CONTROL_ENTRY_MAX_NUM]; 
    
} multicast_control_tbl_t;

#endif /* MULTICAST_CONTROL_H */
