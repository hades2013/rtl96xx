#ifndef HAL_CLS_H
#define HAL_CLS_H

#pragma pack (1)

/*l2 parameter*/
typedef struct HAL_CLS_MAC_CFG_PAR_s
{
    UINT16 classItemID;
    UINT8  *mac;/*mac string "xx:xx:xx:xx:xx:xx"*/;
    UINT8  *macmask;
    UINT8  enable;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_MAC_CFG_PAR_t;

typedef struct HAL_CLS_COS_CFG_PAR_s
{
    UINT16 classItemID;
    UINT8  cos :3;
    UINT8  cosmask:3;
    UINT8  pad:2;
    UINT8  enable;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_COS_CFG_PAR_t;

typedef struct HAL_CLS_VID_CFG_PAR_s
{
    UINT16 classItemID;
    UINT16 vid;
    UINT16 vidmask;
    UINT8  enable;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_VID_CFG_PAR_t;

typedef struct HAL_CLS_TYPELEN_CFG_PAR_s
{
    UINT16 classItemID;
    UINT16 typelen;
    UINT16 typelenmask;
    UINT8  enable;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_TYPELEN_CFG_PAR_t;


/*l3 parameter*/
typedef struct HAL_CLS_IP_CFG_PAR_s
{
    UINT16 classItemID;
    UINT8  *ip/*ip string xx.xx.xx.xx*/;
    UINT8  *ipmask;
    UINT8  enable;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_IP_CFG_PAR_t;

typedef struct HAL_CLS_IPTYPE_CFG_PAR_s
{
    UINT16 classItemID;
    UINT8  iptype;
    UINT8  iptypemask;
    UINT8  enable;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_IPTYPE_CFG_PAR_t;

typedef struct HAL_CLS_IPTOS_CFG_PAR_s
{
    UINT16 classItemID;
    UINT8  iptos;
    UINT8  iptosmask;
    UINT8  enable;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_IPTOS_CFG_PAR_t;

/*l4 parameter*/
typedef struct HAL_CLS_L4PORT_CFG_PAR_s
{
    UINT16 classItemID;
    UINT16 port;
    UINT16 portmask;
    UINT8  enable;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_L4PORT_CFG_PAR_t;

/*other parameter*/

typedef struct HAL_CLS_PORT_CFG_PAR_s
{
    UINT16 classItemID;
    UINT16 port;
    UINT16 portmask;
    UINT8  enable;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_PORT_CFG_PAR_t;

typedef struct HAL_CLS_COS_REMARK_CFG_PAR_s
{
    UINT16 classItemID;
    UINT8  cosremark;
    UINT8  enable;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_COS_REMARK_CFG_PAR_t;

typedef enum HAL_VID_REMARK_ACT_s
{
    INSERT_TAG_AFTER_SAMAC ,
	REMARK_VID_AS_RULE_VID ,
}HAL_VID_REMARK_ACT_e;

typedef enum HAL_TRAF_ACTION_s
{
	HAL_COPY_PACKETS_TO_CPU = 1,
	HAL_DROP_PACKETS = 2,
	HAL_DO_NOT_COPY_TO_CPU = 4,
	HAL_DO_NOT_DROP = 8
}HAL_TRAF_ACTION_e;
typedef struct HAL_CLS_VID_REMARK_CFG_PAR_s
{
    UINT16        classItemID;
    UINT16          vidremark;
    HAL_VID_REMARK_ACT_e type;
    UINT8              enable;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_VID_REMARK_CFG_PAR_t;

typedef struct HAL_CLS_TRANACT_CFG_PAR_s
{
    UINT16    classItemID;
    HAL_TRAF_ACTION_e action;
    UINT8         enable;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_TRANACT_CFG_PAR_t;

typedef struct HAL_CLS_ICOS_MAP_CFG_PAR_s
{
    UINT16  classItemID;
    UINT8   queue;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_ICOS_MAP_CFG_PAR_t;

typedef struct HAL_CLS_MASK_PRIORITY_CFG_PAR_s
{
    UINT16  classItemID;
    UINT8   pri;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_MASK_PRIORITY_CFG_PAR_t;


typedef struct HAL_CLS_DELETE_PAR_s
{
    UINT16  classItemID;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_DELETE_PAR_t;
typedef enum CLS_SHOW_TYPE_s
{
    MASK_ENTRY ,
    ADDR_ENTRY ,
    RULE_ENTRY ,
    CFG_ENTRY  ,
}HAL_CLS_SHOW_TYPE_e;
typedef struct HAL_CLS_CLS_SHOW_PAR_s
{
    UINT16  startid;
    UINT16  endid;
    HAL_CLS_SHOW_TYPE_e type;
    /*return vlaue*/
    UINT32 ret;
}HAL_CLS_CLS_SHOW_PAR_t;

#pragma pack ()

#endif

