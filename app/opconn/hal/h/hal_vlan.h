#ifndef HAL_VLAN_H
#define HAL_VLAN_H

#pragma pack (1)

typedef enum HAL_VLAN_EGRESS_TYPE_S
{
    HAL_UNMODIFY = 0,
    HAL_UNTAG    = 1,
    HAL_TAG      = 2    
}HAL_VLAN_EGRESS_TYPE_E;

typedef enum
{
    HAL_UNMODIFY_EGRESS = 0,
    HAL_UNTAGGED_EGRESS,
    HAL_TAGGED_EGRESS,
    HAL_ADD_TAG
} HAL_PORT_EGRESS_MODE;

typedef enum
{
	HAL_DISABLE = 0,
	HAL_FALLBACK,
	HAL_CHECK,
	HAL_SECURE
} HAL_SW_DOT1Q_MODE;


typedef struct HAL_VLAN_CREATE_PAR_s
{
    /*input parameters*/
    UINT16 vid;
    /*return vlaue*/
    UINT32 ret;
}HAL_VLAN_CREATE_PAR_t;

typedef struct HAL_VLAN_FLUSH_PAR_s
{
    /*return vlaue*/
    UINT32 ret;
}HAL_VLAN_FLUSH_PAR_t;

typedef struct HAL_VLAN_MEMBER_ADD_PAR_s
{
    /*input parameters*/
    UINT16 vid;
    UINT16 port;
    UINT8  type;/*port egress vlan type,0-unmodify,1-untag,2-tag,others invalid.*/
    /*return vlaue*/
    UINT32 ret;
}HAL_VLAN_MEMBER_ADD_PAR_t;


typedef struct HAL_VLAN_MEMBER_DEL_PAR_s
{
    /*input parameters*/
    UINT16 vid;
    UINT16 port;
    /*return vlaue*/
    UINT32 ret;
}HAL_VLAN_MEMBER_DEL_PAR_t;

typedef struct HAL_VLAN_LOCAL_SWITCH_DEL_PAR_s
{
    UINT32 port;
    UINT32 enable;
    UINT32 ret;
}HAL_VLAN_LOCAL_SWITCH_DEL_PAR_t;


#pragma pack ()

#endif
