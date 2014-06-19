#include	"uemf.h"
#include	"wsIntrn.h"
#include	<signal.h>
#include	<unistd.h>
#include	<time.h>
#include	<sys/types.h>
#include	<debug.h>
#include 	<config.h>
#include	<syslog.h>

#include	<cable.h>
#include	<ipc_client.h>
#include	<hex_utils.h>
#include	<str_utils.h>
#include 	"webs_nvram.h"
#include	"asp_variable.h"
#include    "pdt_config.h"
#include 	<ipc_protocol.h>
#include 	"memshare.h"
#include    <lw_config_oid.h>
#include    <sys/ioctl.h>
#include    <net/if.h>
#include    "shutils.h"
#include    <time_zone.h>
#include    <lw_config_api.h>
#include    <lw_drv_pub.h>
#include    <lw_drv_req.h>
#include    <vlan_utils.h>
#include	"asp_variable.h"  


extern int ipc_fd;
extern int port_msk;


struct cgi_handle_t {
	char *cmd;
	int (*request_handle)(webs_t wp, cgi_result_t *result);
};

/* UNI口统计数据 */
typedef struct UNI_COUNTER
{
    /* 正确报文统计 */
    ULONG InUnicast;    /* 接收单播报文 */
    ULONG InMulticast;  /* 接收多播报文 */
    ULONG InBroadcast;  /* 接收广播报文 */
    ULONG InPause;      /* 接收流控报文 */
    ULONG OutUnicast;   /* 发送单播报文 */
    ULONG OutMulticast; /* 发送多播报文 */
    ULONG OutBroadcast; /* 发送广播报文 */
    ULONG OutPause;     /* 发送流控报文 */

    /* 错误报文统计 */
    ULONG InError;      /* 接收错误 */
    ULONG InOverflow;   /* 报文过大 */
    ULONG InUnderflow;  /* 报文过小 */
    ULONG InCrcError;   /* CRC错误  */
    ULONG OutError;     /* 发送错误 */
    ULONG OutCollision; /* 冲突     */
    ULONG OutLate;      /* 时延     */
    ULONG ulInDropped;  /* 接收丢弃 */
    ULONG ulInFilter;   /* 接收过滤 */
    ULONG ulOutFilter;  /* 发送过滤 */


    /* 字节数统计 */
    UINT64  uiRecOctets;        /* 接收到的正确字节总数*/
    ULONG   ulRevBadOctets;     /* 接收到的错误字节数 */
    UINT64  uiTransOctets;      /* 发送的字节总数 */
} UNI_COUNTER_S;

static int sys_strom_getconfig(int *valid, char *sval, char *cp, STROM_CTL_S *req, int *portmsk)
{
#define STORM_UUCAST  0x01
#define STORM_UMCAST  0x02
#define STORM_BCAST   0x04
#define STORM_RATE    0x08
#define PORT_MASK     0x10
#define STORM_CHK     0x1f 

    if (!(*valid & STORM_UUCAST) && !strcmp("uucasten", sval))
    {
        *valid |= STORM_UUCAST;
        req->uucasten = strtoul(cp, NULL, 0);
        if ((0 != req->uucasten) && (1 != req->uucasten))
        {
            DBG_ASSERT(0, "Invalid uucasten mode");
            return 0;
        }
    }
    else if (!(*valid & STORM_UMCAST) && !strcmp("umcasten", sval))
    {
        *valid |= STORM_UMCAST;
        req->umcasten = strtoul(cp, NULL, 0);
        if ((0 != req->umcasten) && (1 != req->umcasten))
        {
            DBG_ASSERT(0, "Invalid umcasten mode");
            return 0;
        }        
    }
    else if (!(*valid & STORM_BCAST) && !strcmp("bcasten", sval))
    {
        *valid |= STORM_BCAST;
        req->bcasten = strtoul(cp, NULL, 0);
        if ((0 != req->bcasten) && (1 != req->bcasten))
        {
            DBG_ASSERT(0, "Invalid bcasten mode");
            return 0;
        } 
    }
    else if (!(*valid & STORM_RATE) && !strcmp("rate", sval))
    {
        *valid |= STORM_RATE;
        req->ratepercent = strtoul(cp, NULL, 0);
        if ((5 != req->ratepercent) && (10 != req->ratepercent)&&
            (20 != req->ratepercent)&& (100 != req->ratepercent))
        {
            DBG_ASSERT(0, "Invalid storm rate");
            return 0;
        } 
    }
    else if (!(*valid & PORT_MASK) && !strcmp("portmask", sval))
    {
        *valid |= PORT_MASK;
        *portmsk = strtoul(cp, NULL, 0);
        if (0 == *portmsk)
        {
            DBG_ASSERT(0, "Invalid storm portmsk");
            return 0;
        } 
    }

    return STORM_CHK;

}
int sys_getstormCurCfg(STROM_CTL_S * storm_cur)
{
	int ret = 0;
	STROM_CTL_S stormcfg[LOGIC_PORT_NO] = {0};
	ipc_stormctl_ack_t *pack = NULL;

	memset(stormcfg, 0, sizeof(STROM_CTL_S)*LOGIC_PORT_NO);

	pack = ipc_port_stormctl(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) 
	{
		memcpy(stormcfg, pack->stormctl, sizeof(STROM_CTL_S)*LOGIC_PORT_NO);
		ret = IPC_STATUS_OK;
	}
	else if (pack)
	{
		ret = pack->hdr.status;
	}
	else 
	{
		ret = IPC_STATUS_FAIL;
	}

	if(pack) 
	{	
		free(pack); 	
	}
	if(ret != IPC_STATUS_OK)
	{
		DBG_ASSERT(0, "Get STORM CTRL faild");
		return 0;
	}

    memcpy(storm_cur, stormcfg, sizeof(STROM_CTL_S)*LOGIC_PORT_NO);
    return 1;
}


int asp_get_single_stormctl(int eid, webs_t wp, int argc, char_t **argv)
{
    int i = 0;
    unsigned char cnt = 0;
    unsigned char arrayidx =0;
	STROM_CTL_S stormctl_cur[LOGIC_PORT_NO] = {0};


    for(i=0 ; i< LOGIC_PORT_NO; i++)
    {
        if(port_msk & (1<<i))
        {
            cnt++;
            arrayidx = i;
        }
    }

    if(1 != cnt)
    {
         websWrite(wp, T("'1','1','1','100','%d'"),port_msk);
         port_msk = 0;
         return 1;
    }
	
    if(1 != sys_getstormCurCfg(stormctl_cur))
    {
        DBG_PRINTF("Get port current stormctl error.\n");
		websWrite(wp, T("'0','0','0','0','0'"));
        port_msk = 0;
        return 1;
    }

    websWrite(wp, T("'%d','%d','%d','%d','%d'"), 
    stormctl_cur[arrayidx].uucasten, stormctl_cur[arrayidx].umcasten,
    stormctl_cur[arrayidx].bcasten, stormctl_cur[arrayidx].ratepercent,
    port_msk
	);
    port_msk = 0;
    
    return 1;

}

int sys_stormrate_handle(webs_t wp, cgi_result_t *result)
{
    STROM_CTL_S req;
    int ret = 0;
	int i = 0;
    int valid = 0;
    int count = 0;
	int portmsk = 0;
	char *cp = NULL;
    char *sval = NULL;
    char svar[10] = {0};  
	ipc_stormctl_ack_t *pack = NULL;
	STROM_CTL_S storm_cur[LOGIC_PORT_NO] = {0};
    memset(&req, 0, sizeof(req));

	while(1) 
    {
		sprintf(svar, "SET%d", count);
		sval = websGetVar(wp, svar, NULL);
		if(!sval)
        {      
            break;
        }
		cp = strchr(sval, '=');
		if (!cp)
        {   
            break;
        }
		*cp = 0;
        cp ++;
		if (!(ret = sys_strom_getconfig(&valid, sval, cp, &req, &portmsk)))
        {
			break;
		}
		if ((valid & ret) == ret)
        {
			break;
		}		
		count ++;
	}	

	if ((valid & ret) != ret)
    {
		DBG_PRINTF("Invalid Form StormCtl Command:%04X\n", valid);
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);	
		return 0;
	}

	if(1 != sys_getstormCurCfg(storm_cur))
	{
		DBG_PRINTF("Get port current storm error.\n");
		SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);	
		return 0;
	}

	for(i=0 ; i< LOGIC_PORT_NO; i++)
	{
		if(portmsk & (1<<i))
		{
			memcpy(&(storm_cur[i]), &req, sizeof(STROM_CTL_S));
		}
	}

	
	pack = ipc_port_stormctl(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, storm_cur);
	IPC_RESULT(pack, result);
	if(pack)
    {
	    free(pack);
	}
	return 0;

}

int asp_get_stormctl_config(int eid, webs_t wp, int argc, char_t **argv)
{
     int ret = 0;
	 int arrayidx = 0;
     STROM_CTL_S stormcfg[LOGIC_PORT_NO] = {0};
     ipc_stormctl_ack_t *pack = NULL;

     memset(stormcfg, 0, sizeof(STROM_CTL_S)*LOGIC_PORT_NO);
     
     pack = ipc_port_stormctl(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
     if (pack && pack->hdr.status == IPC_STATUS_OK) 
     {
		 memcpy(stormcfg, pack->stormctl, sizeof(STROM_CTL_S)*LOGIC_PORT_NO);
		 ret = IPC_STATUS_OK;

     }
     else if (pack)
     {
         ret = pack->hdr.status;
     }
     else 
     {
         ret = IPC_STATUS_FAIL;
     }
     
     if(pack) 
     {   
         free(pack);     
     }
     if(ret != IPC_STATUS_OK)
     {
         DBG_ASSERT(0, "Get STORM CTRL faild");
         return 0;
     }
      
	 for(arrayidx = 0; arrayidx < LOGIC_PORT_NO - 1; arrayidx++)
	 {
	     websWrite(wp, T("'%d','%d','%d','%d',"), stormcfg[arrayidx].uucasten, 
	        stormcfg[arrayidx].umcasten, stormcfg[arrayidx].bcasten, stormcfg[arrayidx].ratepercent);
	 }
	 
     websWrite(wp, T("'%d','%d','%d','%d'"), stormcfg[arrayidx].uucasten, 
        stormcfg[arrayidx].umcasten, stormcfg[arrayidx].bcasten, stormcfg[arrayidx].ratepercent);
	 
	 return 1;

}

INT32 GetPortMibCounter(UINT32 lport, UNI_COUNTER_S*  pMibCtrs, ULONG *RxPacket, ULONG *TxPacket)
{
    int ret;
	UINT64 ullValue = 0;

    if(NULL == pMibCtrs)
    {
        return ERROR;
    }

    if(FALSE == IsValidLgcPort(lport))
    {
        return ERROR;
    }

    ret = DRV_OK;

    ret+=Ioctl_GetPortStatisticsByPort(lport, TxOctets, &ullValue);
	pMibCtrs->uiTransOctets = ullValue;
    ret+=Ioctl_GetPortStatisticsByPort(lport, RxOctets, &ullValue);
	pMibCtrs->uiRecOctets = ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxGoodOctets, &ullValue);
    pMibCtrs->ulRevBadOctets = (ULONG)(pMibCtrs->uiRecOctets - ullValue);

    ret+=Ioctl_GetPortStatisticsByPort(lport, RxUnicastPkts, &ullValue);
	pMibCtrs->InUnicast = (ULONG)ullValue;
    ret+=Ioctl_GetPortStatisticsByPort(lport, TxUnicastPkts, &ullValue);
	pMibCtrs->OutUnicast = (ULONG)ullValue;
    ret+=Ioctl_GetPortStatisticsByPort(lport, RxMulticastPkts, &ullValue);
	pMibCtrs->InMulticast = (ULONG)ullValue;
    ret+=Ioctl_GetPortStatisticsByPort(lport, TxMulticastPkts, &ullValue);
	pMibCtrs->OutMulticast = (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxBroadcastPkts, &ullValue);
	pMibCtrs->InBroadcast = (ULONG)ullValue;
    ret+=Ioctl_GetPortStatisticsByPort(lport, TxBroadcastPkts, &ullValue);
    pMibCtrs->OutBroadcast = (ULONG)ullValue;

	ret+=Ioctl_GetPortStatisticsByPort(lport, RxPausePkts, &ullValue);
	pMibCtrs->InPause = (ULONG)ullValue;
    ret+=Ioctl_GetPortStatisticsByPort(lport, TxPausePkts, &ullValue);
	pMibCtrs->OutPause = (ULONG)ullValue;

	ret+=Ioctl_GetPortStatisticsByPort(lport, RxFCSErrors, &ullValue);
	pMibCtrs->InCrcError = (ULONG)ullValue;	
    ret+=Ioctl_GetPortStatisticsByPort(lport, RxUdersizePkts, &ullValue);
	pMibCtrs->InUnderflow = (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxOversizePkts, &ullValue);
	pMibCtrs->InOverflow = (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxDropPkts, &ullValue);
    pMibCtrs->ulInDropped = (ULONG)ullValue;
	pMibCtrs->InError = pMibCtrs->InUnderflow + pMibCtrs->InOverflow + pMibCtrs->InCrcError + pMibCtrs->ulInDropped;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxFragment, &ullValue);
	pMibCtrs->InUnderflow += (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxJabbers, &ullValue);
	pMibCtrs->InOverflow += (ULONG)ullValue;		

	ret+=Ioctl_GetPortStatisticsByPort(lport, TxSingleCollision, &ullValue);
	pMibCtrs->OutCollision = (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, TxLateCollision, &ullValue);
	pMibCtrs->OutLate = (ULONG)ullValue;
    
    pMibCtrs->OutError = pMibCtrs->OutCollision + pMibCtrs->OutLate;

	ret+=Ioctl_GetPortStatisticsByPort(lport, RxUdersizePkts, &ullValue);
	*RxPacket = (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxFragment, &ullValue);
	*RxPacket += (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxPkt64Octets, &ullValue);
	*RxPacket += (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxPkt65to127Octets, &ullValue);
	*RxPacket += (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxPkt128to255Octets, &ullValue);
	*RxPacket += (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxPkt256to511Octets, &ullValue);
	*RxPacket += (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxPkt512to1023Octets, &ullValue);
	*RxPacket += (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxPkt1024to1518octets, &ullValue);
	*RxPacket += (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, RxPkt1519toMAXoctets, &ullValue);
	*RxPacket += (ULONG)ullValue;

	ret+=Ioctl_GetPortStatisticsByPort(lport, TxUdersizePkts, &ullValue);
	*TxPacket = (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, TxPkt64Octets, &ullValue);
	*TxPacket += (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, TxPkt65to127Octets, &ullValue);
	*TxPacket += (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, TxPkt128to255Octets, &ullValue);
	*TxPacket += (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, TxPkt256to511Octets, &ullValue);
	*TxPacket += (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, TxPkt512to1023Octets, &ullValue);
	*TxPacket += (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, TxPkt1024to1518octets, &ullValue);
	*TxPacket += (ULONG)ullValue;
	ret+=Ioctl_GetPortStatisticsByPort(lport, TxPkt1519toMAXoctets, &ullValue);
	*TxPacket += (ULONG)ullValue;
        
    if(DRV_OK == ret){
        return NO_ERROR;
    }
    else{
        return ERROR;
    }

}


typedef struct tagPORT_MIB_VALUE
{
    CHAR val[BUF_SIZE32];
} PORT_MIB_VALUES;

#define MIBNUM 12

int asp_get_PortMibCounter(int eid, webs_t wp, int argc, char_t **argv)
{
	int i = 0;
	UNI_COUNTER_S mibCounters;
	unsigned int lport = 0;
	unsigned int curport = 0;
	unsigned int arrayidx = 0;
	ULONG inputpkt = 0;
	ULONG outputpkt = 0;
    PORT_MIB_VALUES MIBcount[12] = {0};
    
	memset(&mibCounters, 0, sizeof(UNI_COUNTER_S));
	
    for(i=0 ; i< LOGIC_PORT_NO; i++)
    {
        if(port_msk & (1<<i))
        {
          curport = i+1;
        }
    }

	port_msk = 0;
	if(0 == curport)
	{
		curport = 1;
	}

	for(arrayidx = 0; arrayidx < LOGIC_PORT_NO; arrayidx++)
	{
		lport = arrayidx + 1;
		inputpkt = 0;
		outputpkt = 0;
		memset(&mibCounters, 0, sizeof(UNI_COUNTER_S));
		GetPortMibCounter(lport, &mibCounters, &inputpkt, &outputpkt);
		//inputpkt = mibCounters.InUnicast + mibCounters.InBroadcast + mibCounters.InMulticast;
	    //outputpkt = mibCounters.OutUnicast + mibCounters.OutBroadcast + mibCounters.OutMulticast;

        memset(MIBcount, 0, sizeof(PORT_MIB_VALUES)*MIBNUM);
        sprintf(MIBcount[0].val, "%ld", inputpkt);
        sprintf(MIBcount[1].val, "%ld", outputpkt);
        sprintf(MIBcount[2].val, "%lld",mibCounters.uiRecOctets);
        sprintf(MIBcount[3].val, "%lld", mibCounters.uiTransOctets);
        sprintf(MIBcount[4].val, "%ld", mibCounters.InBroadcast);
        sprintf(MIBcount[5].val, "%ld", mibCounters.OutBroadcast);
        sprintf(MIBcount[6].val, "%ld", mibCounters.InMulticast);
        sprintf(MIBcount[7].val, "%ld", mibCounters.OutMulticast);
        sprintf(MIBcount[8].val, "%ld", mibCounters.InPause);
        sprintf(MIBcount[9].val, "%ld", mibCounters.OutPause);
        sprintf(MIBcount[10].val, "%ld", mibCounters.InError);
        sprintf(MIBcount[11].val, "%ld", mibCounters.OutError);
        
        websWrite(wp, T("'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',"),
        MIBcount[0].val,MIBcount[1].val,MIBcount[2].val,MIBcount[3].val,
        MIBcount[4].val,MIBcount[5].val,MIBcount[6].val,MIBcount[7].val,
        MIBcount[8].val,MIBcount[9].val,MIBcount[10].val,MIBcount[11].val);
        #if 0
        websWrite(wp, T("'%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld',"),
        inputpkt,outputpkt,mibCounters.uiRecOctets,mibCounters.uiTransOctets,
        mibCounters.InBroadcast,mibCounters.OutBroadcast,
        mibCounters.InMulticast,mibCounters.OutMulticast,
        mibCounters.InPause,mibCounters.OutPause,
        mibCounters.InError,mibCounters.OutError);
        #endif
	}
	websWrite(wp, T("'%d'"), curport);

    return 1;	
}




/*Begin add by huangmingjian 2013-08-27*/

#ifdef CONFIG_PRODUCT_5500
extern ponStatus_webDis_t *gm_ponStatus;

static int getDigit(unsigned short usNumber)
{
	int i = 0;
    unsigned short usTempNum = usNumber;
	while(usTempNum)
	{
		usTempNum = usTempNum/10;
		i++;
	}
	return i;
}
static double findPower(double dPowerData)
{
	int i;
	double adPower[100][2] = {
				0.100000 , -1.000000 ,
				0.200000 , -0.698970 ,
				0.300000 , -0.522879 ,
				0.400000 , -0.397940 ,
				0.500000 , -0.301030 ,
				0.600000 , -0.221849 ,
				0.700000 , -0.154902 ,
				0.800000 , -0.096910 ,
				0.900000 , -0.045757 ,
				1.000000 , -0.000000 ,
				1.100000 , 0.041393 ,
				1.200000 , 0.079181 ,
				1.300000 , 0.113943 ,
				1.400000 , 0.146128 ,
				1.500000 , 0.176091 ,
				1.600000 , 0.204120 ,
				1.700000 , 0.230449 ,
				1.800000 , 0.255273 ,
				1.900000 , 0.278754 ,
				2.000000 , 0.301030 ,
				2.100000 , 0.322219 ,
				2.200000 , 0.342423 ,
				2.300000 , 0.361728 ,
				2.400000 , 0.380211 ,
				2.500000 , 0.397940 ,
				2.600000 , 0.414973 ,
				2.700000 , 0.431364 ,
				2.800000 , 0.447158 ,
				2.900000 , 0.462398 ,
				3.000000 , 0.477121 ,
				3.100000 , 0.491362 ,
				3.200000 , 0.505150 ,
				3.300000 , 0.518514 ,
				3.400000 , 0.531479 ,
				3.500000 , 0.544068 ,
				3.600000 , 0.556303 ,
				3.700000 , 0.568202 ,
				3.800000 , 0.579784 ,
				3.900000 , 0.591065 ,
				4.000000 , 0.602060 ,
				4.100000 , 0.612784 ,
				4.200000 , 0.623249 ,
				4.300000 , 0.633468 ,
				4.400000 , 0.643453 ,
				4.500000 , 0.653213 ,
				4.600000 , 0.662758 ,
				4.700000 , 0.672098 ,
				4.800000 , 0.681241 ,
				4.900000 , 0.690196 ,
				5.000000 , 0.698970 ,
				5.100000 , 0.707570 ,
				5.200000 , 0.716003 ,
				5.300000 , 0.724276 ,
				5.400000 , 0.732394 ,
				5.500000 , 0.740363 ,
				5.600000 , 0.748188 ,
				5.700000 , 0.755875 ,
				5.800000 , 0.763428 ,
				5.900000 , 0.770852 ,
				6.000000 , 0.778151 ,
				6.100000 , 0.785330 ,
				6.200000 , 0.792392 ,
				6.300000 , 0.799341 ,
				6.400000 , 0.806180 ,
				6.500000 , 0.812913 ,
				6.600000 , 0.819544 ,
				6.700000 , 0.826075 ,
				6.800000 , 0.832509 ,
				6.900000 , 0.838849 ,
				7.000000 , 0.845098 ,
				7.100000 , 0.851258 ,
				7.200000 , 0.857332 ,
				7.300000 , 0.863323 ,
				7.400000 , 0.869232 ,
				7.500000 , 0.875061 ,
				7.600000 , 0.880814 ,
				7.700000 , 0.886491 ,
				7.800000 , 0.892095 ,
				7.900000 , 0.897627 ,
				8.000000 , 0.903090 ,
				8.100000 , 0.908485 ,
				8.200000 , 0.913814 ,
				8.300000 , 0.919078 ,
				8.400000 , 0.924279 ,
				8.500000 , 0.929419 ,
				8.600000 , 0.934498 ,
				8.700000 , 0.939519 ,
				8.800000 , 0.944483 ,
				8.900000 , 0.949390 ,
				9.000000 , 0.954243 ,
				9.100000 , 0.959041 ,
				9.200000 , 0.963788 ,
				9.300000 , 0.968483 ,
				9.400000 , 0.973128 ,
				9.500000 , 0.977724 ,
				9.600000 , 0.982271 ,
				9.700000 , 0.986772 ,
				9.800000 , 0.991226 ,
				9.900000 , 0.995635 ,
				10.000000 , 1.000000
				};
	if(dPowerData <0.0 || dPowerData >10.0)
		return 0.0;

	for(i = 0; i<100; i++)
	{
		if(dPowerData - adPower[i][0] < 0.000001 && dPowerData - adPower[i][0] > -0.000001)
			return adPower[i][1];
	}
	return 0.0;
}
double GetOptics(unsigned short usPowerData)
{
	int iDigit;
	double dPowerData;
    double dPower;

	iDigit= getDigit(usPowerData);

	switch(iDigit)
	{
	case 1:
		dPowerData = (double)usPowerData/10.0;
		dPower = 10*(findPower(dPowerData) + 1.0 -4.0);
		break;
	case 2:
		dPowerData = (double)((unsigned short)((usPowerData % 10)/1))/10.0;
        /*lint --e{653}*/
		dPowerData = dPowerData+ (double)(usPowerData/10);
		dPower = 10*(findPower(dPowerData) + 1.0 -4.0);
		break;
	case 3:
		dPowerData = (double)((unsigned short)((usPowerData % 100)/10))/10.0;
		dPowerData = dPowerData+ (double)(usPowerData/100);
		dPower = 10*(findPower(dPowerData) + 2.0 -4.0);
		break;
	case 4:
		dPowerData = (double)((unsigned short)((usPowerData % 1000)/100))/10.0;
		dPowerData = dPowerData+ (double)(usPowerData/1000);
		dPower= 10*(findPower(dPowerData) + 3.0 -4.0);
		break;
	case 5:
		dPowerData= (double)((unsigned short)((usPowerData % 10000)/1000))/10.0;
		dPowerData= dPowerData+ (double)(usPowerData/10000);
		dPower= 10*(findPower(dPowerData) + 4.0 -4.0);
		break;
	default:
        dPower = 0.0;
		break;
	}
    /*lint -restore */
	return dPower;
}


int asp_get_PonStatus(int eid, webs_t wp, int argc, char_t **argv)
{	
	#define MAX_TIME 1
	ponStatus_webDis_t *info = gm_ponStatus;
	int i = 0;
	rtk_transceiver_data_app_t para;
	float fData = 0;
	USHORT usData = 0;
	char buf[16] = {0};

	websWrite(wp, T("%s"), "'");
	for(i=RTK_TRANSCEIVER_PARA_TYPE_TEMPERATURE_; i<RTK_TRANSCEIVER_PARA_TYPE_MAX_; i++)
	{
		memset(&para, 0, sizeof(para));
		if(0 != Ioctl_GetPonmacTransceiver(i, &para))
		{
			websWrite(wp, T("%s;"), ERROR_DISPLAY);
			DBG_PRINTF("Get pon paras fail\n");
		}
		else 
		{
			if( i == RTK_TRANSCEIVER_PARA_TYPE_TEMPERATURE_)
			{
				if (128 >= para.buf[0]) 
				{
					fData = (-1)*((~(para.buf[0]))+1)+((double)(para.buf[0])*1/256);
				}
				else
				{  
					fData = para.buf[0]+((double)(para.buf[0])*1/256);
				}
			}
			else if( i == RTK_TRANSCEIVER_PARA_TYPE_VOLTAGE_)
			{
				fData = (double)((para.buf[0] << 8) | para.buf[0])*1/10000;
			}		
			else
			{	
				usData = (para.buf[0] << 8U) | para.buf[1];
				fData = (FLOAT)GetOptics(usData);
				//fData = log10(((double)((para.buf[0] << 8) | para.buf[1])*1/10000))*10;
			}
			sprintf(buf, "%.2f", fData);
			websWrite(wp, T("%s;"), buf);
			
		}

		DBG_PRINTF("%d: 0x%2x 0x%2x, %.2f\n", i, para.buf[0], para.buf[1], fData);
	}

	if(!info->isOnline)
	{	
		for(i=0;i<MAX_TIME; i++)
		{
			sleep(1);
			if(info->isOnline == 1)
			{	
				break;
			}
		}
		if(MAX_TIME == i)
		{	
			websWrite(wp, T("%d;%s;%s;%s;%s;%s'"),
							info->isOnline,
							ERROR_DISPLAY,
							ERROR_DISPLAY,
							ERROR_DISPLAY,
							ERROR_DISPLAY,
							ERROR_DISPLAY);
			return 0;
		}


	}

	if(info->remoteInfovalid)
	{	
		websWrite(wp, T("%d;%s;%s;%s;%s;%s'"),
						info->isOnline,
						"",
						info->ctcVer,
						info->oamVer,
						info->oui,
						info->venderSpecInfo);
	}
	else
	{
		websWrite(wp, T("%d;%s;%s;%s;%s;%s'"),
						info->isOnline,
						"",
						info->ctcVer,
						ERROR_DISPLAY,
						ERROR_DISPLAY,
						ERROR_DISPLAY);
	}	
	
	info->isOnline = 0;
	return 0;
}


int asp_get_PonMibCounter(int eid, webs_t wp, int argc, char_t **argv)
{
	int i = 0;
	rtk_epon_counter_app_t Counter;
	
	memset(&Counter, 0, sizeof(rtk_epon_counter_app_t));
	Counter.llidIdx = 0;
	if(DRV_OK != Ioctl_Get_Epon_Mib_Counter(&Counter))
	{	
		DBG_PRINTF("Get Mib Counter fail\n");
		return 1;
	}
	websWrite(wp, T("%s"), "'");

    for(i=0;i<8;i++)
    {
    	websWrite(wp, T("%d;"), Counter.llidIdxCnt.queueTxFrames[i]);
    }
	websWrite(wp, T(";%d;%d;%d;%d;%d;%d;%d;%d;%d'"), 
					Counter.llidIdxCnt.mpcpTxReport,
					Counter.llidIdxCnt.mpcpRxGate,
					Counter.llidIdxCnt.onuLlidNotBcst,
					Counter.mpcpRxDiscGate,
					Counter.mpcpTxRegRequest,
					Counter.crc8Err,
					Counter.notBcstBitNotOnuLlid,
					Counter.bcstBitPlusOnuLLid,
					Counter.bcstNotOnuLLid);

    return 1;	
}

int sys_pon_count_reset_handle(webs_t wp, cgi_result_t *result)
{
	return Ioctl_EponMibGlobalReset();
}

#endif
/*End add by huangmingjian 2013-08-27*/

int asp_get_linerate(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
    int arrayidx = 0;
	ipc_linerate_ack_t *pack = NULL;
    PORT_LINERATE_VALUES linerate_val[LOGIC_PORT_NO];

	pack = ipc_linerate_req(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) 
    {
        memcpy(linerate_val, pack->linerate_val, sizeof(PORT_LINERATE_VALUES)*LOGIC_PORT_NO);
		ret = IPC_STATUS_OK;
	}
    else if (pack)
    {
		ret = pack->hdr.status;
	}
    else 
    {
		ret = IPC_STATUS_FAIL;
	}
    
	if(pack) 
    {   
        free(pack);     
    }
    
    if(ret != IPC_STATUS_OK)
    {
        DBG_ASSERT(0, "Get port Linerate faild");
        return 0;
    }
    
    for(arrayidx = 0; arrayidx < LOGIC_PORT_NO - 1; arrayidx++)
    {

        websWrite(wp, T("'%d','%d',"), 
		linerate_val[arrayidx].ulRateIn, linerate_val[arrayidx].ulRateOut
		);
    }
    
    websWrite(wp, T("'%d','%d'"), 
	linerate_val[arrayidx].ulRateIn, linerate_val[arrayidx].ulRateOut
	);

    return 1;

}

int asp_get_single_linerate(int eid, webs_t wp, int argc, char_t **argv)
{
    int i = 0;
    unsigned char cnt = 0;
    unsigned char arrayidx =0;
	PORT_LINERATE_VALUES linerate_cur[LOGIC_PORT_NO] = {0};


    for(i=0 ; i< LOGIC_PORT_NO; i++)
    {
        if(port_msk & (1<<i))
        {
            cnt++;
            arrayidx = i;
        }
    }

    if(1 != cnt)
    {
         websWrite(wp, T("'%d','0','0'"),port_msk);
         port_msk = 0;
         return 1;
    }
	
    if(1 != sys_getLinerateCurCfg(linerate_cur))
    {
        DBG_PRINTF("Get port current linerate error.\n");
		websWrite(wp, T("'0','0','0'"));
        port_msk = 0;
        return 1;
    }

    websWrite(wp, T("'%d','%d','%d'"), port_msk,
    linerate_cur[arrayidx].ulRateIn, linerate_cur[arrayidx].ulRateOut
	);
    port_msk = 0;
    
    return 1;

}


int sys_mibcount_getconfig(int *valid, char *sval, char *cp, int *cur_port, int *resetport)
{
#define PORT_CUR    0x01
#define PORT_RESET  0x02
#define MIB_CHK     0x03

	if (!(*valid & PORT_CUR) && !strcmp("curport", sval))
	{
		*valid |= PORT_CUR;
		*cur_port = strtoul(cp, NULL, 0);
	}
	else if (!(*valid & PORT_RESET) && !strcmp("resetport", sval))
	{
		*valid |= PORT_RESET;
		if('a' == cp[0])
		{
			*resetport = 0xff;
		}
		else
		{
			*resetport = strtoul(cp, NULL, 0);
		}
	}
	
    return MIB_CHK;
}

int sys_portmib_handle(webs_t wp, cgi_result_t *result)
{
    int ret = 0;
    int i = 0;
    int valid = 0;
    int count = 0;
    char *cp = NULL;
    char *sval = NULL;
    char svar[10] = {0};  
	int  cur_port = 0;
    int  resetport = 0;


    while(1) 
    {
        sprintf(svar, "SET%d", count);
        sval = websGetVar(wp, svar, NULL);
        if(!sval)
        {      
            break;
        }
        cp = strchr(sval, '=');
        if (!cp)
        {   
            break;
        }
        *cp = 0;
        cp ++;
        if (!(ret = sys_mibcount_getconfig(&valid, sval, cp, &cur_port, &resetport)))
        {
            break;
        }
        if ((valid & ret) == ret)
        {
            break;
        }       
        count ++;
    }   

    if ((valid & ret) != ret)
    {
        DBG_PRINTF("Invalid Form mibcount Command:%04X\n", valid);
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
    }

	port_msk = 1 << (cur_port - 1);
	 
	if(0xff == resetport)
	{
	    for(i=0 ; i< LOGIC_PORT_NO; i++)
	    {
			Ioctl_SetMibCountReset(i+1);
	    }
	}
	else if(0 == resetport)
	{
	    /*just for reload*/
	}
	else
	{
		Ioctl_SetMibCountReset(resetport);
	}
	
    return 0;
}
int sys_linerate_getconfig(int *valid, char *sval, char *cp, PORT_LINERATE_VALUES *req, int *portmsk)
{
#define PORT_MASK    0x01
#define INRTE        0x02
#define OUTRATE      0x04
		
#define LINERATE_CHK      0x07


    if (!(*valid & PORT_MASK) && !strcmp("portmask", sval))
    {
        *valid |= PORT_MASK;
        *portmsk = strtoul(cp, NULL, 0);
        if(*portmsk == 0)
        {
            DBG_ASSERT(0, "Invalid portmask");
            return 0;
        }
    }
    else if (!(*valid & INRTE) && !strcmp("inrate", sval))
    {
        *valid |= INRTE;
        req->ulRateIn = strtoul(cp, NULL, 0);
    }
    else if (!(*valid & OUTRATE) && !strcmp("outrate", sval))
    {
        *valid |= OUTRATE;
        req->ulRateOut = strtoul(cp, NULL, 0);
    }

	return LINERATE_CHK;

}
int sys_getLinerateCurCfg(PORT_LINERATE_VALUES * linerate_cur)
{
	int ret = 0;
    int arrayidx = 0;
	ipc_linerate_ack_t *pack = NULL;
    PORT_LINERATE_VALUES linerate_val[LOGIC_PORT_NO];

	pack = ipc_linerate_req(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) 
    {
        memcpy(linerate_val, pack->linerate_val, sizeof(PORT_LINERATE_VALUES)*LOGIC_PORT_NO);
		ret = IPC_STATUS_OK;
	}
    else if (pack)
    {
		ret = pack->hdr.status;
	}
    else 
    {
		ret = IPC_STATUS_FAIL;
	}
    
	if(pack) 
    {   
        free(pack);     
    }
    
    if(ret != IPC_STATUS_OK)
    {
        DBG_ASSERT(0, "Get port Linerate faild");
        return 0;
    }
    
    for(arrayidx = 0; arrayidx < LOGIC_PORT_NO; arrayidx++)
    {
		linerate_cur[arrayidx].ulRateIn = linerate_val[arrayidx].ulRateIn;
		linerate_cur[arrayidx].ulRateOut = linerate_val[arrayidx].ulRateOut;		
    }

    return 1;
}

int sys_linerate_handle(webs_t wp, cgi_result_t *result)
{
    ipc_linerate_ack_t *pack = NULL;
    PORT_LINERATE_VALUES req;
    int portmsk = 0;
    int ret = 0;
    int i = 0;
    int valid = 0;
    int count = 0;
    char *cp = NULL;
    char *sval = NULL;
    char svar[10] = {0};  
    PORT_LINERATE_VALUES linerate_cur[LOGIC_PORT_NO] = {0};

    memset(&req, 0, sizeof(req));

    while(1) 
    {
        sprintf(svar, "SET%d", count);
        sval = websGetVar(wp, svar, NULL);
        if(!sval)
        {      
            break;
        }
        cp = strchr(sval, '=');
        if (!cp)
        {   
            break;
        }
        *cp = 0;
        cp ++;
        if (!(ret = sys_linerate_getconfig(&valid, sval, cp, &req, &portmsk)))
        {
            break;
        }
        if ((valid & ret) == ret)
        {
            break;
        }       
        count ++;
    }   

    if ((valid & ret) != ret)
    {
        DBG_PRINTF("Invalid Form linerate Command:%04X\n", valid);
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
    }

    /*handle the port cfg according the port mask*/
    if(1 != sys_getLinerateCurCfg(linerate_cur))
    {
        DBG_PRINTF("Get port current linerate error.\n");
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
    }

    for(i=0 ; i< LOGIC_PORT_NO; i++)
    {
        if(portmsk & (1<<i))
        {
            memcpy(&(linerate_cur[i]), &req, sizeof(PORT_LINERATE_VALUES));
        }
    }

    pack = ipc_linerate_req(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, linerate_cur);
    IPC_RESULT(pack, result);
    if(pack)
    {
        free(pack);
    }

    return 0;
}

int sys_mirror_getconfig(int cnt, int* moniportchk, int *valid, char *sval, char *cp, int *req)
{
	char svar[20] = {0};  
	sprintf(svar, "mirrorport%d", cnt+1);

	if (!strcmp(svar, sval))
	{
		req[cnt]= strtoul(cp, NULL, 0);
		/* 
                 0 : no mirror 
                 1:  inmirror
                 2:  outmirror
                 3:  bothmirror
                 4:  moniportflag
		*/
		if((req[cnt] < 0)||(req[cnt] >4))
		{
			DBG_ASSERT(0, "Invalid mirror value");
			return 0;
		}

		if(req[cnt] == 4)
		{
			(*moniportchk)++;
		}
		/*only one monitor port is allowed*/
		if(*moniportchk > 1)
		{
			DBG_ASSERT(0, "Invalid monitor port num");
			return 0;
		}
		/*total  cfgnum chk*/
		(*valid)++;
	}

	return 1;

}


int sys_mirrorset_handle(webs_t wp, cgi_result_t *result)
{
    ipc_mirror_ack_t *pack = NULL;
    int req[LOGIC_PORT_NO] = {0};
	MIRROR_GROUP_S portmirror;
	int moniportchk = 0;
    int ret = 0;
    int i = 0;
	port_num_t portid = 0;
    int valid = 0;
    int count = 0;
    char *cp = NULL;
    char *sval = NULL;
    char svar[10] = {0};  

	memset(&portmirror, 0, sizeof(MIRROR_GROUP_S));

    while(1) 
    {
        sprintf(svar, "SET%d", count);
        sval = websGetVar(wp, svar, NULL);
        if(!sval)
        {      
            break;
        }
        cp = strchr(sval, '=');
        if (!cp)
        {   
            break;
        }
        *cp = 0;
        cp ++;
        if (!(ret = sys_mirror_getconfig(count, &moniportchk, &valid, sval, cp, &req)))
        {
            break;
        }
      
        count ++;
        if (count == LOGIC_PORT_NO)
        {
            break;
        } 
    }   

    if(valid != LOGIC_PORT_NO)
    {
        DBG_PRINTF("Invalid Form MIRROR Command:%04X\n", valid);
        SET_RESULT(result, MSG_FAIL, CGI_ERR_ARGS, 0, 0);   
        return 0;
    }

	ClrLgcMaskAll(&(portmirror.stMirrPortInList));
	ClrLgcMaskAll(&(portmirror.stMirrPortOutList));
	
	if(1 == moniportchk)
	{
	    for(i=0 ; i< LOGIC_PORT_NO; i++)
	    {
	    	portid = i + 1;
			/*no mirror*/
			if(0 == req[i])
			{
				ClrLgcMaskBit(portid, &(portmirror.stMirrPortInList));
				ClrLgcMaskBit(portid, &(portmirror.stMirrPortInList));

			}
			
	    	/*mirror in*/
	    	if(1 == req[i])
			{
				SetLgcMaskBit(portid, &(portmirror.stMirrPortInList));
				ClrLgcMaskBit(portid, &(portmirror.stMirrPortOutList));

			}
			/*mirror out*/
			if(2 == req[i])
			{
				SetLgcMaskBit(portid, &(portmirror.stMirrPortOutList));
				ClrLgcMaskBit(portid, &(portmirror.stMirrPortInList));
			}

			/*mirror both*/
			if(3 == req[i])
			{
				SetLgcMaskBit(portid, &(portmirror.stMirrPortInList));
				SetLgcMaskBit(portid, &(portmirror.stMirrPortOutList));
			}

		    /*mirror port*/
			if(4 == req[i])
			{
				portmirror.ulPortMonitor = portid;
				ClrLgcMaskBit(portid, &(portmirror.stMirrPortInList));
				ClrLgcMaskBit(portid, &(portmirror.stMirrPortInList));
			}
	    }
	}
	else
	{
		/*no mirror*/
		ClrLgcMaskAll(&(portmirror.stMirrPortInList));
		ClrLgcMaskAll(&(portmirror.stMirrPortOutList));
		portmirror.ulPortMonitor = 0xff;
	}

    pack = ipc_mirror_req(ipc_fd, IPC_CONFIG_SET, IPC_APPLY_NOW /*| IPC_APPLY_COMMIT*/, &portmirror);
    IPC_RESULT(pack, result);
    if(pack)
    {
        free(pack);
    }

    return 0;
}

int asp_get_portmirror(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
    int arrayidx = 0;
    port_num_t lport = 0;
	ipc_mirror_ack_t *pack = NULL;
    MIRROR_GROUP_S portmirror;
    char mirrorin_flag = 0;
    char mirrorout_flag = 0;
    int val = 0;

    memset(&portmirror, 0, sizeof(MIRROR_GROUP_S));

	pack = ipc_mirror_req(ipc_fd, IPC_CONFIG_GET, IPC_APPLY_NONE, NULL);
	if (pack && pack->hdr.status == IPC_STATUS_OK) 
    {
        memcpy(&portmirror, &pack->portmirror, sizeof(MIRROR_GROUP_S));
        ret = IPC_STATUS_OK;
	}
    else if (pack)
    {
		ret = pack->hdr.status;
	}
    else 
    {
		ret = IPC_STATUS_FAIL;
	}
    
	if(pack) 
    {   
        free(pack);     
    }
    
    if(ret != IPC_STATUS_OK)
    {
        DBG_ASSERT(0, "Get port mirror faild");
        return 0;
    }
    
    for(arrayidx = 0; arrayidx < LOGIC_PORT_NO - 1; arrayidx++)
    {
        lport = arrayidx + 1;
        mirrorin_flag = 0;
        mirrorout_flag = 0;
        
        if(TstLgcMaskBit(lport, &portmirror.stMirrPortInList))
        {
            mirrorin_flag = 1;
        }

        if(TstLgcMaskBit(lport, &portmirror.stMirrPortOutList))
        {
            mirrorout_flag = 1;
        }

        if(mirrorin_flag && mirrorout_flag)
        {
             val = 3;
        }
        else if(mirrorin_flag)
        {
             val = 1;
        }
        else if(mirrorout_flag)
        {
             val = 2;
        }
        else
        {
            val = 0;
        }
        
        if(portmirror.ulPortMonitor == lport)
        {
            val = 4;
        }

        websWrite(wp, T("'%d',"), val);
    }

    lport++;
    mirrorin_flag = 0;
    mirrorout_flag = 0;
    if(TstLgcMaskBit(lport, &portmirror.stMirrPortInList))
    {
        mirrorin_flag = 1;
    }

    if(TstLgcMaskBit(lport, &portmirror.stMirrPortOutList))
    {
        mirrorout_flag = 1;
    }
   
    if(mirrorin_flag && mirrorout_flag)
    {
         val = 3;
    }
    else if(mirrorin_flag)
    {
         val = 1;
    }
    else if(mirrorout_flag)
    {
         val = 2;
    }
    else
    {
        val = 0;
    }
    
    if(portmirror.ulPortMonitor == lport)
    {
        val = 4;
    }

    websWrite(wp, T("'%d'"), val);

    return 1;

}

int asp_get_portnamelist(int eid, webs_t wp, int argc, char_t **argv)
{
	char tmpbuf[256] = {0};
	char buf[256] = {0};
	port_num_t lport;
	ifindex_t ifindex;
	char szifname[BUF_SIZE_64]= {0};
	char szPseduname[BUF_SIZE_64] = {0};
	
	LgcPortFor(lport)
	{
	    memset(tmpbuf, 0, sizeof(tmpbuf));
        memset(szPseduname, 0, sizeof(szPseduname));
        memset(szifname, 0, sizeof(szifname));
		sprintf(szifname,"%s%d",IF_ETHPORT_NAMEPREFIX,lport);
		IF_GetByIFName(szifname,&ifindex);
		IF_GetWebName(ifindex,szPseduname,sizeof(szPseduname)); 
		if(lport != LOGIC_PORT_NO)
		sprintf(tmpbuf,"'%s',",szPseduname);
		else
		sprintf(tmpbuf,"'%s'",szPseduname);			
		strcat(buf, tmpbuf);
	}

    websWrite(wp, T("%s"), buf);
    return 1;
}

int asp_get_portlogic_num(int eid, webs_t wp, int argc, char_t **argv)
{
    websWrite(wp, T("'%d'"), LOGIC_PORT_NO);
    return 1;
}


