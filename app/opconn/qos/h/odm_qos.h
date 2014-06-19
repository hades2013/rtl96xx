#ifndef  __ODM_QOS_H__
#define __ODM_QOS_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "list_opconn.h"
//wfxi #include "classify.h"
#include "opconn_usr_ioctrl.h"
#include "qos.h"

typedef enum {
    QOS_SP_MODE = 0,	/**<  strict priority scheduling mode */
    QOS_WRR_MODE,		/**<   weight round robin scheduling mode*/
    QOS_MIX_MODE,		/**<  sp and wrr mixed scheduling mode */
    QOS_PLUS_MODE,  /**<  sp and wrr mixed plus scheduling mode */
    QOS_MODE_MAX
} odm_qos_schedmode_t;

#define MAX_QOS_QUEUE 7
#define MIN_QOS_QUEUE 0

#define MAX_QOS_COS 7
#define MIN_QOS_COS 0

typedef enum{
CTC_RULE_NEVER_MATCH=0x0,
CTC_RULE_MATCH,
CTC_RULE_NOT_EQUAL,
}odm_ctc_rule_t;

#define ACL_SATE_INVALID 0x0000
#define ACL_STATE_ACTIVE    0x0001
#define ACL_STATE_READY  0x0002

#define MAX_ACL_NAME 32
#define ACL_MAC_LEN 6

#define ACL_MAX_PRI 8

typedef struct {
    UINT8 uc[ACL_MAC_LEN];
} opl_mac_addr_t;
typedef UINT32 opl_ipv4_addr_t;
typedef UINT32 opl_cls_flag_t[2];
typedef UINT32 opl_cls_action_t;

#define OPL_FIELD_FLG_SET(flag, field)\
    ((flag[(field) / 32]) |= (0x1UL << ((field) % 32)))

#define OPL_FIELD_FLG_CLR(flag, field)\
    ((flag[(field) / 32]) &= (~(0x1UL << ((field) % 32))))

#define OPL_FIELD_FLG_TST(flag, field)\
    (((flag[(field) / 32]) & (0x1UL << ((field) % 32))) ? 1 : 0)

typedef struct opl_cls_sw_s
{
    opl_cls_flag_t    field_flg;
    opl_mac_addr_t     dest_mac;
    opl_mac_addr_t     dest_mac_mask;
	opl_mac_addr_t     src_mac;
    opl_mac_addr_t     src_mac_mask;
	UINT8          cos;
    UINT8          cos_mask;
    UINT16         vid;
    UINT16         vid_mask;
    UINT16         ethtype;
    UINT16         ethtype_mask;
    opl_ipv4_addr_t      src_ipv4;
    opl_ipv4_addr_t      src_ipv4_mask;
    opl_ipv4_addr_t      dst_ipv4;
    opl_ipv4_addr_t      dst_ipv4_mask;
	UINT8          ipv4_proto;
    UINT8          ipv4_proto_mask;
	UINT8          ipv4_dscp;
    UINT8          ipv4_dscp_mask;
    UINT16         src_l4port;
    UINT16         src_l4port_mask;
    UINT16         dst_l4port;
    UINT16         dst_l4port_mask;
	opl_cls_action_t  action_flag;
    UINT16            rmkvid;
    UINT8             cosmap;
    UINT8             queuemap; 	
}opl_cls_sw_t;
#if 0
typedef struct opl_acl_list_s{
    struct list_head list;
    UINT16 ruleid;/*rule id */
    UINT16 tcnum;/*total list number*/
    UINT8 prinum;/*priority acl number*/
    UINT8 rulepri;/*0---normal acl, 1-8---oam acl */
    UINT16 dalindex;/*dal table index*/  
	opl_cls_hwtbl_t hwshadow;
	CLS_CONFIG_INFO_t stClassCfg;
    UINT8 status;
	UINT8  name[MAX_ACL_NAME];/*rule name ,independent*/
} opl_acl_list_t;
#endif

int odmQosCos2QMapSet(UINT8 VlanCos,UINT8 QueId);
int odmQosCos2QMapGet(UINT8 VlanCos, UINT8 *QueId);
int odmQosSchedModeGet(UINT32 portId, UINT32 *pSchedMode);
int odmQosSchedModeSet(UINT32 portId, UINT32 pSchedMode);
int odmQosPortPriRemapGet(UINT32 portId, UINT32 OldVlanCos,UINT32 *NewVlanCos);
int odmQosPortPriRemapSet(UINT32 portId, UINT32 OldVlanCos,UINT32 NewVlanCos);
int odmQosPortPriRemapStatusSet(UINT32 portId,UINT32 Status);
int odmOamClassInstall(UINT16 portId, UINT8 precedence,CLS_CONFIG_INFO_t *stClassCfg);
int odmOamClassRemove(UINT16 portId,UINT8 precedence);
int odmQosConfigDump(int fd,UINT16 portId,int flag);
int OdmAclListShowWithRulePre(int fd);
int OdmAclListDelLowestPre();
opl_acl_list_t * OdmAclListFindPre(UINT8 precedence);
int OdmAclListAdd(UINT16 ucPortS,CLS_CONFIG_INFO_t *stClassCfg);
int OdmAclListDel(UINT16 ucPortS,UINT8 precedence);
int odmCheckQosBlockExist(char *name);
extern int odmCreateClass(char *name);
extern int odmCfgClassFVid(char *name, UINT16 fvid, CLASS_ACT act);
extern int odmCfgClassRmkVid(char  *name,TRANF_ACT t_act,UINT16 fvid,CLASS_ACT act );
extern int odmInstallClassEntry(char  *name,UINT8 stream,char* ports);
extern int odmRemoveClassEntry(char  *name,UINT8 stream);
extern int odmDestroyClass(char *name);
#ifdef __cplusplus
}
#endif

#endif
