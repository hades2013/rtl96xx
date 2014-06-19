/*****************************************************************************
                                                                             
*****************************************************************************/
#ifndef _IF_PUB_H_
#define _IF_PUB_H_

#ifdef  __cplusplus
extern "C"{
#endif
#include <lw_config.h>
#if defined(__KERNEL__)
#include <linux/list.h>
#else
#include <lw_list.h>
#endif
#include <lw_drv_pub.h>
//#include "error"

/* ifIndex define : | sub_type(8bit) |domain(3) |delfag(2)|reserved(11bit) | index(8bit) |*/
typedef unsigned int ifindex_t;
typedef enum tagIfdomain
{
    IF_DOMAIN_NORMAL=0, 
    IF_DOMAIN_DEFAULT, 
    IF_DOMAIN_VIRTUAL,
    IF_DOMAIN_ATOM,   
    IF_DOMAIN_ALL
}IF_DOMAIN_E;
#define IF_FLAG_MASK            0x3
#define IF_FORCE_DELETE_FLAG    0x1
#define IF_NO_COMMIT_FLAG       0x2
#define IF_SUB_TYPE_MASK        0xff
#define IF_SUB_MAXIFCOUNT       256
#define IF_SUB_TYPE(ifidx)      (IF_SUB_TYPE_E)(((ifidx)>>24) & IF_SUB_TYPE_MASK)
#define IF_INDEX_MASK           0xff
#define IF_INDEX(ifidx)         ((ifidx) & IF_INDEX_MASK)
#define IF_DOMAIN_MASK          0x7
#define IF_DOMAIN(ifidx)        (IF_DOMAIN_E)(((ifidx)>>21)&IF_DOMAIN_MASK)
#define IF_IFINDEX(_sub_type, _index)   ((((UINT)_sub_type)<<24)|(_index&IF_INDEX_MASK))
#define IF_ADD_FALG(ifidx,flag) ((ifidx)|(((flag)&IF_FLAG_MASK)<<19))
#define IF_GET_FLAG(ifidx)      (((ifidx)>>19)&IF_FLAG_MASK)
#define IF_CLR_FLAG(_ifidx)     ((_ifidx) & (~((IF_FLAG_MASK)<<19)))
#define IF_INVALID_IFINDEX      (ifindex_t)(-1)
#define IF_DOMAIN_IFINDEX(_sub_type,domain,_index) ((((UINT)_sub_type)<<24)|(((UINT)domain)<<21)|(_index&IF_INDEX_MASK))
#define IFINDEX_ADDDOMAIN(_ifidx, _domain)  (((_ifidx) & (~(((ifindex_t)(IF_DOMAIN_MASK))<<21))) | (((ifindex_t)(IF_DOMAIN_MASK & (_domain)))<<21))

typedef enum tagConnection_type
{
        CT_PUREBRIDGE = 0,
        CT_STATIC = 1,
        CT_DHCP = 2,
        CT_PPPOE = 3,
        CT_DISABLE = 4,
        CT_PPPOA = 5,
        CT_IPOA = 6,
        CT_3G   = 7 , 
        CT_UNDEFINE = 8,
        CT_MAX
}CONNECTION_TYPE_E;




typedef enum tagIf_Attr
{
    IF_PSEUDONAME=0,/*alias NAME:wan1,wan2,lan1,lan2,lan3,lan4*/
    IF_NAME, /*fixed.IF NAME :ethport0,eth0,ethsub0.1 ipsec0,l3vlan1,vlan1*/
    IF_PARENT,
    IF_PHYID,
    IF_DUPLEX,
    IF_SPEED,
    IF_STATE,
    IF_ATTRENABLE,
    IF_MAC,
    IF_TYPE,
    IF_FIBER_INFO_FLAG,
    IF_FIBER_INFO,
    IF_IP,
    IF_MASK,
    IF_MTU,
    IF_GATEWAY,
    IF_LINKNAME,/*netname eth0 eth0.1*/
    IF_NETNAME,/*ppp0*/  
    IF_DNSPRI_DYN,
    IF_DNSSLV_DYN,
    IF_WORKMODE,
    IF_REFCNT,
    IF_MONITOR_STATE,
    IF_ATTR_ALL
}IF_ATTR_E;
#define IF_WORKMODE_BRIDGE    2
#define IF_WORKMODE_ROUTER   1
#define IF_WORKMODE_PHYBASE      0  /* physical & base port */

#define VALID_IF_ATTR_ID(_attr_id)  ((_attr_id) < IF_ATTR_ALL)


#define CT_PUREBRIDGE_STR "bridge"
#define CT_STATIC_STR   "static"
#define CT_DHCP_STR "dhcp-alloc"
#define CT_PPPOE_STR "pppoe-alloc"
#define CT_DISABLE_STR "dial-disable"
#define CT_PPPOA_STR "pppoa-alloc"
#define CT_IPOA_STR "ipoa"
#define CT_3G_STR  "3G"
#define CT_UNDEFINE_STR  "dial-undefine"

/* sub type define */
typedef enum tagIf_Sub_type {
  IF_SUB_ROOT = 0,
  IF_SUB_ETH,
  IF_SUB_ETHSUB,
  IF_SUB_ETHPORT,
  IF_SUB_ATM,
  IF_SUB_LAG,
  IF_SUB_L3VLAN,
  IF_SUB_IPSEC,
  IF_SUB_3G,
  IF_SUB_PHY_ALL,       /*在挂全局事件时使用*/
  IF_SUB_TYPE_COUNT
}IF_SUB_TYPE_E;
#define TEMPINDEX 0xff
#define NOSAVEINDEX 0xfe
#define IF_ROOT_IFINDEX    IF_IFINDEX(IF_SUB_ROOT,0)    /*根接口，保存与接口不相关的配置*/
#define IF_NOSAVE_IFINDEX    IF_IFINDEX(IF_SUB_ROOT,NOSAVEINDEX)
#define IF_TEMP_IFINDEX     IF_NOSAVE_IFINDEX

#define IF_OBJ_SUB_TYPE_MAX  IF_SUB_PHY_ALL
#define IF_DESC_LEN          81
/* PhysicalId define */
#define IF_LOGIF_PHYID  (-1)   /*在创建上层接口时使用，没有对应的physical id*/
#define ISL3INTF(ifindex)  ((IF_SUB_TYPE(ifindex)==IF_SUB_L3VLAN)||\
                             (IF_SUB_TYPE(ifindex)==IF_SUB_IPSEC)||\
                             (IF_SUB_TYPE(ifindex)==IF_SUB_ETH)||\
                             (IF_SUB_TYPE(ifindex)==IF_SUB_ETHSUB))
                             
#define VALID_SUB_TYPE(_subType)  ((IF_SUB_TYPE_E)(_subType) < IF_OBJ_SUB_TYPE_MAX)
#define VALID_IFINDEX(_ifidx)  (VALID_SUB_TYPE(IF_SUB_TYPE((_ifidx))) && \
    (0 == ((_ifidx) & (~ IF_ADD_FALG(IF_DOMAIN_IFINDEX(IF_SUB_TYPE_MASK,IF_DOMAIN_MASK,IF_INDEX_MASK),IF_FLAG_MASK)))))

#define IFNAMESIZE  16
#define MACADDRSIZE  18
#define IFNAMSIZ IFNAMESIZE
/*配置属性成员组表*/
typedef struct tagIF_ATTR
{
    IF_ATTR_E enAttrType;
    CHAR *    pcDesp;
    UINT      uiDataLen;    
}IF_ATTR_S;

typedef enum tagIF_ENABLE
{
    IF_MOD_DISABLE=0,
    IF_MOD_ENABLE
}IF_ENABLE_E;
#define IF_MOD_ENABLE_STR ENABLE_STR
#define IF_MOD_DISABLE_STR    DISABLE_STR

typedef enum tagIF_STATE
{
    IFM_INIT = -1, 
    IFM_PHYLINKDOWN = 0,       /* see also: NETLINK_DOWN , WAN_DISABLE, WAN_STOP */
    IFM_NOIPGET,               /* Currently, the ip has not been got. In dhcp mode, the renew request has sent, but no respond yet.*/
    IFM_LINKINVALID,           /* The link is not valid according linkspy's report. */
    IFM_LINKVALID,             /* The link is valid according linkspy's report . Or the linkspy function is disable it will report directly */
    IFM_MAX
}IF_STATUS_E;

#define IF_LGC_STATE_E IF_STATUS_E

/* 
 * Use this structure to build a interface.
 */
typedef struct tagIf_Info {
    INT iPhysicalId;                    /* in drv lever the iPhysicalId is logic port*/
    IF_SUB_TYPE_E enSubType;
    ifindex_t uiParentIfindex;
    ifindex_t uiSpecIndex;              /* specify ifindex if needed, otherwise set it to be IF_INVALIDINDEX */
    CHAR szPseudoName[IFNAMESIZE];      /* pseudo name seen by outer */
    CHAR szIfName[IFNAMESIZE];   
}IF_INFO_S;

#define IF_INTFINFO_PHYID(x)    ((x).iPhysicalId)
#define IF_INTFINFO_PHYTYPE(x)    ((x).enSubType)
#define IF_INTFINFO_PSEUDO(x)    ((x).szPseudoName)
#define IF_INTFINFO_PARENT(x)    ((x).uiParentIfindex)
#define IF_INTFINFO_SPEC(x)    ((x).uiSpecIndex)

/*在连续的子类型中循环*/
#define ForInRangeSubType(_subType, _start, _end)    \
    for((_subType)=(INT32)(_start); (_subType)<=(_end); (_subType)++)




/*定义接口模块的返回值*/
typedef enum tagIf_Return_Value
{
    IF_OK = 0,                          /* ok */
    IF_NFOUND,                          /* not found interface */
    IF_ALREADY_MAX_NUM,
    IF_ALREADY_EXIST,
    IF_INVALID_IDX,                     /* invalid ifindex */
    IF_ERR_PARAM,                       /* parameter check error */
    IF_ERR_TYPE,                        /* type error */
    IF_ERR_NO_MEM,                      /* no memory */
    IF_ERR_PANIC,                       /*  panic error */
    IF_ERR_MSG,                         /* msg queue error */
    IF_ERR_MSG_TYPE,
    IF_ERR_COMMIT_NONE,                 /* commit with nothing */
    IF_NFUND_REG_FUNC,                  /* registered function is null */
    IF_INIT_ERR,                        /* init function error */
    IF_ERR_ATTRID,                      /* invalid interface attribute id */
    IF_INVALID_DATA,                    /* invalid interface's data */
    IF_CREATE_ERR,                      /* create interface error */
    IF_DELETE_ERR,                      /* delete interface error */
    IF_CHILD_EXIST,                     /* when child exist the interface should not remove*/
    IF_ERR_DEL_DATA,                    
    IF_ERR_REMOVE_OBJ,
    IF_ERR_ADD_OBJ,
    IF_SHORT_BUFF_SIZE,                 /* buffer size too small */
    IF_NO_PARENT,                       
    IF_ERR_GETATTR,                     
    IF_ERR_SETATTR,
    IF_ERR_PDNAME_USED,
    IF_ERR_NAME_USED,
    IF_NO_FREE_INDEX,
    IF_ERR_PHYID,
    IF_ERR_SPEED,
    IF_ERR_DUPLEX,
    IF_ERR_STATE,
    IF_ERR_MAC, 
    IF_ERR_FIBER_INFO_FLAG,
    IF_ERR_FIBER_INFO,
    IF_INVALID_EVENT_TYPE,              /* invalid interface event type*/
    IF_INVALID_EVENT,
    IF_ERR_SPECIFY_TYPE,
    IF_NFOUND_REG_BLOCK,
    IF_RESUME_BLOCK_NFOUND,
    IF_DESTROY_ERR,
    IF_ERR_LINKNAME,
    IF_ERR_GATEWAY,
    IF_ERR_MTU,
    IF_SETIP_ERR,
    IF_SETMASK_ERR,
    IF_ERR_ENABLE,
    IF_VLAN_NOT_EXIST,
    IF_VLAN_IF_ALREADY_EXIST,
    IF_ERR_CREAT_QUEUE,
    IF_ERR_DELETE_QUEUE,
    IF_AGGR_NOT_SUPPORT_MAC_LIMIT,
    IF_ERR_AGG_PORTINFO,
    IF_ERR_AGG_INFO,
    IF_ERR_AGG_FSMPORTINFO,
    IF_LACP_PORTADDFAIL_ALLREADYIN,
    IF_LACP_PORTIPFILTER_ENABLE,
    IF_LACP_PORTIPMACBIND_ENABLE,
    IF_ERR_AGGR_NOT_SUPPORT_MAC_LIMIT,
    IF_ERR_AGGR_INVALID_TYPE,
    IF_LACP_PORTLACPCFGERR_NOTEXIST,
    IF_ERR_AGGR_PORT_IS_MONITOR,
    IF_ERR_AGGR_MAX_MEMBER,
    IF_ERR_AGGR_PORT_IS_AGGRE,
    IF_ERR_AGGR_FE_GE_TOGETHER,
    IF_ERR_AGGR_PORT_IS_ISOLATE,
    IF_ERR_AGGR_PORT_IS_BIDING,
    IF_ERR_AGGR_PORT_IS_ON_AUTH,
    IF_ERR_ISOUSERVLAN_ASSOCIATED_NATIVE_PORT_ADD_TO_AGG,
    IF_LACP_GET_AGGPORTINFO,
    IF_LACP_GET_AGGINFO_ERR,
    IF_LACP_GETPORTMASK_ERR,
    IF_LACP_PORTNOTIN_AGGR,
    IF_LACP_CHECKPORT_ERR,
    IF_LACP_DELETEAGGPORT_ERR,
    IF_LACP_ALLOCATE_OPERKEY_ERR,
    IF_LACP_NO_AGGR_GROUP,
    IF_LACP_AGGR_GROUP_NOT_EXIST,
    IF_LACP_AGGR_GROUP_ALREADY_EXIST,
    IF_LACP_AGGR_DIFFRENT_TYPE,
    IF_GET_PAMASTERPORT_ERR,
    IF_LACP_MODIFYAGGRMODE_ERR,
    IF_LACP_SET_AGGR_MODE_ERR,
    IF_LACP_ENABLE_ERR,
    IF_LACP_MISTYPE,
    IF_LACP_GET_SYSPRI_ERR,
    IF_LACP_GET_SYSMAC_ERR,
    IF_LACP_GET_MASK_ERR,
    IF_ERR_MAX_NUM     /*错误类型在此之前添加*/
}IF_RET_E;


typedef enum  tagSYS_SETUP_STATE
{
    SYS_SETUP_EARLY,
    SYS_SETUP_CFGINIT,/*解析配置构建配置树阶段*/ 
    SYS_SETUP_IFINIT,/*接口初始化阶段*/    
    SYS_SETUP_LATER,
    SYS_SETUP_FINISH  /*处于此状态时,接口才可以发送事件*/   
}SYS_SETUP_STATE_E;


typedef IF_INFO_S IF_CREATE_PAYLOAD_S;

typedef struct tagIf_GetIndex_Payload{
    IF_SUB_TYPE_E enType;
    IF_DOMAIN_E  enDomain;
    ifindex_t uiIfindex; 
}IF_GETINDEX_PAYLOAD_S;

typedef struct tagIf_GetAttr_Payload{
    IF_ATTR_E enAttrId;
    ifindex_t uiIfindex; 
    UINT      uiLen;
}IF_GET_ATTR_PAYLOAD_S;

typedef struct tagIf_SetAttr_Payload{
    IF_ATTR_E enAttrId;
    ifindex_t uiIfindex; 
    UINT      uiLen;
    //UCHAR     data[0];
    union
    {
            UCHAR *paddr;
            UINT64 pad;//point value for 32 64 bits
    }data;
}IF_SET_ATTR_PAYLOAD_S;

typedef struct tagIf_Get_Payload{
    CHAR szPseudoName[IFNAMESIZE];      /* pseudo name seen by outer */
    CHAR szIfName[IFNAMESIZE];      
    IF_DOMAIN_E enDomain;
}IF_GET_PAYLOAD_S;

typedef struct tagIF_DEL_PAYLOAD
{
    ifindex_t ifindex; 
}IF_DEL_PAYLOAD_S;

typedef struct tagIF_TEST_PAYLOAD
{
    ifindex_t ifindex; 
}IF_TEST_PAYLOAD_S;


typedef struct tagIF_SYSSTATE_PAYLOAD
{
    SYS_SETUP_STATE_E enState; 
}IF_SYSSTATE_PAYLOAD_S;
typedef struct tagIF_ATTACH_PAYLOAD
{
    ifindex_t uiIfindex;
    ifindex_t uiPifindex;
}IF_ATTACH_PAYLOAD_S;
typedef struct tagIF_Register_Block_S
{    
    IF_RET_E  (*init)(ifindex_t ifindex,VOID *para);
    IF_RET_E  (*destroy)(ifindex_t ifindex);  
    VOID * (*alloc)(IF_SUB_TYPE_E iftype); 
    VOID  (*free)(CONST VOID *p); 
    IF_RET_E  (*attribute)(ifindex_t uiIfindex,INT iAttrId, VOID * pValue ,UINT iLen);
    IF_RET_E  (*setattr)(ifindex_t uiIfindex, INT iAttrId,VOID * pValue,UINT uiLen );
}IF_REGISTER_BLOCK;

typedef enum tagIfmcmd{
    IF_CMD_CREATE,
    IF_CMD_DELETE,
    IF_CMD_TEST,
    IF_CMD_GETATTR,
    IF_CMD_GETBYPSNAME,
    IF_CMD_GETNEXTINDEX,
    IF_CMD_GETBYIFNAME,
    IF_CMD_GETFIRSTINDEX,
    IF_CMD_SETATTR,
    IF_CMD_SETSYSSTATE,
    IF_CMD_GETSYSSTATE,
    IF_CMD_ATTACH,
    IF_CMD_MAX
}IFM_CMD_E;
typedef struct tagIfmReq{
    IFM_CMD_E cmd;
    IF_RET_E err;
    union {
        IF_CREATE_PAYLOAD_S   ifm_creatpld; 
        IF_DEL_PAYLOAD_S      ifm_delpld;
        IF_TEST_PAYLOAD_S     ifm_testpld;
        IF_GET_ATTR_PAYLOAD_S ifm_getattrpld;
        IF_GETINDEX_PAYLOAD_S ifm_getindexpld;
        IF_SET_ATTR_PAYLOAD_S ifm_setattrpld;  
        IF_SYSSTATE_PAYLOAD_S ifm_sysstatepld;
        IF_ATTACH_PAYLOAD_S   ifm_attach;
        IF_GET_PAYLOAD_S       ifm_getpld;        
    }inputpld;
    #define m_ifm_creatpld   inputpld.ifm_creatpld
    #define m_ifm_delpld     inputpld.ifm_delpld
    #define m_ifm_testpld    inputpld.ifm_testpld
    #define m_ifm_getattrpld inputpld.ifm_getattrpld
    #define m_ifm_getindexpld inputpld.ifm_getindexpld
    #define m_ifm_setattrpld  inputpld.ifm_setattrpld
    #define m_ifm_sysstatepld inputpld.ifm_sysstatepld
    #define m_ifm_attach      inputpld.ifm_attach  
    #define m_ifm_getpld    inputpld.ifm_getpld
    union {
        IF_GETINDEX_PAYLOAD_S  ifm_getidxpld;
        IF_SYSSTATE_PAYLOAD_S  ifm_sysgetstate;
        union
        {
            UCHAR * paddr;
            UINT64 pad;//point value for 32 64 bits
        }pdata;
    }outputpld;
    #define m_ifm_createretpld outputpld.ifm_getidxpld
    #define m_ifm_getattrretpld   outputpld.pdata.paddr
    #define m_ifm_getsysstate    outputpld.ifm_sysgetstate  
    #define m_ifm_getbypsnamepld  outputpld.ifm_getidxpld;
    
}IFM_REQ_S;


#define DEFINE_IF_REGISTER(a) STATIC IF_REGISTER_BLOCK a

//#define IFM_DEBUG 0

#define IFM_DBG_ERR(fmt,arg...) 
#define IFM_DBG_INFO(fmt,arg...)
#if defined(__KERNEL__)
IF_RET_E K_IF_CreatInterface( IN IF_INFO_S * pstInfo, OUT ifindex_t * puiIfIndex );
IF_RET_E K_IF_DeleteInterface( IN ifindex_t uiIfIndex );
IF_RET_E K_IF_GetAttr( IN ifindex_t uiIfindex, IN IF_ATTR_E iAttrId, INOUT VOID * pValue ,IN UINT iLen);
IF_RET_E K_IF_GetNextIfindex( IN IF_SUB_TYPE_E enSubType,INOUT ifindex_t * puiIfindex );
IF_RET_E K_IF_GetByIFNameDomain( IN CHAR * name, IN IF_DOMAIN_E enDomain,OUT ifindex_t * puiIfIndex );
#define  K_IF_GetByIFName(name, puiIfIndex ) K_IF_GetByIFNameDomain(name,IF_DOMAIN_NORMAL,puiIfIndex)
IF_RET_E K_IF_GetByPseudoNameDomain( IN CHAR * name,IN IF_DOMAIN_E enDomain, OUT ifindex_t * puiIfIndex );
#define  K_IF_GetByPseudoName(name, puiIfIndex ) K_IF_GetByPseudoNameDomain(name,IF_DOMAIN_NORMAL,puiIfIndex)
IF_RET_E K_IF_GetFirstIfindexByDomain( IN IF_SUB_TYPE_E enSubType,IN IF_DOMAIN_E enDomain, OUT ifindex_t * puiIfindex );
#define  K_IF_GetFirstIfindex(enSubType,puiIfindex) K_IF_GetFirstIfindexByDomain(enSubType,IF_DOMAIN_NORMAL,puiIfindex)

IF_RET_E K_IF_SetAttr( IN ifindex_t uiIfindex, IN IF_ATTR_E iAttrId, IN VOID * pValue,IN UINT uiLen );
IF_RET_E K_IF_TestIfExist(IN ifindex_t uiIfindex);
IF_RET_E K_IF_AttachParent(IN ifindex_t uiIfindex,IN ifindex_t uiPifindex);

#else
IF_RET_E IF_CreatInterface( IN IF_INFO_S * pstInfo, OUT ifindex_t * puiIfIndex );
IF_RET_E IF_DeleteInterface( IN ifindex_t uiIfIndex );
IF_RET_E IF_GetAttr( IN ifindex_t uiIfindex, IN IF_ATTR_E enAttrId, INOUT VOID * pValue ,IN UINT uiLen);
IF_RET_E IF_GetNextIfindex( IN IF_SUB_TYPE_E enSubType,INOUT ifindex_t * puiIfindex );
IF_RET_E IF_GetByPseudoNameDomain( IN CHAR * name, IN IF_DOMAIN_E enDomain,OUT ifindex_t * puiIfIndex );
#define  IF_GetByPseudoName(name,puiIfIndex) IF_GetByPseudoNameDomain(name,IF_DOMAIN_NORMAL,puiIfIndex)
IF_RET_E IF_GetFirstIfindexByDomain( IN IF_SUB_TYPE_E enSubType,IN IF_DOMAIN_E enDomain, OUT ifindex_t * puiIfindex );
#define  IF_GetFirstIfindex(enSubType,puiIfindex) IF_GetFirstIfindexByDomain(enSubType,IF_DOMAIN_NORMAL,puiIfindex)
IF_RET_E IF_SetAttr( IN ifindex_t uiIfindex, IN IF_ATTR_E iAttrId, IN VOID * pValue,IN UINT uiLen );
IF_RET_E IF_TestIfExist(IN ifindex_t uiIfindex);
IF_RET_E IF_SetSysState(SYS_SETUP_STATE_E enState);
IF_RET_E IF_GetSysState(SYS_SETUP_STATE_E *enState);

INT32 IF_ExistInterfaceInit(void);
IF_RET_E IF_SetPhysicalId(IN ifindex_t uiIfindex, IN UINT32 uiPid);
IF_RET_E IF_GetPhysicalId(IN ifindex_t uiIfindex, OUT UINT32 * puiPid);
IF_RET_E IF_SetPseudoName(IN ifindex_t uiIfindex, IN CHAR * pcName);
IF_RET_E IF_GetByIFNameDomain( IN CHAR * name, IN IF_DOMAIN_E enDomain,OUT ifindex_t * puiIfIndex );
#define IF_GetByIFName( name, puiIfIndex ) IF_GetByIFNameDomain(name,IF_DOMAIN_NORMAL,puiIfIndex)
IF_RET_E IF_L2Up(IN ifindex_t uiIfindex);
IF_RET_E IF_AttachParent(IN ifindex_t uiIfindex,IN ifindex_t uiPifindex);
IF_RET_E IF_L3IfMonitor(VOID);
IF_RET_E IF_GetCliName(IN ifindex_t uiIfindex,INOUT CHAR *pszName,IN UINT uiSize);
IF_RET_E IF_Userstr2ifame(INOUT CHAR *str,INOUT CHAR *pszName,IN UINT uiSize);
IF_RET_E IF_GetWebName(IN ifindex_t uiIfindex,INOUT CHAR *pszName,IN UINT uiSize);

#endif

/*判断接口接口状态*/
#define IF_UP(s)  ((s)>=IFM_NOIPGET)
#define LINE_PROTOCOL_UP(s) ((s)== IFM_LINKVALID)

#define IP_ZERO "0.0.0.0"

/* 
 * Use this structure to build a vlan interface.
 */
#define IF_ETH_MAX  1
#define IF_L3VLAN_MAX  3
#define IF_ROOT_MAX  3
//#define IF_SUB_VLANRANGE1 
#define IF_ETHPORT_MAX 48
typedef struct tagIf_GetVlanList_Payload{
    IF_DOMAIN_E  enDomain;
}IF_GETVLANLIST_PAYLOAD_S;

#define IF_L3VLAN_NAMEPREFIX "l3vlan"
#define IF_ETH_NAMEPREFIX "eth"
#define IF_ETHPORT_NAMEPREFIX "ethport"

#define IF_TMP_NAME "tmproot"
#define IF_L3VLAN_BASE "eth0"
#define CLI_GIGAPORTPREFIX "gigabitethernet"
#define CLI_FASTPORTPREFIX "ethernet"
#define CLI_CABLE "cable"

#define WEB_GIGAPORTPREFIX  "GE"
#define WEB_FASTPORTPREFIX  "FE"
#define WEB_CABLEPORTPREFIX "CAB"
#define IF_AGGR_NAMEPREFIX  "lag"
#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif  /* end of _IF_PUB_H_ */
