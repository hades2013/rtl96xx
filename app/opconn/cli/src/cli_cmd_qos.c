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
* FILENAME:  cli_cmd_qos.c
*
* DESCRIPTION: 
*	
*
* Date Created: Aug 19, 2008
*
* Authors(optional): 
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_cmd_qos.c#1 $
* $Log:$
*
*
**************************************************************************/

#include "cli.h"
#include "cli_cmd_list.h"
#include "qos.h"
#include "odm_qos.h"

STATUS cliCmdQoSCreateClass(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    if (NULL == pstPt->p)
    {
        return ERROR;
    }

    ret = odmCreateClass(pstPt->p);
    switch (ret)
    {
        case QOS_OK:
            return OK;
        case QOS_CLS_EXIST_CHECK_ERROR:
            vosPrintf(pstEnv->nWriteFd, "%%error:Class name is existed!\r\n");
            break;
        default :
            break;
    
    }
    return ERROR;
}
STATUS cliCmdShowRunQos(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	int flag=0;
    if (NULL == pstEnv)
    {
        return ERROR;
    }
	if(pstPt[0].i)
		flag=1;
	odmQosConfigDump(pstEnv->nWriteFd,0,flag);	
}
STATUS cliCmdQoSDeleteClass(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    if (NULL == pstPt->p)
    {
        return ERROR;
    }
    ret = odmDestroyClass(pstPt->p);
    switch (ret)
    {
        case QOS_OK:
            return OK;
        case QOS_CLS_EXIST_CHECK_ERROR:
            vosPrintf(pstEnv->nWriteFd, "%%error:Class name is not existed!\r\n");
            break;
        default :
            break;
    
    }
    vosPrintf(pstEnv->nWriteFd, "%%error:Can not delete class %s!\r\n",pstPt->p);
    return ERROR;
}

STATUS cliCmdQoSDeleteRulePre(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    if (pstPt->u <= 0 || pstPt->u > 8 )
    {
        return ERROR;
    }
    ret = OdmAclListDel(0,pstPt->u);
    switch (ret)
    {
        case QOS_OK:
            return OK;
        case QOS_GENERAL_ERROR:
            vosPrintf(pstEnv->nWriteFd, "%%error:rule precedence is not existed!\r\n");
            break;
        default :
            break;
    
    }
    vosPrintf(pstEnv->nWriteFd, "%%error:Can not delete precedence %d!\r\n",pstPt->i);
    return ERROR;
}



STATUS cliCmdQoSShowClass(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    
    if (NULL == pstEnv)
    {
        return ERROR;
    }
  
    if (pstPt == NULL)
    {
        ret = odmShowClass(pstEnv->para,pstEnv->nWriteFd); 
    }
    else
    {
        if ((NULL == pstPt->p) ||(NULL == pstPt))
        {
            return ERROR;
        }
        cliShowStart(pstEnv);
        ret = odmShowClass(pstPt->p,pstEnv->nWriteFd);
        cliShowEnd(pstEnv);
    }
    switch (ret)
    {
        case QOS_OK:
            return OK;
        case QOS_CLS_EXIST_CHECK_ERROR:
            vosPrintf(pstEnv->nWriteFd, "%%error:Class name is not existed!\r\n");
            break;
        default :
            break;
    
    }
    return ERROR;
}
STATUS cliCmdQoSShowClassList(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    
    if (NULL == pstEnv)
    {
        return ERROR;
    }
  
    odmShowClassList(pstEnv->nWriteFd,0);
    return NO_ERROR;
}
STATUS cliCmdQoSShowClassAll(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    
    if (NULL == pstEnv)
    {
        return ERROR;
    }
  
    odmShowClassList(pstEnv->nWriteFd,1);
    return NO_ERROR;
}
STATUS cliCmdQoSClassMode(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    if (NULL == pstPt->p)
    {
        return ERROR;
    }

    /* you must check if the pstEnv->para is freed */
    if (NULL != pstEnv->para)
    {
        vosFree(pstEnv->para);
        pstEnv->para = NULL;
    }
    ret = odmCheckClassExist(pstPt->p);
    if (ret != OK) 
    {
        vosPrintf(pstEnv->nWriteFd, "%%error:Class name is not exist!\r\n");
        return ERROR;
    }
    pstEnv->para = (void *)vosStrDup(pstPt->p);
    pstEnv->pmode = CLI_MODE_CONFIG_QOS_CLASS;    
    return OK;
}
STATUS cliCmdQoSAddClassMac(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    
    UINT8 mac[6]   = {0,0,0,0,0,0};
    
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    if (utilStrToMacAddr(pstPt[1].p,mac) != OK)
    {
        vosPrintf(pstEnv->nWriteFd, "%%Invalid input detected(xx:xx:xx:xx:xx)\r\n");
        return ERROR;
    }
    if (1 == pstPt[0].i)
    {
        ret = odmCfgClassDestMac(pstEnv->para,pstPt[1].p,CLASS_ADD);
    }
    else if (2 == pstPt[0].i)
    {
        ret = odmCfgClassSrcMac(pstEnv->para,pstPt[1].p,CLASS_ADD);
    }
    else
    {
        return ERROR;
    }
    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
    return OK;
}
STATUS cliCmdQoSAddClassIp(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    char str_ip[20] = "";
    UINT32 ipaddr;

    int ret;
    
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    ipaddr = pstPt[1].u;
    cliIpToStr(ipaddr,str_ip);
    if (!cliIsValidIpAddress (str_ip) )
    {
        vosPrintf(pstEnv->nWriteFd,"%%Invalid IP address \n");
        return ERROR;
    }
    if (1 == pstPt[0].i)
    {
        ret = odmCfgClassDestIp(pstEnv->para,str_ip,CLASS_ADD);
    }
    else if (2 == pstPt[0].i)
    {
        ret = odmCfgClassSrcIp(pstEnv->para,str_ip,CLASS_ADD);
    }
    else
    {
        return ERROR;
    }

    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
    return OK;
}
STATUS cliCmdQoSAddClassCos(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT16 cos;
    int ret;
    
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    /* Comment out following check,for match userpriority command member of pstPt->p is not a pointer but an integer*/
	/*
    if (NULL == pstPt->p)
    {
        return ERROR;
    }
    */
    cos = pstPt->i;
    /*
    cos = strtoul(pstPt->p,NULL,0);
    if (cos > 7) 
    {
        vosPrintf(pstEnv->nWriteFd,"%%Input error value(0-7)\r\n");
        return ERROR;
    }
    */
    ret = odmCfgClassFCos(pstEnv->para,cos,CLASS_ADD);
    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
    return OK;
}
STATUS cliCmdQoSAddClassVlan(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT16 vlan;
    int ret;
    
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    vlan = pstPt->u;
    /*
    if (NULL == pstPt->p)
    {
        return ERROR;
    }
    
    vlan = strtoul(pstPt->p,NULL,0);
    if (vlan > 0xfff) 
    {
        vosPrintf(pstEnv->nWriteFd,"%%Input error value(0-4095)\r\n");
        return ERROR;
    }
    */
    ret = odmCfgClassFVid(pstEnv->para,vlan,CLASS_ADD);
    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
    return OK;
}
STATUS cliCmdQoSAddClassEthType(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int    ret;
    UINT16 typelen;
    
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    /*
    if (NULL == pstPt->p)
    {
        return ERROR;
    }
    
    if (strtoul(pstPt->u,NULL,0) > 0xffff) 
    {
        vosPrintf(pstEnv->nWriteFd,"%%Input error value(0-0xffff)\r\n");
        return ERROR;
    }
    */
    typelen = pstPt->u;
    ret = odmCfgClassTypeLenght(pstEnv->para,typelen,CLASS_ADD);
    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
    return OK;
}
STATUS cliCmdQoSAddClassIpType(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int   ret;
    UINT8 iptype;
    
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
     /*
    if (NULL == pstPt->p)
    {
        return ERROR;
    }
  
    if (strtoul(pstPt->p,NULL,0) > 127) 
    {
        vosPrintf(pstEnv->nWriteFd,"%%Input error value(0-127)\r\n");
        return ERROR;
    }
    */
    iptype = pstPt->u;
    ret = odmCfgClassIpType(pstEnv->para,iptype,CLASS_ADD);
    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
    return OK;
}
STATUS cliCmdQoSAddClassDscp(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int   ret;
    UINT8 ipdscp;
    
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
     /*
    if (NULL == pstPt->p)
    {
        return ERROR;
    }
   
    if (strtoul(pstPt->p,NULL,0) > 63) 
    {
        vosPrintf(pstEnv->nWriteFd,"%%Input error value(0-63)\r\n");
        return ERROR;
    }
    */
    

    odmCfgClassIpTos(pstEnv->para,ipdscp,CLASS_MOV);
    ipdscp = pstPt->i;
    ret = odmCfgClassIpDscp(pstEnv->para,ipdscp,CLASS_ADD);
    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
    return OK;
}
STATUS cliCmdQoSAddClassTos(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int   ret;
    UINT8 iptos;
    
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
     /*
    if (NULL == pstPt->p)
    {
        return ERROR;
    }
   
    if (strtoul(pstPt->p,NULL,0) > 63) 
    {
        vosPrintf(pstEnv->nWriteFd,"%%Input error value(0-63)\r\n");
        return ERROR;
    }
    */
    odmCfgClassIpDscp(pstEnv->para,iptos,CLASS_MOV);
    iptos = pstPt->i;
    ret = odmCfgClassIpTos(pstEnv->para,iptos,CLASS_ADD);
    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
    return OK;
}
STATUS cliCmdQoSAddClassL4Port(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int    ret;
    UINT16 l4port;
    
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    l4port = pstPt[1].u;
    /*
    if (strtoul(pstPt[1].p,NULL,0) > 65535)
    {
        vosPrintf(pstEnv->nWriteFd,"%%Input error value(0-65535)\r\n");
        return ERROR;
    }
    */
    if (1 == pstPt[0].i)
    {
        ret = odmCfgClassDestL4Port(pstEnv->para,l4port,CLASS_ADD);
    }
    else if (2 == pstPt[0].i)
    {
        ret = odmCfgClassSrcL4Port(pstEnv->para,l4port,CLASS_ADD);
    }
    else
    {
        return ERROR;
    }
    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
    return OK;
}
STATUS cliCmdQoSAddClassRemarkQueue(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int   ret;
    UINT8 queue;
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    queue  = pstPt[0].u;

    ret = odmCfgClassQueue(pstEnv->para,queue,CLASS_ADD);

    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
    return OK;
}

STATUS cliCmdQoSAddClassRemarkEtherPri(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int   ret;
    UINT8 queue;
    UINT8 ethpri;
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    ethpri = pstPt[0].u;

    ret = odmCfgClassEthPri(pstEnv->para,ethpri,CLASS_ADD);

    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
    return OK;
}

	
STATUS cliCmdQoSAddClassRemarkRulePri(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    UINT8 queue;
    UINT8 ethpri;
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    ethpri = pstPt[0].u;

    ret = odmCfgClassMaskPri(pstEnv->para,ethpri,CLASS_ADD);

    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
    return OK;
}

STATUS cliCmdQoSAddClassAct(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret = ERROR;
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    if (1 == pstPt[0].u)
    {
        ret = odmCfgClassTranfAct(pstEnv->para,PASS,CLASS_ADD);
    }
    else if (2 == pstPt[0].u)
    {
        ret = odmCfgClassTranfAct(pstEnv->para,DROP,CLASS_ADD);
    }
    else if (3 == pstPt[0].u)
    {
        ret = odmCfgClassTranfAct(pstEnv->para,COPYTOCPU,CLASS_ADD);
    }
    else if (4 == pstPt[0].u)
    {
        ret = odmCfgClassTranfAct(pstEnv->para,SENDTOCPU,CLASS_ADD);
    }
	else if (5 == pstPt[0].u)
    {
			ret=odmCfgClassRmkVid(pstEnv->para,CHANGEVID,pstPt[1].u,CLASS_ADD);  		
	}
    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
    return OK;
}
STATUS cliCmdQoSDelClassAct(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    UINT8 tmp=0;
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    if (1 == pstPt[0].u)
    {
        ret = odmCfgClassQueue(pstEnv->para,tmp,CLASS_MOV);
    }
    else if (2 == pstPt[0].u)
    {
        ret = odmCfgClassEthPri(pstEnv->para,tmp,CLASS_MOV);
    }
    else if (3 == pstPt[0].u)
    {
        ret = odmCfgClassTranfAct(pstEnv->para,tmp,CLASS_MOV);
    }
	else if (4 == pstPt[0].u)
    {
			ret=odmCfgClassRmkVid(pstEnv->para,CHANGEVID,0,CLASS_MOV);     
	}
    else return ERROR;

    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
	else if (NO_ERROR != ret)
        vosPrintf(pstEnv->nWriteFd, "%%General Error,ret id %d\r\n",ret);
    return OK;
}
STATUS cliCmdQoSDelClassPra(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    char tmp;
    int  ret;
    
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    if (1 == pstPt[0].u)
    {
        ret = odmCfgClassDestMac(pstEnv->para,&tmp,CLASS_MOV);
    }
    else if (2 == pstPt[0].u)
    {
        ret = odmCfgClassSrcMac(pstEnv->para,&tmp,CLASS_MOV);
    }
    else if (3 == pstPt[0].u)
    {
        ret = odmCfgClassDestL4Port(pstEnv->para,tmp,CLASS_MOV);
    }
    else if (4 == pstPt[0].u)
    {
        ret = odmCfgClassSrcL4Port(pstEnv->para,tmp,CLASS_MOV);
    }
    else if (5 == pstPt[0].u)
    {
        ret = odmCfgClassDestIp(pstEnv->para,&tmp,CLASS_MOV);
    }
    else if (6 == pstPt[0].u)
    {
        ret = odmCfgClassSrcIp(pstEnv->para,&tmp,CLASS_MOV);
    }
    else if (7 == pstPt[0].u)
    {
        ret = odmCfgClassFCos(pstEnv->para,tmp,CLASS_MOV);
    }
    else if (8 == pstPt[0].u)
    {
        ret = odmCfgClassFVid(pstEnv->para,tmp,CLASS_MOV);
    }
    else if (9 == pstPt[0].u)
    {
        ret = odmCfgClassTypeLenght(pstEnv->para,tmp,CLASS_MOV);
    }
    else if (10 == pstPt[0].u)
    {
        ret = odmCfgClassIpType(pstEnv->para,tmp,CLASS_MOV);
    }
    else if (11 == pstPt[0].u)
    {
        ret = odmCfgClassIpTos(pstEnv->para,tmp,CLASS_MOV);
    }
    else if (12 == pstPt[0].u)
    {
        ret = odmCfgClassIpDscp(pstEnv->para,tmp,CLASS_MOV);
    }
    else
    {
        return ERROR;
    }
    if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial config again!\r\n");
    else if (NO_ERROR != ret)
        vosPrintf(pstEnv->nWriteFd, "%%Parameter(%s)is not existed\r\n",pstEnv->para);
    return OK;
}
STATUS cliCmdQoSClassApply(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    
    if (NULL == pstEnv)
    {
        return ERROR;
    }
    if (NULL == pstEnv->para)
    {
        return ERROR;
    }
    if (NULL == pstPt)
    {
        return ERROR;
    }

    if (0 == pstPt[0].u)
    {
		ret = odmInstallClassEntry(pstEnv->para, BOTH_STREAM_, NULL);
    }
    else {
        ret = odmInstallClassEntry(pstEnv->para, (pstPt[0].u - 1), NULL);
    }
    
    if (NO_ERROR == ret)
    {
        vosPrintf(pstEnv->nWriteFd,"%%successfully!\r\n");
    }
    else if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has been apply,denial apply again!\r\n");
    else 
    {
        vosPrintf(pstEnv->nWriteFd,"%%failed %d!\r\n",ret);
    }
    //ctcClassTestAdd();
    return OK;
}
STATUS cliCmdQoSClassRemove(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    
    if (NULL == pstEnv)
    {
        return ERROR;
    }
    if (NULL == pstEnv->para)
    {
        return ERROR;
    }

    ret = odmRemoveClassEntry(pstEnv->para, 1);
    if (NO_ERROR == ret)
    {
        vosPrintf(pstEnv->nWriteFd,"%%successfully!\r\n");
    }
    else if (ret ==  QOS_CLS_EXIST_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class name is not exist!\r\n");
    else if (ret ==  QOS_CLS_INSTALL_CHECK_ERROR)
        vosPrintf(pstEnv->nWriteFd, "%%This class has not been apply!\r\n");
    else 
    {
        vosPrintf(pstEnv->nWriteFd,"%%failed !\r\n");
    }
    //ctcClassTestmove();
    return OK;
}
STATUS cliCmdQoSShowClassRule(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
   
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    cliShowStart(pstEnv);
	
    ret = dalClsRuleEntryShow(0,127,(pstPt[0].i -1));
    cliShowEnd(pstEnv);
    
    return OK;
}
STATUS cliCmdQoSShowClassMask(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);
    ret = dalClsMaskEntryShow(0,23,(pstPt[0].i -1));
    
    cliShowEnd(pstEnv);
    
    return OK;
}
STATUS cliCmdQoSShowClassAddr(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    
    ret = dalClsAddrEntryShow(0,127,(pstPt[0].i -1));

    
    return OK;
}
STATUS cliCmdQoSShowCfg(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    
    if (NULL == pstEnv )
    {
        return ERROR;
    }
    cliShowStart(pstEnv);
    ret = dalClsCfgShow(1,128);
    cliShowEnd(pstEnv);
    
    return OK;
}

/* begin added by ltang for debug */
STATUS cliExClsMatchAdd(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	UINT32 val=0;
	UINT32 paraval=0;
	UINT8   actval_sel=0;
	UINT8   actval=0;
	UINT8   classItemID=0;
	
	val=	pstPt[0].u;
	paraval=pstPt[1].u;
	actval_sel=pstPt[2].u;
	classItemID==pstPt[4].u;
	
	switch(actval_sel)
	{
		case 1:
			actval=(1<<1)&0xf;
			break;
		case 2:
			actval=(1<<3)&0xf;
			break;
		case 3:
			actval=(1)&0xf;
			break;
		default:
			vosPrintf(pstEnv->nWriteFd, "have a input error . \r\n");
			break;
	}
	
	vosPrintf(pstEnv->nWriteFd, "first para is %d, second para is %d,fouth para is %d  \r\n",val,paraval,classItemID);
	switch(val)
	{
		case 1:
			//eport
                      if(paraval<3)
                      {
				dalClsEgressPortCfg(89,2,paraval,0xff,OPL_ENABLE);
				dalClsTranActCfg( 89,2,actval); //drop the packet
			  }
		   	  else
		   	  {
				vosPrintf(pstEnv->nWriteFd, "input para is larger than 3  \r\n");			
			  }
			break;
		case 2:
			//iport
                      if(paraval<3)
                      {
				dalClsIngressPortCfg(90,2,paraval,0xff,OPL_ENABLE);
				dalClsTranActCfg( 90,2,actval); //drop the packet
			  }
		   	  else
		   	  {
				vosPrintf(pstEnv->nWriteFd, "input para is larger than 3  \r\n");			
			  }
			break;	
		case 3:
			//insertvid
			//classitem=0
			if(classItemID!=0)
			{
				//vosPrintf(pstEnv->nWriteFd, "insertvid: classitem is %d,rmkvid is %d \r\n",classItemID,paraval);
				dalClsRmkVidCfg( classItemID,2,paraval);
				dalClsRmkActCfg(classItemID,2,0x8);
			}
			else
			{
				dalClsRmkVidCfg( 91,2,paraval);
				dalClsRmkActCfg(91,2,0x8);			
			}		
			
			break;
		case 4:
			//tpid				
				dalClsFirstTpidCfg(92,2,paraval,0xffff,OPL_ENABLE);			
				dalClsTranActCfg( 92,2,actval); //drop the packet		
			break;
		case 5:
			//ipversion   /*0xff 应该为0xF*/
			dalClsIpVersionCfg(93, 2, paraval, 0xff, OPL_ENABLE);
			dalClsTranActCfg( 93,2,actval); //drop the packet
			break;
		default:
			vosPrintf(pstEnv->nWriteFd, "have a error . \r\n");
			
	}	
	return 0;
}

/*add for delete extend classify match*/
STATUS cliExClsMatchDel(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	UINT32 val=0;
	UINT32 classitem=0;	
	OPL_STATUS retval=0;

	
	val=	pstPt[0].u;
	classitem=pstPt[2].u;	
	switch(val)
	{
		case 1:
			//eport                      
			  retval=dalExClsDelEntry(89-1);
			  if(OPL_OK!=retval)
			  {
				vosPrintf(pstEnv->nWriteFd, "dalExClsDelEntry Del failed. \r\n");			
			  }
			break;
		case 2:
			//iport
                      retval=dalExClsDelEntry(90-1);
			  if(OPL_OK!=retval)
			  {
				vosPrintf(pstEnv->nWriteFd, "dalExClsDelEntry Del failed. \r\n");			
			  }			
			  break;	
		case 3:
			//insertvid
			retval=dalExClsDelEntry(91-1);
			  if(OPL_OK!=retval)
			  {
				vosPrintf(pstEnv->nWriteFd, "dalExClsDelEntry Del failed. \r\n");			
			  }
			break;
		case 4:
			//tpid				
			retval=dalExClsDelEntry(92-1);
			  if(OPL_OK!=retval)
			  {
				vosPrintf(pstEnv->nWriteFd, "dalExClsDelEntry Del failed. \r\n");			
			  }	
			break;
		case 5:
			//ipversion   /*0xff 应该为0xF*/
			retval=dalExClsDelEntry(93-1);
			  if(OPL_OK!=retval)
			  {
				vosPrintf(pstEnv->nWriteFd, "dalExClsDelEntry Del failed. \r\n");			
			  }
			break;
		default:
			vosPrintf(pstEnv->nWriteFd, "have a error . \r\n");
			
	}
#if 0
	if(0!=classitem)
	{
		  retval=dalExClsDelEntry(classitem-1);
		  if(OPL_OK!=retval)
		  {
			vosPrintf(pstEnv->nWriteFd, "dalExClsDelEntry Del classitem[%d] failed. \r\n",classitem);			
		  }					  
	}
	vosPrintf(pstEnv->nWriteFd, "----ltang for debug end ----  \r\n");
#endif
	return 0;
}

/* end added by ltang for  debug */

STATUS cliCmdQoSShowPre(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    int flag=1;
	
    if (NULL == pstEnv )
    {
        return ERROR;
    }
	if (0 != pstPt[0].i)
		flag=0;
    cliShowStart(pstEnv);
    odmOamClassShowCls(pstEnv->nWriteFd);
    cliShowEnd(pstEnv);
    
    return OK;
}
STATUS cliCmdQoSDelCfg(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    
    if ((NULL == pstEnv ) ||(NULL == pstPt))
    {
        return ERROR;
    }
    
    ret = dalClsRuleDel(pstPt->u);
    
    return OK;
}
STATUS cliCmdQoSDelRule(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    
    if ((NULL == pstEnv ) ||(NULL == pstPt))
    {
        return ERROR;
    }
  
    //ret = clsRuleDel(pstPt->u);
    
    return OK;
}
STATUS cliCmdQoSRmkVid(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret;
    
    if ((NULL == pstEnv ) ||(NULL == pstPt))
    {
        return ERROR;
    }
  
    ret = dalClsRmkVidActSet(pstPt[0].u,pstPt[2].u,pstPt[1].u);
    
    return OK;
}

