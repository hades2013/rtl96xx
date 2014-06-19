/************************************************************************ 
 * RSTP library - Rapid Spanning Tree (802.1t, 802.1w) 
 * Copyright (C) 2001-2003 Optical Access 
 * Author: Alex Rozin 
 * 
 * This file is part of RSTP library. 
 * 
 * RSTP library is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License as published by the 
 * Free Software Foundation; version 2.1 
 * 
 * RSTP library is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser 
 * General Public License for more details. 
 * 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with RSTP library; see the file COPYING.  If not, write to the Free 
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
 * 02111-1307, USA. 
 **********************************************************************/

/*******************************************************************************
**    MODULE     :  RSTP
**
**    FILE       :  rstp_portrcv.c
**      
**    DESCRIPTION:  This file contains Port Receive state machine 
**    
**
**    NOTES      :  None.
**
*******************************************************************************/
/* 
**    MODIFICATION HISTORY:
**
**    Rev #    Date         Author         Description
**    -----    ----------   ------------   ------------------------------------
**    0.1  	   1/10/2007    rzhou         Initial                      
**
*******************************************************************************/

/* Port Receive state machine                                                */
/*rzhou add notes:
Change to 802.1D 2004 Edition
*/
#include "rstp_base.h"
#include "rstp_stpm.h"

#define STATES {      \
  CHOOSE(DISCARD),    \
  CHOOSE(RECEIVE),    \
}

#define GET_STATE_NAME STP_portrcv_get_state_name
#include "rstp_choose.h"

void updtBPDUVersion(STATE_MACH_T * this){
	register PORT_T *port = this->owner.port;
	/*
	Sets rcvdSTP TRUE if the BPDU received is a version 0 or version 1 TCN or a Config BPDU. Sets
rcvdRSTP TRUE if the received BPDU is an RST BPDU.
	*/
    if (BPDU_TOPO_CHANGE_TYPE == port->msgBpduType) 
        port->rcvdSTP = True;

	if(port->msgBpduVersion < 2)
		port->rcvdSTP = True;

    if (BPDU_RSTP == port->msgBpduType) 
		port->rcvdRSTP = True;
}
void
STP_portrcv_enter_state (STATE_MACH_T *this)
{
  register PORT_T *port = this->owner.port;
   
  switch (this->State) {
    //case BEGIN:
    //  break;
    case DISCARD:
      //printf("STP_portrcv_enter_state DISCARD\n");
      port->rcvdBpdu = 
      port->rcvdRSTP = 
      port->rcvdSTP  = False;
	  
	  port->rcvdMsg = False;
      port->edgeDelayWhile = MigrateTime;
      break;
    case RECEIVE:
    //printf("STP_portrcv_enter_state RECEIVE\n");	
      updtBPDUVersion(this);
	  port->operEdge = 
	  port->rcvdBpdu = False; 
	  port->rcvdMsg  = True;
	  port->edgeDelayWhile = MigrateTime;
	  break;
  }
}

Bool
STP_portrcv_check_conditions (STATE_MACH_T *this)
{
  register PORT_T *port = this->owner.port;

  if (((port->rcvdBpdu || (port->edgeDelayWhile != MigrateTime)) && !port->portEnabled) || BEGIN == this->State) {
    return STP_hop_2_state (this, DISCARD);
  }
  switch (this->State) {
    case DISCARD:
    	if(port->rcvdBpdu && port->portEnabled)
    		return STP_hop_2_state (this, RECEIVE);
         break;
    case RECEIVE:	
        if(port->rcvdBpdu && port->portEnabled && !port->rcvdMsg)
        	 return STP_hop_2_state (this, RECEIVE);
    break; 
  }
  return False;
}
