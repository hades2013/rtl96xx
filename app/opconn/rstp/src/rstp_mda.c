/*******************************************************************************
*
* Copyright (C) 2007 OPL, All Rights Reserved
*
* FILENAME: MDA_rstp.c
*
* DESCRIPTION: 
*
* Date Created (optional): 03/08/2007
*
* Authors (optional): 
*     luzhihua
* Reviewed by (optional):
*
* Edit History:
*
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/rstp/src/rstp_mda.c#1 $
* $Log: rstp_mda.c,v $
* Revision 1.1.1.1  2007/08/03 09:02:07  cvsadmin
* Import OPsystem new sources
*
* Revision 1.1.1.1  2007/05/17 02:53:29  schen
* Change CVS server
*
* Revision 1.2  2007/04/25 04:59:24  rzhou
* Add and delete some comments
*
* Revision 1.1.1.1  2007/04/09 06:07:15  pho
* Import OPsystem ipmux_e sources
*
* Revision 1.1.1.1  2007/04/09 04:50:47  pho
* Import OPsystem sources
*
* Revision 1.2  2007/03/21 04:15:02  rzhou
* delete printf and modify some rstp show related
*
* Revision 1.1  2007/03/19 05:09:48  rzhou
* Remove some unused header files and add rstp_mda.c
*
* Revision 1.2  2007/03/09 06:37:34  zlu
* *** empty log message ***
*
* Revision 1.1  2007/03/08 11:14:31  zlu
* *** empty log message ***
*
*
*
*******************************************************************************/
/***** INCLUDE FILES *****/
#include <memory.h>
#include "eno.h"
#include "tlv.h"
#include "tlv_event_id.h"
#include "tlv_attr.h"
#include "task_id.h"
#include "sys_common.h"
#include "cli_keyword.h"
#include "rstp_bitmap.h"

/***************** Global Constants, Variables and Functions Declaration *****************/
int Intf_RstpEnable(unsigned char ucEnableFlag);
int Intf_RstpBdgPri(unsigned short usPri);
int Intf_RstpBdgMax(unsigned char ucMaxVal);
int Intf_RstpBdgDly(unsigned char ucDelayVal);
int Intf_RstpBdgVer(unsigned char ucVerFlag);
int Intf_RstpPortPri(unsigned char ucPort, unsigned char ucPri);
int Intf_RstpPortmcheck(unsigned char ucPort);
int Intf_RstpPcost(unsigned char ucPort, unsigned int uiPcost);
int Intf_RstpEdge(unsigned char ucPort, unsigned char ucEdge);
int Intf_RstpNonstp(unsigned char ucPort, unsigned char ucNonStp);
int Intf_RstpP2p(unsigned char ucPort, unsigned char ucP2p);
int Intf_RstpTrace(unsigned char ucPort, CLI_KEYWORD_TYPE_t enuCliKey);
int Intf_Show_Rstp_Port(unsigned short usPortS, unsigned short usPortE);
int Intf_Show_Rstp_Bridge(void);

/*******************************************************************************
*
* NAME: MDA_rstp
*
* DESCRIPTION: Configuration Management Sync Event thread to handle CLI command
*              requests.
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
int MDA_rstp(char *buf_p)
{
    int id, len;
    struct tlv_msg_s tlv_msg;
    unsigned char cfg_file[SYSTEM_PATH_SIZE];
    int offset = 0;
	unsigned char ucRstpEnable;
	unsigned char ucRstpCmd, ucRstpFver, ucRstpPortCmd;
	unsigned short usRstpBdgPri;
	unsigned char ucRstpBdgMax, ucRstpBdgDly;
	unsigned char ucRstpPortID = 0xFF, ucRstpPortPri, ucRstpPortEdge;
	unsigned int uiRstpPcost;
	unsigned char ucRstpPortNonstp, ucRstpPortP2p;
	unsigned short usCliKeyword;
	CLI_KEYWORD_TYPE_t enuCliKeyType;
	
	ucRstpCmd = ucRstpPortCmd = 0; 
	ucRstpBdgDly = 0;
	usCliKeyword = 0;
	
    memcpy((char *) &tlv_msg, buf_p, sizeof(TLV_MSG_T));
    while (1)
	{
        id = TLV_len_get(&tlv_msg.data[offset], ATTR_TYPE_LEN,
                         (int *) &offset);
        len =
            TLV_len_get(&tlv_msg.data[offset], ATTR_LEN, (int *) &offset);

		if ((0 == id) && (0 == len))
        {
		    break;
        }
		
		switch(id)
		{
		case RSTP_ENABLE:
            TLV_array_get(&tlv_msg.data[offset], (char *) &cfg_file, len,
                          (int *) &offset);
            cfg_file[len] = '\0';
            ucRstpEnable = cfg_file[0];
			Intf_RstpEnable(ucRstpEnable);
            break;
		case RSTP_BRIDGE:
            TLV_array_get(&tlv_msg.data[offset], (char *) &cfg_file, len,
                          (int *) &offset);
            cfg_file[len] = '\0';
            ucRstpCmd = cfg_file[0];
            break;
		case RSTP_BRD_PRI:
            TLV_array_get(&tlv_msg.data[offset], (char *) &cfg_file, len,
                          (int *) &offset);
            cfg_file[len] = '\0';
            usRstpBdgPri = cfg_file[0] << 8;
			usRstpBdgPri |= cfg_file[1];
			Intf_RstpBdgPri(usRstpBdgPri);
            break;
		case RSTP_BRD_MAX:
            TLV_array_get(&tlv_msg.data[offset], (char *) &cfg_file, len,
                          (int *) &offset);
            cfg_file[len] = '\0';
            ucRstpBdgMax = cfg_file[0];
			Intf_RstpBdgMax(ucRstpBdgMax);
            break;
		case RSTP_BRD_FDLAY:
            TLV_array_get(&tlv_msg.data[offset], (char *) &cfg_file, len,
                          (int *) &offset);
            cfg_file[len] = '\0';
            ucRstpBdgDly = cfg_file[0];
			Intf_RstpBdgDly(ucRstpBdgDly);
            break;
		case RSTP_BRD_FVER:
            TLV_array_get(&tlv_msg.data[offset], (char *) &cfg_file, len,
                          (int *) &offset);
            cfg_file[len] = '\0';
            ucRstpFver = cfg_file[0];
			Intf_RstpBdgVer(ucRstpBdgDly);
            break;
		case RSTP_PORT:
            TLV_array_get(&tlv_msg.data[offset], (char *) &cfg_file, len,
                          (int *) &offset);
            cfg_file[len] = '\0';
            ucRstpPortCmd = cfg_file[0];
            break;
		case RSTP_PORTID:
            TLV_array_get(&tlv_msg.data[offset], (char *) &cfg_file, len,
                          (int *) &offset);
            cfg_file[len] = '\0';
            ucRstpPortID = cfg_file[0];
            break;
		case RSTP_PORT_PRI:
            TLV_array_get(&tlv_msg.data[offset], (char *) &cfg_file, len,
                          (int *) &offset);
            cfg_file[len] = '\0';
            ucRstpPortPri = cfg_file[0];
			Intf_RstpPortPri(ucRstpPortID, ucRstpPortPri);
            break;
		case RSTP_PORT_PCOST:
            TLV_array_get(&tlv_msg.data[offset], (char *) &cfg_file, len,
                          (int *) &offset);
            cfg_file[len] = '\0';
            uiRstpPcost = cfg_file[0] << 24;
			uiRstpPcost |= cfg_file[1] << 16;
			uiRstpPcost |= cfg_file[2] << 8;
			uiRstpPcost |= cfg_file[3];
			Intf_RstpPcost(ucRstpPortID, uiRstpPcost);
            break;
		case RSTP_EDGE:
            TLV_array_get(&tlv_msg.data[offset], (char *) &cfg_file, len,
                          (int *) &offset);
            cfg_file[len] = '\0';
            ucRstpPortEdge = cfg_file[0];
			Intf_RstpEdge(ucRstpPortID, ucRstpPortEdge);
            break;
		case RSTP_NONSTP:
            TLV_array_get(&tlv_msg.data[offset], (char *) &cfg_file, len,
                          (int *) &offset);
            cfg_file[len] = '\0';
            ucRstpPortNonstp = cfg_file[0];
			Intf_RstpNonstp(ucRstpPortID, ucRstpPortNonstp);
            break;
		case RSTP_P2P:
            TLV_array_get(&tlv_msg.data[offset], (char *) &cfg_file, len,
                          (int *) &offset);
            cfg_file[len] = '\0';
            ucRstpPortP2p = cfg_file[0];
			Intf_RstpP2p(ucRstpPortID, ucRstpPortP2p);
            break;
		case CLI_KEYWORD:
            TLV_array_get(&tlv_msg.data[offset], (char *) &cfg_file, len,
                          (int *) &offset);
            cfg_file[len] = '\0';
            usCliKeyword = cfg_file[0] << 8;
			usCliKeyword |= cfg_file[1];
            break;
			
		default:
			break;
		}
	}
	switch(ucRstpCmd)
	{
	case 1:
		Intf_RstpBdgPri(0x8000);
		break;
	case 2:
		Intf_RstpBdgMax(20);
		break;
	case 3:
		Intf_RstpBdgDly(15);
		break;
	case 4:
		Intf_RstpBdgVer(0);
		break;
	default:
		break;
	}
	switch(ucRstpPortCmd)
	{
	case 1:
		Intf_RstpPortPri(ucRstpPortID, 128);
		break;
	case 2:
		Intf_RstpPortmcheck(ucRstpPortID);
		break;
	case 3:
		Intf_RstpEdge(ucRstpPortID, 1);
		break;
	case 4:
		Intf_RstpNonstp(ucRstpPortID, 0);
		break;
	case 5:
		Intf_RstpP2p(ucRstpPortID, 2);
		break;
	default:
		break;
	}
	switch(usCliKeyword)
	{
	case RSTP_TRACE_INFO:
		enuCliKeyType = RSTP_TRACE_INFO;
		Intf_RstpTrace(ucRstpPortID, enuCliKeyType);
		break;
	case RSTP_TRACE_ROLETRNS:
		enuCliKeyType = RSTP_TRACE_ROLETRNS;
		Intf_RstpTrace(ucRstpPortID, enuCliKeyType);
		break;
	case RSTP_TRACE_STTRANS:
		enuCliKeyType = RSTP_TRACE_STTRANS;
		Intf_RstpTrace(ucRstpPortID, enuCliKeyType);
		break;
	case RSTP_TRACE_TOPOCH:
		enuCliKeyType = RSTP_TRACE_TOPOCH;
		Intf_RstpTrace(ucRstpPortID, enuCliKeyType);
		break;
	case RSTP_TRACE_MIGRATE:
		enuCliKeyType = RSTP_TRACE_MIGRATE;
		Intf_RstpTrace(ucRstpPortID, enuCliKeyType);
		break;
	case RSTP_TRACE_TRANSMIT:
		enuCliKeyType = RSTP_TRACE_TRANSMIT;
		Intf_RstpTrace(ucRstpPortID, enuCliKeyType);
		break;
	case RSTP_TRACE_P2P:
		enuCliKeyType = RSTP_TRACE_P2P;
		Intf_RstpTrace(ucRstpPortID, enuCliKeyType);
		break;
	case RSTP_TRACE_EDGE:
		enuCliKeyType = RSTP_TRACE_EDGE;
		Intf_RstpTrace(ucRstpPortID, enuCliKeyType);
		break;
	case RSTP_TRACE_PCOST:
		enuCliKeyType = RSTP_TRACE_PCOST;
		Intf_RstpTrace(ucRstpPortID, enuCliKeyType);
		break;
	case RSTP_TRACE_ENABLE:
		enuCliKeyType = RSTP_TRACE_ENABLE;
		Intf_RstpTrace(ucRstpPortID, enuCliKeyType);
		break;
	case RSTP_TRACE_DISABLE:
		enuCliKeyType = RSTP_TRACE_DISABLE;
		Intf_RstpTrace(ucRstpPortID, enuCliKeyType);
		break;
	default:
		break;
	}
	return 0;
}


/******************************************************************************
 *
 * enable or disable rstp
 *
 * ucEnableFlag = 1 ------- enable
 *              = 0 ------- disable
 */
int Intf_RstpEnable(unsigned char ucEnableFlag)
{
	extern int cli_enable (void);
	extern int cli_disable (void);
	//printf("RstpEnable =%d \n",ucEnableFlag);
	if(1 == ucEnableFlag)
		cli_enable();
	else 
		cli_disable();
	return 0;
}
/******************************************************************************
 *
 * default value is 0x8000
 */
int Intf_RstpBdgPri(unsigned short usPri)
{
	extern int cli_br_prio (int br_prio);
	//printf("Intf_RstpBdgPri:usPri =%d \n",usPri);
	cli_br_prio(usPri);
	return 0;
}

/******************************************************************************
 *
 * default value is 20
 */
int Intf_RstpBdgMax(unsigned char ucMaxVal)
{
	extern int cli_br_maxage (int value);
	//printf("Intf_RstpBdgMax:ucMaxVal =%d\n",ucMaxVal);
	cli_br_maxage(ucMaxVal);	
	return 0;
}

/******************************************************************************
 *
 * default value is 15
 */
int Intf_RstpBdgDly(unsigned char ucDelayVal)
{
	extern int cli_br_fdelay (int value);	
	//printf("Intf_RstpBdgDly =%d\n",ucDelayVal);
	cli_br_fdelay(ucDelayVal);
	return 0;
}

/******************************************************************************
 *
 * ucVerFlag = 1 ----------- yes
 *             0 ----------- no
 *
 * default value is 0
 */
int Intf_RstpBdgVer(unsigned char ucVerFlag)
{
	extern int cli_br_fvers (int value);
	//printf("Intf_RstpBdgVer =%d\n",ucVerFlag);
	cli_br_fvers(ucVerFlag);		
	return 0;
}

/******************************************************************************
 *
 * default value is 128
 */
int Intf_RstpPortPri(unsigned char ucPort, unsigned char ucPri)
{
	//printf("Intf_RstpPortPri:ucPort=%d ;ucPri = %d \n",ucPort,ucPri);
	extern int cli_prt_prio (int port_index, int value);
	cli_prt_prio(ucPort, ucPri);
	return 0;
}

int Intf_RstpPortmcheck(unsigned char ucPort)
{
	extern int cli_prt_mcheck (int port_index);
	//printf("Intf_RstpPortmcheck =%d\n",ucPort);
	cli_prt_mcheck(ucPort);	
	return 0;
}

int Intf_RstpPcost(unsigned char ucPort, unsigned int uiPcost)
{
	extern int cli_prt_pcost (int port_index, int value);
	//printf("Intf_RstpPcost:ucPort =%d, uiPcost =%d \n",ucPort, uiPcost);
	cli_prt_pcost(ucPort, uiPcost);
	return 0;
}

/******************************************************************************
 *
 * ucEdge = 1 --------------- yes
 *          0 --------------- no
 *
 * default value is 1
 */
int Intf_RstpEdge(unsigned char ucPort, unsigned char ucEdge)
{
	extern int cli_prt_edge (int port_index, int value);
	//printf("Intf_RstpEdge:ucPort =%d, ucEdge =%d \n",ucPort, ucEdge);
	cli_prt_edge(ucPort, ucEdge);
	return 0;
}

/******************************************************************************
 *
 * ucNonStp = 1 ------------- yes
 *            0 ------------- no
 *
 * default value is 0
 */
int Intf_RstpNonstp(unsigned char ucPort, unsigned char ucNonStp)
{
	extern int cli_prt_non_stp (int port_index, int value);
	//printf("Intf_RstpNonstp:ucPort =%d, ucNonStp =%d \n",ucPort, ucNonStp);
	cli_prt_non_stp(ucPort, ucNonStp);
	return 0;
}

/******************************************************************************
 *
 * ucP2p = 0 --------------- no
 *         1 --------------- yes
 *         2 --------------- all
 *
 * default value is 2
 */
int Intf_RstpP2p(unsigned char ucPort, unsigned char ucP2p)
{
	  extern int cli_prt_p2p (int port_index, unsigned char value);
		//printf("Intf_RstpP2p:ucPort =%d, ucP2p =%d \n",ucPort, ucP2p);
		cli_prt_p2p (ucPort, ucP2p);
		return 0;
}

int Intf_RstpTrace(unsigned char ucPort, CLI_KEYWORD_TYPE_t enuCliKey)
{
		
		return 0;
}
int Intf_Show_Rstp_Bridge(void)
{
	/* begin: add by jiangmingli for RSTP process */
	#if 0
	extern int cli_br_get_cfg(void);
	cli_br_get_cfg ();
	#endif
	/* end: add by jiangmingli for RSTP process */
	return 0;
}

int Intf_Show_Rstp_Port(unsigned short usPortS, unsigned short usPortE)
{
	
	/* begin: add by jiangmingli for RSTP process */
	#if 0
	extern void show_rstp_port (BITMAP_T* ports_bitmap, int detail);	
	#endif
	/* end: add by jiangmingli for RSTP process */
	BITMAP_T  ports;
	int i;
	
	if((0xFFFF == usPortS) && (0xFFFF == usPortE))
	{
        BitmapSetAllBits(&ports);
				/* begin: add by jiangmingli for RSTP process */
				#if 0
				show_rstp_port (&ports, 0);
				#endif
				/* end: add by jiangmingli for RSTP process */
	}
	else if ((0xFFFF == usPortE)&&(usPortS < 1024))
	{
        BitmapSetBit(&ports, (usPortS - 1));
				/* begin: add by jiangmingli for RSTP process */
				#if 0
				show_rstp_port (&ports, 1);
				#endif
				/* end: add by jiangmingli for RSTP process */
	}
	else if((usPortE <1024 )&&(usPortS < 1024)&&(usPortE>=usPortS))
	{			for(i= usPortS;i<=usPortE;i++)
        		BitmapSetBit(&ports, (i - 1));
        /* begin: add by jiangmingli for RSTP process */
		#if 0
		show_rstp_port (&ports, 0);
		#endif
		/* end: add by jiangmingli for RSTP process */
	}
	return 0;
}
