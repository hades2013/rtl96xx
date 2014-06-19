/*
=============================================================================
     File Name: hal_tm.c

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
#include <hal_tm.h>


#define HAL_TM_PRINTF(x)  printf x
#define HAL_TM_TRACE() HAL_TM_PRINTF(("%s,%s,%d\n",__FILE__,__FUNCTION__,__LINE__))

#define HAL_TM_EXIT(s) {ret=s;goto exit;}

/*******************************************************************************
*
* halTmQueueShaperSet:	 
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
OPL_STATUS halTmQueueShaperSet(void *p1)
{
    OPL_STATUS ret;
    HAL_TM_SHAPER_PAR_t *shaper=NULL;

    if (NULL == p1)
    {
        HAL_TM_TRACE();
        HAL_TM_EXIT(OPL_ERR_NULL_POINTER);
    }

    shaper = (HAL_TM_SHAPER_PAR_t *)p1;
    if (OPL_TRUE == shaper->enable)
    {
        ret =  tmQueueShaperEnableHwWrite(shaper->stream,shaper->queue,OPL_TRUE);
        ret += tmQueueCirHwWrite(shaper->stream,shaper->queue,shaper->cir);
        ret += tmQueueCbsHwWrite(shaper->stream,shaper->queue,shaper->cbs);
        
    }
    else if (OPL_FALSE == shaper->enable)
    {
        ret = tmQueueShaperEnableHwWrite(shaper->stream,shaper->queue,OPL_FALSE);
    }
    else
    {
        HAL_TM_TRACE();
        HAL_TM_EXIT(OPL_ERROR);
    }
    
 exit:
    shaper->ret = ret;
    return ;
}


/*******************************************************************************
*
* halTmQueueShaperGet:	 
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
void halTmQueueShaperGet(void *p1)
{
    OPL_STATUS ret;
    HAL_TM_SHAPER_PAR_t *shaper=NULL;

    UINT8 enable;

    if (NULL == p1)
    {
        HAL_TM_TRACE();
        HAL_TM_EXIT(OPL_ERR_NULL_POINTER);
    }

    shaper = (HAL_TM_SHAPER_PAR_t *)p1;

    ret = tmQueueShaperEnableHwRead(shaper->stream,shaper->queue,&enable);
    if (OPL_OK != ret)
    {
        HAL_TM_EXIT(OPL_OK);
    }
    
    if (OPL_TRUE == enable)
    {
      shaper->enable = OPL_TRUE;
      tmQueueCirHwRead(shaper->stream,shaper->queue,&shaper->cir);
      tmQueueCbsHwRead(shaper->stream,shaper->queue,&shaper->cbs);
        
    }    
    else
    {
        shaper->cir = 0;
        shaper->cbs = 0;
        shaper->enable = OPL_FALSE;
    }
    
exit:
    shaper->ret = ret;
    return ;

}


/*******************************************************************************
*
* halTmQueueWredSet:	 
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
void halTmQueueWredSet(void *p1)
{
    OPL_STATUS ret;
    HAL_TM_WRED_PAR_t *wred=NULL;

    if (NULL == p1)
    {
        HAL_TM_TRACE();
        HAL_TM_EXIT(OPL_ERR_NULL_POINTER);
    }

    wred = (HAL_TM_WRED_PAR_t *)p1;
    if (OPL_TRUE == wred->enable)
    {
        ret =  tmWredEnableHwWrite(wred->stream,wred->queue,OPL_TRUE);
        ret += tmWredCfgSet(wred->stream,wred->queue,US_WRED_MINTH,wred->minth);
        ret += tmWredCfgSet(wred->stream,wred->queue,US_WRED_MAXTH,wred->maxth);
        ret += tmWredCfgSet(wred->stream,wred->queue,US_WRED_WEIGHT,wred->weight);
        
    }
    else if (OPL_FALSE == wred->enable)
    {
        ret =  tmWredEnableHwWrite(wred->stream,wred->queue,OPL_FALSE);
    }
    else
    {
        HAL_TM_TRACE();
        HAL_TM_EXIT(OPL_ERROR);
    }
    
exit:
    wred->ret = ret;
    return ;

}

/*******************************************************************************
*
* halTmQueueWredGet:	 
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
void halTmQueueWredGet(void *p1)
{
    OPL_STATUS ret;
    HAL_TM_WRED_PAR_t *wred=NULL;

    UINT32 enable;

    if (NULL == p1)
    {
        HAL_TM_TRACE();
        HAL_TM_EXIT(OPL_ERR_NULL_POINTER);
    }

    wred = (HAL_TM_WRED_PAR_t *)p1;

    ret = tmWredEnableHwRead(wred->stream,wred->queue,&enable);
    if (OPL_TRUE == enable)
    {
        wred->enable = OPL_TRUE;
        tmWredCfgGet(wred->stream,wred->queue,US_WRED_MINTH,&wred->minth);
        tmWredCfgGet(wred->stream,wred->queue,US_WRED_MAXTH,&wred->maxth);
        tmWredCfgGet(wred->stream,wred->queue,US_WRED_WEIGHT,&wred->weight);
        
    }
    else
    {
        wred->enable = OPL_FALSE;
        wred->weight = 0;
        wred->maxth = 0;
        wred->minth = 0;
    }
    
 exit:
    wred->ret = ret;
    return ;
}
/*******************************************************************************
*
* halTmQueuePriSet:	 
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
void halTmQueuePriSet(void *p1)
{
    OPL_STATUS ret;
    HAL_TM_PRIORITY_PAR_t *pri=NULL;

    if (NULL == p1)
    {
        HAL_TM_TRACE();
        HAL_TM_EXIT(OPL_ERR_NULL_POINTER);
    }

    pri = (HAL_TM_PRIORITY_PAR_t *)p1;
    ret = tmQueuePriHwWrite(pri->stream,pri->queue,pri->pri);
exit:
    pri->ret = ret;
    return ;
}


/*******************************************************************************
*
* HalTmQueuePriGet:	 
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
void halTmQueuePriGet(void *p1)
{
    OPL_STATUS ret;
    HAL_TM_PRIORITY_PAR_t *pri=NULL;

    if (NULL == p1)
    {
        HAL_TM_TRACE();
        HAL_TM_EXIT(OPL_ERR_NULL_POINTER);
    }

    pri = (HAL_TM_PRIORITY_PAR_t *)p1;
    ret = tmQueuePriHwRead(pri->stream,pri->queue,&pri->pri);
exit:
    pri->ret = ret;
    return ;
}

/*******************************************************************************
*
* halTmQueueWeiSet:	 
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
void halTmQueueWeiSet(void *p1)
{
    OPL_STATUS ret;
    HAL_TM_WEIGHT_PAR_t *wei=NULL;

    if (NULL == p1)
    {
        HAL_TM_TRACE();
        HAL_TM_EXIT(OPL_ERR_NULL_POINTER);
    }

    wei = (HAL_TM_WEIGHT_PAR_t *)p1;
    ret = tmQueueWeightHwWrite(wei->stream,wei->queue,wei->weight);
exit:
    wei->ret = ret;
    return ;
}


/*******************************************************************************
*
* halTmQueueWeiGet:	 
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
void halTmQueueWeiGet(void *p1)
{
    OPL_STATUS ret;
    HAL_TM_WEIGHT_PAR_t *wei=NULL;

    if (NULL == p1)
    {
        HAL_TM_TRACE();
        HAL_TM_EXIT(OPL_ERR_NULL_POINTER);
    }

    wei = (HAL_TM_WEIGHT_PAR_t *)p1;
    ret = tmQueueWeightHwRead(wei->stream,wei->queue,&wei->weight);
exit:
    wei->ret = ret;
    return ;
}
void halTmInit(void )
{
    OPL_API_UNIT_t clsApiUnit;
    OPL_RESULT ulRet;
    
    /*register tm hal api*/
    clsApiUnit.apiId   = HAL_API_TM_QUEUE_SHAPER_SET;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halTmQueueShaperSet;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_TM_QUEUE_SHAPER_GET;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halTmQueueShaperGet;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_TM_QUEUE_WRED_SET;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halTmQueueWredSet;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_TM_QUEUE_WRED_GET;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halTmQueueWredGet;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_TM_QUEUE_PRIORITY_SET;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halTmQueuePriSet;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_TM_QUEUE_PRIORITY_GET;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halTmQueuePriGet;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_TM_QUEUE_WEIGHT_SET;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halTmQueueWeiSet;
    ulRet = halDrvApiRegister(&clsApiUnit);

    clsApiUnit.apiId   = HAL_API_TM_QUEUE_WEIGHT_GET;
    clsApiUnit.apiFunc = (OPL_API_FUNC)halTmQueueWeiGet;
    ulRet = halDrvApiRegister(&clsApiUnit);

    return ;

}

void halTmTest(UINT32 p1,UINT32 p2)
{
    OPL_API_DATA_t  a_data; 

    HAL_TM_PRIORITY_PAR_t pri;
    pri.queue = p1;
    pri.stream = p2;    

    a_data.apiId = HAL_API_TM_QUEUE_PRIORITY_GET;
    a_data.param = (void *)&pri;
    halAppApiCallSync(&a_data );
    printf("ret:%d pri:%d \n",pri.ret,pri.pri);
    
    return ;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif /*__cplusplus*/
#endif /*__cplusplus*/

