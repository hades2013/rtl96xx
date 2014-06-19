/*****************************************************************************

      File name:memshare.c
      Description:提供共享内存统一接口
      Author:liaohongjun
      Date:
              2012/11/30
==========================================================
      Note:file added by liaohongjun 2012/11/30 of EPN104QID0084
*****************************************************************************/

#ifdef  __cplusplus
    extern "C"{
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "lw_type.h"
#include "lw_drv_pub.h"
#include "lw_drv_req.h"
#include "memshare.h"

/*****************************************************************
    Function:Loopd_portCurState_shm_init
    Description: 环路检测端口状态共享内存初始化
    Author:liaohongjun
    Date:2012/11/29
    Input:     
    Output:         
    Return:
        NULL
        (PORT_LOOPD_STATE_S *)mem_addr    
=================================================
    Note:
*****************************************************************/
PORT_LOOPD_STATE_S *LoopdPortCurStateShmInit(void)

{
    INT shmid = 0;
    void *mem_addr = (void *)0;
    
    shmid = shmget((key_t)LOOPD_CUR_PORTSTATE_SHMID, sizeof(PORT_LOOPD_STATE_S), 0666|IPC_CREAT);
	if(shmid==-1)	
    {      
        MEMSH_DBG("shmget failed");
        return NULL;
    }   

    mem_addr = shmat(shmid,(VOID *)0,0);
	if((void *)-1 == mem_addr)
	{
	    MEMSH_DBG("shmat failed");
		return NULL;
	}  
    
    return (PORT_LOOPD_STATE_S *)mem_addr;
}

/*****************************************************************
    Function:CATV_WorkInfoShmInit
    Description: 光收工作状态信息
    Author:liaohongjun
    Date:2012/12/25
    Input:     
    Output:         
    Return:
        NULL
        (CATV_WORK_INFO_S *)mem_addr    
=================================================
    Note:added by liaohongjun 2012/12/25 of EPN204QID0019
*****************************************************************/
CATV_WORK_INFO_S *CATV_WorkInfoShmInit(void)

{
    INT shmid = 0;
    void *mem_addr = (void *)0;
    
    shmid = shmget((key_t)CATV_WORK_INFO_SHMID, sizeof(CATV_WORK_INFO_S), 0666|IPC_CREAT);
	if(shmid==-1)	
    {      
        MEMSH_DBG("shmget failed");
        return NULL;
    }   

    mem_addr = shmat(shmid,(VOID *)0,0);
	if((void *)-1 == mem_addr)
	{
	    MEMSH_DBG("shmat failed");
		return NULL;
	}  
    
    return (CATV_WORK_INFO_S *)mem_addr;
}

/*****************************************************************
    Function:snmp_data_shmInit
    Description: snmp data shm Init
    Author:huangmingjian
    Date:2012/12/31
    Input:     
    Output:         
    Return:
        NULL
        (PORT_LOOPD_STATE_S *)mem_addr    
=================================================
    Note:
*****************************************************************/

snmp_data_t *snmp_data_shmInit(void)
{
    INT shmid;
    port_num_t lgcPort = 0;
    void *mem_addr = (void *)0;
    shmid = shmget((key_t)SNMP_SHM_LABEL, sizeof(snmp_data_t), 0666|IPC_CREAT);
	if(shmid==-1)	
    {      
        MEMSH_DBG("shmget failed");
        return NULL;
    }   
    mem_addr = shmat(shmid,(VOID *)0,0);
	if((void *)-1 == mem_addr)
	{
	    MEMSH_DBG("shmat failed");
		return NULL;
	}  
    return (snmp_data_t *)mem_addr;
}

#ifdef CONFIG_PRODUCT_5500

/*****************************************************************
    Function:ponStatus_webDis__shmInit
    Description: get pon status to display on web
    Author:huangmingjian
    Date:2013/08/22
    Input:     
    Output:         
    Return:
        NULL
        (ponStatus_webDis_t *)mem_addr    
=================================================
    Note:
*****************************************************************/

ponStatus_webDis_t *ponStatus_webDis_shmInit(void)
{
    INT shmid;
    port_num_t lgcPort = 0;
    void *mem_addr = (void *)0;
    shmid = shmget((key_t)PONSTATUS_SHM_LABEL, sizeof(ponStatus_webDis_t), 0666|IPC_CREAT);
	if(shmid==-1)	
    {      
        MEMSH_DBG("shmget failed");
        return NULL;
    }   
    mem_addr = shmat(shmid,(VOID *)0,0);
	if((void *)-1 == mem_addr)
	{
	    MEMSH_DBG("shmat failed");
		return NULL;
	}  
    return (ponStatus_webDis_t *)mem_addr;
}
#endif

#ifdef  __cplusplus
}
#endif

