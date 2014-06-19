/*
=============================================================================
     File Name: hal_cls.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
wangliang 			2008/8/19		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /*__cplusplus*/
#endif /*__cplusplus*/
#include <hal.h>
#include <hal_cls.h>
#include <defs.h>


#define HAL_CLS_PRINTF(x)  printf x
#define HAL_CLS_TRACE() HAL_CLS_PRINTF(("%s,%s,%d\n",__FILE__,__FUNCTION__,__LINE__))
#define HAL_CLS_EXIT(s) {ret=s;goto exit;}
/*******************************************************************************
*
* halClsDestMacCFG:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsDestMacCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_MAC_CFG_PAR_t *tmp=NULL;

    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }

    tmp = (HAL_CLS_MAC_CFG_PAR_t *)p1;
    ret = dalClsDestMacAddrCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->mac,tmp->macmask,tmp->enable);
 exit:
    tmp->ret = ret;
    return ;
}
/*******************************************************************************
*
* halClsSrcMacCFG:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsSrcMacCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_MAC_CFG_PAR_t *tmp=NULL;

    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }

    tmp = (HAL_CLS_MAC_CFG_PAR_t *)p1;
    ret = dalClsSrcMacAddrCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->mac,tmp->macmask,tmp->enable);
exit:
    tmp->ret = ret;
    return ;
}
/*******************************************************************************
*
* halClsFirstCosCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsFirstCosCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_COS_CFG_PAR_t *tmp=NULL;

    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }

    tmp = (HAL_CLS_COS_CFG_PAR_t *)p1;
    ret = dalClsFirstCosCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->cos,tmp->cosmask,tmp->enable);
exit:
    tmp->ret = ret;
    return ;
}
/*******************************************************************************
*
* halClsFristVidCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsFirstVidCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_VID_CFG_PAR_t *tmp=NULL;

    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }
    tmp = (HAL_CLS_VID_CFG_PAR_t *)p1;
    ret = dalClsFirstVidCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->vid,tmp->vidmask,tmp->enable);
exit:
    tmp->ret = ret;
    return ;
}
/*******************************************************************************
*
* halClsSecondCosCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsSecondCosCfg(void *p1)
{
    OPL_RESULT ret = OPL_ERROR;
   
    return ;
}
/*******************************************************************************
*
* halClsSecondVidCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsSecondVidCfg(void *p1)
{
    OPL_RESULT ret = OPL_ERROR;
    
    return ;
}
/*******************************************************************************
*
* halClsTypeLenCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsTypeLenCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_TYPELEN_CFG_PAR_t *tmp=NULL;
    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }

    tmp = (HAL_CLS_TYPELEN_CFG_PAR_t *)p1;
    ret = dalClsTypeLenCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->typelen,tmp->typelenmask,tmp->enable);
exit:
    tmp->ret = ret;
    return ;
}
/*******************************************************************************
*
* halClsDestIpCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsDestIpCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_IP_CFG_PAR_t *tmp=NULL;

    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }

    tmp = (HAL_CLS_IP_CFG_PAR_t *)p1;
    ret = dalClsDestIpCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->ip,tmp->ipmask,tmp->enable);
exit:
    tmp->ret = ret;
    return ;
    
}
/*******************************************************************************
*
* halClsSrcIpCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsSrcIpCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_IP_CFG_PAR_t *tmp=NULL;

    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }

    tmp = (HAL_CLS_IP_CFG_PAR_t *)p1;
    ret = dalClsSrcIpCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->ip,tmp->ipmask,tmp->enable);
exit:
    tmp->ret = ret;
    return ;
    
}
/*******************************************************************************
*
* halClsIpTypeCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsIpTypeCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_IPTYPE_CFG_PAR_t *tmp=NULL;

    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }

    tmp = (HAL_CLS_IPTYPE_CFG_PAR_t *)p1;
    ret = dalClsIpTypeCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->iptype,tmp->iptypemask,tmp->enable);
exit:
    tmp->ret = ret;
    return ;   
}
/*******************************************************************************
*
* halClsIpTosCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsIpTosCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_IPTOS_CFG_PAR_t *tmp=NULL;

    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }

    tmp = (HAL_CLS_IPTOS_CFG_PAR_t *)p1;
    ret = dalClsIpTosCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->iptos,tmp->iptosmask,tmp->enable);
exit:
    tmp->ret = ret;
    return ;    
}
/*******************************************************************************
*
* halClsL4DestPortCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsL4DestPortCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_L4PORT_CFG_PAR_t *tmp=NULL;

    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }

    tmp = (HAL_CLS_L4PORT_CFG_PAR_t *)p1;
    ret = dalClsL4DestPortCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->port,tmp->portmask,tmp->enable);
    
exit:
    tmp->ret = ret;
    return ; 
}
/*******************************************************************************
*
* halClsL4SrcPortCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsL4SrcPortCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_L4PORT_CFG_PAR_t *tmp=NULL;

    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }

    tmp = (HAL_CLS_L4PORT_CFG_PAR_t *)p1;
    ret = dalClsL4SrcPortCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->port,tmp->portmask,tmp->enable);
    
exit:
    tmp->ret = ret;
    return ;   
}

/*******************************************************************************
*
* halClsIngressPortCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsIngressPortCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_PORT_CFG_PAR_t *tmp=NULL;

    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }

    tmp = (HAL_CLS_PORT_CFG_PAR_t *)p1;
    ret = dalClsIngressPortCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->port,tmp->portmask,tmp->enable);
    
exit:
    tmp->ret = ret;
    return ;   
}

/*******************************************************************************
*
* halClsEgressPortCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsEgressPortCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_PORT_CFG_PAR_t *tmp=NULL;

    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }

    tmp = (HAL_CLS_PORT_CFG_PAR_t *)p1;
    ret = dalClsEgressPortCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->port,tmp->portmask,tmp->enable);
    
exit:
    tmp->ret = ret;
    return ;    
}
/*******************************************************************************
*
* halClsCosRemarkCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsCosRemarkCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_COS_REMARK_CFG_PAR_t *tmp=NULL;

    UINT8 rmkActVal;

    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }

    tmp = (HAL_CLS_COS_REMARK_CFG_PAR_t *)p1;
    dalClsRmkActGet(tmp->classItemID,CLASSIFY_EII_IP,&rmkActVal);
    if (OPL_TRUE == tmp->enable) 
    {
        rmkActVal |= CHANGE_COS_FIELD;
        ret =  dalClsRmkActCfg(tmp->classItemID,CLASSIFY_EII_IP,rmkActVal);
        ret += dalClsRmkCosCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->cosremark);
    }
    else if (OPL_FALSE == tmp->enable)
    {
        rmkActVal &= (~CHANGE_COS_FIELD);
        ret = dalClsRmkActCfg(tmp->classItemID,CLASSIFY_EII_IP,rmkActVal);
    }
    else ret = OPL_ERROR;    
    
exit:
    tmp->ret = ret;
    return ;   
}
/*******************************************************************************
*
* halClsVidRemarkCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsVidRemarkCfg(void *p1)
{
    OPL_RESULT ret = 0;
    HAL_CLS_VID_REMARK_CFG_PAR_t *tmp=NULL;
    UINT8 rmkActVal;
    
    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }

    tmp = (HAL_CLS_VID_REMARK_CFG_PAR_t *)p1;
    dalClsRmkActGet(tmp->classItemID,CLASSIFY_EII_IP,&rmkActVal);
    if (OPL_TRUE == tmp->enable) 
    {
        switch (tmp->type)
        {
            case INSERT_TAG_AFTER_SAMAC:
                /*clear vid remark,reserve icos/cos remark bit.*/
                rmkActVal &= 0x3;
                rmkActVal |= INSERT_TAG_AFTER_SA;
                ret =  dalClsRmkActCfg(tmp->classItemID,CLASSIFY_EII_IP,rmkActVal);
                ret += dalClsRmkVidCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->vidremark);
                break;
            case REMARK_VID_AS_RULE_VID:
                rmkActVal &= 0x3;
                rmkActVal |= CHANGE_VID_AS_RULE_VID;
                ret =  dalClsRmkActCfg(tmp->classItemID,CLASSIFY_EII_IP,rmkActVal);
                ret += dalClsRmkVidCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->vidremark);
                break;
            default :
                break;
        }        
        

    }
    else if (OPL_FALSE == tmp->enable)
    {
        rmkActVal &= 0x3;
        ret = dalClsRmkActCfg(tmp->classItemID,CLASSIFY_EII_IP,rmkActVal);
    }
    else ret = OPL_ERROR;     
exit:
    tmp->ret = ret;
    return ;  
}
/*******************************************************************************
*
* halClsTranActCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsTranActCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_TRANACT_CFG_PAR_t *tmp=NULL;
    UINT8 traActVal;
    
    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }
    tmp = (HAL_CLS_TRANACT_CFG_PAR_t *)p1;
    if (OPL_TRUE == tmp->enable) 
    {
        ret = dalClsTranActCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->action);
    }
    else if (OPL_FALSE == tmp->enable)
    {
        ret = dalClsTranActCfg(tmp->classItemID,CLASSIFY_EII_IP,0);
    }
    else ret = OPL_ERROR; 
    
exit:
    tmp->ret = ret;
    return ;     
}
/*******************************************************************************
*
* halClsICosMapCfg:	 
*
* DESCRIPTION:
* 	flow internel cos(queue) map.the icos value corresponding as the queue vlaue.
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsICosMapCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_ICOS_MAP_CFG_PAR_t *tmp=NULL;
    UINT8 traActVal;
    
    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }
    tmp = (HAL_CLS_ICOS_MAP_CFG_PAR_t *)p1;
    ret = dalClsRmkIcosCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->queue);
    
exit:
    tmp->ret = ret;
    return ;     
}
/*******************************************************************************
*
* halClsMaskPriCfg:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsMaskPriCfg(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_MASK_PRIORITY_CFG_PAR_t *tmp=NULL;
    UINT8 traActVal;
    
    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }
    tmp = (HAL_CLS_MASK_PRIORITY_CFG_PAR_t *)p1;
    ret = dalClsMaskPrioCfg(tmp->classItemID,CLASSIFY_EII_IP,tmp->pri);
    
exit:
    tmp->ret = ret;
    return ;    
}
/*******************************************************************************
*
* halClsDelete:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsDelete(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_DELETE_PAR_t *tmp=NULL;
    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        HAL_CLS_EXIT(OPL_ERR_NULL_POINTER);
    }
    tmp = (HAL_CLS_DELETE_PAR_t *)p1;
    ret = dalClsRuleDel(tmp->classItemID);

exit:
    tmp->ret = ret;
    return ;   
}
/*******************************************************************************
*
* halClsEntryShow:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
         p1:pointer to parameter struct.
*
* OUTPUTS: N/A.

* RETURN:

*
* SEE ALSO: 
*/
void halClsEntryShow(void *p1)
{
    OPL_RESULT ret;
    HAL_CLS_CLS_SHOW_PAR_t *tmp;
    if (NULL == p1)
    {
        HAL_CLS_TRACE();
        return;
    }
#if 0 /*wfxu will be done*/
    tmp = (HAL_CLS_CLS_SHOW_PAR_t *)p1;
    switch (tmp->type)
    {
        case MASK_ENTRY:
            dalClsMaskEntryShow(tmp->startid,tmp->endid,1);
            break;
        case ADDR_ENTRY:
            dalClsAddrEntryShow(tmp->startid,tmp->endid,1);
            break;
        case RULE_ENTRY:
            dalClsRuleEntryShow(tmp->startid,tmp->endid,1);
            break;
        case CFG_ENTRY:
            dalClsCfgShow(1,tmp->startid,tmp->endid);
            break;
        default:
            break;
            
    }
#endif
exit:
    tmp->ret = OPL_OK;
    return ;   
}
#ifdef OPL_DMA0_ETH
UINT8 wanMacClsRule[6];
void halMirrorClsInit(void)
{
  //#ifdef OPL_NO_DMA1_ETH
  UINT8 acMac0[6] = {0};
  UINT8 acMac1[6] = {0};
  UINT8 acMac2[6] = {0};
  //#endif
  UINT8 acMac3[6] = {0};
  UINT8 acMac4[6] = {0};
  UINT8 acMac5[6] = {0};
  UINT8 acMaccmp[6] = {0};
  UINT8 i;
  UINT16 rule=0;
  int ret;
  CLS_CONFIG_INFO_t ClassItemInfoSin;
  UINT8  fullmacMask[MAC_LENGTH] = {
	  0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF
	};

  vosMemSet((char *)&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
  ClassItemInfoSin.dstMacFlag = 1;
  ClassItemInfoSin.dstMac.lowRange[0] = 0xFF;
  ClassItemInfoSin.dstMac.lowRange[1] = 0xFF;
  ClassItemInfoSin.dstMac.lowRange[2] = 0xFF;
  ClassItemInfoSin.dstMac.lowRange[3] = 0xFF;
  ClassItemInfoSin.dstMac.lowRange[4] = 0xFF;
  ClassItemInfoSin.dstMac.lowRange[5] = 0xFF;
  ClassItemInfoSin.lenOrTypeFlag = 1;
  ClassItemInfoSin.lenOrType.lowRange = 0x0806;
  vosMemCpy(&(ClassItemInfoSin.dstMac.highRange),fullmacMask,MAC_LENGTH);
  ClassItemInfoSin.lenOrType.highRange = 0xffff;
  #ifndef OPL_NO_DMA1_ETH
  ClassItemInfoSin.bingress=0x01;
	ClassItemInfoSin.ingressmask=0x01;
	#endif
  ClassItemInfoSin.t_act = (DONOT_DROP|COPY_PKTS_TO_CPU);
  dalVoipClsRuleCtcAdd(&ClassItemInfoSin,&rule);
  
  /* MATCH DHCP OFFER */
  vosMemSet((char *)&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
  ClassItemInfoSin.lenOrTypeFlag = 1;
  ClassItemInfoSin.lenOrType.lowRange = 0x800;
  ClassItemInfoSin.lenOrType.highRange = 0xffff;
  ClassItemInfoSin.ipTypeFlag = 1;
  ClassItemInfoSin.ipType.lowRange = 0x11;
  ClassItemInfoSin.ipType.highRange = 0xff;
  ClassItemInfoSin.srcL4PortFlag = 1;
  ClassItemInfoSin.srcL4Port.lowRange = 67;
  ClassItemInfoSin.srcL4Port.highRange = 0xffff;
  ClassItemInfoSin.dstL4PortFlag = 1;
  ClassItemInfoSin.dstL4Port.lowRange = 68;
  ClassItemInfoSin.dstL4Port.highRange = 0xffff;
  ClassItemInfoSin.bingress=0x01;
  ClassItemInfoSin.ingressmask=0x01;
  ClassItemInfoSin.t_act = (DONOT_DROP|COPY_PKTS_TO_CPU);
  dalVoipClsRuleCtcAdd(&ClassItemInfoSin,&rule);
  
  #ifdef OPL_NO_DMA1_ETH

#if defined(INTERFACE_THREE_WAN_THREE_LAN)
    vosHWAddrGet("eth0", acMac0);
	vosHWAddrGet("eth1", acMac1);
	vosHWAddrGet("eth2", acMac2);
#elif defined(INTERFACE_TWO_WAN_TWO_LAN)
    vosHWAddrGet("eth0", acMac0);
	vosHWAddrGet("eth1", acMac1);
#elif defined(INTERFACE_ONE_WAN_ONE_LAN)
    vosHWAddrGet("eth0", acMac0);
#endif

  #endif

#if defined(INTERFACE_THREE_WAN_THREE_LAN)
    vosHWAddrGet("eth3", acMac3);
	vosHWAddrGet("eth4", acMac4);
	vosHWAddrGet("eth5", acMac5);
#elif defined(INTERFACE_TWO_WAN_TWO_LAN)
    vosHWAddrGet("eth2", acMac2);
	vosHWAddrGet("eth3", acMac3);
#elif defined(INTERFACE_ONE_WAN_ONE_LAN)
    vosHWAddrGet("eth1", acMac1);
#endif  
	
	memset((char *)&wanMacClsRule[0], 0, 6);
	#ifdef OPL_NO_DMA1_ETH
	  for (i=0; i< INTERFACE_MAX*INTERFACE_MODE_MAX; i++)	      
	#else
	  for (i=INTERFACE_MODE_MAX; i< INTERFACE_MAX*INTERFACE_MODE_MAX; i++)
    #endif
	{
	  vosMemSet((char *)&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
	  ClassItemInfoSin.dstMacFlag = 1;
	  
	  //#ifdef OPL_NO_DMA1_ETH
	  if (0 == i)
	  {
        if (memcmp(acMaccmp,acMac0, 6) == 0)
        {
          continue;
        }
		memcpy(ClassItemInfoSin.dstMac.lowRange, acMac0, 6);
	  }
	  else if (1 == i)
	  {
        if (memcmp(acMaccmp,acMac1, 6) == 0)
        {
          continue;
        }
	    memcpy(ClassItemInfoSin.dstMac.lowRange, acMac1, 6);
	  }
	  else if (2 == i)
	  {
        if (memcmp(acMaccmp,acMac2, 6) == 0)
        {
          continue;
        }
	    memcpy(ClassItemInfoSin.dstMac.lowRange, acMac2, 6);
	  }
	  //#endif
	  
	  if (3 == i)
	  {
        if (memcmp(acMaccmp,acMac3, 6) == 0)
        {
          continue;
        }
	    memcpy(ClassItemInfoSin.dstMac.lowRange, acMac3, 6);
	  }
	  else if (4 == i)
	  {
        if (memcmp(acMaccmp,acMac4, 6) == 0)
        {
          continue;
        }
	    memcpy(ClassItemInfoSin.dstMac.lowRange, acMac4, 6);
	  }
	  else if (5 == i)
	  {
        if (memcmp(acMaccmp,acMac5, 6) == 0)
        {
          continue;
        }
	    memcpy(ClassItemInfoSin.dstMac.lowRange, acMac5, 6);
	  }
	  
	  vosMemCpy(&(ClassItemInfoSin.dstMac.highRange),fullmacMask,MAC_LENGTH);
	  #ifndef OPL_NO_DMA1_ETH
	  ClassItemInfoSin.bingress=0x01;
	  ClassItemInfoSin.ingressmask=0x01;
	  #endif
	  ClassItemInfoSin.t_act = (DROP_PKTS|COPY_PKTS_TO_CPU);
    dalVoipClsRuleCtcAdd(&ClassItemInfoSin,&rule);
    wanMacClsRule[i] = rule;
    
    oplRegFieldWrite(0xc0c0, 0, 32, 0);
    oplRegFieldWrite(0xc0c4, 0, 32, 0);
    oplRegFieldWrite(0xc0c8, 0, 32, 0);
    oplRegFieldWrite(0xc0cc, 0, 32, 0);
    oplRegFieldWrite(0xc0f4, 0, 32, 0);

	//printf("configure user mac .......................\n");
	UINT8	userMac[6] = {0x00,0x0E,0x5E,0x00,0x00,0x01};
	
	brgUsrDefineRsvMacHwWrite(0, userMac);
	brgUsrDefineRsvMacCtrlHwWrite(0, 0, 1);
	
	}
  return;
}

void halEocClsInit(void)
{
    CLS_CONFIG_INFO_t ClassItemInfoSin;
    UINT16 rule=0;
   
     /* MATCH EoC packet */
    vosMemSet((char *)&ClassItemInfoSin, 0, sizeof(ClassItemInfoSin));
    ClassItemInfoSin.lenOrTypeFlag = 1;
    ClassItemInfoSin.lenOrType.lowRange = 0x88e1;
    ClassItemInfoSin.lenOrType.highRange = 0xffff;
    ClassItemInfoSin.bingress=0x0;
    ClassItemInfoSin.ingressmask=0x1;
    ClassItemInfoSin.t_act = (DONOT_DROP|COPY_PKTS_TO_CPU);
    dalVoipClsRuleCtcAdd(&ClassItemInfoSin,&rule);

    return;
}
#endif
void halClsInit(void)
{
    OPL_API_UNIT_t clsApiUnit;
    OPL_RESULT ulRet;

    /*register classify hal api*/
    clsApiUnit.apiId   = HAL_API_CLS_DESTMAC_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsDestMacCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_SRCMAC_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsSrcMacCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_FCOS_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsFirstCosCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_FVID_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsFirstVidCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_SCOS_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsSecondCosCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_SVID_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsSecondVidCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_TYPELEN_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsTypeLenCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_DESTIP_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsDestIpCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_SRCIP_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsSrcIpCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_IPTYPE_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsIpTypeCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_IPTOS_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsIpTosCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_L4DESTPORT_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsL4DestPortCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_L4SRCPORT_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsL4SrcPortCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_INGRESSPORT_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsIngressPortCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_EGRESSPORT_CFG;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsEgressPortCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_COS_REMARK;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsCosRemarkCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_VID_REMARK;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsVidRemarkCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_TRANACT;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsTranActCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_ICOS_MAP;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsICosMapCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_MASK_PRIORITY;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsMaskPriCfg;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_DELETE;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsDelete;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_CLS_ENTRYSHOW;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halClsEntryShow;
    ulRet = halDrvApiRegister(&clsApiUnit);

    return ;
}

static UINT16 s_usQinqClfId = CLASSIFY_MAX_ITEM_NUM;

/*******************************************************************************
* brgQinqClfSet
*
* DESCRIPTION:
*  		This function is used to set classifier rule for qinq function that
*       inserts a stag for all upstreams.
*
*	INPUTS:
*			UINT16 usVid
*           UP_DOWN_STREAM_e enDirect
*			
*	OUTPUTS:
*		    void
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS HalQinqClfSet(UINT16 usVid, UP_DOWN_STREAM_e enDirect)
{
    UINT16 usClsId;
    OPL_STATUS ret;
    HAL_CLS_VID_REMARK_CFG_PAR_t stVidInsertCfg;
    HAL_CLS_PORT_CFG_PAR_t stPortCfg;

    if ((4095 <= usVid) ||
        (BOTH_STREAM <= enDirect))
    {
        OPL_DRV_PRINTF(("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__));
        return OPL_ERROR;
    }

    ret = dalClsValidEntryGet(&usClsId);
    if (OPL_OK != ret)
    {
        OPL_DRV_PRINTF(("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__));
        return OPL_ERROR;
    }

    if (CLASSIFY_MAX_ITEM_NUM <= usClsId)
    {
        OPL_DRV_PRINTF(("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__));
        return OPL_ERROR;
    }
    
    stVidInsertCfg.classItemID = usClsId + 1;
    stVidInsertCfg.vidremark   = usVid;
    stVidInsertCfg.type        = INSERT_TAG_AFTER_SAMAC;
    stVidInsertCfg.enable      = OPL_TRUE;

    stPortCfg.classItemID      = usClsId + 1;
    stPortCfg.enable           = OPL_TRUE;
    stPortCfg.portmask         = 1;

    if (UP_STREAM == enDirect)
    {
        stPortCfg.port = 0;
    }
    else if (DOWN_STREAM == enDirect)
    {
        stPortCfg.port = 1;
    }
    
    halClsVidRemarkCfg(&stVidInsertCfg);
    if (OPL_OK != stVidInsertCfg.ret)
    {
        OPL_DRV_PRINTF(("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__));
        return OPL_ERROR;
    }
    
    halClsIngressPortCfg(&stPortCfg);
    if (OPL_OK != stVidInsertCfg.ret)
    {
        OPL_DRV_PRINTF(("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__));
        return OPL_ERROR;
    }

    OPL_DRV_PRINTF(("\nAdd classifier rule %d.\n", usClsId));
    
    s_usQinqClfId = usClsId;
    
    return OPL_OK;
}

/*******************************************************************************
* brgQinqClfReset
*
* DESCRIPTION:
*  		This function is used to reset classifier rule for qinq function that
*       inserts a stag for all upstreams.
*
*	INPUTS:
*			void
*			
*	OUTPUTS:
*		    void
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS HalQinqClfReset(VOID)
{
    UINT16 usClsId;
    HAL_CLS_DELETE_PAR_t stClsDeleteCfg;

    stClsDeleteCfg.classItemID = s_usQinqClfId + 1;
    if (CLASSIFY_MAX_ITEM_NUM <= stClsDeleteCfg.classItemID)
    {
        OPL_DRV_PRINTF(("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__));
        return OPL_OK;
    }
    
    halClsDelete(&stClsDeleteCfg);
    if (OPL_OK != stClsDeleteCfg.ret)
    {
        OPL_DRV_PRINTF(("\nfunc:%s, line:%d.\n",__FUNCTION__,__LINE__));
        return OPL_ERROR;
    }

    OPL_DRV_PRINTF(("\nDelete classifier rule %d.\n", s_usQinqClfId));
    
    s_usQinqClfId = CLASSIFY_MAX_ITEM_NUM;

    return OPL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /*__cplusplus*/
#endif /*__cplusplus*/

