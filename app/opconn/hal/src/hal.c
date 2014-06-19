/*************************************************************************
*
*  COPYRIGHT (C) 2003-2009 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*   used only with Opulan semi-conductor products. 
*
* FILENAME:  hal.c
*
* DESCRIPTION: 
*   
*
* Date Created: Mar 10, 2009
*
* Authors(optional): Yang Chuang Raymond
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/hal/src/hal.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <hal.h>
#include <vos.h>
#include <oam.h>
#include <log.h>

#define MODULE MOD_DRIVERS

OPL_EVENT_FUNC eventFunc;

static int s_nHalInitialized = 0;

#define OPL_MAP_LENGTH   (0x30000)

#define HAL_CHECK_RETURN_NO_VAL if (!s_nHalInitialized) return
#define HAL_CHECK_RETURN if (!s_nHalInitialized) return (ERROR)


static VOS_MUTEX_t s_pstAppLock;
static VOS_MUTEX_t s_pstDrvLock;

#define DEVICE_APP_LOCK vosMutexTake(&s_pstAppLock)
#define DEVICE_APP_UNLOCK vosMutexGive(&s_pstAppLock)

#define DEVICE_DRV_LOCK vosMutexTake(&s_pstDrvLock)
#define DEVICE_DRV_UNLOCK vosMutexGive(&s_pstDrvLock)

static OPL_DEV_DRV_t s_pstDevDrv;
static OPL_DEV_APP_t s_pstDevApp;


extern OPL_RESULT halGBLInit();
extern OPL_RESULT halGBLDestroy();
extern OPL_RESULT halInterruptInit();
extern OPL_RESULT halInterruptDestroy();
extern OPL_RESULT halFdbInit();
extern OPL_RESULT halRstpInit();
extern OPL_RESULT halMcRegister();
extern void halVlanInit();
extern void halClsInit();
extern void halPortRegister();
extern void halTmInit();
#if defined(ONU_1PORT)||defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
extern void halOnuRegister();
#endif


OPL_RESULT  halInit()
{
	void *mmap_buff = OPL_NULL;

    if (s_nHalInitialized) 
        return NO_ERROR;

    if (VOS_MUTEX_OK != vosMutexCreate(&s_pstAppLock))
    {
        fprintf(stderr,"CRITICAL ERROR, Out of memory\r\n");
        return ERROR;
    }

    if (VOS_MUTEX_OK != vosMutexCreate(&s_pstDrvLock))
    {
        fprintf(stderr,"CRITICAL ERROR, Out of memory\r\n");
        return ERROR;
    }

    s_nHalInitialized = 1;

    /* initialize module */ 
    //dalDemonInit();
	opl_modules_init();
	eopl_dma_init();
    halInterruptInit(); 
	OamInit();   
//	EOAM_Init();
    halHostInit();
    halGBLInit();
    halTmInit(); 
    halFdbInit();  
  //  halRstpInit();            
    halVlanInit();           
    halClsInit();              
    halPortRegister();  
    halMcRegister();   
	HalStatsRegister();   
	HalOamRegister();   
#if defined(ONU_1PORT)||defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)||defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    halOnuRegister();  
#endif

    return NO_ERROR;
}

OPL_RESULT  halDestroy()
{
    HAL_CHECK_RETURN;

    s_nHalInitialized = 0;

    halGBLDestroy();
    halInterruptDestroy();

    vosMutexDestroy(&s_pstAppLock);
    //s_pstAppLock = NULL;
    vosMutexDestroy(&s_pstDrvLock);
    //s_pstDrvLock = NULL;

    return NO_ERROR;
}


OPL_RESULT halAppApiCallSync(OPL_API_DATA_t *pstData)
{

    OPL_API_FUNC pFunc;

    HAL_CHECK_RETURN;

    DEVICE_DRV_LOCK;    

    pFunc = s_pstDevDrv.arrayAPI[pstData->apiId].apiFunc;
    if ( NULL == pFunc )
    {
        fprintf(stderr,"CRITICAL ERROR, API table manipulation WRONG, apiId:%d\n",pstData->apiId);
        DEVICE_DRV_UNLOCK;
        return ERROR;
    }

	if (1 != pstData->apiId) {
		OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "call api %d, addr %p.\n", pstData->apiId, pFunc);
	}
    pFunc( pstData->param );

    DEVICE_DRV_UNLOCK;

    return NO_ERROR;
}

OPL_RESULT  halDrvApiRegister(OPL_API_UNIT_t *pstApiUnit)
{
    HAL_CHECK_RETURN;

    DEVICE_DRV_LOCK;
    vosMemCpy(&s_pstDevDrv.arrayAPI[pstApiUnit->apiId],pstApiUnit,sizeof(OPL_API_UNIT_t));
    DEVICE_DRV_UNLOCK;

    return NO_ERROR;
}

OPL_RESULT  halAppEventRegister(OPL_EVENT_UNIT_t *pstEventUint)
{
    HAL_CHECK_RETURN;

    DEVICE_APP_LOCK;
    vosMemCpy( &(s_pstDevApp.arrayEvent[pstEventUint->eventId]),pstEventUint,sizeof(OPL_EVENT_UNIT_t));
    DEVICE_APP_UNLOCK;

    return NO_ERROR;
}

OPL_RESULT  halAppEventIsExisted(OPL_EVENT_ID eventId)
{
    OPL_RESULT retVal = NO_ERROR;
    HAL_CHECK_RETURN;

    if (eventId >= HAL_EVENT_MAX)
        return ERROR;

    DEVICE_APP_LOCK;

    if (NULL == s_pstDevApp.arrayEvent[eventId].eventFunc)
        retVal = ERROR;

    DEVICE_APP_UNLOCK;

    return retVal;
}


static void *halEventThread(void *obj) 
{
    OPL_EVENT_DATA_t *pstData = obj;
    OPL_EVENT_FUNC pFunc;
    
    if (!s_nHalInitialized)
    {
        vosThreadExit(0);
        return NULL;
    }
    
    DEVICE_APP_LOCK;

    pFunc = s_pstDevApp.arrayEvent[pstData->eventId].eventFunc;

    if( ! pFunc )
    {
        fprintf(stderr,"CRITICAL ERROR, event table manipulation WRONG, eventId:%d\n",pstData->eventId);
        DEVICE_APP_UNLOCK;
        vosThreadExit(0);
        return NULL;
    }

    pFunc( pstData->param );

    vosFree(pstData->param);
    vosFree(pstData);

    DEVICE_APP_UNLOCK;

    vosThreadExit(0);

    return NULL;
}

OPL_RESULT  halDrvEventRaise(OPL_EVENT_DATA_t *pstData)
{

    VOS_THREAD_t pstThreadId;
    OPL_EVENT_DATA_t  *pstEventData;
  
    HAL_CHECK_RETURN;

    pstEventData = vosAlloc( sizeof( OPL_EVENT_DATA_t ));
    pstEventData->param = vosAlloc( pstData->length);

    pstEventData->eventId = pstData->eventId;
    pstEventData->eventFunc = pstData->eventFunc;
    pstEventData->length = pstData->length;
    vosMemCpy( pstEventData->param, pstData->param, pstData->length);
    
    pstThreadId = vosThreadCreate("tHalEvt", OPL_STACKSIZE, 60,
                        (void *)halEventThread, (void *)pstEventData);
    if (pstThreadId == NULL) {
        return ERROR;
    }

    return NO_ERROR;
}

static void *halApiThread(void *obj) 
{
    OPL_API_DATA_t *pstData = obj;
    OPL_API_FUNC pFunc;
    
    if (!s_nHalInitialized)
    {
        vosThreadExit(0);
        return NULL;
    }
    
    DEVICE_DRV_LOCK;

    pFunc = s_pstDevDrv.arrayAPI[pstData->apiId].apiFunc;

    if ( NULL == pFunc )
    {
        fprintf(stderr,"CRITICAL ERROR, event table manipulation WRONG, eventId:%d\n",pstData->apiId);
		DEVICE_DRV_UNLOCK;
        vosThreadExit(0);
        return NULL;
    }

    pFunc( pstData->param );
    
    vosFree(pstData->param);
    vosFree(pstData);

    DEVICE_DRV_UNLOCK;

    vosThreadExit(0);

    return NULL;
}


OPL_RESULT halAppApiCallAsync(OPL_API_DATA_t *pstData)
{
    VOS_THREAD_t pstThreadId;
    OPL_API_DATA_t  *pstApiData;

    HAL_CHECK_RETURN;

    pstApiData = vosAlloc( sizeof( OPL_API_DATA_t ));
    pstApiData->param = vosAlloc( pstData->length);

    pstApiData->apiId = pstData->apiId;
    pstApiData->length = pstData->length;
    vosMemCpy(pstApiData->param, pstData->param, pstData->length);
    
    pstThreadId = vosThreadCreate("tHalApiA", OPL_STACKSIZE, 70,
                        (void *)halApiThread, (void *)pstApiData);
    if (pstThreadId == NULL) {
        return ERROR;
    }

    return NO_ERROR;
}


/* test driver for hal */

void api_a(void *p1)
{
    printf("api async call:%d\n",*(int*)p1);
}
void api_s(void *p1)
{
    printf("api sync call:%d\n",*(int*)p1);
}
void event_a(void *p1)
{
    printf("eventa async call:%d\n",*(int*)p1);
}
void event_b(void *p1)
{
    printf("eventb async call:%d\n",*(int*)p1);
}

int main1() 
{
    OPL_API_UNIT_t  api_unit;
    OPL_EVENT_UNIT_t event_unit;
    OPL_EVENT_DATA_t    e_data;
    OPL_API_DATA_t  a_data; 
    int temp1;


    while(1)
    {
    
        api_unit.apiId = 30;
        api_unit.apiFunc = api_a;
        halDrvApiRegister( &api_unit );

        api_unit.apiId = 31;
        api_unit.apiFunc = api_s;
        halDrvApiRegister( &api_unit );


        event_unit.eventId = 20;
        event_unit.eventFunc = event_a;
        halAppEventRegister( &event_unit );

        event_unit.eventId = 21;
        event_unit.eventFunc = event_b;
        halAppEventRegister( &event_unit );


        for( temp1=0;temp1<20;temp1++ )
        {

            a_data.apiId = 30;  
            a_data.length = 4;
            a_data.param = &temp1;
            halAppApiCallSync( &a_data );
            a_data.apiId = 31;  
            a_data.length = 4;
            a_data.param = &temp1;
            halAppApiCallSync( &a_data );

        }
            
        for( temp1=0;temp1<20;temp1++ )
        {

            a_data.apiId = 30;  
            a_data.length = 4;
            a_data.param = &temp1;
            halAppApiCallAsync( &a_data );
            a_data.apiId = 31;  
            a_data.length = 4;
            a_data.param = &temp1;
            halAppApiCallAsync( &a_data );

        }

        for( temp1=0;temp1<20;temp1++ )
        {
            e_data.eventId = 20;    
            e_data.length = 4;
            e_data.param = &temp1;
            halDrvEventRaise( &e_data );
        
            e_data.eventId = 21;    
            e_data.length = 4;
            e_data.param = &temp1;
            halDrvEventRaise( &e_data );
        }
    }

}

