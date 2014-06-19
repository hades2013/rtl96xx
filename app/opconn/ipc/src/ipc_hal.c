/*
=============================================================================
     File Name: ipc_hal.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
			2008/8/19		Initial Version	
----------------   ------------  ----------------------------------------------
*/
	
#include "mc_control.h"
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */
#include <sys/sysinfo.h> 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <vos.h>
#include "ipc_hal.h"
#include "vos_ipc.h"
#include "vos_tlv.h"
#include "odm_port.h"
#include "odm_vlan.h"
#include "odm_pon.h"
#include "version.h"
#include "odm_storm.h"
#include "oam.h"
//wfxu 0402 #include "dal_mac.h"
//wfxu 0402 #include "dal_port.h"
#include "stats.h"
#include "odm_qos.h"
#include "system.h"
#include "oam.h"
#include "zte.h"
#include "opconn_usr_ioctrl.h"

extern const char * product_rev;
extern UINT8	oam_src_mac[6];

extern UINT8	oam_oui[3] ;
extern UINT8	oam_onu_vendor[4] ;
extern UINT8	oam_chip_vendor[2] ;
extern UINT8	oam_onu_model[4] ;
extern UINT8	oam_chip_model[2] ;
extern UINT8	oam_chip_revision ;
extern UINT8	oam_chip_version[3] ;
extern UINT8	oam_firm_ver[2];
extern UINT8	oam_onu_hwarever[8] ;
extern UINT8	oam_onu_swarever[16] ;
extern UINT8	ctc_oui[3];
extern UINT32 oam_ver_type;
static int s_nIpcHalInitialized = 0;
static OPL_IPC_DEV_DRV_t s_IpcHalpstDevDrv;
static VOS_MUTEX_t s_pstIpcDrvLock;
//struct multicast_igmp_cfg igmp_cfg;
CLASSIFY_ITEM_t gClassifyItem[CLASSIFY_MAX_ITEM_NUM]; /* Init this array in dalClsRst() */
extern const char * build_date;
extern const char * build_time;


void IpcHalOnuStatusGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	char onu_mac[18]="";
	int offset = 0;
	int ret = 0;
    UINT32  llid = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	
	
	//ret+=odmPonStatusGet(&odmPonStatus);	
	ret += dalPonLlidGet(&llid);
    if(0 != ret)
    {
    paddr->rtn_code = 2;	//fail 
    }

	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]),1);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]),2);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]),3);

	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]),4);

	offset += 8;
	sprintf(onu_mac,"%02x:%02x:%02x:%02x:%02x:%02x",      
	  oam_src_mac[0],
      oam_src_mac[1],
      oam_src_mac[2],
      oam_src_mac[3],
      oam_src_mac[4],
      oam_src_mac[5]);
	vosTlvSetArray(&(paddr->data[offset]),onu_mac,18);
	
	offset += 22;
	vosTlvSetInt(&(paddr->data[offset]),llid);

	vosMemSet(onu_mac, 0x00, 18);
	sprintf(onu_mac,"%02x:%02x:%02x:%02x:%02x:%02x",      
	  oam_src_mac[0],
      oam_src_mac[1],
      oam_src_mac[2],
      oam_src_mac[3],
      oam_src_mac[4],
      oam_src_mac[5]);
	offset += 8;	
	vosTlvSetArray(&(paddr->data[offset]),onu_mac,18);
	
	offset += 22;
	vosTlvSetInt(&(paddr->data[offset]),6);	
	paddr->rtn_code = 0; //data

	return ;
}

void IpcHalOnuVersionGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	UINT8 ucTemp[32] = {0};
	
	paddr = (VOS_TLV_MSG_t *)pPar;

	vosSafeStrNCpy(ucTemp, MAJOR_VERSION, 32);
	offset = vosTlvArraySet(&(paddr->data[offset]), 1, ucTemp, 32, offset);

	vosSafeStrNCpy(ucTemp, MINOR_VERSION, 32);
	offset = vosTlvArraySet(&(paddr->data[offset]), 2, ucTemp, 32, offset);

	vosSafeStrNCpy(ucTemp, BUILD_NUMBER, 32);
	offset = vosTlvArraySet(&(paddr->data[offset]), 3, ucTemp, 32, offset);

	//vosSafeStrNCpy(ucTemp, product_rev, 32);
	memset(ucTemp, 0, 32);
	offset = vosTlvArraySet(&(paddr->data[offset]), 4, ucTemp, 32, offset);

	paddr->rtn_code = 0; //data

	return ;
}

void IpcHalOnuUptimeGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	long int upTime;
	int offset = 0;
	int ret = 0;
	struct sysinfo s_info;
	
    paddr = (VOS_TLV_MSG_t *)pPar;
    
    ret = sysinfo(&s_info);	
    if(0 != ret)
    {
    paddr->rtn_code = 2;	//fail 
    }
	upTime = s_info.uptime;
	
    offset += 4;
    vosTlvSetInt(&(paddr->data[offset]), upTime);
    paddr->rtn_code = 0;
    return ;

}

void IpcHalOnuCapabilityGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	int ret = 0;
	int	serv_supported;
	int	number_GE;
	int	bitmap_GE[8];
	int	number_FE;
	int	bitmap_FE[8];
	int	number_POTS;
	int	number_E1;	
	int	number_usque;	
	int	quemax_us;
	int	number_dsque;
	int	quemax_ds;
	int	batteryBackup;
		
	paddr = (VOS_TLV_MSG_t *)pPar;
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	serv_supported = 0x06;
	number_GE = 0;
	vosMemSet(bitmap_GE, 0, 8);
	number_FE = 4;
	vosMemSet(bitmap_FE, 0, 8);
	bitmap_FE[7] = 0x0F;
#else
	serv_supported = 0x01;
	number_GE = 1;
	vosMemSet(bitmap_GE, 0, 8);
	bitmap_GE[7] = 0x01;
	number_FE = 0;
	vosMemSet(bitmap_FE, 0, 8);
#endif
	number_POTS = 2;
	number_E1 = 0;
	
#if defined(ONU_4_PORT_88E6045) || defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097) || defined (ONU_1PORT)
	number_usque = 8;
	quemax_us = 8;
	number_dsque = 8;
	quemax_ds = 8;
#endif
	
#if defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	number_usque = 8;
	quemax_us = 8;
	number_dsque = 8;
	quemax_ds = 8;
#endif
	batteryBackup = 0;	

	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), serv_supported);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), number_GE);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), 0);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), number_FE);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), 0);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), number_POTS );
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), number_E1);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), number_usque);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), quemax_us);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), number_dsque);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), quemax_ds);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), batteryBackup);

	paddr->rtn_code = 0;
	return;

}


void IpcHalOnuCapability2Get(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	int ret = 0;
	
	int	onu_type;
	int	multiLlid;
	int    protection_type;
	int    number_of_PONIf;
	int    number_of_slot;
	int 	number_of_interface_type;
	int   fe_interface_type;
	int   number_of_fe_port;
	int   voip_interface_type;
	int   number_of_voip_port;
	int    battery_backup;	
		
	paddr = (VOS_TLV_MSG_t *)pPar;

	onu_type = OAM_CTC_ONU_TYPE_SFU;
	multiLlid = OAM_CTC_SINGLE_LLID;
	protection_type = OAM_CTC_PROTECTION_NONE;
	number_of_PONIf = 1;
	number_of_slot = 0;
	number_of_interface_type = 2;
	fe_interface_type = OAM_CTC_MODULE_FE;
	number_of_fe_port = 4;
	voip_interface_type = OAM_CTC_MODULE_VOIP;
	number_of_voip_port = 2;
	
	battery_backup = 0;	

	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), onu_type);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), multiLlid);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), protection_type);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), number_of_PONIf);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), number_of_slot);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), number_of_interface_type);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), fe_interface_type);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), number_of_fe_port);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), voip_interface_type);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), number_of_voip_port);
	offset += 8;
	vosTlvSetInt(&(paddr->data[offset]), battery_backup);


	paddr->rtn_code = 0;
	return;

}

void IpcHalOnuCtcVerGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	int ret = 0;

	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), oam_ver_type);

	paddr->rtn_code = 0;
	
	return;
}


void IpcHalOnuChipsetGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	int ret = 0;
	oam_chipset_id_t	*p_chip;

    paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	vosTlvSetArray(&(paddr->data[offset]), oam_chip_vendor ,2);
	offset += 36;
	vosTlvSetArray(&(paddr->data[offset]), oam_chip_model ,2);
	offset += 36;
	vosTlvSetArray(&(paddr->data[offset]), &oam_chip_revision,1);
	offset += 36;
	vosTlvSetArray(&(paddr->data[offset]), oam_chip_version,3);
	offset += 36;
	vosTlvSetArray(&(paddr->data[offset]), oam_onu_vendor,4);
	offset += 36;
	vosTlvSetArray(&(paddr->data[offset]), oam_onu_model,4);
	offset += 36;
	vosTlvSetArray(&(paddr->data[offset]), oam_onu_hwarever,8);
	offset += 36;
	vosTlvSetArray(&(paddr->data[offset]), oam_onu_swarever,16);
	offset += 36;
	vosTlvSetArray(&(paddr->data[offset]), oam_firm_ver,2);

	paddr->rtn_code = 0;
    return;
}

void IpcHalOnuChecksumGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int eepromCheckSum = 0;
	int flashCheckSum = 0;
	int offset = 0;
	int ret = 0;
	
     paddr = (VOS_TLV_MSG_t *)pPar;
	 eepromCheckSum = 12;
	 flashCheckSum = 16;
	 ret = 1;
     if( 0 != ret )
     {
        paddr->rtn_code = 2;	//fail 
     }
     offset += 4;
     vosTlvSetInt(&(paddr->data[offset]), eepromCheckSum);
     offset += 8;
     vosTlvSetInt(&(paddr->data[offset]), flashCheckSum);
     paddr->rtn_code = 0;
     return ;

}

void IpcHalOnuFecModeGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int fec_rx_auto = 0;
	int fec_rx_enable = 0;
	int fec_tx_enable;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;

	 ret = odmPonFecEnableGet(&fec_tx_enable);
     if( 0 != ret )
     {
        paddr->rtn_code = 2;	//fail 
     }
     offset += 4;
     vosTlvSetInt(&(paddr->data[offset]), fec_rx_auto);
     offset += 8;
     vosTlvSetInt(&(paddr->data[offset]), fec_rx_enable);
     offset += 8;
     vosTlvSetInt(&(paddr->data[offset]), fec_tx_enable);
     paddr->rtn_code = 0;
     return ;

}

void IpcHalOnuFecModeSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int fec_rx_auto = 0;
	int fec_rx_enable = 0;
	int fec_tx_enable;	
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	fec_rx_auto = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	fec_rx_enable = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	fec_tx_enable = vosTlvIntGet(&(paddr->data[offset]), &offset);


	ret = odmPonFecEnableSet(fec_tx_enable);

	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}

void IpcHalOnuCtcStatusGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int onuCtcStatus;
	int offset = 0;
	int ret = 0;
    ODM_PON_STATUS_t odmPonStatus;
	
    paddr = (VOS_TLV_MSG_t *)pPar;
    ret =  odmPonStatusGet(&odmPonStatus);
    if( 0 != ret)
    {
       paddr->rtn_code = 2;
       return;
    }
    if(STATE_REGISTERD == odmPonStatus.bfDiscCState)
    {
       onuCtcStatus = 1;/*registered*/
    }
    else
    {
       onuCtcStatus = 0; /* unregister*/
    }
    
    offset += 4;
    vosTlvSetInt(&(paddr->data[offset]), onuCtcStatus);
    paddr->rtn_code = 0;
    return ;

}


void IpcHalOnuCtcStatusSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 onuCtcStatus;
	UINT32 mode;
	
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	onuCtcStatus = vosTlvIntGet(&(paddr->data[offset]), &offset);


	ret = 0 ; /* onu ctc register status;*/
	
	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}


void IpcHalOnuMacAddressGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	UINT32 ucif;
	UINT32 ucifMode;
    char macAddr_str[18] = "";
	UINT8 mac[6];
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	ucif = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	ucifMode = vosTlvIntGet(&(paddr->data[offset]), &offset);

	odmSysIfMacAddrGet(ucif, ucifMode, &mac[0]);
	cliMacToStr(mac,macAddr_str);
	
	offset += 4;
	vosTlvSetArray(&(paddr->data[offset]),macAddr_str,18);
	paddr->rtn_code = 0; //data

	return ;

}

void IpcHalOnuMacAddressSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	UINT32 ucif;
	UINT32 ucifMode;	
	char macAddr_str[18] = "";
	int ret=0;
	UINT8 mac[6];
	
	paddr = (VOS_TLV_MSG_t *)pPar;

	offset += 4;
	ucif = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	ucifMode = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), macAddr_str,18,&offset);
	vosStrToMac(macAddr_str,mac);

    VOS_APP_ACCESS_LOCK;
    ret = odmSysIfMacAddrSet(ucif, ucifMode, mac);	
    VOS_APP_ACCESS_UNLOCK;

	if (0 == ret)
	{
	  paddr->rtn_code = 1;  //success
	}
	else
	{
	  paddr->rtn_code = 2;  //fail
	}

	return ;
}

void IpcHalOnuOuiGet(void *pPar,int length) 
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	int ret = 0;

    paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	vosTlvSetArray(&(paddr->data[offset]), oam_oui ,3);
	paddr->rtn_code = 0;

     return ;

}

void IpcHalOnuOuiSet(void *pPar,int length) /*need to fix*/
{
	VOS_TLV_MSG_t *paddr;
	int oui;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	oui = vosTlvIntGet(&(paddr->data[offset]), &offset);
	
	ret = 0;

	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;
}

void IpcHalOnuSnGet(void *pPar,int length) /*need to fix*/
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	int ret = 0;
	char sn[32] = {0};
	int loid_len;
	
	paddr = (VOS_TLV_MSG_t *)pPar;

   	 ret = odmPonSnGet(sn);

	offset = vosTlvArraySet(&(paddr->data[offset]), 1, sn , vosStrLen(sn), offset);

	paddr->rtn_code = 0;

    return ;
}

void IpcHalOnuSnSet(void *pPar,int length) /*need to fix*/
{
	VOS_TLV_MSG_t *paddr;
	char sn[32] = {0};
	int offset = 0;
	int ret = 0;
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), sn, 32, &offset);

	ret = odmPonSnSet(sn);

	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}

void IpcHalOnuPonMacGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
    char macAddr[18] = {0};
	int ret = 0;
	UINT8 mac[6];
	
	paddr = (VOS_TLV_MSG_t *)pPar;

	ret = odmPonMacIdGet(mac);
	if( 0 != ret)
	{
      paddr->rtn_code = 2;//fail
      return;
	}
	cliMacToStr(mac,macAddr);
	offset += 4;
	vosTlvSetArray(&(paddr->data[offset]),macAddr,18);
	paddr->rtn_code = 0; //data
	return ;

}


void IpcHalBpduTransparentGet(void *pPar,int length) 
{
	VOS_TLV_MSG_t *paddr;
	int bpduEnable;
	int offset = 0;
	int ret = 0;
	
    paddr = (VOS_TLV_MSG_t *)pPar;

    /* get BPDU handle mode */
    odmRstpPktHandleModeGet(&bpduEnable);

    offset = vosTlvIntSet(&(paddr->data[offset]), 1, bpduEnable, offset);
    paddr->rtn_code = 0;

    return ;
}

void IpcHalBpduTransparentSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 bpduEnable;
	UINT32 mode;
	
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	bpduEnable = vosTlvIntGet(&(paddr->data[offset]), &offset);

    VOS_APP_ACCESS_LOCK;
	ret = odmRstpPktHandleModeSet(bpduEnable);
    VOS_APP_ACCESS_UNLOCK;
	
	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}

void IpcHalIpAddrGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	UINT32 ucif;
	UINT32 ucifMode;
	UINT32 IpAddr;
    char str_ip[18] = "";
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	ucif = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	ucifMode = vosTlvIntGet(&(paddr->data[offset]), &offset);

	odmSysCfgIfIpAddrGetS(ucif, ucifMode, str_ip);
	
	offset += 4;
	vosTlvSetArray(&(paddr->data[offset]),str_ip,18);
	paddr->rtn_code = 0; //data

	return ;
}

void IpcHalIpMaskGet(void *pPar,int length)
{	
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	UINT32 ucif;
	UINT32 ucifMode;	
	UINT32 IpMaskAddr;
	char str_ipMask[18] = "";
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	ucif = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	ucifMode = vosTlvIntGet(&(paddr->data[offset]), &offset);

	odmSysCfgIfIpMaskGetS(ucif, ucifMode, str_ipMask);
	
	offset += 4;
	vosTlvSetArray(&(paddr->data[offset]),str_ipMask,18);
	paddr->rtn_code = 0; //data

	return ;
}

void IpcHalIpAddrSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	UINT32 IpAddr;
	UINT32 ucif;
	UINT32 ucifMode;	
	char str_ip[18] = "";
	int ret=0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;

	offset += 4;
	ucif = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	ucifMode = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), str_ip,18,&offset);
	cliStrToIp(str_ip,(ULONG *)&IpAddr);

    VOS_APP_ACCESS_LOCK;
	ret+=odmSysCfgIfIpAddrSet(ucif, ucifMode, IpAddr);	
    VOS_APP_ACCESS_UNLOCK;

	if (0 == ret)
	{
	  paddr->rtn_code = 1;  //success
	}
	else
	{
	  paddr->rtn_code = 2;  //fail
	}

	return ;
}

void IpcHalIpMaskSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	UINT32 IpMaskAddr;
	UINT32 ucif;
	UINT32 ucifMode;	
	char str_mask[18] = "";
	int ret=0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	ucif = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	ucifMode = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), str_mask,18,&offset);
	cliStrToIp(str_mask,(ULONG *)&IpMaskAddr);

    VOS_APP_ACCESS_LOCK;
	ret+=odmSysCfgIfIpMaskSet(ucif, ucifMode, IpMaskAddr);
    VOS_APP_ACCESS_UNLOCK;

	if (0 == ret)
	{
	  paddr->rtn_code = 1;  //success
	}
	else
	{
	  paddr->rtn_code = 2;  //fail
	}

	return ;
}


void IpcHalIpGatewaySet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	UINT32 IpGateway;
	UINT32 ucif;
	UINT32 ucifMode;	
	char str_gateway[18] = "";
	int ret=0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	ucif = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	ucifMode = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), str_gateway,18,&offset);
	cliStrToIp(str_gateway,(ULONG *)&IpGateway);

    VOS_APP_ACCESS_LOCK;
	ret+=odmSysCfgIfIpGatewaySet(ucif, ucifMode, IpGateway);
    VOS_APP_ACCESS_UNLOCK;

	if (0 == ret)
	{
	  paddr->rtn_code = 1;	//success
	}
	else
	{
	  paddr->rtn_code = 2;	//fail
	}

	return ;
}

void IpcHalIpDnsSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	UINT32 IpDns;
	UINT32 ucif;
	UINT32 ucifMode;	
	char str_dns[18] = "";
	int ret=0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	ucif = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	ucifMode = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), str_dns,18,&offset);
	cliStrToIp(str_dns,(ULONG *)&IpDns);

    VOS_APP_ACCESS_LOCK;
	ret+=odmSysCfgIfIpDnsSet(ucif, ucifMode, IpDns);
    VOS_APP_ACCESS_UNLOCK;

	if (0 == ret)
	{
	  paddr->rtn_code = 1;	//success
	}
	else
	{
	  paddr->rtn_code = 2;	//fail
	}

	return ;
}

void IpcHalIpGatewayGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	UINT32 IpAddr;
	UINT32 ucif;
	UINT32 ucifMode;	
    char str_gateway[18] = "";
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	ucif = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	ucifMode = vosTlvIntGet(&(paddr->data[offset]), &offset);
	odmSysCfgIfIpGatewayGetS(ucif, ucifMode, str_gateway);
	
	offset += 4;
	vosTlvSetArray(&(paddr->data[offset]),str_gateway,18);
	paddr->rtn_code = 0; //data

	return ;
}

void IpcHalIpDnsGet(void *pPar,int length)
{	
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	UINT32 ucif;
	UINT32 ucifMode;	
	char str_dns[18] = "";
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	ucif = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	ucifMode = vosTlvIntGet(&(paddr->data[offset]), &offset);
	odmSysCfgIfIpDnsGetS(ucif, ucifMode, str_dns);
	
	offset += 4;
	vosTlvSetArray(&(paddr->data[offset]),str_dns,18);
	paddr->rtn_code = 0; //data

	return ;
}


void IpcHalPonMtuGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 cpuMtu;
	int offset = 0;
	int ret = 0;
	
     paddr = (VOS_TLV_MSG_t *)pPar;
	 
	 ret = odmSysMtuGet(&cpuMtu); 
	 if(0 != ret )
	 {
		paddr->rtn_code = 2;   //fail
	 }
	 else {
         offset += 4;
         vosTlvSetInt(&(paddr->data[offset]), cpuMtu);
         paddr->rtn_code = 0;
	 }
     return ;

}

void IpcHalPonMtuSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 cpuMtu;
	UINT32 mode;
	
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	cpuMtu = vosTlvIntGet(&(paddr->data[offset]), &offset);

    VOS_APP_ACCESS_LOCK;
	ret = odmSysMtuSet(cpuMtu);
    VOS_APP_ACCESS_UNLOCK;
	
	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}

void IpcHalFrameFilterTotalNumGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	int ret = 0;
	UINT32 count;
	
    paddr = (VOS_TLV_MSG_t *)pPar;
    ret = odmClassifyWebTotalNumGet(&count);
	if(0 != ret )
	{
	   paddr->rtn_code = 2;	 //fail
	}
 	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), count);
	paddr->rtn_code = 0;   
	return ;
}

void IpcHalFrameFilterGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	int ret = 0;
	UINT32 count;
	UINT32 curCount;
	
    paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	curCount = vosTlvIntGet(&(paddr->data[offset]), &offset);
    ret = odmClassifyWebGet(curCount,&count,&(paddr->data[12]));
	if(0 != ret )
	{
	   paddr->rtn_code = 2;	 //fail
	}
 	/*offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), count);*/
	paddr->rtn_code = 0;   
	return ;
}

void IpcHalFrameFilterSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 type;
	UINT32 transAction;
	char   value[18]={0};
	char   name[20]={0};
	int offset = 0;
	int ret = 0;
	UINT32 portId;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	type = vosTlvIntGet(&(paddr->data[offset]), &offset);	
	offset += 4;
	transAction = vosTlvIntGet(&(paddr->data[offset]), &offset);	
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]),&offset);
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), value,18,&offset);
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), name,20,&offset);

    VOS_APP_ACCESS_LOCK;
    ret = odmOnuClassifySet(type,transAction,portId,1,value,name);
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
   return;
}

void IpcHalFrameFilterDel(void *pPar,int length)
{
    VOS_TLV_MSG_t *paddr;
    UINT32 type;
    UINT32 transAction;
    char	value[18]={0};
    char	name[20]={0};
    UINT16  ruleId;
    int offset = 0;
    int ret = 0;
    UINT32 portId;

    paddr = (VOS_TLV_MSG_t *)pPar;
    offset += 4;
    type = vosTlvIntGet(&(paddr->data[offset]), &offset);	 
    offset += 4;
    transAction = vosTlvIntGet(&(paddr->data[offset]), &offset); 	
    offset += 4;
    portId = vosTlvIntGet(&(paddr->data[offset]),&offset);
    offset += 4;
    vosTlvArrayGet(&(paddr->data[offset]),value,18,&offset);
    offset += 4;
    vosTlvArrayGet(&(paddr->data[offset]), name,20,&offset);

    VOS_APP_ACCESS_LOCK;
    ret = odmOnuClassifySet(type,transAction,portId,0,value,name);
    VOS_APP_ACCESS_UNLOCK;

    if (0 != ret)
    {
        paddr->rtn_code = 2;  //fail
    }
    else
    {
        paddr->rtn_code = 1;  //success
    }
	return;

}

void IpcHalPortMacBindGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32   portId;
	char     macAddrStr[18]="";
	UINT16   vlanId;
	UINT8    entryNum;
	int      entryCount =0;
	UINT32   i =0;
	int      offset = 0;
	int      flag = 1;
	int      ret = 0;
	char tmp[1000] ="";

	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;

    for( portId=1; portId<= 4;portId++)
  	{
  	   ret = odmPortBindMacNumGet(portId,&entryNum);
	   if(0 != ret )
	   {
          paddr->rtn_code = 2;	//fail
          return;
	   }

	   for(i = 1; i <= entryNum; i++)
	   {
		   ret = odmPortBindMacEntryGet(portId, (i-1), &vlanId, &macAddrStr);
	   
    	   if(0 != ret)
    	   {
               paddr->rtn_code = 2;	//fail
               return;
    	   }
		   if(flag ==1)
		   {   
    	   	     entryCount++;
                 vosTlvSetInt(&(paddr->data[offset]), portId);
                 offset += 4;
                 vosTlvSetArray(&(paddr->data[offset]),macAddrStr,18);
    		     offset += 18;
                 flag = 0;
    	   }
		   else
		   {
		   	     entryCount++;
                 vosTlvSetInt(&(paddr->data[offset]), portId);
                 offset += 4;
                 vosTlvSetArray(&(paddr->data[offset]),macAddrStr,18);
			     offset += 18;
		   	}
	   }
	}

	 vosTlvSetArray(&(paddr->data[offset]),tmp, 1000-entryCount*22);
     offset +=1000-entryCount*22 ;
	 offset += 4;
     vosTlvSetInt(&(paddr->data[offset]), entryCount);
	 paddr->rtn_code = 0;
	 return;

}

void IpcHalPortMacBindSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 vlanId = 0;
	UINT32 mac;
	int offset = 0;
	int ret = 0;
	char str_mac[18] ="";
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);	
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), str_mac,18,&offset);

    VOS_APP_ACCESS_LOCK;
	ret += odmPortBindMacEntryAdd(portId,0, str_mac);
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}

	return ;
}

void IpcHalPortMacBindDel(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 vlanId = 0;
	UINT32 mac;
	int offset = 0;
	int ret = 0;
	char str_mac[18] ="";
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), str_mac,18,&offset);	

    VOS_APP_ACCESS_LOCK;
	ret += odmPortBindMacEntryDel(portId,0, str_mac);
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}

void IpcHalPortMacFilterGet(void *pPar,int length)
#if 0
{
  VOS_TLV_MSG_t *paddr;
  UINT8 entryNum;
  int ret = 0;
  int entryCount =0;
  char tmp[1000] ="";
  
  paddr = (VOS_TLV_MSG_t *)pPar;
  ret += dalArlMacEntryShowDiscard(&entryNum, &(paddr->data[4]));
  
  vosTlvSetArray(&(paddr->data[4+entryNum*22]),tmp, 1000-entryNum*22);
  entryCount = entryNum;
  vosTlvSetInt(&(paddr->data[1008]), entryCount);
  paddr->rtn_code = 0;
  
  return;
}
#else
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	char str_mac[18] ="";
	UINT32 vlanId;
	UINT8 entryNum;
	UINT32 i =0;
	int offset = 0;
	int ret = 0;
	int flag = 1;
	int entryCount =0;
	char tmp[1000] ="";

	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;

    for( portId=1; portId<= 4;portId++)
  	{
  	   ret = odmPortMacFilterNumGet(portId,&entryNum);
	   if(0 != ret )
	   {
              paddr->rtn_code = 2;	//fail
              return;
	   }

	   for(i = 1; i <= entryNum; i++)
	   {
		   ret = odmPortMacFilterEntryGet(portId, (i-1), &vlanId, &str_mac);
	   
       	   if(0 != ret)
       	   {
               paddr->rtn_code = 2;	//fail
               return;

       	   }
		   if(flag ==1)
		   {   
		   	     entryCount++;
                 vosTlvSetInt(&(paddr->data[offset]), portId);
                 offset += 4;
                 vosTlvSetArray(&(paddr->data[offset]),str_mac,18);
			     offset += 18;
			     flag = 0;
		   }
		   else
		   {
		   	     entryCount++;
                 vosTlvSetInt(&(paddr->data[offset]), portId);
                 offset += 4;
                 vosTlvSetArray(&(paddr->data[offset]),str_mac,18);
			     offset += 18;		 
		   	}
	   }
	}

	 vosTlvSetArray(&(paddr->data[offset]),tmp, 1000-entryCount*22);
     offset +=1000-entryCount*22 ;
	 offset += 4;
     vosTlvSetInt(&(paddr->data[offset]), entryCount);
	 paddr->rtn_code = 0;
	 return;

}
#endif

void IpcHalPortMacFilterSet(void *pPar,int length)
{
	
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 vlanId;
	int offset = 0;
	int ret = 0;
	char str_mac[18] ="";
	UINT8 mac[6];
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);	
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), str_mac,18,&offset);

    VOS_APP_ACCESS_LOCK;
	ret += odmPortMacFilterEntryAdd(portId,0, str_mac);
    VOS_APP_ACCESS_UNLOCK;

	//vosStrToMac(str_mac, &mac[0]);
	//ret += odmFdbMacDiscard(mac);
	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}

void IpcHalPortMacFilterDel(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 vlanId;
	UINT8 mac[6];
	int offset = 0;
	int ret = 0;
	char str_mac[18] ="";
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), str_mac,18,&offset);

    VOS_APP_ACCESS_LOCK;
	ret += odmPortMacFilterEntryDel(portId,0, str_mac);
    VOS_APP_ACCESS_UNLOCK;

	//vosStrToMac(str_mac, &mac[0]);
	//ret += odmFdbMacDiscardClear(mac);

	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}


void IpcHalMacLimitStatusGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 enable;
	UINT32 numOfMac;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	ret += odmPortMacLimitNumEnableGet(portId,&enable,&numOfMac);
	if (0 != ret)
	{
	  paddr->rtn_code = 2;  //fail
	  return ;
	}
	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), enable);
	paddr->rtn_code = 0;
	return ;

}

void IpcHalMacLimitStatusSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 enable;
	UINT32 numOfMac=0;

	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);	
	offset += 4;
	enable = vosTlvIntGet(&(paddr->data[offset]), &offset);

    VOS_APP_ACCESS_LOCK;
	ret += odmPortMacLimitNumEnableSet(portId, enable,numOfMac);
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
        paddr->rtn_code = 2;  //fail
	}
	else
	{
        paddr->rtn_code = 1;  //success
	}
	return ;

}


void IpcHalPortMacLimitGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 enable;
	UINT32 numOfMac;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	ret += odmPortMacLimitNumEnableGet(portId,&enable,&numOfMac);
	if (0 != ret)
	{
	  paddr->rtn_code = 2;  //fail
	  return ;
	}
	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), numOfMac);
	paddr->rtn_code = 0;
	return ;

}

void IpcHalPortMacLimitSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 enable =TRUE;
	UINT32 numOfMac;

	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	numOfMac = vosTlvIntGet(&(paddr->data[offset]), &offset);	

    VOS_APP_ACCESS_LOCK;
	ret += odmPortMacLimitNumEnableSet(portId, enable,numOfMac);
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
	  paddr->rtn_code = 2;  //fail
	}
	else
	{
	  paddr->rtn_code = 1;  //success
	}

	return ;

}

void IpcHalMacBindStatusGet(void *pPar,int length)
{
	return ;
}

void IpcHalMacBindStatusSet(void *pPar,int length)
{

	return ;

}

void IpcHalPqueueIngressLimitGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int queueType;
	int qPrio;
	UINT16 ingressLimit = 0;
	int offset = 0;
	int ret = 0;
	
    paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	queueType = vosTlvIntGet(&(paddr->data[offset]), &offset);	 
	offset += 4;
	qPrio = vosTlvIntGet(&(paddr->data[offset]), &offset);		
  
    if ((0 == queueType) || (1 == queueType))
    {
        ret = tmQueueMaxCellNumHwRead(queueType, qPrio, &ingressLimit);
    }
    else
    {
        ;
    }
  
	if (0 != ret)
	{
        paddr->rtn_code = 2;  //fail
        return ;
	}

    offset += 4;
    vosTlvSetInt(&(paddr->data[offset]), ingressLimit);
    paddr->rtn_code = 0;
    return ;
}


void IpcHalPqueueBlockSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int queueType;
	int queueId;
	int queueBlock;
	int offset = 0;
	int ret = 0;
	
    paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	queueType = vosTlvIntGet(&(paddr->data[offset]), &offset);	 
	offset += 4;
	queueId = vosTlvIntGet(&(paddr->data[offset]), &offset);	
	offset += 4;
	queueBlock = vosTlvIntGet(&(paddr->data[offset]), &offset);		
  
    if ((0 == queueType) || (1 == queueType))
    {
        VOS_APP_ACCESS_LOCK;
        ret = odmQueueLimitSet(queueType, queueId, queueBlock);
        VOS_APP_ACCESS_UNLOCK;
    }
    else
    {
        ;
    }
  
	if (0 != ret)
	{
        paddr->rtn_code = 2;  //fail
	}
	else
	{
        paddr->rtn_code = 1;  //success
	}
	return ;
}

void IpcHalIngressQueueOccupancyGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int queue;
	int qPrio;
	UINT32 ingressLimit=0;
	int offset = 0;
	int ret = 0;
	
    paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	queue = vosTlvIntGet(&(paddr->data[offset]), &offset);	 
	offset += 4;
	qPrio = vosTlvIntGet(&(paddr->data[offset]), &offset);		
  
    if ((0 == queue) || (1 == queue))
    {
        VOS_APP_ACCESS_LOCK;
        ret = tmCellNumInQueueHwRead(queue, qPrio, &ingressLimit);
        VOS_APP_ACCESS_UNLOCK;
    }
    else
    {
        ;
    }

	if (0 != ret)
	{
        paddr->rtn_code = 2;  //fail
        return ;
	}
	 
    offset += 4;
    vosTlvSetInt(&(paddr->data[offset]), ingressLimit);
    paddr->rtn_code = 0;
    return ;

}

void IpcHalPortIsolationStatusGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT8 isolateState;
	UINT32 numOfMac;
	int offset = 0;
	int ret = 0;
	int tmpState =0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	ret += odmGetLocalSwitch(portId,&isolateState);
	if (0 != ret)
	{
        paddr->rtn_code = 2;  //fail
        return ;
	}
	if(isolateState ==0)
	{
	    tmpState = 1;
	}
	else
	{
	    tmpState = 0;
	}	
	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), tmpState);
	paddr->rtn_code = 0;
	return ;

}

void IpcHalPortIsolationStatusSet(void *pPar,int length)
{
    VOS_TLV_MSG_t *paddr;
    UINT32 portId;
    UINT32 isolateState;
	UINT32 tmpState;

    int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	isolateState = vosTlvIntGet(&(paddr->data[offset]), &offset);
	if(isolateState ==0)
	{
	    tmpState = 1;
	}
	else
	{
	    tmpState = 0;
    }
    for( portId = 1;portId<=ODM_NUM_OF_PORTS;portId++)
    {
        ret += odmSetLocalSwitch(portId, tmpState);
    }
    if (0 != ret)
    {
        paddr->rtn_code = 2;  //fail
	}
	else
	{
        paddr->rtn_code = 1;  //success
	}
	return ;

}


void IpcHalExtendMtuStatusGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int mtuStatus;
	int offset = 0;
	int ret = 0;
	
    paddr = (VOS_TLV_MSG_t *)pPar;

    ret = odmOnuMtuGet(&mtuStatus);
    if (0 != ret)
    {
        paddr->rtn_code = 2;  //fail
        return ;
    }	 
    offset += 4;
    vosTlvSetInt(&(paddr->data[offset]), mtuStatus);
    paddr->rtn_code = 0;
    return ;

}

void IpcHalExtendMtuStatusSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 mtuStatus;
	UINT32 mode;
	
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	mtuStatus = vosTlvIntGet(&(paddr->data[offset]), &offset);

    VOS_APP_ACCESS_LOCK;
	ret = odmOnuMtuSet(mtuStatus);	
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}

void IpcHalPortSpeedModeGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 adminStatus;
	UINT32 portSpeed;
	UINT32 duplex;
	UINT32 speedMode;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	ret += odmPortAutoEnableGet(portId, &adminStatus);
	ret += odmPortSpeedGet(portId, &portSpeed);
	ret += odmPortDuplexGet(portId, &duplex);
	
	if (ret != 0)
	{
	  paddr->rtn_code = 2;  //fail
	  return ;
	}
	if (1 == adminStatus)
	{
	  speedMode = 0;
	}
	else if ((portSpeed==ODM_PORT_SPEED_10M) && (duplex==ODM_PORT_DUPLEX_HALF))
	{
	  speedMode = 1;
	}
	else if ((portSpeed==ODM_PORT_SPEED_10M) && (duplex==ODM_PORT_DUPLEX_FULL))
	{
	  speedMode = 2;
	}
	else if ((portSpeed==ODM_PORT_SPEED_100M) && (duplex==ODM_PORT_DUPLEX_HALF))
	{
	  speedMode = 3;
	}
	else if ((portSpeed==ODM_PORT_SPEED_100M) && (duplex==ODM_PORT_DUPLEX_FULL))
	{
	  speedMode = 4;
	}
	offset += 4;

	vosTlvSetInt(&(paddr->data[offset]), speedMode);
	paddr->rtn_code = 0; //data
	
	return ;
}

void IpcHalPortSpeedModeSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 adminStatus;
	UINT32 portSpeed;
	UINT32 duplex;
	UINT32 speedMode;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	speedMode = vosTlvIntGet(&(paddr->data[offset]), &offset);
	  
	if (0 == speedMode)
	{
	  adminStatus = 1;
	  portSpeed = ODM_PORT_SPEED_10M;
	  duplex = ODM_PORT_DUPLEX_HALF;
	}
	else if (1 == speedMode)
	{
	  adminStatus = 0;
	  portSpeed = ODM_PORT_SPEED_10M;
	  duplex = ODM_PORT_DUPLEX_HALF;
	}
	else if (2 == speedMode)
	{
	  adminStatus = 0;
	  portSpeed = ODM_PORT_SPEED_10M;
	  duplex = ODM_PORT_DUPLEX_FULL;
	}
	else if (3 == speedMode)
	{
	  adminStatus = 0;
	  portSpeed = ODM_PORT_SPEED_100M;
	  duplex = ODM_PORT_DUPLEX_HALF;
	}
	else if (4 == speedMode)
	{
	  adminStatus = 0;
	  portSpeed = ODM_PORT_SPEED_100M;
	  duplex = ODM_PORT_DUPLEX_FULL;
	}
	else
	{
	  paddr->rtn_code = 2;  //fail
	  return ;
	}
	
    if (adminStatus)
    {
    	ret += odmPortSpeedSet(portId, portSpeed);
    	ret += odmPortDuplexSet(portId, duplex);
    	ret += odmPortAutoEnableSet(portId, adminStatus);
    }
    else
    {
    	ret += odmPortAutoEnableSet(portId, adminStatus);
    	ret += odmPortSpeedSet(portId, portSpeed);
    	ret += odmPortDuplexSet(portId, duplex);
    }
	if (0 == ret)
	{
	  paddr->rtn_code = 1;  //success
	}
	else
	{
	  paddr->rtn_code = 2;  //fail
	}
	
	return ;
}

void IpcHalPortStormRestrainGet(void *pPar,int length) 
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 state =0;
	UINT32 mode=0;
	UINT32 limitRate=0 ;
	int offset = 0;
	int ret = 0;

	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	mode = vosTlvIntGet(&(paddr->data[offset]), &offset);	
	ret += odmStormCtrlFrameGet(portId,mode, &state);
	if (0 != ret)
	{
	  paddr->rtn_code = 2;  //fail
	  return ;
	}
	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), state);
	paddr->rtn_code = 0; //data
	
	return ;
}

void IpcHalPortStormRestrainSet(void *pPar,int length) 
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 state;
	UINT32 mode;
	UINT32 limitRate;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	mode = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	state = vosTlvIntGet(&(paddr->data[offset]), &offset);

    VOS_APP_ACCESS_LOCK;
	ret += odmStormCtrlFrameSet(portId,mode,state);
    VOS_APP_ACCESS_UNLOCK;

	if (0 == ret)
	{
        paddr->rtn_code = 1;  //success
	}
	else
	{
        paddr->rtn_code = 2;  //fail
	}
	return ;
}

void IpcHalPortStormUsRateGet(void *pPar,int length) 
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 state =0;
	UINT32 limitRate=0 ;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	ret += odmStormCtrlRateGet(portId, &limitRate);
	if (0 != ret)
	{
        paddr->rtn_code = 2;  //fail
        return ;
	}
	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), limitRate);
	paddr->rtn_code = 0; //data
	
	return ;
}

void IpcHalPortStormUsRateSet(void *pPar,int length) 
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 state;
	UINT32 mode;
	UINT32 limitRate;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	limitRate =vosTlvIntGet(&(paddr->data[offset]), &offset);

    VOS_APP_ACCESS_LOCK;
    ret = odmStormCtrlRateSet(portId,limitRate);
    VOS_APP_ACCESS_UNLOCK;

	if (0 == ret)
	{
	  paddr->rtn_code = 1;  //success
	}
	else
	{
	  paddr->rtn_code = 2;  //fail
	}
	return ;
}


void IpcHalPortStormDsRateGet(void *pPar,int length) 
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 limitRate=0 ;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	#if defined(ONU_4PORT_AR8327)
       ret += odmStormCtrlRateGet(0, &limitRate);
	#else
	ret += odmStormCtrlRateGet(6, &limitRate);
	#endif
	if (0 != ret)
	{
	  paddr->rtn_code = 2;  //fail
	  return ;
	}
	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), limitRate);
	paddr->rtn_code = 0; //data
	
	return ;
}

void IpcHalPortStormDsRateSet(void *pPar,int length) 
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 limitRate;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	limitRate =vosTlvIntGet(&(paddr->data[offset]), &offset);

    VOS_APP_ACCESS_LOCK;
    #if defined(ONU_4PORT_AR8327)
    ret += odmStormCtrlRateSet(0,limitRate);
    #else
    ret += odmStormCtrlRateSet(6,limitRate);
    #endif
    VOS_APP_ACCESS_UNLOCK;

	if (0 == ret)
	{
	  paddr->rtn_code = 1;  //success
	}
	else
	{
	  paddr->rtn_code = 2;  //fail
	}
	return ;
}




void IpcHalPortConfGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
    UINT32 linkState;
	UINT32 powerDown;
	UINT32 pause;
	UINT32 speedDuplex;
	UINT32 autoNeg;
	UINT8  priority;
	UINT32 pvid;
	UINT32 ingressRate;
	UINT8 isolate;
	
	UINT32 adminStatus;
	UINT32 portSpeed;
	UINT32 duplex;
	UINT32 usEnable;
	int offset = 0;
	int ret = 0;

	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
    for( portId=1; portId<= 4;portId++)
  	{
  		/* here is not a bug , just avoid bug,so must get twice linkstate to get correct result */
       ret += odmPortLinkStateGet(portId, &linkState);
       ret += odmPortLinkStateGet(portId, &linkState);	   
       ret += odmPortAdminGet(portId, &powerDown);
       ret += odmPortFlowCtrolEnableGet(portId, &pause);
       ret += odmPortAutoEnableGet(portId, &adminStatus);
       ret += odmPortSpeedGet(portId, &portSpeed);
       ret += odmPortDuplexGet(portId, &duplex);
       if (1 == adminStatus)
       {
         speedDuplex = 0;
       }
       else if ((portSpeed==ODM_PORT_SPEED_10M) && (duplex==ODM_PORT_DUPLEX_HALF))
       {
         speedDuplex = 1;
       }
       else if ((portSpeed==ODM_PORT_SPEED_10M) && (duplex==ODM_PORT_DUPLEX_FULL))
       {
         speedDuplex = 2;
       }
       else if ((portSpeed==ODM_PORT_SPEED_100M) && (duplex==ODM_PORT_DUPLEX_HALF))
       {
         speedDuplex = 3;
       }
       else if ((portSpeed==ODM_PORT_SPEED_100M) && (duplex==ODM_PORT_DUPLEX_FULL))
       {
         speedDuplex = 4;
       }
       ret += odmPortDefaultEtherPriGet(portId, &priority);
       ret += odmPortDefaultVlanGet(portId, &pvid);
	   ret += odmPortUsPolicingEnableGet(portId,&usEnable);
       ret += odmPortUsCirGet(portId,&ingressRate);
       ret += odmGetLocalSwitch(portId, &isolate);

       if (0 != ret)
       {
         paddr->rtn_code = 2;  //fail
         return ;
       }
	   vosTlvSetInt(&(paddr->data[offset]), portId);	   
       offset += 4;   
	   vosTlvSetInt(&(paddr->data[offset]), (int)linkState);	   
       offset += 4;
       vosTlvSetInt(&(paddr->data[offset]), speedDuplex);
       offset += 4;
       vosTlvSetInt(&(paddr->data[offset]), powerDown);
       offset += 4;
       vosTlvSetInt(&(paddr->data[offset]), pause);
       offset += 4;	   
       vosTlvSetInt(&(paddr->data[offset]), pvid);
       offset += 4;
       vosTlvSetInt(&(paddr->data[offset]), (int)priority);
	   if(usEnable ==0)
	   {
            offset += 4;
			ingressRate =0;
            vosTlvSetInt(&(paddr->data[offset]), ingressRate);
	   }
	   else
	   {
		   offset += 4;
		   vosTlvSetInt(&(paddr->data[offset]), ingressRate);
	   }
       offset += 4;
       vosTlvSetInt(&(paddr->data[offset]), (int)isolate);
	   offset += 4;
	}
	paddr->rtn_code = 0;
		
	return ;
}
void IpcHalPortConfSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 linkState;
	UINT32 powerDown;
	UINT32 pause;
	UINT32 speedDuplex;
	UINT32 autoNeg;
	UINT8 priority;
	UINT32 pvid;
	UINT32 ingressRate;
	UINT8 isolate;
	UINT32 portSpeed;
	UINT32 duplex;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	powerDown = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	pause = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	speedDuplex = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	priority = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	pvid = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	ingressRate = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	isolate = vosTlvIntGet(&(paddr->data[offset]), &offset);
	if (0 == speedDuplex)
	{
	  autoNeg = 1;
	  portSpeed = ODM_PORT_SPEED_10M;
	  duplex = ODM_PORT_DUPLEX_HALF;
	}
	else if (1 == speedDuplex)
	{
	  autoNeg = 0;
	  portSpeed = ODM_PORT_SPEED_10M;
	  duplex = ODM_PORT_DUPLEX_HALF;
	}
	else if (2 == speedDuplex)
	{
	  autoNeg = 0;
	  portSpeed = ODM_PORT_SPEED_10M;
	  duplex = ODM_PORT_DUPLEX_FULL;
	}
	else if (3 == speedDuplex)
	{
	  autoNeg = 0;
	  portSpeed = ODM_PORT_SPEED_100M;
	  duplex = ODM_PORT_DUPLEX_HALF;
	}
	else if (4 == speedDuplex)
	{
	  autoNeg = 0;
	  portSpeed = ODM_PORT_SPEED_100M;
	  duplex = ODM_PORT_DUPLEX_FULL;
	}
	ret += odmPortAdminSet(portId, powerDown);
	ret += odmPortFlowCtrolEnableSet(portId, pause);
    if (autoNeg)
    {
    	ret += odmPortSpeedSet(portId, portSpeed);
    	ret += odmPortDuplexSet(portId, duplex);
        ret += odmPortAutoEnableSet(portId, autoNeg);
    }
    else {
        ret += odmPortAutoEnableSet(portId, autoNeg);
    	ret += odmPortSpeedSet(portId, portSpeed);
    	ret += odmPortDuplexSet(portId, duplex);
    }
    VOS_APP_ACCESS_LOCK;
	ret += odmPortDefaultEtherPriSet(portId, priority);
    VOS_APP_ACCESS_UNLOCK;
	ret += odmPortDefaultVlanSet(portId, pvid);
	ret += odmSetLocalSwitch(portId, isolate);
	if(ingressRate == 0)
	{
		ret += odmPortUsPolicingEnableSet(portId,0);
		ret += odmPortUsCirSet(portId,ingressRate);
	}
	else
	{
		ret += odmPortUsPolicingEnableSet(portId,1);
		ret += odmPortUsCirSet(portId,ingressRate);
	}
	
	if (0 == ret)
	{
	  paddr->rtn_code = 1;  //success
	}
	else
	{
	  paddr->rtn_code = 2;  //fail
	}
	return ;
}

void IpcHalPortLinkStateGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 state;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	ret += odmPortLinkStateGet(portId, &state);
	if (0 != ret)
	{
	  paddr->rtn_code = 2;  //fail
	  return ;
	}
	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), (int)state);
	paddr->rtn_code = 0;
	return ;
}

void IpcHalPortVlanModeGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 mode;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	ret += odmPortVlanModeGet(portId, &mode);
	if (0 != ret)
	{
	  paddr->rtn_code = 2;  //fail
	  return ;
	}
	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), mode);
	paddr->rtn_code = 0;
	return ;
}

void IpcHalPortPvidGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 pvid;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	ret += odmPortDefaultVlanGet(portId, &pvid);
	if (0 != ret)
	{
	  paddr->rtn_code = 2;  //fail
	  return ;
	}
	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), pvid);
	paddr->rtn_code = 0;
	return ;
}

void IpcHalPortVlanModeSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 mode;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	mode = vosTlvIntGet(&(paddr->data[offset]), &offset);	

    VOS_APP_ACCESS_LOCK;
	ret += odmPortVlanModeSet(portId, mode);
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
	  paddr->rtn_code = 2;  //fail
	}
	else
	{
	  paddr->rtn_code = 1;  //success
	}
	return ;
}

void IpcHalPortPvidSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 pvid;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	pvid = vosTlvIntGet(&(paddr->data[offset]), &offset);

    VOS_APP_ACCESS_LOCK;
	ret += odmPortDefaultVlanSet(portId, pvid);
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
	  paddr->rtn_code = 2;  //fail
	}
	else
	{
	  paddr->rtn_code = 1;  //success
	}
	return ;
}

void IpcHalQosSchedulingModeGet(void *pPar,int length) 
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 mode;
	int offset = 0;
	int ret = 0;
	for(portId=1;portId<5;portId++)
	{
	   ret = odmQosSchedModeGet(portId,&mode);
       if (0 != ret)
       {
          paddr->rtn_code = 2;	//fail
          return ;
       }
	}
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
    vosTlvSetInt(&(paddr->data[offset]), mode);
    	
	paddr->rtn_code = 0;
	return ;


}

void IpcHalQosSchedulingModeSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 mode;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	mode = vosTlvIntGet(&(paddr->data[offset]), &offset);
	
	for(portId=1;portId<5;portId++)
	{
        VOS_APP_ACCESS_LOCK;
        ret = odmQosSchedModeSet(portId,mode);
        VOS_APP_ACCESS_UNLOCK;

        if (0 != ret)
        {
            paddr->rtn_code = 2;  //fail
        }
        else
        {
            paddr->rtn_code = 1;  //success
        }
	}
	return ;

}
void IpcHalPolicyQueueMappingGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	unsigned char   queueId;
	unsigned char   queuePri;
	int offset = 0;
	int ret = 0;
	
    paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	queuePri = vosTlvIntGet(&(paddr->data[offset]), &offset);	 	
	ret=odmQosCos2QMapGet(queuePri,&queueId);
	if (0 != ret)
	{
	  paddr->rtn_code = 2;  //fail
	  return ;
	}
	 
    offset += 4;
    vosTlvSetInt(&(paddr->data[offset]),queueId );
    paddr->rtn_code = 0;
    return ;

}

void IpcHalPolicyQueueMappingSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	unsigned char  queueId;
	unsigned char  queuePri;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	queuePri= vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	queueId= vosTlvIntGet(&(paddr->data[offset]), &offset);	

    VOS_APP_ACCESS_LOCK;
	ret=odmQosCos2QMapSet(queuePri,queueId);
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
        paddr->rtn_code = 2;	//fail
	}
	else
	{
        paddr->rtn_code = 1;	//success
	}

	return ;

}
static int QueueMapStatus[4]={0};
void IpcHalQueueMappingEnableGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int  portId;
	int  enable;
	int offset = 0;
	int ret = 0;
	
    paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);	 
	ret  = 0; /* ********(portId,&enable);*/

    VOS_APP_ACCESS_LOCK;
	enable =QueueMapStatus[portId];
    VOS_APP_ACCESS_UNLOCK;
	
	if (0 != ret)
	{
        paddr->rtn_code = 2;  //fail
        return ;
	} 
    offset += 4;
    vosTlvSetInt(&(paddr->data[offset]), enable);
    paddr->rtn_code = 0;
    return ;

}

void IpcHalQueueMappingEnableSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int  portId;
	int  enable;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	enable = vosTlvIntGet(&(paddr->data[offset]), &offset);	
	ret = 0;

    VOS_APP_ACCESS_LOCK;
	QueueMapStatus[portId]=enable;
	odmQosPortPriRemapStatusSet(portId,enable);
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
        paddr->rtn_code = 2;	//fail
	}
	else
	{
        paddr->rtn_code = 1;	//success
	}

	return ;

}

void IpcHalVlanPriorityTranlationGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int  portId=0;
	int  queueId=0;
	int  queuePri=0;
	int offset = 0;
	int ret = 0;
	
    paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);	 
	offset += 4;
	queueId = vosTlvIntGet(&(paddr->data[offset]), &offset);	 	
	ret=odmQosPortPriRemapGet(portId,queueId,&queuePri);

	if (0 != ret)
	{
	  paddr->rtn_code = 2;  //fail
	  return ;
	}
	 
    offset += 4;
    vosTlvSetInt(&(paddr->data[offset]), queuePri);
    paddr->rtn_code = 0;
    return ;

}

void IpcHalVlanPriorityTranlationSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int  portId;
	int  queueId;
	int  queuePri;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);	 
	offset += 4;
	queueId = vosTlvIntGet(&(paddr->data[offset]), &offset);	
	offset += 4;
	queuePri = vosTlvIntGet(&(paddr->data[offset]), &offset);

    VOS_APP_ACCESS_LOCK;
	ret=odmQosPortPriRemapSet(portId,queueId,queuePri);
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
        paddr->rtn_code = 2;	//fail
	}
	else
	{
        paddr->rtn_code = 1;	//success
	}

	return ;

}

void IpcHalIgmpSnoopEnableGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int enable;
	int offset = 0;
	int ret = 0;
	
    paddr = (VOS_TLV_MSG_t *)pPar;

    VOS_APP_ACCESS_LOCK;
    if( mcControlBlockModeGet() == MC_MODE_STOP)
    {
        enable = 0; /*igmp snooping disable*/
    }
    else 
    {
        enable = 1;
    }
    VOS_APP_ACCESS_UNLOCK;

    offset += 4;
    vosTlvSetInt(&(paddr->data[offset]), enable);
    paddr->rtn_code = 0;
    return ;
	
}

void IpcHalIgmpSnoopEnableSet(void *pPar,int length)
{
	
	VOS_TLV_MSG_t *paddr;
	UINT32 igmpState;
	UINT32 mode;
	
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	igmpState = vosTlvIntGet(&(paddr->data[offset]), &offset);

	if( igmpState == 0 )  /*igmp snooping diable*/
	{
	   mode = MC_MODE_STOP;
	}
	else 
	{
	   mode = MC_MODE_SNOOPING;
	}

    VOS_APP_ACCESS_LOCK;
	ret = odmMcControlBlockModeSet(mode);
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}

void IpcHalIgmpSnoopAgeTimeGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int time;
	int offset = 0;
	int ret = 0;

	paddr = (VOS_TLV_MSG_t *)pPar;

    VOS_APP_ACCESS_LOCK;
	time = mcControlBlockHostAgingTimeGet();
    VOS_APP_ACCESS_UNLOCK;

	offset += 4;

    vosTlvSetInt(&(paddr->data[offset]), time);
	paddr->rtn_code = 0;
	return ;
}

void IpcHalIgmpSnoopAgeTimeSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 ageTime;

	int offset = 0;
	int ret = 0;

	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	ageTime = vosTlvIntGet(&(paddr->data[offset]), &offset);

    VOS_APP_ACCESS_LOCK;
	ret = odmMcControlBlockHostAgingTimeSet(ageTime);
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
        paddr->rtn_code = 2;	//fail
	}
	else
	{
        paddr->rtn_code = 1;	//success
	}
	return ;

}

void IpcHalMacAgingTimeGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int ageTime;
	int offset = 0;
	int ret = 0;
	
    paddr = (VOS_TLV_MSG_t *)pPar;
    ret = odmFdbAgingTimeGet(&ageTime);
    if (0 != ret)
    {
        paddr->rtn_code = 2;  //fail
        return ;
    }

    offset += 4;
    vosTlvSetInt(&(paddr->data[offset]), ageTime);
    paddr->rtn_code = 0;
    return ;
}

void IpcHalMacAgingTimeSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 ageTime;
	int offset = 0;
	int ret = 0;

	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	ageTime = vosTlvIntGet(&(paddr->data[offset]), &offset);

    VOS_APP_ACCESS_LOCK;
	ret = odmFdbAgingTimeSet(ageTime);
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
        paddr->rtn_code = 2;	//fail
	}
	else
	{
        paddr->rtn_code = 1;	//success
	}
	return ;

}

void IpcHalMacNumGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int filter;  //0:all 1:dynamic 2:static
	int page_num;  //PAGE_SIZE 50
	int port_id;  //0:all

	int total_num;
	int type;
    int port_vec;

	int offset = 0;
	int ret = 0;

	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	filter = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	page_num = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	port_id = vosTlvIntGet(&(paddr->data[offset]), &offset);
	ret = dalArlMacEntryNumGet(filter, page_num, port_id, &total_num);

	if (0 != ret)
	{
        paddr->rtn_code = 2;	//fail
        return ;
	}
	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), total_num);

	paddr->rtn_code = 0;
	return ;
}



void IpcHalMacGetMacList(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int filter;  //0:all 1:dynamic 2:static
	int page_num;  //PAGE_SIZE 50
	int port_id;  //0:all

	int total_num;
	int type;
    int port_vec;

	int offset = 0;
	int ret = 0;

	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	filter = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	page_num = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	port_id = vosTlvIntGet(&(paddr->data[offset]), &offset);
	ret = dalArlMacEntryShowWeb(filter, page_num, port_id, &total_num, &(paddr->data[36]));

	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	  return ;
	}
	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), total_num);

	paddr->rtn_code = 0;
	return ;
}


void IpcHalMacLearnLimitGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int limitStatus;
	int offset = 0;
	int ret = 0;

     paddr = (VOS_TLV_MSG_t *)pPar;
	 ret = odmFdbSwLearnGet(&limitStatus);
	 if (0 != ret)
	 {
	   paddr->rtn_code = 2;  //fail
	   return ;
	 }

     offset += 4;
     vosTlvSetInt(&(paddr->data[offset]), limitStatus);
     paddr->rtn_code = 0;
     return ;


}

void IpcHalMacLearnLimitSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 limitStatus;
	int offset = 0;
	int ret = 0;

	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	limitStatus = vosTlvIntGet(&(paddr->data[offset]), &offset);

    VOS_APP_ACCESS_LOCK;
	ret = odmFdbSwLearnSet(limitStatus);
    VOS_APP_ACCESS_UNLOCK;

	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}

void IpcHalPortMirrorSrcStateGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;
	UINT32 direction;
	UINT32 state;
	int offset = 0;
	int ret = 0;
	UINT32 phyPort;
	BOOL enable = 0;
	BOOL   enable_tmp = 0;

	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	direction = vosTlvIntGet(&(paddr->data[offset]), &offset);


    if (portId <= 4) ///except the uplink port
    {
    	if(0 == direction )
    	{
            ret = odmPortMirrorIngPortGet(portId,&enable);
            if (0 != ret)
            {
                paddr->rtn_code = 2;  //fail
            }
            else
            {
                paddr->rtn_code = 0;  //success
            }
    	}
    	else
    	{
            ret = odmPortMirrorEgPortGet(portId,&enable);
            if (0 != ret)
            {
                paddr->rtn_code = 2;  //fail
            }
            else
            {
                paddr->rtn_code = 0;  //success
            }
    	}
    }else
    {
        paddr->rtn_code = 0;  //success
    }
    offset += 4;
    vosTlvSetInt(&(paddr->data[offset]), enable);
	paddr->rtn_code =0;
	return ;
}

void IpcHalPortMirrorSrcStateSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;	
	UINT32 direction;
	UINT32 state;
	int offset = 0;
	int ret = 0;
	UINT32 phyPort;
	BOOL enable = 0;
	BOOL  state_tmp ;
    UINT8    portid_tmp;

	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	direction = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	state = vosTlvIntGet(&(paddr->data[offset]), &offset);
    if (portId <= 4)//except the uplink port
    {
        if(direction == 0)
        {
            ret = odmPortMirrorIngPortSet(portId,state);
            if (0 != ret)
            {
                paddr->rtn_code = 2;  //fail
            }
            else
            {
                paddr->rtn_code = 1;  //success
            }
        }else
        {
            ret = odmPortMirrorEgPortSet(portId,state);		
            if (0 != ret)
            {
                paddr->rtn_code = 2;  //fail
            }
            else
            {
                paddr->rtn_code = 1;  //success
            }
        }
    }else
    {
        paddr->rtn_code = 1;  //success
    }

	return ;
}

void IpcHalPortMirrorDestPortGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT8 portId;
	UINT32 direction;
	UINT32 state;
	int offset = 0;
	int ret = 0;
	BOOL enable;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	direction = vosTlvIntGet(&(paddr->data[offset]), &offset);

    ret = odmPortMirrorPortGet(&portId);    
    if (0 != ret)
    {
        paddr->rtn_code = 2;  //fail
    }

    offset += 4;
    vosTlvSetInt(&(paddr->data[offset]), (UINT32)portId);

    paddr->rtn_code = 0;  //success

	return ;
}

void IpcHalPortMirrorDestPortSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 portId;	
	UINT32 direction;
	UINT32 state;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	direction = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;
	portId = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;

    ret = odmPortMirrorPortSet(portId);     
    if (0 != ret)
    {
        paddr->rtn_code = 2;  //fail
    }
    else
    {
        paddr->rtn_code = 1;  //success
    }

	return ;
}

void IpcHalOnuModeGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int onuMode;
	int offset = 0;
	int ret = 0;
	
    paddr = (VOS_TLV_MSG_t *)pPar;
    ret = 0; /* odm**** Get(&onuMode);*/
    onuMode=1;
    if (0 != ret)
    {
        paddr->rtn_code = 2;  //fail
        return ;
    }
	 
    offset += 4;
    vosTlvSetInt(&(paddr->data[offset]), onuMode);
    paddr->rtn_code = 0;
    return ;
}

void IpcHalOnuModeSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int onuMode;
	int offset = 0;
	int ret = 0;
	
    paddr = (VOS_TLV_MSG_t *)pPar;
    offset += 4;
    onuMode = vosTlvIntGet(&(paddr->data[offset]), &offset);
    ret = 0;
    if (0 != ret)
    {
        paddr->rtn_code = 2;	//fail
    }
    else
    {
        paddr->rtn_code = 1;	//success
    }

     return ;
}


void IpcHalFlashErase(void *pPar,int length)
{
	return ;
}

void IpcHalFlashWrite(void *pPar,int length)
{
	return ;
}

void IpcHalEmapperGet(void *pPar,int length)
{
	return ;
}

void IpcHalEmapperSet(void *pPar,int length)
{
	return ;
}

void IpcHalFlashImageActiveGet(void *pPar,int length)
{
	return ;
}

void IpcHalFlashImageActiveSet(void *pPar,int length)
{
	return ;
}

void IpcHalFlashImageBurn(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
    UINT8 bootFlag;

	paddr = (VOS_TLV_MSG_t *)pPar;	
	
    bootFlag = vosConfigBootFlagGet(FLASH_BOOT_FLAG_NORMAL);
    if (FLASH_BOOT_OS2_FLAG == bootFlag)
    {
#if 0  /* modified by Gan Zhiheng - 2009/11/19 */
        vosSystem("mtd erase "FLASH_DEV_NAME_OS1);
		vosSystem("mtd write /var/firmware.bin "FLASH_DEV_NAME_OS1);
#else
        printf("write flash start at %u\n", vosTimeGet());
        write_flash("/var/delheaderfirmware.bin", FLASH_DEV_NAME_OS1);
        printf("write flash end at %u\n", vosTimeGet());
#endif /* #if 0 */
        vosSystem("rm -f /var/firmware.bin ");
        vosSystem("rm -f /var/delheaderfirmware.bin ");
		vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS1_FLAG);
    }
    else {
#if 0  /* modified by Gan Zhiheng - 2009/11/19 */
        vosSystem("mtd erase "FLASH_DEV_NAME_OS2);
        vosSystem("mtd write /var/firmware.bin "FLASH_DEV_NAME_OS2);
#else
        printf("write flash start at %u\n", vosTimeGet());
        write_flash("/var/delheaderfirmware.bin", FLASH_DEV_NAME_OS2);
        printf("write flash end at %u\n", vosTimeGet());
#endif /* #if 0 */
        vosSystem("rm -f /var/firmware.bin ");
        vosSystem("rm -f /var/delheaderfirmware.bin ");
		vosConfigBootFlagSet(FLASH_BOOT_FLAG_NORMAL, FLASH_BOOT_OS2_FLAG);
    }
	
	paddr->rtn_code = 1;	
	return ;
}

void IpcHalLaserOnEnableGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 enable;
	int offset = 0;
	int ret = 0;
	
    paddr = (VOS_TLV_MSG_t *)pPar;

    oplRegFieldRead(REG_PMAC_LASER_ON_CFG, 0, 4, &enable);
    if (!(enable & OPCONN_BIT(3)) &&
        !(enable & OPCONN_BIT(2)) &&
        (enable & OPCONN_BIT(1)) &&
        !(enable & OPCONN_BIT(0)))
    {
        enable = 1;
    }
    else
    {
        enable = 0;
    }
    if (0 != ret)
    {
        paddr->rtn_code = 2;  //fail
        return ;
    }

    offset += 4;
    vosTlvSetInt(&(paddr->data[offset]), enable);
    paddr->rtn_code = 0;
    return ;

}

void IpcHalLaserOnEnableSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 enable;
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	enable = vosTlvIntGet(&(paddr->data[offset]), &offset);

    /* Laser on enable just use for testing, we dont need save configuration */
    if (enable)
    {
        oplRegFieldWrite(REG_PMAC_LASER_ON_CFG, 0, 4, 2);
    }
    else
    {
        oplRegFieldWrite(REG_PMAC_LASER_ON_CFG, 0, 4, 1);
    }

	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}

void IpcHalLedConfigSet(void *pPar,int length)
{
	return ;
}


void IpcHalSipDevConfigGet(void *pPar,int length)
{

	return ;
}

void IpcHalSipDevConfigSet(void *pPar,int length)
{
	return ;
}

void IpcHalResetONU(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int ret;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	ret = odmRspSysReset();
	if (0 != ret)
	{
	  paddr->rtn_code = 2;  //fail
	}
	else
	{
	  paddr->rtn_code = 1;  //success
	}
	return ;
}

void IpcHalResetONUFactory(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	vosConfigErase();
    odmUserCfgInit();
	paddr->rtn_code = 1;  //success
	return ;
}


void IpcHalOnuConfigSave(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	vosConfigSave(NULL);

	paddr->rtn_code = 1;  //success
	return ;
}

#define WEB_EXPORT_CONFIG_FILE_NAME "/www/html/webconfigfile.tar.gz"
#define WEB_UPLOAD_CONFIG_FILE      "/var/webconfigfile.tar.gz"

void IpcHalOnuConfigExport(void *pPar, int length)
{
	VOS_TLV_MSG_t *paddr;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	vosConfigExportByWeb(WEB_EXPORT_CONFIG_FILE_NAME);

	printf("\r\n Export config file success!");
	paddr->rtn_code = 1;  //success
	return ;
}

void IpcHalOnuConfigBurn(void *pPar, int length)
{
	VOS_TLV_MSG_t *paddr;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	vosConfigSave(WEB_UPLOAD_CONFIG_FILE);

	printf("\r\n Burn config file success!");
	paddr->rtn_code = 1;  //success
	return ;
}

void IpcHalAdminSecurityConfigSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int ret;
	int offset = 0;
	char name[32] ={0};
	char passwd[32] ={0};
	char tmpPasswd[32] ={0};
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), name,32,&offset);	
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), passwd,32,&offset);	
	if(0 == odmUserExistCheck(name))
	{
        VOS_APP_ACCESS_LOCK;
        if(odmUserPasswordSet(name,passwd,NULL)!= 0)
        {
            paddr->rtn_code = 2;  //fail
        }
        paddr->rtn_code = 1;  //success
        VOS_APP_ACCESS_UNLOCK;
	}
	else{
		if(odmUserAddWithAllParams(name,passwd,ENABLED,ACCESS_LEVEL_ADMINISTRATION,ENGLISH,NULL)!= 0)
		{
			paddr->rtn_code = 2;  //fail
		}
		paddr->rtn_code = 1;  //success

		}
	return ;
}

void IpcHalUserSecurityConfigSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	int ret;
	int offset = 0;
	char name[32] ={0};
	char passwd[32] ={0};
	char tmpPasswd[32] ={0};
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), name,32,&offset);	
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), passwd,32,&offset);
	if(odmUserExistCheck(name)== 0)
	{
        VOS_APP_ACCESS_LOCK;
        if(odmUserPasswordSet(name,passwd,NULL)!= 0)
        {
            paddr->rtn_code = 2;  //fail
        }
        paddr->rtn_code = 1;  //success
        VOS_APP_ACCESS_UNLOCK;
	}
	else{
		if(odmUserAddWithAllParams(name,passwd,ENABLED,ACCESS_LEVEL_USER,ENGLISH,NULL)!= 0)
		{
			paddr->rtn_code = 2;  //fail
		}
		paddr->rtn_code = 1;  

		}
	return ;

}

#if 1 /* add by zttan for TW project */

void IpcHalVlanInfoGet(void *pPar,int length)
{
 	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	int ret = 0;  
	UINT32 vlanNum = 0;
	char vlanInfoBuff[1024];
	vlan_info_t * vlanInfo;

	paddr = (VOS_TLV_MSG_t *)pPar;
	
	vlanInfo = (vlan_info_t *)vlanInfoBuff;
	odmGetVlanListInfo(&vlanNum, vlanInfo);		

	int index;
	for (index = 0; index < vlanNum; index++)
	{
		vlanInfo++;
	}

	offset += 4;
	vosTlvSetInt(&(paddr->data[offset]), vlanNum);
	offset += 8;

	vosTlvSetArray(&(paddr->data[offset]), vlanInfoBuff, sizeof(vlan_info_t) * vlanNum);
	paddr->rtn_code = 0;    
	return ;
}

void IpcHalRealStatisticGet(void *pPar, int length)
{
	VOS_TLV_MSG_t *paddr;
	int offset = 4;	
	
	paddr = (VOS_TLV_MSG_t *)pPar;

	paddr->rtn_code = 0;

	return ;
}

void IpcHalRealStatisticClear(void *pPar, int length)
{
	UINT8 port_id = 0;
	VOS_TLV_MSG_t *paddr;
	int offset = 4;

	paddr = (VOS_TLV_MSG_t *)pPar;
	port_id = vosTlvIntGet(&(paddr->data[offset]), &offset);

	if (port_id > SWITCH_PORT_NUM)
	{
		paddr->rtn_code = 2;
		return ;
	}

	if (0 == port_id)
	{
		CntClr(DAL_CNT_POLL_PON_MASK, 0, 0);
	}
	else
	{
		CntClr(DAL_CNT_POLL_SWITCH_MASK, port_id, 1);
	}

	paddr->rtn_code = 1;	
	return ;
}

void IpcHalCtcStackConfigGet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	CTC_STACK_config_t ctcStackConfigInfo;
	int offset = 4;	
	
	paddr = (VOS_TLV_MSG_t *)pPar;

	vosMemSet(&ctcStackConfigInfo, 0x00, sizeof(CTC_STACK_config_t));
	if (odmPortCtcStackConfigGet(&ctcStackConfigInfo) != OK)
	{
		paddr->rtn_code = 2;
		return ;
	}

	vosTlvSetArray(&(paddr->data[offset]), (char *)&ctcStackConfigInfo, sizeof(CTC_STACK_config_t));

	paddr->rtn_code = 0;
	return ;
}

void IpcHalVlanMemberSet(void *pPar,int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 vid;
	int portId;
	int membTag;
	int offset = 0;
	int ret = 0;
	int type = 0;
	char section[256] = {0};
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
    vid = vosTlvIntGet(&(paddr->data[offset]), &offset);
	offset += 4;	
    membTag = vosTlvIntGet(&(paddr->data[offset]), &offset);

	vosSprintf(section, "Webvlan%0.4d", vid);
	
	/* add a new vlan */
	ret = odmCreateVlanName(section);
	if (0 != ret)
	{
		paddr->rtn_code = 2;  //fail
		return ;
	}
	
	ret = odmSetVlanNameTag(section, vid);
	if (0 != ret)
	{
		paddr->rtn_code = 2;  //fail
		return ;
	}	
	
	for(portId =1;portId<=4;portId++)
	{
		if(((membTag>>(portId-1))&0x1) == 1)
		{
			type = UNMODIFY;
		}
		else
		{
			continue;
		}

		ret = odmAddVlanPort(portId,vid,type); 
		if (0 != ret)
		{
			paddr->rtn_code = 2;  //fail
		}
		else
		{
			paddr->rtn_code = 1;  //success
		}
	
	}

    return;

}

void IpcHalVlanDel(void *pPar, int length)
{
	VOS_TLV_MSG_t *paddr;
	UINT32 vid;
	int offset = 0;
	int ret = 0;
	char section[16] = {0};

	paddr = (VOS_TLV_MSG_t *)pPar;
	
	offset += 4;
    vid = vosTlvIntGet(&(paddr->data[offset]), &offset);

	ret = odmDeleteVlanID(vid);
	if (0 != ret)
	{
		paddr->rtn_code = 2;
	}

	vosSprintf(section, "Webvlan%0.4d", vid);
	ret = odmDeleteVlanName(section);
	if (0 != ret)
	{
		paddr->rtn_code = 2;
	}	

	paddr->rtn_code = 1;
	return ;
}

#if 0
void IpcHalPortLinkStateGet(void *pPar, int length)
{
	UINT8 port_id = 0;
	VOS_TLV_MSG_t *paddr;
	int offset = 4;
	int status;

	paddr = (VOS_TLV_MSG_t *)pPar;
	port_id = vosTlvIntGet(&(paddr->data[offset]), &offset);
	
	if (port_id > SWITCH_PORT_NUM)
	{
		paddr->rtn_code = -2;
		return ;
	}

	odmPortAdminGet(port_id, &status);
	vosTlvSetInt(&(paddr->data[offset]), status);
	paddr->rtn_code = 0;
	return ;
}

void IpcHalPortVlanModeGet(void *pPar, int length)
{
	UINT8 port_id = 0;
	VOS_TLV_MSG_t *paddr;
	int offset = 4;
	int vlan_mode;

	paddr = (VOS_TLV_MSG_t *)pPar;
	port_id = vosTlvIntGet(&(paddr->data[offset]), &offset);
	
	if (port_id > SWITCH_PORT_NUM)
	{
		paddr->rtn_code = -2;
		return ;
	}

	odmPortVlanModeGet(port_id, &vlan_mode);
	vosTlvSetInt(&(paddr->data[offset]), vlan_mode);
	paddr->rtn_code = 0;
	return ;
}

void IpcHalPortVlanModeSet(void *pPar, int length)
{
	UINT8 port_id = 0;
	UINT32 vlan_mode;	
	VOS_TLV_MSG_t *paddr;
	int offset = 4;

	paddr = (VOS_TLV_MSG_t *)pPar;
	port_id = vosTlvIntGet(&(paddr->data[offset]), &offset);
	vlan_mode = vosTlvIntGet(&(paddr->data[offset]), &offset);	
	
	if (port_id > SWITCH_PORT_NUM)
	{
		paddr->rtn_code = -2;
		return ;
	}

	odmPortVlanModeSet(port_id, vlan_mode);
	paddr->rtn_code = 0;
	return ;
}
#endif

#endif

//Eric Yang add for TW

typedef struct IPC_INTERFACE_CFG_s
{
	int      nIf;
    int      nIfMode;
	int      enable;
	int      vlan_enable;
	int	     vlanId;
	int	     vlanPrio; 
	int      type;
	char     option60[32];
	char	 username[32];
	char	 password[32];
	char	 ip[16];
	char	 mask[16];
	char	 gateway[16];
	char	 dns[16];
	char	 devname[32];
}IPC_INTERFACE_CFG_t;

extern const char *g_acIfEthNameStr[INTERFACE_MAX*INTERFACE_MODE_MAX];

void IpcHalIfCfgDataGet(void *pPar,int length)
{
    VOS_TLV_MSG_t *paddr;
    int offset = 0;
    int ret = 0;
    int index = 0;
    IPC_INTERFACE_CFG_t *pIfCfg;
    ODM_SYS_IF_CONFIG_t stOdmIfCfg;

    memset(&stOdmIfCfg,0,sizeof(ODM_SYS_IF_CONFIG_t));

    paddr = (VOS_TLV_MSG_t *)pPar;

 	offset += 4;
	pIfCfg = (IPC_INTERFACE_CFG_t *)(&(paddr->data[offset]));

    stOdmIfCfg.ucIf = pIfCfg->nIf;
    stOdmIfCfg.ucIfMode = pIfCfg->nIfMode;

    index = pIfCfg->nIf * INTERFACE_MODE_MAX + pIfCfg->nIfMode;

    if (index >= INTERFACE_MAX*INTERFACE_MODE_MAX)
    {
        paddr->rtn_code = 2;  //failed
        return ;
    }
    odmSysIfConfigGet(&stOdmIfCfg);

    //set the ENABLE mode

    pIfCfg->enable = stOdmIfCfg.ucIfShareMode;
    pIfCfg->type = stOdmIfCfg.ucIpMode;

    pIfCfg->vlan_enable = stOdmIfCfg.ucTagged;
    pIfCfg->vlanId = stOdmIfCfg.usCVlan;
    pIfCfg->vlanPrio = stOdmIfCfg.ucPriority;

    ///be lack of option60

    vosStrCpy(pIfCfg->username, stOdmIfCfg.acPPPoEUser);
    vosStrCpy(pIfCfg->password, stOdmIfCfg.acPPPoEPass);

    cliIpToStr(stOdmIfCfg.ulIpAddr, pIfCfg->ip);
    cliIpToStr(stOdmIfCfg.ulIpMask, pIfCfg->mask);
    cliIpToStr(stOdmIfCfg.ulGwAddr, pIfCfg->gateway);
    cliIpToStr(stOdmIfCfg.ulDnsAddr, pIfCfg->dns);

    /// be lack of dev name 
    vosStrCpy(pIfCfg->devname, g_acIfEthNameStr[index]);

#if 0
    printf("{name:\"%s\",ifmode:%d,enable:%d,type:%d,ip:\"%s\","
            "netmask:\"%s\",gateway:\"%s\",dns:\"%s\",vlan_enable:%d,vlan_id:%d,vlan_prio:%d", 
            pIfCfg->devname,pIfCfg->nIfMode, pIfCfg->enable, pIfCfg->type, 
            pIfCfg->ip, pIfCfg->mask, 
            pIfCfg->gateway, pIfCfg->dns,
            pIfCfg->vlan_enable,
            pIfCfg->vlanId, pIfCfg->vlanPrio);
    /*  PPPoE 添加用户名和密码  */
    if (pIfCfg->type == IF_IP_MODE_PPPOE)
    {
        printf(",username:\"%s\",password:\"%s\"}\r\n", pIfCfg->username, pIfCfg->password);
    }
    /*  dhcp 添加option60*/
    else if (pIfCfg->type == IF_IP_MODE_DHCP)
    {
        printf(",option60:\"%s\"}\r\n", pIfCfg->option60);
    }
    else
    {
        printf("}\r\n");
    }       
#endif

    paddr->rtn_code = 0;  //success
    return ;
}

void IpcHalIfCfgDataSet(void *pPar,int length)
{
    VOS_TLV_MSG_t *paddr;
    int offset = 0;
    UINT32 ret = 0;
    int index = 0;
    IPC_INTERFACE_CFG_t *pIfCfg;
    ODM_SYS_IF_CONFIG_t stOdmIfCfg;

    memset(&stOdmIfCfg,0,sizeof(ODM_SYS_IF_CONFIG_t));

    paddr = (VOS_TLV_MSG_t *)pPar;

 	offset += 4;
	pIfCfg = (IPC_INTERFACE_CFG_t *)(&(paddr->data[offset]));

    stOdmIfCfg.ucIf = pIfCfg->nIf;
    stOdmIfCfg.ucIfMode = pIfCfg->nIfMode;

    index = pIfCfg->nIf * INTERFACE_MODE_MAX + pIfCfg->nIfMode;

    if (index >= INTERFACE_MAX*INTERFACE_MODE_MAX)
    {
        paddr->rtn_code = 2;  //failed
        return ;
    }

    //set the ENABLE mode

    stOdmIfCfg.ucIfShareMode = (UINT8)pIfCfg->enable;
    stOdmIfCfg.ucIpMode = (UINT8)pIfCfg->type;
    stOdmIfCfg.ucTagged = (UINT8)pIfCfg->vlan_enable;
    stOdmIfCfg.usCVlan = (UINT16)pIfCfg->vlanId;
    stOdmIfCfg.ucPriority = (UINT8)pIfCfg->vlanPrio;

    ///be lack of option60

    vosStrCpy(stOdmIfCfg.acPPPoEUser, pIfCfg->username);
    vosStrCpy(stOdmIfCfg.acPPPoEPass, pIfCfg->password);

    cliStrToIp(pIfCfg->ip, (ULONG *)&stOdmIfCfg.ulIpAddr);
    cliStrToIp(pIfCfg->mask, (ULONG *)&stOdmIfCfg.ulIpMask);
    cliStrToIp(pIfCfg->gateway, (ULONG *)&stOdmIfCfg.ulGwAddr);
    cliStrToIp(pIfCfg->dns, (ULONG *)&stOdmIfCfg.ulDnsAddr);

    VOS_APP_ACCESS_LOCK;
    ret += odmSysIfConfigSet(&stOdmIfCfg);	
    VOS_APP_ACCESS_UNLOCK;

    if (NO_ERROR == ret)
    {
        paddr->rtn_code = 1;  //success
    }
    else {
        paddr->rtn_code = 2;  //failed
    }
    return ;
}

//!EricYang

void IpcHalOnuLOIDGet(void *pPar,int length) /*need to fix*/
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	int ret = 0;
	char loid[32] = {0};
	int loid_len;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	ret = odmPonLoidGet(loid);
	offset = vosTlvArraySet(&(paddr->data[offset]), 1, loid , vosStrLen(loid), offset);
	paddr->rtn_code = 0;

	return ;
}

void IpcHalOnuLOIDSet(void *pPar,int length) /*need to fix*/
{
	VOS_TLV_MSG_t *paddr;
	char loid[32] = {0};
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), loid, 32, &offset);
	
	ret = odmPonLoidSet(loid);

	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}

void IpcHalOnuLOIDPasswdGet(void *pPar,int length) /*need to fix*/
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	int ret = 0;
	char passwd[32] = {0};
	int loid_len;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	ret = odmPonPasswordGet(passwd);
	odmUserEncryptString(passwd);/*decrypt*/

	offset = vosTlvArraySet(&(paddr->data[offset]), 1, passwd , vosStrLen(passwd), offset);
	paddr->rtn_code = 0;

    return ;
}

void IpcHalOnuLOIDPasswdSet(void *pPar,int length) /*need to fix*/
{
	VOS_TLV_MSG_t *paddr;
	char passwd[32] = {0};
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), passwd, 32, &offset);
	
	ret = odmPonPasswordSet(passwd);

	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}

void IpcHalWebPrintfSet(void *pPar,int length) /*need to fix*/
{
	VOS_TLV_MSG_t *paddr;
	char msg[256] = {0};
	int offset = 0;
	int ret = 0;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	vosTlvArrayGet(&(paddr->data[offset]), msg, 256, &offset);
	
	printf("%s",msg);
	if (0 != ret)
	{
	  paddr->rtn_code = 2;	//fail
	}
	else
	{
	  paddr->rtn_code = 1;	//success
	}
	return ;

}


void IpcHalOnuBuildTimeGet(void *pPar,int length) /*need to fix*/
{
	VOS_TLV_MSG_t *paddr;
	int offset = 0;
	int ret = 0;
	char buildTime[32] = {0};
	int loid_len;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	vosSprintf(buildTime, "%s %s", build_time,build_date);
	offset = vosTlvArraySet(&(paddr->data[offset]), 1, buildTime , vosStrLen(buildTime), offset);
	paddr->rtn_code = 0;
    return ;
}

typedef struct oam_debug_type_s{

    UINT32   type;
    UINT32   data_length;
    UINT8    data[992];

}oam_debug_t;

/*add by lizhe for oam debug*/
void IpcHalOamDebug(void *pPar,int length) 
{
	VOS_TLV_MSG_t *paddr;
	oam_debug_t *frame_buff;
 
	int offset = 0;
	int ret = 0;
	int frame_len;
	
	paddr = (VOS_TLV_MSG_t *)pPar;
	offset += 4;
	frame_buff = &paddr->data[offset];
	
	switch(frame_buff->type)
	{
		case 0:
			eopl_ctc_pdu_process_2_0(frame_buff->data, frame_buff->data_length);/*when the onu =0x111111*/
			break;
		case 1:
#ifdef OAM_SUPPORT_ZTE_EXT
			eopl_zte_pdu_process(frame_buff->data, frame_buff->data_length);/*when oui = 0x0f3e*/
#endif
			break;
		case 2:
			eopl_ctc_pdu_process(frame_buff->data, frame_buff->data_length);/*when ctc2.1*/
			break;
		default:
			eopl_ctc_pdu_process_2_0(frame_buff->data, frame_buff->data_length);/*when the onu =0x111111*/
			break;
	}
	
	offset = 0;
	offset = vosTlvArraySet(&(paddr->data[offset]), 1, NULL , 0, offset);
	paddr->rtn_code = 0;
	return ;
}


IPC_HAL_RESULT	IpcHalInit()
{
    if (s_nIpcHalInitialized) 
        return NO_ERROR;
    if (VOS_MUTEX_OK != vosMutexCreate(&s_pstIpcDrvLock))
    {
        fprintf(stderr,"CRITICAL ERROR, Out of memory\r\n");
        return ERROR;
    }	
    s_nIpcHalInitialized = 1;
	IpcHalRegister();
	//printf("IpcHalInit init successful\n");
    return NO_ERROR;
}

IPC_HAL_RESULT  IpcHalDrvApiRegister(OPL_IPC_API_UNIT_t *pstApiUnit)
{
    IPCHAL_CHECK_RETURN;
	
    IPC_DEVICE_DRV_LOCK;
    vosMemCpy(&s_IpcHalpstDevDrv.ipcarrayAPI[pstApiUnit->apiId],pstApiUnit,sizeof(OPL_IPC_API_UNIT_t));
    IPC_DEVICE_DRV_UNLOCK;
	
	return NO_ERROR;
}

IPC_HAL_RESULT IpcHalRegister(void)
{
	IPC_HAL_RESULT		iStatus=0;
	OPL_IPC_API_UNIT_t	stApiUnit;

	stApiUnit.apiId = OP_ONU_STATUS_GET;
	stApiUnit.apiFunc = IpcHalOnuStatusGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_ONU_VERSION_GET;
	stApiUnit.apiFunc = IpcHalOnuVersionGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_ONU_UPTIME_GET;
	stApiUnit.apiFunc = IpcHalOnuUptimeGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_ONU_CAPABILITIY_GET;
	stApiUnit.apiFunc = IpcHalOnuCapabilityGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
/*pippo add for ctc2.1 for web*/
	stApiUnit.apiId = OP_ONU_CAPABILITIY2_GET;
	stApiUnit.apiFunc = IpcHalOnuCapability2Get;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
/*end add*/

/*pippo add for get ctc verser for web*/
	stApiUnit.apiId = OP_ONU_CTC_VER_GET;
	stApiUnit.apiFunc = IpcHalOnuCtcVerGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
/*end add*/

	stApiUnit.apiId = OP_ONU_CHIPSET_GET;
	stApiUnit.apiFunc = IpcHalOnuChipsetGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_ONU_CHECKSUM_GET;
	stApiUnit.apiFunc = IpcHalOnuChecksumGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = OP_ONU_CTC_STATUS_GET;
    stApiUnit.apiFunc = IpcHalOnuCtcStatusGet;
    iStatus += IpcHalDrvApiRegister(&stApiUnit);
    
    stApiUnit.apiId = OP_ONU_CTC_STATUS_SET;
    stApiUnit.apiFunc = IpcHalOnuCtcStatusSet;
    iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
    stApiUnit.apiId = OP_ONU_MACADDRESS_GET;
    stApiUnit.apiFunc = IpcHalOnuMacAddressGet;
    iStatus += IpcHalDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = OP_ONU_OUI_GET;
    stApiUnit.apiFunc = IpcHalOnuOuiGet;
    iStatus += IpcHalDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = OP_ONU_OUI_SET;
    stApiUnit.apiFunc = IpcHalOnuOuiSet;
    iStatus += IpcHalDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = OP_ONU_SN_GET;
    stApiUnit.apiFunc = IpcHalOnuSnGet;
    iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
    stApiUnit.apiId = OP_ONU_SN_SET;
    stApiUnit.apiFunc = IpcHalOnuSnSet;
    iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_ONU_PON_MAC_GET;
	stApiUnit.apiFunc = IpcHalOnuPonMacGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = OP_ONU_MACADDRESS_SET;
    stApiUnit.apiFunc = IpcHalOnuMacAddressSet;
    iStatus += IpcHalDrvApiRegister(&stApiUnit);	
	
	stApiUnit.apiId = OP_ONU_FECMODE_GET;
	stApiUnit.apiFunc = IpcHalOnuFecModeGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_ONU_FECMODE_SET;
	stApiUnit.apiFunc = IpcHalOnuFecModeSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_BPDU_TRANS_GET;
	stApiUnit.apiFunc = IpcHalBpduTransparentGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_BPDU_TRANS_SET;
	stApiUnit.apiFunc = IpcHalBpduTransparentSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_IPADDR_GET;
	stApiUnit.apiFunc = IpcHalIpAddrGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_IPMASK_GET;
	stApiUnit.apiFunc = IpcHalIpMaskGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_IPADDR_SET;
	stApiUnit.apiFunc = IpcHalIpAddrSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_IPMASK_SET;
	stApiUnit.apiFunc = IpcHalIpMaskSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_IP_GATEWAY_GET;
	stApiUnit.apiFunc = IpcHalIpGatewayGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_IP_GATEWAY_SET;
	stApiUnit.apiFunc = IpcHalIpGatewaySet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_IP_DNS_GET;
	stApiUnit.apiFunc = IpcHalIpDnsGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_IP_DNS_SET;
	stApiUnit.apiFunc = IpcHalIpDnsSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_PON_MTU_SET;
	stApiUnit.apiFunc = IpcHalPonMtuSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PON_MTU_GET;
	stApiUnit.apiFunc = IpcHalPonMtuGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_FRAM_FILTER_GET;
	stApiUnit.apiFunc = IpcHalFrameFilterGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_FRAM_FILTER_SET;
	stApiUnit.apiFunc = IpcHalFrameFilterSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_FRAM_FILTER_DEL;
	stApiUnit.apiFunc = IpcHalFrameFilterDel;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_PORT_MAC_BIND_GET;
	stApiUnit.apiFunc = IpcHalPortMacBindGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_PORT_MAC_BIND_SET;
	stApiUnit.apiFunc = IpcHalPortMacBindSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_PORT_MAC_BIND_DEL;
	stApiUnit.apiFunc = IpcHalPortMacBindDel;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_PORT_MAC_FILTER_GET;
	stApiUnit.apiFunc = IpcHalPortMacFilterGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_PORT_MAC_FILTER_SET;
	stApiUnit.apiFunc = IpcHalPortMacFilterSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_PORT_MAC_FILTER_DEL;
	stApiUnit.apiFunc = IpcHalPortMacFilterDel;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_MAC_LIMIT_STATUS_GET;
	stApiUnit.apiFunc = IpcHalMacLimitStatusGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_MAC_LIMIT_STATUS_SET;
	stApiUnit.apiFunc = IpcHalMacLimitStatusSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_PORT_MACLIMIT_NUM_GET;
	stApiUnit.apiFunc = IpcHalPortMacLimitGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PORT_MACLIMIT_NUM_SET;
	stApiUnit.apiFunc = IpcHalPortMacLimitSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_MAC_BIND_STATUS_GET;
	stApiUnit.apiFunc = IpcHalMacBindStatusGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_MAC_BIND_STATUS_SET;
	stApiUnit.apiFunc = IpcHalMacBindStatusSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PQUEUE_INGRESS_LIMIT_GET;
	stApiUnit.apiFunc = IpcHalPqueueIngressLimitGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PQUEUE_QUEUE_BLOCK_SET;
	stApiUnit.apiFunc = IpcHalPqueueBlockSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_PQUEUE_INLIMIT_OCCUPANCY_GET;
	stApiUnit.apiFunc = IpcHalIngressQueueOccupancyGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PORT_ISOLATE_STATUS_GET;
	stApiUnit.apiFunc = IpcHalPortIsolationStatusGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PORT_ISOLATE_STATUS_SET;
	stApiUnit.apiFunc = IpcHalPortIsolationStatusSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_EXTEND_MTU_STATUS_GET;
	stApiUnit.apiFunc = IpcHalExtendMtuStatusGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_EXTEND_MTU_STATUS_SET;
	stApiUnit.apiFunc = IpcHalExtendMtuStatusSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PORT_SPEED_MODE_GET;
	stApiUnit.apiFunc = IpcHalPortSpeedModeGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PORT_SPEED_MODE_SET;
	stApiUnit.apiFunc = IpcHalPortSpeedModeSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_PORT_STORM_RESTRAIN_GET;
	stApiUnit.apiFunc = IpcHalPortStormRestrainGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PORT_STORM_RESTRAIN_SET;
	stApiUnit.apiFunc = IpcHalPortStormRestrainSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_PORT_CONF_GET;
	stApiUnit.apiFunc = IpcHalPortConfGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PORT_CONF_SET;
	stApiUnit.apiFunc = IpcHalPortConfSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_PORT_LINK_STATE_GET;
	stApiUnit.apiFunc = IpcHalPortLinkStateGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PORT_VLAN_MODE_GET;
	stApiUnit.apiFunc = IpcHalPortVlanModeGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PORT_PVID_GET;
	stApiUnit.apiFunc = IpcHalPortPvidGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PORT_VLAN_MODE_SET;
	stApiUnit.apiFunc = IpcHalPortVlanModeSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PORT_PVID_SET;
	stApiUnit.apiFunc = IpcHalPortPvidSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_VLAN_INFO_GET;
	stApiUnit.apiFunc = IpcHalVlanInfoGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_VLAN_MEMBER_SET;
	stApiUnit.apiFunc = IpcHalVlanMemberSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_VLAN_MEMBER_DEL;
	stApiUnit.apiFunc = IpcHalVlanDel;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);	
	
	stApiUnit.apiId = OP_QOS_SCHEDULING_MODE_GET;
	stApiUnit.apiFunc = IpcHalQosSchedulingModeGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_QOS_SCHEDULING_MODE_SET;
	stApiUnit.apiFunc = IpcHalQosSchedulingModeSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_POLICY_QUEUE_MAPPING_GET;
	stApiUnit.apiFunc = IpcHalPolicyQueueMappingGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_POLICY_QUEUE_MAPPING_SET;
	stApiUnit.apiFunc = IpcHalPolicyQueueMappingSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_QUEUE_MAPPING_ENABLE_GET;
	stApiUnit.apiFunc = IpcHalQueueMappingEnableGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_QUEUE_MAPPING_ENABLE_SET;
	stApiUnit.apiFunc = IpcHalQueueMappingEnableSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_VLAN_PRI_TRANSLATION_GET;
	stApiUnit.apiFunc = IpcHalVlanPriorityTranlationGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_VLAN_PRI_TRANSLATION_SET;
	stApiUnit.apiFunc = IpcHalVlanPriorityTranlationSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_IGMP_SNOOP_ENABLE_GET;
	stApiUnit.apiFunc = IpcHalIgmpSnoopEnableGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_IGMP_SNOOP_ENABLE_SET;
	stApiUnit.apiFunc = IpcHalIgmpSnoopEnableSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_IGMP_SNOOP_MAC_AGINGTIME_GET;
	stApiUnit.apiFunc = IpcHalIgmpSnoopAgeTimeGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_IGMP_SNOOP_MAC_AGINGTIME_SET;
	stApiUnit.apiFunc = IpcHalIgmpSnoopAgeTimeSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_MAC_AGING_TIME_GET;
	stApiUnit.apiFunc = IpcHalMacAgingTimeGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_MAC_AGING_TIME_SET;
	stApiUnit.apiFunc = IpcHalMacAgingTimeSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_MAC_NUM_GET;
	stApiUnit.apiFunc = IpcHalMacGetMacList;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);	

	stApiUnit.apiId = OP_MAC_GET_MAC_LIST;
	stApiUnit.apiFunc = IpcHalMacGetMacList;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);	
	
	stApiUnit.apiId = OP_MAC_LEARN_LIMIT_GET;
	stApiUnit.apiFunc = IpcHalMacLearnLimitGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_MAC_LEARN_LIMIT_SET;
	stApiUnit.apiFunc = IpcHalMacLearnLimitSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_PORT_MIRROR_SRC_STATE_GET;
	stApiUnit.apiFunc = IpcHalPortMirrorSrcStateGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PORT_MIRROR_SRC_STATE_SET;
	stApiUnit.apiFunc = IpcHalPortMirrorSrcStateSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PORT_MIRROR_DEST_PORT_GET;
	stApiUnit.apiFunc = IpcHalPortMirrorDestPortGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_PORT_MIRROR_DEST_PORT_SET;
	stApiUnit.apiFunc = IpcHalPortMirrorDestPortSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_ONU_MODE_GET;
	stApiUnit.apiFunc = IpcHalOnuModeGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_ONU_MODE_SET;
	stApiUnit.apiFunc = IpcHalOnuModeSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_FLASH_ERASE;
	stApiUnit.apiFunc =IpcHalFlashErase ;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_FLASH_WRITE;
	stApiUnit.apiFunc =IpcHalFlashWrite ;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_EMAPPER_GET;
	stApiUnit.apiFunc =IpcHalEmapperGet ;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_EMAPPER_SET;
	stApiUnit.apiFunc =IpcHalEmapperSet ;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_FLASH_IMAGE_ACTIVE_GET;
	stApiUnit.apiFunc =IpcHalFlashImageActiveGet ;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_FLASH_IMAGE_ACTIVE_SET;
	stApiUnit.apiFunc =IpcHalFlashImageActiveSet ;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_FLASH_IMAGE_BURN;
	stApiUnit.apiFunc =IpcHalFlashImageBurn ;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_ONU_LASER_ON_ENABLE_GET;
	stApiUnit.apiFunc =IpcHalLaserOnEnableGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_ONU_LASER_ON_ENABLE_SET;
	stApiUnit.apiFunc =IpcHalLaserOnEnableSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_ONU_LED_CONFIG_SET;
	stApiUnit.apiFunc =IpcHalLedConfigSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_SIP_DEV_CFG_DATA_GET;
	stApiUnit.apiFunc =IpcHalSipDevConfigGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_SIP_DEV_CFG_DATA_SET;
	stApiUnit.apiFunc =IpcHalSipDevConfigSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_RESET_ONU;
	stApiUnit.apiFunc =IpcHalResetONU ;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);
	
	stApiUnit.apiId = OP_RESET_ONU_TO_DEFAULT;
	stApiUnit.apiFunc = IpcHalResetONUFactory;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_ONU_SAVE_CONFIG;
	stApiUnit.apiFunc = IpcHalOnuConfigSave;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_ONU_EXPORT_CONFIG;
	stApiUnit.apiFunc = IpcHalOnuConfigExport;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);	

	stApiUnit.apiId = OP_ONU_BURN_CONFIG;
	stApiUnit.apiFunc = IpcHalOnuConfigBurn;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);		

	stApiUnit.apiId = OP_ADMIN_SECURITY_CONFIG_SET;
	stApiUnit.apiFunc = IpcHalAdminSecurityConfigSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_USER_SECURITY_CONFIG_SET;
	stApiUnit.apiFunc = IpcHalUserSecurityConfigSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	#if 1 /* add by zttan for TW project */
	stApiUnit.apiId = OP_REALTIME_STATISTICS_GET;
	stApiUnit.apiFunc = IpcHalRealStatisticGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_REALTIME_STATISTICS_CLEAR;
	stApiUnit.apiFunc = IpcHalRealStatisticClear;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);	

	stApiUnit.apiId = OP_CTC_STACK_CONFIG_GET;
	stApiUnit.apiFunc = IpcHalCtcStackConfigGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);	
	
	#endif

//EricYang add for TW
	stApiUnit.apiId = OP_IF_CFG_DATA_GET;
	stApiUnit.apiFunc = IpcHalIfCfgDataGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_IF_CFG_DATA_SET;
	stApiUnit.apiFunc = IpcHalIfCfgDataSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

//!EricYang

	stApiUnit.apiId = OP_LOID_GET;
	stApiUnit.apiFunc = IpcHalOnuLOIDGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_LOID_SET;
	stApiUnit.apiFunc = IpcHalOnuLOIDSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_LOID_PASSWD_GET;
	stApiUnit.apiFunc = IpcHalOnuLOIDPasswdGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_LOID_PASSWD_SET;
	stApiUnit.apiFunc = IpcHalOnuLOIDPasswdSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_ONU_BUILDTIME_GET;
	stApiUnit.apiFunc = IpcHalOnuBuildTimeGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);		
	
	stApiUnit.apiId = OP_PRINTF_SET;
	stApiUnit.apiFunc = IpcHalWebPrintfSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	stApiUnit.apiId = OP_PORT_STORM_USRATE_SET;
	stApiUnit.apiFunc = IpcHalPortStormUsRateSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);		

	stApiUnit.apiId = OP_PORT_STORM_USRATE_GET;
	stApiUnit.apiFunc = IpcHalPortStormUsRateGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);		

	stApiUnit.apiId = OP_PORT_STORM_DSRATE_SET;
	stApiUnit.apiFunc = IpcHalPortStormDsRateSet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);		

	stApiUnit.apiId = OP_PORT_STORM_DSRATE_GET;
	stApiUnit.apiFunc = IpcHalPortStormDsRateGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);		

//add by lizhe
	stApiUnit.apiId = OP_OAM_DEBUG;
	stApiUnit.apiFunc = IpcHalOamDebug;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);		

    stApiUnit.apiId = OP_ONU_FILTER_TOTALNUM_Get;
	stApiUnit.apiFunc = IpcHalFrameFilterTotalNumGet;
	iStatus += IpcHalDrvApiRegister(&stApiUnit);

	return iStatus;
}

IPC_HAL_RESULT IpcHalAppApiCallSync(OPL_IPC_DATA_t *pstData)
{

    IPC_HAL_FUNC pFunc;

    IPCHAL_CHECK_RETURN;
   
    IPC_DEVICE_DRV_LOCK; 
    pFunc = s_IpcHalpstDevDrv.ipcarrayAPI[pstData->apiId].apiFunc;
    if ( NULL == pFunc )
    {
        fprintf(stderr,"CRITICAL ERROR, IPC HAL API table manipulation WRONG, apiId:%d\n",pstData->apiId);
        IPC_DEVICE_DRV_UNLOCK;
        return ERROR;
    }

    pFunc( pstData->tlvbuff,pstData->tlvlen);

    IPC_DEVICE_DRV_UNLOCK;

    return NO_ERROR;
}

//add by wuxiao 2010.1.18
int  IpcHalTxFaultStutasGet(int *iTxFault)
{
	UINT32 enable;
	int ret = 0;
	
    ret = oplRegFieldRead(REG_PMAC_LASER_ON_CFG, 0, 4, &enable);

	if (0 != ret)
    {
        return -1;
    }
	
    if (!(enable & OPCONN_BIT(3)) &&
        !(enable & OPCONN_BIT(2)) &&
        (enable & OPCONN_BIT(1)) &&
        !(enable & OPCONN_BIT(0)))
    {
        *iTxFault = 1;
    }
    else
    {
        *iTxFault = 0;
    }

    return NO_ERROR ;

}


