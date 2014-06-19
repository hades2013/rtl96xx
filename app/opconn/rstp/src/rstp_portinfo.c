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
**    FILE       :  rstp_port_info.c
**      
**    DESCRIPTION:  This file contains Port Information State Machine. 
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

/* Port Information State Machine : 802.1D 2004 Edition*/

#include "rstp_base.h"
#include "rstp_stpm.h"
#include "rstp_in.h"

#define STATES { \
	CHOOSE(RSTP_PORT_DISABLED), \
	CHOOSE(AGED),     \
  	CHOOSE(UPDATE),   \
  	CHOOSE(CURRENT),  \
  	CHOOSE(RECEIVE),  \
  	CHOOSE(SUPERIOR_DESIGNATED),  \
    CHOOSE(REPEATED_DESIGNATED),   \
    CHOOSE(INFERIOR_DESIGNATED),   \
    CHOOSE(NOT_DESIGNATED),   \
    CHOOSE(OTHER),   \
}

#define GET_STATE_NAME STP_info_get_state_name
#include "rstp_choose.h"

static RCVD_MSG_T rcvBpdu (STATE_MACH_T* this);

#if 1 /* for debug */
void
_stp_dump (char* title, unsigned char* buff, int len)
{
  register int iii;

  printf ("\n%s:", title);
  for (iii = 0; iii < len; iii++) {
    if (! (iii % 24)) Print ("\n%6d:", iii);
    if (! (iii % 8)) Print (" ");
    Print ("%02lx", (unsigned long) buff[iii]);
  }
  Print ("\n");
}
#endif

/*rzhou add here*/
Bool betterorsameInfo(STATE_MACH_T* this, INFO_IS_T newInfoIs){
    
    PORT_T* port = this->owner.port;
    int   bridcmp;
    
    bridcmp = STP_VECT_compare_vector(&port->msgPrio, &port->portPrio);
    if((Received == newInfoIs)&&(Received == port->infoIs)&&(bridcmp <= 0))    
		return True;
 
    bridcmp = STP_VECT_compare_vector(&port->designPrio, &port->portPrio);
 	if((Mine == newInfoIs)&&(Mine == port->infoIs)&&(bridcmp <= 0))    
		return True;
 
 	return False;
}

PORT_INFO_T rcvInfo(STATE_MACH_T* this){
	
	PORT_T* port = this->owner.port;
    int   bridcmp;
	int timecmp;
    int portid;
    
    port->rcvdMsg = rcvBpdu (this);
    bridcmp = STP_VECT_compare_vector(&port->msgPrio, &port->portPrio);
	timecmp = STP_compare_times (&port->msgTimes, &port->portTimes);
    /* begin added by jiangmingli for tw_bug 2066 */
    portid = (UINT8)port->port_id;

	if ((0 != bOnePortLoopDetectEn[portid-1]) && (0 != abPortLoop[portid-1]))
    {
        if ((0 == bridcmp) && (port->msgPrio.design_port == port->port_id))
        {   
	        odmPhyAdminStateSet((UINT8)port->port_id, 0);
            bOnePortLooptimer[portid-1] = bOnePortLoopWhile[portid-1];

			rstpwarningstate[portid - 1] = 1;
        }
    }
    /* end added by jiangmingli for tw_bug 2066 */
    
    // rzhou ,how to represent:The received message conveys a Designated Port Role,
    if((RSTP_PORT_ROLE_DESGN == port->msgPortRole )&&((0 > bridcmp)||((0 == bridcmp)&&(0 != timecmp))))
	{
		opl_rstp_printf(("Superior Msg: bridcmp=%d, timecmp=%d.\n", (int)bridcmp, (int)timecmp));
		if (0 != bridcmp)
		{
			opl_rstp_printf(("Msg--Port:\n root_brg_pri:%d -- %d\n, cost:%d -- %d\n, des_brg_pri:%d -- %d\n, des_port:%d -- %d\n",
							(int)port->msgPrio.root_bridge.prio, (int)port->portPrio.root_bridge.prio,
							(int)port->msgPrio.root_path_cost, (int)port->portPrio.root_path_cost,
							(int)port->msgPrio.design_bridge.prio, (int)port->portPrio.design_bridge.prio,
							(int)port->msgPrio.design_port, (int)port->portPrio.design_port));
		}
		else
		{
			opl_rstp_printf(("Msg--Port:\n msg_age:%d -- %d\n, max_age:%d -- %d\n, hello:%d -- %d\n, fwddelay:%d -- %d\n",
							(int)port->msgTimes.MessageAge, (int)port->portTimes.MessageAge,
							(int)port->msgTimes.MaxAge, (int)port->portTimes.MaxAge,
							(int)port->msgTimes.HelloTime, (int)port->portTimes.HelloTime,
							(int)port->msgTimes.ForwardDelay, (int)port->portTimes.ForwardDelay));
		}
		return SuperiorDesignatedInfo;
	}
	else if((RSTP_PORT_ROLE_DESGN == port->msgPortRole)&&(!bridcmp)&&(0 == timecmp))
	{
		opl_rstp_printf(("Repeated Msg.\n"));
		return RepeatedDesignatedInfo;
	}
	else if((RSTP_PORT_ROLE_DESGN == port->msgPortRole)&&(0 < bridcmp)) 
	{
		opl_rstp_printf(("Inferior Des Msg: bridcmp=%d.\n", (int)bridcmp));
		return InferiorDesignatedInfo;
	}
	else if((RSTP_PORT_ROLE_ROOT == port->msgPortRole)&&( 0 >= bridcmp))
	{
		opl_rstp_printf(("Inferior Root Msg: bridcmp=%d.\n", (int)bridcmp));
		return InferiorRootAlternateInfo;
	}
	else
	{
		opl_rstp_printf(("Other Msg.\n"));
		return OtherInfo;
	}
}

void recordProposal(STATE_MACH_T* this){
	
	PORT_T* port = this->owner.port;
    
    if((RSTP_PORT_ROLE_DESGN == port->msgPortRole)&&(True == port->proposing))
    	port->proposed  = True;
}

void recordPriority(STATE_MACH_T* this){

	PORT_T* port = this->owner.port;
	STP_VECT_copy(&port->portPrio,&port->msgPrio);
}

void recordTimes(STATE_MACH_T* this){
	
	PORT_T* port = this->owner.port;
//rzhou note	
#define HELLOTIME_COMP_MIN_VAL 1
#define HELLOTIME_COMP_MAX_VAL 2
	
	STP_copy_times (&port->portTimes, &port->msgTimes);
	
	if(HELLOTIME_COMP_MIN_VAL > port->msgTimes.HelloTime)
		port->portTimes.HelloTime = HELLOTIME_COMP_MIN_VAL;
}
void recordDispute(STATE_MACH_T* this){
	
	PORT_T* port = this->owner.port;	
	if((True == port->rcvdRSTP)&&(True == port->learning)){
		 port->agreed    = True;
		 port->proposing = False;
	}
}

void recordAgreement(STATE_MACH_T* this){

	PORT_T* port = this->owner.port;
    STPM_T* stpm;   
    Bool rstpVersion;
    stpm = port->owner;
    
    if(stpm->ForceVersion >= NORMAL_RSTP)
    	rstpVersion = True;
    else
    	rstpVersion = False; 
	if(rstpVersion && port->operPointToPointMac&&(AGREEMENT_BIT & port->msgFlags)){
    	port->agreed    = True;
        port->proposing = False;
	}else
		port->agreed    = False;
}

#if 0 /* for debug */
void
_stp_dump (char* title, unsigned char* buff, int len)
{
  register int iii;

  printf ("\n%s:", title);
  for (iii = 0; iii < len; iii++) {
    if (! (iii % 24)) Print ("\n%6d:", iii);
    if (! (iii % 8)) Print (" ");
    Print ("%02lx", (unsigned long) buff[iii]);
  }
  Print ("\n");
}
#endif

static RCVD_MSG_T
rcvBpdu (STATE_MACH_T* this)
{/* 17.19.8 */
  int   bridcmp;
  register PORT_T* port = this->owner.port;

  if (port->msgBpduType == BPDU_TOPO_CHANGE_TYPE) {
#ifdef STP_DBG
    if (this->debug) {
        stp_trace ("%s", "OtherMsg:BPDU_TOPO_CHANGE_TYPE");
    }
#endif
    return OtherMsg;
  }

  port->msgPortRole = RSTP_PORT_ROLE_UNKN;

  if (BPDU_RSTP == port->msgBpduType) {
    port->msgPortRole = (port->msgFlags & PORT_ROLE_MASK) >> PORT_ROLE_OFFS;
  }else if (BPDU_CONFIG_TYPE == port->msgBpduType)  //rzhou add according to 802.1D-2004:17.21.8
  	port->msgPortRole = RSTP_PORT_ROLE_DESGN;       //rzhou add according to 802.1D-2004:17.21.8      

  if (RSTP_PORT_ROLE_DESGN == port->msgPortRole ||
      BPDU_CONFIG_TYPE == port->msgBpduType) {
    bridcmp = STP_VECT_compare_vector (&port->msgPrio, &port->portPrio);

    if (bridcmp < 0 ||
        (! STP_VECT_compare_bridge_id (&port->msgPrio.design_bridge,
                                       &port->portPrio.design_bridge) &&
         port->msgPrio.design_port == port->portPrio.design_port      &&
         STP_compare_times (&port->msgTimes, &port->portTimes))) {
#ifdef STP_DBG
         if (this->debug) {
           stp_trace ("SuperiorDesignateMsg:bridcmp=%d", (int) bridcmp);
         }
#endif
      return SuperiorDesignateMsg;
    }
  }

  if (BPDU_CONFIG_TYPE == port->msgBpduType ||
      RSTP_PORT_ROLE_DESGN == port->msgPortRole) {
    if (! STP_VECT_compare_vector (&port->msgPrio,
                                   &port->portPrio) &&
        ! STP_compare_times (&port->msgTimes, &port->portTimes)) {
#ifdef STP_DBG
        if (this->debug) {
          stp_trace ("%s", "RepeatedDesignateMsg");
        }
#endif
        return RepeatedDesignateMsg;
    }
  }

  if (RSTP_PORT_ROLE_ROOT == port->msgBpduType                    &&
      port->operPointToPointMac                                   &&
      ! STP_VECT_compare_bridge_id (&port->msgPrio.design_bridge,
                                    &port->portPrio.design_bridge) &&
      AGREEMENT_BIT & port->msgFlags) {
#ifdef STP_DBG
    if (this->debug) {
      stp_trace ("%s", "ConfirmedRootMsg");
    }
#endif
    return ConfirmedRootMsg;
  }
  
#ifdef STP_DBG
    if (this->debug) {
      stp_trace ("%s", "OtherMsg");
    }
#endif
  return OtherMsg;
}

static Bool
recordProposed (STATE_MACH_T* this, char* reason)
{/* 17.19.9 */
  register PORT_T* port = this->owner.port;

  if (RSTP_PORT_ROLE_DESGN == port->msgPortRole &&
      (PROPOSAL_BIT & port->msgFlags)           &&
      port->operPointToPointMac) {
    return True;
  }
  return False;
}

static Bool
setTcFlags (STATE_MACH_T* this)
{/* 17.19.13 */
  register PORT_T* port = this->owner.port;

  if (BPDU_TOPO_CHANGE_TYPE == port->msgBpduType) {
#ifdef STP_DBG
      if (this->debug) {
        stp_trace ("port %s rx rcvdTcn", port->port_name);
      }
#endif
    port->rcvdTcn = True;
  } else {
    if (TOLPLOGY_CHANGE_BIT & port->msgFlags) {
#ifdef STP_DBG
      if (this->debug) {
        stp_trace ("(%s-%s) rx rcvdTc 0X%lx",
            port->owner->name, port->port_name,
            (unsigned long) port->msgFlags);
      }
#endif
      port->rcvdTc = True;
    }
    if (TOLPLOGY_CHANGE_ACK_BIT & port->msgFlags) {
#ifdef STP_DBG
      if (this->debug) {
        stp_trace ("port %s rx rcvdTcAck 0X%lx",
            port->port_name,
            (unsigned long) port->msgFlags);
      }
#endif
      port->rcvdTcAck = True;
    }
  }
  return True;
}

static Bool
updtBPDUVersion (STATE_MACH_T* this)
{/* 17.19.18 */
  register PORT_T* port = this->owner.port;

  if (BPDU_TOPO_CHANGE_TYPE == port->msgBpduType) {
    port->rcvdSTP = True;
  }

  if (port->msgBpduVersion < 2) {
    port->rcvdSTP = True;
  }
  
  if (BPDU_RSTP == port->msgBpduType) {
    /* port->port->owner->ForceVersion >= NORMAL_RSTP
       we have checked in STP_info_rx_bpdu */
    port->rcvdRSTP = True;
  }

  return True;
}

static Bool
updtRcvdInfoWhile (STATE_MACH_T* this)
{/* 17.19.19 */
  register int eff_age, dm, dt;
  register int hello3;
  register PORT_T* port = this->owner.port;
  
  eff_age = ( + port->portTimes.MaxAge) / 16;
  if (eff_age < 1) eff_age = 1;
  eff_age += port->portTimes.MessageAge;

  if (eff_age <= port->portTimes.MaxAge) {
    hello3 = 3 *  port->portTimes.HelloTime;
    dm = port->portTimes.MaxAge - eff_age;
    if (dm > hello3)
      dt = hello3;
    else
      dt = dm;
    port->rcvdInfoWhile = dt;
/****
    stp_trace ("ma=%d eff_age=%d dm=%d dt=%d p=%s",
               (int) port->portTimes.MessageAge,
               (int) eff_age, (int) dm, (int) dt, port->port_name);
****/
  } else {
    port->rcvdInfoWhile = 0;
/****/
#ifdef STP_DBG
    /*if (this->debug) */
    {
      stp_trace ("port %s: MaxAge=%d MessageAge=%d HelloTime=%d rcvdInfoWhile=null !",
            port->port_name,
                (int) port->portTimes.MaxAge,
                (int) port->portTimes.MessageAge,
                (int) port->portTimes.HelloTime);
    }
#endif
/****/
  }

  return True;
}
void
STP_info_rx_bpdu (PORT_T* port, struct stp_bpdu_t* bpdu, size_t len)
{  
#if (0)
  _stp_dump ("\nall BPDU", ((unsigned char*) bpdu) - 12, len + 12);
  _stp_dump ("ETH_HEADER", (unsigned char*) &bpdu->eth, 5);
  _stp_dump ("BPDU_HEADER", (unsigned char*) &bpdu->hdr, 4);
  printf ("protocol=%02x%02x version=%02x bpdu_type=%02x\n",
     bpdu->hdr.protocol[0], bpdu->hdr.protocol[1],
     bpdu->hdr.version, bpdu->hdr.bpdu_type);

  _stp_dump ("\nBPDU_BODY", (unsigned char*) &bpdu->body, sizeof (BPDU_BODY_T) + 2);
  printf ("flags=%02x\n", bpdu->body.flags);
  _stp_dump ("root_id", bpdu->body.root_id, 8);
  _stp_dump ("root_path_cost", bpdu->body.root_path_cost, 4);
  _stp_dump ("bridge_id", bpdu->body.bridge_id, 8);
  _stp_dump ("port_id", bpdu->body.port_id, 2);
  _stp_dump ("message_age", bpdu->body.message_age, 2);
  _stp_dump ("max_age", bpdu->body.max_age, 2);
  _stp_dump ("hello_time", bpdu->body.hello_time, 2);
  _stp_dump ("forward_delay", bpdu->body.forward_delay, 2);
  _stp_dump ("ver_1_len", bpdu->ver_1_len, 2);
#endif
  
  /* check bpdu type */
  switch (bpdu->hdr.bpdu_type) {
    case BPDU_CONFIG_TYPE:
	{
		opl_rstp_printf(("Port %d rcv config msg.\n", (int)port->port_id));
      port->rx_cfg_bpdu_cnt++;
#if 0 /* def STP_DBG */
      if (port->info->debug) 
        stp_trace ("CfgBpdu on port %s", port->port_name);
#endif
      if (port->admin_non_stp)
		{
			opl_rstp_printf(("Port %d non stp.\n", (int)port->port_id));
			return;
      	}
      port->rcvdBpdu = True;
      break;
	}
    case BPDU_TOPO_CHANGE_TYPE:
	{
		opl_rstp_printf(("Port %d rcv TC msg.\n", (int)port->port_id));
      port->rx_tcn_bpdu_cnt++;
#if 0 /* def STP_DBG */
      if (port->info->debug)
        stp_trace ("TcnBpdu on port %s", port->port_name);
#endif
      if (port->admin_non_stp) 
	  	{
	  		opl_rstp_printf(("Port %d non stp.\n", (int)port->port_id));
			return;
	  	}
      port->rcvdBpdu = True;
      port->msgBpduVersion = bpdu->hdr.version;
      port->msgBpduType = bpdu->hdr.bpdu_type;
      return;
	}
    case BPDU_RSTP:
	{
		opl_rstp_printf(("Port %d rcv rstp msg.\n", (int)port->port_id));
      port->rx_rstp_bpdu_cnt++;
      if (port->admin_non_stp) 
	  	{
	  		opl_rstp_printf(("Port %d non stp.\n", (int)port->port_id));
			return;
      	}
      if (port->owner->ForceVersion >= NORMAL_RSTP) 
	  {
        port->rcvdBpdu = True;
      } 
	  else 
	  {     
		  opl_rstp_printf(("Port %d force ver %d.\n", (int)port->port_id, (int)port->owner->ForceVersion));
        return;
      }
#if 0 /* def STP_DBG */
      if (port->info->debug)
        stp_trace ("BPDU_RSTP on port %s", port->port_name);
#endif
      break;
	}
    default:
	{
		opl_rstp_printf(("Port %d rcv Unknown msg.\n", (int)port->port_id));
      stp_trace ("RX undef bpdu type=%d", (int) bpdu->hdr.bpdu_type);
      return;
	}

  }

  port->msgBpduVersion = bpdu->hdr.version;
  port->msgBpduType =    bpdu->hdr.bpdu_type;
  port->msgFlags =       bpdu->body.flags;

  /* 17.18.11 */
  STP_VECT_get_vector (&bpdu->body, &port->msgPrio);
  port->msgPrio.bridge_port = port->port_id;

  /* 17.18.12 */
  STP_get_times (&bpdu->body, &port->msgTimes);

  /* 17.18.25, 17.18.26 : see setTcFlags() */

  opl_rstp_printf(("Port %d msg: rcvmsg=%d, ver=%d. type=%d, flag=%d, root_brg_pri=%d, cost=%d, des_brg_pri=%d, des_port=%d.\n",
  				  (int)port->port_id, (int)port->rcvdBpdu,
				  (int)port->msgBpduVersion, (int)port->msgBpduType, (int)port->msgFlags,
				  (int)port->msgPrio.root_bridge.prio, (int)port->msgPrio.root_path_cost,
				  (int)port->msgPrio.design_bridge.prio, (int)port->msgPrio.design_port));

  opl_rstp_printf(("Port %d msg time, bpdu--portmsg:\n msg_age: %d--%d, max_age: %d--%d, hello: %d--%d, fwddelay: %d--%d.\n",
				  (int)port->port_id,
  				  *(unsigned short *)bpdu->body.message_age, port->msgTimes.MessageAge,
				  *(unsigned short *)bpdu->body.max_age, port->msgTimes.MaxAge,
				  *(unsigned short *)bpdu->body.hello_time, port->msgTimes.HelloTime,
				  *(unsigned short *)bpdu->body.forward_delay, port->msgTimes.ForwardDelay));
}

void STP_info_enter_state (STATE_MACH_T* this)
{
  register PORT_T* port = this->owner.port;

  switch (this->State) {
    case BEGIN:
/*rzhou      port->rcvdMsg = OtherMsg;
      port->msgBpduType = -1;
      port->msgPortRole = RSTP_PORT_ROLE_UNKN;
      port->msgFlags = 0;
*/
      /* clear port statistics */
/*      port->rx_cfg_bpdu_cnt =
      port->rx_rstp_bpdu_cnt =
      port->rx_tcn_bpdu_cnt = 0;
*/
    case RSTP_PORT_DISABLED:   
  	  port->rcvdMsg = 
  	  port->proposing =
  	  port->proposed =
  	  port->agree =
  	  port->agreed = False;
  	  
  	  port->rcvdInfoWhile = 0;
  	  
  	  port->infoIs = Disabled;   

  	  port->reselect = True;
      port->selected = False;
        break;
    case AGED:   
      port->infoIs = Aged;
      port->reselect = True;
      port->selected = False;
      break;
    case UPDATE:
      port->proposed = 
      port->proposing = False; 
      port->agreed = port->agreed && betterorsameInfo(this,port->infoIs);// rzhou add later
      port->synced = port->synced && port->agreed; /* In UPDATE */
      STP_VECT_copy (&port->portPrio, &port->designPrio);
      STP_copy_times (&port->portTimes, &port->designTimes);
      port->updtInfo = False;
      port->infoIs = Mine;
      port->newInfo = True;
	  
	  #ifdef STP_DBG
      if (this->debug) {
        STP_VECT_br_id_print ("updated: portPrio.design_bridge",
                            &port->portPrio.design_bridge, True);
      }
	  #endif
      break;   
    case CURRENT:
      break;   
    case RECEIVE:
      port->rcvdInfo = rcvInfo(this);
      break;   
    case SUPERIOR_DESIGNATED:
      port->agreed = 
      port->proposing = False; 
      recordProposal(this);
      setTcFlags(this);
      port->agree = port->agree && betterorsameInfo(this,port->infoIs);
      recordPriority(this);
      recordTimes(this);
      updtRcvdInfoWhile(this);
      port->infoIs =  Received;
      port->reselect = True;
      port->selected = False;
      port->rcvdMsg =  False;

#ifdef STP_DBG
      if (this->debug) {
        STP_VECT_br_id_print ("stored: portPrio.design_bridge",
                            &port->portPrio.design_bridge, True);
        stp_trace ("proposed=%d on port %s",
                   (int) port->proposed, port->port_name);
      }
#endif
      break;   
    case REPEATED_DESIGNATED:
      recordProposal(this);
      setTcFlags(this);
      updtRcvdInfoWhile(this);
      port->rcvdMsg =  False;
      
      break;   
    case INFERIOR_DESIGNATED:
      recordDispute(this);
      port->rcvdMsg =  False;	
      break;   
    	  
    case NOT_DESIGNATED:
      recordAgreement(this);
      setTcFlags(this);
      port->rcvdMsg =  False;   
      break;
      
    case OTHER:
      port->rcvdMsg =  False;
      break;
    }
}

Bool STP_info_check_conditions (STATE_MACH_T* this)
{
  register PORT_T* port = this->owner.port;

  if ((! port->portEnabled && port->infoIs != Disabled) || BEGIN == this->State) {
    return STP_hop_2_state (this, RSTP_PORT_DISABLED);
  }
  switch (this->State) {
    case RSTP_PORT_DISABLED:
      if(port->rcvdMsg)
          return STP_hop_2_state (this, RSTP_PORT_DISABLED);
      if(port->portEnabled)
          return STP_hop_2_state (this, AGED);
      break;
    case AGED:
      if(port->selected && port->updtInfo)
      	  return STP_hop_2_state (this, UPDATE);
      break;    	
    case UPDATE:
      return STP_hop_2_state (this, CURRENT);
    case CURRENT:
      if(port->selected && port->updtInfo)
      	return STP_hop_2_state (this, UPDATE);
      
      if((port->infoIs == Received) && (port->rcvdInfoWhile == 0) &&!port->updtInfo && !port->rcvdMsg)
      	return STP_hop_2_state (this, AGED);
      
      if(port->rcvdMsg && !port->updtInfo)
      	return STP_hop_2_state (this, RECEIVE);      	
      break;    	
    case RECEIVE:
      if(port->rcvdInfo == SuperiorDesignatedInfo)
        return STP_hop_2_state (this, SUPERIOR_DESIGNATED);
      if(port->rcvdInfo == RepeatedDesignatedInfo)
        return STP_hop_2_state (this, REPEATED_DESIGNATED);
      if(port->rcvdInfo == InferiorDesignatedInfo)
      	return STP_hop_2_state (this, INFERIOR_DESIGNATED);
      if(port->rcvdInfo == InferiorRootAlternateInfo)
      	return STP_hop_2_state (this, NOT_DESIGNATED);
      if(port->rcvdInfo == OtherInfo)
        return STP_hop_2_state (this, OTHER);
      break;    	
    case SUPERIOR_DESIGNATED:
      return STP_hop_2_state (this, CURRENT);	
    case REPEATED_DESIGNATED:
    	return STP_hop_2_state (this, CURRENT);	
    case INFERIOR_DESIGNATED:
    	return STP_hop_2_state (this, CURRENT);	
    case NOT_DESIGNATED:
    	return STP_hop_2_state (this, CURRENT);	
    case OTHER:
    	return STP_hop_2_state (this, CURRENT);	
  }
  return False;
}
