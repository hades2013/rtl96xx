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
**    FILE       :  rstp_bridge.c
**      
**    DESCRIPTION:  This file contains RSTP Bridge startup routine. 
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
#include <vos.h>

#include "odm_port.h"
#include "rstp_base.h"
#include "uid_stp.h"
#include "rstp_in.h"
#include "rstp_cli.h"
#include "odm_rstp.h"

#if 0
#ifndef ODM_NUM_OF_PORTS
#define ODM_NUM_OF_PORTS 4
#endif
#endif

VOS_THREAD_t g_stRstpThreadId;

/***************** Global Constants, Variables and Functions Declaration *****************/
/* Rstp maximum frame unit number */
#define RSTP_MAX_UNIT_NUM   10
/* Rstp maximum frame unit size */
#define RSTP_DATA_UNIT_SIZE  300
/* For temp usage */
unsigned short portMap(unsigned short usPort);
/* Bitmap of enabled port */
BITMAP_T    enabled_ports;
/* Rstp thread main loop */
int rstp_loop(void);
/* Rstp seno thread */
static pthread_t rstp_thread_seno;
/* Rstp aeno thread */
static pthread_t rstp_thread_aeno;
/* Rstp frame buffer */
static unsigned char gRstpBuff[RSTP_DATA_UNIT_SIZE];
/* Rstp buffer list */
static unsigned char gRstpBuffBase[RSTP_MAX_UNIT_NUM*RSTP_DATA_UNIT_SIZE];

/* Rstp init state */
unsigned int gRstpInit = 0;
/* Rstp bridge thread shutdown flag */
char shutdown_flag = 0;
/* Tlv Message from CLI */
//struct tlv_msg_s rstp_tlv_msg;
/* Rstp message processing */
void rstp_sync_event_process(void);
/* Rstp control put message */
void rstp_ctl_put(char *buf_p);
/* Port ID from physical to logical */
unsigned short portPhyToLog(unsigned short usPort);
/* Port ID from logical to physical */
unsigned short portLogToPhy(unsigned short usPort);

/***************** Extern function and variable declaration *****************/

/*******************************************************************************
*
* NAME: bridge_start
*
* DESCRIPTION: This function is init the rstp state machine and then enter the              
* rstp state machine loop.
* 
* RETURN: None
*
* INPUTS: None
*					port_index  port ID;
*         enable      0:disable;1:enable.  
*
* OUTPUTS: None
*
* SIDE EFFECTS:
*
*******************************************************************************/
UID_STP_CFG_T uid_cfg;

UID_PORT_CFG_T port_cfg[NUMBER_OF_PORTS];

void bridge_conf_read()
{
	char *str = NULL;
	char *data = NULL;
	char str2[10];
	int i;
	
	//vosConfigFlush();
	
	memset(&uid_cfg, 0, sizeof(UID_STP_CFG_T));
	memset(port_cfg, 0, sizeof(UID_PORT_CFG_T) * NUMBER_OF_PORTS);
	
	str = vosConfigValueGet(CFGFILE_RSTP,
									   RSTP_SECTION_BRD,
									   RSTP_BRD_KEY_VALUE_MODE,
									   RSTP_VALUE_DISABLE);
	if (0 == strcmp(RSTP_VALUE_ENABLE, str))
	{  
		uid_cfg.stp_enabled = STP_ENABLED;
	}
	else
	{
		uid_cfg.stp_enabled = STP_DISABLED;
	}

	uid_cfg.hello_time = vosConfigUInt32Get(CFGFILE_RSTP,
													   RSTP_SECTION_BRD,
													   RSTP_BRD_KEY_U32_HELLOTIME,
													   RSTP_BRD_DEFAULT_HELLOTIME);

	uid_cfg.max_age = vosConfigUInt32Get(CFGFILE_RSTP,
												    RSTP_SECTION_BRD,
												    RSTP_BRD_KEY_U32_MAXAGE,
												    RSTP_BRD_DEFAULT_MAXAGE);

	uid_cfg.forward_delay = vosConfigUInt32Get(CFGFILE_RSTP,
														  RSTP_SECTION_BRD,
														  RSTP_BRD_KEY_U32_FWDDELAY,
														  RSTP_BRD_DEFAULT_FWDDELAYTIME);

	uid_cfg.force_version = vosConfigUInt32Get(CFGFILE_RSTP,
														  RSTP_SECTION_BRD,
														  RSTP_BRD_KEY_U32_VERSION,
														  RSTP_BRD_DEFAULT_FORCEVERSION);

	uid_cfg.bridge_priority = vosConfigUInt32Get(CFGFILE_RSTP,
														    RSTP_SECTION_BRD,
														    RSTP_BRD_KEY_U32_PRI,
														    RSTP_BRD_DEFAULT_PRI);

	uid_cfg.bridge_priority = uid_cfg.bridge_priority * 4096;

	uid_cfg.diameter = vosConfigUInt32Get(CFGFILE_RSTP,
												     RSTP_SECTION_BRD,
												     RSTP_BRD_KEY_U32_DIAMETER,
												     RSTP_BRD_DEFAULT_DIAMETER);

	for(i = 1; i <= NUMBER_OF_PORTS; i++)
	{
		sprintf(str2,"PORT %d",i);
		
		str = vosConfigValueGet(CFGFILE_RSTP,
										   str2,
										   RSTP_PORT_KEY_U32_P2P,
										   RSTP_VALUE_ENABLE);
		if (0 == strcmp(RSTP_VALUE_ENABLE, str))
		{
			port_cfg[i-1].p2plink = STP_ENABLED;
		}
		else
		{
			port_cfg[i-1].p2plink = STP_DISABLED;
		}

		str = vosConfigValueGet(CFGFILE_RSTP,
										   str2,
										   RSTP_PORT_KEY_VALUE_MODE,
										   RSTP_VALUE_DISABLE);
		if (0 == strcmp(RSTP_VALUE_ENABLE, str))
		{
			port_cfg[i-1].enable = STP_ENABLED;
		}
		else
		{
			port_cfg[i-1].enable = STP_DISABLED;
		}

		str = vosConfigValueGet(CFGFILE_RSTP,
										   str2,
										   RSTP_PORT_KEY_VALUE_EDGE,
										   RSTP_VALUE_ENABLE);
		if (0 == strcmp(RSTP_VALUE_ENABLE, str))
		{
			port_cfg[i-1].edge_port = STP_ENABLED;
		}
		else
		{
			port_cfg[i-1].edge_port = STP_DISABLED;
		}

		port_cfg[i-1].priority = vosConfigUInt32Get(CFGFILE_RSTP,
															   RSTP_SECTION_BRD,
															   RSTP_PORT_KEY_U32_PRI,
															   RSTP_PORT_DEFAULT_PRI);

		port_cfg[i-1].priority = port_cfg[i-1].priority * 16;

		port_cfg[i-1].path_cost = vosConfigUInt32Get(CFGFILE_RSTP,
															    RSTP_SECTION_BRD,
															    RSTP_PORT_KEY_U32_PCOST,
															    RSTP_PORT_DEFAULT_PATH_COST);

	}
}
int bridge_start (void)
{
  BITMAP_T  ports;

  register int  iii;
  
  STP_IN_init (NUMBER_OF_PORTS);
  BitmapClear(&enabled_ports);
  BitmapClear(&ports);
  for (iii = 1; iii <= NUMBER_OF_PORTS; iii++) {
    BitmapSetBit(&ports, iii - 1);
  }
  bridge_conf_read();

  uid_cfg.field_mask = BR_CFG_STATE;
  
  snprintf (uid_cfg.vlan_name, NAME_LEN - 1, "Vlan%ld", 0);

  /* create rstp instance */
  iii = STP_IN_stpm_set_cfg (0, &ports, &uid_cfg);
  if (STP_OK != iii) {
    //printf ("FATAL: can't enable:%s\n",STP_IN_get_error_explanation (iii));
    OPL_TRACE();
    return (-1);
  }
  return 0;
}

void bridge_shutdown (void)
{
  int       rc;
  rc = STP_IN_stpm_delete (0);
  if (STP_OK != rc) {
    //printf ("FATAL: can't delete:%s\n", STP_IN_get_error_explanation (rc));
    OPL_TRACE();
  }

	if (STP_Vlan_Had_Not_Yet_Been_Created != rc)
	{
		#if 0
		pthread_cancel(g_stRstpThreadId);
		#endif
		rc = vosThreadDestroy(g_stRstpThreadId);
		if (0 != rc)
		{
			OPL_TRACE();
		}
	}
}
#if 0
char *get_prompt (void)
{
  static char prompt[MAX_CLI_PROMT];
  //snprintf (prompt, MAX_CLI_PROMT - 1, "%s B%ld > ", UT_sprint_time_stamp(), my_pid);
  return prompt;
}
int bridge_control (int port_index,
                    UID_CNTRL_BODY_T* cntrl)
{
  switch (cntrl->cmd) {
    case UID_PORT_CONNECT:
      printf ("connected port p%02d\n", port_index);
      BitmapSetBit(&enabled_ports, port_index - 1);
      STP_IN_enable_port (port_index, True);
      break;
    case UID_PORT_DISCONNECT:
      printf ("disconnected port p%02d\n", port_index);
      BitmapClearBit(&enabled_ports, port_index - 1);
      STP_IN_enable_port (port_index, False);
      break;
    case UID_BRIDGE_SHUTDOWN:
      printf ("shutdown from manager :(\n");
      return 1;
    default:
      printf ("Unknown control command <%d> for port %d\n",
              cntrl->cmd, port_index);
  }
  return 0;
}
#endif 
/*******************************************************************************
*
* NAME: rstp_enable_port
*
* DESCRIPTION: This function is enable or disable rstp port in the rstp state machine.             
*
* RETURN: None
*
* INPUTS: None
*					port_index  port ID;
*         enable      0:disable;1:enable.  
*
* OUTPUTS: None
*
* SIDE EFFECTS:
*
*******************************************************************************/
void rstp_enable_port(int port_index, unsigned char enable)
{
	//rzhou add for test
	/*
	unsigned short usPortID;
	usPortID = portPhyToLog(port_index);
  if (usPortID==0)
  			return;
	port_index = usPortID;
	*/
	
	if(1 == enable){
		BitmapSetBit(&enabled_ports, port_index - 1);
	/* set port enabed */
    //STP_IN_enable_port (port_index, True);
	}else {
			BitmapClearBit(&enabled_ports, port_index - 1);
	  /* set port disabed */
      //STP_IN_enable_port (port_index, False);
	}
}

/*******************************************************************************
*
* NAME: rstp_init
*
* DESCRIPTION: This function is to do rstp thread startup.
*              
*
* RETURN: None
*
* INPUTS: None
*
* OUTPUTS: None
*
* SIDE EFFECTS:
*
*******************************************************************************/
void timer_event()
{
    VOS_TIME_USECS_t processStart = 0;
    VOS_TIME_USECS_t current = 0;

	for(;;)
	{
        current = vosTimeUsecsGet();
		/* generate mpcp register request */
		if ((UINT64)(current - processStart) >= (UINT64)(SECOND_USEC))
		{
            processStart = vosTimeUsecsGet();
		    STP_IN_one_second();
            //printf("timer_event: STP_IN_one_second called\r\n");
		}
		//printf("timer \n");
		vosUSleep(50000);
	}
}
int rstp_init (void)
{
	int i;
	int iRet;

	vosHWAddrGet("eth0", &rstp_dev_mac[0]);
	
	iRet = bridge_start();
	if (0 != iRet)
	{
		return iRet;
	}

	/* begin added by jiangmingli for rstp test */
	/* set all ports enabled, but non-rstp */	
	for (i = 1; i <= NUMBER_OF_PORTS; i++)
	{
		rstp_enable_port(i, 1);
	}
	/* end added by jiangmingli for rstp test */

	/* run timers state machine to update rstp */
	STP_IN_one_second();

	/* create thread to run Timers state machine */
	g_stRstpThreadId = vosThreadCreate("Rstp_timer", OP_VOS_THREAD_STKSZ, 100, timer_event, NULL);
	if (NULL == g_stRstpThreadId)
	{
		OPL_TRACE();
	    return ERR_TASK_CREATE_FAIL;
	}
	#if 0
	iRet = pthread_create(&g_stRstpThreadId,NULL,(void *)&timer_event,NULL);
	if(0 != iRet)
	{
	    //printf("create thread for rstp timer event failed\n");
	    return iRet;
  	}
	#endif

	#if 0
	//rzhou add for test
	rstp_enable_port(1, 1);
	#endif

	return 0;
	//rstp_enable_port(2, 1);
	/*
	rstp_enable_port(3, 1);
	rstp_enable_port(4, 1);
	*/
  //pthread_create(&rstp_thread_seno, NULL, (void *)bridge_start, NULL);
    //pthread_create(&rstp_thread_seno, NULL, (void *)rstp_loop, NULL);
    //pthread_create(&rstp_thread_aeno, NULL, (void *)rstp_control, NULL);
}
/*******************************************************************************
*
* NAME: UT_sprint_time_stamp
*
* DESCRIPTION: This function is to print time stamp.              
* 
* 
* RETURN: None
*
* INPUTS: None
*					port_index  port ID;
*         enable      0:disable;1:enable.  
*
* OUTPUTS: None
*
* SIDE EFFECTS:
*
*******************************************************************************/
char* UT_sprint_time_stamp (void)
{
  time_t      clock;
  struct      tm *local_tm;
  static char time_str[20];

  time(&clock);
  local_tm = localtime (&clock);
  strftime(time_str, 20 - 1, "%H:%M:%S", local_tm);
  return time_str;
}
/*******************************************************************************
*
* NAME: sprint_time_stump
*
* DESCRIPTION: This function is to return a print time stamp.
*              
*
* RETURN: time stamp
*
* INPUTS: None
*
* OUTPUTS: None
*
* SIDE EFFECTS:
*
*******************************************************************************/
char* sprint_time_stump (void)
{
  return UT_sprint_time_stamp();
}

/* begin deleted by jiangmingli for rstp compile */
#if 0
VOS_MUTEX_t gRstpRingBuffMutex = PTHREAD_MUTEX_INITIALIZER;
#endif
/* end deleted by jiangmingli for rstp compile */

unsigned char rstp_cmd_flag = 0;
/*******************************************************************************
*
* NAME: rstp_ctl_put
*
* DESCRIPTION: This function is to put CLI command requests into the temp. buffer.
*              
*
* RETURN: 0   always succeed.
*
* INPUTS: None
*
* OUTPUTS: 
*         buf_p    the temp. buffer of the CLI command.  
* SIDE EFFECTS:
*
*******************************************************************************/
void rstp_ctl_put(char *buf_p)
{
	/* begin deleted by jiangmingli for rstp compile */
	#if 0
    pthread_mutex_lock(&gRstpRingBuffMutex);
	#endif
	/* end deleted by jiangmingli for rstp compile */

    //memcpy((char *) &rstp_tlv_msg, buf_p, sizeof(TLV_MSG_T));
  	rstp_cmd_flag = 1;

	/* begin deleted by jiangmingli for rstp compile */
	#if 0
  	pthread_mutex_unlock(&gRstpRingBuffMutex);
	#endif
	/* end deleted by jiangmingli for rstp compile */

}

/*******************************************************************************
*
* NAME: portPhyToLog (for temp usage)
*
* DESCRIPTION: This function is to map from physical port to logical port.
*              
*
* INPUTS: 
*					usPort    physical Port ID.
*
* OUTPUTS: None
*
* RETURN:   
*         Logical port ID.  
*
* SEE ALSO: 
********************************************************************************/
unsigned short portPhyToLog(unsigned short usPort)
{
	if (usPort==1009)
		return 1;
	else if(usPort==288)
		return 3; 		
	else if(usPort==0)
		return 2;
	else if(usPort==1)
		return 4;
	return 0;
}
/*******************************************************************************
*
* NAME: portLogToPhy (for temp usage)
*
* DESCRIPTION: This function is to map from logical port to physical port.
*              
*
* INPUTS: 
*					usPort    logical port ID.
*
* OUTPUTS: None
*
* RETURN:   
*         physical port ID.
*
* SEE ALSO: 
********************************************************************************/
unsigned short portLogToPhy(unsigned short usPort)
{
	if (usPort==1)
		return 1009;
	else if(usPort==3)
		return 288; 		
	else if(usPort==2)
		return 0;
	else if(usPort==4)
		return 1;
	return 0xFF;
}


