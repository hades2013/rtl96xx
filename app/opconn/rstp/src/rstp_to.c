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
**    FILE       :  rstp_to.c
**      
**    DESCRIPTION:  This file contains This file contains system dependent API
**    from the RStp to a operation system (see rstp_to.h).
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
/* stp_to API for Linux */
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>

#include "hal.h"
#include "hal_rstp.h"

#include "rstp_base.h"
#include "rstp_stpm.h"
#include "rstp_in.h"
#include "rstp_to.h"
#include "opl_dma.h"
#include "opl_host.h"

extern BITMAP_T        enabled_ports;

/*************
void
stp_trace (const char *format, ...)
{
  #define MAX_MSG_LEN  128
  char     msg[MAX_MSG_LEN];
  va_list  args;

  va_start(args, format);
  vsnprintf (msg, MAX_MSG_LEN-1, format, args);
  printf ("%s\n", msg);
  va_end(args);
  
}
***********/

#ifdef STRONGLY_SPEC_802_1W
int
STP_OUT_set_learning (int port_index, int vlan_id, int enable)
{
    OPL_API_DATA_t stData;
    HAL_API_RSTP_PORT_STATE_SET_S stPara;
	//extern OPL_STATUS BridgeIngressMacLmt(UINT16 usPortID, UINT8 ucMacLmt);
  //OPL_STATUS rt; 
  //unsigned char ucMacLmt = 20; //rzhou to be modified
  //rt = BridgeIngressMacLmt(port_index, ucMacLmt);
  OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "STP_OUT_set_learning called \n");

	if (1 == enable)
	{
        #if 0
		dalRstpPortStateSet(port_index, UID_PORT_LEARNING);
        #endif
        stPara.ulPortId = port_index;
        stPara.ucPortState = UID_PORT_LEARNING;
	}
	else
	{
        #if 0
		dalRstpPortStateSet(port_index, UID_PORT_DISCARDING);
        #endif
        stPara.ulPortId = port_index;
        stPara.ucPortState = UID_PORT_DISCARDING;
	}

    stData.apiId = HAL_API_RSTP_PORT_STATE_SET;
    stData.param = &stPara;
    stData.length = sizeof(stPara);
    halAppApiCallSync(&stData);

  //return rt;
  return STP_OK;
}

int
STP_OUT_set_forwarding (int port_index, int vlan_id, int enable)
{
    OPL_API_DATA_t stData;
    HAL_API_RSTP_PORT_STATE_SET_S stPara;
    
	//extern OPL_STATUS BridgeIngressFwd(UINT16 usPortID, UINT8 ucFwde);
	//OPL_STATUS rt; 
	//rt = BridgeIngressFwd(port_index, True);
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "STP_OUT_set_forwarding called \n");

	if (1 == enable)
	{
        #if 0
		dalRstpPortStateSet(port_index, UID_PORT_FORWARDING);
        #endif
        stPara.ulPortId = port_index;
        stPara.ucPortState = UID_PORT_FORWARDING;
	}
	else
	{
        #if 0
		dalRstpPortStateSet(port_index, UID_PORT_DISCARDING);
        #endif
        stPara.ulPortId = port_index;
        stPara.ucPortState = UID_PORT_DISCARDING;
	}

    stData.apiId = HAL_API_RSTP_PORT_STATE_SET;
    stData.param = &stPara;
    stData.length = sizeof(stPara);
    halAppApiCallSync(&stData);
	
	return 0;
  //rzhou del return STP_OK;
}
#else
/* 
 * In many kinds of hardware the state of ports may
 * be changed with another method
 */
int
STP_OUT_set_port_state (IN int port_index, IN int vlan_id,
            IN RSTP_PORT_STATE state)
{
  OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "STP_OUT_set_port_state called \n");
	return STP_OK;
  //return AR_INT_STP_set_port_state (port_index, vlan_id, state);
}
#endif


void
STP_OUT_get_port_mac (int port_index, unsigned char *mac)
{
  static long pid = -1;
  static unsigned char mac_beg[] = {'\0', '\0', '\0', '\0', '\0', '\0'};

#if 0
  if (pid < 0) {
    pid = getpid ();
    memcpy (mac_beg + 1, &pid, 4);
  }
  memcpy (mac, mac_beg, 5);
  mac[5] = port_index;
  //memcpy (mac, STP_MAIN_get_port_mac (port_index), 6);
#endif
	memcpy(mac, &rstp_dev_mac[0], 6);
}

int             /* 1- Up, 0- Down */
STP_OUT_get_port_link_status (int port_index)
{
  /*if (BitmapGetBit (&enabled_ports, (port_index - 1))) return 1;
  return 0;*/
  unsigned int  linkstate = 0;
  
  odmPortLinkStateGet(port_index, &linkstate);
  
  if (0 == linkstate)
    return 0;
  else
    return 1;
}

int
STP_OUT_flush_lt (IN int port_index, IN int vlan_id, LT_FLASH_TYPE_T type, char* reason)
{
    OPL_API_DATA_t stData;
    HAL_API_RSTP_PORT_MAC_FLUSH_S stPara;
    
/****
  stp_trace("clearFDB (%d, %s, '%s')",
        port_index, 
        (type == LT_FLASH_ALL_PORTS_EXCLUDE_THIS) ? "Exclude" : "Only", 
        reason);
****/
	if (LT_FLASH_ONLY_THE_PORT == type)
	{
		//	BridgePortAddrFlush(port_index, ONLY_THE_PORT);
		#if 0
		dalRstpPortMacFlush(port_index, TRUE);
        #endif
        stPara.ulPortid = port_index;
        stPara.bOnlyThisPort = TRUE;
	}
	else
	{
		//LT_FLASH_ALL_PORTS_EXCLUDE_THIS
		//	BridgePortAddrFlush(port_index, EXCLUDE_THE_PORT);
		#if 0
		dalRstpPortMacFlush(port_index, FALSE);
        #endif
        stPara.ulPortid = port_index;
        stPara.bOnlyThisPort = FALSE;
	}

    stData.apiId = HAL_API_RSTP_PORT_MAC_FLUSH;
    stData.param = &stPara;
    stData.length = sizeof(stPara);
    halAppApiCallSync(&stData);

#ifdef __OPSP_DEBUG__
	 OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "STP_OUT_flush_lt called \n");
#endif

  return STP_OK;
}

int
STP_OUT_set_hardware_mode (int vlan_id, UID_STP_MODE_T mode)
{
    OPL_BOOL bEnable;
    OPL_API_DATA_t stData;
    
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "STP_OUT_set_hardware_mode called \n");

    #if 0
	dalRstpModeSet(mode);
    #endif
    bEnable = mode;
    stData.apiId = HAL_API_RSTP_MODE_SET;
    stData.param = &bEnable;
    stData.length = sizeof(bEnable);
    halAppApiCallSync(&stData);
	
  return STP_OK;
  //return AR_INT_STP_set_mode (vlan_id, mode);
}
unsigned char pdu[100];

int
STP_OUT_tx_bpdu (int port_index, int vlan_id,
         unsigned char *bpdu, size_t bpdu_len)
{
	//int i;
	//printf("STP_OUT_tx_bpdu called port %d len %d\n",port_index,bpdu_len);
	//for(i=0;i<bpdu_len;i+=4)
	//printf("%08x",*(UINT32 *)(bpdu+i));
	//printf("\n");

	#if 0
	char buffer[2048];
	host_outbound_hdr_t *pstHostHeader = NULL;
	pstHostHeader = (host_outbound_hdr_t *)&buffer;
	pstHostHeader->eport = port_index;
	memcpy(buffer+sizeof(host_outbound_hdr_t), bpdu, bpdu_len);
	return eopl_dma_send(buffer,bpdu_len + sizeof(host_outbound_hdr_t));
	#endif

	int iRet;

	iRet = eopl_send_to_down_link(port_index, 0, STP_PKT_TYPE, bpdu, bpdu_len);

	return iRet;
}

const char *
STP_OUT_get_port_name (IN int port_index)
{
  //rzhou debug static char tmp[4];
  static char tmp[6];
  //printf("port_index =%d\n",port_index);
  sprintf (tmp, "p%04d", (int) port_index);
  return tmp;
  //return port2str (port_index, &sys_config);
}

unsigned long
STP_OUT_get_deafult_port_path_cost (IN unsigned int portNo)
{
  return 20000;
}

unsigned long STP_OUT_get_port_oper_speed (unsigned int portNo)
{
	#if 0
	//rzhou need to modified to adapt to IPMux-A broard
	if (portNo <= 2)
    return 1000000L;
  else
    return 1000L;
  	#endif

	unsigned int ulPortSpeed;
	int iRet;

	/* not support SpeedGet now, added by jiangmingli */
	#if 0
	iRet = dalPortSpeedGet((unsigned char)portNo, &ulPortSpeed);
	if (0 != iRet)
	{
		return 100L;	/* 100Mbps */
	}
	#endif

	ulPortSpeed = 100L;

	return ulPortSpeed;
	
}

int             /* 1- Full, 0- Half */
STP_OUT_get_duplex (IN int port_index)
{
	#if 0
	//rzhou need to modified to adapt to IPMux-A broard
  return 1;
	#endif
	
	unsigned int ulDuplex;
	int iRet;
	
	/* get port working state, 1 - full duplex, 0 - half duplex */
	iRet = dalPortDuplexGet(port_index, &ulDuplex);
	if (0 != iRet)
	{
		return 1;	/* full duplex */
	}
	
	if (0 == ulDuplex)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int
STP_OUT_get_init_stpm_cfg (IN int vlan_id,
                           INOUT UID_STP_CFG_T* cfg)
{
  cfg->bridge_priority =        DEF_BR_PRIO;
  cfg->max_age =                DEF_BR_MAXAGE;
  cfg->hello_time =             DEF_BR_HELLOT;
  cfg->forward_delay =          DEF_BR_FWDELAY;
  cfg->force_version =          NORMAL_RSTP;

  return STP_OK;
}
  

int
STP_OUT_get_init_port_cfg (IN int vlan_id,
                           IN int port_index,
                           INOUT UID_STP_PORT_CFG_T* cfg)
{
  cfg->port_priority =                  DEF_PORT_PRIO;
  cfg->admin_non_stp =                  DEF_ADMIN_NON_STP;
  //cfg->admin_edge =                     DEF_ADMIN_EDGE;
  cfg->admin_edge =                     0;
  cfg->admin_port_path_cost =           ADMIN_PORT_PATH_COST_AUTO;
  cfg->admin_point2point =              DEF_P2P;

  return STP_OK;
}



