/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  vos_config.h
*
* DESCRIPTION: 
*	
*
* Date Created: Oct 31, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/vos/linux/h/vos_config.h#1 $
* $Log:$
*
*
**************************************************************************/
 
#ifndef __VOS_CONFIG_H_
#define __VOS_CONFIG_H_

#include "vos_hash.h"
#include "lw_list.h"
//#include "config_oid.h"


#ifdef __cplusplus
extern "C" {
#endif
#define vosStrLen 		strlen
#define vosSafeStrNCpy 	strncpy
#define vosPrintk       printf
#define vosPrintf
//#define vosStrDup       strdup
#define TAB 0x09
#define VOSCONFIG_SHMLOCKFILE "/etc/.vosconfiglock.tmp"
#define VOSCFG_SHM_KEY 0x6666
#define VOSCFG_SHM_SIZE  (2*16*64*1024)



#define CONFIG_TMP_FILE "/var/run/.tmpnewcfg"
#define CONFIG_TMP2_FILE "/var/run/.tmpnewcfg2"
//#define CONFIG_DEFAULTFILE "/etc/defaultcfg.txt"
#define CONFIG_DEFAULTFILE "/etc/config/startup"

#define CONFIG_MD5_TMPFILE "/var/run/cfgmd5"
#define CONFIG_DEFAULTSTART "/var/run/startup"
#define CONFIG_CURRENT_FILE "/var/run/current"
#define CONFIG_MNT_START		"/mnt/startup"

#define CONFIG_VIRTUAL_FILE  "/var/run/virtual"
#define CONFIG_NORMAL_FLIE  "/var/run/normal"

#define MD5_LEN      32
//#define PRODUCT_NAME "EPN"
#define CONFIG_CUST_DEVELOP_VER_STR "104"






/*--------------------------Macro definition------------------------- */
#define CFG_NOT_FLUSHED   0
#define CFG_FLUSHED       1


#define CFG_TYPE_INT    0
#define CFG_TYPE_STR    1
#define CFG_TYPE_IPV4     2
#define CFG_TYPE_IPV6   3    

typedef enum
{
    CFG_OID_TYPE_SINGLE=0,
    CFG_OID_TYPE_TABLE   
}CFG_OID_TYPE_E;

typedef struct  tagCFG_OID_REGINFO_S
{
    unsigned int  uiCmoid;
    char  name[256];
    int iMax;/*最大值*/
    int iMin;/*最小值*/
    int iRestore;/*能否被恢复出厂,0表示可以*/
    CFG_OID_TYPE_E enNode_type ;/*NODE_TYPE_SINGLE表示非实例,NODE_TYPE_TABLE表示实例*/
    int ( *CHECK_FUN_PF)(unsigned int uiCmoid,void * val);/*检查处理*/
	int type;
}CFG_OID_REGINFO_S;
#define TRUE 1
#define FALSE 0


/*-----------------global varible/function declaration--------------- */

typedef struct CFG_KEY_DATA_s {
    unsigned int      flag; /* Used for duplicate checking and flushing */
    char        *data;
}CFG_KEY_DATA_t;

typedef struct CFG_SECTION_DATA_s {
     unsigned int      flag; /* Used for duplicate checking and flushing */
    HASH_TABLE_t   *keys;
}CFG_SECTION_DATA_t;

typedef struct CFG_MODULE_DATA_s {
	unsigned int      flag; 
    HASH_TABLE_t   *sections;
} CFG_MODULE_DATA_t;
typedef struct CFG_IFM_DATA_s {
	unsigned int      flag; 
    HASH_TABLE_t   *modules;
}CFG_IFM_DATA_t;

typedef struct CFG_OID_REG_S { 
	struct list_head list;
    CFG_OID_REGINFO_S *data;	
} CFG_OID_REG_t;


typedef struct tagCFG_DATABASE_S
{	
	HASH_TABLE_t *pstConfigDatabase[CONFIG_MAX_DOMAINS];
	int commitflag[CONFIG_MAX_DOMAINS];
	int flag;
} CFG_DATABASE_S;
/*--------------------------Macro definition------------------------- */

#define OP_VOS_CONFIG_STR_MAX       1014/* Max len of "NAME=VALUE\0" */







#if 0
int  vosConfigInit(void);
int  vosConfigDestroy(void);

void vosConfigShow(CFG_DOMAIN_E enDomain,int fd);
int vosConfigValueGet(
	CFG_DOMAIN_E enDomain,
	char *ifm,
    char *fname,
    const char *section,
    char *key,
    char *default_value,
    void *result,
    int retlen);

int vosConfigValueSet(
	CFG_DOMAIN_E enDomain,
	char *ifm,
    char *fname,
    const char *section,
    char *key,
    const char *value);
int vosConfigModuleDelete(CFG_DOMAIN_E enDomain,char* ifm,char *fname);

int vosConfigSectionDelete(CFG_DOMAIN_E enDomain,char* ifm,char *fname, const char *section);
int vosConfigKeyDelete(CFG_DOMAIN_E enDomain,char* ifm,char *fname, char *section, char *key);
int vosConfigSectionCreate(
	CFG_DOMAIN_E enDomain,
	char *ifm,
    char *fname,
    const char *section);
int vosConfigSectionGetByIndex(	CFG_DOMAIN_E enDomain,char *ifm,char *fname, int index, char **section);
#endif
 
#ifdef __cplusplus
}
#endif
 
#endif /* #ifndef __VOS_CONFIG_H_ */
 
