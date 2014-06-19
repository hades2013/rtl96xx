/*****************************************************************************
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include <unistd.h>

DRV_RET_E Drv_IoctlCmd(CHAR *pcDevName, DRV_REQ_S *drv_req )
{
    INT32 fd = 0;
    INT32 ret = 0;    
    CHAR tmpdevname[32];

    if(NULL==pcDevName || NULL==drv_req)
    {
        return DRV_ERR_PARA;
    }
    memset(tmpdevname, 0, sizeof(tmpdevname));
    sprintf(tmpdevname, "/dev/");
    strcat(tmpdevname, pcDevName);

    fd = open( tmpdevname, O_RDWR );
    if( fd <0 )
    {
    	return DRV_ERR_OPEN_DEV;
    }

    ret = ioctl(fd, SIOCDRVPRIVATE, drv_req);

    close(fd);

    if (0 != ret)
    {
        return DRV_IOCTL_ERROR;
    }

    return drv_req->err;
}

#ifdef UT_DRV_TEST
#define Drv_IoctlCmd(_drv, _req)    Drv_IoctlCmd_stub(_drv, _req)
#endif
DRV_RET_E Ioctl_SetSwitchU32Para(DRV_CMD_E cmd,INT32 paraNum,...)
{
    va_list args;
    DRV_REQ_S stReq;
    UINT32 u32Para[IOCTL_SUPPORT_MAX_PARA_NUM]={0};
    INT i = 0;

    if(paraNum > IOCTL_SUPPORT_MAX_PARA_NUM)
    {
        return DRV_PARA_NUM_ERROR;
    }

    va_start(args, paraNum);
    for(i=0; i<paraNum; i++)
    {
        /*lint -esym(628,__builtin_va_arg) */
        u32Para[i] = (UINT32)va_arg(args,UINT32);
    }
	va_end(args);

    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    stReq.cmd = cmd;   
    /*  */
    #if  1
    switch (paraNum)
    {
        case 1:
            stReq.para1_u.uiValue = u32Para[0];
        break;
        case 2:
            stReq.para1_u.uiValue = u32Para[0];
            stReq.para2_u.uiValue = u32Para[1];
            break;
        case 3:
        case DRV_CMD_SET_PORT_LED:
            stReq.para1_u.uiValue = u32Para[0];
            stReq.para2_u.uiValue = u32Para[1];
            stReq.para3_u.uiValue = u32Para[2];   
            break; 
        default:
            return DRV_IOCTL_ERROR;
    }    
    #else
    switch (paraNum)
    {
        case DRV_CMD_SET_PORTENABLE:
        case DRV_CMD_SET_MEDIUM:
        case DRV_CMD_SET_PRIORITY:
        case DRV_CMD_SET_MDIX:
        case DRV_CMD_SET_JUMBO:
        case DRV_CMD_SET_TXRX:
        case DRV_CMD_SET_AGGRMODE:
        case DRV_CMD_SET_AGGR_GLOBAL_CFG_FLAG:
        case DRV_CMD_SET_VLANMODE:
        case DRV_CMD_SET_INTER_LOOPBACK:
        case DRV_CMD_SET_SPDLX:
            stReq.para1_u.uiValue = u32Para[0];
            stReq.para2_u.uiValue = u32Para[1];
            break;
        case DRV_CMD_SET_ABILITY:
        case DRV_CMD_SET_AUTONEG:
        case DRV_CMD_SET_SPEED:
        case DRV_CMD_SET_DUPLEX:
        case DRV_CMD_SET_PAUSE:
        case DRV_CMD_SET_PORT_LED:
        case DRV_CMD_SET_PHYREG:
            stReq.para1_u.uiValue = u32Para[0];
            stReq.para2_u.uiValue = u32Para[1];
            stReq.para3_u.uiValue = u32Para[2];   
            break; 
        default:
            return DRV_IOCTL_ERROR;
    }
    #endif /* #if 0 */
    
    
    return Drv_IoctlCmd(DEV_SWTICH, &stReq);
}


DRV_RET_E Ioctl_GetSwitchU32Para(DRV_CMD_E cmd,UINT uiPort,UINT32 *pResult,UINT32 uiValue)
{   
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;          

    if(pResult == NULL)
    {
        return DRV_ERR_PARA;
    }

    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    stReq.cmd = cmd;
    stReq.p_lport = uiPort;
    stReq.para3_u.uiValue = uiValue;
    
    rev = Drv_IoctlCmd(DEV_SWTICH, &stReq);

    if(DRV_OK == rev)
    {
        switch (cmd)
        {
            case DRV_CMD_GET_PORTMEDIUM:
            case DRV_CMD_GET_CUR_LINK:
        //    case DRV_CMD_GET_LINK_MASK:
            case DRV_CMD_GET_PORTABILITY:
            case DRV_CMD_GET_PORTSPEED:
            case DRV_CMD_GET_PHYREG:
            case DRV_CMD_GET_PORTDUPLEX: 
            case DRV_CMD_GET_PORTMDIX:
            case DRV_CMD_GET_PORTPRIO:
            case DRV_CMD_GET_PORTFLOWCTRL:
            case DRV_CMD_GET_PORTJUMBO:
            case DRV_CMD_GET_PORTFRAMELEN:
            case DRV_CMD_GET_FIBERPORTSPEED:
            /*begin added by liaohongjun 2012/12/10 of EPN104QID0086*/    
            case DRV_CMD_GET_PORTENABLE:  
            /*end added by liaohongjun 2012/12/10 of EPN104QID0086*/    
			case DRV_CMD_GET_AUTONEG:
                *pResult = stReq.para2_u.uiValue;
                break;
            default:
                return DRV_IOCTL_ERROR;
        }
    }    
    return rev;
    
}
DRV_RET_E Ioctl_GetPortLinkMask(logic_pmask_t *pMask)
{
    
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    if(pMask == NULL)
    {
        return DRV_ERR_PARA;
    }
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    stReq.cmd = DRV_CMD_GET_LINK_MASK;   
    
    rev = Drv_IoctlCmd(DEV_SWTICH, &stReq);

    if(DRV_OK == rev)
    {
        memcpy((VOID *)pMask,(CONST VOID *)(&stReq.p_mask),sizeof(logic_pmask_t));
    }
    return rev;
}
DRV_RET_E Ioctl_GetIsolateSet(logic_pmask_t *pMask)
{
    
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    if(pMask == NULL)
    {
        return DRV_ERR_PARA;
    }
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    stReq.cmd = DRV_CMD_GET_ISOLATE_SET;   
    
    rev = Drv_IoctlCmd(DEV_SWTICH, &stReq);

    if(DRV_OK == rev)
    {
        memcpy((VOID *)pMask,(CONST VOID *)(&stReq.p_mask),sizeof(logic_pmask_t));
    }
    return rev;
}

DRV_RET_E Ioctl_AddAclFilter(UINT32 lport, 
                             UINT32 uiRulePrecedence,
                             UINT32 uiAclRuleType,
                             UINT32 uiAclRuleDir,
                             VOID *pRuleValue)
{
    
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    if(pRuleValue == NULL)
    {
        return DRV_ERR_PARA;
    }
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    
    stReq.cmd = DRV_CMD_ADD_ACL_FILTER;   
    stReq.p_lport = lport;
    stReq.para2_u.uiValue = uiRulePrecedence;
    stReq.para4_u.uiValue = uiAclRuleType;
    stReq.para5_u.uiValue = uiAclRuleDir;
    memcpy((void *)&stReq.para3_u, pRuleValue, sizeof(stReq.para3_u));
    
    rev = Drv_IoctlCmd(DEV_SWTICH,&stReq);

    return rev;
}


DRV_RET_E Ioctl_DelAclFilterByPort(UINT32 lport, UINT32 uiRulePrecedence, UINT32 uiAclRuleDir)
{
    
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    
    stReq.cmd = DRV_CMD_DEL_ACL_FILTER_BYPORT;   
    stReq.p_lport = lport;
    stReq.para2_u.uiValue = uiRulePrecedence;
    stReq.para3_u.uiValue = uiAclRuleDir;
    
    rev = Drv_IoctlCmd(DEV_SWTICH,&stReq);

    return rev;
}

DRV_RET_E Ioctl_AddCtcClfByPort(UINT32 lport, 
                             UINT32 uiRulePrecedence,
                             UINT32 uiAclRuleType,
                             VOID *pRuleValue,
                             UINT32 uiRemarkPri)
{
    
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    if(pRuleValue == NULL)
    {
        return DRV_ERR_PARA;
    }
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    
    stReq.cmd = DRV_CMD_ADD_CTC_CLF_BYPORT;   
    stReq.p_lport = lport;
    //memcpy(&(stReq.para2_u.uiValue), pRuleValue, sizeof(stReq.para2_u));
    memcpy(&(stReq.para2_u), pRuleValue, sizeof(stReq.para2_u));
    stReq.para3_u.uiValue = uiRemarkPri;
    stReq.para4_u.uiValue = uiRulePrecedence;
    stReq.para5_u.uiValue = uiAclRuleType;

    rev = Drv_IoctlCmd(DEV_SWTICH,&stReq);

    return rev;
}

DRV_RET_E Ioctl_DelCtcClfByPrec(UINT32 lport, UINT32 uiRulePrecedence)
{
    
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    
    stReq.cmd = DRV_CMD_DEL_CTC_CLF_BYPREC;   
    stReq.p_lport = lport;
    stReq.para2_u.uiValue = uiRulePrecedence;
    
    rev = Drv_IoctlCmd(DEV_SWTICH,&stReq);

    return rev;
}

DRV_RET_E Ioctl_ClearCtcClfByPort(UINT32 lport)
{
    
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    
    stReq.cmd = DRV_CMD_CLEAR_CTC_CLF_BYPORT;   
    stReq.p_lport = lport;

    rev = Drv_IoctlCmd(DEV_SWTICH,&stReq);

    return rev;
}

DRV_RET_E Ioctl_AddCtcClfPriToQueueByPort(UINT32 lport, 
                             UINT32 uiRulePrecedence,
                             UINT32 uiRemarkPri)
{
    
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    
    stReq.cmd = DRV_CMD_ADD_CTC_CLF_PRI_TO_QUEUE_BYPORT;   
    stReq.p_lport = lport;
	stReq.para2_u.uiValue = uiRulePrecedence;
    stReq.para3_u.uiValue = uiRemarkPri;    

    rev = Drv_IoctlCmd(DEV_SWTICH,&stReq);

    return rev;
}

DRV_RET_E Ioctl_DelCtcClfPriToQueueByPrec(UINT32 lport, UINT32 uiRulePrecedence)
{
    
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    
    stReq.cmd = DRV_CMD_DEL_CTC_CLF_PRI_TO_QUEUE_BYPREC;   
    stReq.p_lport = lport;
    stReq.para2_u.uiValue = uiRulePrecedence;
    
    rev = Drv_IoctlCmd(DEV_SWTICH,&stReq);

    return rev;
}

DRV_RET_E Ioctl_ClearCtcClfPriToQueueByPort(UINT32 lport)
{
    
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    
    stReq.cmd = DRV_CMD_CLEAR_CTC_CLF_PRI_TO_QUEUE_BYPORT;   
    stReq.p_lport = lport;

    rev = Drv_IoctlCmd(DEV_SWTICH,&stReq);

    return rev;
}

/*begin added by liaohongjun 2012/11/30 of EPN104QID0084*/
DRV_RET_E Ioctl_AclDropLoopdCreateAddPort(UINT32 lport)
{
    
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    
    stReq.cmd = DRV_CMD_ACL_DROP_LOOPD_CREATE_ADD_PORT;   
    stReq.p_lport = lport;
    rev = Drv_IoctlCmd(DEV_SWTICH,&stReq);

    return rev;
}

DRV_RET_E Ioctl_AclDropLoopdCreateRemovePort(UINT32 lport)
{
    
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    
    stReq.cmd = DRV_CMD_ACL_DROP_LOOPD_CREATE_REMOVE_PORT;   
    stReq.p_lport = lport;
    rev = Drv_IoctlCmd(DEV_SWTICH,&stReq);

    return rev;
}
DRV_RET_E Ioctl_AclDropLoopdDelete(VOID)
{
    
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    
    stReq.cmd = DRV_CMD_ACL_DROP_LOOPD_DELETE;   
    rev = Drv_IoctlCmd(DEV_SWTICH,&stReq);

    return rev;
}
/*end added by liaohongjun 2012/11/30 of EPN104QID0084*/

DRV_RET_E Ioctl_GetPortMediaFiberMask(logic_pmask_t *pMask)
{
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    if(pMask == NULL)
    {
        return DRV_ERR_PARA;
    }
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    stReq.cmd = DRV_CMD_GET_MEDIA_FIBER_MASK;   
    
    rev = Drv_IoctlCmd(DEV_SWTICH, &stReq);

    if(DRV_OK == rev)
    {
        memcpy((VOID *)pMask,(CONST VOID *)(&stReq.p_mask),sizeof(logic_pmask_t));
    }
    return rev;
}

DRV_RET_E Ioctl_GetPortMask(DRV_CMD_E ioctlCmd, UINT32 uiValue, logic_pmask_t *pMask)
{
    DRV_REQ_S stReq;
    DRV_RET_E rev = DRV_OK;  
    
    if(pMask == NULL)
    {
        return DRV_ERR_PARA;
    }
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    stReq.cmd = ioctlCmd;
    stReq.para1_u.uiValue = uiValue;
    
    rev = Drv_IoctlCmd(DEV_SWTICH, &stReq);

    if(DRV_OK == rev)
    {
        memcpy((VOID *)pMask,(CONST VOID *)(&stReq.p_mask),sizeof(logic_pmask_t));
    }
    return rev;
}

DRV_RET_E Ioctl_GetUnionUint32(DRV_CMD_E ioctlCmd, UINT32* puiValue)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    if(puiValue == NULL)
    {
        return DRV_ERR_PARA;
    }
    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;

    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    if(DRV_OK == rv)
    {
        *puiValue = stDrv.para1_u.uiValue;
    }
    return rv;
}
    
DRV_RET_E Ioctl_SetUnionUint32(DRV_CMD_E ioctlCmd, UINT32 uiValue)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    stDrv.para1_u.uiValue = uiValue;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}


/*****************************************************************
Function:Ioctl_SetLaserRegUint32
Description:laser set
Author:huangmingjian
Date:2013/03/15
INPUT: ioctlCmd, regAddr,fieldOffset, fieldWidth and data0
OUTPUT:         
Return: DRV_OK or error

Others:	       
*****************************************************************/

DRV_RET_E Ioctl_SetLaserRegUint32(DRV_CMD_E ioctlCmd, UINT32 regAddr, UINT16 fieldOffset, UINT16 fieldWidth, UINT32 data0)
{
	DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = regAddr;
    stDrv.para2_u.uiValue = fieldOffset;
    stDrv.para3_u.uiValue = fieldWidth;
	stDrv.para4_u.uiValue = data0;
	
	rev = Drv_IoctlCmd(DEV_SWTICH,&stDrv);
	
    return rev;
}


/*****************************************************************
Function:Ioctl_GetLaserRegUint32
Description:laser get
Author:huangmingjian
Date:2013/03/15
INPUT: ioctlCmd, regAddr,fieldOffset, fieldWidth and data0
OUTPUT:         
Return: DRV_OK or error

Others:	       
*****************************************************************/

DRV_RET_E Ioctl_GetLaserRegUint32(DRV_CMD_E ioctlCmd, UINT32 regAddr, UINT16 fieldOffset, UINT16 fieldWidth, UINT32 *data0)
{
	DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = regAddr;
    stDrv.para2_u.uiValue = fieldOffset;
    stDrv.para3_u.uiValue = fieldWidth;
	
	rev = Drv_IoctlCmd(DEV_SWTICH,&stDrv);
	*data0 = stDrv.para4_u.uiValue;
	
    return rev;
}


/*****************************************************************
Function:Ioctl_SetLaserOnTest_Flag
Description:when set laser on test ,we set the flag to 1, else set to 0
Author:huangmingjian
Date:2013/03/19
INPUT:  data0
OUTPUT:         
Return: DRV_OK or error

Others:	       
*****************************************************************/

DRV_RET_E Ioctl_SetLaserOnTest_Flag(DRV_CMD_E ioctlCmd, UINT32 data0)
{
	DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = data0;
	
	rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);
	
    return rev;
}


/*****************************************************************
Function:Ioctl_SetLaserOnTest_Flag
Description:get laser on test flag
Author:huangmingjian
Date:2013/03/19
INPUT:  data0
OUTPUT:         
Return: DRV_OK or error

Others:	       
*****************************************************************/

DRV_RET_E Ioctl_GetLaserOnTest_Flag(DRV_CMD_E ioctlCmd, UINT32 *data0)
{
	DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
	
	rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);
	*data0 = stDrv.para1_u.uiValue;
	
    return rev;
}


DRV_RET_E Ioctl_SetUnionUint32Uint32(DRV_CMD_E ioctlCmd, UINT32 uiValue, UINT32 uiValue2)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    stDrv.para1_u.uiValue = uiValue;
    stDrv.para2_u.uiValue = uiValue2;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}

DRV_RET_E Ioctl_SetUnionUint32Uint32Uint32(DRV_CMD_E ioctlCmd, UINT32 uiValue, UINT32 uiValue2, UINT32 uiValue3)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    stDrv.para1_u.uiValue = uiValue;
    stDrv.para2_u.uiValue = uiValue2;
    stDrv.para3_u.uiValue = uiValue3;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}


DRV_RET_E Ioctl_GetUnionUint32ByUint32(DRV_CMD_E ioctlCmd, UINT32 uiValue, UINT32 *uiValue2)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = uiValue;

    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        *uiValue2 = stDrv.para2_u.uiValue;
    }

    return rev;
}

/*  */
DRV_RET_E Ioctl_GetUnionUint32ByUint32Uint32(DRV_CMD_E ioctlCmd, UINT32 uiValue,  UINT32 uiValue2, UINT32 *uiValue3)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = uiValue;
    stDrv.para2_u.uiValue = uiValue2;

    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        *uiValue3 = stDrv.para3_u.uiValue;
    }

    return rev;
}

DRV_RET_E Ioctl_SetUnionUint32Uint32Uint32Uint32(DRV_CMD_E ioctlCmd, UINT32 uiValue, UINT32 uiValue2, UINT32 uiValue3, UINT32 uiValue4)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    stDrv.para1_u.uiValue = uiValue;
    stDrv.para2_u.uiValue = uiValue2;
    stDrv.para3_u.uiValue = uiValue3;
    stDrv.para4_u.uiValue = uiValue4;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}

DRV_RET_E Ioctl_GetUnionUint32ByUint32Uint32Uint32(DRV_CMD_E ioctlCmd, UINT32 uiValue,  UINT32 uiValue2, UINT32 uiValue3, UINT32 *uiValue4)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = uiValue;
    stDrv.para2_u.uiValue = uiValue2;
    stDrv.para3_u.uiValue = uiValue3;

    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        *uiValue4 = stDrv.para4_u.uiValue;
    }

    return rev;
}
/* 014334 */

/*  */
DRV_RET_E Ioctl_GetUnionFdbEntryByIndex(DRV_CMD_E ioctlCmd, UINT32 uiIndex,  UINT32 *puiFid, mac_common_t *pstFdbEntry)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    if ((NULL == puiFid) ||
        (NULL == pstFdbEntry))
    {
        return DRV_ERR_PARA;
    }
    
    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = uiIndex;
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        *puiFid = stDrv.para3_u.uiValue;
        memcpy(pstFdbEntry,&(stDrv.para2_u.stMacCommon),sizeof(mac_common_t));
    }

    return rev;
}

DRV_RET_E Ioctl_GetUnionUint32Uint32Uint32ByUint32(DRV_CMD_E ioctlCmd, UINT32 uiValue,  UINT32 *puiValue1, UINT32 *puiValue2, UINT32 *puiValue3)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    if ((NULL == puiValue1) ||
        (NULL == puiValue2) ||
        (NULL == puiValue3))
    {
        return DRV_ERR_PARA;
    }
    
    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = uiValue;
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        *puiValue1 = stDrv.para2_u.uiValue;
        *puiValue2 = stDrv.para3_u.uiValue;
        *puiValue3 = stDrv.para4_u.uiValue;
    }

    return rev;
}
/* 014528 */

DRV_RET_E Ioctl_SetUnionUint32Lpmask(DRV_CMD_E ioctlCmd, UINT32 uiValue, logic_pmask_t stLpMask)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    stDrv.para1_u.uiValue = uiValue;
    LgcMaskCopy(&(stDrv.para2_u.lgcMask), &stLpMask);
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}


DRV_RET_E Ioctl_GetUnionUint32MaskMask(DRV_CMD_E ioctlCmd, UINT32 uiValue, logic_pmask_t *pMask, logic_pmask_t *pMask2)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    stDrv.para1_u.uiValue = uiValue;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);
    if(DRV_OK == rv)
    {
        memcpy((VOID *)pMask,(CONST VOID *)&(stDrv.para2_u.lgcMask),sizeof(logic_pmask_t));
        memcpy((VOID *)pMask2,(CONST VOID *)&(stDrv.para3_u.lgcMask),sizeof(logic_pmask_t));
    }

    return rv;
}

DRV_RET_E Ioctl_SetUnionUint32LpmaskLpmask(DRV_CMD_E ioctlCmd, UINT32 uiValue, logic_pmask_t stLpMask, logic_pmask_t stLpMask2)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    stDrv.para1_u.uiValue = uiValue;
    LgcMaskCopy(&(stDrv.para2_u.lgcMask), &stLpMask);
    LgcMaskCopy(&(stDrv.para3_u.lgcMask), &stLpMask2);
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}

DRV_RET_E Ioctl_SetUnionLportLpmaskLpmask(DRV_CMD_E ioctlCmd, port_num_t lport, logic_pmask_t stLpMask, logic_pmask_t stLpMask2)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    stDrv.para1_u.lgcPort = lport;
    LgcMaskCopy(&(stDrv.para2_u.lgcMask), &stLpMask);
    LgcMaskCopy(&(stDrv.para3_u.lgcMask), &stLpMask2);
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}


DRV_RET_E Ioctl_SetUnionUint32MacAdd(DRV_CMD_E ioctlCmd, UINT32 uiValue, mac_address_t mac_address)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    stDrv.para1_u.uiValue = uiValue;
    memcpy(stDrv.para2_u.mac_address, mac_address, sizeof(mac_address_t));
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}

DRV_RET_E Ioctl_SetUnionMacDel(DRV_CMD_E ioctlCmd, mac_delete_t stMacDelete)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    memcpy(&(stDrv.para2_u.stMacDelete), &stMacDelete, sizeof(mac_delete_t));
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}

DRV_RET_E Ioctl_SetUnionMacUcast(DRV_CMD_E ioctlCmd, mac_ucast_t stMacUcast)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    memcpy(&(stDrv.para2_u.stMacUcast), &stMacUcast, sizeof(mac_ucast_t));
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}

DRV_RET_E Ioctl_SetUnionMacMcast(DRV_CMD_E ioctlCmd, mac_mcast_t stMacMcast)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    memcpy(&(stDrv.para2_u.stMacMcast), &stMacMcast, sizeof(mac_mcast_t));
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}

DRV_RET_E Ioctl_GetUnionMacVidUcast(DRV_CMD_E ioctlCmd, UINT32 ulVid, mac_address_t mac_addr, mac_ucast_t *pstMacUcast)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = ulVid;
    memcpy(stDrv.para3_u.mac_address, mac_addr, sizeof(mac_address_t));

    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        memcpy(pstMacUcast, &(stDrv.para2_u.stMacUcast), sizeof(mac_ucast_t));
    }

    return rev;
}

DRV_RET_E Ioctl_GetUnionMacVidMcast(DRV_CMD_E ioctlCmd, UINT32 ulVid, mac_address_t mac_addr, mac_mcast_t *pstMacMcast)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = ulVid;
    memcpy(stDrv.para3_u.mac_address, mac_addr, sizeof(mac_address_t));

    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        memcpy(pstMacMcast, &(stDrv.para2_u.stMacMcast), sizeof(mac_mcast_t));
    }

    return rev;
}

DRV_RET_E Ioctl_GetValidMacByIndex(DRV_CMD_E ioctlCmd, UINT32 ulIdx,  mac_common_t *pstTmpMac)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = ulIdx;
    
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        memcpy(pstTmpMac, &(stDrv.para2_u.stMacCommon), sizeof(mac_common_t));
    }

    return rev;
}

DRV_RET_E Ioctl_GetUnionMacMoreByIndex(DRV_CMD_E ioctlCmd, UINT32 ulIdx, 
    UINT32 uiCount, ULONG ulPointer, UINT32 *puiRealCount, UINT32 *puiNextIdx)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = ulIdx;
    stDrv.para3_u.uiValue = uiCount;
    stDrv.para2_u.ulValue = ulPointer;

    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);
    if(DRV_OK == rev)
    {
        *puiRealCount = stDrv.para4_u.uiValue;
        *puiNextIdx = stDrv.para3_u.uiValue; /*Note:Use Input Vars*/
    }
    return rev;
}


DRV_RET_E Ioctl_GetUnionMacVidMacCommon(DRV_CMD_E ioctlCmd, UINT32 ulVid, mac_address_t mac_addr, mac_common_t *pstTmpMac)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = ulVid;
    memcpy(stDrv.para3_u.mac_address, mac_addr, sizeof(mac_address_t));

    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        memcpy(pstTmpMac, &(stDrv.para2_u.stMacCommon), sizeof(mac_common_t));
    }

    return rev;
}

DRV_RET_E Ioctl_GetConflictMac(DRV_CMD_E ioctlCmd, Mac_op_t stMacOp, Mac_op_t *pstMacOp)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    memcpy(&(stDrv.para2_u.stMacOp), &stMacOp, sizeof(Mac_op_t));

    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        memcpy(pstMacOp, &(stDrv.para3_u.stMacOp), sizeof(Mac_op_t));
    }

    return rev;
}


DRV_RET_E Ioctl_GetUnionUint32ByUlong(DRV_CMD_E ioctlCmd, ULONG ulValue, UINT32 *uiValue)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para2_u.ulValue = ulValue;

    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        *uiValue = stDrv.para3_u.uiValue;
    }

    return rev;
}

DRV_RET_E Ioctl_GetUnionUint32Uint32Uint32ByUlong(DRV_CMD_E ioctlCmd, port_num_t lport, ULONG ulValue, UINT32 uiValue1, UINT32 uiValue2)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para2_u.ulValue = ulValue;
    stDrv.para1_u.lgcPort = lport;
    stDrv.para3_u.uiValue = uiValue1;
    stDrv.para4_u.uiValue = uiValue2;
    
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    return rev;
}

DRV_RET_E Ioctl_GetMIbCountByPort(DRV_CMD_E ioctlCmd, port_num_t lport, MIB_IfStatType_t mibCountType, UINT64 *ui64Value)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.lgcPort = lport;
    stDrv.para2_u.mibCountType = mibCountType;

    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        *ui64Value = stDrv.para3_u.ui64Value;
    }

    return rev;
}

DRV_RET_E Ioctl_GetPortCounter(DRV_CMD_E ioctlCmd, port_num_t lport, CounterName_E portCountType, UINT64 *ui64Value)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.lgcPort = lport;
    stDrv.para2_u.portCounterType = portCountType;

    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        *ui64Value = stDrv.para3_u.ui64Value;
    }

    return rev;
}

DRV_RET_E Ioctl_GetPortStatistics(DRV_CMD_E ioctlCmd, port_num_t lport, stat_reg_t portStatisticsType, UINT64 *ui64Value)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.lgcPort = lport;
    stDrv.para2_u.portStatisticsType = portStatisticsType;

    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        *ui64Value = stDrv.para3_u.ui64Value;
    }

    return rev;
}

/*Begin add by huangmingjian 2013-08-27*/

DRV_RET_E Ioctl_GetEponMibCounter(DRV_CMD_E ioctlCmd, rtk_epon_counter_app_t *Counter)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para2_u.Counter = Counter;
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    return rev;
}

DRV_RET_E Ioctl_Epon_Mib_Global_Reset(DRV_CMD_E ioctlCmd)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    return rev;
}

DRV_RET_E Ioctl_Get_Ponmac_Transceiver(DRV_CMD_E ioctlCmd, rtk_transceiver_parameter_type_app_t para_type, rtk_transceiver_data_app_t *pData)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
	stDrv.para1_u.uiValue  = para_type;
	stDrv.para2_u.pData  = pData;
	
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    return rev;
}

/*End add by huangmingjian 2013-08-27*/

/*Begin add by huangmingjian 2014-01-13*/
#if defined(CONFIG_BOSA)
DRV_RET_E Ioctl_Get_Ponftoverflag(DRV_CMD_E ioctlCmd, UINT32 *FTOverFLAG)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rev = DRV_ERR_PARA;

    memset(&stDrv, 0, sizeof(stDrv));  
    stDrv.cmd = ioctlCmd;
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);
    if(DRV_OK == rev)
    {
		*FTOverFLAG = stDrv.para1_u.uiValue;
    }
	
    return rev;
}


DRV_RET_E Ioctl_Set_Ponftoverflag(DRV_CMD_E ioctlCmd, UINT32 FTOverFLAG)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rev = DRV_ERR_PARA;

    memset(&stDrv, 0, sizeof(stDrv));  
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue  = FTOverFLAG;
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);
 
    return rev;
}

DRV_RET_E Ioctl_Set_Laser(DRV_CMD_E ioctlCmd, UINT32 uiAction)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rev = DRV_ERR_PARA;

    memset(&stDrv, 0, sizeof(stDrv));  
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue  = uiAction;
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);
 
    return rev;
}
DRV_RET_E Ioctl_Get_Laser(DRV_CMD_E ioctlCmd, UINT32 *uiAction)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rev = DRV_ERR_PARA;

    memset(&stDrv, 0, sizeof(stDrv));  
    stDrv.cmd = ioctlCmd;
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);
    if(DRV_OK == rev)
    {
		*uiAction = stDrv.para1_u.uiValue;
    }
	
    return rev;
}


DRV_RET_E Ioctl_Get_Apcset(DRV_CMD_E ioctlCmd, UINT32 *pApcVal)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rev = DRV_ERR_PARA;

    memset(&stDrv, 0, sizeof(stDrv));  
    stDrv.cmd = ioctlCmd;
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);
    if(DRV_OK == rev)
    {
		*pApcVal = stDrv.para1_u.uiValue;
    }
	
    return rev;
}


DRV_RET_E Ioctl_Set_Apcset(DRV_CMD_E ioctlCmd, UINT32 uiApcVal)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rev = DRV_ERR_PARA;

    memset(&stDrv, 0, sizeof(stDrv));  
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue  = uiApcVal;
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);
 
    return rev;
}

DRV_RET_E Ioctl_Get_Imodset(DRV_CMD_E ioctlCmd, UINT32 *pModVal)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rev = DRV_ERR_PARA;

    memset(&stDrv, 0, sizeof(stDrv));  
    stDrv.cmd = ioctlCmd;
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);
    if(DRV_OK == rev)
    {
		*pModVal = stDrv.para1_u.uiValue;
    }
	
    return rev;
}


DRV_RET_E Ioctl_Set_Imodset(DRV_CMD_E ioctlCmd, UINT32 uiModVal)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rev = DRV_ERR_PARA;

    memset(&stDrv, 0, sizeof(stDrv));  
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue  = uiModVal;
    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);
 
    return rev;
}
#endif
/*End add by huangmingjian 2014-01-13*/


DRV_RET_E Ioctl_SetPortRatelimit(DRV_CMD_E ioctlCmd, UINT32 direction, logic_pmask_t stLpMask, UINT32 uiRate)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    stDrv.para1_u.uiValue = direction;
    LgcMaskCopy(&(stDrv.para2_u.lgcMask), &stLpMask);
    stDrv.para3_u.uiValue = uiRate;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}

DRV_RET_E Ioctl_SetTrapCpu(DRV_CMD_E ioctlCmd, port_num_t lport, mac_trap_set_t stTrapSet, UINT32 priority)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    stDrv.para1_u.lgcPort = lport;
    memcpy(&(stDrv.para2_u.stTrapSet), &stTrapSet, sizeof(mac_trap_set_t));
    stDrv.para3_u.uiValue = priority;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}
DRV_RET_E Ioctl_SetRsvMacTrap(DRV_CMD_E ioctlCmd, UINT32 uiIndex, UINT32 priority, rsv_mac_pass_action_t action)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;

    stDrv.para1_u.uiValue = uiIndex; 
    stDrv.p_prio = priority;
    stDrv.para3_u.uiAction = action;
    rv = Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}


DRV_RET_E Ioctl_SetQos1pRemapCtl(DRV_CMD_E ioctlCmd, qos_8021p_to_intpri_t st1pToIntPri)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    memcpy(&(stDrv.para2_u.st1pToIntPri), &st1pToIntPri, sizeof(qos_8021p_to_intpri_t));
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}

DRV_RET_E Ioctl_SetQosDscpRemapCtl(DRV_CMD_E ioctlCmd, qos_dscp_to_intpri_t stDscpToIntPri)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    memcpy(&(stDrv.para2_u.stDscpToIntPri), &stDscpToIntPri, sizeof(qos_dscp_to_intpri_t));
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}


DRV_RET_E Ioctl_SetQosQueueSchedulCtl(DRV_CMD_E ioctlCmd, QueueMode_S stQueueSchedule)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    memcpy(&(stDrv.para2_u.stQueueSchedule), &stQueueSchedule, sizeof(QueueMode_S));
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}

DRV_RET_E Ioctl_InitCmboPortConfig(DRV_CMD_E ioctlCmd, port_num_t lport, COMBO_PORT_CONFIG_S * pstCfg)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv = DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd = ioctlCmd;
    stDrv.p_lport = lport;
    memcpy(&(stDrv.para2_u.stComboCfg), pstCfg, sizeof(COMBO_PORT_CONFIG_S));
    rv = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    return rv;
}
#ifdef UT_DRV_TEST
#undef Drv_IoctlCmd(_drv, _req)
#endif

DRV_RET_E Ioctl_GetCableDiag(DRV_CMD_E ioctlCmd, port_num_t lport, cable_diag_t *pstCableDiag)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    if(pstCableDiag == NULL)
    {
        return DRV_ERR_PARA;
    }
    
    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    stDrv.para1_u.lgcPort = lport;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);
    if(rv == DRV_OK)
    {
        memcpy(pstCableDiag, &(stDrv.para2_u.stCableDiag), sizeof(cable_diag_t));
    }
    return rv;
}


DRV_RET_E Ioctl_GetReg(DRV_CMD_E ioctlCmd, UINT32 uiValue, UINT32 uiValue2, UINT32 *uiValue3)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = uiValue;
    stDrv.para2_u.uiValue = uiValue2;

    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);

    if(DRV_OK == rev)
    {
        *uiValue3 = stDrv.para3_u.uiValue;
    }

    return rev;
}

DRV_RET_E Ioctl_SetReg(DRV_CMD_E ioctlCmd, UINT32 uiValue, UINT32 uiValue2, UINT32 uiValue3)
{
    DRV_REQ_S stDrv;
    DRV_RET_E rev = DRV_OK;

    memset(&stDrv, 0, sizeof(stDrv));
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = uiValue;
    stDrv.para2_u.uiValue = uiValue2;
    stDrv.para3_u.uiValue = uiValue3;

    rev = Drv_IoctlCmd(DEV_SWTICH, &stDrv);
    return rev;
}

DRV_RET_E Ioctl_SetPortStormlimit(DRV_CMD_E ioctlCmd, STORM_CTLTYPE_E type, logic_pmask_t *pLgcPMask, STORM_CTLRATE_S *pstStorm)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    stDrv.para1_u.stormType = type;
    memcpy(&(stDrv.para2_u.lgcMask), pLgcPMask, sizeof(logic_pmask_t));
    memcpy(&(stDrv.para3_u.stStorm), pstStorm, sizeof(STORM_CTLRATE_S));
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}


/*****************************************************************************
    Func Name: Ioctl_CtcDealPortBaseVlanMem
                                               
*****************************************************************************/
DRV_RET_E Ioctl_CtcDealPortBaseVlanMem(DRV_CMD_E ioctlCmd, UINT32 uiVidOrId, logic_pmask_t *pLgcPMask)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));    
    stDrv.cmd=ioctlCmd;
    stDrv.para1_u.uiValue=uiVidOrId;
    memcpy(&(stDrv.para2_u.lgcMask), pLgcPMask, sizeof(logic_pmask_t));
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}

/*****************************************************************************
    Func Name: Ioctl_CtcDelPortAclRule
 
                                                                            
*****************************************************************************/
DRV_RET_E Ioctl_CtcDelPortAclRule(DRV_CMD_E ioctlCmd, UINT32 uiLPort)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));   
    stDrv.cmd=ioctlCmd;
    stDrv.p_lport=uiLPort;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}

/*****************************************************************************
    Func Name: Ioctl_CtcAddPortAclRuleForVlan
                                                        
*****************************************************************************/
DRV_RET_E Ioctl_CtcAddPortAclRuleForVlan(DRV_CMD_E ioctlCmd, UINT32 uiLPort, CTC_VLAN_CFG_S stCtcVlanCfg)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));    
    stDrv.cmd=ioctlCmd;
    stDrv.p_lport=uiLPort;
    stDrv.para2_u.pstCtcVlanCfg = &stCtcVlanCfg;
    //memset(&(stDrv.para2_u.stCtcVlanCfg), &stCtcVlanCfg, sizeof(stCtcVlanCfg));
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}
/*****************************************************************************
    Func Name: Ioctl_CtcPortIngrVlanFilter
                                                     
*****************************************************************************/
DRV_RET_E Ioctl_CtcPortIngrVlanFilter(DRV_CMD_E ioctlCmd, UINT32 uiLPort, BOOL bEnable)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd=ioctlCmd;
    stDrv.p_lport=uiLPort;
    stDrv.para2_u.uiValue = (UINT32)bEnable;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}

/*****************************************************************************
    Func Name: Ioctl_CtcPortIngrVlanRcvMode
                                                          
*****************************************************************************/
DRV_RET_E Ioctl_CtcPortIngrVlanRcvMode(DRV_CMD_E ioctlCmd, UINT32 uiLPort, PORT_INGRESS_MODE_E enIngressMode)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));    
    stDrv.cmd=ioctlCmd;
    stDrv.p_lport=uiLPort;
    stDrv.para2_u.uiValue = (UINT32)enIngressMode;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    return rv;
}

/*****************************************************************************
    Func Name: Ioctl_CtcGetPortVlanCfg
                                                              
*****************************************************************************/
DRV_RET_E Ioctl_CtcGetPortVlanCfg(DRV_CMD_E ioctlCmd, UINT32 uiLPort, CTC_VLAN_CFG_S *pstCtcVlanCfg)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;
    CTC_VLAN_CFG_S stCtcVlanCfgTmp;

    memset(&stDrv,0,sizeof(stDrv));
    memset(&stCtcVlanCfgTmp, 0, sizeof(stCtcVlanCfgTmp));
    stDrv.cmd=ioctlCmd;
    stDrv.p_lport=uiLPort;
    stDrv.para2_u.pstCtcVlanCfg = &stCtcVlanCfgTmp;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);

    if (DRV_OK == rv)
    {
        memcpy(pstCtcVlanCfg, &(stCtcVlanCfgTmp), sizeof(CTC_VLAN_CFG_S));
    }
    
    return rv;
}

/*****************************************************************************
    Func Name: Ioctl_CtcSetPortVlanCfg
                                                            
*****************************************************************************/
DRV_RET_E Ioctl_CtcSetPortVlanCfg(DRV_CMD_E ioctlCmd, UINT32 uiLPort, CTC_VLAN_CFG_S *pstCtcVlanCfg)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;
    CTC_VLAN_CFG_S stCtcVlanCfgTmp;

    memset(&stDrv,0,sizeof(stDrv));
    memset(&stCtcVlanCfgTmp, 0, sizeof(stCtcVlanCfgTmp));
    stDrv.cmd=ioctlCmd;
    stDrv.p_lport=uiLPort;
    stDrv.para2_u.pstCtcVlanCfg = pstCtcVlanCfg;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);
    
    return rv;
}

/*****************************************************************************
    Func Name: Ioctl_CtcCheckEnoughVlanEntry
 ----------------------------------------------------------
                                                                            
*****************************************************************************/
DRV_RET_E Ioctl_CtcCheckEnoughVlanEntry(DRV_CMD_E ioctlCmd, CTC_VLAN_CFG_S stCtcVlanCfg, UINT32 *puiEnough)
{
    DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));

    stDrv.cmd=ioctlCmd;
    stDrv.para2_u.pstCtcVlanCfg = &stCtcVlanCfg;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);
    if (DRV_OK == rv)
    {
        *puiEnough = stDrv.para1_u.uiValue;
    }
    
    return rv;
}
DRV_RET_E ioctl_ctcadds2centry(UINT32 uiLPort,UINT32 svlan,UINT32 cvlan)
{
	DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));

    stDrv.cmd=DRV_CMD_ADDS2C_BY_PORT;
 //   stDrv.para2_u.pstCtcVlanCfg = &stCtcVlanCfg;
 	stDrv.para1_u.lgcPort=uiLPort;
 	stDrv.para2_u.uiValue=svlan;
	stDrv.para3_u.uiValue=cvlan;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);
 
	return rv;
}
DRV_RET_E ioctl_ctcdels2centry(UINT32 uiLPort,UINT32 svlan)
{
	DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));

    stDrv.cmd=DRV_CMD_DELS2C_BY_PORT;
 //   stDrv.para2_u.pstCtcVlanCfg = &stCtcVlanCfg;
 	stDrv.para1_u.lgcPort=uiLPort;
 	stDrv.para2_u.uiValue=svlan;
//	stDrv.para3_u.uiValue=cvlan;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);
 
	return rv;
}

DRV_RET_E ioctl_ctcaddc2sentry(UINT32 uiLPort,UINT32 cvlan,UINT32 svlan)
{
	DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));

    stDrv.cmd=DRV_CMD_ADDC2S_BY_PORT;
 //   stDrv.para2_u.pstCtcVlanCfg = &stCtcVlanCfg;
 	stDrv.para1_u.lgcPort=uiLPort;
 	stDrv.para2_u.uiValue=cvlan;
	stDrv.para3_u.uiValue=svlan;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);
 
	return rv;
}

DRV_RET_E ioctl_ctcdelc2sentry(UINT32 uiLPort,UINT32 cvlan,UINT32 svlan)
{
	DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));

    stDrv.cmd=DRV_CMD_DELC2S_BY_PORT;
 //   stDrv.para2_u.pstCtcVlanCfg = &stCtcVlanCfg;
 	stDrv.para1_u.lgcPort=uiLPort;
 	stDrv.para2_u.uiValue=cvlan;
	stDrv.para3_u.uiValue=svlan;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);
 
	return rv;
}
DRV_RET_E ioctl_SetMcTransltVlanMemberAdd(UINT32 usrvlan, UINT32 mvlan, logic_pmask_t stLgcMask, logic_pmask_t stLgcMaskUntag,logic_pmask_t stLgcMasksvlanUntag)
{
	DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));

    stDrv.cmd=DRV_CMD_ADDTRSLMC_MBR;
 //   stDrv.para2_u.pstCtcVlanCfg = &stCtcVlanCfg;
 	stDrv.para1_u.uiValue=usrvlan;
 	stDrv.para4_u.uiValue=mvlan;
 	//stDrv.para2_u.uiValue=cvlan;
	memcpy(&(stDrv.para2_u.lgcMask),&stLgcMask,sizeof(logic_pmask_t));
	memcpy(&(stDrv.para3_u.lgcMask),&stLgcMaskUntag,sizeof(logic_pmask_t));
	memcpy(&(stDrv.para5_u.lgcMask),&stLgcMasksvlanUntag,sizeof(logic_pmask_t));
	//	stDrv.para4_u.uiValue=mvlan;
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);
}
DRV_RET_E ioctl_SetMctransltVlanMemberRmv(UINT32 usrvlan,UINT32 mvlan, logic_pmask_t stLgcMask)
{
	DRV_REQ_S stDrv; 
    DRV_RET_E rv=DRV_ERR_PARA;

    memset(&stDrv,0,sizeof(stDrv));

    stDrv.cmd=DRV_CMD_DELTRSLMC_MBR;
 	stDrv.para1_u.uiValue=usrvlan;
 	stDrv.para2_u.uiValue=mvlan;
	memcpy(&(stDrv.para3_u.lgcMask),&stLgcMask,sizeof(logic_pmask_t));
    rv=Drv_IoctlCmd(DEV_SWTICH,&stDrv);
}

/*Begin add by huangmingjian 2013-09-24*/
DRV_RET_E Ioctl_SetLookupMissFloodPortMask(DRV_CMD_E ioctlCmd, UINT32 type, logic_pmask_t flood_portmask)
{
	DRV_REQ_S stDrv; 
	DRV_RET_E rv=DRV_ERR_PARA;
	
    memset(&stDrv,0,sizeof(stDrv));  
    stDrv.cmd = ioctlCmd;
    stDrv.para1_u.uiValue = type;
	memcpy(&(stDrv.para2_u.lgcMask), &flood_portmask, sizeof(logic_pmask_t));
	rv=Drv_IoctlCmd(DEV_SWTICH, &stDrv);

	return rv;
}
/*End add by huangmingjian 2013-09-24*/


#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */
