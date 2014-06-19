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
 * Purpose : Definition of SVLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) OAM (802.3ah) configuration
 *
 */


#include "app_basic.h"
#include "omci_defs.h"
#include <sys/socket.h> 
#include <linux/netlink.h>

/*driver related*/
#include <pkt_redirect_user.h>

#if (CONFIG_GPON_VERSION >1)
#include <module/gpon/gpon.h>
#else
#include <rtk/gpon.h>
#endif

#include <rtk/classify.h>
#include <rtk/ponmac.h>
#include <omci_driver.h>



GOS_ERROR_CODE OMCI_wrapper_SetPriQueue(MIB_TABLE_PRIQ_T *pPriQ);



#define TXC_EBB_RAM_BASE_ADDR            0x0
#define TXC_EBB_RAM_ADDR_OFFSET_PER_PORT 0x40
#define TXC_MAX_TMB_PORT_ID              0x3A

#define TXC_INVALID_ALLOC_ID             0xFFFF
#define TXC_INVALID_PORT_ID              0xFFFF

#define TXC_PLOAM_ONU_ID_INDEX           0x0
#define TXC_PLOAM_PORT_ID_AT             0x0
#define TXC_PLOAM_PORT_ID_HI             0x1
#define TXC_PLOAM_PORT_ID_LO             0x2

#define OMCI_GEM_DATA_FLOW_OFFSET           (0)
#define TXC_BDG_OUT_PORT_GEM_FIRST          (315)
#define OMCI_GEM_FLOW_TO_BRIGE_PORT(flowId) ((flowId) + TXC_BDG_OUT_PORT_GEM_FIRST)
#define OMCI_MAX_SERV_ID                    (0x20)
#define OMCI_DS_SERV_ID(id)                 ((id) | OMCI_MAX_SERV_ID)
#define CF_RULE_BASE						(64)





typedef struct {
    UINT16 portId;
    BOOL   inUse;
} omci_flow_info_ts;


typedef struct omci_cf_rule_s
{
	UINT32 cfRuleId;
	LIST_ENTRY(omci_cf_rule_s) entries;
	
}omci_cf_rule_t;

typedef struct
{
    UINT16 igress;
    UINT16 ogress;
    BOOL   inUse;
    LIST_HEAD(cfRuleHead,omci_cf_rule_s) cfHead;
	
} omci_serv_info_ts;


/*for priority Queue and real tcont/tcont queue mapping*/
typedef struct priQueue2TcontMap_s{
	UINT8  tcontId;
	UINT16 tcontEntityId;
	UINT32 queueId;	
	UINT16 queueEntityId;
	UINT16 allocId;
}priQueue2TcontMap_t;



UINT16            gOmciChanAllocId = TXC_INVALID_ALLOC_ID;
UINT16            gOmciChanPortId  = TXC_INVALID_PORT_ID;

omci_flow_info_ts gOmciGemDsFlowInfo[TXC_ONT2G_GEM_PORT_NUM];
omci_flow_info_ts gOmciGemUsFlowInfo[TXC_ONT2G_GEM_PORT_NUM];

omci_serv_info_ts gOmciDsServInfo[OMCI_MAX_SERV_ID];
omci_serv_info_ts gOmciUsServInfo[OMCI_MAX_SERV_ID];



static int gSimFd = -1;
static char fileName[32]="/tmp/omci.log";
static FILE *pFile;
static int logTofile = TRUE;

static priQueue2TcontMap_t q2TcontMap[TXC_TCONT_TCONT_NUM][TXC_TCONT_PRIO_Q_PER_TCONT];



void omci_queueMapSet(UINT32 port, UINT32 qId, UINT16 queueEntityId, UINT16 tcontEntityId)
{
	q2TcontMap[port][qId].queueEntityId = queueEntityId;
	q2TcontMap[port][qId].tcontId= 0;
	q2TcontMap[port][qId].queueId= ((port*8)%32+qId);
	q2TcontMap[port][qId].tcontEntityId = tcontEntityId;
}

static void omci_priQUpdate(UINT16 tcontEntityId)
{
	int i,j;
	MIB_TABLE_PRIQ_T priQ;
	for(i=0;i<TXC_TCONT_TCONT_NUM;i++)
		for(j=0;j<TXC_TCONT_PRIO_Q_PER_TCONT;j++)
	{
		if(q2TcontMap[i][j].tcontEntityId == tcontEntityId)
		{
		 	priQ.EntityID = q2TcontMap[i][j].queueEntityId;
			if(MIB_Get(MIB_TABLE_PRIQ_INDEX,&priQ,sizeof(MIB_TABLE_PRIQ_T))==GOS_OK)
			{
				OMCI_wrapper_SetPriQueue(&priQ);
			}
		}
	}
	return;
}


static void omci_priQUpdateByTcontId(UINT16 tcontId)
{
	int i,j;
	MIB_TABLE_PRIQ_T priQ;
	for(i=0;i<TXC_TCONT_TCONT_NUM;i++)
		for(j=0;j<TXC_TCONT_PRIO_Q_PER_TCONT;j++)
	{
		if(q2TcontMap[i][j].tcontId == tcontId)
		{
		 	priQ.EntityID = q2TcontMap[i][j].queueEntityId;
			if(MIB_Get(MIB_TABLE_PRIQ_INDEX,&priQ,sizeof(MIB_TABLE_PRIQ_T))==GOS_OK)
			{
				OMCI_wrapper_SetPriQueue(&priQ);
			}
		}
	}
	return;
}


static priQueue2TcontMap_t* omci_queueSearchById(UINT16 queueEntityId, UINT16 tcontEntityId)
{
	int i,j;
	for(i=0;i<TXC_TCONT_TCONT_NUM;i++)
		for(j=0;j<TXC_TCONT_PRIO_Q_PER_TCONT;j++)
		{
			if(q2TcontMap[i][j].queueEntityId == queueEntityId && \
			   q2TcontMap[i][j].tcontEntityId == tcontEntityId )
			{
				return &q2TcontMap[i][j];
			}
		}
	return NULL;
}

void omci_queueMapDump(void)
{
	
	int i,j;

	for(i=0;i<TXC_TCONT_TCONT_NUM;i++)
		for(j=0;j<TXC_TCONT_PRIO_Q_PER_TCONT;j++)
		{
			
			OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"=================================");
			OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"Tcont Num	: %d",i);			
			OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"ID 		: %d",q2TcontMap[i][j].tcontId);			
			OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"EntityId 	: 0x%x",q2TcontMap[i][j].tcontEntityId);
			OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"AllocId  	: %d",q2TcontMap[i][j].allocId);
			OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"Queue Num	: %d",j);
			OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"ID  		: %d",q2TcontMap[i][j].queueId);			
			OMCI_LOG(OMCI_LOG_LEVEL_HIGH,"EntityId 	: 0x%x",q2TcontMap[i][j].queueEntityId);			
		}
}


static UINT32 omci_GetQueueById(UINT16 entityId)
{
	int i,j;

	for(i=0;i<TXC_TCONT_TCONT_NUM;i++)
		for(j=0;j<TXC_TCONT_PRIO_Q_PER_TCONT;j++)
		{
			if(q2TcontMap[i][j].queueEntityId==entityId)
			{
				return q2TcontMap[i][j].queueId;
			}
		}
	return TXC_MAX_QUEUE_ID;
}



static UINT32 omci_TcontIdSet(UINT16 entityId, UINT16 tcontId, UINT16 allocId)
{
	int i,j;
	for(i=0;i<TXC_TCONT_TCONT_NUM;i++)
	{
		for(j=0;j<TXC_TCONT_PRIO_Q_PER_TCONT;j++){
			if(q2TcontMap[i][j].tcontEntityId == entityId)
			{
				q2TcontMap[i][j].tcontId = tcontId;				
				q2TcontMap[i][j].allocId = allocId;
				
			}
		}
	}
}


static int32 omci_ioctl_send(int opt,void *data,int len)
{
	int ret;
	rtk_gpon_extMsg_t ioctlMsg;
	
	ioctlMsg.len =len;
	ioctlMsg.optId = opt;
	memcpy(ioctlMsg.extValue,data,len);
#ifndef OMCI_X86
	ret = rtk_gpon_extMsg_get(&ioctlMsg);
#endif
	memcpy(data,ioctlMsg.extValue,len);
	
	return ret;

}



static void omci_logOpen(char *fname)
{
	pFile = fopen(fname,"w+");
}

static void omci_logClose(void)
{
	fclose(pFile);
}

static void omci_logMsg(uint8* msg)
{
	int i;
	if(pFile!=NULL){
		fprintf(pFile,"./omcicli sendpkt \"");
		for(i=0;i<48;i++){
			fprintf(pFile,"%02x ",msg[i]);
		}
		fprintf(pFile,"\"\n");
	}
}




static int omci_Ploam_Handler(rtk_gpon_ploam_t ploam)
{

       OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"PLOAM Handler ...");
    	return OK;
}


GOS_ERROR_CODE OMCI_waitGmacMsg(void)
{
    rtk_gpon_pkt_t gponPkt;
    fd_set		   fdSet;
    unsigned short len=0;
    FD_ZERO(&fdSet);
    FD_SET(gSimFd, &fdSet);
	
	omci_logOpen(fileName);

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Entering Message Loop ...");

    while (gOmciProcOn==TRUE)
    {
        if (select(gSimFd + 1, &fdSet, NULL, NULL, NULL))
        {
        #ifndef OMCI_X86
            if (ptk_redirect_userApp_recvPkt(gSimFd,128, &len,(unsigned char*)&gponPkt) > 0)
            {
            	OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"GPON PKT Type:%d\n",gponPkt.type);
                switch (gponPkt.type){
				case RTK_GPON_MSG_OMCI:
				{
					OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Send Msg to MSG Queue!\n");			
					if (logTofile) omci_logMsg(gponPkt.msg.omci.msg);
					OMCI_SendMsg(OMCI_APPL, OMCI_RX_OMCI_MSG, OMCI_MSG_PRI_NORMAL, 
					(void *)&gponPkt.msg.omci, sizeof(rtk_gpon_omci_msg_t));   
				}				
				break;
				case RTK_GPON_MSG_PLOAM:
				{
					omci_Ploam_Handler(gponPkt.msg.ploam);
				}				
				break;
				default:
				{
				}
				break;
				}
            }
		#endif
        }
    }

    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Leaving Message Loop ...");
	omci_logClose();
    return GOS_OK;
}



GOS_ERROR_CODE OMCI_createGmacDev(void)
{
	int ret;
	if (gSimFd < 0)
	{
	    if (-1 == (gSimFd = socket(PF_NETLINK,SOCK_RAW, NETLINK_USERSOCK)))
	    {
	        return GOS_FAIL;
	    }
	}
	OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Create Socket OK!,fd=%d\n",gSimFd);
	#ifndef OMCI_X86
	if((ret = ptk_redirect_userApp_reg(gSimFd,PR_USER_UID_GPONOMCI,128))!=0)
	{
		return GOS_FAIL;
	}
	#endif
	OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Register to PKT Redirect OK!\n");
	return GOS_OK;
}


GOS_ERROR_CODE OMCI_deleteGmacDev()
{   
    if (gSimFd >= 0)
    {
        close(gSimFd);

        gSimFd = -1;
    }
    return OK;
}



GOS_ERROR_CODE OMCI_wrapper_SendOmciMsg(void* rawMsg)
{
    int	ret;
    rtk_gpon_omci_msg_t omci;
    unsigned short len;
	
    len = sizeof(rtk_gpon_omci_msg_t);
    memset((void *)&omci, 0, len);
	if (logTofile) omci_logMsg(rawMsg);
    memcpy(omci.msg, rawMsg,sizeof(omci.msg));
	#ifndef OMCI_X86
    ret = ptk_redirect_userApp_sendPkt(gSimFd,PR_KERNEL_UID_GPONOMCI,0,len, &omci);
	#endif
	OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"Send OMCI Msg, ret =%d!\n",ret);

    if (ret < 0)
    {
        return GOS_FAIL;
    }
    return GOS_OK;
}



static void omci_RstServInfo(void)
{
    UINT16 servId;
    
    for(servId = 0; servId < OMCI_MAX_SERV_ID; servId++)
    {
        gOmciDsServInfo[servId].inUse = FALSE;
		LIST_INIT(&gOmciDsServInfo[servId].cfHead);
        gOmciUsServInfo[servId].inUse = FALSE;		
		LIST_INIT(&gOmciUsServInfo[servId].cfHead);
    }

    return;
}


static UINT16 omci_GetAvailDsServId(void)
{
    UINT16 servId;
    
    for(servId = 0; servId < OMCI_MAX_SERV_ID; servId++)
    {
        if (gOmciDsServInfo[servId].inUse == FALSE)
            break;
    }

    return servId;
}


static UINT16 omci_GetAvailUsServId(void)
{
    UINT16 servId;
    
    for(servId = 0; servId < OMCI_MAX_SERV_ID; servId++)
    {
        if (gOmciUsServInfo[servId].inUse == FALSE)
            break;
    }

    return servId;
}


static UINT16 omci_GetDsServIdByPortId(UINT16 igress, UINT16 ogress)
{
    UINT16 servId;
    
    for(servId = 0; servId < OMCI_MAX_SERV_ID; servId++)
    {
        if (gOmciDsServInfo[servId].inUse != FALSE && \
            gOmciDsServInfo[servId].igress == igress && \
            gOmciDsServInfo[servId].ogress == ogress)
            break;
    }

    return servId;
}


static UINT16 omci_GetUsServIdByPortId(UINT16 igress, UINT16 ogress)
{
    UINT16 servId;
    
    for(servId = 0; servId < OMCI_MAX_SERV_ID; servId++)
    {
        if (gOmciUsServInfo[servId].inUse != FALSE && \
            gOmciUsServInfo[servId].igress == igress && \
            gOmciUsServInfo[servId].ogress == ogress)
            break;
    }

    return servId;
}


static void omci_RstGemFlowUsg(void)
{
    UINT32 flowId;
    
    for(flowId = 0; flowId < TXC_ONT2G_GEM_PORT_NUM; flowId++)
    {
        gOmciGemDsFlowInfo[flowId].inUse = FALSE;
        gOmciGemUsFlowInfo[flowId].inUse = FALSE;
    }

    return;
}


static UINT16 omci_GetAvailDsFlowId(void)
{
    UINT16 flowId;
    
    for(flowId = OMCI_GEM_DATA_FLOW_OFFSET; flowId < TXC_ONT2G_GEM_PORT_NUM; flowId++)
    {
        if (gOmciGemDsFlowInfo[flowId].inUse == FALSE)
            break;
    }

    return (flowId == TXC_ONT2G_GEM_PORT_NUM ? TXC_ONT2G_GEM_PORT_NUM : flowId);
}


static UINT16 omci_GetAvailUsFlowId(void)
{
    UINT16 flowId;
    
    for(flowId = OMCI_GEM_DATA_FLOW_OFFSET; flowId < TXC_ONT2G_GEM_PORT_NUM; flowId++)
    {
        if (gOmciGemUsFlowInfo[flowId].inUse == FALSE)
            break;
    }

    return (flowId == TXC_ONT2G_GEM_PORT_NUM ? TXC_ONT2G_GEM_PORT_NUM : flowId);
}


static UINT16 omci_GetDsFlowIdByPortId(UINT16 portId)
{
    UINT16 flowId;
    
    for(flowId = OMCI_GEM_DATA_FLOW_OFFSET; flowId < TXC_ONT2G_GEM_PORT_NUM; flowId++)
    {
        if (gOmciGemDsFlowInfo[flowId].inUse != FALSE && gOmciGemDsFlowInfo[flowId].portId == portId)
            break;
    }

    return (flowId == TXC_ONT2G_GEM_PORT_NUM ? TXC_ONT2G_GEM_PORT_NUM : flowId);
}


static UINT16 omci_GetUsFlowIdByPortId(UINT16 portId)
{
    UINT16 flowId;
    
    for(flowId = OMCI_GEM_DATA_FLOW_OFFSET; flowId < TXC_ONT2G_GEM_PORT_NUM; flowId++)
    {
        if (gOmciGemUsFlowInfo[flowId].inUse != FALSE && gOmciGemUsFlowInfo[flowId].portId == portId)

            break;
    }

    return (flowId == TXC_ONT2G_GEM_PORT_NUM ? TXC_ONT2G_GEM_PORT_NUM : flowId);
}




/*for port and instance id mapping*/
static UINT16 omci_GetUNIPortByID(UINT16 id)
{
	switch(id)
	{
	case 0x501:
		return 0;
	break;
	case 0x502:
		return 1;
	break;
	case 0x503:
		return 2;
	break;
	case 0x504:
		return 3;
	break;
	default:
		return 0;
	break;
	}
	return -1;
}


GOS_ERROR_CODE OMCI_initDriv(void)
{
#ifdef __MDU_GMII__    
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"MDU_GMII initializing ... ");
#else /*__SFU_SSSMII__*/
    OMCI_LOG(OMCI_LOG_LEVEL_NORMAL,"SFU_SSSMII initializing ... ");
#endif /*__MDU_GMII__*/

    return GOS_OK;
}



GOS_ERROR_CODE OMCI_wrapper_CreateTcont(UINT16 entityId,UINT16 allocId, PON_US_TRAFF_MG_OPTION tmOpt, UINT16* tcontId)
{
	int32 ret;
	OMCI_TCONT_ts tcont;


	tcont.allocId = allocId;

	ret = omci_ioctl_send(OMCI_IOCTL_TCONT_GET,&tcont,sizeof(OMCI_TCONT_ts));

	*tcontId = tcont.tcontId;
	
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"%s() > allocId %d",__FUNCTION__,allocId);
    omci_TcontIdSet(entityId,tcont.tcontId,allocId);
    omci_priQUpdate(entityId);    
	
    OMCI_LOG(OMCI_LOG_LEVEL_LOW,"create a new tcont [%d], allocId [%d]\n", 
                *tcontId, allocId);
    return GOS_OK;
}



GOS_ERROR_CODE OMCI_wrapper_DeleteTcont(UINT16 entityId,UINT16 allocId)
{
    int32            ret;
	

	omci_TcontIdSet(entityId,0,0);

    if (ret != RT_ERR_OK)
    {
        return GOS_FAIL;
    }
    OMCI_LOG(OMCI_LOG_LEVEL_LOW,"create a new tcont, allocId [%d]\n", allocId);
	
    return GOS_OK;
}


GOS_ERROR_CODE OMCI_wrapper_CfgGemFlow(OMCI_GEM_FLOW_ts data)
{
	int ret,flowId;
	OMCI_TCONT_ts tcont;

	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Create/Delete Flow for Gem Port 0x%x in Direction[%d]", 
	data.portId, data.dir);
	char cmd[128]="";
	sprintf(cmd,"echo Create/Delete Flow for Gem Port 0x%x in Direction[%d] >>/tmp/a",data.portId, data.dir);

	system(cmd);

	if (data.dir == PON_GEMPORT_DIRECTION_US || data.dir == PON_GEMPORT_DIRECTION_BI)
	{
	  		
        	if (data.ena == TRUE)
        	{
		    	/* create a gem u/s flow */
		    	flowId = omci_GetUsFlowIdByPortId(data.portId);
			
		    	if (flowId != TXC_ONT2G_GEM_PORT_NUM)
		    	{
		        	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"flow already exist");
		        	return GOS_OK;
		    	}
	
			flowId = omci_GetAvailUsFlowId();
		    
		    	if (flowId == TXC_ONT2G_GEM_PORT_NUM)
		    	{
		        	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"not found available flow index");
		        	return GOS_FAIL;
		    	}

			tcont.allocId = data.allocId;
	
			ret = omci_ioctl_send(OMCI_IOCTL_TCONT_GET,&tcont,sizeof(OMCI_TCONT_ts));
			if(ret != OMCI_ERR_OK)
			{
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"not found tcont");
		        	return GOS_FAIL;
			}
			data.tcontPId = tcont.tcontId; 
			omci_TcontIdSet(data.tcontEId,data.tcontPId,data.allocId);
			omci_priQUpdate(data.tcontEId);  
			data.flowId = flowId;
			data.queuePId = omci_GetQueueById(data.queueEId);	
			ret = omci_ioctl_send(OMCI_IOCTL_GEMPORT_SET,&data,sizeof(OMCI_GEM_FLOW_ts));
			/*update priQ*/	
			if(ret == OMCI_ERR_OK)
			{      
            			gOmciGemUsFlowInfo[flowId].inUse  = TRUE;
            			gOmciGemUsFlowInfo[flowId].portId = data.portId;
            			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"create a u/s gem flow [%d]", flowId);
				
			}else{
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"create a u/s gem flow [%d] failled, ret = %d",
				flowId,ret);
			}
        	}
        	else 
        	{ 
        		/* delete a gem u/s flow */
        		flowId = omci_GetUsFlowIdByPortId(data.portId);

			if (flowId == TXC_ONT2G_GEM_PORT_NUM)
			{
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"given flow not found");
				return GOS_FAIL;
        		}
			data.flowId = flowId;
			ret = omci_ioctl_send(OMCI_IOCTL_GEMPORT_SET,&data,sizeof(OMCI_GEM_FLOW_ts));
			if(ret==OMCI_ERR_OK){
				gOmciGemUsFlowInfo[flowId].portId = 0;
            			gOmciGemUsFlowInfo[flowId].inUse = FALSE;
            			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"delete a u/s gem flow [%d]", flowId);
        		}
		}
	}

	if (data.dir == PON_GEMPORT_DIRECTION_DS || data.dir == PON_GEMPORT_DIRECTION_BI)
	{        
		if (data.ena == TRUE)
		{
			/* create a gem d/s flow */
			flowId = omci_GetDsFlowIdByPortId(data.portId);
        		if (flowId != TXC_ONT2G_GEM_PORT_NUM)
			{
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"flow already exist");
				return GOS_OK;
			}
        		flowId = omci_GetAvailDsFlowId();
			if (flowId == TXC_ONT2G_GEM_PORT_NUM)
			{
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"not found available flow index");
				return GOS_FAIL;
			}
			data.flowId = flowId;
			ret = omci_ioctl_send(OMCI_IOCTL_GEMPORT_SET,&data,sizeof(OMCI_GEM_FLOW_ts));
			if(ret==OMCI_ERR_OK){

				gOmciGemDsFlowInfo[flowId].inUse  = TRUE;
				gOmciGemDsFlowInfo[flowId].portId = data.portId;
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"create a d/s gem flow [%d]", flowId);
			}else{
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"create a d/s gem flow [%d] failed",flowId,ret);
			}

        	}else
		{
			/* delete a gem d/s flow */
			flowId = omci_GetDsFlowIdByPortId(data.portId);
			if (flowId == TXC_ONT2G_GEM_PORT_NUM)
			{
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"given flow not found");
				return GOS_FAIL;
			}
			/*config to chip*/
			ret = omci_ioctl_send(OMCI_IOCTL_GEMPORT_SET,&data,sizeof(OMCI_GEM_FLOW_ts));
			if(ret==OMCI_ERR_OK)
			{
				gOmciGemDsFlowInfo[flowId].inUse = FALSE;
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"delete a d/s gem flow [%d]", flowId);
			}
		}
	}
    	return GOS_OK;
}


GOS_ERROR_CODE OMCI_wrapper_SetPriQueue(MIB_TABLE_PRIQ_T *pPriQ)
{
	GOS_ERROR_CODE ret;
	priQueue2TcontMap_t *pQ2T;
	OMCI_PRIQ_ts priQ;
	MIB_TABLE_SCHEDULER_T scheduler;
	MIB_TABLE_TCONT_T tcont;
	UINT16 tcontId = (pPriQ->RelatedPort >> 16) & 0xffff;
	int direction; /*1: upstream 0:downstream*/
	int portId;

	memset(&tcont,0,sizeof(MIB_TABLE_TCONT_T));
	memset(&scheduler,0,sizeof(MIB_TABLE_SCHEDULER_T));
	memset(&priQ,0,sizeof(OMCI_PRIQ_ts));

	/*check direction*/
	if(pPriQ->EntityID > 0x7fff)
	{
		direction = PON_GEMPORT_DIRECTION_US;
	}

	/*scheduler policy priority > tcont policy*/
	if(direction==PON_GEMPORT_DIRECTION_US){
		/*start to check QoS part, follow implement guid G.988*/
		if(pPriQ->SchedulerPtr==0)
		{
			priQ.scheduleType = (tcont.Policy==2) ? PON_US_TRAFF_CELL_RATE : PON_US_TRAFF_PRI_SCHE;			
			priQ.weight = pPriQ->Weight;
		}else
		{
		
			scheduler.EntityID = pPriQ->SchedulerPtr;
			if((ret = MIB_Get(MIB_TABLE_SCHEDULER_INDEX,&scheduler,sizeof(MIB_TABLE_SCHEDULER_T)))!=GOS_OK)
			{
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Get Scheduler faild, ret %d",ret);
				return ret;
			}
			priQ.scheduleType = (scheduler.Policy == 2) ? PON_US_TRAFF_CELL_RATE : PON_US_TRAFF_PRI_SCHE;		
			priQ.weight = scheduler.PriWeight;
		}

		if(priQ.scheduleType==STRICT_PRIORITY)
		{
			/*assigned queue*/
		}
		
		tcont.EntityID  = tcontId;
		/*look Tcont policy*/
		if((ret = MIB_Get(MIB_TABLE_TCONT_INDEX,&tcont,sizeof(MIB_TABLE_TCONT_T)))!=GOS_OK)
		{			
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Get Tcont faild, ret %d",ret);
			return ret;
		}
		/*since allocated id is not assigned, do nothing*/
		if(tcont.AllocID==255)
		{
			return GOS_OK;
		}

		priQ.cir = 0;
		priQ.pir = 0x1ffff;
		
		pQ2T = omci_queueSearchById(pPriQ->EntityID,tcontId);
		/*for upstream*/
		if(!pQ2T)
		{
			return GOS_OK;
		}
		
		priQ.queueId = pQ2T->queueId;
		priQ.tcontId = pQ2T->tcontId;
		/*create queue first*/
		ret = omci_ioctl_send(OMCI_IOCTL_PRIQ_SET,&priQ,sizeof(priQ));
		if(ret!=OMCI_ERR_OK)
		{
			OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Get Tcont faild, ret %d",ret);
			return GOS_OK;
		}
		
	}
	else
	{
		portId = (pPriQ->RelatedPort >> 16) && 0x7;
		
	}
	return GOS_OK;
	
}



GOS_ERROR_CODE OMCI_wrapper_ActiveBdgConn(OMCI_TRAFF_RULE_ts *pRule)
{
#if 0

	static BOOL                         bdgInit = FALSE;
	int32    ret = GOS_OK;
	UINT16   servId;
	UINT16   portIn;
	UINT16   portOut;
	int i;


	if (bdgInit == FALSE)
	{
	    OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Initialize Bridge ... ");

	    bdgInit = TRUE;
	}

	OMCI_wrapper_DeactiveBdgConn(data);


	/*port mapping*/
	if (data.dir == PON_GEMPORT_DIRECTION_US)
	{
	    portIn  = omci_GetUNIPortByID(data.ingress);
	    portOut = data.outgress;	
		servId =omci_GetAvailUsServId();
		data.servId = servId;
		ret = omci_ioctl_send(OMCI_IOCTL_CF_ADD,&data,sizeof(OMCI_TRAFF_RULE_ts));

		if(ret==OMCI_ERR_OK)
		{
			gOmciUsServInfo[servId].inUse   = TRUE;
	    	gOmciUsServInfo[servId].igress  = portIn;
	    	gOmciUsServInfo[servId].ogress  = portOut;
		}

	}
	else if (data.dir == PON_GEMPORT_DIRECTION_DS)
	{
	    portIn  = data.ingress;
	    portOut = omci_GetUNIPortByID(data.outgress);
		servId = omci_GetAvailDsServId();
		data.servId = servId;
		ret = omci_ioctl_send(OMCI_IOCTL_CF_ADD,&data,sizeof(OMCI_TRAFF_RULE_ts));

		if(ret==OMCI_ERR_OK)
		{
			gOmciDsServInfo[servId].inUse	= TRUE;
			gOmciDsServInfo[servId].igress  = portIn;
			gOmciDsServInfo[servId].ogress  = portOut;
		}
	}
	
	OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Active Bridge Connection, IN : %d, OUT : %d", portIn, portOut);
#endif
	return GOS_OK;
}


GOS_ERROR_CODE OMCI_wrapper_DeactiveBdgConn(OMCI_TRAFF_RULE_ts data)
{
#if 0
    int32 ret = RT_ERR_OK;

    UINT16 portIn;
    UINT16 portOut;
    UINT16 servId;
	omci_cf_rule_t *pCfRule;
	omci_serv_info_ts *pServ;

    if (data.dir == PON_GEMPORT_DIRECTION_US)
    {
	    portIn  = omci_GetUNIPortByID(data.ingress);
        portOut = data.outgress;
    }
    else if (data.dir == PON_GEMPORT_DIRECTION_DS)
    {
        portIn  = data.ingress;
	    portOut = omci_GetUNIPortByID(data.outgress);
    }

    OMCI_LOG(OMCI_LOG_LEVEL_LOW,"De - Active Bridge Connection, IN : %d, OUT : %d", portIn, portOut);

    if (data.dir == PON_GEMPORT_DIRECTION_DS)
    {
        servId = omci_GetDsServIdByPortId(portIn, portOut);

        if (servId == OMCI_MAX_SERV_ID)
        {
            OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Not Found D/S Matched Service Id");

            return GOS_OK;
        }
		pServ = &gOmciDsServInfo[servId];
    }
    else if (data.dir == PON_GEMPORT_DIRECTION_US)
    {
        servId = omci_GetUsServIdByPortId(portIn, portOut);

        if (servId == OMCI_MAX_SERV_ID)
        {
            OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Not Found U/S Matched Service Id");

            return GOS_OK;
        }
        
        pServ = &gOmciUsServInfo[servId];
    }
    else
    {
        OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Unsupported Traffic");
        
        return GOS_OK;
    }

    
    OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Del Class Rule ---- > ");

    if(pServ!=NULL)
    {
    	pServ->inUse = FALSE;
		for(pCfRule=LIST_FIRST(&pServ->cfHead);pCfRule!=NULL;pCfRule=LIST_NEXT(pCfRule,entries))
		{

			ret = omci_ioctl_send(OMCI_IOCTL_CF_DEL,&pCfRule->cfRuleId,sizeof(pCfRule->cfRuleId));
			if (ret != RT_ERR_OK)
			{
				OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Del Class Rule < ---- FAIL");
				
				return GOS_FAIL;
			}
		}
    }
    
    LIST_INIT(&pServ->cfHead);
    
    OMCI_LOG(OMCI_LOG_LEVEL_LOW,"Del Class Rule < ---- OK");
#endif    
    return GOS_OK;
}


GOS_ERROR_CODE OMCI_wrapper_SetSerialNum(char *serial)
{
	int ret;
	ret = omci_ioctl_send(OMCI_IOCTL_SN_SET,serial,16);
	return GOS_OK;
}


GOS_ERROR_CODE OMCI_wrapper_GetSerialNum(char *serial)
{
	int ret;
	ret = omci_ioctl_send(OMCI_IOCTL_SN_GET,serial,16);
	return GOS_OK;
}

GOS_ERROR_CODE OMCI_wrapper_ResetMib(void)
{
	int ret;
	ret = omci_ioctl_send(OMCI_IOCTL_MIB_RESET,NULL,0);
	return GOS_OK;
	
}
