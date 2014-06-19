/*****************************************************************************
 
------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
  --------------------------------------------------------------------------
                                                                             
*****************************************************************************/
#ifdef  __cplusplus
extern "C"{
#endif
#include <lw_type.h>
#include <string.h>
#include <stdio.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"


/*****************************************************************************
    Func Name:  l2_send_by_port

  Description:  l2 send 
        Input:  
       Output: 
       Return:  DRV_OK
      Caution: 
------------------------------------------------------------------------------
  Modification History                                                      
  DATE        NAME             DESCRIPTION                                  
  --------------------------------------------------------------------------
                                                                            
*****************************************************************************/

DRV_RET_E l2_send_by_port(UCHAR *pMsg, UINT uiLen,l2_send_op *op)
{
    DRV_REQ_S stReq;
    if(NULL == pMsg||0==uiLen||NULL==op)
    {
        return DRV_ERR_PARA;
    }
    memset(&stReq, 0x00, sizeof(DRV_REQ_S));
    stReq.cmd = DRV_CMD_L2_SEND;  
    stReq.pmsg = pMsg;
    stReq.p_msg_len = uiLen;
    memcpy(&stReq.l2_op,op,sizeof(l2_send_op));    
    return Drv_IoctlCmd(DEV_SWTICH, &stReq);
}

#ifdef  __cplusplus
}
#endif

