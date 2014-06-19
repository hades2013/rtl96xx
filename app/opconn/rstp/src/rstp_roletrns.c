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
**    FILE       :  rstp_role_trns.c
**      
**    DESCRIPTION:  This file contains Port Role Transitions state machine. 
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

/* Port Role Transitions state machine : 802.1D 2004 Edition*/
#include "rstp_base.h"
#include "rstp_stpm.h"

#define STATES { \
	CHOOSE(INIT_PORT),      \
	CHOOSE(DISABLE_PORT),      \
	CHOOSE(DISABLED_PORT),      \
	  CHOOSE(ROOT_PROPOSED),      \
	CHOOSE(ROOT_AGREED),      \
	CHOOSE(REROOT),      \
	CHOOSE(ROOT_PORT),      \
	CHOOSE(ROOT_FORWARD),      \
	CHOOSE(ROOT_LEARN),      \
	CHOOSE(REROOTED),      \
      CHOOSE(DESIGNATED_PROPOSE),  \
    CHOOSE(DESIGNATED_SYNCED),   \
    CHOOSE(DESIGNATED_RETIRED),  \
    CHOOSE(DESIGNATED_PORT), \
    CHOOSE(DESIGNATED_DISCARD),   \
    CHOOSE(DESIGNATED_LEARN),    \
    CHOOSE(DESIGNATED_FORWARD),  \
	  CHOOSE(ALTERNATE_PROPOSED), \
	CHOOSE(ALTERNATE_AGREED),      \
	CHOOSE(ALTERNATE_PORT),      \
	CHOOSE(BLOCK_PORT),      \
	CHOOSE(BACKUP_PORT),      \
}

#define GET_STATE_NAME STP_roletrns_get_state_name
#include "rstp_choose.h"

static void
setSyncTree (STATE_MACH_T *this)
{
  register PORT_T* port;

  for (port = this->owner.port->owner->ports; port; port = port->next) {
    port->sync = True; /* in ROOT_PROPOSED (setSyncTree) */
  }
}

static void setReRootTree (STATE_MACH_T *this){
	register PORT_T* port;
	for (port = this->owner.port->owner->ports; port; port = port->next) {
    port->reRoot = True; 
  }
}

static Bool
compute_all_synced (PORT_T* this)
{
  register PORT_T* port;

  for (port = this->owner->ports; port; port = port->next) {
    if (port->port_index == this->port_index) continue;
    if (! port->synced) {
        return False;
    }
  }

  return True;
}

static Bool
compute_re_rooted (PORT_T* this)
{
  register PORT_T* port;

  for (port = this->owner->ports; port; port = port->next) {
    if (port->port_index == this->port_index) continue;
    if (port->rrWhile) {
      return False;
    }
  }
  return True;
}

void
STP_roletrns_enter_state (STATE_MACH_T* this)
{
  register PORT_T*           port = this->owner.port;
  register STPM_T*           stpm;
  register int               EdgeDelay;
  stpm = port->owner;
  
  switch (this->State) {
    case BEGIN:
    case INIT_PORT:
    	 port->role = DisabledPort;
		 /* begin added by jiangmingli for rstp non-stp port */
		 #if 1
    	 port->learn =
         port->forward = False;
		 #endif
		 /* end added by jiangmingli for rstp non-stp port */
         port->synced = False; /* in INIT */
         port->sync = 
         port->reRoot = True; /* in INIT_PORT */
         
         port->rrWhile = stpm->rootTimes.ForwardDelay;
         port->fdWhile = stpm->rootTimes.MaxAge;
         port->rbWhile = 0;        
      	 break;
    case DISABLE_PORT:
         port->role = port->selectedRole;
         port->learn =
      	 port->forward = False; 
         break; 
    case DISABLED_PORT:
    	 port->fdWhile = stpm->rootTimes.MaxAge;
    	 port->synced  = True; /* in INIT */
    	 port->rrWhile = 0;
    	 port->sync = 
         port->reRoot = False; /* in INIT_PORT */
    	 break;
    case ROOT_PROPOSED:
		 setSyncTree(this);
		 port->proposed = False;
         break;
    case ROOT_AGREED:
    	 port->proposed = 
    	 port->sync     = False;
         port->agree    = True;
         port->newInfo = True;
         break;
    case REROOT:
    	 setReRootTree(this);
         break;
    case ROOT_PORT:
    	 port->role = RootPort;
    	 port->rrWhile = stpm->rootTimes.ForwardDelay;
         break;
    case ROOT_FORWARD:
    	 port->fdWhile = 0;
    	 port->forward = True;
         break;
    case ROOT_LEARN:
    	 //port->fdWhile = stpm->rootTimes.ForwardDelay;
    	 if(port->sendRSTP)
         	port->fdWhile = stpm->rootTimes.HelloTime;
         else
         	port->fdWhile = stpm->rootTimes.ForwardDelay;
    	 port->learn = True;
    	 break;     
    case REROOTED:
    	 port->reRoot = False;
         break;
    case DESIGNATED_PROPOSE:
         port->proposing = True; /* in DESIGNATED_PROPOSE */
         //Returns the value of MigrateTime if operPointToPointMAC is TRUE, and the value of MaxAge otherwise.
         if(port->operPointToPointMac) 
         	EdgeDelay = MigrateTime;
         else
         	EdgeDelay = stpm->rootTimes.MaxAge;
         port->edgeDelayWhile = EdgeDelay;
         port->newInfo = True;
    	 break;
    case DESIGNATED_SYNCED:
         port->rrWhile = 0;
         port->synced  = True;
 	     port->sync    = False;
    	 break;
    case DESIGNATED_RETIRED:
    	 port->reRoot    = False;
    	 break;
    case DESIGNATED_PORT:
    	 port->role = DesignatedPort;
    	 break;
    case DESIGNATED_DISCARD:
    	 port->learn    =
    	 port->forward  =
    	 port->disputed = False;
    	 
        if(port->sendRSTP)
         	port->fdWhile = stpm->rootTimes.HelloTime;
         else
         	port->fdWhile = stpm->rootTimes.ForwardDelay;

    	 break;
    case DESIGNATED_LEARN:
    	 port->learn = True;

         if(port->sendRSTP)
         	port->fdWhile = stpm->rootTimes.HelloTime;
         else
         	port->fdWhile = stpm->rootTimes.ForwardDelay;
    	 break;
   case DESIGNATED_FORWARD:
		 port->forward = True;
         port->fdWhile = 0;
    	 port->agreed = port->sendRSTP;
    	 break;    		  
   case ALTERNATE_PROPOSED:
         setSyncTree(this);
         port->proposed = False;
         break;
   case ALTERNATE_AGREED:
         port->proposed = False;
         port->agree    = True;
         port->newInfo  = True;
         break;
   case ALTERNATE_PORT:
		 port->fdWhile =  stpm->rootTimes.ForwardDelay;
		 port->synced  = True;	
         port->rrWhile = 0;
         port->sync    =
         port->reRoot  = False;
         break;
   case BLOCK_PORT:
   	     port->role    = port->selectedRole;
   	     port->learn   =
   	     port->forward = False;
   	     break;
   case BACKUP_PORT:
         port->rbWhile = 2*stpm->rootTimes.HelloTime;
         break;
  };
}
    
Bool
STP_roletrns_check_conditions (STATE_MACH_T* this)
{
  register PORT_T           *port = this->owner.port;
  register STPM_T           *stpm;
  Bool                      allSynced;
  Bool                      allReRooted;
  Bool                      reRooted;
  Bool rstpVersion;
  
  stpm = port->owner;

	/* begin added by jiangmingli for rstp non-stp port */
	if (NonStpPort == port->selectedRole)
	{
		port->role = NonStpPort;
		port->learn = True;
		port->forward = True;
	}
	/* end added by jiangmingli for rstp non-stp port */
  
  //printf("STP_roletrns_check_conditions : port Id =%d \n", port->port_index);
  
  if (BEGIN == this->State) {
    return STP_hop_2_state (this, INIT_PORT);
  }
  //rzhou debug 
  if (port->role != port->selectedRole && port->selected && ! port->updtInfo) {
    switch (port->selectedRole) {
      case DisabledPort:
      	return STP_hop_2_state (this, DISABLE_PORT);
      case AlternatePort:
      case BackupPort:
#if 0 /* def STP_DBG */
        if (this->debug) {
          stp_trace ("hop to BLOCK_PORT role=%d selectedRole=%d",
                                (int) port->role, (int) port->selectedRole);
        }
#endif
        return STP_hop_2_state (this, BLOCK_PORT);
      case RootPort:
        return STP_hop_2_state (this, ROOT_PORT);
      case DesignatedPort:
        return STP_hop_2_state (this, DESIGNATED_PORT);
      default:
        return False;
    }
  }
  switch (this->State) {
    /* 17.23.1 */
    case INIT_PORT:

#ifdef __OPSP_DEBUG__
    	 OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "STP_roletrns_check_conditions : INIT_PORT \n");
#endif

        return STP_hop_2_state (this, DISABLE_PORT);
	case DISABLE_PORT:
        if (!port->selected || port->updtInfo) break;
        if(!port->learning && !port->forwarding)
          	return STP_hop_2_state (this, DISABLED_PORT);
        break; 
    case DISABLED_PORT:
    	if (!port->selected || port->updtInfo) break;
        if((port->fdWhile != stpm->rootTimes.MaxAge)||port->sync || port->reRoot || !port->synced)
           	return STP_hop_2_state (this, DISABLED_PORT);
        break;
    case ROOT_PROPOSED:
	     return STP_hop_2_state (this, ROOT_PORT);
	case ROOT_AGREED:
         return STP_hop_2_state (this, ROOT_PORT);
       	 break;
   	case REROOT:
		 return STP_hop_2_state (this, ROOT_PORT);	
	case ROOT_PORT:
		 if (!port->selected || port->updtInfo) break;
         if(port->rrWhile != stpm->rootTimes.ForwardDelay)
		 	return STP_hop_2_state (this, ROOT_PORT);	
		 if(port->reRoot && port->forward)
		 	return STP_hop_2_state (this, REROOTED);		
       	 //TRUE if the rrWhile timer is clear (zero) for all Ports for the given Tree other than the given Port.
       	 reRooted = compute_re_rooted(port);
       	
       	if(stpm->ForceVersion >= 2)
    		rstpVersion = True;
    	else
    		rstpVersion = False;
    			
    	if(((port->fdWhile == 0) || (reRooted && (port->rbWhile == 0)) && rstpVersion) && !port->learn)
    	 	return STP_hop_2_state (this, ROOT_LEARN);
       	if(((port->fdWhile == 0) || (reRooted && (port->rbWhile == 0)) && rstpVersion) && port->learn && !port->forward)
       	 	return STP_hop_2_state (this, ROOT_FORWARD);
       	if(!port->forward && !port->reRoot)
       	 	return STP_hop_2_state (this, REROOT);
       	allSynced = compute_all_synced(port);
       	
       	if((allSynced && !port->agree) ||(port->proposed && port->agree))
       	 	return STP_hop_2_state (this, ROOT_AGREED);
       	if(port->proposed && !port->agree)
       	 	return STP_hop_2_state (this, ROOT_PROPOSED);
	       	break;
	case ROOT_FORWARD:
         return STP_hop_2_state (this, ROOT_PORT);	 
	case ROOT_LEARN:
		 return STP_hop_2_state (this, ROOT_PORT);	
	case REROOTED:
		 return STP_hop_2_state (this, ROOT_PORT);	
    case DESIGNATED_PROPOSE:
		 return STP_hop_2_state (this, DESIGNATED_PORT);
    case DESIGNATED_SYNCED:
		 return STP_hop_2_state (this, DESIGNATED_PORT);
    case DESIGNATED_RETIRED:
		 return STP_hop_2_state (this, DESIGNATED_PORT);
   		 break;
   	case DESIGNATED_PORT:
		 if (!port->selected || port->updtInfo) break;
		 if((port->rrWhile == 0)&& port->reRoot)
		    return STP_hop_2_state (this, DESIGNATED_RETIRED);
		 if((!port->learning && !port->forwarding && !port->synced) ||(port->agreed && !port->synced) ||(port->operEdge && !port->synced) ||(port->sync && port->synced))   
   		 	return STP_hop_2_state (this, DESIGNATED_SYNCED);
   		 if(!port->forward && !port->agreed &&!port->proposing && !port->operEdge)
   		 	return STP_hop_2_state (this, DESIGNATED_PROPOSE);
   		 if(((port->sync && !port->synced) || (port->reRoot && (port->rrWhile != 0)) || port->disputed) && !port->operEdge && (port->learn || port->forward))
   		 	return STP_hop_2_state (this, DESIGNATED_DISCARD);
   		 if(((port->fdWhile == 0) || port->agreed || port->operEdge) && ((port->rrWhile ==0) || !port->reRoot) && !port->sync && !port->learn)
   		 	return STP_hop_2_state (this, DESIGNATED_LEARN);
   		 if(((port->fdWhile == 0) || port->agreed || port->operEdge) && ((port->rrWhile ==0) || !port->reRoot) && !port->sync && (port->learn && !port->forward))
   		 	return STP_hop_2_state (this, DESIGNATED_FORWARD);					
   	 	 break;
	case DESIGNATED_DISCARD:
         return STP_hop_2_state (this, DESIGNATED_PORT);
    case DESIGNATED_LEARN:
		 return STP_hop_2_state (this, DESIGNATED_PORT);
	case DESIGNATED_FORWARD:
		 return STP_hop_2_state (this, DESIGNATED_PORT);
    case ALTERNATE_PROPOSED:
       	 return STP_hop_2_state (this, ALTERNATE_PORT);
    case ALTERNATE_AGREED:
       	 return STP_hop_2_state (this, ALTERNATE_PORT);
    case ALTERNATE_PORT:
         if (!port->selected || port->updtInfo) break;
         allSynced = compute_all_synced(port);
         if((allSynced && !port->agree)||(port->proposed && port->agree))
          	 return STP_hop_2_state (this, ALTERNATE_AGREED);
         if(port->proposed && !port->agree)
           	 return STP_hop_2_state (this, ALTERNATE_PROPOSED);
         if((port->fdWhile != stpm->rootTimes.ForwardDelay) || port->sync || port->reRoot || !port->synced)
          	 return STP_hop_2_state (this, ALTERNATE_PORT);
         if((port->rbWhile != (unsigned int)2*stpm->rootTimes.HelloTime) &&(port->role == BackupPort))	 	 	 
           	 return STP_hop_2_state (this, BACKUP_PORT);
         break;
	case BLOCK_PORT:
		 if (!port->selected || port->updtInfo) break;
		 if(!port->learning && !port->forwarding)
		 	 return STP_hop_2_state (this, ALTERNATE_PORT);
		 break;
	case BACKUP_PORT:
		 return STP_hop_2_state (this, ALTERNATE_PORT);
  };
  return False;
}

