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
**    FILE       :  rstp_stpm.c
**      
**    DESCRIPTION:  This file contains STP machine instance : bridge per VLAN. 
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
#include "rstp_to.h" /* for STP_OUT_flush_lt */

static STPM_T *bridges = NULL;

static int
_stp_stpm_init_machine (STATE_MACH_T* this)
{
  this->State = BEGIN;
  (*(this->concreteEnterState)) (this);
  return 0;
}

static int
_stp_stpm_iterate_machines (STPM_T* this,
                           int (*iter_callb) (STATE_MACH_T*),
                           Bool exit_on_non_zero_ret)
{
  register STATE_MACH_T* stater;
  register PORT_T*       port;
  int                    iret, mret = 0;

  /* state machines per bridge */
  for (stater = this->machines; stater; stater = stater->next) {
    iret = (*iter_callb) (stater);
    if (exit_on_non_zero_ret && iret)
      return iret;
    else
      mret += iret;
  }

  /* state machines per port */
  for (port = this->ports; port; port = port->next) {
    for (stater = port->machines; stater; stater = stater->next) {
      iret = (*iter_callb) (stater);
      if (exit_on_non_zero_ret && iret)
        return iret;
      else
        mret += iret;
    }
  }
  
  return mret;
}

void
_stp_stpm_init_data (STPM_T* this)
{
  STP_VECT_create (&this->rootPrio,
                   &this->BrId,
                   0,
                   &this->BrId,
                   0, 0);

  this->BrTimes.MessageAge = 0;

  STP_copy_times (&this->rootTimes, &this->BrTimes);
}

static unsigned char
_check_topoch (STPM_T* this)
{
  register PORT_T*  port;
  
  for (port = this->ports; port; port = port->next) {
    if (port->tcWhile) {
      return 1;
    }
  }
  return 0;
}

void
STP_stpm_one_second (STPM_T* param)
{
  STPM_T*           this = (STPM_T*) param;
  register PORT_T*  port;
  register int      iii;
  int linkstatus;

  if (STP_ENABLED != this->admin_state) return;

  for (port = this->ports; port; port = port->next) {
    for (iii = 0; iii < TIMERS_NUMBER; iii++) {
      if (*(port->timers[iii]) > 0) {
        (*port->timers[iii])--;
      }
    }    
    port->uptime++;
    linkstatus = STP_OUT_get_port_link_status(port->port_index);
    if (linkstatus != port->portEnabled)
    {
      if (linkstatus)
  	  {
		STP_IN_enable_port (port->port_index, True);
  	  }
	  else
	  {
	    STP_IN_enable_port (port->port_index, False);
	  }
    }
  }

  STP_stpm_update (this);
  this->Topo_Change = _check_topoch (this);
  if (this->Topo_Change) {
    this->Topo_Change_Count++;
    this->timeSince_Topo_Change = 0;
  } else {
    this->Topo_Change_Count = 0;
    this->timeSince_Topo_Change++;
  }
}

STPM_T*
STP_stpm_create (int vlan_id, char* name)
{
  STPM_T* this;

  STP_NEW_IN_LIST(this, STPM_T, bridges, "stp instance");

  this->admin_state = STP_DISABLED;
  
  this->vlan_id = vlan_id;
  if (name) {
    STP_STRDUP(this->name, name, "stp bridge name");
  }

  this->machines = NULL;
  this->ports = NULL;

	/* begin modified by jiangmingli for RSTP, 2008-10-24 */
	/* initialize bridge basic parameter */
	this->BrTimes.HelloTime = uid_cfg.hello_time;
	this->BrTimes.ForwardDelay = uid_cfg.forward_delay;
	this->BrTimes.MaxAge = uid_cfg.max_age;
	this->ForceVersion = uid_cfg.force_version;
	this->BrId.prio = uid_cfg.bridge_priority;
	vosHWAddrGet("eth0", this->BrId.addr);
	/* end modified by jiangmingli for RSTP, 2008-10-24 */

  STP_STATE_MACH_IN_LIST(rolesel);

#ifdef STP_DBG
  /* this->rolesel->debug = 2;  */
#endif

  return this;
}

int
STP_stpm_enable (STPM_T* this, UID_STP_MODE_T admin_state)
{
  int rc = 0;

  if (admin_state == this->admin_state) {
    /* nothing to do :) */
    return 0;
  }

  if (STP_ENABLED == admin_state) {
    rc = STP_stpm_start (this);
    this->admin_state = admin_state;
  } else {
    this->admin_state = admin_state;
    STP_stpm_stop (this);
  }
  
  return rc;
}

void
STP_stpm_delete (STPM_T* this)
{
  register STPM_T*       tmp;
  register STPM_T*       prev;
  register STATE_MACH_T* stater;
  register PORT_T*       port;
  register void*         pv;

  STP_stpm_enable (this, STP_DISABLED);
  
  for (stater = this->machines; stater; ) {
    pv = (void*) stater->next;
    STP_state_mach_delete (stater);
    this->machines = stater = (STATE_MACH_T*) pv;
  }

  for (port = this->ports; port; ) {
    pv = (void*) port->next;
    STP_port_delete (port);
    this->ports = port = (PORT_T*) pv;
  }

  prev = NULL;
  for (tmp = bridges; tmp; tmp = tmp->next) {
    if (tmp->vlan_id == this->vlan_id) {
      if (prev) {
        prev->next = this->next;
      } else {
        bridges = this->next;
      }
      
      if (this->name)
        STP_FREE(this->name, "stp bridge name");
      STP_FREE(this, "stp instance");
      break;
    }
    prev = tmp;
  }
}

int
STP_stpm_start (STPM_T* this)
{
  register PORT_T* port;

  if (! this->ports) { /* there are not any ports :( */
    return STP_There_Are_No_Ports;
  }

  if (! STP_compute_bridge_id (this)) {/* can't compute bridge id ? :( */
    return STP_Cannot_Compute_Bridge_Prio;
  }

  /* check, that the stpm has unique bridge Id */
  if (0 != STP_stpm_check_bridge_priority (this)) {
    /* there is an enabled bridge with same ID :( */
    return STP_Invalid_Bridge_Priority;
  }

  /* initial bridge and ports parameter, I am root bridge */
  _stp_stpm_init_data (this);

  for (port = this->ports; port; port = port->next) {
    STP_port_init (port, this, True);
  }

#ifndef STRONGLY_SPEC_802_1W
  /* A. see comment near STRONGLY_SPEC_802_1W in topoch.c */
  /* B. port=0 here means: delete for all ports */
#ifdef STP_DBG
  stp_trace("%s (all, start stpm)",
        "clearFDB");
#endif

  STP_OUT_flush_lt (0, this->vlan_id, LT_FLASH_ONLY_THE_PORT, "start stpm");
#endif

#ifdef __OPSP_DEBUG__
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "STP_stpm_update called 00\n");
#endif

  _stp_stpm_iterate_machines (this, _stp_stpm_init_machine, False);

#ifdef __OPSP_DEBUG__
  OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "STP_stpm_update called 01\n");
#endif

  /* update rstp state */
  STP_stpm_update (this);

#ifdef __OPSP_DEBUG__
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "STP_stpm_update called 02\n");
#endif

  return 0;
}

void
STP_stpm_stop (STPM_T* this)
{
}

int
STP_stpm_update (STPM_T* this) /* returns number of loops */
{
  register Bool     need_state_change;
  register int      number_of_loops = 0;

  need_state_change = False; 
  
  for (;;) {/* loop until not need changes */
    need_state_change = _stp_stpm_iterate_machines (this,
                                                   STP_check_condition,
                                                   True);
    if (! need_state_change) return number_of_loops;

    number_of_loops++;
    /* here we know, that at least one stater must be
       updated (it has changed state) */
    number_of_loops += _stp_stpm_iterate_machines (this,
                                                  STP_change_state,
                                                  False);

  }

  return number_of_loops;
}

BRIDGE_ID *
STP_compute_bridge_id (STPM_T* this)
{
  register PORT_T* port;
  register PORT_T* min_num_port = NULL;
  int              port_index = 0;

  for (port = this->ports; port; port = port->next) {
    if (! port_index || port->port_index < port_index) {
      min_num_port = port;
      port_index = port->port_index;
    }
  }

  if (! min_num_port) return NULL; /* IMHO, it may not be */

  STP_OUT_get_port_mac (min_num_port->port_index, this->BrId.addr);

  return &this->BrId;
}

STPM_T*
STP_stpm_get_the_list (void)
{
  return bridges;
}

void
STP_stpm_update_after_bridge_management (STPM_T* this)
{
  register PORT_T* port;

  for (port = this->ports; port; port = port->next) {
    //rzhou change the state of port, but?? 
    port->reselect = True;
    port->selected = False;
  }
}

int
STP_stpm_check_bridge_priority (STPM_T* this)
{
  register STPM_T* oth;

  for (oth = bridges; oth; oth = oth->next) {
    if (STP_ENABLED == oth->admin_state && oth != this &&
        ! STP_VECT_compare_bridge_id (&this->BrId, &oth->BrId)) {
      return STP_Invalid_Bridge_Priority;
    }
  }

  return 0;
}

const char*
STP_stpm_get_port_name_by_id (STPM_T* this, PORT_ID port_id)
{
  register PORT_T* port;

  for (port = this->ports; port; port = port->next) {
    if (port_id == port->port_id) {
        return port->port_name;
    }
  }

  return "Undef?";
}





