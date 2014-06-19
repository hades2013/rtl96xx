/*****************************************************************************
 Copyright (c) 2011, Hangzhou H3C Technologies Co., Ltd. All rights reserved.
------------------------------------------------------------------------------
                            if_obj.h
  Project Code: Miniware
   Module Name:
  Date Created: 2011-2-24 
        Author: tangxiaohu
   Description: if_obj.h header file 

------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
  --------------------------------------------------------------------------
                                                                             
*****************************************************************************/
#ifndef _IF_OBJ_H_
#define _IF_OBJ_H_

#ifdef  __cplusplus
extern "C"{
#endif

typedef struct tagIf_Obj {
    ifindex_t uiIfIndex;
	INT iChildCnt;
	UCHAR ucPseudoName[IFNAMESIZE]; /*对外接口名，可以变更*/
	UCHAR ucIfName[IFNAMESIZE];/*内部使用，不可更改*/
    CHAR szDesc[IF_DESC_LEN];
    IF_ENABLE_E enEnable;    /*enable disable*/
    IF_STATUS_E enStatus;    /*status of the intf*/
    struct tagIf_Obj * pstNext;
    struct tagIf_Obj * pstParent;
    VOID * pstData;
}IF_OBJ_S;

typedef struct tagIF_HEAD
{
    UINT uiMax;
    UINT uiCount;
    IF_OBJ_S *pstNext;
}IF_HEAD_S;
#define IF_ListLockInit(sub_type)           spin_lock_init(lock[sub_type])
#define IF_ListReadLock(sub_type)           read_lock_bh(lock[sub_type])
#define IF_ListReadUnlock(sub_type)         read_unlock_bh(lock[sub_type])
#define IF_ListWriteLock(sub_type)          write_lock_bh(lock[sub_type])
#define IF_ListWriteUnlock(sub_type)        write_unlock_bh(lock[sub_type])


/* 定义与其它用户进程进行通信的数据结构*/
#if 0
typedef enum tagIF_PROC_TYPE
{
    IF_SET_PROC,
    IF_GET_PROC,
    IF_SET_ATTR_PROC,
    
    /*在此前添加*/
    IF_END_PROC
}IF_PROC_TYPE_E;

typedef struct tagIF_MSG
{    
    UINT  uiMsgType;    /*消息类型*/
    UINT  uiLen;        /*数据体长度*/
    UCHAR data[0];
}IF_MSG_S;

/*User_IF_Set和User_IF_SetAttr会用到下面这个结构*/
typedef struct tagIF_SET_PAYLOAD
{
    ifindex_t   uiIfindex;
    UINT        uiId;           /*function id or data id*/          
    UINT        uiParaNum;      /*参数个数*/
    UINT        uiLen;
    UCHAR       data[0];        /*|len|para1|len|para2|len|para3|*/
}IF_SET_PAYLOAD_S;
typedef struct tagIF_DEL_PAYLOAD
{
    ifindex_t ifindex; 
}IF_DEL_PAYLOAD_S;


/*User_IF_Get使用的结构*/
typedef struct tagIF_GET_PAYLOAD
{
    ifindex_t   uiIfindex;
    UINT        uiDataId;           /*data id*/
}IF_GET_PAYLOAD_S;

/*返回值通信结构*/
typedef struct tagIF_RET_PAYLOAD
{
    INT iRet;
    UINT uiPayloadlen;
    UCHAR data[0];
}IF_RET_PAYLOAD_S;



/*以端口为例定义出ETH端口的数据集合和操作集合*/
typedef struct tagIF_ETH_DATA
{
    UCHAR ucEthName[IFNAMESIZE]; 
    INT iEthPhysicalId;
    INT iEthState;
    INT iEthSpeed;
    INT iEthDuplex;
    
}IF_ETH_DATA_S;
#endif

VOID InitObjBitPool(VOID);
IF_OBJ_S * AllocIfObj(VOID);
VOID FreeIfObj(IF_OBJ_S * pstObj);
IF_RET_E AddObjToList(IF_OBJ_S * pstObj);
IF_RET_E RemoveObjFromList(IF_OBJ_S * pstObj);
IF_OBJ_S * GetObjByIfindex(ifindex_t uiIfindex);
IF_OBJ_S * GetObjByPseudoNameByDomain(CHAR * pcPseudo_name,IF_DOMAIN_E enDomain);
#define GetObjByPseudoName(pcPseudo_name) GetObjByPseudoNameByDomain(pcPseudo_name,IF_DOMAIN_NORMAL)
INT TestIfObjExist(ifindex_t uiIfindex);
ifindex_t GetFreeIfindex(IF_SUB_TYPE_E enSubType,IF_DOMAIN_E enDomain);
//ifindex_t GetFreeIfindex(IF_SUB_TYPE_E enSubType);
VOID * MallocIfData(IF_SUB_TYPE_E enSubType);
VOID FreeIfData(ifindex_t ifindex,CONST VOID * pData);
VOID SetIfPseudoName(IF_OBJ_S *pstIfObj, CHAR * pcPseudo_name);
VOID SetIfPhysicalId(IF_OBJ_S *pstIfObj, INT iId);
IF_RET_E DestroyIfData(ifindex_t ifindex);
IF_RET_E InitIfData(ifindex_t ifindex,VOID *pData);


VOID K_IF_Register(UINT uiSubType,IF_REGISTER_BLOCK *pstIfRegBlock);
VOID K_IF_UnRegister(UINT uiSubType);
IF_RET_E InitIfData(ifindex_t ifindex,VOID *pData);
VOID InitIFRegisterTbl (VOID);
VOID SetIfName(IF_OBJ_S *pstIfObj, CHAR * pcIf_name);
IF_RET_E SetPriAttr(IN ifindex_t uiIfindex, IN IF_ATTR_E iAttrId, IN VOID * pValue,IN UINT iLen);
IF_RET_E GetIfPriAttr( IN ifindex_t uiIfindex, IN IF_ATTR_E iAttrId, INOUT VOID * pValue ,IN UINT iLen);
IF_OBJ_S * GetNextObj(IN IF_SUB_TYPE_E enSubType,INOUT ifindex_t uiIfindex);
IF_OBJ_S * GetObjByIfNameBYDomain(CHAR * pcIf_name,IF_DOMAIN_E enDomain);
#define GetObjByIfName(pcIf_name)  GetObjByIfNameBYDomain(pcIf_name,IF_DOMAIN_NORMAL)
IF_RET_E GetFirstIfindexbyDomain( IN IF_SUB_TYPE_E enSubType, IN IF_DOMAIN_E enDomain,OUT ifindex_t * puiIfindex );
#define GetFirstIfindex(enSubType,puiIfindex) GetFirstIfindexbyDomain(enSubType,IF_DOMAIN_NORMAL,puiIfindex)
//IF_RET_E GetFirstIfindex(  IF_SUB_TYPE_E enSubType,  ifindex_t * puiIfindex );
VOID InitIFTbl(VOID);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif  /* end of _IF_OBJ_H_ */
