/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 39101 $
 * $Date: 2013-05-03 04:35:27 -0500 (Fri, 03 May 2013) $
 *
 * Purpose : Definition of OMCI CLI 
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) OMCI (G.984.4) configuration
 *
 */


#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include "app_basic.h"
#include "omci_defs.h"

#if (CONFIG_GPON_VERSION >1)
#include <module/gpon/gpon.h>
#else
#include <rtk/gpon.h>
#endif



static GOS_ERROR_CODE omci_cmd_dumpAvlTree(void)
{
    PON_OMCI_CMD_T msg;
    
	printf("Send Cmd Msg to MSG Queue!\n");
	memset(&msg,0,sizeof(PON_OMCI_CMD_T));

	msg.cmd = PON_OMCI_CMD_DUMPAVL;
	OMCI_SendMsg(OMCI_APPL, OMCI_CMD_MSG, OMCI_MSG_PRI_NORMAL, 
	&msg, sizeof(PON_OMCI_CMD_T));  

	return GOS_OK;
}


static GOS_ERROR_CODE omci_cmd_enableLog(char *cState)
{

    PON_OMCI_CMD_T msg;
	BOOL state = FALSE;
    
	printf("Send Cmd Msg to MSG Queue!\n");
	memset(&msg,0,sizeof(PON_OMCI_CMD_T));

	if(!strcmp(cState,"enable"))
	{
		state = TRUE;
	}

	msg.cmd 	= PON_OMCI_CMD_LOG;
	msg.state 	= state;
	OMCI_SendMsg(OMCI_APPL, OMCI_CMD_MSG, OMCI_MSG_PRI_NORMAL, 
	&msg, sizeof(PON_OMCI_CMD_T));  

	return GOS_OK;
}


static GOS_ERROR_CODE omci_cmd_sendpkt(char* msgOmci)
{
	rtk_gpon_omci_msg_t msg;
	int i;
	char *endPtr,*curPtr;

	printf("Send Msg to MSG Queue!\n");
	memset(&msg,0,sizeof(rtk_gpon_omci_msg_t));
	curPtr = msgOmci;
	for(i =0 ; i< RTK_GPON_OMCI_MSG_LEN;i++)
	{
		msg.msg[i] = strtol(curPtr,&endPtr,16);
		curPtr = endPtr;
	}
	usleep(500);
	return OMCI_SendMsg(OMCI_APPL, OMCI_RX_OMCI_MSG, OMCI_MSG_PRI_NORMAL, 
	(void *)&msg, sizeof(rtk_gpon_omci_msg_t));   
	 
}


static GOS_ERROR_CODE omci_cmd_dumpQueueMap(void)
{
	PON_OMCI_CMD_T msg;
    
	printf("Send Cmd Msg to MSG Queue!\n");
	memset(&msg,0,sizeof(PON_OMCI_CMD_T));

	msg.cmd = PON_OMCI_CMD_DUMPQUEUE;
	OMCI_SendMsg(OMCI_APPL, OMCI_CMD_MSG, OMCI_MSG_PRI_NORMAL, 
	&msg, sizeof(PON_OMCI_CMD_T));  

	return GOS_OK;
}

static GOS_ERROR_CODE omci_cmd_dumpTreeConn(void)
{
	PON_OMCI_CMD_T msg;
    
	printf("Send Cmd Msg to MSG Queue!\n");
	memset(&msg,0,sizeof(PON_OMCI_CMD_T));

	msg.cmd = PON_OMCI_CMD_DUMPTREECONN;
	OMCI_SendMsg(OMCI_APPL, OMCI_CMD_MSG, OMCI_MSG_PRI_NORMAL, 
	&msg, sizeof(PON_OMCI_CMD_T));  

	return GOS_OK;
}


static GOS_ERROR_CODE omci_cmd_dumpMib(char *tableName)
{
	PON_OMCI_CMD_T msg;
    
	printf("Send Cmd Msg to MSG Queue!\n");
	memset(&msg,0,sizeof(PON_OMCI_CMD_T));

	msg.cmd = PON_OMCI_CMD_DUMPMIB;
	if(tableName!=NULL)
	{
		msg.tableId = 0;
		strcpy(msg.filename,tableName);
	}else
	{
		msg.tableId = -1;
	}
	OMCI_SendMsg(OMCI_APPL, OMCI_CMD_MSG, OMCI_MSG_PRI_NORMAL, 
	&msg, sizeof(PON_OMCI_CMD_T));  

	return GOS_OK;
}


static GOS_ERROR_CODE omci_cmd_delMe(int tableId,int entityId)
{


	return GOS_OK;
}


static GOS_ERROR_CODE omci_cmd_set_sn(char *vendorId,int serialNumber)
{

	PON_OMCI_CMD_T msg;

	msg.sn[0] = vendorId[0];	
	msg.sn[1] = vendorId[1];
	msg.sn[2] = vendorId[2];	
	msg.sn[3] = vendorId[3];
	msg.sn[4] = (serialNumber >> 24) & 0xff;
	msg.sn[5] = (serialNumber >> 16) & 0xff;	
	msg.sn[6] = (serialNumber >> 8) & 0xff;	
	msg.sn[7] = (serialNumber)  & 0xff;
	
	msg.cmd = PON_OMCI_CMD_SN_SET;

	OMCI_SendMsg(OMCI_APPL, OMCI_CMD_MSG, OMCI_MSG_PRI_NORMAL, 
	&msg, sizeof(PON_OMCI_CMD_T));  

	return GOS_OK;
}


static GOS_ERROR_CODE omci_cmd_get_sn(void)
{
	
	PON_OMCI_CMD_T msg;
	
	msg.cmd = PON_OMCI_CMD_SN_GET;
	
	printf("Send Cmd Msg to MSG Queue!\n");
	OMCI_SendMsg(OMCI_APPL, OMCI_CMD_MSG, OMCI_MSG_PRI_NORMAL, 
	&msg, sizeof(PON_OMCI_CMD_T));  

	return GOS_OK;
}

static GOS_ERROR_CODE omci_cmd_load_file(char *filename)
{
	
	FILE *pfile;
	char buff[256];
	char *pch;
	
	pfile = fopen(filename,"r");
	if(pfile)
	{
		while(fgets(buff,sizeof(buff),pfile)!=NULL)
		{
			pch = strtok(buff,"\"");
			if(pch) pch = strtok(NULL,"\"");
			if(pch) omci_cmd_sendpkt(pch);
		}
		fclose(pfile);
	}
	return GOS_OK;
}



static GOS_ERROR_CODE omci_cmd_set_help(void)
{
   	printf("Usage: omcicli set [type]\n");
	printf("sn [vendorId] [serialnumber]: set serial number\n");

	printf("End..\n");
	return GOS_OK;
}

static GOS_ERROR_CODE omci_cmd_get_help(void)
{
   	printf("Usage: omcicli get [type]\n");
	printf("sn: get serial number\n");
	printf("tcont: get tcont id by allocId\n");
	printf("End..\n");
	return GOS_OK;
}


static GOS_ERROR_CODE omci_cmd_set(int argc,char **arg)
{
	if(!strcmp(arg[0],"sn") && argc > 1)
	{
		omci_cmd_set_sn(arg[1],strtol(arg[2],NULL,16));
		
	}else
	{
		omci_cmd_set_help();
	}
	return GOS_OK;
}


static GOS_ERROR_CODE omci_cmd_get(int argc,char **arg)
{
	if(!strcmp(arg[0],"sn") && argc > 1)
	{
		omci_cmd_get_sn();
	}
	else
	{
		omci_cmd_get_help();
	}
	return GOS_OK;
}






static GOS_ERROR_CODE omci_cmd_help(void)
{
   	printf("Usage: omcicli [cmd type] [data]\n");
	printf("dump_avltree	: dump AVL tree\n");
	printf("dump_queuemap	: dump tcont & queue mapping table\n");
	printf("log				: enable or disable Log\n");
	printf("sendpkt			: send omci pkt to app for replay\n");
	printf("dump_mib		: dump mib\n");
	printf("dump_treeconn   : dump tree connection\n");
	printf("set			    : set command set\n");
	printf("get			    : get command set\n");
	printf("replay			: load replay and reply it!\n");
	printf("End..\n");
	return GOS_OK;
}


int main(int argc, char* argv[])
{

	if(argc < 2)
	{
		omci_cmd_help();
		return 0;
	}

	if(!strcmp(argv[1],"dump_avltree"))
	{
		omci_cmd_dumpAvlTree();
	}else
	if(!strcmp(argv[1],"log") && argc > 2)
	{
		omci_cmd_enableLog(argv[2]);
	}else
	if(!strcmp(argv[1],"sendpkt") && argc > 2)
	{
		omci_cmd_sendpkt(argv[2]);
	}else
	if(!strcmp(argv[1],"dump_queuemap"))
	{
		omci_cmd_dumpQueueMap();
	}else
	if(!strcmp(argv[1],"dump_treeconn"))
	{
		omci_cmd_dumpTreeConn();
	}else
	if(!strcmp(argv[1],"dump_mib"))
	{
		omci_cmd_dumpMib(argv[2]);
	}
	else
	if(!strcmp(argv[1],"del_me") && argc > 3)
	{
		omci_cmd_delMe(strtol(argv[2],NULL,16),strtol(argv[3],NULL,16));
	}
	else
	if(!strcmp(argv[1],"replay") && argc > 2)
	{
		omci_cmd_load_file(argv[2]);
	}
	else
	if(!strcmp(argv[1],"set") && argc > 2)
	{
		omci_cmd_set(argc-1,&argv[2]);
	}
	else
	if(!strcmp(argv[1],"get") && argc > 2)
	{
		omci_cmd_get(argc-1,&argv[2]);
	}
	else
	{
		omci_cmd_help();
	}
	
	return 0;
}


