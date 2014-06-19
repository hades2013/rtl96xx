/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products.
*
* FILENAME:  errors.h
*
* DESCRIPTION:
*
*
* Date Created: Apr 23, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/h/errors.h#1 $
* $Log:$
*
*
**************************************************************************/

#ifndef __ERRORS_H_
#define __ERRORS_H_


/*public error*/
#define ERROR_CODE_BASE                  500
#define ERR_START		            	(ERROR_CODE_BASE + 1)	    /* Error No start */
#define ERR_INVALID_PARAMETERS			(ERROR_CODE_BASE + 2)	/* Invalid parameters passed to the caller, other than Null Pointer. */
#define ERR_NULL_POINTER		    	(ERROR_CODE_BASE + 3)	/* Null pointer passed to caller. */
#define ERR_MEM_ALLOC_FAIL		  		(ERROR_CODE_BASE + 4)	/* Failed to allocate memory from System Main Memory Pool. */
#define ERR_MEM_FREE_FAIL		    	(ERROR_CODE_BASE + 5)	/* Failed to free memory block. */

#define ERR_SEM_CREATE_FAIL		  		(ERROR_CODE_BASE + 6)	/* Failed to create a semaphore. */
#define ERR_SEM_DELETE_FAIL		  		(ERROR_CODE_BASE + 7)	/* Failed to delete a semaphore. */
#define ERR_SEM_TAKE_FAIL		        (ERROR_CODE_BASE + 8)	/* Failed to execute Take Operation to a semaphore. */
#define ERR_SEM_GIVE_FAIL		    	(ERROR_CODE_BASE + 9)	/* Failed to execute Give Operation to a semaphore. */

#define ERR_TIMER_CREATE_FAIL	 	 	(ERROR_CODE_BASE + 10)	/* Failed to create a RTOS timer. */
#define ERR_TIMER_DELETE_FAIL	  		(ERROR_CODE_BASE + 11)	/* Failed to delete a RTOS timer. */
#define ERR_TIMER_START_FAIL	  		(ERROR_CODE_BASE + 12)	/* Failed to start a RTOS timer. */
#define ERR_TIMER_CANCEL_FAIL	  		(ERROR_CODE_BASE + 13)	/* Failed to stop a RTOS timer. */

#define ERR_TASK_CREATE_FAIL	  		(ERROR_CODE_BASE + 14)	/* Failed to create a task or thread in the RTOS. */
#define ERR_TASK_DELETE_FAIL	  		(ERROR_CODE_BASE + 15)	/* Failed to delete a task or thread in the RTOS. */

#define ERR_MSGQ_CREATE_FAIL	 		(ERROR_CODE_BASE + 16)	/* Failed to create a RTOS message queue. */
#define ERR_MSGQ_DELETE_FAIL	 		(ERROR_CODE_BASE + 17)	/* Failed to delete a RTOS message queue. */
#define ERR_MSGQ_SEND_FAIL		 		(ERROR_CODE_BASE + 18)	/* Failed to send a message to the RTOS Message Queue. */
#define ERR_MSGQ_RECV_FAIL		 		(ERROR_CODE_BASE + 19)	/* Failed to receive a message from the RTOS Message Queue. */

#define ERR_INVALID_MAC			   		(ERROR_CODE_BASE + 20)	/* Invalid MAC Address, such as length error, format error, range error. */
#define ERR_INVALID_UNI_MAC		 		(ERROR_CODE_BASE + 21)	/* Invalid Unicast MAC Address. */
#define ERR_INVALID_MC_MAC		 		(ERROR_CODE_BASE + 22)	/* Invalid Multicast MAC Address. */
#define ERR_INVALID_BC_MAC		 		(ERROR_CODE_BASE + 23)	/* Invalid Broadcast MAC Address. */

#define ERR_INVALID_PORT                (ERROR_CODE_BASE + 24)  /*Invalid logic Port */

#define ERR_CALL_DAL_FAIL               (ERROR_CODE_BASE + 25)  /*Failed to call dal layer function*/

#define ERR_CONFIG_SET_FAIL             (ERROR_CODE_BASE + 26)  /*Failed to set config */
#define ERR_CONFIG_GET_FAIL             (ERROR_CODE_BASE + 27)  /*Failed to get config */

#define ERR_DUPLICATE_ITEM              (ERROR_CODE_BASE + 28)  /* Duplicate item input */
#define ERR_OUT_OF_RANGE                (ERROR_CODE_BASE + 29)  /* Out of range input */
#define ERR_EXCEED_MAXIMUM              (ERROR_CODE_BASE + 30)  /* Exceed maximum input */
#define ERR_DATABASE_FAILED             (ERROR_CODE_BASE + 31)  /* Configuration database failed */

#define ERR_INVALID_IP_ADDR		 		(ERROR_CODE_BASE + 32)	/* Invalid IP Address. */
#define ERR_INVALID_IP_MASK		 		(ERROR_CODE_BASE + 33)	/* Invalid IP Mask. */

#define ERR_INVALID_DATE                (ERROR_CODE_BASE + 34)	/* Invalid date format. */
#define ERR_INVALID_TIME                (ERROR_CODE_BASE + 35)	/* Invalid time format. */

#define ERR_NULL_LOID					(ERROR_CODE_BASE + 36)  /* NULL logical ONU id */
#define ERR_NULL_PASSWORD				(ERROR_CODE_BASE + 37)  /* NULL password */

#define ERR_VARNORESOURCE         0x87

//#############################################################################
// DRIVERS errors start at 1
#define DVRS_BASE                  		    1000
#define DVRS_INIT_ERROR                     (DVRS_BASE + 1)

//#############################################################################
// MULTICAST errors start at 3000
#define MULTICAST_BASE                      3000
#define MULTICAST_INIT_ERROR                (MULTICAST_BASE + 0)
#define MULTICAST_CONFIGURATION_ERROR       (MULTICAST_BASE + 10)
#define MULTICAST_IGMP_PACKET_ERROR         (MULTICAST_BASE + 20)
#define MULTICAST_IGMP_PACKET_CHECKSUM_ERROR         (MULTICAST_BASE + 21)
#define MULTICAST_IGMP_VERSION_UNSUPPORTED  (MULTICAST_BASE + 22)
#define MULTICAST_IGMP_JOIN_ACTION_ERROR    (MULTICAST_BASE + 30)
#define MULTICAST_IGMP_ADD_ERROR            (MULTICAST_BASE + 31)
#define MULTICAST_IGMP_QUERY_ACTION_VLAN_ERROR   (MULTICAST_BASE + 40)
#define MULTICAST_IGMP_QUERY_ACTION_MULTICAST_NOT_EXIST   (MULTICAST_BASE + 41)
#define MULTICAST_IGMP_QUERY_ACTION_PACKET_ERROR   (MULTICAST_BASE + 42)
#define MULTICAST_IGMP_LEAVE_ACTION_ERROR   (MULTICAST_BASE + 50)
#define MULTICAST_IGMP_LEAVE_ACTION_DESTIP_ERROR   (MULTICAST_BASE + 51)
#define MULTICAST_IGMP_LEAVE_ACTION_GROUP_NOT_EXIST_ERROR   (MULTICAST_BASE + 52)
#define MULTICAST_IGMP_LEAVE_ACTION_PORT_NOT_EXIST_ERROR   (MULTICAST_BASE + 53)

#define IGMP_CONFIG_MULTICAST_VLAN_NODE_EXIST (MULTICAST_BASE + 54)
#define IGMP_CONFIG_MULTICAST_VLAN_NODE_NOT_EXIST (MULTICAST_BASE + 55)
#define IGMP_CONFIG_PORT_TAGSTRIP_EXIST (MULTICAST_BASE + 56)

#define MULTICAST_IGMP_MULTICAST_VLAN_ERROR (MULTICAST_BASE + 57)
#define MULTICAST_IGMP_JOIN_GROUP_ADDRESS_ERROR    (MULTICAST_BASE + 58)

#define MULTICAST_IGMP_LEAVE_ACTION_DESTMAC_ERROR   (MULTICAST_BASE + 59)
#define MULTICAST_IGMP_LEAVE_GROUP_ADDRESS_ERROR   (MULTICAST_BASE + 60)
#define MULTICAST_IGMP_PARA_ERROR   (MULTICAST_BASE + 61)
#define MULTICAST_IGMP_MULTICAST_MAC_ERROR (MULTICAST_BASE + 62)
#define MULTICAST_IGMP_JOIN_MAX_GROUP_NUMBER_ERROR    (MULTICAST_BASE + 63)


//#############################################################################
// PORTS errors start at 6000
#define PORTS_BASE                          6000
#define PORTS_INIT_ERROR                    (PORTS_BASE + 0)
#define PORTS_IWF_PARAMETER_OUTOF_RANGE     (PORTS_BASE + 1)
#define PORTS_IWF_UPPT_LATENCY_TYPE_ERROR   (PORTS_BASE + 2)
#define PORTS_IWF_PORT_PHY_MISMATCH         (PORTS_BASE + 3)
#define PORTS_IWF_PARAMETER_INVALID         (PORTS_BASE + 4)

//#############################################################################
// PORT MIRROR errors start at 7000
#define PORT_MIRROR_BASE                                  7000
#define PORT_MIRROR_INVALID_POINTER            (PORT_MIRROR_BASE + 0)
#define PORT_MIRROR_EXIST_CHECK_ERROR        (PORT_MIRROR_BASE + 1)

//#############################################################################
// QOS errors start at 9000
#define QOS_BASE                            9000
#define QOS_INIT_ERROR                      (QOS_BASE + 0)
#define QOS_GENERAL_ERROR                   QOS_INIT_ERROR
#define QOS_INVALID_POINTER                 (QOS_GENERAL_ERROR + 1)
#define QOS_INVALID_TYPE                    (QOS_GENERAL_ERROR + 2)

#define QOS_CLS_EXIST_CHECK_ERROR           (QOS_GENERAL_ERROR + 10)
#define QOS_CLS_INSTALL_CHECK_ERROR         (QOS_GENERAL_ERROR + 11)
#define QOS_CLS_NOANY_MATCH                 (QOS_GENERAL_ERROR + 12)
#define QOS_CLS_ACTION_ERROR                (QOS_GENERAL_ERROR + 13)

//#############################################################################
// SECURITY errros start at 10000
#define SECURITY_BASE                       10000

//#############################################################################
// SNMP errros start at 13000
#define SNMP_BASE                           13000
#define SNMP_INIT_ERROR                     (SNMP_BASE + 0)

//#############################################################################
// SYSTEM errors start at 17000
#define SYS_BASE                  		    17000
#define SYS_INIT_ERROR                      (SYS_BASE + 0)
#define SYS_TELNETD_ALREADY_INITIALIZED     (SYS_BASE + 1)
#define SYS_TELNETD_CREATE_SOCKET_FAILED    (SYS_BASE + 2)
#define SYS_TELNETD_CREATE_FAILED           (SYS_BASE + 3)
#define SYS_TELNETD_NOT_INITIALIZED         (SYS_BASE + 4)
#define SYS_LOG_THREAD_ALREADY_INITIALIZED  (SYS_BASE + 5)
#define SYS_LOG_THREAD_CREATE_FAILED        (SYS_BASE + 6)
#define SYS_LOG_THREAD_NOT_INITIALIZED      (SYS_BASE + 7)
#define SYS_USER_INVALID                    (SYS_BASE + 8)
#define SYS_USER_STATE_INVALID              (SYS_BASE + 9)
#define SYS_USER_PASSWD_INVALID             (SYS_BASE + 10)
#define SYS_USER_PASSWD_DONT_MATCH          (SYS_BASE + 11)
#define SYS_SESSION_INVALID_ID              (SYS_BASE + 12)
#define SYS_SESSION_CAN_NOT_KILL_YOURSELF   (SYS_BASE + 13)
#define SYS_LOG_OUT_OF_MEMORY               (SYS_BASE + 14)
#define SYS_LOG_SERVER_ALREADY_EXIST        (SYS_BASE + 15)
#define SYS_LOG_SERVER_NOT_EXIST        	(SYS_BASE + 16)
#define SYS_LOG_SERVER_CFG_ERROR            (SYS_BASE + 17)

//#############################################################################
// WEB errors start at 23000
#define WEB_BASE                  		    23000
#define WEB_INIT_ERROR                      (WEB_BASE + 0)
#define WEB_ALREADY_INITIALIZED             (WEB_BASE + 1)
#define WEB_HAS_NOT_INITIALIZED             (WEB_BASE + 1)
#define WEB_STOP_FAILED                     (WEB_BASE + 1)
//#############################################################################
// VLAN errors start at 26000
#define VLAN_BASE                           26000
#define VLAN_INIT_ERROR                    (VLAN_BASE + 0)
#define VLAN_GERNEL_ERROR                  (VLAN_INIT_ERROR)
#define VLAN_NAME_ID_MAPERROR              (VLAN_GERNEL_ERROR + 1)
#define VLAN_ID_EXIST_ERR                  (VLAN_GERNEL_ERROR + 2)
#define VLAN_NAME_EXIST_ERR                (VLAN_GERNEL_ERROR + 3)
#define VLAN_PORT_IS_MEMBER                (VLAN_GERNEL_ERROR + 4)
#define VOICEVLAN_PORT_EXCEED               (VLAN_GERNEL_ERROR +5)


//PORT errors start at 29000
#define PORT_BASE						    29000
#define PORT_INIT_ERROR				        (PORT_BASE + 0)
#define PORT_INVALID				    	(PORT_BASE + 1)

//VTT errors start at  32000
#define VTT_BASE                            32000
#define VTT_INIT_EROR                       (VTT_BASE + 0)
#define VTT_GERNEL_ERROR                    (VTT_BASE + 1)
#define VTT_ENTRY_ISEXISTED                 (VTT_BASE + 2)
#define VTT_ENTRY_CONFLICT                  (VTT_BASE + 3)
#define VTT_VLAN_NOT_AT_TRANSLATION_MODE    (VTT_BASE + 4)
#define VTT_ENTRY_FULL                      (VTT_BASE + 5)
#define VTT_NO_SUCH_ENTRY                   (VTT_BASE + 6)
#define VTT_INDEX_OUT_OF_RANGE              (VTT_BASE + 7)

#define VLAN_TRUNK_BASE						33000
#define PORT_NOT_AT_VLAN_TRUNK_MODE    		(VLAN_TRUNK_BASE + 0)
#define PORT_NO_VLAN_TRUNK_RESOURCE			(VLAN_TRUNK_BASE + 1)
#define PORT_VLAN_TRUNK_ENTRY_NOT_EXIST		(VLAN_TRUNK_BASE + 2)


/* begin added by jiangmingli, 2008-08-29 */
//#############################################################################
// FDB errors start at 35000
#define FDB_BASE							35000
#define FDB_MAC_AGINGTIME_SET_ERROR			(FDB_BASE + 1)
#define FDB_MAC_AGINGTIME_GET_ERROR			(FDB_BASE + 2)
#define FDB_MAC_ADD_ERROR					(FDB_BASE + 3)
#define FDB_MAC_DEL_ERROR					(FDB_BASE + 4)
#define FDB_MAC_PORT_ADD_ERROR				(FDB_BASE + 5)
#define FDB_MAC_PORT_DEL_ERROR				(FDB_BASE + 6)
#define FDB_MAC_DEL_ALL_ERROR				(FDB_BASE + 7)
#define FDB_MAC_DISCARD_ERROR				(FDB_BASE + 8)
#define FDB_MAC_DISCARD_CLEAR_ERROR			(FDB_BASE + 9)
#define FDB_MAC_SHOW_ONE_ERROR				(FDB_BASE + 10)
#define FDB_MAC_SHOW_ALL_ERROR				(FDB_BASE + 11)
#define FDB_MAC_MAC_LEARN_SET_ERROR			(FDB_BASE + 12)
#define FDB_MAC_MAC_AGING_SET_ERROR			(FDB_BASE + 13)
#define FDB_MAC_SOFT_LEARN_SET_ERROR        (FDB_BASE + 14)
#define FDB_MAC_SOFT_LEARN_GET_ERROR        (FDB_BASE + 15)
#define FDB_MAC_MOVE_ERROR			        (FDB_BASE + 16)

#define FDB_MAC_CFG_FILE_SECTION_READ_ERROR			(FDB_BASE + 100)
#define FDB_MAC_CFG_FILE_KEY_READ_ERROR				(FDB_BASE + 101)
#define FDB_MAC_CFG_FILE_AGINGTIME_SET_ERROR		(FDB_BASE + 102)
#define FDB_MAC_CFG_FILE_MAC_DISCARD_ADD_ERROR		(FDB_BASE + 103)
#define FDB_MAC_CFG_FILE_MAC_DISCARD_DEL_ERROR		(FDB_BASE + 104)
#define FDB_MAC_CFG_FILE_MAC_ADD_ERROR				(FDB_BASE + 105)
#define FDB_MAC_CFG_FILE_MAC_DEL_ERROR				(FDB_BASE + 106)
#define FDB_MAC_CFG_FILE_MAC_PORT_ADD_ERROR			(FDB_BASE + 107)
#define FDB_MAC_CFG_FILE_MAC_PORT_DEL_ERROR			(FDB_BASE + 108)
/* end added by jiangmingli, 2008-08-29 */

/* begin added by jiangmingli, 2008-10-10 */
//#############################################################################
// RSTP errors start at 38000
#define RSTP_BASE							38000
#define RSTP_BRD_ENABLE_ERROR				(RSTP_BASE + 1)
#define RSTP_BRD_DISABLE_ERROR				(RSTP_BASE + 2)
#define RSTP_BRD_HELLOTIME_SET_ERROR		(RSTP_BASE + 3)
#define RSTP_BRD_FWDDELAY_SET_ERROR			(RSTP_BASE + 4)
#define RSTP_BRD_MAXAGE_SET_ERROR			(RSTP_BASE + 5)
#define RSTP_BRD_VERSION_SET_ERROR			(RSTP_BASE + 6)
#define RSTP_BRD_PRI_SET_ERROR				(RSTP_BASE + 7)
#define RSTP_PORT_MODE_SET_ERROR			(RSTP_BASE + 8)
#define RSTP_PORT_EDGE_SET_ERROR			(RSTP_BASE + 9)
#define RSTP_PORT_P2P_SET_ERROR				(RSTP_BASE + 10)
#define RSTP_PORT_PATH_COST_SET_ERROR		(RSTP_BASE + 11)
#define RSTP_PORT_PRI_SET_ERROR				(RSTP_BASE + 12)
#define RSTP_PORT_STAT_GET_ERROR			(RSTP_BASE + 13)
#define RSTP_BRD_NOT_ENABLE_WARNING			(RSTP_BASE + 14)
#define RSTP_BRD_CFG_RECOVER_ERROR			(RSTP_BASE + 15)
#define RSTP_PORT_CFG_RECOVER_ERROR			(RSTP_BASE + 16)

#define RSTP_CFG_FILE_SECTION_READ_ERROR		(RSTP_BASE + 100)
#define RSTP_CFG_FILE_KEY_READ_ERROR			(RSTP_BASE + 101)
#define RSTP_CFG_FILE_BRD_MODE_SET_ERROR		(RSTP_BASE + 102)
#define RSTP_CFG_FILE_BRD_HELLOTIME_SET_ERROR	(RSTP_BASE + 103)
#define RSTP_CFG_FILE_BRD_FWDDELAY_SET_ERROR	(RSTP_BASE + 104)
#define RSTP_CFG_FILE_BRD_MAXAGE_SET_ERROR		(RSTP_BASE + 105)
#define RSTP_CFG_FILE_BRD_VERSION_SET_ERROR		(RSTP_BASE + 106)
#define RSTP_CFG_FILE_BRD_PRI_SET_ERROR			(RSTP_BASE + 107)
#define RSTP_CFG_FILE_PORT_MODE_SET_ERROR		(RSTP_BASE + 108)
#define RSTP_CFG_FILE_PORT_EDGE_SET_ERROR		(RSTP_BASE + 109)
#define RSTP_CFG_FILE_PORT_P2P_SET_ERROR		(RSTP_BASE + 110)
#define RSTP_CFG_FILE_PORT_PATH_COST_SET_ERROR	(RSTP_BASE + 111)
#define RSTP_CFG_FILE_PORT_PRI_SET_ERROR		(RSTP_BASE + 112)
#define RSTP_CFG_FILE_BRD_DIAMETER_SET_ERROR	(RSTP_BASE + 113)

#define RSTP_INIT_ERROR						(RSTP_BASE + 1000)
/* end added by jiangmingli, 2008-10-10 */


///Get the error message for the given ERRCODE
#define OP_ERROR_MSG(ERRCODE)                 \
    sysGetErrorString(ERRCODE)


#endif /* #ifndef __ERRORS_H_ */

