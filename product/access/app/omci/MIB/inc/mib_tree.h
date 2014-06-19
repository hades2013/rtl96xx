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
 */


#ifndef __MIB_TREE_H__
#define __MIB_TREE_H__


#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include "omci_defs.h"
#include "mib_table.h"


#include "mib_Ontg.h"
#include "mib_Ont2g.h"
#include "mib_OntData.h"
#include "mib_SWImage.h"
#include "mib_Cardholder.h"
#include "mib_CircuitPack.h"

/*9.2.x*/
#include "mib_Anig.h"
#include "mib_Tcont.h"
#include "mib_Scheduler.h"
#include "mib_PriQ.h"
#include "mib_GemPortCtp.h"
#include "mib_GemIwTp.h"
#include "mib_MultiGemIwTp.h"
#include "mib_GemTrafficDescriptor.h"

/*9.3.x*/
#include "mib_MacBriServProf.h"
#include "mib_MacBriPortCfgData.h"
#include "mib_VlanTagOpCfgData.h"
#include "mib_VlanTagFilterData.h"
#include "mib_Map8021pServProf.h"
#include "mib_MacBridgePortPmMonitorHistoryData.h"

/*9.5.x*/
#include "mib_EthPmHistoryData.h"
#include "mib_GemTrafficDescriptor.h"
#include "mib_ThresholdData1.h"
#include "mib_GemTrafficDescriptor.h"
#include "mib_GalEthProf.h"
#include "mib_OltG.h"
#include "mib_GemTrafficDescriptor.h"
#include "mib_ThresholdData2.h"
#include "mib_EthPmHistoryData2.h"
#include "mib_GemTrafficDescriptor.h"
#include "mib_EthPmHistoryData3.h"
#include "mib_ExtVlanTagOperCfgData.h"

#include "mib_McastOperProf.h"
#include "mib_McastSubConfInfo.h"
#include "mib_McastSubMonitor.h"
#include "mib_VEIP.h"

/*9..x*/
#include "mib_EthUni.h"

/*others*/
#include "mib_LoIdAuth.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef enum {

	AVL_KEY_PPTPUNI,
	AVL_KEY_VEIP,
	AVL_KEY_EXTVLAN_UNI,
	AVL_KEY_VLANTAGFILTER_UNI,
	AVL_KEY_VLANTAGOPCFG_UNI,
	AVL_KEY_MACBRIPORT_UNI,
	AVL_KEY_MACBRISERVPROF,
	AVL_KEY_MACBRIPORT_ANI,
	AVL_KEY_EXTVLAN_ANI,
	AVL_KEY_VLANTAGOPCFG_ANI,
	AVL_KEY_VLANTAGFILTER_ANI,
	AVL_KEY_MAP8021PSERVPROF,
	AVL_KEY_GEMIWTP,
	AVL_KEY_MULTIGEMIWTP,
	AVL_KEY_GEMPORTCTP,
	AVL_KEY_PRIQ,
	AVL_KEY_TCONT

}MIB_AVL_KEY_T;



/*for maintain ME relationship*/
typedef struct mib_tree_data_s
{
	MIB_TABLE_INDEX	  tableIndex;
	MIB_AVL_KEY_T key;
	LIST_HEAD(treeNodeHead,mib_entry_s) treeNodeHead;
}MIB_TREE_DATA_T;

typedef struct mib_node_s
{
	MIB_TREE_DATA_T data;
	struct mib_node_s *rChild;
	struct mib_node_s *lChild;	
}MIB_NODE_T;


typedef enum {
	OMCI_TRAF_MODE_FLOW_BASE,
	OMCI_TRAF_MODE_8021P_BASE,
} OMCI_TRAF_MODE_T;


typedef enum {
	OMCI_CONN_STATE_COMMON,
	OMCI_CONN_STATE_NEW,
	OMCI_CONN_STATE_UPDATE,
	OMCI_CONN_STATE_DEL,
	
}OMCI_CONN_STATE_T;

typedef struct mib_tree_conn_s
{
	OMCI_TRAF_MODE_T						traffMode; /*flow base or 1p base*/
	MIB_TABLE_MACBRIPORTCFGDATA_T			*pAniPort;
	MIB_TABLE_MACBRIPORTCFGDATA_T			*pUniPort;
	MIB_TABLE_ETHUNI_T						*pEthUni;	
	MIB_TABLE_MAP8021PSERVPROF_T			*p8021Map;	
	MIB_TABLE_VLANTAGFILTERDATA_T			*pVlanTagFilter;
	MIB_TABLE_VLANTAGOPCFGDATA_T			*pVlanTagOpCfg;
	MIB_TABLE_EXTVLANTAGOPERCFGDATA_T		*pExtVlanCfg;
	MIB_TABLE_MULTIGEMIWTP_T				*pMcastGemIwTp;
	MIB_TABLE_GEMIWTP_T						*pGemIwTp[8];
	MIB_TABLE_GEMPORTCTP_T					*pGemPortCtp[8];
	OMCI_CONN_STATE_T						state;
	LIST_HEAD(usRuleHead,omci_traff_rule_s)	usRuleHead[8];	
	LIST_HEAD(dsRuleHead,omci_traff_rule_s) dsRuleHead[8];
	LIST_ENTRY(mib_tree_conn_s) 			entries;
	
}MIB_TREE_CONN_T;

typedef struct mib_tree_s
{
	
	MIB_NODE_T *root;
	LIST_HEAD(connHead,mib_tree_conn_s) conns; 
	LIST_ENTRY(mib_tree_s) entries;
	
}MIB_TREE_T;

typedef struct mib_forest_s{
	int treeCount;
    	LIST_HEAD(avlhead,mib_tree_s) treeHead;	
}MIB_FOREST_T;


MIB_FOREST_T forest;

#define avlTreeGetMax(a,b) (((a) > (b)) ? (a) : (b)) 





#ifdef __cplusplus
}
#endif


#endif /*__MIB_TREE_H__*/

