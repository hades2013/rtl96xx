/*************************************************************************
*
*  COPYRIGHT (C) 2003-2007 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential                       
*
* 	This software is made available only to customers and prospective        
* 	customers of Opulan Technologies Corporation under license and may be
*	 used only with Opulan semi-conductor products.                           
*
* FILENAME: opl_errno.h
*
* DESCRIPTION: Opulan privated defined Error Codes. this
*	file is supposed to be included by driver Module source files of 
*	Opulan chip products.
*
* Date Created: 05/18/2007
*
*
*
**************************************************************************/

#ifndef OPL_ERRNO_H
#define OPL_ERRNO_H

/***** INCLUDE FILES *****/

#include "opl_driver.h"

/***** DEFINES and ENUMS *****/

#define OPL_ERROR_CODE_BASE						OPL_ZERO


#define OPL_ERR_START		            						(OPL_ERROR_CODE_BASE - 1)	    /* Error No start */
#define OPL_ERR_INVALID_PARAMETERS			(OPL_ERROR_CODE_BASE - 2)	/* Invalid parameters passed to the caller, other than Null Pointer. */
#define OPL_ERR_NULL_POINTER		    				(OPL_ERROR_CODE_BASE - 3)	/* Null pointer passed to caller. */
#define OPL_ERR_REG_WRITE_FAIL	   				(OPL_ERROR_CODE_BASE - 4)	/* Failed to write to a chip register. */
#define OPL_ERR_REG_READ_FAIL		    				(OPL_ERROR_CODE_BASE - 5)	/* Failed to read from a chip register. */
#define OPL_ERR_TAB_WRITE_FAIL		  			(OPL_ERROR_CODE_BASE - 6)	/* Failed to write to a record of a Chip Internal Table. */
#define OPL_ERR_TAB_READ_FAIL		    				(OPL_ERROR_CODE_BASE - 7)	/* Failed to read a record of a Chip Internal Table. */
#define OPL_ERR_TAB_WRITE_TIMEOUT	  		(OPL_ERROR_CODE_BASE - 8)	/* Failed to write to a record of a Chip Internal Table due to hardware response time out. */
#define OPL_ERR_TAB_READ_TIMEOUT	  			(OPL_ERROR_CODE_BASE - 9)	/* Failed to read a record of a Chip Internal Table due to hardware response time out. */
#define OPL_ERR_MEM_ALLOC_FAIL		  			(OPL_ERROR_CODE_BASE - 10)	/* Failed to allocate memory from System Main Memory Pool. */
#define OPL_ERR_MEM_FREE_FAIL		    			(OPL_ERROR_CODE_BASE - 11)	/* Failed to free memory block. */

#define OPL_ERR_SEM_CREATE_FAIL		  			(OPL_ERROR_CODE_BASE - 20)	/* Failed to create a semaphore. */
#define OPL_ERR_SEM_DELETE_FAIL		  			(OPL_ERROR_CODE_BASE - 21)	/* Failed to delete a semaphore. */
#define OPL_ERR_SEM_TAKE_FAIL		    				(OPL_ERROR_CODE_BASE - 22)	/* Failed to execute Take Operation to a semaphore. */
#define OPL_ERR_SEM_GIVE_FAIL		    				(OPL_ERROR_CODE_BASE - 23)	/* Failed to execute Give Operation to a semaphore. */

#define OPL_ERR_TIMER_CREATE_FAIL	 	 		(OPL_ERROR_CODE_BASE - 30)	/* Failed to create a RTOS timer. */
#define OPL_ERR_TIMER_DELETE_FAIL	  			(OPL_ERROR_CODE_BASE - 31)	/* Failed to delete a RTOS timer. */
#define OPL_ERR_TIMER_START_FAIL	  			(OPL_ERROR_CODE_BASE - 32)	/* Failed to start a RTOS timer. */
#define OPL_ERR_TIMER_CANCEL_FAIL	  			(OPL_ERROR_CODE_BASE - 33)	/* Failed to stop a RTOS timer. */

#define OPL_ERR_TASK_CREATE_FAIL	  			(OPL_ERROR_CODE_BASE - 40)	/* Failed to create a task or thread in the RTOS. */
#define OPL_ERR_TASK_DELETE_FAIL	  			(OPL_ERROR_CODE_BASE - 41)	/* Failed to delete a task or thread in the RTOS. */

#define OPL_ERR_MSGQ_CREATE_FAIL	 			(OPL_ERROR_CODE_BASE - 50)	/* Failed to create a RTOS message queue. */
#define OPL_ERR_MSGQ_DELETE_FAIL	 			(OPL_ERROR_CODE_BASE - 51)	/* Failed to delete a RTOS message queue. */
#define OPL_ERR_MSGQ_SEND_FAIL		 			(OPL_ERROR_CODE_BASE - 52)	/* Failed to send a message to the RTOS Message Queue. */
#define OPL_ERR_MSGQ_RECV_FAIL		 			(OPL_ERROR_CODE_BASE - 53)	/* Failed to receive a message from the RTOS Message Queue. */

#define OPL_ERR_INVALID_MAC			   			(OPL_ERROR_CODE_BASE - 60)	/* Invalid MAC Address, such as length error, format error, range error. */
#define OPL_ERR_INVALID_UNI_MAC		 			(OPL_ERROR_CODE_BASE - 61)	/* Invalid Unicast MAC Address. */
#define OPL_ERR_INVALID_MC_MAC		 			(OPL_ERROR_CODE_BASE - 62)	/* Invalid Multicast MAC Address. */
#define OPL_ERR_INVALID_BC_MAC		 			(OPL_ERROR_CODE_BASE - 63)	/* Invalid Broadcast MAC Address. */

/* error code for bridge module */
#define OPL_ERR_BRIDGE_BASE									(OPL_ERROR_CODE_BASE - 9001)	    /* Error No. for bridge */

#define OPL_BRIDGE_VLAN_OUTOF_RANGE					(OPL_ERR_BRIDGE_BASE - 100)
#define OPL_BRIDGE_VLAN_ENTRY_OUTOF_RANGE		(OPL_ERR_BRIDGE_BASE - 101)
#define OPL_BRIDGE_VCC_POINTER_OUTOF_RANGE		(OPL_ERR_BRIDGE_BASE - 102)
#define OPL_BRIDGE_VCC_ENTRY_OUTOF_RANGE		(OPL_ERR_BRIDGE_BASE - 103)
#define OPL_BRIDGE_VCC_ENTRY_TABLE_FULL			(OPL_ERR_BRIDGE_BASE - 104)
#define OPL_BRIDGE_VTT_OUTOF_RANGE						(OPL_ERR_BRIDGE_BASE - 105)
#define OPL_BRIDGE_VTT_RECORD_NOT_FOUND			(OPL_ERR_BRIDGE_BASE - 106)
#define OPL_TABLE_INIT_MEMORY_NULL						(OPL_ERR_BRIDGE_BASE - 107)
#define OPL_BRIDGE_VTT_TABLE_FULL							(OPL_ERR_BRIDGE_BASE - 108)
#define OPL_BRIDGE_VTT_RANGE_FULL							(OPL_ERR_BRIDGE_BASE - 109)
#define OPL_BRIDGE_ETHERMAP_OUTOF_RANGE			(OPL_ERR_BRIDGE_BASE - 110)
#define OPL_BRIDGE_ARL_OUTOF_RANGE						(OPL_ERR_BRIDGE_BASE - 111)
#define OPL_BRIDGE_ARL_NULL_ENTRY							(OPL_ERR_BRIDGE_BASE - 112)
#define OPL_BRIDGE_ARL_HASH_COLLISION				(OPL_ERR_BRIDGE_BASE - 113)
#define OPL_BRIDGE_REVMAC_NUM_OUTOF_RANGE		(OPL_ERR_BRIDGE_BASE - 114)
#define OPL_BRIDGE_PORTID_OUTOF_RANGE				(OPL_ERR_BRIDGE_BASE - 115)
#define OPL_BRIDGE_VLAN_ENTRY_TABLE_FULL			(OPL_ERR_BRIDGE_BASE - 116)
#define OPL_BRIDGE_TPID_OUTOF_RANGE					(OPL_ERR_BRIDGE_BASE - 117)
#define OPL_BRIDGE_ARL_TABLE_FULL							(OPL_ERR_BRIDGE_BASE - 118)	


/*Classify Module*/
#define CLASSIFY_INVALID_PKT_TYPE          					(OPL_ERROR_CODE_BASE - 10001) /*Invalid pakket protocol type*/
#define CLASSIFY_INVALID_PARAMETER_LENGTH  		(OPL_ERROR_CODE_BASE - 10002) /*Para out of valid range*/
#define CLASSIFY_WINDOWS_OUTOF_RANGE       			(OPL_ERROR_CODE_BASE - 10003)
#define CLASSIFY_MASKF_SELECT_NULL		   				(OPL_ERROR_CODE_BASE - 10004)
#define CLASSIFY_RULE_NULL_ITEM			   					(OPL_ERROR_CODE_BASE - 10005)
#define CLASSIFY_FREE_INVALID_MENTRY       				(OPL_ERROR_CODE_BASE - 10006)
#define CLASSIFY_RULE_ITEM_SAME			   					(OPL_ERROR_CODE_BASE - 10007)
#define CLASSIFY_ADDR_TABLE_8COLLISION	   			(OPL_ERROR_CODE_BASE - 10008)
#define CLASSIFY_RULE_TABLE_FULL		   					(OPL_ERROR_CODE_BASE - 10009)
#define CLASSIFY_RULE_TABLE_NOT_EXIST					(OPL_ERROR_CODE_BASE - 10010)
#define CLASSIFY_RULE_TABLE_EXIST							(OPL_ERROR_CODE_BASE - 10011)
#define CLASSIFY_METER_TAB_FULL           				 	(OPL_ERROR_CODE_BASE - 10012)
#define CLASSIFY_COUNTER_EXISTED           					(OPL_ERROR_CODE_BASE - 10013)
#define CLASSIFY_COUNTER_TAB_FULL          				(OPL_ERROR_CODE_BASE - 10014)
#define CLASSIFY_RMK_INVALID_FIELD         				(OPL_ERROR_CODE_BASE - 10015)
#define CLASSIFY_MASK_TABLE_FULL                        (OPL_ERROR_CODE_BASE - 10016)
/* Remark Module */
#define REMARK_PARAMETER_OUTOF_RANGE       			(OPL_ERROR_CODE_BASE - 10016)  
#define REMARK_MAPTYPE_OUTOF_RANGE         			(OPL_ERROR_CODE_BASE - 10017)

#define OPL_ERR_END		             									(OPL_ERROR_CODE_BASE - 100000)	/* Error No end */

/***** STRUCTURES and UNIONS *****/
/*OPL Error Code Table Struct defintion*/
typedef struct OPL_CLT_ERROR_CODE_s 
{
	INT32 errNo;      /*Error Code*/   
   UINT8*  errExp;   /*Error explanation*/
}OPL_CLT_ERROR_CODE_t;

extern void oplerrno (INT32 nErrCode);

#endif /* _OPL_ERRNO_H_ */
