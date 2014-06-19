#ifdef  __cplusplus
extern "C"{
#endif
#include <lw_type.h>
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#if defined(__KERNEL__)
#include <linux/kernel.h>
#include <linux/string.h>
#else
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>


LW_DRV_RET Drv_SetPortSupportFrameLen(port_num_t lport, UINT32 uiLength)
{
     if (!IsValidLgcPort(lport))
    {
        return DRV_INVALID_LPORT;
    }

    return Ioctl_SetPortFrameLen(lport, uiLength);
}

LW_DRV_RET Drv_PortSupportFrameLenInit(VOID)
{
#define MAX_FRAME_LENTH 1536
    port_num_t lport;

    LgcPortFor(lport)
    {
        (VOID)Drv_SetPortSupportFrameLen(lport, MAX_FRAME_LENTH);
    }

    return DRV_OK;
}


BOOL Drv_IsLinkUpPort(port_num_t lport)
{
    UINT32 link_up;

    if(DRV_OK == Ioctl_GetPortCurrentLink(lport, &link_up))
    {
        return (BOOL)link_up;
    }
    else
    {
        return FALSE;
    }

}
LW_DRV_RET Drv_GetLinkUpPorts(logic_pmask_t *pstPortMask)
{
    if(NULL == pstPortMask)
    {
        return DRV_ERR_PARA;
    }

    ClrLgcMaskAll(pstPortMask);

    return Ioctl_GetPortLinkMask(pstPortMask);

}


LW_DRV_RET Drv_PortInit(port_num_t lport)
{
    return DRV_OK;
}

LW_DRV_RET Drv_GetVlanMember(vlan_id_t tdVid, logic_pmask_t * pstPortMask)
{
	logic_pmask_t untagmask;
	Ioctl_GetVlanMember(tdVid,pstPortMask,&untagmask);
    return DRV_OK;
}









#endif

#ifdef  __cplusplus
}
#endif

