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
**    FILE       :  rstp_bridect.c
**      
**    DESCRIPTION:  This file contains Bridge Detection state machine:802.1D 2004 Edition 
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
#include "rstp_base.h"
#include "rstp_stpm.h"
#define STATES {        \
	CHOOSE(EDGE),       \
	CHOOSE(NOT_EDGE),   \
}

#define GET_STATE_NAME STP_bridect_get_state_name
#include "rstp_choose.h"

void STP_bridect_enter_state (STATE_MACH_T *this)
{
  register PORT_T *port = this->owner.port;

  switch (this->State) {
  	//case BEGIN:
    case EDGE:

#ifdef __OPSP_DEBUG__
      OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "STP_bridect_enter_state EDGE\n");
#endif

      port->operEdge = True;
      break;
    case NOT_EDGE:

#ifdef __OPSP_DEBUG__
      OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "STP_bridect_enter_state NOT_EDGE\n");
#endif

      port->operEdge = False; 
      break;
   }
}

Bool
STP_bridect_check_conditions (STATE_MACH_T *this)
{
  register PORT_T *port = this->owner.port;

  if (port->adminEdge && (BEGIN == this->State)) 
   	return STP_hop_2_state (this, EDGE);
  if (!port->adminEdge && (BEGIN == this->State)) 
   	return STP_hop_2_state (this, NOT_EDGE);

  switch (this->State) {
     case EDGE:
        if((!port->portEnabled && !port->adminEdge) || !port->operEdge)
           return STP_hop_2_state (this, NOT_EDGE);	
        break;
     case NOT_EDGE:
        if((!port->portEnabled && port->adminEdge) ||((port->edgeDelayWhile == 0) && port->autoEdge && port->sendRSTP && port->proposing))
           return STP_hop_2_state (this, EDGE);
        break;
  }
  return False;
}
