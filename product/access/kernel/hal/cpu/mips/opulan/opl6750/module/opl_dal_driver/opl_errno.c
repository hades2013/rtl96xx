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
* FILENAME: opl_errno.c
*
* DESCRIPTION: Provided implementation of function to operate OPULAN
*	privated defined Error Codes.
*
* Date Created: 24/05/2007
*
* Authors(optional): yzhang
*
* Reviewed by (optional):
*
*
**************************************************************************/

/***** INCLUDE FILES *****/

#include "opl_driver.h"
#include "opl_errno.h"
#include "opl_debug.h"

/***** DEFINES and ENUMS *****/

OPL_CLT_ERROR_CODE_t OplOpconnErrNoTbl[] = {
	{OPL_ERR_START,OPL_NULL},
	{OPL_ERR_INVALID_PARAMETERS,	  	"Invalid parameters passed to the caller, other than Null Pointer."},
	{OPL_ERR_NULL_POINTER,		      			"Null pointer passed to caller."},
	{OPL_ERR_REG_WRITE_FAIL,	   	  			"Failed to write to a chip register."},
	{OPL_ERR_REG_READ_FAIL,		      		"Failed to read from a chip register."},
	{OPL_ERR_TAB_WRITE_FAIL,		    	"Failed to write to a record of a Chip Internal Table."},
	{OPL_ERR_TAB_READ_FAIL,		     		"Failed to read a record of a Chip Internal Table."},
	{OPL_ERR_TAB_WRITE_TIMEOUT,	   	"Failed to write to a record of a Chip Internal Table due to hardware response time out."},
	{OPL_ERR_TAB_READ_TIMEOUT,	    	"Failed to read a record of a Chip Internal Table due to hardware response time out."},
	{OPL_ERR_MEM_ALLOC_FAIL,		    	"Failed to allocate memory from System Main Memory Pool."},
	{OPL_ERR_MEM_FREE_FAIL,		      		"Failed to free memory block. "},
	{OPL_ERR_SEM_CREATE_FAIL,		   		"Failed to create a semaphore."},
	{OPL_ERR_SEM_DELETE_FAIL,		    	"Failed to delete a semaphore."},
	{OPL_ERR_SEM_TAKE_FAIL,		   	  		"Failed to execute Take Operation to a semaphore."},
	{OPL_ERR_SEM_GIVE_FAIL,		      		"Failed to execute Give Operation to a semaphore."},
	{OPL_ERR_TIMER_CREATE_FAIL,	 	  	"Failed to create a RTOS timer."},
	{OPL_ERR_TIMER_DELETE_FAIL,	    	"Failed to delete a RTOS timer."},
	{OPL_ERR_TIMER_START_FAIL,	    	"Failed to start a RTOS timer."},
	{OPL_ERR_TIMER_CANCEL_FAIL,	    	"Failed to stop a RTOS timer."},
	{OPL_ERR_TASK_CREATE_FAIL,	    	"Failed to create a task or thread in the RTOS."},
	{OPL_ERR_TASK_DELETE_FAIL,	    		"Failed to delete a task or thread in the RTOS."},
	{OPL_ERR_MSGQ_CREATE_FAIL,	    	"Failed to create a RTOS message queue."},
	{OPL_ERR_MSGQ_DELETE_FAIL,	   		"Failed to delete a RTOS message queue."},
	{OPL_ERR_MSGQ_SEND_FAIL,			  	"Failed to send a message to the RTOS Message Queue."},
	{OPL_ERR_MSGQ_RECV_FAIL,		 	 	"Failed to receive a message from the RTOS Message Queue."},
	{OPL_ERR_INVALID_MAC,			      		"Invalid MAC Address, such as length error, format error, range error."},
	{OPL_ERR_INVALID_UNI_MAC,		  		"Invalid Unicast MAC Address."},
	{OPL_ERR_INVALID_MC_MAC,		  		"Invalid Multicast MAC Address."},
	{OPL_ERR_INVALID_BC_MAC,		 	  	"Invalid Broadcast MAC Address."},
	{OPL_ERR_END,OPL_NULL}
}; 

/*******************************************************************************
*
* oplstrerror
*
* DESCRIPTION: This function is to get explanation string pointer by error No.
*
* INPUTS:
*		errno	- error No.
*
* OUTPUT:
*   N/A.
*
* RETURNS:
*   Error code explanation string pointer.
*
* SEE ALSO:
*/
INT8 *oplstrerror(INT32 errno)
{
  /*binsearch algorithm*/
  int low, high, mid;
  
  if ((OPL_ERR_START <= errno)||(OPL_ERR_END >= errno))
  	return OPL_NULL;

  low = 1;
  high = sizeof(OplOpconnErrNoTbl)/sizeof(OPL_CLT_ERROR_CODE_t);
  
  while (low <= high)
  {
    mid = (low + high)/2;
    if (OplOpconnErrNoTbl[mid].errNo == errno)
    { 
       return OplOpconnErrNoTbl[mid].errExp;
    }
    if (OplOpconnErrNoTbl[mid].errNo < errno)
      high = mid - 1;
    else 
      low = mid + 1;
  }
  return OPL_NULL;
}
/*******************************************************************************
*
* oplerrno
*
* DESCRIPTION: This function is used in print out the error No. and the explanation.
*
* INPUTS:
*		nErrCode	- error code.
*
* OUTPUT:
*   N/A.
*
* RETURNS:
*   N/A.
*
* SEE ALSO:
*/
void oplerrno (INT32 nErrCode)
{
	OPL_DRV_PRINTF(("[%d]: %s", nErrCode, oplstrerror(nErrCode)));
}

/* END FILE */
