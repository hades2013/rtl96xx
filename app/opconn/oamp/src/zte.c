/*=============================================================================
FILE - zte.c

DESCRIPTION
	Implements the zte private oam.

MODIFICATION DETAILS
=============================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "log.h"
#if 0 /*wfxu 0402 */
#include "dal_port.h"
#include "dal_vtt.h"
#include "dal_dba.h"
#include "dal_pon.h"
#endif
#include "odm_port.h"
#include "odm_pon.h"
#include "odm_fdb.h"
#include "system.h"
#include "qos.h"
#include "stats.h"
#include "zte.h"
#include "opconn_usr_ioctrl.h"

#ifdef OAM_SUPPORT_ZTE_EXT

#define MODULE MOD_OAM

#define UPSTREAM_BUFFER_DEFAULT 16383
#define DOWNSTREAM_BUFFER_DEFAULT 16383

#define UPSTREAM_BUFFER_MAX 16383
#define DOWNSTREAM_BUFFER_MAX 16383
#define STREAM_BUFFER_MAX 16383

#define CFG_HG_SECTION 	    "General"
#define CFG_HG_IN_NET_STATE	"In_net_state"
#define CFG_HG_IN_NET_STATE_DEFAULT 0
//#define CFG_HG_IN_NET_MAC		"In_net_mac"
#define CFG_HG_IN_NET_PORT	"In_net_port"
#define CFG_HG_IN_NET_PORT_DEFAULT 0
//#define CFG_HG_IN_NET_VLAN	"In_net_vlan"

typedef enum{
	ZTE_PORT_MODE_AUTO      = 0x00,
	ZTE_PORT_MODE_10M_HALF  = 0x01,
	ZTE_PORT_MODE_10M_FULL  = 0x02,
	ZTE_PORT_MODE_100M_HALF = 0x03,
	ZTE_PORT_MODE_100M_FULL = 0x04,
	ZTE_PORT_MODE_1000M     = 0x05,
	ZTE_PORT_MODE_10G       = 0x06,
}ZTE_PORT_MODE_t;

typedef struct {
	u8_t statistics_action;
	UINT32 statistics_interva;
	UINT32 statistics_duration_time;
}statistics_port_t;

statistics_port_t statistics_port[ODM_NUM_OF_PORTS]=
	{
		{0,1000,900},
		{0,1000,900},
		{0,1000,900},
		{0,1000,900},
        {0,1000,900},
	};
typedef struct {
	UINT64 staifInOctets;
	UINT64 staifInUcastPkts;
	UINT64 staifInNUcastPkts;
	UINT64 staifInDiscards;
	UINT64 staifInErrors;	
	UINT64 staifOutOctets;
	UINT64 staifOutUcastPkts;
	UINT64 staifOutNUcastPkts;
	UINT64 staifOutDiscards;
	UINT64 staifOutErrors;	
}statistics_counter_t;

statistics_counter_t statistics_counter1[ODM_NUM_OF_PORTS];
statistics_counter_t statistics_counter2[ODM_NUM_OF_PORTS];

int statistics_port_num[ODM_NUM_OF_PORTS] = {0};
VOS_THREAD_t portStatisticsThreadId[ODM_NUM_OF_PORTS] = {0};
	
/* OAM_ZTE_EXT_LEAF_PORT_MODE */
static oam_zte_port_mode_t stub_zte_port_mode[ODM_NUM_OF_PORTS+1];

/* OAM_ZTE_EXT_LEAF_UNI_ISOLATION */
static oam_zte_uni_isolation_t stub_zte_uni_isolation;

/* OAM_ZTE_EXT_LEAF_PMAC_BUFF_CFG */

static oam_zte_pmac_buff_cfg_t stub_zte_pmac_buff_cfg[2];

/* OAM_ZTE_EXT_LEAF_PMAC_DS_SHAPING */
static oam_zte_pmac_ds_shaping_t stub_zte_pmac_ds_shaping;

/* OAM_ZTE_EXT_LEAF_HG_ACCESS_INFO 0x6003*/
static oam_zte_hg_access_info_t stub_zte_hg_access_info[ODM_NUM_OF_PORTS+1];

/* OAM_ZTE_EXT_LEAF_ONU_ETHER_STATS */
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
extern OPL_CNT_t g_astCntSwhPort[ODM_NUM_OF_PORTS][CNT_SWH_PORT_NUM];
#else
extern OPL_CNT_t g_astCntGe[DAL_CNT_GE_NUM];
#endif

/* OAM_ZTE_EXT_LEAF_TX_LASER_CONTROL */

#define NEED_MATCH 0x01
#define NO_NEED_MATCH 0x00
enum
{
	OPEN_TX_ACTION,
	CLOSE_TX_ACTION,
	CLOSE_MODULE_ACTION,
};

extern oam_instance_tlv_t_2_0	port_instance_2_0;

static char gLaserConfigFlag = 0;
static UINT32 gTxLaserCloseDuration = 0;

extern OPL_CNT_t g_astCntMpcp[DAL_CNT_MPCP_NUM];
extern OPL_CNT_t g_astCntPon[DAL_CNT_PON_NUM];
extern u16_t g_usOutDataIdx;
extern u16_t g_usAlarmOutDataIdx;
extern oam_sate_t oams;
extern u8_t  alarm_out_data[OAM_MAXIMUM_PDU_SIZE+1];
extern u8_t	out_data[OAM_MAXIMUM_PDU_SIZE+4];
extern u8_t	in_data[OAM_MAXIMUM_PDU_SIZE+4];
extern u8_t zte_oui[3];
/*define for hg*/
#define MAC_ASC_F       "%02X%02X%02X%02X%02X%02X"
#define MACPRINT(mac)  mac[5],mac[4],mac[3],mac[2],mac[1],mac[0]
#define CFG_HG_CFG            "/cfg/hg.conf"

#define EOPL_ZTE_HEADER_PROCESS(code,type,port) \
	do { \
		if(0 == g_usOutDataIdx){ \
			p_out = &out_data[0]; \
			eopl_oam_pdu_hdr(p_out, oams.flags, OAM_CODE_ORG_SPEC); \
			p_out += sizeof(oampdu_hdr_t); \
			vosMemCpy(((zte_header_t*)p_out)->oui, zte_oui, 3); \
			((zte_header_t*)p_out)->ext_opcode = code; \
			((zte_header_t*)p_out)->type = type; \
			((zte_header_t*)p_out)->port = port; \
			p_out += sizeof(zte_header_t); \
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(zte_header_t); \
		} \
		else { \
			p_out = &out_data[g_usOutDataIdx]; \
		} \
	} while(0); 

void stripzerostring(char * src, char * dst,int len)
{
	int pos=0;
	int i=0;
	while(src[pos] == 0 && pos< len)
	{
		pos ++ ;	
	}
	for(i=0;i<(len-pos);i++)
	{
		dst[i]=src[i+pos];
	}
}

/* OAM_ZTE_EXT_LEAF_ONU_SN */
u32_t stub_zte_get_onu_sn(u8_t *p_onu_sn)
{
	return odmPonSnGet(p_onu_sn);
}

void eopl_zte_get_onu_support(u8_t type,u8_t port)
{
	u8_t	        *p_out;
	oam_zte_onu_souport_t	*p_onu_souport;
	oam_rsp_indication_t *p_get_rsp;

	EOPL_ZTE_HEADER_PROCESS(OAM_EXT_CODE_GET_RESPONSE,type,port);
	
	p_onu_souport = (oam_zte_onu_souport_t*)p_out;
	p_onu_souport->branch = OAM_ZTE_EXT_BRANCH_ATTR;
	p_onu_souport->leaf = OAM_ZTE_EXT_LEAF_ONU_SUPPORT;
	p_onu_souport->is_souport = 0x01;

	p_onu_souport->width = sizeof(oam_zte_onu_souport_t)-4;
	g_usOutDataIdx += sizeof(oam_zte_onu_souport_t);
}

void eopl_zte_get_onu_sn(u8_t type,u8_t port)
{
	u8_t	        *p_out;
	oam_zte_onu_sn_t	*p_onu_sn;
	oam_rsp_indication_t *p_get_rsp;
	u8_t            oamRspCode;
	u32_t           ret;

	EOPL_ZTE_HEADER_PROCESS(OAM_EXT_CODE_GET_RESPONSE,type,port);
	
	p_onu_sn = (oam_zte_onu_sn_t*)p_out;
	p_onu_sn->branch = OAM_ZTE_EXT_BRANCH_ATTR;
	p_onu_sn->leaf = OAM_ZTE_EXT_LEAF_ONU_SN;

	ret = stub_zte_get_onu_sn(p_onu_sn->sn);
	if(ret!= NO_ERROR)
	{
		p_onu_sn->width = OAM_BAD_PARAM_VALUE;
		g_usOutDataIdx += 4;
	}
	else
	{
		p_onu_sn->width = sizeof(oam_zte_onu_sn_t)-4;
		g_usOutDataIdx += sizeof(oam_zte_onu_sn_t);
	}
}

typedef enum{
	ZTE_CONFIG_MANAGE_INVALID  = 0x00,
	ZTE_CONFIG_MANAGE_SAVE  = 0x01,
	ZTE_CONFIG_MANAGE_ERASE = 0x02,
	ZTE_CONFIG_MANAGE_RESTOR_FACTORY = 0x03,
}ZTE_CONFIG_MANAGE_TYPE_t;

static ZTE_CONFIG_MANAGE_TYPE_t gConfigManageType;

void * AfterConfigParamThread(void *pszParm)
{
	int ret = 0;
	vosSleep(1);
	if(gConfigManageType == ZTE_CONFIG_MANAGE_RESTOR_FACTORY){
		vosConfigErase();
		vosSleep(1);
 		/*wait the response to the olt*/
		ret = odmSysReset();
		gConfigManageType = ZTE_CONFIG_MANAGE_INVALID;
 	}
	if(gConfigManageType == ZTE_CONFIG_MANAGE_SAVE){
		if (vosConfigSave(NULL) < 0)
		{
			//return -1;
		}
		gConfigManageType = ZTE_CONFIG_MANAGE_INVALID;
	}

    vosThreadExit(0);
	return NULL;
}
void 	eopl_zte_set_config_param(u8_t *p_in,u8_t type,u8_t port)
{
	u32_t   ret = OK;
	u8_t    *p_out;

	oam_set_response_t *p_rsp;
	oam_zte_onu_config_param_t    *p_zte_config_param;

	EOPL_ZTE_HEADER_PROCESS(OAM_EXT_CODE_SET_RESPONSE,type,port);

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_ZTE_EXT_BRANCH_ATTR;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_CONFIG_PARAM;
	
    p_zte_config_param = (oam_zte_onu_config_param_t *)p_in;

	gConfigManageType = p_zte_config_param->action;

	if(gConfigManageType < 1 || gConfigManageType > 3){
		p_rsp->indication = OAM_NO_PARAM;
		g_usOutDataIdx += 4;
		return;
	}
	if (NULL == vosThreadCreate("tRstFact", OP_VOS_THREAD_STKSZ, 100, 
        AfterConfigParamThread, NULL))
	{
		OP_DEBUG(DEBUG_LEVEL_WARNING, "Create AfterConfigResetThread thread failed\n");	
        ret = ERROR;
	}
	
	if(!ret)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_BAD_PARAM_VALUE;
	}

	g_usOutDataIdx += 4;
	
	return;
}



/* OAM_ZTE_EXT_LEAF_TYPE_C_GET */
static u8_t stub_zte_type_c_k1;
static u8_t stub_zte_type_c_k2;

u32_t stub_zte_set_type_c(u8_t k1, u8_t k2)
{
	stub_zte_type_c_k1 = k1;
	stub_zte_type_c_k2 = k2;

	return NO_ERROR;
}

u32_t stub_zte_get_type_c(u8_t *p_k1, u8_t *p_k2)
{
	*p_k1 = stub_zte_type_c_k1;
	*p_k2 = stub_zte_type_c_k2;

	return NO_ERROR;
}

void eopl_zte_set_type_c(u8_t *p_in,u8_t type,u8_t port)
{
	u32_t              ret;
	u8_t               *p_out;
	oam_set_response_t *p_rsp;
	oam_zte_type_c_t    *p_zte_type_c;

	EOPL_ZTE_HEADER_PROCESS(OAM_EXT_CODE_SET_RESPONSE,type,port);

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_ZTE_EXT_BRANCH_ATTR;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_TYPE_C_SET;
	
    p_zte_type_c = (oam_zte_type_c_t *)p_in;
	
	ret = stub_zte_set_type_c(p_zte_type_c->k1, p_zte_type_c->k2);
	if(!ret) 
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_BAD_PARAM_VALUE;
	}

	g_usOutDataIdx += 4;
	return;
}

void eopl_zte_get_type_c(u8_t type,u8_t port)
{
	u8_t	        *p_out;
	oam_zte_type_c_t	*p_type_c;
	u32_t           ret;

	EOPL_ZTE_HEADER_PROCESS(OAM_EXT_CODE_GET_RESPONSE,type,port);

	p_type_c  = (oam_zte_type_c_t *)p_out;
	p_type_c->branch = OAM_ZTE_EXT_BRANCH_ATTR;
	p_type_c->leaf = OAM_ZTE_EXT_LEAF_TYPE_C_GET;
	p_type_c->width = 2;

	ret = stub_zte_get_type_c(&p_type_c->k1, &p_type_c->k2);

	g_usOutDataIdx += p_type_c->width + 4;
}

/* OAM_ZTE_EXT_LEAF_ALARM_INFO */
int eopl_zte_alarm_report(u8_t port_id, u16_t alarm_id)
{
	u8_t	            *p_out;
	oam_instance_tlv_t_2_0   *p_inst;
	oam_zte_alarm_info_t *p_zte_alarm_info;
	u32_t                ret;

	if(0 == g_usAlarmOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &alarm_out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usAlarmOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &alarm_out_data[g_usAlarmOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t_2_0 *)p_out;
	p_out += sizeof(oam_instance_tlv_t_2_0);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

	p_inst->branch = 0x36;
	p_inst->leaf = 0x0001;
	p_inst->width = 0x01;
	p_inst->value = port_id;

	p_zte_alarm_info = (oam_zte_alarm_info_t*)p_out;
	p_zte_alarm_info->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_zte_alarm_info->leaf = OAM_ZTE_EXT_LEAF_ALARM_INFO;
	p_zte_alarm_info->width = 5;
	p_zte_alarm_info->alarm_id = alarm_id;

	g_usAlarmOutDataIdx += p_zte_alarm_info->width + 4;

	return ret;
}

/* OAM_ZTE_EXT_LEAF_ALARM_CFG */
static u8_t stub_zte_alarm_cfg;

u32_t stub_zte_get_alarm_cfg(u8_t *p_alarm_cfg)
{
	*p_alarm_cfg = stub_zte_alarm_cfg;

	return NO_ERROR;
}

u32_t stub_zte_set_alarm_cfg(u8_t alarm_cfg)
{
	if(alarm_cfg != stub_zte_alarm_cfg)
	{
		if(alarm_cfg == 1)
		{
			odmAlarmThreadInit();
		}
		else
		{
			odmAlarmThreadShutdown();			
		}	
	}
	
	stub_zte_alarm_cfg = alarm_cfg;
		
	return NO_ERROR;
}

void eopl_zte_get_alarm_cfg()//onu based
{
	u8_t	           *p_out;
	oam_zte_alarm_cfg_t *p_zte_alarm_cfg;
	oam_rsp_indication_t *p_get_rsp;

	u8_t              oamRspCode;
	u8_t                alarm_config;
	u32_t               ret;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	ret = stub_zte_get_alarm_cfg(&alarm_config);

	if(ret != NO_ERROR)
	{
		oamRspCode = OAM_GET_NO_RESOURCE;
		p_get_rsp = (oam_rsp_indication_t *)p_out;
		p_get_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_get_rsp->leaf = OAM_ZTE_EXT_LEAF_ALARM_CFG;
		p_get_rsp->var_indication = oamRspCode;

		g_usOutDataIdx += 4;
	}
	else
	{
		p_zte_alarm_cfg = (oam_zte_alarm_cfg_t*)p_out;
		p_zte_alarm_cfg->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_zte_alarm_cfg->leaf = OAM_ZTE_EXT_LEAF_ALARM_CFG;
		p_zte_alarm_cfg->width = 1;

		p_zte_alarm_cfg->alarm_config = alarm_config;

		g_usOutDataIdx += p_zte_alarm_cfg->width + 4;
	}
}

void eopl_zte_set_alarm_cfg(u8_t *p_in)//onu based
{
	u8_t ret, *p_out;
	oam_set_response_t	*p_rsp;
	oam_zte_alarm_cfg_t  *p_zte_alarm_cfg;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_ALARM_CFG;

	p_zte_alarm_cfg = (oam_zte_alarm_cfg_t *)p_in;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "set alarm config = %d\n", p_zte_alarm_cfg->alarm_config);

	ret = stub_zte_set_alarm_cfg(p_zte_alarm_cfg->alarm_config);
	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += 4;

	return;
}

/* OAM_ZTE_EXT_LEAF_UPGRADE_PARA */
static oam_zte_upgrade_para_t stub_zte_upgrade_para;

u32_t stub_zte_get_upgrade_para(oam_zte_upgrade_para_t *p_upgrade_para)
{

	p_upgrade_para->ip_addr = stub_zte_upgrade_para.ip_addr;
	p_upgrade_para->ip_mask = stub_zte_upgrade_para.ip_mask;
	p_upgrade_para->gateway = stub_zte_upgrade_para.gateway;
	p_upgrade_para->vlanid = stub_zte_upgrade_para.vlanid;
	vosMemCpy(&p_upgrade_para->username, stub_zte_upgrade_para.username, 8);
	vosMemCpy(&p_upgrade_para->password, stub_zte_upgrade_para.password, 8);
	p_upgrade_para->action = stub_zte_upgrade_para.action;
	p_upgrade_para->ver_num = stub_zte_upgrade_para.ver_num;
	vosMemCpy(&p_upgrade_para->ver_name1, stub_zte_upgrade_para.ver_name1, 16);
	vosMemCpy(&p_upgrade_para->ver_name2, stub_zte_upgrade_para.ver_name2, 16);
	vosMemCpy(&p_upgrade_para->ver_name3, stub_zte_upgrade_para.ver_name3, 16);

	return NO_ERROR;
}

u32_t stub_zte_set_upgrade_para(oam_zte_upgrade_para_t *p_upgrade_para)
{
	stub_zte_upgrade_para.ip_addr = p_upgrade_para->ip_addr;
	stub_zte_upgrade_para.ip_mask = p_upgrade_para->ip_mask;
	stub_zte_upgrade_para.gateway = p_upgrade_para->gateway;
	stub_zte_upgrade_para.vlanid = p_upgrade_para->vlanid;
	vosMemCpy(&stub_zte_upgrade_para.username, &p_upgrade_para->username, 8);
	vosMemCpy(&stub_zte_upgrade_para.password, &p_upgrade_para->password, 8);
	stub_zte_upgrade_para.action = p_upgrade_para->action;
	stub_zte_upgrade_para.ver_num = p_upgrade_para->ver_num;
	vosMemCpy(&stub_zte_upgrade_para.ver_name1, &p_upgrade_para->ver_name1, 16);
	vosMemCpy(&stub_zte_upgrade_para.ver_name2, &p_upgrade_para->ver_name2, 16);
	vosMemCpy(&stub_zte_upgrade_para.ver_name3, &p_upgrade_para->ver_name3, 16);

	return NO_ERROR;
}

void eopl_zte_get_upgrade_para()
{
	u8_t	              *p_out;
	oam_zte_upgrade_para_t *p_upgrade_para;
	oam_zte_upgrade_para_t onu_upgrade_para;
	oam_rsp_indication_t	*p_get_rsp;
	u32_t                 ret;
    u8_t                  oamRspCode;
	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	ret = stub_zte_get_upgrade_para(&onu_upgrade_para);

	if(ret != NO_ERROR)
	{
		oamRspCode = OAM_GET_NO_RESOURCE;
		p_get_rsp = (oam_rsp_indication_t *)p_out;
		p_get_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_get_rsp->leaf = OAM_ZTE_EXT_LEAF_UPGRADE_PARA;
		p_get_rsp->var_indication = oamRspCode;

		g_usOutDataIdx += 4;
	}

	else
	{
		p_upgrade_para = (oam_zte_upgrade_para_t*)p_out;
		p_upgrade_para->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_upgrade_para->leaf = OAM_ZTE_EXT_LEAF_UPGRADE_PARA;
		p_upgrade_para->width = 80;

		p_upgrade_para->ip_addr = onu_upgrade_para.ip_addr;
		p_upgrade_para->ip_mask = onu_upgrade_para.ip_mask;
		p_upgrade_para->gateway = onu_upgrade_para.gateway;
		p_upgrade_para->vlanid = onu_upgrade_para.vlanid;
		vosMemCpy(&p_upgrade_para->username, onu_upgrade_para.username, 8);
		vosMemCpy(&p_upgrade_para->password, onu_upgrade_para.password, 8);
		p_upgrade_para->action = onu_upgrade_para.action;
		p_upgrade_para->ver_num = onu_upgrade_para.ver_num;
		vosMemCpy(&p_upgrade_para->ver_name1, onu_upgrade_para.ver_name1, 16);
		vosMemCpy(&p_upgrade_para->ver_name2, onu_upgrade_para.ver_name2, 16);
		vosMemCpy(&p_upgrade_para->ver_name3, onu_upgrade_para.ver_name3, 16);
		p_upgrade_para->width = OAM_SET_BAD_PARAMETERS;

		//g_usOutDataIdx += p_upgrade_para->width + 4;
		g_usOutDataIdx += 1 + 4;


	}


	return;
}

void eopl_zte_set_upgrade_para(u8_t *p_in)
{
	u8_t ret, *p_out;
	oam_set_response_t	  *p_rsp;
	oam_zte_upgrade_para_t *p_zte_upgrade_para;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_UPGRADE_PARA;

	p_zte_upgrade_para = (oam_zte_upgrade_para_t *)p_in;


	ret = stub_zte_set_upgrade_para(p_zte_upgrade_para);
	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += 4;

	return;

}

/* OAM_ZTE_EXT_LEAF_MAC_ADDR_LIMIT */
u32_t stub_zte_get_mac_addr_limit(u8_t port_id, u16_t *p_limit_num)
{
	u32_t ret;
	u32_t mac_limit_num;
	u32_t mac_limit_enable;

	ret = odmPortMacLimitNumEnableGet(port_id, &mac_limit_enable, &mac_limit_num);

	if(ret != NO_ERROR)
		return ret;

	*p_limit_num = mac_limit_num;

	return NO_ERROR;
}

u32_t stub_zte_set_mac_addr_limit(u8_t port_id, u16_t limit_num)
{
	u32_t ret;

	if(limit_num != 0xFFFF)
	{
		ret	= odmPortMacLimitNumEnableSet(port_id,TRUE, limit_num);
	}
	else
	{
		ret	= odmPortMacLimitNumEnableSet(port_id,FALSE, limit_num);
	}

	return ret;
}

void eopl_zte_get_mac_addr_limit()//port based
{
	u8_t	ret;
	u8_t    ucPortS;
	u8_t    ucPortE;
	u8_t    *p_out;
	u8_t    oamRspCode;
    oam_rsp_indication_t *p_get_rsp;
	oam_instance_tlv_t_2_0	*p_inst;
	oam_zte_mac_limit_t  *p_zte_mac_limit;
	u16_t   enable;
	u16_t   limit_num;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_out;
		*p_inst = port_instance_2_0;
		p_out += sizeof(oam_instance_tlv_t_2_0);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);
		
		ret = stub_zte_get_mac_addr_limit(ucPortS, &limit_num);

		if(ret != NO_ERROR)
		{
			oamRspCode = OAM_GET_NO_RESOURCE;
			p_get_rsp = (oam_rsp_indication_t *)p_out;
			p_get_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_get_rsp->leaf = OAM_ZTE_EXT_LEAF_UPGRADE_PARA;
			p_get_rsp->var_indication = oamRspCode;

			g_usOutDataIdx += 4;
		}
        else
        {
			p_zte_mac_limit = (oam_zte_mac_limit_t*)p_out;
			p_zte_mac_limit->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_zte_mac_limit->leaf = OAM_ZTE_EXT_LEAF_MAC_ADDR_LIMIT;
			p_zte_mac_limit->width = 2;
			p_zte_mac_limit->limit_num = limit_num;

			g_usOutDataIdx += p_zte_mac_limit->width + 4;
		}
	}

	return;
}

void eopl_zte_set_mac_addr_limit(u8_t *p_in)//port based
{
	u8_t ret, ucPortS, ucPortE, *p_out;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t_2_0	*p_inst;
	oam_zte_mac_limit_t  *p_zte_mac_limit;

	ret=OPL_OK;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t_2_0 *)p_out;
	*p_inst =  port_instance_2_0;
	p_out += sizeof(oam_instance_tlv_t_2_0);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_MAC_ADDR_LIMIT;

	p_zte_mac_limit = (oam_zte_mac_limit_t *)p_in;
	for(; ucPortS<=ucPortE; ucPortS++)
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "port %d: set mac limit num = %d\n", ucPortS, p_zte_mac_limit->limit_num);
		ret = stub_zte_set_mac_addr_limit(ucPortS, p_zte_mac_limit->limit_num);
		if(ret != NO_ERROR)
			goto send_rsp;
	}
send_rsp:
	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else if (ret == ERR_INVALID_PARAMETERS){
		p_rsp->indication = OAM_SET_BAD_PARAMETERS;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	p_out += sizeof(oam_set_response_t);
	g_usOutDataIdx += sizeof(oam_set_response_t);
}

/* OAM_ZTE_EXT_LEAF_MAC_ADDR_AGING */
u32_t stub_zte_get_mac_addr_aging_time(u32_t *p_aging_time)
{
	u32_t ret;

	ret = odmFdbAgingTimeGet(p_aging_time);

	return ret;
}

u32_t stub_zte_set_mac_addr_aging_time(u32_t aging_time)
{
	u32_t ret;

	ret = odmFdbAgingTimeSet(aging_time);

	return ret;
}

void eopl_zte_get_mac_addr_aging()//onu based
{
	u8_t	           *p_out;
	u8_t               oamRspCode;
	oam_rsp_indication_t *p_get_rsp;
	oam_zte_mac_aging_t *p_zte_mac_aging;
	u32_t              ret;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_zte_mac_aging = (oam_zte_mac_aging_t *)p_out;
	p_zte_mac_aging->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_zte_mac_aging->leaf = OAM_ZTE_EXT_LEAF_MAC_ADDR_AGING;

	ret = stub_zte_get_mac_addr_aging_time(&p_zte_mac_aging->aging_time);

	if(ret != NO_ERROR)
	{
		p_zte_mac_aging->width = OAM_GET_NO_RESOURCE;
		g_usOutDataIdx += 4;
	}
    else
    {
		p_zte_mac_aging->width = sizeof(oam_zte_mac_aging_t);
		g_usOutDataIdx += p_zte_mac_aging->width + 4;
    }

}

void eopl_zte_set_mac_addr_aging(u8_t *p_in)//onu based
{
	u8_t ret, *p_out;
	oam_set_response_t	*p_rsp;
	oam_zte_mac_aging_t  *p_zte_mac_aging;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
		g_usOutDataIdx = sizeof(struct oampdu_hdr) + sizeof(struct  ctc_header);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}
	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_MAC_ADDR_AGING;

	p_zte_mac_aging = (oam_zte_mac_aging_t *)p_in;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "set mac addr aging time = %d\n", p_zte_mac_aging->aging_time);

	ret = stub_zte_set_mac_addr_aging_time(p_zte_mac_aging->aging_time);
	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += 4;

	return;
}

/* OAM_ZTE_EXT_LEAF_PORT_MAC_FILTER */

u32_t stub_zte_port_mac_filter_entry_add(u8_t port_id, u8_t entry_id, oam_zte_port_mac_filter_entry_t *p_entry)
{
	return NO_ERROR;
}

u32_t stub_zte_port_mac_filter_entry_del(u8_t port_id, u8_t entry_id)
{

	return NO_ERROR;
}

u32_t stub_zte_port_mac_filter_entry_get(u8_t port_id, u8_t entry_id, oam_zte_port_mac_filter_entry_t *p_entry)
{
	u16_t vlan_id;
	u8_t  mac_addr_str[32];
	u32_t ret;

	ret = odmPortMacFilterEntryGet(port_id, entry_id, &vlan_id, mac_addr_str);

	if(ret != NO_ERROR)
		return ret;

	p_entry->vlan_id = vlan_id;

	vosStrToMac(mac_addr_str, p_entry->mac_addr);

	return NO_ERROR;
}

u32_t stub_zte_port_mac_filter_add(u8_t port_id, oam_zte_port_mac_filter_entry_t *p_entry)
{
	u8_t  num_of_entries;
	u32_t ret;
	u16_t vlan_id;
	u8_t  mac_addr_str[32];

	odmPortMacFilterNumGet(port_id, &num_of_entries);

	if(num_of_entries == ODM_MAX_NUM_OF_MAC_FILTER)
	{
		return -1;
	}

	vlan_id = p_entry->vlan_id;

	vosMacToStr(p_entry->mac_addr, mac_addr_str);

	ret = odmPortMacFilterEntryAdd(port_id, vlan_id, mac_addr_str);

	return ret;
}

u32_t stub_zte_port_mac_filter_del(u8_t port_id, oam_zte_port_mac_filter_entry_t *p_entry)
{
	u32_t ret;
	u8_t  num_of_entry;
	u16_t vlan_id;
	u8_t  mac_addr_str[32];

	ret = odmPortMacFilterNumGet(port_id, &num_of_entry);
	if(ret != NO_ERROR)
	{
		return ret;
	}

	if(num_of_entry == 0)
	{
		return -1;
	}

	vlan_id = p_entry->vlan_id;

	vosMacToStr(p_entry->mac_addr, mac_addr_str);

	ret = odmPortMacFilterEntryDel(port_id, vlan_id, mac_addr_str);

	return ret;
}

u32_t stub_zte_port_mac_filter_clear(u8_t port_id)
{
	u32_t ret;

	ret = odmPortMacFilterEntryClear(port_id);

	return ret;
}

u32_t stub_zte_port_mac_filter_num_get(u8_t port_id, u8_t *p_entry_num)
{
	u32_t ret;
	u8_t  entry_num;

	ret = odmPortMacFilterNumGet(port_id, &entry_num);

	if(ret != NO_ERROR)
		return ret;

	*p_entry_num = entry_num;

	return NO_ERROR;
}


void eopl_zte_get_port_mac_filter(void)
{
	u8_t	ret;
	u8_t    ucPortS;
	u8_t    ucPortE;
	u8_t    *p_out;
	u8_t    oamRspCode;
	oam_instance_tlv_t_2_0	           *p_inst;
	oam_zte_port_mac_filter_t       *p_zte_port_mac_filter;
	oam_rsp_indication_t * p_get_rsp;
	u8_t    num_of_entries;
	oam_zte_port_mac_filter_entry_t *p_zte_mac_filter_entry;
	u8_t    i;

	ret = NO_ERROR;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_out;
		*p_inst = port_instance_2_0;
		p_out += sizeof(oam_instance_tlv_t_2_0);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

		ret = stub_zte_port_mac_filter_num_get(ucPortS, &num_of_entries);

		if(ret != NO_ERROR)
		{
			oamRspCode = OAM_GET_NO_RESOURCE;
			p_get_rsp = (oam_rsp_indication_t *)p_out;
			p_get_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_get_rsp->leaf = OAM_ZTE_EXT_LEAF_PORT_MAC_FILTER;
			p_get_rsp->var_indication = oamRspCode;

			g_usOutDataIdx += 4;
			return ;
		}

		p_zte_port_mac_filter = (oam_zte_port_mac_filter_t *)p_out;
		p_out += sizeof(oam_zte_port_mac_filter_entry_t);

		p_zte_port_mac_filter->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_zte_port_mac_filter->leaf = OAM_ZTE_EXT_LEAF_PORT_MAC_FILTER;
		p_zte_port_mac_filter->action = 0x03;


		p_zte_port_mac_filter->num_of_entries = num_of_entries;
		p_zte_port_mac_filter->width = 2 + 8 * num_of_entries;


		for(i = 0; i < num_of_entries; i++)
		{

			p_zte_mac_filter_entry = (oam_zte_port_mac_filter_entry_t *)p_out;

			ret = stub_zte_port_mac_filter_entry_get(ucPortS, i, p_zte_mac_filter_entry);

			if(ret != NO_ERROR)
			{
				p_zte_port_mac_filter->width = OAM_GET_NO_RESOURCE;
				g_usOutDataIdx += 4;
				return ;
			}
			else
			{
				p_out += sizeof(oam_zte_port_mac_filter_entry_t);

			}
		}

		g_usOutDataIdx += p_zte_port_mac_filter->width + 4;


	}

	return;
}

void eopl_zte_set_port_mac_filter(u8_t *p_in)
{
	u8_t ucPortS, ucPortE, curPortId;
	u8_t                           *p_out;
	oam_set_response_t	           *p_rsp;
	oam_instance_tlv_t_2_0	           *p_inst;
	oam_zte_port_mac_filter_t       *p_zte_port_mac_filter;
	oam_zte_port_mac_filter_entry_t *p_zte_mac_filter_entry;
	u8_t                           num_of_entries;
	u8_t                           i;
	u32_t                          ret;

	ret=OPL_OK;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t_2_0 *)p_out;
	*p_inst =  port_instance_2_0;
	p_out += sizeof(oam_instance_tlv_t_2_0);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_PORT_MAC_FILTER;

	p_zte_port_mac_filter = (oam_zte_port_mac_filter_t *)p_in;
	p_in += sizeof(oam_zte_port_mac_filter_t);

	num_of_entries = p_zte_port_mac_filter->num_of_entries;

	p_zte_mac_filter_entry = (oam_zte_port_mac_filter_entry_t *)p_in;

	if(p_zte_port_mac_filter->action == 0x00)//delete
	{
		for(i = 0; i < num_of_entries; i++)
		{
			for(curPortId=ucPortS; curPortId<=ucPortE; curPortId++)
			{
				ret = stub_zte_port_mac_filter_del(curPortId, p_zte_mac_filter_entry);
				if(ret != NO_ERROR)
					goto send_rsp;
			}

			p_zte_mac_filter_entry++;
		}
	}
	else if(p_zte_port_mac_filter->action == 0x01)//add
	{
		for(i = 0; i < num_of_entries; i++)
		{
			for(curPortId=ucPortS; curPortId<=ucPortE; curPortId++)
			{
				ret = stub_zte_port_mac_filter_add(curPortId, p_zte_mac_filter_entry);
				if(ret != NO_ERROR)
					goto send_rsp;
			}

			p_zte_mac_filter_entry++;
		}
	}
	else if(p_zte_port_mac_filter->action == 0x02)//clear
	{
		for(; ucPortS<=ucPortE; ucPortS++)
		{
			ret = stub_zte_port_mac_filter_clear(ucPortS);
			if(ret != NO_ERROR)
				goto send_rsp;
		}
	}

send_rsp:
	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	p_out += sizeof(oam_set_response_t);
	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

/* OAM_ZTE_EXT_LEAF_PORT_MAC_BIND */
u32_t stub_zte_port_mac_binding_entry_add(u8_t port_id, u8_t entry_id, oam_zte_port_mac_binding_entry_t *p_entry)
{

	return NO_ERROR;
}

u32_t stub_zte_port_mac_binding_entry_del(u8_t port_id, u8_t entry_id)
{

	return NO_ERROR;
}

u32_t stub_zte_port_mac_binding_entry_get(u8_t port_id, u8_t entry_id, oam_zte_port_mac_binding_entry_t *p_entry)
{
	u16_t vlan_id;
	u8_t  mac_addr_str[32];
	u32_t ret;

	ret = odmPortBindMacEntryGet(port_id, entry_id, &vlan_id, mac_addr_str);

	if(ret != NO_ERROR)
		return ret;

	p_entry->vlan_id = vlan_id;

	vosStrToMac(mac_addr_str, p_entry->mac_addr);

	return NO_ERROR;

}

u32_t stub_zte_port_mac_binding_add(u8_t port_id, oam_zte_port_mac_binding_entry_t *p_entry)
{
	u8_t  num_of_entries;
	u16_t vlan_id;
	u8_t  mac_addr_str[32];
	u32_t ret;

	ret = odmPortBindMacNumGet(port_id, &num_of_entries);
	if(ret != NO_ERROR)
		return ret;

	if(num_of_entries == ODM_MAX_NUM_OF_BIND_MAC)
	{
		return -1;
	}

	vlan_id = p_entry->vlan_id;

	vosMacToStr(p_entry->mac_addr, mac_addr_str);

	ret = odmPortBindMacEntryAdd(port_id, vlan_id, mac_addr_str);

	return ret;
}

u32_t stub_zte_port_mac_binding_del(u8_t port_id, oam_zte_port_mac_binding_entry_t *p_entry)
{
	u32_t ret;
	u8_t  num_of_entry;
	u16_t vlan_id;
	u8_t  mac_addr_str[32];

	ret = odmPortBindMacNumGet(port_id, &num_of_entry);
	if(ret != NO_ERROR)
	{
		return ret;
	}

	if(num_of_entry == 0)
	{
		return -1;
	}

	vlan_id = p_entry->vlan_id;

	vosMacToStr(p_entry->mac_addr, mac_addr_str);

	ret = odmPortBindMacEntryDel(port_id, vlan_id, mac_addr_str);

	return ret;
}

u32_t stub_zte_port_mac_binding_clear(u8_t port_id)
{
	u32_t ret;

	ret = odmPortBindMacEntryClear(port_id);

	return ret;
}

u32_t stub_zte_port_mac_binding_num_get(u8_t port_id, u8_t *p_entry_num)
{
	u32_t ret;
	u8_t  entry_num;

	ret = odmPortBindMacNumGet(port_id, &entry_num);

	if(ret != NO_ERROR)
		return ret;

	*p_entry_num = entry_num;

	return NO_ERROR;

}


void eopl_zte_get_port_mac_binding(void)
{
	u8_t                            ucPortS;
	u8_t                            ucPortE;
	u8_t                            *p_out;
	u8_t                            oamRspCode;
	oam_rsp_indication_t			*p_get_rsp;
	oam_instance_tlv_t_2_0	            *p_inst;
	oam_zte_port_mac_binding_t       *p_zte_port_mac_binding;
	oam_zte_port_mac_binding_entry_t *p_zte_mac_binding_entry;
	u8_t                            i;
	u32_t                           ret;
	u8_t                            num_of_entries;

	ret = NO_ERROR;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_out;
		*p_inst = port_instance_2_0;
		p_out += sizeof(oam_instance_tlv_t_2_0);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

		ret = stub_zte_port_mac_binding_num_get(ucPortS, &num_of_entries);

		if(ret != NO_ERROR)
		{
			oamRspCode = OAM_GET_NO_RESOURCE;
			p_get_rsp = (oam_rsp_indication_t *)p_out;
			p_get_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_get_rsp->leaf = OAM_ZTE_EXT_LEAF_PORT_MAC_BIND;
			p_get_rsp->var_indication = oamRspCode;

			g_usOutDataIdx += 4;
			return ;
		}

		p_zte_port_mac_binding = (oam_zte_port_mac_binding_t*)p_out;
		p_out += sizeof(oam_zte_port_mac_binding_t);

		p_zte_port_mac_binding->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_zte_port_mac_binding->leaf = OAM_ZTE_EXT_LEAF_PORT_MAC_BIND;
		p_zte_port_mac_binding->action = 0x03;

		p_zte_port_mac_binding->num_of_entries = num_of_entries;
		p_zte_port_mac_binding->width = 2 + 8 * num_of_entries;

		for(i = 0; i < num_of_entries; i++)
		{
			p_zte_mac_binding_entry = (oam_zte_port_mac_binding_entry_t *)p_out;

			ret = stub_zte_port_mac_binding_entry_get(ucPortS, i, p_zte_mac_binding_entry);
			if(ret != NO_ERROR)
			{
				p_zte_port_mac_binding->width = OAM_GET_NO_RESOURCE;
				g_usOutDataIdx += 4;
				return ;
			}
			p_out += sizeof(oam_zte_port_mac_binding_entry_t);

		}

		g_usOutDataIdx += p_zte_port_mac_binding->width + 4;
	}

	return;
}

void eopl_zte_set_port_mac_binding(u8_t *p_in)
{
	u8_t ucPortS, ucPortE, curPortId;
	u8_t                            *p_out;
	oam_set_response_t	            *p_rsp;
	oam_instance_tlv_t_2_0	            *p_inst;
	oam_zte_port_mac_binding_t       *p_zte_port_mac_binding;
	oam_zte_port_mac_binding_entry_t *p_zte_mac_binding_entry;
	u8_t                            num_of_entries;
	u8_t                            i;
	u32_t                           ret;

	ret=OPL_OK;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t_2_0 *)p_out;
	*p_inst =  port_instance_2_0;
	p_out += sizeof(oam_instance_tlv_t_2_0);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_PORT_MAC_BIND;

	p_zte_port_mac_binding = (oam_zte_port_mac_binding_t *)p_in;
	p_in += sizeof(oam_zte_port_mac_binding_t);

	num_of_entries = p_zte_port_mac_binding->num_of_entries;

	p_zte_mac_binding_entry = (oam_zte_port_mac_binding_entry_t *)p_in;

	if(p_zte_port_mac_binding->action == 0x00)//delete
	{
		for(i = 0; i < num_of_entries; i++)
		{
			for(curPortId=ucPortS; curPortId<=ucPortE; curPortId++)
			{
				ret = stub_zte_port_mac_binding_del(curPortId, p_zte_mac_binding_entry);
				if(ret != NO_ERROR)
					goto send_rsp;
			}

			p_zte_mac_binding_entry++;
		}
	}
	else if(p_zte_port_mac_binding->action == 0x01)//add
	{
		for(i = 0; i < num_of_entries; i++)
		{
			for(curPortId=ucPortS; curPortId<=ucPortE; curPortId++)
			{
				ret = stub_zte_port_mac_binding_add(curPortId, p_zte_mac_binding_entry);
				if(ret != NO_ERROR)
					goto send_rsp;
			}

			p_zte_mac_binding_entry++;
		}
	}
	else if(p_zte_port_mac_binding->action == 0x02)//clear
	{
		for(; ucPortS<=ucPortE; ucPortS++)
		{
			ret = stub_zte_port_mac_binding_clear(ucPortS);
			if(ret != NO_ERROR)
				goto send_rsp;
		}
	}

send_rsp:
	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	p_out += sizeof(oam_set_response_t);
	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

/* OAM_ZTE_EXT_LEAF_STATIC_MAC_CFG */

u32_t stub_zte_port_static_mac_entry_add(u8_t port_id, u8_t entry_id, oam_zte_static_mac_entry_t *p_entry)
{

	return NO_ERROR;
}

u32_t stub_zte_port_static_mac_entry_del(u8_t port_id, u8_t entry_id)
{

	return NO_ERROR;
}

u32_t stub_zte_port_static_mac_entry_get(u8_t port_id, u8_t entry_id, oam_zte_static_mac_entry_t *p_entry)
{
	u16_t vlan_id;
	u8_t  mac_addr_str[32];
	u32_t ret;

	ret = odmPortStaticMacEntryGet(port_id, entry_id, &vlan_id, mac_addr_str);

	if(ret != NO_ERROR)
		return ret;

	p_entry->vlan_id = vlan_id;

	vosStrToMac(mac_addr_str, p_entry->mac_addr);

	return NO_ERROR;
}

u32_t stub_zte_port_static_mac_num_get(u8_t port_id, u8_t *p_entry_num)
{
	u32_t ret;
	u8_t  entry_num;

	ret = odmPortStaticMacNumGet(port_id, &entry_num);

	if(ret != NO_ERROR)
		return ret;

	*p_entry_num = entry_num;

	return NO_ERROR;

}


u32_t stub_zte_port_static_mac_add(u8_t port_id, oam_zte_static_mac_entry_t *p_entry)
{
	u8_t  num_of_entries;
	u16_t vlan_id;
	u8_t  mac_addr_str[32];
	u32_t ret;

	ret = odmPortStaticMacNumGet(port_id, &num_of_entries);
	if(ret != NO_ERROR)
		return ret;

	if(num_of_entries == ODM_MAX_NUM_OF_STATIC_MAC)
	{
		return -1;
	}

	vlan_id = p_entry->vlan_id;

	vosMacToStr(p_entry->mac_addr, mac_addr_str);

	ret = odmPortStaticMacEntryAdd(port_id, vlan_id, mac_addr_str);

	return ret;
}

u32_t stub_zte_port_static_mac_del(u8_t port_id, oam_zte_static_mac_entry_t *p_entry)
{
	u32_t ret;
	u8_t  num_of_entry;
	u16_t vlan_id;
	u8_t  mac_addr_str[32];

	ret = odmPortStaticMacNumGet(port_id, &num_of_entry);
	if(ret != NO_ERROR)
	{
		return ret;
	}

	if(num_of_entry == 0)
	{
		return -1;
	}

	vlan_id = p_entry->vlan_id;

	vosMacToStr(p_entry->mac_addr, mac_addr_str);

	ret = odmPortStaticMacEntryDel(port_id, vlan_id, mac_addr_str);

	return ret;

}

u32_t stub_zte_port_static_mac_clear(u8_t port_id)
{
	u32_t ret;

	ret = odmPortStaticMacEntryClear(port_id);

	return ret;
}


void eopl_zte_get_static_mac_cfg(void)
{
	u8_t                           ucPortS;
	u8_t                           ucPortE;
	u8_t                           *p_out;
	u8_t                           oamRspCode;
	oam_rsp_indication_t           *p_get_rsp;
	oam_instance_tlv_t_2_0	           *p_inst;
	oam_zte_static_mac_cfg_t        *p_zte_port_static_mac;
	oam_zte_static_mac_entry_t      *p_zte_static_mac_entry;
	u8_t                            i;
	u32_t                           ret;
	u8_t                            num_of_entries;

	ret = NO_ERROR;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_out;
		*p_inst = port_instance_2_0;
		p_out += sizeof(oam_instance_tlv_t_2_0);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

		ret = stub_zte_port_static_mac_num_get(ucPortS, &num_of_entries);

		if(ret != NO_ERROR)
		{
			oamRspCode = OAM_GET_NO_RESOURCE;
			p_get_rsp = (oam_rsp_indication_t *)p_out;
			p_get_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_get_rsp->leaf = OAM_ZTE_EXT_LEAF_STATIC_MAC_CFG;
			p_get_rsp->var_indication = oamRspCode;
			g_usOutDataIdx += 4;
			return ;
		}

		p_zte_port_static_mac = (oam_zte_static_mac_cfg_t*)p_out;
		p_out += sizeof(oam_zte_static_mac_cfg_t);

		p_zte_port_static_mac->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_zte_port_static_mac->leaf = OAM_ZTE_EXT_LEAF_STATIC_MAC_CFG;
		p_zte_port_static_mac->action = 0x03;


		p_zte_port_static_mac->num_of_entries = num_of_entries;
		p_zte_port_static_mac->width = 2 + 8 * num_of_entries;

		for(i = 0; i < num_of_entries; i++)
		{
			p_zte_static_mac_entry = (oam_zte_static_mac_entry_t *)p_out;

			ret=stub_zte_port_static_mac_entry_get(ucPortS, i, p_zte_static_mac_entry);
			if(ret != NO_ERROR)
			{
				p_zte_port_static_mac->width = OAM_GET_NO_RESOURCE;
				g_usOutDataIdx += 4;
				return ;
			}
			p_out += sizeof(oam_zte_static_mac_entry_t);

		}

		g_usOutDataIdx += p_zte_port_static_mac->width + 4;
	}

	return;
}

void eopl_zte_set_static_mac_cfg(u8_t *p_in)
{
	u8_t ucPortS, ucPortE, curPortId;
	u8_t                           *p_out;
	oam_set_response_t	           *p_rsp;
	oam_instance_tlv_t_2_0	           *p_inst;
	oam_zte_static_mac_cfg_t        *p_zte_static_mac;
	oam_zte_static_mac_entry_t      *p_zte_static_mac_entry;
	u8_t                           num_of_entries;
	u8_t                           i;
	u32_t                          ret;

	ret=OPL_OK;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t_2_0 *)p_out;
	*p_inst =  port_instance_2_0;
	p_out += sizeof(oam_instance_tlv_t_2_0);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_STATIC_MAC_CFG;

	p_zte_static_mac = (oam_zte_static_mac_cfg_t *)p_in;
	p_in += sizeof(oam_zte_static_mac_cfg_t);

	num_of_entries = p_zte_static_mac->num_of_entries;

	p_zte_static_mac_entry = (oam_zte_static_mac_entry_t *)p_in;

	if(p_zte_static_mac->action == 0x00)//delete
	{
		for(i = 0; i < num_of_entries; i++)
		{
			for(curPortId=ucPortS; curPortId<=ucPortE; curPortId++)
			{
				ret = stub_zte_port_static_mac_del(curPortId, p_zte_static_mac_entry);
				if(ret != NO_ERROR)
					goto send_rsp;
			}

			p_zte_static_mac_entry++;
		}
	}
	else if(p_zte_static_mac->action == 0x01)//add
	{
		for(i = 0; i < num_of_entries; i++)
		{
			for(curPortId=ucPortS; curPortId<=ucPortE; curPortId++)
			{
				ret = stub_zte_port_static_mac_add(curPortId, p_zte_static_mac_entry);
				if(ret != NO_ERROR)
					goto send_rsp;
			}

			p_zte_static_mac_entry++;
		}
	}
	else if(p_zte_static_mac->action == 0x02)//clear
	{
		for(; ucPortS<=ucPortE; ucPortS++)
		{
			ret = stub_zte_port_static_mac_clear(ucPortS);
			if(ret != NO_ERROR)
				goto send_rsp;
		}
	}

send_rsp:
	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	p_out += sizeof(oam_set_response_t);
	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}


u32_t stub_zte_get_uni_isolation(oam_zte_uni_isolation_t *p_uni_isolation)
{
	u8_t  isolation;
	u32_t ret;

	//p_uni_isolation->isolation = stub_zte_uni_isolation.isolation;
	ret = odmGetLocalSwitch(ODM_START_PORT_NUN, &isolation);

	if(ret != NO_ERROR)
		return ret;

	p_uni_isolation->isolation = isolation?0:1;

	return NO_ERROR;
}

u32_t stub_zte_set_uni_isolation(oam_zte_uni_isolation_t *p_uni_isolation)
{
	u8_t  port_id;
	u32_t ret;

	//stub_zte_uni_isolation.isolation = p_uni_isolation->isolation;
	for(port_id = ODM_START_PORT_NUN; port_id <= ODM_NUM_OF_PORTS; port_id++)
	{
		ret = odmSetLocalSwitch(port_id, p_uni_isolation->isolation?0:1);
		if(ret != NO_ERROR)
			return ret;
	}

	return NO_ERROR;
}

void eopl_zte_get_uni_isolation(void)//onu based
{
	u8_t	               *p_out;
	u8_t					oamRspCode;
	oam_rsp_indication_t	*p_get_rsp;
	oam_zte_uni_isolation_t *p_zte_uni_isolation;
	oam_zte_uni_isolation_t uni_isolation;
	u32_t                  ret;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	ret = stub_zte_get_uni_isolation(&uni_isolation);

	if(ret != NO_ERROR)
	{
		oamRspCode = OAM_GET_NO_RESOURCE;
		p_get_rsp = (oam_rsp_indication_t *)p_out;
		p_get_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_get_rsp->leaf = OAM_ZTE_EXT_LEAF_UNI_ISOLATION;
		p_get_rsp->var_indication = oamRspCode;
		g_usOutDataIdx += 4;
		return ;
	}

	else
	{
		p_zte_uni_isolation = (oam_zte_uni_isolation_t *)p_out;
		p_zte_uni_isolation->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_zte_uni_isolation->leaf = OAM_ZTE_EXT_LEAF_UNI_ISOLATION;
		p_zte_uni_isolation->width = 1;
		p_zte_uni_isolation->isolation = uni_isolation.isolation;

		g_usOutDataIdx += p_zte_uni_isolation->width + 4;
	}

	return;
}

void eopl_zte_set_uni_isolation(u8_t *p_in)
{
	u8_t ret, *p_out;
	oam_set_response_t	    *p_rsp;
	oam_zte_uni_isolation_t  *p_zte_uni_isolation;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_UNI_ISOLATION;

	p_zte_uni_isolation = (oam_zte_uni_isolation_t *)p_in;

	ret = stub_zte_set_uni_isolation(p_zte_uni_isolation);
	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += 4;

	return;
}

u32_t stub_zte_get_uni_eth_stats(u8_t port_id, oam_zte_onu_ether_stats_t *p_eth_stats)
{
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)
	p_eth_stats->ifInOctets = 100;
	p_eth_stats->ifInUcastPkts = 101;
	p_eth_stats->ifInNUcastPkts = 102;
	p_eth_stats->ifInDiscards = 103;
	p_eth_stats->ifInErrors = 104;
	p_eth_stats->ifOutOctets = 105;
	p_eth_stats->ifOutUcastPkts = 106;
	p_eth_stats->ifOutNUcastPkts = 107;
	p_eth_stats->ifOutDiscards = 108;
	p_eth_stats->ifOutErrors = 109;
#elif defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	p_eth_stats->ifInOctets =
	    (UINT64)g_astCntSwhPort[port_id][15].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][15].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][16].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][16].stAdded.uiHigh << 32);
	p_eth_stats->ifInUcastPkts =
        (UINT64)g_astCntSwhPort[port_id][5].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][5].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][6].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][6].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][7].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][7].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][8].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][8].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][9].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][9].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][10].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][10].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][11].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][11].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][12].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][12].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][13].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][13].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][14].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][14].stAdded.uiHigh << 32) -
	    (
        (UINT64)g_astCntSwhPort[port_id][3].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][3].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][4].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][4].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][0].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][0].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][2].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][2].stAdded.uiHigh << 32)
        );
	p_eth_stats->ifInNUcastPkts =
        (UINT64)g_astCntSwhPort[port_id][0].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][0].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][2].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][2].stAdded.uiHigh << 32);
	p_eth_stats->ifInDiscards =
        (UINT64)g_astCntSwhPort[port_id][17].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][17].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][18].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][18].stAdded.uiHigh << 32);
	p_eth_stats->ifInErrors =
        (UINT64)g_astCntSwhPort[port_id][3].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][3].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][4].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][4].stAdded.uiHigh << 32);

	p_eth_stats->ifOutOctets =
        (UINT64)g_astCntSwhPort[port_id][31].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][31].stAdded.uiHigh << 32);
	p_eth_stats->ifOutUcastPkts =
        (UINT64)g_astCntSwhPort[port_id][23].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][23].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][24].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][24].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][25].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][25].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][26].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][26].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][27].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][27].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][28].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][28].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][29].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][29].stAdded.uiHigh << 32) -
	    (
        (UINT64)g_astCntSwhPort[port_id][30].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][30].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][19].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][19].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][21].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][21].stAdded.uiHigh << 32)
        );
	p_eth_stats->ifOutNUcastPkts =
        (UINT64)g_astCntSwhPort[port_id][19].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][19].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][21].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][21].stAdded.uiHigh << 32);
	p_eth_stats->ifOutDiscards =
        (UINT64)g_astCntSwhPort[port_id][22].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][22].stAdded.uiHigh << 32) +
	    (UINT64)g_astCntSwhPort[port_id][33].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][33].stAdded.uiHigh << 32);
	p_eth_stats->ifOutErrors =
        (UINT64)g_astCntSwhPort[port_id][30].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntSwhPort[port_id][30].stAdded.uiHigh << 32);
#else
	OPL_CNT_t oplCounter[DAL_CNT_GE_NUM];

	dalCounterGet(DAL_CNT_POLL_GE_MASK, oplCounter, 0, DAL_CNT_GE_NUM);

	p_eth_stats->ifInOctets =
	    (UINT64)oplCounter[34].stAdded.uiLow +
	    (UINT64)((UINT64)oplCounter[34].stAdded.uiHigh << 32);
	p_eth_stats->ifInUcastPkts =
        (UINT64)oplCounter[13].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[13].stAdded.uiHigh << 32) -
        (UINT64)oplCounter[20].stAdded.uiLow -
        (UINT64)((UINT64)oplCounter[20].stAdded.uiHigh << 32) -
        (UINT64)oplCounter[21].stAdded.uiLow -
        (UINT64)((UINT64)oplCounter[21].stAdded.uiHigh << 32);
	p_eth_stats->ifInNUcastPkts =
        (UINT64)oplCounter[20].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[20].stAdded.uiHigh << 32) +
        (UINT64)oplCounter[21].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[21].stAdded.uiHigh << 32);
	p_eth_stats->ifInDiscards =
        (UINT64)oplCounter[14].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[14].stAdded.uiHigh << 32);
	p_eth_stats->ifInErrors =
        (UINT64)oplCounter[19].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[19].stAdded.uiHigh << 32) -
        (UINT64)oplCounter[13].stAdded.uiLow -
        (UINT64)((UINT64)oplCounter[13].stAdded.uiHigh << 32);

    p_eth_stats->ifOutOctets =
        (UINT64)oplCounter[11].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[11].stAdded.uiHigh << 32);
	p_eth_stats->ifOutUcastPkts =
        (UINT64)oplCounter[0].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[0].stAdded.uiHigh << 32) -
        (UINT64)oplCounter[1].stAdded.uiLow -
        (UINT64)((UINT64)oplCounter[1].stAdded.uiHigh << 32) -
        (UINT64)oplCounter[2].stAdded.uiLow -
        (UINT64)((UINT64)oplCounter[2].stAdded.uiHigh << 32);
	p_eth_stats->ifOutNUcastPkts =
        (UINT64)oplCounter[1].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[1].stAdded.uiHigh << 32) +
        (UINT64)oplCounter[2].stAdded.uiLow +
        (UINT64)((UINT64)oplCounter[2].stAdded.uiHigh << 32);
	p_eth_stats->ifOutDiscards = 0;
	p_eth_stats->ifOutErrors = 0;

#endif

	return NO_ERROR;
}

void eopl_zte_get_onu_ether_stats(void)//port based
{
	u32_t                    ret;
	u8_t                     ucPortS;
	u8_t                     ucPortE;
	u8_t                     *p_out;
	u8_t					 oamRspCode;
	oam_rsp_indication_t     *p_get_rsp;
	oam_instance_tlv_t_2_0	     *p_inst;
	oam_zte_onu_ether_stats_t *p_zte_port_eth_stats;
	oam_zte_onu_ether_stats_t zte_port_eth_stats;

	ret = NO_ERROR;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_out;
		*p_inst = port_instance_2_0;
		p_out += sizeof(oam_instance_tlv_t_2_0);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

		ret = stub_zte_get_uni_eth_stats(ucPortS-1, &zte_port_eth_stats);

		if(ret != NO_ERROR)
		{
			oamRspCode = OAM_GET_NO_RESOURCE;
			p_get_rsp = (oam_rsp_indication_t *)p_out;
			p_get_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_get_rsp->leaf = OAM_ZTE_EXT_LEAF_ONU_ETHER_STATS;
			p_get_rsp->var_indication = oamRspCode;
			g_usOutDataIdx += 4;
			return ;
		}

		p_zte_port_eth_stats = (oam_zte_onu_ether_stats_t *)p_out;
		p_zte_port_eth_stats->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_zte_port_eth_stats->leaf = OAM_ZTE_EXT_LEAF_ONU_ETHER_STATS;
		p_zte_port_eth_stats->width = 112;

		p_zte_port_eth_stats->ifInOctets = zte_port_eth_stats.ifInOctets;
		p_zte_port_eth_stats->ifInUcastPkts = zte_port_eth_stats.ifInUcastPkts;
		p_zte_port_eth_stats->ifInNUcastPkts = zte_port_eth_stats.ifInNUcastPkts;
		p_zte_port_eth_stats->ifInDiscards = zte_port_eth_stats.ifInDiscards;
		p_zte_port_eth_stats->ifInErrors = zte_port_eth_stats.ifInErrors;
		p_zte_port_eth_stats->ifOutOctets = zte_port_eth_stats.ifOutOctets;
		p_zte_port_eth_stats->ifOutUcastPkts = zte_port_eth_stats.ifOutUcastPkts;
		p_zte_port_eth_stats->ifOutNUcastPkts = zte_port_eth_stats.ifOutNUcastPkts;
		p_zte_port_eth_stats->ifOutDiscards = zte_port_eth_stats.ifOutDiscards;
		p_zte_port_eth_stats->ifOutErrors = zte_port_eth_stats.ifOutErrors;

		g_usOutDataIdx += p_zte_port_eth_stats->width + 4;
	}

	return;
}


u32_t stub_zte_get_port_mode(u8_t port_id, oam_zte_port_mode_t *p_zte_port_mode)
{
	u32_t ret;
	u32_t duplex;
	u32_t speed;
	u32_t autoneg;

	//p_zte_port_mode->port_mode = stub_zte_port_mode[port_id].port_mode;
	ret = odmPortAutoEnableGet(port_id, &autoneg);
	if(ret != NO_ERROR)
		return ret;

	ret = odmPortDuplexGet(port_id, &duplex);
	if(ret != NO_ERROR)
		return ret;

	ret = odmPortSpeedGet(port_id, &speed);
	if(ret != NO_ERROR)
		return ret;

	if(autoneg)
	{
		p_zte_port_mode->port_mode = ZTE_PORT_MODE_AUTO;
	}
	else if(speed == ODM_PORT_SPEED_10M)
	{
		if(duplex == ODM_PORT_DUPLEX_HALF)
		{
			p_zte_port_mode->port_mode = ZTE_PORT_MODE_10M_HALF;
		}
		else
		{
			p_zte_port_mode->port_mode = ZTE_PORT_MODE_10M_FULL;
		}
	}
	else if(speed == ODM_PORT_SPEED_100M)
	{
		if(duplex == ODM_PORT_DUPLEX_HALF)
		{
			p_zte_port_mode->port_mode = ZTE_PORT_MODE_100M_HALF;
		}
		else
		{
			p_zte_port_mode->port_mode = ZTE_PORT_MODE_100M_FULL;
		}
	}
	else if(speed == ODM_PORT_SPEED_1000M)
	{
		p_zte_port_mode->port_mode = ZTE_PORT_MODE_1000M;
	}

	return NO_ERROR;
}

u32_t stub_zte_set_port_mode(u8_t port_id, u8_t p_zte_port_mode)
{
	//zte_port_mode[port_id].port_mode = p_zte_port_mode->port_mode;
	u8_t port_mode;
	u32_t autoneg;
	u32_t speed;
	u32_t duplex;
	u32_t ret = NO_ERROR;

	port_mode = p_zte_port_mode-1;

	/* 0x00: auto */
	/* 0x01: 10M half */
	/* 0x02: 10M full */
	/* 0x03: 100M half */
	/* 0x04: 100M full */
	/* 0x05: 1000M */
	/* 0x06: 10G */
	switch(port_mode)
	{
	case ZTE_PORT_MODE_AUTO:
		autoneg = 1;

		ret = odmPortAutoEnableSet(port_id, autoneg);
		if(ret != NO_ERROR)
			return ret ;

		break;

	case ZTE_PORT_MODE_10M_HALF:
		autoneg = 0;
		ret = odmPortAutoEnableSet(port_id, autoneg);
		if(ret != NO_ERROR)
			return ret ;

		speed = ODM_PORT_SPEED_10M;
		duplex = ODM_PORT_DUPLEX_HALF;

		ret = odmPortSpeedSet(port_id, speed);
		if(ret != NO_ERROR)
			return ret;

		ret = odmPortDuplexSet(port_id, duplex);
		if(ret != NO_ERROR)
			return ret;

		break;

	case ZTE_PORT_MODE_10M_FULL:
		autoneg = 0;
		ret = odmPortAutoEnableSet(port_id, autoneg);
		if(ret != NO_ERROR)
			return ret ;

		speed = ODM_PORT_SPEED_10M;
		duplex = ODM_PORT_DUPLEX_FULL;

		ret = odmPortSpeedSet(port_id, speed);
		if(ret != NO_ERROR)
			return ret;

		ret = odmPortDuplexSet(port_id, duplex);
		if(ret != NO_ERROR)
			return ret;

		break;

	case ZTE_PORT_MODE_100M_HALF:
		autoneg = 0;
		ret = odmPortAutoEnableSet(port_id, autoneg);
		if(ret != NO_ERROR)
			return ret ;

		speed = ODM_PORT_SPEED_100M;
		duplex = ODM_PORT_DUPLEX_HALF;

		ret = odmPortSpeedSet(port_id, speed);
		if(ret != NO_ERROR)
			return ret;

		ret = odmPortDuplexSet(port_id, duplex);
		if(ret != NO_ERROR)
			return ret;

		break;

	case ZTE_PORT_MODE_100M_FULL:
		autoneg = 0;
		ret = odmPortAutoEnableSet(port_id, autoneg);
		if(ret != NO_ERROR)
			return ret ;

		speed = ODM_PORT_SPEED_100M;
		duplex = ODM_PORT_DUPLEX_FULL;

		ret = odmPortSpeedSet(port_id, speed);
		if(ret != NO_ERROR)
			return ret;

		ret = odmPortDuplexSet(port_id, duplex);
		if(ret != NO_ERROR)
			return ret;

		break;

	case ZTE_PORT_MODE_1000M:
		autoneg = 0;
		ret = odmPortAutoEnableSet(port_id, autoneg);
		if(ret != NO_ERROR)
			return ret ;

		speed = ODM_PORT_SPEED_1000M;
		duplex = ODM_PORT_DUPLEX_FULL;

		ret = odmPortSpeedSet(port_id, speed);
		if(ret != NO_ERROR)
			return ret;

		ret = odmPortDuplexSet(port_id, duplex);
		if(ret != NO_ERROR)
			return ret;
		break;
	default :
		ret = ERR_INVALID_PARAMETERS;
		 break;
	}
	return ret;
}

u8_t set_port_id = 0;
u8_t set_port_mode = 0;

void *AfterPortModeThread(void * pszParam)
{
	int ret=0;

	vosSleep(1);
	if(set_port_mode != 0)
	{
		ret = stub_zte_set_port_mode(set_port_id , set_port_mode);
		if(ret == NO_ERROR)
		{
			set_port_mode = 0;
			set_port_id = 0;
		}
	}
    vosThreadExit(0);
    return NULL;
}

void eopl_zte_get_port_mode(u8_t type,u8_t port)//port based
{
	u8_t	ret;
	u8_t    *p_out;
	u8_t	oamRspCode;
	oam_rsp_indication_t *p_get_rsp;
	oam_instance_tlv_t_2_0	*p_inst;
	oam_zte_port_mode_t  *p_zte_port_mode;
	oam_zte_port_mode_t   port_mode;

	EOPL_ZTE_HEADER_PROCESS(OAM_EXT_CODE_SET_RESPONSE,type,port);

	p_zte_port_mode = (oam_zte_port_mode_t *)p_out;
	p_zte_port_mode->branch = OAM_ZTE_ZTE_OUI_EXT_BRANCH_ATTR;
	p_zte_port_mode->leaf = OAM_ZTE_EXT_LEAF_GET_PORT_MODE;

	if( type!=0x01 || port > ODM_NUM_OF_PORTS || port < ODM_START_PORT_NUN)
	{
		p_zte_port_mode->width  = OAM_BAD_PARAM_VALUE;
		g_usOutDataIdx += 4;
		return;
	}

	ret = stub_zte_get_port_mode(port, p_zte_port_mode);

	if(ret != NO_ERROR)
	{
		p_zte_port_mode->width  = OAM_BAD_PARAM_VALUE;

		g_usOutDataIdx += 4;
	}
	else
	{

		p_zte_port_mode->width = 1;
		p_zte_port_mode->port_mode = p_zte_port_mode->port_mode+1;

		g_usOutDataIdx += p_zte_port_mode->width + 4;
	}

	return;
}

void eopl_zte_set_port_mode(u8_t *p_in,u8_t type,u8_t port)//port based
{
    u32_t ret;
	u8_t  *p_out;
	oam_set_response_t	*p_rsp;
	oam_instance_tlv_t_2_0	*p_inst;
	oam_zte_port_mode_t  *p_zte_port_mode;

	ret=OPL_OK;

	EOPL_ZTE_HEADER_PROCESS(OAM_EXT_CODE_SET_RESPONSE,type,port);
	
	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_ZTE_ZTE_OUI_EXT_BRANCH_ATTR;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_SET_PORT_MODE;

	if( type!=0x01 || port > ODM_NUM_OF_PORTS || port < ODM_START_PORT_NUN)
	{
		p_rsp->indication = OAM_BAD_PARAM_VALUE;
		g_usOutDataIdx += 4;
		return;
	}

	p_zte_port_mode = (oam_zte_port_mode_t *)p_in;

    set_port_id = port;
	set_port_mode = p_zte_port_mode->port_mode;

	if( set_port_mode > 0x06)
	{
		p_rsp->indication = OAM_BAD_PARAM_VALUE;		
		g_usOutDataIdx += 4;	
		return;
	}
	
	//ret = stub_zte_set_port_mode(port, p_zte_port_mode->port_mode);
	if (NULL == vosThreadCreate("tPortMode", OP_VOS_THREAD_STKSZ, 110, 
        (void *)AfterPortModeThread, NULL))
	{
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING," create AfterPortModeThread thread failed\n");	
        ret = ERROR;
	}
	
	if(!ret)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_BAD_PARAM_VALUE;
	}

	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}


#define OAM_MAX_MAC_NUM 200
#define MAX_ENTITY 17

typedef struct{
u16_t width;
u16_t random_value;
u16_t sequence_num;
u16_t total_addr_num;
}__attribute__((__packed__))mac_table_head_t;

typedef struct
{
char *pMacPool;
int macPos;
int macCount;
}mac_table_t;
typedef struct
{
	mac_table_head_t macHead;
	mac_table_t staticUnicastTable;
	mac_table_t dymUnicastTable;
	mac_table_t multicastTable;	
}mac_table_value_t;

/*add  for get mac table*/
mac_table_value_t gMacPooValue = {.macHead.random_value = 0xffff};

/* This macro checks for a multicast mac address    */
#define IS_MULTICAST_MACADDR(mac)  (((mac)[0] & 0x1) == 1)
    
/* This macro checks for an broadcast mac address     */
#define IS_BROADCAST_MACADDR(mac) (((mac)[0] == 0xFF) && \
    ((mac)[1] == 0xFF) && ((mac)[2] == 0xFF) && \
    ((mac)[3] == 0xFF) && ((mac)[4] == 0xFF) && \
    ((mac)[5] == 0xFF))
	
void init_mac_pool(u16_t randomValue)
{
	gMacPooValue.macHead.random_value = randomValue;
	gMacPooValue.macHead.sequence_num = 0;
	gMacPooValue.macHead.total_addr_num = 0;

	gMacPooValue.staticUnicastTable.macCount = 0;
	gMacPooValue.dymUnicastTable.macCount = 0;
	gMacPooValue.multicastTable.macCount = 0;
	gMacPooValue.staticUnicastTable.macPos = 0;
	gMacPooValue.dymUnicastTable.macPos = 0;
	gMacPooValue.multicastTable.macPos = 0;
	
	
	gMacPooValue.macHead.total_addr_num = 0;
		
	if(gMacPooValue.multicastTable.pMacPool == NULL)
	{
		gMacPooValue.multicastTable.pMacPool = vosAlloc(sizeof(oam_zte_uni_mac_query_entry_t)*OAM_MAX_MAC_NUM);
	}

	if(gMacPooValue.dymUnicastTable.pMacPool == NULL)
	{
		gMacPooValue.dymUnicastTable.pMacPool = vosAlloc(sizeof(oam_zte_uni_mac_query_entry_t)*OAM_MAX_MAC_NUM);
	}

	if(gMacPooValue.staticUnicastTable.pMacPool == NULL)
	{
		gMacPooValue.staticUnicastTable.pMacPool = vosAlloc(sizeof(oam_zte_uni_mac_query_entry_t)*OAM_MAX_MAC_NUM);
	}
}

int add_mac_entity(FDB_MAC_ENTRY_INFO_S *pAtuEntry, mac_table_value_t *pMacPoolValue)
{
	int pos = 0;
	int type; 

	if(IS_MULTICAST_MACADDR(pAtuEntry->aucMacAddress))
	{
		if(IS_BROADCAST_MACADDR((pAtuEntry->aucMacAddress)))
			return 0;
		
		if(pMacPoolValue->multicastTable.macCount < OAM_MAX_MAC_NUM)
		{
			pos = pMacPoolValue->multicastTable.macCount * sizeof(oam_zte_uni_mac_query_entry_t);
			memcpy(&pMacPoolValue->multicastTable.pMacPool[pos], &pAtuEntry->aucMacAddress[0], sizeof(oam_zte_uni_mac_query_entry_t));
			pMacPoolValue->multicastTable.macCount ++;		
		}
		else
		{
			//return -1;
		}
			
	}
	else
	{
		switch (pAtuEntry->usType)
		{
			case FDB_MAC_DYNAMIC:
				if(pMacPoolValue->dymUnicastTable.macCount < OAM_MAX_MAC_NUM)
				{
					pos = pMacPoolValue->dymUnicastTable.macCount * sizeof(oam_zte_uni_mac_query_entry_t);
					memcpy(&pMacPoolValue->dymUnicastTable.pMacPool[pos], &pAtuEntry->aucMacAddress[0], sizeof(oam_zte_uni_mac_query_entry_t));
					pMacPoolValue->dymUnicastTable.macCount ++;		

				}
				else
				{
					//return -1;
				}
				break;
				
			case FDB_MAC_STATIC: 
				if(pMacPoolValue->staticUnicastTable.macCount < OAM_MAX_MAC_NUM)
				{
					pos = pMacPoolValue->staticUnicastTable.macCount * sizeof(oam_zte_uni_mac_query_entry_t);
					memcpy(&pMacPoolValue->staticUnicastTable.pMacPool[pos], &pAtuEntry->aucMacAddress[0], sizeof(oam_zte_uni_mac_query_entry_t));
					pMacPoolValue->staticUnicastTable.macCount ++;		
				}
				else
				{
					//return -1;
				}				
				break;
			default:
				break;
		}
	}

	return 0;
}

void mac_resource_free()
{
	vosFree(gMacPooValue.staticUnicastTable.pMacPool);
	gMacPooValue.staticUnicastTable.pMacPool = NULL;

	vosFree(gMacPooValue.dymUnicastTable.pMacPool);
	gMacPooValue.dymUnicastTable.pMacPool = NULL;

	vosFree(gMacPooValue.multicastTable.pMacPool);
	gMacPooValue.multicastTable.pMacPool = NULL;
}

int add_mac_block_entity(oam_zte_uni_mac_query_block_t  *mac_block, mac_table_t *pMacPoolValue, int have_copyed_num)
{
	
	int cpy_num = 0;
	int total_copy_num = have_copyed_num;
		
	if((pMacPoolValue->macCount -pMacPoolValue->macPos/sizeof(oam_zte_uni_mac_query_entry_t)) > (MAX_ENTITY -total_copy_num))
	{
		cpy_num = MAX_ENTITY -have_copyed_num;
		total_copy_num = MAX_ENTITY;
	}
	else
	{
		cpy_num = pMacPoolValue->macCount -pMacPoolValue->macPos/sizeof(oam_zte_uni_mac_query_entry_t);
		total_copy_num = cpy_num + have_copyed_num;		
	}

	if(cpy_num != 0)
		memcpy(mac_block->mac_value, &pMacPoolValue->pMacPool[pMacPoolValue->macPos], cpy_num* sizeof(oam_zte_uni_mac_query_entry_t));

	pMacPoolValue->macPos += cpy_num *sizeof(oam_zte_uni_mac_query_entry_t);
		
	return cpy_num;
}


u16_t get_mac_entity(oam_zte_uni_mac_query_block_t *mac_block, u16_t  randomValue)
{
	
	int nRet;
	int cpy_num = 0;
	int total_cpy_num = 0;
	oam_zte_uni_mac_query_block_t *mac_block_head = mac_block;	
	int maxRecycleTime = OAM_MAX_MAC_NUM * 3;
	int width = 8;
	int block_head_size = 8;
	FDB_MAC_ENTRY_INFO_S stMacEntryInfo;
	OPL_STATUS rv=0;
	int free_flag = 0;
	UINT32 iterator = 0;
    BOOL bFind = FALSE; 

	if(gMacPooValue.macHead.random_value != randomValue)
	{
		init_mac_pool(randomValue);
		while(1)
		{
			vosMemSet(&stMacEntryInfo, 0, sizeof(FDB_MAC_ENTRY_INFO_S));	

			#if defined(ONU_1PORT)
			rv = dalArlMacEntryTraverse(&iterator, &stMacEntryInfo, &bFind);
			#else
			#endif
			if (TRUE != bFind)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "\r\nno mac entry exists.\r\n");			
				break;
			}
			
			if(maxRecycleTime--)
			{
				nRet = add_mac_entity(&stMacEntryInfo, &gMacPooValue);
				if(nRet != OPL_OK)
				{
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "add_mac_entity fail.\r\n");			
					gMacPooValue.macHead.random_value = 0xffff;
					mac_resource_free();
					return -1;
				}				
			}
			else
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "maxRecycleTime <0.\r\n");			
			
				break;
			}
		}
	}

//	dump_packet(gMacPooValue.dymUnicastTable.pMacPool, gMacPooValue.dymUnicastTable.macCount * sizeof(mac_t));
//	dump_packet(gMacPooValue.staticUnicastTable.pMacPool, gMacPooValue.staticUnicastTable.macCount * sizeof(mac_t));
///	dump_packet(gMacPooValue.multicastTable.pMacPool, gMacPooValue.multicastTable.macCount * sizeof(mac_t));


	cpy_num = add_mac_block_entity(mac_block_head, &gMacPooValue.dymUnicastTable, total_cpy_num);
	total_cpy_num += cpy_num;
	mac_block_head->addr_num = cpy_num; 
	mac_block_head->addr_type = 0x0000; 
	if(cpy_num != 0)
	{
		width += block_head_size;		
	}
	
	if(total_cpy_num < MAX_ENTITY)
	{
		/*前面已有block,下一个block需加在此block之后*/
		if(cpy_num != 0)
		{
			mac_block_head =(oam_zte_uni_mac_query_block_t *)&mac_block_head->mac_value[cpy_num];
		}
		
		cpy_num = add_mac_block_entity(mac_block_head, &gMacPooValue.staticUnicastTable, total_cpy_num);
		total_cpy_num += cpy_num;
		mac_block_head->addr_num = cpy_num; 
		mac_block_head->addr_type = 0x1000; 
		if(cpy_num != 0)
		{
			width += block_head_size;
		}
		
		if(total_cpy_num < MAX_ENTITY)
		{
			/*前面已有block,下一个block需加在此block之后*/
			if(cpy_num != 0)
			{
				mac_block_head =(oam_zte_uni_mac_query_block_t *)&mac_block_head->mac_value[cpy_num];
			}
			
			cpy_num = add_mac_block_entity(mac_block_head, &gMacPooValue.multicastTable, total_cpy_num);
			total_cpy_num += cpy_num;
			mac_block_head->addr_num = cpy_num; 
			mac_block_head->addr_type = 0x2000;
			
			if(cpy_num != 0)
			{
				width += block_head_size;
			}
			
			width += total_cpy_num * sizeof(oam_zte_uni_mac_query_entry_t);
			
		}
		else
		{
			width+=MAX_ENTITY*sizeof(oam_zte_uni_mac_query_entry_t);

		}
	}
	else
	{
		width+=MAX_ENTITY*sizeof(oam_zte_uni_mac_query_entry_t);
	}

	//width += total_cpy_num * sizeof(oam_zte_uni_mac_query_entry_t);
		
	gMacPooValue.macHead.sequence_num++;
	
	gMacPooValue.macHead.total_addr_num = total_cpy_num;

	if(gMacPooValue.multicastTable.macCount != 0 )
	{
		if(gMacPooValue.multicastTable.macCount == gMacPooValue.multicastTable.macPos/sizeof(oam_zte_uni_mac_query_entry_t) )
			free_flag = 1;
	}
	else if(gMacPooValue.staticUnicastTable.macCount != 0 )
	{
		if(gMacPooValue.staticUnicastTable.macCount == gMacPooValue.staticUnicastTable.macPos/sizeof(oam_zte_uni_mac_query_entry_t) )
			free_flag = 1;
	}
	else if(gMacPooValue.dymUnicastTable.macCount == gMacPooValue.dymUnicastTable.macPos/sizeof(oam_zte_uni_mac_query_entry_t))
	{
		free_flag = 1;
	}

	if(free_flag)
	{
		gMacPooValue.macHead.sequence_num |= 0x8000; 	
		mac_resource_free();
	}
		
	return width;
}


void eopl_zte_get_uni_mac_query(u8_t *p_in)//port based
{
	u8_t	ret;
	u8_t    ucPortS;
	u8_t    ucPortE;
	u8_t    *p_out;
	u16_t   random_value;
	oam_zte_uni_mac_query_t *p_zte_mac_query_in;
	oam_zte_uni_mac_query_t *p_zte_mac_query_out;
	oam_zte_uni_mac_query_block_t *p_zte_mac_query_block;
	u16_t width = 0;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_zte_mac_query_in = (oam_zte_uni_mac_query_t *)p_in;
	random_value = p_zte_mac_query_in->random_value;

	
	p_zte_mac_query_out = (oam_zte_uni_mac_query_t *)p_out;
	p_zte_mac_query_out->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_zte_mac_query_out->leaf = OAM_ZTE_EXT_LEAF_UNI_MAC_QUERY;
	
	p_out += sizeof(oam_zte_uni_mac_query_t);
	
	p_zte_mac_query_block = (oam_zte_uni_mac_query_block_t *)p_out;

	width = get_mac_entity(p_zte_mac_query_block, random_value);

	if(width == INVALID_16)
	{
		p_zte_mac_query_out->op_code = OAM_SET_NO_RESOURCE;
		g_usOutDataIdx += 4;
		return ;
	}
	p_zte_mac_query_out->op_code = OAM_SET_OK;
	p_zte_mac_query_out->width = width;
	p_zte_mac_query_out->random_value = random_value;
	p_zte_mac_query_out->seq_no = gMacPooValue.macHead.sequence_num - 1;
	p_zte_mac_query_out->total_num = gMacPooValue.macHead.total_addr_num; 
	
	g_usOutDataIdx += p_zte_mac_query_out->width + 4;

	return;
}


u32_t stub_zte_get_pmac_ds_shaping(oam_zte_pmac_ds_shaping_t *p_zte_pmac_ds_shaping)
{
      UINT32 cir =0;
      UINT32 cbs =0;
	  
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    odmPonUsShapingGet(&p_zte_pmac_ds_shaping->ds_rate_oper,&cir,&cbs);

	//  printf("the oper %x cir %x cbs %x\n",p_zte_pmac_ds_shaping->ds_rate_oper,cir,cbs);
        vosMemCpy(&p_zte_pmac_ds_shaping->ds_rate_cir[0], &cir,3);
        vosMemCpy(&p_zte_pmac_ds_shaping->ds_rate_cbs[0], &cbs, 3);
#endif
	return NO_ERROR;
}

u32_t stub_zte_set_pmac_ds_shaping(oam_zte_pmac_ds_shaping_t *p_zte_pmac_ds_shaping)
{
	UINT32 cir = 0;
	UINT32 cbs = 0;
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)

	vosMemCpy(&cir , &p_zte_pmac_ds_shaping->ds_rate_cir[0],3);
	vosMemCpy(&cbs , &p_zte_pmac_ds_shaping->ds_rate_cbs[0],3);

	//  printf("the oper %x cir %x cbs %x\n",p_zte_pmac_ds_shaping->ds_rate_oper,cir,cbs);

	odmPonUsShapingSet(p_zte_pmac_ds_shaping->ds_rate_oper,cir,cbs);
       
#endif
	return NO_ERROR;
}

void eopl_zte_get_pmac_ds_shaping(void)//port based
{
	u8_t	ret;
	u8_t    *p_out;
	u8_t    oamRspCode;
	oam_rsp_indication_t         *p_get_rsp;
	oam_instance_tlv_t_2_0	     *p_inst;
	oam_zte_pmac_ds_shaping_t *p_zte_pmac_ds_shaping;
	oam_zte_pmac_ds_shaping_t  pmac_ds_shaping;

	ret = NO_ERROR;
	memset(&pmac_ds_shaping , 0, sizeof(oam_zte_pmac_ds_shaping_t));
	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	ret = stub_zte_get_pmac_ds_shaping(&pmac_ds_shaping);

	if(ret != NO_ERROR)
	{
		oamRspCode = OAM_GET_NO_RESOURCE;
		p_get_rsp = (oam_rsp_indication_t *)p_out;
		p_get_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_get_rsp->leaf = OAM_ZTE_EXT_LEAF_PMAC_DS_SHAPING;
		p_get_rsp->var_indication = oamRspCode;
		g_usOutDataIdx += 4;
	}

	else
	{
		p_zte_pmac_ds_shaping = (oam_zte_pmac_ds_shaping_t *)p_out;
		p_zte_pmac_ds_shaping->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_zte_pmac_ds_shaping->leaf = OAM_ZTE_EXT_LEAF_PMAC_DS_SHAPING;

		if(pmac_ds_shaping.ds_rate_oper == 0x00)
		{
			p_zte_pmac_ds_shaping->width = 1;
			p_zte_pmac_ds_shaping->ds_rate_oper = 0x00;
		}
		else if(pmac_ds_shaping.ds_rate_oper == 0x01)
		{
			p_zte_pmac_ds_shaping->width = 7;
			p_zte_pmac_ds_shaping->ds_rate_oper = 0x01;

			vosMemCpy(&p_zte_pmac_ds_shaping->ds_rate_cir[0],
				      &pmac_ds_shaping.ds_rate_cir[0],
				      3);

			vosMemCpy(&p_zte_pmac_ds_shaping->ds_rate_cbs[0],
				      &pmac_ds_shaping.ds_rate_cbs[0],
				      3);
		}

		g_usOutDataIdx += p_zte_pmac_ds_shaping->width + 4;
	}

	return;
}

void eopl_zte_set_pmac_ds_shaping(u8_t *p_in)//port based
{
	u8_t                     *p_out;
	oam_set_response_t	     *p_rsp;
	oam_instance_tlv_t_2_0	     *p_inst;
	oam_zte_pmac_ds_shaping_t *p_zte_pmac_ds_shaping;
	u32_t                     ret;

	ret = NO_ERROR;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_PMAC_DS_SHAPING;

	p_zte_pmac_ds_shaping = (oam_zte_pmac_ds_shaping_t *)p_in;

	ret = stub_zte_set_pmac_ds_shaping(p_zte_pmac_ds_shaping);

	if(ret != NO_ERROR)
			goto send_rsp;

send_rsp:

	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	p_out += sizeof(oam_set_response_t);
	g_usOutDataIdx += sizeof(oam_set_response_t);
}

u32_t stub_zte_get_pmac_buff_cfg(u8_t direction, oam_zte_pmac_buff_cfg_t *p_zte_pmac_buff_cfg)
{
	UINT16 bufsize = 0;
	UINT32 bufsize_byte = 0;
	int ret = 0;
	ret = tmQueueMaxCellNumHwRead(direction,0,&bufsize);
	//printf("get the bufsize is 0x%x\n",bufsize);

	bufsize_byte = bufsize*60;   /*一个cell等于60字节定长*/
	
	p_zte_pmac_buff_cfg->buff_size[0] = (bufsize_byte>>16)  & 0xff;
	p_zte_pmac_buff_cfg->buff_size[1] = (bufsize_byte>>8)  & 0xff;
	p_zte_pmac_buff_cfg->buff_size[2] = bufsize_byte  & 0xff;


	return ret;
}

u32_t stub_zte_set_pmac_buff_cfg(oam_zte_pmac_buff_cfg_t *p_zte_pmac_buff_cfg)
{
	u8_t direction =0;
	u8_t indication = 0;
	UINT32 bufsize = 0;
	int ret=0;
	direction = p_zte_pmac_buff_cfg->direction;
	indication = p_zte_pmac_buff_cfg->mgmt_indication;
	//vosMemCpy(&bufsize , &p_zte_pmac_buff_cfg->buff_size[0], 3);
	bufsize = (p_zte_pmac_buff_cfg->buff_size[0] <<16 )+( p_zte_pmac_buff_cfg->buff_size[1]<<8)+(p_zte_pmac_buff_cfg->buff_size[2]);

	//printf("set the bufsize is 0x%x\n",bufsize);
	
	bufsize = bufsize/60; /*一个cell等于60字节定长*/
	if(bufsize > STREAM_BUFFER_MAX)
	{
		return OPL_ERROR;
	}
	if(indication ==0x00)
	{
		if(direction == DOWN_STREAM)
		{
			ret= odmQueueLimitSet(direction , 0, DOWNSTREAM_BUFFER_DEFAULT);
		}
		else if(direction == UP_STREAM)
		{
			ret= odmQueueLimitSet(direction , 0, UPSTREAM_BUFFER_DEFAULT);
		}		
		else if(direction == 0x02)
		{
			ret = odmQueueLimitSet(DOWN_STREAM , 0, DOWNSTREAM_BUFFER_DEFAULT);
			if(ret != OPL_OK)
			{
				OP_DEBUG(DEBUG_LEVEL_WARNING,"line:%d\n", __LINE__);
				return ret;
			}
			
			ret = odmQueueLimitSet(UP_STREAM , 0, UPSTREAM_BUFFER_DEFAULT);
			if(ret != OPL_OK)
			{
				OP_DEBUG(DEBUG_LEVEL_WARNING,"line:%d\n", __LINE__);
				return ret;
			}

		}
		else
		{
			ret = OPL_ERROR;
		}
	}
	if(indication ==0x01)
	{
		if(direction == 0x02)
		{
			ret = odmQueueLimitSet(DOWN_STREAM , 0, bufsize);
			if(ret != OPL_OK)
			{
				OP_DEBUG(DEBUG_LEVEL_WARNING,"line:%d\n", __LINE__);
				return ret;
			}

			ret = odmQueueLimitSet(UP_STREAM , 0, bufsize);
			if(ret != OPL_OK)
			{
				OP_DEBUG(DEBUG_LEVEL_WARNING,"line:%d\n", __LINE__);
				return ret;
			}

		}
		else if(direction == 0x00 || direction == 0x01)
		{
			ret = odmQueueLimitSet(direction , 0, bufsize);
		}
		else
		{
			ret = OPL_ERROR;
		}
	}

	return ret;

}

void eopl_zte_get_pmac_buff_cfg(u8_t *p_in)//onu based
{
	u8_t	ret;
	u8_t    *p_out;
	oam_instance_tlv_t_2_0	   *p_inst;
	oam_zte_pmac_buff_cfg_t *p_zte_pmac_buff_cfg;
	oam_zte_pmac_buff_cfg_t  pmac_buff_cfg;
	u8_t                    direction = 0;
	u8_t                    indication = 0;

	ret = NO_ERROR;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_zte_pmac_buff_cfg = (oam_zte_pmac_buff_cfg_t *)p_in;
	direction = p_zte_pmac_buff_cfg->direction;
	indication = p_zte_pmac_buff_cfg->mgmt_indication;

	p_zte_pmac_buff_cfg = (oam_zte_pmac_buff_cfg_t *)p_out;
	p_zte_pmac_buff_cfg->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_zte_pmac_buff_cfg->leaf = OAM_ZTE_EXT_LEAF_PMAC_BUFF_CFG;

	ret = stub_zte_get_pmac_buff_cfg(direction, &pmac_buff_cfg);
	if(ret != OPL_OK)
	{
		p_zte_pmac_buff_cfg->width = OAM_GET_NO_RESOURCE;
		g_usOutDataIdx += 4;
		return;
	}

	p_zte_pmac_buff_cfg->direction = direction;


	p_zte_pmac_buff_cfg->mgmt_indication = indication ;

	p_zte_pmac_buff_cfg->width = 5;

	vosMemCpy(&p_zte_pmac_buff_cfg->buff_size[0],&pmac_buff_cfg.buff_size[0], 3);

	g_usOutDataIdx += p_zte_pmac_buff_cfg->width + 4;
}

void eopl_zte_set_pmac_buff_cfg(u8_t *p_in)//onu based
{
	u8_t                     *p_out;
	oam_set_response_t	     *p_rsp;
	oam_instance_tlv_t_2_0	     *p_inst;
	oam_zte_pmac_buff_cfg_t   *p_zte_pmac_buff_cfg;
	u32_t                     ret;

	ret = NO_ERROR;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_PMAC_BUFF_CFG;

	p_zte_pmac_buff_cfg = (oam_zte_pmac_buff_cfg_t *)p_in;

	ret = stub_zte_set_pmac_buff_cfg(p_zte_pmac_buff_cfg);


	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	p_out += sizeof(oam_set_response_t);
	g_usOutDataIdx += sizeof(oam_set_response_t);
}

/* OAM_ZTE_EXT_LEAF_PMAC_BUFF_QUERY */
static oam_zte_pmac_buff_cfg_t stub_zte_pmac_buff_query;

u32_t stub_zte_get_pmac_buff_query(oam_zte_pmac_buff_query_t *p_zte_pmac_buff_query)
{
	UINT32 ds_bufsize = DOWNSTREAM_BUFFER_MAX*60;
	UINT32 us_bufsize = UPSTREAM_BUFFER_MAX *60;

	p_zte_pmac_buff_query->buff_mgmt_capability =0x01;

	p_zte_pmac_buff_query->ds_buff_max[0] = (ds_bufsize>>16)  & 0xff;
	p_zte_pmac_buff_query->ds_buff_max[1] = (ds_bufsize>>8)  & 0xff;
	p_zte_pmac_buff_query->ds_buff_max[2] = ds_bufsize  & 0xff;

	p_zte_pmac_buff_query->us_buff_max[0] = (us_bufsize>>16)  & 0xff;
	p_zte_pmac_buff_query->us_buff_max[1] = (us_bufsize>>8)  & 0xff;
	p_zte_pmac_buff_query->us_buff_max[2] = us_bufsize  & 0xff;
	
	return NO_ERROR;
}

void eopl_zte_get_pmac_buff_query(void)//onu based
{
	u32_t	                 ret;
	u8_t                     *p_out;
	oam_instance_tlv_t_2_0	     *p_inst;
	oam_zte_pmac_buff_query_t *p_zte_pmac_buff_query;
	oam_zte_pmac_buff_query_t  pmac_buff_query;

	ret = NO_ERROR;

	memset(&pmac_buff_query,0,sizeof(oam_zte_pmac_buff_query_t));

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}


	p_zte_pmac_buff_query = (oam_zte_pmac_buff_query_t *)p_out;
	p_zte_pmac_buff_query->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_zte_pmac_buff_query->leaf = OAM_ZTE_EXT_LEAF_PMAC_BUFF_QUERY;
	p_zte_pmac_buff_query->width = 13;

	ret = stub_zte_get_pmac_buff_query(&pmac_buff_query);
	if(ret != NO_ERROR)
	{
		p_zte_pmac_buff_query->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_zte_pmac_buff_query->leaf = OAM_ZTE_EXT_LEAF_PMAC_BUFF_QUERY;
		p_zte_pmac_buff_query->width = OAM_GET_NO_RESOURCE;
		g_usOutDataIdx += 4;

		return;
	}

	p_zte_pmac_buff_query->buff_mgmt_capability = pmac_buff_query.buff_mgmt_capability;

	vosMemCpy(&p_zte_pmac_buff_query->ds_buff_min[0],
			  &pmac_buff_query.ds_buff_min[0],
			  3);

	vosMemCpy(&p_zte_pmac_buff_query->ds_buff_max[0],
			  &pmac_buff_query.ds_buff_max[0],
			  3);

	vosMemCpy(&p_zte_pmac_buff_query->us_buff_min[0],
			  &pmac_buff_query.us_buff_min[0],
			  3);

	vosMemCpy(&p_zte_pmac_buff_query->us_buff_max[0],
			  &pmac_buff_query.us_buff_max[0],
			  3);

	g_usOutDataIdx += p_zte_pmac_buff_query->width + 4;

	return;
}


/* OAM_ZTE_EXT_LEAF_UNI_STATS_CFG */

u32_t stub_zte_get_uni_stats_cfg(u8_t port_id, oam_zte_uni_stats_cfg_t *p_zte_uni_stats_cfg)
{

	if(statistics_port[port_id].statistics_duration_time==0)
	{					
		p_zte_uni_stats_cfg->width=8;

		p_zte_uni_stats_cfg->action=0;
		memset(&p_zte_uni_stats_cfg->stats_interval[0],0,3);
		p_zte_uni_stats_cfg->stats_duration=0;

	}
	else
	{
		p_zte_uni_stats_cfg->width=8;

		p_zte_uni_stats_cfg->action=statistics_port[port_id].statistics_action;
	
		p_zte_uni_stats_cfg->stats_interval[0]=statistics_port[port_id].statistics_interva >>16 & 0xFF;
		p_zte_uni_stats_cfg->stats_interval[1]=statistics_port[port_id].statistics_interva >>8 & 0xFF;
		p_zte_uni_stats_cfg->stats_interval[2]=statistics_port[port_id].statistics_interva & 0xFF;
		
		p_zte_uni_stats_cfg->stats_duration=statistics_port[port_id].statistics_duration_time;

	}

	return NO_ERROR;
}


int haddleStatisticsCount(u8_t port , statistics_counter_t  * statistics_counter)
{
	int ret;
	oam_zte_onu_ether_stats_t zte_port_eth_stats;
	
	memset(&zte_port_eth_stats, 0 , sizeof(oam_zte_onu_ether_stats_t));
	
	ret = stub_zte_get_uni_eth_stats(port, &zte_port_eth_stats);
	
	if (ret != NO_ERROR)
	{
		 OP_DEBUG( DEBUG_LEVEL_DEBUGGING,"OAM ZTE: stub_zte_get_uni_eth_stats failed" );
		return ret;
	}
	statistics_counter->staifOutOctets = zte_port_eth_stats.ifOutOctets;

	statistics_counter->staifOutUcastPkts =  zte_port_eth_stats.ifOutUcastPkts;

	statistics_counter->staifOutNUcastPkts =  zte_port_eth_stats.ifOutNUcastPkts;


	statistics_counter->staifOutErrors = zte_port_eth_stats.ifOutErrors;

	statistics_counter->staifInOctets= zte_port_eth_stats.ifInOctets;

	statistics_counter->staifInUcastPkts =zte_port_eth_stats.ifInUcastPkts;

	statistics_counter->staifInNUcastPkts = zte_port_eth_stats.ifInNUcastPkts;


	statistics_counter->staifInErrors = zte_port_eth_stats.ifInErrors;
	
	return OK;

}


void *haddleStatisticsCollectPort(void * pthparam)
{
	int port = *((int *)pthparam);
	u8_t   action=statistics_port[port].statistics_action;
	UINT32 durtime=statistics_port[port].statistics_duration_time*1000;
	UINT32 interval=statistics_port[port].statistics_interva;
	statistics_counter_t *pCount[2];
	unsigned int i = 0;

	memset(&statistics_counter1[port],0,sizeof(statistics_counter_t));
	memset(&statistics_counter2[port],0,sizeof(statistics_counter_t));
	pCount[0] = &statistics_counter1[port];
	pCount[1] = &statistics_counter2[port];
	
	while(1)
	{
		action=statistics_port[port].statistics_action;
		if(action==0x01)
		{
			durtime=statistics_port[port].statistics_duration_time*1000;
			 interval=statistics_port[port].statistics_interva;
			 
			while(statistics_port[port].statistics_action==0x01 && durtime>=interval)
			{	
				haddleStatisticsCount(port, pCount[i]);
				usleep(interval*1000);
							
				durtime=statistics_port[port].statistics_duration_time*1000;
				interval=statistics_port[port].statistics_interva;		

				durtime-=interval;
				if(statistics_port[port].statistics_action==0x01)
				{
					statistics_port[port].statistics_duration_time=durtime/1000;
				}
				if(durtime< interval)
				{
					statistics_port[port].statistics_action=0;
					statistics_port[port].statistics_interva=0;
					statistics_port[port].statistics_duration_time=0;
				}
				
				i = (++i)%2;

			}	
		}
		vosUSleep(100*1000);	
	}
	vosThreadExit(0);
    return NULL;
}

u32_t stub_zte_set_uni_stats_cfg(u8_t port_id, oam_zte_uni_stats_cfg_t *p_zte_uni_stats_cfg)
{
	u8_t action=0;
	int ret=NO_ERROR;
	int interval=0;
	UINT32 durtime = 0;
	
	action= p_zte_uni_stats_cfg->action;
	interval=((p_zte_uni_stats_cfg->stats_interval[0]<<16)|(p_zte_uni_stats_cfg->stats_interval[1]<<8)|(p_zte_uni_stats_cfg->stats_interval[2]));
	durtime = p_zte_uni_stats_cfg->stats_duration;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, " the set action=0x%x interval = 0x%x durtime = 0x%x\n",action,interval,durtime);
	
	if(interval >= 1000)/*interval 设置大于1000mS 太快了循环太快了导致CPU运转不过来*/
	{
		
		if(action==0x00)/*disable or stop*/
		{
			statistics_port[port_id].statistics_action=0;		
		}
		if(action==0x01) /*start*/
		{
			statistics_port[port_id].statistics_action=p_zte_uni_stats_cfg->action;
			
			statistics_port[port_id].statistics_interva= ((p_zte_uni_stats_cfg->stats_interval[0]<<16)|(p_zte_uni_stats_cfg->stats_interval[1]<<8)|(p_zte_uni_stats_cfg->stats_interval[2]));
			
			statistics_port[port_id].statistics_duration_time= p_zte_uni_stats_cfg->stats_duration;
		
			if(portStatisticsThreadId[port_id]==NULL)
			{	
				statistics_port_num[port_id] = port_id;
				portStatisticsThreadId[port_id] = vosThreadCreate("tPortStats", 
                    OP_VOS_THREAD_STKSZ, 90, (void *)haddleStatisticsCollectPort, 
                    (void *)&statistics_port_num[port_id]);
				if(portStatisticsThreadId[port_id] == NULL)
				{
				    OP_DEBUG(DEBUG_LEVEL_WARNING, 
                        " create haddleStatisticsCollectPort thread failed\n");
                    ret = ERROR;
				}				
			}
		}	  

	}
	else
	{
		ret = ERROR;		
		
	}

	return ret;	
	
}


void eopl_zte_get_uni_stats_cfg(void)//port based
{
	u8_t	ret;
	u8_t    ucPortS;
	u8_t    ucPortE;
	u8_t    *p_out;
	oam_instance_tlv_t_2_0	    *p_inst;
	oam_zte_uni_stats_cfg_t  *p_zte_uni_stats_cfg;
	oam_zte_uni_stats_cfg_t   zte_uni_stats_cfg;

	ret = NO_ERROR;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_out;
		*p_inst = port_instance_2_0;
		p_out += sizeof(oam_instance_tlv_t_2_0);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

		p_zte_uni_stats_cfg = (oam_zte_uni_stats_cfg_t *)p_out;
		p_zte_uni_stats_cfg->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_zte_uni_stats_cfg->leaf = OAM_ZTE_EXT_LEAF_UNI_STATS_CFG;

		ret = stub_zte_get_uni_stats_cfg(ucPortS-1, &zte_uni_stats_cfg);
		if(ret != NO_ERROR)
		{
			return;
		}
		p_zte_uni_stats_cfg->width = zte_uni_stats_cfg.width;
		p_zte_uni_stats_cfg->action = zte_uni_stats_cfg.action;

		vosMemCpy(&p_zte_uni_stats_cfg->stats_interval[0], &zte_uni_stats_cfg.stats_interval[0],3);

		p_zte_uni_stats_cfg->stats_duration = zte_uni_stats_cfg.stats_duration;

		g_usOutDataIdx += p_zte_uni_stats_cfg->width + 4;
	}

	return;
}


void eopl_zte_set_uni_stats_cfg(u8_t *p_in)//port based
{
	u8_t ret, ucPortS, ucPortE, *p_out;
	oam_set_response_t	   *p_rsp;
	oam_instance_tlv_t_2_0	   *p_inst;
	oam_zte_uni_stats_cfg_t *p_zte_uni_stats_cfg;

	ret=OPL_OK;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t_2_0 *)p_out;
	*p_inst =  port_instance_2_0;
	p_out += sizeof(oam_instance_tlv_t_2_0);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_UNI_STATS_CFG;

	p_zte_uni_stats_cfg = (oam_zte_uni_stats_cfg_t *)p_in;
	for(; ucPortS<=ucPortE; ucPortS++)
	{
		ret = stub_zte_set_uni_stats_cfg(ucPortS-1, p_zte_uni_stats_cfg);
		if(ret != NO_ERROR)
			goto send_rsp;
	}

send_rsp:

	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	p_out += sizeof(oam_set_response_t);
	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

/* OAM_ZTE_EXT_LEAF_UNI_STATS_RESET */
u32_t stub_zte_set_uni_stats_reset(u8_t port_id, oam_zte_uni_stats_reset_t *p_zte_uni_stats_cfg)
{
	int ret=NO_ERROR;
	if(p_zte_uni_stats_cfg->reset==0x00)/*reset */
	{
		//to do 清除SWITCH的统计信息;
		//stats_clearArx8306(port_id);

		if(ret==NO_ERROR)
		{
			statistics_port[port_id].statistics_action=0;
			statistics_port[port_id].statistics_interva=1000;
			statistics_port[port_id].statistics_duration_time=900;		
		}
	}
	return ret;
}

void eopl_zte_set_uni_stats_reset(u8_t *p_in)//port based
{
	u8_t ret, ucPortS, ucPortE, *p_out;
	oam_set_response_t	     *p_rsp;
	oam_instance_tlv_t_2_0	     *p_inst;
	oam_zte_uni_stats_reset_t *p_zte_uni_stats_reset;

	ret=OPL_OK;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t_2_0 *)p_out;
	*p_inst =  port_instance_2_0;
	p_out += sizeof(oam_instance_tlv_t_2_0);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_UNI_STATS_RESET;

	p_zte_uni_stats_reset = (oam_zte_uni_stats_reset_t *)p_in;
	for(; ucPortS<=ucPortE; ucPortS++)
	{
		ret = stub_zte_set_uni_stats_reset(ucPortS-1, p_zte_uni_stats_reset);
		if(ret != NO_ERROR)
			goto send_rsp;
	}

send_rsp:

	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	p_out += sizeof(oam_set_response_t);
	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

/* OAM_ZTE_EXT_LEAF_UNI_STATS_QUERY */
u8_t stub_zte_get_uni_stats_query(u8_t port_id,  oam_zte_uni_stats_query_t *p_zte_uni_stats_query)
{
	/*tw should do*/
	if(statistics_port[port_id].statistics_action==1 && statistics_port[port_id].statistics_duration_time!=0)
	{
		p_zte_uni_stats_query->collect_indication = 0x01;
	}
	else
	{
		p_zte_uni_stats_query->collect_indication = 0x00;
	}

	p_zte_uni_stats_query->ifOutOctets  = llabs(statistics_counter2[port_id].staifOutOctets - statistics_counter1[port_id].staifOutOctets);

	p_zte_uni_stats_query->ifOutUcastPkts =llabs(statistics_counter2[port_id].staifOutUcastPkts - statistics_counter1[port_id].staifOutUcastPkts);

	p_zte_uni_stats_query->ifOutNUcastPkts=llabs(statistics_counter2[port_id].staifOutNUcastPkts - statistics_counter1[port_id].staifOutNUcastPkts);
	
	p_zte_uni_stats_query->ifOutErrors =llabs( statistics_counter2[port_id].staifOutErrors-statistics_counter1[port_id].staifOutErrors); 

	p_zte_uni_stats_query->ifInOctets = llabs(statistics_counter2[port_id].staifInOctets-statistics_counter1[port_id].staifInOctets);

	p_zte_uni_stats_query->ifInUcastPkts = llabs(statistics_counter2[port_id].staifInUcastPkts-statistics_counter1[port_id].staifInUcastPkts);

	p_zte_uni_stats_query->ifInNUcastPkts= llabs(statistics_counter2[port_id].staifInNUcastPkts-statistics_counter1[port_id].staifInNUcastPkts);

	p_zte_uni_stats_query->ifInErrors =llabs(statistics_counter2[port_id].staifInErrors-statistics_counter1[port_id].staifInErrors);
	

	return NO_ERROR;
}

void eopl_zte_get_uni_stats_query(void)//port based
{
	u8_t	ret;
	u8_t    ucPortS;
	u8_t    ucPortE;
	u8_t    *p_out;
	oam_instance_tlv_t_2_0	      *p_inst;
	oam_zte_uni_stats_query_t  *p_zte_uni_stats_query;
	oam_zte_uni_stats_query_t   zte_uni_stats_query;

	ret = NO_ERROR;
	
	memset(&zte_uni_stats_query,0,sizeof(oam_zte_uni_stats_query_t));
	
	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_out;
		*p_inst = port_instance_2_0;
		p_out += sizeof(oam_instance_tlv_t_2_0);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

		p_zte_uni_stats_query = (oam_zte_uni_stats_query_t*)p_out;
		p_zte_uni_stats_query->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_zte_uni_stats_query->leaf = OAM_ZTE_EXT_LEAF_UNI_STATS_QUERY;
		p_zte_uni_stats_query->width = 81;

		ret = stub_zte_get_uni_stats_query(ucPortS-1, &zte_uni_stats_query);
		if(ret != OPL_OK)
		{
			return;
		}
		p_zte_uni_stats_query->collect_indication = zte_uni_stats_query.collect_indication;
		p_zte_uni_stats_query->ifInOctets = zte_uni_stats_query.ifInOctets;
		p_zte_uni_stats_query->ifInUcastPkts = zte_uni_stats_query.ifInUcastPkts;
		p_zte_uni_stats_query->ifInNUcastPkts = zte_uni_stats_query.ifInNUcastPkts;
		p_zte_uni_stats_query->ifInDiscards = zte_uni_stats_query.ifInDiscards;
		p_zte_uni_stats_query->ifInErrors = zte_uni_stats_query.ifInErrors;
		p_zte_uni_stats_query->ifOutOctets = zte_uni_stats_query.ifOutOctets;
		p_zte_uni_stats_query->ifOutUcastPkts = zte_uni_stats_query.ifOutUcastPkts;
		p_zte_uni_stats_query->ifOutNUcastPkts = zte_uni_stats_query.ifOutNUcastPkts;
		p_zte_uni_stats_query->ifOutDiscards = zte_uni_stats_query.ifOutDiscards;
		p_zte_uni_stats_query->ifOutErrors = zte_uni_stats_query.ifOutErrors;

		g_usOutDataIdx += p_zte_uni_stats_query->width + 4;
	}

	return;
}


/*光模块tx控制*/
void *thread_config_laser_tx(void *arg)
{
	while(1)
	{
		vosUSleep(100*1000);
		if(gLaserConfigFlag == 0)
		{
			vosUSleep(100*1000);

			if(gTxLaserCloseDuration != 0)
			{
				//printf("duration %lu\n", gTxLaserCloseDuration);

				vosSleep(gTxLaserCloseDuration);
				odmPonTxPowerDown(1);
			}

			gLaserConfigFlag = 1;
		}
	}
	vosThreadExit(0);
    return NULL;
}


int stub_zte_set_tx_laser_conrol(oam_zte_tx_laser_control_t *p_zte_tx_laser_control)
{
	static VOS_THREAD_t thread_id = NULL;

	int ret=NO_ERROR;
	UINT8 lase_state =0 ;
	u8_t mac[6] = {0};
	if(p_zte_tx_laser_control->matchmac_en == NEED_MATCH)
	{
		ret = odmPonMacIdGet(mac);
		if(ret != OK)
		{
			printf("get the mac fail\n");
			return ret;
		}
		if(vosMemCmp(mac,p_zte_tx_laser_control->pon_mac_value,sizeof(mac))!=0)
		{
			printf("the match mac fail\n");
			ret = ERROR;
			return ret;
		}
	}
	switch(p_zte_tx_laser_control->action)
	{
		case OPEN_TX_ACTION:
			lase_state = 0;
			break;
		case CLOSE_TX_ACTION:
			lase_state = 1;
			break;
		case CLOSE_MODULE_ACTION:
			ret = -5;
			return ret;
		default:
			break;
	}
	/*若打开立即执行，关闭则回包后执行*/
	if(lase_state == 0)
	{
		odmPonTxPowerDown(lase_state);
	}
	else
	{
		gLaserConfigFlag = 0;
	}
	
	if(p_zte_tx_laser_control->action!= OPEN_TX_ACTION)
	{
		gTxLaserCloseDuration = p_zte_tx_laser_control->durtime;
		
		if(thread_id == NULL)
		{
            thread_id = vosThreadCreate("tLaserCtrl", OP_VOS_THREAD_STKSZ, 90, 
                (void *)thread_config_laser_tx, NULL);
			if(thread_id == NULL)
			{
				OP_DEBUG(DEBUG_LEVEL_WARNING, "Create the task thread_config_laser_tx failed");
				ret = ERROR;
			}
		}
	}
	return ret;
}

void eopl_zte_set_tx_laser_conrol(u8_t *p_in)//port based
{
	int ret, ucPortS, ucPortE;
	u8_t    *p_out;
	oam_set_response_t	     *p_rsp;
	oam_zte_tx_laser_control_t *p_zte_tx_laser_control;

	ret=OPL_OK;
	
	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_TX_LASER_CONTROL;

	p_zte_tx_laser_control = (oam_zte_tx_laser_control_t *)p_in;

	ret = stub_zte_set_tx_laser_conrol(p_zte_tx_laser_control);


	if(ret == NO_ERROR)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else if(ret == -5)
	{
		p_rsp->indication = OAM_SET_BAD_PARAMETERS;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}
	p_out += sizeof(oam_set_response_t);
	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

typedef struct onu_stats_s{
    pon_statistics_t	pon_statistics;
    port_statistics_t	port_statistics[ODM_NUM_OF_PORTS];
}onu_stats_t;

OPL_STATUS odmOnuRealStatisticGet(onu_stats_t * stats)
{
	pon_statistics_t * ponStats;
	UINT8 Index;

	odmPonStatsGet(&stats->pon_statistics);

	/* get port statistics information */
	for (Index = 0; Index < ODM_NUM_OF_PORTS; Index++)
	{
		odmPonSinglePortStatsGet(Index, &stats->port_statistics[Index]);
	}
	
	return OK;
}

/*OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_1*/

u32_t stub_zte_get_pon_port_stats_1(u8_t port_id,  oam_zte_pon_stats_statics_1_t *p_zte_pon_stats_query)
{
	onu_stats_t stats;
	vosMemSet(&stats,0,sizeof(onu_stats_t));
	
	odmOnuRealStatisticGet(&stats);   
	
	p_zte_pon_stats_query->FramesTransmittedOK = stats.pon_statistics.FrameTx;
	p_zte_pon_stats_query ->OctetsTransmittedOK = stats.pon_statistics.OctetsTx;
	p_zte_pon_stats_query->MulticastFramesXmittedOK= stats.pon_statistics.OctetsTxTopRate;
	p_zte_pon_stats_query->BroadcastFramesXmittedOK = stats.pon_statistics.OctetsTxRate;
	p_zte_pon_stats_query->FramesReceivedOK = stats.pon_statistics.FrameRx;
	p_zte_pon_stats_query->OctetsReceivedOK = stats.pon_statistics.OctetsRx;
	p_zte_pon_stats_query->MulticastFramesReceivedOK = stats.pon_statistics.OctetsRxTopRate;
	p_zte_pon_stats_query->BroadcastFramesReceivedOK = stats.pon_statistics.OctetsRxRate;
	
	return NO_ERROR;
}

void eopl_zte_get_pon_statics_1(void)//port based
{
	u32_t                    ret;
	u8_t                     ucPortS;
	u8_t                     ucPortE;
	u8_t                     *p_out;
	u8_t					 oamRspCode;
	oam_rsp_indication_t     *p_get_rsp;
	oam_instance_tlv_t_2_0	     *p_inst;
	oam_zte_pon_stats_statics_1_t *p_zte_pon_eth_stats;
	oam_zte_pon_stats_statics_1_t  zte_pon_eth_stats;

	ret = NO_ERROR;

	vosMemSet(&zte_pon_eth_stats,0,sizeof(oam_zte_pon_stats_statics_1_t));
	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_out;
		*p_inst = port_instance_2_0;
		p_out += sizeof(oam_instance_tlv_t_2_0);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

		ret = stub_zte_get_pon_port_stats_1(ucPortS-1, &zte_pon_eth_stats);
		
		if(ret != NO_ERROR)
		{
			oamRspCode = OAM_GET_NO_RESOURCE;		
			p_get_rsp = (oam_rsp_indication_t *)p_out;
			p_get_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_get_rsp->leaf = OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_1;
			p_get_rsp->var_indication = oamRspCode;
			g_usOutDataIdx += 4;
			return ;
		}
		
		p_zte_pon_eth_stats = (oam_zte_pon_stats_statics_1_t *)p_out;
		p_zte_pon_eth_stats->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_zte_pon_eth_stats->leaf = OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_1;
		p_zte_pon_eth_stats->width = 112;


		p_zte_pon_eth_stats->FramesTransmittedOK = zte_pon_eth_stats.FramesTransmittedOK;
		p_zte_pon_eth_stats ->OctetsTransmittedOK = zte_pon_eth_stats.OctetsTransmittedOK;
		p_zte_pon_eth_stats->MulticastFramesXmittedOK=zte_pon_eth_stats.MulticastFramesXmittedOK;
		p_zte_pon_eth_stats->BroadcastFramesXmittedOK = zte_pon_eth_stats.BroadcastFramesXmittedOK;
		p_zte_pon_eth_stats->FramesReceivedOK = zte_pon_eth_stats.FramesReceivedOK;
		p_zte_pon_eth_stats->OctetsReceivedOK =zte_pon_eth_stats.OctetsReceivedOK;
		p_zte_pon_eth_stats->MulticastFramesReceivedOK =zte_pon_eth_stats.MulticastFramesReceivedOK;
		p_zte_pon_eth_stats->BroadcastFramesReceivedOK = zte_pon_eth_stats.BroadcastFramesReceivedOK;

		g_usOutDataIdx += p_zte_pon_eth_stats->width + 4;
	}

	return;
}

/*OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_2*/

u32_t stub_zte_get_pon_port_stats_2(u8_t port_id,  oam_zte_pon_stats_statics_2_t *p_zte_pon_stats_query)
{
	u64_t  temp=0;

	p_zte_pon_stats_query->MPCPRxGate = ((((temp>>32) & g_astCntMpcp[0].stAdded.uiHigh) <<32 )|g_astCntMpcp[0].stAdded.uiLow);
	temp=0;
	p_zte_pon_stats_query ->MPCPRxRegister =((((temp>>32) & g_astCntMpcp[1].stAdded.uiHigh) <<32 )|g_astCntMpcp[1].stAdded.uiLow);
	temp=0;

	p_zte_pon_stats_query->MPCPTxRegister= ((((temp>>32) & g_astCntMpcp[12].stAdded.uiHigh) <<32 )|g_astCntMpcp[12].stAdded.uiLow);
	temp=0;

	p_zte_pon_stats_query->FECCorrectedBlocks =((((temp>>32) & g_astCntPon[39].stAdded.uiHigh) <<32 )|g_astCntPon[39].stAdded.uiLow) ;
	temp=0;

	p_zte_pon_stats_query->FECUncorrectableBlocks = ((((temp>>32) & g_astCntPon[40].stAdded.uiHigh) <<32 )|g_astCntPon[40].stAdded.uiLow);
        /*tw should do continue........*/
	
	return NO_ERROR;
}

void eopl_zte_get_pon_statics_2(void)//port based
{
	u32_t                    ret;
	u8_t                     ucPortS;
	u8_t                     ucPortE;
	u8_t                     *p_out;
	u8_t					 oamRspCode;
	oam_rsp_indication_t     *p_get_rsp;
	oam_instance_tlv_t_2_0	     *p_inst;
	oam_zte_pon_stats_statics_2_t *p_zte_pon_eth_stats;
	oam_zte_pon_stats_statics_2_t  zte_pon_eth_stats;

	ret = NO_ERROR;

	vosMemSet(&zte_pon_eth_stats,0,sizeof(oam_zte_pon_stats_statics_2_t));
	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_out;
		*p_inst = port_instance_2_0;
		p_out += sizeof(oam_instance_tlv_t_2_0);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

		ret = stub_zte_get_pon_port_stats_2(ucPortS-1, &zte_pon_eth_stats);
		
		if(ret != NO_ERROR)
		{
			oamRspCode = OAM_GET_NO_RESOURCE;		
			p_get_rsp = (oam_rsp_indication_t *)p_out;
			p_get_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
			p_get_rsp->leaf = OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_2;
			p_get_rsp->var_indication = oamRspCode;
			g_usOutDataIdx += 4;
			return ;
		}
		
		p_zte_pon_eth_stats = (oam_zte_pon_stats_statics_2_t *)p_out;
		p_zte_pon_eth_stats->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_zte_pon_eth_stats->leaf = OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_2;
		p_zte_pon_eth_stats->width = 120;


		p_zte_pon_eth_stats->MPCPRxGate = zte_pon_eth_stats.MPCPRxGate;
		p_zte_pon_eth_stats ->MPCPRxRegister = zte_pon_eth_stats.MPCPRxRegister;

		p_zte_pon_eth_stats->MPCPTxRegister=zte_pon_eth_stats.MPCPTxRegister;

		p_zte_pon_eth_stats->FECCorrectedBlocks =zte_pon_eth_stats.FECCorrectedBlocks;

		p_zte_pon_eth_stats->FECUncorrectableBlocks=zte_pon_eth_stats.FECUncorrectableBlocks;

	g_usOutDataIdx += p_zte_pon_eth_stats->width + 4;
	}

	return;
}

/* OAM_ZTE_EXT_LEAF_HG_REPORT */
u32_t  eopl_zte_hg_report(u8_t port_id, u16_t msg_id, u8_t *msg_info, u8_t msg_len)
{
	u8_t	             *p_out;
	oam_instance_tlv_t_2_0   *p_inst;
	oam_zte_hg_report_t   *p_zte_msg_info;
	u32_t                ret;

	if(0 == g_usAlarmOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &alarm_out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usAlarmOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &alarm_out_data[g_usAlarmOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t_2_0 *)p_out;
	p_out += sizeof(oam_instance_tlv_t_2_0);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

	p_inst->branch = 0x36;
	p_inst->leaf = 0x0001;
	p_inst->width = 0x01;
	p_inst->value = port_id;

	p_zte_msg_info = (oam_zte_hg_report_t *)p_out;
	p_zte_msg_info->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_zte_msg_info->leaf = OAM_ZTE_EXT_LEAF_HG_REPORT;
	p_zte_msg_info->width = 2 + msg_len;
	p_zte_msg_info->msg_id = msg_id;
	
	vosMemCpy(&p_zte_msg_info->msg_info[0], msg_info, msg_len);

	g_usAlarmOutDataIdx += p_zte_msg_info->width + 4;

	OamFrameSend(alarm_out_data, g_usAlarmOutDataIdx);
	g_usAlarmOutDataIdx = 0;

	return ret;
}



/* OAM_ZTE_EXT_LEAF_HG_MAC_INFO 0x6001*/
static u8_t stub_zte_hg_mac_num;
static oam_zte_hg_mac_entry_t  stub_zte_hg_mac_info[32];

u32_t stub_zte_hg_mac_entry_num_get(u8_t *p_entry_num)
{
	*p_entry_num = stub_zte_hg_mac_num;

	return NO_ERROR;
}

u32_t stub_zte_hg_mac_entry_get(u8_t entry_id, oam_zte_hg_mac_entry_t *p_hg_mac_entry)
{
	vosMemCpy(p_hg_mac_entry, &stub_zte_hg_mac_info[entry_id], sizeof(oam_zte_hg_mac_entry_t));

	return NO_ERROR;
}

u32_t stub_zte_hg_mac_entry_add(u8_t entry_id, oam_zte_hg_mac_entry_t *p_hg_mac_entry)
{
	u32_t ret;

	vosMemCpy(&stub_zte_hg_mac_info[entry_id], p_hg_mac_entry, sizeof(oam_zte_hg_mac_entry_t));

	stub_zte_hg_mac_num++;

	return NO_ERROR;
}

u32_t stub_zte_hg_mac_entry_del(u8_t entry_id)
{

	u8_t  i;

	for(i = entry_id+1; i < stub_zte_hg_mac_num; i++)
	{
		
vosMemCpy(&stub_zte_hg_mac_info[i-1],
			       &stub_zte_hg_mac_info[i],
			       sizeof(oam_zte_hg_mac_entry_t));
	}

	vosMemSet(&stub_zte_hg_mac_info[i],
		      0,
		      sizeof(oam_zte_hg_mac_entry_t));


	stub_zte_hg_mac_num--;

	return NO_ERROR;
}

u32_t stub_zte_hg_mac_add(oam_zte_hg_mac_entry_t *p_hg_mac_entry)
{
	u32_t ret;
	u8_t  num_of_entries;
	u8_t  new_entry_id;
	u8_t i = 0;
	oam_zte_hg_mac_entry_t hg_mac_entry;

	ret = stub_zte_hg_mac_entry_num_get(&num_of_entries);
	if(ret != NO_ERROR)
		return -1;

	if(num_of_entries == 32)
	{
		return -1;
	}

	new_entry_id = num_of_entries;

	for (i = 0; i < num_of_entries; i++)
	{
		ret = stub_zte_hg_mac_entry_get(i, &hg_mac_entry);
		if(ret != NO_ERROR)
			return -1;
		
		if(vosMemCmp(&hg_mac_entry, p_hg_mac_entry, sizeof(oam_zte_hg_mac_entry_t)) == 0)
			break;
	}

	if(i == num_of_entries)
		ret = stub_zte_hg_mac_entry_add(new_entry_id, p_hg_mac_entry);

	return ret;
}

u32_t stub_zte_hg_mac_del(oam_zte_hg_mac_entry_t *p_hg_mac_entry)
{
	u8_t  num_of_entries;
	u8_t  hg_mac_id;
	u32_t ret;
	u8_t  i;
	oam_zte_hg_mac_entry_t hg_mac_entry;

	ret = stub_zte_hg_mac_entry_num_get(&num_of_entries);
	if(ret != NO_ERROR)
		return -1;

	if(num_of_entries == 0)
		return -1;

	hg_mac_id = num_of_entries;

	for(i = 0; i < num_of_entries; i++)
	{
		ret = stub_zte_hg_mac_entry_get(i, &hg_mac_entry);

		if(vosMemCmp(p_hg_mac_entry, &hg_mac_entry, sizeof(oam_zte_hg_mac_entry_t)) == 0)
		{
			ret = stub_zte_hg_mac_entry_del(i);
		}
	}

	return NO_ERROR;
}

u32_t stub_zte_hg_mac_clear(void)
{
	u32_t ret;
	u8_t  i;

	for(i = 0; i < 32; i++)
	{
		vosMemSet(&stub_zte_hg_mac_info[i],
			      0,
			      sizeof(oam_zte_hg_mac_entry_t));
	}

	return NO_ERROR;
}

void eopl_zte_get_hg_mac_info(void)//onu based
{
	u32_t 	ret;
	u8_t    *p_out;
	oam_zte_hg_mac_info_t   *p_zte_hg_mac_info;
	oam_zte_hg_mac_info_t   zte_hg_mac_info;
	oam_zte_hg_mac_entry_t  *p_zte_hg_mac_entry;
	oam_zte_hg_mac_entry_t  zte_hg_mac_entry;
	u8_t    num_of_entries;
	u8_t    i;

	ret = NO_ERROR;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_zte_hg_mac_info = (oam_zte_hg_mac_info_t *)p_out;
	p_out += sizeof(oam_zte_hg_mac_info_t);

	p_zte_hg_mac_info->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_zte_hg_mac_info->leaf = OAM_ZTE_EXT_LEAF_HG_MAC_INFO;

	ret = stub_zte_hg_mac_entry_num_get(&num_of_entries);
	if(ret != OPL_OK)
	{
		g_usOutDataIdx = 0;
		return;
	}

	p_zte_hg_mac_info->action = 0x03;
	p_zte_hg_mac_info->num_of_entries = num_of_entries;
	p_zte_hg_mac_info->width = 2 + 6 * num_of_entries;

	for(i = 0; i < num_of_entries; i++)
	{
		p_zte_hg_mac_entry = (oam_zte_hg_mac_entry_t *)p_out;
		p_out += sizeof(oam_zte_hg_mac_entry_t);

		stub_zte_hg_mac_entry_get(i, p_zte_hg_mac_entry);
	}

	g_usOutDataIdx += p_zte_hg_mac_info->width + 4;

	return;

}

void eopl_zte_set_hg_mac_info(u8_t *p_in)//port based
{
	u8_t                       *p_out;
	oam_set_response_t	       *p_rsp;
	oam_instance_tlv_t_2_0	       *p_inst;
	oam_zte_hg_mac_info_t       *p_zte_hg_mac_info;
	oam_zte_hg_mac_entry_t      *p_zte_hg_mac_entry;
	u8_t                       num_of_entries;
	u8_t                       i;
	u32_t                      ret;

	ret=OPL_OK;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_HG_MAC_INFO;

	p_zte_hg_mac_info = (oam_zte_hg_mac_info_t *)p_in;
	p_in += sizeof(oam_zte_hg_mac_info_t);

	num_of_entries = p_zte_hg_mac_info->num_of_entries;

	p_zte_hg_mac_entry = (oam_zte_hg_mac_entry_t *)p_in;

	if(p_zte_hg_mac_info->action == 0x00)//delete
	{
		for(i = 0; i < num_of_entries; i++)
		{
			ret = stub_zte_hg_mac_del(p_zte_hg_mac_entry);
			if(ret != NO_ERROR)
				goto send_rsp;

			p_zte_hg_mac_entry++;
		}
	}
	else if(p_zte_hg_mac_info->action == 0x01)//add
	{
		for(i = 0; i < num_of_entries; i++)
		{
			ret = stub_zte_hg_mac_add(p_zte_hg_mac_entry);
			if(ret != NO_ERROR)
				goto send_rsp;

			p_zte_hg_mac_entry++;
		}
	}
	else if(p_zte_hg_mac_info->action == 0x02)//clear
	{
		ret = stub_zte_hg_mac_clear();
		if(ret != NO_ERROR)
			goto send_rsp;
	}

send_rsp:
	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	p_out += sizeof(oam_set_response_t);
	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

/* OAM_ZTE_EXT_LEAF_HG_MGMT_VLAN 0x6002*/
static oam_zte_hg_mgmt_vlan_t stub_zte_hg_mgmt_vlan;

u32_t stub_zte_get_hg_mgmt_vlan(u16_t *p_mgmt_vlan)
{
	*p_mgmt_vlan = stub_zte_hg_mgmt_vlan.hg_mgmt_vlan;

	return NO_ERROR;
}

u32_t stub_zte_set_hg_mgmt_vlan(u16_t mgmt_vlan)
{
	stub_zte_hg_mgmt_vlan.hg_mgmt_vlan = mgmt_vlan;

	return NO_ERROR;
}

void eopl_zte_get_hg_mgmt_vlan(void)//onu based
{
	u8_t	                *p_out;
	oam_zte_hg_mgmt_vlan_t	*p_zte_hg_mgmt_vlan;
	u32_t                   ret;
	u16_t                   hg_mgmt_vlan;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_zte_hg_mgmt_vlan  = (oam_zte_hg_mgmt_vlan_t *)p_out;
	p_zte_hg_mgmt_vlan->branch = OAM_ZTE_EXT_BRANCH_ATTR;
	p_zte_hg_mgmt_vlan->leaf = OAM_ZTE_EXT_LEAF_HG_MGMT_VLAN;
	p_zte_hg_mgmt_vlan->width = 2;

	ret = stub_zte_get_hg_mgmt_vlan(&hg_mgmt_vlan);

	if(ret != NO_ERROR)
		return;

	p_zte_hg_mgmt_vlan->hg_mgmt_vlan = hg_mgmt_vlan;

	g_usOutDataIdx += p_zte_hg_mgmt_vlan->width + 4;

}

void eopl_zte_set_hg_mgmt_vlan(u8_t *p_in)
{
	u32_t                 ret;
	u8_t                  *p_out;
	oam_set_response_t    *p_rsp;
	oam_zte_hg_mgmt_vlan_t *p_zte_mgmt_vlan;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_ZTE_EXT_BRANCH_ATTR;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_HG_MGMT_VLAN;

	p_zte_mgmt_vlan = (oam_zte_hg_mgmt_vlan_t *)p_in;

	ret = stub_zte_set_hg_mgmt_vlan(p_zte_mgmt_vlan->hg_mgmt_vlan);

	if(ret == NO_ERROR)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += 4;
}

u32_t stub_zte_get_hg_access_info(u8_t port_id, oam_zte_hg_access_info_t *p_info)
{
	vosMemCpy(&p_info->hg_mac[0], &stub_zte_hg_access_info[port_id].hg_mac[0], 6);

	p_info->hg_report_state = stub_zte_hg_access_info[port_id].hg_report_state;

	return NO_ERROR;
}

u32_t stub_zte_set_hg_access_info(u8_t port_id, oam_zte_hg_access_info_t *p_info)
{

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Set hg access port  %d, mac "MAC_ASC_F", state %d\n" ,
		port_id, MACPRINT(p_info->hg_mac), p_info->hg_report_state);
	
	vosMemCpy(&stub_zte_hg_access_info[port_id].hg_mac[0], &p_info->hg_mac[0], 6);

	stub_zte_hg_access_info[port_id].hg_report_state = p_info->hg_report_state;

	return NO_ERROR;
}

void eopl_zte_get_hg_access_info(void)//port based
{
	u32_t	ret;
	u8_t    ucPortS;
	u8_t    ucPortE;
	u8_t    *p_out;
	oam_instance_tlv_t_2_0	    *p_inst;
	oam_zte_hg_access_info_t *p_zte_hg_access_info;

	ret = NO_ERROR;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	for(; ucPortS<=ucPortE; ucPortS++)
	{
		if(0 == g_usOutDataIdx)
		{
			eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
			p_out = &out_data[0];
			p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
			g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		}
		else
		{
			p_out = &out_data[g_usOutDataIdx];
		}

		port_instance_2_0.value = ucPortS;
		p_inst = (oam_instance_tlv_t_2_0 *)p_out;
		*p_inst = port_instance_2_0;
		p_out += sizeof(oam_instance_tlv_t_2_0);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

		p_zte_hg_access_info = (oam_zte_hg_access_info_t*)p_out;
		p_zte_hg_access_info->branch = OAM_BRANCH_EXTENDED_ATTRIB;
		p_zte_hg_access_info->leaf = OAM_ZTE_EXT_LEAF_HG_ACCESS_INFO;
		p_zte_hg_access_info->width = 0x07;

		ret = stub_zte_get_hg_access_info(ucPortS, p_zte_hg_access_info);
		if(ret != OPL_OK)
		{
			return;
		}

		g_usOutDataIdx += p_zte_hg_access_info->width + 4;
	}

	return;
}

void eopl_zte_set_hg_access_info(u8_t *p_in)//port based
{
	u8_t                    ucPortS;
	u8_t                    ucPortE;
	u8_t                    *p_out;
	oam_set_response_t	    *p_rsp;
	oam_instance_tlv_t_2_0	    *p_inst;
	oam_zte_hg_access_info_t *p_zte_hg_access_info;
	u32_t                   ret;

	ret=OPL_OK;

	if(0xFF == port_instance_2_0.value)
	{
		ucPortS = 1;
		ucPortE = ODM_NUM_OF_PORTS;
	}
	else
	{
		ucPortS = port_instance_2_0.value;
		ucPortE = port_instance_2_0.value;
	}

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t_2_0 *)p_out;
	*p_inst =  port_instance_2_0;
	p_out += sizeof(oam_instance_tlv_t_2_0);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_HG_ACCESS_INFO;

	p_zte_hg_access_info = (oam_zte_hg_access_info_t *)p_in;
	for(; ucPortS<=ucPortE; ucPortS++)
	{
		ret = stub_zte_set_hg_access_info(ucPortS, p_zte_hg_access_info);
		if(ret != NO_ERROR)
			goto send_rsp;
	}

send_rsp:

	if(ret == OPL_OK)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	p_out += sizeof(oam_set_response_t);
	g_usOutDataIdx += sizeof(oam_set_response_t);

	return;
}

char *simple_itoa(unsigned int i)
{
    /* 21 digits plus null terminator, good for 64-bit or smaller ints
     * for bigger ints, use a bigger buffer!
     *
     * 4294967295 is, incidentally, MAX_UINT (on 32bit systems at this time)
     * and is 10 bytes long
     */
    static char local[22];
    char *p = &local[21];
    *p-- = '\0';
    do {
        *p-- = '0' + i % 10;
        i /= 10;
    } while (i > 0);
    return p + 1;
}

u32_t stub_zte_hg_msg_set(u8_t *hgMac, u8_t portId, u8_t *reportMsg)
{
	char tmpBuf[20];
	u32_t pvid;
	int retVal;
	
	retVal = odmPortDefaultVlanGet(portId, &pvid);
	if(NO_ERROR != retVal)
	{
		return retVal;
	}
	
	vosSprintf(reportMsg, MAC_ASC_F, MACPRINT(hgMac));
	vosStrCat(reportMsg, "@");
	vosStrCat(reportMsg, simple_itoa(pvid));
	vosStrCat(reportMsg, "@");
	vosStrCat(reportMsg, simple_itoa(pvid + 1200));
	vosStrCat(reportMsg, "@");
	vosStrCat(reportMsg, simple_itoa(pvid+ 2400));

}


u32_t stub_zte_hg_config_save(int inNetState, int portId)
{
	int retVal;
		
	retVal = vosConfigIntegerSet(CFG_HG_CFG, CFG_HG_SECTION, CFG_HG_IN_NET_STATE,inNetState);
	if (0 != retVal)
	{
	    return ERR_CONFIG_SET_FAIL;
	}

	retVal = vosConfigIntegerSet(CFG_HG_CFG, CFG_HG_SECTION, CFG_HG_IN_NET_PORT,portId);
	if (0 != retVal)
	{
	    return ERR_CONFIG_SET_FAIL;
	}
	
	retVal = vosConfigFlushByModule(CFG_HG_CFG);
	if (0 != retVal)
	{
	    return ERR_CONFIG_SET_FAIL;
	}
}

u32_t stub_zte_hg_config_load(int *inNetState, int *portId)
{
	int retVal;
		
	*inNetState = vosConfigIntegerGet(CFG_HG_CFG, CFG_HG_SECTION, CFG_HG_IN_NET_STATE, CFG_HG_IN_NET_STATE_DEFAULT);

	*portId = vosConfigIntegerGet(CFG_HG_CFG, CFG_HG_SECTION, CFG_HG_IN_NET_STATE, CFG_HG_IN_NET_PORT_DEFAULT);
}

#define ONE_PORT_HG
/*add by lizhe 2009-12-25*/
void eopl_zte_hg_check()
{
	u32_t i,j;
	int portId;
	u32_t retVal = 0;
	u8_t entryNum = 0;
	u16_t vlanId;  
	char reportMsg[32];
	int inNetState;
	
	stub_zte_hg_config_load(&inNetState, &portId);
	if(portId != CFG_HG_IN_NET_PORT_DEFAULT)
		stub_zte_hg_access_info[portId].hg_report_state = inNetState;
		
	while(1)
	{
		vosSleep(1);
		#ifdef ONE_PORT_HG
		/*考虑在已有家庭网关入网后再继续查询比较消耗资源暂只实现发现一个网关*/
		for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
		{
			if(stub_zte_hg_access_info[portId].hg_report_state == 1)
				continue;
		}

		if(portId <= ODM_NUM_OF_PORTS)
		{
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "HG has entried net\n");
			continue;
		}
		#endif

		if(stub_zte_hg_mgmt_vlan.hg_mgmt_vlan == 0
			||stub_zte_hg_mac_num == 0)
		{
			continue;
		}

		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Has got manage vlan %d and hg mac, checking begin\n", stub_zte_hg_mgmt_vlan.hg_mgmt_vlan);
				
		for(portId = 1; portId <= ODM_NUM_OF_PORTS; portId++)
		{
			#ifndef ONE_PORT_HG
			if(stub_zte_hg_access_info[portId].hg_report_state == 1)
			{
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "A HG has entried net in port %d\n", portId);
				continue;
			}
			#endif

			retVal = odmPortMacFilterNumGet(portId, &entryNum);
		   	if(retVal != NO_ERROR)
		   	{
				OP_DEBUG(DEBUG_LEVEL_OFF,"Fail to odmPortMacFilterNumGet\n");
				continue;
		   	}

			for(i = 0; i < entryNum; i++)
			{
				for(j = 0; j < stub_zte_hg_mac_num; j++)
				{
					retVal = odmPortMacFilterEntryGet(portId, j, &vlanId, &stub_zte_hg_mac_info[i].mac_addr[j]);
					if(retVal != NO_ERROR)
					{
						OP_DEBUG(DEBUG_LEVEL_OFF, "Fail to odmPortMacFilterEntryGet\n");
						continue;
					}

					OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Get Port:%d, entry Num:%d, Vlan:%d\n",portId, entryNum, vlanId);

					if(vlanId == stub_zte_hg_mgmt_vlan.hg_mgmt_vlan)
					{
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING, 
							"HG has been found, Mac if "MAC_ASC_F"\n", 
							MACPRINT(stub_zte_hg_mac_info[j].mac_addr));

						retVal = stub_zte_hg_msg_set(&stub_zte_hg_mac_info[i].mac_addr[j], portId, reportMsg);
						if(retVal != NO_ERROR)
						{
							OP_DEBUG(DEBUG_LEVEL_OFF, "Fail to eopl_zte_hg_msg_set\n");
						}
						
						while(stub_zte_hg_access_info[portId].hg_report_state == 0)
						{
							/*暂不考虑多个同时互不影响的上报*/
							OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Report hg msg %s to respect to entry net. \n", reportMsg);
							eopl_zte_hg_report(portId, 0x1006, reportMsg, (vosStrLen(reportMsg)+1));//0x1006网关告警ID
							vosSleep(10);
						}

						OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "Entrying net is successfull!!! \n");
					}
				}
			}
		}
	}	
}

/* OAM_ZTE_EXT_LEAF_GBL_PARA 0x0063 */
static oam_zte_gbl_para_t stub_zte_gbl_para;

u32_t stub_zte_get_gbl_para(oam_zte_gbl_para_t *p_zte_gbl_para)
{
    ODM_SYS_IF_CONFIG_t stIpConfig;
    char my_username[32]={0};
    char my_passwd[32]={0};	
    int nLen=0;
    UINT32 retVal= NO_ERROR;

    memset(&stIpConfig,0,sizeof(ODM_SYS_IF_CONFIG_t));

    stIpConfig.ucIf = INTERFACE_WAN;
    /* current version OAM just supported Admin/Media */
    stIpConfig.ucIfMode = INTERFACE_MODE_SIGNAL;
    retVal = odmSysIfConfigGet(&stIpConfig);
    if (NO_ERROR != retVal)
    {
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "fail to get odmSysIfConfigGet!\n");
        return PARAMETER_INPUT_POINTER_NULL;
    }

	p_zte_gbl_para->voice_ip_releation = stIpConfig.ucIfShareMode;

	p_zte_gbl_para->voice_ip_mode =  stIpConfig.ucIpMode;

    if(p_zte_gbl_para->voice_ip_mode == 0)
    {
    	p_zte_gbl_para->iad_ip_address = stIpConfig.ulIpAddr;

    	p_zte_gbl_para->iad_net_mask = stIpConfig.ulIpMask;

    	p_zte_gbl_para->iad_default_gw = stIpConfig.ulGwAddr;

    	p_zte_gbl_para->primary_dns = stIpConfig.ulDnsAddr;
    }
	else if(p_zte_gbl_para->voice_ip_mode == 2)
	{
		p_zte_gbl_para->pppoe_mode = stIpConfig.ucPPPoEAuthType;

		/*pippo add for strip 0*/
		nLen = vosStrLen(stIpConfig.acPPPoEUser);
		if (nLen < 32)
		{
			vosMemSet(&p_zte_gbl_para->pppoe_username[0], 0, 32- nLen);
			vosMemCpy(&p_zte_gbl_para->pppoe_username[32 - nLen], stIpConfig.acPPPoEUser, nLen);
		}
		else
		{
			 vosMemCpy(&p_zte_gbl_para->pppoe_username[0], stIpConfig.acPPPoEUser, sizeof(p_zte_gbl_para->pppoe_username));

		}

		nLen = vosStrLen(stIpConfig.acPPPoEPass);
		if (nLen < 32)
		{
			vosMemSet(&p_zte_gbl_para->pppoe_password[0], 0, 32- nLen);
			vosMemCpy(&p_zte_gbl_para->pppoe_password[32 - nLen], stIpConfig.acPPPoEPass, nLen);
		}
		else
		{
			 vosMemCpy(&p_zte_gbl_para->pppoe_password[0], stIpConfig.acPPPoEPass, sizeof(p_zte_gbl_para->pppoe_password));

		}

	}
	p_zte_gbl_para->tagged_flag = stIpConfig.ucTagged;

    if(p_zte_gbl_para->tagged_flag == 1 || p_zte_gbl_para->tagged_flag == 2)
    {
    	p_zte_gbl_para->voice_data_cvlan = stIpConfig.usCVlan;

    	p_zte_gbl_para->voice_data_pri = stIpConfig.ucPriority;
    }

    if(p_zte_gbl_para->tagged_flag == 2)
    	p_zte_gbl_para->voice_data_svlan = stIpConfig.usSVLAN;

	return retVal;
}

ODM_SYS_IF_CONFIG_t g_stIpConfig_20;
void * stub_onu_config_interface_thread_20(void *pParm)
{
	ODM_SYS_IF_CONFIG_t stIpConfig;		

	memcpy(&stIpConfig, &g_stIpConfig_20, sizeof(stIpConfig));

    if (NO_ERROR != odmSysIfConfigSet(&stIpConfig))
    {	
		OP_DEBUG(DEBUG_LEVEL_WARNING, "OAM config interface failed\n");	
    }

	vosThreadExit(0);
	return NULL;
}
u32_t stub_zte_set_gbl_para(oam_zte_gbl_para_t *p_zte_gbl_para)
{
    char my_username[32]={0};
    char my_passwd[32]={0};	
    UINT32 retVal;

	/*share mode use for media and signal, admin and voip share mode is not achieve*/
	g_stIpConfig_20.ucIfShareMode = INTERFACE_SHARE_MODE_SHARE;
	//g_stIpConfig_20.ucIfShareMode = p_zte_gbl_para->voice_ip_releation;

	g_stIpConfig_20.ucIpMode = p_zte_gbl_para->voice_ip_mode;


	//printf("the vlan id %d\n the pri %d\n",p_zte_gbl_para->voice_data_cvlan,p_zte_gbl_para->voice_data_pri);

    if(g_stIpConfig_20.ucIpMode == 0)
    {
    	g_stIpConfig_20.ulIpAddr = p_zte_gbl_para->iad_ip_address;

    	g_stIpConfig_20.ulIpMask = p_zte_gbl_para->iad_net_mask;

    	g_stIpConfig_20.ulGwAddr = p_zte_gbl_para->iad_default_gw;

    	g_stIpConfig_20.ulDnsAddr = p_zte_gbl_para->primary_dns;
    }
    else if(g_stIpConfig_20.ucIpMode == 2)
    {
	g_stIpConfig_20.ucPPPoEAuthType = p_zte_gbl_para->pppoe_mode;

	stripzerostring(p_zte_gbl_para->pppoe_username, my_username, sizeof(p_zte_gbl_para->pppoe_username));
	stripzerostring(p_zte_gbl_para->pppoe_password, my_passwd, sizeof(p_zte_gbl_para->pppoe_password));	

	vosMemCpy(&g_stIpConfig_20.acPPPoEUser[0], my_username, 32);


	vosMemCpy(&g_stIpConfig_20.acPPPoEPass[0], my_passwd, 32);
    }
	g_stIpConfig_20.ucTagged = p_zte_gbl_para->tagged_flag;

    if(g_stIpConfig_20.ucTagged == 1 || g_stIpConfig_20.ucTagged == 2)
    {
    	g_stIpConfig_20.usCVlan = p_zte_gbl_para->voice_data_cvlan;

    	g_stIpConfig_20.ucPriority = p_zte_gbl_para->voice_data_pri;
    }

    if(g_stIpConfig_20.ucTagged == 2)
	{
		g_stIpConfig_20.usSVLAN = p_zte_gbl_para->voice_data_svlan;
	}

    g_stIpConfig_20.ucIf = INTERFACE_WAN;
    /* current version OAM just supported Admin/Media */
    g_stIpConfig_20.ucIfMode = INTERFACE_MODE_SIGNAL;

	if (NULL == vosThreadCreate("tIntCng", OP_VOS_THREAD_STKSZ, 100, 
        (void *)stub_onu_config_interface_thread_20, &g_stIpConfig_20))
	{
		OP_DEBUG(DEBUG_LEVEL_WARNING, "Create stub_onu_config_interface_thread thread failed\n");	
        return ERROR;
	}
	
	return NO_ERROR;
}


void eopl_zte_get_gbl_para(void)//card based
{
	u8_t	            *p_out;
    u8_t                 oamRspCode = 0;
	oam_rsp_indication_t *p_get_rsp;
	oam_zte_gbl_para_t	*p_zte_gbl_para;
	oam_instance_tlv_t_2_0  *p_inst;
	u32_t               ret;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t_2_0 *)p_out;
	p_out += sizeof(oam_instance_tlv_t_2_0);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

	p_inst->branch = 0x36;
	p_inst->leaf = 0x0001;
	p_inst->width = 0x01;
	p_inst->value = 1;

	p_zte_gbl_para  = (oam_zte_gbl_para_t *)p_out;
	p_zte_gbl_para->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_zte_gbl_para->leaf = OAM_ZTE_EXT_LEAF_GBL_PARA;

	ret = stub_zte_get_gbl_para(p_zte_gbl_para);


	if(ret != NO_ERROR)
	{
		p_zte_gbl_para->width  = OAM_GET_NO_RESOURCE;
		g_usOutDataIdx += 4;
	}
	else
	{
		p_zte_gbl_para->width = sizeof(oam_zte_gbl_para_t)-4;
		g_usOutDataIdx += p_zte_gbl_para->width + 4;
	}

}

void eopl_zte_set_gbl_para(u8_t *p_in)
{
	int					  i = 0;
	u32_t                 ret;
	u8_t                  *p_out;
	oam_set_response_t    *p_rsp;
	oam_zte_gbl_para_t     *p_zte_gbl_para;
	oam_instance_tlv_t_2_0    *p_inst;

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header_2_0(OAM_EXT_CODE_SET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	p_inst = (oam_instance_tlv_t_2_0 *)p_out;

	p_inst->branch = 0x36;
	p_inst->leaf = 0x0001;
	p_inst->width = 0x01;
	p_inst->value = 0x01;

	p_out += sizeof(oam_instance_tlv_t_2_0);
	g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);

	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_BRANCH_EXTENDED_ATTRIB;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_GBL_PARA;


	p_zte_gbl_para = (oam_zte_gbl_para_t *)p_in;

	ret = stub_zte_set_gbl_para(p_zte_gbl_para);
	
	if(ret == NO_ERROR)
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_SET_NO_RESOURCE;
	}

	g_usOutDataIdx += 4;


}

ODM_SYS_IF_CONFIG_t g_stIpMgmgConfig_20;

u32_t stub_set_mgmg_ip_addr_cfg(u32_t ip_addr, u32_t ip_mask)
{
    UINT32 retVal;

	g_stIpMgmgConfig_20.ulIpAddr = ip_addr;

	g_stIpMgmgConfig_20.ulIpMask = ip_mask;

	return NO_ERROR;
}

u32_t stub_set_mgmt_vlan_cfg(u8_t valid, u8_t mgmt_pri, u16_t mgmt_vlan)
{
    UINT32 retVal;

	g_stIpMgmgConfig_20.ucTagged = valid;

	g_stIpMgmgConfig_20.ucPriority = mgmt_pri;
	g_stIpMgmgConfig_20.usCVlan = mgmt_vlan;

	return NO_ERROR;

}

u8_t g_mgmt_status_cfg;

u32_t stub_set_mgmt_status_cfg(u8_t status)
{
#if 0
    ODM_SYS_IF_CONFIG_t stIpConfig;
    UINT32 retVal;

    stIpConfig.ucIf = INTERFACE_WAN;
    /* current version OAM just supported Admin/Media */
    stIpConfig.ucIfMode = INTERFACE_MODE_ADMIN;
    retVal = odmSysIfConfigGet(&stIpConfig);
    if (NO_ERROR != retVal)
    {
        return OAM_SET_BAD_PARAMETERS;
    }

	stIpConfig.ucIfShareMode = INTERFACE_SHARE_MODE_SHARE;

	stIpConfig.ucIpMode = IF_IP_MODE_STATIC;

	stIpConfig.ulIpAddr = 0;

	stIpConfig.ulIpMask = 0;

    stIpConfig.ucIf = INTERFACE_WAN;
    /* current version OAM just supported Admin/Media */
    stIpConfig.ucIfMode = INTERFACE_MODE_ADMIN;
    retVal = odmSysIfConfigSet(&stIpConfig);
    if (NO_ERROR != retVal)
    {
        return OAM_SET_BAD_PARAMETERS;
    }
#else
    g_mgmt_status_cfg = status;
#endif
	return NO_ERROR;
}

u32_t stub_set_mgmt_host_info_cfg(u32_t host_subnet, u32_t host_mask, u32_t gateway)
{
	//g_stIpMgmgConfig_20.ulIpAddr = host_subnet;

	//g_stIpMgmgConfig_20.ulIpMask= host_mask;

	g_stIpMgmgConfig_20.ulGwAddr = gateway;

	return NO_ERROR;
}

void * stub_onu_config_admin_interface_thread_20(void *pParm)
{
	ODM_SYS_IF_CONFIG_t stIpConfig;		

	memcpy(&stIpConfig, &g_stIpMgmgConfig_20, sizeof(stIpConfig));

    if (NO_ERROR != odmSysIfConfigSet(&stIpConfig))
    {
		OP_DEBUG(DEBUG_LEVEL_WARNING, "OAM config interface failed\n");	
    }

	vosThreadExit(0);
	return NULL;
}

void eopl_zte_set_mgmt_ip(u8_t *p_in,u8_t type,u8_t port)
{
	u32_t                ret;
	u8_t                 *p_out;
	oam_set_response_t   *p_rsp;
	oam_zte_mgmt_ip_cfg_t *p_zte_mgmt_ip;
	u8_t vlanValid = 0;

	EOPL_ZTE_HEADER_PROCESS(OAM_EXT_CODE_SET_RESPONSE,type,port);
	
	p_rsp = (oam_set_response_t*)p_out;
	p_rsp->branch = OAM_ZTE_EXT_BRANCH_ATTR;
	p_rsp->leaf = OAM_ZTE_EXT_LEAF_MGMT_IP_CFG;

	p_zte_mgmt_ip = (oam_zte_mgmt_ip_cfg_t *)p_in;

	  g_stIpMgmgConfig_20.ucIf = INTERFACE_WAN;
	/*share mode use for media and signal, admin and voip share mode is not achieve*/
	g_stIpMgmgConfig_20.ucIfShareMode = INTERFACE_SHARE_MODE_SHARE;
    g_stIpMgmgConfig_20.ucIfMode = INTERFACE_MODE_ADMIN;
	g_stIpMgmgConfig_20.ucIpMode = IF_IP_MODE_STATIC;

	if(p_zte_mgmt_ip->mgmt_status == ENABLED)
		vlanValid = ENABLED;
	else
		vlanValid = DISABLED;
	
	ret = stub_set_mgmg_ip_addr_cfg(p_zte_mgmt_ip->ip_addr, p_zte_mgmt_ip->ip_mask);
	if(ret != NO_ERROR)
		goto send_rsp;

	ret = stub_set_mgmt_vlan_cfg(vlanValid ,p_zte_mgmt_ip->mgmt_pri,p_zte_mgmt_ip->mgmt_vlan);
	if(ret != NO_ERROR)
		goto send_rsp;

	ret = stub_set_mgmt_status_cfg(p_zte_mgmt_ip->mgmt_status);
	if(ret != NO_ERROR)
		goto send_rsp;

	ret = stub_set_mgmt_host_info_cfg(p_zte_mgmt_ip->mgmt_host_subnet,
		                              p_zte_mgmt_ip->mgmt_host_mask,
		                              p_zte_mgmt_ip->mgmt_gateway);
	if(ret != NO_ERROR)
		goto send_rsp;


	if (NULL == vosThreadCreate("tIntCng", OP_VOS_THREAD_STKSZ, 100, 
        (void *)stub_onu_config_admin_interface_thread_20, &g_stIpMgmgConfig_20))
	{
		OP_DEBUG(DEBUG_LEVEL_WARNING, "Create stub_onu_config_interface_thread thread failed\n");	
       	ret = ERROR; 
	}

send_rsp:
	if(ret == NO_ERROR) /*for multicast, true means success */
	{
		p_rsp->indication = OAM_SET_OK;
	}
	else
	{
		p_rsp->indication = OAM_BAD_PARAM_VALUE;
	}

	g_usOutDataIdx += 4;
	//printf("ret %x\n",ret);
	return;
}

void eopl_zte_get_mgmt_ip(u8_t type,u8_t port)
{
	u8_t			*p_out;
	oam_zte_mgmt_ip_cfg_t *p_onu_mgmt_ip;
	oam_rsp_indication_t *p_get_mgmt_ip;

    ODM_SYS_IF_CONFIG_t stIpConfig;
	u8_t			oamRspCode;
	u32_t			ret;

	EOPL_ZTE_HEADER_PROCESS(OAM_EXT_CODE_GET_RESPONSE,type,port);

    stIpConfig.ucIf = INTERFACE_WAN;
    stIpConfig.ucIfMode = INTERFACE_MODE_ADMIN;
    ret= odmSysIfConfigGet(&stIpConfig);
	if(ret!= NO_ERROR)
	{
		oamRspCode = OAM_BAD_PARAM_VALUE;

		p_get_mgmt_ip = (oam_rsp_indication_t *)p_out;
		p_get_mgmt_ip->branch = OAM_ZTE_EXT_BRANCH_ATTR;
		p_get_mgmt_ip->leaf = OAM_ZTE_EXT_LEAF_MGMT_IP_CFG;
		p_get_mgmt_ip->var_indication = oamRspCode;

		g_usOutDataIdx += 4;

	}
	else
	{
		p_onu_mgmt_ip = (oam_zte_mgmt_ip_cfg_t*)p_out;

		p_onu_mgmt_ip->branch = OAM_ZTE_EXT_BRANCH_ATTR;
		p_onu_mgmt_ip->leaf = OAM_ZTE_EXT_LEAF_MGMT_IP_CFG;
		p_onu_mgmt_ip->width = 24;

		p_onu_mgmt_ip->ip_addr =stIpConfig.ulIpAddr;
		p_onu_mgmt_ip->ip_mask =stIpConfig.ulIpMask;
		p_onu_mgmt_ip->mgmt_pri =stIpConfig.ucPriority;
		p_onu_mgmt_ip->mgmt_vlan =stIpConfig.usCVlan;
		p_onu_mgmt_ip->mgmt_status = g_mgmt_status_cfg; //stIpConfig.ucAdminEnable; 
		//p_onu_mgmt_ip->mgmt_host_subnet = stIpConfig.ulIpAddr;
		//p_onu_mgmt_ip->mgmt_host_mask = stIpConfig.ulIpMask;
		p_onu_mgmt_ip->mgmt_gateway = stIpConfig.ulGwAddr;

		g_usOutDataIdx += p_onu_mgmt_ip->width + 4;
	}

}


void eopl_zte_no_support(u8_t	branch , u16_t	leaf)
{
	u8_t                            *p_out;
	u8_t                            oamRspCode;
	oam_rsp_indication_t			*p_get_rsp;
	oam_instance_tlv_t_2_0	            *p_inst;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"unknown tlv branch %d leaf %d", 
        branch,
        leaf);

	if(0 == g_usOutDataIdx)
	{
		eopl_ctc_header(OAM_EXT_CODE_GET_RESPONSE);
		p_out = &out_data[0];
		p_out += sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
		g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(ctc_header_t);
	}
	else
	{
		p_out = &out_data[g_usOutDataIdx];
	}

	if(port_instance_2_0.value && port_instance_2_0.value!=200)
	{
		p_inst = (oam_instance_tlv_t_2_0 *)p_out;
		*p_inst = port_instance_2_0;
		p_out += sizeof(oam_instance_tlv_t_2_0);
		g_usOutDataIdx += sizeof(oam_instance_tlv_t_2_0);
	}

	oamRspCode = OAM_GET_BAD_PARA;		
	p_get_rsp = (oam_rsp_indication_t *)p_out;
	p_get_rsp->branch = branch;
	p_get_rsp->leaf = leaf;
	p_get_rsp->var_indication = oamRspCode;
	
	g_usOutDataIdx += 4;

	if(g_usOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
 	{
		oam_pdu_padding(&out_data[g_usOutDataIdx],  OAM_MINIMIZE_PDU_SIZE - g_usOutDataIdx);
		g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
 	}

	OamFrameSend(out_data, g_usOutDataIdx);
	memset(&port_instance_2_0, 0, sizeof(oam_instance_tlv_t_2_0));
	g_usOutDataIdx = 0;

}

void eopl_zte_get_req(u8_t *p_in, u8_t type, u8_t port)
{
	oam_variable_descriptor_t *p_var_desc;
	OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"process zte ext get, type = %x, port = %x\n", type, port);

	while(1)
	{
		p_var_desc = (oam_variable_descriptor_t *)p_in;
		switch(p_var_desc->branch)
		{
			case OAM_ZTE_EXT_BRANCH_ATTR:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get branch: OAM_ZTE_EXT_BRANCH_ATTR\n");
				switch(p_var_desc->leaf)
				{
					case OAM_ZTE_EXT_LEAF_ONU_SUPPORT:
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_ONU_SUPPORT\n");
						eopl_zte_get_onu_support(type,port);
						break;
						
					case OAM_ZTE_EXT_LEAF_ONU_SN:
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_ONU_SN\n");

						eopl_zte_get_onu_sn(type,port);
						break;

					case OAM_ZTE_EXT_LEAF_MGMT_IP_CFG:
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_MGMT_IP_CFG\n");

						eopl_zte_get_mgmt_ip(type,port);
						break;
					case OAM_ZTE_EXT_LEAF_TYPE_C_GET:
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_TYPE_C_GET\n");

						eopl_zte_get_type_c(type,port);
						break;
					default:
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set unknown leaf %d", 
                            p_var_desc->branch,
                            p_var_desc->leaf);
						return;
				}
				p_in += sizeof(oam_variable_descriptor_t);

				break;

			case OAM_ZTE_ZTE_OUI_EXT_BRANCH_ATTR:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get branch: OAM_ZTE_ZTE_OUI_EXT_BRANCH_ATTR\n");
				switch(p_var_desc->leaf)
				{
					case OAM_ZTE_EXT_LEAF_GET_PORT_MODE:
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_GET_PORT_MODE\n");

						eopl_zte_get_port_mode(type,port);//port based();
						break;

					default:
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set unknown leaf %d", 
                            p_var_desc->branch,
                            p_var_desc->leaf);
						return;
				}
				p_in += sizeof(oam_variable_descriptor_t);

				break;
			default:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get branch: NULL\n");

				if(g_usOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
				{
					oam_pdu_padding(&out_data[g_usOutDataIdx],  OAM_MINIMIZE_PDU_SIZE - g_usOutDataIdx);
					g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
			 	}
				OamFrameSend(out_data, g_usOutDataIdx);
				g_usOutDataIdx = 0;
				return;
		}
	}
}

/* zte extension set req */
void eopl_zte_set_req(u8_t *p_in, u8_t type, u8_t port)
{
	oam_variable_descriptor_t	*p_var_desc;
	oam_zte_onu_sn_t             *p_onu_sn;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"process tw ext set, type = %d, port = %d\n", __FUNCTION__, type, port);

	while(1)
	{
		switch(*p_in)
		{
			case OAM_ZTE_EXT_BRANCH_ATTR:
				p_var_desc = (oam_variable_descriptor_t *)p_in;
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set branch OAM_ZTE_EXT_BRANCH_ATTR\n");
				switch(p_var_desc->leaf)
				{
					case OAM_ZTE_EXT_LEAF_CONFIG_PARAM:
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_CONFIG_PARAM\n");
						eopl_zte_set_config_param(p_in,type,port);
						p_in += sizeof(oam_zte_onu_config_param_t) ;

						break;						
					case OAM_ZTE_EXT_LEAF_MGMT_IP_CFG:
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_MGMT_IP_CFG\n");
						eopl_zte_set_mgmt_ip(p_in,type,port);
						p_in += sizeof(oam_zte_mgmt_ip_cfg_t) ;

						break;
					case OAM_ZTE_EXT_LEAF_TYPE_C_SET:
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_TYPE_C_SET\n");
						eopl_zte_set_type_c(p_in,type,port);
						p_in += sizeof(oam_zte_type_c_t) ;

						break;

					default:
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set unknown leaf %d", 
                            p_var_desc->branch,
                            p_var_desc->leaf);
						return;
				}
				break;

			case OAM_ZTE_ZTE_OUI_EXT_BRANCH_ATTR:
				p_var_desc = (oam_variable_descriptor_t *)p_in;
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set branch OAM_ZTE_ZTE_OUI_EXT_BRANCH_ATTR 0x10\n");
				switch(p_var_desc->leaf)
				{
					
					case OAM_ZTE_EXT_LEAF_SET_PORT_MODE:
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_SET_PORT_MODE\n");						
						eopl_zte_set_port_mode(p_in,type,port);
						p_in += sizeof(oam_zte_port_mode_t) ;
						break;
					default:
						OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set unknown leaf %d", 
                            p_var_desc->branch,
                            p_var_desc->leaf);
						return;
				}
				break;	


			default:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set branch null\n");
				if(g_usOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
			 	{
					oam_pdu_padding(&out_data[g_usOutDataIdx],  OAM_MINIMIZE_PDU_SIZE - g_usOutDataIdx);
					g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
			 	}
				OamFrameSend(out_data, g_usOutDataIdx);
				g_usOutDataIdx = 0;
				return;
		}
	}
}


void eopl_zte_pdu_process(u8_t *p_byte,u16_t length)
{
	zte_header_t     *p_zte_hdr;
	oampdu_hdr_t    *p_oam_hdr;
    u8_t	        *p_out;
	zte_header_t     *p_out_zte_hdr;

	p_oam_hdr = (oampdu_hdr_t *)p_byte;
	p_byte += sizeof(oampdu_hdr_t);

	p_zte_hdr = (zte_header_t *)p_byte;
	p_byte += sizeof(zte_header_t);

	switch(p_zte_hdr->ext_opcode)
	{
		case OAM_ZTE_EXT_OPCODE_GET_REQUEST:
			eopl_zte_get_req(p_byte, p_zte_hdr->type, p_zte_hdr->port);
			break;
		case OAM_ZTE_EXT_OPCODE_SET_REQUEST:
			eopl_zte_set_req(p_byte, p_zte_hdr->type, p_zte_hdr->port);
			break;
		default:
            OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"Unknow Ext. Opcode (%d)", p_zte_hdr->ext_opcode);
        	p_out = &out_data[0];
        	eopl_oam_pdu_hdr(p_out, oams.flags, OAM_CODE_ORG_SPEC);
        	p_out += sizeof(oampdu_hdr_t);

        	p_out_zte_hdr = (zte_header_t*)p_out;
        	vosMemCpy(p_out_zte_hdr->oui, p_zte_hdr->oui, 3);
        	p_out_zte_hdr->ext_opcode = p_zte_hdr->ext_opcode;
            
            g_usOutDataIdx = sizeof(oampdu_hdr_t) + sizeof(zte_header_t);
            
            if(g_usOutDataIdx < OAM_MINIMIZE_PDU_SIZE)
		 	{
				oam_pdu_padding(&out_data[g_usOutDataIdx],  OAM_MINIMIZE_PDU_SIZE - g_usOutDataIdx);
				g_usOutDataIdx = OAM_MINIMIZE_PDU_SIZE;
		 	}
			OamFrameSend(out_data, g_usOutDataIdx);
			g_usOutDataIdx = 0;
			break;
	}

	return;
}


void eopl_zte_ctc_get_req(u16_t leaf, u8_t** pp_byte)
{
	u8_t* p_byte = *pp_byte;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"zte ext ctc get req leaf = %x\n", leaf);

	switch(leaf)
	{
		case OAM_ZTE_EXT_LEAF_ALARM_CFG:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_ALARM_CFG\n");
			eopl_zte_get_alarm_cfg();//onu based
			break;

		case OAM_ZTE_EXT_LEAF_UPGRADE_PARA:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_UPGRADE_PARA\n");
			eopl_zte_get_upgrade_para();//onu based
			break;

		case OAM_ZTE_EXT_LEAF_MAC_ADDR_LIMIT:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_MAC_ADDR_LIMIT\n");
			eopl_zte_get_mac_addr_limit();//port based
			break;

		case OAM_ZTE_EXT_LEAF_MAC_ADDR_AGING:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_MAC_ADDR_AGING\n");
			eopl_zte_get_mac_addr_aging();//onu based
			break;

		case OAM_ZTE_EXT_LEAF_PORT_MAC_FILTER:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_PORT_MAC_FILTER\n");
			eopl_zte_get_port_mac_filter();//port based
			break;

		case OAM_ZTE_EXT_LEAF_PORT_MAC_BIND:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_PORT_MAC_BIND\n");
			eopl_zte_get_port_mac_binding();//poty based
			break;

		case OAM_ZTE_EXT_LEAF_STATIC_MAC_CFG:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_STATIC_MAC_CFG\n");
			eopl_zte_get_static_mac_cfg();//port based
			break;

		case OAM_ZTE_EXT_LEAF_UNI_ISOLATION:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_UNI_ISOLATION\n");
			eopl_zte_get_uni_isolation();//onu based
			break;

		case OAM_ZTE_EXT_LEAF_ONU_ETHER_STATS:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_ONU_ETHER_STATS\n");
			eopl_zte_get_onu_ether_stats();//port based
			break;

		case OAM_ZTE_EXT_LEAF_UNI_MAC_QUERY:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_UNI_MAC_QUERY\n");
			eopl_zte_get_uni_mac_query(p_byte);//onu based
			*pp_byte = p_byte + 5;
			break;
		case OAM_ZTE_EXT_LEAF_PMAC_DS_SHAPING:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_PMAC_DS_SHAPING\n");
			eopl_zte_get_pmac_ds_shaping();//onu based
			break;

		case OAM_ZTE_EXT_LEAF_PMAC_BUFF_CFG:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_PMAC_BUFF_CFG\n");
			eopl_zte_get_pmac_buff_cfg(p_byte);//onu based
			*pp_byte = p_byte + 2;
			break;

		case OAM_ZTE_EXT_LEAF_PMAC_BUFF_QUERY:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_PMAC_BUFF_QUERY\n");
			eopl_zte_get_pmac_buff_query();//onu based
			break;

		case OAM_ZTE_EXT_LEAF_UNI_STATS_CFG:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_UNI_STATS_CFG\n");
			eopl_zte_get_uni_stats_cfg();//onu based
			break;

		case OAM_ZTE_EXT_LEAF_UNI_STATS_QUERY:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_UNI_STATS_QUERY\n");
			eopl_zte_get_uni_stats_query();//port based
			break;
		case OAM_ZTE_EXT_LEAF_TX_LASER_ALWAYS_ON:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_TX_LASER_ALWAYS_ON\n");
			//eopl_zte_set_tx_laser_conrol();//port based
			break;
		case OAM_ZTE_EXT_LEAF_TX_LASER_CONTROL:
						
			break;
		case OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_1:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_1\n");
			eopl_zte_get_pon_statics_1();//onu based
			break;
		case OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_2:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_PON_PORT_STATICS_GET_2\n");
			eopl_zte_get_pon_statics_2();//onu based
			break;		

		case OAM_ZTE_EXT_LEAF_GBL_PARA:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_GBL_PARA\n");
			eopl_zte_get_gbl_para();//card based
			break;

		default:
            eopl_not_support_response_2_0((u8_t)*p_byte,leaf,OAM_EXT_CODE_GET_RESPONSE);
            *pp_byte = p_byte + sizeof(oam_rsp_indication_t);
            OAM_DBG("[%s]Unknown leaf value!!\n ",__FUNCTION__);
	}

}


void eopl_zte_ctc_set_req(u16_t leaf, u8_t** pp_byte)
{
	oam_zte_alarm_cfg_t          *p_zte_alarm_cfg;
	oam_zte_upgrade_para_t       *p_zte_upgrade_para;
	oam_zte_mac_limit_t          *p_zte_mac_limit;
	oam_zte_mac_aging_t          *p_zte_mac_aging;
	oam_zte_port_mac_filter_t    *p_zte_port_mac_filter;
	oam_zte_port_mac_binding_t   *p_zte_port_mac_bind;
	oam_zte_static_mac_cfg_t     *p_zte_static_mac_cfg;
	oam_zte_uni_isolation_t      *p_zte_uni_isolation;
	oam_zte_port_mode_t          *p_zte_port_mode;
	oam_zte_pmac_ds_shaping_t    *p_zte_pmac_ds_shaping;
	oam_zte_pmac_buff_cfg_t      *p_zte_pmac_buff_cfg;
	oam_zte_uni_stats_cfg_t      *p_zte_uni_stats_cfg;
	oam_zte_uni_stats_reset_t    *p_zte_uni_stats_reset;
	oam_zte_hg_mac_info_t        *p_zte_hg_mac_info;
	oam_zte_hg_mgmt_vlan_t       *p_zte_hg_mgmt_vlan;
	oam_zte_hg_access_info_t     *p_zte_hg_access_info;
	oam_zte_gbl_para_t           *p_zte_gbl_para;
	oam_zte_tx_laser_control_t   *p_zte_set_tx_control;
	u8_t* p_byte = *pp_byte;

	OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"zte ext ctc set req leaf = %x\n", leaf);
		
	switch(leaf)
	{	/* ZTE CTC extension */
		case OAM_ZTE_EXT_LEAF_ALARM_CFG:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_ALARM_CFG\n");

			p_zte_alarm_cfg = (oam_zte_alarm_cfg_t *)p_byte;

			eopl_zte_set_alarm_cfg(p_byte);//onu based

			*pp_byte  = p_byte + p_zte_alarm_cfg->width + 4;

			break;

		case OAM_ZTE_EXT_LEAF_UPGRADE_PARA:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_UPGRADE_PARA\n");

			p_zte_upgrade_para = (oam_zte_upgrade_para_t *)p_byte;

			eopl_zte_set_upgrade_para(p_byte);//onu based

			*pp_byte = p_byte + p_zte_upgrade_para->width + 4;

			break;

		case OAM_ZTE_EXT_LEAF_MAC_ADDR_LIMIT:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_MAC_ADDR_LIMIT\n");

			p_zte_mac_limit = (oam_zte_mac_limit_t *)p_byte;

			eopl_zte_set_mac_addr_limit(p_byte);//port based

			*pp_byte = p_byte + p_zte_mac_limit->width + 4;

			break;

		case OAM_ZTE_EXT_LEAF_MAC_ADDR_AGING:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_MAC_ADDR_AGING\n");

			p_zte_mac_aging = (oam_zte_mac_aging_t *)p_byte;

			eopl_zte_set_mac_addr_aging(p_byte);//onu based

			*pp_byte = p_byte + p_zte_mac_aging->width + 4;

			break;

		case OAM_ZTE_EXT_LEAF_PORT_MAC_FILTER:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_PORT_MAC_FILTER\n");

			p_zte_port_mac_filter = (oam_zte_port_mac_filter_t *)p_byte;

			eopl_zte_set_port_mac_filter(p_byte);//port based

			*pp_byte = p_byte + p_zte_port_mac_filter->width + 4;

			break;

		case OAM_ZTE_EXT_LEAF_PORT_MAC_BIND:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_PORT_MAC_BIND\n");

			p_zte_port_mac_bind = (oam_zte_port_mac_binding_t *)p_byte;

			eopl_zte_set_port_mac_binding(p_byte);//port based

			*pp_byte = p_byte + p_zte_port_mac_bind->width + 4;

			break;

		case OAM_ZTE_EXT_LEAF_STATIC_MAC_CFG:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_STATIC_MAC_CFG\n");

			p_zte_static_mac_cfg = (oam_zte_static_mac_cfg_t *)p_byte;

			eopl_zte_set_static_mac_cfg(p_byte);//port based

			*pp_byte = p_byte + p_zte_static_mac_cfg->width + 4;

			break;

		case OAM_ZTE_EXT_LEAF_UNI_ISOLATION:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_UNI_ISOLATION\n");

			p_zte_uni_isolation = (oam_zte_uni_isolation_t *)p_byte;

			eopl_zte_set_uni_isolation(p_byte);//onu based

			*pp_byte = p_byte + p_zte_uni_isolation->width + 4;

			break;
			

		case OAM_ZTE_EXT_LEAF_PMAC_DS_SHAPING:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_PMAC_DS_SHAPING\n");

			p_zte_pmac_ds_shaping = (oam_zte_pmac_ds_shaping_t *)p_byte;

			eopl_zte_set_pmac_ds_shaping(p_byte);//port based

			*pp_byte = p_byte + p_zte_pmac_ds_shaping->width + 4;

			break;

		case OAM_ZTE_EXT_LEAF_PMAC_BUFF_CFG:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_PMAC_BUFF_CFG\n");

			p_zte_pmac_buff_cfg = (oam_zte_pmac_buff_cfg_t *)p_byte;

			eopl_zte_set_pmac_buff_cfg(p_byte);//onu based

			*pp_byte = p_byte + p_zte_pmac_buff_cfg->width + 4;

			break;

		case OAM_ZTE_EXT_LEAF_UNI_STATS_CFG:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_UNI_STATS_CFG\n");

			p_zte_uni_stats_cfg = (oam_zte_uni_stats_cfg_t *)p_byte;

			eopl_zte_set_uni_stats_cfg(p_byte);//port based

			*pp_byte = p_byte + p_zte_uni_stats_cfg->width + 4;

			break;

		case OAM_ZTE_EXT_LEAF_UNI_STATS_RESET:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_UNI_STATS_RESET\n");

			p_zte_uni_stats_reset = (oam_zte_uni_stats_reset_t *)p_byte;

			eopl_zte_set_uni_stats_reset(p_byte);//port based

			*pp_byte = p_byte + p_zte_uni_stats_reset->width + 4;

			break;
		/*case OAM_ZTE_EXT_LEAF_TX_LASER_ALWAYS_ON:
						
			break;*/
		case OAM_ZTE_EXT_LEAF_TX_LASER_CONTROL:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"get OAM_ZTE_EXT_LEAF_TX_LASER_CONTROL\n");
			p_zte_set_tx_control = (oam_zte_tx_laser_control_t *)p_byte;
			eopl_zte_set_tx_laser_conrol(p_byte);//onu based
			*pp_byte = p_byte + p_zte_set_tx_control->width + 4;
			break;

		case OAM_ZTE_EXT_LEAF_GBL_PARA:
			OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_GBL_PARA\n");

			p_zte_gbl_para = (oam_zte_gbl_para_t *)p_byte;

			eopl_zte_set_gbl_para(p_byte);//card based

			*pp_byte = p_byte + p_zte_gbl_para->width + 4;

			break;
#if 0
			case OAM_ZTE_EXT_LEAF_HG_MAC_INFO:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_HG_MAC_INFO\n");

				p_zte_hg_mac_info = (oam_zte_hg_mac_info_t *)p_byte;

				eopl_zte_set_hg_mac_info(p_byte);//port based

				p_byte += p_zte_hg_mac_info->width + 4;

				break;

			case OAM_ZTE_EXT_LEAF_HG_MGMT_VLAN:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_HG_MGMT_VLAN\n");

				p_zte_hg_mgmt_vlan = (oam_zte_hg_mgmt_vlan_t *)p_byte;

				eopl_zte_set_hg_mgmt_vlan(p_byte);//onu based

				p_byte += p_zte_hg_mgmt_vlan->width + 4;

				break;

			case OAM_ZTE_EXT_LEAF_HG_ACCESS_INFO:
				OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"set OAM_ZTE_EXT_LEAF_HG_ACCESS_INFO\n");

				p_zte_hg_access_info = (oam_zte_hg_access_info_t *)p_byte;

				eopl_zte_set_hg_access_info(p_byte);//onu based

				p_byte += p_zte_hg_access_info->width + 4;

				break;
#endif

		default:
            eopl_not_support_response_2_0((u8_t)*p_byte,leaf,OAM_EXT_CODE_GET_RESPONSE);
            *pp_byte = p_byte + sizeof(oam_rsp_indication_t);
            OAM_DBG("[%s]Unknown leaf value!!\n ",__FUNCTION__);
			
	}

}

#endif

