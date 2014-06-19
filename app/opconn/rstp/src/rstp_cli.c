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

/*********************************************************************************
* Edit History:
*
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/rstp/src/rstp_cli.c#1 $
* $Log: rstp_cli.c,v $
* Revision 1.1.1.1  2007/08/03 09:02:07  cvsadmin
* Import OPsystem new sources
*
* Revision 1.1.1.1  2007/05/17 02:53:29  schen
* Change CVS server
*
* Revision 1.1.1.1  2007/04/09 06:07:14  pho
* Import OPsystem ipmux_e sources
*
* Revision 1.1.1.1  2007/04/09 04:50:47  pho
* Import OPsystem sources
*
* Revision 1.3  2007/04/02 08:26:29  rzhou
* Modify STP mode from notion s to S
*
* Revision 1.2  2007/03/19 05:02:55  rzhou
* Remove stp_cli.x
*
* 
* *** empty log message ***
*
* 
* *** empty log message ***
*
*
*
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

//#include "cli.h"
#include "opl_debug.h"

#include "rstp_cli.h"
#include "rstp_bitmap.h"
#include "uid_stp.h"
#include "rstp_in.h"
#include "rstp_to.h"

int I_am_a_stupid_hub = 0;

static void
print_bridge_id (int lFd, UID_BRIDGE_ID_T *bridge_id, unsigned char cr)
{
	vosPrintf(lFd,"%04lX-%02x%02x%02x%02x%02x%02x",
				  (unsigned long) bridge_id->prio,
				  (unsigned char) bridge_id->addr[0],
				  (unsigned char) bridge_id->addr[1],
				  (unsigned char) bridge_id->addr[2],
				  (unsigned char) bridge_id->addr[3],
				  (unsigned char) bridge_id->addr[4],
				  (unsigned char) bridge_id->addr[5]);
	if (cr)
	{
		vosPrintf(lFd,"\n");
	}
}

static char *
stp_state2str (RSTP_PORT_STATE stp_port_state, int detail)
{
  if (detail) {
    switch (stp_port_state) {
      case UID_PORT_DISABLED:   return "Disabled";
      case UID_PORT_DISCARDING: return "Discarding";
      case UID_PORT_LEARNING:   return "Learning";
      case UID_PORT_FORWARDING: return "Forwarding";
      case UID_PORT_NON_STP:    return "NoStp";
      default:                  return "Unknown";
    }
  }

  switch (stp_port_state) {
    case UID_PORT_DISABLED:     return "Dis";
    case UID_PORT_DISCARDING:   return "Blk";
    case UID_PORT_LEARNING:     return "Lrn";
    case UID_PORT_FORWARDING:   return "Fwd";
    case UID_PORT_NON_STP:      return "Non";
    default:                    return "Unk";
  }
}

static void CLI_out_port_id (int lFd, int port, unsigned char cr)
{
	vosPrintf(lFd, "%s", STP_OUT_get_port_name (port));
	if (cr)
	{
		vosPrintf(lFd, "\r\n");
	}
}

int cli_enable (void)
{
  UID_STP_CFG_T uid_cfg;
  int rc;

  uid_cfg.field_mask = BR_CFG_STATE;
  uid_cfg.stp_enabled = STP_ENABLED;
  rc = STP_IN_stpm_set_cfg (0, NULL, &uid_cfg);
  if (rc) {
    //printf ("can't enable: %s\n", STP_IN_get_error_explanation (rc));
    OPL_TRACE();
  } else
    I_am_a_stupid_hub = 0;

  return 0;
}

int cli_disable (void)
{
  UID_STP_CFG_T uid_cfg;
  int rc;

  uid_cfg.field_mask = BR_CFG_STATE;
  uid_cfg.stp_enabled = STP_DISABLED;
  rc = STP_IN_stpm_set_cfg (0, NULL, &uid_cfg);
  if (rc) {
    //printf ("can't disable: %s\n", STP_IN_get_error_explanation (rc));
    OPL_TRACE();
  } else
    I_am_a_stupid_hub = 1;

  return 0;
}

int cli_br_get_cfg (int lFd)
{
	UID_STP_STATE_T uid_state;
	UID_STP_CFG_T   uid_cfg;
	int             rc;

	rc = STP_IN_stpm_get_state (0, &uid_state);
	if (rc)
	{
	    vosPrintf(lFd, "can't get rstp bridge state: %s\r\n", STP_IN_get_error_explanation (rc));
	    return 0;
	}
	
	rc = STP_IN_stpm_get_cfg (0, &uid_cfg);
	if (rc)
	{
	    vosPrintf(lFd, "can't get rstp bridge configuration: %s\r\n", STP_IN_get_error_explanation (rc));
	    return 0;
	}


#if 0
  printf("Interface:       %-7s (tag:%d)    State: ",
    uid_state.vlan_name, (int) uid_state.vlan_id);
#else
	vosPrintf(lFd, "State:");
#endif

	switch (uid_state.stp_enabled)
	{
		case STP_ENABLED:  
		{
			vosPrintf(lFd, "enabled\r\n");
			break;
		}
		case STP_DISABLED:
		{
			vosPrintf(lFd, "disabled\r\n");
			break;
		}
		default:
		{
			vosPrintf(lFd, "unknown\r\n");
			return 0;
		}
	}

	vosPrintf(lFd, "BridgeId:        ");
	print_bridge_id(lFd, &uid_state.bridge_id, 0);
	vosPrintf(lFd, "     Bridge Proirity: %lu (0x%lX)\r\n",
				 (unsigned long)uid_state.bridge_id.prio, (unsigned long)uid_state.bridge_id.prio);
	if (uid_cfg.force_version < 2)
	{
		vosPrintf(lFd, "Force Version: stp compatible\r\n");
	}
	else
	{
		vosPrintf(lFd, "Force Version: rstp\r\n");
	}

	vosPrintf(lFd, "Designated Root: ");
	print_bridge_id(lFd, &uid_state.designated_root, 1);
	if (uid_state.root_port)
	{
	    vosPrintf(lFd, "Root Port:       %04lx (", (unsigned long) uid_state.root_port);
        CLI_out_port_id (lFd, uid_state.root_port & 0xfff, False);
	    vosPrintf(lFd, "), Root Cost:     %-lu\r\n", (unsigned long) uid_state.root_path_cost);
	} 
	else 
	{
	    vosPrintf(lFd, "Root Port:       none\r\n");
	}

	if (uid_state.Topo_Change)
	{
	    vosPrintf(lFd, "Topology Change Count: %lu\r\n", uid_state.Topo_Change_Count);
	}
	else
	{
	    vosPrintf(lFd, "Time Since Topology Change: %lu\r\n", uid_state.timeSince_Topo_Change);
	}

	vosPrintf(lFd, "Max Age:         %2d   Bridge Max Age:       %-2d\r\n",
				 (int)uid_state.max_age, (int)uid_cfg.max_age);
	vosPrintf(lFd, "Hello Time:      %2d   Bridge Hello Time:    %-2d\r\n",
			     (int)uid_state.hello_time, (int)uid_cfg.hello_time);
	vosPrintf(lFd, "Forward Delay:   %2d   Bridge Forward Delay: %-2d\r\n",
				 (int)uid_state.forward_delay, (int)uid_cfg.forward_delay);
	vosPrintf(lFd, "Hold Time:       %2d\r\n", (int) uid_cfg.hold_time);

  return 0;
}

void show_rstp_port (int lFd, int detail)
{
	UID_STP_STATE_T      uid_state;
	UID_STP_PORT_STATE_T uid_port;
	UID_STP_PORT_CFG_T   uid_cfg;
	int                  port_index;
	int         rc;
	BITMAP_T* ports_bitmap = &enabled_ports;
	
	rc = STP_IN_stpm_get_state (0, &uid_state);
	if (rc)
	{
	    vosPrintf(lFd, "can't get rstp bridge state: %s\r\n", STP_IN_get_error_explanation (rc));
	} 
	else if (! detail)
	{
	    vosPrintf(lFd, " BridgeId: "); 
		print_bridge_id (lFd, &uid_state.bridge_id, 0);
	    vosPrintf(lFd, "  RootId: "); 
		print_bridge_id (lFd, &uid_state.designated_root, 1);
	}
  
  //rzhou modified as below for (port_index = 0; port_index <= NUMBER_OF_PORTS; port_index++) {
	for (port_index = 1; port_index <= NUMBER_OF_PORTS; port_index++) 
	{
    //printf ("show_rstp_port:%d, ports_bitmap:%x \n", port_index, 0xff&ports_bitmap->part[0]);
		if (! BitmapGetBit(ports_bitmap, port_index - 1))
		{
			continue;
		}
    //printf ("01 show_rstp_port:%d, ports_bitmap:0x%x \n", port_index,0xff&ports_bitmap->part[0]);
    
	    uid_port.port_no = port_index;
	    rc = STP_IN_port_get_state (0, &uid_port);
		if (rc) 
		{
			vosPrintf(lFd, "can't get rstp port state: %s\r\n", STP_IN_get_error_explanation (rc));
			continue;
		}
		memset (&uid_cfg, 0, sizeof (UID_STP_PORT_CFG_T));
		rc = STP_IN_port_get_cfg (0, uid_port.port_no, &uid_cfg);
		if (rc) 
		{
			vosPrintf(lFd, "can't get rstp port config: %s\r\n", STP_IN_get_error_explanation (rc));
			continue;
		}

		if (detail) 
		{
			vosPrintf(lFd, "Stp Port "); 
			CLI_out_port_id(lFd, port_index, False);
#if 0
      printf(": PortId: %04lx in vlan '%s' with tag %d:\n",
        (unsigned long) uid_port.port_id, uid_state.vlan_name, (int) uid_state.vlan_id);
#else
			vosPrintf(lFd, ": PortId: %04lx :\r\n",
						  (unsigned long) uid_port.port_id);
#endif
			vosPrintf(lFd, "Priority:          %-d\r\n", (int) (uid_port.port_id >> 8));
			vosPrintf(lFd, "State:             %-16s", stp_state2str (uid_port.state, 1));
			vosPrintf(lFd, "       Uptime: %-9lu\r\n", uid_port.uptime);
			vosPrintf(lFd, "PortPathCost:      admin: ");
			if (ADMIN_PORT_PATH_COST_AUTO == uid_cfg.admin_port_path_cost)
			{
				vosPrintf(lFd, "%-9s", "Auto");
			}
			else
			{
				vosPrintf(lFd, "%-9lu", uid_cfg.admin_port_path_cost);
			}
			
			vosPrintf(lFd, "       oper: %-9lu\r\n", uid_port.oper_port_path_cost);
			vosPrintf(lFd, "Point2Point:       admin: ");
			
			switch (uid_cfg.admin_point2point) 
			{
				case P2P_FORCE_TRUE:
				{
					vosPrintf(lFd, "%-9s", "ForceYes");
					break;
				}
				case P2P_FORCE_FALSE:
				{
					vosPrintf(lFd, "%-9s", "ForceNo");
					break;
				}
				case P2P_AUTO:
				{
					vosPrintf(lFd, "%-9s", "Auto");
					break;
				}
			}
			
			vosPrintf(lFd, "       oper: %-9s\r\n", uid_port.oper_point2point ? "Yes" : "No");
			vosPrintf(lFd, "Edge:              admin: %-9s       oper: %-9s\r\n",
					      uid_cfg.admin_edge ? "Y" : "N",
						  uid_port.oper_edge ? "Y" : "N");
			vosPrintf(lFd, "Partner:                                  oper: %-9s\r\n",
						  uid_port.oper_stp_neigb ? "Slow" : "Rapid");
        
			if (' ' != uid_port.role) 
			{
				if ('-' != uid_port.role)
				{
					vosPrintf(lFd, "PathCost:          %-lu\r\n", (unsigned long) (uid_port.path_cost));
					vosPrintf(lFd, "Designated Root:   "); 
					print_bridge_id (lFd, &uid_port.designated_root, 1);
					vosPrintf(lFd, "Designated Cost:   %-ld\r\n", (unsigned long) uid_port.designated_cost);
					vosPrintf(lFd, "Designated Bridge: "); 
					print_bridge_id (lFd, &uid_port.designated_bridge, 1);
					vosPrintf(lFd, "Designated Port:   %-4lx\r\n", (unsigned long) uid_port.designated_port);
				}
				
				vosPrintf(lFd, "Role:              ");
				
				switch (uid_port.role) 
				{
					case 'A': 
					{
						vosPrintf(lFd, "Alternate\r\n"); 
						break;
					}
					case 'B': 
					{
						vosPrintf(lFd, "Backup\r\n");
						break;
					}
					case 'R':
					{
						vosPrintf(lFd, "Root\r\n"); 
						break;
					}
					case 'D': 
					{
						vosPrintf(lFd, "Designated\r\n"); 
						break;
					}
					case '-': 
					{
						vosPrintf(lFd, "NonStp\r\n"); 
						break;
					}
					default:  
					{
						vosPrintf(lFd, "Unknown(%c)\r\n", uid_port.role); 
						break;
					}
				}

				if (('R' == uid_port.role) || ('D' == uid_port.role)) 
				{
					/* printf("Tc:                %c  ", uid_port.tc ? 'Y' : 'n'); */
					vosPrintf(lFd, "TcAck:             %c  ",
								  uid_port.top_change_ack ?  'Y' : 'N');
					vosPrintf(lFd, "TcWhile:       %3d\r\n", (int) uid_port.tcWhile);
				}
			}

			if (UID_PORT_DISABLED == uid_port.state || '-' == uid_port.role) 
			{
#if 0
	        printf("helloWhen:       %3d  ", (int) uid_port.helloWhen);
	        printf("lnkWhile:      %3d\n", (int) uid_port.lnkWhile);
	        printf("fdWhile:         %3d\n", (int) uid_port.fdWhile);
#endif
			} 
			else if ('-' != uid_port.role) 
			{
				vosPrintf(lFd, "fdWhile:         %3d  ", (int) uid_port.fdWhile);
				vosPrintf(lFd, "rcvdInfoWhile: %3d\r\n", (int) uid_port.rcvdInfoWhile);
				vosPrintf(lFd, "rbWhile:         %3d  ", (int) uid_port.rbWhile);
				vosPrintf(lFd, "rrWhile:       %3d\r\n", (int) uid_port.rrWhile);
#if 0
	        printf("mdelayWhile:     %3d  ", (int) uid_port.mdelayWhile);
	        printf("lnkWhile:      %3d\n", (int) uid_port.lnkWhile);
	        printf("helloWhen:       %3d  ", (int) uid_port.helloWhen);
	        printf("txCount:       %3d\n", (int) uid_port.txCount);
#endif
			}

			vosPrintf(lFd, "RSTP BPDU rx:      %lu\r\n", (unsigned long) uid_port.rx_rstp_bpdu_cnt);
			vosPrintf(lFd, "CONFIG BPDU rx:    %lu\r\n", (unsigned long) uid_port.rx_cfg_bpdu_cnt);
			vosPrintf(lFd, "TCN BPDU rx:       %lu\r\n", (unsigned long) uid_port.rx_tcn_bpdu_cnt);
		} 
		else 
		{
			vosPrintf(lFd, "%c%c%c  ",
						  (uid_port.oper_point2point) ? ' ' : '*',
						  (uid_port.oper_edge) ?        'E' : ' ',
						  (uid_port.oper_stp_neigb) ?   'S' : ' ');
			CLI_out_port_id(lFd, port_index, False);
			vosPrintf(lFd, " %04lx %3s ", (unsigned long) uid_port.port_id,
						  stp_state2str(uid_port.state, 0));
			vosPrintf(lFd, " ");
			print_bridge_id (lFd, &uid_port.designated_root, 0);
			vosPrintf(lFd, " ");
			print_bridge_id (lFd, &uid_port.designated_bridge, 0);
			vosPrintf(lFd, " %4lx %c", (unsigned long) uid_port.designated_port,  uid_port.role);
			vosPrintf(lFd, "\r\n");
		}

		vosPrintf(lFd, "\r\n");
	}
}

static int cli_pr_get_cfg (int argc, char** argv)
{
  BITMAP_T        ports_bitmap;
  int             port_index;
  char        detail;

  if ('a' == argv[1][0]) {
    BitmapSetAllBits(&ports_bitmap);
    detail = 0;
  } else {
    port_index = strtoul(argv[1], 0, 10);
    BitmapClear(&ports_bitmap);
    BitmapSetBit(&ports_bitmap, port_index - 1);
    detail = 1;
  }
  // printf("00 cli_pr_get_cfg called ports_bitmap:0x %x \n",0xFF&ports_bitmap.part[0]);
  //show_rstp_port (&ports_bitmap, detail);
  //printf("01 cli_pr_get_cfg called ports_bitmap = %x \n",ports_bitmap);
  return 0;
}

static void
set_bridge_cfg_value (unsigned long value, unsigned long val_mask)
{
  UID_STP_CFG_T uid_cfg;
  char*         val_name;
  int           rc;

  uid_cfg.field_mask = val_mask;
  switch (val_mask) {
    case BR_CFG_STATE:
      uid_cfg.stp_enabled = value;
      val_name = "state";
      break;
    case BR_CFG_PRIO:
      uid_cfg.bridge_priority = value;
      val_name = "priority";
      break;
    case BR_CFG_AGE:
      uid_cfg.max_age = value;
      val_name = "max_age";
      break;
    case BR_CFG_HELLO:
      uid_cfg.hello_time = value;
      val_name = "hello_time";
      break;
    case BR_CFG_DELAY:
      uid_cfg.forward_delay = value;
      val_name = "forward_delay";
      break;
    case BR_CFG_FORCE_VER:
      uid_cfg.force_version = value;
      val_name = "force_version";
      break;
    case BR_CFG_AGE_MODE:
    case BR_CFG_AGE_TIME:
    default: 
		//printf ("Invalid value mask 0X%lx\n", val_mask); 
		OPL_LOG_TRACE();
		return;
      break;
  }

  rc = STP_IN_stpm_set_cfg (0, NULL, &uid_cfg);

  if (0 != rc) {
    //printf ("Can't change rstp bridge %s:%s", val_name, STP_IN_get_error_explanation (rc));
    OPL_TRACE();
  } else {
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Changed rstp bridge %s\r\n", val_name);
  }
}

int cli_br_prio (int br_prio)
{
  if (! br_prio) {
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Warning: newPriority=0, are you sure ?\r\n");
  }
  set_bridge_cfg_value (br_prio, BR_CFG_PRIO);
  return 0;
}

int cli_br_maxage (int value)
{
  set_bridge_cfg_value (value, BR_CFG_AGE);
  return 0;
}

int cli_br_fdelay (int value)
{
  set_bridge_cfg_value (value, BR_CFG_DELAY);
  return 0;
}

/* begin modified by jiangmingli for RSTP, 2008-10-24 */
int cli_br_diameter(int iHelloTime, int iDiameter)
{
	UID_STP_CFG_T uid_cfg;
	int rc;

	uid_cfg.field_mask = BR_CFG_HELLO | BR_CFG_DELAY | BR_CFG_AGE;
	uid_cfg.hello_time = iHelloTime;
	uid_cfg.forward_delay =  iDiameter * iHelloTime + 1;;
	uid_cfg.max_age = (iDiameter + 3) * iHelloTime;

	rc = STP_IN_stpm_set_cfg (0, NULL, &uid_cfg);
	if (0 != rc)
	{
		//printf ("Can't change rstp bridge %s:%s", "diameter", STP_IN_get_error_explanation (rc));
		OPL_TRACE();
	}

	return rc;
}
/* end modified by jiangmingli for RSTP, 2008-10-24 */

int cli_br_fvers (int value)
{
  /*long      value = 2L;*/
  switch (value) {
      case 0:
      case 1:
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Accepted 'force_slow'\r\n");
        break;
      case 2:
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Accepted 'rapid'\r\n");
        break;
      default:
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Invalid argument %d \r\n", value);
        return 0;
  }
  set_bridge_cfg_value (value, BR_CFG_FORCE_VER);
  return 0;
}

static void
set_rstp_port_cfg_value (int port_index,
                         unsigned long value,
                         unsigned long val_mask)
{
  UID_STP_PORT_CFG_T uid_cfg;
  int           rc, detail;
  char          *val_name;

  if (port_index > 0) {
    BitmapClear(&uid_cfg.port_bmp);
    BitmapSetBit(&uid_cfg.port_bmp, port_index - 1);
    detail = 1;
  } else {
    BitmapSetAllBits(&uid_cfg.port_bmp);
    detail = 0;
  }

  uid_cfg.field_mask = val_mask;
  switch (val_mask) {
    case PT_CFG_MCHECK:
      val_name = "mcheck";
      break;
    case PT_CFG_COST:
      uid_cfg.admin_port_path_cost = value;
      val_name = "path cost";
      break;
    case PT_CFG_PRIO:
      uid_cfg.port_priority = value;
      val_name = "priority";
      break;
    case PT_CFG_P2P:
      uid_cfg.admin_point2point = (ADMIN_P2P_T) value;
      val_name = "p2p flag";
      break;
    case PT_CFG_EDGE:
      uid_cfg.admin_edge = value;
      val_name = "adminEdge";
      break;
    case PT_CFG_NON_STP:
      uid_cfg.admin_non_stp = value;
      val_name = "adminNonStp";
      break;
#ifdef STP_DBG
    case PT_CFG_DBG_SKIP_TX:
      uid_cfg.skip_tx = value;
      val_name = "skip tx";
      break;
    case PT_CFG_DBG_SKIP_RX:
      uid_cfg.skip_rx = value;
      val_name = "skip rx";
      break;
#endif
    case PT_CFG_STATE:
    default:
      //printf ("Invalid value mask 0X%lx\n", val_mask);
      OPL_TRACE();
      return;
  }

  rc = STP_IN_set_port_cfg (0, &uid_cfg);
  if (0 != rc) {
    //printf ("can't change rstp port[s] %s: %s\n", val_name, STP_IN_get_error_explanation (rc));
    OPL_TRACE();
  } else {
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "changed rstp port[s] %s\r\n", val_name);
  }

  /* show_rstp_port (&uid_cfg.port_bmp, 0); */
}

int cli_prt_prio (int port_index, int value)
{
  /*if ('a' != argv[1][0])
    port_index = strtoul(argv[1], 0, 10);
	*/
	set_rstp_port_cfg_value (port_index, value, PT_CFG_PRIO);
  return 0;
}

int cli_prt_pcost (int port_index, int value)
{
 /* int port_index = 0;
  unsigned long value = 0;

  if ('a' != argv[1][0])
    port_index = strtoul(argv[1], 0, 10);
  value = strtoul(argv[2], 0, 10);

*/
  set_rstp_port_cfg_value (port_index, value, PT_CFG_COST);
  return 0;
}

int cli_prt_mcheck (int port_index)
{
  /*int port_index = 0;
  if ('a' != argv[1][0])
    port_index = strtoul(argv[1], 0, 10);*/
    
  set_rstp_port_cfg_value (port_index, 0, PT_CFG_MCHECK);
  return 0;
}

static int get_bool_arg (int narg, int argc, char** argv,
                         unsigned long* value)
{
  switch (argv[narg][0]) {
    case 'y':
    case 'Y':
      *value = 1;
      break;
    case 'n':
    case 'N':
      *value = 0;
      break;
    default:
      //printf ("Invalid Bollean parameter '%s'\n", argv[narg]);
      OPL_TRACE();
      return -1;
  }
  return 0;
}

int cli_prt_edge (int port_index, int value)
{
 /* int port_index = 0;
  unsigned long value = 1;

  if ('a' != argv[1][0])
    port_index = strtoul(argv[1], 0, 10);

  if (0 != get_bool_arg (2, argc, argv, &value))
    return 0;
*/
  set_rstp_port_cfg_value (port_index, value, PT_CFG_EDGE);
  return 0;
}

int cli_prt_non_stp (int port_index, int value)
{
  /*int port_index = 0;
  unsigned long value = 0;
	if ('a' != argv[1][0])
    port_index = strtoul(argv[1], 0, 10);
  if (0 != get_bool_arg (2, argc, argv, &value))
    return 0;
	*/
  set_rstp_port_cfg_value (port_index, value, PT_CFG_NON_STP);
  return 0;
}

int cli_prt_p2p (int port_index, unsigned char value)
{
  /*int port_index = 0;
  unsigned long value = P2P_FORCE_TRUE;

  if ('a' != argv[1][0])
    port_index = strtoul(argv[1], 0, 10);

  switch (argv[2][0]) {
      case 'y':
      case 'Y':
        value = P2P_FORCE_TRUE;
        break;
      case 'n':
      case 'N':
        value = P2P_FORCE_FALSE;
        break;
      case 'a':
      case 'A':
        value = P2P_AUTO;
        break;
      default:
        printf ("Invalid parameter '%s'\n", argv[2]);
        return 0;
  }*/
  set_rstp_port_cfg_value (port_index, (ADMIN_P2P_T) value, PT_CFG_P2P);
  return 0;
}


#ifdef STP_DBG
static int cli_trace (int port_index, CLI_KEYWORD_TYPE_t enuCliKey)
{
  /*BITMAP_T ports_bitmap;

  if(port_index > 0){
    BitmapClear(&ports_bitmap);
    BitmapSetBit(&ports_bitmap, port_index - 1);
  } else {
    BitmapSetAllBits(&ports_bitmap);
  }
  STP_IN_dbg_set_port_trace (argv[2],
                             argv[3][0] != 'n' && argv[3][0] != 'N',
                             0, &ports_bitmap,
                             1);
  */
  return 0;
}

/****
  PARAM_NUMBER("port number", 1, NUMBER_OF_PORTS, "all")
  PARAM_ENUM("receive or/and transmit")
    PARAM_ENUM_SEL("rx", "receive")
    PARAM_ENUM_SEL("tx", "transmit")
    PARAM_ENUM_DEFAULT("all")
  PARAM_NUMBER("number of BPDU to skip", 0, 10000, "1")
****/
static int cli_skip (int argc, char** argv)
{
  int port_index = 0, to_skip;

  if ('a' != argv[1][0])
    port_index = strtoul(argv[1], 0, 10);

  to_skip = atoi (argv[3]);

  if ('a' == argv[2][0] || 'r' == argv[2][0]) {
    set_rstp_port_cfg_value (port_index, to_skip, PT_CFG_DBG_SKIP_RX);
  }

  if ('a' == argv[2][0] || 't' == argv[2][0]) {
    set_rstp_port_cfg_value (port_index, to_skip, PT_CFG_DBG_SKIP_TX);
  }
  return 0;
}

static int cli_sleep (int argc, char** argv)
{
  int delay = atoi (argv[1]);
  sleep (delay);
  return 0;
}

#endif

int stp_cli_init (void)
{
   I_am_a_stupid_hub = 0;
   return 0;
}

int cli_br_hellotime (int value)
{
  set_bridge_cfg_value (value, BR_CFG_HELLO);
  return 0;
}

static void set_rstp_multiport_cfg_value
(
	unsigned long ulPortBitmap,
	unsigned long *aulValue,
	unsigned long ulMask
)
{
	UID_STP_PORT_CFG_T uid_cfg;
	int rc;
	int iValindex = 0;

	/* if ulPortBitmap=0, set all port */
	if (0 == ulPortBitmap)
	{
		uid_cfg.port_bmp.part[0] = 0x0000000F;
	}
	else
	{
		uid_cfg.port_bmp.part[0] = ulPortBitmap;
	}
	
	uid_cfg.field_mask = ulMask;
	
	/* set path cost cfg */
	if (0 != (ulMask & PT_CFG_COST))
	{
		uid_cfg.admin_port_path_cost = aulValue[iValindex];
		iValindex++;
	}

	/* set priority cfg */
	if (0 != (ulMask & PT_CFG_PRIO))
	{
		uid_cfg.port_priority = aulValue[iValindex];
		iValindex++;
	}

	/* set p2p cfg */
	if (0 != (ulMask & PT_CFG_P2P))
	{
		uid_cfg.admin_point2point = (ADMIN_P2P_T)aulValue[iValindex];
		iValindex++;
	}

	/* set port edge cfg */
	if (0 != (ulMask & PT_CFG_EDGE))
	{
		uid_cfg.admin_edge = aulValue[iValindex];
		iValindex++;
	}

	
	/* set mcheck cfg */
	if (0 != (ulMask & PT_CFG_MCHECK))
	{
	}

	/* set port admin rstp cfg */
	if (0 != (ulMask & PT_CFG_NON_STP))
	{
		uid_cfg.admin_non_stp = aulValue[iValindex];
		iValindex++;
	}

	
#ifdef STP_DBG
	/* set skip rx cfg */
	if (0 != (ulMask & PT_CFG_DBG_SKIP_RX))
	{
		uid_cfg.skip_rx = aulValue[iValindex];
		iValindex++;
	}
	
	/* set skip tx cfg */
	if (0 != (ulMask & PT_CFG_DBG_SKIP_TX))
	{
		uid_cfg.skip_tx = aulValue[iValindex];
		iValindex++;
	}
#endif

	rc = STP_IN_set_port_cfg (0, &uid_cfg);
	if (0 != rc)
	{
		OPL_TRACE();
	}

	return;
}

int cli_multiport_prio(unsigned long ulPortBitmap, int ulValue)
{
	set_rstp_multiport_cfg_value(ulPortBitmap, (unsigned long *)&ulValue, PT_CFG_PRIO);

	return 0;
}

int cli_multiport_pcost(unsigned long ulPortBitmap, int ulValue)
{
	set_rstp_multiport_cfg_value(ulPortBitmap, (unsigned long *)&ulValue, PT_CFG_COST);
	
	return 0;
}

int cli_multiport_mcheck(unsigned long ulPortBitmap, int ulValue)
{
	set_rstp_multiport_cfg_value(ulPortBitmap, (unsigned long *)&ulValue, PT_CFG_MCHECK);
		
	return 0;
}

int cli_multiport_edge(unsigned long ulPortBitmap, int ulValue)
{
	set_rstp_multiport_cfg_value(ulPortBitmap, (unsigned long *)&ulValue, PT_CFG_EDGE);
	
	return 0;
}

int cli_multiport_non_rstp(unsigned long ulPortBitmap, int ulValue)
{
	set_rstp_multiport_cfg_value(ulPortBitmap, (unsigned long *)&ulValue, PT_CFG_NON_STP);
	
	return 0;
}

int cli_multiport_p2p(unsigned long ulPortBitmap, int ulValue)
{
	set_rstp_multiport_cfg_value(ulPortBitmap, (unsigned long *)&ulValue, PT_CFG_P2P);
		
	return 0;
}

