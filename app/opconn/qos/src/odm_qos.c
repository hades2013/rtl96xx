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
* FILENAME:  qosclass.c
*
* DESCRIPTION: 
*	
*
* Date Created: Aug 19, 2008
*
* Authors(optional): lwang.
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/qos/src/odm_qos.c#1 $
* $Log:$
*
*
**************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /*__cplusplus*/
#endif /*__cplusplus*/

#include "opl_debug.h"

#include "qos.h"
#include "log.h"
#include "vos_libc.h"
#include "odm_qos.h"
#include "odm_port.h"

#define MODULE MOD_QOS

#define CLASS_TRACE() OP_DEBUG(DEBUG_LEVEL_INFO, "\n%s,%s,%d!\r\n",__FILE__,__FUNCTION__,__LINE__)
UINT8 OamClsCount = 0;
UINT8 WebClsCount = 0;

void printoamclassdata(CLS_CONFIG_INFO_t *stClassCfg);
static opl_acl_list_t  g_acl_list;
static VOS_SEM_t g_odmClsLock;

#define oplOdmClsLock() vosSemTake(&g_odmClsLock)
#define oplOdmClsUnLock() vosSemGive(&g_odmClsLock)

static UINT16 odmDataFilterRuleId = 0xFFFF;

int OdmAclListDelLowestPre()
{
    struct list_head *tmp;
    opl_acl_list_t *pClsinfo=NULL,*beg;
    struct list_head *head=&g_acl_list.list;

    if(head==NULL)
    {
      return QOS_INVALID_POINTER;
    }
    beg=list_entry(head,opl_acl_list_t,list);
    tmp=head->prev;	
    pClsinfo=list_entry(tmp,opl_acl_list_t,list);
    if(pClsinfo->rulepri>0)
    {
      dalOnuClsRuleCtcDel(pClsinfo);
      list_del(tmp);
      vosFree(pClsinfo);	
      beg->prinum--;
      beg->tcnum--;	
      return NO_ERROR;	
    }
    return QOS_GENERAL_ERROR;
}

int OdmAclListShowWithRulePre(int fd)
{
    struct list_head *pos, *n;
    opl_acl_list_t *pClsinfo=NULL;
    CLS_CONFIG_INFO_t *stClassCfg;
    UINT8 str_ip[20] = "";
    struct list_head *head=&g_acl_list.list;

    if(head==NULL)
    {
      return QOS_INVALID_POINTER;
    }
    pClsinfo=list_entry(head,opl_acl_list_t,list);
    vosPrintf(fd,"precedence number  %d  total %d \n",pClsinfo->prinum,pClsinfo->tcnum); 
    list_for_each_safe(pos, n, head) {
    pClsinfo=list_entry(pos,opl_acl_list_t,list);
    stClassCfg=&(pClsinfo->stClassCfg);
    if(pClsinfo->rulepri>0)
    {
        vosPrintf(fd,"--------- precedence %d Status %s---------------------\n",pClsinfo->rulepri,
        pClsinfo->status==2?"disable":(pClsinfo->status?"active":"invalid"));
        vosPrintf(fd," pri     idx	   mask	  rule    addr	  off\n");
        vosPrintf(fd," %3d    %3d       %3d     %3d     %3d     %3d \n",pClsinfo->rulepri,
        pClsinfo->dalindex,pClsinfo->hwshadow.clsHwMaskID,pClsinfo->hwshadow.clsHwRuleID,
        pClsinfo->hwshadow.clsHwAddr,pClsinfo->hwshadow.clsHwOff);
        vosPrintf(fd,"queueMapId = %d \r\n",stClassCfg->queueMapId);
        vosPrintf(fd,"cosMapVal = %d \r\n",stClassCfg->cosMapVal);
        if(stClassCfg->dstMacFlag)
            vosPrintf(fd,"dstMac = %02x:%02x:%02x:%02x:%02x:%02x\r\n",stClassCfg->dstMac.lowRange[0],
                    stClassCfg->dstMac.lowRange[1],
                    stClassCfg->dstMac.lowRange[2],
                    stClassCfg->dstMac.lowRange[3],
                    stClassCfg->dstMac.lowRange[4],
                    stClassCfg->dstMac.lowRange[5]);
        if(stClassCfg->srcMacFlag)
            vosPrintf(fd,"srcMac = %02x:%02x:%02x:%02x:%02x:%02x\r\n",stClassCfg->srcMac.lowRange[0],
                    stClassCfg->srcMac.lowRange[1],
                    stClassCfg->srcMac.lowRange[2],
                    stClassCfg->srcMac.lowRange[3],
                    stClassCfg->srcMac.lowRange[4],
                    stClassCfg->srcMac.lowRange[5]);

        if(stClassCfg->etherPriFlag)
            vosPrintf(fd,"etherPri = %d \r\n",stClassCfg->etherPri.lowRange);
        if(stClassCfg->vlanIdFlag)
            vosPrintf(fd,"vlanId = %d \r\n",stClassCfg->vlanId.lowRange);
        if(stClassCfg->lenOrTypeFlag)
            vosPrintf(fd,"EtherType = %d \r\n",stClassCfg->lenOrType.lowRange);
        if(stClassCfg->ipTypeFlag)
            vosPrintf(fd,"ipType = %d \r\n",stClassCfg->ipType.lowRange);
        if(stClassCfg->dstIpFlag)
        {
            vosMemSet(str_ip,0,20);
            cliIpToStr(stClassCfg->dstIp.lowRange,str_ip);
            vosPrintf(fd,"dstIp = %s \r\n",str_ip);	
        }
        if(stClassCfg->srcIpFlag)
        {
            vosMemSet(str_ip,0,20);
            cliIpToStr(stClassCfg->srcIp.lowRange,str_ip);
            vosPrintf(fd,"srcIp = %s \r\n",str_ip);
        }
        if(stClassCfg->ipV4DscpFlag)
            vosPrintf(fd,"ipV4Dscp = %d \r\n",(stClassCfg->ipV4Dscp.lowRange)>>2);
        if(stClassCfg->srcL4PortFlag)
            vosPrintf(fd,"srcL4Port = %d \r\n",stClassCfg->srcL4Port.lowRange);
        if(stClassCfg->dstL4PortFlag)
            vosPrintf(fd,"dstL4Port = %d \r\n",stClassCfg->dstL4Port.lowRange);
        vosPrintf(fd,"-------------------------------------------------------------\r\n");
    }
}

  return NO_ERROR;
}

opl_acl_list_t * OdmAclListFindPre(UINT8 precedence)
{
    struct list_head *pos, *n;
    opl_acl_list_t *pClsinfo=NULL;
    UINT8 rulepri=precedence;
    struct list_head *head=&g_acl_list.list;


    if(head==NULL)
    {
        return NULL;
    }
    list_for_each_safe(pos, n, head) {
    pClsinfo=list_entry(pos,opl_acl_list_t,list);
    if(rulepri==pClsinfo->rulepri)
    {
        return pClsinfo;
    }
}
    return NULL;
}

int OdmAclListAdd(UINT16 ucPortS,CLS_CONFIG_INFO_t *stClassCfg)
{
    struct list_head *pos, *prev;
    opl_acl_list_t *pClsinfo=NULL;
    opl_acl_list_t *tmp=NULL,*beg=NULL,*pre=NULL;
    UINT8 rulepri;
    UINT8 match=0;
    int status=NO_ERROR;
    struct list_head *head=&g_acl_list.list;

    if(head==NULL || stClassCfg==NULL)
    {
        return QOS_INVALID_POINTER;
    }

    rulepri=stClassCfg->rulePri;
    if(rulepri>ACL_MAX_PRI )
    {
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"precedence less than 8....\r\n");
        return QOS_INVALID_POINTER;
    }
    pClsinfo=vosAlloc(sizeof(opl_acl_list_t));
    if(pClsinfo==NULL)
    {
        OP_DEBUG(DEBUG_LEVEL_CRITICAL,"system out of memory....\r\n");
        return QOS_INVALID_POINTER;
    }
    vosMemSet(pClsinfo,0,sizeof(opl_acl_list_t));
    pClsinfo->status=ACL_STATE_ACTIVE;
    pClsinfo->rulepri=rulepri;
    vosMemCpy(&(pClsinfo->stClassCfg),stClassCfg,sizeof(CLS_CONFIG_INFO_t));
    beg=list_entry(head,opl_acl_list_t,list);
    if(rulepri==0)
    {
        list_add(&pClsinfo->list,head);
        dalOnuClsRuleCtcAdd(pClsinfo);
        beg->tcnum++;
        return NO_ERROR;
    }
    if(OdmAclListFindPre(rulepri)!=NULL)
    {
        match=1;
    }
#if 0
if(beg->prinum >= ACL_MAX_PRI)

OdmAclListDelLowestPre();

else if(match==1 && rulepri==ACL_MAX_PRI)

OdmAclListDelLowestPre();

#endif
    list_for_each_prev_safe(pos,prev, head){
    tmp=list_entry(pos,opl_acl_list_t,list);
    //pre=list_entry(prev,opl_acl_list_t,list);
    if(match==1 && tmp->rulepri==ACL_MAX_PRI)
    {
        OdmAclListDelLowestPre();
    }
    else if(match==1 && tmp->rulepri>=rulepri)
    {
        tmp->rulepri++;
        status=dalOnuClsRuleCtcChange(tmp,tmp->rulepri-1);
        tmp->hwshadow.clsHwMaskID=tmp->rulepri-1;
    }
    else if(tmp->rulepri<rulepri)
    {
        break;
    }
    }
    beg->prinum++;
    beg->tcnum++;	
    list_add(&pClsinfo->list,pos); 

    /* assign invalid dal index ,if add ctc error, don't need del ctc record */
    pClsinfo->dalindex = 0xFFFF;
    status+=dalOnuClsRuleCtcAdd(pClsinfo);
    if(status!=NO_ERROR)
    {
        pClsinfo->status=ACL_STATE_READY;
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"add error \n");
        OdmAclListDel(ucPortS,rulepri);
        return status;
    }

    return NO_ERROR;	
}

int OdmAclListDel(UINT16 ucPortS,UINT8 precedence)
{
    struct list_head *pos, *n,*del;
    opl_acl_list_t *pClsinfo=NULL;
    opl_acl_list_t *tmp=NULL,*beg=NULL;
    UINT8 rulepri=precedence;
    struct list_head *head=&g_acl_list.list;

    if(head==NULL || precedence >ACL_MAX_PRI)
    {
        return QOS_INVALID_POINTER;
    }

    beg=list_entry(head,opl_acl_list_t,list);
    pClsinfo=OdmAclListFindPre(precedence);
    if(pClsinfo==NULL)
    {
        return QOS_GENERAL_ERROR;
    }
    else
    {
        del = pClsinfo->list.next;
        if (pClsinfo->dalindex != 0xFFFF)
        {
            dalOnuClsRuleCtcDel(pClsinfo);
        }

        list_del(&pClsinfo->list);
        vosFree(pClsinfo);
        for (; del != head;del = del->next)
        {
            tmp=list_entry(del,opl_acl_list_t,list);
            if(tmp->rulepri > 0)
            {
                tmp->rulepri--;
                dalOnuClsRuleCtcChange(tmp,tmp->rulepri-1);
                tmp->hwshadow.clsHwMaskID=tmp->rulepri-1;
            }
        }
        beg->prinum--;
        beg->tcnum--;
    }
    return NO_ERROR;
}
/*
 flag 
 1: clear all  0: clear only precedence 
*/   
int OdmAclListClearAll(UINT16	ucPortS,UINT32 flag)
{
    struct list_head *pos, *n;
    opl_acl_list_t *pClsinfo=NULL,*beg;
    CLS_CONFIG_INFO_t *stClassCfg;
    struct list_head * head=&g_acl_list.list;
 
    if(head==NULL)
    {
        return QOS_INVALID_POINTER;
    }
 
    beg=list_entry(head,opl_acl_list_t,list);
    list_for_each_safe(pos, n, head) {
    pClsinfo=list_entry(pos,opl_acl_list_t,list);
    stClassCfg=&(pClsinfo->stClassCfg);
    if(flag || pClsinfo->rulepri>0)
    {
        if(pClsinfo->rulepri>0 )
            beg->prinum--;
        dalOnuClsRuleCtcDel(pClsinfo); 
        list_del(&pClsinfo->list);  
        vosFree(pClsinfo); 
        beg->tcnum--;
    }
    }
    return NO_ERROR;

}

/*******************************************************************************
*
* odmCheckClassExist:
*
* DESCRIPTION:
* This function check this class if exist
*
* INPUTS:
         name:class name
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCheckClassExist
(
    char *name
)
{
    int status;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    return (vosConfigSectionIsExisted(QOS_CLASS_SESSION,name));
    
}
int odmCheckQosSchedExist
(
    char *name
)
{
    int status;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    return (vosConfigSectionIsExisted(QOS_QosSched_SESSION,name));
    
}
/*******************************************************************************
*
* odmCheckClassInstall: 
*
* DESCRIPTION:
* This function check this class if install
*
* INPUTS:
         name:class name
*
* OUTPUTS: 
         enable:enable/disable install.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCheckClassInstall
(
    char     *name,
    QOS_BOOL *enable
)
{
    if ((name == NULL)||(enable == NULL))
    {
        return QOS_INVALID_POINTER;
    }
    *enable = (vosConfigUInt32Get(QOS_CLASS_SESSION,
                                             name,
                                             CLASSKEY_INSTALL,0xffff) < 0xffff) ? QOS_TRUE : QOS_FALSE;
    return NO_ERROR;
}
/*******************************************************************************
*
* odmCreateClass: 
*
* DESCRIPTION:
* This function create a new qos class entry.
*
* INPUTS:
         name:class name
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCreateClass
(
    char *name
)
{
    int status;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }

    status = odmCheckClassExist(name);
    if (status == 0)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    return (vosConfigSectionCreate(QOS_CLASS_SESSION,name));
    
}
/*******************************************************************************
*
* odmDestroyClass: 
*
* DESCRIPTION:
* This function destory a qos class entry.
*
* INPUTS:
         name:class name
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmDestroyClass
(
    char *name
)
{
    QOS_BOOL en = QOS_FALSE;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }

    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        CLASS_TRACE();
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
    if (odmCheckClassExist(name) != NO_ERROR)
    {
        CLASS_TRACE();
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;

    return vosConfigSectionDelete(QOS_CLASS_SESSION,name);
}
/*******************************************************************************
*
* odmCfgClassDestMac: 
*
* DESCRIPTION:
* This function config class destmac value.
*
* INPUTS:
         name:class name
         mac :mac address.
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassDestMac
(
    char *name,
    char *mac,
    CLASS_ACT act
)
{
    QOS_BOOL en = QOS_FALSE;
    
    if ((name == NULL)||(mac == NULL))
    {
        return QOS_INVALID_POINTER;
    }

    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;

    if (act == CLASS_ADD)
    {
        return (vosConfigValueSet(QOS_CLASS_SESSION,name,CLASSKEY_DESTMAC,mac));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_DESTMAC));
    }
}
/*******************************************************************************
*
* odmCfgClassSrcMac: 
*
* DESCRIPTION:
* This function config class srcmac value.
*
* INPUTS:
         name:class name
         mac :mac address.
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassSrcMac
(
    char *name,
    char *mac,
    CLASS_ACT act
)
{
    QOS_BOOL en = QOS_FALSE;
    if ((name == NULL)||(mac == NULL))
    {
        return QOS_INVALID_POINTER;
    }

    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;

    if (act == CLASS_ADD)
    {
        return (vosConfigValueSet(QOS_CLASS_SESSION,name,CLASSKEY_SRCMAC,mac));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_SRCMAC));
    }
}
/*******************************************************************************
*
* odmCfgClassTypeLenght:
*
* DESCRIPTION:
* This function config class typelength value.
*
* INPUTS:
         name:class name
         typelen :typelength value.
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassTypeLenght
(
    char   *name,
    UINT16 typelen,
    CLASS_ACT act
)
{
    UINT32 type;
    QOS_BOOL en = QOS_FALSE;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }

    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
    type = typelen;
    if (act == CLASS_ADD)
    {
        return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_TYPE,type));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_TYPE));
    }
}
/*******************************************************************************
*
* odmCfgClassFVid: 
*
* DESCRIPTION:
* this function config class fist vlan id.
*
* INPUTS:
         name:class name
         fvid:vlan id.
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassFVid
(
    char   *name,
    UINT16 fvid,
    CLASS_ACT act
)
{
    UINT32 vid;
    QOS_BOOL en = QOS_FALSE;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }

    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
    vid = fvid;
    if (act == CLASS_ADD)
    {
        return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_FVID,vid));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_FVID));
    }
}
/*******************************************************************************
*
* odmCfgClassFCos: 
*
* DESCRIPTION:
* this function config class first cos value.
*
* INPUTS:
         name:class name
         fcos:cos value.
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassFCos
(
    char   *name,
    UINT16 fcos,
    CLASS_ACT act
)
{
    UINT32 cos;
    QOS_BOOL en = QOS_FALSE;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }

    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
    cos = fcos;
    if (act == CLASS_ADD)
    {
        return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_FCOS,cos));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_FCOS));
    }
}
/*******************************************************************************
*
* odmCfgClassDestIp: 
*
* DESCRIPTION:
* this function config class destIP value.
*
* INPUTS:
         name:class name
         ip  :ip address.
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassDestIp
(
    char   *name,
    char   *ip,
    CLASS_ACT act
)
{
    QOS_BOOL en = QOS_FALSE;
    
    if ((name == NULL)||(ip == NULL))
    {
        return QOS_INVALID_POINTER;
    }
    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
    if (act == CLASS_ADD)
    {
        return (vosConfigValueSet(QOS_CLASS_SESSION,name,CLASSKEY_DESTIP,ip));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_DESTIP));
    }
}
/*******************************************************************************
*
* odmCfgClassSrcIp: 
*
* DESCRIPTION:
* this function config class srcIP value.
*
* INPUTS:
         name:class name
         ip  :ip address.
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassSrcIp
(
    char   *name,
    char   *ip,
    CLASS_ACT act
)
{
    QOS_BOOL en = QOS_FALSE;
    
    if ((name == NULL)||(ip == NULL))
    {
        return QOS_INVALID_POINTER;
    }
    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }

    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
    if (act == CLASS_ADD)
    {
        return (vosConfigValueSet(QOS_CLASS_SESSION,name,CLASSKEY_SRCIP,ip));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_SRCIP));
    }
}
int odmCfgClassV6Pre
(
    char   *name,
    UINT8 V6Pre,
    CLASS_ACT act
)

{
    UINT16 v6Pre;
    QOS_BOOL en = QOS_FALSE;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
        v6Pre = V6Pre;
    if (act == CLASS_ADD)
    {
        return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_V6PRE,v6Pre));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_V6PRE));
    }
}
/*******************************************************************************
*
* odmCfgClassDestL4Port: 
*
* DESCRIPTION:
* this function config class DestL4Port value.
*
* INPUTS:
         name:class name
         DestL4Port:L4 port number..
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassDestL4Port
(
    char   *name,
    UINT16 DestL4Port,
    CLASS_ACT act
)
{
    UINT32 port;
    QOS_BOOL en = QOS_FALSE;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
        port = DestL4Port;
    if (act == CLASS_ADD)
    {
        return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_DESTL4PORT,port));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_DESTL4PORT));
    }
}
/*******************************************************************************
*
* odmCfgClassSrcL4Port: 
*
* DESCRIPTION:
* this function config class SrcL4Port value.
*
* INPUTS:
         name:class name
         SrcL4Port:L4 port number..
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassSrcL4Port
(
    char   *name,
    UINT16 SrcL4Port,
    CLASS_ACT act
)
{
    UINT32 port;
    QOS_BOOL en = QOS_FALSE;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
        port = SrcL4Port;
    if (act == CLASS_ADD)
    {
        return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_SRCL4PORT,port));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_SRCL4PORT));
    }
} 
/*******************************************************************************
*
* odmCfgClassIpType: 
*
* DESCRIPTION:
* this function config class IP type value.
*
* INPUTS:
         name:class name
         Iptype:Ip type value.
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassIpType
(
    char   *name,
    UINT8 Iptype,
    CLASS_ACT act
)
{
    UINT32 iptype;
    QOS_BOOL en = QOS_FALSE;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
        iptype = Iptype;
    if (act == CLASS_ADD)
    {
        return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_PROTOCAL,iptype));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_PROTOCAL));
    }
}
/*******************************************************************************
*
* odmCfgClassIpTos: 
*
* DESCRIPTION:
* this function config class IP tos value.
*
* INPUTS:
         name:class name
         Iptos:Ip tos value.
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassIpTos
(
    char   *name,
    UINT8  Iptos,
    CLASS_ACT act
)
{
    UINT32 tos;
    QOS_BOOL en = QOS_FALSE;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
        tos = Iptos;
    if (act == CLASS_ADD)
    {
        return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_TOS,tos));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_TOS));
    }
}
/*******************************************************************************
*
* odmCfgClassIpDscp: 
*
* DESCRIPTION:
* this function config class IPv4 dscp value.
*
* INPUTS:
         name:class name
         dscp:Ipv4 dscp value.
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassIpDscp
(
    char   *name,
    UINT8  dscp,
    CLASS_ACT act
)
{
    UINT32 ipdscp;
    QOS_BOOL en = QOS_FALSE;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) 
    {
        return QOS_CLS_INSTALL_CHECK_ERROR;
    }
    ipdscp = dscp;
    if (act == CLASS_ADD)
    {
        return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_DSCP,ipdscp));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_DSCP));
    }
}
/*******************************************************************************
*
* odmCfgClassQueue: 
*
* DESCRIPTION:
* this function config class queue map value.
*
* INPUTS:
         name:class name
         queue:queue value.
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassQueue
(
    char  *name,
    UINT8 queue,
    CLASS_ACT act
)
{
    
    QOS_BOOL en = QOS_FALSE;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    if (queue > 7)
    {
        return QOS_GENERAL_ERROR;
    }
    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
    if (act == CLASS_ADD)
    {
        /*-ltang debug-!add for bug3222 --start*/
        vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_CHANGEICOS,1);
        /*-ltang debug-!add for bug3222 --end*/
        return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_QUEUE,queue));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_QUEUE));
    }
}
/*******************************************************************************
*
* odmCfgClassEthPri: 
*
* DESCRIPTION:
* this function config class ethernet priority value.
*
* INPUTS:
         name:class name
         ethpri:ethernet priority value.
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/

int odmCfgClassEthPri
(
    char  *name,
    UINT8 ethpri,
    CLASS_ACT act
)
{
    //printf("ethpri = %d\n",ethpri);
    QOS_BOOL en = QOS_FALSE;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    if (ethpri > 7)
    {
        return QOS_GENERAL_ERROR;
    }
    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
    if (act == CLASS_ADD)
    {
        /*-ltang debug-!add for bug3222 --start*/
        vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_CHANGECOS,1);
        /*-ltang debug-!add for bug3222 --end*/	
        return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_PRIORITY,ethpri));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_PRIORITY ));
    }
}
/*******************************************************************************
*
* odmCfgClassMaskPri: 
*
* DESCRIPTION:
* this function config class mask priority value.
*
* INPUTS:
         name:class name
         ethpri:mask priority value.
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassMaskPri
(
    char  *name,
    UINT8 ethpri,
    CLASS_ACT act
)
{
    
    QOS_BOOL en = QOS_FALSE;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    if (ethpri > ACL_MAX_PRI)
    {
        return QOS_GENERAL_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
    if (act == CLASS_ADD)
    {
        return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_Precedence,ethpri));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_Precedence));
    }
}



int odmGetClassNameByRule
(
    UINT16 ruleid,
    char *name
)
{
    UINT8  count;
    UINT8  roop;

    char   *session = NULL;
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }

    count = vosConfigSectionCount(QOS_CLASS_SESSION);

    for (roop = 0;roop < count;roop++)
    {
        if (vosConfigSectionGetByIndex(QOS_CLASS_SESSION,roop,&session) == 0)
        {
            if (ruleid == vosConfigUInt32Get(QOS_CLASS_SESSION,session,CLASSKEY_INSTALL,0))
            {
                vosStrCpy(name,session);
                return NO_ERROR;
            }

        }
    }

    return QOS_GENERAL_ERROR;
}

/*******************************************************************************
*
* odmCfgClassTranfAct: 
*
* DESCRIPTION:
* this function config class mask priority value.
*
* INPUTS:
         name:class name
         ethpri:mask priority value.
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmCfgClassTranfAct
(
    char  *name,
    TRANF_ACT t_act,
    CLASS_ACT act    
)
{
    
    QOS_BOOL en = QOS_FALSE;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }

    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
    if (act == CLASS_ADD)
    {
        return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_TRANFACT,t_act));
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_TRANFACT));
    }
}


/*******************************************************************************
*
* odmCfgClassRmkVid: 
*
* DESCRIPTION:
* this function remark class vid value.
*
* INPUTS:
         name:class name
         t_act:vid remark action,INSERTVID or CHANGEVID
         vid:vlan to be padded or changed        
act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/

int odmCfgClassRmkVid(
    char  *name,
    TRANF_ACT t_act,
    UINT16 fvid,
    CLASS_ACT act 
)
{
    QOS_BOOL en = QOS_FALSE;

    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }

    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
    if (act == CLASS_ADD)
    {
        if(t_act==INSERTVID)
        {
            vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_CHANGEVID);
            return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_INSERTVID,fvid));
        }
        else if(t_act==CHANGEVID)
        {
            vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_INSERTVID);
            return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_CHANGEVID,fvid));
        }
    }
    else
    {
        if(t_act==INSERTVID)
            return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_INSERTVID));	
        else if(t_act==CHANGEVID)    
            return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_CHANGEVID));	
    }
    return NO_ERROR;
}

/*******************************************************************************
*
* odmCfgClassDirection: 
*
* DESCRIPTION:
* this function configure class direction.
*
* INPUTS:
         name:class name
         streamType:UP_STREAM_,DOWN_STREAM_,BOTH_STREAM_
         act :class action(add/move).
*
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/

int odmCfgClassDirection(
    char  *name,
    STREAM_TYPE_T streamType,
    CLASS_ACT act 
)
{
    QOS_BOOL en = QOS_FALSE;

    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }

    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
    if (act == CLASS_ADD)
    {
        if(streamType==UP_STREAM_)
        {
            vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_DIRECTION);	
            return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_DIRECTION,streamType));
        }
        else if(streamType==DOWN_STREAM_)
        {
            vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_DIRECTION);
            return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_DIRECTION,streamType));
        }
        else
        {
            vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_DIRECTION);
            return (vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_DIRECTION,BOTH_STREAM_));
        }
    }
    else
    {
        return (vosConfigKeyDelete(QOS_CLASS_SESSION,name,CLASSKEY_DIRECTION));	
    }
    return NO_ERROR;
}


/*******************************************************************************
*
* odmRemoveClassEntry: 
*
* DESCRIPTION:
* this function remove a class entry from hw.
*
* INPUTS:
         name:class name
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmRemoveClassEntry
(
    char  *name,
    UINT8 stream
)
{
    int    ruleindex = -1;
    UINT32 clsvalue;

    UINT8  count;
    UINT8  roop;

    char   *session = NULL;
    QOS_BOOL en     = QOS_FALSE;
    UINT16 ruleId;
    UINT32 rulepri;

    int status;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    if (en != QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
    switch (stream)
    {
        case UP_STREAM_:
             break;
        case DOWN_STREAM_:
             break;
        case BOTH_STREAM_:
             break;
        default :
            return QOS_INVALID_TYPE;
    }
    clsvalue = vosConfigUInt32Get(QOS_CLASS_SESSION,name,CLASSKEY_INSTALL,0xffff);
    if (clsvalue == 0xffff)
    {
        CLASS_TRACE();
        return QOS_GENERAL_ERROR;
    }
    rulepri = vosConfigUInt32Get(QOS_CLASS_SESSION,name,CLASSKEY_Precedence,0);
    if (rulepri != 0)
    {
        status=odmOamClassRemove(0,rulepri);
    }
    else
        status=dalVoipClsDelEntry(clsvalue);
#if 0
if (OPL_OK == dalAclFindCls(name, &ruleId))
{
	status = dalAclClsDel2(ruleId);
}
#endif

    if (status != NO_ERROR) 
    {
        CLASS_TRACE();
        return QOS_GENERAL_ERROR; 
    }
    vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_INSTALL,0xffff);

    count = vosConfigSectionCount(QOS_CLASS_SESSION);

    for (roop = 0;roop < count;roop++)
    {
        if (vosConfigSectionGetByIndex(QOS_CLASS_SESSION,roop,&session) == 0)
        {
            if (clsvalue == vosConfigUInt32Get(QOS_CLASS_SESSION,session,CLASSKEY_INSTALL,0))
            {
                vosConfigUInt32Set(QOS_CLASS_SESSION,session,CLASSKEY_INSTALL,0xffff);
            }

        }
    }

    return NO_ERROR; 
}
/*******************************************************************************
*
* odmInstallClassEntry: 
*
* DESCRIPTION:
* this function install a class entry to hw.
*
* INPUTS:
         name:class name
         ports:	port map for Atheros switch
* OUTPUTS: N/A.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmInstallClassEntry
(
    char  *name,
    UINT8 stream,
    char* ports
)
{
    char *clsvalue = NULL;
    UINT32   ip    = 0;
    UINT16   rule  = 0;
    UINT8    match = 0;
    UINT8    t_act = 0;
    //UINT8    acttmp= 0;
    //UINT16   ruleid[255] = {0};
    UINT8    index = 0;
    UINT8  fullmacMask[MAC_LENGTH] = {
	  0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF
	}; 

    int status=NO_ERROR;

    UINT8 mac[6]   = {0,0,0,0,0,0};
    QOS_BOOL en    = QOS_FALSE;
    char nametmp[20] = {0};
    UINT16 rmkvid;
    UINT8 rmkvidop;

 
    CLS_CONFIG_INFO_t clsconfiginfo;
    
    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    odmCheckClassInstall(name,&en);
    if (en == QOS_TRUE) return QOS_CLS_INSTALL_CHECK_ERROR;
    
    OPL_MEMSET(&clsconfiginfo,0,sizeof(CLS_CONFIG_INFO_t));
    /*according to the ingress/egress port to divison up/dowm stream*/
    switch (stream)
    {
        case UP_STREAM_:
            clsconfiginfo.bingress = 0;
            clsconfiginfo.ingressmask = 1;
             break;
        case DOWN_STREAM_:
            clsconfiginfo.bingress = 1;
            clsconfiginfo.ingressmask = 1;
             break;
        case BOTH_STREAM_:
            clsconfiginfo.bingress = 0;
            clsconfiginfo.ingressmask = 0;
             break;
        default :
            return QOS_INVALID_TYPE;
    }
    odmCfgClassDirection(name, stream, CLASS_ADD);
    
    /*config classify rule value*/
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DESTMAC,NULL);
    if (clsvalue != NULL)
    {
        if (utilStrToMacAddr(clsvalue, mac) != 0)
             return QOS_GENERAL_ERROR;
        clsconfiginfo.dstMacFlag = 1;
        clsconfiginfo.dstMacOp=CTC_RULE_MATCH;
        OPL_MEMCPY(clsconfiginfo.dstMac.lowRange,mac,6);
        OPL_MEMCPY(&(clsconfiginfo.dstMac.highRange),fullmacMask,MAC_LENGTH);
        match++;
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_SRCMAC,NULL);
    
    if (clsvalue != NULL)
    {
        if (utilStrToMacAddr(clsvalue, mac) != 0)
             return QOS_GENERAL_ERROR;
        clsconfiginfo.srcMacFlag = 1;
        clsconfiginfo.srcMacOp=CTC_RULE_MATCH;
        OPL_MEMCPY(clsconfiginfo.srcMac.lowRange,mac,6);
        OPL_MEMCPY(&(clsconfiginfo.srcMac.highRange),fullmacMask,MAC_LENGTH);
        match++;
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DESTIP,NULL);
    if (clsvalue != NULL)
    {
        clsconfiginfo.dstIpFlag = 1;
        clsconfiginfo.dstIpOp=CTC_RULE_MATCH;
        clsconfiginfo.dstIp.lowRange     = utilStrToIpAddr(clsvalue);
        clsconfiginfo.dstIp.highRange     = 0xffffffff;
        match++;
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_SRCIP,NULL);
    if (clsvalue != NULL)
    {
        clsconfiginfo.srcIpFlag = 1;
        clsconfiginfo.srcIpOp=CTC_RULE_MATCH;
        clsconfiginfo.srcIp.lowRange     = utilStrToIpAddr(clsvalue);
        clsconfiginfo.srcIp.highRange     = 0xffffffff;
        match++;
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_TYPE,NULL);
    if (clsvalue != NULL)
    {
        clsconfiginfo.lenOrTypeFlag = 1;
        clsconfiginfo.lenOrTypeOp=CTC_RULE_MATCH;
        clsconfiginfo.lenOrType.lowRange     = strtoul(clsvalue, NULL, 0);
        clsconfiginfo.lenOrType.highRange     = 0xffff;
        match++;
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_FVID,NULL);
    if (clsvalue != NULL)
    {
        clsconfiginfo.vlanIdFlag = 1;
        clsconfiginfo.vlanIdOp=CTC_RULE_MATCH;
        clsconfiginfo.vlanId.lowRange     = strtoul(clsvalue, NULL, 0);
        clsconfiginfo.vlanId.highRange     = 0xffff;
        match++;
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_FCOS,NULL);
    if (clsvalue != NULL)
    {
        clsconfiginfo.etherPriFlag = 1;
        clsconfiginfo.etherPriOp=CTC_RULE_MATCH;
        clsconfiginfo.etherPri.lowRange     = strtoul(clsvalue, NULL, 0);
        clsconfiginfo.etherPri.highRange     = 0xff;
        if (clsconfiginfo.etherPri.lowRange > 7) return QOS_GENERAL_ERROR;
        match++;
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DESTL4PORT,NULL);
    if (clsvalue != NULL)
    {
        clsconfiginfo.dstL4PortFlag = 1;
        clsconfiginfo.dstL4PortOp=CTC_RULE_MATCH;
        clsconfiginfo.dstL4Port.lowRange     = strtoul(clsvalue, NULL, 0); 
        clsconfiginfo.dstL4Port.highRange     = 0xffff;
        match++;
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_SRCL4PORT,NULL);
    if (clsvalue != NULL)
    {
        clsconfiginfo.srcL4PortFlag = 1;
        clsconfiginfo.srcL4PortOp=CTC_RULE_MATCH;
        clsconfiginfo.srcL4Port.lowRange     = strtoul(clsvalue, NULL, 0); 
        clsconfiginfo.srcL4Port.highRange     = 0xffff;
        match++;
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_PROTOCAL,NULL);
    if (clsvalue != NULL)
    {
        clsconfiginfo.ipTypeFlag = 1;
        clsconfiginfo.ipTypeOp=CTC_RULE_MATCH;
        clsconfiginfo.ipType.lowRange     = strtoul(clsvalue, NULL, 0);    
        clsconfiginfo.ipType.highRange     = 0xff;
        match++;
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_TOS,NULL);
    if (clsvalue != NULL)
    {
        clsconfiginfo.ipV4DscpFlag = 1;
		clsconfiginfo.ipV4DscpOp=CTC_RULE_MATCH;
		if (strtoul(clsvalue, NULL, 0) > 7) return QOS_GENERAL_ERROR;
        clsconfiginfo.ipV4Dscp.lowRange     = (strtoul(clsvalue, NULL, 0)<<5);  
        clsconfiginfo.ipV4Dscp.highRange     = 0xe0;
        match++;
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DSCP,NULL);
    if (clsvalue != NULL)
    {
        clsconfiginfo.ipV4DscpFlag = 1;
		clsconfiginfo.ipV4DscpOp=CTC_RULE_MATCH;
        if (strtoul(clsvalue, NULL, 0) > 63) return QOS_GENERAL_ERROR;
        clsconfiginfo.ipV4Dscp.lowRange     = (strtoul(clsvalue, NULL, 0) << 2);  
        clsconfiginfo.ipV4Dscp.highRange     = 0xfc;
        match++;
    }

    if (match == 0)
    {
        CLASS_TRACE();
        return QOS_CLS_NOANY_MATCH;
    }
    
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_QUEUE,NULL);
    if (clsvalue != NULL)
    {
        clsconfiginfo.queueMapId = strtoul(clsvalue, NULL, 0); 
        if (clsconfiginfo.queueMapId > 7) return QOS_GENERAL_ERROR;
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_PRIORITY,NULL);
    if (clsvalue != NULL)
    {
        clsconfiginfo.cosMapVal = strtoul(clsvalue, NULL, 0); 
        if (clsconfiginfo.cosMapVal > 7) return QOS_GENERAL_ERROR;
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_Precedence,NULL);
    if (clsvalue != NULL)
    {
        clsconfiginfo.rulePri = strtoul(clsvalue, NULL, 0); 
    }
    /*get small field action and big field rule*/
    //dalClscollisionActGet(&clsconfiginfo,&acttmp,ruleid);
/*add by ltang ,for the bug 3222 --start*/

#if 1
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_CHANGEICOS,NULL);
    if (clsvalue != NULL)
    {
        clsconfiginfo.rmkicoschange = strtoul(clsvalue, NULL, 0); 
    }

    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_CHANGECOS,NULL);
    if (clsvalue != NULL)
    {
    
        clsconfiginfo.rmkcoschange = strtoul(clsvalue, NULL, 0); 
    }
#endif
/*add by ltang ,for the bug 3222 --end*/	

    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_TRANFACT,NULL);
    if (clsvalue != NULL)
    {
        t_act = strtoul(clsvalue, NULL, 0); 
    }
    else t_act = PASS;
    
    switch (t_act)
    {
        case DROP:
            /*small filed set donot drop so this class can not offer drop*/
    #if 0
            if (acttmp & DONOT_DROP)
            {
                status = QOS_CLS_ACTION_ERROR;
                return status;
            }
    #endif
            clsconfiginfo.t_act = DROP_PKTS;
            break;
        case PASS:
            /*small filed set drop so this class should force do not drop*/
    #if 0
            if (acttmp & DROP_PKTS)
            {
                clsconfiginfo.t_act = DONOT_DROP;
            }
            else
    #endif
            {
                clsconfiginfo.t_act = 0;
            }
            break;
        case COPYTOCPU:
    #if 0
            /*small filed set donot copy so this class can not offer copy to cpu*/
            if (acttmp & DONOT_COPY_TO_CPU)
            {
                status = QOS_CLS_ACTION_ERROR;
                return status;
            }         
            /*small filed set drop so this class should force do not drop*/
            if (acttmp & DROP_PKTS)
            {
                clsconfiginfo.t_act = (DONOT_DROP|COPY_PKTS_TO_CPU);
            }
            else
    #endif
            {
                clsconfiginfo.t_act = COPY_PKTS_TO_CPU;
            }
            break;
        case SENDTOCPU:
            /*small filed set donot copy or do not drop so this class can not offer send to cpu*/
    #if 0
     if ((acttmp & DONOT_COPY_TO_CPU)||(acttmp & DONOT_DROP))
            {
                status = QOS_CLS_ACTION_ERROR;
                return status;
            }
    #endif
            clsconfiginfo.t_act = (DROP_PKTS|COPY_PKTS_TO_CPU);
            break;
        default:
            return QOS_INVALID_TYPE;
    }

    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_CHANGEVID,NULL);
    if (clsvalue != NULL)
    {
        rmkvid = strtoul(clsvalue, NULL, 0); 
    
    clsconfiginfo.rmkvidOp = RMK_CHANGE_VID;
    clsconfiginfo.rmkvid=rmkvid;
    
    }

    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_INSERTVID,NULL);
    if (clsvalue != NULL)
    {
        rmkvid = strtoul(clsvalue, NULL, 0);
    clsconfiginfo.rmkvidOp = RMK_INSERT_VID;
    clsconfiginfo.rmkvid=rmkvid;
    }
    #if 0
    #if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)
    vosStrCpy(clsconfiginfo.name, name);
    vosStrCpy(clsconfiginfo.ports, ports);	
    vosConfigValueSet(QOS_CLASS_SESSION,name,CLASSKEY_PORTS,ports);
    #endif
   
    status = dalClsRuleCtcAdd(&clsconfiginfo,&rule);
    if (status == NO_ERROR)
    {
        vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_INSTALL,rule);
    }
    #endif 
    if(clsconfiginfo.rulePri!=0)
    {
        status =odmOamClassInstall(0,clsconfiginfo.rulePri,&clsconfiginfo);
        odmDestroyClass(name);	
    }
    else
    {
        status = dalVoipClsRuleCtcAdd(&clsconfiginfo, &rule);
        if (status != NO_ERROR)
        {  
            return status;
        }
    
        vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_INSTALL,rule);
    }
    /*apply big field again*/
    #if 0
    while (ruleid[index])
    {
        vosMemSet(nametmp,0,20);
        if (odmGetClassNameByRule(ruleid[index],nametmp) == NO_ERROR)
        {
            odmRemoveClassEntry(nametmp,0);
            odmInstallClassEntry(nametmp,0, NULL);
        }
        index++;
    }
    #endif
    return status;
}

#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
int odmGetClassName
(
    CLS_CONFIG_INFO_t *stClassCfg,
    char *name
)
{
    return dalAclClsFindName(stClassCfg,name);
}

#else
int odmGetClassName
(
    CLS_CONFIG_INFO_t *stClassCfg,
    char *name
)
{
    UINT8  count;
    UINT8  roop;
    UINT16 ruleId = 0xfff;

    char   *session = NULL;
    if ((stClassCfg == NULL)||(name == NULL))
    {
        return QOS_INVALID_POINTER;
    }

    if (dalClaCfgCheck(stClassCfg,&ruleId) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }

    count = vosConfigSectionCount(QOS_CLASS_SESSION);

    for (roop = 0;roop < count;roop++)
    {
        if (vosConfigSectionGetByIndex(QOS_CLASS_SESSION,roop,&session) == 0)
        {
            if (ruleId == vosConfigUInt32Get(QOS_CLASS_SESSION,session,CLASSKEY_INSTALL,0))
            {
                vosStrCpy(name,session);
                return NO_ERROR;
            }

        }
    }

    return QOS_GENERAL_ERROR;
}
#endif




/*******************************************************************************
*
* odmOamClassRemove: 
*
* DESCRIPTION:
* this function remove a class entry from hw according to OAM packets.(only for oam module).
*
* INPUTS:
         CLS_CONFIG_INFO_t:classification packets struct.
* OUTPUTS:  
         ruleId:rule id.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmOamClassRemove
(
    UINT16 portId, 
    UINT8 precedence

)
{
    int  status;
    //char name[20] = {0};

    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"delete class port %d precedence %d ....\r\n",portId,precedence);

    //  vosSprintf(name,QOS_OAM_NAME,portId,precedence);
    //OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"delete class:%s\r\n",name);
    //status = odmRemoveClassEntry(name,0);
    oplOdmClsLock();
    status =OdmAclListDel(0,precedence);
    oplOdmClsUnLock();
    if (status != 0) 
    {
        CLASS_TRACE();
        return status;
    }
    
    //odmDestroyClass(name);
//    OamClsCount--;

    return NO_ERROR;
}
int odmoamClassRuleGet 
(
    UINT16 portId, 
    UINT8 precedence,
    CLS_CONFIG_INFO_t *clsRule
)
{
    int  status;
    //char name[20] = {0};
    //char *clsvalue = NULL;
    struct list_head *pos, *n;
    opl_acl_list_t *pClsinfo=NULL;
    CLS_CONFIG_INFO_t *stClassCfg;
    UINT8 str_ip[20] = "";

    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"Get class port %d precedence %d ....\r\n",portId,precedence);

    if (clsRule == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    oplOdmClsLock();
    pClsinfo=OdmAclListFindPre(precedence);
    oplOdmClsUnLock();
    if(pClsinfo ==NULL)
    {
        return QOS_INVALID_POINTER;
    }

    stClassCfg=&(pClsinfo->stClassCfg);
    vosMemCpy(clsRule,stClassCfg,sizeof(CLS_CONFIG_INFO_t));
    clsRule->rulePri = precedence; /*fix bug3581, odmClsRule.rulePri is the precedence from the olt configration and may be conflict.*/

    if(pClsinfo->rulepri>0)
    {
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"precedence %d \n",stClassCfg->rulePri);
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"queueMapId = %d \r\n",stClassCfg->queueMapId);
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"cosMapVal = %d \r\n",stClassCfg->cosMapVal);
        if(clsRule->dstMacFlag)
            OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"dstMac = %02x:%02x:%02x:%02x:%02x:%02x\r\n",stClassCfg->dstMac.lowRange[0],
                          stClassCfg->dstMac.lowRange[1],
                          stClassCfg->dstMac.lowRange[2],
                          stClassCfg->dstMac.lowRange[3],
                          stClassCfg->dstMac.lowRange[4],
                          stClassCfg->dstMac.lowRange[5]);
        if(clsRule->srcMacFlag)
            OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"srcMac = %02x:%02x:%02x:%02x:%02x:%02x\r\n",stClassCfg->srcMac.lowRange[0],
                                                      stClassCfg->srcMac.lowRange[1],
                                                      stClassCfg->srcMac.lowRange[2],
                                                      stClassCfg->srcMac.lowRange[3],
                                                      stClassCfg->srcMac.lowRange[4],
                                                      stClassCfg->srcMac.lowRange[5]);
				
				if(stClassCfg->etherPriFlag)
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"etherPri = %d \r\n",stClassCfg->etherPri.lowRange);
				if(stClassCfg->vlanIdFlag)
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"vlanId = %d \r\n",stClassCfg->vlanId.lowRange);
				if(stClassCfg->lenOrTypeFlag)
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"EtherType = %d \r\n",stClassCfg->lenOrType.lowRange);
				if(stClassCfg->ipTypeFlag)
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"ipType = %d \r\n",stClassCfg->ipType.lowRange);
				if(stClassCfg->dstIpFlag)
				{
					vosMemSet(str_ip,0,20);
					cliIpToStr(stClassCfg->dstIp.lowRange,str_ip);
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"dstIp = %s \r\n",str_ip);	
				}
				if(stClassCfg->srcIpFlag)
				{
					vosMemSet(str_ip,0,20);
					cliIpToStr(stClassCfg->srcIp.lowRange,str_ip);
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"srcIp = %s \r\n",str_ip);
				}
				if(stClassCfg->ipV4DscpFlag==1)				
					OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"ipV4Dscp = %d \r\n",(stClassCfg->ipV4Dscp.lowRange)>>5);
        if(stClassCfg->srcL4PortFlag)
            OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"srcL4Port = %d \r\n",stClassCfg->srcL4Port.lowRange);
        if(stClassCfg->dstL4PortFlag)
            OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"dstL4Port = %d \r\n",stClassCfg->dstL4Port.lowRange);
    }
#if 0
 vosSprintf(name,QOS_OAM_NAME,portId,precedence);
 if (vosConfigSectionIsExisted(QOS_CLASS_SESSION,name) != 0)
{
    return QOS_CLS_EXIST_CHECK_ERROR;
}    
 clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_QUEUE,NULL);
 if (clsvalue != NULL)
 {
     clsRule->queueMapId = strtoul(clsvalue, NULL, 0);
 }

 clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_PRIORITY,NULL);/* olt COS ?*/
 if (clsvalue != NULL)
 {
     clsRule->rulePri = strtoul(clsvalue, NULL, 0);
 }

 clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DESTMAC,NULL);
 if (clsvalue != NULL)
 {
     clsRule->dstMacFlag=1;
     OPL_MEMCPY(clsRule->dstMac.lowRange,clsvalue,6);
 }

 clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_SRCMAC,NULL);
 if (clsvalue != NULL)
 {
     clsRule->srcMacFlag=1;
     OPL_MEMCPY(clsRule->srcMac.lowRange,clsvalue,6);

 }
 
 clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_FCOS,NULL);
 if (clsvalue != NULL)
 {
        clsRule->etherPriFlag = 1;
        clsRule->etherPri.lowRange     = strtoul(clsvalue, NULL, 0); 
 }

 clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_FVID,NULL);
 if (clsvalue != NULL)
 {
    clsRule->vlanIdFlag = 1;
    clsRule->vlanId.lowRange     = strtoul(clsvalue, NULL, 0);	 
 }
 
 clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_TYPE,NULL);
 if (clsvalue != NULL)
 {
     clsRule->lenOrTypeFlag = 1;
     clsRule->lenOrType.lowRange	  = strtoul(clsvalue, NULL, 0);

 }
 
 clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DESTIP,NULL);
 if (clsvalue != NULL)
 {
     clsRule->dstIpFlag = 1;
     clsRule->dstIp.lowRange	  = utilStrToIpAddr(clsvalue);
 }

 clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_SRCIP,NULL);
 if (clsvalue != NULL)
 {
     clsRule->srcIpFlag = 1;
     clsRule->srcIp.lowRange	  = utilStrToIpAddr(clsvalue);
 }
 
 clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_PROTOCAL,NULL);
 if (clsvalue != NULL)
 {
     clsRule->ipTypeFlag = 1;
     clsRule->ipType.lowRange	   = strtoul(clsvalue, NULL, 0);  

 }

 clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DSCP,NULL);
 if (clsvalue != NULL)
 {
     clsRule->ipV4DscpFlag = 1;
     clsRule->ipV4Dscp.lowRange	 = strtoul(clsvalue, NULL, 0) ; 
 }
 
 clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_V6PRE,NULL);
 if (clsvalue != NULL)
 {
     clsRule->ipV6PreFlag = 1;
     clsRule->ipV6Pre.lowRange	 = strtoul(clsvalue, NULL, 0); 
 }
 
 clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DESTL4PORT,NULL);
 if (clsvalue != NULL)
 {
     clsRule->dstL4PortFlag = 1;
     clsRule->dstL4Port.lowRange	  = strtoul(clsvalue, NULL, 0); 

 }

 clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_SRCL4PORT,NULL);
 if (clsvalue != NULL)
 {
     clsRule->srcL4PortFlag = 1;
     clsRule->srcL4Port.lowRange	  = strtoul(clsvalue, NULL, 0); 

 }
#endif
    return NO_ERROR;
}

/*******************************************************************************
*
* odmOamClassRemoveAll: 
*
* DESCRIPTION:
* this function remove all class entry from hw according to OAM packets.(only for oam module).
*
* INPUTS:
         N/A
* OUTPUTS:  
         N/A

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmOamClassRemoveAll
(
    UINT16 ucPortS
)
{
    int ret;

    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"remove all class....\r\n");

    oplOdmClsLock();
    ret =  OdmAclListClearAll(ucPortS,0);
    oplOdmClsUnLock(); 

return ret;

   #if 0
UINT8  count=0;
UINT8  roop;
UINT16 ruleId ;
UINT8	*pBuff=NULL;
UINT32 portArry[ODM_NUM_OF_PORTS+ 1]={0};
char   *session = NULL;
QOS_BOOL en = QOS_FALSE;
int rv;
UINT8 portListBuff[255];

#if 0
count = vosConfigSectionCount(QOS_CLASS_SESSION);
while (count)
{

    if (vosConfigSectionGetByIndex(QOS_CLASS_SESSION,(count-1),&session) == 0)
    {
        odmCheckClassInstall(session,&en);

        pBuff = vosConfigValueGet(QOS_CLASS_SESSION,session,CLASSKEY_PORTS,NULL);
        if(pBuff==NULL)
        {
            count --;
            continue;    
        }
     
        vosStrCpy(portListBuff,pBuff);
        vosMemSet(portArry,0,sizeof(portArry));
        rv=convertPortListString2PortArry(portListBuff,portArry,ODM_NUM_OF_PORTS+1);
        if ( portArry[ucPortS]==1 ) 
        {
            if (en == QOS_TRUE)
            {
                odmRemoveClassEntry(session,0);
            }
            odmDestroyClass(session);
        }
    }
    count --;
 } 

#else 
    count = vosConfigSectionCount(QOS_CLASS_SESSION);

    while (count)
    {
    
    if (vosConfigSectionGetByIndex(QOS_CLASS_SESSION,(count-1),&session) == 0)
    {
     odmCheckClassInstall(session,&en);    
     if (en == QOS_TRUE) 
     {
         odmRemoveClassEntry(session,0);
         odmDestroyClass(session);
     }
     else
     {
         odmDestroyClass(session);
     }
     
    
    }
       count --;
    } 
#endif

#endif
}
/*******************************************************************************
*
* odmOamClassRemove: 
*
* DESCRIPTION:
* this function install a class entry to hw according to OAM packets.(only for oam module).
*
* INPUTS:
         CLS_CONFIG_INFO_t:classification packets struct.
* OUTPUTS:  
         ruleId:rule id.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmOamClassShowCls(int fd)
{
    oplOdmClsLock();
    OdmAclListShowWithRulePre(fd);
    oplOdmClsUnLock();
}
int odmOamClassInstall
(
    UINT16 portId,   
    UINT8 precedence,
    CLS_CONFIG_INFO_t *stClassCfg
)
{    
    int  status;
    char name[20] = {0};
    char tmp[20] = {0};

    UINT16 rule;
    UINT8  roop;

    if (stClassCfg == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"add class....\r\n");

#if 0
vosSprintf(stClassCfg->ports, "%d", portId);
#endif

    printoamclassdata(stClassCfg);

    //vosSprintf(name,QOS_OAM_NAME,portId,precedence);

    oplOdmClsLock();
    status =OdmAclListAdd(0,stClassCfg);
    oplOdmClsUnLock();

    //status =dalVoipClsRuleCtcAdd(stClassCfg, &rule);
#if 0
    if(stClassCfg->dstMacFlag)
{
    memset(tmp,0,20);
    vosSprintf(tmp, "%02x:%02x:%02x:%02x:%02x:%02x", 
                        stClassCfg->dstMac.lowRange[0], stClassCfg->dstMac.lowRange[1], 
                        stClassCfg->dstMac.lowRange[2], stClassCfg->dstMac.lowRange[3], 
                        stClassCfg->dstMac.lowRange[4], stClassCfg->dstMac.lowRange[5]);
    odmCfgClassDestMac(name,tmp,CLASS_ADD);
}

if(stClassCfg->srcMacFlag)
{
    memset(tmp,0,20);
    vosSprintf(tmp, "%02x:%02x:%02x:%02x:%02x:%02x",
                        stClassCfg->srcMac.lowRange[0],  stClassCfg->srcMac.lowRange[1], 
                        stClassCfg->srcMac.lowRange[2], stClassCfg->srcMac.lowRange[3], 
                        stClassCfg->srcMac.lowRange[4], stClassCfg->srcMac.lowRange[5]);
    odmCfgClassSrcMac(name,tmp,CLASS_ADD);
}

if(stClassCfg->etherPriFlag)
{
    odmCfgClassFCos(name,stClassCfg->etherPri.lowRange,CLASS_ADD);
}

if(stClassCfg->vlanIdFlag)
{
    odmCfgClassFVid(name,stClassCfg->vlanId.lowRange,CLASS_ADD);
}

if(stClassCfg->lenOrTypeFlag)
{
    odmCfgClassTypeLenght(name,stClassCfg->lenOrType.lowRange,CLASS_ADD);
}

if(stClassCfg->dstIpFlag)
{
    memset(tmp,0,20);
    vosSprintf(tmp, "%d.%d.%d.%d",
                        ((UINT8 *)&(stClassCfg->dstIp.lowRange))[0], ((UINT8 *)&(stClassCfg->dstIp.lowRange))[1],
                        ((UINT8 *)&(stClassCfg->dstIp.lowRange))[2], ((UINT8 *)&(stClassCfg->dstIp.lowRange))[3]);
    odmCfgClassDestIp(name,tmp,CLASS_ADD);
}

if(stClassCfg->srcIpFlag)
{
    memset(tmp,0,20);
    vosSprintf(tmp, "%d.%d.%d.%d",
                        ((UINT8 *)&(stClassCfg->srcIp.lowRange))[0], ((UINT8 *)&(stClassCfg->srcIp.lowRange))[1],
                        ((UINT8 *)&(stClassCfg->srcIp.lowRange))[2], ((UINT8 *)&(stClassCfg->srcIp.lowRange))[3]);
    odmCfgClassSrcIp(name,tmp,CLASS_ADD);
}

if(stClassCfg->ipTypeFlag)
{
    odmCfgClassIpType(name,stClassCfg->ipType.lowRange,CLASS_ADD);
}

if(stClassCfg->ipV4DscpFlag)
{
     odmCfgClassIpDscp(name,(stClassCfg->ipV4Dscp.lowRange)>>2,CLASS_ADD);
}

if ( stClassCfg->ipV6PreFlag )
{
    odmCfgClassV6Pre(name,stClassCfg->ipV6Pre.lowRange,CLASS_ADD);
}

if(stClassCfg->srcL4PortFlag)
{
    odmCfgClassSrcL4Port(name,stClassCfg->srcL4Port.lowRange,CLASS_ADD);
}

if(stClassCfg->dstL4PortFlag)
{
    odmCfgClassDestL4Port(name,stClassCfg->dstL4Port.lowRange,CLASS_ADD);
}

odmCfgClassMaskPri(name,stClassCfg->rulePri,CLASS_ADD);
odmCfgClassQueue(name,  stClassCfg->queueMapId,CLASS_ADD);
odmCfgClassEthPri(name, stClassCfg->cosMapVal,CLASS_ADD);

#if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)
//vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_PORTS,portId);
#endif

vosConfigUInt32Set(QOS_CLASS_SESSION,name,CLASSKEY_INSTALL,rule);
#endif
    return status;
}
int odmShowClass
(
    char *name,
    int  fd
)
{
    char *clsvalue = NULL;
    QOS_BOOL en;
    UINT8 t_act;

#if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    char *pPorts = NULL;
#endif

    if (name == NULL)
    {
        return QOS_INVALID_POINTER;
    }
    if (odmCheckClassExist(name) != NO_ERROR)
    {
        return QOS_CLS_EXIST_CHECK_ERROR;
    }
    if (odmCheckClassInstall(name,&en) != NO_ERROR)
    {
        return QOS_GENERAL_ERROR;
    }
    vosPrintf(fd,"class name: %s \r\n",name);
    vosPrintf(fd,"match:\r\n");
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DESTMAC,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      destinationmac:   %s\r\n",clsvalue);
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_SRCMAC,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      sourcemac:        %s\r\n",clsvalue);

    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DESTIP,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      destinationip:    %s\r\n",clsvalue);
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_SRCIP,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      sourceip:         %s\r\n",clsvalue);
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_TYPE,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      ethertype:        0x%04x\r\n",strtoul(clsvalue, NULL, 0));
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_FVID,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      vlanid:           %s\r\n",clsvalue);
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_FCOS,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      userpriority:     %s\r\n",clsvalue);
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DESTL4PORT,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      l4destinationport:%s\r\n",clsvalue);      
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_SRCL4PORT,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      l4sourceport:     %s\r\n",clsvalue);        
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_PROTOCAL,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      ipprotocol:       %s\r\n",clsvalue);        
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_TOS,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      tos:              %s\r\n",clsvalue);
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DSCP,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      dscp:             %s\r\n",clsvalue);
    }
    vosPrintf(fd,"\r\npolicy:\r\n");
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_QUEUE,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      queue-id:         %s\r\n",clsvalue);
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_PRIORITY,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      ethernetpriority: %s\r\n",clsvalue);
    }

    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_TRANFACT,NULL);
    if (clsvalue != NULL)
    {
        t_act = strtoul(clsvalue, NULL, 0); 
        switch (t_act)
        {
            case DROP:
                vosPrintf(fd,"      transferaction:   %s\r\n","drop");
                break;
            case PASS:
                vosPrintf(fd,"      transferaction:   %s\r\n","pass");
                break;
            case COPYTOCPU:
                vosPrintf(fd,"      transferaction:   %s\r\n","copytocpu");
                break;
            case SENDTOCPU:
                vosPrintf(fd,"      transferaction:   %s\r\n","sendtocpu");
                break;
        }
     }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_CHANGEVID,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      RemarkVid: %s\r\n",clsvalue);
    }
    clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_INSERTVID,NULL);
    if (clsvalue != NULL)
    {
        vosPrintf(fd,"      InsertVid: %s\r\n",clsvalue);
    }

      
    vosPrintf(fd,"\r\napply:\r\n");
    vosPrintf(fd,"      %s\r\n",(en == QOS_TRUE)?"enable":"disable");

#if 0
 vosPrintf(fd,"ports: \r\n");
 pPorts = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_PORTS,NULL);
 if(pPorts!=NULL)
{
    vosPrintf(fd,"      %s\r\n",pPorts);
}
else
{
    vosPrintf(fd,"      None\r\n");
}
#endif

    vosPrintf(fd,"----------------------------------------\r\n");
    return NO_ERROR;
}

int odmClassInit
(
    void
)
{
    UINT8  count;
    UINT8  roop;
    UINT16 ruleId ;

    char   *session = NULL;
    char   *ports = NULL;
    QOS_BOOL en     = QOS_FALSE;
    INT32 ret=0;
    INT32 portid;
    INT8 cosvalue;
    STREAM_TYPE_T stream = BOTH_STREAM_;

    vosMemSet(&g_acl_list, 0, sizeof(opl_acl_list_t));
    INIT_LIST_HEAD(&g_acl_list.list);

    if (VOS_SEM_OK != vosSemCreate(&g_odmClsLock, 0, 1))
    {
        OPL_PRINTF("classify module init failed.\n");
        return QOS_INIT_ERROR;
    }
    count = vosConfigSectionCount(QOS_CLASS_SESSION);
    for (roop = 0;roop < count;roop++)
    {
        if (vosConfigSectionGetByIndex(QOS_CLASS_SESSION,roop,&session) == 0)
        {
            if(odmCheckClassInstall(session,&en)==NO_ERROR)
            {
                if (en == QOS_TRUE)
                {
                    vosConfigUInt32Set(QOS_CLASS_SESSION,session,CLASSKEY_INSTALL,0xffff);
                    stream = vosConfigUInt32Get(QOS_CLASS_SESSION,session,CLASSKEY_DIRECTION,BOTH_STREAM_);
#if 0
     ports=vosConfigValueGet(QOS_CLASS_SESSION,session,CLASSKEY_PORTS,NULL);
     odmInstallClassEntry(session,1, ports);
#else
                    odmInstallClassEntry(session, stream, NULL);
#endif
                }

            }

        }
    }

    count = vosConfigSectionCount(QOS_QosSched_SESSION);
    for (roop = 0;roop < count;roop++)
    {

        if (vosConfigSectionGetByIndex(QOS_QosSched_SESSION,roop,&session) == 0)
        {
        
     #if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
            ret=vosConfigIntegerGet(QOS_QosSched_SESSION,session,PORT_SCHEDMODE,-1);
            if(ret!=-1)
            {     
                portid=vosStrToInt(session,NULL);
                //printf(" \n%s port id schedmode %d  %d\n",session,portid,ret);
                odmQosSchedModeSet(portid,ret);    
            }
     #endif
            ret=vosConfigIntegerGet(QOS_QosSched_SESSION,session,QOS_Cos2QMap,-1);     
            if(ret!=-1)
            {
                cosvalue=vosStrToInt(session+4,NULL);
                //printf("\n %s cos %d to qos set  %d\n",session,cosvalue,ret);
               
                /*Has been set in func tmInit*/
                #if 0
                odmQosCos2QMapSet(cosvalue,ret);
                #endif
         
            }
        }
    }

    {
      
      UINT8 tmi;
      char name[128];
      
      for (tmi =0; tmi<16; tmi ++)
      {
        UINT32 RLT;
        vosSprintf(name, CLASSQUEUELIMIT, tmi);
        RLT = vosConfigUInt32Get(QOS_CLASS_SESSION, CLASSQUEUIESECTION, name, ODM_QUEUE_LIMIT_NUM);
        
        tmQueueMaxCellNumHwWrite((tmi/8), (tmi%8), RLT);
      }
    }

    return NO_ERROR;
}

int odmQueueLimitSet(int queueType, int queueId, int queueBlock)
{
    UINT8 tmi;
    char name[128];
    int ret = 0;
    
    tmi = queueType * 8 + queueId;
    vosSprintf(name, CLASSQUEUELIMIT, tmi);
    vosConfigUInt32Set(QOS_CLASS_SESSION,CLASSQUEUIESECTION,name, queueBlock);
    ret = tmQueueMaxCellNumHwWrite(queueType, queueId, queueBlock);
    
    return ret;
}

int odmCheckQosBlockExist
(
   char *name
)
{
    int status;
    
   if (name == NULL)
   {
        return QOS_INVALID_POINTER;
    }
    return (vosConfigSectionIsExisted(QOS_QOSBLOCK_SESSION,name));
   
}
void printoamclassdata
(
    CLS_CONFIG_INFO_t *stClassCfg
)
{
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"valid = %d\r\n",stClassCfg->valid);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"tact = %d\r\n",stClassCfg->t_act);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"rulePri = %d \r\n",stClassCfg->rulePri);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"queueMapId = %d \r\n",stClassCfg->queueMapId);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"cosMapVal = %d \r\n",stClassCfg->cosMapVal);

    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"--------------------\r\n");

    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"dstMacFlag = %d \r\n",stClassCfg->dstMacFlag);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"srcMacFlag = %d \r\n",stClassCfg->srcMacFlag);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"etherPriFlag = %d \r\n",stClassCfg->etherPriFlag);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"vlanIdFlag = %d \r\n",stClassCfg->vlanIdFlag);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"lenOrTypeFlag = %d \r\n",stClassCfg->lenOrTypeFlag);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"dstIpFlag = %d \r\n",stClassCfg->dstIpFlag);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"srcIpFlag = %d \r\n",stClassCfg->srcIpFlag);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"ipTypeFlag = %d \r\n",stClassCfg->ipTypeFlag);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"ipV4DscpFlag = %d \r\n",stClassCfg->ipV4DscpFlag);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"ipV6PreFlag = %d \r\n",stClassCfg->ipV6PreFlag);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"srcL4PortFlag = %d \r\n",stClassCfg->srcL4PortFlag);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"dstL4PortFlag = %d \r\n",stClassCfg->dstL4PortFlag);

    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"--------------------\r\n");
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"dstMac = %02x:%02x:%02x:%02x:%02x:%02x\r\n",stClassCfg->dstMac.lowRange[0],
                                                      stClassCfg->dstMac.lowRange[1],
                                                      stClassCfg->dstMac.lowRange[2],
                                                      stClassCfg->dstMac.lowRange[3],
                                                      stClassCfg->dstMac.lowRange[4],
                                                      stClassCfg->dstMac.lowRange[5]);
                                                      
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"srcMac = %02x:%02x:%02x:%02x:%02x:%02x\r\n",stClassCfg->srcMac.lowRange[0],
                                                      stClassCfg->srcMac.lowRange[1],
                                                      stClassCfg->srcMac.lowRange[2],
                                                      stClassCfg->srcMac.lowRange[3],
                                                      stClassCfg->srcMac.lowRange[4],
                                                      stClassCfg->srcMac.lowRange[5]);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"etherPri = %d \r\n",stClassCfg->etherPri.lowRange);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"vlanId = %d \r\n",stClassCfg->vlanId.lowRange);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"lenOrType = %d \r\n",stClassCfg->lenOrType.lowRange);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"dstIp = %x \r\n",stClassCfg->dstIp.lowRange);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"srcIp = %x \r\n",stClassCfg->srcIp.lowRange);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"ipType = %d \r\n",stClassCfg->ipType.lowRange);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"ipV4Dscp = %d \r\n",stClassCfg->ipV4Dscp.lowRange);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"ipV6Pre = %d \r\n",stClassCfg->ipV6Pre.lowRange);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"srcL4Port = %d \r\n",stClassCfg->srcL4Port.lowRange);
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING,"dstL4Port = %d \r\n",stClassCfg->dstL4Port.lowRange);

    return;
}

void odmShowClassList
(
    int fd,
    int flag
)
{
    UINT8  count;
    UINT8  roop;
    UINT16 ruleId ;
    UINT16 Rulecnt=0;
    char   *session = NULL;
    QOS_BOOL en     = QOS_FALSE;
    
    count = vosConfigSectionCount(QOS_CLASS_SESSION);
    if (1 != flag)
        vosPrintf(fd,"Class Name List: \r\n");
    for (roop = 0;roop < count;roop++)
    {
        if (vosConfigSectionGetByIndex(QOS_CLASS_SESSION,roop,&session) == 0)
        {
           if (1 == flag)
           {
               odmShowClass(session,fd);
               Rulecnt++;
               vosPrintf(fd,"\r\n");
           }
           else
           {
               vosPrintf(fd,"   %s\r\n",session);
           }

        }
    }
    if (1 == flag && 0!=Rulecnt)
    {
        vosPrintf(fd,"Number Of Rules:	%d\r\n",Rulecnt);
    }
    return ;
    

    return ;
}

int odmQosCos2QMapGet(UINT8 VlanCos, UINT8 *QueId)
{
    UINT32 mode;	
    char name[20]={0};

    vosSprintf(name,PORT_CosRegen,VlanCos);
    if (odmCheckQosSchedExist(name) != NO_ERROR)
    {
        if(tmCosIcosMapHwRead(UP_STREAM,VlanCos,QueId)!=NO_ERROR)
            return QOS_GENERAL_ERROR;	
        else
            return NO_ERROR;
    }
    *QueId=vosConfigUInt32Get(QOS_QosSched_SESSION, name,
                                             QOS_Cos2QMap,0); 
    return NO_ERROR;
}

#ifdef ONU_1PORT_V1E_RF100_GNA_ATE
/*add by xukang for ate test--begin*/
int odmTestInit(void)
{
    INT32 retVal=0;
    INT32 TempVal1=0;
    INT32 TempVal2=0;
    char *name = "1";
    char *mac = "00:06:02:00:00:00";

    retVal = oplRegRead(0x000e*4,&TempVal1);
    if(OK != retVal)
    {
        printf("read 0x000e failed.\n");
        return QOS_INIT_ERROR;
    }
    
    TempVal1 |= 0x1;
    retVal = oplRegWrite(0x000e*4, TempVal1);
    if(OK != retVal)
    {
        printf("set 0x000e failed.\n");
        return QOS_INIT_ERROR;
    }

    retVal = oplRegRead(0x0056*4,&TempVal2);
    printf("---debug1---TempVal2=%d\r\n",TempVal2);
    if(OK != retVal)
    {
        printf("read 0x0056 failed.\n");
        return QOS_INIT_ERROR;
    }
    TempVal2 |= 0x80000;/*set bit 19 to enable PON interanl loopback*/
    printf("---debug2---TempVal2=%d\r\n",TempVal2);
    retVal = oplRegWrite(0x0056*4, TempVal2);
    
    retVal = oplRegRead(0x0056*4,&TempVal2);
    TempVal2&=0xffefffff;/*set bit 20 to be o for  enable PON interanl loopback*/
    retVal = oplRegWrite(0x0056*4, TempVal2);
    if(OK != retVal)
    {
        printf("set 0x0056 failed.\n");
        return QOS_INIT_ERROR;
    }
    

    
    
    retVal = odmCreateClass(name);
    if(OK != retVal)
    {
        printf("create class failed.\n");
        return QOS_INIT_ERROR;
    }
  
    retVal = odmCfgClassDestMac(name,mac,CLASS_ADD);
    if(OK != retVal)
    {
        printf("destmac cfg failed.\n");
        return QOS_INIT_ERROR;
    }
    
    retVal = odmCfgClassEthPri(name,6,CLASS_ADD);
    if (retVal ==  QOS_CLS_EXIST_CHECK_ERROR)
        printf("%%This class name is not exist!\r\n");
    else if (retVal ==  QOS_CLS_INSTALL_CHECK_ERROR)
        printf("%%This class has been apply,denial config again!\r\n");

    retVal = odmCfgClassQueue(name,5,CLASS_ADD);
    if (retVal ==  QOS_CLS_EXIST_CHECK_ERROR)
        printf("%%This class name is not exist!\r\n");
    else if (retVal ==  QOS_CLS_INSTALL_CHECK_ERROR)
        printf("%%This class has been apply,denial config again!\r\n");

    retVal = odmInstallClassEntry(name, BOTH_STREAM_, NULL);
    if (NO_ERROR == retVal)
    {
        printf("%%successfully!\r\n");
    }
    else if (retVal ==  QOS_CLS_EXIST_CHECK_ERROR)
        printf("%%This class name is not exist!\r\n");
    else if (retVal ==  QOS_CLS_INSTALL_CHECK_ERROR)
        printf("%%This class has been apply,denial apply again!\r\n");
    else 
    {
        printf("%%failed!\r\n");
    }
    
    return NO_ERROR;
}
/*add by xukang for ate test--end*/
//#endif
#endif
int odmQosCos2QMapSet(UINT8 VlanCos,UINT8 QueId)
{
    UINT32 mode;
    char name[20]={0};

    if(QueId>MAX_QOS_QUEUE)
    {
        return QOS_GENERAL_ERROR;
    }
    vosSprintf(name,PORT_CosRegen,VlanCos);
    if (odmCheckQosSchedExist(name) != NO_ERROR)
    {
        vosConfigSectionCreate(QOS_QosSched_SESSION,name);
    }
    vosConfigUInt32Set(QOS_QosSched_SESSION, name,QOS_Cos2QMap,QueId); 
    if(tmCosIcosMapHwWrite(UP_STREAM,VlanCos,QueId)!=0)
        return QOS_GENERAL_ERROR;
  
    if(tmCosIcosMapHwWrite(DOWN_STREAM,VlanCos,QueId)!=0)
        return QOS_GENERAL_ERROR;
    

    return NO_ERROR;
}

int odmQosSchedModeGet(UINT32 portId, UINT32 *pSchedMode)
{
    UINT32 mode;
    char name[20];

    vosSprintf(name,"port%4d",portId);
    if (odmCheckQosSchedExist(name) != NO_ERROR)
    {
        //if(dalQosSchModeGet(portId,pSchedMode)!=NO_ERROR)
        return QOS_GENERAL_ERROR;
        //else
        return NO_ERROR;
    }
    *pSchedMode=vosConfigUInt32Get(QOS_QosSched_SESSION, name,
                                             PORT_SCHEDMODE,0); 
    return NO_ERROR;
}

int odmQosSchedModeSet(UINT32 portId, UINT32 pSchedMode)
{
    UINT32 mode;
    char name[20];
    if(pSchedMode>QOS_MODE_MAX)
    {
        return QOS_GENERAL_ERROR;
    }
    vosSprintf(name,"port%4d",portId);
    if (odmCheckQosSchedExist(name) != NO_ERROR)
    {
        vosConfigSectionCreate(QOS_QosSched_SESSION,name);
    }
    vosConfigUInt32Set(QOS_QosSched_SESSION, name,PORT_SCHEDMODE,pSchedMode); 
    //if(dalQosSchModeSet(portId,pSchedMode)!=0)
        return QOS_GENERAL_ERROR;
    return NO_ERROR;
}

int odmQosPortPriRemapSet(UINT32 portId, UINT32 OldVlanCos,UINT32 NewVlanCos)
{
    UINT32 mode;
    char name[20];
    char section[20];

    if(NewVlanCos>MAX_QOS_COS)
    {
        return QOS_GENERAL_ERROR;
    }

    vosSprintf(name,"port%4d",portId);
    if (odmCheckQosSchedExist(name) != NO_ERROR)
    {
        vosConfigSectionCreate(QOS_QosSched_SESSION,name);
    }
    vosSprintf(section, PORT_CosRegen, OldVlanCos);
    vosConfigUInt32Set(QOS_QosSched_SESSION, name,section,NewVlanCos); 
    //if(dalQosPortCosRemapSet(portId,OldVlanCos,NewVlanCos)!=0)
        return QOS_GENERAL_ERROR;
    return NO_ERROR;
}

int odmQosPortPriRemapGet(UINT32 portId, UINT32 OldVlanCos,UINT32 *NewVlanCos)
{
    UINT32 mode;
    char name[20];
    char section[20];

    vosSprintf(name,"port%4d",portId);
    if (odmCheckQosSchedExist(name) != NO_ERROR)
    {
        return NO_ERROR;
    }
    vosSprintf(section, PORT_CosRegen, OldVlanCos);
    *NewVlanCos=vosConfigUInt32Get(QOS_QosSched_SESSION, name,section,0); 

    return NO_ERROR;
}
int odmQosPortPriRemapStatusSet(UINT32 portId,UINT32 Status)
{
    UINT32 mode;
    char name[20];
    char section[20];
    int i;

    vosSprintf(name,"port%4d",portId);
    if (odmCheckQosSchedExist(name) != NO_ERROR)
    {
        return NO_ERROR;
    }
    if(Status!=1)
    {
        for(i=MIN_QOS_COS;i<MAX_QOS_COS;++i)
        {
            vosSprintf(section,PORT_CosRegen,i);    
            vosConfigKeyDelete(QOS_QosSched_SESSION, name,section); 
        }
    }
    return NO_ERROR;
}

int odmQosConfigDump(int fd,UINT16 portId,int flag)
{
    struct list_head *head;
    struct list_head *pos;
    opl_acl_list_t *pClsinfo=NULL;
    CLS_CONFIG_INFO_t *stClassCfg;
    UINT8   name[32] ={0};	
    UINT8 str_ip[20] = "";

    oplOdmClsLock();
    head=&g_acl_list.list;
    if(head==NULL)
    {
        oplOdmClsUnLock();
        return QOS_INVALID_POINTER;
    }

    pClsinfo=list_entry(head,opl_acl_list_t,list);
    if(pClsinfo->prinum > 0)
    {
        vosPrintf(fd,"!\n");
        vosPrintf(fd,"qos\n");
    }
    list_for_each_prev(pos, head) {
    pClsinfo=list_entry(pos,opl_acl_list_t,list);
    stClassCfg=&(pClsinfo->stClassCfg);
    if(flag || pClsinfo->rulepri>0)
    {
    
        vosMemSet(name,0,32);
        vosSprintf(name,QOS_OAM_NAME,portId,pClsinfo->rulepri);
        vosPrintf(fd," create class %s\n",name);
        vosPrintf(fd," class %s\n",name);
        if(stClassCfg->dstMacFlag)
            vosPrintf(fd,"  match dstmac %02x:%02x:%02x:%02x:%02x:%02x\r\n",stClassCfg->dstMac.lowRange[0],
                          stClassCfg->dstMac.lowRange[1],
                          stClassCfg->dstMac.lowRange[2],
                          stClassCfg->dstMac.lowRange[3],
                          stClassCfg->dstMac.lowRange[4],
                          stClassCfg->dstMac.lowRange[5]);
        if(stClassCfg->srcMacFlag)
            vosPrintf(fd,"  match srcmac %02x:%02x:%02x:%02x:%02x:%02x\r\n",stClassCfg->srcMac.lowRange[0],
                                                      stClassCfg->srcMac.lowRange[1],
                                                      stClassCfg->srcMac.lowRange[2],
                                                      stClassCfg->srcMac.lowRange[3],
                                                      stClassCfg->srcMac.lowRange[4],
                                                      stClassCfg->srcMac.lowRange[5]);
				
				if(stClassCfg->etherPriFlag)
					vosPrintf(fd,"  match userpriority %d\r\n",stClassCfg->etherPri.lowRange);
				if(stClassCfg->vlanIdFlag)
					vosPrintf(fd,"  match vlanid %d\r\n",stClassCfg->vlanId.lowRange);
				if(stClassCfg->lenOrTypeFlag)
					vosPrintf(fd,"  match ethertype %d\r\n",stClassCfg->lenOrType.lowRange);
				if(stClassCfg->ipTypeFlag)
					vosPrintf(fd,"  match ipprotocol %d\r\n",stClassCfg->ipType.lowRange);
				if(stClassCfg->dstIpFlag)
				{
					vosMemSet(str_ip,0,20);
					cliIpToStr(stClassCfg->dstIp.lowRange,str_ip);
					vosPrintf(fd,"  match dstipv4 %s\r\n",str_ip);	
				}
				if(stClassCfg->srcIpFlag)
				{		
				    vosMemSet(str_ip,0,20);
					cliIpToStr(stClassCfg->srcIp.lowRange,str_ip);
					vosPrintf(fd,"  match srcipv4 %s\r\n",str_ip);
				}
				if(stClassCfg->ipV4DscpFlag==1)				
				vosPrintf(fd,"  match dscp %d\r\n",(stClassCfg->ipV4Dscp.lowRange)>>5);
     if(stClassCfg->srcL4PortFlag)
     vosPrintf(fd,"  match l4srcport %d\r\n",stClassCfg->srcL4Port.lowRange);
     if(stClassCfg->dstL4PortFlag)
     vosPrintf(fd,"  match l4dstport %d\r\n",stClassCfg->dstL4Port.lowRange);

     vosPrintf(fd,"  set queue %d\r\n",stClassCfg->queueMapId);
     vosPrintf(fd,"  set ethernetpriority %d\r\n",stClassCfg->cosMapVal);
                vosPrintf(fd,"  set precedence %d\r\n",pClsinfo->rulepri);
     vosPrintf(fd,"  apply upstream\r\n",stClassCfg->cosMapVal);
     vosPrintf(fd,"  exit\r\n",stClassCfg->cosMapVal);
    }
}
    oplOdmClsUnLock();

}

int odmOnuClassifySet(UINT32 type, UINT32 transfAct,UINT32 portId,UINT32 flag,char *pValue,char *pName)
{
    UINT16	usRuleId;
    int    iStatus = 0;
    UINT8   acMac[6] = {0};
    UINT8   acIp[16] ={0};	
    UINT32  vlanId;
    UINT32  etherType;
    UINT32  cos;
    UINT32  tos;
    UINT32  ipProtocal;
    UINT32  userPri;
    UINT32  srcPort;
    UINT32  destPort;
    UINT32  etherPri;

    CLS_CONFIG_INFO_t	stClassCfg;
    vosMemSet(&stClassCfg, 0, sizeof(CLS_CONFIG_INFO_t));    

    switch (transfAct)
    {
        case DROP:
            stClassCfg.t_act = DROP_PKTS;
            break;
        case PASS:
            stClassCfg.t_act = DONOT_COPY_TO_CPU;
            break;
        case COPYTOCPU: 
            stClassCfg.t_act = COPY_PKTS_TO_CPU;
            break;
        case SENDTOCPU:
            stClassCfg.t_act = (DROP_PKTS|COPY_PKTS_TO_CPU);
            break;
        default:
            return QOS_INVALID_TYPE;
    }
    switch (type)
    {
        case CLASS_DSTMAC:
            vosStrToMac(pValue, acMac);
            stClassCfg.dstMacFlag = 1;
            vosMemCpy (stClassCfg.dstMac.lowRange, acMac, 6);
            break;
        case CLASS_SRCMAC:
            vosStrToMac(pValue, acMac);
            stClassCfg.srcMacFlag = 1;
            vosMemCpy (stClassCfg.srcMac.lowRange, acMac, 6);
            break;
        case CLASS_ETHERPRI:    
            etherPri = vosStrToInt(pValue, NULL);
            stClassCfg.etherPriFlag = 1;
            stClassCfg.etherPri.lowRange = etherPri;
            break;
        case CLASS_VID:
            vlanId = vosStrToInt(pValue, NULL);
            stClassCfg.vlanIdFlag = 1;
            stClassCfg.vlanId.lowRange = vlanId;
            break;
        case CLASS_ETHERTYPE:
            etherType = vosStrToInt(pValue, NULL);
            stClassCfg.lenOrTypeFlag = 1;
            stClassCfg.lenOrType.lowRange = etherType;
            break;
        case CLASS_DSTIP:
            stClassCfg.dstIpFlag = 1;
            stClassCfg.dstIp.lowRange = string2Ip(pValue);
            break;
        case CLASS_SRCIP:
            stClassCfg.srcIpFlag = 1;
            stClassCfg.srcIp.lowRange = string2Ip(pValue);
            break;
        case CLASS_IPPRO:
            ipProtocal = vosStrToInt(pValue, NULL);
            stClassCfg.ipTypeFlag = 1;
            stClassCfg.ipType.lowRange = ipProtocal;
            break;
        case CLASS_IPDSCP:
            tos = vosStrToInt(pValue, NULL);
            stClassCfg.ipV4DscpFlag = 1;
            stClassCfg.ipV4Dscp.lowRange = tos<<2;
            break;
        case CLASS_DSTPORT:
            destPort = vosStrToInt(pValue, NULL);
            stClassCfg.dstL4PortFlag = 1;
            stClassCfg.dstL4Port.lowRange = destPort;
            break;
        case CLASS_SRCPORT:
            srcPort = vosStrToInt(pValue, NULL);
            stClassCfg.srcL4PortFlag = 1;
            stClassCfg.srcL4Port.lowRange = srcPort;
            break;
        default:
            break;
     }

    #if 0
    vosSprintf(stClassCfg.ports, "%d", portId);
    #endif

#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)
    vosStrCpy(stClassCfg.name,pName);
#endif

    if(0 == flag)
    {
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "classify delete\n");
        iStatus = odmClassWebRemove(pName, &stClassCfg, &usRuleId);
    }
    else if(1 == flag)
    {
        OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "classify add\n");
        iStatus = odmClassWebInstall(pName, &stClassCfg, &usRuleId,transfAct);
    }


    return iStatus;
}


/*******************************************************************************
*
* odmOamClassRemove: 
*
* DESCRIPTION:
* this function remove a class entry from hw according to OAM packets.(only for oam module).
*
* INPUTS:
         CLS_CONFIG_INFO_t:classification packets struct.
* OUTPUTS:  
         ruleId:rule id.

* RETURN:
*  NO_ERROR(0)--success.
      others error.
*
* SEE ALSO: 
*/
int odmClassWebRemove
(
    char *pName,
    CLS_CONFIG_INFO_t *stClassCfg, 
    UINT16 *ruleId
)
{
    int  status;
    int  count = 0;
    int  roop = 0;
    char name[20] = {0};
    char   *session = NULL;
    UINT32 clsvalue;

    if ((stClassCfg == NULL)||(ruleId == NULL))
    {
        return ERR_NULL_POINTER;
    }
    clsvalue = vosConfigUInt32Get(QOS_CLASS_SESSION,pName,CLASSKEY_INSTALL,0xffff);
    if (clsvalue != 0xffff)
    {
        dalVoipClsDelEntry(clsvalue);
    }

#if 0
status = dalClsRuleCtcDel(stClassCfg, ruleId);
if (OPL_OK != status) {
  OPL_LOG_TRACE();
   return status;
}

#endif
    vosConfigSectionDelete(QOS_CLASS_SESSION,pName);

    return NO_ERROR;
}

int odmClassWebInstall
(
    char *pName,
    CLS_CONFIG_INFO_t *stClassCfg, 
    UINT16 *ruleId,
    UINT32 transfAct
)
{    
    char name[20] = {0};
    char tmp[20] = {0};
    int  status;
    UINT16 rule;
    UINT8  roop;

    if (odmCheckClassExist(pName) == 0)
    {
        CLASS_TRACE();
        return QOS_GENERAL_ERROR;
    }
    if (odmCreateClass(pName) != 0) 
    {
        CLASS_TRACE();
        return QOS_GENERAL_ERROR;
    }
#if 0
 status = dalClsRuleCtcAdd(stClassCfg, ruleId);
 if (OPL_OK != status) {
  OPL_LOG_TRACE();
  odmDestroyClass(pName);
  return status;
}
#endif
    status = dalVoipClsRuleCtcAdd(stClassCfg, ruleId);
    if (OPL_OK != status) {
    OPL_LOG_TRACE();
    odmDestroyClass(pName);
    return status;
    }

    if(stClassCfg->dstMacFlag)
    {
        vosMemSet(tmp,0,20);
        vosSprintf(tmp, "%02x:%02x:%02x:%02x:%02x:%02x", 
                        stClassCfg->dstMac.lowRange[0], stClassCfg->dstMac.lowRange[1], 
                        stClassCfg->dstMac.lowRange[2], stClassCfg->dstMac.lowRange[3], 
                        stClassCfg->dstMac.lowRange[4], stClassCfg->dstMac.lowRange[5]);
        odmCfgClassDestMac(pName,tmp,CLASS_ADD);
    }

    if(stClassCfg->srcMacFlag)
    {
        vosMemSet(tmp,0,20);
        vosSprintf(tmp, "%02x:%02x:%02x:%02x:%02x:%02x",
                        stClassCfg->srcMac.lowRange[0],  stClassCfg->srcMac.lowRange[1], 
                        stClassCfg->srcMac.lowRange[2], stClassCfg->srcMac.lowRange[3], 
                        stClassCfg->srcMac.lowRange[4], stClassCfg->srcMac.lowRange[5]);
        odmCfgClassSrcMac(pName,tmp,CLASS_ADD);
    }

    if(stClassCfg->etherPriFlag)
    {
        odmCfgClassFCos(pName,stClassCfg->etherPri.lowRange,CLASS_ADD);
    }

    if(stClassCfg->vlanIdFlag)
    {
        odmCfgClassFVid(pName,stClassCfg->vlanId.lowRange,CLASS_ADD);
    }

    if(stClassCfg->lenOrTypeFlag)
    {
        odmCfgClassTypeLenght(pName,stClassCfg->lenOrType.lowRange,CLASS_ADD);
    }

    if(stClassCfg->dstIpFlag)
    {
        vosMemSet(tmp,0,20);
        vosSprintf(tmp, "%d.%d.%d.%d",
                        ((UINT8 *)&(stClassCfg->dstIp.lowRange))[0], ((UINT8 *)&(stClassCfg->dstIp.lowRange))[1],
                        ((UINT8 *)&(stClassCfg->dstIp.lowRange))[2], ((UINT8 *)&(stClassCfg->dstIp.lowRange))[3]);
        odmCfgClassDestIp(pName,tmp,CLASS_ADD);
    }

    if(stClassCfg->srcIpFlag)
    {
        vosMemSet(tmp,0,20);
        vosSprintf(tmp, "%d.%d.%d.%d",
                        ((UINT8 *)&(stClassCfg->srcIp.lowRange))[0], ((UINT8 *)&(stClassCfg->srcIp.lowRange))[1],
                        ((UINT8 *)&(stClassCfg->srcIp.lowRange))[2], ((UINT8 *)&(stClassCfg->srcIp.lowRange))[3]);
        odmCfgClassSrcIp(pName,tmp,CLASS_ADD);
    }

    if(stClassCfg->ipTypeFlag)
    {
        odmCfgClassIpType(pName,stClassCfg->ipType.lowRange,CLASS_ADD);
    }

    if(stClassCfg->ipV4DscpFlag)
    {
        odmCfgClassIpDscp(pName,(stClassCfg->ipV4Dscp.lowRange)>>2,CLASS_ADD);
    }

    if(stClassCfg->srcL4PortFlag)
    {
        odmCfgClassSrcL4Port(pName,stClassCfg->srcL4Port.lowRange,CLASS_ADD);
    }

    if(stClassCfg->dstL4PortFlag)
    {
        odmCfgClassDestL4Port(pName,stClassCfg->dstL4Port.lowRange,CLASS_ADD);
    }

    odmCfgClassTranfAct(pName,transfAct,CLASS_ADD);
#if 0
#if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)
vosConfigValueSet(QOS_CLASS_SESSION,stClassCfg->name,CLASSKEY_PORTS,stClassCfg->ports);
#endif
#endif
    vosConfigUInt32Set(QOS_CLASS_SESSION,pName,CLASSKEY_INSTALL,*ruleId);

    return NO_ERROR;
}

int odmClassifyWebTotalNumGet(UINT32 *entryTotalNum)
{
    UINT8  count;
    count = vosConfigSectionCount(QOS_CLASS_SESSION);
    *entryTotalNum = count;
    return NO_ERROR;

}

int odmClassifyWebGet(UINT32 curCount,UINT32 *entryNum,UINT8 *pValue)
{

    UINT32  count;
    UINT8  roop;

    char   *session = NULL;
    char   *clsvalue = NULL;
    onu_filter_t  *pOnuFilterCfg;
    UINT32 match=0;
    UINT8 t_act=0;
    char name[20] = {0};
    UINT32 portId =0;
 
    count = vosConfigSectionCount(QOS_CLASS_SESSION);
    for (roop = (curCount-1)*19;((roop < curCount*19) &&(roop<count)) ;roop++)
    {
        if (vosConfigSectionGetByIndex(QOS_CLASS_SESSION,roop,&session) == 0)
        {
            vosStrCpy(name,session);
            t_act = vosConfigUInt32Get(QOS_CLASS_SESSION,name,CLASSKEY_TRANFACT,0);

#if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)       
            portId = vosConfigUInt32Get(QOS_CLASS_SESSION,name,CLASSKEY_PORTS,0);
#endif

            clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DESTMAC,NULL);
            if (clsvalue != NULL)
            {
                pOnuFilterCfg=(onu_filter_t *)(pValue + match*sizeof(onu_filter_t));
                pOnuFilterCfg->type = CLASS_DSTMAC;
                pOnuFilterCfg->transfAct =  t_act ;
                pOnuFilterCfg->portId = portId ;
                vosStrCpy(pOnuFilterCfg->value,clsvalue);
                vosStrCpy(pOnuFilterCfg->name,name);
                match++;
            }
            clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_SRCMAC,NULL);
            if (clsvalue != NULL)
            {
                pOnuFilterCfg=(onu_filter_t *)(pValue+ match*sizeof(onu_filter_t));
                pOnuFilterCfg->type = CLASS_SRCMAC;
                pOnuFilterCfg->transfAct =  t_act ;
                pOnuFilterCfg->portId = portId ;
                vosStrCpy(pOnuFilterCfg->value,clsvalue);
                vosStrCpy(pOnuFilterCfg->name,name);
                match++;
            }
            clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DESTIP,NULL);
            if (clsvalue != NULL)
            {
                pOnuFilterCfg=(onu_filter_t *)(pValue+ match*sizeof(onu_filter_t));
                pOnuFilterCfg->type = CLASS_DSTIP;
                pOnuFilterCfg->transfAct =  t_act ;
                pOnuFilterCfg->portId = portId ;
                vosStrCpy(pOnuFilterCfg->value,clsvalue);
                vosStrCpy(pOnuFilterCfg->name,name);
                match++;
            }
            clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_SRCIP,NULL);
            if (clsvalue != NULL)
            {
                pOnuFilterCfg=(onu_filter_t *)(pValue+ match*sizeof(onu_filter_t));
                pOnuFilterCfg->type = CLASS_SRCIP;
                pOnuFilterCfg->transfAct =  t_act ;
                pOnuFilterCfg->portId = portId ;
                vosStrCpy(pOnuFilterCfg->value,clsvalue);
                vosStrCpy(pOnuFilterCfg->name,name);
                match++;
            }
            clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_TYPE,NULL);
            if (clsvalue != NULL)
            {
                pOnuFilterCfg=(onu_filter_t *)(pValue+ match*sizeof(onu_filter_t));
                pOnuFilterCfg->type = CLASS_ETHERTYPE;
                pOnuFilterCfg->transfAct =  t_act ;
                pOnuFilterCfg->portId = portId ;
                vosStrCpy(pOnuFilterCfg->value,clsvalue);
                vosStrCpy(pOnuFilterCfg->name,name);
                match++;
            }
            clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_FVID,NULL);
            if (clsvalue != NULL)
            {
                pOnuFilterCfg=(onu_filter_t *)(pValue+ match*sizeof(onu_filter_t));
                pOnuFilterCfg->type = CLASS_VID;
                pOnuFilterCfg->transfAct =  t_act ;
                vosStrCpy(pOnuFilterCfg->value,clsvalue);
                pOnuFilterCfg->portId = portId ;
                vosStrCpy(pOnuFilterCfg->name,name);
                match++;
            }
            clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_FCOS,NULL);
            if (clsvalue != NULL)
            {
                pOnuFilterCfg=(onu_filter_t *)(pValue+ match*sizeof(onu_filter_t));
                pOnuFilterCfg->type = CLASS_ETHERPRI;
                pOnuFilterCfg->transfAct =  t_act ;    
                pOnuFilterCfg->portId = portId ;
                vosStrCpy(pOnuFilterCfg->value,clsvalue);
                vosStrCpy(pOnuFilterCfg->name,name);
                match++;
            }
            clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DESTL4PORT,NULL);
            if (clsvalue != NULL)
            {
                pOnuFilterCfg=(onu_filter_t *)(pValue+ match*sizeof(onu_filter_t));
                pOnuFilterCfg->type = CLASS_DSTPORT;
                pOnuFilterCfg->transfAct =  t_act ;
                pOnuFilterCfg->portId = portId ;
                vosStrCpy(pOnuFilterCfg->value,clsvalue);
                vosStrCpy(pOnuFilterCfg->name,name);
                match++;   
            }
            clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_SRCL4PORT,NULL);
            if (clsvalue != NULL)
            {
                pOnuFilterCfg=(onu_filter_t *)(pValue+match*sizeof(onu_filter_t));
                pOnuFilterCfg->type = CLASS_SRCPORT;
                pOnuFilterCfg->transfAct =  t_act ;
                pOnuFilterCfg->portId = portId ;
                vosStrCpy(pOnuFilterCfg->value, clsvalue);
                vosStrCpy(pOnuFilterCfg->name,name);
                match++;      
            }
            clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_PROTOCAL,NULL);
            if (clsvalue != NULL)
            {
                pOnuFilterCfg=(onu_filter_t *)(pValue+match*sizeof(onu_filter_t));
                pOnuFilterCfg->type = CLASS_IPPRO;
                pOnuFilterCfg->transfAct =  t_act ;
                pOnuFilterCfg->portId = portId ;
                vosStrCpy(pOnuFilterCfg->value,clsvalue);
                vosStrCpy(pOnuFilterCfg->name,name);
                match++;      
            }
            clsvalue = vosConfigValueGet(QOS_CLASS_SESSION,name,CLASSKEY_DSCP,NULL);
            if (clsvalue != NULL)
            {
                pOnuFilterCfg=(onu_filter_t *)(pValue+match*sizeof(onu_filter_t));
                pOnuFilterCfg->type =CLASS_IPDSCP;
                pOnuFilterCfg->transfAct =  t_act ;
                pOnuFilterCfg->portId = portId ;
                vosStrCpy(pOnuFilterCfg->value,clsvalue);
                vosStrCpy(pOnuFilterCfg->name,name);
                match++;
            }
        }
    }

    *entryNum = match;
    return NO_ERROR;

}

OPL_STATUS odmClsFilterAdd(void)
{
    INT32 index;
    OPL_STATUS retVal = OPL_OK;
    CLS_CONFIG_INFO_t stClassCfg;

    if (odmDataFilterRuleId != 0xFFFF)
    {
        OP_DEBUG(DEBUG_LEVEL_INFO, "odmClsFilterAdd, already exist.");
        return OPL_OK;
    }
    
    OPL_MEMSET((char *)&stClassCfg, 0, sizeof(stClassCfg));
    
    /* Valid */
    stClassCfg.valid = 1;

    /* Action */
    stClassCfg.t_act = DROP_PKTS;

    stClassCfg.bingress=0x01;
    stClassCfg.ingressmask=0x01;

    retVal = dalVoipClsRuleCtcAdd(&stClassCfg, &odmDataFilterRuleId);
    if (retVal != OPL_OK)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING, "%s[%d]: dalVoipClsRuleCtcAdd Fail, retVal=%d.", __FUNCTION__, __LINE__, retVal);
        return retVal;
    }            
    
    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmClsFilterAdd succeed, odmDataFilterRuleId=0x%x.\r\n", odmDataFilterRuleId);
    
    return OPL_OK;
}

OPL_STATUS odmClsFilterDel(void)
{
    INT32 index;
    OPL_STATUS retVal = OPL_OK;

    if (odmDataFilterRuleId == 0xFFFF)
    {
        OP_DEBUG(DEBUG_LEVEL_INFO, "odmClsFilterDel, not exist.");
        return OPL_OK;
    }

    retVal = dalVoipClsDelEntry(odmDataFilterRuleId);
    if (retVal != OPL_OK)
    {
        OP_DEBUG(DEBUG_LEVEL_WARNING, "%s[%d]: dalVoipClsDelEntry Fail, retVal=%d.", __FUNCTION__, __LINE__, retVal);
        return retVal;
    }

    OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "odmClsFilterDel succeed, odmDataFilterRuleId=0x%x.\r\n", odmDataFilterRuleId);

    odmDataFilterRuleId = 0xFFFF;
  
    return OPL_OK;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif /*__cplusplus*/
#endif /*__cplusplus*/

